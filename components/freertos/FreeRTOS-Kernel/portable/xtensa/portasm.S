/*
 * SPDX-FileCopyrightText: 2015-2019 Cadence Design Systems, Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2016-2023 Espressif Systems (Shanghai) CO LTD
 */
/*
 * Copyright (c) 2015-2019 Cadence Design Systems, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "xtensa_rtos.h"
#include "sdkconfig.h"

#define TOPOFSTACK_OFFS                 0x00    /* StackType_t *pxTopOfStack */

.extern pxCurrentTCBs

#if XCHAL_CP_NUM > 0
/* Offsets used to get a task's coprocessor save area (CPSA) from its TCB */
.extern offset_pxEndOfStack
.extern offset_cpsa
#if configNUM_CORES > 1
/* Offset to TCB_t.xCoreID member. Used to pin unpinned tasks that use the FPU. */
.extern offset_xCoreID
#endif /* configNUM_CORES > 1 */
#endif /* XCHAL_CP_NUM > 0 */

/*
--------------------------------------------------------------------------------
  Macro get_cpsa_from_tcb - get the pointer to a task's CPSA form its TCB

  Entry:
  - "reg_A" contains a pointer to the task's TCB
  Exit:
  - "reg_A" contains pointer the the task's CPSA
  - "reg_B" clobbered

  The two arguments must be different AR registers.
--------------------------------------------------------------------------------
*/
#if XCHAL_CP_NUM > 0
    .macro get_cpsa_from_tcb reg_A reg_B
    /* Get TCB.pxEndOfStack from reg_A */
    movi    \reg_B, offset_pxEndOfStack     /* Move &offset_pxEndOfStack into reg_B */
    l32i    \reg_B, \reg_B, 0               /* Load offset_pxEndOfStack into reg_B */
    add     \reg_A, \reg_A, \reg_B          /* Calculate &pxEndOfStack to reg_A (&TCB + offset_pxEndOfStack) */
    l32i    \reg_A, \reg_A, 0               /* Load TCB.pxEndOfStack into reg_A */
    /* Offset to start of CP save area */
    movi    \reg_B, offset_cpsa             /* Move &offset_cpsa into reg_B */
    l32i    \reg_B, \reg_B, 0               /* Load offset_cpsa into reg_B */
    sub     \reg_A, \reg_A, \reg_B          /* Subtract offset_cpsa from pxEndOfStack to get to start of CP save area (unaligned) */
    /* Align down start of CP save area to 16 byte boundary */
    movi    \reg_B, ~(0xF)
    and     \reg_A, \reg_A, \reg_B          /* Align CP save area pointer to 16 bytes */
    .endm
#endif /* XCHAL_CP_NUM > 0 */

    .global     port_IntStack
    .global     port_switch_flag    //Required by sysview_tracing build
    .text

/*
*******************************************************************************
* _frxt_setup_switch
* void _frxt_setup_switch(void);
*
* Sets an internal flag indicating that a task switch is required on return
* from interrupt handling.
*
*******************************************************************************
*/
    .global     _frxt_setup_switch
    .type       _frxt_setup_switch,@function
    .align      4
_frxt_setup_switch:

    ENTRY(16)

    getcoreid a3
    movi    a2, port_switch_flag
    addx4   a2,  a3, a2
    movi    a3, 1
    s32i    a3, a2, 0

    RET(16)

/*
*******************************************************************************
*                                            _frxt_int_enter
*                                       void _frxt_int_enter(void)
*
* Implements the Xtensa RTOS porting layer's XT_RTOS_INT_ENTER function for
* freeRTOS. Saves the rest of the interrupt context (not already saved).
* May only be called from assembly code by the 'call0' instruction, with
* interrupts disabled.
* See the detailed description of the XT_RTOS_ENTER macro in xtensa_rtos.h.
*
*******************************************************************************
*/
    .globl  _frxt_int_enter
    .type   _frxt_int_enter,@function
    .align  4
