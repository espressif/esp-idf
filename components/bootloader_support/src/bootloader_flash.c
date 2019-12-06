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
#include <esp_flash_encrypt.h>
#if CONFIG_IDF_TARGET_ESP32S2BETA
#include "esp32s2beta/rom/spi_flash.h"
#endif

#ifndef BOOTLOADER_BUILD
/* Normal app version maps to esp_spi_flash.h operations...
 */
static const char *TAG = "bootloader_mmap";

static spi_flash_mmap_handle_t map;

uint32_t bootloader_mmap_get_free_pages(void)
{
    return spi_flash_mmap_get_free_pages(SPI_FLASH_MMAP_DATA);
}

const void *bootloader_mmap(uint32_t src_addr, uint32_t size)
{
    if (map) {
        ESP_LOGE(TAG, "tried to bootloader_mmap twice");
        return NULL; /* existing mapping in use... */
    }
    const void *result = NULL;
    uint32_t src_page = src_addr & ~(SPI_FLASH_MMU_PAGE_SIZE - 1);
    size += (src_addr - src_page);
    esp_err_t err = spi_flash_mmap(src_page, size, SPI_FLASH_MMAP_DATA, &result, &map);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "spi_flash_mmap failed: 0x%x", err);
        return NULL;
    }
    return (void *)((intptr_t)result + (src_addr - src_page));
}

void bootloader_munmap(const void *mapping)
{
    if (mapping && map) {
        spi_flash_munmap(map);
    }
    map = 0;
}

esp_err_t bootloader_flash_read(size_t src, void *dest, size_t size, bool allow_decrypt)
{
    if (allow_decrypt && esp_flash_encryption_enabled()) {
        return spi_flash_read_encrypted(src, dest, size);
    } else {
        return spi_flash_read(src, dest, size);
    }
}

esp_err_t bootloader_flash_write(size_t dest_addr, void *src, size_t size, bool write_encrypted)
{
    if (write_encrypted) {
#if CONFIG_IDF_TARGET_ESP32
        return spi_flash_write_encrypted(dest_addr, src, size);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
        return SPI_Encrypt_Write(dest_addr, src, size);
#endif
    } else {
        return spi_flash_write(dest_addr, src, size);
    }
}

esp_err_t bootloader_flash_erase_sector(size_t sector)
{
    return spi_flash_erase_sector(sector);
}

esp_err_t bootloader_flash_erase_range(uint32_t start_addr, uint32_t size)
{
    return spi_flash_erase_range(start_addr, size);
}

#else
/* Bootloader version, uses ROM functions only */
#include <soc/dport_reg.h>
#if CONFIG_IDF_TARGET_ESP32
#include <esp32/rom/spi_flash.h>
#include <esp32/rom/cache.h>
#elif CONFIG_IDF_TARGET_ESP32S2BETA
#include <esp32s2beta/rom/spi_flash.h>
#include <esp32s2beta/rom/cache.h>
#endif
static const char *TAG = "bootloader_flash";

#if CONFIG_IDF_TARGET_ESP32
/* Use first 50 blocks in MMU for bootloader_mmap,
   50th block for bootloader_flash_read
*/
#define MMU_BLOCK0_VADDR  SOC_DROM_LOW
#define MMU_SIZE          (0x320000)
#define MMU_BLOCK50_VADDR (MMU_BLOCK0_VADDR + MMU_SIZE)
#define FLASH_READ_VADDR MMU_BLOCK50_VADDR
#elif CONFIG_IDF_TARGET_ESP32S2BETA
/* Use first 63 blocks in MMU for bootloader_mmap,
   63th block for bootloader_flash_read
*/
#define MMU_BLOCK0_VADDR  SOC_DROM_LOW
#define MMU_SIZE          (0x3f0000)
#define MMU_BLOCK63_VADDR (MMU_BLOCK0_VADDR + MMU_SIZE)
#define FLASH_READ_VADDR MMU_BLOCK63_VADDR
#endif

#define MMU_FREE_PAGES    (MMU_SIZE / FLASH_BLOCK_SIZE)
    
static bool mapped;

// Current bootloader mapping (ab)used for bootloader_read()
static uint32_t current_read_mapping = UINT32_MAX;

uint32_t bootloader_mmap_get_free_pages(void)
{
    /**
     * Allow mapping up to 50 of the 51 available MMU blocks (last one used for reads)
     * Since, bootloader_mmap function below assumes it to be 0x320000 (50 pages), we can safely do this.
     */
    return MMU_FREE_PAGES;
}

