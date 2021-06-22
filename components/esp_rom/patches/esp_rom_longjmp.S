/*
    Copyright (c) 2001-2006 by Tensilica Inc.

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
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
  This file contains a modified version of the original Xtensa longjmp implementation.
  In this modified version, setting WINDOWSTART = 1 << WINDOWBASE is done inside a critical section.
  This is necessary because after a FreeRTOS context switch in IDF, the values of WINDOWBASE and WINDOWSTART
  are not guaranteed to be the same as before the context switch.
*/

#include <xtensa/corebits.h>
#include <sdkconfig.h>

/* void longjmp (jmp_buf env, int val) */

    .align  4
    .literal_position
    .global __wrap_longjmp
    .type   __wrap_longjmp, @function
__wrap_longjmp:
    entry   sp, 16

    /* Deactivate interrupts in order to modify WINDOWBASE and WINDOWSTART. */
    rsr     a7, PS                     /* to be restored after SPILL_ALL_WINDOWS */
    movi    a5, PS_EXCM                /* PS_INTLEVEL_MASK */
    or      a5, a7, a5                 /* get the current INTLEVEL */
    wsr     a5, PS

    /* Invalidate all but the current window;
       set WindowStart to (1 << WindowBase).  */
    rsr a5, WINDOWBASE
    movi    a4, 1
    ssl a5
    sll a4, a4
    wsr a4, WINDOWSTART
    rsync

    /* Activate interrupts again after modifying WINDOWBASE and WINDOWSTART. */
    wsr     a7, PS

#if !CONFIG_IDF_TARGET_ESP32S3

    /*
        If not on S3, replacement of only the first instructions,
        then jump back to original longjmp implementation.
        The jump target is the instrucion
    	    l32i	a0, a2, 64
        of the original code. Hence, the original code's entry instruction and windowstart modification are left
        out.
     */
    movi a0, __real_longjmp + 20
    jx a0

    .size   __wrap_longjmp, . - __wrap_longjmp

#else /* CONFIG_IDF_TARGET_ESP32S3 */
    /*
       If on S3, we replace the whole function for simplicity. The placement of longjmp in ROM is ECO-dependent
       on S3.
    */

    /*
	   Return to the return address of the setjmp, using the
	   window size bits from the setjmp call so that the caller
	   will be able to find the return value that we put in a2.  */

	l32i	a0, a2, 64

	/* Copy the first 4 saved registers from jmp_buf into the save area
	   at the current sp so that the values will be restored to registers
	   when longjmp returns.  */

	addi	a7, a1, -16
	l32i	a4, a2, 0
	l32i	a5, a2, 4
	s32i	a4, a7, 0
	s32i	a5, a7, 4
	l32i	a4, a2, 8
	l32i	a5, a2, 12
	s32i	a4, a7, 8
	s32i	a5, a7, 12

	/* Copy the remaining 0-8 saved registers.  */
	extui	a7, a0, 30, 2
	blti	a7, 2, .Lendlj
	l32i	a8, a2, 52
	slli	a4, a7, 4
	sub	a6, a8, a4
	addi	a5, a2, 16
	addi	a8, a8, -16		// a8 = end of register overflow area
.Lljloop:
	l32i	a7, a5, 0
	l32i	a4, a5, 4
	s32i	a7, a6, 0
	s32i	a4, a6, 4
	l32i	a7, a5, 8
	l32i	a4, a5, 12
	s32i	a7, a6, 8
	s32i	a4, a6, 12
	addi	a5, a5, 16
	addi	a6, a6, 16
	blt	a6, a8, .Lljloop
.Lendlj:

	/* The 4 words saved from the register save area at the target's
	   sp are copied back to the target procedure's save area.  The
	   only point of this is to prevent a catastrophic failure in
	   case the contents were moved by an alloca after calling
	   setjmp.  This is a bit paranoid but it doesn't cost much.  */

	l32i	a7, a2, 4		// load the target stack pointer
	addi	a7, a7, -16		// find the destination save area
	l32i	a4, a2, 48
	l32i	a5, a2, 52
	s32i	a4, a7, 0
	s32i	a5, a7, 4
	l32i	a4, a2, 56
	l32i	a5, a2, 60
	s32i	a4, a7, 8
	s32i	a5, a7, 12

	/* Return val ? val : 1.  */
	movi	a2, 1
	movnez	a2, a3, a3

	retw
    .size   __wrap_longjmp, . - __wrap_longjmp

#endif /* CONFIG_IDF_TARGET_ESP32S3 */
