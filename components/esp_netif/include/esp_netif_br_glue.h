/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Handle of bridge netif glue - an intermediate layer between ESP-NETIF and bridge ports ESP-NETIFs
 * to access ports io drivers properties
 *
 */
typedef struct esp_netif_br_glue_t* esp_netif_br_glue_handle_t;

/**
 * @brief Create a netif glue for bridge
 * @note bridge netif glue is used to attach ports netifs to the bridge (e.g. to get io driver statuses)
 *
 * @return - glue object on success
 *         - NULL on fail
 */
esp_netif_br_glue_handle_t esp_netif_br_glue_new(void);

/**
 * @brief Add a port to the bridge netif glue
 *
 * @param netif_br_glue bridge netif glue
 * @param esp_netif_port port netif
 * @return - ESP_OK on success
 */
esp_err_t esp_netif_br_glue_add_port(esp_netif_br_glue_handle_t netif_br_glue, esp_netif_t *esp_netif_port);

/**
 * @brief Delete netif glue of bridge
 *
 * @param netif_br_glue bridge netif glue
 * @return - ESP_OK: delete netif glue successfully
 */
esp_err_t esp_netif_br_glue_del(esp_netif_br_glue_handle_t netif_br_glue);

#ifdef __cplusplus
}
#endif
