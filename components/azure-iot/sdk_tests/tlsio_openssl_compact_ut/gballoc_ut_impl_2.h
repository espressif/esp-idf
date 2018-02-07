// Copyright(c) Microsoft.All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is made an integral part of a unit test implementation with a #include. It
// is broken out for reuse and readability. 

#ifndef GBALLOC_UT_IMPL_2_H
#define GBALLOC_UT_IMPL_2_H

#include <stdbool.h>

// This adds memory checking to the gballoc mocking
// NOTE: Using ASSERTs wouuld be nice, but this file must preceed their definition
// to avoid the malloc infinite recursion problem


#ifndef GBALLOC_UT_IMPL_MAX_ALLOCS
#define GBALLOC_UT_IMPL_MAX_ALLOCS 1000
#endif

static void* memory_blocks[GBALLOC_UT_IMPL_MAX_ALLOCS];
static uint32_t memory_block_count;


static void init_gballoc_checks()
{
    uint32_t i;
	memory_block_count = 0;
	for (i = 0; i < GBALLOC_UT_IMPL_MAX_ALLOCS; i++)
	{
		memory_blocks[i] = NULL;
	}
}

static void add_gballoc_memory_block(void* block)
{
	if (block != NULL)
	{
		if (memory_block_count < GBALLOC_UT_IMPL_MAX_ALLOCS)
		{
			memory_blocks[memory_block_count] = block;
			memory_block_count++;
		}
		else
		{
			ASSERT_FAIL("GBALLOC_UT_IMPL_MAX_ALLOCS is too small in add_gballoc_memory_block");
		}
	}
}

static void remove_gballoc_memory_block(void* block)
{
	if (block != NULL)
	{
		bool found = false;
        uint32_t i;
		for (i = 0; i < memory_block_count; i++)
		{
			if (memory_blocks[i] == block)
			{
				memory_blocks[i] = NULL;
				found = true;
				break;
			}
		}
		if (!found)
		{
			ASSERT_FAIL("unknown block in remove_gballoc_memory_block");
		}
	}
}

static void assert_gballoc_checks()
{
    uint32_t i;
	for (i = 0; i < memory_block_count; i++)
	{
		if (memory_blocks[i] != NULL)
		{
			ASSERT_FAIL("undeleted memory block");
		}
	}
}

#endif // GBALLOC_UT_IMPL_2_H
