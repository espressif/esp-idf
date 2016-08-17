/** @file */

// xos_thread.h - XOS Thread API interface and data structures.

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

// NOTE: Do not include this file directly in your application. Including
// xos.h will automatically include this file.


#ifndef __XOS_THREAD_H__
#define __XOS_THREAD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "xos_types.h"
#include "xos_params.h"


//-----------------------------------------------------------------------------
//  Number of thread priority levels.
//-----------------------------------------------------------------------------
#ifndef XOS_NUM_PRIORITY
#error "XOS_NUM_PRIORITY must be defined (in xos_params.h)."
#endif
#if XOS_NUM_PRIORITY > 32
#error "The number of thread priority levels (XOS_NUM_PRIORITY) must be <= 32."
#endif
#define XOS_MAX_PRIORITY        (XOS_NUM_PRIORITY)


//-----------------------------------------------------------------------------
//  Macro for thread self pointer.
//-----------------------------------------------------------------------------
#define XOS_THREAD_SELF         (xos_thread_id())


//-----------------------------------------------------------------------------
///
///  Thread entry function pointer type.
///
//-----------------------------------------------------------------------------
typedef int32_t (XosThreadFunc)(void * arg, int32_t wake_value);


//-----------------------------------------------------------------------------
//  Thread switcher function signature.
//-----------------------------------------------------------------------------
typedef struct XosThread XosThread;
typedef int32_t (XosSwitchFunc)(XosThread *);


//-----------------------------------------------------------------------------
///
///  Condition evaluation callback function pointer type.
///
//-----------------------------------------------------------------------------
typedef int32_t (XosCondFunc)(void * arg, int32_t sig_value, XosThread * thread);


//-----------------------------------------------------------------------------
///
///  Thread exit handler function pointer type.
///
//-----------------------------------------------------------------------------
typedef int32_t (XosThdExitFunc)(int32_t exitcode);


//-----------------------------------------------------------------------------
//  Thread queue structure. Used to implement the ready queues as well
//  as the wait queues.
//-----------------------------------------------------------------------------
typedef struct XosThreadQueue {
  XosThread *  head;    // Pointer to first thread in queue, or 0 if none.
  XosThread ** tail;    // Pointer to last thread's r_next pointer, or
                        // to "head" if none.
} XosThreadQueue;


//-----------------------------------------------------------------------------
//  Stack frame for a thread that is not running. That is, it has either
//  been preempted or has yielded.
//-----------------------------------------------------------------------------
typedef union XosFrame {
  XosExcFrame   e;      // resume_fn == &xos_resume_preempted_thread
  XosCoopFrame  c;      // resume_fn == &xos_resume_cooperative_thread
  // nothing for resume_fn == &xos_resume_idle_thread
  // nothing for resume_fn == &xos_resume_by_restart
} XosFrame;


//-----------------------------------------------------------------------------
//  Thread Control Block. Tracks the state and control information associated
//  with a thread.
//
//  IMPORTANT:  keep this in sync with TCB_*** offsets in xos_common.h .
//-----------------------------------------------------------------------------
struct XosThread {
  XosThread *   r_next;         // 00 Next thread in queue (eg. ready queue of
                                // its priority, or some queue of blocked threads)
                                // Should be NULL if not in any queue.

  XosThread **  r_pprev;        // 04 Points to previous queue entry's r_next
                                // pointer (i.e. to itself), or to queue head
                                // if first in queue. NULL if not in any queue.

  XosThread *   all_next;       // 08 Next in list of all threads.
  
  void *        resume_fn;      // 12 Pointer to the routine to be called to
                                // resume this thread. On entry to such code:
                                //   a2 == xos_curr_threadptr (thread being resumed)
                                //   a3 == &xos_globals

  XosFrame *    esf;            // 16 Pointer to saved exception stack frame,
                                // just below thread's current stack pointer.
                                // For RTC threads, this is valid only while the
                                // thread is preempted, not when it is blocked.

  void *        tie_save;       // 20 TIE state save area. May be NULL if there
                                // is not TIE state saved for this thread.
  
  int32_t       wake_value;     // 24 Value returned from block call (by wake call)
                                // (for RTC: pass this to start function??)
  
  XosSwitchFunc * switch_fn;    // 28 Pointer to a function that
                                // can be called from within this thread, to save
                                // this thread's state and switch to a specified
                                // other thread. Returns wake value.

  void *        stack_base;     // 32 Base of stack as specified by thread creator.
  
