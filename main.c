#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <time.h>  
#define MAX 1000
#include "card.h"
#include "billing.h"
#include "record.h"
#include "computer.h"     

int count = 0; // 卡片数量
int recordCount = 0; // 消费记录数量

// 主程序
int main()
{
    SetConsoleOutputCP(65001); // 设置控制台输出编码为UTF-8，以支持中文显示
    char choice[10];  // 声明选择变量

    while (1)
    {
    // 输出一个菜单
    showMenu();
    printf("请选择菜单项编号(0~8): ");
    
    scanf("%s", &choice);
    
    if (choice[0] < '0' || choice[0] > '8' || strlen(choice) != 1) 
    {
        printf("输入错误：请重新输入！\n");
        int ch; 
        while ((ch = getchar()) != '\n' && ch != EOF);

        system("pause");
        system("cls");

        continue; 
    }

    switch (choice[0])
    {   case '1':
            add_card();
        break;
        case '2':
            search_card();
        break;
        case '3':
            on_computer();
        break;
        case '4':
            off_computer();
        break;
        case '5':
            charge();
        break;
        case '6': 
            refund(); 
        break;
        case '7':
            statistics();
        break;
        case '8':
            cancel_card();
        break;
        case '0':
        {   printf("退出系统\n");
            return 0;
        }
        default:
        {   printf("输入错误，请重新输入\n");
            system("pause");
            system("cls");
            break;
        }
    }
  }
    return 0;
}