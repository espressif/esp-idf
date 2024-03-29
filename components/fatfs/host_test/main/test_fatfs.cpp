/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>

#include "ff.h"
#include "esp_partition.h"
#include "wear_levelling.h"
#include "diskio_impl.h"
#include "diskio_wl.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Create volume, open file, write and read back data", "[fatfs]")
{
    FRESULT fr_result;
    BYTE pdrv;
    FATFS fs;
    FIL file;
    UINT bw;

    esp_err_t esp_result;

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, "storage");

    // Mount wear-levelled partition
    wl_handle_t wl_handle;
    esp_result = wl_mount(partition, &wl_handle);
    REQUIRE(esp_result == ESP_OK);

    // Get a physical drive
    esp_result = ff_diskio_get_drive(&pdrv);
    REQUIRE(esp_result == ESP_OK);

    // Register physical drive as wear-levelled partition
    esp_result = ff_diskio_register_wl_partition(pdrv, wl_handle);

    // Create FAT volume on the entire disk
    LBA_t part_list[] = {100, 0, 0, 0};
    BYTE work_area[FF_MAX_SS];

    fr_result = f_fdisk(pdrv, part_list, work_area);
    REQUIRE(fr_result == FR_OK);
    const MKFS_PARM opt = {(BYTE)FM_ANY, 0, 0, 0, 0};
    fr_result = f_mkfs("", &opt, work_area, sizeof(work_area)); // Use default volume

    // Mount the volume
    fr_result = f_mount(&fs, "", 0);
    REQUIRE(fr_result == FR_OK);

    // Open, write and read data
    fr_result = f_open(&file, "test.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
    REQUIRE(fr_result == FR_OK);

    // Generate data
    uint32_t data_size = 100000;

    char *data = (char*) malloc(data_size);
    char *read = (char*) malloc(data_size);

    for(uint32_t i = 0; i < data_size; i += sizeof(i))
    {
        *((uint32_t*)(data + i)) = i;
    }

    // Write generated data
    fr_result = f_write(&file, data, data_size, &bw);
    REQUIRE(fr_result == FR_OK);
    REQUIRE(bw == data_size);

    // Move to beginning of file
    fr_result = f_lseek(&file, 0);
    REQUIRE(fr_result == FR_OK);

    // Read written data
    fr_result = f_read(&file, read, data_size, &bw);
    REQUIRE(fr_result == FR_OK);
    REQUIRE(bw == data_size);

    REQUIRE(memcmp(data, read, data_size) == 0);

    // Close file
    fr_result = f_close(&file);
    REQUIRE(fr_result == FR_OK);

    // Unmount default volume
    fr_result = f_mount(0, "", 0);
    REQUIRE(fr_result == FR_OK);

    // Clear
    free(read);
    free(data);
    ff_diskio_unregister(pdrv);
    ff_diskio_clear_pdrv_wl(wl_handle);
    esp_result = wl_unmount(wl_handle);
    REQUIRE(esp_result == ESP_OK);
}

static void prepare_fatfs(const char* partition_label, const esp_partition_t** partition, wl_handle_t* wl_handle, BYTE* pdrv)
{
    FRESULT fr_result;
    esp_err_t esp_result;

    *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, partition_label);
    REQUIRE(partition != NULL);
    printf("partition address=0x%x\n", (*partition)->address);
    printf("partition size=0x%x\n", (*partition)->size);

    // Mount wear-levelled partition
    esp_result = wl_mount(*partition, wl_handle);
    REQUIRE(esp_result == ESP_OK);

    // Get a physical drive
    BYTE _pdrv;
    esp_result = ff_diskio_get_drive(&_pdrv);
    REQUIRE(esp_result == ESP_OK);
    printf("using pdrv=%i\n", _pdrv);
    char drv[3] = {(char)('0' + _pdrv), ':', 0};
    *pdrv = _pdrv;

    // Register physical drive as wear-levelled partition
    esp_result = ff_diskio_register_wl_partition(_pdrv, *wl_handle);

    // Create FAT volume on the entire disk
    LBA_t part_list[] = {100, 0, 0, 0};
    BYTE work_area[FF_MAX_SS];

    fr_result = f_fdisk(_pdrv, part_list, work_area);
    REQUIRE(fr_result == FR_OK);
    const MKFS_PARM opt = {(BYTE)FM_ANY, 0, 0, 0, 0};
    fr_result = f_mkfs(drv, &opt, work_area, sizeof(work_area)); // Use default volume
    REQUIRE(fr_result == FR_OK);
}

/*
 * This just tests formatting from FATFS library itself, not directly VFS FATFS (SPIFLASH) API
 * like `esp_vfs_fat_spiflash_format_rw_wl` function, since `vfs` is not buildable on linux host
 * at the time of writing this - therefore there also is a device test_apps test in
 * `components/fatfs/test_apps/flash_wl/main/test_fatfs_flash_wl.c` which tests our VFS FATFS SPIFLASH API.
 */
