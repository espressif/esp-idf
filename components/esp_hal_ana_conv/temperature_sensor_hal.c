/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/temperature_sensor_hal.h"
#include "hal/temperature_sensor_ll.h"
#include "hal/temperature_sensor_periph.h"
#include "hal/log.h"
#include "esp_rom_sys.h"

__attribute__((unused)) static const char *TAG_TSENS = "temperature_sensor_hal";

#define INT_NOT_USED 999999

// HAL layer maintains its own copies of these variables to avoid changing existing logic
static int s_hal_record_min = INT_NOT_USED;
static int s_hal_record_max = INT_NOT_USED;
static uint8_t s_hal_tsens_idx = 2; // Index for temperature attribute, set 2(middle) as default value

int temperature_sensor_hal_get_raw_value(void)
{
    int raw_value = temperature_sensor_ll_get_raw_value();
    return (TEMPERATURE_SENSOR_LL_ADC_FACTOR_INT * raw_value - TEMPERATURE_SENSOR_LL_DAC_FACTOR_INT * temperature_sensor_attributes[s_hal_tsens_idx].offset - TEMPERATURE_SENSOR_LL_OFFSET_FACTOR_INT) / TEMPERATURE_SENSOR_LL_DENOMINATOR;
}

int16_t temperature_sensor_hal_get_degree(bool *range_changed)
{
    int degree = temperature_sensor_hal_get_raw_value();
    uint8_t temperature_dac;

    // 1. Check whether temperature value is in range
    if (s_hal_record_min != INT_NOT_USED && degree >= s_hal_record_min && degree <= s_hal_record_max) {
        // If degree is in range, not needed to do any check to save time. Otherwise, choose proper range and record.
        if (range_changed != NULL) {
            *range_changed = false;
        }
        return degree;
    }

    // 2. If temperature value is not in range, adjust to proper range
    if (degree >= temperature_sensor_attributes[1].range_max) {
        s_hal_tsens_idx = 0;
    } else if (degree >= temperature_sensor_attributes[2].range_max && degree < temperature_sensor_attributes[1].range_max) {
        s_hal_tsens_idx = 1;
    } else if (degree <= temperature_sensor_attributes[2].range_min && degree > temperature_sensor_attributes[3].range_min) {
        s_hal_tsens_idx = 3;
    } else if (degree <= temperature_sensor_attributes[3].range_min) {
        s_hal_tsens_idx = 4;
    } else {
        s_hal_tsens_idx = 2;
    }
    HAL_EARLY_LOGD(TAG_TSENS, "range changed, change to index %d", s_hal_tsens_idx);
    temperature_dac = temperature_sensor_attributes[s_hal_tsens_idx].reg_val;
    s_hal_record_min = temperature_sensor_attributes[s_hal_tsens_idx].range_min;
    s_hal_record_max = temperature_sensor_attributes[s_hal_tsens_idx].range_max;

    temperature_sensor_ll_set_range(temperature_dac);

    // 3. Then, read value again
    // Before reading the temperature value, ticks need to be delayed, otherwise a wrong value will be returned.
    // As what has been recommended and tested, 300us is a good interval to get the correct value after adjust range.
    esp_rom_delay_us(300);
    degree = temperature_sensor_hal_get_raw_value();
    if (range_changed != NULL) {
        *range_changed = true;
    }

    return degree;
}

void temperature_sensor_hal_init(void)
{
    // Set the range as recorded.
    temperature_sensor_ll_set_range(temperature_sensor_attributes[s_hal_tsens_idx].reg_val);
}

void temperature_sensor_hal_sync_tsens_idx(uint8_t tsens_idx)
{
    s_hal_tsens_idx = tsens_idx;
}
