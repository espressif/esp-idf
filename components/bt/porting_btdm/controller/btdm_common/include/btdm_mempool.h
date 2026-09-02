/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
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
#include "btdm_user_cfg.h"
#include "queue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BTDM_MEMPOOL_EXT_FUNC(f) e_##f

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
#if UC_BT_CTRL_MEMPOOL_BLOCK_REUSED
  /** The number of allocated blocks. */
  uint16_t mp_alloc_blocks;
#endif // UC_BT_CTRL_MEMPOOL_BLOCK_REUSED
};

#define BTDM_ALIGNMENT 4
#define BTDM_MEMPOOL_SIZE(n, blksize)                                          \
  ((((blksize) + ((BTDM_ALIGNMENT)-1)) / (BTDM_ALIGNMENT)) * (n))

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
 * Indicates an extended mempool.  Address can be safely cast to
 * (struct btdm_mempool_ext *).
 */
#define BTDM_MEMPOOL_F_EXT (1 << 0)
/* Flag to indicate runtime allocation mode */
#define BTDM_MEMPOOL_F_RUNTIME (1 << 1)
/* Flag to indicate reuse block for runtime allocation mode */
#define BTDM_MEMPOOL_F_REUSED (1 << 2)
/* Flag to indicate no address range comparison */
#define BTDM_MEMPOOL_F_FRAG (1 << 3)
/* Flag to indicate mempool source */
#define BTDM_MEMPOOL_F_CONTROLLER (1 << 4)
#define BTDM_MEMPOOL_F_LOW_PRIO (1 << 5)
#define BTDM_MEMPOOL_F_FORBID (1 << 6)
#define BTDM_MEMPOOL_F_COMBINATION (1 << 7)

struct btdm_mempool_ext;

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
typedef btdm_osal_error_t btdm_mempool_put_fn(struct btdm_mempool_ext *ome,
                                              void *data, void *arg);

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
typedef void *btdm_mempool_get_fn(struct btdm_mempool_ext *ome, void *arg);

struct btdm_mempool_ext {
  struct btdm_mempool mpe_mp;

  /* Callback that is executed immediately when a block is freed. */
  btdm_mempool_put_fn *mpe_put_cb;
  void *mpe_put_arg;
  btdm_mempool_get_fn *mpe_get_cb;
  void *mpe_get_arg;
};

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
btdm_osal_error_t e_btdm_mempool_init(struct btdm_mempool *mp, uint16_t blocks,
                                      uint32_t block_size, void *membuf,
                                      const char *name);
#define btdm_mempool_init BTDM_MEMPOOL_EXT_FUNC(btdm_mempool_init)

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
btdm_osal_error_t e_btdm_mempool_ext_init(struct btdm_mempool_ext *mpe,
                                          uint16_t blocks, uint32_t block_size,
                                          void *membuf, const char *name);
#define btdm_mempool_ext_init BTDM_MEMPOOL_EXT_FUNC(btdm_mempool_ext_init)

/**
 * Assign the put and get callback for an extended memory pool.
 *
 * @param mpe      The extended memory pool
 * @param put_cb   Block put callback function
 * @param put_arg  Argument for put callback
 * @param get_cb   Block put callback function
 * @param get_arg  Argument for get callback
 */
void e_btdm_mempool_ext_register_cb(struct btdm_mempool_ext *mpe, void *put_cb,
                                    void *put_arg, void *get_cb, void *get_arg);
#define btdm_mempool_ext_register_cb                                           \
  BTDM_MEMPOOL_EXT_FUNC(btdm_mempool_ext_register_cb)

/**
 * Removes the specified mempool from the list of initialized mempools.
 *
 * @param mp                    The mempool to unregister.
 *
 * @return                      0 on success;
 *                              OS_INVALID_PARM if the mempool is not
 *                                  registered.
 */
btdm_osal_error_t e_btdm_mempool_unregister(struct btdm_mempool *mp);
#define btdm_mempool_unregister BTDM_MEMPOOL_EXT_FUNC(btdm_mempool_unregister)

/**
 * Clears a memory pool.
 *
 * @param mp            The mempool to clear.
 *
 * @return btdm_osal_error_t
 */
btdm_osal_error_t e_btdm_mempool_clear(struct btdm_mempool *mp);
#define btdm_mempool_clear BTDM_MEMPOOL_EXT_FUNC(btdm_mempool_clear)

/**
 * Clears an extended memory pool.
 *
 * @param mpe           The extended memory pool to clear.
 *
 * @return btdm_osal_error_t
 */
