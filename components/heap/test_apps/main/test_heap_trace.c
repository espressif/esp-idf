/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/*
 Generic test for heap tracing support

 Only compiled in if CONFIG_HEAP_TRACING is set
*/

#include <esp_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"
#include "unity.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef CONFIG_HEAP_TRACING
// only compile in heap tracing tests if tracing is enabled

#include "esp_heap_trace.h"

TEST_CASE("heap trace leak check", "[heap]")
{
    heap_trace_record_t recs[8];
    heap_trace_init_standalone(recs, 8);

    printf("Leak check test\n"); // Print something before trace starts, or stdout allocations skew total counts
    fflush(stdout);

    heap_trace_start(HEAP_TRACE_LEAKS);

    void *a = malloc(64);
    memset(a, '3', 64);

    void *b = malloc(96);
    memset(b, '4', 11);

    printf("a.address %p vs %p b.address %p vs %p\n", a, recs[0].address, b, recs[1].address);

    heap_trace_dump();
    TEST_ASSERT_EQUAL(2, heap_trace_get_count());

    heap_trace_record_t trace_a, trace_b;
    heap_trace_get(0, &trace_a);
    heap_trace_get(1, &trace_b);

    printf("trace_a.address %p trace_bb.address %p\n", trace_a.address, trace_b.address);

    TEST_ASSERT_EQUAL_PTR(a, trace_a.address);
    TEST_ASSERT_EQUAL_PTR(b, trace_b.address);

    TEST_ASSERT_EQUAL_PTR(recs[0].address, trace_a.address);
    TEST_ASSERT_EQUAL_PTR(recs[1].address, trace_b.address);

    free(a);

    TEST_ASSERT_EQUAL(1, heap_trace_get_count());

    heap_trace_get(0, &trace_b);
    TEST_ASSERT_EQUAL_PTR(b, trace_b.address);

    /* buffer deletes trace_a when freed,
       so trace_b at head of buffer */
    TEST_ASSERT_EQUAL_PTR(recs[0].address, trace_b.address);

    heap_trace_stop();
}

TEST_CASE("heap trace wrapped buffer check", "[heap]")
{
    const size_t N = 8;
    heap_trace_record_t recs[N];
    heap_trace_init_standalone(recs, N);

    heap_trace_start(HEAP_TRACE_LEAKS);

    void *ptrs[N+1];
    for (int i = 0; i < N+1; i++) {
        ptrs[i] = malloc(i*3);
    }

    // becuase other mallocs happen as part of this control flow,
    // we can't guarantee N entries of ptrs[] are in the heap check buffer.
    // but we should guarantee at least the last one is
    bool saw_last_ptr = false;
    for (int i = 0; i < N; i++) {
        heap_trace_record_t rec;
        heap_trace_get(i, &rec);
        if (rec.address == ptrs[N-1]) {
            saw_last_ptr = true;
        }
    }
    TEST_ASSERT(saw_last_ptr);

    void *other = malloc(6);

    heap_trace_dump();

    for (int i = 0; i < N+1; i++) {
        free(ptrs[i]);
    }

    heap_trace_dump();

    bool saw_other = false;

    for (int i = 0; i < heap_trace_get_count(); i++) {
        heap_trace_record_t rec;
        heap_trace_get(i, &rec);

        // none of ptr[]s should be in the heap trace any more
        for (int j = 0; j < N+1; j++) {
            TEST_ASSERT_NOT_EQUAL(ptrs[j], rec.address);
        }
        if (rec.address == other) {
            saw_other = true;
        }
    }

    // 'other' pointer should be somewhere in the leak dump
    TEST_ASSERT(saw_other);

    heap_trace_stop();
}

static void print_floats_task(void *ignore)
{
    heap_trace_start(HEAP_TRACE_ALL);
    char buf[16] = { };
    volatile float f = 12.3456;
    sprintf(buf, "%.4f", f);
    TEST_ASSERT_EQUAL_STRING("12.3456", buf);
    heap_trace_stop();

    vTaskDelete(NULL);
}

TEST_CASE("can trace allocations made by newlib", "[heap]")
{
    const size_t N = 8;
    heap_trace_record_t recs[N];
    heap_trace_init_standalone(recs, N);

    /* Verifying that newlib code performs an allocation is very fiddly:

       - Printing a float allocates data associated with the task, but only the
       first time a task prints a float of this length. So we do it in a one-shot task
       to avoid possibility it already happened.

       - If newlib is updated this test may start failing if the printf() implementation
       changes. (This version passes for both nano & regular formatting in newlib 2.2.0)

       - We also do the tracing in the task so we only capture things directly related to it.
    */

    xTaskCreate(print_floats_task, "print_float", 4096, NULL, 5, NULL);
    vTaskDelay(10);

    /* has to be at least a few as newlib allocates via multiple different function calls */
    TEST_ASSERT(heap_trace_get_count() > 3);
}


#endif
