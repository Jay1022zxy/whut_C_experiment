#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "card.h"
#include "billing.h"
#include "computer.h"

static int is_unique_card_id(const char *cardID) // 检查卡号是否唯一
{
    Card *current = card_head;                   // 从链表头开始遍历
    while (current != NULL)                     
    {
        if (strcmp(current->cardID, cardID) == 0)
        {
            return 0;               // 如果找到相同的卡号，返回0表示不唯一
        }
        current = current->next;    // 继续遍历下一个节点
    }
    return 1;                       // 如果遍历完整个链表都没有找到相同的卡号，返回1表示唯一
}

void add_card()
{
    // 分配内存并初始化为0
    Card *new_card = (Card *)calloc(1, sizeof(Card));      // calloc开辟1个Card结构体大小的内存，并将其初始化为0              
    Billing *new_billing = (Billing *)calloc(1, sizeof(Billing));
    Login *new_login = (Login *)calloc(1, sizeof(Login));
    Settle *new_settle = (Settle *)calloc(1, sizeof(Settle));

    if (new_card == NULL || new_billing == NULL || new_login == NULL || new_settle == NULL)  // 检查内存分配是否成功
    {
        // 如果内存分配失败，释放已分配的内存并返回
        free(new_card);    
        free(new_billing);
        free(new_login);
        free(new_settle);
        printf("内存分配失败。\n");
        system("pause");
        system("cls");
        return;
    }

    printf("-----------添加卡-----------\n");

    while (1)
    {
        printf("请输入5位卡号: ");
        scanf("%9s", new_card->cardID);

        if (strlen(new_card->cardID) != 5)
        {
            printf("卡号必须是5位。\n");
            continue;
        }
        if (!is_unique_card_id(new_card->cardID))
        {
            printf("卡号已存在，请重新输入。\n");
            continue;
        }
        break;
    }

    while (1)
    {
        int has_letter = 0; // 字母数
        int has_digit = 0;  // 数字数
        int i;

        printf("请输入密码(字母+数字，最长15位): ");
        scanf("%49s", new_card->code);

        if (strlen(new_card->code) > 15)
        {
            printf("密码长度不能超过15位。\n");
            continue;
        }

        for (i = 0; new_card->code[i] != '\0'; ++i)           // 检查密码中是否包含字母和数字
        {
            if ((new_card->code[i] >= 'a' && new_card->code[i] <= 'z') ||
                (new_card->code[i] >= 'A' && new_card->code[i] <= 'Z'))
            {
                has_letter = 1;
            }
            else if (new_card->code[i] >= '0' && new_card->code[i] <= '9')
            {
                has_digit = 1;
            }
        }

        if (!has_letter || !has_digit) 
        {
            printf("密码必须同时包含字母和数字。\n");
            continue;
        }
        break;
    }

    while (1)
    {
        printf("请输入开卡金额: ");
        if (scanf("%lf", &new_card->money) != 1)
        {
            int ch;
            printf("金额输入无效。\n");
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            continue;
        }
        if (new_card->money < 0)
        {
            printf("金额不能为负数。\n");
            continue;
        }
        break;
    }

    new_card->state = 1;             // 设置为正常状态
    new_card->used_money = 0;        // 累计使用金额初始化为0
    new_card->use_count = 0;         // 使用次数初始化为0
    new_card->last_time[0] = '\0';   // 上次使用时间初始化为空字符串
    new_card->next = NULL;           // 新卡的下一个指针初始化为NULL

    // 账单信息与卡片信息同步
    strcpy(new_billing->cardID, new_card->cardID);  
    strcpy(new_billing->code, new_card->code);      
    new_billing->money = new_card->money;
    new_billing->amount_money = 0;
    new_billing->state = 1;
    new_billing->use_count = 0;
    new_billing->last_time[0] = '\0';
    new_billing->nStatus = 1;
    new_billing->next = NULL;
    
    // 登录记录与卡片信息同步
    strcpy(new_login->cardID, new_card->cardID);
    new_login->money = new_card->money;
    new_login->used_money = 0;
    new_login->use_count = 0;
    new_login->login_time[0] = '\0';
    new_login->next = NULL;
    
    // 结算记录与卡片信息同步
    strcpy(new_settle->cardID, new_card->cardID);
    new_settle->money = new_card->money;
    new_settle->used_money = 0;
    new_settle->use_count = 0;
    new_settle->settle_time[0] = '\0';
    new_settle->nStatus = 1;
    new_settle->next = NULL;

    // 将新卡添加到链表末尾
    if (card_head == NULL)
    {
        card_head = new_card;
        billing_head = new_billing;
        login_head = new_login;
        settle_head = new_settle;
    }
    else
    {
        // 找到链表的末尾并添加新节点
        Card *card_tail = card_head;             
        Billing *billing_tail = billing_head;
        Login *login_tail = login_head;
        Settle *settle_tail = settle_head;

        // 遍历到链表的末尾
        while (card_tail->next != NULL) card_tail = card_tail->next;
        while (billing_tail->next != NULL) billing_tail = billing_tail->next;
        while (login_tail->next != NULL) login_tail = login_tail->next;
        while (settle_tail->next != NULL) settle_tail = settle_tail->next;

        // 将新节点添加到链表末尾
        card_tail->next = new_card;
        billing_tail->next = new_billing;
        login_tail->next = new_login;
        settle_tail->next = new_settle;
    }

    count++;  // 增加卡片数量

    printf("---------添加卡信息成功！---------\n");

    printf("+--------+------------------+---------------+------------+\n");
    printf("| 卡号   | 密码             |  余 额        | 状态       |\n");
    printf("+--------+------------------+---------------+------------+\n");

    printf("| %-6s | %-16s | %-10.2fRMB | %-13s |\n",
         new_card->cardID,  new_card->code, new_card->money, "已 激 活");
    printf("+--------+------------------+---------------+------------+\n");

    system("pause");
    system("cls");
}

void change_password(Card *current_card)
{
    char new_password[50];

    while (1)
    {
        int has_letter = 0; // 字母数
        int has_digit = 0;  // 数字数
        int i;

        printf("请确认原密码(字母+数字，最长15位): ");
        char old_password[50];
        scanf("%49s", old_password);
        if (strcmp(current_card->code, old_password) != 0)
        {
            printf("原密码错误。\n");
            continue;
        }

        printf("请输入新密码(字母+数字，最长15位): ");
        scanf("%49s", new_password);

        if (strlen(new_password) > 15)
        {
            printf("密码长度不能超过15位。\n");
            continue;
        }

        for (i = 0; new_password[i] != '\0'; ++i)           // 检查密码中是否包含字母和数字
        {
            if ((new_password[i] >= 'a' && new_password[i] <= 'z') ||
                (new_password[i] >= 'A' && new_password[i] <= 'Z'))
            {
                has_letter = 1;
            }
            else if (new_password[i] >= '0' && new_password[i] <= '9')
            {
                has_digit = 1;
            }
        }

        if (!has_letter || !has_digit) 
        {
            printf("密码必须同时包含字母和数字。\n");
            continue;
        }
        break;
    }

    strcpy(current_card->code, new_password); // 更新卡片的密码

    printf("密码修改成功！\n");
    system("pause");
    system("cls");
}