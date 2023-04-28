/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 *   This file includes compile-time configuration constants for OpenThread.
 */

#ifndef OPENTHREAD_SPINEL_CONFIG_H_
#define OPENTHREAD_SPINEL_CONFIG_H_

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

#endif // OPENTHREAD_SPINEL_CONFIG_H_
