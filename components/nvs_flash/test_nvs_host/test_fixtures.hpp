/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "nvs_partition.hpp"
#include "nvs_encrypted_partition.hpp"
#include "spi_flash_emulation.h"
#include "nvs.h"

class PartitionEmulation : public nvs::Partition {
public:
    PartitionEmulation(SpiFlashEmulator *spi_flash_emulator,
            uint32_t address,
            uint32_t size,
            const char *partition_name = NVS_DEFAULT_PART_NAME)
        : partition_name(partition_name), flash_emu(spi_flash_emulator), address(address), size(size)
    {
        assert(partition_name);
        assert(flash_emu);
        assert(size);
        readonly = false;
    }

    const char *get_partition_name() override
    {
        return partition_name;
    }

    esp_err_t read_raw(size_t src_offset, void* dst, size_t size) override
    {
        if (!flash_emu->read(reinterpret_cast<uint32_t*>(dst), src_offset, size)) {
            return ESP_ERR_FLASH_OP_FAIL;
        }

        return ESP_OK;
    }

    esp_err_t read(size_t src_offset, void* dst, size_t size) override
    {
        if (!flash_emu->read(reinterpret_cast<uint32_t*>(dst), src_offset, size)) {
            return ESP_ERR_FLASH_OP_FAIL;
        }

        return ESP_OK;
    }

    esp_err_t write_raw(size_t dst_offset, const void* src, size_t size) override
    {
        if (!flash_emu->write(dst_offset, reinterpret_cast<const uint32_t*>(src), size)) {
            return ESP_ERR_FLASH_OP_FAIL;
        }

        return ESP_OK;
    }

    esp_err_t write(size_t dst_offset, const void* src, size_t size) override
    {
        if (!flash_emu->write(dst_offset, reinterpret_cast<const uint32_t*>(src), size)) {
            return ESP_ERR_FLASH_OP_FAIL;
        }

        return ESP_OK;
    }

    esp_err_t erase_range(size_t dst_offset, size_t size) override
    {
        if (size % SPI_FLASH_SEC_SIZE != 0) {
            return ESP_ERR_INVALID_SIZE;
        }

        if (dst_offset % SPI_FLASH_SEC_SIZE != 0) {
            return ESP_ERR_INVALID_ARG;
        }

        size_t start_sector = dst_offset / SPI_FLASH_SEC_SIZE;
        size_t num_sectors = size / SPI_FLASH_SEC_SIZE;
        for (size_t sector = start_sector; sector < (start_sector + num_sectors); sector++) {
            if (!flash_emu->erase(sector)) {
                return ESP_ERR_FLASH_OP_FAIL;
            }
        }

        return ESP_OK;
    }

    uint32_t get_address() override
    {
        return address;
    }

    uint32_t get_size() override
    {
        return size;
    }

    bool get_readonly() override
    {
        return readonly;
    }

private:
    const char *partition_name;

    SpiFlashEmulator *flash_emu;

    uint32_t address;

    uint32_t size;

    bool readonly;
};

struct PartitionEmulationFixture {
    PartitionEmulationFixture(uint32_t start_sector = 0,
            uint32_t sector_size = 1,
            const char *partition_name = NVS_DEFAULT_PART_NAME)
        : emu(start_sector + sector_size),
          part(&emu, start_sector * SPI_FLASH_SEC_SIZE, sector_size * SPI_FLASH_SEC_SIZE, partition_name) {
    }

    ~PartitionEmulationFixture() { }

    SpiFlashEmulator emu;

    PartitionEmulation part;
};

struct EncryptedPartitionFixture {
    EncryptedPartitionFixture(nvs_sec_cfg_t *cfg,
            uint32_t start_sector = 0,
            uint32_t sector_size = 1,
            const char *partition_name = NVS_DEFAULT_PART_NAME)
        : esp_partition(), emu(start_sector + sector_size),
          part(&esp_partition) {
        esp_partition.address = start_sector * SPI_FLASH_SEC_SIZE;
        esp_partition.size = sector_size * SPI_FLASH_SEC_SIZE;
        strncpy(esp_partition.label, partition_name, PART_NAME_MAX_SIZE);
        assert(part.init(cfg) == ESP_OK);
    }

    ~EncryptedPartitionFixture() { }

    esp_partition_t esp_partition;

    SpiFlashEmulator emu;

    nvs::NVSEncryptedPartition part;
};
