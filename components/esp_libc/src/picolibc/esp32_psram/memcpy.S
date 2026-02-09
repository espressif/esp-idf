/*
 * SPDX-FileCopyrightText: 2002-2008 Tensilica Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 */
/* ANSI C standard library function memcpy.

   Copyright (c) 2002-2008 Tensilica Inc.

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

/* If the Xtensa Unaligned Load Exception option is not used, this
   code can run a few cycles faster by relying on the low address bits
   being ignored.  However, if the code is then run with an Xtensa ISS
   client that checks for unaligned accesses, it will produce a lot of
   warning messages.  Set this flag to disable the use of unaligned
   accesses and keep the ISS happy.  */

/* #define UNALIGNED_ADDRESSES_CHECKED XCHAL_UNALIGNED_LOAD_EXCEPTION */
#define UNALIGNED_ADDRESSES_CHECKED 1


/* void *memcpy (void *dst, const void *src, size_t len)

   The algorithm is as follows:

   If the destination is unaligned, align it by conditionally
   copying 1- and/or 2-byte pieces.

   If the source is aligned, copy 16 bytes with a loop, and then finish up
   with 8, 4, 2, and 1-byte copies conditional on the length.

   Else (if source is unaligned), do the same, but use SRC to align the
   source data.

   This code tries to use fall-through branches for the common
   case of aligned source and destination and multiple of 4 (or 8) length.  */


/* Byte by byte copy.  */

	.text
	.begin schedule
	.align	XCHAL_INST_FETCH_WIDTH
	.literal_position
__memcpy_aux:

	/* Skip bytes to get proper alignment for three-byte loop */
.skip XCHAL_INST_FETCH_WIDTH - 3

.Lbytecopy:
#if XCHAL_HAVE_LOOPS
	loopnez	a4, 2f
#else
	beqz	a4, 2f
	add	a7, a3, a4	// a7 = end address for source
#endif
1:	l8ui	a6, a3, 0
	addi	a3, a3, 1
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	nop
	nop
	nop
#endif
	s8i	a6, a5, 0
	addi	a5, a5, 1
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
#if !XCHAL_HAVE_LOOPS
	bltu	a3, a7, 1b
#endif
2:	leaf_return


/* Destination is unaligned.  */

	.align	4
.Ldst1mod2: // dst is only byte aligned

	/* Do short copies byte-by-byte.  */
	bltui	a4, 7, .Lbytecopy

	/* Copy 1 byte.  */
	l8ui	a6, a3, 0
	addi	a3, a3, 1
	addi	a4, a4, -1
	s8i	a6, a5, 0
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	addi	a5, a5, 1

	/* Return to main algorithm if dst is now aligned.  */
	bbci.l	a5, 1, .Ldstaligned

.Ldst2mod4: // dst has 16-bit alignment

	/* Do short copies byte-by-byte.  */
	bltui	a4, 6, .Lbytecopy

	/* Copy 2 bytes.  */
	l8ui	a6, a3, 0
	l8ui	a7, a3, 1
	addi	a3, a3, 2
	addi	a4, a4, -2
	s8i	a6, a5, 0
	s8i	a7, a5, 1
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	addi	a5, a5, 2

	/* dst is now aligned; return to main algorithm.  */
	j	.Ldstaligned


	.align	4
	.global	memcpy
	.type	memcpy, @function
memcpy:
	leaf_entry sp, 16
	/* a2 = dst, a3 = src, a4 = len */

	mov	a5, a2		// copy dst so that a2 is return value
	bbsi.l	a2, 0, .Ldst1mod2
	bbsi.l	a2, 1, .Ldst2mod4
.Ldstaligned:

	/* Get number of loop iterations with 16B per iteration.  */
	srli	a7, a4, 4

	/* Check if source is aligned.  */
	slli 	a8, a3, 30
	bnez	a8, .Lsrcunaligned

	/* Destination and source are word-aligned, use word copy.  */
#if XCHAL_HAVE_LOOPS
	loopnez	a7, 2f
#else
	beqz	a7, 2f
	slli	a8, a7, 4
	add	a8, a8, a3	// a8 = end of last 16B source chunk
#endif

#if XTENSA_ESP32_PSRAM_CACHE_FIX

1:	l32i	a6, a3, 0
	l32i	a7, a3, 4
	s32i	a6, a5, 0
	s32i	a7, a5, 4
	memw
	l32i	a6, a3, 8
	l32i	a7, a3, 12
	s32i	a6, a5, 8
	s32i	a7, a5, 12
	memw

	addi	a3, a3, 16
	addi	a5, a5, 16

#else

