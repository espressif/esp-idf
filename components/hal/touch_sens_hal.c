/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "soc/soc_caps.h"
#include "hal/touch_sensor_ll.h"
#include "hal/touch_sens_hal.h"
#include "hal/touch_sens_types.h"


typedef struct {
    bool deep_slp_allow_pd;
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
    touch_ll_set_power_on_wait_cycle(cfg->power_on_wait_ticks);
    touch_ll_set_measure_interval_ticks(cfg->meas_interval_ticks);

#if SOC_TOUCH_SENSOR_VERSION == 1
    touch_ll_set_intr_trigger_mode(cfg->intr_trig_mode);
    touch_ll_set_intr_trigger_group(cfg->intr_trig_group);
    touch_ll_set_charge_window_duration(cfg->sample_cfg->charge_duration_ticks);
    touch_ll_set_charge_voltage_high_limit(cfg->sample_cfg->charge_volt_lim_h);
    touch_ll_set_charge_voltage_low_limit(cfg->sample_cfg->charge_volt_lim_l);
#elif SOC_TOUCH_SENSOR_VERSION == 2
    touch_ll_sleep_set_channel_num(TOUCH_LL_NULL_CHANNEL);
    touch_ll_set_timeout(cfg->timeout_ticks);
    touch_ll_set_charge_times(cfg->sample_cfg->charge_times);
    touch_ll_set_charge_voltage_high_limit(cfg->sample_cfg->charge_volt_lim_h);
    touch_ll_set_charge_voltage_low_limit(cfg->sample_cfg->charge_volt_lim_l);
    touch_ll_set_idle_channel_connection(cfg->sample_cfg->idle_conn);
    touch_ll_set_bias_type(cfg->sample_cfg->bias_type);
#elif SOC_TOUCH_SENSOR_VERSION == 3
    touch_ll_sleep_set_channel_num(TOUCH_LL_NULL_CHANNEL);
    touch_ll_set_timeout(cfg->timeout_ticks);
    touch_ll_sample_cfg_set_engaged_num(cfg->sample_cfg_num);
    touch_ll_set_out_mode(cfg->output_mode);
    for (int i = 0; i < cfg->sample_cfg_num; i++) {
        touch_ll_set_clock_div(i, cfg->sample_cfg[i].div_num);
        touch_ll_set_charge_times(i, cfg->sample_cfg[i].charge_times);
        touch_ll_sample_cfg_set_rc_filter(i, cfg->sample_cfg[i].rc_filter_cap, cfg->sample_cfg[i].rc_filter_res);
        touch_ll_sample_cfg_set_driver(i, cfg->sample_cfg[i].low_drv, cfg->sample_cfg[i].high_drv);
        touch_ll_sample_cfg_bypass_shield_output(i, cfg->sample_cfg[i].bypass_shield_output);
        touch_ll_sample_cfg_set_bias_voltage(i, cfg->sample_cfg[i].bias_volt);
    }
#else
    HAL_ASSERT(0);  // Unsupported touch sensor version
#endif
}

void touch_hal_save_sleep_config(int deep_slp_chan, const touch_hal_config_t *deep_slp_cfg, bool dslp_allow_pd)
{
    s_touch_slp_obj.deep_slp_chan = deep_slp_chan;
    s_touch_slp_obj.deep_slp_allow_pd = dslp_allow_pd;
    /* If particular deep sleep configuration is given, save it and apply it before entering the deep sleep */
    if (deep_slp_cfg) {
        s_touch_slp_obj.apply_slp_cfg = true;
        memcpy(&s_touch_slp_obj.slp_cfg, deep_slp_cfg, sizeof(touch_hal_config_t));
    } else {
        s_touch_slp_obj.apply_slp_cfg = false;
    }
}

#if SOC_TOUCH_SENSOR_VERSION == 1
//This function will only be called when the chip is going to deep sleep.
static void s_touch_hal_apply_sleep_config(void)
{
    /* Apply the particular configuration for deep sleep */
    if (s_touch_slp_obj.apply_slp_cfg) {
        /* Save the current channel threshold first, because they will be reset by hardware after the recofniguration */
        uint32_t chan_thresh[SOC_TOUCH_SENSOR_NUM] = {};
        for (int i = 0; i < SOC_TOUCH_SENSOR_NUM; i++) {
            chan_thresh[i] = touch_ll_get_chan_active_threshold(i);
        }
        /* Reconfigure the touch sensor to use the sleep configuration */
        touch_hal_config_controller(&s_touch_slp_obj.slp_cfg);
        /* Restore the channel threshold */
        for (int i = 0; i < SOC_TOUCH_SENSOR_NUM; i++) {
            touch_ll_set_chan_active_threshold(i, chan_thresh[i]);
        }
    }
}
#else
//This function will only be called when the chip is going to deep sleep.
static void s_touch_hal_apply_sleep_config(void)
{
    /* Apply the particular configuration for deep sleep */
    if (s_touch_slp_obj.apply_slp_cfg) {
        touch_hal_config_controller(&s_touch_slp_obj.slp_cfg);
    }
    /* Whether to enable touch sensor wake-up the chip from deep sleep */
    if (s_touch_slp_obj.deep_slp_chan >= 0) {
        if (s_touch_slp_obj.deep_slp_allow_pd) {
            touch_ll_sleep_set_channel_num(s_touch_slp_obj.deep_slp_chan);
        } else {
            /* If not allow power down but sleep channel is set, then only enable this channel during the deep sleep */
            touch_ll_sleep_set_channel_num(TOUCH_LL_NULL_CHANNEL);
            touch_ll_enable_channel_mask(BIT(s_touch_slp_obj.deep_slp_chan));
        }
    } else {
        touch_ll_sleep_set_channel_num(TOUCH_LL_NULL_CHANNEL);
    }
}
#endif

void touch_hal_prepare_deep_sleep(void)
{
    s_touch_hal_apply_sleep_config();
#if SOC_TOUCH_SUPPORT_BENCHMARK
    touch_ll_sleep_reset_benchmark();
#endif
    touch_ll_interrupt_clear(TOUCH_LL_INTR_MASK_ALL);
}
