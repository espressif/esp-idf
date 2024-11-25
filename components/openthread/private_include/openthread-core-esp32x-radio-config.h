/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#if CONFIG_OPENTHREAD_HEADER_CUSTOM
#include CONFIG_OPENTHREAD_CUSTOM_HEADER_FILE_NAME
#endif

#define OPENTHREAD_RADIO 1

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_INFO
 *
 * The platform-specific string to insert into the OpenThread version string.
 *
 */
#ifdef OPENTHREAD_CONFIG_PLATFORM_INFO
#error `OPENTHREAD_CONFIG_PLATFORM_INFO` is redefined.
#endif
#define OPENTHREAD_CONFIG_PLATFORM_INFO CONFIG_OPENTHREAD_PLATFORM_INFO

/**
 * @def PACKAGE_NAME
 *
 * Define to the full name of this package.
 *
 */
#ifdef PACKAGE_NAME
#error `PACKAGE_NAME` is redefined.
#endif
#define PACKAGE_NAME CONFIG_OPENTHREAD_PACKAGE_NAME

/**
 * @def PACKAGE_STRING
 *
 * Define to the full name and version of this package.
 *
 */
#ifdef PACKAGE_STRING
#error `PACKAGE_STRING` is redefined.
#endif
#define PACKAGE_STRING (PACKAGE_NAME " - " PACKAGE_VERSION)

/**
 * @def OPENTHREAD_CONFIG_LOG_LEVEL
 *
 * The log level (used at compile time). If `OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE` is set, this defines the most
 * verbose log level possible. See `OPENTHREAD_CONFIG_LOG_LEVEL_INIT` to set the initial log level.
 *
 */
#ifdef OPENTHREAD_CONFIG_LOG_LEVEL
#error `OPENTHREAD_CONFIG_LOG_LEVEL` is redefined.
#endif
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

/**
 * @def OPENTHREAD_CONFIG_NCP_SPI_ENABLE
 *
 * Define to 1 to enable NCP SPI support.
 *
 */
#ifdef OPENTHREAD_CONFIG_NCP_SPI_ENABLE
#error `OPENTHREAD_CONFIG_NCP_SPI_ENABLE` is redefined.
#endif
#define OPENTHREAD_CONFIG_NCP_SPI_ENABLE CONFIG_OPENTHREAD_RCP_SPI

/**
 * @def OPENTHREAD_CONFIG_NCP_HDLC_ENABLE
 *
 * Define to 1 to enable NCP HDLC support.
 *
 */
#ifdef OPENTHREAD_CONFIG_NCP_HDLC_ENABLE
#error `OPENTHREAD_CONFIG_NCP_HDLC_ENABLE` is redefined.
#endif
#define OPENTHREAD_CONFIG_NCP_HDLC_ENABLE CONFIG_OPENTHREAD_RCP_UART

/**
 * @def OPENTHREAD_LIB_SPINEL_RX_FRAME_BUFFER_SIZE
 *
 * Specifies the rx frame buffer size used by `SpinelInterface` in RCP host code. This is applicable/used when
 * `RadioSpinel` platform is used.
 *
 */
#ifdef OPENTHREAD_LIB_SPINEL_RX_FRAME_BUFFER_SIZE
#error `OPENTHREAD_LIB_SPINEL_RX_FRAME_BUFFER_SIZE` is redefined.
#endif
#define OPENTHREAD_LIB_SPINEL_RX_FRAME_BUFFER_SIZE CONFIG_OPENTHREAD_SPINEL_RX_FRAME_BUFFER_SIZE

#if CONFIG_OPENTHREAD_COMMISSIONER
#error "Commissioner shouldn't be enabled for RCP"
#endif

#if CONFIG_OPENTHREAD_JOINER
#error "Joiner shouldn't be enabled for RCP"
#endif

#ifdef OPENTHREAD_CONFIG_DIAG_ENABLE
#error `OPENTHREAD_CONFIG_DIAG_ENABLE` is redefined.
#endif
#if CONFIG_OPENTHREAD_DIAG
#define OPENTHREAD_CONFIG_DIAG_ENABLE 1
#else
#define OPENTHREAD_CONFIG_DIAG_ENABLE 0
#endif

