/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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

#define ESP_HW_STACK_GUARD_NOT_FIRED UINT32_MAX

/* The functions below are designed to be used in interrupt/panic handler
 * In case using them in user's code put them into critical section.*/

void esp_hw_stack_guard_monitor_start(void);
void esp_hw_stack_guard_monitor_stop(void);
void esp_hw_stack_guard_set_bounds(uint32_t sp_min, uint32_t sp_max);
void esp_hw_stack_guard_get_bounds(uint32_t core_id, uint32_t *sp_min, uint32_t *sp_max);
uint32_t esp_hw_stack_guard_get_fired_cpu(void);
uint32_t esp_hw_stack_guard_get_pc(uint32_t core_id);

#ifdef __cplusplus
};
#endif

#else // __ASSEMBLER__

#include "hal/assist_debug_ll.h"

#define ASSIST_DEBUG_CORE_0_INTR_ENA_REG_IMM (ASSIST_DEBUG_CORE_0_INTR_ENA_REG >> 12)
#define ASSIST_DEBUG_CORE_0_SP_MIN_OFFSET    (ASSIST_DEBUG_CORE_0_SP_MIN_REG - DR_REG_ASSIST_DEBUG_BASE)
#define ASSIST_DEBUG_CORE_0_SP_MAX_OFFSET    (ASSIST_DEBUG_CORE_0_SP_MAX_REG - DR_REG_ASSIST_DEBUG_BASE)
#define ASSIST_DEBUG_CORE_0_SP_SPILL_OFFSET  (ASSIST_DEBUG_CORE_0_INTR_ENA_REG - DR_REG_ASSIST_DEBUG_BASE)

.macro ESP_HW_STACK_GUARD_SET_BOUNDS_CPU0 reg1
lui     \reg1, ASSIST_DEBUG_CORE_0_INTR_ENA_REG_IMM
sw      a0, ASSIST_DEBUG_CORE_0_SP_MIN_OFFSET(\reg1)
sw      a1, ASSIST_DEBUG_CORE_0_SP_MAX_OFFSET(\reg1)
.endm

.macro ESP_HW_STACK_GUARD_MONITOR_STOP_CPU0 reg1 reg2
lui     \reg1, ASSIST_DEBUG_CORE_0_INTR_ENA_REG_IMM
lw      \reg2, ASSIST_DEBUG_CORE_0_SP_SPILL_OFFSET(\reg1)
andi    \reg2, \reg2, ~ASSIST_DEBUG_SP_SPILL_BITS
sw      \reg2, ASSIST_DEBUG_CORE_0_SP_SPILL_OFFSET(\reg1)
.endm

.macro ESP_HW_STACK_GUARD_MONITOR_START_CPU0 reg1 reg2
lui     \reg1, ASSIST_DEBUG_CORE_0_INTR_ENA_REG_IMM
lw      \reg2, ASSIST_DEBUG_CORE_0_SP_SPILL_OFFSET(\reg1)
ori     \reg2, \reg2, ASSIST_DEBUG_SP_SPILL_BITS
sw      \reg2, ASSIST_DEBUG_CORE_0_SP_SPILL_OFFSET(\reg1)
.endm

#if SOC_CPU_CORES_NUM > 1

#define ASSIST_DEBUG_CORE_1_INTR_ENA_REG_IMM (ASSIST_DEBUG_CORE_1_INTR_ENA_REG >> 12)
#define ASSIST_DEBUG_CORE_1_SP_MIN_OFFSET    (ASSIST_DEBUG_CORE_1_SP_MIN_REG - DR_REG_ASSIST_DEBUG_BASE)
#define ASSIST_DEBUG_CORE_1_SP_MAX_OFFSET    (ASSIST_DEBUG_CORE_1_SP_MAX_REG - DR_REG_ASSIST_DEBUG_BASE)
#define ASSIST_DEBUG_CORE_1_SP_SPILL_OFFSET  (ASSIST_DEBUG_CORE_1_INTR_ENA_REG - DR_REG_ASSIST_DEBUG_BASE)

.macro ESP_HW_STACK_GUARD_SET_BOUNDS_CPU1 reg1
lui     \reg1, ASSIST_DEBUG_CORE_1_INTR_ENA_REG_IMM
sw      a0, ASSIST_DEBUG_CORE_1_SP_MIN_OFFSET(\reg1)
sw      a1, ASSIST_DEBUG_CORE_1_SP_MAX_OFFSET(\reg1)
.endm

.macro ESP_HW_STACK_GUARD_MONITOR_STOP_CPU1 reg1 reg2
lui     \reg1, ASSIST_DEBUG_CORE_1_INTR_ENA_REG_IMM
lw      \reg2, ASSIST_DEBUG_CORE_1_SP_SPILL_OFFSET(\reg1)
andi    \reg2, \reg2, ~ASSIST_DEBUG_SP_SPILL_BITS
sw      \reg2, ASSIST_DEBUG_CORE_1_SP_SPILL_OFFSET(\reg1)
.endm

.macro ESP_HW_STACK_GUARD_MONITOR_START_CPU1 reg1 reg2
lui     \reg1, ASSIST_DEBUG_CORE_1_INTR_ENA_REG_IMM
lw      \reg2, ASSIST_DEBUG_CORE_1_SP_SPILL_OFFSET(\reg1)
ori     \reg2, \reg2, ASSIST_DEBUG_SP_SPILL_BITS
sw      \reg2, ASSIST_DEBUG_CORE_1_SP_SPILL_OFFSET(\reg1)
.endm

.macro ESP_HW_STACK_GUARD_SET_BOUNDS_CUR_CORE reg1
/* Check the current core ID */
csrr \reg1, mhartid
beqz \reg1, 1f
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
beqz \reg1, 1f
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
beqz \reg1, 1f
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
