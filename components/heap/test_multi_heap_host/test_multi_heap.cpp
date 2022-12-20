#include "catch.hpp"
#include "multi_heap.h"

#include "../multi_heap_config.h"
#include "../tlsf/tlsf.h"
#include "../tlsf/tlsf_common.h"
#include "../tlsf/tlsf_block_functions.h"

#include <string.h>
#include <assert.h>

/* The functions __malloc__ and __free__ are used to call the libc
 * malloc and free and allocate memory from the host heap. Since the test
 * `TEST_CASE("multi_heap many random allocations", "[multi_heap]")`
 * calls multi_heap_allocation_impl() with sizes that can go up to 8MB,
 * an allocatation on the heap will be prefered rather than the stack which
 * might not have the necessary memory.
 */
static void *__malloc__(size_t bytes)
{
    return malloc(bytes);
}

static void __free__(void *ptr)
{
    free(ptr);
}

/* Insurance against accidentally using libc heap functions in tests */
#undef free
#define free #error
#undef malloc
#define malloc #error
#undef calloc
#define calloc #error
#undef realloc
#define realloc #error

TEST_CASE("multi_heap simple allocations", "[multi_heap]")
{
    uint8_t small_heap[4 * 1024];

    multi_heap_handle_t heap = multi_heap_register(small_heap, sizeof(small_heap));

    size_t test_alloc_size = (multi_heap_free_size(heap) + 4) / 2;

    printf("New heap:\n");
    multi_heap_dump(heap);
    printf("*********************\n");

    uint8_t *buf = (uint8_t *)multi_heap_malloc(heap, test_alloc_size);

    printf("small_heap %p buf %p\n", small_heap, buf);
    REQUIRE( buf != NULL );
    REQUIRE((intptr_t)buf >= (intptr_t)small_heap);
    REQUIRE( (intptr_t)buf < (intptr_t)(small_heap + sizeof(small_heap)));

    REQUIRE( multi_heap_get_allocated_size(heap, buf) >= test_alloc_size );
    REQUIRE( multi_heap_get_allocated_size(heap, buf) < test_alloc_size + 16);

    memset(buf, 0xEE, test_alloc_size);

    REQUIRE( multi_heap_malloc(heap, test_alloc_size) == NULL );

    multi_heap_free(heap, buf);

    printf("Empty?\n");
    multi_heap_dump(heap);
    printf("*********************\n");

    /* Now there should be space for another allocation */
    buf = (uint8_t *)multi_heap_malloc(heap, test_alloc_size);
    REQUIRE( buf != NULL );
    multi_heap_free(heap, buf);

    REQUIRE( multi_heap_free_size(heap) > multi_heap_minimum_free_size(heap) );
}


TEST_CASE("multi_heap fragmentation", "[multi_heap]")
{
    const size_t HEAP_SIZE = 4 * 1024;
    uint8_t small_heap[HEAP_SIZE];
    multi_heap_handle_t heap = multi_heap_register(small_heap, sizeof(small_heap));

    const size_t alloc_size = 500;

    void *p[4];
    for (int i = 0; i < 4; i++) {
        multi_heap_dump(heap);
        REQUIRE(  multi_heap_check(heap, true) );
        p[i] = multi_heap_malloc(heap, alloc_size);
        printf("%d = %p ****->\n", i, p[i]);
        multi_heap_dump(heap);
        REQUIRE( p[i] != NULL );
    }

    printf("allocated %p %p %p %p\n", p[0], p[1], p[2], p[3]);

    REQUIRE( multi_heap_malloc(heap, alloc_size * 5) == NULL ); /* no room to allocate 5*alloc_size now */

    printf("4 allocations:\n");
    multi_heap_dump(heap);
    printf("****************\n");

    multi_heap_free(heap, p[0]);
    multi_heap_free(heap, p[1]);
    multi_heap_free(heap, p[3]);

    printf("1 allocations:\n");
    multi_heap_dump(heap);
    printf("****************\n");

    void *big = multi_heap_malloc(heap, alloc_size * 3);
    //Blocks in TLSF are organized in different form, so this makes no sense
    multi_heap_free(heap, big);

    multi_heap_free(heap, p[2]);

    printf("0 allocations:\n");
    multi_heap_dump(heap);
    printf("****************\n");

    big = multi_heap_malloc(heap, alloc_size * 2);
    //Blocks in TLSF are organized in different form, so this makes no sense
    multi_heap_free(heap, big);
}

