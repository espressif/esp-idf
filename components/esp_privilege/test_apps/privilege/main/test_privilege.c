/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "esp_cpu.h"
#include "privilege_test_fixture.h"
#include "riscv/rvruntime-frames.h"
#include "riscv/rv_utils.h"
#include "privilege_code.h"
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
#include "hal/assist_debug_ll.h"
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

#define USER_STACK_SIZE     2048
#define TEST_TASK_STACK     4096
#define TEST_TASK_PRIO      (TEST_MAIN_TASK_PRIO - 1)

typedef struct {
    uint8_t stack[USER_STACK_SIZE] __attribute__((aligned(16)));
    privilege_shared_t shared;
    esp_err_t err;
    int ret;
    SemaphoreHandle_t done;
} privilege_task_ctx_t;

static privilege_task_ctx_t s_ctx[2];

static void privilege_count_task(void *arg)
{
    privilege_task_ctx_t *ctx = (privilege_task_ctx_t *)arg;
    ctx->err = test_privilege_call(privilege_count_until_stop, 0, (uint32_t)&ctx->shared,
                                   ctx->stack, sizeof(ctx->stack), &ctx->ret);
    xSemaphoreGive(ctx->done);
    vTaskDelete(NULL);
}

static TaskHandle_t start_privilege_count_task(privilege_task_ctx_t *ctx, BaseType_t core)
{
    TaskHandle_t handle = NULL;
    ctx->shared.counter = 0;
    ctx->shared.stop = 0;
    ctx->err = ESP_FAIL;
    ctx->ret = -1;
    ctx->done = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(ctx->done);
    TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCore(privilege_count_task, "privilege", TEST_TASK_STACK, ctx,
                                                      TEST_TASK_PRIO, &handle, core));
    return handle;
}

static void stop_privilege_count_task(privilege_task_ctx_t *ctx)
{
    ctx->shared.stop = 1;
    TEST_ASSERT_TRUE(xSemaphoreTake(ctx->done, pdMS_TO_TICKS(1000)));
    vSemaphoreDelete(ctx->done);
    TEST_ASSERT_EQUAL(ESP_OK, ctx->err);
    TEST_ASSERT_EQUAL_UINT32(ctx->shared.counter, (uint32_t)ctx->ret);
}

/* Returns true if the task, which must not be running, was preempted while executing U-mode code */
static bool task_preempted_in_privilege(TaskHandle_t task)
{
    /* The first field of the TCB is the saved stack pointer, pointing to the task's trap frame */
    const RvExcFrame *frame = *(RvExcFrame **)task;
    return (frame->mstatus & MSTATUS_MPP) == 0;
}

TEST_CASE("U-mode call returns the value of the U-mode code", "[privilege]")
{
    static uint8_t stack[USER_STACK_SIZE] __attribute__((aligned(16)));
    privilege_shared_t shared = { 0 };
    int ret = 0;

    TEST_ASSERT_EQUAL(ESP_OK, test_privilege_call(privilege_arith, 7, (uint32_t)&shared, stack, sizeof(stack), &ret));
    TEST_ASSERT_EQUAL(8, ret);
    TEST_ASSERT_EQUAL_UINT32(21, shared.out);

    /* The U-mode code runs on the given stack */
    TEST_ASSERT_EQUAL(ESP_OK, test_privilege_call(privilege_get_sp, 0, 0, stack, sizeof(stack), &ret));
    TEST_ASSERT_GREATER_THAN_UINT32((uint32_t)stack, (uint32_t)ret);
    TEST_ASSERT_LESS_OR_EQUAL_UINT32((uint32_t)stack + sizeof(stack), (uint32_t)ret);

    /* The U-mode code does not receive the M-mode global pointer */
    TEST_ASSERT_EQUAL(ESP_OK, test_privilege_call(privilege_get_gp, 0, 0, stack, sizeof(stack), &ret));
    TEST_ASSERT_EQUAL_HEX32(0, ret);
}

