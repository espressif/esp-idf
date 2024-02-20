/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for Touch Sensor (common part)

#include "hal/touch_sensor_hal.h"
#include "hal/touch_sensor_types.h"
#include "soc/soc_caps.h"

void touch_hal_config(touch_pad_t touch_num)
{
    touch_ll_set_threshold(touch_num, TOUCH_PAD_THRESHOLD_MAX);
    touch_ll_set_slope(touch_num, TOUCH_PAD_SLOPE_DEFAULT);
    touch_ll_set_tie_option(touch_num, TOUCH_PAD_TIE_OPT_DEFAULT);
}

void touch_hal_set_voltage(const touch_hal_volt_t *volt)
{
    touch_ll_set_voltage_high(volt->refh);
    touch_ll_set_voltage_low(volt->refl);
    touch_ll_set_voltage_attenuation(volt->atten);
}

void touch_hal_get_voltage(touch_hal_volt_t *volt)
{
    touch_ll_get_voltage_high(&volt->refh);
    touch_ll_get_voltage_low(&volt->refl);
    touch_ll_get_voltage_attenuation(&volt->atten);
}

void touch_hal_set_meas_mode(touch_pad_t touch_num, const touch_hal_meas_mode_t *meas)
{
    touch_ll_set_slope(touch_num, meas->slope);
    touch_ll_set_tie_option(touch_num, meas->tie_opt);
}

void touch_hal_get_meas_mode(touch_pad_t touch_num, touch_hal_meas_mode_t *meas)
{
    touch_ll_get_slope(touch_num, &meas->slope);
    touch_ll_get_tie_option(touch_num, &meas->tie_opt);
}
