// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
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
 * See readme.md in soc/include/hal/readme.md
 ******************************************************************************/

// The LL layer for ESP32 SIGMADELTA register operations

#pragma once

#include <stdbool.h>
#include "hal/misc.h"
#include "soc/sigmadelta_periph.h"
#include "soc/gpio_sd_struct.h"
#include "hal/sigmadelta_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Get SIGMADELTA hardware instance with giving sigmadelta num
#define SIGMADELTA_LL_GET_HW(num) (((num) == 0) ? (&SIGMADELTA) : NULL)

/**
 * @brief Set Sigma-delta enable
 *
 * @param hw Peripheral SIGMADELTA hardware instance address.
 * @param en Sigma-delta enable value
 */
static inline void sigmadelta_ll_set_en(gpio_sd_dev_t *hw, bool en)
{
    hw->misc.function_clk_en = en;
}

/**
 * @brief Set Sigma-delta channel duty.
 *
 * @param hw Peripheral SIGMADELTA hardware instance address.
 * @param channel Sigma-delta channel number
 * @param duty Sigma-delta duty of one channel, the value ranges from -128 to 127, recommended range is -90 ~ 90.
 *             The waveform is more like a random one in this range.
 */
static inline void sigmadelta_ll_set_duty(gpio_sd_dev_t *hw, sigmadelta_channel_t channel, int8_t duty)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->channel[channel], duty, (uint32_t)duty);
}

/**
 * @brief Set Sigma-delta channel's clock pre-scale value.
 *
 * @param hw Peripheral SIGMADELTA hardware instance address.
 * @param channel Sigma-delta channel number
 * @param val The divider of source clock, ranges from 0 to 255
 */
static inline void sigmadelta_ll_set_prescale(gpio_sd_dev_t *hw, sigmadelta_channel_t channel, uint8_t prescale)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->channel[channel], prescale, prescale);
}

#ifdef __cplusplus
}
#endif
