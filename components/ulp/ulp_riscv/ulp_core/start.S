/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "ulp_riscv_interrupt_ops.h"

    .section .text
    .global __start

.type __start, %function
__start:
    /* setup the stack pointer */
    la sp, __stack_top

#if CONFIG_ULP_RISCV_INTERRUPT_ENABLE
    /* Enable interrupts globally */
    maskirq_insn(zero, zero)
#endif /* CONFIG_ULP_RISCV_INTERRUPT_ENABLE */

    /* Start ULP user code */
    call ulp_riscv_rescue_from_monitor
    call main
    call ulp_riscv_halt
loop:
    j loop
    .size __start, .-__start
