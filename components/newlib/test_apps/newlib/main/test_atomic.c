/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "unity.h"
#include <stdatomic.h>
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_cpu.h"
#include "esp_private/cache_utils.h"

#define RECORD_TIME_PREPARE() uint32_t __t1, __t2
#define RECORD_TIME_START()   do {__t1 = esp_cpu_get_cycle_count();}while(0)
#define RECORD_TIME_END(p_time) do{__t2 = esp_cpu_get_cycle_count(); *p_time = (__t2-__t1);}while(0)


#define TEST_TIMES  11

//Test twice, and only get the result of second time, to avoid influence of cache miss
#define TEST_WRAP_START()   \
            RECORD_TIME_PREPARE(); \
            spi_flash_disable_interrupts_caches_and_other_cpu(); \
            for (int i = 0; i < 2; i++) { \
                RECORD_TIME_START();

#define TEST_WRAP_END(output)  \
                RECORD_TIME_END(output); \
            } \
            spi_flash_enable_interrupts_caches_and_other_cpu();

typedef void (*test_f)(uint32_t* t_op);

static const char TAG[] = "test_atomic";
static uint32_t s_t_ref;

static void sorted_array_insert(uint32_t* array, int* size, uint32_t item)
{
    int pos;
    for (pos = *size; pos>0; pos--) {
        if (array[pos-1] < item) break;
        array[pos] = array[pos-1];
    }
    array[pos]=item;
    (*size)++;
}

static void test_flow(const char* name, test_f func)
{
    int t_flight_num = 0;
    uint32_t t_flight_sorted[TEST_TIMES];

    for (int i = 0; i < TEST_TIMES; i++) {
        uint32_t t_op;
        func(&t_op);
        sorted_array_insert(t_flight_sorted, &t_flight_num, t_op);
    }
    for (int i = 0; i < TEST_TIMES; i++) {
        ESP_LOGI(TAG, "%s: %" PRIu32 " ops", name, t_flight_sorted[i]-s_t_ref);
    }
}

static IRAM_ATTR void test_ref(uint32_t* t_op)
{
    TEST_WRAP_START()
    TEST_WRAP_END(t_op)
    s_t_ref = *t_op;
}

static IRAM_ATTR void test_atomic_load(uint32_t* t_op)
{
    atomic_uint_fast32_t var;
    uint32_t target = rand();
    TEST_WRAP_START()
    target = atomic_load(&var);
    TEST_WRAP_END(t_op)
    (void) target;
}

static IRAM_ATTR void test_atomic_store(uint32_t* t_op)
{
    atomic_uint_fast32_t var;
    uint32_t src = rand();
    TEST_WRAP_START()
    atomic_store(&var, src);
    TEST_WRAP_END(t_op)
}

static IRAM_ATTR void test_atomic_store_load(uint32_t* t_op)
{
    atomic_uint_fast32_t var;
    uint32_t src = rand();
    TEST_WRAP_START()
    atomic_store(&var, src);
    src = atomic_load(&var);
    TEST_WRAP_END(t_op)
}

static IRAM_ATTR void test_atomic_fetch_and(uint32_t* t_op)
{
    atomic_uint_fast32_t var;
    uint32_t src = rand();
    TEST_WRAP_START()
    src = atomic_fetch_and(&var, src);
    TEST_WRAP_END(t_op)
}

static IRAM_ATTR void test_atomic_fetch_or(uint32_t* t_op)
{
    atomic_uint_fast32_t var;
    uint32_t src = rand();
    TEST_WRAP_START()
    src = atomic_fetch_or(&var, src);
    TEST_WRAP_END(t_op)
}

static IRAM_ATTR void test_atomic_compare_exchange(uint32_t* t_op)
{
    atomic_uint_fast32_t var;
    uint32_t src = rand();
    uint32_t cmp = rand();
    bool res;
    TEST_WRAP_START()
    res = atomic_compare_exchange_weak(&var, &src, cmp);
    TEST_WRAP_END(t_op)
    (void) res;
}

TEST_CASE("test atomic","[atomic]")
{
    test_flow("ref", test_ref);

    test_flow("atomic_load", test_atomic_load);
    test_flow("atomic_store", test_atomic_store);
    test_flow("atomic_compare_exchange", test_atomic_compare_exchange);
    test_flow("atomic_store + atomic_load", test_atomic_store_load);
    test_flow("atomic_and", test_atomic_fetch_and);
    test_flow("atomic_or", test_atomic_fetch_or);
}
