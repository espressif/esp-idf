/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "dac_priv_common.h"
#include "driver/dac_cosine.h"
#include "hal/clk_tree_ll.h"
#include "hal/dac_ll.h"
#include "esp_clk_tree.h"
#include "esp_check.h"
#include "esp_log.h"

struct dac_cosine_s {
    dac_cosine_config_t     cfg;        /*!< Cosine mode configurations */
    bool                    is_started; /*!< Flag: is the channel started(not cosine wave generator) */
};

/* Cosine wave generator reference count
 * The cosine wave generator is shared by dac channels */
static uint32_t s_cwg_refer_cnt = 0;

/* The frequency of cosine wave generator */
static uint32_t s_cwg_freq = 0;

esp_err_t dac_cosine_new_channel(const dac_cosine_config_t *cos_cfg, dac_cosine_handle_t *ret_handle)
{
    /* Parameters validation */
    DAC_NULL_POINTER_CHECK(cos_cfg);
    DAC_NULL_POINTER_CHECK(ret_handle);
    ESP_RETURN_ON_FALSE(IS_VALID_DAC_CHANNEL(cos_cfg->chan_id), ESP_ERR_INVALID_ARG, TAG, "invalid dac channel id");
    ESP_RETURN_ON_FALSE(cos_cfg->freq_hz >= (130 / clk_ll_rc_fast_get_divider()), ESP_ERR_NOT_SUPPORTED, TAG, "The cosine wave frequency is too low");
    ESP_RETURN_ON_FALSE((!s_cwg_freq) || cos_cfg->flags.force_set_freq || (cos_cfg->freq_hz == s_cwg_freq),
                        ESP_ERR_INVALID_STATE, TAG, "The cosine wave frequency has set already, not allowed to update unless `force_set_freq` is set");

    esp_err_t ret = ESP_OK;
    /* Allocate cosine handle */
    dac_cosine_handle_t handle = heap_caps_calloc(1, sizeof(struct dac_cosine_s), DAC_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_NO_MEM, TAG, "no memory for the dac cosine handle");
    /* Assign configurations */
    handle->cfg = *cos_cfg;
    if (handle->cfg.clk_src == 0) {
        handle->cfg.clk_src = DAC_COSINE_CLK_SRC_DEFAULT;
    }
    /* Register the handle */
    ESP_GOTO_ON_ERROR(dac_priv_register_channel(handle->cfg.chan_id), err1, TAG, "register dac channel %d failed", handle->cfg.chan_id);

    /* Get the cosine wave generator clock frequency */
    uint32_t rtc_clk_freq = 0;
    esp_clk_tree_src_get_freq_hz((soc_module_clk_t)handle->cfg.clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &rtc_clk_freq);

    if (rtc_clk_freq == 0) {
        ESP_LOGW(TAG, "RTC clock calibration failed, using the approximate value as default");
        rtc_clk_freq = SOC_CLK_RC_FAST_FREQ_APPROX;
    }
    DAC_ENTER_CRITICAL();
    /* Set coefficients for cosine wave generator */
    if ((!s_cwg_freq) || handle->cfg.flags.force_set_freq) {
        dac_ll_cw_set_freq(handle->cfg.freq_hz, rtc_clk_freq);
        s_cwg_freq = handle->cfg.freq_hz;
    }
    dac_ll_cw_set_atten(handle->cfg.chan_id, handle->cfg.atten);
    dac_ll_cw_set_phase(handle->cfg.chan_id, handle->cfg.phase);
    dac_ll_cw_set_dc_offset(handle->cfg.chan_id, handle->cfg.offset);
    DAC_EXIT_CRITICAL();

    *ret_handle = handle;
    return ret;

err1:
    free(handle);
    return ret;
}

esp_err_t dac_cosine_del_channel(dac_cosine_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(!handle->is_started, ESP_ERR_INVALID_STATE, TAG,
                        "the dac cosine generator is not stopped yet");

    ESP_RETURN_ON_ERROR(dac_priv_deregister_channel(handle->cfg.chan_id), TAG,
                        "deregister dac channel %d failed", handle->cfg.chan_id);
    /* Clear the frequency if no channel using it */
    if (!s_cwg_refer_cnt) {
        s_cwg_freq = 0;
    }
    free(handle);

    return ESP_OK;
}

esp_err_t dac_cosine_start(dac_cosine_handle_t handle)
{
    esp_err_t ret = ESP_OK;
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(!handle->is_started, ESP_ERR_INVALID_STATE, TAG,
                        "the dac channel has already started");
    /* Acquire the cosine wave generator clock */
    ESP_RETURN_ON_ERROR(esp_clk_tree_enable_src((soc_module_clk_t)handle->cfg.clk_src, true), TAG, "cosine clock enable failed");
    /* Enabled DAC channel */
    ESP_GOTO_ON_ERROR(dac_priv_enable_channel(handle->cfg.chan_id), err, TAG,
                      "enable dac channel %d failed", handle->cfg.chan_id);
    /* Enabled the cosine wave generator if no channel using it before */
    DAC_ENTER_CRITICAL();
    if (s_cwg_refer_cnt == 0) {
        dac_ll_cw_generator_enable();
    }
    /* Connect the DAC channel to the cosine wave generator */
    dac_ll_cw_enable_channel(handle->cfg.chan_id, true);
    s_cwg_refer_cnt++;
    handle->is_started = true;
    DAC_EXIT_CRITICAL();

    return ESP_OK;

err:
    esp_clk_tree_enable_src((soc_module_clk_t)handle->cfg.clk_src, false);
    return ret;
}

esp_err_t dac_cosine_stop(dac_cosine_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->is_started, ESP_ERR_INVALID_STATE, TAG,
                        "the dac channel has already stopped");

    /* Enabled DAC channel */
    ESP_RETURN_ON_ERROR(dac_priv_disable_channel(handle->cfg.chan_id), TAG,
                        "disable dac channel %d failed", handle->cfg.chan_id);
    DAC_ENTER_CRITICAL();
    /* Disconnect the DAC channel from the cosine wave generator */
    dac_ll_cw_enable_channel(handle->cfg.chan_id, false);
    s_cwg_refer_cnt--;
    /* Disable the cosine wave generator if no channel using it */
    if (s_cwg_refer_cnt == 0) {
        dac_ll_cw_generator_disable();
    }
    handle->is_started = false;
    DAC_EXIT_CRITICAL();
    /* Release the cosine wave generator clock */
    ESP_RETURN_ON_ERROR(esp_clk_tree_enable_src((soc_module_clk_t)handle->cfg.clk_src, false), TAG, "cosine clock disable failed");

    return ESP_OK;
}
