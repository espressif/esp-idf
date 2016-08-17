
// xos_common.h - Macros and definitions common to C and assembly code.

// Copyright (c) 2003-2015 Cadence Design Systems, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#ifndef __XOS_COMMON_H__
#define __XOS_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <xtensa/corebits.h>
#include <xtensa/config/system.h>
#include <xtensa/config/tie.h>

#include "xos_params.h"


//-----------------------------------------------------------------------------
//  Macros that help define structures for both C and assembler.
//  These are somewhat different from the XTOS version in xtruntime-frames.h.
//-----------------------------------------------------------------------------
#if defined(_ASMLANGUAGE) || defined(__ASSEMBLER__)

#define STRUCT_BEGIN            .pushsection .text; .struct 0
#define STRUCT_FIELD(ctype,size,asname,name)    asname: .space  size
#define STRUCT_AFIELD(ctype,size,asname,name,n) asname: .space  (size)*(n)
#define STRUCT_END(sname)       sname##Size:; .popsection

#else

#define STRUCT_BEGIN            typedef struct {
#define STRUCT_FIELD(ctype,size,asname,name)    ctype   name;
#define STRUCT_AFIELD(ctype,size,asname,name,n) ctype   name[n];
#define STRUCT_END(sname)       } sname;

#endif //_ASMLANGUAGE || __ASSEMBLER__


//-----------------------------------------------------------------------------
//  Offsets relative to xos_globals.
//-----------------------------------------------------------------------------
#define XOS_INTLEVEL_MASK       0       // offset to the level mask
#define XOS_INTENABLE_MASK      4       // offset to the enable mask
#define XOS_CURR_THREADPTR      8       // offset to the current thread ptr
#define XOS_NEXT_THREADPTR      12      // offset to the next thread ptr
#define XOS_INTERRUPT_TABLE     16      // offset to the interrupt table


//-----------------------------------------------------------------------------
//  Offsets for xos_interrupt_table[] entries.
//-----------------------------------------------------------------------------
#define XOS_INTTAB_HANDLER      (XOS_INTERRUPT_TABLE+0)  // ofs to interrupt handler
#define XOS_INTTAB_ARG          (XOS_INTERRUPT_TABLE+4)  // ofs to interrupt handler arg
#define XOS_INTTAB_PS           (XOS_INTERRUPT_TABLE+8)  // (hwpri) PS for interrupt level
#define XOS_INTTAB_LEVEL        (XOS_INTERRUPT_TABLE+8)  // (swpri) interrupt level (1..7)
#define XOS_INTTAB_PRI          (XOS_INTERRUPT_TABLE+9)  // (swpri) interrupt priority (0..255)
#define XOS_INTTAB_PRIMASK      (XOS_INTERRUPT_TABLE+12) // (swpri) mask of higher pri. interrupts


//-----------------------------------------------------------------------------
//  Exception/interrupt stack frame layout for a pre-empted thread
//  tcb->resume_fn == &xos_resume_preempted_thread).
//  Pointed to by thread->esf. Located just below thread's current stack ptr.
//  Thread's a1 == thread->esf + XosExcFrameSize.
//  NOTE: exception frame size is a multiple of 16.
//-----------------------------------------------------------------------------
STRUCT_BEGIN
STRUCT_AFIELD(long,4,FRAME_AREG,areg, 12)       // a4-a15 (offsets 0 thru 44)
                                                // (a1 is computed, a0,a2-a3 are in s32e range of a1)
