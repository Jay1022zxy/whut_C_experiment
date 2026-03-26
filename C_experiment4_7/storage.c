#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "card.h"
#include "billing.h"
#include "computer.h"
#include "record.h"
#include "storage.h"

#define DATA_FILE "data.bin"
#define DATA_VERSION 1

// 魔数和版本号用于校验数据文件的正确性,确保加载的数据格式和版本与当前程序兼容,避免出现数据解析错误或程序崩溃等问题
static const unsigned char DATA_MAGIC_BYTES[8] = {'C', 'D', 'B', 'I', 'N', '1', '\0', '\0'}; // 二进制文件标识,用于校验文件格式是否正确

// 不直接使用.h中的结构体定义,而是定义一些新的结构体来保存需要写入文件的纯数据字段
// 这样做的好处是可以避免直接将链表结构中的指针等非数据字段写入文件,从而简化了文件的读写操作,同时也提高了数据的安全性和兼容性

typedef struct FileHeader // 定义数据文件头结构体,用于保存文件标识、版本号、卡片数量和记录数量
{
    unsigned char magic[8];
    int version;
    int card_count;
    int record_count;
} FileHeader;

typedef struct CardData // 定义卡片数据结构体,只保存需要写入文件的纯数据字段
{
    char cardID[10];
    char code[50];
    double money;
    double used_money;
    int state;
    int use_count;
    char last_time[50];
} CardData;

typedef struct BillingData // 定义账单数据结构体,只保存需要写入文件的纯数据字段
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

typedef struct LoginData // 定义上机数据结构体,只保存需要写入文件的纯数据字段
{
    char cardID[10];
    double money;
    double used_money;
    int use_count;
    char login_time[50];
} LoginData;

typedef struct SettleData // 定义下机数据结构体,只保存需要写入文件的纯数据字段
{
    char cardID[10];
    double money;
    double used_money;
    int use_count;
    char settle_time[50];
    int nStatus;
} SettleData;

typedef struct RecordData // 定义消费记录数据结构体,卡号长度要和业务结构保持一致
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

static int write_block(FILE *file, const void *data, size_t size)   // 将一块内存按二进制写入文件,写入成功返回1,失败返回0
{
    return fwrite(data, size, 1, file) == 1;  // fwrite函数写入数据到文件,返回成功写入的元素数量,如果成功写入1个元素则返回1,否则返回0表示写入失败
}

static int read_block(FILE *file, void *data, size_t size)          // 从文件中按二进制读取一块数据,读取成功返回1,失败返回0
{
    return fread(data, size, 1, file) == 1;  // fread函数读取数据从文件,返回成功读取的元素数量,如果成功读取1个元素则返回1,否则返回0表示读取失败
}

static void free_cards(void)                   // 释放卡片链表的内存,并将头指针置为NULL,以便后续重新加载数据时使用
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

static void free_billings(void)               // 释放账单链表的内存,并将头指针置为NULL,以便后续重新加载数据时使用
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

static void free_logins(void)                 // 释放登录记录链表的内存,并将头指针置为NULL,以便后续重新加载数据时使用
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

static void free_settles(void)                 // 释放结算记录链表的内存,并将头指针置为NULL,以便后续重新加载数据时使用
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

static void free_records(void)                 // 释放记录链表的内存,并将头指针置为NULL,以便后续重新加载数据时使用
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

void initialize_data(void)               // 初始化数据,释放所有链表的内存,并将全局变量count和recordCount重置为0,以便后续重新加载数据时使用
{
    // 在加载数据之前先调用initialize_data()函数来释放之前加载的数据的内存,并将全局变量count和recordCount重置为0
    // 以确保加载新的数据时不会出现内存泄漏或数据混乱的情况
    free_cards();
    free_billings();
    free_logins();
    free_settles();
    free_records();
    count = 0;
    recordCount = 0;
}

// 将新的卡片节点添加到卡片链表的末尾,如果链表为空则将头指针指向新节点,否则找到链表的末尾并将新节点链接到末尾
static void append_card(Card *node)
{
    Card **tail = &card_head;               // 使用二级指针来简化链表末尾的添加操作,避免了需要单独处理链表为空的情况
    while (*tail != NULL)
    {
        tail = &((*tail)->next);            // 遍历到链表的末尾
    }
    *tail = node;                           // 将新节点链接到末尾
    node->next = NULL;                      // 确保新节点的下一个指针为NULL,以标志链表的结束
}

