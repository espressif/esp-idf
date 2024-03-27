/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
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
#include "esp_heap_caps_init.h"
#include "hal/mmu_hal.h"
#include "hal/mmu_ll.h"
#include "hal/cache_ll.h"
#include "esp_private/esp_psram_io.h"
#include "esp_private/esp_psram_extram.h"
#include "esp_private/mmu_psram_flash.h"
#include "esp_psram_impl.h"
#include "esp_psram.h"
#include "esp_private/esp_mmu_map_private.h"
#include "esp_mmu_map.h"
#include "esp_private/startup_internal.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/himem.h"
#include "esp32/rom/cache.h"
#include "esp_private/esp_cache_esp32_private.h"
#endif

/**
 * Two types of PSRAM memory regions for now:
 * - 8bit aligned
 * - 32bit aligned
 */
#define PSRAM_MEM_TYPE_NUM          2
#define PSRAM_MEM_8BIT_ALIGNED      0
#define PSRAM_MEM_32BIT_ALIGNED     1

#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
extern uint8_t _ext_ram_bss_start;
extern uint8_t _ext_ram_bss_end;
#endif //#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY

#if CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY
extern uint8_t _ext_ram_noinit_start;
extern uint8_t _ext_ram_noinit_end;
#endif  //#if CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY

typedef struct {
    intptr_t vaddr_start;
    intptr_t vaddr_end;
    size_t size;        //in bytes
} psram_mem_t;

typedef struct {
    bool is_initialised;
    /**
     * @note 1
     * As we can't use heap allocator during this stage, we need to statically declare these regions.
     * Luckily only S2 has two different types of memory regions:
     * - byte-aligned memory
     * - word-aligned memory
     * On the other hand, the type number usually won't be very big
     *
     * On other chips, only one region is needed.
     * So for chips other than S2, size of `regions_to_heap[1]` and `mapped_regions[1]`will always be zero.
     *
     * If in the future, this condition is worse (dbus memory isn't consecutive), we need to delegate this context
     * to chip-specific files, and only keep a (void *) pointer here pointing to those chip-specific contexts
     */
    psram_mem_t regions_to_heap[PSRAM_MEM_TYPE_NUM];     //memory regions that are available to be added to the heap allocator
    psram_mem_t mapped_regions[PSRAM_MEM_TYPE_NUM];      //mapped memory regions
} psram_ctx_t;

static psram_ctx_t s_psram_ctx;
static const char* TAG = "esp_psram";

ESP_SYSTEM_INIT_FN(init_psram, CORE, BIT(0), 103)
{
#if CONFIG_SPIRAM_BOOT_INIT && (CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MALLOC)
    if (esp_psram_is_initialized()) {
        esp_err_t r = esp_psram_extram_add_to_heap_allocator();
        if (r != ESP_OK) {
            ESP_EARLY_LOGE(TAG, "External RAM could not be added to heap!");
            abort();
        }
#if CONFIG_SPIRAM_USE_MALLOC
        heap_caps_malloc_extmem_enable(CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL);
#endif
    }
#endif
    return ESP_OK;
}

#if CONFIG_IDF_TARGET_ESP32
//If no function in esp_himem.c is used, this function will be linked into the
//binary instead of the one in esp_himem.c, automatically making sure no memory
//is reserved if no himem function is used.
size_t __attribute__((weak)) esp_himem_reserved_area_size(void)
{
    return 0;
}

static void IRAM_ATTR s_mapping(int v_start, int size)
{
    //Enable external RAM in MMU
    cache_sram_mmu_set(0, 0, v_start, 0, 32, (size / 1024 / 32));
    //Flush and enable icache for APP CPU
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    DPORT_CLEAR_PERI_REG_MASK(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CACHE_MASK_DRAM1);
    cache_sram_mmu_set(1, 0, v_start, 0, 32, (size / 1024 / 32));
#endif
}
#endif  //CONFIG_IDF_TARGET_ESP32

