/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*----------------------------------------------------------------------------------------------------
 * Abstraction layer for PSRAM. PSRAM device related registers and MMU/Cache related code should be
 * abstracted to lower layers.
 *
 * When we add more types of external RAM memory, this can be made into a more intelligent dispatcher.
 *----------------------------------------------------------------------------------------------------*/
#include <sys/param.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "esp_heap_caps_init.h"
#include "esp_psram.h"
#include "esp_mmu_map.h"
#include "hal/mmu_hal.h"
#include "hal/mmu_ll.h"
#include "hal/cache_ll.h"
#include "hal/efuse_hal.h"
#include "soc/soc_caps.h"
#include "esp_private/esp_psram_io.h"
#include "esp_private/esp_psram_extram.h"
#include "esp_private/esp_mmu_map_private.h"
#include "esp_private/esp_psram_impl.h"
#include "esp_private/esp_psram_mspi.h"
#include "esp_private/startup_internal.h"
#if SOC_SPIRAM_XIP_SUPPORTED
#include "esp_private/mmu_psram_flash.h"
#endif
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/himem.h"
#include "esp32/rom/cache.h"
#include "esp_private/esp_cache_esp32_private.h"
#endif

#if CONFIG_IDF_TARGET_ESP32
#define MMU_PAGE_SIZE                   0x8000
#else
#define MMU_PAGE_SIZE                   CONFIG_MMU_PAGE_SIZE
#endif
#define MMU_PAGE_TO_BYTES(page_id)      ((page_id) * MMU_PAGE_SIZE)
#define BYTES_TO_MMU_PAGE(bytes)        ((bytes) / MMU_PAGE_SIZE)

/**
 * Two types of PSRAM memory regions for now:
 * - 8bit aligned
 * - 32bit aligned
 */
#define PSRAM_MEM_TYPE_NUM          2
#define PSRAM_MEM_8BIT_ALIGNED      0
#define PSRAM_MEM_32BIT_ALIGNED     1

#if CONFIG_SPIRAM_FLASH_LOAD_TO_PSRAM
#define PSRAM_EARLY_LOGI   ESP_DRAM_LOGI
#else
#define PSRAM_EARLY_LOGI   ESP_EARLY_LOGI
#endif

#if CONFIG_SPIRAM_RODATA
extern uint8_t _rodata_reserved_end;
#endif /* CONFIG_SPIRAM_RODATA */

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
extern uint8_t _instruction_reserved_end;
#endif /* CONFIG_SPIRAM_FETCH_INSTRUCTIONS */

#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
extern uint8_t _ext_ram_bss_start;
extern uint8_t _ext_ram_bss_end;
#endif //#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY

#if CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY
extern uint8_t _ext_ram_noinit_start;
extern uint8_t _ext_ram_noinit_end;
#endif  //#if CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY

#define ALIGN_UP_BY(num, align) (((num) + ((align) - 1)) & ~((align) - 1))

typedef struct {
    intptr_t vaddr_start;
    intptr_t vaddr_end;
    size_t size;        //in bytes
} psram_mem_t;

typedef struct {
    bool is_chip_initialised;   // if psram hardware is initialised or not
    bool is_initialised;        // if psram initialised with memory mapping or not and is ready to use
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
static const DRAM_ATTR char TAG[] = "esp_psram";

ESP_SYSTEM_INIT_FN(psram_core_stage_init, CORE, BIT(0), 103)
{
    esp_err_t ret = ESP_FAIL;

#if CONFIG_SPIRAM_BOOT_INIT && (CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MALLOC)

#if (CONFIG_IDF_TARGET_ESP32C5 && CONFIG_ESP32C5_REV_MIN_FULL <= 100) || (CONFIG_IDF_TARGET_ESP32C61 && CONFIG_ESP32C61_REV_MIN_FULL <= 100)
    if (efuse_hal_chip_revision() <= 100) {
        ESP_EARLY_LOGW(TAG, "Due to hardware issue on ESP32-C5/C61 (Rev v1.0), PSRAM contents won't be encrypted (for flash encryption enabled case)");
        ESP_EARLY_LOGW(TAG, "Please avoid using PSRAM for security sensitive data e.g., TLS stack allocations (CONFIG_MBEDTLS_EXTERNAL_MEM_ALLOC)");
    }
#endif
    if (esp_psram_is_initialized()) {
        ret = esp_psram_extram_add_to_heap_allocator();
        if (ret != ESP_OK) {
            ESP_EARLY_LOGE(TAG, "External RAM could not be added to heap!");
            return ret;
        }
#if CONFIG_SPIRAM_USE_MALLOC
        heap_caps_malloc_extmem_enable(CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL);
#endif
    }
#endif

    ret = esp_psram_mspi_register_isr();
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to register PSRAM ISR!");
        return ret;
    }

    ret = esp_psram_mspi_mb_init();
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to initialize PSRAM MSPI memory barrier!");
        return ret;
    }

    return ret;
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

