/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "ulp_riscv_register_ops.h"

#define ULP_RISCV_TIMER_INT                         (1 << 0U)   /* Internal Timer Interrupt */
#define ULP_RISCV_EBREAK_ECALL_ILLEGAL_INSN_INT     (1 << 1U)   /* EBREAK, ECALL or Illegal instruction */
#define ULP_RISCV_BUS_ERROR_INT                     (1 << 2U)   /* Bus Error (Unaligned Memory Access) */
#define ULP_RISCV_PERIPHERAL_INTERRUPT              (1 << 31U)  /* RTC Peripheral Interrupt */
#define ULP_RISCV_INTERNAL_INTERRUPT                (ULP_RISCV_TIMER_INT | ULP_RISCV_EBREAK_ECALL_ILLEGAL_INSN_INT | ULP_RISCV_BUS_ERROR_INT)

/* This is the global interrupt handler for ULP RISC-V.
 * It is called from ulp_riscv_vectors.S
 */
void __attribute__((weak)) _ulp_riscv_interrupt_handler(uint32_t q1)
{
    /* Call respective interrupt handlers based on the interrupt status in q1 */

    /* Internal Interrupts */
    if (q1 & ULP_RISCV_INTERNAL_INTERRUPT) {
        // TODO
    }

    /* External/Peripheral interrupts */
    if (q1 & ULP_RISCV_PERIPHERAL_INTERRUPT) {
        // TODO

        /* Peripheral interrupts */

        /* RTC I2C interrupt */

        /* RTC IO interrupt */
    }
}
