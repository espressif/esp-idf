/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "unity.h"
#include "esp_log.h"
#include "esp_partition.h"
#include "esp_blockdev.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "ff.h"
#include "diskio_impl.h"
#include "diskio_bdl.h"

static const char *TAG = "test_fatfs_bdl";

void app_main(void)
{
    unity_run_menu();
}

/* ===================================================================== */
/* Helper: create partition BDL and erase it                             */
/* ===================================================================== */

static esp_blockdev_handle_t s_test_bdl = NULL;

static void test_setup_partition_bdl(const char *label)
{
    esp_err_t err = esp_partition_get_blockdev(
            ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT,
            label, &s_test_bdl);
    TEST_ESP_OK(err);
    TEST_ASSERT_NOT_NULL(s_test_bdl);

    ESP_LOGI(TAG, "Partition BDL: disk_size=%llu, erase_size=%u",
             (unsigned long long)s_test_bdl->geometry.disk_size,
             (unsigned)s_test_bdl->geometry.erase_size);
}

static void test_teardown_partition_bdl(void)
{
    if (s_test_bdl) {
        s_test_bdl->ops->release(s_test_bdl);
        s_test_bdl = NULL;
    }
}

/* ===================================================================== */
/* Test: BDL diskio low-level on partition BDL                           */
/* ===================================================================== */

