/*
 * SPDX-FileCopyrightText: 2016-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <ctype.h>
#include "sdkconfig.h"
#include "esp_types.h"
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

#ifndef NDEBUG
// Enable built-in checks in queue.h in debug builds
#define INVARIANTS
#endif
#include "sys/queue.h"
#include "hal/adc_types.h"
#include "hal/adc_hal.h"


#define ADC_GET_IO_NUM(periph, channel) (adc_channel_io_map[periph][channel])


extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define ADC_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define ADC_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)


/*---------------------------------------------------------------
                        HALL SENSOR
---------------------------------------------------------------*/

static int hall_sensor_get_value(void)    //hall sensor without LNA
{
    int hall_value;

    adc_power_acquire();

    ADC_ENTER_CRITICAL();
    /* disable other peripherals. */
    adc_ll_amp_disable();
    adc_ll_hall_enable();
    // set controller
    adc_ll_set_controller( ADC_NUM_1, ADC_LL_CTRL_RTC );
    hall_value = adc_hal_hall_convert();
    adc_ll_hall_disable();
    ADC_EXIT_CRITICAL();

    adc_power_release();
    return hall_value;
}

/**
 * To Be Deprecated
 */
extern esp_err_t adc_common_gpio_init(adc_unit_t adc_unit, adc_channel_t channel);
int hall_sensor_read(void)
{
    adc_common_gpio_init(ADC_UNIT_1, ADC1_CHANNEL_0);
    adc_common_gpio_init(ADC_UNIT_1, ADC1_CHANNEL_3);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_0);
    return hall_sensor_get_value();
}
