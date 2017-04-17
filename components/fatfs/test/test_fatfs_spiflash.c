// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/unistd.h>
#include "unity.h"
#include "test_utils.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "test_fatfs_common.h"
#include "wear_levelling.h"
#include "esp_partition.h"


static wl_handle_t s_test_wl_handle;
static void test_setup()
{
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5
    };

    TEST_ESP_OK(esp_vfs_fat_spiflash_mount("/spiflash", NULL, &mount_config, &s_test_wl_handle));
}

static void test_teardown()
{
    TEST_ESP_OK(esp_vfs_fat_spiflash_unmount("/spiflash", s_test_wl_handle));
}

TEST_CASE("(WL) can format partition", "[fatfs][wear_levelling]")
{
    const esp_partition_t* part = get_test_data_partition();
    esp_partition_erase_range(part, 0, part->size);
    test_setup();
    test_teardown();
}

TEST_CASE("(WL) can create and write file", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_create_file_with_text("/spiflash/hello.txt", fatfs_test_hello_str);
    test_teardown();
}

TEST_CASE("(WL) can read file", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_create_file_with_text("/spiflash/hello.txt", fatfs_test_hello_str);
    test_fatfs_read_file("/spiflash/hello.txt");
    test_teardown();
}

TEST_CASE("(WL) can open maximum number of files", "[fatfs][wear_levelling]")
{
    size_t max_files = FOPEN_MAX - 3; /* account for stdin, stdout, stderr */
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = max_files
    };
    TEST_ESP_OK(esp_vfs_fat_spiflash_mount("/spiflash", NULL, &mount_config, &s_test_wl_handle));
    test_fatfs_open_max_files("/spiflash/f", max_files);
    TEST_ESP_OK(esp_vfs_fat_spiflash_unmount("/spiflash", s_test_wl_handle));
}

TEST_CASE("(WL) overwrite and append file", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_overwrite_append("/spiflash/hello.txt");
    test_teardown();
}

TEST_CASE("(WL) can lseek", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_lseek("/spiflash/seek.txt");
    test_teardown();
}


TEST_CASE("(WL) stat returns correct values", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_stat("/spiflash/stat.txt");
    test_teardown();
}

TEST_CASE("(WL) unlink removes a file", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_unlink("/spiflash/unlink.txt");
    test_teardown();
}

TEST_CASE("(WL) link copies a file, rename moves a file", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_link_rename("/spiflash/link");
    test_teardown();
}

TEST_CASE("(WL) can create and remove directories", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_mkdir_rmdir("/spiflash/dir");
    test_teardown();
}

TEST_CASE("(WL) opendir, readdir, rewinddir, seekdir work as expected", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_opendir_readdir_rewinddir("/spiflash/dir");
    test_teardown();
}

TEST_CASE("(WL) multiple tasks can use same volume", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_concurrent("/spiflash/f");
    test_teardown();
}

TEST_CASE("(WL) write/read speed test", "[fatfs][wear_levelling]")
{
    /* Erase partition before running the test to get consistent results */
    const esp_partition_t* part = get_test_data_partition();
    esp_partition_erase_range(part, 0, part->size);

    test_setup();

    const size_t buf_size = 16 * 1024;
    uint32_t* buf = (uint32_t*) calloc(1, buf_size);
    for (size_t i = 0; i < buf_size / 4; ++i) {
        buf[i] = esp_random();
    }
    const size_t file_size = 256 * 1024;
    const char* file = "/spiflash/256k.bin";

    test_fatfs_rw_speed(file, buf, 4 * 1024, file_size, true);
    test_fatfs_rw_speed(file, buf, 8 * 1024, file_size, true);
    test_fatfs_rw_speed(file, buf, 16 * 1024, file_size, true);

    test_fatfs_rw_speed(file, buf, 4 * 1024, file_size, false);
    test_fatfs_rw_speed(file, buf, 8 * 1024, file_size, false);
    test_fatfs_rw_speed(file, buf, 16 * 1024, file_size, false);

    unlink(file);

    free(buf);
    test_teardown();
}
