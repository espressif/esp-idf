/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Registers */
/** Type of gmac_rmii_clk register
 *  GMAC IO MUX configuration register for GMAC_RMII_CLK
 */
typedef union {
    struct {
        /** gmac_rmii_clk_slp_sel : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enter sleep mode for GMAC_RMII_CLK.
         *  0: Not enter
         *  1: Enter
         */
        uint32_t gmac_rmii_clk_slp_sel:1;
        /** gmac_rmii_clk_mcu_wpd : R/W; bitpos: [1]; default: 0;
         *  Configure whether or not to enable pull-down resistor of GMAC_RMII_CLK in sleep
         *  mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_rmii_clk_mcu_wpd:1;
        /** gmac_rmii_clk_mcu_wpu : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable pull-up resistor of GMAC_RMII_CLK during sleep
         *  mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_rmii_clk_mcu_wpu:1;
        /** gmac_rmii_clk_mcu_ie : R/W; bitpos: [3]; default: 1;
         *  Configures whether or not to enable the input of GMAC_RMII_CLK during sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_rmii_clk_mcu_ie:1;
        /** gmac_rmii_clk_mcu_drv : R/W; bitpos: [5:4]; default: 2;
         *  Configures the drive strength of GMAC_RMII_CLK during sleep mode.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t gmac_rmii_clk_mcu_drv:2;
        /** gmac_rmii_clk_fun_wpd : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable pull-down resistor of GMAC_RMII_CLK.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_rmii_clk_fun_wpd:1;
        /** gmac_rmii_clk_fun_wpu : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not enable pull-up resistor of GMAC_RMII_CLK.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_rmii_clk_fun_wpu:1;
        /** gmac_rmii_clk_fun_ie : R/W; bitpos: [8]; default: 1;
         *  Configures whether or not to enable input of GMAC_RMII_CLK.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_rmii_clk_fun_ie:1;
        /** gmac_rmii_clk_fun_drv : R/W; bitpos: [10:9]; default: 2;
         *  Configures the drive strength of GMAC_RMII_CLK.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t gmac_rmii_clk_fun_drv:2;
        /** gmac_rmii_clk_hys_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable the hysteresis function of the pin when
         *  CNNT_IO_MUX_GMAC_RMII_CLK_HYS_SEL is set to 1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_rmii_clk_hys_en:1;
        /** gmac_rmii_clk_hys_sel : R/W; bitpos: [12]; default: 0;
         *  Configures to choose the signal for enabling the hysteresis function for
         *  GMAC_RMII_CLK.
         *  0: Choose the output enable signal of eFuse
         *  1: Choose the output enable signal of CNNT_IO_MUX_GMAC_RMII_CLK_HYS_EN
         */
        uint32_t gmac_rmii_clk_hys_sel:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cnnt_io_mux_gmac_rmii_clk_reg_t;

/** Type of gmac_rx_clk register
 *  GMAC IO MUX configuration register for GMAC_RX_CLK
 */
typedef union {
    struct {
        /** gmac_rx_clk_slp_sel : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enter sleep mode for GMAC_RX_CLK.
         *  0: Not enter
         *  1: Enter
         */
        uint32_t gmac_rx_clk_slp_sel:1;
        /** gmac_rx_clk_mcu_wpd : R/W; bitpos: [1]; default: 0;
         *  Configure whether or not to enable pull-down resistor of GMAC_RX_CLK in sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_rx_clk_mcu_wpd:1;
        /** gmac_rx_clk_mcu_wpu : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable pull-up resistor of GMAC_RX_CLK during sleep
         *  mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_rx_clk_mcu_wpu:1;
        /** gmac_rx_clk_mcu_ie : R/W; bitpos: [3]; default: 1;
         *  Configures whether or not to enable the input of GMAC_RX_CLK during sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_rx_clk_mcu_ie:1;
        /** gmac_rx_clk_mcu_drv : R/W; bitpos: [5:4]; default: 2;
         *  Configures the drive strength of GMAC_RX_CLK during sleep mode.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t gmac_rx_clk_mcu_drv:2;
        /** gmac_rx_clk_fun_wpd : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable pull-down resistor of GMAC_RX_CLK.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_rx_clk_fun_wpd:1;
        /** gmac_rx_clk_fun_wpu : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not enable pull-up resistor of GMAC_RX_CLK.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_rx_clk_fun_wpu:1;
        /** gmac_rx_clk_fun_ie : R/W; bitpos: [8]; default: 1;
         *  Configures whether or not to enable input of GMAC_RX_CLK.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_rx_clk_fun_ie:1;
        /** gmac_rx_clk_fun_drv : R/W; bitpos: [10:9]; default: 2;
         *  Configures the drive strength of GMAC_RX_CLK.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t gmac_rx_clk_fun_drv:2;
        /** gmac_rx_clk_hys_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable the hysteresis function of the pin when
         *  CNNT_IO_MUX_GMAC_RX_CLK_HYS_SEL is set to 1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_rx_clk_hys_en:1;
        /** gmac_rx_clk_hys_sel : R/W; bitpos: [12]; default: 0;
         *  Configures to choose the signal for enabling the hysteresis function for
         *  GMAC_RX_CLK.
         *  0: Choose the output enable signal of eFuse
         *  1: Choose the output enable signal of CNNT_IO_MUX_GMAC_RX_CLK_HYS_EN
         */
        uint32_t gmac_rx_clk_hys_sel:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cnnt_io_mux_gmac_rx_clk_reg_t;

/** Type of gmac_phy_rxdv register
 *  GMAC IO MUX configuration register for GMAC_PHY_RXDV
 */
typedef union {
    struct {
        /** gmac_phy_rxdv_slp_sel : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enter sleep mode for GMAC_PHY_RXDV.
         *  0: Not enter
         *  1: Enter
         */
        uint32_t gmac_phy_rxdv_slp_sel:1;
        /** gmac_phy_rxdv_mcu_wpd : R/W; bitpos: [1]; default: 0;
         *  Configure whether or not to enable pull-down resistor of GMAC_PHY_RXDV in sleep
         *  mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxdv_mcu_wpd:1;
        /** gmac_phy_rxdv_mcu_wpu : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable pull-up resistor of GMAC_PHY_RXDV during sleep
         *  mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxdv_mcu_wpu:1;
        /** gmac_phy_rxdv_mcu_ie : R/W; bitpos: [3]; default: 1;
         *  Configures whether or not to enable the input of GMAC_PHY_RXDV during sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxdv_mcu_ie:1;
        /** gmac_phy_rxdv_mcu_drv : R/W; bitpos: [5:4]; default: 2;
         *  Configures the drive strength of GMAC_PHY_RXDV during sleep mode.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t gmac_phy_rxdv_mcu_drv:2;
        /** gmac_phy_rxdv_fun_wpd : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable pull-down resistor of GMAC_PHY_RXDV.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxdv_fun_wpd:1;
        /** gmac_phy_rxdv_fun_wpu : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not enable pull-up resistor of GMAC_PHY_RXDV.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxdv_fun_wpu:1;
        /** gmac_phy_rxdv_fun_ie : R/W; bitpos: [8]; default: 1;
         *  Configures whether or not to enable input of GMAC_PHY_RXDV.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxdv_fun_ie:1;
        /** gmac_phy_rxdv_fun_drv : R/W; bitpos: [10:9]; default: 2;
         *  Configures the drive strength of GMAC_PHY_RXDV.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t gmac_phy_rxdv_fun_drv:2;
        /** gmac_phy_rxdv_hys_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable the hysteresis function of the pin when
         *  CNNT_IO_MUX_GMAC_PHY_RXDV_HYS_SEL is set to 1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxdv_hys_en:1;
        /** gmac_phy_rxdv_hys_sel : R/W; bitpos: [12]; default: 0;
         *  Configures to choose the signal for enabling the hysteresis function for
         *  GMAC_PHY_RXDV.
         *  0: Choose the output enable signal of eFuse
         *  1: Choose the output enable signal of CNNT_IO_MUX_GMAC_PHY_RXDV_HYS_EN
         */
        uint32_t gmac_phy_rxdv_hys_sel:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cnnt_io_mux_gmac_phy_rxdv_reg_t;

/** Type of gmac_phy_rxd3 register
 *  GMAC IO MUX configuration register for GMAC_PHY_RXD3
 */
typedef union {
    struct {
        /** gmac_phy_rxd3_slp_sel : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enter sleep mode for GMAC_PHY_RXD3.
         *  0: Not enter
         *  1: Enter
         */
        uint32_t gmac_phy_rxd3_slp_sel:1;
        /** gmac_phy_rxd3_mcu_wpd : R/W; bitpos: [1]; default: 0;
         *  Configure whether or not to enable pull-down resistor of GMAC_PHY_RXD3 in sleep
         *  mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd3_mcu_wpd:1;
        /** gmac_phy_rxd3_mcu_wpu : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable pull-up resistor of GMAC_PHY_RXD3 during sleep
         *  mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd3_mcu_wpu:1;
        /** gmac_phy_rxd3_mcu_ie : R/W; bitpos: [3]; default: 1;
         *  Configures whether or not to enable the input of GMAC_PHY_RXD3 during sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd3_mcu_ie:1;
        /** gmac_phy_rxd3_mcu_drv : R/W; bitpos: [5:4]; default: 2;
         *  Configures the drive strength of GMAC_PHY_RXD3 during sleep mode.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t gmac_phy_rxd3_mcu_drv:2;
        /** gmac_phy_rxd3_fun_wpd : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable pull-down resistor of GMAC_PHY_RXD3.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd3_fun_wpd:1;
        /** gmac_phy_rxd3_fun_wpu : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not enable pull-up resistor of GMAC_PHY_RXD3.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd3_fun_wpu:1;
        /** gmac_phy_rxd3_fun_ie : R/W; bitpos: [8]; default: 1;
         *  Configures whether or not to enable input of GMAC_PHY_RXD3.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd3_fun_ie:1;
        /** gmac_phy_rxd3_fun_drv : R/W; bitpos: [10:9]; default: 2;
         *  Configures the drive strength of GMAC_PHY_RXD3.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t gmac_phy_rxd3_fun_drv:2;
        /** gmac_phy_rxd3_hys_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable the hysteresis function of the pin when
         *  CNNT_IO_MUX_GMAC_PHY_RXD3_HYS_SEL is set to 1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd3_hys_en:1;
        /** gmac_phy_rxd3_hys_sel : R/W; bitpos: [12]; default: 0;
         *  Configures to choose the signal for enabling the hysteresis function for
         *  GMAC_PHY_RXD3.
         *  0: Choose the output enable signal of eFuse
         *  1: Choose the output enable signal of CNNT_IO_MUX_GMAC_PHY_RXD3_HYS_EN
         */
        uint32_t gmac_phy_rxd3_hys_sel:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cnnt_io_mux_gmac_phy_rxd3_reg_t;

/** Type of gmac_phy_rxd2 register
 *  GMAC IO MUX configuration register for GMAC_PHY_RXD2
 */
typedef union {
    struct {
        /** gmac_phy_rxd2_slp_sel : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enter sleep mode for GMAC_PHY_RXD2.
         *  0: Not enter
         *  1: Enter
         */
        uint32_t gmac_phy_rxd2_slp_sel:1;
        /** gmac_phy_rxd2_mcu_wpd : R/W; bitpos: [1]; default: 0;
         *  Configure whether or not to enable pull-down resistor of GMAC_PHY_RXD2 in sleep
         *  mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd2_mcu_wpd:1;
        /** gmac_phy_rxd2_mcu_wpu : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable pull-up resistor of GMAC_PHY_RXD2 during sleep
         *  mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd2_mcu_wpu:1;
        /** gmac_phy_rxd2_mcu_ie : R/W; bitpos: [3]; default: 1;
         *  Configures whether or not to enable the input of GMAC_PHY_RXD2 during sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd2_mcu_ie:1;
        /** gmac_phy_rxd2_mcu_drv : R/W; bitpos: [5:4]; default: 2;
         *  Configures the drive strength of GMAC_PHY_RXD2 during sleep mode.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t gmac_phy_rxd2_mcu_drv:2;
        /** gmac_phy_rxd2_fun_wpd : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable pull-down resistor of GMAC_PHY_RXD2.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd2_fun_wpd:1;
        /** gmac_phy_rxd2_fun_wpu : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not enable pull-up resistor of GMAC_PHY_RXD2.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd2_fun_wpu:1;
        /** gmac_phy_rxd2_fun_ie : R/W; bitpos: [8]; default: 1;
         *  Configures whether or not to enable input of GMAC_PHY_RXD2.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd2_fun_ie:1;
        /** gmac_phy_rxd2_fun_drv : R/W; bitpos: [10:9]; default: 2;
         *  Configures the drive strength of GMAC_PHY_RXD2.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t gmac_phy_rxd2_fun_drv:2;
        /** gmac_phy_rxd2_hys_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable the hysteresis function of the pin when
         *  CNNT_IO_MUX_GMAC_PHY_RXD2_HYS_SEL is set to 1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd2_hys_en:1;
        /** gmac_phy_rxd2_hys_sel : R/W; bitpos: [12]; default: 0;
         *  Configures to choose the signal for enabling the hysteresis function for
         *  GMAC_PHY_RXD2.
         *  0: Choose the output enable signal of eFuse
         *  1: Choose the output enable signal of CNNT_IO_MUX_GMAC_PHY_RXD2_HYS_EN
         */
        uint32_t gmac_phy_rxd2_hys_sel:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cnnt_io_mux_gmac_phy_rxd2_reg_t;

/** Type of gmac_phy_rxd1 register
 *  GMAC IO MUX configuration register for GMAC_PHY_RXD1
 */
typedef union {
    struct {
        /** gmac_phy_rxd1_slp_sel : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enter sleep mode for GMAC_PHY_RXD1.
         *  0: Not enter
         *  1: Enter
         */
        uint32_t gmac_phy_rxd1_slp_sel:1;
        /** gmac_phy_rxd1_mcu_wpd : R/W; bitpos: [1]; default: 0;
         *  Configure whether or not to enable pull-down resistor of GMAC_PHY_RXD1 in sleep
         *  mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd1_mcu_wpd:1;
        /** gmac_phy_rxd1_mcu_wpu : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable pull-up resistor of GMAC_PHY_RXD1 during sleep
         *  mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd1_mcu_wpu:1;
        /** gmac_phy_rxd1_mcu_ie : R/W; bitpos: [3]; default: 1;
         *  Configures whether or not to enable the input of GMAC_PHY_RXD1 during sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd1_mcu_ie:1;
        /** gmac_phy_rxd1_mcu_drv : R/W; bitpos: [5:4]; default: 2;
         *  Configures the drive strength of GMAC_PHY_RXD1 during sleep mode.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t gmac_phy_rxd1_mcu_drv:2;
        /** gmac_phy_rxd1_fun_wpd : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable pull-down resistor of GMAC_PHY_RXD1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd1_fun_wpd:1;
        /** gmac_phy_rxd1_fun_wpu : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not enable pull-up resistor of GMAC_PHY_RXD1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd1_fun_wpu:1;
        /** gmac_phy_rxd1_fun_ie : R/W; bitpos: [8]; default: 1;
         *  Configures whether or not to enable input of GMAC_PHY_RXD1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd1_fun_ie:1;
        /** gmac_phy_rxd1_fun_drv : R/W; bitpos: [10:9]; default: 2;
         *  Configures the drive strength of GMAC_PHY_RXD1.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t gmac_phy_rxd1_fun_drv:2;
        /** gmac_phy_rxd1_hys_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable the hysteresis function of the pin when
         *  CNNT_IO_MUX_GMAC_PHY_RXD1_HYS_SEL is set to 1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd1_hys_en:1;
        /** gmac_phy_rxd1_hys_sel : R/W; bitpos: [12]; default: 0;
         *  Configures to choose the signal for enabling the hysteresis function for
         *  GMAC_PHY_RXD1.
         *  0: Choose the output enable signal of eFuse
         *  1: Choose the output enable signal of CNNT_IO_MUX_GMAC_PHY_RXD1_HYS_EN
         */
        uint32_t gmac_phy_rxd1_hys_sel:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cnnt_io_mux_gmac_phy_rxd1_reg_t;

/** Type of gmac_phy_rxd0 register
 *  GMAC IO MUX configuration register for GMAC_PHY_RXD0
 */
typedef union {
    struct {
        /** gmac_phy_rxd0_slp_sel : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enter sleep mode for GMAC_PHY_RXD0.
         *  0: Not enter
         *  1: Enter
         */
        uint32_t gmac_phy_rxd0_slp_sel:1;
        /** gmac_phy_rxd0_mcu_wpd : R/W; bitpos: [1]; default: 0;
         *  Configure whether or not to enable pull-down resistor of GMAC_PHY_RXD0 in sleep
         *  mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd0_mcu_wpd:1;
        /** gmac_phy_rxd0_mcu_wpu : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable pull-up resistor of GMAC_PHY_RXD0 during sleep
         *  mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd0_mcu_wpu:1;
        /** gmac_phy_rxd0_mcu_ie : R/W; bitpos: [3]; default: 1;
         *  Configures whether or not to enable the input of GMAC_PHY_RXD0 during sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd0_mcu_ie:1;
        /** gmac_phy_rxd0_mcu_drv : R/W; bitpos: [5:4]; default: 2;
         *  Configures the drive strength of GMAC_PHY_RXD0 during sleep mode.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t gmac_phy_rxd0_mcu_drv:2;
        /** gmac_phy_rxd0_fun_wpd : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable pull-down resistor of GMAC_PHY_RXD0.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd0_fun_wpd:1;
        /** gmac_phy_rxd0_fun_wpu : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not enable pull-up resistor of GMAC_PHY_RXD0.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd0_fun_wpu:1;
        /** gmac_phy_rxd0_fun_ie : R/W; bitpos: [8]; default: 1;
         *  Configures whether or not to enable input of GMAC_PHY_RXD0.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd0_fun_ie:1;
        /** gmac_phy_rxd0_fun_drv : R/W; bitpos: [10:9]; default: 2;
         *  Configures the drive strength of GMAC_PHY_RXD0.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t gmac_phy_rxd0_fun_drv:2;
        /** gmac_phy_rxd0_hys_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable the hysteresis function of the pin when
         *  CNNT_IO_MUX_GMAC_PHY_RXD0_HYS_SEL is set to 1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t gmac_phy_rxd0_hys_en:1;
        /** gmac_phy_rxd0_hys_sel : R/W; bitpos: [12]; default: 0;
         *  Configures to choose the signal for enabling the hysteresis function for
         *  GMAC_PHY_RXD0.
         *  0: Choose the output enable signal of eFuse
         *  1: Choose the output enable signal of CNNT_IO_MUX_GMAC_PHY_RXD0_HYS_EN
         */
        uint32_t gmac_phy_rxd0_hys_sel:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cnnt_io_mux_gmac_phy_rxd0_reg_t;

/** Type of sdio_data0 register
 *  SDIO IO MUX configuration register for SDIO_DATA0
 */
typedef union {
    struct {
        /** sdio_data0_slp_sel : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enter sleep mode for SDIO_DATA0.
         *  0: Not enter
         *  1: Enter
         */
        uint32_t sdio_data0_slp_sel:1;
        /** sdio_data0_mcu_wpd : R/W; bitpos: [1]; default: 0;
         *  Configure whether or not to enable pull-down resistor of SDIO_DATA0 in sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data0_mcu_wpd:1;
        /** sdio_data0_mcu_wpu : R/W; bitpos: [2]; default: 1;
         *  Configures whether or not to enable pull-up resistor of SDIO_DATA0 during sleep
         *  mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data0_mcu_wpu:1;
        /** sdio_data0_mcu_ie : R/W; bitpos: [3]; default: 1;
         *  Configures whether or not to enable the input of SDIO_DATA0 during sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data0_mcu_ie:1;
        /** sdio_data0_mcu_drv : R/W; bitpos: [5:4]; default: 2;
         *  Configures the drive strength of SDIO_DATA0 during sleep mode.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t sdio_data0_mcu_drv:2;
        /** sdio_data0_fun_wpd : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable pull-down resistor of SDIO_DATA0
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data0_fun_wpd:1;
        /** sdio_data0_fun_wpu : R/W; bitpos: [7]; default: 1;
         *  Configures whether or not enable pull-up resistor of SDIO_DATA0.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data0_fun_wpu:1;
        /** sdio_data0_fun_ie : R/W; bitpos: [8]; default: 1;
         *  Configures whether or not to enable input of SDIO_DATA0.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data0_fun_ie:1;
        /** sdio_data0_fun_drv : R/W; bitpos: [10:9]; default: 2;
         *  Configures the drive strength of SDIO_DATA0.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t sdio_data0_fun_drv:2;
        /** sdio_data0_hys_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable the hysteresis function of the pin when
         *  CNNT_IO_MUX_SDIO_DATA0_HYS_SEL is set to 1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data0_hys_en:1;
        /** sdio_data0_hys_sel : R/W; bitpos: [12]; default: 0;
         *  Configures to choose the signal for enabling the hysteresis function for
         *  SDIO_DATA0.
         *  0: Choose the output enable signal of eFuse
         *  1: Choose the output enable signal of CNNT_IO_MUX_SDIO_DATA0_HYS_EN
         */
        uint32_t sdio_data0_hys_sel:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cnnt_io_mux_sdio_data0_reg_t;

/** Type of sdio_data1 register
 *  SDIO IO MUX configuration register for SDIO_DATA1
 */
typedef union {
    struct {
        /** sdio_data1_slp_sel : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enter sleep mode for SDIO_DATA1.
         *  0: Not enter
         *  1: Enter
         */
        uint32_t sdio_data1_slp_sel:1;
        /** sdio_data1_mcu_wpd : R/W; bitpos: [1]; default: 0;
         *  Configure whether or not to enable pull-down resistor of SDIO_DATA1 in sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data1_mcu_wpd:1;
        /** sdio_data1_mcu_wpu : R/W; bitpos: [2]; default: 1;
         *  Configures whether or not to enable pull-up resistor of SDIO_DATA1 during sleep
         *  mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data1_mcu_wpu:1;
        /** sdio_data1_mcu_ie : R/W; bitpos: [3]; default: 1;
         *  Configures whether or not to enable the input of SDIO_DATA1 during sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data1_mcu_ie:1;
        /** sdio_data1_mcu_drv : R/W; bitpos: [5:4]; default: 2;
         *  Configures the drive strength of SDIO_DATA1 during sleep mode.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t sdio_data1_mcu_drv:2;
        /** sdio_data1_fun_wpd : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable pull-down resistor of SDIO_DATA1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data1_fun_wpd:1;
        /** sdio_data1_fun_wpu : R/W; bitpos: [7]; default: 1;
         *  Configures whether or not enable pull-up resistor of SDIO_DATA1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data1_fun_wpu:1;
        /** sdio_data1_fun_ie : R/W; bitpos: [8]; default: 1;
         *  Configures whether or not to enable input of SDIO_DATA1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data1_fun_ie:1;
        /** sdio_data1_fun_drv : R/W; bitpos: [10:9]; default: 2;
         *  Configures the drive strength of SDIO_DATA1.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t sdio_data1_fun_drv:2;
        /** sdio_data1_hys_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable the hysteresis function of the pin when
         *  CNNT_IO_MUX_SDIO_DATA1_HYS_SEL is set to 1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data1_hys_en:1;
        /** sdio_data1_hys_sel : R/W; bitpos: [12]; default: 0;
         *  Configures to choose the signal for enabling the hysteresis function for
         *  SDIO_DATA1.
         *  0: Choose the output enable signal of eFuse
         *  1: Choose the output enable signal of CNNT_IO_MUX_SDIO_DATA1_HYS_EN
         */
        uint32_t sdio_data1_hys_sel:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cnnt_io_mux_sdio_data1_reg_t;

/** Type of sdio_data2 register
 *  SDIO IO MUX configuration register for SDIO_DATA2
 */
typedef union {
    struct {
        /** sdio_data2_slp_sel : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enter sleep mode for SDIO_DATA2.
         *  0: Not enter
         *  1: Enter
         */
        uint32_t sdio_data2_slp_sel:1;
        /** sdio_data2_mcu_wpd : R/W; bitpos: [1]; default: 0;
         *  Configure whether or not to enable pull-down resistor of SDIO_DATA2 in sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data2_mcu_wpd:1;
        /** sdio_data2_mcu_wpu : R/W; bitpos: [2]; default: 1;
         *  Configures whether or not to enable pull-up resistor of SDIO_DATA2 during sleep
         *  mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data2_mcu_wpu:1;
        /** sdio_data2_mcu_ie : R/W; bitpos: [3]; default: 1;
         *  Configures whether or not to enable the input of SDIO_DATA2 during sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data2_mcu_ie:1;
        /** sdio_data2_mcu_drv : R/W; bitpos: [5:4]; default: 2;
         *  Configures the drive strength of SDIO_DATA2 during sleep mode.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t sdio_data2_mcu_drv:2;
        /** sdio_data2_fun_wpd : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable pull-down resistor of SDIO_DATA2.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data2_fun_wpd:1;
        /** sdio_data2_fun_wpu : R/W; bitpos: [7]; default: 1;
         *  Configures whether or not enable pull-up resistor of SDIO_DATA2.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data2_fun_wpu:1;
        /** sdio_data2_fun_ie : R/W; bitpos: [8]; default: 1;
         *  Configures whether or not to enable input of SDIO_DATA2.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data2_fun_ie:1;
        /** sdio_data2_fun_drv : R/W; bitpos: [10:9]; default: 2;
         *  Configures the drive strength of SDIO_DATA2.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t sdio_data2_fun_drv:2;
        /** sdio_data2_hys_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable the hysteresis function of the pin when
         *  CNNT_IO_MUX_SDIO_DATA2_HYS_SEL is set to 1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data2_hys_en:1;
        /** sdio_data2_hys_sel : R/W; bitpos: [12]; default: 0;
         *  Configures to choose the signal for enabling the hysteresis function for
         *  SDIO_DATA2.
         *  0: Choose the output enable signal of eFuse
         *  1: Choose the output enable signal of CNNT_IO_MUX_SDIO_DATA2_HYS_EN
         */
        uint32_t sdio_data2_hys_sel:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cnnt_io_mux_sdio_data2_reg_t;

/** Type of sdio_data3 register
 *  SDIO IO MUX configuration register for SDIO_DATA3
 */
typedef union {
    struct {
        /** sdio_data3_slp_sel : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enter sleep mode for SDIO_DATA3.
         *  0: Not enter
         *  1: Enter
         */
        uint32_t sdio_data3_slp_sel:1;
        /** sdio_data3_mcu_wpd : R/W; bitpos: [1]; default: 0;
         *  Configure whether or not to enable pull-down resistor of SDIO_DATA3 in sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data3_mcu_wpd:1;
        /** sdio_data3_mcu_wpu : R/W; bitpos: [2]; default: 1;
         *  Configures whether or not to enable pull-up resistor of SDIO_DATA3 during sleep
         *  mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data3_mcu_wpu:1;
        /** sdio_data3_mcu_ie : R/W; bitpos: [3]; default: 1;
         *  Configures whether or not to enable the input of SDIO_DATA3 during sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data3_mcu_ie:1;
        /** sdio_data3_mcu_drv : R/W; bitpos: [5:4]; default: 2;
         *  Configures the drive strength of SDIO_DATA3 during sleep mode.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t sdio_data3_mcu_drv:2;
        /** sdio_data3_fun_wpd : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable pull-down resistor of SDIO_DATA3.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data3_fun_wpd:1;
        /** sdio_data3_fun_wpu : R/W; bitpos: [7]; default: 1;
         *  Configures whether or not enable pull-up resistor of SDIO_DATA3.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data3_fun_wpu:1;
        /** sdio_data3_fun_ie : R/W; bitpos: [8]; default: 1;
         *  Configures whether or not to enable input of SDIO_DATA3.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data3_fun_ie:1;
        /** sdio_data3_fun_drv : R/W; bitpos: [10:9]; default: 2;
         *  Configures the drive strength of SDIO_DATA3.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t sdio_data3_fun_drv:2;
        /** sdio_data3_hys_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable the hysteresis function of the pin when
         *  CNNT_IO_MUX_SDIO_DATA3_HYS_SEL is set to 1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_data3_hys_en:1;
        /** sdio_data3_hys_sel : R/W; bitpos: [12]; default: 0;
         *  Configures to choose the signal for enabling the hysteresis function for
         *  SDIO_DATA3.
         *  0: Choose the output enable signal of eFuse
         *  1: Choose the output enable signal of CNNT_IO_MUX_SDIO_DATA3_HYS_EN
         */
        uint32_t sdio_data3_hys_sel:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cnnt_io_mux_sdio_data3_reg_t;

/** Type of sdio_clk register
 *  SDIO IO MUX configuration register for SDIO_CLK
 */
typedef union {
    struct {
        /** sdio_clk_slp_sel : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enter sleep mode for SDIO_CLK.
         *  0: Not enter
         *  1: Enter
         */
        uint32_t sdio_clk_slp_sel:1;
        /** sdio_clk_mcu_wpd : R/W; bitpos: [1]; default: 0;
         *  Configure whether or not to enable pull-down resistor of SDIO_CLK in sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_clk_mcu_wpd:1;
        /** sdio_clk_mcu_wpu : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable pull-up resistor of SDIO_CLK during sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_clk_mcu_wpu:1;
        /** sdio_clk_mcu_ie : R/W; bitpos: [3]; default: 1;
         *  Configures whether or not to enable the input of SDIO_CLK during sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_clk_mcu_ie:1;
        /** sdio_clk_mcu_drv : R/W; bitpos: [5:4]; default: 2;
         *  Configures the drive strength of SDIO_CLK during sleep mode.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t sdio_clk_mcu_drv:2;
        /** sdio_clk_fun_wpd : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable pull-down resistor of SDIO_CLK.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_clk_fun_wpd:1;
        /** sdio_clk_fun_wpu : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not enable pull-up resistor of SDIO_CLK.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_clk_fun_wpu:1;
        /** sdio_clk_fun_ie : R/W; bitpos: [8]; default: 1;
         *  Configures whether or not to enable input of SDIO_CLK.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_clk_fun_ie:1;
        /** sdio_clk_fun_drv : R/W; bitpos: [10:9]; default: 2;
         *  Configures the drive strength of SDIO_CLK.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t sdio_clk_fun_drv:2;
        /** sdio_clk_hys_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable the hysteresis function of the pin when
         *  CNNT_IO_MUX_SDIO_CLK_HYS_SEL is set to 1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_clk_hys_en:1;
        /** sdio_clk_hys_sel : R/W; bitpos: [12]; default: 0;
         *  Configures to choose the signal for enabling the hysteresis function for SDIO_CLK.
         *  0: Choose the output enable signal of eFuse
         *  1: Choose the output enable signal of CNNT_IO_MUX_SDIO_CLK_HYS_EN
         */
        uint32_t sdio_clk_hys_sel:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cnnt_io_mux_sdio_clk_reg_t;

/** Type of sdio_cmd register
 *  SDIO IO MUX configuration register for SDIO_CMD
 */
typedef union {
    struct {
        /** sdio_cmd_slp_sel : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enter sleep mode for SDIO_CMD.
         *  0: Not enter
         *  1: Enter
         */
        uint32_t sdio_cmd_slp_sel:1;
        /** sdio_cmd_mcu_wpd : R/W; bitpos: [1]; default: 0;
         *  Configure whether or not to enable pull-down resistor of SDIO_CMD in sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_cmd_mcu_wpd:1;
        /** sdio_cmd_mcu_wpu : R/W; bitpos: [2]; default: 1;
         *  Configures whether or not to enable pull-up resistor of SDIO_CMD during sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_cmd_mcu_wpu:1;
        /** sdio_cmd_mcu_ie : R/W; bitpos: [3]; default: 1;
         *  Configures whether or not to enable the input of SDIO_CMD during sleep mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_cmd_mcu_ie:1;
        /** sdio_cmd_mcu_drv : R/W; bitpos: [5:4]; default: 2;
         *  Configures the drive strength of SDIO_CMD during sleep mode.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t sdio_cmd_mcu_drv:2;
        /** sdio_cmd_fun_wpd : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable pull-down resistor of SDIO_CMD.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_cmd_fun_wpd:1;
        /** sdio_cmd_fun_wpu : R/W; bitpos: [7]; default: 1;
         *  Configures whether or not enable pull-up resistor of SDIO_CMD.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_cmd_fun_wpu:1;
        /** sdio_cmd_fun_ie : R/W; bitpos: [8]; default: 1;
         *  Configures whether or not to enable input of SDIO_CMD.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_cmd_fun_ie:1;
        /** sdio_cmd_fun_drv : R/W; bitpos: [10:9]; default: 2;
         *  Configures the drive strength of SDIO_CMD.
         *  0: ~5 mA
         *  1: ~10 mA
         *  2: ~20 mA
         *  3: ~40 mA
         */
        uint32_t sdio_cmd_fun_drv:2;
        /** sdio_cmd_hys_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable the hysteresis function of the pin when
         *  CNNT_IO_MUX_SDIO_CMD_HYS_SEL is set to 1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sdio_cmd_hys_en:1;
        /** sdio_cmd_hys_sel : R/W; bitpos: [12]; default: 0;
         *  Configures to choose the signal for enabling the hysteresis function for SDIO_CMD.
         *  0: Choose the output enable signal of eFuse
         *  1: Choose the output enable signal of CNNT_IO_MUX_SDIO_CMD_HYS_EN
         */
        uint32_t sdio_cmd_hys_sel:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} cnnt_io_mux_sdio_cmd_reg_t;

/** Type of ctrl register
 *  SDIO IO MUX configuration register for ctrl sel
 */
typedef union {
    struct {
        /** sdio_pad_pin_ctrl_ded_sel : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to use dedicated ctrl signals for SDIO PAD.
         *  0: Not use dedicated ctrl signals
         *  1: Use dedicated ctrl signals
         */
        uint32_t sdio_pad_pin_ctrl_ded_sel:1;
        /** gmac_pad_pin_ctrl_ded_sel : R/W; bitpos: [1]; default: 0;
         *  Configures whether or not to use dedicated ctrl signals for GMAC PAD.
         *  0: Not use dedicated ctrl signals
         *  1: Use dedicated ctrl signals
         */
        uint32_t gmac_pad_pin_ctrl_ded_sel:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} cnnt_io_mux_ctrl_reg_t;


/** Group: Clock Gate Register */
/** Type of clock_gate register
 *  CNNT_IO_MUX clock gate register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether or not to enable clock gate.
         *  0: Not enable
         *  1: Enable, the clock is free running.
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} cnnt_io_mux_clock_gate_reg_t;


/** Group: Version Register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** reg_date : R/W; bitpos: [27:0]; default: 38834464;
         *  Version control register
         */
        uint32_t reg_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cnnt_io_mux_date_reg_t;


typedef struct {
    volatile cnnt_io_mux_gmac_rmii_clk_reg_t gmac_rmii_clk;
    volatile cnnt_io_mux_gmac_rx_clk_reg_t gmac_rx_clk;
    volatile cnnt_io_mux_gmac_phy_rxdv_reg_t gmac_phy_rxdv;
    volatile cnnt_io_mux_gmac_phy_rxd3_reg_t gmac_phy_rxd3;
    volatile cnnt_io_mux_gmac_phy_rxd2_reg_t gmac_phy_rxd2;
    volatile cnnt_io_mux_gmac_phy_rxd1_reg_t gmac_phy_rxd1;
    volatile cnnt_io_mux_gmac_phy_rxd0_reg_t gmac_phy_rxd0;
    volatile cnnt_io_mux_sdio_data0_reg_t sdio_data0;
    volatile cnnt_io_mux_sdio_data1_reg_t sdio_data1;
    volatile cnnt_io_mux_sdio_data2_reg_t sdio_data2;
    volatile cnnt_io_mux_sdio_data3_reg_t sdio_data3;
    volatile cnnt_io_mux_sdio_clk_reg_t sdio_clk;
    volatile cnnt_io_mux_sdio_cmd_reg_t sdio_cmd;
    uint32_t reserved_034[240];
    volatile cnnt_io_mux_ctrl_reg_t ctrl;
    volatile cnnt_io_mux_clock_gate_reg_t clock_gate;
    volatile cnnt_io_mux_date_reg_t date;
} cnnt_io_mux_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(cnnt_io_mux_dev_t) == 0x400, "Invalid size of cnnt_io_mux_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
