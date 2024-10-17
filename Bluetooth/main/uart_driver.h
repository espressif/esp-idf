/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __UART_DRIVER_H__
#define __UART_DRIVER_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp_bluedroid_hci.h"

/**
 * @brief open HCI transport of uart
 */
void hci_uart_open(void);

/**
 * @brief close HCI transport of uart
 */
void hci_uart_close(void);

/**
 * @brief send data from host to HCI transport
 *
 * @param[in] data  pointer to data buffer
 * @param[in] len   length of data
 */
void hci_uart_send(uint8_t *data, uint16_t len);

/**
 * @brief host checks whether it can send data to HCI transport
 *
 * @return true if host can send data, false otherwise
 */
bool hci_check_send_available(void);

/**
 * @brief register host callbacks
 *
 * @param[in] callback  HCI driver callbacks
 */
esp_err_t hci_register_host_callback(const esp_bluedroid_hci_driver_callbacks_t *callback);

#endif /* __UART_DRIVER_H__ */
