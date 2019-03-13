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
#include "esp_spiram.h"

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
    DPORT_INTERRUPT_DISABLE();
    for (int i = 0; i < REGIONS_COUNT * PAGES_PER_REGION; ++i) {
        uint32_t entry_pro = DPORT_SEQUENCE_REG_READ((uint32_t)&DPORT_PRO_FLASH_MMU_TABLE[i]);
        uint32_t entry_app = DPORT_SEQUENCE_REG_READ((uint32_t)&DPORT_APP_FLASH_MMU_TABLE[i]);

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
    DPORT_INTERRUPT_RESTORE();
}

static void IRAM_ATTR get_mmu_region(spi_flash_mmap_memory_t memory, int* out_begin, int* out_size,uint32_t* region_addr)
{
    if (memory == SPI_FLASH_MMAP_DATA) {
        // Vaddr0
        *out_begin = 0;
        *out_size = 64;
        *region_addr = VADDR0_START_ADDR;
    } else {
        // only part of VAddr1 is usable, so adjust for that
        *out_begin = PRO_IRAM0_FIRST_USABLE_PAGE;
        *out_size = 3 * 64 - *out_begin;
        *region_addr = VADDR1_FIRST_USABLE_ADDR;
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
    // prepare a linear pages array to feed into spi_flash_mmap_pages
    int *pages = heap_caps_malloc(sizeof(int)*page_count, MALLOC_CAP_INTERNAL);
    if (pages == NULL) {
        return ESP_ERR_NO_MEM;
    }
    for (int i = 0; i < page_count; i++) {
        pages[i] = phys_page+i;
    }
    ret = spi_flash_mmap_pages(pages, page_count, memory, out_ptr, out_handle);
    free(pages);
    return ret;
}

esp_err_t IRAM_ATTR spi_flash_mmap_pages(const int *pages, size_t page_count, spi_flash_mmap_memory_t memory,
                         const void** out_ptr, spi_flash_mmap_handle_t* out_handle)
{
    esp_err_t ret;
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
        DPORT_INTERRUPT_DISABLE();
        for (pos = start; pos < start + page_count; ++pos, ++pageno) {
            int table_val = (int) DPORT_SEQUENCE_REG_READ((uint32_t)&DPORT_PRO_FLASH_MMU_TABLE[pos]);
            uint8_t refcnt = s_mmap_page_refcnt[pos]; 
            if (refcnt != 0 && table_val != pages[pageno]) {
                break;
            }
        }
        DPORT_INTERRUPT_RESTORE();
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
        DPORT_INTERRUPT_DISABLE();
        for (int i = start; i != start + page_count; ++i, ++pageno) {
            // sanity check: we won't reconfigure entries with non-zero reference count
            uint32_t entry_pro = DPORT_SEQUENCE_REG_READ((uint32_t)&DPORT_PRO_FLASH_MMU_TABLE[i]);
            uint32_t entry_app = DPORT_SEQUENCE_REG_READ((uint32_t)&DPORT_APP_FLASH_MMU_TABLE[i]);
            assert(s_mmap_page_refcnt[i] == 0 ||
                    (entry_pro == pages[pageno] &&
                     entry_app == pages[pageno]));
            if (s_mmap_page_refcnt[i] == 0) {
                if (entry_pro != pages[pageno] || entry_app != pages[pageno]) {
                    DPORT_PRO_FLASH_MMU_TABLE[i] = pages[pageno];
                    DPORT_APP_FLASH_MMU_TABLE[i] = pages[pageno];
                    need_flush = true;
                }
            }
            ++s_mmap_page_refcnt[i];
        }
        DPORT_INTERRUPT_RESTORE();
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
    if (need_flush) {
#if CONFIG_SPIRAM_SUPPORT
        esp_spiram_writeback_cache();
#endif
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

static void IRAM_ATTR NOINLINE_ATTR spi_flash_protected_mmap_init()
{
    spi_flash_disable_interrupts_caches_and_other_cpu();
    spi_flash_mmap_init();
    spi_flash_enable_interrupts_caches_and_other_cpu();
}

static uint32_t IRAM_ATTR NOINLINE_ATTR spi_flash_protected_read_mmu_entry(int index)
{
    uint32_t value;
    spi_flash_disable_interrupts_caches_and_other_cpu();
    value = DPORT_REG_READ((uint32_t)&DPORT_PRO_FLASH_MMU_TABLE[index]);
    spi_flash_enable_interrupts_caches_and_other_cpu();
    return value;
}

void spi_flash_mmap_dump()
{
    spi_flash_protected_mmap_init();

    mmap_entry_t* it;
    for (it = LIST_FIRST(&s_mmap_entries_head); it != NULL; it = LIST_NEXT(it, entries)) {
        printf("handle=%d page=%d count=%d\n", it->handle, it->page, it->count);
    }
    for (int i = 0; i < REGIONS_COUNT * PAGES_PER_REGION; ++i) {
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
    DPORT_INTERRUPT_DISABLE();
    for (int i = region_begin; i < region_begin + region_size; ++i) {
        if (s_mmap_page_refcnt[i] == 0 && DPORT_SEQUENCE_REG_READ((uint32_t)&DPORT_PRO_FLASH_MMU_TABLE[i]) == INVALID_ENTRY_VAL) {
            count++;
        }
    }
    DPORT_INTERRUPT_RESTORE();
    spi_flash_enable_interrupts_caches_and_other_cpu();
    return count;
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
    uint32_t phys_page = spi_flash_protected_read_mmu_entry(cache_page);
    if (phys_page == INVALID_ENTRY_VAL) {
        /* page is not mapped */
        return SPI_FLASH_CACHE2PHYS_FAIL;
    }
    uint32_t phys_offs = phys_page * SPI_FLASH_MMU_PAGE_SIZE;
    return phys_offs | (c & (SPI_FLASH_MMU_PAGE_SIZE-1));
}

const void *IRAM_ATTR spi_flash_phys2cache(uint32_t phys_offs, spi_flash_mmap_memory_t memory)
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
    spi_flash_disable_interrupts_caches_and_other_cpu();
    DPORT_INTERRUPT_DISABLE();
    for (int i = start; i < end; i++) {
        if (DPORT_SEQUENCE_REG_READ((uint32_t)&DPORT_PRO_FLASH_MMU_TABLE[i]) == phys_page) {
            i -= page_delta;
            intptr_t cache_page =  base + (SPI_FLASH_MMU_PAGE_SIZE * i);
            DPORT_INTERRUPT_RESTORE();
            spi_flash_enable_interrupts_caches_and_other_cpu();
            return (const void *) (cache_page | (phys_offs & (SPI_FLASH_MMU_PAGE_SIZE-1)));
        }
    }
    DPORT_INTERRUPT_RESTORE();
    spi_flash_enable_interrupts_caches_and_other_cpu();
    return NULL;
}

static bool IRAM_ATTR is_page_mapped_in_cache(uint32_t phys_page)
{
    int start[2], end[2];

    /* SPI_FLASH_MMAP_DATA */
    start[0] = 0;
    end[0] = 64;

    /* SPI_FLASH_MMAP_INST */
    start[1] = PRO_IRAM0_FIRST_USABLE_PAGE;
    end[1] = 256;

    DPORT_INTERRUPT_DISABLE();
    for (int j = 0; j < 2; j++) {
        for (int i = start[j]; i < end[j]; i++) {
            if (DPORT_SEQUENCE_REG_READ((uint32_t)&DPORT_PRO_FLASH_MMU_TABLE[i]) == phys_page) {
                DPORT_INTERRUPT_RESTORE();
                return true;
            }
        }
    }
    DPORT_INTERRUPT_RESTORE();
    return false;
}

/* Validates if given flash address has corresponding cache mapping, if yes, flushes cache memories */
IRAM_ATTR bool spi_flash_check_and_flush_cache(size_t start_addr, size_t length)
{
    /* align start_addr & length to full MMU pages */
    uint32_t page_start_addr = start_addr & ~(SPI_FLASH_MMU_PAGE_SIZE-1);
    length += (start_addr - page_start_addr);
    length = (length + SPI_FLASH_MMU_PAGE_SIZE - 1) & ~(SPI_FLASH_MMU_PAGE_SIZE-1);
    for (uint32_t addr = page_start_addr; addr < page_start_addr + length; addr += SPI_FLASH_MMU_PAGE_SIZE) {
        uint32_t page = addr / SPI_FLASH_MMU_PAGE_SIZE;
        if (page >= 256) {
            return false; /* invalid address */
        }

        if (is_page_mapped_in_cache(page)) {
#if CONFIG_SPIRAM_SUPPORT
            esp_spiram_writeback_cache();
#endif
            Cache_Flush(0);
#ifndef CONFIG_FREERTOS_UNICORE
            Cache_Flush(1);
#endif
            return true;
        }
    }
    return false;
}