esp_err_t esp_psram_init(void)
{
    if (s_psram_ctx.is_initialised) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = ESP_FAIL;
    ret = esp_psram_impl_enable();
    if (ret != ESP_OK) {
#if CONFIG_SPIRAM_IGNORE_NOTFOUND
        ESP_EARLY_LOGE(TAG, "PSRAM enabled but initialization failed. Bailing out.");
#endif
        return ret;
    }
    s_psram_ctx.is_initialised = true;

    uint32_t psram_physical_size = 0;
    ret = esp_psram_impl_get_physical_size(&psram_physical_size);
    assert(ret == ESP_OK);

    ESP_EARLY_LOGI(TAG, "Found %" PRIu32 "MB PSRAM device", psram_physical_size / (1024 * 1024));
    ESP_EARLY_LOGI(TAG, "Speed: %dMHz", CONFIG_SPIRAM_SPEED);
#if CONFIG_IDF_TARGET_ESP32
#if CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    ESP_EARLY_LOGI(TAG, "PSRAM initialized, cache is in normal (1-core) mode.");
#else
    ESP_EARLY_LOGI(TAG, "PSRAM initialized, cache is in low/high (2-core) mode.");
#endif
#endif

    uint32_t psram_available_size = 0;
    ret = esp_psram_impl_get_available_size(&psram_available_size);
    assert(ret == ESP_OK);

    __attribute__((unused)) uint32_t total_available_size = psram_available_size;
    /**
     * `start_page` is the psram physical address in MMU page size.
     * MMU page size on ESP32S2 is 64KB
     * e.g.: psram physical address 16 is in page 0
     *
     * Here we plan to copy FLASH instructions to psram physical address 0, which is the No.0 page.
     */
    __attribute__((unused)) uint32_t start_page = 0;
#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA
    uint32_t used_page = 0;
#endif

    //------------------------------------Copy Flash .text to PSRAM-------------------------------------//
#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
    ret = mmu_config_psram_text_segment(start_page, total_available_size, &used_page);
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "No enough psram memory for instructon!");
        abort();
    }
    start_page += used_page;
    psram_available_size -= MMU_PAGE_TO_BYTES(used_page);
    ESP_EARLY_LOGV(TAG, "after copy .text, used page is %" PRIu32 ", start_page is %" PRIu32 ", psram_available_size is %" PRIu32 " B", used_page, start_page, psram_available_size);
#endif  //#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS

    //------------------------------------Copy Flash .rodata to PSRAM-------------------------------------//
#if CONFIG_SPIRAM_RODATA
    ret = mmu_config_psram_rodata_segment(start_page, total_available_size, &used_page);
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "No enough psram memory for rodata!");
        abort();
    }
    start_page += used_page;
    psram_available_size -= MMU_PAGE_TO_BYTES(used_page);
    ESP_EARLY_LOGV(TAG, "after copy .rodata, used page is %" PRIu32 ", start_page is %" PRIu32 ", psram_available_size is %" PRIu32 " B", used_page, start_page, psram_available_size);
#endif  //#if CONFIG_SPIRAM_RODATA

    //----------------------------------Map the PSRAM physical range to MMU-----------------------------//
    /**
     * @note 2
     * Similarly to @note 1, we expect HW DBUS memory to be consecutive.
     *
     * If situation is worse in the future (memory region isn't consecutive), we need to put these logics into chip-specific files
     */
    size_t total_mapped_size = 0;
    size_t size_to_map = 0;
    size_t byte_aligned_size = 0;
    ret = esp_mmu_map_get_max_consecutive_free_block_size(MMU_MEM_CAP_READ | MMU_MEM_CAP_WRITE | MMU_MEM_CAP_8BIT | MMU_MEM_CAP_32BIT, MMU_TARGET_PSRAM0, &byte_aligned_size);
    assert(ret == ESP_OK);
    size_to_map = MIN(byte_aligned_size, psram_available_size);

    const void *v_start_8bit_aligned = NULL;
    ret = esp_mmu_map_reserve_block_with_caps(size_to_map, MMU_MEM_CAP_READ | MMU_MEM_CAP_WRITE | MMU_MEM_CAP_8BIT | MMU_MEM_CAP_32BIT, MMU_TARGET_PSRAM0, &v_start_8bit_aligned);
    assert(ret == ESP_OK);

