/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include <stdarg.h>
#include "esp_log_config.h"
#include "esp_log_write.h"
#include "esp_rom_sys.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Outputs a formatted log message.
 *
 * @note It internally calls a vprintf function that was set by esp_log_set_vprintf().
 * @see esp_log_set_vprintf
 *
 * @param config    The config log
 * @param format    The format string for the log message.
 * @param args      The variable argument list containing the values to be formatted.
 */
__attribute__((always_inline)) static inline void esp_log_vprintf(esp_log_config_t config, const char *format, va_list args)
{
#if BOOTLOADER_BUILD
    esp_rom_vprintf(format, args);
#else // APP
    extern vprintf_like_t esp_log_vprint_func;
#if ESP_LOG_VERSION == 2
    vprintf_like_t vprint_func[2] = {
        esp_log_vprint_func,
        esp_rom_vprintf,
    };
    vprint_func[config.opts.constrained_env](format, args);
#else // ESP_LOG_VERSION == 1
    esp_log_vprint_func(format, args);
#endif // ESP_LOG_VERSION == 1
#endif // APP
}

/**
 * @brief Outputs a formatted log message.
 *
 * @note It internally calls esp_log_vprintf().
 * @see esp_log_vprintf
 *
 * @param config    The config log
 * @param format    The format string for the log message.
 * @param ...       Optional arguments to be formatted according to the format string.
 */
void esp_log_printf(esp_log_config_t config, const char *format, ...) __attribute__((format(printf, 2, 3)));

#ifdef __cplusplus
}
#endif
