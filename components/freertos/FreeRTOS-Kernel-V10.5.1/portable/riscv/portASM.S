/*
 * FreeRTOS Kernel V10.5.1 (ESP-IDF SMP modified)
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-FileCopyrightText: 2021 Amazon.com, Inc. or its affiliates
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/*
 * The FreeRTOS kernel's RISC-V port is split between the the code that is
 * common across all currently supported RISC-V chips (implementations of the
 * RISC-V ISA), and code which tailors the port to a specific RISC-V chip:
 *
 * + The code that is common to all RISC-V chips is implemented in
 *   FreeRTOS\Source\portable\GCC\RISC-V-RV32\portASM.S.  There is only one
 *   portASM.S file because the same file is used no matter which RISC-V chip is
 *   in use.
 *
 * + The code that tailors the kernel's RISC-V port to a specific RISC-V
 *   chip is implemented in freertos_risc_v_chip_specific_extensions.h.  There
 *   is one freertos_risc_v_chip_specific_extensions.h that can be used with any
 *   RISC-V chip that both includes a standard CLINT and does not add to the
 *   base set of RISC-V registers.  There are additional
 *   freertos_risc_v_chip_specific_extensions.h files for RISC-V implementations
 *   that do not include a standard CLINT or do add to the base set of RISC-V
 *   registers.
 *
 * CARE MUST BE TAKEN TO INCLDUE THE CORRECT
 * freertos_risc_v_chip_specific_extensions.h HEADER FILE FOR THE CHIP
 * IN USE.  To include the correct freertos_risc_v_chip_specific_extensions.h
 * header file ensure the path to the correct header file is in the assembler's
 * include path.
 *
 * This freertos_risc_v_chip_specific_extensions.h is for use on RISC-V chips
 * that include a standard CLINT and do not add to the base set of RISC-V
 * registers.
 *
 */

#include "portContext.h"

/* Check the freertos_risc_v_chip_specific_extensions.h and/or command line
definitions. */
#if defined( portasmHAS_CLINT ) && defined( portasmHAS_MTIME )
    #error The portasmHAS_CLINT constant has been deprecated.  Please replace it with portasmHAS_MTIME.  portasmHAS_CLINT and portasmHAS_MTIME cannot both be defined at once.  See https://www.FreeRTOS.org/Using-FreeRTOS-on-RISC-V.html
#endif

#ifdef portasmHAS_CLINT
    #warning The portasmHAS_CLINT constant has been deprecated.  Please replace it with portasmHAS_MTIME and portasmHAS_SIFIVE_CLINT.  For now portasmHAS_MTIME and portasmHAS_SIFIVE_CLINT are derived from portasmHAS_CLINT.  See https://www.FreeRTOS.org/Using-FreeRTOS-on-RISC-V.html
    #define portasmHAS_MTIME portasmHAS_CLINT
    #define portasmHAS_SIFIVE_CLINT portasmHAS_CLINT
#endif

#ifndef portasmHAS_MTIME
    #error freertos_risc_v_chip_specific_extensions.h must define portasmHAS_MTIME to either 1 (MTIME clock present) or 0 (MTIME clock not present).  See https://www.FreeRTOS.org/Using-FreeRTOS-on-RISC-V.html
#endif

#ifndef portasmHAS_SIFIVE_CLINT
    #define portasmHAS_SIFIVE_CLINT 0
#endif

.global xPortStartFirstTask
.global pxPortInitialiseStack
.global freertos_risc_v_trap_handler
.global freertos_risc_v_exception_handler
.global freertos_risc_v_interrupt_handler
.global freertos_risc_v_mtimer_interrupt_handler

.extern vTaskSwitchContext
.extern xTaskIncrementTick
.extern pullMachineTimerCompareRegister
.extern pullNextTime
.extern uxTimerIncrementsForOneTick /* size_t type so 32-bit on 32-bit core and 64-bits on 64-bit core. */
.extern xTaskReturnAddress

.weak freertos_risc_v_application_exception_handler
.weak freertos_risc_v_application_interrupt_handler
/*-----------------------------------------------------------*/

