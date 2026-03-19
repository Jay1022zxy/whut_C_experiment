#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "card.h"
#include "billing.h"
#include "computer.h"
#include "record.h"
#include "storage.h"

#define DATA_FILE "data.txt"

static const char *text_field(const char *value)              // 将空字符串转换为"-"以便保存到文件中
{
    return (value != NULL && value[0] != '\0') ? value : "-";
}

static void normalize_text_field(char *value)                 // 将从文件读取的"-"转换回空字符串,以便在程序中使用
{
    if (strcmp(value, "-") == 0)
    {
        value[0] = '\0';
    }
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
    while (*tail != NULL) tail = &((*tail)->next); // 遍历到链表的末尾
    *tail = node;                           // 将新节点链接到末尾
    node->next = NULL;                      // 确保新节点的下一个指针为NULL,以标志链表的结束
}

// 将新的账单节点添加到账单链表的末尾,如果链表为空则将头指针指向新节点,否则找到链表的末尾并将新节点链接到末尾
static void append_billing(Billing *node)   
{
    Billing **tail = &billing_head;
    while (*tail != NULL) tail = &((*tail)->next);
    *tail = node;
    node->next = NULL;
}

// 将新的登录记录节点添加到登录记录链表的末尾,如果链表为空则将头指针指向新节点,否则找到链表的末尾并将新节点链接到末尾
static void append_login(Login *node)
{
    Login **tail = &login_head;
    while (*tail != NULL) tail = &((*tail)->next);
    *tail = node;
    node->next = NULL;
}

// 将新的结算记录节点添加到结算记录链表的末尾,如果链表为空则将头指针指向新节点,否则找到链表的末尾并将新节点链接到末尾
static void append_settle(Settle *node)
{
    Settle **tail = &settle_head;
    while (*tail != NULL) tail = &((*tail)->next);
    *tail = node;
    node->next = NULL;
}

// 将新的记录节点添加到记录链表的末尾,如果链表为空则将头指针指向新节点,否则找到链表的末尾并将新节点链接到末尾
static void append_record(Record *node)
{
    Record **tail = &record_head;
    while (*tail != NULL) tail = &((*tail)->next);
    *tail = node;
    node->next = NULL;
}

