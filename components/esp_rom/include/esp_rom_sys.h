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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Print formated string to console device
 * @note float and long long data are not supported!
 *
 * @param fmt Format string
 * @param ... Additional arguments, depending on the format string
 * @return int: Total number of characters written on success; A negative number on failure.
 */
int esp_rom_printf(const char *fmt, ...);

/**
 * @brief Pauses execution for us microseconds
 *
 * @param us Number of microseconds to pause
 */
void esp_rom_delay_us(uint32_t us);

/**
 * @brief esp_rom_printf can print message to different channels simultaneously.
 *        This function can help install the low level putc function for esp_rom_printf.
 * 
 * @param channel Channel number (startting from 1)
 * @param putc Function pointer to the putc implementation. Set NULL can disconnect esp_rom_printf with putc.
 */
void esp_rom_install_channel_putc(int channel, void (*putc)(char c));

/**
 *  @brief Disable logging from the ROM code.
 */
void esp_rom_disable_logging(void);

#ifdef __cplusplus
}
#endif
