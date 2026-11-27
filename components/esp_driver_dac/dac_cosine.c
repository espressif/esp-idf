/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdatomic.h>
#include <string.h>
#include "dac_priv_common.h"
#include "driver/dac_cosine.h"
#include "hal/dac_ll.h"
#include "esp_check.h"
#include "esp_log.h"

typedef enum {
    DAC_COS_FSM_REGISTERED,
    DAC_COS_FSM_ENABLED,
    DAC_COS_FSM_WAIT,
} dac_cosine_fsm_t;

struct dac_cosine_s {
    dac_cosine_config_t       cfg;        /*!< Cosine mode configurations */
    _Atomic dac_cosine_fsm_t  fsm;        /*!< FSM state */
};

static uint32_t s_cosine_freq_hz;
static uint32_t s_cosine_freq_ref_cnt;

/* Due to frequency aliasing, fstep values above the Nyquist limit (half the phase accumulator range) are invalid. */
#define DAC_SINTX_FSTEP_LIMIT  (DAC_LL_SINTX_PHASE_ACC_RANGE / 2)

static __attribute__((const)) dac_sintx_freq_hw_t s_dac_cosine_calc_freq(uint32_t clk_freq_hz, uint32_t cw_freq_hz)
{
#if DAC_LL_SINTX_HAS_TIMER_TARGET
    /**
     * clk_freq_hz is DAC_HS_CLK
     * freq = (hs_clk / max(timer_target, 1)) * (fstep / DAC_LL_SINTX_PHASE_ACC_RANGE)
     *
     * @note Since the upper 8 bits of the phase accumulator are used to index the LUT,
     *       we prefer fstep values no greater than 256 to avoid skipping LUT entries.
     */
    const uint64_t clk = clk_freq_hz, cw = cw_freq_hz;
    dac_sintx_freq_hw_t best = {.timer_target = 1, .fstep = 1};

    /* Return true if config A yields a smaller absolute frequency error than B. */
    bool better_sintx_config(dac_sintx_freq_hw_t a, dac_sintx_freq_hw_t b) {
        uint64_t a_diff = DAC_ABS_DIFF(clk * a.fstep, cw * DAC_LL_SINTX_PHASE_ACC_RANGE * a.timer_target);
        uint64_t b_diff = DAC_ABS_DIFF(clk * b.fstep, cw * DAC_LL_SINTX_PHASE_ACC_RANGE * b.timer_target);
        return a_diff * b.timer_target < b_diff * a.timer_target;
    }

    /**
     * The ideal relationship between `fstep` and `timer_target` is linear, but both parameters are subject to integer quantization.
     * fstep / timer_target = (cw * DAC_LL_SINTX_PHASE_ACC_RANGE) / clk = k
     */
    if (cw * DAC_LL_SINTX_PHASE_ACC_RANGE < clk) {
        // fstep / timer_target = k < 1, we iterate fstep from 1 to 256
        for (uint64_t s = 1; s <= 256; s++) {
            uint32_t tt_floor = DAC_CLAMP(clk * s / (cw * DAC_LL_SINTX_PHASE_ACC_RANGE), 1, DAC_SINTX_TIMER_TARGET_V);
            uint32_t tt_ceil = DAC_CLAMP(tt_floor + 1, 1, DAC_SINTX_TIMER_TARGET_V);
            dac_sintx_freq_hw_t candidate = {.timer_target = tt_floor, .fstep = s};
            if (better_sintx_config(candidate, best)) {
                best = candidate;
            }
            candidate.timer_target = tt_ceil;
            if (better_sintx_config(candidate, best)) {
                best = candidate;
            }
        }
    } else {
        // fstep / timer_target = k >= 1, we iterate timer_target from 1 until fstep > 256
        for (uint64_t tt = 1; ; tt++) {
            uint32_t s = DAC_CLAMP(DAC_DIV_ROUND(cw * tt * DAC_LL_SINTX_PHASE_ACC_RANGE, clk), 1, DAC_SINTX_FSTEP_LIMIT);
            if (tt > 1 && s > 256) {
                break;
            }
            dac_sintx_freq_hw_t candidate = {.timer_target = tt, .fstep = s};
            if (better_sintx_config(candidate, best)) {
                best = candidate;
            }
        }
    }

    return best;
#else
    /* freq = clk * fstep / DAC_LL_SINTX_PHASE_ACC_RANGE */
    uint64_t fstep = DAC_DIV_ROUND((uint64_t)cw_freq_hz * DAC_LL_SINTX_PHASE_ACC_RANGE, clk_freq_hz);
    return (dac_sintx_freq_hw_t) {
        .fstep = DAC_CLAMP(fstep, 1, DAC_SINTX_FSTEP_LIMIT),
    };
#endif
}

