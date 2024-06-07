/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "soc/soc_pins.h"
#include "hal/touch_sensor_ll.h"
#include "hal/touch_sensor_hal.h"
#include "hal/touch_sensor_types.h"
#include "soc/soc_caps.h"

typedef struct {
    int deep_slp_chan;
    touch_hal_config_t slp_cfg;
    bool apply_slp_cfg;
} touch_hal_deep_sleep_obj_t;

static touch_hal_deep_sleep_obj_t s_touch_slp_obj = {
    .deep_slp_chan = -1,
    .apply_slp_cfg = false,
};

void touch_hal_config_controller(const touch_hal_config_t *cfg)
{
    HAL_ASSERT(cfg);
    touch_ll_sleep_set_channel_num(TOUCH_LL_NULL_CHANNEL);
    touch_ll_set_out_mode(cfg->output_mode);
    touch_ll_set_power_on_wait_cycle(cfg->power_on_wait_ticks);
    touch_ll_set_measure_interval_ticks(cfg->meas_interval_ticks);
    if (cfg->timeout_ticks) {
        touch_ll_timeout_enable(cfg->timeout_ticks);
    } else {
        touch_ll_timeout_disable();
    }

    touch_ll_sample_cfg_set_engaged_num(cfg->sample_cfg_num);
    for (int i = 0; i < cfg->sample_cfg_num; i++) {
        touch_ll_set_clock_div(i, cfg->sample_cfg[i].div_num);
        touch_ll_set_charge_times(i, cfg->sample_cfg[i].charge_times);
        touch_ll_sample_cfg_set_rc_filter(i, cfg->sample_cfg[i].rc_filter_cap, cfg->sample_cfg[i].rc_filter_res);
        touch_ll_sample_cfg_set_driver(i, cfg->sample_cfg[i].low_drv, cfg->sample_cfg[i].high_drv);
        touch_ll_sample_cfg_bypass_shield_output(i, cfg->sample_cfg[i].bypass_shield_output);
        touch_ll_sample_cfg_set_bias_voltage(i, cfg->sample_cfg[i].bias_volt);
    }
}

void touch_hal_save_sleep_config(int deep_slp_chan, const touch_hal_config_t *deep_slp_cfg)
{
    s_touch_slp_obj.deep_slp_chan = deep_slp_chan;
    /* If particular deep sleep configuration is given, save it and apply it before entering the deep sleep */
    if (deep_slp_chan >= 0 && deep_slp_cfg) {
        s_touch_slp_obj.apply_slp_cfg = true;
        memcpy(&s_touch_slp_obj.slp_cfg, deep_slp_cfg, sizeof(touch_hal_config_t));
    } else {
        s_touch_slp_obj.apply_slp_cfg = false;
    }
}

//This function will only be called when the chip is going to deep sleep.
static void s_touch_hal_apply_sleep_config(void)
{
    /* Apply the particular configuration for deep sleep */
    if (s_touch_slp_obj.apply_slp_cfg) {
        touch_hal_config_controller(&s_touch_slp_obj.slp_cfg);
    }
    /* Whether to enable touch sensor wake-up the chip from deep sleep */
    if (s_touch_slp_obj.deep_slp_chan >= 0) {
        touch_ll_sleep_set_channel_num(s_touch_slp_obj.deep_slp_chan);
        touch_ll_set_channel_mask(BIT(s_touch_slp_obj.deep_slp_chan));
    } else {
        touch_ll_sleep_set_channel_num(TOUCH_LL_NULL_CHANNEL);
    }
}

void touch_hal_prepare_deep_sleep(void)
{
    s_touch_hal_apply_sleep_config();
    touch_ll_sleep_reset_benchmark();
    touch_ll_intr_clear(TOUCH_LL_INTR_MASK_ALL);
}
