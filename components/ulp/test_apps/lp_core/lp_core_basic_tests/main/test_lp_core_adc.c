/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/adc_types.h"
#include "hal/adc_ll.h"
#include "lp_core_test_app_adc.h"
#include "ulp_lp_core.h"
#include "ulp_lp_core_lp_adc_shared.h"
#include "hal/adc_periph.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "driver/temperature_sensor.h"
#include "esp_timer.h"

#include "unity.h"

extern const uint8_t lp_core_main_adc_bin_start[] asm("_binary_lp_core_test_app_adc_bin_start");
extern const uint8_t lp_core_main_adc_bin_end[]   asm("_binary_lp_core_test_app_adc_bin_end");

#if CONFIG_IDF_TARGET_ESP32P4
// Threshold values picked up empirically after manual testing
#define ADC_TEST_LOW_VAL         1000
#define ADC_TEST_HIGH_VAL        3200
#else
#error "ADC threshold values not defined"
#endif

#define ADC_GET_IO_NUM(unit, channel) (adc_channel_io_map[unit][channel])

#define ADC_SETTLE_DELAY_MS      100 /* RC settle of the weak internal pulls */
#define ADC_SCAN_TIMEOUT_MS      100 /* upper bound on LP scan completion */
#define ADC_SCANS_TO_WAIT        2   /* completed LP scans before taking a snapshot */

static bool s_lp_adc_inited;
static bool s_lp_firmware_started;
static adc_unit_t s_lp_adc_unit;

static void test_adc_set_io_level(adc_unit_t unit, adc_channel_t channel, bool level)
{
    TEST_ASSERT(channel < ADC_LL_CHANNEL_NUM(unit) && "invalid channel");

    uint32_t io_num = ADC_GET_IO_NUM(unit, channel);
    TEST_ESP_OK(gpio_set_pull_mode(io_num, (level ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY)));
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
    if (rtc_gpio_is_valid_gpio(io_num)) {
        if (level) {
            TEST_ESP_OK(rtc_gpio_pullup_en(io_num));
            TEST_ESP_OK(rtc_gpio_pulldown_dis(io_num));
        } else {
            TEST_ESP_OK(rtc_gpio_pullup_dis(io_num));
            TEST_ESP_OK(rtc_gpio_pulldown_en(io_num));
        }
    }
#endif
}

static void test_adc_set_all_io_level(adc_unit_t unit, bool level)
{
    for (int ch = 0; ch < ADC_LL_CHANNEL_NUM(unit); ch++) {
        test_adc_set_io_level(unit, ch, level);
    }
}

static void load_and_start_lp_core_firmware(ulp_lp_core_cfg_t* cfg, const uint8_t* firmware_start, const uint8_t* firmware_end)
{
    TEST_ASSERT(ulp_lp_core_load_binary(firmware_start,
                                        (firmware_end - firmware_start)) == ESP_OK);

    TEST_ASSERT(ulp_lp_core_run(cfg) == ESP_OK);
    s_lp_firmware_started = true;
}

static uint32_t adc_scan_seq_get(void)
{
    return *((volatile uint32_t *)&ulp_adc_scan_seq);
}

static void snapshot_adc_raw(int *dst, int n)
{
    volatile int *src = (volatile int *)&ulp_adc_raw;
    for (int i = 0; i < n; i++) {
        dst[i] = src[i];
    }
}

static void lp_adc_init_tracked(adc_unit_t unit_id)
{
    ESP_ERROR_CHECK(lp_core_lp_adc_init(unit_id));
    s_lp_adc_unit = unit_id;
    s_lp_adc_inited = true;
}

static esp_err_t lp_adc_hw_deinit(void)
{
    esp_err_t ret = ESP_OK;
    if (s_lp_adc_inited) {
        ret = lp_core_lp_adc_deinit(s_lp_adc_unit);
        s_lp_adc_inited = false;
    }
    return ret;
}

