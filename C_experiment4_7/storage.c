#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "card.h"
#include "billing.h"
#include "computer.h"
#include "record.h"
#include "storage.h"
#include "admin_card.h"

#define DATA_FILE "data.bin"
#define DATA_VERSION 3

// 用固定魔数标记二进制文件，加载时先验证文件是不是本程序生成的。
static const unsigned char DATA_MAGIC_BYTES[8] = {'C', 'D', 'B', 'I', 'N', '1', '\0', '\0'};

// V1 是旧版文件头，只包含卡和消费记录数量。
typedef struct FileHeaderV1
{
    unsigned char magic[8];
    int version;
    int card_count;
    int record_count;
} FileHeaderV1;

// V2 在 V1 的基础上追加了管理员数量，用来兼容新的管理员链表持久化。
typedef struct FileHeaderV2
{
    unsigned char magic[8];
    int version;
    int card_count;
    int record_count;
    int admin_count;
} FileHeaderV2;

// 下面这些结构体只负责“落盘数据”，不包含 next 指针，
// 这样写文件时不会把链表指针一起写进去。
typedef struct CardData
{
    char cardID[10];
    char code[50];
    double money;
    double used_money;
    int state;
    int use_count;
    char last_time[50];
} CardData;

typedef struct BillingData
{
    char cardID[10];
    char code[50];
    double money;
    double amount_money;
    int state;
    int use_count;
    char last_time[50];
    int nStatus;
} BillingData;

typedef struct LoginData
{
    char cardID[10];
    double money;
    double used_money;
    int use_count;
    char login_time[50];
} LoginData;

typedef struct SettleData
{
    char cardID[10];
    double money;
    double used_money;
    int use_count;
    char settle_time[50];
    int nStatus;
} SettleData;

typedef struct RecordData
{
    char cardID[10];
    double amount;
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
} RecordData;

typedef struct AdminDataV2
{
    char account[50];
    char password[50];
    char name[50];
} AdminDataV2;

typedef struct AdminData
{
    char account[50];
    char password[50];
    char name[50];
    int permissions;
} AdminData;

// 按块写入数据，成功返回 1，失败返回 0。
static int write_block(FILE *file, const void *data, size_t size)
{
    return fwrite(data, size, 1, file) == 1;
}

// 按块读取数据，成功返回 1，失败返回 0。
static int read_block(FILE *file, void *data, size_t size)
{
    return fread(data, size, 1, file) == 1;
}

// 释放卡链表，供重新加载数据前清空旧内存使用。
static void free_cards(void)
{
    Card *current = card_head;

    while (current != NULL)
    {
        Card *next = current->next;
        free(current);
        current = next;
    }

    card_head = NULL;
}

// 释放账单链表。
static void free_billings(void)
{
    Billing *current = billing_head;

    while (current != NULL)
    {
        Billing *next = current->next;
        free(current);
        current = next;
    }

    billing_head = NULL;
}

// 释放上机记录链表。
static void free_logins(void)
{
    Login *current = login_head;

    while (current != NULL)
    {
        Login *next = current->next;
        free(current);
        current = next;
    }

    login_head = NULL;
}

// 释放下机结算链表。
static void free_settles(void)
{
    Settle *current = settle_head;

    while (current != NULL)
    {
        Settle *next = current->next;
        free(current);
        current = next;
    }

    settle_head = NULL;
}

// 释放消费记录链表。
static void free_records(void)
{
    Record *current = record_head;

    while (current != NULL)
    {
        Record *next = current->next;
        free(current);
        current = next;
    }

    record_head = NULL;
}

// 初始化运行时数据。
// 每次重新加载文件前都先清空所有链表，避免内存泄漏和脏数据残留。
void initialize_data(void)
{
    free_cards();
    free_billings();
    free_logins();
    free_settles();
    free_records();
    free_admin_list();
    count = 0;
    recordCount = 0;
}

// 下面几个 append_* 都是“尾插法”，把新节点接到对应链表末尾。
static void append_card(Card *node)
{
    Card **tail = &card_head;

    while (*tail != NULL)
    {
        tail = &((*tail)->next);
    }

    *tail = node;
    node->next = NULL;
}

static void append_billing(Billing *node)
{
    Billing **tail = &billing_head;

    while (*tail != NULL)
    {
        tail = &((*tail)->next);
    }

    *tail = node;
    node->next = NULL;
}

