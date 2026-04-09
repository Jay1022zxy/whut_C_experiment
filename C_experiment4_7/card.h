#ifndef CARD_H
#define CARD_H
#define MAX 1000
extern int count;        // 声明全局变量count，表示卡片数量
#include "billing.h"

typedef struct Card 
{
    char cardID[10];    // 卡号固定为5位且唯一
    char code[50];      // 密码
    double money;       // 余额
    double used_money;  // 累计使用金额
    int state;          // 当前状态 0:未激活 1:正常 2:上机 3:注销
    int use_count;      // 使用次数
    char last_time[50]; // 上次使用时间
    
    struct Card *next;  // 指向下一个卡片的指针，用于链表结构
} Card;

extern Card *card_head;      // 声明一个全局指针head，指向卡片链表的头节点

void add_card();       // 添加卡函数声明
void search_card();    // 查询卡函数声明
void on_computer();    // 上机函数声明
void off_computer();   // 下机函数声明
void charge();         // 充值函数声明
void refund();         // 退款函数声明
void cancel_card();    // 注销卡函数声明
void search_card_user(Card *current_card, Billing *current_billing); // 用户查询卡信息函数声明，传入当前用户的卡片和账单信息
void search_records_user(Card *current_card, Billing *current_billing); // 用户查询消费记录函数声明，传入当前用户的卡片和账单信息
void change_password(Card *current_card); // 修改密码函数声明
void recharge(Card *current_card, Billing *current_billing); // 充值函数声明，传入当前用户的卡片和账单信息
void refund_user(Card *current_card, Billing *current_billing); // 退款函数声明
void reset_password(); // 重置用户密码函数声明

#endif // CARD_H