TEST_CASE("(BDL) diskio register, format, write and read on partition", "[fatfs][bdl]")
{
    test_setup_partition_bdl("storage");

    BYTE pdrv;
    TEST_ESP_OK(ff_diskio_get_drive(&pdrv));
    TEST_ESP_OK(ff_diskio_register_bdl(pdrv, s_test_bdl));

    char drv[3] = {(char)('0' + pdrv), ':', 0};

    WORD sec_size = 0;
    TEST_ASSERT_EQUAL(RES_OK, ff_disk_ioctl(pdrv, GET_SECTOR_SIZE, &sec_size));
    ESP_LOGI(TAG, "Sector size: %u", (unsigned)sec_size);

    DWORD sec_count = 0;
    TEST_ASSERT_EQUAL(RES_OK, ff_disk_ioctl(pdrv, GET_SECTOR_COUNT, &sec_count));
    ESP_LOGI(TAG, "Sector count: %lu", (unsigned long)sec_count);

    BYTE work_area[FF_MAX_SS];
    const MKFS_PARM opt = {(BYTE)(FM_ANY | FM_SFD), 2, 0, 0, sec_size};
    TEST_ASSERT_EQUAL(FR_OK, f_mkfs(drv, &opt, work_area, sizeof(work_area)));

    FATFS fs;
    TEST_ASSERT_EQUAL(FR_OK, f_mount(&fs, drv, 1));

    FIL file;
    UINT bw;
    TEST_ASSERT_EQUAL(FR_OK, f_open(&file, "test.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE));

    const char *test_data = "Hello from FatFS over BDL partition!";
    TEST_ASSERT_EQUAL(FR_OK, f_write(&file, test_data, strlen(test_data), &bw));
    TEST_ASSERT_EQUAL(strlen(test_data), bw);

    TEST_ASSERT_EQUAL(FR_OK, f_lseek(&file, 0));

    char read_buf[128] = {};
    TEST_ASSERT_EQUAL(FR_OK, f_read(&file, read_buf, sizeof(read_buf) - 1, &bw));
    TEST_ASSERT_EQUAL(strlen(test_data), bw);
    TEST_ASSERT_EQUAL_STRING(test_data, read_buf);

    TEST_ASSERT_EQUAL(FR_OK, f_close(&file));
    TEST_ASSERT_EQUAL(FR_OK, f_mount(0, drv, 0));

    ff_diskio_unregister(pdrv);
    ff_diskio_clear_pdrv_bdl(s_test_bdl);
    test_teardown_partition_bdl();
}

/* ===================================================================== */
/* Test: BDL VFS mount/unmount on partition BDL                          */
/* ===================================================================== */

TEST_CASE("(BDL) VFS mount, file operations and unmount", "[fatfs][bdl]")
{
    test_setup_partition_bdl("storage");

    esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
    };
    TEST_ESP_OK(esp_vfs_fat_bdl_mount("/bdltest", s_test_bdl, &mount_config));

    const char *hello_str = "Hello from BDL VFS FatFS!\n";
    const char *filename = "/bdltest/hello.txt";

    FILE *f = fopen(filename, "w");
    TEST_ASSERT_NOT_NULL(f);
    fprintf(f, "%s", hello_str);
    fclose(f);

    f = fopen(filename, "r");
    TEST_ASSERT_NOT_NULL(f);
    char buf[128] = {};
    TEST_ASSERT_NOT_NULL(fgets(buf, sizeof(buf), f));
    fclose(f);
    TEST_ASSERT_EQUAL_STRING(hello_str, buf);

    struct stat st;
    TEST_ASSERT_EQUAL(0, stat(filename, &st));
    TEST_ASSERT_EQUAL(strlen(hello_str), st.st_size);

    TEST_ASSERT_EQUAL(0, unlink(filename));

    TEST_ESP_OK(esp_vfs_fat_bdl_unmount("/bdltest", s_test_bdl));
    test_teardown_partition_bdl();
}

/* ===================================================================== */
/* Test: BDL geometry is correct for partition BDL                       */
/* ===================================================================== */

TEST_CASE("(BDL) partition BDL geometry matches partition size", "[fatfs][bdl]")
{
    const esp_partition_t *part = esp_partition_find_first(
            ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, "storage");
    TEST_ASSERT_NOT_NULL(part);

    test_setup_partition_bdl("storage");

    TEST_ASSERT_EQUAL(part->size, s_test_bdl->geometry.disk_size);
    TEST_ASSERT(s_test_bdl->geometry.erase_size > 0);
    TEST_ASSERT(s_test_bdl->geometry.read_size > 0);
    TEST_ASSERT_EQUAL(0, s_test_bdl->geometry.disk_size % s_test_bdl->geometry.erase_size);

    test_teardown_partition_bdl();
}

/* ===================================================================== */
/* Test: Two BDL volumes on separate partitions                          */
/* ===================================================================== */

TEST_CASE("(BDL) two BDL volumes coexist", "[fatfs][bdl]")
{
    esp_blockdev_handle_t bdl1 = NULL;
    esp_blockdev_handle_t bdl2 = NULL;

    TEST_ESP_OK(esp_partition_get_blockdev(
            ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT,
            "storage", &bdl1));
    TEST_ESP_OK(esp_partition_get_blockdev(
            ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT,
            "storage2", &bdl2));

    esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
    };

    TEST_ESP_OK(esp_vfs_fat_bdl_mount("/bdl1", bdl1, &mount_config));
    TEST_ESP_OK(esp_vfs_fat_bdl_mount("/bdl2", bdl2, &mount_config));

    FILE *f1 = fopen("/bdl1/a.txt", "w");
    TEST_ASSERT_NOT_NULL(f1);
    fprintf(f1, "vol1");
    fclose(f1);

    FILE *f2 = fopen("/bdl2/b.txt", "w");
    TEST_ASSERT_NOT_NULL(f2);
    fprintf(f2, "vol2");
    fclose(f2);

    char buf[16] = {};
    f1 = fopen("/bdl1/a.txt", "r");
    TEST_ASSERT_NOT_NULL(f1);
    fgets(buf, sizeof(buf), f1);
    fclose(f1);
    TEST_ASSERT_EQUAL_STRING("vol1", buf);

    memset(buf, 0, sizeof(buf));
    f2 = fopen("/bdl2/b.txt", "r");
    TEST_ASSERT_NOT_NULL(f2);
    fgets(buf, sizeof(buf), f2);
    fclose(f2);
    TEST_ASSERT_EQUAL_STRING("vol2", buf);

    TEST_ESP_OK(esp_vfs_fat_bdl_unmount("/bdl1", bdl1));
    TEST_ESP_OK(esp_vfs_fat_bdl_unmount("/bdl2", bdl2));

    bdl1->ops->release(bdl1);
    bdl2->ops->release(bdl2);
}
