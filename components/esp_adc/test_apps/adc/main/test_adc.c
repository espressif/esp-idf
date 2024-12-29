/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/adc_periph.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_monitor.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "test_common_adc.h"
#include "esp_rom_sys.h"

const __attribute__((unused)) static char *TAG = "TEST_ADC";

/*---------------------------------------------------------------
        ADC General Macros
---------------------------------------------------------------*/
//ADC Channels
#if CONFIG_IDF_TARGET_ESP32
#define ADC1_TEST_CHAN0          ADC_CHANNEL_4
#define ADC1_TEST_CHAN1          ADC_CHANNEL_5
#define ADC2_TEST_CHAN0          ADC_CHANNEL_0
static const char *TAG_CH[2][10] = {{"ADC1_CH4", "ADC1_CH5"}, {"ADC2_CH0"}};
#else
#define ADC1_TEST_CHAN0          ADC_CHANNEL_2
#define ADC1_TEST_CHAN1          ADC_CHANNEL_3
#define ADC2_TEST_CHAN0          ADC_CHANNEL_0
static const char *TAG_CH[2][10] = {{"ADC1_CH2", "ADC1_CH3"}, {"ADC2_CH0"}};
#endif

/*---------------------------------------------------------------
        ADC Oneshot High / Low test
---------------------------------------------------------------*/
//ESP32C3 ADC2 oneshot mode is not supported anymore
#define ADC_TEST_ONESHOT_HIGH_LOW_TEST_ADC2    ((SOC_ADC_PERIPH_NUM >= 2) && !CONFIG_IDF_TARGET_ESP32C3)

