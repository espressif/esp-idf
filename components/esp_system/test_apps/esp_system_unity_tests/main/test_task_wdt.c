/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "sdkconfig.h"

#if CONFIG_ESP_TASK_WDT_EN

#include <stdbool.h>
#include "unity.h"
#include "esp_rom_sys.h"
#include "esp_task_wdt.h"
#include "test_utils.h"
#include "soc/rtc.h"
#include "soc/soc_caps.h"

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_MWDT_SUPPORT_SLEEP_RETENTION
#include "esp_sleep.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/esp_pmu.h"
#endif

#define TASK_WDT_TIMEOUT_MS     1000

static volatile bool timeout_flag;

void esp_task_wdt_isr_user_handler(void)
{
    timeout_flag = true;
}

TEST_CASE("Task WDT task timeout", "[task_wdt]")
{
    timeout_flag = false;
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = TASK_WDT_TIMEOUT_MS,
        .idle_core_mask = 0,
        .trigger_panic = false,
    };
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_init(&twdt_config));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_add(NULL));
    // Short delay to allow timeout to occur
    esp_rom_delay_us(TASK_WDT_TIMEOUT_MS * 1000);
    TEST_ASSERT_EQUAL(true, timeout_flag);
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_delete(NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_deinit());
}

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_MWDT_SUPPORT_SLEEP_RETENTION && SOC_LIGHT_SLEEP_SUPPORTED
TEST_CASE("Task WDT task timeout after peripheral powerdown lightsleep", "[task_wdt]")
{
    timeout_flag = false;
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = TASK_WDT_TIMEOUT_MS,
        .idle_core_mask = 0,
        .trigger_panic = false,
    };
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_init(&twdt_config));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_add(NULL));

    TEST_ESP_OK(sleep_cpu_configure(true));
    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);
    esp_sleep_enable_timer_wakeup(10 * 1000);

    esp_light_sleep_start();

    TEST_ASSERT_EQUAL(PMU_SLEEP_PD_TOP, sleep_ctx.sleep_flags & PMU_SLEEP_PD_TOP);
    TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_request_result);
    esp_sleep_set_sleep_context(NULL);

    // Short delay to allow timeout to occur
    esp_rom_delay_us(TASK_WDT_TIMEOUT_MS * 1000);
    TEST_ASSERT_EQUAL(true, timeout_flag);
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_delete(NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_deinit());
    TEST_ESP_OK(sleep_cpu_configure(false));
}
#endif

#if SOC_MWDT_SUPPORT_XTAL

#if CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32H21
#define TEST_CPU_FREQUENCY_MHZ 48
#else
#define TEST_CPU_FREQUENCY_MHZ 40
#endif

TEST_CASE("Task WDT task timeout - CPU Frequency changed", "[task_wdt]")
{
    rtc_cpu_freq_config_t old_config, new_config;
    rtc_clk_cpu_freq_get_config(&old_config);

    TEST_ASSERT(rtc_clk_cpu_freq_mhz_to_config(TEST_CPU_FREQUENCY_MHZ, &new_config));
    rtc_clk_cpu_freq_set_config(&new_config);

    timeout_flag = false;
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = TASK_WDT_TIMEOUT_MS,
        .idle_core_mask = 0,
        .trigger_panic = false,
    };
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_init(&twdt_config));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_add(NULL));
    /* Short delay to allow timeout to occur, if WDT depends on any of the clocks changed
       then the timeout should be slower and test will fail */
    esp_rom_delay_us(TASK_WDT_TIMEOUT_MS * 1000);
    TEST_ASSERT_EQUAL(true, timeout_flag);
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_delete(NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_deinit());

    rtc_clk_cpu_freq_set_config(&old_config);
}
#endif //SOC_MWDT_SUPPORT_XTAL

