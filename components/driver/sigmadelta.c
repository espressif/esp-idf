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
#include "soc/soc.h"
#include "driver/periph_ctrl.h"
#include "driver/sigmadelta.h"

static const char* SIGMADELTA_TAG = "SIGMADELTA";
#define SIGMADELTA_CHANNEL_ERR_STR  "SIGMADELTA CHANNEL ERROR"
#define SIGMADELTA_IO_ERR_STR       "SIGMADELTA GPIO NUM ERROR"

#define SIGMADELTA_CHECK(a,str,ret_val) if(!(a)) { \
	    ESP_LOGE(SIGMADELTA_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str); \
		return (ret_val); \
        }

esp_err_t sigmadelta_config(sigmadelta_config_t *config)
{
    SIGMADELTA_CHECK(config->channel < SIGMADELTA_CHANNEL_MAX, SIGMADELTA_CHANNEL_ERR_STR, ESP_ERR_INVALID_ARG);
    SIGMADELTA_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(config->sigmadelta_gpio), SIGMADELTA_IO_ERR_STR, ESP_ERR_INVALID_ARG);
    sigmadelta_set_duty(config->channel, config->sigmadelta_duty);
    sigmadelta_set_prescale(config->channel, config->sigmadelta_prescale);
    sigmadelta_set_pin(config->channel, config->sigmadelta_gpio);
    return ESP_OK;
}

esp_err_t sigmadelta_set_duty(sigmadelta_channel_t channel, int8_t duty)
{
    SIGMADELTA_CHECK(channel < SIGMADELTA_CHANNEL_MAX, SIGMADELTA_CHANNEL_ERR_STR, ESP_ERR_INVALID_ARG);
    SIGMADELTA.channel[channel].duty = duty;
    return ESP_OK;
}

esp_err_t sigmadelta_set_prescale(sigmadelta_channel_t channel, uint8_t prescale)
{
    SIGMADELTA_CHECK(channel < SIGMADELTA_CHANNEL_MAX, SIGMADELTA_CHANNEL_ERR_STR, ESP_ERR_INVALID_ARG);
    SIGMADELTA.channel[channel].prescale = prescale;
    return ESP_OK;
}

esp_err_t sigmadelta_set_pin(sigmadelta_channel_t channel, gpio_num_t gpio_num)
{
    SIGMADELTA_CHECK(channel < SIGMADELTA_CHANNEL_MAX, SIGMADELTA_CHANNEL_ERR_STR, ESP_ERR_INVALID_ARG);
    SIGMADELTA_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), SIGMADELTA_IO_ERR_STR, ESP_ERR_INVALID_ARG);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[gpio_num], PIN_FUNC_GPIO);
    gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
    gpio_matrix_out(gpio_num, GPIO_SD0_OUT_IDX + channel, 0, 0);
    return ESP_OK;
}

