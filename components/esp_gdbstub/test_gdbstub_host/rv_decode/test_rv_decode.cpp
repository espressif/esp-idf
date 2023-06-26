/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "catch.hpp"

extern "C" {
#include "esp_gdbstub_common.h"
#include "rv_decode.h"

esp_gdbstub_frame_t regs;
esp_gdbstub_frame_t *temp_regs_frame = &regs;
} // extern "C"

#if 0
#define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

struct inst_list_s {
    uint32_t inst;
    const char *name;
};

struct inst_list_s rv32i_nojump[] = {
    {0x31011d37, "lui"},
    {0x00000917, "auipc"},
    {0x0b1c0e03, "lb"},
    {0xc8aecc03, "lbu"},
    {0x07271103, "lh"},
    {0x02525403, "lhu"},
    {0x00392903, "lw"},
    {0x000000a3, "sb"},
    {0x00009da3, "sh"},
    {0x0000a423, "sw"},
    {0xd3060d13, "addi"},
    {0x000cad13, "slti"},
    {0x00003d13, "sltiu"},
    {0x0001ce13, "xori"},
    {0x00006013, "ori"},
    {0x00007c13, "andi"},
    {0x00099913, "slli"},
    {0x00005e13, "srli"},
    {0x4107d313, "srai"},
    {0x0000fe33, "and"},
    {0x401e0933, "sub"},
    {0x000012b3, "sll"},
    {0x0036a033, "slt"},
    {0x00853433, "sltu"},
    {0x00004033, "xor"},
    {0x00005133, "srl"},
    {0x40e7d7b3, "sra"},
    {0x000062b3, "or"},
    {0x000071b3, "and"},
    {0x0000000f, "fence"},
    {0x0000100f, "fence.i"},
    {0x00002ef3, "csrr"},
    {0x010312f3, "csrrw"},
    {0x00005473, "csrrwi"},
    {0x0001b073, "csrc"},
    {0x01007073, "csrci"},
    {0x000078f3, "csrrci"},
    {0x0002a073, "csrs"},
    {0x0000e073, "csrsi"},
    {0x000225f3, "csrrs"}};

struct inst_list_s rv32m_nojump[] = {
    {0x025a0133, "mul"},
    {0x02031233, "mulh"},
    {0x02f437b3, "mulhu"},
    {0x03012e33, "mulhsu"},
    {0x02a6c6b3, "div"},
    {0x02f5d533, "divu"},
    {0x02a47433, "remu"},
    {0x0324e4b3, "rem"}};

struct inst_list_s rv32a_nojump[] = {
    {0x100523af, "lr.w"},
    {0x19d2ae2f, "sc.w"},
    {0x0c07202f, "amoswap.w"},
    {0x0000222f, "amoadd.w"},
    {0x2072a2af, "amoxor.w"},
    {0x6072a2af, "amoand.w"},
    {0x4072a2af, "amoor.w"},
    {0x801c262f, "amomin.w"}};

struct inst_list_s rv32c_nojump[] = {
    {0x8522, "c.mv"},
    {0x4200, "c.lw"},
    {0x4501, "c.li"},
    {0x6605, "c.lui"},
    {0x8e65, "c.and"},
    {0x985d, "c.andi"},
    {0x8c9d, "c.sub"},
    {0x8f41, "c.or"},
    {0x8da5, "c.xor"},
    {0x100a, "c.slli"},
    {0x0002, "c.slli64"},
    {0x8011, "c.srli"},
    {0x8081, "c.srli64"},
    {0x9459, "c.srai"},
    {0x8481, "c.srai64"}};