  void *        stack_end;      // 36 End of stack (adjusted for TIE state save area
                                // if any).

  XosThreadFunc * entry;        // 40 Pointer to thread entry function. Used for
                                // RTC thread restart.
  
  void *        arg;            // 44 Argument value passed to entry function.

  bool          ready;          // 48 Set when thread is ready to run, and is in
                                // its priority queue (i.e. r_pprev is set when
                                // this flag is set).

  bool          in_exit;        // Exit flag, nonzero when in exit processing.

  int8_t        priority;       // Thread priority, 0 .. (XOS_MAX_PRI - 1). Higher
                                // numbers have higher priority. This must only be
                                // changed when thread is not ready, or by calling
                                // xos_thread_set_priority().

  int8_t        preempt_pri;    // This thread's preemption blocking priority.
                                // (preempt_pri >= priority). A thread's priority
                                // must be higher than another's preempt_pri to be
                                // able to preempt it. Note that preempt_pri can
                                // change during runtime e.g. due to priority
                                // inheritance.

  uint32_t      flags;          // 52 Thread creation flags.

  const char *  name;           // 56 Thread name (mainly for debug).

  const char *  block_cause;    // 60 Reason for blocking. Valid only when thread
                                // not ready (r_pprev == 0).

  XosThread *   container;      // 64 Thread whose stack will be used to run
                                // this thread. Valid for RTC threads only, else NULL.

  XosThdExitFunc * exit_func;   // 68 Thread exit handler function pointer.

  XosThreadQueue  exit_waiters; // 72 Queue of threads waiting for this one to exit.

  XosThreadQueue * wq_ptr;      // 80 If this thread is in a wait queue, this
                                // points to the queue. Must be NULL when
                                // thread not in a queue.

  XosCondFunc * cond_fn;        // 84 Condition function. Valid only while thread
                                // is blocked on condition.

  void *        cond_arg;       // 88 Argument to be passed to condition function.

  uint16_t      cp_mask;        // 92 Mask of coprocessors used.
  uint16_t      cp_saved;       // 94 Mask of coprocessors saved.

  uint32_t      event_bits;     // 96 event bits
  uint32_t      event_mask;     // 100 event bit mask
  uint32_t      event_flags;    // 104 event flags

  void *        clib_ptr;       // 108 Pointer to C lib context struct.

  uint32_t      sig;            // 112 Signature of valid TCB

  uint32_t      resume_ccount;  // 116 cycle count at resume
  uint64_t      cycle_count;    // 120 number of cycles consumed (approx).
                                // NOTE: must be 8-byte aligned
  uint32_t      normal_resumes; // 128 Number of non-preemptive resumptions.
  uint32_t      preempt_resumes;// 132 Number of preemptive resumptions.

#if XOS_OPT_THREAD_SAFE_CLIB
  CLIB_THREAD_STRUCT;           // C library context area.
#endif
};


//-----------------------------------------------------------------------------
//  User-visible flags for xos_thread_create().
//-----------------------------------------------------------------------------
#define XOS_THREAD_SUSPEND      0x0001  ///< Create suspended instead of ready
#define XOS_THREAD_RTC          0x0002  ///< Run-to-completion thread
#define XOS_THREAD_NO_CP        0x0004  ///< Thread does not use coprocessors


//-----------------------------------------------------------------------------
//  Flags used by thread creation extra parameters.
//-----------------------------------------------------------------------------
#define XOS_TP_COPROC_MASK      0x0001
#define XOS_TP_PREEMPT_PRI      0x0002
#define XOS_TP_EXIT_HANDLER     0x0004


//-----------------------------------------------------------------------------
//  Thread creation extra parameters.
//-----------------------------------------------------------------------------
typedef struct XosThreadParm {
  uint32_t      parms_mask;     // Combination of XOS_TP_xxx flags indicating
                                // which parameters are valid.

  uint16_t      cp_mask;        // Mask of coprocessors the thread can access.

  uint32_t      preempt_pri;    // Initial preemption blocking priority. Can be
                                // changed later via xos_thread_set_priority().

  XosThdExitFunc * handler;     // Exit handler function.

} XosThreadParm;


//-----------------------------------------------------------------------------
//  Wrapper struct for RTC (run to completion) thread.
//-----------------------------------------------------------------------------
typedef struct XosRtcThread {
  struct XosThread thread;
} XosRtcThread;


//-----------------------------------------------------------------------------
//  External variables.
//-----------------------------------------------------------------------------
extern XosThread *      xos_curr_threadptr;     // Current active thread
extern XosThread *      xos_next_threadptr;     // Next ready thread
extern XosThread *      xos_all_threads;        // List of all threads