static void append_login(Login *node)
{
    Login **tail = &login_head;

    while (*tail != NULL)
    {
        tail = &((*tail)->next);
    }

    *tail = node;
    node->next = NULL;
}

static void append_settle(Settle *node)
{
    Settle **tail = &settle_head;

    while (*tail != NULL)
    {
        tail = &((*tail)->next);
    }

    *tail = node;
    node->next = NULL;
}

static void append_record(Record *node)
{
    Record **tail = &record_head;

    while (*tail != NULL)
    {
        tail = &((*tail)->next);
    }

    *tail = node;
    node->next = NULL;
}

// 从 data.bin 恢复程序数据。
// 读取顺序必须和 save_data() 的写入顺序保持完全一致。
int load_data(void)
{
    FILE *file = fopen(DATA_FILE, "rb");
    FileHeaderV1 header_v1;
    int admin_count = 0;
    int i;

    initialize_data();

    if (file == NULL)
    {
        return 0;
    }

    // 先读旧版公共头部。这样既能兼容 V1，也能继续读取 V2。
    if (!read_block(file, &header_v1, sizeof(FileHeaderV1)) ||
        memcmp(header_v1.magic, DATA_MAGIC_BYTES, sizeof(header_v1.magic)) != 0 ||
        header_v1.card_count < 0 || header_v1.record_count < 0 ||
        (header_v1.version != 1 && header_v1.version != 2 && header_v1.version != DATA_VERSION))
    {
        initialize_data();
        fclose(file);
        return 0;
    }

    // V2 文件会额外存管理员数量；V1 文件没有管理员数据。
    if (header_v1.version >= 2)
    {
        if (!read_block(file, &admin_count, sizeof(admin_count)) || admin_count < 0)
        {
            initialize_data();
            fclose(file);
            return 0;
        }
    }

    count = header_v1.card_count;
    recordCount = header_v1.record_count;

    // 先恢复卡、账单、上机、下机四条一一对应的链表。
    for (i = 0; i < count; i++)
    {
        CardData card_data;
        BillingData billing_data;
        LoginData login_data;
        SettleData settle_data;
        Card *card_node = (Card *)calloc(1, sizeof(Card));
        Billing *billing_node = (Billing *)calloc(1, sizeof(Billing));
        Login *login_node = (Login *)calloc(1, sizeof(Login));
        Settle *settle_node = (Settle *)calloc(1, sizeof(Settle));

        if (card_node == NULL || billing_node == NULL || login_node == NULL || settle_node == NULL ||
            !read_block(file, &card_data, sizeof(CardData)) ||
            !read_block(file, &billing_data, sizeof(BillingData)) ||
            !read_block(file, &login_data, sizeof(LoginData)) ||
            !read_block(file, &settle_data, sizeof(SettleData)))
        {
            free(card_node);
            free(billing_node);
            free(login_node);
            free(settle_node);
            initialize_data();
            fclose(file);
            return 0;
        }

        // 把纯数据结构还原成链表节点。
        memcpy(card_node->cardID, card_data.cardID, sizeof(card_node->cardID));
        memcpy(card_node->code, card_data.code, sizeof(card_node->code));
        card_node->money = card_data.money;
        card_node->used_money = card_data.used_money;
        card_node->state = card_data.state;
        card_node->use_count = card_data.use_count;
        memcpy(card_node->last_time, card_data.last_time, sizeof(card_node->last_time));

        memcpy(billing_node->cardID, billing_data.cardID, sizeof(billing_node->cardID));
        memcpy(billing_node->code, billing_data.code, sizeof(billing_node->code));
        billing_node->money = billing_data.money;
        billing_node->amount_money = billing_data.amount_money;
        billing_node->state = billing_data.state;
        billing_node->use_count = billing_data.use_count;
        memcpy(billing_node->last_time, billing_data.last_time, sizeof(billing_node->last_time));
        billing_node->nStatus = billing_data.nStatus;

        memcpy(login_node->cardID, login_data.cardID, sizeof(login_node->cardID));
        login_node->money = login_data.money;
        login_node->used_money = login_data.used_money;
        login_node->use_count = login_data.use_count;
        memcpy(login_node->login_time, login_data.login_time, sizeof(login_node->login_time));

        memcpy(settle_node->cardID, settle_data.cardID, sizeof(settle_node->cardID));
        settle_node->money = settle_data.money;
        settle_node->used_money = settle_data.used_money;
        settle_node->use_count = settle_data.use_count;
        memcpy(settle_node->settle_time, settle_data.settle_time, sizeof(settle_node->settle_time));
        settle_node->nStatus = settle_data.nStatus;

        append_card(card_node);
        append_billing(billing_node);
        append_login(login_node);
        append_settle(settle_node);
    }

    // 再恢复消费记录链表。
    for (i = 0; i < recordCount; i++)
    {
        RecordData record_data;
        Record *record_node = (Record *)calloc(1, sizeof(Record));

        if (record_node == NULL || !read_block(file, &record_data, sizeof(RecordData)))
        {
            free(record_node);
            initialize_data();
            fclose(file);
            return 0;
        }

        memcpy(record_node->cardID, record_data.cardID, sizeof(record_node->cardID));
        record_node->amount = record_data.amount;
        record_node->year = record_data.year;
        record_node->month = record_data.month;
        record_node->day = record_data.day;
        record_node->hour = record_data.hour;
        record_node->minute = record_data.minute;
        record_node->second = record_data.second;

        append_record(record_node);
    }

    // 最后恢复管理员链表。
    for (i = 0; i < admin_count; i++)
    {
        Admin *admin_node = (Admin *)calloc(1, sizeof(Admin));

        if (admin_node == NULL)
        {
            free(admin_node);
            initialize_data();
            fclose(file);
            return 0;
        }

        if (header_v1.version == 2)
        {
            AdminDataV2 admin_data_v2;

            if (!read_block(file, &admin_data_v2, sizeof(AdminDataV2)))
            {
                free(admin_node);
                initialize_data();
                fclose(file);
                return 0;
            }

            memcpy(admin_node->account, admin_data_v2.account, sizeof(admin_node->account));
            memcpy(admin_node->password, admin_data_v2.password, sizeof(admin_node->password));
            memcpy(admin_node->name, admin_data_v2.name, sizeof(admin_node->name));
            admin_node->permissions = ADMIN_PERMISSION_ALL;
        }
        else
        {
            AdminData admin_data;

            if (!read_block(file, &admin_data, sizeof(AdminData)))
            {
                free(admin_node);
                initialize_data();
                fclose(file);
                return 0;
            }

            memcpy(admin_node->account, admin_data.account, sizeof(admin_node->account));
            memcpy(admin_node->password, admin_data.password, sizeof(admin_node->password));
            memcpy(admin_node->name, admin_data.name, sizeof(admin_node->name));
            admin_node->permissions = normalize_admin_permissions(admin_data.permissions);

            if (admin_node->permissions == 0)
            {
                admin_node->permissions = ADMIN_PERMISSION_ALL;
            }
        }

        admin_node->next = NULL;

        append_admin(admin_node);
    }

    fclose(file);

    // 如果旧文件里没有管理员数据，就补一个默认管理员，保证系统可登录。
    ensure_default_admin();
    return 1;
}

