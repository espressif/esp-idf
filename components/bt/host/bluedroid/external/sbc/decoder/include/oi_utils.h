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
#ifndef _OI_UTILS_H
#define _OI_UTILS_H
/**
 * @file
 *
 * This file provides the interface for utility functions.
 * Among the utilities are strlen (string length), strcmp (string compare), and
 * other string manipulation functions. These are provided for those plaforms
 * where this functionality is not available in stdlib.
 */

/**********************************************************************************
  $Revision: #1 $
***********************************************************************************/

#include <stdarg.h>
#include "oi_common.h"
#include "oi_string.h"
#include "oi_bt_spec.h"

/** \addtogroup Misc Miscellaneous APIs */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque type for a callback function handle. See OI_ScheduleCallbackFunction()
 */
typedef OI_UINT32 OI_CALLBACK_HANDLE;


/**
 * Function prototype for a timed procedure callback.
 *
 * @param arg                 Value that was passed into the OI_ScheduleCallback() function
 *
 */
typedef void (*OI_SCHEDULED_CALLBACK)(void *arg);


/**
 * Registers a function to be called when a timeout expires. This API uses BLUEmagic's internal
 * function dispatch mechanism, so applications that make extensive use of this facility may need to
 * increase the value of DispatchTableSize in the configuration block for the dispatcher (see
 * oi_bt_stack_config.h).
 *
 * @param callbackFunction    The function that will be called when the timeout expires
 *
 * @param arg                 Value that will be returned as the parameter to the callback function.
 *
 * @param timeout             A timeout expressed in OI_INTERVALs (tenths of seconds). This can be
 *                            zero in which case the callback function will be called as soon as
 *                            possible.
 *
 * @param handle              NULL or a pointer receive the callback handle.
 *
 * @return                    OI_OK if the function was reqistered, or an error status.
 */
OI_STATUS OI_ScheduleCallbackFunction(OI_SCHEDULED_CALLBACK callbackFunction,
                                      void                 *arg,
                                      OI_INTERVAL           timeout,
                                      OI_CALLBACK_HANDLE   *handle);


/**
 * Cancels a function registered with OI_ScheduleCallbackFunction() before its timer expires.
 *
 * @param handle              handle returned by  OI_ScheduleCallbackFunction().
 *
 * @return                    OI_OK if the function was cancelled, or an error status.
 */
OI_STATUS OI_CancelCallbackFunction(OI_CALLBACK_HANDLE handle);


/**
 * Registers a function to be called when a timeout expires. This version does not return a handle
 * so can only be canceled by calling OI_CancelCallback().
 *
 * @param callbackFunction    The function that will be called when the timeout expires
 *
 * @param arg                 Value that will be returned as the parameter to the callback function.
 *
 * @param timeout             A timeout expressed in OI_INTERVALs (tenths of seconds). This can be
 *                            zero in which case the callback function will be called as soon as
 *                            possible.
 *
 * @return                    OI_OK if the function was reqistered, or an error status.
 */
#define OI_ScheduleCallback(f, a, t)  OI_ScheduleCallbackFunction(f, a, t, NULL);


/**
 * Cancels a function registered with OI_ScheduleCallback() before its timer expires. This
 * function will cancel the first entry matches the indicated callback function pointer.
 *
 * @param callbackFunction    The function that was originally registered
 *
 * @return                    OI_OK if the function was cancelled, or an error status.
 */
OI_STATUS OI_CancelCallback(OI_SCHEDULED_CALLBACK callbackFunction);


/**
 * Parse a Bluetooth device address from the specified string.
 *
 * @param str   the string to parse
 * @param addr  the parsed address, if successful
 *
 * @return TRUE if an address was successfully parsed, FALSE otherwise
 */

OI_BOOL OI_ParseBdAddr(const OI_CHAR *str,
                       OI_BD_ADDR    *addr) ;

