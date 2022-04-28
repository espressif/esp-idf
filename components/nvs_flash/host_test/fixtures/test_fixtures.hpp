// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "nvs_partition.hpp"
#include "nvs.h"
#include "nvs_page.hpp"
#include "nvs_storage.hpp"
#include <exception>
#include <string>

#ifdef CONFIG_NVS_ENCRYPTION
#include "nvs_encrypted_partition.hpp"
#endif

extern "C" {
#include "Mockesp_partition.h"
}

struct FixtureException : std::exception {
    FixtureException(const std::string& msg) : msg(msg) { }

    const char *what() {
        return msg.c_str();
    }

    std::string msg;
};

class PartitionMock : public nvs::Partition {
public:
    PartitionMock(uint32_t address, uint32_t size)
        : partition(), address(address), size(size)
    {
        assert(size);
    }

    const char *get_partition_name() override
    {
        return "";
    }

    esp_err_t read_raw(size_t src_offset, void* dst, size_t size) override
    {
        return esp_partition_read_raw(&partition, src_offset, dst, size);
    }

    esp_err_t read(size_t src_offset, void* dst, size_t size) override
    {
        return esp_partition_read(&partition, src_offset, dst, size);
    }

    esp_err_t write_raw(size_t dst_offset, const void* src, size_t size) override
    {
        return esp_partition_write_raw(&partition, dst_offset, src, size);
    }

    esp_err_t write(size_t dst_offset, const void* src, size_t size) override
    {
        return esp_partition_write(&partition, dst_offset, src, size);
    }

    esp_err_t erase_range(size_t dst_offset, size_t size) override
    {
        return esp_partition_erase_range(&partition, dst_offset, size);
    }

    uint32_t get_address() override
    {
        return address;
    }

    uint32_t get_size() override
    {
        return size;
    }

    const esp_partition_t partition;

private:
    uint32_t address;

    uint32_t size;
};

#ifdef CONFIG_NVS_ENCRYPTION
struct EncryptedPartitionFixture {
    EncryptedPartitionFixture(nvs_sec_cfg_t *cfg,
            uint32_t start_sector = 0,
            uint32_t sector_size = 1,
            const char *partition_name = NVS_DEFAULT_PART_NAME)
        : esp_partition(), emu(start_sector + sector_size),
          part(partition_name, &esp_partition) {
        esp_partition.address = start_sector * SPI_FLASH_SEC_SIZE;
        esp_partition.size = sector_size * SPI_FLASH_SEC_SIZE;
        assert(part.init(cfg) == ESP_OK);
    }

    ~EncryptedPartitionFixture() { }

    esp_partition_t esp_partition;

    SpiFlashEmulator emu;

    nvs::NVSEncryptedPartition part;
};
#endif

struct PartitionMockFixture {
    PartitionMockFixture(uint32_t start_sector = 0,
            uint32_t sector_size = 1,
            const char *partition_name = NVS_DEFAULT_PART_NAME)
        : part_mock(start_sector * SPI_FLASH_SEC_SIZE, sector_size * SPI_FLASH_SEC_SIZE) {
        std::fill_n(raw_header, sizeof(raw_header)/sizeof(raw_header[0]), UINT8_MAX);

        // This resets the mocks and prevents meeting accidental expectations from previous tests.
        Mockesp_partition_Init();
    }

    ~PartitionMockFixture() { }

    uint8_t raw_header[512];

    PartitionMock part_mock;
};

struct NVSPageFixture : public PartitionMockFixture {
    NVSPageFixture(uint32_t start_sector = 0,
            uint32_t sector_size = 1,
            const char *partition_name = NVS_DEFAULT_PART_NAME)
        : PartitionMockFixture(start_sector, sector_size, partition_name), page()
    {
        esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_raw_ReturnArrayThruPtr_dst(raw_header, 32);

        for (int i = 0; i < 8; i++) {
            esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
            esp_partition_read_raw_ReturnArrayThruPtr_dst(raw_header, 512);
        }

        if (page.load(&part_mock, start_sector) != ESP_OK) throw FixtureException("couldn't setup page");
    }

    nvs::Page page;
};

struct NVSValidPageFlashFixture : public PartitionMockFixture {
    const static uint8_t NS_INDEX = 1;

    // valid header
    uint8_t raw_header_valid [32];

    // entry table with one entry
    uint8_t raw_entry_table [32];

    uint8_t ns_entry [32];

    uint8_t value_entry [32];

