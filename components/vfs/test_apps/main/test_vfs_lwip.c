/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/param.h>
#include <sys/socket.h>
#include "unity.h"
#include "test_utils.h"

TEST_CASE("fstat() sets st_mode to socket type", "[vfs][lwip]")
{
    test_case_uses_tcpip();
    int socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct stat stat = { 0 };
    fstat(socket_fd, &stat);

    TEST_ASSERT_TRUE(S_ISSOCK(stat.st_mode));
    TEST_ASSERT_FALSE(S_ISBLK(stat.st_mode));
    TEST_ASSERT_FALSE(S_ISCHR(stat.st_mode));
    TEST_ASSERT_FALSE(S_ISDIR(stat.st_mode));
    TEST_ASSERT_FALSE(S_ISREG(stat.st_mode));
    TEST_ASSERT_FALSE(S_ISLNK(stat.st_mode));

    close(socket_fd);
}
