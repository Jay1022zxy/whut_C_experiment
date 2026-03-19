#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include "card.h"  
#include "billing.h"
#include "computer.h"
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
            if (cards[i].state == 0)  // state为0表示未激活状态
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
                printf("该卡已上机！\n");
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

                billings[i].nStatus = 0; // 设置消费状态为未结算

                // 记录上机时间
                SYSTEMTIME st;       // 获取系统时间
                GetLocalTime(&st);   // 获取当前系统时间

                sprintf(logins[i].login_time, "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, 
                        st.wDay, st.wHour, st.wMinute, st.wSecond); // 记录上机时间
                
                strcpy(cards[i].last_time, logins[i].login_time);   // 更新卡片的上次使用时间
                
                printf("------------- 上机信息如下------------\n");
                printf("+--------+---------------+----------------------+\n");
                printf("| 卡号   | 余额          | 上机时间             |\n");
                printf("+--------+---------------+----------------------+\n");
                printf("| %-6s | %-10.2fRMB | %-20s |\n", cards[i].cardID, cards[i].money, logins[i].login_time);
                printf("+--------+---------------+----------------------+\n");

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