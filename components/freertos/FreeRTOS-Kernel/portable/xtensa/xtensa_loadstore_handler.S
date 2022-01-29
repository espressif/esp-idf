/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 *  LoadStoreErrorCause:     Occurs when trying to access 32 bit addressable memory region as 8 bit or 16 bit
 *  LoadStoreAlignmentCause: Occurs when trying to access in an unaligned manner
 *
 *          xxxx xxxx = imm8 field
 *               yyyy = imm4 field
 *               ssss = s field
 *               tttt = t field
 *
 *                            16                0
 *                            -------------------
 *          L32I.N            yyyy ssss tttt 1000
 *          S32I.N            yyyy ssss tttt 1001
 *
 *                  23                          0
 *                  -----------------------------
 *          L8UI    xxxx xxxx 0000 ssss tttt 0010               <- LoadStoreError
 *          L16UI   xxxx xxxx 0001 ssss tttt 0010               <- LoadStoreError, LoadStoreAlignment
 *          L16SI   xxxx xxxx 1001 ssss tttt 0010               <- LoadStoreError, LoadStoreAlignment
 *          L32I    xxxx xxxx 0010 ssss tttt 0010               <- LoadStoreAlignment
 *
 *          S8I     xxxx xxxx 0100 ssss tttt 0010               <- LoadStoreError
 *          S16I    xxxx xxxx 0101 ssss tttt 0010               <- LoadStoreError, LoadStoreAlignment
 *          S32I    xxxx xxxx 0110 ssss tttt 0010               <- LoadStoreAlignment
 *
 *                   ******* UNSUPPORTED *******
 *
 *          L32E    0000 1001 rrrr ssss tttt 0000
 *          S32E    0100 1001 rrrr ssss tttt 0000
 *                  -----------------------------
 */

#include "xtensa_rtos.h"
#include "sdkconfig.h"
#include "soc/soc.h"

#define LOADSTORE_HANDLER_STACK_SZ      8
    .section .bss, "aw"
    .balign 16
LoadStoreHandlerStack:
    .rept LOADSTORE_HANDLER_STACK_SZ
    .word 0
    .endr


/* LoadStoreErrorCause handler:
 *
 * Completes 8-bit or 16-bit load/store instructions from 32-bit aligned memory region
 * Called from UserExceptionVector if EXCCAUSE is LoadStoreErrorCause
 */

    .global   LoadStoreErrorHandler
    .section .iram1, "ax"

    .literal_position

    .balign 4
LoadStoreErrorHandler:
    .type   LoadStoreErrorHandler, @function

    wsr     a0, depc                            // Save return address in depc
    mov     a0, sp
    movi    sp, LoadStoreHandlerStack
    s32i    a0, sp, 0x04                        // Since a0 contains value of a1
    s32i    a2, sp, 0x08
    s32i    a3, sp, 0x0c
    s32i    a4, sp, 0x10

    rsr     a0, sar                             // Save SAR in a0 to restore later

    /* Check whether the address lies in the valid range */
    rsr     a3, excvaddr
    movi    a4, _iram_text_end                       // End of code section of IRAM
    bge     a3, a4, 1f
    movi    a4, SOC_CACHE_APP_LOW               // Check if in APP cache region
    blt     a3, a4, .LS_wrong_opcode
    movi    a4, SOC_CACHE_APP_HIGH
    bge     a3, a4, .LS_wrong_opcode
    j       2f

1:
    movi    a4, SOC_IRAM_HIGH                   // End of IRAM address range
    bge     a3, a4, .LS_wrong_opcode

2:
    /* Examine the opcode which generated the exception */
    /* Note: Instructions are in this order to avoid pipeline stalls. */
    rsr     a2, epc1
    movi    a4, ~3
    ssa8l   a2                                  // sar is now correct shift for aligned read
    and     a2, a2, a4                          // a2 now 4-byte aligned address of instruction
    l32i    a4, a2, 0
    l32i    a2, a2, 4

    src     a2, a2, a4                          // a2 now instruction that failed
    bbci    a2, 1, .LS_wrong_opcode
    bbsi    a2, 14, .LSE_store_op               // Store instruction

    /* l8/l16ui/l16si */
    movi    a4, ~3
    and     a4, a3, a4                          // a4 now word aligned read address

    ssa8l   a3                                  // sar is now shift to extract a3's byte
    l32i    a4, a4, 0                           // perform the actual read
    srl     a4, a4                              // shift right correct distance
    extui   a3, a2, 12, 4
    bnez    a3, 1f                              // l16ui/l16si
    extui   a4, a4, 0, 8                        // mask off bits needed for an l8
    j       2f

