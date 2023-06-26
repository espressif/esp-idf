/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "esp_vfs.h"
#include "unity.h"

static int open_errno_test_open(const char * path, int flags, int mode)
{
    errno = EIO;
    return -1;
}

TEST_CASE("esp_vfs_open sets correct errno", "[vfs]")
{
    esp_vfs_t desc = {
        .open = open_errno_test_open
    };
    TEST_ESP_OK(esp_vfs_register("/test", &desc, NULL));

    int fd = open("/test/path", 0, 0);
    int e = errno;
    TEST_ASSERT_EQUAL(-1, fd);
    TEST_ASSERT_EQUAL(EIO, e);

    fd = open("/nonexistent/path", 0, 0);
    e = errno;
    TEST_ASSERT_EQUAL(-1, fd);
    TEST_ASSERT_EQUAL(ENOENT, e);

    TEST_ESP_OK(esp_vfs_unregister("/test"));
}
