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
#include "esp_private/esp_clk_tree_common.h"
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

static bool dac_cosine_clk_src_is_supported(dac_cosine_clk_src_t clk_src)
{
    const dac_cosine_clk_src_t supported[] = SOC_DAC_COSINE_CLKS;
    for (size_t i = 0; i < sizeof(supported) / sizeof(supported[0]); i++) {
        if (clk_src == supported[i]) {
            return true;
        }
    }
    return false;
}

esp_err_t dac_cosine_new_channel(const dac_cosine_config_t *cos_cfg, dac_cosine_handle_t *ret_handle)
{
    /* Parameters validation */
    DAC_NULL_POINTER_CHECK(cos_cfg);
    DAC_NULL_POINTER_CHECK(ret_handle);
    ESP_RETURN_ON_FALSE(IS_VALID_DAC_CHANNEL(cos_cfg->chan_id), ESP_ERR_INVALID_ARG, TAG, "invalid dac channel id");
    ESP_RETURN_ON_FALSE(cos_cfg->freq_hz > 0, ESP_ERR_INVALID_ARG, TAG, "invalid cosine wave frequency");

    dac_cosine_clk_src_t clk_src = cos_cfg->clk_src ? : DAC_COSINE_CLK_SRC_DEFAULT;
    ESP_RETURN_ON_FALSE(dac_cosine_clk_src_is_supported(clk_src), ESP_ERR_INVALID_ARG, TAG, "invalid DAC cosine clock source");

    esp_err_t ret = ESP_OK;

    /* Allocate cosine handle */
    dac_cosine_handle_t handle = heap_caps_calloc(1, sizeof(struct dac_cosine_s), DAC_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_NO_MEM, TAG, "no memory for the dac cosine handle");
    handle->cfg = *cos_cfg;
    handle->cfg.clk_src = clk_src;
    atomic_store(&handle->fsm, DAC_COS_FSM_REGISTERED);

    /* Acquire the generator clock and resolve its frequency */
    uint32_t clk_freq = 0;
    ESP_GOTO_ON_ERROR(esp_clk_tree_enable_src((soc_module_clk_t)handle->cfg.clk_src, true), err_handle, TAG, "enable clock failed");
    ESP_GOTO_ON_ERROR(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)handle->cfg.clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clk_freq),
                      err_clk, TAG, "get clock frequency failed");

    ESP_GOTO_ON_ERROR(dac_priv_channel_register(cos_cfg->chan_id), err_clk, TAG, "register dac channel %d failed", cos_cfg->chan_id);

    /* Claim the shared cosine wave generator in tone mode and program the wave frequency (this also
     * starts the generator). Rejected if the direct-output path is in use or a different frequency
     * is already set. */
    ESP_GOTO_ON_ERROR(dac_priv_sintx_acquire_tone(cos_cfg->freq_hz, clk_freq, cos_cfg->flags.force_set_freq), err_dereg, TAG, "acquire cosine wave generator failed");

    int16_t offset = cos_cfg->offset;
    if (cos_cfg->phase == DAC_COSINE_PHASE_180) {
        offset = -offset;
    }
#if SOC_DAC_SINTX_LUT_SIGNED
#error "not implemented"
#else
    const int16_t base_offset = 0;
    /* Hardware DC register is signed: -128~127 */
    const int16_t min_offset = -128, max_offset = 127;
#endif
    if (offset < min_offset || offset > max_offset) {
        /* User-facing range: phase 0° → [min-B, max-B]; phase 180° → [B-max, B-min] */
        const int16_t min_user = (cos_cfg->phase == DAC_COSINE_PHASE_0) ? (min_offset - base_offset) : (base_offset - max_offset);
        const int16_t max_user = min_user + (max_offset - min_offset);
        ESP_LOGW(TAG, "DAC cosine DC offset %d out of range [%d, %d], saturating",
                 cos_cfg->offset, min_user, max_user);
        offset = (offset < min_offset) ? min_offset : max_offset;
    }

    /* Set the per-channel cosine wave parameters. */
    DAC_ENTER_CRITICAL();
    dac_ll_cw_set_atten(handle->cfg.chan_id, handle->cfg.atten);
    dac_ll_cw_set_phase(handle->cfg.chan_id, handle->cfg.phase);
    dac_ll_cw_set_offset(handle->cfg.chan_id, offset);
    DAC_EXIT_CRITICAL();

    *ret_handle = handle;
    return ESP_OK;

err_dereg:
    dac_priv_channel_deregister(cos_cfg->chan_id);
err_clk:
    esp_clk_tree_enable_src((soc_module_clk_t)handle->cfg.clk_src, false);
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

    ESP_RETURN_ON_ERROR(dac_priv_channel_deregister(handle->cfg.chan_id), TAG,
                        "deregister dac channel %d failed", handle->cfg.chan_id);
    ESP_RETURN_ON_ERROR(dac_priv_sintx_release(), TAG, "release dac sintx generator failed");
    ESP_RETURN_ON_ERROR(esp_clk_tree_enable_src((soc_module_clk_t)handle->cfg.clk_src, false), TAG, "disable clock failed");
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
    /* The generator is already running; starting only powers on the pad so the wave reaches the output. */
    ESP_GOTO_ON_ERROR(dac_priv_channel_enable(handle->cfg.chan_id, DAC_DATA_SOURCE_COSINE), err,
                      TAG, "enable dac channel %d failed", handle->cfg.chan_id);

    atomic_store(&handle->fsm, DAC_COS_FSM_ENABLED);
    return ESP_OK;
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
    /* Power off the pad; the generator keeps running */
    ESP_GOTO_ON_ERROR(dac_priv_channel_disable(handle->cfg.chan_id), err,
                      TAG, "disable dac channel %d failed", handle->cfg.chan_id);

    atomic_store(&handle->fsm, DAC_COS_FSM_REGISTERED);
    return ESP_OK;
err:
    atomic_store(&handle->fsm, DAC_COS_FSM_ENABLED);
    return ret;
}

uint8_t dac_cosine_get_bitwidth(dac_cosine_handle_t handle)
{
    if (!handle) {
        return 0;
    }

    return SOC_DAC_RESOLUTION;
}
