/*
 * SPDX-FileCopyrightText: 2018-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <inttypes.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <multi_heap.h>
#include "multi_heap_internal.h"
#include "heap_private.h"
#include "esp_heap_task_info.h"
#include "esp_heap_task_info_internal.h"
#include "heap_memory_layout.h"
#include "esp_log.h"

#ifdef CONFIG_HEAP_TASK_TRACKING

const static char *TAG = "heap_task_tracking";

static SemaphoreHandle_t s_task_tracking_mutex = NULL;
static StaticSemaphore_t s_task_tracking_mutex_buf;


typedef struct alloc_stats {
    heap_task_block_t alloc_stat;
    STAILQ_ENTRY(alloc_stats) next_alloc_stat;
} alloc_stats_t;

/**
 * @brief Internal singly linked list used to gather information of the heap used
 * by a given task.
 */
typedef struct heap_stats {
    multi_heap_handle_t heap;
    heap_stat_t heap_stat;
    STAILQ_HEAD(alloc_stats_ll, alloc_stats) allocs_stats;
    STAILQ_ENTRY(heap_stats) next_heap_stat;
} heap_stats_t;

/** @brief Internal singly linked list used to gather information on all created
 * tasks since startup.
 */
typedef struct task_stats {
    task_stat_t task_stat;
    STAILQ_HEAD(heap_stats_ll, heap_stats) heaps_stats;
    SLIST_ENTRY(task_stats) next_task_info;
} task_info_t;


static SLIST_HEAD(task_stats_ll, task_stats) task_stats = SLIST_HEAD_INITIALIZER(task_stats);

FORCE_INLINE_ATTR heap_t* find_biggest_heap(void)
{
    heap_t *heap = NULL;
    heap_t *biggest_heap = NULL;
    SLIST_FOREACH(heap, &registered_heaps, next) {
        /* In the case where we are currently looking for the biggest heap during startup,
         * before the scheduler stated, all the memory regions marked as startup stacks will
         * be NULL here. As such, they must be ignored. After boot up, this statement will
         * never be true. */
        if (heap->heap == NULL) {
            /* Continue the loop */
        } else if (biggest_heap == NULL) {
            biggest_heap = heap;
        } else if ((biggest_heap->end - biggest_heap->start) < (heap->end - heap->start)) {
            biggest_heap = heap;
        }
    }
    return biggest_heap;
}

/**
 * @brief Create a new alloc stats entry object
 *
 * @param heap_stats The heap statistics of the heap used for the allocation
 * @param task_handle The task handler of the task which performed the allocation
 * @param ptr The address of the allocation
 * @param size The size of the allocation
 */
static HEAP_IRAM_ATTR void create_new_alloc_stats_entry(heap_stats_t *heap_stats, alloc_stats_t *alloc_stats, TaskHandle_t task_handle, void *ptr, size_t size)
{
    // init the list of allocs with a new entry in heap_stats->allocs_stats. No need
    // to memset the memory since all field will be set later in the function.
    if (!alloc_stats) {
        // find the heap with the most available free memory to store the statistics
        heap_t *heap_used_for_alloc = find_biggest_heap();

        alloc_stats = multi_heap_malloc(heap_used_for_alloc->heap, sizeof(alloc_stats_t));
        if (!alloc_stats) {
            ESP_LOGE(TAG, "Could not allocate memory to add new task statistics");
            return;
        }
    }

    alloc_stats->alloc_stat.task = task_handle;
    alloc_stats->alloc_stat.address = ptr;
    alloc_stats->alloc_stat.size = size;

    STAILQ_INSERT_TAIL(&heap_stats->allocs_stats, alloc_stats, next_alloc_stat);
}

/**
 * @brief Create a new heap stats entry object
 *
 * @param task_stats The task statistics of the task that triggered the allocation
 * @param used_heap Information about the heap used for the allocation
 * @param caps The caps of the heap used for the allocation
 * @param size The size of the allocation
 */
static HEAP_IRAM_ATTR void create_new_heap_stats_entry(task_info_t *task_stats, heap_t *used_heap, void *ptr, uint32_t caps, size_t size)
{
    // find the heap with the most available free memory to store the statistics
    heap_t *heap_used_for_alloc = find_biggest_heap();

    // init the list of heap with a new entry in task_stats->heaps_stats. No need
    // to memset the memory since all field will be set later in the function.
    heap_stats_t *heap_stats = multi_heap_malloc(heap_used_for_alloc->heap, sizeof(heap_stats_t));
    if (!heap_stats) {
        ESP_LOGE(TAG, "Could not allocate memory to add new task statistics");
        return;
    }

    // create the alloc stats for the new heap entry
    STAILQ_INIT(&heap_stats->allocs_stats);

    task_stats->task_stat.heap_count += 1;

    heap_stats->heap = used_heap->heap;
    heap_stats->heap_stat.name = used_heap->name;
    heap_stats->heap_stat.size = used_heap->end - used_heap->start;
    heap_stats->heap_stat.caps = caps;
    heap_stats->heap_stat.current_usage = size;
    heap_stats->heap_stat.peak_usage = size;
    heap_stats->heap_stat.alloc_count = 1;
    heap_stats->heap_stat.alloc_stat = NULL; // this will be used to point at the user defined array of alloc_stat

    STAILQ_INSERT_TAIL(&task_stats->heaps_stats, heap_stats, next_heap_stat);

    create_new_alloc_stats_entry(heap_stats, NULL, task_stats->task_stat.handle, ptr, size);
}

