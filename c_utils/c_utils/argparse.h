#ifndef C_UTILS_ARGPARSE_H
#define C_UTILS_ARGPARSE_H

#include <stdbool.h>

typedef enum {
    ARG_BOOLEAN,
    ARG_INT,
    ARG_STRING,
    ARG_DOUBLE,
} arg_type_t;

typedef struct {
    arg_type_t type;
    char short_name;
    const char *long_name;
    const char *help;
    void *value;      // 指向存储结果的变量
    bool found;       // 是否在命令行中找到
    bool required;    // 是否为必需参数
    const char *default_value; // 默认值（字符串形式）
} arg_option_t;

typedef struct {
    const char *description;
    const char *usage;
    arg_option_t *options;
    int option_count;
    const char **positional_args;
    int positional_count;
} argparse_t;

// 创建与初始化
argparse_t* argparse_create(const char *description, const char *usage);
void        argparse_destroy(argparse_t *ap);

// 添加选项
bool argparse_add_bool(argparse_t *ap, char short_name, const char *long_name, 
                       const char *help, bool *value, bool required);
bool argparse_add_int(argparse_t *ap, char short_name, const char *long_name, 
                      const char *help, int *value, int default_val, bool required);
bool argparse_add_double(argparse_t *ap, char short_name, const char *long_name, 
                         const char *help, double *value, double default_val, bool required);
bool argparse_add_string(argparse_t *ap, char short_name, const char *long_name, 
                         const char *help, const char **value, const char *default_val, bool required);

// 解析命令行参数
// 返回 0 表示成功，-1 表示解析错误或显示了帮助信息
int argparse_parse(argparse_t *ap, int argc, char **argv);

// 打印帮助信息
void argparse_help(const argparse_t *ap);
void argparse_usage(const argparse_t *ap);

// 检查选项是否被设置
bool argparse_is_set(const argparse_t *ap, const char *long_name);

#endif // C_UTILS_ARGPARSE_H
