#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <multi_heap.h>
#include "multi_heap_internal.h"
#include "multi_heap_poisoning.h"
#include "heap_private.h"
#include "esp_heap_debug.h"

#ifdef CONFIG_HEAP_TASK_TRACKING

// return all possible capabilities (across all priorities) for a given heap

inline static uint32_t get_all_caps(const heap_t *heap)
{
    uint32_t all_caps = 0;
    for (int prio = 0; prio < SOC_MEMORY_TYPE_NO_PRIOS; prio++) {
        all_caps |= heap->caps[prio];
    }
    return all_caps;
}

// For each task that has allocated memory from the heap, return totals for
// allocations of each type of heap region (8-bit DRAM, 8-bit D/IRAM, 32-bit
// IRAM) into provided array of heap_dump_totals_t structs 'totals'.

// If one or more task handles is provided in array 'tasks', for each block
// allocated by one of the tasks a heap_dump_block_t struct is written into
// provided array 'buffer' to give the task, address, size and region type for
// the block.

size_t esp_heap_debug_dump_totals(heap_dump_totals_t* totals, size_t* ntotal, size_t max,
				  TaskHandle_t* tasks, size_t ntasks,
				  heap_dump_block_t* buffer, size_t size)
{
    heap_t *heap;
    heap_block_t *b;
    poison_head_t *d;
    TaskHandle_t btask;
    size_t count = *ntotal;
    size_t remaining = size;
    size_t i;

    SLIST_FOREACH(heap, &registered_heaps, next) {
	heap_metadata_t *meta = heap->heap;
	if (meta == NULL) {
	    continue;
	}
	uint32_t caps = get_all_caps(heap);
	int type = 0;
	if (caps & MALLOC_CAP_EXEC)
	    ++type;
	if (!(caps & MALLOC_CAP_8BIT))
	    ++type;
	if (caps & MALLOC_CAP_SPIRAM)
	    type = 3;
	b = &meta->first_block;
	multi_heap_internal_lock(meta);
	while (b && remaining > 0) {
	    if (b->header & BLOCK_FREE_FLAG) {
		b = (heap_block_t*)(b->header & NEXT_BLOCK_MASK);
		continue;
	    }
	    d = (poison_head_t*)b->data;
	    btask = d->task;
	    size_t index;
	    for (index = 0; index < count; ++index) {
		if (totals[index].task == btask)
		    break;
	    }
	    if (index < count)
		totals[index].after[type] += d->alloc_size;
	    else {
		if (count < max) {
		    totals[count].task = btask;
		    for (i = 0; i < NUM_USED_TYPES; ++i) {
			totals[count].before[i] = 0;
			totals[count].after[i] = 0;
		    }
		    totals[count].after[type] = d->alloc_size;
		    ++count;
		}
	    }
	    if (tasks) {
		for (i = 0; i < ntasks; ++i) {
		    if (btask == tasks[i])
			break;
		}
		if (i == ntasks) {
		    b = (heap_block_t*)(b->header & NEXT_BLOCK_MASK);
		    continue;
		}
	    }
	    buffer->task = btask;
	    buffer->address = (void*)d + sizeof(poison_head_t);
	    buffer->size = d->alloc_size;
	    buffer->type = type;
	    ++buffer;
	    --remaining;
	    b = (heap_block_t*)(b->header & NEXT_BLOCK_MASK);
	}
	multi_heap_internal_unlock(meta);
    }
    *ntotal = count;
    return size - remaining;
}
#endif
