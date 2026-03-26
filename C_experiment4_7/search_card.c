#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "card.h"
#include "billing.h"

static const char *state_text(int state)  // 将状态码转换为文本显示，返回一个字符串指针
{
    if (state == 1) return "正常";
    if (state == 2) return "上机";
    if (state == 3) return "注销";
    return "未知";
}

void search_card()
{
    char input[10]; 
    int found = 0; // 标记是否找到匹配的卡片
    Card *card_current = card_head;           // 从链表头开始遍历卡片链表
    Billing *billing_current = billing_head;  // 从链表头开始遍历账单链表

    printf("-----------查询卡-----------\n");
    printf("请输入卡号(支持模糊查询): ");
    scanf("%9s", input);

    while (card_current != NULL && billing_current != NULL)
    {
        if (strstr(card_current->cardID, input) != NULL) // 使用strstr函数进行模糊查询，如果输入的字符串在卡号中出现则返回非NULL
        {
            if (!found)
            {
                printf("+--------+--------+------------+------------+------------+----------------------+\n");
                printf("| 卡号   | 状态   | 余额       | 累计消费   | 使用次数   | 最后使用时间         |\n");
                printf("+--------+--------+------------+------------+------------+----------------------+\n");
            }

            // 为了美观输出,如果last_time为空则显示"暂 无 记 录"且占24个字符,否则显示last_time的内容且占20个字符
            if (card_current->last_time[0] == '\0')
            {
            printf("| %-6s | %-8s | %-10.2f | %-10.2f | %-10d | %-24s |\n",
                   card_current->cardID,
                   state_text(card_current->state),   // 将状态码转换为文本显示
                   card_current->money,
                   billing_current->amount_money,
                   card_current->use_count,
                  "暂 无 记 录");
            found = 1; // 标记已经找到至少一个匹配的卡片
            }
            else
            {
                printf("| %-6s | %-8s | %-10.2f | %-10.2f | %-10d | %-20s |\n",
                       card_current->cardID,
                       state_text(card_current->state),   // 将状态码转换为文本显示
                       card_current->money,
                       billing_current->amount_money,
                       card_current->use_count,
                       card_current->last_time);
            found = 1;
            }
        }

        card_current = card_current->next;       // 继续遍历下一个卡片节点
        billing_current = billing_current->next; // 继续遍历下一个账单节点，保持与卡片链表同步
    }

    if (found)
    {
        printf("+--------+--------+------------+------------+------------+----------------------+\n");
    }
    else
    {
        printf("没有查到对应卡号。\n");
    }

    system("pause");
    system("cls");
}
