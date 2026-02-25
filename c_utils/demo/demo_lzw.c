/**
 * LZW 压缩演示程序
 *
 * 功能：
 * - 数据压缩和解压
 * - 压缩率计算
 * - 配置选项
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/lzw.h"

// 演示 1: 基本压缩
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本压缩 ===\n");

    // 测试数据 - 有重复模式的数据压缩效果好
    const char *input = "TOBEORNOTTOBEORTOBEORNOT";
    size_t input_len = strlen(input);

    printf("原始数据: \"%s\"\n", input);
    printf("原始大小: %zu 字节\n", input_len);

    unsigned char *output = malloc(input_len * 2);
    size_t output_len;

    lzw_error_t error = lzw_encode_ex((const unsigned char*)input, input_len,
                                       output, input_len * 2, &output_len, NULL);

    if (error == LZW_OK) {
        printf("压缩后大小: %zu 字节\n", output_len);
        printf("压缩率: %.2f%%\n", lzw_calculate_ratio(input_len, output_len) * 100);

        // 解压验证
        unsigned char *decoded = malloc(input_len + 1);
        size_t decoded_len;

        error = lzw_decode(output, output_len, decoded, input_len + 1, &decoded_len);
        if (error == LZW_OK) {
            decoded[decoded_len] = '\0';
            printf("解压结果: \"%s\"\n", decoded);
            printf("验证: %s\n", strcmp(input, (char*)decoded) == 0 ? "成功" : "失败");
        }

        free(decoded);
    } else {
        printf("压缩失败: %d\n", error);
    }

    free(output);
}

// 演示 2: 不同数据类型的压缩效果
static void demo_compression_types(void) {
    printf("\n=== 演示 2: 不同数据类型的压缩效果 ===\n");

    struct {
        const char *name;
        const char *data;
    } tests[] = {
        {"重复文本", "AAAAAAAAAABBBBBBBBBB"},
        {"随机文本", "qwertyuiopasdfghjkl"},
        {"自然语言", "the quick brown fox jumps over the lazy dog"},
        {"代码片段", "int main(){return 0;}int main(){return 0;}"},
        {NULL, NULL}
    };

    printf("%-15s %-12s %-12s %-12s\n", "数据类型", "原始大小", "压缩后", "压缩率");
    printf("%-15s %-12s %-12s %-12s\n", "---------------", "------------", "------------", "------------");

    for (int i = 0; tests[i].name; i++) {
        size_t input_len = strlen(tests[i].data);
        unsigned char *output = malloc(input_len * 2);
        size_t output_len;

        lzw_encode_ex((const unsigned char*)tests[i].data, input_len,
                      output, input_len * 2, &output_len, NULL);

        double ratio = lzw_calculate_ratio(input_len, output_len);
        printf("%-15s %-12zu %-12zu %-11.1f%%\n",
               tests[i].name, input_len, output_len, ratio * 100);

        free(output);
    }
}

// 演示 3: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 3: 配置选项 ===\n");

    lzw_config_t config;
    lzw_get_default_config(&config);

    printf("默认配置:\n");
    printf("  最大代码大小: %zu\n", config.max_code_size);
    printf("  初始代码大小: %zu\n", config.initial_code_size);
    printf("  最大字典大小: %zu\n", config.max_dictionary_size);
    printf("  启用重置: %s\n", config.enable_reset ? "是" : "否");
    printf("  变长编码: %s\n", config.use_variable_length ? "是" : "否");
    printf("  启用统计: %s\n", config.enable_stats ? "是" : "否");
}

// 演示 4: 算法原理
static void demo_algorithm(void) {
    printf("\n=== 演示 4: LZW 算法原理 ===\n");

    printf("LZW (Lempel-Ziv-Welch) 算法:\n\n");

    printf("编码过程:\n");
    printf("  1. 初始化字典，包含所有单字符\n");
    printf("  2. 读取输入，构建字符串\n");
    printf("  3. 如果字符串在字典中，继续扩展\n");
    printf("  4. 如果不在，输出前一个代码，添加新字符串到字典\n");
    printf("  5. 重复直到输入结束\n\n");

    printf("解码过程:\n");
    printf("  1. 初始化相同的字典\n");
    printf("  2. 读取代码，输出对应字符串\n");
    printf("  3. 将前一个字符串+当前首字符加入字典\n");
    printf("  4. 重复直到所有代码处理完毕\n\n");

    printf("示例:\n");
    printf("  输入: \"ABABABA\"\n");
    printf("  字典初始化: A=65, B=66\n");
    printf("  编码输出: 65, 66, 256(AB), 258(ABA)...\n");
}

// 演示 5: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 5: 应用场景 ===\n");

    printf("1. GIF 图像格式\n");
    printf("   - 使用 LZW 压缩图像数据\n");
    printf("   - 高效压缩连续色调图像\n");
    printf("\n");

    printf("2. TIFF 图像格式\n");
    printf("   - 可选 LZW 压缩\n");
    printf("   - 无损压缩\n");
    printf("\n");

    printf("3. PDF 文档\n");
    printf("   - 支持 LZW 压缩\n");
    printf("   - 减小文档大小\n");
    printf("\n");

    printf("4. Unix compress\n");
    printf("   - 经典的 .Z 文件格式\n");
    printf("   - 使用 LZW 算法\n");
    printf("\n");

    printf("5. V.42bis 调制解调器\n");
    printf("   - 数据压缩标准\n");
    printf("   - 提高传输效率\n");
}

// 演示 6: 优缺点分析
static void demo_pros_cons(void) {
    printf("\n=== 演示 6: 优缺点分析 ===\n");

    printf("优点:\n");
    printf("  - 无损压缩\n");
    printf("  - 自适应字典\n");
    printf("  - 不需要预先存储字典\n");
    printf("  - 解压速度快\n");
    printf("  - 适合重复模式的数据\n\n");

    printf("缺点:\n");
    printf("  - 专利问题 (已过期)\n");
    printf("  - 对小文件效果不佳\n");
    printf("  - 字典可能溢出\n");
    printf("  - 压缩率不如现代算法\n");
}

int main(void) {
    printf("========================================\n");
    printf("    LZW 压缩演示\n");
    printf("========================================\n");

    demo_basic();
    demo_compression_types();
    demo_config();
    demo_algorithm();
    demo_applications();
    demo_pros_cons();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
