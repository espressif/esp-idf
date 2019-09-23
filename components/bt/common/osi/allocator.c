/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "bt_common.h"
#include "osi/allocator.h"

extern void *pvPortZalloc(size_t size);
extern void vPortFree(void *pv);


#if HEAP_MEMORY_DEBUG

#define OSI_MEM_DBG_INFO_MAX    1024*3
typedef struct {
    void *p;
    int size;
    const char *func;
    int line;
} osi_mem_dbg_info_t;

static uint32_t mem_dbg_count = 0;
static osi_mem_dbg_info_t mem_dbg_info[OSI_MEM_DBG_INFO_MAX];
static uint32_t mem_dbg_current_size = 0;
static uint32_t mem_dbg_max_size = 0;

#define OSI_MEM_DBG_MAX_SECTION_NUM 5
typedef struct {
    bool used;
    uint32_t max_size;
} osi_mem_dbg_max_size_section_t;
static osi_mem_dbg_max_size_section_t mem_dbg_max_size_section[OSI_MEM_DBG_MAX_SECTION_NUM];

void osi_mem_dbg_init(void)
{
    int i;

    for (i = 0; i < OSI_MEM_DBG_INFO_MAX; i++) {
        mem_dbg_info[i].p = NULL;
        mem_dbg_info[i].size = 0;
        mem_dbg_info[i].func = NULL;
        mem_dbg_info[i].line = 0;
    }
    mem_dbg_count = 0;
    mem_dbg_current_size = 0;
    mem_dbg_max_size = 0;

    for (i = 0; i < OSI_MEM_DBG_MAX_SECTION_NUM; i++){
        mem_dbg_max_size_section[i].used = false;
        mem_dbg_max_size_section[i].max_size = 0;
    }
}

void osi_mem_dbg_record(void *p, int size, const char *func, int line)
{
    int i;

    if (!p || size == 0) {
        OSI_TRACE_ERROR("%s invalid !!\n", __func__);
        return;
    }

    for (i = 0; i < OSI_MEM_DBG_INFO_MAX; i++) {
        if (mem_dbg_info[i].p == NULL) {
            mem_dbg_info[i].p = p;
            mem_dbg_info[i].size = size;
            mem_dbg_info[i].func = func;
            mem_dbg_info[i].line = line;
            mem_dbg_count++;
            break;
        }
    }

    if (i >= OSI_MEM_DBG_INFO_MAX) {
        OSI_TRACE_ERROR("%s full %s %d !!\n", __func__, func, line);
    }

    mem_dbg_current_size += size;
    if(mem_dbg_max_size < mem_dbg_current_size) {
        mem_dbg_max_size = mem_dbg_current_size;
    }

    for (i = 0; i < OSI_MEM_DBG_MAX_SECTION_NUM; i++){
        if (mem_dbg_max_size_section[i].used) {
            if(mem_dbg_max_size_section[i].max_size < mem_dbg_current_size) {
                mem_dbg_max_size_section[i].max_size = mem_dbg_current_size;
            }
        }
    }
}

void osi_mem_dbg_clean(void *p, const char *func, int line)
{
    int i;

    if (!p) {
        OSI_TRACE_ERROR("%s invalid\n", __func__);
        return;
    }

    for (i = 0; i < OSI_MEM_DBG_INFO_MAX; i++) {
        if (mem_dbg_info[i].p == p) {
            mem_dbg_current_size -= mem_dbg_info[i].size;
            mem_dbg_info[i].p = NULL;
            mem_dbg_info[i].size = 0;
            mem_dbg_info[i].func = NULL;
            mem_dbg_info[i].line = 0;
            mem_dbg_count--;
            break;
        }
    }

    if (i >= OSI_MEM_DBG_INFO_MAX) {
        OSI_TRACE_ERROR("%s full %s %d !!\n", __func__, func, line);
    }
}

void osi_mem_dbg_show(void)
{
    int i;

    for (i = 0; i < OSI_MEM_DBG_INFO_MAX; i++) {
        if (mem_dbg_info[i].p || mem_dbg_info[i].size != 0 ) {
            OSI_TRACE_ERROR("--> p %p, s %d, f %s, l %d\n", mem_dbg_info[i].p, mem_dbg_info[i].size, mem_dbg_info[i].func, mem_dbg_info[i].line);
        }
    }
    OSI_TRACE_ERROR("--> count %d\n", mem_dbg_count);
    OSI_TRACE_ERROR("--> size %dB\n--> max size %dB\n", mem_dbg_current_size, mem_dbg_max_size);
}