_frxt_int_enter:

    /* Save a12-13 in the stack frame as required by _xt_context_save. */
    s32i    a12, a1, XT_STK_A12
    s32i    a13, a1, XT_STK_A13

    /* Save return address in a safe place (free a0). */
    mov     a12, a0

    /* Save the rest of the interrupted context (preserves A12-13). */
    call0   _xt_context_save

    /*
    Save interrupted task's SP in TCB only if not nesting.
    Manage nesting directly rather than call the generic IntEnter()
    (in windowed ABI we can't call a C function here anyway because PS.EXCM is still set).
    */
    getcoreid a4
    movi    a2,  port_xSchedulerRunning
    addx4   a2,  a4, a2
    movi    a3,  port_interruptNesting
    addx4   a3,  a4, a3
    l32i    a2,  a2, 0                  /* a2 = port_xSchedulerRunning     */
    beqz    a2,  1f                     /* scheduler not running, no tasks */
    l32i    a2,  a3, 0                  /* a2 = port_interruptNesting      */
    addi    a2,  a2, 1                  /* increment nesting count         */
    s32i    a2,  a3, 0                  /* save nesting count              */
    bnei    a2,  1, .Lnested            /* !=0 before incr, so nested      */

    movi    a2,  pxCurrentTCBs
    addx4   a2,  a4, a2
    l32i    a2,  a2, 0                  /* a2 = current TCB                */
    beqz    a2,  1f
    s32i    a1,  a2, TOPOFSTACK_OFFS    /* pxCurrentTCBs->pxTopOfStack = SP */
    movi    a1,  port_IntStack+configISR_STACK_SIZE   /* a1 = top of intr stack for CPU 0  */
    movi    a2,  configISR_STACK_SIZE   /* add configISR_STACK_SIZE * cpu_num to arrive at top of stack for cpu_num */
    mull    a2,  a4, a2
    add     a1,  a1, a2                 /* for current proc */

    #if CONFIG_FREERTOS_FPU_IN_ISR && XCHAL_CP_NUM > 0
    rsr     a3, CPENABLE                /* Restore thread scope CPENABLE */
    addi    sp, sp,-4                   /* ISR will manage FPU coprocessor by forcing */
    s32i    a3, a1, 0                   /* its trigger */
    #endif

.Lnested:
1:
    #if CONFIG_FREERTOS_FPU_IN_ISR && XCHAL_CP_NUM > 0
    movi    a3,  0              /* whilst ISRs pending keep CPENABLE exception active */
    wsr     a3,  CPENABLE
    rsync
    #endif

    mov     a0,  a12                    /* restore return addr and return  */
    ret

/*
*******************************************************************************
*                                            _frxt_int_exit
*                                       void _frxt_int_exit(void)
*
* Implements the Xtensa RTOS porting layer's XT_RTOS_INT_EXIT function for
* FreeRTOS. If required, calls vPortYieldFromInt() to perform task context
* switching, restore the (possibly) new task's context, and return to the
* exit dispatcher saved in the task's stack frame at XT_STK_EXIT.
* May only be called from assembly code by the 'call0' instruction. Does not
* return to caller.
* See the description of the XT_RTOS_ENTER macro in xtensa_rtos.h.
*
*******************************************************************************
*/
    .globl  _frxt_int_exit
    .type   _frxt_int_exit,@function
    .align  4
_frxt_int_exit:

    getcoreid a4
    movi    a2,  port_xSchedulerRunning
    addx4   a2,  a4, a2
    movi    a3,  port_interruptNesting
    addx4   a3,  a4, a3
    rsil    a0,  XCHAL_EXCM_LEVEL       /* lock out interrupts             */
    l32i    a2,  a2, 0                  /* a2 = port_xSchedulerRunning     */
    beqz    a2,  .Lnoswitch             /* scheduler not running, no tasks */
    l32i    a2,  a3, 0                  /* a2 = port_interruptNesting      */
    addi    a2,  a2, -1                 /* decrement nesting count         */
    s32i    a2,  a3, 0                  /* save nesting count              */
    bnez    a2,  .Lnesting              /* !=0 after decr so still nested  */

    #if CONFIG_FREERTOS_FPU_IN_ISR && XCHAL_CP_NUM > 0
    l32i    a3,  sp, 0                  /* Grab last CPENABLE before leave ISR */
    addi    sp,  sp, 4
    wsr     a3, CPENABLE
    rsync                               /* ensure CPENABLE was modified */
    #endif

    movi    a2,  pxCurrentTCBs
    addx4   a2,  a4, a2
    l32i    a2,  a2, 0                  /* a2 = current TCB                */
    beqz    a2,  1f                     /* no task ? go to dispatcher      */
    l32i    a1,  a2, TOPOFSTACK_OFFS    /* SP = pxCurrentTCBs->pxTopOfStack */

    movi    a2,  port_switch_flag       /* address of switch flag          */
    addx4   a2,  a4, a2                 /* point to flag for this cpu      */
    l32i    a3,  a2, 0                  /* a3 = port_switch_flag           */
    beqz    a3,  .Lnoswitch             /* flag = 0 means no switch reqd   */
    movi    a3,  0
    s32i    a3,  a2, 0                  /* zero out the flag for next time */

