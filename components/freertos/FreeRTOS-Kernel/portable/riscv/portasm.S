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
#include "riscv/csr_hwlp.h"
#include "riscv/csr_pie.h"

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

/**
 * @brief Macro to generate a routine that saves a coprocessor's registers in the previous owner's TCB dedicated save area.
 * This routine aborts if the coprocessor is used from an ISR, since this is not allowed in ESP-IDF.
 * However it is allowed to use these coprocessors in the init process, so no error will be triggered if the
 * current TCB is NULL.
 *
 * @param name The name of the coprocessor, this will be used to generate the label, so it must not contain special characters
 * @param coproc_idx Index of the coprocessor in the coprocessor save area, this value can be found in rvruntime definition
 * @param enable_coproc Macro that takes a scratch register as a parameter and  enables the coprocessor.
 * @param save_coproc_regs Macro that takes a frame as a parameter and saves all the coprocessors' registers in that frame.
 * @param restore_coproc_regs Macro that takes a frame as a parameter and restores all the coprocessors' registers from that.
 *
 * Note: macros given as parameters can freely use temporary registers
 */
.macro generate_coprocessor_routine name, coproc_idx, enable_coproc, save_coproc_regs, restore_coproc_regs

    .global rtos_save_\name\()_coproc
    .type rtos_save_\name\()_coproc, @function
rtos_save_\name\()_coproc:
    /* If we are in an interrupt context, we have to abort. We don't allow using the coprocessors from ISR */
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
    /* Enable the coprocessor needed by the current task */
    \enable_coproc a1
    mv    s0, ra
    call  rtos_current_tcb
    /* If the current TCB is NULL, the coprocessor is used during initialization, even before
     * the scheduler started. Consider this a valid usage, it will be disabled as soon as the
     * scheduler is started anyway */
    beqz  a0, rtos_save_\name\()_coproc_norestore
    mv    s1, a0                    /* s1 = pxCurrentTCBs */
    /* Prepare parameters of pxPortUpdateCoprocOwner */
    mv    a2, a0
    li    a1, \coproc_idx
    csrr  a0, mhartid
    call  pxPortUpdateCoprocOwner
    /* If the save area is NULL, no need to save context */
    beqz  a0, rtos_save_\name\()_coproc_nosave
    /* If the former owner is the current task (new owner), the return value is -1, we can skip restoring the
     * coprocessor context and return directly */
    li    a1, -1
    beq   a0, a1, rtos_save_\name\()_coproc_norestore
    /* Save the coprocessor context in the structure */
    lw    a0, RV_COPROC_SA+\coproc_idx*4(a0)      /* a0 = RvCoprocSaveArea->sa_coprocs[coproc_idx] */
    \save_coproc_regs a0
rtos_save_\name\()_coproc_nosave:
#if ( configNUM_CORES > 1 )
    /* Pin current task to current core */
    mv    a0, s1
    csrr  a1, mhartid
    call  vPortTaskPinToCore
#endif /* configNUM_CORES > 1 */
    /* Check if we have to restore a previous context from the current TCB */
    mv    a0, s1
    /* Do not allocate memory for the coprocessor yet, delay this until another task wants to use it.
     * This guarantees that if a stack overflow occurs when allocating the coprocessor context on the stack,
     * the current task context is flushed and updated in the TCB, generating a correct backtrace
     * from the panic handler.  */
    li    a1, 0
    li    a2, \coproc_idx
    call  pxPortGetCoprocArea
    /* Get the enable flags from the coprocessor save area */
    lw    a1, RV_COPROC_ENABLE(a0)
    /* To avoid having branches below, set the coprocessor enable flag now */
    ori   a2, a1, 1 << \coproc_idx
    sw    a2, RV_COPROC_ENABLE(a0)
    /* Check if the former coprocessor enable bit was set */
    andi  a2, a1, 1 << \coproc_idx
    beqz  a2, rtos_save_\name\()_coproc_norestore
    /* Enable bit was set, restore the coprocessor context */
    lw    a0, RV_COPROC_SA+\coproc_idx*4(a0)      /* a0 = RvCoprocSaveArea->sa_coprocs[\coproc_idx] */
    \restore_coproc_regs a0
rtos_save_\name\()_coproc_norestore:
    /* Return from routine via s0, instead of ra */
    jr    s0
    .size rtos_save_\name\()_coproc, .-rtos_save_\name\()_coproc

