/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
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

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/cache.h"
#include "soc/dport_reg.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/cache.h"
#endif

void mmu_hal_init(void)
{
    mmu_ll_unmap_all(0);
#if !CONFIG_FREERTOS_UNICORE
    mmu_ll_unmap_all(1);
#endif
}

#if !CONFIG_IDF_TARGET_ESP32
//If decided, add a jira ticket for implementing these APIs on ESP32
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
    HAL_ASSERT((paddr + len - 1) < mmu_hal_pages_to_bytes(mmu_id, MMU_MAX_PADDR_PAGE_NUM));
    HAL_ASSERT(mmu_ll_check_valid_ext_vaddr_region(mmu_id, vaddr, len));

    uint32_t page_num = (len + page_size_in_bytes - 1) / page_size_in_bytes;
    uint32_t entry_id = 0;
    uint32_t mmu_val;     //This is the physical address in the format that MMU supported

    *out_len = mmu_hal_pages_to_bytes(mmu_id, page_num);
    mmu_val = mmu_ll_format_paddr(mmu_id, paddr);

    while (page_num) {
        entry_id = mmu_ll_get_entry_id(mmu_id, vaddr);
        mmu_ll_write_entry(mmu_id, entry_id, mmu_val, mem_type);
        vaddr += page_size_in_bytes;
        mmu_val++;
        page_num--;
    }
}
#endif //#if !CONFIG_IDF_TARGET_ESP32