1:
    extui   a4, a4, 0, 16
    bbci    a2, 15, 2f                          // l16ui

    /* Sign adjustment */
    slli    a4, a4, 16
    srai    a4, a4, 16                          // a4 contains the value

2:
    /* a4 contains the value */
    rsr     a3, epc1
    addi    a3, a3, 3
    wsr     a3, epc1
    wsr     a0, sar
    rsr     a0, excsave1

    extui   a2, a2, 3, 5
    blti    a2, 10, .LSE_stack_reg

    movi    a3, .LS_jumptable_base
    addx8   a2, a2, a3                          // a2 is now the address to jump to
    l32i    a3, sp, 0x0c
    jx      a2

.LSE_stack_reg:
    addx2   a2, a2, sp
    s32i    a4, a2, 0

    /* Restore all values */
    l32i    a4, sp, 0x10
    l32i    a3, sp, 0x0c
    l32i    a2, sp, 0x08
    l32i    a1, sp, 0x04
    rfe

.LSE_store_op:
    s32i    a5, a1, 0x14
    s32i    a6, a1, 0x18

    /* a2 -> instruction that caused the error */
    /* a3 -> unaligned address */
    extui   a4, a2, 4, 4
    blti    a4, 7, 1f
    movi    a5, .LSE_store_reg
    addx8   a5, a4, a5
    jx      a5

1:
    addx4   a4, a4, sp
    l32i    a4, a4, 0

.LSE_store_data:
    /* a4 contains the value */
    rsr     a6, epc1
    addi    a6, a6, 3
    wsr     a6, epc1

    ssa8b   a3
    movi    a5, -1
    bbsi    a2, 12, 1f                          // s16
    extui   a4, a4, 0, 8
    movi    a6, 0xff
    j       2f
1:
    extui   a4, a4, 0, 16
    movi    a6, 0xffff
2:
    sll     a4, a4                              // shift the value to proper offset
    sll     a6, a6
    xor     a5, a5, a6                          // a5 contains the mask

    movi    a6, ~3
    and     a3, a3, a6                          // a3 has the aligned address
    l32i    a6, a3, 0                           // a6 contains the data at the aligned address
    and     a6, a6, a5
    or      a4, a6, a4
    s32i    a4, a3, 0

    /* Restore registers */
    wsr     a0, sar

    l32i    a6, sp, 0x18
    l32i    a5, sp, 0x14
    l32i    a4, sp, 0x10
    l32i    a3, sp, 0x0c
    l32i    a2, sp, 0x08
    l32i    a1, sp, 0x04
    rsr     a0, excsave1

    rfe

.LSE_store_reg:
    .org .LSE_store_reg + (7 * 8)
    mov     a4, a7
    j .LSE_store_data

    .org .LSE_store_reg + (8 * 8)
    mov     a4, a8
    j .LSE_store_data

    .org .LSE_store_reg + (9 * 8)
    mov     a4, a9
    j .LSE_store_data

    .org .LSE_store_reg + (10 * 8)
    mov     a4, a10
    j .LSE_store_data

    .org .LSE_store_reg + (11 * 8)
    mov     a4, a11
    j .LSE_store_data

    .org .LSE_store_reg + (12 * 8)
    mov     a4, a12
    j .LSE_store_data

    .org .LSE_store_reg + (13 * 8)
    mov     a4, a13
    j .LSE_store_data

    .org .LSE_store_reg + (14 * 8)
    mov     a4, a14
    j .LSE_store_data

    .org .LSE_store_reg + (15 * 8)
    mov     a4, a15
    j .LSE_store_data


/* LoadStoreAlignmentCause handler:
 *
 * Completes unaligned 16-bit and 32-bit load/store instructions from 32-bit aligned memory region
 * Called from UserExceptionVector if EXCCAUSE is LoadStoreAlignmentCause
 */

    .global   AlignmentErrorHandler
    .section .iram1, "ax"

    .literal_position

    .balign 4
