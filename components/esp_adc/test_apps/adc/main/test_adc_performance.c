/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unity.h"
#include "test_utils.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_cpu.h"
#include "soc/adc_periph.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "test_common_adc.h"

#if CONFIG_IDF_TARGET_ESP32 ||  SOC_ADC_CALIBRATION_V1_SUPPORTED

__attribute__((unused)) static const char *TAG = "TEST_ADC";

/*---------------------------------------------------------------
        ADC Oneshot Average / STD_Deviation Test
---------------------------------------------------------------*/
#define TEST_COUNT               (1<<SOC_ADC_RTC_MAX_BITWIDTH)
#define MAX_ARRAY_SIZE           (1<<SOC_ADC_RTC_MAX_BITWIDTH)

static int s_adc_count[MAX_ARRAY_SIZE]={};
static int s_adc_offset = -1;

static int s_insert_point(uint32_t value)
{
    const bool fixed_size = true;

    if (s_adc_offset < 0) {
        if (fixed_size) {
            TEST_ASSERT_GREATER_OR_EQUAL(4096, MAX_ARRAY_SIZE);
            s_adc_offset = 0;   //Fixed to 0 because the array can hold all the data in 12 bits
        } else {
            s_adc_offset = MAX((int)value - MAX_ARRAY_SIZE/2, 0);
        }
    }

    if (!fixed_size && (value < s_adc_offset || value >= s_adc_offset + MAX_ARRAY_SIZE)) {
        TEST_ASSERT_GREATER_OR_EQUAL(s_adc_offset, value);
        TEST_ASSERT_LESS_THAN(s_adc_offset + MAX_ARRAY_SIZE, value);
    }

    s_adc_count[value - s_adc_offset] ++;
    return value - s_adc_offset;
}

static void s_reset_array(void)
{
    memset(s_adc_count, 0, sizeof(s_adc_count));
    s_adc_offset = -1;
}

static uint32_t s_get_average(void)
{
    uint32_t sum = 0;
    int count = 0;
    for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
        sum += s_adc_count[i] * (s_adc_offset+i);
        count += s_adc_count[i];
    }
    return sum/count;
}

static void s_print_summary(bool figure)
{
    const int MAX_WIDTH=20;
    int max_count = 0;
    int start = -1;
    int end = -1;
    uint32_t sum = 0;
    int count = 0;
    for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
        if (s_adc_count[i] > max_count) {
            max_count = s_adc_count[i];
        }
        if (s_adc_count[i] > 0 && start < 0) {
            start = i;
        }
        if (s_adc_count[i] > 0) {
            end = i;
        }
        count += s_adc_count[i];
        sum += s_adc_count[i] * (s_adc_offset+i);
    }

    if (figure) {
        for (int i = start; i <= end; i++) {
            printf("%4d ", i+s_adc_offset);
            int count = s_adc_count[i] * MAX_WIDTH / max_count;
            for (int j = 0; j < count; j++) {
                putchar('|');
            }
            printf("    %d\n", s_adc_count[i]);
        }
    }
    float average = (float)sum/count;

    float variation_square = 0;
    for (int i = start; i <= end; i ++) {
        if (s_adc_count[i] == 0) {
            continue;
        }
        float delta = i + s_adc_offset - average;
        variation_square += (delta * delta) * s_adc_count[i];
    }

    printf("%d points.\n", count);
    printf("average: %.1f\n", (float)sum/count);
    printf("std: %.2f\n", sqrt(variation_square/count));
}

#if CONFIG_IDF_TARGET_ESP32
#define TEST_STD_ADC1_CHANNEL0    ADC_CHANNEL_6
#else
#define TEST_STD_ADC1_CHANNEL0    ADC_CHANNEL_2
#endif

TEST_CASE("ADC1 oneshot raw average / std_deviation", "[adc_oneshot][ignore][manual]")
{
    adc_channel_t channel = TEST_STD_ADC1_CHANNEL0;
    int raw = 0;
    bool print_figure = false;

    //-------------ADC1 Init---------------//
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    TEST_ESP_OK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    //-------------ADC Channel Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = SOC_ADC_RTC_MAX_BITWIDTH,
    };

    //-------------ADC Calibration Init---------------//
    bool do_calibration = false;
    adc_cali_handle_t cali_handle[TEST_ATTEN_NUMS] = {};
    for (int i = 0; i < TEST_ATTEN_NUMS; i++) {
        do_calibration = test_adc_calibration_init(ADC_UNIT_1, g_test_atten[i], ADC_BITWIDTH_DEFAULT, &cali_handle[i]);
    }
    if (!do_calibration) {
        ESP_LOGW(TAG, "calibration fail, jump calibration\n");
    }

    for (int i = 0; i < TEST_ATTEN_NUMS; i++) {

        //-------------ADC1 Channel Config---------------//
        config.atten = g_test_atten[i];
        TEST_ESP_OK(adc_oneshot_config_channel(adc1_handle, channel, &config));
        ESP_LOGI("TEST_ADC", "Test with atten: %d", g_test_atten[i]);

        while (1) {

            s_reset_array();

            for (int i = 0; i < TEST_COUNT; i++) {
                TEST_ESP_OK(adc_oneshot_read(adc1_handle, channel, &raw));
                s_insert_point(raw);
            }
            s_print_summary(print_figure);
            break;
        }

        if (do_calibration) {
            uint32_t raw = s_get_average();
            int voltage_mv = 0;
            TEST_ESP_OK(adc_cali_raw_to_voltage(cali_handle[i], raw, &voltage_mv));
            printf("Voltage = %d mV\n", voltage_mv);
        }
    }

    TEST_ESP_OK(adc_oneshot_del_unit(adc1_handle));
    for (int i = 0; i < TEST_ATTEN_NUMS; i++) {
        if (cali_handle[i]) {
            test_adc_calibration_deinit(cali_handle[i]);
        }
    }
}


