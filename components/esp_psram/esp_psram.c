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
#include "hal/mmu_hal.h"
#include "hal/cache_ll.h"
#include "esp_private/esp_psram_io.h"
#include "esp_private/esp_psram_extram.h"
#include "esp_private/mmu.h"
#include "esp_psram_impl.h"
#include "esp_psram.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/himem.h"
#include "esp32/rom/cache.h"
#endif


#if CONFIG_IDF_TARGET_ESP32
#if CONFIG_FREERTOS_UNICORE
#define PSRAM_MODE PSRAM_VADDR_MODE_NORMAL
#else
#define PSRAM_MODE PSRAM_VADDR_MODE_LOWHIGH
#endif
#else
#define PSRAM_MODE PSRAM_VADDR_MODE_NORMAL
#endif


#if CONFIG_SPIRAM

#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
extern uint8_t _ext_ram_bss_start;
extern uint8_t _ext_ram_bss_end;
#endif //#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY

#if CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY
extern uint8_t _ext_ram_noinit_start;
extern uint8_t _ext_ram_noinit_end;
#endif  //#if CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY

//These variables are in bytes
static intptr_t s_allocable_vaddr_start;
static intptr_t s_allocable_vaddr_end;
static intptr_t s_mapped_vaddr_start;
static intptr_t s_mapped_vaddr_end;

static bool s_spiram_inited;
static const char* TAG = "esp_psram";


#if CONFIG_IDF_TARGET_ESP32
//If no function in esp_himem.c is used, this function will be linked into the
//binary instead of the one in esp_himem.c, automatically making sure no memory
//is reserved if no himem function is used.
size_t __attribute__((weak)) esp_himem_reserved_area_size(void) {
    return 0;
}

static void IRAM_ATTR s_mapping(int v_start, int size)
{
    //Enable external RAM in MMU
    cache_sram_mmu_set(0, 0, v_start, 0, 32, (size / 1024 / 32));
    //Flush and enable icache for APP CPU
#if !CONFIG_FREERTOS_UNICORE
    DPORT_CLEAR_PERI_REG_MASK(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CACHE_MASK_DRAM1);
    cache_sram_mmu_set(1, 0, v_start, 0, 32, (size / 1024 / 32));
#endif
}
#endif  //CONFIG_IDF_TARGET_ESP32

esp_err_t esp_psram_init(void)
{
    if (s_spiram_inited) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t ret;
    ret = esp_psram_impl_enable(PSRAM_MODE);
    if (ret != ESP_OK) {
#if CONFIG_SPIRAM_IGNORE_NOTFOUND
        ESP_EARLY_LOGE(TAG, "SPI RAM enabled but initialization failed. Bailing out.");
#endif
        return ret;
    }
    s_spiram_inited = true;

    uint32_t psram_physical_size = 0;
    ret = esp_psram_impl_get_physical_size(&psram_physical_size);
    assert(ret == ESP_OK);

    ESP_EARLY_LOGI(TAG, "Found %dMB SPI RAM device", psram_physical_size / (1024 * 1024));
    ESP_EARLY_LOGI(TAG, "Speed: %dMHz", CONFIG_SPIRAM_SPEED);
#if CONFIG_IDF_TARGET_ESP32
    ESP_EARLY_LOGI(TAG, "PSRAM initialized, cache is in %s mode.", \
                                          (PSRAM_MODE==PSRAM_VADDR_MODE_EVENODD)?"even/odd (2-core)": \
                                          (PSRAM_MODE==PSRAM_VADDR_MODE_LOWHIGH)?"low/high (2-core)": \
                                          (PSRAM_MODE==PSRAM_VADDR_MODE_NORMAL)?"normal (1-core)":"ERROR");
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
    ESP_EARLY_LOGV(TAG, "after copy .text, used page is %d, start_page is %d, psram_available_size is %d B", used_page, start_page, psram_available_size);
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
    ESP_EARLY_LOGV(TAG, "after copy .rodata, used page is %d, start_page is %d, psram_available_size is %d B", used_page, start_page, psram_available_size);
#endif  //#if CONFIG_SPIRAM_RODATA


    //----------------------------------Map the PSRAM physical range to MMU-----------------------------//
    intptr_t vaddr_start = mmu_get_psram_vaddr_start();
    if (vaddr_start + psram_available_size > mmu_get_psram_vaddr_end()) {
        psram_available_size = mmu_get_psram_vaddr_end() - vaddr_start;
        ESP_EARLY_LOGV(TAG, "Virtual address not enough for PSRAM, map as much as we can. %dMB is mapped", psram_available_size / 1024 / 1024);
    }

#if CONFIG_IDF_TARGET_ESP32
    s_mapping(vaddr_start, psram_available_size);
#else
    uint32_t actual_mapped_len = 0;
    mmu_hal_map_region(0, MMU_TARGET_PSRAM0, vaddr_start, MMU_PAGE_TO_BYTES(start_page), psram_available_size, &actual_mapped_len);
    ESP_EARLY_LOGV(TAG, "actual_mapped_len is 0x%x bytes", actual_mapped_len);

    cache_bus_mask_t bus_mask = cache_ll_l1_get_bus(0, vaddr_start, actual_mapped_len);
    cache_ll_l1_enable_bus(0, bus_mask);
#if !CONFIG_FREERTOS_UNICORE
    bus_mask = cache_ll_l1_get_bus(1, vaddr_start, actual_mapped_len);
    cache_ll_l1_enable_bus(1, bus_mask);
#endif
#endif  //#if CONFIG_IDF_TARGET_ESP32

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
    ESP_EARLY_LOGV(TAG, "ext_bss_size is %d", ext_bss_size);
    s_allocable_vaddr_start += ext_bss_size;
#endif  //#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY

#if CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY
    uint32_t ext_noinit_size = ((intptr_t)&_ext_ram_noinit_end - (intptr_t)&_ext_ram_noinit_start);
    ESP_EARLY_LOGV(TAG, "ext_noinit_size is %d", ext_noinit_size);
    s_allocable_vaddr_start += ext_noinit_size;
#endif

#if CONFIG_IDF_TARGET_ESP32
    s_allocable_vaddr_end -= esp_himem_reserved_area_size() - 1;
#endif

    ESP_EARLY_LOGV(TAG, "s_allocable_vaddr_start is 0x%x, s_allocable_vaddr_end is 0x%x", s_allocable_vaddr_start, s_allocable_vaddr_end);
    return ESP_OK;
}

