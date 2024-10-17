/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define ESP_ROM_HAS_CRC_LE                  (1) // ROM CRC library supports Little Endian
#define ESP_ROM_HAS_CRC_BE                  (1) // ROM CRC library supports Big Endian
#define ESP_ROM_UART_CLK_IS_XTAL            (1) // UART clock source is selected to XTAL in ROM
#define ESP_ROM_USB_SERIAL_DEVICE_NUM       (6) // The serial port ID (UART, USB, ...) of USB_SERIAL_JTAG in the ROM.
#define ESP_ROM_USB_OTG_NUM                 (5) // The serial port ID (UART, USB, ...) of USB_OTG_CDC in the ROM.
#define ESP_ROM_HAS_RETARGETABLE_LOCKING    (1) // ROM was built with retargetable locking
#define ESP_ROM_GET_CLK_FREQ                (1) // Get clk frequency with rom function `ets_get_cpu_frequency`
#define ESP_ROM_HAS_RVFPLIB                 (1) // ROM has the rvfplib
#define ESP_ROM_HAS_HAL_WDT                 (1) // ROM has the implementation of Watchdog HAL driver
#define ESP_ROM_HAS_HAL_SYSTIMER            (1) // ROM has the implementation of Systimer HAL driver
#define ESP_ROM_HAS_LAYOUT_TABLE            (1) // ROM has the layout table
#define ESP_ROM_WDT_INIT_PATCH              (1) // ROM version does not configure the clock
#define ESP_ROM_HAS_LP_ROM                  (1) // ROM also has a LP ROM placed in LP memory
#define ESP_ROM_WITHOUT_REGI2C              (1) // ROM has no regi2c APIs
#define ESP_ROM_HAS_NEWLIB                  (1) // ROM has newlib (at least parts of it) functions included
#define ESP_ROM_HAS_NEWLIB_NANO_FORMAT      (1) // ROM has the newlib nano version of formatting functions
#define ESP_ROM_HAS_NEWLIB_NANO_PRINTF_FLOAT_BUG  (1) // ROM has the printf float bug with newlib nano version
#define ESP_ROM_HAS_VERSION                 (1) // ROM has version/eco information
#define ESP_ROM_CLIC_INT_TYPE_PATCH         (1) // ROM api esprv_intc_int_set_type configuring edge type interrupt is invalid
#define ESP_ROM_HAS_OUTPUT_PUTC_FUNC        (1) // ROM has esp_rom_output_putc (or ets_write_char_uart)
