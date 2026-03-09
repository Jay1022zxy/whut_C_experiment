#include <stdio.h>
#include <windows.h>
#include <string.h>
#define MAX 1000

#include "card.h"  // 02查询卡的函数实现

// 2. 查询卡函数(strstr函数实现模糊查询)
void search_card()
{
    printf("-----------查询卡-----------\n");

    char input[10];
    printf("请输入查询的卡号(支持模糊查询): ");
    scanf("%s", input);

    int count0 = 0;

    for (int i = 0; i < count; i++)
    {
        if (strstr(cards[i].cardID, input) != NULL)   // 使用strstr进行模糊查询，检查卡号中是否包含输入的字符串
        {
            if(count0 == 0)
            {
                printf("-------------------------------------查询结果-----------------------------------\n");
                printf("+--------+--------+------------+------------+------------+----------------------+\n");
                printf("| 卡号   | 状态   | 余额       | 累计使用   | 使用次数   | 上次使用时间         |\n");
                printf("+--------+--------+------------+------------+------------+----------------------+\n");
            }
            
            // 如果暂未使用，则显示"暂无记录"
            if (cards[i].last_time[0] == '\0') 
            {
            
            printf("| %-6s | %-8s | %-10.2lf | %-10.2lf | %-10d | %-24s |\n",  //保持表格对齐
                   cards[i].cardID,
                   cards[i].state == 1 ? "正常" : cards[i].state == 2 ? "上机" : "注销",
                   cards[i].money,
                   cards[i].used_money,
                   cards[i].use_count,
                   "暂 无 记 录"); // 如果last_time为空，则显示"暂无记录"
            count0++; // 统计查询到的卡片数量
            }
            else
            {
                printf("| %-6s | %-8s | %-10.2lf | %-10.2lf | %-10d | %-20s |\n",  //保持表格对齐
                   cards[i].cardID,
                   cards[i].state == 1 ? "正常" : cards[i].state == 2 ? "上机" : "注销",
                   cards[i].money,
                   cards[i].used_money,
                   cards[i].use_count,
                   cards[i].last_time); // 显示上次使用时间
            count0++; // 统计查询到的卡片数量
            }
        }
    }

    if(count0 > 0)
    {
        printf("+--------+--------+------------+------------+------------+----------------------+\n");
    }
    else
    {
        printf("未查询到该卡！\n");
    }
 
    system("pause");
    system("cls");
}
