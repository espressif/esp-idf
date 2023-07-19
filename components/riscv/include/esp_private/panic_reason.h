/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

/* Since riscv does not replace mcause with "pseudo_reason" as it xtensa does
 * PANIC_RSN_* defined with original interrupt numbers to make it work in
 * common code
 */
#define PANIC_RSN_INTWDT_CPU0 ETS_INT_WDT_INUM

//TODO: IDF-7511
#if SOC_CPU_CORES_NUM > 1
#define PANIC_RSN_INTWDT_CPU1 ETS_INT_WDT_INUM
#endif
#define PANIC_RSN_CACHEERR    3
