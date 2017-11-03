/*******************************************************************************
Copyright (c) 2006-2015 Cadence Design Systems Inc.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------

        XTENSA VECTORS AND LOW LEVEL HANDLERS FOR AN RTOS

  Xtensa low level exception and interrupt vectors and handlers for an RTOS.

  Interrupt handlers and user exception handlers support interaction with
  the RTOS by calling XT_RTOS_INT_ENTER and XT_RTOS_INT_EXIT before and
  after user's specific interrupt handlers. These macros are defined in
  xtensa_<rtos>.h to call suitable functions in a specific RTOS.

  Users can install application-specific interrupt handlers for low and
  medium level interrupts, by calling xt_set_interrupt_handler(). These
  handlers can be written in C, and must obey C calling convention. The
  handler table is indexed by the interrupt number. Each handler may be
  provided with an argument. 

  Note that the system timer interrupt is handled specially, and is
  dispatched to the RTOS-specific handler. This timer cannot be hooked
  by application code.

  Optional hooks are also provided to install a handler per level at 
  run-time, made available by compiling this source file with 
  '-DXT_INTEXC_HOOKS' (useful for automated testing).

!!  This file is a template that usually needs to be modified to handle       !!
!!  application specific interrupts. Search USER_EDIT for helpful comments    !!
!!  on where to insert handlers and how to write them.                        !!

  Users can also install application-specific exception handlers in the
  same way, by calling xt_set_exception_handler(). One handler slot is
  provided for each exception type. Note that some exceptions are handled
  by the porting layer itself, and cannot be taken over by application
  code in this manner. These are the alloca, syscall, and coprocessor
  exceptions.

  The exception handlers can be written in C, and must follow C calling
  convention. Each handler is passed a pointer to an exception frame as
  its single argument. The exception frame is created on the stack, and
  holds the saved context of the thread that took the exception. If the
  handler returns, the context will be restored and the instruction that
  caused the exception will be retried. If the handler makes any changes
  to the saved state in the exception frame, the changes will be applied
  when restoring the context.

  Because Xtensa is a configurable architecture, this port supports all user
  generated configurations (except restrictions stated in the release notes).
  This is accomplished by conditional compilation using macros and functions
  defined in the Xtensa HAL (hardware adaptation layer) for your configuration.
  Only the relevant parts of this file will be included in your RTOS build.
  For example, this file provides interrupt vector templates for all types and
  all priority levels, but only the ones in your configuration are built.

  NOTES on the use of 'call0' for long jumps instead of 'j':
   1. This file should be assembled with the -mlongcalls option to xt-xcc.
   2. The -mlongcalls compiler option causes 'call0 dest' to be expanded to
      a sequence 'l32r a0, dest' 'callx0 a0' which works regardless of the
      distance from the call to the destination. The linker then relaxes
      it back to 'call0 dest' if it determines that dest is within range.
      This allows more flexibility in locating code without the performance
      overhead of the 'l32r' literal data load in cases where the destination
      is in range of 'call0'. There is an additional benefit in that 'call0'
      has a longer range than 'j' due to the target being word-aligned, so 
      the 'l32r' sequence is less likely needed.
   3. The use of 'call0' with -mlongcalls requires that register a0 not be 
      live at the time of the call, which is always the case for a function 
      call but needs to be ensured if 'call0' is used as a jump in lieu of 'j'.
   4. This use of 'call0' is independent of the C function call ABI.

*******************************************************************************/

#include "xtensa_rtos.h"
#include "esp_panic.h"
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"

/*
  Define for workaround: pin no-cpu-affinity tasks to a cpu when fpu is used.
  Please change this when the tcb structure is changed
*/
#define TASKTCB_XCOREID_OFFSET (0x38+configMAX_TASK_NAME_LEN+3)&~3
.extern pxCurrentTCB

/* Enable stack backtrace across exception/interrupt - see below */
#ifdef CONFIG_FREERTOS_INTERRUPT_BACKTRACE
#define XT_DEBUG_BACKTRACE    1
#endif


/*
--------------------------------------------------------------------------------
  Defines used to access _xtos_interrupt_table.
--------------------------------------------------------------------------------
*/
#define XIE_HANDLER     0
#define XIE_ARG         4
#define XIE_SIZE        8


/*
  Macro get_percpu_entry_for - convert a per-core ID into a multicore entry.
  Basically does reg=reg*portNUM_PROCESSORS+current_core_id
  Multiple versions here to optimize for specific portNUM_PROCESSORS values.
*/
    .macro get_percpu_entry_for reg scratch
#if (portNUM_PROCESSORS == 1)
    /* No need to do anything */
#elif  (portNUM_PROCESSORS == 2)
    /* Optimized 2-core code. */
    getcoreid \scratch
    addx2 \reg,\reg,\scratch
#else
    /* Generalized n-core code. Untested! */
    movi \scratch,portNUM_PROCESSORS
    mull \scratch,\reg,\scratch
    getcoreid \reg
    add \reg,\scratch,\reg
#endif
   .endm
/*
--------------------------------------------------------------------------------
  Macro extract_msb - return the input with only the highest bit set.

  Input  : "ain"  - Input value, clobbered.
  Output : "aout" - Output value, has only one bit set, MSB of "ain".
  The two arguments must be different AR registers.
--------------------------------------------------------------------------------
*/

    .macro  extract_msb     aout ain
1:
    addi    \aout, \ain, -1         /* aout = ain - 1        */
    and     \ain, \ain, \aout       /* ain  = ain & aout     */
    bnez    \ain, 1b                /* repeat until ain == 0 */
    addi    \aout, \aout, 1         /* return aout + 1       */
    .endm

/*
--------------------------------------------------------------------------------
  Macro dispatch_c_isr - dispatch interrupts to user ISRs.
  This will dispatch to user handlers (if any) that are registered in the
  XTOS dispatch table (_xtos_interrupt_table). These handlers would have
  been registered by calling _xtos_set_interrupt_handler(). There is one
  exception - the timer interrupt used by the OS will not be dispatched
  to a user handler - this must be handled by the caller of this macro.

  Level triggered and software interrupts are automatically deasserted by
  this code.

  ASSUMPTIONS:
    -- PS.INTLEVEL is set to "level" at entry
    -- PS.EXCM = 0, C calling enabled

  NOTE: For CALL0 ABI, a12-a15 have not yet been saved.

  NOTE: This macro will use registers a0 and a2-a7. The arguments are:
    level -- interrupt level
    mask  -- interrupt bitmask for this level
--------------------------------------------------------------------------------
*/

    .macro  dispatch_c_isr    level  mask

    #ifdef CONFIG_PM_TRACE
    movi a6, 0 /* = ESP_PM_TRACE_IDLE */
    getcoreid a7
    call4 esp_pm_trace_exit
    #endif // CONFIG_PM_TRACE

    /* Get mask of pending, enabled interrupts at this level into a2. */

.L_xt_user_int_&level&:
    rsr     a2, INTENABLE
    rsr     a3, INTERRUPT
    movi    a4, \mask
    and     a2, a2, a3
    and     a2, a2, a4
    beqz    a2, 9f                          /* nothing to do */

    /* This bit of code provides a nice debug backtrace in the debugger.
       It does take a few more instructions, so undef XT_DEBUG_BACKTRACE
       if you want to save the cycles.
    */
    #ifdef XT_DEBUG_BACKTRACE
    #ifndef __XTENSA_CALL0_ABI__
    rsr     a0, EPC_1 + \level - 1          /* return address */
    movi    a4, 0xC0000000                  /* constant with top 2 bits set (call size) */
    or      a0, a0, a4                      /* set top 2 bits */
    addx2   a0, a4, a0                      /* clear top bit -- simulating call4 size   */
    #endif
    #endif

    #ifdef CONFIG_PM_ENABLE
    call4 esp_pm_impl_isr_hook
    #endif

    #ifdef XT_INTEXC_HOOKS
    /* Call interrupt hook if present to (pre)handle interrupts. */
    movi    a4, _xt_intexc_hooks
    l32i    a4, a4, \level << 2
    beqz    a4, 2f
    #ifdef __XTENSA_CALL0_ABI__
    callx0  a4
    beqz    a2, 9f
    #else
    mov     a6, a2
    callx4  a4
    beqz    a6, 9f
    mov     a2, a6
    #endif
