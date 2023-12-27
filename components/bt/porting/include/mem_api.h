/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_ESP_NIMBLE_CONTROLLER && CONFIG_BT_CONTROLLER_ENABLED

struct os_mempool;
struct os_mbuf_pool;


int r_mem_malloc_mempool(struct os_mempool *mempool, uint16_t num_blocks,
                       uint32_t block_size, char *name, void **out_buf);
#define mem_malloc_mempool r_mem_malloc_mempool

int r_mem_malloc_mempool_ext(struct os_mempool_ext *mempool, uint16_t num_blocks,
                           uint32_t block_size, char *name, void **out_buf);
#define mem_malloc_mempool_ext r_mem_malloc_mempool_ext


int r_mem_malloc_mbuf_pool(struct os_mempool *mempool,
                         struct os_mbuf_pool *mbuf_pool, uint16_t num_blocks,
                         uint32_t block_size, char *name,
                         void **out_buf);
#define mem_malloc_mbuf_pool r_mem_malloc_mbuf_pool

int r_mem_malloc_mbufpkt_pool(struct os_mempool *mempool,
                            struct os_mbuf_pool *mbuf_pool, int num_blocks,
                            int block_size, char *name,
                            void **out_buf);
#define mem_malloc_mbufpkt_pool r_mem_malloc_mbufpkt_pool

int r_mem_init_mbuf_pool(void *mem, struct os_mempool *mempool,
                       struct os_mbuf_pool *mbuf_pool, int num_blocks,
                       int block_size, const char *name);
#define mem_init_mbuf_pool r_mem_init_mbuf_pool



/**
 * Specifies a function used as a callback.  Functions of this type allocate an
 * mbuf chain meant to hold a packet fragment.  The resulting mbuf must contain
 * a pkthdr.
 *
 * @param frag_size             The number of data bytes that the mbuf will
 *                                  eventually contain.
 * @param arg                   A generic parameter.
 *
 * @return                      An allocated mbuf chain on success;
 *                              NULL on failure.
 */
typedef struct os_mbuf *mem_frag_alloc_fn(uint16_t frag_size, void *arg);



struct os_mbuf *r_mem_split_frag(struct os_mbuf **om, uint16_t max_frag_sz,
                               mem_frag_alloc_fn *alloc_cb, void *cb_arg);
#define mem_split_frag r_mem_split_frag

void *r_mem_pullup_obj(struct os_mbuf **om, uint16_t len);
#define mem_pullup_obj r_mem_pullup_obj

#endif

#ifdef __cplusplus
}
#endif