//-----------------------------------------------------------------------------
///
///  Set thread creation parameter: the group of coprocessors that this thread
///  will use. This must be set during thread creation, and cannot be changed
///  after the thread has been created. Defining this allows reduction of
///  memory usage (for CP state saving) in some circumstances, and can also
///  speed up the context switch time.
///
///  NOTE: Support for this is not currently implemented. If a thread uses
///  any coprocessor, space for all coprocessors must be reserved.
///
///  \param     parms           Thread creation parameter structure. Must be
///                             allocated by the caller.
///
///  \param     cp_mask         Bitmask of coprocessors thread is allowed to
///                             use. Bit 0 for coprocessor 0, etc.
///
///  \return    Returns nothing.
///
//-----------------------------------------------------------------------------
static inline void
xos_threadp_set_cp_mask(XosThreadParm * parms, uint16_t cp_mask)
{   
    if (parms != XOS_NULL) {
        parms->parms_mask |= XOS_TP_COPROC_MASK;
        parms->cp_mask = cp_mask;
    }
}


//-----------------------------------------------------------------------------
///
///  Set thread creation parameter: thread pre-emption priority.
///
///  \param     parms           Thread creation parameter structure. Must be
///                             allocated by caller.
///
///  \param     preempt_pri     Thread pre-emption blocking priority.
///                             From 0 .. XOS_NUM_PRIORITY - 1.
///                             Must be greater or equal to the thread priority
///                             (if not, is automatically set to thread priority).
///
///  \return    Returns nothing.
///
//-----------------------------------------------------------------------------
static inline void
xos_threadp_set_preemption_priority(XosThreadParm * parms, int8_t preempt_pri)
{
    if (parms != XOS_NULL) {
        parms->parms_mask |= XOS_TP_PREEMPT_PRI;
        parms->preempt_pri = preempt_pri;
    }
}


//-----------------------------------------------------------------------------
///
///  Set thread creation parameter: thread exit handler.
///
///  \param     parms           Thread creation parameter structure. Must be
///                             allocated by caller.
///
///  \param     handler         Exit handler function.
///
///  \return    Returns nothing.
//-----------------------------------------------------------------------------
static inline void
xos_threadp_set_exit_handler(XosThreadParm * parms, XosThdExitFunc * handler)
{
    if (parms != XOS_NULL) {
        parms->parms_mask |= XOS_TP_EXIT_HANDLER;
        parms->handler     = handler;
    }
}


//-----------------------------------------------------------------------------
///
///  Create a new thread. If the thread is not created suspended, then it will
///  be made ready as soon as it is created, and will immediately run if it is
///  the highest priority non-blocked thread in the system.
///
///  \param     thread          Pointer to the thread descriptor (an otherwise
///                             unused XosThread structure, usually allocated
///                             by the caller for the lifetime of the thread,
///                             for example as a global variable).
///
///  \param     container       Pointer to separate thread acting as "container"
///                             for this one. At the moment, this is only meaningful
///                             for run-to-completion (RTC) threads (identified with
///                             the XOS_THREAD_RTC flag), in which case the container
///                             must have the same priority and also be an RTC thread.
///                             (The priority restriction may be lifted in a future
///                             implementation, with appropriate constraints on dynamic
///                             reprioritization of the created thread).
///
///  \param     entry           Thread entry function, takes one argument.
///
///  \param     arg             Argument "void*" that is passed to the thread function.
///
///  \param     name            Unique name of the thread, for debug/display purposes.
///                             This string must be valid for the lifetime of the thread
///                             (only a pointer to it is stored in the thread control block).
///                             Typically consists of identifier chars with no spaces.
///
///  \param     stack           Base of initial stack for the thread, allocated by the 
///                             caller. Need not be aligned (initial stack pointer will be
///                             computed and aligned from given stack base and size).
///                             Required argument, except for run-to-completion threads 
///                             when container is non-NULL, in which case the container's
///                             stack is used and this argument must be NULL.
///
///  \param     stack_size      Size of the stack, in bytes.
///                             NOTE: stack should be at least XOS_STACK_EXTRA bytes plus
///                             whatever the thread actually needs if the thread will use
///                             coprocessors/TIE state. If the thread will not touch the
///                             coprocessors, then it should be XOS_STACK_EXTRA_NO_CP
///                             plus whatever the thread actually needs.
///                             Recommended minimum stack sizes are defined by the constants
///                             XOS_STACK_MIN_SIZE and XOS_STACK_MIN_SIZE_NO_CP.
///
///                             For run-to-completion threads where container is non-NULL, 
///                             stack_size specifies the minimum stack size required for
///                             the thread; it should be smaller or equal to the container's
///                             stack.
///
///  \param     priority        Initial thread priority. From 0 .. XOS_MAX_PRI - 1.
///                             Higher numbers are higher priority.
///
///  \param     parms           Pointer to extra parameters structure, or 0 if none given.
///                             Use xos_thread_p_***() functions to set parameters in the
///                             structure.
///
///  \param     flags           Option flags:
///                             - XOS_THREAD_SUSPEND -- Leave thread suspended instead of
///                               making it ready. The thread can be made ready to run later
///                               by calling xos_thread_resume().
///                             - XOS_THREAD_RTC -- Run-to-completion thread.
///                             - XOS_THREAD_NO_CP -- Thread does not use coprocessors.
///                               No coprocessor state will be saved for this thread.
///                               Threads that have this flag set will not allocate any
///                               storage for saving coprocessor state and so can have
///                               smaller stacks.
/// 
///  NOTE: xos_start_main() calls xos_thread_create() to convert main() into the 'main'
///  thread.
///
///  \return    Returns XOS_OK if successful, error code otherwise.
///
//-----------------------------------------------------------------------------
int32_t
xos_thread_create(XosThread *     thread,
                  XosThread *     container,
                  XosThreadFunc * entry,
                  void *          arg,
                  const char *    name, 
                  void *          stack,
                  uint32_t        stack_size,
                  int32_t         priority,
                  XosThreadParm * parms,
                  uint32_t        flags );


