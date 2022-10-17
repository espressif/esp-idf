/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdlib.h>
#include "esp_attr.h"
#include "sdkconfig.h"
#include "hal/uart_ll.h"

#if CONFIG_IDF_TARGET_ESP32
/**
 * The function defined in ROM code has a bug, so we re-implement it here.
 */
IRAM_ATTR void esp_rom_uart_tx_wait_idle(uint8_t uart_no)
{
    while (!uart_ll_is_tx_idle(UART_LL_GET_HW(uart_no))) {};
}
#endif

IRAM_ATTR void esp_rom_uart_set_clock_baudrate(uint8_t uart_no, uint32_t clock_hz, uint32_t baud_rate)
{
    uart_ll_set_baudrate(UART_LL_GET_HW(uart_no), baud_rate, clock_hz);
}