.macro portUPDATE_MTIMER_COMPARE_REGISTER
    load_x a0, pullMachineTimerCompareRegister  /* Load address of compare register into a0. */
    load_x a1, pullNextTime                     /* Load the address of ullNextTime into a1. */

    #if( __riscv_xlen == 32 )

        /* Update the 64-bit mtimer compare match value in two 32-bit writes. */
        li a4, -1
        lw a2, 0(a1)                /* Load the low word of ullNextTime into a2. */
        lw a3, 4(a1)                /* Load the high word of ullNextTime into a3. */
        sw a4, 0(a0)                /* Low word no smaller than old value to start with - will be overwritten below. */
        sw a3, 4(a0)                /* Store high word of ullNextTime into compare register.  No smaller than new value. */
        sw a2, 0(a0)                /* Store low word of ullNextTime into compare register. */
        lw t0, uxTimerIncrementsForOneTick  /* Load the value of ullTimerIncrementForOneTick into t0 (could this be optimized by storing in an array next to pullNextTime?). */
        add a4, t0, a2              /* Add the low word of ullNextTime to the timer increments for one tick (assumes timer increment for one tick fits in 32-bits). */
        sltu t1, a4, a2             /* See if the sum of low words overflowed (what about the zero case?). */
        add t2, a3, t1              /* Add overflow to high word of ullNextTime. */
        sw a4, 0(a1)                /* Store new low word of ullNextTime. */
        sw t2, 4(a1)                /* Store new high word of ullNextTime. */

    #endif /* __riscv_xlen == 32 */

    #if( __riscv_xlen == 64 )

        /* Update the 64-bit mtimer compare match value. */
        ld t2, 0(a1)                /* Load ullNextTime into t2. */
        sd t2, 0(a0)                /* Store ullNextTime into compare register. */
        ld t0, uxTimerIncrementsForOneTick  /* Load the value of ullTimerIncrementForOneTick into t0 (could this be optimized by storing in an array next to pullNextTime?). */
        add t4, t0, t2              /* Add ullNextTime to the timer increments for one tick. */
        sd t4, 0(a1)                /* Store ullNextTime. */

    #endif /* __riscv_xlen == 64 */
    .endm
/*-----------------------------------------------------------*/

/*
 * Unlike other ports pxPortInitialiseStack() is written in assembly code as it
 * needs access to the portasmADDITIONAL_CONTEXT_SIZE constant.  The prototype
 * for the function is as per the other ports:
 * StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters );
 *
 * As per the standard RISC-V ABI pxTopcOfStack is passed in in a0, pxCode in
 * a1, and pvParameters in a2.  The new top of stack is passed out in a0.
 *
 * RISC-V maps registers to ABI names as follows (X1 to X31 integer registers
 * for the 'I' profile, X1 to X15 for the 'E' profile, currently I assumed).
 *
 * Register      ABI Name    Description                       Saver
 * x0            zero        Hard-wired zero                   -
 * x1            ra          Return address                    Caller
 * x2            sp          Stack pointer                     Callee
 * x3            gp          Global pointer                    -
 * x4            tp          Thread pointer                    -
 * x5-7          t0-2        Temporaries                       Caller
 * x8            s0/fp       Saved register/Frame pointer      Callee
 * x9            s1          Saved register                    Callee
 * x10-11        a0-1        Function Arguments/return values  Caller
 * x12-17        a2-7        Function arguments                Caller
 * x18-27        s2-11       Saved registers                   Callee
 * x28-31        t3-6        Temporaries                       Caller
 *
 * The RISC-V context is saved t FreeRTOS tasks in the following stack frame,
 * where the global and thread pointers are currently assumed to be constant so
 * are not saved:
 *
 * mstatus
 * xCriticalNesting
 * x31
 * x30
 * x29
 * x28
 * x27
 * x26
 * x25
 * x24
 * x23
 * x22
 * x21
 * x20
 * x19
 * x18
 * x17
 * x16
 * x15
 * x14
 * x13
 * x12
 * x11
 * pvParameters
 * x9
 * x8
 * x7
 * x6
 * x5
 * portTASK_RETURN_ADDRESS
 * [chip specific registers go here]
 * pxCode
 */
pxPortInitialiseStack:
    csrr t0, mstatus                    /* Obtain current mstatus value. */
    andi t0, t0, ~0x8                   /* Ensure interrupts are disabled when the stack is restored within an ISR.  Required when a task is created after the schedulre has been started, otherwise interrupts would be disabled anyway. */
    addi t1, x0, 0x188                  /* Generate the value 0x1880, which are the MPIE and MPP bits to set in mstatus. */
    slli t1, t1, 4
    or t0, t0, t1                       /* Set MPIE and MPP bits in mstatus value. */

    addi a0, a0, -portWORD_SIZE
    store_x t0, 0(a0)                   /* mstatus onto the stack. */
    addi a0, a0, -portWORD_SIZE         /* Space for critical nesting count. */
    store_x x0, 0(a0)                   /* Critical nesting count starts at 0 for every task. */