TEST_CASE("Test mounting 2 volumes, writing data and formatting the 2nd one, reading data", "[fatfs]")
{
    FRESULT fr_result;
    esp_err_t esp_result;

    const char* partition_label0 = "storage";
    const esp_partition_t *partition0 = NULL;
    BYTE pdrv0 = UINT8_MAX;
    FATFS fs0;
    wl_handle_t wl_handle0 = WL_INVALID_HANDLE;

    const char* partition_label1 = "storage2";
    const esp_partition_t *partition1 = NULL;
    BYTE pdrv1 = UINT8_MAX;
    FATFS fs1;
    wl_handle_t wl_handle1 = WL_INVALID_HANDLE;

    size_t data_size = 10;


    // Mount the volume 0
    prepare_fatfs(partition_label0, &partition0, &wl_handle0, &pdrv0);
    REQUIRE(partition0 != NULL);
    REQUIRE(wl_handle0 != WL_INVALID_HANDLE);
    REQUIRE(pdrv0 == 0);
    char drv0[3] = {(char)('0' + pdrv0), ':', 0};
    fr_result = f_mount(&fs0, drv0, 0);
    REQUIRE(fr_result == FR_OK);

    // Open file and write data
    FIL file0;
    UINT bw0;
    fr_result = f_open(&file0, "0:/test0.txt", FA_OPEN_ALWAYS | FA_WRITE);
    REQUIRE(fr_result == FR_OK);
    // Write data
    const char *data0 = "123456789";
    char read0[10] = {0};
    fr_result = f_write(&file0, data0, data_size, &bw0);
    REQUIRE(fr_result == FR_OK);
    REQUIRE(bw0 == data_size);
    // Close file
    fr_result = f_close(&file0);
    REQUIRE(fr_result == FR_OK);

    // Unmount volume 0
    fr_result = f_mount(0, drv0, 0);
    REQUIRE(fr_result == FR_OK);


    // Mount the volume 1
    prepare_fatfs(partition_label1, &partition1, &wl_handle1, &pdrv1);
    REQUIRE(partition1 != NULL);
    REQUIRE(wl_handle1 != WL_INVALID_HANDLE);
    REQUIRE(pdrv1 == 1);
    char drv1[3] = {(char)('0' + pdrv1), ':', 0};
    fr_result = f_mount(&fs1, drv1, 0);
    REQUIRE(fr_result == FR_OK);

    // Open file and write data
    FIL file1;
    UINT bw1;
    fr_result = f_open(&file1, "1:/test1.txt", FA_OPEN_ALWAYS | FA_WRITE);
    REQUIRE(fr_result == FR_OK);
    // Write data
    const char* data1 = "987654321";
    char read1[10] = {0};
    fr_result = f_write(&file1, data1, data_size, &bw1);
    REQUIRE(fr_result == FR_OK);
    REQUIRE(bw1 == data_size);
    // Close file
    fr_result = f_close(&file1);
    REQUIRE(fr_result == FR_OK);

    // Unmount volume 1
    fr_result = f_mount(0, drv1, 0);
    REQUIRE(fr_result == FR_OK);

    // Format the volume 1
    const size_t workbuf_size = 4096;
    void *workbuf = ff_memalloc(workbuf_size);
    REQUIRE(workbuf != NULL);
    const MKFS_PARM opt = {(BYTE)(FM_ANY | FM_SFD), 0, 0, 0, CONFIG_WL_SECTOR_SIZE};
    fr_result = f_mkfs(drv1, &opt, workbuf, workbuf_size);
    free(workbuf);
    workbuf = NULL;
    REQUIRE(fr_result == FR_OK);
    printf("partition1 formatted\n");


    // Remount the volume 1
    fr_result = f_mount(&fs1, drv1, 1);
    REQUIRE(fr_result == FR_OK);
    // Open file and read data from file1
    fr_result = f_open(&file1, "1:/test1.txt", FA_OPEN_ALWAYS | FA_READ);
    REQUIRE(fr_result == FR_OK);
    // Read written data from file1
    fr_result = f_read(&file1, read1, data_size, &bw1);
    REQUIRE(fr_result == FR_OK);
    REQUIRE(bw1 != data_size);
    // Compare data
    printf("data1=%s, read1=%s\n", data1, read1);
    REQUIRE(strncmp(data1, read1, data_size-1) != 0); // 987654321 should be ersead due to formatting
    // Close file from file1
    fr_result = f_close(&file1);
    REQUIRE(fr_result == FR_OK);


    // Remount the volume 0
    fr_result = f_mount(&fs0, drv0, 1);
    REQUIRE(fr_result == FR_OK);
    // Open file and read data from file0
    fr_result = f_open(&file0, "0:/test0.txt", FA_OPEN_ALWAYS | FA_READ);
    REQUIRE(fr_result == FR_OK);
    // Read written data from file0
    fr_result = f_read(&file0, read0, data_size, &bw0);
    REQUIRE(fr_result == FR_OK);
    REQUIRE(bw0 == data_size);
    // Compare data
    printf("data0=%s, read0=%s\n", data0, read0);
    REQUIRE(strncmp(data0, read0, data_size-1) == 0); // should match since the partition was not formatted
    // Close file from file0
    fr_result = f_close(&file0);
    REQUIRE(fr_result == FR_OK);


    // Unmount both volumes
    fr_result = f_mount(0, drv0, 0);
    REQUIRE(fr_result == FR_OK);
    fr_result = f_mount(0, drv1, 0);
    REQUIRE(fr_result == FR_OK);

    // Clear
    ff_diskio_unregister(pdrv0);
    ff_diskio_unregister(pdrv1);
    ff_diskio_clear_pdrv_wl(wl_handle0);
    ff_diskio_clear_pdrv_wl(wl_handle1);
    esp_result = wl_unmount(wl_handle0);
    REQUIRE(esp_result == ESP_OK);
    esp_result = wl_unmount(wl_handle1);
    REQUIRE(esp_result == ESP_OK);
}