/**
 * @brief Create a new task info entry in task_stats if the tasks allocating memory is not in task_stats already.
 *
 * @param heap The heap by the task to allocate memory
 * @param task_handle The task handle of the task allocating memory
 * @param task_stats The task entry in task_stats. If NULL, the task allocating memory is allocating for the first time
 * @param ptr The address of the allocation
 * @param size The size of the allocation
 * @param caps The ORED caps of the heap used for the allocation
 */
static HEAP_IRAM_ATTR void create_new_task_stats_entry(heap_t *used_heap, TaskHandle_t task_handle, task_info_t *task_info, void *ptr, size_t size, uint32_t caps)
{
    // If task_info passed as parameter is NULL, it means the this task is doing
    // its first allocation. Add the task entry to task_info and add heap_stats
    // to this new task_info entry.
    // If task_info is not NULL, it means that the task already allocated memory
    // but now it is allocating in a new heap for the first time. Don't add a new
    // task entry to task_info but add a new heap_stats to the task_info
    if (!task_info) {
        // find the heap with the most available free memory to store the statistics
        heap_t *heap_used_for_alloc = find_biggest_heap();

        // create the task_stats entry. No need to memset since all fields are set later
        task_info = multi_heap_malloc(heap_used_for_alloc->heap, sizeof(task_info_t));
        if (!task_info) {
            ESP_LOGE(TAG, "Could not allocate memory to add new task statistics");
            return;
        }

        // create the heap stats for the new task entry
        STAILQ_INIT(&task_info->heaps_stats);

        task_info->task_stat.handle = task_handle;
        task_info->task_stat.is_alive = true;
        task_info->task_stat.overall_peak_usage = size;
        task_info->task_stat.overall_current_usage = size;
        task_info->task_stat.heap_count = 0;
        task_info->task_stat.heap_stat = NULL; // this will be used to point at the user defined array of heap_stat
        if (task_handle == 0x00) {
            char task_name[] = "Pre-scheduler";
            strcpy(task_info->task_stat.name, task_name);
        } else {
            strcpy(task_info->task_stat.name, pcTaskGetName(task_handle));
        }

        // Add the new / first task_info in the list (sorted by decreasing address).
        // The decreasing order is chosen because the task_handle 0x00000000 is used for pre-scheduler
        // operations and therefore need to appear last so it is not parsed when trying to find a suitable
        // task to update the stats from.
        if (SLIST_EMPTY(&task_stats) || task_info->task_stat.handle >= SLIST_FIRST(&task_stats)->task_stat.handle) {
            // the list is empty, or the new task handler is at a higher address than the one from the first item
            SLIST_INSERT_HEAD(&task_stats, task_info, next_task_info);
        } else {
            // the new task handle is at a lower address than the first item in the list, go through the list to
            // properly insert the new item
            task_info_t *cur_task_info = NULL;
            task_info_t *prev_task_info = NULL;
            SLIST_FOREACH(cur_task_info, &task_stats, next_task_info) {
                if (cur_task_info->task_stat.handle < task_info->task_stat.handle) {
                    SLIST_INSERT_AFTER(prev_task_info, task_info, next_task_info);
                    break;
                } else {
                    prev_task_info = cur_task_info;
                }
            }
            // here should be a last case handling: new task info as a task handle address smaller than all existing
            // items in the list. But this is case is impossible given that the pre-scheduler allocations always
            // happen first and the task handle defaults to 0x00000000 for the pre-scheduler so it will always be
            // last in the list.
        }
    }

    create_new_heap_stats_entry(task_info, used_heap, ptr, caps, size);
}

#if !CONFIG_HEAP_TRACK_DELETED_TASKS
/**
 * @brief Delete an entry from the list of task statistics
 *
 * @param task_info The task statistics to delete from the list of task statistics
 */
