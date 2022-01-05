/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SIGMADELTA port number, the max port number is (SIGMADELTA_NUM_MAX -1).
 */
typedef enum {
    SIGMADELTA_PORT_0,      /*!< SIGMADELTA port 0 */
    SIGMADELTA_PORT_MAX,    /*!< SIGMADELTA port max */
} sigmadelta_port_t;

_Static_assert(SIGMADELTA_PORT_MAX == SOC_SIGMADELTA_NUM, "Sigma-delta port num incorrect.");

/**
 * @brief Sigma-delta channel list
 */
typedef enum {
    SIGMADELTA_CHANNEL_0,   /*!< Sigma-delta channel 0 */
    SIGMADELTA_CHANNEL_1,   /*!< Sigma-delta channel 1 */
    SIGMADELTA_CHANNEL_2,   /*!< Sigma-delta channel 2 */
    SIGMADELTA_CHANNEL_3,   /*!< Sigma-delta channel 3 */
#if SOC_SIGMADELTA_CHANNEL_NUM > 4
    SIGMADELTA_CHANNEL_4,   /*!< Sigma-delta channel 4 */
    SIGMADELTA_CHANNEL_5,   /*!< Sigma-delta channel 5 */
    SIGMADELTA_CHANNEL_6,   /*!< Sigma-delta channel 6 */
    SIGMADELTA_CHANNEL_7,   /*!< Sigma-delta channel 7 */
#endif
    SIGMADELTA_CHANNEL_MAX, /*!< Sigma-delta channel max */
} sigmadelta_channel_t;

#ifdef __cplusplus
}
#endif
