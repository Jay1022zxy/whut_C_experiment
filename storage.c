#include <stdio.h>
#include <string.h>
#include "card.h"
#include "billing.h"
#include "computer.h"
#include "record.h"
#include "storage.h"

#define DATA_FILE "data.txt"

static const char *text_field(const char *value)    // 将空字符串转换为"-"以便保存到文件中
{  
    return value[0] == '\0' ? "-" : value;          // 如果字符串为空，则返回"-"，否则返回原字符串
}

static void restore_text_field(char *value)         // 将"-"转换回空字符串以便在程序中使用
{
    if (strcmp(value, "-") == 0)
    {
        value[0] = '\0';
    }
}

static void trim_newline(char *line)               // 去除字符串末尾的换行符
{
    size_t len = strlen(line);

    while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))   
    {
        line[len - 1] = '\0';            // 将末尾的换行符或回车符替换为字符串结束符
        len--;                           // 更新字符串长度
    }
}

static int split_line(char *line, char *fields[], int max_fields)  // 将一行文本按照'|'分割成多个字段，并存储在fields数组中，返回字段数量
{
    int count_fields = 0;
    char *start = line;                     // 从行的开始位置开始分割字段
    char *pos;                              // 用于查找分隔符的位置

    while (count_fields < max_fields)       // 循环直到达到最大字段数量
    {
        fields[count_fields++] = start;     // 将当前字段的起始位置存储在fields数组中
        pos = strchr(start, '|');           // strchr函数用于查找分隔符'|'在字符串中的位置
        if (pos == NULL)                    // 如果没有找到分隔符，说明已经是最后一个字段了
        {
            break;
        }
        *pos = '\0';                      
        start = pos + 1;                    
    }

    return count_fields;
}

void initialize_data(void)                 // 初始化数据，将全局变量和数组重置为初始状态
// memset函数用于将cards、billings、logins、settles和records数组中的所有元素设置为0，确保它们处于初始状态
{
    count = 0;
    recordCount = 0;

    memset(cards, 0, sizeof(cards));
    memset(billings, 0, sizeof(billings));
    memset(logins, 0, sizeof(logins));
    memset(settles, 0, sizeof(settles));
    memset(records, 0, sizeof(records));
}

