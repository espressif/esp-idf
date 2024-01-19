/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "portmacro.h"
#include "freertos/FreeRTOSConfig.h"
#include "soc/soc_caps.h"
#include "riscv/rvruntime-frames.h"

    .extern pxCurrentTCBs

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
#include "esp_private/hw_stack_guard.h"
#endif

    .global port_uxInterruptNesting
    .global port_xSchedulerRunning
    .global xIsrStackTop
    .global pxCurrentTCBs
    .global vTaskSwitchContext
    .global xPortSwitchFlag
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    .global xIsrStackBottom
    .global esp_hw_stack_guard_monitor_stop
    .global esp_hw_stack_guard_monitor_start
    .global esp_hw_stack_guard_set_bounds
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

    .section .text


#if SOC_CPU_COPROC_NUM > 0

#if SOC_CPU_HAS_FPU

/* Bit to set in mstatus to enable the FPU */
#define CSR_MSTATUS_FPU_ENABLE      (1 << 13)
/* Bit to clear in mstatus to disable the FPU */
#define CSR_MSTATUS_FPU_DISABLE     (3 << 13)

.macro save_fpu_regs frame=sp
    fsw     ft0,  RV_FPU_FT0(\frame)
    fsw     ft1,  RV_FPU_FT1(\frame)
    fsw     ft2,  RV_FPU_FT2(\frame)
    fsw     ft3,  RV_FPU_FT3(\frame)
    fsw     ft4,  RV_FPU_FT4(\frame)
    fsw     ft5,  RV_FPU_FT5(\frame)
    fsw     ft6,  RV_FPU_FT6(\frame)
    fsw     ft7,  RV_FPU_FT7(\frame)
    fsw     fs0,  RV_FPU_FS0(\frame)
    fsw     fs1,  RV_FPU_FS1(\frame)
    fsw     fa0,  RV_FPU_FA0(\frame)
    fsw     fa1,  RV_FPU_FA1(\frame)
    fsw     fa2,  RV_FPU_FA2(\frame)
    fsw     fa3,  RV_FPU_FA3(\frame)
    fsw     fa4,  RV_FPU_FA4(\frame)
    fsw     fa5,  RV_FPU_FA5(\frame)
    fsw     fa6,  RV_FPU_FA6(\frame)
    fsw     fa7,  RV_FPU_FA7(\frame)
    fsw     fs2,  RV_FPU_FS2(\frame)
    fsw     fs3,  RV_FPU_FS3(\frame)
    fsw     fs4,  RV_FPU_FS4(\frame)
    fsw     fs5,  RV_FPU_FS5(\frame)
    fsw     fs6,  RV_FPU_FS6(\frame)
    fsw     fs7,  RV_FPU_FS7(\frame)
    fsw     fs8,  RV_FPU_FS8(\frame)
    fsw     fs9,  RV_FPU_FS9(\frame)
    fsw     fs10, RV_FPU_FS10(\frame)
    fsw     fs11, RV_FPU_FS11(\frame)
    fsw     ft8,  RV_FPU_FT8 (\frame)
    fsw     ft9,  RV_FPU_FT9 (\frame)
    fsw     ft10, RV_FPU_FT10(\frame)
    fsw     ft11, RV_FPU_FT11(\frame)
.endm

.macro restore_fpu_regs frame=sp
    flw     ft0,  RV_FPU_FT0(\frame)
    flw     ft1,  RV_FPU_FT1(\frame)
    flw     ft2,  RV_FPU_FT2(\frame)
    flw     ft3,  RV_FPU_FT3(\frame)
    flw     ft4,  RV_FPU_FT4(\frame)
    flw     ft5,  RV_FPU_FT5(\frame)
    flw     ft6,  RV_FPU_FT6(\frame)
    flw     ft7,  RV_FPU_FT7(\frame)
    flw     fs0,  RV_FPU_FS0(\frame)
    flw     fs1,  RV_FPU_FS1(\frame)
    flw     fa0,  RV_FPU_FA0(\frame)
    flw     fa1,  RV_FPU_FA1(\frame)
    flw     fa2,  RV_FPU_FA2(\frame)
    flw     fa3,  RV_FPU_FA3(\frame)
    flw     fa4,  RV_FPU_FA4(\frame)
    flw     fa5,  RV_FPU_FA5(\frame)
    flw     fa6,  RV_FPU_FA6(\frame)
    flw     fa7,  RV_FPU_FA7(\frame)
    flw     fs2,  RV_FPU_FS2(\frame)
    flw     fs3,  RV_FPU_FS3(\frame)
    flw     fs4,  RV_FPU_FS4(\frame)
    flw     fs5,  RV_FPU_FS5(\frame)
    flw     fs6,  RV_FPU_FS6(\frame)
    flw     fs7,  RV_FPU_FS7(\frame)
    flw     fs8,  RV_FPU_FS8(\frame)
    flw     fs9,  RV_FPU_FS9(\frame)
    flw     fs10, RV_FPU_FS10(\frame)
    flw     fs11, RV_FPU_FS11(\frame)
    flw     ft8,  RV_FPU_FT8(\frame)
    flw     ft9,  RV_FPU_FT9(\frame)
    flw     ft10, RV_FPU_FT10(\frame)
    flw     ft11, RV_FPU_FT11(\frame)
