/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "hal/assert.h"
#include "hal/misc.h"
#include "hal/timg_ll.h"
#include "soc/timer_group_struct.h"
#include "soc/dport_reg.h"

// Get timer group register base address with giving group number
#define LACT_LL_GET_HW(group_id) ((group_id == 0) ? (&TIMERG0) : (&TIMERG1))

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set clock prescale for LACT timer
 *
 * @param hw Timer Group register base address
 * @param divider Prescale value (0 and 1 are not valid)
 */
__attribute__((always_inline))
static inline void lact_ll_set_clock_prescale(timg_dev_t *hw, uint32_t divider)
{
    HAL_ASSERT(divider >= 2);
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->lactconfig, lact_divider, divider);
}

#ifdef __cplusplus
}
#endif