TEST_CASE("ADC oneshot high/low test", "[adc_oneshot]")
{
    static int adc_raw[2][10];

    //-------------ADC1 Init---------------//
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    TEST_ESP_OK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

#if ADC_TEST_ONESHOT_HIGH_LOW_TEST_ADC2
    //-------------ADC2 Init---------------//
    adc_oneshot_unit_handle_t adc2_handle;
    adc_oneshot_unit_init_cfg_t init_config2 = {
        .unit_id = ADC_UNIT_2,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    TEST_ESP_OK(adc_oneshot_new_unit(&init_config2, &adc2_handle));
#endif //#if ADC_TEST_ONESHOT_HIGH_LOW_TEST_ADC2

    //-------------ADC1 TEST Channel 0 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    TEST_ESP_OK(adc_oneshot_config_channel(adc1_handle, ADC1_TEST_CHAN0, &config));

    //-------------ADC1 TEST Channel 1 Config---------------//
    TEST_ESP_OK(adc_oneshot_config_channel(adc1_handle, ADC1_TEST_CHAN1, &config));

#if ADC_TEST_ONESHOT_HIGH_LOW_TEST_ADC2
    //-------------ADC2 TEST Channel 0 Config---------------//
    TEST_ESP_OK(adc_oneshot_config_channel(adc2_handle, ADC2_TEST_CHAN0, &config));
#endif //#if ADC_TEST_ONESHOT_HIGH_LOW_TEST_ADC2

    test_adc_set_io_level(ADC_UNIT_1, ADC1_TEST_CHAN0, 0);
    TEST_ESP_OK(adc_oneshot_read(adc1_handle, ADC1_TEST_CHAN0, &adc_raw[0][0]));
    ESP_LOGI(TAG_CH[0][0], "raw  data: %d", adc_raw[0][0]);
    TEST_ASSERT_INT_WITHIN(ADC_TEST_LOW_THRESH, ADC_TEST_LOW_VAL, adc_raw[0][0]);

    test_adc_set_io_level(ADC_UNIT_1, ADC1_TEST_CHAN1, 1);
    TEST_ESP_OK(adc_oneshot_read(adc1_handle, ADC1_TEST_CHAN1, &adc_raw[0][1]));
    ESP_LOGI(TAG_CH[0][1], "raw  data: %d", adc_raw[0][1]);
    TEST_ASSERT_INT_WITHIN(ADC_TEST_HIGH_THRESH, ADC_TEST_HIGH_VAL, adc_raw[0][1]);

#if ADC_TEST_ONESHOT_HIGH_LOW_TEST_ADC2
    test_adc_set_io_level(ADC_UNIT_2, ADC2_TEST_CHAN0, 0);
    TEST_ESP_OK(adc_oneshot_read(adc2_handle, ADC2_TEST_CHAN0, &adc_raw[1][0]));
    ESP_LOGI(TAG_CH[1][0], "raw  data: %d", adc_raw[1][0]);
    TEST_ASSERT_INT_WITHIN(ADC_TEST_LOW_THRESH, ADC_TEST_LOW_VAL, adc_raw[1][0]);
#endif //#if ADC_TEST_ONESHOT_HIGH_LOW_TEST_ADC2

    test_adc_set_io_level(ADC_UNIT_1, ADC1_TEST_CHAN0, 1);
    TEST_ESP_OK(adc_oneshot_read(adc1_handle, ADC1_TEST_CHAN0, &adc_raw[0][0]));
    ESP_LOGI(TAG_CH[0][0], "raw  data: %d", adc_raw[0][0]);
    TEST_ASSERT_INT_WITHIN(ADC_TEST_HIGH_THRESH, ADC_TEST_HIGH_VAL, adc_raw[0][0]);

    test_adc_set_io_level(ADC_UNIT_1, ADC1_TEST_CHAN1, 0);
    TEST_ESP_OK(adc_oneshot_read(adc1_handle, ADC1_TEST_CHAN1, &adc_raw[0][1]));
    ESP_LOGI(TAG_CH[0][1], "raw  data: %d", adc_raw[0][1]);
    TEST_ASSERT_INT_WITHIN(ADC_TEST_LOW_THRESH, ADC_TEST_LOW_VAL, adc_raw[0][1]);

#if ADC_TEST_ONESHOT_HIGH_LOW_TEST_ADC2
    test_adc_set_io_level(ADC_UNIT_2, ADC2_TEST_CHAN0, 1);
    TEST_ESP_OK(adc_oneshot_read(adc2_handle, ADC2_TEST_CHAN0, &adc_raw[1][0]));
    ESP_LOGI(TAG_CH[1][0], "raw  data: %d", adc_raw[1][0]);
    TEST_ASSERT_INT_WITHIN(ADC_TEST_HIGH_THRESH, ADC_TEST_HIGH_VAL, adc_raw[1][0]);
#endif //#if ADC_TEST_ONESHOT_HIGH_LOW_TEST_ADC2

    TEST_ESP_OK(adc_oneshot_del_unit(adc1_handle));
#if ADC_TEST_ONESHOT_HIGH_LOW_TEST_ADC2
    TEST_ESP_OK(adc_oneshot_del_unit(adc2_handle));
#endif //#if ADC_TEST_ONESHOT_HIGH_LOW_TEST_ADC2
}

TEST_CASE("ADC oneshot stress test that get zero even if convent done", "[adc_oneshot]")
{
    //There is a hardware limitation. After ADC get DONE signal, it still need a delay to synchronize ADC raw data or it may get zero even if getting DONE signal.

    int test_num = 100;
    adc_channel_t channel = ADC1_TEST_CHAN1;
    adc_atten_t atten = ADC_ATTEN_DB_12;
    adc_unit_t unit_id = ADC_UNIT_1;

    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = unit_id,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = SOC_ADC_RTC_MAX_BITWIDTH,
        .atten = atten,
    };

    int raw_data = 0;
    srand(199);

    for (int i = 0; i < test_num; i++) {
        test_adc_set_io_level(unit_id, ADC1_TEST_CHAN1, 1);

        TEST_ESP_OK(adc_oneshot_new_unit(&init_config1, &adc1_handle));
        TEST_ESP_OK(adc_oneshot_config_channel(adc1_handle, channel, &config));
        TEST_ESP_OK(adc_oneshot_read(adc1_handle, channel, &raw_data));

        TEST_ASSERT_NOT_EQUAL(0, raw_data);

        TEST_ESP_OK(adc_oneshot_del_unit(adc1_handle));

        esp_rom_delay_us(rand() % 512);
    }
}

