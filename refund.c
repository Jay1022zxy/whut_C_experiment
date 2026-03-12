#include <stdio.h>
#include <string.h>
#include <windows.h>
#define MAX 1000
#include "card.h"  
#include "billing.h"

void refund()
{
    printf("-----------退款-----------\n");
    char cardID[10];
    printf("请输入要退款的卡号(5位数字):\n");
    scanf("%9s", cardID);
    for (int i = 0; i < count; i++)
    {
        if (strcmp(cards[i].cardID, cardID) == 0)
        {
            if (cards[i].state == 3)  // state为3表示注销状态
            {
                printf("该卡已注销，无法退款！\n");
                system("pause");
                system("cls");
                return;
            }
            if (cards[i].state == 2) // state为2表示上机状态
            {
                printf("该卡正在使用中，无法退款！\n");
                system("pause");
                system("cls");
                return;
            }
            if (billings[i].nStatus == 0) // nStatus为0表示未结算状态
            {
                printf("该卡有未结算的消费，无法退款！\n");
                system("pause");
                system("cls");
                return;
            }
            // 输入密码验证
            printf("请输入密码:\n");
            char inputCode[50];
            if (scanf("%49s", inputCode) != 1)
            {
                printf("密码输入失败，请重新输入！\n");
                while (getchar() != '\n');  // 清除非法输入
                return;
            }
            if (strcmp(cards[i].code, inputCode) != 0)
            {
                printf("密码错误！\n");
                system("pause");
                system("cls");
                return;
            }
            if (strcmp(cards[i].code, inputCode) == 0)
            {
                double amount;
                printf("请输入退款金额: \n");
                if (scanf("%lf", &amount) != 1 || amount <= 0)
                {
                    printf("输入错误：请输入正数金额！\n");
                    while (getchar() != '\n');  // 清除非法输入
                    continue;
                }
                if (amount > cards[i].money)
                {
                    printf("退款金额超过当前余额！\n");
                    system("pause");
                    system("cls");
                    return;
                }
                cards[i].money -= amount; // 减少余额
                // 对余额进行四舍五入，保留两位小数
                cards[i].money = (int)(cards[i].money * 100 + 0.5) / 100.0;

                printf("-----------退款结果如下-----------\n");
                printf("卡号:%s\n", cards[i].cardID);
                printf("退款金额: %.2f\n", amount);
                printf("剩余余额: %.2f\n", cards[i].money);
                system("pause");
                system("cls");            
                return;
            }  
        }
    }
    printf("未找到该卡！\n");
}