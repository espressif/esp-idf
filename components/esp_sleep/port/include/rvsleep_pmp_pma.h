/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "soc/soc_caps.h"

#if SOC_CPU_PMP_EXTENDED_REGIONS && !CONFIG_ESP32P4_SELECTS_REV_LESS_V3
#define RV_SLEEP_PMP_REGION_NUM     32
#else
#define RV_SLEEP_PMP_REGION_NUM     16
#endif
#define RV_SLEEP_PMA_REGION_NUM     16

#if !defined(_ASMLANGUAGE) && !defined(__ASSEMBLER__)

#include "riscv/csr.h"

#if RV_SLEEP_PMP_REGION_NUM == 32
#define RV_SLEEP_SAVE_PMPADDR_EXTENDED(addr) \
    (addr)[16] = RV_READ_CSR(pmpaddr16); \
    (addr)[17] = RV_READ_CSR(pmpaddr17); \
    (addr)[18] = RV_READ_CSR(pmpaddr18); \
    (addr)[19] = RV_READ_CSR(pmpaddr19); \
    (addr)[20] = RV_READ_CSR(pmpaddr20); \
    (addr)[21] = RV_READ_CSR(pmpaddr21); \
    (addr)[22] = RV_READ_CSR(pmpaddr22); \
    (addr)[23] = RV_READ_CSR(pmpaddr23); \
    (addr)[24] = RV_READ_CSR(pmpaddr24); \
    (addr)[25] = RV_READ_CSR(pmpaddr25); \
    (addr)[26] = RV_READ_CSR(pmpaddr26); \
    (addr)[27] = RV_READ_CSR(pmpaddr27); \
    (addr)[28] = RV_READ_CSR(pmpaddr28); \
    (addr)[29] = RV_READ_CSR(pmpaddr29); \
    (addr)[30] = RV_READ_CSR(pmpaddr30); \
    (addr)[31] = RV_READ_CSR(pmpaddr31);
#define RV_SLEEP_SAVE_PMPCFG_EXTENDED(cfg) \
    (cfg)[4] = RV_READ_CSR(pmpcfg4); \
    (cfg)[5] = RV_READ_CSR(pmpcfg5); \
    (cfg)[6] = RV_READ_CSR(pmpcfg6); \
    (cfg)[7] = RV_READ_CSR(pmpcfg7);
#define RV_SLEEP_RESTORE_PMPADDR_EXTENDED(addr) \
    RV_WRITE_CSR(pmpaddr16, (addr)[16]); \
    RV_WRITE_CSR(pmpaddr17, (addr)[17]); \
    RV_WRITE_CSR(pmpaddr18, (addr)[18]); \
    RV_WRITE_CSR(pmpaddr19, (addr)[19]); \
    RV_WRITE_CSR(pmpaddr20, (addr)[20]); \
    RV_WRITE_CSR(pmpaddr21, (addr)[21]); \
    RV_WRITE_CSR(pmpaddr22, (addr)[22]); \
    RV_WRITE_CSR(pmpaddr23, (addr)[23]); \
    RV_WRITE_CSR(pmpaddr24, (addr)[24]); \
    RV_WRITE_CSR(pmpaddr25, (addr)[25]); \
    RV_WRITE_CSR(pmpaddr26, (addr)[26]); \
    RV_WRITE_CSR(pmpaddr27, (addr)[27]); \
    RV_WRITE_CSR(pmpaddr28, (addr)[28]); \
    RV_WRITE_CSR(pmpaddr29, (addr)[29]); \
    RV_WRITE_CSR(pmpaddr30, (addr)[30]); \
    RV_WRITE_CSR(pmpaddr31, (addr)[31]);
#define RV_SLEEP_RESTORE_PMPCFG_EXTENDED(cfg) \
    RV_WRITE_CSR(pmpcfg4, (cfg)[4]); \
    RV_WRITE_CSR(pmpcfg5, (cfg)[5]); \
    RV_WRITE_CSR(pmpcfg6, (cfg)[6]); \
    RV_WRITE_CSR(pmpcfg7, (cfg)[7]);
#else
#define RV_SLEEP_SAVE_PMPADDR_EXTENDED(addr)
#define RV_SLEEP_SAVE_PMPCFG_EXTENDED(cfg)
#define RV_SLEEP_RESTORE_PMPADDR_EXTENDED(addr)
#define RV_SLEEP_RESTORE_PMPCFG_EXTENDED(cfg)
#endif

