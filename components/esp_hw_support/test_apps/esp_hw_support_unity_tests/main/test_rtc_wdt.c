/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "unity.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/rtc.h"
#include "soc/interrupts.h"
#include "hal/wdt_hal.h"
#include "hal/wdt_types.h"
#include "esp_timer.h"
#include "esp_system.h"
#include "esp_intr_alloc.h"
#include "esp_private/rtc_ctrl.h"
#include "esp_private/esp_sleep_internal.h"

/* As the RTC slow clock used by RWDT is derived using RC oscillator by default,
 * the timing accuracy is not very precise, and may vary from chip to chip.
 * So, using some margin for the timeout checks.
 */
#define WDT_TIMEOUT_MARGIN_MS(X) (X * 30 / 100)  // 30% margin

static const uint32_t wdt_stage_timeout_ms[] = {
    100,  // Stage-0 timeout in ms
    500,  // Stage-1 timeout in ms
    1000, // Stage-2 timeout in ms
    3000  // Stage-3 timeout in ms
};
static volatile bool wdt_int_triggered = false;
static int64_t wdt_start_time[4];
static int64_t wdt_stage_int_time[4];
static wdt_hal_context_t rtc_wdt_ctx;

static void IRAM_ATTR rtc_wdt_isr(void *arg)
{
    static wdt_stage_t wdt_intr_stage = WDT_STAGE0;
    wdt_int_triggered = true;
    wdt_stage_int_time[wdt_intr_stage] = esp_timer_get_time();
    /* Only clear the interrupt status, but don't feed the WDT yet, this will trigger next stage interrupt */
    rwdt_ll_clear_intr_status(rtc_wdt_ctx.rwdt_dev);

    /* Keep track of wdt interrupt stages */
    wdt_intr_stage++;

    /* Handle the interrupt and disable rtc wdt after final stage, as maximum 4 stages are supported */
    if (wdt_intr_stage > WDT_STAGE3) {
        wdt_intr_stage = WDT_STAGE0;
        wdt_hal_write_protect_disable(&rtc_wdt_ctx);
        wdt_hal_handle_intr(&rtc_wdt_ctx);
        wdt_hal_disable(&rtc_wdt_ctx);
        wdt_hal_write_protect_enable(&rtc_wdt_ctx);
    }
}

