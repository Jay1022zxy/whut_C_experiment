#include <stdio.h>
#include <windows.h>
#include "showMenu.h"

void showMainMenu()   // 显示主菜单,身份选择
{
    printf("作者：周祥宇  数学类2502  学号：1025009088\n");
    printf("欢迎进入计费管理系统\n");
    printf("------------选择身份------------\n");
    printf("1. 管理员\n");
    printf("2. 用户\n");
    printf("0. 退出\n");
    printf("--------------------------------\n");
}

void showMasterMenu() // 显示管理员菜单
{   
    printf("作者：周祥宇  数学类2502  学号：1025009088\n");
    printf("欢迎进入管理员界面\n");
    printf("----------管理员菜单---------\n");
    printf("1. 添加卡\n");
    printf("2. 查询卡\n");
    printf("3. 上机\n");
    printf("4. 下机\n");
    printf("5. 充值\n");
    printf("6. 退费\n");
    printf("7. 查询统计\n");
    printf("8. 注销卡\n");
    printf("0. 退出\n");
    printf("----------------------------\n"); 
}

void showUserMenu()   // 显示用户菜单
{
    printf("作者：周祥宇  数学类2502  学号：1025009088\n");
    printf("欢迎进入用户界面\n");
    printf("------------用户菜单----------\n");
    printf("1. 上机\n");
    printf("2. 下机\n");
    printf("3. 查询卡信息\n");
    printf("4. 查询消费记录\n");
    printf("5. 修改密码\n");
    printf("0. 退出\n"); 
    printf("----------------------------\n"); 
}