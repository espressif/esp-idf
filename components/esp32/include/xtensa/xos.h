/** @file */

// xos.h - XOS API interface and data structures visible to user code.

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


#ifndef __XOS_H__
#define __XOS_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "xos_types.h"

#include <xtensa/config/core.h>
#if XCHAL_HAVE_INTERRUPTS
#include <xtensa/tie/xt_core.h>
#include <xtensa/tie/xt_interrupt.h>
#endif

#include "xos_common.h"
#include "xos_errors.h"
#include "xos_regaccess.h"


//-----------------------------------------------------------------------------
// Convert x into a literal string.
//-----------------------------------------------------------------------------
#define _XOS_STR(x)             __XOS_STR(x)
#define __XOS_STR(x)            #x


//-----------------------------------------------------------------------------
// XOS version.
//-----------------------------------------------------------------------------
#define XOS_VERSION_MAJOR       1
#define XOS_VERSION_MINOR       10
#define XOS_VERSION_STRING      "1.10"      ///< XOS version string.


//-----------------------------------------------------------------------------
// Runtime error handling.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
///
///  Reports a fatal error and halts XOS operation, i.e. halts the system. This
///  function will call a user-registered error handler (if one has been set) 
///  and then halt the system. The user handler may do system-specific things
///  such as record the error reason in nonvolatile memory etc.
///
///  \param     errcode         Error code. May be any user defined value < 0.
///                             Values >=0 are reserved for use by the system.
///
///  \param     errmsg          Optional text string describing the error.
///
///  \return    This function does not return.
///
//-----------------------------------------------------------------------------
void
xos_fatal_error(int32_t errcode, const char * errmsg);


#if XOS_DEBUG

// Do not call directly.
void
xos_assert(const char * file, int32_t line);

//-----------------------------------------------------------------------------
///
///  Check condition and fail if condition expression is false.
///  In debug builds, an assertion failure will cause a fatal error to be
///  reported. In non-debug builds, assertions are compiled out.
///
///  NOTE: Remember that any code in XOS_ASSERT() statements gets compiled out
///  for non-debug builds.
///
//-----------------------------------------------------------------------------

#define XOS_ASSERT(expr)        if ((expr) == 0) xos_assert(__FILE__, __LINE__)

#else

#define XOS_ASSERT(expr)

#endif


//-----------------------------------------------------------------------------
///
/// Interrupt handler function pointer type.
///
//-----------------------------------------------------------------------------
typedef void    (XosIntFunc)(void * arg);

//-----------------------------------------------------------------------------
///
/// Print handler function pointer type.
///
//-----------------------------------------------------------------------------
typedef int32_t (XosPrintFunc)(void * arg, const char * fmt, ...);

//-----------------------------------------------------------------------------
///
/// Fatal error handler function pointer type.
///
//-----------------------------------------------------------------------------
typedef void    (XosFatalErrFunc)(int32_t errcode, const char * errmsg);

//-----------------------------------------------------------------------------
///
/// Exception handler function pointer type.
///
//-----------------------------------------------------------------------------
typedef void    (XosExcHandlerFunc)(XosExcFrame * frame);


//-----------------------------------------------------------------------------
///
///  Install a user defined exception handler for the specified exception type.
///  This will override the default XOS exception handler. The handler is a C
///  function that is passed one parameter -- a pointer to the exception frame.
///  The exception frame is allocated on the stack of the thread that caused the
///  exception, and contains saved state and exception information. For details
///  of the exception frame see the structure XosExcFrame.
///
///  \param     exc             Exception type (number) to override. The exception
///                             numbers are enumerated in <xtensa/corebits.h>.
///
///  \param     handler         Pointer to handler function to be installed.
///                             To revert to the default handler, pass NULL.
///
///  \return    Returns a pointer to previous handler installed, if any.
///
//-----------------------------------------------------------------------------
XosExcHandlerFunc *
xos_register_exception_handler(int32_t exc, XosExcHandlerFunc * handler);