// 把当前内存中的链表按固定顺序写回 data.bin。
int save_data(void)
{
    FILE *file = fopen(DATA_FILE, "wb");
    Card *card_current = card_head;
    Billing *billing_current = billing_head;
    Login *login_current = login_head;
    Settle *settle_current = settle_head;
    Record *record_current = record_head;
    Admin *admin_current = admin_head;
    FileHeaderV2 header;
    int card_total = 0;
    int record_total = 0;
    int admin_total = 0;

    if (file == NULL)
    {
        return 0;
    }

    // 卡、账单、上机、下机四条链表在设计上必须长度一致。
    // 这里先统计并顺便做一致性校验，防止写出损坏文件。
    while (card_current != NULL && billing_current != NULL &&
           login_current != NULL && settle_current != NULL)
    {
        card_total++;
        card_current = card_current->next;
        billing_current = billing_current->next;
        login_current = login_current->next;
        settle_current = settle_current->next;
    }

    if (card_current != NULL || billing_current != NULL ||
        login_current != NULL || settle_current != NULL)
    {
        fclose(file);
        return 0;
    }

    // 统计消费记录数量。
    while (record_current != NULL)
    {
        record_total++;
        record_current = record_current->next;
    }

    // 统计管理员数量。
    while (admin_current != NULL)
    {
        admin_total++;
        admin_current = admin_current->next;
    }

    count = card_total;
    recordCount = record_total;

    // 先写文件头，后面的读取就靠这里的版本号和数量信息来解析。
    memset(&header, 0, sizeof(FileHeaderV2));
    memcpy(header.magic, DATA_MAGIC_BYTES, sizeof(header.magic));
    header.version = DATA_VERSION;
    header.card_count = count;
    header.record_count = recordCount;
    header.admin_count = admin_total;

    if (!write_block(file, &header, sizeof(FileHeaderV2)))
    {
        fclose(file);
        return 0;
    }

    // 按“卡 -> 账单 -> 上机 -> 下机”的顺序依次写入。
    card_current = card_head;
    billing_current = billing_head;
    login_current = login_head;
    settle_current = settle_head;
    while (card_current != NULL && billing_current != NULL &&
           login_current != NULL && settle_current != NULL)
    {
        CardData card_data = {0};
        BillingData billing_data = {0};
        LoginData login_data = {0};
        SettleData settle_data = {0};

        memcpy(card_data.cardID, card_current->cardID, sizeof(card_data.cardID));
        memcpy(card_data.code, card_current->code, sizeof(card_data.code));
        card_data.money = card_current->money;
        card_data.used_money = card_current->used_money;
        card_data.state = card_current->state;
        card_data.use_count = card_current->use_count;
        memcpy(card_data.last_time, card_current->last_time, sizeof(card_data.last_time));

        memcpy(billing_data.cardID, billing_current->cardID, sizeof(billing_data.cardID));
        memcpy(billing_data.code, billing_current->code, sizeof(billing_data.code));
        billing_data.money = billing_current->money;
        billing_data.amount_money = billing_current->amount_money;
        billing_data.state = billing_current->state;
        billing_data.use_count = billing_current->use_count;
        memcpy(billing_data.last_time, billing_current->last_time, sizeof(billing_data.last_time));
        billing_data.nStatus = billing_current->nStatus;

        memcpy(login_data.cardID, login_current->cardID, sizeof(login_data.cardID));
        login_data.money = login_current->money;
        login_data.used_money = login_current->used_money;
        login_data.use_count = login_current->use_count;
        memcpy(login_data.login_time, login_current->login_time, sizeof(login_data.login_time));

        memcpy(settle_data.cardID, settle_current->cardID, sizeof(settle_data.cardID));
        settle_data.money = settle_current->money;
        settle_data.used_money = settle_current->used_money;
        settle_data.use_count = settle_current->use_count;
        memcpy(settle_data.settle_time, settle_current->settle_time, sizeof(settle_data.settle_time));
        settle_data.nStatus = settle_current->nStatus;

        if (!write_block(file, &card_data, sizeof(CardData)) ||
            !write_block(file, &billing_data, sizeof(BillingData)) ||
            !write_block(file, &login_data, sizeof(LoginData)) ||
            !write_block(file, &settle_data, sizeof(SettleData)))
        {
            fclose(file);
            return 0;
        }

        card_current = card_current->next;
        billing_current = billing_current->next;
        login_current = login_current->next;
        settle_current = settle_current->next;
    }

    // 继续写入消费记录。
    record_current = record_head;
    while (record_current != NULL)
    {
        RecordData record_data = {0};

        memcpy(record_data.cardID, record_current->cardID, sizeof(record_data.cardID));
        record_data.amount = record_current->amount;
        record_data.year = record_current->year;
        record_data.month = record_current->month;
        record_data.day = record_current->day;
        record_data.hour = record_current->hour;
        record_data.minute = record_current->minute;
        record_data.second = record_current->second;

        if (!write_block(file, &record_data, sizeof(RecordData)))
        {
            fclose(file);
            return 0;
        }

        record_current = record_current->next;
    }

    // 最后写入管理员链表。
    admin_current = admin_head;
    while (admin_current != NULL)
    {
        AdminData admin_data = {0};

        memcpy(admin_data.account, admin_current->account, sizeof(admin_data.account));
        memcpy(admin_data.password, admin_current->password, sizeof(admin_data.password));
        memcpy(admin_data.name, admin_current->name, sizeof(admin_data.name));
        admin_data.permissions = normalize_admin_permissions(admin_current->permissions);

        if (!write_block(file, &admin_data, sizeof(AdminData)))
        {
            fclose(file);
            return 0;
        }

        admin_current = admin_current->next;
    }

    fclose(file);
    return 1;
}
