/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/unistd.h>
#include "unity.h"
#include "esp_vfs_fat.h"

static wl_handle_t s_test_wl_handle;
static void test_setup(void)
{
  // With this configuration, for 32k partition size,
  // 4 sectors will be used for WL and 4 sectors for FATFS
  // (1 FAT,  1 root directory, 1 reserved and 1 data sector)
    esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .use_one_fat = true,
    };

    TEST_ESP_OK(esp_vfs_fat_spiflash_format_cfg_rw_wl("/spiflash", "storage1", &mount_config));
    TEST_ESP_OK(esp_vfs_fat_spiflash_mount_rw_wl("/spiflash", "storage1", &mount_config, &s_test_wl_handle));
}

static void test_teardown(void)
{
    TEST_ESP_OK(esp_vfs_fat_spiflash_unmount_rw_wl("/spiflash", s_test_wl_handle));
}

static void test_write_data_sec(int num_data_sec)
{
    int fd = open("/spiflash/test.txt", O_CREAT | O_WRONLY);
    TEST_ASSERT_NOT_EQUAL(-1, fd);

    // Generate data
    uint32_t data_size = 4096*num_data_sec;

    char *data = (char*) malloc(data_size);
    char *read_data = (char*) malloc(data_size);

    for(uint32_t i = 0; i < (data_size); i += sizeof(i))
    {
        *((uint32_t*)(data + i)) = i;
    }
    ssize_t wr = write(fd, data, data_size);
    if (num_data_sec == 1) {
        TEST_ASSERT_EQUAL(data_size, wr);
    } else {
        TEST_ASSERT_NOT_EQUAL(data_size, wr);
    }
    TEST_ASSERT_EQUAL(0, close(fd));

    fd = open("/spiflash/test.txt", O_RDONLY);
    int r = read(fd, read_data, data_size);
    if (num_data_sec == 1) {
        TEST_ASSERT_EQUAL(data_size, r);
    } else {
        TEST_ASSERT_NOT_EQUAL(data_size, r);
    }
    TEST_ASSERT_EQUAL(0, strcmp(data, read_data));
    TEST_ASSERT_EQUAL(0, close(fd));
}

TEST_CASE("(WL) can format small partition and read-write data", "[fatfs][wear_levelling][timeout=120]")
{
    test_setup();
    test_write_data_sec(1); //for 1 data sectors, write and read func should work
    test_write_data_sec(2); //for 2 data sectors, write and read func should fail
    test_teardown();
}
