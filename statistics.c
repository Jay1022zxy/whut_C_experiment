#include <stdio.h>
#include <string.h>
#include <windows.h>
#define MAX 1000
#include "card.h"
#include "billing.h"
#include "record.h"
#include "computer.h"   

int date_to_int(int year, int month, int day)  // 将日期转换为整数，格式为YYYYMMDD
{
    return year * 10000 + month * 100 + day;
}

// 1. 查询某卡片某时间段内的消费记录
void search_card_records()
{
    char cardID[6];
    char code[50];
    int year, month, day;
    int end_year, end_month, end_day;

    printf("-----------查询某卡片某时间段内的消费记录-----------\n");
    printf("请输入卡号(5位数字): ");
    scanf("%5s", cardID);
    
    if (strcmp(cardID, "00000") == 0)
    {
        printf("卡号格式错误，请重新输入！\n");
        return;
    }

    int cardIndex = -1;
    for (int i = 0; i < count; i++)
    {
        if (strcmp(cards[i].cardID, cardID) == 0)
        {
            cardIndex = i;
            break;
        }
    }

    if (cardIndex == -1)
    {
        printf("未找到该卡号！\n");
        return;
    }

    printf("请输入密码: ");
    scanf("%49s", code);

    if (strcmp(cards[cardIndex].code, code) != 0)
    {
        printf("密码错误！\n");
        return;
    }

    printf("请输入开始日期(格式: 年 月 日,以空格分隔): ");
    scanf("%d %d %d", &year, &month, &day);

    printf("请输入结束日期(格式: 年 月 日,以空格分隔): ");
    scanf("%d %d %d", &end_year, &end_month, &end_day);
    
    int start = date_to_int(year, month, day);
    int end = date_to_int(end_year, end_month, end_day);

    int found = 0;    // 标记是否找到记录

    for (int i = 0; i < count; i++)
    {
        int log_year, log_month, log_day;

        // 从 login_time 里提取 年月日
        sscanf(logins[i].login_time, "%d-%d-%d", &log_year, &log_month, &log_day);

        int cur = date_to_int(log_year, log_month, log_day);

        if (cur >= start && cur <= end)
        {
            printf("+--------+--------+----------------------+----------------------+\n");
            printf("| 卡号   | 消费   | 上机时间             | 下机时间             |\n");
            printf("+--------+--------+----------------------+----------------------+\n");

            printf("| %-6s | %-6.2f | %-20s | %-20s |\n",
                cards[i].cardID,
                billings[i].amount_money,
                logins[i].login_time,
                settles[i].settle_time);
            found = 1;            // 标记找到记录
            
            printf("+--------+--------+----------------------+----------------------+\n");

            system("pause");
            system("cls");
        }
    }

            

    if (!found)
    {
        printf("该时间段内没有记录。\n");

        system("pause");
        system("cls");

    }
}


// 2. 查询商家某时间段内的营业额
void search_merchant_turnover_period()
{
    int sy, sm, sd;
    int ey, em, ed;

    printf("-----------查询商家某时间段内的营业额-----------\n");
    
    printf("请输入开始日期(格式: 年 月 日): ");
    scanf("%d %d %d", &sy, &sm, &sd);

    printf("请输入结束日期(格式: 年 月 日): ");
    scanf("%d %d %d", &ey, &em, &ed);

    int start = date_to_int(sy, sm, sd);
    int end = date_to_int(ey, em, ed);

    double total = 0.0;

    for (int i = 0; i < recordCount; i++)
    {
        int cur = date_to_int(records[i].year, records[i].month, records[i].day);

        if (cur >= start && cur <= end)
        {
            total += records[i].amount;
        }
    }

    printf("在 %04d-%02d-%02d 到 %04d-%02d-%02d 期间的营业额为：%.2f\n",
           sy, sm, sd, ey, em, ed, total);

    system("pause");
    system("cls");

}


// 3. 查询商家某年每个月的营业额
void search_merchant_turnover_year()
{
    int year;
    double monthTotal[12] = {0};

    printf("请输入年份: ");
    scanf("%d", &year);

    for (int i = 0; i < recordCount; i++)
    {
        if (records[i].year == year)
        {
            monthTotal[records[i].month - 1] += records[i].amount;
        }
    }

    printf("\n在 %d 年每个月的营业额如下：\n", year);
    for (int i = 0; i < 12; i++)
    {
        printf("%2d 月：%.2f\n", i + 1, monthTotal[i]);
    }

    system("pause");
    system("cls");

}

void statistics()
{
    int choice;

    while (1)
    {
        printf("-----------查询统计-----------\n");
        printf("1. 查询某卡片某时间段内的消费记录\n");
        printf("2. 查询商家某时间段内的营业额\n");
        printf("3. 查询商家某年每个月的营业额\n");
        printf("0. 返回主菜单\n");
        printf("请选择查询统计项编号(0~3): ");
        if (scanf("%d", &choice) != 1)
        {
            printf("输入错误：请输入数字！\n");
            int ch; 
            while ((ch = getchar()) != '\n' && ch != EOF);
            continue;
        }

        switch (choice)
        {
            case 1:
                search_card_records();
                break;
            case 2:
                search_merchant_turnover_period();
                break;
            case 3:
                search_merchant_turnover_year();
                break;
            case 0:
                return;   // 返回主菜单
            default:
                printf("输入错误，请重新输入！\n");
                system("pause");
                system("cls");
                break;
        }
    }
}