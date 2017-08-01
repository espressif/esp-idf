// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <rom/spi_flash.h>
#include <rom/cache.h>
#include <soc/soc.h>
#include <soc/dport_reg.h>
#include "sdkconfig.h"
#include "esp_ipc.h"
#include "esp_attr.h"
#include "esp_spi_flash.h"
#include "esp_flash_encrypt.h"
#include "esp_log.h"
#include "cache_utils.h"

#ifndef NDEBUG
// Enable built-in checks in queue.h in debug builds
#define INVARIANTS
#endif
#include "rom/queue.h"

#define REGIONS_COUNT 4
#define PAGES_PER_REGION 64
#define INVALID_ENTRY_VAL 0x100
#define VADDR0_START_ADDR 0x3F400000
#define VADDR1_START_ADDR 0x40000000
#define VADDR1_FIRST_USABLE_ADDR 0x400D0000
#define PRO_IRAM0_FIRST_USABLE_PAGE ((VADDR1_FIRST_USABLE_ADDR - VADDR1_START_ADDR) / SPI_FLASH_MMU_PAGE_SIZE + 64)

/* Ensure pages in a region haven't been marked as written via
   spi_flash_mark_modified_region(). If the page has
   been written, flush the entire flash cache before returning.

   This ensures stale cache entries are never read after fresh calls
   to spi_flash_mmap(), while keeping the number of cache flushes to a
   minimum.

   Returns true if cache was flushed.
*/
static bool spi_flash_ensure_unmodified_region(size_t start_addr, size_t length);

typedef struct mmap_entry_{
    uint32_t handle;
    int page;
    int count;
    LIST_ENTRY(mmap_entry_) entries;
} mmap_entry_t;


static LIST_HEAD(mmap_entries_head, mmap_entry_) s_mmap_entries_head =
        LIST_HEAD_INITIALIZER(s_mmap_entries_head);
static uint8_t s_mmap_page_refcnt[REGIONS_COUNT * PAGES_PER_REGION] = {0};
static uint32_t s_mmap_last_handle = 0;


static void IRAM_ATTR spi_flash_mmap_init()
{
    if (s_mmap_page_refcnt[0] != 0) {
        return; /* mmap data already initialised */
    }

    for (int i = 0; i < REGIONS_COUNT * PAGES_PER_REGION; ++i) {
        uint32_t entry_pro = DPORT_PRO_FLASH_MMU_TABLE[i];
        uint32_t entry_app = DPORT_APP_FLASH_MMU_TABLE[i];
        if (entry_pro != entry_app) {
            // clean up entries used by boot loader
            entry_pro = DPORT_FLASH_MMU_TABLE_INVALID_VAL;
            DPORT_PRO_FLASH_MMU_TABLE[i] = DPORT_FLASH_MMU_TABLE_INVALID_VAL;
        }
        if ((entry_pro & INVALID_ENTRY_VAL) == 0 && (i == 0 || i == PRO_IRAM0_FIRST_USABLE_PAGE || entry_pro != 0)) {
            s_mmap_page_refcnt[i] = 1;
        } else {
            DPORT_PRO_FLASH_MMU_TABLE[i] = DPORT_FLASH_MMU_TABLE_INVALID_VAL;
            DPORT_APP_FLASH_MMU_TABLE[i] = DPORT_FLASH_MMU_TABLE_INVALID_VAL;
        }
    }
}

esp_err_t IRAM_ATTR spi_flash_mmap(size_t src_addr, size_t size, spi_flash_mmap_memory_t memory,
                         const void** out_ptr, spi_flash_mmap_handle_t* out_handle)
{
    esp_err_t ret;
    if (src_addr & 0xffff) {
        return ESP_ERR_INVALID_ARG;
    }
    if (src_addr + size > g_rom_flashchip.chip_size) {
        return ESP_ERR_INVALID_ARG;
    }
    // region which should be mapped
    int phys_page = src_addr / SPI_FLASH_MMU_PAGE_SIZE;
    int page_count = (size + SPI_FLASH_MMU_PAGE_SIZE - 1) / SPI_FLASH_MMU_PAGE_SIZE;
    //prepare a linear pages array to feed into spi_flash_mmap_pages
    int *pages=malloc(sizeof(int)*page_count);
    if (pages==NULL) {
        return ESP_ERR_NO_MEM;
    }
    for (int i = 0; i < page_count; i++) {
        pages[i] = phys_page+i;
    }
    ret=spi_flash_mmap_pages(pages, page_count, memory, out_ptr, out_handle);
    free(pages);
    return ret;
}

