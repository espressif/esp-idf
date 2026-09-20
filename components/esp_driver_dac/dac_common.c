/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <stdatomic.h>

#include "dac_priv_common.h"
#include "freertos/FreeRTOS.h"
#include "hal/dac_periph.h"
#include "hal/dac_types.h"
#include "hal/dac_ll.h"
#include "esp_private/gpio.h"
#include "esp_check.h"
#include "esp_log.h"

#if ! (SOC_IS(ESP32) || SOC_IS(ESP32S2))
portMUX_TYPE dac_priv_spinlock = portMUX_INITIALIZER_UNLOCKED;
#endif // ! (SOC_IS(ESP32) || SOC_IS(ESP32S2))

/*---------------------------------------------------------------
                  Channel (analog pad) management
---------------------------------------------------------------*/

typedef enum {
    DAC_CHAN_FSM_IDLE,
    DAC_CHAN_FSM_REGISTERED,
    DAC_CHAN_FSM_ENABLED,
    DAC_CHAN_FSM_WAIT,        // transition state
} dac_channel_fsm_t;

static _Atomic dac_channel_fsm_t s_dac_chan_fsm[SOC_DAC_CHAN_NUM] = {
    [0 ... SOC_DAC_CHAN_NUM - 1] = DAC_CHAN_FSM_IDLE,
};

esp_err_t dac_priv_channel_register(dac_channel_t chan_id)
{
    ESP_RETURN_ON_FALSE(IS_VALID_DAC_CHANNEL(chan_id), ESP_ERR_INVALID_ARG, TAG, "channel id is invalid");

    dac_channel_fsm_t expected_fsm = DAC_CHAN_FSM_IDLE;
    if (atomic_compare_exchange_strong(&s_dac_chan_fsm[chan_id], &expected_fsm, DAC_CHAN_FSM_REGISTERED)) {
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "dac channel %d has been registered", chan_id);
        return ESP_ERR_INVALID_STATE;
    }
}

esp_err_t dac_priv_channel_deregister(dac_channel_t chan_id)
{
    ESP_RETURN_ON_FALSE(IS_VALID_DAC_CHANNEL(chan_id), ESP_ERR_INVALID_ARG, TAG, "channel id is invalid");

    dac_channel_fsm_t expected_fsm = DAC_CHAN_FSM_REGISTERED;
    if (atomic_compare_exchange_strong(&s_dac_chan_fsm[chan_id], &expected_fsm, DAC_CHAN_FSM_IDLE)) {
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "dac channel %d is still enabled or not registered", chan_id);
        return ESP_ERR_INVALID_STATE;
    }
}

esp_err_t dac_priv_channel_enable(dac_channel_t chan_id, dac_data_source_t source)
{
    ESP_RETURN_ON_FALSE(IS_VALID_DAC_CHANNEL(chan_id), ESP_ERR_INVALID_ARG, TAG, "channel id is invalid");

    dac_channel_fsm_t expected_fsm = DAC_CHAN_FSM_REGISTERED;
    if (atomic_compare_exchange_strong(&s_dac_chan_fsm[chan_id], &expected_fsm, DAC_CHAN_FSM_WAIT)) {
        gpio_num_t gpio_num = (gpio_num_t)dac_periph_signal.dac_channel_io_num[chan_id];
        gpio_config_as_analog(gpio_num);
        DAC_ENTER_CRITICAL();
        dac_ll_pad_set_data_source(chan_id, source);
        dac_ll_sync_by_adc(false);
        dac_ll_pad_power_on(chan_id);
        DAC_EXIT_CRITICAL();
        atomic_store(&s_dac_chan_fsm[chan_id], DAC_CHAN_FSM_ENABLED);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "dac channel %d is already enabled or not registered", chan_id);
        return ESP_ERR_INVALID_STATE;
    }
}

esp_err_t dac_priv_channel_disable(dac_channel_t chan_id)
{
    ESP_RETURN_ON_FALSE(IS_VALID_DAC_CHANNEL(chan_id), ESP_ERR_INVALID_ARG, TAG, "channel id is invalid");

    dac_channel_fsm_t expected_fsm = DAC_CHAN_FSM_ENABLED;
    if (atomic_compare_exchange_strong(&s_dac_chan_fsm[chan_id], &expected_fsm, DAC_CHAN_FSM_WAIT)) {
        DAC_ENTER_CRITICAL();
        dac_ll_pad_power_down(chan_id);
        DAC_EXIT_CRITICAL();
        atomic_store(&s_dac_chan_fsm[chan_id], DAC_CHAN_FSM_REGISTERED);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "dac channel %d is not enabled", chan_id);
        return ESP_ERR_INVALID_STATE;
    }
}

/*---------------------------------------------------------------
                Cosine (Sintx) generator management
---------------------------------------------------------------*/
/**
 * The cosine wave generator is a single shared resource: every channel that outputs a cosine wave
 * uses the same generator, and therefore the same frequency. On some targets, the same Sintx generator
 * also serves the direct (DC) software output. The tone and DC modes are mutually exclusive there.
 * This manager reference counts the generator, enforces the mode exclusion and the single shared
 * frequency.
 */

