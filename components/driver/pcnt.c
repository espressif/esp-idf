// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "driver/pcnt.h"
#include "driver/periph_ctrl.h"

#define PCNT_CHANNEL_ERR_STR  "PCNT CHANNEL ERROR"
#define PCNT_UNIT_ERR_STR  "PCNT UNIT ERROR"
#define PCNT_GPIO_ERR_STR  "PCNT GPIO NUM ERROR"
#define PCNT_ADDRESS_ERR_STR  "PCNT ADDRESS ERROR"
#define PCNT_PARAM_ERR_STR   "PCNT PARAM ERROR"
#define PCNT_COUNT_MODE_ERR_STR "PCNT COUNTER MODE ERROR"
#define PCNT_CTRL_MODE_ERR_STR  "PCNT CTRL MODE ERROR"
#define PCNT_EVT_TYPE_ERR_STR   "PCNT value type error"

static const char* PCNT_TAG = "pcnt";
#define PCNT_CHECK(a, str, ret_val) \
    if (!(a)) { \
        ESP_LOGE(PCNT_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    }

static portMUX_TYPE pcnt_spinlock = portMUX_INITIALIZER_UNLOCKED;

#define PCNT_ENTER_CRITICAL(mux)    portENTER_CRITICAL(mux)
#define PCNT_EXIT_CRITICAL(mux)     portEXIT_CRITICAL(mux)
#define PCNT_ENTER_CRITICAL_ISR(mux)    portENTER_CRITICAL_ISR(mux)
#define PCNT_EXIT_CRITICAL_ISR(mux)     portEXIT_CRITICAL_ISR(mux)

esp_err_t pcnt_unit_config(pcnt_config_t *pcnt_config)
{
    uint8_t unit = pcnt_config->unit;
    uint8_t channel = pcnt_config->channel;
    int input_io = pcnt_config->pulse_gpio_num;
    int ctrl_io = pcnt_config->ctrl_gpio_num;

    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(channel < PCNT_CHANNEL_MAX, PCNT_CHANNEL_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(input_io < 0 || (GPIO_IS_VALID_GPIO(input_io) && (input_io != ctrl_io)), "PCNT pluse input io error", ESP_ERR_INVALID_ARG);
    PCNT_CHECK(ctrl_io < 0 || GPIO_IS_VALID_GPIO(ctrl_io), "PCNT ctrl io error", ESP_ERR_INVALID_ARG);
    PCNT_CHECK((pcnt_config->pos_mode < PCNT_COUNT_MAX) && (pcnt_config->neg_mode < PCNT_COUNT_MAX), PCNT_COUNT_MODE_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK((pcnt_config->hctrl_mode < PCNT_MODE_MAX) && (pcnt_config->lctrl_mode < PCNT_MODE_MAX), PCNT_CTRL_MODE_ERR_STR, ESP_ERR_INVALID_ARG);
    /*Enalbe hardware module*/
    periph_module_enable(PERIPH_PCNT_MODULE);
    /*Set counter range*/
    pcnt_set_event_value(unit, PCNT_EVT_H_LIM, pcnt_config->counter_h_lim);
    pcnt_set_event_value(unit, PCNT_EVT_L_LIM, pcnt_config->counter_l_lim);
    /*Default value after reboot is positive, we disable these events like others*/
    pcnt_event_disable(unit, PCNT_EVT_H_LIM);
    pcnt_event_disable(unit, PCNT_EVT_L_LIM);
    pcnt_event_disable(unit, PCNT_EVT_ZERO);
    pcnt_filter_disable(unit);
    /*set pulse input and control mode*/
    pcnt_set_mode(unit, channel, pcnt_config->pos_mode, pcnt_config->neg_mode, pcnt_config->hctrl_mode, pcnt_config->lctrl_mode);
    /*Set pulse input and control pins*/
    pcnt_set_pin(unit, channel, input_io, ctrl_io);
    return ESP_OK;
}

esp_err_t pcnt_set_mode(pcnt_unit_t unit, pcnt_channel_t channel, pcnt_count_mode_t pos_mode, pcnt_count_mode_t neg_mode, pcnt_ctrl_mode_t hctrl_mode, pcnt_ctrl_mode_t lctrl_mode)
{
    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(channel < PCNT_CHANNEL_MAX, PCNT_CHANNEL_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK((pos_mode < PCNT_COUNT_MAX) && (neg_mode < PCNT_COUNT_MAX), PCNT_COUNT_MODE_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK((hctrl_mode < PCNT_MODE_MAX) && (lctrl_mode < PCNT_MODE_MAX), PCNT_CTRL_MODE_ERR_STR, ESP_ERR_INVALID_ARG);

    if(channel == 0) {
        PCNT.conf_unit[unit].conf0.ch0_pos_mode = pos_mode;
        PCNT.conf_unit[unit].conf0.ch0_neg_mode = neg_mode;
        PCNT.conf_unit[unit].conf0.ch0_hctrl_mode = hctrl_mode;
        PCNT.conf_unit[unit].conf0.ch0_lctrl_mode = lctrl_mode;
    } else {
        PCNT.conf_unit[unit].conf0.ch1_pos_mode = pos_mode;
        PCNT.conf_unit[unit].conf0.ch1_neg_mode = neg_mode;
        PCNT.conf_unit[unit].conf0.ch1_hctrl_mode = hctrl_mode;
        PCNT.conf_unit[unit].conf0.ch1_lctrl_mode = lctrl_mode;
    }
    return ESP_OK;
}

esp_err_t pcnt_set_pin(pcnt_unit_t unit, pcnt_channel_t channel, int pulse_io, int ctrl_io)
{
    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(channel < PCNT_CHANNEL_MAX, PCNT_CHANNEL_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(GPIO_IS_VALID_GPIO(pulse_io) || pulse_io < 0, PCNT_GPIO_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(GPIO_IS_VALID_GPIO(ctrl_io) || ctrl_io < 0, PCNT_GPIO_ERR_STR, ESP_ERR_INVALID_ARG);
    int input_sig_index = (channel == 0 ? PCNT_SIG_CH0_IN0_IDX + 4 * unit : PCNT_SIG_CH1_IN0_IDX + 4 * unit);
    int ctrl_sig_index = (channel == 0 ? PCNT_CTRL_CH0_IN0_IDX + 4 * unit : PCNT_CTRL_CH1_IN0_IDX + 4 * unit);
    if(pulse_io >= 0) {
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[pulse_io], PIN_FUNC_GPIO);
        gpio_set_direction(pulse_io, GPIO_MODE_INPUT);
        gpio_set_pull_mode(pulse_io, GPIO_PULLUP_ONLY);
        gpio_matrix_in(pulse_io, input_sig_index, 0);
    }
    if(ctrl_io >= 0) {
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[ctrl_io], PIN_FUNC_GPIO);
        gpio_set_direction(ctrl_io, GPIO_MODE_INPUT);
        gpio_set_pull_mode(ctrl_io, GPIO_PULLUP_ONLY);
        gpio_matrix_in(ctrl_io, ctrl_sig_index, 0);
    }
    return ESP_OK;
}

esp_err_t pcnt_get_counter_value(pcnt_unit_t pcnt_unit, int16_t* count)
{
    PCNT_CHECK(pcnt_unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(count != NULL, PCNT_ADDRESS_ERR_STR, ESP_ERR_INVALID_ARG);
    *count = (int16_t) PCNT.cnt_unit[pcnt_unit].cnt_val;
    return ESP_OK;
}

esp_err_t pcnt_counter_pause(pcnt_unit_t pcnt_unit)
{
    PCNT_CHECK(pcnt_unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_ENTER_CRITICAL(&pcnt_spinlock);
    PCNT.ctrl.val |= BIT(PCNT_CNT_PAUSE_U0_S + (pcnt_unit * 2));
    PCNT_EXIT_CRITICAL(&pcnt_spinlock);
    return ESP_OK;
}

esp_err_t pcnt_counter_resume(pcnt_unit_t pcnt_unit)
{
    PCNT_CHECK(pcnt_unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_ENTER_CRITICAL(&pcnt_spinlock);
    PCNT.ctrl.val &= (~(BIT(PCNT_CNT_PAUSE_U0_S + (pcnt_unit * 2))));
    PCNT_EXIT_CRITICAL(&pcnt_spinlock);
    return ESP_OK;
}

esp_err_t pcnt_counter_clear(pcnt_unit_t pcnt_unit)
{
    PCNT_CHECK(pcnt_unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_ENTER_CRITICAL(&pcnt_spinlock);
    PCNT.ctrl.val &= (~(BIT(PCNT_PLUS_CNT_RST_U0_S + (pcnt_unit * 2))));
    PCNT_EXIT_CRITICAL(&pcnt_spinlock);
    return ESP_OK;
}

esp_err_t pcnt_intr_enable(pcnt_unit_t pcnt_unit)
{
    PCNT_CHECK(pcnt_unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_ENTER_CRITICAL(&pcnt_spinlock);
    PCNT.int_ena.val |= BIT(PCNT_CNT_THR_EVENT_U0_INT_ENA_S + pcnt_unit);
    PCNT_EXIT_CRITICAL(&pcnt_spinlock);
    return ESP_OK;
}

esp_err_t pcnt_intr_disable(pcnt_unit_t pcnt_unit)
{
    PCNT_CHECK(pcnt_unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_ENTER_CRITICAL(&pcnt_spinlock);
    PCNT.int_ena.val &= (~(BIT(PCNT_CNT_THR_EVENT_U0_INT_ENA_S + pcnt_unit)));
    PCNT_EXIT_CRITICAL(&pcnt_spinlock);
    return ESP_OK;
}

esp_err_t pcnt_event_enable(pcnt_unit_t unit, pcnt_evt_type_t evt_type)
{
    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(evt_type < PCNT_EVT_MAX, PCNT_EVT_TYPE_ERR_STR, ESP_ERR_INVALID_ARG);
    if(evt_type == PCNT_EVT_L_LIM) {
        PCNT.conf_unit[unit].conf0.thr_l_lim_en = 1;
    } else if(evt_type == PCNT_EVT_H_LIM) {
        PCNT.conf_unit[unit].conf0.thr_h_lim_en = 1;
    } else if(evt_type == PCNT_EVT_THRES_0) {
        PCNT.conf_unit[unit].conf0.thr_thres0_en = 1;
    } else if(evt_type == PCNT_EVT_THRES_1) {
        PCNT.conf_unit[unit].conf0.thr_thres1_en = 1;
    } else if(evt_type == PCNT_EVT_ZERO) {
        PCNT.conf_unit[unit].conf0.thr_zero_en = 1;
    }
    return ESP_OK;
}

esp_err_t pcnt_event_disable(pcnt_unit_t unit, pcnt_evt_type_t evt_type)
{
    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(evt_type < PCNT_EVT_MAX, PCNT_EVT_TYPE_ERR_STR, ESP_ERR_INVALID_ARG);
    if(evt_type == PCNT_EVT_L_LIM) {
        PCNT.conf_unit[unit].conf0.thr_l_lim_en = 0;
    } else if(evt_type == PCNT_EVT_H_LIM) {
        PCNT.conf_unit[unit].conf0.thr_h_lim_en = 0;
    } else if(evt_type == PCNT_EVT_THRES_0) {
        PCNT.conf_unit[unit].conf0.thr_thres0_en = 0;
    } else if(evt_type == PCNT_EVT_THRES_1) {
        PCNT.conf_unit[unit].conf0.thr_thres1_en = 0;
    } else if(evt_type == PCNT_EVT_ZERO) {
        PCNT.conf_unit[unit].conf0.thr_zero_en = 0;
    }
    return ESP_OK;
}

esp_err_t pcnt_set_event_value(pcnt_unit_t unit, pcnt_evt_type_t evt_type, int16_t value)
{
    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(evt_type < PCNT_EVT_MAX, PCNT_EVT_TYPE_ERR_STR, ESP_ERR_INVALID_ARG);
    if(evt_type == PCNT_EVT_L_LIM) {
        PCNT.conf_unit[unit].conf2.cnt_l_lim = value;
    } else if(evt_type == PCNT_EVT_H_LIM) {
        PCNT.conf_unit[unit].conf2.cnt_h_lim = value;
    } else if(evt_type == PCNT_EVT_THRES_0) {
        PCNT.conf_unit[unit].conf1.cnt_thres0 = value;
    } else if(evt_type == PCNT_EVT_THRES_1) {
        PCNT.conf_unit[unit].conf1.cnt_thres1 = value;
    }
    return ESP_OK;
}

esp_err_t pcnt_get_event_value(pcnt_unit_t unit, pcnt_evt_type_t evt_type, int16_t *value)
{
    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(evt_type < PCNT_EVT_MAX, PCNT_EVT_TYPE_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(value != NULL, PCNT_ADDRESS_ERR_STR, ESP_ERR_INVALID_ARG);

    if(evt_type == PCNT_EVT_L_LIM) {
        *value = (int16_t) PCNT.conf_unit[unit].conf2.cnt_l_lim;
    } else if(evt_type == PCNT_EVT_H_LIM) {
        *value = (int16_t) PCNT.conf_unit[unit].conf2.cnt_h_lim;
    } else if(evt_type == PCNT_EVT_THRES_0) {
        *value = (int16_t) PCNT.conf_unit[unit].conf1.cnt_thres0;
    } else if(evt_type == PCNT_EVT_THRES_1) {
        *value = (int16_t) PCNT.conf_unit[unit].conf1.cnt_thres1;
    } else {
        *value = 0;
    }
    return ESP_OK;
}

esp_err_t pcnt_set_filter_value(pcnt_unit_t unit, uint16_t filter_val)
{
    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(filter_val < 1024, PCNT_PARAM_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT.conf_unit[unit].conf0.filter_thres = filter_val;
    return ESP_OK;
}

esp_err_t pcnt_get_filter_value(pcnt_unit_t unit, uint16_t *filter_val)
{
    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(filter_val != NULL, PCNT_ADDRESS_ERR_STR, ESP_ERR_INVALID_ARG);

    *filter_val = PCNT.conf_unit[unit].conf0.filter_thres;
    return ESP_OK;
}

esp_err_t pcnt_filter_enable(pcnt_unit_t unit)
{
    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT.conf_unit[unit].conf0.filter_en = 1;
    return ESP_OK;
}

esp_err_t pcnt_filter_disable(pcnt_unit_t unit)
{
    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT.conf_unit[unit].conf0.filter_en = 0;
    return ESP_OK;
}

esp_err_t pcnt_isr_register(void (*fun)(void*), void * arg, int intr_alloc_flags, pcnt_isr_handle_t *handle)
{
    PCNT_CHECK(fun != NULL, PCNT_ADDRESS_ERR_STR, ESP_ERR_INVALID_ARG);
    return esp_intr_alloc(ETS_PCNT_INTR_SOURCE, intr_alloc_flags, fun, arg, handle);
}

