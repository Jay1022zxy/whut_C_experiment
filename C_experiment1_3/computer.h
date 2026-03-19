#ifndef COMPUTER_H
#define COMPUTER_H
#define MAX 1000
extern int count;        // 声明全局变量count，表示卡片数量

typedef struct login
{
    char cardID[10];     // 卡号固定为5位且唯一
    double money;        // 余额
    double used_money;   // 累计使用金额
    int use_count;       // 使用次数
    char login_time[50]; // 上机时间
} Login;

typedef struct settle
{
    char cardID[10];      // 卡号固定为5位且唯一
    double money;         // 余额
    double used_money;    // 累计使用金额
    int use_count;        // 使用次数
    char settle_time[50]; // 结算时间
    int nStatus;          // 消费状态 0:未结算 1:已结算
} Settle;


Login logins[MAX]; // 定义全局数组logins
Settle settles[MAX]; // 定义全局数组settles

extern Login logins[MAX]; // 声明全局数组logins
extern Settle settles[MAX]; // 声明全局数组settles

void on_computer();    // 上机函数声明
void off_computer();   // 下机函数声明

#endif // COMPUTER_H

