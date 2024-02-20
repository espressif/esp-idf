/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
/**
 * @def OPENTHREAD_CONFIG_PLATFORM_INFO
 *
 * The platform-specific string to insert into the OpenThread version string.
 *
 */
#define OPENTHREAD_CONFIG_PLATFORM_INFO CONFIG_IDF_TARGET

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_ASSERT_MANAGEMENT
 *
 * The assert is managed by platform defined logic when this flag is set.
 *
 */
#define OPENTHREAD_CONFIG_PLATFORM_ASSERT_MANAGEMENT 1

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
 *
 * Define to 1 to enable otPlatFlash* APIs to support non-volatile storage.
 *
 * When defined to 1, the platform MUST implement the otPlatFlash* APIs instead of the otPlatSettings* APIs.
 *
 */
#define OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE 0

/**
 * @def OPENTHREAD_CONFIG_LOG_OUTPUT
 *
 * The ESP-IDF platform provides an otPlatLog() function.
 */
#define OPENTHREAD_CONFIG_LOG_OUTPUT OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED

/**
 * @def OPENTHREAD_CONFIG_LOG_LEVEL
 *
 * The log level (used at compile time). If `OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE` is set, this defines the most
 * verbose log level possible. See `OPENTHREAD_CONFIG_LOG_LEVEL_INIT` to set the initial log level.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_LEVEL
#if CONFIG_LOG_DEFAULT_LEVEL_NONE
#define OPENTHREAD_CONFIG_LOG_LEVEL OT_LOG_LEVEL_NONE
#elif CONFIG_LOG_DEFAULT_LEVEL_ERROR
#define OPENTHREAD_CONFIG_LOG_LEVEL OT_LOG_LEVEL_CRIT
#elif CONFIG_LOG_DEFAULT_LEVEL_WARN
#define OPENTHREAD_CONFIG_LOG_LEVEL OT_LOG_LEVEL_WARN
#elif CONFIG_LOG_DEFAULT_LEVEL_INFO
#define OPENTHREAD_CONFIG_LOG_LEVEL OT_LOG_LEVEL_WARN
#elif CONFIG_LOG_DEFAULT_LEVEL_DEBUG
#define OPENTHREAD_CONFIG_LOG_LEVEL OT_LOG_LEVEL_INFO
#elif CONFIG_LOG_DEFAULT_LEVEL_VERBOSE
#define OPENTHREAD_CONFIG_LOG_LEVEL OT_LOG_LEVEL_DEBG
#endif
#endif

#define    OPENTHREAD_CONFIG_LOG_CLI 1
#define    OPENTHREAD_CONFIG_LOG_PKT_DUMP 1
#define    OPENTHREAD_CONFIG_LOG_PLATFORM 1

/**
 * @def OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS
 *
 * The number of message buffers in buffer pool
 */
#define OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS 50

/**
 * @def OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
 *
 * Define to 1 to enable Thread Test Harness reference device support.
 *
 */
#define OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE 0

/**
 * @def OPENTHREAD_CONFIG_NCP_SPI_ENABLE
 *
 * Define to 1 to enable NCP SPI support.
 *
 */
#define OPENTHREAD_CONFIG_NCP_SPI_ENABLE CONFIG_OPENTHREAD_RCP_SPI

/**
 * @def OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER
 *
 * Define to 1 to enable NCP Spinel Encrypter.
 *
 */
#define OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER 0

/**
 * @def OPENTHREAD_CONFIG_NCP_HDLC_ENABLE
 *
 * Define to 1 to enable NCP HDLC support.
 *
 */
#define OPENTHREAD_CONFIG_NCP_HDLC_ENABLE CONFIG_OPENTHREAD_RCP_UART

/**
 * @def PACKAGE_NAME
 *
 * Define to the full name of this package.
 *
 */
#define PACKAGE_NAME "openthread-esp32"

/**
 * @def PACKAGE_STRING
 *
 * Define to the full name and version of this package.
 *
 */
#define PACKAGE_STRING (PACKAGE_NAME " - " PACKAGE_VERSION)

/**
 * @def OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS
 *
 * Define as 1 to enable bultin-mbedtls.
 *
 * Note that the OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS determines whether to use bultin-mbedtls as well as
 * whether to manage mbedTLS internally, such as memory allocation and debug.
 *
 */
#define OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS 0

/**
 * @def OPENTHREAD_CONFIG_MAX_STATECHANGE_HANDLERS
 *
 * The maximum number of state-changed callback handlers (set using `otSetStateChangedCallback()`).
 *
 */
#define OPENTHREAD_CONFIG_MAX_STATECHANGE_HANDLERS 3

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_RADIO_SPINEL_RX_FRAME_BUFFER_SIZE
 *
 * Specifies the rx frame buffer size used by `SpinelInterface` in RCP host code. This is applicable/used when
 * `RadioSpinel` platform is used.
 *
 */
#define OPENTHREAD_CONFIG_PLATFORM_RADIO_SPINEL_RX_FRAME_BUFFER_SIZE 1024

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
 *
 * Define as 1 to enable microsecond timer.
 *
 */
#define OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_CSMA_BACKOFF_ENABLE
 *
 * Define to 1 to enable software CSMA-CA backoff logic.
 *
 */
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_CSMA_BACKOFF_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE
 *
 * Define to 1 to enable software retransmission logic.
 *
 */
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_SECURITY_ENABLE
 *
 * Define to 1 to enable software transmission security logic.
 *
 */
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_SECURITY_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_TIMING_ENABLE
 *
 * Define to 1 to enable software transmission target time logic.
 *
 */
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_TIMING_ENABLE 1

/**
 * The configurable definitions via Kconfig
 */
#if CONFIG_OPENTHREAD_COMMISSIONER
#error "Commissioner shouldn't be enabled for RCP"
#endif

#if CONFIG_OPENTHREAD_JOINER
#error "Joiner shouldn't be enabled for RCP"
#endif

#if CONFIG_OPENTHREAD_DIAG
#define OPENTHREAD_CONFIG_DIAG_ENABLE 1
#endif

#define OPENTHREAD_RADIO 1

#if CONFIG_OPENTHREAD_LINK_METRICS
/**
 * @def OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
 *
 * Define as 1 to support Thread 1.2 Link Metrics Subject feature.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
#define OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE 1
#endif
#endif //CONFIG_OPENTHREAD_LINK_METRICS

#if CONFIG_OPENTHREAD_NCP_VENDOR_HOOK
/**
 * @def OPENTHREAD_ENABLE_NCP_VENDOR_HOOK
 *
 * Define as 1 to support ESP OpenThread NCP vendor commands
 *
 */
#ifndef OPENTHREAD_ENABLE_NCP_VENDOR_HOOK
#define OPENTHREAD_ENABLE_NCP_VENDOR_HOOK 1
#endif
#endif //CONFIG_OPENTHREAD_NCP_VENDOR_HOOK
