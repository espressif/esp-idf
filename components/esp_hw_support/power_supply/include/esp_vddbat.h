/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_VDDBAT_PROTECT_SUPPORTED

/**
 * @brief VDDBAT under voltage protection
 *
 * The driver detects the under voltage and calls a user callback from the interrupt handler.
 * Everything else is up to the application: deep sleep cannot be requested from an ISR, so the
 * callback typically only unblocks a task that does the actual shutdown. See the
 * `lowpower/vddbat` example for a complete flow.
 */

/**
 * @brief Type of the VDDBAT under voltage protection handle
 */
typedef struct vddbat_protect_t *vddbat_protect_handle_t;

typedef enum {
    ESP_VDDBAT_STATE_NORMAL,            /*!< VDDBAT is above the under voltage threshold */
    ESP_VDDBAT_STATE_UNDER_VOLTAGE,     /*!< VDDBAT dropped below the under voltage threshold */
} esp_vddbat_state_t;

/**
 * @brief Configuration of the VDDBAT under voltage protection
 */
typedef struct {
    uint32_t under_volt_filter_ticks;   /*!< Net low ticks of the RC_FAST clocked filter counter needed
                                             to confirm an under voltage, from 1 to 1023 */
    uint32_t up_volt_filter_ticks;      /*!< Filter counter value at which a recovery is confirmed, must
                                             be lower than `under_volt_filter_ticks`. The distance between
                                             the two is the hysteresis of the filter */
} vddbat_protect_config_t;

/**
 * @brief Default protection configuration
 *
 * 512 net low ticks is about 64 us at the nominal 8 MHz RC_FAST.
 */
#define VDDBAT_PROTECT_DEFAULT_CONFIG() { \
    .under_volt_filter_ticks = 512,       \
    .up_volt_filter_ticks = 128,          \
}

/**
 * @brief VDDBAT under voltage callback
 *
 * Called from the interrupt handler once an under voltage is confirmed. It runs in ISR context,
 * so it must not block and must not request deep sleep directly.
 * Detection stays enabled after the callback. Call esp_vddbat_protect_disable() here to stop
 * further reports, and esp_vddbat_protect_enable() after VDDBAT recovers.
 *
 * @param[in] handle   the handle returned by esp_vddbat_protect_install()
 * @param[in] user_arg the argument passed to esp_vddbat_protect_register_callbacks()
 * @return whether a high priority task has been woken up by this function
 */
typedef bool (*esp_vddbat_under_volt_cb_t)(vddbat_protect_handle_t handle, void *user_arg);

/**
 * @brief Group of VDDBAT protection callbacks, all of them run in ISR context
 */
typedef struct {
    esp_vddbat_under_volt_cb_t on_under_volt;   /*!< Called on a confirmed under voltage */
} vddbat_protect_event_callbacks_t;

/**
 * @brief Install the VDDBAT under voltage protection
 *
 * @param[in]  config     protection configuration, see VDDBAT_PROTECT_DEFAULT_CONFIG()
 * @param[out] ret_handle the created protection handle
 * @return
 *  - ESP_OK: driver installed
 *  - ESP_ERR_INVALID_ARG: NULL pointer, or the filter ticks are out of range
 *  - ESP_ERR_INVALID_STATE: the driver is already installed
 *  - ESP_ERR_NO_MEM: out of memory
 *  - Others: interrupt allocation failed
 */
esp_err_t esp_vddbat_protect_install(const vddbat_protect_config_t *config, vddbat_protect_handle_t *ret_handle);

/**
 * @brief Register the VDDBAT protection callbacks
 *
 * @param[in] handle   the handle returned by esp_vddbat_protect_install()
 * @param[in] cbs      group of callbacks
 * @param[in] user_arg argument passed back to the callbacks
 * @return
 *  - ESP_OK: callbacks registered
 *  - ESP_ERR_INVALID_ARG: invalid handle or NULL `cbs`, or the callback is not in IRAM and
 *    `user_arg` is not in internal RAM while CONFIG_ESP_VDDBAT_UNDER_VOLT_PROTECT_ISR_CACHE_SAFE is enabled
 *  - ESP_ERR_INVALID_STATE: protection is enabled
 */
