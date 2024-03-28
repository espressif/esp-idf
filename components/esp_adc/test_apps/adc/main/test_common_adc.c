/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include "unity.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "soc/adc_periph.h"
#include "test_common_adc.h"
#include "hal/adc_ll.h"

__attribute__((unused)) static const char *TAG = "TEST_ADC";

/*---------------------------------------------------------------
        ADC Attenuation
---------------------------------------------------------------*/
#if CONFIG_IDF_TARGET_ESP32C2
adc_atten_t g_test_atten[TEST_ATTEN_NUMS] = {ADC_ATTEN_DB_0, ADC_ATTEN_DB_12};
#else
adc_atten_t g_test_atten[TEST_ATTEN_NUMS] = {ADC_ATTEN_DB_0, ADC_ATTEN_DB_2_5, ADC_ATTEN_DB_6, ADC_ATTEN_DB_12};
#endif

#if SOC_ADC_DIG_IIR_FILTER_SUPPORTED
adc_digi_iir_filter_coeff_t g_test_filter_coeff[TEST_FILTER_COEFF_NUMS] = {
    ADC_DIGI_IIR_FILTER_COEFF_2,
    ADC_DIGI_IIR_FILTER_COEFF_4,
    ADC_DIGI_IIR_FILTER_COEFF_8,
    ADC_DIGI_IIR_FILTER_COEFF_16,
    ADC_DIGI_IIR_FILTER_COEFF_64,
};
#endif

/*---------------------------------------------------------------
        ADC Calibration
---------------------------------------------------------------*/
bool test_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_bitwidth_t bitwidth, adc_cali_handle_t *out_handle)
{
    esp_err_t ret = ESP_FAIL;
    adc_cali_handle_t handle = NULL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = unit,
        .chan = channel,
        .atten = atten,
        .bitwidth = bitwidth,
    };
    ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);

#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = unit,
        .atten = atten,
        .bitwidth = bitwidth,
    };
    ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
#endif

    if (ret == ESP_OK) {
        calibrated = true;
    } else if (ret == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGW(TAG, "calibration fail due to lack of eFuse bits");
    } else {
        TEST_ASSERT(false);
    }

    *out_handle = handle;

    return calibrated;
}

void test_adc_calibration_deinit(adc_cali_handle_t handle)
{
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Curve Fitting");
    TEST_ESP_OK(adc_cali_delete_scheme_curve_fitting(handle));

#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Line Fitting");
    TEST_ESP_OK(adc_cali_delete_scheme_line_fitting(handle));
#endif
}

/*---------------------------------------------------------------
        ADC GPIO
---------------------------------------------------------------*/
#define ADC_GET_IO_NUM(unit, channel) (adc_channel_io_map[unit][channel])

void test_adc_set_io_level(adc_unit_t unit, adc_channel_t channel, bool level)
{
    TEST_ASSERT(channel < SOC_ADC_CHANNEL_NUM(unit) && "invalid channel");

#if !ADC_LL_RTC_GPIO_SUPPORTED
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
    TEST_ESP_OK(gpio_set_pull_mode(io_num, (level ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY)));
#endif
}

void test_adc_set_io_middle(adc_unit_t unit, adc_channel_t channel)
{
    TEST_ASSERT(channel < SOC_ADC_CHANNEL_NUM(unit) && "invalid channel");

    uint32_t io_num = ADC_GET_IO_NUM(unit, channel);

#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
    TEST_ESP_OK(gpio_set_pull_mode(io_num, GPIO_PULLUP_PULLDOWN));
#else
    TEST_ESP_OK(rtc_gpio_init(io_num));
    TEST_ESP_OK(rtc_gpio_pullup_en(io_num));
    TEST_ESP_OK(rtc_gpio_pulldown_en(io_num));
    TEST_ESP_OK(rtc_gpio_set_direction(io_num, RTC_GPIO_MODE_DISABLED));
#endif
    vTaskDelay(10 / portTICK_PERIOD_MS);
}
