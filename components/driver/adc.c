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

#include <esp_types.h>
#include <stdlib.h>
#include <ctype.h>
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include "soc/rtc.h"
#include "rtc_io.h"
#include "adc.h"
#include "dac.h"
#include "sys/lock.h"
#include "driver/gpio.h"
#include "adc1_i2s_private.h"

#include "hal/adc_types.h"
#include "hal/adc_hal.h"

#define ADC_MAX_MEAS_NUM_DEFAULT      (255)
#define ADC_MEAS_NUM_LIM_DEFAULT      (1)
#define SAR_ADC_CLK_DIV_DEFUALT       (2)

#define DIG_ADC_OUTPUT_FORMAT_DEFUALT (ADC_DIG_FORMAT_12BIT)
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

/*
In ADC2, there're two locks used for different cases:
1. lock shared with app and WIFI:
   when wifi using the ADC2, we assume it will never stop,
   so app checks the lock and returns immediately if failed.

2. lock shared between tasks:
   when several tasks sharing the ADC2, we want to guarantee
   all the requests will be handled.
   Since conversions are short (about 31us), app returns the lock very soon,
   we use a spinlock to stand there waiting to do conversions one by one.

adc2_spinlock should be acquired first, then adc2_wifi_lock or rtc_spinlock.
*/
//prevent ADC2 being used by wifi and other tasks at the same time.
static _lock_t adc2_wifi_lock;
//prevent ADC2 being used by tasks (regardless of WIFI)
static portMUX_TYPE adc2_spinlock = portMUX_INITIALIZER_UNLOCKED;

//prevent ADC1 being used by I2S dma and other tasks at the same time.
static _lock_t adc1_i2s_lock;
/*---------------------------------------------------------------
                    ADC Common
---------------------------------------------------------------*/

void adc_power_always_on(void)
{
    ADC_ENTER_CRITICAL();
    adc_hal_set_power_manage(ADC_POWER_SW_ON);
    ADC_EXIT_CRITICAL();
}

void adc_power_on(void)
{
    ADC_ENTER_CRITICAL();
    /* The power FSM controlled mode saves more power, while the ADC noise may get increased. */
#ifndef CONFIG_ADC_FORCE_XPD_FSM
    /* Set the power always on to increase precision. */
    adc_hal_set_power_manage(ADC_POWER_SW_ON);
#else
    /* Use the FSM to turn off the power while not used to save power. */
    if (adc_hal_get_power_manage() != ADC_POWER_BY_FSM) {
        adc_hal_set_power_manage(ADC_POWER_SW_ON);
    }
#endif
    ADC_EXIT_CRITICAL();
}

void adc_power_off(void)
{
    ADC_ENTER_CRITICAL();
    adc_hal_set_power_manage(ADC_POWER_SW_OFF);
    ADC_EXIT_CRITICAL();
}

esp_err_t adc_set_clk_div(uint8_t clk_div)
{
    ADC_ENTER_CRITICAL();
    adc_hal_set_clk_div(clk_div);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_set_i2s_data_source(adc_i2s_source_t src)
{
    ADC_CHECK(src < ADC_I2S_DATA_SRC_MAX, "ADC i2s data source error", ESP_ERR_INVALID_ARG);
    ADC_ENTER_CRITICAL();
    adc_hal_dig_set_data_source(src);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_gpio_init(adc_unit_t adc_unit, adc_channel_t channel)
{
    gpio_num_t gpio_num = 0;
    if (adc_unit & ADC_UNIT_1) {
        ADC_CHANNEL_CHECK(ADC_NUM_1, channel);
        gpio_num = ADC_GET_IO_NUM(ADC_NUM_1, channel);

        ADC_CHECK_RET(rtc_gpio_init(gpio_num));
        ADC_CHECK_RET(rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED));
        ADC_CHECK_RET(gpio_set_pull_mode(gpio_num, GPIO_FLOATING));
    }
    if (adc_unit & ADC_UNIT_2) {
        ADC_CHANNEL_CHECK(ADC_NUM_2, channel);
        gpio_num = ADC_GET_IO_NUM(ADC_NUM_2, channel);
        ADC_CHECK_RET(rtc_gpio_init(gpio_num));
        ADC_CHECK_RET(rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED));
        ADC_CHECK_RET(gpio_set_pull_mode(gpio_num, GPIO_FLOATING));
    }
    return ESP_OK;
}