esp_err_t dac_cosine_new_channel(const dac_cosine_config_t *cos_cfg, dac_cosine_handle_t *ret_handle)
{
    /* Parameters validation */
    DAC_NULL_POINTER_CHECK(cos_cfg);
    DAC_NULL_POINTER_CHECK(ret_handle);
    ESP_RETURN_ON_FALSE(IS_VALID_DAC_CHANNEL(cos_cfg->chan_id), ESP_ERR_INVALID_ARG, TAG, "invalid dac channel id");
    ESP_RETURN_ON_FALSE(dac_priv_is_valid_channel_config(&cos_cfg->chan_cfg), ESP_ERR_INVALID_ARG, TAG, "invalid DAC channel configuration");
    ESP_RETURN_ON_FALSE(cos_cfg->freq_hz > 0, ESP_ERR_INVALID_ARG, TAG, "invalid cosine wave frequency");
    ESP_RETURN_ON_FALSE(cos_cfg->atten < DAC_COSINE_ATTEN_MAX && (cos_cfg->phase == DAC_COSINE_PHASE_0 || cos_cfg->phase == DAC_COSINE_PHASE_180),
                        ESP_ERR_INVALID_ARG, TAG, "invalid cosine post-processing parameter");

    dac_cosine_clk_src_t clk_src = cos_cfg->clk_src ? : DAC_COSINE_CLK_SRC_DEFAULT;
    ESP_RETURN_ON_FALSE(IS_VALID_DAC_CLK_SRC(clk_src, SOC_DAC_COSINE_CLKS), ESP_ERR_INVALID_ARG, TAG, "invalid DAC cosine clock source");

    esp_err_t ret = ESP_OK;

    /* Allocate cosine handle */
    dac_cosine_handle_t handle = heap_caps_calloc(1, sizeof(struct dac_cosine_s), DAC_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_NO_MEM, TAG, "no memory for the dac cosine handle");
    handle->cfg = *cos_cfg;
    handle->cfg.clk_src = clk_src;
    atomic_store(&handle->fsm, DAC_COS_FSM_REGISTERED);

    /* Acquire the generator clock and resolve its frequency */
    uint32_t clk_freq = 0;
    ESP_GOTO_ON_ERROR(dac_priv_clk_acquire_unified((soc_module_clk_t)clk_src, DAC_LS_CLK_CONSTRAINT_DEFAULT, &clk_freq),
                      err_handle, TAG, "acquire DAC clock failed");

    /* Acquire the analog channel */
    ESP_GOTO_ON_ERROR(dac_priv_channel_register(cos_cfg->chan_id), err_clk, TAG, "register dac channel %d failed", cos_cfg->chan_id);

    /* Claim the shared generator and its frequency configuration. */
    ESP_GOTO_ON_ERROR(dac_priv_sintx_acquire(DAC_SINTX_MODE_TONE), err_chan, TAG, "SINTX is occupied by another mode");

    dac_sintx_freq_hw_t freq_hw = s_dac_cosine_calc_freq(clk_freq, cos_cfg->freq_hz);
    DAC_ENTER_CRITICAL();
    if (s_cosine_freq_ref_cnt == 0) {
        // first user
        s_cosine_freq_ref_cnt = 1;
        s_cosine_freq_hz = cos_cfg->freq_hz;
        dac_priv_sintx_set_freq_unlocked(freq_hw);
    } else if (s_cosine_freq_hz == cos_cfg->freq_hz) {
        // same frequency
        s_cosine_freq_ref_cnt++;
    } else if (cos_cfg->flags.force_set_freq) {
        // force set
        s_cosine_freq_ref_cnt++;
        s_cosine_freq_hz = cos_cfg->freq_hz;
        dac_priv_sintx_set_freq_unlocked(freq_hw);
    } else {
        // conflict, and force_set_freq is not set.
        ret = ESP_ERR_INVALID_STATE;
    }
    DAC_EXIT_CRITICAL();
    ESP_GOTO_ON_ERROR(ret, err_sintx, TAG, "frequency conflict");

    /* Configure the per-channel post-processing pipeline. */
    int32_t offset = cos_cfg->offset;
    if (cos_cfg->phase == DAC_COSINE_PHASE_180) {
        offset = -offset;
    }
#if DAC_LL_SINTX_LUT_SIGNED
    const int16_t base_offset = 0;
    /* Hardware DC register is signed */
    const uint8_t bitwidth = dac_priv_chan_cfg_get_bitwidth(&cos_cfg->chan_cfg);
    const int16_t min_offset = -(1 << (bitwidth - 1)), max_offset = (1 << (bitwidth - 1)) - 1;
#else
    const int16_t full_scale = 1 << dac_priv_chan_cfg_get_bitwidth(&cos_cfg->chan_cfg);
    /**
     * Shift the attenuated waveform to the center line:
     * - 0 dB attenuation: no shift required
     * - 6 dB attenuation: shift by 1/2 − 1/4 = 4/16 full scale
     * - 12 dB attenuation: shift by 1/2 − 1/8 = 6/16 full scale
     * - 18 dB attenuation: shift by 1/2 − 1/16 = 7/16 full scale
     */
    const int16_t base_offset = (full_scale / 16) * (8 - (8 >> cos_cfg->atten));
    /* Hardware DC register is unsigned */
    const int16_t min_offset = 0, max_offset = full_scale - 1;
#endif
    offset += base_offset;
    if (offset < min_offset || offset > max_offset) {
        /* User-facing range: phase 0° → [min-base, max-base]; phase 180° → [base-max, base-min] */
        const int16_t min_user = (cos_cfg->phase == DAC_COSINE_PHASE_0) ? (min_offset - base_offset) : (base_offset - max_offset);
        const int16_t max_user = min_user + (max_offset - min_offset);
        ESP_LOGW(TAG, "DAC cosine DC offset %d out of range [%d, %d], saturating",
                 cos_cfg->offset, min_user, max_user);
        offset = (offset < min_offset) ? min_offset : max_offset;
    }

    DAC_ENTER_CRITICAL();
    dac_ll_cw_set_atten(handle->cfg.chan_id, handle->cfg.atten);
    dac_ll_cw_set_phase(handle->cfg.chan_id, handle->cfg.phase);
    dac_ll_cw_set_offset(handle->cfg.chan_id, offset);
    DAC_EXIT_CRITICAL();

    *ret_handle = handle;
    return ESP_OK;

err_sintx:
    dac_priv_sintx_release();
err_chan:
    dac_priv_channel_deregister(cos_cfg->chan_id);
err_clk:
    dac_priv_clk_release_unified((soc_module_clk_t)clk_src, DAC_LS_CLK_CONSTRAINT_DEFAULT);
err_handle:
    free(handle);
    return ret;
}