static void wait_for_interrupt(int wait_ms)
{
    /* Wait for interrupt to trigger or timeout using esp_timer_get_time for better accuracy */
    int64_t start_us = esp_timer_get_time();
    int64_t timeout_us = wait_ms * 1000;
    while (!wdt_int_triggered && (esp_timer_get_time() - start_us) < timeout_us) {
        /* Yield to other tasks */
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

TEST_CASE("RTC WDT triggers interrupt at expected time for all stages", "[rtc_wdt][intr]")
{
    uint32_t stage_timeout_ticks[4];
    wdt_stage_t stage = WDT_STAGE0;

    /* Convert stage timeouts from ms to ticks */
    for (stage = WDT_STAGE0; stage <= WDT_STAGE3; stage++) {
        stage_timeout_ticks[stage] = (uint32_t)(wdt_stage_timeout_ms[stage] * rtc_clk_slow_freq_get_hz() / 1000ULL);
    }

    /* Print timeout and clock info */
    printf("RTC WDT test: \n");
    for (stage = WDT_STAGE0; stage <= WDT_STAGE3; stage++) {
        printf("Stage-%d timeout %lu ticks (%lu ms),\n", stage, stage_timeout_ticks[stage], wdt_stage_timeout_ms[stage]);
    }
    printf("RTC slow clock %lu Hz\n", rtc_clk_slow_freq_get_hz());

    /* Allocate interrupt for RTC WDT */
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32C3
    esp_err_t err = rtc_isr_register(rtc_wdt_isr, NULL, RTC_CNTL_WDT_INT_ST_M, RTC_INTR_FLAG_IRAM);
#else
    intr_handle_t ret_handle;
    esp_err_t err = esp_intr_alloc(ETS_LP_WDT_INTR_SOURCE, 0, rtc_wdt_isr, NULL, &ret_handle);
#endif
    TEST_ASSERT_EQUAL(ESP_OK, err);

    /* Setup RTC WDT to trigger interrupt upon timeout */
    wdt_hal_init(&rtc_wdt_ctx, WDT_RWDT, 0, true);
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    for (stage = WDT_STAGE0; stage <= WDT_STAGE3; stage++) {
        wdt_hal_config_stage(&rtc_wdt_ctx, stage, stage_timeout_ticks[stage], WDT_STAGE_ACTION_INT);
    }
    wdt_hal_enable(&rtc_wdt_ctx);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);
    TEST_ASSERT_TRUE(wdt_hal_is_enabled(&rtc_wdt_ctx));

    printf("RTC WDT setup complete, waiting for interrupt...\n");

    /* Feed the watchdog at the start */
    wdt_hal_feed(&rtc_wdt_ctx);

    /* Wait for interrupt after each stage of WDT timeout */
    for (stage = WDT_STAGE0; stage <= WDT_STAGE3; stage++) {
        /* Mark the start time afresh for each stage */
        wdt_start_time[stage] = esp_timer_get_time();
        wdt_int_triggered = false;
        wait_for_interrupt(wdt_stage_timeout_ms[stage] * 3); // Wait for three times the stage timeout at max
        printf("Stage-%d interrupt received for RTC WDT\n", stage);
        TEST_ASSERT_TRUE_MESSAGE(wdt_int_triggered, "RTC WDT interrupt did not trigger!");
    }

    /* Deregister ISR as interrupt is received or wait time is over */
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32C3
    rtc_isr_deregister(rtc_wdt_isr, NULL);
#else
    esp_intr_free(ret_handle);
#endif

    /* Check that wdt interrupts for various stages occurred within a reasonable window (WDT_TIMEOUT_MARGIN_MS) */
    int64_t elapsed_us[4], expected_us[4];
    for (stage = WDT_STAGE0; stage <= WDT_STAGE3; stage++) {
        elapsed_us[stage] = wdt_stage_int_time[stage] - wdt_start_time[stage];
        expected_us[stage] = wdt_stage_timeout_ms[stage] * 1000;
        printf("Stage-%d: interrupt time %lld us, start time %lld us\n", stage, wdt_stage_int_time[stage], wdt_start_time[stage]);
        printf("Stage-%d: expected %lld us, elapsed %lld us\n", stage, expected_us[stage], elapsed_us[stage]);
        TEST_ASSERT_INT_WITHIN(WDT_TIMEOUT_MARGIN_MS(wdt_stage_timeout_ms[stage]) * 1000, expected_us[stage], elapsed_us[stage]);
    }
}

/* MULTI test case for RTC WDT interrupt then reset */
static void rtc_wdt_setup_then_reset_SYSTEM(void)
{
    uint32_t stage_timeout_ticks = (uint32_t)(3000 * rtc_clk_slow_freq_get_hz() / 1000ULL);   /* 3000ms for interrupt */

    /* Setup RTC WDT */
    wdt_hal_init(&rtc_wdt_ctx, WDT_RWDT, 0, true);
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE0, stage_timeout_ticks, WDT_STAGE_ACTION_RESET_SYSTEM);
    wdt_hal_enable(&rtc_wdt_ctx);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);
    TEST_ASSERT_TRUE(wdt_hal_is_enabled(&rtc_wdt_ctx));
    printf("RTC WDT setup stage-0 RESET (3000ms)\n");

    /* Wait for reset: after stage0 timeout, the CPU will reset */
    printf("Waiting for RTC WDT to reset the main system...\n");
    while (1) {
        vTaskDelay(100);
    }
}

/* MULTI test case for RTC WDT interrupt then reset */
static void rtc_wdt_setup_then_reset_RTC(void)
{
    uint32_t stage_timeout_ticks = (uint32_t)(3000 * rtc_clk_slow_freq_get_hz() / 1000ULL);   /* 3000ms for interrupt */

    /* Setup RTC WDT */
    wdt_hal_init(&rtc_wdt_ctx, WDT_RWDT, 0, true);
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE0, stage_timeout_ticks, WDT_STAGE_ACTION_RESET_RTC);
    wdt_hal_enable(&rtc_wdt_ctx);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);
    TEST_ASSERT_TRUE(wdt_hal_is_enabled(&rtc_wdt_ctx));
    printf("RTC WDT setup stage-0 RESET (3000ms)\n");

    /* Wait for reset: after stage0 timeout, the CPU will reset */
    printf("Waiting for RTC WDT to reset the main system and RTC...\n");
    while (1) {
        vTaskDelay(100);
    }
}