/**
 * Add the PSRAM available region to heap allocator. Heap allocator knows the capabilities of this type of memory,
 * so there's no need to explicitly specify them.
 */
esp_err_t esp_psram_extram_add_to_heap_allocator(void)
{
    ESP_EARLY_LOGI(TAG, "Adding pool of %dK of external SPI memory to heap allocator", (s_allocable_vaddr_end - s_allocable_vaddr_start) / 1024);
    return heap_caps_add_region(s_allocable_vaddr_start, s_allocable_vaddr_end);
}

esp_err_t IRAM_ATTR esp_psram_extram_get_mapped_range(intptr_t *out_vstart, intptr_t *out_vend)
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

esp_err_t esp_psram_extram_get_alloced_range(intptr_t *out_vstart, intptr_t *out_vend)
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

bool IRAM_ATTR esp_psram_is_initialized(void)
{
    return s_spiram_inited;
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
bool esp_psram_extram_test(void)
{
#if CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY
    const void *keepout_addr_low = (const void*)&_ext_ram_noinit_start;
    const void *keepout_addr_high = (const void*)&_ext_ram_noinit_end;
#else
    const void *keepout_addr_low = 0;
    const void *keepout_addr_high = 0;
#endif

    volatile int *spiram = (volatile int *)s_mapped_vaddr_start;
    size_t p;
    size_t s = s_mapped_vaddr_end - s_mapped_vaddr_start;
    int errct=0;
    int initial_err=-1;
    for (p=0; p<(s/sizeof(int)); p+=8) {
        const void *addr = (const void *)&spiram[p];
        if ((keepout_addr_low <= addr) && (addr < keepout_addr_high)) {
            continue;
        }
        spiram[p]=p^0xAAAAAAAA;
    }
    for (p=0; p<(s/sizeof(int)); p+=8) {
        const void *addr = (const void *)&spiram[p];
        if ((keepout_addr_low <= addr) && (addr < keepout_addr_high)) {
            continue;
        }
        if (spiram[p]!=(p^0xAAAAAAAA)) {
            errct++;
            if (errct==1) initial_err=p*4;
        }
    }
    if (errct) {
        ESP_EARLY_LOGE(TAG, "SPI SRAM memory test fail. %d/%d writes failed, first @ %X\n", errct, s/32, initial_err + s_mapped_vaddr_start);
        return false;
    } else {
        ESP_EARLY_LOGI(TAG, "SPI SRAM memory test OK");
        return true;
    }
}

#endif  //#if CONFIG_SPIRAM
