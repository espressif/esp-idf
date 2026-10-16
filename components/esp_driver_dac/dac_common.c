/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <stdatomic.h>
#include <stdbool.h>

#include "dac_priv_common.h"
#include "freertos/FreeRTOS.h"
#include "hal/dac_periph.h"
#include "hal/dac_types.h"
#include "hal/dac_ll.h"
#if DAC_LL_SHARED_HS_LS_CLK
#include "esp_clk_tree.h"
#include "esp_private/esp_clk_tree_common.h"
#endif
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
/* Global dac spin lock for the whole DAC driver */
portMUX_TYPE dac_spinlock = portMUX_INITIALIZER_UNLOCKED;

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

esp_err_t dac_priv_channel_enable(dac_channel_t chan_id, dac_channel_config_t chan_cfg, dac_data_source_t source)
{
    ESP_RETURN_ON_FALSE(IS_VALID_DAC_CHANNEL(chan_id), ESP_ERR_INVALID_ARG, TAG, "channel id is invalid");

#if SOC_DAC_HS_RESOLUTION && SOC_DAC_SUPPORT_BUFFER_CFG
    const bool high_speed = chan_cfg.output_mode == DAC_CHANNEL_OUTPUT_CURRENT;
    const bool buffered = !high_speed && (chan_cfg.buffer_mode != DAC_CHANNEL_BUFFER_BYPASS);
    /* Reduced voltage swing supports the full input code range without clipping at the ends. */
    const bool full_code_range = buffered && (chan_cfg.buffer_mode == DAC_CHANNEL_BUFFER_REDUCED_SWING);
#else
    (void)chan_cfg;  // ignore chan_cfg
#endif

    dac_channel_fsm_t expected_fsm = DAC_CHAN_FSM_REGISTERED;
    if (atomic_compare_exchange_strong(&s_dac_chan_fsm[chan_id], &expected_fsm, DAC_CHAN_FSM_WAIT)) {
        gpio_num_t gpio_num = (gpio_num_t)dac_periph_signal.dac_channel_io_num[chan_id];
        gpio_config_as_analog(gpio_num);
        DAC_ENTER_CRITICAL();
#if SOC_DAC_HS_RESOLUTION && SOC_DAC_SUPPORT_BUFFER_CFG
        dac_ll_pad_enable_high_speed(chan_id, high_speed);
        dac_ll_pad_enable_buffer(chan_id, buffered);
        dac_ll_pad_set_buffer_code_range(chan_id, full_code_range);
#endif
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

#if DAC_LL_SHARED_HS_LS_CLK
/*---------------------------------------------------------------
                  DAC HS/LS clock management
---------------------------------------------------------------*/
/**
 * Frequency relationships:
 * DAC_HS_CLK = clock source / hs_div
 * DAC_LS_CLK = DAC_HS_CLK / ls_div
 */

/* Limits must be in descending order of frequency by constraint enum value:
 * s_dac_clk_get_ls_limit_unlocked() uses the last active constraint as the strictest limit.
 */
static const uint32_t s_dac_ls_clk_max_freq_hz[DAC_LS_CLK_CONSTRAINT_COUNT] = {
    [DAC_LS_CLK_CONSTRAINT_DEFAULT]     = 1000000,  // 1MHz
    [DAC_LS_CLK_CONSTRAINT_CALIBRATION] = 500000,   // 500kHz
    [DAC_LS_CLK_CONSTRAINT_SAMPLE_HOLD] = 50000,    // 50kHz
};

typedef struct {
    soc_module_clk_t source;
    uint32_t source_freq_hz;
    uint32_t hs_div;
    uint32_t ls_div;
    uint32_t ref_cnt;   // ref_cnt = sum of constraint_ref_cnt
    uint32_t constraint_ref_cnt[DAC_LS_CLK_CONSTRAINT_COUNT];
} dac_clk_state_t;

static dac_clk_state_t s_dac_clk;

/* The caller must hold the DAC spinlock. */
static inline uint32_t s_dac_clk_get_ls_limit_unlocked(void)
{
    uint32_t limit = s_dac_ls_clk_max_freq_hz[DAC_LS_CLK_CONSTRAINT_DEFAULT];
    for (int i = 0; i < DAC_LS_CLK_CONSTRAINT_COUNT; i++) {
        if (s_dac_clk.constraint_ref_cnt[i]) {
            limit = s_dac_ls_clk_max_freq_hz[i];
        }
    }
    return limit;
}

esp_err_t dac_priv_clk_acquire(soc_module_clk_t source, dac_ls_clk_constraint_t constraint)
{
    ESP_RETURN_ON_FALSE(IS_VALID_DAC_CLK_SRC(source, SOC_DAC_DIGI_CLKS), ESP_ERR_INVALID_ARG, TAG, "invalid DAC clock source");
    ESP_RETURN_ON_FALSE((unsigned)constraint < DAC_LS_CLK_CONSTRAINT_COUNT, ESP_ERR_INVALID_ARG, TAG, "invalid DAC LS clock constraint");

    ESP_RETURN_ON_ERROR(esp_clk_tree_enable_src(source, true), TAG, "acquire DAC clock source failed");
    esp_err_t ret = ESP_OK;
    uint32_t source_freq_hz = 0;
    ESP_GOTO_ON_ERROR(esp_clk_tree_src_get_freq_hz(source, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &source_freq_hz),
                      err_source, TAG, "get DAC clock source frequency failed");

    bool keep_source_ref = false;
    DAC_ENTER_CRITICAL();
    if (s_dac_clk.ref_cnt == 0) {
        // first user
        uint32_t ls_limit = s_dac_ls_clk_max_freq_hz[constraint];
        uint32_t hs_div = 1;
        uint32_t ls_div = DAC_DIV_CEIL(source_freq_hz, ls_limit);
        if (ls_div < DAC_LL_CLK_LS_DIV_MIN || ls_div > DAC_LL_CLK_LS_DIV_MAX ||
                hs_div < DAC_LL_CLK_HS_DIV_MIN || hs_div > DAC_LL_CLK_HS_DIV_MAX) {
            ret = ESP_ERR_NOT_SUPPORTED;
        } else {
            dac_ll_clk_set_source(source);
            dac_ll_clk_set_hs_div(hs_div);
            dac_ll_clk_enable(true);
            // Note: Access to the register containing ls_div requires the HS clock.
            dac_ll_clk_set_ls_div(ls_div);
            s_dac_clk.source = source;
            s_dac_clk.source_freq_hz = source_freq_hz;
            s_dac_clk.hs_div = hs_div;
            s_dac_clk.ls_div = ls_div;
            s_dac_clk.ref_cnt++;
            s_dac_clk.constraint_ref_cnt[constraint]++;
            keep_source_ref = true;
        }
    } else if (s_dac_clk.source != source) {
        // conflict
        ret = ESP_ERR_INVALID_STATE;
    } else {
        // existing users share the same clock source
        uint32_t limit = s_dac_ls_clk_max_freq_hz[constraint];
        if (limit < (s_dac_clk.source_freq_hz / s_dac_clk.hs_div / s_dac_clk.ls_div)) {
            // stricter constraints
            uint32_t ls_div = DAC_DIV_CEIL(s_dac_clk.source_freq_hz, limit);
            if (ls_div < DAC_LL_CLK_LS_DIV_MIN || ls_div > DAC_LL_CLK_LS_DIV_MAX) {
                ret = ESP_ERR_NOT_SUPPORTED;
            } else {
                dac_ll_clk_set_ls_div(ls_div);
                s_dac_clk.ls_div = ls_div;
                s_dac_clk.ref_cnt++;
                s_dac_clk.constraint_ref_cnt[constraint]++;
            }
        } else {
            s_dac_clk.ref_cnt++;
            s_dac_clk.constraint_ref_cnt[constraint]++;
        }
    }
    DAC_EXIT_CRITICAL();

    /* The manager retains only the first user's upstream-clock reference. */
    if (!keep_source_ref) {
err_source:
        esp_clk_tree_enable_src(source, false);
    }
    return ret;
}

esp_err_t dac_priv_clk_get_hs_freq_hz(uint32_t *freq_hz)
{
    esp_err_t ret = ESP_OK;
    DAC_ENTER_CRITICAL();
    if (s_dac_clk.ref_cnt) {
        if (freq_hz) {
            *freq_hz = DAC_DIV_ROUND(s_dac_clk.source_freq_hz, s_dac_clk.hs_div);
        }
    } else {
        ret = ESP_ERR_INVALID_STATE;
    }
    DAC_EXIT_CRITICAL();
    return ret;
}

esp_err_t dac_priv_clk_get_ls_freq_hz(uint32_t *freq_hz)
{
    esp_err_t ret = ESP_OK;
    DAC_ENTER_CRITICAL();
    if (s_dac_clk.ref_cnt) {
        if (freq_hz) {
            *freq_hz = DAC_DIV_ROUND(s_dac_clk.source_freq_hz, s_dac_clk.hs_div * s_dac_clk.ls_div);
        }
    } else {
        ret = ESP_ERR_INVALID_STATE;
    }
    DAC_EXIT_CRITICAL();
    return ret;
}

esp_err_t dac_priv_clk_release(dac_ls_clk_constraint_t constraint)
{
    ESP_RETURN_ON_FALSE((unsigned)constraint < DAC_LS_CLK_CONSTRAINT_COUNT, ESP_ERR_INVALID_ARG, TAG, "invalid DAC LS clock constraint");

    esp_err_t ret = ESP_OK;
    soc_module_clk_t to_release = SOC_MOD_CLK_INVALID;
    DAC_ENTER_CRITICAL();
    if (s_dac_clk.constraint_ref_cnt[constraint] == 0) {
        ret = ESP_ERR_INVALID_STATE;
    } else {
        s_dac_clk.constraint_ref_cnt[constraint]--;
        if (--s_dac_clk.ref_cnt == 0) {
            dac_ll_clk_enable(false);
            to_release = s_dac_clk.source;
            s_dac_clk = (dac_clk_state_t) {};
        } else {
            uint32_t limit = s_dac_clk_get_ls_limit_unlocked();
            uint32_t ls_div = DAC_DIV_CEIL(s_dac_clk.source_freq_hz, limit);
            if (s_dac_clk.ls_div != ls_div) {
                dac_ll_clk_set_ls_div(ls_div);
                s_dac_clk.ls_div = ls_div;
            }
        }
    }
    DAC_EXIT_CRITICAL();

    if (to_release != SOC_MOD_CLK_INVALID) {
        ESP_RETURN_ON_ERROR(esp_clk_tree_enable_src(to_release, false), TAG, "release DAC clock source failed");
    }
    return ret;
}

#endif // DAC_LL_SHARED_HS_LS_CLK

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

typedef struct {
    dac_sintx_mode_t mode;
    uint32_t declared_ref_cnt;
    uint32_t running_ref_cnt;
} dac_sintx_state_t;

static dac_sintx_state_t s_dac_sintx_state;

esp_err_t dac_priv_sintx_acquire(dac_sintx_mode_t mode)
{
    esp_err_t ret = ESP_OK;
    DAC_ENTER_CRITICAL();
    if (s_dac_sintx_state.declared_ref_cnt == 0) {
        // We are the first user. Declare the generator mode without starting it.
        s_dac_sintx_state.declared_ref_cnt = 1;
        s_dac_sintx_state.mode = mode;
    } else if (s_dac_sintx_state.mode != mode) {
        // The generator is busy in other mode. Conflict.
        ret = ESP_ERR_INVALID_STATE;
    } else {
        s_dac_sintx_state.declared_ref_cnt++;
    }
    DAC_EXIT_CRITICAL();
    return ret;
}

esp_err_t dac_priv_sintx_start(void)
{
    esp_err_t ret = ESP_OK;
    DAC_ENTER_CRITICAL();
    if (s_dac_sintx_state.declared_ref_cnt > s_dac_sintx_state.running_ref_cnt) {
        if (s_dac_sintx_state.running_ref_cnt == 0) {
#if DAC_LL_DC_VIA_SINTX
            if (s_dac_sintx_state.mode == DAC_SINTX_MODE_DC) {
                dac_ll_cw_enable_dc();
            } else
#endif
            {
                dac_ll_cw_enable_tone();
            }
        }
        s_dac_sintx_state.running_ref_cnt++;
    } else {
        ret = ESP_ERR_INVALID_STATE;
    }
    DAC_EXIT_CRITICAL();
    return ret;
}

esp_err_t dac_priv_sintx_stop(void)
{
    esp_err_t ret = ESP_OK;
    DAC_ENTER_CRITICAL();
    if (s_dac_sintx_state.running_ref_cnt == 0) {
        ret = ESP_ERR_INVALID_STATE;
    } else if (--s_dac_sintx_state.running_ref_cnt == 0) {
        dac_ll_cw_disable();
    }
    DAC_EXIT_CRITICAL();
    return ret;
}

esp_err_t dac_priv_sintx_release(void)
{
    esp_err_t ret = ESP_OK;
    DAC_ENTER_CRITICAL();
    if (s_dac_sintx_state.declared_ref_cnt > s_dac_sintx_state.running_ref_cnt) {
        s_dac_sintx_state.declared_ref_cnt--;
    } else {
        ret = ESP_ERR_INVALID_STATE;
    }
    DAC_EXIT_CRITICAL();
    return ret;
}

void dac_priv_sintx_set_freq_unlocked(dac_sintx_freq_hw_t freq_hw)
{
#if DAC_LL_SINTX_HAS_TIMER_TARGET
    dac_ll_cw_set_timer_target(freq_hw.timer_target);
#endif
    dac_ll_cw_set_fstep(freq_hw.fstep);
}

#if CONFIG_DAC_ENABLE_DEBUG_LOG
__attribute__((constructor))
static void dac_override_default_log_level(void)
{
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
}
#endif
