/** @file */

// xos_params.h - user-settable compile time parameters for XOS.

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


#ifndef __XOS_PARAMS_H__
#define __XOS_PARAMS_H__

#include <xtensa/config/core.h>

#ifdef __cplusplus
extern "C" {
#endif


//-----------------------------------------------------------------------------
///
/// Number of thread priority levels. At this time XOS supports a maximum of
/// 32 priority levels (0 - 31).
///
//-----------------------------------------------------------------------------
#ifndef XOS_NUM_PRIORITY
#define XOS_NUM_PRIORITY                16       // Default is 16
#endif


//-----------------------------------------------------------------------------
///
/// Debug flags - Set to 1 to enable debug mode (and more verbose operation).
/// Can be set individually, or define XOS_DEBUG_ALL to enable all of them.
///
/// - XOS_DEBUG           -- Generic OS debug
/// - XOS_COND_DEBUG      -- Condition objects debug
/// - XOS_EVENT_DEBUG     -- Event objects debug
/// - XOS_MSGQ_DEBUG      -- Message queue debug
/// - XOS_MUTEX_DEBUG     -- Mutex objects debug
/// - XOS_SEM_DEBUG       -- Semaphore objects debug
/// - XOS_THREAD_DEBUG    -- Thread module debug
/// - XOS_TIMER_DEBUG     -- Timer module debug
///
/// WARNING: Enabling one or more of these flags will affect system performance
/// and timing.
///
/// NOTE: Not all of these have been fully implemented.
///
//-----------------------------------------------------------------------------
#if defined XOS_DEBUG_ALL

#define XOS_DEBUG                       1
#define XOS_THREAD_DEBUG                1
#define XOS_TIMER_DEBUG                 1
#define XOS_COND_DEBUG                  1
#define XOS_MUTEX_DEBUG                 1
#define XOS_SEM_DEBUG                   1
#define XOS_EVENT_DEBUG                 1
#define XOS_MSGQ_DEBUG                  1

#else

#ifndef XOS_DEBUG
#define XOS_DEBUG                       0
#endif
#ifndef XOS_THREAD_DEBUG
#define XOS_THREAD_DEBUG                0
#endif
#ifndef XOS_TIMER_DEBUG
#define XOS_TIMER_DEBUG                 0
#endif
#ifndef XOS_COND_DEBUG
#define XOS_COND_DEBUG                  0
#endif
#ifndef XOS_MUTEX_DEBUG
#define XOS_MUTEX_DEBUG                 0
#endif
#ifndef XOS_SEM_DEBUG
#define XOS_SEM_DEBUG                   0
#endif
#ifndef XOS_EVENT_DEBUG
#define XOS_EVENT_DEBUG                 0
#endif
#ifndef XOS_MSGQ_DEBUG
#define XOS_MSGQ_DEBUG                  0
#endif

#endif


//-----------------------------------------------------------------------------
///
/// Set this option to 1 to enable runtime statistics collection for XOS.
/// NOTE: Enabling this option does have some impact on runtime performance
/// and OS footprint.
///
//-----------------------------------------------------------------------------
#ifndef XOS_OPT_STATS
#define XOS_OPT_STATS                   1
#endif


//-----------------------------------------------------------------------------
///
/// Set this option to 1 to enable statistics tracking for message queues.
/// enabling this will cause message queue objects to increase in size, and add
/// some overhead to message queue processing.
///
//-----------------------------------------------------------------------------
#ifndef XOS_OPT_MSGQ_STATS
#define XOS_OPT_MSGQ_STATS              0
#endif


//-----------------------------------------------------------------------------
///
/// Size of interrupt stack in bytes. Shared by all interrupt handlers. Must be
/// sized to handle worst case nested interrupts. This is also used by the idle
/// thread so must exist even if interrupts are not configured.
///
//-----------------------------------------------------------------------------
#ifndef XOS_INT_STACK_SIZE
#if XCHAL_HAVE_INTERRUPTS
#define XOS_INT_STACK_SIZE              8192
#else
#define XOS_INT_STACK_SIZE              32
#endif
#endif


//-----------------------------------------------------------------------------
///
/// Default maximum interrupt level at which XOS primitives may be called.
/// It is the level at which interrupts are disabled by default.
/// See also description of xos_set_int_pri_level().
///
//-----------------------------------------------------------------------------
#ifndef XOS_MAX_OS_INTLEVEL
#define XOS_MAX_OS_INTLEVEL             XCHAL_EXCM_LEVEL
#endif


//-----------------------------------------------------------------------------
///
/// Set this to 1 to enable stack checking. The stack is filled with a pattern
/// on thread creation, and the stack is checked at certain times during system
/// operation.
/// WARNING: Enabling this option can have some impact on runtime performance.
///
//-----------------------------------------------------------------------------
#ifndef XOS_OPT_STACK_CHECK
#if XOS_DEBUG
#define XOS_OPT_STACK_CHECK             1
#else
#define XOS_OPT_STACK_CHECK             0
#endif
#endif


//-----------------------------------------------------------------------------
///
/// Set XOS_CLOCK_FREQ to the system clock frequency if this is known ahead of
/// time. Otherwise, call xos_set_clock_freq() to set it at run time.
///
//-----------------------------------------------------------------------------
#ifndef XOS_CLOCK_FREQ
#define XOS_CLOCK_FREQ                  1000000
#endif
#define XOS_DEFAULT_CLOCK_FREQ          XOS_CLOCK_FREQ


//-----------------------------------------------------------------------------
///
/// Set this option to 1 to enable software prioritization of interrupts. The
/// priority scheme applied is that a higher interrupt number at the same level
/// will have higher priority.
///
//-----------------------------------------------------------------------------
#ifndef XOS_OPT_INTERRUPT_SWPRI
#define XOS_OPT_INTERRUPT_SWPRI         1
#endif


//-----------------------------------------------------------------------------
///
/// Set this option to 1 to use the thread-safe version of the C runtime library.
/// You may need to enable this if you call C library functions from multiple
/// threads -- see the documentation for the relevant C library to determine if
/// this is necessary. This option increases the size of the TCB.
/// NOTE: At this time only the newlib and xclib libraries are supported for
/// thread safety.
///
//-----------------------------------------------------------------------------
#include <xtensa/config/system.h>

#ifndef XOS_OPT_THREAD_SAFE_CLIB

#if XSHAL_CLIB == XTHAL_CLIB_XCLIB
#define XOS_OPT_THREAD_SAFE_CLIB        1
#elif XSHAL_CLIB == XTHAL_CLIB_NEWLIB
#define XOS_OPT_THREAD_SAFE_CLIB        1
#else
#define XOS_OPT_THREAD_SAFE_CLIB        0
#endif

#endif


//-----------------------------------------------------------------------------
///
/// Set this option to 1 to enable the wait timeout feature. This allows waits
/// on waitable objects to expire after a specified timeout.
///
//-----------------------------------------------------------------------------
#ifndef XOS_OPT_WAIT_TIMEOUT
#define XOS_OPT_WAIT_TIMEOUT            1
#endif


//-----------------------------------------------------------------------------
///
/// Set this option to 1 to enable threads waiting on timer objects. If this
/// feature is not used, turning it off will make timer objects smaller, and
/// reduce the time taken by timer expiry processing (by a small amount).
///
//-----------------------------------------------------------------------------
#ifndef XOS_OPT_TIMER_WAIT
#define XOS_OPT_TIMER_WAIT              1
#endif


//-----------------------------------------------------------------------------
///
/// Set this option to 1 to enable time-slicing between multiple threads at the
/// same priority. If this option is enabled then on every timer tick the timer
/// handler will switch out the current thread if there is another ready thread
/// at the same priority, and allow the latter thread to run. Execution will be
/// round robin switched among all the threads at the same priority.
///
/// Currently the time slice interval is fixed to be one timer tick.
///
/// This feature is most useful if fixed duration timer ticks are used.
/// If dynamic ticking is enabled, then time slicing will work unpredictably
/// because the interval between ticks will vary. In some cases it may be
/// better to turn time slicing off.
///
//-----------------------------------------------------------------------------
#ifndef XOS_OPT_TIME_SLICE
#define XOS_OPT_TIME_SLICE              1
#endif


#ifdef __cplusplus
}
#endif

#endif  // __XOS_PARAMS_H__

