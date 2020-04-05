// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "esp_log.h"
#include "driver/pcnt.h"
#include "driver/periph_ctrl.h"
#include "hal/pcnt_hal.h"
#include "soc/pcnt_caps.h"

#define PCNT_CHANNEL_ERR_STR  "PCNT CHANNEL ERROR"
#define PCNT_UNIT_ERR_STR  "PCNT UNIT ERROR"
#define PCNT_GPIO_ERR_STR  "PCNT GPIO NUM ERROR"
#define PCNT_ADDRESS_ERR_STR  "PCNT ADDRESS ERROR"
#define PCNT_PARAM_ERR_STR   "PCNT PARAM ERROR"
#define PCNT_COUNT_MODE_ERR_STR "PCNT COUNTER MODE ERROR"
#define PCNT_CTRL_MODE_ERR_STR  "PCNT CTRL MODE ERROR"
#define PCNT_EVT_TYPE_ERR_STR   "PCNT value type error"
#define PCNT_LIMT_VAL_ERR_STR   "PCNT limit value error"
#define PCNT_NUM_ERR_STR   "PCNT num error"
#define PCNT_DRIVER_ERR_STR  "PCNT driver error"

#define PCNT_ENTER_CRITICAL(mux)    portENTER_CRITICAL(mux)
#define PCNT_EXIT_CRITICAL(mux)     portEXIT_CRITICAL(mux)

static const char *TAG = "pcnt";

#define PCNT_CHECK(a, str, ret_val) \
    if (!(a)) { \
        ESP_LOGE(TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    }

typedef struct {
    pcnt_hal_context_t hal;        /*!< PCNT hal context*/
} pcnt_obj_t;

static pcnt_obj_t *p_pcnt_obj[PCNT_PORT_MAX] = {0};

#define PCNT_OBJ_CHECK(pcnt_port) { \
    PCNT_CHECK((pcnt_port < PCNT_PORT_MAX), PCNT_NUM_ERR_STR, ESP_ERR_INVALID_ARG); \
    PCNT_CHECK((p_pcnt_obj[pcnt_port]), PCNT_DRIVER_ERR_STR, ESP_ERR_INVALID_STATE); \
}

typedef struct {
    void(*fn)(void *args);   /*!< isr function */
    void *args;              /*!< isr function args */
} pcnt_isr_func_t;

static pcnt_isr_func_t *pcnt_isr_func = NULL;
static pcnt_isr_handle_t pcnt_isr_service = NULL;
static portMUX_TYPE pcnt_spinlock = portMUX_INITIALIZER_UNLOCKED;

