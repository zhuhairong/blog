#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>

#include "sha256_tiny.h"
#include "md5.h"
#include "sha1.h"
#include "crc32.h"
#include "adler32.h"
#include "crypto.h"
#include "fs_utils.h"
#include "terminal.h"
#include "argparse.h"

#define BUFFER_SIZE 65536
#define MAX_PATH_LEN 4096
#define MAX_FILES 10000

typedef enum {
    HASH_MD5 = 0,
    HASH_SHA1 = 1,
    HASH_SHA256 = 2,
    HASH_CRC32 = 3,
    HASH_ADLER32 = 4,
    HASH_ALL = 5
} hash_algorithm_t;

typedef struct {
    char path[MAX_PATH_LEN];
    char md5[33];
    char sha1[41];
    char sha256[65];
    uint32_t crc32;
    uint32_t adler32;
    size_t file_size;
    time_t mod_time;
    bool verified;
    bool match;
} file_checksum_t;

typedef struct {
    file_checksum_t *files;
    size_t count;
    size_t capacity;
    size_t total_size;
    size_t passed;
    size_t failed;
    size_t errors;
    double elapsed_time;
} checksum_result_t;

static const char* algorithm_to_string(hash_algorithm_t algo) {
    switch (algo) {
        case HASH_MD5: return "MD5";
        case HASH_SHA1: return "SHA1";
        case HASH_SHA256: return "SHA256";
        case HASH_CRC32: return "CRC32";
        case HASH_ADLER32: return "ADLER32";
        case HASH_ALL: return "ALL";
        default: return "UNKNOWN";
    }
}

static hash_algorithm_t string_to_algorithm(const char *str) {
    if (strcasecmp(str, "md5") == 0) return HASH_MD5;
    if (strcasecmp(str, "sha1") == 0) return HASH_SHA1;
    if (strcasecmp(str, "sha256") == 0) return HASH_SHA256;
    if (strcasecmp(str, "crc32") == 0) return HASH_CRC32;
    if (strcasecmp(str, "adler32") == 0) return HASH_ADLER32;
    if (strcasecmp(str, "all") == 0) return HASH_ALL;
    return HASH_MD5;
}

static void bytes_to_hex(const uint8_t *bytes, size_t len, char *hex) {
    static const char hex_chars[] = "0123456789abcdef";
    for (size_t i = 0; i < len; i++) {
        hex[i * 2] = hex_chars[(bytes[i] >> 4) & 0x0f];
        hex[i * 2 + 1] = hex_chars[bytes[i] & 0x0f];
    }
    hex[len * 2] = '\0';
}

static bool compute_file_md5(const char *filepath, char *output) {
    FILE *fp = fopen(filepath, "rb");
    if (!fp) return false;
    
    md5_ctx_t ctx;
    md5_init(&ctx);
    
    uint8_t buffer[BUFFER_SIZE];
    size_t bytes_read;
    
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        md5_update(&ctx, buffer, bytes_read);
    }
    
    uint8_t digest[16];
    md5_final(&ctx, digest);
    bytes_to_hex(digest, 16, output);
    
    fclose(fp);
    return true;
}

static bool compute_file_sha1(const char *filepath, char *output) {
    sha1_ctx_t ctx;
    sha1_init(&ctx);
    
    FILE *fp = fopen(filepath, "rb");
    if (!fp) return false;
    
    uint8_t buffer[BUFFER_SIZE];
    size_t bytes_read;
    
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        sha1_update(&ctx, buffer, bytes_read);
    }
    
    uint8_t digest[20];
    sha1_final(&ctx, digest);
    bytes_to_hex(digest, 20, output);
    
    fclose(fp);
    return true;
}

static bool compute_file_sha256(const char *filepath, char *output) {
    FILE *fp = fopen(filepath, "rb");
    if (!fp) return false;
    
    sha256_ctx_t ctx;
    sha256_init(&ctx);
    
    uint8_t buffer[BUFFER_SIZE];
    size_t bytes_read;
    
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        sha256_update(&ctx, buffer, bytes_read);
    }
    
    uint8_t digest[32];
    sha256_final(&ctx, digest);
    bytes_to_hex(digest, 32, output);
    
    fclose(fp);
    return true;
}