#if CONFIG_IDF_TARGET_ESP32P4 && !CONFIG_ESP32P4_SELECTS_REV_LESS_V3
#include "hal/psram_ctrlr_ll.h"
#include "hal/efuse_hal.h"
static void IRAM_ATTR esp_psram_p4_rev3_workaround(void)
{
    spi_mem_s_dev_t backup_reg = {};
    psram_ctrlr_ll_backup_registers(PSRAM_CTRLR_LL_MSPI_ID_2, &backup_reg);

    __attribute__((unused)) volatile uint32_t val = 0;
    psram_ctrlr_ll_disable_core_err_resp();

    /**
     * this workaround is to have two dummy reads, therefore
     * - map 1 page
     * - read 2 times
     * - delay 1us
     *
     * The mapping will be overwritten by the real mapping in `s_psram_mapping`
     */
    mmu_ll_write_entry(1, 0, 0, MMU_TARGET_PSRAM0);
    val = *(uint32_t *)(0x88000000);
    val = *(uint32_t *)(0x88000080);
    esp_rom_delay_us(1);

    _psram_ctrlr_ll_reset_module_clock(PSRAM_CTRLR_LL_MSPI_ID_2);
    psram_ctrlr_ll_enable_core_err_resp();
    psram_ctrlr_ll_restore_registers(PSRAM_CTRLR_LL_MSPI_ID_2, &backup_reg);
}
#endif

static esp_err_t s_psram_chip_init(void)
{
    if (s_psram_ctx.is_chip_initialised) {
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
    s_psram_ctx.is_chip_initialised = true;

    uint32_t psram_physical_size = 0;
    ret = esp_psram_impl_get_physical_size(&psram_physical_size);
    assert(ret == ESP_OK);

    PSRAM_EARLY_LOGI(TAG, "Found %" PRIu32 "MB PSRAM device", psram_physical_size / (1024 * 1024));
    PSRAM_EARLY_LOGI(TAG, "Speed: %dMHz", CONFIG_SPIRAM_SPEED);
#if CONFIG_IDF_TARGET_ESP32
#if CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    ESP_EARLY_LOGI(TAG, "PSRAM initialized, cache is in normal (1-core) mode.");
#else
    ESP_EARLY_LOGI(TAG, "PSRAM initialized, cache is in low/high (2-core) mode.");
#endif
#endif
    return ESP_OK;
}

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA
static void s_xip_psram_placement(uint32_t *psram_available_size, uint32_t *out_start_page)
{
    __attribute__((unused)) uint32_t total_available_size = *psram_available_size;
    uint32_t available_size = *psram_available_size;
    /**
     * `start_page` is the psram physical address in MMU page size.
     * MMU page size on ESP32S2 is 64KB
     * e.g.: psram physical address 16 is in page 0
     *
     * Here we plan to copy FLASH instructions to psram physical address 0, which is the No.0 page.
     */
    uint32_t start_page = 0;
    uint32_t used_page = 0;
    esp_err_t ret = ESP_FAIL;

    //------------------------------------Copy Flash .rodata to PSRAM-------------------------------------//
#if CONFIG_SPIRAM_RODATA
    ret = mmu_config_psram_rodata_segment(start_page, total_available_size, &used_page);
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "No enough psram memory for rodata!");
        abort();
    }
    start_page += used_page;
    available_size -= MMU_PAGE_TO_BYTES(used_page);
    ESP_EARLY_LOGV(TAG, "after copy .rodata, used page is %d, start_page is %d, available_size is %d B", used_page, start_page, available_size);
#endif  //#if CONFIG_SPIRAM_RODATA

    //------------------------------------Copy Flash .text to PSRAM-------------------------------------//
