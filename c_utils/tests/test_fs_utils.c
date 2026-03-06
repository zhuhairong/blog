#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "../c_utils/utest.h"
#include "../c_utils/fs_utils.h"

static const char* test_dir = "/tmp/test_fs_utils_dir";
static const char* test_file = "/tmp/test_fs_utils_dir/test_file.txt";

static void setup_test_env() {
    fs_rmdir(test_dir, true, NULL);
    fs_mkdir(test_dir, true, NULL);
}

static void cleanup_test_env() {
    fs_rmdir(test_dir, true, NULL);
}

void test_fs_default_options() {
    TEST(Fs_DefaultOptions);
    fs_options_t opts = fs_default_options();
    EXPECT_TRUE(opts.buffer_size > 0);
    EXPECT_TRUE(opts.follow_symlinks == true || opts.follow_symlinks == false);
    EXPECT_TRUE(opts.create_dirs == true || opts.create_dirs == false);
    EXPECT_TRUE(opts.atomic_write == true || opts.atomic_write == false);
    EXPECT_TRUE(opts.secure_permissions == true || opts.secure_permissions == false);
}

void test_fs_write_read_all() {
    TEST(Fs_WriteReadAll);
    cleanup_test_env();
    setup_test_env();
    
    const char* data = "Hello, World!";
    fs_error_t error;
    bool result = fs_write_all(test_file, data, strlen(data), NULL, &error);
    EXPECT_TRUE(result);
    EXPECT_EQ(error, FS_OK);
    
    size_t size;
    char* content = fs_read_all(test_file, &size, &error);
    EXPECT_TRUE(content != NULL);
    EXPECT_EQ(size, strlen(data));
    EXPECT_EQ(memcmp(content, data, size), 0);
    
    free(content);
    cleanup_test_env();
}

void test_fs_exists() {
    TEST(Fs_Exists);
    
    fs_error_t error;
    EXPECT_TRUE(fs_exists("/tmp", &error));
    EXPECT_FALSE(fs_exists("/nonexistent_path_12345", &error));
}

void test_fs_file_size() {
    TEST(Fs_FileSize);
    setup_test_env();
    
    const char* data = "1234567890";
    fs_error_t error;
    fs_write_all(test_file, data, strlen(data), NULL, &error);
    
    long size = fs_file_size(test_file, &error);
    EXPECT_EQ(size, (long)strlen(data));
    
    cleanup_test_env();
}

void test_fs_extname() {
    TEST(Fs_Extname);
    fs_error_t error;
    
    const char* ext = fs_extname("/path/to/file.txt", &error);
    EXPECT_TRUE(ext != NULL);
    EXPECT_EQ(strcmp(ext, ".txt"), 0);
    
    ext = fs_extname("file.tar.gz", &error);
    EXPECT_TRUE(ext != NULL);
    EXPECT_EQ(strcmp(ext, ".gz"), 0);
    
    ext = fs_extname("noextension", &error);
    EXPECT_TRUE(ext != NULL);
    EXPECT_EQ(strcmp(ext, ""), 0);
    
    ext = fs_extname(".hiddenfile", &error);
    EXPECT_TRUE(ext != NULL);
    EXPECT_EQ(strcmp(ext, ""), 0);
    
    ext = fs_extname("/a/b/c/", &error);
    EXPECT_TRUE(ext != NULL);
    EXPECT_EQ(strcmp(ext, ""), 0);
}

void test_fs_basename() {
    TEST(Fs_Basename);
    fs_error_t error;
    
    const char* base = fs_basename("/path/to/file.txt", &error);
    EXPECT_TRUE(base != NULL);
    EXPECT_EQ(strcmp(base, "file.txt"), 0);
    
    base = fs_basename("/path/to/dir/", &error);
    EXPECT_TRUE(base != NULL);
    EXPECT_EQ(strcmp(base, ""), 0);
    
    base = fs_basename("file.txt", &error);
    EXPECT_TRUE(base != NULL);
    EXPECT_EQ(strcmp(base, "file.txt"), 0);
    
    base = fs_basename("/", &error);
    EXPECT_TRUE(base != NULL);
    EXPECT_EQ(strcmp(base, ""), 0);
    
    base = fs_basename("//", &error);
    EXPECT_TRUE(base != NULL);
    EXPECT_EQ(strcmp(base, ""), 0);
}