.endm


.macro fpu_read_dirty_bit reg
    csrr    \reg, mstatus
    srli    \reg, \reg, 13
    andi    \reg, \reg, 1
.endm


.macro fpu_clear_dirty_bit reg
    li      \reg, 1 << 13
    csrc    mstatus, \reg
.endm


.macro fpu_enable reg
    li      \reg, CSR_MSTATUS_FPU_ENABLE
    csrs   mstatus, \reg
.endm


.macro fpu_disable reg
    li      \reg, CSR_MSTATUS_FPU_DISABLE
    csrc   mstatus, \reg
.endm

    .global vPortTaskPinToCore
    .global vPortCoprocUsedInISR
    .global pxPortUpdateCoprocOwner

/**
 * @brief Save the current FPU context in the FPU owner's save area
 *
 * @param sp Interuptee's RvExcFrame address
 *
 * Note: Since this routine is ONLY meant to be called from _panic_handler routine,
 * it is possible to alter `s0-s11` registers
 */
    .global rtos_save_fpu_coproc
    .type rtos_save_fpu_coproc, @function
rtos_save_fpu_coproc:
    /* If we are in an interrupt context, we have to abort. We don't allow using the FPU from ISR */
#if ( configNUM_CORES > 1 )
    csrr  a2, mhartid                     /* a2 = coreID */
    slli  a2, a2, 2                       /* a2 = coreID * 4 */
    la    a1, port_uxInterruptNesting     /* a1 = &port_uxInterruptNesting */
    add   a1, a1, a2                      /* a1 = &port_uxInterruptNesting[coreID] */
    lw    a1, 0(a1)                       /* a1 = port_uxInterruptNesting[coreID] */
#else /* ( configNUM_CORES <= 1 ) */
    lw    a1, (port_uxInterruptNesting)   /* a1 = port_uxInterruptNesting */
#endif /* ( configNUM_CORES > 1 ) */
    /* SP still contains the RvExcFrame address */
    mv    a0, sp
    bnez  a1, vPortCoprocUsedInISR
    /* Enable the FPU needed by the current task */
    fpu_enable a1
    mv    s0, ra
    call  rtos_current_tcb
    /* If the current TCB is NULL, the FPU is used during initialization, even before
     * the scheduler started. Consider this a valid usage, the FPU will be disabled
     * as soon as the scheduler is started anyway*/
    beqz  a0, rtos_save_fpu_coproc_norestore
    mv    s1, a0                    /* s1 = pxCurrentTCBs */
    /* Prepare parameters of pxPortUpdateCoprocOwner */
    mv    a2, a0
    li    a1, FPU_COPROC_IDX
    csrr  a0, mhartid
    call  pxPortUpdateCoprocOwner
    /* If the save area is NULL, no need to save context */
    beqz  a0, rtos_save_fpu_coproc_nosave
    /* Save the FPU context in the structure */
    lw    a0, RV_COPROC_SA+FPU_COPROC_IDX*4(a0)      /* a0 = RvCoprocSaveArea->sa_coprocs[FPU_COPROC_IDX] */
    save_fpu_regs a0
    csrr  a1, fcsr
    sw    a1, RV_FPU_FCSR(a0)
rtos_save_fpu_coproc_nosave:
#if ( configNUM_CORES > 1 )
    /* Pin current task to current core */
    mv    a0, s1
    csrr  a1, mhartid
    call  vPortTaskPinToCore
