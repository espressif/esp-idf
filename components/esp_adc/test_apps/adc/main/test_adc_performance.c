/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_filter.h"
#include "test_common_adc.h"
#include "idf_performance.h"

__attribute__((unused)) static const char *TAG = "TEST_ADC";

#if CONFIG_IDF_TARGET_ESP32
#define TEST_STD_ADC1_CHANNEL0    ADC_CHANNEL_5
#else
#define TEST_STD_ADC1_CHANNEL0    ADC_CHANNEL_2
#endif

static int s_adc_count_size;
static int *s_p_adc_count;
static int s_adc_offset = -1;

static int s_insert_point(uint32_t value)
{
    const bool fixed_size = true;

    if (s_adc_offset < 0) {
        if (fixed_size) {
            TEST_ASSERT_GREATER_OR_EQUAL(4096, s_adc_count_size);
            s_adc_offset = 0;   //Fixed to 0 because the array can hold all the data in 12 bits
        } else {
            s_adc_offset = MAX((int)value - s_adc_count_size / 2, 0);
        }
    }

    if (!fixed_size && (value < s_adc_offset || value >= s_adc_offset + s_adc_count_size)) {
        TEST_ASSERT_GREATER_OR_EQUAL(s_adc_offset, value);
        TEST_ASSERT_LESS_THAN(s_adc_offset + s_adc_count_size, value);
    }

    s_p_adc_count[value - s_adc_offset] ++;
    return value - s_adc_offset;
}

static void s_reset_array(int array_size)
{
    s_adc_count_size = array_size;
    s_p_adc_count = (int *)heap_caps_calloc(1, s_adc_count_size * sizeof(int), MALLOC_CAP_INTERNAL);
    TEST_ASSERT(s_p_adc_count);
    s_adc_offset = -1;
}

static void s_destroy_array(void)
{
    free(s_p_adc_count);
    s_p_adc_count = NULL;
    s_adc_count_size = 0;
}

__attribute__((unused))
static uint32_t s_get_average(void)
{
    uint32_t sum = 0;
    int count = 0;
    for (int i = 0; i < s_adc_count_size; i++) {
        sum += s_p_adc_count[i] * (s_adc_offset + i);
        count += s_p_adc_count[i];
    }
    return sum / count;
}

static float s_print_summary(bool figure)
{
    const int MAX_WIDTH = 20;
    int max_count = 0;
    int start = -1;
    int end = -1;
    uint32_t sum = 0;
    int count = 0;
    for (int i = 0; i < s_adc_count_size; i++) {
        if (s_p_adc_count[i] > max_count) {
            max_count = s_p_adc_count[i];
        }
        if (s_p_adc_count[i] > 0 && start < 0) {
            start = i;
        }
        if (s_p_adc_count[i] > 0) {
            end = i;
        }
        count += s_p_adc_count[i];
        sum += s_p_adc_count[i] * (s_adc_offset + i);
    }

    if (figure) {
        for (int i = start; i <= end; i++) {
            printf("%4d ", i + s_adc_offset);
            int count = s_p_adc_count[i] * MAX_WIDTH / max_count;
            for (int j = 0; j < count; j++) {
                putchar('|');
            }
            printf("    %d\n", s_p_adc_count[i]);
        }
    }
    float average = (float)sum / count;

    float variation_square = 0;
    for (int i = start; i <= end; i ++) {
        if (s_p_adc_count[i] == 0) {
            continue;
        }
        float delta = i + s_adc_offset - average;
        variation_square += (delta * delta) * s_p_adc_count[i];
    }

    printf("%d points.\n", count);
    printf("average: %.1f\n", (float)sum / count);
    printf("std: %.2f\n", sqrt(variation_square / count));

    return sqrt(variation_square / count);
}

