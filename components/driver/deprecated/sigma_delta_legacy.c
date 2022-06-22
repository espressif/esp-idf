/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "driver/gpio.h"
#include "driver/sigmadelta_types_legacy.h"
#include "soc/sdm_periph.h"
#include "hal/sdm_hal.h"
#include "hal/sdm_ll.h"
#include "hal/gpio_hal.h"
#include "esp_rom_gpio.h"

static const char *TAG = "sdm(legacy)";

#define SIGMADELTA_CHECK(a,str,ret_val) ESP_RETURN_ON_FALSE(a, ret_val, TAG, "%s", str)

typedef struct {
    sdm_hal_context_t hal;        /*!< SIGMADELTA hal context*/
} sigmadelta_obj_t;

static sigmadelta_obj_t *p_sigmadelta_obj[SIGMADELTA_PORT_MAX] = {0};

#define SIGMADELTA_OBJ_CHECK(sigmadelta_port) { \
    SIGMADELTA_CHECK((sigmadelta_port < SIGMADELTA_PORT_MAX), "sigmadelta port error", ESP_ERR_INVALID_ARG); \
    SIGMADELTA_CHECK((p_sigmadelta_obj[sigmadelta_port]), "sigmadelta driver has not been initialized", ESP_ERR_INVALID_STATE); \
}

static inline esp_err_t _sigmadelta_set_duty(sigmadelta_port_t sigmadelta_port, sigmadelta_channel_t channel, int8_t duty)
{
    SIGMADELTA_OBJ_CHECK(sigmadelta_port);

    sdm_ll_set_duty(p_sigmadelta_obj[sigmadelta_port]->hal.dev, channel, duty);
    return ESP_OK;
}

static inline esp_err_t _sigmadelta_set_prescale(sigmadelta_port_t sigmadelta_port, sigmadelta_channel_t channel, uint8_t prescale)
{
    SIGMADELTA_OBJ_CHECK(sigmadelta_port);

    sdm_ll_set_prescale(p_sigmadelta_obj[sigmadelta_port]->hal.dev, channel, prescale + 1);
    return ESP_OK;
}

static inline esp_err_t _sigmadelta_set_pin(sigmadelta_port_t sigmadelta_port, sigmadelta_channel_t channel, gpio_num_t gpio_num)
{
    SIGMADELTA_OBJ_CHECK(sigmadelta_port);

    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_num], PIN_FUNC_GPIO);
    gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
    esp_rom_gpio_connect_out_signal(gpio_num, sigma_delta_periph_signals.channels[channel].sd_sig, 0, 0);
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

    sdm_hal_init(&(p_sigmadelta_obj[sigmadelta_port]->hal), sigmadelta_port);
    sdm_ll_enable_clock(p_sigmadelta_obj[sigmadelta_port]->hal.dev, true);
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
