#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "card.h"
#include "billing.h"
#include "computer.h"

void on_computer(void)
{
    char cardID[10];
    char inputCode[50];

    Card *card_current = card_head;  
    Billing *billing_current = billing_head;
    Login *login_current = login_head;
    Settle *settle_current = settle_head;

    printf("-----------上机-----------\n");
    printf("请输入要上机的卡号: ");
    scanf("%9s", cardID);

    while (card_current != NULL && billing_current != NULL &&      // 确保所有链表都未到达末尾
           login_current != NULL && settle_current != NULL)
    {
        if (strcmp(card_current->cardID, cardID) == 0)
        {
            if (card_current->state == 3)
            {
                printf("该卡已注销，不能上机。\n");
                system("pause");
                system("cls");
                return;
            }
            if (card_current->state == 2)
            {
                printf("该卡已经在上机状态。\n");
                system("pause");
                system("cls");
                return;
            }
            if (card_current->money <= 0)
            {
                printf("余额不足，不能上机。\n");
                system("pause");
                system("cls");
                return;
            }

            printf("请输入密码: ");
            scanf("%49s", inputCode);
            if (strcmp(card_current->code, inputCode) != 0)
            {
                printf("密码错误。\n");
                system("pause");
                system("cls");
                return;
            }

            SYSTEMTIME st;          // 获取系统时间的结构体
            GetLocalTime(&st);      // 获取当前系统时间

            // 将当前时间格式化为字符串，存储在登录记录的login_time字段中
            sprintf(login_current->login_time, "%04d-%02d-%02d %02d:%02d:%02d",
                    st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

            card_current->state = 2;        // 更新卡片状态为上机
            card_current->use_count++;      // 增加使用次数
            strcpy(card_current->last_time, login_current->login_time);

            // 更新账单信息
            billing_current->state = 2;     // 更新账单状态为上机
            billing_current->money = card_current->money;
            billing_current->use_count = card_current->use_count;
            billing_current->nStatus = 0;   // 设置账单状态为未结算
            strcpy(billing_current->last_time, login_current->login_time);

            // 更新上机记录
            login_current->money = card_current->money;
            login_current->used_money = billing_current->amount_money;
            login_current->use_count = card_current->use_count;

            // 更新结算记录
            settle_current->money = card_current->money;
            settle_current->used_money = billing_current->amount_money;
            settle_current->use_count = card_current->use_count;
            settle_current->settle_time[0] = '\0';
            settle_current->nStatus = 0;   // 设置结算状态为未结算

            printf("------------- 上机信息如下------------\n");
            printf("+--------+---------------+----------------------+\n");
            printf("| 卡号   | 余额          | 上机时间             |\n");
            printf("+--------+---------------+----------------------+\n");
            printf("| %-6s | %-10.2fRMB | %-20s |\n", card_current->cardID, card_current->money, login_current->login_time);
            printf("+--------+---------------+----------------------+\n");

            system("pause");
            system("cls");
            return;
        
        }

        // 移动到下一个节点
        card_current = card_current->next;
        billing_current = billing_current->next;
        login_current = login_current->next;
        settle_current = settle_current->next;
    }

    printf("未找到该卡。\n");
    system("pause");
    system("cls");
}