static bool compute_file_crc32(const char *filepath, uint32_t *output) {
    crc32_error_t err;
    *output = crc32_compute_file(filepath, CRC32_STANDARD, &err);
    return err == CRC32_OK;
}

static bool compute_file_adler32(const char *filepath, uint32_t *output) {
    adler32_ctx_t *ctx = NULL;
    adler32_config_t config;
    memset(&config, 0, sizeof(config));
    
    adler32_error_t err = adler32_create(&ctx, &config);
    if (err != ADLER32_OK) return false;
    
    err = adler32_compute_file(ctx, filepath, output);
    adler32_destroy(ctx);
    
    return err == ADLER32_OK;
}

static bool compute_checksums(const char *filepath, file_checksum_t *fc, 
                              hash_algorithm_t algo) {
    struct stat st;
    if (stat(filepath, &st) != 0) {
        return false;
    }
    
    strncpy(fc->path, filepath, MAX_PATH_LEN - 1);
    fc->file_size = st.st_size;
    fc->mod_time = st.st_mtime;
    fc->verified = false;
    fc->match = false;
    
    bool success = true;
    
    if (algo == HASH_ALL || algo == HASH_MD5) {
        if (!compute_file_md5(filepath, fc->md5)) {
            strcpy(fc->md5, "ERROR");
            success = false;
        }
    }
    
    if (algo == HASH_ALL || algo == HASH_SHA1) {
        if (!compute_file_sha1(filepath, fc->sha1)) {
            strcpy(fc->sha1, "ERROR");
            success = false;
        }
    }
    
    if (algo == HASH_ALL || algo == HASH_SHA256) {
        if (!compute_file_sha256(filepath, fc->sha256)) {
            strcpy(fc->sha256, "ERROR");
            success = false;
        }
    }
    
    if (algo == HASH_ALL || algo == HASH_CRC32) {
        if (!compute_file_crc32(filepath, &fc->crc32)) {
            fc->crc32 = 0;
            success = false;
        }
    }
    
    if (algo == HASH_ALL || algo == HASH_ADLER32) {
        if (!compute_file_adler32(filepath, &fc->adler32)) {
            fc->adler32 = 0;
            success = false;
        }
    }
    
    return success;
}

static checksum_result_t* result_create(size_t capacity) {
    checksum_result_t *result = calloc(1, sizeof(checksum_result_t));
    if (!result) return NULL;
    
    result->files = calloc(capacity, sizeof(file_checksum_t));
    if (!result->files) {
        free(result);
        return NULL;
    }
    
    result->capacity = capacity;
    result->count = 0;
    return result;
}

static void result_free(checksum_result_t *result) {
    if (result) {
        free(result->files);
        free(result);
    }
}

static void result_add_file(checksum_result_t *result, const file_checksum_t *fc) {
    if (result->count >= result->capacity) {
        size_t new_capacity = result->capacity * 2;
        file_checksum_t *new_files = realloc(result->files, 
                                              new_capacity * sizeof(file_checksum_t));
        if (!new_files) return;
        result->files = new_files;
        result->capacity = new_capacity;
    }
    
    result->files[result->count++] = *fc;
    result->total_size += fc->file_size;
}

