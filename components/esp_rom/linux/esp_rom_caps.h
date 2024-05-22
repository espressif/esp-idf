/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define ESP_ROM_USB_OTG_NUM                 (-1) // No USB_OTG CDC in the ROM, set -1 for Kconfig usage.
#define ESP_ROM_USB_SERIAL_DEVICE_NUM       (-1) // No USB_SERIAL_JTAG in the ROM, set -1 for Kconfig usage.
#define ESP_ROM_HAS_ETS_PRINTF_BUG          (1)  // The build will include esp_rom_install_uart_printf, which is used for host tests.
#define ESP_ROM_HAS_OUTPUT_PUTC_FUNC        (1)  // The Linux target has an implementation of esp_rom_output_putc.