.endm



#if SOC_CPU_HAS_HWLOOP

/**
 * @brief Macros to enable and disable the hardware loop feature on the current core
 */
.macro hwlp_enable scratch_reg=a0
    li      \scratch_reg, 1
    csrw    CSR_HWLP_STATE_REG, \scratch_reg
.endm

/**
 * @brief Disable HW Loop CPU feature while returning the former status in the given register
 */
.macro hwlp_disable reg
    csrrw \reg, CSR_HWLP_STATE_REG, zero
    /* Only keep the lowest two bits */
    andi  \reg, \reg, 0b11
    /* If register is 0, HWLP was off */
    beqz  \reg, 1f
    /* It was ON, return the enable bit in \reg */
    li    \reg, 1 << HWLP_COPROC_IDX
1:
.endm

/**
 * @brief Macros to save and restore the hardware loop registers to and from the given frame
 */
.macro hwlp_save_regs frame=sp
    csrr    a1, CSR_LOOP0_START_ADDR
    sw      a1, RV_HWLOOP_START0(\frame)
    csrr    a1, CSR_LOOP0_END_ADDR
    sw      a1, RV_HWLOOP_END0(\frame)
    csrr    a1, CSR_LOOP0_COUNT
    sw      a1, RV_HWLOOP_COUNT0(\frame)
    csrr    a1, CSR_LOOP1_START_ADDR
    sw      a1, RV_HWLOOP_START1(\frame)
    csrr    a1, CSR_LOOP1_END_ADDR
    sw      a1, RV_HWLOOP_END1(\frame)
    csrr    a1, CSR_LOOP1_COUNT
    sw      a1, RV_HWLOOP_COUNT1(\frame)
.endm

.macro hwlp_restore_regs frame=sp
    lw      a1, RV_HWLOOP_START0(\frame)
    csrw    CSR_LOOP0_START_ADDR, a1
    lw      a1, RV_HWLOOP_END0(\frame)
    csrw    CSR_LOOP0_END_ADDR, a1
    lw      a1, RV_HWLOOP_COUNT0(\frame)
    csrw    CSR_LOOP0_COUNT, a1
    lw      a1, RV_HWLOOP_START1(\frame)
    csrw    CSR_LOOP1_START_ADDR, a1
    lw      a1, RV_HWLOOP_END1(\frame)
    csrw    CSR_LOOP1_END_ADDR, a1
    lw      a1, RV_HWLOOP_COUNT1(\frame)
    csrw    CSR_LOOP1_COUNT, a1
.endm


generate_coprocessor_routine hwlp, HWLP_COPROC_IDX, hwlp_enable, hwlp_save_regs, hwlp_restore_regs

#endif /* SOC_CPU_HAS_HWLOOP */


#if SOC_CPU_HAS_PIE

/**
 * @brief Macros to enable and disable the hardware loop feature on the current core
 */
.macro pie_enable scratch_reg=a0
    li      \scratch_reg, 1
    csrw    CSR_PIE_STATE_REG, \scratch_reg
.endm

/**
 * @brief Disable HW Loop CPU feature while returning the former status in the given register
 */
.macro pie_disable reg
    csrrw \reg, CSR_PIE_STATE_REG, zero
    /* Only keep the lowest two bits, if register is 0, PIE was off */
    andi  \reg, \reg, 0b11
    beqz  \reg, 1f
    /* It was ON, return the enable bit in \reg */
    li    \reg, 1 << PIE_COPROC_IDX
1:
.endm

/**
 * @brief Macros to save and restore the hardware loop registers to and from the given frame
 */
