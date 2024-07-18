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
#define OPENTHREAD_CONFIG_MAC_MAX_CSMA_BACKOFFS_DIRECT CONFIG_OPENTHREAD_SPINEL_MAC_MAX_CSMA_BACKOFFS_DIRECT

/*----The following options set fixed default values but can be overridden by the user header file.----*/

/**
 * @def OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT
 *
 * Defines the max count of RCP failures allowed to be recovered.
 * 0 means to disable RCP failure recovering.
 *
 */
#ifndef OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT
#define OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT 3
#endif

#ifndef OPENTHREAD_SPINEL_CONFIG_VENDOR_HOOK_ENABLE
#define OPENTHREAD_SPINEL_CONFIG_VENDOR_HOOK_ENABLE 1
#endif


/**
 * @def OPENTHREAD_SPINEL_CONFIG_COMPATIBILITY_ERROR_CALLBACK_ENABLE
 *
 * Enables compatibility error callback in Spinel
 */
#ifndef OPENTHREAD_SPINEL_CONFIG_COMPATIBILITY_ERROR_CALLBACK_ENABLE
#define OPENTHREAD_SPINEL_CONFIG_COMPATIBILITY_ERROR_CALLBACK_ENABLE 1
#endif
