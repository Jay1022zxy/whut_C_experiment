#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <time.h>
#define MAX 1000
#include "card.h"
#include "billing.h"
#include "record.h"
#include "computer.h"
#include "storage.h"
#include "showMenu.h"
// 定义全局变量
Card *card_head = NULL;       // 定义一个全局指针head，指向卡片链表的头节点
Billing *billing_head = NULL; // 定义全局指针，指向账单链表的头节点
Login *login_head = NULL;     // 定义全局指针，指向登录记录链表的头节点
Settle *settle_head = NULL;   // 定义全局指针，指向结算记录链表的头节点
Record *record_head = NULL;   // 定义一个全局指针record_head，指向记录链表的头节点

int count = 0;
int recordCount = 0;

int main()
{
    SetConsoleOutputCP(65001);       // 设置控制台输出为UTF-8编码
    load_data();                     // 加载数据

    while (1)
    {
        char choice[10];

        showMenu();
        printf("请选择菜单项编号(0~8): ");

        scanf("%9s", choice);

        if (choice[0] < '0' || choice[0] > '8' || strlen(choice) != 1)
        {
            printf("输入错误：请重新输入。\n");
            {
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF); // 清除输入缓冲区
            }

            system("pause");
            system("cls");
            continue;
        }

        switch (choice[0])
        {
            case '1':
                add_card();
                if (!save_data()) printf("保存到 data.txt 失败！\n");
                break;
            case '2':
                search_card();
                break;
            case '3':
                on_computer();
                if (!save_data()) printf("保存到 data.txt 失败！\n");
                break;
            case '4':
                off_computer();
                if (!save_data()) printf("保存到 data.txt 失败！\n");
                break;
            case '5':
                charge();
                if (!save_data()) printf("保存到 data.txt 失败！\n");
                break;
            case '6':
                refund();
                if (!save_data()) printf("保存到 data.txt 失败！\n");
                break;
            case '7':
                statistics();
                break;
            case '8':
                cancel_card();
                if (!save_data()) printf("保存到 data.txt 失败！\n");
                break;
            case '0':
                save_data();
                printf("退出系统\n");
                return 0;
            default:
                printf("输入错误，请重新输入\n");
                system("pause");
                system("cls");
                break;
        }
    }

    return 0;
}
// test_git_check