AlignmentErrorHandler:
    .type   AlignmentErrorHandler, @function

    wsr     a0, depc                            // Save return address in depc
    mov     a0, sp
    movi    sp, LoadStoreHandlerStack
    s32i    a0, sp, 0x04                        // Since a0 contains value of a1
    s32i    a2, sp, 0x08
    s32i    a3, sp, 0x0c
    s32i    a4, sp, 0x10

    rsr     a0, sar                             // Save SAR in a0 to restore later

    /* Check whether the address lies in the valid range */
    rsr     a3, excvaddr
    movi    a4, _iram_text_end                       // End of code section of IRAM
    bge     a3, a4, 1f
    movi    a4, SOC_CACHE_APP_LOW               // Check if in APP cache region
    blt     a3, a4, .LS_wrong_opcode
    movi    a4, SOC_CACHE_APP_HIGH
    bge     a3, a4, .LS_wrong_opcode
    j       2f

1:
    movi    a4, SOC_IRAM_HIGH                   // End of IRAM address range
    bge     a3, a4, .LS_wrong_opcode

2:
    /* Examine the opcode which generated the exception */
    /* Note: Instructions are in this order to avoid pipeline stalls. */
    rsr     a2, epc1
    movi    a4, ~3
    ssa8l   a2                                  // sar is now correct shift for aligned read
    and     a2, a2, a4                          // a2 now 4-byte aligned address of instruction
    l32i    a4, a2, 0
    l32i    a2, a2, 4

    /* a2 has the instruction that caused the error */
    src     a2, a2, a4
    extui   a4, a2, 0, 4
    addi    a4, a4, -9
    beqz    a4, .LSA_store_op
    bbsi    a2, 14, .LSA_store_op

    ssa8l   a3                                  // a3 contains the unaligned address
    movi    a4, ~3
    and     a4, a3, a4                          // a4 has the aligned address
    l32i    a3, a4, 0
    l32i    a4, a4, 4
    src     a4, a4, a3

    rsr     a3, epc1
    addi    a3, a3, 2
    bbsi    a2, 3, 1f                           // l32i.n
    bbci    a2, 1, .LS_wrong_opcode
    addi    a3, a3, 1

    bbsi    a2, 13, 1f                          // l32
    extui   a4, a4, 0, 16
    bbci    a2, 15, 1f                          // l16ui

    /* Sign adjustment */
    slli    a4, a4, 16
    srai    a4, a4, 16                          // a4 contains the value

1:
    wsr     a3, epc1
    wsr     a0, sar
    rsr     a0, excsave1

    extui   a2, a2, 4, 4
    blti    a2, 5, .LSA_stack_reg               // a3 contains the target register

    movi    a3, .LS_jumptable_base
    slli    a2, a2, 4
    add     a2, a2, a3                          // a2 is now the address to jump to
    l32i    a3, sp, 0x0c
    jx      a2

.LSA_stack_reg:
    addx4   a2, a2, sp
    s32i    a4, a2, 0

    /* Restore all values */
    l32i    a4, sp, 0x10
    l32i    a3, sp, 0x0c
    l32i    a2, sp, 0x08
    l32i    a1, sp, 0x04
    rfe

/* Store instruction */
.LSA_store_op:
    s32i    a5, sp, 0x14
    s32i    a6, sp, 0x18
    s32i    a7, sp, 0x1c

    /* a2 -> instruction that caused the error */
    /* a3 -> unaligned address */
    extui   a4, a2, 4, 4
    blti    a4, 8, 1f
    movi    a5, .LSA_store_reg
    addx8   a5, a4, a5
    jx      a5

1:
    addx4   a4, a4, sp
    l32i    a4, a4, 0                           // a4 contains the value

.LSA_store_data:
    movi    a6, 0

    rsr     a7, epc1
    addi    a7, a7 ,2
    bbsi    a2, 3, 1f                           // s32i.n
    bbci    a2, 1, .LS_wrong_opcode

    addi    a7, a7, 1
    bbsi    a2, 13, 1f                          // s32i

    movi    a5, -1
    extui   a4, a4, 0, 16
    slli    a6, a5, 16                          // 0xffff0000

1:
    wsr     a7, epc1
    movi    a5, ~3
    and     a5, a3, a5                          // a5 has the aligned address

    ssa8b   a3
    movi    a3, -1
    src     a7, a6, a3
    src     a3, a3, a6

    /* Store data on lower address */
    l32i    a6, a5, 0
    and     a6, a6, a7
    sll     a7, a4
    or      a6, a6, a7
    s32i    a6, a5, 0

    /* Store data on higher address */
    l32i    a7, a5, 4
    srl     a6, a4
    and     a3, a7, a3
    or      a3, a3, a6
    s32i    a3, a5, 4

    /* Restore registers */
    wsr     a0, sar
    rsr     a0, excsave1

    l32i    a7, sp, 0x1c
    l32i    a6, sp, 0x18
    l32i    a5, sp, 0x14
    l32i    a4, sp, 0x10
    l32i    a3, sp, 0x0c
    l32i    a2, sp, 0x08
    l32i    a1, sp, 0x04
    rfe