// 从文件中加载数据
int load_data(void)
{
    FILE *file = fopen(DATA_FILE, "r");   // 打开数据文件进行读取,如果文件不存在则返回0表示加载失败,否则继续读取文件内容
    // 定义一个缓冲区用于读取文件中的标签,每行数据的开头部分,用来区分不同类型的数据,例如"COUNT"表示卡片数量,"CARD"表示卡片信息,
    // "BILLING"表示账单信息,"LOGIN"表示登录记录,"SETTLE"表示结算记录,"RECORD_COUNT"表示记录数量,"RECORD"表示记录信息等
    char tag[32];       

    // 在加载数据之前先调用initialize_data()函数来释放之前加载的数据的内存,并将全局变量count和recordCount重置为0,
    // 以确保加载新的数据时不会出现内存泄漏或数据混乱的情况
    initialize_data();     

    if (file == NULL)  // 如果文件不存在或无法打开,则返回0表示加载失败,否则继续读取文件内容
    {
        return 0;
    }

    // 使用fscanf函数按照预定的格式从文件中读取数据,根据不同的标签来区分不同类型的数据,
    // 并将读取的数据存储到相应的链表中,如果在读取过程中出现任何错误,例如格式不正确或内存分配失败等,
    // 则调用initialize_data()函数来释放已经加载的数据的内存,并返回0表示加载失败
    // [^|]表示匹配除|以外的任意字符,因此%31[^|]|表示读取最多31个非|字符并将其存储到tag数组中,然后跳过一个|字符,以便继续读取后续的数据
    while (fscanf(file, "%31[^|]|", tag) == 1)  
    {
        // 如果标签是"COUNT",则读取卡片数量并存储到全局变量count中,如果读取失败则调用initialize_data()函数来释放已经加载的数据的内存,并返回0表示加载失败
        if (strcmp(tag, "COUNT") == 0)    
        {
            if (fscanf(file, "%d\n", &count) != 1)
            {
                initialize_data();
                fclose(file);
                return 0;
            }
        }
        // 如果标签是"CARD",则读取卡片信息并存储到卡片链表中,如果读取失败或内存分配失败则调用initialize_data()函数来释放已经加载的数据的内存,并返回0表示加载失败
        else if (strcmp(tag, "CARD") == 0)
        {
            Card *node = (Card *)calloc(1, sizeof(Card));
            if (node == NULL ||
                fscanf(file, "%9[^|]|%49[^|]|%lf|%lf|%d|%d|%49[^\n]\n",
                       node->cardID, node->code, &node->money, &node->used_money,
                       &node->state, &node->use_count, node->last_time) != 7)
            {
                free(node);
                initialize_data();
                fclose(file);
                return 0;
            }
            normalize_text_field(node->last_time);  // 将从文件读取的last_time字段中的"-"转换回空字符串,以便在程序中使用
            append_card(node);                      // 将新的卡片节点添加到卡片链表的末尾
        }
        // 如果标签是"BILLING",则读取账单信息并存储到账单链表中,如果读取失败或内存分配失败则调用initialize_data()函数来释放已经加载的数据的内存,并返回0表示加载失败
        else if (strcmp(tag, "BILLING") == 0)
        {
            Billing *node = (Billing *)calloc(1, sizeof(Billing));
            if (node == NULL ||
                fscanf(file, "%9[^|]|%49[^|]|%lf|%lf|%d|%d|%49[^|]|%d\n",
                       node->cardID, node->code, &node->money, &node->amount_money,
                       &node->state, &node->use_count, node->last_time, &node->nStatus) != 8)
            {
                free(node);
                initialize_data();
                fclose(file);
                return 0;
            }
            normalize_text_field(node->last_time);
            append_billing(node);  // 将新的账单节点添加到账单链表的末尾
        }
        // 如果标签是"LOGIN",则读取登录记录信息并存储到登录记录链表中,如果读取失败或内存分配失败则调用initialize_data()函数来释放已经加载的数据的内存,并返回0表示加载失败
        else if (strcmp(tag, "LOGIN") == 0)
        {
            Login *node = (Login *)calloc(1, sizeof(Login));
            if (node == NULL ||
                fscanf(file, "%9[^|]|%lf|%lf|%d|%49[^\n]\n",
                       node->cardID, &node->money, &node->used_money,
                       &node->use_count, node->login_time) != 5)
            {
                free(node);
                initialize_data();
                fclose(file);
                return 0;
            }
            normalize_text_field(node->login_time);
            append_login(node);   // 将新的登录记录节点添加到登录记录链表的末尾
        }
        // 如果标签是"SETTLE",则读取结算记录信息并存储到结算记录链表中,如果读取失败或内存分配失败则调用initialize_data()函数来释放已经加载的数据的内存,并返回0表示加载失败
        else if (strcmp(tag, "SETTLE") == 0)
        {
            Settle *node = (Settle *)calloc(1, sizeof(Settle));
            if (node == NULL ||
                fscanf(file, "%9[^|]|%lf|%lf|%d|%49[^|]|%d\n",
                       node->cardID, &node->money, &node->used_money,
                       &node->use_count, node->settle_time, &node->nStatus) != 6)
            {
                free(node);
                initialize_data();
                fclose(file);
                return 0;
            }
            normalize_text_field(node->settle_time);
            append_settle(node);   // 将新的结算记录节点添加到结算记录链表的末尾
        }
        // 如果标签是"RECORD_COUNT",则读取记录数量并存储到全局变量recordCount中,如果读取失败则调用initialize_data()函数来释放已经加载的数据的内存,并返回0表示加载失败
        else if (strcmp(tag, "RECORD_COUNT") == 0)
        {
            if (fscanf(file, "%d\n", &recordCount) != 1)
            {
                initialize_data();
                fclose(file);
                return 0;
            }
        }
        // 如果标签是"RECORD",则读取记录信息并存储到记录链表中,如果读取失败或内存分配失败则调用initialize_data()函数来释放已经加载的数据的内存,并返回0表示加载失败
        else if (strcmp(tag, "RECORD") == 0)
        {
            Record *node = (Record *)calloc(1, sizeof(Record));
            if (node == NULL ||
                fscanf(file, "%5[^|]|%lf|%d|%d|%d|%d|%d|%d\n",
                       node->cardID, &node->amount, &node->year, &node->month,
                       &node->day, &node->hour, &node->minute, &node->second) != 8)
            {
                free(node);
                initialize_data();
                fclose(file);
                return 0;
            }
            append_record(node);   // 将新的记录节点添加到记录链表的末尾
        }
        else
        {
            initialize_data();
            fclose(file);
            return 0;
        }
    }

    fclose(file);  // 关闭文件
    return 1;
}

