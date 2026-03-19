#ifndef RECORD_H
#define RECORD_H

#define MAX_RECORDS 10000

typedef struct Record
{
    char cardID[6];
    double amount;
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    struct Record *next;
} Record;

extern Record *record_head;  // 定义一个全局指针record_head，指向记录链表的头节点
extern int recordCount;

#endif