#if CONFIG_IDF_TARGET_ESP32
    s_mapping((int)v_start_8bit_aligned, size_to_map);
#else
    uint32_t actual_mapped_len = 0;
#if MMU_LL_MMU_PER_TARGET
    mmu_hal_map_region(1, MMU_TARGET_PSRAM0, (intptr_t)v_start_8bit_aligned, MMU_PAGE_TO_BYTES(start_page), size_to_map, &actual_mapped_len);
#else
    mmu_hal_map_region(0, MMU_TARGET_PSRAM0, (intptr_t)v_start_8bit_aligned, MMU_PAGE_TO_BYTES(start_page), size_to_map, &actual_mapped_len);
#endif
    start_page += BYTES_TO_MMU_PAGE(actual_mapped_len);
    ESP_EARLY_LOGV(TAG, "8bit-aligned-region: actual_mapped_len is 0x%" PRIx32 " bytes", actual_mapped_len);

    cache_bus_mask_t bus_mask = cache_ll_l1_get_bus(0, (uint32_t)v_start_8bit_aligned, actual_mapped_len);
    cache_ll_l1_enable_bus(0, bus_mask);
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    bus_mask = cache_ll_l1_get_bus(1, (uint32_t)v_start_8bit_aligned, actual_mapped_len);
    cache_ll_l1_enable_bus(1, bus_mask);
#endif
#endif  //#if CONFIG_IDF_TARGET_ESP32

    s_psram_ctx.mapped_regions[PSRAM_MEM_8BIT_ALIGNED].size = size_to_map;
    s_psram_ctx.mapped_regions[PSRAM_MEM_8BIT_ALIGNED].vaddr_start = (intptr_t)v_start_8bit_aligned;
    s_psram_ctx.mapped_regions[PSRAM_MEM_8BIT_ALIGNED].vaddr_end = (intptr_t)v_start_8bit_aligned + size_to_map;
    s_psram_ctx.regions_to_heap[PSRAM_MEM_8BIT_ALIGNED].size = size_to_map;
    s_psram_ctx.regions_to_heap[PSRAM_MEM_8BIT_ALIGNED].vaddr_start = (intptr_t)v_start_8bit_aligned;
    s_psram_ctx.regions_to_heap[PSRAM_MEM_8BIT_ALIGNED].vaddr_end = (intptr_t)v_start_8bit_aligned + size_to_map;
    ESP_EARLY_LOGV(TAG, "8bit-aligned-range: 0x%x B, starting from: %p", s_psram_ctx.mapped_regions[PSRAM_MEM_8BIT_ALIGNED].size, v_start_8bit_aligned);
    total_mapped_size += size_to_map;