void test_fs_dirname() {
    TEST(Fs_Dirname);
    fs_error_t error;
    
    char* dir = fs_dirname("/path/to/file.txt", &error);
    EXPECT_TRUE(dir != NULL);
    EXPECT_EQ(strcmp(dir, "/path/to"), 0);
    free(dir);
    
    dir = fs_dirname("/path/to/dir/", &error);
    EXPECT_TRUE(dir != NULL);
    EXPECT_EQ(strcmp(dir, "/path/to"), 0);
    free(dir);
    
    dir = fs_dirname("file.txt", &error);
    EXPECT_TRUE(dir != NULL);
    EXPECT_EQ(strcmp(dir, "."), 0);
    free(dir);
    
    dir = fs_dirname("/", &error);
    EXPECT_TRUE(dir != NULL);
    EXPECT_EQ(strcmp(dir, "/"), 0);
    free(dir);
    
    dir = fs_dirname("//", &error);
    EXPECT_TRUE(dir != NULL);
    EXPECT_EQ(strcmp(dir, "/"), 0);
    free(dir);
}

void test_fs_mkdir_rmdir() {
    TEST(Fs_MkdirRmdir);
    fs_rmdir("/tmp/test_fs_mkdir_recursive", true, NULL);
    
    const char* new_dir = "/tmp/test_fs_mkdir_recursive/a/b/c";
    
    fs_error_t error;
    bool result = fs_mkdir(new_dir, true, &error);
    EXPECT_TRUE(result);
    EXPECT_EQ(error, FS_OK);
    if (result) {
        EXPECT_TRUE(fs_exists(new_dir, &error));
        
        result = fs_rmdir("/tmp/test_fs_mkdir_recursive", true, &error);
        EXPECT_TRUE(result);
        EXPECT_FALSE(fs_exists("/tmp/test_fs_mkdir_recursive", &error));
    }
    fs_rmdir("/tmp/test_fs_mkdir_recursive", true, NULL);
}

void test_fs_rmdir_recursive() {
    TEST(Fs_Rmdir_Recursive);
    const char* base_dir = "/tmp/test_fs_rmdir_recursive";
    const char* sub_dir1 = "/tmp/test_fs_rmdir_recursive/subdir1";
    const char* sub_dir2 = "/tmp/test_fs_rmdir_recursive/subdir1/subdir2";
    const char* file1 = "/tmp/test_fs_rmdir_recursive/file1.txt";
    const char* file2 = "/tmp/test_fs_rmdir_recursive/subdir1/file2.txt";
    const char* file3 = "/tmp/test_fs_rmdir_recursive/subdir1/subdir2/file3.txt";
    
    fs_rmdir(base_dir, true, NULL);
    
    fs_error_t error;
    EXPECT_TRUE(fs_mkdir(sub_dir2, true, &error));
    
    EXPECT_TRUE(fs_write_all(file1, "content1", 8, NULL, &error));
    EXPECT_TRUE(fs_write_all(file2, "content2", 8, NULL, &error));
    EXPECT_TRUE(fs_write_all(file3, "content3", 8, NULL, &error));
    
    EXPECT_TRUE(fs_exists(file1, &error));
    EXPECT_TRUE(fs_exists(file2, &error));
    EXPECT_TRUE(fs_exists(file3, &error));
    
    bool result = fs_rmdir(base_dir, true, &error);
    EXPECT_TRUE(result);
    EXPECT_EQ(error, FS_OK);
    
    EXPECT_FALSE(fs_exists(base_dir, &error));
    EXPECT_FALSE(fs_exists(sub_dir1, &error));
    EXPECT_FALSE(fs_exists(file1, &error));
    
    fs_rmdir(base_dir, true, NULL);
}