TEST_CASE("U-mode call can be repeated", "[privilege]")
{
    static uint8_t stack[USER_STACK_SIZE] __attribute__((aligned(16)));
    privilege_shared_t shared = { 0 };

    for (int i = 0; i < 10000; i++) {
        int ret = -1;
        TEST_ASSERT_EQUAL(ESP_OK, test_privilege_call(privilege_arith, i, (uint32_t)&shared, stack, sizeof(stack), &ret));
        TEST_ASSERT_EQUAL(i + 1, ret);
        TEST_ASSERT_EQUAL_UINT32(i * 3, shared.out);
    }
}

static volatile uint32_t s_busy_counter;
static volatile bool s_busy_stop;

static void busy_task(void *arg)
{
    SemaphoreHandle_t done = (SemaphoreHandle_t)arg;
    while (!s_busy_stop) {
        s_busy_counter++;
    }
    xSemaphoreGive(done);
    vTaskDelete(NULL);
}

TEST_CASE("U-mode code is preempted and resumed", "[privilege]")
{
    privilege_task_ctx_t *ctx = &s_ctx[0];
    SemaphoreHandle_t busy_done = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(busy_done);
    s_busy_counter = 0;
    s_busy_stop = false;

    /* The U-mode task and an M-mode task of the same priority share core 0 through time slicing,
     * while this (higher priority) task periodically preempts both of them */
    TaskHandle_t privilege_task = start_privilege_count_task(ctx, 0);
    TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCore(busy_task, "busy", TEST_TASK_STACK, busy_done,
                                                      TEST_TASK_PRIO, NULL, 0));
    int preempted_in_privilege = 0;
    uint32_t last_counter = 0;
    int counter_progress = 0;
    const int samples = pdMS_TO_TICKS(500);
    for (int i = 0; i < samples; i++) {
        vTaskDelay(1);
        if (task_preempted_in_privilege(privilege_task)) {
            preempted_in_privilege++;
        }
        if (ctx->shared.counter != last_counter) {
            counter_progress++;
            last_counter = ctx->shared.counter;
        }
    }
    s_busy_stop = true;
    TEST_ASSERT_TRUE(xSemaphoreTake(busy_done, pdMS_TO_TICKS(1000)));
    vSemaphoreDelete(busy_done);
    stop_privilege_count_task(ctx);

    printf("U-mode counter %"PRIu32", M-mode counter %"PRIu32", preempted in U-mode %d, progress %d\n",
           ctx->shared.counter, s_busy_counter, preempted_in_privilege, counter_progress);
    TEST_ASSERT_GREATER_THAN_UINT32(0, s_busy_counter);
    TEST_ASSERT_GREATER_THAN(0, preempted_in_privilege);
    /* The U-mode code keeps running between the preemptions */
    TEST_ASSERT_GREATER_THAN(samples / 5, counter_progress);
}

#if !CONFIG_FREERTOS_UNICORE
TEST_CASE("U-mode code runs on both cores", "[privilege]")
{
    start_privilege_count_task(&s_ctx[0], 0);
    start_privilege_count_task(&s_ctx[1], 1);

    uint32_t counters_before[2] = { s_ctx[0].shared.counter, s_ctx[1].shared.counter };
    vTaskDelay(pdMS_TO_TICKS(100));
    uint32_t counters_after[2] = { s_ctx[0].shared.counter, s_ctx[1].shared.counter };

    stop_privilege_count_task(&s_ctx[0]);
    stop_privilege_count_task(&s_ctx[1]);

    TEST_ASSERT_GREATER_THAN_UINT32(counters_before[0], counters_after[0]);
    TEST_ASSERT_GREATER_THAN_UINT32(counters_before[1], counters_after[1]);
}
#endif /* !CONFIG_FREERTOS_UNICORE */

TEST_CASE("M-mode illegal instruction still panics", "[privilege_panic][ignore]")
{
    __asm__ volatile(".word 0");
    TEST_FAIL_MESSAGE("Should have panicked");
}

#if !CONFIG_FREERTOS_UNICORE
static volatile bool s_migration_observed;
static void migration_observer(void *arg)
{
    TaskHandle_t target = (TaskHandle_t)arg;
    TickType_t start = xTaskGetTickCount();
    /* Occupy core 1. The unpinned U task must resume on core 0 while main sleeps. */
    while (xTaskGetTickCount() - start < pdMS_TO_TICKS(1000)) {
        if (xTaskGetCurrentTaskHandleForCore(0) == target) {
            s_migration_observed = true;
            break;
        }
    }
    vTaskDelete(NULL);
}

