#include <stdio.h>
#include <string.h>
#include <windows.h>
#define MAX 1000

#include "card.h"  

void off_computer()
{
    printf("-----------下机-----------\n");
    printf("请输入要下机的卡号(5位数字):\n");
    char cardID[10];
    scanf("%9s", cardID);
    for (int i = 0; i < count; i++)
    {
        if (strcmp(cards[i].cardID, cardID) == 0)
        {
            if (cards[i].state != 2)  // state为2表示上机状态
            {
                printf("该卡未在使用中，无法下机！\n");
                system("pause");
                system("cls");
                return;
            }
            else
            {
                // 输入密码验证
                char input_code[50];
                printf("请输入密码: \n");
                if (scanf("%49s", input_code) != 1)
                {
                    printf("密码输入失败，请重新输入！\n");
                    while (getchar() != '\n');  // 清除非法输入
                    return;
                }
                if (strcmp(cards[i].code, input_code) != 0)
                {
                    printf("密码错误，无法下机！\n");
                    system("pause");
                    system("cls");
                    return;
                }
                
                if (strcmp(cards[i].code,input_code) == 0)
                {
                    //记录上机时间
                    char time_last[20];
                    strcpy(time_last, cards[i].last_time); 

                    // 记录下机时间
                    SYSTEMTIME st;       // 获取系统时间
                    GetLocalTime(&st);   // 获取当前系统时间
                    sprintf(cards[i].last_time, "%04d-%02d-%02d %02d:%02d:%02d",
                    st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
                    double session_time = (st.wHour - atoi(time_last + 11)) * 3600 + 
                                  (st.wMinute - atoi(time_last + 14)) * 60 + 
                                  (st.wSecond - atoi(time_last + 17)); // 计算上机时长，单位为秒

                    cards[i].used_money += session_time * 0.01; // 假设每秒钟收费0.01元 
                    // 对花费进行四舍五入，保留两位小数
                    cards[i].used_money = (int)(cards[i].used_money * 100 + 0.5) / 100.0;

                    cards[i].money -= session_time * 0.01;      // 从余额中扣除费用
                    // 对余额进行四舍五入，保留两位小数
                    cards[i].money = (int)(cards[i].money * 100 + 0.5) / 100.0;

                    if (cards[i].money < 0)  // 余额不足，无法下机
                    {
                        printf("余额不足，请充值！\n");
                        cards[i].last_time[0] = '\0';  // 清空下机时间
                        system("pause");
                        system("cls");
                        return;
                    }

                    printf("下机成功！\n"); 
                    cards[i].state = 1;   // 更新状态为正常

                    printf("+--------+--------+------------+----------------------+----------------------+\n");
                    printf("| 卡号   | 消费   | 余额       | 上机时间             | 下机时间             |\n");
                    printf("+--------+--------+------------+----------------------+----------------------+\n");
                    printf("| %-6s | %-6.2f | %-10.2f | %-20s | %-20s |\n", 
                        cards[i].cardID, cards[i].used_money, cards[i].money, 
                        time_last, cards[i].last_time);
                    printf("+--------+--------+------------+----------------------+----------------------+\n");
                    system("pause");
                    system("cls");            
                    return;
                }
            }
        }
    }
    printf("未找到该卡！\n");
    system("pause");
    system("cls");
}