// Copyright 2016-2019 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_types.h>
#include <stdlib.h>
#include <ctype.h>
#include "esp_log.h"
#include "sys/lock.h"
#include "soc/rtc.h"
#include "soc/periph_defs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_intr_alloc.h"
#include "driver/rtc_io.h"
#include "driver/touch_pad.h"
#include "driver/rtc_cntl.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "esp32/rom/ets_sys.h"

#ifndef NDEBUG
// Enable built-in checks in queue.h in debug builds
#define INVARIANTS
#endif
#include "sys/queue.h"
#include "hal/touch_sensor_types.h"
#include "hal/touch_sensor_hal.h"

typedef struct {
    TimerHandle_t timer;
    uint16_t filtered_val[TOUCH_PAD_MAX];
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

static const char *TOUCH_TAG = "TOUCH_SENSOR";
#define TOUCH_CHECK(a, str, ret_val) ({                                             \
    if (!(a)) {                                                                     \
        ESP_LOGE(TOUCH_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);   \
        return (ret_val);                                                           \
    }                                                                               \
})
#define TOUCH_CHANNEL_CHECK(channel) TOUCH_CHECK(channel < SOC_TOUCH_SENSOR_NUM, "Touch channel error", ESP_ERR_INVALID_ARG)
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
    TOUCH_CHECK(fn, "Touch_Pad ISR null", ESP_ERR_INVALID_ARG);
    return rtc_isr_register(fn, arg, RTC_CNTL_TOUCH_INT_ST_M);
}

esp_err_t touch_pad_isr_register(intr_handler_t fn, void *arg)
{
    TOUCH_CHECK(fn, "Touch_Pad ISR null", ESP_ERR_INVALID_ARG);
    return rtc_isr_register(fn, arg, RTC_CNTL_TOUCH_INT_ST_M);
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
    static uint32_t s_filtered_temp[TOUCH_PAD_MAX] = {0};

    if (s_touch_pad_filter == NULL || rtc_touch_mux == NULL) {
        return;
    }
    uint16_t val = 0;
    touch_fsm_mode_t mode;
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    touch_pad_get_fsm_mode(&mode);
    for (int i = 0; i < TOUCH_PAD_MAX; i++) {
        if ((s_touch_pad_init_bit >> i) & 0x1) {
            _touch_pad_read(i, &val, mode);
            s_touch_pad_filter->raw_val[i] = val;
            s_filtered_temp[i] = s_filtered_temp[i] == 0 ? ((uint32_t)val << TOUCH_PAD_SHIFT_DEFAULT) : s_filtered_temp[i];
            s_filtered_temp[i] = _touch_filter_iir((val << TOUCH_PAD_SHIFT_DEFAULT),
                                                   s_filtered_temp[i], TOUCH_PAD_FILTER_FACTOR_DEFAULT);
            s_touch_pad_filter->filtered_val[i] = (s_filtered_temp[i] + TOUCH_PAD_SHIFT_ROUND_DEFAULT) >> TOUCH_PAD_SHIFT_DEFAULT;
        }
    }
    xTimerReset(s_touch_pad_filter->timer, portMAX_DELAY);
    xSemaphoreGive(rtc_touch_mux);
    if (s_filter_cb != NULL) {
        //return the raw data and filtered data.
        s_filter_cb(s_touch_pad_filter->raw_val, s_touch_pad_filter->filtered_val);
    }
}