int load_data(void)                      // 从文件中加载数据到程序中，返回1表示成功，0表示失败
{
    FILE *file = fopen(DATA_FILE, "r");  // 以只读模式打开数据文件
    char line[256];                      // 用于存储从文件中读取的一行文本
    char *fields[10];                    // 用于存储分割后的字段，最多支持10个字段
    int field_count;
    int i;

    initialize_data();                   // 在加载数据之前先初始化数据，确保全局变量和数组处于初始状态

    if (file == NULL)
    {
        return 0;
    }

    if (fgets(line, sizeof(line), file) == NULL) // 从文件中读取第一行文本，如果读取失败则关闭文件并返回0
    {
        fclose(file);
        return 0;
    }

    trim_newline(line);                         // 去除读取的行末尾的换行符，确保后续处理时不会受到换行符的干扰
    field_count = split_line(line, fields, 2);  // 将读取的行按照'|'分割成字段，存储在fields数组中，并返回字段数量

    // 验证字段数量是否为2，且第一个字段是否为"COUNT"，第二个字段是否能够成功解析为整数，并且该整数在合理范围内（0到MAX之间）
    if (field_count != 2 || strcmp(fields[0], "COUNT") != 0 || sscanf(fields[1], "%d", &count) != 1 ||
        count < 0 || count > MAX)
    {
        fclose(file);
        initialize_data();
        return 0;
    }

    // 验证通过后，count变量就被成功解析并赋值了，可以继续加载后续的数据了
    for (i = 0; i < count; i++)
    {
        // 依次读取每个卡片的数据，包括CARD、BILLING、LOGIN和SETTLE四行数据，并进行验证和解析
        // 如果任何一行数据的格式不正确或者解析失败，都关闭文件并初始化数据后返回0
        if (fgets(line, sizeof(line), file) == NULL)
        {
            fclose(file);
            initialize_data();
            return 0;
        }

        trim_newline(line);                         // 去除读取的行末尾的换行符
        field_count = split_line(line, fields, 8);  // card行的字段数量应该是8，分别是"CARD"、cardID、code、money、used_money、state、use_count和last_time
        // 验证字段数量是否为8，且第一个字段是否为"CARD"，后续字段是否能够成功解析为对应的数据类型，并且这些数据在合理范围内
        if (field_count != 8 || strcmp(fields[0], "CARD") != 0 ||
            sscanf(fields[3], "%lf", &cards[i].money) != 1 ||
            sscanf(fields[4], "%lf", &cards[i].used_money) != 1 ||
            sscanf(fields[5], "%d", &cards[i].state) != 1 ||
            sscanf(fields[6], "%d", &cards[i].use_count) != 1)
        {
            fclose(file);
            initialize_data();
            return 0;
        }
        strcpy(cards[i].cardID, fields[1]);
        strcpy(cards[i].code, fields[2]);
        strcpy(cards[i].last_time, fields[7]);
        restore_text_field(cards[i].last_time);

        // 同样的验证和解析过程也适用于BILLING、LOGIN和SETTLE的数据行，确保每个字段都符合预期的格式和范围
        if (fgets(line, sizeof(line), file) == NULL)
        {
            fclose(file);
            initialize_data();
            return 0;
        }

        trim_newline(line);
        field_count = split_line(line, fields, 9);  // BILLING行的字段数量应该是9，分别是"BILLING"、cardID、code、money、amount_money、state、use_count、last_time和nStatus
        if (field_count != 9 || strcmp(fields[0], "BILLING") != 0 ||
            sscanf(fields[3], "%lf", &billings[i].money) != 1 ||
            sscanf(fields[4], "%lf", &billings[i].amount_money) != 1 ||
            sscanf(fields[5], "%d", &billings[i].state) != 1 ||
            sscanf(fields[6], "%d", &billings[i].use_count) != 1 ||
            sscanf(fields[8], "%d", &billings[i].nStatus) != 1)
        {
            fclose(file);
            initialize_data();
            return 0;
        }
        strcpy(billings[i].cardID, fields[1]);
        strcpy(billings[i].code, fields[2]);
        strcpy(billings[i].last_time, fields[7]);
        restore_text_field(billings[i].last_time);

        if (fgets(line, sizeof(line), file) == NULL)
        {
            fclose(file);
            initialize_data();
            return 0;
        }

        trim_newline(line);
        field_count = split_line(line, fields, 6);  // LOGIN行的字段数量应该是6，分别是"LOGIN"、cardID、money、used_money、use_count和login_time
        if (field_count != 6 || strcmp(fields[0], "LOGIN") != 0 ||
            sscanf(fields[2], "%lf", &logins[i].money) != 1 ||
            sscanf(fields[3], "%lf", &logins[i].used_money) != 1 ||
            sscanf(fields[4], "%d", &logins[i].use_count) != 1)
        {
            fclose(file);
            initialize_data();
            return 0;
        }
        strcpy(logins[i].cardID, fields[1]);
        strcpy(logins[i].login_time, fields[5]);
        restore_text_field(logins[i].login_time);
        
        if (fgets(line, sizeof(line), file) == NULL)
        {
            fclose(file);
            initialize_data();
            return 0;
        }
        trim_newline(line);
        field_count = split_line(line, fields, 7); // SETTLE行的字段数量应该是7，分别是"SETTLE"、cardID、money、used_money、use_count、settle_time和nStatus
        if (field_count != 7 || strcmp(fields[0], "SETTLE") != 0 ||
            sscanf(fields[2], "%lf", &settles[i].money) != 1 ||
            sscanf(fields[3], "%lf", &settles[i].used_money) != 1 ||
            sscanf(fields[4], "%d", &settles[i].use_count) != 1 ||
            sscanf(fields[6], "%d", &settles[i].nStatus) != 1)
        {
            fclose(file);
            initialize_data();
            return 0;
        }
        strcpy(settles[i].cardID, fields[1]);
        strcpy(settles[i].settle_time, fields[5]);
        restore_text_field(settles[i].settle_time);
    }

    if (fgets(line, sizeof(line), file) == NULL)
    {
        fclose(file);
        initialize_data();
        return 0;
    }

    trim_newline(line);
    field_count = split_line(line, fields, 2);  // 读取记录数量行，字段数量应该是2，分别是"RECORD_COUNT"和recordCount
    if (field_count != 2 || strcmp(fields[0], "RECORD_COUNT") != 0 ||
        sscanf(fields[1], "%d", &recordCount) != 1 ||
        recordCount < 0 || recordCount > MAX_RECORDS)
    {
        fclose(file);
        initialize_data();
        return 0;
    }

    for (i = 0; i < recordCount; i++)
    {
        if (fgets(line, sizeof(line), file) == NULL)
        {
            fclose(file);
            initialize_data();
            return 0;
        }

        trim_newline(line);
        field_count = split_line(line, fields, 9);  // RECORD行的字段数量应该是9，分别是"RECORD"、cardID、amount、year、month、day、hour、minute和second
        if (field_count != 9 || strcmp(fields[0], "RECORD") != 0 ||
            sscanf(fields[2], "%lf", &records[i].amount) != 1 ||
            sscanf(fields[3], "%d", &records[i].year) != 1 ||
            sscanf(fields[4], "%d", &records[i].month) != 1 ||
            sscanf(fields[5], "%d", &records[i].day) != 1 ||
            sscanf(fields[6], "%d", &records[i].hour) != 1 ||
            sscanf(fields[7], "%d", &records[i].minute) != 1 ||
            sscanf(fields[8], "%d", &records[i].second) != 1)
        {
            fclose(file);
            initialize_data();
            return 0;
        }

        strcpy(records[i].cardID, fields[1]);
    }

    fclose(file);
    return 1;
}

