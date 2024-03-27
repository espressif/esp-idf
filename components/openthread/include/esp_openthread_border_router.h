/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_netif.h"
#include "esp_netif_types.h"
#include "esp_openthread.h"
#include "openthread/instance.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Sets the backbone interface used for border routing.
 *
 * @note This function must be called before esp_openthread_init
 *
 * @param[in]  backbone_netif   The backbone network interface (WiFi or ethernet)
 *
 */
void esp_openthread_set_backbone_netif(esp_netif_t *backbone_netif);

/**
 * @brief   Initializes the border router features of OpenThread.
 *
 * @note Calling this function will make the device behave as an OpenThread
 *       border router. Kconfig option CONFIG_OPENTHREAD_BORDER_ROUTER is required.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NOT_SUPPORTED if feature not supported
 *      - ESP_ERR_INVALID_STATE if already initialized
 *      - ESP_FIAL on other failures
 *
 */
esp_err_t esp_openthread_border_router_init(void);

/**
 * @brief   Deinitializes the border router features of OpenThread.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if not initialized
 *      - ESP_FIAL on other failures
 *
 */
esp_err_t esp_openthread_border_router_deinit(void);

/**
 * @brief   Gets the backbone interface of OpenThread border router.
 *
 * @return
 *      The backbone interface or NULL if border router not initialized.
 *
 */
esp_netif_t *esp_openthread_get_backbone_netif(void);

/**
 * @brief   Registers the callback for RCP failure.
 *
 */
void esp_openthread_register_rcp_failure_handler(esp_openthread_rcp_failure_handler handler);

/**
 * @brief   Deinitializes the conneciton to RCP.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if fail to deinitialize RCP
 *
 */
esp_err_t esp_openthread_rcp_deinit(void);

/**
 * @brief   Initializes the conneciton to RCP.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL if fail to initialize RCP
 *
 */
esp_err_t esp_openthread_rcp_init(void);

#ifdef __cplusplus
}
#endif
