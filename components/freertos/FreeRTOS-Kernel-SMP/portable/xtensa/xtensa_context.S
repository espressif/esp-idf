/*
 * SPDX-FileCopyrightText: 2015-2019 Cadence Design Systems, Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2016-2022 Espressif Systems (Shanghai) CO LTD
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

/*
 * XTENSA CONTEXT SAVE AND RESTORE ROUTINES
 *
 * Low-level Call0 functions for handling generic context save and restore of
 * registers not specifically addressed by the interrupt vectors and handlers.
 * Those registers (not handled by these functions) are PC, PS, A0, A1 (SP).
 * Except for the calls to RTOS functions, this code is generic to Xtensa.
 *
 * Note that in Call0 ABI, interrupt handlers are expected to preserve the callee-
 * save regs (A12-A15), which is always the case if the handlers are coded in C.
 * However A12, A13 are made available as scratch registers for interrupt dispatch
 * code, so are presumed saved anyway, and are always restored even in Call0 ABI.
 * Only A14, A15 are truly handled as callee-save regs.
 *
 * Because Xtensa is a configurable architecture, this port supports all user
 * generated configurations (except restrictions stated in the release notes).
 * This is accomplished by conditional compilation using macros and functions
 * defined in the Xtensa HAL (hardware adaptation layer) for your configuration.
 * Only the processor state included in your configuration is saved and restored,
 * including any processor state added by user configuration options or TIE.
 */

/*  Warn nicely if this file gets named with a lowercase .s instead of .S:  */
#define NOERROR #
NOERROR: .error "C preprocessor needed for this file: make sure its filename\
 ends in uppercase .S, or use xt-xcc's -x assembler-with-cpp option."


#include "xtensa_rtos.h"
#include "xtensa_context.h"
#include "xt_asm_utils.h"

#ifdef XT_USE_OVLY
#include <xtensa/overlay_os_asm.h>
#endif

    .text

/*******************************************************************************

_xt_context_save

    !! MUST BE CALLED ONLY BY 'CALL0' INSTRUCTION !!

Saves all Xtensa processor state except PC, PS, A0, A1 (SP), A12, A13, in the
interrupt stack frame defined in xtensa_rtos.h.
Its counterpart is _xt_context_restore (which also restores A12, A13).

Caller is expected to have saved PC, PS, A0, A1 (SP), A12, A13 in the frame.
This function preserves A12 & A13 in order to provide the caller with 2 scratch
regs that need not be saved over the call to this function. The choice of which
2 regs to provide is governed by xthal_window_spill_nw and xthal_save_extra_nw,
to avoid moving data more than necessary. Caller can assign regs accordingly.

Entry Conditions:
    A0  = Return address in caller.
    A1  = Stack pointer of interrupted thread or handler ("interruptee").
    Original A12, A13 have already been saved in the interrupt stack frame.
    Other processor state except PC, PS, A0, A1 (SP), A12, A13, is as at the
    point of interruption.
    If windowed ABI, PS.EXCM = 1 (exceptions disabled).

Exit conditions:
    A0  = Return address in caller.
    A1  = Stack pointer of interrupted thread or handler ("interruptee").
    A12, A13 as at entry (preserved).
    If windowed ABI, PS.EXCM = 1 (exceptions disabled).

*******************************************************************************/

    .global _xt_context_save
    .type   _xt_context_save,@function
    .align  4
    .literal_position
    .align  4

