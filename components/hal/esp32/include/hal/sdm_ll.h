/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "hal/misc.h"
#include "hal/assert.h"
#include "soc/gpio_sd_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set Sigma-delta enable
 *
 * @param hw Peripheral SIGMADELTA hardware instance address.
 * @param en Sigma-delta enable value
 */
static inline void sdm_ll_enable_clock(gpio_sd_dev_t *hw, bool en)
{
    // The clk enable register does not exist on ESP32.
}

/**
 * @brief Set Sigma-delta channel duty.
 *
 * @param hw Peripheral SIGMADELTA hardware instance address.
 * @param channel Sigma-delta channel number
 * @param duty Sigma-delta duty of one channel, the value ranges from -128 to 127, recommended range is -90 ~ 90.
 *             The waveform is more like a random one in this range.
 */
__attribute__((always_inline))
static inline void sdm_ll_set_duty(gpio_sd_dev_t *hw, int channel, int8_t duty)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->channel[channel], duty, (uint32_t)duty);
}

/**
 * @brief Set Sigma-delta channel's clock pre-scale value.
 *
 * @param hw Peripheral SIGMADELTA hardware instance address.
 * @param channel Sigma-delta channel number
 * @param prescale The divider of source clock, ranges from 1 to 256
 */
static inline void sdm_ll_set_prescale(gpio_sd_dev_t *hw, int channel, uint32_t prescale)
{
    HAL_ASSERT(prescale && prescale <= 256);
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->channel[channel], prescale, prescale - 1);
}

#ifdef __cplusplus
}
#endif