void test_fs_unlink() {
    TEST(Fs_Unlink);
    setup_test_env();
    
    fs_error_t error;
    fs_write_all(test_file, "test", 4, NULL, &error);
    EXPECT_TRUE(fs_exists(test_file, &error));
    
    bool result = fs_unlink(test_file, &error);
    EXPECT_TRUE(result);
    EXPECT_EQ(error, FS_OK);
    EXPECT_FALSE(fs_exists(test_file, &error));
    
    cleanup_test_env();
}

void test_fs_rename() {
    TEST(Fs_Rename);
    setup_test_env();
    
    const char* new_file = "/tmp/test_fs_utils_dir/renamed.txt";
    fs_error_t error;
    fs_write_all(test_file, "test", 4, NULL, &error);
    
    bool result = fs_rename(test_file, new_file, &error);
    EXPECT_TRUE(result);
    EXPECT_EQ(error, FS_OK);
    EXPECT_FALSE(fs_exists(test_file, &error));
    EXPECT_TRUE(fs_exists(new_file, &error));
    
    fs_unlink(new_file, &error);
    cleanup_test_env();
}

void test_fs_copy() {
    TEST(Fs_Copy);
    setup_test_env();
    
    const char* copy_file = "/tmp/test_fs_utils_dir/copy.txt";
    const char* data = "Hello, Copy!";
    fs_error_t error;
    fs_write_all(test_file, data, strlen(data), NULL, &error);
    
    bool result = fs_copy(test_file, copy_file, NULL, &error);
    EXPECT_TRUE(result);
    EXPECT_EQ(error, FS_OK);
    EXPECT_TRUE(fs_exists(copy_file, &error));
    
    size_t size;
    char* content = fs_read_all(copy_file, &size, &error);
    EXPECT_TRUE(content != NULL);
    EXPECT_EQ(size, strlen(data));
    
    free(content);
    fs_unlink(copy_file, &error);
    cleanup_test_env();
}

void test_fs_stat() {
    TEST(Fs_Stat);
    setup_test_env();
    
    const char* data = "test content for stat";
    fs_error_t error;
    fs_write_all(test_file, data, strlen(data), NULL, &error);
    
    fs_file_info_t info;
    bool result = fs_stat(test_file, &info, &error);
    EXPECT_TRUE(result);
    EXPECT_EQ(error, FS_OK);
    
    EXPECT_TRUE(info.path != NULL);
    EXPECT_EQ(info.type, FS_TYPE_REGULAR);
    EXPECT_EQ(info.size, strlen(data));
    EXPECT_TRUE(info.mtime > 0);
    EXPECT_TRUE(info.atime > 0);
    EXPECT_TRUE(info.ctime > 0);
    
    if (info.path) {
        free(info.path);
    }
    
    result = fs_stat("/tmp", &info, &error);
    EXPECT_TRUE(result);
    EXPECT_EQ(info.type, FS_TYPE_DIRECTORY);
    if (info.path) {
        free(info.path);
    }
    
    cleanup_test_env();
}

void test_fs_read_dir() {
    TEST(Fs_ReadDir);
    setup_test_env();
    
    const char* file1 = "/tmp/test_fs_utils_dir/file1.txt";
    const char* file2 = "/tmp/test_fs_utils_dir/file2.txt";
    const char* subdir = "/tmp/test_fs_utils_dir/subdir";
    
    fs_error_t error;
    fs_write_all(file1, "1", 1, NULL, &error);
    fs_write_all(file2, "2", 1, NULL, &error);
    fs_mkdir(subdir, false, &error);
    
    char** entries;
    size_t count;
    bool result = fs_read_dir(test_dir, &entries, &count, &error);
    EXPECT_TRUE(result);
    EXPECT_EQ(error, FS_OK);
    EXPECT_TRUE(count >= 3);
    
    bool found_file1 = false, found_file2 = false, found_subdir = false;
    for (size_t i = 0; i < count; i++) {
        if (strcmp(entries[i], "file1.txt") == 0) found_file1 = true;
        if (strcmp(entries[i], "file2.txt") == 0) found_file2 = true;
        if (strcmp(entries[i], "subdir") == 0) found_subdir = true;
    }
    EXPECT_TRUE(found_file1);
    EXPECT_TRUE(found_file2);
    EXPECT_TRUE(found_subdir);
    
    fs_free_dir_entries(&entries, count);
    
    cleanup_test_env();
}

