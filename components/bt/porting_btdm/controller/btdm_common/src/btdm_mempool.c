/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "btdm_mempool.h"

#define BTDM_MEM_TRUE_BLOCK_SIZE(bsize)  BTDM_OSAL_ALIGN(bsize, BTDM_OSAL_ALIGNMENT)
#define BTDM_MEMPOOL_TRUE_BLOCK_SIZE(mp) BTDM_MEM_TRUE_BLOCK_SIZE(mp->mp_block_size)

static STAILQ_HEAD(, btdm_mempool) s_btdm_osal_mempool_list = STAILQ_HEAD_INITIALIZER(s_btdm_osal_mempool_list);

#if UC_BT_CTRL_RUNTIME_ALLOC_MEMPOOL
static btdm_osal_error_t
e_btdm_mempool_mem_free(struct btdm_mempool *mp)
{
    /*  Extended mempool has its own free logic */
    if (mp->mp_flags & BTDM_MEMPOOL_F_EXT) {
        return BTDM_OSAL_OK;
    }

    /* For runtime allocation mode, check whether all blocks have been freed */
    if (!(mp->mp_flags & BTDM_MEMPOOL_F_RUNTIME)) {
        return BTDM_OSAL_EINVAL;
    }

    /* NOTE: if mempool gets cleared before all mem block is retrieved, it may leads to mem trampling */
    assert(mp->mp_num_free == mp->mp_num_blocks);

#if UC_BT_CTRL_MEMPOOL_BLOCK_REUSED
    struct btdm_memblock *block_ptr;

    /* For block reused mode, free all allocated blocks */
    if (mp->mp_flags & BTDM_MEMPOOL_F_REUSED) {
        uint16_t cnt;
        for (cnt = 0; cnt < mp->mp_alloc_blocks; cnt ++) {
            block_ptr = SLIST_FIRST(mp);
            if (block_ptr) {
                assert(block_ptr);
                SLIST_REMOVE_HEAD(mp, mb_next);
                btdm_osal_free(block_ptr);
            }
        }
        mp->mp_alloc_blocks = 0;
    }
#endif

    /* Only reset statistics */
    SLIST_FIRST(mp) = NULL;
    mp->mp_min_free = mp->mp_num_blocks;

    return BTDM_OSAL_OK;
}
#endif // UC_BT_CTRL_RUNTIME_ALLOC_MEMPOOL

static btdm_osal_error_t
e_btdm_mempool_init_internal(struct btdm_mempool *mp, uint16_t blocks,
                           uint32_t block_size, void *membuf, const char *name,
                           uint8_t flags)
{
  int true_block_size;
  int i;
  uint8_t *block_addr;
  struct btdm_memblock *block_ptr;

    /* Check for valid parameters */
    if (!mp || (block_size == 0)) {
        return BTDM_OSAL_INVALID_PARM;
    }

    /* For runtime allocation mode, membuf can be NULL */
#if !UC_BT_CTRL_RUNTIME_ALLOC_MEMPOOL
    if ((!membuf) && (blocks != 0)) {
        return BTDM_OSAL_INVALID_PARM;
    }
#endif

    if (membuf != NULL) {
        /* Blocks need to be sized properly and memory buffer should be
         * aligned
         */
        if (((uint32_t)(uintptr_t)membuf & (BTDM_OSAL_ALIGNMENT - 1)) != 0) {
            return BTDM_OSAL_MEM_NOT_ALIGNED;
        }
    }

    /* Initialize the memory pool structure */
    mp->mp_block_size = block_size;
    mp->mp_num_free = blocks;
    mp->mp_min_free = blocks;
    mp->mp_flags = flags;
    mp->mp_num_blocks = blocks;
    mp->mp_membuf_addr = (uint32_t)(uintptr_t)membuf;
    mp->name = name;
    SLIST_FIRST(mp) = membuf;

#if UC_BT_CTRL_RUNTIME_ALLOC_MEMPOOL
    if (membuf == NULL) {
        /* Runtime allocation mode */
        mp->mp_membuf_addr = 0;
        mp->mp_flags |= BTDM_MEMPOOL_F_RUNTIME;
#if UC_BT_CTRL_MEMPOOL_BLOCK_REUSED
        mp->mp_flags |= BTDM_MEMPOOL_F_REUSED;
        mp->mp_alloc_blocks = 0;
#endif
        SLIST_FIRST(mp) = NULL;

        /* Check if mempool is already in the list (reinitialization case) */
        struct btdm_mempool *cur;
        if (!STAILQ_EMPTY(&s_btdm_osal_mempool_list)) {
            STAILQ_FOREACH(cur, &s_btdm_osal_mempool_list, mp_list) {
                if (cur->name == mp->name) {
                    /* Mempool is already in the list, remove it first */
                    btdm_mempool_unregister(cur);
                    break;
                }
            }
        }
        STAILQ_INSERT_TAIL(&s_btdm_osal_mempool_list, mp, mp_list);

        return BTDM_OSAL_OK;
    }
#endif
    if (blocks > 0) {
        true_block_size = BTDM_MEMPOOL_TRUE_BLOCK_SIZE(mp);

        /* Chain the memory blocks to the free list */
        block_addr = (uint8_t *)membuf;
        block_ptr = (struct btdm_memblock *)block_addr;
        for (i = 1; i < blocks; i++) {
            block_addr += true_block_size;
            SLIST_NEXT(block_ptr, mb_next) = (struct btdm_memblock *)block_addr;
            block_ptr = (struct btdm_memblock *)block_addr;
        }

        /* Last one in the list should be NULL */
        SLIST_NEXT(block_ptr, mb_next) = NULL;
    }

    /* Check if mempool is already in the list (reinitialization case) */
    struct btdm_mempool *cur;
    if (!STAILQ_EMPTY(&s_btdm_osal_mempool_list)) {
        STAILQ_FOREACH(cur, &s_btdm_osal_mempool_list, mp_list) {
            if (cur->name == mp->name) {
                /* Mempool is already in the list, remove it first */
                btdm_mempool_unregister(cur);
                break;
            }
        }
    }

    STAILQ_INSERT_TAIL(&s_btdm_osal_mempool_list, mp, mp_list);

    return BTDM_OSAL_OK;
}

