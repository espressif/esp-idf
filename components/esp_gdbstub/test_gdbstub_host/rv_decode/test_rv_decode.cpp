/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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

struct inst_list_s xesppie_nojump[] = {
    {0x0287805f, "esp.vcmulas.s16.qacc.h"},
    {0x03c0633b, "esp.vcmulas.s16.qacc.h.ld.ip"},
    {0x0300207f, "esp.vcmulas.s16.qacc.h.ld.xp"},
    {0x0283805f, "esp.vcmulas.s16.qacc.l"},
    {0x01c0633b, "esp.vcmulas.s16.qacc.l.ld.ip"},
    {0x0100207f, "esp.vcmulas.s16.qacc.l.ld.xp"},
    {0x0285805f, "esp.vcmulas.s8.qacc.h"},
    {0x02c0633b, "esp.vcmulas.s8.qacc.h.ld.ip"},
    {0x0200207f, "esp.vcmulas.s8.qacc.h.ld.xp"},
    {0x0281805f, "esp.vcmulas.s8.qacc.l"},
    {0x00c0633b, "esp.vcmulas.s8.qacc.l.ld.ip"},
    {0x0000207f, "esp.vcmulas.s8.qacc.l.ld.xp"},
    {0x02c7005f, "esp.vmulas.s16.qacc"},
    {0x02e060bb, "esp.vmulas.s16.qacc.ld.ip"},
    {0x020822ff, "esp.vmulas.s16.qacc.ld.xp"},
    {0x03e060bb, "esp.vmulas.s16.qacc.st.ip"},
    {0x030822ff, "esp.vmulas.s16.qacc.st.xp"},
    {0x02c3005f, "esp.vmulas.s16.xacc"},
    {0x00e060bb, "esp.vmulas.s16.xacc.ld.ip"},
    {0x000822ff, "esp.vmulas.s16.xacc.ld.xp"},
    {0x01e060bb, "esp.vmulas.s16.xacc.st.ip"},
    {0x010822ff, "esp.vmulas.s16.xacc.st.xp"},
    {0x02c5005f, "esp.vmulas.s8.qacc"},
    {0x026060bb, "esp.vmulas.s8.qacc.ld.ip"},
    {0x020022ff, "esp.vmulas.s8.qacc.ld.xp"},
    {0x036060bb, "esp.vmulas.s8.qacc.st.ip"},
    {0x030022ff, "esp.vmulas.s8.qacc.st.xp"},
    {0x02c1005f, "esp.vmulas.s8.xacc"},
    {0x006060bb, "esp.vmulas.s8.xacc.ld.ip"},
    {0x000022ff, "esp.vmulas.s8.xacc.ld.xp"},
    {0x016060bb, "esp.vmulas.s8.xacc.st.ip"},
    {0x010022ff, "esp.vmulas.s8.xacc.st.xp"},
    {0x02c6005f, "esp.vmulas.u16.qacc"},
    {0x02a060bb, "esp.vmulas.u16.qacc.ld.ip"},
    {0x020820ff, "esp.vmulas.u16.qacc.ld.xp"},
    {0x03a060bb, "esp.vmulas.u16.qacc.st.ip"},
    {0x030820ff, "esp.vmulas.u16.qacc.st.xp"},
    {0x02c2005f, "esp.vmulas.u16.xacc"},
    {0x00a060bb, "esp.vmulas.u16.xacc.ld.ip"},
    {0x000820ff, "esp.vmulas.u16.xacc.ld.xp"},
    {0x01a060bb, "esp.vmulas.u16.xacc.st.ip"},
    {0x010820ff, "esp.vmulas.u16.xacc.st.xp"},
    {0x02c4005f, "esp.vmulas.u8.qacc"},
    {0x022060bb, "esp.vmulas.u8.qacc.ld.ip"},
    {0x020020ff, "esp.vmulas.u8.qacc.ld.xp"},
    {0x032060bb, "esp.vmulas.u8.qacc.st.ip"},
    {0x030020ff, "esp.vmulas.u8.qacc.st.xp"},
    {0x02c0005f, "esp.vmulas.u8.xacc"},
    {0x002060bb, "esp.vmulas.u8.xacc.ld.ip"},
    {0x000020ff, "esp.vmulas.u8.xacc.ld.xp"},
    {0x012060bb, "esp.vmulas.u8.xacc.st.ip"},
    {0x010020ff, "esp.vmulas.u8.xacc.st.xp"},
    {0x006061bb, "esp.vmulas.s16.qacc.ldbc.incp"},
    {0x002061bb, "esp.vmulas.s8.qacc.ldbc.incp"},
    {0x004061bb, "esp.vmulas.u16.qacc.ldbc.incp"},
    {0x000061bb, "esp.vmulas.u8.qacc.ldbc.incp"},
    {0x02f0005f, "esp.vsmulas.s16.qacc"},
    {0x038063bb, "esp.vsmulas.s16.qacc.ld.incp"},
    {0x02b0005f, "esp.vsmulas.s8.qacc"},
    {0x018063bb, "esp.vsmulas.s8.qacc.ld.incp"},
    {0x02d0005f, "esp.vsmulas.u16.qacc"},
    {0x028063bb, "esp.vsmulas.u16.qacc.ld.incp"},
    {0x0290005f, "esp.vsmulas.u8.qacc"},
    {0x008063bb, "esp.vsmulas.u8.qacc.ld.incp"},
    {0x0006245f, "esp.cmul.s16"},
    {0x00c0403f, "esp.cmul.s16.ld.incp"},
    {0x02c0403f, "esp.cmul.s16.st.incp"},
    {0x0002245f, "esp.cmul.s8"},
    {0x0040403f, "esp.cmul.s8.ld.incp"},
    {0x0240403f, "esp.cmul.s8.st.incp"},
    {0x0004245f, "esp.cmul.u16"},
    {0x0080403f, "esp.cmul.u16.ld.incp"},
    {0x0280403f, "esp.cmul.u16.st.incp"},
    {0x0000245f, "esp.cmul.u8"},
    {0x0000403f, "esp.cmul.u8.ld.incp"},
    {0x0200403f, "esp.cmul.u8.st.incp"},
    {0x90c0505b, "esp.max.s16.a"},
    {0x90a0505b, "esp.max.s32.a"},
    {0x9040505b, "esp.max.s8.a"},
    {0x9080505b, "esp.max.u16.a"},
    {0x9020505b, "esp.max.u32.a"},
    {0x9000505b, "esp.max.u8.a"},
    {0x90d0505b, "esp.min.s16.a"},
    {0x90b0505b, "esp.min.s32.a"},
    {0x9050505b, "esp.min.s8.a"},
    {0x9090505b, "esp.min.u16.a"},
    {0x9030505b, "esp.min.u32.a"},
    {0x9010505b, "esp.min.u8.a"},
    {0x8000105b, "esp.vabs.16"},
    {0x8000085b, "esp.vabs.32"},
    {0x8000005b, "esp.vabs.8"},
    {0x0284065f, "esp.vadd.s16"},
    {0x0108603b, "esp.vadd.s16.ld.incp"},
    {0x0308603b, "esp.vadd.s16.st.incp"},
    {0x0284055f, "esp.vadd.s32"},
    {0x0100613b, "esp.vadd.s32.ld.incp"},
    {0x0300613b, "esp.vadd.s32.st.incp"},
    {0x0280065f, "esp.vadd.s8"},
    {0x0008603b, "esp.vadd.s8.ld.incp"},
    {0x0208603b, "esp.vadd.s8.st.incp"},
    {0x0284045f, "esp.vadd.u16"},
    {0x0100603b, "esp.vadd.u16.ld.incp"},
    {0x0300603b, "esp.vadd.u16.st.incp"},
    {0x0280055f, "esp.vadd.u32"},
    {0x0000613b, "esp.vadd.u32.ld.incp"},
    {0x0200613b, "esp.vadd.u32.st.incp"},
    {0x0280045f, "esp.vadd.u8"},
    {0x0000603b, "esp.vadd.u8.ld.incp"},
    {0x0200603b, "esp.vadd.u8.st.incp"},
    {0x0000505b, "esp.vclamp.s16"},
    {0x0006ac5f, "esp.vmax.s16"},
    {0x0068403f, "esp.vmax.s16.ld.incp"},
    {0x00e8403f, "esp.vmax.s16.st.incp"},
    {0x0005ac5f, "esp.vmax.s32"},
    {0x0058403f, "esp.vmax.s32.ld.incp"},
    {0x00d8403f, "esp.vmax.s32.st.incp"},
    {0x0002ac5f, "esp.vmax.s8"},
    {0x0028403f, "esp.vmax.s8.ld.incp"},
    {0x00a8403f, "esp.vmax.s8.st.incp"},
    {0x0004ac5f, "esp.vmax.u16"},
    {0x0048403f, "esp.vmax.u16.ld.incp"},
    {0x00c8403f, "esp.vmax.u16.st.incp"},
    {0x0001ac5f, "esp.vmax.u32"},
    {0x0018403f, "esp.vmax.u32.ld.incp"},
    {0x0098403f, "esp.vmax.u32.st.incp"},
    {0x0000ac5f, "esp.vmax.u8"},
    {0x0008403f, "esp.vmax.u8.ld.incp"},
    {0x0088403f, "esp.vmax.u8.st.incp"},
    {0x00063c5f, "esp.vmin.s16"},
    {0x0168403f, "esp.vmin.s16.ld.incp"},
    {0x01e8403f, "esp.vmin.s16.st.incp"},
    {0x00053c5f, "esp.vmin.s32"},
    {0x0158403f, "esp.vmin.s32.ld.incp"},
    {0x01d8403f, "esp.vmin.s32.st.incp"},
    {0x00023c5f, "esp.vmin.s8"},
    {0x0128403f, "esp.vmin.s8.ld.incp"},
    {0x01a8403f, "esp.vmin.s8.st.incp"},
    {0x00043c5f, "esp.vmin.u16"},
    {0x0148403f, "esp.vmin.u16.ld.incp"},
    {0x01c8403f, "esp.vmin.u16.st.incp"},
    {0x00013c5f, "esp.vmin.u32"},
    {0x0118403f, "esp.vmin.u32.ld.incp"},
    {0x0198403f, "esp.vmin.u32.st.incp"},
    {0x00003c5f, "esp.vmin.u8"},
    {0x0108403f, "esp.vmin.u8.ld.incp"},
    {0x0188403f, "esp.vmin.u8.st.incp"},
    {0x0006bc5f, "esp.vmul.s16"},
    {0x0368403f, "esp.vmul.s16.ld.incp"},
    {0x0283045f, "esp.vmul.s16.s8xs8"},
    {0x03e8403f, "esp.vmul.s16.st.incp"},
    {0x0287045f, "esp.vmul.s32.s16xs16"},
    {0x0002bc5f, "esp.vmul.s8"},
    {0x0328403f, "esp.vmul.s8.ld.incp"},
    {0x03a8403f, "esp.vmul.s8.st.incp"},
    {0x0004bc5f, "esp.vmul.u16"},
    {0x0348403f, "esp.vmul.u16.ld.incp"},
    {0x03c8403f, "esp.vmul.u16.st.incp"},
    {0x0000bc5f, "esp.vmul.u8"},
    {0x0308403f, "esp.vmul.u8.ld.incp"},
    {0x0388403f, "esp.vmul.u8.st.incp"},
    {0x02a0605f, "esp.vprelu.s16"},
    {0x0220605f, "esp.vprelu.s8"},
    {0x82005c5b, "esp.vrelu.s16"},
    {0x8200585b, "esp.vrelu.s8"},
    {0x1a80025f, "esp.vsadds.s16"},
    {0x0a80025f, "esp.vsadds.s8"},
    {0x1280025f, "esp.vsadds.u16"},
    {0x0280025f, "esp.vsadds.u8"},
    {0x1800583b, "esp.vsat.s16"},
    {0x1800543b, "esp.vsat.s32"},
    {0x1800483b, "esp.vsat.s8"},
    {0x1800503b, "esp.vsat.u16"},
    {0x1800443b, "esp.vsat.u32"},
    {0x1800403b, "esp.vsat.u8"},
    {0x1e80025f, "esp.vssubs.s16"},
    {0x0e80025f, "esp.vssubs.s8"},
    {0x1680025f, "esp.vssubs.u16"},
    {0x0680025f, "esp.vssubs.u8"},
    {0x028406df, "esp.vsub.s16"},
    {0x0188613b, "esp.vsub.s16.ld.incp"},
    {0x0388613b, "esp.vsub.s16.st.incp"},
    {0x028405df, "esp.vsub.s32"},
    {0x0100633b, "esp.vsub.s32.ld.incp"},
    {0x0300633b, "esp.vsub.s32.st.incp"},
    {0x028006df, "esp.vsub.s8"},
    {0x0088613b, "esp.vsub.s8.ld.incp"},
    {0x0288613b, "esp.vsub.s8.st.incp"},
    {0x028404df, "esp.vsub.u16"},
    {0x0180613b, "esp.vsub.u16.ld.incp"},
    {0x0380613b, "esp.vsub.u16.st.incp"},
    {0x028005df, "esp.vsub.u32"},
    {0x0000633b, "esp.vsub.u32.ld.incp"},
    {0x0200633b, "esp.vsub.u32.st.incp"},
    {0x028004df, "esp.vsub.u8"},
    {0x0080613b, "esp.vsub.u8.ld.incp"},
    {0x0280613b, "esp.vsub.u8.st.incp"},
    {0x04840433, "esp.addx2"},
    {0x08840433, "esp.addx4"},
    {0x40842433, "esp.sat"},
    {0x44840433, "esp.subx2"},
    {0x48840433, "esp.subx4"},
    {0x0004205f, "esp.andq"},
    {0x0006205f, "esp.notq"},
    {0x0000205f, "esp.orq"},
    {0x0002205f, "esp.xorq"},
    {0x0001b45f, "esp.vcmp.eq.s16"},
    {0x00012c5f, "esp.vcmp.eq.s32"},
    {0x0000b45f, "esp.vcmp.eq.s8"},
    {0x0001345f, "esp.vcmp.eq.u16"},
    {0x00002c5f, "esp.vcmp.eq.u32"},
    {0x0000345f, "esp.vcmp.eq.u8"},
    {0x0005b45f, "esp.vcmp.gt.s16"},
    {0x00052c5f, "esp.vcmp.gt.s32"},
    {0x0004b45f, "esp.vcmp.gt.s8"},
    {0x0005345f, "esp.vcmp.gt.u16"},
    {0x00042c5f, "esp.vcmp.gt.u32"},
    {0x0004345f, "esp.vcmp.gt.u8"},
    {0x0003b45f, "esp.vcmp.lt.s16"},
    {0x00032c5f, "esp.vcmp.lt.s32"},
    {0x0002b45f, "esp.vcmp.lt.s8"},
    {0x0003345f, "esp.vcmp.lt.u16"},
    {0x00022c5f, "esp.vcmp.lt.u32"},
    {0x0002345f, "esp.vcmp.lt.u8"},
    {0x1060005b, "esp.mov.s16.qacc"},
    {0x1020005b, "esp.mov.s8.qacc"},
    {0x1040005b, "esp.mov.u16.qacc"},
    {0x1000005b, "esp.mov.u8.qacc"},
    {0x00c0005f, "esp.movi.16.a"},
    {0x80e0005f, "esp.movi.16.q"},
    {0x8080005f, "esp.movi.32.a"},
    {0x8090005f, "esp.movi.32.q"},
    {0x0080005f, "esp.movi.8.a"},
    {0x80a0005f, "esp.movi.8.q"},
    {0x80d0005f, "esp.movx.r.cfg"},
    {0x84d0005f, "esp.movx.r.fft.bit.width"},
    {0x8cd0005f, "esp.movx.r.perf"},
    {0x80b0005f, "esp.movx.r.sar"},
    {0x88b0005f, "esp.movx.r.sar.bytes"},
    {0x8cb0005f, "esp.movx.r.xacc.h"},
    {0x84b0005f, "esp.movx.r.xacc.l"},
    {0x90d0005f, "esp.movx.w.cfg"},
    {0x94d0005f, "esp.movx.w.fft.bit.width"},
    {0x9cd0005f, "esp.movx.w.perf"},
    {0x90b0005f, "esp.movx.w.sar"},
    {0x98b0005f, "esp.movx.w.sar.bytes"},
    {0x9cb0005f, "esp.movx.w.xacc.h"},
    {0x94b0005f, "esp.movx.w.xacc.l"},
    {0x1800585b, "esp.vext.s16"},
    {0x0800585b, "esp.vext.s8"},
    {0x1000585b, "esp.vext.u16"},
    {0x0000585b, "esp.vext.u8"},
    {0x0286005f, "esp.vunzip.16"},
    {0x0284805f, "esp.vunzip.32"},
    {0x0284005f, "esp.vunzip.8"},
    {0x00c04c5b, "esp.vunzipt.16"},
    {0x00804c5b, "esp.vunzipt.8"},
    {0x0282005f, "esp.vzip.16"},
    {0x0280805f, "esp.vzip.32"},
    {0x0280005f, "esp.vzip.8"},
    {0x00404c5b, "esp.vzipt.16"},
    {0x00004c5b, "esp.vzipt.8"},
    {0x0040005b, "esp.zero.q"},
    {0x0000025b, "esp.zero.qacc"},
    {0x0000005b, "esp.zero.xacc"},
    {0x0000007b, "esp.fft.ams.s16.ld.incp"},
    {0x0000207b, "esp.fft.ams.s16.ld.incp.uaup"},
    {0x0000607b, "esp.fft.ams.s16.ld.r32.decp"},
    {0x0000203f, "esp.fft.ams.s16.st.incp"},
    {0x1000025b, "esp.fft.bitrev"},
    {0x0000003f, "esp.fft.cmul.s16.ld.xp"},
    {0x0000007f, "esp.fft.cmul.s16.st.xp"},
    {0x0281045f, "esp.fft.r2bf.s16"},
    {0x0200605f, "esp.fft.r2bf.s16.st.incp"},
    {0x0000203b, "esp.fft.vst.r32.decp"},
    {0x8800203b, "esp.ld.128.usar.ip"},
    {0x8000405f, "esp.ld.128.usar.xp"},
    {0xa000433b, "esp.ld.xacc.ip"},
    {0xe08040bb, "esp.ldqa.s16.128.ip"},
    {0x1300535b, "esp.ldqa.s16.128.xp"},
    {0x608040bb, "esp.ldqa.s8.128.ip"},
    {0x1300515b, "esp.ldqa.s8.128.xp"},
    {0xa08040bb, "esp.ldqa.u16.128.ip"},
    {0x1300525b, "esp.ldqa.u16.128.xp"},
    {0x208040bb, "esp.ldqa.u8.128.ip"},
    {0x1300505b, "esp.ldqa.u8.128.xp"},
    {0xc600203b, "esp.vldbc.16.ip"},
    {0x9600405f, "esp.vldbc.16.xp"},
    {0x8e00203b, "esp.vldbc.32.ip"},
    {0x8e00405f, "esp.vldbc.32.xp"},
    {0x4600203b, "esp.vldbc.8.ip"},
    {0x8600405f, "esp.vldbc.8.xp"},
    {0xc880403b, "esp.vldext.s16.ip"},
    {0xf000605f, "esp.vldext.s16.xp"},
    {0x4880403b, "esp.vldext.s8.ip"},
    {0x7000605f, "esp.vldext.s8.xp"},
    {0x8880403b, "esp.vldext.u16.ip"},
    {0xb000605f, "esp.vldext.u16.xp"},
    {0x0880403b, "esp.vldext.u8.ip"},
    {0x3000605f, "esp.vldext.u8.xp"},
    {0x2800403b, "esp.vldhbc.16.incp"},
    {0x4080403b, "esp.ld.qacc.h.h.128.ip"},
    {0x6080403b, "esp.ld.qacc.h.l.128.ip"},
    {0x0080403b, "esp.ld.qacc.l.h.128.ip"},
    {0x2080403b, "esp.ld.qacc.l.l.128.ip"},
    {0x6000413b, "esp.ld.ua.state.ip"},
    {0x0080205f, "esp.ldxq.32"},
    {0xc080403b, "esp.st.qacc.h.h.128.ip"},
    {0xe080403b, "esp.st.qacc.h.l.128.ip"},
    {0x8080403b, "esp.st.qacc.l.h.128.ip"},
    {0xa080403b, "esp.st.qacc.l.l.128.ip"},
    {0xe000413b, "esp.st.ua.state.ip"},
    {0x8080205f, "esp.stxq.32"},
    {0x4200203b, "esp.vld.128.ip"},
    {0x8200405f, "esp.vld.128.xp"},
    {0x6400203b, "esp.vld.h.64.ip"},
    {0x8c00405f, "esp.vld.h.64.xp"},
    {0x2400203b, "esp.vld.l.64.ip"},
    {0x8400405f, "esp.vld.l.64.xp"},
    {0xc200203b, "esp.vst.128.ip"},
    {0x9200405f, "esp.vst.128.xp"},
    {0xe400203b, "esp.vst.h.64.ip"},
    {0x9c00405f, "esp.vst.h.64.xp"},
    {0xa400203b, "esp.vst.l.64.ip"},
    {0x9400405f, "esp.vst.l.64.xp"},
    {0x0000485b, "esp.slci.2q"},
    {0x0000405f, "esp.slcxxp.2q"},
    {0x8024005b, "esp.src.q"},
    {0x8000213b, "esp.src.q.ld.ip"},
    {0x0000003b, "esp.src.q.ld.xp"},
    {0x8024105b, "esp.src.q.qup"},
    {0x0080485b, "esp.srci.2q"},
    {0x9864005b, "esp.srcmb.s16.q.qacc"},
    {0xd800203b, "esp.srcmb.s16.qacc"},
    {0x8864005b, "esp.srcmb.s8.q.qacc"},
    {0x5800203b, "esp.srcmb.s8.qacc"},
    {0x9064005b, "esp.srcmb.u16.q.qacc"},
    {0x9800203b, "esp.srcmb.u16.qacc"},
    {0x8064005b, "esp.srcmb.u8.q.qacc"},
    {0x1800203b, "esp.srcmb.u8.qacc"},
    {0x0000405b, "esp.srcq.128.st.incp"},
    {0x0000445f, "esp.srcxxp.2q"},
    {0x94f0005f, "esp.srs.s.xacc"},
    {0x84f0005f, "esp.srs.u.xacc"},
    {0x8004005b, "esp.vsl.32"},
    {0x0020005f, "esp.vsld.16"},
    {0x0010005f, "esp.vsld.32"},
    {0x0000005f, "esp.vsld.8"},
    {0x8004035b, "esp.vsr.s32"},
    {0x8004015b, "esp.vsr.u32"},
    {0x0060005f, "esp.vsrd.16"},
    {0x0050005f, "esp.vsrd.32"},
    {0x0040005f, "esp.vsrd.8"},
    {0xe00041bb, "esp.st.s.xacc.ip"},
    {0x600041bb, "esp.st.u.xacc.ip"},
    {0x0000502b, "esp.lp.setupi"},
    {0x0004402b, "esp.lp.setup"},
    {0x0000002b, "esp.lp.starti"},
    {0x0000102b, "esp.lp.endi"},
    {0x0000302b, "esp.lp.counti"},
    {0x0004202b, "esp.lp.count"}};


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

TEST_CASE("decode xesppie instructions")
{
    uintptr_t pc;
    uint32_t inst;
    uintptr_t inst_addr = (uintptr_t)&inst;

    for (size_t i = 0; i < sizeof(xesppie_nojump)/sizeof(xesppie_nojump[0]); i++) {
        inst = xesppie_nojump[i].inst;
        DEBUG_PRINTF("testing instruction %s\n", xesppie_nojump[i].name);
        CHECK(rv_compute_next_pc(temp_regs_frame, inst_addr) == inst_addr + 4);
    }
}