#ifdef __riscv_32e
    addi a0, a0, -(6 * portWORD_SIZE)   /* Space for registers x11-x15. */
#else
    addi a0, a0, -(22 * portWORD_SIZE)  /* Space for registers x11-x31. */
#endif
    store_x a2, 0(a0)                   /* Task parameters (pvParameters parameter) goes into register X10/a0 on the stack. */
    addi a0, a0, -(6 * portWORD_SIZE)   /* Space for registers x5-x9. */
    load_x t0, xTaskReturnAddress
    store_x t0, 0(a0)                   /* Return address onto the stack. */
    addi t0, x0, portasmADDITIONAL_CONTEXT_SIZE /* The number of chip specific additional registers. */
chip_specific_stack_frame:              /* First add any chip specific registers to the stack frame being created. */
    beq t0, x0, 1f                      /* No more chip specific registers to save. */
    addi a0, a0, -portWORD_SIZE         /* Make space for chip specific register. */
    store_x x0, 0(a0)                   /* Give the chip specific register an initial value of zero. */
    addi t0, t0, -1                     /* Decrement the count of chip specific registers remaining. */
    j chip_specific_stack_frame         /* Until no more chip specific registers. */
1:
    addi a0, a0, -portWORD_SIZE
    store_x a1, 0(a0)                   /* mret value (pxCode parameter) onto the stack. */
    ret
/*-----------------------------------------------------------*/

xPortStartFirstTask:
    load_x  sp, pxCurrentTCB            /* Load pxCurrentTCB. */
    load_x  sp, 0( sp )                 /* Read sp from first TCB member. */

    load_x  x1, 0( sp ) /* Note for starting the scheduler the exception return address is used as the function return address. */

    portasmRESTORE_ADDITIONAL_REGISTERS /* Defined in freertos_risc_v_chip_specific_extensions.h to restore any registers unique to the RISC-V implementation. */

    load_x  x7, 4 * portWORD_SIZE( sp )     /* t2 */
    load_x  x8, 5 * portWORD_SIZE( sp )     /* s0/fp */
    load_x  x9, 6 * portWORD_SIZE( sp )     /* s1 */
    load_x  x10, 7 * portWORD_SIZE( sp )    /* a0 */
    load_x  x11, 8 * portWORD_SIZE( sp )    /* a1 */
    load_x  x12, 9 * portWORD_SIZE( sp )    /* a2 */
    load_x  x13, 10 * portWORD_SIZE( sp )   /* a3 */
    load_x  x14, 11 * portWORD_SIZE( sp )   /* a4 */
    load_x  x15, 12 * portWORD_SIZE( sp )   /* a5 */
#ifndef __riscv_32e
    load_x  x16, 13 * portWORD_SIZE( sp )   /* a6 */
    load_x  x17, 14 * portWORD_SIZE( sp )   /* a7 */
    load_x  x18, 15 * portWORD_SIZE( sp )   /* s2 */
    load_x  x19, 16 * portWORD_SIZE( sp )   /* s3 */
    load_x  x20, 17 * portWORD_SIZE( sp )   /* s4 */
    load_x  x21, 18 * portWORD_SIZE( sp )   /* s5 */
    load_x  x22, 19 * portWORD_SIZE( sp )   /* s6 */
    load_x  x23, 20 * portWORD_SIZE( sp )   /* s7 */
    load_x  x24, 21 * portWORD_SIZE( sp )   /* s8 */
    load_x  x25, 22 * portWORD_SIZE( sp )   /* s9 */
    load_x  x26, 23 * portWORD_SIZE( sp )   /* s10 */
    load_x  x27, 24 * portWORD_SIZE( sp )   /* s11 */
    load_x  x28, 25 * portWORD_SIZE( sp )   /* t3 */
    load_x  x29, 26 * portWORD_SIZE( sp )   /* t4 */
    load_x  x30, 27 * portWORD_SIZE( sp )   /* t5 */
    load_x  x31, 28 * portWORD_SIZE( sp )   /* t6 */
#endif

    load_x  x5, portCRITICAL_NESTING_OFFSET * portWORD_SIZE( sp )    /* Obtain xCriticalNesting value for this task from task's stack. */
    load_x  x6, pxCriticalNesting           /* Load the address of xCriticalNesting into x6. */
    store_x x5, 0( x6 )                     /* Restore the critical nesting value for this task. */

    load_x  x5, portMSTATUS_OFFSET * portWORD_SIZE( sp )    /* Initial mstatus into x5 (t0). */
    addi    x5, x5, 0x08                    /* Set MIE bit so the first task starts with interrupts enabled - required as returns with ret not eret. */
    csrrw   x0, mstatus, x5                 /* Interrupts enabled from here! */

    load_x  x5, 2 * portWORD_SIZE( sp )     /* Initial x5 (t0) value. */
    load_x  x6, 3 * portWORD_SIZE( sp )     /* Initial x6 (t1) value. */

    addi    sp, sp, portCONTEXT_SIZE
    ret
