#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <multi_heap.h>
#include "multi_heap_internal.h"
#include "heap_private.h"
#include "heap_task_info.h"

#ifdef CONFIG_HEAP_TASK_TRACKING

/*
 * For each task that has allocated memory from the heap, return totals for
 * allocations of each type of heap region (8-bit DRAM, 8-bit D/IRAM, 32-bit
 * IRAM, SPIRAM) into provided array of heap_dump_totals_t structs 'totals'.
 *
 * If one or more task handles is provided in array 'tasks', for each block
 * allocated by one of the tasks a heap_dump_block_t struct is written into
 * provided array 'buffer' to give the task, address, size and region type for
 * the block.
 */
size_t heap_caps_get_per_task_info(heap_dump_totals_t* totals, size_t* ntotal, size_t max,
                                   TaskHandle_t* tasks, size_t ntasks,
                                   heap_dump_block_t* buffer, size_t size)
{
    heap_t *reg;
    size_t count = *ntotal;
    size_t remaining = size;
    size_t i;

    SLIST_FOREACH(reg, &registered_heaps, next) {
        multi_heap_handle_t heap = reg->heap;
        if (heap == NULL) {
            continue;
        }
        uint32_t caps = get_all_caps(reg);
        enum region_types type = DRAM;
        if (caps & MALLOC_CAP_EXEC) {
            type = D_IRAM;
        }
        if (!(caps & MALLOC_CAP_8BIT)) {
            type = IRAM;
        }
        if (caps & MALLOC_CAP_SPIRAM) {
            type = SPIRAM;
        }
        multi_heap_block_handle_t b = multi_heap_get_first_block(heap);
        multi_heap_internal_lock(heap);
        while (b) {
            if (multi_heap_is_free(b)) {
                b = multi_heap_get_next_block(heap, b);
                continue;
            }
            void* p = multi_heap_get_block_address(b);  // Safe, only arithmetic
            size_t bsize = multi_heap_get_allocated_size(heap, p); // Validates
            TaskHandle_t btask = (TaskHandle_t)multi_heap_get_block_owner(b);
            size_t index;
            for (index = 0; index < count; ++index) {
                if (totals[index].task == btask) {
                    break;
                }
            }
            if (index < count) {
                totals[index].size[type] += bsize;
            }
            else {
                if (count < max) {
                    totals[count].task = btask;
                    totals[count].size[type] = bsize;
                    ++count;
                }
            }
            if (tasks) {
                for (i = 0; i < ntasks; ++i) {
                    if (btask == tasks[i]) {
                        break;
                    }
                }
                if (i == ntasks) {
                    b = multi_heap_get_next_block(heap, b);
                    continue;
                }
            }
            if (remaining > 0) {
                buffer->task = btask;
                buffer->address = p;
                buffer->size = bsize;
                buffer->type = type;
                ++buffer;
                --remaining;
            }
            b = multi_heap_get_next_block(heap, b);
        }
        multi_heap_internal_unlock(heap);
    }
    *ntotal = count;
    return size - remaining;
}
#endif
