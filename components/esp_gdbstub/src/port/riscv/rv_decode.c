/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "rv_decode.h"

static inline uint32_t rv_inst_len(uint32_t inst)
{
#ifdef __riscv_c
    if ((inst & 0x3) != 0x3)
        return 2; /* 16-bit instructions.  */
#endif /* __riscv_c  */
    return 4; /* 32-bit instructions.  */
}

static uint32_t rv_get_register_value(esp_gdbstub_frame_t *frame, uint32_t r_num)
{
     r_num &= 0x1F;
     if (r_num == 0) { /* zero-wired  */
        return 0;
     }
     return (&frame->mepc)[r_num];
}

static uint32_t rv_get_rs1_value(esp_gdbstub_frame_t *frame, uint32_t inst)
{
     return rv_get_register_value(frame, inst >> 15);
}

static uint32_t rv_get_rs2_value(esp_gdbstub_frame_t *frame, uint32_t inst)
{
     return rv_get_register_value(frame, inst >> 20);
}

static int32_t rv_get_branch_next_inst_offset(esp_gdbstub_frame_t *frame, uint32_t inst)
{
    uint32_t funct = (inst >> 12) & 0x7;
    uint32_t rs1 = rv_get_rs1_value(frame, inst);
    uint32_t rs2 = rv_get_rs2_value(frame, inst);
    if ((funct == 0 && rs1 == rs2) ||               /* beq  true */
        (funct == 1 && rs1 != rs2) ||               /* bne  true */
        (funct == 4 && (int32_t) rs1 <  (int32_t) rs2) ||   /* blt  true */
        (funct == 5 && (int32_t) rs1 >= (int32_t) rs2) ||   /* bge  true */
        (funct == 6 && rs1 <  rs2) ||               /* bltu true */
        (funct == 7 && rs1 >= rs2)) {               /* bgeu true */
        return ((inst >> 8 ) & 0xF ) << 1  |        /* imm[4:1]  */
               ((inst >> 25) & 0x3F) << 5  |        /* imm[10:5] */
               ((inst >> 7 ) & 0x1 ) << 11 |        /* imm[11]   */
               ((inst >> 31) ? 0xFFFFF000 : 0);     /* imm[12] is sign part  */;
    }
    return rv_inst_len(inst); /* branch will not jump. Next instruction will be executed  */
}

static int32_t rv_get_jal_next_inst_offset(uint32_t inst)
{
    return ((inst >> 21) & 0x3FF) << 1  |           /* imm[10:1]  */
           ((inst >> 20) & 0x1  ) << 11 |           /* imm[11] */
           ((inst >> 12) & 0xFF ) << 12 |           /* imm[19:12] */
           ((inst >> 31) ? 0xFFF00000 : 0);         /* imm[20] is sign bit  */
}

static uint32_t rv_get_jalr_next_inst(esp_gdbstub_frame_t *frame, uint32_t inst)
{
    uint32_t rs1 = rv_get_rs1_value(frame, inst);
    int32_t imm = ((inst >> 20) & 0xFFF);           /* imm[11:0]  */
    imm |= (imm >> 11) ? 0xFFFFF000 : 0;            /* imm[11] is sign bit  */
    return rs1 + imm;
}

#ifdef __riscv_c /* compressed riscv instruction set  */
static uint32_t rv_get_c_rs1_value(esp_gdbstub_frame_t *frame, uint32_t inst)
{
     return rv_get_register_value(frame, inst >> 7);
}

static uint32_t rv_get_c_rs2_num(uint32_t inst)
{
     return (inst >> 2) & 0x1F;
}

static uint32_t rv_get_c_rd_num(uint32_t inst)
{
     return (inst >> 7) & 0x1F;
}

static int32_t rv_get_c_jal_next_inst_offset(uint32_t inst)
{
    return ((inst >> 3 ) & 0x7 ) << 1  |        /* imm[3:1]  */
           ((inst >> 11) & 0x1 ) << 4  |        /* imm[4]    */
           ((inst >> 2 ) & 0x1 ) << 5  |        /* imm[5]    */
           ((inst >> 7 ) & 0x1 ) << 6  |        /* imm[6]    */
           ((inst >> 6 ) & 0x1 ) << 7  |        /* imm[7]    */
           ((inst >> 9 ) & 0x3 ) << 8  |        /* imm[9:8]  */
           ((inst >> 8 ) & 0x1 ) << 10 |        /* imm[10]   */
           ((inst >> 12) & 0x1 ) << 11 |        /* imm[11]   */
           ((inst >> 12) & 0x1 ? 0xFFFFF000 : 0);     /* imm[11] is sign part  */;
}

static int32_t rv_get_c_branch_next_inst_offset(esp_gdbstub_frame_t *frame,uint32_t inst)
{
    const int32_t rs1_value = (&frame->s0)[(inst >> 7) & 7];
    const bool is_bnez = (inst >> 13) & 1;
    if ((rs1_value == 0 && !is_bnez) ||
        (rs1_value != 0 && is_bnez)) {
        return ((inst >> 3 ) & 0x3 ) << 1 |     /* imm[2:1]  */
               ((inst >> 10) & 0x3 ) << 3 |     /* imm[4:3]  */
               ((inst >> 2 ) & 0x1 ) << 5 |     /* imm[5]    */
               ((inst >> 5 ) & 0x3 ) << 6 |     /* imm[7:6]  */
               ((inst >> 12) & 0x1 ) << 8 |     /* imm[8]    */
               ((inst >> 12) & 0x1 ? 0xFFFFFF00 : 0);     /* imm[8] is sign part  */;
    }
    return 2;
}
#endif /* __riscv_c  */

uintptr_t rv_compute_next_pc(esp_gdbstub_frame_t *frame, uintptr_t inst_addr)
{
    const uint32_t inst = *((uint32_t *) inst_addr);
    const uint32_t inst_len = rv_inst_len(inst);
    if (inst_len == 4) { /* this is 32-bit instruction */
        switch (inst & 0x7f) {
            case 0x63: /* branch */
                return inst_addr + rv_get_branch_next_inst_offset(frame, inst);
            case 0x6F: /* jal  */
                return inst_addr + rv_get_jal_next_inst_offset(inst);
            case 0x67: /* jalr  */
                return rv_get_jalr_next_inst(frame, inst);
        }
    }
#ifdef __riscv_c /* compressed riscv instruction set  */
    const uint32_t funct3 = (inst & 0xFFFF) >> 13;
    if ((inst & 3) == 1) {
        switch (funct3) {
            case 1: /* c.jal  */
            case 5: /* c.j  */
                return inst_addr + rv_get_c_jal_next_inst_offset(inst);
            case 6: /* c.beqz  */
            case 7: /* c.bnez  */
                return inst_addr + rv_get_c_branch_next_inst_offset(frame, inst);
        }
    } else if ((inst & 3) == 2) {
        uint32_t rs2 = rv_get_c_rs2_num(inst);
        uint32_t rd = rv_get_c_rd_num(inst);
        /* c.jr and c.jalr:
         *
         * They must have funct3 == 0b100, rd!=0 and rs2==0
         * See Table 1.6: Instruction listing for RVC, Quadrant 2
         * in The RISC-V Compressed Instruction Set Manual
         */
        if (funct3 == 4 && rd != 0 && rs2 == 0) {
            return rv_get_c_rs1_value(frame, inst);
        }
    }
#endif /* __riscv_c  */
    return inst_addr + inst_len;
}