esp_err_t touch_pad_set_meas_time(uint16_t sleep_cycle, uint16_t meas_cycle)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_set_meas_time(meas_cycle);
    touch_hal_set_sleep_time(sleep_cycle);
    TOUCH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t touch_pad_get_meas_time(uint16_t *sleep_cycle, uint16_t *meas_cycle)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_get_meas_time(meas_cycle);
    touch_hal_get_sleep_time(sleep_cycle);
    TOUCH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t touch_pad_set_trigger_mode(touch_trigger_mode_t mode)
{
    TOUCH_CHECK((mode < TOUCH_TRIGGER_MAX), TOUCH_PARAM_CHECK_STR("mode"), ESP_ERR_INVALID_ARG);
    TOUCH_ENTER_CRITICAL();
    touch_hal_set_trigger_mode(mode);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_get_trigger_mode(touch_trigger_mode_t *mode)
{
    touch_hal_get_trigger_mode(mode);
    return ESP_OK;
}

esp_err_t touch_pad_set_trigger_source(touch_trigger_src_t src)
{
    TOUCH_CHECK((src < TOUCH_TRIGGER_SOURCE_MAX), TOUCH_PARAM_CHECK_STR("src"), ESP_ERR_INVALID_ARG);
    TOUCH_ENTER_CRITICAL();
    touch_hal_set_trigger_source(src);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_get_trigger_source(touch_trigger_src_t *src)
{
    touch_hal_get_trigger_source(src);
    return ESP_OK;
}

esp_err_t touch_pad_set_group_mask(uint16_t set1_mask, uint16_t set2_mask, uint16_t en_mask)
{
    TOUCH_CHECK((set1_mask <= SOC_TOUCH_SENSOR_BIT_MASK_MAX), "touch set1 bitmask error", ESP_ERR_INVALID_ARG);
    TOUCH_CHECK((set2_mask <= SOC_TOUCH_SENSOR_BIT_MASK_MAX), "touch set2 bitmask error", ESP_ERR_INVALID_ARG);
    TOUCH_CHECK((en_mask <= SOC_TOUCH_SENSOR_BIT_MASK_MAX), "touch work_en bitmask error", ESP_ERR_INVALID_ARG);

    TOUCH_ENTER_CRITICAL();
    touch_hal_set_group_mask(set1_mask, set2_mask);
    touch_hal_set_channel_mask(en_mask);
    TOUCH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t touch_pad_get_group_mask(uint16_t *set1_mask, uint16_t *set2_mask, uint16_t *en_mask)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_get_channel_mask(en_mask);
    touch_hal_get_group_mask(set1_mask, set2_mask);
    TOUCH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t touch_pad_clear_group_mask(uint16_t set1_mask, uint16_t set2_mask, uint16_t en_mask)
{
    TOUCH_CHECK((set1_mask <= SOC_TOUCH_SENSOR_BIT_MASK_MAX), "touch set1 bitmask error", ESP_ERR_INVALID_ARG);
    TOUCH_CHECK((set2_mask <= SOC_TOUCH_SENSOR_BIT_MASK_MAX), "touch set2 bitmask error", ESP_ERR_INVALID_ARG);
    TOUCH_CHECK((en_mask <= SOC_TOUCH_SENSOR_BIT_MASK_MAX), "touch work_en bitmask error", ESP_ERR_INVALID_ARG);

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
    TOUCH_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);
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
        uint32_t rtc_clk = rtc_clk_slow_freq_get_hz();
        touch_pad_set_group_mask((1 << touch_num), (1 << touch_num), (1 << touch_num));
        touch_pad_get_meas_time(&sleep_time, &meas_cycle);
        //If the FSM mode is 'TOUCH_FSM_MODE_TIMER', The data will be ready after one measurement cycle
        //after this function is executed, otherwise, the "touch_value" by "touch_pad_read" is 0.
        wait_time_ms = sleep_time / (rtc_clk / 1000) + meas_cycle / (RTC_FAST_CLK_FREQ_APPROX / 1000);
        wait_tick = wait_time_ms / portTICK_RATE_MS;
        vTaskDelay(wait_tick ? wait_tick : 1);
        s_touch_pad_init_bit |= (1 << touch_num);
    } else {
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t touch_pad_init(void)
{
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
    TOUCH_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);
    if (s_touch_pad_filter != NULL) {
        touch_pad_filter_stop();
        touch_pad_filter_delete();
    }
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    s_touch_pad_init_bit = 0x0000;
    TOUCH_ENTER_CRITICAL();
    touch_hal_deinit();
    TOUCH_EXIT_CRITICAL();
    xSemaphoreGive(rtc_touch_mux);
    vSemaphoreDelete(rtc_touch_mux);
    rtc_touch_mux = NULL;
    return ESP_OK;
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
    TOUCH_CHECK(touch_value != NULL, "touch_value", ESP_ERR_INVALID_ARG);
    TOUCH_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);

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
    TOUCH_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);
    TOUCH_CHANNEL_CHECK(touch_num);
    TOUCH_CHECK(touch_value != NULL, "touch_value", ESP_ERR_INVALID_ARG);
    TOUCH_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_FAIL);
    *touch_value = s_touch_pad_filter->raw_val[touch_num];
    if (*touch_value == 0) {
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

IRAM_ATTR esp_err_t touch_pad_read_filtered(touch_pad_t touch_num, uint16_t *touch_value)
{
    TOUCH_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);
    TOUCH_CHANNEL_CHECK(touch_num);
    TOUCH_CHECK(touch_value != NULL, "touch_value", ESP_ERR_INVALID_ARG);
    TOUCH_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_FAIL);
    *touch_value = (s_touch_pad_filter->filtered_val[touch_num]);
    if (*touch_value == 0) {
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

esp_err_t touch_pad_set_filter_period(uint32_t new_period_ms)
{
    TOUCH_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_ERR_INVALID_STATE);
    TOUCH_CHECK(new_period_ms > 0, "Touch pad filter period error", ESP_ERR_INVALID_ARG);
    TOUCH_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_ERR_INVALID_STATE);

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter != NULL) {
        xTimerChangePeriod(s_touch_pad_filter->timer, new_period_ms / portTICK_PERIOD_MS, portMAX_DELAY);
        s_touch_pad_filter->period = new_period_ms;
    } else {
        ESP_LOGE(TOUCH_TAG, "Touch pad filter deleted");
        ret = ESP_ERR_INVALID_STATE;
    }
    xSemaphoreGive(rtc_touch_mux);
    return ret;
}

