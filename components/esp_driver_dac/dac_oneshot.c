/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "soc/soc_caps.h"
#include "dac_priv_common.h"
#include "driver/dac_oneshot.h"

#if CONFIG_DAC_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_check.h"
#if CONFIG_PM_ENABLE
#include "esp_pm.h"
#endif

struct dac_oneshot_s {
    dac_oneshot_config_t cfg;       /*!< Oneshot mode configurations */
};

static const char *TAG = "dac_oneshot";

esp_err_t dac_oneshot_new_channel(const dac_oneshot_config_t *oneshot_cfg, dac_oneshot_handle_t *ret_handle)
{
#if CONFIG_DAC_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    /* Parameters validation */
    DAC_NULL_POINTER_CHECK(oneshot_cfg);
    DAC_NULL_POINTER_CHECK(ret_handle);
    ESP_RETURN_ON_FALSE(oneshot_cfg->chan_id < SOC_DAC_CHAN_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid dac channel id");

    esp_err_t ret = ESP_OK;
    /* Resources allocation */
    dac_oneshot_handle_t handle = heap_caps_calloc(1, sizeof(struct dac_oneshot_s), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_NO_MEM, TAG, "no memory for the dac oneshot handle");
    memcpy(&handle->cfg, oneshot_cfg, sizeof(dac_oneshot_config_t));

    /* Register and enable the dac channel */
    ESP_GOTO_ON_ERROR(dac_priv_register_channel(oneshot_cfg->chan_id, "dac oneshot"), err2, TAG, "register dac channel %d failed", oneshot_cfg->chan_id);
    ESP_GOTO_ON_ERROR(dac_priv_enable_channel(oneshot_cfg->chan_id), err1, TAG, "enable dac channel %d failed", oneshot_cfg->chan_id);

    *ret_handle = handle;
    return ret;

err1:
    dac_priv_deregister_channel(oneshot_cfg->chan_id);
err2:
    free(handle);
    return ret;
}

esp_err_t dac_oneshot_del_channel(dac_oneshot_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);

    /* Disable and deregister the channel */
    ESP_RETURN_ON_ERROR(dac_priv_disable_channel(handle->cfg.chan_id), TAG, "disable dac channel %d failed", handle->cfg.chan_id);
    ESP_RETURN_ON_ERROR(dac_priv_deregister_channel(handle->cfg.chan_id), TAG, "deregister dac channel %d failed", handle->cfg.chan_id);

    /* Free resources */
    free(handle);

    return ESP_OK;
}

esp_err_t dac_oneshot_output_voltage(dac_oneshot_handle_t handle, uint8_t digi_value)
{
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Set the voltage by the digital value */
    DAC_RTC_ENTER_CRITICAL_SAFE();
    dac_ll_update_output_value(handle->cfg.chan_id, digi_value);
    DAC_RTC_EXIT_CRITICAL_SAFE();

    return ESP_OK;
}
