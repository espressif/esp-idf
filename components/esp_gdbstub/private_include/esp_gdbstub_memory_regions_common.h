/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
#include "soc/reg_base.h"
#include "soc/usb_serial_jtag_struct.h"
#else
#include "soc/uart_reg.h"
#include "soc/uart_struct.h"
#endif


static inline bool is_transport_memory_region(intptr_t addr)
{
#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    return addr >= DR_REG_USB_SERIAL_JTAG_BASE &&
           addr <= DR_REG_USB_SERIAL_JTAG_BASE + sizeof(USB_SERIAL_JTAG);
#else
    return addr >= REG_UART_BASE(CONFIG_ESP_CONSOLE_UART_NUM) &&
           addr <= REG_UART_BASE(CONFIG_ESP_CONSOLE_UART_NUM) + sizeof(UART0);
#endif
}