/* Test that malloc/free does not leave free space fragmented */
TEST_CASE("multi_heap defrag", "[multi_heap]")
{
    void *p[4];
    uint8_t small_heap[4 * 1024];
    multi_heap_info_t info, info2;
    multi_heap_handle_t heap = multi_heap_register(small_heap, sizeof(small_heap));

    printf("0 ---\n");
    multi_heap_dump(heap);
    REQUIRE( multi_heap_check(heap, true) );
    multi_heap_get_info(heap, &info);
    REQUIRE( 0 == info.allocated_blocks );
    REQUIRE( 1 == info.free_blocks );

    printf("1 ---\n");
    p[0] = multi_heap_malloc(heap, 128);
    p[1] = multi_heap_malloc(heap, 32);
    multi_heap_dump(heap);
    REQUIRE( multi_heap_check(heap, true) );

    printf("2 ---\n");
    multi_heap_free(heap, p[0]);
    p[2] = multi_heap_malloc(heap, 64);
    multi_heap_dump(heap);
    REQUIRE( p[2] == p[0] );
    REQUIRE( multi_heap_check(heap, true) );

    printf("3 ---\n");
    multi_heap_free(heap, p[2]);
    p[3] = multi_heap_malloc(heap, 32);
    multi_heap_dump(heap);
    REQUIRE( p[3] == p[0] );
    REQUIRE( multi_heap_check(heap, true) );

    multi_heap_get_info(heap, &info2);
    REQUIRE( 2 == info2.allocated_blocks );
    REQUIRE( 2 == info2.free_blocks );

    multi_heap_free(heap, p[0]);
    multi_heap_free(heap, p[1]);
    multi_heap_get_info(heap, &info2);
    REQUIRE( 0 == info2.allocated_blocks );
    REQUIRE( 1 == info2.free_blocks );
    REQUIRE( info.total_free_bytes == info2.total_free_bytes );
}

/* Test that malloc/free does not leave free space fragmented
   Note: With fancy poisoning, realloc is implemented as malloc-copy-free and this test does not apply.
 */
#ifndef MULTI_HEAP_POISONING_SLOW
TEST_CASE("multi_heap defrag realloc", "[multi_heap]")
{
    void *p[4];
    uint8_t small_heap[4 * 1024];
    multi_heap_info_t info, info2;
    multi_heap_handle_t heap = multi_heap_register(small_heap, sizeof(small_heap));

    printf("0 ---\n");
    multi_heap_dump(heap);
    REQUIRE( multi_heap_check(heap, true) );
    multi_heap_get_info(heap, &info);
    REQUIRE( 0 == info.allocated_blocks );
    REQUIRE( 1 == info.free_blocks );

    printf("1 ---\n");
    p[0] = multi_heap_malloc(heap, 128);
    p[1] = multi_heap_malloc(heap, 32);
    multi_heap_dump(heap);
    REQUIRE( multi_heap_check(heap, true) );

    printf("2 ---\n");
    p[2] = multi_heap_realloc(heap, p[0], 64);
    multi_heap_dump(heap);
    REQUIRE( p[2] == p[0] );
    REQUIRE( multi_heap_check(heap, true) );

    printf("3 ---\n");
    p[3] = multi_heap_realloc(heap, p[2], 32);
    multi_heap_dump(heap);
    REQUIRE( p[3] == p[0] );
    REQUIRE( multi_heap_check(heap, true) );

    multi_heap_get_info(heap, &info2);
    REQUIRE( 2 == info2.allocated_blocks );
    REQUIRE( 2 == info2.free_blocks );

    multi_heap_free(heap, p[0]);
    multi_heap_free(heap, p[1]);
    multi_heap_get_info(heap, &info2);
    REQUIRE( 0 == info2.allocated_blocks );
    REQUIRE( 1 == info2.free_blocks );
    REQUIRE( info.total_free_bytes == info2.total_free_bytes );
}
#endif


