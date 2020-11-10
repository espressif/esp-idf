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

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for SIGMADELTA.
// There is no parameter check in the hal layer, so the caller must ensure the correctness of the parameters.

#pragma once

#include "soc/sigmadelta_periph.h"
#include "hal/sigmadelta_types.h"
#include "hal/sigmadelta_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Context that should be maintained by both the driver and the HAL
 */

typedef struct {
    gpio_sd_dev_t *dev;
} sigmadelta_hal_context_t;

/**
 * @brief Set Sigma-delta channel duty.
 *
 * @param hal Context of the HAL layer
 * @param channel Sigma-delta channel number
 * @param duty Sigma-delta duty of one channel, the value ranges from -128 to 127, recommended range is -90 ~ 90.
 *             The waveform is more like a random one in this range.
 */
#define sigmadelta_hal_set_duty(hal, channel, duty) sigmadelta_ll_set_duty((hal)->dev, channel, duty)

/**
 * @brief Set Sigma-delta channel's clock pre-scale value.
 *
 * @param hal Context of the HAL layer
 * @param channel Sigma-delta channel number
 * @param prescale The divider of source clock, ranges from 0 to 255
 */
#define sigmadelta_hal_set_prescale(hal, channel, prescale) sigmadelta_ll_set_prescale((hal)->dev, channel, prescale)

/**
 * @brief Init the SIGMADELTA hal and set the SIGMADELTA to the default configuration. This function should be called first before other hal layer function is called
 *
 * @param hal Context of the HAL layer
 * @param sigmadelta_num The uart port number, the max port number is (SIGMADELTA_NUM_MAX -1)
 */
void sigmadelta_hal_init(sigmadelta_hal_context_t *hal, int sigmadelta_num);

#ifdef __cplusplus
}
#endif
