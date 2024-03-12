/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <sys/param.h>
#include <sys/queue.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"

#include "soc/soc_caps.h"
#include "hal/cache_types.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "hal/mmu_types.h"
#include "hal/mmu_hal.h"
#include "hal/mmu_ll.h"

#include "esp_private/cache_utils.h"
#include "esp_private/esp_cache_esp32_private.h"
#include "esp_private/esp_mmu_map_private.h"
#include "ext_mem_layout.h"
#include "esp_mmu_map.h"

//This is for size align
#define ALIGN_UP_BY(num, align) (((num) + ((align) - 1)) & ~((align) - 1))
//This is for vaddr align
#define ALIGN_DOWN_BY(num, align) ((num) & (~((align) - 1)))

//This flag indicates the memory region is merged, we don't care about it anymore
#define MEM_REGION_MERGED             -1

/**
 * We have some hw related tests for vaddr region capabilites
 * Use this macro to disable paddr check as we need to reuse certain paddr blocks
 */
#define ENABLE_PADDR_CHECK            !ESP_MMAP_TEST_ALLOW_MAP_TO_MAPPED_PADDR

static DRAM_ATTR const char *TAG = "mmap";

/**
 * @brief MMU Memory Mapping Driver
 *
 * Driver Backgrounds:
 *
 * --------------------------------------------------------------------------------------------------------
 *                                            Memory Pool                                                 |
 * --------------------------------------------------------------------------------------------------------
 * |                       Memory Region 0                              | Memory Region 1 |     ...       |
 * --------------------------------------------------------------------------------------------------------
 * | Block 0 | Slot 0 | Block 1 | Block 2 |  ...  | Slot 1 (final slot) |          ...                    |
 * --------------------------------------------------------------------------------------------------------
 *
 * - A block is a piece of vaddr range that is dynamically mapped. Blocks are doubly linked:
 *   Block 0 <-> Block 1 <-> Block 2
 * - A Slot is the vaddr range between 2 blocks.
 */

/**
 * Struct for a block
 */
typedef struct mem_block_ {
    uint32_t laddr_start;  //linear address start of this block
    uint32_t laddr_end;    //linear address end of this block
    intptr_t vaddr_start;  //virtual address start of this block
    intptr_t vaddr_end;    //virtual address end of this block
    size_t size;           //size of this block, should be aligned to MMU page size
    int caps;              //caps of this block, `mmu_mem_caps_t`
    uint32_t paddr_start;  //physical address start of this block
    uint32_t paddr_end;    //physical address end of this block
    mmu_target_t target;   //physical target that this block is mapped to
    TAILQ_ENTRY(mem_block_) entries;  //link entry
} mem_block_t;

/**
 * Struct for a memory region
 */
typedef struct mem_region_ {
    cache_bus_mask_t bus_id;  //cache bus mask of this region
    uint32_t start;           //linear address start of this region
    uint32_t end;             //linear address end of this region
    size_t region_size;       //region size, in bytes
    uint32_t free_head;       //linear address free head of this region
    size_t max_slot_size;     //max slot size within this region
    int caps;                 //caps of this region, `mmu_mem_caps_t`
    mmu_target_t targets;     //physical targets that this region is supported
    TAILQ_HEAD(mem_block_head_, mem_block_) mem_block_head;      //link head of allocated blocks within this region
} mem_region_t;

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
    mem_region_t mem_regions[SOC_MMU_LINEAR_ADDRESS_REGION_NUM];
} mmu_ctx_t;

static mmu_ctx_t s_mmu_ctx;

#if ENABLE_PADDR_CHECK
static bool s_is_enclosed(uint32_t block_start, uint32_t block_end, uint32_t new_block_start, uint32_t new_block_size);
static bool s_is_overlapped(uint32_t block_start, uint32_t block_end, uint32_t new_block_start, uint32_t new_block_size);
#endif  //#if ENABLE_PADDR_CHECK

#if CONFIG_APP_BUILD_USE_FLASH_SECTIONS

static cache_bus_mask_t s_get_bus_mask(uint32_t vaddr_start, uint32_t len)
{
#if CACHE_LL_EXT_MEM_VIA_L2CACHE
    return cache_ll_l2_get_bus(0, vaddr_start, len);
#else
    return cache_ll_l1_get_bus(0, vaddr_start, len);
#endif
}

