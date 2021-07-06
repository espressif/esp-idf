// Copyright 2016-2019 Espressif Systems (Shanghai) PTE LTD
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
#include "soc/rtc.h"
#include "soc/periph_defs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_intr_alloc.h"
#include "driver/rtc_io.h"
#include "driver/rtc_cntl.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "sdkconfig.h"

#include "esp32/rom/ets_sys.h"

#ifndef NDEBUG
// Enable built-in checks in queue.h in debug builds
#define INVARIANTS
#endif
#include "sys/queue.h"
#include "hal/adc_types.h"
#include "hal/adc_hal.h"

#define ADC_MAX_MEAS_NUM_DEFAULT      (255)
#define ADC_MEAS_NUM_LIM_DEFAULT      (1)

#define DIG_ADC_OUTPUT_FORMAT_DEFUALT (ADC_DIGI_FORMAT_12BIT)
#define DIG_ADC_ATTEN_DEFUALT         (ADC_ATTEN_DB_11)
#define DIG_ADC_BIT_WIDTH_DEFUALT     (ADC_WIDTH_BIT_12)

#define ADC_CHECK_RET(fun_ret) ({                  \
    if (fun_ret != ESP_OK) {                                \
        ESP_LOGE(ADC_TAG,"%s:%d\n",__FUNCTION__,__LINE__);  \
        return ESP_FAIL;                                    \
    }                                                       \
})

static const char *ADC_TAG = "ADC";

#define ADC_CHECK(a, str, ret_val) ({                                               \
    if (!(a)) {                                                                     \
        ESP_LOGE(ADC_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);   \
        return (ret_val);                                                           \
    }                                                                               \
})

#define ADC_GET_IO_NUM(periph, channel) (adc_channel_io_map[periph][channel])

#define ADC_CHANNEL_CHECK(periph, channel) ADC_CHECK(channel < SOC_ADC_CHANNEL_NUM(periph), "ADC"#periph" channel error", ESP_ERR_INVALID_ARG)

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define ADC_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define ADC_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/

esp_err_t adc_set_i2s_data_source(adc_i2s_source_t src)
{
    ADC_CHECK(src < ADC_I2S_DATA_SRC_MAX, "ADC i2s data source error", ESP_ERR_INVALID_ARG);
    ADC_ENTER_CRITICAL();
    adc_hal_digi_set_data_source(src);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_i2s_mode_init(adc_unit_t adc_unit, adc_channel_t channel)
{
    if (adc_unit & ADC_UNIT_1) {
        ADC_CHECK((SOC_ADC_SUPPORT_DMA_MODE(ADC_NUM_1)), "ADC1 not support DMA for now.", ESP_ERR_INVALID_ARG);
        ADC_CHANNEL_CHECK(ADC_NUM_1, channel);
    }
    if (adc_unit & ADC_UNIT_2) {
        ADC_CHECK((SOC_ADC_SUPPORT_DMA_MODE(ADC_NUM_2)), "ADC2 not support DMA for now.", ESP_ERR_INVALID_ARG);
        ADC_CHANNEL_CHECK(ADC_NUM_2, channel);
    }

    adc_digi_pattern_table_t adc1_pattern[1];
    adc_digi_pattern_table_t adc2_pattern[1];
    adc_digi_config_t dig_cfg = {
        .conv_limit_en = ADC_MEAS_NUM_LIM_DEFAULT,
        .conv_limit_num = ADC_MAX_MEAS_NUM_DEFAULT,
        .format = DIG_ADC_OUTPUT_FORMAT_DEFUALT,
        .conv_mode = (adc_digi_convert_mode_t)adc_unit,
    };

    if (adc_unit & ADC_UNIT_1) {
        adc1_pattern[0].atten = DIG_ADC_ATTEN_DEFUALT;
        adc1_pattern[0].bit_width = DIG_ADC_BIT_WIDTH_DEFUALT;
        adc1_pattern[0].channel = channel;
        dig_cfg.adc1_pattern_len = 1;
        dig_cfg.adc1_pattern = adc1_pattern;
    }
    if (adc_unit & ADC_UNIT_2) {
        adc2_pattern[0].atten = DIG_ADC_ATTEN_DEFUALT;
        adc2_pattern[0].bit_width = DIG_ADC_BIT_WIDTH_DEFUALT;
        adc2_pattern[0].channel = channel;
        dig_cfg.adc2_pattern_len = 1;
        dig_cfg.adc2_pattern = adc2_pattern;
    }
    adc_gpio_init(adc_unit, channel);
    ADC_ENTER_CRITICAL();
    adc_hal_digi_init();
    adc_hal_digi_controller_config(&dig_cfg);
    ADC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t adc_digi_init(void)
{
    ADC_ENTER_CRITICAL();
    adc_hal_digi_init();
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_deinit(void)
{
    adc_power_release();
    ADC_ENTER_CRITICAL();
    adc_hal_digi_deinit();
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_controller_config(const adc_digi_config_t *config)
{
    /* If enable digital controller, adc xpd should always on. */
    adc_power_acquire();
    ADC_ENTER_CRITICAL();
    adc_hal_digi_controller_config(config);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

/*---------------------------------------------------------------
                    RTC controller setting
---------------------------------------------------------------*/

/*---------------------------------------------------------------
                        HALL SENSOR
---------------------------------------------------------------*/

static int hall_sensor_get_value(void)    //hall sensor without LNA
{
    int hall_value;

    adc_power_acquire();

    ADC_ENTER_CRITICAL();
    /* disable other peripherals. */
    adc_hal_amp_disable();
    adc_hal_hall_enable();
    // set controller
    adc_hal_set_controller( ADC_NUM_1, ADC_CTRL_RTC );
    hall_value = adc_hal_hall_convert();
    adc_hal_hall_disable();
    ADC_EXIT_CRITICAL();

    adc_power_release();
    return hall_value;
}

int hall_sensor_read(void)
{
    adc_gpio_init(ADC_UNIT_1, ADC1_CHANNEL_0);
    adc_gpio_init(ADC_UNIT_1, ADC1_CHANNEL_3);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_0);
    return hall_sensor_get_value();
}