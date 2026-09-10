/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_intr_alloc.h"
#include "esp_memory_utils.h"
#include "esp_sleep.h"
#include "esp_vddbat.h"
#include "esp_private/critical_section.h"
#include "freertos/FreeRTOS.h"
#include "hal/vddbat_ll.h"
#include "hal/brownout_ll.h"
#include "hal/power_supply_periph.h"

static const char *TAG = "vddbat";

// The vddbat interrupt is shared with lp_timer.
#if CONFIG_ESP_VDDBAT_UNDER_VOLT_PROTECT_ISR_CACHE_SAFE
#define VDDBAT_INTR_ALLOC_FLAG      (ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_SHARED)
#define VDDBAT_MEM_ALLOC_CAPS       (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define VDDBAT_INTR_ALLOC_FLAG      (ESP_INTR_FLAG_SHARED)
#define VDDBAT_MEM_ALLOC_CAPS       (MALLOC_CAP_DEFAULT)
#endif

struct vddbat_protect_t {
    volatile esp_vddbat_state_t state;
    esp_vddbat_under_volt_cb_t on_under_volt;
    void *user_arg;
    intr_handle_t intr;
    bool enabled;
};

/* The VGOOD comparator is a single hardware unit, only one handle can exist */
static vddbat_protect_handle_t s_vddbat_handle;

static void vddbat_isr_default_handler(void *arg)
{
    vddbat_protect_handle_t handle = (vddbat_protect_handle_t)arg;

    if (!(vddbat_ll_get_intr_status() & VDDBAT_LL_UNDER_VOLT_INTR)) {
        return;
    }
    vddbat_ll_clear_intr_mask(VDDBAT_LL_UNDER_VOLT_INTR);
    handle->state = ESP_VDDBAT_STATE_UNDER_VOLTAGE;

    /* Detection stays enabled, the callback calls esp_vddbat_protect_disable() to stop it.
     * Read once, the callback can be detached while this interrupt is in flight */
    esp_vddbat_under_volt_cb_t cb = handle->on_under_volt;
    if (cb && cb(handle, handle->user_arg)) {
        portYIELD_FROM_ISR();
    }
}

esp_err_t esp_vddbat_get_state(vddbat_protect_handle_t handle, esp_vddbat_state_t *state)
{
    ESP_RETURN_ON_FALSE(handle && handle == s_vddbat_handle && state, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    *state = handle->state;
    return ESP_OK;
}

bool esp_vddbat_is_under_volt(vddbat_protect_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle && handle == s_vddbat_handle, false, TAG, "invalid handle");
    return vddbat_ll_is_under_volt();
}

esp_err_t esp_vddbat_protect_enable(vddbat_protect_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle && handle == s_vddbat_handle, ESP_ERR_INVALID_ARG, TAG, "invalid handle");
    ESP_RETURN_ON_FALSE(handle->on_under_volt, ESP_ERR_INVALID_STATE, TAG, "callback not registered");
    vddbat_ll_clear_count();
    vddbat_ll_clear_intr_mask(VDDBAT_LL_INTR_MASK);
    handle->state = ESP_VDDBAT_STATE_NORMAL;
    handle->enabled = true;
    vddbat_ll_enable_intr_mask(VDDBAT_LL_UNDER_VOLT_INTR, true);
    return ESP_OK;
}

esp_err_t esp_vddbat_prepare_deep_sleep(vddbat_protect_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle && handle == s_vddbat_handle, ESP_ERR_INVALID_ARG, TAG, "invalid handle");
    esp_sleep_enable_vddbat_recover_wakeup();
    if (!vddbat_ll_is_under_volt()) {
        esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_VDDBAT_UP_VOLT);
        vddbat_ll_clear_intr_mask(VDDBAT_LL_INTR_MASK);
        return ESP_ERR_INVALID_STATE;
    }
    vddbat_ll_clear_intr_mask(VDDBAT_LL_UNDER_VOLT_INTR);
    return ESP_OK;
}