static HEAP_IRAM_ATTR void delete_task_info_entry(task_info_t *task_info)
{
    if (task_info == NULL) {
        return;
    }

    heap_stats_t *current_heap_stat = STAILQ_FIRST(&task_info->heaps_stats);
    heap_stats_t *prev_heap_stat = NULL;

    // pointer used to free the memory of the statistics
    heap_t *containing_heap = NULL;

    // remove all entries from task_info->heaps_stats and free the memory
    while(current_heap_stat != NULL) {
        prev_heap_stat = current_heap_stat;
        current_heap_stat = STAILQ_NEXT(current_heap_stat, next_heap_stat);

        /* remove all entries from heap_stats->allocs_stats */
        alloc_stats_t *alloc_stat = NULL;
        while ((alloc_stat = STAILQ_FIRST( &prev_heap_stat->allocs_stats)) != NULL) {
            STAILQ_REMOVE(&prev_heap_stat->allocs_stats, alloc_stat, alloc_stats, next_alloc_stat);
            containing_heap = find_containing_heap(alloc_stat);
            // prev_heap_stat must be allocated somewhere
            if (containing_heap != NULL) {
                multi_heap_free(containing_heap->heap, alloc_stat);
            }
        }
        if (STAILQ_EMPTY(&prev_heap_stat->allocs_stats)) {
            STAILQ_REMOVE(&task_info->heaps_stats, prev_heap_stat, heap_stats, next_heap_stat);
            containing_heap = find_containing_heap(prev_heap_stat);
            // prev_heap_stat must be allocated somewhere
            if (containing_heap != NULL) {
                multi_heap_free(containing_heap->heap, prev_heap_stat);
            }
        }
    }
    if (STAILQ_EMPTY(&task_info->heaps_stats)) {
        // remove task_info from task_stats (and free the memory)
        SLIST_REMOVE(&task_stats, task_info, task_stats, next_task_info);
        containing_heap = find_containing_heap(task_info);
        if (containing_heap != NULL) {
            multi_heap_free(containing_heap->heap, task_info);
        }
    }
}
#endif // !CONFIG_HEAP_TRACK_DELETED_TASKS

HEAP_IRAM_ATTR void heap_caps_update_per_task_info_alloc(heap_t *heap, void *ptr, size_t size, uint32_t caps)
{
    if (s_task_tracking_mutex == NULL) {
        s_task_tracking_mutex = xSemaphoreCreateMutexStatic(&s_task_tracking_mutex_buf);
        assert(s_task_tracking_mutex);
    }

    TaskHandle_t task_handle = xTaskGetCurrentTaskHandle();
    task_info_t *task_info = NULL;

    xSemaphoreTake(s_task_tracking_mutex, portMAX_DELAY);
    /* find the task in the list and update the overall stats */
    SLIST_FOREACH(task_info, &task_stats, next_task_info) {
        if (task_info->task_stat.handle == task_handle && task_info->task_stat.is_alive) {
            task_info->task_stat.overall_current_usage += size;
            if (task_info->task_stat.overall_current_usage > task_info->task_stat.overall_peak_usage) {
                task_info->task_stat.overall_peak_usage = task_info->task_stat.overall_current_usage;
            }

            heap_stats_t *heap_stats = NULL;
            /* find the heap in the list and update the overall stats */
            STAILQ_FOREACH(heap_stats, &task_info->heaps_stats, next_heap_stat) {
                if (heap_stats->heap == heap->heap) {
                    heap_stats->heap_stat.current_usage += size;
                    heap_stats->heap_stat.alloc_count++;
                    if (heap_stats->heap_stat.current_usage > heap_stats->heap_stat.peak_usage) {
                        heap_stats->heap_stat.peak_usage = heap_stats->heap_stat.current_usage;
                    }

                    /* add the alloc info to the list */
                    create_new_alloc_stats_entry(heap_stats, NULL, task_handle, ptr, size);

                    xSemaphoreGive(s_task_tracking_mutex);
                    return;
                }
            }
            break;
        }

        // since the list of task info is sorted by decreasing size, if the current task info
        // has a smaller task handle address than the one we are checking against, we can be sure
        // the task handle will not be found in the list, and we can break the loop.
        if (task_info->task_stat.handle < task_handle) {
            task_info = NULL;
            break;
        }
    }

    // No task entry was found OR no heap in the task entry was found.
    // Add the info to the list (either new task stats or new heap stat if task_info not NULL)
    create_new_task_stats_entry(heap, task_handle, task_info, ptr, size, caps);

    xSemaphoreGive(s_task_tracking_mutex);
}