esp_err_t dac_cosine_del_channel(dac_cosine_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);

    dac_cosine_fsm_t expected_fsm = DAC_COS_FSM_REGISTERED;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&handle->fsm, &expected_fsm, DAC_COS_FSM_WAIT),
                        ESP_ERR_INVALID_STATE, TAG, "dac cosine is running");

    DAC_ENTER_CRITICAL();
    s_cosine_freq_ref_cnt--;
    DAC_EXIT_CRITICAL();
    ESP_RETURN_ON_ERROR(dac_priv_sintx_release(), TAG, "release dac sintx generator failed");
    ESP_RETURN_ON_ERROR(dac_priv_channel_deregister(handle->cfg.chan_id), TAG,
                        "deregister dac channel %d failed", handle->cfg.chan_id);
    ESP_RETURN_ON_ERROR(dac_priv_clk_release_unified((soc_module_clk_t)handle->cfg.clk_src, DAC_LS_CLK_CONSTRAINT_DEFAULT),
                        TAG, "release DAC clock failed");
    free(handle);

    return ESP_OK;
}

esp_err_t dac_cosine_start(dac_cosine_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);

    dac_cosine_fsm_t expected_fsm = DAC_COS_FSM_REGISTERED;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&handle->fsm, &expected_fsm, DAC_COS_FSM_WAIT),
                        ESP_ERR_INVALID_STATE, TAG, "dac cosine already started");

    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_ERROR(dac_priv_sintx_start(), err, TAG, "start dac sintx generator failed");
    ESP_GOTO_ON_ERROR(dac_priv_channel_enable(handle->cfg.chan_id, handle->cfg.chan_cfg, DAC_DATA_SOURCE_COSINE), err_sintx,
                      TAG, "enable dac channel %d failed", handle->cfg.chan_id);

    atomic_store(&handle->fsm, DAC_COS_FSM_ENABLED);
    return ESP_OK;
err_sintx:
    dac_priv_sintx_stop();
err:
    atomic_store(&handle->fsm, DAC_COS_FSM_REGISTERED);
    return ret;
}

esp_err_t dac_cosine_stop(dac_cosine_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);

    dac_cosine_fsm_t expected_fsm = DAC_COS_FSM_ENABLED;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&handle->fsm, &expected_fsm, DAC_COS_FSM_WAIT),
                        ESP_ERR_INVALID_STATE, TAG, "dac cosine already stopped");

    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_ERROR(dac_priv_channel_disable(handle->cfg.chan_id), err,
                      TAG, "disable dac channel %d failed", handle->cfg.chan_id);
    ESP_GOTO_ON_ERROR(dac_priv_sintx_stop(), err_reenable, TAG, "stop dac sintx generator failed");

    atomic_store(&handle->fsm, DAC_COS_FSM_REGISTERED);
    return ESP_OK;
err_reenable:
    dac_priv_channel_enable(handle->cfg.chan_id, handle->cfg.chan_cfg, DAC_DATA_SOURCE_COSINE);
err:
    atomic_store(&handle->fsm, DAC_COS_FSM_ENABLED);
    return ret;
}

uint8_t dac_cosine_get_bitwidth(dac_cosine_handle_t handle)
{
    if (!handle) {
        return 0;
    }

    return dac_priv_chan_cfg_get_bitwidth(&handle->cfg.chan_cfg);
}
