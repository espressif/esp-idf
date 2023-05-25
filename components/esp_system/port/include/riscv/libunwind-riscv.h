
/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This file describes the frame types for RISC-V, required for
 * parsing `eh_frame` and `eh_frame_hdr`, and more generally libunwind.
 */

#pragma once

#include <stddef.h>
#include "esp_attr.h"
#include "riscv/rvruntime-frames.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Define the size of a CPU register.
 */
#define ARCH_WORD_SIZE  (sizeof(long))

/**
 * @brief Retrive the index of a field inside a structure. All the fields
 * must have a word size.
 */
#define indexof(structure,field) (offsetof(structure, field) / ARCH_WORD_SIZE)

/**
 * @brief Define the Executionframe as RvExcFrame for this implementation.
 */
typedef RvExcFrame ExecutionFrame;

/**
 * @brief Enumeration of all the registers for RISC-V architecture
 */
typedef enum {
    UNW_RISCV_PC = indexof(ExecutionFrame, mepc),
    UNW_RISCV_RA = indexof(ExecutionFrame, ra),
    UNW_RISCV_SP = indexof(ExecutionFrame, sp),
    UNW_RISCV_GP = indexof(ExecutionFrame, gp),
    UNW_RISCV_TP = indexof(ExecutionFrame, tp),
    UNW_RISCV_T0 = indexof(ExecutionFrame, t0),
    UNW_RISCV_T1 = indexof(ExecutionFrame, t1),
    UNW_RISCV_T2 = indexof(ExecutionFrame, t2),
    UNW_RISCV_S0 = indexof(ExecutionFrame, s0),
    UNW_RISCV_S1 = indexof(ExecutionFrame, s1),
    UNW_RISCV_A0 = indexof(ExecutionFrame, a0),
    UNW_RISCV_A1 = indexof(ExecutionFrame, a1),
    UNW_RISCV_A2 = indexof(ExecutionFrame, a2),
    UNW_RISCV_A3 = indexof(ExecutionFrame, a3),
    UNW_RISCV_A4 = indexof(ExecutionFrame, a4),
    UNW_RISCV_A5 = indexof(ExecutionFrame, a5),
    UNW_RISCV_A6 = indexof(ExecutionFrame, a6),
    UNW_RISCV_A7 = indexof(ExecutionFrame, a7),
    UNW_RISCV_S2 = indexof(ExecutionFrame, s2),
    UNW_RISCV_S3 = indexof(ExecutionFrame, s3),
    UNW_RISCV_S4 = indexof(ExecutionFrame, s4),
    UNW_RISCV_S5 = indexof(ExecutionFrame, s5),
    UNW_RISCV_S6 = indexof(ExecutionFrame, s6),
    UNW_RISCV_S7 = indexof(ExecutionFrame, s7),
    UNW_RISCV_S8 = indexof(ExecutionFrame, s8),
    UNW_RISCV_S9 = indexof(ExecutionFrame, s9),
    UNW_RISCV_S10 = indexof(ExecutionFrame, s10),
    UNW_RISCV_S11 = indexof(ExecutionFrame, s11),
    UNW_RISCV_T3 = indexof(ExecutionFrame, t3),
    UNW_RISCV_T4 = indexof(ExecutionFrame, t4),
    UNW_RISCV_T5 = indexof(ExecutionFrame, t5),
    UNW_RISCV_T6 = indexof(ExecutionFrame, t6),
    UNW_RISCV_MSTATUS = indexof(ExecutionFrame, mstatus),
    UNW_RISCV_MTVEC = indexof(ExecutionFrame, mtvec),
    UNW_RISCV_MCAUSE = indexof(ExecutionFrame, mcause),
    UNW_RISCV_MTVAL = indexof(ExecutionFrame, mtval),
    UNW_RISCV_MHARTID = indexof(ExecutionFrame, mhartid),
} riscv_regnum_t;

/**
 * @brief Number of registers in the ExecutionFrame structure.
 *
 * This will be used to define and initialize the DWARF machine state.
 * In practice, we only have 16 registers that are callee saved, thus, we could
 * only save them and ignore the rest. However, code to calculate mapping of
 * CPU registers to DWARF registers would take more than the 16 registers we
 * would save... so save all registers.
 */
