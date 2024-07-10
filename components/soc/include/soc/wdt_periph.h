/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/timer_group_reg.h"
#include "soc/timer_group_struct.h"
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "soc/regdma.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The value that needs to be written to TIMG_WDT_WKEY to write-enable the wdt registers */
#define TIMG_WDT_WKEY_VALUE 0x50D83AA1

#if SOC_MWDT_SUPPORT_SLEEP_RETENTION
typedef struct {
    const regdma_entries_config_t *link_list;
    uint32_t link_num;
} tg_reg_ctx_link_t;

extern const tg_reg_ctx_link_t tg_wdt_regs_retention[SOC_TIMER_GROUPS];
#endif // SOC_MWDT_SUPPORT_SLEEP_RETENTION

#ifdef __cplusplus
}
#endif
