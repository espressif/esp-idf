/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief
 * This is for P4 and future chips with similar arch.
 * The XIP PSRAM is done by CPU copy, v1(see mmu_psram_flash.c) is done by Cache copy
 */

#include <sys/param.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "soc/ext_mem_defs.h"
#include "hal/mmu_hal.h"
#include "hal/mmu_ll.h"
#include "hal/cache_hal.h"
#include "esp_private/mmu_psram_flash.h"
#include "esp_mmu_map.h"
#include "esp_heap_caps.h"
#include "esp_private/image_process.h"

#define ALIGN_UP_BY(num, align) (((num) + ((align) - 1)) & ~((align) - 1))
#define ALIGN_DOWN_BY(num, align) ((num) & (~((align) - 1)))

/**
 * If using `int`, then for CLANG, with enabled optimization when inlined function is provided with the address of external symbol, the two least bits of the constant used inside that function get cleared.
 * Optimizer assumes that address of external symbol should be aligned to 4-bytes and therefore aligns constant value used for bitwise AND operation with that address.
 *
 * This means `extern int _instruction_reserved_start;` can be unaligned to 4 bytes, whereas using `char` can solve this issue.
 *
 * As we only use these symbol address, we declare them as `char` here
 */
extern char _instruction_reserved_start;
extern char _instruction_reserved_end;
extern char _rodata_reserved_start;
extern char _rodata_reserved_end;

