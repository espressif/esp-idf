/*
 Combined unit tests & benchmarking for spinlock "portMUX" functionality
*/

#include <esp_types.h>
#include <stdio.h>
#include "rom/ets_sys.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"
#include "soc/uart_reg.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/cpu.h"

#include "idf_performance.h"

#define REPEAT_OPS 10000

static uint32_t start, end;

#define BENCHMARK_START() do {                  \
        RSR(CCOUNT, start);                     \
    } while(0)

#define BENCHMARK_END(OPERATION) do {                       \
        RSR(CCOUNT, end);                                           \
        printf("%s took %d cycles/op (%d cycles for %d ops)\n",     \
               OPERATION, (end - start)/REPEAT_OPS,                 \
               (end - start), REPEAT_OPS);                          \
    } while(0)

TEST_CASE("portMUX spinlocks (no contention)", "[freertos]")
{
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    BENCHMARK_START();

    for (int i = 0; i < REPEAT_OPS; i++) {
        portENTER_CRITICAL(&mux);
        portEXIT_CRITICAL(&mux);
    }
    BENCHMARK_END("no contention lock");

#ifdef CONFIG_FREERTOS_UNICORE
    TEST_PERFORMANCE_LESS_THAN(FREERTOS_SPINLOCK_CYCLES_PER_OP_UNICORE, "%d cycles/op", ((end - start)/REPEAT_OPS));
#else
    TEST_PERFORMANCE_LESS_THAN(FREERTOS_SPINLOCK_CYCLES_PER_OP, "%d cycles/op", ((end - start)/REPEAT_OPS));
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
}

#if portNUM_PROCESSORS == 2

static volatile int shared_value;
static portMUX_TYPE shared_mux;
static xSemaphoreHandle done_sem;

static void task_shared_value_increment(void *ignore)
{
    for (int i = 0; i < REPEAT_OPS; i++) {
        portENTER_CRITICAL(&shared_mux);
        shared_value++;
        portEXIT_CRITICAL(&shared_mux);
    }
    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}

TEST_CASE("portMUX cross-core locking", "[freertos]")
{
    done_sem = xSemaphoreCreateCounting(2, 0);
    vPortCPUInitializeMutex(&shared_mux);
    shared_value = 0;

    BENCHMARK_START();

    xTaskCreatePinnedToCore(task_shared_value_increment, "INC0", 2048, NULL, UNITY_FREERTOS_PRIORITY + 1, NULL, UNITY_FREERTOS_CPU ? 0 : 1);
    xTaskCreatePinnedToCore(task_shared_value_increment, "INC1", 2048, NULL, UNITY_FREERTOS_PRIORITY + 1, NULL, UNITY_FREERTOS_CPU);

    for(int i = 0; i < 2; i++) {
        if(!xSemaphoreTake(done_sem, 10000/portTICK_PERIOD_MS)) {
            TEST_FAIL_MESSAGE("done_sem not released by test task");
        }
    }

    BENCHMARK_END("cross-core incrementing");
    vSemaphoreDelete(done_sem);

    TEST_ASSERT_EQUAL_INT(REPEAT_OPS * 2, shared_value);
}

TEST_CASE("portMUX high contention", "[freertos]")
{
    const int TOTAL_TASKS = 8; /* half on each core */
    done_sem = xSemaphoreCreateCounting(TOTAL_TASKS, 0);
    vPortCPUInitializeMutex(&shared_mux);
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

    for(int i = 0; i < TOTAL_TASKS; i++) {
        if(!xSemaphoreTake(done_sem, 10000/portTICK_PERIOD_MS)) {
            TEST_FAIL_MESSAGE("done_sem not released by test task");
        }
    }

    BENCHMARK_END("cross-core high contention");
    vSemaphoreDelete(done_sem);

    TEST_ASSERT_EQUAL_INT(REPEAT_OPS * TOTAL_TASKS, shared_value);
}

#endif // portNUM_PROCESSORS == 2

