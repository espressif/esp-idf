/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "soc/mmu.h"
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_memory_utils.h"
#include "spi_flash_mmap.h"
#include "esp_flash_encrypt.h"
#include "esp_log.h"
#include "esp_private/cache_utils.h"
#include "hal/mmu_ll.h"
#include "esp_rom_spiflash.h"

#if CONFIG_IDF_TARGET_ESP32
#include "soc/dport_reg.h"
#include "esp32/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/cache.h"
#include "soc/extmem_reg.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/cache.h"
#include "soc/extmem_reg.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32H4
#include "esp32h4/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/rom/cache.h"
#endif

#if CONFIG_SPIRAM
#include "esp_private/esp_psram_extram.h"
#include "esp_private/mmu_psram_flash.h"
#endif

#ifndef NDEBUG
// Enable built-in checks in queue.h in debug builds
#define INVARIANTS
#endif
#include "sys/queue.h"

#define IROM0_PAGES_NUM (SOC_MMU_IROM0_PAGES_END - SOC_MMU_IROM0_PAGES_START)
#define DROM0_PAGES_NUM (SOC_MMU_DROM0_PAGES_END - SOC_MMU_DROM0_PAGES_START)
#define PAGES_LIMIT ((SOC_MMU_IROM0_PAGES_END > SOC_MMU_DROM0_PAGES_END) ? SOC_MMU_IROM0_PAGES_END:SOC_MMU_DROM0_PAGES_END)
#define INVALID_PHY_PAGE(page_size)                ((page_size) - 1)

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
extern int _instruction_reserved_start;
extern int _instruction_reserved_end;
#endif

#if CONFIG_SPIRAM_RODATA
extern int _rodata_reserved_start;
extern int _rodata_reserved_end;
#endif

#if !CONFIG_SPI_FLASH_ROM_IMPL

typedef struct mmap_entry_{
    uint32_t handle;
    int page;
    int count;
    LIST_ENTRY(mmap_entry_) entries;
} mmap_entry_t;


static LIST_HEAD(mmap_entries_head, mmap_entry_) s_mmap_entries_head =
        LIST_HEAD_INITIALIZER(s_mmap_entries_head);
static uint8_t s_mmap_page_refcnt[SOC_MMU_REGIONS_COUNT * SOC_MMU_PAGES_PER_REGION] = {0};
static uint32_t s_mmap_last_handle = 0;


static void IRAM_ATTR spi_flash_mmap_init(void)
{
    if (s_mmap_page_refcnt[SOC_MMU_DROM0_PAGES_START] != 0) {
        return; /* mmap data already initialised */
    }
    for (int i = 0; i < SOC_MMU_REGIONS_COUNT * SOC_MMU_PAGES_PER_REGION; ++i) {
        uint32_t entry_pro = mmu_ll_read_entry(MMU_TABLE_CORE0, i);
#if !CONFIG_FREERTOS_UNICORE && CONFIG_IDF_TARGET_ESP32
        uint32_t entry_app = mmu_ll_read_entry(MMU_TABLE_CORE1, i);

        if (entry_pro != entry_app) {
            // clean up entries used by boot loader
            mmu_ll_set_entry_invalid(MMU_TABLE_CORE0, i);
        }
#endif
        bool entry_pro_invalid = mmu_ll_get_entry_is_invalid(MMU_TABLE_CORE0, i);
        if (!entry_pro_invalid && (i == SOC_MMU_DROM0_PAGES_START || i == SOC_MMU_PRO_IRAM0_FIRST_USABLE_PAGE || entry_pro != 0)) {
            s_mmap_page_refcnt[i] = 1;
        } else {
            mmu_ll_set_entry_invalid(MMU_TABLE_CORE0, i);
#if !CONFIG_FREERTOS_UNICORE && CONFIG_IDF_TARGET_ESP32
            mmu_ll_set_entry_invalid(MMU_TABLE_CORE1, i);
#endif
        }
    }
}

static void IRAM_ATTR get_mmu_region(spi_flash_mmap_memory_t memory, int* out_begin, int* out_size,uint32_t* region_addr)
{
    if (memory == SPI_FLASH_MMAP_DATA) {
        // Vaddr0
        *out_begin = SOC_MMU_DROM0_PAGES_START;
        *out_size = DROM0_PAGES_NUM;
        *region_addr = SOC_MMU_VADDR0_START_ADDR;
    } else {
        // only part of VAddr1 is usable, so adjust for that
        *out_begin = SOC_MMU_PRO_IRAM0_FIRST_USABLE_PAGE;
        *out_size = SOC_MMU_IROM0_PAGES_END - *out_begin;
        *region_addr = SOC_MMU_VADDR1_FIRST_USABLE_ADDR;
    }
}

