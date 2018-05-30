/** @file */
// xos_types.h - XOS type definitions.

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


#ifndef __XOS_TYPES_H__
#define __XOS_TYPES_H__

#if !defined(_ASMLANGUAGE) && !defined(__ASSEMBLER__)

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

//-----------------------------------------------------------------------------
// The following are defined here because of the variations in the C libraries
// that we need to work with.
//    - Not all of them have stdbool.h
//    - Not all of them define NULL as (void *)0
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
///
/// XOS define for NULL value. This makes the NULL value independent of the
/// C library (not all of them define NULL the same way).
///
//-----------------------------------------------------------------------------
#define XOS_NULL    ((void *)0)


//-----------------------------------------------------------------------------
///
/// XOS definition of 'bool' type. Some C libraries do not support stdbool.h.
///
//-----------------------------------------------------------------------------
#ifndef bool
#define bool        int8_t
#define false       0        ///< XOS definition of 'false'
#define true        1        ///< XOS definition of 'true'
#endif

#ifdef __cplusplus
}
#endif

#endif // !defined(_ASMLANGUAGE) && !defined(__ASSEMBLER__)

#endif // __XOS_TYPES_H__

