/*
 * SPDX-FileCopyrightText: 2010-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdlib.h>
#include "esp_attr.h"
#include "sdkconfig.h"
#include "hal/uart_ll.h"
#include "hal/efuse_hal.h"
#include "esp_rom_caps.h"
#include "rom/uart.h"

#if CONFIG_IDF_TARGET_ESP32
/**
 * The function defined in ROM code has a bug, so we re-implement it here.
 */
IRAM_ATTR void esp_rom_output_tx_wait_idle(uint8_t uart_no)
{
    while (!uart_ll_is_tx_idle(UART_LL_GET_HW(uart_no))) {};
}
#endif

#if CONFIG_IDF_TARGET_ESP32C3
/**
 * The ESP32-C3 ROM has released two versions, one is the ECO3 version,
 * and the other is the version before ECO3 (include ECO0 ECO1 ECO2).
 * These two versions of the ROM code do not list uart_tx_switch wrap
 * function in the ROM interface, so here use the uart_tx_switch direct
 * address instead.
 */
IRAM_ATTR void esp_rom_output_set_as_console(uint8_t uart_no)
{
    typedef void (*rom_func_t)(uint8_t);
    rom_func_t uart_tx_switch = NULL;

    if (efuse_hal_chip_revision() < 3) {
        uart_tx_switch = (rom_func_t)0x4004b8ca;
    } else {
        uart_tx_switch = (rom_func_t)0x4004c166;
    }
    uart_tx_switch(uart_no);
}
#endif

#if !ESP_ROM_HAS_UART_BUF_SWITCH
IRAM_ATTR void esp_rom_output_switch_buffer(uint8_t uart_no)
{
    UartDevice *uart = GetUartDevice();
    uart->buff_uart_no = uart_no;
}
#endif // !ESP_ROM_HAS_UART_BUF_SWITCH
