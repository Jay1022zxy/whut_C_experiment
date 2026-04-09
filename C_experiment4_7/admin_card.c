#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "admin_card.h"
#include "card.h"
#include "computer.h"
#include "find_card.h"

static void copy_text(char *dest, size_t size, const char *src)
{
    if (size == 0)
    {
        return;
    }

    if (src == NULL)
    {
        dest[0] = '\0';
        return;
    }

    strncpy(dest, src, size - 1);
    dest[size - 1] = '\0';
}

static void append_text(char *dest, size_t size, const char *src)
{
    size_t length;

    if (dest == NULL || src == NULL || size == 0)
    {
        return;
    }

    length = strlen(dest);
    if (length >= size - 1)
    {
        return;
    }

    snprintf(dest + length, size - length, "%s", src);
}

static int count_admins(void)
{
    int total = 0;
    Admin *current = admin_head;

    while (current != NULL)
    {
        total++;
        current = current->next;
    }

    return total;
}

static int count_admins_with_permission(int permission)
{
    int total = 0;
    Admin *current = admin_head;

    while (current != NULL)
    {
        if (admin_has_permission(current, permission))
        {
            total++;
        }
        current = current->next;
    }

    return total;
}

static int utf8_char_byte_count(unsigned char ch)
{
    if ((ch & 0x80) == 0)
    {
        return 1;
    }

    if ((ch & 0xE0) == 0xC0)
    {
        return 2;
    }

    if ((ch & 0xF0) == 0xE0)
    {
        return 3;
    }

    if ((ch & 0xF8) == 0xF0)
    {
        return 4;
    }

    return 1;
}

static int text_display_width(const char *text)
{
    int width = 0;
    size_t i = 0;

    if (text == NULL)
    {
        return 0;
    }

    while (text[i] != '\0')
    {
        int char_bytes;
        size_t step = 1;
        unsigned char ch = (unsigned char)text[i];

        width += (ch < 0x80) ? 1 : 2;
        char_bytes = utf8_char_byte_count(ch);
        while (step < (size_t)char_bytes && text[i + step] != '\0')
        {
            step++;
        }
        i += step;
    }

    return width;
}

static void print_table_cell(const char *text, int content_width)
{
    int padding;
    int i;
    const char *safe_text = (text != NULL) ? text : "";

    padding = content_width - text_display_width(safe_text);
    if (padding < 0)
    {
        padding = 0;
    }

    printf("| %s", safe_text);
    for (i = 0; i < padding; ++i)
    {
        putchar(' ');
    }
    putchar(' ');
}

static void print_admin_result_header(const char *status_title)
{
    printf("+----------------------+----------------------+--------------------------+----------+\n");
    print_table_cell("管理员账号", 20);
    print_table_cell("管理员昵称", 20);
    print_table_cell("拥有权限", 24);
    print_table_cell(status_title, 8);
    printf("|\n");
    printf("+----------------------+----------------------+--------------------------+----------+\n");
}

static void print_admin_result_row(const Admin *admin, const char *status)
{
    char permission_text[64];

    if (admin == NULL)
    {
        return;
    }

    format_admin_permissions(admin->permissions, permission_text, sizeof(permission_text));
    print_table_cell(admin->account, 20);
    print_table_cell(admin->name, 20);
    print_table_cell(permission_text, 24);
    print_table_cell(status, 8);
    printf("|\n");
}

static void print_admin_result_footer(void)
{
    printf("+----------------------+----------------------+--------------------------+----------+\n");
}

static int parse_permission_selection(const char *input)
{
    int permissions = 0;
    size_t i;

    if (input == NULL || input[0] == '\0')
    {
        return -1;
    }

    for (i = 0; input[i] != '\0'; ++i)
    {
        switch (input[i])
        {
            case '1':
                permissions |= ADMIN_PERMISSION_CARD;
                break;
            case '2':
                permissions |= ADMIN_PERMISSION_STATISTICS;
                break;
            case '3':
                permissions |= ADMIN_PERMISSION_MANAGE;
                break;
            default:
                return -1;
        }
    }

    return permissions;
}

static int input_admin_permissions(void)
{
    char input[16];
    int permissions;

    while (1)
    {
        printf("请选择管理员权限，可多选并直接输入编号（例如 13 表示卡务+管理）：\n");
        printf("1. 卡务权限\n");
        printf("2. 统计权限\n");
        printf("3. 管理权限\n");
        printf("请输入权限编号组合: ");
        scanf("%15s", input);

        permissions = parse_permission_selection(input);
        if (permissions > 0)
        {
            return permissions;
        }

        printf("权限输入无效，请重新选择。\n");
    }
}

