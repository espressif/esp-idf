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
#include "sdkconfig.h"
#include "hal/uart_ll.h"
#include "soc/uart_struct.h"

#if CONFIG_IDF_TARGET_ESP32
/**
 * The function defined in ROM code has a bug, so we re-implement it here.
 */
IRAM_ATTR void esp_rom_uart_tx_wait_idle(uint8_t uart_no)
{
    uart_dev_t *device = NULL;
    switch (uart_no) {
    case 0:
        device = &UART0;
        break;
    case 1:
        device = &UART1;
        break;
    default:
        device = &UART2;
        break;
    }
    while (!uart_ll_is_tx_idle(device));
}
#endif

IRAM_ATTR void esp_rom_uart_set_clock_baudrate(uint8_t uart_no, uint32_t clock_hz, uint32_t baud_rate)
{
    extern void uart_div_modify(uint8_t uart_no, uint32_t DivLatchValue);
    uart_div_modify(uart_no, (clock_hz << 4) / baud_rate);
}
