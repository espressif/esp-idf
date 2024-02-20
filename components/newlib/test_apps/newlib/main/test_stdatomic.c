/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
#include "esp_pthread.h"
#include "freertos/portmacro.h"
#include "unity.h"

/* non-static to prevent optimization */
atomic_ullong g_atomic64;
atomic_uint   g_atomic32;
atomic_ushort g_atomic16;
atomic_uchar  g_atomic8;


TEST_CASE("stdatomic - test_64bit_atomics", "[newlib_stdatomic]")
{
    unsigned long long x64 = 0;
    g_atomic64 = 0;  // calls atomic_store

    x64 += atomic_fetch_or (&g_atomic64, 0x1111111111111111ULL);
    x64 += atomic_fetch_xor(&g_atomic64, 0x3333333333333333ULL);
    x64 += atomic_fetch_and(&g_atomic64, 0xf0f0f0f0f0f0f0f0ULL);
    x64 += atomic_fetch_sub(&g_atomic64, 0x0f0f0f0f0f0f0f0fULL);
    x64 += atomic_fetch_add(&g_atomic64, 0x2222222222222222ULL);
#ifndef __clang__
    x64 += __atomic_fetch_nand_8 (&g_atomic64, 0xAAAAAAAAAAAAAAAAULL, 0);

    TEST_ASSERT_EQUAL_HEX64(0x9797979797979797ULL, x64);
    TEST_ASSERT_EQUAL_HEX64(0xDDDDDDDDDDDDDDDDULL, g_atomic64);  // calls atomic_load
#else
    TEST_ASSERT_EQUAL_HEX64(0x6464646464646464ULL, x64);
    TEST_ASSERT_EQUAL_HEX64(0x3333333333333333ULL, g_atomic64);  // calls atomic_load
#endif
}

TEST_CASE("stdatomic - test_32bit_atomics", "[newlib_stdatomic]")
{
    unsigned int x32 = 0;
    g_atomic32 = 0;

    x32 += atomic_fetch_or (&g_atomic32, 0x11111111U);
    x32 += atomic_fetch_xor(&g_atomic32, 0x33333333U);
    x32 += atomic_fetch_and(&g_atomic32, 0xf0f0f0f0U);
    x32 += atomic_fetch_sub(&g_atomic32, 0x0f0f0f0fU);
    x32 += atomic_fetch_add(&g_atomic32, 0x22222222U);
#ifndef __clang__
    x32 += __atomic_fetch_nand_4 (&g_atomic32, 0xAAAAAAAAU, 0);

    TEST_ASSERT_EQUAL_HEX32(0x97979797U, x32);
    TEST_ASSERT_EQUAL_HEX32(0xDDDDDDDDU, g_atomic32);
#else
    TEST_ASSERT_EQUAL_HEX32(0x64646464U, x32);
    TEST_ASSERT_EQUAL_HEX32(0x33333333U, g_atomic32);  // calls atomic_load
#endif
}

TEST_CASE("stdatomic - test_16bit_atomics", "[newlib_stdatomic]")
{
    unsigned int x16 = 0;
    g_atomic16 = 0;

    x16 += atomic_fetch_or (&g_atomic16, 0x1111);
    x16 += atomic_fetch_xor(&g_atomic16, 0x3333);
    x16 += atomic_fetch_and(&g_atomic16, 0xf0f0);
    x16 += atomic_fetch_sub(&g_atomic16, 0x0f0f);
    x16 += atomic_fetch_add(&g_atomic16, 0x2222);
#ifndef __clang__
    x16 += __atomic_fetch_nand_2 (&g_atomic16, 0xAAAA, 0);

    TEST_ASSERT_EQUAL_HEX16(0x9797, x16);
    TEST_ASSERT_EQUAL_HEX16(0xDDDD, g_atomic16);
#else
    TEST_ASSERT_EQUAL_HEX16(0x6464, x16);
    TEST_ASSERT_EQUAL_HEX16(0x3333, g_atomic16);  // calls atomic_load
#endif
}