2:
    #endif

    /* Now look up in the dispatch table and call user ISR if any. */
    /* If multiple bits are set then MSB has highest priority.     */

    extract_msb  a4, a2                     /* a4 = MSB of a2, a2 trashed */

    #ifdef XT_USE_SWPRI
    /* Enable all interrupts at this level that are numerically higher
       than the one we just selected, since they are treated as higher
       priority.
    */
    movi    a3, \mask                       /* a3 = all interrupts at this level */
    add     a2, a4, a4                      /* a2 = a4 << 1 */
    addi    a2, a2, -1                      /* a2 = mask of 1's <= a4 bit */
    and     a2, a2, a3                      /* a2 = mask of all bits <= a4 at this level */
    movi    a3, _xt_intdata
    l32i    a6, a3, 4                       /* a6 = _xt_vpri_mask */
    neg     a2, a2
    addi    a2, a2, -1                      /* a2 = mask to apply */
    and     a5, a6, a2                      /* mask off all bits <= a4 bit */
    s32i    a5, a3, 4                       /* update _xt_vpri_mask */
    rsr     a3, INTENABLE
    and     a3, a3, a2                      /* mask off all bits <= a4 bit */
    wsr     a3, INTENABLE
    rsil    a3, \level - 1                  /* lower interrupt level by 1 */
    #endif

    movi    a3, XT_TIMER_INTEN              /* a3 = timer interrupt bit */
    wsr     a4, INTCLEAR                    /* clear sw or edge-triggered interrupt */
    beq     a3, a4, 7f                      /* if timer interrupt then skip table */

    find_ms_setbit a3, a4, a3, 0            /* a3 = interrupt number */

    get_percpu_entry_for a3, a12
    movi    a4, _xt_interrupt_table
    addx8   a3, a3, a4                      /* a3 = address of interrupt table entry */
    l32i    a4, a3, XIE_HANDLER             /* a4 = handler address */
    #ifdef __XTENSA_CALL0_ABI__
    mov     a12, a6                         /* save in callee-saved reg */
    l32i    a2, a3, XIE_ARG                 /* a2 = handler arg */
    callx0  a4                              /* call handler */
    mov     a2, a12
    #else
    mov     a2, a6                          /* save in windowed reg */
    l32i    a6, a3, XIE_ARG                 /* a6 = handler arg */
    callx4  a4                              /* call handler */
    #endif

    #ifdef XT_USE_SWPRI
    j       8f
    #else
    j       .L_xt_user_int_&level&          /* check for more interrupts */
    #endif

7:

    .ifeq XT_TIMER_INTPRI - \level
.L_xt_user_int_timer_&level&:
    /*
    Interrupt handler for the RTOS tick timer if at this level.
    We'll be reading the interrupt state again after this call
    so no need to preserve any registers except a6 (vpri_mask).
    */

    #ifdef __XTENSA_CALL0_ABI__
    mov     a12, a6
    call0   XT_RTOS_TIMER_INT
    mov     a2, a12
    #else
    mov     a2, a6
    call4   XT_RTOS_TIMER_INT
    #endif
    .endif

    #ifdef XT_USE_SWPRI
    j       8f
    #else
    j       .L_xt_user_int_&level&          /* check for more interrupts */
    #endif

    #ifdef XT_USE_SWPRI
8:
    /* Restore old value of _xt_vpri_mask from a2. Also update INTENABLE from
       virtual _xt_intenable which _could_ have changed during interrupt
       processing. */

    movi    a3, _xt_intdata
    l32i    a4, a3, 0                       /* a4 = _xt_intenable    */
    s32i    a2, a3, 4                       /* update _xt_vpri_mask  */
    and     a4, a4, a2                      /* a4 = masked intenable */
    wsr     a4, INTENABLE                   /* update INTENABLE      */
    #endif

9:
    /* done */

    .endm


/*
--------------------------------------------------------------------------------
  Panic handler.
  Should be reached by call0 (preferable) or jump only. If call0, a0 says where 
  from. If on simulator, display panic message and abort, else loop indefinitely.
--------------------------------------------------------------------------------
*/

    .section .iram1,"ax"
    .global panicHandler

    .global     _xt_panic
    .type       _xt_panic,@function
    .align      4
    .literal_position
    .align      4

_xt_panic:
    /* Allocate exception frame and save minimal context. */
    mov     a0, sp
    addi    sp, sp, -XT_STK_FRMSZ
    s32i    a0, sp, XT_STK_A1
    #if XCHAL_HAVE_WINDOWED
    s32e    a0, sp, -12                     /* for debug backtrace */
    #endif
    rsr     a0, PS                          /* save interruptee's PS */
    s32i    a0, sp, XT_STK_PS
    rsr     a0, EPC_1                       /* save interruptee's PC */
    s32i    a0, sp, XT_STK_PC
    #if XCHAL_HAVE_WINDOWED
    s32e    a0, sp, -16                     /* for debug backtrace */
    #endif
    s32i    a12, sp, XT_STK_A12             /* _xt_context_save requires A12- */
    s32i    a13, sp, XT_STK_A13             /* A13 to have already been saved */
    call0   _xt_context_save

    /* Save exc cause and vaddr into exception frame */
    rsr     a0, EXCCAUSE
    s32i    a0, sp, XT_STK_EXCCAUSE
    rsr     a0, EXCVADDR
    s32i    a0, sp, XT_STK_EXCVADDR

    /* _xt_context_save seems to save the current a0, but we need the interuptees a0. Fix this. */
    rsr     a0, EXCSAVE_1                   /* save interruptee's a0 */

    s32i    a0, sp, XT_STK_A0

    /* Set up PS for C, disable all interrupts except NMI and debug, and clear EXCM. */
    movi    a0, PS_INTLEVEL(5) | PS_UM | PS_WOE
    wsr     a0, PS

    //Call panic handler
    mov     a6,sp
    call4 panicHandler


    .align 4
//Call using call0. Prints the hex char in a2. Kills a3, a4, a5
panic_print_hex:
    movi a3,0x60000000
    movi a4,8
panic_print_hex_loop:
    l32i a5, a3, 0x1c
    extui a5, a5, 16, 8
    bgei a5,64,panic_print_hex_loop

    srli a5,a2,28
    bgei a5,10,panic_print_hex_a
    addi a5,a5,'0'
    j panic_print_hex_ok
panic_print_hex_a:
    addi a5,a5,'A'-10
panic_print_hex_ok:
    s32i a5,a3,0
    slli a2,a2,4
    
    addi a4,a4,-1
    bnei a4,0,panic_print_hex_loop
    movi a5,' '
    s32i a5,a3,0

    ret



    .section    .rodata, "a"
    .align      4



/*
--------------------------------------------------------------------------------
    Hooks to dynamically install handlers for exceptions and interrupts.
    Allows automated regression frameworks to install handlers per test.
    Consists of an array of function pointers indexed by interrupt level, 
    with index 0 containing the entry for user exceptions.
    Initialized with all 0s, meaning no handler is installed at each level.
    See comment in xtensa_rtos.h for more details.

    *WARNING*  This array is for all CPUs, that is, installing a hook for 
    one CPU will install it for all others as well!
--------------------------------------------------------------------------------
*/

    #ifdef XT_INTEXC_HOOKS
    .data
    .global     _xt_intexc_hooks
    .type       _xt_intexc_hooks,@object
    .align      4

_xt_intexc_hooks:
    .fill       XT_INTEXC_HOOK_NUM, 4, 0
    #endif


/*
--------------------------------------------------------------------------------
  EXCEPTION AND LEVEL 1 INTERRUPT VECTORS AND LOW LEVEL HANDLERS
  (except window exception vectors).

  Each vector goes at a predetermined location according to the Xtensa
  hardware configuration, which is ensured by its placement in a special
  section known to the Xtensa linker support package (LSP). It performs
  the minimum necessary before jumping to the handler in the .text section.

  The corresponding handler goes in the normal .text section. It sets up
  the appropriate stack frame, saves a few vector-specific registers and
  calls XT_RTOS_INT_ENTER to save the rest of the interrupted context
  and enter the RTOS, then sets up a C environment. It then calls the
  user's interrupt handler code (which may be coded in C) and finally 
  calls XT_RTOS_INT_EXIT to transfer control to the RTOS for scheduling.

  While XT_RTOS_INT_EXIT does not return directly to the interruptee,
  eventually the RTOS scheduler will want to dispatch the interrupted
  task or handler. The scheduler will return to the exit point that was
  saved in the interrupt stack frame at XT_STK_EXIT.
--------------------------------------------------------------------------------
*/


/*
--------------------------------------------------------------------------------
Debug Exception.
--------------------------------------------------------------------------------
*/

#if XCHAL_HAVE_DEBUG

    .begin      literal_prefix .DebugExceptionVector
    .section    .DebugExceptionVector.text, "ax"
    .global     _DebugExceptionVector
    .align      4
    .global     xt_debugexception
_DebugExceptionVector:
    wsr     a0, EXCSAVE+XCHAL_DEBUGLEVEL    /* preserve a0 */
    call0   xt_debugexception            /* load exception handler */

    .end        literal_prefix

#endif

/*
--------------------------------------------------------------------------------
Double Exception.
Double exceptions are not a normal occurrence. They indicate a bug of some kind.
--------------------------------------------------------------------------------
*/

#ifdef XCHAL_DOUBLEEXC_VECTOR_VADDR

    .begin      literal_prefix .DoubleExceptionVector
    .section    .DoubleExceptionVector.text, "ax"
    .global     _DoubleExceptionVector
    .align      4

_DoubleExceptionVector:

    #if XCHAL_HAVE_DEBUG
    break   1, 4                            /* unhandled double exception */
    #endif
    movi    a0,PANIC_RSN_DOUBLEEXCEPTION
    wsr     a0,EXCCAUSE
    call0   _xt_panic                       /* does not return */
    rfde                                    /* make a0 point here not later */

    .end        literal_prefix

#endif /* XCHAL_DOUBLEEXC_VECTOR_VADDR */

/*
--------------------------------------------------------------------------------
Kernel Exception (including Level 1 Interrupt from kernel mode).
--------------------------------------------------------------------------------
*/

    .begin      literal_prefix .KernelExceptionVector
    .section    .KernelExceptionVector.text, "ax"
    .global     _KernelExceptionVector
    .align      4

_KernelExceptionVector:

    wsr     a0, EXCSAVE_1                   /* preserve a0 */
    call0   _xt_kernel_exc                  /* kernel exception handler */
    /* never returns here - call0 is used as a jump (see note at top) */

    .end        literal_prefix

    .section .iram1,"ax"
    .align      4

