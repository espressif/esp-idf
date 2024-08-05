/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "soc/soc.h"
#include "soc/periph_defs.h"
#include "soc/soc_caps.h"

#if SOC_I2S_SUPPORTED
#include "soc/i2s_struct.h"
#include "soc/i2s_reg.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_I2S_SUPPORTED
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
    const periph_module_t module;
} i2s_signal_conn_t;

extern const i2s_signal_conn_t i2s_periph_signal[SOC_I2S_NUM];

#if SOC_LP_I2S_SUPPORTED
extern const i2s_signal_conn_t lp_i2s_periph_signal[SOC_LP_I2S_NUM];
#endif

#endif // SOC_I2S_SUPPORTED

#ifdef __cplusplus
}
#endif
