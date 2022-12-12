/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This file is a patch for the tlsf implementation stored in ROM
 * - tlsf_check() now implements a call to a hook giving the user the possibility
 * to implement specific checks on the memory of every free blocks.
 * - The function tlsf_poison_check_pfunc_set() was added to allow the user to
 * register the hook function called in tlsf_check().
 */

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "esp_rom_caps.h"
#include "esp_rom_tlsf.h"

/*!
 * @brief Opaque types for TLSF implementation
 */
typedef void* tlsf_t;
typedef void* pool_t;
typedef void* tlsf_walker;

/* ----------------------------------------------------------------
 * Bring certain inline functions, macro and structures from the
 * tlsf ROM implementation to be able to compile the patch.
 * ---------------------------------------------------------------- */

#define tlsf_cast(t, exp)	((t) (exp))

enum tlsf_config {
    /* log2 of number of linear subdivisions of block sizes. Larger
    ** values require more memory in the control structure. Values of
    ** 4 or 5 are typical.
    */
    SL_INDEX_COUNT_LOG2 = 5,

    /* All allocation sizes and addresses are aligned to 4 bytes. */
    ALIGN_SIZE_LOG2 = 2,
    ALIGN_SIZE = (1 << ALIGN_SIZE_LOG2),

/*
    ** We support allocations of sizes up to (1 << FL_INDEX_MAX) bits.
    ** However, because we linearly subdivide the second-level lists, and
    ** our minimum size granularity is 4 bytes, it doesn't make sense to
    ** create first-level lists for sizes smaller than SL_INDEX_COUNT * 4,
    ** or (1 << (SL_INDEX_COUNT_LOG2 + 2)) bytes, as there we will be
    ** trying to split size ranges into more slots than we have available.
    ** Instead, we calculate the minimum threshold size, and place all
    ** blocks below that size into the 0th first-level list.
    */

    /* Fix the value of FL_INDEX_MAX to match the value that is defined
     * in the ROM implementation. */
    FL_INDEX_MAX = 18, //Each pool can have up 256KB

    SL_INDEX_COUNT = (1 << SL_INDEX_COUNT_LOG2),
    FL_INDEX_SHIFT = (SL_INDEX_COUNT_LOG2 + ALIGN_SIZE_LOG2),
    FL_INDEX_COUNT = (FL_INDEX_MAX - FL_INDEX_SHIFT + 1),

    SMALL_BLOCK_SIZE = (1 << FL_INDEX_SHIFT),
};

#define block_header_free_bit  (1 << 0)
#define block_header_prev_free_bit  (1 << 1)
#define block_header_overhead  (sizeof(size_t))
#define block_start_offset (offsetof(block_header_t, size) + sizeof(size_t))
#define block_size_min  (sizeof(block_header_t) - sizeof(block_header_t*))

typedef ptrdiff_t tlsfptr_t;

typedef struct block_header_t
{
    /* Points to the previous physical block. */
    struct block_header_t* prev_phys_block;

    /* The size of this block, excluding the block header. */
    size_t size;

    /* Next and previous free blocks. */
    struct block_header_t* next_free;
    struct block_header_t* prev_free;
} block_header_t;

/* The TLSF control structure. */
typedef struct control_t
{
    /* Empty lists point at this block to indicate they are free. */
    block_header_t block_null;

    /* Bitmaps for free lists. */
    unsigned int fl_bitmap;
    unsigned int sl_bitmap[FL_INDEX_COUNT];

    /* Head of free lists. */
    block_header_t* blocks[FL_INDEX_COUNT][SL_INDEX_COUNT];
} control_t;

static inline __attribute__((__always_inline__)) int tlsf_fls(unsigned int word)
{
    const int bit = word ? 32 - __builtin_clz(word) : 0;
    return bit - 1;
}

static inline __attribute__((__always_inline__)) size_t block_size(const block_header_t* block)
{
    return block->size & ~(block_header_free_bit | block_header_prev_free_bit);
}

static inline __attribute__((__always_inline__)) int block_is_free(const block_header_t* block)
{
    return tlsf_cast(int, block->size & block_header_free_bit);
}

static inline __attribute__((__always_inline__)) int block_is_prev_free(const block_header_t* block)
{
    return tlsf_cast(int, block->size & block_header_prev_free_bit);
}

static inline __attribute__((__always_inline__)) block_header_t* offset_to_block(const void* ptr, size_t size)
{
    return tlsf_cast(block_header_t*, tlsf_cast(tlsfptr_t, ptr) + size);
}

static inline __attribute__((__always_inline__)) void* block_to_ptr(const block_header_t* block)
{
    return tlsf_cast(void*,
        tlsf_cast(unsigned char*, block) + block_start_offset);
}

static inline __attribute__((__always_inline__)) block_header_t* block_next(const block_header_t* block)
{
    block_header_t* next = offset_to_block(block_to_ptr(block),
        block_size(block) - block_header_overhead);
    return next;
}

static inline __attribute__((__always_inline__)) void mapping_insert(size_t size, int* fli, int* sli)
{
    int fl, sl;
    if (size < SMALL_BLOCK_SIZE)
    {
        /* Store small blocks in first list. */
        fl = 0;
        sl = tlsf_cast(int, size) >> 2;
    }
    else
    {
        fl = tlsf_fls(size);
        sl = tlsf_cast(int, size >> (fl - SL_INDEX_COUNT_LOG2)) ^ (1 << SL_INDEX_COUNT_LOG2);
        fl -= (FL_INDEX_SHIFT - 1);
    }
    *fli = fl;
    *sli = sl;
}

/* ----------------------------------------------------------------
 * End of the environment necessary to compile and link the patch
 * defined below
 * ---------------------------------------------------------------- */

static poison_check_pfunc_t s_poison_check_region = NULL;

