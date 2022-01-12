/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


/*----------------------------------------------------------------------------------------------------
 * Abstraction layer for PSRAM. PSRAM device related registers and MMU/Cache related code shouls be
 * abstracted to lower layers.
 *
 * When we add more types of external RAM memory, this can be made into a more intelligent dispatcher.
 *----------------------------------------------------------------------------------------------------*/
#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "esp_heap_caps_init.h"
#include "esp32s2/spiram.h"
#include "esp_private/mmu_psram.h"
#include "spiram_psram.h"

#define PSRAM_MODE PSRAM_VADDR_MODE_NORMAL

#if CONFIG_SPIRAM

#if CONFIG_SPIRAM_SPEED_40M
#define PSRAM_SPEED PSRAM_CACHE_S40M
#elif CONFIG_SPIRAM_SPEED_80M
#define PSRAM_SPEED PSRAM_CACHE_S80M
#else
#define PSRAM_SPEED PSRAM_CACHE_S20M
#endif

#define MMU_PAGE_TO_BYTES(page_id)      ((page_id) << 16)


#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
extern uint8_t _ext_ram_bss_start;
extern uint8_t _ext_ram_bss_end;
#endif //#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY

//These variables are in bytes
static uint32_t s_allocable_vaddr_start;
static uint32_t s_allocable_vaddr_end;

static bool spiram_inited=false;
static const char* TAG = "spiram";

bool esp_spiram_test(uint32_t v_start, uint32_t size);


esp_err_t esp_spiram_init(void)
{
    esp_err_t r;
    r = psram_enable(PSRAM_SPEED, PSRAM_MODE);
    if (r != ESP_OK) {
#if CONFIG_SPIRAM_IGNORE_NOTFOUND
        ESP_EARLY_LOGE(TAG, "SPI RAM enabled but initialization failed. Bailing out.");
#endif
        return r;
    }

    spiram_inited = true;

    //TODO IDF-4380
    size_t spiram_size = esp_spiram_get_size();
#if (CONFIG_SPIRAM_SIZE != -1)
    if (spiram_size != CONFIG_SPIRAM_SIZE) {
        ESP_EARLY_LOGE(TAG, "Expected %dKiB chip but found %dKiB chip. Bailing out..", CONFIG_SPIRAM_SIZE/1024, spiram_size/1024);
        return ESP_ERR_INVALID_SIZE;
    }
#endif

    ESP_EARLY_LOGI(TAG, "Found %dMBit SPI RAM device",
                                          (spiram_size*8)/(1024*1024));
    ESP_EARLY_LOGI(TAG, "SPI RAM mode: %s", PSRAM_SPEED == PSRAM_CACHE_S40M ? "sram 40m" : \
                                          PSRAM_SPEED == PSRAM_CACHE_S80M ? "sram 80m" : "sram 20m");
    ESP_EARLY_LOGI(TAG, "PSRAM initialized, cache is in %s mode.", \
                                          (PSRAM_MODE==PSRAM_VADDR_MODE_EVENODD)?"even/odd (2-core)": \
                                          (PSRAM_MODE==PSRAM_VADDR_MODE_LOWHIGH)?"low/high (2-core)": \
                                          (PSRAM_MODE==PSRAM_VADDR_MODE_NORMAL)?"normal (1-core)":"ERROR");

    uint32_t psram_available_size = spiram_size;
    /**
     * `start_page` is the psram physical address in MMU page size.
     * MMU page size on ESP32S2 is 64KB
     * e.g.: psram physical address 16 is in page 0
     *
     * Here we plan to copy FLASH instructions to psram physical address 0, which is the No.0 page.
     */
    uint32_t start_page = 0;
#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA
    uint32_t used_page = 0;
#endif

    //------------------------------------Copy Flash .text to PSRAM-------------------------------------//
#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
    r = mmu_config_psram_text_segment(start_page, spiram_size, &used_page);
    if (r != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "No enough psram memory for instructon!");
        abort();
    }
    start_page += used_page;
    psram_available_size -= MMU_PAGE_TO_BYTES(used_page);
    ESP_EARLY_LOGV(TAG, "after copy .text, used page is %d, start_page is %d, psram_available_size is %d B", used_page, start_page, psram_available_size);
#endif  //#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS

    //------------------------------------Copy Flash .rodata to PSRAM-------------------------------------//