TEST_CASE("unpinned run migrates between cores without changing affinity", "[privilege]")
{
    privilege_task_ctx_t *ctx = &s_ctx[0];
    TaskHandle_t task = start_privilege_count_task(ctx, tskNO_AFFINITY);
    TickType_t start = xTaskGetTickCount();
    bool source_seen = false;
    while (xTaskGetTickCount() - start < pdMS_TO_TICKS(1000)) {
        if (xTaskGetCurrentTaskHandleForCore(1) == task && ctx->shared.counter != 0) {
            source_seen = true;
            break;
        }
    }
    s_migration_observed = false;
    BaseType_t affinity_before = xTaskGetCoreID(task);
    TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCore(migration_observer, "observe", TEST_TASK_STACK,
                                                      task, TEST_MAIN_TASK_PRIO + 1, NULL, 1));
    vTaskDelay(pdMS_TO_TICKS(1100));
    BaseType_t affinity_after = xTaskGetCoreID(task);
    stop_privilege_count_task(ctx);
    printf("U-mode migration trace, core1=%d core0=%d affinity=%d/%d\n",
           source_seen, s_migration_observed, (int)affinity_before, (int)affinity_after);
    TEST_ASSERT_TRUE(source_seen);
    TEST_ASSERT_TRUE(s_migration_observed);
    TEST_ASSERT_EQUAL(tskNO_AFFINITY, affinity_before);
    TEST_ASSERT_EQUAL(affinity_before, affinity_after);
}
#endif /* !CONFIG_FREERTOS_UNICORE */

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
static bool stack_guard_is_monitoring(void)
{
    portDISABLE_INTERRUPTS();
    uint32_t monitor = REG_READ(esp_cpu_get_core_id() ? ASSIST_DEBUG_CORE_1_MONITOR_REG : ASSIST_DEBUG_CORE_0_MONITOR_REG);
    portENABLE_INTERRUPTS();
    return (monitor & ASSIST_DEBUG_SP_SPILL_BITS) == ASSIST_DEBUG_SP_SPILL_BITS;
}

typedef struct {
    bool monitoring[2];
    esp_err_t err;
    int ret;
    SemaphoreHandle_t done;
} stack_guard_check_t;

static void stack_guard_check_task(void *arg)
{
    static uint8_t stack[portNUM_PROCESSORS][USER_STACK_SIZE] __attribute__((aligned(16)));
    stack_guard_check_t *check = arg;
    uint8_t *user_stack = stack[esp_cpu_get_core_id()];
    privilege_shared_t shared = { 0 };

    check->monitoring[0] = stack_guard_is_monitoring();
    check->err = test_privilege_call(privilege_arith, 7, (uint32_t)&shared, user_stack, USER_STACK_SIZE, &check->ret);
    check->monitoring[1] = stack_guard_is_monitoring();
    xSemaphoreGive(check->done);
    vTaskDelete(NULL);
}

TEST_CASE("Hardware stack guard monitors M-mode code again after U-mode code", "[privilege]")
{
    for (int core = 0; core < portNUM_PROCESSORS; core++) {
        stack_guard_check_t check = { .done = xSemaphoreCreateBinary() };
        TEST_ASSERT_NOT_NULL(check.done);
        TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCore(stack_guard_check_task, "guard", TEST_TASK_STACK,
                                                          &check, TEST_TASK_PRIO, NULL, core));
        TEST_ASSERT_TRUE(xSemaphoreTake(check.done, pdMS_TO_TICKS(1000)));
        vSemaphoreDelete(check.done);
        TEST_ASSERT_EQUAL(ESP_OK, check.err);
        TEST_ASSERT_EQUAL(8, check.ret);
        TEST_ASSERT_TRUE(check.monitoring[0]);
        TEST_ASSERT_TRUE(check.monitoring[1]);
    }
}
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

TEST_CASE("M-mode fault with nonzero scratch retains the machine stack", "[privilege_panic][ignore]")
{
    /* mscratch holds a stale value, as it does while U-mode code runs. A trap from M-mode must
     * keep the M-mode stack. */
    __asm__ volatile("csrci mstatus, 8; li t0, 0x1234; csrw mscratch, t0; .word 0" ::: "t0", "memory");
}