1:
    /*
    Call0 ABI callee-saved regs a12-15 need to be saved before possible preemption.
    However a12-13 were already saved by _frxt_int_enter().
    */
    #ifdef __XTENSA_CALL0_ABI__
    s32i    a14, a1, XT_STK_A14
    s32i    a15, a1, XT_STK_A15
    #endif

    #ifdef __XTENSA_CALL0_ABI__
    call0   vPortYieldFromInt       /* call dispatch inside the function; never returns */
    #else
    call4   vPortYieldFromInt       /* this one returns */
    call0   _frxt_dispatch          /* tail-call dispatcher */
    /* Never returns here. */
    #endif

.Lnoswitch:
    /*
    If we came here then about to resume the interrupted task.
    */

.Lnesting:
    /*
    We come here only if there was no context switch, that is if this
    is a nested interrupt, or the interrupted task was not preempted.
    In either case there's no need to load the SP.
    */

    /* Restore full context from interrupt stack frame */
    call0   _xt_context_restore

    /*
    Must return via the exit dispatcher corresponding to the entrypoint from which
    this was called. Interruptee's A0, A1, PS, PC are restored and the interrupt
    stack frame is deallocated in the exit dispatcher.
    */
    l32i    a0,  a1, XT_STK_EXIT
    ret


/*
**********************************************************************************************************
*                                           _frxt_timer_int
*                                      void _frxt_timer_int(void)
*
* Implements the Xtensa RTOS porting layer's XT_RTOS_TIMER_INT function for FreeRTOS.
* Called every timer interrupt.
* Manages the tick timer and calls xPortSysTickHandler() every tick.
* See the detailed description of the XT_RTOS_ENTER macro in xtensa_rtos.h.
*
* Callable from C (obeys ABI conventions). Implemented in assmebly code for performance.
*
**********************************************************************************************************
*/
#ifdef CONFIG_FREERTOS_SYSTICK_USES_CCOUNT
    .globl  _frxt_timer_int
    .type   _frxt_timer_int,@function
    .align  4
_frxt_timer_int:

    /*
    Xtensa timers work by comparing a cycle counter with a preset value.  Once the match occurs
    an interrupt is generated, and the handler has to set a new cycle count into the comparator.
    To avoid clock drift due to interrupt latency, the new cycle count is computed from the old,
    not the time the interrupt was serviced. However if a timer interrupt is ever serviced more
    than one tick late, it is necessary to process multiple ticks until the new cycle count is
    in the future, otherwise the next timer interrupt would not occur until after the cycle
    counter had wrapped (2^32 cycles later).

    do {
        ticks++;
        old_ccompare = read_ccompare_i();
        write_ccompare_i( old_ccompare + divisor );
        service one tick;
        diff = read_ccount() - old_ccompare;
    } while ( diff > divisor );
    */

    ENTRY(16)

    #ifdef CONFIG_PM_TRACE
    movi a6, 1 /* = ESP_PM_TRACE_TICK */
    getcoreid a7
    call4 esp_pm_trace_enter
    #endif // CONFIG_PM_TRACE