TEST_CASE("stdatomic - test_8bit_atomics", "[newlib_stdatomic]")
{
    unsigned int x8 = 0;
    g_atomic8 = 0;

    x8 += atomic_fetch_or (&g_atomic8, 0x11);
    x8 += atomic_fetch_xor(&g_atomic8, 0x33);
    x8 += atomic_fetch_and(&g_atomic8, 0xf0);
    x8 += atomic_fetch_sub(&g_atomic8, 0x0f);
    x8 += atomic_fetch_add(&g_atomic8, 0x22);
#ifndef __clang__
    x8 += __atomic_fetch_nand_1 (&g_atomic8, 0xAA, 0);

    TEST_ASSERT_EQUAL_HEX8(0x97, x8);
    TEST_ASSERT_EQUAL_HEX8(0xDD, g_atomic8);
#else
    TEST_ASSERT_EQUAL_HEX8(0x64, x8);
    TEST_ASSERT_EQUAL_HEX8(0x33, g_atomic8);  // calls atomic_load
#endif
}

#ifndef __clang__
TEST_CASE("stdatomic - test_64bit_atomics", "[newlib_stdatomic]")
{
    unsigned long long x64 = 0;
    g_atomic64 = 0;  // calls atomic_store

    x64 += __atomic_or_fetch_8 (&g_atomic64, 0x1111111111111111ULL, 0);
    x64 += __atomic_xor_fetch_8(&g_atomic64, 0x3333333333333333ULL, 0);
    x64 += __atomic_and_fetch_8(&g_atomic64, 0xf0f0f0f0f0f0f0f0ULL, 0);
    x64 += __atomic_sub_fetch_8(&g_atomic64, 0x0f0f0f0f0f0f0f0fULL, 0);
    x64 += __atomic_add_fetch_8(&g_atomic64, 0x2222222222222222ULL, 0);
    x64 += __atomic_nand_fetch_8(&g_atomic64, 0xAAAAAAAAAAAAAAAAULL, 0);

    TEST_ASSERT_EQUAL_HEX64(0x7575757575757574ULL, x64);
    TEST_ASSERT_EQUAL_HEX64(0xDDDDDDDDDDDDDDDDULL, g_atomic64);  // calls atomic_load
}

TEST_CASE("stdatomic - test_32bit_atomics", "[newlib_stdatomic]")
{
    unsigned int x32 = 0;
    g_atomic32 = 0;

    x32 += __atomic_or_fetch_4 (&g_atomic32, 0x11111111U, 0);
    x32 += __atomic_xor_fetch_4(&g_atomic32, 0x33333333U, 0);
    x32 += __atomic_and_fetch_4(&g_atomic32, 0xf0f0f0f0U, 0);
    x32 += __atomic_sub_fetch_4(&g_atomic32, 0x0f0f0f0fU, 0);
    x32 += __atomic_add_fetch_4(&g_atomic32, 0x22222222U, 0);
    x32 += __atomic_nand_fetch_4 (&g_atomic32, 0xAAAAAAAAU, 0);

    TEST_ASSERT_EQUAL_HEX32(0x75757574U, x32);
    TEST_ASSERT_EQUAL_HEX32(0xDDDDDDDDU, g_atomic32);
}

TEST_CASE("stdatomic - test_16bit_atomics", "[newlib_stdatomic]")
{
    unsigned int x16 = 0;
    g_atomic16 = 0;

    x16 += __atomic_or_fetch_2 (&g_atomic16, 0x1111, 0);
    x16 += __atomic_xor_fetch_2(&g_atomic16, 0x3333, 0);
    x16 += __atomic_and_fetch_2(&g_atomic16, 0xf0f0, 0);
    x16 += __atomic_sub_fetch_2(&g_atomic16, 0x0f0f, 0);
    x16 += __atomic_add_fetch_2(&g_atomic16, 0x2222, 0);
    x16 += __atomic_nand_fetch_2 (&g_atomic16, 0xAAAA, 0);

    TEST_ASSERT_EQUAL_HEX16(0x7574, x16);
    TEST_ASSERT_EQUAL_HEX16(0xDDDD, g_atomic16);
}

TEST_CASE("stdatomic - test_8bit_atomics", "[newlib_stdatomic]")
{
    unsigned int x8 = 0;
    g_atomic8 = 0;

    x8 += __atomic_or_fetch_1 (&g_atomic8, 0x11, 0);
    x8 += __atomic_xor_fetch_1(&g_atomic8, 0x33, 0);
    x8 += __atomic_and_fetch_1(&g_atomic8, 0xf0, 0);
    x8 += __atomic_sub_fetch_1(&g_atomic8, 0x0f, 0);
    x8 += __atomic_add_fetch_1(&g_atomic8, 0x22, 0);
    x8 += __atomic_nand_fetch_1 (&g_atomic8, 0xAA, 0);

    TEST_ASSERT_EQUAL_HEX8(0x74, x8);
    TEST_ASSERT_EQUAL_HEX8(0xDD, g_atomic8);
}

