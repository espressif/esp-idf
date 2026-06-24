/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/lp_i2s_hal.h"
#include "driver/i2s_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get LP I2S soc handle
 *
 * @param[in] chan  LP I2S channel handle
 *
 * @return LP I2S soc handle
 */
lp_i2s_soc_handle_t lp_i2s_get_soc_handle(lp_i2s_chan_handle_t chan);

#ifdef __cplusplus
}
#endif
