/*
Abstraction layer for spi-ram. For now, it's no more than a stub for the spiram_psram functions, but if
we add more types of external RAM memory, this can be made into a more intelligent dispatcher.
*/

/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp32s3/spiram.h"
#include "spiram_psram.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "soc/soc.h"
#include "esp_heap_caps_init.h"
#include "soc/soc_memory_layout.h"
#include "soc/dport_reg.h"
#include "esp32s3/rom/cache.h"
#include "soc/ext_mem_defs.h"
#include "soc/extmem_reg.h"

/**
 * @note consider abstract these cache register operations, so as to make `spiram.c` not needed to be IRAM-SAFE.
 * This file only contains abstract operations.
 */

#define PSRAM_MODE PSRAM_VADDR_MODE_NORMAL

#define MMU_PAGE_SIZE    0x10000

#if CONFIG_SPIRAM

static const char *TAG = "spiram";

#if CONFIG_SPIRAM_SPEED_40M
#define PSRAM_SPEED PSRAM_CACHE_S40M
#else  //#if CONFIG_SPIRAM_SPEED_80M
#define PSRAM_SPEED PSRAM_CACHE_S80M
#endif

static bool s_spiram_inited = false;

//These variables are in bytes
static uint32_t s_allocable_vaddr_start;
static uint32_t s_allocable_vaddr_end;
static DRAM_ATTR uint32_t s_mapped_vaddr_start;
static DRAM_ATTR uint32_t s_mapped_size;

/**
 * Initially map all psram physical address to virtual address.
 * If psram physical size is larger than virtual address range, then only map the virtual address range.
 */
void IRAM_ATTR esp_spiram_init_cache(void)
{
    esp_err_t ret = psram_get_available_size(&s_mapped_size);
    if (ret != ESP_OK) {
        abort();
    }
    if ((SOC_EXTRAM_DATA_HIGH - SOC_EXTRAM_DATA_LOW) < s_mapped_size) {
        //Decide these logics when there's a real PSRAM with larger size
        ESP_EARLY_LOGE(TAG, "Virtual address not enough for PSRAM!");
        abort();
    }
    s_mapped_vaddr_start = SOC_EXTRAM_DATA_HIGH - s_mapped_size;

    Cache_Suspend_DCache();
    Cache_Dbus_MMU_Set(MMU_ACCESS_SPIRAM, s_mapped_vaddr_start, 0, 64, s_mapped_size >> 16, 0);
    REG_CLR_BIT(EXTMEM_DCACHE_CTRL1_REG, EXTMEM_DCACHE_SHUT_CORE0_BUS);
#if !CONFIG_FREERTOS_UNICORE
    REG_CLR_BIT(EXTMEM_DCACHE_CTRL1_REG, EXTMEM_DCACHE_SHUT_CORE1_BUS);
#endif
    Cache_Resume_DCache(0);

    //Currently no non-heap stuff on ESP32S3
    s_allocable_vaddr_start = s_mapped_vaddr_start;
    s_allocable_vaddr_end = SOC_EXTRAM_DATA_HIGH;
}

/*
 Simple RAM test. Writes a word every 32 bytes. Takes about a second to complete for 4MiB. Returns
 true when RAM seems OK, false when test fails. WARNING: Do not run this before the 2nd cpu has been
 initialized (in a two-core system) or after the heap allocator has taken ownership of the memory.
*/
bool esp_spiram_test(void)
{
    volatile int *spiram = (volatile int *)s_mapped_vaddr_start;

    size_t s = s_mapped_size;
    size_t p;
    int errct = 0;
    int initial_err = -1;

    for (p = 0; p < (s / sizeof(int)); p += 8) {
        spiram[p] = p ^ 0xAAAAAAAA;
    }
    for (p = 0; p < (s / sizeof(int)); p += 8) {
        if (spiram[p] != (p ^ 0xAAAAAAAA)) {
            errct++;
            if (errct == 1) {
                initial_err = p * 4;
            }
            if (errct < 4) {
                ESP_EARLY_LOGE(TAG, "SPI SRAM error@%08x:%08x/%08x \n", &spiram[p], spiram[p], p ^ 0xAAAAAAAA);
            }
        }
    }
    if (errct) {
        ESP_EARLY_LOGE(TAG, "SPI SRAM memory test fail. %d/%d writes failed, first @ %X\n", errct, s / 32, initial_err + SOC_EXTRAM_DATA_LOW);
        return false;
    } else {
        ESP_EARLY_LOGI(TAG, "SPI SRAM memory test OK");
        return true;
    }
}

