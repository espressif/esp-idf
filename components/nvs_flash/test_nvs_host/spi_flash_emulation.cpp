/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_partition.h"
#include "spi_flash_emulation.h"

static SpiFlashEmulator* s_emulator = nullptr;

void spi_flash_emulator_set(SpiFlashEmulator* e)
{
    s_emulator = e;
}

esp_err_t esp_partition_erase_range(const esp_partition_t* partition,
                                    size_t offset, size_t size)
{
    if (!s_emulator) {
        return ESP_ERR_FLASH_OP_TIMEOUT;
    }

    if (size % SPI_FLASH_SEC_SIZE != 0) {
        return ESP_ERR_INVALID_SIZE;
    }

    if (offset % SPI_FLASH_SEC_SIZE != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    size_t start_sector = offset / SPI_FLASH_SEC_SIZE;
    size_t num_sectors = size / SPI_FLASH_SEC_SIZE;
    for (size_t sector = start_sector; sector < (start_sector + num_sectors); sector++) {
        if (!s_emulator->erase(sector)) {
            return ESP_ERR_FLASH_OP_FAIL;
        }
    }

    return ESP_OK;
}

esp_err_t esp_partition_read(const esp_partition_t* partition,
                             size_t src_offset, void* dst, size_t size)
{
    if (!s_emulator) {
        return ESP_ERR_FLASH_OP_TIMEOUT;
    }

    if (!s_emulator->read(reinterpret_cast<uint32_t*>(dst), src_offset, size)) {
        return ESP_ERR_FLASH_OP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_partition_read_raw(const esp_partition_t* partition,
                                 size_t src_offset, void* dst, size_t size)
{
    if (!s_emulator) {
        return ESP_ERR_FLASH_OP_TIMEOUT;
    }

    if (!s_emulator->read(reinterpret_cast<uint32_t*>(dst), src_offset, size)) {
        return ESP_ERR_FLASH_OP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_partition_write(const esp_partition_t* partition,
                             size_t dst_offset, const void* src, size_t size)
{
    if (!s_emulator) {
        return ESP_ERR_FLASH_OP_TIMEOUT;
    }

    if (!s_emulator->write(dst_offset, reinterpret_cast<const uint32_t*>(src), size)) {
        return ESP_ERR_FLASH_OP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_partition_write_raw(const esp_partition_t* partition,
                                  size_t dst_offset, const void* src, size_t size)
{
    if (!s_emulator) {
        return ESP_ERR_FLASH_OP_TIMEOUT;
    }

    if (!s_emulator->write(dst_offset, reinterpret_cast<const uint32_t*>(src), size)) {
        return ESP_ERR_FLASH_OP_FAIL;
    }

    return ESP_OK;
}

// timing data for ESP8266, 160MHz CPU frequency, 80MHz flash requency
// all values in microseconds
// values are for block sizes starting at 4 bytes and going up to 4096 bytes
static size_t readTimes[] = {7, 5, 6, 7, 11, 18, 32, 60, 118, 231, 459};
static size_t writeTimes[] = {19, 23, 35, 57, 106, 205, 417, 814, 1622, 3200, 6367};
static size_t blockEraseTime = 37142;


static size_t timeInterp(uint32_t bytes, size_t* lut)
{
    const int lut_size = sizeof(readTimes)/sizeof(readTimes[0]);
    int lz = __builtin_clz(bytes / 4);
    int log_size = 32 - lz;
    size_t x2 = 1 << (log_size + 2);
    size_t y2 = lut[std::min(log_size, lut_size - 1)];
    size_t x1 = 1 << (log_size + 1);
    size_t y1 = lut[log_size - 1];
    return (bytes - x1) * (y2 - y1) / (x2 - x1) + y1;
}

size_t SpiFlashEmulator::getReadOpTime(uint32_t bytes)
{
    return timeInterp(bytes, readTimes);
}

size_t SpiFlashEmulator::getWriteOpTime(uint32_t bytes)
{
    return timeInterp(bytes, writeTimes);
}

size_t SpiFlashEmulator::getEraseOpTime()
{
    return blockEraseTime;
}
