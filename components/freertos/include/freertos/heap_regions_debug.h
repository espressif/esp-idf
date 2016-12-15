#ifndef _HEAP_REGION_DEBUG_H
#define _HEAP_REGION_DEBUG_H

#include "FreeRTOS.h"

#if (configENABLE_MEMORY_DEBUG == 1)

#define DEBUG_DOG_VALUE 0x1a2b3c4d
#define DEBUG_MAX_INFO_NUM 20
#define DEBUG_TYPE_MALLOC 1
#define DEBUG_TYPE_FREE   2

typedef struct {
    unsigned int dog;
    char task[4];
    unsigned int pc;
}block_head_t;

typedef struct {
    unsigned int dog;
}block_tail_t;

/* Please keep this definition same as BlockLink_t */
typedef struct _os_block_t {
    struct _os_block_t *next;
    size_t   size;
    unsigned int xtag;
}os_block_t;

typedef struct {
    block_head_t head;
    os_block_t os_block;
}debug_block_t;

typedef struct _mem_dbg_info{
    void     *addr;
    char     *task;
    uint32_t pc;
    uint32_t time;
    uint8_t  type;
}mem_dbg_info_t;

typedef struct _mem_dbg_ctl{
    mem_dbg_info_t info[DEBUG_MAX_INFO_NUM];
    uint32_t cnt;
}mem_dbg_ctl_t;

#define BLOCK_HEAD_LEN sizeof(block_head_t)
#define BLOCK_TAIL_LEN sizeof(block_tail_t)
#define OS_BLOCK(_b)   ((os_block_t*)((debug_block_t*)((char*)(_b) + BLOCK_HEAD_LEN)))
#define DEBUG_BLOCK(_b)  ((debug_block_t*)((char*)(_b) - BLOCK_HEAD_LEN))
#define HEAD_DOG(_b) ((_b)->head.dog)
#define TAIL_DOG(_b) (*(unsigned int*)((char*)(_b) + (((_b)->os_block.size & (~g_alloc_bit) ) - BLOCK_TAIL_LEN)))

#define DOG_ASSERT()\
{\
    mem_debug_show();\
    abort();\
}

extern void mem_check_block(void * data);
extern void mem_init_dog(void *data);
extern void mem_debug_init(size_t size, void *start, void *end, portMUX_TYPE *mutex);
extern void mem_malloc_block(void *data);
extern void mem_free_block(void *data);
extern void mem_check_all(void* pv);

#else

#define mem_check_block(...)
#define mem_init_dog(...)

#define BLOCK_HEAD_LEN 0
#define BLOCK_TAIL_LEN 0

#endif

#endif
