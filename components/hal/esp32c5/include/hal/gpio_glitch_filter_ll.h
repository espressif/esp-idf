/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdbool.h>
#include "hal/assert.h"
#include "soc/gpio_ext_struct.h"

#define GPIO_LL_GLITCH_FILTER_MAX_WINDOW  64

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable GPIO glitch filter
 *
 * @param hw Glitch filter register base address
 * @param filter_idx Glitch filter index
 * @param enable True to enable, false to disable
 */
static inline void gpio_ll_glitch_filter_enable(gpio_glitch_filter_dev_t *hw, uint32_t filter_idx, bool enable)
{
    hw->glitch_filter_chn[filter_idx].filter_chn_en = enable;
}

/**
 * @brief Set the input GPIO for the glitch filter
 *
 * @param hw Glitch filter register base address
 * @param filter_idx Glitch filter index
 * @param gpio_num GPIO number
 */
static inline void gpio_ll_glitch_filter_set_gpio(gpio_glitch_filter_dev_t *hw, uint32_t filter_idx, uint32_t gpio_num)
{
    hw->glitch_filter_chn[filter_idx].filter_chn_input_io_num = gpio_num;
}

/**
 * @brief Set the coefficient of the glitch filter window
 *
 * @param hw Glitch filter register base address
 * @param filter_idx Glitch filter index
 * @param window_width Window width, in IOMUX clock ticks
 * @param window_threshold Window threshold, in IOMUX clock ticks
 */
static inline void gpio_ll_glitch_filter_set_window_coeff(gpio_glitch_filter_dev_t *hw, uint32_t filter_idx, uint32_t window_width, uint32_t window_thres)
{
    HAL_ASSERT(window_thres <= window_width);
    hw->glitch_filter_chn[filter_idx].filter_chn_window_width = window_width - 1;
    hw->glitch_filter_chn[filter_idx].filter_chn_window_thres = window_thres - 1;
}

#ifdef __cplusplus
}
#endif
