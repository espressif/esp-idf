/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <stdio.h>
#include <pthread.h>
#include "unity.h"
#include "unity_fixture.h"

/* non-static to prevent optimization */
atomic_ullong g_atomic64;
atomic_uint   g_atomic32;
atomic_ushort g_atomic16;
atomic_uchar  g_atomic8;


TEST_GROUP(stdatomic);

TEST_SETUP(stdatomic)
{
}

TEST_TEAR_DOWN(stdatomic)
{
}

TEST(stdatomic, test_64bit_atomics)
{
    unsigned long long x64 = 0;
    g_atomic64 = 0;  // calls atomic_store

    x64 += atomic_fetch_or (&g_atomic64, 0x1111111111111111ULL);
    x64 += atomic_fetch_xor(&g_atomic64, 0x3333333333333333ULL);
    x64 += atomic_fetch_and(&g_atomic64, 0xf0f0f0f0f0f0f0f0ULL);
    x64 += atomic_fetch_sub(&g_atomic64, 0x0f0f0f0f0f0f0f0fULL);
    x64 += atomic_fetch_add(&g_atomic64, 0x2222222222222222ULL);

    TEST_ASSERT_EQUAL_HEX64(0x6464646464646464ULL, x64);
    TEST_ASSERT_EQUAL_HEX64(0x3333333333333333ULL, g_atomic64);  // calls atomic_load
}

TEST(stdatomic, test_32bit_atomics)
{
    unsigned int x32 = 0;
    g_atomic32 = 0;

    x32 += atomic_fetch_or (&g_atomic32, 0x11111111U);
    x32 += atomic_fetch_xor(&g_atomic32, 0x33333333U);
    x32 += atomic_fetch_and(&g_atomic32, 0xf0f0f0f0U);
    x32 += atomic_fetch_sub(&g_atomic32, 0x0f0f0f0fU);
    x32 += atomic_fetch_add(&g_atomic32, 0x22222222U);

    TEST_ASSERT_EQUAL_HEX32(0x64646464, x32);
    TEST_ASSERT_EQUAL_HEX32(0x33333333, g_atomic32);
}

TEST(stdatomic, test_16bit_atomics)
{
    unsigned int x16 = 0;
    g_atomic16 = 0;

    x16 += atomic_fetch_or (&g_atomic16, 0x1111);
    x16 += atomic_fetch_xor(&g_atomic16, 0x3333);
    x16 += atomic_fetch_and(&g_atomic16, 0xf0f0);
    x16 += atomic_fetch_sub(&g_atomic16, 0x0f0f);
    x16 += atomic_fetch_add(&g_atomic16, 0x2222);

    TEST_ASSERT_EQUAL_HEX16(0x6464, x16);
    TEST_ASSERT_EQUAL_HEX16(0x3333, g_atomic16);
}

TEST(stdatomic, test_8bit_atomics)
{
    unsigned int x8 = 0;
    g_atomic8 = 0;

    x8 += atomic_fetch_or (&g_atomic8, 0x11);
    x8 += atomic_fetch_xor(&g_atomic8, 0x33);
    x8 += atomic_fetch_and(&g_atomic8, 0xf0);
    x8 += atomic_fetch_sub(&g_atomic8, 0x0f);
    x8 += atomic_fetch_add(&g_atomic8, 0x22);

    TEST_ASSERT_EQUAL_HEX8(0x64, x8);
    TEST_ASSERT_EQUAL_HEX16(0x33, g_atomic8);
}

static void *exclusion_test_task(void *arg);

TEST(stdatomic, test_exclusion)
{
    /* Check 64-bit atomics for exclusion.
     * Only atomic_fetch_add/sub are checked, since all 64-bit atomics use the
     * same locking implementation.
     */
    g_atomic64 = 0;
    pthread_t thread1;
    pthread_t thread2;
    pthread_create(&thread1, NULL, exclusion_test_task, (void*) 1);
    pthread_create(&thread2, NULL, exclusion_test_task, (void*) 0);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    TEST_ASSERT_EQUAL(0, g_atomic64);
}

/* Two threads run in parallel, incrementing and decrementing
 * a single 64-bit variable. In the end the variable should
 * have the same value as at the start.
 */
static void* exclusion_test_task(void *varg)
{
    int arg = (int) varg;
    for (int i = 0; i < 1000000; ++i) {
        if (arg == 0) {
            atomic_fetch_add(&g_atomic64, 1ULL);
        } else {
            atomic_fetch_sub(&g_atomic64, 1ULL);
        }
    }
    return NULL;
}

TEST_GROUP_RUNNER(stdatomic)
{
    RUN_TEST_CASE(stdatomic, test_64bit_atomics)
    RUN_TEST_CASE(stdatomic, test_32bit_atomics)
    RUN_TEST_CASE(stdatomic, test_16bit_atomics)
    RUN_TEST_CASE(stdatomic, test_8bit_atomics)
    RUN_TEST_CASE(stdatomic, test_exclusion)
}