//-----------------------------------------------------------------------------
///
///  Install a user defined fatal error handler. This function will be called if
///  a fatal error is reported either by user code or by XOS itself. It will be
///  passed the same arguments that are passed to xos_fatal_error().
///
///  The handler need not return. It should make minimal assumptions about the
///  state of the system. In particular, it should not assume that further XOS
///  system calls will succeed.
///
///  \param     handler         Pointer to handler function to be installed.
///
///  \return    Returns a pointer to previous handler installed, if any.
///
//-----------------------------------------------------------------------------
XosFatalErrFunc *
xos_register_fatal_error_handler(XosFatalErrFunc * handler);


#ifdef _XOS_INCLUDE_INTERNAL_
# include "xos_internal.h"
#endif


#include "xos_thread.h"
#include "xos_timer.h"
#include "xos_cond.h"
#include "xos_event.h"
#include "xos_mutex.h"
#include "xos_msgq.h"
#include "xos_semaphore.h"
#include "xos_stopwatch.h"


//-----------------------------------------------------------------------------
///
///  Register a handler function to call when interrupt "num" occurs.
///
///  For level-triggered and timer interrupts, the handler function will have
///  to clear the source of the interrupt before returning, to avoid infinitely
///  retaking the interrupt. Edge-triggered and software interrupts are 
///  automatically cleared by the OS interrupt dispatcher (see xos_handlers.S).
///
///  \param     num             Xtensa internal interrupt number (0..31). To
///                             refer to a specific external interrupt number
///                             (BInterrupt pin), use HAL macro XCHAL_EXTINTx_NUM
///                             where 'x' is the external number.
///
///  \param     handler         Pointer to handler function.
///
///  \param     arg             Argument passed to handler.
///
///  \return    Returns XOS_OK if successful, else error code.
///
//-----------------------------------------------------------------------------
int32_t
xos_register_interrupt_handler(int32_t num, XosIntFunc * handler, void * arg);


//-----------------------------------------------------------------------------
///
///  Unregister a handler function for interrupt "num". If no handler was 
///  installed, this function will have no effect.
///
///  \param     num             Xtensa internal interrupt number (0..31). To
///                             refer to a specific external interrupt number
///                             (BInterrupt pin), use HAL macro XCHAL_EXTINTx_NUM
///                             where 'x' is the external number.
///
///  \return    Returns XOS_OK if successful, else error code.
///
//-----------------------------------------------------------------------------
int32_t
xos_unregister_interrupt_handler(int32_t num);


//-----------------------------------------------------------------------------
///
///  Register a high priority interrupt handler for interrupt level "level".
///
///  Unlike low and medium priority interrupt handlers, high priority handlers
///  are not installed for a specific interrupt number, but for an interrupt
///  level. The level must be above XCHAL_EXCM_LEVEL. The handler function must
///  be written in assembly since C handlers are not supported for levels above
///  XCHAL_EXCM_LEVEL. The handler function must preserve all registers except
///  a0, and must return to the dispatcher via a "ret" instruction, not "rfi".
///
///  NOTE: This method of dispatch takes a few cycles of overhead. If you wish
///  to save even these cycles, then you can define your own dispatch function
///  to override the built-in dispatcher. See xos_handlers.S for more details.
///
///  \param     level           The interrupt level to be handled.
///
///  \param     handler         Pointer to handler function.
///
///  \return    Returns XOS_OK if successful, else error code.
///
//-----------------------------------------------------------------------------
int32_t
xos_register_hp_interrupt_handler(int32_t level, void * handler);


//-----------------------------------------------------------------------------
///
///  Enable a specific interrupt, by interrupt number.
///  The state (enabled vs. disabled) of individual interrupts is global, i.e.
///  not associated with any specific thread. Depending on system options and
///  implementation, this state may be stored in one of two ways:
///  - directly in the INTENABLE register, or
///  - in a global variable (this is generally the case when INTENABLE is used
///    not just to control what interrupts are enabled globally, but also for
///    software interrupt prioritization within an interrupt level, effectively
///    providing finer grained levels; in this case XOS takes care to update
///    INTENABLE whenever either the global enabled-state variable or the 
///    per-thread fine-grained-level variable change).
///  Thus it is best to never access the INTENABLE register directly.
///
///  To modify thread-specific interrupt priority level, use one of:
///    - xos_set_int_pri_level()
///    - xos_restore_int_pri_level()
///    - xos_disable_interrupts()
///    - xos_restore_interrupts()
///
///  NOTE: To refer to a specific external interrupt number (BInterrupt pin),
///  use HAL macro XCHAL_EXTINTx_NUM where 'x' is the external interrupt
///  number.  For example, to enable external interrupt 3 (BInterrupt[3]),
///  you can use:
///
///      xos_interrupt_enable( XCHAL_EXTINT3_NUM );
///
///  \param     intnum          Interrupt number to enable. Must range between 0-31.
///
///  \return    Returns nothing.
///
//-----------------------------------------------------------------------------
void
xos_interrupt_enable(uint32_t intnum);


//-----------------------------------------------------------------------------
///
///  Disable a specific individual interrupt, by interrupt number.
///
///  This is the counterpart to xos_interrupt_enable(). See the description
///  of xos_interrupt_enable() for further comments and notes.
///
///  \param     intnum          Interrupt number to disable. Must range between 0-31.
///
///  \return    Returns nothing.
///
//-----------------------------------------------------------------------------
void
xos_interrupt_disable(uint32_t intnum);


//-----------------------------------------------------------------------------
///
///  Get the CPU's current interrupt priority level. Interrupts at or below this
///  priority level are blocked.
///
///  \return    Returns the current IPL, ranging from 0 to XCHAL_NUM_INTLEVELS.
///
//-----------------------------------------------------------------------------
static inline uint32_t
xos_get_int_pri_level(void)
{
#if XCHAL_HAVE_INTERRUPTS
    return XT_RSR_PS() & 0xF;
#else
    return 0;
#endif
}


//-----------------------------------------------------------------------------
///
///  Set the CPU's interrupt priority level to the specified level, but only if
///  the current IPL is below the one requested. This function will never cause
///  the interrupt priority level to be lowered from the current level.
///  Call this function to block interrupts at or below the specified priority
///  level.
///
///  When setting the IPL temporarily (such as in a critical section), call
///  xos_set_int_pri_level(), execute the critical code section, and then call
///  xos_restore_int_pri_level().
///
///  The interrupt priority level is part of the thread context, so it is saved
///  and restored across context switches. To enable and disable individual
///  interrupts globally, use the functions xos_interrupt_enable() and
///  xos_interrupt_disable() instead.
///
///  NOTE: It is usually not required to disable interrupts at a level higher
///  than that of the highest priority interrupt that interacts with the OS
///  (i.e. calls into XOS such that threads may be woken / blocked / 
///  reprioritized / switched, or otherwise access XOS data structures).
///  In XOS, that maximum level is XOS_MAX_OS_INTLEVEL, which defaults to
///  XCHAL_EXCM_LEVEL. This may be modified by editing xos_params.h and
///  rebuilding XOS.
///
///  \param     level           The new interrupt priority level (IPL).
///
///  \return    Returns a value that can be used to restore the previous
///             priority level by calling xos_restore_int_pri_level(). This
///             value should be treated as opaque by application code, and
///             should be passed unchanged to the restore function.
///
//-----------------------------------------------------------------------------
__attribute__((always_inline))
static inline uint32_t
xos_set_int_pri_level(uint32_t level)
{
#if XCHAL_HAVE_INTERRUPTS
#pragma no_reorder
    uint32_t ps = XT_RSR_PS();

    if (level > (ps & 0xF)) {
        level = (ps & ~0xF) | level;
        XT_WSR_PS(level);
        XT_RSYNC();
    }

    return ps;
#else
    return 0;
#endif
}