void multi_heap_allocation_impl(int heap_size)
{
    uint8_t *big_heap = (uint8_t *) __malloc__(heap_size);
    const int NUM_POINTERS = 64;

    printf("Running multi-allocation test with heap_size %d...\n", heap_size);

    REQUIRE( big_heap );
    multi_heap_handle_t heap = multi_heap_register(big_heap, heap_size);

    void *p[NUM_POINTERS] = { 0 };
    size_t s[NUM_POINTERS] = { 0 };

    const size_t initial_free = multi_heap_free_size(heap);

    const int ITERATIONS = 5000;

    for (int i = 0; i < ITERATIONS; i++) {
        /* check all pointers allocated so far are valid inside big_heap */
        for (int j = 0; j < NUM_POINTERS; j++) {
            if (p[j] != NULL) {
            }
        }

        uint8_t n = rand() % NUM_POINTERS;

        if (i % 4 == 0) {
            /* 1 in 4 iterations, try to realloc the buffer instead
               of using malloc/free
            */
            size_t new_size = (rand() % 1023) + 1;
            void *new_p = multi_heap_realloc(heap, p[n], new_size);
            printf("realloc %p -> %p (%zu -> %zu)\n", p[n], new_p, s[n], new_size);
            multi_heap_check(heap, true);
            if (new_size == 0 || new_p != NULL) {
                p[n] = new_p;
                s[n] = new_size;
                if (new_size > 0) {
                    REQUIRE( p[n] >= big_heap );
                    REQUIRE( p[n] < big_heap + heap_size );
                    memset(p[n], n, new_size);
                }
            }
            continue;
        }
        if (p[n] != NULL) {
            if (s[n] > 0) {
                /* Verify pre-existing contents of p[n] */
                uint8_t compare[s[n]];
                memset(compare, n, s[n]);
                /*REQUIRE*/assert( memcmp(compare, p[n], s[n]) == 0 );
            }
            REQUIRE( multi_heap_check(heap, true) );
            multi_heap_free(heap, p[n]);
            printf("freed %p (%zu)\n", p[n], s[n]);
            if (!multi_heap_check(heap, true)) {
                printf("FAILED iteration %d after freeing %p\n", i, p[n]);
                multi_heap_dump(heap);
                REQUIRE(0);
            }
        }

        s[n] = rand() % 1024;
        REQUIRE( multi_heap_check(heap, true) );
        p[n] = multi_heap_malloc(heap, s[n]);
        printf("malloc %p (%zu)\n", p[n], s[n]);
        if (p[n] != NULL) {
            REQUIRE( p[n] >= big_heap );
            REQUIRE( p[n] < big_heap + heap_size );
        }
        if (!multi_heap_check(heap, true)) {
            printf("FAILED iteration %d after mallocing %p (%zu bytes)\n", i, p[n], s[n]);
            multi_heap_dump(heap);
            REQUIRE(0);
        }
        if (p[n] != NULL) {
            memset(p[n], n, s[n]);
        }
    }

    for (int i = 0; i < NUM_POINTERS; i++) {
        multi_heap_free(heap, p[i]);
        if (!multi_heap_check(heap, true)) {
            printf("FAILED during cleanup after freeing %p\n", p[i]);
            multi_heap_dump(heap);
            REQUIRE(0);
        }
    }

    REQUIRE( initial_free == multi_heap_free_size(heap) );
    __free__(big_heap);
}

TEST_CASE("multi_heap many random allocations", "[multi_heap]")
{
    size_t poolsize[] = { 15, 255, 4095, 8191 };
    for (size_t i = 0; i < sizeof(poolsize)/sizeof(size_t); i++) {
        multi_heap_allocation_impl(poolsize[i] * 1024);
    }
}

