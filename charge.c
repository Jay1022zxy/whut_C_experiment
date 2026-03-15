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
    for (int i = 0; i < count; i++)
    {
        if (strcmp(cards[i].cardID, cardID) == 0)
        {
            if (cards[i].state == 3)  // state为3表示注销状态
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
            cards[i].money += amount; // 增加余额
            // 对余额进行四舍五入，保留两位小数
            cards[i].money = (int)(cards[i].money * 100 + 0.5) / 100.0;

            printf("-----------充值结果如下-----------\n");

            printf("+----------------------+----------------------+----------------------+\n");
            printf("| 卡号                 | 充值金额             | 当前余额             |\n");
            printf("+----------------------+----------------------+----------------------+\n");
            printf("| %-20s | %-20.2f | %-20.2f |\n", cards[i].cardID, amount, cards[i].money);
            printf("+----------------------+----------------------+----------------------+\n");
            
            system("pause");
            system("cls");            
            return;
        }
    }
    printf("未找到该卡！\n");
    system("pause");
    system("cls"); 
}