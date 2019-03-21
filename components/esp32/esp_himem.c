// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp32/spiram.h"
#include "esp32/rom/cache.h"
#include "sdkconfig.h"
#include "esp32/himem.h"
#include "soc/soc.h"
#include "esp_log.h"

/*
So, why does the API look this way and is so inflexible to not allow any maps beyond the full 32K chunks? Most of
it has to do with the fact that the cache works on the *virtual* addresses What this comes down to is that while it's
allowed to map a range of physical memory into the address space two times, there's no cache consistency between the
two regions.

This means that a write to region A may or may not show up, perhaps delayed, in region B, as it depends on
the time that the writeback to SPI RAM is done on A and the time before the corresponding cache line is invalidated
on B. Note that this goes for every 32-byte cache line: this implies that if a program writes to address X and Y within
A, the write to Y may show up before the write to X does.

It gets even worse when both A and B are written: theoretically, a write to a 32-byte cache line in A can be entirely
undone because of a write to a different addres in B that happens to be in the same 32-byte cache line.

Because of these reasons, we do not allow double mappings at all. This, however, has other implications that make
supporting ranges not really useful. Because the lack of double mappings, applications will need to do their own
management of mapped regions, meaning they will normally map in and out blocks at a time anyway, as mapping more
fluent regions would result in the chance of accidentally mapping two overlapping regions. As this is the case,
to keep the code simple, at the moment we just force these blocks to be equal to the 32K MMU page size. The API
itself does allow for more granular allocations, so if there's a pressing need for a more complex solution in the
future, we can do this.

Note: In the future, we can expand on this api to do a memcpy() between SPI RAM and (internal) memory using the SPI1
peripheral. This needs support for SPI1 to be in the SPI driver, however.
*/

#if CONFIG_SPIRAM_BANKSWITCH_ENABLE
#define SPIRAM_BANKSWITCH_RESERVE CONFIG_SPIRAM_BANKSWITCH_RESERVE
#else
#define SPIRAM_BANKSWITCH_RESERVE 0
#endif

#define CACHE_BLOCKSIZE (32*1024)

//Start of the virtual address range reserved for himem use
#define VIRT_HIMEM_RANGE_START (SOC_EXTRAM_DATA_LOW+(128-SPIRAM_BANKSWITCH_RESERVE)*CACHE_BLOCKSIZE)
//Start MMU block reserved for himem use
#define VIRT_HIMEM_RANGE_BLOCKSTART (128-SPIRAM_BANKSWITCH_RESERVE)
//Start physical block
#define PHYS_HIMEM_BLOCKSTART (128-SPIRAM_BANKSWITCH_RESERVE)

#define TAG "esp_himem"

#define HIMEM_CHECK(cond, str, err) if (cond) do {ESP_LOGE(TAG, "%s: %s", __FUNCTION__, str); return err; } while(0)

// Metadata for a block of physical RAM
typedef struct {
    unsigned int is_alloced: 1;
    unsigned int is_mapped: 1;
} ramblock_t;

//Metadata for a 32-K memory address range
typedef struct {
    unsigned int is_alloced: 1;
    unsigned int is_mapped: 1;
    unsigned int ram_block: 16;
} rangeblock_t;

static ramblock_t *s_ram_descriptor = NULL;
static rangeblock_t *s_range_descriptor = NULL;
static int s_ramblockcnt = 0;
static const int s_rangeblockcnt = SPIRAM_BANKSWITCH_RESERVE;

//Handle for a window of address space
typedef struct esp_himem_rangedata_t {
    int block_ct;
    int block_start;
} esp_himem_rangedata_t;

//Handle for a range of physical memory
typedef struct esp_himem_ramdata_t {
    int block_ct;
    uint16_t *block;
} esp_himem_ramdata_t;

static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

static inline int ramblock_idx_valid(int ramblock_idx)
{
    return (ramblock_idx >= 0 && ramblock_idx < s_ramblockcnt);
}

static inline int rangeblock_idx_valid(int rangeblock_idx)
{
    return (rangeblock_idx >= 0 && rangeblock_idx < s_rangeblockcnt);
}