TEST_CASE("multi_heap_get_info() function", "[multi_heap]")
{
    uint8_t heapdata[4 * 1024];
    multi_heap_handle_t heap = multi_heap_register(heapdata, sizeof(heapdata));
    multi_heap_info_t before, after, freed;

    multi_heap_get_info(heap, &before);
    printf("before: total_free_bytes %zu\ntotal_allocated_bytes %zu\nlargest_free_block %zu\nminimum_free_bytes %zu\nallocated_blocks %zu\nfree_blocks %zu\ntotal_blocks %zu\n",
           before.total_free_bytes,
           before.total_allocated_bytes,
           before.largest_free_block,
           before.minimum_free_bytes,
           before.allocated_blocks,
           before.free_blocks,
           before.total_blocks);

    REQUIRE( 0 == before.allocated_blocks );
    REQUIRE( 0 == before.total_allocated_bytes );
    REQUIRE( before.total_free_bytes == before.minimum_free_bytes );

    void *x = multi_heap_malloc(heap, 32);
    multi_heap_get_info(heap, &after);
    printf("after: total_free_bytes %zu\ntotal_allocated_bytes %zu\nlargest_free_block %zu\nminimum_free_bytes %zu\nallocated_blocks %zu\nfree_blocks %zu\ntotal_blocks %zu\n",
           after.total_free_bytes,
           after.total_allocated_bytes,
           after.largest_free_block,
           after.minimum_free_bytes,
           after.allocated_blocks,
           after.free_blocks,
           after.total_blocks);

    REQUIRE( 1 == after.allocated_blocks );
    REQUIRE( 32 == after.total_allocated_bytes );
    REQUIRE( after.minimum_free_bytes < before.minimum_free_bytes);
    REQUIRE( after.minimum_free_bytes > 0 );

    multi_heap_free(heap, x);
    multi_heap_get_info(heap, &freed);
    printf("freed: total_free_bytes %zu\ntotal_allocated_bytes %zu\nlargest_free_block %zu\nminimum_free_bytes %zu\nallocated_blocks %zu\nfree_blocks %zu\ntotal_blocks %zu\n",
           freed.total_free_bytes,
           freed.total_allocated_bytes,
           freed.largest_free_block,
           freed.minimum_free_bytes,
           freed.allocated_blocks,
           freed.free_blocks,
           freed.total_blocks);

    REQUIRE( 0 == freed.allocated_blocks );
    REQUIRE( 0 == freed.total_allocated_bytes );
    REQUIRE( before.total_free_bytes == freed.total_free_bytes );
    REQUIRE( after.minimum_free_bytes == freed.minimum_free_bytes );
}

TEST_CASE("multi_heap minimum-size allocations", "[multi_heap]")
{
    uint8_t heapdata[4096];
    void *p[sizeof(heapdata) / sizeof(void *)] = {NULL};
    const size_t NUM_P = sizeof(p) / sizeof(void *);
    size_t allocated_size = 0;
    multi_heap_handle_t heap = multi_heap_register(heapdata, sizeof(heapdata));
    size_t before_free = multi_heap_free_size(heap);

    size_t i;
    for (i = 0; i < NUM_P; i++) {
        //TLSF minimum block size is 4 bytes
        p[i] = multi_heap_malloc(heap, 1);
        if (p[i] == NULL) {
            break;
        }
    }

    REQUIRE( i < NUM_P); // Should have run out of heap before we ran out of pointers
    printf("Allocated %zu minimum size chunks\n", i);

    REQUIRE(multi_heap_free_size(heap) < before_free);
    multi_heap_check(heap, true);

    /* Free in random order */
    bool has_allocations = true;
    while (has_allocations) {
        i = rand() % NUM_P;
        multi_heap_free(heap, p[i]);
        p[i] = NULL;
        multi_heap_check(heap, true);

        has_allocations = false;
        for (i = 0; i < NUM_P && !has_allocations; i++) {
            has_allocations = (p[i] != NULL);
        }
    }

    /* all freed! */
    REQUIRE( before_free == multi_heap_free_size(heap) );
}

