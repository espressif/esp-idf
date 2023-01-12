// Copyright 2010-2020 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>
#include <stdlib.h>
#include "esp_attr.h"
#include "hal/efuse_hal.h"
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
    (void)clock_hz;
    uart_ll_set_baudrate(UART_LL_GET_HW(uart_no), baud_rate);
}

#if CONFIG_IDF_TARGET_ESP32C3
/**
 * The ESP32-C3 ROM has released two versions, one is the ECO3 version,
 * and the other is the version before ECO3 (include ECO0 ECO1 ECO2).
 * These two versions of the ROM code do not list uart_tx_switch wrap
 * function in the ROM interface, so here use the uart_tx_switch direct
 * address instead.
 */
IRAM_ATTR void esp_rom_uart_set_as_console(uint8_t uart_no)
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
