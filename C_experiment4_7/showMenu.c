#include <stdio.h>
#include "showMenu.h"

void showMainMenu(void)
{
    printf("作者: 周祥宇  数学2502  学号: 1025009088\n");
    printf("欢迎使用计费管理系统\n");
    printf("------------ 角色选择 ------------\n");
    printf("1. 管理员\n");
    printf("2. 用户\n");
    printf("0. 退出\n");
    printf("----------------------------------\n");
}

void showMasterMenu(const Admin *current_admin)
{
    char permission_text[64];

    printf("作者: 周祥宇  数学2502  学号: 1025009088\n");
    printf("欢迎使用管理员面板\n");

    if (current_admin != NULL)
    {
        format_admin_permissions(current_admin->permissions, permission_text, sizeof(permission_text));
        printf("当前管理员: %s (%s)\n", current_admin->account, current_admin->name);
        printf("拥有权限: %s\n", permission_text);
    }

    printf("------------- 管理员菜单 -------------\n");
    printf("1. 添加卡片        [卡务]\n");
    printf("2. 查询卡片        [卡务]\n");
    printf("3. 上机            [卡务]\n");
    printf("4. 下机            [卡务]\n");
    printf("5. 充值            [卡务]\n");
    printf("6. 退费            [卡务]\n");
    printf("7. 查询统计        [统计]\n");
    printf("8. 注销卡          [卡务]\n");
    printf("9. 重置密码        [卡务]\n");
    printf("10. 添加管理员     [管理]\n");
    printf("11. 删除管理员     [管理]\n");
    printf("12. 显示所有管理员 [管理]\n");
    printf("13. 显示在线卡片   [卡务]\n");
    printf("0. 退出\n");
    printf("--------------------------------------\n");
}

void showUserMenu(void)
{
    printf("作者: 周祥宇  数学2502  学号: 1025009088\n");
    printf("欢迎使用用户面板\n");
    printf("------------- 用户菜单 --------------\n");
    printf("1. 上机\n");
    printf("2. 下机\n");
    printf("3. 查询卡信息\n");
    printf("4. 查询消费记录\n");
    printf("5. 修改密码\n");
    printf("6. 充值\n");
    printf("7. 退费\n");
    printf("0. 退出\n");
    printf("-------------------------------------\n");
}