/**
 * Printf function for platforms which have no stdio or printf available.
 * OI_Printf supports the basic formatting types, with the exception of
 * floating point types. Additionally, OI_Printf supports several formats
 * specific to BLUEmagic 3.0 software:
 *
 * \%!   prints the string for an #OI_STATUS value.
 *       @code OI_Printf("There was an error %!", status); @endcode
 *
 * \%@   prints a hex dump of a buffer.
 *       Requires a pointer to the buffer and a signed integer length
 *       (0 for default length). If the buffer is large, only an excerpt will
 *       be printed.
 *       @code OI_Printf("Contents of buffer %@", buffer, sizeof(buffer)); @endcode
 *
 * \%:   prints a Bluetooth address in the form "HH:HH:HH:HH:HH:HH".
 *       Requires a pointer to an #OI_BD_ADDR.
 *       @code OI_Printf("Bluetooth address %:", &bdaddr); @endcode
 *
 * \%^   decodes and prints a data element as formatted XML.
 *       Requires a pointer to an #OI_DATAELEM.
 *       @code OI_Printf("Service attribute list is:\n%^", &attributes); @endcode
 *
 * \%/   prints the base file name of a path, that is, the final substring
 *       following a '/' or '\\' character. Requires a pointer to a null
 *       terminated string.
 *       @code OI_Printf("File %/", "c:\\dir1\\dir2\\file.txt"); @endcode
 *
 * \%~   prints a string, escaping characters as needed to display it in
 *       ASCII. Requires a pointer to an #OI_PSTR and an #OI_UNICODE_ENCODING
 *       parameter.
 *       @code OI_Printf("Identifier %~", &id, OI_UNICODE_UTF16_BE); @endcode
 *
 * \%[   inserts an ANSI color escape sequence. Requires a single character
 *       identifying the color to select. Colors are red (r/R), green (g/G),
 *       blue (b/B), yellow (y/Y), cyan (c/C), magenta (m/M), white (W),
 *       light-gray (l/L), dark-gray (d/D), and black (0). The lower case is
 *       dim, the upper case is bright (except in the case of light-gray and
 *       dark-gray, where bright and dim are identical). Any other value will
 *       select the default color.
 *       @code OI_Printf("%[red text %[black %[normal\n", 'r', '0', 0); @endcode
 *
 * \%a   same as \%s, except '\\r' and '\\n' are output as "<cr>" and "<lf>".
 *       \%?a is valid, but \%la is not.
 *
 * \%b   prints an integer in base 2.
 *       @code OI_Printf("Bits are %b", I); @endcode
 *
 * \%lb  prints a long integer in base 2.
 *
 * \%?b  prints the least significant N bits of an integer (or long integer)
 *       in base 2. Requires the integer and a length N.
 *       @code OI_Printf("Bottom 4 bits are: %?b", I, 4); @endcode
 *
 * \%B   prints an integer as boolean text, "TRUE" or "FALSE".
 *       @code OI_Printf("The value 0 is %B, the value 1 is %B", 0, 1); @endcode
 *
 * \%?s  prints a substring up to a specified maximum length.
 *       Requires a pointer to a string and a length parameter.
 *       @code OI_Printf("String prefix is %?s", str, 3); @endcode
 *
 * \%ls  same as \%S.
 *
 * \%S   prints a UTF16 string as UTF8 (plain ASCII, plus 8-bit char sequences
 *       where needed). Requires a pointer to #OI_CHAR16. \%?S is valid. The
 *       length parameter is in OI_CHAR16 characters.
 *
 * \%T   prints time, formatted as "secs.msecs".
 *       Requires pointer to #OI_TIME struct, NULL pointer prints current time.
 *       @code OI_Printf("The time now is %T", NULL); @endcode
 *
 *  @param format   The format string
 *
 */
void OI_Printf(const OI_CHAR *format, ...);


/**
 * Var-args version OI_Printf
 *
 * @param format   Same as for OI_Printf.
 *
 * @param argp     Var-args list.
 */
void OI_VPrintf(const OI_CHAR *format, va_list argp);


/**
 * Writes a formatted string to a buffer. This function supports the same format specifiers as
 * OI_Printf().
 *
 * @param buffer   Destination buffer for the formatted string.
 *
 * @param bufLen   The length of the destination buffer.
 *
 * @param format   The format string
 *
 * @return   Number of characters written or -1 in the case of an error.
 */
OI_INT32 OI_SNPrintf(OI_CHAR *buffer,
                     OI_UINT16 bufLen,
                     const OI_CHAR *format, ...);