/*-----------------------------------------------------------*/

freertos_risc_v_application_exception_handler:
    csrr t0, mcause     /* For viewing in the debugger only. */
    csrr t1, mepc       /* For viewing in the debugger only */
    csrr t2, mstatus    /* For viewing in the debugger only */
    j .
/*-----------------------------------------------------------*/

freertos_risc_v_application_interrupt_handler:
    csrr t0, mcause     /* For viewing in the debugger only. */
    csrr t1, mepc       /* For viewing in the debugger only */
    csrr t2, mstatus    /* For viewing in the debugger only */
    j .
/*-----------------------------------------------------------*/

.section .text.freertos_risc_v_exception_handler
freertos_risc_v_exception_handler:
    portcontextSAVE_EXCEPTION_CONTEXT
    /* a0 now contains mcause. */
    li t0, 11                           /* 11 == environment call. */
    bne a0, t0, other_exception         /* Not an M environment call, so some other exception. */
    call vTaskSwitchContext
    portcontextRESTORE_CONTEXT

other_exception:
    call freertos_risc_v_application_exception_handler
    portcontextRESTORE_CONTEXT
/*-----------------------------------------------------------*/

.section .text.freertos_risc_v_interrupt_handler
freertos_risc_v_interrupt_handler:
    portcontextSAVE_INTERRUPT_CONTEXT
    call freertos_risc_v_application_interrupt_handler
    portcontextRESTORE_CONTEXT
/*-----------------------------------------------------------*/

.section .text.freertos_risc_v_mtimer_interrupt_handler
freertos_risc_v_mtimer_interrupt_handler:
    portcontextSAVE_INTERRUPT_CONTEXT
    portUPDATE_MTIMER_COMPARE_REGISTER
    call xTaskIncrementTick
    beqz a0, exit_without_context_switch    /* Don't switch context if incrementing tick didn't unblock a task. */
    call vTaskSwitchContext
exit_without_context_switch:
    portcontextRESTORE_CONTEXT
/*-----------------------------------------------------------*/

.section .text.freertos_risc_v_trap_handler
.align 8
freertos_risc_v_trap_handler:
    portcontextSAVE_CONTEXT_INTERNAL

    csrr a0, mcause
    csrr a1, mepc

    bge a0, x0, synchronous_exception

asynchronous_interrupt:
    store_x a1, 0( sp )                 /* Asynchronous interrupt so save unmodified exception return address. */
    load_x sp, xISRStackTop             /* Switch to ISR stack. */
    j handle_interrupt

synchronous_exception:
    addi a1, a1, 4                      /* Synchronous so update exception return address to the instruction after the instruction that generated the exeption. */
    store_x a1, 0( sp )                 /* Save updated exception return address. */
    load_x sp, xISRStackTop             /* Switch to ISR stack. */
    j handle_exception

handle_interrupt:
#if( portasmHAS_MTIME != 0 )

    test_if_mtimer:                     /* If there is a CLINT then the mtimer is used to generate the tick interrupt. */
        addi t0, x0, 1
        slli t0, t0, __riscv_xlen - 1   /* LSB is already set, shift into MSB.  Shift 31 on 32-bit or 63 on 64-bit cores. */
        addi t1, t0, 7                  /* 0x8000[]0007 == machine timer interrupt. */
        bne a0, t1, application_interrupt_handler

        portUPDATE_MTIMER_COMPARE_REGISTER
        call xTaskIncrementTick
        beqz a0, processed_source       /* Don't switch context if incrementing tick didn't unblock a task. */
        call vTaskSwitchContext
        j processed_source

#endif /* portasmHAS_MTIME */

application_interrupt_handler:
    call freertos_risc_v_application_interrupt_handler
    j processed_source

handle_exception:
    /* a0 contains mcause. */
    li t0, 11                                   /* 11 == environment call. */
    bne a0, t0, application_exception_handler   /* Not an M environment call, so some other exception. */
    call vTaskSwitchContext
    j processed_source

application_exception_handler:
    call freertos_risc_v_application_exception_handler
    j processed_source                  /* No other exceptions handled yet. */

processed_source:
    portcontextRESTORE_CONTEXT
/*-----------------------------------------------------------*/
