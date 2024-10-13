/*
 * SPDX-FileCopyrightText: 2015-2021  Claire Xenia Wolf <claire@yosyshq.com>
 * SPDX-FileContributor: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This header file defines custom instructions for interrupt handling on the
 * ULP RISC-V. The architecture of the processor and therefore, the interrupt
 * handling is based on the PicoRV32 CPU. Details about the operations are
 * available at https://github.com/YosysHQ/picorv32#custom-instructions-for-irq-handling
 */

/* Define encoding for all general purpose RISC-V registers */
#define regnum_zero 0
#define regnum_ra   1
#define regnum_sp   2
#define regnum_gp   3
#define regnum_tp   4
#define regnum_t0   5
#define regnum_t1   6
#define regnum_t2   7
#define regnum_s0   8
#define regnum_s1   9
#define regnum_a0  10
#define regnum_a1  11
#define regnum_a2  12
#define regnum_a3  13
#define regnum_a4  14
#define regnum_a5  15
#define regnum_a6  16
#define regnum_a7  17
#define regnum_s2  18
#define regnum_s3  19
#define regnum_s4  20
#define regnum_s5  21
#define regnum_s6  22
#define regnum_s7  23
#define regnum_s8  24
#define regnum_s9  25
#define regnum_s10 26
#define regnum_s11 27
#define regnum_t3  28
#define regnum_t4  29
#define regnum_t5  30
#define regnum_t6  31

/* Define encoding for special interrupt handling registers, viz., q0, q1, q2 and q3 */
#define regnum_q0   0
#define regnum_q1   1
#define regnum_q2   2
#define regnum_q3   3

/* All custom interrupt handling instructions follow the standard R-type instruction format from RISC-V ISA
 * with the same opcode of custom0 (0001011).
 */
#define r_type_insn(_f7, _rs2, _rs1, _f3, _rd, _opc) \
.word (((_f7) << 25) | ((_rs2) << 20) | ((_rs1) << 15) | ((_f3) << 12) | ((_rd) << 7) | ((_opc) << 0))

/**
 * Instruction: getq rd, qs
 * Description: This instruction copies the value of Qx into a general purpose register rd
 */
#define getq_insn(_rd, _qs) \
r_type_insn(0b0000000, 0, regnum_ ## _qs, 0b100, regnum_ ## _rd, 0b0001011)

/**
 * Instruction: setq qd, rs
 * Description: This instruction copies the value of general purpose register rs to Qx
 */
#define setq_insn(_qd, _rs) \
r_type_insn(0b0000001, 0, regnum_ ## _rs, 0b010, regnum_ ## _qd, 0b0001011)

/**
 * Instruction: retirq
 * Description: This instruction copies the value of Q0 to CPU PC, and renables interrupts
 */
#define retirq_insn() \
r_type_insn(0b0000010, 0, 0, 0b000, 0, 0b0001011)

/**
 * Instruction: maskirq rd, rs
 * Description: This instruction copies the value of the register IRQ Mask to the register rd, and copies the value
 *              of register rs to to IRQ mask.
 */
#define maskirq_insn(_rd, _rs) \
r_type_insn(0b0000011, 0, regnum_ ## _rs, 0b110, regnum_ ## _rd, 0b0001011)

#ifdef __cplusplus
}
#endif