.macro pie_save_regs frame=a0
    /* Save the 128-bit Q registers from the frame memory and then frame += 16 */
    esp.vst.128.ip  q0, \frame, 16
    esp.vst.128.ip  q1, \frame, 16
    esp.vst.128.ip  q2, \frame, 16
    esp.vst.128.ip  q4, \frame, 16
    esp.vst.128.ip  q5, \frame, 16
    esp.vst.128.ip  q6, \frame, 16
    esp.vst.128.ip  q7, \frame, 16
    /* Save the QACC_H and QACC_L registers, each being 256 bits big */
    esp.st.qacc.l.l.128.ip \frame, 16
    esp.st.qacc.l.h.128.ip \frame, 16
    esp.st.qacc.h.l.128.ip \frame, 16
    esp.st.qacc.h.h.128.ip \frame, 16
    /* UA_STATE register (128 bits) */
    esp.st.ua.state.ip \frame, 16
    /* XACC register (40 bits) */
    esp.st.u.xacc.ip \frame, 8
    /* The following registers will be stored in the same word */
    /* SAR register (6 bits) */
    esp.movx.r.sar a1
    slli a2, a1, 8
    /* SAR_BYTES register (4 bits) */
    esp.movx.r.sar.bytes a1
    slli a1, a1, 4
    or   a2, a2, a1
    /* FFT_BIT_WIDTH register (4 bits) */
    esp.movx.r.fft.bit.width a1
    or  a2, a2, a1
    sw  a2, (\frame)
.endm


.macro pie_restore_regs frame=a0
    /* Restore the 128-bit Q registers from the frame memory and then frame += 16 */
    esp.vld.128.ip  q0, \frame, 16
    esp.vld.128.ip  q1, \frame, 16
    esp.vld.128.ip  q2, \frame, 16
    esp.vld.128.ip  q4, \frame, 16
    esp.vld.128.ip  q5, \frame, 16
    esp.vld.128.ip  q6, \frame, 16
    esp.vld.128.ip  q7, \frame, 16
    /* Save the QACC_H and QACC_L registers, each being 256 bits big */
    esp.ld.qacc.l.l.128.ip \frame, 16
    esp.ld.qacc.l.h.128.ip \frame, 16
    esp.ld.qacc.h.l.128.ip \frame, 16
    esp.ld.qacc.h.h.128.ip \frame, 16
    /* UA_STATE register (128 bits) */
    esp.ld.ua.state.ip \frame, 16
    /* XACC register (40 bits) */
    esp.ld.xacc.ip \frame, 8
    /* The following registers are stored in the same word */
    lw  a2, (\frame)
    /* FFT_BIT_WIDTH register (4 bits) */
    andi a1, a2, 0xf
    esp.movx.w.sar a1
    /* SAR_BYTES register (4 bits) */
    srli a2, a2, 4
    andi a1, a2, 0xf
    esp.movx.w.sar.bytes a1
    /* SAR register (6 bits) */
    srli a2, a2, 4
    andi a1, a2, 0x3f
    esp.movx.w.fft.bit.width a1
.endm

generate_coprocessor_routine pie, PIE_COPROC_IDX, pie_enable, pie_save_regs, pie_restore_regs

#endif /* SOC_CPU_HAS_PIE */


#if SOC_CPU_HAS_FPU

/* Bit to set in mstatus to enable the FPU */
#define CSR_MSTATUS_FPU_ENABLE      (1 << 13)
/* Bit to clear in mstatus to disable the FPU */
#define CSR_MSTATUS_FPU_DISABLE     (3 << 13)

.macro fpu_save_regs frame=sp
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
    csrr    a1,   fcsr
    sw      a1,   RV_FPU_FCSR(\frame)
.endm

.macro fpu_restore_regs frame=sp
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
    lw      a1,   RV_FPU_FCSR(\frame)
    csrw    fcsr, a1
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
    li     \reg, CSR_MSTATUS_FPU_ENABLE
    csrs   mstatus, \reg
.endm


.macro fpu_disable reg
    li     \reg, CSR_MSTATUS_FPU_DISABLE
    csrc   mstatus, \reg
.endm

generate_coprocessor_routine fpu, FPU_COPROC_IDX, fpu_enable, fpu_save_regs, fpu_restore_regs

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
 * @returns Context that should be given to `rtos_int_exit`. On targets that have coprocessors,
 * this value is a bitmap where bit i is 1 if coprocessor i is enable, 0 if it is disabled.
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
    /* In case we jump, return value (a0) is correct */
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
    li      a0, 0                           /* return 0 in case we are going to branch */
    bnez    a1, rtos_int_enter_end          /* if (port_uxInterruptNesting[coreID] > 0) jump to rtos_int_enter_end */

    li      a7, 0
#if SOC_CPU_COPROC_NUM > 0
    /* Disable the coprocessors to forbid the ISR from using it */
#if SOC_CPU_HAS_HWLOOP
    /* The current HWLP status will be returned in a0 */
    hwlp_disable a0
    or      a7, a7, a0
