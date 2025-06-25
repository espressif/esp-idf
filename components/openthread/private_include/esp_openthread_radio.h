/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>

#include "esp_err.h"
#include "esp_openthread_types.h"
#include "openthread/instance.h"

#if (CONFIG_ESP_COEX_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE)
#include "esp_coex_i154.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function initializes the OpenThread radio.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM if allocation has failed
 *
 */
esp_err_t esp_openthread_radio_init(const esp_openthread_platform_config_t *config);

/**
 * @brief This function deinitializes the OpenThread radio.
 *
 */
void esp_openthread_radio_deinit(void);

/**
 * @brief This function updates the radio fds and timeouts to the main loop.
 *
 * @param[inout]    mainloop    The main loop context.
 *
 */
void esp_openthread_radio_update(esp_openthread_mainloop_context_t *mainloop);

/**
 * @brief This function handles netif change for radio spinel.
 *
 * @param[in]    state    The updated netif state.
 *
 */
void esp_openthread_handle_netif_state_change(bool state);

/**
 * @brief This function performs the OpenThread radio process.
 *
 * @param[in]    instance   The OpenThread instance.
 * @param[in]    mainloop   The main loop context.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failure
 *
 */
esp_err_t esp_openthread_radio_process(otInstance *instance, const esp_openthread_mainloop_context_t *mainloop);


#if (CONFIG_ESP_COEX_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE)

/**
 * @brief  Set the coexist config.
 *
 * @param[in]  config     The config of coexist.
 *
 */
void esp_openthread_set_coex_config(esp_ieee802154_coex_config_t config);

/**
 * @brief  Get the coexist config.
 *
 * @return
 *        - The config of coexist.
 *
 */
esp_ieee802154_coex_config_t esp_openthread_get_coex_config(void);

/**
 * @brief This function updates the TREL fds and timeouts to the main loop.
 *
 * @param[inout]    mainloop    The main loop context.
 *
 */
void esp_openthread_trel_update(esp_openthread_mainloop_context_t *mainloop);

/**
 * @brief This function performs the OpenThread TREL process.
 *
 * @param[in]    instance   The OpenThread instance.
 * @param[in]    mainloop   The main loop context.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failure
 *
 */
esp_err_t esp_openthread_trel_process(otInstance *aInstance, const esp_openthread_mainloop_context_t *mainloop);
#endif

#ifdef __cplusplus
}
#endif
