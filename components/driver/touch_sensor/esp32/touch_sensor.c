/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <ctype.h>
#include "sdkconfig.h"
#include "esp_types.h"
#include "esp_log.h"
#include "sys/lock.h"
#include "soc/rtc.h"
#include "soc/periph_defs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_intr_alloc.h"
#include "driver/rtc_io.h"
#include "driver/touch_pad.h"
#include "esp_private/rtc_ctrl.h"
#include "driver/gpio.h"
#include "esp_check.h"

#ifndef NDEBUG
// Enable built-in checks in queue.h in debug builds
#define INVARIANTS
#endif
#include "sys/queue.h"
#include "hal/touch_sensor_types.h"
#include "hal/touch_sensor_hal.h"

typedef struct {
    esp_timer_handle_t timer;
    uint16_t filtered_val[TOUCH_PAD_MAX];
    uint32_t filter_last_val[TOUCH_PAD_MAX];
    uint16_t raw_val[TOUCH_PAD_MAX];
    uint32_t filter_period;
    uint32_t period;
    bool enable;
} touch_pad_filter_t;
static touch_pad_filter_t *s_touch_pad_filter = NULL;
// check if touch pad be initialized.
static uint16_t s_touch_pad_init_bit = 0x0000;
static filter_cb_t s_filter_cb = NULL;
static SemaphoreHandle_t rtc_touch_mux = NULL;

#define TOUCH_PAD_FILTER_FACTOR_DEFAULT   (4)   // IIR filter coefficient.
#define TOUCH_PAD_SHIFT_DEFAULT           (4)   // Increase computing accuracy.
#define TOUCH_PAD_SHIFT_ROUND_DEFAULT     (8)   // ROUND = 2^(n-1); rounding off for fractional.

static __attribute__((unused)) const char *TOUCH_TAG = "TOUCH_SENSOR";

#define TOUCH_CHANNEL_CHECK(channel) ESP_RETURN_ON_FALSE(channel < SOC_TOUCH_SENSOR_NUM, ESP_ERR_INVALID_ARG, TOUCH_TAG,  "Touch channel error");
#define TOUCH_NULL_POINTER_CHECK(p, name) ESP_RETURN_ON_FALSE((p), ESP_ERR_INVALID_ARG, TOUCH_TAG, "input param '"name"' is NULL")
#define TOUCH_PARAM_CHECK_STR(s)     ""s" parameter error"

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define TOUCH_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define TOUCH_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)

/*---------------------------------------------------------------
                    Touch Pad
---------------------------------------------------------------*/
//Some register bits of touch sensor 8 and 9 are mismatched, we need to swap the bits.
#define BITSWAP(data, n, m)   (((data >> n) &  0x1)  == ((data >> m) & 0x1) ? (data) : ((data) ^ ((0x1 <<n) | (0x1 << m))))
#define TOUCH_BITS_SWAP(v)  BITSWAP(v, TOUCH_PAD_NUM8, TOUCH_PAD_NUM9)
static esp_err_t _touch_pad_read(touch_pad_t touch_num, uint16_t *touch_value, touch_fsm_mode_t mode);

esp_err_t touch_pad_isr_handler_register(void (*fn)(void *), void *arg, int no_use, intr_handle_t *handle_no_use)
{
    ESP_RETURN_ON_FALSE(fn, ESP_ERR_INVALID_ARG, TOUCH_TAG,  "Touch_Pad ISR null");
    return rtc_isr_register(fn, arg, RTC_CNTL_TOUCH_INT_ST_M, 0);
}

esp_err_t touch_pad_isr_register(intr_handler_t fn, void *arg)
{
    ESP_RETURN_ON_FALSE(fn, ESP_ERR_INVALID_ARG, TOUCH_TAG,  "Touch_Pad ISR null");
    return rtc_isr_register(fn, arg, RTC_CNTL_TOUCH_INT_ST_M, 0);
}

