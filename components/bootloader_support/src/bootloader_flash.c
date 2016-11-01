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
#include <stddef.h>

#include <bootloader_flash.h>
#include <esp_log.h>
#include <esp_spi_flash.h> /* including in bootloader for error values */

#ifndef BOOTLOADER_BUILD
/* Normal app version maps to esp_spi_flash.h operations...
 */
static const char *TAG = "bootloader_mmap";

static spi_flash_mmap_memory_t map;

const void *bootloader_mmap(uint32_t src_addr, uint32_t size)
{
    if (map) {
        ESP_LOGE(TAG, "tried to bootloader_mmap twice");
        return NULL; /* existing mapping in use... */
    }
    const void *result = NULL;
    esp_err_t err = spi_flash_mmap(src_addr, size, SPI_FLASH_MMAP_DATA, &result, &map);
    if (err != ESP_OK) {
        result = NULL;
    }
    return result;
}

void bootloader_unmap(const void *mapping)
{
    if(mapping && map) {
        spi_flash_munmap(map);
    }
    map = 0;
}

esp_err_t bootloader_flash_read(size_t src, void *dest, size_t size)
{
    return spi_flash_read(src, dest, size);
}

#else
/* Bootloader version, uses ROM functions only */
#include <rom/spi_flash.h>
#include <rom/cache.h>

static const char *TAG = "bootloader_flash";

static bool mapped;

const void *bootloader_mmap(uint32_t src_addr, uint32_t size)
{
    if (mapped) {
        ESP_LOGE(TAG, "tried to bootloader_mmap twice");
        return NULL; /* can't map twice */
    }

    uint32_t src_addr_aligned = src_addr & 0xffff0000;
    uint32_t count = (size + 0xffff) / 0x10000;
    Cache_Read_Disable(0);
    Cache_Flush(0);
    ESP_LOGD(TAG, "mmu set paddr=%08x count=%d", src_addr_aligned, count );
    cache_flash_mmu_set( 0, 0, 0x3f400000, src_addr_aligned, 64, count );
    Cache_Read_Enable( 0 );

    mapped = true;

    return (void *)(0x3f400000 + (src_addr - src_addr_aligned));
}

void bootloader_unmap(const void *mapping)
{
    if (mapped)  {
        /* Full MMU reset */
        Cache_Read_Disable(0);
        Cache_Flush(0);
        mmu_init(0);
        mapped = false;
    }
}

esp_err_t bootloader_flash_read(size_t src_addr, void *dest, size_t size)
{
    if(src_addr & 3) {
        ESP_LOGE(TAG, "bootloader_flash_read src_addr not 4-byte aligned");
        return ESP_FAIL;
    }
    if((intptr_t)dest & 3) {
        ESP_LOGE(TAG, "bootloader_flash_read dest not 4-byte aligned");
        return ESP_FAIL;
    }

    Cache_Read_Disable(0);
    Cache_Flush(0);
    SpiFlashOpResult r = SPIRead(src_addr, dest, size);
    Cache_Read_Enable(0);

    switch(r) {
    case SPI_FLASH_RESULT_OK:
        return ESP_OK;
    case SPI_FLASH_RESULT_ERR:
        return ESP_ERR_FLASH_OP_FAIL;
    case SPI_FLASH_RESULT_TIMEOUT:
        return ESP_ERR_FLASH_OP_TIMEOUT;
    default:
        return ESP_FAIL;
    }
}

#endif