typedef enum {
    DAC_SINTX_MODE_TONE = 0,    /* cosine wave output */
#if SOC_DAC_DC_VIA_SINTX
    DAC_SINTX_MODE_DC,          /* direct (DC) software output */
#endif
} dac_sintx_mode_t;

typedef struct {
    dac_sintx_mode_t mode;
    uint32_t ref_cnt;
    uint32_t freq;
} dac_sintx_state_t;

static dac_sintx_state_t s_dac_sintx_state;

typedef struct {
#if SOC_DAC_SINTX_HAS_TIMER_TARGET
    uint32_t timer_target;
#endif
    uint32_t fstep;
} dac_sintx_freq_hw_t;

/* Due to frequency aliasing, fstep values above the Nyquist limit (2^16)/2, are effectively invalid */
#define DAC_SINTX_FSTEP_MAX     0x8000

/**
 * @brief Compute Sintx hardware frequency parameters from the requested tone frequency.
 */
static __attribute__((const)) dac_sintx_freq_hw_t dac_priv_sintx_calc_freq(uint32_t freq_hz, uint32_t clk_freq_hz)
{
#if SOC_DAC_SINTX_HAS_TIMER_TARGET
#error "Not implemented"
#else
    /* freq = clk * fstep / 2^16 */
    uint64_t fstep = DAC_DIV_ROUND((uint64_t)freq_hz << 16, clk_freq_hz);
    return (dac_sintx_freq_hw_t) {
        .fstep = DAC_CLAMP(fstep, 1, DAC_SINTX_FSTEP_MAX),
    };
#endif
}

static inline void dac_priv_sintx_apply_freq(dac_sintx_freq_hw_t hw)
{
#if SOC_DAC_SINTX_HAS_TIMER_TARGET
    dac_ll_cw_set_timer_target(hw.timer_target);
#endif
    dac_ll_cw_set_fstep(hw.fstep);
}

esp_err_t dac_priv_sintx_acquire_tone(uint32_t freq_hz, uint32_t clk_freq_hz, bool force_set_freq)
{
    dac_sintx_freq_hw_t hw_freq = dac_priv_sintx_calc_freq(freq_hz, clk_freq_hz);

    esp_err_t ret = ESP_OK;
    DAC_ENTER_CRITICAL();
    if (s_dac_sintx_state.ref_cnt == 0) {
        // We are the first user. Lock the generator in tone mode and start the generator.
        s_dac_sintx_state.ref_cnt = 1;
        s_dac_sintx_state.mode = DAC_SINTX_MODE_TONE;
        s_dac_sintx_state.freq = freq_hz;

        dac_priv_sintx_apply_freq(hw_freq);
        dac_ll_cw_enable_tone();
    } else if (s_dac_sintx_state.mode != DAC_SINTX_MODE_TONE) {
        // The generator is busy in other mode. Conflict.
        ret = ESP_ERR_INVALID_STATE;
    } else if (s_dac_sintx_state.freq == freq_hz) {
        // Other users already requested the same frequency. Just ride it.
        s_dac_sintx_state.ref_cnt++;
    } else if (force_set_freq) {
        // Re-program the frequency.
        s_dac_sintx_state.ref_cnt++;
        s_dac_sintx_state.freq = freq_hz;

        dac_ll_cw_disable();
        dac_priv_sintx_apply_freq(hw_freq);
        dac_ll_cw_enable_tone();
    } else {
        // Other users already requested a different frequency. Conflict.
        ret = ESP_ERR_INVALID_STATE;
    }
    DAC_EXIT_CRITICAL();
    return ret;
}

#if SOC_DAC_DC_VIA_SINTX
esp_err_t dac_priv_sintx_acquire_dc(void)
{
    esp_err_t ret = ESP_OK;
    DAC_ENTER_CRITICAL();
    if (s_dac_sintx_state.ref_cnt == 0) {
        // We are the first user. Lock the generator in DC mode and start the generator.
        s_dac_sintx_state.ref_cnt = 1;
        s_dac_sintx_state.mode = DAC_SINTX_MODE_DC;
        dac_ll_cw_enable_dc();
    } else if (s_dac_sintx_state.mode == DAC_SINTX_MODE_DC) {
        // Other users already requested the DC mode. Just ride it.
        s_dac_sintx_state.ref_cnt++;
    } else {
        // Other users already requested the cosine mode. Conflict.
        ret = ESP_ERR_INVALID_STATE;
    }
    DAC_EXIT_CRITICAL();
    return ret;
}
#endif // SOC_DAC_DC_VIA_SINTX

esp_err_t dac_priv_sintx_release(void)
{
    esp_err_t ret = ESP_OK;
    DAC_ENTER_CRITICAL();
    if (s_dac_sintx_state.ref_cnt > 0) {
        s_dac_sintx_state.ref_cnt--;
        if (s_dac_sintx_state.ref_cnt == 0) {
            // We are the last user. Stop the generator.
            dac_ll_cw_disable();
        }
    } else {
        ret = ESP_ERR_INVALID_STATE;
    }
    DAC_EXIT_CRITICAL();
    return ret;
}

#if CONFIG_DAC_ENABLE_DEBUG_LOG
__attribute__((constructor))
static void dac_override_default_log_level(void)
{
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
}
#endif