static checksum_result_t* compute_directory_checksums(const char *dirpath, 
                                                       hash_algorithm_t algo,
                                                       bool recursive) {
    checksum_result_t *result = result_create(256);
    if (!result) return NULL;
    
    DIR *dir = opendir(dirpath);
    if (!dir) {
        result_free(result);
        return NULL;
    }
    
    struct dirent *entry;
    char filepath[MAX_PATH_LEN];
    
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, entry->d_name);
        
        struct stat st;
        if (stat(filepath, &st) != 0) {
            continue;
        }
        
        if (S_ISREG(st.st_mode)) {
            file_checksum_t fc;
            memset(&fc, 0, sizeof(fc));
            
            if (compute_checksums(filepath, &fc, algo)) {
                result_add_file(result, &fc);
            } else {
                result->errors++;
            }
        } else if (S_ISDIR(st.st_mode) && recursive) {
            checksum_result_t *sub_result = compute_directory_checksums(filepath, algo, recursive);
            if (sub_result) {
                for (size_t i = 0; i < sub_result->count; i++) {
                    result_add_file(result, &sub_result->files[i]);
                }
                result->errors += sub_result->errors;
                result_free(sub_result);
            }
        }
    }
    
    closedir(dir);
    return result;
}

static checksum_result_t* compute_files_checksums(char **files, size_t count,
                                                   hash_algorithm_t algo) {
    checksum_result_t *result = result_create(count);
    if (!result) return NULL;
    
    for (size_t i = 0; i < count; i++) {
        struct stat st;
        if (stat(files[i], &st) != 0) {
            result->errors++;
            continue;
        }
        
        if (S_ISREG(st.st_mode)) {
            file_checksum_t fc;
            memset(&fc, 0, sizeof(fc));
            
            if (compute_checksums(files[i], &fc, algo)) {
                result_add_file(result, &fc);
            } else {
                result->errors++;
            }
        } else if (S_ISDIR(st.st_mode)) {
            checksum_result_t *sub_result = compute_directory_checksums(files[i], algo, true);
            if (sub_result) {
                for (size_t j = 0; j < sub_result->count; j++) {
                    result_add_file(result, &sub_result->files[j]);
                }
                result->errors += sub_result->errors;
                result_free(sub_result);
            }
        }
    }
    
    return result;
}

static bool write_checksum_file(const char *output_file, checksum_result_t *result,
                                hash_algorithm_t algo) {
    FILE *fp = fopen(output_file, "w");
    if (!fp) return false;
    
    fprintf(fp, "# 文件校验和报告\n");
    fprintf(fp, "# 生成时间: %s", ctime(&(time_t){time(NULL)}));
    fprintf(fp, "# 算法: %s\n", algorithm_to_string(algo));
    fprintf(fp, "# 文件数量: %zu\n", result->count);
    fprintf(fp, "# 总大小: %zu bytes\n\n", result->total_size);
    
    for (size_t i = 0; i < result->count; i++) {
        file_checksum_t *fc = &result->files[i];
        
        switch (algo) {
            case HASH_MD5:
                fprintf(fp, "%s  %s\n", fc->md5, fc->path);
                break;
            case HASH_SHA1:
                fprintf(fp, "%s  %s\n", fc->sha1, fc->path);
                break;
            case HASH_SHA256:
                fprintf(fp, "%s  %s\n", fc->sha256, fc->path);
                break;
            case HASH_CRC32:
                fprintf(fp, "%08x  %s\n", fc->crc32, fc->path);
                break;
            case HASH_ADLER32:
                fprintf(fp, "%08x  %s\n", fc->adler32, fc->path);
                break;
            case HASH_ALL:
                fprintf(fp, "File: %s\n", fc->path);
                fprintf(fp, "  Size: %zu\n", fc->file_size);
                fprintf(fp, "  MD5: %s\n", fc->md5);
                fprintf(fp, "  SHA1: %s\n", fc->sha1);
                fprintf(fp, "  SHA256: %s\n", fc->sha256);
                fprintf(fp, "  CRC32: %08x\n", fc->crc32);
                fprintf(fp, "  Adler32: %08x\n\n", fc->adler32);
                break;
        }
    }
    
    fclose(fp);
    return true;
}

