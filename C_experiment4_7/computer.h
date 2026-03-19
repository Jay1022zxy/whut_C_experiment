#ifndef COMPUTER_H
#define COMPUTER_H

typedef struct login
{
    char cardID[10];
    double money;
    double used_money;
    int use_count;
    char login_time[50];
    struct login *next;
} Login;

typedef struct settle
{
    char cardID[10];
    double money;
    double used_money;
    int use_count;
    char settle_time[50];
    int nStatus;
    struct settle *next;
} Settle;

extern Login *login_head;    // 定义全局指针，指向登录记录链表的头节点
extern Settle *settle_head;  // 定义全局指针，指向结算记录链表的头节点

void on_computer(void);
void off_computer(void);

#endif
