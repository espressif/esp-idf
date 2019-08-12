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

#include "esp32s2beta/rom/ets_sys.h"

#ifndef NDEBUG
// Enable built-in checks in queue.h in debug builds
#define INVARIANTS
#endif
#include "sys/queue.h"

#define TOUCH_PAD_FILTER_FACTOR_DEFAULT   (4)   // IIR filter coefficient.
#define TOUCH_PAD_SHIFT_DEFAULT           (4)   // Increase computing accuracy.
#define TOUCH_PAD_SHIFT_ROUND_DEFAULT     (8)   // ROUND = 2^(n-1); rounding off for fractional.
#define TOUCH_PAD_MEASURE_WAIT_DEFAULT  (0xFF)  // The timer frequency is 8Mhz, the max value is 0xff
#define DAC_ERR_STR_CHANNEL_ERROR   "DAC channel error"

#define RTC_MODULE_CHECK(a, str, ret_val) ({                                                \
    if (!(a)) {                                                                             \
        ESP_LOGE(RTC_MODULE_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);    \
        return (ret_val);                                                                   \
    }                                                                                       \
})

#define RTC_RES_CHECK(res, ret_val) ({                                          \
    if ( (res) != ESP_OK) {                                                     \
        ESP_LOGE(RTC_MODULE_TAG,"%s:%d (%s)", __FILE__, __LINE__, __FUNCTION__);\
        return (ret_val);                                                       \
    }                                                                           \
})

static portMUX_TYPE rtc_spinlock = portMUX_INITIALIZER_UNLOCKED;
#define RTC_TOUCH_ENTER_CRITICAL() portENTER_CRITICAL(&rtc_spinlock)
#define RTC_TOUCH_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)

static SemaphoreHandle_t rtc_touch_mux = NULL;
static const char *RTC_MODULE_TAG = "RTC_MODULE";

/*---------------------------------------------------------------
                    Touch Pad
---------------------------------------------------------------*/
esp_err_t touch_pad_isr_register(intr_handler_t fn, void *arg, touch_pad_intr_mask_t intr_mask)
{
    assert(fn != NULL);
    return rtc_isr_register(fn, arg, TOUCH_PAD_INTR_MASK_ALL & (intr_mask << RTC_CNTL_TOUCH_DONE_INT_ENA_S));
}

esp_err_t touch_pad_isr_deregister(intr_handler_t fn, void *arg)
{
    return rtc_isr_deregister(fn, arg);
}

