/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_netif.h"
#include "esp_openthread_types.h"
#include "openthread/instance.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief  Default configuration reference of OpenThread esp-netif
*/
#define ESP_NETIF_INHERENT_DEFAULT_OPENTHREAD() \
    {   \
        .flags = 0, \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(mac) \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(ip_info) \
        .get_ip_event = 0,    \
        .lost_ip_event = 0,   \
        .if_key = "OT_DEF",  \
        .if_desc = "openthread",    \
        .route_prio = 15      \
};

#define ESP_NETIF_DEFAULT_OPENTHREAD()                     \
    {                                                      \
        .base = &g_esp_netif_inherent_openthread_config,   \
        .driver = NULL,                                    \
        .stack = &g_esp_netif_netstack_default_openthread, \
    }

extern const esp_netif_netstack_config_t g_esp_netif_netstack_default_openthread;
extern const esp_netif_inherent_config_t g_esp_netif_inherent_openthread_config;

/**
 * @brief This function initializes the OpenThread network interface glue.
 *
 * @param[in] config    The platform configuration.
 *
 * @return
 *      - glue pointer on success
 *      - NULL on failure
 *
 */
void *esp_openthread_netif_glue_init(const esp_openthread_platform_config_t *config);

/**
 * @brief This function deinitializes the OpenThread network interface glue.
 *
 */
void esp_openthread_netif_glue_deinit(void);

/**
 * @brief This function acquires the OpenThread netif.
 *
 * @return
 *      The OpenThread netif or NULL if not initialzied.
 *
 */
esp_netif_t *esp_openthread_get_netif(void);

/**
 * @brief This function register a handler for meshcop-e service publish event and remove event.
 *
 * @param[in] handler       The handler.
 * @param[in] for_publish   The usage of handler, true for publish event and false for remove event.
 *
 */
void esp_openthread_register_meshcop_e_handler(esp_event_handler_t handler, bool for_publish);

#ifdef __cplusplus
}
#endif
