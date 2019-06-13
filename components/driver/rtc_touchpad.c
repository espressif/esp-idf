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
#include "soc/rtc_periph.h"
#include "soc/sens_periph.h"
#include "soc/rtc_io_reg.h"
#include "soc/rtc_io_struct.h"
#include "soc/sens_reg.h"
#include "soc/sens_struct.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/rtc.h"
#include "soc/periph_defs.h"
#include "rtc_io.h"
#include "touch_pad.h"
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_intr_alloc.h"
#include "sys/lock.h"
#include "driver/rtc_cntl.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/ets_sys.h"
#elif CONFIG_IDF_TARGET_ESP32S2BETA
#include "esp32s2beta/rom/ets_sys.h"
#endif

#ifndef NDEBUG
// Enable built-in checks in queue.h in debug builds
#define INVARIANTS
#endif
#include "sys/queue.h"

#if CONFIG_IDF_TARGET_ESP32S2BETA

#define TOUCH_PAD_FILTER_FACTOR_DEFAULT   (4)   // IIR filter coefficient.
#define TOUCH_PAD_SHIFT_DEFAULT           (4)   // Increase computing accuracy.
#define TOUCH_PAD_SHIFT_ROUND_DEFAULT     (8)   // ROUND = 2^(n-1); rounding off for fractional.
#define DAC_ERR_STR_CHANNEL_ERROR   "DAC channel error"

static portMUX_TYPE rtc_spinlock = portMUX_INITIALIZER_UNLOCKED;
static const char *RTC_MODULE_TAG = "RTC_MODULE";

#define RTC_MODULE_CHECK(a, str, ret_val) if (!(a)) {                                \
    ESP_LOGE(RTC_MODULE_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str); \
    return (ret_val);                                                              \
}

#define RTC_RES_CHECK(res, ret_val) if ( (a) != ESP_OK) {                           \
        ESP_LOGE(RTC_MODULE_TAG,"%s:%d (%s)", __FILE__, __LINE__, __FUNCTION__); \
        return (ret_val);                                                              \
}
static SemaphoreHandle_t rtc_touch_mux = NULL;

// check if touch pad be inited.
static uint16_t s_touch_pad_init_bit = 0x0000;

