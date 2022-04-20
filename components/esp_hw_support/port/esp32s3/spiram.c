/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


/*----------------------------------------------------------------------------------------------------
 * Abstraction layer for PSRAM. PSRAM device related registers and MMU/Cache related code shouls be
 * abstracted to lower layers.
 *
 * When we add more types of external RAM memory, this can be made into a more intelligent dispatcher.
 *----------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_heap_caps_init.h"
#include "esp_private/spiram_private.h"
#include "esp32s3/spiram.h"
#include "spiram_psram.h"
#include "hal/mmu_hal.h"
#include "hal/cache_ll.h"


#define PSRAM_MODE PSRAM_VADDR_MODE_NORMAL

#define MMU_PAGE_SIZE    0x10000
#define ALIGN_UP_BY(num, align) (((num) + ((align) - 1)) & ~((align) - 1))

#if CONFIG_SPIRAM_SPEED_40M
#define PSRAM_SPEED PSRAM_CACHE_S40M
#else  //#if CONFIG_SPIRAM_SPEED_80M
#define PSRAM_SPEED PSRAM_CACHE_S80M
#endif


static const char *TAG = "spiram";
static bool s_spiram_inited;

//These variables are in bytes
static intptr_t s_allocable_vaddr_start;
static intptr_t s_allocable_vaddr_end;
static intptr_t s_mapped_vaddr_start;
static intptr_t s_mapped_vaddr_end;


#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
extern uint8_t _ext_ram_bss_start;
extern uint8_t _ext_ram_bss_end;
#endif //#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY


static bool esp_spiram_test(uint32_t v_start, uint32_t size);


esp_err_t esp_spiram_init(void)
{
    assert(!s_spiram_inited);
    esp_err_t ret;
    uint32_t psram_physical_size = 0;
    ret = psram_enable(PSRAM_SPEED, PSRAM_MODE);
    if (ret != ESP_OK) {
#if CONFIG_SPIRAM_IGNORE_NOTFOUND
        ESP_EARLY_LOGE(TAG, "SPI RAM enabled but initialization failed. Bailing out.");
#endif
        return ret;
    }
    s_spiram_inited = true;

    ret = psram_get_physical_size(&psram_physical_size);
    assert(ret == ESP_OK);

#if (CONFIG_SPIRAM_SIZE != -1)
    if (psram_physical_size != CONFIG_SPIRAM_SIZE) {
        ESP_EARLY_LOGE(TAG, "Expected %dMB chip but found %dMB chip. Bailing out..", (CONFIG_SPIRAM_SIZE / 1024 / 1024), (psram_physical_size / 1024 / 1024));
        return ESP_ERR_INVALID_SIZE;
    }
#endif
    ESP_EARLY_LOGI(TAG, "Found %dMB SPI RAM device", psram_physical_size / (1024 * 1024));
    ESP_EARLY_LOGI(TAG, "Speed: %dMHz", CONFIG_SPIRAM_SPEED);

    /**
     * TODO IDF-4318
     * Add these feature here:
     * - Copy Flash text into PSRAM
     * - Copy Flash rodata into PSRAM
     */
    //----------------------------------Map the PSRAM physical range to MMU-----------------------------//
    uint32_t vaddr_start = 0;
    extern uint32_t _rodata_reserved_end;
    uint32_t rodata_end_aligned = ALIGN_UP_BY((uint32_t)&_rodata_reserved_end, MMU_PAGE_SIZE);
    vaddr_start = rodata_end_aligned;
    ESP_EARLY_LOGV(TAG, "rodata_end_aligned is 0x%x bytes", rodata_end_aligned);

    uint32_t psram_available_size = 0;
    ret = psram_get_available_size(&psram_available_size);
    assert(ret == ESP_OK);

    if (vaddr_start + psram_available_size > DRAM0_CACHE_ADDRESS_HIGH) {
        //Decide these logics when there's a real PSRAM with larger size
        ESP_EARLY_LOGE(TAG, "Virtual address not enough for PSRAM!");
        abort();
    }

    //On ESP32S3, MMU is shared for both of the cores. Note this when porting `spiram.c`
    uint32_t actual_mapped_len = 0;
    mmu_hal_map_region(0, MMU_TARGET_PSRAM0, vaddr_start, 0, psram_available_size, &actual_mapped_len);
    ESP_EARLY_LOGV(TAG, "actual_mapped_len is 0x%x bytes", actual_mapped_len);

    cache_bus_mask_t bus_mask = cache_ll_l1_get_bus(0, vaddr_start, actual_mapped_len);
    cache_ll_l1_enable_bus(0, bus_mask);