//-----------------------------------------------------------------------------
///
///  Remove thread and free up all resources. Thread must have exited already.
///  After this call returns, all resources allocated to the thread (e.g. TCB,
///  stack space, etc.) can be reused.
///
///  \param     thread          Handle of thread to be deleted.
///
///  \return    Returns XOS_OK on success, else error code.
///
///  NOTE: A thread cannot call this on itself.
///
//-----------------------------------------------------------------------------
int32_t     
xos_thread_delete(XosThread * thread);


//-----------------------------------------------------------------------------
///
///  Force the thread to terminate. The thread execution is aborted, but exit
///  processing will still happen, i.e. the exit handler (if any) will be run.
///  After termination, any other threads waiting on this thread are notified.
///  This function cannot be called on the current thread.
///
///  \param     thread          Handle of thread to be aborted.
///
///  \param     exitcode        Exit code returned to any waiting threads.
///
///  \return    Returns XOS_OK on success, else error code.
///
///  NOTE: If the thread is blocked waiting for something, the wait is aborted
///  and the thread is made ready.
///  NOTE: The thread is not guaranteed to have exited when this call returns.
///  It will be made ready and set up for exit processing, but when the exit
///  processing will actually happen depends on the state of the system and
///  the priority of the thread being aborted.
///
//-----------------------------------------------------------------------------
int32_t
xos_thread_abort(XosThread * thread, int32_t exitcode);


//-----------------------------------------------------------------------------
///
///  Exit the current thread. The exit handler (if any) will be run before the
///  thread terminates.
///
///  \param     exitcode                Exit code to be returned to any waiting threads.
///
///  \return    This function does not return.
///
///  NOTE: This is automatically called if the thread returns from its entry
///  function. The entry function's return value will be passed as the exit
///  code.
///
//-----------------------------------------------------------------------------
void
xos_thread_exit(int32_t exitcode);


//-----------------------------------------------------------------------------
///
///  Wait until the specified thread exits and get its exit code. If the thread
///  has exited already, an error will be returned.
///
///  \param     thread          The thread to wait for. Cannot be "self", i.e.
///                             one cannot wait on one's own exit.
///
///  \param     p_exitcode      If not null, the exit code will be returned here.
///
///  \return    Returns XOS_OK on sucess, else error code.
///
//-----------------------------------------------------------------------------
int32_t
xos_thread_join(XosThread * thread, int32_t * p_exitcode);


//-----------------------------------------------------------------------------
///
///  Yield the CPU to the next thread in line. The calling thread remains ready
///  and is placed at the tail of the ready queue at its current priority level.
///  If there are no threads at the same priority level that are ready to run,
///  then this call will return immediately.
///
///  \return    Returns nothing.
///
//-----------------------------------------------------------------------------
void
xos_thread_yield();


