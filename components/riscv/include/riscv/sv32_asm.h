/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/*
 * Assembly-only Sv32 helpers. This header intentionally contains no C
 * declarations so it can be included from preprocessed assembly sources.
 */
#if defined(_ASMLANGUAGE) || defined(__ASSEMBLER__)

/** Write a pre-encoded Sv32/Bare value to satp without fencing. */
.macro riscv_sv32_write_satp reg
    csrw    satp, \reg
.endm

#endif
