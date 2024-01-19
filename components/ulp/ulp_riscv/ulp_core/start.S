/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ulp_riscv_interrupt_ops.h"

    .section .text
    .global __start

.type __start, %function
__start:
    /* setup the stack pointer */
    la sp, __stack_top

    /* Enable interrupts globally */
    maskirq_insn(zero, zero)

    /* Start ULP user code */
    call ulp_riscv_rescue_from_monitor
    call main
    call ulp_riscv_halt
loop:
    j loop
    .size __start, .-__start
