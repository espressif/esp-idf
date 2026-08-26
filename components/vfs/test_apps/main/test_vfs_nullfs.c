/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include "unity.h"
#include "esp_vfs.h"

#include "esp_vfs_null.h"
#include "unity_test_runner.h"

TEST_CASE("Can open and close /dev/null", "[vfs_nullfs]")
{
    int fd = open("/dev/null", O_RDWR);
    TEST_ASSERT(fd >= 0);
    close(fd);
}

TEST_CASE("Can write to /dev/null", "[vfs_nullfs]")
{
    int fd = open("/dev/null", O_RDWR);
    TEST_ASSERT(fd >= 0);

    // Can write to /dev/null
    ssize_t ret = write(fd, "hello", 5);
    TEST_ASSERT_EQUAL(5, ret);

    // Write does not change the file offset
    off_t offset = lseek(fd, 0, SEEK_CUR);
    TEST_ASSERT_EQUAL(0, offset);

    close(fd);
}

TEST_CASE("Can read from /dev/null", "[vfs_nullfs]")
{
    int fd = open("/dev/null", O_RDWR);
    TEST_ASSERT(fd >= 0);

    // Can read from /dev/null
    char buf[5] = {0};
    ssize_t ret = read(fd, buf, 5);

    // Read always returns 0 bytes -> EOF
    TEST_ASSERT_EQUAL(0, ret);

    // Read does not modify the buffer
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL(0, buf[i]);
    }

    // Read does not change the file offset
    off_t offset = lseek(fd, 0, SEEK_CUR);
    TEST_ASSERT_EQUAL(0, offset);


    close(fd);
}

TEST_CASE("Can lseek /dev/null", "[vfs_nullfs]")
{
    int fd = open("/dev/null", O_RDWR);
    TEST_ASSERT(fd >= 0);

    off_t offset = lseek(fd, 0, SEEK_SET);
    TEST_ASSERT_EQUAL(0, offset);

    offset = lseek(fd, 0, SEEK_CUR);
    TEST_ASSERT_EQUAL(0, offset);

    offset = lseek(fd, 0, SEEK_END);
    TEST_ASSERT_EQUAL(0, offset);

    close(fd);
}

TEST_CASE("Can fstat /dev/null", "[vfs_nullfs]")
{
    int fd = open("/dev/null", O_RDWR);
    TEST_ASSERT(fd >= 0);

    struct stat st;
    int ret = fstat(fd, &st);
    TEST_ASSERT_EQUAL(0, ret);

    TEST_ASSERT_EQUAL(0, st.st_size);
    TEST_ASSERT_EQUAL(S_IFCHR, st.st_mode & S_IFMT);

    close(fd);
}

TEST_CASE("Can fsync /dev/null", "[vfs_nullfs]")
{
    int fd = open("/dev/null", O_RDWR);
    TEST_ASSERT(fd >= 0);

    int ret = fsync(fd);
    TEST_ASSERT_EQUAL(0, ret);

    close(fd);
}

TEST_CASE("Can pread /dev/null", "[vfs_nullfs]")
{
    int fd = open("/dev/null", O_RDWR);
    TEST_ASSERT(fd >= 0);

    char buf[5] = {0};
    ssize_t ret = pread(fd, buf, 5, 0);
    TEST_ASSERT_EQUAL(0, ret);

    close(fd);
}

TEST_CASE("Can pwrite /dev/null", "[vfs_nullfs]")
{
    int fd = open("/dev/null", O_RDWR);
    TEST_ASSERT(fd >= 0);

    ssize_t ret = pwrite(fd, "hello", 5, 0);
    TEST_ASSERT_EQUAL(5, ret);

    close(fd);
}

TEST_CASE("Can stat /dev/null", "[vfs_nullfs]")
{
    struct stat st;
    int ret = stat("/dev/null", &st);
    TEST_ASSERT_EQUAL(0, ret);

    TEST_ASSERT_EQUAL(0, st.st_size);
    TEST_ASSERT_EQUAL(S_IFCHR, st.st_mode & S_IFMT);
}
