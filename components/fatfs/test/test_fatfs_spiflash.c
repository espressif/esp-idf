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
static void test_setup(void)
{
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5
    };

    TEST_ESP_OK(esp_vfs_fat_spiflash_mount("/spiflash", NULL, &mount_config, &s_test_wl_handle));
}

static void test_teardown(void)
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

TEST_CASE("(WL) can read file with pread", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_create_file_with_text("/spiflash/hello.txt", fatfs_test_hello_str);
    test_fatfs_pread_file("/spiflash/hello.txt");
    test_teardown();
}

TEST_CASE("(WL) pwrite() works well", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_pwrite_file("/spiflash/hello.txt");
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

TEST_CASE("(WL) can truncate", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_truncate_file("/spiflash/truncate.txt");
    test_teardown();
}

TEST_CASE("(WL) stat returns correct values", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_stat("/spiflash/stat.txt", "/spiflash");
    test_teardown();
}

TEST_CASE("(WL) utime sets modification time", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_utime("/spiflash/utime.txt", "/spiflash");
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

TEST_CASE("(WL) can opendir root directory of FS", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_can_opendir("/spiflash");
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

TEST_CASE("(WL) write/read speed test", "[fatfs][wear_levelling][timeout=60]")
{
    /* Erase partition before running the test to get consistent results */
    const esp_partition_t* part = get_test_data_partition();
    esp_partition_erase_range(part, 0, part->size);

    test_setup();

    const size_t buf_size = 16 * 1024;
    uint32_t* buf = (uint32_t*) calloc(1, buf_size);
    esp_fill_random(buf, buf_size);
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

/*
 * In FatFs menuconfig, set CONFIG_FATFS_API_ENCODING to UTF-8 and set the
 * Codepage to CP936 (Simplified Chinese) in order to run the following tests.
 * Ensure that the text editor is UTF-8 compatible when compiling these tests.
 */
#if defined(CONFIG_FATFS_API_ENCODING_UTF_8) && (CONFIG_FATFS_CODEPAGE == 936)
TEST_CASE("(WL) can read file with UTF-8 encoded strings", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_create_file_with_text("/spiflash/测试文件.txt", fatfs_test_hello_str_utf);
    test_fatfs_read_file_utf_8("/spiflash/测试文件.txt");
    test_teardown();
}

TEST_CASE("(WL) opendir, readdir, rewinddir, seekdir work as expected using UTF-8 encoded strings", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_opendir_readdir_rewinddir_utf_8("/spiflash/目录");
    test_teardown();
}
#endif

#ifdef CONFIG_SPIRAM
TEST_CASE("FATFS prefers SPI RAM for allocations", "[fatfs]")
{
    test_setup();
    DIR* dir = opendir("/spiflash");
    TEST_ASSERT_NOT_NULL(dir);
    TEST_ASSERT(esp_ptr_external_ram(dir));
    closedir(dir);
    test_teardown();
}
#endif // CONFIG_SPIRAM
