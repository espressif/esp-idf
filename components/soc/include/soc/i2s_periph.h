/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "soc/soc.h"
#include "soc/periph_defs.h"
#include "soc/i2s_struct.h"
#include "soc/i2s_reg.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 Stores a bunch of per-I2S-peripheral data.
*/
typedef struct {
    const uint8_t mck_out_sig;

    const uint8_t m_tx_bck_sig;
    const uint8_t m_rx_bck_sig;
    const uint8_t m_tx_ws_sig;
    const uint8_t m_rx_ws_sig;

    const uint8_t s_tx_bck_sig;
    const uint8_t s_rx_bck_sig;
    const uint8_t s_tx_ws_sig;
    const uint8_t s_rx_ws_sig;

    const uint8_t data_out_sig;
    const uint8_t data_out1_sig; // Only valid in version 2
    const uint8_t data_in_sig;

    const uint8_t irq;
    const periph_module_t module;
} i2s_signal_conn_t;

extern const i2s_signal_conn_t i2s_periph_signal[SOC_I2S_NUM];

#ifdef __cplusplus
}
#endif
