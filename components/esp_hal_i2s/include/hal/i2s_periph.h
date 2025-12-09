/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "soc/soc.h"
#include "soc/interrupts.h"
#include "soc/soc_caps.h"
#include "soc/soc_caps_full.h"
#include "soc/regdma.h"
#if SOC_HAS(PAU)
#include "soc/retention_periph_defs.h"
#endif

#if SOC_HAS(I2S)
#include "soc/i2s_struct.h"
#include "soc/i2s_reg.h"
#include "hal/i2s_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_HAS(I2S)
/*
 Stores a bunch of per-I2S-peripheral data.
*/
typedef struct {
    const uint8_t mck_out_sig;
    const uint8_t mck_in_sig;

    const uint8_t m_tx_bck_sig;
    const uint8_t m_rx_bck_sig;
    const uint8_t m_tx_ws_sig;
    const uint8_t m_rx_ws_sig;

    const uint8_t s_tx_bck_sig;
    const uint8_t s_rx_bck_sig;
    const uint8_t s_tx_ws_sig;
    const uint8_t s_rx_ws_sig;

    union {
        const uint8_t data_out_sig;
#if SOC_I2S_PDM_MAX_TX_LINES
        const uint8_t data_out_sigs[SOC_I2S_PDM_MAX_TX_LINES]; // Only valid in version 2
#endif
    };

    union {
        const uint8_t data_in_sig;
#if SOC_I2S_PDM_MAX_RX_LINES
        const uint8_t data_in_sigs[SOC_I2S_PDM_MAX_RX_LINES];
#endif
    };

    const uint8_t irq;
} i2s_signal_conn_t;

extern const i2s_signal_conn_t i2s_periph_signal[I2S_LL_GET(INST_NUM)];

#if SOC_LP_I2S_SUPPORTED
extern const i2s_signal_conn_t lp_i2s_periph_signal[SOC_LP_I2S_NUM];
#endif

#if SOC_HAS(PAU)
typedef struct {
    const periph_retention_module_t retention_module;
    const regdma_entries_config_t *entry_array;
    uint32_t array_size;
} i2s_reg_retention_info_t;

extern const i2s_reg_retention_info_t i2s_reg_retention_info[I2S_LL_GET(INST_NUM)];
#endif  // SOC_HAS(PAU)
#endif  // SOC_HAS(I2S)

#ifdef __cplusplus
}
#endif
