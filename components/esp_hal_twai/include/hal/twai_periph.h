/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/regdma.h"
#if SOC_HAS(PAU)
#include "soc/retention_periph_defs.h"
#endif
#if SOC_HAS(TWAI)
#include "hal/twai_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_HAS(TWAI)

typedef struct {
    const char *module_name;       // peripheral name
    const int irq_id;              // interrupt source ID
    const int timer_irq_id;        // time base interrupt source ID
    const int tx_sig;              // TX signal ID in GPIO matrix
    const int rx_sig;              // RX signal ID in GPIO matrix
    const int clk_out_sig;         // CLK_OUT signal ID in GPIO matrix
    const int bus_off_sig;         // BUS_OFF status signal ID in GPIO matrix
    const int stand_by_sig;        // STAND_BY signal ID in GPIO matrix
} twai_signal_conn_t;

extern const twai_signal_conn_t twai_periph_signals[SOC_TWAI_CONTROLLER_NUM];

#if SOC_HAS(PAU)
typedef struct {
    const periph_retention_module_t module_id;
    const regdma_entries_config_t *entry_array;
    uint32_t array_size;
} twai_reg_retention_info_t;

extern const twai_reg_retention_info_t twai_reg_retention_info[SOC_TWAI_CONTROLLER_NUM];
#endif  // SOC_HAS(PAU)

#endif // SOC_HAS(TWAI)

#ifdef __cplusplus
}
#endif