HEAP_IRAM_ATTR void heap_caps_update_per_task_info_realloc(heap_t *heap, void *old_ptr, void *new_ptr,
                                                           size_t old_size, TaskHandle_t old_task,
                                                           size_t new_size, uint32_t caps)
{
    TaskHandle_t task_handle = xTaskGetCurrentTaskHandle();
    bool task_in_list = false;
    task_info_t *task_info = NULL;
    alloc_stats_t *alloc_stat = NULL;

    xSemaphoreTake(s_task_tracking_mutex, portMAX_DELAY);
    SLIST_FOREACH(task_info, &task_stats, next_task_info) {
        if (task_info->task_stat.handle == old_task) {
            heap_stats_t *heap_stats = NULL;
            task_info->task_stat.overall_current_usage -= old_size;
            STAILQ_FOREACH(heap_stats, &task_info->heaps_stats, next_heap_stat) {
                if (heap_stats->heap == heap->heap) {
                    heap_stats->heap_stat.current_usage -= old_size;
                    heap_stats->heap_stat.alloc_count--;

                    /* remove the alloc from the list. The updated alloc stats are added later
                     * in the function */
                    STAILQ_FOREACH(alloc_stat, &heap_stats->allocs_stats, next_alloc_stat) {
                        if (alloc_stat->alloc_stat.address == old_ptr) {
                            STAILQ_REMOVE(&heap_stats->allocs_stats, alloc_stat, alloc_stats, next_alloc_stat);
                            /* keep the memory used to store alloc_stat since we will fill it with new alloc
                                * info later in the function */
                            break;
                        }
                    }
                    break;
                }
            }
        }

        if (task_info->task_stat.handle == task_handle && task_info->task_stat.is_alive) {
            heap_stats_t *heap_stats = NULL;
            task_info->task_stat.overall_current_usage += new_size;
            STAILQ_FOREACH(heap_stats, &task_info->heaps_stats, next_heap_stat) {
                if (heap_stats->heap == heap->heap) {
                    heap_stats->heap_stat.current_usage  += new_size;
                    heap_stats->heap_stat.alloc_count++;
                    if (heap_stats->heap_stat.current_usage > heap_stats->heap_stat.peak_usage) {
                        heap_stats->heap_stat.peak_usage = heap_stats->heap_stat.current_usage;
                    }

                    create_new_alloc_stats_entry(heap_stats, alloc_stat, task_handle, new_ptr, new_size);
                    break;
                }
            }
            task_in_list = true;
        }

        if (task_info->task_stat.overall_current_usage > task_info->task_stat.overall_peak_usage) {
            task_info->task_stat.overall_peak_usage = task_info->task_stat.overall_current_usage;
        }
    }

    if (!task_in_list) {
        // No task entry was found OR no heap in the task entry was found.
        // Add the info to the list (either new task stats or new heap stat if task_info not NULL)
        create_new_task_stats_entry(heap, task_handle, task_info, new_ptr, new_size, caps);
    }

    xSemaphoreGive(s_task_tracking_mutex);
}

HEAP_IRAM_ATTR void heap_caps_update_per_task_info_free(heap_t *heap, void *ptr)
{
    void *block_owner_ptr = MULTI_HEAP_REMOVE_BLOCK_OWNER_OFFSET(ptr);
    TaskHandle_t task_handle = MULTI_HEAP_GET_BLOCK_OWNER(block_owner_ptr);
    if (!task_handle) {
        return;
    }

    task_info_t *task_info = NULL;
#if !CONFIG_HEAP_TRACK_DELETED_TASKS
    task_info_t *task_info_to_delete = NULL;
#endif // !CONFIG_HEAP_TRACK_DELETED_TASKS

    xSemaphoreTake(s_task_tracking_mutex, portMAX_DELAY);
    /* find the matching task */
    SLIST_FOREACH(task_info, &task_stats, next_task_info) {
        /* check all tasks (alive and deleted) since the free can come from any tasks,
         * not necessarily the one which allocated the memory. */
        if (task_info->task_stat.handle == task_handle) {
            heap_stats_t *heap_stats = NULL;
            alloc_stats_t *alloc_stat = NULL;
            /* find the matching heap */
            STAILQ_FOREACH(heap_stats, &task_info->heaps_stats, next_heap_stat) {
                if(heap_stats->heap == heap->heap) {
                    /* find the matching allocation and remove it from the list*/
                    STAILQ_FOREACH(alloc_stat, &heap_stats->allocs_stats, next_alloc_stat) {
                        if (alloc_stat->alloc_stat.address == ptr) {
                            STAILQ_REMOVE(&heap_stats->allocs_stats, alloc_stat, alloc_stats, next_alloc_stat);
                            /* keep the memory used to store alloc_stat since we will fill it with new alloc
                             * info later in the function */
                            break;
                        }
                    }

                    if (alloc_stat != NULL) {
                        heap_stats->heap_stat.alloc_count--;
                        heap_stats->heap_stat.current_usage -= alloc_stat->alloc_stat.size;
                        task_info->task_stat.overall_current_usage -= alloc_stat->alloc_stat.size;
                    }
                }
            }

            /* free the memory used to store alloc_stat */
            heap_t *containing_heap = find_containing_heap(alloc_stat);
            // task_stats must be allocated somewhere
            if (containing_heap != NULL) {
                multi_heap_free(containing_heap->heap, alloc_stat);
            }
        }

        // when a task is deleted, esp_caps_free is called to delete the TCB of the task from vTaskDelete.
        // Try to make a TaskHandle out of ptr and compare it to the list of tasks in task_stats.
        // If one task_info contains the newly made TaskHandle from ptr it means that esp_caps_free
        // was indeed called from vTaskDelete. We can then update the task_stats by marking the corresponding
        // task as deleted.
        if (task_info->task_stat.handle == ptr) {
            // we found the task info from the task that is being deleted.
            task_info->task_stat.is_alive = false;
#if !CONFIG_HEAP_TRACK_DELETED_TASKS
            task_info_to_delete = task_info;
#endif // !CONFIG_HEAP_TRACK_DELETED_TASKS
        }
    }

#if !CONFIG_HEAP_TRACK_DELETED_TASKS
    // remove the entry related to the task that was just deleted.
    if (task_info_to_delete != NULL) {
        delete_task_info_entry(task_info_to_delete);
    }
#endif // !CONFIG_HEAP_TRACK_DELETED_TASKS

    xSemaphoreGive(s_task_tracking_mutex);
}

