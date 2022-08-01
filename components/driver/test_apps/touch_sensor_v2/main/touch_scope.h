/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @brief Send data info to digital scope.
 * @param data Pointer to send buff.
 * @param channel_num The number of channels to be displayed.
 * @return the length of successfully transmitted data.
 */
int test_tp_print_to_scope(float *data, unsigned char channel_num);

/**
 * @brief Initialize the UART so that the touch sensor data is output to the digital scope.
 * @return
 *      - ESP_OK Success
 *      - ESP_FAIL UART error
 */
esp_err_t test_tp_scope_debug_init(uint8_t uart_num, int tx_io_num, int rx_io_num, int baud_rate);
