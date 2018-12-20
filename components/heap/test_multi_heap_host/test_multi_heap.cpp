#include "catch.hpp"
#include "multi_heap.h"

#include "../multi_heap_config.h"

#include <string.h>
#include <assert.h>

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
    uint8_t small_heap[128];

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
    uint8_t small_heap[256];
    multi_heap_handle_t heap = multi_heap_register(small_heap, sizeof(small_heap));

    const size_t alloc_size = 24;

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
    REQUIRE( p[3] == big ); /* big should go where p[3] was freed from */
    multi_heap_free(heap, big);

    multi_heap_free(heap, p[2]);

    printf("0 allocations:\n");
    multi_heap_dump(heap);
    printf("****************\n");

    big = multi_heap_malloc(heap, alloc_size * 2);
    REQUIRE( p[0] == big ); /* big should now go where p[0] was freed from */
    multi_heap_free(heap, big);
}

/* Test that malloc/free does not leave free space fragmented */
TEST_CASE("multi_heap defrag", "[multi_heap]")
{
    void *p[4];
    uint8_t small_heap[512];
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
    uint8_t small_heap[512];
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


TEST_CASE("multi_heap many random allocations", "[multi_heap]")
{
    uint8_t big_heap[1024];
    const int NUM_POINTERS = 64;

    printf("Running multi-allocation test...\n");

    void *p[NUM_POINTERS] = { 0 };
    size_t s[NUM_POINTERS] = { 0 };
    multi_heap_handle_t heap = multi_heap_register(big_heap, sizeof(big_heap));

    const size_t initial_free = multi_heap_free_size(heap);

    const int ITERATIONS = 100000;

    for (int i = 0; i < ITERATIONS; i++) {
        /* check all pointers allocated so far are valid inside big_heap */
        for (int j = 0; j < NUM_POINTERS; j++) {
            if (p[j] != NULL) {
            }
        }

        uint8_t n = rand() % NUM_POINTERS;

        if (rand() % 4 == 0) {
            /* 1 in 4 iterations, try to realloc the buffer instead
               of using malloc/free
            */
            size_t new_size = rand() % 1024;
            void *new_p = multi_heap_realloc(heap, p[n], new_size);
            printf("realloc %p -> %p (%zu -> %zu)\n", p[n], new_p, s[n], new_size);
            multi_heap_check(heap, true);
            if (new_size == 0 || new_p != NULL) {
                p[n] = new_p;
                s[n] = new_size;
                if (new_size > 0) {
                    REQUIRE( p[n] >= big_heap );
                    REQUIRE( p[n] < big_heap + sizeof(big_heap) );
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
            REQUIRE( p[n] < big_heap + sizeof(big_heap) );
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
}

TEST_CASE("multi_heap_get_info() function", "[multi_heap]")
{
    uint8_t heapdata[256];
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
    uint8_t heapdata[16384];
    void *p[sizeof(heapdata) / sizeof(void *)];
    const size_t NUM_P = sizeof(p) / sizeof(void *);
    multi_heap_handle_t heap = multi_heap_register(heapdata, sizeof(heapdata));

    size_t before_free = multi_heap_free_size(heap);

    size_t i;
    for (i = 0; i < NUM_P; i++) {
        p[i] = multi_heap_malloc(heap, 1);
        if (p[i] == NULL) {
            break;
        }
    }

    REQUIRE( i < NUM_P); // Should have run out of heap before we ran out of pointers
    printf("Allocated %zu minimum size chunks\n", i);

    REQUIRE( 0 == multi_heap_free_size(heap) );
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
    const uint32_t PATTERN = 0xABABDADA;
    uint8_t small_heap[300];
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

    uint32_t *e = (uint32_t *)multi_heap_malloc(heap, 64);
    REQUIRE( multi_heap_check(heap, true));
    REQUIRE( a == e ); /* 'e' takes the block formerly occupied by 'a' */

    multi_heap_free(heap, d);
    uint32_t *f = (uint32_t *)multi_heap_realloc(heap, b, 64);
    REQUIRE( multi_heap_check(heap, true) );
    REQUIRE( f == b ); /* 'b' should be extended in-place, over space formerly occupied by 'd' */

#ifdef MULTI_HEAP_POISONING
#define TOO_MUCH 92 + 1
#else
#define TOO_MUCH 128 + 1
#endif
    /* not enough contiguous space left in the heap */
    uint32_t *g = (uint32_t *)multi_heap_realloc(heap, e, TOO_MUCH);
    REQUIRE( g == NULL );

    multi_heap_free(heap, f);
    /* try again */
    g = (uint32_t *)multi_heap_realloc(heap, e, 128);
    REQUIRE( multi_heap_check(heap, true) );
    REQUIRE( e == g ); /* 'g' extends 'e' in place, into the space formerly held by 'f' */
#endif
}

TEST_CASE("corrupt heap block", "[multi_heap]")
{
    uint8_t small_heap[256];
    multi_heap_handle_t heap = multi_heap_register(small_heap, sizeof(small_heap));

    void *a = multi_heap_malloc(heap, 32);
    REQUIRE( multi_heap_check(heap, true) );
    memset(a, 0xEE, 64);
    REQUIRE( !multi_heap_check(heap, true) );
}

TEST_CASE("unaligned heaps", "[multi_heap]")
{
    const size_t CHUNK_LEN = 256;
    const size_t CANARY_LEN = 16;
    const uint8_t CANARY_BYTE = 0x3E;
    uint8_t heap_chunk[CHUNK_LEN + CANARY_LEN * 2];

    /* Put some canary bytes before and after the bytes we intend to use for
       the heap, make sure they aren't ever overwritten */
    memset(heap_chunk, CANARY_BYTE, CANARY_LEN);
    memset(heap_chunk + CANARY_LEN + CHUNK_LEN, CANARY_BYTE, CANARY_LEN);

    for (int i = 0; i < 8; i++) {
        printf("Testing with offset %d\n", i);
        multi_heap_handle_t heap = multi_heap_register(heap_chunk + CANARY_LEN + i, CHUNK_LEN - i);
        multi_heap_info_t info;

        REQUIRE( multi_heap_check(heap, true) );

        multi_heap_get_info(heap, &info);

        REQUIRE( info.total_free_bytes > CHUNK_LEN - 64 - i );
        REQUIRE( info.largest_free_block > CHUNK_LEN - 64 - i );

        void *a = multi_heap_malloc(heap, info.largest_free_block);
        REQUIRE( a != NULL );
        memset(a, 0xAA, info.largest_free_block);

        REQUIRE( multi_heap_check(heap, true) );

        multi_heap_free(heap, a);

        REQUIRE( multi_heap_check(heap, true) );

        for (unsigned j = 0; j < CANARY_LEN; j++) { // check canaries
            REQUIRE( heap_chunk[j] == CANARY_BYTE );
            REQUIRE( heap_chunk[CHUNK_LEN + CANARY_LEN + j] == CANARY_BYTE );
        }
    }
}