static void s_reserve_irom_region(mem_region_t *hw_mem_regions, int region_nums)
{
    /**
     * We follow the way how 1st bootloader load flash .text:
     *
     * - Now IBUS addresses (between `_instruction_reserved_start` and `_instruction_reserved_end`) are consecutive on all chips,
     *   we strongly rely on this to calculate the .text length
     */
    extern int _instruction_reserved_start;
    extern int _instruction_reserved_end;
    size_t irom_len_to_reserve = (uint32_t)&_instruction_reserved_end - (uint32_t)&_instruction_reserved_start;
    assert((mmu_ll_vaddr_to_laddr((uint32_t)&_instruction_reserved_end) - mmu_ll_vaddr_to_laddr((uint32_t)&_instruction_reserved_start)) == irom_len_to_reserve);

    irom_len_to_reserve += (uint32_t)&_instruction_reserved_start - ALIGN_DOWN_BY((uint32_t)&_instruction_reserved_start, CONFIG_MMU_PAGE_SIZE);
    irom_len_to_reserve = ALIGN_UP_BY(irom_len_to_reserve, CONFIG_MMU_PAGE_SIZE);
    cache_bus_mask_t bus_mask = s_get_bus_mask((uint32_t)&_instruction_reserved_start, irom_len_to_reserve);

    for (int i = 0; i < SOC_MMU_LINEAR_ADDRESS_REGION_NUM; i++) {
        if (bus_mask & hw_mem_regions[i].bus_id) {
            if (hw_mem_regions[i].region_size <= irom_len_to_reserve) {
                hw_mem_regions[i].free_head = hw_mem_regions[i].end;
                hw_mem_regions[i].max_slot_size = 0;
                irom_len_to_reserve -= hw_mem_regions[i].region_size;
            } else {
                hw_mem_regions[i].free_head = hw_mem_regions[i].free_head + irom_len_to_reserve;
                hw_mem_regions[i].max_slot_size -= irom_len_to_reserve;
            }
        }
    }
}

static void s_reserve_drom_region(mem_region_t *hw_mem_regions, int region_nums)
{
    /**
     * Similarly, we follow the way how 1st bootloader load flash .rodata:
     */
    extern int _rodata_reserved_start;
    extern int _rodata_reserved_end;
    size_t drom_len_to_reserve = (uint32_t)&_rodata_reserved_end - (uint32_t)&_rodata_reserved_start;
    assert((mmu_ll_vaddr_to_laddr((uint32_t)&_rodata_reserved_end) - mmu_ll_vaddr_to_laddr((uint32_t)&_rodata_reserved_start)) == drom_len_to_reserve);

    drom_len_to_reserve += (uint32_t)&_rodata_reserved_start - ALIGN_DOWN_BY((uint32_t)&_rodata_reserved_start, CONFIG_MMU_PAGE_SIZE);
    drom_len_to_reserve = ALIGN_UP_BY(drom_len_to_reserve, CONFIG_MMU_PAGE_SIZE);
    cache_bus_mask_t bus_mask = s_get_bus_mask((uint32_t)&_rodata_reserved_start, drom_len_to_reserve);

    for (int i = 0; i < SOC_MMU_LINEAR_ADDRESS_REGION_NUM; i++) {
        if (bus_mask & hw_mem_regions[i].bus_id) {
            if (hw_mem_regions[i].region_size <= drom_len_to_reserve) {
                hw_mem_regions[i].free_head = hw_mem_regions[i].end;
                hw_mem_regions[i].max_slot_size = 0;
                drom_len_to_reserve -= hw_mem_regions[i].region_size;
            } else {
                hw_mem_regions[i].free_head = hw_mem_regions[i].free_head + drom_len_to_reserve;
                hw_mem_regions[i].max_slot_size -= drom_len_to_reserve;
            }
        }
    }
}
#endif  //#if CONFIG_APP_BUILD_USE_FLASH_SECTIONS

