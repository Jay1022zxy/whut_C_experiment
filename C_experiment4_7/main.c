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
#include "find_card.h"
// 定义全局变量
Card *card_head = NULL;       // 定义一个全局指针head，指向卡片链表的头节点
Billing *billing_head = NULL; // 定义全局指针，指向账单链表的头节点
Login *login_head = NULL;     // 定义全局指针，指向登录记录链表的头节点
Settle *settle_head = NULL;   // 定义全局指针，指向结算记录链表的头节点
Record *record_head = NULL;   // 定义一个全局指针，指向记录链表的头节点

int count = 0;
int recordCount = 0;

int main()
{
    SetConsoleOutputCP(65001);       // 设置控制台输出为UTF-8编码
    load_data();                     // 加载数据

    while (1)
    {
        char choice[10];

        showMainMenu();              // 显示主菜单

        printf("请选择菜单项编号: ");

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
            {
                printf("请输入管理员密码: ");
                char password[50];
                scanf("%49s", password);
                if (strcmp(password, "admin123") == 0) // 验证管理员密码，暂时设置为"admin123"，可以根据需要修改
                {
                    system("cls");
                    int adminMenuRunning = 1;
                    while (adminMenuRunning)  // 管理员菜单循环
                    {
                        showMasterMenu(); // 显示管理员菜单
                        printf("请输入管理员菜单项编号: ");

                        char masterChoice[10];
                        scanf("%9s", masterChoice);

                        if (masterChoice[0] < '0' || masterChoice[0] > '8' || strlen(masterChoice) != 1)
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
                        switch (masterChoice[0])
                        {
                            case '1':
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
                            case '0':       // 返回主菜单
                                system("cls");
                                save_data();
                                adminMenuRunning = 0;     // 退出管理员菜单，返回主菜单
                                break; 
                            default:
                                printf("输入错误：请重新输入。\n");
                                system("pause");
                                system("cls");
                                break;
                        }
                    }
                }
                else
                {
                    printf("密码错误！\n");
                    system("pause");
                    system("cls");
                }
                break;  
            }
            case '2':
            {
                printf("请输入账号: ");
                char account[50];
                scanf("%49s", account);

                Card *card_current = find_card_by_id(account); // 根据输入的账号在卡片链表中查找对应的卡片节点

                Card *current_card = NULL;
                Billing *current_billing = NULL;
                Login *current_login = NULL;
                Settle *current_settle = NULL;

                if (card_current == NULL)
                {
                    printf("未找到该账号。\n");
                    system("pause");
                    system("cls");
                    break;
                }

                // 判断卡号是否已注销
                if (card_current->state == 3)
                {
                    printf("该账号已注销，无法登录。\n");
                    system("pause");
                    system("cls");
                    break;
                }
                
                printf("请输入密码: ");
                char password[50];
                scanf("%49s", password);

                if (strcmp(card_current->code, password) == 0) // 验证密码
                {
                    system("cls");

                    current_card = card_current;
                    current_billing = find_billing_by_id(current_card->cardID);
                    current_login = find_login_by_id(current_card->cardID);
                    current_settle = find_settle_by_id(current_card->cardID);

                    if (current_billing == NULL || current_login == NULL || current_settle == NULL)
                    {
                        printf("用户关联数据异常，登录失败！\n");
                        current_card = NULL;
                        current_billing = NULL;
                        current_login = NULL;
                        current_settle = NULL;
                        system("pause");
                        system("cls");
                        break;
                    }

                    int userMenuRunning = 1;
                    while (userMenuRunning)    // 用户菜单循环
                    {
                        showUserMenu(); // 显示用户菜单
                        printf("请输入用户菜单项编号: ");

                        char masterChoice[10];
                        scanf("%9s", masterChoice);

                        if (masterChoice[0] < '0' || masterChoice[0] > '8' || strlen(masterChoice) != 1)
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
                        switch (masterChoice[0])
                        {
                            case '1':    // 上机功能
                                on_computer_user(current_card, current_billing, current_login, current_settle); 
                                break;
                            case '2':
                                off_computer_user(current_card, current_billing, current_login, current_settle); 
                                break;
                            case '3':
                                search_card_user(current_card, current_billing); // 查询卡信息功能，传入当前用户的卡片和账单信息
                                break;
                            case '4':
                                search_records_user(current_card, current_billing);
                                break;
                            case '5':
                                change_password(current_card);
                                break;  
                            case '0':
                                save_data();
                                printf("退出系统\n");
                                userMenuRunning = 0;   // 退出用户菜单，返回主菜单
                                system("cls");
                                break;
                            default:
                                printf("输入错误：请重新输入。\n");
                                system("pause");
                                system("cls");
                                break;
                        }
                    }
                }
                else
                {
                    printf("密码错误！\n");
                    system("pause");
                    system("cls");
                }
                break;  
            }
            case '0':
                save_data();
                printf("退出系统\n");
                return 0;
            default:
                printf("输入错误：请重新输入。\n");
                system("pause");
                system("cls");
                break;
        }
    }
    return 0;
}
