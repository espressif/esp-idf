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
#include "soc/syscon_periph.h"
#include "soc/rtc.h"
#include "soc/periph_defs.h"
#include "rtc_io.h"
#include "touch_pad.h"
#include "adc.h"
#include "dac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_intr_alloc.h"
#include "sys/lock.h"
#include "driver/rtc_cntl.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "adc1_i2s_private.h"
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

#define TOUCH_PAD_FILTER_FACTOR_DEFAULT   (4)   // IIR filter coefficient.
#define TOUCH_PAD_SHIFT_DEFAULT           (4)   // Increase computing accuracy.
#define TOUCH_PAD_SHIFT_ROUND_DEFAULT     (8)   // ROUND = 2^(n-1); rounding off for fractional.

portMUX_TYPE rtc_spinlock = portMUX_INITIALIZER_UNLOCKED;
#if CONFIG_IDF_TARGET_ESP32
static SemaphoreHandle_t rtc_touch_mux = NULL;
#endif

#if CONFIG_IDF_TARGET_ESP32
typedef struct {
    TimerHandle_t timer;
    uint16_t filtered_val[TOUCH_PAD_MAX];
    uint16_t raw_val[TOUCH_PAD_MAX];
    uint32_t filter_period;
    uint32_t period;
    bool enable;
} touch_pad_filter_t;
static touch_pad_filter_t *s_touch_pad_filter = NULL;
// check if touch pad be inited.
static uint16_t s_touch_pad_init_bit = 0x0000;
static filter_cb_t s_filter_cb = NULL;
#endif

#if CONFIG_IDF_TARGET_ESP32

static const char *RTC_MODULE_TAG = "RTC_MODULE";

#define RTC_MODULE_CHECK(a, str, ret_val) if (!(a)) {                                \
    ESP_LOGE(RTC_MODULE_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str); \
    return (ret_val);                                                              \
}

#define RTC_RES_CHECK(res, ret_val) if ( (a) != ESP_OK) {                           \
        ESP_LOGE(RTC_MODULE_TAG,"%s:%d (%s)", __FILE__, __LINE__, __FUNCTION__); \
        return (ret_val);                                                              \
}

/*---------------------------------------------------------------
                    Touch Pad
---------------------------------------------------------------*/
//Some register bits of touch sensor 8 and 9 are mismatched, we need to swap the bits.
#define BITSWAP(data, n, m)   (((data >> n) &  0x1)  == ((data >> m) & 0x1) ? (data) : ((data) ^ ((0x1 <<n) | (0x1 << m))))
#define TOUCH_BITS_SWAP(v)  BITSWAP(v, TOUCH_PAD_NUM8, TOUCH_PAD_NUM9)
static esp_err_t _touch_pad_read(touch_pad_t touch_num, uint16_t *touch_value, touch_fsm_mode_t mode);

//Some registers of touch sensor 8 and 9 are mismatched, we need to swap register index
inline static touch_pad_t touch_pad_num_wrap(touch_pad_t touch_num)
{
    if (touch_num == TOUCH_PAD_NUM8) {
        return TOUCH_PAD_NUM9;
    } else if (touch_num == TOUCH_PAD_NUM9) {
        return TOUCH_PAD_NUM8;
    }
    return touch_num;
}

esp_err_t touch_pad_isr_handler_register(void (*fn)(void *), void *arg, int no_use, intr_handle_t *handle_no_use)
{
    RTC_MODULE_CHECK(fn, "Touch_Pad ISR null", ESP_ERR_INVALID_ARG);
#if CONFIG_IDF_TARGET_ESP32
    return rtc_isr_register(fn, arg, RTC_CNTL_TOUCH_INT_ST_M);
#else
    return ESP_FAIL;
#endif
}

esp_err_t touch_pad_isr_register(intr_handler_t fn, void* arg)
{
    RTC_MODULE_CHECK(fn, "Touch_Pad ISR null", ESP_ERR_INVALID_ARG);
#if CONFIG_IDF_TARGET_ESP32
    return rtc_isr_register(fn, arg, RTC_CNTL_TOUCH_INT_ST_M);
#else
    return ESP_FAIL;
#endif
}