TEST_CASE("Task WDT inactive when no task to watch", "[task_wdt]")
{
    /* Make sure a timeout is NOT trigger when we have no task to watch */
    timeout_flag = false;
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = TASK_WDT_TIMEOUT_MS,
        .idle_core_mask = 0,
        .trigger_panic = false,
    };
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_init(&twdt_config));
    esp_rom_delay_us(2 * TASK_WDT_TIMEOUT_MS * 1000);
    TEST_ASSERT_EQUAL(false, timeout_flag);
    /* Add a task to watch, it should start the watchdog */
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_add(NULL));
    esp_rom_delay_us(TASK_WDT_TIMEOUT_MS * 1000);
    TEST_ASSERT_EQUAL(true, timeout_flag);
    /* Remove the task we just added and make sure the WDT is stopped*/
    timeout_flag = false;
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_delete(NULL));
    esp_rom_delay_us(2 * TASK_WDT_TIMEOUT_MS * 1000);
    TEST_ASSERT_EQUAL(false, timeout_flag);
    /* Success, terminate the test */
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_deinit());
}

TEST_CASE("Task WDT can be reconfigured", "[task_wdt]")
{
    /* Make sure a timeout is NOT trigger when we have no task to watch */
    timeout_flag = false;
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = TASK_WDT_TIMEOUT_MS / 2,
        .idle_core_mask = 0,
        .trigger_panic = false,
    };
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_init(&twdt_config));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_add(NULL));
    /* Timer started, check that a timeout is raised after a while */
    esp_rom_delay_us((TASK_WDT_TIMEOUT_MS / 2 + 1) * 1000);
    TEST_ASSERT_EQUAL(true, timeout_flag);
    /* Reconfigure the timer with a bigger timeout. The timer is restarted
     * after reconfiguring it. */
    twdt_config.timeout_ms = TASK_WDT_TIMEOUT_MS;
    timeout_flag = false;
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_reconfigure(&twdt_config));
    esp_rom_delay_us((TASK_WDT_TIMEOUT_MS / 2 + 1) * 1000);
    TEST_ASSERT_EQUAL(false, timeout_flag);
    /* Should be triggered now, we've spent TASK_WDT_TIMEOUT_MS waiting */
    esp_rom_delay_us((TASK_WDT_TIMEOUT_MS / 2 + 1) * 1000);
    TEST_ASSERT_EQUAL(true, timeout_flag);
    /* Success, terminate the test */
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_delete(NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_deinit());
}

TEST_CASE("Task WDT task feed", "[task_wdt]")
{
    timeout_flag = false;
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = TASK_WDT_TIMEOUT_MS,
        .idle_core_mask = 0,
        .trigger_panic = false,
    };
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_init(&twdt_config));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_add(NULL));
    // Feed the watchdog after a short delay
    esp_rom_delay_us((TASK_WDT_TIMEOUT_MS * 1000) / 2);
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_reset());
    esp_rom_delay_us((TASK_WDT_TIMEOUT_MS * 1000) / 2);
    TEST_ASSERT_EQUAL(false, timeout_flag);
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_delete(NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_deinit());
}

TEST_CASE("Task WDT user timeout", "[task_wdt]")
{
    const char *user_name = "test_user";
    esp_task_wdt_user_handle_t user_handle;
    timeout_flag = false;
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = TASK_WDT_TIMEOUT_MS,
        .idle_core_mask = 0,
        .trigger_panic = false,
    };
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_init(&twdt_config));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_add_user(user_name, &user_handle));
    // Short delay to allow timeout to occur
    esp_rom_delay_us(TASK_WDT_TIMEOUT_MS * 1000);
    TEST_ASSERT_EQUAL(true, timeout_flag);
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_delete_user(user_handle));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_deinit());
}

TEST_CASE("Task WDT user feed", "[task_wdt]")
{
    const char *user_name = "test_user";
    esp_task_wdt_user_handle_t user_handle;
    timeout_flag = false;
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = TASK_WDT_TIMEOUT_MS,
        .idle_core_mask = 0,
        .trigger_panic = false,
    };
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_init(&twdt_config));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_add_user(user_name, &user_handle));
    // Feed the watchdog after a short delay
    esp_rom_delay_us((TASK_WDT_TIMEOUT_MS * 1000) / 2);
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_reset_user(user_handle));
    esp_rom_delay_us((TASK_WDT_TIMEOUT_MS * 1000) / 2);
    TEST_ASSERT_EQUAL(false, timeout_flag);
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_delete_user(user_handle));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_deinit());
}

#endif // CONFIG_ESP_TASK_WDT_EN
