/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 Tests for the touch sensor device driver for ESP32-S2 & ESP32-S3
*/
#include "sdkconfig.h"

#include <string.h>
#include <inttypes.h>
#include "esp_system.h"
#include "driver/touch_pad.h"
#include "unity.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/sens_reg.h"
#include "soc/sens_struct.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/rtc_io_reg.h"
#include "soc/rtc_io_struct.h"
#include "soc/syscon_reg.h"
#include "driver/rtc_io.h"
#include "esp_rom_sys.h"

static const char *TAG = "test_touch";

#define PLATFORM_SELECT            (1)  //0: pxp; 1: chip
#if (PLATFORM_SELECT == 0)              //PXP platform
#define SET_BREAK_POINT(flag) REG_WRITE(SYSCON_DATE_REG, flag)
//PXP clk is slower.
#define SYS_DELAY_TIME_MOM    (1/40)
#define RTC_SLOW_CLK_FLAG     1     // Slow clock is 32KHz.
void test_pxp_deinit_io(void)
{
    for (int i = 0; i < 22; i++) {
        rtc_gpio_init(i);
    }
}
#else
//PXP clk is slower.
#define SET_BREAK_POINT(flag)
#define SYS_DELAY_TIME_MOM    (1)
#define RTC_SLOW_CLK_FLAG     0     // Slow clock is 32KHz.
void test_pxp_deinit_io(void)
{
    ;
}
#endif

#define TOUCH_READ_INVALID_VAL          (SOC_TOUCH_PAD_THRESHOLD_MAX)
#define TOUCH_READ_ERROR_THRESH         (0.1) // 10% error
#define TOUCH_INTR_THRESHOLD            (0.1)
#define TOUCH_EXCEED_TIME_MS            (1000)

#define TOUCH_REG_BASE_TEST() ({    \
   TEST_ASSERT_EQUAL_UINT32(REG_READ(RTC_CNTL_DATE_REG), RTCCNTL.date.date);   \
   TEST_ASSERT_EQUAL_UINT32(REG_READ(SENS_SARDATE_REG), SENS.sardate.sar_date);     \
   TEST_ASSERT_EQUAL_UINT32(REG_READ(RTC_IO_DATE_REG), RTCIO.date.date);           \
})

#define TEST_TOUCH_COUNT_NUM    (5)
#define TEST_TOUCH_CHANNEL      (14)
static touch_pad_t touch_list[TEST_TOUCH_CHANNEL] = {
    // TOUCH_PAD_NUM0, is GPIO0, for download.
    TOUCH_PAD_NUM1,
    TOUCH_PAD_NUM2,
    TOUCH_PAD_NUM3,
    TOUCH_PAD_NUM4,
    TOUCH_PAD_NUM5,
    TOUCH_PAD_NUM6,
    TOUCH_PAD_NUM7,
    TOUCH_PAD_NUM8,
    TOUCH_PAD_NUM9,
    TOUCH_PAD_NUM10,
    TOUCH_PAD_NUM11,
    TOUCH_PAD_NUM12,
    TOUCH_PAD_NUM13,
    TOUCH_PAD_NUM14
};

#define TOUCH_WATERPROOF_RING_PAD TOUCH_PAD_NUM1
static touch_pad_t proximity_pad[3] = {
    TOUCH_PAD_NUM2,
    TOUCH_PAD_NUM3,
    TOUCH_PAD_NUM4,
};

static QueueHandle_t que_touch = NULL;
typedef struct touch_msg {
    touch_pad_intr_mask_t intr_mask;
    uint32_t pad_num;
    uint32_t pad_status;
    uint32_t pad_val;
    uint32_t slp_proxi_cnt;
    uint32_t slp_proxi_base;
} touch_event_t;

static uint32_t s_touch_timeout_mask = 0;

static void printf_touch_hw_read(const char *str)
{
    uint32_t touch_value;
    printf("[%s] ", str);
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        touch_pad_read_raw_data(touch_list[i], &touch_value);
        printf("[%d]%"PRIu32" ", touch_list[i], touch_value);
    }
    printf("\r\n");
}

static void printf_touch_benchmark_read(const char *str)
{
    uint32_t touch_value;
    printf("[%s] ", str);
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_value) );
        printf("[%d]%"PRIu32" ", touch_list[i], touch_value);
    }
    printf("\r\n");
}

static void printf_touch_smooth_read(const char *str)
{
    uint32_t touch_value;
    printf("[%s] ", str);
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        touch_pad_filter_read_smooth(touch_list[i], &touch_value);
        printf("[%d]%"PRIu32" ", touch_list[i], touch_value);
    }
    printf("\r\n");
}


static void test_timeout_trigger_fake(touch_pad_t pad_num)
{
    touch_pad_set_cnt_mode(pad_num, TOUCH_PAD_SLOPE_0, TOUCH_PAD_TIE_OPT_DEFAULT);
}

static void test_timeout_normal(touch_pad_t pad_num)
{
    touch_pad_set_cnt_mode(pad_num, TOUCH_PAD_SLOPE_7, TOUCH_PAD_TIE_OPT_DEFAULT);
}

/*
 * Change the slope to get larger value from touch sensor.
 */
static void test_press_fake(touch_pad_t pad_num)
{
    touch_pad_set_cnt_mode(pad_num, TOUCH_PAD_SLOPE_3, TOUCH_PAD_TIE_OPT_DEFAULT);
}

/*
 * Change the slope to get larger value from touch sensor.
 */
static void test_release_fake(touch_pad_t pad_num)
{
    touch_pad_set_cnt_mode(pad_num, TOUCH_PAD_SLOPE_7, TOUCH_PAD_TIE_OPT_DEFAULT);
}

static void test_touch_push_all(void)
{
    ESP_LOGI(TAG, "touch push");
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        test_press_fake(touch_list[i]);
    }
}

static void test_touch_release_all(void)
{
    ESP_LOGI(TAG, "touch release");
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        test_release_fake(touch_list[i]);
    }
}

static void test_touch_benchmark(void)
{
    uint32_t touch_val[TEST_TOUCH_CHANNEL] = {0};
    uint32_t touch_temp[TEST_TOUCH_CHANNEL] = {0};

    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_val[i]) );
    }
    for (int i = 0; i < 10; i++) {
        vTaskDelay(20 / portTICK_PERIOD_MS);
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_temp[i]) );
            TEST_ASSERT_EQUAL(touch_temp[i], touch_val[i]);
        }
    }
}

/*
 * Test the stable and change of touch sensor reading in SW mode.
 */
esp_err_t test_touch_sw_read(void)
{
    uint32_t touch_value[TEST_TOUCH_CHANNEL] = {0};
    uint32_t touch_temp[TEST_TOUCH_CHANNEL] = {0};
    uint32_t touch_push[TEST_TOUCH_CHANNEL] = {0};
    int test_cnt = TEST_TOUCH_COUNT_NUM;

    ESP_LOGI(TAG, "  >> %s <<", __func__);
    TEST_ESP_OK( touch_pad_init() );
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i]) );
    }
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_SW) );
    TEST_ESP_OK( touch_pad_fsm_start() );

    while (test_cnt--) {
        test_touch_release_all();

        /* Read the touch sensor raw data in SW mode. */
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            TEST_ESP_OK( touch_pad_sw_start() );
            while (!touch_pad_meas_is_done()) ;
            TEST_ESP_OK( touch_pad_read_raw_data(touch_list[i], &touch_value[i]) );
            printf("T%d:[%4"PRIu32"] ", touch_list[i], touch_value[i]);
            TEST_ASSERT_NOT_EQUAL(TOUCH_READ_INVALID_VAL, touch_value[i]);
        }
        printf("\n");
        /* Check the stable of reading. */
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            if (touch_temp[i]) {
                TEST_ASSERT_UINT32_WITHIN((uint32_t)((float)touch_temp[i]*TOUCH_READ_ERROR_THRESH), touch_temp[i], touch_value[i]);
            }
            touch_temp[i] = touch_value[i];
        }

        test_touch_push_all();

        /* Read the touch sensor raw data in SW mode. */
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            TEST_ESP_OK( touch_pad_sw_start() );
            while (!touch_pad_meas_is_done()) ;
            TEST_ESP_OK( touch_pad_read_raw_data(touch_list[i], &touch_push[i]) );
            printf("T%d:[%4"PRIu32"] ", touch_list[i], touch_push[i]);
            TEST_ASSERT_NOT_EQUAL(TOUCH_READ_INVALID_VAL, touch_push[i]);
        }
        printf("\n");
        /* Check the change of reading. */
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            TEST_ASSERT_GREATER_THAN(touch_value[i], touch_push[i]);
        }
    }
    TEST_ESP_OK( touch_pad_deinit() );

    return ESP_OK;
}

/*
 * Test the stable and change of touch sensor reading in timer mode.
 * TEST POINT:
 * 1. Timer mode for FSM.
 * 2. Touch channel slope setting.
 * 3. Touch reading stable.
 */
esp_err_t test_touch_timer_read(void)
{
    uint32_t touch_value[TEST_TOUCH_CHANNEL] = {0};
    uint32_t touch_temp[TEST_TOUCH_CHANNEL] = {0};
    uint32_t touch_push[TEST_TOUCH_CHANNEL] = {0};
    int test_cnt = TEST_TOUCH_COUNT_NUM;

    ESP_LOGI(TAG, "  >> %s <<", __func__);
    TEST_ESP_OK( touch_pad_init() );
    /* Set different slope for channels to test slope function. */
    printf("Set slope for channel: ");
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i]) );
        TEST_ESP_OK( touch_pad_set_cnt_mode(touch_list[i], i % 7 ? i % 7 : 1, TOUCH_PAD_TIE_OPT_DEFAULT) );
        printf("[ch%d-%d] ", touch_list[i], i % 7 ? i % 7 : 1);
    }
    printf("\n");
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER) );
    TEST_ESP_OK( touch_pad_fsm_start() );

    /* Wait touch sensor stable */
    vTaskDelay(50 * SYS_DELAY_TIME_MOM / portTICK_PERIOD_MS);

    while (test_cnt--) {
        test_touch_release_all();
        vTaskDelay(50  * SYS_DELAY_TIME_MOM / portTICK_PERIOD_MS);

        // Start task to read values sensed by pads
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            TEST_ESP_OK( touch_pad_read_raw_data(touch_list[i], &touch_value[i]) );
            TEST_ASSERT_NOT_EQUAL(TOUCH_READ_INVALID_VAL, touch_value[i]);
            printf("T%d:[%4"PRIu32"] ", touch_list[i], touch_value[i]);
        }
        printf("\n");
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            if (touch_temp[i]) {
                TEST_ASSERT_UINT32_WITHIN((uint32_t)((float)touch_temp[i]*TOUCH_READ_ERROR_THRESH), touch_temp[i], touch_value[i]);
            }
            touch_temp[i] = touch_value[i];
        }

        test_touch_push_all();
        vTaskDelay(50  * SYS_DELAY_TIME_MOM / portTICK_PERIOD_MS);

        /* Read the touch sensor raw data in FSM mode. */
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            TEST_ESP_OK( touch_pad_read_raw_data(touch_list[i], &touch_push[i]) );
            printf("T%d:[%4"PRIu32"] ", touch_list[i], touch_push[i]);
            TEST_ASSERT_NOT_EQUAL(TOUCH_READ_INVALID_VAL, touch_push[i]);
        }
        printf("\n");
        /* Check the change of reading. */
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            TEST_ASSERT_GREATER_THAN(touch_value[i], touch_push[i]);
        }
    }
    TEST_ESP_OK( touch_pad_deinit() );

    return ESP_OK;
}

/*
 * Test the filter mode.
 * TEST POINT:
 * 1. Timer mode for FSM.
 * 2. Touch reading stable.
 * 3. Touch reading init value.
 * 4. Touch reading filtered value equal to raw data.
 */
