/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "soc/parl_io_reg.h"
#include "soc/parl_io_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    struct {
        struct {
            const int data_sigs[SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH];
            const int clk_out_sig;
            const int clk_in_sig;
        } tx_units[SOC_PARLIO_TX_UNITS_PER_GROUP];
        struct {
            const int data_sigs[SOC_PARLIO_RX_UNIT_MAX_DATA_WIDTH];
            const int clk_out_sig;
            const int clk_in_sig;
        } rx_units[SOC_PARLIO_RX_UNITS_PER_GROUP];
        const int tx_irq_id;
        const int rx_irq_id;
        const periph_module_t module;
    } groups[SOC_PARLIO_GROUPS];
} parlio_signal_conn_t;

extern const parlio_signal_conn_t parlio_periph_signals;

#ifdef __cplusplus
}
#endif
