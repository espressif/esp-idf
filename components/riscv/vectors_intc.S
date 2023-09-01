/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "soc/soc.h"

#if ETS_INT_WDT_INUM != 24
    #error "ETS_INT_WDT_INUM expected to be 24"
#endif

/* If memory protection interrupts are meant to trigger a panic, attach them to panic handler,
 * else, attach them to the interrupt handler. */
#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
    #define MEMPROT_ISR _panic_handler
#else
    #define MEMPROT_ISR _interrupt_handler
#endif // CONFIG_ESP_SYSTEM_MEMPROT_FEATURE

/* Same goes for the assist debug interrupt */
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    #define ASTDBG_ISR _panic_handler
#else
    #define ASTDBG_ISR _interrupt_handler
#endif // CONFIG_ESP_SYSTEM_HW_STACK_GUARD


    /* Handlers defined in the `vector.S` file, common to all RISC-V targets */
    .global _interrupt_handler
    .global _panic_handler

    .section .exception_vectors_table.text

    /* This is the vector table. MTVEC points here.
     *
     * Use 4-byte instructions here. 1 instruction = 1 entry of the table.
     * The CPU jumps to MTVEC (i.e. the first entry) in case of an exception,
     * and (MTVEC & 0xfffffffc) + (mcause & 0x7fffffff) * 4, in case of an interrupt.
     *
     * Note: for our CPU, we need to place this on a 256-byte boundary, as CPU
     * only uses the 24 MSBs of the MTVEC, i.e. (MTVEC & 0xffffff00).
     */
    .balign 0x100

    /* Since each entry must take 4-byte, let's temporarily disable the compressed
     * instruction set that could potentially generate 2-byte instructions. */
    .option push
    .option norvc

    .global _vector_table
    .type _vector_table, @function
_vector_table:
    j _panic_handler            /* 0: Exception entry */
    j _interrupt_handler        /* 1: Free interrupt number */
    j _interrupt_handler        /* 2: Free interrupt number */
    j _interrupt_handler        /* 3: Free interrupt number */
    j _interrupt_handler        /* 4: Free interrupt number */
    j _interrupt_handler        /* 5: Free interrupt number */
    j _interrupt_handler        /* 6: Free interrupt number */
    j _interrupt_handler        /* 7: Free interrupt number */
    j _interrupt_handler        /* 8: Free interrupt number */
    j _interrupt_handler        /* 9: Free interrupt number */
    j _interrupt_handler        /* 10: Free interrupt number */
    j _interrupt_handler        /* 11: Free interrupt number */
    j _interrupt_handler        /* 12: Free interrupt number */
    j _interrupt_handler        /* 13: Free interrupt number */
    j _interrupt_handler        /* 14: Free interrupt number */
    j _interrupt_handler        /* 15: Free interrupt number */
    j _interrupt_handler        /* 16: Free interrupt number */
    j _interrupt_handler        /* 17: Free interrupt number */
    j _interrupt_handler        /* 18: Free interrupt number */
    j _interrupt_handler        /* 19: Free interrupt number */
    j _interrupt_handler        /* 20: Free interrupt number */
    j _interrupt_handler        /* 21: Free interrupt number */
    j _interrupt_handler        /* 22: Free interrupt number */
    j _interrupt_handler        /* 23: Free interrupt number */
    j _panic_handler            /* 24: ETS_INT_WDT_INUM panic-interrupt (soc-level panic) */
    j _panic_handler            /* 25: ETS_CACHEERR_INUM panic-interrupt (soc-level panic) */
    j MEMPROT_ISR               /* 26: ETS_MEMPROT_ERR_INUM handler (soc-level panic) */
    j ASTDBG_ISR                /* 27: ETS_ASSIST_DEBUG_INUM handler (soc-level panic) */
    j _interrupt_handler        /* 28: Free interrupt number */
    j _interrupt_handler        /* 29: Free interrupt number */
    j _interrupt_handler        /* 30: Free interrupt number */
    j _interrupt_handler        /* 31: Free interrupt number */

    .size _vector_table, .-_vector_table

    /* Re-enable the compressed instruction set it is was enabled before */
    .option pop