esp_err_t test_touch_filtered_read(void)
{
    uint32_t touch_value[TEST_TOUCH_CHANNEL] = {0};
    uint32_t touch_temp[TEST_TOUCH_CHANNEL] = {0};

    ESP_LOGI(TAG, "  >> %s <<", __func__);
    TEST_ESP_OK( touch_pad_init() );
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i]) );
    }
    touch_filter_config_t filter_info = {
        .mode = TOUCH_PAD_FILTER_IIR_32,           // Test jitter and filter 1/4.
        .debounce_cnt = 1,      // 1 time count.
        .noise_thr = 0,         // 50%
        .jitter_step = 4,       // use for jitter mode.
        .smh_lvl = TOUCH_PAD_SMOOTH_IIR_2,
    };
    TEST_ESP_OK( touch_pad_filter_set_config(&filter_info) );
    TEST_ESP_OK( touch_pad_filter_enable() );
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER) );
    TEST_ESP_OK( touch_pad_fsm_start() );

    /* Wait touch pad init done. */
    vTaskDelay(50 * SYS_DELAY_TIME_MOM / portTICK_PERIOD_MS);
    /* Test the stable for init value of touch reading.
     * Ideal: benchmark == raw data == smooth data.
     */
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_value[i]) );
        TEST_ASSERT_NOT_EQUAL(TOUCH_READ_INVALID_VAL, touch_value[i]);
        TEST_ESP_OK( touch_pad_read_raw_data(touch_list[i], &touch_temp[i]) );
        TEST_ASSERT_NOT_EQUAL(TOUCH_READ_INVALID_VAL, touch_temp[i]);
        TEST_ASSERT_UINT32_WITHIN((uint32_t)((float)touch_temp[i]*TOUCH_READ_ERROR_THRESH), touch_temp[i], touch_value[i]);
        TEST_ESP_OK( touch_pad_filter_read_smooth(touch_list[i], &touch_temp[i]) );
        TEST_ASSERT_NOT_EQUAL(TOUCH_READ_INVALID_VAL, touch_temp[i]);
        TEST_ASSERT_UINT32_WITHIN((uint32_t)((float)touch_temp[i]*TOUCH_READ_ERROR_THRESH), touch_temp[i], touch_value[i]);
    }
    printf("touch filter init value:\n");
    printf_touch_hw_read("raw  ");
    printf_touch_benchmark_read("base ");
    printf_touch_smooth_read("smooth");
    printf("\n");

    int test_cnt = TEST_TOUCH_COUNT_NUM;
    while (test_cnt--) {
        /* Touch reading filtered value equal to raw data. */
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            TEST_ESP_OK( touch_pad_read_raw_data(touch_list[i], &touch_value[i]) );
            TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_temp[i]) );
            TEST_ASSERT_UINT32_WITHIN((uint32_t)((float)touch_temp[i]*TOUCH_READ_ERROR_THRESH), touch_temp[i], touch_value[i]);
            TEST_ESP_OK( touch_pad_filter_read_smooth(touch_list[i], &touch_temp[i]) );
            TEST_ASSERT_UINT32_WITHIN((uint32_t)((float)touch_temp[i]*TOUCH_READ_ERROR_THRESH), touch_temp[i], touch_value[i]);
        }
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            if (touch_temp[i]) {
                TEST_ASSERT_UINT32_WITHIN((uint32_t)((float)touch_temp[i]*TOUCH_READ_ERROR_THRESH), touch_temp[i], touch_value[i]);
            }
            touch_temp[i] = touch_value[i];
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
    TEST_ESP_OK( touch_pad_deinit() );

    return ESP_OK;
}

TEST_CASE("Touch Sensor reading test (SW, Timer, filter)", "[touch]")
{
    TOUCH_REG_BASE_TEST();
    TEST_ESP_OK( test_touch_sw_read() );
    TEST_ESP_OK( test_touch_timer_read() );
    TEST_ESP_OK( test_touch_filtered_read() );
}

/*
 * Test the base patameter mode.
 * TEST POINT:
 * 1. measure time and sleep time setting.
 * 2. Charge / incharge voltage threshold setting.
 * 3. Touch slope setting.
 * 4. Touch reading filtered value equal to raw data.
 */
int test_touch_base_parameter(touch_pad_t pad_num, int meas_time, int slp_time,
                              int vol_h, int vol_l, int vol_a, int slope, bool is_conn_gnd)
{
    uint32_t touch_value = 0;
    uint32_t touch_temp = 0, touch_filter;
    uint64_t val_sum = 0;
    int test_cnt = TEST_TOUCH_COUNT_NUM;

    ESP_LOGI(TAG, "  >> %s <<", __func__);
    TEST_ESP_OK( touch_pad_init() );
    /* Note: init all channel, but test one channel. */
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i]) );
    }

    TEST_ESP_OK( touch_pad_set_cnt_mode(pad_num, slope, TOUCH_PAD_TIE_OPT_DEFAULT) );
    TEST_ESP_OK( touch_pad_set_measurement_interval(slp_time) );
    TEST_ESP_OK( touch_pad_set_charge_discharge_times(meas_time) );
    TEST_ESP_OK( touch_pad_set_voltage(vol_h, vol_l, vol_a) );
    TEST_ESP_OK( touch_pad_set_idle_channel_connect(is_conn_gnd) );
    ESP_LOGI(TAG, "meas_time[%d]_slp_time[%d]_vol_h[%d]_vol_l[%d]_vol_a[%d]_slope[%d]_is_conn_gnd[%d]",
             meas_time, slp_time, vol_h, vol_l, vol_a, slope, is_conn_gnd);

    touch_filter_config_t filter_info = {
        .mode = TOUCH_PAD_FILTER_IIR_32,    // Test jitter and filter 1/4.
        .debounce_cnt = 1,      // 1 time count.
        .noise_thr = 0,         // 50%
        .jitter_step = 4,       // use for jitter mode.
        .smh_lvl = TOUCH_PAD_SMOOTH_IIR_2,
    };
    TEST_ESP_OK( touch_pad_filter_set_config(&filter_info) );
    TEST_ESP_OK( touch_pad_filter_enable() );
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER) );
    TEST_ESP_OK( touch_pad_fsm_start() );

    /* Some parameters will delay the init time. so wait longger time */
    vTaskDelay(100 / portTICK_PERIOD_MS);

    while (test_cnt--) {
        /* Correctness of reading. Ideal: benchmark == raw data == smooth data. */
        TEST_ESP_OK( touch_pad_read_raw_data(pad_num, &touch_value) );
        TEST_ESP_OK( touch_pad_read_benchmark(pad_num, &touch_filter) );
        TEST_ASSERT_UINT32_WITHIN((uint32_t)((float)touch_filter*TOUCH_READ_ERROR_THRESH), touch_filter, touch_value);
        TEST_ESP_OK( touch_pad_filter_read_smooth(pad_num, &touch_filter) );
        TEST_ASSERT_UINT32_WITHIN((uint32_t)((float)touch_filter*TOUCH_READ_ERROR_THRESH), touch_filter, touch_value);

        /* Stable of reading */
        TEST_ESP_OK( touch_pad_read_raw_data(pad_num, &touch_value) );
        TEST_ASSERT_NOT_EQUAL(TOUCH_READ_INVALID_VAL, touch_value);
        if (touch_temp) {
            TEST_ASSERT_UINT32_WITHIN((uint32_t)((float)touch_temp*TOUCH_READ_ERROR_THRESH), touch_temp, touch_value);
        }
        touch_temp = touch_value;

        printf("T%d:[%4"PRIu32"] ", pad_num, touch_value);
        val_sum += touch_value; // For check.
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
    printf("\n");

    TEST_ESP_OK( touch_pad_deinit() );
    return (uint32_t)(val_sum / TEST_TOUCH_COUNT_NUM);
}

TEST_CASE("Touch Sensor base parameters test (meas_time, voltage, slope, inv_conn)", "[touch]")
{
    int touch_val[5] = {0};

    ESP_LOGI(TAG, "Charge / incharge voltage level test");
    touch_val[0] = test_touch_base_parameter(touch_list[2], TOUCH_PAD_MEASURE_CYCLE_DEFAULT, TOUCH_PAD_SLEEP_CYCLE_DEFAULT,
                   TOUCH_HVOLT_2V4, TOUCH_LVOLT_0V8, TOUCH_HVOLT_ATTEN_1V5,
                   TOUCH_PAD_SLOPE_DEFAULT, true);
    touch_val[1] = test_touch_base_parameter(touch_list[2], TOUCH_PAD_MEASURE_CYCLE_DEFAULT, TOUCH_PAD_SLEEP_CYCLE_DEFAULT,
                   TOUCH_HVOLT_2V5, TOUCH_LVOLT_0V6, TOUCH_HVOLT_ATTEN_1V,
                   TOUCH_PAD_SLOPE_DEFAULT, true);
    touch_val[2] = test_touch_base_parameter(touch_list[2], TOUCH_PAD_MEASURE_CYCLE_DEFAULT, TOUCH_PAD_SLEEP_CYCLE_DEFAULT,
                   TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_0V,
                   TOUCH_PAD_SLOPE_DEFAULT, true);

    TEST_ASSERT_GREATER_THAN(touch_val[0], touch_val[1]);
    TEST_ASSERT_GREATER_THAN(touch_val[1], touch_val[2]);

    ESP_LOGI(TAG, "Measure time / sleep time test");
    touch_val[0] = test_touch_base_parameter(touch_list[1], 0xff, 0x1ff,
                   TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD, TOUCH_PAD_LOW_VOLTAGE_THRESHOLD, TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD, TOUCH_PAD_SLOPE_DEFAULT, true);
    touch_val[1] = test_touch_base_parameter(touch_list[1], 0xfff, 0xff,
                   TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD, TOUCH_PAD_LOW_VOLTAGE_THRESHOLD, TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD, TOUCH_PAD_SLOPE_DEFAULT, true);
    touch_val[2] = test_touch_base_parameter(touch_list[1], 0x1fff, 0xf,
                   TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD, TOUCH_PAD_LOW_VOLTAGE_THRESHOLD, TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD, TOUCH_PAD_SLOPE_DEFAULT, true);

    TEST_ASSERT_GREATER_THAN(touch_val[0], touch_val[1]);
    TEST_ASSERT_GREATER_THAN(touch_val[1], touch_val[2]);

    ESP_LOGI(TAG, "Charge / incharge slope level test");
    touch_val[0] = test_touch_base_parameter(touch_list[0], TOUCH_PAD_MEASURE_CYCLE_DEFAULT, TOUCH_PAD_SLEEP_CYCLE_DEFAULT,
                   TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD, TOUCH_PAD_LOW_VOLTAGE_THRESHOLD, TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD, 7, true);
    touch_val[1] = test_touch_base_parameter(touch_list[0], TOUCH_PAD_MEASURE_CYCLE_DEFAULT, TOUCH_PAD_SLEEP_CYCLE_DEFAULT,
                   TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD, TOUCH_PAD_LOW_VOLTAGE_THRESHOLD, TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD, 5, true);
    touch_val[2] = test_touch_base_parameter(touch_list[0], TOUCH_PAD_MEASURE_CYCLE_DEFAULT, TOUCH_PAD_SLEEP_CYCLE_DEFAULT,
                   TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD, TOUCH_PAD_LOW_VOLTAGE_THRESHOLD, TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD, 3, true);

    TEST_ASSERT_GREATER_THAN(touch_val[0], touch_val[1]);
    TEST_ASSERT_GREATER_THAN(touch_val[1], touch_val[2]);

    /* The GND option causes larger parasitic capacitance and larger reading */
    ESP_LOGI(TAG, "Inactive connect test");
    touch_val[0] = test_touch_base_parameter(touch_list[3], TOUCH_PAD_MEASURE_CYCLE_DEFAULT, TOUCH_PAD_SLEEP_CYCLE_DEFAULT,
                   TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD, TOUCH_PAD_LOW_VOLTAGE_THRESHOLD, TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD, TOUCH_PAD_SLOPE_DEFAULT,
                   false);
    touch_val[1] = test_touch_base_parameter(touch_list[3], TOUCH_PAD_MEASURE_CYCLE_DEFAULT, TOUCH_PAD_SLEEP_CYCLE_DEFAULT,
                   TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD, TOUCH_PAD_LOW_VOLTAGE_THRESHOLD, TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD, TOUCH_PAD_SLOPE_DEFAULT,
                   true);
    TEST_ASSERT_GREATER_THAN(touch_val[0], touch_val[1]);
}

/*
 * Check active interrupt of touch channels.
 */
static esp_err_t test_touch_check_ch_touched(uint32_t test_ch_num, uint32_t exceed_time_ms)
{
    touch_event_t evt = {0};
    esp_err_t ret = ESP_FAIL;
    printf("Active: ");
    while (1) {
        if (pdTRUE == xQueueReceive(que_touch, &evt, exceed_time_ms / portTICK_PERIOD_MS)) {
            if (evt.intr_mask & TOUCH_PAD_INTR_MASK_ACTIVE) {
                printf("0x%"PRIx32", ", evt.pad_status);
                if (test_ch_num == __builtin_popcount(evt.pad_status)) {
                    ret = ESP_OK;
                    break;
                }
            } else if (evt.intr_mask & (TOUCH_PAD_INTR_MASK_DONE | TOUCH_PAD_INTR_MASK_SCAN_DONE)) {
                continue;
            } else {    // If the interrupt type error, test error.
                ESP_LOGI(TAG, "Touch[%"PRIu32"] intr error, status %"PRIx32", evt_msk0x%x", evt.pad_num, evt.pad_status, evt.intr_mask);
                break;
            }
        } else {
            ESP_LOGI(TAG, "Touch intr exceed time");
            break;
        }
    }
    printf("\n");
    return ret;
}

/*
 * Check inactive interrupt of touch channels.
 */
