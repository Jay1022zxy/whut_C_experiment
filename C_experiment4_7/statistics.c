#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "card.h"
#include "record.h"

static int date_to_int(int year, int month, int day)  // 将日期转换为整数格式,以便比较日期的先后顺序,格式为YYYYMMDD
{
    return year * 10000 + month * 100 + day;
}

static Card *find_card_by_id(const char *cardID)     // 根据卡号在卡片链表中查找对应的卡片节点,如果找到则返回指向该节点的指针,否则返回NULL
{
    Card *current = card_head;
    while (current != NULL)
    {
        if (strcmp(current->cardID, cardID) == 0)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// 1. 查询某卡在时间段内的消费记录,首先输入卡号和密码进行验证,然后输入开始日期和结束日期,最后遍历记录链表,找到符合条件的记录并输出
static void search_card_records(void)
{
    char cardID[6];
    char code[50];
    int sy, sm, sd, ey, em, ed;
    int start, end;
    int found = 0;

    Card *card_current;
    Record *record_current = record_head;

    printf("-----------查询卡消费记录-----------\n");
    printf("请输入卡号: ");
    scanf("%5s", cardID);

    card_current = find_card_by_id(cardID); // 寻找对应的卡片节点,如果找不到则提示未找到该卡并返回
    if (card_current == NULL)
    {
        printf("未找到该卡。\n");
        system("pause");
        system("cls");
        return;
    }

    printf("请输入密码: ");
    scanf("%49s", code);
    if (strcmp(card_current->code, code) != 0)
    {
        printf("密码错误。\n");
        system("pause");
        system("cls");
        return;
    }

    printf("请输入开始日期(年 月 日): ");
    scanf("%d %d %d", &sy, &sm, &sd);
    printf("请输入结束日期(年 月 日): ");
    scanf("%d %d %d", &ey, &em, &ed);

    start = date_to_int(sy, sm, sd);
    end = date_to_int(ey, em, ed);

    while (record_current != NULL)
    {
        int cur = date_to_int(record_current->year, record_current->month, record_current->day);
        if (strcmp(record_current->cardID, cardID) == 0 && cur >= start && cur <= end)
        {
            if (!found)  // 如果是第一次找到符合条件的记录,则输出表头
            {
                printf("+--------+--------+----------------------+\n");
                printf("| 卡号   | 金额   | 时间                 |\n");
                printf("+--------+--------+----------------------+\n");
            }

            printf("| %-6s | %-6.2f | %04d-%02d-%02d %02d:%02d:%02d  |\n",
                   record_current->cardID,
                   record_current->amount,
                   record_current->year,
                   record_current->month,
                   record_current->day,
                   record_current->hour,
                   record_current->minute,
                   record_current->second);
            found = 1;
        }
        record_current = record_current->next;
    }

    if (found) // 如果找到至少一条符合条件的记录,则输出表尾,否则提示没有记录
    {
        printf("+--------+--------+----------------------+\n");
    }
    else
    {
        printf("该时间段内没有消费记录。\n");
        system("pause");
        system("cls");
    }
}

// 2. 查询时间段营业额,首先输入开始日期和结束日期,然后遍历记录链表,找到符合条件的记录并累加金额,最后输出总营业额
static void search_merchant_turnover_period(void)
{
    int sy, sm, sd, ey, em, ed;
    int start, end;
    double total = 0.0; // 定义一个变量total来存储总营业额,初始值为0.0
    Record *record_current = record_head;

    printf("-----------查询时间段营业额-----------\n");
    printf("请输入开始日期(年 月 日): ");
    scanf("%d %d %d", &sy, &sm, &sd);
    printf("请输入结束日期(年 月 日): ");
    scanf("%d %d %d", &ey, &em, &ed);

    start = date_to_int(sy, sm, sd);
    end = date_to_int(ey, em, ed);

    // 遍历记录链表,找到符合条件的记录并累加金额
    while (record_current != NULL) 
    {
        int cur = date_to_int(record_current->year, record_current->month, record_current->day);
        if (cur >= start && cur <= end)
        {
            total += record_current->amount;
        }
        record_current = record_current->next;
    }

    printf("%04d-%02d-%02d 到 %04d-%02d-%02d 的营业额为: %.2f\n",
           sy, sm, sd, ey, em, ed, total);
    system("pause");
    system("cls");
}

// 3. 查询某年每月营业额,首先输入年份,然后遍历记录链表,找到符合条件的记录并累加每个月的金额,最后输出每个月的营业额
static void search_merchant_turnover_year(void)   
{
    int year;
    double monthTotal[12] = {0}; // 定义一个数组monthTotal来存储每个月的营业额,初始值都为0.0
    int i;
    Record *record_current = record_head;

    printf("请输入年份: ");
    scanf("%d", &year);

    while (record_current != NULL) // 遍历记录链表,找到符合条件的记录并累加每个月的金额
    {
        if (record_current->year == year && record_current->month >= 1 && record_current->month <= 12)
        {
            monthTotal[record_current->month - 1] += record_current->amount; // 累加每个月的金额,注意数组索引从0开始,所以要减1
        }
        record_current = record_current->next;
    }

    for (i = 0; i < 12; ++i)
    {
        printf("%2d 月: %.2f\n", i + 1, monthTotal[i]);
    }
    system("pause");
    system("cls");
}

void statistics(void)
{
    int choice;

    while (1)
    {
        printf("-----------统计查询-----------\n");
        printf("1. 查询某卡在时间段内的消费记录\n");
        printf("2. 查询时间段营业额\n");
        printf("3. 查询某年每月营业额\n");
        printf("0. 返回主菜单\n");
        printf("请输入选项: ");

        if (scanf("%d", &choice) != 1)
        {
            int ch;
            printf("输入错误，请重新输入。\n");
            while ((ch = getchar()) != '\n' && ch != EOF) {}
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
                system("pause");
                system("cls");
                return;
            default:
                printf("输入错误，请重新输入。\n");
                break;
        }

    }
}
