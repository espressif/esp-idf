/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_netif.h"
#include "driver/uart.h"

/** @brief Configuration of SLIP network interface
 *
 */
#define ESP_NETIF_INHERENT_DEFAULT_SLIP() \
    {   \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(mac) \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(ip_info) \
        .get_ip_event = 0,    \
        .lost_ip_event = 0,   \
        .if_key = "SLP_DEF",  \
        .if_desc = "slip",    \
        .route_prio = 16,     \
        .bridge_info = NULL \
};

extern esp_netif_netstack_config_t *netstack_default_slip;

// Forward declare modem object
typedef struct slip_modem slip_modem_t;

// Filter callbacks for handling application specific slip messages
typedef bool slip_rx_filter_cb_t(slip_modem_t *slip, uint8_t *data, uint32_t len);


/** @brief Configuration structure for SLIP modem interface
 *
 */
typedef struct {
    uart_port_t uart_dev; /* UART device for reading and writing SLIP information, this must be initialised externally */

    int uart_tx_pin;      /* UART TX pin number */
    int uart_rx_pin;      /* UART TX pin number */

    uint32_t uart_baud;   /* UART baud rate */

    uint32_t rx_buffer_len;             /* Length of buffer for RX messages */

    slip_rx_filter_cb_t *rx_filter;     /* Filter for parsing out non-SLIP messages from incoming SLIP stream */
    esp_ip6_addr_t *ipv6_addr;

} slip_modem_config_t;



/** @brief Create a slip modem
 *
 * @param[in]    slip configured esp netif
 * @param[in]    configuration for the slip modem
 *
 * @returns
 *          - slip modem driver glue object
 */
slip_modem_t *slip_modem_create(esp_netif_t *slip_netif, slip_modem_config_t *modem_config);

/** @brief Destroy a slip modem
 *
 * @param[in]    slip modem object for destruction
 *
 * @return
 *          - ESP_OK on success
 */
esp_err_t slip_modem_destroy(slip_modem_t *slip);

/**
 * @brief Getter for the internally configured IPv6 address
 *
 * @param[in]    slip modem object
 *
 * @returns
 *          - ipv6 address
 */
const esp_ip6_addr_t *slip_modem_get_ipv6_address(slip_modem_t *slip);

/**
 * @brief  Data path API that forward the supplied data to the attached network interface
 *
 * @param[in]    slip modem object
 * @param[in]    buffer pointer to the outgoing data
 * @param[in]    len length of the data
 *
 */
void slip_modem_raw_output(slip_modem_t *slip, void *buffer, size_t len);
