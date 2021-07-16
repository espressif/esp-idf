
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
 * This file describes the frame types for x86, required for
 * parsing `eh_frame` and `eh_frame_hdr`.
 */

#ifndef EH_FRAME_PARSER_IMPL_H
#define EH_FRAME_PARSER_IMPL_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Number of registers in the ExecutionFrame structure.
 */
#define EXECUTION_FRAME_MAX_REGS   (11)

/**
 * @brief Definition of the x86 DWARF tegisters set.
 * The following registers order has been taken from GCC's `i386.c` file:
 */
typedef struct x86ExcFrame
{
    union {
        struct {
            uint32_t eax;
            uint32_t ecx;
            uint32_t edx;
            uint32_t ebx;
            uint32_t esp;
            uint32_t ebp;
            uint32_t esi;
            uint32_t edi;
            uint32_t eip;
            uint32_t eflags;
            uint32_t trapno;
        };
        uint32_t registers[EXECUTION_FRAME_MAX_REGS];
    };
} x86ExcFrame;

/**
 * @brief Define the Executionframe as RvExcFrame for this implementation.
 */
typedef x86ExcFrame ExecutionFrame;

/**
 * @brief Reference the PC register of the execution frame
 */
#define EXECUTION_FRAME_PC(struct)  ((struct).eip)

/**
 * @brief Reference the SP register of the execution frame
 */
#define EXECUTION_FRAME_SP(struct)  ((struct).esp)

/**
 * @brief Index of SP register in the execution frame.
 */
#define EXECUTION_FRAME_SP_REG      (offsetof(x86ExcFrame, esp)/sizeof(uint32_t))

/**
 * @brief Get register i of the execution frame
 */
#define EXECUTION_FRAME_REG(frame, i) ((frame)->registers[(i)])

#endif // _EH_FRAME_PARSER_IMPL_H
