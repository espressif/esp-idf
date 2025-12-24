/*
 * SPDX-FileCopyrightText: 2001-2008 Tensilica Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 */
/* ANSI C standard library function strcpy.

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
	.global	strcpy
	.type	strcpy, @function
strcpy:
	leaf_entry sp, 16
	/* a2 = dst, a3 = src */

	mov	a10, a2		// leave dst in return value register
	movi	a4, MASK0
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

.Lsrc1mod2: // src address is odd
	l8ui	a8, a3, 0	// get byte 0
	addi	a3, a3, 1	// advance src pointer
	s8i	a8, a10, 0	// store byte 0
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	beqz	a8, 1f		// if byte 0 is zero
	addi	a10, a10, 1	// advance dst pointer
	bbci.l	a3, 1, .Lsrcaligned // if src is now word-aligned

.Lsrc2mod4: // src address is 2 mod 4
	l8ui	a8, a3, 0	// get byte 0
	/* 1-cycle interlock */
	s8i	a8, a10, 0	// store byte 0
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	beqz	a8, 1f		// if byte 0 is zero
	l8ui	a8, a3, 1	// get byte 0
	addi	a3, a3, 2	// advance src pointer
	s8i	a8, a10, 1	// store byte 0
	addi	a10, a10, 2	// advance dst pointer
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	bnez	a8, .Lsrcaligned
1:	leaf_return


/* dst is word-aligned; src is word-aligned.  */

	.align	4
#if XCHAL_HAVE_LOOPS
#if XCHAL_HAVE_DENSITY
	/* (2 mod 4) alignment for loop instruction */
#else
	/* (1 mod 4) alignment for loop instruction */
	.byte	0
	.byte	0
#endif
.Laligned:
#if XCHAL_HAVE_DENSITY
	_movi.n	a8, 0		// set up for the maximum loop count
#else
	_movi	a8, 0		// set up for the maximum loop count
#endif
	loop	a8, .Lz3	// loop forever (almost anyway)
	l32i	a8, a3, 0	// get word from src
	addi	a3, a3, 4	// advance src pointer
	bnone	a8, a4, .Lz0	// if byte 0 is zero
	bnone	a8, a5, .Lz1	// if byte 1 is zero
	bnone	a8, a6, .Lz2	// if byte 2 is zero
	s32i	a8, a10, 0	// store word to dst
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	l32i	a8, a10, 0
	s32i	a8, a10, 0
#endif
	bnone	a8, a7, .Lz3	// if byte 3 is zero
	addi	a10, a10, 4	// advance dst pointer

#else /* !XCHAL_HAVE_LOOPS */

1:	addi	a10, a10, 4	// advance dst pointer
.Laligned:
	l32i	a8, a3, 0	// get word from src
	addi	a3, a3, 4	// advance src pointer
	bnone	a8, a4, .Lz0	// if byte 0 is zero
	bnone	a8, a5, .Lz1	// if byte 1 is zero
	bnone	a8, a6, .Lz2	// if byte 2 is zero
	s32i	a8, a10, 0	// store word to dst
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	l32i	a8, a10, 0
	s32i	a8, a10, 0
#endif

	bany	a8, a7, 1b	// if byte 3 is zero
#endif /* !XCHAL_HAVE_LOOPS */

.Lz3:	/* Byte 3 is zero.  */
	leaf_return

.Lz0:	/* Byte 0 is zero.  */
#ifdef __XTENSA_EB__
	movi	a8, 0
#endif
	s8i	a8, a10, 0
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	leaf_return

.Lz1:	/* Byte 1 is zero.  */
#ifdef __XTENSA_EB__
        extui   a8, a8, 16, 16
#endif
	s16i	a8, a10, 0
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	leaf_return

.Lz2:	/* Byte 2 is zero.  */
#ifdef __XTENSA_EB__
        extui   a8, a8, 16, 16
#endif
	s16i	a8, a10, 0
	movi	a8, 0
	s8i	a8, a10, 2
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	leaf_return

#if 1
/* For now just use byte copy loop for the unaligned destination case.  */

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
	s8i	a8, a10, 0
	addi	a10, a10, 1
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
#if XCHAL_HAVE_LOOPS
	beqz	a8, 2f
#else
	bnez	a8, 1b
#endif
2:	leaf_return

#else /* 0 */

/* This code is not functional yet.  */

.Ldstunaligned:
	l32i	a9, a2, 0	// load word from dst
#ifdef __XTENSA_EB__
	ssa8b	a9		// rotate by dst alignment so that
	src	a9, a9, a9	// shift in loop will put back in place
	ssa8l	a9		// shift left by byte*8
#else
	ssa8l	a9		// rotate by dst alignment so that
	src	a9, a9, a9	// shift in loop will put back in place
	ssa8b	a9		// shift left by 32-byte*8
#endif

/* dst is word-aligned; src is unaligned.  */

.Ldstunalignedloop:
	l32i	a8, a3, 0	// get word from src
	/* 1-cycle interlock */
	bnone	a8, a4, .Lu0	// if byte 0 is zero
	bnone	a8, a5, .Lu1	// if byte 1 is zero
	bnone	a8, a6, .Lu2	// if byte 2 is zero
	src	a9, a8, a9	// combine last word and this word
	s32i	a9, a10, 0	// store word to dst
	bnone	a8, a7, .Lu3	// if byte 3 is nonzero, iterate
	l32i	a9, a3, 4	// get word from src
	addi	a3, a3, 8	// advance src pointer
	bnone	a9, a4, .Lu4	// if byte 0 is zero
	bnone	a9, a5, .Lu5	// if byte 1 is zero
	bnone	a9, a6, .Lu6	// if byte 2 is zero
	src	a8, a9, a8	// combine last word and this word
	s32i	a8, a10, 4	// store word to dst
	addi	a10, a10, 8	// advance dst pointer
	bany	a8, a7, .Ldstunalignedloop // if byte 3 is nonzero, iterate

	/* Byte 7 is zero.  */
.Lu7:	leaf_return

.Lu0:	/* Byte 0 is zero.  */
#ifdef __XTENSA_EB__
	movi	a8, 0
#endif
	s8i	a8, a10, 0
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	leaf_return

.Lu1:	/* Byte 1 is zero.  */
#ifdef __XTENSA_EB__
        extui   a8, a8, 16, 16
#endif
	s16i	a8, a10, 0
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	leaf_return

.Lu2:	/* Byte 2 is zero.  */
	s16i	a8, a10, 0
	movi	a8, 0
	s8i	a8, a10, 2
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	leaf_return

#endif /* 0 */
	.end schedule

	.size	strcpy, . - strcpy


	.global	esp_libc_include_strcpy_impl
	.type	esp_libc_include_strcpy_impl, @function
esp_libc_include_strcpy_impl:
	.size	esp_libc_include_strcpy_impl, . - esp_libc_include_strcpy_impl