#if SOC_ADC_CALIBRATION_V1_SUPPORTED
/*---------------------------------------------------------------
        ADC Oneshot with Light Sleep
---------------------------------------------------------------*/
#include <inttypes.h>
#include "esp_sleep.h"
#include "esp_private/regi2c_ctrl.h"
#include "soc/regi2c_saradc.h"

#define TEST_REGI2C_ANA_CALI_BYTE_NUM   8

static void s_adc_oneshot_with_sleep(adc_unit_t unit_id, adc_channel_t channel)
{
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

    //-------------ADC Calibration Init---------------//
    bool do_calibration = false;
    adc_cali_handle_t cali_handle[TEST_ATTEN_NUMS] = {};
    for (int i = 0; i < TEST_ATTEN_NUMS; i++) {
        do_calibration = test_adc_calibration_init(unit_id, channel, g_test_atten[i], SOC_ADC_RTC_MAX_BITWIDTH, &cali_handle[i]);
    }
    if (!do_calibration) {
        ESP_LOGW(TAG, "No efuse bits burnt, only test the regi2c analog register values");
    }

    for (int i = 0; i < TEST_ATTEN_NUMS; i++) {

        //-------------ADC Channel Config---------------//
        config.atten = g_test_atten[i];
        TEST_ESP_OK(adc_oneshot_config_channel(adc_handle, channel, &config));
        printf("Test with atten: %d\n", g_test_atten[i]);

        //---------------------------------Before Sleep-----------------------------------//
        printf("Before Light Sleep\n");
        int raw_expected = 0;
        int cali_expected = 0;
        uint8_t regi2c_cali_val_before[TEST_REGI2C_ANA_CALI_BYTE_NUM] = {};

        //Read
        TEST_ESP_OK(adc_oneshot_read(adc_handle, channel, &raw_expected));
        if (do_calibration) {
            TEST_ESP_OK(adc_cali_raw_to_voltage(cali_handle[i], raw_expected, &cali_expected));
        }

        //Print regi2c
        printf("regi2c cali val is: ");
        for (int j = 0; j < TEST_REGI2C_ANA_CALI_BYTE_NUM; j++) {
            regi2c_cali_val_before[j] = regi2c_ctrl_read_reg(I2C_SAR_ADC, I2C_SAR_ADC_HOSTID, j);
            printf("0x%x ", regi2c_cali_val_before[j]);
        }
        printf("\n");

        //Print result
        ESP_LOGI(TAG, "ADC%d Chan%d: raw  data: %d", unit_id + 1, channel, raw_expected);
        ESP_LOGI(TAG, "ADC%d Chan%d: cali data: %d", unit_id + 1, channel, cali_expected);

        //---------------------------------Sleep-----------------------------------//
        esp_sleep_enable_timer_wakeup(30 * 1000);
        esp_light_sleep_start();

        //---------------------------------After Sleep-----------------------------------//
        printf("After Light Sleep\n");
        int raw_after_sleep = 0;
        int cali_after_sleep = 0;
        uint8_t regi2c_cali_val_after[TEST_REGI2C_ANA_CALI_BYTE_NUM] = {};

        //Print regi2c
        printf("regi2c cali val is: ");
        for (int i = 0; i < TEST_REGI2C_ANA_CALI_BYTE_NUM; i++) {
            regi2c_cali_val_after[i] = regi2c_ctrl_read_reg(I2C_SAR_ADC, I2C_SAR_ADC_HOSTID, i);
            printf("0x%x ", regi2c_cali_val_after[i]);
        }
        printf("\n");

        //Read
        TEST_ESP_OK(adc_oneshot_read(adc_handle, channel, &raw_after_sleep));
        if (do_calibration) {
            TEST_ESP_OK(adc_cali_raw_to_voltage(cali_handle[i], raw_after_sleep, &cali_after_sleep));
        }

        //Print result
        ESP_LOGI(TAG, "ADC%d Chan%d: raw  data: %d", unit_id + 1, channel, raw_after_sleep);
        if (do_calibration) {
            ESP_LOGI(TAG, "ADC%d Chan%d: cali data: %d", unit_id + 1, channel, cali_after_sleep);
        }

        //Compare
        int32_t raw_diff = raw_expected - raw_after_sleep;
        ESP_LOGI(TAG, "ADC%d Chan%d: raw difference: %"PRId32, unit_id + 1, channel, raw_diff);

        if (do_calibration) {
            int32_t cali_diff = cali_expected - cali_after_sleep;
            ESP_LOGI(TAG, "ADC%d Chan%d: cali difference: %"PRId32, unit_id + 1, channel, cali_diff);
        }

        //Test Calibration registers
        for (int i = 0; i < TEST_REGI2C_ANA_CALI_BYTE_NUM; i++) {
            TEST_ASSERT_EQUAL(regi2c_cali_val_before[i], regi2c_cali_val_after[i]);
        }
        ESP_LOGI(TAG, "Cali register settings unchanged");

    }
    TEST_ESP_OK(adc_oneshot_del_unit(adc_handle));
    for (int i = 0; i < TEST_ATTEN_NUMS; i++) {
        if (cali_handle[i]) {
            test_adc_calibration_deinit(cali_handle[i]);
        }
    }
}

