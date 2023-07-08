/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifndef __ASSEMBLER__
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The functions below are designed to be used in interrupt/panic handler
 * In case using them in user's code put them into critical section.*/

void esp_hw_stack_guard_monitor_start(void);
void esp_hw_stack_guard_monitor_stop(void);
void esp_hw_stack_guard_set_bounds(uint32_t sp_min, uint32_t sp_max);
void esp_hw_stack_guard_get_bounds(uint32_t *sp_min, uint32_t *sp_max);
bool esp_hw_stack_guard_is_fired(void);
uint32_t esp_hw_stack_guard_get_pc(void);

#ifdef __cplusplus
};
#endif

#else // __ASSEMBLER__

#include "hal/assist_debug_ll.h"

#define ASSIST_DEBUG_CORE_0_INTR_ENA_REG_IMM (ASSIST_DEBUG_CORE_0_INTR_ENA_REG >> 12)
#define ASSIST_DEBUG_CORE_0_SP_MIN_OFFSET    (ASSIST_DEBUG_CORE_0_SP_MIN_REG - ASSIST_DEBUG_CORE_0_INTR_ENA_REG)
#define ASSIST_DEBUG_CORE_0_SP_MAX_OFFSET    (ASSIST_DEBUG_CORE_0_SP_MAX_REG - ASSIST_DEBUG_CORE_0_INTR_ENA_REG)

.macro ESP_HW_STACK_GUARD_SET_BOUNDS_CPU0
    lui     t0, ASSIST_DEBUG_CORE_0_INTR_ENA_REG_IMM
    sw      a0, ASSIST_DEBUG_CORE_0_SP_MIN_OFFSET(t0)
    sw      a1, ASSIST_DEBUG_CORE_0_SP_MAX_OFFSET(t0)
.endm

.macro ESP_HW_STACK_GUARD_MONITOR_STOP_CPU0
    lui     t0, ASSIST_DEBUG_CORE_0_INTR_ENA_REG_IMM
    lw      t1, 0(t0)
    andi    t1, t1, ~ASSIST_DEBUG_SP_SPILL_BITS
    sw      t1, 0(t0)
.endm

.macro ESP_HW_STACK_GUARD_MONITOR_START_CPU0
    lui     t0, ASSIST_DEBUG_CORE_0_INTR_ENA_REG_IMM
    lw      t1, 0(t0)
    ori     t1, t1, ASSIST_DEBUG_SP_SPILL_BITS
    sw      t1, 0(t0)
.endm

#endif // __ASSEMBLER__
