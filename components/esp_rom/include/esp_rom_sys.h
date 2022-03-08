/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "sdkconfig.h"
#include <stdint.h>
#include "soc/reset_reasons.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 * @brief Install UART1 as the default console channel, equivalent to `esp_rom_install_channel_putc(1, esp_rom_uart_putc)`
 */
void esp_rom_install_uart_printf(void);

/**
 * @brief Get reset reason of CPU
 *
 * @param cpu_no CPU number
 * @return Reset reason code (see in soc/reset_reasons.h)
 */
soc_reset_reason_t esp_rom_get_reset_reason(int cpu_no);

/**
 * @brief Route peripheral interrupt sources to CPU's interrupt port by matrix
 *
 * Usually there're 4 steps to use an interrupt:
 * 1. Route peripheral interrupt source to CPU. e.g.  esp_rom_route_intr_matrix(0, ETS_WIFI_MAC_INTR_SOURCE, ETS_WMAC_INUM)
 * 2. Set interrupt handler for CPU
 * 3. Enable CPU interupt
 * 4. Enable peripheral interrupt
 *
 * @param cpu_core The CPU number, which the peripheral interupt will inform to
 * @param periph_intr_id The peripheral interrupt source number
 * @param cpu_intr_num The CPU interrupt number
 */
void esp_rom_route_intr_matrix(int cpu_core, uint32_t periph_intr_id, uint32_t cpu_intr_num);

/**
 * @brief Get the real CPU ticks per us
 *
 * @return CPU ticks per us
 */
uint32_t esp_rom_get_cpu_ticks_per_us(void);

#ifdef __cplusplus
}
#endif
