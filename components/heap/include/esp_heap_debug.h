#ifndef _ESP_HEAP_DEBUG_H
#define _ESP_HEAP_DEBUG_H

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

extern size_t esp_heap_debug_dump_totals(heap_dump_totals_t* totals, size_t* ntotal, size_t max,
					 TaskHandle_t* tasks, size_t ntasks,
					 heap_dump_block_t* buffer, size_t size);
#ifdef __cplusplus
}
#endif

#endif
#endif // _ESP_HEAP_DEBUG_H
