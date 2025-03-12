/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Output a char over usb-serial if printing has not been disabled
 *
 * @param c the char to output
 */
void esp_rom_usb_serial_putc(char c);

#ifdef __cplusplus
}
#endif
