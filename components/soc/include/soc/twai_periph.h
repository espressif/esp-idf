/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#if SOC_PAU_SUPPORTED
#include "soc/regdma.h"
#include "soc/retention_periph_defs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_TWAI_SUPPORTED

typedef struct {
    struct {
        const periph_module_t module;  // peripheral module
        const int irq_id;              // interrupt source ID
        const int tx_sig;              // TX signal ID in GPIO matrix
        const int rx_sig;              // RX signal ID in GPIO matrix
        const int clk_out_sig;         // CLK_OUT signal ID in GPIO matrix
        const int bus_off_sig;         // BUS_OFF status signal ID in GPIO matrix
        const int stand_by_sig;        // STAND_BY signal ID in GPIO matrix
    } controllers[SOC_TWAI_CONTROLLER_NUM];
} twai_controller_signal_conn_t;

extern const twai_controller_signal_conn_t twai_controller_periph_signals;

#if SOC_PAU_SUPPORTED
typedef struct {
    const periph_retention_module_t module_id;
    const regdma_entries_config_t *entry_array;
    uint32_t array_size;
} twai_reg_retention_info_t;

extern const twai_reg_retention_info_t twai_reg_retention_info[SOC_TWAI_CONTROLLER_NUM];
#endif  // SOC_PAU_SUPPORTED

#endif // SOC_TWAI_SUPPORTED

#ifdef __cplusplus
}
#endif
