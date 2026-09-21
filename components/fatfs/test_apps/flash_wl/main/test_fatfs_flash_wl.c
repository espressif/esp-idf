/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/unistd.h>
#include <errno.h>
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

TEST_CASE("(WL) fcntl F_SETFL preserves access mode", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_fcntl_setfl("/spiflash/fcntl.txt");
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

TEST_CASE("(WL) can esp_vfs_fat_create_contiguous_file", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_create_contiguous_file("/spiflash", "/spiflash/expand.txt");
    test_teardown();
}

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

TEST_CASE("(WL) rename to an existing destination", "[fatfs][wear_levelling]")
{
    test_setup();

    const char *src = "/spiflash/ren_src.txt";
    const char *dst = "/spiflash/ren_dst.txt";

    FILE *f = fopen(src, "w");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_TRUE(fputs("source", f) >= 0);
    TEST_ASSERT_EQUAL(0, fclose(f));

    f = fopen(dst, "w");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_TRUE(fputs("destination", f) >= 0);
    TEST_ASSERT_EQUAL(0, fclose(f));

    char buf[32];
    errno = 0;
    int ret = rename(src, dst);

#ifdef CONFIG_FATFS_VFS_RENAME_REPLACES_DESTINATION
    /* POSIX behavior: the destination is replaced. */
    TEST_ASSERT_EQUAL(0, ret);

    memset(buf, 0, sizeof(buf));
    f = fopen(dst, "r");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_NOT_NULL(fgets(buf, sizeof(buf), f));
    TEST_ASSERT_EQUAL(0, fclose(f));
    TEST_ASSERT_EQUAL_STRING("source", buf);

    TEST_ASSERT_NULL(fopen(src, "r"));

    TEST_ASSERT_EQUAL(0, unlink(dst));
#else
    /* Default FatFs behavior: the rename is refused and nothing changes. */
    TEST_ASSERT_EQUAL(-1, ret);
    TEST_ASSERT_EQUAL(EEXIST, errno);

    memset(buf, 0, sizeof(buf));
    f = fopen(dst, "r");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_NOT_NULL(fgets(buf, sizeof(buf), f));
    TEST_ASSERT_EQUAL(0, fclose(f));
    TEST_ASSERT_EQUAL_STRING("destination", buf);

    memset(buf, 0, sizeof(buf));
    f = fopen(src, "r");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_NOT_NULL(fgets(buf, sizeof(buf), f));
    TEST_ASSERT_EQUAL(0, fclose(f));
    TEST_ASSERT_EQUAL_STRING("source", buf);

    TEST_ASSERT_EQUAL(0, unlink(src));
    TEST_ASSERT_EQUAL(0, unlink(dst));
#endif

    test_teardown();
}

TEST_CASE("(WL) rename obeys the POSIX rules on directories", "[fatfs][wear_levelling]")
{
    test_setup();

    const char *file = "/spiflash/ren_f.txt";
    const char *dir = "/spiflash/ren_d";
    const char *dir2 = "/spiflash/ren_d2";

    FILE *f = fopen(file, "w");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_TRUE(fputs("payload", f) >= 0);
    TEST_ASSERT_EQUAL(0, fclose(f));
    TEST_ASSERT_EQUAL(0, mkdir(dir, 0755));

    struct stat st;

    /* A file may not replace a directory, and the directory must survive. */
    errno = 0;
    TEST_ASSERT_EQUAL(-1, rename(file, dir));
#ifdef CONFIG_FATFS_VFS_RENAME_REPLACES_DESTINATION
    TEST_ASSERT_EQUAL(EISDIR, errno);
#else
    TEST_ASSERT_EQUAL(EEXIST, errno);
#endif
    TEST_ASSERT_EQUAL(0, stat(dir, &st));
    TEST_ASSERT_TRUE(S_ISDIR(st.st_mode));

    /* A directory may not replace a file, and the file must survive. */
    errno = 0;
    TEST_ASSERT_EQUAL(-1, rename(dir, file));
#ifdef CONFIG_FATFS_VFS_RENAME_REPLACES_DESTINATION
    TEST_ASSERT_EQUAL(ENOTDIR, errno);
#else
    TEST_ASSERT_EQUAL(EEXIST, errno);
#endif
    TEST_ASSERT_EQUAL(0, stat(file, &st));
    TEST_ASSERT_FALSE(S_ISDIR(st.st_mode));

    /* A directory may not replace a non-empty directory. */
    TEST_ASSERT_EQUAL(0, mkdir(dir2, 0755));
    f = fopen("/spiflash/ren_d2/occupant.txt", "w");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_EQUAL(0, fclose(f));
    errno = 0;
    TEST_ASSERT_EQUAL(-1, rename(dir, dir2));
#ifdef CONFIG_FATFS_VFS_RENAME_REPLACES_DESTINATION
    TEST_ASSERT_EQUAL(ENOTEMPTY, errno);
#else
    TEST_ASSERT_EQUAL(EEXIST, errno);
#endif
    TEST_ASSERT_EQUAL(0, stat("/spiflash/ren_d2/occupant.txt", &st));

    TEST_ASSERT_EQUAL(0, unlink("/spiflash/ren_d2/occupant.txt"));

    /* A directory may replace an empty one. */
    errno = 0;
    int ret = rename(dir, dir2);
#ifdef CONFIG_FATFS_VFS_RENAME_REPLACES_DESTINATION
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(0, stat(dir2, &st));
    TEST_ASSERT_TRUE(S_ISDIR(st.st_mode));
    TEST_ASSERT_EQUAL(-1, stat(dir, &st));
    TEST_ASSERT_EQUAL(0, rmdir(dir2));
#else
    TEST_ASSERT_EQUAL(-1, ret);
    TEST_ASSERT_EQUAL(EEXIST, errno);
    TEST_ASSERT_EQUAL(0, rmdir(dir));
    TEST_ASSERT_EQUAL(0, rmdir(dir2));
#endif
    TEST_ASSERT_EQUAL(0, unlink(file));

    test_teardown();
}

/* Only meaningful with the option enabled: without it f_rename() happily moves
 * the directory into its own tree, which corrupts the volume, so there is no
 * safe way to exercise the case. */
#ifdef CONFIG_FATFS_VFS_RENAME_REJECTS_SELF_NESTING
TEST_CASE("(WL) rename refuses to move a directory into itself", "[fatfs][wear_levelling]")
{
    test_setup();

    const char *dir = "/spiflash/mv_d";
    TEST_ASSERT_EQUAL(0, mkdir(dir, 0755));

    struct stat st;

    errno = 0;
    TEST_ASSERT_EQUAL(-1, rename(dir, "/spiflash/mv_d/child"));
    TEST_ASSERT_EQUAL(EINVAL, errno);
    TEST_ASSERT_EQUAL(0, stat(dir, &st));
    TEST_ASSERT_TRUE(S_ISDIR(st.st_mode));

    /* Nesting is rejected at any depth, not just directly below the source. */
    errno = 0;
    TEST_ASSERT_EQUAL(-1, rename(dir, "/spiflash/mv_d/a/b"));
    TEST_ASSERT_EQUAL(EINVAL, errno);

    /* The destination is matched by directory entry rather than by path bytes,
     * so a spelling that differs only in case is caught as well. */
    errno = 0;
    TEST_ASSERT_EQUAL(-1, rename(dir, "/spiflash/MV_D/child"));
    TEST_ASSERT_EQUAL(EINVAL, errno);
    TEST_ASSERT_EQUAL(0, stat(dir, &st));
    TEST_ASSERT_TRUE(S_ISDIR(st.st_mode));

    /* The 8.3 alias of a long name denotes the same directory, which a
     * comparison of path bytes would not recognise. */
    const char *long_dir = "/spiflash/longdirname";
    TEST_ASSERT_EQUAL(0, mkdir(long_dir, 0755));
    errno = 0;
    TEST_ASSERT_EQUAL(-1, rename(long_dir, "/spiflash/LONGDI~1/child"));
    TEST_ASSERT_EQUAL(EINVAL, errno);
    TEST_ASSERT_EQUAL(0, stat(long_dir, &st));
    TEST_ASSERT_TRUE(S_ISDIR(st.st_mode));
    TEST_ASSERT_EQUAL(0, rmdir(long_dir));

    /* A name that merely shares a prefix is a different directory, and moving
     * the directory elsewhere stays allowed. */
    TEST_ASSERT_EQUAL(0, rename(dir, "/spiflash/mv_dd"));
    TEST_ASSERT_EQUAL(0, stat("/spiflash/mv_dd", &st));
    TEST_ASSERT_TRUE(S_ISDIR(st.st_mode));
    TEST_ASSERT_EQUAL(0, rmdir("/spiflash/mv_dd"));

    test_teardown();
}
#endif // CONFIG_FATFS_VFS_RENAME_REJECTS_SELF_NESTING

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

TEST_CASE("(WL) readdir stat cache with two open directories", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_readdir_stat_dual_opendir("/spiflash/readdir_cache");
    test_teardown();
}

TEST_CASE("(WL) readdir stat cache stale after truncate", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_readdir_stat_stale_after_truncate("/spiflash/readdir_cache");
    test_teardown();
}

TEST_CASE("(WL) readdir stat cache stale after unlink", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_readdir_stat_stale_after_unlink("/spiflash/readdir_cache");
    test_teardown();
}

TEST_CASE("(WL) readdir stat cache concurrent dual opendir", "[fatfs][wear_levelling]")
{
    test_setup();
    test_fatfs_readdir_stat_concurrent_dual_opendir("/spiflash/readdir_cache");
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