// 将当前的数据保存到文件中,首先打开文件进行写入,然后按照预定的格式将数据写入文件,最后关闭文件并返回1表示保存成功
// 如果在保存过程中出现任何错误,例如无法打开文件或写入失败等,则返回0表示保存失败
int save_data(void)
{
    FILE *file = fopen(DATA_FILE, "w");   // 打开数据文件进行写入,如果文件不存在则创建新文件,如果无法打开则返回0表示保存失败,否则继续写入数据
    // 定义一些指针变量用于遍历卡片链表、账单链表、登录记录链表、结算记录链表和记录链表,以及一些变量用于统计卡片数量和记录数量
    Card *card_current = card_head;
    Billing *billing_current = billing_head;
    Login *login_current = login_head;
    Settle *settle_current = settle_head;
    Record *record_current = record_head;

    int card_total = 0;
    int record_total = 0;

    if (file == NULL)
    {
        return 0;
    }

    // 首先统计卡片数量和记录数量,然后将卡片数量写入文件,接着遍历卡片链表、账单链表、登录记录链表和结算记录链表
    // 将每个节点的数据按照预定的格式写入文件,最后将记录数量写入文件,并遍历记录链表将每个节点的数据按照预定的格式写入文件
    while (card_current != NULL && billing_current != NULL &&
           login_current != NULL && settle_current != NULL)
    {
        card_total++;  
        // 遍历到链表的末尾,同时统计卡片数量和记录数量,以便后续将这些统计数据写入文件中
        card_current = card_current->next;     
        billing_current = billing_current->next;
        login_current = login_current->next;
        settle_current = settle_current->next;
    }

    while (record_current != NULL)
    {
        record_total++;
        record_current = record_current->next;
    }

    count = card_total;
    recordCount = record_total;

    fprintf(file, "COUNT|%d\n", count);   // 将卡片数量写入文件

    // 将卡片链表、账单链表、登录记录链表和结算记录链表的数据写入文件
    card_current = card_head;             
    billing_current = billing_head;
    login_current = login_head;
    settle_current = settle_head;
    // 遍历到链表的末尾,同时将每个节点的数据按照预定的格式写入文件中,以便后续加载数据时能够正确解析和使用这些数据
    while (card_current != NULL && billing_current != NULL &&
           login_current != NULL && settle_current != NULL)
    {
        fprintf(file, "CARD|%s|%s|%.2f|%.2f|%d|%d|%s\n",
                card_current->cardID, card_current->code, card_current->money,
                card_current->used_money, card_current->state, card_current->use_count,
                text_field(card_current->last_time));
        fprintf(file, "BILLING|%s|%s|%.2f|%.2f|%d|%d|%s|%d\n",
                billing_current->cardID, billing_current->code, billing_current->money,
                billing_current->amount_money, billing_current->state,
                billing_current->use_count, text_field(billing_current->last_time),
                billing_current->nStatus);
        fprintf(file, "LOGIN|%s|%.2f|%.2f|%d|%s\n",
                login_current->cardID, login_current->money, login_current->used_money,
                login_current->use_count, text_field(login_current->login_time));
        fprintf(file, "SETTLE|%s|%.2f|%.2f|%d|%s|%d\n",
                settle_current->cardID, settle_current->money, settle_current->used_money,
                settle_current->use_count, text_field(settle_current->settle_time),
                settle_current->nStatus);

        // 遍历到链表的末尾,同时将每个节点的数据按照预定的格式写入文件中,以便后续加载数据时能够正确解析和使用这些数据
        // 注意每个字段之间用|分隔,每条记录占一行
        card_current = card_current->next;
        billing_current = billing_current->next;
        login_current = login_current->next;
        settle_current = settle_current->next;
    }

    fprintf(file, "RECORD_COUNT|%d\n", recordCount);
    record_current = record_head;
    while (record_current != NULL)
    {
        fprintf(file, "RECORD|%s|%.2f|%d|%d|%d|%d|%d|%d\n",
                record_current->cardID, record_current->amount, record_current->year,
                record_current->month, record_current->day, record_current->hour,
                record_current->minute, record_current->second);
        record_current = record_current->next;
    }

    fclose(file);  // 关闭文件
    return 1;
}