static void set_bank(int virt_bank, int phys_bank, int ct)
{
    int r;
    r = cache_sram_mmu_set( 0, 0, SOC_EXTRAM_DATA_LOW + CACHE_BLOCKSIZE * virt_bank, phys_bank * CACHE_BLOCKSIZE, 32, ct );
    assert(r == 0);
    r = cache_sram_mmu_set( 1, 0, SOC_EXTRAM_DATA_LOW + CACHE_BLOCKSIZE * virt_bank, phys_bank * CACHE_BLOCKSIZE, 32, ct );
    assert(r == 0);
}

size_t esp_himem_get_phys_size()
{
    int paddr_start = (4096 * 1024) - (CACHE_BLOCKSIZE * SPIRAM_BANKSWITCH_RESERVE);
    return esp_spiram_get_size()-paddr_start;
}

size_t esp_himem_get_free_size()
{
    size_t ret=0;
    for (int i = 0; i < s_ramblockcnt; i++) {
        if (!s_ram_descriptor[i].is_alloced) ret+=CACHE_BLOCKSIZE;
    }
    return ret;
}

size_t esp_himem_reserved_area_size() {
    return CACHE_BLOCKSIZE * SPIRAM_BANKSWITCH_RESERVE;
}


void __attribute__((constructor)) esp_himem_init()
{
    if (SPIRAM_BANKSWITCH_RESERVE == 0) return;
    int maxram=esp_spiram_get_size();
    //catch double init
    HIMEM_CHECK(s_ram_descriptor != NULL, "already initialized", ); //Looks weird; last arg is empty so it expands to 'return ;'
    HIMEM_CHECK(s_range_descriptor != NULL, "already initialized", );
    //need to have some reserved banks
    HIMEM_CHECK(SPIRAM_BANKSWITCH_RESERVE == 0, "No banks reserved for himem", );
    //Start and end of physical reserved memory. Note it starts slightly under
    //the 4MiB mark as the reserved banks can't have an unity mapping to be used by malloc
    //anymore; we treat them as himem instead.
    int paddr_start = (4096 * 1024) - (CACHE_BLOCKSIZE * SPIRAM_BANKSWITCH_RESERVE);
    int paddr_end = maxram;
    s_ramblockcnt = ((paddr_end - paddr_start) / CACHE_BLOCKSIZE);
    //Allocate data structures
    s_ram_descriptor = calloc(sizeof(ramblock_t), s_ramblockcnt);
    s_range_descriptor = calloc(sizeof(rangeblock_t), SPIRAM_BANKSWITCH_RESERVE);
    if (s_ram_descriptor == NULL || s_range_descriptor == NULL) {
        ESP_EARLY_LOGE(TAG, "Cannot allocate memory for meta info. Not initializing!");
        free(s_ram_descriptor);
        free(s_range_descriptor);
        return;
    }
    ESP_EARLY_LOGI(TAG, "Initialized. Using last %d 32KB address blocks for bank switching on %d KB of physical memory.",
                SPIRAM_BANKSWITCH_RESERVE, (paddr_end - paddr_start)/1024);
}


//Allocate count not-necessarily consecutive physical RAM blocks, return numbers in blocks[]. Return
//true if blocks can be allocated, false if not.
static bool allocate_blocks(int count, uint16_t *blocks_out)
{
    int n = 0;
    for (int i = 0; i < s_ramblockcnt && n != count; i++) {
        if (!s_ram_descriptor[i].is_alloced) {
            blocks_out[n] = i;
            n++;
        }
    }
    if (n == count) {
        //All blocks could be allocated. Mark as in use.
        for (int i = 0; i < count; i++) {
            s_ram_descriptor[blocks_out[i]].is_alloced = true;
            assert(s_ram_descriptor[blocks_out[i]].is_mapped == false);
        }
        return true;
    } else {
        //Error allocating blocks
        return false;
    }
}