static esp_err_t test_touch_check_ch_released(uint32_t test_ch_num, uint32_t exceed_time_ms)
{
    touch_event_t evt = {0};
    esp_err_t ret = ESP_FAIL;
    printf("Inactive: ");
    while (1) {
        if (pdTRUE == xQueueReceive(que_touch, &evt, exceed_time_ms / portTICK_PERIOD_MS)) {
            if (evt.intr_mask & TOUCH_PAD_INTR_MASK_INACTIVE) {
                printf("0x%"PRIx32", ", evt.pad_status);
                if ((TEST_TOUCH_CHANNEL - test_ch_num) == __builtin_popcount(evt.pad_status)) {
                    ret = ESP_OK;
                    break;
                }
            } else if (evt.intr_mask & (TOUCH_PAD_INTR_MASK_DONE | TOUCH_PAD_INTR_MASK_SCAN_DONE)) {
                continue;
            } else {    // If the interrupt type error, test error.
                ESP_LOGI(TAG, "Touch[%"PRIu32"] intr error, status %"PRIx32", evt_msk0x%x", evt.pad_num, evt.pad_status, evt.intr_mask);
                break;
            }
        } else {
            ESP_LOGI(TAG, "Touch intr exceed time");
            break;
        }
    }
    printf("\n");
    return ret;
}

static esp_err_t test_touch_check_ch_touched_with_proximity(uint32_t test_ch_num, uint32_t exceed_time_ms)
{
    uint32_t count = 0;
    uint16_t ch_mask = 0;
    touch_event_t evt = {0};
    esp_err_t ret = ESP_FAIL;

    TEST_ESP_OK( touch_pad_proximity_get_count(TOUCH_PAD_MAX, &count) );
    printf("Active: ");
    while (1) {
        if (pdTRUE == xQueueReceive(que_touch, &evt, exceed_time_ms / portTICK_PERIOD_MS)) {
            if (evt.intr_mask & TOUCH_PAD_INTR_MASK_ACTIVE) {
                printf("0x%"PRIx32", ", evt.pad_status);
                if (test_ch_num == __builtin_popcount(evt.pad_status)) {
                    ret = ESP_OK;
                    break;
                }
            } else if (evt.intr_mask & (TOUCH_PAD_INTR_MASK_SCAN_DONE)) {
                touch_pad_get_channel_mask(&ch_mask);
                for (int i = TOUCH_PAD_MAX - 1; i >= 0; i--) {
                    if (BIT(i) & ch_mask) {
                        if (evt.pad_num == i) {
                            if (count == evt.slp_proxi_cnt) {
                                esp_rom_printf("priximity base(%"PRIu32") cnt(%"PRIu32")\n", evt.slp_proxi_base, evt.slp_proxi_cnt);
                            }
                        }
                    }
                }
                continue;
            } else {    // If the interrupt type error, test error.
                ESP_LOGI(TAG, "Touch[%"PRIu32"] intr error, status %"PRIx32", evt_msk0x%x", evt.pad_num, evt.pad_status, evt.intr_mask);
                continue;
            }
        } else {
            ESP_LOGI(TAG, "Touch intr exceed time");
            break;
        }
    }
    printf("\n");
    return (esp_err_t)ret;
}

static esp_err_t test_touch_check_ch_released_with_proximity(uint32_t test_ch_num, uint32_t exceed_time_ms)
{
    uint32_t count = 0;
    uint16_t ch_mask = 0;
    touch_event_t evt = {0};
    esp_err_t ret = ESP_FAIL;

    TEST_ESP_OK( touch_pad_proximity_get_count(TOUCH_PAD_MAX, &count) );
    printf("Inactive: ");
    while (1) {
        if (pdTRUE == xQueueReceive(que_touch, &evt, exceed_time_ms / portTICK_PERIOD_MS)) {
            if (evt.intr_mask & TOUCH_PAD_INTR_MASK_INACTIVE) {
                printf("0x%"PRIx32", ", evt.pad_status);
                if ((TEST_TOUCH_CHANNEL - test_ch_num) == __builtin_popcount(evt.pad_status)) {
                    ret = ESP_OK;
                    break;
                }
            } else if (evt.intr_mask & (TOUCH_PAD_INTR_MASK_SCAN_DONE)) {
                touch_pad_get_channel_mask(&ch_mask);
                for (int i = TOUCH_PAD_MAX - 1; i >= 0; i--) {
                    if (BIT(i) & ch_mask) {
                        if (evt.pad_num == i) {
                            if (count == evt.slp_proxi_cnt) {
                                esp_rom_printf("priximity base(%"PRIu32") cnt(%"PRIu32")\n", evt.slp_proxi_base, evt.slp_proxi_cnt);
                            }
                        }
                    }
                }
                continue;
            } else {    // If the interrupt type error, test error.
                ESP_LOGI(TAG, "Touch[%"PRIu32"] intr error, status %"PRIx32", evt_msk0x%x", evt.pad_num, evt.pad_status, evt.intr_mask);
                continue;
            }
        } else {
            ESP_LOGI(TAG, "Touch intr exceed time");
            break;
        }
    }
    printf("\n");
    return (esp_err_t)ret;
}

/*
 * Check scan done interrupt of touch channels.
 */
static esp_err_t test_touch_check_ch_intr_scan_done(void)
{
    touch_event_t evt = {0};
    uint16_t ch_mask = 0;
    esp_err_t ret = ESP_FAIL;
    /* Check the scan done interrupt. */
    while (1) {
        if (pdTRUE == xQueueReceive(que_touch, &evt, 1000 / portTICK_PERIOD_MS)) {
            /* Scan done interrupt have bug that be trigger by last two channel. */
            if (evt.intr_mask & TOUCH_PAD_INTR_MASK_SCAN_DONE) {
                touch_pad_get_channel_mask(&ch_mask);
                for (int i = TOUCH_PAD_MAX - 1; i >= 0; i--) {
                    if (BIT(i) & ch_mask) {
                        if (evt.pad_num == i) {
                            ESP_LOGI(TAG, "touch _SCAN_DONE INTR be triggered");
                            ret = ESP_OK;
                        }
                        goto NEXT_TEST;
                    }
                }
            } else if (evt.intr_mask & (TOUCH_PAD_INTR_MASK_DONE | TOUCH_PAD_INTR_MASK_SCAN_DONE)) {
                continue;
            } else {    // If the interrupt type error, test error.
                ESP_LOGI(TAG, "Touch[%"PRIu32"] intr error, status %"PRIx32", evt_msk0x%x", evt.pad_num, evt.pad_status, evt.intr_mask);
                break;
            }
        } else {
            ESP_LOGI(TAG, "Touch intr exceed time");
            break;
        }
    }
NEXT_TEST:
    printf("\n");
    return (esp_err_t)ret;
}

/*
 * Check timeout interrupt of touch channels.
 */
static esp_err_t test_touch_check_ch_intr_timeout(touch_pad_t pad_num)
{
    esp_err_t ret = ESP_FAIL;
    touch_event_t evt = {0};

    while (1) {
        if (pdTRUE == xQueueReceive(que_touch, &evt, 1000 / portTICK_PERIOD_MS)) {
            /* Scan done interrupt have bug that be trigger by last two channel. */
            if (evt.intr_mask & TOUCH_PAD_INTR_MASK_TIMEOUT) {
                if (pad_num == evt.pad_num) {
                    ESP_LOGI(TAG, "touch TIMEOUT be triggered");
                    s_touch_timeout_mask = 0;
                    ret = ESP_OK;
                    touch_pad_timeout_resume();
                    break;
                } else {
                    esp_rom_printf("-timeout %x T[%"PRIu32"] status %"PRIx32", evt_msk %x -\n",
                               s_touch_timeout_mask, evt.pad_num, evt.pad_status, evt.intr_mask);
                    touch_pad_timeout_resume();
                }
            } else {
                continue;
            }
        } else {
            ESP_LOGI(TAG, "Touch intr exceed time");
            break;
        }
    }
    printf("\n");
    return (esp_err_t)ret;
}