/*---------------------------------------------------------------
                    Touch Pad
---------------------------------------------------------------*/
esp_err_t touch_pad_isr_register(intr_handler_t fn, void* arg, touch_pad_intr_mask_t intr_mask)
{
    RTC_MODULE_CHECK(intr_mask < TOUCH_PAD_INTR_MASK_MAX, "intr mask err", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(fn, "Touch_Pad ISR null", ESP_ERR_INVALID_ARG);
    return rtc_isr_register(fn, arg, intr_mask << RTC_CNTL_TOUCH_DONE_INT_ENA_S);
}

esp_err_t touch_pad_isr_deregister(intr_handler_t fn, void *arg)
{
    return rtc_isr_deregister(fn, arg);
}

esp_err_t touch_pad_set_meas_time(uint16_t sleep_cycle, uint16_t meas_cycle)
{
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    portENTER_CRITICAL(&rtc_spinlock);
    // touch sensor sleep cycle Time = sleep_cycle / RTC_SLOW_CLK( can be 150k or 32k depending on the options)
    RTCCNTL.touch_ctrl1.touch_sleep_cycles = sleep_cycle;
    //touch sensor measure time= meas_cycle / 8Mhz
    RTCCNTL.touch_ctrl1.touch_meas_num = meas_cycle;
    //the waiting cycles (in 8MHz) between TOUCH_START and TOUCH_XPD
    RTCCNTL.touch_ctrl2.touch_xpd_wait = 255; //wait volt stable
    portEXIT_CRITICAL(&rtc_spinlock);
    xSemaphoreGive(rtc_touch_mux);
    return ESP_OK;
}

esp_err_t touch_pad_get_meas_time(uint16_t *sleep_cycle, uint16_t *meas_cycle)
{
    RTC_MODULE_CHECK(sleep_cycle != NULL, "parameter is NULL", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(meas_cycle != NULL, "parameter is NULL", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    *sleep_cycle = RTCCNTL.touch_ctrl1.touch_sleep_cycles;
    *meas_cycle = RTCCNTL.touch_ctrl1.touch_meas_num;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_set_inactive_connect(touch_pad_conn_type_t type)
{
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.touch_scan_ctrl.touch_inactive_connection = type;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_inactive_connect(touch_pad_conn_type_t *type)
{
    if(type) {
        *type = RTCCNTL.touch_scan_ctrl.touch_inactive_connection;
        return ESP_FAIL;
    } else {
        return ESP_OK;
    }
}

esp_err_t touch_pad_set_voltage(touch_high_volt_t refh, touch_low_volt_t refl, touch_volt_atten_t atten)
{
    portENTER_CRITICAL(&rtc_spinlock);
    if (refh > TOUCH_HVOLT_KEEP) {
        RTCCNTL.touch_ctrl2.touch_drefh = refh;
    }
    if (refl > TOUCH_LVOLT_KEEP) {
        RTCCNTL.touch_ctrl2.touch_drefl = refl;
    }
    if (atten > TOUCH_HVOLT_ATTEN_KEEP) {
        RTCCNTL.touch_ctrl2.touch_drange = atten;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_voltage(touch_high_volt_t *refh, touch_low_volt_t *refl, touch_volt_atten_t *atten)
{
    portENTER_CRITICAL(&rtc_spinlock);
    if (refh) {
        *refh = RTCCNTL.touch_ctrl2.touch_drefh;
    }
    if (refl) {
        *refl = RTCCNTL.touch_ctrl2.touch_drefl;
    }
    if (atten) {
        *atten = RTCCNTL.touch_ctrl2.touch_drange;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_set_cnt_mode(touch_pad_t touch_num, touch_cnt_slope_t slope)
{
    portENTER_CRITICAL(&rtc_spinlock);
    RTCIO.touch_pad[touch_num].tie_opt = TOUCH_PAD_TIE_OPT_LOW;
    RTCIO.touch_pad[touch_num].dac = slope;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_cnt_mode(touch_pad_t touch_num, touch_cnt_slope_t *slope)
{
    portENTER_CRITICAL(&rtc_spinlock);
    if(slope) {
        *slope = RTCIO.touch_pad[touch_num].dac;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_io_init(touch_pad_t touch_num)
{
    RTC_MODULE_CHECK((touch_num < TOUCH_PAD_MAX), "touch IO error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK((touch_num > TOUCH_PAD_NUM0), "touch IO error", ESP_ERR_INVALID_ARG);
    gpio_num_t gpio_num = GPIO_NUM_1;
    gpio_num = touch_num;
    rtc_gpio_init(gpio_num);
    rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED);
    rtc_gpio_pulldown_dis(gpio_num);
    rtc_gpio_pullup_dis(gpio_num);
    return ESP_OK;
}

esp_err_t touch_pad_wait_init_done()
{
    // TODO
    return ESP_FAIL;
}

/*touch_pad_fsm_start_vf, after set mask*/
esp_err_t touch_pad_fsm_start(touch_fsm_mode_t mode)
{
    RTC_MODULE_CHECK((mode < TOUCH_FSM_MODE_MAX), "touch fsm mode error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.touch_ctrl2.touch_start_en = 0; //stop touch fsm
    RTCCNTL.touch_ctrl2.touch_start_force = mode;
    RTCCNTL.touch_ctrl2.touch_slp_timer_en = (mode == TOUCH_FSM_MODE_TIMER ? 1 : 0);
    RTCCNTL.touch_ctrl2.touch_clkgate_en = 1; //enable touch clock for FSM. or force enable.
    SENS.sar_touch_chn_st.touch_channel_clr = TOUCH_PAD_BIT_MASK_MAX;   // clear baseline
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_fsm_stop()
{
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.touch_ctrl2.touch_start_en = 0; //stop touch fsm
    RTCCNTL.touch_ctrl2.touch_slp_timer_en = 0;
    RTCCNTL.touch_ctrl2.touch_clkgate_en = 0; //enable touch clock for FSM. or force enable.
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_fsm_mode(touch_fsm_mode_t *mode)
{
    if (mode) {
        *mode = RTCCNTL.touch_ctrl2.touch_start_force;
    }
    return ESP_OK;
}

/*
 * If doing measure, the flag will be self-clear.
 * After measure, the flag will be set.
 */
uint8_t touch_pad_means_is_done()
{
    return SENS.sar_touch_chn_st.touch_meas_done;
}

/* return the current scan channel. */
esp_err_t touch_pad_sw_start(touch_pad_t *current_scan)
{
    RTC_MODULE_CHECK((RTCCNTL.touch_ctrl2.touch_start_force == TOUCH_FSM_MODE_SW),
            "touch IO error", ESP_FAIL);
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.touch_ctrl2.touch_start_en = 0;
    RTCCNTL.touch_ctrl2.touch_start_en = 1;
    portEXIT_CRITICAL(&rtc_spinlock);
    if(current_scan) {
        *current_scan = SENS.sar_touch_status0.touch_scan_curr;
    }
    return ESP_OK;
}

/* If set "TOUCH_PAD_THRESHOLD_MAX", the filter is not triger.  */
esp_err_t touch_pad_set_thresh(touch_pad_t touch_num, uint32_t threshold)
{
    RTC_MODULE_CHECK((threshold != 0), "threshold error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK((touch_num < TOUCH_PAD_MAX), "touch IO error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK((touch_num > TOUCH_PAD_NUM0), "touch0 no thresh", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    SENS.touch_thresh[touch_num-1].thresh = threshold;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_thresh(touch_pad_t touch_num, uint32_t *threshold)
{
    RTC_MODULE_CHECK((touch_num < TOUCH_PAD_MAX), "touch IO error", ESP_ERR_INVALID_ARG);
    if (threshold) {
        *threshold = SENS.touch_thresh[touch_num-1].thresh;
    }
    return ESP_OK;
}

/* If set mask, the FSM timer should be stop firsty.
 * Noitce: The touchpad that in scan map ,should be deinit digital function firstly.
 * */
esp_err_t touch_pad_set_group_mask(uint16_t enable_mask)
{
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.touch_scan_ctrl.touch_scan_pad_map  |= (enable_mask&TOUCH_PAD_BIT_MASK_MAX);
    SENS.sar_touch_conf.touch_outen |= (enable_mask&TOUCH_PAD_BIT_MASK_MAX);
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_group_mask(uint16_t *enable_mask)
{
    portENTER_CRITICAL(&rtc_spinlock);
    *enable_mask = SENS.sar_touch_conf.touch_outen \
            & RTCCNTL.touch_scan_ctrl.touch_scan_pad_map \
            & TOUCH_PAD_BIT_MASK_MAX;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

/* If clear all mask, the FSM timer should be stop firsty. */
esp_err_t touch_pad_clear_group_mask(uint16_t enable_mask)
{
    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_conf.touch_outen &= ~(enable_mask&TOUCH_PAD_BIT_MASK_MAX);
    RTCCNTL.touch_scan_ctrl.touch_scan_pad_map  &= ~(enable_mask&TOUCH_PAD_BIT_MASK_MAX);
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

uint32_t IRAM_ATTR touch_pad_get_int_status()
{
    return REG_READ(RTC_CNTL_INT_ST_REG) >> (RTC_CNTL_TOUCH_DONE_INT_ST_S) & (TOUCH_PAD_INTR_MASK_ALL);
}

/*
 * The "touch_pad_active" will be change auto.
 * If status bit is 1, this pad is be active, else, this pad is inactive.
 */
uint32_t IRAM_ATTR touch_pad_get_status()
{
    return (SENS.sar_touch_chn_st.touch_pad_active & TOUCH_PAD_BIT_MASK_MAX);
}
// get current scan channel. note: in interrupt and set a litter sleep time.
uint32_t IRAM_ATTR touch_pad_get_scan_curr()
{
    return (SENS.sar_touch_status0.touch_scan_curr);
}

/*
 * Normaly, Should't call this function manual. .
 */
esp_err_t IRAM_ATTR touch_pad_clear_status()
{
    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_conf.touch_status_clr = 1;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_intr_enable(touch_pad_intr_type_t type)
{
    portENTER_CRITICAL(&rtc_spinlock);
    if(type == TOUCH_PAD_INTR_DONE) {
        RTCCNTL.int_ena.rtc_touch_done = 1;
    } else if(type == TOUCH_PAD_INTR_ACTIVE) {
        RTCCNTL.int_ena.rtc_touch_active = 1;
    } else if(type == TOUCH_PAD_INTR_INACTIVE) {
        RTCCNTL.int_ena.rtc_touch_inactive = 1;
    } else if(type == TOUCH_PAD_INTR_ALL){
        RTCCNTL.int_ena.rtc_touch_done = 1;
        RTCCNTL.int_ena.rtc_touch_active = 1;
        RTCCNTL.int_ena.rtc_touch_inactive = 1;
    } else {
        ESP_LOGE(RTC_MODULE_TAG, "no this intr type");
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_intr_disable(touch_pad_intr_type_t type)
{
    portENTER_CRITICAL(&rtc_spinlock);
    if(type == TOUCH_PAD_INTR_DONE) {
        RTCCNTL.int_ena.rtc_touch_done = 0;
    } else if(type == TOUCH_PAD_INTR_ACTIVE) {
        RTCCNTL.int_ena.rtc_touch_active = 0;
    } else if(type == TOUCH_PAD_INTR_INACTIVE) {
        RTCCNTL.int_ena.rtc_touch_inactive = 0;
    } else if(type == TOUCH_PAD_INTR_ALL){
        RTCCNTL.int_ena.rtc_touch_done = 0;
        RTCCNTL.int_ena.rtc_touch_active = 0;
        RTCCNTL.int_ena.rtc_touch_inactive = 0;
    } else {
        ESP_LOGE(RTC_MODULE_TAG, "no this intr type");
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

uint32_t IRAM_ATTR touch_pad_intr_get_mask()
{
    return REG_READ(RTC_CNTL_INT_ENA_REG) >> (RTC_CNTL_TOUCH_DONE_INT_ENA_S) & (TOUCH_PAD_INTR_MASK_ALL);
}

esp_err_t touch_pad_config(touch_pad_t touch_num)
{
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);
    RTC_MODULE_CHECK(touch_num < TOUCH_PAD_MAX, "Touch_Pad Num Err", ESP_ERR_INVALID_ARG);

    touch_pad_io_init(touch_num);
    touch_pad_set_cnt_mode(touch_num, TOUCH_PAD_SLOPE_7);
    touch_pad_set_thresh(touch_num, TOUCH_PAD_THRESHOLD_MAX);
    touch_pad_set_group_mask(BIT(touch_num));
    
    return ESP_OK;
}

esp_err_t touch_pad_init()
{
    if (rtc_touch_mux == NULL) {
        rtc_touch_mux = xSemaphoreCreateMutex();
    }
    if (rtc_touch_mux == NULL) {
        return ESP_FAIL;
    }
    touch_pad_intr_disable(TOUCH_PAD_INTR_ALL);
    touch_pad_clear_group_mask(TOUCH_PAD_BIT_MASK_MAX);
    touch_pad_clear_status();
    touch_pad_set_meas_time(TOUCH_PAD_SLEEP_CYCLE_DEFAULT, TOUCH_PAD_MEASURE_CYCLE_DEFAULT);
    // Set reference voltage for charging/discharging
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_0V5);
    touch_pad_set_inactive_connect(TOUCH_PAD_CONN_GND);
    return ESP_OK;
}

esp_err_t touch_pad_deinit()
{
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    s_touch_pad_init_bit = 0x0000;
    touch_pad_fsm_stop();
    touch_pad_clear_status();
    touch_pad_intr_disable(TOUCH_PAD_INTR_ALL);
    xSemaphoreGive(rtc_touch_mux);
    vSemaphoreDelete(rtc_touch_mux);
    rtc_touch_mux = NULL;
    return ESP_OK;
}

/*raw data 的单位是时间，单位是 1/8MHz */
IRAM_ATTR esp_err_t touch_pad_read_raw(touch_pad_t touch_num, uint32_t *raw_data)
{
    RTC_MODULE_CHECK(touch_num > TOUCH_PAD_NUM0, "Touch pad0 is forbidden", ESP_FAIL);
    if (raw_data) {
        *raw_data = SENS.touch_meas[touch_num].meas_out;
    }
    return ESP_OK;
}


IRAM_ATTR esp_err_t touch_pad_filter_baseline_read(touch_pad_t touch_num, uint32_t *basedata)
{
    RTC_MODULE_CHECK(touch_num > TOUCH_PAD_NUM0, "Touch pad0 is forbidden", ESP_FAIL);
    if (basedata) {
        *basedata = SENS.sar_touch_status[touch_num-1].touch_pad_baseline;
    }
    return ESP_OK;
}

IRAM_ATTR esp_err_t touch_pad_read_debounce(touch_pad_t touch_num, uint32_t *debounce)
{
    RTC_MODULE_CHECK(touch_num > TOUCH_PAD_NUM0, "Touch pad0 is forbidden", ESP_FAIL);
    if (debounce) {
        *debounce = SENS.sar_touch_status[touch_num-1].touch_pad_debounce;
    }
    return ESP_OK;
}

IRAM_ATTR esp_err_t touch_pad_read_thresh(touch_pad_t touch_num, uint32_t *thresh_out)
{
    RTC_MODULE_CHECK(touch_num > TOUCH_PAD_NUM0, "Touch pad0 is forbidden", ESP_FAIL);
    if (thresh_out) {
        *thresh_out = SENS.touch_thresh[touch_num-1].thresh;
    }
    return ESP_OK;
}

/* Should be call after clk enable and filter enable. */
esp_err_t touch_pad_filter_baseline_reset(touch_pad_t touch_num)
{
    RTC_MODULE_CHECK(touch_num <= TOUCH_PAD_MAX, "Touch_Pad Num Err", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    if(touch_num == TOUCH_PAD_MAX) {
        SENS.sar_touch_chn_st.touch_channel_clr = TOUCH_PAD_BIT_MASK_MAX;
    } else {
        SENS.sar_touch_chn_st.touch_channel_clr = BIT(touch_num);
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_filter_set_config(touch_filter_config_t filter_info)
{
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.touch_filter_ctrl.touch_filter_mode = filter_info.mode;
    RTCCNTL.touch_filter_ctrl.touch_debounce = filter_info.debounce_cnt;
    RTCCNTL.touch_filter_ctrl.touch_hysteresis = filter_info.hysteresis_thr;
    RTCCNTL.touch_filter_ctrl.touch_noise_thres = filter_info.noise_thr;
    RTCCNTL.touch_filter_ctrl.touch_neg_noise_thres = filter_info.noise_neg_thr;
    RTCCNTL.touch_filter_ctrl.touch_neg_noise_limit = filter_info.neg_noise_limit;
    RTCCNTL.touch_filter_ctrl.touch_jitter_step = filter_info.jitter_step;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_filter_get_config(touch_filter_config_t *filter_info)
{
    portENTER_CRITICAL(&rtc_spinlock);
    filter_info->mode = RTCCNTL.touch_filter_ctrl.touch_filter_mode;
    filter_info->debounce_cnt = RTCCNTL.touch_filter_ctrl.touch_debounce;
    filter_info->hysteresis_thr = RTCCNTL.touch_filter_ctrl.touch_hysteresis;
    filter_info->noise_thr = RTCCNTL.touch_filter_ctrl.touch_noise_thres;
    filter_info->noise_neg_thr = RTCCNTL.touch_filter_ctrl.touch_neg_noise_thres;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_filter_start()
{
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.touch_filter_ctrl.touch_filter_en = 1;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_filter_stop()
{
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.touch_filter_ctrl.touch_filter_en = 0;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_denoise_enable()
{
    touch_pad_clear_group_mask(BIT(TOUCH_PAD_NUM0));
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.touch_scan_ctrl.touch_denoise_en = 1;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_denoise_disable()
{
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.touch_scan_ctrl.touch_denoise_en = 0;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_denoise_set_config(touch_pad_denoise_t denoise)
{
    touch_pad_set_cnt_mode(TOUCH_PAD_NUM0, TOUCH_PAD_SLOPE_7);
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.touch_ctrl2.touch_refc = denoise.cap_level;
    RTCCNTL.touch_scan_ctrl.touch_denoise_res = denoise.grade;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_denoise_get_config(touch_pad_denoise_t *denoise)
{
    if(denoise) {
        denoise->grade = RTCCNTL.touch_scan_ctrl.touch_denoise_res;
        denoise->cap_level = RTCCNTL.touch_ctrl2.touch_refc;
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

esp_err_t touch_pad_denoise_data_get(uint32_t *data)
{
    *data = SENS.sar_touch_status0.touch_denoise_data;
    return ESP_OK;
}

/*
 * waterproof function include two setting part: 'shield pad driver' and 'guard ring pad num'
 * @note waterproof and touch function are mutually exclusive. if config touch14, dont use shield.
 * @note self-calibration is implemented in hardware.
 * @note touch_out_ring point to touch0, can disable the guatd ring function ?
 * @note "touch_bufdrv" should user config ?
 */
esp_err_t touch_pad_waterproof_set_config(touch_pad_waterproof_t waterproof)
{
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.touch_scan_ctrl.touch_out_ring = waterproof.guard_ring_pad;
    RTCCNTL.touch_scan_ctrl.touch_bufdrv = waterproof.shield_driver;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_waterproof_get_config(touch_pad_waterproof_t *waterproof)
{
    if(waterproof) {
        waterproof->guard_ring_pad = RTCCNTL.touch_scan_ctrl.touch_out_ring;
        waterproof->shield_driver = RTCCNTL.touch_scan_ctrl.touch_bufdrv;
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

esp_err_t touch_pad_waterproof_enable()
{
    touch_pad_clear_group_mask(BIT(TOUCH_PAD_NUM14));
    touch_pad_io_init(TOUCH_PAD_NUM14);
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.touch_scan_ctrl.touch_shield_pad_en = 1;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_waterproof_disable()
{
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.touch_scan_ctrl.touch_shield_pad_en = 0;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

/*
 * @note approach pad can set three pin.
 * @note if clear the approach pad, point this pad to touch0 and then reset this channel baseline.
 * @note the approach thresh is abs value.
 * @note TODO: add channel reset reg.
 */
esp_err_t touch_pad_approach_set_config(touch_pad_approach_t approach)
{
    portENTER_CRITICAL(&rtc_spinlock);
    if(approach.select_pad0) {
        SENS.sar_touch_conf.touch_approach_pad0 = approach.select_pad0;
    }
    if(approach.select_pad1) {
        SENS.sar_touch_conf.touch_approach_pad1 = approach.select_pad1;
    }
    if(approach.select_pad2) {
        SENS.sar_touch_conf.touch_approach_pad2 = approach.select_pad2;
    }
    RTCCNTL.touch_approach.touch_approach_meas_time = approach.means_num;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_approach_get_config(touch_pad_approach_t *approach)
{
    if(approach) {
        approach->select_pad0 = SENS.sar_touch_conf.touch_approach_pad0;
        approach->select_pad1 = SENS.sar_touch_conf.touch_approach_pad1;
        approach->select_pad2 = SENS.sar_touch_conf.touch_approach_pad2;
        approach->means_num = RTCCNTL.touch_approach.touch_approach_meas_time;
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

uint32_t touch_pad_approach_get_cnt(uint8_t pad)
{
    uint32_t cnt = 0;
    if(pad == 0){
        cnt = SENS.sar_touch_status16.touch_approach_pad0_cnt;
    } else if(pad == 1) {
        cnt = SENS.sar_touch_status16.touch_approach_pad1_cnt;
    } else if(pad == 2) {
        cnt = SENS.sar_touch_status16.touch_approach_pad2_cnt;
    } else if(pad == 3) {
        cnt = SENS.sar_touch_status16.touch_slp_approach_cnt;
    }
    return cnt;
}

/* TODO */
esp_err_t touch_pad_approach_disable()
{
    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_conf.touch_approach_pad0 = 0;
    SENS.sar_touch_conf.touch_approach_pad1 = 0;
    SENS.sar_touch_conf.touch_approach_pad2 = 0;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

/* After touch_clkgate_en = 0, reset the whole of touch module. */
esp_err_t touch_pad_reset()
{
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.touch_ctrl2.touch_reset = 0;
    RTCCNTL.touch_ctrl2.touch_reset = 1;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

/************** sleep pad setting ***********************/

esp_err_t touch_pad_sleep_pad_config(touch_pad_t pad, uint32_t sleep_thr, uint8_t is_approach)
{
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.touch_slp_thres.touch_slp_pad = pad;
    RTCCNTL.touch_slp_thres.touch_slp_th = sleep_thr;
    RTCCNTL.touch_slp_thres.touch_slp_approach_en = is_approach?1:0;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

/**
 * Get sleep touch pad baseline data.
 *
 * @param baseline 
 */
void touch_sleep_baseline_get(uint32_t *baseline)
{
    *baseline = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS15_REG, SENS_TOUCH_SLP_BASELINE);
}

/**
 * Get sleep touch pad debounce data.
 *
 * @param debounce 
 */
void touch_sleep_debounce_get(uint32_t *debounce)
{
    *debounce = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS15_REG, SENS_TOUCH_SLP_DEBOUNCE);
}

/**
 * Get sleep touch pad approach cnt data.
 *
 * @param approach_cnt 
 */
void touch_sleep_approach_cnt_get(uint32_t *approach_cnt)
{
    *approach_cnt = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS16_REG, SENS_TOUCH_SLP_APPROACH_CNT);
}

esp_err_t touch_pad_get_wakeup_status(touch_pad_t *pad_num)
{
    uint32_t touch_mask = SENS.sar_touch_chn_st.touch_pad_active;
    *pad_num = __builtin_ffs(touch_mask) - 1;
    return ESP_OK;
}

#endif