#if CONFIG_IDF_TARGET_ESP32S2
    /**
     * On ESP32S2, there are 2 types of DBUS memory:
     * - byte-aligned-memory
     * - word-aligned-memory
     *
     * If byte-aligned-memory isn't enough, we search for word-aligned-memory to do mapping
     */
    if (total_mapped_size < psram_available_size) {
        size_to_map = psram_available_size - total_mapped_size;

        size_t word_aligned_size = 0;
        ret = esp_mmu_map_get_max_consecutive_free_block_size(MMU_MEM_CAP_READ | MMU_MEM_CAP_WRITE | MMU_MEM_CAP_32BIT, MMU_TARGET_PSRAM0,  &word_aligned_size);
        assert(ret == ESP_OK);
        size_to_map = MIN(word_aligned_size, size_to_map);

        const void *v_start_32bit_aligned = NULL;
        ret = esp_mmu_map_reserve_block_with_caps(size_to_map, MMU_MEM_CAP_READ | MMU_MEM_CAP_WRITE | MMU_MEM_CAP_32BIT, MMU_TARGET_PSRAM0, &v_start_32bit_aligned);
        assert(ret == ESP_OK);

        mmu_hal_map_region(0, MMU_TARGET_PSRAM0, (intptr_t)v_start_32bit_aligned, MMU_PAGE_TO_BYTES(start_page), size_to_map, &actual_mapped_len);
        ESP_EARLY_LOGV(TAG, "32bit-aligned-region: actual_mapped_len is 0x%" PRIx32 " bytes", actual_mapped_len);

        cache_bus_mask_t bus_mask = cache_ll_l1_get_bus(0, (uint32_t)v_start_32bit_aligned, actual_mapped_len);
        cache_ll_l1_enable_bus(0, bus_mask);

        s_psram_ctx.mapped_regions[PSRAM_MEM_32BIT_ALIGNED].size = size_to_map;
        s_psram_ctx.mapped_regions[PSRAM_MEM_32BIT_ALIGNED].vaddr_start = (intptr_t)v_start_32bit_aligned;
        s_psram_ctx.mapped_regions[PSRAM_MEM_32BIT_ALIGNED].vaddr_end = (intptr_t)v_start_32bit_aligned + size_to_map;
        s_psram_ctx.regions_to_heap[PSRAM_MEM_32BIT_ALIGNED].size = size_to_map;
        s_psram_ctx.regions_to_heap[PSRAM_MEM_32BIT_ALIGNED].vaddr_start = (intptr_t)v_start_32bit_aligned;
        s_psram_ctx.regions_to_heap[PSRAM_MEM_32BIT_ALIGNED].vaddr_end = (intptr_t)v_start_32bit_aligned + size_to_map;
        ESP_EARLY_LOGV(TAG, "32bit-aligned-range: 0x%x B, starting from: %p", s_psram_ctx.mapped_regions[PSRAM_MEM_32BIT_ALIGNED].size, v_start_32bit_aligned);
        total_mapped_size += size_to_map;
    }
#endif  //  #if CONFIG_IDF_TARGET_ESP32S2

    if (total_mapped_size < psram_available_size) {
        ESP_EARLY_LOGW(TAG, "Virtual address not enough for PSRAM, map as much as we can. %dMB is mapped", total_mapped_size / 1024 / 1024);
    }

    /*------------------------------------------------------------------------------
    * After mapping, we DON'T care about the PSRAM PHYSICAL ADDRESSS ANYMORE!
    *----------------------------------------------------------------------------*/

    //------------------------------------Configure .bss in PSRAM-------------------------------------//
#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
    //should never be negative number
    uint32_t ext_bss_size = ((intptr_t)&_ext_ram_bss_end - (intptr_t)&_ext_ram_bss_start);
    ESP_EARLY_LOGV(TAG, "ext_bss_size is %" PRIu32, ext_bss_size);
    s_psram_ctx.regions_to_heap[PSRAM_MEM_8BIT_ALIGNED].vaddr_start += ext_bss_size;
    s_psram_ctx.regions_to_heap[PSRAM_MEM_8BIT_ALIGNED].size -= ext_bss_size;
#endif  //#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY

#if CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY
    uint32_t ext_noinit_size = ((intptr_t)&_ext_ram_noinit_end - (intptr_t)&_ext_ram_noinit_start);
    ESP_EARLY_LOGV(TAG, "ext_noinit_size is %" PRIu32, ext_noinit_size);
    s_psram_ctx.regions_to_heap[PSRAM_MEM_8BIT_ALIGNED].vaddr_start += ext_noinit_size;
    s_psram_ctx.regions_to_heap[PSRAM_MEM_8BIT_ALIGNED].size -= ext_noinit_size;
