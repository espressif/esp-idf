/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <sys/lock.h>
#include "esp_check.h"
#include "esp_hw_log.h"
#include "esp_log.h"
#include "esp_private/esp_clk_tree_common.h"
#include "soc/rtc.h"

ESP_HW_LOG_ATTR_TAG(s_tag, "esp_clk_tree_apll");
static _lock_t s_periph_apll_mutex;

typedef struct {
    uint32_t o_div;
    uint32_t sdm0;
    uint32_t sdm1;
    uint32_t sdm2;
} apll_freq_config_t;

typedef enum {
    APLL_MODE_IDLE,
    APLL_MODE_FIXED,
    APLL_MODE_ADJUSTABLE,
} apll_mode_t;

typedef struct {
    atomic_uint configured_freq_hz;
    int ref_cnt;
    apll_mode_t mode;
    apll_freq_config_t pending_config; // pending configuration for delayed acquisition
} apll_resource_t;

static apll_resource_t s_apll_resource;

esp_err_t esp_clk_tree_apll_get_info(esp_clk_tree_apll_info_t *info)
{
    ESP_RETURN_ON_FALSE(info, ESP_ERR_INVALID_ARG, s_tag, "invalid argument");

    info->frequency_hz = atomic_load(&s_apll_resource.configured_freq_hz);
    info->config_valid = info->frequency_hz != 0;
    return ESP_OK;
}

esp_err_t esp_clk_tree_apll_acquire(const esp_clk_tree_src_config_t *config)
{
    bool acquire_allowed = true;
    bool adjustable = config && config->flags.adjustable;

    _lock_acquire(&s_periph_apll_mutex);
    apll_mode_t mode = s_apll_resource.mode;
    bool idle = mode == APLL_MODE_IDLE;
    if (adjustable) {
        acquire_allowed = idle;
    } else { // apply for fixed mode
        acquire_allowed = mode != APLL_MODE_ADJUSTABLE;
    }

    if (acquire_allowed) {
        s_apll_resource.ref_cnt++;
        if (idle) {
            s_apll_resource.mode = adjustable ? APLL_MODE_ADJUSTABLE : APLL_MODE_FIXED;
            rtc_clk_apll_enable(true);
            uint32_t configured_freq_hz = atomic_load(&s_apll_resource.configured_freq_hz);
            if (configured_freq_hz != 0) {
                apll_freq_config_t config_to_apply = s_apll_resource.pending_config;
                s_apll_resource.pending_config = (apll_freq_config_t) {};
                ESP_LOGD(s_tag, "APLL works at %"PRIu32" Hz with coefficients [sdm0] %"PRIu32" [sdm1] %"PRIu32" [sdm2] %"PRIu32" [o_div] %"PRIu32"",
                         configured_freq_hz, config_to_apply.sdm0, config_to_apply.sdm1,
                         config_to_apply.sdm2, config_to_apply.o_div);
                rtc_clk_apll_coeff_set(config_to_apply.o_div, config_to_apply.sdm0, config_to_apply.sdm1,
                                       config_to_apply.sdm2);
            }
        }
    }
    _lock_release(&s_periph_apll_mutex);

    ESP_RETURN_ON_FALSE(acquire_allowed, ESP_ERR_INVALID_STATE, s_tag,
                        "can't acquire APLL, resource conflict or initialization in progress");
    return ESP_OK;
}

esp_err_t esp_clk_tree_apll_release(void)
{
    bool valid_state = true;
    _lock_acquire(&s_periph_apll_mutex);
    if (s_apll_resource.ref_cnt <= 0) {
        valid_state = false;
    } else {
        s_apll_resource.ref_cnt--;
        if (s_apll_resource.ref_cnt == 0) {
            s_apll_resource.mode = APLL_MODE_IDLE;
            atomic_store(&s_apll_resource.configured_freq_hz, 0);
            s_apll_resource.pending_config = (apll_freq_config_t) {};
            rtc_clk_apll_enable(false);
        }
    }
    _lock_release(&s_periph_apll_mutex);
    ESP_RETURN_ON_FALSE(valid_state, ESP_ERR_INVALID_STATE, s_tag, "APLL released too many times");
    return ESP_OK;
}

esp_err_t esp_clk_tree_apll_freq_set(uint32_t expt_freq_hz, uint32_t *real_freq_hz)
{
    apll_freq_config_t requested_config = {};
    uint32_t resolved_freq_hz = rtc_clk_apll_coeff_calc(expt_freq_hz, &requested_config.o_div,
                                                        &requested_config.sdm0, &requested_config.sdm1,
                                                        &requested_config.sdm2);
    ESP_RETURN_ON_FALSE(resolved_freq_hz, ESP_ERR_INVALID_ARG, s_tag,
                        "APLL coefficients calculate failed");

    esp_err_t ret = ESP_ERR_INVALID_STATE;
    _lock_acquire(&s_periph_apll_mutex);
    uint32_t configured_freq_hz = atomic_load(&s_apll_resource.configured_freq_hz);
    bool same_frequency = configured_freq_hz == resolved_freq_hz;
    apll_mode_t mode = s_apll_resource.mode;
    if (same_frequency) {
        // An idempotent request never changes the clock observed by active consumers.
        ret = ESP_OK;
    } else if (mode == APLL_MODE_IDLE) {
        // Not acquired yet, store the request for later acquisition
        s_apll_resource.pending_config = requested_config;
        ret = ESP_OK;
    } else if (mode == APLL_MODE_FIXED && configured_freq_hz != 0) {
        // A configured fixed resource rejects requests for a different frequency
        ret = ESP_ERR_INVALID_STATE;
    } else {
        // Fixed but not configured, or in adjustable mode, apply the request
        ESP_LOGD(s_tag, "APLL works at %"PRIu32" Hz with coefficients [sdm0] %"PRIu32" [sdm1] %"PRIu32" [sdm2] %"PRIu32" [o_div] %"PRIu32"",
                 resolved_freq_hz, requested_config.sdm0, requested_config.sdm1,
                 requested_config.sdm2, requested_config.o_div);
        rtc_clk_apll_coeff_set(requested_config.o_div, requested_config.sdm0, requested_config.sdm1,
                               requested_config.sdm2);
        ret = ESP_OK;
    }
    if (ret == ESP_OK && !same_frequency) {
        atomic_store(&s_apll_resource.configured_freq_hz, resolved_freq_hz);
    }
    uint32_t actual_freq_hz = atomic_load(&s_apll_resource.configured_freq_hz);
    _lock_release(&s_periph_apll_mutex);

    if (real_freq_hz) {
        *real_freq_hz = actual_freq_hz;
    }
    ESP_RETURN_ON_FALSE(ret == ESP_OK, ret, s_tag, "APLL frequency is locked by active consumers");
    return ret;
}

bool esp_clk_tree_apll_is_power_on(void)
{
    _lock_acquire(&s_periph_apll_mutex);
    bool on = s_apll_resource.ref_cnt > 0;
    _lock_release(&s_periph_apll_mutex);
    return on;
}