.L_xt_timer_int_catchup:

    /* Update the timer comparator for the next tick. */
    #ifdef XT_CLOCK_FREQ
    movi    a2, XT_TICK_DIVISOR         /* a2 = comparator increment          */
    #else
    movi    a3, _xt_tick_divisor
    l32i    a2, a3, 0                   /* a2 = comparator increment          */
    #endif
    rsr     a3, XT_CCOMPARE             /* a3 = old comparator value          */
    add     a4, a3, a2                  /* a4 = new comparator value          */
    wsr     a4, XT_CCOMPARE             /* update comp. and clear interrupt   */
    esync

    #ifdef __XTENSA_CALL0_ABI__
    /* Preserve a2 and a3 across C calls. */
    s32i    a2, sp, 4
    s32i    a3, sp, 8
    #endif

    /* Call the FreeRTOS tick handler (see port_systick.c). */
    #ifdef __XTENSA_CALL0_ABI__
    call0   xPortSysTickHandler
    #else
    call4   xPortSysTickHandler
    #endif

    #ifdef __XTENSA_CALL0_ABI__
    /* Restore a2 and a3. */
    l32i    a2, sp, 4
    l32i    a3, sp, 8
    #endif

    /* Check if we need to process more ticks to catch up. */
    esync                               /* ensure comparator update complete  */
    rsr     a4, CCOUNT                  /* a4 = cycle count                   */
    sub     a4, a4, a3                  /* diff = ccount - old comparator     */
    blt     a2, a4, .L_xt_timer_int_catchup  /* repeat while diff > divisor */

#ifdef CONFIG_PM_TRACE
    movi a6, 1 /* = ESP_PM_TRACE_TICK */
    getcoreid a7
    call4 esp_pm_trace_exit
#endif // CONFIG_PM_TRACE

    RET(16)
#endif // CONFIG_FREERTOS_SYSTICK_USES_CCOUNT

    /*
**********************************************************************************************************
*                                           _frxt_tick_timer_init
*                                      void _frxt_tick_timer_init(void)
*
* Initialize timer and timer interrrupt handler (_xt_tick_divisor_init() has already been been called).
* Callable from C (obeys ABI conventions on entry).
*
**********************************************************************************************************
*/
#ifdef CONFIG_FREERTOS_SYSTICK_USES_CCOUNT
    .globl  _frxt_tick_timer_init
    .type   _frxt_tick_timer_init,@function
    .align  4
_frxt_tick_timer_init:

    ENTRY(16)


    /* Set up the periodic tick timer (assume enough time to complete init). */
    #ifdef XT_CLOCK_FREQ
    movi    a3, XT_TICK_DIVISOR
    #else
    movi    a2, _xt_tick_divisor
    l32i    a3, a2, 0
    #endif
    rsr     a2, CCOUNT              /* current cycle count */
    add     a2, a2, a3              /* time of first timer interrupt */
    wsr     a2, XT_CCOMPARE         /* set the comparator */

    /*
    Enable the timer interrupt at the device level. Don't write directly
    to the INTENABLE register because it may be virtualized.
    */
    #ifdef __XTENSA_CALL0_ABI__
    movi    a2, XT_TIMER_INTEN
    call0   xt_ints_on
    #else
    movi    a6, XT_TIMER_INTEN
    movi    a3, xt_ints_on
    callx4  a3
    #endif

    RET(16)
#endif // CONFIG_FREERTOS_SYSTICK_USES_CCOUNT

/*
**********************************************************************************************************
*                                    DISPATCH THE HIGH READY TASK
*                                     void _frxt_dispatch(void)
*
* Switch context to the highest priority ready task, restore its state and dispatch control to it.
*
* This is a common dispatcher that acts as a shared exit path for all the context switch functions
* including vPortYield() and vPortYieldFromInt(), all of which tail-call this dispatcher
* (for windowed ABI vPortYieldFromInt() calls it indirectly via _frxt_int_exit() ).
*
* The Xtensa port uses different stack frames for solicited and unsolicited task suspension (see
* comments on stack frames in xtensa_context.h). This function restores the state accordingly.
* If restoring a task that solicited entry, restores the minimal state and leaves CPENABLE clear.
* If restoring a task that was preempted, restores all state including the task's CPENABLE.
*
* Entry:
*   pxCurrentTCBs  points to the TCB of the task to suspend,
*   Because it is tail-called without a true function entrypoint, it needs no 'entry' instruction.
*
* Exit:
*   If incoming task called vPortYield() (solicited), this function returns as if from vPortYield().
*   If incoming task was preempted by an interrupt, this function jumps to exit dispatcher.
*
**********************************************************************************************************
*/
    .globl  _frxt_dispatch
    .type   _frxt_dispatch,@function
    .align  4