void esp_mmu_map_init(void)
{
    mem_region_t hw_mem_regions[SOC_MMU_LINEAR_ADDRESS_REGION_NUM] = {};

    for (int i = 0; i < SOC_MMU_LINEAR_ADDRESS_REGION_NUM; i++) {
        hw_mem_regions[i].start = g_mmu_mem_regions[i].start;
        hw_mem_regions[i].end = g_mmu_mem_regions[i].end;
        hw_mem_regions[i].region_size = g_mmu_mem_regions[i].size;
        hw_mem_regions[i].max_slot_size = g_mmu_mem_regions[i].size;
        hw_mem_regions[i].free_head = g_mmu_mem_regions[i].start;
        hw_mem_regions[i].bus_id = g_mmu_mem_regions[i].bus_id;
        hw_mem_regions[i].caps = g_mmu_mem_regions[i].caps;
        hw_mem_regions[i].targets = g_mmu_mem_regions[i].targets;
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
        assert(__builtin_popcount(hw_mem_regions[i].bus_id) == 1);
#endif
        assert(hw_mem_regions[i].region_size % CONFIG_MMU_PAGE_SIZE == 0);
    }

#if CONFIG_APP_BUILD_USE_FLASH_SECTIONS
    //First reserve memory regions used for irom and drom, as we must follow the way how 1st bootloader load them
    s_reserve_irom_region(hw_mem_regions, SOC_MMU_LINEAR_ADDRESS_REGION_NUM);
    s_reserve_drom_region(hw_mem_regions, SOC_MMU_LINEAR_ADDRESS_REGION_NUM);
#endif  //#if CONFIG_APP_BUILD_USE_FLASH_SECTIONS

    if (SOC_MMU_LINEAR_ADDRESS_REGION_NUM > 1) {
        //Now we can coalesce adjacent regions
        for (int i = 1; i < SOC_MMU_LINEAR_ADDRESS_REGION_NUM; i++) {
            mem_region_t *a = &hw_mem_regions[i - 1];
            mem_region_t *b = &hw_mem_regions[i];
            if ((b->free_head == a->end) && (b->caps == a->caps) && (b->targets == a->targets)) {
                a->caps = MEM_REGION_MERGED;
                b->bus_id |= a->bus_id;
                b->start = a->start;
                b->region_size += a->region_size;
                b->free_head = a->free_head;
                b->max_slot_size += a->max_slot_size;
            }
        }
    }

    //Count the mem regions left after coalescing
    uint32_t region_num = 0;
    for (int i = 0; i < SOC_MMU_LINEAR_ADDRESS_REGION_NUM; i++) {
        if (hw_mem_regions[i].caps != MEM_REGION_MERGED) {
            region_num++;
        }
    }
    ESP_EARLY_LOGV(TAG, "after coalescing, %" PRIu32 " regions are left", region_num);

    //Initialise `s_mmu_ctx.mem_regions[]`, as we've done all static allocation, to prepare available virtual memory regions
    uint32_t available_region_idx = 0;
    s_mmu_ctx.num_regions = region_num;
    for (int i = 0; i < SOC_MMU_LINEAR_ADDRESS_REGION_NUM; i++) {
        if (hw_mem_regions[i].caps == MEM_REGION_MERGED) {
            continue;
        }

        memcpy(&s_mmu_ctx.mem_regions[available_region_idx], &hw_mem_regions[i], sizeof(mem_region_t));
        available_region_idx++;
    }

    for (int i = 0; i < available_region_idx; i++) {
        TAILQ_INIT(&s_mmu_ctx.mem_regions[i].mem_block_head);
    }

    assert(available_region_idx == region_num);
}

static esp_err_t s_mem_caps_check(mmu_mem_caps_t caps)
{
    if (caps & MMU_MEM_CAP_EXEC) {
        if ((caps & MMU_MEM_CAP_8BIT) || (caps & MMU_MEM_CAP_WRITE)) {
            //None of the executable memory are expected to be 8-bit accessible or writable.
            return ESP_ERR_INVALID_ARG;
        }
        caps |= MMU_MEM_CAP_32BIT;
    }
    return ESP_OK;
}

esp_err_t esp_mmu_map_get_max_consecutive_free_block_size(mmu_mem_caps_t caps, mmu_target_t target, size_t *out_len)
{
    ESP_RETURN_ON_FALSE(out_len, ESP_ERR_INVALID_ARG, TAG, "null pointer");
    ESP_RETURN_ON_ERROR(s_mem_caps_check(caps), TAG, "invalid caps");
    *out_len = 0;

    size_t max = 0;

    for (int i = 0; i < s_mmu_ctx.num_regions; i++) {
        if (((s_mmu_ctx.mem_regions[i].caps & caps) == caps) && ((s_mmu_ctx.mem_regions[i].targets & target) == target)) {
            if (s_mmu_ctx.mem_regions[i].max_slot_size > max) {
                max = s_mmu_ctx.mem_regions[i].max_slot_size;
            }
        }
    }

    *out_len = max;

    return ESP_OK;
}

static int32_t s_find_available_region(mem_region_t *mem_regions, uint32_t region_nums, size_t size, mmu_mem_caps_t caps, mmu_target_t target)
{
    int32_t found_region_id = -1;
    for (int i = 0; i < region_nums; i++) {
        if (((mem_regions[i].caps & caps) == caps) && ((mem_regions[i].targets & target) == target)) {
            if (mem_regions[i].max_slot_size >= size) {
                found_region_id = i;
                break;
            }
        }
    }
    return found_region_id;
}

esp_err_t esp_mmu_map_reserve_block_with_caps(size_t size, mmu_mem_caps_t caps, mmu_target_t target, const void **out_ptr)
{
    ESP_RETURN_ON_FALSE(out_ptr, ESP_ERR_INVALID_ARG, TAG, "null pointer");
    ESP_RETURN_ON_ERROR(s_mem_caps_check(caps), TAG, "invalid caps");

    size_t aligned_size = ALIGN_UP_BY(size, CONFIG_MMU_PAGE_SIZE);
    uint32_t laddr = 0;

    int32_t found_region_id = s_find_available_region(s_mmu_ctx.mem_regions, s_mmu_ctx.num_regions, aligned_size, caps, target);
    if (found_region_id == -1) {
        ESP_EARLY_LOGE(TAG, "no such vaddr range");
        return ESP_ERR_NOT_FOUND;
    }

    laddr = (uint32_t)s_mmu_ctx.mem_regions[found_region_id].free_head;
    s_mmu_ctx.mem_regions[found_region_id].free_head += aligned_size;
    s_mmu_ctx.mem_regions[found_region_id].max_slot_size -= aligned_size;
    ESP_EARLY_LOGV(TAG, "found laddr is 0x%" PRIx32, laddr);

    uint32_t vaddr = 0;
    if (caps & MMU_MEM_CAP_EXEC) {
        vaddr = mmu_ll_laddr_to_vaddr(laddr, MMU_VADDR_INSTRUCTION, target);
    } else {
        vaddr = mmu_ll_laddr_to_vaddr(laddr, MMU_VADDR_DATA, target);
    }
    *out_ptr = (void *)vaddr;

    return ESP_OK;
}