_xt_kernel_exc:
    #if XCHAL_HAVE_DEBUG
    break   1, 0                            /* unhandled kernel exception */
    #endif
    movi    a0,PANIC_RSN_KERNELEXCEPTION
    wsr     a0,EXCCAUSE
    call0   _xt_panic                       /* does not return */
    rfe                                     /* make a0 point here not there */


/*
--------------------------------------------------------------------------------
User Exception (including Level 1 Interrupt from user mode).
--------------------------------------------------------------------------------
*/

    .begin      literal_prefix .UserExceptionVector
    .section    .UserExceptionVector.text, "ax"
    .global     _UserExceptionVector
    .type       _UserExceptionVector,@function
    .align      4

_UserExceptionVector:

    wsr     a0, EXCSAVE_1                   /* preserve a0 */
    call0   _xt_user_exc                    /* user exception handler */
    /* never returns here - call0 is used as a jump (see note at top) */

    .end        literal_prefix

/*
--------------------------------------------------------------------------------
  Insert some waypoints for jumping beyond the signed 8-bit range of
  conditional branch instructions, so the conditional branchces to specific
  exception handlers are not taken in the mainline. Saves some cycles in the
  mainline.
--------------------------------------------------------------------------------
*/

    .section .iram1,"ax"

    #if XCHAL_HAVE_WINDOWED
    .align      4
_xt_to_alloca_exc:
    call0   _xt_alloca_exc                  /* in window vectors section */
    /* never returns here - call0 is used as a jump (see note at top) */
    #endif

    .align      4
_xt_to_syscall_exc:
    call0   _xt_syscall_exc
    /* never returns here - call0 is used as a jump (see note at top) */

    #if XCHAL_CP_NUM > 0
    .align      4
_xt_to_coproc_exc:
    call0   _xt_coproc_exc
    /* never returns here - call0 is used as a jump (see note at top) */
    #endif


/*
--------------------------------------------------------------------------------
  User exception handler.
--------------------------------------------------------------------------------
*/

    .type       _xt_user_exc,@function
    .align      4

_xt_user_exc:

    /* If level 1 interrupt then jump to the dispatcher */
    rsr     a0, EXCCAUSE
    beqi    a0, EXCCAUSE_LEVEL1INTERRUPT, _xt_lowint1

    /* Handle any coprocessor exceptions. Rely on the fact that exception
       numbers above EXCCAUSE_CP0_DISABLED all relate to the coprocessors.
    */
    #if XCHAL_CP_NUM > 0
    bgeui   a0, EXCCAUSE_CP0_DISABLED, _xt_to_coproc_exc
    #endif

    /* Handle alloca and syscall exceptions */
    #if XCHAL_HAVE_WINDOWED
    beqi    a0, EXCCAUSE_ALLOCA,  _xt_to_alloca_exc
    #endif
    beqi    a0, EXCCAUSE_SYSCALL, _xt_to_syscall_exc

    /* Handle all other exceptions. All can have user-defined handlers. */
    /* NOTE: we'll stay on the user stack for exception handling.       */

    /* Allocate exception frame and save minimal context. */
    mov     a0, sp
    addi    sp, sp, -XT_STK_FRMSZ
    s32i    a0, sp, XT_STK_A1
    #if XCHAL_HAVE_WINDOWED
    s32e    a0, sp, -12                     /* for debug backtrace */
    #endif
    rsr     a0, PS                          /* save interruptee's PS */
    s32i    a0, sp, XT_STK_PS
    rsr     a0, EPC_1                       /* save interruptee's PC */
    s32i    a0, sp, XT_STK_PC
    #if XCHAL_HAVE_WINDOWED
    s32e    a0, sp, -16                     /* for debug backtrace */
    #endif
    s32i    a12, sp, XT_STK_A12             /* _xt_context_save requires A12- */
    s32i    a13, sp, XT_STK_A13             /* A13 to have already been saved */
    call0   _xt_context_save

    /* Save exc cause and vaddr into exception frame */
    rsr     a0, EXCCAUSE
    s32i    a0, sp, XT_STK_EXCCAUSE
    rsr     a0, EXCVADDR
    s32i    a0, sp, XT_STK_EXCVADDR

    /* _xt_context_save seems to save the current a0, but we need the interuptees a0. Fix this. */
    rsr     a0, EXCSAVE_1                   /* save interruptee's a0 */
    s32i    a0, sp, XT_STK_A0

    /* Set up PS for C, reenable hi-pri interrupts, and clear EXCM. */
    #ifdef __XTENSA_CALL0_ABI__
    movi    a0, PS_INTLEVEL(XCHAL_EXCM_LEVEL) | PS_UM
    #else
    movi    a0, PS_INTLEVEL(XCHAL_EXCM_LEVEL) | PS_UM | PS_WOE
    #endif
    wsr     a0, PS

    #ifdef XT_DEBUG_BACKTRACE
    #ifndef __XTENSA_CALL0_ABI__
    rsr     a0, EPC_1                       /* return address for debug backtrace */
    movi    a5, 0xC0000000                  /* constant with top 2 bits set (call size) */
    rsync                                   /* wait for WSR.PS to complete */
    or      a0, a0, a5                      /* set top 2 bits */
    addx2   a0, a5, a0                      /* clear top bit -- thus simulating call4 size */
    #else
    rsync                                   /* wait for WSR.PS to complete */
    #endif
    #endif

    rsr     a2, EXCCAUSE                    /* recover exc cause */

    #ifdef XT_INTEXC_HOOKS
    /*
    Call exception hook to pre-handle exceptions (if installed).
    Pass EXCCAUSE in a2, and check result in a2 (if -1, skip default handling).
    */
    movi    a4, _xt_intexc_hooks
    l32i    a4, a4, 0                       /* user exception hook index 0 */
    beqz    a4, 1f
.Ln_xt_user_exc_call_hook:
    #ifdef __XTENSA_CALL0_ABI__
    callx0  a4
    beqi    a2, -1, .L_xt_user_done
    #else
    mov     a6, a2
    callx4  a4
    beqi    a6, -1, .L_xt_user_done
    mov     a2, a6
    #endif
1:
    #endif

    rsr     a2, EXCCAUSE                    /* recover exc cause */
    movi    a3, _xt_exception_table
    get_percpu_entry_for a2, a4
    addx4   a4, a2, a3                      /* a4 = address of exception table entry */
    l32i    a4, a4, 0                       /* a4 = handler address */
    #ifdef __XTENSA_CALL0_ABI__
    mov     a2, sp                          /* a2 = pointer to exc frame */
    callx0  a4                              /* call handler */
    #else
    mov     a6, sp                          /* a6 = pointer to exc frame */
    callx4  a4                              /* call handler */
    #endif

.L_xt_user_done:

    /* Restore context and return */
    call0   _xt_context_restore
    l32i    a0, sp, XT_STK_PS               /* retrieve interruptee's PS */
    wsr     a0, PS
    l32i    a0, sp, XT_STK_PC               /* retrieve interruptee's PC */
    wsr     a0, EPC_1
    l32i    a0, sp, XT_STK_A0               /* retrieve interruptee's A0 */
    l32i    sp, sp, XT_STK_A1               /* remove exception frame */
    rsync                                   /* ensure PS and EPC written */
    rfe                                     /* PS.EXCM is cleared */


/*
--------------------------------------------------------------------------------
  Exit point for dispatch. Saved in interrupt stack frame at XT_STK_EXIT
  on entry and used to return to a thread or interrupted interrupt handler.
--------------------------------------------------------------------------------
*/

    .global     _xt_user_exit
    .type       _xt_user_exit,@function
    .align      4
_xt_user_exit:
    l32i    a0, sp, XT_STK_PS               /* retrieve interruptee's PS */
    wsr     a0, PS
    l32i    a0, sp, XT_STK_PC               /* retrieve interruptee's PC */
    wsr     a0, EPC_1
    l32i    a0, sp, XT_STK_A0               /* retrieve interruptee's A0 */
    l32i    sp, sp, XT_STK_A1               /* remove interrupt stack frame */
    rsync                                   /* ensure PS and EPC written */
    rfe                                     /* PS.EXCM is cleared */


/*

--------------------------------------------------------------------------------
Syscall Exception Handler (jumped to from User Exception Handler).
Syscall 0 is required to spill the register windows (no-op in Call 0 ABI).
Only syscall 0 is handled here. Other syscalls return -1 to caller in a2.
--------------------------------------------------------------------------------
*/

    .section .iram1,"ax"
    .type       _xt_syscall_exc,@function
    .align      4
_xt_syscall_exc:

    #ifdef __XTENSA_CALL0_ABI__
    /*
    Save minimal regs for scratch. Syscall 0 does nothing in Call0 ABI.
    Use a minimal stack frame (16B) to save A2 & A3 for scratch.
    PS.EXCM could be cleared here, but unlikely to improve worst-case latency.
    rsr     a0, PS
    addi    a0, a0, -PS_EXCM_MASK
    wsr     a0, PS
    */
    addi    sp, sp, -16
    s32i    a2, sp, 8
    s32i    a3, sp, 12
    #else   /* Windowed ABI */
    /*
    Save necessary context and spill the register windows.
    PS.EXCM is still set and must remain set until after the spill.
    Reuse context save function though it saves more than necessary.
    For this reason, a full interrupt stack frame is allocated.
    */
    addi    sp, sp, -XT_STK_FRMSZ           /* allocate interrupt stack frame */
    s32i    a12, sp, XT_STK_A12             /* _xt_context_save requires A12- */
    s32i    a13, sp, XT_STK_A13             /* A13 to have already been saved */
    call0   _xt_context_save
    #endif

    /*
    Grab the interruptee's PC and skip over the 'syscall' instruction.
    If it's at the end of a zero-overhead loop and it's not on the last
    iteration, decrement loop counter and skip to beginning of loop.
    */
    rsr     a2, EPC_1                       /* a2 = PC of 'syscall' */
    addi    a3, a2, 3                       /* ++PC                 */
    #if XCHAL_HAVE_LOOPS
    rsr     a0, LEND                        /* if (PC == LEND       */
    bne     a3, a0, 1f
    rsr     a0, LCOUNT                      /*     && LCOUNT != 0)  */
    beqz    a0, 1f                          /* {                    */
    addi    a0, a0, -1                      /*   --LCOUNT           */
    rsr     a3, LBEG                        /*   PC = LBEG          */
    wsr     a0, LCOUNT                      /* }                    */
    #endif
1:  wsr     a3, EPC_1                       /* update PC            */

    /* Restore interruptee's context and return from exception. */
    #ifdef __XTENSA_CALL0_ABI__
    l32i    a2, sp, 8
    l32i    a3, sp, 12
    addi    sp, sp, 16
    #else
    call0   _xt_context_restore
    addi    sp, sp, XT_STK_FRMSZ
    #endif
    movi    a0, -1
    movnez  a2, a0, a2                      /* return -1 if not syscall 0 */
    rsr     a0, EXCSAVE_1
    rfe

/*
--------------------------------------------------------------------------------
Co-Processor Exception Handler (jumped to from User Exception Handler).
These exceptions are generated by co-processor instructions, which are only
allowed in thread code (not in interrupts or kernel code). This restriction is 
deliberately imposed to reduce the burden of state-save/restore in interrupts.
--------------------------------------------------------------------------------
*/
#if XCHAL_CP_NUM > 0

    .section .rodata, "a"

/* Offset to CP n save area in thread's CP save area. */
    .global _xt_coproc_sa_offset
    .type   _xt_coproc_sa_offset,@object
    .align  16                      /* minimize crossing cache boundaries */
_xt_coproc_sa_offset:
    .word   XT_CP0_SA, XT_CP1_SA, XT_CP2_SA, XT_CP3_SA
    .word   XT_CP4_SA, XT_CP5_SA, XT_CP6_SA, XT_CP7_SA

/* Bitmask for CP n's CPENABLE bit. */
    .type   _xt_coproc_mask,@object
    .align  16,,8                   /* try to keep it all in one cache line */
    .set    i, 0
_xt_coproc_mask:
    .rept   XCHAL_CP_MAX
    .long   (i<<16) | (1<<i)    // upper 16-bits = i, lower = bitmask
    .set    i, i+1
    .endr

    .data

/* Owner thread of CP n, identified by thread's CP save area (0 = unowned). */
    .global _xt_coproc_owner_sa
    .type   _xt_coproc_owner_sa,@object
    .align  16,,XCHAL_CP_MAX<<2     /* minimize crossing cache boundaries */
_xt_coproc_owner_sa:
    .space  (XCHAL_CP_MAX * portNUM_PROCESSORS) << 2

    .section .iram1,"ax"


    .align  4
.L_goto_invalid:
    j   .L_xt_coproc_invalid    /* not in a thread (invalid) */
    .align  4
.L_goto_done:
    j   .L_xt_coproc_done


/*
--------------------------------------------------------------------------------
  Coprocessor exception handler.
  At entry, only a0 has been saved (in EXCSAVE_1).
--------------------------------------------------------------------------------
*/

    .type   _xt_coproc_exc,@function
    .align  4

_xt_coproc_exc:

    /* Allocate interrupt stack frame and save minimal context. */
    mov     a0, sp                          /* sp == a1 */
    addi    sp, sp, -XT_STK_FRMSZ           /* allocate interrupt stack frame */
    s32i    a0, sp, XT_STK_A1               /* save pre-interrupt SP */
    #if XCHAL_HAVE_WINDOWED
    s32e    a0, sp, -12                     /* for debug backtrace */
    #endif
    rsr     a0, PS                          /* save interruptee's PS */
    s32i    a0, sp, XT_STK_PS
    rsr     a0, EPC_1                       /* save interruptee's PC */
    s32i    a0, sp, XT_STK_PC
    rsr     a0, EXCSAVE_1                   /* save interruptee's a0 */
    s32i    a0, sp, XT_STK_A0
    #if XCHAL_HAVE_WINDOWED
    s32e    a0, sp, -16                     /* for debug backtrace */
    #endif
    movi    a0, _xt_user_exit               /* save exit point for dispatch */
    s32i    a0, sp, XT_STK_EXIT

    rsr     a0, EXCCAUSE
    s32i    a5, sp, XT_STK_A5               /* save a5 */
    addi    a5, a0, -EXCCAUSE_CP0_DISABLED  /* a5 = CP index */

    /* Save a few more of interruptee's registers (a5 was already saved). */
    s32i    a2,  sp, XT_STK_A2
    s32i    a3,  sp, XT_STK_A3
    s32i    a4,  sp, XT_STK_A4
    s32i    a15, sp, XT_STK_A15

    /* Get co-processor state save area of new owner thread. */
    call0   XT_RTOS_CP_STATE                /* a15 = new owner's save area */
    beqz    a15, .L_goto_invalid            /* not in a thread (invalid) */

    /* Enable the co-processor's bit in CPENABLE. */
    movi    a0, _xt_coproc_mask
    rsr     a4, CPENABLE                    /* a4 = CPENABLE */
    addx4   a0, a5, a0                      /* a0 = &_xt_coproc_mask[n] */
    l32i    a0, a0, 0                       /* a0 = (n << 16) | (1 << n) */

    /* FPU operations are incompatible with non-pinned tasks. If we have a FPU operation
       here, to keep the entire thing from crashing, it's better to pin the task to whatever
       core we're running on now. */
    movi    a2, pxCurrentTCB
    getcoreid a3
    addx4     a2,  a3, a2
    l32i    a2, a2, 0                       /* a2 = start of pxCurrentTCB[cpuid] */
    addi    a2, a2, TASKTCB_XCOREID_OFFSET  /* offset to xCoreID in tcb struct */
    s32i    a3, a2, 0                       /* store current cpuid */

    /* Grab correct xt_coproc_owner_sa for this core */
    movi    a2, XCHAL_CP_MAX << 2
    mull    a2, a2, a3                      /* multiply by current processor id */
    movi    a3, _xt_coproc_owner_sa         /* a3 = base of owner array */
    add     a3, a3, a2                      /* a3 = owner area needed for this processor */

    extui   a2, a0, 0, 16                   /* coprocessor bitmask portion */
    or      a4, a4, a2                      /* a4 = CPENABLE | (1 << n) */
    wsr     a4, CPENABLE

/* 
Keep loading _xt_coproc_owner_sa[n] atomic (=load once, then use that value
everywhere): _xt_coproc_release assumes it works like this in order not to need
locking.
*/


    /* Get old coprocessor owner thread (save area ptr) and assign new one.  */
    addx4   a3,  a5, a3                      /* a3 = &_xt_coproc_owner_sa[n] */
    l32i    a2,  a3, 0                       /* a2 = old owner's save area */
    s32i    a15, a3, 0                       /* _xt_coproc_owner_sa[n] = new */
    rsync                                    /* ensure wsr.CPENABLE is complete */

    /* Only need to context switch if new owner != old owner. */
    beq     a15, a2, .L_goto_done           /* new owner == old, we're done */

    /* If no old owner then nothing to save. */
    beqz    a2, .L_check_new

    /* If old owner not actively using CP then nothing to save. */
    l16ui   a4,  a2,  XT_CPENABLE           /* a4 = old owner's CPENABLE */
    bnone   a4,  a0,  .L_check_new          /* old owner not using CP    */

.L_save_old:
    /* Save old owner's coprocessor state. */

    movi    a5, _xt_coproc_sa_offset

    /* Mark old owner state as no longer active (CPENABLE bit n clear). */
    xor     a4,  a4,  a0                    /* clear CP bit in CPENABLE    */
    s16i    a4,  a2,  XT_CPENABLE           /* update old owner's CPENABLE */

    extui   a4,  a0,  16,  5                /* a4 = CP index = n */
    addx4   a5,  a4,  a5                    /* a5 = &_xt_coproc_sa_offset[n] */

    /* Mark old owner state as saved (CPSTORED bit n set). */
    l16ui   a4,  a2,  XT_CPSTORED           /* a4 = old owner's CPSTORED */
    l32i    a5,  a5,  0                     /* a5 = XT_CP[n]_SA offset */
    or      a4,  a4,  a0                    /* set CP in old owner's CPSTORED */
    s16i    a4,  a2,  XT_CPSTORED           /* update old owner's CPSTORED */
    l32i    a2, a2, XT_CP_ASA               /* ptr to actual (aligned) save area */
    extui   a3, a0, 16, 5                   /* a3 = CP index = n */
    add     a2, a2, a5                      /* a2 = old owner's area for CP n */

    /*
    The config-specific HAL macro invoked below destroys a2-5, preserves a0-1.
    It is theoretically possible for Xtensa processor designers to write TIE 
    that causes more address registers to be affected, but it is generally 
    unlikely. If that ever happens, more registers needs to be saved/restored
    around this macro invocation, and the value in a15 needs to be recomputed.
    */
    xchal_cpi_store_funcbody

.L_check_new:
    /* Check if any state has to be restored for new owner. */
    /* NOTE: a15 = new owner's save area, cannot be zero when we get here. */

    l16ui   a3,  a15, XT_CPSTORED           /* a3 = new owner's CPSTORED */
    movi    a4, _xt_coproc_sa_offset
    bnone   a3,  a0,  .L_check_cs           /* full CP not saved, check callee-saved */
    xor     a3,  a3,  a0                    /* CPSTORED bit is set, clear it */
    s16i    a3,  a15, XT_CPSTORED           /* update new owner's CPSTORED */

    /* Adjust new owner's save area pointers to area for CP n. */
    extui   a3,  a0, 16, 5                  /* a3 = CP index = n */
    addx4   a4,  a3, a4                     /* a4 = &_xt_coproc_sa_offset[n] */
    l32i    a4,  a4, 0                      /* a4 = XT_CP[n]_SA */
    l32i    a5, a15, XT_CP_ASA              /* ptr to actual (aligned) save area */
    add     a2,  a4, a5                     /* a2 = new owner's area for CP */

    /*
    The config-specific HAL macro invoked below destroys a2-5, preserves a0-1.
    It is theoretically possible for Xtensa processor designers to write TIE 
    that causes more address registers to be affected, but it is generally 
    unlikely. If that ever happens, more registers needs to be saved/restored
    around this macro invocation.
    */
    xchal_cpi_load_funcbody

    /* Restore interruptee's saved registers. */
    /* Can omit rsync for wsr.CPENABLE here because _xt_user_exit does it. */
.L_xt_coproc_done:
    l32i    a15, sp, XT_STK_A15
    l32i    a5,  sp, XT_STK_A5
    l32i    a4,  sp, XT_STK_A4
    l32i    a3,  sp, XT_STK_A3
    l32i    a2,  sp, XT_STK_A2
    call0   _xt_user_exit                   /* return via exit dispatcher */
    /* Never returns here - call0 is used as a jump (see note at top) */

.L_check_cs:
    /* a0 = CP mask in low bits, a15 = new owner's save area */
    l16ui   a2, a15, XT_CP_CS_ST            /* a2 = mask of CPs saved    */
    bnone   a2,  a0, .L_xt_coproc_done      /* if no match then done     */
    and     a2,  a2, a0                     /* a2 = which CPs to restore */
    extui   a2,  a2, 0, 8                   /* extract low 8 bits        */
    s32i    a6,  sp, XT_STK_A6              /* save extra needed regs    */
    s32i    a7,  sp, XT_STK_A7
    s32i    a13, sp, XT_STK_A13
    s32i    a14, sp, XT_STK_A14
    call0   _xt_coproc_restorecs            /* restore CP registers      */
    l32i    a6,  sp, XT_STK_A6              /* restore saved registers   */
    l32i    a7,  sp, XT_STK_A7
    l32i    a13, sp, XT_STK_A13
    l32i    a14, sp, XT_STK_A14
    j       .L_xt_coproc_done

    /* Co-processor exception occurred outside a thread (not supported). */
.L_xt_coproc_invalid:
    movi    a0,PANIC_RSN_COPROCEXCEPTION
    wsr     a0,EXCCAUSE
    call0   _xt_panic                       /* not in a thread (invalid) */
    /* never returns */


#endif /* XCHAL_CP_NUM */


/*
-------------------------------------------------------------------------------
  Level 1 interrupt dispatch. Assumes stack frame has not been allocated yet.
-------------------------------------------------------------------------------
*/

    .section .iram1,"ax"
    .type       _xt_lowint1,@function
    .align      4

_xt_lowint1:
    mov     a0, sp                          /* sp == a1 */
    addi    sp, sp, -XT_STK_FRMSZ           /* allocate interrupt stack frame */
    s32i    a0, sp, XT_STK_A1               /* save pre-interrupt SP */
    rsr     a0, PS                          /* save interruptee's PS */
    s32i    a0, sp, XT_STK_PS
    rsr     a0, EPC_1                       /* save interruptee's PC */
    s32i    a0, sp, XT_STK_PC
    rsr     a0, EXCSAVE_1                   /* save interruptee's a0 */
    s32i    a0, sp, XT_STK_A0
    movi    a0, _xt_user_exit               /* save exit point for dispatch */
    s32i    a0, sp, XT_STK_EXIT

    /* Save rest of interrupt context and enter RTOS. */
    call0   XT_RTOS_INT_ENTER               /* common RTOS interrupt entry */

    /* !! We are now on the RTOS system stack !! */ 

    /* Set up PS for C, enable interrupts above this level and clear EXCM. */
    #ifdef __XTENSA_CALL0_ABI__
    movi    a0, PS_INTLEVEL(1) | PS_UM
    #else 
    movi    a0, PS_INTLEVEL(1) | PS_UM | PS_WOE
    #endif
    wsr     a0, PS
    rsync

    /* OK to call C code at this point, dispatch user ISRs */

    dispatch_c_isr 1 XCHAL_INTLEVEL1_MASK

    /* Done handling interrupts, transfer control to OS */
    call0   XT_RTOS_INT_EXIT                /* does not return directly here */


/*
-------------------------------------------------------------------------------
  MEDIUM PRIORITY (LEVEL 2+) INTERRUPT VECTORS AND LOW LEVEL HANDLERS.

  Medium priority interrupts are by definition those with priority greater
  than 1 and not greater than XCHAL_EXCM_LEVEL. These are disabled by
  setting PS.EXCM and therefore can easily support a C environment for
  handlers in C, and interact safely with an RTOS.

  Each vector goes at a predetermined location according to the Xtensa
  hardware configuration, which is ensured by its placement in a special
  section known to the Xtensa linker support package (LSP). It performs
  the minimum necessary before jumping to the handler in the .text section.

  The corresponding handler goes in the normal .text section. It sets up
  the appropriate stack frame, saves a few vector-specific registers and
  calls XT_RTOS_INT_ENTER to save the rest of the interrupted context
  and enter the RTOS, then sets up a C environment. It then calls the
  user's interrupt handler code (which may be coded in C) and finally 
  calls XT_RTOS_INT_EXIT to transfer control to the RTOS for scheduling.

  While XT_RTOS_INT_EXIT does not return directly to the interruptee,
  eventually the RTOS scheduler will want to dispatch the interrupted
  task or handler. The scheduler will return to the exit point that was
  saved in the interrupt stack frame at XT_STK_EXIT.
-------------------------------------------------------------------------------
*/

#if XCHAL_EXCM_LEVEL >= 2

    .begin      literal_prefix .Level2InterruptVector
    .section    .Level2InterruptVector.text, "ax"
    .global     _Level2Vector
    .type       _Level2Vector,@function
    .align      4
_Level2Vector:
    wsr     a0, EXCSAVE_2                   /* preserve a0 */
    call0   _xt_medint2                     /* load interrupt handler */
    /* never returns here - call0 is used as a jump (see note at top) */

    .end        literal_prefix

    .section .iram1,"ax"
    .type       _xt_medint2,@function
    .align      4
_xt_medint2:
    mov     a0, sp                          /* sp == a1 */
    addi    sp, sp, -XT_STK_FRMSZ           /* allocate interrupt stack frame */
    s32i    a0, sp, XT_STK_A1               /* save pre-interrupt SP */
    rsr     a0, EPS_2                       /* save interruptee's PS */
    s32i    a0, sp, XT_STK_PS
    rsr     a0, EPC_2                       /* save interruptee's PC */
    s32i    a0, sp, XT_STK_PC
    rsr     a0, EXCSAVE_2                   /* save interruptee's a0 */
    s32i    a0, sp, XT_STK_A0
    movi    a0, _xt_medint2_exit            /* save exit point for dispatch */
    s32i    a0, sp, XT_STK_EXIT

    /* Save rest of interrupt context and enter RTOS. */
    call0   XT_RTOS_INT_ENTER               /* common RTOS interrupt entry */

    /* !! We are now on the RTOS system stack !! */

    /* Set up PS for C, enable interrupts above this level and clear EXCM. */
    #ifdef __XTENSA_CALL0_ABI__
    movi    a0, PS_INTLEVEL(2) | PS_UM
    #else
    movi    a0, PS_INTLEVEL(2) | PS_UM | PS_WOE
    #endif
    wsr     a0, PS
    rsync

    /* OK to call C code at this point, dispatch user ISRs */

    dispatch_c_isr 2 XCHAL_INTLEVEL2_MASK

    /* Done handling interrupts, transfer control to OS */
    call0   XT_RTOS_INT_EXIT                /* does not return directly here */

    /*
    Exit point for dispatch. Saved in interrupt stack frame at XT_STK_EXIT
    on entry and used to return to a thread or interrupted interrupt handler.
    */
    .global     _xt_medint2_exit
    .type       _xt_medint2_exit,@function
    .align      4
_xt_medint2_exit:
    /* Restore only level-specific regs (the rest were already restored) */
    l32i    a0, sp, XT_STK_PS               /* retrieve interruptee's PS */
    wsr     a0, EPS_2
    l32i    a0, sp, XT_STK_PC               /* retrieve interruptee's PC */
    wsr     a0, EPC_2
    l32i    a0, sp, XT_STK_A0               /* retrieve interruptee's A0 */
    l32i    sp, sp, XT_STK_A1               /* remove interrupt stack frame */
    rsync                                   /* ensure EPS and EPC written */
    rfi     2

#endif  /* Level 2 */

#if XCHAL_EXCM_LEVEL >= 3

    .begin      literal_prefix .Level3InterruptVector
    .section    .Level3InterruptVector.text, "ax"
    .global     _Level3Vector
    .type       _Level3Vector,@function
    .align      4
_Level3Vector:
    wsr     a0, EXCSAVE_3                   /* preserve a0 */
    call0   _xt_medint3                     /* load interrupt handler */
    /* never returns here - call0 is used as a jump (see note at top) */

    .end        literal_prefix

    .section .iram1,"ax"
    .type       _xt_medint3,@function
    .align      4
_xt_medint3:
    mov     a0, sp                          /* sp == a1 */
    addi    sp, sp, -XT_STK_FRMSZ           /* allocate interrupt stack frame */
    s32i    a0, sp, XT_STK_A1               /* save pre-interrupt SP */
    rsr     a0, EPS_3                       /* save interruptee's PS */
    s32i    a0, sp, XT_STK_PS
    rsr     a0, EPC_3                       /* save interruptee's PC */
    s32i    a0, sp, XT_STK_PC
    rsr     a0, EXCSAVE_3                   /* save interruptee's a0 */
    s32i    a0, sp, XT_STK_A0
    movi    a0, _xt_medint3_exit            /* save exit point for dispatch */
    s32i    a0, sp, XT_STK_EXIT

    /* Save rest of interrupt context and enter RTOS. */
    call0   XT_RTOS_INT_ENTER               /* common RTOS interrupt entry */

    /* !! We are now on the RTOS system stack !! */

    /* Set up PS for C, enable interrupts above this level and clear EXCM. */
    #ifdef __XTENSA_CALL0_ABI__
    movi    a0, PS_INTLEVEL(3) | PS_UM
    #else
    movi    a0, PS_INTLEVEL(3) | PS_UM | PS_WOE
    #endif
    wsr     a0, PS
    rsync

    /* OK to call C code at this point, dispatch user ISRs */

    dispatch_c_isr 3 XCHAL_INTLEVEL3_MASK

    /* Done handling interrupts, transfer control to OS */
    call0   XT_RTOS_INT_EXIT                /* does not return directly here */

    /*
    Exit point for dispatch. Saved in interrupt stack frame at XT_STK_EXIT
    on entry and used to return to a thread or interrupted interrupt handler.
    */
    .global     _xt_medint3_exit
    .type       _xt_medint3_exit,@function
    .align      4
_xt_medint3_exit:
    /* Restore only level-specific regs (the rest were already restored) */
    l32i    a0, sp, XT_STK_PS               /* retrieve interruptee's PS */
    wsr     a0, EPS_3
    l32i    a0, sp, XT_STK_PC               /* retrieve interruptee's PC */
    wsr     a0, EPC_3
    l32i    a0, sp, XT_STK_A0               /* retrieve interruptee's A0 */
    l32i    sp, sp, XT_STK_A1               /* remove interrupt stack frame */
    rsync                                   /* ensure EPS and EPC written */
    rfi     3

#endif  /* Level 3 */

#if XCHAL_EXCM_LEVEL >= 4

    .begin      literal_prefix .Level4InterruptVector
    .section    .Level4InterruptVector.text, "ax"
    .global     _Level4Vector
    .type       _Level4Vector,@function
    .align      4
_Level4Vector:
    wsr     a0, EXCSAVE_4                   /* preserve a0 */
    call0   _xt_medint4                     /* load interrupt handler */

    .end        literal_prefix

    .section .iram1,"ax"
    .type       _xt_medint4,@function
    .align      4
_xt_medint4:
    mov     a0, sp                          /* sp == a1 */
    addi    sp, sp, -XT_STK_FRMSZ           /* allocate interrupt stack frame */
    s32i    a0, sp, XT_STK_A1               /* save pre-interrupt SP */
    rsr     a0, EPS_4                       /* save interruptee's PS */
    s32i    a0, sp, XT_STK_PS
    rsr     a0, EPC_4                       /* save interruptee's PC */
    s32i    a0, sp, XT_STK_PC
    rsr     a0, EXCSAVE_4                   /* save interruptee's a0 */
    s32i    a0, sp, XT_STK_A0
    movi    a0, _xt_medint4_exit            /* save exit point for dispatch */
    s32i    a0, sp, XT_STK_EXIT

    /* Save rest of interrupt context and enter RTOS. */
    call0   XT_RTOS_INT_ENTER               /* common RTOS interrupt entry */

    /* !! We are now on the RTOS system stack !! */

    /* Set up PS for C, enable interrupts above this level and clear EXCM. */
    #ifdef __XTENSA_CALL0_ABI__
    movi    a0, PS_INTLEVEL(4) | PS_UM
    #else
    movi    a0, PS_INTLEVEL(4) | PS_UM | PS_WOE
    #endif
    wsr     a0, PS
    rsync

    /* OK to call C code at this point, dispatch user ISRs */

    dispatch_c_isr 4 XCHAL_INTLEVEL4_MASK

    /* Done handling interrupts, transfer control to OS */
    call0   XT_RTOS_INT_EXIT                /* does not return directly here */

    /*
    Exit point for dispatch. Saved in interrupt stack frame at XT_STK_EXIT
    on entry and used to return to a thread or interrupted interrupt handler.
    */
    .global     _xt_medint4_exit
    .type       _xt_medint4_exit,@function
    .align      4
_xt_medint4_exit:
    /* Restore only level-specific regs (the rest were already restored) */
    l32i    a0, sp, XT_STK_PS               /* retrieve interruptee's PS */
    wsr     a0, EPS_4
    l32i    a0, sp, XT_STK_PC               /* retrieve interruptee's PC */
    wsr     a0, EPC_4
    l32i    a0, sp, XT_STK_A0               /* retrieve interruptee's A0 */
    l32i    sp, sp, XT_STK_A1               /* remove interrupt stack frame */
    rsync                                   /* ensure EPS and EPC written */
    rfi     4

#endif  /* Level 4 */

#if XCHAL_EXCM_LEVEL >= 5

    .begin      literal_prefix .Level5InterruptVector
    .section    .Level5InterruptVector.text, "ax"
    .global     _Level5Vector
    .type       _Level5Vector,@function
    .align      4
_Level5Vector:
    wsr     a0, EXCSAVE_5                   /* preserve a0 */
    call0   _xt_medint5                     /* load interrupt handler */

    .end        literal_prefix

    .section .iram1,"ax"
    .type       _xt_medint5,@function
    .align      4
_xt_medint5:
    mov     a0, sp                          /* sp == a1 */
    addi    sp, sp, -XT_STK_FRMSZ           /* allocate interrupt stack frame */
    s32i    a0, sp, XT_STK_A1               /* save pre-interrupt SP */
    rsr     a0, EPS_5                       /* save interruptee's PS */
    s32i    a0, sp, XT_STK_PS
    rsr     a0, EPC_5                       /* save interruptee's PC */
    s32i    a0, sp, XT_STK_PC
    rsr     a0, EXCSAVE_5                   /* save interruptee's a0 */
    s32i    a0, sp, XT_STK_A0
    movi    a0, _xt_medint5_exit            /* save exit point for dispatch */
    s32i    a0, sp, XT_STK_EXIT

    /* Save rest of interrupt context and enter RTOS. */
    call0   XT_RTOS_INT_ENTER               /* common RTOS interrupt entry */

    /* !! We are now on the RTOS system stack !! */

    /* Set up PS for C, enable interrupts above this level and clear EXCM. */
    #ifdef __XTENSA_CALL0_ABI__
    movi    a0, PS_INTLEVEL(5) | PS_UM
    #else
    movi    a0, PS_INTLEVEL(5) | PS_UM | PS_WOE
    #endif
    wsr     a0, PS
    rsync

    /* OK to call C code at this point, dispatch user ISRs */

    dispatch_c_isr 5 XCHAL_INTLEVEL5_MASK

    /* Done handling interrupts, transfer control to OS */
    call0   XT_RTOS_INT_EXIT                /* does not return directly here */

    /*
    Exit point for dispatch. Saved in interrupt stack frame at XT_STK_EXIT
    on entry and used to return to a thread or interrupted interrupt handler.
    */
    .global     _xt_medint5_exit
    .type       _xt_medint5_exit,@function
    .align      4
_xt_medint5_exit:
    /* Restore only level-specific regs (the rest were already restored) */
    l32i    a0, sp, XT_STK_PS               /* retrieve interruptee's PS */
    wsr     a0, EPS_5
    l32i    a0, sp, XT_STK_PC               /* retrieve interruptee's PC */
    wsr     a0, EPC_5
    l32i    a0, sp, XT_STK_A0               /* retrieve interruptee's A0 */
    l32i    sp, sp, XT_STK_A1               /* remove interrupt stack frame */
    rsync                                   /* ensure EPS and EPC written */
    rfi     5

#endif  /* Level 5 */

#if XCHAL_EXCM_LEVEL >= 6

    .begin      literal_prefix .Level6InterruptVector
    .section    .Level6InterruptVector.text, "ax"
    .global     _Level6Vector
    .type       _Level6Vector,@function
    .align      4
_Level6Vector:
    wsr     a0, EXCSAVE_6                   /* preserve a0 */
    call0   _xt_medint6                     /* load interrupt handler */

    .end        literal_prefix

    .section .iram1,"ax"
    .type       _xt_medint6,@function
    .align      4
_xt_medint6:
    mov     a0, sp                          /* sp == a1 */
    addi    sp, sp, -XT_STK_FRMSZ           /* allocate interrupt stack frame */
    s32i    a0, sp, XT_STK_A1               /* save pre-interrupt SP */
    rsr     a0, EPS_6                       /* save interruptee's PS */
    s32i    a0, sp, XT_STK_PS
    rsr     a0, EPC_6                       /* save interruptee's PC */
    s32i    a0, sp, XT_STK_PC
    rsr     a0, EXCSAVE_6                   /* save interruptee's a0 */
    s32i    a0, sp, XT_STK_A0
    movi    a0, _xt_medint6_exit            /* save exit point for dispatch */
    s32i    a0, sp, XT_STK_EXIT

    /* Save rest of interrupt context and enter RTOS. */
    call0   XT_RTOS_INT_ENTER               /* common RTOS interrupt entry */

    /* !! We are now on the RTOS system stack !! */

    /* Set up PS for C, enable interrupts above this level and clear EXCM. */
    #ifdef __XTENSA_CALL0_ABI__
    movi    a0, PS_INTLEVEL(6) | PS_UM
    #else
    movi    a0, PS_INTLEVEL(6) | PS_UM | PS_WOE
    #endif
    wsr     a0, PS
    rsync

    /* OK to call C code at this point, dispatch user ISRs */

    dispatch_c_isr 6 XCHAL_INTLEVEL6_MASK

    /* Done handling interrupts, transfer control to OS */
    call0   XT_RTOS_INT_EXIT                /* does not return directly here */

    /*
    Exit point for dispatch. Saved in interrupt stack frame at XT_STK_EXIT
    on entry and used to return to a thread or interrupted interrupt handler.
    */
    .global     _xt_medint6_exit
    .type       _xt_medint6_exit,@function
    .align      4
_xt_medint6_exit:
    /* Restore only level-specific regs (the rest were already restored) */
    l32i    a0, sp, XT_STK_PS               /* retrieve interruptee's PS */
    wsr     a0, EPS_6
    l32i    a0, sp, XT_STK_PC               /* retrieve interruptee's PC */
    wsr     a0, EPC_6
    l32i    a0, sp, XT_STK_A0               /* retrieve interruptee's A0 */
    l32i    sp, sp, XT_STK_A1               /* remove interrupt stack frame */
    rsync                                   /* ensure EPS and EPC written */
    rfi     6

#endif  /* Level 6 */


/*******************************************************************************

HIGH PRIORITY (LEVEL > XCHAL_EXCM_LEVEL) INTERRUPT VECTORS AND HANDLERS

High priority interrupts are by definition those with priorities greater
than XCHAL_EXCM_LEVEL. This includes non-maskable (NMI). High priority
interrupts cannot interact with the RTOS, that is they must save all regs
they use and not call any RTOS function.

A further restriction imposed by the Xtensa windowed architecture is that
high priority interrupts must not modify the stack area even logically
"above" the top of the interrupted stack (they need to provide their
own stack or static save area).

Cadence Design Systems recommends high priority interrupt handlers be coded in assembly
and used for purposes requiring very short service times.

Here are templates for high priority (level 2+) interrupt vectors.
They assume only one interrupt per level to avoid the burden of identifying
which interrupts at this level are pending and enabled. This allows for 
minimum latency and avoids having to save/restore a2 in addition to a0.
If more than one interrupt per high priority level is configured, this burden
is on the handler which in any case must provide a way to save and restore
registers it uses without touching the interrupted stack.

Each vector goes at a predetermined location according to the Xtensa
hardware configuration, which is ensured by its placement in a special
section known to the Xtensa linker support package (LSP). It performs
the minimum necessary before jumping to the handler in the .text section.

*******************************************************************************/

/*
These stubs just call xt_highintX/xt_nmi to handle the real interrupt. Please define
these in an external assembly source file. If these symbols are not defined anywhere
else, the defaults in xtensa_vector_defaults.S are used.
*/

#if XCHAL_NUM_INTLEVELS >=2 && XCHAL_EXCM_LEVEL <2 && XCHAL_DEBUGLEVEL !=2

    .begin      literal_prefix .Level2InterruptVector
    .section    .Level2InterruptVector.text, "ax"
    .global     _Level2Vector
    .type       _Level2Vector,@function
    .global     xt_highint2
    .align      4
_Level2Vector:
    wsr     a0, EXCSAVE_2                   /* preserve a0 */
    call0   xt_highint2                    /* load interrupt handler */

    .end        literal_prefix

#endif  /* Level 2 */

#if XCHAL_NUM_INTLEVELS >=3 && XCHAL_EXCM_LEVEL <3 && XCHAL_DEBUGLEVEL !=3

    .begin      literal_prefix .Level3InterruptVector
    .section    .Level3InterruptVector.text, "ax"
    .global     _Level3Vector
    .type       _Level3Vector,@function
    .global     xt_highint3
    .align      4
_Level3Vector:
    wsr     a0, EXCSAVE_3                   /* preserve a0 */
    call0   xt_highint3                    /* load interrupt handler */
    /* never returns here - call0 is used as a jump (see note at top) */

    .end        literal_prefix

#endif  /* Level 3 */

#if XCHAL_NUM_INTLEVELS >=4 && XCHAL_EXCM_LEVEL <4 && XCHAL_DEBUGLEVEL !=4

    .begin      literal_prefix .Level4InterruptVector
    .section    .Level4InterruptVector.text, "ax"
    .global     _Level4Vector
    .type       _Level4Vector,@function
    .global     xt_highint4
    .align      4
_Level4Vector:
    wsr     a0, EXCSAVE_4                   /* preserve a0 */
    call0   xt_highint4                    /* load interrupt handler */
    /* never returns here - call0 is used as a jump (see note at top) */

    .end        literal_prefix

#endif  /* Level 4 */

#if XCHAL_NUM_INTLEVELS >=5 && XCHAL_EXCM_LEVEL <5 && XCHAL_DEBUGLEVEL !=5

    .begin      literal_prefix .Level5InterruptVector
    .section    .Level5InterruptVector.text, "ax"
    .global     _Level5Vector
    .type       _Level5Vector,@function
    .global     xt_highint5
    .align      4
_Level5Vector:
    wsr     a0, EXCSAVE_5                   /* preserve a0 */
    call0   xt_highint5                    /* load interrupt handler */
    /* never returns here - call0 is used as a jump (see note at top) */

    .end        literal_prefix

#endif  /* Level 5 */

#if XCHAL_NUM_INTLEVELS >=6 && XCHAL_EXCM_LEVEL <6 && XCHAL_DEBUGLEVEL !=6

    .begin      literal_prefix .Level6InterruptVector
    .section    .Level6InterruptVector.text, "ax"
    .global     _Level6Vector
    .type       _Level6Vector,@function
    .global     xt_highint6
    .align      4
_Level6Vector:
    wsr     a0, EXCSAVE_6                   /* preserve a0 */
    call0   xt_highint6                    /* load interrupt handler */
    /* never returns here - call0 is used as a jump (see note at top) */

    .end        literal_prefix

#endif  /* Level 6 */

#if XCHAL_HAVE_NMI

    .begin      literal_prefix .NMIExceptionVector
    .section    .NMIExceptionVector.text, "ax"
    .global     _NMIExceptionVector
    .type       _NMIExceptionVector,@function
    .global     xt_nmi
    .align      4
_NMIExceptionVector:
    wsr     a0, EXCSAVE + XCHAL_NMILEVEL  _ /* preserve a0 */
    call0   xt_nmi                         /* load interrupt handler */
    /* never returns here - call0 is used as a jump (see note at top) */

    .end        literal_prefix

#endif  /* NMI */


/*******************************************************************************

WINDOW OVERFLOW AND UNDERFLOW EXCEPTION VECTORS AND ALLOCA EXCEPTION HANDLER

Here is the code for each window overflow/underflow exception vector and 
(interspersed) efficient code for handling the alloca exception cause.
Window exceptions are handled entirely in the vector area and are very
tight for performance. The alloca exception is also handled entirely in 
the window vector area so comes at essentially no cost in code size.
Users should never need to modify them and Cadence Design Systems recommends 
they do not.

Window handlers go at predetermined vector locations according to the
Xtensa hardware configuration, which is ensured by their placement in a
special section known to the Xtensa linker support package (LSP). Since
their offsets in that section are always the same, the LSPs do not define
a section per vector.

These things are coded for XEA2 only (XEA1 is not supported).

Note on Underflow Handlers:
The underflow handler for returning from call[i+1] to call[i]
must preserve all the registers from call[i+1]'s window.
In particular, a0 and a1 must be preserved because the RETW instruction
will be reexecuted (and may even underflow if an intervening exception
has flushed call[i]'s registers).
Registers a2 and up may contain return values.

*******************************************************************************/

#if XCHAL_HAVE_WINDOWED

    .section .WindowVectors.text, "ax"

/*
--------------------------------------------------------------------------------
Window Overflow Exception for Call4.

Invoked if a call[i] referenced a register (a4-a15)
that contains data from ancestor call[j];
call[j] had done a call4 to call[j+1].
On entry here:
    window rotated to call[j] start point;
        a0-a3 are registers to be saved;
        a4-a15 must be preserved;
        a5 is call[j+1]'s stack pointer.
--------------------------------------------------------------------------------
*/

    .org    0x0
    .global _WindowOverflow4
