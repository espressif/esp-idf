/*
 * SPDX-FileCopyrightText: 2001-2008 Tensilica Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 */
/* ANSI C standard library function strncpy.

   Copyright (c) 2001-2008 Tensilica Inc.

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

#include <picolibc.h>

#include "xtensa-asm.h"

	.text
.begin schedule
	.align	4
	.literal_position
__strncpy_aux:

.Lsrc1mod2: // src address is odd
	l8ui	a8, a3, 0	// get byte 0
	addi	a3, a3, 1	// advance src pointer
	s8i	a8, a10, 0	// store byte 0
	addi	a4, a4, -1	// decrement n
	beqz    a4, .Lret       // if n is zero
	addi	a10, a10, 1	// advance dst pointer
	beqz	a8, .Lfill	// if byte 0 is zero
	bbci.l	a3, 1, .Lsrcaligned // if src is now word-aligned

.Lsrc2mod4: // src address is 2 mod 4
	l8ui	a8, a3, 0	// get byte 0
	addi	a4, a4, -1	// decrement n
	s8i	a8, a10, 0	// store byte 0
	beqz    a4, .Lret       // if n is zero
	addi	a10, a10, 1	// advance dst pointer
	beqz	a8, .Lfill	// if byte 0 is zero
	l8ui	a8, a3, 1	// get byte 0
	addi	a3, a3, 2	// advance src pointer
	s8i	a8, a10, 0	// store byte 0
	addi	a4, a4, -1	// decrement n
	beqz    a4, .Lret       // if n is zero
	addi	a10, a10, 1	// advance dst pointer
	bnez	a8, .Lsrcaligned
	j	.Lfill

.Lret:
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	leaf_return


	.align	4
	.global	strncpy
	.type	strncpy, @function
strncpy:
	leaf_entry sp, 16
	/* a2 = dst, a3 = src */

	mov	a10, a2		// leave dst in return value register
	beqz    a4, .Lret       // if n is zero

	movi	a11, MASK0
	movi	a5, MASK1
	movi	a6, MASK2
	movi	a7, MASK3
	bbsi.l	a3, 0, .Lsrc1mod2
	bbsi.l	a3, 1, .Lsrc2mod4
.Lsrcaligned:

	/* Check if the destination is aligned.  */
	movi	a8, 3
	bnone	a10, a8, .Laligned

	j	.Ldstunaligned


/* Fill the dst with zeros -- n is at least 1.  */

.Lfill:
	movi	a9, 0
	bbsi.l	a10, 0, .Lfill1mod2
	bbsi.l	a10, 1, .Lfill2mod4
.Lfillaligned:
	blti	a4, 4, .Lfillcleanup

	/* Loop filling complete words with zero.  */
#if XCHAL_HAVE_LOOPS

	srai	a8, a4, 2
	loop	a8, 1f
	s32i	a9, a10, 0
	addi	a10, a10, 4

1:	slli	a8, a8, 2
	sub	a4, a4, a8

#else /* !XCHAL_HAVE_LOOPS */

1:	s32i	a9, a10, 0
	addi	a10, a10, 4
	addi	a4, a4, -4
	bgei    a4, 4, 1b

#endif /* !XCHAL_HAVE_LOOPS */

	beqz	a4, 2f

.Lfillcleanup:
	/* Fill leftover (1 to 3) bytes with zero.  */
	s8i	a9, a10, 0	// store byte 0
	addi	a4, a4, -1	// decrement n
	addi	a10, a10, 1
	bnez    a4, .Lfillcleanup

2:
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	leaf_return

.Lfill1mod2: // dst address is odd
	s8i	a9, a10, 0	// store byte 0
	addi	a4, a4, -1	// decrement n
	beqz    a4, 2b		// if n is zero
	addi    a10, a10, 1	// advance dst pointer
	bbci.l	a10, 1, .Lfillaligned // if dst is now word-aligned

.Lfill2mod4: // dst address is 2 mod 4
	s8i	a9, a10, 0	// store byte 0
	addi	a4, a4, -1	// decrement n
	beqz    a4, 2b		// if n is zero
	s8i	a9, a10, 1	// store byte 1
	addi	a4, a4, -1	// decrement n
	beqz    a4, 2b		// if n is zero
	addi    a10, a10, 2	// advance dst pointer
	j	.Lfillaligned


