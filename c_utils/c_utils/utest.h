#ifndef C_UTILS_UTEST_H
#define C_UTILS_UTEST_H

#include <stdio.h>
#include <string.h>
#include <math.h>

static int utest_total = 0;
static int utest_failed = 0;
static int utest_initialized = 0;

#define UTEST_BEGIN() \
    do { \
        utest_initialized = 1; \
        utest_total = 0; \
        utest_failed = 0; \
        printf("\x1b[36m[==========]\x1b[0m 单元测试开始\n"); \
    } while(0)

#define UTEST_END() \
    do { \
        printf("\x1b[36m[==========]\x1b[0m 测试结束: %d 通过, %d 失败\n", \
               utest_total - utest_failed, utest_failed); \
    } while(0); \
    return utest_failed == 0 ? 0 : 1

#define UTEST_SUMMARY() \
    printf("\x1b[36m[==========]\x1b[0m 测试结束: %d 通过, %d 失败\n", \
           utest_total - utest_failed, utest_failed)

/* Internal: check that UTEST_BEGIN() was called before any assertion */
#define UTEST_CHECK_INIT_() \
    do { \
        if (!utest_initialized) { \
            fprintf(stderr, "\x1b[31m[  FATAL   ]\x1b[0m %s:%d: 断言在 UTEST_BEGIN() 之前调用\n", __FILE__, __LINE__); \
            utest_total++; \
            utest_failed++; \
        } \
    } while(0)

#define EXPECT_TRUE(cond) \
    do { \
        UTEST_CHECK_INIT_(); \
        utest_total++; \
        if (!(cond)) { \
            printf("\x1b[31m[  FAILED  ]\x1b[0m %s:%d: 预期 (%s) 为真\n", __FILE__, __LINE__, #cond); \
            utest_failed++; \
        } \
    } while(0)

#define EXPECT_EQ(a, b) \
    do { \
        UTEST_CHECK_INIT_(); \
        utest_total++; \
        if ((a) != (b)) { \
            printf("\x1b[31m[  FAILED  ]\x1b[0m %s:%d: 预期 %s == %s (实际值: %lld vs %lld)\n", \
                   __FILE__, __LINE__, #a, #b, (long long)(a), (long long)(b)); \
            utest_failed++; \
        } \
    } while(0)

#define EXPECT_STR_EQ(a, b) \
    do { \
        UTEST_CHECK_INIT_(); \
        utest_total++; \
        if (strcmp((a), (b)) != 0) { \
            printf("\x1b[31m[  FAILED  ]\x1b[0m %s:%d: 预期 \"%s\" == \"%s\" (实际值: \"%s\" vs \"%s\")\n", \
                   __FILE__, __LINE__, #a, #b, \
                   (const char*)(a) ? (const char*)(a) : "(null)", \
                   (const char*)(b) ? (const char*)(b) : "(null)"); \
            utest_failed++; \
        } \
    } while(0)

#define EXPECT_STREQ(a, b) EXPECT_STR_EQ(a, b)

#define EXPECT_FALSE(cond) \
    do { \
        UTEST_CHECK_INIT_(); \
        utest_total++; \
        if (cond) { \
            printf("\x1b[31m[  FAILED  ]\x1b[0m %s:%d: 预期 (%s) 为假\n", __FILE__, __LINE__, #cond); \
            utest_failed++; \
        } \
    } while(0)

#define EXPECT_DOUBLE_EQ(a, b) \
    do { \
        UTEST_CHECK_INIT_(); \
        utest_total++; \
        if (fabs((double)(a) - (double)(b)) >= 0.0001) { \
            printf("\x1b[31m[  FAILED  ]\x1b[0m %s:%d: 预期 %s == %s (浮点数比较, 值: %g vs %g)\n", \
                   __FILE__, __LINE__, #a, #b, (double)(a), (double)(b)); \
            utest_failed++; \
        } \
    } while(0)

#define EXPECT_NE(a, b) EXPECT_TRUE((a) != (b))
#define EXPECT_LT(a, b) EXPECT_TRUE((a) < (b))
#define EXPECT_GT(a, b) EXPECT_TRUE((a) > (b))
#define EXPECT_LE(a, b) EXPECT_TRUE((a) <= (b))
#define EXPECT_GE(a, b) EXPECT_TRUE((a) >= (b))

#define TEST(name) printf("\x1b[32m[ RUN      ]\x1b[0m %s\n", #name)

#endif // C_UTILS_UTEST_H