static void test_touch_intr_cb(void *arg)
{
    uint32_t cnt, touch_value;
    int task_awoken = pdFALSE;
    touch_event_t evt;
    evt.intr_mask = touch_pad_read_intr_status_mask();
    evt.pad_status = touch_pad_get_status();
    evt.pad_num = touch_pad_get_current_meas_channel();

    if (!evt.intr_mask) {
        return;
    }
    if (evt.intr_mask & TOUCH_PAD_INTR_MASK_SCAN_DONE) {
        touch_pad_read_benchmark(evt.pad_num, &evt.pad_val);
        touch_pad_sleep_channel_t slp_config;
        touch_pad_sleep_channel_get_info(&slp_config);
        touch_pad_sleep_channel_read_benchmark(slp_config.touch_num, &touch_value);
        touch_pad_sleep_channel_read_proximity_cnt(slp_config.touch_num, &cnt);
        evt.slp_proxi_cnt = cnt;
        evt.slp_proxi_base = touch_value;
    }
    if (evt.intr_mask & TOUCH_PAD_INTR_MASK_TIMEOUT) {
        s_touch_timeout_mask |= (BIT(evt.pad_num));
    }

    xQueueSendFromISR(que_touch, &evt, &task_awoken);
    if (task_awoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

/*
 * Test the touch active/inactive interrupt.
 * TEST POINT:
 * 1. Touch interrupt.
 * 2. Raw data noise.
 * 3. smooth data and benchmark data.
 */
esp_err_t test_touch_interrupt(void)
{
    uint32_t touch_value, smooth;
    int test_cnt = TEST_TOUCH_COUNT_NUM;

    ESP_LOGI(TAG, "  >> %s <<", __func__);
    if (que_touch == NULL) {
        que_touch = xQueueCreate(TEST_TOUCH_CHANNEL, sizeof(touch_event_t));
        /* Should register once. */
        touch_pad_isr_register(test_touch_intr_cb, NULL, TOUCH_PAD_INTR_MASK_ALL);
    } else {
        xQueueReset(que_touch);
    }
    TEST_ESP_OK( touch_pad_init() );
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i]) );
    }
    touch_filter_config_t filter_info = {
        .mode = TOUCH_PAD_FILTER_IIR_16,           // Test jitter and filter 1/4.
        .debounce_cnt = 1,      // 1 time count.
        .noise_thr = 0,         // 50%
        .jitter_step = 4,       // use for jitter mode.
        .smh_lvl = TOUCH_PAD_SMOOTH_IIR_2,
    };
    TEST_ESP_OK( touch_pad_filter_set_config(&filter_info) );
    TEST_ESP_OK( touch_pad_filter_enable() );
    /* Register touch interrupt ISR, enable intr type. */
    TEST_ESP_OK( touch_pad_intr_enable(TOUCH_PAD_INTR_MASK_ACTIVE | TOUCH_PAD_INTR_MASK_INACTIVE) );
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER) );
    TEST_ESP_OK( touch_pad_fsm_start() );

    // Initialize and start a software filter to detect slight change of capacitance.
    vTaskDelay(50 / portTICK_PERIOD_MS);

    /* Set threshold of touch sensor */
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_value) );
        TEST_ESP_OK( touch_pad_filter_read_smooth(touch_list[i], &smooth) );
        TEST_ESP_OK( touch_pad_set_thresh(touch_list[i], touch_value * TOUCH_INTR_THRESHOLD) );
        ESP_LOGI(TAG, "test init: touch pad [%d] base %"PRIu32", smooth %"PRIu32", thresh %"PRIu32"",
                 touch_list[i], touch_value, smooth, (uint32_t)(touch_value * TOUCH_INTR_THRESHOLD));
    }

    while (test_cnt--) {
        test_touch_push_all();
        TEST_ESP_OK( test_touch_check_ch_touched(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
        printf_touch_hw_read("push");

        test_touch_benchmark();

        test_touch_release_all();
        TEST_ESP_OK( test_touch_check_ch_released(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
        printf_touch_hw_read("release");
    }

    TEST_ESP_OK( touch_pad_deinit() );

    return ESP_OK;
}

/*
 * Test the touch active/inactive, scan_done interrupt.
 * TEST POINT:
 * 1. Touch interrupt.
 * 2. Raw data noise.
 * 3. smooth data and benchmark data.
 */
esp_err_t test_touch_scan_done_interrupt(void)
{
    ESP_LOGI(TAG, "  >> %s <<", __func__);
    uint32_t touch_value, smooth;
    int test_cnt = TEST_TOUCH_COUNT_NUM;

    if (que_touch == NULL) {
        que_touch = xQueueCreate(TEST_TOUCH_CHANNEL, sizeof(touch_event_t));
        /* Should register once. */
        TEST_ESP_OK( touch_pad_isr_register(test_touch_intr_cb, NULL, TOUCH_PAD_INTR_MASK_ALL) );
    } else {
        xQueueReset(que_touch);
    }
    TEST_ESP_OK( touch_pad_init() );
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i]) );
    }
    touch_filter_config_t filter_info = {
        .mode = TOUCH_PAD_FILTER_IIR_16,           // Test jitter and filter 1/4.
        .debounce_cnt = 1,      // 1 time count.
        .noise_thr = 0,         // 50%
        .jitter_step = 4,       // use for jitter mode.
        .smh_lvl = TOUCH_PAD_SMOOTH_IIR_2,
    };
    TEST_ESP_OK( touch_pad_filter_set_config(&filter_info) );
    TEST_ESP_OK( touch_pad_filter_enable() );
    /* Register touch interrupt ISR, enable intr type. */
    TEST_ESP_OK( touch_pad_intr_enable(TOUCH_PAD_INTR_MASK_SCAN_DONE | TOUCH_PAD_INTR_MASK_ACTIVE | TOUCH_PAD_INTR_MASK_INACTIVE) );
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER) );
    TEST_ESP_OK( touch_pad_fsm_start() );

    /* Check the scan done interrupt */
    TEST_ESP_OK( test_touch_check_ch_intr_scan_done() );

    vTaskDelay(50 / portTICK_PERIOD_MS);

    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_value) );
        TEST_ESP_OK( touch_pad_filter_read_smooth(touch_list[i], &smooth) );
        TEST_ESP_OK( touch_pad_set_thresh(touch_list[i], touch_value * TOUCH_INTR_THRESHOLD) );
        ESP_LOGI(TAG, "test init: touch pad [%d] base %"PRIu32", smooth %"PRIu32", thresh %"PRIu32"", \
                 touch_list[i], touch_value, smooth, (uint32_t)(touch_value * TOUCH_INTR_THRESHOLD));
    }

    while (test_cnt--) {
        test_touch_push_all();
        TEST_ESP_OK( test_touch_check_ch_touched(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
        printf_touch_hw_read("push");

        test_touch_benchmark();

        test_touch_release_all();
        TEST_ESP_OK( test_touch_check_ch_released(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
        printf_touch_hw_read("release");
    }

    TEST_ESP_OK( touch_pad_deinit() );

    return ESP_OK;
}

/*
 * Test the touch active/inactive, timeout interrupt.
 * TEST POINT:
 * 1. Touch interrupt.
 * 2. Raw data noise.
 * 3. smooth data and benchmark data.
 */
esp_err_t test_touch_timeout_interrupt(void)
{
    ESP_LOGI(TAG, "  >> %s <<", __func__);
    uint32_t touch_value, smooth;

    if (que_touch == NULL) {
        que_touch = xQueueCreate(TEST_TOUCH_CHANNEL, sizeof(touch_event_t));
        /* Should register once. */
        TEST_ESP_OK( touch_pad_isr_register(test_touch_intr_cb, NULL, TOUCH_PAD_INTR_MASK_ALL) );
    } else {
        xQueueReset(que_touch);
    }
    TEST_ESP_OK( touch_pad_init() );
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i]) );
    }
    touch_filter_config_t filter_info = {
        .mode = TOUCH_PAD_FILTER_IIR_16,           // Test jitter and filter 1/4.
        .debounce_cnt = 1,      // 1 time count.
        .noise_thr = 0,         // 50%
        .jitter_step = 4,       // use for jitter mode.
        .smh_lvl = TOUCH_PAD_SMOOTH_IIR_2,
    };
    TEST_ESP_OK( touch_pad_filter_set_config(&filter_info) );
    TEST_ESP_OK( touch_pad_filter_enable() );
    /* Register touch interrupt ISR, enable intr type. */
    TEST_ESP_OK( touch_pad_intr_enable(TOUCH_PAD_INTR_MASK_TIMEOUT | TOUCH_PAD_INTR_MASK_ACTIVE | TOUCH_PAD_INTR_MASK_INACTIVE) );
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER) );
    TEST_ESP_OK( touch_pad_fsm_start() );

    // Initialize and start a software filter to detect slight change of capacitance.
    vTaskDelay(50 * SYS_DELAY_TIME_MOM / portTICK_PERIOD_MS);

    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_value) );
        TEST_ESP_OK( touch_pad_filter_read_smooth(touch_list[i], &smooth) );
        TEST_ESP_OK( touch_pad_set_thresh(touch_list[i], touch_value * TOUCH_INTR_THRESHOLD) );
        ESP_LOGI(TAG, "test init: touch pad [%d] base %"PRIu32", smooth %"PRIu32", thresh %"PRIu32"",
                 touch_list[i], touch_value, smooth, (uint32_t)(touch_value * TOUCH_INTR_THRESHOLD));
    }
    /* Set timeout parameter */
    TEST_ESP_OK( touch_pad_read_benchmark(touch_list[0], &touch_value) );
    TEST_ESP_OK( touch_pad_timeout_set(true , touch_value * 10) );

    // Only fake push one touch pad.
    vTaskDelay(50 * SYS_DELAY_TIME_MOM / portTICK_PERIOD_MS);
    test_timeout_trigger_fake(touch_list[0]);
    TEST_ESP_OK( test_touch_check_ch_intr_timeout(touch_list[0]) );
    test_timeout_normal(touch_list[0]);

    vTaskDelay(50 * SYS_DELAY_TIME_MOM / portTICK_PERIOD_MS);
    printf_touch_hw_read("raw  ");
    printf_touch_benchmark_read("base ");
    printf_touch_smooth_read("smooth");

    int test_cnt = TEST_TOUCH_COUNT_NUM;
    while (test_cnt--) {
        test_touch_push_all();
        TEST_ESP_OK( test_touch_check_ch_touched(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
        printf_touch_hw_read("push");

        test_touch_benchmark();

        test_touch_release_all();
        TEST_ESP_OK( test_touch_check_ch_released(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
        printf_touch_hw_read("release");
    }

    TEST_ESP_OK( touch_pad_deinit() );

    return ESP_OK;
}

TEST_CASE("Touch Sensor interrupt test (active, inactive, scan_done, timeout)", "[touch]")
{
    TEST_ESP_OK( test_touch_interrupt() );
    TEST_ESP_OK( test_touch_scan_done_interrupt() );
    TEST_ESP_OK( test_touch_timeout_interrupt() );
}

static void test_touch_measure_step(uint32_t step)
{
    /* Fake the process of debounce. */
    // printf("measure cnt %"PRIu32": [ ", step);
    for (int i = 0; i < step; i++) {
        for (int j = 0; j < TEST_TOUCH_CHANNEL; j++) {
            TEST_ESP_OK( touch_pad_sw_start() );
            while (!touch_pad_meas_is_done()) ;
        }
        // printf(".");
    }
    // printf(" ]\n");
}

/*
 * Test the touch active/inactive, scan_done interrupt.
 * TEST POINT:
 * 1. Touch interrupt.
 * 2. Raw data noise.
 * 3. smooth data and benchmark data.
 */
esp_err_t test_touch_filter_parameter_debounce(int deb_cnt)
{
    uint32_t touch_value;
    int test_cnt = 2;

    ESP_LOGI(TAG, "  >> %s <<", __func__);
    if (que_touch == NULL) {
        que_touch = xQueueCreate(TEST_TOUCH_CHANNEL, sizeof(touch_event_t));
        /* Should register once. */
        TEST_ESP_OK( touch_pad_isr_register(test_touch_intr_cb, NULL, TOUCH_PAD_INTR_MASK_ALL) );
    } else {
        xQueueReset(que_touch);
    }
    TEST_ESP_OK( touch_pad_init() );
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i]) );
    }
    touch_filter_config_t filter_info = {
        .mode = TOUCH_PAD_FILTER_IIR_128,           // Test jitter and filter 1/4.
        .debounce_cnt = ((deb_cnt < 0) ? 1 : deb_cnt) ,      // 1 time count.
        .noise_thr = 0,         // 50%
        .jitter_step = 4,       // use for jitter mode.
        .smh_lvl = TOUCH_PAD_SMOOTH_OFF,
    };
    TEST_ESP_OK( touch_pad_filter_set_config(&filter_info) );
    TEST_ESP_OK( touch_pad_filter_enable() );
    /* Register touch interrupt ISR, enable intr type. */
    TEST_ESP_OK( touch_pad_intr_enable(TOUCH_PAD_INTR_MASK_ACTIVE | TOUCH_PAD_INTR_MASK_INACTIVE) );
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_SW) );
    TEST_ESP_OK( touch_pad_fsm_start() );

    /* Run to wait the data become stable. */
    test_touch_measure_step(20); // 2 scan loop

    /* Set the threshold. */
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_value) );
        TEST_ESP_OK( touch_pad_set_thresh(touch_list[i], touch_value * TOUCH_INTR_THRESHOLD) );
        ESP_LOGI(TAG, "test init: touch pad [%d] base %"PRIu32", thresh %"PRIu32"", \
                 touch_list[i], touch_value, (uint32_t)(touch_value * TOUCH_INTR_THRESHOLD));
    }

    while (test_cnt--) {
        test_touch_push_all();
        /* Fake the process of push debounce. */
        test_touch_measure_step(deb_cnt);       // measure n times. touch state not changed.
        TEST_ESP_ERR( ESP_FAIL, test_touch_check_ch_touched(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
        test_touch_measure_step(1);   // measure n+1 times. touch state changed.
        TEST_ESP_OK( test_touch_check_ch_touched(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
        printf_touch_hw_read("push");

        test_touch_release_all();
        /* Fake the process of release debounce. */
        test_touch_measure_step(deb_cnt);       // measure n times. touch state not changed.
        TEST_ESP_ERR( ESP_FAIL, test_touch_check_ch_released(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
        test_touch_measure_step(1);   // measure n+1 times. touch state changed.
        TEST_ESP_OK( test_touch_check_ch_released(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
        printf_touch_hw_read("release");
    }

    TEST_ESP_OK( touch_pad_deinit() );

    return ESP_OK;
}

esp_err_t test_touch_filter_parameter_reset(int reset_cnt)
{
    uint32_t touch_value, base_value;

    ESP_LOGI(TAG, "  >> %s <<", __func__);
    if (que_touch == NULL) {
        que_touch = xQueueCreate(TEST_TOUCH_CHANNEL, sizeof(touch_event_t));
        /* Should register once. */
        TEST_ESP_OK( touch_pad_isr_register(test_touch_intr_cb, NULL, TOUCH_PAD_INTR_MASK_ALL) );
    } else {
        xQueueReset(que_touch);
    }
    TEST_ESP_OK( touch_pad_init() );
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i]) );
    }
    reset_cnt = ((reset_cnt < 0) ? 10 : reset_cnt);
    touch_filter_config_t filter_info = {
        .mode = TOUCH_PAD_FILTER_IIR_16,           // Test jitter and filter 1/4.
        .debounce_cnt = 1,      // 1 time count.
        .noise_thr = 0,         // 50%
        .jitter_step = 4,       // use for jitter mode.
        .smh_lvl = TOUCH_PAD_SMOOTH_OFF,
    };
    TEST_ESP_OK( touch_pad_filter_set_config(&filter_info) );
    TEST_ESP_OK( touch_pad_filter_enable() );
    /* Register touch interrupt ISR, enable intr type. */
    TEST_ESP_OK( touch_pad_intr_enable(TOUCH_PAD_INTR_MASK_ACTIVE | TOUCH_PAD_INTR_MASK_INACTIVE) );
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_SW) );
    TEST_ESP_OK( touch_pad_fsm_start() );

    /* Run to wait the data become stable. */
    test_touch_measure_step(20); // 2 scan loop

    /* Set the threshold. */
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_value) );
        TEST_ESP_OK( touch_pad_set_thresh(touch_list[i], touch_value * TOUCH_INTR_THRESHOLD) );
        ESP_LOGI(TAG, "test init: touch pad [%d] base %"PRIu32", thresh %"PRIu32"", \
                 touch_list[i], touch_value, (uint32_t)(touch_value * TOUCH_INTR_THRESHOLD));
    }

    /* 1. Fake init status is touched. */
    test_touch_push_all();
    TEST_ESP_OK( touch_pad_reset_benchmark(TOUCH_PAD_MAX) );
    /* Run to wait the data become stable. */
    test_touch_measure_step(20); // 2 scan loop
    printf_touch_hw_read("[raw ] reset:");
    printf_touch_benchmark_read("[base] reset:");

    /* 2. Fake the touch status is released. */
    test_touch_release_all();
    /* 3. Fake measure `reset_cnt + 1` times to reset the benchmark. */
    test_touch_measure_step(reset_cnt);
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_read_raw_data(touch_list[i], &touch_value) );
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &base_value) );
        if ((base_value - touch_value) < (base_value * TOUCH_INTR_THRESHOLD)) {
            ESP_LOGE(TAG, "reset cnt err");
            TEST_FAIL();
        }
    }
    printf_touch_hw_read("[raw ] cnt:");
    printf_touch_benchmark_read("[base] cnt:");

    /*The benchmark on S2 will track the raw data in real time while the channel is not active.
      But on S3, it track the smooth data. And due to the latency of the smooth data,
      the benchmark will be updated to the last smooth data. Thus we have to read smooth data here
      but read benchmark after one measurement step. */