static bool parse_checksum_file(const char *input_file, checksum_result_t *result,
                                hash_algorithm_t algo) {
    FILE *fp = fopen(input_file, "r");
    if (!fp) return false;
    
    char line[MAX_PATH_LEN + 100];
    
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        
        char hash[128] = {0};
        char path[MAX_PATH_LEN] = {0};
        
        if (algo == HASH_ALL) {
            if (strncmp(line, "File: ", 6) == 0) {
                file_checksum_t fc;
                memset(&fc, 0, sizeof(fc));
                sscanf(line, "File: %s", fc.path);
                
                while (fgets(line, sizeof(line), fp)) {
                    if (line[0] == '\n') break;
                    
                    if (strncmp(line, "  Size: ", 8) == 0) {
                        sscanf(line, "  Size: %zu", &fc.file_size);
                    } else if (strncmp(line, "  MD5: ", 7) == 0) {
                        sscanf(line, "  MD5: %32s", fc.md5);
                    } else if (strncmp(line, "  SHA1: ", 8) == 0) {
                        sscanf(line, "  SHA1: %40s", fc.sha1);
                    } else if (strncmp(line, "  SHA256: ", 10) == 0) {
                        sscanf(line, "  SHA256: %64s", fc.sha256);
                    } else if (strncmp(line, "  CRC32: ", 9) == 0) {
                        sscanf(line, "  CRC32: %x", &fc.crc32);
                    } else if (strncmp(line, "  Adler32: ", 11) == 0) {
                        sscanf(line, "  Adler32: %x", &fc.adler32);
                    }
                }
                
                result_add_file(result, &fc);
            }
        } else {
            if (sscanf(line, "%127s %4095s", hash, path) == 2) {
                file_checksum_t fc;
                memset(&fc, 0, sizeof(fc));
                strncpy(fc.path, path, MAX_PATH_LEN - 1);
                
                switch (algo) {
                    case HASH_MD5:
                        strncpy(fc.md5, hash, 32);
                        break;
                    case HASH_SHA1:
                        strncpy(fc.sha1, hash, 40);
                        break;
                    case HASH_SHA256:
                        strncpy(fc.sha256, hash, 64);
                        break;
                    case HASH_CRC32:
                        fc.crc32 = (uint32_t)strtoul(hash, NULL, 16);
                        break;
                    case HASH_ADLER32:
                        fc.adler32 = (uint32_t)strtoul(hash, NULL, 16);
                        break;
                    default:
                        break;
                }
                
                result_add_file(result, &fc);
            }
        }
    }
    
    fclose(fp);
    return true;
}

static checksum_result_t* verify_checksums(const char *checksum_file, 
                                           hash_algorithm_t algo) {
    checksum_result_t *expected = result_create(256);
    if (!expected) return NULL;
    
    if (!parse_checksum_file(checksum_file, expected, algo)) {
        result_free(expected);
        return NULL;
    }
    
    for (size_t i = 0; i < expected->count; i++) {
        file_checksum_t *fc = &expected->files[i];
        
        file_checksum_t actual;
        memset(&actual, 0, sizeof(actual));
        
        if (compute_checksums(fc->path, &actual, algo)) {
            fc->verified = true;
            
            bool match = false;
            switch (algo) {
                case HASH_MD5:
                    match = (strcmp(fc->md5, actual.md5) == 0);
                    break;
                case HASH_SHA1:
                    match = (strcmp(fc->sha1, actual.sha1) == 0);
                    break;
                case HASH_SHA256:
                    match = (strcmp(fc->sha256, actual.sha256) == 0);
                    break;
                case HASH_CRC32:
                    match = (fc->crc32 == actual.crc32);
                    break;
                case HASH_ADLER32:
                    match = (fc->adler32 == actual.adler32);
                    break;
                case HASH_ALL:
                    match = (strcmp(fc->md5, actual.md5) == 0 &&
                             strcmp(fc->sha1, actual.sha1) == 0 &&
                             strcmp(fc->sha256, actual.sha256) == 0 &&
                             fc->crc32 == actual.crc32 &&
                             fc->adler32 == actual.adler32);
                    break;
            }
            
            fc->match = match;
            if (match) {
                expected->passed++;
            } else {
                expected->failed++;
            }
        } else {
            fc->verified = false;
            fc->match = false;
            expected->errors++;
        }
    }
    
    return expected;
}

