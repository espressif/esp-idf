/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @def OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
 *
 * Define 1 to enable feeding an OpenThread message to encoder/decoder.
 *
 */
#ifndef OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
#define OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE 0
#endif

/**
 * @def OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT
 *
 * Defines the max count of RCP failures allowed to be recovered.
 * 0 means to disable RCP failure recovering.
 *
 */
#ifndef OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT
// TZ-567: Set OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT to 3 after adding rcp failure notification mechanism
#define OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT 0
#endif

/**
 * @def OPENTHREAD_SPINEL_CONFIG_RCP_CUSTOM_RESTORATION
 *
 * Define 1 to call the custom RCP failure handler on RCP failure.
 *
 */
#ifndef OPENTHREAD_SPINEL_CONFIG_RCP_CUSTOM_RESTORATION
#define OPENTHREAD_SPINEL_CONFIG_RCP_CUSTOM_RESTORATION 0
#endif

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_RADIO_SPINEL_RX_FRAME_BUFFER_SIZE
 *
 * Specifies the rx frame buffer size used by `SpinelInterface` in RCP host code. This is applicable/used when
 * `RadioSpinel` platform is used.
 *
 */
#define OPENTHREAD_CONFIG_PLATFORM_RADIO_SPINEL_RX_FRAME_BUFFER_SIZE CONFIG_OPENTHREAD_SPINEL_RX_FRAME_BUFFER_SIZE