esp_err_t IRAM_ATTR spi_flash_mmap_pages(int *pages, size_t page_count, spi_flash_mmap_memory_t memory,
                         const void** out_ptr, spi_flash_mmap_handle_t* out_handle)
{
    esp_err_t ret;
    bool did_flush, need_flush = false;
    if (!page_count) {
        return ESP_ERR_INVALID_ARG;
    }
    for (int i = 0; i < page_count; i++) {
        if (pages[i] < 0 || pages[i]*SPI_FLASH_MMU_PAGE_SIZE >= g_rom_flashchip.chip_size) {
            return ESP_ERR_INVALID_ARG;
        }
    }
    mmap_entry_t* new_entry = (mmap_entry_t*) malloc(sizeof(mmap_entry_t));
    if (new_entry == 0) {
        return ESP_ERR_NO_MEM;
    }

    spi_flash_disable_interrupts_caches_and_other_cpu();

    did_flush = 0;
    for (int i = 0; i < page_count; i++) {
        if (spi_flash_ensure_unmodified_region(pages[i]*SPI_FLASH_MMU_PAGE_SIZE, SPI_FLASH_MMU_PAGE_SIZE)) {
            did_flush = 1;
        }
    }
    spi_flash_mmap_init();
    // figure out the memory region where we should look for pages
    int region_begin;   // first page to check
    int region_size;    // number of pages to check
    uint32_t region_addr;  // base address of memory region
    if (memory == SPI_FLASH_MMAP_DATA) {
        // Vaddr0
        region_begin = 0;
        region_size = 64;
        region_addr = VADDR0_START_ADDR;
    } else {
        // only part of VAddr1 is usable, so adjust for that
        region_begin = PRO_IRAM0_FIRST_USABLE_PAGE;
        region_size = 3 * 64 - region_begin;
        region_addr = VADDR1_FIRST_USABLE_ADDR;
    }
    if (region_size < page_count) {
        return ESP_ERR_NO_MEM;
    }
    // The following part searches for a range of MMU entries which can be used.
    // Algorithm is essentially naïve strstr algorithm, except that unused MMU
    // entries are treated as wildcards.
    int start;
    int end = region_begin + region_size - page_count;
    for (start = region_begin; start < end; ++start) {
        int pageno = 0;
        int pos;
        for (pos = start; pos < start + page_count; ++pos, ++pageno) {
            int table_val = (int) DPORT_PRO_FLASH_MMU_TABLE[pos];
            uint8_t refcnt = s_mmap_page_refcnt[pos]; 
            if (refcnt != 0 && table_val != pages[pageno]) {
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
        *out_handle = 0;
        *out_ptr = NULL;
        ret = ESP_ERR_NO_MEM;
    } else {
        // set up mapping using pages
        uint32_t pageno = 0;
        for (int i = start; i != start + page_count; ++i, ++pageno) {
            // sanity check: we won't reconfigure entries with non-zero reference count
            assert(s_mmap_page_refcnt[i] == 0 ||
                    (DPORT_PRO_FLASH_MMU_TABLE[i] == pages[pageno] &&
                     DPORT_APP_FLASH_MMU_TABLE[i] == pages[pageno]));
            if (s_mmap_page_refcnt[i] == 0) {
                if (DPORT_PRO_FLASH_MMU_TABLE[i] != pages[pageno] || DPORT_APP_FLASH_MMU_TABLE[i] != pages[pageno]) {
                    DPORT_PRO_FLASH_MMU_TABLE[i] = pages[pageno];
                    DPORT_APP_FLASH_MMU_TABLE[i] = pages[pageno];
                    need_flush = true;
                }
            }
            ++s_mmap_page_refcnt[i];
        }

        LIST_INSERT_HEAD(&s_mmap_entries_head, new_entry, entries);
        new_entry->page = start;
        new_entry->count = page_count;
        new_entry->handle = ++s_mmap_last_handle;
        *out_handle = new_entry->handle;
        *out_ptr = (void*) (region_addr + (start - region_begin) * SPI_FLASH_MMU_PAGE_SIZE);
        ret = ESP_OK;
    }

    /* This is a temporary fix for an issue where some
       cache reads may see stale data.

       Working on a long term fix that doesn't require invalidating
       entire cache.
    */
    if (!did_flush && need_flush) {
        Cache_Flush(0);
        Cache_Flush(1);
    }

    spi_flash_enable_interrupts_caches_and_other_cpu();
    if (*out_ptr == NULL) {
        free(new_entry);
    }
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
                    DPORT_PRO_FLASH_MMU_TABLE[i] = INVALID_ENTRY_VAL;
                    DPORT_APP_FLASH_MMU_TABLE[i] = INVALID_ENTRY_VAL;
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

void spi_flash_mmap_dump()
{
    spi_flash_mmap_init();
    mmap_entry_t* it;
    for (it = LIST_FIRST(&s_mmap_entries_head); it != NULL; it = LIST_NEXT(it, entries)) {
        printf("handle=%d page=%d count=%d\n", it->handle, it->page, it->count);
    }
    for (int i = 0; i < REGIONS_COUNT * PAGES_PER_REGION; ++i) {
        if (s_mmap_page_refcnt[i] != 0) {
            printf("page %d: refcnt=%d paddr=%d\n",
                    i, (int) s_mmap_page_refcnt[i], DPORT_PRO_FLASH_MMU_TABLE[i]);
        }
    }
}

/* 256-bit (up to 16MB of 64KB pages) bitset of all flash pages
   that have been written to since last cache flush.

   Before mmaping a page, need to flush caches if that page has been
   written to.

   Note: It's possible to do some additional performance tweaks to
   this algorithm, as we actually only need to flush caches if a page
   was first mmapped, then written to, then is about to be mmaped a
   second time. This is a fair bit more complex though, so unless
   there's an access pattern that this would significantly boost then
   it's probably not worth it.
*/
static uint32_t written_pages[256/32];

static bool update_written_pages(size_t start_addr, size_t length, bool mark);

void IRAM_ATTR spi_flash_mark_modified_region(size_t start_addr, size_t length)
{
    update_written_pages(start_addr, length, true);
}

static IRAM_ATTR bool spi_flash_ensure_unmodified_region(size_t start_addr, size_t length)
{
    return update_written_pages(start_addr, length, false);
}

/* generic implementation for the previous two functions */
static inline IRAM_ATTR bool update_written_pages(size_t start_addr, size_t length, bool mark)
{
    /* align start_addr & length to full MMU pages */
    uint32_t page_start_addr = start_addr & ~(SPI_FLASH_MMU_PAGE_SIZE-1);
    length += (start_addr - page_start_addr);
    length = (length + SPI_FLASH_MMU_PAGE_SIZE - 1) & ~(SPI_FLASH_MMU_PAGE_SIZE-1);
    for (uint32_t addr = page_start_addr; addr < page_start_addr + length; addr += SPI_FLASH_MMU_PAGE_SIZE) {
        int page = addr / SPI_FLASH_MMU_PAGE_SIZE;
        if (page >= 256) {
            return false; /* invalid address */
        }

        int idx = page / 32;
        uint32_t bit = 1 << (page % 32);

        if (mark) {
            written_pages[idx] |= bit;
        } else if (written_pages[idx] & bit) {
            /* it is tempting to write a version of this that only
               flushes each CPU's cache as needed. However this is
               tricky because mmaped memory can be used on un-pinned
               cores, or the pointer passed between CPUs.
            */
            Cache_Flush(0);
#ifndef CONFIG_FREERTOS_UNICORE
            Cache_Flush(1);
#endif
            bzero(written_pages, sizeof(written_pages));
            return true;
        }
    }
    return false;
}


uint32_t spi_flash_cache2phys(const void *cached)
{
    intptr_t c = (intptr_t)cached;
    size_t cache_page;
    if (c >= VADDR1_START_ADDR && c < VADDR1_FIRST_USABLE_ADDR) {
        /* IRAM address, doesn't map to flash */
        return SPI_FLASH_CACHE2PHYS_FAIL;
    }
    else if (c < VADDR1_FIRST_USABLE_ADDR) {
        /* expect cache is in DROM */
        cache_page = (c - VADDR0_START_ADDR) / SPI_FLASH_MMU_PAGE_SIZE;
    } else {
        /* expect cache is in IROM */
        cache_page = (c - VADDR1_START_ADDR) / SPI_FLASH_MMU_PAGE_SIZE + 64;
    }

    if (cache_page >= 256) {
        /* cached address was not in IROM or DROM */
        return SPI_FLASH_CACHE2PHYS_FAIL;
    }
    uint32_t phys_page = DPORT_PRO_FLASH_MMU_TABLE[cache_page];
    if (phys_page == INVALID_ENTRY_VAL) {
        /* page is not mapped */
        return SPI_FLASH_CACHE2PHYS_FAIL;
    }
    uint32_t phys_offs = phys_page * SPI_FLASH_MMU_PAGE_SIZE;
    return phys_offs | (c & (SPI_FLASH_MMU_PAGE_SIZE-1));
}


const void *spi_flash_phys2cache(uint32_t phys_offs, spi_flash_mmap_memory_t memory)
{
    uint32_t phys_page = phys_offs / SPI_FLASH_MMU_PAGE_SIZE;
    int start, end, page_delta;
    intptr_t base;

    if (memory == SPI_FLASH_MMAP_DATA) {
        start = 0;
        end = 64;
        base = VADDR0_START_ADDR;
        page_delta = 0;
    } else {
        start = PRO_IRAM0_FIRST_USABLE_PAGE;
        end = 256;
        base = VADDR1_START_ADDR;
        page_delta = 64;
    }

    for (int i = start; i < end; i++) {
        if (DPORT_PRO_FLASH_MMU_TABLE[i] == phys_page) {
            i -= page_delta;
            intptr_t cache_page =  base + (SPI_FLASH_MMU_PAGE_SIZE * i);
            return (const void *) (cache_page | (phys_offs & (SPI_FLASH_MMU_PAGE_SIZE-1)));
        }
    }
    return NULL;
}
