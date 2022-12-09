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

#include "esp_log.h"
#include "esp_err.h"
#include "driver/sigmadelta.h"
#include "esp_heap_caps.h"
#include "hal/sigmadelta_hal.h"

static const char *TAG = "SIGMADELTA";

#define SIGMADELTA_CHECK(a,str,ret_val) if(!(a)) { \
        ESP_LOGE(TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str); \
        return (ret_val); \
    }

typedef struct {
    sigmadelta_hal_context_t hal;        /*!< SIGMADELTA hal context*/
} sigmadelta_obj_t;

static sigmadelta_obj_t *p_sigmadelta_obj[SIGMADELTA_PORT_MAX] = {0};

#define SIGMADELTA_OBJ_CHECK(sigmadelta_port) { \
    SIGMADELTA_CHECK((sigmadelta_port < SIGMADELTA_PORT_MAX), "sigmadelta port error", ESP_ERR_INVALID_ARG); \
    SIGMADELTA_CHECK((p_sigmadelta_obj[sigmadelta_port]), "sigmadelta driver has not been initialized", ESP_ERR_INVALID_STATE); \
}

static inline esp_err_t _sigmadelta_set_duty(sigmadelta_port_t sigmadelta_port, sigmadelta_channel_t channel, int8_t duty)
{
    SIGMADELTA_OBJ_CHECK(sigmadelta_port);
    
    sigmadelta_hal_set_duty(&(p_sigmadelta_obj[sigmadelta_port]->hal), channel, duty);
    return ESP_OK;
}

static inline esp_err_t _sigmadelta_set_prescale(sigmadelta_port_t sigmadelta_port, sigmadelta_channel_t channel, uint8_t prescale)
{
    SIGMADELTA_OBJ_CHECK(sigmadelta_port);

    sigmadelta_hal_set_prescale(&(p_sigmadelta_obj[sigmadelta_port]->hal), channel, prescale);
    return ESP_OK;
}

static inline esp_err_t _sigmadelta_set_pin(sigmadelta_port_t sigmadelta_port, sigmadelta_channel_t channel, gpio_num_t gpio_num)
{
    SIGMADELTA_OBJ_CHECK(sigmadelta_port);

    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[gpio_num], PIN_FUNC_GPIO);
    gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
    gpio_matrix_out(gpio_num, GPIO_SD0_OUT_IDX + channel, 0, 0);
    return ESP_OK;
}

static inline esp_err_t _sigmadelta_config(sigmadelta_port_t sigmadelta_port, const sigmadelta_config_t *config)
{
    SIGMADELTA_OBJ_CHECK(sigmadelta_port);

    _sigmadelta_set_duty(sigmadelta_port, config->channel, config->sigmadelta_duty);
    _sigmadelta_set_prescale(sigmadelta_port, config->channel, config->sigmadelta_prescale);
    _sigmadelta_set_pin(sigmadelta_port, config->channel, config->sigmadelta_gpio);
    return ESP_OK;
}

esp_err_t sigmadelta_deinit(sigmadelta_port_t sigmadelta_port)
{
    SIGMADELTA_OBJ_CHECK(sigmadelta_port);

    heap_caps_free(p_sigmadelta_obj[sigmadelta_port]);
    p_sigmadelta_obj[sigmadelta_port] = NULL;
    return ESP_OK;
}

esp_err_t sigmadelta_init(sigmadelta_port_t sigmadelta_port)
{
    SIGMADELTA_CHECK((sigmadelta_port < SIGMADELTA_PORT_MAX), "sigmadelta_port error", ESP_ERR_INVALID_ARG);
    SIGMADELTA_CHECK((p_sigmadelta_obj[sigmadelta_port]) == NULL, "sigmadelta driver already initialized", ESP_ERR_INVALID_STATE);

    p_sigmadelta_obj[sigmadelta_port] = (sigmadelta_obj_t *) heap_caps_calloc(1, sizeof(sigmadelta_obj_t), MALLOC_CAP_DEFAULT);

    if (p_sigmadelta_obj[sigmadelta_port] == NULL) {
        ESP_LOGE(TAG, "TAG driver malloc error");
        return ESP_FAIL;
    }

    sigmadelta_hal_init(&(p_sigmadelta_obj[sigmadelta_port]->hal), sigmadelta_port);
    return ESP_OK;
}

// TODO: The following functions are wrappers, for compatibility with current API.

esp_err_t sigmadelta_set_duty(sigmadelta_channel_t channel, int8_t duty)
{
    SIGMADELTA_CHECK(channel < SIGMADELTA_CHANNEL_MAX, "sigmadelta channel error", ESP_ERR_INVALID_ARG);
    return _sigmadelta_set_duty(SIGMADELTA_PORT_0, channel, duty);
}

esp_err_t sigmadelta_set_prescale(sigmadelta_channel_t channel, uint8_t prescale)
{
    SIGMADELTA_CHECK(channel < SIGMADELTA_CHANNEL_MAX, "sigmadelta channel error", ESP_ERR_INVALID_ARG);
    return _sigmadelta_set_prescale(SIGMADELTA_PORT_0, channel, prescale);
}

esp_err_t sigmadelta_set_pin(sigmadelta_channel_t channel, gpio_num_t gpio_num)
{
    SIGMADELTA_CHECK(channel < SIGMADELTA_CHANNEL_MAX, "sigmadelta channel error", ESP_ERR_INVALID_ARG);
    SIGMADELTA_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "sigmadelta gpio num error", ESP_ERR_INVALID_ARG);
    return _sigmadelta_set_pin(SIGMADELTA_PORT_0, channel, gpio_num);
}

esp_err_t sigmadelta_config(const sigmadelta_config_t *config)
{
    SIGMADELTA_CHECK(config->channel < SIGMADELTA_CHANNEL_MAX, "sigmadelta channel error", ESP_ERR_INVALID_ARG);
    SIGMADELTA_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(config->sigmadelta_gpio), "sigmadelta gpio num error", ESP_ERR_INVALID_ARG);
    esp_err_t ret;

    if ((p_sigmadelta_obj[SIGMADELTA_PORT_0]) == NULL) {
        ret = sigmadelta_init(SIGMADELTA_PORT_0);
        if (ret != ESP_OK) {
            return ret;
        }
    }
    return _sigmadelta_config(SIGMADELTA_PORT_0, config);
}
