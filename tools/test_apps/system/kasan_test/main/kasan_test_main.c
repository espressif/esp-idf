/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Unity-based KASAN test application.
 *
 * With CONFIG_KASAN_NO_HALT: all tests run in one boot cycle; each test
 * triggers a bug and asserts that kasan_get_error_count() increased.
 *
 * Without CONFIG_KASAN_NO_HALT (default): select individual tests via the
 * Unity menu.  Each error test causes an abort; pytest verifies the panic.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "unity.h"
#include "esp_log.h"
#include "esp_kasan.h"

static const char *TAG = "kasan_test";

/* ---------------------------------------------------------------------- */

TEST_CASE("heap buffer overflow", "[kasan]")
{
#if CONFIG_KASAN_NO_HALT
    kasan_reset_error_count();
#endif
    char *buf = (char *)malloc(16);
    TEST_ASSERT_NOT_NULL(buf);
    memset(buf, 'A', 16);
    buf[16] = 'X';  /* write into right redzone */
#if CONFIG_KASAN_NO_HALT
    TEST_ASSERT_GREATER_THAN(0, kasan_get_error_count());
#endif
    free(buf);
}

/* ---------------------------------------------------------------------- */

TEST_CASE("use-after-free write", "[kasan]")
{
#if CONFIG_KASAN_NO_HALT
    kasan_reset_error_count();
#endif
    char *buf = (char *)malloc(32);
    TEST_ASSERT_NOT_NULL(buf);
    memset(buf, 0, 32);
    free(buf);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuse-after-free"
    buf[4] = 'Y';
#pragma GCC diagnostic pop
#if CONFIG_KASAN_NO_HALT
    TEST_ASSERT_GREATER_THAN(0, kasan_get_error_count());
#endif
}

/* ---------------------------------------------------------------------- */

TEST_CASE("use-after-free read", "[kasan]")
{
#if CONFIG_KASAN_NO_HALT
    kasan_reset_error_count();
#endif
    int *buf = (int *)malloc(4 * sizeof(int));
    TEST_ASSERT_NOT_NULL(buf);
    buf[0] = 42;
    free(buf);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuse-after-free"
    volatile int val = buf[0];
    (void)val;
#pragma GCC diagnostic pop
#if CONFIG_KASAN_NO_HALT
    TEST_ASSERT_GREATER_THAN(0, kasan_get_error_count());
#endif
}

/* ---------------------------------------------------------------------- */

TEST_CASE("heap buffer underflow", "[kasan]")
{
#if CONFIG_KASAN_NO_HALT
    kasan_reset_error_count();
#endif
    char *buf = (char *)malloc(16);
    TEST_ASSERT_NOT_NULL(buf);
    buf[-1] = 'Z';  /* write into left redzone */
#if CONFIG_KASAN_NO_HALT
    TEST_ASSERT_GREATER_THAN(0, kasan_get_error_count());
    /* The underflow write corrupted the redzone header; skip free to avoid
     * side-effects from a bad header read.  Small intentional leak. */
#else
    free(buf);
#endif
}

/* ---------------------------------------------------------------------- */

TEST_CASE("large heap overflow", "[kasan]")
{
#if CONFIG_KASAN_NO_HALT
    kasan_reset_error_count();
#endif
    char *buf = (char *)malloc(8);
    TEST_ASSERT_NOT_NULL(buf);
    for (int i = 0; i < 8; i++) {
        buf[i] = 0;
    }
    buf[8] = 'X';  /* overflow into right redzone */
#if CONFIG_KASAN_NO_HALT
    TEST_ASSERT_GREATER_THAN(0, kasan_get_error_count());
#endif
    free(buf);
}

/* ---------------------------------------------------------------------- */

TEST_CASE("no false positive", "[kasan]")
{
#if CONFIG_KASAN_NO_HALT
    kasan_reset_error_count();
#endif
    char *buf = (char *)malloc(32);
    TEST_ASSERT_NOT_NULL(buf);
    memset(buf, 'A', 32);
    volatile char c = buf[31];
    (void)c;
    free(buf);
#if CONFIG_KASAN_NO_HALT
    TEST_ASSERT_EQUAL_UINT32(0, kasan_get_error_count());
#endif
    ESP_LOGI(TAG, "no-bug test PASSED");
}

