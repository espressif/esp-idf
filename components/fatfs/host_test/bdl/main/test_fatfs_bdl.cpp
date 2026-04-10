/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>

#include "ff.h"
#include "diskio_impl.h"
#include "diskio_bdl.h"
#include "esp_blockdev.h"
#include "esp_blockdev/memory.h"
#include "esp_vfs_fat.h"
#include "esp_vfs.h"

#include "esp_partition.h"
#include "wear_levelling.h"
#include "diskio_wl.h"

#include <catch2/catch_test_macros.hpp>

/* ===================================================================== */
/* Test 1: FatFS directly on memory BDL (no wear-levelling)              */
/* ===================================================================== */

TEST_CASE("BDL: Create volume on memory BDL, write and read back data", "[fatfs][bdl]")
{
    static uint8_t backing[256 * 1024];
    memset(backing, 0xFF, sizeof(backing));

    const esp_blockdev_geometry_t geometry = {
        .disk_size = sizeof(backing),
        .read_size  = 1,
        .write_size = 1,
        .erase_size = 4096,
        .recommended_write_size = 0,
        .recommended_read_size  = 0,
        .recommended_erase_size = 0,
    };

    esp_blockdev_handle_t mem_dev = NULL;
    REQUIRE(esp_blockdev_memory_get_from_buffer(backing, sizeof(backing),
                                                &geometry, false, &mem_dev) == ESP_OK);

    BYTE pdrv;
    REQUIRE(ff_diskio_get_drive(&pdrv) == ESP_OK);
    REQUIRE(ff_diskio_register_bdl(pdrv, mem_dev) == ESP_OK);

    char drv[3] = {(char)('0' + pdrv), ':', 0};
    LBA_t part_list[] = {100, 0, 0, 0};
    BYTE work_area[FF_MAX_SS];

    REQUIRE(f_fdisk(pdrv, part_list, work_area) == FR_OK);
    const MKFS_PARM opt = {(BYTE)(FM_ANY | FM_SFD), 0, 0, 128, 0};
    REQUIRE(f_mkfs(drv, &opt, work_area, sizeof(work_area)) == FR_OK);

    FATFS fs;
    REQUIRE(f_mount(&fs, drv, 1) == FR_OK);

    FIL file;
    UINT bw;
    REQUIRE(f_open(&file, "test.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE) == FR_OK);

    uint32_t data_size = 1000;
    char *data = (char *)malloc(data_size);
    char *read_buf = (char *)malloc(data_size);
    for (uint32_t i = 0; i < data_size; i += sizeof(i)) {
        *((uint32_t *)(data + i)) = i;
    }

    REQUIRE(f_write(&file, data, data_size, &bw) == FR_OK);
    REQUIRE(bw == data_size);

    REQUIRE(f_lseek(&file, 0) == FR_OK);
    REQUIRE(f_read(&file, read_buf, data_size, &bw) == FR_OK);
    REQUIRE(bw == data_size);
    REQUIRE(memcmp(data, read_buf, data_size) == 0);

    REQUIRE(f_close(&file) == FR_OK);
    REQUIRE(f_mount(0, drv, 0) == FR_OK);

    free(read_buf);
    free(data);
    ff_diskio_unregister(pdrv);
    ff_diskio_clear_pdrv_bdl(mem_dev);
    mem_dev->ops->release(mem_dev);
}

/* ===================================================================== */
/* Test 2: FatFS BDL diskio driver registration and geometry             */
/* ===================================================================== */

TEST_CASE("BDL: Geometry is correctly reported via ioctl", "[fatfs][bdl]")
{
    static uint8_t backing[128 * 1024];
    memset(backing, 0xFF, sizeof(backing));

    const esp_blockdev_geometry_t geometry = {
        .disk_size = sizeof(backing),
        .read_size  = 1,
        .write_size = 1,
        .erase_size = 512,
        .recommended_write_size = 0,
        .recommended_read_size  = 0,
        .recommended_erase_size = 0,
    };

    esp_blockdev_handle_t mem_dev = NULL;
    REQUIRE(esp_blockdev_memory_get_from_buffer(backing, sizeof(backing),
                                                &geometry, false, &mem_dev) == ESP_OK);

    BYTE pdrv;
    REQUIRE(ff_diskio_get_drive(&pdrv) == ESP_OK);
    REQUIRE(ff_diskio_register_bdl(pdrv, mem_dev) == ESP_OK);

    WORD sec_size = 0;
    REQUIRE(ff_disk_ioctl(pdrv, GET_SECTOR_SIZE, &sec_size) == RES_OK);
    REQUIRE(sec_size == 512);

    DWORD sec_count = 0;
    REQUIRE(ff_disk_ioctl(pdrv, GET_SECTOR_COUNT, &sec_count) == RES_OK);
    REQUIRE(sec_count == sizeof(backing) / 512);

    REQUIRE(ff_disk_ioctl(pdrv, CTRL_SYNC, NULL) == RES_OK);

    ff_diskio_unregister(pdrv);
    ff_diskio_clear_pdrv_bdl(mem_dev);
    mem_dev->ops->release(mem_dev);
}

/* ===================================================================== */
/* Test 3: FatFS BDL pdrv lookup functions                               */
/* ===================================================================== */

TEST_CASE("BDL: pdrv lookup and clear", "[fatfs][bdl]")
{
    static uint8_t backing[64 * 1024];
    memset(backing, 0xFF, sizeof(backing));

    const esp_blockdev_geometry_t geometry = {
        .disk_size = sizeof(backing),
        .read_size  = 1,
        .write_size = 1,
        .erase_size = 4096,
        .recommended_write_size = 0,
        .recommended_read_size  = 0,
        .recommended_erase_size = 0,
    };

    esp_blockdev_handle_t mem_dev = NULL;
    REQUIRE(esp_blockdev_memory_get_from_buffer(backing, sizeof(backing),
                                                &geometry, false, &mem_dev) == ESP_OK);

    REQUIRE(ff_diskio_get_pdrv_bdl(mem_dev) == 0xff);

    BYTE pdrv;
    REQUIRE(ff_diskio_get_drive(&pdrv) == ESP_OK);
    REQUIRE(ff_diskio_register_bdl(pdrv, mem_dev) == ESP_OK);

    REQUIRE(ff_diskio_get_pdrv_bdl(mem_dev) == pdrv);

    ff_diskio_clear_pdrv_bdl(mem_dev);
    REQUIRE(ff_diskio_get_pdrv_bdl(mem_dev) == 0xff);

    ff_diskio_unregister(pdrv);
    mem_dev->ops->release(mem_dev);
}

/* ===================================================================== */
/* Test 4: FatFS BDL VFS mount/unmount via esp_vfs_fat_bdl_mount()       */
/* ===================================================================== */

TEST_CASE("BDL VFS: mount, write and read via POSIX API", "[fatfs][bdl][vfs]")
{
    static uint8_t backing[256 * 1024];
    memset(backing, 0xFF, sizeof(backing));

    const esp_blockdev_geometry_t geometry = {
        .disk_size = sizeof(backing),
        .read_size  = 1,
        .write_size = 1,
        .erase_size = 4096,
        .recommended_write_size = 0,
        .recommended_read_size  = 0,
        .recommended_erase_size = 0,
    };

    esp_blockdev_handle_t mem_dev = NULL;
    REQUIRE(esp_blockdev_memory_get_from_buffer(backing, sizeof(backing),
                                                &geometry, false, &mem_dev) == ESP_OK);

    esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
    };
    REQUIRE(esp_vfs_fat_bdl_mount("/bdl", mem_dev, &mount_config) == ESP_OK);

    const char *test_str = "BDL FatFS test data!\n";
    const char *filename = "/bdl/hello.txt";

    int fd = open(filename, O_CREAT | O_RDWR, 0777);
    REQUIRE(fd != -1);
    ssize_t sz = write(fd, test_str, strlen(test_str));
    REQUIRE(sz == (ssize_t)strlen(test_str));
    REQUIRE(0 == close(fd));

    fd = open(filename, O_RDONLY);
    REQUIRE(fd != -1);
    char buf[64] = {};
    sz = read(fd, buf, sizeof(buf));
    REQUIRE(sz == (ssize_t)strlen(test_str));
    REQUIRE(0 == memcmp(buf, test_str, strlen(test_str)));
    REQUIRE(0 == close(fd));

    REQUIRE(esp_vfs_fat_bdl_unmount("/bdl", mem_dev) == ESP_OK);
    mem_dev->ops->release(mem_dev);
}

/* ===================================================================== */
/* Test 5: FatFS BDL on partition (via WL legacy path for reference)     */
/* Uses the classic WL path alongside BDL to show they coexist.          */
/* ===================================================================== */

TEST_CASE("BDL and legacy WL coexist on different drives", "[fatfs][bdl]")
{
    static uint8_t backing[256 * 1024];
    memset(backing, 0xFF, sizeof(backing));

    const esp_blockdev_geometry_t geometry = {
        .disk_size = sizeof(backing),
        .read_size  = 1,
        .write_size = 1,
        .erase_size = 4096,
        .recommended_write_size = 0,
        .recommended_read_size  = 0,
        .recommended_erase_size = 0,
    };

    esp_blockdev_handle_t mem_dev = NULL;
    REQUIRE(esp_blockdev_memory_get_from_buffer(backing, sizeof(backing),
                                                &geometry, false, &mem_dev) == ESP_OK);

    const esp_partition_t *partition = esp_partition_find_first(
            ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, "storage");
    REQUIRE(partition != NULL);

    wl_handle_t wl_handle;
    REQUIRE(wl_mount(partition, &wl_handle) == ESP_OK);

    BYTE pdrv_wl;
    REQUIRE(ff_diskio_get_drive(&pdrv_wl) == ESP_OK);
    REQUIRE(ff_diskio_register_wl_partition(pdrv_wl, wl_handle) == ESP_OK);

    BYTE pdrv_bdl;
    REQUIRE(ff_diskio_get_drive(&pdrv_bdl) == ESP_OK);
    REQUIRE(ff_diskio_register_bdl(pdrv_bdl, mem_dev) == ESP_OK);

    REQUIRE(pdrv_wl != pdrv_bdl);

    WORD sec_size_wl = 0;
    REQUIRE(ff_disk_ioctl(pdrv_wl, GET_SECTOR_SIZE, &sec_size_wl) == RES_OK);

    WORD sec_size_bdl = 0;
    REQUIRE(ff_disk_ioctl(pdrv_bdl, GET_SECTOR_SIZE, &sec_size_bdl) == RES_OK);
    REQUIRE(sec_size_bdl == 4096);

    ff_diskio_unregister(pdrv_bdl);
    ff_diskio_clear_pdrv_bdl(mem_dev);
    ff_diskio_unregister(pdrv_wl);
    ff_diskio_clear_pdrv_wl(wl_handle);
    REQUIRE(wl_unmount(wl_handle) == ESP_OK);
    mem_dev->ops->release(mem_dev);
}
