#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "card.h"
#include "billing.h"
#include "computer.h"
#include "record.h"

void off_computer(void)
{
    char cardID[10];
    char inputCode[50];

    // 定义指针变量，分别指向卡片链表、账单链表、登录记录链表和结算记录链表的当前节点
    Card *card_current = card_head;  
    Billing *billing_current = billing_head;
    Login *login_current = login_head;
    Settle *settle_current = settle_head;

    printf("-----------下机-----------\n");
    printf("请输入要下机的卡号: ");
    scanf("%9s", cardID);

    while (card_current != NULL && billing_current != NULL &&     // 确保所有链表都在遍历
           login_current != NULL && settle_current != NULL)
    {
        if (strcmp(card_current->cardID, cardID) == 0) // 找到匹配的卡号
        {
            double session_seconds;
            double amount;
            SYSTEMTIME st;                             // 获取系统时间的结构体
            Record *new_record;

            if (card_current->state != 2)           // 上机状态为2，如果不是上机状态则无法下机
            {
                printf("该卡当前不在上机状态。\n");
                system("pause");
                system("cls");
                return;
            }

            printf("请输入密码: ");
            scanf("%20s", inputCode);
            if (strcmp(card_current->code, inputCode) != 0)
            {
                printf("密码错误。\n");
                system("pause");
                system("cls");
                return;
            }

            GetLocalTime(&st);  // 获取当前系统时间
            // 将当前时间格式化为字符串，存储在结算记录的settle_time字段中
            sprintf(settle_current->settle_time, "%04d-%02d-%02d %02d:%02d:%02d",
                    st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

            // 用atoi函数提取登录时间中的小时、分钟和秒，并计算上机时长 +11, +14, +17是因为登录时间格式为 "YYYY-MM-DD HH:MM:SS"
            // +11是跳过前面10个字符的日期和空格，直接定位到小时部分；+14是跳过小时和冒号，定位到分钟部分；+17是跳过分钟和冒号，定位到秒部分。
            session_seconds = (st.wHour - atoi(login_current->login_time + 11)) * 3600.0 +   
                              (st.wMinute - atoi(login_current->login_time + 14)) * 60.0 +
                              (st.wSecond - atoi(login_current->login_time + 17));
            // 如果计算结果为负数，说明跨过了午夜，需要加上24小时的秒数
            if (session_seconds < 0)  
            {
                session_seconds += 24 * 3600.0;  
            }

            amount = session_seconds * 0.02;             // 每秒0.02元
            amount = (int)(amount * 100 + 0.5) / 100.0;  // 保留两位小数，四舍五入

            if (amount > card_current->money)            // 余额不足，无法下机
            {
                printf("余额不足，请先充值。\n");
                billing_current->nStatus = 0;            // 设置账单状态为未结算
                settle_current->nStatus = 0;             // 设置下机状态为未结算
                settle_current->settle_time[0] = '\0';   // 清空下机时间

                system("pause");
                system("cls");

                return;
            }

            // 更新卡片信息
            card_current->money -= amount;         
            card_current->money = (int)(card_current->money * 100 + 0.5) / 100.0;
            card_current->used_money += amount;
            strcpy(card_current->last_time, settle_current->settle_time);  // 更新上次使用时间

            // 更新账单信息
            billing_current->amount_money += amount;      // 累计使用金额
            billing_current->amount_money = (int)(billing_current->amount_money * 100 + 0.5) / 100.0;
            billing_current->money = card_current->money; // 更新账单中的余额
            billing_current->use_count = card_current->use_count; // 更新使用次数
            billing_current->nStatus = 1;                 // 设置账单状态为已结算
            strcpy(billing_current->last_time, settle_current->settle_time); // 更新账单中的上次使用时间

            // 更新上机记录和下机记录
            login_current->money = card_current->money;   // 更新上机记录中的余额
            login_current->used_money = billing_current->amount_money;
            login_current->use_count = card_current->use_count;

            // 结算记录更新
            settle_current->money = card_current->money;
            settle_current->used_money = billing_current->amount_money;
            settle_current->use_count = card_current->use_count;
            settle_current->nStatus = 1; // 设置结算状态为已结算

            // 记录消费信息到Record链表
            new_record = (Record *)calloc(1, sizeof(Record)); // 分配内存并初始化为0
            if (new_record != NULL)
            {
                // 将消费记录的信息填充到新记录中
                strcpy(new_record->cardID, card_current->cardID);
                new_record->amount = amount;
                new_record->year = st.wYear;
                new_record->month = st.wMonth;
                new_record->day = st.wDay;
                new_record->hour = st.wHour;
                new_record->minute = st.wMinute;
                new_record->second = st.wSecond;
                new_record->next = NULL; // 初始化新记录的next指针为NULL

                if (record_head == NULL) // 如果记录链表为空，则将新记录设置为头节点
                {
                    record_head = new_record;
                }
                else
                {
                    Record *tail = record_head; 
                    while (tail->next != NULL) tail = tail->next; // 找到链表的最后一个节点
                    tail->next = new_record;    // 将新记录链接到链表末尾
                }
                recordCount++; // 记录总数加1
            }

            printf("下机成功！\n"); 
            card_current->state = 1; // 更新卡片状态为正常
            billing_current->nStatus = 1; // 更新账单状态为已结算
            settle_current->nStatus = 1; // 更新结算状态为已结算

            printf("+--------+--------+------------+----------------------+----------------------+\n");
            printf("| 卡号   | 消费   | 余额       | 上机时间             | 下机时间             |\n");
            printf("+--------+--------+------------+----------------------+----------------------+\n");
            printf("| %-6s | %-6.2f | %-10.2f | %-20s | %-20s |\n", 
                card_current->cardID, billing_current->amount_money, card_current->money, 
                card_current->last_time, settle_current->settle_time);
            printf("+--------+--------+------------+----------------------+----------------------+\n");
            system("pause");
            system("cls");            
            return;
            return;
        }
        // 继续遍历链表
        card_current = card_current->next;
        billing_current = billing_current->next;
        login_current = login_current->next;
        settle_current = settle_current->next;
    }

    printf("未找到该卡。\n");
    system("pause");
    system("cls");
}