esp_err_t esp_vddbat_protect_register_callbacks(vddbat_protect_handle_t handle, const vddbat_protect_event_callbacks_t *cbs, void *user_arg);

/**
 * @brief Enable the VDDBAT under voltage protection
 *
 * Handles the under voltage detection.
 * @note This is not called during startup, the application decides when the protection becomes
 *       active. There is no under voltage protection before this call.
 *
 * @param[in] handle the handle returned by esp_vddbat_protect_install()
 * @return
 *  - ESP_OK: detection armed
 *  - ESP_ERR_INVALID_ARG: invalid handle
 *  - ESP_ERR_INVALID_STATE: no callback has been registered
 */
esp_err_t esp_vddbat_protect_enable(vddbat_protect_handle_t handle);

/**
 * @brief Disable the VDDBAT under voltage protection
 *
 * Disables the detection. The interrupt handler, the handle and the callbacks stay in place, so
 * esp_vddbat_protect_enable() can arm it again. esp_vddbat_protect_uninstall() releases the rest.
 *
 * @note Can be called from the under voltage callback. It is placed in IRAM when
 *       CONFIG_ESP_VDDBAT_UNDER_VOLT_PROTECT_ISR_CACHE_SAFE is enabled.
 *
 * @param[in] handle the handle returned by esp_vddbat_protect_install()
 * @return
 *  - ESP_OK: protection disabled
 *  - ESP_ERR_INVALID_ARG: invalid handle
 *  - ESP_ERR_INVALID_STATE: protection is not enabled
 */
esp_err_t esp_vddbat_protect_disable(vddbat_protect_handle_t handle);

/**
 * @brief Uninstall the VDDBAT under voltage protection
 *
 * Releases the interrupt handler and the handle, and puts VDDBAT_VGOOD back on the analog reset
 * path. The protection has to be disabled first.
 *
 * @param[in] handle the handle returned by esp_vddbat_protect_install()
 * @return
 *  - ESP_OK: driver uninstalled
 *  - ESP_ERR_INVALID_ARG: invalid handle
 *  - ESP_ERR_INVALID_STATE: protection is still enabled
 */
esp_err_t esp_vddbat_protect_uninstall(vddbat_protect_handle_t handle);

/**
 * @brief Prepare deep sleep that wakes when VDDBAT recovers
 *
 * Handles the VDDBAT recovery wakeup and clears the under-voltage interrupt. Does not enter
 * deep sleep. Call esp_deep_sleep_start() afterwards, after any extra shutdown work.
 *
 * @param[in] handle the handle returned by esp_vddbat_protect_install()
 * @return
 *  - ESP_ERR_INVALID_ARG: invalid handle
 *  - ESP_ERR_INVALID_STATE: VDDBAT already recovered
 */
esp_err_t esp_vddbat_prepare_deep_sleep(vddbat_protect_handle_t handle);

/**
 * @brief Get the live under voltage flag of the hardware filter
 *
 * @param[in] handle the handle returned by esp_vddbat_protect_install()
 * @return true while VDDBAT is below the under voltage threshold
 */
bool esp_vddbat_is_under_volt(vddbat_protect_handle_t handle);

/**
 * @brief Get the last reported VDDBAT state
 *
 * Reflects what the interrupt handler last observed, use esp_vddbat_is_under_volt(handle) for the
 * current hardware state.
 *
 * @param[in]  handle the handle returned by esp_vddbat_protect_install()
 * @param[out] state  the last reported state
 * @return
 *  - ESP_OK: state read
 *  - ESP_ERR_INVALID_ARG: `handle` or `state` is NULL
 */
esp_err_t esp_vddbat_get_state(vddbat_protect_handle_t handle, esp_vddbat_state_t *state);

#endif // SOC_VDDBAT_PROTECT_SUPPORTED

#ifdef __cplusplus
}
#endif
