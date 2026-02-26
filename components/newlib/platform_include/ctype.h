/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include_next <ctype.h>

#if CONFIG_LIBC_PICOLIBC_NEWLIB_COMPATIBILITY
#ifndef _U
#define _U __CTYPE_UPPER
#endif

#ifndef _L
#define _L __CTYPE_LOWER
#endif

#ifndef _N
#define _N __CTYPE_DIGIT
#endif

#ifndef _S
#define _S __CTYPE_SPACE
#endif

#ifndef _P
#define _P __CTYPE_PUNCT
#endif

#ifndef _C
#define _C __CTYPE_CNTRL
#endif

#ifndef _X
#define _X __CTYPE_HEX
#endif

#ifndef _B
#define _B __CTYPE_BLANK
#endif
#endif
