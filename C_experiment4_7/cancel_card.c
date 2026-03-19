#include <stdio.h>
#include <string.h>
#include <windows.h>
#define MAX 1000
#include "card.h"  

void cancel_card()
{
    printf("-----------注销卡-----------\n");
    char cardID[10];
    printf("请输入要注销的卡号(5位数字):\n");
    scanf("%9s", cardID);

    Card *card_current = card_head;

    for (int i = 0; i < count; i++)
    {
        if (strcmp(card_current->cardID, cardID) == 0)
        {
            if (card_current->state == 3)  // state为3表示注销状态
            {
                printf("该卡已注销！\n");
                system("pause");
                system("cls");
                return;
            }
            if (card_current->state == 2) // state为2表示上机状态
            {
                printf("该卡正在使用中，无法注销！\n");
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
            if (strcmp(card_current->code, inputCode) != 0)
            {
                printf("密码错误！\n");
                system("pause");
                system("cls");
                return;
            }
            if (strcmp(card_current->code, inputCode) == 0)
            {
                printf("注销操作不可恢复，是否继续？(y/n):\n");
                char confirm;
                if (scanf(" %c", &confirm) != 1 || (confirm != 'y' && confirm != 'Y'))
                {
                    printf("取消注销操作！\n");
                    system("pause");
                    system("cls");
                    return;
                }

                printf("------------注销信息如下-----------\n");
                printf("+----------------------+----------------------+\n");
                printf("| 卡号                 | 退款金额             |\n");
                printf("+----------------------+----------------------+\n");
                printf("| %-20s | %-20.2f |\n", card_current->cardID, card_current->money);
                printf("+----------------------+----------------------+\n");

                card_current->state = 3;   // 更新状态为注销
                card_current->money = 0;   // 余额清零

                system("pause");
                system("cls");            
                return;
            }
        }
    }
} 