//#if XCHAL_HAVE_LOOPS
STRUCT_FIELD (long,4,FRAME_LBEG,lbeg)
STRUCT_FIELD (long,4,FRAME_LEND,lend)
STRUCT_FIELD (long,4,FRAME_LCOUNT,lcount)
//#endif
//#if XCHAL_HAVE_MAC16
STRUCT_FIELD (long,4,FRAME_ACCLO,acclo)
STRUCT_FIELD (char,1,FRAME_ACCHI,acchi)
//#endif
STRUCT_FIELD (char,1,FRAME_SAR,sar)
STRUCT_FIELD (short,2,FRAME_PAD0,pad0)          // unused
STRUCT_FIELD (long,4,FRAME_EXCCAUSE,exccause)
STRUCT_FIELD (long,4,FRAME_EXCVADDR,excvaddr)
STRUCT_FIELD (long,4,FRAME_PAD1,pad1)           // unused -- pad to make multiple of 16 bytes
STRUCT_FIELD (long,4,FRAME_PAD2,pad2)
STRUCT_FIELD (long,4,FRAME_PS,ps)               // (XOS_FRAME_SIZE-44) in S32E range of end
STRUCT_FIELD (long,4,FRAME_PC,pc)               // (XOS_FRAME_SIZE-40) in S32E range of end
STRUCT_FIELD (long,4,FRAME_A0,a0)
STRUCT_FIELD (long,4,FRAME_A2,a2)               // (XOS_FRAME_SIZE-32) in S32E range of end
STRUCT_FIELD (long,4,FRAME_A3,a3)               // (XOS_FRAME_SIZE-28) in S32E range of end
STRUCT_FIELD (long,4,FRAME_LEVELMASK,levelmask) //
STRUCT_FIELD (long,4,FRAME_NESTCHAIN,nestchain) // nested C function call chain ptr
// Caller's a0-a3 save area below SP. These fields MUST be the last ones in the
// struct so that they are guaranteed to be just under the original SP (before
// we allocate the exception frame).
STRUCT_AFIELD (long,4,FRAME_CWINSAVE,cwinsave, 4)  // (XOS_FRAME_SIZE-16)
STRUCT_END(XosExcFrame)                         // NOTE: exception frame size is 128

#define FRAME_AR(x)         (FRAME_AREG + x*4 - 16)

#if defined(_ASMLANGUAGE) || defined(__ASSEMBLER__)
#define XOS_FRAME_SIZE      XosExcFrameSize
#else
#define XOS_FRAME_SIZE      sizeof(XosExcFrame)
#endif


//-----------------------------------------------------------------------------
//  Stack frame layout for a cooperatively switched out thread
//  (tcb->resume_fn == &xos_resume_cooperative_thread).
//  Pointed to by thread->esf.  This is a function frame.
//  Thread's a1 == thread->esf.
//-----------------------------------------------------------------------------
STRUCT_BEGIN
STRUCT_FIELD (long,4,CFRAME_A0,a0)              // return PC
STRUCT_FIELD (long,4,CFRAME_LEVELMASK,levelmask)
STRUCT_FIELD (long,4,CFRAME_PS,ps)
#ifdef __XTENSA_CALL0_ABI__
STRUCT_FIELD (long,4,CFRAME_PAD0,pad0)
STRUCT_AFIELD(long,4,CFRAME_AREG,areg,4)        // callee-saved regs a12-a15
#endif
STRUCT_END(XosCoopFrame)


//-----------------------------------------------------------------------------
//  Offsets into thread control block (must match xos_thread.h !!)
//-----------------------------------------------------------------------------
#define TCB_RESUME_FN           12      // ptr to thread resume asm sequence
#define TCB_STACK_ESF           16      // saved stack ptr (actually, ptr to ESF)
#define TCB_TIE_SAVE            20      // ptr to TIE save area
#define TCB_RETVALUE            24      // ptr to xos_block return value
#define TCB_STACK_END           36      // ptr to end of stack (thread's initial stack ptr)
#define TCB_STARTUP_ENTRY       40      // ptr to thread entry function
#define TCB_STARTUP_ARG         44      // ptr to thread entry function's arg
#define TCB_READY               48      // thread ready state (1 byte)
#define TCB_CLIB_PTR            108     // thread C lib context pointer

#define TCB_RESUME_CCOUNT       116     // cycle count at last resume
#define TCB_CYCLE_COUNT         120     // number of cycles consumed
#define TCB_NORMAL_RESUMES      128     // number of cooperative/restart thread resumes
#define TCB_PREEMPT_RESUMES     132     // number of pre-emptive thread resumes


