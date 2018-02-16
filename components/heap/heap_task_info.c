// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <multi_heap.h>
#include "multi_heap_internal.h"
#include "heap_private.h"
#include "esp_heap_task_info.h"

#ifdef CONFIG_HEAP_TASK_TRACKING

/*
 * Return per-task heap allocation totals and lists of blocks.
 *
 * For each task that has allocated memory from the heap, return totals for
 * allocations within regions matching one or more sets of capabilities.
 *
 * Optionally also return an array of structs providing details about each
 * block allocated by one or more requested tasks, or by all tasks.
 *
 * Returns the number of block detail structs returned.
 */
size_t heap_caps_get_per_task_info(heap_task_info_params_t *params)
{
    heap_t *reg;
    heap_task_block_t *blocks = params->blocks;
    size_t count = *params->num_totals;
    size_t remaining = params->max_blocks;

    // Clear out totals for any prepopulated tasks.
    if (params->totals) {
        for (size_t i = 0; i < count; ++i) {
            for (size_t type = 0; type < NUM_HEAP_TASK_CAPS; ++type) {
                params->totals[i].size[type] = 0;
                params->totals[i].count[type] = 0;
            }
        }
    }

    SLIST_FOREACH(reg, &registered_heaps, next) {
        multi_heap_handle_t heap = reg->heap;
        if (heap == NULL) {
            continue;
        }

        // Find if the capabilities of this heap region match on of the desired
        // sets of capabilities.
        uint32_t caps = get_all_caps(reg);
        uint32_t type;
        for (type = 0; type < NUM_HEAP_TASK_CAPS; ++type) {
            if ((caps & params->mask[type]) == params->caps[type]) {
                break;
            }
        }
        if (type == NUM_HEAP_TASK_CAPS) {
            continue;
        }

        multi_heap_block_handle_t b = multi_heap_get_first_block(heap);
        multi_heap_internal_lock(heap);
        for ( ; b ; b = multi_heap_get_next_block(heap, b)) {
            if (multi_heap_is_free(b)) {
                continue;
            }
            void *p = multi_heap_get_block_address(b);  // Safe, only arithmetic
            size_t bsize = multi_heap_get_allocated_size(heap, p); // Validates
            TaskHandle_t btask = (TaskHandle_t)multi_heap_get_block_owner(b);

            // Accumulate per-task allocation totals.
            if (params->totals) {
                size_t i;
                for (i = 0; i < count; ++i) {
                    if (params->totals[i].task == btask) {
                        break;
                    }
                }
                if (i < count) {
                    params->totals[i].size[type] += bsize;
                    params->totals[i].count[type] += 1;
                }
                else {
                    if (count < params->max_totals) {
                        params->totals[count].task = btask;
                        params->totals[count].size[type] = bsize;
                        params->totals[i].count[type] = 1;
                        ++count;
                    }
                }
            }

            // Return details about allocated blocks for selected tasks.
            if (blocks && remaining > 0) {
                if (params->tasks) {
                    size_t i;
                    for (i = 0; i < params->num_tasks; ++i) {
                        if (btask == params->tasks[i]) {
                            break;
                        }
                    }
                    if (i == params->num_tasks) {
                        continue;
                    }
                }
                blocks->task = btask;
                blocks->address = p;
                blocks->size = bsize;
                ++blocks;
                --remaining;
            }
        }
        multi_heap_internal_unlock(heap);
    }
    *params->num_totals = count;
    return params->max_blocks - remaining;
}

#endif // CONFIG_HEAP_TASK_TRACKING
