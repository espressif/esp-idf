/* $Id: //depot/dev/Foxhill/Xtensa/OS/include/xtensa/mpuasm.h#5 $ */

/*
 * Copyright (c) 2016 Cadence Design Systems, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _MPUASM_H_
#define _MPUASM_H_
#include <xtensa/config/core.h>

/*
 * Macro for writing MPU map.
 *
 * Parameters:
 * 	a_map				=>	address register containing pointer to MPU map
 * 	a_num_entries		=>	number of entries in the forementioned map
 * 	a_temp1, a_temp2.	=>  address register temporaries
 * 	a_temp3, a_temp4
 */

.macro mpu_write_map a_map, a_num_entries, a_temp1, a_temp2, a_temp3, a_temp4
#if XCHAL_HAVE_MPU
	movi	\a_temp1, 0
	wsr.cacheadrdis \a_temp1 // enable the cache in all regions
	wsr.mpuenb	\a_temp1	// disable all foreground entries

    // Clear out the unused entries.
    //
    // Currently we are clearing out all the entries because currently
    // the entries must be ordered even if they are all disabled.
    // If out of order entries were permitted when all are disabled,
    // performance could be improved by clearing XCHAL_MPU_ENTRIES - n
	// (n = number of entries) rather than XCHAL_MPU_ENTRIES - 1 entries.
	//
	movi	\a_temp2, 0
	movi	\a_temp3, XCHAL_MPU_ENTRIES - 1
	j	1f
	.align 16 // this alignment is done to ensure that
1:
	memw     // todo currently wptlb must be preceeded by a memw.  The instructions must
	// be aligned to ensure that both are in the same cache line.  These statements should be
	// properly conditionalized when that restriction is removed from the HW
	wptlb	\a_temp2, \a_temp1
	addi	\a_temp2, \a_temp2, 1
	bltu	\a_temp2, \a_temp3, 1b

        // Write the new entries.
	//
	beqz	\a_num_entries, 4f				// if no entries, skip loop
	addx8	\a_map, \a_num_entries, \a_map			// compute end of provided map
	j		3f
	.align 16
2:	memw     // todo currently wptlb must be preceeded by a memw.  The instructions must
	// be aligned to ensure that both are in the same cache line.  These statements should be
	// properly conditionalized when that restriction is removed from the HW
	wptlb	\a_temp2, \a_temp4
	addi	\a_temp3, \a_temp3, -1
	beqz	\a_num_entries, 4f		// loop until done
3:	addi	\a_map, \a_map, -8
	l32i	\a_temp2, \a_map, 4			// get at (acc.rights, memtype)
	l32i	\a_temp4, \a_map, 0			// get as (vstart, valid)
	addi	\a_num_entries, \a_num_entries, -1
	extui	\a_temp1, \a_temp2, 0, 5			// entry index portion
	xor		\a_temp2, \a_temp2, \a_temp1			// zero it
	or		\a_temp2, \a_temp2, \a_temp3			// set index = \a_temp3
	j		2b
4:
#endif
.endm

/*
 * Macro for reading MPU map
 *
 * Parameters:
 * 	a_map_ptr			=> address register pointing to memory where map is written
 * 	a_temp1, a_temp2	=> address register temporaries
 */
.macro mpu_read_map a_map_ptr, a_temp1, a_temp2
#if XCHAL_HAVE_MPU
	movi	\a_temp1, XCHAL_MPU_ENTRIES // set index to last entry + 1
	addx8	\a_map_ptr, \a_temp1, \a_map_ptr // set map ptr to last entry + 1
1:	addi	\a_temp1, \a_temp1, -1 // decrement index
	addi	\a_map_ptr, \a_map_ptr, -8 // decrement index
	rptlb0	\a_temp2, \a_temp1 // read 1/2 of entry
	s32i	\a_temp2, \a_map_ptr, 0 // write 1/2 of entry
	rptlb1	\a_temp2,	\a_temp1
	s32i	\a_temp2, \a_map_ptr, 4
	bnez	\a_temp1, 1b // loop until done
#endif
	.endm

#endif
