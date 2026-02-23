#include "../c_utils/utest.h"
#include "../c_utils/ringbuffer.h"
#include <string.h>

void test_ringbuffer_create() {
    TEST(RingBuffer_Create);
    ringbuffer_t* rb = ringbuffer_create(64);
    EXPECT_TRUE(rb != NULL);
    ringbuffer_free(rb);
}

void test_ringbuffer_write_read() {
    TEST(RingBuffer_WriteRead);
    ringbuffer_t* rb = ringbuffer_create(64);
    EXPECT_TRUE(rb != NULL);
    
    const char* data = "hello";
    size_t written = ringbuffer_write(rb, data, 5);
    EXPECT_EQ(written, 5);
    
    char buffer[16] = {0};
    size_t read = ringbuffer_read(rb, buffer, 5);
    EXPECT_EQ(read, 5);
    EXPECT_STR_EQ(buffer, "hello");
    
    ringbuffer_free(rb);
}

void test_ringbuffer_full_empty() {
    TEST(RingBuffer_FullEmpty);
    ringbuffer_t* rb = ringbuffer_create(8);
    EXPECT_TRUE(rb != NULL);
    EXPECT_TRUE(ringbuffer_is_empty(rb));
    
    const char* data = "12345678";
    ringbuffer_write(rb, data, 8);
    EXPECT_TRUE(ringbuffer_is_full(rb));
    
    char buffer[8];
    ringbuffer_read(rb, buffer, 8);
    EXPECT_TRUE(ringbuffer_is_empty(rb));
    
    ringbuffer_free(rb);
}

void test_ringbuffer_wrap() {
    TEST(RingBuffer_Wrap);
    ringbuffer_t* rb = ringbuffer_create(8);
    EXPECT_TRUE(rb != NULL);
    
    ringbuffer_write(rb, "12345678", 8);
    char buffer[8];
    ringbuffer_read(rb, buffer, 4);
    
    ringbuffer_write(rb, "ABCD", 4);
    
    ringbuffer_read(rb, buffer, 8);
    EXPECT_TRUE(memcmp(buffer, "5678ABCD", 8) == 0);
    
    ringbuffer_free(rb);
}

void test_ringbuffer_available() {
    TEST(RingBuffer_Available);
    ringbuffer_t* rb = ringbuffer_create(16);
    EXPECT_TRUE(rb != NULL);
    
    EXPECT_EQ(ringbuffer_available_write(rb), 16);
    EXPECT_EQ(ringbuffer_available_read(rb), 0);
    
    ringbuffer_write(rb, "hello", 5);
    EXPECT_EQ(ringbuffer_available_write(rb), 11);
    EXPECT_EQ(ringbuffer_available_read(rb), 5);
    
    ringbuffer_free(rb);
}

int main() {
    UTEST_BEGIN();
    test_ringbuffer_create();
    test_ringbuffer_write_read();
    test_ringbuffer_full_empty();
    test_ringbuffer_wrap();
    test_ringbuffer_available();
    UTEST_END();
}