const static char *TAG = "mmu_psram";
static uint32_t s_irom_vaddr_start;
static uint32_t s_drom_vaddr_start;
static size_t s_irom_size;
static size_t s_drom_size;
static int s_irom_paddr_offset;
static int s_drom_paddr_offset;

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA
static uint32_t s_do_load_from_flash(uint32_t flash_paddr_start, uint32_t size, uint32_t target_vaddr_start, uint32_t target_paddr_start)
{
    uint32_t flash_end_page_vaddr = SOC_DRAM_FLASH_ADDRESS_HIGH - CONFIG_MMU_PAGE_SIZE;
    ESP_EARLY_LOGV(TAG, "flash_paddr_start: 0x%"PRIx32", flash_end_page_vaddr: 0x%"PRIx32", size: 0x%"PRIx32", target_vaddr_start: 0x%"PRIx32, flash_paddr_start, flash_end_page_vaddr, size, target_vaddr_start);
    assert((flash_paddr_start % CONFIG_MMU_PAGE_SIZE) == 0);
    assert((flash_end_page_vaddr % CONFIG_MMU_PAGE_SIZE) == 0);
    assert((target_vaddr_start % CONFIG_MMU_PAGE_SIZE) == 0);

    uint32_t mapped_size = 0;
    while (mapped_size < size) {
        uint32_t actual_mapped_len = 0;
        mmu_hal_map_region(MMU_LL_PSRAM_MMU_ID, MMU_TARGET_PSRAM0, target_vaddr_start, target_paddr_start + mapped_size, CONFIG_MMU_PAGE_SIZE, &actual_mapped_len);
        assert(actual_mapped_len == CONFIG_MMU_PAGE_SIZE);

        mmu_hal_map_region(MMU_LL_FLASH_MMU_ID, MMU_TARGET_FLASH0, flash_end_page_vaddr, flash_paddr_start + mapped_size, CONFIG_MMU_PAGE_SIZE, &actual_mapped_len);
        assert(actual_mapped_len == CONFIG_MMU_PAGE_SIZE);

        cache_hal_invalidate_addr(target_vaddr_start, CONFIG_MMU_PAGE_SIZE);
        cache_hal_invalidate_addr(flash_end_page_vaddr, CONFIG_MMU_PAGE_SIZE);
        memcpy((void *)target_vaddr_start, (void *)flash_end_page_vaddr, CONFIG_MMU_PAGE_SIZE);

        ESP_EARLY_LOGV(TAG, "target_vaddr_start: 0x%"PRIx32, target_vaddr_start);
        mapped_size += CONFIG_MMU_PAGE_SIZE;
        target_vaddr_start += CONFIG_MMU_PAGE_SIZE;
    }

    ESP_EARLY_LOGV(TAG, "mapped_size: 0x%"PRIx32, mapped_size);
    assert(mapped_size == ALIGN_UP_BY(size, CONFIG_MMU_PAGE_SIZE));

    return mapped_size;
}
#endif //#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
esp_err_t mmu_config_psram_text_segment(uint32_t start_page, uint32_t psram_size, uint32_t *out_page)
{
    size_t irom_size = ALIGN_UP_BY((uint32_t)&_instruction_reserved_end, CONFIG_MMU_PAGE_SIZE) - ALIGN_DOWN_BY((uint32_t)&_instruction_reserved_start, CONFIG_MMU_PAGE_SIZE);
    s_irom_size = irom_size;

    uint32_t flash_drom_paddr_start = 0;
    uint32_t flash_irom_paddr_start = 0;
    image_process_get_flash_segments_info(&flash_drom_paddr_start, &flash_irom_paddr_start);
    flash_irom_paddr_start = ALIGN_DOWN_BY(flash_irom_paddr_start, CONFIG_MMU_PAGE_SIZE);
    ESP_EARLY_LOGV(TAG, "flash_irom_paddr_start: 0x%x", flash_irom_paddr_start);

    if ((MMU_PAGE_TO_BYTES(start_page) + irom_size) > psram_size) {
        ESP_EARLY_LOGE(TAG, "PSRAM space not enough for the Flash instructions, need %"PRId32" B, from %"PRId32" B to %"PRId32" B", irom_size, MMU_PAGE_TO_BYTES(start_page), MMU_PAGE_TO_BYTES(start_page) + irom_size);
        return ESP_ERR_NO_MEM;
    }

    uint32_t irom_load_addr_aligned = ALIGN_DOWN_BY((uint32_t)&_instruction_reserved_start, CONFIG_MMU_PAGE_SIZE);
    s_irom_paddr_offset = flash_irom_paddr_start - MMU_PAGE_TO_BYTES(start_page);
    s_irom_vaddr_start = irom_load_addr_aligned;
    ESP_EARLY_LOGV(TAG, "flash_irom_paddr_start: 0x%"PRIx32", MMU_PAGE_TO_BYTES(start_page): 0x%"PRIx32", s_irom_paddr_offset: 0x%"PRIx32", s_irom_vaddr_start: 0x%"PRIx32, flash_irom_paddr_start, MMU_PAGE_TO_BYTES(start_page), s_irom_paddr_offset, s_irom_vaddr_start);

    uint32_t mapped_size = 0;
    mapped_size = s_do_load_from_flash(flash_irom_paddr_start, irom_size, irom_load_addr_aligned, MMU_PAGE_TO_BYTES(start_page));
    cache_hal_writeback_addr(irom_load_addr_aligned, irom_size);

    ESP_EARLY_LOGV(TAG, "after mapping text, starting from paddr=0x%08"PRIx32" and vaddr=0x%08"PRIx32", 0x%"PRIx32" bytes are mapped", MMU_PAGE_TO_BYTES(start_page), irom_load_addr_aligned, mapped_size);

    start_page += BYTES_TO_MMU_PAGE(irom_size);
    *out_page = start_page;

    ESP_EARLY_LOGI(TAG, ".text xip on psram");
    return ESP_OK;
}
#endif  //#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS

#if CONFIG_SPIRAM_RODATA
esp_err_t mmu_config_psram_rodata_segment(uint32_t start_page, uint32_t psram_size, uint32_t *out_page)
{
    size_t drom_size = ALIGN_UP_BY((uint32_t)&_rodata_reserved_end, CONFIG_MMU_PAGE_SIZE) - ALIGN_DOWN_BY((uint32_t)&_rodata_reserved_start, CONFIG_MMU_PAGE_SIZE);
    s_drom_size = drom_size;

    uint32_t flash_drom_paddr_start = 0;
    uint32_t flash_irom_paddr_start = 0;
    image_process_get_flash_segments_info(&flash_drom_paddr_start, &flash_irom_paddr_start);
    flash_drom_paddr_start = ALIGN_DOWN_BY(flash_drom_paddr_start, CONFIG_MMU_PAGE_SIZE);
    ESP_EARLY_LOGV(TAG, "flash_drom_paddr_start: 0x%x", flash_drom_paddr_start);

    if ((MMU_PAGE_TO_BYTES(start_page) + drom_size) > psram_size) {
        ESP_EARLY_LOGE(TAG, "PSRAM space not enough for the Flash rodata, need %"PRId32" B, from %"PRId32" B to %"PRId32" B", drom_size, MMU_PAGE_TO_BYTES(start_page), MMU_PAGE_TO_BYTES(start_page) + drom_size);
        return ESP_ERR_NO_MEM;
    }

    uint32_t drom_load_addr_aligned = ALIGN_DOWN_BY((uint32_t)&_rodata_reserved_start, CONFIG_MMU_PAGE_SIZE);
    s_drom_paddr_offset = flash_drom_paddr_start - MMU_PAGE_TO_BYTES(start_page);
    s_drom_vaddr_start = drom_load_addr_aligned;
    ESP_EARLY_LOGV(TAG, "flash_drom_paddr_start: 0x%"PRIx32", MMU_PAGE_TO_BYTES(start_page): 0x%"PRIx32", s_drom_paddr_offset: 0x%"PRIx32", s_drom_vaddr_start: 0x%"PRIx32, flash_drom_paddr_start, MMU_PAGE_TO_BYTES(start_page), s_drom_paddr_offset, s_drom_vaddr_start);

    uint32_t mapped_size = 0;
    mapped_size = s_do_load_from_flash(flash_drom_paddr_start, drom_size, drom_load_addr_aligned, MMU_PAGE_TO_BYTES(start_page));
    cache_hal_writeback_addr(drom_load_addr_aligned, drom_size);

    ESP_EARLY_LOGV(TAG, "after mapping rodata, starting from paddr=0x%08"PRIx32" and vaddr=0x%08"PRIx32", 0x%"PRIx32" bytes are mapped", MMU_PAGE_TO_BYTES(start_page), drom_load_addr_aligned, mapped_size);

    start_page += BYTES_TO_MMU_PAGE(drom_size);
    *out_page = start_page;

    ESP_EARLY_LOGI(TAG, ".rodata xip on psram");
    return ESP_OK;
}
#endif  //#if CONFIG_SPIRAM_RODATA

size_t mmu_xip_psram_flash_vaddr_to_paddr(const void *ptr)
{
    if (ptr == NULL) {
        return UINT32_MAX;
    }

    size_t paddr_on_flash = 0;
    uint32_t psram_paddr = 0;
    mmu_target_t target = MMU_TARGET_FLASH0;

    if ((uint32_t)ptr >= s_irom_vaddr_start && (uint32_t)ptr < (s_irom_vaddr_start + s_irom_size)) {
        bool is_mapped = mmu_hal_vaddr_to_paddr(MMU_LL_PSRAM_MMU_ID, (uint32_t)ptr, &psram_paddr, &target);
        assert(is_mapped);
        assert(target == MMU_TARGET_PSRAM0);
        paddr_on_flash = psram_paddr + s_irom_paddr_offset;
    } else if ((uint32_t)ptr >= s_drom_vaddr_start && (uint32_t)ptr < (s_drom_vaddr_start + s_drom_size)) {
        bool is_mapped = mmu_hal_vaddr_to_paddr(MMU_LL_PSRAM_MMU_ID, (uint32_t)ptr, &psram_paddr, &target);
        assert(is_mapped);
        assert(target == MMU_TARGET_PSRAM0);
        paddr_on_flash = psram_paddr + s_drom_paddr_offset;
    } else {
        paddr_on_flash = UINT32_MAX;
    }

    return paddr_on_flash;
}