#if !CONFIG_FREERTOS_UNICORE
    bus_mask = cache_ll_l1_get_bus(1, vaddr_start, actual_mapped_len);
    cache_ll_l1_enable_bus(1, bus_mask);
#endif

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
    s_mapped_vaddr_start = vaddr_start;
    s_mapped_vaddr_end = vaddr_start + psram_available_size;
    s_allocable_vaddr_start = vaddr_start;
    s_allocable_vaddr_end = vaddr_start + psram_available_size;


    //------------------------------------Configure .bss in PSRAM-------------------------------------//
#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
    //should never be negative number
    uint32_t ext_bss_size = ((intptr_t)&_ext_ram_bss_end - (intptr_t)&_ext_ram_bss_start);
    ESP_EARLY_LOGV(TAG, "_ext_ram_bss_start is 0x%x, _ext_ram_bss_start is 0x%x, ext_bss_size is 0x%x bytes", &_ext_ram_bss_start, &_ext_ram_bss_end, ext_bss_size);

    s_allocable_vaddr_start += ext_bss_size;
#endif  //#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY

    ESP_EARLY_LOGV(TAG, "s_allocable_vaddr_start is 0x%x, s_allocable_vaddr_end is 0x%x", s_allocable_vaddr_start, s_allocable_vaddr_end);
    return ESP_OK;
}

/**
 * Add the PSRAM available region to heap allocator. Heap allocator knows the capabilities of this type of memory,
 * so there's no need to explicitly specify them.
 */
esp_err_t esp_spiram_add_to_heapalloc(void)
{
    ESP_EARLY_LOGI(TAG, "Adding pool of %dK of external SPI memory to heap allocator", (s_allocable_vaddr_end - s_allocable_vaddr_start) / 1024);
    return heap_caps_add_region(s_allocable_vaddr_start, s_allocable_vaddr_end);
}

esp_err_t IRAM_ATTR esp_spiram_get_mapped_range(intptr_t *out_vstart, intptr_t *out_vend)
{
    if (!out_vstart || !out_vend) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!s_spiram_inited) {
        return ESP_ERR_INVALID_STATE;
    }

    *out_vstart = s_mapped_vaddr_start;
    *out_vend = s_mapped_vaddr_end;
    return ESP_OK;
}

esp_err_t esp_spiram_get_alloced_range(intptr_t *out_vstart, intptr_t *out_vend)
{
    if (!out_vstart || !out_vend) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!s_spiram_inited) {
        return ESP_ERR_INVALID_STATE;
    }

    *out_vstart = s_allocable_vaddr_start;
    *out_vend = s_allocable_vaddr_end;
    return ESP_OK;
}

esp_err_t esp_spiram_reserve_dma_pool(size_t size)
{
    if (size == 0) {
        return ESP_OK;
    }
    ESP_EARLY_LOGI(TAG, "Reserving pool of %dK of internal memory for DMA/internal allocations", size / 1024);
    uint8_t *dma_heap = heap_caps_malloc(size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (!dma_heap) {
        return ESP_ERR_NO_MEM;
    }
    uint32_t caps[] = {MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL, 0, MALLOC_CAP_8BIT | MALLOC_CAP_32BIT};
    return heap_caps_add_region_with_caps(caps, (intptr_t) dma_heap, (intptr_t) dma_heap + size);
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

/*
 Simple RAM test. Writes a word every 32 bytes. Takes about a second to complete for 4MiB. Returns
 true when RAM seems OK, false when test fails. WARNING: Do not run this before the 2nd cpu has been
 initialized (in a two-core system) or after the heap allocator has taken ownership of the memory.
*/
static bool esp_spiram_test(uint32_t v_start, uint32_t size)
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
        ESP_EARLY_LOGI(TAG, "SPI SRAM memory test OK");
        return true;
    }
}
