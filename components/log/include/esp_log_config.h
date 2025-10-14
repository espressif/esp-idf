/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <inttypes.h>
#include "esp_log_level.h"
#include "esp_assert.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @cond */
/// Log version macros.
#define ESP_LOG_V2                               (1)
#if BOOTLOADER_BUILD
#define ESP_LOG_VERSION                          (CONFIG_BOOTLOADER_LOG_VERSION)
#else
#define ESP_LOG_VERSION                          (CONFIG_LOG_VERSION)
#endif

/**
 * This define controls the log configuration options, particularly when the built project operates in constrained environments.
 *
 * For the bootloader build:
 *   It is always set to "1" because certain log features are unnecessary, reducing the binary size.
 *
 * For the application build:
 *   If this define is set in the user namespace, all logs in the specific file will use a simplified path for logging.
 *
 * This define determines whether the "constrained_env" flag will be enabled in ESP_LOG_CONFIGS_DEFAULT, indicating that logging is capable of functioning
 * in constrained environments such as the bootloader, ISR, startup code, early log, or when the cache is disabled. In these cases,
 * `esp_log()` utilizes a simplified implementation to output logs.
 */
#if BOOTLOADER_BUILD
#define ESP_LOG_CONSTRAINED_ENV                  (1)
#else // !BOOTLOADER_BUILD
#ifndef ESP_LOG_CONSTRAINED_ENV
#define ESP_LOG_CONSTRAINED_ENV                  (0)
#endif
#endif // !BOOTLOADER_BUILD

/**
 * This define helps control log configuration options, specifically whether color output will be appended to log messages.
 * If this define is not set in the user namespace, its value will be determined by Kconfig options.
 * If the define is set in the user namespace, it will override the default configurations (ESP_LOG_CONFIGS_DEFAULT) for the user's specific file.
 * This define controls the "dis_color" flag, which is included in ESP_LOG_CONFIGS_DEFAULT
 */
#ifndef ESP_LOG_COLOR_DISABLED
#if (!BOOTLOADER_BUILD && CONFIG_LOG_COLORS) || (BOOTLOADER_BUILD && CONFIG_BOOTLOADER_LOG_COLORS)
#define ESP_LOG_COLOR_DISABLED                   (0)
#else
#define ESP_LOG_COLOR_DISABLED                   (1)
#endif
#endif

/**
 * This define helps control log configuration options, specifically whether timestamp will be appended to log message.
 * If this define is not set in the user namespace, its value will be determined by Kconfig options.
 * If the define is set in the user namespace, it will override the default configurations (ESP_LOG_CONFIGS_DEFAULT) for the user's specific file.
 * This define controls the "dis_timestamp" flag, which is included in ESP_LOG_CONFIGS_DEFAULT
 */
#ifndef ESP_LOG_TIMESTAMP_DISABLED
#if (!BOOTLOADER_BUILD && CONFIG_LOG_TIMESTAMP_SOURCE_NONE) || (BOOTLOADER_BUILD && CONFIG_BOOTLOADER_LOG_TIMESTAMP_SOURCE_NONE)
#define ESP_LOG_TIMESTAMP_DISABLED               (1)
#else
#define ESP_LOG_TIMESTAMP_DISABLED               (0)
#endif
#endif

/** @cond */
#if (!BOOTLOADER_BUILD && CONFIG_LOG_MODE_TEXT_EN) || (BOOTLOADER_BUILD && CONFIG_BOOTLOADER_LOG_MODE_TEXT_EN)
#define ESP_LOG_MODE_TEXT_EN                   (1)
#else
#define ESP_LOG_MODE_TEXT_EN                   (0)
#endif
/** @endcond */

/**
 * This define helps control log configuration options, specifically whether binary mode will be enabled.
 * If this define is not set in the user namespace, its value will be determined by Kconfig options.
 * If the define is set in the user namespace, it will override the default configurations (ESP_LOG_CONFIGS_DEFAULT) for the user's specific file.
 * This define controls the "binary_mode" flag, which is included in ESP_LOG_CONFIGS_DEFAULT
 */
#ifndef ESP_LOG_MODE_BINARY_EN
#if (!BOOTLOADER_BUILD && CONFIG_LOG_MODE_BINARY_EN) || (BOOTLOADER_BUILD && CONFIG_BOOTLOADER_LOG_MODE_BINARY_EN)
#define ESP_LOG_MODE_BINARY_EN (1)
#else
#define ESP_LOG_MODE_BINARY_EN (0)
#endif
#elif ESP_LOG_MODE_TEXT_EN == 1 && ESP_LOG_MODE_BINARY_EN == 1
// Text logging mode is enabled via CONFIG option, but binary mode is enabled via ESP_LOG_MODE_BINARY_EN define in the user namespace.
// Suppressing binary logging mode.
#undef ESP_LOG_MODE_BINARY_EN
#define ESP_LOG_MODE_BINARY_EN (0)
#endif

/**
 * This define helps control log configuration options, specifically whether formatting (ex.: color, timestamp) will be appended to log message.
 * If the define is set in the user namespace, it will override the default configurations (ESP_LOG_CONFIGS_DEFAULT) for the user's specific file.
 * This define controls the "require_formatting" flag, which is included in ESP_LOG_CONFIGS_DEFAULT
 */
#ifndef ESP_LOG_FORMATTING_DISABLED
#define ESP_LOG_FORMATTING_DISABLED              (0)
#endif
/** @endcond */

