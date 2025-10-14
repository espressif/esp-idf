/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "driver/touch_sens.h"
#include "hal/touch_sensor_ll.h"
#include "esp_log.h"
#include "esp_attr.h"

static touch_sensor_sample_config_t s_sample_cfg[TOUCH_SAMPLE_CFG_NUM] = {
#if SOC_TOUCH_SENSOR_VERSION == 1
    TOUCH_SENSOR_V1_DEFAULT_SAMPLE_CONFIG(5.0, TOUCH_VOLT_LIM_L_0V5, TOUCH_VOLT_LIM_H_1V7),
#elif SOC_TOUCH_SENSOR_VERSION == 2
    TOUCH_SENSOR_V2_DEFAULT_SAMPLE_CONFIG(500, TOUCH_VOLT_LIM_L_0V5, TOUCH_VOLT_LIM_H_2V2),
#elif SOC_TOUCH_SENSOR_VERSION == 3
    TOUCH_SENSOR_V3_DEFAULT_SAMPLE_CONFIG(1, 1, 1),
    TOUCH_SENSOR_V3_DEFAULT_SAMPLE_CONFIG(2, 1, 1),
    TOUCH_SENSOR_V3_DEFAULT_SAMPLE_CONFIG(4, 1, 1),
#else
#error "Target not support"
#endif
};

static touch_channel_config_t s_chan_cfg = {
#if SOC_TOUCH_SENSOR_VERSION == 1
    .abs_active_thresh = {1000},
    .charge_speed = TOUCH_CHARGE_SPEED_7,
    .init_charge_volt = TOUCH_INIT_CHARGE_VOLT_DEFAULT,
    .group = TOUCH_CHAN_TRIG_GROUP_BOTH,
#elif SOC_TOUCH_SENSOR_VERSION == 2
    .active_thresh = {
        2000,
    },
    .charge_speed = TOUCH_CHARGE_SPEED_7,
    .init_charge_volt = TOUCH_INIT_CHARGE_VOLT_DEFAULT,
#elif SOC_TOUCH_SENSOR_VERSION == 3
    .active_thresh = {
        5000,
        2500,
        1000,
    },
#endif
};

TEST_CASE("touch_sens_install_uninstall_test", "[touch]")
{
    touch_sensor_handle_t touch = NULL;
    touch_channel_handle_t touch_chan[TOUCH_TOTAL_CHAN_NUM] = {NULL};

    touch_sensor_config_t sens_cfg = TOUCH_SENSOR_DEFAULT_BASIC_CONFIG(TOUCH_SAMPLE_CFG_NUM, s_sample_cfg);
    /* Allocate new controller */
    TEST_ESP_OK(touch_sensor_new_controller(&sens_cfg, &touch));
    TEST_ASSERT(touch_sensor_new_controller(&sens_cfg, &touch) == ESP_ERR_INVALID_STATE);
    /* Configuring the filter */
    touch_sensor_filter_config_t filter_cfg = TOUCH_SENSOR_DEFAULT_FILTER_CONFIG();
    TEST_ESP_OK(touch_sensor_config_filter(touch, &filter_cfg));

    for (int i = 0; i < TOUCH_TOTAL_CHAN_NUM; i++) {
        TEST_ESP_OK(touch_sensor_new_channel(touch, i + TOUCH_MIN_CHAN_ID, &s_chan_cfg, &touch_chan[i]));
    }
    touch_channel_handle_t fault_chan = NULL;
    TEST_ASSERT(touch_sensor_new_channel(touch, TOUCH_TOTAL_CHAN_NUM + TOUCH_MIN_CHAN_ID, &s_chan_cfg, &fault_chan) == ESP_ERR_INVALID_ARG);
    TEST_ASSERT(touch_sensor_new_channel(touch, TOUCH_MIN_CHAN_ID, &s_chan_cfg, &fault_chan) == ESP_ERR_INVALID_STATE);

    TEST_ESP_OK(touch_sensor_enable(touch));
    TEST_ASSERT(touch_sensor_del_channel(touch_chan[0]) == ESP_ERR_INVALID_STATE);
    TEST_ESP_OK(touch_sensor_disable(touch));

    TEST_ASSERT(touch_sensor_del_controller(touch) == ESP_ERR_INVALID_STATE);

    for (int i = 0; i < TOUCH_TOTAL_CHAN_NUM; i++) {
        TEST_ESP_OK(touch_sensor_del_channel(touch_chan[i]));
    }
    TEST_ESP_OK(touch_sensor_del_controller(touch));
}

