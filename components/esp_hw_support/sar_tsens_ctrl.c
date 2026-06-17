/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "soc/soc_caps.h"
#include "freertos/FreeRTOS.h"
#include "esp_private/sar_periph_ctrl.h"
#include "esp_log.h"
#include "esp_timer.h"

#if SOC_TEMP_SENSOR_SUPPORTED
#include "hal/temperature_sensor_ll.h"
#include "hal/temperature_sensor_hal.h"
#include "soc/periph_defs.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/adc_share_hw_ctrl.h"
#include "esp_private/critical_section.h"

extern __attribute__((unused)) portMUX_TYPE rtc_spinlock;


/*------------------------------------------------------------------------------------------------------------
-----------------------------------------Temperature Sensor---------------------------------------------------
------------------------------------------------------------------------------------------------------------*/
ESP_LOG_ATTR_TAG(TAG_TSENS, "temperature_sensor");

#define INT_NOT_USED 999999

#define TSENS_LINE_REGRESSION_US (200)

static int s_temperature_sensor_power_cnt;
static bool s_first_temp_read = false;
static int64_t timer1 = 0;

void temperature_sensor_power_acquire(void)
{
    esp_os_enter_critical(&rtc_spinlock);
    s_temperature_sensor_power_cnt++;
    if (s_temperature_sensor_power_cnt == 1) {
        s_first_temp_read = true;
        regi2c_saradc_enable();
#if !SOC_TSENS_IS_INDEPENDENT_FROM_ADC
        adc_apb_periph_claim();
#endif
        PERIPH_RCC_ATOMIC() {
            temperature_sensor_ll_bus_clk_enable(true);
            temperature_sensor_ll_reset_module();
        }
        temperature_sensor_ll_enable(true);
        // Initialize HAL layer with the same tsens_idx
        temperature_sensor_hal_init();
        timer1 = esp_timer_get_time();
    }
    esp_os_exit_critical(&rtc_spinlock);
}

void temperature_sensor_power_release(void)
{
    esp_os_enter_critical(&rtc_spinlock);
    s_temperature_sensor_power_cnt--;
    /* Sanity check */
    if (s_temperature_sensor_power_cnt < 0) {
        esp_os_exit_critical(&rtc_spinlock);
        ESP_LOGE(TAG_TSENS, "%s called, but s_temperature_sensor_power_cnt == 0", __func__);
        abort();
    } else if (s_temperature_sensor_power_cnt == 0) {
        temperature_sensor_ll_enable(false);
        PERIPH_RCC_ATOMIC() {
            temperature_sensor_ll_bus_clk_enable(false);
        }
#if !SOC_TSENS_IS_INDEPENDENT_FROM_ADC
        adc_apb_periph_free();
#endif
        regi2c_saradc_disable();
    }
    esp_os_exit_critical(&rtc_spinlock);
}

int16_t temp_sensor_get_raw_value(bool *range_changed)
{
    int16_t result;

    esp_os_enter_critical(&rtc_spinlock);

    // When this is the first time reading a value, check whether the time here minus the
    // initialization time is greater than 200 microseconds (the time for linear regression).
    // If it is less than 200 microseconds, continue waiting here.
    if (s_first_temp_read == true) {
        int64_t timer2 = esp_timer_get_time();
        int64_t diff = timer2 - timer1;
        if (diff < TSENS_LINE_REGRESSION_US) {
            esp_rom_delay_us(TSENS_LINE_REGRESSION_US - diff);
        }
        s_first_temp_read = false;
    }

    adc_reset_lock_acquire();
    result = temperature_sensor_hal_get_degree(range_changed);
    adc_reset_lock_release();

    esp_os_exit_critical(&rtc_spinlock);

    return result;
}

#endif
