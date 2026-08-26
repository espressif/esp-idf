/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "soc/soc_caps.h"

#if SOC_CPU_HAS_HWLOOP

    .text
    .align      4

/**
 * @brief Perform a hardware loop with a given number of iterations
 *
 * @param a0 Number of iterations
 */
    .global use_hwlp
    .type use_hwlp, @function
use_hwlp:
    /* The toolchain doesn't support HWLP instructions yet, manually set it up */
    la   a2, start
    csrw 0x7c6, a2
    la   a2, end
    csrw 0x7c7, a2
    csrw 0x7c8, a0
    li   a1, 0
    /* Hardware loops must have at least 8 32-bit instructions or 16 16-bit instructions */
start:
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
end:
    addi a1, a1, 1
    mv   a0, a1
    ret
    .size  use_hwlp, .-use_hwlp

#endif /* SOC_CPU_HAS_HWLOOP */
