/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "soc/soc_caps.h"

#if CONFIG_IDF_TARGET_ARCH_RISCV && !SOC_INT_CLIC_SUPPORTED

#include "esp_attr.h"
#include "esp_cpu.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "riscv/rv_utils.h"
#include "soc/system_intr.h"
#include "unity.h"

#if SOC_INT_PLIC_SUPPORTED
#include "soc/intpri_reg.h"
#define TEST_LOW_INTR_REG    INTPRI_CPU_INTR_FROM_CPU_2_REG
#define TEST_HIGH_INTR_REG   INTPRI_CPU_INTR_FROM_CPU_3_REG
#else
#include "soc/system_reg.h"
#define TEST_LOW_INTR_REG    SYSTEM_CPU_INTR_FROM_CPU_2_REG
#define TEST_HIGH_INTR_REG   SYSTEM_CPU_INTR_FROM_CPU_3_REG
#endif

static volatile unsigned s_low_count;
static volatile unsigned s_high_count;
static volatile bool s_masked;
static volatile bool s_low_active;
static volatile bool s_task_ran_in_isr;
static volatile bool s_nested_seen;
static volatile uint32_t s_task_threshold;
static TaskHandle_t s_waiter;

/* Software requests make the nesting and yield paths deterministic. This does not reproduce
 * the timing-dependent threshold-write issue reported with asynchronous peripheral interrupts. */
static bool IRAM_ATTR wait_for_interrupt(volatile unsigned *count, unsigned expected)
{
    const uint32_t start = esp_cpu_get_cycle_count();
    while (*count != expected && esp_cpu_get_cycle_count() - start < 100000) {}
    return *count == expected;
}

static void IRAM_ATTR high_priority_isr(void *arg)
{
    REG_WRITE(TEST_HIGH_INTR_REG, 0);
    if (s_waiter != NULL) {
        s_nested_seen = s_low_active;
    } else {
        REG_WRITE(TEST_LOW_INTR_REG, 1);
        const uint32_t start = esp_cpu_get_cycle_count();
        while (esp_cpu_get_cycle_count() - start < 1000) {}
    }
    s_high_count++;
}

static void IRAM_ATTR low_priority_isr(void *arg)
{
    REG_WRITE(TEST_LOW_INTR_REG, 0);
    s_low_count++;
    if (!s_masked && s_waiter != NULL) {
        s_low_active = true;
        BaseType_t woken = pdFALSE;
        vTaskNotifyGiveFromISR(s_waiter, &woken);
        portYIELD_FROM_ISR(woken);
        REG_WRITE(TEST_HIGH_INTR_REG, 1);
        wait_for_interrupt(&s_high_count, 2);
        s_low_active = false;
    }
}

static void threshold_waiter(void *arg)
{
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    s_task_threshold = REG_READ(INTERRUPT_CURRENT_CORE_INT_THRESH_REG);
    rv_utils_restore_intlevel_regval(RVHAL_INTR_ENABLE_THRESH);
    asm volatile("fence" ::: "memory");
    s_task_ran_in_isr = s_low_active;
    xTaskNotifyGive((TaskHandle_t)arg);
    vTaskSuspend(NULL);
}

TEST_CASE("RISC-V interrupt mask survives high-priority and nested interrupts", "[freertos]")
{
    intr_handle_t low_handle = NULL;
    intr_handle_t high_handle = NULL;
    s_low_count = s_high_count = 0;
    s_masked = s_low_active = s_task_ran_in_isr = s_nested_seen = false;
    s_task_threshold = UINT32_MAX;
    s_waiter = NULL;
    ulTaskNotifyTake(pdTRUE, 0);
    REG_WRITE(TEST_LOW_INTR_REG, 0);
    REG_WRITE(TEST_HIGH_INTR_REG, 0);

    TEST_ESP_OK(esp_intr_alloc(SYS_CPU_INTR_FROM_CPU_2_SOURCE, ESP_INTR_FLAG_LEVEL1 | ESP_INTR_FLAG_IRAM, low_priority_isr, NULL, &low_handle));
    esp_err_t err = esp_intr_alloc(SYS_CPU_INTR_FROM_CPU_3_SOURCE,
                                   ESP_INTR_FLAG_LEVEL3 | ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_INTRDISABLED,
                                   high_priority_isr, NULL, &high_handle);
    if (err != ESP_OK) {
        esp_intr_free(low_handle);
    }
    TEST_ESP_OK(err);
    /* Allocate at supported level 3, then raise the CPU interrupt priority to 5. */
    esp_cpu_intr_set_priority(esp_intr_get_intno(high_handle), 5);
    TEST_ESP_OK(esp_intr_enable(high_handle));

    /* 1. Priority 5 runs under the mask; its priority-1 request must wait until unmasking. */
    s_masked = true;
    const UBaseType_t old_threshold = portSET_INTERRUPT_MASK_FROM_ISR();
    REG_WRITE(TEST_HIGH_INTR_REG, 1);
    const bool high_ran = wait_for_interrupt(&s_high_count, 1);
    const unsigned low_while_masked = s_low_count;
    const uint32_t masked_threshold = REG_READ(INTERRUPT_CURRENT_CORE_INT_THRESH_REG);
    s_masked = false;
    portCLEAR_INTERRUPT_MASK_FROM_ISR(old_threshold);
    const bool low_ran = wait_for_interrupt(&s_low_count, 1);

    /* 2. Priority 1 wakes a task, then nests priority 5. The task must run after both ISRs exit. */
    const BaseType_t created = xTaskCreate(threshold_waiter, "threshold", 2048, xTaskGetCurrentTaskHandle(),
                                           uxTaskPriorityGet(NULL) + 1, &s_waiter);
    bool switched = false;
    if (created == pdPASS) {
        REG_WRITE(TEST_LOW_INTR_REG, 1);
        switched = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000)) == 1;
        vTaskDelete(s_waiter);
    }

    esp_intr_disable(high_handle);
    esp_intr_disable(low_handle);
    REG_WRITE(TEST_LOW_INTR_REG, 0);
    REG_WRITE(TEST_HIGH_INTR_REG, 0);
    esp_cpu_intr_set_priority(esp_intr_get_intno(high_handle), 3);
    TEST_ESP_OK(esp_intr_free(high_handle));
    TEST_ESP_OK(esp_intr_free(low_handle));

    TEST_ASSERT_TRUE(high_ran);
    TEST_ASSERT_EQUAL(0, low_while_masked);
    TEST_ASSERT_EQUAL(RVHAL_EXCM_LEVEL, masked_threshold);
    TEST_ASSERT_TRUE(low_ran);
    TEST_ASSERT_EQUAL(pdPASS, created);
    TEST_ASSERT_TRUE(switched);
    TEST_ASSERT_TRUE(s_nested_seen);
    TEST_ASSERT_FALSE(s_task_ran_in_isr);
    TEST_ASSERT_EQUAL(RVHAL_INTR_ENABLE_THRESH, s_task_threshold);
    TEST_ASSERT_EQUAL(2, s_low_count);
    TEST_ASSERT_EQUAL(2, s_high_count);
}

#endif // CONFIG_IDF_TARGET_ARCH_RISCV && !SOC_INT_CLIC_SUPPORTED