/* dst is word-aligned; src is word-aligned; n is at least 1.  */

	.align	4
#if XCHAL_HAVE_LOOPS
#if XCHAL_HAVE_DENSITY
	/* (2 mod 4) alignment for loop instruction */
#else
	/* (1 mod 4) alignment for loop instruction */
	.byte	0
	.byte	0
#endif
#endif
.Laligned:
#if XCHAL_HAVE_LOOPS
#if XCHAL_HAVE_DENSITY
	_movi.n	a8, 0		// set up for the maximum loop count
#else
	_movi	a8, 0		// set up for the maximum loop count
#endif
	loop	a8, 1f		// loop forever (almost anyway)
	blti	a4, 5, .Ldstunaligned // n is near limit; do one at a time
	l32i	a8, a3, 0	// get word from src
	addi	a3, a3, 4	// advance src pointer
	bnone	a8, a11, .Lz0	// if byte 0 is zero
	bnone	a8, a5, .Lz1	// if byte 1 is zero
	bnone	a8, a6, .Lz2	// if byte 2 is zero
	s32i	a8, a10, 0	// store word to dst
	addi	a4, a4, -4	// decrement n
	addi	a10, a10, 4	// advance dst pointer
	bnone	a8, a7, .Lfill	// if byte 3 is zero
1:

#else /* !XCHAL_HAVE_LOOPS */

1:	blti	a4, 5, .Ldstunaligned // n is near limit; do one at a time
	l32i	a8, a3, 0	// get word from src
	addi	a3, a3, 4	// advance src pointer
	bnone	a8, a11, .Lz0	// if byte 0 is zero
	bnone	a8, a5, .Lz1	// if byte 1 is zero
	bnone	a8, a6, .Lz2	// if byte 2 is zero
	s32i	a8, a10, 0	// store word to dst
	addi	a4, a4, -4	// decrement n
	addi	a10, a10, 4	// advance dst pointer
	bany	a8, a7, 1b	// no zeroes
#endif /* !XCHAL_HAVE_LOOPS */

	j	.Lfill

.Lz0:	/* Byte 0 is zero.  */
#ifdef __XTENSA_EB__
	movi	a8, 0
#endif
	s8i	a8, a10, 0
	addi	a4, a4, -1	// decrement n
	addi	a10, a10, 1	// advance dst pointer
	j	.Lfill

.Lz1:	/* Byte 1 is zero.  */
#ifdef __XTENSA_EB__
        extui   a8, a8, 16, 16
#endif
	s16i	a8, a10, 0
	addi	a4, a4, -2	// decrement n
	addi	a10, a10, 2	// advance dst pointer
	j	.Lfill

.Lz2:	/* Byte 2 is zero.  */
#ifdef __XTENSA_EB__
        extui   a8, a8, 16, 16
#endif
	s16i	a8, a10, 0
	movi	a8, 0
	s8i	a8, a10, 2
	addi	a4, a4, -3	// decrement n
	addi	a10, a10, 3	// advance dst pointer
	j	.Lfill

	.align	4
#if XCHAL_HAVE_LOOPS
#if XCHAL_HAVE_DENSITY
	/* (2 mod 4) alignment for loop instruction */
#else
	/* (1 mod 4) alignment for loop instruction */
	.byte	0
	.byte	0
#endif
#endif
.Ldstunaligned:

#if XCHAL_HAVE_LOOPS
#if XCHAL_HAVE_DENSITY
	_movi.n	a8, 0		// set up for the maximum loop count
#else
	_movi	a8, 0		// set up for the maximum loop count
#endif
	loop	a8, 2f		// loop forever (almost anyway)
#endif
1:	l8ui	a8, a3, 0
	addi	a3, a3, 1
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	nop
	nop
	nop
#endif
	s8i	a8, a10, 0
	addi	a4, a4, -1
	beqz	a4, 3f
	addi	a10, a10, 1
#if XCHAL_HAVE_LOOPS
	beqz	a8, 2f
#else
	bnez	a8, 1b
#endif
2:	j	.Lfill

3:
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	leaf_return
.end schedule

	.size	strncpy, . - strncpy

	.global	esp_libc_include_strncpy_impl
	.type	esp_libc_include_strncpy_impl, @function
esp_libc_include_strncpy_impl:
	.size	esp_libc_include_strncpy_impl, . - esp_libc_include_strncpy_impl