//-----------------------------------------------------------------------------
///
///  Suspend the specified thread. The thread will remain suspended until 
///  xos_thread_resume() has been called on it. If the thread is already blocked
///  on some other condition, then this function will return an error.
///
///  \param     thread          Handle of thread being suspended. A thread can
///                             use the special handle XOS_THREAD_SELF to suspend
///                             itself.
///
///  \return    Returns XOS_OK on success, else error code.
///
//-----------------------------------------------------------------------------
int32_t
xos_thread_suspend(XosThread * thread);


//-----------------------------------------------------------------------------
///
///  Resume a suspended thread. If the thread is not suspended or is blocked on
///  some other condition then this function will do nothing. Otherwise, it will
///  be made ready, and this can cause an immediate context switch if the thread
///  is at a higher priority than the calling thread.
///
///  \param     thread          Handle of thread being resumed.
///
///  \return    Returns XOS_OK on success, else error code.
///
//-----------------------------------------------------------------------------
int32_t
xos_thread_resume(XosThread * thread);


//-----------------------------------------------------------------------------
///
///  Get the priority of the specified thread. This returns the priority of the
///  queried thread at this instant, however this can change at any time due to
///  other activity in the system.
///
///  \param     thread          Handle of thread being queried. A thread can use
///                             the special handle XOS_THREAD_SELF to query itself.
///
///  \return    Returns the thread's current priority, or -1 if the thread handle
///             is not valid.
///
//-----------------------------------------------------------------------------
static inline int32_t
xos_thread_get_priority(XosThread * thread)
{
    XOS_ASSERT(thread);
    return thread ? thread->priority : -1;
}


//-----------------------------------------------------------------------------
///
///  Set the priority of the specified thread. The thread must exist. 
///
///  \param     thread          Handle of thread being affected. A thread can
///                             use the special handle XOS_THREAD_SELF to specify
///                             itself.
///
///  \param     priority        The new priority level to be set.
///
///  \return    Returns XOS_OK on success, else error code.
///
///  NOTE: Calling this function can result in a scheduler activation, and the
///  caller may be suspended as a result.
///
//-----------------------------------------------------------------------------
int32_t
xos_thread_set_priority(XosThread * thread, int32_t priority);


//-----------------------------------------------------------------------------
///
///  Return the name of the specified thread.
///
///  \param     thread          Handle of thread being queried. A thread can use
///                             the special handle XOS_THREAD_SELF to specify
///                             itself.
///
///  \return    Returns a pointer to the name string if available, else NULL.
///
//-----------------------------------------------------------------------------
static inline const char *
xos_thread_get_name(XosThread * thread)
{
    XOS_ASSERT(thread);
    return thread ? thread->name : 0;
}


//-----------------------------------------------------------------------------
///
///  Set the name of the specified thread.
///
///  \param     thread          Handle of thread whose name is to be set. A thread
///                             can use the special handle XOS_THREAD_SELF to specify
///                             itself.
///
///  \param     name            Pointer to the new name string. The string is not
///                             copied, only the pointer is saved. So the string
///                             must be persistent for the life of the thread.
///
///  \return    Returns XOS_OK on success, else error code.
///
//-----------------------------------------------------------------------------
static inline int32_t
xos_thread_set_name(XosThread * thread, const char * name)
{
    XOS_ASSERT(thread);
    if (thread != XOS_NULL) {
        thread->name = name;
        return XOS_OK;
    }

    return XOS_ERR_INVALID_PARAMETER;
}


//-----------------------------------------------------------------------------
///
///  Set an exit handler for the specified thread. The exit handler is run when
///  the thread terminates, either by calling xos_thread_exit() or by returning
///  from its entry function. It will also be called if the thread is being
///  terminated due to e.g. an unhandled exception.
///
///  The handler must be a function defined as e.g.:
///
///      int32_t exit_handler(int32_t exitcode);
///
///  The exit handler runs in the context of the exiting thread, and can call
///  system services. It is provided with a single parameter which is the
///  thread's exit code (the exit code may be set to an error code if the
///  thread is being terminated due to an error or exception). The handler
///  must return a value which will be set as the thread's exit code.
///
///  \param     thread          Handle of the thread for which the handler is
///                             to be installed. A thread can use the special
///                             handle XOS_THREAD_SELF to specify itself.
///
///  \param     func            Pointer to exit handler function. To clear an
///                             existing handler, pass NULL as the pointer.
///
///  \return    Returns XOS_OK on success, else error code.
///
//-----------------------------------------------------------------------------
int32_t
xos_thread_set_exit_handler(XosThread * thread, XosThdExitFunc * func);