void test_fs_strerror() {
    TEST(Fs_Strerror);
    
    const char* msg = fs_strerror(FS_OK);
    EXPECT_TRUE(msg != NULL);
    EXPECT_TRUE(strlen(msg) > 0);
    
    msg = fs_strerror(FS_ERROR_INVALID_PARAM);
    EXPECT_TRUE(msg != NULL);
    EXPECT_TRUE(strlen(msg) > 0);
    
    msg = fs_strerror(FS_ERROR_FILE_NOT_FOUND);
    EXPECT_TRUE(msg != NULL);
    EXPECT_TRUE(strlen(msg) > 0);
    
    msg = fs_strerror(FS_ERROR_MEMORY_ALLOC);
    EXPECT_TRUE(msg != NULL);
    EXPECT_TRUE(strlen(msg) > 0);
    
    msg = fs_strerror((fs_error_t)9999);
    EXPECT_TRUE(msg != NULL);
}

void test_fs_is_absolute_path() {
    TEST(Fs_IsAbsolutePath);
    
    EXPECT_TRUE(fs_is_absolute_path("/usr/local"));
    EXPECT_TRUE(fs_is_absolute_path("/"));
    EXPECT_TRUE(fs_is_absolute_path("/home/user/file.txt"));
    
    EXPECT_FALSE(fs_is_absolute_path("relative/path"));
    EXPECT_FALSE(fs_is_absolute_path("./file.txt"));
    EXPECT_FALSE(fs_is_absolute_path("../parent"));
    EXPECT_FALSE(fs_is_absolute_path("file.txt"));
}

void test_fs_realpath() {
    TEST(Fs_Realpath);
    setup_test_env();
    
    fs_error_t error;
    char buffer[4096];
    
    char* result = fs_realpath("/tmp", buffer, sizeof(buffer), &error);
    EXPECT_TRUE(result != NULL);
    EXPECT_EQ(error, FS_OK);
    EXPECT_TRUE(strstr(result, "tmp") != NULL);
    
    result = fs_realpath("/nonexistent_path_12345", buffer, sizeof(buffer), &error);
    EXPECT_TRUE(result == NULL);
    
    cleanup_test_env();
}

void test_fs_cwd_operations() {
    TEST(Fs_CwdOperations);
    char original_cwd[4096];
    char buffer[4096];
    fs_error_t error;
    
    char* cwd = fs_getcwd(original_cwd, sizeof(original_cwd), &error);
    EXPECT_TRUE(cwd != NULL);
    EXPECT_EQ(error, FS_OK);
    
    setup_test_env();
    
    bool result = fs_chdir(test_dir, &error);
    EXPECT_TRUE(result);
    EXPECT_EQ(error, FS_OK);
    
    cwd = fs_getcwd(buffer, sizeof(buffer), &error);
    EXPECT_TRUE(cwd != NULL);
    
    result = fs_chdir(original_cwd, &error);
    EXPECT_TRUE(result);
    
    cleanup_test_env();
}