esp_err_t IRAM_ATTR spi_flash_mmap(size_t src_addr, size_t size, spi_flash_mmap_memory_t memory,
                         const void** out_ptr, spi_flash_mmap_handle_t* out_handle)
{
    esp_err_t ret;
    if (src_addr & INVALID_PHY_PAGE(CONFIG_MMU_PAGE_SIZE)) {
        return ESP_ERR_INVALID_ARG;
    }
    if ((src_addr + size) > g_rom_flashchip.chip_size) {
        return ESP_ERR_INVALID_ARG;
    }
    // region which should be mapped
    int phys_page = src_addr / SPI_FLASH_MMU_PAGE_SIZE;
    int page_count = (size + SPI_FLASH_MMU_PAGE_SIZE - 1) / SPI_FLASH_MMU_PAGE_SIZE;
    // prepare a linear pages array to feed into spi_flash_mmap_pages
    int *pages = heap_caps_malloc(sizeof(int)*page_count, MALLOC_CAP_INTERNAL);
    if (pages == NULL) {
        return ESP_ERR_NO_MEM;
    }
    for (int i = 0; i < page_count; i++) {
        pages[i] = (phys_page+i);
    }
    ret = spi_flash_mmap_pages(pages, page_count, memory, out_ptr, out_handle);
    free(pages);
    return ret;
}

