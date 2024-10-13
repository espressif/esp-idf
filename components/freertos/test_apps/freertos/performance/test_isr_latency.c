/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_intr_alloc.h"
#include "esp_cpu.h"
#include "unity.h"
#include "test_utils.h"
#if CONFIG_IDF_TARGET_ARCH_XTENSA
#include "xtensa/hal.h"
#include "xtensa_api.h"     // Replace with interrupt allocator API (IDF-3891)
#define TEST_SET_INT_MASK(mask) xt_set_intset(mask)
#define TEST_CLR_INT_MASK(mask) xt_set_intclear(mask)
#elif CONFIG_IDF_TARGET_ARCH_RISCV
#include "riscv/interrupt.h"
#define TEST_SET_INT_MASK(mask) esprv_int_enable(mask)
#define TEST_CLR_INT_MASK(mask) esprv_int_disable(mask)
#endif

#ifndef __riscv // TODO: IDF-4416

#define SW_ISR_LEVEL_1  7

static SemaphoreHandle_t sync;
static SemaphoreHandle_t end_sema;
static uint32_t cycle_before_trigger;
static uint32_t cycle_before_exit;
static uint32_t delta_enter_cycles = 0;
static uint32_t delta_exit_cycles = 0;

static void software_isr_using_parameter_vportyield(void *arg)
{
    (void)arg;
    BaseType_t yield;
    delta_enter_cycles += esp_cpu_get_cycle_count() - cycle_before_trigger;
    TEST_CLR_INT_MASK(1 << SW_ISR_LEVEL_1);

    xSemaphoreGiveFromISR(sync, &yield);
    portYIELD_FROM_ISR(yield);

    cycle_before_exit = esp_cpu_get_cycle_count();
}

static void software_isr_using_no_argument_vportyield(void *arg)
{
    (void)arg;
    BaseType_t yield;
    delta_enter_cycles += esp_cpu_get_cycle_count() - cycle_before_trigger;

    TEST_CLR_INT_MASK(1 << SW_ISR_LEVEL_1);

    xSemaphoreGiveFromISR(sync, &yield);
    if (yield) {
        portYIELD_FROM_ISR();
    }
    cycle_before_exit = esp_cpu_get_cycle_count();
}

static void test_task(void *arg)
{
    (void) arg;

    for (int i = 0; i < 10000; i++) {
        cycle_before_trigger = esp_cpu_get_cycle_count();
        TEST_SET_INT_MASK(1 << SW_ISR_LEVEL_1);
        xSemaphoreTake(sync, portMAX_DELAY);
        delta_exit_cycles += esp_cpu_get_cycle_count() - cycle_before_exit;
    }

    delta_enter_cycles /= 10000;
    delta_exit_cycles /= 10000;

    xSemaphoreGive(end_sema);
    vTaskDelete(NULL);
}

TEST_CASE("isr latency test vport-yield-from-isr with no parameter", "[freertos] [ignore]")
{
    intr_handle_t handle;
    esp_err_t err = esp_intr_alloc(ETS_INTERNAL_SW0_INTR_SOURCE, ESP_INTR_FLAG_LEVEL1, &software_isr_using_no_argument_vportyield, NULL, &handle);
    TEST_ASSERT_EQUAL_HEX32(ESP_OK, err);

    sync = xSemaphoreCreateBinary();
    TEST_ASSERT(sync != NULL);
    end_sema = xSemaphoreCreateBinary();
    TEST_ASSERT(end_sema != NULL);
    xTaskCreatePinnedToCore(test_task, "tst", 4096, NULL, configMAX_PRIORITIES - 1, NULL, 0);
    vTaskDelay(100);
    BaseType_t result = xSemaphoreTake(end_sema, portMAX_DELAY);
    TEST_ASSERT_EQUAL_HEX32(pdTRUE, result);
    TEST_PERFORMANCE_LESS_THAN(ISR_ENTER_CYCLES, "%"PRIu32" cycles", delta_enter_cycles);
    TEST_PERFORMANCE_LESS_THAN(ISR_EXIT_CYCLES, "%"PRIu32" cycles", delta_exit_cycles);

    esp_intr_free(handle);
}

TEST_CASE("isr latency test vport-yield-from-isr with parameter", "[freertos][ignore]")
{
    intr_handle_t handle;
    esp_err_t err = esp_intr_alloc(ETS_INTERNAL_SW0_INTR_SOURCE, ESP_INTR_FLAG_LEVEL1, &software_isr_using_parameter_vportyield, NULL, &handle);
    TEST_ASSERT_EQUAL_HEX32(ESP_OK, err);

    sync = xSemaphoreCreateBinary();
    TEST_ASSERT(sync != NULL);
    end_sema = xSemaphoreCreateBinary();
    TEST_ASSERT(end_sema != NULL);
    xTaskCreatePinnedToCore(test_task, "tst", 4096, NULL, configMAX_PRIORITIES - 1, NULL, 0);
    BaseType_t result = xSemaphoreTake(end_sema, portMAX_DELAY);
    TEST_ASSERT_EQUAL_HEX32(pdTRUE, result);
    TEST_PERFORMANCE_LESS_THAN(ISR_ENTER_CYCLES, "%"PRIu32" cycles", delta_enter_cycles);
    TEST_PERFORMANCE_LESS_THAN(ISR_EXIT_CYCLES, "%"PRIu32" cycles", delta_exit_cycles);

    esp_intr_free(handle);
}

#endif // __riscv
