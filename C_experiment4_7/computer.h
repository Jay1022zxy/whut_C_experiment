#ifndef COMPUTER_H
#define COMPUTER_H

typedef struct login // 定义上机记录结构体
{
    char cardID[10];
    double money;
    double used_money;
    int use_count;
    char login_time[50];
    struct login *next;
} Login;

typedef struct settle // 定义下机记录结构体
{
    char cardID[10];
    double money;
    double used_money;
    int use_count;
    char settle_time[50];
    int nStatus;
    struct settle *next;
} Settle;

extern Login *login_head;    // 声明全局指针，指向登录记录链表的头节点
extern Settle *settle_head;  // 声明全局指针，指向结算记录链表的头节点

void on_computer(void);
void off_computer(void);
void on_computer_user(Card *current_card, Billing *current_billing, Login *current_login, Settle *current_settle);
void off_computer_user(Card *current_card, Billing *current_billing, Login *current_login, Settle *current_settle);

#endif