#if CONFIG_IDF_TARGET_ESP32S3
    uint32_t smooth_data[TEST_TOUCH_CHANNEL] = {0};
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_filter_read_smooth(touch_list[i], &(smooth_data[i])) );
    }
#endif
    /* Run 1 time measurement, the benchmark will update after finishing the channel scan*/
    test_touch_measure_step(1);
    printf_touch_hw_read("[raw ] cnt+1:");
    printf_touch_smooth_read("[smooth]cnt+1:");
    printf_touch_benchmark_read("[base] cnt+1:");
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &base_value) );
#if CONFIG_IDF_TARGET_ESP32S2
        /* In ESP32S3, benchmark will update to the raw data. */
        TEST_ESP_OK( touch_pad_read_raw_data(touch_list[i], &touch_value) );
        /* Here we compare the benchmark with raw data directly */
        TEST_ASSERT_EQUAL_UINT32(base_value, touch_value);
#elif CONFIG_IDF_TARGET_ESP32S3
        /* In ESP32S3, benchmark will update to the smooth data. Smooth data is filtered from raw data by IIR.
           Here we compare the benchmark with the previous smooth data*/
        TEST_ASSERT_EQUAL_UINT32(base_value, smooth_data[i]);
#endif
    }

    int test_cnt = 2;
    while (test_cnt--) {
        test_touch_push_all();
        /* Fake the process of push debounce. */
        test_touch_measure_step(filter_info.debounce_cnt + 1);
        TEST_ESP_OK( test_touch_check_ch_touched(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
        printf_touch_hw_read("push");

        test_touch_release_all();
        /* Fake the process of release debounce. */
        test_touch_measure_step(filter_info.debounce_cnt + 1);
        TEST_ESP_OK( test_touch_check_ch_released(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
        printf_touch_hw_read("release");
    }

    TEST_ESP_OK( touch_pad_deinit() );

    return ESP_OK;
}

esp_err_t test_touch_filter_parameter_jitter(int jitter_step)
{
    uint32_t touch_value, base_value = 0;

    ESP_LOGI(TAG, "  >> %s <<", __func__);
    if (que_touch == NULL) {
        que_touch = xQueueCreate(TEST_TOUCH_CHANNEL, sizeof(touch_event_t));
        /* Should register once. */
        TEST_ESP_OK( touch_pad_isr_register(test_touch_intr_cb, NULL, TOUCH_PAD_INTR_MASK_ALL) );
    } else {
        xQueueReset(que_touch);
    }
    TEST_ESP_OK( touch_pad_init() );
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i]) );
    }
    jitter_step = ((jitter_step < 0) ? 4 : jitter_step);
    touch_filter_config_t filter_info = {
        .mode = TOUCH_PAD_FILTER_JITTER,           // Test jitter and filter 1/4.
        .debounce_cnt = 1,      // 1 time count.
        .noise_thr = 0,         // 50%
        .jitter_step = jitter_step,       // use for jitter mode.
    };
    TEST_ESP_OK( touch_pad_filter_set_config(&filter_info) );
    TEST_ESP_OK( touch_pad_filter_enable() );
    /* Register touch interrupt ISR, enable intr type. */
    TEST_ESP_OK( touch_pad_intr_enable(TOUCH_PAD_INTR_MASK_ACTIVE | TOUCH_PAD_INTR_MASK_INACTIVE) );
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_SW) );
    TEST_ESP_OK( touch_pad_fsm_start() );
    /* Run to wait the data become stable. */
    test_touch_measure_step(20); // 2 scan loop

    /* Check the jitter step. */
    printf_touch_benchmark_read("[smooth] t1:");
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        test_touch_measure_step(1);
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_value) );
        test_press_fake(touch_list[i]);
        test_touch_measure_step(1);
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &base_value) );
        TEST_ASSERT_EQUAL_UINT32(jitter_step, (base_value - touch_value));
    }
    printf_touch_benchmark_read("[smooth] t2:");
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        test_touch_measure_step(1);
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_value) );
        test_release_fake(touch_list[i]);
        test_touch_measure_step(1);
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &base_value) );
        TEST_ASSERT_EQUAL_UINT32(jitter_step, (touch_value - base_value));
    }
    printf_touch_benchmark_read("[smooth] t3:");

    /* Set the threshold. */
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        //read benchmark value
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_value) );
        //set interrupt threshold.
        TEST_ESP_OK( touch_pad_set_thresh(touch_list[i], touch_value * TOUCH_INTR_THRESHOLD) );
        ESP_LOGI(TAG, "test init: touch pad [%d] base %"PRIu32", thresh %"PRIu32"", \
                 touch_list[i], touch_value, (uint32_t)(touch_value * TOUCH_INTR_THRESHOLD));
    }

    int test_cnt = 2;
    while (test_cnt--) {
        test_touch_push_all();
        /* Fake the process of push debounce. */
        test_touch_measure_step(filter_info.debounce_cnt + 1);
        TEST_ESP_OK( test_touch_check_ch_touched(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
        printf_touch_smooth_read("push");

        test_touch_release_all();
        /* Fake the process of release debounce. */
        test_touch_measure_step(filter_info.debounce_cnt + 1);
        TEST_ESP_OK( test_touch_check_ch_released(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
        printf_touch_smooth_read("release");
    }

    TEST_ESP_OK( touch_pad_deinit() );

    return ESP_OK;
}

TEST_CASE("Touch Sensor filter paramter test (debounce, reset, jitter)", "[touch]")
{
    ESP_LOGI(TAG, "*********** touch filter debounce test ********************");
    TEST_ESP_OK( test_touch_filter_parameter_debounce(0) );
    TEST_ESP_OK( test_touch_filter_parameter_debounce(3) );
    TEST_ESP_OK( test_touch_filter_parameter_debounce(7) );

    ESP_LOGI(TAG, "*********** touch filter benchmark reset ********************");
    TEST_ESP_OK( test_touch_filter_parameter_reset(0xF) );

    ESP_LOGI(TAG, "*********** touch filter jitter test ********************");
    TEST_ESP_OK( test_touch_filter_parameter_jitter(1) );
    TEST_ESP_OK( test_touch_filter_parameter_jitter(5) );
    TEST_ESP_OK( test_touch_filter_parameter_jitter(15) );
}

esp_err_t test_touch_denoise(uint32_t out_val[], uint32_t *denoise_val, touch_pad_denoise_grade_t grade, touch_pad_denoise_cap_t cap)
{
    uint32_t touch_value;

    ESP_LOGI(TAG, "  >> %s <<", __func__);
    ESP_LOGI(TAG, "Denoise level (%d), cap level (%d) ", grade, cap);
    if (que_touch == NULL) {
        que_touch = xQueueCreate(TEST_TOUCH_CHANNEL, sizeof(touch_event_t));
        /* Should register once. */
        TEST_ESP_OK( touch_pad_isr_register(test_touch_intr_cb, NULL, TOUCH_PAD_INTR_MASK_ALL) );
    } else {
        xQueueReset(que_touch);
    }
    TEST_ESP_OK( touch_pad_init() );
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i]) );
    }
    /* Denoise setting at TouchSensor 0. */
    touch_pad_denoise_t denoise = {
        /* The bits to be cancelled are determined according to the noise level. */
        .grade = (grade < 0) ? TOUCH_PAD_DENOISE_BIT4 : grade,
        .cap_level = (cap < 0) ? TOUCH_PAD_DENOISE_CAP_L4 : cap,
    };
    TEST_ESP_OK( touch_pad_denoise_set_config(&denoise) );
    TEST_ESP_OK( touch_pad_denoise_enable() );
    ESP_LOGI(TAG, "Denoise function init");
    touch_filter_config_t filter_info = {
        .mode = TOUCH_PAD_FILTER_IIR_16,           // Test jitter and filter 1/4.
        .debounce_cnt = 1,      // 1 time count.
        .noise_thr = 0,         // 50%
        .jitter_step = 4,       // use for jitter mode.
    };
    TEST_ESP_OK( touch_pad_filter_set_config(&filter_info) );
    TEST_ESP_OK( touch_pad_filter_enable() );
    /* Register touch interrupt ISR, enable intr type. */
    TEST_ESP_OK( touch_pad_intr_enable(TOUCH_PAD_INTR_MASK_ACTIVE | TOUCH_PAD_INTR_MASK_INACTIVE) );
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_SW) );
    TEST_ESP_OK( touch_pad_fsm_start() );
    /* Run to wait the data become stable. */
    test_touch_measure_step(20); // 2 scan loop

    /* Set the threshold. */
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_value) );
        TEST_ESP_OK( touch_pad_set_thresh(touch_list[i], touch_value * TOUCH_INTR_THRESHOLD) );
        if (out_val) {
            /* Output value for check. */
            out_val[i] = touch_value;
        }
    }
    printf_touch_benchmark_read("Denoise");
    if (denoise_val) {
        touch_pad_denoise_read_data(denoise_val);
    }

    int test_cnt = 1;
    while (test_cnt--) {
        test_touch_push_all();
        /* Fake the process of push debounce. */
        test_touch_measure_step(filter_info.debounce_cnt + 1);
        TEST_ESP_OK( test_touch_check_ch_touched(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );

        test_touch_release_all();
        /* Fake the process of release debounce. */
        test_touch_measure_step(filter_info.debounce_cnt + 1);
        TEST_ESP_OK( test_touch_check_ch_released(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
    }

    TEST_ESP_OK( touch_pad_deinit() );

    return ESP_OK;
}

TEST_CASE("Touch Sensor denoise test (cap, level)", "[touch]")
{
    uint32_t val_1[TEST_TOUCH_CHANNEL];
    uint32_t val_2[TEST_TOUCH_CHANNEL];
    uint32_t val_3[TEST_TOUCH_CHANNEL];
    uint32_t denoise_val[TOUCH_PAD_DENOISE_CAP_MAX];

    ESP_LOGI(TAG, "*********** touch filter denoise level test ********************");
    TEST_ESP_OK( test_touch_denoise(val_1, &denoise_val[0], TOUCH_PAD_DENOISE_BIT4, TOUCH_PAD_DENOISE_CAP_L0) );
    TEST_ESP_OK( test_touch_denoise(val_2, NULL, TOUCH_PAD_DENOISE_BIT8, TOUCH_PAD_DENOISE_CAP_L0) );
    TEST_ESP_OK( test_touch_denoise(val_3, NULL, TOUCH_PAD_DENOISE_BIT12, TOUCH_PAD_DENOISE_CAP_L0) );

    /*`TOUCH_PAD_DENOISE_BIT4` has a small denoise value, which may be smaller than the noise amplitude of the touch reading, so no verification for it.*/
    if ((((denoise_val[0] >> 4) & 0xF) != 0) && (((denoise_val[0] >> 8) & 0xF) != 0)) {
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            TEST_ASSERT_GREATER_THAN(val_3[i], val_2[i]);
        }
    } else {
        /* If the value of denoise is approximately 0,
        The difference between touch reading is very small. Should skip value test. */
        ESP_LOGI(TAG, "denoise value is %"PRIu32"", denoise_val[0]);
    }

    ESP_LOGI(TAG, "*********** touch filter denoise cap level test ********************");
    TEST_ESP_OK( test_touch_denoise(NULL, &denoise_val[0], TOUCH_PAD_DENOISE_BIT8, TOUCH_PAD_DENOISE_CAP_L0) );
    TEST_ESP_OK( test_touch_denoise(NULL, &denoise_val[1], TOUCH_PAD_DENOISE_BIT8, TOUCH_PAD_DENOISE_CAP_L1) );
    TEST_ESP_OK( test_touch_denoise(NULL, &denoise_val[2], TOUCH_PAD_DENOISE_BIT8, TOUCH_PAD_DENOISE_CAP_L2) );
    TEST_ESP_OK( test_touch_denoise(NULL, &denoise_val[3], TOUCH_PAD_DENOISE_BIT8, TOUCH_PAD_DENOISE_CAP_L3) );
    TEST_ESP_OK( test_touch_denoise(NULL, &denoise_val[4], TOUCH_PAD_DENOISE_BIT8, TOUCH_PAD_DENOISE_CAP_L4) );
    TEST_ESP_OK( test_touch_denoise(NULL, &denoise_val[5], TOUCH_PAD_DENOISE_BIT8, TOUCH_PAD_DENOISE_CAP_L5) );
    TEST_ESP_OK( test_touch_denoise(NULL, &denoise_val[6], TOUCH_PAD_DENOISE_BIT8, TOUCH_PAD_DENOISE_CAP_L6) );
    TEST_ESP_OK( test_touch_denoise(NULL, &denoise_val[7], TOUCH_PAD_DENOISE_BIT8, TOUCH_PAD_DENOISE_CAP_L7) );

    printf("denoise read: ");
    for (int i = 0; i < TOUCH_PAD_DENOISE_CAP_MAX - 1; i++) {
        TEST_ASSERT_GREATER_OR_EQUAL(denoise_val[i], denoise_val[i + 1]);
        printf("%"PRIu32" ", denoise_val[i]);
    }
    printf("\n");
}

esp_err_t test_touch_waterproof(void)
{
    uint32_t touch_value;
    int test_cnt = TEST_TOUCH_COUNT_NUM;

    ESP_LOGI(TAG, "  >> %s <<", __func__);
    if (que_touch == NULL) {
        que_touch = xQueueCreate(TEST_TOUCH_CHANNEL, sizeof(touch_event_t));
        /* Should register once. */
        touch_pad_isr_register(test_touch_intr_cb, NULL, TOUCH_PAD_INTR_MASK_ALL);
    } else {
        xQueueReset(que_touch);
    }
    TEST_ESP_OK( touch_pad_init() );
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i]) );
    }
    /* Denoise setting at TouchSensor 0. */
    touch_pad_denoise_t denoise = {
        /* The bits to be cancelled are determined according to the noise level. */
        .grade = TOUCH_PAD_DENOISE_BIT4,
        .cap_level = TOUCH_PAD_DENOISE_CAP_L4,
    };
    TEST_ESP_OK( touch_pad_denoise_set_config(&denoise) );
    TEST_ESP_OK( touch_pad_denoise_enable() );
    ESP_LOGI(TAG, "Denoise function init");
    touch_filter_config_t filter_info = {
        .mode = TOUCH_PAD_FILTER_IIR_16,           // Test jitter and filter 1/4.
        .debounce_cnt = 1,      // 1 time count.
        .noise_thr = 0,         // 50%
        .jitter_step = 4,       // use for jitter mode.
    };
    TEST_ESP_OK( touch_pad_filter_set_config(&filter_info) );
    TEST_ESP_OK( touch_pad_filter_enable() );
    /* Register touch interrupt ISR, enable intr type. */
    TEST_ESP_OK( touch_pad_intr_enable(TOUCH_PAD_INTR_MASK_ACTIVE | TOUCH_PAD_INTR_MASK_INACTIVE) );
    /* Waterproof function */
    touch_pad_waterproof_t waterproof = {
        .guard_ring_pad = TOUCH_WATERPROOF_RING_PAD,   // If no ring pad, set 0;
        /* It depends on the number of the parasitic capacitance of the shield pad. */
        .shield_driver = TOUCH_PAD_SHIELD_DRV_L0,   //40pf
    };
    TEST_ESP_OK( touch_pad_waterproof_set_config(&waterproof) );
    TEST_ESP_OK( touch_pad_waterproof_enable() );
    ESP_LOGI(TAG, "touch pad waterproof init");
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER) );
    TEST_ESP_OK( touch_pad_fsm_start() );

    vTaskDelay(50 / portTICK_PERIOD_MS);

    /* Set the threshold. */
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_value) );
        TEST_ESP_OK( touch_pad_set_thresh(touch_list[i], touch_value * TOUCH_INTR_THRESHOLD) );
    }

    while (test_cnt--) {
        test_touch_push_all();
        vTaskDelay(20 / portTICK_PERIOD_MS);
        TEST_ESP_OK( test_touch_check_ch_touched(TEST_TOUCH_CHANNEL - 1, TOUCH_EXCEED_TIME_MS) ); // take off shield pad
        printf_touch_hw_read("push");

        test_touch_release_all();
        vTaskDelay(20 / portTICK_PERIOD_MS);
        TEST_ESP_OK( test_touch_check_ch_released(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
        printf_touch_hw_read("release");
    }
    TEST_ESP_OK( touch_pad_deinit() );

    return ESP_OK;
}

TEST_CASE("Touch Sensor waterproof guard test", "[touch]")
{
    ESP_LOGI(TAG, "*********** touch filter waterproof guard test ********************");
    TEST_ESP_OK( test_touch_waterproof() );
}

esp_err_t test_touch_proximity(int meas_num)
{
    ESP_LOGI(TAG, "  >> %s <<", __func__);

    uint32_t touch_value;
    if (que_touch == NULL) {
        que_touch = xQueueCreate(TEST_TOUCH_CHANNEL, sizeof(touch_event_t));
        /* Should register once. */
        TEST_ESP_OK( touch_pad_isr_register(test_touch_intr_cb, NULL, TOUCH_PAD_INTR_MASK_ALL) );
    } else {
        xQueueReset(que_touch);
    }
    TEST_ESP_OK( touch_pad_init() );
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i]) );
    }
    /* Denoise setting at TouchSensor 0. */
    touch_pad_denoise_t denoise = {
        /* The bits to be cancelled are determined according to the noise level. */
        .grade = TOUCH_PAD_DENOISE_BIT4,
        .cap_level = TOUCH_PAD_DENOISE_CAP_L4,
    };
    TEST_ESP_OK( touch_pad_denoise_set_config(&denoise) );
    TEST_ESP_OK( touch_pad_denoise_enable() );
    ESP_LOGI(TAG, "Denoise function init");
    touch_filter_config_t filter_info = {
        .mode = TOUCH_PAD_FILTER_IIR_16,           // Test jitter and filter 1/4.
        .debounce_cnt = 1,      // 1 time count.
        .noise_thr = 0,         // 50%
        .jitter_step = 4,       // use for jitter mode.
    };
    TEST_ESP_OK( touch_pad_filter_set_config(&filter_info) );
    TEST_ESP_OK( touch_pad_filter_enable() );
    /* Register touch interrupt ISR, enable intr type. */
    TEST_ESP_OK( touch_pad_intr_enable(TOUCH_PAD_INTR_MASK_ACTIVE | TOUCH_PAD_INTR_MASK_INACTIVE) );
    /* Waterproof function */
    touch_pad_waterproof_t waterproof = {
        .guard_ring_pad = TOUCH_WATERPROOF_RING_PAD,// If no ring pad, set 0;
        /* It depends on the number of the parasitic capacitance of the shield pad. */
        .shield_driver = TOUCH_PAD_SHIELD_DRV_L0,   //40pf
    };
    TEST_ESP_OK( touch_pad_waterproof_set_config(&waterproof) );
    TEST_ESP_OK( touch_pad_waterproof_enable() );
    ESP_LOGI(TAG, "touch pad waterproof init");
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER) );
    TEST_ESP_OK( touch_pad_fsm_start() );

    vTaskDelay(50 / portTICK_PERIOD_MS);

    /* Set the threshold. */
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_value) );
        if (touch_list[i] == proximity_pad[0] ||
                touch_list[i] == proximity_pad[1] ||
                touch_list[i] == proximity_pad[2]) {
            /* The threshold of proximity pad is the sum of touch reading `meas_num` times */
            TEST_ESP_OK( touch_pad_set_thresh(touch_list[i],
                                              meas_num * touch_value * (1 + TOUCH_INTR_THRESHOLD)) );
            ESP_LOGI(TAG, "proximity pad [%d] base %"PRIu32", thresh %"PRIu32"", touch_list[i], touch_value,
                     (uint32_t)(meas_num * touch_value * (1 + TOUCH_INTR_THRESHOLD)));
        } else {
            TEST_ESP_OK( touch_pad_set_thresh(touch_list[i], touch_value * TOUCH_INTR_THRESHOLD) );
            ESP_LOGI(TAG, "touch pad [%d] base %"PRIu32", thresh %"PRIu32"", \
                     touch_list[i], touch_value, (uint32_t)(touch_value * TOUCH_INTR_THRESHOLD));
        }
    }
    /* Should stop the measure, then change the config. */
    while (!touch_pad_meas_is_done());
    TEST_ESP_OK( touch_pad_fsm_stop() );
    /* Proximity function */
    TEST_ESP_OK( touch_pad_proximity_enable(proximity_pad[0], true) );
    TEST_ESP_OK( touch_pad_proximity_enable(proximity_pad[1], true) );
    TEST_ESP_OK( touch_pad_proximity_enable(proximity_pad[2], true) );
    TEST_ESP_OK( touch_pad_proximity_set_count(TOUCH_PAD_MAX, meas_num < 0 ? 16 : meas_num) );
    ESP_LOGI(TAG, "touch pad proximity init");
    TEST_ESP_OK( touch_pad_fsm_start() );

    vTaskDelay(20 / portTICK_PERIOD_MS);
    int test_cnt = TEST_TOUCH_COUNT_NUM;
    while (test_cnt--) {
        test_touch_push_all();
        vTaskDelay(20 / portTICK_PERIOD_MS);
        TEST_ESP_OK( test_touch_check_ch_touched(TEST_TOUCH_CHANNEL - 1, TOUCH_EXCEED_TIME_MS) ); // take off shield pad
        printf_touch_hw_read("push");

        test_touch_release_all();
        vTaskDelay(20 / portTICK_PERIOD_MS);
        TEST_ESP_OK( test_touch_check_ch_released(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
        printf_touch_hw_read("release");
    }

    TEST_ESP_OK( touch_pad_deinit() );

    return ESP_OK;
}

TEST_CASE("Touch Sensor proximity test", "[touch]")
{
    ESP_LOGI(TAG, "*********** touch proximity test ********************");

    TEST_ESP_OK( test_touch_proximity(5) );
    TEST_ESP_OK( test_touch_proximity(1) );
}

esp_err_t test_touch_sleep_reading_stable(touch_pad_t sleep_pad)
{
    uint32_t touch_temp = 0;
    uint32_t touch_value, smooth, ret_val;
    int test_cnt = TEST_TOUCH_COUNT_NUM;

    ESP_LOGI(TAG, "  >> %s <<", __func__);
    if (que_touch == NULL) {
        que_touch = xQueueCreate(TEST_TOUCH_CHANNEL, sizeof(touch_event_t));
        /* Should register once. */
        TEST_ESP_OK( touch_pad_isr_register(test_touch_intr_cb, NULL, TOUCH_PAD_INTR_MASK_ALL) );
    } else {
        xQueueReset(que_touch);
    }
    TEST_ESP_OK( touch_pad_init() );
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i]) );
    }
    // /* Denoise setting at TouchSensor 0. */
    touch_pad_denoise_t denoise = {
        /* The bits to be cancelled are determined according to the noise level. */
        .grade = TOUCH_PAD_DENOISE_BIT4,
        .cap_level = TOUCH_PAD_DENOISE_CAP_L4,
    };
    TEST_ESP_OK( touch_pad_denoise_set_config(&denoise) );
    TEST_ESP_OK( touch_pad_denoise_enable() );
    touch_filter_config_t filter_info = {
        .mode = TOUCH_PAD_FILTER_IIR_16,           // Test jitter and filter 1/4.
        .debounce_cnt = 1,      // 1 time count.
        .noise_thr = 0,         // 50%
        .jitter_step = 4,       // use for jitter mode.
        .smh_lvl = TOUCH_PAD_SMOOTH_OFF,
    };
    TEST_ESP_OK( touch_pad_filter_set_config(&filter_info) );
    TEST_ESP_OK( touch_pad_filter_enable() );
    TEST_ESP_OK( touch_pad_sleep_channel_enable(sleep_pad, true) );
    TEST_ESP_OK( touch_pad_sleep_channel_enable_proximity(sleep_pad, false) );
    /* Register touch interrupt ISR, enable intr type. */
    TEST_ESP_OK( touch_pad_intr_enable(TOUCH_PAD_INTR_MASK_ACTIVE | TOUCH_PAD_INTR_MASK_INACTIVE) );
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER) );
    TEST_ESP_OK( touch_pad_fsm_start() );

    // Initialize and start a software filter to detect slight change of capacitance.
    vTaskDelay(50 * SYS_DELAY_TIME_MOM / portTICK_PERIOD_MS);

    /* Set threshold of touch sensor */
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_value) );
        TEST_ESP_OK( touch_pad_filter_read_smooth(touch_list[i], &smooth) );
        TEST_ESP_OK( touch_pad_set_thresh(touch_list[i], touch_value * TOUCH_INTR_THRESHOLD) );
        ESP_LOGI(TAG, "test init: touch pad [%d] base %"PRIu32", smooth %"PRIu32", thresh %"PRIu32"",
                 touch_list[i], touch_value, smooth, (uint32_t)(touch_value * TOUCH_INTR_THRESHOLD));
    }

    /* Sleep channel setting */
    TEST_ESP_OK( touch_pad_sleep_channel_read_benchmark(sleep_pad, &touch_value) );
    TEST_ESP_OK( touch_pad_sleep_set_threshold(sleep_pad, touch_value * TOUCH_INTR_THRESHOLD) );
    vTaskDelay(50 * SYS_DELAY_TIME_MOM / portTICK_PERIOD_MS);

    while (test_cnt--) {
        /* Touch reading filtered value equal to raw data. */
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            TEST_ESP_OK( touch_pad_sleep_channel_read_data(sleep_pad, &touch_value) );
            TEST_ESP_OK( touch_pad_sleep_channel_read_benchmark(sleep_pad, &touch_temp) );
            TEST_ASSERT_UINT32_WITHIN((uint32_t)((float)touch_temp*TOUCH_READ_ERROR_THRESH), touch_temp, touch_value);
            TEST_ESP_OK( touch_pad_sleep_channel_read_smooth(sleep_pad, &touch_temp) );
            TEST_ASSERT_UINT32_WITHIN((uint32_t)((float)touch_temp*TOUCH_READ_ERROR_THRESH), touch_temp, touch_value);
        }
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            if (touch_temp) {
                TEST_ASSERT_UINT32_WITHIN((uint32_t)((float)touch_temp*TOUCH_READ_ERROR_THRESH), touch_temp, touch_value);
            }
            touch_temp = touch_value;
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
    TEST_ESP_OK( touch_pad_sleep_channel_read_benchmark(sleep_pad, &ret_val) );

    TEST_ESP_OK( touch_pad_deinit() );

    return ret_val;
}


