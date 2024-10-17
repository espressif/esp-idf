/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/soc_caps.h"

/* Need a way to signal which core caused the INT WDT like we do with EXCAUSE on xtensa.
   Choosing a large number that is unlikely to conflict with any actual riscv mcauses
   bit 12 and above are always zero on the CPU used by P4
 */
#define PANIC_RSN_INTWDT_CPU0 ETS_INT_WDT_INUM
#if SOC_CPU_CORES_NUM > 1
#define PANIC_RSN_INTWDT_CPU1_FLAG (1 << 12)
#define PANIC_RSN_INTWDT_CPU1 (PANIC_RSN_INTWDT_CPU1_FLAG | ETS_INT_WDT_INUM)
#endif

#define PANIC_RSN_CACHEERR    3

#define MCAUSE_ILLIGAL_INSTRUCTION_ACCESS    1
#define MCAUSE_ILLEGAL_INSTRUCTION           2
#define MCAUSE_LOAD_ACCESS_FAULT             5
#define MCAUSE_INVALID_VALUE                 0xDEADC0DE // Frame mcause value was written by SW to indicate no useful info, e.g. during a register dump without a crash