static void print_result(checksum_result_t *result, hash_algorithm_t algo, 
                         bool verbose) {
    printf("\n");
    term_printf(TERM_ANSI_CYAN, "═══════════════════════════════════════════════════════════════\n");
    term_printf(TERM_ANSI_CYAN, "                        校验结果报告                            \n");
    term_printf(TERM_ANSI_CYAN, "═══════════════════════════════════════════════════════════════\n");
    printf("\n");
    
    printf("算法: %s\n", algorithm_to_string(algo));
    printf("文件数量: %zu\n", result->count);
    printf("总大小: %.2f MB (%zu bytes)\n", 
           (double)result->total_size / (1024 * 1024), result->total_size);
    printf("耗时: %.3f 秒\n", result->elapsed_time);
    
    if (result->passed > 0 || result->failed > 0) {
        printf("\n验证结果:\n");
        term_printf(TERM_ANSI_GREEN, "  通过: %zu\n", result->passed);
        if (result->failed > 0) {
            term_printf(TERM_ANSI_RED, "  失败: %zu\n", result->failed);
        } else {
            printf("  失败: 0\n");
        }
    }
    
    if (result->errors > 0) {
        term_printf(TERM_ANSI_YELLOW, "  错误: %zu\n", result->errors);
    }
    
    if (verbose) {
        printf("\n文件详情:\n");
        printf("───────────────────────────────────────────────────────────────\n");
        
        for (size_t i = 0; i < result->count; i++) {
            file_checksum_t *fc = &result->files[i];
            
            if (fc->verified) {
                if (fc->match) {
                    term_printf(TERM_ANSI_GREEN, "✓ ");
                } else {
                    term_printf(TERM_ANSI_RED, "✗ ");
                }
            } else {
                term_printf(TERM_ANSI_YELLOW, "? ");
            }
            
            printf("%s", fc->path);
            
            if (verbose > 1) {
                printf(" (%.2f KB)", (double)fc->file_size / 1024);
                
                switch (algo) {
                    case HASH_MD5:
                        printf(" [%s]", fc->md5);
                        break;
                    case HASH_SHA1:
                        printf(" [%s]", fc->sha1);
                        break;
                    case HASH_SHA256:
                        printf(" [%s]", fc->sha256);
                        break;
                    case HASH_CRC32:
                        printf(" [%08x]", fc->crc32);
                        break;
                    case HASH_ADLER32:
                        printf(" [%08x]", fc->adler32);
                        break;
                    case HASH_ALL:
                        printf("\n    MD5: %s", fc->md5);
                        printf("\n    SHA1: %s", fc->sha1);
                        printf("\n    SHA256: %s", fc->sha256);
                        break;
                }
            }
            
            printf("\n");
        }
    }
    
    printf("\n");
    
    if (result->failed == 0 && result->errors == 0) {
        term_printf(TERM_ANSI_GREEN, "✓ 所有文件校验通过！\n");
    } else if (result->failed > 0) {
        term_printf(TERM_ANSI_RED, "✗ 有 %zu 个文件校验失败！\n", result->failed);
    }
}

static void print_help(const char *prog) {
    term_printf(TERM_ANSI_CYAN, "\n文件完整性校验工具 v1.0\n");
    term_printf(TERM_ANSI_CYAN, "═══════════════════════════════════════════════════════════════\n\n");
    
    printf("用法: %s <命令> [选项] <文件或目录...>\n\n", prog);
    
    printf("命令:\n");
    printf("  compute    计算文件校验和\n");
    printf("  verify     验证文件完整性\n");
    printf("  compare    比较两个文件的校验和\n\n");
    
    printf("选项:\n");
    printf("  -a, --algorithm <algo>  哈希算法 (md5, sha1, sha256, crc32, adler32, all)\n");
    printf("                          默认: sha256\n");
    printf("  -o, --output <file>     输出到校验和文件\n");
    printf("  -r, --recursive         递归处理目录\n");
    printf("  -v, --verbose           详细输出\n");
    printf("  -q, --quiet             静默模式\n");
    printf("  -h, --help              显示帮助信息\n\n");
    
    printf("示例:\n");
    printf("  %s compute -a sha256 file.txt\n", prog);
    printf("  %s compute -a md5 -r -o checksums.md5 /path/to/dir\n", prog);
    printf("  %s verify -a sha256 checksums.sha256\n", prog);
    printf("  %s compare file1.txt file2.txt\n", prog);
}

