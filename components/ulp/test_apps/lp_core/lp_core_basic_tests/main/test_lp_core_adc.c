/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/adc_types.h"
#include "lp_core_test_app_adc.h"
#include "ulp_lp_core.h"
#include "ulp_lp_core_lp_adc_shared.h"
#include "soc/adc_periph.h"
#include "driver/gpio.h"
#include "driver/temperature_sensor.h"

#include "unity.h"

extern const uint8_t lp_core_main_adc_bin_start[] asm("_binary_lp_core_test_app_adc_bin_start");
extern const uint8_t lp_core_main_adc_bin_end[]   asm("_binary_lp_core_test_app_adc_bin_end");

#if CONFIG_IDF_TARGET_ESP32P4
// Threshold values picked up empirically after manual testing
#define ADC_TEST_LOW_VAL         1500
#define ADC_TEST_HIGH_VAL        2000
#else
#error "ADC threshold values not defined"
#endif

#define ADC_GET_IO_NUM(unit, channel) (adc_channel_io_map[unit][channel])

static void test_adc_set_io_level(adc_unit_t unit, adc_channel_t channel, bool level)
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
#endif
}

static void load_and_start_lp_core_firmware(ulp_lp_core_cfg_t* cfg, const uint8_t* firmware_start, const uint8_t* firmware_end)
{
    TEST_ASSERT(ulp_lp_core_load_binary(firmware_start,
                                        (firmware_end - firmware_start)) == ESP_OK);

    TEST_ASSERT(ulp_lp_core_run(cfg) == ESP_OK);

}

void test_lp_adc(adc_unit_t unit_id)
{
    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    load_and_start_lp_core_firmware(&cfg, lp_core_main_adc_bin_start, lp_core_main_adc_bin_end);

    /* LP ADC Init */
    ESP_ERROR_CHECK(lp_core_lp_adc_init(unit_id));

    /* LP ADC channel config */
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

    /* Set all the ADC channel IOs to low */
    test_adc_set_io_level(unit_id, ADC_CHANNEL_0, 0);
    test_adc_set_io_level(unit_id, ADC_CHANNEL_1, 0);
    test_adc_set_io_level(unit_id, ADC_CHANNEL_2, 0);
    test_adc_set_io_level(unit_id, ADC_CHANNEL_3, 0);
    test_adc_set_io_level(unit_id, ADC_CHANNEL_4, 0);
    test_adc_set_io_level(unit_id, ADC_CHANNEL_5, 0);
    test_adc_set_io_level(unit_id, ADC_CHANNEL_6, 0);
    test_adc_set_io_level(unit_id, ADC_CHANNEL_7, 0);

    vTaskDelay(10);

    int *adc_raw = (int *)&ulp_adc_raw;

    /* Verify that the LP ADC values reflect a low-state of the input pins */
    for (int i = 0; i < SOC_ADC_CHANNEL_NUM(unit_id); i++) {
        printf("LP ADC low[%d] = %d\n", i, adc_raw[i]);
        TEST_ASSERT_LESS_THAN_INT(ADC_TEST_LOW_VAL, adc_raw[i]);
    }

    /* Set all the ADC channel IOs to high */
    test_adc_set_io_level(unit_id, ADC_CHANNEL_0, 1);
    test_adc_set_io_level(unit_id, ADC_CHANNEL_1, 1);
    test_adc_set_io_level(unit_id, ADC_CHANNEL_2, 1);
    test_adc_set_io_level(unit_id, ADC_CHANNEL_3, 1);
    test_adc_set_io_level(unit_id, ADC_CHANNEL_4, 1);
    test_adc_set_io_level(unit_id, ADC_CHANNEL_5, 1);
    test_adc_set_io_level(unit_id, ADC_CHANNEL_6, 1);
    test_adc_set_io_level(unit_id, ADC_CHANNEL_7, 1);

    vTaskDelay(10);

    /* Verify that the LP ADC values reflect a high-state of the input pins */
    for (int i = 0; i < SOC_ADC_CHANNEL_NUM(unit_id); i++) {
        printf("LP ADC high[%d] = %d\n", i, adc_raw[i]);
        TEST_ASSERT_GREATER_THAN_INT(ADC_TEST_HIGH_VAL, adc_raw[i]);
    }

    /* Deinit LP ADC */
    ESP_ERROR_CHECK(lp_core_lp_adc_deinit(unit_id));
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
    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    load_and_start_lp_core_firmware(&cfg, lp_core_main_adc_bin_start, lp_core_main_adc_bin_end);

    for (int i = 0; i < 100; i++) {
        /* LP ADC Init */
        ESP_ERROR_CHECK(lp_core_lp_adc_init(unit_id));

        /* LP ADC channel config */
        const lp_core_lp_adc_chan_cfg_t config = {
            .atten = ADC_ATTEN_DB_12,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };

        TEST_ASSERT(lp_core_lp_adc_config_channel(unit_id, ADC_CHANNEL_0, &config) == ESP_OK);

        /* Set LP ADC channel IO and read raw value */
        test_adc_set_io_level(unit_id, ADC_CHANNEL_0, 1);
        vTaskDelay(10);
        int *adc_raw = (int *)&ulp_adc_raw;
        TEST_ASSERT_NOT_EQUAL(0, adc_raw[0]);

        /* De-init LP ADC */
        ESP_ERROR_CHECK(lp_core_lp_adc_deinit(unit_id));
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

    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    load_and_start_lp_core_firmware(&cfg, lp_core_main_adc_bin_start, lp_core_main_adc_bin_end);

    /* LP ADC Init */
    ESP_ERROR_CHECK(lp_core_lp_adc_init(ADC_UNIT_1));

    /* LP ADC channel config */
    const lp_core_lp_adc_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    /* Configure ADC channel 0 */
    TEST_ASSERT(lp_core_lp_adc_config_channel(ADC_UNIT_1, ADC_CHANNEL_0, &config) == ESP_OK);

    int *adc_raw = (int *)&ulp_adc_raw;
    cnt = 2;
    while (cnt--) {
        printf("LP ADC%d Channel[%d] Raw Data: %d\n", ADC_UNIT_1 + 1, 0, adc_raw[0]);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    TEST_ESP_OK(lp_core_lp_adc_deinit(ADC_UNIT_1));

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
