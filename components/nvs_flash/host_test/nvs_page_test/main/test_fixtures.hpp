/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "nvs_partition.hpp"
#include "esp_private/partition_linux.h"
#include "nvs.h"
#include "nvs_page.hpp"
#include "nvs_storage.hpp"
#include "nvs_partition.hpp"
#include <exception>
#include <string>
#include "esp_log.h"


struct FixtureException : std::exception {
    FixtureException(const std::string& msg) : msg(msg) { }
    const char *what() const noexcept {
        return msg.c_str();
    }

    std::string msg;
};

class PartitionEmulationFixture {
public:
    PartitionEmulationFixture(  uint32_t start_sector = 0,
                                uint32_t sector_count = 1,
                                const char *partition_name = NVS_DEFAULT_PART_NAME) :
        esp_partition(),
        part(&esp_partition)
    {
        if (esp_partition_file_mmap((const uint8_t **) &p_part_desc_addr_start) != ESP_OK) {
            TEST_FAIL_MESSAGE("Failed to initialize esp_partition_file_mmap");
        }

        esp_partition.address = start_sector * SPI_FLASH_SEC_SIZE;
        esp_partition.size = (start_sector + sector_count) * SPI_FLASH_SEC_SIZE;
        esp_partition.erase_size = ESP_PARTITION_EMULATED_SECTOR_SIZE;
        esp_partition.type = ESP_PARTITION_TYPE_DATA;
        esp_partition.subtype = ESP_PARTITION_SUBTYPE_DATA_NVS;
        strncpy(esp_partition.label, partition_name, PART_NAME_MAX_SIZE);
    }

    ~PartitionEmulationFixture()
    {
        // ensure underlying mmaped file gets deleted after unmap.
        esp_partition_file_mmap_ctrl_t *p_ctrl = esp_partition_get_file_mmap_ctrl_input();
        p_ctrl->remove_dump = true;
        esp_partition_file_munmap();
    }

public:
    // erases all partition space
    esp_err_t erase_all()
    {
        return esp_partition_erase_range(&esp_partition, esp_partition.address, esp_partition.size);
    }

    // ensures that n-th attempt to esp_partition_write or esp_partition_write_raw fails. To fail 1st attemtpt specify 1, 2nd 2...to disable, call with 0
    void fail_write_at(const size_t failing_attempt_no)
    {
        esp_partition_clear_stats();

        size_t err_delay = (failing_attempt_no == 0) ? SIZE_MAX : failing_attempt_no - 1;
        esp_partition_fail_after(err_delay, ESP_PARTITION_FAIL_AFTER_MODE_WRITE);
    }

    // ensures that n-th attempt to esp_partition_erase_range fails. To fail 1st attemtpt specify 1, 2nd 2...to disable, call with 0
    void fail_erase_at(const size_t failing_attempt_no)
    {
        esp_partition_clear_stats();

        size_t err_delay = (failing_attempt_no == 0) ? SIZE_MAX : failing_attempt_no - 1;
        esp_partition_fail_after(err_delay, ESP_PARTITION_FAIL_AFTER_MODE_ERASE);
    }

    // writes block of data to the offset relative to the beginning of partition
    esp_err_t write_raw(const size_t dst_offset, const void* src, size_t size)
    {
        // instead of esp_partition_write_raw we will write directly to the mapped memory as some of our write
        // operations are actually simulating wrong flash behaviour and linux emulator prevents usto do invalid flash operations
        // mapped memory base: p_part_desc_addr_start
        // partition begin offset: esp_partition.address
        // partition size esp_partition.size

        if((dst_offset + size) > esp_partition.size) return ESP_ERR_INVALID_SIZE;

        memcpy(p_part_desc_addr_start + esp_partition.address + dst_offset, src, size);
        return ESP_OK;
    }

    // dumps content of memory at  given dst_offset and of the size to the console
    esp_err_t dumpRaw(const size_t dst_offset, size_t size)
    {
        const size_t column_size = 4;   //number of bytes in column
        const size_t column_count = 8;  //number of columns on the line

        uint8_t buff[column_size];      // buffer for data from flash, one column

        if(size == 0) return ESP_ERR_NO_MEM;
        if(size % column_size) return ESP_ERR_INVALID_SIZE; // must be multiple of 4

        esp_err_t err = ESP_OK;

        size_t columns = size / column_size;
        volatile size_t column;

        for(column = 0; column < columns; column = column + 1)
        {
            // read column
            if((err = esp_partition_read_raw(&esp_partition, dst_offset + (column * column_size), buff, column_size)) != ESP_OK) return err;

            // print column
            printf("%02x%02x%02x%02x",
               ((uint8_t*) buff)[0],
               ((uint8_t*) buff)[1],
               ((uint8_t*) buff)[2],
               ((uint8_t*) buff)[3]);

            // if it is last column of line or last column at all, print newline, else print space
            if ( ((column+1) % column_count) == 0 || ((column+1) == columns) )
            {
                printf("\n");
            }
            else
            {
                 printf(" ");
            }

        }

        return ESP_OK;
    }

    // compares partition content at dst_offset with data block at address src of length size
    // returns ESP_OK if data matches
    // returns ESP_ERR_NOT_FOUND id data doesn't match
    // returns any other error code in case partition read operation fails
    esp_err_t compare_raw(const size_t dst_offset, const void* src, size_t size)
    {
        if(size == 0) return ESP_ERR_NO_MEM;
        if(src == nullptr) return ESP_ERR_INVALID_ARG;

        uint8_t* buff = ( uint8_t*) malloc(size);
        if(buff == nullptr) return ESP_ERR_NO_MEM;

        esp_err_t err = ESP_OK;

        do {
            if((err = esp_partition_read_raw(&esp_partition, dst_offset, buff, size)) != ESP_OK) break;
            if(memcmp(buff, src, size) != 0) err = ESP_ERR_NOT_FOUND;
        } while (false);

        if(err!=ESP_OK) {
            ESP_LOGI(TAG, "::compare_raw Data do not match. Beginning of data from flash:");
            // print up to first 32 bytes read from flash
            static const size_t max_chars = 32;
            for(size_t i = 0; i < ((max_chars < size) ? max_chars : size); i++) {
                ESP_LOGI(TAG, "0x%02x", ((uint8_t*) buff)[i]);
            }
        }

        free(buff);

        return err;
    }

protected:
    esp_partition_t esp_partition;
public:
    nvs::NVSPartition part;
private:
    uint8_t *p_part_desc_addr_start;
};

// Prepares empty (filled with 0xff) space in NVS partition and initializes nvs::Page with pointer to this data.
struct NVSPageFixture : public PartitionEmulationFixture {

    nvs::Page page;

    NVSPageFixture(uint32_t start_sector = 0,
            uint32_t sector_count = 1,
            const char *partition_name = NVS_DEFAULT_PART_NAME)
        : PartitionEmulationFixture(start_sector, sector_count, partition_name), page()
    {
        erase_all();
        if (page.load(&part, start_sector) != ESP_OK) throw FixtureException("couldn't setup page");
    }
};

// Prepares page fixture with one namespace, and one value entry
struct NVSValidPageFixture : public PartitionEmulationFixture {
    const static uint8_t NS_INDEX = 1;

    // raw header
    uint8_t raw_header [4];

    // valid header
    uint8_t raw_header_valid [32];

    // entry table with one entry
    uint8_t raw_entry_table [32];

    // namespace entry
    uint8_t ns_entry [32];

    // value entry
    uint8_t value_entry [32];

    nvs::Page page;

    NVSValidPageFixture(uint32_t start_sector = 0,
            uint32_t sector_size = 1,
            const char *partition_name = NVS_DEFAULT_PART_NAME)
        : PartitionEmulationFixture(start_sector, sector_size, partition_name),
        raw_header {0xff, 0xff, 0xff, 0xff},
        raw_header_valid {0xfe, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc2, 0x16, 0xdd, 0xdc},
        ns_entry {0x00, 0x01, 0x01, 0xff, 0x68, 0xc5, 0x3f, 0x0b, 't', 'e', 's', 't', '_', 'n', 's', '\0',
                '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        value_entry {0x01, 0x01, 0x01, 0xff, 0x3d, 0xf3, 0x99, 0xe5, 't', 'e', 's', 't', '_', 'v', 'a', 'l',
                'u', 'e', '\0', '\0', '\0', '\0', '\0', '\0', 47, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        page()
    {
        std::fill_n(raw_entry_table, sizeof(raw_entry_table)/sizeof(raw_entry_table[0]), 0);
        raw_entry_table[0] = 0xfa;

        // Expected sequence of data blocks read by page.load
        write_raw(  0, raw_header_valid, sizeof(raw_header_valid));
        write_raw( 32, raw_entry_table,  sizeof(raw_entry_table));
        write_raw(128, raw_header,       sizeof(raw_header));
        write_raw( 64, ns_entry,         sizeof(ns_entry));
        write_raw( 96, value_entry,      sizeof(value_entry));

        if (page.load(&part, start_sector) != ESP_OK) throw FixtureException("couldn't setup page");
    }
};

// Prepares page fixture with one namespace, and one blob value entry
struct NVSValidBlobPageFixture : public PartitionEmulationFixture {
    const static uint8_t NS_INDEX = 1;
    const static size_t BLOB_DATA_SIZE = 32;

    // raw header
    uint8_t raw_header [512];

    // valid header
    uint8_t raw_header_valid [32];

    // entry table with one entry
    uint8_t raw_entry_table [32];

    // namespace entry
    uint8_t ns_entry [32];

    // blob entry
    uint8_t blob_entry [32];

    // blob data space
    uint8_t blob_data [BLOB_DATA_SIZE];

    // blob index entry
    uint8_t blob_index [32];

    nvs::Page page;

    NVSValidBlobPageFixture(uint32_t start_sector = 0,
            uint32_t sector_size = 1,
            const char *partition_name = NVS_DEFAULT_PART_NAME)
        : PartitionEmulationFixture(start_sector, sector_size, partition_name),
        raw_header_valid {0xfe, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc2, 0x16, 0xdd, 0xdc},
        ns_entry {0x00, 0x01, 0x01, 0xff, 0x68, 0xc5, 0x3f, 0x0b, 't', 'e', 's', 't', '_', 'n', 's', '\0',
                '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        blob_entry {0x01, 0x42, 0x02, 0x00, 0xaa, 0xf3, 0x23, 0x87, 't', 'e', 's', 't', '_', 'b', 'l', 'o',
                'b', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 0x20, 0x00, 0xff, 0xff, 0xc6, 0x96, 0x86, 0xd9},
        blob_data {0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef,
                0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef},
        blob_index {0x01, 0x48, 0x01, 0xff, 0x42, 0x6b, 0xdf, 0x66, 't', 'e', 's', 't', '_', 'b', 'l', 'o',
                'b', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 0x20, 0x00, 0x00, 0x00, 0x01, 0x00, 0xff, 0xff},
        page()
    {
        erase_all();
        std::fill_n(raw_header, sizeof(raw_header)/sizeof(raw_header[0]), 0xFF);
        std::fill_n(raw_entry_table, sizeof(raw_entry_table)/sizeof(raw_entry_table[0]), 0xFF);
        raw_entry_table[0] = 0xaa;

        // Expected sequence of data blocks read by page.load
        // read page header 32B
        write_raw(  0, raw_header_valid, sizeof(raw_header_valid));
        // read entry table 32B
        write_raw( 32, raw_entry_table,  sizeof(raw_entry_table));
        // read next free entry's header 512B
        write_raw(192, raw_header,       sizeof(raw_header));
        // read namespace entry 32B
        write_raw( 64, ns_entry,         sizeof(ns_entry));
        // read normal blob entry + index, not the data 32B
        write_raw( 96, blob_entry,       sizeof(blob_entry));
        // not read but prepare also blob data 32B
        write_raw(128, blob_data,       sizeof(blob_data));
        // will be read twice, second time during duplicated entry check 32B
        write_raw(160, blob_index,       sizeof(blob_index));

        if (page.load(&part, start_sector) != ESP_OK) throw FixtureException("couldn't setup page");
    }
};
