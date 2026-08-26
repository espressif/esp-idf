/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Helper function for the test case in test_context_save_clobber.c */

#if defined(__XTENSA__)
#include "xtensa/config/core-isa.h"
#if defined(XCHAL_HAVE_WINDOWED)

    .data
recursion_count:
    .word 0

    .text
    .global     test_context_save_clober_func
    .type       test_context_save_clober_func,@function
    .align      4

/* This function recursively calls itself via call4, making sure each frame
 * uses only 4 registers. For this reason, recursion count can not be
 * a function argument (it would have to be in a6) and is placed into .data
 * section above.
 */
test_context_save_clober_func:
    entry a1, 16

    /* load recursion count from memory */
    movi a3, recursion_count
    l32i a2, a3, 0

    /* if it is zero, initialize it to 16 (=64 physical registers / 4 registers per call) */
    bnez a2, 1f
    movi a2, 16
1:

    /* decrement the counter and write it back */
    addi a2, a2, -1
    s32i a2, a3, 0

    /* counter not zero? do a recursive call */
    beqz a2, wait
    call4 test_context_save_clober_func
    j end

wait:
    /* Counter has reached zero, and we have 16 frames on the stack.
     * Delay for a few seconds, expecting in interrupt to happen.
     */
    movi a3, 100000000
1:
    addi a3, a3, -1
    bnez a3, 1b

end:
    retw

#endif // XCHAL_HAVE_WINDOWED
#endif // __XTENSA__
