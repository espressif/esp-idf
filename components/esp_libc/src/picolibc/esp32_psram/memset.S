/*
 * SPDX-FileCopyrightText: 2001-2008 Tensilica Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 */
/* ANSI C standard library function memset.

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

/* void *memset (void *dst, int c, size_t length)

   The algorithm is as follows:

   Create a word with c in all byte positions.

   If the destination is aligned, set 16B chunks with a loop, and then
   finish up with 8B, 4B, 2B, and 1B stores conditional on the length.

   If the destination is unaligned, align it by conditionally
   setting 1B and/or 2B and then go to aligned case.

   This code tries to use fall-through branches for the common
   case of an aligned destination (except for the branches to
   the alignment labels).  */


/* Byte-by-byte set.  */

	.text
	.begin schedule
	.align	XCHAL_INST_FETCH_WIDTH
	.literal_position
__memset_aux:

	/* Skip bytes to get proper alignment for three-byte loop */
.skip XCHAL_INST_FETCH_WIDTH - 3

.Lbyteset:
#if XCHAL_HAVE_LOOPS
	loopnez	a4, 2f
#else
	beqz	a4, 2f
	add	a6, a5, a4	// a6 = ending address
#endif
1:	s8i	a3, a5, 0
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
	addi	a5, a5, 1
#if !XCHAL_HAVE_LOOPS
	bltu	a5, a6, 1b
#endif
2:	leaf_return


/* Destination is unaligned.  */

	.align	4

.Ldst1mod2: // dst is only byte aligned

	/* Do short sizes byte-by-byte.  */
	bltui	a4, 8, .Lbyteset

	/* Set 1 byte.  */
	s8i	a3, a5, 0
	addi	a5, a5, 1
	addi	a4, a4, -1
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif

	/* Now retest if dst is aligned.  */
	_bbci.l	a5, 1, .Ldstaligned

.Ldst2mod4: // dst has 16-bit alignment

	/* Do short sizes byte-by-byte.  */
	bltui	a4, 8, .Lbyteset

	/* Set 2 bytes.  */
	s16i	a3, a5, 0
	addi	a5, a5, 2
	addi	a4, a4, -2
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif

	/* dst is now aligned; return to main algorithm */
	j	.Ldstaligned


	.align	4
	.global	memset
	.type	memset, @function
memset:
	leaf_entry sp, 16
	/* a2 = dst, a3 = c, a4 = length */

	/* Duplicate character into all bytes of word.  */
	extui	a3, a3, 0, 8
	slli	a7, a3, 8
	or	a3, a3, a7
	slli	a7, a3, 16
	or	a3, a3, a7

	mov	a5, a2		// copy dst so that a2 is return value

	/* Check if dst is unaligned.  */
	_bbsi.l	a2, 0, .Ldst1mod2
	_bbsi.l	a2, 1, .Ldst2mod4
.Ldstaligned:

	/* Get number of loop iterations with 16B per iteration.  */
	srli	a7, a4, 4

#if XTENSA_ESP32_PSRAM_CACHE_FIX
	//do not do this if we have less than one iteration to do
	beqz	a7, 2f
	//this seems to work to prefetch the cache line
	s32i	a3, a5, 0
	nop
#endif

	/* Destination is word-aligned.  */
#if XCHAL_HAVE_LOOPS
	loopnez	a7, 2f
#else
	beqz	a7, 2f
	slli	a6, a7, 4
	add	a6, a6, a5	// a6 = end of last 16B chunk
#endif
	/* Set 16 bytes per iteration.  */
1:	s32i	a3, a5, 0
	s32i	a3, a5, 4
	s32i	a3, a5, 8
	s32i	a3, a5, 12
	addi	a5, a5, 16
#if !XCHAL_HAVE_LOOPS
	bltu	a5, a6, 1b
#endif

	/* Set any leftover pieces smaller than 16B.  */
2:	bbci.l	a4, 3, 3f

	/* Set 8 bytes.  */
	s32i	a3, a5, 0
	s32i	a3, a5, 4
	addi	a5, a5, 8

3:	bbci.l	a4, 2, 4f

	/* Set 4 bytes.  */
	s32i	a3, a5, 0
	addi	a5, a5, 4

4:	bbci.l	a4, 1, 5f

	/* Set 2 bytes.  */
	s16i	a3, a5, 0
	addi	a5, a5, 2
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif

5:	bbci.l	a4, 0, 6f

	/* Set 1 byte.  */
	s8i	a3, a5, 0
#if XTENSA_ESP32_PSRAM_CACHE_FIX
	memw
#endif
6:	leaf_return

	.end schedule

	.size	memset, . - memset


	.global	esp_libc_include_memset_impl
	.type	esp_libc_include_memset_impl, @function
esp_libc_include_memset_impl:
	.size	esp_libc_include_memset_impl, . - esp_libc_include_memset_impl
