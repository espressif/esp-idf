#include <stdio.h>
#include <string.h>

#include "ff.h"
#include "esp_partition.h"
#include "wear_levelling.h"
#include "diskio_impl.h"
#include "diskio_wl.h"

#include "catch.hpp"

extern "C" void _spi_flash_init(const char* chip_size, size_t block_size, size_t sector_size, size_t page_size, const char* partition_bin);

TEST_CASE("create volume, open file, write and read back data", "[fatfs]")
{
    _spi_flash_init(CONFIG_ESPTOOLPY_FLASHSIZE, CONFIG_WL_SECTOR_SIZE * 16, CONFIG_WL_SECTOR_SIZE, CONFIG_WL_SECTOR_SIZE, "partition_table.bin");

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

    free(read);
    free(data);
}
