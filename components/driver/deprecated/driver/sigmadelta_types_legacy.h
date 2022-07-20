/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "driver/gpio.h" // for gpio_num_t type define

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

/**
 * @brief Sigma-delta channel list
 */
typedef enum {
    SIGMADELTA_CHANNEL_0,   /*!< Sigma-delta channel 0 */
    SIGMADELTA_CHANNEL_1,   /*!< Sigma-delta channel 1 */
    SIGMADELTA_CHANNEL_2,   /*!< Sigma-delta channel 2 */
    SIGMADELTA_CHANNEL_3,   /*!< Sigma-delta channel 3 */
#if SOC_SDM_CHANNELS_PER_GROUP > 4
    SIGMADELTA_CHANNEL_4,   /*!< Sigma-delta channel 4 */
    SIGMADELTA_CHANNEL_5,   /*!< Sigma-delta channel 5 */
    SIGMADELTA_CHANNEL_6,   /*!< Sigma-delta channel 6 */
    SIGMADELTA_CHANNEL_7,   /*!< Sigma-delta channel 7 */
#endif
    SIGMADELTA_CHANNEL_MAX, /*!< Sigma-delta channel max */
} sigmadelta_channel_t;

/**
 * @brief Sigma-delta configure struct
 */
typedef struct {
    sigmadelta_channel_t channel; /*!< Sigma-delta channel number */
    int8_t sigmadelta_duty;       /*!< Sigma-delta duty, duty ranges from -128 to 127. */
    uint8_t sigmadelta_prescale;  /*!< Sigma-delta prescale, prescale ranges from 0 to 255. */
    gpio_num_t sigmadelta_gpio;   /*!< Sigma-delta output io number, refer to gpio.h for more details. */
} sigmadelta_config_t;

#ifdef __cplusplus
}
#endif
