/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_rom_caps.h"
#include "hal/mmu_ll.h"
#include "hal/mmu_hal.h"
#include "hal/cache_hal.h"
#if ESP_ROM_NEEDS_SET_CACHE_MMU_SIZE
#include "soc/mmu.h"
#endif

#include "esp_private/esp_mmu_map_private.h"
#include "esp_mmu_map.h"
#include "esp_rom_spiflash.h"
#if CONFIG_SPIRAM
#include "esp_private/esp_psram_extram.h"
#include "esp_private/mmu_psram_flash.h"
#endif

#if CONFIG_IDF_TARGET_ESP32
#include "esp_private/esp_cache_esp32_private.h"
#endif

#include "esp_private/cache_utils.h"
#include "spi_flash_mmap.h"

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
extern char _instruction_reserved_start;
extern char _instruction_reserved_end;
#endif

#if CONFIG_SPIRAM_RODATA
extern char _rodata_reserved_start;
extern char _rodata_reserved_end;
#endif

#if !CONFIG_SPI_FLASH_ROM_IMPL


typedef struct mmap_block_t {
    uint32_t *vaddr_list;
    int list_num;
} mmap_block_t;


esp_err_t spi_flash_mmap(size_t src_addr, size_t size, spi_flash_mmap_memory_t memory,
                         const void** out_ptr, spi_flash_mmap_handle_t* out_handle)
{
    esp_err_t ret = ESP_FAIL;
    mmu_mem_caps_t caps = 0;
    void *ptr = NULL;
    mmap_block_t *block = NULL;
    uint32_t *vaddr_list = NULL;

    block = heap_caps_calloc(1, sizeof(mmap_block_t), MALLOC_CAP_INTERNAL);
    if (!block) {
        ret = ESP_ERR_NO_MEM;
        goto err;
    }

    vaddr_list = heap_caps_calloc(1, 1 * sizeof(uint32_t), MALLOC_CAP_INTERNAL);
    if (!vaddr_list) {
        ret = ESP_ERR_NO_MEM;
        goto err;
    }

    block->vaddr_list = vaddr_list;

    if (memory == SPI_FLASH_MMAP_INST) {
        caps = MMU_MEM_CAP_EXEC | MMU_MEM_CAP_32BIT;
    } else {
        caps = MMU_MEM_CAP_READ | MMU_MEM_CAP_8BIT;
    }
    ret = esp_mmu_map(src_addr, size, MMU_TARGET_FLASH0, caps, ESP_MMU_MMAP_FLAG_PADDR_SHARED, &ptr);
    if (ret == ESP_OK) {
        vaddr_list[0] = (uint32_t)ptr;
        block->list_num = 1;

    } else if (ret == ESP_ERR_INVALID_STATE) {
        /**
         * paddr region is mapped already,
         * to keep `flash_mmap.c` original behaviour, we consider this as a valid behaviour.
         * Set `list_num` to 0 so we don't need to call `esp_mmu_unmap` to this one, as `esp_mmu_map`
         * doesn't really create a new handle.
         */
        block->list_num = 0;
    } else {
        goto err;
    }

    *out_ptr = ptr;
    *out_handle = (uint32_t)block;

    return ESP_OK;

err:
    if (vaddr_list) {
        free(vaddr_list);
    }
    if (block) {
        free(block);
    }
    return ret;
}


static int s_find_non_contiguous_block_nums(const int *pages, int page_count)
{
    int nums = 1;
    int last_end = pages[0] + 1;

    for (int i = 1; i < page_count; i++) {
        if (pages[i] != last_end) {
            nums++;
        }
        last_end = pages[i] + 1;
    }
    return nums;
}

static void s_merge_contiguous_pages(const int *pages, uint32_t page_count, int block_nums, int (*out_blocks)[2])
{
    uint32_t last_end = pages[0] + 1;
    int new_array_id = 0;
    out_blocks[new_array_id][0] = pages[0];
    out_blocks[new_array_id][1] = 1;

    for (int i = 1; i < page_count; i++) {

        if (pages[i] != last_end) {
            new_array_id += 1;
            assert(new_array_id < block_nums);
            out_blocks[new_array_id][0] = pages[i];
            out_blocks[new_array_id][1] = 1;
        } else {
            out_blocks[new_array_id][1] += 1;
        }

        last_end = pages[i] + 1;
    }
}

static void s_pages_to_bytes(int (*blocks)[2], int block_nums)
{
    for (int i = 0; i < block_nums; i++) {
        blocks[i][0] = blocks[i][0] * CONFIG_MMU_PAGE_SIZE;
        blocks[i][1] = blocks[i][1] * CONFIG_MMU_PAGE_SIZE;
    }
}