const void *bootloader_mmap(uint32_t src_addr, uint32_t size)
{
    if (mapped) {
        ESP_LOGE(TAG, "tried to bootloader_mmap twice");
        return NULL; /* can't map twice */
    }
    if (size > MMU_SIZE) {
        ESP_LOGE(TAG, "bootloader_mmap excess size %x", size);
        return NULL;
    }

    uint32_t src_addr_aligned = src_addr & MMU_FLASH_MASK;
    uint32_t count = bootloader_cache_pages_to_map(size, src_addr);
#if CONFIG_IDF_TARGET_ESP32
    Cache_Read_Disable(0);
    Cache_Flush(0);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    uint32_t autoload = Cache_Suspend_ICache();
    Cache_Invalidate_ICache_All();
#endif
    ESP_LOGD(TAG, "mmu set paddr=%08x count=%d size=%x src_addr=%x src_addr_aligned=%x",
             src_addr & MMU_FLASH_MASK, count, size, src_addr, src_addr_aligned );
#if CONFIG_IDF_TARGET_ESP32
    int e = cache_flash_mmu_set(0, 0, MMU_BLOCK0_VADDR, src_addr_aligned, 64, count);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    int e = Cache_Ibus_MMU_Set(DPORT_MMU_ACCESS_FLASH, MMU_BLOCK0_VADDR, src_addr_aligned, 64, count, 0);
#endif
    if (e != 0) {
        ESP_LOGE(TAG, "cache_flash_mmu_set failed: %d\n", e);
#if CONFIG_IDF_TARGET_ESP32
        Cache_Read_Enable(0);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
        Cache_Resume_ICache(autoload);
#endif
        return NULL;
    }
#if CONFIG_IDF_TARGET_ESP32
    Cache_Read_Enable(0);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    Cache_Resume_ICache(autoload);
#endif

    mapped = true;

    return (void *)(MMU_BLOCK0_VADDR + (src_addr - src_addr_aligned));
}

void bootloader_munmap(const void *mapping)
{
    if (mapped)  {
#if CONFIG_IDF_TARGET_ESP32
        /* Full MMU reset */
        Cache_Read_Disable(0);
        Cache_Flush(0);
        mmu_init(0);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
        //TODO, save the autoload value.
        Cache_Suspend_ICache();
        Cache_Invalidate_ICache_All();
        Cache_MMU_Init();
#endif
        mapped = false;
        current_read_mapping = UINT32_MAX;
    }
}

static esp_err_t spi_to_esp_err(esp_rom_spiflash_result_t r)
{
    switch (r) {
    case ESP_ROM_SPIFLASH_RESULT_OK:
        return ESP_OK;
    case ESP_ROM_SPIFLASH_RESULT_ERR:
        return ESP_ERR_FLASH_OP_FAIL;
    case ESP_ROM_SPIFLASH_RESULT_TIMEOUT:
        return ESP_ERR_FLASH_OP_TIMEOUT;
    default:
        return ESP_FAIL;
    }
}

static esp_err_t bootloader_flash_read_no_decrypt(size_t src_addr, void *dest, size_t size)
{
#if CONFIG_IDF_TARGET_ESP32
    Cache_Read_Disable(0);
    Cache_Flush(0);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    uint32_t autoload = Cache_Suspend_ICache();
#endif
    esp_rom_spiflash_result_t r = esp_rom_spiflash_read(src_addr, dest, size);
#if CONFIG_IDF_TARGET_ESP32
    Cache_Read_Enable(0);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    Cache_Resume_ICache(autoload);
#endif

    return spi_to_esp_err(r);
}

static esp_err_t bootloader_flash_read_allow_decrypt(size_t src_addr, void *dest, size_t size)
{
    uint32_t *dest_words = (uint32_t *)dest;

    for (int word = 0; word < size / 4; word++) {
        uint32_t word_src = src_addr + word * 4;  /* Read this offset from flash */
        uint32_t map_at = word_src & MMU_FLASH_MASK; /* Map this 64KB block from flash */
        uint32_t *map_ptr;
        if (map_at != current_read_mapping) {
            /* Move the 64KB mmu mapping window to fit map_at */
#if CONFIG_IDF_TARGET_ESP32
            Cache_Read_Disable(0);
            Cache_Flush(0);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
            uint32_t autoload = Cache_Suspend_ICache();
            Cache_Invalidate_ICache_All();
#endif
            ESP_LOGD(TAG, "mmu set block paddr=0x%08x (was 0x%08x)", map_at, current_read_mapping);
#if CONFIG_IDF_TARGET_ESP32
            int e = cache_flash_mmu_set(0, 0, FLASH_READ_VADDR, map_at, 64, 1);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
            int e = Cache_Ibus_MMU_Set(DPORT_MMU_ACCESS_FLASH, FLASH_READ_VADDR, map_at, 64, 1, 0);
#endif
            if (e != 0) {
                ESP_LOGE(TAG, "cache_flash_mmu_set failed: %d\n", e);
#if CONFIG_IDF_TARGET_ESP32
                Cache_Read_Enable(0);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
                Cache_Resume_ICache(autoload);
#endif
                return ESP_FAIL;
            }
            current_read_mapping = map_at;
#if CONFIG_IDF_TARGET_ESP32
            Cache_Read_Enable(0);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
            Cache_Resume_ICache(autoload);
#endif
        }
        map_ptr = (uint32_t *)(FLASH_READ_VADDR + (word_src - map_at));
        dest_words[word] = *map_ptr;
    }
    return ESP_OK;
}

