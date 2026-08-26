/******************************************************************************
 *
 *  Copyright (C) 2014 The Android Open Source Project
 *  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#ifndef _OI_OSINTERFACE_H
#define _OI_OSINTERFACE_H
/**
 @file
 * This file provides the platform-independent interface for functions for which
 * implementation is platform-specific.
 *
 * The functions in this header file define the operating system or hardware
 * services needed by the BLUEmagic 3.0 protocol stack. The
 * actual implementation of these services is platform-dependent.
 *
 */

/**********************************************************************************
  $Revision: #1 $
***********************************************************************************/

#include "oi_stddefs.h"
#include "oi_time.h"
#include "oi_status.h"
#include "oi_modules.h"

/** \addtogroup Misc Miscellaneous APIs */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Terminates execution.
 *
 * @param reason  Reason for termination
 */
void OI_FatalError(OI_STATUS reason);

/**
 * This function logs an error.
 *
 * When built for release mode, BLUEmagic 3 errors are logged to
 * this function. (in debug mode, errors are logged via
 * OI_Print()).
 *
 * @param module Module in which the error was detected (see
 *                oi_modules.h)
 * @param lineno Line number of the C file OI_SLOG_ERROR called
 * @param status Status code associated with the error event
 */
void OI_LogError(OI_MODULE module, OI_INT lineno, OI_STATUS status);

/**
 * This function initializes the debug code handling.
 *
 * When built for debug mode, this function performs platform
 * dependent initialization to handle message codes passed in
 * via OI_SetMsgCode().
 */
void OI_InitDebugCodeHandler(void);


/**
 * This function reads the time from the real time clock.
 *
 * All timing in BM3 is relative, typically a granularity
 * of 5 or 10 msecs is adequate.
 *
 * @param[out] now  Pointer to the buffer to which the current
 *       time will be returned
 */
void OI_Time_Now(OI_TIME *now);

/**
 * This function causes the current thread to sleep for the
 * specified amount of time. This function must be called
 * without the stack access token.
 *
 * @note BM3 corestack and profiles never suspend and never call
 * OI_Sleep. The use of OI_Sleep is limited to applications and
 * platform-specific code.
 *
 * If your port and applications never use OI_Sleep, this function can be left unimplemented.
 *
 * @param milliseconds  Number of milliseconds to sleep
 */
void OI_Sleep(OI_UINT32 milliseconds);


/**
 * Defines for message type codes.
 */
#define OI_MSG_CODE_APPLICATION               0   /**< Application output */
#define OI_MSG_CODE_ERROR                     1   /**< Error message output */
#define OI_MSG_CODE_WARNING                   2   /**< Warning message output */
#define OI_MSG_CODE_TRACE                     3   /**< User API function trace output */
#define OI_MSG_CODE_PRINT1                    4   /**< Catagory 1 debug print output */
#define OI_MSG_CODE_PRINT2                    5   /**< Catagory 2 debug print output */
#define OI_MSG_CODE_HEADER                    6   /**< Error/Debug output header */

/**
 * This function is used to indicate the type of text being output with
 * OI_Print(). For the Linux and Win32 platforms, it will set
 * the color of the text. Other possible uses could be to insert
 * HTML style tags, add some other message type indication, or
 * be completely ignored altogether.
 *
 * @param code  OI_MSG_CODE_* indicating setting the message type.
 */
void OI_SetMsgCode(OI_UINT8 code);

/**
 * All output from OI_Printf() and all debug output is sent to OI_Print.
 * Typically, if the platform has a console, OI_Print() is sent to stdout.
 * Embedded platforms typically send OI_Print() output to a serial port.
 *
 * @param str  String to print
 */
void OI_Print(OI_CHAR const *str);

/**
 *  In cases where OI_Print() is sending output to a logfile in addition to console,
 *  it is desirable to also put console input into the logfile.
 *  This function can be called by the console input process.
 *
 *  @note This is an optional API which is strictly
 *  between the platform-specific stack_console and osinterface
 *  modules. This API need only be implemented on those
 *  platforms where is serves a useful purpose, e.g., win32.
 *
 * @param str  Console input string
 */

void OI_Print_ConsoleInput(OI_CHAR const *str);

/**
 *  This function computes the CRC16 of the program image.
 */
OI_UINT16  OI_ProgramImageCRC16(void);

/**
 * Writes an integer to stdout in hex. This macro is intended
 * for selective use when debugging in small memory
 * configurations or other times when it is not possible to use
 * OI_DBGPRINT.
 *
 * @param n  the integer to print
 */

#define OI_Print_Int(n) \
{ \
    static const OI_CHAR _digits[] = "0123456789ABCDEF"; \
    OI_CHAR _buf[9]; \
    OI_CHAR *_str = &_buf[8]; \
    OI_UINT32 _i = n; \
    *_str = 0; \
    do { *(--_str) = _digits[(_i & 0xF)]; _i >>= 4; } while (_i); \
    OI_Print(_str); \
}

/**
 *  Application Dynamic Memory allocation.
 *
 *  These APIs are provided for application use on those
 *  platforms which have no dynamic memory support. Memory is
 *  allocated from the pool-based heap managed by the stack's
 *  internal memory manager.
 */
void *OI_APP_Malloc(OI_INT32 size);
void OI_APP_Free(void *ptr);

/*****************************************************************************/
#ifdef __cplusplus
}
#endif

/**@}*/

#endif /* _OI_OSINTERFACE_H */