#if CONFIG_SPIRAM_RODATA
    r = mmu_config_psram_rodata_segment(start_page, spiram_size, &used_page);
    if (r != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "No enough psram memory for rodata!");
        abort();
    }
    start_page += used_page;
    psram_available_size -= MMU_PAGE_TO_BYTES(used_page);
    ESP_EARLY_LOGV(TAG, "after copy .rodata, used page is %d, start_page is %d, psram_available_size is %d B", used_page, start_page, psram_available_size);
#endif  //#if CONFIG_SPIRAM_RODATA

    //Map the PSRAM physical range to MMU
    static DRAM_ATTR uint32_t vaddr_start = 0;
    mmu_map_psram(MMU_PAGE_TO_BYTES(start_page), MMU_PAGE_TO_BYTES(start_page) + psram_available_size, &vaddr_start);
    if (r != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "MMU PSRAM mapping wrong!");
        abort();
    }

#if CONFIG_SPIRAM_MEMTEST
    //After mapping, simple test SPIRAM first
    bool ext_ram_ok = esp_spiram_test(vaddr_start, psram_available_size);
    if (!ext_ram_ok) {
        ESP_EARLY_LOGE(TAG, "External RAM failed memory test!");
        abort();
    }
#endif  //#if CONFIG_SPIRAM_MEMTEST

    /*------------------------------------------------------------------------------
    * After mapping, we DON'T care about the PSRAM PHYSICAL ADDRESSS ANYMORE!
    *----------------------------------------------------------------------------*/
    s_allocable_vaddr_start = vaddr_start;
    s_allocable_vaddr_end = vaddr_start + psram_available_size;

    //------------------------------------Configure .bss in PSRAM-------------------------------------//
#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
    //should never be negative number
    uint32_t ext_bss_size = ((intptr_t)&_ext_ram_bss_end - (intptr_t)&_ext_ram_bss_start);
    ESP_EARLY_LOGV(TAG, "ext_bss_size is %d", ext_bss_size);

    s_allocable_vaddr_start += ext_bss_size;
#endif  //#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY

    ESP_EARLY_LOGV(TAG, "s_allocable_vaddr_start is 0x%x, s_allocable_vaddr_end is 0x%x", s_allocable_vaddr_start, s_allocable_vaddr_end);
    return ESP_OK;
}

esp_err_t esp_spiram_add_to_heapalloc(void)
{
    return heap_caps_add_region(s_allocable_vaddr_start, s_allocable_vaddr_end - 1);
}

static uint8_t *dma_heap;

esp_err_t esp_spiram_reserve_dma_pool(size_t size) {
    if (size==0) return ESP_OK; //no-op
    ESP_EARLY_LOGI(TAG, "Reserving pool of %dK of internal memory for DMA/internal allocations", size/1024);
    dma_heap=heap_caps_malloc(size, MALLOC_CAP_DMA|MALLOC_CAP_INTERNAL);
    if (!dma_heap) return ESP_ERR_NO_MEM;
    uint32_t caps[]={MALLOC_CAP_DMA|MALLOC_CAP_INTERNAL, 0, MALLOC_CAP_8BIT|MALLOC_CAP_32BIT};
    return heap_caps_add_region_with_caps(caps, (intptr_t) dma_heap, (intptr_t) dma_heap+size-1);
}

//TODO IDF-4380
size_t esp_spiram_get_size(void)
{
    if (!spiram_inited) {
        ESP_EARLY_LOGE(TAG, "SPI RAM not initialized");
        abort();
    }

    psram_size_t size=psram_get_size();
    if (size==PSRAM_SIZE_16MBITS) return 2*1024*1024;
    if (size==PSRAM_SIZE_32MBITS) return 4*1024*1024;
    if (size==PSRAM_SIZE_64MBITS) return 8*1024*1024;
    return CONFIG_SPIRAM_SIZE;
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
    return spiram_inited;
}

uint8_t esp_spiram_get_cs_io(void)
{
    return psram_get_cs_io();
}

/*
 Simple RAM test. Writes a word every 32 bytes. Takes about a second to complete for 4MiB. Returns
 true when RAM seems OK, false when test fails. WARNING: Do not run this before the 2nd cpu has been
 initialized (in a two-core system) or after the heap allocator has taken ownership of the memory.
*/
bool esp_spiram_test(uint32_t v_start, uint32_t size)
{
    volatile int *spiram = (volatile int *)v_start;

    size_t s = size;
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
        return true;
    }
}
#endif  //#if CONFIG_SPIRAM
