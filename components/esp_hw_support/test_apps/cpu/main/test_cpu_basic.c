/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "soc/soc_caps.h"
#include "esp_cpu.h"
#include "esp_timer.h"
#include "test_utils.h"

static intptr_t __attribute__((noinline)) get_call_addr_delta(void)
{
    intptr_t return_addr = (intptr_t)__builtin_return_address(0);
    intptr_t call_addr = esp_cpu_get_call_addr(return_addr);
    return return_addr - call_addr;
}

typedef struct {
    TaskHandle_t waiter;
    int observed_core_id;
} core_id_task_ctx_t;

static void core_id_task(void *arg)
{
    core_id_task_ctx_t *ctx = (core_id_task_ctx_t *)arg;

    ctx->observed_core_id = esp_cpu_get_core_id();
    xTaskNotifyGive(ctx->waiter);
    vTaskDelete(NULL);
}

TEST_CASE("CPU basic: current core id matches pinned task core", "[cpu][cpu_basic]")
{
    const TaskHandle_t waiter = xTaskGetCurrentTaskHandle();
    core_id_task_ctx_t task_ctx[SOC_CPU_CORES_NUM] = {0};

    for (int core_id = 0; core_id < SOC_CPU_CORES_NUM; ++core_id) {
        task_ctx[core_id].waiter = waiter;
        task_ctx[core_id].observed_core_id = -1;
        TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCore(core_id_task,
                                                          "cpu_core_id",
                                                          2048,
                                                          &task_ctx[core_id],
                                                          UNITY_FREERTOS_PRIORITY + 1,
                                                          NULL,
                                                          core_id));
    }

    for (int core_id = 0; core_id < SOC_CPU_CORES_NUM; ++core_id) {
        /* Use pdFALSE (counting mode) so accumulated notifications don't cause
         * a false failure if both tasks complete before the first wait. */
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    }

    for (int core_id = 0; core_id < SOC_CPU_CORES_NUM; ++core_id) {
        TEST_ASSERT_EQUAL(core_id, task_ctx[core_id].observed_core_id);
    }
}

TEST_CASE("CPU basic: stack pointer is near current stack frame", "[cpu][cpu_basic]")
{
    uint32_t local_marker = 0;
    uintptr_t sp = (uintptr_t)esp_cpu_get_sp();
    uintptr_t local = (uintptr_t)&local_marker;
    uintptr_t delta = (sp > local) ? (sp - local) : (local - sp);

    TEST_ASSERT_TRUE(delta < 4096);
}

TEST_CASE("CPU basic: cycle count increases", "[cpu][cpu_basic]")
{
    esp_cpu_cycle_count_t start = esp_cpu_get_cycle_count();
    for (volatile int i = 0; i < 256; i++) {
        __asm__ __volatile__("nop");
    }
    esp_cpu_cycle_count_t end = esp_cpu_get_cycle_count();

    TEST_ASSERT_TRUE((end - start) > 0);
}

#define CYCLE_WINDOW_MS     100

typedef struct {
    TaskHandle_t waiter;
    esp_cpu_cycle_count_t busy_cycles;
    esp_cpu_cycle_count_t idle_cycles;
    int64_t busy_us;
    int64_t idle_us;
} cycle_rate_ctx_t;

/*
 * Runs pinned, because the counter belongs to a core: a task that migrates
 * mid-measurement subtracts one core's count from another's.
 */
static void cycle_rate_task(void *arg)
{
    cycle_rate_ctx_t *ctx = (cycle_rate_ctx_t *)arg;

    /* Spinning, so the core never sleeps. Interrupts that preempt the spin are
     * still the core executing, so they are counted too and do not skew this. */
    int64_t t0 = esp_timer_get_time();
    esp_cpu_cycle_count_t c0 = esp_cpu_get_cycle_count();
    while (esp_timer_get_time() - t0 < CYCLE_WINDOW_MS * 1000) { }
    ctx->busy_cycles = esp_cpu_get_cycle_count() - c0;
    ctx->busy_us = esp_timer_get_time() - t0;

    /* Blocked, so the idle task owns the core and puts it to sleep. */
    t0 = esp_timer_get_time();
    c0 = esp_cpu_get_cycle_count();
    vTaskDelay(pdMS_TO_TICKS(CYCLE_WINDOW_MS));
    ctx->idle_cycles = esp_cpu_get_cycle_count() - c0;
    ctx->idle_us = esp_timer_get_time() - t0;

    xTaskNotifyGive(ctx->waiter);
    vTaskDelete(NULL);
}

