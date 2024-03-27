/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 Tests for the dac device driver
*/
#include <inttypes.h>
#include "esp_system.h"

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "soc/soc_caps.h"
#define CONFIG_ADC_SUPPRESS_DEPRECATE_WARN 1
#include "driver/adc.h"

#include "driver/dac.h"
#include "esp_adc_cal.h"

static const char *TAG = "test_dac";

#ifdef CONFIG_IDF_TARGET_ESP32
#define ADC_TEST_WIDTH         ADC_WIDTH_BIT_12
#elif defined CONFIG_IDF_TARGET_ESP32S2
#define ADC_TEST_WIDTH         ADC_WIDTH_BIT_13   //ESP32S2 only support 13 bit width
#endif
#define ADC_TEST_ATTEN         ADC_ATTEN_DB_12

#if CONFIG_IDF_TARGET_ESP32
#define ADC_TEST_CHANNEL_NUM   ADC2_CHANNEL_8   // GPIO25
#define DAC_TEST_CHANNEL_NUM   DAC_CHAN_0    // GPIO25
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_TEST_CHANNEL_NUM   ADC2_CHANNEL_6   // GPIO17
#define DAC_TEST_CHANNEL_NUM   DAC_CHAN_0    // GPIO17
#endif

#define DAC_OUT_MAX       (200)
#define DAC_OUT_TIMES     (10)
#define DAC_OUT_STEP      (DAC_OUT_MAX / DAC_OUT_TIMES)

#define DAC_TEST_TIMES    (100)

TEST_CASE("DAC_output(RTC)_check_by_adc", "[dac_legacy]")
{
    gpio_num_t adc_gpio_num, dac_gpio_num;

    TEST_ESP_OK(adc2_pad_get_io_num(ADC_TEST_CHANNEL_NUM, &adc_gpio_num));
    TEST_ESP_OK(dac_pad_get_io_num(DAC_TEST_CHANNEL_NUM, &dac_gpio_num));

    printf("Please connect ADC2 CH%d-GPIO%d <--> DAC CH%d-GPIO%d.\n", ADC_TEST_CHANNEL_NUM, adc_gpio_num,
           DAC_TEST_CHANNEL_NUM + 1, dac_gpio_num);

    TEST_ESP_OK(dac_output_enable(DAC_TEST_CHANNEL_NUM));

    //be sure to do the init before using adc2.
    printf("adc2_init...\n");
    TEST_ESP_OK(adc2_config_channel_atten(ADC_TEST_CHANNEL_NUM, ADC_TEST_ATTEN));

    vTaskDelay(2 * portTICK_PERIOD_MS);

    printf("start conversion.\n");
    int output_data = 0;
    int read_raw = 0, read_old = 0;
    for (int i = 0; i < DAC_OUT_TIMES; i++) {
        TEST_ESP_OK(dac_output_voltage(DAC_TEST_CHANNEL_NUM, output_data));
        output_data += DAC_OUT_STEP;
        vTaskDelay(2 * portTICK_PERIOD_MS);
        TEST_ESP_OK(adc2_get_raw(ADC_TEST_CHANNEL_NUM, ADC_TEST_WIDTH, &read_raw));
        ESP_LOGI(TAG, "DAC: %d - ADC: %d", output_data, read_raw);
        if (read_old != 0) {
            TEST_ASSERT_GREATER_THAN(read_old, read_raw);
        }
        read_old = read_raw;
    }
    TEST_ESP_OK(dac_output_disable(DAC_TEST_CHANNEL_NUM));
}

TEST_CASE("DAC_cw_generator_output(RTC)_check_by_adc", "[dac_legacy]")
{
    gpio_num_t adc_gpio_num, dac_gpio_num;

    TEST_ESP_OK(adc2_pad_get_io_num(ADC_TEST_CHANNEL_NUM, &adc_gpio_num));
    TEST_ESP_OK(dac_pad_get_io_num(DAC_TEST_CHANNEL_NUM, &dac_gpio_num));

    printf("Please connect ADC2 CH%d-GPIO%d <--> DAC CH%d-GPIO%d.\n", ADC_TEST_CHANNEL_NUM, adc_gpio_num,
           DAC_TEST_CHANNEL_NUM + 1, dac_gpio_num);

    dac_cw_config_t cw = {
        .en_ch = DAC_TEST_CHANNEL_NUM,
        .scale = DAC_CW_SCALE_2,
        .phase = DAC_CW_PHASE_0,
        .freq = 1000,
#if CONFIG_IDF_TARGET_ESP32
        .offset = 64,
#elif CONFIG_IDF_TARGET_ESP32S2
        .offset = 16,
#endif
    };
    TEST_ESP_OK(dac_cw_generator_config(&cw));
    TEST_ESP_OK(dac_cw_generator_enable());
    TEST_ESP_OK(dac_output_enable(DAC_TEST_CHANNEL_NUM));

    //be sure to do the init before using adc2.
    printf("adc2_init...\n");
    TEST_ESP_OK(adc2_config_channel_atten(ADC_TEST_CHANNEL_NUM, ADC_TEST_ATTEN));

    vTaskDelay(2 * portTICK_PERIOD_MS);

    printf("start conversion.\n");
    int read_raw[3] = {0};
    for (int i = 0; i < DAC_TEST_TIMES; i++) {
        vTaskDelay(10 * portTICK_PERIOD_MS);
        TEST_ESP_OK(adc2_get_raw(ADC_TEST_CHANNEL_NUM, ADC_TEST_WIDTH, &read_raw[0]));
        ESP_LOGI(TAG, "ADC: %d", read_raw[0]);
        read_raw[2] = read_raw[1];
        read_raw[1] = read_raw[0];
    }

    TEST_ESP_OK(dac_cw_generator_disable());
    TEST_ESP_OK(dac_output_disable(DAC_TEST_CHANNEL_NUM));
}