#endif

#if CONFIG_IDF_TARGET_ESP32
    s_psram_ctx.regions_to_heap[PSRAM_MEM_8BIT_ALIGNED].size -= esp_himem_reserved_area_size() - 1;
#endif

    //will be removed, TODO: IDF-6944
#if CONFIG_IDF_TARGET_ESP32
    cache_driver_t drv = {
        NULL,
        esp_psram_extram_writeback_cache,
    };
    cache_register_writeback(&drv);
#endif

    return ESP_OK;
}

esp_err_t esp_psram_extram_add_to_heap_allocator(void)
{
    esp_err_t ret = ESP_FAIL;

    uint32_t byte_aligned_caps[] = {MALLOC_CAP_SPIRAM | MALLOC_CAP_DEFAULT, 0, MALLOC_CAP_8BIT | MALLOC_CAP_32BIT};
    ret = heap_caps_add_region_with_caps(byte_aligned_caps,
                                         s_psram_ctx.regions_to_heap[PSRAM_MEM_8BIT_ALIGNED].vaddr_start,
                                         s_psram_ctx.regions_to_heap[PSRAM_MEM_8BIT_ALIGNED].vaddr_end);
    if (ret != ESP_OK) {
        return ret;
    }

    if (s_psram_ctx.regions_to_heap[PSRAM_MEM_32BIT_ALIGNED].size) {
        assert(s_psram_ctx.regions_to_heap[PSRAM_MEM_32BIT_ALIGNED].vaddr_start);
        uint32_t word_aligned_caps[] = {MALLOC_CAP_SPIRAM | MALLOC_CAP_DEFAULT, 0, MALLOC_CAP_32BIT};
        ret = heap_caps_add_region_with_caps(word_aligned_caps,
                                             s_psram_ctx.regions_to_heap[PSRAM_MEM_32BIT_ALIGNED].vaddr_start,
                                             s_psram_ctx.regions_to_heap[PSRAM_MEM_32BIT_ALIGNED].vaddr_end);
        if (ret != ESP_OK) {
            return ret;
        }
    }

    ESP_EARLY_LOGI(TAG, "Adding pool of %dK of PSRAM memory to heap allocator",
                   (s_psram_ctx.regions_to_heap[PSRAM_MEM_8BIT_ALIGNED].size + s_psram_ctx.regions_to_heap[PSRAM_MEM_32BIT_ALIGNED].size) / 1024);

    return ESP_OK;
}

bool IRAM_ATTR esp_psram_check_ptr_addr(const void *p)
{
    if (!s_psram_ctx.is_initialised) {
        return false;
    }

    return ((intptr_t)p >= s_psram_ctx.mapped_regions[PSRAM_MEM_8BIT_ALIGNED].vaddr_start && (intptr_t)p < s_psram_ctx.mapped_regions[PSRAM_MEM_8BIT_ALIGNED].vaddr_end) ||
           ((intptr_t)p >= s_psram_ctx.mapped_regions[PSRAM_MEM_32BIT_ALIGNED].vaddr_start && (intptr_t)p < s_psram_ctx.mapped_regions[PSRAM_MEM_32BIT_ALIGNED].vaddr_end);
}

