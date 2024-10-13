/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-C5 LP_Timer register operations

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/soc.h"
#include "soc/lp_timer_struct.h"
#include "soc/lp_timer_reg.h"
#include "soc/lp_aon_reg.h"
#include "hal/lp_timer_types.h"
#include "hal/misc.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

FORCE_INLINE_ATTR void lp_timer_ll_set_alarm_target(lp_timer_dev_t *dev, uint8_t timer_id, uint64_t value)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->target[timer_id].hi, main_timer_tar_high0, (value >> 32) & 0xFFFF);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->target[timer_id].lo, main_timer_tar_low0, value & 0xFFFFFFFF);
}

FORCE_INLINE_ATTR void lp_timer_ll_set_target_enable(lp_timer_dev_t *dev, uint8_t timer_id, bool en)
{
    dev->target[timer_id].hi.main_timer_tar_en0 = en;
}

FORCE_INLINE_ATTR uint32_t lp_timer_ll_get_counter_value_low(lp_timer_dev_t *dev, uint8_t buffer_id)
{
    return HAL_FORCE_READ_U32_REG_FIELD(dev->counter[buffer_id].lo, main_timer_buf0_low);
}

FORCE_INLINE_ATTR uint32_t lp_timer_ll_get_counter_value_high(lp_timer_dev_t *dev, uint8_t buffer_id)
{
    return HAL_FORCE_READ_U32_REG_FIELD(dev->counter[buffer_id].hi, main_timer_buf0_high);
}

FORCE_INLINE_ATTR void lp_timer_ll_counter_snapshot(lp_timer_dev_t *dev)
{
    dev->update.main_timer_update = 1;
}

FORCE_INLINE_ATTR void lp_timer_ll_clear_alarm_intr_status(lp_timer_dev_t *dev)
{
    dev->int_clr.soc_wakeup_int_clr = 1;
}

FORCE_INLINE_ATTR void lp_timer_ll_clear_overflow_intr_status(lp_timer_dev_t *dev)
{
    dev->int_clr.overflow_clr = 1;
}

FORCE_INLINE_ATTR void lp_timer_ll_clear_lp_alarm_intr_status(lp_timer_dev_t *dev)
{
    dev->lp_int_clr.main_timer_lp_int_clr = 1;
}

FORCE_INLINE_ATTR uint32_t lp_timer_ll_get_lp_intr_raw(lp_timer_dev_t *dev)
{
    return dev->lp_int_raw.val;
}

FORCE_INLINE_ATTR void lp_timer_ll_clear_lp_intsts_mask(lp_timer_dev_t *dev, uint32_t mask)
{
    dev->lp_int_clr.val = mask;
}

#ifdef __cplusplus
}
#endif
