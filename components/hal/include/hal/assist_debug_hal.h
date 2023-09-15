/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_attr.h"
#include "soc/soc_caps.h"
#if SOC_ASSIST_DEBUG_SUPPORTED
#include "hal/assist_debug_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


#if SOC_ASSIST_DEBUG_SUPPORTED
FORCE_INLINE_ATTR void assist_debug_hal_sp_int_enable(uint32_t core_id)
{
    assist_debug_ll_sp_spill_interrupt_enable(core_id);
}

FORCE_INLINE_ATTR void assist_debug_hal_sp_int_disable(uint32_t core_id)
{
    assist_debug_ll_sp_spill_interrupt_disable(core_id);
}

FORCE_INLINE_ATTR void assist_debug_hal_sp_int_clear(uint32_t core_id)
{
    assist_debug_ll_sp_spill_interrupt_clear(core_id);
}

FORCE_INLINE_ATTR void assist_debug_hal_sp_mon_enable(uint32_t core_id)
{
    assist_debug_ll_sp_spill_monitor_enable(core_id);
}

FORCE_INLINE_ATTR void assist_debug_hal_sp_mon_disable(uint32_t core_id)
{
    assist_debug_ll_sp_spill_monitor_disable(core_id);
}

FORCE_INLINE_ATTR uint32_t assist_debug_hal_get_sp_ovf_pc(uint32_t core_id)
{
    return assist_debug_ll_sp_spill_get_pc(core_id);
}

FORCE_INLINE_ATTR void assist_debug_hal_get_sp_bounds(uint32_t core_id, uint32_t *sp_min, uint32_t *sp_max)
{
    if (sp_min) {
        *sp_min = assist_debug_ll_sp_spill_get_min(core_id);
    }
    if (sp_max) {
        *sp_max = assist_debug_ll_sp_spill_get_max(core_id);
    }
}

FORCE_INLINE_ATTR void assist_debug_hal_set_sp_bounds(uint32_t core_id, uint32_t sp_min, uint32_t sp_max)
{
    assist_debug_ll_sp_spill_set_min(core_id, sp_min);
    assist_debug_ll_sp_spill_set_max(core_id, sp_max);
}

FORCE_INLINE_ATTR uint32_t assist_debug_hal_is_sp_ovf_fired(uint32_t core_id)
{
    return assist_debug_ll_sp_spill_is_fired(core_id);
}

#endif // SOC_ASSIST_DEBUG_SUPPORTED

#ifdef __cplusplus
}
#endif
