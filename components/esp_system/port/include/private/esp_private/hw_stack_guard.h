/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifndef __ASSEMBLER__
#include "sdkconfig.h"
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

.macro ESP_HW_STACK_GUARD_SET_BOUNDS_CPU0 reg1
    lui     \reg1, ASSIST_DEBUG_CORE_0_INTR_ENA_REG_IMM
    sw      a0, ASSIST_DEBUG_CORE_0_SP_MIN_OFFSET(\reg1)
    sw      a1, ASSIST_DEBUG_CORE_0_SP_MAX_OFFSET(\reg1)
.endm

.macro ESP_HW_STACK_GUARD_MONITOR_STOP_CPU0 reg1 reg2
    lui     \reg1, ASSIST_DEBUG_CORE_0_INTR_ENA_REG_IMM
    lw      \reg2, 0(\reg1)
    andi    \reg2, \reg2, ~ASSIST_DEBUG_SP_SPILL_BITS
    sw      \reg2, 0(\reg1)
.endm

.macro ESP_HW_STACK_GUARD_MONITOR_START_CPU0 reg1 reg2
    lui     \reg1, ASSIST_DEBUG_CORE_0_INTR_ENA_REG_IMM
    lw      \reg2, 0(\reg1)
    ori     \reg2, \reg2, ASSIST_DEBUG_SP_SPILL_BITS
    sw      \reg2, 0(\reg1)
.endm


#if SOC_CPU_CORES_NUM > 1

#define ASSIST_DEBUG_CORE_1_INTR_ENA_REG_IMM (ASSIST_DEBUG_CORE_1_INTR_ENA_REG >> 12)
#define ASSIST_DEBUG_CORE_1_SP_MIN_OFFSET    (ASSIST_DEBUG_CORE_1_SP_MIN_REG - ASSIST_DEBUG_CORE_1_INTR_ENA_REG)
#define ASSIST_DEBUG_CORE_1_SP_MAX_OFFSET    (ASSIST_DEBUG_CORE_1_SP_MAX_REG - ASSIST_DEBUG_CORE_1_INTR_ENA_REG)

.macro ESP_HW_STACK_GUARD_SET_BOUNDS_CPU1 reg1
    lui     \reg1, ASSIST_DEBUG_CORE_1_INTR_ENA_REG_IMM
    sw      a0, ASSIST_DEBUG_CORE_1_SP_MIN_OFFSET(\reg1)
    sw      a1, ASSIST_DEBUG_CORE_1_SP_MAX_OFFSET(\reg1)
.endm

.macro ESP_HW_STACK_GUARD_MONITOR_STOP_CPU1 reg1 reg2
    lui     \reg1, ASSIST_DEBUG_CORE_1_INTR_ENA_REG_IMM
    lw      \reg2, 0(\reg1)
    andi    \reg2, \reg2, ~ASSIST_DEBUG_SP_SPILL_BITS
    sw      \reg2, 0(\reg1)
.endm

.macro ESP_HW_STACK_GUARD_MONITOR_START_CPU1 reg1 reg2
    lui     \reg1, ASSIST_DEBUG_CORE_1_INTR_ENA_REG_IMM
    lw      \reg2, 0(\reg1)
    ori     \reg2, \reg2, ASSIST_DEBUG_SP_SPILL_BITS
    sw      \reg2, 0(\reg1)
.endm

.macro ESP_HW_STACK_GUARD_SET_BOUNDS_CUR_CORE reg1
    /* Check the current core ID */
    csrr \reg1, mhartid
    beqz \reg1, @1f
    /* Core 1 */
    ESP_HW_STACK_GUARD_SET_BOUNDS_CPU1 \reg1
    j 2f
1:
    /* Core 0 */
    ESP_HW_STACK_GUARD_SET_BOUNDS_CPU0 \reg1
2:
.endm

.macro ESP_HW_STACK_GUARD_MONITOR_START_CUR_CORE reg1 reg2
    /* Check the current core ID */
    csrr \reg1, mhartid
    beqz \reg1, @1f
    /* Core 1 */
    ESP_HW_STACK_GUARD_MONITOR_START_CPU1 \reg1 \reg2
    j 2f
1:
    /* Core 0 */
    ESP_HW_STACK_GUARD_MONITOR_START_CPU0 \reg1 \reg2
2:
.endm

.macro ESP_HW_STACK_GUARD_MONITOR_STOP_CUR_CORE reg1 reg2
    /* Check the current core ID */
    csrr \reg1, mhartid
    beqz \reg1, @1f
    /* Core 1 */
    ESP_HW_STACK_GUARD_MONITOR_STOP_CPU1 \reg1 \reg2
    j 2f
1:
    /* Core 0 */
    ESP_HW_STACK_GUARD_MONITOR_STOP_CPU0 \reg1 \reg2
2:
.endm

#else // SOC_CPU_CORES_NUM <= 1

#define ESP_HW_STACK_GUARD_SET_BOUNDS_CUR_CORE      ESP_HW_STACK_GUARD_SET_BOUNDS_CPU0
#define ESP_HW_STACK_GUARD_MONITOR_START_CUR_CORE   ESP_HW_STACK_GUARD_MONITOR_START_CPU0
#define ESP_HW_STACK_GUARD_MONITOR_STOP_CUR_CORE    ESP_HW_STACK_GUARD_MONITOR_STOP_CPU0

#endif // SOC_CPU_CORES_NUM > 1


#endif // __ASSEMBLER__
