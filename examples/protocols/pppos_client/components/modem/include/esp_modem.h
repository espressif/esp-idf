// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_modem_dce.h"
#include "esp_modem_dte.h"
#include "esp_event.h"
#include "driver/uart.h"
#include "esp_modem_compat.h"

/**
 * @brief Declare Event Base for ESP Modem
 *
 */
ESP_EVENT_DECLARE_BASE(ESP_MODEM_EVENT);

/**
 * @brief ESP Modem Event
 *
 */
typedef enum {
    ESP_MODEM_EVENT_PPP_START = 0,       /*!< ESP Modem Start PPP Session */
    ESP_MODEM_EVENT_PPP_STOP  = 3,       /*!< ESP Modem Stop PPP Session*/
    ESP_MODEM_EVENT_UNKNOWN   = 4        /*!< ESP Modem Unknown Response */
} esp_modem_event_t;

/**
 * @brief ESP Modem DTE Configuration
 *
 */
typedef struct {
    uart_port_t port_num;           /*!< UART port number */
    uart_word_length_t data_bits;   /*!< Data bits of UART */
    uart_stop_bits_t stop_bits;     /*!< Stop bits of UART */
    uart_parity_t parity;           /*!< Parity type */
    modem_flow_ctrl_t flow_control; /*!< Flow control type */
    uint32_t baud_rate;             /*!< Communication baud rate */
} esp_modem_dte_config_t;

/**
 * @brief Type used for reception callback
 *
 */
typedef esp_err_t (*esp_modem_on_receive)(void *buffer, size_t len, void *context);

/**
 * @brief ESP Modem DTE Default Configuration
 *
 */
#define ESP_MODEM_DTE_DEFAULT_CONFIG()          \
    {                                           \
        .port_num = UART_NUM_1,                 \
        .data_bits = UART_DATA_8_BITS,          \
        .stop_bits = UART_STOP_BITS_1,          \
        .parity = UART_PARITY_DISABLE,          \
        .baud_rate = 115200,                    \
        .flow_control = MODEM_FLOW_CONTROL_NONE \
    }

/**
 * @brief Create and initialize Modem DTE object
 *
 * @param config configuration of ESP Modem DTE object
 * @return modem_dte_t*
 *      - Modem DTE object
 */
modem_dte_t *esp_modem_dte_init(const esp_modem_dte_config_t *config);

/**
 * @brief Register event handler for ESP Modem event loop
 *
 * @param dte modem_dte_t type object
 * @param handler event handler to register
 * @param handler_args arguments for registered handler
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM on allocating memory for the handler failed
 *      - ESP_ERR_INVALID_ARG on invalid combination of event base and event id
 */
esp_err_t esp_modem_set_event_handler(modem_dte_t *dte, esp_event_handler_t handler, int32_t event_id, void *handler_args);

/**
 * @brief Unregister event handler for ESP Modem event loop
 *
 * @param dte modem_dte_t type object
 * @param handler event handler to unregister
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG on invalid combination of event base and event id
 */
esp_err_t esp_modem_remove_event_handler(modem_dte_t *dte, esp_event_handler_t handler);

/**
 * @brief Setup PPP Session
 *
 * @param dte Modem DTE object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_start_ppp(modem_dte_t *dte);

/**
 * @brief Exit PPP Session
 *
 * @param dte Modem DTE Object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_stop_ppp(modem_dte_t *dte);

/**
 * @brief Setup on reception callback
 *
 * @param dte ESP Modem DTE object
 * @param receive_cb Function pointer to the reception callback
 * @param receive_cb_ctx Contextual pointer to be passed to the reception callback
 *
 * @return ESP_OK on success
 */
esp_err_t esp_modem_set_rx_cb(modem_dte_t *dte, esp_modem_on_receive receive_cb, void *receive_cb_ctx);

#ifdef __cplusplus
}
#endif
