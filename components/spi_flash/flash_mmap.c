/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "sdkconfig.h"
#include "esp_bit_defs.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_rom_caps.h"
#include "hal/mmu_ll.h"
#include "hal/mmu_hal.h"
#include "hal/cache_hal.h"
#include "soc/soc_caps.h"
#if ESP_ROM_NEEDS_SET_CACHE_MMU_SIZE
#include "soc/mmu.h"
#endif

#include "esp_private/esp_mmu_map_private.h"
#include "esp_private/esp_cache_private.h"
#include "esp_mmu_map.h"
#include "esp_rom_spiflash.h"
#if CONFIG_SPIRAM
#include "esp_private/esp_psram_extram.h"
#if SOC_SPIRAM_XIP_SUPPORTED
#include "esp_private/mmu_psram_flash.h"
#endif
#endif

#if CONFIG_IDF_TARGET_ESP32
#include "esp_private/esp_cache_esp32_private.h"
#endif

#include "esp_private/cache_utils.h"
#include "spi_flash_mmap.h"
#include "esp_private/flash_mmap.h"

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
extern char _instruction_reserved_start;
extern char _instruction_reserved_end;
#endif

#if CONFIG_SPIRAM_RODATA
extern char _rodata_reserved_start;
extern char _rodata_reserved_end;
#endif


/* 0x1000000, 16MB */
#define FLASH_MMAP_ADDR_24BIT_MAX  (BIT(24))

#if !CONFIG_IDF_TARGET_ESP32
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mmap lock implementation.
// This lock allows external caller (flash driver) freezing the mmap flash pages when erasing.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
    _lock_t outer_mux;
    bool frozen;
    int freezing_wait_count;
    int acquired_count; //minus value means number of waiting callers
    SemaphoreHandle_t semphr_freeze;
    SemaphoreHandle_t semphr_acq;
} mmap_lock_t;

static mmap_lock_t s_mmap_lock;

static esp_err_t mmap_lock_init(void)
{
    esp_err_t ret = ESP_OK;
    _lock_init(&s_mmap_lock.outer_mux);
    s_mmap_lock.frozen = false;
    s_mmap_lock.freezing_wait_count = 0;
    s_mmap_lock.acquired_count = 0;

    s_mmap_lock.semphr_freeze = xSemaphoreCreateBinary();
    if (s_mmap_lock.semphr_freeze == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto err;
    }

    s_mmap_lock.semphr_acq = xSemaphoreCreateBinary();
    if (s_mmap_lock.semphr_acq == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto err;
    }
    return ESP_OK;
err:
    if (s_mmap_lock.semphr_freeze != NULL) {
        vSemaphoreDelete(s_mmap_lock.semphr_freeze);
        s_mmap_lock.semphr_freeze = NULL;
    }
    return ret;
}

static void mmap_lock_acquire(void)
{
    mmap_lock_t* const lock = &s_mmap_lock;
    bool wait = false;
    _lock_acquire(&lock->outer_mux);
    if (!lock->frozen) {
        assert(lock->acquired_count >= 0);
        lock->acquired_count++;
    } else {
        //Register one event
        assert(lock->acquired_count <= 0);
        lock->acquired_count--;
        wait = true;
    }
    _lock_release(&lock->outer_mux);

    if (wait) {
        //Wait for event
        xSemaphoreTake(lock->semphr_acq, portMAX_DELAY);
    }
}

static void mmap_lock_release(void)
{
    mmap_lock_t* const lock = &s_mmap_lock;
    bool wakeup_freeze = false;
    _lock_acquire(&lock->outer_mux);
    assert(lock->acquired_count > 0);
    assert(lock->frozen == false);
    lock->acquired_count--;
    if (lock->acquired_count == 0 && lock->freezing_wait_count > 0) {
        //All acquiring nodes have released, and there are waiting freezing requests
        //Go to the freezing state and wake up one freeze request
        lock->freezing_wait_count--;
        lock->frozen = true;
        wakeup_freeze = true;
    }
    _lock_release(&lock->outer_mux);

    if (wakeup_freeze) {
        //Wake up one freezing request
        xSemaphoreGive(lock->semphr_freeze);
    }
}

static void mmap_lock_freeze(void)
{
    mmap_lock_t* const lock = &s_mmap_lock;
    bool wait = false;
    _lock_acquire(&lock->outer_mux);
    if (lock->acquired_count > 0 || lock->frozen) {
        //If frozen, or already acquired, register one event and wait for it
        lock->freezing_wait_count++;
        wait = true;
    } else {
        lock->frozen = true;
    }
    _lock_release(&lock->outer_mux);

    if (wait) {
        //Wait for event trigger
        xSemaphoreTake(lock->semphr_freeze, portMAX_DELAY);
    }
}

