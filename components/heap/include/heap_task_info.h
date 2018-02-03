#ifndef _HEAP_TASK_INFO_H
#define _HEAP_TASK_INFO_H

#ifdef CONFIG_HEAP_TASK_TRACKING

#ifdef __cplusplus
extern "C" {
#endif

enum region_types {
    DRAM = 0,
    D_IRAM = 1,
    IRAM = 2,
    SPIRAM = 3,
    NUM_USED_TYPES = 4
};      

typedef struct {
    TaskHandle_t task;
    int size[NUM_USED_TYPES];
} heap_dump_totals_t;

typedef struct {
    TaskHandle_t task;
    void* address;
    int size: 24;                   // The size of the allocated block.
    int type: 7;                    // Type of block's region
    int unused: 1;                  // (forces 32-bit access for type)
} heap_dump_block_t;

/**
 * @brief Return per-task heap allocation totals and lists of blocks.
 *
 * For each task that has allocated memory from the heap, return totals for
 * allocations of each type of heap region (8-bit DRAM, 8-bit D/IRAM, 32-bit
 * IRAM, SPIRAM) into provided array of heap_dump_totals_t structs 'totals'.
 *
 * If one or more task handles is provided in array 'tasks', for each block
 * allocated by one of the tasks a heap_dump_block_t struct is written into
 * provided array 'buffer' to give the task, address, size and region type for
 * the block.
 */
extern size_t heap_caps_get_per_task_info(heap_dump_totals_t* totals, size_t* ntotal, size_t max,
                                          TaskHandle_t* tasks, size_t ntasks,
                                          heap_dump_block_t* buffer, size_t size);
#ifdef __cplusplus
}
#endif

#endif
#endif // _HEAP_TASK_INFO_H