int normalize_admin_permissions(int permissions)
{
    return permissions & ADMIN_PERMISSION_ALL;
}

const char *admin_permission_name(int permission) // 仅支持单一权限输入
{
    switch (permission)
    {
        case ADMIN_PERMISSION_CARD:
            return "卡务";
        case ADMIN_PERMISSION_STATISTICS:
            return "统计";
        case ADMIN_PERMISSION_MANAGE:
            return "管理";
        default:
            return "未知";
    }
}

void format_admin_permissions(int permissions, char *buffer, size_t size)
{
    int normalized_permissions = normalize_admin_permissions(permissions);
    int first = 1;

    if (buffer == NULL || size == 0)
    {
        return;
    }

    buffer[0] = '\0';

    if (normalized_permissions == 0)
    {
        copy_text(buffer, size, "无权限");
        return;
    }

    if ((normalized_permissions & ADMIN_PERMISSION_CARD) != 0)
    {
        append_text(buffer, size, admin_permission_name(ADMIN_PERMISSION_CARD));
        first = 0;
    }

    if ((normalized_permissions & ADMIN_PERMISSION_STATISTICS) != 0)
    {
        if (!first)
        {
            append_text(buffer, size, "、");
        }
        append_text(buffer, size, admin_permission_name(ADMIN_PERMISSION_STATISTICS));
        first = 0;
    }

    if ((normalized_permissions & ADMIN_PERMISSION_MANAGE) != 0)
    {
        if (!first)
        {
            append_text(buffer, size, "、");
        }
        append_text(buffer, size, admin_permission_name(ADMIN_PERMISSION_MANAGE));
    }
}

int admin_has_permission(const Admin *admin, int permission)
{
    if (admin == NULL)
    {
        return 0;
    }

    return (normalize_admin_permissions(admin->permissions) & permission) == permission;
}

int admin_required_permission_for_menu(int menu_choice)
{
    switch (menu_choice)
    {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 8:
        case 9:
        case 13:
            return ADMIN_PERMISSION_CARD;
        case 7:
            return ADMIN_PERMISSION_STATISTICS;
        case 10:
        case 11:
        case 12:
            return ADMIN_PERMISSION_MANAGE;
        default:
            return 0;
    }
}

Admin *create_admin(const char *account, const char *password, const char *name, int permissions)
{
    Admin *node = (Admin *)calloc(1, sizeof(Admin));

    if (node == NULL)
    {
        return NULL;
    }

    copy_text(node->account, sizeof(node->account), account);
    copy_text(node->password, sizeof(node->password), password);
    copy_text(node->name, sizeof(node->name), name);
    node->permissions = normalize_admin_permissions(permissions);
    node->next = NULL;

    return node;
}

void append_admin(Admin *node)
{
    Admin **tail;

    if (node == NULL)
    {
        return;
    }

    node->permissions = normalize_admin_permissions(node->permissions);
    tail = &admin_head;
    while (*tail != NULL)
    {
        tail = &((*tail)->next);
    }

    *tail = node;
    node->next = NULL;
}

void free_admin_list(void)
{
    Admin *current = admin_head;

    while (current != NULL)
    {
        Admin *next = current->next;
        free(current);
        current = next;
    }

    admin_head = NULL;
}