esp_err_t esp_himem_alloc(size_t size, esp_himem_handle_t *handle_out)
{
    if (size % CACHE_BLOCKSIZE != 0) {
        return ESP_ERR_INVALID_SIZE;
    }
    int blocks = size / CACHE_BLOCKSIZE;
    esp_himem_ramdata_t *r = calloc(sizeof(esp_himem_ramdata_t), 1);
    if (!r) {
        goto nomem;
    }
    r->block = calloc(sizeof(uint16_t), blocks);
    if (!r->block) {
        goto nomem;
    }
    portENTER_CRITICAL(&spinlock);
    int ok = allocate_blocks(blocks, r->block);
    portEXIT_CRITICAL(&spinlock);
    if (!ok) {
        goto nomem;
    }
    r->block_ct = blocks;
    *handle_out = r;
    return ESP_OK;
nomem:
    if (r) {
        free(r->block);
    }
    free(r);
    return ESP_ERR_NO_MEM;
}

esp_err_t esp_himem_free(esp_himem_handle_t handle)
{
    //Check if any of the blocks is still mapped; fail if this is the case.
    for (int i = 0; i < handle->block_ct; i++) {
        assert(ramblock_idx_valid(handle->block[i]));
        HIMEM_CHECK(s_ram_descriptor[handle->block[i]].is_mapped, "block in range still mapped", ESP_ERR_INVALID_ARG);
    }
    //Mark blocks as free
    portENTER_CRITICAL(&spinlock);
    for (int i = 0; i < handle->block_ct; i++) {
        s_ram_descriptor[handle->block[i]].is_alloced = false;
    }
    portEXIT_CRITICAL(&spinlock);

    //Free handle
    free(handle->block);
    free(handle);
    return ESP_OK;
}


esp_err_t esp_himem_alloc_map_range(size_t size, esp_himem_rangehandle_t *handle_out)
{
    HIMEM_CHECK(s_ram_descriptor == NULL, "Himem not available!", ESP_ERR_INVALID_STATE);
    HIMEM_CHECK(size % CACHE_BLOCKSIZE != 0, "requested size not aligned to blocksize", ESP_ERR_INVALID_SIZE);
    int blocks = size / CACHE_BLOCKSIZE;
    esp_himem_rangedata_t *r = calloc(sizeof(esp_himem_rangedata_t), 1);
    if (!r) {
        return ESP_ERR_NO_MEM;
    }
    r->block_ct = blocks;
    r->block_start = -1;
    int start_free = 0;
    portENTER_CRITICAL(&spinlock);
    for (int i = 0; i < s_rangeblockcnt; i++) {
        if (s_range_descriptor[i].is_alloced) {
            start_free = i + 1; //optimistically assume next block is free...
        } else if (i - start_free == blocks - 1) {
            //We found a span of blocks that's big enough to allocate the requested range in.
            r->block_start = start_free;
            break;
        }
    }

    if (r->block_start == -1) {
        //Couldn't find enough free blocks
        free(r);
        portEXIT_CRITICAL(&spinlock);
        return ESP_ERR_NO_MEM;
    }
    //Range is found. Mark the blocks as in use.
    for (int i = 0; i < blocks; i++) {
        s_range_descriptor[r->block_start + i].is_alloced = 1;
    }
    portEXIT_CRITICAL(&spinlock);
    //All done.
    *handle_out = r;
    return ESP_OK;
}

esp_err_t esp_himem_free_map_range(esp_himem_rangehandle_t handle)
{
    //Check if any of the blocks in the range have a mapping
    for (int i = 0; i < handle->block_ct; i++) {
        assert(rangeblock_idx_valid(handle->block_start + i));
        assert(s_range_descriptor[i + handle->block_start].is_alloced == 1); //should be, if handle is valid
        HIMEM_CHECK(s_range_descriptor[i + handle->block_start].is_mapped, "memory still mapped to range", ESP_ERR_INVALID_ARG);
    }
    //We should be good to free this. Mark blocks as free.
    portENTER_CRITICAL(&spinlock);
    for (int i = 0; i < handle->block_ct; i++) {
        s_range_descriptor[i + handle->block_start].is_alloced = 0;
    }
    portEXIT_CRITICAL(&spinlock);
    free(handle);
    return ESP_OK;
}


