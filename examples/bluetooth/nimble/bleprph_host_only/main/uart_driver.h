/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __UART_DRIVER_H__
#define __UART_DRIVER_H__

#include <stdint.h>
#include <stdbool.h>

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

#endif /* __UART_DRIVER_H__ */