//-----------------------------------------------------------------------------
///
///  Return the ID (handle) of the current thread.
///
///  \return    Returns the handle of the current thread. This handle can be
///             used in all XOS system calls.
///
///  NOTE: If called from interrupt context, returns the handle of the thread
///  that was preempted.
///
//-----------------------------------------------------------------------------
static inline XosThread *
xos_thread_id()
{
    return xos_curr_threadptr;
}


//-----------------------------------------------------------------------------
///
///  Return the coprocessor mask for the specified thread.
///
///  \param     thread          Handle of thread being queried.
///
///  \return    Returns the mask for the specified thread if available, else 0.
///
//-----------------------------------------------------------------------------
static inline uint16_t
xos_thread_cp_mask(XosThread * thread)
{
    XOS_ASSERT(thread);
    return thread ? thread->cp_mask : 0;
}


//-----------------------------------------------------------------------------
///
///  Return the wake value for the specified thread.
///
///  \return    thread          Handle of thread being queried.
///
///  \return    Returns The last set wake value. There is no way to detect what
///             action set the wake value and when.
///
//-----------------------------------------------------------------------------
static inline int32_t
xos_thread_get_wake_value(XosThread * thread)
{
    XOS_ASSERT(thread);
    return thread ? thread->wake_value : 0;
}


//-----------------------------------------------------------------------------
///
///  Return the current value of the event bits for the current thread.
///  This function takes no parameters.
///
///  \return    Returns the current value of the event bits. The event bits
///             are set when the thread is woken from an event wait. They will
///             not change while the thread is running. There is no way to
///             determine when the event bits were last updated.
///
//-----------------------------------------------------------------------------
static inline uint32_t
xos_thread_get_event_bits(void)
{
    XosThread * thread = xos_thread_id();
    return thread ? thread->event_bits : 0;
}


//-----------------------------------------------------------------------------
///
///  Enum values for thread state.
///
//-----------------------------------------------------------------------------
typedef enum xos_thread_state_t {
    XOS_THREAD_STATE_INVALID = 0,       ///< Invalid thread
    XOS_THREAD_STATE_BLOCKED,           ///< Thread is blocked
    XOS_THREAD_STATE_READY,             ///< Thread is ready to run
    XOS_THREAD_STATE_RUNNING,           ///< Thread is running
    XOS_THREAD_STATE_EXITED,            ///< Thread has exited
} xos_thread_state_t;


//-----------------------------------------------------------------------------
///
///  Return the state of the specified thread.
///
///  \param     thread          Handle of thread being queried.
///
///  \return    Returns one of the following values:
///             - XOS_THREAD_STATE_RUNNING -- The thread is currently running.
///             - XOS_THREAD_STATE_READY   -- The thread is ready to run.
///             - XOS_THREAD_STATE_BLOCKED -- The thread is blocked on something.
///             - XOS_THREAD_STATE_INVALID -- The thread handle is invalid.
///             - XOS_THREAD_STATE_EXITED  -- The thread has exited.
///
//-----------------------------------------------------------------------------
xos_thread_state_t
xos_thread_get_state(XosThread * thread);


//-----------------------------------------------------------------------------
///
///  Disable thread preemption. Prevents context switching to another thread.
///  However, interrupt handlers will still continue to be run. Multiple calls
///  will nest, and the same number of calls to xos_preemption_enable() will be
///  required to re-enable preemption. If the calling thread yields the CPU or
///  exits without enabling preemption, it will cause a system halt.
///  If the calling thread encounters a fatal error, preemption will be enabled
///  during fatal error handling.
///
///  \return    Returns the new value of preemption disable flag after this call.
///
///  NOTE: Cannot be called from interrupt context.
///
//-----------------------------------------------------------------------------
uint32_t
xos_preemption_disable(void);


//-----------------------------------------------------------------------------
///
///  Enable thread preemption. Has no effect if preemption was already enabled.
///  Otherwise, it decrements the value of the preemption disable flag and if
///  the value goes to zero, enables preemption.
///
///  \return    Returns the new value of preemption disable flag after this call.
///
///  NOTE: If scheduling gets enabled, it may cause an immediate context switch
///  if higher priority threads are ready.
///
//-----------------------------------------------------------------------------
uint32_t
xos_preemption_enable(void);


