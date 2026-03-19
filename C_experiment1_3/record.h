#ifndef RECORD_H
#define RECORD_H
#define MAX 1000
#include "card.h"
#define MAX_RECORDS 10000
typedef struct
{
    char cardID[6];          // 消费卡号
    double amount;           // 消费金额
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
} Record;

extern int count;                      // 声明全局变量count，表示卡片数量

Record records[MAX_RECORDS];           // 定义一个全局数组来存储消费记录
extern Record records[MAX_RECORDS];    // 定义一个全局数组来存储消费记录

extern int recordCount;                // 声明全局变量recordCount，用于跟踪当前记录数量

#endif