Admin *find_admin_by_account(const char *account)
{
    Admin *current = admin_head;

    while (current != NULL)
    {
        if (strcmp(current->account, account) == 0)
        {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

Admin *validate_admin_login(const char *account, const char *password)
{
    Admin *admin = find_admin_by_account(account);

    if (admin == NULL)
    {
        return NULL;
    }

    if (strcmp(admin->password, password) != 0)
    {
        return NULL;
    }

    return admin;
}

void ensure_default_admin(void)
{
    Admin *default_admin;

    if (admin_head != NULL)
    {
        return;
    }

    default_admin = create_admin("admin", "admin123", "system_admin", ADMIN_PERMISSION_ALL);
    if (default_admin != NULL)
    {
        append_admin(default_admin);
    }
}

void add_admin(void)
{
    char account[50];
    char password[50];
    char name[50];
    int permissions;
    Admin *new_admin;

    printf("----------- 添加管理员 -----------\n");
    printf("输入管理员账号: ");
    scanf("%49s", account);

    if (find_admin_by_account(account) != NULL)
    {
        printf("该管理员账号已存在。\n");
        system("pause");
        system("cls");
        return;
    }

    printf("输入管理员密码: ");
    scanf("%49s", password);

    printf("输入管理员昵称: ");
    scanf("%49s", name);

    permissions = input_admin_permissions();
    new_admin = create_admin(account, password, name, permissions);
    if (new_admin == NULL)
    {
        printf("添加管理员失败：内存分配错误。\n");
        system("pause");
        system("cls");
        return;
    }

    append_admin(new_admin);

    printf("管理员添加成功。\n");
    print_admin_result_header("操作结果");
    print_admin_result_row(new_admin, "添加成功");
    print_admin_result_footer();

    system("pause");
    system("cls");
}

void delete_admin(const char *current_account)
{
    char account[50];
    Admin *current = admin_head;
    Admin *previous = NULL;

    printf("---------- 删除管理员 ----------\n");

    if (count_admins() <= 1)
    {
        printf("只剩下一个管理员，不允许删除。\n");
        system("pause");
        system("cls");
        return;
    }

    printf("输入要删除的管理员账号: ");
    scanf("%49s", account);

    if (current_account != NULL && strcmp(account, current_account) == 0)
    {
        printf("当前登录的管理员不能被删除。\n");
        system("pause");
        system("cls");
        return;
    }

    while (current != NULL)
    {
        if (strcmp(current->account, account) == 0)
        {
            break;
        }

        previous = current;
        current = current->next;
    }

    if (current == NULL)
    {
        printf("未找到该管理员。\n");
        system("pause");
        system("cls");
        return;
    }

    if (admin_has_permission(current, ADMIN_PERMISSION_MANAGE) &&
        count_admins_with_permission(ADMIN_PERMISSION_MANAGE) <= 1)
    {
        printf("至少要保留一个拥有管理权限的管理员。\n");
        system("pause");
        system("cls");
        return;
    }

    if (previous == NULL)
    {
        admin_head = current->next;
    }
    else
    {
        previous->next = current->next;
    }

    printf("管理员删除成功。\n");
    print_admin_result_header("操作结果");
    print_admin_result_row(current, "删除成功");
    print_admin_result_footer();

    free(current);

    system("pause");
    system("cls");
}

void show_all_admins(void)
{
    int index = 1;
    Admin *current = admin_head;

    printf("-------- 所有管理员信息 --------\n");

    if (current == NULL)
    {
        printf("当前没有管理员信息。\n");
        system("pause");
        system("cls");
        return;
    }

    printf("+------+----------------------+----------------------+------------------------------------+\n");
    print_table_cell("序号", 4);
    print_table_cell("管理员账号", 20);
    print_table_cell("管理员昵称", 20);
    print_table_cell("拥有权限", 34);
    printf("|\n");
    printf("+------+----------------------+----------------------+------------------------------------+\n");

    while (current != NULL)
    {
        char index_text[12];
        char permission_text[64];

        format_admin_permissions(current->permissions, permission_text, sizeof(permission_text));
        snprintf(index_text, sizeof(index_text), "%d", index);
        print_table_cell(index_text, 4);
        print_table_cell(current->account, 20);
        print_table_cell(current->name, 20);
        print_table_cell(permission_text, 34);
        printf("|\n");
        index++;
        current = current->next;
    }

    printf("+------+----------------------+----------------------+------------------------------------+\n");
    printf("管理员总数: %d\n", index - 1);

    system("pause");
    system("cls");
}

void search_online_cards(void)
{
    Card *card_current = card_head;
    int found = 0;

    printf("-------- 当前在线卡片 --------\n");

    while (card_current != NULL)
    {
        if (card_current->state == 2)
        {
            Login *login_current = find_login_by_id(card_current->cardID);
            const char *login_time = "暂无记录";

            if (!found)
            {
                printf("+--------+--------+--------------+----------------------+\n");
                printf("| 卡号   | 状态   | 当前余额     | 登录时间             |\n");
                printf("+--------+--------+--------------+----------------------+\n");
            }

            if (login_current != NULL && login_current->login_time[0] != '\0')
            {
                login_time = login_current->login_time;
            }

            printf("| %-6s | %-8s | %-12.2f | %-20s |\n",
                   card_current->cardID,
                   "在线",
                   card_current->money,
                   login_time);
            found++;
        }

        card_current = card_current->next;
    }

    if (!found)
    {
        printf("当前没有在线卡片。\n");
    }
    else
    {
        printf("+--------+--------+--------------+----------------------+\n");
        printf("在线卡片总数: %d\n", found);
    }

    system("pause");
    system("cls");
}

void admin_card(void)
{
    ensure_default_admin();
}
