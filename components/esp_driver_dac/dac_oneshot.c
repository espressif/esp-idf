/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "dac_priv_common.h"
#include "driver/dac_oneshot.h"
#include "hal/dac_ll.h"
#include "esp_check.h"
#include "esp_log.h"

struct dac_oneshot_s {
    dac_oneshot_config_t cfg;       /*!< Oneshot mode configurations */
};

esp_err_t dac_oneshot_new_channel(const dac_oneshot_config_t *oneshot_cfg, dac_oneshot_handle_t *ret_handle)
{
    /* Parameters validation */
    DAC_NULL_POINTER_CHECK(oneshot_cfg);
    DAC_NULL_POINTER_CHECK(ret_handle);
    ESP_RETURN_ON_FALSE(IS_VALID_DAC_CHANNEL(oneshot_cfg->chan_id), ESP_ERR_INVALID_ARG, TAG, "invalid dac channel id");
    ESP_RETURN_ON_FALSE(dac_priv_is_valid_channel_config(&oneshot_cfg->chan_cfg), ESP_ERR_INVALID_ARG, TAG, "invalid DAC channel configuration");

    dac_oneshot_clk_src_t clk_src = oneshot_cfg->clk_src ? : DAC_ONESHOT_CLK_SRC_DEFAULT;
    ESP_RETURN_ON_FALSE(IS_VALID_DAC_CLK_SRC(clk_src, SOC_DAC_ONESHOT_CLKS), ESP_ERR_INVALID_ARG, TAG, "invalid DAC oneshot clock source");

    esp_err_t ret = ESP_OK;
    /* Allocate the handle */
    dac_oneshot_handle_t handle = heap_caps_calloc(1, sizeof(struct dac_oneshot_s), DAC_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_NO_MEM, TAG, "no memory for the dac oneshot handle");
    handle->cfg = *oneshot_cfg;
    handle->cfg.clk_src = clk_src;

    /* Acquire DAC clocks */
    ESP_GOTO_ON_ERROR(dac_priv_clk_acquire_unified((soc_module_clk_t)clk_src, DAC_LS_CLK_CONSTRAINT_DEFAULT, NULL),
                      err_handle, TAG, "acquire DAC clock failed");

    /* Register the dac channel */
    ESP_GOTO_ON_ERROR(dac_priv_channel_register(oneshot_cfg->chan_id), err_clk, TAG, "register dac channel %d failed", oneshot_cfg->chan_id);

    /* Acquire and start SINTX */
#if DAC_LL_DC_VIA_SINTX
    ESP_GOTO_ON_ERROR(dac_priv_sintx_acquire(DAC_SINTX_MODE_DC), err_chan, TAG, "acquire DAC sintx failed");
    DAC_ENTER_CRITICAL();
    dac_priv_sintx_set_freq_unlocked((dac_sintx_freq_hw_t) {
        .timer_target = 0,
        .fstep = 0,
    });
    DAC_EXIT_CRITICAL();
    ESP_GOTO_ON_ERROR(dac_priv_sintx_start(), err_sintx, TAG, "start DAC sintx generator failed");
#endif

    /* Initialize the output code before powering on the pad. */
    DAC_ENTER_CRITICAL();
#if DAC_LL_DC_VIA_SINTX
    dac_ll_cw_set_atten(oneshot_cfg->chan_id, DAC_COSINE_ATTEN_DEFAULT);
    dac_ll_cw_set_phase(oneshot_cfg->chan_id, DAC_COSINE_PHASE_0);
#endif
    dac_ll_pad_set_output_code(oneshot_cfg->chan_id, 0);
    DAC_EXIT_CRITICAL();

    ESP_GOTO_ON_ERROR(dac_priv_channel_enable(oneshot_cfg->chan_id, oneshot_cfg->chan_cfg, DAC_DATA_SOURCE_DIRECT), err_stop, TAG, "enable dac channel %d failed", oneshot_cfg->chan_id);

    *ret_handle = handle;
    return ret;

err_stop:
#if DAC_LL_DC_VIA_SINTX
    dac_priv_sintx_stop();
err_sintx:
    dac_priv_sintx_release();
err_chan:
#endif
    dac_priv_channel_deregister(oneshot_cfg->chan_id);
err_clk:
    dac_priv_clk_release_unified((soc_module_clk_t)clk_src, DAC_LS_CLK_CONSTRAINT_DEFAULT);
err_handle:
    free(handle);
    return ret;
}

esp_err_t dac_oneshot_del_channel(dac_oneshot_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);

    ESP_RETURN_ON_ERROR(dac_priv_channel_disable(handle->cfg.chan_id), TAG, "disable dac channel %d failed", handle->cfg.chan_id);
#if DAC_LL_DC_VIA_SINTX
    ESP_RETURN_ON_ERROR(dac_priv_sintx_stop(), TAG, "stop DAC sintx failed");
    ESP_RETURN_ON_ERROR(dac_priv_sintx_release(), TAG, "release DAC sintx failed");
#endif
    ESP_RETURN_ON_ERROR(dac_priv_channel_deregister(handle->cfg.chan_id), TAG, "deregister dac channel %d failed", handle->cfg.chan_id);
    ESP_RETURN_ON_ERROR(dac_priv_clk_release_unified((soc_module_clk_t)handle->cfg.clk_src, DAC_LS_CLK_CONSTRAINT_DEFAULT),
                        TAG, "release DAC clock failed");

    /* Free resources */
    free(handle);

    return ESP_OK;
}

esp_err_t dac_oneshot_output_code(dac_oneshot_handle_t handle, uint16_t digi_value)
{
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }

    if (digi_value >= (1U << dac_priv_chan_cfg_get_bitwidth(&handle->cfg.chan_cfg))) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Set the analog output by the digital value */
    DAC_ENTER_CRITICAL_SAFE();
    dac_ll_pad_set_output_code(handle->cfg.chan_id, digi_value);
    DAC_EXIT_CRITICAL_SAFE();

    return ESP_OK;
}

esp_err_t dac_oneshot_output_voltage(dac_oneshot_handle_t handle, uint16_t digi_value)
__attribute__((alias("dac_oneshot_output_code")));

uint8_t dac_oneshot_get_bitwidth(dac_oneshot_handle_t handle)
{
    if (!handle) {
        return 0;
    }

    return dac_priv_chan_cfg_get_bitwidth(&handle->cfg.chan_cfg);
}
