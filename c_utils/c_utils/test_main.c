#include "dstr.h"
#include "vec.h"
#include "log.h"
#include "fs_utils.h"
#include "time_utils.h"
#include <stdio.h>
#include <stdlib.h>

#include "utest.h"
#include "uuid.h"
#include "bitset.h"

#include "argparse.h"
#include "arena.h"

int main(int argc, char **argv) {
    UTEST_BEGIN();

    // 1. 命令行解析演示
    TEST(ArgParse_Basic);
    bool verbose = false;
    int count = 0;
    const char *name = "default";
    arg_option_t options[] = {
        {ARG_BOOLEAN, 'v', "verbose", "启用详细输出", &verbose},
        {ARG_INT,     'c', "count",   "设置数量",     &count},
        {ARG_STRING,  'n', "name",    "设置名称",     &name}
    };
    argparse_t ap = {"演示程序描述", options, 3};
    // 这里模拟解析 (实际使用时传 argc, argv)
    // argparse_parse(&ap, argc, argv); 

    // 2. Arena 内存池演示
    TEST(Arena_Memory);
    arena_t *arena = arena_create(1024);
    char *p1 = arena_alloc(arena, 100);
    int *p2 = arena_alloc(arena, sizeof(int) * 50);
    EXPECT_TRUE(p1 != NULL);
    EXPECT_TRUE(p2 != NULL);
    arena_destroy(arena); // 一键释放 p1 和 p2

    // 3. UUID 演示
    TEST(UUID_Generation);
    char uid[37];
    uuid_v4(uid);
    LOG_INFO("Generated UUID: %s", uid);
    EXPECT_EQ(strlen(uid), 36);

    // 3. Bitset 演示
    TEST(Bitset_Operations);
    bitset_t *bs = bitset_create(100);
    bitset_set(bs, 10);
    bitset_set(bs, 50);
    EXPECT_TRUE(bitset_get(bs, 10));
    EXPECT_TRUE(!bitset_get(bs, 11));
    LOG_INFO("Bitset count: %zu", bitset_count(bs));
    bitset_free(bs);

    // 4. 动态字符串演示
    TEST(Dynamic_String);
    dstr s = dstr_new("Hello");
    s = dstr_append(s, " World");
    EXPECT_STR_EQ(s, "Hello World");
    dstr_free(s);

    UTEST_END();
}
