#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "card.h"  
#define MAX 1000
extern int count;        // 声明全局变量count，表示卡片数量

// 3. 上机函数
void on_computer()
{
    printf("-----------上机-----------\n");
    printf("请输入要上机的卡号(5位数字):\n");
    char cardID[10];
    scanf("%9s", cardID);
    for (int i = 0; i < count; i++)
    {
        if (strcmp(cards[i].cardID, cardID) == 0)
        {
            if (cards[i].state != 1)  // state为1表示正常状态
            {
                printf("该卡未激活，无法上机！\n");
                system("pause");
                system("cls");
                return;
            }
            if (cards[i].money <= 0)
            {
                printf("余额不足，无法上机！\n");
                system("pause");
                system("cls");
                return;
            }
            if (cards[i].state == 2) // state为2表示上机状态
            {
                printf("该卡已在使用中！\n");
                system("pause");
                system("cls");
                return;
            }
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
            else
            {
                printf("上机成功！\n");
                cards[i].use_count++; // 增加使用次数
                cards[i].state = 2;   // 更新状态为上机中
                // 记录上机时间
                SYSTEMTIME st;       // 获取系统时间
                GetLocalTime(&st);   // 获取当前系统时间

                sprintf(cards[i].last_time, "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, 
                        st.wDay, st.wHour, st.wMinute, st.wSecond); // 记录上机时间
                
                printf("--------------上机成功---------------\n");
                printf("卡号: %s\n", cards[i].cardID);
                printf("余额: %.2f\n", cards[i].money);
                printf("上机时间: %s\n", cards[i].last_time);

                system("pause");
                system("cls");
                return;
            }
            
            return;
        }
    }
    printf("未找到该卡！\n");
    system("pause");
    system("cls");
}