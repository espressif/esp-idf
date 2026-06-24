/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#if SOC_PARLIO_SUPPORTED
#include "soc/parl_io_reg.h"
#include "soc/parl_io_struct.h"
#include "hal/parlio_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_HAS(PARLIO)
typedef struct {
    struct {
        const int data_sigs[SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH];
        const int clk_out_sig;
        const int clk_in_sig;
        const int cs_sig;
    } tx_units[PARLIO_LL_GET(TX_UNITS_PER_INST)];
    struct {
        const int data_sigs[SOC_PARLIO_RX_UNIT_MAX_DATA_WIDTH];
        const int clk_out_sig;
        const int clk_in_sig;
    } rx_units[PARLIO_LL_GET(RX_UNITS_PER_INST)];
    const int tx_irq_id;
    const int rx_irq_id;
    const char *module_name;
} soc_parlio_signal_desc_t;

extern const soc_parlio_signal_desc_t soc_parlio_signals[PARLIO_LL_GET(INST_NUM)];

#endif

#ifdef __cplusplus
}
#endif