.LSA_store_reg:
    .org .LSA_store_reg + (8 * 8)
    mov     a4, a8
    j       .LSA_store_data

    .org .LSA_store_reg + (9 * 8)
    mov     a4, a9
    j       .LSA_store_data

    .org .LSA_store_reg + (10 * 8)
    mov     a4, a10
    j       .LSA_store_data

    .org .LSA_store_reg + (11 * 8)
    mov     a4, a11
    j       .LSA_store_data

    .org .LSA_store_reg + (12 * 8)
    mov     a4, a12
    j       .LSA_store_data

    .org .LSA_store_reg + (13 * 8)
    mov     a4, a13
    j       .LSA_store_data

    .org .LSA_store_reg + (14 * 8)
    mov     a4, a14
    j       .LSA_store_data

    .org .LSA_store_reg + (15 * 8)
    mov     a4, a15
    j       .LSA_store_data

/*
 * Common routines for both the exception handlers
 */
    .balign 4
.LS_jumptable:
    /*  The first 5 entries (80 bytes) of this table are unused (registers
        a0..a4 are handled separately above).  Rather than have a whole bunch
        of wasted space, just pretend that the table starts 80 bytes
        earlier in memory. */
    .set    .LS_jumptable_base, .LS_jumptable - (16 * 5)

    .org    .LS_jumptable_base + (16 * 5)
    mov     a5, a4
    l32i    a4, sp, 0x10
    l32i    a2, sp, 0x08
    l32i    a1, sp, 0x04
    rfe

    .org    .LS_jumptable_base + (16 * 6)
    mov     a6, a4
    l32i    a4, sp, 0x10
    l32i    a2, sp, 0x08
    l32i    a1, sp, 0x04
    rfe

    .org    .LS_jumptable_base + (16 * 7)
    mov     a7, a4
    l32i    a4, sp, 0x10
    l32i    a2, sp, 0x08
    l32i    a1, sp, 0x04
    rfe

    .org    .LS_jumptable_base + (16 * 8)
    mov     a8, a4
    l32i    a4, sp, 0x10
    l32i    a2, sp, 0x08
    l32i    a1, sp, 0x04
    rfe

    .org    .LS_jumptable_base + (16 * 9)
    mov     a9, a4
    l32i    a4, sp, 0x10
    l32i    a2, sp, 0x08
    l32i    a1, sp, 0x04
    rfe

    .org    .LS_jumptable_base + (16 * 10)
    mov     a10, a4
    l32i    a4, sp, 0x10
    l32i    a2, sp, 0x08
    l32i    a1, sp, 0x04
    rfe

    .org    .LS_jumptable_base + (16 * 11)
    mov     a11, a4
    l32i    a4, sp, 0x10
    l32i    a2, sp, 0x08
    l32i    a1, sp, 0x04
    rfe

    .org    .LS_jumptable_base + (16 * 12)
    mov     a12, a4
    l32i    a4, sp, 0x10
    l32i    a2, sp, 0x08
    l32i    a1, sp, 0x04
    rfe

    .org    .LS_jumptable_base + (16 * 13)
    mov     a13, a4
    l32i    a4, sp, 0x10
    l32i    a2, sp, 0x08
    l32i    a1, sp, 0x04
    rfe

    .org    .LS_jumptable_base + (16 * 14)
    mov     a14, a4
    l32i    a4, sp, 0x10
    l32i    a2, sp, 0x08
    l32i    a1, sp, 0x04
    rfe

    .org    .LS_jumptable_base + (16 * 15)
    mov     a15, a4
    l32i    a4, sp, 0x10
    l32i    a2, sp, 0x08
    l32i    a1, sp, 0x04
    rfe

.LS_wrong_opcode:
    /* Reaches here if the address is in invalid range or the opcode isn't supported.
     * Restore registers and jump back to _xt_user_exc
     */
    wsr     a0, sar
    l32i    a4, sp, 0x10
    l32i    a3, sp, 0x0c
    l32i    a2, sp, 0x08
    l32i    a1, sp, 0x04
    rsr     a0, depc
    ret                                         // Equivalent to jx a0
