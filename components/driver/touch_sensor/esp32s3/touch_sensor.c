/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include <stdlib.h>
#include <ctype.h>
#include "esp_log.h"
#include "sys/lock.h"
#include "soc/soc_pins.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_intr_alloc.h"
#include "driver/rtc_io.h"
#include "driver/touch_pad.h"
#include "esp_private/rtc_ctrl.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_check.h"

#include "hal/touch_sensor_types.h"
#include "hal/touch_sensor_hal.h"

#ifndef NDEBUG
// Enable built-in checks in queue.h in debug builds
#define INVARIANTS
#endif
#include "sys/queue.h"

#define TOUCH_PAD_FILTER_FACTOR_DEFAULT   (4)   // IIR filter coefficient.
#define TOUCH_PAD_SHIFT_DEFAULT           (4)   // Increase computing accuracy.
#define TOUCH_PAD_SHIFT_ROUND_DEFAULT     (8)   // ROUND = 2^(n-1); rounding off for fractional.
#define TOUCH_PAD_MEASURE_WAIT_DEFAULT  (0xFF)  // The timer frequency is 8Mhz, the max value is 0xff

static __attribute__((unused)) const char *TOUCH_TAG = "TOUCH_SENSOR";

#define TOUCH_CHANNEL_CHECK(channel) do { \
        ESP_RETURN_ON_FALSE(channel < SOC_TOUCH_SENSOR_NUM && channel >= 0, ESP_ERR_INVALID_ARG, TOUCH_TAG,  "Touch channel error"); \
        ESP_RETURN_ON_FALSE(channel != SOC_TOUCH_DENOISE_CHANNEL, ESP_ERR_INVALID_ARG, TOUCH_TAG,  "TOUCH0 is internal denoise channel"); \
    } while (0);
#define TOUCH_CH_MASK_CHECK(mask) ESP_RETURN_ON_FALSE((mask <= TOUCH_PAD_BIT_MASK_ALL), ESP_ERR_INVALID_ARG, TOUCH_TAG,  "touch channel bitmask error");
#define TOUCH_INTR_MASK_CHECK(mask) ESP_RETURN_ON_FALSE(mask & TOUCH_PAD_INTR_MASK_ALL, ESP_ERR_INVALID_ARG, TOUCH_TAG,  "intr mask error");
#define TOUCH_NULL_POINTER_CHECK(p, name) ESP_RETURN_ON_FALSE((p), ESP_ERR_INVALID_ARG, TOUCH_TAG, "input param '"name"' is NULL")
#define TOUCH_PARAM_CHECK_STR(s)           ""s" parameter error"

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define TOUCH_ENTER_CRITICAL_SAFE()  portENTER_CRITICAL_SAFE(&rtc_spinlock) // Can be called in isr and task.
#define TOUCH_EXIT_CRITICAL_SAFE()  portEXIT_CRITICAL_SAFE(&rtc_spinlock)
#define TOUCH_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define TOUCH_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)

static SemaphoreHandle_t rtc_touch_mux = NULL;

/*---------------------------------------------------------------
                    Touch Pad
---------------------------------------------------------------*/

esp_err_t touch_pad_isr_register(intr_handler_t fn, void *arg, touch_pad_intr_mask_t intr_mask)
{
    ESP_RETURN_ON_FALSE(fn, ESP_ERR_INVALID_ARG, TOUCH_TAG,  TOUCH_PARAM_CHECK_STR("intr_mask"));
    TOUCH_INTR_MASK_CHECK(intr_mask);

    uint32_t en_msk = 0;
    if (intr_mask & TOUCH_PAD_INTR_MASK_DONE) {
        en_msk |= RTC_CNTL_TOUCH_DONE_INT_ST_M;
    }
    if (intr_mask & TOUCH_PAD_INTR_MASK_ACTIVE) {
        en_msk |= RTC_CNTL_TOUCH_ACTIVE_INT_ST_M;
    }
    if (intr_mask & TOUCH_PAD_INTR_MASK_INACTIVE) {
        en_msk |= RTC_CNTL_TOUCH_INACTIVE_INT_ST_M;
    }
    if (intr_mask & TOUCH_PAD_INTR_MASK_SCAN_DONE) {
        en_msk |= RTC_CNTL_TOUCH_SCAN_DONE_INT_ST_M;
    }
    if (intr_mask & TOUCH_PAD_INTR_MASK_TIMEOUT) {
        en_msk |= RTC_CNTL_TOUCH_TIMEOUT_INT_ST_M;
    }
#if SOC_TOUCH_PROXIMITY_MEAS_DONE_SUPPORTED
    if (intr_mask & TOUCH_PAD_INTR_MASK_PROXI_MEAS_DONE) {
        en_msk |= RTC_CNTL_TOUCH_APPROACH_LOOP_DONE_INT_ST_M;
    }
#endif
    esp_err_t ret = rtc_isr_register(fn, arg, en_msk, 0);

    return ret;
}

