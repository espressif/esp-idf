/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 Tests for the touch sensor device driver for ESP32
*/
#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32

#include "esp_system.h"
#include "driver/touch_pad.h"
#include "unity.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "test_utils.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/sens_reg.h"
#include "soc/sens_struct.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/rtc_io_reg.h"
#include "soc/rtc_io_struct.h"
#include "esp_rom_sys.h"

static const char *TAG = "test_touch";

#define TOUCH_READ_INVALID_VAL          (0)
#define TOUCHPAD_FILTER_TOUCH_PERIOD    (10)

#define TOUCH_REG_BASE_TEST() ({    \
   TEST_ASSERT_EQUAL_UINT32(RTC_CNTL_BROWN_OUT_REG, &RTCCNTL.brown_out.val);   \
   TEST_ASSERT_EQUAL_UINT32(REG_GET_FIELD(SENS_SARDATE_REG, SENS_SAR_DATE), SENS.sardate.sar_date);     \
   TEST_ASSERT_EQUAL_UINT32(REG_GET_FIELD(RTC_IO_DATE_REG, RTC_IO_IO_DATE), RTCIO.date.date);           \
})

#define TOUCH_READ_ERROR     (100)
#define TEST_TOUCH_COUNT_NUM (10)
#define TEST_TOUCH_CHANNEL   (3)
static touch_pad_t touch_list[TEST_TOUCH_CHANNEL] = {
    // TOUCH_PAD_NUM0,
    // TOUCH_PAD_NUM1 is GPIO0, for download.
    // TOUCH_PAD_NUM2,
    // TOUCH_PAD_NUM3,
    // TOUCH_PAD_NUM4,
    // TOUCH_PAD_NUM5,
    // TOUCH_PAD_NUM6,
    TOUCH_PAD_NUM7,
    TOUCH_PAD_NUM8,
    TOUCH_PAD_NUM9,
};

static void printf_touch_hw_read(const char *str)
{
    uint16_t touch_value;
    printf("[%s] ", str);
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        while (!touch_pad_meas_is_done()) ;
        touch_pad_read_raw_data(touch_list[i], &touch_value);
        printf("[%d]%d ", touch_list[i], touch_value);
    }
    printf("\r\n");
}

/*
 * Change the slope to get larger value from touch sensor.
 */
static void test_press_fake(touch_pad_t pad_num)
{
    touch_pad_set_cnt_mode(pad_num, TOUCH_PAD_SLOPE_2, TOUCH_PAD_TIE_OPT_DEFAULT);
}

/*
 * Change the slope to get smaller value from touch sensor.
 */
static void test_release_fake(touch_pad_t pad_num)
{
    touch_pad_set_cnt_mode(pad_num, TOUCH_PAD_SLOPE_7, TOUCH_PAD_TIE_OPT_DEFAULT);
}

static esp_err_t test_touch_sw_read_test_runner(void)
{
    ESP_LOGI(TAG, "%s", __func__);
    uint16_t touch_value;

    TEST_ESP_OK( touch_pad_init() );
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_SW) );

    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i], TOUCH_READ_INVALID_VAL) );
    }

    // Start task to read values sensed by pads
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        printf("test T%d\n", touch_list[i]);
        touch_pad_read(touch_list[i], &touch_value);
        printf("T%d:[%4d] ", touch_list[i], touch_value);
    }
    printf("\n");

    TEST_ESP_OK( touch_pad_deinit() );

    return ESP_OK;
}

static esp_err_t test_touch_sw_read(void)
{
    ESP_LOGI(TAG, "%s", __func__);
    uint16_t touch_value;

    TEST_ESP_OK( touch_pad_init() );
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_SW) );

    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i], TOUCH_READ_INVALID_VAL) );
    }

    // Start task to read values sensed by pads
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        printf("test T%d\n", touch_list[i]);
        TEST_ESP_OK( touch_pad_read(touch_list[i], &touch_value) );
        printf("T%d:[%4d] ", touch_list[i], touch_value);
        TEST_ASSERT_NOT_EQUAL(TOUCH_READ_INVALID_VAL, touch_value);
    }
    printf("\n");

    TEST_ESP_OK( touch_pad_deinit() );

    return ESP_OK;
}

static esp_err_t test_touch_timer_read(void)
{
    ESP_LOGI(TAG, "%s", __func__);
    uint16_t touch_value[TEST_TOUCH_CHANNEL], touch_temp[TEST_TOUCH_CHANNEL];
    int t_cnt = TEST_TOUCH_COUNT_NUM;

    TEST_ESP_OK( touch_pad_init() );
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER) );

    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i], TOUCH_READ_INVALID_VAL) );
    }
    // Start task to read values sensed by pads
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_read(touch_list[i], &touch_value[i]) );
        printf("T%d:[%4d] ", touch_list[i], touch_value[i]);
        TEST_ASSERT_NOT_EQUAL(TOUCH_READ_INVALID_VAL, touch_value[i]);
    }
    while (t_cnt--) {
        // Start task to read values sensed by pads
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            TEST_ESP_OK( touch_pad_read(touch_list[i], &touch_temp[i]) );
            TEST_ASSERT_NOT_EQUAL(TOUCH_READ_INVALID_VAL, touch_temp[i]);
            TEST_ASSERT_UINT32_WITHIN(TOUCH_READ_ERROR, touch_temp[i], touch_value[i]);
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }

    TEST_ESP_OK( touch_pad_deinit() );

    return ESP_OK;
}

