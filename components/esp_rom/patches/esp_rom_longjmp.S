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

/*
    Replacement of the first instructions of void longjmp (jmp_buf env, int val)
*/

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

    /* Jump back to original longjmp implementation.
        The jump target is the instrucion
    	    l32i	a0, a2, 64
        of the original code. Hence, the original code's entry instruction and windowstart modification are left
        out.
     */
    movi a0, __real_longjmp + 20
    jx a0

    .size   __wrap_longjmp, . - __wrap_longjmp