TEST_CASE("Touch Sensor sleep pad reading stable test", "[touch]")
{
    ESP_LOGI(TAG, "*********** touch sleep pad low power (wakeup) test ********************");
    test_touch_sleep_reading_stable(touch_list[0]);
}

/*
 * Test the touch sleep pad interrupt in normal mode.
 * TEST POINT:
 * 1. Touch sleep pad interrupt.
 * 2. sleep pad reading.
 * 3. sleep pad enable proximity.
 */
uint32_t test_touch_sleep_pad_proximity(touch_pad_t sleep_pad, bool is_proximity, uint32_t meas_num)
{
    uint32_t touch_value, smooth, ret_val;
    uint32_t measure_out;
    uint32_t proximity_cnt;
    uint32_t touch_thres;
    int test_cnt = TEST_TOUCH_COUNT_NUM;

    ESP_LOGI(TAG, "  >> %s <<", __func__);
    if (que_touch == NULL) {
        que_touch = xQueueCreate(TEST_TOUCH_CHANNEL, sizeof(touch_event_t));
        /* Should register once. */
        TEST_ESP_OK( touch_pad_isr_register(test_touch_intr_cb, NULL, TOUCH_PAD_INTR_MASK_ALL) );
    } else {
        xQueueReset(que_touch);
    }
    TEST_ESP_OK( touch_pad_init() );
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i]) );
    }
    /* Denoise setting at TouchSensor 0. */
    touch_pad_denoise_t denoise = {
        /* The bits to be cancelled are determined according to the noise level. */
        .grade = TOUCH_PAD_DENOISE_BIT4,
        .cap_level = TOUCH_PAD_DENOISE_CAP_L4,
    };
    TEST_ESP_OK( touch_pad_denoise_set_config(&denoise) );
    TEST_ESP_OK( touch_pad_denoise_enable() );
    touch_filter_config_t filter_info = {
        .mode = TOUCH_PAD_FILTER_IIR_16,           // Test jitter and filter 1/4.
        .debounce_cnt = 1,      // 1 time count.
        .noise_thr = 0,         // 50%
        .jitter_step = 4,       // use for jitter mode.
        .smh_lvl = TOUCH_PAD_SMOOTH_OFF,
    };
    TEST_ESP_OK( touch_pad_filter_set_config(&filter_info) );
    TEST_ESP_OK( touch_pad_filter_enable() );
    /* Sleep channel setting */
    TEST_ESP_OK( touch_pad_sleep_channel_enable(sleep_pad, true) );
    TEST_ESP_OK( touch_pad_sleep_channel_enable_proximity(sleep_pad, is_proximity) );
    /* Register touch interrupt ISR, enable intr type. */
    TEST_ESP_OK( touch_pad_intr_enable(TOUCH_PAD_INTR_MASK_SCAN_DONE | TOUCH_PAD_INTR_MASK_ACTIVE | TOUCH_PAD_INTR_MASK_INACTIVE) );
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER) );
    TEST_ESP_OK( touch_pad_fsm_start() );

    // Initialize and start a software filter to detect slight change of capacitance.
    vTaskDelay(100 * SYS_DELAY_TIME_MOM / portTICK_PERIOD_MS);

    if (is_proximity) {
        /* Set the threshold. */
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            if (touch_list[i] == sleep_pad) {
                touch_pad_sleep_channel_read_smooth(sleep_pad, &touch_value);
                touch_pad_sleep_set_threshold(sleep_pad, meas_num * touch_value * (1 + TOUCH_INTR_THRESHOLD));
                ESP_LOGI(TAG, "Sleep pad [%d] base %"PRIu32", thresh %"PRIu32"", touch_list[i], touch_value,
                         (uint32_t)(meas_num * touch_value * (1 + TOUCH_INTR_THRESHOLD)));
            } else if (touch_list[i] == sleep_pad) {
                touch_pad_sleep_channel_read_smooth(sleep_pad, &touch_value);
                /* The threshold of proximity pad is the sum of touch reading `meas_num` times */
                touch_pad_sleep_set_threshold(sleep_pad, meas_num * touch_value * (1 + TOUCH_INTR_THRESHOLD));
                ESP_LOGI(TAG, "proximity pad [%d] base %"PRIu32", thresh %"PRIu32"", touch_list[i], touch_value,
                         (uint32_t)(meas_num * touch_value * (1 + TOUCH_INTR_THRESHOLD)));
            } else {
                TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_value) );
                TEST_ESP_OK( touch_pad_set_thresh(touch_list[i], touch_value * TOUCH_INTR_THRESHOLD) );
                ESP_LOGI(TAG, "touch pad [%d] base %"PRIu32", thresh %"PRIu32"", \
                         touch_list[i], touch_value, (uint32_t)(touch_value * TOUCH_INTR_THRESHOLD));
            }
        }
        /* Should stop the measure, then change the config. */
        while (!touch_pad_meas_is_done());
        TEST_ESP_OK( touch_pad_fsm_stop() );
        /* Proximity function */
        TEST_ESP_OK( touch_pad_proximity_enable(proximity_pad[0], false) );
        TEST_ESP_OK( touch_pad_proximity_enable(proximity_pad[1], false) );
        TEST_ESP_OK( touch_pad_proximity_enable(proximity_pad[2], false) );
        TEST_ESP_OK( touch_pad_proximity_enable(sleep_pad, true) );
        TEST_ESP_OK( touch_pad_proximity_set_count(TOUCH_PAD_MAX, meas_num) );
        ESP_LOGI(TAG, "touch pad proximity init");
        TEST_ESP_OK( touch_pad_fsm_start() );
        vTaskDelay(100 * SYS_DELAY_TIME_MOM / portTICK_PERIOD_MS);
    } else {
        /* Set threshold of touch sensor */
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_value) );
            TEST_ESP_OK( touch_pad_filter_read_smooth(touch_list[i], &smooth) );
            TEST_ESP_OK( touch_pad_set_thresh(touch_list[i], touch_value * TOUCH_INTR_THRESHOLD) );
            ESP_LOGI(TAG, "test init: touch pad [%d] base %"PRIu32", smooth %"PRIu32", thresh %"PRIu32"",
                     touch_list[i], touch_value, smooth, (uint32_t)(touch_value * TOUCH_INTR_THRESHOLD));
        }
        /* Sleep channel setting */
        TEST_ESP_OK( touch_pad_sleep_channel_read_smooth(sleep_pad, &touch_value) );
        TEST_ESP_OK( touch_pad_sleep_set_threshold(sleep_pad, touch_value * TOUCH_INTR_THRESHOLD) );
        vTaskDelay(50 * SYS_DELAY_TIME_MOM / portTICK_PERIOD_MS);
    }

    TEST_ESP_OK( touch_pad_sleep_channel_read_smooth(sleep_pad, &ret_val) );

    while (test_cnt--) {
        test_touch_push_all();
        TEST_ESP_OK( test_touch_check_ch_touched_with_proximity(TEST_TOUCH_CHANNEL, 5000) );
        printf_touch_hw_read("push");
        if (is_proximity) {
            TEST_ESP_OK( touch_pad_sleep_channel_read_smooth(sleep_pad, &smooth) );
            TEST_ESP_OK( touch_pad_sleep_channel_read_benchmark(sleep_pad, &touch_value) );
            TEST_ESP_OK( touch_pad_proximity_get_data(sleep_pad, &measure_out) );
            TEST_ESP_OK( touch_pad_sleep_channel_read_proximity_cnt(sleep_pad, &proximity_cnt) );
            TEST_ESP_OK( touch_pad_sleep_get_threshold(sleep_pad, &touch_thres) );
            printf("touch slp smooth %"PRIu32", base %"PRIu32", proxi %"PRIu32" cnt %"PRIu32" thres%"PRIu32" status 0x%"PRIx32"\n",
                   smooth, touch_value, measure_out, proximity_cnt,
                   touch_thres, touch_pad_get_status());
        }
        test_touch_release_all();
        TEST_ESP_OK( test_touch_check_ch_released_with_proximity(TEST_TOUCH_CHANNEL, 5000) );
        printf_touch_hw_read("release");
        if (is_proximity) {
            TEST_ESP_OK( touch_pad_sleep_channel_read_smooth(sleep_pad, &smooth) );
            TEST_ESP_OK( touch_pad_sleep_channel_read_benchmark(sleep_pad, &touch_value) );
            TEST_ESP_OK( touch_pad_proximity_get_data(sleep_pad, &measure_out) );
            TEST_ESP_OK( touch_pad_sleep_channel_read_proximity_cnt(sleep_pad, &proximity_cnt) );
            printf("touch slp smooth %"PRIu32", base %"PRIu32", proxi %"PRIu32" cnt %"PRIu32" status 0x%"PRIx32"\n",
                   smooth, touch_value, measure_out, proximity_cnt, touch_pad_get_status());
        }
    }

    TEST_ESP_OK( touch_pad_deinit() );

    return ret_val;
}

