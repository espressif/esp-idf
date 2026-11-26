/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Prepares the CPU to resume U-mode code */

#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "riscv/encoding.h"
#include "esp_private/privilege_trap.h"
#if SOC_CPU_HAS_PIE
#include "riscv/csr_pie.h"
#endif /* SOC_CPU_HAS_PIE */

    .section .iram1, "ax"

/**
 * @brief Prepare the CPU to resume a task in U-mode
 *
 * Switches the CPU state to the U-mode code of the task.
 *
 * @note Must be called with interrupts disabled and with the user sp in mscratch.
 *
 * @param t0 Address of the call context of the task. t0 is preserved, and t1, t2 and ra are clobbered.
 */
    .global rv_user_trap_resume
    .type rv_user_trap_resume, @function
rv_user_trap_resume:
    /* Disable the CPU extensions that hold task state, so that U-mode code cannot change the extension registers */

    /* Disable the FPU for U-mode code */
    li      t1, MSTATUS_FS
    csrc    mstatus, t1
    /* Make WFI and counter reads raise an exception in U-mode */
    li      t1, MSTATUS_TW
    csrs    mstatus, t1
    csrw    mcounteren, zero
#if SOC_CPU_HAS_PIE
    csrw    CSR_PIE_STATE_REG, zero
#endif /* SOC_CPU_HAS_PIE */
#if SOC_CPU_HAS_HWLOOP
    /* Disable the hardware loops and record the hardware-loop state of M-mode code as DIRTY or CLEAN. A trap from
     * U-mode writes the recorded state back, because the exception entry code treats a hardware-loop instruction that
     * traps in the OFF state as an instruction of an ISR. */
    csrrwi  t1, CSR_HWLP_STATE_REG, HWLP_OFF_STATE
    andi    t1, t1, 3
    li      t2, HWLP_DIRTY_STATE
    beq     t1, t2, .Lhwlp_state_recorded
    li      t1, HWLP_CLEAN_STATE
.Lhwlp_state_recorded:
    sw      t1, RV_UCTX_MACHINE_HWLP_STATE(t0)
#endif /* SOC_CPU_HAS_HWLOOP */
#if SOC_CPU_HAS_DSP
    csrw    CSR_DSP_STATE_REG, zero
#endif /* SOC_CPU_HAS_DSP */
#if SOC_ASYNCHRONOUS_BUS_ERROR_MODE
    /* Make bus errors synchronous for U-mode code. The fence waits for the earlier M-mode stores to complete before bus
     * errors become synchronous. */
    fence
    li      t1, MHINT_SBE
    csrs    MHINT_CSR, t1
#endif /* SOC_ASYNCHRONOUS_BUS_ERROR_MODE */
#if SOC_CPU_HAS_ZC_EXTENSIONS
    lw      t1, RV_UCTX_USER_JVT(t0)
    csrw    JVT_CSR, t1
#endif /* SOC_CPU_HAS_ZC_EXTENSIONS */
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* Pause the stack guard, because U-mode code runs on its own stack, outside the task's stack bounds */
    ESP_HW_STACK_GUARD_MONITOR_STOP_CUR_CORE t1 t2
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */
    lw      t1, RV_UCTX_SATP(t0)
    rv_user_activate_satp t1, t2
    ret
    .size rv_user_trap_resume, .-rv_user_trap_resume