esp_err_t esp_himem_map(esp_himem_handle_t handle, esp_himem_rangehandle_t range, size_t ram_offset, size_t range_offset, size_t len, int flags, void **out_ptr)
{
    int ram_block = ram_offset / CACHE_BLOCKSIZE;
    int range_block = range_offset / CACHE_BLOCKSIZE;
    int blockcount = len / CACHE_BLOCKSIZE;
    HIMEM_CHECK(s_ram_descriptor == NULL, "Himem not available!", ESP_ERR_INVALID_STATE);
    //Offsets and length must be block-aligned
    HIMEM_CHECK(ram_offset % CACHE_BLOCKSIZE != 0, "ram offset not aligned to blocksize", ESP_ERR_INVALID_ARG);
    HIMEM_CHECK(range_offset % CACHE_BLOCKSIZE != 0, "range not aligned to blocksize", ESP_ERR_INVALID_ARG);
    HIMEM_CHECK(len % CACHE_BLOCKSIZE != 0, "length not aligned to blocksize", ESP_ERR_INVALID_ARG);
    //ram and range should be within allocated range
    HIMEM_CHECK(ram_block + blockcount > handle->block_ct, "args not in range of phys ram handle", ESP_ERR_INVALID_SIZE);
    HIMEM_CHECK(range_block + blockcount > range->block_ct, "args not in range of range handle", ESP_ERR_INVALID_SIZE);

    //Check if ram blocks aren't already mapped, and if memory range is unmapped
    for (int i = 0; i < blockcount; i++) {
        HIMEM_CHECK(s_ram_descriptor[handle->block[i + ram_block]].is_mapped, "ram already mapped", ESP_ERR_INVALID_STATE);
        HIMEM_CHECK(s_range_descriptor[range->block_start + i + range_block].is_mapped, "range already mapped", ESP_ERR_INVALID_STATE);
    }

    //Map and mark as mapped
    portENTER_CRITICAL(&spinlock);
    for (int i = 0; i < blockcount; i++) {
        assert(ramblock_idx_valid(handle->block[i + ram_block]));
        s_ram_descriptor[handle->block[i + ram_block]].is_mapped = 1;
        s_range_descriptor[range->block_start + i + range_block].is_mapped = 1;
        s_range_descriptor[range->block_start + i + range_block].ram_block = handle->block[i + ram_block];
    }
    portEXIT_CRITICAL(&spinlock);
    for (int i = 0; i < blockcount; i++) {
        set_bank(VIRT_HIMEM_RANGE_BLOCKSTART + range->block_start + i + range_block, handle->block[i + ram_block] + PHYS_HIMEM_BLOCKSTART, 1);
    }

    //Set out pointer
    *out_ptr = (void *)(VIRT_HIMEM_RANGE_START + (range->block_start + range_offset) * CACHE_BLOCKSIZE);
    return ESP_OK;
}

esp_err_t esp_himem_unmap(esp_himem_rangehandle_t range, void *ptr, size_t len)
{
    //Note: doesn't actually unmap, just clears cache and marks blocks as unmapped.
    //Future optimization: could actually lazy-unmap here: essentially, do nothing and only clear the cache when we re-use
    //the block for a different physical address.
    int range_offset = (uint32_t)ptr - VIRT_HIMEM_RANGE_START;
    int range_block = (range_offset / CACHE_BLOCKSIZE) - range->block_start;
    int blockcount = len / CACHE_BLOCKSIZE;
    HIMEM_CHECK(range_offset % CACHE_BLOCKSIZE != 0, "range offset not block-aligned", ESP_ERR_INVALID_ARG);
    HIMEM_CHECK(len % CACHE_BLOCKSIZE != 0, "map length not block-aligned", ESP_ERR_INVALID_ARG);
    HIMEM_CHECK(range_block + blockcount > range->block_ct, "range out of bounds for handle", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&spinlock);
    for (int i = 0; i < blockcount; i++) {
        int ramblock = s_range_descriptor[range->block_start + i + range_block].ram_block;
        assert(ramblock_idx_valid(ramblock));
        s_ram_descriptor[ramblock].is_mapped = 0;
        s_range_descriptor[range->block_start + i + range_block].is_mapped = 0;
    }
    esp_spiram_writeback_cache();
    portEXIT_CRITICAL(&spinlock);
    return ESP_OK;
}


