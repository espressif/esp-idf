
/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file DWARF Exception Frames parser header
 *
 * This file describes the frame types for RISC-V, required for
 * parsing `eh_frame` and `eh_frame_hdr`.
 *
 */

#pragma once

#include "riscv/rvruntime-frames.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Define the Executionframe as RvExcFrame for this implementation.
 */
typedef RvExcFrame ExecutionFrame;

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
#define EXECUTION_FRAME_SP_REG      (offsetof(RvExcFrame, sp)/sizeof(uint32_t))

/**
 * @brief Get register i of the execution frame.
 */
#define EXECUTION_FRAME_REG(frame, i) (((uint32_t*) (frame))[(i)])

#ifdef __cplusplus
}
#endif

// #endif // _EH_FRAME_PARSER_IMPL_H
