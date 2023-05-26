
/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Define the size of a CPU register.
 */
#define ARCH_WORD_SIZE  (sizeof(long))

/**
 * @brief Retrieve the index of a field inside a structure. All the fields
 * must have a word size.
 */
#define indexof(structure,field) (offsetof(structure, field) / ARCH_WORD_SIZE)

/**
 * @brief Number of registers in the ExecutionFrame structure.
 */
#define EXECUTION_FRAME_MAX_REGS   (11)

/**
 * @brief Definition of the x86 DWARF registers set.
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
 * @brief Enumeration of the registers for x86 (32-bit) architecture
 */
typedef enum {
    UNW_X86_EAX = indexof(ExecutionFrame, eax),
    UNW_X86_ECX = indexof(ExecutionFrame, ecx),
    UNW_X86_EDX = indexof(ExecutionFrame, edx),
    UNW_X86_EBX = indexof(ExecutionFrame, ebx),
    UNW_X86_ESP = indexof(ExecutionFrame, esp),
    UNW_X86_EBP = indexof(ExecutionFrame, ebp),
    UNW_X86_ESI = indexof(ExecutionFrame, esi),
    UNW_X86_EDI = indexof(ExecutionFrame, edi),
    UNW_X86_EIP = indexof(ExecutionFrame, eip),
    UNW_X86_EFLAGS = indexof(ExecutionFrame, eflags),
    UNW_X86_TRAPNO = indexof(ExecutionFrame, trapno),
} x86_regnum_t;

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
#define EXECUTION_FRAME_SP_REG      (indexof(x86ExcFrame, esp))

/**
 * @brief Get register i of the execution frame
 */
#define EXECUTION_FRAME_REG(frame, i) ((frame)->registers[(i)])

/**
 * @brief Get the current context
 *
 * @note For x86, this needs to be a macro, else, the compiler will not inline it,
 * even if we specify __attribute__((always_inline))
 *
 * @param ExecutionFrame* Pointer to the frame/context to fill
 */
#define UNW_GET_CONTEXT(frame) { \
    __asm__ __volatile__(".intel_syntax noprefix\n\t" \
                         "mov DWORD PTR [%0 + %c1], eax\n\t" \
                         "mov DWORD PTR [%0 + %c2], ecx\n\t" \
                         "mov DWORD PTR [%0 + %c3], edx\n\t" \
                         "mov DWORD PTR [%0 + %c4], ebx\n\t" \
                         "mov DWORD PTR [%0 + %c5], esp\n\t" \
                         "mov DWORD PTR [%0 + %c6], ebp\n\t" \
                         "mov DWORD PTR [%0 + %c7], esi\n\t" \
                         "mov DWORD PTR [%0 + %c8], edi\n\t" \
                         "mov DWORD PTR [%0 + %c11], 0\n\t"  \
                         /* Special part for retrieving PC */ \
                         "call __get_pc\n" \
                         "__get_pc: pop ebx\n\t" \
                         "mov DWORD PTR [%0 + %c9], ebx\n\t" \
                         /* Same for the flags */ \
                         "pushfd\n\t" \
                         "pop ebx\n\t" \
                         "mov DWORD PTR [%0 + %c10], ebx\n\t" \
                         /* Restore EBX */ \
                         "mov ebx, [%0 + %c4]\n\t" \
                         ".att_syntax prefix" \
                         : \
                         : "r" (frame), \
                           "i" (UNW_X86_EAX * ARCH_WORD_SIZE), \
                           "i" (UNW_X86_ECX * ARCH_WORD_SIZE), \
                           "i" (UNW_X86_EDX * ARCH_WORD_SIZE), \
                           "i" (UNW_X86_EBX * ARCH_WORD_SIZE), \
                           "i" (UNW_X86_ESP * ARCH_WORD_SIZE), \
                           "i" (UNW_X86_EBP * ARCH_WORD_SIZE), \
                           "i" (UNW_X86_ESI * ARCH_WORD_SIZE), \
                           "i" (UNW_X86_EDI * ARCH_WORD_SIZE), \
                           "i" (UNW_X86_EIP * ARCH_WORD_SIZE), \
                           "i" (UNW_X86_EFLAGS * ARCH_WORD_SIZE), \
                           "i" (UNW_X86_TRAPNO * ARCH_WORD_SIZE) \
    ); \
}

#ifdef __cplusplus
}
#endif
