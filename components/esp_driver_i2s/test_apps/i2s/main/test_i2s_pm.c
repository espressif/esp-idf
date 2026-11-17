/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"
#include "driver/uart.h"
#include "esp_pm.h"
#include "esp_timer.h"
#include "soc/clk_tree_defs.h"
#include "soc/soc_caps.h"
#include "unity.h"

#include "../../test_inc/test_i2s.h"

#define I2S_PM_TEST_SAMPLE_RATE_HZ  48000
#if SOC_I2S_SUPPORTS_RC_FAST
#define I2S_PM_TEST_RC_FAST_SAMPLE_RATE_HZ  24000
#endif
#define I2S_PM_TEST_DURATION_US     5000
#define I2S_PM_TEST_TOLERANCE_PCT   2
#define I2S_PM_TEST_UART_PORT       UART_NUM_1

#if CONFIG_IDF_TARGET_ESP32S3
static const i2s_clock_src_t s_i2s_test_clk_sources[] = {
    I2S_CLK_SRC_PLL_240M, I2S_CLK_SRC_PLL_160M, I2S_CLK_SRC_XTAL, I2S_CLK_SRC_EXTERNAL
};
#elif CONFIG_IDF_TARGET_ESP32H4 || CONFIG_IDF_TARGET_ESP32H21
static const i2s_clock_src_t s_i2s_test_clk_sources[] = {
    I2S_CLK_SRC_PLL_96M, I2S_CLK_SRC_XTAL_X2, I2S_CLK_SRC_XTAL, I2S_CLK_SRC_EXTERNAL
};
#else
static const i2s_clock_src_t s_i2s_test_clk_sources[] = SOC_I2S_CLKS;
#endif

static void i2s_pm_test_timer_callback(void *arg)
{
    xTaskNotifyGive((TaskHandle_t)arg);
}

static uint32_t i2s_pm_test_lock_delta(const esp_pm_lock_stats_t *after,
                                       const esp_pm_lock_stats_t *before,
                                       esp_pm_lock_type_t type, bool acquired)
{
    size_t after_value = acquired ? after[type].acquired : after[type].created;
    size_t before_value = acquired ? before[type].acquired : before[type].created;
    return (uint32_t)(after_value - before_value);
}

static esp_pm_lock_type_t i2s_pm_test_expected_lock(i2s_clock_src_t clk_src)
{
#if CONFIG_IDF_TARGET_ESP32
    if (clk_src == I2S_CLK_SRC_PLL_160M || clk_src == I2S_CLK_SRC_APLL) {
        return ESP_PM_APB_FREQ_MAX;
    }
#elif CONFIG_IDF_TARGET_ESP32S3
    if (clk_src == I2S_CLK_SRC_PLL_240M || clk_src == I2S_CLK_SRC_PLL_160M) {
        return ESP_PM_APB_FREQ_MAX;
    }
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3
    if (clk_src == I2S_CLK_SRC_PLL_160M) {
        return ESP_PM_APB_FREQ_MAX;
    }
#endif
    return ESP_PM_NO_LIGHT_SLEEP;
}

static void i2s_pm_test_check_created_lock(esp_pm_lock_type_t expected_type, const esp_pm_lock_stats_t *before)
{
    esp_pm_lock_stats_t after[ESP_PM_LOCK_MAX] = {};
    TEST_ESP_OK(esp_pm_get_lock_stats_all(after));
    for (esp_pm_lock_type_t type = ESP_PM_CPU_FREQ_MAX; type < ESP_PM_LOCK_MAX; type++) {
        TEST_ASSERT_EQUAL_UINT32(type == expected_type ? 1 : 0,
                                 i2s_pm_test_lock_delta(after, before, type, false));
    }
}

static void i2s_pm_test_check_acquired_lock(esp_pm_lock_type_t expected_type, const esp_pm_lock_stats_t *before)
{
    esp_pm_lock_stats_t after[ESP_PM_LOCK_MAX] = {};
    TEST_ESP_OK(esp_pm_get_lock_stats_all(after));
    for (esp_pm_lock_type_t type = ESP_PM_APB_FREQ_MAX; type < ESP_PM_LOCK_MAX; type++) {
        TEST_ASSERT_EQUAL_UINT32(type == expected_type ? 1 : 0,
                                 i2s_pm_test_lock_delta(after, before, type, true));
    }
}

