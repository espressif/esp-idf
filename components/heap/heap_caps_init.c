// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "heap_private.h"
#include <assert.h>
#include <string.h>
#include <sys/lock.h>

#include "esp_log.h"
#include "multi_heap.h"
#include "esp_heap_caps_init.h"
#include "soc/soc_memory_layout.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "heap_init";

/* Linked-list of registered heaps */
struct registered_heap_ll registered_heaps;

static void register_heap(heap_t *region)
{
    region->heap = multi_heap_register((void *)region->start, region->end - region->start);
    if (region->heap != NULL) {
        ESP_EARLY_LOGD(TAG, "New heap initialised at %p", region->heap);
    }
}

void heap_caps_enable_nonos_stack_heaps()
{
    heap_t *heap;
    SLIST_FOREACH(heap, &registered_heaps, next) {
        // Assume any not-yet-registered heap is
        // a nonos-stack heap
        if (heap->heap == NULL) {
            register_heap(heap);
            if (heap->heap != NULL) {
                multi_heap_set_lock(heap->heap, &heap->heap_mux);
            }
        }
    }
}

//Modify regions array to disable the given range of memory.
static void disable_mem_region(soc_memory_region_t *regions, intptr_t from, intptr_t to)
{
    //Align from and to on word boundaries
    from = from & ~3;
    to = (to + 3) & ~3;

    for (int i = 0; i < soc_memory_region_count; i++) {
        soc_memory_region_t *region = &regions[i];

        intptr_t regStart = region->start;
        intptr_t regEnd = region->start + region->size;
        if (regStart >= from && regEnd <= to) {
            //Entire region falls in the range. Disable entirely.
            regions[i].type = -1;
        } else if (regStart >= from && regEnd > to && regStart < to) {
            //Start of the region falls in the range. Modify address/len.
            intptr_t overlap = to - regStart;
            region->start += overlap;
            region->size -= overlap;
            if (region->iram_address) {
                region->iram_address += overlap;
            }
        } else if (regStart < from && regEnd > from && regEnd <= to) {
            //End of the region falls in the range. Modify length.
            region->size -= regEnd - from;
        } else if (regStart < from && regEnd > to) {
            //Range punches a hole in the region! We do not support this.
            ESP_EARLY_LOGE(TAG, "region %d: hole punching is not supported!", i);
            regions->type = -1; //Just disable memory region. That'll teach them!
        }
    }
}

/*
Warning: These variables are assumed to have the start and end of the data and iram
area used statically by the program, respectively. These variables are defined in the ld
file.
*/
extern int _data_start, _heap_start, _init_start, _iram_text_end;