esp_err_t adc_set_data_inv(adc_unit_t adc_unit, bool inv_en)
{
    ADC_ENTER_CRITICAL();
    if (adc_unit & ADC_UNIT_1) {
        adc_hal_output_invert(ADC_NUM_1, inv_en);
    }
    if (adc_unit & ADC_UNIT_2) {
        adc_hal_output_invert(ADC_NUM_1, inv_en);
    }
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_set_data_width(adc_unit_t adc_unit, adc_bits_width_t bits)
{
    ADC_CHECK(bits < ADC_WIDTH_MAX, "ADC bit width error", ESP_ERR_INVALID_ARG);
    ADC_ENTER_CRITICAL();
    if (adc_unit & ADC_UNIT_1) {
        adc_hal_rtc_set_output_format(ADC_NUM_1, bits);
    }
    if (adc_unit & ADC_UNIT_2) {
        adc_hal_rtc_set_output_format(ADC_NUM_2, bits);
        adc_hal_pwdet_set_cct(SOC_ADC_PWDET_CCT_DEFAULT);
    }
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

/* this function should be called in the critical section. */
static int adc_convert(adc_ll_num_t adc_n, int channel)
{
    return adc_hal_convert(adc_n, channel);
}

/*-------------------------------------------------------------------------------------
 *                      ADC I2S
 *------------------------------------------------------------------------------------*/
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

    adc_ll_pattern_table_t adc1_pattern[1];
    adc_ll_pattern_table_t adc2_pattern[1];
    adc_hal_dig_config_t dig_cfg = {
        .conv_limit_en = ADC_MEAS_NUM_LIM_DEFAULT,
        .conv_limit_num = ADC_MAX_MEAS_NUM_DEFAULT,
        .clk_div = SAR_ADC_CLK_DIV_DEFUALT,
        .format = DIG_ADC_OUTPUT_FORMAT_DEFUALT,
        .conv_mode = (adc_ll_convert_mode_t)adc_unit,
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

    ADC_ENTER_CRITICAL();
    adc_hal_init();
    adc_hal_dig_controller_config(&dig_cfg);
    ADC_EXIT_CRITICAL();

    return ESP_OK;
}

/*-------------------------------------------------------------------------------------
 *                      ADC1
 *------------------------------------------------------------------------------------*/
esp_err_t adc1_pad_get_io_num(adc1_channel_t channel, gpio_num_t *gpio_num)
{
    ADC_CHANNEL_CHECK(ADC_NUM_1, channel);
    int io = ADC_GET_IO_NUM(ADC_NUM_1, channel);
    if (io < 0) {
        return ESP_ERR_INVALID_ARG;
    } else {
        *gpio_num = (gpio_num_t)io;
    }

    return ESP_OK;
}

esp_err_t adc1_config_channel_atten(adc1_channel_t channel, adc_atten_t atten)
{
    ADC_CHANNEL_CHECK(ADC_NUM_1, channel);
    ADC_CHECK(atten < ADC_ATTEN_MAX, "ADC Atten Err", ESP_ERR_INVALID_ARG);
    adc_gpio_init(ADC_UNIT_1, channel);
    adc_hal_set_atten(ADC_NUM_1, channel, atten);
    return ESP_OK;
}

esp_err_t adc1_config_width(adc_bits_width_t width_bit)
{
    ADC_CHECK(width_bit < ADC_WIDTH_MAX, "ADC bit width error", ESP_ERR_INVALID_ARG);
    adc_hal_rtc_set_output_format(ADC_NUM_1, width_bit);
    adc_hal_output_invert(ADC_NUM_1, true);
    return ESP_OK;
}

esp_err_t adc1_i2s_mode_acquire(void)
{
    /* Use locks to avoid digtal and RTC controller conflicts.
       for adc1, block until acquire the lock. */
    _lock_acquire( &adc1_i2s_lock );
    ESP_LOGD( ADC_TAG, "i2s mode takes adc1 lock." );
    ADC_ENTER_CRITICAL();
    adc_hal_set_power_manage(ADC_POWER_SW_ON);
    /* switch SARADC into DIG channel */
    adc_hal_set_controller(ADC_NUM_1, ADC_CTRL_DIG);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc1_adc_mode_acquire(void)
{
    /* Use locks to avoid digtal and RTC controller conflicts.
       for adc1, block until acquire the lock. */
    _lock_acquire( &adc1_i2s_lock );
    ADC_ENTER_CRITICAL();
    /* switch SARADC into RTC channel. */
    adc_hal_set_controller(ADC_NUM_1, ADC_CTRL_RTC);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc1_lock_release(void)
{
    ADC_CHECK((uint32_t *)adc1_i2s_lock != NULL, "adc1 lock release called before acquire", ESP_ERR_INVALID_STATE );
    /* Use locks to avoid digtal and RTC controller conflicts.
       for adc1, block until acquire the lock. */
    _lock_release( &adc1_i2s_lock );
    return ESP_OK;
}

int adc1_get_raw(adc1_channel_t channel)
{
    uint16_t adc_value;
    ADC_CHANNEL_CHECK(ADC_NUM_1, channel);
    adc1_adc_mode_acquire();

    adc_power_on();
    ADC_ENTER_CRITICAL();
    /* disable other peripherals. */
    adc_hal_hall_disable();
    /* currently the LNA is not open, close it by default. */
    adc_hal_amp_disable();
    /* set controller */
    adc_hal_set_controller(ADC_NUM_1, ADC_CTRL_RTC);
    /* start conversion */
    adc_value = adc_convert(ADC_NUM_1, channel);
    ADC_EXIT_CRITICAL();

    adc1_lock_release();
    return adc_value;
}

int adc1_get_voltage(adc1_channel_t channel)    //Deprecated. Use adc1_get_raw() instead
{
    return adc1_get_raw(channel);
}

void adc1_ulp_enable(void)
{
    adc_power_on();

    ADC_ENTER_CRITICAL();
    adc_hal_set_controller(ADC_NUM_1, ADC_CTRL_ULP);
    /* since most users do not need LNA and HALL with uLP, we disable them here
       open them in the uLP if needed. */
    /* disable other peripherals. */
    adc_hal_hall_disable();
    adc_hal_amp_disable();
    ADC_EXIT_CRITICAL();
}

/*---------------------------------------------------------------
                    ADC2
---------------------------------------------------------------*/
esp_err_t adc2_pad_get_io_num(adc2_channel_t channel, gpio_num_t *gpio_num)
{
    ADC_CHANNEL_CHECK(ADC_NUM_2, channel);
    int io = ADC_GET_IO_NUM(ADC_NUM_2, channel);
    if (io < 0) {
        return ESP_ERR_INVALID_ARG;
    } else {
        *gpio_num = (gpio_num_t)io;
    }

    return ESP_OK;
}

esp_err_t adc2_wifi_acquire(void)
{
    /* Wi-Fi module will use adc2. Use locks to avoid conflicts. */
    _lock_acquire( &adc2_wifi_lock );
    ESP_LOGD( ADC_TAG, "Wi-Fi takes adc2 lock." );
    return ESP_OK;
}

esp_err_t adc2_wifi_release(void)
{
    ADC_CHECK((uint32_t *)adc2_wifi_lock != NULL, "wifi release called before acquire", ESP_ERR_INVALID_STATE );

    _lock_release( &adc2_wifi_lock );
    ESP_LOGD( ADC_TAG, "Wi-Fi returns adc2 lock." );
    return ESP_OK;
}

static esp_err_t adc2_pad_init(adc2_channel_t channel)
{
    gpio_num_t gpio_num = 0;
    ADC_CHECK_RET(adc2_pad_get_io_num(channel, &gpio_num));
    ADC_CHECK_RET(rtc_gpio_init(gpio_num));
    ADC_CHECK_RET(rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED));
    ADC_CHECK_RET(gpio_set_pull_mode(gpio_num, GPIO_FLOATING));
    return ESP_OK;
}

esp_err_t adc2_config_channel_atten(adc2_channel_t channel, adc_atten_t atten)
{
    ADC_CHANNEL_CHECK(ADC_NUM_2, channel);
    ADC_CHECK(atten <= ADC_ATTEN_11db, "ADC2 Atten Err", ESP_ERR_INVALID_ARG);

    adc2_pad_init(channel);
    portENTER_CRITICAL( &adc2_spinlock );

    //lazy initialization
    //avoid collision with other tasks
    if ( _lock_try_acquire( &adc2_wifi_lock ) == -1 ) {
        //try the lock, return if failed (wifi using).
        portEXIT_CRITICAL( &adc2_spinlock );
        return ESP_ERR_TIMEOUT;
    }
    adc_hal_set_atten(ADC_NUM_2, channel, atten);
    _lock_release( &adc2_wifi_lock );

    portEXIT_CRITICAL( &adc2_spinlock );
    return ESP_OK;
}

static inline void adc2_config_width(adc_bits_width_t width_bit)
{
    ADC_ENTER_CRITICAL();
    adc_hal_rtc_set_output_format(ADC_NUM_2, width_bit);
    adc_hal_pwdet_set_cct(SOC_ADC_PWDET_CCT_DEFAULT);
    adc_hal_output_invert(ADC_NUM_2, true);
    ADC_EXIT_CRITICAL();
}

static inline void adc2_dac_disable( adc2_channel_t channel)
{
    if ( channel == ADC2_CHANNEL_8 ) { // the same as DAC channel 1
        dac_output_disable(DAC_CHANNEL_1);
    } else if ( channel == ADC2_CHANNEL_9 ) {
        dac_output_disable(DAC_CHANNEL_2);
    }
}

//registers in critical section with adc1:
//SENS_SAR_START_FORCE_REG,
esp_err_t adc2_get_raw(adc2_channel_t channel, adc_bits_width_t width_bit, int *raw_out)
{
    uint16_t adc_value = 0;
    ADC_CHECK(channel < ADC2_CHANNEL_MAX, "ADC Channel Err", ESP_ERR_INVALID_ARG);

    //in critical section with whole rtc module
    adc_power_on();

    //avoid collision with other tasks
    portENTER_CRITICAL(&adc2_spinlock);
    //lazy initialization
    //try the lock, return if failed (wifi using).
    if ( _lock_try_acquire( &adc2_wifi_lock ) == -1 ) {
        portEXIT_CRITICAL( &adc2_spinlock );
        return ESP_ERR_TIMEOUT;
    }

    //disable other peripherals
#ifdef CONFIG_ADC_DISABLE_DAC
    adc2_dac_disable(channel);
#endif
    // set controller
    // in critical section with whole rtc module
    // because the PWDET use the same registers, place it here.
    adc2_config_width(width_bit);
    adc_hal_set_controller(ADC_NUM_2, ADC_CTRL_RTC);
    //start converting
    adc_value = adc_convert(ADC_NUM_2, channel);
    _lock_release( &adc2_wifi_lock );
    portEXIT_CRITICAL(&adc2_spinlock);

    *raw_out = (int)adc_value;
    return ESP_OK;
}

esp_err_t adc2_vref_to_gpio(gpio_num_t gpio)
{
    adc_power_always_on();               //Select power source of ADC
    if (adc_hal_vref_output(gpio) != true) {
        return ESP_ERR_INVALID_ARG;
    } else {
        //Configure RTC gpio
        rtc_gpio_init(gpio);
        rtc_gpio_set_direction(gpio, RTC_GPIO_MODE_DISABLED);
        rtc_gpio_pullup_dis(gpio);
        rtc_gpio_pulldown_dis(gpio);
        return ESP_OK;
    }
}

/*---------------------------------------------------------------
                        HALL SENSOR
---------------------------------------------------------------*/

static int hall_sensor_get_value(void)    //hall sensor without LNA
{
    int hall_value;

    adc_power_on();

    ADC_ENTER_CRITICAL();
    /* disable other peripherals. */
    adc_hal_amp_disable();
    adc_hal_hall_enable();
    // set controller
    adc_hal_set_controller( ADC_NUM_1, ADC_CTRL_RTC );
    hall_value = adc_hal_hall_convert();
    ADC_EXIT_CRITICAL();

    return hall_value;
}

int hall_sensor_read(void)
{
    adc_gpio_init(ADC_NUM_1, ADC1_CHANNEL_0);
    adc_gpio_init(ADC_NUM_1, ADC1_CHANNEL_3);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_0);
    return hall_sensor_get_value();
}
