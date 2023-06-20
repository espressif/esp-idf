/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "nvs_partition.hpp"
#include "esp_private/partition_linux.h"
#include "nvs.h"
#include <random>
#include <fcntl.h>
#include <unistd.h>

class PartitionEmulationFixture {
public:
    PartitionEmulationFixture(  uint32_t start_sector = 0,
                                uint32_t sector_size = 1,
                                const char *partition_name = NVS_DEFAULT_PART_NAME) :
        esp_partition()
    {

        if (esp_partition_file_mmap((const uint8_t **) &p_part_desc_addr_start) != ESP_OK) {
            FAIL("Failed to initialize esp_partition_file_mmap");
        }

        esp_partition.address = start_sector * SPI_FLASH_SEC_SIZE;
        esp_partition.size = (start_sector + sector_size) * SPI_FLASH_SEC_SIZE;
        esp_partition.erase_size = ESP_PARTITION_EMULATED_SECTOR_SIZE;
        esp_partition.type = ESP_PARTITION_TYPE_DATA;
        esp_partition.subtype = ESP_PARTITION_SUBTYPE_DATA_NVS;
        strncpy(esp_partition.label, partition_name, PART_NAME_MAX_SIZE);
        p_part = new (std::nothrow) nvs::NVSPartition(&esp_partition);
        REQUIRE(p_part != nullptr);
    }

    // initializes the partition and loads partition binary file into it
    PartitionEmulationFixture(  uint32_t start_sector,
                                uint32_t sector_size,
                                const char *partition_name,
                                const char *partition_binary) : PartitionEmulationFixture(start_sector, sector_size, partition_name)
    {
        int file_fd = -1;
        off_t size = -1;
        void *p_buff = nullptr;
        char const *fail_msg = nullptr;

        do {
            // get file size
            file_fd = open(partition_binary, O_RDONLY);
            if (file_fd == -1) {
                fail_msg = "Failed to open file with partition content";
                break;
            }
            size = lseek(file_fd, 0L, SEEK_END);
            if (size < 0) {
                fail_msg = "falied to seek in file with partition content";
                break;
            }

            // check if file fits into the partitiion
            if (size > sector_size * SPI_FLASH_SEC_SIZE) {
                fail_msg = "file with partition content doesn't fit into the partition";
                break;
            }

            // allocate local buffer
            p_buff = malloc((size_t) size);
            if (p_buff == nullptr) {
                fail_msg = "unable to allocate buffer for reading file with partition content";
                break;
            }

            // laoad file into local buffer
            int res = lseek(file_fd, 0L, SEEK_SET);
            if (res < 0) {
                fail_msg = "falied to seek in file with partition content";
                break;
            }
            size = read(file_fd, p_buff, size);
            if (size < 0) {
                fail_msg = "cannot read file with partition content";
                break;
            }

            // erase whole partition
            if (ESP_OK != esp_partition_erase_range(&esp_partition, 0, sector_size * SPI_FLASH_SEC_SIZE)) {
                fail_msg = "cannot erase partition prior to write partition binary from file";
                break;
            }

            // write local buffer to the partition
            if (ESP_OK != esp_partition_write_raw(&esp_partition, 0, p_buff, size)) {
                fail_msg = "cannot write to the partition";
                break;
            }
        } while (false);

        // close file
        if (file_fd >= 0) {
            close(file_fd);
        }

        // deallocate buffer
        if (p_buff != nullptr) {
            free(p_buff);
        }

        if(fail_msg != nullptr) {
            FAIL(fail_msg);
        }
    }

    void randomize(uint32_t seed)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        gen.seed(seed);

        esp_partition_file_mmap_ctrl_t *p_ctrl = esp_partition_get_file_mmap_ctrl_act();
        REQUIRE(p_ctrl != nullptr);
        std::generate_n(p_part_desc_addr_start, p_ctrl->flash_file_size, gen);
    }

    // absolute sectorNumber is used here
    bool erase(size_t sectorNumber)
    {
        size_t offset = sectorNumber * SPI_FLASH_SEC_SIZE;

        // check the upper bound
        esp_partition_file_mmap_ctrl_t *p_ctrl = esp_partition_get_file_mmap_ctrl_act();
        REQUIRE(p_ctrl != nullptr);
        if (offset > p_ctrl->flash_file_size) {
            return false;
        }

        // esp_partition_erase_range uses offset relative to the begining of partition
        return (esp_partition_erase_range(&esp_partition,
                                          offset - esp_partition.address,
                                          SPI_FLASH_SEC_SIZE) == ESP_OK);
    }

    ~PartitionEmulationFixture()
    {
        delete p_part;

        // ensure underlying mmaped file gets deleted after unmap.
        esp_partition_file_mmap_ctrl_t *p_ctrl = esp_partition_get_file_mmap_ctrl_input();
        p_ctrl->remove_dump = true;
        esp_partition_file_munmap();
    }

    nvs::NVSPartition *part()
    {
        return p_part;
    }

    const esp_partition_t *get_esp_partition() const
    {
        return &esp_partition;
    }

    nvs::NVSPartition *p_part;
    esp_partition_t esp_partition;
    uint8_t *p_part_desc_addr_start;
};

// fixture with 2 partitions
class PartitionEmulationFixture2 : public PartitionEmulationFixture {
public:
    PartitionEmulationFixture2( uint32_t start_sector1 = 0,
                                uint32_t sector_size1 = 1,
                                const char *partition_name1 = "nvs1",
                                uint32_t start_sector2 = 1,
                                uint32_t sector_size2 = 1,
                                const char *partition_name2 = "nvs2"
                              ) :
        PartitionEmulationFixture(start_sector1, sector_size1, partition_name1), esp_partition2()
    {
        // for 2nd partition
        esp_partition2.address = start_sector2 * SPI_FLASH_SEC_SIZE;
        esp_partition2.size = (start_sector2 + sector_size2) * SPI_FLASH_SEC_SIZE;
        esp_partition2.erase_size = ESP_PARTITION_EMULATED_SECTOR_SIZE;
        esp_partition2.type = ESP_PARTITION_TYPE_DATA;
        esp_partition2.subtype = ESP_PARTITION_SUBTYPE_DATA_NVS;
        strncpy(esp_partition2.label, partition_name2, PART_NAME_MAX_SIZE);
        p_part2 = new (std::nothrow) nvs::NVSPartition(&esp_partition2);
        REQUIRE(p_part2 != nullptr);
    }

    ~PartitionEmulationFixture2()
    {
        delete p_part2;
    }

    nvs::NVSPartition *part2()
    {
        return p_part2;
    }

    nvs::NVSPartition *p_part2;
    esp_partition_t esp_partition2;
};
