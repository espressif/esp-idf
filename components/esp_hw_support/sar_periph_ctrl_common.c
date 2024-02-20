/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "soc/soc_caps.h"
#include "freertos/FreeRTOS.h"
#include "esp_private/sar_periph_ctrl.h"
#include "esp_log.h"

#if SOC_TEMP_SENSOR_SUPPORTED
#include "hal/temperature_sensor_ll.h"
#include "soc/temperature_sensor_periph.h"
#include "soc/periph_defs.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/adc_share_hw_ctrl.h"

extern __attribute__((unused)) portMUX_TYPE rtc_spinlock;


/*------------------------------------------------------------------------------------------------------------
-----------------------------------------Temperature Sensor---------------------------------------------------
------------------------------------------------------------------------------------------------------------*/
static const char *TAG_TSENS = "temperature_sensor";

#define INT_NOT_USED 999999

#if !SOC_RCC_IS_INDEPENDENT
#define TSENS_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define TSENS_RCC_ATOMIC()
#endif

static int s_record_min = INT_NOT_USED;
static int s_record_max = INT_NOT_USED;
static int s_temperature_sensor_power_cnt;

static uint8_t s_tsens_idx = 2; // Index for temperature attribute, set 2(middle) as default value

void temperature_sensor_power_acquire(void)
{
    portENTER_CRITICAL(&rtc_spinlock);
    s_temperature_sensor_power_cnt++;
    if (s_temperature_sensor_power_cnt == 1) {
        regi2c_saradc_enable();
#if !SOC_TSENS_IS_INDEPENDENT_FROM_ADC
        adc_apb_periph_claim();
#endif
        TSENS_RCC_ATOMIC() {
            temperature_sensor_ll_bus_clk_enable(true);
            temperature_sensor_ll_reset_module();
        }
        temperature_sensor_ll_enable(true);
    }
    portEXIT_CRITICAL(&rtc_spinlock);
}

void temperature_sensor_power_release(void)
{
    portENTER_CRITICAL(&rtc_spinlock);
    s_temperature_sensor_power_cnt--;
    /* Sanity check */
    if (s_temperature_sensor_power_cnt < 0) {
        portEXIT_CRITICAL(&rtc_spinlock);
        ESP_LOGE(TAG_TSENS, "%s called, but s_temperature_sensor_power_cnt == 0", __func__);
        abort();
    } else if (s_temperature_sensor_power_cnt == 0) {
        temperature_sensor_ll_enable(false);
        TSENS_RCC_ATOMIC() {
            temperature_sensor_ll_bus_clk_enable(false);
        }
#if !SOC_TSENS_IS_INDEPENDENT_FROM_ADC
        adc_apb_periph_free();
#endif
        regi2c_saradc_disable();
    }
    portEXIT_CRITICAL(&rtc_spinlock);
}

static int temperature_sensor_get_raw_value(void)
{
    int raw_value  = temperature_sensor_ll_get_raw_value();
    return (TEMPERATURE_SENSOR_LL_ADC_FACTOR * raw_value - TEMPERATURE_SENSOR_LL_DAC_FACTOR * temperature_sensor_attributes[s_tsens_idx].offset - TEMPERATURE_SENSOR_LL_OFFSET_FACTOR);
}

void temp_sensor_sync_tsens_idx(int tsens_idx)
{
    s_tsens_idx = tsens_idx;
}

int16_t temp_sensor_get_raw_value(bool *range_changed)
{
    portENTER_CRITICAL(&rtc_spinlock);

    int degree = temperature_sensor_get_raw_value();
    uint8_t temperature_dac;

    // 1. Check whether temperature value is in range
    if (s_record_min != INT_NOT_USED && degree >= s_record_min && degree <= s_record_max) {
        // If degree is in range, not needed to do any check to save time. Otherwise, choose proper range and record.
        if (range_changed != NULL) {
            *range_changed = false;
        }
        portEXIT_CRITICAL(&rtc_spinlock);
        return degree;
    }

    // 2. If temperature value is not in range, adjust to proper range
    if (degree >= temperature_sensor_attributes[1].range_max) {
        s_tsens_idx = 0;
    } else if (degree >= temperature_sensor_attributes[2].range_max && degree < temperature_sensor_attributes[1].range_max) {
        s_tsens_idx = 1;
    } else if (degree <= temperature_sensor_attributes[2].range_min && degree > temperature_sensor_attributes[3].range_min) {
        s_tsens_idx = 3;
    } else if (degree <= temperature_sensor_attributes[3].range_min) {
        s_tsens_idx = 4;
    } else {
        s_tsens_idx = 2;
    }
    ESP_EARLY_LOGD(TAG_TSENS, "range changed, change to index %d", s_tsens_idx);
    temperature_dac = temperature_sensor_attributes[s_tsens_idx].reg_val;
    s_record_min = temperature_sensor_attributes[s_tsens_idx].range_min;
    s_record_max = temperature_sensor_attributes[s_tsens_idx].range_max;

    temperature_sensor_ll_set_range(temperature_dac);

    // 3. Then, read value again
    // Before reading the temperature value, ticks need to be delayed, otherwise a wrong value will be returned.
    // As what has been recommended and tested, 300us is a good interval to get the correct value after adjust range.
    esp_rom_delay_us(300);
    degree = temperature_sensor_get_raw_value();
    if (range_changed != NULL) {
        *range_changed = true;
    }

    portEXIT_CRITICAL(&rtc_spinlock);
    return degree;
}

#endif
