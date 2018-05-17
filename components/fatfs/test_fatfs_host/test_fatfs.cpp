#include <stdio.h>
#include <string.h>

#include "ff.h"
#include "esp_partition.h"
#include "wear_levelling.h"
#include "diskio.h"
#include "diskio_spiflash.h"

#include "catch.hpp"

TEST_CASE("create volume, open file, write and read back data", "[fatfs]")
{
    FRESULT fr_result;
    BYTE pdrv;
    FATFS fs;
    FIL file;
    UINT bw;

    esp_err_t esp_result;

    // Create a 4MB partition
    uint32_t size = 0x00400000;
    int flash_handle = esp_flash_create(size, 4096, 1);
    esp_partition_t partition = esp_partition_create(size, 0, flash_handle);

    // Mount wear-levelled partition
    wl_handle_t wl_handle;
    esp_result = wl_mount(&partition, &wl_handle);
    REQUIRE(esp_result == ESP_OK);

    // Get a physical drive
    esp_result = ff_diskio_get_drive(&pdrv);
    REQUIRE(esp_result == ESP_OK);

    // Register physical drive as wear-levelled partition
    esp_result = ff_diskio_register_wl_partition(pdrv, wl_handle);

    // Create FAT volume on the entire disk
    DWORD part_list[] = {100, 0, 0, 0};
    BYTE work_area[FF_MAX_SS];

    fr_result = f_fdisk(pdrv, part_list, work_area);
    REQUIRE(fr_result == FR_OK);
    fr_result = f_mkfs("", FM_ANY, 0, work_area, sizeof(work_area)); // Use default volume

    // Mount the volume
    fr_result = f_mount(&fs, "", 0);
    REQUIRE(fr_result == FR_OK);

    // Open, write and read data
    fr_result = f_open(&file, "test.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
    REQUIRE(fr_result == FR_OK);

    const char data[] = "Hello, World!";
    char *read = (char*) malloc(sizeof(data));

    fr_result = f_write(&file, data, sizeof(data), &bw);
    REQUIRE(fr_result == FR_OK);
    REQUIRE(bw == sizeof(data));

    // Move to beginning of file
    fr_result = f_lseek(&file, 0);
    REQUIRE(fr_result == FR_OK);

    fr_result = f_read(&file, read, sizeof(data), &bw);
    REQUIRE(fr_result == FR_OK);
    REQUIRE(bw == sizeof(data));

    REQUIRE(strcmp(data, read) == 0);

    // Close file
    fr_result = f_close(&file);
    REQUIRE(fr_result == FR_OK);

    // Unmount default volume
    fr_result = f_mount(0, "", 0);
    REQUIRE(fr_result == FR_OK);

    esp_partition_delete(partition);

    free(read);
}