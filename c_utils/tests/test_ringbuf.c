#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/ringbuf.h"

void test_ringbuf_create_free() {
    TEST(RingBuf_CreateFree);
    ringbuf_t* rb = ringbuf_create(64);
    EXPECT_TRUE(rb != NULL);
    
    ringbuf_free(rb);
}

void test_ringbuf_is_empty() {
    TEST(RingBuf_IsEmpty);
    ringbuf_t* rb = ringbuf_create(64);
    EXPECT_TRUE(rb != NULL);
    
    bool empty = ringbuf_is_empty(rb);
    EXPECT_TRUE(empty);
    
    ringbuf_free(rb);
}

void test_ringbuf_write_read() {
    TEST(RingBuf_WriteRead);
    ringbuf_t* rb = ringbuf_create(64);
    EXPECT_TRUE(rb != NULL);
    
    uint8_t data[] = {1, 2, 3, 4, 5};
    size_t written = ringbuf_write(rb, data, 5);
    EXPECT_EQ(written, 5);
    
    uint8_t out[5];
    size_t read = ringbuf_read(rb, out, 5);
    EXPECT_EQ(read, 5);
    
    EXPECT_EQ(out[0], 1);
    EXPECT_EQ(out[4], 5);
    
    ringbuf_free(rb);
}

void test_ringbuf_size_avail() {
    TEST(RingBuf_SizeAvail);
    ringbuf_t* rb = ringbuf_create(64);
    EXPECT_TRUE(rb != NULL);
    
    EXPECT_EQ(ringbuf_size(rb), 0);
    EXPECT_EQ(ringbuf_avail(rb), 64);
    
    uint8_t data[] = {1, 2, 3};
    ringbuf_write(rb, data, 3);
    
    EXPECT_EQ(ringbuf_size(rb), 3);
    EXPECT_EQ(ringbuf_avail(rb), 61);
    
    ringbuf_free(rb);
}

void test_ringbuf_clear() {
    TEST(RingBuf_Clear);
    ringbuf_t* rb = ringbuf_create(64);
    EXPECT_TRUE(rb != NULL);
    
    uint8_t data[] = {1, 2, 3};
    ringbuf_write(rb, data, 3);
    
    uint8_t out[3];
    ringbuf_read(rb, out, 3);
    EXPECT_TRUE(ringbuf_is_empty(rb));
    
    ringbuf_free(rb);
}

int main() {
    test_ringbuf_create_free();
    test_ringbuf_is_empty();
    test_ringbuf_write_read();
    test_ringbuf_size_avail();
    test_ringbuf_clear();

    return 0;
}
