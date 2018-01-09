#pragma once

typedef struct {
    uint32_t head_canary;
#ifdef CONFIG_HEAP_TASK_TRACKING
    TaskHandle_t task;
#endif
    size_t alloc_size;
} poison_head_t;

typedef struct {
    uint32_t tail_canary;
} poison_tail_t;