//-----------------------------------------------------------------------------
//  Coprocessor state handling:
//  The coprocessor state save area is allocated on the thread stack. The stack
//  must be sized appropriately. Threads that do not use coprocessors need not
//  allocate the storage area.
//
//  Along with the save area for each coprocessor, two bitmasks with flags per
//  coprocessor (laid out as in the CPENABLE reg) help manage context switching
//  coprocessors as efficiently as possible:
//
//  XT_CPENABLE
//    The contents of a non-running thread's CPENABLE register.
//    It represents the coprocessors owned (and whose state is still needed)
//    by the thread. When a thread is preempted, its CPENABLE is saved here.
//    When a thread solicits a context switch, its CPENABLE is cleared - the
//    compiler has saved the (caller-saved) coprocessor state if needed.
//    When a non-running thread loses ownership of a CP, its bit is cleared.
//    When a thread runs, it's XT_CPENABLE is loaded into the CPENABLE reg.
//    Avoids coprocessor exceptions when no change of ownership is needed.
//
//  XT_CPSTORED
//    A bitmask with the same layout as CPENABLE, a bit per coprocessor.
//    Indicates whether the state of each coprocessor is saved in the state 
//    save area. When a thread enters the kernel, only the state of coprocs
//    still enabled in CPENABLE is saved. When the coprocessor exception 
//    handler assigns ownership of a coprocessor to a thread, it restores 
//    the saved state only if this bit is set, and clears this bit.
//
//  XT_CP_CS_ST
//    A bitmask with the same layout as CPENABLE, a bit per co-processor.
//    Indicates whether callee-saved state is saved in the state save area.
//    Callee-saved state is saved by itself on a solicited context switch,
//    and restored when needed by the coprocessor exception handler.
//    Unsolicited switches will cause the entire coprocessor to be saved
//    when necessary.
//
//  XT_NCP_ASA
//    Pointer to aligned save area for non-CP state. This is always filled
//    in, even if there is no non-CP state to be saved. If there is no state
//    to be saved then no space is actually allocated and this pointer is
//    not used.
//
//  XT_CP_ASA
//    Pointer to aligned save area for coprocessor state. This is filled in
//    only if coprocessor state is to be saved for the thread. Allows it to be
//    aligned more than the overall save area (which might be stack-aligned
//    or TCB-aligned). Especially relevant for Xtensa cores configured with a
//    very large data path that requires alignment greater than 16 bytes (ABI
//    stack alignment).
//-----------------------------------------------------------------------------

#define ALIGNUP(n, val) (((val) + (n)-1) & -(n))

//  Offsets of each coprocessor save area within the 'aligned save area'.
//  The non-CP TIE state save area is at offset 0, so that it does not
//  move around if some or all coprocessors are not to be saved.

#define XT_NCP_SA       0
#define XT_CP0_SA       ALIGNUP(XCHAL_CP0_SA_ALIGN, XT_NCP_SA + XCHAL_NCP_SA_SIZE)
#define XT_CP1_SA       ALIGNUP(XCHAL_CP1_SA_ALIGN, XT_CP0_SA + XCHAL_CP0_SA_SIZE)
#define XT_CP2_SA       ALIGNUP(XCHAL_CP2_SA_ALIGN, XT_CP1_SA + XCHAL_CP1_SA_SIZE)
#define XT_CP3_SA       ALIGNUP(XCHAL_CP3_SA_ALIGN, XT_CP2_SA + XCHAL_CP2_SA_SIZE)
#define XT_CP4_SA       ALIGNUP(XCHAL_CP4_SA_ALIGN, XT_CP3_SA + XCHAL_CP3_SA_SIZE)
#define XT_CP5_SA       ALIGNUP(XCHAL_CP5_SA_ALIGN, XT_CP4_SA + XCHAL_CP4_SA_SIZE)
#define XT_CP6_SA       ALIGNUP(XCHAL_CP6_SA_ALIGN, XT_CP5_SA + XCHAL_CP5_SA_SIZE)
#define XT_CP7_SA       ALIGNUP(XCHAL_CP7_SA_ALIGN, XT_CP6_SA + XCHAL_CP6_SA_SIZE)

#define XT_TOT_SA_SIZE  ALIGNUP(16, XT_CP7_SA + XCHAL_CP7_SA_SIZE)
#define XT_NCP_SA_SIZE  XCHAL_NCP_SA_SIZE

//  Offsets within the overall save area

#define XT_CPENABLE     0   // (2 bytes) coprocessors active for this thread
#define XT_CPSTORED     2   // (2 bytes) coprocessors saved for this thread
#define XT_CP_CS_ST     4   // (2 bytes) coprocessor callee-saved regs for this thread
#define XT_NCP_ASA      8   // (4 bytes) ptr to aligned save area for nonCP state
#define XT_CP_ASA      12   // (4 bytes) ptr to aligned save area for CP state

//  Overall size allows for dynamic alignment, make sure multiple of 4 bytes.
//    XT_CP_SIZE  - total space needed for all coprocessors + nonCP state + hdr
//    XT_NCP_SIZE - total space needed for nonCP state + hdr

#define XT_CP_SIZE      ALIGNUP(4, (16 + XT_TOT_SA_SIZE + XCHAL_TOTAL_SA_ALIGN))
#define XT_NCP_SIZE     ALIGNUP(4, (16 + XT_NCP_SA_SIZE + XCHAL_TOTAL_SA_ALIGN))


