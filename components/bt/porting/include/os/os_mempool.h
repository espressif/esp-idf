/*
 * SPDX-FileCopyrightText: 2015-2022 The Apache Software Foundation (ASF)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2019-2026 Espressif Systems (Shanghai) CO LTD
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

/**
 * @addtogroup OSKernel
 * @{
 *   @defgroup OSMempool Memory Pools
 *   @{
 */


#ifndef _OS_MEMPOOL_H_
#define _OS_MEMPOOL_H_

#include <stdbool.h>
#include "os/os.h"

#if CONFIG_BT_NIMBLE_ENABLED
#include "syscfg/syscfg.h"
#endif // CONFIG_BT_NIMBLE_ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A memory block structure. This simply contains a pointer to the free list
 * chain and is only used when the block is on the free list. When the block
 * has been removed from the free list the entire memory block is usable by the
 * caller.
 */
struct os_memblock {
    SLIST_ENTRY(os_memblock) mb_next;
};

/* XXX: Change this structure so that we keep the first address in the pool? */
/* XXX: add memory debug structure and associated code */
/* XXX: Change how I coded the SLIST_HEAD here. It should be named:
   SLIST_HEAD(,os_memblock) mp_head; */

/**
 * Memory pool
 */
struct os_mempool {
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
    STAILQ_ENTRY(os_mempool) mp_list;
    SLIST_HEAD(,os_memblock);
    /** Name for memory block */
    const char *name;
#if CONFIG_BT_NIMBLE_ENABLED
#if MYNEWT_VAL(MP_BLOCK_REUSED)
    /** The number of allocated blocks. */
    uint16_t mp_alloc_blocks;
#endif // MYNEWT_VAL(MP_BLOCK_REUSED)
#endif // CONFIG_BT_NIMBLE_ENABLED
};

/**
 * Indicates an extended mempool.  Address can be safely cast to
 * (struct os_mempool_ext *).
 */
#define OS_MEMPOOL_F_EXT         BIT(0)
/* Flag to indicate runtime allocation mode */
#define OS_MEMPOOL_F_RUNTIME     BIT(1)
/* Flag to indicate reuse block for runtime allocation mode */
#define OS_MEMPOOL_F_REUSED      BIT(2)
/* Flag to indicate no address range comparison */
#define OS_MEMPOOL_F_FRAG        BIT(3)
/* Flag to indicate mempool source */
#define OS_MEMPOOL_F_CONTROLLER  BIT(4)
#define OS_MEMPOOL_F_LOW_PRIO    BIT(5)
#define OS_MEMPOOL_F_FORBID      BIT(6)
#define OS_MEMPOOL_F_COMBINATION BIT(7)

struct os_mempool_ext;

/**
 * Block put callback function.  If configured, this callback gets executed
 * whenever a block is freed to the corresponding extended mempool.  Note: The
 * os_memblock_put() function calls this callback instead of freeing the block
 * itself.  Therefore, it is the callback's responsibility to free the block
 * via a call to os_memblock_put_from_cb().
 *
 * @param ome                   The extended mempool that a block is being
 *                                  freed back to.
 * @param data                  The block being freed.
 * @param arg                   Optional argument configured along with the
 *                                  callback.
 *
 * @return                      Indicates whether the block was successfully
 *                                  freed.  A non-zero value should only be
 *                                  returned if the block was not successfully
 *                                  released back to its pool.
 */
typedef os_error_t os_mempool_put_fn(struct os_mempool_ext *ome, void *data,
                                     void *arg);

/**
 * Block get callback function.  If configured, this callback gets executed
 * whenever a block is fetched from the corresponding extended mempool.
 *
 * @param ome                   The extended mempool that a block is being
 *                                  fetched.
 * @param arg                   Optional argument configured along with the
 *                                  callback.
 *
 * @return  void *              pointer of block, return NULL if get failed
 */
typedef void *os_mempool_get_fn(struct os_mempool_ext *ome, void *arg);

struct os_mempool_ext {
    struct os_mempool mpe_mp;

    /* Callback that is executed immediately when a block is freed. */
    os_mempool_put_fn *mpe_put_cb;
    void *mpe_put_arg;
    os_mempool_get_fn *mpe_get_cb;
    void *mpe_get_arg;
};

#define OS_MEMPOOL_INFO_NAME_LEN (32)

/**
 * Information describing a memory pool, used to return OS information
 * to the management layer.
 */
struct os_mempool_info {
    /** Size of the memory blocks in the pool */
    int omi_block_size;
    /** Number of memory blocks in the pool */
    int omi_num_blocks;
    /** Number of free memory blocks */
    int omi_num_free;
    /** Minimum number of free memory blocks ever */
    int omi_min_free;
    /** Name of the memory pool */
    char omi_name[OS_MEMPOOL_INFO_NAME_LEN];
};

/**
 * Get information about the next system memory pool.
 *
 * @param mempool The current memory pool, or NULL if starting iteration.
 * @param info    A pointer to the structure to return memory pool information
 *                into.
 *
 * @return The next memory pool in the list to get information about, or NULL
 *         when at the last memory pool.
 */
struct os_mempool *os_mempool_info_get_next(struct os_mempool *,
                                            struct os_mempool_info *);

