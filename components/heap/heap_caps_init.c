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
#include <esp_log.h>
#include <multi_heap.h>
#include <soc/soc_memory_layout.h>

static const char *TAG = "heap_init";

heap_t *registered_heaps;
size_t num_registered_heaps;

static void register_heap(heap_t *region)
{
    region->heap = multi_heap_register((void *)region->start, region->end - region->start);
    ESP_EARLY_LOGD(TAG, "New heap initialised at %p", region->heap);
    assert(region->heap);
}

void heap_caps_enable_nonos_stack_heaps()
{
    for (int i = 0; i < num_registered_heaps; i++) {
        // Assume any not-yet-registered heap is
        // a nonos-stack heap
        heap_t *heap = &registered_heaps[i];
        if (heap->heap == NULL) {
            register_heap(heap);
            multi_heap_set_lock(heap->heap, &heap->heap_mux);
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
    num_registered_heaps = 0;
    for (int i = 0; i < soc_memory_region_count; i++) {
        if (regions[i].type != -1) {
            num_registered_heaps++;
        }
    }

    /* Start by allocating the registered heap data on the stack.

       Once we have a heap to copy it to, we will copy it to a heap buffer.
    */
    multi_heap_handle_t first_heap = NULL;
    heap_t temp_heaps[num_registered_heaps];
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
        assert(heap_idx <= num_registered_heaps);

        heap->type = region->type;
        heap->start = region->start;
        heap->end = region->start + region->size;
        memcpy(heap->caps, type->caps, sizeof(heap->caps));
        vPortCPUInitializeMutex(&heap->heap_mux);

        ESP_EARLY_LOGI(TAG, "At %08X len %08X (%d KiB): %s",
                       region->start, region->size, region->size / 1024, type->name);

        if (type->startup_stack) {
            /* Will be registered when OS scheduler starts */
            heap->heap = NULL;
        } else {
            register_heap(heap);
            if (first_heap == NULL) {
                first_heap = heap->heap;
            }
        }
    }

    /* Allocate the permanent heap data that we'll use for runtime */
    assert(heap_idx == num_registered_heaps);
    registered_heaps = multi_heap_malloc(first_heap, sizeof(heap_t) * num_registered_heaps);
    memcpy(registered_heaps, temp_heaps, sizeof(heap_t)*num_registered_heaps);

    /* Now the heap_mux fields live on the heap, assign them */
    for (int i = 0; i < num_registered_heaps; i++) {
        if (registered_heaps[i].heap != NULL) {
            multi_heap_set_lock(registered_heaps[i].heap, &registered_heaps[i].heap_mux);
        }
    }
}