/*
Initialize the heap allocator. We pass it a bunch of region descriptors, but we need to modify those first to accommodate for
the data as loaded by the bootloader.
ToDo: The regions are different when stuff like trace memory, BT, ... is used. Modify the regions struct on the fly for this.
Same with loading of apps. Same with using SPI RAM.
*/
void heap_caps_init()
{
    /* Copy the soc_memory_regions data to the stack, so we can
       manipulate it. */
    soc_memory_region_t regions[soc_memory_region_count];
    memcpy(regions, soc_memory_regions, sizeof(soc_memory_region_t)*soc_memory_region_count);

    //Disable the bits of memory where this code is loaded.
    disable_mem_region(regions, (intptr_t)&_data_start, (intptr_t)&_heap_start);           //DRAM used by bss/data static variables
    disable_mem_region(regions, (intptr_t)&_init_start, (intptr_t)&_iram_text_end);        //IRAM used by code

    // Disable all regions reserved on this SoC
    for (int i = 0; i < soc_reserved_region_count; i++) {
        disable_mem_region(regions, soc_reserved_regions[i].start,
                           soc_reserved_regions[i].end);
    }

    //The heap allocator will treat every region given to it as separate. In order to get bigger ranges of contiguous memory,
    //it's useful to coalesce adjacent regions that have the same type.

    for (int i = 1; i < soc_memory_region_count; i++) {
        soc_memory_region_t *a = &regions[i - 1];
        soc_memory_region_t *b = &regions[i];
        if (b->start == a->start + a->size && b->type == a->type ) {
            a->type = -1;
            b->start = a->start;
            b->size += a->size;
        }
    }

    /* Count the heaps left after merging */
    size_t num_heaps = 0;
    for (int i = 0; i < soc_memory_region_count; i++) {
        if (regions[i].type != -1) {
            num_heaps++;
        }
    }

    /* Start by allocating the registered heap data on the stack.

       Once we have a heap to copy it to, we will copy it to a heap buffer.
    */
    heap_t temp_heaps[num_heaps];
    size_t heap_idx = 0;

    ESP_EARLY_LOGI(TAG, "Initializing. RAM available for dynamic allocation:");
    for (int i = 0; i < soc_memory_region_count; i++) {
        soc_memory_region_t *region = &regions[i];
        const soc_memory_type_desc_t *type = &soc_memory_types[region->type];
        heap_t *heap = &temp_heaps[heap_idx];
        if (region->type == -1) {
            continue;
        }
        heap_idx++;
        assert(heap_idx <= num_heaps);

        memcpy(heap->caps, type->caps, sizeof(heap->caps));
        heap->start = region->start;
        heap->end = region->start + region->size;
        vPortCPUInitializeMutex(&heap->heap_mux);
        if (type->startup_stack) {
            /* Will be registered when OS scheduler starts */
            heap->heap = NULL;
        } else {
            register_heap(heap);
        }
        SLIST_NEXT(heap, next) = NULL;

        ESP_EARLY_LOGI(TAG, "At %08X len %08X (%d KiB): %s",
                       region->start, region->size, region->size / 1024, type->name);
    }

    assert(heap_idx == num_heaps);

    /* Allocate the permanent heap data that we'll use as a linked list at runtime.

       Allocate this part of data contiguously, even though it's a linked list... */
    assert(SLIST_EMPTY(&registered_heaps));

    heap_t *heaps_array = NULL;
    for (int i = 0; i < num_heaps; i++) {
        if (heap_caps_match(&temp_heaps[i], MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL)) {
            /* use the first DRAM heap which can fit the data */
            heaps_array = multi_heap_malloc(temp_heaps[i].heap, sizeof(heap_t) * num_heaps);
            if (heaps_array != NULL) {
                break;
            }
        }
    }
    assert(heaps_array != NULL); /* if NULL, there's not enough free startup heap space */

    memcpy(heaps_array, temp_heaps, sizeof(heap_t)*num_heaps);

    /* Iterate the heaps and set their locks, also add them to the linked list. */
    for (int i = 0; i < num_heaps; i++) {
        if (heaps_array[i].heap != NULL) {
            multi_heap_set_lock(heaps_array[i].heap, &heaps_array[i].heap_mux);
        }
        if (i == 0) {
            SLIST_INSERT_HEAD(&registered_heaps, &heaps_array[0], next);
        } else {
            SLIST_INSERT_AFTER(&heaps_array[i-1], &heaps_array[i], next);
        }
    }
}

esp_err_t heap_caps_add_region(intptr_t start, intptr_t end)
{
    if (start == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    for (int i = 0; i < soc_memory_region_count; i++) {
        const soc_memory_region_t *region = &soc_memory_regions[i];
        // Test requested start only as 'end' may be in a different region entry, assume 'end' has same caps
        if (region->start <= start && (region->start + region->size) > start) {
            const uint32_t *caps = soc_memory_types[region->type].caps;
            return heap_caps_add_region_with_caps(caps, start, end);
        }
    }

    return ESP_ERR_NOT_FOUND;
}

esp_err_t heap_caps_add_region_with_caps(const uint32_t caps[], intptr_t start, intptr_t end)
{
    esp_err_t err = ESP_FAIL;
    if (caps == NULL || start == 0 || end == 0 || end < start) {
        return ESP_ERR_INVALID_ARG;
    }

    //Check if region overlaps the start and/or end of an existing region. If so, the
    //region is invalid (or maybe added twice)
    heap_t *heap;
    SLIST_FOREACH(heap, &registered_heaps, next) {
        if ( start <= heap->start &&  heap->start <=end ) return ESP_FAIL;
        if ( start <= heap->end &&  heap->end <=end ) return ESP_FAIL;
    }

    heap_t *p_new = malloc(sizeof(heap_t));
    if (p_new == NULL) {
        err = ESP_ERR_NO_MEM;
        goto done;
    }
    memcpy(p_new->caps, caps, sizeof(p_new->caps));
    p_new->start = start;
    p_new->end = end;
    vPortCPUInitializeMutex(&p_new->heap_mux);
    p_new->heap = multi_heap_register((void *)start, end - start);
    SLIST_NEXT(p_new, next) = NULL;
    if (p_new->heap == NULL) {
        err = ESP_FAIL;
        goto done;
    }
    multi_heap_set_lock(p_new->heap, &p_new->heap_mux);

    /* (This insertion is atomic to registered_heaps, so
       we don't need to worry about thread safety for readers,
       only for writers. */
    static _lock_t registered_heaps_write_lock;
    _lock_acquire(&registered_heaps_write_lock);
    SLIST_INSERT_HEAD(&registered_heaps, p_new, next);
    _lock_release(&registered_heaps_write_lock);

    err = ESP_OK;

 done:
    if (err != ESP_OK) {
        free(p_new);
    }
    return err;
}