#if CONFIG_IDF_TARGET_ESP32S2
static int helper_calc_dac_output(int mV)
{
    return mV * 0.07722;
}
static bool subtest_adc_dac(int mV_ref, esp_adc_cal_characteristics_t * chars)
{
    dac_output_voltage(DAC_TEST_CHANNEL_NUM, helper_calc_dac_output(mV_ref));
    vTaskDelay(pdMS_TO_TICKS(80));
    int raw;
    adc2_get_raw((adc2_channel_t)ADC_TEST_CHANNEL_NUM, ADC_WIDTH_BIT_13, &raw);
    uint32_t voltage = esp_adc_cal_raw_to_voltage(raw, chars);
    TEST_ASSERT_INT_WITHIN(200, mV_ref, voltage);   // 200 mV error allowance, because both DAC and ADC have error
    return true;
}

TEST_CASE("esp32s2_adc2-dac_with_adc2_calibration", "[dac_legacy]")
{
    gpio_num_t adc_gpio_num, dac_gpio_num;
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) != ESP_OK) {
        TEST_IGNORE_MESSAGE("Warning: This esp32s2 board does not support calibration. This test will be skipped.\n");
    }
    TEST_ESP_OK(adc2_pad_get_io_num(ADC_TEST_CHANNEL_NUM, &adc_gpio_num));
    TEST_ESP_OK(dac_pad_get_io_num(DAC_TEST_CHANNEL_NUM, &dac_gpio_num));
    printf("Please connect ADC2 CH%d-GPIO%d <--> DAC CH%d-GPIO%d.\n", ADC_TEST_CHANNEL_NUM, adc_gpio_num,
           DAC_TEST_CHANNEL_NUM + 1, dac_gpio_num);
    TEST_ESP_OK(dac_output_enable(DAC_TEST_CHANNEL_NUM));

    esp_adc_cal_characteristics_t chars;

    printf("Test 0dB atten...\n");
    adc2_config_channel_atten((adc2_channel_t)ADC_TEST_CHANNEL_NUM, ADC_ATTEN_DB_0);
    esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_13, 0, &chars);
    printf("a %"PRIu32", b %"PRIu32"\n", chars.coeff_a, chars.coeff_b);
    subtest_adc_dac(750, &chars);

    printf("Test 2.5dB atten...\n");
    adc2_config_channel_atten((adc2_channel_t)ADC_TEST_CHANNEL_NUM, ADC_ATTEN_DB_2_5);
    esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_DB_2_5, ADC_WIDTH_BIT_13, 0, &chars);
    printf("a %"PRIu32", b %"PRIu32"\n", chars.coeff_a, chars.coeff_b);
    subtest_adc_dac(1100, &chars);

    printf("Test 6dB atten...\n");
    adc2_config_channel_atten((adc2_channel_t)ADC_TEST_CHANNEL_NUM, ADC_ATTEN_DB_6);
    esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_DB_6, ADC_WIDTH_BIT_13, 0, &chars);
    printf("a %"PRIu32", b %"PRIu32"\n", chars.coeff_a, chars.coeff_b);
    subtest_adc_dac(800, &chars);
    subtest_adc_dac(1250, &chars);

    printf("Test 11dB atten...\n");
    adc2_config_channel_atten((adc2_channel_t)ADC_TEST_CHANNEL_NUM, ADC_ATTEN_DB_12);
    esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_13, 0, &chars);
    printf("a %"PRIu32", b %"PRIu32"\n", chars.coeff_a, chars.coeff_b);
    subtest_adc_dac(1500, &chars);
    subtest_adc_dac(2500, &chars);
}
#endif
