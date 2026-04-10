/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef _BTDM_MEMPOOL_H_
#define _BTDM_MEMPOOL_H_

#include "btdm_osal.h"
#include "queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A memory block structure. This simply contains a pointer to the free list
 * chain and is only used when the block is on the free list. When the block
 * has been removed from the free list the entire memory block is usable by the
 * caller.
 */
struct btdm_memblock {
    SLIST_ENTRY(btdm_memblock) mb_next;
};

/* XXX: Change this structure so that we keep the first address in the pool? */
/* XXX: add memory debug structure and associated code */
/* XXX: Change how I coded the SLIST_HEAD here. It should be named:
   SLIST_HEAD(,btdm_memblock) mp_head; */

/**
 * Memory pool
 */
struct btdm_mempool {
    /** Size of the memory blocks, in bytes. */
    uint32_t mp_block_size;
    /** The number of memory blocks. */
    uint16_t mp_num_blocks;
    /** The number of free blocks left */
    uint16_t mp_num_free;
    /** The lowest number of free blocks seen */
    uint16_t mp_min_free;
    /** Bitmap of OS_MEMPOOL_F_[...] values. */
    uint8_t mp_flags;
    /** Address of memory buffer used by pool */
    uint32_t mp_membuf_addr;
    STAILQ_ENTRY(btdm_mempool) mp_list;
    SLIST_HEAD(, btdm_memblock);
    /** Name for memory block */
    const char *name;
};

/**
 * Indicates an extended mempool.  Address can be safely cast to
 * (struct btdm_mempool_ext *).
 */
#define BTDM_MEMPOOL_F_EXT 0x01

#define BTDM_ALIGNMENT 4
#define BTDM_MEMPOOL_SIZE(n, blksize)                                                              \
    ((((blksize) + ((BTDM_ALIGNMENT) - 1)) / (BTDM_ALIGNMENT)) * (n))

#if (BTDM_ALIGNMENT == 4)
typedef uint32_t btdm_membuf_t;
#elif (BTDM_ALIGNMENT == 8)
typedef uint64_t btdm_membuf_t;
#elif (BTDM_ALIGNMENT == 16)
typedef __uint128_t btdm_membuf_t;
#else
#error "Unhandled `BTDM_ALIGNMENT` for `btdm_membuf_t`"
#endif /* BTDM_ALIGNMENT == * */

/**
 * Initialize a memory pool.
 *
 * @param mp            Pointer to a pointer to a mempool
 * @param blocks        The number of blocks in the pool
 * @param blocks_size   The size of the block, in bytes.
 * @param membuf        Pointer to memory to contain blocks.
 * @param name          Name of the pool.
 *
 * @return btdm_osal_error_t
 */
static inline btdm_osal_error_t
btdm_mempool_init(struct btdm_mempool *mp, uint16_t blocks, uint32_t block_size, void *membuf,
                  const char *name)
{
    extern btdm_osal_error_t r_btdm_os_mempool_init(struct btdm_mempool * mp, uint16_t blocks,
                                                    uint32_t block_size, void *membuf,
                                                    const char *name);
    return r_btdm_os_mempool_init(mp, blocks, block_size, membuf, name);
}

#if 0
/**
 * Initializes an extended memory pool.  Extended attributes (e.g., callbacks)
 * are not specified when this function is called; they are assigned manually
 * after initialization.
 *
 * @param mpe           The extended memory pool to initialize.
 * @param blocks        The number of blocks in the pool.
 * @param block_size    The size of each block, in bytes.
 * @param membuf        Pointer to memory to contain blocks.
 * @param name          Name of the pool.
 *
 * @return btdm_osal_error_t
 */
static inline btdm_osal_error_t
btdm_mempool_ext_init(struct btdm_mempool_ext *mpe, uint16_t blocks, uint32_t block_size,
                      void *membuf, const char *name)
{
    extern btdm_osal_error_t r_btdm_os_mempool_ext_init(struct btdm_mempool_ext * mpe,
                                                        uint16_t blocks, uint32_t block_size,
                                                        void *membuf, const char *name);
    return r_btdm_os_mempool_ext_init(mpe, blocks, block_size, membuf, name);
}
#endif

/**
 * Clears a memory pool.
 *
 * @param mp            The mempool to clear.
 *
 * @return btdm_osal_error_t
 */
static inline btdm_osal_error_t
btdm_mempool_clear(struct btdm_mempool *mp)
{
    extern btdm_osal_error_t r_btdm_os_mempool_clear(struct btdm_mempool * mp);
    return r_btdm_os_mempool_clear(mp);
}

/**
 * Performs an integrity check of the specified mempool.  This function
 * attempts to detect memory corruption in the specified memory pool.
 *
 * @param mp                    The mempool to check.
 *
 * @return                      true if the memory pool passes the integrity
 *                                  check;
 *                              false if the memory pool is corrupt.
 */
static inline bool
btdm_mempool_is_sane(const struct btdm_mempool *mp)
{
    extern bool r_btdm_os_mempool_is_sane(const struct btdm_mempool *mp);
    return r_btdm_os_mempool_is_sane(mp);
}

/**
 * Checks if a memory block was allocated from the specified mempool.
 *
 * @param mp                    The mempool to check as parent.
 * @param block_addr            The memory block to check as child.
 *
 * @return                      0 if the block does not belong to the mempool;
 *                              1 if the block does belong to the mempool.
 */
static inline int
btdm_memblock_from(const struct btdm_mempool *mp, const void *block_addr)
{
    extern int r_btdm_os_memblock_from(const struct btdm_mempool *mp, const void *block_addr);
    return r_btdm_os_memblock_from(mp, block_addr);
}

/**
 * Get a memory block from a memory pool
 *
 * @param mp Pointer to the memory pool
 *
 * @return void* Pointer to block if available; NULL otherwise
 */
static inline void *
btdm_memblock_get(struct btdm_mempool *mp)
{
    extern void *r_btdm_os_memblock_get(struct btdm_mempool * mp);
    return r_btdm_os_memblock_get(mp);
}

/**
 * Puts the memory block back into the pool, ignoring the put callback, if any.
 * This function should only be called from a put callback to free a block
 * without causing infinite recursion.
 *
 * @param mp Pointer to memory pool
 * @param block_addr Pointer to memory block
 *
 * @return btdm_osal_error_t
 */
static inline btdm_osal_error_t
btdm_mempool_put_from_cb(struct btdm_mempool *mp, void *block_addr)
{
    extern btdm_osal_error_t r_btdm_os_memblock_put_from_cb(struct btdm_mempool * mp,
                                                            void *block_addr);
    return r_btdm_os_memblock_put_from_cb(mp, block_addr);
}

/**
 * Puts the memory block back into the pool
 *
 * @param mp Pointer to memory pool
 * @param block_addr Pointer to memory block
 *
 * @return btdm_osal_error_t
 */
static inline btdm_osal_error_t
btdm_memblock_put(struct btdm_mempool *mp, void *block_addr)
{
    extern btdm_osal_error_t r_btdm_os_memblock_put(struct btdm_mempool * mp, void *block_addr);
    return r_btdm_os_memblock_put(mp, block_addr);
}

static inline uint16_t
btdm_memblock_free_num_get(struct btdm_mempool *mp)
{
    extern uint16_t r_btdm_os_memblock_num_free(struct btdm_mempool * mp);
    return r_btdm_os_memblock_num_free(mp);
}

#ifdef __cplusplus
}
#endif

#endif /* _BTDM_MEMPOOL_H_ */