esp_err_t esp_vddbat_protect_register_callbacks(vddbat_protect_handle_t handle, const vddbat_protect_event_callbacks_t *cbs, void *user_arg)
{
    ESP_RETURN_ON_FALSE(handle && handle == s_vddbat_handle && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(!handle->enabled, ESP_ERR_INVALID_STATE, TAG, "register callbacks before enable");
#if CONFIG_ESP_VDDBAT_UNDER_VOLT_PROTECT_ISR_CACHE_SAFE
    ESP_RETURN_ON_FALSE(!cbs->on_under_volt || esp_ptr_in_iram(cbs->on_under_volt), ESP_ERR_INVALID_ARG, TAG, "callback not in IRAM");
    ESP_RETURN_ON_FALSE(!user_arg || esp_ptr_internal(user_arg), ESP_ERR_INVALID_ARG, TAG, "user argument not in internal RAM");
#endif

    handle->user_arg = user_arg;
    handle->on_under_volt = cbs->on_under_volt;
    return ESP_OK;
}

esp_err_t esp_vddbat_protect_install(const vddbat_protect_config_t *config, vddbat_protect_handle_t *ret_handle)
{
    ESP_RETURN_ON_FALSE(config && ret_handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(config->under_volt_filter_ticks != 0 &&
                        config->under_volt_filter_ticks <= VDDBAT_LL_FILTER_TICK_MAX &&
                        config->up_volt_filter_ticks < config->under_volt_filter_ticks,
                        ESP_ERR_INVALID_ARG, TAG, "invalid filter ticks");
    ESP_RETURN_ON_FALSE(!s_vddbat_handle, ESP_ERR_INVALID_STATE, TAG, "already installed");

    vddbat_protect_handle_t handle = heap_caps_calloc(1, sizeof(struct vddbat_protect_t), VDDBAT_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_NO_MEM, TAG, "no memory");

    vddbat_ll_set_vgood_threshold(BROWNOUT_DETECTOR_LL_THRESHOLD_VAL);
    vddbat_ll_set_under_volt_filter_ticks(config->under_volt_filter_ticks);
    vddbat_ll_set_up_volt_filter_ticks(config->up_volt_filter_ticks);
    vddbat_ll_clear_count();
    vddbat_ll_clear_intr_mask(VDDBAT_LL_INTR_MASK);

    /* Shared with the brownout detector. The enable mask stays off until esp_vddbat_protect_enable() */
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_ERROR(esp_intr_alloc_intrstatus(power_supply_periph_signal.irq, VDDBAT_INTR_ALLOC_FLAG,
                                                (uint32_t)vddbat_ll_intr_get_status_reg(), VDDBAT_LL_UNDER_VOLT_INTR,
                                                &vddbat_isr_default_handler, handle, &handle->intr),
                      err, TAG, "interrupt allocation failed");

    vddbat_ll_enable_vgood_reset_source(false);
    s_vddbat_handle = handle;
    *ret_handle = handle;
    return ESP_OK;
err:
    free(handle);
    return ret;
}

esp_err_t esp_vddbat_protect_disable(vddbat_protect_handle_t handle)
{
    ESP_RETURN_ON_FALSE_ISR(handle && handle == s_vddbat_handle, ESP_ERR_INVALID_ARG, TAG, "invalid handle");
    ESP_RETURN_ON_FALSE_ISR(handle->enabled, ESP_ERR_INVALID_STATE, TAG, "not enabled");

    vddbat_ll_enable_intr_mask(VDDBAT_LL_INTR_MASK, false);
    vddbat_ll_clear_intr_mask(VDDBAT_LL_INTR_MASK);
    handle->enabled = false;
    return ESP_OK;
}

esp_err_t esp_vddbat_protect_uninstall(vddbat_protect_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle && handle == s_vddbat_handle, ESP_ERR_INVALID_ARG, TAG, "invalid handle");
    ESP_RETURN_ON_FALSE(!handle->enabled, ESP_ERR_INVALID_STATE, TAG, "disable before uninstall");

    esp_intr_free(handle->intr);
    /* Hand the under voltage back to the analog reset path, software no longer watches it */
    vddbat_ll_enable_vgood_reset_source(true);
    s_vddbat_handle = NULL;
    free(handle);
    return ESP_OK;
}
