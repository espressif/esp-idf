/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef H_ESP_HCI_DRIVER_
#define H_ESP_HCI_DRIVER_
#include <stdint.h>

/**
 * @brief Enumeration of HCI transport direction.
 */
typedef enum {
    HCI_DRIVER_DIR_C2H = 0x00,  ///< From controller to host.
    HCI_DRIVER_DIR_H2C,         ///< From host to controller.
} hci_driver_direction_t;

typedef enum {
    HCI_DRIVER_TYPE_CMD = 0x01, ///< HCI Command Indicator.
    HCI_DRIVER_TYPE_ACL,        ///< HCI ACL Data Indicator.
    HCI_DRIVER_TYPE_SYNC,       ///< HCI Synchronous Data Indicator.
    HCI_DRIVER_TYPE_EVT,        ///< HCI Event Indicator.
    HCI_DRIVER_TYPE_ISO,        ///< HCI Isochronous Data Indicator.
    HCI_DRIVER_TYPE_VENDOR,     ///< HCI Vendor data Indicator.
} hci_driver_data_type_t;

typedef int hci_driver_forward_fn(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length,
                                  hci_driver_direction_t dir);

/**
 *  @brief Structure of HCI driver operations.
 */
typedef struct hci_driver_ops {
    int (*hci_driver_tx)(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length,
                         hci_driver_direction_t dir);
    int (*hci_driver_init)(hci_driver_forward_fn *cb);
    void (*hci_driver_deinit)(void);
} hci_driver_ops_t;


#if CONFIG_BT_LE_HCI_INTERFACE_USE_RAM
extern hci_driver_ops_t hci_driver_vhci_ops;
#endif // CONFIG_BT_LE_HCI_INTERFACE_USE_RAM

#if CONFIG_BT_LE_HCI_INTERFACE_USE_UART
#if CONFIG_BT_LE_UART_HCI_DMA_MODE
extern hci_driver_ops_t hci_driver_uart_dma_ops;
/**
 * @brief Reconfigure the UART pins for the HCI driver.
 *
 * This function changes the UART pin configuration for the HCI driver.
 *
 * @param tx_pin   The pin number for the UART TX (transmit) line.
 * @param rx_pin   The pin number for the UART RX (receive) line.
 * @param cts_pin  The pin number for the UART CTS (clear to send) line.
 * @param rts_pin  The pin number for the UART RTS (request to send) line.
 *
 * @return int     Returns 0 on success, or a negative error code on failure.
 */
int hci_driver_uart_dma_reconfig_pin(int tx_pin, int rx_pin, int cts_pin, int rts_pin);
#define hci_uart_reconfig_pin               hci_driver_uart_dma_reconfig_pin
#else
extern hci_driver_ops_t hci_driver_uart_ops;
/**
 * @brief Reconfigure the UART pins for the HCI driver.
 *
 * This function changes the UART pin configuration for the HCI driver.
 *
 * @param tx_pin   The pin number for the UART TX (transmit) line.
 * @param rx_pin   The pin number for the UART RX (receive) line.
 * @param cts_pin  The pin number for the UART CTS (clear to send) line.
 * @param rts_pin  The pin number for the UART RTS (request to send) line.
 *
 * @return int     Returns 0 on success, or a negative error code on failure.
 */
int hci_driver_uart_reconfig_pin(int tx_pin, int rx_pin, int cts_pin, int rts_pin);
#define hci_uart_reconfig_pin               hci_driver_uart_reconfig_pin
#endif // CONFIG_BT_LE_UART_HCI_DMA_MODE
#endif // CONFIG_BT_LE_HCI_INTERFACE_USE_UART

#endif // H_ESP_HCI_DRIVER_