#if SOC_LIGHT_SLEEP_SUPPORTED && CONFIG_ESP_SLEEP_ENABLE_RTC_WDT_IN_SLEEP
/* MULTI test case for light sleep prepare timeout then RTC WDT resets RTC */
static void rtc_wdt_light_sleep_prepare_timeout(void)
{
    uint32_t stage_timeout_ticks = (uint32_t)(1000 * rtc_clk_slow_freq_get_hz() / 1000ULL);   /* 1000ms for interrupt */
    esp_sleep_enable_timer_wakeup(3000000);

    /* Setup RTC WDT */
    // Here enable RTC_WDT then the setup in sleep flow will be skipped
    wdt_hal_init(&rtc_wdt_ctx, WDT_RWDT, 0, true);
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE0, stage_timeout_ticks, WDT_STAGE_ACTION_RESET_RTC);
    wdt_hal_enable(&rtc_wdt_ctx);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);
    TEST_ASSERT_TRUE(wdt_hal_is_enabled(&rtc_wdt_ctx));
    printf("RTC WDT setup stage-0 RESET (1000ms)\n");

    esp_light_sleep_start();
    /* Wait for RTC WDT to reset the main system and RTC */
    esp_restart();
}
#endif

#if SOC_DEEP_SLEEP_SUPPORTED && CONFIG_ESP_SLEEP_ENABLE_RTC_WDT_IN_SLEEP
/* MULTI test case for deep sleep prepare timeout then RTC WDT resets RTC */
static void rtc_wdt_deep_sleep_prepare_timeout(void)
{
    uint32_t stage_timeout_ticks = (uint32_t)(1000 * rtc_clk_slow_freq_get_hz() / 1000ULL);   /* 1000ms for interrupt */
    esp_sleep_enable_timer_wakeup(3000000);

    /* Setup RTC WDT */
    // Here enable RTC_WDT then the setup in sleep flow will be skipped
    wdt_hal_init(&rtc_wdt_ctx, WDT_RWDT, 0, true);
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE0, stage_timeout_ticks, WDT_STAGE_ACTION_RESET_RTC);
    wdt_hal_enable(&rtc_wdt_ctx);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);
    TEST_ASSERT_TRUE(wdt_hal_is_enabled(&rtc_wdt_ctx));
    printf("RTC WDT setup stage-0 RESET (1000ms)\n");

    esp_deep_sleep_start();
}
#endif

static void rtc_wdt_verify_SYSTEM_reset(void)
{
    printf("Confirming if reset reason matches 0x9 (main system reset)\n");
#if CONFIG_IDF_TARGET_ESP32P4
    TEST_ASSERT_EQUAL(RESET_REASON_CORE_RWDT, esp_rom_get_reset_reason(CONFIG_ESP_MAIN_TASK_AFFINITY));
#else
    TEST_ASSERT_EQUAL(RESET_REASON_CORE_RTC_WDT, esp_rom_get_reset_reason(CONFIG_ESP_MAIN_TASK_AFFINITY));
#endif
}

static void rtc_wdt_verify_RTC_reset(void)
{
    printf("Confirming if reset reason matches 0x10 (main system + RTC reset)\n");
#if CONFIG_IDF_TARGET_ESP32P4
    TEST_ASSERT_EQUAL(RESET_REASON_SYS_RWDT, esp_rom_get_reset_reason(CONFIG_ESP_MAIN_TASK_AFFINITY));
#else
    TEST_ASSERT_EQUAL(RESET_REASON_SYS_RTC_WDT, esp_rom_get_reset_reason(CONFIG_ESP_MAIN_TASK_AFFINITY));
#endif
}

TEST_CASE_MULTIPLE_STAGES(
    "RTC WDT resets SYSTEM and verifies reset reason",
    "[rtc_wdt][intr][reset]",
    rtc_wdt_setup_then_reset_SYSTEM,
    rtc_wdt_verify_SYSTEM_reset
)

TEST_CASE_MULTIPLE_STAGES(
    "RTC WDT resets RTC and verifies reset reason",
    "[rtc_wdt][intr][reset]",
    rtc_wdt_setup_then_reset_RTC,
    rtc_wdt_verify_RTC_reset
)

#if SOC_LIGHT_SLEEP_SUPPORTED && CONFIG_ESP_SLEEP_ENABLE_RTC_WDT_IN_SLEEP
TEST_CASE_MULTIPLE_STAGES(
    "Light sleep prepare timeout then RTC WDT resets RTC",
    "[rtc_wdt][lightsleep][reset]",
    rtc_wdt_light_sleep_prepare_timeout,
    rtc_wdt_verify_RTC_reset
)
#endif

#if SOC_DEEP_SLEEP_SUPPORTED && CONFIG_ESP_SLEEP_ENABLE_RTC_WDT_IN_SLEEP
TEST_CASE_MULTIPLE_STAGES(
    "Deep sleep prepare timeout then RTC WDT resets RTC",
    "[rtc_wdt][deepsleep][reset]",
    rtc_wdt_deep_sleep_prepare_timeout,
    rtc_wdt_verify_RTC_reset
)
#endif