_xt_context_save:

    s32i    a2,  sp, XT_STK_A2
    s32i    a3,  sp, XT_STK_A3
    s32i    a4,  sp, XT_STK_A4
    s32i    a5,  sp, XT_STK_A5
    s32i    a6,  sp, XT_STK_A6
    s32i    a7,  sp, XT_STK_A7
    s32i    a8,  sp, XT_STK_A8
    s32i    a9,  sp, XT_STK_A9
    s32i    a10, sp, XT_STK_A10
    s32i    a11, sp, XT_STK_A11

    /*
    Call0 ABI callee-saved regs a12-15 do not need to be saved here.
    a12-13 are the caller's responsibility so it can use them as scratch.
    So only need to save a14-a15 here for Windowed ABI (not Call0).
    */
    #ifndef __XTENSA_CALL0_ABI__
    s32i    a14, sp, XT_STK_A14
    s32i    a15, sp, XT_STK_A15
    #endif

    rsr     a3,  SAR
    s32i    a3,  sp, XT_STK_SAR

    #if XCHAL_HAVE_LOOPS
    rsr     a3,  LBEG
    s32i    a3,  sp, XT_STK_LBEG
    rsr     a3,  LEND
    s32i    a3,  sp, XT_STK_LEND
    rsr     a3,  LCOUNT
    s32i    a3,  sp, XT_STK_LCOUNT
    #endif

    #ifdef XT_USE_SWPRI
    /* Save virtual priority mask */
    movi    a3,  _xt_vpri_mask
    l32i    a3,  a3, 0
    s32i    a3,  sp, XT_STK_VPRI
    #endif

    #if XCHAL_EXTRA_SA_SIZE > 0 || !defined(__XTENSA_CALL0_ABI__)
    mov     a9,  a0                     /* preserve ret addr */
    #endif

    s32i    a12, sp, XT_STK_TMP0        /* temp. save stuff in stack frame */
    s32i    a13, sp, XT_STK_TMP1
    s32i    a9,  sp, XT_STK_TMP2

    l32i    a12, sp, XT_STK_A12         /* recover original a9,12,13 */
    l32i    a13, sp, XT_STK_A13
    l32i    a9,  sp, XT_STK_A9

    #if XCHAL_EXTRA_SA_SIZE > 0
    addi    a2,  sp, XT_STK_EXTRA       /* where to save it */
    # if XCHAL_EXTRA_SA_ALIGN > 16
    movi    a3, -XCHAL_EXTRA_SA_ALIGN
    and     a2, a2, a3                  /* align dynamically >16 bytes */
    # endif
    call0   xthal_save_extra_nw         /* destroys a0,2,3 */
    #endif

    #ifndef __XTENSA_CALL0_ABI__
    #ifdef XT_USE_OVLY
    l32i    a9,  sp, XT_STK_PC          /* recover saved PC */
    _xt_overlay_get_state    a9, a12, a13
    s32i    a9,  sp, XT_STK_OVLY        /* save overlay state */
    #endif

    /* SPILL_ALL_WINDOWS macro requires window overflow exceptions to be enabled,
     * i.e. PS.EXCM cleared and PS.WOE set.
     * Since we are going to clear PS.EXCM, we also need to increase INTLEVEL
     * at least to XCHAL_EXCM_LEVEL. This matches that value of effective INTLEVEL
     * at entry (CINTLEVEL=max(PS.INTLEVEL, XCHAL_EXCM_LEVEL) when PS.EXCM is set.
     * Since WindowOverflow exceptions will trigger inside SPILL_ALL_WINDOWS,
     * need to save/restore EPC1 as well.
     * Note: even though a4-a15 are saved into the exception frame, we should not
     * clobber them until after SPILL_ALL_WINDOWS. This is because these registers
     * may contain live windows belonging to previous frames in the call stack.
     * These frames will be spilled by SPILL_ALL_WINDOWS, and if the register was
     * used as a temporary by this code, the temporary value would get stored
     * onto the stack, instead of the real value.
     */
    rsr     a2, PS                     /* to be restored after SPILL_ALL_WINDOWS */
    movi    a0, PS_INTLEVEL_MASK
    and     a3, a2, a0                 /* get the current INTLEVEL */
    bgeui   a3, XCHAL_EXCM_LEVEL, 1f   /* calculate max(INTLEVEL, XCHAL_EXCM_LEVEL) */
    movi    a3, XCHAL_EXCM_LEVEL
