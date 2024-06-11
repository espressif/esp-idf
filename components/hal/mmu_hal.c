/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <sys/param.h>
#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_attr.h"
#include "hal/assert.h"
#include "hal/mmu_hal.h"
#include "hal/mmu_ll.h"
#include "soc/soc_caps.h"
#include "rom/cache.h"

void mmu_hal_init(void)
{
#if CONFIG_ESP_ROM_RAM_APP_NEEDS_MMU_INIT
    ROM_Boot_Cache_Init();
#endif

    mmu_ll_set_page_size(0, CONFIG_MMU_PAGE_SIZE);
    mmu_hal_unmap_all();
}

void mmu_hal_unmap_all(void)
{
#if SOC_MMU_PER_EXT_MEM_TARGET
    mmu_ll_unmap_all(MMU_LL_FLASH_MMU_ID);
    mmu_ll_unmap_all(MMU_LL_PSRAM_MMU_ID);
#else
    mmu_ll_unmap_all(0);
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    mmu_ll_unmap_all(1);
#endif
#endif
}

uint32_t mmu_hal_pages_to_bytes(uint32_t mmu_id, uint32_t page_num)
{
    mmu_page_size_t page_size = mmu_ll_get_page_size(mmu_id);
    uint32_t shift_code = 0;
    switch (page_size) {
        case MMU_PAGE_64KB:
            shift_code = 16;
            break;
        case MMU_PAGE_32KB:
            shift_code = 15;
            break;
        case MMU_PAGE_16KB:
            shift_code = 14;
            break;
        default:
            HAL_ASSERT(shift_code);
    }
    return page_num << shift_code;
}

uint32_t mmu_hal_bytes_to_pages(uint32_t mmu_id, uint32_t bytes)
{
    mmu_page_size_t page_size = mmu_ll_get_page_size(mmu_id);
    uint32_t shift_code = 0;
    switch (page_size) {
        case MMU_PAGE_64KB:
            shift_code = 16;
            break;
        case MMU_PAGE_32KB:
            shift_code = 15;
            break;
        case MMU_PAGE_16KB:
            shift_code = 14;
            break;
        default:
            HAL_ASSERT(shift_code);
    }
    return bytes >> shift_code;
}

void mmu_hal_map_region(uint32_t mmu_id, mmu_target_t mem_type, uint32_t vaddr, uint32_t paddr, uint32_t len, uint32_t *out_len)
{
    uint32_t page_size_in_bytes = mmu_hal_pages_to_bytes(mmu_id, 1);
    HAL_ASSERT(vaddr % page_size_in_bytes == 0);
    HAL_ASSERT(paddr % page_size_in_bytes == 0);
    HAL_ASSERT(mmu_ll_check_valid_paddr_region(mmu_id, paddr, len));
    HAL_ASSERT(mmu_hal_check_valid_ext_vaddr_region(mmu_id, vaddr, len, MMU_VADDR_DATA | MMU_VADDR_INSTRUCTION));

    uint32_t page_num = (len + page_size_in_bytes - 1) / page_size_in_bytes;
    uint32_t entry_id = 0;
    uint32_t mmu_val;     //This is the physical address in the format that MMU supported

    *out_len = mmu_hal_pages_to_bytes(mmu_id, page_num);
    mmu_val = mmu_ll_format_paddr(mmu_id, paddr, mem_type);

    while (page_num) {
        entry_id = mmu_ll_get_entry_id(mmu_id, vaddr);
        mmu_ll_write_entry(mmu_id, entry_id, mmu_val, mem_type);
        vaddr += page_size_in_bytes;
        mmu_val++;
        page_num--;
    }
}

void mmu_hal_unmap_region(uint32_t mmu_id, uint32_t vaddr, uint32_t len)
{
    uint32_t page_size_in_bytes = mmu_hal_pages_to_bytes(mmu_id, 1);
    HAL_ASSERT(vaddr % page_size_in_bytes == 0);
    HAL_ASSERT(mmu_hal_check_valid_ext_vaddr_region(mmu_id, vaddr, len, MMU_VADDR_DATA | MMU_VADDR_INSTRUCTION));

    uint32_t page_num = (len + page_size_in_bytes - 1) / page_size_in_bytes;
    uint32_t entry_id = 0;
    while (page_num) {
        entry_id = mmu_ll_get_entry_id(mmu_id, vaddr);
        mmu_ll_set_entry_invalid(mmu_id, entry_id);
        vaddr += page_size_in_bytes;
        page_num--;
    }
}

bool mmu_hal_vaddr_to_paddr(uint32_t mmu_id, uint32_t vaddr, uint32_t *out_paddr, mmu_target_t *out_target)
{
    HAL_ASSERT(mmu_hal_check_valid_ext_vaddr_region(mmu_id, vaddr, 1, MMU_VADDR_DATA | MMU_VADDR_INSTRUCTION));
    uint32_t entry_id = mmu_ll_get_entry_id(mmu_id, vaddr);
    if (!mmu_ll_check_entry_valid(mmu_id, entry_id)) {
        return false;
    }

    uint32_t page_size_in_bytes = mmu_hal_pages_to_bytes(mmu_id, 1);
    uint32_t offset = (uint32_t)vaddr % page_size_in_bytes;

    *out_target = mmu_ll_get_entry_target(mmu_id, entry_id);
    uint32_t paddr_base = mmu_ll_entry_id_to_paddr_base(mmu_id, entry_id);
    *out_paddr = paddr_base | offset;

    return true;
}

bool mmu_hal_paddr_to_vaddr(uint32_t mmu_id, uint32_t paddr, mmu_target_t target, mmu_vaddr_t type, uint32_t *out_vaddr)
{
    HAL_ASSERT(mmu_ll_check_valid_paddr_region(mmu_id, paddr, 1));

    uint32_t mmu_val = mmu_ll_format_paddr(mmu_id, paddr, target);
    int entry_id = mmu_ll_find_entry_id_based_on_map_value(mmu_id, mmu_val, target);
    if (entry_id == -1) {
        return false;
    }

    uint32_t page_size_in_bytes = mmu_hal_pages_to_bytes(mmu_id, 1);
    uint32_t offset = paddr % page_size_in_bytes;
    uint32_t vaddr_base = mmu_ll_entry_id_to_vaddr_base(mmu_id, entry_id, type);
    if (vaddr_base == 0) {
        return false;
    }
    *out_vaddr = vaddr_base | offset;

    return true;
}

bool mmu_hal_check_valid_ext_vaddr_region(uint32_t mmu_id, uint32_t vaddr_start, uint32_t len, mmu_vaddr_t type)
{
    return mmu_ll_check_valid_ext_vaddr_region(mmu_id, vaddr_start, len, type);
}
