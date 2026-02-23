#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../c_utils/utest.h"
#include "../c_utils/shm.h"

static const char* test_shm_name = "/test_c_utils_shm";

static void cleanup_shm() {
    shm_unlink(test_shm_name);
}

void test_shm_open_map_close() {
    TEST(Shm_OpenMapClose);
    cleanup_shm();
    
    void* ptr = shm_open_map(test_shm_name, 4096);
    EXPECT_TRUE(ptr != NULL);
    
    if (ptr) {
        shm_close_unmap(ptr, test_shm_name, 4096);
    }
    
    cleanup_shm();
}

void test_shm_open_map_ex() {
    TEST(Shm_OpenMapEx);
    cleanup_shm();
    
    shm_state_t state;
    
    void* ptr = shm_open_map_ex(test_shm_name, 4096, NULL, &state);
    EXPECT_TRUE(ptr != NULL);
    
    if (ptr) {
        shm_close_unmap(ptr, test_shm_name, 4096);
    }
    
    cleanup_shm();
}

void test_shm_close_unmap_ex() {
    TEST(Shm_CloseUnmapEx);
    cleanup_shm();
    
    void* ptr = shm_open_map(test_shm_name, 4096);
    EXPECT_TRUE(ptr != NULL);
    
    if (ptr) {
        shm_state_t state;
        shm_error_t error = shm_close_unmap_ex(ptr, test_shm_name, 4096, NULL, &state);
        EXPECT_EQ(error, SHM_OK);
    }
    
    cleanup_shm();
}

int main() {
    test_shm_open_map_close();
    test_shm_open_map_ex();
    test_shm_close_unmap_ex();

    return 0;
}
