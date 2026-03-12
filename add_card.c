#include <stdio.h>
#include <string.h>
#include <windows.h>
#define MAX 1000
#include "billing.h"
#include "card.h"  
#include "record.h"

void add_card()
{   
    printf("-----------添加卡-----------\n");
    // 1) 卡号必须为5位数字且唯一
    while (1)
    {
        int duplicate = 0;  // 检查卡号是否已存在
        printf("请输入卡号(5位数字): \n");
        if (scanf("%s", cards[count].cardID) != 1)
        {
            printf("输入不是数字，请重新输入！\n");
            while (getchar() != '\n');  // 清除非法输入
            continue; 
        }

        if (strlen(cards[count].cardID) != 5 ) //限制卡号必须为5位
        {
            printf("卡号格式错误，请重新输入！\n");
            continue;
        }

        for (int i = 0; i < count; i++)
        {
            if (strcmp(cards[i].cardID, cards[count].cardID) == 0)
            {
                duplicate = 1;
                break;
            }
        }
        if (duplicate)
        {
            printf("卡号已存在，请重新输入！\n");
            continue;
        }
        break;
    }
    // 2) 密码必须包含字母和数字
    while (1)
    {
        int hasLetter = 0, hasDigit = 0;
        printf("请输入密码(字母和数字组合且最多为20个字符): \n");
        scanf("%s", cards[count].code);

        if (strlen(cards[count].code) > 20)   // 密码长度限制
        {
            printf("密码长度超过20个字符，请重新输入！\n");
            continue;
        }

        for (int i = 0; cards[count].code[i] != '\0'; i++)   // 检查密码中是否包含字母和数字
        {
            if ((cards[count].code[i] >= 'a' && cards[count].code[i] <= 'z') ||
                (cards[count].code[i] >= 'A' && cards[count].code[i] <= 'Z'))
            {
                hasLetter = 1;
            }
            else if (cards[count].code[i] >= '0' && cards[count].code[i] <= '9')
            {
                hasDigit = 1;
            }
        }

        if ((hasLetter == 0 || hasDigit == 0)) 
        {
            printf("密码必须包含字母和数字的组合，请重新输入！\n");
            continue;
        }
        break;
    }

    // 3) 开卡金额不能为负数
    while (1)
    {
        printf("请输入开卡金额(RMB): \n");
        if (scanf("%lf", &cards[count].money) != 1)
        {
            printf("金额输入无效，请输入数字！\n");
            while (getchar() != '\n');  // 清除非法输入
            continue;
        }

        if (cards[count].money < 0)
        {
            printf("开卡金额不能为负数，请重新输入！\n");
            continue;
        }
        break;
    }

    cards[count].state = 1;           // 默认状态为正常
    cards[count].use_count = 0;
    cards[count].used_money = 0;
    cards[count].last_time[0] = '\0'; // 初始化为空字符串

    billings[count].nStatus = 1; // 初始化消费状态为已结算

    records[count].amount = 0; // 初始化消费金额为0

    count++;
    
    printf("---------添加卡信息成功！---------\n");

    printf("+--------+------------------+---------------+------------+\n");
    printf("| 卡号   | 密码             |  余 额        | 状态       |\n");
    printf("+--------+------------------+---------------+------------+\n");

    printf("| %-6s | %-16s | %-10.2fRMB | %-13s |\n",
         cards[count - 1].cardID,  cards[count - 1].code, cards[count - 1].money, "已 激 活");
    printf("+--------+------------------+---------------+------------+\n");

    system("pause");
    system("cls");
}