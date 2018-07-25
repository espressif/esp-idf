#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_spi_flash.h"
#include "esp_partition.h"
#include "wear_levelling.h"
#include "WL_Flash.h"
#include "SpiFlash.h"

#include "catch.hpp"

#include "sdkconfig.h"

extern "C" void init_spi_flash(const char* chip_size, size_t block_size, size_t sector_size, size_t page_size, const char* partition_bin);
extern SpiFlash spiflash;

#define TEST_COUNT_MAX 100

TEST_CASE("write and read back data", "[wear_levelling]")
{
    init_spi_flash(CONFIG_ESPTOOLPY_FLASHSIZE, CONFIG_WL_SECTOR_SIZE * 16, CONFIG_WL_SECTOR_SIZE, CONFIG_WL_SECTOR_SIZE, "partition_table.bin");

    esp_err_t result;
    wl_handle_t wl_handle;

    int flash_handle;
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");

    // Mount wear-levelled partition
    result = wl_mount(partition, &wl_handle);
    REQUIRE(result == ESP_OK);

    // Get the sector size
    uint32_t sector_size = wl_sector_size(wl_handle);
    REQUIRE(sector_size == CONFIG_WL_SECTOR_SIZE);

    uint8_t* data = (uint8_t*) malloc(partition->size);
    uint8_t* read = (uint8_t*) malloc(partition->size);

    uint32_t sectors = partition->size / sector_size;

    // Generate data
    for(uint32_t sector = 0; sector < sectors; sector++)
    {
        uint32_t sector_address = sector * sector_size;

        for(uint32_t i = 0; i < sector_size / sizeof(i); i++)
        {
            ((uint32_t*) data)[i] = sector_address + i;
        }
    }

    // Write data
    result = wl_write(wl_handle, 0, data, partition->size);
    REQUIRE(result == ESP_OK);

    // Read data
    result = wl_read(wl_handle, 0, read, partition->size);
    REQUIRE(result == ESP_OK);

    // Verify that written and read data match
    REQUIRE(memcmp(data, read, partition->size));

    // Erase some ranges
    result = wl_erase_range(wl_handle, 0, sector_size);
    REQUIRE(result == ESP_OK);
    result = wl_erase_range(wl_handle, 12288, sector_size * 2);
    REQUIRE(result == ESP_OK);
    result = wl_erase_range(wl_handle, 28672, sector_size * 3);
    REQUIRE(result == ESP_OK);

    // Expected data after erasure
    memset(data + 0, 0xFF, sector_size);
    memset(data + 12288, 0xFF, sector_size * 2);
    memset(data + 28672, 0xFF, sector_size * 3);

    // Read again, with erased ranges
    result = wl_read(wl_handle, 0, read, partition->size);
    REQUIRE(result == ESP_OK);

    // Verify that written and read data match
    REQUIRE(memcmp(data, read, partition->size));

    // Unmount
    result = wl_unmount(wl_handle);
    REQUIRE(result == ESP_OK);

    free(data);
    free(read);
}

TEST_CASE("power down test", "[wear_levelling]")
{
    init_spi_flash(CONFIG_ESPTOOLPY_FLASHSIZE, CONFIG_WL_SECTOR_SIZE * 16, CONFIG_WL_SECTOR_SIZE, CONFIG_WL_SECTOR_SIZE, "partition_table.bin");

    esp_err_t result;
    wl_handle_t wl_handle;

    int flash_handle;
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");

    // Mount wear-levelled partition
    result = wl_mount(partition, &wl_handle);
    REQUIRE(result == ESP_OK);

    // Get wl partition information
    size_t sector_size = wl_sector_size(wl_handle);
    int32_t sectors_count = wl_size(wl_handle) / sector_size;

    uint32_t add_const = 0;
    uint32_t *sector_data = new uint32_t[sector_size / sizeof(uint32_t)];

    // Fill partition with check data
    for (int32_t i = 0; i < sectors_count; i++) {
        REQUIRE(wl_erase_range(wl_handle, i * sector_size, sector_size) == ESP_OK);
        for (uint32_t m = 0; m < sector_size / sizeof(uint32_t); m++) {
            uint32_t temp_data = i * sector_size + add_const + m;
            sector_data[m] = temp_data;
        }
        REQUIRE(wl_write(wl_handle, i * sector_size, sector_data, sector_size) == ESP_OK);
    }

    for (int32_t i = 0; i < sectors_count; i++) {
        result |= wl_read(wl_handle, i * sector_size, sector_data, sector_size);
        for (uint32_t m = 0; m < sector_size / sizeof(uint32_t); m++) {
            uint32_t temp_data = i * sector_size + add_const + m;
            REQUIRE(temp_data == sector_data[m]);
            if (temp_data != sector_data[m]) {
                printf("Error - read: %08x, expected %08x\n", sector_data[m], temp_data);
            }
        }
    }

    // Perform test
    int32_t max_count = 100;
    int32_t max_check_count = TEST_COUNT_MAX;
    
    printf("used_sectors_count=%d\n", max_check_count);

    for (int32_t k = 0; k < max_check_count; k++) {

        spiflash.set_total_erase_cycles_limit(max_count);

        int32_t err_sector = -1;
        for (int32_t i = 0; i < sectors_count; i++) {
            result = ESP_OK;
            result = wl_erase_range(wl_handle, i * sector_size, sector_size);
            if (result != ESP_OK) {
                err_sector = i;
                break;
            }
            for (uint32_t m = 0; m < sector_size / sizeof(uint32_t); m++) {
                uint32_t temp_data = i * sector_size + add_const + m;
                sector_data[m] = temp_data;
            }
            result = wl_write(wl_handle, i * sector_size, sector_data, sector_size);
            if (result != ESP_OK) {
                err_sector = i;
                break;
            }
        }

        if (err_sector >= 0) {
            max_count++;
        } else {
            max_count = 0;
        }

        spiflash.set_total_erase_cycles_limit(0);

        result = wl_unmount(wl_handle);
        REQUIRE(result == ESP_OK);

        result = wl_mount(partition, &wl_handle);
        REQUIRE(result == ESP_OK);

        for (int32_t i = 0; i < sectors_count; i++) {
            if (i != err_sector) {
                result |= wl_read(wl_handle, i * sector_size, sector_data, sector_size);
                for (uint32_t m = 0; m < sector_size / sizeof(uint32_t); m++) {
                    uint32_t temp_data = i * sector_size + add_const + m;
                    REQUIRE(temp_data == sector_data[m]);
                    if (temp_data != sector_data[m]) {
                        printf("Error - read: %08x, expected %08x, m=%i, sector=%i\n", sector_data[m], temp_data, m, i);
                    }
                }
            }
        }

        if (err_sector != -1) {
            result |= wl_erase_range(wl_handle, err_sector * sector_size, sector_size);
            for (uint32_t m = 0; m < sector_size / sizeof(uint32_t); m++) {
                uint32_t temp_data = err_sector * sector_size + add_const + m;
                sector_data[m] = temp_data;
            }
            result |= wl_write(wl_handle, err_sector * sector_size, sector_data, sector_size);
        }

        spiflash.reset_total_erase_cycles();

        printf("[%3.f%%] err_sector=%i\n", (float)k / ((float)max_check_count) * 100.0f, err_sector);
    }

    delete[] sector_data;

    // Unmount
    result = wl_unmount(wl_handle);
    REQUIRE(result == ESP_OK);
}