btdm_osal_error_t
e_btdm_mempool_init(struct btdm_mempool *mp, uint16_t blocks, uint32_t block_size,
                void *membuf, const char *name)
{
    return e_btdm_mempool_init_internal(mp, blocks, block_size, membuf, name, 0);
}

btdm_osal_error_t
e_btdm_mempool_ext_init(struct btdm_mempool_ext *mpe, uint16_t blocks,
                      uint32_t block_size, void *membuf, const char *name)
{
    int rc;

    rc = e_btdm_mempool_init_internal(&mpe->mpe_mp, blocks, block_size, membuf,
                                      name, BTDM_MEMPOOL_F_EXT);
    if (rc != 0) {
        return rc;
    }

    mpe->mpe_put_cb = NULL;
    mpe->mpe_put_arg = NULL;
    mpe->mpe_get_cb = NULL;
    mpe->mpe_get_arg = NULL;

    return 0;
}

void
e_btdm_mempool_ext_register_cb(struct btdm_mempool_ext *mpe,
                             void *put_cb, void *put_arg,
                             void *get_cb, void *get_arg)
{
    mpe->mpe_put_cb = put_cb;
    mpe->mpe_put_arg = put_arg;
    mpe->mpe_get_cb = get_cb;
    mpe->mpe_get_arg = get_arg;
}

btdm_osal_error_t
e_btdm_mempool_unregister(struct btdm_mempool *mp)
{
    struct btdm_mempool *cur;

    /* Remove the mempool from the global stailq.  This is done manually rather
     * than with `STAILQ_REMOVE` to allow for a graceful failure if the mempool
     * isn't found.
     */
    cur = NULL;
    if (!STAILQ_EMPTY(&s_btdm_osal_mempool_list)) {
        STAILQ_FOREACH(cur, &s_btdm_osal_mempool_list, mp_list) {
            if (cur == mp) {
                break;
            }
        }
    }

    if (cur == NULL) {
        return BTDM_OSAL_INVALID_PARM;
    }

#if UC_BT_CTRL_RUNTIME_ALLOC_MEMPOOL
    e_btdm_mempool_mem_free(cur);
#endif // UC_BT_CTRL_RUNTIME_ALLOC_MEMPOOL
    STAILQ_REMOVE(&s_btdm_osal_mempool_list, cur, btdm_mempool, mp_list);

    return 0;
}

