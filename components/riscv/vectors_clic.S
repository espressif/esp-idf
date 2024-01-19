/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "soc/soc.h"

/* If memory protection interrupts are meant to trigger a panic, attach them to panic handler,
 * else, attach them to the interrupt handler. */
#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
    #define MEMPROT_ISR _panic_handler
#else
    #define MEMPROT_ISR _interrupt_handler
#endif // CONFIG_ESP_SYSTEM_MEMPROT_FEATURE

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    #define ASTDBG_ISR _panic_handler
#else
    #define ASTDBG_ISR _interrupt_handler
#endif // CONFIG_ESP_SYSTEM_HW_STACK_GUARD

#if CONFIG_ESP_IPC_ISR_ENABLE
    #define IPC_ISR_HANDLER esp_ipc_isr_handler
#else
    #define IPC_ISR_HANDLER _interrupt_handler
#endif // CONFIG_ESP_IPC_ISR_ENABLE

/* The system interrupts are not used for now, so trigger a panic every time one occurs. */
#define _system_int_handler _panic_handler

    /* Handlers defined in the `vector.S` file, common to all RISC-V targets */
    .global _interrupt_handler
    .global _panic_handler

    .section .exception_vectors_table.text

    /* Prevent the compiler from generating 2-byte instruction in the vector tables */
    .option push
    .option norvc

    /**
     * Non-hardware vectored interrupt entry. MTVEC CSR points here.
     *
     * On targets that use CLIC as their interrupt controller, when an exception occurs, the CPU
     * jumps to the address stored in MTVEC[31:6] << 6. The CPU will also jump to this location
     * if an interrupt is configured as non-vectored (CLIC_INT_ATTR.shv = 0).
     *
     * Because of the left-shift `<< 6`, this entry must be aligned on 64.
     */
    .global _vector_table
    .type _vector_table, @function
    .balign 0x40
_vector_table:
    j _panic_handler
    .size _vector_table, .-_vector_table


    /**
     * Vectored interrupt table. MTVT CSR points here.
     *
     * If an interrupt occurs and is configured as (hardware) vectored, the CPU will jump to
     * MTVT[31:0] + 4 * interrupt_id
     *
     * In the case of the ESP32P4, the interrupt matrix, between the CPU interrupt lines
     * and the peripherals, offers 32 lines. As such, the interrupt_id between 0 and 31.
     *
     * Since the interrupts are initialized as vectored on CPU start, we can manage the special
     * interrupts ETS_T1_WDT_INUM, ETS_CACHEERR_INUM and ETS_MEMPROT_ERR_INUM here.
     */
    .balign 0x40
    .global _mtvt_table
    .type _mtvt_table, @function
_mtvt_table:
    .word _system_int_handler       /* 0: System interrupt number. Exceptions are non-vectored, won't load this. */
    .word _system_int_handler       /* 1: System interrupt number */
    .word _system_int_handler       /* 2: System interrupt number */
    .word _system_int_handler       /* 3: System interrupt number */
    .word _system_int_handler       /* 4: System interrupt number */
    .word _system_int_handler       /* 5: System interrupt number */
    .word _system_int_handler       /* 6: System interrupt number */
    .word _system_int_handler       /* 7: System interrupt number */
    .word _system_int_handler       /* 8: System interrupt number */
    .word _system_int_handler       /* 9: System interrupt number */
    .word _system_int_handler       /* 10: System interrupt number */
    .word _system_int_handler       /* 11: System interrupt number */
    .word _system_int_handler       /* 12: System interrupt number */
    .word _system_int_handler       /* 13: System interrupt number */
    .word _system_int_handler       /* 14: System interrupt number */
    .word _system_int_handler       /* 15: System interrupt number */
    .word _interrupt_handler        /* 16: Free interrupt number */
    .word _interrupt_handler        /* 17: Free interrupt number */
    .word _interrupt_handler        /* 18: Free interrupt number */
    .word _interrupt_handler        /* 19: Free interrupt number */
    .word _interrupt_handler        /* 20: Free interrupt number */
    .word _interrupt_handler        /* 21: Free interrupt number */
    .word _interrupt_handler        /* 22: Free interrupt number */
    .word _interrupt_handler        /* 23: Free interrupt number */
    .word _interrupt_handler        /* 24: Free interrupt number */
    .word _interrupt_handler        /* 25: Free interrupt number */
    .word _interrupt_handler        /* 26: Free interrupt number */
    .word _interrupt_handler        /* 27: Free interrupt number */
    .word _interrupt_handler        /* 28: Free interrupt number */
    .word _interrupt_handler        /* 29: Free interrupt number */
    .word _interrupt_handler        /* 30: Free interrupt number */
    .word _interrupt_handler        /* 31: Free interrupt number */
    .word _interrupt_handler        /* 32: Free interrupt number */
    .word _interrupt_handler        /* 33: Free interrupt number */
    .word _interrupt_handler        /* 34: Free interrupt number */
    .word _interrupt_handler        /* 35: Free interrupt number */
    .word _interrupt_handler        /* 36: Free interrupt number */
    .word _interrupt_handler        /* 37: Free interrupt number */
    .word _interrupt_handler        /* 38: Free interrupt number */
    .word _interrupt_handler        /* 39: Free interrupt number */
    .word _panic_handler            /* 40: ETS_INT_WDT_INUM (+16) panic-interrupt (soc-level panic) */
    .word _panic_handler            /* 41: ETS_CACHEERR_INUM (+16) panic-interrupt (soc-level panic) */
    .word MEMPROT_ISR               /* 42: ETS_MEMPROT_ERR_INUM (+16) handler (soc-level panic) */
    .word ASTDBG_ISR                /* 43: ETS_ASSIST_DEBUG_INUM (+16) handler (soc-level panic) */
    .word IPC_ISR_HANDLER           /* 44: ETS_IPC_ISR_INUM (+16) handler*/
    .word _interrupt_handler        /* 45: Free interrupt number */
    .word _interrupt_handler        /* 46: Free interrupt number */
    .word _interrupt_handler        /* 47: Free interrupt number */

    .size _mtvt_table, .-_mtvt_table
    .option pop
