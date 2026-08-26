/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"
#if SOC_LP_VAD_SUPPORTED
#include "esp_check.h"
#include "esp_err.h"
#include "ulp_lp_core_lp_vad_shared.h"
#if SOC_LP_I2S_SUPPORT_VAD
//For VAD
#include "hal/lp_i2s_ll.h"
#include "hal/lp_i2s_hal.h"
#include "esp_private/lp_i2s_private.h"
#endif  //SOC_LP_I2S_SUPPORT_VAD

//make this available for multi vad id in future
vad_unit_handle_t s_vad_handle;

esp_err_t lp_core_lp_vad_init(lp_vad_t vad_id, const lp_core_lp_vad_cfg_t *init_config)
{
#if IS_ULP_COCPU
    // Not supported
    return ESP_ERR_NOT_SUPPORTED;
#else
    esp_err_t ret = lp_i2s_vad_new_unit(vad_id, init_config, &s_vad_handle);
    return ret;
#endif
}

esp_err_t lp_core_lp_vad_enable(lp_vad_t vad_id)
{
#if IS_ULP_COCPU
    // Not supported
    return ESP_ERR_NOT_SUPPORTED;
#else
    esp_err_t ret = lp_i2s_vad_enable(s_vad_handle);
    return ret;
#endif
}

esp_err_t lp_core_lp_vad_disable(lp_vad_t vad_id)
{
#if IS_ULP_COCPU
    // Not supported
    return ESP_ERR_NOT_SUPPORTED;
#else
    esp_err_t ret = lp_i2s_vad_disable(s_vad_handle);
    return ret;
#endif
}

esp_err_t lp_core_lp_vad_deinit(lp_vad_t vad_id)
{
#if IS_ULP_COCPU
    // Not supported
    return ESP_ERR_NOT_SUPPORTED;
#else
    esp_err_t ret = lp_i2s_vad_del_unit(s_vad_handle);
    return ret;
#endif
}
#endif /* SOC_LP_VAD_SUPPORTED */