typedef struct {
    int active_count;
    int inactive_count;
#if SOC_TOUCH_SENSOR_VERSION == 1
    int hw_active_count;
#endif
} test_touch_cb_data_t;

static touch_channel_config_t s_test_get_chan_cfg_by_benchmark(uint32_t benchmark[], uint32_t num, float coeff)
{
    touch_channel_config_t chan_cfg = s_chan_cfg;
    for (int i = 0; i < num; i++) {
#if SOC_TOUCH_SENSOR_VERSION == 1
        chan_cfg.abs_active_thresh[i] = benchmark[i] * (1 - coeff);
        printf("[Sampler %d] benchmark %5"PRIu32" abs thresh %4"PRIu32"\n",
               i, benchmark[i], chan_cfg.abs_active_thresh[i]);
#else
        chan_cfg.active_thresh[i] = benchmark[i] * coeff;
        printf("[Sampler %d] benchmark %5"PRIu32" thresh %4"PRIu32"\n",
               i, benchmark[i], chan_cfg.active_thresh[i]);
#endif
    }
    return chan_cfg;
}

static void s_test_touch_do_initial_scanning(touch_sensor_handle_t touch, int scan_times)
{
    /* Enable the touch sensor to do the initial scanning, so that to initialize the channel data */
    TEST_ESP_OK(touch_sensor_enable(touch));
    /* Scan the enabled touch channels for several times, to make sure the initial channel data is stable */
    for (int i = 0; i < scan_times; i++) {
        TEST_ESP_OK(touch_sensor_trigger_oneshot_scanning(touch, 2000));
    }
    /* Disable the touch channel to rollback the state */
    TEST_ESP_OK(touch_sensor_disable(touch));
}

#if CONFIG_TOUCH_ISR_IRAM_SAFE
#define TEST_TCH_IRAM_ATTR  IRAM_ATTR
#else
#define TEST_TCH_IRAM_ATTR
#endif

#if SOC_TOUCH_SENSOR_VERSION == 1
static bool TEST_TCH_IRAM_ATTR s_test_touch_on_hw_active_callback(touch_sensor_handle_t sens_handle, const touch_hw_active_event_data_t *event, void *user_ctx)
{
    test_touch_cb_data_t *cb_data = (test_touch_cb_data_t *)user_ctx;
    cb_data->hw_active_count++;
    return false;
}
#endif

static bool TEST_TCH_IRAM_ATTR s_test_touch_on_active_callback(touch_sensor_handle_t sens_handle, const touch_active_event_data_t *event, void *user_ctx)
{
    ESP_EARLY_LOGI("touch_callback", "[CH %d] active", (int)event->chan_id);
    test_touch_cb_data_t *cb_data = (test_touch_cb_data_t *)user_ctx;
    cb_data->active_count++;
    return false;
}

static bool TEST_TCH_IRAM_ATTR s_test_touch_on_inactive_callback(touch_sensor_handle_t sens_handle, const touch_inactive_event_data_t *event, void *user_ctx)
{
    ESP_EARLY_LOGI("touch_callback", "[CH %d] inactive", (int)event->chan_id);
    test_touch_cb_data_t *cb_data = (test_touch_cb_data_t *)user_ctx;
    cb_data->inactive_count++;
    return false;
}

static void s_test_touch_simulate_touch(touch_sensor_handle_t touch, touch_channel_handle_t touch_chan, bool active)
{
#if SOC_TOUCH_SENSOR_VERSION <= 2
    touch_chan_info_t chan_info = {};
    touch_sensor_get_channel_info(touch_chan, &chan_info);
    touch_ll_set_charge_speed(chan_info.chan_id, active ? TOUCH_CHARGE_SPEED_4 : TOUCH_CHARGE_SPEED_7);
#elif SOC_TOUCH_SENSOR_VERSION == 3
    touch_ll_set_internal_capacitor(active ? 0x7f : 0);
#endif
}

static void s_test_touch_log_data(touch_channel_handle_t touch_chan, uint32_t sample_cfg_num, const char *tag)
{
    uint32_t data[sample_cfg_num];
    TEST_ESP_OK(touch_channel_read_data(touch_chan, TOUCH_CHAN_DATA_TYPE_SMOOTH, data));
    printf("%s:", tag);
    for (int i = 0; i < sample_cfg_num; i++) {
        printf(" %"PRIu32, data[i]);
    }
    printf("\n");
}

#define TEST_ACTIVE_THRESH_RATIO    (0.01f)

