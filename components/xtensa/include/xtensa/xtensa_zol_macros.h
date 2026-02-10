/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

 /*
 * Zero-overhead loop adjustment helpers for Xtensa assembly code.
 *
 * Used by exception handlers to emulate loop hardware when an exception
 * occurs at LEND so execution resumes at LBEG with LCOUNT decremented.
 */

#ifndef XTENSA_ZOL_MACROS_H
#define XTENSA_ZOL_MACROS_H

#if __XTENSA__
#include "xtensa/config/core-isa.h"
#include "xtensa/config/xt_specreg.h"

/**
 * Adjust loop counter and return address for exceptions at LEND.
 *
 * When an exception occurs on instruction at LEND address of a zero-overhead loop,
 * we must decrement LCOUNT and set EPC back to LBEG so the loop continues
 * iterating after the exception is handled. Otherwise, the loop exits prematurely.
 *
 * if (EPC == LEND && LCOUNT != 0) {
 *     LCOUNT--;
 *     EPC = LBEG;
 * }
 *
 * param[in/out] epc_reg - register containing EPC, updated to LBEG if needed
 * param[in/out] tmp_reg - temporary register for intermediate values
 *
 * Return: Use epc_reg to set corrected EPC value.
 */
.macro XT_ZOL_EPC_LCOUNT_RESTORE epc_reg tmp_reg
    #if XCHAL_HAVE_LOOPS
    rsr     \tmp_reg, XT_REG_LEND
    bne     \epc_reg, \tmp_reg, 1f
    rsr     \tmp_reg, XT_REG_LCOUNT
    beqz    \tmp_reg, 1f
    addi    \tmp_reg, \tmp_reg, -1
    wsr     \tmp_reg, XT_REG_LCOUNT
    rsr     \epc_reg, XT_REG_LBEG
1:
    #endif
.endm

#endif /* __XTENSA__ */

#endif /* XTENSA_ZOL_MACROS_H */