#if SOC_ADC_DMA_SUPPORTED
/*---------------------------------------------------------------
        ADC Continuous Average / STD_Deviation Test
---------------------------------------------------------------*/
#if (SOC_ADC_DIGI_RESULT_BYTES == 2)
#define ADC_TEST_OUTPUT_TYPE                ADC_DIGI_OUTPUT_FORMAT_TYPE1
#define EXAMPLE_ADC_GET_CHANNEL(result)     ((result).type1.channel)
#define EXAMPLE_ADC_GET_DATA(result)        ((result).type1.data)
#else
#define ADC_TEST_OUTPUT_TYPE                ADC_DIGI_OUTPUT_FORMAT_TYPE2
#define EXAMPLE_ADC_GET_CHANNEL(result)     ((result).type2.channel)
#define EXAMPLE_ADC_GET_DATA(result)        ((result).type2.data)
#endif
#define ADC_TEST_FREQ_HZ        (50 * 1000)
#define ADC_TEST_PKG_SIZE       512
#define ADC_TEST_CNT            4096

static bool IRAM_ATTR s_conv_done_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
{
    BaseType_t mustYield = pdFALSE;
    TaskHandle_t task_handle = *(TaskHandle_t *)(user_data);
    //Notify that ADC continuous driver has done enough number of conversions
    vTaskNotifyGiveFromISR(task_handle, &mustYield);

    return (mustYield == pdTRUE);
}

static float test_adc_continuous_std(adc_atten_t atten, bool filter_en, int filter_coeff, bool is_performance_test)
{
    uint8_t *result = heap_caps_calloc(1, ADC_TEST_CNT * SOC_ADC_DIGI_RESULT_BYTES, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_32BIT);
    TEST_ASSERT(result);
    bool print_figure = false;
    TaskHandle_t task_handle = xTaskGetCurrentTaskHandle();

    //-------------ADC Init---------------//
    adc_continuous_handle_t handle = NULL;
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = 4096,
        .conv_frame_size = ADC_TEST_PKG_SIZE,
    };
    TEST_ESP_OK(adc_continuous_new_handle(&adc_config, &handle));

    adc_continuous_evt_cbs_t cbs = {
        .on_conv_done = s_conv_done_cb,
    };
    TEST_ESP_OK(adc_continuous_register_event_callbacks(handle, &cbs, &task_handle));

    //-------------ADC Config---------------//
    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = ADC_TEST_FREQ_HZ,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_TEST_OUTPUT_TYPE,
    };
    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    adc_pattern[0].atten = atten;
    adc_pattern[0].channel = TEST_STD_ADC1_CHANNEL0;
    adc_pattern[0].unit = ADC_UNIT_1;
    adc_pattern[0].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
    dig_cfg.adc_pattern = adc_pattern;
    dig_cfg.pattern_num = 1;
    TEST_ESP_OK(adc_continuous_config(handle, &dig_cfg));

#if SOC_ADC_DIG_IIR_FILTER_SUPPORTED
    adc_iir_filter_handle_t filter_hdl = NULL;
    if (filter_en) {
        adc_continuous_iir_filter_config_t filter_config = {
            .unit = ADC_UNIT_1,
            .channel = TEST_STD_ADC1_CHANNEL0,
            .coeff = filter_coeff,
        };
        TEST_ESP_OK(adc_new_continuous_iir_filter(handle, &filter_config, &filter_hdl));
        TEST_ESP_OK(adc_continuous_iir_filter_enable(filter_hdl));
    }
#endif

    if (is_performance_test) {
        test_adc_set_io_middle(ADC_UNIT_1, TEST_STD_ADC1_CHANNEL0);
    }

    if (filter_en) {
        ESP_LOGI("TEST_ADC", "Test with atten: %d, filter coeff: %d", atten, filter_coeff);
    } else {
        ESP_LOGI("TEST_ADC", "Test with atten: %d, no filter", atten);
    }

    s_reset_array((1 << SOC_ADC_DIGI_MAX_BITWIDTH));
    TEST_ESP_OK(adc_continuous_start(handle));

    int remain_count = ADC_TEST_CNT;
    while (remain_count) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        int already_got = ADC_TEST_CNT - remain_count;
        uint32_t ret_num = 0;
        TEST_ESP_OK(adc_continuous_read(handle, result + already_got * SOC_ADC_DIGI_RESULT_BYTES, ADC_TEST_PKG_SIZE, &ret_num, 0));
        remain_count -= ret_num / SOC_ADC_DIGI_RESULT_BYTES;
    }

    adc_digi_output_data_t *p = (void*)result;
    for (int i = 0; i < ADC_TEST_CNT; i++) {
        TEST_ASSERT_EQUAL(TEST_STD_ADC1_CHANNEL0, EXAMPLE_ADC_GET_CHANNEL(p[i]));
        s_insert_point(EXAMPLE_ADC_GET_DATA(p[i]));
    }
    float std = s_print_summary(print_figure);

    TEST_ESP_OK(adc_continuous_stop(handle));
#if SOC_ADC_DIG_IIR_FILTER_SUPPORTED
    if (filter_en) {
        TEST_ESP_OK(adc_continuous_iir_filter_disable(filter_hdl));
        TEST_ESP_OK(adc_del_continuous_iir_filter(filter_hdl));
    }
#endif
    TEST_ESP_OK(adc_continuous_deinit(handle));
    ulTaskNotifyTake(pdTRUE, 0);
    s_destroy_array();
    free(result);

    return std;
}

TEST_CASE("ADC1 continuous raw average and std_deviation", "[adc_continuous][manual]")
{
    for (int i = 0; i < TEST_ATTEN_NUMS; i ++) {
#if SOC_ADC_DIG_IIR_FILTER_SUPPORTED
        //filter disabled
        test_adc_continuous_std(g_test_atten[i], false, 0, false);
        //filter with different coeffs
        for (int j = 0; j < TEST_FILTER_COEFF_NUMS; j ++) {
            test_adc_continuous_std(g_test_atten[i], true, g_test_filter_coeff[j], false);
        }
#else
        //filter disabled
        test_adc_continuous_std(g_test_atten[i], false, 0, false);
#endif
    }
}

TEST_CASE("ADC1 continuous std deviation performance, no filter", "[adc_continuous][performance]")
{
    float std = test_adc_continuous_std(ADC_ATTEN_DB_12, false, 0, true);
    TEST_PERFORMANCE_LESS_THAN(ADC_CONTINUOUS_STD_ATTEN3_NO_FILTER, "%.2f", std);
}

#if SOC_ADC_DIG_IIR_FILTER_SUPPORTED
TEST_CASE("ADC1 continuous std deviation performance, with filter", "[adc_continuous][performance]")
{
    float std = test_adc_continuous_std(ADC_ATTEN_DB_12, false, 0, true);
    TEST_PERFORMANCE_LESS_THAN(ADC_CONTINUOUS_STD_ATTEN3_NO_FILTER, "%.2f", std);

    std = test_adc_continuous_std(ADC_ATTEN_DB_12, true, ADC_DIGI_IIR_FILTER_COEFF_2, true);
    TEST_PERFORMANCE_LESS_THAN(ADC_CONTINUOUS_STD_ATTEN3_FILTER_2, "%.2f", std);

    std = test_adc_continuous_std(ADC_ATTEN_DB_12, true, ADC_DIGI_IIR_FILTER_COEFF_4, true);
    TEST_PERFORMANCE_LESS_THAN(ADC_CONTINUOUS_STD_ATTEN3_FILTER_4, "%.2f", std);

    std = test_adc_continuous_std(ADC_ATTEN_DB_12, true, ADC_DIGI_IIR_FILTER_COEFF_8, true);
    TEST_PERFORMANCE_LESS_THAN(ADC_CONTINUOUS_STD_ATTEN3_FILTER_8, "%.2f", std);

    std = test_adc_continuous_std(ADC_ATTEN_DB_12, true, ADC_DIGI_IIR_FILTER_COEFF_16, true);
    TEST_PERFORMANCE_LESS_THAN(ADC_CONTINUOUS_STD_ATTEN3_FILTER_16, "%.2f", std);

    std = test_adc_continuous_std(ADC_ATTEN_DB_12, true, ADC_DIGI_IIR_FILTER_COEFF_64, true);
    TEST_PERFORMANCE_LESS_THAN(ADC_CONTINUOUS_STD_ATTEN3_FILTER_64, "%.2f", std);
}
#endif  //#if SOC_ADC_DIG_IIR_FILTER_SUPPORTED
#endif  //#if SOC_ADC_DMA_SUPPORTED

#if CONFIG_IDF_TARGET_ESP32 ||  SOC_ADC_CALIBRATION_V1_SUPPORTED
/*---------------------------------------------------------------
        ADC Oneshot Average / STD_Deviation Test
---------------------------------------------------------------*/
static float test_adc_oneshot_std(adc_atten_t atten, bool is_performance_test)
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
    adc_cali_handle_t cali_handle = NULL;
    do_calibration = test_adc_calibration_init(ADC_UNIT_1, channel, atten, ADC_BITWIDTH_DEFAULT, &cali_handle);
    if (!do_calibration) {
        ESP_LOGW(TAG, "calibration fail, jump calibration");
    }

    //-------------ADC1 Channel Config---------------//
    config.atten = atten;
    TEST_ESP_OK(adc_oneshot_config_channel(adc1_handle, channel, &config));
    ESP_LOGI("TEST_ADC", "Test with atten: %d", atten);

    s_reset_array((1 << SOC_ADC_RTC_MAX_BITWIDTH));

    if (is_performance_test) {
        test_adc_set_io_middle(ADC_UNIT_1, TEST_STD_ADC1_CHANNEL0);
    }

    float std;
    while (1) {
        for (int i = 0; i < s_adc_count_size; i++) {
            TEST_ESP_OK(adc_oneshot_read(adc1_handle, channel, &raw));
            s_insert_point(raw);
        }
        std = s_print_summary(print_figure);
        break;
    }

    if (do_calibration) {
        uint32_t raw = s_get_average();
        int voltage_mv = 0;
        TEST_ESP_OK(adc_cali_raw_to_voltage(cali_handle, raw, &voltage_mv));
        printf("Voltage = %d mV\n", voltage_mv);
    }

    s_destroy_array();

    TEST_ESP_OK(adc_oneshot_del_unit(adc1_handle));
    if (cali_handle) {
        test_adc_calibration_deinit(cali_handle);
    }

    return std;
}

TEST_CASE("ADC1 oneshot raw average and std_deviation", "[adc_oneshot][manual]")
{
    for (int i = 0; i < TEST_ATTEN_NUMS; i++) {
        test_adc_oneshot_std(g_test_atten[i], false);
    }
}

TEST_CASE("ADC1 oneshot std_deviation performance", "[adc_oneshot][performance]")
{
    float std = test_adc_oneshot_std(ADC_ATTEN_DB_12, true);
    TEST_PERFORMANCE_LESS_THAN(ADC_ONESHOT_STD_ATTEN3, "%.2f", std);
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
        do_calibration = test_adc_calibration_init(unit_id, channel, g_test_atten[i], SOC_ADC_RTC_MAX_BITWIDTH, &cali_handle[i]);
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

TEST_CASE("ADC1 Calibration Speed", "[adc][manual]")
{
    s_adc_cali_speed(ADC_UNIT_1, ADC1_CALI_SPEED_TEST_CHAN0);
}

#if (SOC_ADC_PERIPH_NUM >= 2) && !CONFIG_IDF_TARGET_ESP32C3
//ESP32C3 ADC2 oneshot mode is not supported anymore
TEST_CASE("ADC2 Calibration Speed", "[adc][manual]")
{
    s_adc_cali_speed(ADC_UNIT_2, ADC2_CALI_SPEED_TEST_CHAN0);
}
#endif  //#if (SOC_ADC_PERIPH_NUM >= 2) && !CONFIG_IDF_TARGET_ESP32C3

#endif  //#if CONFIG_IDF_TARGET_ESP32 ||  SOC_ADC_CALIBRATION_V1_SUPPORTED