uint32_t osi_mem_dbg_get_max_size(void)
{
    return mem_dbg_max_size;
}

uint32_t osi_mem_dbg_get_current_size(void)
{
    return mem_dbg_current_size;
}

void osi_men_dbg_set_section_start(uint8_t index)
{
    if (index >= OSI_MEM_DBG_MAX_SECTION_NUM) {
        OSI_TRACE_ERROR("Then range of index should be between 0 and %d, current index is %d.\n",
                            OSI_MEM_DBG_MAX_SECTION_NUM - 1, index);
        return;
    }

    if (mem_dbg_max_size_section[index].used) {
        OSI_TRACE_WARNING("This index(%d) has been started, restart it.\n", index);
    }

    mem_dbg_max_size_section[index].used = true;
    mem_dbg_max_size_section[index].max_size = mem_dbg_current_size;
}

void osi_men_dbg_set_section_end(uint8_t index)
{
    if (index >= OSI_MEM_DBG_MAX_SECTION_NUM) {
        OSI_TRACE_ERROR("Then range of index should be between 0 and %d, current index is %d.\n",
                            OSI_MEM_DBG_MAX_SECTION_NUM - 1, index);
        return;
    }

    if (!mem_dbg_max_size_section[index].used) {
        OSI_TRACE_ERROR("This index(%d) has not been started.\n", index);
        return;
    }

    mem_dbg_max_size_section[index].used = false;
}

uint32_t osi_mem_dbg_get_max_size_section(uint8_t index)
{
    if (index >= OSI_MEM_DBG_MAX_SECTION_NUM){
        OSI_TRACE_ERROR("Then range of index should be between 0 and %d, current index is %d.\n",
                            OSI_MEM_DBG_MAX_SECTION_NUM - 1, index);
        return 0;
    }

    return mem_dbg_max_size_section[index].max_size;
}
#endif

char *osi_strdup(const char *str)
{
    size_t size = strlen(str) + 1;  // + 1 for the null terminator
    char *new_string = (char *)osi_calloc(size);

    if (!new_string) {
        return NULL;
    }

    memcpy(new_string, str, size);
    return new_string;
}

void *osi_malloc_func(size_t size)
{
#if HEAP_MEMORY_DEBUG
    void *p;
#if HEAP_ALLOCATION_FROM_SPIRAM_FIRST
    p = heap_caps_malloc_prefer(size, 2, MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL);
#else
    p = malloc(size);
#endif /* #if HEAP_ALLOCATION_FROM_SPIRAM_FIRST */
    osi_mem_dbg_record(p, size, __func__, __LINE__);
    return p;
#else
#if HEAP_ALLOCATION_FROM_SPIRAM_FIRST
    return heap_caps_malloc_prefer(size, 2, MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL);
#else
    return malloc(size);
#endif /* #if HEAP_ALLOCATION_FROM_SPIRAM_FIRST */
#endif /* #if HEAP_MEMORY_DEBUG */
}

void *osi_calloc_func(size_t size)
{
#if HEAP_MEMORY_DEBUG
    void *p;
#if HEAP_ALLOCATION_FROM_SPIRAM_FIRST
    p = heap_caps_calloc_prefer(1, size, 2, MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL);
#else
    p = calloc(1, size);
#endif /* #if HEAP_ALLOCATION_FROM_SPIRAM_FIRST */
    osi_mem_dbg_record(p, size, __func__, __LINE__);
    return p;
#else
#if HEAP_ALLOCATION_FROM_SPIRAM_FIRST
    return heap_caps_calloc_prefer(1, size, 2, MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL);
#else
    return calloc(1, size);
#endif /* #if HEAP_ALLOCATION_FROM_SPIRAM_FIRST */
#endif /* #if HEAP_MEMORY_DEBUG */
}

void osi_free_func(void *ptr)
{
#if HEAP_MEMORY_DEBUG
    osi_mem_dbg_clean(ptr, __func__, __LINE__);
#endif
    free(ptr);
}
