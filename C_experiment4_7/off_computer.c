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
        if (strcmp(card_current->cardID, cardID) == 0)
        {
            double session_seconds;
            double amount;
            SYSTEMTIME st;
            Record *new_record;

            if (card_current->state != 2)             
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

            GetLocalTime(&st);
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
            amount = (int)(amount * 100 + 0.5) / 100.0;

            if (amount > card_current->money)
            {
                printf("余额不足，请先充值。\n");
                billing_current->nStatus = 0;            // 设置账单状态为未结算
                settle_current->nStatus = 0;             // 设置结算状态为未结算
                settle_current->settle_time[0] = '\0';   // 清空结算时间

                system("pause");
                system("cls");

                return;
            }

            card_current->money -= amount;         
            card_current->money = (int)(card_current->money * 100 + 0.5) / 100.0;
            card_current->used_money += amount;
            strcpy(card_current->last_time, settle_current->settle_time);  // 更新上次使用时间

            // 更新账单信息
            billing_current->amount_money += amount;
            billing_current->amount_money = (int)(billing_current->amount_money * 100 + 0.5) / 100.0;
            billing_current->money = card_current->money;
            billing_current->use_count = card_current->use_count;
            billing_current->nStatus = 1;
            strcpy(billing_current->last_time, settle_current->settle_time);

            // 更新登录记录和结算记录
            login_current->money = card_current->money;
            login_current->used_money = billing_current->amount_money;
            login_current->use_count = card_current->use_count;

            // 结算记录更新
            settle_current->money = card_current->money;
            settle_current->used_money = billing_current->amount_money;
            settle_current->use_count = card_current->use_count;
            settle_current->nStatus = 1;

            // 记录消费信息到Record链表
            new_record = (Record *)calloc(1, sizeof(Record));
            if (new_record != NULL)
            {
                strcpy(new_record->cardID, card_current->cardID);
                new_record->amount = amount;
                new_record->year = st.wYear;
                new_record->month = st.wMonth;
                new_record->day = st.wDay;
                new_record->hour = st.wHour;
                new_record->minute = st.wMinute;
                new_record->second = st.wSecond;
                new_record->next = NULL;

                if (record_head == NULL)
                {
                    record_head = new_record;
                }
                else
                {
                    Record *tail = record_head;
                    while (tail->next != NULL) tail = tail->next;
                    tail->next = new_record;
                }
                recordCount++;
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