#if (OS_ALIGNMENT == 4)
typedef uint32_t os_membuf_t;
#elif (OS_ALIGNMENT == 8)
typedef uint64_t os_membuf_t;
#elif (OS_ALIGNMENT == 16)
typedef __uint128_t os_membuf_t;
#else
#error "Unhandled `OS_ALIGNMENT` for `os_membuf_t`"
#endif /* OS_ALIGNMENT == * */
#define OS_MEMPOOL_SIZE(n,blksize)      ((((blksize) + ((OS_ALIGNMENT)-1)) / (OS_ALIGNMENT)) * (n))

/** Calculates the number of bytes required to initialize a memory pool. */
#define OS_MEMPOOL_BYTES(n,blksize)     \
    (sizeof (os_membuf_t) * OS_MEMPOOL_SIZE((n), (blksize)))

/**
 * Initialize a memory pool.
 *
 * @param mp            Pointer to a pointer to a mempool
 * @param blocks        The number of blocks in the pool
 * @param blocks_size   The size of the block, in bytes.
 * @param membuf        Pointer to memory to contain blocks.
 * @param name          Name of the pool.
 *
 * @return os_error_t
 */
os_error_t os_mempool_init(struct os_mempool *mp, uint16_t blocks,
                           uint32_t block_size, void *membuf, const char *name);

/**
 * @brief Initialize a memory pool.
 * INTERNAL USE ONLY
 *
 * @param mp            Pointer to a pointer to a mempool
 * @param blocks        The number of blocks in the pool
 * @param blocks_size   The size of the block, in bytes.
 * @param membuf        Pointer to memory to contain blocks.
 * @param name          Name of the pool.
 * @param flags         Mempool flags
 *
 * @return os_error_t
 */
os_error_t
os_mempool_init_internal(struct os_mempool *mp, uint16_t blocks,
                         uint32_t block_size, void *membuf, const char *name,
                         uint8_t flags);

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
 * @return os_error_t
 */
os_error_t os_mempool_ext_init(struct os_mempool_ext *mpe, uint16_t blocks,
                               uint32_t block_size, void *membuf, const char *name);

/**
 * Assign the put and get callback for an extended memory pool.
 *
 * @param mpe      The extended memory pool
 * @param put_cb   Block put callback function
 * @param put_arg  Argument for put callback
 * @param get_cb   Block put callback function
 * @param get_arg  Argument for get callback
 */
void os_ext_mempool_register_cb(struct os_mempool_ext *mpe, void *put_cb, void *put_arg,
                                void *get_cb, void *get_arg);

/**
 * Removes the specified mempool from the list of initialized mempools.
 *
 * @param mp                    The mempool to unregister.
 *
 * @return                      0 on success;
 *                              OS_INVALID_PARM if the mempool is not
 *                                  registered.
 */
os_error_t os_mempool_unregister(struct os_mempool *mp);

/**
 * Clears a memory pool.
 *
 * @param mp            The mempool to clear.
 *
 * @return os_error_t
 */
os_error_t os_mempool_clear(struct os_mempool *mp);

/**
 * Clears an extended memory pool.
 *
 * @param mpe            The extended memory pool to clear.
 *
 * @return os_error_t
 */
os_error_t os_mempool_ext_clear(struct os_mempool_ext *mpe);

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
bool os_mempool_is_sane(const struct os_mempool *mp);

/**
 * Checks if a memory block was allocated from the specified mempool.
 *
 * @param mp                    The mempool to check as parent.
 * @param block_addr            The memory block to check as child.
 *
 * @return                      0 if the block does not belong to the mempool;
 *                              1 if the block does belong to the mempool.
 */
int os_memblock_from(const struct os_mempool *mp, const void *block_addr);

/**
 * Get a memory block from a memory pool
 *
 * @param mp Pointer to the memory pool
 *
 * @return void* Pointer to block if available; NULL otherwise
 */
void *os_memblock_get(struct os_mempool *mp);

/**
 * Puts the memory block back into the pool, ignoring the put callback, if any.
 * This function should only be called from a put callback to free a block
 * without causing infinite recursion.
 *
 * @param mp Pointer to memory pool
 * @param block_addr Pointer to memory block
 *
 * @return os_error_t
 */
os_error_t os_memblock_put_from_cb(struct os_mempool *mp, void *block_addr);

/**
 * Puts the memory block back into the pool
 *
 * @param mp Pointer to memory pool
 * @param block_addr Pointer to memory block
 *
 * @return os_error_t
 */
os_error_t os_memblock_put(struct os_mempool *mp, void *block_addr);

/**
 * @brief Set the mp_flags of memory pool
 *
 * @param mp Pointer to memory pool
 * @param flags Flags value
 */
void os_mempool_flags_set(struct os_mempool *mp, uint8_t flags);

/**
 * @brief Clear the mp_flags of memory pool
 *
 * @param mp Pointer to memory pool
 * @param flags Flags value
 */
void os_mempool_flags_clear(struct os_mempool *mp, uint8_t flags);

#ifdef __cplusplus
}
#endif

#endif  /* _OS_MEMPOOL_H_ */


/**
 *   @} OSMempool
 * @} OSKernel
 */