#define RV_SLEEP_SAVE_PMP_REGS(addr, cfg) \
    do {                                     \
        (addr)[0] = RV_READ_CSR(pmpaddr0);   \
        (addr)[1] = RV_READ_CSR(pmpaddr1);   \
        (addr)[2] = RV_READ_CSR(pmpaddr2);   \
        (addr)[3] = RV_READ_CSR(pmpaddr3);   \
        (addr)[4] = RV_READ_CSR(pmpaddr4);   \
        (addr)[5] = RV_READ_CSR(pmpaddr5);   \
        (addr)[6] = RV_READ_CSR(pmpaddr6);   \
        (addr)[7] = RV_READ_CSR(pmpaddr7);   \
        (addr)[8] = RV_READ_CSR(pmpaddr8);   \
        (addr)[9] = RV_READ_CSR(pmpaddr9);   \
        (addr)[10] = RV_READ_CSR(pmpaddr10); \
        (addr)[11] = RV_READ_CSR(pmpaddr11); \
        (addr)[12] = RV_READ_CSR(pmpaddr12); \
        (addr)[13] = RV_READ_CSR(pmpaddr13); \
        (addr)[14] = RV_READ_CSR(pmpaddr14); \
        (addr)[15] = RV_READ_CSR(pmpaddr15); \
        RV_SLEEP_SAVE_PMPADDR_EXTENDED(addr) \
        (cfg)[0] = RV_READ_CSR(pmpcfg0);     \
        (cfg)[1] = RV_READ_CSR(pmpcfg1);     \
        (cfg)[2] = RV_READ_CSR(pmpcfg2);     \
        (cfg)[3] = RV_READ_CSR(pmpcfg3);     \
        RV_SLEEP_SAVE_PMPCFG_EXTENDED(cfg)   \
    } while (0)

#define RV_SLEEP_RESTORE_PMP_REGS(addr, cfg) \
    do {                                        \
        RV_WRITE_CSR(pmpaddr0, (addr)[0]);      \
        RV_WRITE_CSR(pmpaddr1, (addr)[1]);      \
        RV_WRITE_CSR(pmpaddr2, (addr)[2]);      \
        RV_WRITE_CSR(pmpaddr3, (addr)[3]);      \
        RV_WRITE_CSR(pmpaddr4, (addr)[4]);      \
        RV_WRITE_CSR(pmpaddr5, (addr)[5]);      \
        RV_WRITE_CSR(pmpaddr6, (addr)[6]);      \
        RV_WRITE_CSR(pmpaddr7, (addr)[7]);      \
        RV_WRITE_CSR(pmpaddr8, (addr)[8]);      \
        RV_WRITE_CSR(pmpaddr9, (addr)[9]);      \
        RV_WRITE_CSR(pmpaddr10, (addr)[10]);    \
        RV_WRITE_CSR(pmpaddr11, (addr)[11]);    \
        RV_WRITE_CSR(pmpaddr12, (addr)[12]);    \
        RV_WRITE_CSR(pmpaddr13, (addr)[13]);    \
        RV_WRITE_CSR(pmpaddr14, (addr)[14]);    \
        RV_WRITE_CSR(pmpaddr15, (addr)[15]);    \
        RV_SLEEP_RESTORE_PMPADDR_EXTENDED(addr) \
        RV_WRITE_CSR(pmpcfg0, (cfg)[0]);        \
        RV_WRITE_CSR(pmpcfg1, (cfg)[1]);        \
        RV_WRITE_CSR(pmpcfg2, (cfg)[2]);        \
        RV_WRITE_CSR(pmpcfg3, (cfg)[3]);        \
        RV_SLEEP_RESTORE_PMPCFG_EXTENDED(cfg)   \
    } while (0)

