/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#include "driver/uart.h"
#include "os/os_mbuf.h"
#include "esp_bt.h"
#include "esp_hci_transport.h"

/**
 * @brief UART configuration parameters for the HCI driver
 */
typedef struct hci_driver_uart_params_config
{
    uint8_t hci_uart_port;                   /*!< Port of UART for HCI */
    uint8_t hci_uart_data_bits;              /*!< Data bits of UART for HCI */
    uint8_t hci_uart_stop_bits;              /*!< Stop bits of UART for HCI */
    uint8_t hci_uart_flow_ctrl;              /*!< Flow control of UART for HCI */
    uint8_t hci_uart_parity;                 /*!< UART parity */
    uint8_t hci_uart_driver_mode;            /*!< UART driver mode */
    uint32_t hci_uart_baud;                  /*!< Baudrate of UART for HCI */
    int hci_uart_tx_pin;                     /*!< Tx Pin number of UART for HCI */
    int hci_uart_rx_pin;                     /*!< Rx Pin number of UART for HCI */
    int hci_uart_rts_pin;                    /*!< RTS Pin number of UART for HCI */
    int hci_uart_cts_pin;                    /*!< CTS Pin number of UART for HCI */
}  hci_driver_uart_params_config_t;

#define BT_HCI_DRIVER_UART_CONFIG_DEFAULT() {                                       \
    .hci_uart_port          = DEFAULT_BT_LE_HCI_UART_PORT,                          \
    .hci_uart_baud          = DEFAULT_BT_LE_HCI_UART_BAUD,                          \
    .hci_uart_tx_pin        = DEFAULT_BT_LE_HCI_UART_TX_PIN ,                       \
    .hci_uart_rx_pin        = DEFAULT_BT_LE_HCI_UART_RX_PIN,                        \
    .hci_uart_cts_pin       = DEFAULT_BT_LE_HCI_UART_CTS_PIN,                       \
    .hci_uart_rts_pin       = DEFAULT_BT_LE_HCI_UART_RTS_PIN,                       \
    .hci_uart_data_bits     = DEFAULT_BT_LE_HCI_UART_DATA_BITS,                     \
    .hci_uart_stop_bits     = DEFAULT_BT_LE_HCI_UART_STOP_BITS,                     \
    .hci_uart_flow_ctrl     = DEFAULT_BT_LE_HCI_UART_FLOW_CTRL,                     \
    .hci_uart_parity        = DEFAULT_BT_LE_HCI_UART_PARITY,                        \
}

/**
 * @brief Configures the HCI driver UART parameters.
 *        This function sets up the UART interface according to the specified configuration parameters.
 *
 * @param uart_config A pointer to a structure containing the UART configuration parameters.
 *                    The structure should include details such as baud rate, parity, stop bits, and flow control.
 *                    Ensure that the uart_config structure is correctly initialized before calling this function.
 *
 * @return int    Returns 0 on success, or a non-zero error code on failure.
 *
 * @note This function should be called before any UART communication is initiated.
 */
int hci_driver_uart_config(hci_driver_uart_params_config_t *uart_config);

/**
 * @brief Update the UART pin configuration for the HCI driver.
 *
 * This function updates the TX, RX, CTS, and RTS pin assignments for the HCI driver operating over UART.
 * It allows dynamic reconfiguration of UART pins as needed.
 *
 * @param tx_pin  The GPIO number assigned to the UART TX pin.
 * @param rx_pin  The GPIO number assigned to the UART RX pin.
 * @param cts_pin The GPIO number assigned to the UART CTS pin.
 * @param rts_pin The GPIO number assigned to the UART RTS pin.
 *
 * @return 0 on success, or a negative error code on failure.
 */
int hci_driver_uart_pin_update(int tx_pin, int rx_pin, int cts_pin, int rts_pin);

/**
 * @brief Retrieves the current UART configuration parameters for the HCI driver.
 *
 * @return hci_driver_uart_params_config_t* Pointer to the structure with UART configuration parameters.
 */
hci_driver_uart_params_config_t * hci_driver_uart_config_param_get(void);

#ifdef __cplusplus
}
#endif
