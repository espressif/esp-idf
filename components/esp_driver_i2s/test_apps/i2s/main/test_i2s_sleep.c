/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "unity_test_utils.h"
#include "driver/i2s_std.h"
#include "driver/uart.h"
#include "soc/i2s_struct.h"
#include "soc/soc_caps.h"
#include "esp_sleep.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"
#include "../../test_inc/test_i2s.h"

#define TEST_I2S_PD_SLEEP   (SOC_HAS(PAU) && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP)

extern void i2s_read_write_test(i2s_chan_handle_t tx_chan, i2s_chan_handle_t rx_chan);

static void s_test_i2s_enter_light_sleep(int sec, bool allow_power_down)
{
    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);
    printf("Entering light sleep for %d seconds\n", sec);
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    printf("Enable CPU power down\n");
    TEST_ESP_OK(sleep_cpu_configure(true));
#endif
    uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(sec * 1000 * 1000));
    TEST_ESP_OK(esp_light_sleep_start());

#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif
    printf("Woke up from light sleep\n");

    TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_request_result);
#if SOC_HAS(PAU)
    // check if the power domain also is powered down
    TEST_ASSERT_EQUAL(allow_power_down ? PMU_SLEEP_PD_TOP : 0, (sleep_ctx.sleep_flags) & PMU_SLEEP_PD_TOP);
#endif
    esp_sleep_set_sleep_context(NULL);
}

static void s_test_i2s_sleep(i2s_chan_handle_t tx_handle, i2s_chan_handle_t rx_handle, bool allow_power_down)
{
    /* Enter light sleep before I2S channel enabled and wake up after 1 second */
    s_test_i2s_enter_light_sleep(1, allow_power_down);
    /* Check whether I2S can work correctly after light sleep */
    TEST_ESP_OK(i2s_channel_enable(tx_handle));
    TEST_ESP_OK(i2s_channel_enable(rx_handle));
    i2s_read_write_test(tx_handle, rx_handle);
}

static void s_test_i2s_power_on_sleep(i2s_chan_handle_t tx_handle, i2s_chan_handle_t rx_handle)
{
    s_test_i2s_sleep(tx_handle, rx_handle, false);
}

#if TEST_I2S_PD_SLEEP
static void s_test_i2s_power_down_sleep(i2s_chan_handle_t tx_handle, i2s_chan_handle_t rx_handle)
{
#if SOC_GDMA_SUPPORT_SLEEP_RETENTION
    s_test_i2s_sleep(tx_handle, rx_handle, true);
#else
    /* I2S retention is depended on GDMA retention.
     * Only take two registers as sample to check the I2S retention when GDMA retention has not been supported. */
    i2s_tx_conf_reg_t tx_reg_before_slp = I2S0.tx_conf;
    i2s_rx_conf_reg_t rx_reg_before_slp = I2S0.rx_conf;
    /* Enter light sleep before I2S channel enabled and wake up after 1 second */
    s_test_i2s_enter_light_sleep(1, true);
    /* Only check whether the register values are restored if GDMA retention has not been supported */
    i2s_tx_conf_reg_t tx_reg_after_slp = I2S0.tx_conf;
    i2s_rx_conf_reg_t rx_reg_after_slp = I2S0.rx_conf;

    TEST_ASSERT_EQUAL_UINT32(tx_reg_before_slp.val, tx_reg_after_slp.val);
    TEST_ASSERT_EQUAL_UINT32(rx_reg_before_slp.val, rx_reg_after_slp.val);

    TEST_ESP_OK(i2s_channel_enable(tx_handle));
    TEST_ESP_OK(i2s_channel_enable(rx_handle));

    tx_reg_before_slp.val = I2S0.tx_conf.val;
    rx_reg_before_slp.val = I2S0.rx_conf.val;
    /* Enter light sleep before I2S channel enabled and wake up after 1 second */
    s_test_i2s_enter_light_sleep(1, true);
    /* Only check whether the register values are restored if GDMA retention has not been supported */
    tx_reg_after_slp.val = I2S0.tx_conf.val;
    rx_reg_after_slp.val = I2S0.rx_conf.val;

    TEST_ASSERT_EQUAL_UINT32(tx_reg_before_slp.val, tx_reg_after_slp.val);
    TEST_ASSERT_EQUAL_UINT32(rx_reg_before_slp.val, rx_reg_after_slp.val);
#endif  // SOC_GDMA_SUPPORT_SLEEP_RETENTION
}
#endif  // TEST_I2S_PD_SLEEP

void test_i2s_sleep_usability(bool allow_power_down)
{
    i2s_chan_handle_t tx_handle;
    i2s_chan_handle_t rx_handle;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    chan_cfg.allow_pd = allow_power_down;
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = I2S_TEST_MASTER_DEFAULT_PIN,
    };
    std_cfg.gpio_cfg.din = std_cfg.gpio_cfg.dout;
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_cfg));

    if (!allow_power_down) {
        s_test_i2s_power_on_sleep(tx_handle, rx_handle);
    }
#if TEST_I2S_PD_SLEEP
    else {
        s_test_i2s_power_down_sleep(tx_handle, rx_handle);
    }
#endif

    printf("I2S works as expected after light sleep\n");

    TEST_ESP_OK(i2s_channel_disable(tx_handle));
    TEST_ESP_OK(i2s_channel_disable(rx_handle));
    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}

TEST_CASE("I2S_light_sleep_usability_test", "[i2s]")
{
    printf("\nTesting I2S power on light sleep...\n");
    test_i2s_sleep_usability(false);
#if TEST_I2S_PD_SLEEP
    printf("\nTesting I2S power down light sleep...\n");
    test_i2s_sleep_usability(true);
#endif
}
