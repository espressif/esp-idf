/** @file */

// xos_errors.h - XOS error codes.

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


#ifndef __XOS_ERRORS_H__
#define __XOS_ERRORS_H__

#include "xos_types.h"


#define _XOS_ERR_FIRST          (-65536)
#define _XOS_ERR_LAST           (-1)


//-----------------------------------------------------------------------------
///
///  List of XOS error codes. All error codes are negative integers, except for
///  XOS_OK which is zero.
///  XOS error codes occupy the range from -65536 up to -1.
///  The function IS_XOS_ERRCODE() can be used to check if a value lies within
///  the error code range.
///
//-----------------------------------------------------------------------------
typedef enum xos_err_t {
    XOS_OK = 0,

    XOS_ERR_NOT_FOUND = _XOS_ERR_FIRST,         ///< Object not found
    XOS_ERR_INVALID_PARAMETER,                  ///< Function parameter is invalid
    XOS_ERR_LIMIT,                              ///< Limit exceeded
    XOS_ERR_NOT_OWNED,                          ///< Object not owned by caller
    XOS_ERR_MUTEX_LOCKED,                       ///< Mutex is already locked
    XOS_ERR_MUTEX_NOT_OWNED,                    ///< Mutex not owned by caller
    XOS_ERR_MUTEX_ALREADY_OWNED,                ///< Mutex already owned by caller
    XOS_ERR_MUTEX_DELETE,                       ///< Mutex being waited on has been deleted
    XOS_ERR_COND_DELETE,                        ///< Condition being waited on has been deleted
    XOS_ERR_SEM_DELETE,                         ///< Semaphore being waited on has been deleted
    XOS_ERR_SEM_BUSY,                           ///< Semaphore is not available
    XOS_ERR_EVENT_DELETE,                       ///< Event being waited on has been deleted
    XOS_ERR_MSGQ_FULL,                          ///< Message queue is full
    XOS_ERR_MSGQ_EMPTY,                         ///< Message queue is empty
    XOS_ERR_MSGQ_DELETE,                        ///< Message queue being waited on has been deleted
    XOS_ERR_TIMER_DELETE,                       ///< Timer being waited on has been deleted
    XOS_ERR_CONTAINER_NOT_RTC,                  ///< Containing thread not of RTC type
    XOS_ERR_CONTAINER_NOT_SAME_PRI,             ///< Containing thread not at same priority
    XOS_ERR_STACK_TOO_SMALL,                    ///< Thread stack is too small
    XOS_ERR_CONTAINER_ILLEGAL,                  ///< Illegal container thread
    XOS_ERR_ILLEGAL_OPERATION,                  ///< This operation is not allowed
    XOS_ERR_THREAD_EXITED,                      ///< The thread has already exited
    XOS_ERR_NO_TIMER,                           ///< No suitable timer found
    XOS_ERR_FEATURE_NOT_PRESENT,                ///< This feature is disabled or not implemented
    XOS_ERR_TIMEOUT,                            ///< Wait timed out

    XOS_ERR_UNHANDLED_INTERRUPT,                ///< No handler for interrupt
    XOS_ERR_UNHANDLED_EXCEPTION,                ///< No handler for exception
    XOS_ERR_INTERRUPT_CONTEXT,                  ///< Operation is illegal in interrupt context
    XOS_ERR_THREAD_BLOCKED,                     ///< Thread already blocked
    XOS_ERR_ASSERT_FAILED,                      ///< Runtime assertion failure
    XOS_ERR_CLIB_ERR,                           ///< Error in C library thread safety module
    XOS_ERR_INTERNAL_ERROR,                     ///< XOS internal error

    XOS_ERR_LAST = _XOS_ERR_LAST,
} xos_err_t;


//-----------------------------------------------------------------------------
///
///  Check if a value is a valid XOS error code.
///
///  \param     val             Value to check
///
///  \return    Returns nonzero if 'val' is in the XOS error code range.
///
//-----------------------------------------------------------------------------
static inline int32_t
IS_XOS_ERRCODE(xos_err_t val)
{
    return ((val >= _XOS_ERR_FIRST) && (val <= _XOS_ERR_LAST));
}


#endif // __XOS_ERRORS_H__