_frxt_dispatch:

    #ifdef __XTENSA_CALL0_ABI__
    call0   vTaskSwitchContext  // Get next TCB to resume
    movi    a2, pxCurrentTCBs
    getcoreid a3
    addx4   a2,  a3, a2
    #else
    call4   vTaskSwitchContext  // Get next TCB to resume
    movi    a2, pxCurrentTCBs
    getcoreid a3
    addx4   a2,  a3, a2
    #endif
    l32i    a3,  a2, 0
    l32i    sp,  a3, TOPOFSTACK_OFFS     /* SP = next_TCB->pxTopOfStack;  */
    s32i    a3,  a2, 0

    /* Determine the type of stack frame. */
    l32i    a2,  sp, XT_STK_EXIT        /* exit dispatcher or solicited flag */
    bnez    a2,  .L_frxt_dispatch_stk

.L_frxt_dispatch_sol:

    /* Solicited stack frame. Restore minimal context and return from vPortYield(). */
    #if XCHAL_HAVE_THREADPTR
    l32i    a2,  sp, XT_SOL_THREADPTR
    wur.threadptr a2
    #endif
    l32i    a3,  sp, XT_SOL_PS
    #ifdef __XTENSA_CALL0_ABI__
    l32i    a12, sp, XT_SOL_A12
    l32i    a13, sp, XT_SOL_A13
    l32i    a14, sp, XT_SOL_A14
    l32i    a15, sp, XT_SOL_A15
    #endif
    l32i    a0,  sp, XT_SOL_PC
    #if XCHAL_CP_NUM > 0
    /* Ensure wsr.CPENABLE is complete (should be, it was cleared on entry). */
    rsync
    #endif
    /* As soons as PS is restored, interrupts can happen. No need to sync PS. */
    wsr     a3,  PS
    #ifdef __XTENSA_CALL0_ABI__
    addi    sp,  sp, XT_SOL_FRMSZ
    ret
    #else
    retw
    #endif

.L_frxt_dispatch_stk:

    #if XCHAL_CP_NUM > 0
    /* Restore CPENABLE from task's co-processor save area. */
    movi    a2, pxCurrentTCBs           /* cp_state =                       */
    getcoreid a3
    addx4   a2, a3, a2
    l32i    a2, a2, 0
    get_cpsa_from_tcb a2, a3            /* After this, pointer to CP save area is in a2, a3 is destroyed */
    l16ui   a3, a2, XT_CPENABLE         /* CPENABLE = cp_state->cpenable;   */
    wsr     a3, CPENABLE
    #endif

    /* Interrupt stack frame. Restore full context and return to exit dispatcher. */
    call0   _xt_context_restore

    /* In Call0 ABI, restore callee-saved regs (A12, A13 already restored). */
    #ifdef __XTENSA_CALL0_ABI__
    l32i    a14, sp, XT_STK_A14
    l32i    a15, sp, XT_STK_A15
    #endif

    #if XCHAL_CP_NUM > 0
    /* Ensure wsr.CPENABLE has completed. */
    rsync
    #endif

    /*
    Must return via the exit dispatcher corresponding to the entrypoint from which
    this was called. Interruptee's A0, A1, PS, PC are restored and the interrupt
    stack frame is deallocated in the exit dispatcher.
    */
    l32i    a0, sp, XT_STK_EXIT
    ret


/*
**********************************************************************************************************
*                            PERFORM A SOLICTED CONTEXT SWITCH (from a task)
*                                        void vPortYield(void)
*
* This function saves the minimal state needed for a solicited task suspension, clears CPENABLE,
* then tail-calls the dispatcher _frxt_dispatch() to perform the actual context switch
*
* At Entry:
*   pxCurrentTCBs  points to the TCB of the task to suspend
*   Callable from C (obeys ABI conventions on entry).
*
* Does not return to caller.
*
**********************************************************************************************************
*/
    .globl  vPortYield
    .type   vPortYield,@function
    .align  4
