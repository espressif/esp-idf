/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This file will be redesigned into MMU driver, to maintain all the external
 * memory contexts including:
 * - Flash
 * - PSRAM
 * - DDR
 *
 * Now only MMU-PSRAM related private APIs
 */

#include <stdint.h>
#include <string.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_check.h"
#include "soc/soc_caps.h"
#include "ext_mem_layout.h"
#include "freertos/FreeRTOS.h"
#include "hal/cache_types.h"
#include "hal/cache_ll.h"
#include "hal/mmu_types.h"
#include "hal/mmu_ll.h"
#include "mmu.h"


#define ALIGN_UP_BY(num, align) (((num) + ((align) - 1)) & ~((align) - 1))
#define MMU_PAGE_SIZE           CONFIG_MMU_PAGE_SIZE

//This flag indicates the memory region is merged, we don't care about it anymore
#define MEM_REGION_MERGED             -1

static const char *TAG = "mmu";
extern int _instruction_reserved_start;
extern int _instruction_reserved_end;
extern int _rodata_reserved_start;
extern int _rodata_reserved_end;

typedef struct mmu_linear_mem_ {
    cache_bus_mask_t bus_id;
    intptr_t start;
    intptr_t end;
    size_t pool_size;
    intptr_t free_head;
    size_t free_size;
    int caps;
} mmu_linear_mem_t;

typedef struct {
    /**
     * number of memory regions that are available, after coalescing, this number should be smaller than or equal to `SOC_MMU_LINEAR_ADDRESS_REGION_NUM`
     */
    uint32_t num_regions;
    /**
     * This saves the available MMU linear address regions,
     * after reserving flash .rodata and .text, and after coalescing.
     * Only the first `num_regions` items are valid
     */
    mmu_linear_mem_t mem_regions[SOC_MMU_LINEAR_ADDRESS_REGION_NUM];
} mmu_ctx_t;

static mmu_ctx_t s_mmu_ctx;


static void s_reserve_irom_region(mmu_linear_mem_t *hw_mem_regions, int region_nums)
{
    /**
     * We follow the way how 1st bootloader load flash .text:
     *
     * - Now IBUS addresses (between `_instruction_reserved_start` and `_instruction_reserved_end`) are consecutive on all chips,
     *   we strongly rely on this to calculate the .text length
     */
    size_t irom_len_to_reserve = (uint32_t)&_instruction_reserved_end - (uint32_t)&_instruction_reserved_start;
    assert((mmu_ll_vaddr_to_laddr((uint32_t)&_instruction_reserved_end) - mmu_ll_vaddr_to_laddr((uint32_t)&_instruction_reserved_start)) == irom_len_to_reserve);

    irom_len_to_reserve = ALIGN_UP_BY(irom_len_to_reserve, MMU_PAGE_SIZE);
    cache_bus_mask_t bus_mask = cache_ll_l1_get_bus(0, (uint32_t)&_instruction_reserved_start, irom_len_to_reserve);

    for (int i = 0; i < SOC_MMU_LINEAR_ADDRESS_REGION_NUM; i++) {
        if (bus_mask & hw_mem_regions[i].bus_id) {
            if (hw_mem_regions[i].pool_size <= irom_len_to_reserve) {
                hw_mem_regions[i].free_head = hw_mem_regions[i].end;
                hw_mem_regions[i].free_size = 0;
                irom_len_to_reserve -= hw_mem_regions[i].pool_size;
            } else {
                hw_mem_regions[i].free_head = hw_mem_regions[i].free_head + irom_len_to_reserve;
                hw_mem_regions[i].free_size -= irom_len_to_reserve;
            }
        }
    }
}