1:
    movi    a0, PS_UM | PS_WOE         /* clear EXCM, enable window overflow, set new INTLEVEL */
    or      a3, a3, a0
    wsr     a3, ps
    rsr     a0, EPC1                   /* to be restored after SPILL_ALL_WINDOWS */

    addi    sp,  sp, XT_STK_FRMSZ      /* go back to spill register region */
    SPILL_ALL_WINDOWS                  /* place the live register windows there */
    addi    sp,  sp, -XT_STK_FRMSZ     /* return the current stack pointer and proceed with context save*/

    wsr     a2, PS                     /* restore to the value at entry */
    rsync
    wsr     a0, EPC1                   /* likewise */

    #endif /* __XTENSA_CALL0_ABI__ */

    l32i    a12, sp, XT_STK_TMP0       /* restore the temp saved registers */
    l32i    a13, sp, XT_STK_TMP1       /* our return address is there */
    l32i    a9,  sp, XT_STK_TMP2

    #if XCHAL_EXTRA_SA_SIZE > 0 || !defined(__XTENSA_CALL0_ABI__)
    mov     a0, a9                      /* retrieve ret addr */
    #endif

    ret

/*******************************************************************************

_xt_context_restore

    !! MUST BE CALLED ONLY BY 'CALL0' INSTRUCTION !!

Restores all Xtensa processor state except PC, PS, A0, A1 (SP) (and in Call0
ABI, A14, A15 which are preserved by all interrupt handlers) from an interrupt
stack frame defined in xtensa_rtos.h .
Its counterpart is _xt_context_save (whose caller saved A12, A13).

Caller is responsible to restore PC, PS, A0, A1 (SP).

Entry Conditions:
    A0  = Return address in caller.
    A1  = Stack pointer of interrupted thread or handler ("interruptee").

Exit conditions:
    A0  = Return address in caller.
    A1  = Stack pointer of interrupted thread or handler ("interruptee").
    Other processor state except PC, PS, A0, A1 (SP), is as at the point
    of interruption.

*******************************************************************************/

    .global _xt_context_restore
    .type   _xt_context_restore,@function
    .align  4
    .literal_position
    .align  4
_xt_context_restore:

    #if XCHAL_EXTRA_SA_SIZE > 0
    /*
    NOTE: Normally the xthal_restore_extra_nw macro only affects address
    registers a2-a5. It is theoretically possible for Xtensa processor
    designers to write TIE that causes more address registers to be
    affected, but it is generally unlikely. If that ever happens,
    more registers need to be saved/restored around this macro invocation.
    Here we only assume a13 is preserved.
    Future Xtensa tools releases might limit the regs that can be affected.
    */
    mov     a13, a0                     /* preserve ret addr */
    addi    a2,  sp, XT_STK_EXTRA       /* where to find it */
    # if XCHAL_EXTRA_SA_ALIGN > 16
    movi    a3, -XCHAL_EXTRA_SA_ALIGN
    and     a2, a2, a3                  /* align dynamically >16 bytes */
    # endif
    call0   xthal_restore_extra_nw      /* destroys a0,2,3,4,5 */
    mov     a0,  a13                    /* retrieve ret addr */
    #endif

    #if XCHAL_HAVE_LOOPS
    l32i    a2,  sp, XT_STK_LBEG
    l32i    a3,  sp, XT_STK_LEND
    wsr     a2,  LBEG
    l32i    a2,  sp, XT_STK_LCOUNT
    wsr     a3,  LEND
    wsr     a2,  LCOUNT
    #endif

    #ifdef XT_USE_OVLY
    /*
    If we are using overlays, this is a good spot to check if we need
    to restore an overlay for the incoming task. Here we have a bunch
    of registers to spare. Note that this step is going to use a few
    bytes of storage below SP (SP-20 to SP-32) if an overlay is going
    to be restored.
    */
    l32i    a2,  sp, XT_STK_PC          /* retrieve PC */
    l32i    a3,  sp, XT_STK_PS          /* retrieve PS */
    l32i    a4,  sp, XT_STK_OVLY        /* retrieve overlay state */
    l32i    a5,  sp, XT_STK_A1          /* retrieve stack ptr */
    _xt_overlay_check_map    a2, a3, a4, a5, a6
    s32i    a2,  sp, XT_STK_PC          /* save updated PC */
    s32i    a3,  sp, XT_STK_PS          /* save updated PS */
    #endif

    #ifdef XT_USE_SWPRI
    /* Restore virtual interrupt priority and interrupt enable */
    movi    a3,  _xt_intdata
    l32i    a4,  a3, 0                  /* a4 = _xt_intenable */
    l32i    a5,  sp, XT_STK_VPRI        /* a5 = saved _xt_vpri_mask */
    and     a4,  a4, a5
    wsr     a4,  INTENABLE              /* update INTENABLE */
    s32i    a5,  a3, 4                  /* restore _xt_vpri_mask */
    #endif

    l32i    a3,  sp, XT_STK_SAR
    l32i    a2,  sp, XT_STK_A2
    wsr     a3,  SAR
    l32i    a3,  sp, XT_STK_A3
    l32i    a4,  sp, XT_STK_A4
    l32i    a5,  sp, XT_STK_A5
    l32i    a6,  sp, XT_STK_A6
    l32i    a7,  sp, XT_STK_A7
    l32i    a8,  sp, XT_STK_A8
    l32i    a9,  sp, XT_STK_A9
    l32i    a10, sp, XT_STK_A10
    l32i    a11, sp, XT_STK_A11

    /*
    Call0 ABI callee-saved regs a12-15 do not need to be restored here.
    However a12-13 were saved for scratch before XT_RTOS_INT_ENTER(),
    so need to be restored anyway, despite being callee-saved in Call0.
    */
    l32i    a12, sp, XT_STK_A12
    l32i    a13, sp, XT_STK_A13
    #ifndef __XTENSA_CALL0_ABI__
    l32i    a14, sp, XT_STK_A14
    l32i    a15, sp, XT_STK_A15
    #endif

    ret


