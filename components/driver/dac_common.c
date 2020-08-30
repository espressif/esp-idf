// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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


#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "driver/rtc_io.h"
#include "driver/dac.h"
#include "soc/dac_periph.h"
#include "hal/dac_hal.h"

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.

static const char *DAC_TAG = "DAC";

#define DAC_CHECK(a, str, ret_val) ({                                               \
    if (!(a)) {                                                                     \
        ESP_LOGE(DAC_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);   \
        return (ret_val);                                                           \
    }                                                                               \
})

/*---------------------------------------------------------------
                    DAC
---------------------------------------------------------------*/
esp_err_t dac_pad_get_io_num(dac_channel_t channel, gpio_num_t *gpio_num)
{
    DAC_CHECK(channel < DAC_CHANNEL_MAX, "DAC channel error", ESP_ERR_INVALID_ARG);

    *gpio_num = (gpio_num_t)dac_periph_signal.dac_channel_io_num[channel];

    return ESP_OK;
}

static esp_err_t dac_rtc_pad_init(dac_channel_t channel)
{
    DAC_CHECK(channel < DAC_CHANNEL_MAX, "DAC channel error", ESP_ERR_INVALID_ARG);

    gpio_num_t gpio_num = 0;
    dac_pad_get_io_num(channel, &gpio_num);
    rtc_gpio_init(gpio_num);
    rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED);
    rtc_gpio_pullup_dis(gpio_num);
    rtc_gpio_pulldown_dis(gpio_num);

    return ESP_OK;
}

esp_err_t dac_output_enable(dac_channel_t channel)
{
    DAC_CHECK(channel < DAC_CHANNEL_MAX, "DAC channel error", ESP_ERR_INVALID_ARG);

    dac_rtc_pad_init(channel);
    portENTER_CRITICAL(&rtc_spinlock);
    dac_hal_power_on(channel);
    dac_hal_rtc_sync_by_adc(false);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t dac_output_disable(dac_channel_t channel)
{
    DAC_CHECK(channel < DAC_CHANNEL_MAX, "DAC channel error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&rtc_spinlock);
    dac_hal_power_down(channel);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t dac_output_voltage(dac_channel_t channel, uint8_t dac_value)
{
    DAC_CHECK(channel < DAC_CHANNEL_MAX, "DAC channel error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&rtc_spinlock);
    dac_hal_update_output_value(channel, dac_value);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t dac_out_voltage(dac_channel_t channel, uint8_t dac_value)
{
    DAC_CHECK(channel < DAC_CHANNEL_MAX, "DAC channel error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&rtc_spinlock);
    dac_hal_update_output_value(channel, dac_value);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t dac_cw_generator_enable(void)
{
    portENTER_CRITICAL(&rtc_spinlock);
    dac_hal_cw_generator_enable();
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t dac_cw_generator_disable(void)
{
    portENTER_CRITICAL(&rtc_spinlock);
    dac_hal_cw_generator_disable();
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t dac_cw_generator_config(dac_cw_config_t *cw)
{
    if (!cw) return ESP_ERR_INVALID_ARG;

    portENTER_CRITICAL(&rtc_spinlock);
    dac_hal_cw_generator_config(cw);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}