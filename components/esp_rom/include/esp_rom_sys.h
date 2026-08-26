/*
 * SPDX-FileCopyrightText: 2010-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "sdkconfig.h"
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdarg.h>
#include "soc/reset_reasons.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief Software Reset digital core include RTC.
  *
  * It is not recommended to use this function in esp-idf, use
  * esp_restart() instead.
  */
void esp_rom_software_reset_system(void);

/**
  * @brief Software Reset cpu core.
  *
  * It is not recommended to use this function in esp-idf, use
  * esp_restart() instead.
  *
  * @param  cpu_no : The CPU to reset, 0 for PRO CPU, 1 for APP CPU.
  */
void esp_rom_software_reset_cpu(int cpu_no);

/**
 * @brief Print formatted string to console device
 * @note float and long long data are not supported!
 *
 * @param fmt Format string
 * @param ... Additional arguments, depending on the format string
 * @return int: Total number of characters written on success; A negative number on failure.
 */
int esp_rom_printf(const char *fmt, ...);

/**
 * @brief Print formatted string to console device
 * @note float and long long data are not supported!
 *
 * @param fmt Format string
 * @param ap List of arguments.
 * @return int: Total number of characters written on success; A negative number on failure.
 */
int esp_rom_vprintf(const char *fmt, va_list ap);

/**
 * @brief Convert an unsigned integer value to a string representation in the specified radix.
 *
 * This function converts the given unsigned integer value to a string representation in the specified radix.
 * The resulting string is stored in the provided character buffer `buf`.
 *
 * @param[in] val    The unsigned integer value to be converted.
 * @param[in] radix  The base of the numeral system to be used for the conversion.
 *                   It determines the number of unique digits in the numeral system
 *                   (e.g., 2 for binary, 10 for decimal, 16 for hexadecimal).
 * @param[in] pad    The optional padding width (0 - unused) for the resulting string. It adds zero-padding.
 *                   (val=123, pad=6 -> result=000123).
 * @param[in] digits Pointer to a character array representing the digits of the
 *                   numeral system. The array must contain characters in the order of increasing
 *                   values, corresponding to the digits of the radix. For example, "0123456789ABCDEF"
 *                   or hexadecimal.
 * @param[out] buf   Pointer to the character buffer where the resulting string will
 *                   be stored. The buffer must have enough space to accommodate the entire converted
 *                   string, including the null-terminator.
 *
 * @return The length of the resulting string (excluding the null-terminator).
 *
 * @note The buffer `buf` must have sufficient space to hold the entire converted string, including the null-terminator.
 *       The caller is responsible for ensuring the buffer's size is large enough to prevent buffer overflow.
 * @note The provided `digits` array must have enough elements to cover the entire radix used for conversion. Otherwise, undefined behavior may occur.
 */
int esp_rom_cvt(unsigned long long val, long radix, int pad, const char *digits, char *buf);

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
 * @param channel Channel number (starting from 1)
 * @param putc Function pointer to the putc implementation. Set NULL can disconnect esp_rom_printf with putc.
 */
void esp_rom_install_channel_putc(int channel, void (*putc)(char c));

/**
 * @brief It outputs a character to different channels simultaneously.
 *        This function is used by esp_rom_printf/esp_rom_vprintf.
 *
 * @param c Char to output.
 */
void esp_rom_output_to_channels(char c);

/**
 * @brief Install UART1 as the default console channel, equivalent to `esp_rom_install_channel_putc(1, esp_rom_output_putc)`
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
 * 3. Enable CPU interrupt
 * 4. Enable peripheral interrupt
 *
 * @param cpu_core The CPU number, which the peripheral interrupt will inform to
 * @param periph_intr_id The peripheral interrupt source number
 * @param cpu_intr_num The CPU (external) interrupt number. On targets that use CLIC as their interrupt controller,
 *                     this number represents the external interrupt number. For example, passing `cpu_intr_num = i`
 *                     to this function would in fact bind peripheral source to CPU interrupt `CLIC_EXT_INTR_NUM_OFFSET + i`.
 */
void esp_rom_route_intr_matrix(int cpu_core, uint32_t periph_intr_id, uint32_t cpu_intr_num);

/**
 * @brief Get the real CPU ticks per us
 *
 * @return CPU ticks per us
 */
uint32_t esp_rom_get_cpu_ticks_per_us(void);

/**
 * @brief Set the real CPU tick rate
 *
 * @note Call this function when CPU frequency is changed, otherwise the `esp_rom_delay_us` can be inaccurate.
 *
 * @param ticks_per_us CPU ticks per us
 */
void esp_rom_set_cpu_ticks_per_us(uint32_t ticks_per_us);

#if SOC_RECOVERY_BOOTLOADER_SUPPORTED || __DOXYGEN__
/**
 * @brief Returns the offset from which the bootloader image is used to load.
 *
 * The offset can point to either the PRIMARY or RECOVERY bootloader.
 *
 * @return The offset of the active bootloader.
 */
uint32_t esp_rom_get_bootloader_offset(void);
#endif // SOC_RECOVERY_BOOTLOADER_SUPPORTED

#ifdef __cplusplus
}
#endif