static inline esp_err_t _pcnt_set_mode(pcnt_port_t pcnt_port, pcnt_unit_t unit, pcnt_channel_t channel, pcnt_count_mode_t pos_mode, pcnt_count_mode_t neg_mode, pcnt_ctrl_mode_t hctrl_mode, pcnt_ctrl_mode_t lctrl_mode)
{
    PCNT_OBJ_CHECK(pcnt_port);
    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(channel < PCNT_CHANNEL_MAX, PCNT_CHANNEL_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK((pos_mode < PCNT_COUNT_MAX) && (neg_mode < PCNT_COUNT_MAX), PCNT_COUNT_MODE_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK((hctrl_mode < PCNT_MODE_MAX) && (lctrl_mode < PCNT_MODE_MAX), PCNT_CTRL_MODE_ERR_STR, ESP_ERR_INVALID_ARG);

    pcnt_hal_set_mode(&(p_pcnt_obj[pcnt_port]->hal), unit, channel, pos_mode, neg_mode, hctrl_mode, lctrl_mode);
    return ESP_OK;
}

static inline esp_err_t _pcnt_set_pin(pcnt_port_t pcnt_port, pcnt_unit_t unit, pcnt_channel_t channel, int pulse_io, int ctrl_io)
{
    PCNT_OBJ_CHECK(pcnt_port);
    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(channel < PCNT_CHANNEL_MAX, PCNT_CHANNEL_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(GPIO_IS_VALID_GPIO(pulse_io) || pulse_io < 0, PCNT_GPIO_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(GPIO_IS_VALID_GPIO(ctrl_io) || ctrl_io < 0, PCNT_GPIO_ERR_STR, ESP_ERR_INVALID_ARG);

    int sig_base  = (channel == 0) ? PCNT_SIG_CH0_IN0_IDX  : PCNT_SIG_CH1_IN0_IDX;
    int ctrl_base = (channel == 0) ? PCNT_CTRL_CH0_IN0_IDX : PCNT_CTRL_CH1_IN0_IDX;
    if (unit > 4) {
        sig_base  += 12; // GPIO matrix assignments have a gap between units 4 & 5
        ctrl_base += 12;
    }

    int input_sig_index = sig_base  + (4 * unit);
    int ctrl_sig_index  = ctrl_base + (4 * unit);

    if (pulse_io >= 0) {
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[pulse_io], PIN_FUNC_GPIO);
        gpio_set_direction(pulse_io, GPIO_MODE_INPUT);
        gpio_set_pull_mode(pulse_io, GPIO_PULLUP_ONLY);
        gpio_matrix_in(pulse_io, input_sig_index, 0);
    }

    if (ctrl_io >= 0) {
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[ctrl_io], PIN_FUNC_GPIO);
        gpio_set_direction(ctrl_io, GPIO_MODE_INPUT);
        gpio_set_pull_mode(ctrl_io, GPIO_PULLUP_ONLY);
        gpio_matrix_in(ctrl_io, ctrl_sig_index, 0);
    }

    return ESP_OK;
}

static inline esp_err_t _pcnt_get_counter_value(pcnt_port_t pcnt_port, pcnt_unit_t pcnt_unit, int16_t *count)
{
    PCNT_OBJ_CHECK(pcnt_port);
    PCNT_CHECK(pcnt_unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(count != NULL, PCNT_ADDRESS_ERR_STR, ESP_ERR_INVALID_ARG);
    pcnt_hal_get_counter_value(&(p_pcnt_obj[pcnt_port]->hal), pcnt_unit, count);
    return ESP_OK;
}

static inline esp_err_t _pcnt_counter_pause(pcnt_port_t pcnt_port, pcnt_unit_t pcnt_unit)
{
    PCNT_OBJ_CHECK(pcnt_port);
    PCNT_CHECK(pcnt_unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_ENTER_CRITICAL(&pcnt_spinlock);
    pcnt_hal_counter_pause(&(p_pcnt_obj[pcnt_port]->hal), pcnt_unit);
    PCNT_EXIT_CRITICAL(&pcnt_spinlock);
    return ESP_OK;
}

static inline esp_err_t _pcnt_counter_resume(pcnt_port_t pcnt_port, pcnt_unit_t pcnt_unit)
{
    PCNT_OBJ_CHECK(pcnt_port);
    PCNT_CHECK(pcnt_unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_ENTER_CRITICAL(&pcnt_spinlock);
    pcnt_hal_counter_resume(&(p_pcnt_obj[pcnt_port]->hal), pcnt_unit);
    PCNT_EXIT_CRITICAL(&pcnt_spinlock);
    return ESP_OK;
}

static inline esp_err_t _pcnt_counter_clear(pcnt_port_t pcnt_port, pcnt_unit_t pcnt_unit)
{
    PCNT_OBJ_CHECK(pcnt_port);
    PCNT_CHECK(pcnt_unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_ENTER_CRITICAL(&pcnt_spinlock);
    pcnt_hal_counter_clear(&(p_pcnt_obj[pcnt_port]->hal), pcnt_unit);
    PCNT_EXIT_CRITICAL(&pcnt_spinlock);
    return ESP_OK;
}

static inline esp_err_t _pcnt_intr_enable(pcnt_port_t pcnt_port, pcnt_unit_t pcnt_unit)
{
    PCNT_OBJ_CHECK(pcnt_port);
    PCNT_CHECK(pcnt_unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_ENTER_CRITICAL(&pcnt_spinlock);
    pcnt_hal_intr_enable(&(p_pcnt_obj[pcnt_port]->hal), pcnt_unit);
    PCNT_EXIT_CRITICAL(&pcnt_spinlock);
    return ESP_OK;
}

static inline esp_err_t _pcnt_intr_disable(pcnt_port_t pcnt_port, pcnt_unit_t pcnt_unit)
{
    PCNT_OBJ_CHECK(pcnt_port);
    PCNT_CHECK(pcnt_unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_ENTER_CRITICAL(&pcnt_spinlock);
    pcnt_hal_intr_disable(&(p_pcnt_obj[pcnt_port]->hal), pcnt_unit);
    PCNT_EXIT_CRITICAL(&pcnt_spinlock);
    return ESP_OK;
}

static inline esp_err_t _pcnt_event_enable(pcnt_port_t pcnt_port, pcnt_unit_t unit, pcnt_evt_type_t evt_type)
{
    PCNT_OBJ_CHECK(pcnt_port);
    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(evt_type < PCNT_EVT_MAX, PCNT_EVT_TYPE_ERR_STR, ESP_ERR_INVALID_ARG);
    pcnt_hal_event_enable(&(p_pcnt_obj[pcnt_port]->hal), unit, evt_type);
    return ESP_OK;
}

static inline esp_err_t _pcnt_event_disable(pcnt_port_t pcnt_port, pcnt_unit_t unit, pcnt_evt_type_t evt_type)
{
    PCNT_OBJ_CHECK(pcnt_port);
    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(evt_type < PCNT_EVT_MAX, PCNT_EVT_TYPE_ERR_STR, ESP_ERR_INVALID_ARG);
    pcnt_hal_event_disable(&(p_pcnt_obj[pcnt_port]->hal), unit, evt_type);
    return ESP_OK;
}

static inline esp_err_t _pcnt_set_event_value(pcnt_port_t pcnt_port, pcnt_unit_t unit, pcnt_evt_type_t evt_type, int16_t value)
{
    PCNT_OBJ_CHECK(pcnt_port);
    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(evt_type < PCNT_EVT_MAX, PCNT_EVT_TYPE_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(!(evt_type == PCNT_EVT_L_LIM && value > 0), PCNT_LIMT_VAL_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(!(evt_type == PCNT_EVT_H_LIM && value < 0), PCNT_LIMT_VAL_ERR_STR, ESP_ERR_INVALID_ARG);
    pcnt_hal_set_event_value(&(p_pcnt_obj[pcnt_port]->hal), unit, evt_type, value);
    return ESP_OK;
}

static inline esp_err_t _pcnt_get_event_value(pcnt_port_t pcnt_port, pcnt_unit_t unit, pcnt_evt_type_t evt_type, int16_t *value)
{
    PCNT_OBJ_CHECK(pcnt_port);
    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(evt_type < PCNT_EVT_MAX, PCNT_EVT_TYPE_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(value != NULL, PCNT_ADDRESS_ERR_STR, ESP_ERR_INVALID_ARG);

    pcnt_hal_get_event_value(&(p_pcnt_obj[pcnt_port]->hal), unit, evt_type, value);
    return ESP_OK;
}

static inline esp_err_t _pcnt_set_filter_value(pcnt_port_t pcnt_port, pcnt_unit_t unit, uint16_t filter_val)
{
    PCNT_OBJ_CHECK(pcnt_port);
    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(filter_val < 1024, PCNT_PARAM_ERR_STR, ESP_ERR_INVALID_ARG);
    pcnt_hal_set_filter_value(&(p_pcnt_obj[pcnt_port]->hal), unit, filter_val);
    return ESP_OK;
}

static inline esp_err_t _pcnt_get_filter_value(pcnt_port_t pcnt_port, pcnt_unit_t unit, uint16_t *filter_val)
{
    PCNT_OBJ_CHECK(pcnt_port);
    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(filter_val != NULL, PCNT_ADDRESS_ERR_STR, ESP_ERR_INVALID_ARG);

    pcnt_hal_get_filter_value(&(p_pcnt_obj[pcnt_port]->hal), unit, filter_val);
    return ESP_OK;
}

static inline esp_err_t _pcnt_filter_enable(pcnt_port_t pcnt_port, pcnt_unit_t unit)
{
    PCNT_OBJ_CHECK(pcnt_port);
    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    pcnt_hal_filter_enable(&(p_pcnt_obj[pcnt_port]->hal), unit);
    return ESP_OK;
}

static inline esp_err_t _pcnt_filter_disable(pcnt_port_t pcnt_port, pcnt_unit_t unit)
{
    PCNT_OBJ_CHECK(pcnt_port);
    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    pcnt_hal_filter_disable(&(p_pcnt_obj[pcnt_port]->hal), unit);
    return ESP_OK;
}

static inline esp_err_t _pcnt_isr_handler_add(pcnt_port_t pcnt_port, pcnt_unit_t unit, void(*isr_handler)(void *), void *args)
{
    PCNT_OBJ_CHECK(pcnt_port);
    PCNT_CHECK(pcnt_isr_func != NULL, "ISR service is not installed, call pcnt_install_isr_service() first", ESP_ERR_INVALID_STATE);
    PCNT_CHECK(unit < PCNT_UNIT_MAX, "PCNT unit error", ESP_ERR_INVALID_ARG);
    PCNT_ENTER_CRITICAL(&pcnt_spinlock);
    _pcnt_intr_disable(PCNT_PORT_0, unit);

    if (pcnt_isr_func) {
        pcnt_isr_func[unit].fn = isr_handler;
        pcnt_isr_func[unit].args = args;
    }

    _pcnt_intr_enable(PCNT_PORT_0, unit);
    PCNT_EXIT_CRITICAL(&pcnt_spinlock);
    return ESP_OK;
}

static inline esp_err_t _pcnt_isr_handler_remove(pcnt_port_t pcnt_port, pcnt_unit_t unit)
{
    PCNT_OBJ_CHECK(pcnt_port);
    PCNT_CHECK(pcnt_isr_func != NULL, "ISR service is not installed", ESP_ERR_INVALID_STATE);
    PCNT_CHECK(unit < PCNT_UNIT_MAX, "PCNT unit error", ESP_ERR_INVALID_ARG);
    PCNT_ENTER_CRITICAL(&pcnt_spinlock);
    _pcnt_intr_disable(PCNT_PORT_0, unit);

    if (pcnt_isr_func) {
        pcnt_isr_func[unit].fn = NULL;
        pcnt_isr_func[unit].args = NULL;
    }

    PCNT_EXIT_CRITICAL(&pcnt_spinlock);
    return ESP_OK;
}

// pcnt interrupt service
static void IRAM_ATTR pcnt_intr_service(void *arg)
{
    uint32_t status;
    pcnt_port_t pcnt_port = (pcnt_port_t)arg;
    pcnt_hal_get_intr_status(&(p_pcnt_obj[pcnt_port]->hal), &status);
    
    while (status) {
        int unit = __builtin_ffs(status) - 1;
        status &= ~(1 << unit);

        if (pcnt_isr_func[unit].fn != NULL) {
            (pcnt_isr_func[unit].fn)(pcnt_isr_func[unit].args);
        }
    }
    pcnt_hal_clear_intr_status(&(p_pcnt_obj[pcnt_port]->hal), status);
}

static inline esp_err_t _pcnt_isr_service_install(pcnt_port_t pcnt_port, int intr_alloc_flags)
{
    PCNT_OBJ_CHECK(pcnt_port);
    PCNT_CHECK(pcnt_isr_func == NULL, "ISR service already installed", ESP_ERR_INVALID_STATE);
    esp_err_t ret = ESP_FAIL;
    pcnt_isr_func = (pcnt_isr_func_t *) calloc(PCNT_UNIT_MAX, sizeof(pcnt_isr_func_t));

    if (pcnt_isr_func == NULL) {
        ret = ESP_ERR_NO_MEM;
    } else {
        ret = pcnt_isr_register(pcnt_intr_service, (void *)pcnt_port, intr_alloc_flags, &pcnt_isr_service);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "pcnt isr registration failed, maybe you need `pcnt_isr_unregister` to unregister your isr");
            free(pcnt_isr_func);
            pcnt_isr_func = NULL;
        }
    }

    return ret;
}

static inline esp_err_t _pcnt_isr_service_uninstall(pcnt_port_t pcnt_port)
{
    PCNT_OBJ_CHECK(pcnt_port);
    PCNT_CHECK(pcnt_isr_func != NULL, "ISR Service not installed yet.", ESP_ERR_INVALID_STATE);
    esp_err_t ret = ESP_FAIL;
    ret = pcnt_isr_unregister(pcnt_isr_service);
    free(pcnt_isr_func);
    pcnt_isr_func = NULL;
    pcnt_isr_service = NULL;

    return ret;
}

static inline esp_err_t _pcnt_unit_config(pcnt_port_t pcnt_port, const pcnt_config_t *pcnt_config)
{
    PCNT_OBJ_CHECK(pcnt_port);
    uint8_t unit = pcnt_config->unit;
    uint8_t channel = pcnt_config->channel;
    int input_io = pcnt_config->pulse_gpio_num;
    int ctrl_io = pcnt_config->ctrl_gpio_num;

    PCNT_CHECK(unit < PCNT_UNIT_MAX, PCNT_UNIT_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(channel < PCNT_CHANNEL_MAX, PCNT_CHANNEL_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK(input_io < 0 || (GPIO_IS_VALID_GPIO(input_io) && (input_io != ctrl_io)), "PCNT pulse input io error", ESP_ERR_INVALID_ARG);
    PCNT_CHECK(ctrl_io < 0 || GPIO_IS_VALID_GPIO(ctrl_io), "PCNT ctrl io error", ESP_ERR_INVALID_ARG);
    PCNT_CHECK((pcnt_config->pos_mode < PCNT_COUNT_MAX) && (pcnt_config->neg_mode < PCNT_COUNT_MAX), PCNT_COUNT_MODE_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK((pcnt_config->hctrl_mode < PCNT_MODE_MAX) && (pcnt_config->lctrl_mode < PCNT_MODE_MAX), PCNT_CTRL_MODE_ERR_STR, ESP_ERR_INVALID_ARG);
    /*Enalbe hardware module*/
    static bool pcnt_enable = false;
    if (pcnt_enable == false) {
        periph_module_reset(PERIPH_PCNT_MODULE);
        pcnt_enable = true;
    }
    periph_module_enable(PERIPH_PCNT_MODULE);
    /*Set counter range*/
    _pcnt_set_event_value(pcnt_port, unit, PCNT_EVT_H_LIM, pcnt_config->counter_h_lim);
    _pcnt_set_event_value(pcnt_port, unit, PCNT_EVT_L_LIM, pcnt_config->counter_l_lim);
    /*Default value after reboot is positive, we disable these events like others*/
    _pcnt_event_disable(pcnt_port, unit, PCNT_EVT_H_LIM);
    _pcnt_event_disable(pcnt_port, unit, PCNT_EVT_L_LIM);
    _pcnt_event_disable(pcnt_port, unit, PCNT_EVT_ZERO);
    _pcnt_filter_disable(pcnt_port, unit);
    /*set pulse input and control mode*/
    _pcnt_set_mode(pcnt_port, unit, channel, pcnt_config->pos_mode, pcnt_config->neg_mode, pcnt_config->hctrl_mode, pcnt_config->lctrl_mode);
    /*Set pulse input and control pins*/
    _pcnt_set_pin(pcnt_port, unit, channel, input_io, ctrl_io);
    return ESP_OK;
}

esp_err_t pcnt_deinit(pcnt_port_t pcnt_port)
{
    PCNT_OBJ_CHECK(pcnt_port);

    heap_caps_free(p_pcnt_obj[pcnt_port]);
    p_pcnt_obj[pcnt_port] = NULL;
    return ESP_OK;
}

esp_err_t pcnt_init(pcnt_port_t pcnt_port)
{
    PCNT_CHECK((pcnt_port < PCNT_PORT_MAX), PCNT_NUM_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_CHECK((p_pcnt_obj[pcnt_port]) == NULL, "pcnt driver already initted", ESP_ERR_INVALID_STATE);

    p_pcnt_obj[pcnt_port] = (pcnt_obj_t *)heap_caps_calloc(1, sizeof(pcnt_obj_t), MALLOC_CAP_DEFAULT);

    if (p_pcnt_obj[pcnt_port] == NULL) {
        ESP_LOGE(TAG, "PCNT driver malloc error");
        return ESP_FAIL;
    }

    pcnt_hal_init(&(p_pcnt_obj[pcnt_port]->hal), pcnt_port);
    return ESP_OK;
}

// TODO: The following functions are wrappers, for compatibility with current API.

esp_err_t pcnt_unit_config(const pcnt_config_t *pcnt_config)
{
    esp_err_t ret;

    if ((p_pcnt_obj[PCNT_PORT_0]) == NULL) {
        ret = pcnt_init(PCNT_PORT_0);
        if (ret != ESP_OK) {
            return ret;
        }
    }
    return _pcnt_unit_config(PCNT_PORT_0, pcnt_config);
}

esp_err_t pcnt_set_mode(pcnt_unit_t unit, pcnt_channel_t channel, pcnt_count_mode_t pos_mode, pcnt_count_mode_t neg_mode, pcnt_ctrl_mode_t hctrl_mode, pcnt_ctrl_mode_t lctrl_mode)
{
    return _pcnt_set_mode(PCNT_PORT_0, unit, channel, pos_mode, neg_mode, hctrl_mode, lctrl_mode);
}

esp_err_t pcnt_set_pin(pcnt_unit_t unit, pcnt_channel_t channel, int pulse_io, int ctrl_io)
{
    return _pcnt_set_pin(PCNT_PORT_0, unit, channel, pulse_io, ctrl_io);
}

esp_err_t pcnt_get_counter_value(pcnt_unit_t pcnt_unit, int16_t *count)
{
    return _pcnt_get_counter_value(PCNT_PORT_0, pcnt_unit, count);
}

esp_err_t pcnt_counter_pause(pcnt_unit_t pcnt_unit)
{
    return _pcnt_counter_pause(PCNT_PORT_0, pcnt_unit);
}

esp_err_t pcnt_counter_resume(pcnt_unit_t pcnt_unit)
{
    return _pcnt_counter_resume(PCNT_PORT_0, pcnt_unit);
}

esp_err_t pcnt_counter_clear(pcnt_unit_t pcnt_unit)
{
    return _pcnt_counter_clear(PCNT_PORT_0, pcnt_unit);
}

esp_err_t pcnt_intr_enable(pcnt_unit_t pcnt_unit)
{
    return _pcnt_intr_enable(PCNT_PORT_0, pcnt_unit);
}

esp_err_t pcnt_intr_disable(pcnt_unit_t pcnt_unit)
{
    return _pcnt_intr_disable(PCNT_PORT_0, pcnt_unit);
}

esp_err_t pcnt_event_enable(pcnt_unit_t unit, pcnt_evt_type_t evt_type)
{
    return _pcnt_event_enable(PCNT_PORT_0, unit, evt_type);
}

esp_err_t pcnt_event_disable(pcnt_unit_t unit, pcnt_evt_type_t evt_type)
{
    return _pcnt_event_disable(PCNT_PORT_0, unit, evt_type);
}

esp_err_t pcnt_set_event_value(pcnt_unit_t unit, pcnt_evt_type_t evt_type, int16_t value)
{
    return _pcnt_set_event_value(PCNT_PORT_0, unit, evt_type, value);
}

esp_err_t pcnt_get_event_value(pcnt_unit_t unit, pcnt_evt_type_t evt_type, int16_t *value)
{
    return _pcnt_get_event_value(PCNT_PORT_0, unit, evt_type, value);
}

esp_err_t pcnt_set_filter_value(pcnt_unit_t unit, uint16_t filter_val)
{
    return _pcnt_set_filter_value(PCNT_PORT_0, unit, filter_val);
}

esp_err_t pcnt_get_filter_value(pcnt_unit_t unit, uint16_t *filter_val)
{
    return _pcnt_get_filter_value(PCNT_PORT_0, unit, filter_val);
}

esp_err_t pcnt_filter_enable(pcnt_unit_t unit)
{
    return _pcnt_filter_enable(PCNT_PORT_0, unit);
}

esp_err_t pcnt_filter_disable(pcnt_unit_t unit)
{
    return _pcnt_filter_disable(PCNT_PORT_0, unit);
}

esp_err_t pcnt_isr_unregister(pcnt_isr_handle_t handle)
{
    esp_err_t ret = ESP_FAIL;
    PCNT_ENTER_CRITICAL(&pcnt_spinlock);
    ret = esp_intr_free(handle);
    PCNT_EXIT_CRITICAL(&pcnt_spinlock);
    return ret;
}

esp_err_t pcnt_isr_register(void (*fun)(void *), void *arg, int intr_alloc_flags, pcnt_isr_handle_t *handle)
{
    esp_err_t ret = ESP_FAIL;
    PCNT_CHECK(fun != NULL, PCNT_ADDRESS_ERR_STR, ESP_ERR_INVALID_ARG);
    PCNT_ENTER_CRITICAL(&pcnt_spinlock);
    ret = esp_intr_alloc(ETS_PCNT_INTR_SOURCE, intr_alloc_flags, fun, arg, handle);
    PCNT_EXIT_CRITICAL(&pcnt_spinlock);
    return ret;
}

esp_err_t pcnt_isr_handler_add(pcnt_unit_t unit, void(*isr_handler)(void *), void *args)
{
    return _pcnt_isr_handler_add(PCNT_PORT_0, unit, isr_handler, args);
}

esp_err_t pcnt_isr_handler_remove(pcnt_unit_t unit)
{
    return _pcnt_isr_handler_remove(PCNT_PORT_0, unit);
}


esp_err_t pcnt_isr_service_install(int intr_alloc_flags)
{
    return _pcnt_isr_service_install(PCNT_PORT_0, intr_alloc_flags);
}

void pcnt_isr_service_uninstall()
{
    _pcnt_isr_service_uninstall(PCNT_PORT_0);
}