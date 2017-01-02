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
#include "esp_log.h"
#include "cache_utils.h"

#ifndef NDEBUG
// Enable built-in checks in queue.h in debug builds
#define INVARIANTS
#endif
#include "rom/queue.h"

#define REGIONS_COUNT 4
#define PAGES_PER_REGION 64
#define FLASH_PAGE_SIZE 0x10000
#define INVALID_ENTRY_VAL 0x100
#define VADDR0_START_ADDR 0x3F400000
#define VADDR1_START_ADDR 0x40000000
#define VADDR1_FIRST_USABLE_ADDR 0x400D0000
#define PRO_IRAM0_FIRST_USABLE_PAGE ((VADDR1_FIRST_USABLE_ADDR - VADDR1_START_ADDR) / FLASH_PAGE_SIZE + 64)


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
    for (int i = 0; i < REGIONS_COUNT * PAGES_PER_REGION; ++i) {
        uint32_t entry_pro = DPORT_PRO_FLASH_MMU_TABLE[i];
        uint32_t entry_app = DPORT_APP_FLASH_MMU_TABLE[i];
        if (entry_pro != entry_app) {
            // clean up entries used by boot loader
            entry_pro = 0;
            DPORT_PRO_FLASH_MMU_TABLE[i] = 0;
        }
        if ((entry_pro & 0x100) == 0 && (i == 0 || i == PRO_IRAM0_FIRST_USABLE_PAGE || entry_pro != 0)) {
            s_mmap_page_refcnt[i] = 1;
        }
    }
}

esp_err_t IRAM_ATTR spi_flash_mmap(size_t src_addr, size_t size, spi_flash_mmap_memory_t memory,
                         const void** out_ptr, spi_flash_mmap_handle_t* out_handle)
{
    esp_err_t ret;
    mmap_entry_t* new_entry = (mmap_entry_t*) malloc(sizeof(mmap_entry_t));
    if (new_entry == 0) {
        return ESP_ERR_NO_MEM;
    }
    if (src_addr & 0xffff) {
        return ESP_ERR_INVALID_ARG;
    }
    if (src_addr + size > g_rom_flashchip.chip_size) {
        return ESP_ERR_INVALID_ARG;
    }
    spi_flash_disable_interrupts_caches_and_other_cpu();
    if (s_mmap_page_refcnt[0] == 0) {
        spi_flash_mmap_init();
    }
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
        region_begin = VADDR1_FIRST_USABLE_ADDR;
        region_size = 3 * 64 - region_begin;
        region_addr = VADDR1_FIRST_USABLE_ADDR;
    }
    // region which should be mapped
    int phys_page = src_addr / FLASH_PAGE_SIZE;
    int page_count = (size + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;
    // The following part searches for a range of MMU entries which can be used.
    // Algorithm is essentially naïve strstr algorithm, except that unused MMU
    // entries are treated as wildcards.
    int start;
    int end = region_begin + region_size - page_count;
    for (start = region_begin; start < end; ++start) {
        int page = phys_page;
        int pos;
        for (pos = start; pos < start + page_count; ++pos, ++page) {
            int table_val = (int) DPORT_PRO_FLASH_MMU_TABLE[pos];
            uint8_t refcnt = s_mmap_page_refcnt[pos];
            if (refcnt != 0 && table_val != page) {
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
        // set up mapping using pages [start, start + page_count)
        uint32_t entry_val = (uint32_t) phys_page;
        for (int i = start; i != start + page_count; ++i, ++entry_val) {
            // sanity check: we won't reconfigure entries with non-zero reference count
            assert(s_mmap_page_refcnt[i] == 0 ||
                    (DPORT_PRO_FLASH_MMU_TABLE[i] == entry_val &&
                     DPORT_APP_FLASH_MMU_TABLE[i] == entry_val));
            if (s_mmap_page_refcnt[i] == 0) {
                DPORT_PRO_FLASH_MMU_TABLE[i] = entry_val;
                DPORT_APP_FLASH_MMU_TABLE[i] = entry_val;
            }
            ++s_mmap_page_refcnt[i];
        }

        LIST_INSERT_HEAD(&s_mmap_entries_head, new_entry, entries);
        new_entry->page = start;
        new_entry->count = page_count;
        new_entry->handle = ++s_mmap_last_handle;
        *out_handle = new_entry->handle;
        *out_ptr = (void*) (region_addr + start * FLASH_PAGE_SIZE);
        ret = ESP_OK;
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
    if (s_mmap_page_refcnt[0] == 0) {
        spi_flash_mmap_init();
    }
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
