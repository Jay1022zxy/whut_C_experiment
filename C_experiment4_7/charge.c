#include <stdio.h>
#include <string.h>
#include <windows.h>
#define MAX 1000
#include "card.h"  

void charge()
{
    printf("-----------充值-----------\n");
    char cardID[10];
    printf("请输入要充值的卡号(5位数字):\n");
    scanf("%9s", cardID);
    
    Card *card_current = card_head; // 从链表头开始遍历

    for (int i = 0; i < count; i++) // 遍历链表
    {
        if (strcmp(card_current->cardID, cardID) == 0)
        {
            if (card_current->state == 3)  // state为3表示注销状态
            {
                printf("该卡已注销，无法充值！\n");
                system("pause");
                system("cls");
                return;
            }
            double amount;
            printf("请输入充值金额: \n");
            if (scanf("%lf", &amount) != 1 || amount <= 0)
            {
                printf("输入错误：请输入正数金额！\n");
                while (getchar() != '\n');  // 清除非法输入
                continue;
            }
            card_current->money += amount; // 增加余额
            // 对余额进行四舍五入，保留两位小数
            card_current->money = (int)(card_current->money * 100 + 0.5) / 100.0;

            printf("-----------充值结果如下-----------\n");

            printf("+----------------------+----------------------+----------------------+\n");
            printf("| 卡号                 | 充值金额             | 当前余额             |\n");
            printf("+----------------------+----------------------+----------------------+\n");
            printf("| %-20s | %-20.2f | %-20.2f |\n", card_current->cardID, amount, card_current->money);
            printf("+----------------------+----------------------+----------------------+\n");
            
            system("pause");
            system("cls");            
            return;
        }
        card_current = card_current->next; // 移动到下一个节点
    }
    printf("未找到该卡！\n");
    system("pause");
    system("cls"); 
}

void recharge(Card *current_card, Billing *current_billing)
{
    printf("-----------充值-----------\n");
    double amount;
    printf("请输入充值金额: \n");
    if (scanf("%lf", &amount) != 1 || amount <= 0)
    {
        printf("输入错误：请输入正数金额！\n");
        while (getchar() != '\n');  // 清除非法输入
        return;
    }
    current_card->money += amount; // 增加余额
    // 对余额进行四舍五入，保留两位小数
    current_card->money = (int)(current_card->money * 100 + 0.5) / 100.0;

    printf("-----------充值结果如下-----------\n");

    printf("+----------------------+----------------------+----------------------+\n");
    printf("| 卡号                 | 充值金额             | 当前余额             |\n");
    printf("+----------------------+----------------------+----------------------+\n");
    printf("| %-20s | %-20.2f | %-20.2f |\n", current_card->cardID, amount, current_card->money);
    printf("+----------------------+----------------------+----------------------+\n");
    
    system("pause");
    system("cls");            
}