/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include "unity.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "soc/adc_periph.h"

#define ADC_GET_IO_NUM(unit, channel) (adc_channel_io_map[unit][channel])

/**
 * We use weak pulldown, `ADC_TEST_LOW_THRESH` may vary.
 * If connect to GND, `ADC_TEST_LOW_THRESH` will usually within 10
 */
#if CONFIG_IDF_TARGET_ESP32
#define ADC_TEST_LOW_VAL         0
#define ADC_TEST_LOW_THRESH      10

#define ADC_TEST_HIGH_VAL        4095
#define ADC_TEST_HIGH_THRESH     10

#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_TEST_LOW_VAL         0
#define ADC_TEST_LOW_THRESH      35

#define ADC_TEST_HIGH_VAL        8191
#define ADC_TEST_HIGH_THRESH     10

#elif CONFIG_IDF_TARGET_ESP32C3
#define ADC_TEST_LOW_VAL         0
#define ADC_TEST_LOW_THRESH      60     //This is due to ADC2 accuracy is not as good as ADC1, and also we use weak pulldown

#define ADC_TEST_HIGH_VAL        4095
#define ADC_TEST_HIGH_THRESH     10

#elif CONFIG_IDF_TARGET_ESP32S3
#define ADC_TEST_LOW_VAL         0
#define ADC_TEST_LOW_THRESH      15

#define ADC_TEST_HIGH_VAL        4095
#define ADC_TEST_HIGH_THRESH     0

#elif CONFIG_IDF_TARGET_ESP32C2
#define ADC_TEST_LOW_VAL         0
#define ADC_TEST_LOW_THRESH      15

#define ADC_TEST_HIGH_VAL        3400
#define ADC_TEST_HIGH_THRESH     200

#elif CONFIG_IDF_TARGET_ESP32C6
#define ADC_TEST_LOW_VAL         0
#define ADC_TEST_LOW_THRESH      15

#define ADC_TEST_HIGH_VAL        3350
#define ADC_TEST_HIGH_THRESH     200

#elif CONFIG_IDF_TARGET_ESP32H2
#define ADC_TEST_LOW_VAL         0
#define ADC_TEST_LOW_THRESH      17

#define ADC_TEST_HIGH_VAL        3390
#define ADC_TEST_HIGH_THRESH     200

#elif CONFIG_IDF_TARGET_ESP32P4
#define ADC_TEST_LOW_VAL         3100
#define ADC_TEST_LOW_THRESH      200

#define ADC_TEST_HIGH_VAL        4095
#define ADC_TEST_HIGH_THRESH     200

#elif CONFIG_IDF_TARGET_ESP32C5
#define ADC_TEST_LOW_VAL         2195
#define ADC_TEST_LOW_THRESH      200

#define ADC_TEST_HIGH_VAL        4095
#define ADC_TEST_HIGH_THRESH     200

#endif

//ADC Channels
#if CONFIG_IDF_TARGET_ESP32
#define ADC1_TEST_CHAN0          ADC1_CHANNEL_4
#define ADC2_TEST_CHAN0          ADC2_CHANNEL_0
#elif (SOC_ADC_PERIPH_NUM >= 2)
#define ADC1_TEST_CHAN0          ADC1_CHANNEL_2
#define ADC2_TEST_CHAN0          ADC2_CHANNEL_0
#else
#define ADC1_TEST_CHAN0          ADC1_CHANNEL_2
#endif

//ESP32C3 ADC2 oneshot mode is not supported anymore
#define ADC_TEST_ADC2    ((SOC_ADC_PERIPH_NUM >= 2) && !CONFIG_IDF_TARGET_ESP32C3)

const __attribute__((unused)) static char *TAG = "TEST_ADC_LEGACY";

void test_adc_set_io_level(adc_unit_t unit, adc_channel_t channel, bool level)
{
    TEST_ASSERT(channel < SOC_ADC_CHANNEL_NUM(unit) && "invalid channel");

#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
    uint32_t io_num = ADC_GET_IO_NUM(unit, channel);
    TEST_ESP_OK(gpio_set_pull_mode(io_num, (level ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY)));
#else
    gpio_num_t io_num = ADC_GET_IO_NUM(unit, channel);
    if (level) {
        TEST_ESP_OK(rtc_gpio_pullup_en(io_num));
        TEST_ESP_OK(rtc_gpio_pulldown_dis(io_num));
    } else {
        TEST_ESP_OK(rtc_gpio_pullup_dis(io_num));
        TEST_ESP_OK(rtc_gpio_pulldown_en(io_num));
    }
#endif
}

TEST_CASE("Legacy ADC oneshot high/low test", "[legacy_adc_oneshot]")
{
    int adc_raw = 0;
    //ADC1 config
    TEST_ESP_OK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    TEST_ESP_OK(adc1_config_channel_atten(ADC1_TEST_CHAN0, ADC_ATTEN_DB_12));
#if ADC_TEST_ADC2
    //ADC2 config
    TEST_ESP_OK(adc2_config_channel_atten(ADC2_TEST_CHAN0, ADC_ATTEN_DB_12));
#endif

    test_adc_set_io_level(ADC_UNIT_1, (adc1_channel_t)ADC1_TEST_CHAN0, 0);
    adc_raw = adc1_get_raw(ADC1_TEST_CHAN0);
    ESP_LOGI(TAG, "ADC%d Channel %d raw: %d", ADC_UNIT_1, ADC1_TEST_CHAN0, adc_raw);
    TEST_ASSERT_INT_WITHIN(ADC_TEST_LOW_THRESH, ADC_TEST_LOW_VAL, adc_raw);

    test_adc_set_io_level(ADC_UNIT_1, (adc1_channel_t)ADC1_TEST_CHAN0, 1);
    adc_raw = adc1_get_raw(ADC1_TEST_CHAN0);
    ESP_LOGI(TAG, "ADC%d Channel %d raw: %d", ADC_UNIT_1, ADC1_TEST_CHAN0, adc_raw);
    TEST_ASSERT_INT_WITHIN(ADC_TEST_HIGH_THRESH, ADC_TEST_HIGH_VAL, adc_raw);

#if ADC_TEST_ADC2
    test_adc_set_io_level(ADC_UNIT_2, (adc2_channel_t)ADC2_TEST_CHAN0, 0);
    TEST_ESP_OK(adc2_get_raw(ADC2_TEST_CHAN0, ADC_WIDTH_BIT_DEFAULT, &adc_raw));
    ESP_LOGI(TAG, "ADC%d Channel %d raw: %d", ADC_UNIT_2, ADC2_TEST_CHAN0, adc_raw);
    TEST_ASSERT_INT_WITHIN(ADC_TEST_LOW_THRESH, ADC_TEST_LOW_VAL, adc_raw);

    test_adc_set_io_level(ADC_UNIT_2, (adc2_channel_t)ADC2_TEST_CHAN0, 1);
    TEST_ESP_OK(adc2_get_raw(ADC2_TEST_CHAN0, ADC_WIDTH_BIT_DEFAULT, &adc_raw));
    ESP_LOGI(TAG, "ADC%d Channel %d raw: %d", ADC_UNIT_2, ADC2_TEST_CHAN0, adc_raw);
    TEST_ASSERT_INT_WITHIN(ADC_TEST_HIGH_THRESH, ADC_TEST_HIGH_VAL, adc_raw);
#endif
}