#define RV_SLEEP_SAVE_PMA_REGS(addr, cfg) \
    do {                                           \
        (addr)[0] = RV_READ_CSR(CSR_PMAADDR(0));   \
        (addr)[1] = RV_READ_CSR(CSR_PMAADDR(1));   \
        (addr)[2] = RV_READ_CSR(CSR_PMAADDR(2));   \
        (addr)[3] = RV_READ_CSR(CSR_PMAADDR(3));   \
        (addr)[4] = RV_READ_CSR(CSR_PMAADDR(4));   \
        (addr)[5] = RV_READ_CSR(CSR_PMAADDR(5));   \
        (addr)[6] = RV_READ_CSR(CSR_PMAADDR(6));   \
        (addr)[7] = RV_READ_CSR(CSR_PMAADDR(7));   \
        (addr)[8] = RV_READ_CSR(CSR_PMAADDR(8));   \
        (addr)[9] = RV_READ_CSR(CSR_PMAADDR(9));   \
        (addr)[10] = RV_READ_CSR(CSR_PMAADDR(10)); \
        (addr)[11] = RV_READ_CSR(CSR_PMAADDR(11)); \
        (addr)[12] = RV_READ_CSR(CSR_PMAADDR(12)); \
        (addr)[13] = RV_READ_CSR(CSR_PMAADDR(13)); \
        (addr)[14] = RV_READ_CSR(CSR_PMAADDR(14)); \
        (addr)[15] = RV_READ_CSR(CSR_PMAADDR(15)); \
        (cfg)[0] = RV_READ_CSR(CSR_PMACFG(0));     \
        (cfg)[1] = RV_READ_CSR(CSR_PMACFG(1));     \
        (cfg)[2] = RV_READ_CSR(CSR_PMACFG(2));     \
        (cfg)[3] = RV_READ_CSR(CSR_PMACFG(3));     \
        (cfg)[4] = RV_READ_CSR(CSR_PMACFG(4));     \
        (cfg)[5] = RV_READ_CSR(CSR_PMACFG(5));     \
        (cfg)[6] = RV_READ_CSR(CSR_PMACFG(6));     \
        (cfg)[7] = RV_READ_CSR(CSR_PMACFG(7));     \
        (cfg)[8] = RV_READ_CSR(CSR_PMACFG(8));     \
        (cfg)[9] = RV_READ_CSR(CSR_PMACFG(9));     \
        (cfg)[10] = RV_READ_CSR(CSR_PMACFG(10));   \
        (cfg)[11] = RV_READ_CSR(CSR_PMACFG(11));   \
        (cfg)[12] = RV_READ_CSR(CSR_PMACFG(12));   \
        (cfg)[13] = RV_READ_CSR(CSR_PMACFG(13));   \
        (cfg)[14] = RV_READ_CSR(CSR_PMACFG(14));   \
        (cfg)[15] = RV_READ_CSR(CSR_PMACFG(15));   \
    } while (0)

#define RV_SLEEP_RESTORE_PMA_REGS(addr, cfg) \
    do {                                           \
        RV_WRITE_CSR(CSR_PMAADDR(0), (addr)[0]);   \
        RV_WRITE_CSR(CSR_PMAADDR(1), (addr)[1]);   \
        RV_WRITE_CSR(CSR_PMAADDR(2), (addr)[2]);   \
        RV_WRITE_CSR(CSR_PMAADDR(3), (addr)[3]);   \
        RV_WRITE_CSR(CSR_PMAADDR(4), (addr)[4]);   \
        RV_WRITE_CSR(CSR_PMAADDR(5), (addr)[5]);   \
        RV_WRITE_CSR(CSR_PMAADDR(6), (addr)[6]);   \
        RV_WRITE_CSR(CSR_PMAADDR(7), (addr)[7]);   \
        RV_WRITE_CSR(CSR_PMAADDR(8), (addr)[8]);   \
        RV_WRITE_CSR(CSR_PMAADDR(9), (addr)[9]);   \
        RV_WRITE_CSR(CSR_PMAADDR(10), (addr)[10]); \
        RV_WRITE_CSR(CSR_PMAADDR(11), (addr)[11]); \
        RV_WRITE_CSR(CSR_PMAADDR(12), (addr)[12]); \
        RV_WRITE_CSR(CSR_PMAADDR(13), (addr)[13]); \
        RV_WRITE_CSR(CSR_PMAADDR(14), (addr)[14]); \
        RV_WRITE_CSR(CSR_PMAADDR(15), (addr)[15]); \
        RV_WRITE_CSR(CSR_PMACFG(0), (cfg)[0]);     \
        RV_WRITE_CSR(CSR_PMACFG(1), (cfg)[1]);     \
        RV_WRITE_CSR(CSR_PMACFG(2), (cfg)[2]);     \
        RV_WRITE_CSR(CSR_PMACFG(3), (cfg)[3]);     \
        RV_WRITE_CSR(CSR_PMACFG(4), (cfg)[4]);     \
        RV_WRITE_CSR(CSR_PMACFG(5), (cfg)[5]);     \
        RV_WRITE_CSR(CSR_PMACFG(6), (cfg)[6]);     \
        RV_WRITE_CSR(CSR_PMACFG(7), (cfg)[7]);     \
        RV_WRITE_CSR(CSR_PMACFG(8), (cfg)[8]);     \
        RV_WRITE_CSR(CSR_PMACFG(9), (cfg)[9]);     \
        RV_WRITE_CSR(CSR_PMACFG(10), (cfg)[10]);   \
        RV_WRITE_CSR(CSR_PMACFG(11), (cfg)[11]);   \
        RV_WRITE_CSR(CSR_PMACFG(12), (cfg)[12]);   \
        RV_WRITE_CSR(CSR_PMACFG(13), (cfg)[13]);   \
        RV_WRITE_CSR(CSR_PMACFG(14), (cfg)[14]);   \
        RV_WRITE_CSR(CSR_PMACFG(15), (cfg)[15]);   \
    } while (0)

#endif