esp_err_t spi_flash_mmap_pages(const int *pages, size_t page_count, spi_flash_mmap_memory_t memory,
                         const void** out_ptr, spi_flash_mmap_handle_t* out_handle)
{
    esp_err_t ret = ESP_FAIL;
    mmu_mem_caps_t caps = 0;
    mmap_block_t *block = NULL;
    uint32_t *vaddr_list = NULL;
    int successful_cnt = 0;

    int block_num = s_find_non_contiguous_block_nums(pages, page_count);
    int paddr_blocks[block_num][2];
    s_merge_contiguous_pages(pages, page_count, block_num, paddr_blocks);
    s_pages_to_bytes(paddr_blocks, block_num);

    block = heap_caps_calloc(1, sizeof(mmap_block_t), MALLOC_CAP_INTERNAL);
    if (!block) {
        ret = ESP_ERR_NO_MEM;
        goto err;
    }

    vaddr_list = heap_caps_calloc(1, block_num * sizeof(uint32_t), MALLOC_CAP_INTERNAL);
    if (!vaddr_list) {
        ret = ESP_ERR_NO_MEM;
        goto err;
    }

    if (memory == SPI_FLASH_MMAP_INST) {
        caps = MMU_MEM_CAP_EXEC | MMU_MEM_CAP_32BIT;
    } else {
        caps = MMU_MEM_CAP_READ | MMU_MEM_CAP_8BIT;
    }
    for (int i = 0; i < block_num; i++) {
        void *ptr = NULL;
        ret = esp_mmu_map(paddr_blocks[i][0], paddr_blocks[i][1], MMU_TARGET_FLASH0, caps, ESP_MMU_MMAP_FLAG_PADDR_SHARED, &ptr);
        if (ret == ESP_OK) {
            vaddr_list[i] = (uint32_t)ptr;
            successful_cnt++;
        } else {
            /**
             * A note for `ret == ESP_ERR_INVALID_STATE`:
             * If one of the `*pages` are mapped already, this means we can't find a
             * consecutive vaddr block for these `*pages`
             */
            goto err;
        }
        vaddr_list[i] = (uint32_t)ptr;
    }

    block->vaddr_list = vaddr_list;
    block->list_num = successful_cnt;

    /**
     * We get a contiguous vaddr block, but may contain multiple esp_mmu handles.
     * The first handle vaddr is the start address of this contiguous vaddr block.
     */
    *out_ptr = (void *)vaddr_list[0];
    *out_handle = (uint32_t)block;

    return ESP_OK;

err:
    for (int i = 0; i < successful_cnt; i++) {
        esp_mmu_unmap((void *)vaddr_list[i]);
    }
    if (vaddr_list) {
        free(vaddr_list);
    }
    if (block) {
        free(block);
    }
    return ret;
}


void spi_flash_munmap(spi_flash_mmap_handle_t handle)
{
    esp_err_t ret = ESP_FAIL;
    mmap_block_t *block = (void *)handle;

    for (int i = 0; i < block->list_num; i++) {
        ret = esp_mmu_unmap((void *)block->vaddr_list[i]);
        if (ret == ESP_ERR_NOT_FOUND) {
            assert(0 && "invalid handle, or handle already unmapped");
        }
    }

    free(block->vaddr_list);
    free(block);
}


void spi_flash_mmap_dump(void)
{
    esp_mmu_map_dump_mapped_blocks(stdout);
}


uint32_t spi_flash_mmap_get_free_pages(spi_flash_mmap_memory_t memory)
{
    mmu_mem_caps_t caps = 0;
    if (memory == SPI_FLASH_MMAP_INST) {
        caps = MMU_MEM_CAP_EXEC | MMU_MEM_CAP_32BIT;
    } else {
        caps = MMU_MEM_CAP_READ | MMU_MEM_CAP_8BIT;
    }

    size_t len = 0;
    esp_mmu_map_get_max_consecutive_free_block_size(caps, MMU_TARGET_FLASH0, &len);
    return len / CONFIG_MMU_PAGE_SIZE;
}

static bool IRAM_ATTR is_page_mapped_in_cache(uint32_t phys_addr, const void **out_ptr)
{
    *out_ptr = NULL;
    mmu_mem_caps_t caps = 0;

    esp_err_t err = esp_mmu_paddr_find_caps(phys_addr, &caps);
    if (err == ESP_OK) {
    // On ESP32, we will always flush all, so always return true, and don't care the vaddr
#if !CONFIG_IDF_TARGET_ESP32
        uint32_t vaddr = 0;
        if (caps & MMU_MEM_CAP_EXEC) {
            mmu_hal_paddr_to_vaddr(0, phys_addr, MMU_TARGET_FLASH0, MMU_VADDR_INSTRUCTION, &vaddr);
        } else {
            mmu_hal_paddr_to_vaddr(0, phys_addr, MMU_TARGET_FLASH0, MMU_VADDR_DATA, &vaddr);
        }
        *out_ptr = (void *)vaddr;
#endif
        return true;
    }
    return false;
}