void test_null_parameters() {
    TEST(NullParameters);
    fs_error_t error;
    size_t size;
    fs_file_info_t info;
    char** entries;
    size_t count;
    char buffer[256];
    
    char* result = fs_read_all(NULL, &size, &error);
    EXPECT_TRUE(result == NULL);
    
    bool success = fs_write_all(NULL, "data", 4, NULL, &error);
    EXPECT_FALSE(success);
    
    success = fs_write_all(test_file, NULL, 4, NULL, &error);
    EXPECT_FALSE(success);
    
    success = fs_exists(NULL, &error);
    EXPECT_FALSE(success);
    
    long size_result = fs_file_size(NULL, &error);
    EXPECT_EQ(size_result, -1L);
    
    const char* str_result = fs_extname(NULL, &error);
    EXPECT_TRUE(str_result == NULL || str_result[0] == '\0');
    
    str_result = fs_basename(NULL, &error);
    EXPECT_TRUE(str_result == NULL || str_result[0] == '\0');
    
    char* dir_result = fs_dirname(NULL, &error);
    EXPECT_TRUE(dir_result == NULL);
    
    success = fs_mkdir(NULL, true, &error);
    EXPECT_FALSE(success);
    
    success = fs_rmdir(NULL, true, &error);
    EXPECT_FALSE(success);
    
    success = fs_unlink(NULL, &error);
    EXPECT_FALSE(success);
    
    success = fs_rename(NULL, "new", &error);
    EXPECT_FALSE(success);
    
    success = fs_rename("old", NULL, &error);
    EXPECT_FALSE(success);
    
    success = fs_copy(NULL, "dest", NULL, &error);
    EXPECT_FALSE(success);
    
    success = fs_copy("src", NULL, NULL, &error);
    EXPECT_FALSE(success);
    
    success = fs_stat(NULL, &info, &error);
    EXPECT_FALSE(success);
    
    success = fs_read_dir(NULL, &entries, &count, &error);
    EXPECT_FALSE(success);
    
    char* cwd = fs_getcwd(NULL, 0, &error);
    EXPECT_TRUE(cwd != NULL || error != FS_OK);
    if (cwd) free(cwd);
    
    success = fs_chdir(NULL, &error);
    EXPECT_FALSE(success);
    
    EXPECT_FALSE(fs_is_absolute_path(NULL));
    
    char* realpath_result = fs_realpath(NULL, buffer, sizeof(buffer), &error);
    EXPECT_TRUE(realpath_result == NULL);
}

void test_empty_file() {
    TEST(EmptyFile);
    setup_test_env();
    
    const char* empty_file = "/tmp/test_fs_utils_dir/empty.txt";
    fs_error_t error;
    
    bool result = fs_write_all(empty_file, "", 0, NULL, &error);
    EXPECT_TRUE(result);
    EXPECT_EQ(error, FS_OK);
    
    size_t size;
    char* content = fs_read_all(empty_file, &size, &error);
    EXPECT_TRUE(content != NULL);
    EXPECT_EQ(size, 0);
    
    free(content);
    
    long file_size = fs_file_size(empty_file, &error);
    EXPECT_EQ(file_size, 0L);
    
    cleanup_test_env();
}

void test_binary_file() {
    TEST(BinaryFile);
    setup_test_env();
    
    const char* binary_file = "/tmp/test_fs_utils_dir/binary.bin";
    unsigned char binary_data[256];
    fs_error_t error;
    
    for (int i = 0; i < 256; i++) {
        binary_data[i] = (unsigned char)i;
    }
    
    bool result = fs_write_all(binary_file, binary_data, sizeof(binary_data), NULL, &error);
    EXPECT_TRUE(result);
    EXPECT_EQ(error, FS_OK);
    
    size_t size;
    char* content = fs_read_all(binary_file, &size, &error);
    EXPECT_TRUE(content != NULL);
    EXPECT_EQ(size, sizeof(binary_data));
    EXPECT_EQ(memcmp(content, binary_data, size), 0);
    
    free(content);
    cleanup_test_env();
}