static void mmap_lock_unfreeze(void)
{
    mmap_lock_t* const lock = &s_mmap_lock;
    bool wakeup_frozen = false;
    int wakeup_acq_count = 0;
    assert(lock->frozen);
    assert(lock->acquired_count <= 0);
    _lock_acquire(&lock->outer_mux);
    if (lock->acquired_count < 0) {
        //acquiring requests has higher priority than freezing request
        lock->frozen = false;
        lock->acquired_count = -lock->acquired_count;
        wakeup_acq_count = lock->acquired_count;
    } else if (lock->freezing_wait_count > 0) {
        lock->freezing_wait_count--;
        lock->frozen = true;
        wakeup_frozen = true;
    } else {
        //otherwise no one owns the lock
        lock->frozen = false;
    }
    _lock_release(&lock->outer_mux);

    if (wakeup_frozen) {
        //Wake one freezing request
        xSemaphoreGive(lock->semphr_freeze);
    } else {
        //Wake up all acquiring requests
        for (int i = 0; i < wakeup_acq_count; i++) {
            xSemaphoreGive(lock->semphr_acq);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Interfaces for mmap API and external caller (flash driver).
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//the count and the mapping table (esp_mmu_map) can only be touched when the mmap lock is acquired
static int s_mmap_remain_count; //number of mmap regions that are still in use

#define MMAP_CNT_INCREASE()  do { \
    assert(s_mmap_remain_count >= 0); \
    s_mmap_remain_count++; \
} while (0)
#define MMAP_CNT_DECREASE()  do { \
    s_mmap_remain_count--; \
    assert(s_mmap_remain_count >= 0); \
} while (0)

esp_err_t flash_mmap_lock_init(void)
{
    return mmap_lock_init();
}

bool flash_mmap_remain(void)
{
    return s_mmap_remain_count > 0;
}

void flash_mmap_lock_freeze(void)
{
    mmap_lock_freeze();
}

void flash_mmap_lock_unfreeze(void)
{
    mmap_lock_unfreeze();
}

#else //!CONFIG_IDF_TARGET_ESP32
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Empty interfaces for mmap APIs (ESP32 only).
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define mmap_lock_acquire() do {} while (0)
#define mmap_lock_release() do {} while (0)

#define MMAP_CNT_INCREASE() do {} while (0)
#define MMAP_CNT_DECREASE() do {} while (0)

#endif //!CONFIG_IDF_TARGET_ESP32


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mmap operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if !MMAP_ROM_IMPL_ENABLED
typedef struct mmap_block_t {
    uint32_t *vaddr_list;
    int list_num;
    uint32_t permanent; //When this flag is set, the mmap region will last for a very long time. Don't wait for the unmap and release the mmap lock immediately when exit mmap calls.
} mmap_block_t;

/* ROM and patch information
 * Latest: Add OS function to avoid concurrent access with erase/program when XIP from PSRAM
 * V1: added to ROM
 */
// Called from esp_flash_read_encrypted which is also a ROM function.
esp_err_t spi_flash_mmap(size_t src_addr, size_t size, spi_flash_mmap_flag_t flags,
                         const void** out_ptr, spi_flash_mmap_handle_t* out_handle)
{
#if !CONFIG_BOOTLOADER_CACHE_32BIT_ADDR_QUAD_FLASH && !CONFIG_BOOTLOADER_CACHE_32BIT_ADDR_OCTAL_FLASH
    if (src_addr >= FLASH_MMAP_ADDR_24BIT_MAX || size > FLASH_MMAP_ADDR_24BIT_MAX || src_addr > FLASH_MMAP_ADDR_24BIT_MAX - size) {
        ESP_LOGE("flash_mmap", "Address 0x%08x is out of range for 24bit flash mapping, see CONFIG_BOOTLOADER_CACHE_32BIT_ADDR_QUAD_FLASH and CONFIG_BOOTLOADER_CACHE_32BIT_ADDR_OCTAL_FLASH for more details", src_addr);
        return ESP_ERR_INVALID_ARG;
    }
#endif
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
    block->permanent = !(flags & SPI_FLASH_MMAP_FLAG_BLOCKS_WRITE);

    if (flags & SPI_FLASH_MMAP_FLAG_INST) {
        caps = MMU_MEM_CAP_EXEC | MMU_MEM_CAP_32BIT;
    } else {
        caps = MMU_MEM_CAP_READ | MMU_MEM_CAP_8BIT;
    }

    mmap_lock_acquire();
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
        mmap_lock_release();
        goto err;
    }
    MMAP_CNT_INCREASE();

    *out_ptr = ptr;
    *out_handle = (uint32_t)block;

    if (block->permanent) {
        //If the mmap is permanent, the lock is released without waiting for the unmap.
        mmap_lock_release();
    }
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

/* ROM and patch information
 * Latest: Add OS function to avoid concurrent access with erase/program
 * V1: added to ROM
 */
// Called from esp_flash_read_encrypted which is also a ROM function.
esp_err_t spi_flash_mmap_pages(const int *pages, size_t page_count, spi_flash_mmap_flag_t flags,
                         const void** out_ptr, spi_flash_mmap_handle_t* out_handle)
{
#if !CONFIG_BOOTLOADER_CACHE_32BIT_ADDR_QUAD_FLASH && !CONFIG_BOOTLOADER_CACHE_32BIT_ADDR_OCTAL_FLASH
    for (size_t i = 0; i < page_count; i++) {
        uint32_t phys = (uint32_t)pages[i] * CONFIG_MMU_PAGE_SIZE;
        if (phys >= FLASH_MMAP_ADDR_24BIT_MAX) {
            ESP_LOGE("flash_mmap", "Page %d (addr 0x%08x) is out of range for 24bit flash mapping", pages[i], phys);
            return ESP_ERR_INVALID_ARG;
        }
    }
#endif
    esp_err_t ret = ESP_FAIL;
    mmu_mem_caps_t caps = 0;
    mmap_block_t *block = NULL;
    uint32_t *vaddr_list = NULL;
    int successful_cnt = 0;
    bool mmap_lock_acquired = false;

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

    if (flags & SPI_FLASH_MMAP_FLAG_INST) {
        caps = MMU_MEM_CAP_EXEC | MMU_MEM_CAP_32BIT;
    } else {
        caps = MMU_MEM_CAP_READ | MMU_MEM_CAP_8BIT;
    }

    mmap_lock_acquire();
    mmap_lock_acquired = true;
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

    block->permanent = !(flags & SPI_FLASH_MMAP_FLAG_BLOCKS_WRITE);
    block->vaddr_list = vaddr_list;
    block->list_num = successful_cnt;

    MMAP_CNT_INCREASE();

    /**
     * We get a contiguous vaddr block, but may contain multiple esp_mmu handles.
     * The first handle vaddr is the start address of this contiguous vaddr block.
     */
    *out_ptr = (void *)vaddr_list[0];
    *out_handle = (uint32_t)block;

    if (block->permanent) {
        //If the mmap is permanent, the lock is released without waiting for the unmap.
        mmap_lock_release();
    }

    return ESP_OK;

err:
    for (int i = 0; i < successful_cnt; i++) {
        esp_mmu_unmap((void *)vaddr_list[i]);
    }
    if (vaddr_list) {
        free(vaddr_list);
    }
    if (mmap_lock_acquired) {
        mmap_lock_release();
    }
    if (block) {
        free(block);
    }
    return ret;
}

/* ROM and patch information
 * Latest: Add OS function to avoid concurrent access with erase/program
 * V1: added to ROM
 */
// Called from esp_flash_read_encrypted which is also a ROM function.
void spi_flash_munmap(spi_flash_mmap_handle_t handle)
{
    esp_err_t ret = ESP_FAIL;
    mmap_block_t *block = (void *)handle;

    if (block->permanent) {
        mmap_lock_acquire();
    }

    for (int i = 0; i < block->list_num; i++) {
        ret = esp_mmu_unmap((void *)block->vaddr_list[i]);
        if (ret == ESP_ERR_NOT_FOUND) {
            assert(0 && "invalid handle, or handle already unmapped");
        }
    }
    MMAP_CNT_DECREASE();

    mmap_lock_release();

    free(block->vaddr_list);
    free(block);
}

#else //!MMAP_ROM_IMPL_ENABLED
//Using ROM v1, which can't understand other flags like SPI_FLASH_MMAP_FLAG_BLOCKS_WRITE.
//Handle the BLOCKS_WRITE flag and lock in the wrapper, then call ROM impl.
//
//The "permanent" state (i.e. no BLOCKS_WRITE, lock released immediately after mmap) is encoded
//in BIT(31) of the returned handle, mirroring the mmap_block_t::permanent field in the IDF
//implementation above.
extern esp_err_t rom_spi_flash_mmap(size_t src_addr, size_t size, spi_flash_mmap_flag_t flags,
                         const void** out_ptr, spi_flash_mmap_handle_t* out_handle);
extern esp_err_t rom_spi_flash_mmap_pages(const int *pages, size_t page_count, spi_flash_mmap_flag_t flags,
                         const void** out_ptr, spi_flash_mmap_handle_t* out_handle);
extern void rom_spi_flash_munmap(spi_flash_mmap_handle_t handle);

#define ROM_MMAP_HANDLE_PERMANENT_BIT    BIT(31)

esp_err_t spi_flash_mmap(size_t src_addr, size_t size, spi_flash_mmap_flag_t flags,
                         const void** out_ptr, spi_flash_mmap_handle_t* out_handle)
{
    bool permanent = !(flags & SPI_FLASH_MMAP_FLAG_BLOCKS_WRITE);
    flags &= ~SPI_FLASH_MMAP_FLAG_BLOCKS_WRITE;

    mmap_lock_acquire();
    esp_err_t ret = rom_spi_flash_mmap(src_addr, size, flags, out_ptr, out_handle);
    if (ret != ESP_OK) {
        mmap_lock_release();
        return ret;
    }
    MMAP_CNT_INCREASE();

    if (permanent) {
        assert((*out_handle & ROM_MMAP_HANDLE_PERMANENT_BIT) == 0);
        *out_handle |= ROM_MMAP_HANDLE_PERMANENT_BIT;
        mmap_lock_release();
    }
    return ESP_OK;
}

esp_err_t spi_flash_mmap_pages(const int *pages, size_t page_count, spi_flash_mmap_flag_t flags,
                         const void** out_ptr, spi_flash_mmap_handle_t* out_handle)
{
    bool permanent = !(flags & SPI_FLASH_MMAP_FLAG_BLOCKS_WRITE);
    flags &= ~SPI_FLASH_MMAP_FLAG_BLOCKS_WRITE;

    mmap_lock_acquire();
    esp_err_t ret = rom_spi_flash_mmap_pages(pages, page_count, flags, out_ptr, out_handle);
    if (ret != ESP_OK) {
        mmap_lock_release();
        return ret;
    }
    MMAP_CNT_INCREASE();

    if (permanent) {
        assert((*out_handle & ROM_MMAP_HANDLE_PERMANENT_BIT) == 0);
        *out_handle |= ROM_MMAP_HANDLE_PERMANENT_BIT;
        mmap_lock_release();
    }
    return ESP_OK;
}

void spi_flash_munmap(spi_flash_mmap_handle_t handle)
{
    bool permanent = handle & ROM_MMAP_HANDLE_PERMANENT_BIT;
    spi_flash_mmap_handle_t rom_handle = handle & ~ROM_MMAP_HANDLE_PERMANENT_BIT;

    if (permanent) {
        mmap_lock_acquire();
    }

    rom_spi_flash_munmap(rom_handle);
    MMAP_CNT_DECREASE();

    mmap_lock_release();
}

#endif //!MMAP_ROM_IMPL_ENABLED

#if !MMAP_ROM_IMPL_ENABLED
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
                esp_cache_sync_ops_enter_critical_section();
                cache_hal_invalidate_addr((uint32_t)vaddr, SPI_FLASH_MMU_PAGE_SIZE);
                esp_cache_sync_ops_exit_critical_section();
                ret = true;
            }
