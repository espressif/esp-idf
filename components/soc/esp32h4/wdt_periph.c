/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/wdt_periph.h"
#include "soc/soc_caps.h"

#if SOC_PAU_SUPPORTED

#define N_REGS_TGWDT          6 // TIMG_WDTCONFIG0_REG ... TIMG_WDTCONFIG5_REG & TIMG_INT_ENA_TIMERS_REG

static const regdma_entries_config_t tg0_wdt_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_TG0_WDT_LINK(0x00), TIMG_WDTWPROTECT_REG(0),   TIMG_WDT_WKEY_VALUE,     TIMG_WDT_WKEY_M,           1, 0), .owner = ENTRY(0) | ENTRY(2) },
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TG0_WDT_LINK(0x01), TIMG_WDTCONFIG0_REG(0),    TIMG_WDTCONFIG0_REG(0),  N_REGS_TGWDT,              0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [2] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_TG0_WDT_LINK(0x02), TIMG_WDTCONFIG0_REG(0),    TIMG_WDT_CONF_UPDATE_EN, TIMG_WDT_CONF_UPDATE_EN_M, 1, 0), .owner = ENTRY(0) | ENTRY(2) },
    [3] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_TG0_WDT_LINK(0x03), TIMG_WDTFEED_REG(0),       TIMG_WDT_FEED,           TIMG_WDT_FEED_M,           1, 0), .owner = ENTRY(0) | ENTRY(2) },
    [4] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TG0_WDT_LINK(0x04), TIMG_INT_ENA_TIMERS_REG(0), TIMG_INT_ENA_TIMERS_REG(0), 1,                      0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [5] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_TG0_WDT_LINK(0x05), TIMG_WDTWPROTECT_REG(0),   0,                       TIMG_WDT_WKEY_M,           1, 0), .owner = ENTRY(0) | ENTRY(2) },
};

static const regdma_entries_config_t tg1_wdt_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_TG1_WDT_LINK(0x00), TIMG_WDTWPROTECT_REG(1),   TIMG_WDT_WKEY_VALUE,     TIMG_WDT_WKEY_M,           1, 0), .owner = ENTRY(0) | ENTRY(2) },
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TG1_WDT_LINK(0x01), TIMG_INT_ENA_TIMERS_REG(1), TIMG_INT_ENA_TIMERS_REG(1), 1,                      0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [2] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TG1_WDT_LINK(0x02), TIMG_WDTCONFIG0_REG(1),    TIMG_WDTCONFIG0_REG(1),  N_REGS_TGWDT,              0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [3] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_TG1_WDT_LINK(0x03), TIMG_WDTCONFIG0_REG(1),    TIMG_WDT_CONF_UPDATE_EN, TIMG_WDT_CONF_UPDATE_EN_M, 1, 0), .owner = ENTRY(0) | ENTRY(2) },
    [4] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_TG0_WDT_LINK(0x04), TIMG_WDTFEED_REG(1),       TIMG_WDT_FEED,           TIMG_WDT_FEED_M,           1, 0), .owner = ENTRY(0) | ENTRY(2) },
    [5] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_TG1_WDT_LINK(0x05), TIMG_WDTWPROTECT_REG(1),   0,                       TIMG_WDT_WKEY_M,           1, 0), .owner = ENTRY(0) | ENTRY(2) },
};

const tg_reg_ctx_link_t tg_wdt_regs_retention[SOC_TIMER_GROUPS] = {
    [0] = {tg0_wdt_regs_retention, ARRAY_SIZE(tg0_wdt_regs_retention)},
    [1] = {tg1_wdt_regs_retention, ARRAY_SIZE(tg1_wdt_regs_retention)},
};

#endif // SOC_PAU_SUPPORTED