_WindowOverflow4:

    s32e    a0, a5, -16     /* save a0 to call[j+1]'s stack frame */
    s32e    a1, a5, -12     /* save a1 to call[j+1]'s stack frame */
    s32e    a2, a5,  -8     /* save a2 to call[j+1]'s stack frame */
    s32e    a3, a5,  -4     /* save a3 to call[j+1]'s stack frame */
    rfwo                    /* rotates back to call[i] position */

/*
--------------------------------------------------------------------------------
Window Underflow Exception for Call4

Invoked by RETW returning from call[i+1] to call[i]
where call[i]'s registers must be reloaded (not live in ARs);
where call[i] had done a call4 to call[i+1].
On entry here:
        window rotated to call[i] start point;
        a0-a3 are undefined, must be reloaded with call[i].reg[0..3];
        a4-a15 must be preserved (they are call[i+1].reg[0..11]);
        a5 is call[i+1]'s stack pointer.
--------------------------------------------------------------------------------
*/

    .org    0x40
    .global _WindowUnderflow4
_WindowUnderflow4:

    l32e    a0, a5, -16     /* restore a0 from call[i+1]'s stack frame */
    l32e    a1, a5, -12     /* restore a1 from call[i+1]'s stack frame */
    l32e    a2, a5,  -8     /* restore a2 from call[i+1]'s stack frame */
    l32e    a3, a5,  -4     /* restore a3 from call[i+1]'s stack frame */
    rfwu

/*
--------------------------------------------------------------------------------
Handle alloca exception generated by interruptee executing 'movsp'.
This uses space between the window vectors, so is essentially "free".
All interruptee's regs are intact except a0 which is saved in EXCSAVE_1,
and PS.EXCM has been set by the exception hardware (can't be interrupted).
The fact the alloca exception was taken means the registers associated with
the base-save area have been spilled and will be restored by the underflow
handler, so those 4 registers are available for scratch.
The code is optimized to avoid unaligned branches and minimize cache misses.
--------------------------------------------------------------------------------
*/

    .align  4
    .global _xt_alloca_exc
_xt_alloca_exc:

    rsr     a0, WINDOWBASE  /* grab WINDOWBASE before rotw changes it */
    rotw    -1              /* WINDOWBASE goes to a4, new a0-a3 are scratch */
    rsr     a2, PS
    extui   a3, a2, XCHAL_PS_OWB_SHIFT, XCHAL_PS_OWB_BITS
    xor     a3, a3, a4      /* bits changed from old to current windowbase */
    rsr     a4, EXCSAVE_1   /* restore original a0 (now in a4) */
    slli    a3, a3, XCHAL_PS_OWB_SHIFT
    xor     a2, a2, a3      /* flip changed bits in old window base */
    wsr     a2, PS          /* update PS.OWB to new window base */
    rsync

    _bbci.l a4, 31, _WindowUnderflow4
    rotw    -1              /* original a0 goes to a8 */
    _bbci.l a8, 30, _WindowUnderflow8
    rotw    -1
    j               _WindowUnderflow12

/*
--------------------------------------------------------------------------------
Window Overflow Exception for Call8

Invoked if a call[i] referenced a register (a4-a15)
that contains data from ancestor call[j];
call[j] had done a call8 to call[j+1].
On entry here:
    window rotated to call[j] start point;
        a0-a7 are registers to be saved;
        a8-a15 must be preserved;
        a9 is call[j+1]'s stack pointer.
--------------------------------------------------------------------------------
*/

    .org    0x80
    .global _WindowOverflow8
_WindowOverflow8:

    s32e    a0, a9, -16     /* save a0 to call[j+1]'s stack frame */
    l32e    a0, a1, -12     /* a0 <- call[j-1]'s sp
                               (used to find end of call[j]'s frame) */
    s32e    a1, a9, -12     /* save a1 to call[j+1]'s stack frame */
    s32e    a2, a9,  -8     /* save a2 to call[j+1]'s stack frame */
    s32e    a3, a9,  -4     /* save a3 to call[j+1]'s stack frame */
    s32e    a4, a0, -32     /* save a4 to call[j]'s stack frame */
    s32e    a5, a0, -28     /* save a5 to call[j]'s stack frame */
    s32e    a6, a0, -24     /* save a6 to call[j]'s stack frame */
    s32e    a7, a0, -20     /* save a7 to call[j]'s stack frame */
    rfwo                    /* rotates back to call[i] position */

/*
--------------------------------------------------------------------------------
Window Underflow Exception for Call8

Invoked by RETW returning from call[i+1] to call[i]
where call[i]'s registers must be reloaded (not live in ARs);
where call[i] had done a call8 to call[i+1].
On entry here:
        window rotated to call[i] start point;
        a0-a7 are undefined, must be reloaded with call[i].reg[0..7];
        a8-a15 must be preserved (they are call[i+1].reg[0..7]);
        a9 is call[i+1]'s stack pointer.
--------------------------------------------------------------------------------
*/

    .org    0xC0
    .global _WindowUnderflow8
_WindowUnderflow8:

    l32e    a0, a9, -16     /* restore a0 from call[i+1]'s stack frame */
    l32e    a1, a9, -12     /* restore a1 from call[i+1]'s stack frame */
    l32e    a2, a9,  -8     /* restore a2 from call[i+1]'s stack frame */
    l32e    a7, a1, -12     /* a7 <- call[i-1]'s sp
                               (used to find end of call[i]'s frame) */
    l32e    a3, a9,  -4     /* restore a3 from call[i+1]'s stack frame */
    l32e    a4, a7, -32     /* restore a4 from call[i]'s stack frame */
    l32e    a5, a7, -28     /* restore a5 from call[i]'s stack frame */
    l32e    a6, a7, -24     /* restore a6 from call[i]'s stack frame */
    l32e    a7, a7, -20     /* restore a7 from call[i]'s stack frame */
    rfwu

/*
--------------------------------------------------------------------------------
Window Overflow Exception for Call12

Invoked if a call[i] referenced a register (a4-a15)
that contains data from ancestor call[j];
call[j] had done a call12 to call[j+1].
On entry here:
    window rotated to call[j] start point;
        a0-a11 are registers to be saved;
        a12-a15 must be preserved;
        a13 is call[j+1]'s stack pointer.
--------------------------------------------------------------------------------
*/

    .org    0x100
    .global _WindowOverflow12
_WindowOverflow12:

    s32e    a0,  a13, -16   /* save a0 to call[j+1]'s stack frame */
    l32e    a0,  a1,  -12   /* a0 <- call[j-1]'s sp
                               (used to find end of call[j]'s frame) */
    s32e    a1,  a13, -12   /* save a1 to call[j+1]'s stack frame */
    s32e    a2,  a13,  -8   /* save a2 to call[j+1]'s stack frame */
    s32e    a3,  a13,  -4   /* save a3 to call[j+1]'s stack frame */
    s32e    a4,  a0,  -48   /* save a4 to end of call[j]'s stack frame */
    s32e    a5,  a0,  -44   /* save a5 to end of call[j]'s stack frame */
    s32e    a6,  a0,  -40   /* save a6 to end of call[j]'s stack frame */
    s32e    a7,  a0,  -36   /* save a7 to end of call[j]'s stack frame */
    s32e    a8,  a0,  -32   /* save a8 to end of call[j]'s stack frame */
    s32e    a9,  a0,  -28   /* save a9 to end of call[j]'s stack frame */
    s32e    a10, a0,  -24   /* save a10 to end of call[j]'s stack frame */
    s32e    a11, a0,  -20   /* save a11 to end of call[j]'s stack frame */
    rfwo                    /* rotates back to call[i] position */

/*
--------------------------------------------------------------------------------
Window Underflow Exception for Call12

Invoked by RETW returning from call[i+1] to call[i]
where call[i]'s registers must be reloaded (not live in ARs);
where call[i] had done a call12 to call[i+1].
On entry here:
        window rotated to call[i] start point;
        a0-a11 are undefined, must be reloaded with call[i].reg[0..11];
        a12-a15 must be preserved (they are call[i+1].reg[0..3]);
        a13 is call[i+1]'s stack pointer.
--------------------------------------------------------------------------------
*/

    .org 0x140
    .global _WindowUnderflow12
_WindowUnderflow12:

    l32e    a0,  a13, -16   /* restore a0 from call[i+1]'s stack frame */
    l32e    a1,  a13, -12   /* restore a1 from call[i+1]'s stack frame */
    l32e    a2,  a13,  -8   /* restore a2 from call[i+1]'s stack frame */
    l32e    a11, a1,  -12   /* a11 <- call[i-1]'s sp
                               (used to find end of call[i]'s frame) */
    l32e    a3,  a13,  -4   /* restore a3 from call[i+1]'s stack frame */
    l32e    a4,  a11, -48   /* restore a4 from end of call[i]'s stack frame */
    l32e    a5,  a11, -44   /* restore a5 from end of call[i]'s stack frame */
    l32e    a6,  a11, -40   /* restore a6 from end of call[i]'s stack frame */
    l32e    a7,  a11, -36   /* restore a7 from end of call[i]'s stack frame */
    l32e    a8,  a11, -32   /* restore a8 from end of call[i]'s stack frame */
    l32e    a9,  a11, -28   /* restore a9 from end of call[i]'s stack frame */
    l32e    a10, a11, -24   /* restore a10 from end of call[i]'s stack frame */
    l32e    a11, a11, -20   /* restore a11 from end of call[i]'s stack frame */
    rfwu

#endif /* XCHAL_HAVE_WINDOWED */

    .section    .UserEnter.text, "ax"
    .global     call_user_start
    .type       call_user_start,@function
    .align      4
    .literal_position