#ifdef OPENTHREAD_ENABLE_NCP_VENDOR_HOOK
#error `OPENTHREAD_ENABLE_NCP_VENDOR_HOOK` is redefined.
#endif
#if CONFIG_OPENTHREAD_NCP_VENDOR_HOOK
/**
 * @def OPENTHREAD_ENABLE_NCP_VENDOR_HOOK
 *
 * Define as 1 to support ESP OpenThread NCP vendor commands
 *
 */
#define OPENTHREAD_ENABLE_NCP_VENDOR_HOOK 1
#else
#define OPENTHREAD_ENABLE_NCP_VENDOR_HOOK 0
#endif //CONFIG_OPENTHREAD_NCP_VENDOR_HOOK

/**
 * @def OPENTHREAD_CONFIG_MAC_MAX_CSMA_BACKOFFS_DIRECT
 *
 * The maximum number of backoffs the CSMA-CA algorithm will attempt before declaring a channel access failure.
 *
 * Equivalent to macMaxCSMABackoffs in IEEE 802.15.4-2006, default value is 4.
 *
 */
#ifdef OPENTHREAD_CONFIG_MAC_MAX_CSMA_BACKOFFS_DIRECT
#error `OPENTHREAD_CONFIG_MAC_MAX_CSMA_BACKOFFS_DIRECT` is redefined.
#endif
#define OPENTHREAD_CONFIG_MAC_MAX_CSMA_BACKOFFS_DIRECT CONFIG_OPENTHREAD_MAC_MAX_CSMA_BACKOFFS_DIRECT

/**
 * @def OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
 *
 * Define as 1 to enable the time synchronization service feature.
 *
 */
#ifdef OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
#error `OPENTHREAD_CONFIG_TIME_SYNC_ENABLE` is redefined.
#endif
#if CONFIG_OPENTHREAD_TIME_SYNC
#define OPENTHREAD_CONFIG_TIME_SYNC_ENABLE 1
#else
#define OPENTHREAD_CONFIG_TIME_SYNC_ENABLE 0
#endif

/*----The following options set fixed default values but can be overridden by the user header file.----*/

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

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_ASSERT_MANAGEMENT
 *
 * The assert is managed by platform defined logic when this flag is set.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_ASSERT_MANAGEMENT
#define OPENTHREAD_CONFIG_PLATFORM_ASSERT_MANAGEMENT 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_OUTPUT
 *
 * The ESP-IDF platform provides an otPlatLog() function.
 */
#ifndef OPENTHREAD_CONFIG_LOG_OUTPUT
#define OPENTHREAD_CONFIG_LOG_OUTPUT OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED
#endif

/**
 * @def OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS
 *
 * The number of message buffers in buffer pool
 */
#ifndef OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS
#define OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS 50
#endif

/**
 * @def OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS
 *
 * Define as 1 to enable bultin-mbedtls.
 *
 * Note that the OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS determines whether to use bultin-mbedtls as well as
 * whether to manage mbedTLS internally, such as memory allocation and debug.
 *
 */
#ifndef OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS
#define OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS 0
#endif

/**
 * @def OPENTHREAD_CONFIG_MAX_STATECHANGE_HANDLERS
 *
 * The maximum number of state-changed callback handlers (set using `otSetStateChangedCallback()`).
 *
 */
#ifndef OPENTHREAD_CONFIG_MAX_STATECHANGE_HANDLERS
#define OPENTHREAD_CONFIG_MAX_STATECHANGE_HANDLERS 3
#endif

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
 *
 * Define as 1 to enable microsecond timer.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
#define OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_CSMA_BACKOFF_ENABLE
 *
 * Define to 1 to enable software CSMA-CA backoff logic.
 *
 */
#ifndef OPENTHREAD_CONFIG_MAC_SOFTWARE_CSMA_BACKOFF_ENABLE
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_CSMA_BACKOFF_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE
 *
 * Define to 1 to enable software retransmission logic.
 *
 */
#ifndef OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_SECURITY_ENABLE
 *
 * Define to 1 to enable software transmission security logic.
 *
 */
#ifndef OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_SECURITY_ENABLE
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_SECURITY_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_TIMING_ENABLE
 *
 * Define to 1 to enable software transmission target time logic.
 *
 */
#ifndef OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_TIMING_ENABLE
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_TIMING_ENABLE 1
#endif