1:	l32i	a6, a3, 0
	l32i	a7, a3, 4
	s32i	a6, a5, 0
	l32i	a6, a3, 8
	s32i	a7, a5, 4
	l32i	a7, a3, 12
	s32i	a6, a5, 8
	addi	a3, a3, 16
	s32i	a7, a5, 12
	addi	a5, a5, 16

#endif


#if !XCHAL_HAVE_LOOPS
	bltu	a3, a8, 1b
#endif

	/* Copy any leftover pieces smaller than 16B.  */
2:	bbci.l	a4, 3, 3f

	/* Copy 8 bytes.  */
	l32i	a6, a3, 0
	l32i	a7, a3, 4
	addi	a3, a3, 8
	s32i	a6, a5, 0
	s32i	a7, a5, 4
	addi	a5, a5, 8

3:	bbsi.l	a4, 2, 4f
	bbsi.l	a4, 1, 5f
	bbsi.l	a4, 0, 6f
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	leaf_return

	.align 4
	/* Copy 4 bytes.  */
4:	l32i	a6, a3, 0
	addi	a3, a3, 4
	s32i	a6, a5, 0
	addi	a5, a5, 4
	bbsi.l	a4, 1, 5f
	bbsi.l	a4, 0, 6f
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	leaf_return

	/* Copy 2 bytes.  */
5:	l16ui	a6, a3, 0
	addi	a3, a3, 2
	s16i	a6, a5, 0
	addi	a5, a5, 2
	bbsi.l	a4, 0, 6f
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	leaf_return

	/* Copy 1 byte.  */
6:	l8ui	a6, a3, 0
	s8i	a6, a5, 0

.Ldone:
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	leaf_return


/* Destination is aligned; source is unaligned.  */

	.align	4
.Lsrcunaligned:
	/* Avoid loading anything for zero-length copies.  */
	beqz	a4, .Ldone

	/* Copy 16 bytes per iteration for word-aligned dst and
	   unaligned src.  */
	ssa8	a3		// set shift amount from byte offset
#if UNALIGNED_ADDRESSES_CHECKED
	srli    a11, a8, 30     // save unalignment offset for below
	sub	a3, a3, a11	// align a3
#endif
	l32i	a6, a3, 0	// load first word
#if XCHAL_HAVE_LOOPS
	loopnez	a7, 2f
#else
	beqz	a7, 2f
	slli	a10, a7, 4
	add	a10, a10, a3	// a10 = end of last 16B source chunk
#endif
1:	l32i	a7, a3, 4
	l32i	a8, a3, 8
	src_b	a6, a6, a7
	s32i	a6, a5, 0
	l32i	a9, a3, 12
	src_b	a7, a7, a8
	s32i	a7, a5, 4
	l32i	a6, a3, 16
	src_b	a8, a8, a9
	s32i	a8, a5, 8
	addi	a3, a3, 16
	src_b	a9, a9, a6
	s32i	a9, a5, 12
	addi	a5, a5, 16
#if !XCHAL_HAVE_LOOPS
	bltu	a3, a10, 1b
#endif

2:	bbci.l	a4, 3, 3f

	/* Copy 8 bytes.  */
	l32i	a7, a3, 4
	l32i	a8, a3, 8
	src_b	a6, a6, a7
	s32i	a6, a5, 0
	addi	a3, a3, 8
	src_b	a7, a7, a8
	s32i	a7, a5, 4
	addi	a5, a5, 8
	mov	a6, a8

3:	bbci.l	a4, 2, 4f

	/* Copy 4 bytes.  */
	l32i	a7, a3, 4
	addi	a3, a3, 4
	src_b	a6, a6, a7
	s32i	a6, a5, 0
	addi	a5, a5, 4
	mov	a6, a7
4:
#if UNALIGNED_ADDRESSES_CHECKED
	add	a3, a3, a11	// readjust a3 with correct misalignment
#endif
	bbsi.l	a4, 1, 5f
	bbsi.l	a4, 0, 6f
	leaf_return

	/* Copy 2 bytes.  */
5:	l8ui	a6, a3, 0
	l8ui	a7, a3, 1
	addi	a3, a3, 2
	s8i	a6, a5, 0
	s8i	a7, a5, 1
	addi	a5, a5, 2
	bbsi.l	a4, 0, 6f
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	leaf_return

	/* Copy 1 byte.  */
6:	l8ui	a6, a3, 0
	s8i	a6, a5, 0
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	leaf_return

	.end schedule

	.size	memcpy, . - memcpy


	.global	esp_libc_include_memcpy_impl
	.type	esp_libc_include_memcpy_impl, @function
esp_libc_include_memcpy_impl:
	.size	esp_libc_include_memcpy_impl, . - esp_libc_include_memcpy_impl