//-----------------------------------------------------------------------------
///
///  Initialize XOS thread support and start scheduler.
///
///  Must be called from main() before calling any other thread function.
///  This function initializes thread support, creates the idle thread, and
///  starts the scheduler. It does not return to its caller. This means that
///  at least one user thread must be created before calling xos_start().
///  Otherwise, the scheduler will run the idle thread since it will be the
///  only thread in the system, and no other thread can be created.
///
///  NOTE: This function does not initialize timer/tick support. For timer
///  services to be available xos_start_system_timer() must be called.
///
///  NOTE: xos_start() and xos_start_main() are exclusive, both cannot be
///  called within the same application.
///
///  \param     flags           Currently unused (pass 0).
///
///  \return    Does not return.
///
//-----------------------------------------------------------------------------
void
xos_start(uint32_t flags);


//-----------------------------------------------------------------------------
///
///  Initialize XOS thread support and create init (main) thread.
///
///  Must be called from main() before calling any other thread function.
///  This function converts the caller into the 'main' or 'init' thread, and
///  returns to the caller after completing initialization. 
///
///  NOTE: This function does not initialize timer/tick support. For timer
///  services to be available xos_start_system_timer() must be called.
///
///  NOTE: xos_start_main() and xos_start() are exclusive, both cannot be
///  called within the same application.
///
///  \param     name            Name of main thread (see xos_thread_create()).
///
///  \param     priority        Initial priority of main thread.
///
///  \param     flags           Currently unused (pass 0).
///
///  \return    Returns nothing.
///
//-----------------------------------------------------------------------------
void
xos_start_main(const char * name, int8_t priority, uint32_t flags);


//-----------------------------------------------------------------------------
///
///  Per-thread stats structure.
///  Note that the CPU use % is approximate, both because of cycle counting
///  and because of integer division. So all the threads' CPU % will not add
///  up to exactly 100%.
///
//-----------------------------------------------------------------------------
typedef struct XosThreadStats {
  XosThread *   thread;            ///< Thread handle (or pseudo-handle)
  uint32_t      cpu_pct;           ///< CPU use % for this thread
  uint32_t      normal_switches;   ///< Number of non-preemptive switches.
  uint32_t      preempt_switches;  ///< Number of preemptive switches.
  uint64_t      cycle_count;       ///< Number of cycles consumed.
} XosThreadStats;


//-----------------------------------------------------------------------------
//  Thread pseudo-handles.
//-----------------------------------------------------------------------------
#define XOS_THD_STATS_IDLE      ((XosThread *) 1)
#define XOS_THD_STATS_INTR      ((XosThread *) 2)


//-----------------------------------------------------------------------------
///
///  Get the thread statistics for the specified thread. Statistics are only
///  available if XOS_OPT_STATS has been enabled. Otherwise, the function
///  will return XOS_OK, but the structure contents will be undefined.
///
///  \param     thread          Handle of thread being queried. The following
///                             special pseudo-handles can be used:
///                             - XOS_THD_STATS_IDLE -- stats for idle thread
///                             - XOS_THD_STATS_INTR -- stats for interrupt processing
///
///  \param     stats           Pointer to XosThreadStats struct to be filled in.
///
///  \return    Returns XOS_OK on success, else error code.
///
///  NOTE: Can be called from interrupt context.
///  NOTE: This call will not fill in the "thread" and "cpu_pct" fields in the
///  "stats" structure. The thread handle is already known, and calculating the
///  CPU loading can take quite a bit of time so is not done here.
///
//-----------------------------------------------------------------------------
int32_t
xos_thread_get_stats(XosThread * thread, XosThreadStats * stats);


