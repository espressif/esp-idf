/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "soc/adc_periph.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "test_common_adc.h"

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

#if (SOC_ADC_PERIPH_NUM >= 2)
    //-------------ADC2 Init---------------//
    adc_oneshot_unit_handle_t adc2_handle;
    adc_oneshot_unit_init_cfg_t init_config2 = {
        .unit_id = ADC_UNIT_2,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    TEST_ESP_OK(adc_oneshot_new_unit(&init_config2, &adc2_handle));
#endif //#if (SOC_ADC_PERIPH_NUM >= 2)

    //-------------ADC1 TEST Channel 0 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
    TEST_ESP_OK(adc_oneshot_config_channel(adc1_handle, ADC1_TEST_CHAN0, &config));

    //-------------ADC1 TEST Channel 1 Config---------------//
    TEST_ESP_OK(adc_oneshot_config_channel(adc1_handle, ADC1_TEST_CHAN1, &config));

#if (SOC_ADC_PERIPH_NUM >= 2)
    //-------------ADC2 TEST Channel 0 Config---------------//
    TEST_ESP_OK(adc_oneshot_config_channel(adc2_handle, ADC2_TEST_CHAN0, &config));
#endif //#if (SOC_ADC_PERIPH_NUM >= 2)

    test_adc_set_io_level(ADC_UNIT_1, ADC1_TEST_CHAN0, 0);
    TEST_ESP_OK(adc_oneshot_read(adc1_handle, ADC1_TEST_CHAN0, &adc_raw[0][0]));
    ESP_LOGI(TAG_CH[0][0], "raw  data: %d", adc_raw[0][0]);
    TEST_ASSERT_INT_WITHIN(ADC_TEST_LOW_THRESH, ADC_TEST_LOW_VAL, adc_raw[0][0]);

    test_adc_set_io_level(ADC_UNIT_1, ADC1_TEST_CHAN1, 1);
    TEST_ESP_OK(adc_oneshot_read(adc1_handle, ADC1_TEST_CHAN1, &adc_raw[0][1]));
    ESP_LOGI(TAG_CH[0][1], "raw  data: %d", adc_raw[0][1]);
    TEST_ASSERT_INT_WITHIN(ADC_TEST_HIGH_THRESH, ADC_TEST_HIGH_VAL, adc_raw[0][1]);

#if (SOC_ADC_PERIPH_NUM >= 2)
    test_adc_set_io_level(ADC_UNIT_2, ADC2_TEST_CHAN0, 0);
    TEST_ESP_OK(adc_oneshot_read(adc2_handle, ADC2_TEST_CHAN0, &adc_raw[1][0]));
    ESP_LOGI(TAG_CH[1][0], "raw  data: %d", adc_raw[1][0]);
    TEST_ASSERT_INT_WITHIN(ADC_TEST_LOW_THRESH, ADC_TEST_LOW_VAL, adc_raw[1][0]);
#endif //#if (SOC_ADC_PERIPH_NUM >= 2)


    test_adc_set_io_level(ADC_UNIT_1, ADC1_TEST_CHAN0, 1);
    TEST_ESP_OK(adc_oneshot_read(adc1_handle, ADC1_TEST_CHAN0, &adc_raw[0][0]));
    ESP_LOGI(TAG_CH[0][0], "raw  data: %d", adc_raw[0][0]);
    TEST_ASSERT_INT_WITHIN(ADC_TEST_HIGH_THRESH, ADC_TEST_HIGH_VAL, adc_raw[0][0]);

    test_adc_set_io_level(ADC_UNIT_1, ADC1_TEST_CHAN1, 0);
    TEST_ESP_OK(adc_oneshot_read(adc1_handle, ADC1_TEST_CHAN1, &adc_raw[0][1]));
    ESP_LOGI(TAG_CH[0][1], "raw  data: %d", adc_raw[0][1]);
    TEST_ASSERT_INT_WITHIN(ADC_TEST_LOW_THRESH, ADC_TEST_LOW_VAL, adc_raw[0][1]);

#if (SOC_ADC_PERIPH_NUM >= 2)
    test_adc_set_io_level(ADC_UNIT_2, ADC2_TEST_CHAN0, 1);
    TEST_ESP_OK(adc_oneshot_read(adc2_handle, ADC2_TEST_CHAN0, &adc_raw[1][0]));
    ESP_LOGI(TAG_CH[1][0], "raw  data: %d", adc_raw[1][0]);
    TEST_ASSERT_INT_WITHIN(ADC_TEST_HIGH_THRESH, ADC_TEST_HIGH_VAL, adc_raw[1][0]);
#endif //#if (SOC_ADC_PERIPH_NUM >= 2)


    TEST_ESP_OK(adc_oneshot_del_unit(adc1_handle));
#if (SOC_ADC_PERIPH_NUM >= 2)
    TEST_ESP_OK(adc_oneshot_del_unit(adc2_handle));
#endif //#if (SOC_ADC_PERIPH_NUM >= 2)
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
        do_calibration = test_adc_calibration_init(unit_id, g_test_atten[i], SOC_ADC_RTC_MAX_BITWIDTH, &cali_handle[i]);
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
        ESP_LOGI(TAG, "Cali register settings unchanged\n");

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

TEST_CASE("test ADC1 Single Read with Light Sleep", "[adc][manul][ignore]")
{
    s_adc_oneshot_with_sleep(ADC_UNIT_1, ADC1_SLEEP_TEST_CHAN);
}

#if (SOC_ADC_PERIPH_NUM >= 2)
TEST_CASE("test ADC2 Single Read with Light Sleep", "[adc][manul][ignore]")
{
    s_adc_oneshot_with_sleep(ADC_UNIT_2, ADC2_SLEEP_TEST_CHAN);
}
#endif  //#if (SOC_ADC_PERIPH_NUM >= 2)

#endif  //#if SOC_ADC_CALIBRATION_V1_SUPPORTED