static void s_reserve_drom_region(mmu_linear_mem_t *hw_mem_regions, int region_nums)
{
    /**
     * Similarly, we follow the way how 1st bootloader load flash .rodata:
     */
    size_t drom_len_to_reserve = (uint32_t)&_rodata_reserved_end - (uint32_t)&_rodata_reserved_start;
    assert((mmu_ll_vaddr_to_laddr((uint32_t)&_rodata_reserved_end) - mmu_ll_vaddr_to_laddr((uint32_t)&_rodata_reserved_start)) == drom_len_to_reserve);

    drom_len_to_reserve = ALIGN_UP_BY(drom_len_to_reserve, MMU_PAGE_SIZE);
    cache_bus_mask_t bus_mask = cache_ll_l1_get_bus(0, (uint32_t)&_rodata_reserved_start, drom_len_to_reserve);

    for (int i = 0; i < SOC_MMU_LINEAR_ADDRESS_REGION_NUM; i++) {
        if (bus_mask & hw_mem_regions[i].bus_id) {
            if (hw_mem_regions[i].pool_size <= drom_len_to_reserve) {
                hw_mem_regions[i].free_head = hw_mem_regions[i].end;
                hw_mem_regions[i].free_size = 0;
                drom_len_to_reserve -= hw_mem_regions[i].pool_size;
            } else {
                hw_mem_regions[i].free_head = hw_mem_regions[i].free_head + drom_len_to_reserve;
                hw_mem_regions[i].free_size -= drom_len_to_reserve;
            }
        }
    }
}

void esp_mmu_init(void)
{
    mmu_linear_mem_t hw_mem_regions[SOC_MMU_LINEAR_ADDRESS_REGION_NUM] = {};

    for (int i = 0; i < SOC_MMU_LINEAR_ADDRESS_REGION_NUM; i++) {
        hw_mem_regions[i].start = g_mmu_mem_regions[i].start;
        hw_mem_regions[i].end = g_mmu_mem_regions[i].end;
        hw_mem_regions[i].pool_size = g_mmu_mem_regions[i].size;
        hw_mem_regions[i].free_size = g_mmu_mem_regions[i].size;
        hw_mem_regions[i].free_head = g_mmu_mem_regions[i].start;
        hw_mem_regions[i].bus_id = g_mmu_mem_regions[i].bus_id;
        hw_mem_regions[i].caps = g_mmu_mem_regions[i].caps;
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
        assert(__builtin_popcount(hw_mem_regions[i].bus_id) == 1);
#endif
        assert(hw_mem_regions[i].pool_size % MMU_PAGE_SIZE == 0);
    }

    //First reserve memory regions used for irom and drom, as we must follow the way how 1st bootloader load them
    s_reserve_irom_region(hw_mem_regions, SOC_MMU_LINEAR_ADDRESS_REGION_NUM);
    s_reserve_drom_region(hw_mem_regions, SOC_MMU_LINEAR_ADDRESS_REGION_NUM);

    if (SOC_MMU_LINEAR_ADDRESS_REGION_NUM > 1) {
        //Now we can coalesce adjacent regions
        for (int i = 1; i < SOC_MMU_LINEAR_ADDRESS_REGION_NUM; i++) {
            mmu_linear_mem_t *a = &hw_mem_regions[i - 1];
            mmu_linear_mem_t *b = &hw_mem_regions[i];
            if ((b->free_head == a->end) && (b->caps == a->caps)) {
                a->caps = MEM_REGION_MERGED;
                b->bus_id |= a->bus_id;
                b->start = a->start;
                b->pool_size += a->pool_size;
                b->free_head = a->free_head;
                b->free_size += a->free_size;
            }
        }
    }

    //Count the mem regions left after coalescing
    uint32_t region_num = 0;
    for (int i = 0; i < SOC_MMU_LINEAR_ADDRESS_REGION_NUM; i++) {
        if(hw_mem_regions[i].caps != MEM_REGION_MERGED) {
            region_num++;
        }
    }
    ESP_EARLY_LOGV(TAG, "after coalescing, %d regions are left", region_num);

    //Initialise `s_mmu_ctx.mem_regions[]`, as we've done all static allocation, to prepare available virtual memory regions
    uint32_t available_region_idx = 0;
    s_mmu_ctx.num_regions = region_num;
    for (int i = 0; i < SOC_MMU_LINEAR_ADDRESS_REGION_NUM; i++) {
        if (hw_mem_regions[i].caps == MEM_REGION_MERGED) {
            continue;
        }

        memcpy(&s_mmu_ctx.mem_regions[available_region_idx], &hw_mem_regions[i], sizeof(mmu_linear_mem_t));
        available_region_idx++;
    }

    assert(available_region_idx == region_num);
}

