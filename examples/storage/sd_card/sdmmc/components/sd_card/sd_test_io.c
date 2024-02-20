/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"
#include "esp_cpu.h"
#include "esp_log.h"
#include "sd_test_io.h"

const static char *TAG = "SD_TEST";

#define ADC_ATTEN_DB              ADC_ATTEN_DB_12
#define GPIO_INPUT_PIN_SEL(pin)   (1ULL<<pin)

#if CONFIG_EXAMPLE_ENABLE_ADC_FEATURE
static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .chan = channel,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif //ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif //ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED

    *out_handle = handle;
    if (ret != ESP_OK) {
        if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
            ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
        } else {
            ESP_LOGE(TAG, "Invalid arg or no memory");
        }
    }

    return calibrated;
}

static void example_adc_calibration_deinit(adc_cali_handle_t handle)
{
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(handle));

#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
#endif
}

static float get_pin_voltage(int i, adc_oneshot_unit_handle_t adc_handle, bool do_calibration, adc_cali_handle_t adc_cali_handle)
{
    int voltage = 0;
    int val;
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, i, &config));

    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, i, &val));
    if (do_calibration) {
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc_cali_handle, val, &voltage));
    }

    return (float)voltage/1000;
}
#endif //CONFIG_EXAMPLE_ENABLE_ADC_FEATURE

static uint32_t get_cycles_until_pin_level(int i, int level, int timeout) {
    uint32_t start = esp_cpu_get_cycle_count();
    while(gpio_get_level(i) == !level && esp_cpu_get_cycle_count() - start < timeout) {
        ;
    }
    uint32_t end = esp_cpu_get_cycle_count();
    return end - start;
}

void check_sd_card_pins(pin_configuration_t *config, const int pin_count)
{
    ESP_LOGI(TAG, "Testing SD pin connections and pullup strength");
    gpio_config_t io_conf = {};
    for (int i = 0; i < pin_count; ++i) {
        io_conf.intr_type = GPIO_INTR_DISABLE;
        io_conf.mode = GPIO_MODE_INPUT_OUTPUT_OD;
        io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL(config->pins[i]);
        io_conf.pull_down_en = 0;
        io_conf.pull_up_en = 0;
        gpio_config(&io_conf);
    }

    printf("\n**** PIN recovery time ****\n\n");

    for (int i = 0; i < pin_count; ++i) {
        gpio_set_direction(config->pins[i], GPIO_MODE_INPUT_OUTPUT_OD);
        gpio_set_level(config->pins[i], 0);
        usleep(100);
        gpio_set_level(config->pins[i], 1);
        uint32_t cycles = get_cycles_until_pin_level(config->pins[i], 1, 10000);
        printf("PIN %2d %3s  %"PRIu32" cycles\n", config->pins[i], config->names[i], cycles);
    }

    printf("\n**** PIN recovery time with weak pullup ****\n\n");

    for (int i = 0; i < pin_count; ++i) {
        gpio_set_direction(config->pins[i], GPIO_MODE_INPUT_OUTPUT_OD);
        gpio_pullup_en(config->pins[i]);
        gpio_set_level(config->pins[i], 0);
        usleep(100);
        gpio_set_level(config->pins[i], 1);
        uint32_t cycles = get_cycles_until_pin_level(config->pins[i], 1, 10000);
        printf("PIN %2d %3s  %"PRIu32" cycles\n", config->pins[i], config->names[i], cycles);
        gpio_pullup_dis(config->pins[i]);
    }

#if CONFIG_EXAMPLE_ENABLE_ADC_FEATURE

    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = CONFIG_EXAMPLE_ADC_UNIT,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    adc_cali_handle_t *adc_cali_handle = (adc_cali_handle_t *)malloc(sizeof(adc_cali_handle_t)*pin_count);
    bool *do_calibration = (bool *)malloc(sizeof(bool)*pin_count);

    for (int i = 0; i < pin_count; i++) {
        do_calibration[i] = adc_calibration_init(CONFIG_EXAMPLE_ADC_UNIT, i, ADC_ATTEN_DB, &adc_cali_handle[i]);
    }

    printf("\n**** PIN voltage levels ****\n\n");

    for (int i = 0; i < pin_count; ++i) {
        float voltage = get_pin_voltage(config->adc_channels[i], adc_handle, do_calibration[i], adc_cali_handle[i]);
        printf("PIN %2d %3s  %.1fV\n", config->pins[i], config->names[i], voltage);
    }

    printf("\n**** PIN voltage levels with weak pullup ****\n\n");

    for (int i = 0; i < pin_count; ++i) {
        gpio_pullup_en(config->pins[i]);
        usleep(100);
        float voltage = get_pin_voltage(config->adc_channels[i], adc_handle, do_calibration[i], adc_cali_handle[i]);
        printf("PIN %2d %3s  %.1fV\n", config->pins[i], config->names[i], voltage);
        gpio_pullup_dis(config->pins[i]);
    }

    printf("\n**** PIN cross-talk ****\n\n");

    printf("              ");
    for (int i = 0; i < pin_count; ++i) {
        printf("%3s   ", config->names[i]);
    }
    printf("\n");
    for (int i = 0; i < pin_count; ++i) {
        gpio_set_direction(config->pins[i], GPIO_MODE_INPUT_OUTPUT_OD);
        gpio_set_level(config->pins[i], 0);
        printf("PIN %2d %3s    ", config->pins[i], config->names[i]);
        for (int j = 0; j < pin_count; ++j) {
            if (j == i) {
                printf(" --   ");
                continue;
            }
            usleep(100);
            float voltage = get_pin_voltage(config->adc_channels[j], adc_handle, do_calibration[i], adc_cali_handle[i]);
            printf("%1.1fV  ", voltage);
        }
        printf("\n");
        gpio_set_direction(config->pins[i], GPIO_MODE_INPUT);
    }

    printf("\n**** PIN cross-talk with weak pullup ****\n\n");

    printf("              ");
    for (int i = 0; i < pin_count; ++i) {
        printf("%3s   ", config->names[i]);
    }
    printf("\n");
    for (int i = 0; i < pin_count; ++i) {
        gpio_set_direction(config->pins[i], GPIO_MODE_INPUT_OUTPUT_OD);
        gpio_set_level(config->pins[i], 0);
        printf("PIN %2d %3s    ", config->pins[i], config->names[i]);
        for (int j = 0; j < pin_count; ++j) {
            if (j == i) {
                printf(" --   ");
                continue;
            }
            gpio_pullup_en(config->pins[j]);
            usleep(100);
            float voltage = get_pin_voltage(config->adc_channels[j], adc_handle, do_calibration[i], adc_cali_handle[i]);
            printf("%1.1fV  ", voltage);
            gpio_pullup_dis(config->pins[j]);
        }
        printf("\n");
        gpio_set_direction(config->pins[i], GPIO_MODE_INPUT);
    }

    for (int i = 0; i < pin_count; ++i) {
        if (do_calibration[i]) {
            example_adc_calibration_deinit(adc_cali_handle[i]);
        }
    }
#endif //CONFIG_EXAMPLE_ENABLE_ADC_FEATURE
}