esp_err_t touch_pad_set_meas_time(uint16_t sleep_cycle, uint16_t meas_times)
{
    RTC_TOUCH_ENTER_CRITICAL();
    // touch sensor sleep cycle Time = sleep_cycle / RTC_SLOW_CLK( can be 150k or 32k depending on the options)
    RTCCNTL.touch_ctrl1.touch_sleep_cycles = sleep_cycle;
    //The times of charge and discharge in each measure process of touch channels.
    RTCCNTL.touch_ctrl1.touch_meas_num = meas_times;
    //the waiting cycles (in 8MHz) between TOUCH_START and TOUCH_XPD
    RTCCNTL.touch_ctrl2.touch_xpd_wait = TOUCH_PAD_MEASURE_WAIT_DEFAULT; //wait volt stable
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_get_meas_time(uint16_t *sleep_cycle, uint16_t *meas_times)
{
    if (sleep_cycle) {
        *sleep_cycle = RTCCNTL.touch_ctrl1.touch_sleep_cycles;
    }
    if (meas_times) {
        *meas_times = RTCCNTL.touch_ctrl1.touch_meas_num;
    }
    return ESP_OK;
}

esp_err_t touch_pad_set_inactive_connect(touch_pad_conn_type_t type)
{
    RTC_TOUCH_ENTER_CRITICAL();
    RTCCNTL.touch_scan_ctrl.touch_inactive_connection = type;
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_get_inactive_connect(touch_pad_conn_type_t *type)
{
    RTC_MODULE_CHECK(type != NULL, "parameter is NULL", ESP_ERR_INVALID_ARG);
    *type = RTCCNTL.touch_scan_ctrl.touch_inactive_connection;
    return ESP_OK;
}

esp_err_t touch_pad_set_voltage(touch_high_volt_t refh, touch_low_volt_t refl, touch_volt_atten_t atten)
{
    RTC_TOUCH_ENTER_CRITICAL();
    if (refh > TOUCH_HVOLT_KEEP) {
        RTCCNTL.touch_ctrl2.touch_drefh = refh;
    }
    if (refl > TOUCH_LVOLT_KEEP) {
        RTCCNTL.touch_ctrl2.touch_drefl = refl;
    }
    if (atten > TOUCH_HVOLT_ATTEN_KEEP) {
        RTCCNTL.touch_ctrl2.touch_drange = atten;
    }
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_get_voltage(touch_high_volt_t *refh, touch_low_volt_t *refl, touch_volt_atten_t *atten)
{
    if (refh) {
        *refh = RTCCNTL.touch_ctrl2.touch_drefh;
    }
    if (refl) {
        *refl = RTCCNTL.touch_ctrl2.touch_drefl;
    }
    if (atten) {
        *atten = RTCCNTL.touch_ctrl2.touch_drange;
    }
    return ESP_OK;
}

esp_err_t touch_pad_set_cnt_mode(touch_pad_t touch_num, touch_cnt_slope_t slope, touch_tie_opt_t opt)
{
    RTC_TOUCH_ENTER_CRITICAL();
    RTCIO.touch_pad[touch_num].tie_opt = opt;
    RTCIO.touch_pad[touch_num].dac = slope;
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_get_cnt_mode(touch_pad_t touch_num, touch_cnt_slope_t *slope, touch_tie_opt_t *opt)
{
    if (slope) {
        *slope = RTCIO.touch_pad[touch_num].dac;
    }
    if (opt) {
        *opt = RTCIO.touch_pad[touch_num].tie_opt;
    }
    return ESP_OK;
}

esp_err_t touch_pad_io_init(touch_pad_t touch_num)
{
    RTC_MODULE_CHECK(touch_num != TOUCH_DENOISE_CHANNEL,
                    "please use `touch_pad_denoise_enable` to set denoise channel", ESP_ERR_INVALID_ARG);
    rtc_gpio_init(touch_num);
    rtc_gpio_set_direction(touch_num, RTC_GPIO_MODE_DISABLED);
    rtc_gpio_pulldown_dis(touch_num);
    rtc_gpio_pullup_dis(touch_num);
    return ESP_OK;
}

esp_err_t touch_pad_wait_init_done(void)
{
    // TODO
    return ESP_FAIL;
}

esp_err_t touch_pad_fsm_start(void)
{
    RTC_TOUCH_ENTER_CRITICAL();
    RTCCNTL.touch_ctrl2.touch_clkgate_en = 1; //enable touch clock for FSM. or force enable.
    SENS.sar_touch_chn_st.touch_channel_clr = TOUCH_PAD_BIT_MASK_MAX;   // clear SENS_TOUCH_SLP_BASELINE
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_fsm_stop(void)
{
    RTC_TOUCH_ENTER_CRITICAL();
    RTCCNTL.touch_ctrl2.touch_start_en = 0; //stop touch fsm
    RTCCNTL.touch_ctrl2.touch_slp_timer_en = 0;
    RTCCNTL.touch_ctrl2.touch_clkgate_en = 0; //enable touch clock for FSM. or force enable.
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_set_fsm_mode(touch_fsm_mode_t mode)
{
    RTC_TOUCH_ENTER_CRITICAL();
    RTCCNTL.touch_ctrl2.touch_start_en = 0; //stop touch fsm
    RTCCNTL.touch_ctrl2.touch_start_force = mode;
    RTCCNTL.touch_ctrl2.touch_slp_timer_en = (mode == TOUCH_FSM_MODE_TIMER ? 1 : 0);
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_get_fsm_mode(touch_fsm_mode_t *mode)
{
    assert(mode != NULL);
    *mode = RTCCNTL.touch_ctrl2.touch_start_force;
    return ESP_OK;
}

bool touch_pad_meas_is_done(void)
{
    return SENS.sar_touch_chn_st.touch_meas_done;
}

esp_err_t touch_pad_sw_start(void)
{
    RTC_MODULE_CHECK((RTCCNTL.touch_ctrl2.touch_start_force == TOUCH_FSM_MODE_SW),
                     "touch fsm mode error", ESP_ERR_INVALID_STATE);
    RTC_TOUCH_ENTER_CRITICAL();
    RTCCNTL.touch_ctrl2.touch_start_en = 0;
    RTCCNTL.touch_ctrl2.touch_start_en = 1;
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_set_thresh(touch_pad_t touch_num, uint32_t threshold)
{
    RTC_MODULE_CHECK((touch_num < TOUCH_PAD_MAX) && (touch_num != TOUCH_DENOISE_CHANNEL), "touch num error", ESP_ERR_INVALID_ARG);
    RTC_TOUCH_ENTER_CRITICAL();
    SENS.touch_thresh[touch_num - 1].thresh = threshold;
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_get_thresh(touch_pad_t touch_num, uint32_t *threshold)
{
    if (threshold) {
        *threshold = SENS.touch_thresh[touch_num - 1].thresh;
    }
    return ESP_OK;
}

esp_err_t touch_pad_set_group_mask(uint16_t enable_mask)
{
    RTC_TOUCH_ENTER_CRITICAL();
    RTCCNTL.touch_scan_ctrl.touch_scan_pad_map  |= (enable_mask & TOUCH_PAD_BIT_MASK_MAX);
    SENS.sar_touch_conf.touch_outen |= (enable_mask & TOUCH_PAD_BIT_MASK_MAX);
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_get_group_mask(uint16_t *enable_mask)
{
    RTC_TOUCH_ENTER_CRITICAL();
    *enable_mask = SENS.sar_touch_conf.touch_outen \
                   & RTCCNTL.touch_scan_ctrl.touch_scan_pad_map \
                   & TOUCH_PAD_BIT_MASK_MAX;
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_clear_group_mask(uint16_t enable_mask)
{
    RTC_TOUCH_ENTER_CRITICAL();
    SENS.sar_touch_conf.touch_outen &= ~(enable_mask & TOUCH_PAD_BIT_MASK_MAX);
    RTCCNTL.touch_scan_ctrl.touch_scan_pad_map  &= ~(enable_mask & TOUCH_PAD_BIT_MASK_MAX);
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

uint32_t IRAM_ATTR touch_pad_get_status(void)
{
    return (SENS.sar_touch_chn_st.touch_pad_active & TOUCH_PAD_BIT_MASK_MAX);
}

static esp_err_t touch_pad_clear_status(void)
{
    SENS.sar_touch_conf.touch_status_clr = 1;
    return ESP_OK;
}

touch_pad_t IRAM_ATTR touch_pad_get_scan_curr(void)
{
    return (touch_pad_t)(SENS.sar_touch_status0.touch_scan_curr);
}

esp_err_t touch_pad_intr_enable(touch_pad_intr_mask_t int_mask)
{
    RTC_TOUCH_ENTER_CRITICAL();
    if (int_mask & TOUCH_PAD_INTR_MASK_DONE) {
        RTCCNTL.int_ena.rtc_touch_done = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_ACTIVE) {
        RTCCNTL.int_ena.rtc_touch_active = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_INACTIVE) {
        RTCCNTL.int_ena.rtc_touch_inactive = 1;
    }
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_intr_disable(touch_pad_intr_mask_t int_mask)
{
    RTC_TOUCH_ENTER_CRITICAL();
    if (int_mask & TOUCH_PAD_INTR_MASK_DONE) {
        RTCCNTL.int_ena.rtc_touch_done = 0;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_ACTIVE) {
        RTCCNTL.int_ena.rtc_touch_active = 0;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_INACTIVE) {
        RTCCNTL.int_ena.rtc_touch_inactive = 0;
    }
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

uint32_t touch_pad_intr_status_get_mask(void)
{
    return ((REG_READ(RTC_CNTL_INT_ST_REG) >> (RTC_CNTL_TOUCH_DONE_INT_ST_S)) & TOUCH_PAD_INTR_MASK_ALL);
}

esp_err_t touch_pad_config(touch_pad_t touch_num)
{
    RTC_MODULE_CHECK(touch_num != TOUCH_DENOISE_CHANNEL, \
                    "please use `touch_pad_denoise_enable` to set denoise channel", ESP_ERR_INVALID_ARG);
    touch_pad_io_init(touch_num);
    touch_pad_set_cnt_mode(touch_num, TOUCH_PAD_SLOPE_7, TOUCH_PAD_TIE_OPT_LOW);
    touch_pad_set_thresh(touch_num, TOUCH_PAD_THRESHOLD_MAX);
    touch_pad_set_group_mask(BIT(touch_num));
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
    touch_pad_intr_disable(TOUCH_PAD_INTR_ALL);
    touch_pad_clear_group_mask(TOUCH_PAD_BIT_MASK_MAX);
    touch_pad_clear_status();
    touch_pad_set_meas_time(TOUCH_PAD_SLEEP_CYCLE_DEFAULT, TOUCH_PAD_MEASURE_CYCLE_DEFAULT);
    // Set reference voltage for charging/discharging
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_0V5);
    touch_pad_set_inactive_connect(TOUCH_PAD_CONN_GND);
    return ESP_OK;
}

esp_err_t touch_pad_deinit(void)
{
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    touch_pad_fsm_stop();
    touch_pad_clear_status();
    touch_pad_intr_disable(TOUCH_PAD_INTR_ALL);
    xSemaphoreGive(rtc_touch_mux);
    vSemaphoreDelete(rtc_touch_mux);
    rtc_touch_mux = NULL;
    return ESP_OK;
}

esp_err_t IRAM_ATTR touch_pad_read_raw_data(touch_pad_t touch_num, uint32_t *raw_data)
{
    if (raw_data) {
        *raw_data = SENS.touch_meas[touch_num].meas_out;
    }
    return ESP_OK;
}


esp_err_t IRAM_ATTR touch_pad_filter_baseline_read(touch_pad_t touch_num, uint32_t *basedata)
{
    RTC_MODULE_CHECK(touch_num != TOUCH_DENOISE_CHANNEL, "denoise channel don't support", ESP_ERR_INVALID_ARG);
    if (basedata) {
        *basedata = SENS.sar_touch_status[touch_num - 1].touch_pad_baseline;
    }
    return ESP_OK;
}

esp_err_t touch_pad_filter_debounce_read(touch_pad_t touch_num, uint32_t *debounce)
{
    RTC_MODULE_CHECK(touch_num != TOUCH_DENOISE_CHANNEL, "denoise channel don't support", ESP_ERR_INVALID_ARG);
    if (debounce) {
        *debounce = SENS.sar_touch_status[touch_num - 1].touch_pad_debounce;
    }
    return ESP_OK;
}

/* Should be call after clk enable and filter enable. */
esp_err_t touch_pad_filter_baseline_reset(touch_pad_t touch_num)
{
    RTC_TOUCH_ENTER_CRITICAL();
    if (touch_num == TOUCH_PAD_MAX) {
        SENS.sar_touch_chn_st.touch_channel_clr = TOUCH_PAD_BIT_MASK_MAX;
    } else {
        SENS.sar_touch_chn_st.touch_channel_clr = BIT(touch_num);
    }
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_filter_set_config(touch_filter_config_t *filter_info)
{
    RTC_TOUCH_ENTER_CRITICAL();
    RTCCNTL.touch_filter_ctrl.touch_filter_mode = filter_info->mode;
    RTCCNTL.touch_filter_ctrl.touch_debounce = filter_info->debounce_cnt;
    RTCCNTL.touch_filter_ctrl.touch_hysteresis = filter_info->hysteresis_thr;
    RTCCNTL.touch_filter_ctrl.touch_noise_thres = filter_info->noise_thr;
    RTCCNTL.touch_filter_ctrl.touch_neg_noise_thres = filter_info->noise_neg_thr;
    RTCCNTL.touch_filter_ctrl.touch_neg_noise_limit = filter_info->neg_noise_limit;
    RTCCNTL.touch_filter_ctrl.touch_jitter_step = filter_info->jitter_step;
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_filter_get_config(touch_filter_config_t *filter_info)
{
    RTC_TOUCH_ENTER_CRITICAL();
    filter_info->mode = RTCCNTL.touch_filter_ctrl.touch_filter_mode;
    filter_info->debounce_cnt = RTCCNTL.touch_filter_ctrl.touch_debounce;
    filter_info->hysteresis_thr = RTCCNTL.touch_filter_ctrl.touch_hysteresis;
    filter_info->noise_thr = RTCCNTL.touch_filter_ctrl.touch_noise_thres;
    filter_info->noise_neg_thr = RTCCNTL.touch_filter_ctrl.touch_neg_noise_thres;
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_filter_enable(void)
{
    RTC_TOUCH_ENTER_CRITICAL();
    RTCCNTL.touch_filter_ctrl.touch_filter_en = 1;
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_filter_disable(void)
{
    RTC_TOUCH_ENTER_CRITICAL();
    RTCCNTL.touch_filter_ctrl.touch_filter_en = 0;
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_denoise_enable(void)
{
    RTC_TOUCH_ENTER_CRITICAL();
    RTCCNTL.touch_scan_ctrl.touch_scan_pad_map  &= ~(BIT(TOUCH_DENOISE_CHANNEL));
    RTCCNTL.touch_scan_ctrl.touch_denoise_en = 1;
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_denoise_disable(void)
{
    RTC_TOUCH_ENTER_CRITICAL();
    RTCCNTL.touch_scan_ctrl.touch_denoise_en = 0;
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_denoise_set_config(touch_pad_denoise_t denoise)
{
    RTC_TOUCH_ENTER_CRITICAL();
    RTCIO.touch_pad[TOUCH_DENOISE_CHANNEL].tie_opt = TOUCH_PAD_TIE_OPT_LOW;
    RTCIO.touch_pad[TOUCH_DENOISE_CHANNEL].dac = TOUCH_PAD_SLOPE_7;
    RTCCNTL.touch_ctrl2.touch_refc = denoise.cap_level;
    RTCCNTL.touch_scan_ctrl.touch_denoise_res = denoise.grade;
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_denoise_get_config(touch_pad_denoise_t *denoise)
{
    RTC_TOUCH_ENTER_CRITICAL();
    denoise->grade = RTCCNTL.touch_scan_ctrl.touch_denoise_res;
    denoise->cap_level = RTCCNTL.touch_ctrl2.touch_refc;
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_denoise_data_get(uint32_t *data)
{
    if (data) {
        *data = SENS.sar_touch_status0.touch_denoise_data;
    }
    return ESP_OK;
}

esp_err_t touch_pad_waterproof_set_config(touch_pad_waterproof_t waterproof)
{
    RTC_TOUCH_ENTER_CRITICAL();
    RTCCNTL.touch_scan_ctrl.touch_out_ring = waterproof.guard_ring_pad;
    RTCCNTL.touch_scan_ctrl.touch_bufdrv = waterproof.shield_driver;
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_waterproof_get_config(touch_pad_waterproof_t *waterproof)
{
    if (waterproof) {
        RTC_TOUCH_ENTER_CRITICAL();
        waterproof->guard_ring_pad = RTCCNTL.touch_scan_ctrl.touch_out_ring;
        waterproof->shield_driver = RTCCNTL.touch_scan_ctrl.touch_bufdrv;
        RTC_TOUCH_EXIT_CRITICAL();
    }
    return ESP_OK;
}

esp_err_t touch_pad_waterproof_enable(void)
{
    touch_pad_io_init(TOUCH_SHIELD_CHANNEL);
    RTC_TOUCH_ENTER_CRITICAL();
    RTCCNTL.touch_scan_ctrl.touch_scan_pad_map  &= ~(BIT(TOUCH_SHIELD_CHANNEL));
    RTCCNTL.touch_scan_ctrl.touch_shield_pad_en = 1;
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_waterproof_disable(void)
{
    RTC_TOUCH_ENTER_CRITICAL();
    RTCCNTL.touch_scan_ctrl.touch_shield_pad_en = 0;
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_proximity_set_config(touch_pad_proximity_t proximity)
{
    RTC_TOUCH_ENTER_CRITICAL();
    if (proximity.select_pad0) {
        SENS.sar_touch_conf.touch_approach_pad0 = proximity.select_pad0;
    }
    if (proximity.select_pad1) {
        SENS.sar_touch_conf.touch_approach_pad1 = proximity.select_pad1;
    }
    if (proximity.select_pad2) {
        SENS.sar_touch_conf.touch_approach_pad2 = proximity.select_pad2;
    }
    RTCCNTL.touch_approach.touch_approach_meas_time = proximity.meas_num;
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_proximity_get_config(touch_pad_proximity_t *proximity)
{
    if (proximity) {
        RTC_TOUCH_ENTER_CRITICAL();
        proximity->select_pad0 = SENS.sar_touch_conf.touch_approach_pad0;
        proximity->select_pad1 = SENS.sar_touch_conf.touch_approach_pad1;
        proximity->select_pad2 = SENS.sar_touch_conf.touch_approach_pad2;
        proximity->meas_num = RTCCNTL.touch_approach.touch_approach_meas_time;
        RTC_TOUCH_EXIT_CRITICAL();
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t touch_pad_proximity_get_meas_cnt(touch_pad_t touch_num, uint32_t *cnt)
{
    if (cnt == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (SENS.sar_touch_conf.touch_approach_pad0 == touch_num) {
        *cnt = SENS.sar_touch_status16.touch_approach_pad0_cnt;
    } else if (SENS.sar_touch_conf.touch_approach_pad1 == touch_num) {
        *cnt = SENS.sar_touch_status16.touch_approach_pad1_cnt;
    } else if (SENS.sar_touch_conf.touch_approach_pad2 == touch_num) {
        *cnt = SENS.sar_touch_status16.touch_approach_pad2_cnt;
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t touch_pad_proximity_data_get(touch_pad_t touch_num, uint32_t *measure_out)
{
    if ((SENS.sar_touch_conf.touch_approach_pad0 != touch_num)
            && (SENS.sar_touch_conf.touch_approach_pad1 != touch_num)
            && (SENS.sar_touch_conf.touch_approach_pad2 != touch_num)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (ESP_OK != touch_pad_filter_baseline_read(touch_num, measure_out)) {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t touch_pad_reset()
{
    RTC_TOUCH_ENTER_CRITICAL();
    RTCCNTL.touch_ctrl2.touch_reset = 0;
    RTCCNTL.touch_ctrl2.touch_reset = 1;
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

/************** sleep pad setting ***********************/

esp_err_t touch_pad_sleep_channel_config(touch_pad_sleep_channel_t slp_config)
{
    RTC_TOUCH_ENTER_CRITICAL();
    RTCCNTL.touch_slp_thres.touch_slp_pad = slp_config.touch_num;
    RTCCNTL.touch_slp_thres.touch_slp_th = slp_config.sleep_pad_threshold;
    RTCCNTL.touch_slp_thres.touch_slp_approach_en = slp_config.en_proximity;
    RTC_TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_sleep_channel_baseline_get(uint32_t *baseline)
{
    if (baseline) {
        *baseline = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS15_REG, SENS_TOUCH_SLP_BASELINE);
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t touch_pad_sleep_channel_debounce_get(uint32_t *debounce)
{
    if (debounce) {
        *debounce = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS15_REG, SENS_TOUCH_SLP_DEBOUNCE);
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t touch_pad_sleep_channel_proximity_cnt_get(uint32_t *approach_cnt)
{
    if (approach_cnt) {
        *approach_cnt = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS16_REG, SENS_TOUCH_SLP_APPROACH_CNT);
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t touch_pad_get_wakeup_status(touch_pad_t *pad_num)
{
    if (pad_num) {
        *pad_num = (touch_pad_t)RTCCNTL.touch_slp_thres.touch_slp_pad;
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}