/*---------------------------------------------------------------
        ADC Calibration Speed
---------------------------------------------------------------*/
#define CPU_FREQ_MHZ                    CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ

#define RECORD_TIME_PREPARE() uint32_t __t1, __t2
#define RECORD_TIME_START()             do {__t1 = esp_cpu_get_cycle_count();}while(0)
#define RECORD_TIME_END(p_time)         do{__t2 = esp_cpu_get_cycle_count(); *p_time = (__t2-__t1);}while(0)
#define GET_US_BY_CCOUNT(t)             ((double)t/CPU_FREQ_MHZ)


//ADC Channels
#if CONFIG_IDF_TARGET_ESP32
#define ADC1_CALI_SPEED_TEST_CHAN0            ADC_CHANNEL_6
#define ADC2_CALI_SPEED_TEST_CHAN0            ADC_CHANNEL_0
#else
#define ADC1_CALI_SPEED_TEST_CHAN0            ADC_CHANNEL_2
#define ADC2_CALI_SPEED_TEST_CHAN0            ADC_CHANNEL_0
#endif

#define TIMES_PER_ATTEN                 10


static IRAM_ATTR NOINLINE_ATTR uint32_t get_cali_time_in_ccount(adc_cali_handle_t cali_handle, int adc_raw)
{
    uint32_t time;
    int voltage = 0;
    RECORD_TIME_PREPARE();
    RECORD_TIME_START();
    adc_cali_raw_to_voltage(cali_handle, adc_raw, &voltage);
    RECORD_TIME_END(&time);

    return time;
}

static void s_adc_cali_speed(adc_unit_t unit_id, adc_channel_t channel)
{
    //-------------ADC Calibration Init---------------//
    bool do_calibration = false;
    adc_cali_handle_t cali_handle[TEST_ATTEN_NUMS] = {};
    for (int i = 0; i < TEST_ATTEN_NUMS; i++) {
        do_calibration = test_adc_calibration_init(unit_id, g_test_atten[i], SOC_ADC_RTC_MAX_BITWIDTH, &cali_handle[i]);
    }

    if (!do_calibration) {
        ESP_LOGW(TAG, "no efuse burnt, jump test");
    } else {

        ESP_LOGI(TAG, "CPU FREQ is %dMHz", CPU_FREQ_MHZ);
        uint32_t adc_time_record[4][TIMES_PER_ATTEN] = {};
        int adc_raw = 0;

        //-------------ADC Init---------------//
        adc_oneshot_unit_handle_t adc_handle;
        adc_oneshot_unit_init_cfg_t init_config = {
            .unit_id = unit_id,
            .ulp_mode = ADC_ULP_MODE_DISABLE,
        };
        TEST_ESP_OK(adc_oneshot_new_unit(&init_config, &adc_handle));

        //-------------ADC Channel Config---------------//
        adc_oneshot_chan_cfg_t config = {
            .bitwidth = SOC_ADC_RTC_MAX_BITWIDTH,
        };

        //atten0 ~ atten3
        for (int i = 0; i < TEST_ATTEN_NUMS; i++) {

            //-------------ADC Channel Config---------------//
            config.atten = g_test_atten[i];
            TEST_ESP_OK(adc_oneshot_config_channel(adc_handle, channel, &config));
            ESP_LOGI("TEST_ADC", "Test with atten: %d", g_test_atten[i]);

            for (int j = 0; j < TIMES_PER_ATTEN; j++) {
                TEST_ESP_OK(adc_oneshot_read(adc_handle, channel, &adc_raw));
                adc_time_record[i][j] = get_cali_time_in_ccount(cali_handle[i], adc_raw);
                IDF_LOG_PERFORMANCE("ADC1 Cali time", "%d us", (int)GET_US_BY_CCOUNT(adc_time_record[i][j]));
            }
        }

        TEST_ESP_OK(adc_oneshot_del_unit(adc_handle));
        for (int i = 0; i < TEST_ATTEN_NUMS; i++) {
            if (cali_handle[i]) {
                test_adc_calibration_deinit(cali_handle[i]);
            }
        }
    }
}

TEST_CASE("ADC1 Calibration Speed", "[adc][ignore][manual]")
{
    s_adc_cali_speed(ADC_UNIT_1, ADC1_CALI_SPEED_TEST_CHAN0);
}

#if (SOC_ADC_PERIPH_NUM >= 2)
TEST_CASE("ADC2 Calibration Speed", "[adc][ignore][manual]")
{
    s_adc_cali_speed(ADC_UNIT_2, ADC2_CALI_SPEED_TEST_CHAN0);
}
#endif  //#if (SOC_ADC_PERIPH_NUM >= 2)

#endif  //#if CONFIG_IDF_TARGET_ESP32 ||  SOC_ADC_CALIBRATION_V1_SUPPORTED
