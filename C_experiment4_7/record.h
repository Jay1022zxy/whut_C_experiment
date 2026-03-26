#ifndef RECORD_H
#define RECORD_H

#define MAX_RECORDS 10000

typedef struct Record // 定义消费记录结构体
{
    char cardID[10];
    double amount;
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    struct Record *next;
} Record;

extern Record *record_head;  // 声明一个全局指针record_head，指向记录链表的头节点
extern int recordCount;

#endif
