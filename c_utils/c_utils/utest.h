#ifndef C_UTILS_UTEST_H
#define C_UTILS_UTEST_H

#include <stdio.h>
#include <string.h>

static int utest_total = 0;
static int utest_failed = 0;

#define UTEST_BEGIN() printf("\x1b[36m[==========]\x1b[0m 单元测试开始\n")

#define UTEST_END() \
    printf("\x1b[36m[==========]\x1b[0m 测试结束: %d 通过, %d 失败\n", \
           utest_total - utest_failed, utest_failed); \
    return utest_failed == 0 ? 0 : 1

#define EXPECT_TRUE(cond) \
    do { \
        utest_total++; \
        if (!(cond)) { \
            printf("\x1b[31m[  FAILED  ]\x1b[0m %s:%d: 预期 (%s) 为真\n", __FILE__, __LINE__, #cond); \
            utest_failed++; \
        } \
    } while(0)

#define EXPECT_EQ(a, b) \
    do { \
        utest_total++; \
        if ((a) != (b)) { \
            printf("\x1b[31m[  FAILED  ]\x1b[0m %s:%d: 预期 %s == %s\n", __FILE__, __LINE__, #a, #b); \
            utest_failed++; \
        } \
    } while(0)

#define EXPECT_STR_EQ(a, b) \
    do { \
        utest_total++; \
        if (strcmp(a, b) != 0) { \
            printf("\x1b[31m[  FAILED  ]\x1b[0m %s:%d: 预期 \"%s\" == \"%s\"\n", __FILE__, __LINE__, a, b); \
            utest_failed++; \
        } \
    } while(0)

#define EXPECT_STREQ(a, b) EXPECT_STR_EQ(a, b)

#define EXPECT_FALSE(cond) \
    do { \
        utest_total++; \
        if (cond) { \
            printf("\x1b[31m[  FAILED  ]\x1b[0m %s:%d: 预期 (%s) 为假\n", __FILE__, __LINE__, #cond); \
            utest_failed++; \
        } \
    } while(0)

#define EXPECT_DOUBLE_EQ(a, b) \
    do { \
        utest_total++; \
        if ((a) != (b)) { \
            printf("\x1b[31m[  FAILED  ]\x1b[0m %s:%d: 预期 %s == %s (浮点数比较)\n", __FILE__, __LINE__, #a, #b); \
            utest_failed++; \
        } \
    } while(0)

#define EXPECT_NE(a, b) EXPECT_TRUE((a) != (b))
#define EXPECT_LT(a, b) EXPECT_TRUE((a) < (b))
#define EXPECT_GT(a, b) EXPECT_TRUE((a) > (b))

#define TEST(name) printf("\x1b[32m[ RUN      ]\x1b[0m %s\n", #name)

#endif // C_UTILS_UTEST_H
