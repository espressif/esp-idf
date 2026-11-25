/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "esp_attr.h"
#include "unity.h"
#include "test_utils.h"
#include "driver/gpio.h"
#include "driver/temperature_sensor.h"
#include "hal/adc_ll.h"
#include "hal/adc_periph.h"
#include "ulp_lp_core.h"
#include "ulp_lp_core_lp_adc_shared.h"
#include "lp_sleep_common.h"
#include "lp_sleep_payload.h"
#include "lp_core_test_app_adc_sleep.h"

extern const uint8_t lp_core_test_app_adc_sleep_bin_start[]
asm("_binary_lp_core_test_app_adc_sleep_bin_start");
extern const uint8_t lp_core_test_app_adc_sleep_bin_end[]
asm("_binary_lp_core_test_app_adc_sleep_bin_end");

/* P4 deep-sleep CI observes weak-pull readings up to about 1212 low
 * and about 2941 high, across revisions 3.2 and 3.3. */
#define ADC_TEST_LOW_THRESHOLD  1300
#define ADC_TEST_HIGH_THRESHOLD 2800
#define TEMPERATURE_MAX_DELTA_C  6.0f
#define ADC_GET_IO_NUM(unit, channel) (adc_channel_io_map[unit][channel])

static RTC_DATA_ATTR uint32_t s_adc_atten_before[LP_SLEEP_ADC_CHANNELS];
static RTC_DATA_ATTR float s_temperature_before;

typedef enum {
    ADC_EXPECT_LOW,
    ADC_EXPECT_HIGH,
} adc_expect_t;

static void lp_adc_set_pull(bool high, bool release_hold)
{
    for (int channel = 0; channel < LP_SLEEP_ADC_CHANNELS; channel++) {
        gpio_num_t io_num = ADC_GET_IO_NUM(ADC_UNIT_1, channel);
        if (release_hold) {
            TEST_ESP_OK(gpio_hold_dis(io_num));
        }
        TEST_ESP_OK(gpio_set_pull_mode(io_num, high ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY));
        lp_sleep_keep_pin(io_num);
        TEST_ESP_OK(gpio_hold_en(io_num));
    }
}

static void lp_adc_release_pins(void)
{
    for (int channel = 0; channel < LP_SLEEP_ADC_CHANNELS; channel++) {
        gpio_num_t io_num = ADC_GET_IO_NUM(ADC_UNIT_1, channel);
        TEST_ESP_OK(gpio_hold_dis(io_num));
    }
}

static void lp_adc_init(void)
{
    const lp_core_lp_adc_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    TEST_ASSERT_EQUAL(LP_SLEEP_ADC_CHANNELS, ADC_LL_CHANNEL_NUM(ADC_UNIT_1));
    TEST_ESP_OK(lp_core_lp_adc_init(ADC_UNIT_1));
    for (int channel = 0; channel < LP_SLEEP_ADC_CHANNELS; channel++) {
        TEST_ESP_OK(lp_core_lp_adc_config_channel(ADC_UNIT_1, channel, &config));
        s_adc_atten_before[channel] = adc_ll_get_atten(ADC_UNIT_1, channel);
    }
}

static void lp_adc_check(adc_expect_t expected)
{
    lp_sleep_expect_ulp_wakeup();

    for (int channel = 0; channel < LP_SLEEP_ADC_CHANNELS; channel++) {
        int raw = (int)ulp_adc_sleep_raw[channel];
        printf("LP ADC channel %d ret %ld raw %d\n",
               channel, (long)ulp_adc_sleep_ret[channel], raw);
        TEST_ASSERT_EQUAL(ESP_OK, (esp_err_t)ulp_adc_sleep_ret[channel]);
        TEST_ASSERT_EQUAL_UINT32(s_adc_atten_before[channel],
                                 adc_ll_get_atten(ADC_UNIT_1, channel));
        if (expected == ADC_EXPECT_LOW) {
            TEST_ASSERT_LESS_THAN_INT(ADC_TEST_LOW_THRESHOLD, raw);
        } else {
            TEST_ASSERT_GREATER_THAN_INT(ADC_TEST_HIGH_THRESHOLD, raw);
        }
    }
}

static void lp_adc_run(bool load_binary)
{
    lp_sleep_run(lp_core_test_app_adc_sleep_bin_start, lp_core_test_app_adc_sleep_bin_end,
                 ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER, load_binary);
    hp_core_deep_sleep_enter();
}

static void lp_adc_sleeper_first(void)
{
    lp_adc_init();
    lp_adc_set_pull(false, false);
    lp_adc_run(true);
}

static void lp_adc_sleeper_second(void)
{
    lp_adc_check(ADC_EXPECT_LOW);

    /* Keep the ADC configuration, but replace the held low test stimulus. */
    lp_adc_set_pull(true, true);
    lp_adc_run(false);
}

static void lp_adc_sleeper_check(void)
{
    lp_adc_check(ADC_EXPECT_HIGH);
    lp_adc_release_pins();
}

static temperature_sensor_handle_t temperature_sensor_read(float *value)
{
    temperature_sensor_handle_t sensor = NULL;
    temperature_sensor_config_t config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);

    TEST_ESP_OK(temperature_sensor_install(&config, &sensor));
    TEST_ESP_OK(temperature_sensor_enable(sensor));
    TEST_ESP_OK(temperature_sensor_get_celsius(sensor, value));
    printf("Temperature sensor value: %.02f C\n", *value);
    return sensor;
}

static temperature_sensor_handle_t temperature_sensor_check(void)
{
    float value;
    temperature_sensor_handle_t sensor = temperature_sensor_read(&value);
    TEST_ASSERT_FLOAT_WITHIN(TEMPERATURE_MAX_DELTA_C, s_temperature_before, value);
    return sensor;
}

static void lp_adc_temperature_sleeper_first(void)
{
    /* The driver handle is lost when deep sleep resets HP, but the LP
     * temperature-sensor hardware stays enabled while LP ADC samples. */
    (void)temperature_sensor_read(&s_temperature_before);

    lp_adc_init();
    lp_adc_set_pull(false, false);
    lp_adc_run(true);
}

static void lp_adc_temperature_sleeper_second(void)
{
    lp_adc_check(ADC_EXPECT_LOW);
    (void)temperature_sensor_check();

    lp_adc_set_pull(true, true);
    lp_adc_run(false);
}

static void lp_adc_temperature_sleeper_check(void)
{
    lp_adc_check(ADC_EXPECT_HIGH);
    lp_adc_release_pins();
    temperature_sensor_handle_t sensor = temperature_sensor_check();
    TEST_ESP_OK(temperature_sensor_disable(sensor));
    TEST_ESP_OK(temperature_sensor_uninstall(sensor));
}

TEST_CASE_MULTIPLE_STAGES(
    "LP ADC multi-channel reads during deep sleep",
    "[lp_core][lp_adc_sleep][test_env=generic][timeout=120][deepsleep]",
    lp_adc_sleeper_first,
    lp_adc_sleeper_second,
    lp_adc_sleeper_check);

TEST_CASE_MULTIPLE_STAGES(
    "Temperature sensor does not interfere with LP-ADC during deep sleep",
    "[lp_core][lp_adc_temperature_sleep][test_env=generic][timeout=120][deepsleep]",
    lp_adc_temperature_sleeper_first,
    lp_adc_temperature_sleeper_second,
    lp_adc_temperature_sleeper_check);
