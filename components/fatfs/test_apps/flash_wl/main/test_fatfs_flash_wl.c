/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "unity.h"
#include "esp_partition.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "test_fatfs_common.h"
#include "wear_levelling.h"
#include "esp_partition.h"
#include "esp_memory_utils.h"
#include "vfs_fat_internal.h"

void app_main(void)
{
    unity_run_menu();
}

static wl_handle_t s_test_wl_handle;
static void test_setup(void)
{
    esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
    };

    TEST_ESP_OK(esp_vfs_fat_spiflash_mount_rw_wl("/spiflash", NULL, &mount_config, &s_test_wl_handle));
}

static void test_teardown(void)
{
    TEST_ESP_OK(esp_vfs_fat_spiflash_unmount_rw_wl("/spiflash", s_test_wl_handle));
}

#ifdef CONFIG_SPI_WL_TEST_ERASE_PARTITION
static void corrupt_wl_data(void)
{
    const esp_partition_t* part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, NULL);
    TEST_ASSERT_NOT_NULL(part);
    TEST_ESP_OK(esp_partition_erase_range(part, 0, part->size));
}
#endif

TEST_CASE("(WL) can format partition", "[fatfs][wear_levelling][timeout=120]")
{
#ifdef CONFIG_SPI_WL_TEST_ERASE_PARTITION
    corrupt_wl_data();
#endif
    TEST_ESP_OK(esp_vfs_fat_spiflash_format_rw_wl("/spiflash", NULL));
    test_setup();
    vfs_fat_spiflash_ctx_t* ctx = get_vfs_fat_spiflash_ctx(s_test_wl_handle);
    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_TRUE(ctx->fs->n_fats == 2); // 2 FATs are created by default
    test_teardown();
}

TEST_CASE("(WL) can format partition with config", "[fatfs][wear_levelling][timeout=120]")
{
#ifdef CONFIG_SPI_WL_TEST_ERASE_PARTITION
    corrupt_wl_data();
#endif
    esp_vfs_fat_mount_config_t format_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .use_one_fat = true,
    };
    TEST_ESP_OK(esp_vfs_fat_spiflash_format_cfg_rw_wl("/spiflash", NULL, &format_config));
    test_setup();
    vfs_fat_spiflash_ctx_t* ctx = get_vfs_fat_spiflash_ctx(s_test_wl_handle);
    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_TRUE(ctx->fs->n_fats == 1);
    test_teardown();
}

TEST_CASE("(WL) can format when the FAT is mounted already", "[fatfs][wear_levelling][timeout=120]")
{
    test_setup();
    TEST_ESP_OK(esp_vfs_fat_spiflash_format_rw_wl("/spiflash", NULL));
    test_fatfs_create_file_with_text("/spiflash/hello.txt", fatfs_test_hello_str);
    test_fatfs_pread_file("/spiflash/hello.txt");
    test_teardown();
}

TEST_CASE("(WL) can format when the FAT is mounted already with config", "[fatfs][wear_levelling][timeout=120]")
{
    TEST_ESP_OK(esp_vfs_fat_spiflash_format_rw_wl("/spiflash", NULL)); // To reset the FAT number to 2
    test_setup();
    vfs_fat_spiflash_ctx_t* ctx = get_vfs_fat_spiflash_ctx(s_test_wl_handle);
    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_TRUE(ctx->fs->n_fats == 2);
    esp_vfs_fat_mount_config_t format_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .use_one_fat = true,
    };
    TEST_ESP_OK(esp_vfs_fat_spiflash_format_cfg_rw_wl("/spiflash", NULL, &format_config));
    TEST_ASSERT_TRUE(ctx->fs->n_fats == 1);
    test_fatfs_create_file_with_text("/spiflash/hello.txt", fatfs_test_hello_str);
    test_fatfs_pread_file("/spiflash/hello.txt");
    test_teardown();
}

