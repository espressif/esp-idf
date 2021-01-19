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
#include "esp_spi_flash.h"
#include "esp_flash.h"
#include "spi_flash_emulation.h"

static SpiFlashEmulator* s_emulator = nullptr;

static esp_flash_t esp_flash_default_chip_instance;

esp_flash_t *esp_flash_default_chip = &esp_flash_default_chip_instance;

void spi_flash_emulator_set(SpiFlashEmulator* e)
{
    s_emulator = e;
}

esp_err_t spi_flash_erase_sector(size_t sec)
{
    if (!s_emulator) {
        return ESP_ERR_FLASH_OP_TIMEOUT;
    }

    if (!s_emulator->erase(sec)) {
        return ESP_ERR_FLASH_OP_FAIL;
    }

    return ESP_OK;
}

esp_err_t spi_flash_write(size_t des_addr, const void *src_addr, size_t size)
{
    if (!s_emulator) {
        return ESP_ERR_FLASH_OP_TIMEOUT;
    }

    if (!s_emulator->write(des_addr, reinterpret_cast<const uint32_t*>(src_addr), size)) {
        return ESP_ERR_FLASH_OP_FAIL;
    }

    return ESP_OK;
}

esp_err_t spi_flash_read(size_t src_addr, void *des_addr, size_t size)
{
    if (!s_emulator) {
        return ESP_ERR_FLASH_OP_TIMEOUT;
    }

    if (!s_emulator->read(reinterpret_cast<uint32_t*>(des_addr), src_addr, size)) {
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