btdm_osal_error_t
e_btdm_mempool_clear(struct btdm_mempool *mp)
{
    struct btdm_memblock *block_ptr;
    int true_block_size;
    uint8_t *block_addr;
    uint16_t blocks;

    if (!mp) {
        return BTDM_OSAL_INVALID_PARM;
    }

#if UC_BT_CTRL_RUNTIME_ALLOC_MEMPOOL
    if (e_btdm_mempool_mem_free(mp) == BTDM_OSAL_OK) {
        return BTDM_OSAL_OK;
    }
#endif // UC_BT_CTRL_RUNTIME_ALLOC_MEMPOOL

    true_block_size = BTDM_MEMPOOL_TRUE_BLOCK_SIZE(mp);

    /* cleanup the memory pool structure */
    mp->mp_num_free = mp->mp_num_blocks;
    mp->mp_min_free = mp->mp_num_blocks;
    SLIST_FIRST(mp) = (void *)(uintptr_t)mp->mp_membuf_addr;

    /* Chain the memory blocks to the free list */
    block_addr = (uint8_t *)(uintptr_t)mp->mp_membuf_addr;
    block_ptr = (struct btdm_memblock *)block_addr;
    blocks = mp->mp_num_blocks;

    while (blocks > 1) {
        block_addr += true_block_size;
        SLIST_NEXT(block_ptr, mb_next) = (struct btdm_memblock *)block_addr;
        block_ptr = (struct btdm_memblock *)block_addr;
        --blocks;
    }

    /* Last one in the list should be NULL */
    SLIST_NEXT(block_ptr, mb_next) = NULL;

    return BTDM_OSAL_OK;
}

btdm_osal_error_t
e_btdm_mempool_ext_clear(struct btdm_mempool_ext *mpe)
{
    btdm_osal_error_t rc;

    mpe->mpe_put_cb = NULL;
    mpe->mpe_put_arg = NULL;
    mpe->mpe_get_cb = NULL;
    mpe->mpe_get_arg = NULL;

    rc = btdm_mempool_clear(&mpe->mpe_mp);
#if UC_BT_CTRL_RUNTIME_ALLOC_MEMPOOL
    mpe->mpe_mp.mp_flags &= ~BTDM_MEMPOOL_F_EXT;
#else
    mpe->mpe_mp.mp_flags = 0;
#endif
    return rc;
}

bool
e_btdm_mempool_is_sane(const struct btdm_mempool *mp)
{
    struct btdm_memblock *block;

#if UC_BT_CTRL_RUNTIME_ALLOC_MEMPOOL
    /* Runtime mode cannot verify sanity */
    if (mp->mp_flags & BTDM_MEMPOOL_F_RUNTIME) {
        assert(0);
    }
#endif

    /* Verify that each block in the free list belongs to the mempool. */
    SLIST_FOREACH(block, mp, mb_next) {
        if (!e_btdm_memblock_from(mp, block)) {
            return false;
        }
    }

    return true;
}

int
e_btdm_memblock_from(const struct btdm_mempool *mp, const void *block_addr)
{
    uint32_t true_block_size;
    uintptr_t baddr32;
    uint32_t end;

#if UC_BT_CTRL_RUNTIME_ALLOC_MEMPOOL
    /* Runtime allocation mode doesn't support from */
    if (mp->mp_flags & BTDM_MEMPOOL_F_RUNTIME) {
        assert(0);
    }
#endif

    static_assert(sizeof block_addr == sizeof baddr32,
                  "Pointer to void must be 32-bits.");

    baddr32 = (uint32_t)(uintptr_t)block_addr;
    true_block_size = BTDM_MEMPOOL_TRUE_BLOCK_SIZE(mp);
    end = mp->mp_membuf_addr + (mp->mp_num_blocks * true_block_size);

    /* Check that the block is in the memory buffer range. */
    if ((baddr32 < mp->mp_membuf_addr) || (baddr32 >= end)) {
        return 0;
    }

    /* All freed blocks should be on true block size boundaries! */
    if (!(mp->mp_flags & BTDM_MEMPOOL_F_COMBINATION) &&
        ((baddr32 - mp->mp_membuf_addr) % true_block_size) != 0) {
        return 0;
    }

    return 1;
}