#endif /* configNUM_CORES > 1 */
    /* Check if we have to restore a previous FPU context from the current TCB */
    mv    a0, s1
    /* Do not allocate memory for the FPU yet, delay this until another task wants to use it.
     * This guarantees that if a stack overflow occurs when allocating FPU context on the stack,
     * the current task context is flushed and updated in the TCB, generating a correct backtrace
     * from the panic handler.  */
    li    a1, 0
    li    a2, FPU_COPROC_IDX
    call  pxPortGetCoprocArea
    /* Get the enable flags from the coprocessor save area */
    lw    a1, RV_COPROC_ENABLE(a0)
    /* To avoid having branches below, set the FPU enable flag now */
    ori   a2, a1, 1 << FPU_COPROC_IDX
    sw    a2, RV_COPROC_ENABLE(a0)
    /* Check if the former FPU enable bit was set */
    andi  a2, a1, 1 << FPU_COPROC_IDX
    beqz  a2, rtos_save_fpu_coproc_norestore
    /* FPU enable bit was set, restore the FPU context */
    lw    a0, RV_COPROC_SA+FPU_COPROC_IDX*4(a0)      /* a0 = RvCoprocSaveArea->sa_coprocs[FPU_COPROC_IDX] */
    restore_fpu_regs a0
    lw    a1, RV_FPU_FCSR(a0)
    csrw  fcsr, a1
rtos_save_fpu_coproc_norestore:
    /* Return from routine via s0, instead of ra */
    jr    s0
    .size rtos_save_fpu_coproc, .-rtos_save_fpu_coproc

#endif /* SOC_CPU_HAS_FPU */

#endif /* SOC_CPU_COPROC_NUM > 0 */


/**
 * @brief Get current TCB on current core
 */
    .type rtos_current_tcb, @function
rtos_current_tcb:
#if ( configNUM_CORES > 1 )
    csrr    a1, mhartid
    slli    a1, a1, 2
    la      a0, pxCurrentTCBs               /* a0 = &pxCurrentTCBs */
    add     a0, a0, a1                      /* a0 = &pxCurrentTCBs[coreID] */
    lw      a0, 0(a0)                       /* a0 = pxCurrentTCBs[coreID] */
#else
    /* Recover the stack of next task */
    lw      a0, pxCurrentTCBs
#endif /* ( configNUM_CORES > 1 ) */
    ret
    .size, .-rtos_current_tcb


/**
 * This function makes the RTOS aware about an ISR entering. It takes the
 * current task stack pointer and places it into the pxCurrentTCBs.
 * It then loads the ISR stack into sp.
 * TODO: ISR nesting code improvements ?
 * In the routines below, let's use a0-a5 registers to let the compiler generate
 * 16-bit instructions.
 */
    .global rtos_int_enter
    .type rtos_int_enter, @function
rtos_int_enter:
#if ( configNUM_CORES > 1 )
    csrr    a5, mhartid                     /* a5 = coreID */
    slli    a5, a5, 2                       /* a5 = coreID * 4 */
    la      a0, port_xSchedulerRunning      /* a0 = &port_xSchedulerRunning */
    add     a0, a0, a5                      /* a0 = &port_xSchedulerRunning[coreID] */
    lw      a0, (a0)                        /* a0 = port_xSchedulerRunning[coreID] */
#else
    lw      a0, port_xSchedulerRunning      /* a0 = port_xSchedulerRunning */
#endif /* ( configNUM_CORES > 1 ) */
    beqz    a0, rtos_int_enter_end          /* if (port_xSchedulerRunning[coreID] == 0) jump to rtos_int_enter_end */

    /* Increment the ISR nesting count */
    la      a0, port_uxInterruptNesting     /* a0 = &port_uxInterruptNesting */
#if ( configNUM_CORES > 1 )
    add     a0, a0, a5                      /* a0 = &port_uxInterruptNesting[coreID] // a5 already contains coreID * 4 */
#endif /* ( configNUM_CORES > 1 ) */
    lw      a1, 0(a0)                       /* a1 = port_uxInterruptNesting[coreID] */
    addi    a2, a1, 1                       /* a2 = a1 + 1 */
    sw      a2, 0(a0)                       /* port_uxInterruptNesting[coreID] = a2 */

    /* If we reached here from another low-priority ISR, i.e, port_uxInterruptNesting[coreID] > 0, then skip stack pushing to TCB */
    bnez    a1, rtos_int_enter_end          /* if (port_uxInterruptNesting[coreID] > 0) jump to rtos_int_enter_end */

