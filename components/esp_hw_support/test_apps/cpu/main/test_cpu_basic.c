/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "soc/soc_caps.h"
#include "esp_cpu.h"
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
