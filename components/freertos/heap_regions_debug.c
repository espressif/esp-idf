#include "heap_regions_debug.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if (configENABLE_MEMORY_DEBUG == 1)

static os_block_t g_malloc_list, *g_free_list=NULL, *g_end;
static size_t g_heap_struct_size;
static mem_dbg_ctl_t g_mem_dbg;
char g_mem_print = 0;
static portMUX_TYPE *g_malloc_mutex = NULL;
#define MEM_DEBUG(...)

void mem_debug_init(size_t size, void *start, void *end, portMUX_TYPE *mutex)
{
    MEM_DEBUG("size=%d start=%p end=%p mutex=%p%x\n", size, start, end, mutex);
    memset(&g_mem_dbg, 0, sizeof(g_mem_dbg));
    memset(&g_malloc_list, 0, sizeof(g_malloc_list));
    g_malloc_mutex = mutex;
    g_heap_struct_size = size;
    g_free_list = start;
    g_end = end;
}

void mem_debug_push(char type, void *addr)
{
    os_block_t *b = (os_block_t*)addr;
    debug_block_t *debug_b = DEBUG_BLOCK(b);

    MEM_DEBUG("push type=%d addr=%p\n", type, addr);
    if (g_mem_print){
        if (type == DEBUG_TYPE_MALLOC){
            ets_printf("task=%s t=%s s=%u a=%p\n", debug_b->head.task?debug_b->head.task:"", type==DEBUG_TYPE_MALLOC?"m":"f", b->size, addr);
        } else {
            ets_printf("task=%s t=%s s=%u a=%p\n", debug_b->head.task?debug_b->head.task:"", type==DEBUG_TYPE_MALLOC?"m":"f", b->size, addr);
        }
    } else {
        mem_dbg_info_t *info = &g_mem_dbg.info[g_mem_dbg.cnt%DEBUG_MAX_INFO_NUM];

        info->addr = addr;
        info->type = type;
        info->time = g_mem_dbg.cnt;
        g_mem_dbg.cnt++;
    }
}

void mem_debug_malloc_show(void)
{
    os_block_t *b = g_malloc_list.next;
    debug_block_t *d;

    taskENTER_CRITICAL(g_malloc_mutex);
    while (b){
        d = DEBUG_BLOCK(b);
        d->head.task[3] = '\0';
        ets_printf("t=%s s=%u a=%p\n", d->head.task?d->head.task:"", b->size, b);
        b = b->next;
    }
    taskEXIT_CRITICAL(g_malloc_mutex);
}

void mem_debug_show(void)
{
    uint32_t i;

    if (!g_mem_print) return;

    for (i=0; i<DEBUG_MAX_INFO_NUM; i++){
        ets_printf("%u %s %p\n", g_mem_dbg.info[i].time, g_mem_dbg.info[i].type == DEBUG_TYPE_FREE?"f":"m", g_mem_dbg.info[i].addr);
    }
}

void mem_check_block(void* data)
{
    debug_block_t *b = DEBUG_BLOCK(data);

    MEM_DEBUG("check block data=%p\n", data);
    if (data && (HEAD_DOG(b) == DEBUG_DOG_VALUE)){
        if (TAIL_DOG(b) != DEBUG_DOG_VALUE){
            ets_printf("f task=%s a=%p h=%08x t=%08x\n", b->head.task?b->head.task:"", b, HEAD_DOG(b), TAIL_DOG(b));
            DOG_ASSERT();
        }
    } else {
        ets_printf("f task=%s a=%p h=%08x\n", b->head.task?b->head.task:"", b, HEAD_DOG(b));\
        DOG_ASSERT();
    }
}

void mem_init_dog(void *data)
{
    debug_block_t *b = DEBUG_BLOCK(data);
    xTaskHandle task;

    MEM_DEBUG("init dog, data=%p debug_block=%p block_size=%x\n", data, b, b->os_block.size);
    if (!data) return;
#if (INCLUDE_pcTaskGetTaskName == 1)
    task = xTaskGetCurrentTaskHandle();
    if (task){
        strncpy(b->head.task, pcTaskGetTaskName(task), 3);
        b->head.task[3] = '\0';
    } 
#else
    b->head.task = '\0';
#endif
    HEAD_DOG(b) = DEBUG_DOG_VALUE;
    TAIL_DOG(b) = DEBUG_DOG_VALUE;
}

void mem_check_all(void* pv)
{
    os_block_t *b;

    if (pv){
        char *puc = (char*)(pv);
        os_block_t *b;
        puc -= (g_heap_struct_size - BLOCK_TAIL_LEN - BLOCK_HEAD_LEN);
        b = (os_block_t*)puc;
        mem_check_block(b);
    }

    taskENTER_CRITICAL(g_malloc_mutex);
    b = g_free_list->next;
    while(b && b != g_end){
        mem_check_block(b);
        ets_printf("check b=%p size=%d ok\n", b, b->size);
        b = b->next;
    }
    taskEXIT_CRITICAL(g_malloc_mutex);
}

void mem_malloc_show(void)
{
    os_block_t *b = g_malloc_list.next;
    debug_block_t *debug_b;

    while (b){
        debug_b = DEBUG_BLOCK(b);
        ets_printf("%s %p %p %u\n", debug_b->head.task, debug_b, b, b->size);
        b = b->next;
    }
}

void mem_malloc_block(void *data)
{
    os_block_t *b = (os_block_t*)data;

    MEM_DEBUG("mem malloc block data=%p, size=%u\n", data, b->size);
    mem_debug_push(DEBUG_TYPE_MALLOC, data);

    if (b){
        b->next = g_malloc_list.next;
        g_malloc_list.next = b;
    }
}

void mem_free_block(void *data)
{
    os_block_t *del = (os_block_t*)data;
    os_block_t *b = g_malloc_list.next;
    os_block_t *pre = &g_malloc_list;
    debug_block_t *debug_b;

    MEM_DEBUG("mem free block data=%p, size=%d\n", data, del->size);
    mem_debug_push(DEBUG_TYPE_FREE, data);

    if (!del) {
        return;
    }

    while (b){
        if ( (del == b) ){
            pre->next = b->next;
            b->next = NULL;
            return;
        }
        pre = b;
        b = b->next;
    }

    debug_b = DEBUG_BLOCK(del);
    ets_printf("%s %p %p %u already free\n", debug_b->head.task, debug_b, del, del->size);
    mem_malloc_show();
    abort();
}

#endif