#define EXECUTION_FRAME_MAX_REGS   (32)

/**
 * @brief Reference the PC register of the execution frame.
 */
#define EXECUTION_FRAME_PC(frame)   ((frame).mepc)

/**
 * @brief Reference the SP register of the execution frame.
 */
#define EXECUTION_FRAME_SP(frame)   ((frame).sp)

/**
 * @brief Index of SP register in the execution frame.
 */
#define EXECUTION_FRAME_SP_REG      (indexof(RvExcFrame, sp))

/**
 * @brief Get register i of the execution frame.
 */
#define EXECUTION_FRAME_REG(frame, i) (((uint32_t*) (frame))[(i)])

/**
 * @brief Get the current context
 */
FORCE_INLINE_ATTR void UNW_GET_CONTEXT(ExecutionFrame* frame) {
    __asm__ __volatile__("sw t0, %1(%0)\n"
                         "auipc t0, 0\n"
                         "sw t0, %2(%0)\n"
                         "sw ra, %3(%0)\n"
                         "sw sp, %4(%0)\n"
                         "sw gp, %5(%0)\n"
                         "sw tp, %6(%0)\n"
                         "sw t1, %7(%0)\n"
                         "sw t2, %8(%0)\n"
                         "sw s0, %9(%0)\n"
                         "sw s1, %10(%0)\n"
                         "sw a0, %11(%0)\n"
                         "sw a1, %12(%0)\n"
                         "sw a2, %13(%0)\n"
                         "sw a3, %14(%0)\n"
                         "sw a4, %15(%0)\n"
                         "sw a5, %16(%0)\n"
                         "sw a6, %17(%0)\n"
                         "sw a7, %18(%0)\n"
                         "sw s2, %19(%0)\n"
                         "sw s3, %20(%0)\n"
                         "sw s4, %21(%0)\n"
                         "sw s5, %22(%0)\n"
                         "sw s6, %23(%0)\n"
                         "sw s7, %24(%0)\n"
                         "sw s8, %25(%0)\n"
                         "sw s9, %26(%0)\n"
                         "sw s10, %27(%0)\n"
                         "sw s11, %28(%0)\n"
                         "sw t3, %29(%0)\n"
                         :
                         : "r" (frame),
                           "i" (UNW_RISCV_T0 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_PC * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_RA * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_SP * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_GP * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_TP * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_T1 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_T2 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_S0 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_S1 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_A0 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_A1 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_A2 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_A3 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_A4 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_A5 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_A6 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_A7 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_S2 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_S3 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_S4 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_S5 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_S6 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_S7 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_S8 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_S9 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_S10 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_S11 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_T3 * ARCH_WORD_SIZE)
    );
    /* GCC doesn't allow us to have more than 30 operands in a single
     * __asm__ __volatile__ definition, so we have to split it into 2 */
    __asm__ __volatile__("sw t4, %1(%0)\n"
                         "sw t5, %2(%0)\n"
                         "sw t6, %3(%0)\n"
                         "csrr t0, mstatus\n"
                         "sw t0, %4(%0)\n"
                         "csrr t0, mtvec\n"
                         "sw t0, %5(%0)\n"
                         "csrr t0, mcause\n"
                         "sw t0, %6(%0)\n"
                         "csrr t0, mtval\n"
                         "sw t0, %7(%0)\n"
                         "csrr t0, mhartid\n"
                         "sw t0, %8(%0)\n"
                         /* We have to restore t0 as it may be in use by the function that makes the use of this assembly snippet */
                         "lw t0, %9(%0)\n"
                         :
                         : "r" (frame),
                           "i" (UNW_RISCV_T4 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_T5 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_T6 * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_MSTATUS * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_MTVEC * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_MCAUSE * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_MTVAL * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_MHARTID * ARCH_WORD_SIZE),
                           "i" (UNW_RISCV_T0 * ARCH_WORD_SIZE)
    );
}

#ifdef __cplusplus
}
#endif