static void measure_cycle_rates(cycle_rate_ctx_t *ctx)
{
    ctx->waiter = xTaskGetCurrentTaskHandle();
    TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCore(cycle_rate_task,
                                                      "cycle_rate",
                                                      2048,
                                                      ctx,
                                                      UNITY_FREERTOS_PRIORITY + 1,
                                                      NULL,
                                                      esp_cpu_get_core_id()));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(4 * CYCLE_WINDOW_MS + 1000)));
}

/*
 * Whether the counter keeps running through WFI is what SOC_CPU_CCOUNT_STOPS_IN_WFI
 * records, and what decides if the counter can serve as a time base -- it is
 * why CONFIG_ESP_TRACE_TS_SOURCE_CCOUNT is not offered where it stalls. Check
 * both directions, so a new target that needs the cap but lacks it fails here
 * instead of producing silently compressed timestamps.
 */
TEST_CASE("CPU basic: cycle count while the CPU is idle matches SOC_CPU_CCOUNT_STOPS_IN_WFI", "[cpu][cpu_basic]")
{
    cycle_rate_ctx_t ctx = {0};
    measure_cycle_rates(&ctx);

    /* Rates, not raw counts: vTaskDelay rounds to whole ticks, so the idle
     * window is a little shorter than the spin it is compared against. */
    uint64_t busy_rate = (uint64_t)ctx.busy_cycles * 1000000ULL / (uint64_t)ctx.busy_us;
    uint64_t idle_rate = (uint64_t)ctx.idle_cycles * 1000000ULL / (uint64_t)ctx.idle_us;

    printf("idle: %llu Hz over %" PRId64 " us, busy: %llu Hz over %" PRId64 " us\n",
           (unsigned long long)idle_rate, ctx.idle_us,
           (unsigned long long)busy_rate, ctx.busy_us);

#if SOC_CPU_CCOUNT_STOPS_IN_WFI
    /* A tenth is a wide margin around the ~0.1% measured on ESP32-C6 and
     * ESP32-C3; anything near parity means the counter did not stall. */
    TEST_ASSERT_TRUE(idle_rate < busy_rate / 10);
#else
    /* Measured at parity to within 0.01% on ESP32-C5/C61/H4/P4. */
    TEST_ASSERT_TRUE(idle_rate > busy_rate - busy_rate / 20);
#endif
}

TEST_CASE("CPU basic: current privilege level is sane", "[cpu][cpu_basic]")
{
    int privilege = esp_cpu_get_curr_privilege_level();

#if __XTENSA__
    TEST_ASSERT_EQUAL(-1, privilege);
#else
    TEST_ASSERT_TRUE(privilege >= 0);
#endif
}

TEST_CASE("CPU basic: pc to addr conversion matches architecture", "[cpu][cpu_basic]")
{
    uintptr_t addr = (uintptr_t)esp_cpu_pc_to_addr(0x00001234U);

#if __XTENSA__
    TEST_ASSERT_EQUAL_HEX32(0x40001234U, addr);
#else
    TEST_ASSERT_EQUAL_HEX32(0x00001234U, addr);
#endif
}

TEST_CASE("CPU basic: call addr delta matches architecture", "[cpu][cpu_basic]")
{
#if __XTENSA__
    TEST_ASSERT_EQUAL(3, get_call_addr_delta());
#else
    TEST_ASSERT_EQUAL(4, get_call_addr_delta());
#endif
}

TEST_CASE("CPU basic: compare and set succeeds on matching value", "[cpu][cpu_basic]")
{
    volatile uint32_t value = 1;

    TEST_ASSERT_TRUE(esp_cpu_compare_and_set(&value, 1, 2));
    TEST_ASSERT_EQUAL_UINT32(2, value);
}

TEST_CASE("CPU basic: compare and set fails on mismatched value", "[cpu][cpu_basic]")
{
    volatile uint32_t value = 2;

    TEST_ASSERT_FALSE(esp_cpu_compare_and_set(&value, 1, 3));
    TEST_ASSERT_EQUAL_UINT32(2, value);
}

TEST_CASE("CPU basic: thread pointer can be set and restored", "[cpu][cpu_basic]")
{
    uintptr_t marker_words[4] = {0};
    void *original = esp_cpu_get_threadptr();
    void *readback;
    void *restored;

    esp_cpu_set_threadptr((void *)marker_words);
    readback = esp_cpu_get_threadptr();
    esp_cpu_set_threadptr(original);
    restored = esp_cpu_get_threadptr();

    TEST_ASSERT_EQUAL_PTR(marker_words, readback);
    TEST_ASSERT_EQUAL_PTR(original, restored);
}