//-----------------------------------------------------------------------------
//  Stack size computation.
//
//  XOS_STACK_MIN_SIZE
//    The minimum recommended stack size for any XOS thread. If you want to
//    use a stack size smaller than this, you will have to verify that the
//    smaller size will work under all operating conditions.
//
//  XOS_STACK_MIN_SIZE_NO_CP
//    The minimum recommended atack size for threads that will not use any
//    coprocessor resources. No coprocessor state will be saved/restored
//    for these threads. Non-CP TIE state will still be saved/restored.
//    These threads must be created with the flag XOS_THREAD_NO_CP.
//
//  XOS_STACK_EXTRA
//    The amount of stack space used by the system to:
//      - save coprocessor state
//      - save non-coprocessor TIE state
//      - allocate an interrupt/exception frame
//
//  XOS_STACK_EXTRA_NO_CP
//    The amount of stack space used by the system to:
//      - save non-coprocessor TIE state
//      - allocate an interrupt/exception frame
//-----------------------------------------------------------------------------

#define XOS_STACK_EXTRA             (XOS_FRAME_SIZE + XT_CP_SIZE)
#define XOS_STACK_EXTRA_NO_CP       (XOS_FRAME_SIZE + XT_NCP_SIZE)

#ifdef __XTENSA_CALL0_ABI__
#define XOS_STACK_MIN_SIZE          (XOS_STACK_EXTRA + 0x180)
#define XOS_STACK_MIN_SIZE_NO_CP    (XOS_STACK_EXTRA_NO_CP + 0x180)
#else
#define XOS_STACK_MIN_SIZE          (XOS_STACK_EXTRA + 0x200)
#define XOS_STACK_MIN_SIZE_NO_CP    (XOS_STACK_EXTRA_NO_CP + 0x200)
#endif


//-----------------------------------------------------------------------------
//  Items related to C library thread safety.
//-----------------------------------------------------------------------------
#if XOS_OPT_THREAD_SAFE_CLIB

#if XSHAL_CLIB == XTHAL_CLIB_XCLIB
  #if !defined(_ASMLANGUAGE) && !defined(__ASSEMBLER__)
  #include <sys/reent.h>
  #endif
  #define CLIB_THREAD_STRUCT      struct _reent xclib_reent
  #define GLOBAL_CLIB_PTR         _reent_ptr
#elif XSHAL_CLIB == XTHAL_CLIB_NEWLIB
  #if !defined(_ASMLANGUAGE) && !defined(__ASSEMBLER__)
  #include <sys/reent.h>
  #endif
  #define CLIB_THREAD_STRUCT      struct _reent newlib_reent
  #define GLOBAL_CLIB_PTR         _impure_ptr
#else
  #error The selected C runtime library is not thread safe.
#endif

#endif // XOS_OPT_THREAD_SAFE_CLIB


//-----------------------------------------------------------------------------
//  Check (MAX_OS_INTLEVEL,EXCM_LEVEL)
//-----------------------------------------------------------------------------
#if XOS_MAX_OS_INTLEVEL >= XCHAL_EXCM_LEVEL
# define XOS_MAX_OSEXCM_LEVEL   XOS_MAX_OS_INTLEVEL
#else
# warning "XOS_MAX_OS_INTLEVEL was set below XCHAL_EXCM_LEVEL: this was never tested"
# define XOS_MAX_OSEXCM_LEVEL   XCHAL_EXCM_LEVEL
#endif


//-----------------------------------------------------------------------------
//  Detect if in interrupt context.
//-----------------------------------------------------------------------------
#if XCHAL_HAVE_INTERRUPTS
#define INTERRUPT_CONTEXT       ((XT_RSR_PS() & PS_UM) == 0)
#else
#define INTERRUPT_CONTEXT       0
#endif


//-----------------------------------------------------------------------------
//  Xtensa tools version.
//-----------------------------------------------------------------------------
#if defined __XCC__
#define XTTOOLS_VERSION         (__XCC__ + __XCC_MINOR__)
#else
#define XTTOOLS_VERSION         (0)
#endif


//-----------------------------------------------------------------------------
//  Erratum workarounds.
//-----------------------------------------------------------------------------

// Erratum 487 fix is available in version RF.3 onwards and RG.2 onwards.
#if ((__XCC__ == 11000) && (__XCC_MINOR__ >= 3)) || (XTTOOLS_VERSION >= 12002)
#define HWERR_487_FIX           hw_erratum_487_fix
#else
#define HWERR_487_FIX
#endif


#ifdef __cplusplus
}
#endif

#endif  // __XOS_COMMON_H__