/*******************************************************************************

_xt_coproc_init

Initializes global co-processor management data, setting all co-processors
to "unowned". Leaves CPENABLE as it found it (does NOT clear it).

Called during initialization of the RTOS, before any threads run.

This may be called from normal Xtensa single-threaded application code which
might use co-processors. The Xtensa run-time initialization enables all
co-processors. They must remain enabled here, else a co-processor exception
might occur outside of a thread, which the exception handler doesn't expect.

Entry Conditions:
    Xtensa single-threaded run-time environment is in effect.
    No thread is yet running.

Exit conditions:
    None.

Obeys ABI conventions per prototype:
    void _xt_coproc_init(void)

*******************************************************************************/

#if XCHAL_CP_NUM > 0

    .global _xt_coproc_init
    .type   _xt_coproc_init,@function
    .align  4
    .literal_position
    .align  4
_xt_coproc_init:
    ENTRY0

    /* Initialize thread co-processor ownerships to 0 (unowned). */
    movi    a2, _xt_coproc_owner_sa         /* a2 = base of owner array */
    addi    a3, a2, (XCHAL_CP_MAX*portNUM_PROCESSORS) << 2       /* a3 = top+1 of owner array */
    movi    a4, 0                           /* a4 = 0 (unowned) */
1:  s32i    a4, a2, 0
    addi    a2, a2, 4
    bltu    a2, a3, 1b

    RET0

#endif


/*******************************************************************************

_xt_coproc_release

Releases any and all co-processors owned by a given thread. The thread is
identified by it's co-processor state save area defined in xtensa_context.h .

Must be called before a thread's co-proc save area is deleted to avoid
memory corruption when the exception handler tries to save the state.
May be called when a thread terminates or completes but does not delete
the co-proc save area, to avoid the exception handler having to save the
thread's co-proc state before another thread can use it (optimization).

Entry Conditions:
    A2  = Pointer to base of co-processor state save area.
    A3  = Core ID of the task (must be pinned) who's coproc ownership we are
          releasing.

Exit conditions:
    None.

Obeys ABI conventions per prototype:
    void _xt_coproc_release(void * coproc_sa_base, BaseType_t xTargetCoreID)

*******************************************************************************/

#if XCHAL_CP_NUM > 0

    .global _xt_coproc_release
    .type   _xt_coproc_release,@function
    .align  4
    .literal_position
    .align  4
_xt_coproc_release:
    ENTRY0                                  /* a2 = base of save area */
                                            /* a3 = xTargetCoreID */

    movi    a4, XCHAL_CP_MAX << 2           /* a4 = size of an owner array */
    mull    a4, a3, a4                      /* a4 = offset to the owner array of the target core */
    movi    a3, _xt_coproc_owner_sa         /* a3 = base of all owner arrays */
    add     a3, a3, a4                      /* a3 = base of owner array of the target core */
    addi    a4, a3, XCHAL_CP_MAX << 2       /* a4 = top+1 of owner array of the target core */
    movi    a5, 0                           /* a5 = 0 (unowned) */

    rsil    a6, XCHAL_EXCM_LEVEL            /* lock interrupts */