vPortYield:

    #ifdef __XTENSA_CALL0_ABI__
    addi    sp,  sp, -XT_SOL_FRMSZ
    #else
    entry   sp,  XT_SOL_FRMSZ
    #endif

    rsr     a2,  PS
    s32i    a0,  sp, XT_SOL_PC
    s32i    a2,  sp, XT_SOL_PS
    #if XCHAL_HAVE_THREADPTR
    rur.threadptr a2
    s32i    a2,  sp, XT_SOL_THREADPTR
    #endif
    #ifdef __XTENSA_CALL0_ABI__
    s32i    a12, sp, XT_SOL_A12         /* save callee-saved registers      */
    s32i    a13, sp, XT_SOL_A13
    s32i    a14, sp, XT_SOL_A14
    s32i    a15, sp, XT_SOL_A15
    #else
    /* Spill register windows. Calling xthal_window_spill() causes extra    */
    /* spills and reloads, so we will set things up to call the _nw version */
    /* instead to save cycles.                                              */
    movi    a6,  ~(PS_WOE_MASK|PS_INTLEVEL_MASK)  /* spills a4-a7 if needed */
    and     a2,  a2, a6                           /* clear WOE, INTLEVEL    */
    addi    a2,  a2, XCHAL_EXCM_LEVEL             /* set INTLEVEL           */
    wsr     a2,  PS
    rsync
    call0   xthal_window_spill_nw
    l32i    a2,  sp, XT_SOL_PS                    /* restore PS             */
    wsr     a2,  PS
    #endif

    rsil    a2,  XCHAL_EXCM_LEVEL       /* disable low/med interrupts       */

    #if XCHAL_CP_NUM > 0
    /* Save coprocessor callee-saved state (if any). At this point CPENABLE */
    /* should still reflect which CPs were in use (enabled).                */
    call0   _xt_coproc_savecs
    #endif

    movi    a2,  pxCurrentTCBs
    getcoreid a3
    addx4   a2,  a3, a2
    l32i    a2,  a2, 0                  /* a2 = pxCurrentTCBs                */
    movi    a3,  0
    s32i    a3,  sp, XT_SOL_EXIT        /* 0 to flag as solicited frame     */
    s32i    sp,  a2, TOPOFSTACK_OFFS    /* pxCurrentTCBs->pxTopOfStack = SP  */

    #if XCHAL_CP_NUM > 0
    /* Clear CPENABLE, also in task's co-processor state save area. */
    get_cpsa_from_tcb a2, a3            /* After this, pointer to CP save area is in a2, a3 is destroyed */
    movi    a3,  0
    wsr     a3,  CPENABLE
    beqz    a2,  1f
    s16i    a3,  a2, XT_CPENABLE        /* clear saved cpenable             */
1:
    #endif

    /* Tail-call dispatcher. */
    call0   _frxt_dispatch
    /* Never reaches here. */


/*
**********************************************************************************************************
*                         PERFORM AN UNSOLICITED CONTEXT SWITCH (from an interrupt)
*                                        void vPortYieldFromInt(void)
*
* This calls the context switch hook (removed), saves and clears CPENABLE, then tail-calls the dispatcher
* _frxt_dispatch() to perform the actual context switch.
*
* At Entry:
*   Interrupted task context has been saved in an interrupt stack frame at pxCurrentTCBs->pxTopOfStack.
*   pxCurrentTCBs  points to the TCB of the task to suspend,
*   Callable from C (obeys ABI conventions on entry).
*
* At Exit:
*   Windowed ABI defers the actual context switch until the stack is unwound to interrupt entry.
*   Call0 ABI tail-calls the dispatcher directly (no need to unwind) so does not return to caller.
*
**********************************************************************************************************
*/
    .globl  vPortYieldFromInt
    .type   vPortYieldFromInt,@function
    .align  4
vPortYieldFromInt:

    ENTRY(16)

    #if XCHAL_CP_NUM > 0
    /* Save CPENABLE in task's co-processor save area, and clear CPENABLE.  */
    movi    a2, pxCurrentTCBs           /* cp_state =                       */
    getcoreid a3
    addx4   a2, a3, a2
    l32i    a2, a2, 0

    get_cpsa_from_tcb a2, a3            /* After this, pointer to CP save area is in a2, a3 is destroyed */

    rsr     a3, CPENABLE
    s16i    a3, a2, XT_CPENABLE         /* cp_state->cpenable = CPENABLE;   */
    movi    a3, 0
    wsr     a3, CPENABLE                /* disable all co-processors        */
    #endif

    #ifdef __XTENSA_CALL0_ABI__
    /* Tail-call dispatcher. */
    call0   _frxt_dispatch
    /* Never reaches here. */
    #else
    RET(16)
    #endif

