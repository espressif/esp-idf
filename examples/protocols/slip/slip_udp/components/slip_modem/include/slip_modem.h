// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_netif.h"
#include "esp_netif_slip.h"

#include "driver/uart.h"

// Forward declare modem object
typedef struct esp_slip_modem esp_slip_modem_t;

// Filter callbacks for handling application specific slip messages
typedef bool slip_rx_filter_cb_t(void *ctx, uint8_t *data, uint32_t len);


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
    void *rx_filter_ctx;                /* Context to be passed to SLIP filter function */

} esp_slip_modem_config_t;


/** @brief Create a slip modem
 *
 * @param[in]    slip configured esp netif
 * @param[in]    configuration for the slip modem
 *
 * @returns
 *          - slip modem driver glue object
 */
void *esp_slip_modem_create(esp_netif_t *slip_netif, esp_slip_modem_config_t *modem_config);

/** @brief Destroy a slip modem
 *
 * @param[in]    slip modem object for destruction
 *
 * @return
 *          - ESP_OK on success
 */
esp_err_t esp_slip_modem_destroy(esp_slip_modem_t *slip_modem);