esp_err_t esp_psram_extram_reserve_dma_pool(size_t size)
{
    if (size == 0) {
        return ESP_OK; //no-op
    }

    ESP_EARLY_LOGI(TAG, "Reserving pool of %dK of internal memory for DMA/internal allocations", size / 1024);
    /* Pool may be allocated in multiple non-contiguous chunks, depending on available RAM */
    while (size > 0) {
        size_t next_size = heap_caps_get_largest_free_block(MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
        next_size = MIN(next_size, size);

        ESP_EARLY_LOGD(TAG, "Allocating block of size %d bytes", next_size);
        uint8_t *dma_heap = heap_caps_malloc(next_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
        if (!dma_heap || next_size == 0) {
            return ESP_ERR_NO_MEM;
        }

        uint32_t caps[] = {0, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL, MALLOC_CAP_8BIT | MALLOC_CAP_32BIT};
        esp_err_t e = heap_caps_add_region_with_caps(caps, (intptr_t)dma_heap, (intptr_t)dma_heap + next_size - 1);
        if (e != ESP_OK) {
            return e;
        }
        size -= next_size;
    }
    return ESP_OK;
}

bool IRAM_ATTR __attribute__((pure)) esp_psram_is_initialized(void)
{
    return s_psram_ctx.is_initialised;
}

size_t esp_psram_get_size(void)
{
    uint32_t available_size = 0;
    esp_err_t ret = esp_psram_impl_get_available_size(&available_size);
    if (ret != ESP_OK) {
        //This means PSRAM isn't initialised, to keep back-compatibility, set size to 0.
        available_size = 0;
    }
    return (size_t)available_size;
}

uint8_t esp_psram_io_get_cs_io(void)
{
    return esp_psram_impl_get_cs_io();
}

/*
 Simple RAM test. Writes a word every 32 bytes. Takes about a second to complete for 4MiB. Returns
 true when RAM seems OK, false when test fails. WARNING: Do not run this before the 2nd cpu has been
 initialized (in a two-core system) or after the heap allocator has taken ownership of the memory.
*/
static bool s_test_psram(intptr_t v_start, size_t size, intptr_t reserved_start, intptr_t reserved_end)
{
    volatile int *spiram = (volatile int *)v_start;
    size_t p;
    int errct = 0;
    int initial_err = -1;
    for (p = 0; p < (size / sizeof(int)); p += 8) {
        intptr_t addr = (intptr_t)&spiram[p];
        if ((reserved_start <= addr) && (addr < reserved_end)) {
            continue;
        }
        spiram[p] = p ^ 0xAAAAAAAA;
    }
    for (p = 0; p < (size / sizeof(int)); p += 8) {
        intptr_t addr = (intptr_t)&spiram[p];
        if ((reserved_start <= addr) && (addr < reserved_end)) {
            continue;
        }
        if (spiram[p] != (p ^ 0xAAAAAAAA)) {
            errct++;
            if (errct == 1) {
                initial_err = p * 4;
            }
        }
    }
    if (errct) {
        ESP_EARLY_LOGE(TAG, "SPI SRAM memory test fail. %d/%d writes failed, first @ %X", errct, size / 32, initial_err + v_start);
        return false;
    } else {
        ESP_EARLY_LOGI(TAG, "SPI SRAM memory test OK");
        return true;
    }

}

bool esp_psram_extram_test(void)
{
    bool test_success = false;
#if CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY
    intptr_t noinit_vstart = (intptr_t)&_ext_ram_noinit_start;
    intptr_t noinit_vend = (intptr_t)&_ext_ram_noinit_end;
#else
    intptr_t noinit_vstart = 0;
    intptr_t noinit_vend = 0;
#endif
    test_success = s_test_psram(s_psram_ctx.mapped_regions[PSRAM_MEM_8BIT_ALIGNED].vaddr_start,
                                s_psram_ctx.mapped_regions[PSRAM_MEM_8BIT_ALIGNED].size,
                                noinit_vstart,
                                noinit_vend);
    if (!test_success) {
        return false;
    }

    if (s_psram_ctx.mapped_regions[PSRAM_MEM_32BIT_ALIGNED].size) {
        test_success = s_test_psram(s_psram_ctx.mapped_regions[PSRAM_MEM_32BIT_ALIGNED].vaddr_start,
                                    s_psram_ctx.mapped_regions[PSRAM_MEM_32BIT_ALIGNED].size,
                                    0,
                                    0);
    }
    if (!test_success) {
        return false;
    }

    return true;
}