#if SOC_CPU_COPROC_NUM > 0
    /* Disable the FPU to forbid the ISR from using it. We don't need to re-enable it manually since the caller
     * will restore `mstatus` before returning from interrupt. */
    fpu_disable a0
#endif /* SOC_CPU_COPROC_NUM > 0 */


#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* esp_hw_stack_guard_monitor_stop(); pass the scratch registers */
    ESP_HW_STACK_GUARD_MONITOR_STOP_CUR_CORE a0 a1
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

    /* Save the current sp in pxCurrentTCBs[coreID] and load the ISR stack on to sp */
#if ( configNUM_CORES > 1 )
    la      a0, pxCurrentTCBs               /* a0 = &pxCurrentTCBs */
    add     a0, a0, a5                      /* a0 = &pxCurrentTCBs[coreID] // a5 already contains coreID * 4 */
    lw      a0, (a0)                        /* a0 = pxCurrentTCBs[coreID] */
    sw      sp, 0(a0)                       /* pxCurrentTCBs[coreID] = sp */
    la      a0, xIsrStackTop                /* a0 = &xIsrStackTop */
    add     a0, a0, a5                      /* a0 = &xIsrStackTop[coreID] // a5 already contains coreID * 4 */
    lw      sp, (a0)                        /* sp = xIsrStackTop[coreID] */
#else
    lw      a0, pxCurrentTCBs               /* a0 = pxCurrentTCBs */
    sw      sp, 0(a0)                       /* pxCurrentTCBs[0] = sp */
    lw      sp, xIsrStackTop                /* sp = xIsrStackTop */
#endif /* ( configNUM_CORES > 1 ) */

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* Prepare the parameters for esp_hw_stack_guard_set_bounds(xIsrStackBottom, xIsrStackTop); */
#if ( configNUM_CORES > 1 )
    /* Load the xIsrStack for the current core and set the new bounds */
    la      a0, xIsrStackBottom
    add     a0, a0, a5                      /* a0 = &xIsrStackBottom[coreID] */
    lw      a0, (a0)                        /* a0 = xIsrStackBottom[coreID] */
#else
    lw      a0, xIsrStackBottom
#endif /* ( configNUM_CORES > 1 ) */
    mv      a1, sp
    /* esp_hw_stack_guard_set_bounds(xIsrStackBottom[coreID], xIsrStackTop[coreID]);
     */
    ESP_HW_STACK_GUARD_SET_BOUNDS_CUR_CORE a2
    ESP_HW_STACK_GUARD_MONITOR_START_CUR_CORE a0 a1
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

rtos_int_enter_end:
    ret

/**
 * @brief Restore the stack pointer of the next task to run.
 *
 * @param a0 Former mstatus
 *
 * @returns New mstatus (potentially with coprocessors disabled)
 */
    .global rtos_int_exit
    .type rtos_int_exit, @function
rtos_int_exit:
    /* To speed up this routine and because this current routine is only meant to be called from the interrupt
     * handler, let's use callee-saved registers instead of stack space. Registers `s3-s11` are not used by
     * the caller */
    mv      s11, a0
#if ( configNUM_CORES > 1 )
    csrr    a1, mhartid                     /* a1 = coreID */
    slli    a1, a1, 2                       /* a1 = a1 * 4 */
    la      a0, port_xSchedulerRunning      /* a0 = &port_xSchedulerRunning */
    add     a0, a0, a1                      /* a0 = &port_xSchedulerRunning[coreID] */
    lw      a0, (a0)                        /* a0 = port_xSchedulerRunning[coreID] */
#else
    lw      a0, port_xSchedulerRunning      /* a0 = port_xSchedulerRunning */
#endif /* ( configNUM_CORES > 1 ) */
    beqz    a0, rtos_int_exit_end           /* if (port_uxSchedulerRunning == 0) jump to rtos_int_exit_end */

    /* Update nesting interrupts counter */
    la      a2, port_uxInterruptNesting     /* a2 = &port_uxInterruptNesting */
#if ( configNUM_CORES > 1 )
    add     a2, a2, a1                      /* a2 = &port_uxInterruptNesting[coreID] // a1 already contains coreID * 4 */
