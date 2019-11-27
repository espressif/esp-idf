// Copyright 2016-2018 Espressif Systems (Shanghai) PTE LTD
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

#include "esp32s2beta/rom/ets_sys.h"
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

static const char *TOUCH_TAG = "TOUCH_SENSOR";
#define TOUCH_CHECK(a, str, ret_val) ({                                             \
    if (!(a)) {                                                                     \
        ESP_LOGE(TOUCH_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);   \
        return (ret_val);                                                           \
    }                                                                               \
})
#define TOUCH_CHANNEL_CHECK(channel) do { \
        TOUCH_CHECK(channel < SOC_TOUCH_SENSOR_NUM && channel >= 0, "Touch channel error", ESP_ERR_INVALID_ARG); \
        TOUCH_CHECK(channel != SOC_TOUCH_DENOISE_CHANNEL, "TOUCH0 is internal denoise channel", ESP_ERR_INVALID_ARG); \
    } while (0);
#define TOUCH_CH_MASK_CHECK(mask) TOUCH_CHECK((mask <= SOC_TOUCH_SENSOR_BIT_MASK_MAX), "touch channel bitmask error", ESP_ERR_INVALID_ARG)
#define TOUCH_INTR_MASK_CHECK(mask) TOUCH_CHECK(mask & TOUCH_PAD_INTR_MASK_ALL, "intr mask error", ESP_ERR_INVALID_ARG)
#define TOUCH_PARAM_CHECK_STR(s)           ""s" parameter error"

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define TOUCH_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define TOUCH_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)

static SemaphoreHandle_t rtc_touch_mux = NULL;

/*---------------------------------------------------------------
                    Touch Pad
---------------------------------------------------------------*/

esp_err_t touch_pad_isr_register(intr_handler_t fn, void *arg, touch_pad_intr_mask_t intr_mask)
{
    TOUCH_CHECK(fn != NULL, TOUCH_PARAM_CHECK_STR("intr_mask"), ESP_ERR_INVALID_ARG);
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
    return rtc_isr_register(fn, arg, en_msk);
}