//ADC Channels
#if CONFIG_IDF_TARGET_ESP32
#define ADC1_SLEEP_TEST_CHAN          ADC_CHANNEL_6
#define ADC2_SLEEP_TEST_CHAN          ADC_CHANNEL_0
#else
#define ADC1_SLEEP_TEST_CHAN          ADC_CHANNEL_2
#define ADC2_SLEEP_TEST_CHAN          ADC_CHANNEL_0
#endif

TEST_CASE("test ADC1 Single Read with Light Sleep", "[adc]")
{
    s_adc_oneshot_with_sleep(ADC_UNIT_1, ADC1_SLEEP_TEST_CHAN);
}

#if (SOC_ADC_PERIPH_NUM >= 2) && !CONFIG_IDF_TARGET_ESP32C3
//ESP32C3 ADC2 oneshot mode is not supported anymore
TEST_CASE("test ADC2 Single Read with Light Sleep", "[adc]")
{
    s_adc_oneshot_with_sleep(ADC_UNIT_2, ADC2_SLEEP_TEST_CHAN);
}
#endif  //#if (SOC_ADC_PERIPH_NUM >= 2) && !CONFIG_IDF_TARGET_ESP32C3

#endif  //#if SOC_ADC_CALIBRATION_V1_SUPPORTED

#if SOC_ADC_MONITOR_SUPPORTED && CONFIG_SOC_ADC_DMA_SUPPORTED
#if CONFIG_IDF_TARGET_ESP32S2
#define TEST_ADC_FORMAT_TYPE   ADC_DIGI_OUTPUT_FORMAT_TYPE1
#else
#define TEST_ADC_FORMAT_TYPE   ADC_DIGI_OUTPUT_FORMAT_TYPE2
#endif
bool IRAM_ATTR test_high_cb(adc_monitor_handle_t monitor_handle, const adc_monitor_evt_data_t *event_data, void *user_data)
{
    return false;
}
TEST_CASE("ADC continuous monitor init_deinit", "[adc]")
{
    adc_continuous_handle_t handle = NULL;
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = 1024,
        .conv_frame_size = SOC_ADC_DIGI_DATA_BYTES_PER_CONV * 2,
    };
    TEST_ESP_OK(adc_continuous_new_handle(&adc_config, &handle));

    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    for (int i = 0; i < 1; i++) {
        adc_pattern[i].atten = ADC_ATTEN_DB_12;
        adc_pattern[i].channel = i;
        adc_pattern[i].unit = ADC_UNIT_1;
        adc_pattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
    }
    adc_continuous_config_t dig_cfg = {
        .pattern_num = 1,
        .adc_pattern = adc_pattern,
        .sample_freq_hz = SOC_ADC_SAMPLE_FREQ_THRES_LOW,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = TEST_ADC_FORMAT_TYPE,
    };
    TEST_ESP_OK(adc_continuous_config(handle, &dig_cfg));

    //try to enable without installed
    adc_monitor_handle_t monitor_handle = NULL;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, adc_continuous_monitor_enable(monitor_handle));

    //try to install with invalid argument
    adc_monitor_config_t adc_monitor_cfg = {
        .adc_unit = 2,
        .channel = 2,
        .h_threshold = 3000,
        .l_threshold = -1,
    };
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, adc_new_continuous_monitor(handle, &adc_monitor_cfg, &monitor_handle));

    //try to install when adc is running
    adc_monitor_cfg.adc_unit = ADC_UNIT_1;
    TEST_ESP_OK(adc_continuous_start(handle));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, adc_new_continuous_monitor(handle, &adc_monitor_cfg, &monitor_handle));
    TEST_ESP_OK(adc_continuous_stop(handle));

    //normal install
    TEST_ESP_OK(adc_new_continuous_monitor(handle, &adc_monitor_cfg, &monitor_handle));

    //try register callback funcs when monitor is running
    adc_monitor_evt_cbs_t monitor_cb = {
        .on_over_high_thresh = test_high_cb,
        .on_below_low_thresh = NULL,
    };
    TEST_ESP_OK(adc_continuous_monitor_enable(monitor_handle));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, adc_continuous_monitor_register_event_callbacks(monitor_handle, &monitor_cb, NULL));
    TEST_ESP_OK(adc_continuous_monitor_disable(monitor_handle));

    //normal register cbs
    TEST_ESP_OK(adc_continuous_monitor_register_event_callbacks(monitor_handle, &monitor_cb, NULL));

    //try init so many monitor, we totally have 2 monitors actually
    adc_monitor_handle_t monitor_handle_2 = NULL, monitor_handle_3 = NULL;
