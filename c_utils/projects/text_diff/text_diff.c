#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "levenshtein.h"
#include "terminal.h"
#include "argparse.h"
#include "fs_utils.h"

#define MAX_LINE_LEN 4096
#define MAX_LINES 10000
#define CONTEXT_LINES 3

typedef enum {
    DIFF_EQUAL = 0,
    DIFF_ADD = 1,
    DIFF_DELETE = 2,
    DIFF_MODIFY = 3
} diff_type_t;

typedef struct {
    diff_type_t type;
    int old_line;
    int new_line;
    char *content;
} diff_line_t;

typedef struct {
    diff_line_t *lines;
    int count;
    int capacity;
    int additions;
    int deletions;
    int modifications;
} diff_result_t;

typedef struct {
    char **lines;
    int count;
    int capacity;
} file_lines_t;

static file_lines_t* read_file_lines(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return NULL;
    
    file_lines_t *fl = calloc(1, sizeof(file_lines_t));
    if (!fl) {
        fclose(fp);
        return NULL;
    }
    
    fl->lines = calloc(MAX_LINES, sizeof(char*));
    fl->capacity = MAX_LINES;
    
    char buffer[MAX_LINE_LEN];
    while (fgets(buffer, sizeof(buffer), fp) && fl->count < fl->capacity) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }
        fl->lines[fl->count] = malloc(len + 1);
        memcpy(fl->lines[fl->count], buffer, len + 1);
        fl->count++;
    }
    
    fclose(fp);
    return fl;
}

static void free_file_lines(file_lines_t *fl) {
    if (!fl) return;
    for (int i = 0; i < fl->count; i++) {
        free(fl->lines[i]);
    }
    free(fl->lines);
    free(fl);
}

static diff_result_t* create_diff_result(void) {
    diff_result_t *dr = calloc(1, sizeof(diff_result_t));
    if (!dr) return NULL;
    
    dr->lines = calloc(MAX_LINES * 2, sizeof(diff_line_t));
    dr->capacity = MAX_LINES * 2;
    return dr;
}

static void free_diff_result(diff_result_t *dr) {
    if (!dr) return;
    for (int i = 0; i < dr->count; i++) {
        free(dr->lines[i].content);
    }
    free(dr->lines);
    free(dr);
}

static void add_diff_line(diff_result_t *dr, diff_type_t type, int old_line, int new_line, const char *content) {
    if (dr->count >= dr->capacity) return;
    
    diff_line_t *dl = &dr->lines[dr->count++];
    dl->type = type;
    dl->old_line = old_line;
    dl->new_line = new_line;
    dl->content = strdup(content);
    
    switch (type) {
        case DIFF_ADD: dr->additions++; break;
        case DIFF_DELETE: dr->deletions++; break;
        case DIFF_MODIFY: dr->modifications++; break;
        default: break;
    }
}

static int find_lcs(file_lines_t *old_file, file_lines_t *new_file, int **lcs_table) {
    int m = old_file->count;
    int n = new_file->count;
    
    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++) {
            if (i == 0 || j == 0) {
                lcs_table[i][j] = 0;
            } else if (strcmp(old_file->lines[i-1], new_file->lines[j-1]) == 0) {
                lcs_table[i][j] = lcs_table[i-1][j-1] + 1;
            } else {
                lcs_table[i][j] = (lcs_table[i-1][j] > lcs_table[i][j-1]) ? lcs_table[i-1][j] : lcs_table[i][j-1];
            }
        }
    }
    
    return lcs_table[m][n];
}