    NVSValidPageFlashFixture(uint32_t start_sector = 0,
            uint32_t sector_size = 1,
            const char *partition_name = NVS_DEFAULT_PART_NAME)
        : PartitionMockFixture(start_sector, sector_size, partition_name),
        raw_header_valid {0xfe, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc2, 0x16, 0xdd, 0xdc},
        ns_entry {0x00, 0x01, 0x01, 0xff, 0x68, 0xc5, 0x3f, 0x0b, 't', 'e', 's', 't', '_', 'n', 's', '\0',
                '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        value_entry {0x01, 0x01, 0x01, 0xff, 0x3d, 0xf3, 0x99, 0xe5, 't', 'e', 's', 't', '_', 'v', 'a', 'l',
                'u', 'e', '\0', '\0', '\0', '\0', '\0', '\0', 47, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}
    {
        std::fill_n(raw_entry_table, sizeof(raw_entry_table)/sizeof(raw_entry_table[0]), 0);
        raw_entry_table[0] = 0xfa;

        // read page header
        esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_raw_ReturnArrayThruPtr_dst(raw_header_valid, 32);

        // read entry table
        esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_raw_ReturnArrayThruPtr_dst(raw_entry_table, 32);

        // read next free entry's header
        esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_raw_ReturnArrayThruPtr_dst(raw_header, 4);

        // read namespace entry
        esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_ReturnArrayThruPtr_dst(ns_entry, 32);

        // read normal entry
        esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_ReturnArrayThruPtr_dst(value_entry, 32);

        // read normal entry second time during duplicated entry check
        esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_ReturnArrayThruPtr_dst(value_entry, 32);
    }
};

struct NVSValidPageFixture : public NVSValidPageFlashFixture {
    NVSValidPageFixture(uint32_t start_sector = 0,
            uint32_t sector_size = 1,
            const char *partition_name = NVS_DEFAULT_PART_NAME)
        : NVSValidPageFlashFixture(start_sector, sector_size, partition_name), page()
    {
        if (page.load(&part_mock, start_sector) != ESP_OK) throw FixtureException("couldn't setup page");
    }

    nvs::Page page;
};

struct NVSValidStorageFixture : public PartitionMockFixture {
    const static uint8_t NS_INDEX = 1;

    uint8_t ns_entry [32];

    uint8_t empty_entry [32];