/* ---------------------------------------------------------------------- */
/*
 * GCC -fsanitize=kernel-address instrumentation calls a sized family of
 * stubs: __asan_load<N>_noabort and __asan_store<N>_noabort for
 * N in {1, 2, 4, 8, 16, N}.  The two tests below exercise every stub
 * directly so the runtime contract (link symbol present, valid access
 * passes, poisoned access reports an error) is verified for each size.
 *
 * Calling the stubs directly is intentional: GCC's choice of which sized
 * stub to emit depends on access size, alignment, and target ISA, so
 * relying on instrumentation alone leaves gaps (this is exactly how the
 * 16-byte variants were missed previously: the dedicated test code
 * never tripped GCC into emitting `__asan_*16_noabort`).
 */

extern void __asan_load1_noabort(void *addr);
extern void __asan_load2_noabort(void *addr);
extern void __asan_load4_noabort(void *addr);
extern void __asan_load8_noabort(void *addr);
extern void __asan_load16_noabort(void *addr);
/*
 * The size parameter for the variable-size ASAN check stubs is declared as
 * `int` by GCC's builtin, so we have to match that type here even though
 * the size in practice is non-negative.  Using `size_t` would trigger
 * -Werror=builtin-declaration-mismatch under newer GCC (15+).
 */
extern void __asan_loadN_noabort(void *addr, int size);

extern void __asan_store1_noabort(void *addr);
extern void __asan_store2_noabort(void *addr);
extern void __asan_store4_noabort(void *addr);
extern void __asan_store8_noabort(void *addr);
extern void __asan_store16_noabort(void *addr);
extern void __asan_storeN_noabort(void *addr, int size);

/* All 12 sized ASAN check stubs in one call set.
 * Returns the number of stub calls made (always 12). */
static unsigned exercise_all_asan_stubs(void *p)
{
    __asan_load1_noabort(p);
    __asan_load2_noabort(p);
    __asan_load4_noabort(p);
    __asan_load8_noabort(p);
    __asan_load16_noabort(p);
    __asan_loadN_noabort(p, 7);

    __asan_store1_noabort(p);
    __asan_store2_noabort(p);
    __asan_store4_noabort(p);
    __asan_store8_noabort(p);
    __asan_store16_noabort(p);
    __asan_storeN_noabort(p, 7);

    return 12;
}

TEST_CASE("asan stubs valid access no error", "[kasan]")
{
#if CONFIG_KASAN_NO_HALT
    kasan_reset_error_count();
#endif
    /* malloc(64) gives us 64 valid bytes; the largest stub reads 16 bytes
     * starting at p, so p..p+63 is safely inside the allocation. */
    char *buf = (char *)malloc(64);
    TEST_ASSERT_NOT_NULL(buf);
    memset(buf, 'A', 64);

    unsigned calls = exercise_all_asan_stubs(buf);
    TEST_ASSERT_EQUAL_UINT(12, calls);

#if CONFIG_KASAN_NO_HALT
    /* Each stub touched only valid bytes; no error should have been raised. */
    TEST_ASSERT_EQUAL_UINT32(0, kasan_get_error_count());
#endif
    free(buf);
    ESP_LOGI(TAG, "asan stubs valid-access test PASSED");
}

#if CONFIG_KASAN_NO_HALT
TEST_CASE("asan stubs poisoned access all sizes", "[kasan]")
{
    kasan_reset_error_count();

    /* A freed pointer sits in the quarantine FIFO with its full block
     * shadow poisoned, so every stub size will trip the shadow check. */
    char *buf = (char *)malloc(64);
    TEST_ASSERT_NOT_NULL(buf);
    memset(buf, 0, 64);
    free(buf);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuse-after-free"
    unsigned calls = exercise_all_asan_stubs(buf);
#pragma GCC diagnostic pop
    TEST_ASSERT_EQUAL_UINT(12, calls);

    /* Exactly one error per stub call: 6 sized loads + 6 sized stores. */
    TEST_ASSERT_EQUAL_UINT32(12, kasan_get_error_count());
    ESP_LOGI(TAG, "asan stubs poisoned-access test PASSED (12 errors)");
}
#endif /* CONFIG_KASAN_NO_HALT */

/* ---------------------------------------------------------------------- */

void app_main(void)
{
    ESP_LOGI(TAG, "KASAN test application starting");
    unity_run_menu();
}
