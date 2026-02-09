/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-P4 LP_CLKRST register operations

#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include "soc/soc.h"
#include "soc/lp_clkrst_struct.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Select CPU reset vector
 * @param boot_from_lp_ram
 *        true:     boot from LP TCM RAM:   0x50108000
 *        false:    boot from HP TCM ROM:   0x4FC00000
 */
__attribute__((always_inline))
static inline void lp_clkrst_ll_boot_from_lp_ram(bool boot_from_lp_ram)
{
    LP_AON_CLKRST.hpcpu_reset_ctrl0.hpcore0_stat_vector_sel = !boot_from_lp_ram;
}

#ifdef __cplusplus
}
#endif