TEST_CASE("(WL) can format specified FAT when more are mounted", "[fatfs][wear_levelling][timeout=120]")
{
    esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
    };
    wl_handle_t s_test_wl_handle1;
    wl_handle_t s_test_wl_handle2;
    TEST_ESP_OK(esp_vfs_fat_spiflash_mount_rw_wl("/spiflash1", "storage", &mount_config, &s_test_wl_handle1));
    TEST_ESP_OK(esp_vfs_fat_spiflash_mount_rw_wl("/spiflash2", "storage2", &mount_config, &s_test_wl_handle2));

    test_fatfs_create_file_with_text("/spiflash1/hello.txt", fatfs_test_hello_str);
    test_fatfs_create_file_with_text("/spiflash2/hello.txt", fatfs_test_hello_str);

    TEST_ESP_OK(esp_vfs_fat_spiflash_format_rw_wl("/spiflash2", "storage2"));

    FILE* f = fopen("/spiflash2/hello.txt", "r");
    TEST_ASSERT_NULL(f); // File is erased on the formatted FAT
    test_fatfs_pread_file("/spiflash1/hello.txt"); // File is still readable on the other FAT

    TEST_ESP_OK(esp_vfs_fat_spiflash_unmount_rw_wl("/spiflash1", s_test_wl_handle1));
    TEST_ESP_OK(esp_vfs_fat_spiflash_unmount_rw_wl("/spiflash2", s_test_wl_handle2));
}

TEST_CASE("(WL) can create and write file", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_create_file_with_text("/spiflash/hello.txt", fatfs_test_hello_str);
    test_teardown();
}

TEST_CASE("(WL) can create and open file with O_CREAT flag", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_create_file_with_o_creat_flag("/spiflash/hello.txt");
    test_fatfs_open_file_with_o_creat_flag("/spiflash/hello.txt");
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
    esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = max_files
    };
    TEST_ESP_OK(esp_vfs_fat_spiflash_mount_rw_wl("/spiflash", NULL, &mount_config, &s_test_wl_handle));
    test_fatfs_open_max_files("/spiflash/f", max_files);
    TEST_ESP_OK(esp_vfs_fat_spiflash_unmount_rw_wl("/spiflash", s_test_wl_handle));
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
    test_fatfs_truncate_file("/spiflash/truncate.txt", true);
    test_teardown();
}

TEST_CASE("(WL) can ftruncate", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_ftruncate_file("/spiflash/ftrunc.txt", true);
    test_teardown();
}

#if FF_USE_EXPAND
TEST_CASE("(WL) can esp_vfs_fat_create_contiguous_file", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_create_contiguous_file("/spiflash", "/spiflash/expand.txt");
    test_teardown();
}
#endif

TEST_CASE("(WL) stat returns correct values", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_stat("/spiflash/stat.txt", "/spiflash");
    test_teardown();
}

TEST_CASE("(WL) stat returns correct mtime if DST is enabled", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_mtime_dst("/spiflash/statdst.txt", "/spiflash");
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

TEST_CASE("(WL) readdir, stat work as expected", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_readdir_stat("/spiflash/dir");
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

TEST_CASE("(WL) fatfs does not ignore leading spaces", "[fatfs][wear_levelling]")
{
    // the functionality of ignoring leading and trailing whitespaces is not implemented yet
    // when the feature is implemented, this test will fail
    // please, remove the test and implement the functionality in fatfsgen.py to preserve the consistency
    test_setup();
    test_leading_spaces();
    test_teardown();
}


TEST_CASE("(WL) write/read speed test", "[fatfs][wear_levelling][timeout=60]")
{
    /* Erase partition before running the test to get consistent results */
    const esp_partition_t* part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
            ESP_PARTITION_SUBTYPE_DATA_FAT, NULL);
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

TEST_CASE("(WL) can get partition info", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_info("/spiflash", "/spiflash/test.txt");
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
#endif //defined(CONFIG_FATFS_API_ENCODING_UTF_8) && (CONFIG_FATFS_CODEPAGE == 936)

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

#if CONFIG_FATFS_IMMEDIATE_FSYNC

TEST_CASE("Size is correct after write when immediate fsync is enabled", "[fatfs]")
{
    test_setup();
    test_fatfs_size("/spiflash/size.txt", "random text\n preferably something relatively long\n");
    test_teardown();
}

#endif // CONFIG_FATFS_IMMEDIATE_FSYNC