esp_err_t touch_pad_set_measurement_interval(uint16_t interval_cycle)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_set_sleep_time(interval_cycle);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_get_measurement_interval(uint16_t *interval_cycle)
{
    TOUCH_NULL_POINTER_CHECK(interval_cycle, "interval_cycle");
    TOUCH_ENTER_CRITICAL();
    touch_hal_get_sleep_time(interval_cycle);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_set_charge_discharge_times(uint16_t charge_discharge_times)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_set_meas_times(charge_discharge_times);
    TOUCH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t touch_pad_get_charge_discharge_times(uint16_t *charge_discharge_times)
{
    TOUCH_NULL_POINTER_CHECK(charge_discharge_times, "charge_discharge_times");
    TOUCH_ENTER_CRITICAL();
    touch_hal_get_measure_times(charge_discharge_times);
    TOUCH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t touch_pad_set_meas_time(uint16_t sleep_cycle, uint16_t meas_times)
{
    touch_pad_set_charge_discharge_times(meas_times);
    touch_pad_set_measurement_interval(sleep_cycle);
    return ESP_OK;
}

esp_err_t touch_pad_get_meas_time(uint16_t *sleep_cycle, uint16_t *meas_times)
{
    TOUCH_NULL_POINTER_CHECK(sleep_cycle, "sleep_cycle");
    TOUCH_NULL_POINTER_CHECK(meas_times, "meas_times");
    touch_pad_get_measurement_interval(sleep_cycle);
    touch_pad_get_charge_discharge_times(meas_times);
    return ESP_OK;
}

esp_err_t touch_pad_set_idle_channel_connect(touch_pad_conn_type_t type)
{
    ESP_RETURN_ON_FALSE(type < TOUCH_PAD_CONN_MAX, ESP_ERR_INVALID_ARG, TOUCH_TAG,  TOUCH_PARAM_CHECK_STR("type"));
    TOUCH_ENTER_CRITICAL();
    touch_hal_set_idle_channel_connect(type);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_get_idle_channel_connect(touch_pad_conn_type_t *type)
{
    TOUCH_NULL_POINTER_CHECK(type, "type");
    touch_hal_get_idle_channel_connect(type);
    return ESP_OK;
}

bool touch_pad_meas_is_done(void)
{
    return touch_hal_meas_is_done();
}

esp_err_t touch_pad_set_channel_mask(uint16_t enable_mask)
{
    TOUCH_CH_MASK_CHECK(enable_mask);
    TOUCH_ENTER_CRITICAL();
    touch_hal_set_channel_mask(enable_mask);
    TOUCH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t touch_pad_get_channel_mask(uint16_t *enable_mask)
{
    TOUCH_NULL_POINTER_CHECK(enable_mask, "enable_mask");
    TOUCH_ENTER_CRITICAL();
    touch_hal_get_channel_mask(enable_mask);
    TOUCH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t touch_pad_clear_channel_mask(uint16_t enable_mask)
{
    TOUCH_CH_MASK_CHECK(enable_mask);
    TOUCH_ENTER_CRITICAL();
    touch_hal_clear_channel_mask(enable_mask);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

touch_pad_t IRAM_ATTR touch_pad_get_current_meas_channel(void)
{
    return (touch_pad_t)touch_hal_get_current_meas_channel();
}

esp_err_t touch_pad_intr_enable(touch_pad_intr_mask_t int_mask)
{
    if (!(int_mask & TOUCH_PAD_INTR_MASK_ALL)) {
        return ESP_ERR_INVALID_ARG;
    }
    TOUCH_ENTER_CRITICAL_SAFE();
    touch_hal_intr_enable(int_mask);
    TOUCH_EXIT_CRITICAL_SAFE();
    return ESP_OK;
}

esp_err_t touch_pad_intr_disable(touch_pad_intr_mask_t int_mask)
{
    if (!(int_mask & TOUCH_PAD_INTR_MASK_ALL)) {
        return ESP_ERR_INVALID_ARG;
    }
    TOUCH_ENTER_CRITICAL_SAFE();
    touch_hal_intr_disable(int_mask);
    TOUCH_EXIT_CRITICAL_SAFE();
    return ESP_OK;
}

esp_err_t touch_pad_intr_clear(touch_pad_intr_mask_t int_mask)
{
    TOUCH_INTR_MASK_CHECK(int_mask);
    TOUCH_ENTER_CRITICAL();
    touch_hal_intr_clear(int_mask);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

uint32_t touch_pad_read_intr_status_mask(void)
{
    return touch_hal_read_intr_status_mask();
}

esp_err_t touch_pad_timeout_set(bool enable, uint32_t threshold)
{
    TOUCH_ENTER_CRITICAL();
    if (enable) {
        touch_hal_timeout_enable();
    } else {
        touch_hal_timeout_disable();
    }
    touch_hal_timeout_set_threshold(threshold);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_timeout_get_threshold(uint32_t *threshold)
{
    TOUCH_NULL_POINTER_CHECK(threshold, "threshold");
    TOUCH_ENTER_CRITICAL();
    touch_hal_timeout_get_threshold(threshold);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_timeout_resume(void)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_timer_force_done();
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_config(touch_pad_t touch_num)
{
    TOUCH_CHANNEL_CHECK(touch_num);

    touch_pad_io_init(touch_num);
    TOUCH_ENTER_CRITICAL();
    touch_hal_config(touch_num);
    touch_hal_set_channel_mask(BIT(touch_num));
    TOUCH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t touch_pad_init(void)
{
    if (rtc_touch_mux == NULL) {
        rtc_touch_mux = xSemaphoreCreateMutex();
    }
    if (rtc_touch_mux == NULL) {
        return ESP_ERR_NO_MEM;
    }
    TOUCH_ENTER_CRITICAL();
    touch_hal_init();
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_deinit(void)
{
    ESP_RETURN_ON_FALSE(rtc_touch_mux, ESP_FAIL, TOUCH_TAG,  "Touch pad not initialized");
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    TOUCH_ENTER_CRITICAL();
    touch_hal_deinit();
    TOUCH_EXIT_CRITICAL();
    xSemaphoreGive(rtc_touch_mux);
    vSemaphoreDelete(rtc_touch_mux);
    rtc_touch_mux = NULL;
    return ESP_OK;
}

esp_err_t touch_pad_reset(void)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_reset();
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t IRAM_ATTR touch_pad_read_raw_data(touch_pad_t touch_num, uint32_t *raw_data)
{
    TOUCH_CHANNEL_CHECK(touch_num);
    TOUCH_NULL_POINTER_CHECK(raw_data, "raw_data");
    TOUCH_ENTER_CRITICAL_SAFE();
    *raw_data = touch_hal_read_raw_data(touch_num);
    TOUCH_EXIT_CRITICAL_SAFE();
    return ESP_OK;
}

esp_err_t IRAM_ATTR touch_pad_filter_read_smooth(touch_pad_t touch_num, uint32_t *smooth_data)
{
    TOUCH_CHANNEL_CHECK(touch_num);
    TOUCH_NULL_POINTER_CHECK(smooth_data, "smooth_data");
    TOUCH_ENTER_CRITICAL_SAFE();
    touch_hal_filter_read_smooth(touch_num, smooth_data);
    TOUCH_EXIT_CRITICAL_SAFE();
    return ESP_OK;
}

esp_err_t IRAM_ATTR touch_pad_read_benchmark(touch_pad_t touch_num, uint32_t *benchmark)
{
    TOUCH_CHANNEL_CHECK(touch_num);
    TOUCH_NULL_POINTER_CHECK(benchmark, "benchmark");
    TOUCH_ENTER_CRITICAL_SAFE();
    touch_hal_read_benchmark(touch_num, benchmark);
    TOUCH_EXIT_CRITICAL_SAFE();
    return ESP_OK;
}

/* Should be call after clk enable and filter enable. */
esp_err_t touch_pad_reset_benchmark(touch_pad_t touch_num)
{
    ESP_RETURN_ON_FALSE(touch_num <= TOUCH_PAD_MAX && touch_num >= 0, ESP_ERR_INVALID_ARG, TOUCH_TAG,  "Touch channel error");
    TOUCH_ENTER_CRITICAL();
    touch_hal_reset_benchmark(touch_num);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_filter_set_config(const touch_filter_config_t *filter_info)
{
    TOUCH_NULL_POINTER_CHECK(filter_info, "filter_info");
    ESP_RETURN_ON_FALSE(filter_info->mode < TOUCH_PAD_FILTER_MAX, ESP_ERR_INVALID_ARG, TOUCH_TAG,  TOUCH_PARAM_CHECK_STR("mode"));
    ESP_RETURN_ON_FALSE(filter_info->debounce_cnt <= TOUCH_DEBOUNCE_CNT_MAX, ESP_ERR_INVALID_ARG, TOUCH_TAG,  TOUCH_PARAM_CHECK_STR("debounce"));
    ESP_RETURN_ON_FALSE(filter_info->noise_thr <= TOUCH_NOISE_THR_MAX, ESP_ERR_INVALID_ARG, TOUCH_TAG,  TOUCH_PARAM_CHECK_STR("noise"));
    ESP_RETURN_ON_FALSE(filter_info->jitter_step <= TOUCH_JITTER_STEP_MAX, ESP_ERR_INVALID_ARG, TOUCH_TAG,  TOUCH_PARAM_CHECK_STR("jitter_step"));
    ESP_RETURN_ON_FALSE(filter_info->smh_lvl < TOUCH_PAD_SMOOTH_MAX, ESP_ERR_INVALID_ARG, TOUCH_TAG,  TOUCH_PARAM_CHECK_STR("smooth level"));

    TOUCH_ENTER_CRITICAL();
    touch_hal_filter_set_config(filter_info);
    TOUCH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t touch_pad_filter_get_config(touch_filter_config_t *filter_info)
{
    TOUCH_NULL_POINTER_CHECK(filter_info, "filter_info");
    TOUCH_ENTER_CRITICAL();
    touch_hal_filter_get_config(filter_info);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_filter_enable(void)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_filter_enable();
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_filter_disable(void)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_filter_disable();
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_denoise_enable(void)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_clear_channel_mask(BIT(SOC_TOUCH_DENOISE_CHANNEL));
    touch_hal_denoise_enable();
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_denoise_disable(void)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_denoise_disable();
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_denoise_set_config(const touch_pad_denoise_t *denoise)
{
    TOUCH_NULL_POINTER_CHECK(denoise, "denoise");
    ESP_RETURN_ON_FALSE(denoise->grade < TOUCH_PAD_DENOISE_MAX, ESP_ERR_INVALID_ARG, TOUCH_TAG,  TOUCH_PARAM_CHECK_STR("grade"));
    ESP_RETURN_ON_FALSE(denoise->cap_level < TOUCH_PAD_DENOISE_CAP_MAX, ESP_ERR_INVALID_ARG, TOUCH_TAG,  TOUCH_PARAM_CHECK_STR("cap_level"));

    const touch_hal_meas_mode_t meas = {
        .slope = TOUCH_PAD_SLOPE_DEFAULT,
        .tie_opt = TOUCH_PAD_TIE_OPT_DEFAULT,
    };
    TOUCH_ENTER_CRITICAL();
    touch_hal_set_meas_mode(SOC_TOUCH_DENOISE_CHANNEL, &meas);
    touch_hal_denoise_set_config(denoise);
    TOUCH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t touch_pad_denoise_get_config(touch_pad_denoise_t *denoise)
{
    TOUCH_NULL_POINTER_CHECK(denoise, "denoise");
    TOUCH_ENTER_CRITICAL();
    touch_hal_denoise_get_config(denoise);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_denoise_read_data(uint32_t *data)
{
    touch_hal_denoise_read_data(data);
    return ESP_OK;
}

esp_err_t touch_pad_waterproof_set_config(const touch_pad_waterproof_t *waterproof)
{
    TOUCH_NULL_POINTER_CHECK(waterproof, "waterproof");
    ESP_RETURN_ON_FALSE(waterproof->guard_ring_pad < SOC_TOUCH_SENSOR_NUM, ESP_ERR_INVALID_ARG, TOUCH_TAG,  TOUCH_PARAM_CHECK_STR("pad"));
    ESP_RETURN_ON_FALSE(waterproof->shield_driver < TOUCH_PAD_SHIELD_DRV_MAX, ESP_ERR_INVALID_ARG, TOUCH_TAG,  TOUCH_PARAM_CHECK_STR("shield_driver"));

    TOUCH_ENTER_CRITICAL();
    touch_hal_waterproof_set_config(waterproof);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_waterproof_get_config(touch_pad_waterproof_t *waterproof)
{
    TOUCH_NULL_POINTER_CHECK(waterproof, "waterproof");
    TOUCH_ENTER_CRITICAL();
    touch_hal_waterproof_get_config(waterproof);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_waterproof_enable(void)
{
    touch_pad_io_init(SOC_TOUCH_SHIELD_CHANNEL);
    TOUCH_ENTER_CRITICAL();
    touch_hal_waterproof_enable();
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_waterproof_disable(void)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_waterproof_disable();
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_proximity_enable(touch_pad_t touch_num, bool enabled)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(touch_num < TOUCH_PAD_MAX, ESP_ERR_INVALID_ARG, TOUCH_TAG,  "Touch channel error");

    TOUCH_ENTER_CRITICAL();
    if (!touch_hal_enable_proximity(touch_num, enabled)) {
        ret = ESP_ERR_NOT_SUPPORTED;
    }
    TOUCH_EXIT_CRITICAL();
    return ret;
}

esp_err_t touch_pad_proximity_set_count(touch_pad_t touch_num, uint32_t count)
{
    ESP_RETURN_ON_FALSE(count <= TOUCH_PROXIMITY_MEAS_NUM_MAX, ESP_ERR_INVALID_ARG, TOUCH_TAG,  TOUCH_PARAM_CHECK_STR("measure count"));

    TOUCH_ENTER_CRITICAL();
    touch_hal_proximity_set_meas_times(count);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_proximity_get_count(touch_pad_t touch_num, uint32_t *count)
{
    ESP_RETURN_ON_FALSE(count, ESP_ERR_INVALID_ARG, TOUCH_TAG,  TOUCH_PARAM_CHECK_STR("measure count"));

    TOUCH_ENTER_CRITICAL_SAFE();
    touch_hal_proximity_get_meas_times(count);
    TOUCH_EXIT_CRITICAL_SAFE();
    return ESP_OK;
}

/**
 * @brief Get measure count of proximity channel.
 *        The proximity sensor measurement is the accumulation of touch channel measurements.
 * @param touch_num touch pad index
 * @param cnt Pointer to receive proximity channel measurement count
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG parameter is NULL
 */
esp_err_t touch_pad_proximity_read_meas_cnt(touch_pad_t touch_num, uint32_t *cnt)
{
    TOUCH_NULL_POINTER_CHECK(cnt, "cnt");
    ESP_RETURN_ON_FALSE(touch_hal_proximity_pad_check(touch_num), ESP_ERR_INVALID_ARG, TOUCH_TAG,  "touch num is not proximity");
    TOUCH_ENTER_CRITICAL_SAFE();
    touch_hal_proximity_read_meas_cnt(touch_num, cnt);
    TOUCH_EXIT_CRITICAL_SAFE();
    return ESP_OK;
}

esp_err_t touch_pad_proximity_get_data(touch_pad_t touch_num, uint32_t *measure_out)
{
    TOUCH_NULL_POINTER_CHECK(measure_out, "measure_out");
    ESP_RETURN_ON_FALSE(touch_hal_proximity_pad_check(touch_num), ESP_ERR_INVALID_ARG, TOUCH_TAG,  "touch num is not proximity");
    TOUCH_ENTER_CRITICAL_SAFE();
    touch_hal_read_benchmark(touch_num, measure_out);
    TOUCH_EXIT_CRITICAL_SAFE();
    return ESP_OK;
}

/************** sleep pad setting ***********************/

esp_err_t touch_pad_sleep_channel_get_info(touch_pad_sleep_channel_t *slp_config)
{
    TOUCH_NULL_POINTER_CHECK(slp_config, "slp_config");
    TOUCH_ENTER_CRITICAL_SAFE();
    touch_hal_sleep_channel_get_config(slp_config);
    TOUCH_EXIT_CRITICAL_SAFE();
    return ESP_OK;
}

esp_err_t touch_pad_sleep_channel_enable(touch_pad_t pad_num, bool enable)
{
    TOUCH_CHANNEL_CHECK(pad_num);

    TOUCH_ENTER_CRITICAL();
    touch_hal_sleep_channel_enable(pad_num, enable);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_sleep_channel_enable_proximity(touch_pad_t pad_num, bool enable)
{
    TOUCH_CHANNEL_CHECK(pad_num);

    TOUCH_ENTER_CRITICAL();
    if (enable) {
        touch_hal_sleep_enable_approach();
    } else {
        touch_hal_sleep_disable_approach();
    }
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_sleep_get_channel_num(touch_pad_t *pad_num)
{
    TOUCH_NULL_POINTER_CHECK(pad_num, "pad_num");
    TOUCH_ENTER_CRITICAL();
    touch_hal_sleep_get_channel_num(pad_num);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_sleep_set_threshold(touch_pad_t pad_num, uint32_t touch_thres)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_sleep_set_threshold(touch_thres);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_sleep_get_threshold(touch_pad_t pad_num, uint32_t *touch_thres)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_sleep_get_threshold(touch_thres);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_sleep_channel_read_benchmark(touch_pad_t pad_num, uint32_t *benchmark)
{
    TOUCH_NULL_POINTER_CHECK(benchmark, "benchmark");
    TOUCH_ENTER_CRITICAL_SAFE();
    touch_hal_sleep_read_benchmark(benchmark);
    TOUCH_EXIT_CRITICAL_SAFE();
    return ESP_OK;
}

esp_err_t touch_pad_sleep_channel_read_smooth(touch_pad_t pad_num, uint32_t *smooth_data)
{
    TOUCH_NULL_POINTER_CHECK(smooth_data, "smooth_data");
    TOUCH_ENTER_CRITICAL_SAFE();
    touch_hal_sleep_read_smooth(smooth_data);
    TOUCH_EXIT_CRITICAL_SAFE();
    return ESP_OK;
}

esp_err_t touch_pad_sleep_channel_read_data(touch_pad_t pad_num, uint32_t *raw_data)
{
    TOUCH_NULL_POINTER_CHECK(raw_data, "raw_data");
    TOUCH_ENTER_CRITICAL_SAFE();
    touch_hal_sleep_read_data(raw_data);
    TOUCH_EXIT_CRITICAL_SAFE();
    return ESP_OK;
}

esp_err_t touch_pad_sleep_channel_reset_benchmark(void)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_sleep_reset_benchmark();
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_sleep_channel_read_debounce(touch_pad_t pad_num, uint32_t *debounce)
{
    TOUCH_NULL_POINTER_CHECK(debounce, "debounce");
    touch_hal_sleep_read_debounce(debounce);
    return ESP_OK;
}

esp_err_t touch_pad_sleep_channel_read_proximity_cnt(touch_pad_t pad_num, uint32_t *approach_cnt)
{
    TOUCH_NULL_POINTER_CHECK(approach_cnt, "approach_cnt");
    touch_hal_sleep_read_proximity_cnt(approach_cnt);
    return ESP_OK;
}

esp_err_t touch_pad_sleep_channel_set_work_time(uint16_t sleep_cycle, uint16_t meas_times)
{
    touch_hal_sleep_channel_set_work_time(sleep_cycle, meas_times);
    return ESP_OK;
}