static diff_result_t* compute_diff(file_lines_t *old_file, file_lines_t *new_file) {
    int m = old_file->count;
    int n = new_file->count;
    
    int **lcs_table = malloc((m + 1) * sizeof(int*));
    for (int i = 0; i <= m; i++) {
        lcs_table[i] = calloc(n + 1, sizeof(int));
    }
    
    find_lcs(old_file, new_file, lcs_table);
    
    diff_result_t *dr = create_diff_result();
    
    int i = m, j = n;
    diff_line_t *temp_lines = calloc(MAX_LINES * 2, sizeof(diff_line_t));
    int temp_count = 0;
    
    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && strcmp(old_file->lines[i-1], new_file->lines[j-1]) == 0) {
            temp_lines[temp_count].type = DIFF_EQUAL;
            temp_lines[temp_count].old_line = i;
            temp_lines[temp_count].new_line = j;
            temp_lines[temp_count].content = strdup(old_file->lines[i-1]);
            i--; j--;
        } else if (j > 0 && (i == 0 || lcs_table[i][j-1] >= lcs_table[i-1][j])) {
            temp_lines[temp_count].type = DIFF_ADD;
            temp_lines[temp_count].old_line = 0;
            temp_lines[temp_count].new_line = j;
            temp_lines[temp_count].content = strdup(new_file->lines[j-1]);
            j--;
        } else if (i > 0) {
            temp_lines[temp_count].type = DIFF_DELETE;
            temp_lines[temp_count].old_line = i;
            temp_lines[temp_count].new_line = 0;
            temp_lines[temp_count].content = strdup(old_file->lines[i-1]);
            i--;
        }
        temp_count++;
    }
    
    for (int k = temp_count - 1; k >= 0; k--) {
        add_diff_line(dr, temp_lines[k].type, temp_lines[k].old_line, temp_lines[k].new_line, temp_lines[k].content);
        free(temp_lines[k].content);
    }
    
    free(temp_lines);
    for (int i = 0; i <= m; i++) {
        free(lcs_table[i]);
    }
    free(lcs_table);
    
    return dr;
}