#if portNUM_PROCESSORS > 1
    /* If multicore, we must also acquire the _xt_coproc_owner_sa_lock spinlock
     * to ensure thread safe access of _xt_coproc_owner_sa between cores. */
    spinlock_take a7 a8 _xt_coproc_owner_sa_lock
#endif /* portNUM_PROCESSORS > 1 */

1:  l32i    a7, a3, 0                       /* a7 = owner at a3 */
    bne     a2, a7, 2f                      /* if (coproc_sa_base == owner) */
    s32i    a5, a3, 0                       /*   owner = unowned */
2:  addi    a3, a3, 1<<2                    /* a3 = next entry in owner array */
    bltu    a3, a4, 1b                      /* repeat until end of array */

#if portNUM_PROCESSORS > 1
    /* Release previously taken spinlock */
    spinlock_release a7 a8 _xt_coproc_owner_sa_lock
#endif /* portNUM_PROCESSORS > 1 */
    wsr     a6, PS                          /* restore interrupts */

    RET0

#endif


/*******************************************************************************
_xt_coproc_savecs

If there is a current thread and it has a coprocessor state save area, then
save all callee-saved state into this area. This function is called from the
solicited context switch handler. It calls a system-specific function to get
the coprocessor save area base address.

Entry conditions:
    - The thread being switched out is still the current thread.
    - CPENABLE state reflects which coprocessors are active.
    - Registers have been saved/spilled already.

Exit conditions:
    - All necessary CP callee-saved state has been saved.
    - Registers a2-a7, a13-a15 have been trashed.

Must be called from assembly code only, using CALL0.
*******************************************************************************/
#if XCHAL_CP_NUM > 0

    .extern     _xt_coproc_sa_offset   /* external reference */

    .global     _xt_coproc_savecs
    .type       _xt_coproc_savecs,@function
    .align  4
    .literal_position
    .align      4
_xt_coproc_savecs:

    /* At entry, CPENABLE should be showing which CPs are enabled. */

    rsr     a2, CPENABLE                /* a2 = which CPs are enabled      */
    beqz    a2, .Ldone                  /* quick exit if none              */
    mov     a14, a0                     /* save return address             */
    call0   XT_RTOS_CP_STATE            /* get address of CP save area     */
    mov     a0, a14                     /* restore return address          */
    beqz    a15, .Ldone                 /* if none then nothing to do      */
    s16i    a2, a15, XT_CP_CS_ST        /* save mask of CPs being stored   */
    movi    a13, _xt_coproc_sa_offset   /* array of CP save offsets        */
    l32i    a15, a15, XT_CP_ASA         /* a15 = base of aligned save area */

#if XCHAL_CP0_SA_SIZE
    bbci.l  a2, 0, 2f                   /* CP 0 not enabled                */
    l32i    a14, a13, 0                 /* a14 = _xt_coproc_sa_offset[0]   */
    add     a3, a14, a15                /* a3 = save area for CP 0         */
    xchal_cp0_store a3, a4, a5, a6, a7 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP1_SA_SIZE
    bbci.l  a2, 1, 2f                   /* CP 1 not enabled                */
    l32i    a14, a13, 4                 /* a14 = _xt_coproc_sa_offset[1]   */
    add     a3, a14, a15                /* a3 = save area for CP 1         */
    xchal_cp1_store a3, a4, a5, a6, a7 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP2_SA_SIZE
    bbci.l  a2, 2, 2f
    l32i    a14, a13, 8
    add     a3, a14, a15
    xchal_cp2_store a3, a4, a5, a6, a7 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP3_SA_SIZE
    bbci.l  a2, 3, 2f
    l32i    a14, a13, 12
    add     a3, a14, a15
    xchal_cp3_store a3, a4, a5, a6, a7 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP4_SA_SIZE
    bbci.l  a2, 4, 2f
    l32i    a14, a13, 16
    add     a3, a14, a15
    xchal_cp4_store a3, a4, a5, a6, a7 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP5_SA_SIZE
    bbci.l  a2, 5, 2f
    l32i    a14, a13, 20
    add     a3, a14, a15
    xchal_cp5_store a3, a4, a5, a6, a7 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP6_SA_SIZE
    bbci.l  a2, 6, 2f
    l32i    a14, a13, 24
    add     a3, a14, a15
    xchal_cp6_store a3, a4, a5, a6, a7 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP7_SA_SIZE
    bbci.l  a2, 7, 2f
    l32i    a14, a13, 28
    add     a3, a14, a15
    xchal_cp7_store a3, a4, a5, a6, a7 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

