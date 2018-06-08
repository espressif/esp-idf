#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_spi_flash.h"
#include "esp_partition.h"
#include "wear_levelling.h"
#include "WL_Flash.h"

#include "catch.hpp"

#include "sdkconfig.h"

extern "C" void init_spi_flash(size_t chip_size, size_t block_size, size_t sector_size, size_t page_size, const char* partition_bin);

TEST_CASE("write and read back data", "[wear_levelling]")
{
    init_spi_flash(0x00400000, CONFIG_WL_SECTOR_SIZE * 16, CONFIG_WL_SECTOR_SIZE, CONFIG_WL_SECTOR_SIZE, "partition_table.bin");

    esp_err_t result;
    wl_handle_t wl_handle;

    int flash_handle;
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, "storage");

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