esp_err_t heap_caps_get_all_task_stat(heap_all_tasks_stat_t *tasks_stat)
{
    if (tasks_stat == NULL ||
        (tasks_stat->stat_arr == NULL && tasks_stat->task_count != 0) ||
        (tasks_stat->heap_stat_start == NULL && tasks_stat->heap_count != 0) ||
        (tasks_stat->alloc_stat_start == NULL && tasks_stat->alloc_count != 0)) {
        return ESP_ERR_INVALID_ARG;
    }

    size_t task_index = 0;
    size_t heap_index = 0;
    size_t alloc_index = 0;
    task_info_t *task_info = NULL;

    xSemaphoreTake(s_task_tracking_mutex, portMAX_DELAY);
    SLIST_FOREACH(task_info, &task_stats, next_task_info) {
        // If there is no more task stat entries available in tasks_stat->stat_arr
        // break the loop and return the function.
        if (task_index >= tasks_stat->task_count) {
            break;
        }
        memcpy(tasks_stat->stat_arr + task_index, &task_info->task_stat, sizeof(task_stat_t));
        task_stat_t *current_task_stat = tasks_stat->stat_arr + task_index;
        task_index++;

        // If no more heap stat entries in the array are available, just proceed
        // with filling task stats but skip filling info on heap stat and alloc stat.
        if (heap_index + task_info->task_stat.heap_count > tasks_stat->heap_count) {
            current_task_stat->heap_stat = NULL;
            continue;
        }

        // set the pointer where the heap info for the given task will
        // be in the user array
        current_task_stat->heap_stat = tasks_stat->heap_stat_start + heap_index;
        heap_index += task_info->task_stat.heap_count;


        // copy the stats of the different heaps the task has used and the different allocs
        // allocated in those heaps. If the number of entries remaining for alloc stats is
        // inferior to the number of allocs allocated on the current heap no alloc stat will
        // be copied at all.
        size_t h_index = 0;
        heap_stats_t *heap_info = STAILQ_FIRST(&task_info->heaps_stats);
        while(h_index < task_info->task_stat.heap_count || heap_info != NULL) {
            // increase alloc_index before filling the alloc info of the given heap
            // to avoid running out of alloc stat entry while doing it.
            if (alloc_index + heap_info->heap_stat.alloc_count > tasks_stat->alloc_count) {
                heap_info->heap_stat.alloc_stat = NULL;
            } else {
                // set the pointer where the alloc info for the given heap will
                // be in the user array
                heap_info->heap_stat.alloc_stat = tasks_stat->alloc_stat_start + alloc_index;
                // fill the alloc array in heap_info by running through all blocks of a given heap
                // and storing info about the blocks allocated by the given task
                alloc_stats_t *alloc_stats = NULL;
                size_t a_index = 0;
                STAILQ_FOREACH(alloc_stats, &heap_info->allocs_stats, next_alloc_stat) {
                    heap_info->heap_stat.alloc_stat[a_index] = alloc_stats->alloc_stat;
                    a_index++;
                }

                alloc_index += heap_info->heap_stat.alloc_count;
            }

            memcpy(current_task_stat->heap_stat + h_index, &heap_info->heap_stat, sizeof(heap_stat_t));
            h_index++;
            heap_info = STAILQ_NEXT(heap_info, next_heap_stat);
        }
    }

    xSemaphoreGive(s_task_tracking_mutex);

    tasks_stat->task_count = task_index;
    tasks_stat->heap_count = heap_index;
    tasks_stat->alloc_count = alloc_index;

    return ESP_OK;
}