/**
 * @brief Logging configuration structure for ESP log.
 */
typedef struct {
    union {
        struct {
            esp_log_level_t log_level: ESP_LOG_LEVEL_LEN; /*!< Log level */
            uint32_t constrained_env: 1;                  /*!< Flag indicating if logging is from a constrained environment (e.g., bootloader, ISR, startup code, early log, or when the cache is disabled). In such cases, esp_rom_vprintf is used instead of the vprintf. */
            uint32_t require_formatting: 1;               /*!< Flag specifying whether the log message needs additional formatting. If set, esp_log() will add formatting elements like color, timestamp, and tag to the log message. */
            uint32_t dis_color: 1;                        /*!< Flag to disable color in log output. If set, log messages will not include color codes. */
            uint32_t dis_timestamp: 1;                    /*!< Flag to disable timestamps in log output. If set, log messages will not include timestamps. */
            uint32_t binary_mode : 1;                     /*!< Flag to indicate binary mode. */
            uint32_t reserved: 24;                        /*!< Reserved for future use. Should be initialized to 0. */
        } opts;
        uint32_t data;                                    /*!< Raw data representing all options in a 32-bit word. */
    };
} esp_log_config_t;

/** @cond */
#define ESP_LOG_OFFSET_CONSTRAINED_ENV           (ESP_LOG_LEVEL_LEN) /*!< Offset for constrained_env field from esp_log_config_t */
#define ESP_LOG_OFFSET_REQUIRE_FORMATTING        (4) /*!< Offset for require_formatting field from esp_log_config_t */
#define ESP_LOG_OFFSET_DIS_COLOR_OFFSET          (5) /*!< Offset for dis_color field from esp_log_config_t */
#define ESP_LOG_OFFSET_DIS_TIMESTAMP             (6) /*!< Offset for dis_timestamp field from esp_log_config_t */
#define ESP_LOG_OFFSET_BINARY_MODE               (7) /*!< Offset for binary_mode field from esp_log_config_t */

ESP_STATIC_ASSERT(ESP_LOG_OFFSET_CONSTRAINED_ENV == ESP_LOG_LEVEL_LEN, "The log level should not overlap the following fields in esp_log_config_t");
/** @endcond */

#define ESP_LOG_CONFIG_LEVEL_MASK                ((1 << ESP_LOG_LEVEL_LEN) - 1) /*!< Mask for level field in esp_log_config_t */
#define ESP_LOG_CONFIG_CONSTRAINED_ENV           (1 << ESP_LOG_OFFSET_CONSTRAINED_ENV) /*!< Value for constrained_env field in esp_log_config_t */
#define ESP_LOG_CONFIG_REQUIRE_FORMATTING        (1 << ESP_LOG_OFFSET_REQUIRE_FORMATTING) /*!< Value for require_formatting field in esp_log_config_t */
#define ESP_LOG_CONFIG_DIS_COLOR                 (1 << ESP_LOG_OFFSET_DIS_COLOR_OFFSET)  /*!< Value for dis_color field in esp_log_config_t */
#define ESP_LOG_CONFIG_DIS_TIMESTAMP             (1 << ESP_LOG_OFFSET_DIS_TIMESTAMP)  /*!< Value for dis_timestamp field in esp_log_config_t */
#define ESP_LOG_CONFIG_BINARY_MODE               (1 << ESP_LOG_OFFSET_BINARY_MODE) /*!< Value for binary_mode field in esp_log_config_t */

/**
 * @brief Macro for setting log configurations according to selected Kconfig options.
 *
 * @note The `require_formatting` flag is always set for logv2.
 */
#ifndef ESP_LOG_CONFIGS_DEFAULT
#define ESP_LOG_CONFIGS_DEFAULT ( \
      ((ESP_LOG_CONSTRAINED_ENV)     ? (ESP_LOG_CONFIG_CONSTRAINED_ENV)    : 0) \
    | ((ESP_LOG_FORMATTING_DISABLED) ? (0) : (ESP_LOG_CONFIG_REQUIRE_FORMATTING)) \
    | ((ESP_LOG_COLOR_DISABLED)      ? (ESP_LOG_CONFIG_DIS_COLOR)          : 0) \
    | ((ESP_LOG_TIMESTAMP_DISABLED)  ? (ESP_LOG_CONFIG_DIS_TIMESTAMP)      : 0) \
    | ((ESP_LOG_MODE_BINARY_EN)      ? (ESP_LOG_CONFIG_BINARY_MODE)        : 0))
#endif

/**
 * @brief Macro to initialize an `esp_log_config_t` structure.
 *
 * This macro directly initializes an `esp_log_config_t` structure by setting the raw `data` field
 * with the provided `configs` parameter. It is useful when you have already defined the configuration
 * settings and wish to apply them directly.
 *
 * @param configs The raw configuration data to initialize the `esp_log_config_t` structure.
 *
 * @return An initialized `esp_log_config_t` structure containing the specified configuration data.
 */
#if defined(__cplusplus)
#define ESP_LOG_CONFIG_INIT(configs) (__extension__({ \
    esp_log_config_t __esp_log_config; \
    __esp_log_config.data = (configs); \
    __esp_log_config;}))
#else
#define ESP_LOG_CONFIG_INIT(configs) ((esp_log_config_t){.data = (configs)})
#endif

#ifdef __cplusplus
}
#endif