esp_err_t touch_pad_get_filter_period(uint32_t *p_period_ms)
{
    TOUCH_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_ERR_INVALID_STATE);
    TOUCH_CHECK(p_period_ms != NULL, "Touch pad period pointer error", ESP_ERR_INVALID_ARG);
    TOUCH_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_ERR_INVALID_STATE);

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter != NULL) {
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
    TOUCH_CHECK(filter_period_ms >= portTICK_PERIOD_MS, "Touch pad filter period error", ESP_ERR_INVALID_ARG);
    TOUCH_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_ERR_INVALID_STATE);

    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter == NULL) {
        s_touch_pad_filter = (touch_pad_filter_t *) calloc(1, sizeof(touch_pad_filter_t));
        if (s_touch_pad_filter == NULL) {
            goto err_no_mem;
        }
    }
    if (s_touch_pad_filter->timer == NULL) {
        s_touch_pad_filter->timer = xTimerCreate("filter_tmr", filter_period_ms / portTICK_PERIOD_MS, pdFALSE,
                                    NULL, touch_pad_filter_cb);
        if (s_touch_pad_filter->timer == NULL) {
            free(s_touch_pad_filter);
            s_touch_pad_filter = NULL;
            goto err_no_mem;
        }
        s_touch_pad_filter->period = filter_period_ms;
    }
    xSemaphoreGive(rtc_touch_mux);
    touch_pad_filter_cb(NULL);
    return ESP_OK;

err_no_mem:
    xSemaphoreGive(rtc_touch_mux);
    return ESP_ERR_NO_MEM;
}

esp_err_t touch_pad_filter_stop(void)
{
    TOUCH_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_ERR_INVALID_STATE);
    TOUCH_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_ERR_INVALID_STATE);
    esp_err_t ret = ESP_OK;
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter != NULL) {
        xTimerStop(s_touch_pad_filter->timer, portMAX_DELAY);
    } else {
        ESP_LOGE(TOUCH_TAG, "Touch pad filter deleted");
        ret = ESP_ERR_INVALID_STATE;
    }
    xSemaphoreGive(rtc_touch_mux);
    return ret;
}

esp_err_t touch_pad_filter_delete(void)
{
    TOUCH_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_ERR_INVALID_STATE);
    TOUCH_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_ERR_INVALID_STATE);
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter != NULL) {
        if (s_touch_pad_filter->timer != NULL) {
            xTimerStop(s_touch_pad_filter->timer, portMAX_DELAY);
            xTimerDelete(s_touch_pad_filter->timer, portMAX_DELAY);
            s_touch_pad_filter->timer = NULL;
        }
        free(s_touch_pad_filter);
        s_touch_pad_filter = NULL;
    }
    xSemaphoreGive(rtc_touch_mux);
    return ESP_OK;
}
