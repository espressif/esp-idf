/*
 * SPDX-FileCopyrightText: 2015-2022 The Apache Software Foundation (ASF)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2019-2026 Espressif Systems (Shanghai) CO LTD
 */

#include "os/os.h"

#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "mem_api.h"
#include "esp_nimble_mem.h"
#if CONFIG_BT_NIMBLE_ENABLED
#include "os/os_trace_api.h"
#include "syscfg/syscfg.h"
#include "modlog/modlog.h"
#if !MYNEWT_VAL(OS_SYSVIEW_TRACE_MEMPOOL)
#define OS_TRACE_DISABLE_FILE_API
#endif
#endif

#if CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY
#define MEMPOOL_IRAM_ATTR
#else
#define MEMPOOL_IRAM_ATTR           IRAM_ATTR
#endif // CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY

#define OS_MEM_TRUE_BLOCK_SIZE(bsize)   OS_ALIGN(bsize, OS_ALIGNMENT)
#if MYNEWT_VAL(OS_MEMPOOL_GUARD)
#define OS_MEMPOOL_TRUE_BLOCK_SIZE(mp)                                  \
    (((mp)->mp_flags & OS_MEMPOOL_F_EXT) ?                              \
     OS_MEM_TRUE_BLOCK_SIZE(mp->mp_block_size) :                       \
     (OS_MEM_TRUE_BLOCK_SIZE(mp->mp_block_size) + sizeof(os_membuf_t)))
#else
#define OS_MEMPOOL_TRUE_BLOCK_SIZE(mp) OS_MEM_TRUE_BLOCK_SIZE(mp->mp_block_size)
#endif

#if !MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)
STAILQ_HEAD(, os_mempool) g_os_mempool_list = STAILQ_HEAD_INITIALIZER(g_os_mempool_list);
#else
STAILQ_HEAD(, os_mempool) g_os_mempool_list;
static bool g_os_mempool_list_inited;
#endif // !MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)

#if MYNEWT_VAL(OS_MEMPOOL_POISON)
static uint32_t os_mem_poison = 0xde7ec7ed;

static_assert(sizeof(struct os_memblock) % 4 == 0, "sizeof(struct os_memblock) shall be aligned to 4");
static_assert(sizeof(os_mem_poison) == 4, "sizeof(os_mem_poison) shall be 4");

static void MEMPOOL_IRAM_ATTR
os_mempool_poison(const struct os_mempool *mp, void *start)
{
    uint32_t *p;
    uint32_t *end;
    int sz;

    sz = OS_MEM_TRUE_BLOCK_SIZE(mp->mp_block_size);
    p = start;
    end = p + sz / 4;
    p += sizeof(struct os_memblock) / 4;

    while (p < end) {
        *p = os_mem_poison;
        p++;
    }
}

static void MEMPOOL_IRAM_ATTR
os_mempool_poison_check(const struct os_mempool *mp, void *start)
{
    uint32_t *p;
    uint32_t *end;
    int sz;

    sz = OS_MEM_TRUE_BLOCK_SIZE(mp->mp_block_size);
    p = start;
    end = p + sz / 4;
    p += sizeof(struct os_memblock) / 4;

    while (p < end) {
        assert(*p == os_mem_poison);
        p++;
    }
}
#else
#define os_mempool_poison(mp, start)
#define os_mempool_poison_check(mp, start)
#endif
#if MYNEWT_VAL(OS_MEMPOOL_GUARD)
#define OS_MEMPOOL_GUARD_PATTERN 0xBAFF1ED1

static void MEMPOOL_IRAM_ATTR
os_mempool_guard(const struct os_mempool *mp, void *start)
{
    uint32_t *tgt;

    if ((mp->mp_flags & OS_MEMPOOL_F_EXT) == 0) {
        tgt = (uint32_t *)((uintptr_t)start +
                           OS_MEM_TRUE_BLOCK_SIZE(mp->mp_block_size));
        *tgt = OS_MEMPOOL_GUARD_PATTERN;
    }
}

