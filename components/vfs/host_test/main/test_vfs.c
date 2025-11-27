/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#include "esp_vfs.h"
#include "test_vfs_linux_dev.h"
#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(vfs_linux);

TEST_SETUP(vfs_linux)
{
}

TEST_TEAR_DOWN(vfs_linux)
{
}

static void test_create_file_with_text(const char* name, const char* text)
{
    int fd = -1;
    fd = open(name, O_CREAT|O_RDWR, 0777);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    ssize_t sz = write(fd, text, strlen(text));
    TEST_ASSERT_EQUAL(strlen(text), sz);

    TEST_ASSERT_EQUAL(0, close(fd));
}

TEST(vfs_linux,test_linux_vfs_open)
{
    linux_vfs_dev_register();

    const char *test_str = "Espressif";
    const char *filename = "/linux/test.txt";
    test_create_file_with_text(filename, test_str);

    int fd = open(filename, O_RDWR);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    int len = strlen(test_str);
    char buf[10];
    TEST_ASSERT_EQUAL(len, read(fd, buf, len));
    TEST_ASSERT_EQUAL(0, strncmp(test_str, buf, len));
    close(fd);
    unlink(filename);
    printf("Inside %s\n",__func__);

    linux_vfs_dev_unregister();
}

static void test_lseek(void)
{
    const char *test_str = "0123456789\n";
    const char *filename = "/linux/lseek.txt";
    test_create_file_with_text(filename, test_str);

    int fd = open(filename, O_RDWR);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    off_t off = lseek(fd, 6, SEEK_CUR);
    TEST_ASSERT_EQUAL(6, off);
    off = lseek(fd, 3, SEEK_SET);
    TEST_ASSERT_EQUAL(3, off);
    off = lseek(fd, -9, SEEK_END);
    TEST_ASSERT_EQUAL(2, off);

    close(fd);
    unlink(filename);
}

TEST(vfs_linux, test_lseek_via_vfs)
{
    printf("Inside %s\n",__func__);
    linux_vfs_dev_register();
    printf("Inside %s\n",__func__);
    test_lseek();
    linux_vfs_dev_unregister();
}

static void test_pread_pwrite(void)
{
    const char *test_str = "0123456789";
    const char *filename = "/linux/pread_pwrite.txt";
    test_create_file_with_text(filename, test_str);

    int fd = open(filename, O_RDWR);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    // Test pwrite
    const char *write_str = "ABCD";
    ssize_t written = pwrite(fd, write_str, strlen(write_str), 2);
    TEST_ASSERT_EQUAL(strlen(write_str), written);

    // Test pread
    char read_buf[10] = {0};
    ssize_t bytes_read = pread(fd, read_buf, 4, 2);
    TEST_ASSERT_EQUAL(4, bytes_read);
    TEST_ASSERT_EQUAL(0, strncmp(write_str, read_buf, 4));

    close(fd);
    unlink(filename);
}

TEST(vfs_linux, test_pread_pwrite_via_vfs)
{
    linux_vfs_dev_register();
    test_pread_pwrite();
    linux_vfs_dev_unregister();
}

static void test_unlink(void)
{
    const char *filename = "/linux/unlink.txt";
    test_create_file_with_text(filename, "test");

    // Test that file exists
    int fd = open(filename, O_RDWR);
    TEST_ASSERT_NOT_EQUAL(-1, fd);
    close(fd);

    // Test unlink
    TEST_ASSERT_EQUAL(0, unlink(filename));

    // Test that file no longer exists
    fd = open(filename, O_RDWR);
    TEST_ASSERT_EQUAL(-1, fd);
    TEST_ASSERT_EQUAL(ENOENT, errno);
}

TEST(vfs_linux, test_unlink_via_vfs)
{
    linux_vfs_dev_register();
    test_unlink();
    linux_vfs_dev_unregister();
}

