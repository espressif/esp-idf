/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_eth_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Handle of netif glue - an intermediate layer between netif and Ethernet driver
 *
 */
typedef struct esp_eth_netif_glue_t* esp_eth_netif_glue_handle_t;

/**
 * @brief Create a netif glue for Ethernet driver
 * @note netif glue is used to attach io driver to TCP/IP netif
 *
 * @param eth_hdl Ethernet driver handle
 * @return glue object, which inherits esp_netif_driver_base_t
 */
esp_eth_netif_glue_handle_t esp_eth_new_netif_glue(esp_eth_handle_t eth_hdl);

/**
 * @brief Delete netif glue of Ethernet driver
 *
 * @param eth_netif_glue netif glue
 * @return -ESP_OK: delete netif glue successfully
 */
esp_err_t esp_eth_del_netif_glue(esp_eth_netif_glue_handle_t eth_netif_glue);

#ifdef __cplusplus
}
#endif