//-----------------------------------------------------------------------------
///
///  Restores the CPU to a previously saved interrupt priority level. This level
///  must have been obtained by calling xos_set_int_pri_level().
///
///  \param     oldval          Return value from xos_set_int_pri_level().
///
///  \return    Returns nothing.
///
//-----------------------------------------------------------------------------
__attribute__((always_inline))
static inline void
xos_restore_int_pri_level(const uint32_t oldval)
{
#if XCHAL_HAVE_INTERRUPTS
#pragma no_reorder
    XT_WSR_PS(oldval);
    XT_RSYNC();
#else
    // Nothing
#endif
}


//-----------------------------------------------------------------------------
///
///  Disable all interrupts that can interact directly with the OS. This is a
///  convenience function, shorthand for setting the IPL to XOS_MAX_OS_INTLEVEL.
///
///  Returns: A value that can be used to restore the previous priority level
///  by calling xos_restore_interrupts(). This value should be treated as
///  opaque by application code, and should be passed unchanged to the restore
///  function.
///
//-----------------------------------------------------------------------------
static inline uint32_t
xos_disable_interrupts(void)
{
    return xos_set_int_pri_level(XOS_MAX_OS_INTLEVEL);
}


//-----------------------------------------------------------------------------
///
///  Restore the CPU's previously saved interrupt status. This is a convenience
///  function, the counterpart to xos_disable_interrupts().
///
///  \return    rval            Return value from xos_disable_interrupts().
///
///  \return    Returns nothing.
///
//-----------------------------------------------------------------------------
static inline void
xos_restore_interrupts(uint32_t rval)
{
    xos_restore_int_pri_level(rval);
}


#ifdef _XOS_INCLUDE_INTERNAL_

//-----------------------------------------------------------------------------
//  Enter an OS critical section, i.e. get exclusive access to OS critical 
//  state and data structures. Code that manipulates the state of OS objects
//  or modifies internal OS state must call this function first, to ensure
//  that it has exclusive access. On a single-core system, this is equivalent
//  to blocking all interrupts that can interact directly with the OS, i.e.
//  all interrupts at or below XOS_MAX_OS_INTLEVEL. In a multi-core system
//  this is likely to be implemented differently to achieve the same effect.
//
//  Returns: A value that is to be used to restore the state of the CPU when
//  exiting the critical section. This must be treated as opaque and passed
//  unmodified to xos_critical_exit().
//
//  NOTE: This function is meant for use in OS code, not in applications.
//-----------------------------------------------------------------------------
__attribute__((always_inline))
static inline uint32_t
xos_critical_enter(void)
{
#if XCHAL_HAVE_INTERRUPTS
    // This function cannot be called by high-level interrupt handlers,
    // i.e. it can never be called with intlevel > XOS_MAX_OS_INTLEVEL.
    // So, we do not need to check current intlevel because we will not
    // ever be lowering it by setting it to XOS_MAX_OS_INTLEVEL.
    // NOTE: sync after RSIL not needed.
    return XT_RSIL(XOS_MAX_OS_INTLEVEL);
#else
    return 0;
#endif
}


//-----------------------------------------------------------------------------
//  Exit an OS critical section and restore CPU state. See the documentation
//  for xos_critical_enter().
//
//  cflags              Return value from xos_critical_enter().
//                      Must be treated as an opaque value.
//
//  Returns: Nothing.
//
//  NOTE: This function is meant for use in OS code, not in applications.
//-----------------------------------------------------------------------------
__attribute__((always_inline))
static inline void
xos_critical_exit(uint32_t cflags)
{
    xos_restore_int_pri_level(cflags);
}

#endif // _XOS_INCLUDE_INTERNAL_


// This file uses things defined above
#include "xos_syslog.h"


// Misc

//-----------------------------------------------------------------------------
//  Helper function to list all threads in system. Useful for debug.
//-----------------------------------------------------------------------------
void
xos_display_threads(void * arg, XosPrintFunc * print_fn);


#ifdef __cplusplus
}
#endif

#endif  // __XOS_H__