static void test_fstat(void)
{
    const char *filename = "/linux/fstat.txt";
    test_create_file_with_text(filename, "test");

    int fd = open(filename, O_RDWR);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    struct stat st;
    TEST_ASSERT_EQUAL(0, fstat(fd, &st));
    TEST_ASSERT_TRUE(S_ISREG(st.st_mode));  // Check if it's a regular file
    TEST_ASSERT_EQUAL(4, st.st_size);       // Check file size

    close(fd);
    unlink(filename);
}

TEST(vfs_linux, test_fstat_via_vfs)
{
    linux_vfs_dev_register();
    test_fstat();
    linux_vfs_dev_unregister();
}

static void test_fcntl(void)
{
    const char *filename = "/linux/fcntl.txt";
    test_create_file_with_text(filename, "test");

    int fd = open(filename, O_RDWR);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    // Test F_GETFL
    int flags = fcntl(fd, F_GETFL);
    TEST_ASSERT_NOT_EQUAL(-1, flags);
    TEST_ASSERT_EQUAL(O_RDWR, (flags & O_RDWR));  // Check if file is opened in read-write mode
    close(fd);
    unlink(filename);
}

TEST(vfs_linux, test_fcntl_via_vfs)
{
    linux_vfs_dev_register();
    test_fcntl();
    linux_vfs_dev_unregister();
}

static void test_ftruncate(void)
{
    const char *test_str = "0123456789\n";
    const char *filename = "/linux/ftruncate.txt";
    test_create_file_with_text(filename, test_str);

    int fd = open(filename, O_RDWR);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    // Test truncating to a smaller size
    const char truncated_1[] = "01234";
    off_t truncated_len = strlen(truncated_1);
    TEST_ASSERT_EQUAL(0, ftruncate(fd, truncated_len));

    // Verify the file size after truncation
    struct stat st;
    TEST_ASSERT_EQUAL(0, fstat(fd, &st));
    TEST_ASSERT_EQUAL(truncated_len, st.st_size);

    // Read back the truncated content
    char buf[32] = {0};
    TEST_ASSERT_EQUAL(truncated_len, read(fd, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL(0, strncmp(truncated_1, buf, truncated_len));

    // Test truncating to a larger size
    off_t new_size = truncated_len + 5;
    TEST_ASSERT_EQUAL(0, ftruncate(fd, new_size));
    TEST_ASSERT_EQUAL(0, fstat(fd, &st));
    TEST_ASSERT_EQUAL(new_size, st.st_size);

    // Test truncating to zero
    TEST_ASSERT_EQUAL(0, ftruncate(fd, 0));
    TEST_ASSERT_EQUAL(0, fstat(fd, &st));
    TEST_ASSERT_EQUAL(0, st.st_size);

    // Test invalid length
    TEST_ASSERT_EQUAL(-1, ftruncate(fd, -1));
    TEST_ASSERT_EQUAL(EINVAL, errno);

    close(fd);
    unlink(filename);
}

TEST(vfs_linux, test_ftruncate_via_vfs)
{
    linux_vfs_dev_register();
    test_ftruncate();
    linux_vfs_dev_unregister();
}

TEST_GROUP_RUNNER(vfs_linux)
{
    RUN_TEST_CASE(vfs_linux, test_linux_vfs_open);
    RUN_TEST_CASE(vfs_linux, test_lseek_via_vfs);
    RUN_TEST_CASE(vfs_linux, test_pread_pwrite_via_vfs);
    RUN_TEST_CASE(vfs_linux, test_unlink_via_vfs);
    RUN_TEST_CASE(vfs_linux, test_fstat_via_vfs);
    RUN_TEST_CASE(vfs_linux, test_fcntl_via_vfs);
    RUN_TEST_CASE(vfs_linux, test_ftruncate_via_vfs);
}

static void run_all_tests(void)
{
    RUN_TEST_GROUP(vfs_linux);
}

int main(int argc, char **argv)
{
    UNITY_MAIN_FUNC(run_all_tests);
    return 0;
}