#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
    ret = mmu_config_psram_text_segment(start_page, total_available_size, &used_page);
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "No enough psram memory for instructon!");
        abort();
    }
    start_page += used_page;
    available_size -= MMU_PAGE_TO_BYTES(used_page);
    ESP_EARLY_LOGV(TAG, "after copy .text, used page is %" PRIu32 ", start_page is %" PRIu32 ", psram_available_size is %" PRIu32 " B", used_page, start_page, psram_available_size);
#endif  //#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS

    *psram_available_size = available_size;
    *out_start_page = start_page;
}
#endif  //#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA

static void s_psram_mapping(uint32_t psram_available_size, uint32_t start_page)
{
    esp_err_t ret = ESP_FAIL;
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
    (void)ret;

#if CONFIG_IDF_TARGET_ESP32
    s_mapping((int)v_start_8bit_aligned, size_to_map);
#else
    uint32_t actual_mapped_len = 0;
#if SOC_MMU_PER_EXT_MEM_TARGET
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
    * After mapping, we DON'T care about the PSRAM PHYSICAL ADDRESS ANYMORE!
    *----------------------------------------------------------------------------*/

    //------------------------------------Configure other sections in PSRAM-------------------------------------//
    uintptr_t ext_section_start = UINTPTR_MAX;
    uintptr_t ext_section_end = 0;

#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
    ext_section_start = (uintptr_t)&_ext_ram_bss_start;
    ext_section_end = (uintptr_t)&_ext_ram_bss_end;
#endif  //#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY

#if CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY
    if ((uintptr_t)&_ext_ram_noinit_start < ext_section_start) {
        ext_section_start = (uintptr_t)&_ext_ram_noinit_start;
    }
    if ((uintptr_t)&_ext_ram_noinit_end > ext_section_end) {
        ext_section_end = (uintptr_t)&_ext_ram_noinit_end;
    }
#endif  //#if CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY

    if ((ext_section_start != UINTPTR_MAX) || (ext_section_end != 0)) {
        assert(ext_section_end >= ext_section_start);
        uint32_t ext_section_size = ext_section_end - ext_section_start;
        ESP_EARLY_LOGV(TAG, "ext_section_size is %" PRIu32, ext_section_size);
        s_psram_ctx.regions_to_heap[PSRAM_MEM_8BIT_ALIGNED].vaddr_start += ext_section_size;
        s_psram_ctx.regions_to_heap[PSRAM_MEM_8BIT_ALIGNED].size -= ext_section_size;
    }

#if CONFIG_IDF_TARGET_ESP32
    s_psram_ctx.regions_to_heap[PSRAM_MEM_8BIT_ALIGNED].size -= esp_himem_reserved_area_size() - 1;
    s_psram_ctx.regions_to_heap[PSRAM_MEM_8BIT_ALIGNED].vaddr_end -= esp_himem_reserved_area_size();
#endif
}

esp_err_t esp_psram_chip_init(void)
{
    return s_psram_chip_init();
}

esp_err_t esp_psram_init(void)
{
    esp_err_t ret = ESP_FAIL;

    if (!s_psram_ctx.is_chip_initialised) {
        ret = esp_psram_chip_init();
        if (ret != ESP_OK) {
            return ret;
        }
    }

#if CONFIG_IDF_TARGET_ESP32P4 && !CONFIG_ESP32P4_SELECTS_REV_LESS_V3
    // This workaround is only needed for P4 rev 300 (3.0.0)
    unsigned chip_revision = efuse_hal_chip_revision();
    if (chip_revision == 300) {
        esp_psram_p4_rev3_workaround();
    }
#endif

    uint32_t psram_available_size = 0;
    ret = esp_psram_impl_get_available_size(&psram_available_size);
    assert(ret == ESP_OK);

    /**
     * `start_page` is the psram physical address in MMU page size.
     * MMU page size on ESP32S2 is 64KB
     * e.g.: psram physical address 16 is in page 0
     *
     * Here we plan to copy FLASH instructions to psram physical address 0, which is the No.0 page.
     */
    __attribute__((unused)) uint32_t start_page = 0;

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA
#if (CONFIG_IDF_TARGET_ESP32C5 && CONFIG_ESP32C5_REV_MIN_FULL <= 100) || (CONFIG_IDF_TARGET_ESP32C61 && CONFIG_ESP32C61_REV_MIN_FULL <= 100)
    if (efuse_hal_chip_revision() <= 100) {
        ESP_EARLY_LOGW(TAG, "Due to hardware issue on ESP32-C5/C61 (Rev v1.0), PSRAM contents won't be encrypted (for flash encryption enabled case)");
        ESP_EARLY_LOGW(TAG, "Please avoid using PSRAM for execution as the code/rodata shall be copied as plaintext and this could pose a security risk.");
    }
#endif
    s_xip_psram_placement(&psram_available_size, &start_page);
#endif

    s_psram_mapping(psram_available_size, start_page);

    //will be removed, TODO: IDF-6944
#if CONFIG_IDF_TARGET_ESP32
    cache_driver_t drv = {
        NULL,
        esp_psram_extram_writeback_cache,
    };
    cache_register_writeback(&drv);
#endif

    s_psram_ctx.is_initialised = true;
    return ESP_OK;
}

