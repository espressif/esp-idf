/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 Combined unit tests & benchmarking for spinlock "portMUX" functionality
*/

#include <esp_types.h>
#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "unity.h"
#include "esp_cpu.h"

#include "test_utils.h"

#if !CONFIG_FREERTOS_SMP    // Known issue in Amazon SMP FreeRTOS port IDF-6204

#define REPEAT_OPS 10000

static uint32_t start, end;

#define BENCHMARK_START() do {                  \
        start = esp_cpu_get_cycle_count();                     \
    } while(0)

#define BENCHMARK_END(OPERATION) do {                       \
        end = esp_cpu_get_cycle_count();                                          \
        printf("%s took %"PRIu32" cycles/op (%"PRIu32" cycles for %d ops)\n",     \
               OPERATION, (end - start)/REPEAT_OPS,                 \
               (end - start), REPEAT_OPS);                          \
    } while(0)

TEST_CASE("portMUX spinlocks (no contention)", "[freertos]")
{
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    BENCHMARK_START();

    for (int i = 0; i < REPEAT_OPS; i++) {
        portENTER_CRITICAL_ISR(&mux);
        portEXIT_CRITICAL_ISR(&mux);
    }
    BENCHMARK_END("no contention lock");

#if CONFIG_FREERTOS_UNICORE
    TEST_PERFORMANCE_LESS_THAN(FREERTOS_SPINLOCK_CYCLES_PER_OP_UNICORE, "%"PRIu32" cycles/op", ((end - start) / REPEAT_OPS));
#else
#if CONFIG_SPIRAM
    TEST_PERFORMANCE_LESS_THAN(FREERTOS_SPINLOCK_CYCLES_PER_OP_PSRAM, "%"PRIu32" cycles/op", ((end - start) / REPEAT_OPS));
#else
    TEST_PERFORMANCE_LESS_THAN(FREERTOS_SPINLOCK_CYCLES_PER_OP, "%"PRIu32" cycles/op", ((end - start) / REPEAT_OPS));
#endif
#endif
}

TEST_CASE("portMUX recursive locks (no contention)", "[freertos]")
{
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    BENCHMARK_START();

    const int RECURSE_COUNT = 25;

    for (int i = 0; i < REPEAT_OPS / RECURSE_COUNT; i++) {
        for (int j = 0; j < RECURSE_COUNT; j++) {
            portENTER_CRITICAL(&mux);
        }
        for (int j = 0; j < RECURSE_COUNT; j++) {
            portEXIT_CRITICAL(&mux);
        }
    }
    BENCHMARK_END("no contention recursive");

#if CONFIG_FREERTOS_UNICORE
    TEST_PERFORMANCE_LESS_THAN(FREERTOS_SPINLOCK_CYCLES_PER_OP_UNICORE, "%"PRIu32" cycles/op", ((end - start) / REPEAT_OPS));
#else
#if CONFIG_SPIRAM
    TEST_PERFORMANCE_LESS_THAN(FREERTOS_SPINLOCK_CYCLES_PER_OP_PSRAM, "%"PRIu32" cycles/op", ((end - start) / REPEAT_OPS));
#else
    TEST_PERFORMANCE_LESS_THAN(FREERTOS_SPINLOCK_CYCLES_PER_OP, "%"PRIu32" cycles/op", ((end - start) / REPEAT_OPS));
#endif
#endif
}

#if CONFIG_FREERTOS_NUMBER_OF_CORES == 2

static volatile int shared_value;
static portMUX_TYPE *shared_mux;
static SemaphoreHandle_t done_sem;

static void task_shared_value_increment(void *ignore)
{
    for (int i = 0; i < REPEAT_OPS; i++) {
        portENTER_CRITICAL(shared_mux);
        shared_value++;
        portEXIT_CRITICAL(shared_mux);
    }
    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}

TEST_CASE("portMUX cross-core locking", "[freertos]")
{
    shared_mux = heap_caps_malloc(sizeof(portMUX_TYPE), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    done_sem = xSemaphoreCreateCounting(2, 0);
    portMUX_INITIALIZE(shared_mux);
    shared_value = 0;

    BENCHMARK_START();

    xTaskCreatePinnedToCore(task_shared_value_increment, "INC0", 2048, NULL, UNITY_FREERTOS_PRIORITY + 1, NULL, UNITY_FREERTOS_CPU ? 0 : 1);
    xTaskCreatePinnedToCore(task_shared_value_increment, "INC1", 2048, NULL, UNITY_FREERTOS_PRIORITY + 1, NULL, UNITY_FREERTOS_CPU);

    for (int i = 0; i < 2; i++) {
        if (!xSemaphoreTake(done_sem, 10000 / portTICK_PERIOD_MS)) {
            TEST_FAIL_MESSAGE("done_sem not released by test task");
        }
    }

    BENCHMARK_END("cross-core incrementing");
    vSemaphoreDelete(done_sem);
    free(shared_mux);

    TEST_ASSERT_EQUAL_INT(REPEAT_OPS * 2, shared_value);
}

void portmux_high_contention_test(uint32_t lock_malloc_caps)
{
    const int TOTAL_TASKS = 8; /* half on each core */
    shared_mux = heap_caps_malloc(sizeof(portMUX_TYPE), lock_malloc_caps);
    done_sem = xSemaphoreCreateCounting(TOTAL_TASKS, 0);
    portMUX_INITIALIZE(shared_mux);
    shared_value = 0;

    BENCHMARK_START();

    for (int i = 0; i < TOTAL_TASKS / 2; i++) {
        /* as each task has a higher priority than previous, expect
           them to preempt the earlier created task, at least on the
           other core (this core has the unity task, until that
           blocks)... */
        xTaskCreatePinnedToCore(task_shared_value_increment, "INC0", 2048, NULL, tskIDLE_PRIORITY + 1 + i, NULL, UNITY_FREERTOS_CPU ? 0 : 1);
        xTaskCreatePinnedToCore(task_shared_value_increment, "INC1", 2048, NULL, tskIDLE_PRIORITY + 1 + i, NULL, UNITY_FREERTOS_CPU);
    }

    for (int i = 0; i < TOTAL_TASKS; i++) {
        if (!xSemaphoreTake(done_sem, 10000 / portTICK_PERIOD_MS)) {
            TEST_FAIL_MESSAGE("done_sem not released by test task");
        }
    }

    BENCHMARK_END("cross-core high contention");
    vSemaphoreDelete(done_sem);
    free(shared_mux);

    TEST_ASSERT_EQUAL_INT(REPEAT_OPS * TOTAL_TASKS, shared_value);
}

TEST_CASE("portMUX high contention", "[freertos]")
{
    portmux_high_contention_test(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
}

#if CONFIG_SPIRAM_USE_MALLOC || CONFIG_SPIRAM_USE_CAPS_ALLOC
TEST_CASE("portMUX high contention, PSRAM", "[freertos]")
{
    portmux_high_contention_test(MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
}
#endif// CONFIG_SPIRAM_USE_MALLOC || CONFIG_SPIRAM_USE_CAPS_ALLOC

#endif // CONFIG_FREERTOS_NUMBER_OF_CORES == 2

#endif // !CONFIG_FREERTOS_SMP
