/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#define DEBUG_OUTPUT 0
#define LOOP_COUNT 3

#if DEBUG_OUTPUT
.section .data
loop_counter_str: .string "loop counter is %d\n"
#endif

.section .text
.type test_xesppie_loops, @function
.global test_xesppie_loops

/* This workaround is intended to help the MI2 interpreter find labels in
 * assembly code. */
.globl test_loop_start
.type test_loop_start, @function

test_xesppie_loops:
    addi    sp, sp, -16
    sw      ra, 12(sp)
#if DEBUG_OUTPUT
    sw      s0, 8(sp)

    la      s0, loop_counter_str
#endif
    li      t0, LOOP_COUNT

test_loop_start:
    esp.lp.setup 0, t0, test_loop_end
    csrr    a1, 0x7C8
#if DEBUG_OUTPUT
    mv      a0, s0
    call    ets_printf
#endif
test_loop_end:
    nop

#if DEBUG_OUTPUT
    lw      s0, 8(sp)
#endif
    lw      ra, 12(sp)
    addi    sp, sp, 16
    ret
.size  test_xesppie_loops, .-test_xesppie_loops