// 将新的账单节点添加到账单链表的末尾,如果链表为空则将头指针指向新节点,否则找到链表的末尾并将新节点链接到末尾
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

// 将新的登录记录节点添加到登录记录链表的末尾,如果链表为空则将头指针指向新节点,否则找到链表的末尾并将新节点链接到末尾
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

// 将新的结算记录节点添加到结算记录链表的末尾,如果链表为空则将头指针指向新节点,否则找到链表的末尾并将新节点链接到末尾
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

// 将新的记录节点添加到记录链表的末尾,如果链表为空则将头指针指向新节点,否则找到链表的末尾并将新节点链接到末尾
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

// 从文件中加载数据
int load_data(void) 
{
    // rb 模式以二进制只读方式打开同级目录下的数据文件,如果文件不存在则返回0表示加载失败,否则继续读取文件内容
    FILE *file = fopen(DATA_FILE, "rb");   // 以二进制只读方式打开同级目录下的数据文件,如果文件不存在则返回0表示加载失败,否则继续读取文件内容
    FileHeader header;  // 定义一个文件头结构体变量用于保存从文件中读取的标识、版本号、卡片数量和记录数量等信息
    int i;

    // 在加载数据之前先调用initialize_data()函数来释放之前加载的数据的内存,并将全局变量count和recordCount重置为0,
    // 以确保加载新的数据时不会出现内存泄漏或数据混乱的情况
    initialize_data();

    if (file == NULL)  // 如果文件不存在或无法打开,则返回0表示加载失败
    {
        return 0;
    }

    // 先读取文件头,检查二进制文件标识、版本号以及记录数量是否正确,如果出现任何错误则清空已加载的数据并返回0表示失败
    if (!read_block(file, &header, sizeof(FileHeader)) ||
        memcmp(header.magic, DATA_MAGIC_BYTES, sizeof(header.magic)) != 0 ||
        header.version != DATA_VERSION ||
        header.card_count < 0 || header.record_count < 0)
    {
        initialize_data();
        fclose(file);
        return 0;
    }

    count = header.card_count;          // 从文件头中获取卡片数量并保存到全局变量count中,以便后续加载数据时使用
    recordCount = header.record_count;  // 从文件头中获取记录数量并保存到全局变量recordCount中,以便后续加载数据时使用

    // 依次读取每张卡对应的卡片信息、账单信息、上机记录和结算记录,再恢复到对应的链表中
    for (i = 0; i < count; i++)
    {
        // 定义一些结构体变量用于保存从文件中读取的卡片数据、账单数据、上机数据和结算数据,以及一些指针变量用于创建新的链表节点
        CardData card_data;         
        BillingData billing_data;
        LoginData login_data;
        SettleData settle_data;
        // 为每条链表创建一个新的节点,并分配内存,如果内存分配失败则返回0表示加载失败
        Card *card_node = (Card *)calloc(1, sizeof(Card));
        Billing *billing_node = (Billing *)calloc(1, sizeof(Billing));
        Login *login_node = (Login *)calloc(1, sizeof(Login));
        Settle *settle_node = (Settle *)calloc(1, sizeof(Settle));

        // 如果在读取过程中出现任何错误,例如内存分配失败或读取失败等,则释放已分配的内存并清空已加载的数据,最后关闭文件并返回0表示加载失败
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

        // 将从文件中读取的卡片数据、账单数据、上机数据和结算数据复制到对应的链表节点中,以便后续在程序中使用这些数据
        // memcpy函数用于复制内存数据,第一个参数是目标地址,第二个参数是源地址,第三个参数是要复制的字节数
        // 这里使用sizeof运算符来确保复制的字节数与结构体字段的大小一致,避免出现内存越界或数据损坏等问题
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

        append_card(card_node);          // 将新的卡片节点添加到卡片链表的末尾
        append_billing(billing_node);    // 将新的账单节点添加到账单链表的末尾
        append_login(login_node);        // 将新的登录记录节点添加到登录记录链表的末尾
        append_settle(settle_node);      // 将新的结算记录节点添加到结算记录链表的末尾
    }

    // 最后继续读取消费记录,并恢复到记录链表中
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

        append_record(record_node);      // 将新的记录节点添加到记录链表的末尾
    }

    fclose(file);  // 关闭文件
    return 1;
}

