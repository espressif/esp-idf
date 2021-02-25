// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <sys/param.h>
#include <string.h>
#include "test_utils.h"

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32, ESP32S2, ESP32S3)
//API only supported for C3 now.

#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"

#define TEST_COUNT      4096
#define MAX_ARRAY_SIZE  4096
#define TEST_ATTEN      ADC_ATTEN_MAX //Set to ADC_ATTEN_*db to test a single attenuation       only

static int s_adc_count[MAX_ARRAY_SIZE]={};
static int s_adc_offset = -1;

static int insert_point(uint32_t value)
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

static void reset_array(void)
{
    memset(s_adc_count, 0, sizeof(s_adc_count));
    s_adc_offset = -1;
}

static uint32_t get_average(void)
{
    uint32_t sum = 0;
    int count = 0;
    for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
        sum += s_adc_count[i] * (s_adc_offset+i);
        count += s_adc_count[i];
    }
    return sum/count;
}

static void print_summary(bool figure)
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

static void continuous_adc_init(uint16_t adc1_chan_mask, uint16_t adc2_chan_mask, adc_channel_t *channel, uint8_t channel_num, adc_atten_t atten)
{
    adc_digi_init_config_t adc_dma_config = {
        .max_store_buf_size = TEST_COUNT*2,
        .conv_num_each_intr = 128,
        .adc1_chan_mask = adc1_chan_mask,
        .adc2_chan_mask = adc2_chan_mask,
    };
    TEST_ESP_OK(adc_digi_initialize(&adc_dma_config));

    adc_digi_pattern_table_t adc_pattern[10] = {0};
    adc_digi_config_t dig_cfg = {
        .conv_limit_en = 0,
        .conv_limit_num = 250,
        .sample_freq_hz = 83333,
    };

    dig_cfg.adc_pattern_len = channel_num;
    for (int i = 0; i < channel_num; i++) {
        uint8_t unit = ((channel[i] >> 3) & 0x1);
        uint8_t ch = channel[i] & 0x7;
        adc_pattern[i].atten = atten;
        adc_pattern[i].channel = ch;
        adc_pattern[i].unit = unit;
    }
    dig_cfg.adc_pattern = adc_pattern;
    TEST_ESP_OK(adc_digi_controller_config(&dig_cfg));
}

TEST_CASE("test_adc_dma", "[adc][ignore][manual]")
{
    uint16_t adc1_chan_mask = BIT(2);
    uint16_t adc2_chan_mask = 0;
    adc_channel_t channel[1] = {ADC1_CHANNEL_2};
    adc_atten_t target_atten = TEST_ATTEN;

    const int output_data_size = sizeof(adc_digi_output_data_t);

    int buffer_size = TEST_COUNT*output_data_size;
    uint8_t* read_buf = malloc(buffer_size);
    TEST_ASSERT_NOT_NULL(read_buf);

    adc_atten_t atten;
    bool print_figure;
    if (target_atten == ADC_ATTEN_MAX) {
        atten = ADC_ATTEN_DB_0;
        target_atten = ADC_ATTEN_DB_11;
        print_figure = false;
    } else {
        atten = target_atten;
        print_figure = true;
    }

    while (1) {
        ESP_LOGI("TEST_ADC", "Test with atten: %d", atten);
        memset(read_buf, 0xce, buffer_size);

        bool do_calibration = false;

        esp_adc_cal_characteristics_t chan1_char = {};
        esp_adc_cal_value_t cal_ret = esp_adc_cal_characterize(ADC_UNIT_1, atten, ADC_WIDTH_12Bit, 0, &chan1_char);
        if (cal_ret == ESP_ADC_CAL_VAL_EFUSE_TP) {
            do_calibration = true;
        }

        continuous_adc_init(adc1_chan_mask, adc2_chan_mask, channel, sizeof(channel) / sizeof(adc_channel_t), atten);
        adc_digi_start();

        int remain_count = TEST_COUNT;
        while (remain_count) {
            int already_got = TEST_COUNT - remain_count;
            uint32_t ret_num;
            TEST_ESP_OK(adc_digi_read_bytes(read_buf + already_got*output_data_size,
                                    remain_count*output_data_size, &ret_num, ADC_MAX_DELAY));

            TEST_ASSERT((ret_num % output_data_size) == 0);
            remain_count -= ret_num / output_data_size;
        }

        adc_digi_output_data_t *p = (void*)read_buf;
        reset_array();
        for (int i = 0; i < TEST_COUNT; i++) {
            insert_point(p[i].type2.data);
        }

        print_summary(print_figure);

        if (do_calibration) {
            uint32_t raw = get_average();
            uint32_t voltage_mv = esp_adc_cal_raw_to_voltage(raw, &chan1_char);
            printf("Voltage = %d mV\n", voltage_mv);
        }

        adc_digi_stop();
        TEST_ESP_OK(adc_digi_deinitialize());

        if (atten == target_atten) {
            break;
        }

        atten++;
    }

    free(read_buf);
}

TEST_CASE("test_adc_single", "[adc][ignore][manual]")
{
    adc_atten_t target_atten = TEST_ATTEN;
    adc_atten_t atten;
    bool print_figure;
    if (target_atten == ADC_ATTEN_MAX) {
        atten = ADC_ATTEN_DB_0;
        target_atten = ADC_ATTEN_DB_11;
        print_figure = false;
    } else {
        atten = target_atten;
        print_figure = true;
    }

    adc1_config_width(ADC_WIDTH_BIT_12);


    while (1) {
        ESP_LOGI("TEST_ADC", "Test with atten: %d", atten);

        adc1_config_channel_atten(ADC1_CHANNEL_2, atten);

        bool do_calibration = false;

        esp_adc_cal_characteristics_t chan1_char = {};
        esp_adc_cal_value_t cal_ret = esp_adc_cal_characterize(ADC_UNIT_1, atten, ADC_WIDTH_12Bit, 0, &chan1_char);
        if (cal_ret == ESP_ADC_CAL_VAL_EFUSE_TP) {
            do_calibration = true;
        }


        const int test_count = TEST_COUNT;
        adc1_channel_t channel = ADC1_CHANNEL_2;
        while (1) {

            reset_array();

            for (int i = 0; i < test_count; i++) {
                uint32_t raw = adc1_get_raw(channel);
                insert_point(raw);
            }
            print_summary(print_figure);
            break;
        }

        if (do_calibration) {
            uint32_t raw = get_average();
            uint32_t voltage_mv = esp_adc_cal_raw_to_voltage(raw, &chan1_char);
            printf("Voltage = %d mV\n", voltage_mv);
        }


        if (atten == target_atten) {
            break;
        }
        atten++;
    }
}

#endif