int save_data(void)         // 将程序中的数据保存到文件中，返回1表示成功，0表示失败
{
    FILE *file = fopen(DATA_FILE, "w");  // 以写入模式打开数据文件，如果文件不存在则创建，如果文件存在则覆盖原有内容
    int i;

    if (file == NULL)
    {
        return 0;
    }

    fprintf(file, "COUNT|%d\n", count);  // 首先写入卡片数量行，格式为"COUNT|count"，其中count是当前卡片的数量

    for (i = 0; i < count; i++)
    {
        fprintf(file, "CARD|%s|%s|%.2f|%.2f|%d|%d|%s\n",
                cards[i].cardID,
                cards[i].code,
                cards[i].money,
                cards[i].used_money,
                cards[i].state,
                cards[i].use_count,
                text_field(cards[i].last_time));

        fprintf(file, "BILLING|%s|%s|%.2f|%.2f|%d|%d|%s|%d\n",
                billings[i].cardID,
                billings[i].code,
                billings[i].money,
                billings[i].amount_money,
                billings[i].state,
                billings[i].use_count,
                text_field(billings[i].last_time),
                billings[i].nStatus);

        fprintf(file, "LOGIN|%s|%.2f|%.2f|%d|%s\n",
                logins[i].cardID,
                logins[i].money,
                logins[i].used_money,
                logins[i].use_count,
                text_field(logins[i].login_time));

        fprintf(file, "SETTLE|%s|%.2f|%.2f|%d|%s|%d\n",
                settles[i].cardID,
                settles[i].money,
                settles[i].used_money,
                settles[i].use_count,
                text_field(settles[i].settle_time),
                settles[i].nStatus);
    }

    fprintf(file, "RECORD_COUNT|%d\n", recordCount);  // 写入记录数量行，格式为"RECORD_COUNT|recordCount"，其中recordCount是当前记录的数量

    for (i = 0; i < recordCount; i++)
    {
        fprintf(file, "RECORD|%s|%.2f|%d|%d|%d|%d|%d|%d\n",
                records[i].cardID,
                records[i].amount,
                records[i].year,
                records[i].month,
                records[i].day,
                records[i].hour,
                records[i].minute,
                records[i].second);
    }

    fclose(file);
    return 1;
}