#if CONFIG_IDF_TARGET_ESP32S2
    adc_monitor_cfg.adc_unit = ADC_UNIT_2;  //s2 can't use two monitor on same ADC unit
#endif
    TEST_ESP_OK(adc_new_continuous_monitor(handle, &adc_monitor_cfg, &monitor_handle_2));
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, adc_new_continuous_monitor(handle, &adc_monitor_cfg, &monitor_handle_3));

    //try delete them, as monitor_handle_3 should be NULL because it should init failed
    TEST_ESP_OK(adc_del_continuous_monitor(monitor_handle_2));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, adc_del_continuous_monitor(monitor_handle_3));

    //try delete it when adc is running but monitor not running
    TEST_ESP_OK(adc_continuous_start(handle));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, adc_del_continuous_monitor(monitor_handle));
    TEST_ESP_OK(adc_continuous_stop(handle));

    //normal option
    TEST_ESP_OK(adc_continuous_monitor_enable(monitor_handle));
    TEST_ESP_OK(adc_continuous_monitor_disable(monitor_handle));

    //normal uninstall
    TEST_ESP_OK(adc_del_continuous_monitor(monitor_handle));

    TEST_ESP_OK(adc_continuous_deinit(handle));
}

#if SOC_ADC_CALIBRATION_V1_SUPPORTED
#define TEST_ADC_CHANNEL    ADC_CHANNEL_0
static uint32_t m1h_cnt, m1l_cnt;

