#include <stdio.h>
#include <windows.h>
#include <string.h>
#define MAX 1000
#include "card.h"     

int count = 0; // 卡片数量

// 主程序
int main()
{
    SetConsoleOutputCP(65001); // 设置控制台输出编码为UTF-8，以支持中文显示
    int choice;  // 声明选择变量

    while (1)
    {
    // 输出一个菜单
    showMenu();
    printf("请选择菜单项编号(0~8): ");
    if (scanf("%d", &choice) != 1)
    {
        printf("输入错误：请输入数字！\n");
        int ch; 
        while ((ch = getchar()) != '\n' && ch != EOF);
        continue;
    }

    switch (choice)
    {   case 1:
            add_card();
        break;
        case 2:
            search_card();
        break;
        case 3:
            on_computer();
        break;
        case 4:
            off_computer();
        break;
        case 5:
            charge();
        break;
        case 6: 
            refund();
        break;
        case 7:
        {   printf("-----------查询统计-----------\n");
            // 查询统计的代码 statistics();
            break;
        }
        case 8:
            cancel_card();
        break;
        case 0:
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