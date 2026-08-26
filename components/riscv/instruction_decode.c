/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include "riscv/instruction_decode.h"

typedef union {
    struct {
        unsigned int opcode: 7;
        unsigned int rd: 5;
        int imm_19_12: 8;
        int imm_11: 1;
        int imm_10_1: 10;
        int imm20: 1;
    };
    unsigned int inst;
} riscv_jal_intruction_t;

int riscv_decode_offset_from_jal_instruction(const intptr_t inst_addr)
{
    riscv_jal_intruction_t *jal_inst = (riscv_jal_intruction_t *)inst_addr;
    // check if it's a valid JAL instruction
    if (jal_inst->opcode != 0x6f && jal_inst->rd != 0) {
        abort();
    }
    return (jal_inst->imm_10_1 | jal_inst->imm_11 << 10 | jal_inst->imm_19_12 << 11 | jal_inst->imm20 << 19) << 1;
}
