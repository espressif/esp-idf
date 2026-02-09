/*
 * SPDX-FileCopyrightText: 2006 Tensilica Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 */
/* Copyright (c) 2006 Tensilica Inc.

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  */

/* Define macros for leaf function entry and return, supporting either the
 * standard register windowed ABI or the non-windowed call0 ABI.  These
 * macros do not allocate any extra stack space, so they only work for
 * leaf functions that do not need to spill anything to the stack.  */

#include <machine/core-isa.h>

.macro leaf_entry reg, size
#if XCHAL_HAVE_WINDOWED && !__XTENSA_CALL0_ABI__
entry \reg, \size
#else
/* do nothing */
#endif
.endm

.macro leaf_return
#if XCHAL_HAVE_WINDOWED && !__XTENSA_CALL0_ABI__
retw
#else
ret
#endif
.endm

.macro  src_b   r, w0, w1
#ifdef __XTENSA_EB__
src \r, \w0, \w1
#else
src \r, \w1, \w0
#endif
.endm

.macro  ssa8    r
#ifdef __XTENSA_EB__
ssa8b   \r
#else
ssa8l   \r
#endif
.endm

#if XCHAL_HAVE_BE
#define MASK0 0xff000000
#define MASK1 0x00ff0000
#define MASK2 0x0000ff00
#define MASK3 0x000000ff
#else
#define MASK0 0x000000ff
#define MASK1 0x0000ff00
#define MASK2 0x00ff0000
#define MASK3 0xff000000
#endif