static uint32_t _touch_filter_iir(uint32_t in_now, uint32_t out_last, uint32_t k)
{
    if (k == 0) {
        return in_now;
    } else {
        uint32_t out_now = (in_now + (k - 1) * out_last) / k;
        return out_now;
    }
}

esp_err_t touch_pad_set_filter_read_cb(filter_cb_t read_cb)
{
    s_filter_cb = read_cb;
    return ESP_OK;
}

static void touch_pad_filter_cb(void *arg)
{
    if (s_touch_pad_filter == NULL) {
        return;
    }
    uint16_t val = 0;
    touch_fsm_mode_t mode;
    touch_pad_get_fsm_mode(&mode);
    for (int i = 0; i < TOUCH_PAD_MAX; i++) {
        if ((s_touch_pad_init_bit >> i) & 0x1) {
            _touch_pad_read(i, &val, mode);
            s_touch_pad_filter->raw_val[i] = val;
            s_touch_pad_filter->filter_last_val[i] = s_touch_pad_filter->filter_last_val[i] == 0 ?
                ((uint32_t)val << TOUCH_PAD_SHIFT_DEFAULT) : s_touch_pad_filter->filter_last_val[i];
            s_touch_pad_filter->filter_last_val[i] = _touch_filter_iir((val << TOUCH_PAD_SHIFT_DEFAULT),
                s_touch_pad_filter->filter_last_val[i], TOUCH_PAD_FILTER_FACTOR_DEFAULT);
            s_touch_pad_filter->filtered_val[i] =
                (s_touch_pad_filter->filter_last_val[i] + TOUCH_PAD_SHIFT_ROUND_DEFAULT) >> TOUCH_PAD_SHIFT_DEFAULT;
        }
    }
    if (s_filter_cb) {
        //return the raw data and filtered data.
        s_filter_cb(s_touch_pad_filter->raw_val, s_touch_pad_filter->filtered_val);
    }
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

esp_err_t touch_pad_set_measurement_clock_cycles(uint16_t clock_cycle)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_set_meas_time(clock_cycle);
    TOUCH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t touch_pad_get_measurement_clock_cycles(uint16_t *clock_cycle)
{
    TOUCH_NULL_POINTER_CHECK(clock_cycle, "clock_cycle");
    TOUCH_ENTER_CRITICAL();
    touch_hal_get_meas_time(clock_cycle);
    TOUCH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t touch_pad_set_meas_time(uint16_t sleep_cycle, uint16_t meas_cycle)
{
    touch_pad_set_measurement_clock_cycles(meas_cycle);
    touch_pad_set_measurement_interval(sleep_cycle);
    return ESP_OK;
}

esp_err_t touch_pad_get_meas_time(uint16_t *sleep_cycle, uint16_t *meas_cycle)
{
    TOUCH_NULL_POINTER_CHECK(sleep_cycle, "sleep_cycle");
    TOUCH_NULL_POINTER_CHECK(meas_cycle, "meas_cycle");
    touch_pad_get_measurement_interval(sleep_cycle);
    touch_pad_get_measurement_clock_cycles(meas_cycle);
    return ESP_OK;
}

esp_err_t touch_pad_set_trigger_mode(touch_trigger_mode_t mode)
{
    ESP_RETURN_ON_FALSE((mode < TOUCH_TRIGGER_MAX), ESP_ERR_INVALID_ARG, TOUCH_TAG,  TOUCH_PARAM_CHECK_STR("mode"));
    TOUCH_ENTER_CRITICAL();
    touch_hal_set_trigger_mode(mode);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_get_trigger_mode(touch_trigger_mode_t *mode)
{
    TOUCH_NULL_POINTER_CHECK(mode, "mode");
    touch_hal_get_trigger_mode(mode);
    return ESP_OK;
}

esp_err_t touch_pad_set_trigger_source(touch_trigger_src_t src)
{
    ESP_RETURN_ON_FALSE((src < TOUCH_TRIGGER_SOURCE_MAX), ESP_ERR_INVALID_ARG, TOUCH_TAG,  TOUCH_PARAM_CHECK_STR("src"));
    TOUCH_ENTER_CRITICAL();
    touch_hal_set_trigger_source(src);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_get_trigger_source(touch_trigger_src_t *src)
{
    TOUCH_NULL_POINTER_CHECK(src, "src");
    touch_hal_get_trigger_source(src);
    return ESP_OK;
}

esp_err_t touch_pad_set_group_mask(uint16_t set1_mask, uint16_t set2_mask, uint16_t en_mask)
{
    ESP_RETURN_ON_FALSE((set1_mask <= TOUCH_PAD_BIT_MASK_ALL), ESP_ERR_INVALID_ARG, TOUCH_TAG,  "touch set1 bitmask error");
    ESP_RETURN_ON_FALSE((set2_mask <= TOUCH_PAD_BIT_MASK_ALL), ESP_ERR_INVALID_ARG, TOUCH_TAG,  "touch set2 bitmask error");
    ESP_RETURN_ON_FALSE((en_mask <= TOUCH_PAD_BIT_MASK_ALL), ESP_ERR_INVALID_ARG, TOUCH_TAG,  "touch work_en bitmask error");

    TOUCH_ENTER_CRITICAL();
    touch_hal_set_group_mask(set1_mask, set2_mask);
    touch_hal_set_channel_mask(en_mask);
    TOUCH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t touch_pad_get_group_mask(uint16_t *set1_mask, uint16_t *set2_mask, uint16_t *en_mask)
{
    TOUCH_NULL_POINTER_CHECK(set1_mask, "set1_mask");
    TOUCH_NULL_POINTER_CHECK(set2_mask, "set2_mask");
    TOUCH_NULL_POINTER_CHECK(en_mask, "en_mask");
    TOUCH_ENTER_CRITICAL();
    touch_hal_get_channel_mask(en_mask);
    touch_hal_get_group_mask(set1_mask, set2_mask);
    TOUCH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t touch_pad_clear_group_mask(uint16_t set1_mask, uint16_t set2_mask, uint16_t en_mask)
{
    ESP_RETURN_ON_FALSE((set1_mask <= TOUCH_PAD_BIT_MASK_ALL), ESP_ERR_INVALID_ARG, TOUCH_TAG,  "touch set1 bitmask error");
    ESP_RETURN_ON_FALSE((set2_mask <= TOUCH_PAD_BIT_MASK_ALL), ESP_ERR_INVALID_ARG, TOUCH_TAG,  "touch set2 bitmask error");
    ESP_RETURN_ON_FALSE((en_mask <= TOUCH_PAD_BIT_MASK_ALL), ESP_ERR_INVALID_ARG, TOUCH_TAG,  "touch work_en bitmask error");

    TOUCH_ENTER_CRITICAL();
    touch_hal_clear_channel_mask(en_mask);
    touch_hal_clear_group_mask(set1_mask, set2_mask);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_intr_enable(void)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_intr_enable();
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_intr_disable(void)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_intr_disable();
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_intr_clear(void)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_intr_clear();
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

bool touch_pad_meas_is_done(void)
{
    return touch_hal_meas_is_done();
}

esp_err_t touch_pad_config(touch_pad_t touch_num, uint16_t threshold)
{
    ESP_RETURN_ON_FALSE(rtc_touch_mux, ESP_FAIL, TOUCH_TAG,  "Touch pad not initialized");
    TOUCH_CHANNEL_CHECK(touch_num);
    touch_fsm_mode_t mode;
    touch_pad_io_init(touch_num);
    TOUCH_ENTER_CRITICAL();
    touch_hal_config(touch_num);
    touch_hal_set_threshold(touch_num, threshold);
    TOUCH_EXIT_CRITICAL();
    touch_pad_get_fsm_mode(&mode);
    if (TOUCH_FSM_MODE_SW == mode) {
        touch_pad_clear_group_mask((1 << touch_num), (1 << touch_num), (1 << touch_num));
        s_touch_pad_init_bit |= (1 << touch_num);
    } else if (TOUCH_FSM_MODE_TIMER == mode) {
        uint16_t sleep_time = 0;
        uint16_t meas_cycle = 0;
        uint32_t wait_time_ms = 0;
        uint32_t wait_tick = 0;
        uint32_t rtc_clk_freq = rtc_clk_slow_freq_get_hz();
        assert(rtc_clk_freq != 0);
        touch_pad_set_group_mask((1 << touch_num), (1 << touch_num), (1 << touch_num));
        touch_pad_get_measurement_interval(&sleep_time);
        touch_pad_get_measurement_clock_cycles(&meas_cycle);

        //If the FSM mode is 'TOUCH_FSM_MODE_TIMER', The data will be ready after one measurement cycle
        //after this function is executed, otherwise, the "touch_value" by "touch_pad_read" is 0.
        wait_time_ms = sleep_time / (rtc_clk_freq / 1000) + meas_cycle / (SOC_CLK_RC_FAST_FREQ_APPROX / 1000);
        wait_tick = wait_time_ms / portTICK_PERIOD_MS;
        vTaskDelay(wait_tick ? wait_tick : 1);
        s_touch_pad_init_bit |= (1 << touch_num);
    } else {
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t touch_pad_init(void)
{
#ifdef CONFIG_RTC_EXT_CRYST_ADDIT_CURRENT_V2
    ESP_LOGE(TOUCH_TAG, "Touch Pad can't work because it provides current to external XTAL");
    return ESP_ERR_NOT_SUPPORTED;
#endif // CONFIG_RTC_EXT_CRYST_ADDIT_CURRENT_V2
    if (rtc_touch_mux == NULL) {
        rtc_touch_mux = xSemaphoreCreateMutex();
    }
    if (rtc_touch_mux == NULL) {
        return ESP_FAIL;
    }
    TOUCH_ENTER_CRITICAL();
    touch_hal_init();
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_deinit(void)
{
    ESP_RETURN_ON_FALSE(rtc_touch_mux, ESP_FAIL, TOUCH_TAG,  "Touch pad not initialized");
    esp_err_t ret = ESP_OK;
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter) {
        if (s_touch_pad_filter->timer) {
            ESP_GOTO_ON_ERROR(esp_timer_stop(s_touch_pad_filter->timer), err, TOUCH_TAG, "failed to stop the timer");
            ESP_GOTO_ON_ERROR(esp_timer_delete(s_touch_pad_filter->timer), err, TOUCH_TAG, "failed to delete the timer");
            s_touch_pad_filter->timer = NULL;
        }
        free(s_touch_pad_filter);
        s_touch_pad_filter = NULL;
    }
    s_touch_pad_init_bit = 0x0000;
    TOUCH_ENTER_CRITICAL();
    touch_hal_deinit();
    TOUCH_EXIT_CRITICAL();
    xSemaphoreGive(rtc_touch_mux);
    vSemaphoreDelete(rtc_touch_mux);
    rtc_touch_mux = NULL;
    return ESP_OK;
err:
    xSemaphoreGive(rtc_touch_mux);
    return ret;
}

static esp_err_t _touch_pad_read(touch_pad_t touch_num, uint16_t *touch_value, touch_fsm_mode_t mode)
{
    esp_err_t res = ESP_OK;
    if (TOUCH_FSM_MODE_SW == mode) {
        touch_pad_set_group_mask((1 << touch_num), (1 << touch_num), (1 << touch_num));
        touch_pad_sw_start();
        while (!touch_hal_meas_is_done()) {};
        *touch_value = touch_hal_read_raw_data(touch_num);
        touch_pad_clear_group_mask((1 << touch_num), (1 << touch_num), (1 << touch_num));
    } else if (TOUCH_FSM_MODE_TIMER == mode) {
        while (!touch_hal_meas_is_done()) {};
        *touch_value = touch_hal_read_raw_data(touch_num);
    } else {
        res = ESP_FAIL;
    }
    if (*touch_value == 0) {
        res = ESP_ERR_INVALID_STATE;
    }
    return res;
}

esp_err_t touch_pad_read(touch_pad_t touch_num, uint16_t *touch_value)
{
    TOUCH_CHANNEL_CHECK(touch_num);
    TOUCH_NULL_POINTER_CHECK(touch_value, "touch_value");
    ESP_RETURN_ON_FALSE(rtc_touch_mux, ESP_FAIL, TOUCH_TAG,  "Touch pad not initialized");

    esp_err_t res = ESP_OK;
    touch_fsm_mode_t mode;
    touch_pad_get_fsm_mode(&mode);
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    res = _touch_pad_read(touch_num, touch_value, mode);
    xSemaphoreGive(rtc_touch_mux);
    return res;
}

IRAM_ATTR esp_err_t touch_pad_read_raw_data(touch_pad_t touch_num, uint16_t *touch_value)
{
    ESP_RETURN_ON_FALSE(rtc_touch_mux, ESP_FAIL, TOUCH_TAG,  "Touch pad not initialized");
    TOUCH_CHANNEL_CHECK(touch_num);
    TOUCH_NULL_POINTER_CHECK(touch_value, "touch_value");
    ESP_RETURN_ON_FALSE(s_touch_pad_filter, ESP_FAIL, TOUCH_TAG,  "Touch pad filter not initialized");
    *touch_value = s_touch_pad_filter->raw_val[touch_num];
    if (*touch_value == 0) {
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

IRAM_ATTR esp_err_t touch_pad_read_filtered(touch_pad_t touch_num, uint16_t *touch_value)
{
    ESP_RETURN_ON_FALSE(rtc_touch_mux, ESP_FAIL, TOUCH_TAG,  "Touch pad not initialized");
    TOUCH_CHANNEL_CHECK(touch_num);
    TOUCH_NULL_POINTER_CHECK(touch_value, "touch_value");
    ESP_RETURN_ON_FALSE(s_touch_pad_filter, ESP_FAIL, TOUCH_TAG,  "Touch pad filter not initialized");
    *touch_value = (s_touch_pad_filter->filtered_val[touch_num]);
    if (*touch_value == 0) {
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

esp_err_t touch_pad_set_filter_period(uint32_t new_period_ms)
{
    ESP_RETURN_ON_FALSE(s_touch_pad_filter, ESP_ERR_INVALID_STATE, TOUCH_TAG,  "Touch pad filter not initialized");
    ESP_RETURN_ON_FALSE(new_period_ms > 0, ESP_ERR_INVALID_ARG, TOUCH_TAG,  "Touch pad filter period error");
    ESP_RETURN_ON_FALSE(rtc_touch_mux, ESP_ERR_INVALID_STATE, TOUCH_TAG,  "Touch pad not initialized");

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    ESP_GOTO_ON_ERROR(esp_timer_stop(s_touch_pad_filter->timer), err, TOUCH_TAG, "failed to stop the timer");
    ESP_GOTO_ON_ERROR(esp_timer_start_periodic(s_touch_pad_filter->timer, new_period_ms * 1000), err, TOUCH_TAG, "failed to start the timer");
    s_touch_pad_filter->period = new_period_ms;
err:
    xSemaphoreGive(rtc_touch_mux);
    return ret;
}

esp_err_t touch_pad_get_filter_period(uint32_t *p_period_ms)
{
    ESP_RETURN_ON_FALSE(s_touch_pad_filter, ESP_ERR_INVALID_STATE, TOUCH_TAG,  "Touch pad filter not initialized");
    TOUCH_NULL_POINTER_CHECK(p_period_ms, "p_period_ms");
    ESP_RETURN_ON_FALSE(rtc_touch_mux, ESP_ERR_INVALID_STATE, TOUCH_TAG,  "Touch pad not initialized");

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter) {
        *p_period_ms = s_touch_pad_filter->period;
    } else {
        ESP_LOGE(TOUCH_TAG, "Touch pad filter deleted");
        ret = ESP_ERR_INVALID_STATE;
    }
    xSemaphoreGive(rtc_touch_mux);
    return ret;
}

esp_err_t touch_pad_filter_start(uint32_t filter_period_ms)
{
    ESP_RETURN_ON_FALSE(filter_period_ms >= portTICK_PERIOD_MS, ESP_ERR_INVALID_ARG, TOUCH_TAG,  "Touch pad filter period error");
    ESP_RETURN_ON_FALSE(rtc_touch_mux, ESP_ERR_INVALID_STATE, TOUCH_TAG,  "Touch pad not initialized");

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter == NULL) {
        s_touch_pad_filter = (touch_pad_filter_t *) calloc(1, sizeof(touch_pad_filter_t));
        ESP_GOTO_ON_FALSE(s_touch_pad_filter, ESP_ERR_NO_MEM, err_no_mem, TOUCH_TAG, "no memory for filter");
    }
    if (s_touch_pad_filter->timer == NULL) {
        esp_timer_create_args_t timer_cfg = {
            .callback = touch_pad_filter_cb,
            .arg = NULL,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "touch filter timer",
            .skip_unhandled_events = true,
        };
        ESP_GOTO_ON_ERROR(esp_timer_create(&timer_cfg, &(s_touch_pad_filter->timer)),
                          err_timer_create, TOUCH_TAG, "failed to create the filter timer");
        s_touch_pad_filter->period = filter_period_ms;
        touch_pad_filter_cb(NULL); // Trigger once immediately to get the initial raw value
        ESP_GOTO_ON_ERROR(esp_timer_start_periodic(s_touch_pad_filter->timer, filter_period_ms * 1000),
                          err_timer_start, TOUCH_TAG, "failed to start the filter timer");
    }

    xSemaphoreGive(rtc_touch_mux);
    return ret;

err_timer_start:
    esp_timer_delete(s_touch_pad_filter->timer);
err_timer_create:
    free(s_touch_pad_filter);
    s_touch_pad_filter = NULL;
err_no_mem:
    xSemaphoreGive(rtc_touch_mux);
    return ret;
}

esp_err_t touch_pad_filter_stop(void)
{
    ESP_RETURN_ON_FALSE(s_touch_pad_filter, ESP_ERR_INVALID_STATE, TOUCH_TAG,  "Touch pad filter not initialized");
    ESP_RETURN_ON_FALSE(rtc_touch_mux, ESP_ERR_INVALID_STATE, TOUCH_TAG,  "Touch pad not initialized");
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    esp_err_t ret = esp_timer_stop(s_touch_pad_filter->timer);
    if (ret != ESP_OK) {
        ESP_LOGE(TOUCH_TAG, "failed to stop the timer");
    }
    xSemaphoreGive(rtc_touch_mux);
    return ret;
}

esp_err_t touch_pad_filter_delete(void)
{
    ESP_RETURN_ON_FALSE(s_touch_pad_filter, ESP_ERR_INVALID_STATE, TOUCH_TAG,  "Touch pad filter not initialized");
    ESP_RETURN_ON_FALSE(rtc_touch_mux, ESP_ERR_INVALID_STATE, TOUCH_TAG,  "Touch pad not initialized");
    esp_err_t ret = ESP_OK;
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter->timer) {
        ESP_GOTO_ON_ERROR(esp_timer_stop(s_touch_pad_filter->timer), err, TOUCH_TAG, "failed to stop the timer");
        ESP_GOTO_ON_ERROR(esp_timer_delete(s_touch_pad_filter->timer), err, TOUCH_TAG, "failed to delete the timer");
        s_touch_pad_filter->timer = NULL;
    }
    free(s_touch_pad_filter);
    s_touch_pad_filter = NULL;
err:
    xSemaphoreGive(rtc_touch_mux);
    return ret;
}