//-----------------------------------------------------------------------------
///
///  Get CPU loading statistics for the system. This function computes the CPU
///  percentage use for all threads in the system (including the idle thread and
///  the 'interrupt thread' (interrupt context). It also returns the cycle count
///  and number of context switches for each thread.
///  Statistics are only available if XOS_OPT_STATS has been enabled.
///  Otherwise, the function will return XOS_OK, but the structure contents will
///  be undefined.
///
///  IMPORTANT: The entry for interrupt context does not contain a real thread
///  handle. It uses the pseudo-handle XOS_THD_STATS_INTR to indicate that this
///  entry reports interrupt statistics. This pseudo-handle cannot be used for
///  any other thread operations or queries.
///
///  NOTE: This function disables interrupts while traversing the thread list.
///  It does not leave interrupts disabled during the computations, as that can
///  take a fair amount of time.
///
///  \param     stats           Pointer to an array of XosThreadStats structures.
///                             The array must be large enough to accommodate all
///                             threads in the system.
///
///  \param     size            The number of elements available in the array. If
///                             this is smaller than the number of threads plus one
///                             (for the interrupt context) then XOS_ERR_INVALID_PARAMETER
///                             will be returned and '*size' will be set to the
///                             minimum number of elements required. On a successful
///                             return, '*size' is set to the number of elements
///                             actually filled in.
///
///  \param     reset           If nonzero, then thread stats counters are reset
///                             after reading. This is useful if you want to track
///                             the stats so as to get a better idea of current
///                             system loading. E.g. calling this function once a
///                             second with 'reset' nonzero will provide CPU load
///                             information for the last second on each call.
///
///  \return    Returns XOS_OK on success, else error code. In particular,
///             XOS_ERR_INVALID_PARAMETER will be returned if the output buffer
///             is too small.
///
//-----------------------------------------------------------------------------
int32_t
xos_get_cpu_load(XosThreadStats * stats, int32_t * size, int32_t reset);


#ifdef _XOS_INCLUDE_INTERNAL_

// Signature of valid thread object
#define XOS_THREAD_SIG  0x54485244


// Extern functions
void
xos_init(void);

bool
xos_init_done(void);

bool
xos_started(void);

int32_t
xos_schedule(XosThread * curr_thread);

void
xos_q_remove(XosThreadQueue * queue, XosThread * thread);

XosThread *
xos_q_pop(XosThreadQueue * queue);

int32_t
xos_wake_queue(XosThreadQueue * queue, const char * expected_cause, int32_t wake_value);

// Well known block causes
extern const char * const xos_blkon_idle;       // (for idle thread only)
extern const char * const xos_blkon_suspend;
extern const char * const xos_blkon_delay;
extern const char * const xos_blkon_exited;
extern const char * const xos_blkon_join;
extern const char * const xos_blkon_event;
extern const char * const xos_blkon_condition;
extern const char * const xos_blkon_mutex;
extern const char * const xos_blkon_sem;
extern const char * const xos_blkon_msgq;


//-----------------------------------------------------------------------------
//  Blocks the current active thread.
//
//  Currently, this can be called from an interrupt handler to block the thread
//  that was interrupted. Note that in interrupt context the current thread can
//  already be in the blocked state, due to a previous call to this function.
//  Can be called with interrupts enabled.
//
//  block_cause         Reason for blocking.
//
//  block_queue         Queue on to which this thread should be pushed once it
//                      is blocked. Can be NULL.
//
//  must_schedule       If nonzero, then forces a scheduling operation to pick
//                      the next thread to run, even if there are ready threads
//                      at the same priority level as the blocked thread.
//
//  use_priority        If nonzero, then the blocked thread will be queued in
//                      priority order in the specified block queue. If zero,
//                      the thread is queued in FIFO order. If no queue has
//                      been specified, this parameter is ignored.
//
//  Returns: The value passed by xos_thread_wake().
//-----------------------------------------------------------------------------
int32_t
xos_block(const char *     block_cause,
          XosThreadQueue * block_queue,
          int32_t          must_schedule,
          int32_t          use_priority);


//-----------------------------------------------------------------------------
//  Unblocks the specified blocked thread and puts it at the tail end of its
//  ready queue. Schedules it if it is higher priority than the current thread.
//  No effect if the thread is not blocked with the specified cause.
//
//  thread              The thread to wake up (make ready).
//
//  expected_cause      The expected block cause of the thread. Thread will be
//                      woken only if its block cause matches this cause, or if
//                      expected_cause is zero.
//  
//  wake_value          The value to be passed to the woken thread as a return
//                      value from xos_thread_block().
//
//  Returns: nothing.
//
//  The target thread can be woken at a different priority by changing its
//  priority while the thread is blocked.
//  Can be called with interrupts enabled. Can be called in interrupt context.
//-----------------------------------------------------------------------------
void
xos_thread_wake(XosThread * thread, const char * expected_cause, int32_t wakevalue);


//-----------------------------------------------------------------------------
//  Function to init C library per-thread and reentrancy support.
//-----------------------------------------------------------------------------
#if XOS_OPT_THREAD_SAFE_CLIB
void
xos_clib_init(void);

void
xos_clib_thread_init(XosThread * thread);

void
xos_clib_thread_cleanup(XosThread * thread);
#endif

#endif


#ifdef __cplusplus
}
#endif

#endif  // __XOS_THREAD_H__