#endif /* ( configNUM_CORES > 1 ) */
    lw      a0, 0(a2)                       /* a0 = port_uxInterruptNesting[coreID] */

    /* Already zero, protect against underflow */
    beqz    a0, isr_skip_decrement          /* if (port_uxInterruptNesting[coreID] == 0) jump to isr_skip_decrement */
    addi    a0, a0, -1                      /* a0 = a0 - 1 */
    sw      a0, 0(a2)                       /* port_uxInterruptNesting[coreID] = a0 */
    /* May still have interrupts pending, skip section below and exit */
    bnez    a0, rtos_int_exit_end

isr_skip_decrement:
    /* If the CPU reached this label, a2 (uxInterruptNesting) is 0 for sure */

    /* Schedule the next task if a yield is pending */
    la      a0, xPortSwitchFlag             /* a0 = &xPortSwitchFlag */
#if ( configNUM_CORES > 1 )
    add     a0, a0, a1                      /* a0 = &xPortSwitchFlag[coreID] // a1 already contains coreID * 4  */
#endif /* ( configNUM_CORES > 1 ) */
    lw      a2, 0(a0)                       /* a2 = xPortSwitchFlag[coreID] */
    beqz    a2, no_switch                   /* if (xPortSwitchFlag[coreID] == 0) jump to no_switch */

    /* Preserve return address and schedule next task. To speed up the process, and because this current routine
     * is only meant to be called from the interrupt handle, let's save some speed and space by using callee-saved
     * registers instead of stack space. Registers `s3-s11` are not used by the caller */
    mv      s10, ra
#if ( SOC_CPU_COPROC_NUM > 0 )
    /* In the cases where the newly scheduled task is different from the previously running one,
     * we have to disable the coprocessor(s) to let them trigger an exception on first use.
     * Else, if the same task is scheduled, do not change the coprocessor(s) state. */
    call    rtos_current_tcb
    mv      s9, a0
    call    vTaskSwitchContext
    call    rtos_current_tcb
    beq     a0, s9, rtos_int_exit_no_change
    /* Disable the coprocessors in s11 register (former mstatus) */
    li      a0, ~CSR_MSTATUS_FPU_DISABLE
    and     s11, s11, a0
rtos_int_exit_no_change:
#else /* ( SOC_CPU_COPROC_NUM == 0 ) */
    call    vTaskSwitchContext
#endif /* ( SOC_CPU_COPROC_NUM > 0 ) */
    mv      ra, s10

    /* Clears the switch pending flag */
    la      a0, xPortSwitchFlag             /* a0 = &xPortSwitchFlag */
#if ( configNUM_CORES > 1 )
    /* C routine vTaskSwitchContext may change the temp registers, so we read again */
    csrr    a1, mhartid                     /* a1 = coreID */
    slli    a1, a1, 2                       /* a1 = a1 * 4 */
    add     a0, a0, a1                      /* a0 = &xPortSwitchFlag[coreID]; */
#endif /* ( configNUM_CORES > 1 ) */
    sw      zero, 0(a0)                     /* xPortSwitchFlag[coreID] = 0; */

no_switch:

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* esp_hw_stack_guard_monitor_stop(); pass the scratch registers */
    ESP_HW_STACK_GUARD_MONITOR_STOP_CUR_CORE a0 a1
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */


#if ( configNUM_CORES > 1 )
    /* Recover the stack of next task and prepare to exit */
    csrr    a1, mhartid
    slli    a1, a1, 2
    la      a0, pxCurrentTCBs               /* a0 = &pxCurrentTCBs */
    add     a0, a0, a1                      /* a0 = &pxCurrentTCBs[coreID] */
    lw      a0, 0(a0)                       /* a0 = pxCurrentTCBs[coreID] */
    lw      sp, 0(a0)                       /* sp = previous sp */
#else
    /* Recover the stack of next task */
    lw      a0, pxCurrentTCBs
    lw      sp, 0(a0)
#endif /* ( configNUM_CORES > 1 ) */


#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* esp_hw_stack_guard_set_bounds(pxCurrentTCBs[0]->pxStack,
     *                               pxCurrentTCBs[0]->pxEndOfStack);
     */
    lw      a1, PORT_OFFSET_PX_END_OF_STACK(a0)
    lw      a0, PORT_OFFSET_PX_STACK(a0)
    ESP_HW_STACK_GUARD_SET_BOUNDS_CUR_CORE a2
    /* esp_hw_stack_guard_monitor_start(); */
    ESP_HW_STACK_GUARD_MONITOR_START_CUR_CORE a0 a1
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

rtos_int_exit_end:
    mv      a0, s11                         /* a0 = new mstatus */
    ret
