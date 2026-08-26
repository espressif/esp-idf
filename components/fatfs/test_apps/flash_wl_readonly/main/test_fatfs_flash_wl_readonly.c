/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include "unity.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "wear_levelling.h"

static const char *BASE_PATH = "/spiflash";
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

static void prepare_filesystem(void)
{
    esp_vfs_fat_mount_config_t cfg = {
        .format_if_mount_failed = true,
        .max_files = 5,
    };
    TEST_ESP_OK(esp_vfs_fat_spiflash_mount_rw_wl(BASE_PATH, NULL, &cfg, &s_wl_handle));

    FILE *f = fopen("/spiflash/hello.txt", "w");
    TEST_ASSERT_NOT_NULL(f);
    fprintf(f, "hello");
    fclose(f);

    TEST_ASSERT_EQUAL(0, mkdir("/spiflash/testdir", 0755));

    TEST_ESP_OK(esp_vfs_fat_spiflash_unmount_rw_wl(BASE_PATH, s_wl_handle));
    s_wl_handle = WL_INVALID_HANDLE;
}

static void mount_readonly(void)
{
    esp_vfs_fat_mount_config_t cfg = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .read_only = true,
    };
    TEST_ESP_OK(esp_vfs_fat_spiflash_mount_rw_wl(BASE_PATH, NULL, &cfg, &s_wl_handle));
}

static void unmount(void)
{
    TEST_ESP_OK(esp_vfs_fat_spiflash_unmount_rw_wl(BASE_PATH, s_wl_handle));
    s_wl_handle = WL_INVALID_HANDLE;
}

void app_main(void)
{
    prepare_filesystem();
    unity_run_menu();
}

TEST_CASE("read_only mount: can read existing file", "[fatfs][readonly]")
{
    mount_readonly();

    FILE *f = fopen("/spiflash/hello.txt", "r");
    TEST_ASSERT_NOT_NULL(f);
    char buf[16] = {};
    TEST_ASSERT_EQUAL(5, fread(buf, 1, sizeof(buf), f));
    TEST_ASSERT_EQUAL_STRING("hello", buf);
    fclose(f);

    unmount();
}

TEST_CASE("read_only mount: open for write fails with EROFS", "[fatfs][readonly]")
{
    mount_readonly();

    int fd = open("/spiflash/new.txt", O_CREAT | O_RDWR, 0666);
    TEST_ASSERT_EQUAL(-1, fd);
    TEST_ASSERT_EQUAL(EROFS, errno);

    fd = open("/spiflash/hello.txt", O_WRONLY);
    TEST_ASSERT_EQUAL(-1, fd);
    TEST_ASSERT_EQUAL(EROFS, errno);

    unmount();
}

TEST_CASE("read_only mount: mkdir fails with EROFS", "[fatfs][readonly]")
{
    mount_readonly();

    TEST_ASSERT_EQUAL(-1, mkdir("/spiflash/newdir", 0755));
    TEST_ASSERT_EQUAL(EROFS, errno);

    unmount();
}

TEST_CASE("read_only mount: unlink fails with EROFS", "[fatfs][readonly]")
{
    mount_readonly();

    TEST_ASSERT_EQUAL(-1, unlink("/spiflash/hello.txt"));
    TEST_ASSERT_EQUAL(EROFS, errno);

    unmount();
}

TEST_CASE("read_only mount: rename fails with EROFS", "[fatfs][readonly]")
{
    mount_readonly();

    TEST_ASSERT_EQUAL(-1, rename("/spiflash/hello.txt", "/spiflash/bye.txt"));
    TEST_ASSERT_EQUAL(EROFS, errno);

    unmount();
}

TEST_CASE("read_only mount: stat and opendir still work", "[fatfs][readonly]")
{
    mount_readonly();

    struct stat st;
    TEST_ASSERT_EQUAL(0, stat("/spiflash/hello.txt", &st));
    TEST_ASSERT(st.st_mode & S_IFREG);

    TEST_ASSERT_EQUAL(0, stat("/spiflash/testdir", &st));
    TEST_ASSERT(st.st_mode & S_IFDIR);

    DIR *dir = opendir("/spiflash");
    TEST_ASSERT_NOT_NULL(dir);
    TEST_ASSERT_EQUAL(0, closedir(dir));

    unmount();
}

TEST_CASE("read_only mount: format_if_mount_failed and read_only are mutually exclusive", "[fatfs][readonly]")
{
    esp_vfs_fat_mount_config_t cfg = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .read_only = true,
    };
    wl_handle_t wl_handle = WL_INVALID_HANDLE;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      esp_vfs_fat_spiflash_mount_rw_wl(BASE_PATH, NULL, &cfg, &wl_handle));
}
