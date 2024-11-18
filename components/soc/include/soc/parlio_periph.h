/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#if SOC_PARLIO_SUPPORTED
#include "soc/parl_io_reg.h"
#include "soc/parl_io_struct.h"
#endif
#include "soc/regdma.h"
#if SOC_PARLIO_SUPPORT_SLEEP_RETENTION
#include "soc/retention_periph_defs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_PARLIO_SUPPORTED
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

#if SOC_PARLIO_SUPPORT_SLEEP_RETENTION
typedef struct {
    const periph_retention_module_t retention_module;
    const regdma_entries_config_t *regdma_entry_array;
    uint32_t array_size;
} parlio_reg_retention_info_t;

extern const parlio_reg_retention_info_t parlio_reg_retention_info[SOC_PARLIO_GROUPS];
#endif // SOC_PARLIO_SUPPORT_SLEEP_RETENTION

#endif

#ifdef __cplusplus
}
#endif
