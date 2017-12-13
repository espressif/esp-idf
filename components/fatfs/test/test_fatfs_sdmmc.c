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
#include "esp_log.h"
#include "esp_system.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "diskio.h"
#include "ff.h"
#include "test_fatfs_common.h"


static void test_setup(void)
{
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5
    };
    TEST_ESP_OK(esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, NULL));
}

static void test_teardown(void)
{
    TEST_ESP_OK(esp_vfs_fat_sdmmc_unmount());
}

static const char* test_filename = "/sdcard/hello.txt";

TEST_CASE("Mount fails cleanly without card inserted", "[fatfs][ignore]")
{
    size_t heap_size;
    HEAP_SIZE_CAPTURE(heap_size);
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5
    };

    for (int i = 0; i < 3; ++i) {
        printf("Initializing card, attempt %d ", i);
        esp_err_t err = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, NULL);
        printf(" err=%d\n", err);
        TEST_ESP_ERR(ESP_FAIL, err);
    }
    HEAP_SIZE_CHECK(heap_size, 0);
}

TEST_CASE("(SD) can create and write file", "[fatfs][sdcard][ignore]")
{
    test_setup();
    test_fatfs_create_file_with_text(test_filename, fatfs_test_hello_str);
    test_teardown();
}

TEST_CASE("(SD) can read file", "[fatfs][ignore]")
{
    test_setup();
    test_fatfs_create_file_with_text(test_filename, fatfs_test_hello_str);
    test_fatfs_read_file(test_filename);
    test_teardown();
}


TEST_CASE("(SD) overwrite and append file", "[fatfs][sdcard][ignore]")
{
    test_setup();
    test_fatfs_overwrite_append(test_filename);
    test_teardown();
}


TEST_CASE("(SD) can lseek", "[fatfs][sdcard][ignore]")
{
    test_setup();
    test_fatfs_lseek("/sdcard/seek.txt");
    test_teardown();
}

TEST_CASE("(SD) stat returns correct values", "[fatfs][ignore]")
{
    test_setup();
    test_fatfs_stat("/sdcard/stat.txt", "/sdcard");
    test_teardown();
}

TEST_CASE("(SD) unlink removes a file", "[fatfs][ignore]")
{
    test_setup();
    test_fatfs_unlink("/sdcard/unlink.txt");
    test_teardown();
}

TEST_CASE("(SD) link copies a file, rename moves a file", "[fatfs][ignore]")
{
    test_setup();
    test_fatfs_link_rename("/sdcard/link");
    test_teardown();
}

TEST_CASE("(SD) can create and remove directories", "[fatfs][ignore]")
{
    test_setup();
    test_fatfs_mkdir_rmdir("/sdcard/dir");
    test_teardown();
}

TEST_CASE("(SD) can opendir root directory of FS", "[fatfs][ignore]")
{
    test_setup();
    test_fatfs_can_opendir("/sdcard");
    test_teardown();
}

TEST_CASE("(SD) opendir, readdir, rewinddir, seekdir work as expected", "[fatfs][ignore]")
{
    test_setup();
    test_fatfs_opendir_readdir_rewinddir("/sdcard/dir");
    test_teardown();
}

TEST_CASE("(SD) multiple tasks can use same volume", "[fatfs][ignore]")
{
    test_setup();
    test_fatfs_concurrent("/sdcard/f");
    test_teardown();
}

static void speed_test(void* buf, size_t buf_size, size_t file_size, bool write);

TEST_CASE("(SD) write/read speed test", "[fatfs][sdcard][ignore]")
{
    size_t heap_size;
    HEAP_SIZE_CAPTURE(heap_size);

    const size_t buf_size = 16 * 1024;
    uint32_t* buf = (uint32_t*) calloc(1, buf_size);
    for (size_t i = 0; i < buf_size / 4; ++i) {
        buf[i] = esp_random();
    }
    const size_t file_size = 4 * 1024 * 1024;

    speed_test(buf, 4 * 1024, file_size, true);
    speed_test(buf, 8 * 1024, file_size, true);
    speed_test(buf, 16 * 1024, file_size, true);

    speed_test(buf, 4 * 1024, file_size, false);
    speed_test(buf, 8 * 1024, file_size, false);
    speed_test(buf, 16 * 1024, file_size, false);

    free(buf);

    HEAP_SIZE_CHECK(heap_size, 0);
}

static void speed_test(void* buf, size_t buf_size, size_t file_size, bool write)
{
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = write,
        .max_files = 5
    };
    TEST_ESP_OK(esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, NULL));

    test_fatfs_rw_speed("/sdcard/4mb.bin", buf, buf_size, file_size, write);

    TEST_ESP_OK(esp_vfs_fat_sdmmc_unmount());
}

TEST_CASE("(SD) mount two FAT partitions, SDMMC and WL, at the same time", "[fatfs][sdcard][ignore]")
{
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5
    };

    const char* filename_sd = "/sdcard/sd.txt";
    const char* filename_wl = "/spiflash/wl.txt";
    const char* str_sd = "this is sd\n";
    const char* str_wl = "this is spiflash\n";

    /* Mount FATFS in SD can WL at the same time. Create a file on each FS */
    wl_handle_t wl_handle = WL_INVALID_HANDLE;
    test_setup();
    TEST_ESP_OK(esp_vfs_fat_spiflash_mount("/spiflash", NULL, &mount_config, &wl_handle));
    unlink(filename_sd);
    unlink(filename_wl);
    test_fatfs_create_file_with_text(filename_sd, str_sd);
    test_fatfs_create_file_with_text(filename_wl, str_wl);
    TEST_ESP_OK(esp_vfs_fat_spiflash_unmount("/spiflash", wl_handle));
    test_teardown();

    /* Check that the file "sd.txt" was created on FS in SD, and has the right data */
    test_setup();
    TEST_ASSERT_NULL(fopen(filename_wl, "r"));
    FILE* f = fopen(filename_sd, "r");
    TEST_ASSERT_NOT_NULL(f);
    char buf[64];
    TEST_ASSERT_NOT_NULL(fgets(buf, sizeof(buf) - 1, f));
    TEST_ASSERT_EQUAL(0, strcmp(buf, str_sd));
    fclose(f);
    test_teardown();

    /* Check that the file "wl.txt" was created on FS in WL, and has the right data */
    TEST_ESP_OK(esp_vfs_fat_spiflash_mount("/spiflash", NULL, &mount_config, &wl_handle));
    TEST_ASSERT_NULL(fopen(filename_sd, "r"));
    f = fopen(filename_wl, "r");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_NOT_NULL(fgets(buf, sizeof(buf) - 1, f));
    TEST_ASSERT_EQUAL(0, strcmp(buf, str_wl));
    fclose(f);
    TEST_ESP_OK(esp_vfs_fat_spiflash_unmount("/spiflash", wl_handle));
}