TEST_CASE("multi_heap_realloc()", "[multi_heap]")
{
    const size_t HEAP_SIZE = 4 * 1024;
    const uint32_t PATTERN = 0xABABDADA;
    uint8_t small_heap[HEAP_SIZE];
    multi_heap_handle_t heap = multi_heap_register(small_heap, sizeof(small_heap));

    uint32_t *a = (uint32_t *)multi_heap_malloc(heap, 64);
    uint32_t *b = (uint32_t *)multi_heap_malloc(heap, 32);
    REQUIRE( a != NULL );
    REQUIRE( b != NULL );
    REQUIRE( b > a); /* 'b' takes the block after 'a' */

    *a = PATTERN;
    uint32_t *c = (uint32_t *)multi_heap_realloc(heap, a, 72);
    REQUIRE( multi_heap_check(heap, true));
    REQUIRE(  c  != NULL );
    REQUIRE( c > b ); /* 'a' moves, 'c' takes the block after 'b' */
    REQUIRE( *c == PATTERN );

#ifndef MULTI_HEAP_POISONING_SLOW
    // "Slow" poisoning implementation doesn't reallocate in place, so these
    // test will fail...
    uint32_t *d = (uint32_t *)multi_heap_realloc(heap, c, 36);
    REQUIRE( multi_heap_check(heap, true) );
    REQUIRE( c == d ); /* 'c' block should be shrunk in-place */
    REQUIRE( *d == PATTERN);
    // biggest allocation possible to completely fill the block left free after it was reallocated
    uint32_t *e = (uint32_t *)multi_heap_malloc(heap, 60);
    REQUIRE( multi_heap_check(heap, true));
    REQUIRE( a == e ); /* 'e' takes the block formerly occupied by 'a' */

    multi_heap_free(heap, d);
    uint32_t *f = (uint32_t *)multi_heap_realloc(heap, b, 64);
    REQUIRE( multi_heap_check(heap, true) );
    REQUIRE( f == b ); /* 'b' should be extended in-place, over space formerly occupied by 'd' */

#define TOO_MUCH HEAP_SIZE + 1
    /* not enough contiguous space left in the heap */
    uint32_t *g = (uint32_t *)multi_heap_realloc(heap, e, TOO_MUCH);
    REQUIRE( g == NULL );

    multi_heap_free(heap, f);
    /* try again */
    g = (uint32_t *)multi_heap_realloc(heap, e, 128);
    REQUIRE( multi_heap_check(heap, true) );
    REQUIRE( e == g ); /* 'g' extends 'e' in place, into the space formerly held by 'f' */

#endif // MULTI_HEAP_POISONING_SLOW
}

// TLSF only accepts heaps aligned to 4-byte boundary so
// only aligned allocation tests make sense.
TEST_CASE("multi_heap aligned allocations", "[multi_heap]")
{
    uint8_t test_heap[4 * 1024];
    multi_heap_handle_t heap = multi_heap_register(test_heap, sizeof(test_heap));
    uint32_t aligments = 0; // starts from alignment by 4-byte boundary
    size_t old_size = multi_heap_free_size(heap);
    size_t leakage = 1024;
    printf("[ALIGNED_ALLOC] heap_size before: %d \n", old_size);

    printf("New heap:\n");
    multi_heap_dump(heap);
    printf("*********************\n");

    for(;aligments <= 256; aligments++) {

        //Use some stupid size value to test correct alignment even in strange
        //memory layout objects:
        uint8_t *buf = (uint8_t *)multi_heap_aligned_alloc(heap, (aligments + 137), aligments );
        if(((aligments & (aligments - 1)) != 0) || (!aligments)) {
            REQUIRE( buf == NULL );
        } else {
            REQUIRE( buf != NULL );
            REQUIRE((intptr_t)buf >= (intptr_t)test_heap);
            REQUIRE((intptr_t)buf < (intptr_t)(test_heap + sizeof(test_heap)));

            printf("[ALIGNED_ALLOC] alignment required: %u \n", aligments);
            printf("[ALIGNED_ALLOC] address of allocated memory: %p \n\n", (void *)buf);
            //Address of obtained block must be aligned with selected value
            REQUIRE(((intptr_t)buf & (aligments - 1)) == 0);

            //Write some data, if it corrupts memory probably the heap
            //canary verification will fail:
            memset(buf, 0xA5, (aligments + 137));

            multi_heap_free(heap, buf);
        }
    }

    /* Check that TLSF doesn't allocate a memory space smaller than required.
     * In any case, TLSF will write data in the previous block than the one
     * allocated. Thus, we should try to get/allocate this previous block. If
     * the poisoned filled pattern has beeen overwritten by TLSF, then this
     * previous block will trigger an exception.
     * More info on this bug in !16296. */
    const size_t size = 50; /* TLSF will round the size up */
    uint8_t *buf1 = (uint8_t *)multi_heap_aligned_alloc(heap, size, 4);
    uint8_t *buf2 = (uint8_t *)multi_heap_aligned_alloc(heap, size, 4);
    multi_heap_free(heap, buf1);
    /* By specifying a size equal of the gap between buf1 and buf2. We are
     * trying to force TLSF to allocate two consecutive blocks. */
    buf1 = (uint8_t *)multi_heap_aligned_alloc(heap, buf2 - buf1, 4);
    multi_heap_free(heap, buf2);


    printf("[ALIGNED_ALLOC] heap_size after: %d \n", multi_heap_free_size(heap));
    REQUIRE((old_size - multi_heap_free_size(heap)) <= leakage);
}