#endif // #ifndef __clang__


#define TEST_EXCLUSION(n) TEST_CASE("stdatomic - test_" #n "bit_exclusion", "[newlib_stdatomic]") \
{                                                                            \
    g_atomic ## n = 0;                                                       \
    pthread_t thread1;                                                       \
    pthread_t thread2;                                                       \
    esp_pthread_cfg_t cfg = esp_pthread_get_default_config();                \
    cfg.pin_to_core = (xPortGetCoreID() + 1) % portNUM_PROCESSORS;           \
    esp_pthread_set_cfg(&cfg);                                               \
    pthread_create(&thread1, NULL, exclusion_task_ ## n, (void*) 1);         \
    cfg.pin_to_core = xPortGetCoreID();                                      \
    esp_pthread_set_cfg(&cfg);                                               \
    pthread_create(&thread2, NULL, exclusion_task_ ## n, (void*) 0);         \
    pthread_join(thread1, NULL);                                             \
    pthread_join(thread2, NULL);                                             \
    TEST_ASSERT_EQUAL(0, g_atomic ## n);                                     \
}

#define TEST_EXCLUSION_TASK(n) static void* exclusion_task_ ## n(void *varg) \
{                                                                            \
    int arg = (int) varg;                                                    \
    for (int i = 0; i < 1000000; ++i) {                                      \
        if (arg == 0) {                                                      \
            atomic_fetch_add(&g_atomic ## n, 1ULL);                          \
        } else {                                                             \
            atomic_fetch_sub(&g_atomic ## n, 1ULL);                          \
        }                                                                    \
    }                                                                        \
    return NULL;                                                             \
}

TEST_EXCLUSION_TASK(64)
TEST_EXCLUSION(64)

TEST_EXCLUSION_TASK(32)
TEST_EXCLUSION(32)

TEST_EXCLUSION_TASK(16)
TEST_EXCLUSION(16)

TEST_EXCLUSION_TASK(8)
TEST_EXCLUSION(8)


#define ITER_COUNT 20000

#define TEST_RACE_OPERATION(ASSERT_SUFFIX, NAME, LHSTYPE, PRE, POST, INIT, FINAL) \
                                                                                  \
static _Atomic LHSTYPE var_##NAME;                                                \
                                                                                  \
static void *test_thread_##NAME (void *arg)                                       \
{                                                                                 \
  for (int i = 0; i < ITER_COUNT; i++)                                            \
    {                                                                             \
      PRE var_##NAME POST;                                                        \
    }                                                                             \
  return NULL;                                                                    \
}                                                                                 \
                                                                                  \
TEST_CASE("stdatomic - test_" #NAME, "[newlib_stdatomic]")                        \
{                                                                                 \
  pthread_t thread_id1;                                                           \
  pthread_t thread_id2;                                                           \
  var_##NAME = (INIT);                                                            \
  esp_pthread_cfg_t cfg = esp_pthread_get_default_config();                       \
  cfg.pin_to_core = (xPortGetCoreID() + 1) % portNUM_PROCESSORS;                  \
  esp_pthread_set_cfg(&cfg);                                                      \
  pthread_create (&thread_id1, NULL, test_thread_##NAME, NULL);                   \
  cfg.pin_to_core = xPortGetCoreID();                                             \
  esp_pthread_set_cfg(&cfg);                                                      \
  pthread_create (&thread_id2, NULL, test_thread_##NAME, NULL);                   \
  pthread_join (thread_id1, NULL);                                                \
  pthread_join (thread_id2, NULL);                                                \
  TEST_ASSERT_EQUAL##ASSERT_SUFFIX((FINAL), var_##NAME);                          \
}

// Note that the assert at the end is doing an excat bitwise comparison.
// This easily can fail due to rounding errors. However, there is currently
// no corresponding Unity assert macro for long double. USE THIS WITH CARE!
#define TEST_RACE_OPERATION_LONG_DOUBLE(NAME, LHSTYPE, PRE, POST, INIT, FINAL) \
                                                                               \
static _Atomic LHSTYPE var_##NAME;                                             \
                                                                               \
static void *test_thread_##NAME (void *arg)                                    \
{                                                                              \
  for (int i = 0; i < ITER_COUNT; i++)                                         \
    {                                                                          \
      PRE var_##NAME POST;                                                     \
    }                                                                          \
  return NULL;                                                                 \
}                                                                              \
                                                                               \
TEST_CASE("stdatomic - test_" #NAME, "[newlib_stdatomic]")                     \
{                                                                              \
  pthread_t thread_id1;                                                        \
  pthread_t thread_id2;                                                        \
  var_##NAME = (INIT);                                                         \
  const LHSTYPE EXPECTED = (FINAL);                                            \
  esp_pthread_cfg_t cfg = esp_pthread_get_default_config();                    \
  cfg.pin_to_core = (xPortGetCoreID() + 1) % portNUM_PROCESSORS;               \
  esp_pthread_set_cfg(&cfg);                                                   \
  pthread_create (&thread_id1, NULL, test_thread_##NAME, NULL);                \
  cfg.pin_to_core = xPortGetCoreID();                                          \
  esp_pthread_set_cfg(&cfg);                                                   \
  pthread_create (&thread_id2, NULL, test_thread_##NAME, NULL);                \
  pthread_join (thread_id1, NULL);                                             \
  pthread_join (thread_id2, NULL);                                             \
  TEST_ASSERT(EXPECTED == var_##NAME);                                         \
}

TEST_RACE_OPERATION ( ,uint8_add, uint8_t, , += 1, 0, (uint8_t) (2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint8_add_3, uint8_t, , += 3, 0, (uint8_t) (6*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint8_postinc, uint8_t, , ++, 0, (uint8_t) (2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint8_preinc, uint8_t, ++, , 0, (uint8_t) (2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint8_sub, uint8_t, , -= 1, 0, (uint8_t) -(2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint8_sub_3, uint8_t, , -= 3, 0, (uint8_t) -(6*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint8_postdec, uint8_t, , --, 0, (uint8_t) -(2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint8_predec, uint8_t, --, , 0, (uint8_t) -(2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint8_mul, uint8_t, , *= 3, 1, (uint8_t) 0x1)

TEST_RACE_OPERATION ( ,uint16_add, uint16_t, , += 1, 0, (uint16_t) (2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint16_add_3, uint16_t, , += 3, 0, (uint16_t) (6*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint16_postinc, uint16_t, , ++, 0, (uint16_t) (2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint16_preinc, uint16_t, ++, , 0, (uint16_t) (2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint16_sub, uint16_t, , -= 1, 0, (uint16_t) -(2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint16_sub_3, uint16_t, , -= 3, 0, (uint16_t) -(6*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint16_postdec, uint16_t, , --, 0, (uint16_t) -(2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint16_predec, uint16_t, --, , 0, (uint16_t) -(2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint16_mul, uint16_t, , *= 3, 1, (uint16_t) 0x6D01)

TEST_RACE_OPERATION ( ,uint32_add, uint32_t, , += 1, 0, (uint32_t) (2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint32_add_3, uint32_t, , += 3, 0, (uint32_t) (6*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint32_postinc, uint32_t, , ++, 0, (uint32_t) (2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint32_preinc, uint32_t, ++, , 0, (uint32_t) (2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint32_sub, uint32_t, , -= 1, 0, (uint32_t) -(2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint32_sub_3, uint32_t, , -= 3, 0, (uint32_t) -(6*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint32_postdec, uint32_t, , --, 0, (uint32_t) -(2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint32_predec, uint32_t, --, , 0, (uint32_t) -(2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint32_mul, uint32_t, , *= 3, 1, (uint32_t) 0xC1E36D01U)

TEST_RACE_OPERATION ( ,uint64_add, uint64_t, , += 1, 0, (uint64_t) (2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint64_add_3, uint64_t, , += 3, 0, (uint64_t) (6*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint64_add_neg, uint64_t, , += 1, -10000, (uint64_t) (2*ITER_COUNT-10000))
TEST_RACE_OPERATION ( ,uint64_postinc, uint64_t, , ++, 0, (uint64_t) (2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint64_postinc_neg, uint64_t, , ++, -10000, (uint64_t) (2*ITER_COUNT-10000))
TEST_RACE_OPERATION ( ,uint64_preinc, uint64_t, ++, , 0, (uint64_t) (2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint64_preinc_neg, uint64_t, ++, , -10000, (uint64_t) (2*ITER_COUNT-10000))
TEST_RACE_OPERATION ( ,uint64_sub, uint64_t, , -= 1, 0, (uint64_t) -(2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint64_sub_3, uint64_t, , -= 3, 0, (uint64_t) -(6*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint64_sub_neg, uint64_t, , -= 1, 10000, (uint64_t) ((-2*ITER_COUNT)+10000))
TEST_RACE_OPERATION ( ,uint64_postdec, uint64_t, , --, 0, (uint64_t) -(2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint64_postdec_neg, uint64_t, , --, 10000, (uint64_t) ((-2*ITER_COUNT)+10000))
TEST_RACE_OPERATION ( ,uint64_predec, uint64_t, --, , 0, (uint64_t) -(2*ITER_COUNT))
TEST_RACE_OPERATION ( ,uint64_predec_neg, uint64_t, --, , 10000, (uint64_t) ((-2*ITER_COUNT)+10000))
TEST_RACE_OPERATION ( ,uint64_mul, uint64_t, , *= 3, 1, (uint64_t) 0x988EE974C1E36D01ULL)

TEST_RACE_OPERATION (_FLOAT ,float_add, float, , += 1, 0, (2*ITER_COUNT))
TEST_RACE_OPERATION (_FLOAT ,complex_float_add, _Complex float, , += 1, 0, (2*ITER_COUNT))
TEST_RACE_OPERATION (_FLOAT ,float_postinc, float, , ++, 0, (2*ITER_COUNT))
TEST_RACE_OPERATION (_FLOAT ,float_preinc, float, ++, , 0, (2*ITER_COUNT))
TEST_RACE_OPERATION (_FLOAT ,float_sub, float, , -= 1, 0, -(2*ITER_COUNT))
TEST_RACE_OPERATION (_FLOAT ,complex_float_sub, _Complex float, , -= 1, 0, -(2*ITER_COUNT))
TEST_RACE_OPERATION (_FLOAT ,float_postdec, float, , --, 0, -(2*ITER_COUNT))
TEST_RACE_OPERATION (_FLOAT ,float_predec, float, --, , 0, -(2*ITER_COUNT))

TEST_RACE_OPERATION (_DOUBLE ,double_add, double, , += 1, 0, (2*ITER_COUNT))
TEST_RACE_OPERATION (_DOUBLE ,complex_double_add, _Complex double, , += 1, 0, (2*ITER_COUNT))
TEST_RACE_OPERATION (_DOUBLE ,double_postinc, double, , ++, 0, (2*ITER_COUNT))
TEST_RACE_OPERATION (_DOUBLE ,double_preinc, double, ++, , 0, (2*ITER_COUNT))
TEST_RACE_OPERATION (_DOUBLE ,double_sub, double, , -= 1, 0, -(2*ITER_COUNT))
TEST_RACE_OPERATION (_DOUBLE ,complex_double_sub, _Complex double, , -= 1, 0, -(2*ITER_COUNT))
TEST_RACE_OPERATION (_DOUBLE ,double_postdec, double, , --, 0, -(2*ITER_COUNT))
TEST_RACE_OPERATION (_DOUBLE ,double_predec, double, --, , 0, -(2*ITER_COUNT))

TEST_RACE_OPERATION_LONG_DOUBLE (long_double_add, long double, , += 1, 0, (2*ITER_COUNT))
TEST_RACE_OPERATION_LONG_DOUBLE (complex_long_double_add, _Complex long double, , += 1, 0, (2*ITER_COUNT))
TEST_RACE_OPERATION_LONG_DOUBLE (long_double_postinc, long double, , ++, 0, (2*ITER_COUNT))
TEST_RACE_OPERATION_LONG_DOUBLE (long_double_sub, long double, , -= 1, 0, -(2*ITER_COUNT))
TEST_RACE_OPERATION_LONG_DOUBLE (long_double_preinc, long double, ++, , 0, (2*ITER_COUNT))
TEST_RACE_OPERATION_LONG_DOUBLE (complex_long_double_sub, _Complex long double, , -= 1, 0, -(2*ITER_COUNT))
TEST_RACE_OPERATION_LONG_DOUBLE (long_double_postdec, long double, , --, 0, -(2*ITER_COUNT))
TEST_RACE_OPERATION_LONG_DOUBLE (long_double_predec, long double, --, , 0, -(2*ITER_COUNT))