static void print_diff_result(diff_result_t *dr, bool show_color, bool show_line_numbers) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                        差异比较结果                          ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║  \033[32m+%d 添加\033[0m  |  \033[31m-%d 删除\033[0m  |  %d 相同                       ║\n", 
           dr->additions, dr->deletions, dr->count - dr->additions - dr->deletions);
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");
    
    for (int i = 0; i < dr->count; i++) {
        diff_line_t *dl = &dr->lines[i];
        
        if (show_color) {
            switch (dl->type) {
                case DIFF_ADD:
                    printf("\033[32m");
                    break;
                case DIFF_DELETE:
                    printf("\033[31m");
                    break;
                case DIFF_MODIFY:
                    printf("\033[33m");
                    break;
                default:
                    printf("\033[0m");
                    break;
            }
        }
        
        if (show_line_numbers) {
            if (dl->type == DIFF_ADD) {
                printf("+%4d        | ", dl->new_line);
            } else if (dl->type == DIFF_DELETE) {
                printf("-%4d        | ", dl->old_line);
            } else {
                printf(" %4d  %4d   | ", dl->old_line, dl->new_line);
            }
        } else {
            switch (dl->type) {
                case DIFF_ADD: printf("+ "); break;
                case DIFF_DELETE: printf("- "); break;
                default: printf("  "); break;
            }
        }
        
        printf("%s", dl->content);
        
        if (show_color) {
            printf("\033[0m");
        }
        printf("\n");
    }
}

static void print_unified_diff(diff_result_t *dr, const char *old_name, const char *new_name, int context) {
    printf("--- %s\n", old_name);
    printf("+++ %s\n", new_name);
    
    int i = 0;
    while (i < dr->count) {
        while (i < dr->count && dr->lines[i].type == DIFF_EQUAL) i++;
        
        if (i >= dr->count) break;
        
        int start = i;
        int start_context = start - context;
        if (start_context < 0) start_context = 0;
        
        int end = i;
        while (end < dr->count && dr->lines[end].type != DIFF_EQUAL) end++;
        
        int end_context = end + context;
        if (end_context > dr->count) end_context = dr->count;
        
        int old_start = 0, old_count = 0, new_start = 0, new_count = 0;
        for (int j = start_context; j < end_context; j++) {
            if (dr->lines[j].type == DIFF_DELETE || dr->lines[j].type == DIFF_EQUAL) {
                if (old_start == 0) old_start = dr->lines[j].old_line;
                old_count++;
            }
            if (dr->lines[j].type == DIFF_ADD || dr->lines[j].type == DIFF_EQUAL) {
                if (new_start == 0) new_start = dr->lines[j].new_line;
                new_count++;
            }
        }
        
        printf("@@ -%d,%d +%d,%d @@\n", old_start, old_count, new_start, new_count);
        
        for (int j = start_context; j < end_context; j++) {
            diff_line_t *dl = &dr->lines[j];
            switch (dl->type) {
                case DIFF_ADD:
                    printf("+%s\n", dl->content);
                    break;
                case DIFF_DELETE:
                    printf("-%s\n", dl->content);
                    break;
                default:
                    printf(" %s\n", dl->content);
                    break;
            }
        }
        
        i = end;
    }
}

static void print_similarity(const char *file1, const char *file2) {
    file_lines_t *f1 = read_file_lines(file1);
    file_lines_t *f2 = read_file_lines(file2);
    
    if (!f1 || !f2) {
        printf("无法读取文件\n");
        if (f1) free_file_lines(f1);
        if (f2) free_file_lines(f2);
        return;
    }
    
    printf("\n文件相似度分析:\n");
    printf("═══════════════════════════════════════════════════════════\n");
    
    int total_lines = (f1->count > f2->count) ? f1->count : f2->count;
    int matching_lines = 0;
    
    int min_count = (f1->count < f2->count) ? f1->count : f2->count;
    for (int i = 0; i < min_count; i++) {
        double sim = levenshtein_similarity(f1->lines[i], f2->lines[i]);
        if (sim > 0.9) matching_lines++;
    }
    
    double line_similarity = (double)matching_lines / total_lines * 100;
    
    char *all_text1 = malloc(MAX_LINE_LEN * MAX_LINES);
    char *all_text2 = malloc(MAX_LINE_LEN * MAX_LINES);
    all_text1[0] = '\0';
    all_text2[0] = '\0';
    
    for (int i = 0; i < f1->count && strlen(all_text1) < MAX_LINE_LEN * MAX_LINES - MAX_LINE_LEN; i++) {
        strcat(all_text1, f1->lines[i]);
        strcat(all_text1, "\n");
    }
    for (int i = 0; i < f2->count && strlen(all_text2) < MAX_LINE_LEN * MAX_LINES - MAX_LINE_LEN; i++) {
        strcat(all_text2, f2->lines[i]);
        strcat(all_text2, "\n");
    }
    
    double text_similarity = levenshtein_similarity(all_text1, all_text2) * 100;
    
    free(all_text1);
    free(all_text2);
    
    printf("  文件1: %s (%d 行)\n", file1, f1->count);
    printf("  文件2: %s (%d 行)\n", file2, f2->count);
    printf("  ─────────────────────────────────────────────────────────\n");
    printf("  行级相似度: %.2f%% (%d/%d 行匹配)\n", line_similarity, matching_lines, total_lines);
    printf("  文本相似度: %.2f%%\n", text_similarity);
    printf("═══════════════════════════════════════════════════════════\n");
    
    free_file_lines(f1);
    free_file_lines(f2);
}

static void print_help(const char *program) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                     文本差异比较工具                          ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║  用法: %s [选项] <文件1> <文件2>              ║\n", program);
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║  选项:                                                      ║\n");
    printf("║    -h, --help        显示帮助信息                           ║\n");
    printf("║    -n, --line-num    显示行号                               ║\n");
    printf("║    -u, --unified     显示统一格式差异                       ║\n");
    printf("║    -s, --similarity  显示相似度分析                         ║\n");
    printf("║    -c, --no-color    禁用彩色输出                           ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
}

int main(int argc, char **argv) {
    if (argc < 3) {
        print_help(argv[0]);
        return 1;
    }
    
    bool show_color = true;
    bool show_line_numbers = false;
    bool unified_format = false;
    bool show_similarity_only = false;
    const char *file1 = NULL;
    const char *file2 = NULL;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--line-num") == 0) {
            show_line_numbers = true;
        } else if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "--unified") == 0) {
            unified_format = true;
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--similarity") == 0) {
            show_similarity_only = true;
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--no-color") == 0) {
            show_color = false;
        } else if (argv[i][0] != '-') {
            if (!file1) file1 = argv[i];
            else if (!file2) file2 = argv[i];
        }
    }
    
    if (!file1 || !file2) {
        fprintf(stderr, "错误: 需要指定两个文件进行比较\n");
        return 1;
    }
    
    if (show_similarity_only) {
        print_similarity(file1, file2);
        return 0;
    }
    
    file_lines_t *old_file = read_file_lines(file1);
    file_lines_t *new_file = read_file_lines(file2);
    
    if (!old_file || !new_file) {
        fprintf(stderr, "错误: 无法读取文件\n");
        if (old_file) free_file_lines(old_file);
        if (new_file) free_file_lines(new_file);
        return 1;
    }
    
    diff_result_t *dr = compute_diff(old_file, new_file);
    
    if (unified_format) {
        print_unified_diff(dr, file1, file2, CONTEXT_LINES);
    } else {
        print_diff_result(dr, show_color, show_line_numbers);
    }
    
    free_diff_result(dr);
    free_file_lines(old_file);
    free_file_lines(new_file);
    
    return 0;
}