/**
 * Var-args version OI_SNPrintf
 *
 * @param buffer   Destination buffer for the formatted string.
 *
 * @param bufLen   The length of the destination buffer.
 *
 * @param format   The format string
 *
 * @param argp     Var-args list.
 *
 * @return   Number of characters written or -1 in the case of an error.
 */
OI_INT32 OI_VSNPrintf(OI_CHAR *buffer,
                      OI_UINT16 bufLen,
                      const OI_CHAR *format, va_list argp);


/**
 * Convert a string to an integer.
 *
 * @param str  the string to parse
 *
 * @return the integer value of the string or 0 if the string could not be parsed
 */
OI_INT OI_atoi(const OI_CHAR *str);


/**
 * Parse a signed integer in a string.
 *
 * Skips leading whitespace (space and tabs only) and parses a decimal or hex string. Hex string
 * must be prefixed by "0x". Returns pointer to first character following the integer. Returns the
 * pointer passed in if the string does not describe an integer.
 *
 * @param str    String to parse.
 *
 * @param val    Pointer to receive the parsed integer value.
 *
 * @return       A pointer to the first character following the integer or the pointer passed in.
 */
const OI_CHAR *OI_ScanInt(const OI_CHAR *str,
                          OI_INT32 *val);


/**
 * Parse an unsigned integer in a string.
 *
 * Skips leading whitespace (space and tabs only) and parses a decimal or hex string. Hex string
 * must be prefixed by "0x". Returns pointer to first character following the integer. Returns the
 * pointer passed in if the string does not describe an integer.
 *
 * @param str    String to parse.
 *
 * @param val    Pointer to receive the parsed unsigned integer value.
 *
 * @return       A pointer to the first character following the unsigned integer or the pointer passed in.
 */
const OI_CHAR *OI_ScanUInt(const OI_CHAR *str,
                           OI_UINT32 *val);

/**
 * Parse a whitespace delimited substring out of a string.
 *
 * @param str     Input string to parse.
 * @param outStr  Buffer to return the substring
 * @param len     Length of outStr
 *
 *
 * @return       A pointer to the first character following the substring or the pointer passed in.
 */
const OI_CHAR *OI_ScanStr(const OI_CHAR *str,
                          OI_CHAR *outStr,
                          OI_UINT16 len);


/**
 * Parse a string for one of a set of alternative value. Skips leading whitespace (space and tabs
 * only) and parses text matching one of the alternative strings. Returns pointer to first character
 * following the matched text.
 *
 * @param str    String to parse.
 *
 * @param alts   Alternative matching strings separated by '|'
 *
 * @param index  Pointer to receive the index of the matching alternative, return value is -1 if
 *               there is no match.
 *
 * @return       A pointer to the first character following the matched value or the pointer passed in
 *               if there was no matching text.
 */
const OI_CHAR *OI_ScanAlt(const OI_CHAR *str,
                          const OI_CHAR *alts,
                          OI_INT *index);

/**
 * Parse a string for a BD Addr. Skips leading whitespace (space and tabs only) and parses a
 * Bluetooth device address with nibbles optionally separated by colons. Return pointet to first
 * character following the BD Addr.
 *
 * @param str    String to parse.
 *
 * @param addr   Pointer to receive the Bluetooth device address
 *
 * @return       A pointer to the first character following the BD Addr or the pointer passed in.
 */
const OI_CHAR *OI_ScanBdAddr(const OI_CHAR *str,
                             OI_BD_ADDR *addr);


/** Get a character from a digit integer value (0 - 9). */
#define OI_DigitToChar(d) ((d) + '0')

/**
 * Determine Maximum and Minimum between two arguments.
 *
 * @param a  1st value
 * @param b  2nd value
 *
 * @return the max or min value between a & b
 */
#define OI_MAX(a, b) (((a) < (b)) ? (b) : (a) )
#define OI_MIN(a, b) (((a) > (b)) ? (b) : (a) )

/**
 * Compare two BD_ADDRs
 * SAME_BD_ADDR - Boolean: TRUE if they are the same address
 */

#define SAME_BD_ADDR(x, y)      (0 == OI_MemCmp((x),(y),OI_BD_ADDR_BYTE_SIZE) )

#ifdef __cplusplus
}
#endif

/**@}*/

#endif /* _OI_UTILS_H */