esp_err_t esp_psram_extram_add_to_heap_allocator(void)
{
    esp_err_t ret = ESP_FAIL;

    uint32_t byte_aligned_caps[] = {MALLOC_CAP_SPIRAM | MALLOC_CAP_DEFAULT, 0, MALLOC_CAP_8BIT | MALLOC_CAP_32BIT | MALLOC_CAP_SIMD};
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

    // To allow using the page alignment gaps created while mapping the flash segments,
    // the alignment gaps must be configured with correct memory protection configurations.
#if CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION
    // Here, SOC_MMU_DI_VADDR_SHARED is necessary because, for the targets that have separate data and instruction virtual address spaces,
    // the SPIRAM gap created due to the alignment needed while placing the instruction segment in the instruction virtual address space
    // cannot be added in heap because the region cannot be configured with write permissions.
#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS && SOC_MMU_DI_VADDR_SHARED
    if ((uint32_t)&_instruction_reserved_end & (CONFIG_MMU_PAGE_SIZE - 1)) {
        uint32_t instruction_alignment_gap_heap_start, instruction_alignment_gap_heap_end;
        mmu_psram_get_instruction_alignment_gap_info(&instruction_alignment_gap_heap_start, &instruction_alignment_gap_heap_end);
        ret = heap_caps_add_region_with_caps(byte_aligned_caps, instruction_alignment_gap_heap_start, instruction_alignment_gap_heap_end);
        if (ret == ESP_OK) {
            ESP_EARLY_LOGI(TAG, "Adding pool of %dK of PSRAM memory gap generated due to end address alignment of irom to the heap allocator", (instruction_alignment_gap_heap_end - instruction_alignment_gap_heap_start) / 1024);
        }
    }
#endif /* CONFIG_SPIRAM_FETCH_INSTRUCTIONS */

    // In the case of ESP32S2, the rodata is mapped to a read-only region (SOC_DROM0_ADDRESS_LOW - SOC_DROM0_ADDRESS_HIGH), thus we cannot add this region to the heap.
#if CONFIG_SPIRAM_RODATA && !CONFIG_IDF_TARGET_ESP32S2
    if ((uint32_t)&_rodata_reserved_end & (CONFIG_MMU_PAGE_SIZE - 1)) {
        uint32_t rodata_alignment_gap_heap_start, rodata_alignment_gap_heap_end;
        mmu_psram_get_rodata_alignment_gap_info(&rodata_alignment_gap_heap_start, &rodata_alignment_gap_heap_end);
        ret = heap_caps_add_region_with_caps(byte_aligned_caps, rodata_alignment_gap_heap_start, rodata_alignment_gap_heap_end);
        if (ret == ESP_OK) {
            ESP_EARLY_LOGI(TAG, "Adding pool of %dK of PSRAM memory gap generated due to end address alignment of drom to the heap allocator", (rodata_alignment_gap_heap_end - rodata_alignment_gap_heap_start) / 1024);
        }
    }
#endif /* CONFIG_SPIRAM_RODATA */
#endif /* CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION */

    return ESP_OK;
}