    NVSValidStorageFixture(uint32_t start_sector = 0,
            uint32_t sector_size = 3,
            const char *partition_name = NVS_DEFAULT_PART_NAME)
        : PartitionMockFixture(start_sector, sector_size, partition_name),
        ns_entry {0x00, 0x01, 0x01, 0xff, 0x68, 0xc5, 0x3f, 0x0b, 't', 'e', 's', 't', '_', 'n', 's', '\0',
                '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        empty_entry(),
        storage(&part_mock)
    {
        std::fill_n(empty_entry, sizeof(empty_entry)/sizeof(empty_entry[0]), 0xFF);

        // entry table with one entry
        uint8_t raw_entry_table [32];

        uint8_t header_full_page [] = {
                0xfe, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc2, 0x16, 0xdd, 0xdc};

        uint8_t header_second_page [] = {
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

        uint8_t header_third_page [] = {
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

        // entry_table with all elements deleted except the namespace entry written and the last entry free
        std::fill_n(raw_entry_table, sizeof(raw_entry_table)/sizeof(raw_entry_table[0]), 0);
        raw_entry_table[0] = 0x02;
        raw_entry_table[31] = 0xFC;

        // read full page header
        esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_raw_ReturnArrayThruPtr_dst(header_full_page, 32);

        // read entry table
        esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_raw_ReturnArrayThruPtr_dst(raw_entry_table, 32);

        // reading entry table checks empty entry
        esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_raw_ReturnArrayThruPtr_dst(empty_entry, 32);

        // read namespace entry
        esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_ReturnArrayThruPtr_dst(ns_entry, 32);

        // read last two pages' headers, which trigger an automatic full read each because each page is empty
        esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_raw_ReturnArrayThruPtr_dst(header_second_page, 32);
        for (int i = 0; i < 8; i++) {
            esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
            esp_partition_read_raw_ReturnArrayThruPtr_dst(raw_header, 512);
        }
        esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_raw_ReturnArrayThruPtr_dst(header_third_page, 32);
        for (int i = 0; i < 8; i++) {
            esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
            esp_partition_read_raw_ReturnArrayThruPtr_dst(raw_header, 512);
        }

        // read namespace entry in duplicated header item check of pagemanager::load
        esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_ReturnArrayThruPtr_dst(ns_entry, 32);

        // storage finally actually reads namespace
        esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_ReturnArrayThruPtr_dst(ns_entry, 32);

        // storage looks for blob index entries
        esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_ReturnArrayThruPtr_dst(ns_entry, 32);

        // Storage::eraseOrphanDataBlobs() also wants to take it's turn...
        esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_ReturnArrayThruPtr_dst(ns_entry, 32);

        if (storage.init(start_sector, sector_size) != ESP_OK) throw FixtureException("couldn't setup page");
    }

    nvs::Storage storage;
};

struct NVSValidBlobPageFixture : public PartitionMockFixture {
    const static uint8_t NS_INDEX = 1;
    const static size_t BLOB_DATA_SIZE = 32;

    // valid header
    uint8_t raw_header_valid [32];

    // entry table with one entry
    uint8_t raw_entry_table [32];

    uint8_t ns_entry [32];

    uint8_t blob_entry [32];
    uint8_t blob_data [BLOB_DATA_SIZE];
    uint8_t blob_index [32];

    NVSValidBlobPageFixture(uint32_t start_sector = 0,
            uint32_t sector_size = 1,
            const char *partition_name = NVS_DEFAULT_PART_NAME)
        : PartitionMockFixture(start_sector, sector_size, partition_name),
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
        std::fill_n(raw_entry_table, sizeof(raw_entry_table)/sizeof(raw_entry_table[0]), 0xFF);
        raw_entry_table[0] = 0xaa;

        // read page header
        esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_raw_ReturnArrayThruPtr_dst(raw_header_valid, 32);

        // read entry table
        esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_raw_ReturnArrayThruPtr_dst(raw_entry_table, 32);

        // read next free entry's header
        esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_raw_ReturnArrayThruPtr_dst(raw_header, 4);

        // read namespace entry
        esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_ReturnArrayThruPtr_dst(ns_entry, 32);

        // read normal blob entry + index, not the data
        esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_ReturnArrayThruPtr_dst(blob_entry, 32);
        esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_ReturnArrayThruPtr_dst(blob_index, 32);

        // read normal entry second time during duplicated entry check
        esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_ReturnArrayThruPtr_dst(blob_entry, 32);

        if (page.load(&part_mock, start_sector) != ESP_OK) throw FixtureException("couldn't setup page");
    }

    nvs::Page page;
};

struct NVSFullPageFixture : public PartitionMockFixture {
    const static uint8_t NS_INDEX = 1;

    // valid header
    uint8_t raw_header_valid [32];

    // entry table with one entry
    uint8_t raw_entry_table [32];

    uint8_t ns_entry [32];

    uint8_t value_entry [32];

    NVSFullPageFixture(uint32_t start_sector = 0,
            uint32_t sector_size = 1,
            const char *partition_name = NVS_DEFAULT_PART_NAME,
            bool load = true)
        : PartitionMockFixture(start_sector, sector_size, partition_name),
        raw_header_valid {0xfc, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xa3, 0x48, 0x9f, 0x38},
        ns_entry {0x00, 0x01, 0x01, 0xff, 0x68, 0xc5, 0x3f, 0x0b, 't', 'e', 's', 't', '_', 'n', 's', '\0',
                '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        value_entry {0x01, 0x01, 0x01, 0xff, 0x3d, 0xf3, 0x99, 0xe5, 't', 'e', 's', 't', '_', 'v', 'a', 'l',
                'u', 'e', '\0', '\0', '\0', '\0', '\0', '\0', 47, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        page()
    {
        // entry_table with all elements deleted except the namespace entry written and the last entry free
        std::fill_n(raw_entry_table, sizeof(raw_entry_table)/sizeof(raw_entry_table[0]), 0);
        raw_entry_table[0] = 0x0a;
        raw_entry_table[31] = 0xFC;

        // read page header
        esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_raw_ReturnArrayThruPtr_dst(raw_header_valid, 32);

        // read entry table
        esp_partition_read_raw_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_raw_ReturnArrayThruPtr_dst(raw_entry_table, 32);

        // no next free entry check, only one entry written

        // read namespace entry
        esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_ReturnArrayThruPtr_dst(ns_entry, 32);

        // read normal entry
        esp_partition_read_ExpectAnyArgsAndReturn(ESP_OK);
        esp_partition_read_ReturnArrayThruPtr_dst(value_entry, 32);

        // no duplicated entry check

        if (load) {
            if (page.load(&part_mock, start_sector) != ESP_OK) throw FixtureException("couldn't setup page");
        }
    }

    nvs::Page page;
};
