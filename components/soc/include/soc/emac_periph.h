/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_num.h"


#ifdef __cplusplus
extern "C" {
#endif

#if SOC_EMAC_SUPPORTED
/**
 * This structure lists pin numbers
 */
typedef struct {
    uint32_t mdc_idx;
    uint32_t mdo_idx;
    uint32_t mdi_idx;
    uint32_t mii_tx_clk_i_idx;
    uint32_t mii_tx_en_o_idx;
    uint32_t mii_txd0_o_idx;
    uint32_t mii_txd1_o_idx;
    uint32_t mii_txd2_o_idx;
    uint32_t mii_txd3_o_idx;
    uint32_t mii_rx_clk_i_idx;
    uint32_t mii_rx_dv_i_idx;
    uint32_t mii_rxd0_i_idx;
    uint32_t mii_rxd1_i_idx;
    uint32_t mii_rxd2_i_idx;
    uint32_t mii_rxd3_i_idx;
    uint32_t mii_col_i_idx;
    uint32_t mii_crs_i_idx;
    uint32_t mii_rx_er_i_idx;
    uint32_t mii_tx_er_o_idx;
} emac_io_info_t;

typedef struct {
    gpio_num_t gpio_num;
    uint32_t func;
} emac_iomux_info_t;

typedef struct {
    const emac_iomux_info_t *clki;
    const emac_iomux_info_t *clko;
    const emac_iomux_info_t *tx_en;
    const emac_iomux_info_t *txd0;
    const emac_iomux_info_t *txd1;
    const emac_iomux_info_t *crs_dv;
    const emac_iomux_info_t *rxd0;
    const emac_iomux_info_t *rxd1;
    const emac_iomux_info_t *tx_er;
    const emac_iomux_info_t *rx_er;
} emac_rmii_iomux_info_t;

typedef struct {
    const emac_iomux_info_t *clk_tx;
    const emac_iomux_info_t *tx_en;
    const emac_iomux_info_t *txd0;
    const emac_iomux_info_t *txd1;
    const emac_iomux_info_t *txd2;
    const emac_iomux_info_t *txd3;
    const emac_iomux_info_t *clk_rx;
    const emac_iomux_info_t *rx_dv;
    const emac_iomux_info_t *rxd0;
    const emac_iomux_info_t *rxd1;
    const emac_iomux_info_t *rxd2;
    const emac_iomux_info_t *rxd3;
    const emac_iomux_info_t *tx_er;
    const emac_iomux_info_t *rx_er;
    const emac_iomux_info_t *col_in;
    const emac_iomux_info_t *crs_in;
} emac_mii_iomux_info_t;

extern const emac_io_info_t emac_io_idx;
extern const emac_rmii_iomux_info_t emac_rmii_iomux_pins;
extern const emac_mii_iomux_info_t emac_mii_iomux_pins;

#endif // SOC_EMAC_SUPPORTED

#ifdef __cplusplus
}
#endif