void tlsf_poison_check_pfunc_set(poison_check_pfunc_t pfunc)
{
    s_poison_check_region = pfunc;
}

#define tlsf_insist_no_assert(x) { if (!(x)) { status--; } }

int tlsf_check(tlsf_t tlsf)
{
    int i, j;

    control_t* control = tlsf_cast(control_t*, tlsf);
    int status = 0;

    /* Check that the free lists and bitmaps are accurate. */
    for (i = 0; i < FL_INDEX_COUNT; ++i)
    {
        for (j = 0; j < SL_INDEX_COUNT; ++j)
        {
            const int fl_map = control->fl_bitmap & (1 << i);
            const int sl_list = control->sl_bitmap[i];
            const int sl_map = sl_list & (1 << j);
            const block_header_t* block = control->blocks[i][j];

            /* Check that first- and second-level lists agree. */
            if (!fl_map)
            {
                tlsf_insist_no_assert(!sl_map && "second-level map must be null");
            }

            if (!sl_map)
            {
                tlsf_insist_no_assert(block == &control->block_null && "block list must be null");
                continue;
            }

            /* Check that there is at least one free block. */
            tlsf_insist_no_assert(sl_list && "no free blocks in second-level map");
            tlsf_insist_no_assert(block != &control->block_null && "block should not be null");

            while (block != &control->block_null)
            {
                int fli, sli;
                const bool is_block_free = block_is_free(block);
                tlsf_insist_no_assert(is_block_free && "block should be free");
                tlsf_insist_no_assert(!block_is_prev_free(block) && "blocks should have coalesced");
                tlsf_insist_no_assert(!block_is_free(block_next(block)) && "blocks should have coalesced");
                tlsf_insist_no_assert(block_is_prev_free(block_next(block)) && "block should be free");
                tlsf_insist_no_assert(block_size(block) >= block_size_min && "block not minimum size");

                mapping_insert(block_size(block), &fli, &sli);
                tlsf_insist_no_assert(fli == i && sli == j && "block size indexed in wrong list");

                /* block_size(block) returns the size of the usable memory when the block is allocated.
                 * As the block under test is free, we need to subtract to the block size the next_free
                 * and prev_free fields of the block header as they are not a part of the usable memory
                 * when the block is free. In addition, we also need to subtract the size of prev_phys_block
                 * as this field is in fact part of the current free block and not part of the next (allocated)
                 * block. Check the comments in block_split function for more details.
                 */
                const size_t actual_free_block_size = block_size(block)
                                                        - offsetof(block_header_t, next_free)
                                                        - block_header_overhead;

                if (s_poison_check_region != NULL) {
                    tlsf_insist_no_assert(s_poison_check_region((char *)block + sizeof(block_header_t),
                                          actual_free_block_size, is_block_free, true /* print errors */));
                }

                block = block->next_free;
            }
        }
    }

    return status;
}

#undef tlsf_insist_no_assert

/* Set up the TLSF ROM patches here */

/*!
 * @brief Structure to store all the functions of a TLSF implementation.
 * The goal of this table is to change any of the address here in order
 * to let the ROM code call another function implementation than the one
 * in ROM.
 */
struct heap_tlsf_stub_table_t {
    tlsf_t (*tlsf_create)(void* mem);
    tlsf_t (*tlsf_create_with_pool)(void* mem, size_t bytes);
    pool_t (*tlsf_get_pool)(tlsf_t tlsf);
    pool_t (*tlsf_add_pool)(tlsf_t tlsf, void* mem, size_t bytes);
    void (*tlsf_remove_pool)(tlsf_t tlsf, pool_t pool);

    void* (*tlsf_malloc)(tlsf_t tlsf, size_t size);
    void* (*tlsf_memalign)(tlsf_t tlsf, size_t align, size_t size);
    void* (*tlsf_memalign_offs)(tlsf_t tlsf, size_t align, size_t size, size_t offset);
    void* (*tlsf_realloc)(tlsf_t tlsf, void* ptr, size_t size);
    void (*tlsf_free)(tlsf_t tlsf, void* ptr);

    size_t (*tlsf_block_size)(void* ptr);
    size_t (*tlsf_size)(void);
    size_t (*tlsf_align_size)(void);
    size_t (*tlsf_block_size_min)(void);
    size_t (*tlsf_block_size_max)(void);
    size_t (*tlsf_pool_overhead)(void);
    size_t (*tlsf_alloc_overhead)(void);

    void (*tlsf_walk_pool)(pool_t pool, tlsf_walker walker, void* user);

    int (*tlsf_check)(tlsf_t tlsf);
    int (*tlsf_check_pool)(pool_t pool);
};

/* We need the original table from the ROM */
extern struct heap_tlsf_stub_table_t* heap_tlsf_table_ptr;

/* We will copy the ROM table and modify the functions we patch */
struct heap_tlsf_stub_table_t heap_tlsf_patch_table_ptr;

/*!
 * @brief Setup the TLSF ROM patches.
 * This function must be called when setting up the heap. It will put in place the function patched
 * from the ROM implementation.
 * This function must not be defined as static, as it is marked as "undefined" in the linker flags
 * (to force the linker to integrate the functions of this file inside the final binary)
 */
void __attribute__((constructor)) tlsf_set_rom_patches(void)
{
    /* Copy the ROM default table inside our table */
    memcpy(&heap_tlsf_patch_table_ptr, heap_tlsf_table_ptr, sizeof(struct heap_tlsf_stub_table_t));

    /* Set the patched function here */
    heap_tlsf_patch_table_ptr.tlsf_check = tlsf_check;

    /* Set our table as the one to use in the ROM code */
    heap_tlsf_table_ptr = &heap_tlsf_patch_table_ptr;
}
