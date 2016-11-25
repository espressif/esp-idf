// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <rom/spi_flash.h>
#include <rom/cache.h>
#include <soc/soc.h>
#include <soc/dport_reg.h>
#include "sdkconfig.h"
#include "esp_ipc.h"
#include "esp_attr.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "cache_utils.h"

/* bytes erased by SPIEraseBlock() ROM function */
#define BLOCK_ERASE_SIZE 32768

#if CONFIG_SPI_FLASH_ENABLE_COUNTERS
static const char* TAG = "spi_flash";
static spi_flash_counters_t s_flash_stats;

#define COUNTER_START()     uint32_t ts_begin = xthal_get_ccount()
#define COUNTER_STOP(counter)  \
    do{ \
        s_flash_stats.counter.count++; \
        s_flash_stats.counter.time += (xthal_get_ccount() - ts_begin) / (XT_CLOCK_FREQ / 1000000); \
    } while(0)

#define COUNTER_ADD_BYTES(counter, size) \
    do { \
        s_flash_stats.counter.bytes += size; \
    } while (0)

#else
#define COUNTER_START()
#define COUNTER_STOP(counter)
#define COUNTER_ADD_BYTES(counter, size)

#endif //CONFIG_SPI_FLASH_ENABLE_COUNTERS

static esp_err_t spi_flash_translate_rc(SpiFlashOpResult rc);

void spi_flash_init()
{
    spi_flash_init_lock();
#if CONFIG_SPI_FLASH_ENABLE_COUNTERS
    spi_flash_reset_counters();
#endif
}

size_t spi_flash_get_chip_size()
{
    return g_rom_flashchip.chip_size;
}

SpiFlashOpResult IRAM_ATTR spi_flash_unlock()
{
    static bool unlocked = false;
    if (!unlocked) {
        SpiFlashOpResult rc = SPIUnlock();
        if (rc != SPI_FLASH_RESULT_OK) {
            return rc;
        }
        unlocked = true;
    }
    return SPI_FLASH_RESULT_OK;
}

esp_err_t IRAM_ATTR spi_flash_erase_sector(size_t sec)
{
    return spi_flash_erase_range(sec * SPI_FLASH_SEC_SIZE, SPI_FLASH_SEC_SIZE);
}

esp_err_t IRAM_ATTR spi_flash_erase_range(uint32_t start_addr, uint32_t size)
{
    if (start_addr % SPI_FLASH_SEC_SIZE != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (size % SPI_FLASH_SEC_SIZE != 0) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (size + start_addr > spi_flash_get_chip_size()) {
        return ESP_ERR_INVALID_SIZE;
    }
    size_t start = start_addr / SPI_FLASH_SEC_SIZE;
    size_t end = start + size / SPI_FLASH_SEC_SIZE;
    const size_t sectors_per_block = BLOCK_ERASE_SIZE / SPI_FLASH_SEC_SIZE;
    COUNTER_START();
    spi_flash_disable_interrupts_caches_and_other_cpu();
    SpiFlashOpResult rc;
    rc = spi_flash_unlock();
    if (rc == SPI_FLASH_RESULT_OK) {
        for (size_t sector = start; sector != end && rc == SPI_FLASH_RESULT_OK; ) {
            if (sector % sectors_per_block == 0 && end - sector > sectors_per_block) {
                rc = SPIEraseBlock(sector / sectors_per_block);
                sector += sectors_per_block;
                COUNTER_ADD_BYTES(erase, sectors_per_block * SPI_FLASH_SEC_SIZE);
            }
            else {
                rc = SPIEraseSector(sector);
                ++sector;
                COUNTER_ADD_BYTES(erase, SPI_FLASH_SEC_SIZE);
            }
        }
    }
    spi_flash_enable_interrupts_caches_and_other_cpu();
    COUNTER_STOP(erase);
    return spi_flash_translate_rc(rc);
}

esp_err_t IRAM_ATTR spi_flash_write(size_t dest_addr, const void *src, size_t size)
{
    // Destination alignment is also checked in ROM code, but we can give
    // better error code here
    // TODO: add handling of unaligned destinations
    if (dest_addr % 4 != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (size % 4 != 0) {
        return ESP_ERR_INVALID_SIZE;
    }
    if ((uint32_t) src < 0x3ff00000) {
        // if source address is in DROM, we won't be able to read it
        // from within SPIWrite
        // TODO: consider buffering source data using heap and writing it anyway?
        return ESP_ERR_INVALID_ARG;
    }
    // Out of bound writes are checked in ROM code, but we can give better
    // error code here
    if (dest_addr + size > g_rom_flashchip.chip_size) {
        return ESP_ERR_INVALID_SIZE;
    }
    COUNTER_START();
    spi_flash_disable_interrupts_caches_and_other_cpu();
    SpiFlashOpResult rc;
    rc = spi_flash_unlock();
    if (rc == SPI_FLASH_RESULT_OK) {
        rc = SPIWrite((uint32_t) dest_addr, (const uint32_t*) src, (int32_t) size);
        COUNTER_ADD_BYTES(write, size);
    }
    spi_flash_enable_interrupts_caches_and_other_cpu();
    COUNTER_STOP(write);
    return spi_flash_translate_rc(rc);
}

esp_err_t IRAM_ATTR spi_flash_read(size_t src_addr, void *dest, size_t size)
{
    // TODO: replace this check with code which deals with unaligned destinations
    if (((ptrdiff_t) dest) % 4 != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    // Source alignment is also checked in ROM code, but we can give
    // better error code here
    // TODO: add handling of unaligned destinations
    if (src_addr % 4 != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (size % 4 != 0) {
        return ESP_ERR_INVALID_SIZE;
    }
    // Out of bound reads are checked in ROM code, but we can give better
    // error code here
    if (src_addr + size > g_rom_flashchip.chip_size) {
        return ESP_ERR_INVALID_SIZE;
    }
    COUNTER_START();
    spi_flash_disable_interrupts_caches_and_other_cpu();
    SpiFlashOpResult rc = SPIRead((uint32_t) src_addr, (uint32_t*) dest, (int32_t) size);
    COUNTER_ADD_BYTES(read, size);
    spi_flash_enable_interrupts_caches_and_other_cpu();
    COUNTER_STOP(read);
    return spi_flash_translate_rc(rc);
}

static esp_err_t spi_flash_translate_rc(SpiFlashOpResult rc)
{
    switch (rc) {
    case SPI_FLASH_RESULT_OK:
        return ESP_OK;
    case SPI_FLASH_RESULT_TIMEOUT:
        return ESP_ERR_FLASH_OP_TIMEOUT;
    case SPI_FLASH_RESULT_ERR:
    default:
        return ESP_ERR_FLASH_OP_FAIL;
    }
}

#if CONFIG_SPI_FLASH_ENABLE_COUNTERS

static inline void dump_counter(spi_flash_counter_t* counter, const char* name)
{
    ESP_LOGI(TAG, "%s  count=%8d  time=%8dms  bytes=%8d\n", name,
            counter->count, counter->time, counter->bytes);
}

const spi_flash_counters_t* spi_flash_get_counters()
{
    return &s_flash_stats;
}

void spi_flash_reset_counters()
{
    memset(&s_flash_stats, 0, sizeof(s_flash_stats));
}

void spi_flash_dump_counters()
{
    dump_counter(&s_flash_stats.read,  "read ");
    dump_counter(&s_flash_stats.write, "write");
    dump_counter(&s_flash_stats.erase, "erase");
}

#endif //CONFIG_SPI_FLASH_ENABLE_COUNTERS