TEST_CASE("decode rv32i instructions")
{
    uintptr_t pc;
    uint32_t inst;
    uintptr_t inst_addr = (uintptr_t)&inst;

    for (size_t i = 0; i < sizeof(rv32i_nojump)/sizeof(rv32i_nojump[0]); i++) {
        inst = rv32i_nojump[i].inst;
        DEBUG_PRINTF("testing instruction %s\n", rv32i_nojump[i].name);
        CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 4);
    }

    /* beq positive offset  */
    /* 420147bc:	0af50b63          	beq	a0,a5,42014872  */
    regs.a0 = regs.a5 = 0x777;
    inst = 0x0af50b63;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x42014872 - 0x420147bc));
    regs.a0 = 0x111;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 4);

    /* beq negative offset  */
    /* 40383b48:	fae789e3          	beq	a5,a4,40383afa  */
    regs.a4 = regs.a5 = 0x777;
    inst = 0xfae789e3;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x40383afa - 0x40383b48));
    regs.a4 = 0x111;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 4);

    /* bne positive offset  */
    /* 42014798:	10f51163          	bne	a0,a5,4201489a  */
    regs.a0 = regs.a5 = 0x777;
    inst = 0x10f51163;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 4);
    regs.a0 = 0x111;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x4201489a - 0x42014798));

    /* bne negative offset  */
    /* 4200b7e0:	fed711e3          	bne	a4,a3,4200b7c2  */
    regs.a3 = regs.a4 = 0x777;
    inst = 0xfed711e3;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 4);
    regs.a3 = 0x111;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x4200b7c2 - 0x4200b7e0));

    /* blt positive offset  */
    /* 420117a0:	04f54763          	blt	a0,a5,420117ee  */
    regs.a0 = regs.a5 = 0x777;
    inst = 0x04f54763;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 4);
    regs.a0++;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 4);
    regs.a0 = 0x111;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x420117ee - 0x420117a0));

    /* blt negative offset  */
    /* 42014448:	fcd5c0e3          	blt	a1,a3,42014408  */
    regs.a1 = regs.a3 = 0x777;
    inst = 0xfcd5c0e3;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 4);
    regs.a1++;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 4);
    regs.a1 = 0x111;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x42014408 - 0x42014448));

    /* bltu positive offset  */
    /* 42010dfa:	00f56a63          	bltu	a0,a5,42010e0e  */
    regs.a0 = regs.a5 = 0x777;
    inst = 0x00f56a63;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 4);
    regs.a0++;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 4);
    regs.a0 = 0x111;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x42010e0e - 0x42010dfa));

    /* bltu negative offset  */
    /* 4200137c:	f79be5e3          	bltu	s7,s9,420012e6  */
    regs.s7 = regs.s9 = 0x777;
    inst = 0xf79be5e3;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 4);
    regs.s7++;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 4);
    regs.s7 = 0x111;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x420012e6 - 0x4200137c));

    /* bge positive offset  */
    /* 4200cf88:	02f55763          	bge	    a0,a5,4200cfb6  */
    regs.a0 = regs.a5 = 0x777;
    inst = 0x02f55763;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x4200cfb6 - 0x4200cf88));
    regs.a0++;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x4200cfb6 - 0x4200cf88));
    regs.a0 = 0x111;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 4);

    /* bge negative offset  */
    /* 420001bc:	fe87d6e3          	bge	a5,s0,420001a8  */
    regs.a5 = regs.s0 = 0x777;
    inst = 0xfe87d6e3;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x420001a8 - 0x420001bc));
    regs.a5++;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x420001a8 - 0x420001bc));
    regs.a5 = 0x111;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 4);

    /* bgeu positive offset  */
    /* 40383552:	02f57d63          	bgeu	a0,a5,4038358c  */
    regs.a0 = regs.a5 = 0x777;
    inst = 0x02f57d63;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x4038358c - 0x40383552));
    regs.a0++;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x4038358c - 0x40383552));
    regs.a0 = 0x111;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 4);

    /* bgeu negative offset  */
    /* 42001178:	fae7fbe3          	bgeu	a5,a4,4200112e  */
    regs.a5 = regs.a4 = 0x777;
    inst = 0xfae7fbe3;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x4200112e - 0x42001178));
    regs.a5++;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x4200112e - 0x42001178));
    regs.a5 = 0x111;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 4);

    /* jal positive offset  */
    /* 4200f872:	016010ef          	jal	ra,42010888  */
    inst = 0x016010ef;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x42010888 - 0x4200f872));

    /* jal negative offset  */
    /* 42015c94:	e31ff0ef          	jal	ra,42015ac4  */
    inst = 0xe31ff0ef;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x42015ac4 - 0x42015c94));

    /* jalr positive offset  */
    /* 4200fee0:	47c080e7          	jalr	1148(ra)  # 40000358 */
    regs.ra = 0x40000358 - 1148;
    inst = 0x47c080e7;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == 0x40000358);

    /* jalr negative offset  */
    /* 4038779e:	8a6080e7          	jalr	-1882(ra) # 40000040  */
    regs.ra = 0x40000040 - (-1882);
    inst = 0x8a6080e7;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == 0x40000040);
}