#endif /* SOC_CPU_HAS_HWLOOP */

#if SOC_CPU_HAS_PIE
    /* The current HWLP status will be returned in a0 */
    pie_disable a0
    or      a7, a7, a0
#endif /* SOC_CPU_HAS_PIE */

#if SOC_CPU_HAS_FPU
    fpu_disable a0
#endif /* SOC_CPU_HAS_FPU */
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

    /* Return the coprocessor context from a7 */
    mv      a0, a7
rtos_int_enter_end:
    ret

/**
 * @brief Restore the stack pointer of the next task to run.
 *
 * @param a0 Former mstatus
 * @param a1 Context returned by `rtos_int_enter`. On targets that have coprocessors, this value is a bitmap
 *           where bit i is 1 if coprocessor i was enable, 0 if it was disabled.
 *
 * @returns New mstatus (potentially with coprocessors disabled)
 */
    .global rtos_int_exit
    .type rtos_int_exit, @function
rtos_int_exit:
    /* To speed up this routine and because this current routine is only meant to be called from the interrupt
     * handler, let's use callee-saved registers instead of stack space. Registers `s5-s11` are not used by
     * the caller */
    mv      s11, a0
#if SOC_CPU_COPROC_NUM > 0
    /* Save a1 as it contains the bitmap with the enabled coprocessors */
    mv      s8, a1
#endif

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
    la      s7, xPortSwitchFlag             /* a0 = &xPortSwitchFlag */
#if ( configNUM_CORES > 1 )
    add     s7, s7, a1                      /* a0 = &xPortSwitchFlag[coreID] // a1 already contains coreID * 4  */
#endif /* ( configNUM_CORES > 1 ) */
    lw      a0, 0(s7)                       /* a2 = xPortSwitchFlag[coreID] */
    beqz    a0, no_switch_restore_coproc    /* if (xPortSwitchFlag[coreID] == 0) jump to no_switch_restore_coproc */

    /* Preserve return address and schedule next task. To speed up the process, and because this current routine
     * is only meant to be called from the interrupt handle, let's save some speed and space by using callee-saved
     * registers instead of stack space. Registers `s3-s11` are not used by the caller */
    mv      s10, ra
#if ( SOC_CPU_COPROC_NUM > 0 )
    /* In the cases where the newly scheduled task is different from the previously running one,
     * we have to disable the coprocessors to let them trigger an exception on first use.
     * Else, if the same task is scheduled, restore the former coprocessors state (before the interrupt) */
    call    rtos_current_tcb
    /* Keep former TCB in s9 */
    mv      s9, a0
#endif
    call    vTaskSwitchContext
#if ( SOC_CPU_COPROC_NUM == 0 )
    mv      ra, s10                         /* Restore original return address */
#endif
    /* Clears the switch pending flag (stored in s7) */
    sw      zero, 0(s7)                     /* xPortSwitchFlag[coreID] = 0; */

#if ( SOC_CPU_COPROC_NUM > 0 )
    /* If the Task to schedule is NOT the same as the former one (s9), keep the coprocessors disabled */
    call    rtos_current_tcb
    mv      ra, s10                         /* Restore original return address */
    beq     a0, s9, no_switch_restore_coproc

#if SOC_CPU_HAS_FPU
    /* Disable the FPU in the `mstatus` value to return */
    li      a0, ~CSR_MSTATUS_FPU_DISABLE
    and     s11, s11, a0
#endif /* SOC_CPU_HAS_FPU */
    j       no_switch_restored

#endif /* ( SOC_CPU_COPROC_NUM > 0 ) */

no_switch_restore_coproc:
    /* We reach here either because there is no switch scheduled or because the TCB that is going to be scheduled
     * is the same as the one that has been interrupted. In both cases, we need to restore the coprocessors status */
#if SOC_CPU_HAS_HWLOOP
    andi    a0, s8, 1 << HWLP_COPROC_IDX
    beqz    a0, 1f
    hwlp_enable a0
1:
#endif /* SOC_CPU_HAS_HWLOOP */

#if SOC_CPU_HAS_PIE
    andi    a0, s8, 1 << PIE_COPROC_IDX
    beqz    a0, 1f
    pie_enable a0
1:
#endif /* SOC_CPU_HAS_PIE */

no_switch_restored:

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
