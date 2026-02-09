/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
    #include "soc/soc_caps.h"

#if SOC_CPU_HAS_DSP

    #include "riscv/csr_dsp.h"

    /**
     * @brief Let's allow a small subset of registers to be used in the macro below
     */
    .set regnum_a0, 10
    .set regnum_a1, 11
    .set regnum_a2, 12
    .set regnum_a3, 13
    .set regnum_a4, 14
    .set regnum_a5, 15
    .set regnum_a6, 16
    .set regnum_a7, 17

    /**
     * @brief The toolchain doesn't support DSP instructions yet, define it as a constant.
     */
    .macro macs32  _rs1, _rs2
        .word ( ( regnum_\_rs1 << 15 ) | ( regnum_\_rs2 << 20) | 0b100<<12 | 0b1011011 )
    .endm

    .global g_dsp_constant

    /**
     * @brief Multiply the given ID by the global constant defined as g_dsp_constant.
     * NOTE: The goal of the function is not to be fast and efficient, on the contrary, it needs to be
     * slow and long so that it will be preempted.
     *
     * Parameters:
     *  a0: 32-bit id
     *
     * Returns:
     *  a0: multiplied value
     */
    .globl dsp_id_mul
dsp_id_mul:
    csrw 0x809, zero    // SAR = 0
    csrw 0x806, zero    // XACC_L
    csrw 0x807, zero    // XACC_H
    li a1, 1
    /* Load the constant in a2 */
    lw a2, (g_dsp_constant)
1:
    // Perform ACC += a0 (=id) * a1 (=1)
    macs32 a1, a0
    addi a2, a2, -1
    bnez a2, 1b
    // Get the lowest bits
    csrr a0, 0x806    // XACC_L
    ret


#endif // SOC_CPU_HAS_DSP