esp_err_t bootloader_flash_read(size_t src_addr, void *dest, size_t size, bool allow_decrypt)
{
    if (src_addr & 3) {
        ESP_LOGE(TAG, "bootloader_flash_read src_addr 0x%x not 4-byte aligned", src_addr);
        return ESP_FAIL;
    }
    if (size & 3) {
        ESP_LOGE(TAG, "bootloader_flash_read size 0x%x not 4-byte aligned", size);
        return ESP_FAIL;
    }
    if ((intptr_t)dest & 3) {
        ESP_LOGE(TAG, "bootloader_flash_read dest 0x%x not 4-byte aligned", (intptr_t)dest);
        return ESP_FAIL;
    }

    if (allow_decrypt) {
        return bootloader_flash_read_allow_decrypt(src_addr, dest, size);
    } else {
        return bootloader_flash_read_no_decrypt(src_addr, dest, size);
    }
}

esp_err_t bootloader_flash_write(size_t dest_addr, void *src, size_t size, bool write_encrypted)
{
    esp_err_t err;
    size_t alignment = write_encrypted ? 32 : 4;
    if ((dest_addr % alignment) != 0) {
        ESP_LOGE(TAG, "bootloader_flash_write dest_addr 0x%x not %d-byte aligned", dest_addr, alignment);
        return ESP_FAIL;
    }
    if ((size % alignment) != 0) {
        ESP_LOGE(TAG, "bootloader_flash_write size 0x%x not %d-byte aligned", size, alignment);
        return ESP_FAIL;
    }
    if (((intptr_t)src % 4) != 0) {
        ESP_LOGE(TAG, "bootloader_flash_write src 0x%x not 4 byte aligned", (intptr_t)src);
        return ESP_FAIL;
    }

    err = spi_to_esp_err(esp_rom_spiflash_unlock());
    if (err != ESP_OK) {
        return err;
    }

    if (write_encrypted) {
#if CONFIG_IDF_TARGET_ESP32
        return spi_to_esp_err(esp_rom_spiflash_write_encrypted(dest_addr, src, size));
#elif CONFIG_IDF_TARGET_ESP32S2BETA
        // TODO: use the same ROM AP here
        return spi_to_esp_err(SPI_Encrypt_Write(dest_addr, src, size));
#endif
    } else {
        return spi_to_esp_err(esp_rom_spiflash_write(dest_addr, src, size));
    }
}

esp_err_t bootloader_flash_erase_sector(size_t sector)
{
    return spi_to_esp_err(esp_rom_spiflash_erase_sector(sector));
}

esp_err_t bootloader_flash_erase_range(uint32_t start_addr, uint32_t size)
{
    if (start_addr % FLASH_SECTOR_SIZE != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (size % FLASH_SECTOR_SIZE != 0) {
        return ESP_ERR_INVALID_SIZE;
    }
    size_t start = start_addr / FLASH_SECTOR_SIZE;
    size_t end = start + size / FLASH_SECTOR_SIZE;
    const size_t sectors_per_block = FLASH_BLOCK_SIZE / FLASH_SECTOR_SIZE;

    esp_rom_spiflash_result_t rc = ESP_ROM_SPIFLASH_RESULT_OK;
    for (size_t sector = start; sector != end && rc == ESP_ROM_SPIFLASH_RESULT_OK; ) {
        if (sector % sectors_per_block == 0 && end - sector >= sectors_per_block) {
            rc = esp_rom_spiflash_erase_block(sector / sectors_per_block);
            sector += sectors_per_block;
        } else {
            rc = esp_rom_spiflash_erase_sector(sector);
            ++sector;
        }
    }
    return spi_to_esp_err(rc);
}
#endif