esp_err_t heap_caps_get_single_task_stat(heap_single_task_stat_t *task_stat, TaskHandle_t task_handle)
{
    if (task_stat == NULL ||
        (task_stat->heap_stat_start == NULL && task_stat->heap_count != 0) ||
        (task_stat->alloc_stat_start == NULL && task_stat->alloc_count != 0)) {
        return ESP_ERR_INVALID_ARG;
    }

    if (task_handle == NULL) {
        task_handle = xTaskGetCurrentTaskHandle();
    }

    task_info_t *task_info = NULL;

    xSemaphoreTake(s_task_tracking_mutex, portMAX_DELAY);
    SLIST_FOREACH(task_info, &task_stats, next_task_info) {
        if(task_info->task_stat.handle == task_handle) {
            // copy the task_stat of the task itself
            memcpy(&task_stat->stat, &task_info->task_stat, sizeof(task_stat_t));
            break;
        }
    }
    xSemaphoreGive(s_task_tracking_mutex);

    if (task_info == NULL) {
        return ESP_FAIL;
    }

    task_stat->stat.heap_stat = task_stat->heap_stat_start;

    // copy the stats of the different heaps the task has used and the different blocks
    // allocated in those heaps. If the number of entries remaining for block stats is
    // inferior to the number of blocks allocated on the current heap no block stat will
    // be copied at all.
    size_t heap_index = 0;
    size_t alloc_index = 0;

    xSemaphoreTake(s_task_tracking_mutex, portMAX_DELAY);
    heap_stats_t *heap_info = STAILQ_FIRST(&task_info->heaps_stats);
    while(heap_index < task_info->task_stat.heap_count || heap_info != NULL) {
        // check that there is enough heap_stat entry left to add another one to the user defined
        // array of heap_stat
        if (heap_index >= task_stat->heap_count) {
            break;
        }

        // increase alloc_index before filling the block info of the given heap
        // to avoid running out of block stat entry while doing it.
        if (alloc_index + heap_info->heap_stat.alloc_count > task_stat->alloc_count) {
            heap_info->heap_stat.alloc_stat = NULL;
        } else {
            // set the pointer where the block info for the given heap will
            // be in the user array
            heap_info->heap_stat.alloc_stat = task_stat->alloc_stat_start + alloc_index;

            // fill the alloc array in heap_info by running through all blocks of a given heap
            // and storing info about the blocks allocated by the given task
            alloc_stats_t *alloc_stats = NULL;
            size_t a_index = 0;
            STAILQ_FOREACH(alloc_stats, &heap_info->allocs_stats, next_alloc_stat) {
                heap_info->heap_stat.alloc_stat[a_index] = alloc_stats->alloc_stat;
                a_index++;
            }

            alloc_index += heap_info->heap_stat.alloc_count;
        }

        memcpy(task_stat->stat.heap_stat + heap_index, &heap_info->heap_stat, sizeof(heap_stat_t));
        heap_index++;
        heap_info = STAILQ_NEXT(heap_info, next_heap_stat);
    }
    xSemaphoreGive(s_task_tracking_mutex);

    task_stat->heap_count = heap_index;
    task_stat->alloc_count = alloc_index;

    return ESP_OK;
}

static void heap_caps_print_task_info(FILE *stream, task_info_t *task_info, bool is_last_task_info)
{
    if (stream == NULL) {
        stream = stdout;
    }

    const char *task_info_visual = is_last_task_info ? " " : "│";
    const char *task_info_visual_start = is_last_task_info ? "└" : "├";
    fprintf(stream, "%s %s: %s, CURRENT MEMORY USAGE %d, PEAK MEMORY USAGE %d, TOTAL HEAP USED %d:\n", task_info_visual_start,
                                                                                                      task_info->task_stat.is_alive ? "ALIVE" : "DELETED",
                                                                                                      task_info->task_stat.name,
                                                                                                      task_info->task_stat.overall_current_usage,
                                                                                                      task_info->task_stat.overall_peak_usage,
                                                                                                      task_info->task_stat.heap_count);

    heap_stats_t *heap_info = NULL;
    STAILQ_FOREACH(heap_info, &task_info->heaps_stats, next_heap_stat) {
        const char *next_heap_visual = !STAILQ_NEXT(heap_info, next_heap_stat) ? " " : "│";
        const char *next_heap_visual_start = !STAILQ_NEXT(heap_info, next_heap_stat) ? "└" : "├";
        fprintf(stream, "%s    %s HEAP: %s, CAPS: 0x%08lx, SIZE: %d, USAGE: CURRENT %d (%d%%), PEAK %d (%d%%), ALLOC COUNT: %d\n",
                task_info_visual,
                next_heap_visual_start,
                heap_info->heap_stat.name,
                heap_info->heap_stat.caps,
                heap_info->heap_stat.size,
                heap_info->heap_stat.current_usage,
                (heap_info->heap_stat.current_usage * 100) / heap_info->heap_stat.size,
                heap_info->heap_stat.peak_usage,
                (heap_info->heap_stat.peak_usage * 100) / heap_info->heap_stat.size,
                heap_info->heap_stat.alloc_count);

        alloc_stats_t *alloc_stats = NULL;
        STAILQ_FOREACH(alloc_stats, &heap_info->allocs_stats, next_alloc_stat) {
            fprintf(stream, "%s    %s    ├ ALLOC %p, SIZE %" PRIu32 "\n", task_info_visual,
                                                                next_heap_visual,
                                                                alloc_stats->alloc_stat.address,
                                                                alloc_stats->alloc_stat.size);
        }
    }
}