void *
e_btdm_memblock_get(struct btdm_mempool *mp)
{
    struct btdm_mempool_ext *mpe;
    struct btdm_memblock *block;

    /* Check to make sure they passed in a memory pool (or something) */
    block = NULL;

    if (mp && mp->mp_flags & BTDM_MEMPOOL_F_EXT) {
        mpe = (struct btdm_mempool_ext *)mp;
        if (mpe->mpe_get_cb != NULL) {
            block = mpe->mpe_get_cb(mpe, mpe->mpe_get_arg);
            return block;
        }
    }
#if UC_BT_CTRL_RUNTIME_ALLOC_MEMPOOL
    /* Runtime allocation mode */
    if (mp && mp->mp_flags & BTDM_MEMPOOL_F_RUNTIME) {
        bool need_alloc = false;
        void *allocated_block;
        uint32_t alloc_size;

        btdm_osal_hw_enter_critical();

        if (mp->mp_num_free) {
#if UC_BT_CTRL_MEMPOOL_BLOCK_REUSED
            if (mp->mp_flags & BTDM_MEMPOOL_F_REUSED) {
                if (!SLIST_EMPTY(mp)) {
                    block = SLIST_FIRST(mp);
                    SLIST_REMOVE_HEAD(mp, mb_next);
                } else {
                    assert(mp->mp_alloc_blocks < mp->mp_num_blocks);
                    need_alloc = true;
                    mp->mp_alloc_blocks++;
                }
            } else
#endif
            {
                need_alloc = true;
            }
            /* Decrement number free by 1 */
            mp->mp_num_free--;
            if (mp->mp_min_free > mp->mp_num_free) {
                mp->mp_min_free = mp->mp_num_free;
            }
        }

        btdm_osal_hw_exit_critical(0);

        /* Allocate outside critical section to avoid holding lock too long */
        if (need_alloc) {
            alloc_size = BTDM_MEMPOOL_TRUE_BLOCK_SIZE(mp);
            allocated_block = btdm_osal_malloc(alloc_size, 0);

            if (allocated_block) {
                /* Save mempool pointer for block */
                block = (struct btdm_memblock *)(allocated_block);
            } else {
                // Should not happen
                btdm_osal_hw_enter_critical();
                mp->mp_num_free++;
#if UC_BT_CTRL_MEMPOOL_BLOCK_REUSED
                if (mp->mp_flags & BTDM_MEMPOOL_F_REUSED) {
                    mp->mp_alloc_blocks--;
                }
#endif
                btdm_osal_hw_exit_critical(0);
            }
        }

        return block;
    }
#endif

    if (mp) {
        btdm_osal_hw_enter_critical();
        /* Check for any free */
        if (mp->mp_num_free) {
            /* Get a free block */
            block = SLIST_FIRST(mp);

            /* Set new free list head */
            SLIST_FIRST(mp) = SLIST_NEXT(block, mb_next);

            /* Decrement number free by 1 */
            mp->mp_num_free--;
            if (mp->mp_min_free > mp->mp_num_free) {
                mp->mp_min_free = mp->mp_num_free;
            }
        }
        btdm_osal_hw_exit_critical(0);
    }


    return (void *)block;
}

btdm_osal_error_t
e_btdm_memblock_put_from_cb(struct btdm_mempool *mp, void *block_addr)
{
    struct btdm_memblock *block;

#if UC_BT_CTRL_RUNTIME_ALLOC_MEMPOOL
    bool need_free = true;

    if (mp->mp_flags & BTDM_MEMPOOL_F_RUNTIME) {

        /* Runtime allocation mode - free directly */
        btdm_osal_hw_enter_critical();
#if UC_BT_CTRL_MEMPOOL_BLOCK_REUSED
        if (mp->mp_flags & BTDM_MEMPOOL_F_REUSED) {
            block = (struct btdm_memblock *)block_addr;
            SLIST_INSERT_HEAD(mp, block, mb_next);
            need_free = false;
        }
#endif
        mp->mp_num_free++;
        assert(mp->mp_num_blocks >= mp->mp_num_free);
        btdm_osal_hw_exit_critical(0);

        /* Free outside critical section */
        if (need_free) {
            btdm_osal_free(block_addr);
        }

        return BTDM_OSAL_OK;
    }
#endif
    /* Validate that the block belongs to this mempool */
    if (!(mp->mp_flags & BTDM_MEMPOOL_F_FRAG) &&
        !e_btdm_memblock_from(mp, block_addr)) {
        return BTDM_OSAL_INVALID_PARM;
    }

    block = (struct btdm_memblock *)block_addr;
    btdm_osal_hw_enter_critical();

    /* Check for duplicate free - verify block is not already in free list */
    {
        struct btdm_memblock *cur;
        SLIST_FOREACH(cur, mp, mb_next) {
            if (cur == block) {
                btdm_osal_hw_exit_critical(0);
                return BTDM_OSAL_INVALID_PARM;
            }
        }
    }

    /* Check that the number free doesn't exceed number blocks */
    if (mp->mp_num_free >= mp->mp_num_blocks) {
        btdm_osal_hw_exit_critical(0);
        return BTDM_OSAL_INVALID_PARM;
    }

    /* Chain current free list pointer to this block; make this block head */
    SLIST_NEXT(block, mb_next) = SLIST_FIRST(mp);
    SLIST_FIRST(mp) = block;

    /* Increment number free */
    mp->mp_num_free++;

    btdm_osal_hw_exit_critical(0);

    return BTDM_OSAL_OK;
}

btdm_osal_error_t
e_btdm_memblock_put(struct btdm_mempool *mp, void *block_addr)
{
    struct btdm_mempool_ext *mpe;
    btdm_osal_error_t ret;
#if UC_BT_CTRL_MEMPOOL_CHECK
    struct btdm_memblock *block;
#endif

    /* Make sure parameters are valid */
    if ((mp == NULL) || (block_addr == NULL)) {
        ret = BTDM_OSAL_INVALID_PARM;
        goto done;
    }

#if UC_BT_CTRL_MEMPOOL_CHECK
#if UC_BT_CTRL_RUNTIME_ALLOC_MEMPOOL
    if (!(mp->mp_flags & BTDM_MEMPOOL_F_RUNTIME))
#endif
    {
        /* Check that the block we are freeing is a valid block! */
        assert(btdm_memblock_from(mp, block_addr));
    }

    /*
     * Check for duplicate free.
     */
    SLIST_FOREACH(block, mp, mb_next) {
        assert(block != (struct btdm_memblock *)block_addr);
    }
#endif

    /* If this is an extended mempool with a put callback, call the callback
     * instead of freeing the block directly.
     */
    if (mp->mp_flags & BTDM_MEMPOOL_F_EXT) {
        mpe = (struct btdm_mempool_ext *)mp;
        if (mpe->mpe_put_cb != NULL) {
            ret = mpe->mpe_put_cb(mpe, block_addr, mpe->mpe_put_arg);
            goto done;
        }
    }

    /* No callback; free the block. */
    ret = btdm_memblock_put_from_cb(mp, block_addr);

done:
    return ret;
}

void
e_btdm_mempool_flags_set(struct btdm_mempool *mp, uint8_t flags)
{
    mp->mp_flags |= flags;
}

void
e_btdm_mempool_flags_clear(struct btdm_mempool *mp, uint8_t flags)
{
    mp->mp_flags &= ~flags;
}

uint16_t
e_btdm_mempool_free_blocks_num(struct btdm_mempool *mp)
{
    uint16_t free_blocks_num;

    btdm_osal_hw_enter_critical();
    free_blocks_num = mp->mp_num_free;
    btdm_osal_hw_exit_critical(0);

    return free_blocks_num;
}

void
e_btdm_mempool_module_init(void)
{

}

void
e_btdm_mempool_deinit(struct btdm_mempool *mp)
{
    btdm_mempool_unregister(mp);
}

btdm_osal_error_t
e_btdm_mempool_deinit_all(bool is_controller)
{
    btdm_osal_error_t err = BTDM_OSAL_INVALID_PARM;
    struct btdm_mempool *mp = NULL;
    struct btdm_mempool *next = NULL;

    mp = STAILQ_FIRST(&s_btdm_osal_mempool_list);

    // All mempool blocks should be reclaimed and mempool removed from mempool list after nimble deinit
    while (mp) {
        if (is_controller == !!(mp->mp_flags & BTDM_MEMPOOL_F_CONTROLLER)) {
            next = STAILQ_NEXT(mp, mp_list);
            btdm_mempool_unregister(mp);
            mp = next;
            err = BTDM_OSAL_OK;
        } else {
            mp = STAILQ_NEXT(mp, mp_list);
        }
    }

    return  err;
}

bool
e_btdm_mempool_has_live_pool(void)
{
    return  !STAILQ_EMPTY(&s_btdm_osal_mempool_list);
}
