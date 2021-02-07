
// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file DWARF Exception Frames parser header
 *
 * This file describes the frame types for RISC-V, required for
 * parsing `eh_frame` and `eh_frame_hdr`.
 *
 */

#ifndef EH_FRAME_PARSER_IMPL_H
#define EH_FRAME_PARSER_IMPL_H

#include "riscv/rvruntime-frames.h"

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

#endif // _EH_FRAME_PARSER_IMPL_H