// TLSF has some overhead when allocating blocks, check that overhead
TEST_CASE("multi_heap allocation overhead", "[multi_heap]")
{
    uint8_t heapdata[4 * 1024];
    size_t alloc_size = 256;
    multi_heap_handle_t heap = multi_heap_register(heapdata, sizeof(heapdata));
    size_t free_bytes_1 = multi_heap_free_size(heap);

    /* Allocate any amount of data, in any case there will be an overhead */
    void *x = multi_heap_malloc(heap, alloc_size);

    /* free_bytes_2 should be free_bytes_1 - alloc_size - overhead.
     * We don't know the value of overhead, let's check that it is non-zero */
    size_t free_bytes_2 = multi_heap_free_size(heap);
    REQUIRE( free_bytes_1 > free_bytes_2 );
    REQUIRE( free_bytes_1 - free_bytes_2 > alloc_size );

    multi_heap_free(heap, x);
}

/* This test will corrupt the memory of a free block in the heap and check
 * that in the case of comprehensive poisoning the heap corruption is detected
 * by multi_heap_check(). For light poisoning and no poisoning, the test will
 * check that multi_heap_check() does not report the corruption.
 */
TEST_CASE("multi_heap poisoning detection", "[multi_heap]")
{
    const size_t HEAP_SIZE = 4 * 1024;

    /* define heap related data */
    uint8_t heap_mem[HEAP_SIZE];
    memset(heap_mem, 0x00, HEAP_SIZE);

    /* register the heap memory. One free block only will be available */
    multi_heap_handle_t heap = multi_heap_register(heap_mem, HEAP_SIZE);

    control_t *tlsf_ptr = (control_t*)(heap_mem + 20);
    const size_t control_t_size = tlsf_ptr->size;
    const size_t heap_t_size = 20;

    /* offset in memory at which to find the first free memory byte */
    const size_t free_memory_offset = heap_t_size + control_t_size + sizeof(block_header_t) - block_header_overhead;

    /* block header of the free block under test in the heap () */
    const block_header_t* block = (block_header_t*)(heap_mem + free_memory_offset - sizeof(block_header_t));

    /* actual number of bytes potentially filled with the free pattern in the free block under test */
    const size_t effective_free_size = block_size(block) - block_header_overhead - offsetof(block_header_t, next_free);

    /* variable used in the test */
    size_t affected_byte = 0x00;
    uint8_t original_value = 0x00;
    uint8_t corrupted_value = 0x00;

    /* repeat the corruption a few times to cover more of the free memory */
    for (size_t i = 0; i < effective_free_size; i++)
    {
        /* corrupt random bytes in the heap (it needs to be bytes from free memory in
         * order to check that the comprehensive poisoning is doing its job) */
        affected_byte = free_memory_offset + i;
        corrupted_value = (rand() % UINT8_MAX) | 1;

        /* keep the good value in store in order to check that when we set the byte back
         * to its original value, multi_heap_check() no longer returns the heap corruption. */
        original_value = heap_mem[affected_byte];

        /* make sure we are not replacing the original value with the same value */
        heap_mem[affected_byte] ^= corrupted_value;

        bool is_heap_ok = multi_heap_check(heap, true);
#ifdef CONFIG_HEAP_POISONING_COMPREHENSIVE
        /* check that multi_heap_check() detects the corruption */
        REQUIRE(is_heap_ok == false);
#else
        /* the comprehensive corruption is not checked in the multi_heap_check() */
        REQUIRE(is_heap_ok == true);
#endif
        /* fix the corruption  */
        heap_mem[affected_byte] = original_value;

        /* check that multi_heap_check() stops reporting the corruption */
        is_heap_ok = multi_heap_check(heap, true);
        REQUIRE(is_heap_ok == true);
    }
}
