/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#ifndef LWIP_HDR_MEMP_H
#define LWIP_HDR_MEMP_H

#ifdef __cplusplus
extern "C" {
#endif

/* run once with empty definition to handle all custom includes in lwippools.h */
#define LWIP_MEMPOOL(name,num,size,desc)
#include "lwip/priv/memp_std.h"

/* Create the list of all memory pools managed by memp. MEMP_MAX represents a NULL pool at the end */
typedef enum {
#define LWIP_MEMPOOL(name,num,size,desc)  MEMP_##name,
#include "lwip/priv/memp_std.h"
  MEMP_MAX
} memp_t;

#include "lwip/priv/memp_priv.h"

/* Private mempools example:
 * .h: only when pool is used in multiple .c files: LWIP_MEMPOOL_PROTOTYPE(my_private_pool);
 * .c:
 *   - in global variables section: LWIP_MEMPOOL_DECLARE(my_private_pool, 10, sizeof(foo), "Some description")
 *   - call ONCE before using pool (e.g. in some init() function): LWIP_MEMPOOL_INIT(my_private_pool);
 *   - allocate: void* my_new_mem = LWIP_MEMPOOL_ALLOC(my_private_pool);
 *   - free: LWIP_MEMPOOL_FREE(my_private_pool, my_new_mem);
 *
 * To relocate a pool, declare it as extern in cc.h. Example for GCC:
 *   extern u8_t __attribute__((section(".onchip_mem"))) memp_memory_my_private_pool[];
 */

extern const struct memp_desc* const memp_pools[MEMP_MAX];

#define LWIP_MEMPOOL_PROTOTYPE(name) extern const struct memp_desc memp_ ## name

#if MEMP_MEM_MALLOC

#include "lwip/mem.h"

#define memp_init()
#if ESP_STATS_MEM
static inline void* memp_malloc(int type)
{
    ESP_CNT_MEM_MALLOC_INC(type);
    return mem_malloc(memp_pools[type]->size);
}

static inline void memp_free(int type, void *mem)
{
    ESP_CNT_MEM_FREE_INC(type);
    mem_free(mem);
}

//#define memp_malloc(type)     mem_malloc(memp_pools[type]->size); ESP_CNT_MEM_MALLOC_INC(type)
//#define memp_free(type, mem)  mem_free(mem); ESP_CNT_MEM_FREE_INC(type)
#else
#define memp_malloc(type)     mem_malloc(memp_pools[type]->size)
#define memp_free(type, mem)  mem_free(mem)
#endif

#define LWIP_MEMPOOL_DECLARE(name,num,size,desc) \
  const struct memp_desc memp_ ## name = { \
    LWIP_MEM_ALIGN_SIZE(size) \
  };

#define LWIP_MEMPOOL_INIT(name)
#define LWIP_MEMPOOL_ALLOC(name)   mem_malloc(memp_ ## name.size)
#define LWIP_MEMPOOL_FREE(name, x) mem_free(x)

#else /* MEMP_MEM_MALLOC */

#define LWIP_MEMPOOL_DECLARE(name,num,size,desc) u8_t memp_memory_ ## name ## _base \
    [((num) * (MEMP_SIZE + MEMP_ALIGN_SIZE(size)))]; \
    \
  static struct memp *memp_tab_ ## name; \
    \
  const struct memp_desc memp_ ## name = { \
    LWIP_MEM_ALIGN_SIZE(size), \
    (num), \
    DECLARE_LWIP_MEMPOOL_DESC(desc) \
    memp_memory_ ## name ## _base, \
    &memp_tab_ ## name \
  };

#define LWIP_MEMPOOL_INIT(name)    memp_init_pool(&memp_ ## name)
#define LWIP_MEMPOOL_ALLOC(name)   memp_malloc_pool(&memp_ ## name)
#define LWIP_MEMPOOL_FREE(name, x) memp_free_pool(&memp_ ## name, (x))

#if MEM_USE_POOLS
/** This structure is used to save the pool one element came from. */
struct memp_malloc_helper
{
   memp_t poolnr;
#if MEMP_OVERFLOW_CHECK
   u16_t size;
#endif /* MEMP_OVERFLOW_CHECK */
};

#endif /* MEM_USE_POOLS */

void  memp_init(void);

#if MEMP_OVERFLOW_CHECK
void *memp_malloc_fn(memp_t type, const char* file, const int line);
#define memp_malloc(t) memp_malloc_fn((t), __FILE__, __LINE__)
#else
void *memp_malloc(memp_t type);
#endif
void  memp_free(memp_t type, void *mem);

#endif /* MEMP_MEM_MALLOC */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_MEMP_H */