void lp_adc_test_teardown(void)
{
    (void)lp_adc_hw_deinit();
    if (s_lp_firmware_started) {
        ulp_lp_core_stop();
        s_lp_firmware_started = false;
    }
}

static void delay_poll_tick(void)
{
    /* pdMS_TO_TICKS(1) is 0 at a 10 ms FreeRTOS tick, so wait one tick. */
    vTaskDelay(1);
}

static void wait_for_adc_scans(uint32_t count)
{
    uint32_t start_seq = adc_scan_seq_get();
    const int64_t deadline_us = esp_timer_get_time() + (int64_t)ADC_SCAN_TIMEOUT_MS * 1000;
    while ((adc_scan_seq_get() - start_seq) < count && esp_timer_get_time() < deadline_us) {
        delay_poll_tick();
    }
    TEST_ASSERT_MESSAGE((adc_scan_seq_get() - start_seq) >= count,
                        "LP ADC did not complete the expected number of scans");
}

/* Every sample is taken after the GPIO change. Values may span two adjacent
 * LP scans; that is fine because the pull state is stable. Do not search
 * for a passing window.
 */
static void snapshot_after_gpio_change(int *dst, int n)
{
    vTaskDelay(pdMS_TO_TICKS(ADC_SETTLE_DELAY_MS));
    wait_for_adc_scans(ADC_SCANS_TO_WAIT);
    snapshot_adc_raw(dst, n);
}

void test_lp_adc(adc_unit_t unit_id)
{
    int adc_raw[8];
    const int n = ADC_LL_CHANNEL_NUM(unit_id);
    TEST_ASSERT(n <= (int)(sizeof(adc_raw) / sizeof(adc_raw[0])));
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    /* Init ADC and GPIO before starting the LP firmware so the first scan
     * is not taken on unconfigured channels / floating pins.
     */
    lp_adc_init_tracked(unit_id);

    const lp_core_lp_adc_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    /* Configure all ADC channels.
     * LP ADC1: Channels 0 - 7
     * LP ADC2: Channels 0 - 5
     */
    TEST_ASSERT(lp_core_lp_adc_config_channel(unit_id, ADC_CHANNEL_0, &config) == ESP_OK);
    TEST_ASSERT(lp_core_lp_adc_config_channel(unit_id, ADC_CHANNEL_1, &config) == ESP_OK);
    TEST_ASSERT(lp_core_lp_adc_config_channel(unit_id, ADC_CHANNEL_2, &config) == ESP_OK);
    TEST_ASSERT(lp_core_lp_adc_config_channel(unit_id, ADC_CHANNEL_3, &config) == ESP_OK);
    TEST_ASSERT(lp_core_lp_adc_config_channel(unit_id, ADC_CHANNEL_4, &config) == ESP_OK);
    TEST_ASSERT(lp_core_lp_adc_config_channel(unit_id, ADC_CHANNEL_5, &config) == ESP_OK);
    if (unit_id == ADC_UNIT_1) {
        TEST_ASSERT(lp_core_lp_adc_config_channel(unit_id, ADC_CHANNEL_6, &config) == ESP_OK);
        TEST_ASSERT(lp_core_lp_adc_config_channel(unit_id, ADC_CHANNEL_7, &config) == ESP_OK);
    }

    test_adc_set_all_io_level(unit_id, 0);
    load_and_start_lp_core_firmware(&cfg, lp_core_main_adc_bin_start, lp_core_main_adc_bin_end);
    snapshot_after_gpio_change(adc_raw, n);

    for (int i = 0; i < n; i++) {
        printf("LP ADC low[%d] = %d\n", i, adc_raw[i]);
        TEST_ASSERT_LESS_THAN_INT(ADC_TEST_LOW_VAL, adc_raw[i]);
    }

    test_adc_set_all_io_level(unit_id, 1);
    snapshot_after_gpio_change(adc_raw, n);

    for (int i = 0; i < n; i++) {
        printf("LP ADC high[%d] = %d\n", i, adc_raw[i]);
        TEST_ASSERT_GREATER_THAN_INT(ADC_TEST_HIGH_VAL, adc_raw[i]);
    }

    TEST_ESP_OK(lp_adc_hw_deinit());
}

