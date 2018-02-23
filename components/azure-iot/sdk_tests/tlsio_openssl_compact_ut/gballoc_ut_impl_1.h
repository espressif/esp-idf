// Copyright(c) Microsoft.All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is made an integral part of a unit test implementation with a #include. It
// is broken out for reuse and readability. 

#ifndef GBALLOC_UT_IMPL_1_H
#define GBALLOC_UT_IMPL_1_H

// These functions add memory leak checking to the unit test itself rather than
// relying on Valgrind, which is inconvenient for troubleshooting.
static void add_gballoc_memory_block(void* block);
static void remove_gballoc_memory_block(void* block);

/**
* The gballoc.h will replace the malloc, free, and realloc by the my_gballoc functions, in this case,
*    if you define these mock functions after include the gballoc.h, you will create an infinity recursion,
*    so, places the my_gballoc functions before the #include "azure_c_shared_utility/gballoc.h"
*/
static void* my_gballoc_malloc(size_t size)
{
	void* result = malloc(size);
	add_gballoc_memory_block(result);
	return result;
}

static void* my_gballoc_realloc(void* ptr, size_t size)
{
    void* result;
	remove_gballoc_memory_block(ptr);
	result = realloc(ptr, size);
	add_gballoc_memory_block(result);
	return result;
}

static void my_gballoc_free(void* ptr)
{
	remove_gballoc_memory_block(ptr);
	free(ptr);
}

#endif // GBALLOC_UT_IMPL_1_H


