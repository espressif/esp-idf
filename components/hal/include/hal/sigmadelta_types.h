// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

/**
 * @brief Sigma-delta configure struct
 */
typedef struct {
    sigmadelta_channel_t channel;    /*!< Sigma-delta channel number */
    int8_t sigmadelta_duty;          /*!< Sigma-delta duty, duty ranges from -128 to 127. */
    uint8_t sigmadelta_prescale;     /*!< Sigma-delta prescale, prescale ranges from 0 to 255. */
    uint8_t  sigmadelta_gpio;        /*!< Sigma-delta output io number, refer to gpio.h for more details. */
} sigmadelta_config_t;

#ifdef __cplusplus
}
#endif