TEST_CASE("LP ADC 1 raw read test", "[lp_core]")
{
    test_lp_adc(ADC_UNIT_1);
}

// Enable when DIG-396 is fixed
// TEST_CASE("LP ADC 2 raw read test", "[lp_core]")
// {
//     test_lp_adc(ADC_UNIT_2);
// }

static void test_lp_adc_stress(adc_unit_t unit_id)
{
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };
    int adc_raw;

    /* Firmware is started once and left running across init/deinit cycles so
     * the LP core keeps converting while HP has released the ADC.
     */
    for (int i = 0; i < 100; i++) {
        lp_adc_init_tracked(unit_id);

        const lp_core_lp_adc_chan_cfg_t config = {
            .atten = ADC_ATTEN_DB_12,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };

        TEST_ASSERT(lp_core_lp_adc_config_channel(unit_id, ADC_CHANNEL_0, &config) == ESP_OK);

        test_adc_set_io_level(unit_id, ADC_CHANNEL_0, 1);
        if (!s_lp_firmware_started) {
            load_and_start_lp_core_firmware(&cfg, lp_core_main_adc_bin_start, lp_core_main_adc_bin_end);
        }
        snapshot_after_gpio_change(&adc_raw, 1);
        TEST_ASSERT_NOT_EQUAL(0, adc_raw);

        TEST_ESP_OK(lp_adc_hw_deinit());
    }
}

TEST_CASE("LP ADC 1 raw read stress test", "[lp_core]")
{
    test_lp_adc_stress(ADC_UNIT_1);
}

// Enable when DIG-396 is fixed
// TEST_CASE("LP ADC 2 raw read stress test", "[lp_core]")
// {
//     test_lp_adc_stress(ADC_UNIT_2);
// }

TEST_CASE("Test temperature sensor does not affect LP ADC", "[lp_core]")
{
    printf("Install temperature sensor, expected temp ranger range: 10~50 ℃\n");
    temperature_sensor_handle_t temp_sensor = NULL;
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);
    TEST_ESP_OK(temperature_sensor_install(&temp_sensor_config, &temp_sensor));
    int cnt = 2;
    float tsens_value;
    while (cnt--) {
        temperature_sensor_enable(temp_sensor);
        TEST_ESP_OK(temperature_sensor_get_celsius(temp_sensor, &tsens_value));
        printf("Temperature value %.02f ℃\n", tsens_value);
        vTaskDelay(pdMS_TO_TICKS(100));
        TEST_ESP_OK(temperature_sensor_disable(temp_sensor));
    }

    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    lp_adc_init_tracked(ADC_UNIT_1);

    const lp_core_lp_adc_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    TEST_ASSERT(lp_core_lp_adc_config_channel(ADC_UNIT_1, ADC_CHANNEL_0, &config) == ESP_OK);
    load_and_start_lp_core_firmware(&cfg, lp_core_main_adc_bin_start, lp_core_main_adc_bin_end);

    int adc_raw;
    cnt = 2;
    while (cnt--) {
        wait_for_adc_scans(ADC_SCANS_TO_WAIT);
        snapshot_adc_raw(&adc_raw, 1);
        printf("LP ADC%d Channel[%d] Raw Data: %d\n", ADC_UNIT_1 + 1, 0, adc_raw);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    TEST_ESP_OK(lp_adc_hw_deinit());
    lp_adc_test_teardown();

    cnt = 2;
    while (cnt--) {
        temperature_sensor_enable(temp_sensor);
        TEST_ESP_OK(temperature_sensor_get_celsius(temp_sensor, &tsens_value));
        printf("Temperature value %.02f ℃\n", tsens_value);
        vTaskDelay(pdMS_TO_TICKS(100));
        TEST_ESP_OK(temperature_sensor_disable(temp_sensor));
    }

    TEST_ESP_OK(temperature_sensor_uninstall(temp_sensor));
}