esp_err_t esp_mmu_get_largest_free_block(int caps, size_t *out_len)
{
    ESP_RETURN_ON_FALSE(out_len, ESP_ERR_INVALID_ARG, TAG, "null pointer");
    if (caps & MMU_MEM_CAP_EXEC) {
        if ((caps & MMU_MEM_CAP_8BIT) || (caps & MMU_MEM_CAP_WRITE)) {
            //None of the executable memory are expected to be 8-bit accessible or writable.
            return ESP_ERR_INVALID_ARG;
        }
    }
    *out_len = 0;

    size_t max = 0;

    for (int i = 0; i < s_mmu_ctx.num_regions; i++) {
        if ((s_mmu_ctx.mem_regions[i].caps & caps) == caps) {
            if (s_mmu_ctx.mem_regions[i].free_size > max) {
                max = s_mmu_ctx.mem_regions[i].free_size;
            }
        }
    }

    *out_len = max;

    return ESP_OK;
}

esp_err_t esp_mmu_find_vaddr_range(size_t size, uint32_t caps, const void **out_ptr)
{
    ESP_RETURN_ON_FALSE(out_ptr, ESP_ERR_INVALID_ARG, TAG, "null pointer");
    if (caps & MMU_MEM_CAP_EXEC) {
        if ((caps & MMU_MEM_CAP_8BIT) || (caps & MMU_MEM_CAP_WRITE)) {
            //None of the executable memory are expected to be 8-bit accessible or writable.
            return ESP_ERR_INVALID_ARG;
        }
        caps |= MMU_MEM_CAP_32BIT;
    }

    size_t aligned_size = ALIGN_UP_BY(size, MMU_PAGE_SIZE);
    bool is_match = false;
    uint32_t laddr = 0;

    for (int i = 0; i < s_mmu_ctx.num_regions; i++) {
        if ((s_mmu_ctx.mem_regions[i].caps & caps) == caps) {
            if (s_mmu_ctx.mem_regions[i].free_size < aligned_size) {
                continue;
            } else {
                laddr = (uint32_t)s_mmu_ctx.mem_regions[i].free_head;
                s_mmu_ctx.mem_regions[i].free_head += aligned_size;
                s_mmu_ctx.mem_regions[i].free_size -= aligned_size;
                is_match = true;
                break;
            }
        }
    }
    ESP_RETURN_ON_FALSE(is_match, ESP_ERR_NOT_FOUND, TAG, "no such vaddr range");
    ESP_EARLY_LOGV(TAG, "found laddr is 0x%x", laddr);

    if (caps & MMU_MEM_CAP_EXEC) {
        laddr = mmu_ll_laddr_to_vaddr(laddr, MMU_VADDR_INSTRUCTION);
    } else {
        laddr = mmu_ll_laddr_to_vaddr(laddr, MMU_VADDR_DATA);
    }
    *out_ptr = (void *)laddr;

    return ESP_OK;
}

esp_err_t esp_mmu_dump_region_usage(void)
{
    for (int i = 0; i < s_mmu_ctx.num_regions; i++) {
        ESP_EARLY_LOGI(TAG, "bus_id: 0x%x", s_mmu_ctx.mem_regions[i].bus_id);
        ESP_EARLY_LOGI(TAG, "start: 0x%x", s_mmu_ctx.mem_regions[i].start);
        ESP_EARLY_LOGI(TAG, "end: 0x%x", s_mmu_ctx.mem_regions[i].end);
        ESP_EARLY_LOGI(TAG, "pool_size: 0x%x", s_mmu_ctx.mem_regions[i].pool_size);
        ESP_EARLY_LOGI(TAG, "free_head: 0x%x", s_mmu_ctx.mem_regions[i].free_head);
        ESP_EARLY_LOGI(TAG, "free_size: 0x%x", s_mmu_ctx.mem_regions[i].free_size);
        ESP_EARLY_LOGI(TAG, "caps: 0x%x\n", s_mmu_ctx.mem_regions[i].caps);
    }

    return ESP_OK;
}