bool IRAM_ATTR esp_psram_check_ptr_addr(const void *p)
{
    if (!s_psram_ctx.is_initialised) {
        return false;
    }

    if (((intptr_t)p >= s_psram_ctx.mapped_regions[PSRAM_MEM_8BIT_ALIGNED].vaddr_start && (intptr_t)p < s_psram_ctx.mapped_regions[PSRAM_MEM_8BIT_ALIGNED].vaddr_end) ||
            ((intptr_t)p >= s_psram_ctx.mapped_regions[PSRAM_MEM_32BIT_ALIGNED].vaddr_start && (intptr_t)p < s_psram_ctx.mapped_regions[PSRAM_MEM_32BIT_ALIGNED].vaddr_end)) {
        return true;
    }

#if CONFIG_SPIRAM_RODATA
    if (mmu_psram_check_ptr_addr_in_xip_psram_rodata_region(p)) {
        return true;
    }
#endif

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
    if (mmu_psram_check_ptr_addr_in_xip_psram_instruction_region(p)) {
        return true;
    }
#endif

    return false;
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

void esp_psram_bss_init(void)
{
#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
    size_t size = (&_ext_ram_bss_end - &_ext_ram_bss_start) * sizeof(_ext_ram_bss_start);
    memset(&_ext_ram_bss_start, 0, size);
#endif
}

#if CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION
static inline uint32_t s_get_ext_bss_size(void)
{
#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
    return ((intptr_t)&_ext_ram_bss_end - (intptr_t)&_ext_ram_bss_start);
#else
    return 0;
#endif /* CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY */
}

static inline uint32_t s_get_ext_noinit_size(void)
{
#if CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY
    return ((intptr_t)&_ext_ram_noinit_end - (intptr_t)&_ext_ram_noinit_start);
#else
    return 0;
#endif /* CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY */
}

/**
 * @brief Calculates the effective PSRAM memory that would be / is mapped.
 *
 * @return The size of PSRAM memory that would be / is mapped in bytes, or 0 if PSRAM isn't successfully initialized
 */
static size_t esp_psram_get_effective_mapped_size(void)
{
    size_t byte_aligned_size = 0;
    size_t total_mapped_size = 0;

    // return if the PSRAM is not enabled
    if (!s_psram_ctx.is_chip_initialised) {
        return 0;
    }

    if (s_psram_ctx.is_initialised) {
        return s_psram_ctx.mapped_regions[PSRAM_MEM_8BIT_ALIGNED].size + s_psram_ctx.mapped_regions[PSRAM_MEM_32BIT_ALIGNED].size;
    } else {
        uint32_t psram_available_size = 0;
        esp_err_t ret = esp_psram_impl_get_available_size(&psram_available_size);
        assert(ret == ESP_OK);
        (void)ret;

#if CONFIG_SPIRAM_RODATA
        psram_available_size -= mmu_psram_get_rodata_segment_length();
#endif /* CONFIG_SPIRAM_RODATA */

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
        psram_available_size -= mmu_psram_get_text_segment_length();
#endif /* CONFIG_SPIRAM_FETCH_INSTRUCTIONS */

        ret = esp_mmu_map_get_max_consecutive_free_block_size(MMU_MEM_CAP_READ | MMU_MEM_CAP_WRITE | MMU_MEM_CAP_8BIT | MMU_MEM_CAP_32BIT, MMU_TARGET_PSRAM0, &byte_aligned_size);
        assert(ret == ESP_OK);
        total_mapped_size += MIN(byte_aligned_size, psram_available_size - total_mapped_size);

#if CONFIG_IDF_TARGET_ESP32S2
        if (total_mapped_size < psram_available_size) {
            size_t word_aligned_size = 0;
            ret = esp_mmu_map_get_max_consecutive_free_block_size(MMU_MEM_CAP_READ | MMU_MEM_CAP_WRITE | MMU_MEM_CAP_32BIT, MMU_TARGET_PSRAM0,  &word_aligned_size);
            assert(ret == ESP_OK);
            total_mapped_size += MIN(word_aligned_size, psram_available_size - total_mapped_size);
        }
#endif
        return total_mapped_size;
    }
}

size_t esp_psram_get_heap_size_to_protect(void)
{
    // return if the PSRAM is not enabled
    if (!s_psram_ctx.is_chip_initialised) {
        return 0;
    }

    if (s_psram_ctx.is_initialised) {
        return s_psram_ctx.regions_to_heap[PSRAM_MEM_8BIT_ALIGNED].size + s_psram_ctx.regions_to_heap[PSRAM_MEM_32BIT_ALIGNED].size;
    } else {
        size_t effective_mapped_size = esp_psram_get_effective_mapped_size();
        if (effective_mapped_size == 0) {
            return 0;
        }

        effective_mapped_size -= s_get_ext_bss_size();
        effective_mapped_size -= s_get_ext_noinit_size();

#if CONFIG_IDF_TARGET_ESP32
        effective_mapped_size -= esp_himem_reserved_area_size() - 1;
#endif
        return effective_mapped_size;
    }
}
#endif /* CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION */
