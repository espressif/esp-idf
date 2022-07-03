/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <sys/param.h>
#include <string.h>
#include "esp_log.h"
#include "test_utils.h"
#include "esp_adc_cal.h"
#include "driver/adc_common.h"
#include "esp_cpu.h"

__attribute__((unused)) static const char *TAG = "ADC";

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32, ESP32S2, ESP32S3, ESP32C3, ESP32C2)
//TODO: IDF-3160

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

#endif  //#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32, ESP32S2, ESP32S3, ESP32C3, ESP32C2)


#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2) //TODO IDF-3908

/********************************************************************************
 *      ADC Speed Related Tests
 ********************************************************************************/

#define RECORD_TIME_PREPARE() uint32_t __t1, __t2
#define RECORD_TIME_START()             do {__t1 = esp_cpu_get_ccount();}while(0)
#define RECORD_TIME_END(p_time)         do{__t2 = esp_cpu_get_ccount(); *p_time = (__t2-__t1);}while(0)
#define GET_US_BY_CCOUNT(t)             ((double)t/CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ)


//ADC Channels
#if CONFIG_IDF_TARGET_ESP32
#define ADC1_CALI_TEST_CHAN0            ADC1_CHANNEL_6
#define ADC2_CALI_TEST_CHAN0            ADC2_CHANNEL_0
#else
#define ADC1_CALI_TEST_CHAN0            ADC1_CHANNEL_2
#define ADC2_CALI_TEST_CHAN0            ADC2_CHANNEL_0
#endif

//ADC Calibration
#if CONFIG_IDF_TARGET_ESP32
#define ADC_TEST_CALI_SCHEME            ESP_ADC_CAL_VAL_EFUSE_VREF
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_TEST_CALI_SCHEME            ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32C3
#define ADC_TEST_CALI_SCHEME            ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32S3
#define ADC_TEST_CALI_SCHEME            ESP_ADC_CAL_VAL_EFUSE_TP_FIT
#endif

#define TIMES_PER_ATTEN                 10

static esp_adc_cal_characteristics_t adc1_chars;
static esp_adc_cal_characteristics_t adc2_chars;

static void adc_single_cali_init(adc_unit_t adc_n, adc_channel_t chan, uint32_t atten)
{
    esp_err_t ret;
    esp_adc_cal_value_t ret_val = ESP_ADC_CAL_VAL_NOT_SUPPORTED;

    ret = esp_adc_cal_check_efuse(ADC_TEST_CALI_SCHEME);
    if (ret == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGE(TAG, "Cali scheme not supported!");
        TEST_ASSERT(ret != ESP_ERR_NOT_SUPPORTED);
    } else if (ret != ESP_OK) {
        ESP_LOGW(TAG, "No cali eFuse, but will run the test");
    }

    if (adc_n == ADC_UNIT_1) {
        ret_val = esp_adc_cal_characterize(adc_n, atten, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
        TEST_ESP_OK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
        TEST_ESP_OK(adc1_config_channel_atten((adc1_channel_t)chan, atten));
    } else if (adc_n == ADC_UNIT_2) {
        TEST_ESP_OK(adc2_config_channel_atten((adc2_channel_t)chan, atten));
        ret_val = esp_adc_cal_characterize(adc_n, atten, ADC_WIDTH_BIT_DEFAULT, 0, &adc2_chars);
    }
    if (ret_val == ESP_ADC_CAL_VAL_NOT_SUPPORTED) {
        ESP_LOGW(TAG, "No cali eFuse, or invalid arg, but will run the test");
    }
    ESP_LOGI(TAG, "ADC%d, channel%d, atten%d", adc_n, chan, atten);
}

static IRAM_ATTR NOINLINE_ATTR uint32_t get_cali_time_in_ccount(uint32_t adc_raw, esp_adc_cal_characteristics_t *chars)
{
    uint32_t time;

    RECORD_TIME_PREPARE();
    RECORD_TIME_START();
    esp_adc_cal_raw_to_voltage(adc_raw, chars);
    RECORD_TIME_END(&time);

    return time;
}

TEST_CASE("test_adc_single_cali_time", "[adc][ignore][manual]")
{
    ESP_LOGI(TAG, "CPU FREQ is %dMHz", CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ);
    uint32_t adc1_time_record[4][TIMES_PER_ATTEN] = {};
    uint32_t adc2_time_record[4][TIMES_PER_ATTEN] = {};
    int adc1_raw = 0;
    int adc2_raw = 0;

    //atten0 ~ atten3
    for (int i = 0; i < 4; i++) {
        ESP_LOGI(TAG, "----------------atten%d----------------", i);
        adc_single_cali_init(ADC_UNIT_1, ADC1_CALI_TEST_CHAN0, i);
        adc_single_cali_init(ADC_UNIT_2, ADC2_CALI_TEST_CHAN0, i);

        for (int j = 0; j < TIMES_PER_ATTEN; j++) {

            adc1_raw = adc1_get_raw(ADC1_CALI_TEST_CHAN0);
            TEST_ESP_OK(adc2_get_raw(ADC2_CALI_TEST_CHAN0, ADC_WIDTH_BIT_DEFAULT, &adc2_raw));

            adc1_time_record[i][j] = get_cali_time_in_ccount(adc1_raw, &adc1_chars);
            adc2_time_record[i][j] = get_cali_time_in_ccount(adc2_raw, &adc2_chars);

            IDF_LOG_PERFORMANCE("ADC1 Cali time", "%d us", (int)GET_US_BY_CCOUNT(adc1_time_record[i][j]));
            IDF_LOG_PERFORMANCE("ADC2 Cali time", "%d us", (int)GET_US_BY_CCOUNT(adc2_time_record[i][j]));
        }
    }
}


/********************************************************************************
 *      ADC Single with Light Sleep
 ********************************************************************************/
#include <inttypes.h>
#include "esp_sleep.h"
#include "esp_private/regi2c_ctrl.h"
#if REGI2C_ANA_CALI_PD_WORKAROUND
#include "regi2c_saradc.h"
#endif

//ADC Channels
#if CONFIG_IDF_TARGET_ESP32
#define ADC1_SLEEP_TEST_CHAN          ADC1_CHANNEL_6
#define ADC2_SLEEP_TEST_CHAN          ADC2_CHANNEL_0
static const char *TAG_CH[2][10] = {{"ADC1_CH6"}, {"ADC2_CH0"}};
#else
#define ADC1_SLEEP_TEST_CHAN          ADC1_CHANNEL_2
#define ADC2_SLEEP_TEST_CHAN          ADC2_CHANNEL_0
static const char *TAG_CH[2][10] = {{"ADC1_CH2"}, {"ADC2_CH0"}};
#endif

//ADC Attenuation
#define ADC_SLEEP_TEST_ATTEN           ADC_ATTEN_DB_6

//ADC Calibration
#if CONFIG_IDF_TARGET_ESP32
#define ADC_SLEEP_TEST_CALI_SCHEME     ESP_ADC_CAL_VAL_EFUSE_VREF
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_SLEEP_TEST_CALI_SCHEME     ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32C3
#define ADC_SLEEP_TEST_CALI_SCHEME     ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32S3
#define ADC_SLEEP_TEST_CALI_SCHEME     ESP_ADC_CAL_VAL_EFUSE_TP_FIT
#endif

static esp_adc_cal_characteristics_t adc1_chars;
static esp_adc_cal_characteristics_t adc2_chars;


static bool adc_calibration_init(void)
{
    esp_err_t ret;
    bool cali_enable = false;

    ret = esp_adc_cal_check_efuse(ADC_SLEEP_TEST_CALI_SCHEME);
    if (ret == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGW(TAG, "Calibration scheme not supported, skip software calibration");
    } else if (ret == ESP_ERR_INVALID_VERSION) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else if (ret == ESP_OK) {
        cali_enable = true;
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_SLEEP_TEST_ATTEN, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
        esp_adc_cal_characterize(ADC_UNIT_2, ADC_SLEEP_TEST_ATTEN, ADC_WIDTH_BIT_DEFAULT, 0, &adc2_chars);
    } else {
        ESP_LOGE(TAG, "Invalid arg");
    }

    return cali_enable;
}

#define TEST_REGI2C_ANA_CALI_BYTE_NUM   8

TEST_CASE("test ADC1 Single Read with Light Sleep", "[adc][manul][ignore]")
{
    //ADC1 config
    TEST_ESP_OK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    TEST_ESP_OK(adc1_config_channel_atten(ADC1_SLEEP_TEST_CHAN, ADC_SLEEP_TEST_ATTEN));

    //ADC config calibration
    bool cali_en = adc_calibration_init();

    int raw_expected = 0;
    uint32_t cali_expected = 0;
    uint8_t regi2c_cali_val_before[TEST_REGI2C_ANA_CALI_BYTE_NUM] = {};

    int raw_after_sleep = 0;
    uint32_t cali_after_sleep = 0;
    uint8_t regi2c_cali_val_after[TEST_REGI2C_ANA_CALI_BYTE_NUM] = {};

    //---------------------------------Before Sleep-----------------------------------//
    ESP_LOGI("Before", "Light Sleep");

    //Read
    raw_expected = adc1_get_raw(ADC1_SLEEP_TEST_CHAN);
    if (cali_en) {
        cali_expected = esp_adc_cal_raw_to_voltage(raw_expected, &adc1_chars);
    }

#if REGI2C_ANA_CALI_PD_WORKAROUND
    //Print regi2c
    for (int i = 0; i < TEST_REGI2C_ANA_CALI_BYTE_NUM; i++) {
        regi2c_cali_val_before[i] = regi2c_ctrl_read_reg(I2C_SAR_ADC, I2C_SAR_ADC_HOSTID, i);
        printf("regi2c cali val is 0x%x", regi2c_cali_val_before[i]);
    }
    printf("\n");
#endif

    //Print result
    ESP_LOGI(TAG_CH[0][0], "ADC1 raw data: %d", raw_expected);
    if (cali_en) {
        ESP_LOGI(TAG_CH[0][0], "ADC1 cali data: %d", cali_expected);
    }

    //---------------------------------After Sleep-----------------------------------//
    ESP_LOGI("After", "Light Sleep");
    esp_sleep_enable_timer_wakeup(30 * 1000);
    esp_light_sleep_start();
    ESP_LOGI(TAG, "Wakeup from light sleep.");

#if REGI2C_ANA_CALI_PD_WORKAROUND
    //Print regi2c
    for (int i = 0; i < TEST_REGI2C_ANA_CALI_BYTE_NUM; i++) {
        regi2c_cali_val_after[i] = regi2c_ctrl_read_reg(I2C_SAR_ADC, I2C_SAR_ADC_HOSTID, i);
        printf("regi2c cali val is 0x%x", regi2c_cali_val_after[i]);
    }
    printf("\n");
#endif

    //Read
    raw_after_sleep = adc1_get_raw(ADC1_SLEEP_TEST_CHAN);
    if (cali_en) {
        cali_after_sleep = esp_adc_cal_raw_to_voltage(raw_after_sleep, &adc1_chars);
    }

    //Print result
    ESP_LOGI(TAG_CH[0][0], "after light sleep, ADC1 cali data: %d", raw_after_sleep);
    if (cali_en) {
        ESP_LOGI(TAG_CH[0][0], "after light sleep, ADC1 cali data: %d", cali_after_sleep);
    }

    //Compare
    int32_t raw_diff = raw_expected - raw_after_sleep;
    IDF_LOG_PERFORMANCE("ADC1 raw diff after sleep", "%d", raw_diff);
    if (cali_en) {
        int32_t cali_diff = cali_expected - cali_after_sleep;
        IDF_LOG_PERFORMANCE("ADC1 cali diff after sleep", "%d mV", cali_diff);
    }

    for (int i = 0; i < TEST_REGI2C_ANA_CALI_BYTE_NUM; i++) {
        TEST_ASSERT_EQUAL(regi2c_cali_val_before[i], regi2c_cali_val_after[i]);
    }
}

TEST_CASE("test ADC2 Single Read with Light Sleep", "[adc][manul][ignore]")
{
    //ADC2 config
    ESP_ERROR_CHECK(adc2_config_channel_atten(ADC2_SLEEP_TEST_CHAN, ADC_SLEEP_TEST_ATTEN));
    //ADC config calibration
    bool cali_en = adc_calibration_init();

    int raw_expected = 0;
    uint32_t cali_expected = 0;
    uint8_t regi2c_cali_val_before[TEST_REGI2C_ANA_CALI_BYTE_NUM] = {};

    int raw_after_sleep = 0;
    uint32_t cali_after_sleep = 0;
    uint8_t regi2c_cali_val_after[TEST_REGI2C_ANA_CALI_BYTE_NUM] = {};

    //---------------------------------Before Sleep-----------------------------------//
    ESP_LOGI("Before", "Light Sleep");

    //Read
    TEST_ESP_OK(adc2_get_raw(ADC2_SLEEP_TEST_CHAN, ADC_WIDTH_BIT_DEFAULT, &raw_expected));
    if (cali_en) {
        cali_expected = esp_adc_cal_raw_to_voltage(raw_expected, &adc2_chars);
    }

#if REGI2C_ANA_CALI_PD_WORKAROUND
    //Print regi2c
    for (int i = 0; i < TEST_REGI2C_ANA_CALI_BYTE_NUM; i++) {
        regi2c_cali_val_before[i] = regi2c_ctrl_read_reg(I2C_SAR_ADC, I2C_SAR_ADC_HOSTID, i);
        printf("regi2c cali val is 0x%x", regi2c_cali_val_before[i]);
    }
    printf("\n");
#endif

    //Print result
    ESP_LOGI(TAG_CH[1][0], "ADC2 raw data: %d", raw_expected);
    if (cali_en) {
        ESP_LOGI(TAG_CH[1][0], "ADC2 cali data: %d", cali_expected);
    }

    //---------------------------------After Sleep-----------------------------------//
    ESP_LOGI("After", "Light Sleep");
    esp_sleep_enable_timer_wakeup(30 * 1000);
    esp_light_sleep_start();
    ESP_LOGI(TAG, "Wakeup from light sleep.");

#if REGI2C_ANA_CALI_PD_WORKAROUND
    //Print regi2c
    for (int i = 0; i < TEST_REGI2C_ANA_CALI_BYTE_NUM; i++) {
        regi2c_cali_val_after[i] = regi2c_ctrl_read_reg(I2C_SAR_ADC, I2C_SAR_ADC_HOSTID, i);
        printf("regi2c cali val is 0x%x", regi2c_cali_val_after[i]);
    }
    printf("\n");
#endif

    //Read
    TEST_ESP_OK(adc2_get_raw(ADC2_SLEEP_TEST_CHAN, ADC_WIDTH_BIT_DEFAULT, &raw_after_sleep));
    if (cali_en) {
        cali_after_sleep += esp_adc_cal_raw_to_voltage(raw_after_sleep, &adc2_chars);
    }

    //Print result
    ESP_LOGI(TAG_CH[1][0], "after light sleep, ADC2 cali data: %d", raw_after_sleep);
    if (cali_en) {
        ESP_LOGI(TAG_CH[1][0], "after light sleep, ADC2 cali data: %d", cali_after_sleep);
    }

    //Compare
    int32_t raw_diff = raw_expected - raw_after_sleep;
    IDF_LOG_PERFORMANCE("ADC2 raw diff after sleep", "%d", raw_diff);
    if (cali_en) {
        int32_t cali_diff = cali_expected - cali_after_sleep;
        IDF_LOG_PERFORMANCE("ADC2 cali diff after sleep", "%d mV", cali_diff);
    }
    for (int i = 0; i < TEST_REGI2C_ANA_CALI_BYTE_NUM; i++) {
        TEST_ASSERT_EQUAL(regi2c_cali_val_before[i], regi2c_cali_val_after[i]);
    }
}
#endif   //#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2) //TODO IDF-3908