btdm_osal_error_t e_btdm_mempool_ext_clear(struct btdm_mempool_ext *mpe);
#define btdm_mempool_ext_clear BTDM_MEMPOOL_EXT_FUNC(btdm_mempool_ext_clear)

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
bool e_btdm_mempool_is_sane(const struct btdm_mempool *mp);
#define btdm_mempool_is_sane BTDM_MEMPOOL_EXT_FUNC(btdm_mempool_is_sane)

/**
 * Checks if a memory block was allocated from the specified mempool.
 *
 * @param mp                    The mempool to check as parent.
 * @param block_addr            The memory block to check as child.
 *
 * @return                      0 if the block does not belong to the mempool;
 *                              1 if the block does belong to the mempool.
 */
int e_btdm_memblock_from(const struct btdm_mempool *mp, const void *block_addr);
#define btdm_memblock_from BTDM_MEMPOOL_EXT_FUNC(btdm_memblock_from)

/**
 * Get a memory block from a memory pool
 *
 * @param mp Pointer to the memory pool
 *
 * @return void* Pointer to block if available; NULL otherwise
 */
void *e_btdm_memblock_get(struct btdm_mempool *mp);
#define btdm_memblock_get BTDM_MEMPOOL_EXT_FUNC(btdm_memblock_get)

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
btdm_osal_error_t e_btdm_memblock_put_from_cb(struct btdm_mempool *mp, void *block_addr);
#define btdm_memblock_put_from_cb BTDM_MEMPOOL_EXT_FUNC(btdm_memblock_put_from_cb)

/**
 * Puts the memory block back into the pool
 *
 * @param mp Pointer to memory pool
 * @param block_addr Pointer to memory block
 *
 * @return btdm_osal_error_t
 */
btdm_osal_error_t e_btdm_memblock_put(struct btdm_mempool *mp,
                                      void *block_addr);
#define btdm_memblock_put BTDM_MEMPOOL_EXT_FUNC(btdm_memblock_put)

/**
 * Set the flags of a memory pool.
 *
 * @param mp Pointer to memory pool
 * @param flags Flags value
 */
void e_btdm_mempool_flags_set(struct btdm_mempool *mp, uint8_t flags);
#define btdm_mempool_flags_set BTDM_MEMPOOL_EXT_FUNC(btdm_mempool_flags_set)

/**
 * Clear the flags of a memory pool.
 *
 * @param mp Pointer to memory pool
 * @param flags Flags value
 */
void e_btdm_mempool_flags_clear(struct btdm_mempool *mp, uint8_t flags);
#define btdm_mempool_flags_clear BTDM_MEMPOOL_EXT_FUNC(btdm_mempool_flags_clear)

/**
 * Get the number of free blocks in a memory pool.
 *
 * @param mp Pointer to memory pool
 *
 * @return uint16_t Number of free blocks
 */
uint16_t e_btdm_mempool_free_blocks_num(struct btdm_mempool *mp);
#define btdm_mempool_free_blocks_num                                           \
  BTDM_MEMPOOL_EXT_FUNC(btdm_mempool_free_blocks_num)

/**
 * Initialize the memory pool module.
 */
void e_btdm_mempool_module_init(void);
#define btdm_mempool_module_init BTDM_MEMPOOL_EXT_FUNC(btdm_mempool_module_init)

/**
 * Deinitialize a memory pool.
 *
 * @param mp Pointer to memory pool
 */
void e_btdm_mempool_deinit(struct btdm_mempool *mp);
#define btdm_mempool_deinit BTDM_MEMPOOL_EXT_FUNC(btdm_mempool_deinit)

/**
 * Deinitialize all of memory pools.
 *
 * @param is_controller Whether called from controller.
 *
 * @return BTDM_OSAL_OK on success; BTDM_OSAL_INVALID_PARM if not found corresponding memory pools.
 */
btdm_osal_error_t e_btdm_mempool_deinit_all(bool is_controller);
#define btdm_mempool_deinit_all BTDM_MEMPOOL_EXT_FUNC(btdm_mempool_deinit_all)

/**
 * Check if there are any live memory pools.
 *
 * @return true if there are any live memory pools; false otherwise.
 */
bool e_btdm_mempool_has_live_pool(void);
#define btdm_mempool_has_live_pool BTDM_MEMPOOL_EXT_FUNC(btdm_mempool_has_live_pool)

#ifdef __cplusplus
}
#endif

#endif /* _BTDM_MEMPOOL_H_ */