static void heap_caps_print_task_overview(FILE *stream, task_info_t *task_info, bool is_first_task_info, bool is_last_task_info)
{
    if (stream == NULL) {
        stream = stdout;
    }

    if (is_first_task_info) {
        fprintf(stream, "┌────────────────────┬─────────┬──────────────────────┬───────────────────┬─────────────────┐\n");
        fprintf(stream, "│ TASK               │ STATUS  │ CURRENT MEMORY USAGE │ PEAK MEMORY USAGE │ TOTAL HEAP USED │\n");
        fprintf(stream, "├────────────────────┼─────────┼──────────────────────┼───────────────────┼─────────────────┤\n");
    }

    task_stat_t task_stat = task_info->task_stat;
    fprintf(stream, "│ %18s │ %7s │ %20d │ %17d │ %15d │\n",
                    task_stat.name,
                    task_stat.is_alive ? "ALIVE  " : "DELETED",
                    task_stat.overall_current_usage,
                    task_stat.overall_peak_usage,
                    task_stat.heap_count);

    if (is_last_task_info) {
        fprintf(stream, "└────────────────────┴─────────┴──────────────────────┴───────────────────┴─────────────────┘\n");
    }
}

void heap_caps_print_single_task_stat(FILE *stream, TaskHandle_t task_handle)
{
    if (task_handle == NULL) {
        task_handle = xTaskGetCurrentTaskHandle();
    }

    task_info_t *task_info = NULL;

    xSemaphoreTake(s_task_tracking_mutex, portMAX_DELAY);
    SLIST_FOREACH(task_info, &task_stats, next_task_info) {
        if (task_info->task_stat.handle == task_handle) {
            heap_caps_print_task_info(stream, task_info, true);

            xSemaphoreGive(s_task_tracking_mutex);
            return;
        }
    }
    xSemaphoreGive(s_task_tracking_mutex);
}

void heap_caps_print_all_task_stat(FILE *stream)
{
    task_info_t *task_info = NULL;

    xSemaphoreTake(s_task_tracking_mutex, portMAX_DELAY);
    SLIST_FOREACH(task_info, &task_stats, next_task_info) {
        const bool last_task_info = (SLIST_NEXT(task_info, next_task_info) == NULL);
        heap_caps_print_task_info(stream, task_info, last_task_info);
    }
    xSemaphoreGive(s_task_tracking_mutex);
}

void heap_caps_print_single_task_stat_overview(FILE *stream, TaskHandle_t task_handle)
{
    if (task_handle == NULL) {
        task_handle = xTaskGetCurrentTaskHandle();
    }

    task_info_t *task_info = NULL;

    xSemaphoreTake(s_task_tracking_mutex, portMAX_DELAY);
    SLIST_FOREACH(task_info, &task_stats, next_task_info) {
        if (task_info->task_stat.handle == task_handle) {
            heap_caps_print_task_overview(stream, task_info, true, true);

            xSemaphoreGive(s_task_tracking_mutex);
            return;
        }
    }
    xSemaphoreGive(s_task_tracking_mutex);
}

void heap_caps_print_all_task_stat_overview(FILE *stream)
{
    task_info_t *task_info = NULL;
    bool is_first_task_info = true;

    xSemaphoreTake(s_task_tracking_mutex, portMAX_DELAY);
    SLIST_FOREACH(task_info, &task_stats, next_task_info) {
        const bool last_task_info = (SLIST_NEXT(task_info, next_task_info) == NULL);
        heap_caps_print_task_overview(stream, task_info, is_first_task_info, last_task_info);
        is_first_task_info = false;
    }
    xSemaphoreGive(s_task_tracking_mutex);
}

esp_err_t heap_caps_alloc_single_task_stat_arrays(heap_single_task_stat_t *task_stat, TaskHandle_t task_handle)
{
    if (task_handle == NULL) {
        task_handle = xTaskGetCurrentTaskHandle();
    }

    task_stat->heap_stat_start = NULL;
    task_stat->alloc_stat_start = NULL;
    task_stat->heap_count = 0;
    task_stat->alloc_count = 0;

    task_info_t *task_info = NULL;

    xSemaphoreTake(s_task_tracking_mutex, portMAX_DELAY);
    SLIST_FOREACH(task_info, &task_stats, next_task_info) {
        if(task_info->task_stat.handle == task_handle && task_info->task_stat.is_alive) {
            task_stat->heap_count = task_info->task_stat.heap_count;
            heap_stats_t *heap_info = NULL;
            STAILQ_FOREACH(heap_info, &task_info->heaps_stats, next_heap_stat) {
                task_stat->alloc_count += heap_info->heap_stat.alloc_count;
            }
            break;
        }
    }
    xSemaphoreGive(s_task_tracking_mutex);

    // allocate the memory used to store the statistics of allocs, heaps
    if (task_stat->heap_count != 0) {
        heap_t *heap_used_for_alloc = find_biggest_heap();
        task_stat->heap_stat_start = multi_heap_malloc(heap_used_for_alloc->heap, task_stat->heap_count * sizeof(heap_stat_t));
        if (task_stat->heap_stat_start == NULL) {
            return ESP_FAIL;
        }
    }
    if (task_stat->alloc_count != 0) {
        heap_t *heap_used_for_alloc = find_biggest_heap();
        task_stat->alloc_stat_start = multi_heap_malloc(heap_used_for_alloc->heap, task_stat->alloc_count * sizeof(heap_task_block_t));
        if (task_stat->alloc_stat_start == NULL) {
            return ESP_FAIL;
        }
    }

    return ESP_OK;
}

