#include <stdio.h>
#include <windows.h>
#include <string.h>
#define MAX 1000

// 菜单函数
void showMenu()
{   
    printf("作者：周祥宇  学号：1025009088\n");
    printf("欢迎进入计费管理系统\n");
    printf("------------菜单------------\n");
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

// 卡的结构体
struct Card
{
    int cardID;         // 卡号固定为5位且唯一
    char code[50];      // 密码
    double money;       // 余额
    int isActive;       // 是否激活（1表示激活，0表示未激活）
    int use_count;      // 使用次数
    char last_date[20]; // 上次使用日期
    char last_time[20]; // 上次使用时间
}cards[MAX];

int count = 0; // 卡片数量

// 1. 添加卡函数
void add_Card()
{   
    printf("-----------添加卡-----------\n");
    // 1) 卡号必须为5位数字且唯一
    while (1)
    {
        int duplicate = 0;  // 检查卡号是否已存在
        printf("请输入卡号(5位数字): \n");
        if (scanf("%d", &cards[count].cardID) != 1)
        {
            printf("输入不是数字，请重新输入！\n");
            while (getchar() != '\n');  // 清除非法输入
            continue; 
        }

        if (cards[count].cardID < 10000 || cards[count].cardID > 99999)
        {
            printf("卡号格式错误，请重新输入！\n");
            continue;
        }

        for (int i = 0; i < count; i++)
        {
            if (cards[i].cardID == cards[count].cardID)
            {
                duplicate = 1;
                break;
            }
        }

        if (duplicate)
        {
            printf("卡号已存在，请重新输入！\n");
            continue;
        }
        break;
    }
    // 2) 密码必须包含字母和数字
    while (1)
    {
        int hasLetter = 0, hasDigit = 0;
        printf("请输入密码: \n");
        if (scanf("%49s", cards[count].code) != 1)
        {
            printf("密码输入失败，请重新输入！\n");
            while (getchar() != '\n');  // 清除非法输入
            continue;
        }

        for (int i = 0; cards[count].code[i] != '\0'; i++)
        {
            if ((cards[count].code[i] >= 'a' && cards[count].code[i] <= 'z') ||
                (cards[count].code[i] >= 'A' && cards[count].code[i] <= 'Z'))
            {
                hasLetter = 1;
            }
            else if (cards[count].code[i] >= '0' && cards[count].code[i] <= '9')
            {
                hasDigit = 1;
            }
        }

        if (!(hasLetter && hasDigit))
        {
            printf("密码必须包含字母和数字的组合，请重新输入！\n");
            continue;
        }

        break;
    }

    // 3) 开卡金额不能为负数
    while (1)
    {
        printf("请输入开卡金额(RMB): \n");
        if (scanf("%lf", &cards[count].money) != 1)
        {
            printf("金额输入无效，请输入数字！\n");
            while (getchar() != '\n');  // 清除非法输入
            continue;
        }

        if (cards[count].money < 0)
        {
            printf("开卡金额不能为负数，请重新输入！\n");
            continue;
        }
        break;
    }

    cards[count].isActive = 1;  // 默认激活
    cards[count].use_count = 0;
    cards[count].last_date[0] = '\0'; // 初始化为空字符串

    count++;
    printf("添加卡信息成功！\n");
    // 输出刚才添加的卡的信息
    printf("卡号: %d\n", cards[count - 1].cardID);
    printf("密码: %s\n", cards[count - 1].code);
    printf("状态: %s\n", cards[count - 1].isActive ? "激活" : "未激活");
    printf("余额: %.2lf RMB\n", cards[count - 1].money);    
    system("pause");
    system("cls");
}

// 2. 查询卡函数
void search_Card()
{
    printf("-----------查询卡-----------\n");
    int cardID;
    printf("请输入查询的卡号(5位数字): ");
    if (scanf("%d", &cardID) != 1)
    {
        printf("输入错误：请输入数字！\n");
        while (getchar() != '\n');  // 清除非法输入
        return;
    }
    
    for (int i = 0; i < count; i++)
    {
        if (cards[i].cardID == cardID)
        {
            printf("卡号: %d\n", cards[i].cardID);
            printf("状态: %s\n", cards[i].isActive ? "激活" : "未激活");
            printf("余额: %.2lf RMB\n", cards[i].money);
            printf("累计使用: %.2lf RMB\n", cards[i].money - cards[i].money); // 这里需要根据实际使用情况计算
            printf("使用次数: %d次\n", cards[i].use_count);
            if(cards[i].last_date[0] != '\0')
                printf("最后使用时间: %s\n", cards[i].last_date);
            else
                printf("最后使用时间: 暂无记录\n");
            
            system("pause");
            system("cls");
            return;
        }
    }
    printf("未查询到该卡！\n");
    system("pause");
    system("cls");
}

// 3. 上机函数
void on_Computer()
{
    printf("-----------上机-----------\n");
    printf("请输入要上机的卡号(5位数字):\n");
    int cardID;
    if (scanf("%d", &cardID) != 1)    {
        printf("输入错误：请输入数字！\n");
        while (getchar() != '\n');  // 清除非法输入
        return;
    }
    scanf("%d", &cardID);
    for (int i = 0; i < count; i++)
    {
        if (cards[i].cardID == cardID)
        {
            if (!cards[i].isActive)
            {
                printf("该卡未激活，无法上机！\n");
                system("pause");
                system("cls");
                return;
            }
            printf("请输入密码:\n");
            char inputCode[50];
            if (scanf("%49s", inputCode) != 1)
            {
                printf("密码输入失败，请重新输入！\n");
                while (getchar() != '\n');  // 清除非法输入
                return;
            }
            if (strcmp(cards[i].code, inputCode) != 0)
            {
                printf("密码错误！\n");
                system("pause");
                system("cls");
                return;
            }
            else
            {
                printf("上机成功！\n");
                // 记录上机时间
                SYSTEMTIME st;       // 获取系统时间
                GetLocalTime(&st);   // 获取当前系统时间
               
                system("pause");
                system("cls");
                return;
            }
            
            return;
        }
    }
}
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
            add_Card();
        break;
        case 2:
            search_Card();
        break;
        case 3:
            on_Computer();
        break;
        case 4:
        {   printf("-----------下机-----------\n");
            // 下机的代码 off_Computer();
            break;
        }
        case 5:
        {   printf("-----------充值-----------\n");
            // 充值的代码 charge();
            break;
        }
        case 6:
        {   printf("-----------退费-----------\n");
            // 退费的代码 refund();
            break;
        }
        case 7:
        {   printf("-----------查询统计-----------\n");
            // 查询统计的代码 statistics();
            break;
        }
        case 8:
        {   printf("-----------注销卡-----------\n");
            // 注销卡的代码 cancel_Card();
            break;
        }
        case 0:
        {   printf("退出系统\n");
            return 0;
        }
        default:
        {   printf("输入错误，请重新输入\n");
            showMenu();
            break;
        }
    }
  }
    return 0;
}