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

void touch_hal_init(void)
{
    touch_ll_intr_disable(TOUCH_PAD_INTR_ALL);
    touch_ll_clear_channel_mask(SOC_TOUCH_SENSOR_BIT_MASK_MAX);
    touch_ll_clear_trigger_status_mask();
    touch_ll_set_meas_times(TOUCH_PAD_MEASURE_CYCLE_DEFAULT);
    touch_ll_set_sleep_time(TOUCH_PAD_SLEEP_CYCLE_DEFAULT);
    touch_ll_set_voltage_high(TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD);
    touch_ll_set_voltage_low(TOUCH_PAD_LOW_VOLTAGE_THRESHOLD);
    touch_ll_set_voltage_attenuation(TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD);
    touch_ll_set_inactive_connect(TOUCH_PAD_INACTIVE_CONNECT_DEFAULT);
}

void touch_hal_deinit(void)
{
    touch_hal_stop_fsm();
    touch_hal_clear_trigger_status_mask();
    touch_ll_intr_disable(TOUCH_PAD_INTR_ALL);
}

void touch_hal_filter_set_config(const touch_filter_config_t *filter_info)
{
    touch_ll_filter_set_filter_mode(filter_info->mode);
    touch_ll_filter_set_debounce(filter_info->debounce_cnt);
    touch_ll_filter_set_hysteresis(filter_info->hysteresis_thr);
    touch_ll_filter_set_noise_thres(filter_info->noise_thr);
    touch_ll_filter_set_neg_noise_thres(filter_info->noise_neg_thr);
    touch_ll_filter_set_baseline_reset(filter_info->neg_noise_limit);
    touch_ll_filter_set_jitter_step(filter_info->jitter_step);
}

void touch_hal_filter_get_config(touch_filter_config_t *filter_info)
{
    touch_ll_filter_get_filter_mode(&filter_info->mode);
    touch_ll_filter_get_debounce(&filter_info->debounce_cnt);
    touch_ll_filter_get_hysteresis(&filter_info->hysteresis_thr);
    touch_ll_filter_get_noise_thres(&filter_info->noise_thr);
    touch_ll_filter_get_neg_noise_thres(&filter_info->noise_neg_thr);
    touch_ll_filter_get_baseline_reset(&filter_info->neg_noise_limit);
    touch_ll_filter_get_jitter_step(&filter_info->jitter_step);
}

void touch_hal_denoise_set_config(const touch_pad_denoise_t *denoise)
{
    touch_ll_denoise_set_cap_level(denoise->cap_level);
    touch_ll_denoise_set_grade(denoise->grade);
}

void touch_hal_denoise_get_config(touch_pad_denoise_t *denoise)
{
    touch_ll_denoise_get_cap_level(&denoise->cap_level);
    touch_ll_denoise_get_grade(&denoise->grade);
}

void touch_hal_denoise_enable(void)
{
    touch_ll_clear_channel_mask(1U << SOC_TOUCH_DENOISE_CHANNEL);
    touch_ll_denoise_enable();
}

void touch_hal_waterproof_set_config(const touch_pad_waterproof_t *waterproof)
{
    touch_ll_waterproof_set_guard_pad(waterproof->guard_ring_pad);
    touch_ll_waterproof_set_sheild_driver(waterproof->shield_driver);
}

void touch_hal_waterproof_get_config(touch_pad_waterproof_t *waterproof)
{
    touch_ll_waterproof_get_guard_pad(&waterproof->guard_ring_pad);
    touch_ll_waterproof_get_sheild_driver(&waterproof->shield_driver);
}

void touch_hal_waterproof_enable(void)
{
    touch_ll_clear_channel_mask(1U << SOC_TOUCH_SHIELD_CHANNEL);
    touch_ll_waterproof_enable();
}

void touch_hal_proximity_set_config(const touch_pad_proximity_t *proximity)
{
    touch_ll_proximity_set_channel_num(proximity->select_pad);
    touch_ll_proximity_set_meas_times(proximity->meas_num);
}

void touch_hal_proximity_get_config(touch_pad_proximity_t *proximity)
{
    touch_ll_proximity_get_channel_num(proximity->select_pad);
    touch_ll_proximity_get_meas_times(&proximity->meas_num);
}

void touch_hal_sleep_channel_config(const touch_pad_sleep_channel_t *slp_config)
{
    touch_ll_sleep_set_channel_num(slp_config->touch_num);
    touch_ll_sleep_set_threshold(slp_config->sleep_pad_threshold);
    if (slp_config->en_proximity) {
        touch_ll_sleep_enable_approach();
    } else {
        touch_ll_sleep_disable_approach();
    }
}
