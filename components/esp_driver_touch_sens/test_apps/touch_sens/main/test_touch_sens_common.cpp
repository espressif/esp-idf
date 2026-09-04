/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
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
#include "esp_heap_caps.h"
#include "test_helper.h"
#include "esp_sleep.h"

#if SOC_LP_CORE_SUPPORTED
#include "ulp_lp_core.h"
#elif SOC_RISCV_COPROC_SUPPORTED
#include "ulp_riscv.h"
#elif SOC_ULP_FSM_SUPPORTED
#include "ulp.h"
#include "ulp_main.h"
#endif

/**************************** Configurations ****************************/

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

static const touch_sensor_config_t s_sens_cfg = TOUCH_SENSOR_DEFAULT_BASIC_CONFIG(TOUCH_SAMPLE_CFG_NUM, s_sample_cfg);

static const touch_channel_config_t s_chan_cfg = {
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

static const touch_sensor_filter_config_t s_filter_cfg = TOUCH_SENSOR_DEFAULT_FILTER_CONFIG();

/**************************** Callback ****************************/

typedef struct {
    int active_count;
    int inactive_count;
#if SOC_TOUCH_SENSOR_VERSION == 1
    int hw_active_count;
#endif
} test_touch_cb_data_t;

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

/**************************** Helper Functions ****************************/

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

#define TEST_ACTIVE_THRESH_RATIO          (0.01f)

static touch_channel_config_t s_test_get_chan_cfg_by_benchmark(uint32_t benchmark[])
{
    touch_channel_config_t chan_cfg = s_chan_cfg;
    for (int i = 0; i < TOUCH_SAMPLE_CFG_NUM; i++) {
#if SOC_TOUCH_SENSOR_VERSION == 1
        chan_cfg.abs_active_thresh[i] = benchmark[i] * (1 - TEST_ACTIVE_THRESH_RATIO);
        printf("[Sampler %d] benchmark %5" PRIu32 " abs thresh %4" PRIu32 "\n",
               i, benchmark[i], chan_cfg.abs_active_thresh[i]);
#else
        chan_cfg.active_thresh[i] = benchmark[i] * TEST_ACTIVE_THRESH_RATIO;
        printf("[Sampler %d] benchmark %5" PRIu32 " thresh %4" PRIu32 "\n",
               i, benchmark[i], chan_cfg.active_thresh[i]);
#endif
    }
    return chan_cfg;
}

typedef struct {
    touch_sensor_handle_t sensor;
    touch_channel_handle_t channels[];
} test_touch_fixture_t;

static void s_test_touch_fixture_init(test_touch_fixture_t *fixture, size_t num, const uint32_t* chan_ids, test_touch_cb_data_t *cb_data)
{
    TEST_ASSERT(fixture && num > 0 && chan_ids && cb_data);

    TEST_ESP_OK(touch_sensor_new_controller(&s_sens_cfg, &fixture->sensor));
    TEST_ESP_OK(touch_sensor_config_filter(fixture->sensor, &s_filter_cfg));
    for (size_t i = 0; i < num; i++) {
        TEST_ESP_OK(touch_sensor_new_channel(fixture->sensor, chan_ids[i], &s_chan_cfg, &fixture->channels[i]));
    }

#if SOC_TOUCH_SENSOR_VERSION == 3 && !SOC_IS(ESP32S31)
    /* Connect the touch channels to the internal capacitor */
    touch_ll_enable_internal_capacitor(true);
#endif

    s_test_touch_do_initial_scanning(fixture->sensor, 3);

    /* Re-configure the threshold according to the benchmark */
    for (size_t i = 0; i < num; i++) {
        uint32_t benchmark[TOUCH_SAMPLE_CFG_NUM] = {};
#if SOC_TOUCH_SUPPORT_BENCHMARK
        TEST_ESP_OK(touch_channel_read_data(fixture->channels[i], TOUCH_CHAN_DATA_TYPE_BENCHMARK, benchmark));
#else
        TEST_ESP_OK(touch_channel_read_data(fixture->channels[i], TOUCH_CHAN_DATA_TYPE_SMOOTH, benchmark));
#endif
        for (size_t j = 0; j < TOUCH_SAMPLE_CFG_NUM; j++) {
            TEST_ASSERT_GREATER_THAN(0, benchmark[j]);
        }
        touch_channel_config_t chan_cfg = s_test_get_chan_cfg_by_benchmark(benchmark);
        TEST_ESP_OK(touch_sensor_reconfig_channel(fixture->channels[i], &chan_cfg));
    }

    /* Register the callbacks */
    touch_event_callbacks_t callbacks = {};
    callbacks.on_active = s_test_touch_on_active_callback;
    callbacks.on_inactive = s_test_touch_on_inactive_callback;
#if SOC_TOUCH_SENSOR_VERSION == 1
    callbacks.on_hw_active = s_test_touch_on_hw_active_callback;
#endif
    TEST_ESP_OK(touch_sensor_register_callbacks(fixture->sensor, &callbacks, cb_data));
}

/**************************** Test Cases ****************************/

TEST_CASE("touch_sens_install_uninstall_test", "[touch]")
{
    touch_sensor_handle_t touch = NULL;
    touch_channel_handle_t touch_chan[TOUCH_TOTAL_CHAN_NUM] = {};

    /* Allocate new controller */
    TEST_ESP_OK(touch_sensor_new_controller(&s_sens_cfg, &touch));
    TEST_ASSERT(touch_sensor_new_controller(&s_sens_cfg, &touch) == ESP_ERR_INVALID_STATE);
    /* Configuring the filter */
    TEST_ESP_OK(touch_sensor_config_filter(touch, &s_filter_cfg));

    /* Allocate all channels */
    for (int i = 0; i < TOUCH_TOTAL_CHAN_NUM; i++) {
        TEST_ESP_OK(touch_sensor_new_channel(touch, i + TOUCH_MIN_CHAN_ID, &s_chan_cfg, &touch_chan[i]));
    }
    touch_channel_handle_t fault_chan = NULL;
    TEST_ASSERT(touch_sensor_new_channel(touch, TOUCH_MAX_CHAN_ID + 1, &s_chan_cfg, &fault_chan) == ESP_ERR_INVALID_ARG);  // out of range
    TEST_ASSERT(touch_sensor_new_channel(touch, TOUCH_MIN_CHAN_ID, &s_chan_cfg, &fault_chan) == ESP_ERR_INVALID_STATE);  // already allocated

    TEST_ESP_OK(touch_sensor_enable(touch));
    TEST_ASSERT(touch_sensor_del_channel(touch_chan[0]) == ESP_ERR_INVALID_STATE);
    TEST_ESP_OK(touch_sensor_disable(touch));

    TEST_ASSERT(touch_sensor_del_controller(touch) == ESP_ERR_INVALID_STATE);

    for (int i = 0; i < TOUCH_TOTAL_CHAN_NUM; i++) {
        TEST_ESP_OK(touch_sensor_del_channel(touch_chan[i]));
    }
    TEST_ESP_OK(touch_sensor_del_controller(touch));
}

#define TEST_TOUCH_CNT                    (3)

static void s_test_touch_log_data(touch_channel_handle_t touch_chan, const char *tag)
{
    uint32_t data[TOUCH_SAMPLE_CFG_NUM] = {};
    TEST_ESP_OK(touch_channel_read_data(touch_chan, TOUCH_CHAN_DATA_TYPE_SMOOTH, data));
    printf("%s:", tag);
    for (int i = 0; i < TOUCH_SAMPLE_CFG_NUM; i++) {
        printf(" %" PRIu32, data[i]);
    }
    printf("\n");
}

TEST_CASE("touch_sens_active_inactive_test", "[touch]")
{
    test_touch_fixture_t* fixture = (test_touch_fixture_t*)alloca(sizeof(test_touch_fixture_t) + sizeof(touch_channel_handle_t));
    uint32_t chan_ids[1] = {TOUCH_MIN_CHAN_ID};
    test_touch_cb_data_t cb_data = {};
    s_test_touch_fixture_init(fixture, 1, chan_ids, &cb_data);

    touch_sensor_handle_t touch = fixture->sensor;
    touch_channel_handle_t touch_chan = fixture->channels[0];

    TEST_ESP_OK(touch_sensor_enable(touch));
    TEST_ESP_OK(touch_sensor_start_continuous_scanning(touch));
    vTaskDelay(pdMS_TO_TICKS(20));

    for (int i = 0; i < TEST_TOUCH_CNT; i++) {
        printf("\nSimulate Touch [%d] ->\n--------------------------\n", i + 1);
        // Read data before touched
        s_test_touch_log_data(touch_chan, "Data Before");
        // Simulate touch
        test_touch_simulate_touch(chan_ids[0], true);
        vTaskDelay(pdMS_TO_TICKS(100));

        // Read data after touched
        s_test_touch_log_data(touch_chan, "Data After ");
        // Simulate release
        test_touch_simulate_touch(chan_ids[0], false);
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
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(TEST_TOUCH_CNT, cb_data.hw_active_count);
#endif  // SOC_TOUCH_SENSOR_VERSION == 1
    TEST_ASSERT_EQUAL_INT32(TEST_TOUCH_CNT, cb_data.active_count);
    TEST_ASSERT_EQUAL_INT32(TEST_TOUCH_CNT, cb_data.inactive_count);
}

#if SOC_TOUCH_SENSOR_VERSION > 1
#define TEST_SCAN_TIMES          100

TEST_CASE("touch_sens_current_meas_channel_test", "[touch]")
{
    touch_sensor_handle_t touch = NULL;
    touch_channel_handle_t touch_chan = NULL;

    TEST_ESP_OK(touch_sensor_new_controller(&s_sens_cfg, &touch));
    TEST_ESP_OK(touch_sensor_config_filter(touch, &s_filter_cfg));

    /* Loop all channels */
    for (uint32_t ch_id = TOUCH_MIN_CHAN_ID; ch_id <= TOUCH_MAX_CHAN_ID; ch_id++) {
        /* New a channel */
        TEST_ESP_OK(touch_sensor_new_channel(touch, ch_id, &s_chan_cfg, &touch_chan));
        TEST_ESP_OK(touch_sensor_enable(touch));
        /* Trigger one-shot scanning to update the current measuring channel */
        printf("Testing channel %lu\n", ch_id);
        /**
         * @note Some pins are connected to external components (such as pull-up resistors),
         *       so a timeout may occur here.
         */
        touch_sensor_trigger_oneshot_scanning(touch, 100);
        /* Read the current measuring channel for several times */
        for (int i = 0; i < TEST_SCAN_TIMES; i++) {
            /* Check if the current measuring channel is the same as the channel id */
            TEST_ASSERT(touch_ll_get_current_meas_channel() == ch_id);
        }
        TEST_ESP_OK(touch_sensor_disable(touch));
        TEST_ESP_OK(touch_sensor_del_channel(touch_chan));
    }
    TEST_ESP_OK(touch_sensor_del_controller(touch));
}
#endif  // SOC_TOUCH_SENSOR_VERSION > 1

/**
 * @note ESP32-P4 + sleep + PSRAM + O0 optimization can cause SPM overflow.
 *       Therefore, disable the sleep test when iram_safe is enabled.
 */
#if SOC_ULP_SUPPORTED && !(CONFIG_TOUCH_ISR_IRAM_SAFE && SOC_IS(ESP32P4))
extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_main_bin_start");
extern const uint8_t ulp_main_bin_end[] asm("_binary_ulp_main_bin_end");

#define TEST_ULP_WAKEUP_PERIOD_US            (1 * 1000 * 1000)

/**
 * @note The ULP start APIs (ulp_xxx_run) start a timer to trigger ULP execution periodically,
 *       but do not guarantee whether the ULP will run once immediately. Therefore, each ULP
 *       program includes logic to skip the first run, ensuring that the touch event is simulated
 *       only after entering sleep.
 */
static void s_test_touch_start_ulp(void)
{
#if SOC_LP_CORE_SUPPORTED
    TEST_ESP_OK(ulp_lp_core_load_binary(ulp_main_bin_start, ulp_main_bin_end - ulp_main_bin_start));
    ulp_lp_core_cfg_t cfg = {};
    cfg.wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER;
    cfg.lp_timer_sleep_duration_us = TEST_ULP_WAKEUP_PERIOD_US;
    TEST_ESP_OK(ulp_lp_core_run(&cfg));
#elif SOC_RISCV_COPROC_SUPPORTED
    TEST_ESP_OK(ulp_riscv_load_binary(ulp_main_bin_start, ulp_main_bin_end - ulp_main_bin_start));
    TEST_ESP_OK(ulp_set_wakeup_period(0, TEST_ULP_WAKEUP_PERIOD_US));
    TEST_ESP_OK(ulp_riscv_run());
#elif SOC_ULP_FSM_SUPPORTED
    TEST_ESP_OK(ulp_load_binary(0, ulp_main_bin_start, (ulp_main_bin_end - ulp_main_bin_start) / sizeof(uint32_t)));
    TEST_ESP_OK(ulp_set_wakeup_period(0, TEST_ULP_WAKEUP_PERIOD_US));
    TEST_ESP_OK(ulp_run(&ulp_entry - RTC_SLOW_MEM));
#else
#error "No supported ULP"
#endif
}

TEST_CASE("touch_sens_light_sleep_wakeup_test", "[touch]")
{
    test_touch_fixture_t *fixture = (test_touch_fixture_t *)alloca(
                                        sizeof(test_touch_fixture_t) + sizeof(touch_channel_handle_t) * 2);
    const uint32_t chan_ids[] = {TEST_TOUCH_WAKEUP_CHANNEL, TEST_TOUCH_WAKEUP_CHANNEL + 1};
    test_touch_cb_data_t cb_data = {};
    s_test_touch_fixture_init(fixture, 2, chan_ids, &cb_data);
    touch_sensor_handle_t touch = fixture->sensor;

#if SOC_TOUCH_SENSOR_VERSION == 3
    /**
     * @note For TOUCH_SENSOR_VERSION == 3, since we cannot simulate an isolated trigger on a single channel,
     *       the thresholds for all other channels are raised to the maximum to prevent them from being triggered.
     */
    touch_channel_config_t inactive_cfg;
    for (int i = 0; i < TOUCH_SAMPLE_CFG_NUM; i++) {
        inactive_cfg.active_thresh[i] = TOUCH_LL_ACTIVE_THRESH_MAX;
    }
    TEST_ESP_OK(touch_sensor_reconfig_channel(fixture->channels[1], &inactive_cfg));
#endif

    touch_sleep_config_t sleep_cfg = {};
    sleep_cfg.slp_wakeup_lvl = TOUCH_LIGHT_SLEEP_WAKEUP;
    TEST_ESP_OK(touch_sensor_config_sleep_wakeup(touch, &sleep_cfg));
    TEST_ESP_OK(touch_sensor_enable(touch));
    TEST_ESP_OK(touch_sensor_start_continuous_scanning(touch));

    /* Set up the ULP to simulate touch wake-up */
    s_test_touch_start_ulp();

    /* Enter light sleep */
    for (int retry = 0; esp_light_sleep_start() == ESP_ERR_SLEEP_REJECT; retry++) {
        if (retry > 3) {
            TEST_FAIL_MESSAGE("enter light sleep failed");
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    /* Wakeup from sleep */
    uint32_t wakeup_causes = esp_sleep_get_wakeup_causes();

    if (wakeup_causes & BIT(ESP_SLEEP_WAKEUP_TOUCHPAD)) {
        printf("wakeup by touchpad\n");
    } else {
        printf("wakeup by other causes: %" PRIu32 "\n", wakeup_causes);
        TEST_FAIL();
    }

#if SOC_TOUCH_SENSOR_VERSION == 1
    printf("hardware active interrupt count: %d\n", cb_data.hw_active_count);
    TEST_ASSERT_GREATER_OR_EQUAL(1, cb_data.hw_active_count);
#else
    TEST_ASSERT_EQUAL(1, cb_data.active_count);
#endif
    for (int cnt = 0; cb_data.inactive_count != cb_data.active_count; cnt++) {
        if (cnt > 100) {
            TEST_FAIL_MESSAGE("timeout waiting for inactive callback");
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    TEST_ESP_OK(touch_sensor_stop_continuous_scanning(touch));
    TEST_ESP_OK(touch_sensor_disable(touch));
    TEST_ESP_OK(touch_sensor_config_sleep_wakeup(touch, NULL));
    TEST_ESP_OK(touch_sensor_del_channel(fixture->channels[0]));
    TEST_ESP_OK(touch_sensor_del_channel(fixture->channels[1]));
    TEST_ESP_OK(touch_sensor_del_controller(touch));
}

static void s_test_touch_deep_sleep_enter_common(bool allow_pd)
{
    test_touch_fixture_t *fixture = (test_touch_fixture_t *)alloca(
                                        sizeof(test_touch_fixture_t) + sizeof(touch_channel_handle_t));
    const uint32_t chan_ids[] = {TEST_TOUCH_WAKEUP_CHANNEL};
    test_touch_cb_data_t cb_data = {};
    s_test_touch_fixture_init(fixture, 1, chan_ids, &cb_data);
    touch_sensor_handle_t touch = fixture->sensor;
    touch_channel_handle_t touch_chan = fixture->channels[0];

    touch_sleep_config_t sleep_cfg = {};
    sleep_cfg.slp_wakeup_lvl = TOUCH_DEEP_SLEEP_WAKEUP;
#if SOC_TOUCH_SENSOR_VERSION > 1
    if (allow_pd) {
        touch_chan_info_t chan_info = {};
        TEST_ESP_OK(touch_sensor_get_channel_info(touch_chan, &chan_info));
        sleep_cfg.deep_slp_allow_pd = true;
        sleep_cfg.deep_slp_chan = touch_chan;
        for (int i = 0; i < TOUCH_SAMPLE_CFG_NUM; i++) {
            sleep_cfg.deep_slp_thresh[i] = chan_info.active_thresh[i];
            printf("[Sampler %d] deep sleep thresh %" PRIu32 "\n", i, sleep_cfg.deep_slp_thresh[i]);
        }
    }
#else
    (void)allow_pd;
    (void)touch_chan;
#endif
    TEST_ESP_OK(touch_sensor_config_sleep_wakeup(touch, &sleep_cfg));
    TEST_ESP_OK(touch_sensor_enable(touch));
    TEST_ESP_OK(touch_sensor_start_continuous_scanning(touch));

    /* Set up the ULP to simulate touch wake-up after entering sleep */
    s_test_touch_start_ulp();

    esp_deep_sleep_start();
    TEST_FAIL_MESSAGE("Should not reach here after deep sleep");
}

static void s_test_touch_deep_sleep_check(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_DEEPSLEEP, esp_reset_reason());

    uint32_t wakeup_causes = esp_sleep_get_wakeup_causes();
    if (wakeup_causes & BIT(ESP_SLEEP_WAKEUP_TOUCHPAD)) {
        printf("wakeup by touchpad\n");
    } else {
        printf("wakeup by other causes: %" PRIu32 "\n", wakeup_causes);
        TEST_FAIL();
    }
}

static void s_test_touch_deep_sleep_enter(void)
{
    s_test_touch_deep_sleep_enter_common(false);
}

TEST_CASE_MULTIPLE_STAGES("touch_sens_deep_sleep_wakeup_test",
                          "[touch][reset=DEEPSLEEP_RESET]",
                          s_test_touch_deep_sleep_enter,
                          s_test_touch_deep_sleep_check);

#if SOC_TOUCH_SENSOR_VERSION > 1
static void s_test_touch_deep_sleep_pd_enter(void)
{
    s_test_touch_deep_sleep_enter_common(true);
}

TEST_CASE_MULTIPLE_STAGES("touch_sens_deep_sleep_pd_wakeup_test",
                          "[touch][reset=DEEPSLEEP_RESET]",
                          s_test_touch_deep_sleep_pd_enter,
                          s_test_touch_deep_sleep_check);
#endif  // SOC_TOUCH_SENSOR_VERSION > 1
#endif  // SOC_ULP_SUPPORTED && !(CONFIG_TOUCH_ISR_IRAM_SAFE && SOC_IS(ESP32P4))