IRAM_ATTR esp_err_t esp_mmu_paddr_find_caps(const esp_paddr_t paddr, mmu_mem_caps_t *out_caps)
{
    mem_region_t *region = NULL;
    mem_block_t *mem_block = NULL;
    bool found = false;
    mem_block_t *found_block = NULL;
    if (out_caps == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    for (int i = 0; i < s_mmu_ctx.num_regions; i++) {
        region = &s_mmu_ctx.mem_regions[i];

        TAILQ_FOREACH(mem_block, &region->mem_block_head, entries) {
            if (mem_block == TAILQ_FIRST(&region->mem_block_head) || mem_block == TAILQ_LAST(&region->mem_block_head, mem_block_head_)) {
                //we don't care the dummy_head and the dummy_tail
                continue;
            }

            //now we are only traversing the actual dynamically allocated blocks, dummy_head and dummy_tail are excluded already
            if (mem_block->paddr_start == paddr) {
                found = true;
                found_block = mem_block;
                break;
            }
        }
    }

    if (!found) {
        return ESP_ERR_NOT_FOUND;
    }

    *out_caps = found_block->caps;
    return ESP_OK;
}

static void IRAM_ATTR NOINLINE_ATTR s_do_cache_invalidate(uint32_t vaddr_start, uint32_t size)
{
#if CONFIG_IDF_TARGET_ESP32
    /**
     * On ESP32, due to hardware limitation, we don't have an
     * easy way to sync between cache and external memory wrt
     * certain range. So we do a full sync here
     */
    cache_sync();
#else   //Other chips
    cache_hal_invalidate_addr(vaddr_start, size);
#endif // CONFIG_IDF_TARGET_ESP32
}

#if MMU_LL_MMU_PER_TARGET
FORCE_INLINE_ATTR uint32_t s_mapping_operation(mmu_target_t target, uint32_t vaddr_start, esp_paddr_t paddr_start, uint32_t size)
{
    uint32_t actual_mapped_len = 0;
    uint32_t mmu_id = 0;
    if (target == MMU_TARGET_FLASH0) {
        mmu_id = MMU_LL_FLASH_MMU_ID;
    } else {
        mmu_id = MMU_LL_PSRAM_MMU_ID;
    }
    mmu_hal_map_region(mmu_id, target, vaddr_start, paddr_start, size, &actual_mapped_len);

    return actual_mapped_len;
}
#else
FORCE_INLINE_ATTR uint32_t s_mapping_operation(mmu_target_t target, uint32_t vaddr_start, esp_paddr_t paddr_start, uint32_t size)
{
    uint32_t actual_mapped_len = 0;

    mmu_hal_map_region(0, target, vaddr_start, paddr_start, size, &actual_mapped_len);
#if (SOC_MMU_PERIPH_NUM == 2)
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    mmu_hal_map_region(1, target, vaddr_start, paddr_start, size, &actual_mapped_len);
#endif //  #if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
#endif //  #if (SOC_MMU_PERIPH_NUM == 2)

    return actual_mapped_len;
}
#endif

static void IRAM_ATTR NOINLINE_ATTR s_do_mapping(mmu_target_t target, uint32_t vaddr_start, esp_paddr_t paddr_start, uint32_t size)
{
    /**
     * Disable Cache, after this function, involved code and data should be placed in internal RAM.
     *
     * @note we call this for now, but this will be refactored to move out of `spi_flash`
     */
    spi_flash_disable_interrupts_caches_and_other_cpu();

    uint32_t actual_mapped_len = s_mapping_operation(target, vaddr_start, paddr_start, size);

    cache_bus_mask_t bus_mask = cache_ll_l1_get_bus(0, vaddr_start, size);
    cache_ll_l1_enable_bus(0, bus_mask);
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    bus_mask = cache_ll_l1_get_bus(0, vaddr_start, size);
    cache_ll_l1_enable_bus(1, bus_mask);
#endif

    s_do_cache_invalidate(vaddr_start, size);

    //enable Cache, after this function, internal RAM access is no longer mandatory
    spi_flash_enable_interrupts_caches_and_other_cpu();

    ESP_EARLY_LOGV(TAG, "actual_mapped_len is 0x%"PRIx32, actual_mapped_len);
}

esp_err_t esp_mmu_map(esp_paddr_t paddr_start, size_t size, mmu_target_t target, mmu_mem_caps_t caps, int flags, void **out_ptr)
{
    esp_err_t ret = ESP_FAIL;
    ESP_RETURN_ON_FALSE(out_ptr, ESP_ERR_INVALID_ARG, TAG, "null pointer");
#if !SOC_SPIRAM_SUPPORTED || CONFIG_IDF_TARGET_ESP32
    ESP_RETURN_ON_FALSE(!(target & MMU_TARGET_PSRAM0), ESP_ERR_NOT_SUPPORTED, TAG, "PSRAM is not supported");
#endif
    ESP_RETURN_ON_FALSE((paddr_start % CONFIG_MMU_PAGE_SIZE == 0), ESP_ERR_INVALID_ARG, TAG, "paddr must be rounded up to the nearest multiple of CONFIG_MMU_PAGE_SIZE");
    ESP_RETURN_ON_ERROR(s_mem_caps_check(caps), TAG, "invalid caps");

    size_t aligned_size = ALIGN_UP_BY(size, CONFIG_MMU_PAGE_SIZE);
    int32_t found_region_id = s_find_available_region(s_mmu_ctx.mem_regions, s_mmu_ctx.num_regions, aligned_size, caps, target);
    if (found_region_id == -1) {
        ESP_EARLY_LOGE(TAG, "no such vaddr range");
        return ESP_ERR_NOT_FOUND;
    }

    //Now we're sure we can find an available block inside a certain region
    mem_region_t *found_region = &s_mmu_ctx.mem_regions[found_region_id];
    mem_block_t *dummy_head = NULL;
    mem_block_t *dummy_tail = NULL;
    mem_block_t *new_block = NULL;

    if (TAILQ_EMPTY(&found_region->mem_block_head)) {
        dummy_head = (mem_block_t *)heap_caps_calloc(1, sizeof(mem_block_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        ESP_GOTO_ON_FALSE(dummy_head, ESP_ERR_NO_MEM, err, TAG, "no mem");

        dummy_head->laddr_start = found_region->free_head;
        dummy_head->laddr_end = found_region->free_head;
        //We don't care vaddr or paddr address for dummy head
        dummy_head->size = 0;
        dummy_head->caps = caps;
        TAILQ_INSERT_HEAD(&found_region->mem_block_head, dummy_head, entries);

        dummy_tail = (mem_block_t *)heap_caps_calloc(1, sizeof(mem_block_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        ESP_GOTO_ON_FALSE(dummy_tail, ESP_ERR_NO_MEM, err, TAG, "no mem");

        dummy_tail->laddr_start = found_region->end;
        dummy_tail->laddr_end = found_region->end;
        //We don't care vaddr or paddr address for dummy tail
        dummy_tail->size = 0;
        dummy_tail->caps = caps;
        TAILQ_INSERT_TAIL(&found_region->mem_block_head, dummy_tail, entries);
    }

    //Check if paddr is overlapped
    mem_block_t *mem_block = NULL;

#if ENABLE_PADDR_CHECK
    bool is_enclosed = false;
    bool is_overlapped = false;
    bool allow_overlap = flags & ESP_MMU_MMAP_FLAG_PADDR_SHARED;

    TAILQ_FOREACH(mem_block, &found_region->mem_block_head, entries) {
        if (target == mem_block->target) {
            if ((s_is_enclosed(mem_block->paddr_start, mem_block->paddr_end, paddr_start, aligned_size))) {
                //the to-be-mapped paddr block is mapped already
                is_enclosed = true;
                break;
            }

            if (!allow_overlap && (s_is_overlapped(mem_block->paddr_start, mem_block->paddr_end, paddr_start, aligned_size))) {
                is_overlapped = true;
                break;
            }
        }
    }

    if (is_enclosed) {
        ESP_LOGW(TAG, "paddr block is mapped already, vaddr_start: %p, size: 0x%x", (void *)mem_block->vaddr_start, mem_block->size);
        *out_ptr = (void *)mem_block->vaddr_start;
        return ESP_ERR_INVALID_STATE;
    }

    if (!allow_overlap && is_overlapped) {
        ESP_LOGE(TAG, "paddr block is overlapped with an already mapped paddr block");
        return ESP_ERR_INVALID_ARG;
    }
#endif //#if ENABLE_PADDR_CHECK

    new_block = (mem_block_t *)heap_caps_calloc(1, sizeof(mem_block_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(new_block, ESP_ERR_NO_MEM, err, TAG, "no mem");

    //Reserve this block as it'll be mapped
    bool found = false;
    // Get the end address of the dummy_head block, which is always first block on the list
    uint32_t last_end = TAILQ_FIRST(&found_region->mem_block_head)->laddr_end;
    size_t slot_len = 0;
    size_t max_slot_len = 0;
    mem_block_t *found_block = NULL;  //This stands for the block we found, whose slot between its prior block is where we will insert the new block to

    TAILQ_FOREACH(mem_block, &found_region->mem_block_head, entries) {
        slot_len = mem_block->laddr_start - last_end;

        if (!found) {
            if (slot_len >= aligned_size) {
                //Found it
                found = true;
                found_block = mem_block;
                slot_len -= aligned_size;
                new_block->laddr_start = last_end;
            }
        }

        max_slot_len = (slot_len > max_slot_len) ? slot_len : max_slot_len;
        last_end = mem_block->laddr_end;
    }

    assert(found);
    //insert the to-be-mapped new block to the list
    TAILQ_INSERT_BEFORE(found_block, new_block, entries);

    //Finally, we update the max_slot_size
    found_region->max_slot_size = max_slot_len;

    //Now we fill others according to the found `new_block->laddr_start`
    new_block->laddr_end = new_block->laddr_start + aligned_size;
    new_block->size = aligned_size;
    new_block->caps = caps;
    new_block->paddr_start = paddr_start;
    new_block->paddr_end = paddr_start + aligned_size;
    new_block->target = target;
    if (caps & MMU_MEM_CAP_EXEC) {
        new_block->vaddr_start = mmu_ll_laddr_to_vaddr(new_block->laddr_start, MMU_VADDR_INSTRUCTION, target);
        new_block->vaddr_end = mmu_ll_laddr_to_vaddr(new_block->laddr_end, MMU_VADDR_INSTRUCTION, target);
    } else {
        new_block->vaddr_start = mmu_ll_laddr_to_vaddr(new_block->laddr_start, MMU_VADDR_DATA, target);
        new_block->vaddr_end = mmu_ll_laddr_to_vaddr(new_block->laddr_end, MMU_VADDR_DATA, target);
    }

    //do mapping
    s_do_mapping(target, new_block->vaddr_start, paddr_start, aligned_size);
    *out_ptr = (void *)new_block->vaddr_start;

    return ESP_OK;

err:
    if (dummy_tail) {
        free(dummy_tail);
    }
    if (dummy_head) {
        free(dummy_head);
    }

    return ret;
}

#if MMU_LL_MMU_PER_TARGET
FORCE_INLINE_ATTR void s_unmapping_operation(uint32_t vaddr_start, uint32_t size)
{
    uint32_t mmu_id = 0;
    mmu_target_t target = mmu_ll_vaddr_to_target(vaddr_start);

    if (target == MMU_TARGET_FLASH0) {
        mmu_id = MMU_LL_FLASH_MMU_ID;
    } else {
        mmu_id = MMU_LL_PSRAM_MMU_ID;
    }
    mmu_hal_unmap_region(mmu_id, vaddr_start, size);
}
#else
FORCE_INLINE_ATTR void s_unmapping_operation(uint32_t vaddr_start, uint32_t size)
{
    mmu_hal_unmap_region(0, vaddr_start, size);
#if (SOC_MMU_PERIPH_NUM == 2)
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    mmu_hal_unmap_region(1, vaddr_start, size);
#endif //  #if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
#endif //  #if (SOC_MMU_PERIPH_NUM == 2)
}
#endif

static void IRAM_ATTR NOINLINE_ATTR s_do_unmapping(uint32_t vaddr_start, uint32_t size)
{
    /**
     * Disable Cache, after this function, involved code and data should be placed in internal RAM.
     *
     * @note we call this for now, but this will be refactored to move out of `spi_flash`
     */
    spi_flash_disable_interrupts_caches_and_other_cpu();

    s_unmapping_operation(vaddr_start, size);

    //enable Cache, after this function, internal RAM access is no longer mandatory
    spi_flash_enable_interrupts_caches_and_other_cpu();
}

esp_err_t esp_mmu_unmap(void *ptr)
{
    ESP_RETURN_ON_FALSE(ptr, ESP_ERR_INVALID_ARG, TAG, "null pointer");

    mem_region_t *region = NULL;
    mem_block_t *mem_block = NULL;
    uint32_t ptr_laddr = mmu_ll_vaddr_to_laddr((uint32_t)ptr);
    size_t slot_len = 0;

    for (int i = 0; i < s_mmu_ctx.num_regions; i++) {
        if (ptr_laddr >= s_mmu_ctx.mem_regions[i].free_head && ptr_laddr < s_mmu_ctx.mem_regions[i].end) {
            region = &s_mmu_ctx.mem_regions[i];
        }
    }
    ESP_RETURN_ON_FALSE(region, ESP_ERR_NOT_FOUND, TAG, "munmap target pointer is outside external memory regions");

    bool found = false;
    mem_block_t *found_block = NULL;
    TAILQ_FOREACH(mem_block, &region->mem_block_head, entries) {
        if (mem_block == TAILQ_FIRST(&region->mem_block_head) || mem_block == TAILQ_LAST(&region->mem_block_head, mem_block_head_)) {
            //we don't care the dummy_head and the dummy_tail
            continue;
        }

        //now we are only traversing the actual dynamically allocated blocks, dummy_head and dummy_tail are excluded already
        if (mem_block->laddr_start == ptr_laddr) {
            slot_len = TAILQ_NEXT(mem_block, entries)->laddr_start - TAILQ_PREV(mem_block, mem_block_head_, entries)->laddr_end;
            region->max_slot_size = (slot_len > region->max_slot_size) ? slot_len : region->max_slot_size;

            found = true;
            found_block = mem_block;
            break;
        }
    }

    ESP_RETURN_ON_FALSE(found, ESP_ERR_NOT_FOUND, TAG, "munmap target pointer isn't mapped yet");

    //do unmap
    s_do_unmapping(mem_block->vaddr_start, mem_block->size);
    //remove the already unmapped block from the list
    TAILQ_REMOVE(&region->mem_block_head, found_block, entries);
    free(found_block);

    return ESP_OK;
}

esp_err_t esp_mmu_map_dump_mapped_blocks(FILE* stream)
{
    char line[100];
    for (int i = 0; i < s_mmu_ctx.num_regions; i++) {
        fprintf(stream, "region %d:\n", i);
        fprintf(stream, "%-15s %-14s %-14s %-12s %-12s %-12s\n", "Bus ID", "Start", "Free Head", "End", "Caps", "Max Slot Size");

        char *buf = line;
        size_t len = sizeof(line);
        memset(line, 0x0, len);
        snprintf(buf, len, "0x%-13x 0x%-12"PRIx32" 0x%-11"PRIx32"  0x%-10"PRIx32" 0x%-10x 0x%-8x\n",
                 s_mmu_ctx.mem_regions[i].bus_id,
                 s_mmu_ctx.mem_regions[i].start,
                 s_mmu_ctx.mem_regions[i].free_head,
                 s_mmu_ctx.mem_regions[i].end,
                 s_mmu_ctx.mem_regions[i].caps,
                 s_mmu_ctx.mem_regions[i].max_slot_size);
        fputs(line, stream);

        fprintf(stream, "mapped blocks:\n");
        fprintf(stream, "%-4s %-13s %-12s %-12s %-6s %-13s %-11s\n", "ID", "Vaddr Start", "Vaddr End", "Block Size", "Caps", "Paddr Start", "Paddr End");
        mem_region_t *region = &s_mmu_ctx.mem_regions[i];
        mem_block_t *mem_block = NULL;
        int id = 0;
        TAILQ_FOREACH(mem_block, &region->mem_block_head, entries) {
            if (mem_block != TAILQ_FIRST(&region->mem_block_head) && mem_block != TAILQ_LAST(&region->mem_block_head, mem_block_head_)) {
                snprintf(buf, len, "%-4d 0x%-11x 0x%-10x 0x%-10x 0x%-4x 0x%-11"PRIx32" 0x%-8"PRIx32"\n",
                         id,
                         mem_block->vaddr_start,
                         mem_block->vaddr_end,
                         mem_block->size,
                         mem_block->caps,
                         mem_block->paddr_start,
                         mem_block->paddr_end);
                fputs(line, stream);
                id++;
            }
        }
        fprintf(stream, "\n");
    }

    return ESP_OK;
}

/*---------------------------------------------------------------
        Private dump functions, IRAM Safe
---------------------------------------------------------------*/
esp_err_t IRAM_ATTR esp_mmu_map_dump_mapped_blocks_private(void)
{
    for (int i = 0; i < s_mmu_ctx.num_regions; i++) {
        mem_region_t *region = &s_mmu_ctx.mem_regions[i];
        mem_block_t *mem_block = NULL;
        TAILQ_FOREACH(mem_block, &region->mem_block_head, entries) {
            if (mem_block != TAILQ_FIRST(&region->mem_block_head) && mem_block != TAILQ_LAST(&region->mem_block_head, mem_block_head_)) {
                ESP_DRAM_LOGI(TAG, "block vaddr_start: 0x%x", mem_block->vaddr_start);
                ESP_DRAM_LOGI(TAG, "block vaddr_end: 0x%x", mem_block->vaddr_end);
                ESP_DRAM_LOGI(TAG, "block size: 0x%x", mem_block->size);
                ESP_DRAM_LOGI(TAG, "block caps: 0x%x", mem_block->caps);
                ESP_DRAM_LOGI(TAG, "block paddr_start: 0x%x", mem_block->paddr_start);
                ESP_DRAM_LOGI(TAG, "block paddr_end: 0x%x", mem_block->paddr_end);
            }
        }
        ESP_DRAM_LOGI(TAG, "region bus_id: 0x%x", s_mmu_ctx.mem_regions[i].bus_id);
        ESP_DRAM_LOGI(TAG, "region start: 0x%x", s_mmu_ctx.mem_regions[i].start);
        ESP_DRAM_LOGI(TAG, "region end: 0x%x", s_mmu_ctx.mem_regions[i].end);
        ESP_DRAM_LOGI(TAG, "region caps: 0x%x", s_mmu_ctx.mem_regions[i].caps);
    }

    return ESP_OK;
}

/*---------------------------------------------------------------
    Helper APIs for conversion between vaddr and paddr
---------------------------------------------------------------*/
static bool NOINLINE_ATTR IRAM_ATTR s_vaddr_to_paddr(uint32_t vaddr, esp_paddr_t *out_paddr, mmu_target_t *out_target)
{
    //we call this for now, but this will be refactored to move out of `spi_flash`
    spi_flash_disable_interrupts_caches_and_other_cpu();
    //On ESP32, core 1 settings should be the same as the core 0
    bool is_mapped = mmu_hal_vaddr_to_paddr(0, vaddr, out_paddr, out_target);
    spi_flash_enable_interrupts_caches_and_other_cpu();

    return is_mapped;
}

esp_err_t esp_mmu_vaddr_to_paddr(void *vaddr, esp_paddr_t *out_paddr, mmu_target_t *out_target)
{
    ESP_RETURN_ON_FALSE(vaddr && out_paddr, ESP_ERR_INVALID_ARG, TAG, "null pointer");
    ESP_RETURN_ON_FALSE(mmu_hal_check_valid_ext_vaddr_region(0, (uint32_t)vaddr, 1, MMU_VADDR_DATA | MMU_VADDR_INSTRUCTION), ESP_ERR_INVALID_ARG, TAG, "not a valid external virtual address");

    esp_paddr_t paddr = 0;
    mmu_target_t target = 0;

    bool is_mapped = s_vaddr_to_paddr((uint32_t)vaddr, &paddr, &target);
    ESP_RETURN_ON_FALSE(is_mapped, ESP_ERR_NOT_FOUND, TAG, "vaddr isn't mapped");

    *out_paddr = paddr;
    *out_target = target;

    return ESP_OK;
}

static bool NOINLINE_ATTR IRAM_ATTR s_paddr_to_vaddr(esp_paddr_t paddr, mmu_target_t target, mmu_vaddr_t type, uint32_t *out_vaddr)
{
    //we call this for now, but this will be refactored to move out of `spi_flash`
    spi_flash_disable_interrupts_caches_and_other_cpu();
    //On ESP32, core 1 settings should be the same as the core 0
    bool found = mmu_hal_paddr_to_vaddr(0, paddr, target, type, out_vaddr);
    spi_flash_enable_interrupts_caches_and_other_cpu();

    return found;
}

esp_err_t esp_mmu_paddr_to_vaddr(esp_paddr_t paddr, mmu_target_t target, mmu_vaddr_t type, void **out_vaddr)
{
    ESP_RETURN_ON_FALSE(out_vaddr, ESP_ERR_INVALID_ARG, TAG, "null pointer");

    uint32_t vaddr = 0;
    bool found = false;

    found = s_paddr_to_vaddr(paddr, target, type, &vaddr);
    ESP_RETURN_ON_FALSE(found, ESP_ERR_NOT_FOUND, TAG, "paddr isn't mapped");

    *out_vaddr = (void *)vaddr;

    return ESP_OK;
}

#if ENABLE_PADDR_CHECK
/*---------------------------------------------------------------
    Helper functions to check block
---------------------------------------------------------------*/
/**
 * Check if a new block is enclosed by another, e.g.
 *
 * This is enclosed:
 *
 *       new_block_start               new_block_end
 *              |-------- New Block --------|
 *      |--------------- Block ---------------|
 * block_start                              block_end
 *
 * @note Note the difference between `s_is_overlapped()` below
 *
 * @param block_start     An original block start
 * @param block_end       An original block end
 * @param new_block_start New block start
 * @param new_block_size  New block size
 *
 * @return True: new block is enclosed; False: new block is not enclosed
 */
static bool s_is_enclosed(uint32_t block_start, uint32_t block_end, uint32_t new_block_start, uint32_t new_block_size)
{
    bool is_enclosed = false;
    uint32_t new_block_end = new_block_start + new_block_size;

    if ((new_block_start >= block_start) && (new_block_end <= block_end)) {
        is_enclosed = true;
    } else {
        is_enclosed = false;
    }

    return is_enclosed;
}

/**
 * Check if a new block is overlapped by another, e.g.
 *
 * This is overlapped:
 *
 *       new_block_start                 new_block_end
 *              |---------- New Block ----------|
 *      |--------------- Block ---------------|
 * block_start                              block_end
 *
 * @note Note the difference between `s_is_enclosed()` above
 *
 * @param block_start     An original block start
 * @param block_end       An original block end
 * @param new_block_start New block start
 * @param new_block_size  New block size
 *
 * @return True: new block is overlapped; False: new block is not overlapped
 */
static bool s_is_overlapped(uint32_t block_start, uint32_t block_end, uint32_t new_block_start, uint32_t new_block_size)
{
    bool is_overlapped = false;
    uint32_t new_block_end = new_block_start + new_block_size;

    if (((new_block_start < block_start) && (new_block_end > block_start)) ||
            ((new_block_start < block_end) && (new_block_end > block_end))) {
        is_overlapped = true;
    } else {
        is_overlapped = false;
    }

    return is_overlapped;
}
#endif  //#if ENABLE_PADDR_CHECK