TEST_CASE("decode rv32c instructions")
{
    uintptr_t pc;
    uint32_t inst;
    uintptr_t inst_addr = (uintptr_t)&inst;

    for (size_t i = 0; i < sizeof(rv32c_nojump)/sizeof(rv32c_nojump[0]); i++) {
        inst = rv32c_nojump[i].inst;
        DEBUG_PRINTF("testing instruction %s\n", rv32i_nojump[i].name);
        CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 2);
    }

    /* beqz positive offset */
    /* 4200db50:	c119                	beqz	a0,4200db56  */
    regs.a0 = 0;
    inst = 0xc119;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x4200db56 - 0x4200db50));
    regs.a0 = 0xff;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 2);

    /* beqz negative offset */
    /* 42014886:	d96d                	beqz	a0,42014878  */
    regs.a0 = 0;
    inst = 0xd96d;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x42014878 - 0x42014886));
    regs.a0 = 0xff;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 2);

    /* bnez positive offset */
    /* 4200ee80:	e165                	bnez	a0,4200ef60  */
    regs.a0 = 0xff;
    inst = 0xe165;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x4200ef60 - 0x4200ee80));
    regs.a0 = 0;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 2);

    /* bnez negative offset */
    /* 4200f472:	ff29                	bnez	a4,4200f3cc  */
    regs.a4 = 0xff;
    inst = 0xff29;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x4200f3cc - 0x4200f472));
    regs.a4 = 0;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 2);

    /* jal positive offset  */
    /* 4200ffb8:	2d5d                	jal	4201066e  */
    inst = 0x2d5d;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x4201066e - 0x4200ffb8));

    /* jal negative offset  */
    /* 42005588:	24cd                	jal	4200586a  */
    inst = 0x24cd;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + (0x4200586a - 0x42005588));

    /* 42010366:	9782                	jalr	a5  */
    regs.a5 = 0x88888888;
    inst = 0x9782;
    CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == 0x88888888);
}

TEST_CASE("decode rv32a instructions")
{
    uintptr_t pc;
    uint32_t inst;
    uintptr_t inst_addr = (uintptr_t)&inst;

    for (size_t i = 0; i < sizeof(rv32a_nojump)/sizeof(rv32a_nojump[0]); i++) {
        inst = rv32i_nojump[i].inst;
        DEBUG_PRINTF("testing instruction %s\n", rv32i_nojump[i].name);
        CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 4);
    }
}

TEST_CASE("decode rv32m instructions")
{
    uintptr_t pc;
    uint32_t inst;
    uintptr_t inst_addr = (uintptr_t)&inst;

    for (size_t i = 0; i < sizeof(rv32m_nojump)/sizeof(rv32m_nojump[0]); i++) {
        inst = rv32i_nojump[i].inst;
        DEBUG_PRINTF("testing instruction %s\n", rv32i_nojump[i].name);
        CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 4);
    }
}