void heap_caps_free_single_task_stat_arrays(heap_single_task_stat_t *task_stat)
{
    if (task_stat->heap_stat_start != NULL) {
        heap_t *heap_used_for_alloc = find_containing_heap(task_stat->heap_stat_start);
        assert(heap_used_for_alloc != NULL);
        multi_heap_free(heap_used_for_alloc->heap, task_stat->heap_stat_start);
        task_stat->heap_stat_start = NULL;
        task_stat->heap_count = 0;
    }
    if (task_stat->alloc_stat_start != NULL) {
        heap_t *heap_used_for_alloc = find_containing_heap(task_stat->alloc_stat_start);
        assert(heap_used_for_alloc != NULL);
        multi_heap_free(heap_used_for_alloc->heap, task_stat->alloc_stat_start);
        task_stat->alloc_stat_start = NULL;
        task_stat->alloc_count = 0;
    }
}

esp_err_t heap_caps_alloc_all_task_stat_arrays(heap_all_tasks_stat_t *tasks_stat)
{
    tasks_stat->stat_arr = NULL;
    tasks_stat->heap_stat_start = NULL;
    tasks_stat->alloc_stat_start = NULL;
    tasks_stat->task_count = 0;
    tasks_stat->heap_count = 0;
    tasks_stat->alloc_count = 0;

    task_info_t *task_info = NULL;

    xSemaphoreTake(s_task_tracking_mutex, portMAX_DELAY);
    SLIST_FOREACH(task_info, &task_stats, next_task_info) {
        tasks_stat->task_count += 1;

        tasks_stat->heap_count += task_info->task_stat.heap_count;
        heap_stats_t *heap_info = NULL;
        STAILQ_FOREACH(heap_info, &task_info->heaps_stats, next_heap_stat) {
            tasks_stat->alloc_count += heap_info->heap_stat.alloc_count;
        }
    }
    xSemaphoreGive(s_task_tracking_mutex);

    // allocate the memory used to store the statistics of allocs, heaps and tasks
    if (tasks_stat->task_count != 0) {
        heap_t *heap_used_for_alloc = find_biggest_heap();
        tasks_stat->stat_arr = multi_heap_malloc(heap_used_for_alloc->heap, tasks_stat->task_count * sizeof(task_stat_t));
        if (tasks_stat->stat_arr == NULL) {
            return ESP_FAIL;
        }
    }
    if (tasks_stat->heap_count != 0) {
        heap_t *heap_used_for_alloc = find_biggest_heap();
        tasks_stat->heap_stat_start = multi_heap_malloc(heap_used_for_alloc->heap, tasks_stat->heap_count * sizeof(heap_stat_t));
        if (tasks_stat->heap_stat_start == NULL) {
            return ESP_FAIL;
        }
    }
    if (tasks_stat->alloc_count != 0) {
        heap_t *heap_used_for_alloc = find_biggest_heap();
        tasks_stat->alloc_stat_start = multi_heap_malloc(heap_used_for_alloc->heap, tasks_stat->alloc_count * sizeof(heap_task_block_t));
        if (tasks_stat->alloc_stat_start == NULL) {
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

void heap_caps_free_all_task_stat_arrays(heap_all_tasks_stat_t *tasks_stat)
{
    if (tasks_stat->stat_arr != NULL) {
        heap_t *heap_used_for_alloc = find_containing_heap(tasks_stat->stat_arr);
        assert(heap_used_for_alloc != NULL);
        multi_heap_free(heap_used_for_alloc->heap, tasks_stat->stat_arr);
        tasks_stat->stat_arr = NULL;
        tasks_stat->task_count = 0;
    }
    if (tasks_stat->heap_stat_start != NULL) {
        heap_t *heap_used_for_alloc = find_containing_heap(tasks_stat->heap_stat_start);
        assert(heap_used_for_alloc != NULL);
        multi_heap_free(heap_used_for_alloc->heap, tasks_stat->heap_stat_start);
        tasks_stat->heap_stat_start = NULL;
        tasks_stat->heap_count = 0;
    }
    if (tasks_stat->alloc_stat_start != NULL) {
        heap_t *heap_used_for_alloc = find_containing_heap(tasks_stat->alloc_stat_start);
        assert(heap_used_for_alloc != NULL);
        multi_heap_free(heap_used_for_alloc->heap, tasks_stat->alloc_stat_start);
        tasks_stat->alloc_stat_start = NULL;
        tasks_stat->alloc_count = 0;
    }
}

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
            TaskHandle_t btask = MULTI_HEAP_GET_BLOCK_OWNER(p);
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
                } else {
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