static int cmd_compute(int argc, char **argv, hash_algorithm_t algo,
                       const char *output_file, bool recursive, bool verbose) {
    if (argc < 1) {
        fprintf(stderr, "错误: 请指定文件或目录\n");
        return 1;
    }
    
    clock_t start = clock();
    
    checksum_result_t *result = compute_files_checksums(argv, argc, algo);
    if (!result) {
        fprintf(stderr, "错误: 无法计算校验和\n");
        return 1;
    }
    
    result->elapsed_time = (double)(clock() - start) / CLOCKS_PER_SEC;
    
    if (output_file) {
        if (write_checksum_file(output_file, result, algo)) {
            if (!verbose) {
                printf("校验和已保存到: %s\n", output_file);
            }
        } else {
            fprintf(stderr, "错误: 无法写入输出文件\n");
            result_free(result);
            return 1;
        }
    }
    
    print_result(result, algo, verbose);
    result_free(result);
    
    return 0;
}

static int cmd_verify(const char *checksum_file, hash_algorithm_t algo, 
                      bool verbose) {
    if (!checksum_file) {
        fprintf(stderr, "错误: 请指定校验和文件\n");
        return 1;
    }
    
    clock_t start = clock();
    
    checksum_result_t *result = verify_checksums(checksum_file, algo);
    if (!result) {
        fprintf(stderr, "错误: 无法读取校验和文件\n");
        return 1;
    }
    
    result->elapsed_time = (double)(clock() - start) / CLOCKS_PER_SEC;
    
    print_result(result, algo, verbose);
    
    int exit_code = (result->failed > 0 || result->errors > 0) ? 1 : 0;
    result_free(result);
    
    return exit_code;
}