#endif // CONFIG_IDF_TARGET_ESP32

        }
    }
    return ret;
}
#endif //!MMAP_ROM_IMPL_ENABLED

#if !MMAP_ROM_IMPL_ENABLED || CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA
/* ROM and patch information
 * Latest: Add the mapping from psram physical address to flash when CONFIG_SPIRAM_FETCH_INSTRUCTIONS or CONFIG_SPIRAM_RODATA enabled
 * V1 (Latest): added to ROM
 */
// The ROM implementation returns physical address of the PSRAM when the .text or .rodata is in the PSRAM.
// Patched when XIP from PSRAM (partially) enabled.
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

/* ROM and patch information
 * Latest: Add the mapping from flash physical address to psram when CONFIG_SPIRAM_FETCH_INSTRUCTIONS or CONFIG_SPIRAM_RODATA enabled
 * V1 (Latest): added to ROM
 */
// The ROM implementation takes physical address of the PSRAM when the .text or .rodata is in the PSRAM.
// Patched when XIP from PSRAM (partially) enabled.
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
    if (phys_page >= rodata_flash_start_page_get() && phys_page <= rodata_flash_end_page_get()) {
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
#endif //!MMAP_ROM_IMPL_ENABLED || CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA

ESP_STATIC_ASSERT(SPI_FLASH_MMAP_FLAG_DATA + SPI_FLASH_MMAP_FLAG_INST < SPI_FLASH_MMAP_FLAG_BLOCKS_WRITE, "spi_flash_mmap_memory_t not compatible with spi_flash_mmap_flag_t");