static void MEMPOOL_IRAM_ATTR
os_mempool_guard_check(const struct os_mempool *mp, void *start)
{
    uint32_t *tgt;

    if ((mp->mp_flags & OS_MEMPOOL_F_EXT) == 0) {
        tgt = (uint32_t *)((uintptr_t)start +
                           OS_MEM_TRUE_BLOCK_SIZE(mp->mp_block_size));
        assert(*tgt == OS_MEMPOOL_GUARD_PATTERN);
    }
}
#else
#define os_mempool_guard(mp, start)
#define os_mempool_guard_check(mp, start)
#endif

#if MYNEWT_VAL(MP_RUNTIME_ALLOC)
static os_error_t os_mempool_mem_free(struct os_mempool *mp);
#endif // MYNEWT_VAL(MP_RUNTIME_ALLOC)

#if MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)
static void
os_mempool_list_ensure_init(void)
{
    if (!g_os_mempool_list_inited) {
        STAILQ_INIT(&g_os_mempool_list);
        g_os_mempool_list_inited = true;
    }
}
#endif // MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)

os_error_t
os_mempool_init_internal(struct os_mempool *mp, uint16_t blocks,
                         uint32_t block_size, void *membuf, const char *name,
                         uint8_t flags)
{
    int true_block_size;
    int i;
    uint8_t *block_addr;
    struct os_memblock *block_ptr;

    /* Check for valid parameters */
    if (!mp || (block_size == 0)) {
        return OS_INVALID_PARM;
    }

    /* For runtime allocation mode, membuf can be NULL */
#if !MYNEWT_VAL(MP_RUNTIME_ALLOC)
    if ((!membuf) && (blocks != 0)) {
        return OS_INVALID_PARM;
    }
#endif

    if (membuf != NULL) {
        /* Blocks need to be sized properly and memory buffer should be
         * aligned
         */
        if (((uint32_t)(uintptr_t)membuf & (OS_ALIGNMENT - 1)) != 0) {
            return OS_MEM_NOT_ALIGNED;
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

#if MYNEWT_VAL(MP_RUNTIME_ALLOC)
    if (membuf == NULL) {
        /* Runtime allocation mode */
        mp->mp_membuf_addr = 0;
        mp->mp_flags |= OS_MEMPOOL_F_RUNTIME;
#if MYNEWT_VAL(MP_BLOCK_REUSED)
        mp->mp_flags |= OS_MEMPOOL_F_REUSED;
        mp->mp_alloc_blocks = 0;
#endif
        SLIST_FIRST(mp) = NULL;
#if MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)
        os_mempool_list_ensure_init();
#endif // MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)

        /* Check if mempool is already in the list (reinitialization case) */
        struct os_mempool *cur;
        if (!STAILQ_EMPTY(&g_os_mempool_list)) {
            STAILQ_FOREACH(cur, &g_os_mempool_list, mp_list) {
                if (cur->name == mp->name) {
                    /* Mempool is already in the list, remove it first */
                    os_mempool_unregister(cur);
                    break;
                }
            }
        }
        STAILQ_INSERT_TAIL(&g_os_mempool_list, mp, mp_list);
        return OS_OK;
    }
#endif
    if (blocks > 0) {
        os_mempool_poison(mp, membuf);
        os_mempool_guard(mp, membuf);
        true_block_size = OS_MEMPOOL_TRUE_BLOCK_SIZE(mp);

        /* Chain the memory blocks to the free list */
        block_addr = (uint8_t *)membuf;
        block_ptr = (struct os_memblock *)block_addr;
        for (i = 1; i < blocks; i++) {
            block_addr += true_block_size;
            os_mempool_poison(mp, block_addr);
            os_mempool_guard(mp, block_addr);
            SLIST_NEXT(block_ptr, mb_next) = (struct os_memblock *)block_addr;
            block_ptr = (struct os_memblock *)block_addr;
        }

        /* Last one in the list should be NULL */
        SLIST_NEXT(block_ptr, mb_next) = NULL;
    }

#if MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)
    os_mempool_list_ensure_init();
#endif // MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)
    /* Check if mempool is already in the list (reinitialization case) */
    struct os_mempool *cur;
    if (!STAILQ_EMPTY(&g_os_mempool_list)) {
        STAILQ_FOREACH(cur, &g_os_mempool_list, mp_list) {
            if (cur->name == mp->name) {
                /* Mempool is already in the list, remove it first */
                os_mempool_unregister(cur);
                break;
            }
        }
    }

    STAILQ_INSERT_TAIL(&g_os_mempool_list, mp, mp_list);

    return OS_OK;
}

os_error_t
os_mempool_init(struct os_mempool *mp, uint16_t blocks, uint32_t block_size,
                void *membuf, const char *name)
{
    return os_mempool_init_internal(mp, blocks, block_size, membuf, name, 0);
}

os_error_t
os_mempool_ext_init(struct os_mempool_ext *mpe, uint16_t blocks,
                    uint32_t block_size, void *membuf, const char *name)
{
    int rc;

    rc = os_mempool_init_internal(&mpe->mpe_mp, blocks, block_size, membuf,
                                  name, OS_MEMPOOL_F_EXT);
    if (rc != 0) {
        return rc;
    }

    mpe->mpe_put_cb = NULL;
    mpe->mpe_put_arg = NULL;
    mpe->mpe_get_cb = NULL;
    mpe->mpe_get_arg = NULL;

    return 0;
}

void os_ext_mempool_register_cb(struct os_mempool_ext *mpe,
                                void *put_cb, void *put_arg,
                                void *get_cb, void *get_arg)
{
    mpe->mpe_put_cb = put_cb;
    mpe->mpe_put_arg = put_arg;
    mpe->mpe_get_cb = get_cb;
    mpe->mpe_get_arg = get_arg;
}

os_error_t
os_mempool_unregister(struct os_mempool *mp)
{
    struct os_mempool *cur;

    /* Remove the mempool from the global stailq.  This is done manually rather
     * than with `STAILQ_REMOVE` to allow for a graceful failure if the mempool
     * isn't found.
     */
#if MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)
    os_mempool_list_ensure_init();
#endif // MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)

    cur = NULL;
    if (!STAILQ_EMPTY(&g_os_mempool_list)) {
        STAILQ_FOREACH(cur, &g_os_mempool_list, mp_list) {
            if (cur == mp) {
                break;
            }
        }
    }

    if (cur == NULL) {
        return OS_INVALID_PARM;
    }

#if MYNEWT_VAL(MP_RUNTIME_ALLOC)
    os_mempool_mem_free(cur);
#endif // MYNEWT_VAL(MP_RUNTIME_ALLOC)
    STAILQ_REMOVE(&g_os_mempool_list, cur, os_mempool, mp_list);
#if MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)
    if (STAILQ_EMPTY(&g_os_mempool_list)) {
        g_os_mempool_list_inited = false;
    }
#endif // MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)
    return 0;
}

os_error_t
os_mempool_clear(struct os_mempool *mp)
{
    struct os_memblock *block_ptr;
    int true_block_size;
    uint8_t *block_addr;
    uint16_t blocks;

    if (!mp) {
        return OS_INVALID_PARM;
    }

#if MYNEWT_VAL(MP_RUNTIME_ALLOC)
    if (os_mempool_mem_free(mp) == OS_OK) {
        return OS_OK;
    }
#endif // MYNEWT_VAL(MP_RUNTIME_ALLOC)

    true_block_size = OS_MEMPOOL_TRUE_BLOCK_SIZE(mp);

    /* cleanup the memory pool structure */
    mp->mp_num_free = mp->mp_num_blocks;
    mp->mp_min_free = mp->mp_num_blocks;
    os_mempool_poison(mp, (void *)mp->mp_membuf_addr);
    os_mempool_guard(mp, (void *)mp->mp_membuf_addr);
    SLIST_FIRST(mp) = (void *)(uintptr_t)mp->mp_membuf_addr;

    /* Chain the memory blocks to the free list */
    block_addr = (uint8_t *)(uintptr_t)mp->mp_membuf_addr;
    block_ptr = (struct os_memblock *)block_addr;
    blocks = mp->mp_num_blocks;

    while (blocks > 1) {
        block_addr += true_block_size;
        os_mempool_poison(mp, block_addr);
        os_mempool_guard(mp, block_addr);
        SLIST_NEXT(block_ptr, mb_next) = (struct os_memblock *)block_addr;
        block_ptr = (struct os_memblock *)block_addr;
        --blocks;
    }

    /* Last one in the list should be NULL */
    SLIST_NEXT(block_ptr, mb_next) = NULL;

    return OS_OK;
}

os_error_t
os_mempool_ext_clear(struct os_mempool_ext *mpe)
{
    os_error_t rc;

    mpe->mpe_put_cb = NULL;
    mpe->mpe_put_arg = NULL;
    mpe->mpe_get_cb = NULL;
    mpe->mpe_get_arg = NULL;

    rc = os_mempool_clear(&mpe->mpe_mp);
#if MYNEWT_VAL(MP_RUNTIME_ALLOC)
    mpe->mpe_mp.mp_flags &= ~OS_MEMPOOL_F_EXT;
#else
    mpe->mpe_mp.mp_flags = 0;
#endif
    return rc;
}

bool
os_mempool_is_sane(const struct os_mempool *mp)
{
    struct os_memblock *block;

#if MYNEWT_VAL(MP_RUNTIME_ALLOC)
    /* Runtime mode cannot verify sanity */
    if (mp->mp_flags & OS_MEMPOOL_F_RUNTIME) {
        assert(0);
    }
#endif

    /* Verify that each block in the free list belongs to the mempool. */
    SLIST_FOREACH(block, mp, mb_next) {
        if (!os_memblock_from(mp, block)) {
            return false;
        }
        os_mempool_poison_check(mp, block);
        os_mempool_guard_check(mp, block);
    }

    return true;
}

int MEMPOOL_IRAM_ATTR
os_memblock_from(const struct os_mempool *mp, const void *block_addr)
{
    uint32_t true_block_size;
    uintptr_t baddr32;
    uint32_t end;

#if MYNEWT_VAL(MP_RUNTIME_ALLOC)
    /* Runtime allocation mode doesn't support from */
    if (mp->mp_flags & OS_MEMPOOL_F_RUNTIME) {
        assert(0);
    }
#endif

    static_assert(sizeof block_addr == sizeof baddr32,
                  "Pointer to void must be 32-bits.");

    baddr32 = (uint32_t)(uintptr_t)block_addr;
    true_block_size = OS_MEMPOOL_TRUE_BLOCK_SIZE(mp);
    end = mp->mp_membuf_addr + (mp->mp_num_blocks * true_block_size);

    /* Check that the block is in the memory buffer range. */
    if ((baddr32 < mp->mp_membuf_addr) || (baddr32 >= end)) {
        return 0;
    }

    /* All freed blocks should be on true block size boundaries! */
    if (!(mp->mp_flags & OS_MEMPOOL_F_COMBINATION) &&
        ((baddr32 - mp->mp_membuf_addr) % true_block_size) != 0) {
        return 0;
    }

    return 1;
}

void * MEMPOOL_IRAM_ATTR
os_memblock_get(struct os_mempool *mp)
{
    os_sr_t sr;
    struct os_mempool_ext *mpe;
    struct os_memblock *block;

    /* Check to make sure they passed in a memory pool (or something) */
    block = NULL;

    if (mp && mp->mp_flags & OS_MEMPOOL_F_EXT) {
        mpe = (struct os_mempool_ext *)mp;
        if (mpe->mpe_get_cb != NULL) {
            block = mpe->mpe_get_cb(mpe, mpe->mpe_get_arg);
            return block;
        }
    }
#if MYNEWT_VAL(MP_RUNTIME_ALLOC)
    /* Runtime allocation mode */
    if (mp && mp->mp_flags & OS_MEMPOOL_F_RUNTIME) {
        bool need_alloc = false;
        void *allocated_block;
        uint32_t alloc_size;

        OS_ENTER_CRITICAL(sr);

        if (mp->mp_num_free) {
#if MYNEWT_VAL(MP_BLOCK_REUSED)
            if (mp->mp_flags & OS_MEMPOOL_F_REUSED) {
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

        OS_EXIT_CRITICAL(sr);

        /* Allocate outside critical section to avoid holding lock too long */
        if (need_alloc) {
            alloc_size = OS_MEMPOOL_TRUE_BLOCK_SIZE(mp);
            allocated_block = nimble_platform_mem_malloc(alloc_size);

            if (allocated_block) {
                /* Initialize poison and guard */
                os_mempool_poison(mp, allocated_block);
                os_mempool_guard(mp, allocated_block);
                /* Save mempool pointer for block */
                block = (struct os_memblock *)(allocated_block);
            } else {
                // Should not happen
                OS_ENTER_CRITICAL(sr);
                mp->mp_num_free++;
                if (mp->mp_flags & OS_MEMPOOL_F_REUSED) {
                    mp->mp_alloc_blocks--;
                }
                OS_EXIT_CRITICAL(sr);
            }
        } else if (block) {
            os_mempool_poison_check(mp, block);
            os_mempool_guard_check(mp, block);
        }

        return block;
    }
#endif

    if (mp) {
        OS_ENTER_CRITICAL(sr);
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
        OS_EXIT_CRITICAL(sr);

        if (block) {
            os_mempool_poison_check(mp, block);
            os_mempool_guard_check(mp, block);
        }
    }


    return (void *)block;
}

os_error_t MEMPOOL_IRAM_ATTR
os_memblock_put_from_cb(struct os_mempool *mp, void *block_addr)
{
    os_sr_t sr;
    struct os_memblock *block;

#if MYNEWT_VAL(MP_RUNTIME_ALLOC)
    if (mp->mp_flags & OS_MEMPOOL_F_RUNTIME) {
        bool need_free = true;
        os_mempool_guard_check(mp, block_addr);

        /* Runtime allocation mode - free directly */
        OS_ENTER_CRITICAL(sr);
#if MYNEWT_VAL(MP_BLOCK_REUSED)
        if (mp->mp_flags & OS_MEMPOOL_F_REUSED) {
            block = (struct os_memblock *)block_addr;
            SLIST_INSERT_HEAD(mp, block, mb_next);
            need_free = false;
        }
#endif
        mp->mp_num_free++;
        assert(mp->mp_num_blocks >= mp->mp_num_free);
        OS_EXIT_CRITICAL(sr);

        /* Free outside critical section */
        if (need_free) {
            nimble_platform_mem_free(block_addr);
        } else {
            os_mempool_poison(mp, block_addr);
        }
        return OS_OK;
    }
#endif
    /* Validate that the block belongs to this mempool */
    if (!(mp->mp_flags & OS_MEMPOOL_F_FRAG) &&
        !os_memblock_from(mp, block_addr)) {
        return OS_INVALID_PARM;
    }

    os_mempool_guard_check(mp, block_addr);
    os_mempool_poison(mp, block_addr);

    block = (struct os_memblock *)block_addr;
    OS_ENTER_CRITICAL(sr);

    /* Check for duplicate free - verify block is not already in free list */
    {
        struct os_memblock *cur;
        SLIST_FOREACH(cur, mp, mb_next) {
            if (cur == block) {
                OS_EXIT_CRITICAL(sr);
                return OS_INVALID_PARM;
            }
        }
    }

    /* Check that the number free doesn't exceed number blocks */
    if (mp->mp_num_free >= mp->mp_num_blocks) {
        OS_EXIT_CRITICAL(sr);
        return OS_INVALID_PARM;
    }

    /* Chain current free list pointer to this block; make this block head */
    SLIST_NEXT(block, mb_next) = SLIST_FIRST(mp);
    SLIST_FIRST(mp) = block;

    /* Increment number free */
    mp->mp_num_free++;

    OS_EXIT_CRITICAL(sr);


    return OS_OK;
}

os_error_t MEMPOOL_IRAM_ATTR
os_memblock_put(struct os_mempool *mp, void *block_addr)
{
    struct os_mempool_ext *mpe;
    os_error_t ret;
#if MYNEWT_VAL(OS_MEMPOOL_CHECK)
    struct os_memblock *block;
#endif

    /* Make sure parameters are valid */
    if ((mp == NULL) || (block_addr == NULL)) {
        ret = OS_INVALID_PARM;
        goto done;
    }

#if MYNEWT_VAL(OS_MEMPOOL_CHECK)
#if MYNEWT_VAL(MP_RUNTIME_ALLOC)
    if (!(mp->mp_flags & OS_MEMPOOL_F_RUNTIME))
#endif
    {
        /* Check that the block we are freeing is a valid block! */
        assert(os_memblock_from(mp, block_addr));
    }

    /*
     * Check for duplicate free.
     */
    SLIST_FOREACH(block, mp, mb_next) {
        assert(block != (struct os_memblock *)block_addr);
    }
#endif
    /* If this is an extended mempool with a put callback, call the callback
     * instead of freeing the block directly.
     */
    if (mp->mp_flags & OS_MEMPOOL_F_EXT) {
        mpe = (struct os_mempool_ext *)mp;
        if (mpe->mpe_put_cb != NULL) {
            ret = mpe->mpe_put_cb(mpe, block_addr, mpe->mpe_put_arg);
            goto done;
        }
    }

    /* No callback; free the block. */
    ret = os_memblock_put_from_cb(mp, block_addr);

done:
    return ret;
}

struct os_mempool *
os_mempool_info_get_next(struct os_mempool *mp, struct os_mempool_info *omi)
{
    struct os_mempool *cur;

#if MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)
    os_mempool_list_ensure_init();
#endif // MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)

    if (mp == NULL) {
        cur = STAILQ_FIRST(&g_os_mempool_list);
    } else {
        cur = STAILQ_NEXT(mp, mp_list);
    }

    if (cur == NULL) {
        return (NULL);
    }

    omi->omi_block_size = cur->mp_block_size;
    omi->omi_num_blocks = cur->mp_num_blocks;
    omi->omi_num_free = cur->mp_num_free;
    omi->omi_min_free = cur->mp_min_free;
    strncpy(omi->omi_name, cur->name, sizeof(omi->omi_name) - 1);
    omi->omi_name[sizeof(omi->omi_name) - 1] = '\0';

    return (cur);
}

void
os_mempool_flags_set(struct os_mempool *mp, uint8_t flags)
{
    mp->mp_flags |= flags;
}

void
os_mempool_flags_clear(struct os_mempool *mp, uint8_t flags)
{
    mp->mp_flags &= ~flags;
}

void
os_mempool_module_init(void)
{
#if MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)
    os_mempool_list_ensure_init();
#endif // MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)

}

#if MYNEWT_VAL(MP_RUNTIME_ALLOC)
static os_error_t
os_mempool_mem_free(struct os_mempool *mp)
{
    /* For runtime allocation mode, check whether all blocks have been freed */
    if (!(mp->mp_flags & OS_MEMPOOL_F_RUNTIME)) {
        return OS_EINVAL;
    }

    /* NOTE: if mempool gets cleared before all mem block is retrieved, it may leads to mem trampling */
    assert(mp->mp_num_free == mp->mp_num_blocks);
#if MYNEWT_VAL(MP_BLOCK_REUSED)
    struct os_memblock *block_ptr;

    /* For block reused mode, free all allocated blocks */
    if (mp->mp_flags & OS_MEMPOOL_F_REUSED) {
        uint16_t cnt;
        for (cnt = 0; cnt < mp->mp_alloc_blocks; cnt ++) {
            block_ptr = SLIST_FIRST(mp);
            if (block_ptr) {
                assert(block_ptr);
                SLIST_REMOVE_HEAD(mp, mb_next);
                nimble_platform_mem_free(block_ptr);
            }
        }
        mp->mp_alloc_blocks = 0;
    }
#endif
    /* Only reset statistics */
    SLIST_FIRST(mp) = NULL;
    mp->mp_min_free = mp->mp_num_blocks;
    return OS_OK;
}
#endif

void
os_mempool_deinit(bool is_controller)
{
    struct os_mempool *mp = NULL;
    struct os_mempool *next = NULL;

    mp = STAILQ_FIRST(&g_os_mempool_list);

    // All mempool blocks should be reclaimed and mempool removed from mempool list after nimble deinit
    while (mp) {
        if (is_controller == !!(mp->mp_flags & OS_MEMPOOL_F_CONTROLLER)) {
            next = STAILQ_NEXT(mp, mp_list);
            os_mempool_unregister(mp);
            mp = next;
        } else {
            mp = STAILQ_NEXT(mp, mp_list);
        }
    }
}
