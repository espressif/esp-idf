/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-C6 PMU register operations

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/soc.h"
#include "esp_attr.h"
#include "hal/assert.h"
#include "soc/pmu_struct.h"
#include "hal/pmu_types.h"

#ifdef __cplusplus
extern "C" {
#endif

FORCE_INLINE_ATTR uint32_t pmu_ll_lp_get_interrupt_raw(pmu_dev_t *hw)
{
    return hw->lp_int_raw.val;
}

FORCE_INLINE_ATTR void pmu_ll_lp_clear_intsts_mask(pmu_dev_t *hw, uint32_t mask)
{
    hw->lp_int_raw.val = mask;
}

#ifdef __cplusplus
}
#endif
