#ifndef BILLING_H
#define BILLING_H
#define MAX 1000
extern int count;        // 声明全局变量count，表示卡片数量

typedef struct Billing
{
    char cardID[10];      // 卡号固定为5位且唯一
    char code[50];        // 密码
    double money;         // 余额
    double amount_money;  // 累计使用金额
    int state;            // 当前状态 0:未激活 1:正常 2:上机 3:注销
    int use_count;        // 使用次数
    char last_time[50];   // 上次使用时间
    int nStatus;          // 消费状态 0:未结算 1:已结算  
    
} Billing;


Billing billings[MAX]; // 定义全局数组billings
extern Billing billings[MAX]; // 声明全局数组billings

void search_card();    // 查询卡函数声明
void on_computer();    // 上机函数声明
void off_computer();   // 下机函数声明
void charge();         // 充值函数声明



#endif // BILLING_H