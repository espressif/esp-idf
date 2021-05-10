/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @brief Initialize console output (UART or USB)
 */
void bootloader_console_init(void);

/**
 * @brief Flush and otherwise de-initialize console output.
 */
void bootloader_console_deinit(void);

/**
 * @brief "Write character to USB" function for ets_install_putc1.
 * Only defined if USB CDC is used for console output.
 */
void bootloader_console_write_char_usb(char c);
