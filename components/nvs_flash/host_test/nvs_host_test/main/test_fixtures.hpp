/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "nvs_partition.hpp"
#include "esp_private/partition_linux.h"
#include "nvs.h"

class PartitionEmulationFixture {
public:
    PartitionEmulationFixture(  uint32_t start_sector = 0,
                                uint32_t sector_size = 1,
                                const char *partition_name = NVS_DEFAULT_PART_NAME) :
        esp_partition()
    {

        if (esp_partition_file_mmap((const uint8_t **) &p_part_desc_addr_start) != ESP_OK) {
            throw ("Failed to initialize esp_partition_file_mmap");
        }

        esp_partition.address = start_sector * SPI_FLASH_SEC_SIZE;
        esp_partition.size = sector_size * SPI_FLASH_SEC_SIZE;
        esp_partition.erase_size = ESP_PARTITION_EMULATED_SECTOR_SIZE;
        strncpy(esp_partition.label, partition_name, PART_NAME_MAX_SIZE);
        p_part = new (std::nothrow) nvs::NVSPartition(&esp_partition);
        CHECK(p_part != nullptr);
    }

    ~PartitionEmulationFixture()
    {
        delete p_part;
        esp_partition_file_munmap();
    }

    nvs::NVSPartition *part()
    {
        return p_part;
    }

    nvs::NVSPartition *p_part;
    esp_partition_t esp_partition;
    uint8_t *p_part_desc_addr_start;
};
