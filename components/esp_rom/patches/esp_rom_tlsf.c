/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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

#define block_header_free_bit  (1 << 0)
#define block_header_prev_free_bit  (1 << 1)
#define block_header_overhead  (sizeof(size_t))
#define block_start_offset (offsetof(block_header_t, size) + sizeof(size_t))

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

static inline __attribute__((always_inline)) block_header_t* block_from_ptr(const void* ptr)
{
	return tlsf_cast(block_header_t*,
		tlsf_cast(unsigned char*, ptr) - block_start_offset);
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

typedef struct integrity_t
{
	int prev_status;
	int status;
} integrity_t;

static void integrity_walker(void* ptr, size_t size, int used, void* user)
{
	block_header_t* block = block_from_ptr(ptr);
	integrity_t* integ = tlsf_cast(integrity_t*, user);
	const int this_prev_status = block_is_prev_free(block) ? 1 : 0;
	const int this_status = block_is_free(block) ? 1 : 0;
	const size_t this_block_size = block_size(block);

	int status = 0;
	tlsf_insist_no_assert(integ->prev_status == this_prev_status && "prev status incorrect");
	tlsf_insist_no_assert(size == this_block_size && "block size incorrect");

	if (s_poison_check_region != NULL)
	{
		/* block_size(block) returns the size of the usable memory when the block is allocated.
		 * As the block under test is free, we need to subtract to the block size the next_free
		 * and prev_free fields of the block header as they are not a part of the usable memory
		 * when the block is free. In addition, we also need to subtract the size of prev_phys_block
		 * as this field is in fact part of the current free block and not part of the next (allocated)
		 * block. Check the comments in block_split function for more details.
		 */
		const size_t actual_free_block_size = used ? this_block_size :
													 this_block_size - offsetof(block_header_t, next_free)- block_header_overhead;

		void* ptr_block = used ? (void*)block + block_start_offset :
								 (void*)block + sizeof(block_header_t);

		tlsf_insist_no_assert(s_poison_check_region(ptr_block, actual_free_block_size, !used, true));
	}

	integ->prev_status = this_status;
	integ->status += status;
}

extern void tlsf_walk_pool(pool_t pool, tlsf_walker walker, void* user);
int tlsf_check_pool(pool_t pool)
{
	/* Check that the blocks are physically correct. */
	integrity_t integ = { 0, 0 };
	tlsf_walk_pool(pool, integrity_walker, &integ);

	return integ.status;
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
    heap_tlsf_patch_table_ptr.tlsf_check_pool = tlsf_check_pool;

    /* Set our table as the one to use in the ROM code */
    heap_tlsf_table_ptr = &heap_tlsf_patch_table_ptr;
}
