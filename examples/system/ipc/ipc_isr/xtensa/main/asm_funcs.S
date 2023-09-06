/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/config/system.h>
#include <xtensa/hal.h>

/* get_ps_other_cpu(void *arg)
 *
 * It should be called by the CALLX0 command from the handler of High-priority interrupt.
 * Only these registers [a2, a3, a4] can be used here.
 * Returns PS.
 */
    .section    .iram1, "ax"
    .align      4
    .global     get_ps_other_cpu
    .type       get_ps_other_cpu, @function
// Args:
// a2 - void* arg
get_ps_other_cpu:
    rsr     a3, PS
    s32i    a3, a2, 0
    ret


/* extended_ipc_isr_asm(void *arg)
 *
 * It should be called by the CALLX0 command from the handler of High-priority interrupt.
 * Only these registers [a2, a3, a4] can be used here.
 * This function receives a structure (arg) where can be saved some regs
 * to get them available here, at the end of the function we recover the saved regs.
 */
    .section    .iram1, "ax"
    .align      4
    .global     extended_ipc_isr_asm
    .type       extended_ipc_isr_asm, @function
    // Args:
    // a2 - arg_data_t* arg
extended_ipc_isr_asm:

    /* save all registers (a5-a15 -> regs[11]) */
    s32i    a5,  a2, 0
    s32i    a6,  a2, 4
    s32i    a7,  a2, 8
    s32i    a8,  a2, 12
    s32i    a9,  a2, 16
    s32i    a10, a2, 20
    s32i    a11, a2, 24
    s32i    a12, a2, 28
    s32i    a13, a2, 32
    s32i    a14, a2, 36
    s32i    a15, a2, 40

    /* do some work with a2 - a15 */
    l32i    a5, a2, 44 /* a5 <- in[0] */
    l32i    a6, a2, 48 /* a6 <- in[1] */
    l32i    a7, a2, 52 /* a7 <- in[2] */

    or      a8, a5, a6
    or      a8, a8, a7

    add     a9, a5, a6
    add     a9, a9, a7

    mov     a10, a7

    rsr     a11, PS
    s32i    a8,  a2, 56  /* a8 -> out[0] */
    s32i    a9,  a2, 60  /* a9 -> out[1] */
    s32i    a10, a2, 64 /* a10 -> out[2] */
    s32i    a11, a2, 68 /* a11 -> out[3] */

    /* restore all saved registers (regs[11] -> a5-a15) */
    l32i    a5,  a2, 0
    l32i    a6,  a2, 4
    l32i    a7,  a2, 8
    l32i    a8,  a2, 12
    l32i    a9,  a2, 16
    l32i    a10, a2, 20
    l32i    a11, a2, 24
    l32i    a12, a2, 28
    l32i    a13, a2, 32
    l32i    a14, a2, 36
    l32i    a15, a2, 40
    ret