static esp_err_t test_touch_filtered_read(void)
{
    ESP_LOGI(TAG, "%s", __func__);
    uint16_t touch_value, touch_temp;
    int t_cnt = TEST_TOUCH_COUNT_NUM;

    TEST_ESP_OK( touch_pad_init() );
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER) );

    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i], TOUCH_READ_INVALID_VAL) );
    }
    // Initialize and start a software filter to detect slight change of capacitance.
    touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    while (t_cnt--) {
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            TEST_ESP_OK( touch_pad_read(touch_list[i], &touch_value) );
            TEST_ASSERT_NOT_EQUAL(TOUCH_READ_INVALID_VAL, touch_value);
            TEST_ESP_OK( touch_pad_read_raw_data(touch_list[i], &touch_value) );
            TEST_ASSERT_NOT_EQUAL(TOUCH_READ_INVALID_VAL, touch_value);
            TEST_ESP_OK( touch_pad_read_filtered(touch_list[i], &touch_temp) );
            TEST_ASSERT_NOT_EQUAL(TOUCH_READ_INVALID_VAL, touch_temp);
            TEST_ASSERT_UINT32_WITHIN(TOUCH_READ_ERROR, touch_temp, touch_value);
            printf("T%d:[%4d] ", touch_list[i], touch_value);
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    printf("\n");

    TEST_ESP_OK( touch_pad_deinit() );

    return ESP_OK;
}

// test the basic configuration function with right parameters and error parameters
TEST_CASE("Touch Sensor all channel read test", "[touch]")
{
    TOUCH_REG_BASE_TEST();
    test_touch_sw_read_test_runner();
    TEST_ESP_OK( test_touch_sw_read() );
    TEST_ESP_OK( test_touch_timer_read() );
    TEST_ESP_OK( test_touch_filtered_read() );
}

static int test_touch_parameter(touch_pad_t pad_num, int meas_time, int slp_time, int vol_h, int vol_l, int vol_a, int slope)
{
    ESP_LOGI(TAG, "%s", __func__);
    uint16_t touch_value;
    TEST_ESP_OK( touch_pad_init() );
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER) );
    TEST_ESP_OK( touch_pad_config(pad_num, TOUCH_READ_INVALID_VAL) );

    touch_pad_set_measurement_interval(slp_time);
    touch_pad_set_measurement_clock_cycles(meas_time);
    touch_pad_set_voltage(vol_h, vol_l, vol_a);
    touch_pad_set_cnt_mode(pad_num, slope, TOUCH_PAD_TIE_OPT_DEFAULT);

    // Initialize and start a software filter to detect slight change of capacitance.
    TEST_ESP_OK(touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD));
    vTaskDelay(500 / portTICK_PERIOD_MS);

    // Start task to read values sensed by pads
    TEST_ESP_OK( touch_pad_read(pad_num, &touch_value) );
    TEST_ASSERT_NOT_EQUAL(TOUCH_READ_INVALID_VAL, touch_value);
    printf("T%d:[%4d] ", pad_num, touch_value);
    TEST_ESP_OK( touch_pad_read_raw_data(pad_num, &touch_value) );
    TEST_ASSERT_NOT_EQUAL(TOUCH_READ_INVALID_VAL, touch_value);
    printf("T%d:[%4d] ", pad_num, touch_value);
    TEST_ESP_OK( touch_pad_read_filtered(pad_num, &touch_value) );
    TEST_ASSERT_NOT_EQUAL(TOUCH_READ_INVALID_VAL, touch_value);
    printf("T%d:[%4d] \n", pad_num, touch_value);

    TEST_ESP_OK( touch_pad_deinit() );

    return touch_value;
}