.Ldone:
    ret
#endif


/*******************************************************************************
_xt_coproc_restorecs

Restore any callee-saved coprocessor state for the incoming thread.
This function is called from coprocessor exception handling, when giving
ownership to a thread that solicited a context switch earlier. It calls a
system-specific function to get the coprocessor save area base address.

Entry conditions:
    - The incoming thread is set as the current thread.
    - CPENABLE is set up correctly for all required coprocessors.
    - a2 = mask of coprocessors to be restored.

Exit conditions:
    - All necessary CP callee-saved state has been restored.
    - CPENABLE - unchanged.
    - Registers a2-a7, a13-a15 have been trashed.

Must be called from assembly code only, using CALL0.
*******************************************************************************/
#if XCHAL_CP_NUM > 0

    .global     _xt_coproc_restorecs
    .type       _xt_coproc_restorecs,@function
    .align  4
    .literal_position
    .align      4
_xt_coproc_restorecs:

    mov     a14, a0                     /* save return address             */
    call0   XT_RTOS_CP_STATE            /* get address of CP save area     */
    mov     a0, a14                     /* restore return address          */
    beqz    a15, .Ldone2                /* if none then nothing to do      */
    l16ui   a3, a15, XT_CP_CS_ST        /* a3 = which CPs have been saved  */
    xor     a3, a3, a2                  /* clear the ones being restored   */
    s32i    a3, a15, XT_CP_CS_ST        /* update saved CP mask            */
    movi    a13, _xt_coproc_sa_offset   /* array of CP save offsets        */
    l32i    a15, a15, XT_CP_ASA         /* a15 = base of aligned save area */

#if XCHAL_CP0_SA_SIZE
    bbci.l  a2, 0, 2f                   /* CP 0 not enabled                */
    l32i    a14, a13, 0                 /* a14 = _xt_coproc_sa_offset[0]   */
    add     a3, a14, a15                /* a3 = save area for CP 0         */
    xchal_cp0_load a3, a4, a5, a6, a7 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP1_SA_SIZE
    bbci.l  a2, 1, 2f                   /* CP 1 not enabled                */
    l32i    a14, a13, 4                 /* a14 = _xt_coproc_sa_offset[1]   */
    add     a3, a14, a15                /* a3 = save area for CP 1         */
    xchal_cp1_load a3, a4, a5, a6, a7 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP2_SA_SIZE
    bbci.l  a2, 2, 2f
    l32i    a14, a13, 8
    add     a3, a14, a15
    xchal_cp2_load a3, a4, a5, a6, a7 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP3_SA_SIZE
    bbci.l  a2, 3, 2f
    l32i    a14, a13, 12
    add     a3, a14, a15
    xchal_cp3_load a3, a4, a5, a6, a7 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP4_SA_SIZE
    bbci.l  a2, 4, 2f
    l32i    a14, a13, 16
    add     a3, a14, a15
    xchal_cp4_load a3, a4, a5, a6, a7 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP5_SA_SIZE
    bbci.l  a2, 5, 2f
    l32i    a14, a13, 20
    add     a3, a14, a15
    xchal_cp5_load a3, a4, a5, a6, a7 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP6_SA_SIZE
    bbci.l  a2, 6, 2f
    l32i    a14, a13, 24
    add     a3, a14, a15
    xchal_cp6_load a3, a4, a5, a6, a7 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP7_SA_SIZE
    bbci.l  a2, 7, 2f
    l32i    a14, a13, 28
    add     a3, a14, a15
    xchal_cp7_load a3, a4, a5, a6, a7 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

.Ldone2:
    ret

#endif
