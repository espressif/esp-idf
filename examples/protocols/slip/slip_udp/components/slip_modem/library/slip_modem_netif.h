/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @brief Stop the esp slip netif
 *
 * @param[in]    esp_netif handle to slip esp-netif instance
 *
 * @return
 *         - ESP_OK on success
 */
esp_err_t slip_modem_netif_stop(esp_netif_t *esp_netif);

/**
 * @brief Start the esp slip netif
 *
 * @param[in]    esp_netif handle to slip esp-netif instance
 * @param[in]    addr IPv6 address associated with this SLIP interface
 *
 * @return
 *         - ESP_OK on success
 */
esp_err_t slip_modem_netif_start(esp_netif_t *esp_netif, esp_ip6_addr_t *addr);

/**
 * @brief  Data path API to write raw packet ous the SLIP interface
 *
 * This API is typically used when implementing user defined methods
 *
 * @param[in]    esp_netif handle to slip esp-netif instance
 * @param[in]    buffer pointer to the outgoing data
 * @param[in]    len length of the data
 *
 * @return
 *         - ESP_OK on success
 */
void slip_modem_netif_raw_output(esp_netif_t *netif, void *buffer, size_t len);