/* Validates if given flash address has corresponding cache mapping, if yes, flushes cache memories */
IRAM_ATTR bool spi_flash_check_and_flush_cache(size_t start_addr, size_t length)
{
    bool ret = false;
    /* align start_addr & length to full MMU pages */
    uint32_t page_start_addr = start_addr & ~(SPI_FLASH_MMU_PAGE_SIZE-1);
    length += (start_addr - page_start_addr);
    length = (length + SPI_FLASH_MMU_PAGE_SIZE - 1) & ~(SPI_FLASH_MMU_PAGE_SIZE-1);
    for (uint32_t addr = page_start_addr; addr < page_start_addr + length; addr += SPI_FLASH_MMU_PAGE_SIZE) {
        if (addr >= g_rom_flashchip.chip_size) {
            return false; /* invalid address */
        }

        const void *vaddr = NULL;
        if (is_page_mapped_in_cache(addr, &vaddr)) {
#if CONFIG_IDF_TARGET_ESP32
            cache_sync();
            return true;
#else // CONFIG_IDF_TARGET_ESP32
            if (vaddr != NULL) {
                cache_hal_invalidate_addr((uint32_t)vaddr, SPI_FLASH_MMU_PAGE_SIZE);
                ret = true;
            }
#endif // CONFIG_IDF_TARGET_ESP32

        }
    }
    return ret;
}
#endif //!CONFIG_SPI_FLASH_ROM_IMPL

#if !CONFIG_SPI_FLASH_ROM_IMPL || CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA
//The ROM implementation returns physical address of the PSRAM when the .text or .rodata is in the PSRAM.
//Always patch it when SPIRAM_FETCH_INSTRUCTIONS or SPIRAM_RODATA is set.
size_t spi_flash_cache2phys(const void *cached)
{
    if (cached == NULL) {
        return SPI_FLASH_CACHE2PHYS_FAIL;
    }

    esp_err_t ret = ESP_FAIL;
    uint32_t paddr = 0;
    mmu_target_t target = 0;

#if CONFIG_SPIRAM_FLASH_LOAD_TO_PSRAM //TODO: IDF-9049
    paddr = mmu_xip_psram_flash_vaddr_to_paddr(cached);
    //SPI_FLASH_CACHE2PHYS_FAIL is UINT32_MAX
    if (paddr != SPI_FLASH_CACHE2PHYS_FAIL) {
        return paddr;
    }
#endif

    ret = esp_mmu_vaddr_to_paddr((void *)cached, &paddr, &target);
    if (ret != ESP_OK) {
        return SPI_FLASH_CACHE2PHYS_FAIL;
    }

    int offset = 0;

#if !CONFIG_SPIRAM_FLASH_LOAD_TO_PSRAM
#if CONFIG_SPIRAM_RODATA
    if ((uint32_t)cached >= (uint32_t)&_rodata_reserved_start && (uint32_t)cached <= (uint32_t)&_rodata_reserved_end) {
        offset = rodata_flash2spiram_offset();
    }
#endif
#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
    if ((uint32_t)cached >= (uint32_t)&_instruction_reserved_start && (uint32_t)cached <= (uint32_t)&_instruction_reserved_end) {
        offset = instruction_flash2spiram_offset();
    }
#endif
#endif  //#if !CONFIG_SPIRAM_FLASH_LOAD_TO_PSRAM

    return paddr + offset * CONFIG_MMU_PAGE_SIZE;
}

const void * spi_flash_phys2cache(size_t phys_offs, spi_flash_mmap_memory_t memory)
{
    esp_err_t ret = ESP_FAIL;
    void *ptr = NULL;
    mmu_target_t target = MMU_TARGET_FLASH0;

    __attribute__((unused)) uint32_t phys_page = phys_offs / CONFIG_MMU_PAGE_SIZE;
#if !CONFIG_SPIRAM_FLASH_LOAD_TO_PSRAM
#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
    if (phys_page >= instruction_flash_start_page_get() && phys_page <= instruction_flash_end_page_get()) {
        target = MMU_TARGET_PSRAM0;
        phys_offs -= instruction_flash2spiram_offset() * CONFIG_MMU_PAGE_SIZE;
    }
#endif

#if CONFIG_SPIRAM_RODATA
    if (phys_page >= rodata_flash_start_page_get() && phys_page <= rodata_flash_start_page_get()) {
        target = MMU_TARGET_PSRAM0;
        phys_offs -= rodata_flash2spiram_offset() * CONFIG_MMU_PAGE_SIZE;
    }
#endif
#endif  //#if !CONFIG_SPIRAM_FLASH_LOAD_TO_PSRAM

    mmu_vaddr_t type = (memory == SPI_FLASH_MMAP_DATA) ? MMU_VADDR_DATA : MMU_VADDR_INSTRUCTION;
    ret = esp_mmu_paddr_to_vaddr(phys_offs, target, type, &ptr);
    if (ret == ESP_ERR_NOT_FOUND) {
        return NULL;
    }
    assert(ret == ESP_OK);
    return (const void *)ptr;
}
#endif //!CONFIG_SPI_FLASH_ROM_IMPL || CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA
