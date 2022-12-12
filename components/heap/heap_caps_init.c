/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "heap_private.h"
#include <assert.h>
#include <string.h>
#include <sys/lock.h>

#include "esp_log.h"
#include "multi_heap.h"
#include "multi_heap_platform.h"
#include "esp_heap_caps_init.h"
#include "heap_memory_layout.h"

static const char *TAG = "heap_init";

/* Linked-list of registered heaps */
struct registered_heap_ll registered_heaps;

static void register_heap(heap_t *region)
{
    size_t heap_size = region->end - region->start;
    assert(heap_size <= HEAP_SIZE_MAX);
    region->heap = multi_heap_register((void *)region->start, heap_size);
    if (region->heap != NULL) {
        ESP_EARLY_LOGD(TAG, "New heap initialised at %p", region->heap);
    }
}

void heap_caps_enable_nonos_stack_heaps(void)
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

/* Initialize the heap allocator to use all of the memory not
   used by static data or reserved for other purposes
 */
void heap_caps_init(void)
{
#ifdef CONFIG_HEAP_TLSF_USE_ROM_IMPL
    extern void multi_heap_in_rom_init(void);
    multi_heap_in_rom_init();
#endif
    /* Get the array of regions that we can use for heaps
       (with reserved memory removed already.)
     */
    size_t num_regions = soc_get_available_memory_region_max_count();
    soc_memory_region_t regions[num_regions];
    num_regions = soc_get_available_memory_regions(regions);

    // the following for loop will calculate the number of possible heaps
    // based on how many regions were coalesed.
    size_t num_heaps = num_regions;

    //The heap allocator will treat every region given to it as separate. In order to get bigger ranges of contiguous memory,
    //it's useful to coalesce adjacent regions that have the same type.
    for (size_t i = 1; i < num_regions; i++) {
        soc_memory_region_t *a = &regions[i - 1];
        soc_memory_region_t *b = &regions[i];
        if (b->start == (intptr_t)(a->start + a->size) && b->type == a->type ) {
            a->type = -1;
            b->start = a->start;
            b->size += a->size;

            // remove one heap from the number of heaps as
            // 2 regions just got coalesed.
            num_heaps--;
        }
    }

    /* Start by allocating the registered heap data on the stack.

       Once we have a heap to copy it to, we will copy it to a heap buffer.
    */
    heap_t temp_heaps[num_heaps];
    size_t heap_idx = 0;

    ESP_EARLY_LOGI(TAG, "Initializing. RAM available for dynamic allocation:");
    for (size_t i = 0; i < num_regions; i++) {
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
        MULTI_HEAP_LOCK_INIT(&heap->heap_mux);
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
    for (size_t i = 0; i < num_heaps; i++) {
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
    for (size_t i = 0; i < num_heaps; i++) {
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

    for (size_t i = 0; i < soc_memory_region_count; i++) {
        const soc_memory_region_t *region = &soc_memory_regions[i];
        // Test requested start only as 'end' may be in a different region entry, assume 'end' has same caps
        if (region->start <= start && (intptr_t)(region->start + region->size) > start) {
            const uint32_t *caps = soc_memory_types[region->type].caps;
            return heap_caps_add_region_with_caps(caps, start, end);
        }
    }

    return ESP_ERR_NOT_FOUND;
}

/* This API is used for internal test purpose and hence its not marked as static */
bool heap_caps_check_add_region_allowed(intptr_t heap_start, intptr_t heap_end, intptr_t start, intptr_t end)
{
    /*
     *  We assume that in any region, the "start" must be strictly less than the end.
     *  Specially, the 3rd scenario can be allowed. For example, allocate memory from heap,
     *  then change the capability and call this function to create a new region for special
     *  application.
     *  This 'start = start' and 'end = end' scenario is incorrect because the same region
     *  cannot be added twice. In fact, registering the same memory region as a heap twice
     *  would cause a corruption and then an exception at runtime.
     *
     *  the existing heap region                                  s(tart)                e(nd)
     *                                                            |----------------------|
     *
     *  1.add region  (e1<s)                                |-----|                                      correct: bool condition_1 = end < heap_start;
     *
     *  2.add region  (s2<s && e2>s)                        |-----------------|                          wrong:   bool condition_2 = start < heap_start && end > heap_start;
     *                                                      |---------------------------------|          wrong
     *
     *  3.add region  (s3>=s && e3<e)                             |---------------|                      correct: bool condition_3 = start >= heap_start && end < heap_end;
     *                                                                  |--------------|                 correct
     *
     *  4.add region  (s4<e && e4>e)                              |------------------------|             wrong:   bool condition_4 = start < heap_end && end > heap_end;
     *                                                                  |---------------------|          wrong
     *
     *  5.add region  (s5>=e)                                                            |----|          correct: bool condition_5 = start >= heap_end;
     *
     * 6.add region (s6==s && e6==e)                              |----------------------|               wrong: bool condition_6 = start == heap_start && end == heap_end;
     */

    bool condition_2 = start < heap_start && end > heap_start;        // if true then region not allowed
    bool condition_4 = start < heap_end && end > heap_end;            // if true then region not allowed
    bool condition_6 = start == heap_start && end == heap_end;        // if true then region not allowed

    return !(condition_2 || condition_4 || condition_6);
}

esp_err_t heap_caps_add_region_with_caps(const uint32_t caps[], intptr_t start, intptr_t end)
{
    esp_err_t err = ESP_FAIL;
    if (caps == NULL || start == 0 || end == 0 || end <= start) {
        return ESP_ERR_INVALID_ARG;
    }

    //Check if region overlaps the start and/or end of an existing region. If so, the
    //region is invalid (or maybe added twice)
    heap_t *heap;
    SLIST_FOREACH(heap, &registered_heaps, next) {
        if (!heap_caps_check_add_region_allowed(heap->start, heap->end, start, end)) {
            ESP_EARLY_LOGD(TAG, "invalid overlap detected with existing heap region");
            return ESP_FAIL;
        }
    }

    heap_t *p_new = heap_caps_malloc(sizeof(heap_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    if (p_new == NULL) {
        err = ESP_ERR_NO_MEM;
        goto done;
    }
    memcpy(p_new->caps, caps, sizeof(p_new->caps));
    p_new->start = start;
    p_new->end = end;
    MULTI_HEAP_LOCK_INIT(&p_new->heap_mux);
    p_new->heap = multi_heap_register((void *)start, end - start);
    SLIST_NEXT(p_new, next) = NULL;
    if (p_new->heap == NULL) {
        err = ESP_ERR_INVALID_SIZE;
        goto done;
    }
    multi_heap_set_lock(p_new->heap, &p_new->heap_mux);

    /* (This insertion is atomic to registered_heaps, so
       we don't need to worry about thread safety for readers,
       only for writers. */
    static multi_heap_lock_t registered_heaps_write_lock = MULTI_HEAP_LOCK_STATIC_INITIALIZER;
    MULTI_HEAP_LOCK(&registered_heaps_write_lock);
    SLIST_INSERT_HEAD(&registered_heaps, p_new, next);
    MULTI_HEAP_UNLOCK(&registered_heaps_write_lock);

    err = ESP_OK;

 done:
    if (err != ESP_OK) {
        free(p_new);
    }
    return err;
}