bool IRAM_ATTR m1h_cb(adc_monitor_handle_t monitor_handle, const adc_monitor_evt_data_t *event_data, void *user_data)
{
    m1h_cnt ++;
    return false;
}
bool IRAM_ATTR m1l_cb(adc_monitor_handle_t monitor_handle, const adc_monitor_evt_data_t *event_data, void *user_data)
{
    m1l_cnt ++;
    return false;
}
TEST_CASE("ADC continuous monitor functionary", "[adc]")
{
    adc_continuous_handle_t handle = NULL;
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = 1024,
        .conv_frame_size = SOC_ADC_DIGI_DATA_BYTES_PER_CONV * 2,
    };
    TEST_ESP_OK(adc_continuous_new_handle(&adc_config, &handle));

    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    for (int i = 0; i < 2; i++) {
        adc_pattern[i].atten = ADC_ATTEN_DB_12;
        adc_pattern[i].channel = TEST_ADC_CHANNEL;
        adc_pattern[i].unit = ADC_UNIT_1;
        adc_pattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
    }
    adc_continuous_config_t dig_cfg = {
        .pattern_num = 2,
        .adc_pattern = adc_pattern,
        .sample_freq_hz = SOC_ADC_SAMPLE_FREQ_THRES_LOW,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = TEST_ADC_FORMAT_TYPE,
    };
    TEST_ESP_OK(adc_continuous_config(handle, &dig_cfg));

    //config monitor
    adc_monitor_handle_t monitor_handle;
    adc_monitor_config_t adc_monitor_cfg = {
        .adc_unit = ADC_UNIT_1,
        .channel = TEST_ADC_CHANNEL,
#if CONFIG_IDF_TARGET_ESP32S2
        .h_threshold = -1,      //S2 support only one threshold for one monitor
#else
        .h_threshold = ADC_TEST_HIGH_VAL_DMA - (ADC_TEST_HIGH_VAL_DMA - ADC_TEST_LOW_VAL) / 4,
#endif
        .l_threshold = ADC_TEST_LOW_VAL + (ADC_TEST_HIGH_VAL_DMA - ADC_TEST_LOW_VAL) / 4,
    };
    adc_monitor_evt_cbs_t monitor_cb = {
#if !CONFIG_IDF_TARGET_ESP32S2
        .on_over_high_thresh = m1h_cb,
#endif
        .on_below_low_thresh = m1l_cb,
    };
    TEST_ESP_OK(adc_new_continuous_monitor(handle, &adc_monitor_cfg, &monitor_handle));
    TEST_ESP_OK(adc_continuous_monitor_register_event_callbacks(monitor_handle, &monitor_cb, NULL));
    TEST_ESP_OK(adc_continuous_monitor_enable(monitor_handle));
    TEST_ESP_OK(adc_continuous_start(handle));

    int adc_io;
    adc_continuous_channel_to_io(ADC_UNIT_1, TEST_ADC_CHANNEL, &adc_io);
    printf("Using ADC_CHANNEL_%d on GPIO%d\n", TEST_ADC_CHANNEL, adc_io);

    // Test with internal gpio pull up/down, detail and order refer to `gpio_pull_mode_t`
    // pull_up + pull_down to get half ADC convert
    for (uint8_t i = 0; i < 8; i++) {
        int pull_mode = i % GPIO_FLOATING;
        gpio_set_pull_mode(adc_io, pull_mode);
        vTaskDelay(10); // wait some time for GPIO level be stable
        m1h_cnt = 0;
        m1l_cnt = 0;
        vTaskDelay(1000);   //time to count monitor interrupt
        printf("%d\t %s\t high_cnt %4ld\tlow_cnt %4ld\n", i, (pull_mode == 0) ? "up  " : (pull_mode == 1) ? "down" : "mid ", m1h_cnt, m1l_cnt);

        switch (pull_mode) {
        case GPIO_PULLUP_ONLY:
#if !CONFIG_IDF_TARGET_ESP32S2
            TEST_ASSERT_UINT32_WITHIN(SOC_ADC_SAMPLE_FREQ_THRES_LOW * 0.1, SOC_ADC_SAMPLE_FREQ_THRES_LOW, m1h_cnt);
            TEST_ASSERT_EQUAL(0, m1l_cnt);  //low limit should NOT triggrted when pull_up
#endif
            break;
        case GPIO_PULLDOWN_ONLY:
            TEST_ASSERT_UINT32_WITHIN(SOC_ADC_SAMPLE_FREQ_THRES_LOW * 0.1, SOC_ADC_SAMPLE_FREQ_THRES_LOW, m1l_cnt);
            TEST_ASSERT_EQUAL(0, m1h_cnt);
            break;
        case GPIO_PULLUP_PULLDOWN:
            TEST_ASSERT_EQUAL(0, m1h_cnt);  //half votage, both limit should NOT triggered
            TEST_ASSERT_EQUAL(0, m1l_cnt);
            break;
        default: printf("unknown gpio pull mode !!!\n");
        }
    }
    TEST_ESP_OK(adc_continuous_stop(handle));
    TEST_ESP_OK(adc_continuous_monitor_disable(monitor_handle));
    TEST_ESP_OK(adc_del_continuous_monitor(monitor_handle));
    TEST_ESP_OK(adc_continuous_deinit(handle));
}
#endif  //SOC_ADC_CALIBRATION_V1_SUPPORTED
#endif  //SOC_ADC_MONITOR_SUPPORTED && CONFIG_SOC_ADC_DMA_SUPPORTED