// 将当前的数据保存到文件中,首先打开文件进行写入,然后按照预定的二进制格式将数据写入文件,最后关闭文件并返回1表示保存成功
// 如果在保存过程中出现任何错误,例如无法打开文件或写入失败等,则返回0表示保存失败
int save_data(void)
{
    FILE *file = fopen(DATA_FILE, "wb");   // 以二进制写入方式打开同级目录下的数据文件,如果文件不存在则创建新文件,如果无法打开则返回0表示保存失败
    
    // 定义一些指针变量用于遍历卡片链表、账单链表、登录记录链表、结算记录链表和记录链表
    Card *card_current = card_head;       
    Billing *billing_current = billing_head;
    Login *login_current = login_head;
    Settle *settle_current = settle_head;
    Record *record_current = record_head;
    FileHeader header;
    int card_total = 0;   // 定义一个变量card_total用于统计卡片数量,以便后续写入文件头时使用,初始值为0
    int record_total = 0; // 定义一个变量record_total用于统计记录数量,以便后续写入文件头时使用,初始值为0

    if (file == NULL)
    {
        return 0;
    }

    // 首先统计卡片数量和记录数量,二进制保存时卡片、账单、登录、结算四条链表应当保持一一对应
    // 这段保持了四条链表长度一致
    while (card_current != NULL && billing_current != NULL &&
           login_current != NULL && settle_current != NULL)
    {
        card_total++;  // 统计卡片数量,以便后续写入文件头时使用
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

    while (record_current != NULL)
    {
        record_total++; // 统计记录数量,以便后续写入文件头时使用
        record_current = record_current->next;
    }

    count = card_total;
    recordCount = record_total;

    // 准备文件头数据,包括二进制文件标识、版本号、卡片数量和记录数量等信息,以便后续写入文件时使用
    // memset函数用于将一块内存区域设置为指定的值,第一个参数是要设置的内存区域的起始地址,第二个参数是要设置的值,第三个参数是要设置的字节数
    memset(&header, 0, sizeof(FileHeader)); // 将文件头结构体变量初始化为0,以确保所有字段都有默认值,避免出现未初始化的字段导致的数据错误或安全问题
    memcpy(header.magic, DATA_MAGIC_BYTES, sizeof(header.magic)); // 将预定义的二进制文件标识复制到文件头结构体变量的magic字段中,以便后续加载数据时校验文件格式是否正确
    header.version = DATA_VERSION;
    // 将全局变量count和recordCount的值保存到文件头结构体变量的card_count字段和record_count字段中,以便后续加载数据时使用这些信息来正确解析和使用数据
    header.card_count = count;
    header.record_count = recordCount;  

    // 先将文件头写入文件,后续加载时先读取这里的标识和数量信息
    
    if (!write_block(file, &header, sizeof(FileHeader))) 
    {
        fclose(file);
        return 0;
    }

    // 将卡片链表、账单链表、登录记录链表和结算记录链表的数据按二进制顺序写入文件
    // 这里使用了四个指针变量分别遍历四条链表,并在每次循环中将当前节点的数据复制到对应的结构体变量中
    // 然后调用write_block函数将这些数据按二进制写入文件
    card_current = card_head;
    billing_current = billing_head;
    login_current = login_head;
    settle_current = settle_head;
    while (card_current != NULL && billing_current != NULL &&
           login_current != NULL && settle_current != NULL)
    {
        // 定义一些结构体变量用于保存从链表节点中复制的卡片数据、账单数据、上机数据和结算数据,以便后续按二进制写入文件时使用
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

        // 将从链表节点中复制的数据按二进制写入文件,如果写入失败则关闭文件并返回0表示保存失败
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

    // 最后将记录链表的数据也按二进制顺序写入文件,这样后续加载数据时能够正确解析和使用这些数据
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

    fclose(file);  // 关闭文件
    return 1;
}
