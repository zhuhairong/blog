#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../c_utils/utest.h"
#include "../c_utils/lockfile.h"

static const char* test_lockfile = "/tmp/test_lockfile.lock";

static void cleanup_test_files() {
    unlink(test_lockfile);
}

void test_lockfile_lock_unlock() {
    TEST(Lockfile_LockUnlock);
    cleanup_test_files();
    
    int fd = lockfile_lock(test_lockfile);
    EXPECT_TRUE(fd >= 0);
    
    lockfile_unlock(fd);
    
    cleanup_test_files();
}

void test_lockfile_try_lock() {
    TEST(Lockfile_TryLock);
    cleanup_test_files();
    
    lockfile_error_t error;
    int fd = lockfile_try_lock(test_lockfile, &error);
    EXPECT_TRUE(fd >= 0);
    EXPECT_EQ(error, LOCKFILE_OK);
    
    lockfile_unlock(fd);
    
    cleanup_test_files();
}

void test_lockfile_is_locked() {
    TEST(Lockfile_IsLocked);
    cleanup_test_files();
    
    lockfile_error_t error;
    bool locked = lockfile_is_locked(test_lockfile, &error);
    EXPECT_FALSE(locked);
    
    int fd = lockfile_lock(test_lockfile);
    
    locked = lockfile_is_locked(test_lockfile, &error);
    EXPECT_TRUE(locked);
    
    lockfile_unlock(fd);
    
    cleanup_test_files();
}

void test_lockfile_default_config() {
    TEST(Lockfile_DefaultConfig);
    lockfile_config_t config;
    lockfile_get_default_config(&config);
    
    EXPECT_TRUE(config.blocking || config.non_blocking);
}

void test_lockfile_lock_ex() {
    TEST(Lockfile_LockEx);
    cleanup_test_files();
    
    lockfile_config_t config;
    lockfile_get_default_config(&config);
    config.non_blocking = true;
    
    lockfile_error_t error;
    int fd = lockfile_lock_ex(test_lockfile, &config, &error);
    EXPECT_TRUE(fd >= 0);
    EXPECT_EQ(error, LOCKFILE_OK);
    
    lockfile_unlock(fd);
    
    cleanup_test_files();
}

void test_lockfile_double_lock() {
    TEST(Lockfile_DoubleLock);
    cleanup_test_files();
    
    int fd1 = lockfile_lock(test_lockfile);
    EXPECT_TRUE(fd1 >= 0);
    
    lockfile_error_t error;
    int fd2 = lockfile_try_lock(test_lockfile, &error);
    EXPECT_TRUE(fd2 < 0 || error == LOCKFILE_ALREADY_LOCKED);
    
    lockfile_unlock(fd1);
    
    cleanup_test_files();
}

void test_lockfile_get_status() {
    TEST(Lockfile_GetStatus);
    cleanup_test_files();
    
    bool is_locked;
    pid_t lock_owner;
    lockfile_error_t error = lockfile_get_status(test_lockfile, &is_locked, &lock_owner);
    EXPECT_TRUE(error == LOCKFILE_OK || error == LOCKFILE_FILE_ERROR);
    
    cleanup_test_files();
}

void test_lockfile_force_unlock() {
    TEST(Lockfile_ForceUnlock);
    cleanup_test_files();
    
    int fd = lockfile_lock(test_lockfile);
    EXPECT_TRUE(fd >= 0);
    
    lockfile_error_t error = lockfile_force_unlock(test_lockfile);
    EXPECT_EQ(error, LOCKFILE_OK);
    
    error = lockfile_get_status(test_lockfile, NULL, NULL);
    
    cleanup_test_files();
}

int main() {
    test_lockfile_lock_unlock();
    test_lockfile_try_lock();
    test_lockfile_is_locked();
    test_lockfile_default_config();
    test_lockfile_lock_ex();
    test_lockfile_double_lock();
    test_lockfile_get_status();
    test_lockfile_force_unlock();

    return 0;
}
