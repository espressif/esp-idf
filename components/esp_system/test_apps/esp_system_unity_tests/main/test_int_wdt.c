/*
 * SPDX-FileCopyrightText: 2026 LeonB
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_chip_info.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "esp_private/esp_sleep_internal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_private/eco3_livelock_workaround.h"
#include "esp_private/esp_int_wdt.h"
#include "soc/soc.h"
#include "unity.h"
#include "test_utils.h"

#if CONFIG_IDF_TARGET_ESP32 && CONFIG_ESP_INT_WDT && CONFIG_ESP32_ECO3_CACHE_LOCK_FIX

#include "soc/timer_group_reg.h"

#define IWDT_TEST_ITERATIONS 32
#define IWDT_SLEEP_ITERATIONS 100
#define IWDT_SLEEP_MAX_ATTEMPTS 1000

extern volatile uint32_t _lx_intr_livelock_max;

typedef struct {
    SemaphoreHandle_t done;
    bool sleep;
    bool passed;
    esp_err_t sleep_result;
    int attempts;
    int successes;
    int rejections;
    uint32_t recovery_budget;
    uint32_t stage0_ticks;
    uint32_t stage1_ticks;
} iwdt_test_context_t;

static bool check_iwdt_mode(iwdt_test_context_t *context, bool enabled)
{
    // In the disabled mode no recovery handler should rewrite these registers.
    context->recovery_budget = _lx_intr_livelock_max;
    context->stage0_ticks = REG_READ(TIMG_WDTCONFIG2_REG(1));
    context->stage1_ticks = REG_READ(TIMG_WDTCONFIG3_REG(1));
    uint32_t expected_budget = enabled ? CONFIG_ESP_INT_WDT_TIMEOUT_MS / 20 - 1 : 0;
    return context->recovery_budget == expected_budget
           && context->stage0_ticks == IWDT_STAGE0_TIMEOUT_US / (expected_budget + 1)
           && context->stage1_ticks == IWDT_STAGE1_TIMEOUT_US;
}

static void test_iwdt_on_core(void *arg)
{
    iwdt_test_context_t *context = arg;
    context->passed = true;
    context->sleep_result = ESP_OK;
    context->attempts = 0;
    context->successes = 0;
    context->rejections = 0;
    int iterations = context->sleep ? IWDT_SLEEP_ITERATIONS : IWDT_TEST_ITERATIONS;
    int max_attempts = context->sleep ? IWDT_SLEEP_MAX_ATTEMPTS : IWDT_TEST_ITERATIONS;
    while (context->attempts < max_attempts && context->successes < iterations) {
        ++context->attempts;
        if (context->sleep) {
            context->sleep_result = esp_light_sleep_start();
            // Sleep rejection can follow workaround disable; verify that rollback restored the enabled mode.
            context->passed = check_iwdt_mode(context, true);
            if (context->sleep_result == ESP_OK) {
                ++context->successes;
            } else if (context->sleep_result == ESP_ERR_SLEEP_REJECT) {
                ++context->rejections;
            } else {
                context->passed = false;
            }
        } else {
            esp_int_wdt_livelock_workaround(false, 0);
            context->passed = check_iwdt_mode(context, false);
            // Allow ticks on both CPUs, including the CPU0 watchdog feed hook.
            vTaskDelay(3);
            context->passed = check_iwdt_mode(context, false) && context->passed;
            esp_int_wdt_livelock_workaround(true, 0);
            context->passed = check_iwdt_mode(context, true) && context->passed;
            ++context->successes;
        }
        if (!context->passed) {
            break;
        }
        vTaskDelay(1);
    }
    context->passed = context->passed && context->successes == iterations;
    // Restore the mode before reporting a failed check to Unity.
    esp_int_wdt_livelock_workaround(true, 0);
    xSemaphoreGive(context->done);
    vTaskSuspend(NULL);
}

static void run_iwdt_test(bool sleep)
{
    if (!soc_has_cache_lock_bug()) {
        TEST_IGNORE_MESSAGE("Requires an ESP32 revision with the cache livelock workaround");
    }
    if (sleep) {
        TEST_ASSERT_EQUAL(ESP_OK, esp_sleep_enable_timer_wakeup(20000));
    }
    iwdt_test_context_t context = {
        .done = xSemaphoreCreateBinary(),
        .sleep = sleep,
    };
    TEST_ASSERT_NOT_NULL(context.done);
    for (int core = 0; core < CONFIG_FREERTOS_NUMBER_OF_CORES; ++core) {
        TaskHandle_t task = NULL;
        BaseType_t created = xTaskCreatePinnedToCore(test_iwdt_on_core, "iwdt_test", 4096,
                                                     &context, UNITY_FREERTOS_PRIORITY + 1, &task, core);
        if (created != pdPASS) {
            vSemaphoreDelete(context.done);
            if (sleep) {
                esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
            }
            TEST_FAIL_MESSAGE("Failed to create IWDT test task");
        }
        BaseType_t completed = xSemaphoreTake(context.done, portMAX_DELAY);
        // The other core may still be returning from xSemaphoreGive. Wait
        // until the worker is suspended so deletion reclaims it immediately.
        while (eTaskGetState(task) != eSuspended) {
            vTaskDelay(1);
        }
        vTaskDelete(task);
        if (sleep) {
            printf("core=%d sleep attempts=%d successes=%d rejections=%d\n",
                   core, context.attempts, context.successes, context.rejections);
        }
        if (completed != pdTRUE || !context.passed) {
            esp_int_wdt_livelock_workaround(true, 0);
            vSemaphoreDelete(context.done);
            if (sleep) {
                esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
            }
            printf("core=%d sleep_result=%d max=%" PRIu32 " stage0=%" PRIu32 " stage1=%" PRIu32 "\n",
                   core, context.sleep_result, context.recovery_budget, context.stage0_ticks, context.stage1_ticks);
            TEST_ASSERT_EQUAL_MESSAGE(pdTRUE, completed, "IWDT test task timed out");
            TEST_FAIL_MESSAGE("Unexpected IWDT mode or sleep result");
        }
    }
    vSemaphoreDelete(context.done);
    if (sleep) {
        TEST_ASSERT_EQUAL(ESP_OK, esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER));
    }
}

TEST_CASE("interrupt watchdog mode survives tick feeds on both cores", "[int_wdt]")
{
    run_iwdt_test(false);
}

TEST_CASE("interrupt watchdog mode survives light sleep on both cores", "[int_wdt][lightsleep]")
{
    run_iwdt_test(true);
}

#if !CONFIG_FREERTOS_UNICORE
TEST_CASE("expired interrupt watchdog deadline rejects light sleep", "[int_wdt][lightsleep]")
{
    if (!soc_has_cache_lock_bug()) {
        TEST_IGNORE_MESSAGE("Requires an ESP32 revision with the cache livelock workaround");
    }
    iwdt_test_context_t context = { 0 };
    TEST_ASSERT_TRUE(check_iwdt_mode(&context, true));
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(20000));
    esp_err_t result = esp_light_sleep_start_with_iwdt_deadline(esp_timer_get_time());
    TEST_ESP_OK(esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER));

    TEST_ASSERT_EQUAL(ESP_ERR_SLEEP_REJECT, result);
    TEST_ASSERT_TRUE(check_iwdt_mode(&context, true));
}
#endif

#endif // CONFIG_IDF_TARGET_ESP32 && CONFIG_ESP_INT_WDT && CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