static int cmd_compare(const char *file1, const char *file2, 
                       hash_algorithm_t algo, bool verbose) {
    if (!file1 || !file2) {
        fprintf(stderr, "错误: 请指定两个文件\n");
        return 1;
    }
    
    file_checksum_t fc1, fc2;
    memset(&fc1, 0, sizeof(fc1));
    memset(&fc2, 0, sizeof(fc2));
    
    printf("比较文件:\n");
    printf("  文件1: %s\n", file1);
    printf("  文件2: %s\n\n", file2);
    
    if (!compute_checksums(file1, &fc1, algo)) {
        fprintf(stderr, "错误: 无法计算文件1的校验和\n");
        return 1;
    }
    
    if (!compute_checksums(file2, &fc2, algo)) {
        fprintf(stderr, "错误: 无法计算文件2的校验和\n");
        return 1;
    }
    
    bool match = false;
    
    printf("校验和比较:\n");
    printf("───────────────────────────────────────────────────────────────\n");
    
    if (algo == HASH_ALL || algo == HASH_MD5) {
        match = (strcmp(fc1.md5, fc2.md5) == 0);
        if (match) {
            term_printf(TERM_ANSI_GREEN, "MD5:    ✓ 匹配\n");
        } else {
            term_printf(TERM_ANSI_RED, "MD5:    ✗ 不匹配\n");
        }
        if (verbose || !match) {
            printf("        文件1: %s\n", fc1.md5);
            printf("        文件2: %s\n", fc2.md5);
        }
    }
    
    if (algo == HASH_ALL || algo == HASH_SHA1) {
        match = (strcmp(fc1.sha1, fc2.sha1) == 0);
        if (match) {
            term_printf(TERM_ANSI_GREEN, "SHA1:   ✓ 匹配\n");
        } else {
            term_printf(TERM_ANSI_RED, "SHA1:   ✗ 不匹配\n");
        }
        if (verbose || !match) {
            printf("        文件1: %s\n", fc1.sha1);
            printf("        文件2: %s\n", fc2.sha1);
        }
    }
    
    if (algo == HASH_ALL || algo == HASH_SHA256) {
        match = (strcmp(fc1.sha256, fc2.sha256) == 0);
        if (match) {
            term_printf(TERM_ANSI_GREEN, "SHA256: ✓ 匹配\n");
        } else {
            term_printf(TERM_ANSI_RED, "SHA256: ✗ 不匹配\n");
        }
        if (verbose || !match) {
            printf("        文件1: %s\n", fc1.sha256);
            printf("        文件2: %s\n", fc2.sha256);
        }
    }
    
    if (algo == HASH_ALL || algo == HASH_CRC32) {
        match = (fc1.crc32 == fc2.crc32);
        if (match) {
            term_printf(TERM_ANSI_GREEN, "CRC32:  ✓ 匹配\n");
        } else {
            term_printf(TERM_ANSI_RED, "CRC32:  ✗ 不匹配\n");
        }
        if (verbose || !match) {
            printf("        文件1: %08x\n", fc1.crc32);
            printf("        文件2: %08x\n", fc2.crc32);
        }
    }
    
    if (algo == HASH_ALL || algo == HASH_ADLER32) {
        match = (fc1.adler32 == fc2.adler32);
        if (match) {
            term_printf(TERM_ANSI_GREEN, "Adler32: ✓ 匹配\n");
        } else {
            term_printf(TERM_ANSI_RED, "Adler32: ✗ 不匹配\n");
        }
        if (verbose || !match) {
            printf("         文件1: %08x\n", fc1.adler32);
            printf("         文件2: %08x\n", fc2.adler32);
        }
    }
    
    printf("\n文件大小:\n");
    printf("  文件1: %zu bytes\n", fc1.file_size);
    printf("  文件2: %zu bytes\n", fc2.file_size);
    
    if (fc1.file_size != fc2.file_size) {
        term_printf(TERM_ANSI_YELLOW, "  注意: 文件大小不同\n");
    }
    
    printf("\n");
    
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_help(argv[0]);
        return 1;
    }
    
    const char *command = argv[1];
    
    if (strcmp(command, "-h") == 0 || strcmp(command, "--help") == 0) {
        print_help(argv[0]);
        return 0;
    }
    
    hash_algorithm_t algo = HASH_SHA256;
    const char *output_file = NULL;
    bool recursive = false;
    bool verbose = false;
    bool quiet = false;
    
    int file_start = 2;
    
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--algorithm") == 0) {
            if (i + 1 < argc) {
                algo = string_to_algorithm(argv[++i]);
            }
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if (i + 1 < argc) {
                output_file = argv[++i];
            }
        } else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--recursive") == 0) {
            recursive = true;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "--quiet") == 0) {
            quiet = true;
        } else if (argv[i][0] != '-') {
            file_start = i;
            break;
        }
    }
    
    if (strcmp(command, "compute") == 0) {
        return cmd_compute(argc - file_start, argv + file_start, algo, 
                          output_file, recursive, verbose);
    } else if (strcmp(command, "verify") == 0) {
        const char *checksum_file = (file_start < argc) ? argv[file_start] : NULL;
        return cmd_verify(checksum_file, algo, verbose);
    } else if (strcmp(command, "compare") == 0) {
        const char *file1 = (file_start < argc) ? argv[file_start] : NULL;
        const char *file2 = (file_start + 1 < argc) ? argv[file_start + 1] : NULL;
        return cmd_compare(file1, file2, algo, verbose);
    } else {
        fprintf(stderr, "错误: 未知命令 '%s'\n", command);
        print_help(argv[0]);
        return 1;
    }
}
