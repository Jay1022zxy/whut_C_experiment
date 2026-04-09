#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <windows.h>
#include "card.h"
#include "billing.h"
#include "record.h"
#include "computer.h"
#include "storage.h"
#include "showMenu.h"
#include "find_card.h"
#include "admin_card.h"

Card *card_head = NULL;
Billing *billing_head = NULL;
Login *login_head = NULL;
Settle *settle_head = NULL;
Record *record_head = NULL;
Admin *admin_head = NULL;
int count = 0;
int recordCount = 0;

static void clear_input_buffer(void)
{
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF)
    {
    }
}

static int is_single_digit_choice(const char *choice, char min, char max) // 判断输入是否为单个数字且在指定范围内
{
    return strlen(choice) == 1 && choice[0] >= min && choice[0] <= max;
}

static int parse_choice_in_range(const char *choice, int min, int max, int *value)
{
    char *end = NULL;
    long parsed_value;

    if (choice == NULL || choice[0] == '\0')
    {
        return 0;
    }

    parsed_value = strtol(choice, &end, 10);
    if (*end != '\0' || parsed_value < min || parsed_value > max)
    {
        return 0;
    }

    *value = (int)parsed_value;
    return 1;
}

static void show_permission_denied(int permission)
{
    printf("当前管理员没有%s权限，无法执行该功能。\n", admin_permission_name(permission));
    system("pause");
    system("cls");
}

int main(void)
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, ".UTF-8");

    load_data();
    admin_card();

    while (1)
    {
        char choice[10];

        showMainMenu();
        printf("请输入菜单项编号: ");
        scanf("%9s", choice);

        if (!is_single_digit_choice(choice, '0', '2'))
        {
            printf("输入错误：请重新输入。\n");
            clear_input_buffer();
            system("pause");
            system("cls");
            continue;
        }

        switch (choice[0])
        {
            case '1':
            {
                char account[50];
                char password[50];
                Admin *current_admin;

                printf("请输入管理员账号: ");
                scanf("%49s", account);

                printf("请输入管理员密码: ");
                scanf("%49s", password);

                current_admin = validate_admin_login(account, password);
                if (current_admin != NULL)
                {
                    int admin_menu_running = 1;
                    system("cls");

                    while (admin_menu_running)
                    {
                        char master_choice[10];
                        int master_choice_value;
                        int required_permission;

                        showMasterMenu(current_admin);
                        printf("请输入管理员菜单项编号: ");
                        scanf("%9s", master_choice);

                        if (!parse_choice_in_range(master_choice, 0, 13, &master_choice_value))
                        {
                            printf("输入错误：请重新输入。\n");
                            clear_input_buffer();
                            system("pause");
                            system("cls");
                            continue;
                        }

                        required_permission = admin_required_permission_for_menu(master_choice_value);
                        if (required_permission != 0 &&
                            !admin_has_permission(current_admin, required_permission))
                        {
                            show_permission_denied(required_permission);
                            continue;
                        }

                        switch (master_choice_value)
                        {
                            case 1:
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
                                statistics();
                                break;
                            case 8:
                                cancel_card();
                                break;
                            case 9:
                                reset_password();
                                break;
                            case 10:
                                add_admin();
                                break;
                            case 11:
                                delete_admin(current_admin->account);
                                break;
                            case 12:
                                show_all_admins();
                                break;
                            case 13:
                                search_online_cards();
                                break;
                            case 0:
                                save_data();
                                admin_menu_running = 0;
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
                    printf("管理员账号或密码错误！\n");
                    system("pause");
                    system("cls");
                }
                break;
            }
            case '2':
            {
                char account[50];
                char password[50];
                Card *card_current;
                Card *current_card = NULL;
                Billing *current_billing = NULL;
                Login *current_login = NULL;
                Settle *current_settle = NULL;

                printf("请输入账号: ");
                scanf("%49s", account);

                card_current = find_card_by_id(account);
                if (card_current == NULL)
                {
                    printf("未找到该账号。\n");
                    system("pause");
                    system("cls");
                    break;
                }

                if (card_current->state == 3)
                {
                    printf("该账号已注销，无法登录。\n");
                    system("pause");
                    system("cls");
                    break;
                }

                printf("请输入密码: ");
                scanf("%49s", password);

                if (strcmp(card_current->code, password) == 0)
                {
                    int user_menu_running = 1;

                    system("cls");
                    current_card = card_current;
                    current_billing = find_billing_by_id(current_card->cardID);
                    current_login = find_login_by_id(current_card->cardID);
                    current_settle = find_settle_by_id(current_card->cardID);

                    if (current_billing == NULL || current_login == NULL || current_settle == NULL)
                    {
                        printf("用户关联数据异常，登录失败！\n");
                        system("pause");
                        system("cls");
                        break;
                    }

                    while (user_menu_running)
                    {
                        char user_choice[10];

                        showUserMenu();
                        printf("请输入用户菜单项编号: ");
                        scanf("%9s", user_choice);

                        if (!is_single_digit_choice(user_choice, '0', '7'))
                        {
                            printf("输入错误：请重新输入。\n");
                            clear_input_buffer();
                            system("pause");
                            system("cls");
                            continue;
                        }

                        switch (user_choice[0])
                        {
                            case '1':
                                on_computer_user(current_card, current_billing, current_login, current_settle);
                                break;
                            case '2':
                                off_computer_user(current_card, current_billing, current_login, current_settle);
                                break;
                            case '3':
                                search_card_user(current_card, current_billing);
                                break;
                            case '4':
                                search_records_user(current_card, current_billing);
                                break;
                            case '5':
                                change_password(current_card);
                                break;
                            case '6':
                                recharge(current_card, current_billing);
                                break;
                            case '7':
                                refund_user(current_card, current_billing);
                                break;
                            case '0':
                                save_data();
                                printf("退出系统\n");
                                user_menu_running = 0;
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