TEST_CASE("touch_sens_active_inactive_test", "[touch]")
{
    touch_sensor_handle_t touch = NULL;
    touch_channel_handle_t touch_chan = NULL;

    touch_sensor_config_t sens_cfg = TOUCH_SENSOR_DEFAULT_BASIC_CONFIG(TOUCH_SAMPLE_CFG_NUM, s_sample_cfg);
    TEST_ESP_OK(touch_sensor_new_controller(&sens_cfg, &touch));

    /* Configuring the filter */
    touch_sensor_filter_config_t filter_cfg = TOUCH_SENSOR_DEFAULT_FILTER_CONFIG();
    TEST_ESP_OK(touch_sensor_config_filter(touch, &filter_cfg));
    TEST_ESP_OK(touch_sensor_new_channel(touch, TOUCH_MIN_CHAN_ID, &s_chan_cfg, &touch_chan));
#if SOC_TOUCH_SENSOR_VERSION == 3
    /* Connect the touch channels to the internal capacitor */
    touch_ll_enable_internal_capacitor(true);
#endif  // SOC_TOUCH_SENSOR_VERSION == 3

    s_test_touch_do_initial_scanning(touch, 3);

    /* Read benchmark */
    uint32_t benchmark[TOUCH_SAMPLE_CFG_NUM] = {0};
#if SOC_TOUCH_SUPPORT_BENCHMARK
    touch_chan_data_type_t data_type = TOUCH_CHAN_DATA_TYPE_BENCHMARK;
#else
    touch_chan_data_type_t data_type = TOUCH_CHAN_DATA_TYPE_SMOOTH;
#endif  // SOC_TOUCH_SUPPORT_BENCHMARK
    TEST_ESP_OK(touch_channel_read_data(touch_chan, data_type, benchmark));
    /* Test whether success to finish the initial scanning */
    for (int i = 0; i < TOUCH_SAMPLE_CFG_NUM; i++) {
        TEST_ASSERT_GREATER_THAN(0, benchmark[i]);
    }
    /* Re-configure the threshold according to the benchmark */
    touch_channel_config_t chan_cfg = s_test_get_chan_cfg_by_benchmark(benchmark, TOUCH_SAMPLE_CFG_NUM, TEST_ACTIVE_THRESH_RATIO);
    TEST_ESP_OK(touch_sensor_reconfig_channel(touch_chan, &chan_cfg));
    touch_event_callbacks_t callbacks = {
        .on_active = s_test_touch_on_active_callback,
        .on_inactive = s_test_touch_on_inactive_callback,
#if SOC_TOUCH_SENSOR_VERSION == 1
        .on_hw_active = s_test_touch_on_hw_active_callback,
#endif
    };
    test_touch_cb_data_t cb_data = {};
    TEST_ESP_OK(touch_sensor_register_callbacks(touch, &callbacks, &cb_data));

    TEST_ESP_OK(touch_sensor_enable(touch));
    TEST_ESP_OK(touch_sensor_start_continuous_scanning(touch));
    vTaskDelay(pdMS_TO_TICKS(20));

    int touch_cnt = 3;
    for (int i = 0; i < touch_cnt; i++) {
        printf("\nSimulate Touch [%d] ->\n--------------------------\n", i + 1);
        // Read data before touched
        s_test_touch_log_data(touch_chan, TOUCH_SAMPLE_CFG_NUM, "Data Before");
        // Simulate touch
        s_test_touch_simulate_touch(touch, touch_chan, true);
        vTaskDelay(pdMS_TO_TICKS(100));

        // Read data after touched
        s_test_touch_log_data(touch_chan, TOUCH_SAMPLE_CFG_NUM, "Data After ");
        // Simulate release
        s_test_touch_simulate_touch(touch, touch_chan, false);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    printf("\n");

    TEST_ESP_OK(touch_sensor_stop_continuous_scanning(touch));
    TEST_ESP_OK(touch_sensor_disable(touch));
    TEST_ESP_OK(touch_sensor_del_channel(touch_chan));
    TEST_ESP_OK(touch_sensor_del_controller(touch));

    /* Check the callback count */
#if SOC_TOUCH_SENSOR_VERSION == 1
    // The Touch V1 interrupt will keep triggering as long as the channel data is below the threshold
    // So it might be greater than the touch count
    printf("hardware active interrupt count: %d\n", cb_data.hw_active_count);
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(touch_cnt, cb_data.hw_active_count);
#endif  // SOC_TOUCH_SENSOR_VERSION == 1
    TEST_ASSERT_EQUAL_INT32(touch_cnt, cb_data.active_count);
    TEST_ASSERT_EQUAL_INT32(touch_cnt, cb_data.inactive_count);
}