esp_err_t touch_pad_set_meas_time(uint16_t sleep_cycle, uint16_t meas_times)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_set_meas_times(meas_times);
    touch_hal_set_sleep_time(sleep_cycle);
    TOUCH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t touch_pad_get_meas_time(uint16_t *sleep_cycle, uint16_t *meas_times)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_get_measure_times(meas_times);
    touch_hal_get_sleep_time(sleep_cycle);
    TOUCH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t touch_pad_set_inactive_connect(touch_pad_conn_type_t type)
{
    TOUCH_CHECK(type < TOUCH_PAD_CONN_MAX, TOUCH_PARAM_CHECK_STR("type"), ESP_ERR_INVALID_ARG);
    TOUCH_ENTER_CRITICAL();
    touch_hal_set_inactive_connect(type);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_get_inactive_connect(touch_pad_conn_type_t *type)
{
    touch_hal_get_inactive_connect(type);
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
    TOUCH_INTR_MASK_CHECK(int_mask);
    TOUCH_ENTER_CRITICAL();
    touch_hal_intr_enable(int_mask);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_intr_disable(touch_pad_intr_mask_t int_mask)
{
    TOUCH_INTR_MASK_CHECK(int_mask);
    TOUCH_ENTER_CRITICAL();
    touch_hal_intr_disable(int_mask);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

uint32_t touch_pad_read_intr_status_mask(void)
{
    return touch_hal_read_intr_status_mask();
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
    TOUCH_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);
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
    *raw_data = touch_hal_read_raw_data(touch_num);
    return ESP_OK;
}

esp_err_t IRAM_ATTR touch_pad_filter_read_baseline(touch_pad_t touch_num, uint32_t *basedata)
{
    TOUCH_CHANNEL_CHECK(touch_num);
    touch_hal_filter_read_baseline(touch_num, basedata);
    return ESP_OK;
}

/* Should be call after clk enable and filter enable. */
esp_err_t touch_pad_filter_reset_baseline(touch_pad_t touch_num)
{
    TOUCH_CHECK(touch_num <= TOUCH_PAD_MAX && touch_num >= 0, "Touch channel error", ESP_ERR_INVALID_ARG);
    TOUCH_ENTER_CRITICAL();
    touch_hal_filter_reset_baseline(touch_num);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_filter_set_config(touch_filter_config_t *filter_info)
{
    TOUCH_CHECK(filter_info->mode < TOUCH_PAD_FILTER_MAX, TOUCH_PARAM_CHECK_STR("mode"), ESP_ERR_INVALID_ARG);
    TOUCH_CHECK(filter_info->debounce_cnt <= TOUCH_DEBOUNCE_CNT_MAX, TOUCH_PARAM_CHECK_STR("debounce"), ESP_ERR_INVALID_ARG);
    TOUCH_CHECK(filter_info->hysteresis_thr <= TOUCH_HYSTERESIS_THR_MAX, TOUCH_PARAM_CHECK_STR("hysteresis"), ESP_ERR_INVALID_ARG);
    TOUCH_CHECK(filter_info->noise_thr <= TOUCH_NOISE_THR_MAX, TOUCH_PARAM_CHECK_STR("noise"), ESP_ERR_INVALID_ARG);
    TOUCH_CHECK(filter_info->noise_neg_thr <= TOUCH_NOISE_NEG_THR_MAX, TOUCH_PARAM_CHECK_STR("noise"), ESP_ERR_INVALID_ARG);
    TOUCH_CHECK(filter_info->neg_noise_limit <= TOUCH_NEG_NOISE_CNT_LIMIT, TOUCH_PARAM_CHECK_STR("noise_limit"), ESP_ERR_INVALID_ARG);
    TOUCH_CHECK(filter_info->jitter_step <= TOUCH_JITTER_STEP_MAX, TOUCH_PARAM_CHECK_STR("jitter_step"), ESP_ERR_INVALID_ARG);

    TOUCH_ENTER_CRITICAL();
    touch_hal_filter_set_config(filter_info);
    TOUCH_EXIT_CRITICAL();
    
    return ESP_OK;
}

esp_err_t touch_pad_filter_get_config(touch_filter_config_t *filter_info)
{
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

esp_err_t touch_pad_denoise_set_config(touch_pad_denoise_t *denoise)
{
    TOUCH_CHECK(denoise->grade < TOUCH_PAD_DENOISE_MAX, TOUCH_PARAM_CHECK_STR("grade"), ESP_ERR_INVALID_ARG);
    TOUCH_CHECK(denoise->cap_level < TOUCH_PAD_DENOISE_CAP_MAX, TOUCH_PARAM_CHECK_STR("cap_level"), ESP_ERR_INVALID_ARG);
    
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

esp_err_t touch_pad_waterproof_set_config(touch_pad_waterproof_t *waterproof)
{
    TOUCH_CHECK(waterproof->guard_ring_pad < SOC_TOUCH_SENSOR_NUM, TOUCH_PARAM_CHECK_STR("pad"), ESP_ERR_INVALID_ARG);
    TOUCH_CHECK(waterproof->shield_driver < TOUCH_PAD_SHIELD_DRV_MAX, TOUCH_PARAM_CHECK_STR("shield_driver"), ESP_ERR_INVALID_ARG);

    TOUCH_ENTER_CRITICAL();
    touch_hal_waterproof_set_config(waterproof);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_waterproof_get_config(touch_pad_waterproof_t *waterproof)
{
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

esp_err_t touch_pad_proximity_set_config(touch_pad_proximity_t *proximity)
{
    for (int i=0; i<SOC_TOUCH_PROXIMITY_CHANNEL_NUM; i++) {
        TOUCH_CHECK(proximity->select_pad[i] < SOC_TOUCH_SENSOR_NUM, TOUCH_PARAM_CHECK_STR("pad"), ESP_ERR_INVALID_ARG);
    }
    TOUCH_CHECK(proximity->meas_num <= TOUCH_PROXIMITY_MEAS_NUM_MAX, TOUCH_PARAM_CHECK_STR("meas_num"), ESP_ERR_INVALID_ARG);

    TOUCH_ENTER_CRITICAL();
    touch_hal_proximity_set_config(proximity);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_proximity_get_config(touch_pad_proximity_t *proximity)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_proximity_get_config(proximity);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_proximity_read_meas_cnt(touch_pad_t touch_num, uint32_t *cnt)
{
    TOUCH_CHECK(touch_hal_proximity_pad_check(touch_num), "touch_num is not proximity", ESP_ERR_INVALID_ARG);
    touch_hal_proximity_read_meas_cnt(touch_num, cnt);
    return ESP_OK;
}

esp_err_t touch_pad_proximity_data_get(touch_pad_t touch_num, uint32_t *measure_out)
{
    TOUCH_CHECK(touch_hal_proximity_pad_check(touch_num), "touch_num is not proximity", ESP_ERR_INVALID_ARG);
    touch_hal_filter_read_baseline(touch_num, measure_out);
    return ESP_OK;
}

/************** sleep pad setting ***********************/

esp_err_t touch_pad_sleep_channel_config(touch_pad_sleep_channel_t *slp_config)
{
    TOUCH_CHECK(slp_config->touch_num < SOC_TOUCH_SENSOR_NUM, TOUCH_PARAM_CHECK_STR("pad"), ESP_ERR_INVALID_ARG);

    TOUCH_ENTER_CRITICAL();
    touch_hal_sleep_channel_config(slp_config);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_sleep_channel_read_baseline(uint32_t *baseline)
{
    touch_hal_sleep_read_baseline(baseline);
    return ESP_OK;
}

esp_err_t touch_pad_sleep_channel_read_debounce(uint32_t *debounce)
{
    touch_hal_sleep_read_debounce(debounce);
    return ESP_OK;
}

esp_err_t touch_pad_sleep_channel_read_proximity_cnt(uint32_t *approach_cnt)
{
    touch_hal_sleep_read_proximity_cnt(approach_cnt);
    return ESP_OK;
}