#include "argparse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_OPTIONS 64
#define MAX_POSITIONAL 32

static bool is_valid_short_name(char c) {
    return isalnum((unsigned char)c);
}

static bool is_valid_long_name(const char *name) {
    if (!name || !*name) return false;
    if (!isalpha((unsigned char)*name)) return false;
    for (const char *p = name; *p; p++) {
        if (!isalnum((unsigned char)*p) && *p != '-') return false;
    }
    return true;
}

argparse_t* argparse_create(const char *description, const char *usage) {
    argparse_t *ap = calloc(1, sizeof(argparse_t));
    if (!ap) return NULL;
    
    ap->description = description;
    ap->usage = usage;
    ap->options = calloc(MAX_OPTIONS, sizeof(arg_option_t));
    if (!ap->options) {
        free(ap);
        return NULL;
    }
    
    ap->positional_args = calloc(MAX_POSITIONAL, sizeof(const char*));
    if (!ap->positional_args) {
        free(ap->options);
        free(ap);
        return NULL;
    }
    
    ap->option_count = 0;
    ap->positional_count = 0;
    return ap;
}

void argparse_destroy(argparse_t *ap) {
    if (!ap) return;
    free(ap->options);
    free(ap->positional_args);
    free(ap);
}

static bool add_option(argparse_t *ap, arg_option_t opt) {
    if (!ap || ap->option_count >= MAX_OPTIONS) return false;
    
    // 检查重复
    for (int i = 0; i < ap->option_count; i++) {
        if (ap->options[i].short_name == opt.short_name) return false;
        if (strcmp(ap->options[i].long_name, opt.long_name) == 0) return false;
    }
    
    ap->options[ap->option_count++] = opt;
    return true;
}

bool argparse_add_bool(argparse_t *ap, char short_name, const char *long_name, 
                       const char *help, bool *value, bool required) {
    if (!ap || !is_valid_short_name(short_name) || !is_valid_long_name(long_name) || !value) 
        return false;
    
    arg_option_t opt = {
        .type = ARG_BOOLEAN,
        .short_name = short_name,
        .long_name = long_name,
        .help = help ? help : "",
        .value = value,
        .found = false,
        .required = required,
        .default_value = NULL
    };
    *value = false;
    return add_option(ap, opt);
}

bool argparse_add_int(argparse_t *ap, char short_name, const char *long_name, 
                      const char *help, int *value, int default_val, bool required) {
    if (!ap || !is_valid_short_name(short_name) || !is_valid_long_name(long_name) || !value) 
        return false;
    
    arg_option_t opt = {
        .type = ARG_INT,
        .short_name = short_name,
        .long_name = long_name,
        .help = help ? help : "",
        .value = value,
        .found = false,
        .required = required,
        .default_value = NULL
    };
    *value = default_val;
    return add_option(ap, opt);
}

bool argparse_add_double(argparse_t *ap, char short_name, const char *long_name, 
                         const char *help, double *value, double default_val, bool required) {
    if (!ap || !is_valid_short_name(short_name) || !is_valid_long_name(long_name) || !value) 
        return false;
    
    arg_option_t opt = {
        .type = ARG_DOUBLE,
        .short_name = short_name,
        .long_name = long_name,
        .help = help ? help : "",
        .value = value,
        .found = false,
        .required = required,
        .default_value = NULL
    };
    *value = default_val;
    return add_option(ap, opt);
}

bool argparse_add_string(argparse_t *ap, char short_name, const char *long_name, 
                         const char *help, const char **value, const char *default_val, bool required) {
    if (!ap || !is_valid_short_name(short_name) || !is_valid_long_name(long_name) || !value) 
        return false;
    
    arg_option_t opt = {
        .type = ARG_STRING,
        .short_name = short_name,
        .long_name = long_name,
        .help = help ? help : "",
        .value = value,
        .found = false,
        .required = required,
        .default_value = default_val
    };
    *value = default_val;
    return add_option(ap, opt);
}

void argparse_usage(const argparse_t *ap) {
    if (!ap) return;
    
    if (ap->usage) {
        printf("用法: %s\n", ap->usage);
    } else {
        printf("用法: [选项]");
        if (ap->positional_count > 0) {
            for (int i = 0; i < ap->positional_count; i++) {
                printf(" <%s>", ap->positional_args[i]);
            }
        }
        printf("\n");
    }
}

void argparse_help(const argparse_t *ap) {
    if (!ap) return;
    
    argparse_usage(ap);
    
    if (ap->description) {
        printf("\n%s\n", ap->description);
    }
    
    if (ap->option_count > 0) {
        printf("\n选项:\n");
        for (int i = 0; i < ap->option_count; i++) {
            const arg_option_t *opt = &ap->options[i];
            char type_indicator[16] = "";
            if (opt->type == ARG_INT) strcpy(type_indicator, " INT");
            else if (opt->type == ARG_DOUBLE) strcpy(type_indicator, " FLOAT");
            else if (opt->type == ARG_STRING) strcpy(type_indicator, " STR");
            
            char required_indicator[8] = "";
            if (opt->required) strcpy(required_indicator, " *");
            
            printf("  -%c, --%-12s%s%s\n", 
                   opt->short_name, opt->long_name, opt->help, required_indicator);
            printf("      %s%s\n", type_indicator, required_indicator);
        }
    }
    
    printf("  -h, --help         显示帮助信息\n");
}

bool argparse_is_set(const argparse_t *ap, const char *long_name) {
    if (!ap || !long_name) return false;
    
    for (int i = 0; i < ap->option_count; i++) {
        if (strcmp(ap->options[i].long_name, long_name) == 0) {
            return ap->options[i].found;
        }
    }
    return false;
}

int argparse_parse(argparse_t *ap, int argc, char **argv) {
    if (!ap || !argv || argc < 1) return -1;
    
    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
        
        if (!arg) continue;
        
        if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            argparse_help(ap);
            return -1;
        }

        bool found = false;
        for (int j = 0; j < ap->option_count; j++) {
            arg_option_t *opt = &ap->options[j];
            char short_buf[3] = {'-', opt->short_name, '\0'};
            char long_buf[64];
            snprintf(long_buf, sizeof(long_buf), "--%s", opt->long_name);

            if (strcmp(arg, short_buf) == 0 || strcmp(arg, long_buf) == 0) {
                opt->found = true;
                found = true;
                
                if (opt->type == ARG_BOOLEAN) {
                    *(bool*)opt->value = true;
                } else {
                    if (i + 1 >= argc) {
                        fprintf(stderr, "错误: 选项 %s 需要参数\n", arg);
                        return -1;
                    }
                    const char *val = argv[++i];
                    
                    switch (opt->type) {
                        case ARG_INT:
                            *(int*)opt->value = atoi(val);
                            break;
                        case ARG_DOUBLE:
                            *(double*)opt->value = atof(val);
                            break;
                        case ARG_STRING:
                            *(const char**)opt->value = val;
                            break;
                        default:
                            break;
                    }
                }
                break;
            }
        }
        
        if (!found) {
            // 检查是否是位置参数
            if (arg[0] != '-') {
                if (ap->positional_count < MAX_POSITIONAL) {
                    ap->positional_args[ap->positional_count++] = arg;
                    continue;
                }
            }
            fprintf(stderr, "错误: 未知选项 %s\n", arg);
            return -1;
        }
    }
    
    // 检查必需参数
    for (int i = 0; i < ap->option_count; i++) {
        if (ap->options[i].required && !ap->options[i].found) {
            fprintf(stderr, "错误: 必需选项 --%s 未提供\n", ap->options[i].long_name);
            return -1;
        }
    }
    
    return 0;
}