TEST_CASE("Touch Sensor sleep pad and proximity interrupt test", "[touch]")
{
    ESP_LOGI(TAG, "*********** touch sleep pad interrupt test ********************");
    test_touch_sleep_pad_proximity(touch_list[0], false, 0);
    test_touch_sleep_pad_proximity(touch_list[0], false, 0);
    test_touch_sleep_pad_proximity(touch_list[0], false, 0);

    ESP_LOGI(TAG, "*********** touch sleep pad interrupt (proximity) test ********************");
    test_touch_sleep_pad_proximity(touch_list[0], true, 1);
    test_touch_sleep_pad_proximity(touch_list[0], true, 3);
    test_touch_sleep_pad_proximity(touch_list[0], true, 5);
}

/*
 * Test the touch sleep pad interrupt in normal mode.
 * TEST POINT:
 * 1. Touch sleep pad interrupt.
 * 2. sleep pad reading.
 * 3. denoise, waterproof
 */
esp_err_t test_touch_sleep_pad_interrupt_wakeup_deep_sleep(touch_pad_t sleep_pad)
{
    uint32_t touch_value, smooth, raw;

    ESP_LOGI(TAG, "  >> %s <<", __func__);
    if (que_touch == NULL) {
        que_touch = xQueueCreate(TEST_TOUCH_CHANNEL, sizeof(touch_event_t));
        /* Should register once. */
        TEST_ESP_OK( touch_pad_isr_register(test_touch_intr_cb, NULL, TOUCH_PAD_INTR_MASK_ALL) );
    } else {
        xQueueReset(que_touch);
    }
    TEST_ESP_OK( touch_pad_init() );
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i]) );
    }
    // /* Denoise setting at TouchSensor 0. */
    touch_pad_denoise_t denoise = {
        /* The bits to be cancelled are determined according to the noise level. */
        .grade = TOUCH_PAD_DENOISE_BIT4,
        .cap_level = TOUCH_PAD_DENOISE_CAP_L4,
    };
    TEST_ESP_OK( touch_pad_denoise_set_config(&denoise) );
    TEST_ESP_OK( touch_pad_denoise_disable() );
    touch_filter_config_t filter_info = {
        .mode = TOUCH_PAD_FILTER_IIR_16,           // Test jitter and filter 1/4.
        .debounce_cnt = 1,      // 1 time count.
        .noise_thr = 0,         // 50%
        .jitter_step = 4,       // use for jitter mode.
        .smh_lvl = TOUCH_PAD_SMOOTH_OFF,
    };
    TEST_ESP_OK( touch_pad_filter_set_config(&filter_info) );
    TEST_ESP_OK( touch_pad_filter_enable() );
    /* Sleep channel setting */
    TEST_ESP_OK( touch_pad_sleep_channel_enable(sleep_pad, true) );
    TEST_ESP_OK( touch_pad_sleep_channel_enable_proximity(sleep_pad, false) );
    /* Register touch interrupt ISR, enable intr type. */
    TEST_ESP_OK( touch_pad_intr_enable(TOUCH_PAD_INTR_MASK_ACTIVE | TOUCH_PAD_INTR_MASK_INACTIVE) );
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER) );
    TEST_ESP_OK( touch_pad_fsm_start() );

    // Initialize and start a software filter to detect slight change of capacitance.
    vTaskDelay(50 * SYS_DELAY_TIME_MOM / portTICK_PERIOD_MS);

    /* Set threshold of touch sensor */
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_value) );
        TEST_ESP_OK( touch_pad_filter_read_smooth(touch_list[i], &smooth) );
        TEST_ESP_OK( touch_pad_set_thresh(touch_list[i], touch_value * TOUCH_INTR_THRESHOLD) );
        ESP_LOGI(TAG, "test init: touch pad [%d] base %"PRIu32", smooth %"PRIu32", thresh %"PRIu32"",
                 touch_list[i], touch_value, smooth, (uint32_t)(touch_value * TOUCH_INTR_THRESHOLD));
    }

    /* Sleep channel setting */
    TEST_ESP_OK( touch_pad_sleep_channel_read_benchmark(sleep_pad, &touch_value) );
    TEST_ESP_OK( touch_pad_sleep_set_threshold(sleep_pad, touch_value * TOUCH_INTR_THRESHOLD) );

    vTaskDelay(50 * SYS_DELAY_TIME_MOM / portTICK_PERIOD_MS);

    test_touch_push_all();
    TEST_ESP_OK( test_touch_check_ch_touched(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
    printf_touch_hw_read("push");
    TEST_ESP_OK( touch_pad_sleep_channel_read_smooth(sleep_pad, &smooth) );
    TEST_ESP_OK( touch_pad_sleep_channel_read_data(sleep_pad, &raw) );
    TEST_ESP_OK( touch_pad_sleep_channel_read_benchmark(sleep_pad, &touch_value) );
    printf("touch slp raw %"PRIu32", smooth %"PRIu32", base %"PRIu32", status 0x%"PRIx32"\n", raw, smooth, touch_value, touch_pad_get_status());

    test_touch_release_all();
    TEST_ESP_OK( test_touch_check_ch_released(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
    printf_touch_hw_read("release");
    TEST_ESP_OK( touch_pad_sleep_channel_read_smooth(sleep_pad, &smooth) );
    TEST_ESP_OK( touch_pad_sleep_channel_read_data(sleep_pad, &raw) );
    TEST_ESP_OK( touch_pad_sleep_channel_read_benchmark(sleep_pad, &touch_value) );
    printf("touch slp raw %"PRIu32", smooth %"PRIu32", base %"PRIu32", status 0x%"PRIx32"\n", raw, smooth, touch_value, touch_pad_get_status());

    return ESP_OK;
}

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32S3) //TODO: IDF-5218

#include <sys/time.h>
#include "esp_sleep.h"

static RTC_DATA_ATTR struct timeval sleep_enter_time;

static void test_deep_sleep_init(void)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    int sleep_time_ms = (now.tv_sec - sleep_enter_time.tv_sec) * 1000 + (now.tv_usec - sleep_enter_time.tv_usec) / 1000;
    printf("RTC_CNTL_SLP_WAKEUP_CAUSE_REG %x\n", REG_READ(RTC_CNTL_SLP_WAKEUP_CAUSE_REG));
    switch (esp_sleep_get_wakeup_cause()) {
    case ESP_SLEEP_WAKEUP_EXT1: {
        uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
        if (wakeup_pin_mask != 0) {
            int pin = __builtin_ffsll(wakeup_pin_mask) - 1;
            printf("Wake up from GPIO %"PRIu32"\n", pin);
        } else {
            printf("Wake up from GPIO\n");
        }
        break;
    }
    case ESP_SLEEP_WAKEUP_TIMER: {
        printf("Wake up from timer. Time spent in deep sleep: %"PRIu32"ms\n", sleep_time_ms);
        break;
    }
    case ESP_SLEEP_WAKEUP_TOUCHPAD: {
        printf("Wake up from touch on pad %"PRIu32"\n", esp_sleep_get_touchpad_wakeup_status());
        break;
    }
    case ESP_SLEEP_WAKEUP_UNDEFINED:
    default: {
        printf("Not a deep sleep reset\n");
        ESP_LOGI(TAG, "*********** touch sleep pad wakeup test ********************");
        /* Sleep pad should be init once. */
        test_touch_sleep_pad_interrupt_wakeup_deep_sleep(touch_list[0]);
    }
    }

    vTaskDelay(100 * SYS_DELAY_TIME_MOM / portTICK_PERIOD_MS);

    printf("Enabling touch pad wakeup\n");
    esp_sleep_enable_touchpad_wakeup();

    printf("Entering deep sleep\n");
    gettimeofday(&sleep_enter_time, NULL);
}

TEST_CASE("Touch Sensor sleep pad wakeup deep sleep test", "[touch][ignore]")
{
    test_deep_sleep_init();

    /* Change the work duty of touch sensor to reduce current. */
    touch_pad_set_measurement_interval(100);
    touch_pad_set_charge_discharge_times(TOUCH_PAD_MEASURE_CYCLE_DEFAULT);

    /* Close PD current in deep sleep. */
    RTCCNTL.bias_conf.pd_cur_deep_slp = 1;
    RTCCNTL.bias_conf.pd_cur_monitor = 1;
    RTCCNTL.bias_conf.bias_sleep_deep_slp = 1;
    RTCCNTL.bias_conf.bias_sleep_monitor = 1;

    esp_deep_sleep_start();
}
#endif //TEMPORARY_DISABLED_FOR_TARGETS(..)

#include "touch_scope.h"
/*
 * 0: 10 channels raw/smooth/benchmark data debug.
 * 1: 5  channges smooth + benchmark data debug.
 * 2: 1  channels filter data.
 */
#define SCOPE_DEBUG_TYPE            2
#define TOUCH_THRESHOLD             0.5
#define TOUCH_SHELD_PAD            (1)
#define SCOPE_DEBUG_CHANNEL_MAX    (10)
#define SCOPE_DEBUG_ENABLE         (0)
#define SCOPE_UART_BUADRATE        (256000)
#define SCOPE_DEBUG_FREQ_MS        (50)

void test_touch_slope_debug(int pad_num)
{
    touch_event_t evt;
    uint32_t touch_value, smooth;

    ESP_LOGI(TAG, "  >> %s <<", __func__);
    if (que_touch == NULL) {
        que_touch = xQueueCreate(TEST_TOUCH_CHANNEL, sizeof(touch_event_t));
        /* Should register once. */
        touch_pad_isr_register(test_touch_intr_cb, NULL, TOUCH_PAD_INTR_MASK_ALL);
    } else {
        xQueueReset(que_touch);
    }
    TEST_ESP_OK( touch_pad_init() );
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_config(touch_list[i]) );
    }
    touch_filter_config_t filter_info = {
        .mode = TOUCH_PAD_FILTER_IIR_32,           // Test jitter and filter 1/4.
        .debounce_cnt = 1,      // 1 time count.
        .noise_thr = 3,         // 50%
        .jitter_step = 4,       // use for jitter mode.
        .smh_lvl = TOUCH_PAD_SMOOTH_IIR_2,
    };
    TEST_ESP_OK( touch_pad_filter_set_config(&filter_info) );
    TEST_ESP_OK( touch_pad_filter_enable() );
    /* Register touch interrupt ISR, enable intr type. */
    TEST_ESP_OK( touch_pad_intr_enable(TOUCH_PAD_INTR_MASK_ACTIVE | TOUCH_PAD_INTR_MASK_INACTIVE) );
    TEST_ESP_OK( touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER) );
    TEST_ESP_OK( touch_pad_fsm_start() );
    /* Waterproof function */
    touch_pad_waterproof_t waterproof = {
        .guard_ring_pad = 0,   // If no ring pad, set 0;
        /* It depends on the number of the parasitic capacitance of the shield pad. */
        .shield_driver = TOUCH_PAD_SHIELD_DRV_L2,   //40pf
    };
    TEST_ESP_OK( touch_pad_waterproof_set_config(&waterproof) );
    TEST_ESP_OK( touch_pad_waterproof_enable() );
    ESP_LOGI(TAG, "touch pad waterproof init");

    // Initialize and start a software filter to detect slight change of capacitance.
    vTaskDelay(50 / portTICK_PERIOD_MS);

    /* Set threshold of touch sensor */
    for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
        TEST_ESP_OK( touch_pad_read_benchmark(touch_list[i], &touch_value) );
        TEST_ESP_OK( touch_pad_filter_read_smooth(touch_list[i], &smooth) );
        TEST_ESP_OK( touch_pad_set_thresh(touch_list[i], touch_value * TOUCH_THRESHOLD) );
        ESP_LOGI(TAG, "test init: touch pad [%d] base %"PRIu32", smooth %"PRIu32", thresh %"PRIu32"", \
                 touch_list[i], touch_value, smooth, (uint32_t)(touch_value * TOUCH_THRESHOLD));
    }

    float scope_temp[SCOPE_DEBUG_CHANNEL_MAX] = {0};  // max scope channel is 10.
    uint32_t scope_data[SCOPE_DEBUG_CHANNEL_MAX] = {0};  // max scope channel is 10.
    test_tp_scope_debug_init(0, -1, -1, SCOPE_UART_BUADRATE);

