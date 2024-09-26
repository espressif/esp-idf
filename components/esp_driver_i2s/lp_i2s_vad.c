/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include "soc/soc_caps.h"
#include "stdatomic.h"
#if SOC_LP_VAD_SUPPORTED
#include "esp_check.h"
#include "esp_err.h"
#include "driver/lp_i2s_vad.h"
#include "esp_heap_caps.h"
#include "hal/lp_i2s_ll.h"
#include "hal/lp_i2s_hal.h"
#include "esp_private/lp_i2s_private.h"

#define LP_VAD_MEM_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)

static const char *LP_VAD_TAG = "LP_VAD";

typedef enum {
    VAD_FSM_INIT,
    VAD_FSM_ENABLE,
} vad_fsm_t;

typedef struct vad_unit_ctx_t {
    lp_i2s_soc_handle_t hw;
    lp_vad_t vad_id;
    vad_fsm_t fsm;
} vad_unit_ctx_t;

static atomic_bool s_vad_id_claimed[SOC_ADC_PERIPH_NUM] = {ATOMIC_VAR_INIT(false)};

static bool s_vad_claim(lp_vad_t vad_id)
{
    bool false_var = false;
    return atomic_compare_exchange_strong(&s_vad_id_claimed[vad_id], &false_var, true);
}

static bool s_vad_free(lp_vad_t vad_id)
{
    bool true_var = true;
    return atomic_compare_exchange_strong(&s_vad_id_claimed[vad_id], &true_var, false);
}

esp_err_t lp_i2s_vad_new_unit(lp_vad_t vad_id, const lp_vad_init_config_t *init_config, vad_unit_handle_t *ret_unit)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(init_config, ESP_ERR_INVALID_ARG, LP_VAD_TAG, "invalid arg");
    ESP_RETURN_ON_FALSE(init_config->lp_i2s_chan, ESP_ERR_INVALID_ARG, LP_VAD_TAG, "LP I2S not initialised");
    ESP_RETURN_ON_FALSE(init_config->vad_config.init_frame_num >= LP_VAD_LL_INIT_FRAME_MIN && init_config->vad_config.init_frame_num <= LP_VAD_LL_INIT_FRAME_MAX, ESP_ERR_INVALID_ARG, LP_VAD_TAG, "invalid init frame num");

    bool success_claim = s_vad_claim(vad_id);
    ESP_RETURN_ON_FALSE(success_claim, ESP_ERR_NOT_FOUND, LP_VAD_TAG, "vad%"PRId32" is already in use", vad_id);

    vad_unit_ctx_t *unit = heap_caps_calloc(1, sizeof(vad_unit_ctx_t), LP_VAD_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(unit, ESP_ERR_NO_MEM, err, LP_VAD_TAG, "no mem for unit");

    unit->hw = lp_i2s_get_soc_handle(init_config->lp_i2s_chan);
    ESP_LOGD(LP_VAD_TAG, "unit->hw: %p", unit->hw);
    lp_vad_ll_set_init_frame_num(unit->hw, init_config->vad_config.init_frame_num);
    lp_vad_ll_set_init_min_energy(unit->hw, init_config->vad_config.min_energy_thresh);
    lp_vad_ll_set_speak_activity_thresh(unit->hw, init_config->vad_config.speak_activity_thresh);
    lp_vad_ll_set_non_speak_activity_thresh(unit->hw, init_config->vad_config.non_speak_activity_thresh);
    lp_vad_ll_set_min_speak_activity_thresh(unit->hw, init_config->vad_config.min_speak_activity_thresh);
    lp_vad_ll_set_max_speak_activity_thresh(unit->hw, init_config->vad_config.max_speak_activity_thresh);
    lp_vad_ll_skip_band_energy(unit->hw, init_config->vad_config.skip_band_energy_thresh);
    unit->fsm = VAD_FSM_INIT;
    *ret_unit = unit;

    return ESP_OK;
err:
    bool success_free = s_vad_free(vad_id);
    assert(success_free);

    return ret;
}

esp_err_t lp_i2s_vad_enable(vad_unit_handle_t unit)
{
    ESP_RETURN_ON_FALSE(unit, ESP_ERR_INVALID_ARG, LP_VAD_TAG, "invalid arg");
    ESP_RETURN_ON_FALSE(unit->fsm == VAD_FSM_INIT, ESP_ERR_INVALID_STATE, LP_VAD_TAG, "The driver is enabled already");

    lp_vad_ll_enable(unit->hw, true);
    unit->fsm = VAD_FSM_ENABLE;
    return ESP_OK;
}

esp_err_t lp_i2s_vad_disable(vad_unit_handle_t unit)
{
    ESP_RETURN_ON_FALSE(unit, ESP_ERR_INVALID_ARG, LP_VAD_TAG, "invalid arg");
    ESP_RETURN_ON_FALSE(unit->fsm == VAD_FSM_ENABLE, ESP_ERR_INVALID_STATE, LP_VAD_TAG, "The driver is not enabled yet");

    lp_vad_ll_enable(unit->hw, false);
    unit->fsm = VAD_FSM_INIT;
    return ESP_OK;
}

esp_err_t lp_i2s_vad_del_unit(vad_unit_handle_t unit)
{
    ESP_RETURN_ON_FALSE(unit, ESP_ERR_INVALID_ARG, LP_VAD_TAG, "invalid arg");
    ESP_RETURN_ON_FALSE(unit->fsm == VAD_FSM_INIT, ESP_ERR_INVALID_STATE, LP_VAD_TAG, "The driver is still in enabled state");

    bool success_free = s_vad_free(unit->vad_id);
    ESP_RETURN_ON_FALSE(success_free, ESP_ERR_NOT_FOUND, LP_VAD_TAG, "vad%"PRId32" isn't in use", unit->vad_id);

    free(unit);
    return ESP_OK;
}
#endif /* SOC_LP_VAD_SUPPORTED */
