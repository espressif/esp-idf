/** @file */

// xos_semaphore.h - XOS Semaphore API interface and data structures.

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

#ifndef __XOS_SEMAPHORE_H__
#define __XOS_SEMAPHORE_H__

#include "xos_types.h"

#ifdef __cplusplus
extern "C" {
#endif


//-----------------------------------------------------------------------------
// Semaphore flags.
//-----------------------------------------------------------------------------
#define XOS_SEM_WAIT_PRIORITY     0x0000  ///< Wake waiters in priority order (default)
#define XOS_SEM_WAIT_FIFO         0x0001  ///< Wake waiters in FIFO order
#define XOS_SEM_PRIORITY_INV      0x0004  // Protect against priority inversion


//-----------------------------------------------------------------------------
///
/// XosSem object.
///
//-----------------------------------------------------------------------------
typedef struct XosSem {
  uint32_t              count;          ///< Current count
  XosThreadQueue        waitq;          ///< Queue of waiters.
  uint32_t              flags;          ///< Properties.
#if XOS_SEM_DEBUG
  uint32_t              sig;            // Valid signature indicates inited.
#endif
} XosSem;


//-----------------------------------------------------------------------------
///
///  Initialize a semaphore object before first use.
///
///  \param     sem             Pointer to semaphore object.
///
///  \param     flags           Creation flags:
///                             - XOS_SEM_WAIT_FIFO -- queue waiting threads in
///                               fifo order.
///                             - XOS_SEM_WAIT_PRIORITY -- queue waiting threads
///                               by priority. This is the default.
///                             - XOS_SEM_PRIORITY_INV -- protect against priority
///                               inversion.
///
///  \param     initial_count   Initial count for semaphore on creation.
///
///  \return    Returns XOS_OK on success, else error code.
///
///  NOTE: XOS_SEM_PRIORITY_INV is NOT supported in the current release. It will
///  be supported in a future release.
///
//-----------------------------------------------------------------------------
int32_t
xos_sem_create(XosSem * sem, uint32_t flags, uint32_t initial_count);


//-----------------------------------------------------------------------------
///
///  Destroy a semaphore object. Must have been previously created by calling
///  xos_sem_create().
///
///  \param     sem             Pointer to semaphore object.
///
///  \return    Returns XOS_OK on success, else error code.
///
//-----------------------------------------------------------------------------
int32_t
xos_sem_delete(XosSem * sem);


//-----------------------------------------------------------------------------
///
///  Decrement the semaphore count: block until the decrement is possible.
///  The semaphore must have been initialized.
///
///  \param     sem             Pointer to semaphore object.
///
///  \return    Returns XOS_OK on success, else error code.
///
//-----------------------------------------------------------------------------
int32_t
xos_sem_get(XosSem * sem);


//-----------------------------------------------------------------------------
///
///  Decrement the semaphore count: block until the decrement is possible or
///  the timeout expires. The semaphore must have been initialized.
///
///  \param     sem             Pointer to semaphore object.
///
///  \param     to_cycles       Timeout in cycles. Convert from time to cycles
///                             using the helper functions provided in xos_timer.
///                             A value of zero indicates no timeout.
///
///  \return    Returns XOS_OK on success, XOS_ERR_TIMEOUT on timeout, else error code.
///
///  NOTE: If XOS_OPT_WAIT_TIMEOUT is not enabled, then the timeout value is
///  ignored, and no timeout will occur.
///
//-----------------------------------------------------------------------------
int32_t
xos_sem_get_timeout(XosSem * sem, uint64_t to_cycles);


//-----------------------------------------------------------------------------
///
///  Increment the semaphore count. The semaphore must have been initialized.
///  Remember that this action may wake up a waiting thread, and if that thread
///  is higher priority then there will be an immediate context switch.
///
///  \param     sem             Pointer to semaphore object.
///
///  \return    Returns XOS_OK on success, else error code.   
///
//-----------------------------------------------------------------------------
int32_t
xos_sem_put(XosSem * sem);


//-----------------------------------------------------------------------------
///
///  Try to decrement the semaphore, but do not block if the semaphore count is
///  zero. Return immediately. The semaphore must have been initialized.
///
///  \param     sem             Pointer to semaphore object.
///
///  \return    Returns XOS_OK on success (semaphore decremented), else error code.
///
//-----------------------------------------------------------------------------
int32_t
xos_sem_tryget(XosSem * sem);


//-----------------------------------------------------------------------------
///
///  Return the count of the semaphore but do not attempt to decrement it.
///  The semaphore must have been initialized.
///
///  \param     sem             Pointer to semaphore object.
///
///  \return    Returns semaphore count, -1 on error.
///
//-----------------------------------------------------------------------------
static inline int32_t
xos_sem_test(XosSem * sem)
{
    XOS_ASSERT(sem);

    return sem ? sem->count : -1;
}


#ifdef __cplusplus
}
#endif

#endif  //      __XOS_SEMAPHORE_H__

