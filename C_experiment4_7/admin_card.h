#ifndef ADMIN_CARD_H
#define ADMIN_CARD_H

#include <stddef.h>

enum
{
    ADMIN_PERMISSION_CARD = 1 << 0,
    ADMIN_PERMISSION_STATISTICS = 1 << 1,
    ADMIN_PERMISSION_MANAGE = 1 << 2,
    ADMIN_PERMISSION_ALL = ADMIN_PERMISSION_CARD | ADMIN_PERMISSION_STATISTICS | ADMIN_PERMISSION_MANAGE
};

typedef struct Admin
{
    char account[50];
    char password[50];
    char name[50];
    int permissions;
    struct Admin *next;
} Admin;

extern Admin *admin_head;

Admin *create_admin(const char *account, const char *password, const char *name, int permissions);
void append_admin(Admin *node);
void free_admin_list(void);
Admin *find_admin_by_account(const char *account);
Admin *validate_admin_login(const char *account, const char *password);
void ensure_default_admin(void);
void add_admin(void);
void delete_admin(const char *current_account);
void show_all_admins(void);
void search_online_cards(void);
void admin_card(void);
int admin_has_permission(const Admin *admin, int permission);
int admin_required_permission_for_menu(int menu_choice);
int normalize_admin_permissions(int permissions);
const char *admin_permission_name(int permission);
void format_admin_permissions(int permissions, char *buffer, size_t size);

#endif // ADMIN_CARD_H