esp_err_t touch_pad_isr_deregister(intr_handler_t fn, void *arg)
{
    return rtc_isr_deregister(fn, arg);
}

static esp_err_t touch_pad_get_io_num(touch_pad_t touch_num, gpio_num_t *gpio_num)
{
    switch (touch_num) {
    case TOUCH_PAD_NUM0:
        *gpio_num = TOUCH_PAD_NUM0_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM1:
        *gpio_num = TOUCH_PAD_NUM1_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM2:
        *gpio_num = TOUCH_PAD_NUM2_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM3:
        *gpio_num = TOUCH_PAD_NUM3_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM4:
        *gpio_num = TOUCH_PAD_NUM4_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM5:
        *gpio_num = TOUCH_PAD_NUM5_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM6:
        *gpio_num = TOUCH_PAD_NUM6_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM7:
        *gpio_num = TOUCH_PAD_NUM7_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM8:
        *gpio_num = TOUCH_PAD_NUM8_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM9:
        *gpio_num = TOUCH_PAD_NUM9_GPIO_NUM;
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
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
    if(s_filter_cb != NULL) {
        //return the raw data and filtered data.
        s_filter_cb(s_touch_pad_filter->raw_val, s_touch_pad_filter->filtered_val);
    }
}

esp_err_t touch_pad_set_meas_time(uint16_t sleep_cycle, uint16_t meas_cycle)
{
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    portENTER_CRITICAL(&rtc_spinlock);
    //touch sensor sleep cycle Time = sleep_cycle / RTC_SLOW_CLK( can be 150k or 32k depending on the options)
    SENS.sar_touch_ctrl2.touch_sleep_cycles = sleep_cycle;
    //touch sensor measure time= meas_cycle / 8Mhz
    SENS.sar_touch_ctrl1.touch_meas_delay = meas_cycle;
    //the waiting cycles (in 8MHz) between TOUCH_START and TOUCH_XPD
    SENS.sar_touch_ctrl1.touch_xpd_wait = TOUCH_PAD_MEASURE_WAIT_DEFAULT;
    portEXIT_CRITICAL(&rtc_spinlock);
    xSemaphoreGive(rtc_touch_mux);
    return ESP_OK;
}

esp_err_t touch_pad_get_meas_time(uint16_t *sleep_cycle, uint16_t *meas_cycle)
{
    portENTER_CRITICAL(&rtc_spinlock);
    if (sleep_cycle) {
        *sleep_cycle = SENS.sar_touch_ctrl2.touch_sleep_cycles;
    }
    if (meas_cycle) {
        *meas_cycle = SENS.sar_touch_ctrl1.touch_meas_delay;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_set_voltage(touch_high_volt_t refh, touch_low_volt_t refl, touch_volt_atten_t atten)
{
    RTC_MODULE_CHECK(((refh < TOUCH_HVOLT_MAX) && (refh >= (int )TOUCH_HVOLT_KEEP)), "touch refh error",
            ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(((refl < TOUCH_LVOLT_MAX) && (refh >= (int )TOUCH_LVOLT_KEEP)), "touch refl error",
            ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(((atten < TOUCH_HVOLT_ATTEN_MAX) && (refh >= (int )TOUCH_HVOLT_ATTEN_KEEP)), "touch atten error",
            ESP_ERR_INVALID_ARG);
#if CONFIG_IDF_TARGET_ESP32
    portENTER_CRITICAL(&rtc_spinlock);
    if (refh > TOUCH_HVOLT_KEEP) {
        RTCIO.touch_cfg.drefh = refh;
    }
    if (refl > TOUCH_LVOLT_KEEP) {
        RTCIO.touch_cfg.drefl = refl;
    }
    if (atten > TOUCH_HVOLT_ATTEN_KEEP) {
        RTCIO.touch_cfg.drange = atten;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
#endif
    return ESP_OK;
}

esp_err_t touch_pad_get_voltage(touch_high_volt_t *refh, touch_low_volt_t *refl, touch_volt_atten_t *atten)
{
#if CONFIG_IDF_TARGET_ESP32
    portENTER_CRITICAL(&rtc_spinlock);
    if (refh) {
        *refh = RTCIO.touch_cfg.drefh;
    }
    if (refl) {
        *refl = RTCIO.touch_cfg.drefl;
    }
    if (atten) {
        *atten = RTCIO.touch_cfg.drange;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
#endif
    return ESP_OK;
}

esp_err_t touch_pad_set_cnt_mode(touch_pad_t touch_num, touch_cnt_slope_t slope, touch_tie_opt_t opt)
{
    RTC_MODULE_CHECK((slope < TOUCH_PAD_SLOPE_MAX), "touch slope error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK((opt < TOUCH_PAD_TIE_OPT_MAX), "touch opt error", ESP_ERR_INVALID_ARG);

    touch_pad_t touch_pad_wrap = touch_pad_num_wrap(touch_num);
    portENTER_CRITICAL(&rtc_spinlock);
    RTCIO.touch_pad[touch_pad_wrap].tie_opt = opt;
    RTCIO.touch_pad[touch_num].dac = slope;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_cnt_mode(touch_pad_t touch_num, touch_cnt_slope_t *slope, touch_tie_opt_t *opt)
{
    RTC_MODULE_CHECK((touch_num < TOUCH_PAD_MAX), "touch IO error", ESP_ERR_INVALID_ARG);

    touch_pad_t touch_pad_wrap = touch_pad_num_wrap(touch_num);
    portENTER_CRITICAL(&rtc_spinlock);
    if(opt) {
        *opt = RTCIO.touch_pad[touch_pad_wrap].tie_opt;
    }
    if(slope) {
        *slope = RTCIO.touch_pad[touch_num].dac;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_io_init(touch_pad_t touch_num)
{
    RTC_MODULE_CHECK((touch_num < TOUCH_PAD_MAX), "touch IO error", ESP_ERR_INVALID_ARG);
    gpio_num_t gpio_num = GPIO_NUM_0;
    touch_pad_get_io_num(touch_num, &gpio_num);
    rtc_gpio_init(gpio_num);
    rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED);
    rtc_gpio_pulldown_dis(gpio_num);
    rtc_gpio_pullup_dis(gpio_num);
    return ESP_OK;
}

esp_err_t touch_pad_set_fsm_mode(touch_fsm_mode_t mode)
{
    RTC_MODULE_CHECK((mode < TOUCH_FSM_MODE_MAX), "touch fsm mode error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_ctrl2.touch_start_en = 0;
    SENS.sar_touch_ctrl2.touch_start_force = mode;
    RTCCNTL.state0.touch_slp_timer_en = (mode == TOUCH_FSM_MODE_TIMER ? 1 : 0);
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_fsm_mode(touch_fsm_mode_t *mode)
{
    if (mode) {
        *mode = SENS.sar_touch_ctrl2.touch_start_force;
    }
    return ESP_OK;
}

esp_err_t touch_pad_sw_start(void)
{
    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_ctrl2.touch_start_en = 0;
    SENS.sar_touch_ctrl2.touch_start_en = 1;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_set_thresh(touch_pad_t touch_num, uint16_t threshold)
{
    RTC_MODULE_CHECK((touch_num < TOUCH_PAD_MAX), "touch IO error", ESP_ERR_INVALID_ARG);
    touch_pad_t tp_wrap = touch_pad_num_wrap(touch_num);
    portENTER_CRITICAL(&rtc_spinlock);
    if (tp_wrap & 0x1) {
        SENS.touch_thresh[tp_wrap / 2].l_thresh = threshold;
    } else {
        SENS.touch_thresh[tp_wrap / 2].h_thresh = threshold;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_thresh(touch_pad_t touch_num, uint16_t *threshold)
{
    RTC_MODULE_CHECK((touch_num < TOUCH_PAD_MAX), "touch IO error", ESP_ERR_INVALID_ARG);
    touch_pad_t tp_wrap = touch_pad_num_wrap(touch_num);
    if (threshold) {
        *threshold = (tp_wrap & 0x1 )? \
                SENS.touch_thresh[tp_wrap / 2].l_thresh : \
                SENS.touch_thresh[tp_wrap / 2].h_thresh;
    }
    return ESP_OK;
}

esp_err_t touch_pad_set_trigger_mode(touch_trigger_mode_t mode)
{
    RTC_MODULE_CHECK((mode < TOUCH_TRIGGER_MAX), "touch trigger mode error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_ctrl1.touch_out_sel = mode;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_trigger_mode(touch_trigger_mode_t *mode)
{
    if (mode) {
        *mode = SENS.sar_touch_ctrl1.touch_out_sel;
    }
    return ESP_OK;
}

esp_err_t touch_pad_set_trigger_source(touch_trigger_src_t src)
{
    RTC_MODULE_CHECK((src < TOUCH_TRIGGER_SOURCE_MAX), "touch trigger source error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_ctrl1.touch_out_1en = src;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_trigger_source(touch_trigger_src_t *src)
{
    if (src) {
        *src = SENS.sar_touch_ctrl1.touch_out_1en;
    }
    return ESP_OK;
}

esp_err_t touch_pad_set_group_mask(uint16_t set1_mask, uint16_t set2_mask, uint16_t en_mask)
{
    RTC_MODULE_CHECK((set1_mask <= TOUCH_PAD_BIT_MASK_MAX), "touch set1 bitmask error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK((set2_mask <= TOUCH_PAD_BIT_MASK_MAX), "touch set2 bitmask error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK((en_mask <= TOUCH_PAD_BIT_MASK_MAX), "touch work_en bitmask error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_enable.touch_pad_outen1 |= TOUCH_BITS_SWAP(set1_mask);
    SENS.sar_touch_enable.touch_pad_outen2 |= TOUCH_BITS_SWAP(set2_mask);
    SENS.sar_touch_enable.touch_pad_worken |= TOUCH_BITS_SWAP(en_mask);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t touch_pad_get_group_mask(uint16_t *set1_mask, uint16_t *set2_mask, uint16_t *en_mask)
{
    portENTER_CRITICAL(&rtc_spinlock);
    if (set1_mask) {
        *set1_mask = TOUCH_BITS_SWAP(SENS.sar_touch_enable.touch_pad_outen1);
    }
    if (set2_mask) {
        *set2_mask = TOUCH_BITS_SWAP(SENS.sar_touch_enable.touch_pad_outen2);
    }
    if (en_mask) {
        *en_mask = TOUCH_BITS_SWAP(SENS.sar_touch_enable.touch_pad_worken);
    }
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t touch_pad_clear_group_mask(uint16_t set1_mask, uint16_t set2_mask, uint16_t en_mask)
{
    RTC_MODULE_CHECK((set1_mask <= TOUCH_PAD_BIT_MASK_MAX), "touch set1 bitmask error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK((set2_mask <= TOUCH_PAD_BIT_MASK_MAX), "touch set2 bitmask error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK((en_mask <= TOUCH_PAD_BIT_MASK_MAX), "touch work_en bitmask error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_enable.touch_pad_outen1 &= TOUCH_BITS_SWAP(~set1_mask);
    SENS.sar_touch_enable.touch_pad_outen2 &= TOUCH_BITS_SWAP(~set2_mask);
    SENS.sar_touch_enable.touch_pad_worken &= TOUCH_BITS_SWAP(~en_mask);
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

uint32_t IRAM_ATTR touch_pad_get_status(void)
{
    uint32_t status = SENS.sar_touch_ctrl2.touch_meas_en;
    return TOUCH_BITS_SWAP(status);
}

esp_err_t IRAM_ATTR touch_pad_clear_status(void)
{
    SENS.sar_touch_ctrl2.touch_meas_en_clr = 1;
    return ESP_OK;
}

esp_err_t touch_pad_intr_enable(void)
{
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.int_ena.rtc_touch = 1;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_intr_disable(void)
{
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.int_ena.rtc_touch = 0;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_config(touch_pad_t touch_num, uint16_t threshold)
{
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);
    RTC_MODULE_CHECK(touch_num < TOUCH_PAD_MAX, "Touch_Pad Num Err", ESP_ERR_INVALID_ARG);
    touch_fsm_mode_t mode;
    touch_pad_set_thresh(touch_num, threshold);
    touch_pad_io_init(touch_num);
    touch_pad_set_cnt_mode(touch_num, TOUCH_PAD_SLOPE_7, TOUCH_PAD_TIE_OPT_LOW);
    touch_pad_get_fsm_mode(&mode);
    if (TOUCH_FSM_MODE_SW == mode) {
        touch_pad_clear_group_mask((1 << touch_num), (1 << touch_num), (1 << touch_num));
        s_touch_pad_init_bit |= (1 << touch_num);
    } else if (TOUCH_FSM_MODE_TIMER == mode){
        uint16_t sleep_time = 0;
        uint16_t meas_cycle = 0;
        uint32_t wait_time_ms = 0;
        uint32_t wait_tick = 0;
        uint32_t rtc_clk = rtc_clk_slow_freq_get_hz();
        touch_pad_set_group_mask((1 << touch_num), (1 << touch_num), (1 << touch_num));
        touch_pad_get_meas_time(&sleep_time, &meas_cycle);
        //If the FSM mode is 'TOUCH_FSM_MODE_TIMER', The data will be ready after one measurement cycle
        //after this function is executed, otherwise, the "touch_value" by "touch_pad_read" is 0.
        wait_time_ms = sleep_time/(rtc_clk/1000) + meas_cycle/(RTC_FAST_CLK_FREQ_APPROX/1000);
        wait_tick = wait_time_ms/portTICK_RATE_MS;
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
    touch_pad_intr_disable();
    touch_pad_clear_group_mask(TOUCH_PAD_BIT_MASK_MAX, TOUCH_PAD_BIT_MASK_MAX, TOUCH_PAD_BIT_MASK_MAX);
    touch_pad_set_trigger_mode(TOUCH_TRIGGER_MODE_DEFAULT);
    touch_pad_set_trigger_source(TOUCH_TRIGGER_SOURCE_DEFAULT);
    touch_pad_clear_status();
    touch_pad_set_meas_time(TOUCH_PAD_SLEEP_CYCLE_DEFAULT, TOUCH_PAD_MEASURE_CYCLE_DEFAULT);
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_DEFAULT);
    return ESP_OK;
}

esp_err_t touch_pad_deinit(void)
{
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);
    if (s_touch_pad_filter != NULL) {
        touch_pad_filter_stop();
        touch_pad_filter_delete();
    }
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    s_touch_pad_init_bit = 0x0000;
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_SW);
    touch_pad_clear_status();
    touch_pad_intr_disable();
    xSemaphoreGive(rtc_touch_mux);
    vSemaphoreDelete(rtc_touch_mux);
    rtc_touch_mux = NULL;
    return ESP_OK;
}

static esp_err_t _touch_pad_read(touch_pad_t touch_num, uint16_t *touch_value, touch_fsm_mode_t mode)
{
    esp_err_t res = ESP_OK;
    touch_pad_t tp_wrap = touch_pad_num_wrap(touch_num);
    if (TOUCH_FSM_MODE_SW == mode) {
        touch_pad_set_group_mask((1 << touch_num), (1 << touch_num), (1 << touch_num));
        touch_pad_sw_start();
        while (SENS.sar_touch_ctrl2.touch_meas_done == 0) {};
        *touch_value = (tp_wrap & 0x1) ? \
                            SENS.touch_meas[tp_wrap / 2].l_val: \
                            SENS.touch_meas[tp_wrap / 2].h_val;

        touch_pad_clear_group_mask((1 << touch_num), (1 << touch_num), (1 << touch_num));
    } else if (TOUCH_FSM_MODE_TIMER == mode) {
        while (SENS.sar_touch_ctrl2.touch_meas_done == 0) {};
        *touch_value = (tp_wrap & 0x1) ? \
                            SENS.touch_meas[tp_wrap / 2].l_val: \
                            SENS.touch_meas[tp_wrap / 2].h_val;
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
    RTC_MODULE_CHECK(touch_num < TOUCH_PAD_MAX, "Touch_Pad Num Err", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(touch_value != NULL, "touch_value", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);

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
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);
    RTC_MODULE_CHECK(touch_num < TOUCH_PAD_MAX, "Touch_Pad Num Err", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(touch_value != NULL, "touch_value", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_FAIL);
    *touch_value = s_touch_pad_filter->raw_val[touch_num];
    if (*touch_value == 0) {
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

IRAM_ATTR esp_err_t touch_pad_read_filtered(touch_pad_t touch_num, uint16_t *touch_value)
{
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);
    RTC_MODULE_CHECK(touch_num < TOUCH_PAD_MAX, "Touch_Pad Num Err", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(touch_value != NULL, "touch_value", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_FAIL);
    *touch_value = (s_touch_pad_filter->filtered_val[touch_num]);
    if (*touch_value == 0) {
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

esp_err_t touch_pad_set_filter_period(uint32_t new_period_ms)
{
    RTC_MODULE_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_ERR_INVALID_STATE);
    RTC_MODULE_CHECK(new_period_ms > 0, "Touch pad filter period error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_ERR_INVALID_STATE);

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter != NULL) {
        xTimerChangePeriod(s_touch_pad_filter->timer, new_period_ms / portTICK_PERIOD_MS, portMAX_DELAY);
        s_touch_pad_filter->period = new_period_ms;
    } else {
        ESP_LOGE(RTC_MODULE_TAG, "Touch pad filter deleted");
        ret = ESP_ERR_INVALID_STATE;
    }
    xSemaphoreGive(rtc_touch_mux);
    return ret;
}

esp_err_t touch_pad_get_filter_period(uint32_t* p_period_ms)
{
    RTC_MODULE_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_ERR_INVALID_STATE);
    RTC_MODULE_CHECK(p_period_ms != NULL, "Touch pad period pointer error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_ERR_INVALID_STATE);

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter != NULL) {
        *p_period_ms = s_touch_pad_filter->period;
    } else {
        ESP_LOGE(RTC_MODULE_TAG, "Touch pad filter deleted");
        ret = ESP_ERR_INVALID_STATE;
    }
    xSemaphoreGive(rtc_touch_mux);
    return ret;
}

esp_err_t touch_pad_filter_start(uint32_t filter_period_ms)
{
    RTC_MODULE_CHECK(filter_period_ms >= portTICK_PERIOD_MS, "Touch pad filter period error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_ERR_INVALID_STATE);

    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter == NULL) {
        s_touch_pad_filter = (touch_pad_filter_t *) calloc(1, sizeof(touch_pad_filter_t));
        if (s_touch_pad_filter == NULL) {
            goto err_no_mem;
        }
    }
    if (s_touch_pad_filter->timer == NULL) {
        s_touch_pad_filter->timer = xTimerCreate("filter_tmr", filter_period_ms / portTICK_PERIOD_MS, pdFALSE,
        NULL, (void(*)(TimerHandle_t))touch_pad_filter_cb);
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
    RTC_MODULE_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_ERR_INVALID_STATE);
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_ERR_INVALID_STATE);
    esp_err_t ret = ESP_OK;
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter != NULL) {
        xTimerStop(s_touch_pad_filter->timer, portMAX_DELAY);
    } else {
        ESP_LOGE(RTC_MODULE_TAG, "Touch pad filter deleted");
        ret = ESP_ERR_INVALID_STATE;
    }
    xSemaphoreGive(rtc_touch_mux);
    return ret;
}

esp_err_t touch_pad_filter_delete(void)
{
    RTC_MODULE_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_ERR_INVALID_STATE);
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_ERR_INVALID_STATE);
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

esp_err_t touch_pad_get_wakeup_status(touch_pad_t *pad_num)
{
    uint32_t touch_mask = SENS.sar_touch_ctrl2.touch_meas_en;
    if(touch_mask == 0) {
        return ESP_FAIL;
    }
    *pad_num = touch_pad_num_wrap((touch_pad_t)(__builtin_ffs(touch_mask) - 1));
    return ESP_OK;
}
#endif

/*---------------------------------------------------------------
                        INTERRUPT HANDLER
---------------------------------------------------------------*/


typedef struct rtc_isr_handler_ {
    uint32_t mask;
    intr_handler_t handler;
    void* handler_arg;
    SLIST_ENTRY(rtc_isr_handler_) next;
} rtc_isr_handler_t;

static SLIST_HEAD(rtc_isr_handler_list_, rtc_isr_handler_) s_rtc_isr_handler_list =
        SLIST_HEAD_INITIALIZER(s_rtc_isr_handler_list);
portMUX_TYPE s_rtc_isr_handler_list_lock = portMUX_INITIALIZER_UNLOCKED;
static intr_handle_t s_rtc_isr_handle;

static void rtc_isr(void* arg)
{
    uint32_t status = REG_READ(RTC_CNTL_INT_ST_REG);
    rtc_isr_handler_t* it;
    portENTER_CRITICAL_ISR(&s_rtc_isr_handler_list_lock);
    SLIST_FOREACH(it, &s_rtc_isr_handler_list, next) {
        if (it->mask & status) {
            portEXIT_CRITICAL_ISR(&s_rtc_isr_handler_list_lock);
            (*it->handler)(it->handler_arg);
            portENTER_CRITICAL_ISR(&s_rtc_isr_handler_list_lock);
        }
    }
    portEXIT_CRITICAL_ISR(&s_rtc_isr_handler_list_lock);
    REG_WRITE(RTC_CNTL_INT_CLR_REG, status);
}

static esp_err_t rtc_isr_ensure_installed(void)
{
    esp_err_t err = ESP_OK;
    portENTER_CRITICAL(&s_rtc_isr_handler_list_lock);
    if (s_rtc_isr_handle) {
        goto out;
    }

    REG_WRITE(RTC_CNTL_INT_ENA_REG, 0);
    REG_WRITE(RTC_CNTL_INT_CLR_REG, UINT32_MAX);
    err = esp_intr_alloc(ETS_RTC_CORE_INTR_SOURCE, 0, &rtc_isr, NULL, &s_rtc_isr_handle);
    if (err != ESP_OK) {
        goto out;
    }

out:
    portEXIT_CRITICAL(&s_rtc_isr_handler_list_lock);
    return err;
}


esp_err_t rtc_isr_register(intr_handler_t handler, void* handler_arg, uint32_t rtc_intr_mask)
{
    esp_err_t err = rtc_isr_ensure_installed();
    if (err != ESP_OK) {
        return err;
    }

    rtc_isr_handler_t* item = malloc(sizeof(*item));
    if (item == NULL) {
        return ESP_ERR_NO_MEM;
    }
    item->handler = handler;
    item->handler_arg = handler_arg;
    item->mask = rtc_intr_mask;
    portENTER_CRITICAL(&s_rtc_isr_handler_list_lock);
    SLIST_INSERT_HEAD(&s_rtc_isr_handler_list, item, next);
    portEXIT_CRITICAL(&s_rtc_isr_handler_list_lock);
    return ESP_OK;
}

esp_err_t rtc_isr_deregister(intr_handler_t handler, void* handler_arg)
{
    rtc_isr_handler_t* it;
    rtc_isr_handler_t* prev = NULL;
    bool found = false;
    portENTER_CRITICAL(&s_rtc_isr_handler_list_lock);
    SLIST_FOREACH(it, &s_rtc_isr_handler_list, next) {
        if (it->handler == handler && it->handler_arg == handler_arg) {
            if (it == SLIST_FIRST(&s_rtc_isr_handler_list)) {
                SLIST_REMOVE_HEAD(&s_rtc_isr_handler_list, next);
            } else {
                SLIST_REMOVE_AFTER(prev, next);
            }
            found = true;
            free(it);
            break;
        }
        prev = it;
    }
    portEXIT_CRITICAL(&s_rtc_isr_handler_list_lock);
    return found ? ESP_OK : ESP_ERR_INVALID_STATE;
}
