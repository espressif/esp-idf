// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

// The HAL layer for Touch Sensor (common part)

#include "hal/touch_sensor_hal.h"
#include "hal/touch_sensor_types.h"
#include "soc/soc_caps.h"

void touch_hal_config(touch_pad_t touch_num)
{
    touch_ll_set_threshold(touch_num, SOC_TOUCH_PAD_THRESHOLD_MAX);
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
