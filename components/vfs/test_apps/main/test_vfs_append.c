/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include "unity.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_spiffs.h"
#include "wear_levelling.h"

#define TEST_PARTITION_LABEL "flash_test"

#define OPEN_MODE   0
#define MSG1        "Hello"
#define MSG2        " "
#define MSG3        "world!"

static inline void test_write(int fd, const char *str, const char *msg)
{
    TEST_ASSERT_EQUAL_MESSAGE(strlen(str), write(fd, str, strlen(str)), msg);
}

static inline void test_read(int fd, const char *str, const char *msg)
{
    char buf[strlen(str)];
    TEST_ASSERT_EQUAL_MESSAGE(strlen(str), read(fd, buf, strlen(str)), msg);
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(str, buf, strlen(str), msg);
}

static inline void test_read_fails(int fd, const char *msg)
{
    char buf;
    TEST_ASSERT_EQUAL_MESSAGE(0, read(fd, &buf, 1), msg);
}

static void test_append(const char *path)
{
    int fd = open(path, O_RDWR | O_APPEND | O_CREAT | O_TRUNC, OPEN_MODE);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    test_write(fd, MSG1, "write MSG1");
    test_read_fails(fd, "read fails MSG1");
    lseek(fd, 0, SEEK_SET);
    test_read(fd, MSG1, "read MSG1");

    lseek(fd, 0, SEEK_SET);
    test_write(fd, MSG2, "write MSG2");
    test_read_fails(fd, "read fails MSG2"); //because write moved the pointer
    lseek(fd, 0, SEEK_SET);
    test_read(fd, MSG1 MSG2, "read MSG1 + MSG2");

    TEST_ASSERT_NOT_EQUAL(-1, close(fd));
    fd = open(path, O_RDWR | O_APPEND, OPEN_MODE);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    //after reopening the pointer should be at the beginning
    test_read(fd, MSG1 MSG2, "read reopening");

    lseek(fd, strlen(MSG1), SEEK_SET);
    test_read(fd, MSG2, "read MSG2");
    lseek(fd, strlen(MSG1), SEEK_SET);
    test_write(fd, MSG3, "write MSG3");
    test_read_fails(fd, "read fails MSG3"); //because write moved the pointer
    lseek(fd, strlen(MSG1), SEEK_SET);
    test_read(fd, MSG2 MSG3, "read MSG2 + MSG3");

    lseek(fd, 0, SEEK_SET);
    test_read(fd, MSG1 MSG2 MSG3, "read MSG1 + MSG2 + MSG3");

    TEST_ASSERT_NOT_EQUAL(-1, close(fd));
    TEST_ASSERT_NOT_EQUAL(-1, unlink(path));
}

TEST_CASE("open() with O_APPEND on FATFS works well", "[vfs][FATFS]")
{
    wl_handle_t test_wl_handle;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 2
    };
    TEST_ESP_OK(esp_vfs_fat_spiflash_mount_rw_wl("/spiflash", NULL, &mount_config, &test_wl_handle));

    test_append("/spiflash/file.txt");

    TEST_ESP_OK(esp_vfs_fat_spiflash_unmount_rw_wl("/spiflash", test_wl_handle));
}

TEST_CASE("open() with O_APPEND on SPIFFS works well", "[vfs][spiffs]")
{
    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = TEST_PARTITION_LABEL,
      .max_files = 2,
      .format_if_mount_failed = true
    };
    TEST_ESP_OK(esp_vfs_spiffs_register(&conf));

    test_append("/spiffs/file.txt");

    TEST_ESP_OK(esp_vfs_spiffs_unregister(TEST_PARTITION_LABEL));
}