#if SCOPE_DEBUG_TYPE == 0
    while (1) {
        for (int i = 0; i < TEST_TOUCH_CHANNEL; i++) {
            touch_pad_read_raw_data(touch_list[i], &scope_data[i]);
            // touch_pad_filter_read_smooth(touch_list[i], &scope_data[i]);
            // touch_pad_read_benchmark(touch_list[i], &scope_data[i]);
            scope_temp[i] = scope_data[i];
        }
        test_tp_print_to_scope(scope_temp, TEST_TOUCH_CHANNEL);
        vTaskDelay(SCOPE_DEBUG_FREQ_MS / portTICK_PERIOD_MS);
    }
#elif SCOPE_DEBUG_TYPE == 1
    while (1) {
        int cnt = 0;
        for (int i = 0; i < 5; i++) {
            touch_pad_read_raw_data(touch_list[i], &scope_data[i]);
            scope_temp[i] = scope_data[i];
        }
        for (int i = 0; i < 5; i++) {
            touch_pad_filter_read_smooth(touch_list[i], &scope_data[i]);
            scope_temp[i + SCOPE_DEBUG_CHANNEL_MAX / 2] = scope_data[i];
        }
        test_tp_print_to_scope(scope_temp, SCOPE_DEBUG_CHANNEL_MAX);
        vTaskDelay(SCOPE_DEBUG_FREQ_MS / portTICK_PERIOD_MS);
    }
#elif SCOPE_DEBUG_TYPE == 2
    uint32_t status;
    touch_pad_read_benchmark(pad_num, &status);
    while (1) {
        xQueueReceive(que_touch, &evt, SCOPE_DEBUG_FREQ_MS / portTICK_PERIOD_MS);
        //read filtered value
        touch_pad_read_raw_data(pad_num, &scope_data[0]);
        touch_pad_read_benchmark(pad_num, &scope_data[1]);
        touch_pad_get_thresh(pad_num, &scope_data[2]);
        touch_pad_filter_read_smooth(pad_num, &scope_data[8]);
        // raw data
        scope_temp[0] = scope_data[0];
        // benchmark
        scope_temp[1] = scope_data[1];
        // smooth data
        scope_temp[8] = scope_data[8];
        // noise thr
        scope_temp[2] = scope_temp[1] - scope_data[2] * 0.5;
        // noise thr
        scope_temp[3] = scope_temp[1] + scope_data[2] * 0.5;
        // touch thr
        scope_temp[4] = scope_temp[1] + scope_data[2];
        scope_temp[5] = scope_temp[4] - scope_data[2] * 0.0625;
        scope_temp[6] = scope_temp[4] + scope_data[2] * 0.0625;
        // touch status
        if (touch_pad_get_status() & BIT(pad_num)) {
            scope_temp[7] = status + 100;
        } else {
            scope_temp[7] = status - 100;  //0:release; 1:push;
        }
        test_tp_print_to_scope(scope_temp, 9);
    }
#elif SCOPE_DEBUG_TYPE == 3
    while (1) {
        test_touch_push_all();
        TEST_ESP_OK( test_touch_check_ch_touched(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
        printf_touch_hw_read("push");

        test_touch_benchmark();

        test_touch_release_all();
        TEST_ESP_OK( test_touch_check_ch_released(TEST_TOUCH_CHANNEL, TOUCH_EXCEED_TIME_MS) );
        printf_touch_hw_read("release");
    }
#endif
    TEST_ESP_OK( touch_pad_deinit() );
}