esp_err_t IRAM_ATTR spi_flash_mmap_pages(const int *pages, size_t page_count, spi_flash_mmap_memory_t memory,
                         const void** out_ptr, spi_flash_mmap_handle_t* out_handle)
{
    esp_err_t ret;
    const void* temp_ptr = *out_ptr = NULL;
    spi_flash_mmap_handle_t temp_handle = *out_handle = (spi_flash_mmap_handle_t)NULL;
    bool need_flush = false;
    if (!page_count) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!esp_ptr_internal(pages)) {
        return ESP_ERR_INVALID_ARG;
    }
    for (int i = 0; i < page_count; i++) {
        if (pages[i] < 0 || pages[i]*SPI_FLASH_MMU_PAGE_SIZE >= g_rom_flashchip.chip_size) {
            return ESP_ERR_INVALID_ARG;
        }
    }
    mmap_entry_t* new_entry = (mmap_entry_t*) heap_caps_malloc(sizeof(mmap_entry_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    if (new_entry == 0) {
        return ESP_ERR_NO_MEM;
    }

    spi_flash_disable_interrupts_caches_and_other_cpu();

    spi_flash_mmap_init();
    // figure out the memory region where we should look for pages
    int region_begin;   // first page to check
    int region_size;    // number of pages to check
    uint32_t region_addr;  // base address of memory region
    get_mmu_region(memory,&region_begin,&region_size,&region_addr);
    if (region_size < page_count) {
        spi_flash_enable_interrupts_caches_and_other_cpu();
        return ESP_ERR_NO_MEM;
    }
    // The following part searches for a range of MMU entries which can be used.
    // Algorithm is essentially naïve strstr algorithm, except that unused MMU
    // entries are treated as wildcards.
    int start;
    // the " + 1" is a fix when loop the MMU table pages, because the last MMU page
    // is valid as well if it have not been used
    int end = region_begin + region_size - page_count + 1;
    for (start = region_begin; start < end; ++start) {
        int pageno = 0;
        int pos;
        for (pos = start; pos < start + page_count; ++pos, ++pageno) {
            int table_val = (int) mmu_ll_read_entry(MMU_TABLE_CORE0, pos);
            uint8_t refcnt = s_mmap_page_refcnt[pos];
            if (refcnt != 0 && table_val != SOC_MMU_PAGE_IN_FLASH(pages[pageno])) {
                break;
            }
        }
        // whole mapping range matched, bail out
        if (pos - start == page_count) {
            break;
        }
    }
    // checked all the region(s) and haven't found anything?
    if (start == end) {
        ret = ESP_ERR_NO_MEM;
    } else {
        // set up mapping using pages
        uint32_t pageno = 0;
        for (int i = start; i != start + page_count; ++i, ++pageno) {
            // sanity check: we won't reconfigure entries with non-zero reference count
            uint32_t entry_pro = mmu_ll_read_entry(MMU_TABLE_CORE0, i);
#if !CONFIG_FREERTOS_UNICORE && CONFIG_IDF_TARGET_ESP32
            uint32_t entry_app = mmu_ll_read_entry(MMU_TABLE_CORE1, i);
#endif
            assert(s_mmap_page_refcnt[i] == 0 ||
                    (entry_pro == SOC_MMU_PAGE_IN_FLASH(pages[pageno])
#if !CONFIG_FREERTOS_UNICORE && CONFIG_IDF_TARGET_ESP32
                     && entry_app == SOC_MMU_PAGE_IN_FLASH(pages[pageno])
#endif
                    ));
            if (s_mmap_page_refcnt[i] == 0) {
                if (entry_pro != SOC_MMU_PAGE_IN_FLASH(pages[pageno])
#if !CONFIG_FREERTOS_UNICORE && CONFIG_IDF_TARGET_ESP32
                || entry_app != SOC_MMU_PAGE_IN_FLASH(pages[pageno])
#endif
                ) {
                    mmu_ll_write_entry(MMU_TABLE_CORE0, i, pages[pageno], 0);
#if !CONFIG_FREERTOS_UNICORE && CONFIG_IDF_TARGET_ESP32
                    mmu_ll_write_entry(MMU_TABLE_CORE1, i, pages[pageno], 0);
#endif

#if !CONFIG_IDF_TARGET_ESP32
                    Cache_Invalidate_Addr(region_addr + (i - region_begin) * SPI_FLASH_MMU_PAGE_SIZE, SPI_FLASH_MMU_PAGE_SIZE);
#endif
                    need_flush = true;
                }
            }
            ++s_mmap_page_refcnt[i];
        }
        LIST_INSERT_HEAD(&s_mmap_entries_head, new_entry, entries);
        new_entry->page = start;
        new_entry->count = page_count;
        new_entry->handle = ++s_mmap_last_handle;
        temp_handle = new_entry->handle;
        temp_ptr = (void*) (region_addr + (start - region_begin) * SPI_FLASH_MMU_PAGE_SIZE);
        ret = ESP_OK;
    }

    /* This is a temporary fix for an issue where some
       cache reads may see stale data.

       Working on a long term fix that doesn't require invalidating
       entire cache.
    */
    if (need_flush) {
#if CONFIG_IDF_TARGET_ESP32
#if CONFIG_SPIRAM
        esp_psram_extram_writeback_cache();
#endif // CONFIG_SPIRAM
        Cache_Flush(0);
#if !CONFIG_FREERTOS_UNICORE
        Cache_Flush(1);
#endif // !CONFIG_FREERTOS_UNICORE
#endif // CONFIG_IDF_TARGET_ESP32
    }

    spi_flash_enable_interrupts_caches_and_other_cpu();
    if (temp_ptr == NULL) {
        free(new_entry);
    }
    *out_ptr = temp_ptr;
    *out_handle = temp_handle;
    return ret;
}

void IRAM_ATTR spi_flash_munmap(spi_flash_mmap_handle_t handle)
{
    spi_flash_disable_interrupts_caches_and_other_cpu();
    mmap_entry_t* it;
    // look for handle in linked list
    for (it = LIST_FIRST(&s_mmap_entries_head); it != NULL; it = LIST_NEXT(it, entries)) {
        if (it->handle == handle) {
            // for each page, decrement reference counter
            // if reference count is zero, disable MMU table entry to
            // facilitate debugging of use-after-free conditions
            for (int i = it->page; i < it->page + it->count; ++i) {
                assert(s_mmap_page_refcnt[i] > 0);
                if (--s_mmap_page_refcnt[i] == 0) {
                    mmu_ll_set_entry_invalid(MMU_TABLE_CORE0, i);
#if !CONFIG_FREERTOS_UNICORE && CONFIG_IDF_TARGET_ESP32
                    mmu_ll_set_entry_invalid(MMU_TABLE_CORE1, i);
#endif
                }
            }
            LIST_REMOVE(it, entries);
            break;
        }
    }
    spi_flash_enable_interrupts_caches_and_other_cpu();
    if (it == NULL) {
        assert(0 && "invalid handle, or handle already unmapped");
    }
    free(it);
}

static void IRAM_ATTR NOINLINE_ATTR spi_flash_protected_mmap_init(void)
{
    spi_flash_disable_interrupts_caches_and_other_cpu();
    spi_flash_mmap_init();
    spi_flash_enable_interrupts_caches_and_other_cpu();
}

static uint32_t IRAM_ATTR NOINLINE_ATTR spi_flash_protected_read_mmu_entry(int index)
{
    uint32_t value;
    spi_flash_disable_interrupts_caches_and_other_cpu();
    value = mmu_ll_read_entry(MMU_TABLE_CORE0, index);
    spi_flash_enable_interrupts_caches_and_other_cpu();
    return value;
}

void spi_flash_mmap_dump(void)
{
    spi_flash_protected_mmap_init();

    mmap_entry_t* it;
    for (it = LIST_FIRST(&s_mmap_entries_head); it != NULL; it = LIST_NEXT(it, entries)) {
        printf("handle=%d page=%d count=%d\n", it->handle, it->page, it->count);
    }
    for (int i = 0; i < SOC_MMU_REGIONS_COUNT * SOC_MMU_PAGES_PER_REGION; ++i) {
        if (s_mmap_page_refcnt[i] != 0) {
            uint32_t paddr = spi_flash_protected_read_mmu_entry(i);
            printf("page %d: refcnt=%d paddr=%d\n", i, (int) s_mmap_page_refcnt[i], paddr);
        }
    }
}

uint32_t IRAM_ATTR spi_flash_mmap_get_free_pages(spi_flash_mmap_memory_t memory)
{
    spi_flash_disable_interrupts_caches_and_other_cpu();
    spi_flash_mmap_init();
    int count = 0;
    int region_begin;   // first page to check
    int region_size;    // number of pages to check
    uint32_t region_addr;  // base address of memory region
    get_mmu_region(memory,&region_begin,&region_size,&region_addr);
    for (int i = region_begin; i < region_begin + region_size; ++i) {
        bool entry_is_invalid = mmu_ll_get_entry_is_invalid(MMU_TABLE_CORE0, i);
        if (s_mmap_page_refcnt[i] == 0 && entry_is_invalid) {
            count++;
        }
    }
    spi_flash_enable_interrupts_caches_and_other_cpu();
    return count;
}

size_t spi_flash_cache2phys(const void *cached)
{
    intptr_t c = (intptr_t)cached;
    size_t cache_page;
    int offset = 0;
    if (c >= SOC_MMU_VADDR1_START_ADDR && c < SOC_MMU_VADDR1_FIRST_USABLE_ADDR) {
        /* IRAM address, doesn't map to flash */
        return SPI_FLASH_CACHE2PHYS_FAIL;
    }
    if (c < SOC_MMU_VADDR1_FIRST_USABLE_ADDR) {
        /* expect cache is in DROM */
        cache_page = (c - SOC_MMU_VADDR0_START_ADDR) / SPI_FLASH_MMU_PAGE_SIZE + SOC_MMU_DROM0_PAGES_START;
#if CONFIG_SPIRAM_RODATA
        if (c >= (uint32_t)&_rodata_reserved_start && c <= (uint32_t)&_rodata_reserved_end) {
            offset = rodata_flash2spiram_offset();
        }
#endif
    } else {
        /* expect cache is in IROM */
        cache_page = (c - SOC_MMU_VADDR1_START_ADDR) / SPI_FLASH_MMU_PAGE_SIZE + SOC_MMU_IROM0_PAGES_START;
#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
        if (c >= (uint32_t)&_instruction_reserved_start && c <= (uint32_t)&_instruction_reserved_end) {
            offset = instruction_flash2spiram_offset();
        }
#endif
    }

    if (cache_page >= PAGES_LIMIT) {
        /* cached address was not in IROM or DROM */
        return SPI_FLASH_CACHE2PHYS_FAIL;
    }
    uint32_t phys_page = spi_flash_protected_read_mmu_entry(cache_page);
    bool entry_is_invalid = mmu_ll_get_entry_is_invalid(MMU_TABLE_CORE0, cache_page);
    if (entry_is_invalid) {
        /* page is not mapped */
        return SPI_FLASH_CACHE2PHYS_FAIL;
    }
    uint32_t phys_offs = ((phys_page & SOC_MMU_ADDR_MASK) + offset) * SPI_FLASH_MMU_PAGE_SIZE;
    return phys_offs | (c & (SPI_FLASH_MMU_PAGE_SIZE-1));
}

const void *IRAM_ATTR spi_flash_phys2cache(size_t phys_offs, spi_flash_mmap_memory_t memory)
{
    uint32_t phys_page = phys_offs / SPI_FLASH_MMU_PAGE_SIZE;
    int start, end, page_delta;
    intptr_t base;

    if (memory == SPI_FLASH_MMAP_DATA) {
        start = SOC_MMU_DROM0_PAGES_START;
        end = SOC_MMU_DROM0_PAGES_END;
        base = SOC_MMU_VADDR0_START_ADDR;
        page_delta = SOC_MMU_DROM0_PAGES_START;
    } else {
        start = SOC_MMU_PRO_IRAM0_FIRST_USABLE_PAGE;
        end = SOC_MMU_IROM0_PAGES_END;
        base = SOC_MMU_VADDR1_START_ADDR;
        page_delta = SOC_MMU_IROM0_PAGES_START;
    }
    spi_flash_disable_interrupts_caches_and_other_cpu();
    for (int i = start; i < end; i++) {
        uint32_t mmu_value = mmu_ll_read_entry(MMU_TABLE_CORE0, i);
#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
        if (phys_page >= instruction_flash_start_page_get() && phys_page <= instruction_flash_end_page_get()) {
            if (mmu_value & MMU_ACCESS_SPIRAM) {
                mmu_value += instruction_flash2spiram_offset();
                mmu_value = (mmu_value & SOC_MMU_ADDR_MASK) | MMU_ACCESS_FLASH;
            }
        }
#endif
#if CONFIG_SPIRAM_RODATA
        if (phys_page >= rodata_flash_start_page_get() && phys_page <= rodata_flash_start_page_get()) {
            if (mmu_value & MMU_ACCESS_SPIRAM) {
                mmu_value += rodata_flash2spiram_offset();
                mmu_value = (mmu_value & SOC_MMU_ADDR_MASK) | MMU_ACCESS_FLASH;
            }
        }
#endif
        if (mmu_value == SOC_MMU_PAGE_IN_FLASH(phys_page)) {
            i -= page_delta;
            intptr_t cache_page =  base + (SPI_FLASH_MMU_PAGE_SIZE * i);
            spi_flash_enable_interrupts_caches_and_other_cpu();
            return (const void *) (cache_page | (phys_offs & (SPI_FLASH_MMU_PAGE_SIZE-1)));
        }
    }
    spi_flash_enable_interrupts_caches_and_other_cpu();
    return NULL;
}

static bool IRAM_ATTR is_page_mapped_in_cache(uint32_t phys_page, const void **out_ptr)
{
    int start[2], end[2];

    *out_ptr = NULL;

    /* SPI_FLASH_MMAP_DATA */
    start[0] = SOC_MMU_DROM0_PAGES_START;
    end[0] = SOC_MMU_DROM0_PAGES_END;

    /* SPI_FLASH_MMAP_INST */
    start[1] = SOC_MMU_PRO_IRAM0_FIRST_USABLE_PAGE;
    end[1] = SOC_MMU_IROM0_PAGES_END;

    for (int j = 0; j < 2; j++) {
        for (int i = start[j]; i < end[j]; i++) {
            uint32_t entry_pro = mmu_ll_read_entry(MMU_TABLE_CORE0, i);
            if (entry_pro == SOC_MMU_PAGE_IN_FLASH(phys_page)) {
#if !CONFIG_IDF_TARGET_ESP32
                if (j == 0) { /* SPI_FLASH_MMAP_DATA */
                    *out_ptr = (const void *)(SOC_MMU_VADDR0_START_ADDR + SPI_FLASH_MMU_PAGE_SIZE * (i - start[0]));
                } else { /* SPI_FLASH_MMAP_INST */
                    *out_ptr = (const void *)(SOC_MMU_VADDR1_FIRST_USABLE_ADDR + SPI_FLASH_MMU_PAGE_SIZE * (i - start[1]));
                }
#endif
                return true;
            }
        }
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
        uint32_t page = addr / SPI_FLASH_MMU_PAGE_SIZE;
        // TODO: IDF-4969
        if (page >= 256) {
            return false; /* invalid address */
        }

        const void *vaddr = NULL;
        if (is_page_mapped_in_cache(page, &vaddr)) {
#if CONFIG_IDF_TARGET_ESP32
#if CONFIG_SPIRAM
            esp_psram_extram_writeback_cache();
#endif
            Cache_Flush(0);
#ifndef CONFIG_FREERTOS_UNICORE
            Cache_Flush(1);
#endif
            return true;
#else // CONFIG_IDF_TARGET_ESP32
            if (vaddr != NULL) {
                Cache_Invalidate_Addr((uint32_t)vaddr, SPI_FLASH_MMU_PAGE_SIZE);
                ret = true;
            }
#endif // CONFIG_IDF_TARGET_ESP32

        }
    }
    return ret;
}
#endif //!CONFIG_SPI_FLASH_ROM_IMPL