TEST_CASE("I2S TX clock remains continuous during DFS", "[i2s][pm]")
{
    esp_timer_handle_t timer = NULL;
    esp_timer_create_args_t timer_args = {
        .callback = i2s_pm_test_timer_callback,
        .arg = xTaskGetCurrentTaskHandle(),
        .name = "i2s_pm_test",
    };
    TEST_ESP_OK(esp_timer_create(&timer_args, &timer));

    const size_t test_clk_source_num = sizeof(s_i2s_test_clk_sources) / sizeof(s_i2s_test_clk_sources[0]);
    for (size_t i = 0; i < test_clk_source_num; i++) {
        i2s_clock_src_t clk_src = s_i2s_test_clk_sources[i];
#if SOC_I2S_HW_VERSION_2
        if (clk_src == I2S_CLK_SRC_EXTERNAL) {
            continue;
        }
#endif
#if CONFIG_IDF_TARGET_ESP32P4 && CONFIG_ESP32P4_SELECTS_REV_LESS_V3
        if (clk_src == I2S_CLK_SRC_PLL_160M) {
            continue;
        }
#endif

        uint32_t sample_rate_hz = I2S_PM_TEST_SAMPLE_RATE_HZ;
#if SOC_I2S_SUPPORTS_RC_FAST
        if (clk_src == I2S_CLK_SRC_RC_FAST) {
            sample_rate_hz = I2S_PM_TEST_RC_FAST_SAMPLE_RATE_HZ;
        }
#endif
        printf("Testing I2S clock source %d at %"PRIu32" Hz\n", (int)clk_src, sample_rate_hz);
        i2s_chan_handle_t tx_handle = NULL;
        i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
        i2s_std_config_t std_cfg = {
            .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate_hz),
            .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT,
                                                            I2S_SLOT_MODE_STEREO),
            .gpio_cfg = I2S_TEST_MASTER_DEFAULT_PIN,
        };
        std_cfg.clk_cfg.clk_src = clk_src;
        esp_pm_lock_type_t expected_lock = i2s_pm_test_expected_lock(clk_src);

        esp_pm_lock_stats_t before_create[ESP_PM_LOCK_MAX] = {};
        TEST_ESP_OK(esp_pm_get_lock_stats_all(before_create));
        TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));
        TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
        i2s_pm_test_check_created_lock(expected_lock, before_create);

        esp_pm_lock_stats_t before_enable[ESP_PM_LOCK_MAX] = {};
        TEST_ESP_OK(esp_pm_get_lock_stats_all(before_enable));
        TEST_ESP_OK(i2s_channel_enable(tx_handle));
        i2s_pm_test_check_acquired_lock(expected_lock, before_enable);

        uart_bitrate_detect_config_t uart_cfg = {
            .rx_io_num = MASTER_WS_IO,
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
            .source_clk = UART_SCLK_REF_TICK,
#else
            .source_clk = UART_SCLK_XTAL,
#endif
        };
        TEST_ESP_OK(uart_detect_bitrate_start(I2S_PM_TEST_UART_PORT, &uart_cfg));
        int64_t start_time_us = esp_timer_get_time();
        TEST_ESP_OK(esp_timer_start_once(timer, I2S_PM_TEST_DURATION_US));
        /* Yield the CPU for the 5 ms measurement window. */
        TEST_ASSERT_GREATER_THAN_UINT32(0, ulTaskNotifyTake(pdTRUE, portMAX_DELAY));
        int64_t end_time_us = esp_timer_get_time();

        uart_bitrate_res_t uart_res = {};
        TEST_ESP_OK(uart_detect_bitrate_stop(I2S_PM_TEST_UART_PORT, true, &uart_res));
        TEST_ESP_OK(esp_pm_dump_locks(stdout));

        uint32_t elapsed_us = (uint32_t)(end_time_us - start_time_us);
        uint32_t expected_edges = (uint32_t)(((uint64_t)sample_rate_hz * 2 * elapsed_us + 500000)
                                             / 1000000);
        uint32_t edge_tolerance = expected_edges * I2S_PM_TEST_TOLERANCE_PCT / 100;
        printf("clk_src=%d, elapsed=%"PRIu32" us, edges=%"PRIu32", expected=%"PRIu32
               ", pos_period=%"PRIu32", uart_clk=%"PRIu32" Hz\n",
               (int)clk_src, elapsed_us, uart_res.edge_cnt, expected_edges,
               uart_res.pos_period, uart_res.clk_freq_hz);
        TEST_ASSERT_UINT32_WITHIN(edge_tolerance, expected_edges, uart_res.edge_cnt);

        TEST_ASSERT_GREATER_THAN_UINT32(0, uart_res.pos_period);
        bool period_is_reliable = true;
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
        /* The auto-baud period counter uses APB on these targets, even when UART uses REF_TICK. */
        period_is_reliable = expected_lock == ESP_PM_APB_FREQ_MAX;
#endif
        if (period_is_reliable) {
            uint32_t measured_ws_hz = (uart_res.clk_freq_hz + uart_res.pos_period / 2) / uart_res.pos_period;
            uint32_t frequency_tolerance = sample_rate_hz * I2S_PM_TEST_TOLERANCE_PCT / 100;
            TEST_ASSERT_UINT32_WITHIN(frequency_tolerance, sample_rate_hz, measured_ws_hz);
        }

        TEST_ESP_OK(i2s_channel_disable(tx_handle));
        TEST_ESP_OK(i2s_del_channel(tx_handle));
    }

    TEST_ESP_OK(esp_timer_delete(timer));
}