/*
**********************************************************************************************************
*                                        _frxt_task_coproc_state
*                                   void _frxt_task_coproc_state(void)
*
* Implements the Xtensa RTOS porting layer's XT_RTOS_CP_STATE function for FreeRTOS.
*
* May only be called when a task is running, not within an interrupt handler (returns 0 in that case).
* May only be called from assembly code by the 'call0' instruction. Does NOT obey ABI conventions.
* Returns in A15 a pointer to the base of the co-processor state save area for the current task.
* See the detailed description of the XT_RTOS_ENTER macro in xtensa_rtos.h.
*
**********************************************************************************************************
*/
#if XCHAL_CP_NUM > 0

    .globl  _frxt_task_coproc_state
    .type   _frxt_task_coproc_state,@function
    .align  4
_frxt_task_coproc_state:


    /* We can use a3 as a scratchpad, the instances of code calling XT_RTOS_CP_STATE don't seem to need it saved. */
    getcoreid a3
    movi    a15, port_xSchedulerRunning /* if (port_xSchedulerRunning              */
    addx4   a15, a3,a15
    l32i    a15, a15, 0
    beqz    a15, 1f
    movi    a15, port_interruptNesting  /* && port_interruptNesting == 0           */
    addx4   a15, a3, a15
    l32i    a15, a15, 0
    bnez    a15, 1f

    movi    a15, pxCurrentTCBs
    addx4   a15, a3, a15
    l32i    a15, a15, 0                 /* && pxCurrentTCBs != 0) {                 */

    beqz    a15, 2f
    get_cpsa_from_tcb a15, a3           /* After this, pointer to CP save area is in a15, a3 is destroyed */
    ret

1:  movi    a15, 0
2:  ret

#endif /* XCHAL_CP_NUM > 0 */

/*
**********************************************************************************************************
*                                        _frxt_coproc_exc_hook
*                                   void _frxt_coproc_exc_hook(void)
*
* Implements the Xtensa RTOS porting layer's XT_RTOS_CP_EXC_HOOK function for FreeRTOS.
*
* May only be called from assembly code by the 'call0' instruction. Does NOT obey ABI conventions.
* May only only use a2-4, a15 (all other regs must be preserved).
* See the detailed description of the XT_RTOS_ENTER macro in xtensa_rtos.h.
*
**********************************************************************************************************
*/
#if XCHAL_CP_NUM > 0

    .globl  _frxt_coproc_exc_hook
    .type   _frxt_coproc_exc_hook,@function
    .align  4
_frxt_coproc_exc_hook:

    #if configNUM_CORES > 1
    getcoreid a2                            /* a2 = xCurCoreID */
    /* if (port_xSchedulerRunning[xCurCoreID] == 0) */
    movi    a3, port_xSchedulerRunning
    addx4   a3, a2, a3
    l32i    a3, a3, 0
    beqz    a3, 1f                          /* Scheduler hasn't started yet. Return. */
    /* if (port_interruptNesting[xCurCoreID] != 0) */
    movi    a3, port_interruptNesting
    addx4   a3, a2, a3
    l32i    a3, a3, 0
    bnez    a3, 1f                          /* We are in an interrupt. Return*/
    /* CP operations are incompatible with unpinned tasks. Thus we pin the task
    to the current running core. */
    movi    a3, pxCurrentTCBs
    addx4   a3, a2, a3
    l32i    a3, a3, 0                       /* a3 = pxCurrentTCBs[xCurCoreID] */
    movi    a4, offset_xCoreID
    l32i    a4, a4, 0                       /* a4 = offset_xCoreID */
    add     a3, a3, a4                      /* a3 = &TCB.xCoreID */
    s32i    a2, a3, 0                       /* TCB.xCoreID = xCurCoreID */
1:
    #endif /* configNUM_CORES > 1 */

    ret

#endif /* XCHAL_CP_NUM > 0 */