void test_path_edge_cases() {
    TEST(PathEdgeCases);
    fs_error_t error;
    
    const char* base = fs_basename("", &error);
    EXPECT_TRUE(base != NULL);
    
    char* dir = fs_dirname("", &error);
    EXPECT_TRUE(dir != NULL);
    free(dir);
    
    const char* ext = fs_extname("", &error);
    EXPECT_TRUE(ext != NULL);
    
    base = fs_basename("///", &error);
    EXPECT_TRUE(base != NULL);
    
    dir = fs_dirname("///", &error);
    EXPECT_TRUE(dir != NULL);
    free(dir);
    
    base = fs_basename("file", &error);
    EXPECT_TRUE(base != NULL);
    EXPECT_EQ(strcmp(base, "file"), 0);
    
    dir = fs_dirname("file", &error);
    EXPECT_TRUE(dir != NULL);
    EXPECT_EQ(strcmp(dir, "."), 0);
    free(dir);
    
    ext = fs_extname("file", &error);
    EXPECT_TRUE(ext != NULL);
    EXPECT_EQ(strcmp(ext, ""), 0);
}

void test_error_paths() {
    TEST(ErrorPaths);
    fs_error_t error;
    fs_file_info_t info;
    char** entries;
    size_t count;
    char buffer[10];
    
    char* content = fs_read_all("/nonexistent/file.txt", &count, &error);
    EXPECT_TRUE(content == NULL);
    EXPECT_TRUE(error != FS_OK);
    
    bool result = fs_write_all("/nonexistent_dir/file.txt", "data", 4, NULL, &error);
    EXPECT_FALSE(result);
    EXPECT_TRUE(error != FS_OK);
    
    long size = fs_file_size("/nonexistent_file_12345.txt", &error);
    EXPECT_EQ(size, -1L);
    EXPECT_TRUE(error != FS_OK);
    
    result = fs_unlink("/nonexistent_file_12345.txt", &error);
    EXPECT_FALSE(result);
    EXPECT_TRUE(error != FS_OK);
    
    result = fs_rename("/nonexistent_src_12345", "/tmp/dst_12345", &error);
    EXPECT_FALSE(result);
    EXPECT_TRUE(error != FS_OK);
    
    result = fs_copy("/nonexistent_src_12345", "/tmp/dst_copy_12345", NULL, &error);
    EXPECT_FALSE(result);
    EXPECT_TRUE(error != FS_OK);
    
    result = fs_stat("/nonexistent_file_12345.txt", &info, &error);
    EXPECT_FALSE(result);
    EXPECT_TRUE(error != FS_OK);
    
    result = fs_read_dir("/nonexistent_dir_12345", &entries, &count, &error);
    EXPECT_FALSE(result);
    EXPECT_TRUE(error != FS_OK);
    
    result = fs_mkdir("/nonexistent_dir_12345/deep", false, &error);
    EXPECT_FALSE(result);
    EXPECT_TRUE(error != FS_OK);
    
    result = fs_chdir("/nonexistent_dir_12345", &error);
    EXPECT_FALSE(result);
    EXPECT_TRUE(error != FS_OK);
    
    char* realpath_result = fs_realpath("/nonexistent_12345", buffer, sizeof(buffer), &error);
    EXPECT_TRUE(realpath_result == NULL);
    EXPECT_TRUE(error != FS_OK);
}

int main() {
    printf("=== Running fs_utils tests ===\n\n");
    
    test_fs_default_options();
    test_fs_write_read_all();
    test_fs_exists();
    test_fs_file_size();
    test_fs_extname();
    test_fs_basename();
    test_fs_dirname();
    test_fs_mkdir_rmdir();
    test_fs_rmdir_recursive();
    test_fs_unlink();
    test_fs_rename();
    test_fs_copy();
    test_fs_stat();
    test_fs_read_dir();
    test_fs_strerror();
    test_fs_is_absolute_path();
    test_fs_realpath();
    test_fs_cwd_operations();
    test_null_parameters();
    test_empty_file();
    test_binary_file();
    test_path_edge_cases();
    test_error_paths();

    printf("\n=== All tests completed ===\n");
    return 0;
}
