/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
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
#include <stdio.h>
#include <assert.h>

#include "esp_rom_caps.h"
#include "esp_rom_tlsf.h"
#include "esp_rom_sys.h"

#include "tlsf_block_functions.h"
#include "tlsf_control_functions.h"

/*
 * IMPORTANT!
 *
 * Several versions of the TLSF in ROM exist and can be divided in
 * 3 categories:
 *   1- Versions where tlsf_check and tlsf_walk_pool do not call
 *      the poison check hook (oldest versions)
 * 	 2- Versions where tlsf_check does call the poison check hook,
 *      but tlsf_walk_pool still doesn't (intermediate versions)
 *   3- Versions where both tlsf_check and tlsf_walk_pool call the
 *      the poison check hook (latest versions)
 *
 * Category 3 does not require the patch defined in this file.
 *
 * Category 2 requires the patch but only for tlsf_walk_pool. In those
 * versions, a definition of tlsf_poison_check_pfunc_set is present in
 * the TLSF ROM which makes the definition in this file unnecessary since
 * the linker will keep the ROM definition. For this reason, tlsf_walk_pool_pfunc_set
 * is defined and called in multi_heap_poisoning.c to make sure that the poison check
 * hook also gets set in this file. The definition of tlsf_poison_check_pfunc_set could
 * be removed from this file if not for the needs of category 1.
 *
 * Category 1 versions of the ROM TLSF do not provide a definition of
 * tlsf_poison_check_pfunc_set, making the definition below necessary for
 * any application to compile and link properly. The added patch of tlsf_walk_pool
 * makes the patch of tlsf_check unnecessary in this file since tlsf_check was only
 * calling the poison check hook for free blocks while tlsf_walk_pool calls the poison
 * check hook for all blocks (free and used). For this reason, tlsf_poison_check_pfunc_set
 * is left empty.
 * */

static poison_check_pfunc_t s_walk_pool_check_region = NULL;

void tlsf_poison_check_pfunc_set(poison_check_pfunc_t pfunc)
{
}

void tlsf_walk_pool_pfunc_set(poison_check_pfunc_t pfunc)
{
    s_walk_pool_check_region = pfunc;
}

#define tlsf_insist_no_assert(x) { if (!(x)) { status--; } }

typedef struct integrity_t
{
	int prev_status;
	int status;
} integrity_t;

static bool integrity_walker(void* ptr, size_t size, int used, void* user)
{
	block_header_t* block = block_from_ptr(ptr);
	integrity_t* integ = tlsf_cast(integrity_t*, user);
	const int this_prev_status = block_is_prev_free(block) ? 1 : 0;
	const int this_status = block_is_free(block) ? 1 : 0;
	const size_t this_block_size = block_size(block);

	int status = 0;
	tlsf_insist_no_assert(integ->prev_status == this_prev_status && "prev status incorrect");
	tlsf_insist_no_assert(size == this_block_size && "block size incorrect");

	if (s_walk_pool_check_region != NULL)
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

		tlsf_insist_no_assert(s_walk_pool_check_region(ptr_block, actual_free_block_size, !used, true));
	}

	integ->prev_status = this_status;
	integ->status += status;

    return true;
}

static bool default_walker(void* ptr, size_t size, int used, void* user)
{
	(void)user;
	printf("\t%p %s size: %x (%p)\n", ptr, used ? "used" : "free", (unsigned int)size, block_from_ptr(ptr));
	return true;
}

static void tlsf_walk_pool_impl(pool_t pool, tlsf_walker walker, void* user)
{
	tlsf_walker pool_walker = walker ? walker : default_walker;
	block_header_t* block =
		offset_to_block(pool, -(int)block_header_overhead);

	bool ret_val = true;
	while (block && !block_is_last(block) && ret_val == true)
	{
		ret_val = pool_walker(
			block_to_ptr(block),
			block_size(block),
			!block_is_free(block),
			user);

		if (ret_val == true) {
			block = block_next(block);
		}
	}
}

static int tlsf_check_pool_impl(pool_t pool)
{
	/* Check that the blocks are physically correct. */
	integrity_t integ = { 0, 0 };
	tlsf_walk_pool_impl(pool, integrity_walker, &integ);

	return integ.status;
}

#undef tlsf_insist_no_assert

/* Set up the TLSF ROM patches here */

/*
 * struct heap_tlsf_stub_table_t is defined in the per-target header.
 * Its layout exactly mirrors the TLSF vtable in ROM for the current target.
 * The header is produced by gen_esp_rom_heap_tlsf_stub_table.py from the
 * target's .rom.heap.ld file; run that script whenever the .ld file changes.
 */
#include "esp_rom_heap_tlsf_stub_table.h"

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
    heap_tlsf_patch_table_ptr.tlsf_check_pool = tlsf_check_pool_impl;
    heap_tlsf_patch_table_ptr.tlsf_walk_pool = tlsf_walk_pool_impl;

    /* Set our table as the one to use in the ROM code */
    heap_tlsf_table_ptr = &heap_tlsf_patch_table_ptr;
}

/*
 * @brief Helper function to call the patched tlsf_walk_pool.
 * This is needed because the ROM's tlsf_walk_pool symbol cannot be overridden
 * on chips where it's a strong symbol (not using PROVIDE in linker script).
 * Other patch files can call this to access the patched version.
 */
void tlsf_walk_pool_patched(pool_t pool, tlsf_walker walker, void* user)
{
    heap_tlsf_table_ptr->tlsf_walk_pool(pool, walker, user);
}