//TODO IDF-4318
// static uint32_t pages_for_flash = 0;
static uint32_t instruction_in_spiram = 0;
static uint32_t rodata_in_spiram = 0;

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
static int instr_flash2spiram_offs = 0;
static uint32_t instr_start_page = 0;
static uint32_t instr_end_page = 0;
#endif

#if CONFIG_SPIRAM_RODATA
static int rodata_flash2spiram_offs = 0;
static uint32_t rodata_start_page = 0;
static uint32_t rodata_end_page = 0;
#endif

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA
static uint32_t page0_mapped = 0;
static uint32_t page0_page = INVALID_PHY_PAGE;
#endif

uint32_t esp_spiram_instruction_access_enabled(void)
{
    return instruction_in_spiram;
}

uint32_t esp_spiram_rodata_access_enabled(void)
{
    return rodata_in_spiram;
}

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
esp_err_t esp_spiram_enable_instruction_access(void)
{
    //TODO IDF-4318, `pages_for_flash` will be overwritten, however it influences the psram size to be added to the heap allocator.
    abort();
}
#endif

#if CONFIG_SPIRAM_RODATA
esp_err_t esp_spiram_enable_rodata_access(void)
{
    //TODO IDF-4318, `pages_for_flash` will be overwritten, however it influences the psram size to be added to the heap allocator.
    abort();
}
#endif

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
void instruction_flash_page_info_init(void)
{
    uint32_t instr_page_cnt = ((uint32_t)&_instruction_reserved_end - SOC_IROM_LOW + MMU_PAGE_SIZE - 1) / MMU_PAGE_SIZE;

    instr_start_page = *(volatile uint32_t *)(DR_REG_MMU_TABLE + CACHE_IROM_MMU_START);
    instr_start_page &= MMU_VALID_VAL_MASK;
    instr_end_page = instr_start_page + instr_page_cnt - 1;
}

uint32_t IRAM_ATTR instruction_flash_start_page_get(void)
{
    return instr_start_page;
}

uint32_t IRAM_ATTR instruction_flash_end_page_get(void)
{
    return instr_end_page;
}

int IRAM_ATTR instruction_flash2spiram_offset(void)
{
    return instr_flash2spiram_offs;
}
#endif

#if CONFIG_SPIRAM_RODATA
void rodata_flash_page_info_init(void)
{
    uint32_t rodata_page_cnt = ((uint32_t)&_rodata_reserved_end - ((uint32_t)&_rodata_reserved_start & ~ (MMU_PAGE_SIZE - 1)) + MMU_PAGE_SIZE - 1) / MMU_PAGE_SIZE;

    rodata_start_page = *(volatile uint32_t *)(DR_REG_MMU_TABLE + CACHE_DROM_MMU_START);
    rodata_start_page &= MMU_VALID_VAL_MASK;
    rodata_end_page = rodata_start_page + rodata_page_cnt - 1;
}

uint32_t IRAM_ATTR rodata_flash_start_page_get(void)
{
    return rodata_start_page;
}

uint32_t IRAM_ATTR rodata_flash_end_page_get(void)
{
    return rodata_end_page;
}

int IRAM_ATTR rodata_flash2spiram_offset(void)
{
    return rodata_flash2spiram_offs;
}
#endif