TEST_CASE("Touch Sensor parameters test", "[touch]")
{
    int touch_val[5] = {0};

    ESP_LOGI(TAG, "Charge / incharge voltage level test");
    touch_val[0] = test_touch_parameter(touch_list[2], TOUCH_PAD_MEASURE_CYCLE_DEFAULT, TOUCH_PAD_SLEEP_CYCLE_DEFAULT,
                                        TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_0V,
                                        TOUCH_PAD_SLOPE_DEFAULT);
    touch_val[1] = test_touch_parameter(touch_list[2], TOUCH_PAD_MEASURE_CYCLE_DEFAULT, TOUCH_PAD_SLEEP_CYCLE_DEFAULT,
                                        TOUCH_HVOLT_2V5, TOUCH_LVOLT_0V6, TOUCH_HVOLT_ATTEN_1V,
                                        TOUCH_PAD_SLOPE_DEFAULT);
    touch_val[2] = test_touch_parameter(touch_list[0], TOUCH_PAD_MEASURE_CYCLE_DEFAULT, TOUCH_PAD_SLEEP_CYCLE_DEFAULT,
                                        TOUCH_HVOLT_2V4, TOUCH_LVOLT_0V8, TOUCH_HVOLT_ATTEN_1V5,
                                        TOUCH_PAD_SLOPE_DEFAULT);

    TEST_ASSERT_GREATER_OR_EQUAL(touch_val[0], touch_val[1]);
    TEST_ASSERT_GREATER_OR_EQUAL(touch_val[1], touch_val[2]);

    ESP_LOGI(TAG, "Measure time / sleep time test");
    touch_val[0] = test_touch_parameter(touch_list[0], 0xff, 0xa,
                                        TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD, TOUCH_PAD_LOW_VOLTAGE_THRESHOLD, TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD, TOUCH_PAD_SLOPE_DEFAULT);
    touch_val[1] = test_touch_parameter(touch_list[0], 0x1ff, 0xf,
                                        TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD, TOUCH_PAD_LOW_VOLTAGE_THRESHOLD, TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD, TOUCH_PAD_SLOPE_DEFAULT);
    touch_val[2] = test_touch_parameter(touch_list[0], 0x2fff, 0x1f,
                                        TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD, TOUCH_PAD_LOW_VOLTAGE_THRESHOLD, TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD, TOUCH_PAD_SLOPE_DEFAULT);

    TEST_ASSERT_GREATER_OR_EQUAL(touch_val[0], touch_val[1]);
    TEST_ASSERT_GREATER_OR_EQUAL(touch_val[1], touch_val[2]);

    ESP_LOGI(TAG, "Charge / incharge slope level test");
    touch_val[0] = test_touch_parameter(touch_list[1], TOUCH_PAD_MEASURE_CYCLE_DEFAULT, TOUCH_PAD_SLEEP_CYCLE_DEFAULT,
                                        TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD, TOUCH_PAD_LOW_VOLTAGE_THRESHOLD, TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD, 1);
    touch_val[1] = test_touch_parameter(touch_list[1], TOUCH_PAD_MEASURE_CYCLE_DEFAULT, TOUCH_PAD_SLEEP_CYCLE_DEFAULT,
                                        TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD, TOUCH_PAD_LOW_VOLTAGE_THRESHOLD, TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD, 3);
    touch_val[2] = test_touch_parameter(touch_list[1], TOUCH_PAD_MEASURE_CYCLE_DEFAULT, TOUCH_PAD_SLEEP_CYCLE_DEFAULT,
                                        TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD, TOUCH_PAD_LOW_VOLTAGE_THRESHOLD, TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD, 7);

    TEST_ASSERT_GREATER_OR_EQUAL(touch_val[0], touch_val[1]);
    TEST_ASSERT_GREATER_OR_EQUAL(touch_val[1], touch_val[2]);
}

static bool s_pad_activated[TOUCH_PAD_MAX];

static void test_touch_intr_cb(void *arg)
{
    uint32_t pad_intr = touch_pad_get_status();
    //clear interrupt
    touch_pad_clear_status();
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        if ((pad_intr >> touch_list[i]) & 0x1) {
            s_pad_activated[touch_list[i]] = true;
        }
    }
}

static esp_err_t test_touch_interrupt(void)
{
    ESP_LOGI(TAG, "%s", __func__);
    uint16_t touch_value;

    TEST_ESP_OK( touch_pad_init() );
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER) );
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);

    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i], TOUCH_READ_INVALID_VAL) );
    }
    // Initialize and start a software filter to detect slight change of capacitance.
    touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        //read filtered value
        TEST_ESP_OK( touch_pad_read_filtered(touch_list[i], &touch_value) );
        ESP_LOGI(TAG, "test init: touch pad [%d] val is %d", touch_list[i], touch_value);
        //set interrupt threshold.
        TEST_ESP_OK( touch_pad_set_thresh(touch_list[i], touch_value * 2 / 3) );
    }

    // Register touch interrupt ISR
    TEST_ESP_OK( touch_pad_isr_register(test_touch_intr_cb, NULL) );
    TEST_ESP_OK( touch_pad_clear_status() );
    TEST_ESP_OK( touch_pad_intr_enable() );

    int test_cnt = TEST_TOUCH_COUNT_NUM;
    while (test_cnt--) {
        ESP_LOGI(TAG, "touch push");
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            test_press_fake(touch_list[i]);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
        printf_touch_hw_read("push");

        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            if (s_pad_activated[touch_list[i]] == false) {
                ESP_LOGE(TAG, "touch%d not active", touch_list[i]);
                TEST_FAIL();
            }
        }
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            s_pad_activated[touch_list[i]] = 0;
        }

        ESP_LOGI(TAG, "touch release");
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            test_release_fake(touch_list[i]);
        }
        printf_touch_hw_read("release");
    }

    TEST_ESP_OK( touch_pad_deinit() );

    return ESP_OK;
}

TEST_CASE("Touch Sensor interrupt test", "[touch]")
{
    TEST_ESP_OK( test_touch_interrupt() );
}

#endif // CONFIG_IDF_TARGET_ESP32