esp_err_t esp_spiram_init(void)
{
    esp_err_t r;
    uint32_t psram_physical_size = 0;
    r = psram_enable(PSRAM_SPEED, PSRAM_MODE);
    if (r != ESP_OK) {
#if CONFIG_SPIRAM_IGNORE_NOTFOUND
        ESP_EARLY_LOGE(TAG, "SPI RAM enabled but initialization failed. Bailing out.");
#endif
        return r;
    }
    s_spiram_inited = true;

    r = psram_get_physical_size(&psram_physical_size);
    if (r != ESP_OK) {
        abort();
    }
#if (CONFIG_SPIRAM_SIZE != -1)
    if (psram_physical_size != CONFIG_SPIRAM_SIZE) {
        ESP_EARLY_LOGE(TAG, "Expected %dMB chip but found %dMB chip. Bailing out..", (CONFIG_SPIRAM_SIZE / 1024 / 1024), (psram_physical_size / 1024 / 1024));
        return ESP_ERR_INVALID_SIZE;
    }
#endif
    ESP_EARLY_LOGI(TAG, "Found %dMB SPI RAM device", psram_physical_size / (1024 * 1024));
    ESP_EARLY_LOGI(TAG, "Speed: %dMHz", CONFIG_SPIRAM_SPEED);
    ESP_EARLY_LOGI(TAG, "Initialized, cache is in %s mode.", \
                   (PSRAM_MODE == PSRAM_VADDR_MODE_EVENODD) ? "even/odd (2-core)" : \
                   (PSRAM_MODE == PSRAM_VADDR_MODE_LOWHIGH) ? "low/high (2-core)" : \
                   (PSRAM_MODE == PSRAM_VADDR_MODE_NORMAL) ? "normal (1-core)" : "ERROR");
    return ESP_OK;
}

/**
 * Add entire external RAM region to heap allocator. Heap allocator knows the capabilities of this type of memory,
 * so there's no need to explicitly specify them.
 */
esp_err_t esp_spiram_add_to_heapalloc(void)
{
    ESP_EARLY_LOGI(TAG, "Adding pool of %dK of external SPI memory to heap allocator", (s_allocable_vaddr_end - s_allocable_vaddr_start) / 1024);
    return heap_caps_add_region(s_allocable_vaddr_start, s_allocable_vaddr_end - 1);
}


static uint8_t *dma_heap;

esp_err_t esp_spiram_reserve_dma_pool(size_t size)
{
    if (size == 0) {
        return ESP_OK;    //no-op
    }
    ESP_EARLY_LOGI(TAG, "Reserving pool of %dK of internal memory for DMA/internal allocations", size / 1024);
    dma_heap = heap_caps_malloc(size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (!dma_heap) {
        return ESP_ERR_NO_MEM;
    }
    uint32_t caps[] = {MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL, 0, MALLOC_CAP_8BIT | MALLOC_CAP_32BIT};
    return heap_caps_add_region_with_caps(caps, (intptr_t) dma_heap, (intptr_t) dma_heap + size - 1);
}

size_t esp_spiram_get_size(void)
{
    if (!s_spiram_inited) {
        ESP_EARLY_LOGE(TAG, "SPI RAM not initialized");
        abort();
    }

    uint32_t size = 0;  //in bytes
    esp_err_t ret = psram_get_available_size(&size);
    if (ret == ESP_OK) {
        return size;
    } else {
        return 0;
    }
}

/*
 Before flushing the cache, if psram is enabled as a memory-mapped thing, we need to write back the data in the cache to the psram first,
 otherwise it will get lost. For now, we just read 64/128K of random PSRAM memory to do this.
*/
void IRAM_ATTR esp_spiram_writeback_cache(void)
{
    extern void Cache_WriteBack_All(void);
    Cache_WriteBack_All();
}

/**
 * @brief If SPI RAM(PSRAM) has been initialized
 *
 * @return true SPI RAM has been initialized successfully
 * @return false SPI RAM hasn't been initialized or initialized failed
 */
bool esp_spiram_is_initialized(void)
{
    return s_spiram_inited;
}

uint8_t esp_spiram_get_cs_io(void)
{
    return psram_get_cs_io();
}

#endif
