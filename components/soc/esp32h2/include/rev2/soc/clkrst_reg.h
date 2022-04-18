/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** SYSTEM_SYSCLK_CONF_REG register
 *  register description
 */
#define SYSTEM_SYSCLK_CONF_REG (DR_REG_CLKRST_BASE + 0x0)
/** SYSTEM_CLK_XTAL_FREQ : RO; bitpos: [7:0]; default: 0;
 *  Need add description
 */
#define SYSTEM_CLK_XTAL_FREQ    0x000000FFU
#define SYSTEM_CLK_XTAL_FREQ_M  (SYSTEM_CLK_XTAL_FREQ_V << SYSTEM_CLK_XTAL_FREQ_S)
#define SYSTEM_CLK_XTAL_FREQ_V  0x000000FFU
#define SYSTEM_CLK_XTAL_FREQ_S  0
/** SYSTEM_SPLL_FREQ : RO; bitpos: [15:8]; default: 0;
 *  Need add description
 */
#define SYSTEM_SPLL_FREQ    0x000000FFU
#define SYSTEM_SPLL_FREQ_M  (SYSTEM_SPLL_FREQ_V << SYSTEM_SPLL_FREQ_S)
#define SYSTEM_SPLL_FREQ_V  0x000000FFU
#define SYSTEM_SPLL_FREQ_S  8
/** SYSTEM_SOC_CLK_SEL : R/W; bitpos: [17:16]; default: 3;
 *  Need add description
 */
#define SYSTEM_SOC_CLK_SEL    0x00000003U
#define SYSTEM_SOC_CLK_SEL_M  (SYSTEM_SOC_CLK_SEL_V << SYSTEM_SOC_CLK_SEL_S)
#define SYSTEM_SOC_CLK_SEL_V  0x00000003U
#define SYSTEM_SOC_CLK_SEL_S  16

/** SYSTEM_CPUCLK_CONF_REG register
 *  register description
 */
#define SYSTEM_CPUCLK_CONF_REG (DR_REG_CLKRST_BASE + 0x4)
/** SYSTEM_CPU_DIV_NUM : R/W; bitpos: [7:0]; default: 0;
 *  Need add description
 */
#define SYSTEM_CPU_DIV_NUM    0x000000FFU
#define SYSTEM_CPU_DIV_NUM_M  (SYSTEM_CPU_DIV_NUM_V << SYSTEM_CPU_DIV_NUM_S)
#define SYSTEM_CPU_DIV_NUM_V  0x000000FFU
#define SYSTEM_CPU_DIV_NUM_S  0

#define SYSTEM_PRE_DIV_CNT  SYSTEM_CPU_DIV_NUM
#define SYSTEM_PRE_DIV_CNT_M  SYSTEM_CPU_DIV_NUM_M
#define SYSTEM_PRE_DIV_CNT_V  SYSTEM_CPU_DIV_NUM_V
#define SYSTEM_PRE_DIV_CNT_S  SYSTEM_CPU_DIV_NUM_S

/** SYSTEM_CPU_DIV_NUMERATOR : R/W; bitpos: [13:8]; default: 0;
 *  Need add description
 */
#define SYSTEM_CPU_DIV_NUMERATOR    0x0000003FU
#define SYSTEM_CPU_DIV_NUMERATOR_M  (SYSTEM_CPU_DIV_NUMERATOR_V << SYSTEM_CPU_DIV_NUMERATOR_S)
#define SYSTEM_CPU_DIV_NUMERATOR_V  0x0000003FU
#define SYSTEM_CPU_DIV_NUMERATOR_S  8
/** SYSTEM_CPU_DIV_DENOMINATOR : R/W; bitpos: [21:16]; default: 0;
 *  Need add description
 */
#define SYSTEM_CPU_DIV_DENOMINATOR    0x0000003FU
#define SYSTEM_CPU_DIV_DENOMINATOR_M  (SYSTEM_CPU_DIV_DENOMINATOR_V << SYSTEM_CPU_DIV_DENOMINATOR_S)
#define SYSTEM_CPU_DIV_DENOMINATOR_V  0x0000003FU
#define SYSTEM_CPU_DIV_DENOMINATOR_S  16

/** SYSTEM_BUSCLK_CONF_REG register
 *  register description
 */
#define SYSTEM_BUSCLK_CONF_REG (DR_REG_CLKRST_BASE + 0x8)
/** SYSTEM_APB_DIV_NUM : R/W; bitpos: [7:0]; default: 0;
 *  Need add description
 */
#define SYSTEM_APB_DIV_NUM    0x000000FFU
#define SYSTEM_APB_DIV_NUM_M  (SYSTEM_APB_DIV_NUM_V << SYSTEM_APB_DIV_NUM_S)
#define SYSTEM_APB_DIV_NUM_V  0x000000FFU
#define SYSTEM_APB_DIV_NUM_S  0
/** SYSTEM_AHB_DIV_NUM : R/W; bitpos: [15:8]; default: 0;
 *  Need add description
 */
#define SYSTEM_AHB_DIV_NUM    0x000000FFU
#define SYSTEM_AHB_DIV_NUM_M  (SYSTEM_AHB_DIV_NUM_V << SYSTEM_AHB_DIV_NUM_S)
#define SYSTEM_AHB_DIV_NUM_V  0x000000FFU
#define SYSTEM_AHB_DIV_NUM_S  8

/** SYSTEM_MODCLK_CONF_REG register
 *  register description
 */
#define SYSTEM_MODCLK_CONF_REG (DR_REG_CLKRST_BASE + 0xc)
/** SYSTEM_MODEM_CLK_SEL : R/W; bitpos: [1:0]; default: 1;
 *  Need add description
 */
#define SYSTEM_MODEM_CLK_SEL    0x00000003U
#define SYSTEM_MODEM_CLK_SEL_M  (SYSTEM_MODEM_CLK_SEL_V << SYSTEM_MODEM_CLK_SEL_S)
#define SYSTEM_MODEM_CLK_SEL_V  0x00000003U
#define SYSTEM_MODEM_CLK_SEL_S  0
/** SYSTEM_ETM_CLK_SEL : R/W; bitpos: [2]; default: 0;
 *  Need add description
 */
#define SYSTEM_ETM_CLK_SEL    (BIT(2))
#define SYSTEM_ETM_CLK_SEL_M  (SYSTEM_ETM_CLK_SEL_V << SYSTEM_ETM_CLK_SEL_S)
#define SYSTEM_ETM_CLK_SEL_V  0x00000001U
#define SYSTEM_ETM_CLK_SEL_S  2
/** SYSTEM_ETM_CLK_ACTIVE : R/W; bitpos: [3]; default: 0;
 *  Need add description
 */
#define SYSTEM_ETM_CLK_ACTIVE    (BIT(3))
#define SYSTEM_ETM_CLK_ACTIVE_M  (SYSTEM_ETM_CLK_ACTIVE_V << SYSTEM_ETM_CLK_ACTIVE_S)
#define SYSTEM_ETM_CLK_ACTIVE_V  0x00000001U
#define SYSTEM_ETM_CLK_ACTIVE_S  3
/** SYSTEM_COEX_LPCLK_SEL : R/W; bitpos: [5:4]; default: 0;
 *  Need add description
 */
#define SYSTEM_COEX_LPCLK_SEL    0x00000003U
#define SYSTEM_COEX_LPCLK_SEL_M  (SYSTEM_COEX_LPCLK_SEL_V << SYSTEM_COEX_LPCLK_SEL_S)
#define SYSTEM_COEX_LPCLK_SEL_V  0x00000003U
#define SYSTEM_COEX_LPCLK_SEL_S  4
/** SYSTEM_COEX_LPCLK_DIV : R/W; bitpos: [15:6]; default: 999;
 *  Need add description
 */
#define SYSTEM_COEX_LPCLK_DIV    0x000003FFU
#define SYSTEM_COEX_LPCLK_DIV_M  (SYSTEM_COEX_LPCLK_DIV_V << SYSTEM_COEX_LPCLK_DIV_S)
#define SYSTEM_COEX_LPCLK_DIV_V  0x000003FFU
#define SYSTEM_COEX_LPCLK_DIV_S  6
/** SYSTEM_BT_DFM_CLK_INV_PHASE : R/W; bitpos: [17:16]; default: 0;
 *  Need add description
 */
#define SYSTEM_BT_DFM_CLK_INV_PHASE    0x00000003U
#define SYSTEM_BT_DFM_CLK_INV_PHASE_M  (SYSTEM_BT_DFM_CLK_INV_PHASE_V << SYSTEM_BT_DFM_CLK_INV_PHASE_S)
#define SYSTEM_BT_DFM_CLK_INV_PHASE_V  0x00000003U
#define SYSTEM_BT_DFM_CLK_INV_PHASE_S  16

/** SYSTEM_CLK_OUT_EN_REG register
 *  register description
 */
#define SYSTEM_CLK_OUT_EN_REG (DR_REG_CLKRST_BASE + 0x10)
/** SYSTEM_CLK_8M_BT_OEN : R/W; bitpos: [3]; default: 1;
 *  Need add description
 */
#define SYSTEM_CLK_8M_BT_OEN    (BIT(3))
#define SYSTEM_CLK_8M_BT_OEN_M  (SYSTEM_CLK_8M_BT_OEN_V << SYSTEM_CLK_8M_BT_OEN_S)
#define SYSTEM_CLK_8M_BT_OEN_V  0x00000001U
#define SYSTEM_CLK_8M_BT_OEN_S  3
/** SYSTEM_CLK_16M_BT_OEN : R/W; bitpos: [4]; default: 1;
 *  Need add description
 */
#define SYSTEM_CLK_16M_BT_OEN    (BIT(4))
#define SYSTEM_CLK_16M_BT_OEN_M  (SYSTEM_CLK_16M_BT_OEN_V << SYSTEM_CLK_16M_BT_OEN_S)
#define SYSTEM_CLK_16M_BT_OEN_V  0x00000001U
#define SYSTEM_CLK_16M_BT_OEN_S  4
/** SYSTEM_CLK_32M_BT_OEN : R/W; bitpos: [5]; default: 1;
 *  Need add description
 */
#define SYSTEM_CLK_32M_BT_OEN    (BIT(5))
#define SYSTEM_CLK_32M_BT_OEN_M  (SYSTEM_CLK_32M_BT_OEN_V << SYSTEM_CLK_32M_BT_OEN_S)
#define SYSTEM_CLK_32M_BT_OEN_V  0x00000001U
#define SYSTEM_CLK_32M_BT_OEN_S  5
/** SYSTEM_CLK_APB_OEN : R/W; bitpos: [6]; default: 1;
 *  Need add description
 */
#define SYSTEM_CLK_APB_OEN    (BIT(6))
#define SYSTEM_CLK_APB_OEN_M  (SYSTEM_CLK_APB_OEN_V << SYSTEM_CLK_APB_OEN_S)
#define SYSTEM_CLK_APB_OEN_V  0x00000001U
#define SYSTEM_CLK_APB_OEN_S  6
/** SYSTEM_CLK_AHB_OEN : R/W; bitpos: [7]; default: 1;
 *  Need add description
 */
#define SYSTEM_CLK_AHB_OEN    (BIT(7))
#define SYSTEM_CLK_AHB_OEN_M  (SYSTEM_CLK_AHB_OEN_V << SYSTEM_CLK_AHB_OEN_S)
#define SYSTEM_CLK_AHB_OEN_V  0x00000001U
#define SYSTEM_CLK_AHB_OEN_S  7
/** SYSTEM_CLK_CPU_OEN : R/W; bitpos: [8]; default: 1;
 *  Need add description
 */
#define SYSTEM_CLK_CPU_OEN    (BIT(8))
#define SYSTEM_CLK_CPU_OEN_M  (SYSTEM_CLK_CPU_OEN_V << SYSTEM_CLK_CPU_OEN_S)
#define SYSTEM_CLK_CPU_OEN_V  0x00000001U
#define SYSTEM_CLK_CPU_OEN_S  8
/** SYSTEM_CLK_SPLL_OEN : R/W; bitpos: [9]; default: 1;
 *  Need add description
 */
#define SYSTEM_CLK_SPLL_OEN    (BIT(9))
#define SYSTEM_CLK_SPLL_OEN_M  (SYSTEM_CLK_SPLL_OEN_V << SYSTEM_CLK_SPLL_OEN_S)
#define SYSTEM_CLK_SPLL_OEN_V  0x00000001U
#define SYSTEM_CLK_SPLL_OEN_S  9
/** SYSTEM_CLK_XTAL_OEN : R/W; bitpos: [10]; default: 1;
 *  Need add description
 */
#define SYSTEM_CLK_XTAL_OEN    (BIT(10))
#define SYSTEM_CLK_XTAL_OEN_M  (SYSTEM_CLK_XTAL_OEN_V << SYSTEM_CLK_XTAL_OEN_S)
#define SYSTEM_CLK_XTAL_OEN_V  0x00000001U
#define SYSTEM_CLK_XTAL_OEN_S  10
/** SYSTEM_CLK_RFDAC_OEN : R/W; bitpos: [11]; default: 1;
 *  Need add description
 */
#define SYSTEM_CLK_RFDAC_OEN    (BIT(11))
#define SYSTEM_CLK_RFDAC_OEN_M  (SYSTEM_CLK_RFDAC_OEN_V << SYSTEM_CLK_RFDAC_OEN_S)
#define SYSTEM_CLK_RFDAC_OEN_V  0x00000001U
#define SYSTEM_CLK_RFDAC_OEN_S  11
/** SYSTEM_CLK_RFADC_OEN : R/W; bitpos: [12]; default: 1;
 *  Need add description
 */
#define SYSTEM_CLK_RFADC_OEN    (BIT(12))
#define SYSTEM_CLK_RFADC_OEN_M  (SYSTEM_CLK_RFADC_OEN_V << SYSTEM_CLK_RFADC_OEN_S)
#define SYSTEM_CLK_RFADC_OEN_V  0x00000001U
#define SYSTEM_CLK_RFADC_OEN_S  12

/** SYSTEM_MODEM_CLK_EN_REG register
 *  register description
 */
#define SYSTEM_MODEM_CLK_EN_REG (DR_REG_CLKRST_BASE + 0x14)
/** SYSTEM_FE_CAL_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Need add description
 */
#define SYSTEM_FE_CAL_CLK_EN    (BIT(0))
#define SYSTEM_FE_CAL_CLK_EN_M  (SYSTEM_FE_CAL_CLK_EN_V << SYSTEM_FE_CAL_CLK_EN_S)
#define SYSTEM_FE_CAL_CLK_EN_V  0x00000001U
#define SYSTEM_FE_CAL_CLK_EN_S  0
/** SYSTEM_FE_CLK_EN : R/W; bitpos: [1]; default: 0;
 *  Need add description
 */
#define SYSTEM_FE_CLK_EN    (BIT(1))
#define SYSTEM_FE_CLK_EN_M  (SYSTEM_FE_CLK_EN_V << SYSTEM_FE_CLK_EN_S)
#define SYSTEM_FE_CLK_EN_V  0x00000001U
#define SYSTEM_FE_CLK_EN_S  1
/** SYSTEM_MAC_CLK_EN : R/W; bitpos: [2]; default: 0;
 *  Need add description
 */
#define SYSTEM_MAC_CLK_EN    (BIT(2))
#define SYSTEM_MAC_CLK_EN_M  (SYSTEM_MAC_CLK_EN_V << SYSTEM_MAC_CLK_EN_S)
#define SYSTEM_MAC_CLK_EN_V  0x00000001U
#define SYSTEM_MAC_CLK_EN_S  2
/** SYSTEM_BT_CLK_EN : R/W; bitpos: [3]; default: 0;
 *  Need add description
 */
#define SYSTEM_BT_CLK_EN    (BIT(3))
#define SYSTEM_BT_CLK_EN_M  (SYSTEM_BT_CLK_EN_V << SYSTEM_BT_CLK_EN_S)
#define SYSTEM_BT_CLK_EN_V  0x00000001U
#define SYSTEM_BT_CLK_EN_S  3
/** SYSTEM_BTMAC_CLK_EN : R/W; bitpos: [4]; default: 0;
 *  Need add description
 */
#define SYSTEM_BTMAC_CLK_EN    (BIT(4))
#define SYSTEM_BTMAC_CLK_EN_M  (SYSTEM_BTMAC_CLK_EN_V << SYSTEM_BTMAC_CLK_EN_S)
#define SYSTEM_BTMAC_CLK_EN_V  0x00000001U
#define SYSTEM_BTMAC_CLK_EN_S  4
/** SYSTEM_SDIO_CLK_EN : R/W; bitpos: [5]; default: 1;
 *  Need add description
 */
#define SYSTEM_SDIO_CLK_EN    (BIT(5))
#define SYSTEM_SDIO_CLK_EN_M  (SYSTEM_SDIO_CLK_EN_V << SYSTEM_SDIO_CLK_EN_S)
#define SYSTEM_SDIO_CLK_EN_V  0x00000001U
#define SYSTEM_SDIO_CLK_EN_S  5
/** SYSTEM_EMAC_CLK_EN : R/W; bitpos: [6]; default: 1;
 *  Need add description
 */
#define SYSTEM_EMAC_CLK_EN    (BIT(6))
#define SYSTEM_EMAC_CLK_EN_M  (SYSTEM_EMAC_CLK_EN_V << SYSTEM_EMAC_CLK_EN_S)
#define SYSTEM_EMAC_CLK_EN_V  0x00000001U
#define SYSTEM_EMAC_CLK_EN_S  6
/** SYSTEM_MACPWR_CLK_EN : R/W; bitpos: [7]; default: 1;
 *  Need add description
 */
#define SYSTEM_MACPWR_CLK_EN    (BIT(7))
#define SYSTEM_MACPWR_CLK_EN_M  (SYSTEM_MACPWR_CLK_EN_V << SYSTEM_MACPWR_CLK_EN_S)
#define SYSTEM_MACPWR_CLK_EN_V  0x00000001U
#define SYSTEM_MACPWR_CLK_EN_S  7
/** SYSTEM_RW_BTMAC_CLK_EN : R/W; bitpos: [8]; default: 0;
 *  Need add description
 */
#define SYSTEM_RW_BTMAC_CLK_EN    (BIT(8))
#define SYSTEM_RW_BTMAC_CLK_EN_M  (SYSTEM_RW_BTMAC_CLK_EN_V << SYSTEM_RW_BTMAC_CLK_EN_S)
#define SYSTEM_RW_BTMAC_CLK_EN_V  0x00000001U
#define SYSTEM_RW_BTMAC_CLK_EN_S  8
/** SYSTEM_I2C_CLK_EN : R/W; bitpos: [9]; default: 1;
 *  Need add description
 */
#define SYSTEM_I2C_CLK_EN    (BIT(9))
#define SYSTEM_I2C_CLK_EN_M  (SYSTEM_I2C_CLK_EN_V << SYSTEM_I2C_CLK_EN_S)
#define SYSTEM_I2C_CLK_EN_V  0x00000001U
#define SYSTEM_I2C_CLK_EN_S  9
/** SYSTEM_I2CMST_CLK_EN : R/W; bitpos: [10]; default: 1;
 *  Need add description
 */
#define SYSTEM_I2CMST_CLK_EN    (BIT(10))
#define SYSTEM_I2CMST_CLK_EN_M  (SYSTEM_I2CMST_CLK_EN_V << SYSTEM_I2CMST_CLK_EN_S)
#define SYSTEM_I2CMST_CLK_EN_V  0x00000001U
#define SYSTEM_I2CMST_CLK_EN_S  10
/** SYSTEM_COEX_CLK_EN : R/W; bitpos: [11]; default: 0;
 *  Need add description
 */
#define SYSTEM_COEX_CLK_EN    (BIT(11))
#define SYSTEM_COEX_CLK_EN_M  (SYSTEM_COEX_CLK_EN_V << SYSTEM_COEX_CLK_EN_S)
#define SYSTEM_COEX_CLK_EN_V  0x00000001U
#define SYSTEM_COEX_CLK_EN_S  11
/** SYSTEM_IEEE802154BB_CLK_EN : R/W; bitpos: [12]; default: 0;
 *  Need add description
 */
#define SYSTEM_IEEE802154BB_CLK_EN    (BIT(12))
#define SYSTEM_IEEE802154BB_CLK_EN_M  (SYSTEM_IEEE802154BB_CLK_EN_V << SYSTEM_IEEE802154BB_CLK_EN_S)
#define SYSTEM_IEEE802154BB_CLK_EN_V  0x00000001U
#define SYSTEM_IEEE802154BB_CLK_EN_S  12
/** SYSTEM_IEEE802154MAC_CLK_EN : R/W; bitpos: [13]; default: 0;
 *  Need add description
 */
#define SYSTEM_IEEE802154MAC_CLK_EN    (BIT(13))
#define SYSTEM_IEEE802154MAC_CLK_EN_M  (SYSTEM_IEEE802154MAC_CLK_EN_V << SYSTEM_IEEE802154MAC_CLK_EN_S)
#define SYSTEM_IEEE802154MAC_CLK_EN_V  0x00000001U
#define SYSTEM_IEEE802154MAC_CLK_EN_S  13
/** SYSTEM_BLE_SEC_ECB_CLK_EN : R/W; bitpos: [14]; default: 0;
 *  Need add description
 */
#define SYSTEM_BLE_SEC_ECB_CLK_EN    (BIT(14))
#define SYSTEM_BLE_SEC_ECB_CLK_EN_M  (SYSTEM_BLE_SEC_ECB_CLK_EN_V << SYSTEM_BLE_SEC_ECB_CLK_EN_S)
#define SYSTEM_BLE_SEC_ECB_CLK_EN_V  0x00000001U
#define SYSTEM_BLE_SEC_ECB_CLK_EN_S  14
/** SYSTEM_BLE_SEC_CCM_CLK_EN : R/W; bitpos: [15]; default: 0;
 *  Need add description
 */
#define SYSTEM_BLE_SEC_CCM_CLK_EN    (BIT(15))
#define SYSTEM_BLE_SEC_CCM_CLK_EN_M  (SYSTEM_BLE_SEC_CCM_CLK_EN_V << SYSTEM_BLE_SEC_CCM_CLK_EN_S)
#define SYSTEM_BLE_SEC_CCM_CLK_EN_V  0x00000001U
#define SYSTEM_BLE_SEC_CCM_CLK_EN_S  15
/** SYSTEM_BLE_SEC_AAR_CLK_EN : R/W; bitpos: [16]; default: 0;
 *  Need add description
 */
#define SYSTEM_BLE_SEC_AAR_CLK_EN    (BIT(16))
#define SYSTEM_BLE_SEC_AAR_CLK_EN_M  (SYSTEM_BLE_SEC_AAR_CLK_EN_V << SYSTEM_BLE_SEC_AAR_CLK_EN_S)
#define SYSTEM_BLE_SEC_AAR_CLK_EN_V  0x00000001U
#define SYSTEM_BLE_SEC_AAR_CLK_EN_S  16
/** SYSTEM_BLE_SEC_CLK_EN : R/W; bitpos: [17]; default: 0;
 *  Need add description
 */
#define SYSTEM_BLE_SEC_CLK_EN    (BIT(17))
#define SYSTEM_BLE_SEC_CLK_EN_M  (SYSTEM_BLE_SEC_CLK_EN_V << SYSTEM_BLE_SEC_CLK_EN_S)
#define SYSTEM_BLE_SEC_CLK_EN_V  0x00000001U
#define SYSTEM_BLE_SEC_CLK_EN_S  17
/** SYSTEM_BTLC_CLK_EN : R/W; bitpos: [18]; default: 0;
 *  Need add description
 */
#define SYSTEM_BTLC_CLK_EN    (BIT(18))
#define SYSTEM_BTLC_CLK_EN_M  (SYSTEM_BTLC_CLK_EN_V << SYSTEM_BTLC_CLK_EN_S)
#define SYSTEM_BTLC_CLK_EN_V  0x00000001U
#define SYSTEM_BTLC_CLK_EN_S  18
/** SYSTEM_RFDAC_CLK_EN : R/W; bitpos: [19]; default: 0;
 *  Need add description
 */
#define SYSTEM_RFDAC_CLK_EN    (BIT(19))
#define SYSTEM_RFDAC_CLK_EN_M  (SYSTEM_RFDAC_CLK_EN_V << SYSTEM_RFDAC_CLK_EN_S)
#define SYSTEM_RFDAC_CLK_EN_V  0x00000001U
#define SYSTEM_RFDAC_CLK_EN_S  19
/** SYSTEM_RFADC_CLK_EN : R/W; bitpos: [20]; default: 0;
 *  Need add description
 */
#define SYSTEM_RFADC_CLK_EN    (BIT(20))
#define SYSTEM_RFADC_CLK_EN_M  (SYSTEM_RFADC_CLK_EN_V << SYSTEM_RFADC_CLK_EN_S)
#define SYSTEM_RFADC_CLK_EN_V  0x00000001U
#define SYSTEM_RFADC_CLK_EN_S  20
/** SYSTEM_DATA_DUMP_CLK_EN : R/W; bitpos: [21]; default: 0;
 *  Need add description
 */
#define SYSTEM_DATA_DUMP_CLK_EN    (BIT(21))
#define SYSTEM_DATA_DUMP_CLK_EN_M  (SYSTEM_DATA_DUMP_CLK_EN_V << SYSTEM_DATA_DUMP_CLK_EN_S)
#define SYSTEM_DATA_DUMP_CLK_EN_V  0x00000001U
#define SYSTEM_DATA_DUMP_CLK_EN_S  21
/** SYSTEM_BT_DFM_CLK_EN : R/W; bitpos: [22]; default: 0;
 *  Need add description
 */
#define SYSTEM_BT_DFM_CLK_EN    (BIT(22))
#define SYSTEM_BT_DFM_CLK_EN_M  (SYSTEM_BT_DFM_CLK_EN_V << SYSTEM_BT_DFM_CLK_EN_S)
#define SYSTEM_BT_DFM_CLK_EN_V  0x00000001U
#define SYSTEM_BT_DFM_CLK_EN_S  22

/** SYSTEM_MODEM_RST_EN_REG register
 *  register description
 */
#define SYSTEM_MODEM_RST_EN_REG (DR_REG_CLKRST_BASE + 0x18)
/** SYSTEM_FE_RST : R/W; bitpos: [0]; default: 0;
 *  Need add description
 */
#define SYSTEM_FE_RST    (BIT(0))
#define SYSTEM_FE_RST_M  (SYSTEM_FE_RST_V << SYSTEM_FE_RST_S)
#define SYSTEM_FE_RST_V  0x00000001U
#define SYSTEM_FE_RST_S  0
/** SYSTEM_MAC_RST : R/W; bitpos: [1]; default: 0;
 *  Need add description
 */
#define SYSTEM_MAC_RST    (BIT(1))
#define SYSTEM_MAC_RST_M  (SYSTEM_MAC_RST_V << SYSTEM_MAC_RST_S)
#define SYSTEM_MAC_RST_V  0x00000001U
#define SYSTEM_MAC_RST_S  1
/** SYSTEM_BT_RST : R/W; bitpos: [2]; default: 0;
 *  Need add description
 */
#define SYSTEM_BT_RST    (BIT(2))
#define SYSTEM_BT_RST_M  (SYSTEM_BT_RST_V << SYSTEM_BT_RST_S)
#define SYSTEM_BT_RST_V  0x00000001U
#define SYSTEM_BT_RST_S  2
/** SYSTEM_BTMAC_RST : R/W; bitpos: [3]; default: 0;
 *  Need add description
 */
#define SYSTEM_BTMAC_RST    (BIT(3))
#define SYSTEM_BTMAC_RST_M  (SYSTEM_BTMAC_RST_V << SYSTEM_BTMAC_RST_S)
#define SYSTEM_BTMAC_RST_V  0x00000001U
#define SYSTEM_BTMAC_RST_S  3
/** SYSTEM_SDIO_RST : R/W; bitpos: [4]; default: 0;
 *  Need add description
 */
#define SYSTEM_SDIO_RST    (BIT(4))
#define SYSTEM_SDIO_RST_M  (SYSTEM_SDIO_RST_V << SYSTEM_SDIO_RST_S)
#define SYSTEM_SDIO_RST_V  0x00000001U
#define SYSTEM_SDIO_RST_S  4
/** SYSTEM_EMAC_RST : R/W; bitpos: [5]; default: 0;
 *  Need add description
 */
#define SYSTEM_EMAC_RST    (BIT(5))
#define SYSTEM_EMAC_RST_M  (SYSTEM_EMAC_RST_V << SYSTEM_EMAC_RST_S)
#define SYSTEM_EMAC_RST_V  0x00000001U
#define SYSTEM_EMAC_RST_S  5
/** SYSTEM_MACPWR_RST : R/W; bitpos: [6]; default: 0;
 *  Need add description
 */
#define SYSTEM_MACPWR_RST    (BIT(6))
#define SYSTEM_MACPWR_RST_M  (SYSTEM_MACPWR_RST_V << SYSTEM_MACPWR_RST_S)
#define SYSTEM_MACPWR_RST_V  0x00000001U
#define SYSTEM_MACPWR_RST_S  6
/** SYSTEM_RW_BTMAC_RST : R/W; bitpos: [7]; default: 0;
 *  Need add description
 */
#define SYSTEM_RW_BTMAC_RST    (BIT(7))
#define SYSTEM_RW_BTMAC_RST_M  (SYSTEM_RW_BTMAC_RST_V << SYSTEM_RW_BTMAC_RST_S)
#define SYSTEM_RW_BTMAC_RST_V  0x00000001U
#define SYSTEM_RW_BTMAC_RST_S  7
/** SYSTEM_RW_BTLP_RST : R/W; bitpos: [8]; default: 0;
 *  Need add description
 */
#define SYSTEM_RW_BTLP_RST    (BIT(8))
#define SYSTEM_RW_BTLP_RST_M  (SYSTEM_RW_BTLP_RST_V << SYSTEM_RW_BTLP_RST_S)
#define SYSTEM_RW_BTLP_RST_V  0x00000001U
#define SYSTEM_RW_BTLP_RST_S  8
/** SYSTEM_RW_BTREG_RST : R/W; bitpos: [9]; default: 0;
 *  Need add description
 */
#define SYSTEM_RW_BTREG_RST    (BIT(9))
#define SYSTEM_RW_BTREG_RST_M  (SYSTEM_RW_BTREG_RST_V << SYSTEM_RW_BTREG_RST_S)
#define SYSTEM_RW_BTREG_RST_V  0x00000001U
#define SYSTEM_RW_BTREG_RST_S  9
/** SYSTEM_RW_BTLPREG_RST : R/W; bitpos: [10]; default: 0;
 *  Need add description
 */
#define SYSTEM_RW_BTLPREG_RST    (BIT(10))
#define SYSTEM_RW_BTLPREG_RST_M  (SYSTEM_RW_BTLPREG_RST_V << SYSTEM_RW_BTLPREG_RST_S)
#define SYSTEM_RW_BTLPREG_RST_V  0x00000001U
#define SYSTEM_RW_BTLPREG_RST_S  10
/** SYSTEM_BT_REG_RST : R/W; bitpos: [11]; default: 0;
 *  Need add description
 */
#define SYSTEM_BT_REG_RST    (BIT(11))
#define SYSTEM_BT_REG_RST_M  (SYSTEM_BT_REG_RST_V << SYSTEM_BT_REG_RST_S)
#define SYSTEM_BT_REG_RST_V  0x00000001U
#define SYSTEM_BT_REG_RST_S  11
/** SYSTEM_COEX_RST : R/W; bitpos: [12]; default: 0;
 *  Need add description
 */
#define SYSTEM_COEX_RST    (BIT(12))
#define SYSTEM_COEX_RST_M  (SYSTEM_COEX_RST_V << SYSTEM_COEX_RST_S)
#define SYSTEM_COEX_RST_V  0x00000001U
#define SYSTEM_COEX_RST_S  12
/** SYSTEM_IEEE802154BB_RST : R/W; bitpos: [13]; default: 0;
 *  Need add description
 */
#define SYSTEM_IEEE802154BB_RST    (BIT(13))
#define SYSTEM_IEEE802154BB_RST_M  (SYSTEM_IEEE802154BB_RST_V << SYSTEM_IEEE802154BB_RST_S)
#define SYSTEM_IEEE802154BB_RST_V  0x00000001U
#define SYSTEM_IEEE802154BB_RST_S  13
/** SYSTEM_IEEE802154MAC_RST : R/W; bitpos: [14]; default: 0;
 *  Need add description
 */
#define SYSTEM_IEEE802154MAC_RST    (BIT(14))
#define SYSTEM_IEEE802154MAC_RST_M  (SYSTEM_IEEE802154MAC_RST_V << SYSTEM_IEEE802154MAC_RST_S)
#define SYSTEM_IEEE802154MAC_RST_V  0x00000001U
#define SYSTEM_IEEE802154MAC_RST_S  14
/** SYSTEM_BLE_SEC_ECB_RST : R/W; bitpos: [15]; default: 0;
 *  Need add description
 */
#define SYSTEM_BLE_SEC_ECB_RST    (BIT(15))
#define SYSTEM_BLE_SEC_ECB_RST_M  (SYSTEM_BLE_SEC_ECB_RST_V << SYSTEM_BLE_SEC_ECB_RST_S)
#define SYSTEM_BLE_SEC_ECB_RST_V  0x00000001U
#define SYSTEM_BLE_SEC_ECB_RST_S  15
/** SYSTEM_BLE_SEC_CCM_RST : R/W; bitpos: [16]; default: 0;
 *  Need add description
 */
#define SYSTEM_BLE_SEC_CCM_RST    (BIT(16))
#define SYSTEM_BLE_SEC_CCM_RST_M  (SYSTEM_BLE_SEC_CCM_RST_V << SYSTEM_BLE_SEC_CCM_RST_S)
#define SYSTEM_BLE_SEC_CCM_RST_V  0x00000001U
#define SYSTEM_BLE_SEC_CCM_RST_S  16
/** SYSTEM_BLE_SEC_AAR_RST : R/W; bitpos: [17]; default: 0;
 *  Need add description
 */
#define SYSTEM_BLE_SEC_AAR_RST    (BIT(17))
#define SYSTEM_BLE_SEC_AAR_RST_M  (SYSTEM_BLE_SEC_AAR_RST_V << SYSTEM_BLE_SEC_AAR_RST_S)
#define SYSTEM_BLE_SEC_AAR_RST_V  0x00000001U
#define SYSTEM_BLE_SEC_AAR_RST_S  17
/** SYSTEM_BLE_SEC_RST : R/W; bitpos: [18]; default: 0;
 *  Need add description
 */
#define SYSTEM_BLE_SEC_RST    (BIT(18))
#define SYSTEM_BLE_SEC_RST_M  (SYSTEM_BLE_SEC_RST_V << SYSTEM_BLE_SEC_RST_S)
#define SYSTEM_BLE_SEC_RST_V  0x00000001U
#define SYSTEM_BLE_SEC_RST_S  18
/** SYSTEM_APB_RET_RST : R/W; bitpos: [19]; default: 0;
 *  Need add description
 */
#define SYSTEM_APB_RET_RST    (BIT(19))
#define SYSTEM_APB_RET_RST_M  (SYSTEM_APB_RET_RST_V << SYSTEM_APB_RET_RST_S)
#define SYSTEM_APB_RET_RST_V  0x00000001U
#define SYSTEM_APB_RET_RST_S  19
/** SYSTEM_DATA_DUMP_RST : R/W; bitpos: [20]; default: 0;
 *  Need add description
 */
#define SYSTEM_DATA_DUMP_RST    (BIT(20))
#define SYSTEM_DATA_DUMP_RST_M  (SYSTEM_DATA_DUMP_RST_V << SYSTEM_DATA_DUMP_RST_S)
#define SYSTEM_DATA_DUMP_RST_V  0x00000001U
#define SYSTEM_DATA_DUMP_RST_S  20

/** SYSTEM_PERIP_CLK_CONF_REG register
 *  register description
 */
#define SYSTEM_PERIP_CLK_CONF_REG (DR_REG_CLKRST_BASE + 0x1c)
/** SYSTEM_SEC_DIV_NUM : R/W; bitpos: [7:0]; default: 1;
 *  Need add description
 */
#define SYSTEM_SEC_DIV_NUM    0x000000FFU
#define SYSTEM_SEC_DIV_NUM_M  (SYSTEM_SEC_DIV_NUM_V << SYSTEM_SEC_DIV_NUM_S)
#define SYSTEM_SEC_DIV_NUM_V  0x000000FFU
#define SYSTEM_SEC_DIV_NUM_S  0
/** SYSTEM_USB_DEVICE_DIV_NUM : R/W; bitpos: [15:8]; default: 1;
 *  Need add description
 */
#define SYSTEM_USB_DEVICE_DIV_NUM    0x000000FFU
#define SYSTEM_USB_DEVICE_DIV_NUM_M  (SYSTEM_USB_DEVICE_DIV_NUM_V << SYSTEM_USB_DEVICE_DIV_NUM_S)
#define SYSTEM_USB_DEVICE_DIV_NUM_V  0x000000FFU
#define SYSTEM_USB_DEVICE_DIV_NUM_S  8
/** SYSTEM_TWAI_DIV_NUM : R/W; bitpos: [23:16]; default: 1;
 *  Need add description
 */
#define SYSTEM_TWAI_DIV_NUM    0x000000FFU
#define SYSTEM_TWAI_DIV_NUM_M  (SYSTEM_TWAI_DIV_NUM_V << SYSTEM_TWAI_DIV_NUM_S)
#define SYSTEM_TWAI_DIV_NUM_V  0x000000FFU
#define SYSTEM_TWAI_DIV_NUM_S  16
/** SYSTEM_MSPI_DIV_NUM : R/W; bitpos: [31:24]; default: 1;
 *  Need add description
 */
#define SYSTEM_MSPI_DIV_NUM    0x000000FFU
#define SYSTEM_MSPI_DIV_NUM_M  (SYSTEM_MSPI_DIV_NUM_V << SYSTEM_MSPI_DIV_NUM_S)
#define SYSTEM_MSPI_DIV_NUM_V  0x000000FFU
#define SYSTEM_MSPI_DIV_NUM_S  24

/** SYSTEM_PERIP_CLK_EN0_REG register
 *  register description
 */
#define SYSTEM_PERIP_CLK_EN0_REG (DR_REG_CLKRST_BASE + 0x20)
/** SYSTEM_TIMERS_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Need add description
 */
#define SYSTEM_TIMERS_CLK_EN    (BIT(0))
#define SYSTEM_TIMERS_CLK_EN_M  (SYSTEM_TIMERS_CLK_EN_V << SYSTEM_TIMERS_CLK_EN_S)
#define SYSTEM_TIMERS_CLK_EN_V  0x00000001U
#define SYSTEM_TIMERS_CLK_EN_S  0
/** SYSTEM_SPI01_CLK_EN : R/W; bitpos: [1]; default: 1;
 *  Need add description
 */
#define SYSTEM_SPI01_CLK_EN    (BIT(1))
#define SYSTEM_SPI01_CLK_EN_M  (SYSTEM_SPI01_CLK_EN_V << SYSTEM_SPI01_CLK_EN_S)
#define SYSTEM_SPI01_CLK_EN_V  0x00000001U
#define SYSTEM_SPI01_CLK_EN_S  1
/** SYSTEM_UART_CLK_EN : R/W; bitpos: [2]; default: 1;
 *  Need add description
 */
#define SYSTEM_UART_CLK_EN    (BIT(2))
#define SYSTEM_UART_CLK_EN_M  (SYSTEM_UART_CLK_EN_V << SYSTEM_UART_CLK_EN_S)
#define SYSTEM_UART_CLK_EN_V  0x00000001U
#define SYSTEM_UART_CLK_EN_S  2
/** SYSTEM_WDG_CLK_EN : R/W; bitpos: [3]; default: 1;
 *  Need add description
 */
#define SYSTEM_WDG_CLK_EN    (BIT(3))
#define SYSTEM_WDG_CLK_EN_M  (SYSTEM_WDG_CLK_EN_V << SYSTEM_WDG_CLK_EN_S)
#define SYSTEM_WDG_CLK_EN_V  0x00000001U
#define SYSTEM_WDG_CLK_EN_S  3
/** SYSTEM_I2S0_CLK_EN : R/W; bitpos: [4]; default: 0;
 *  Need add description
 */
#define SYSTEM_I2S0_CLK_EN    (BIT(4))
#define SYSTEM_I2S0_CLK_EN_M  (SYSTEM_I2S0_CLK_EN_V << SYSTEM_I2S0_CLK_EN_S)
#define SYSTEM_I2S0_CLK_EN_V  0x00000001U
#define SYSTEM_I2S0_CLK_EN_S  4
/** SYSTEM_UART1_CLK_EN : R/W; bitpos: [5]; default: 1;
 *  Need add description
 */
#define SYSTEM_UART1_CLK_EN    (BIT(5))
#define SYSTEM_UART1_CLK_EN_M  (SYSTEM_UART1_CLK_EN_V << SYSTEM_UART1_CLK_EN_S)
#define SYSTEM_UART1_CLK_EN_V  0x00000001U
#define SYSTEM_UART1_CLK_EN_S  5
/** SYSTEM_SPI2_CLK_EN : R/W; bitpos: [6]; default: 1;
 *  Need add description
 */
#define SYSTEM_SPI2_CLK_EN    (BIT(6))
#define SYSTEM_SPI2_CLK_EN_M  (SYSTEM_SPI2_CLK_EN_V << SYSTEM_SPI2_CLK_EN_S)
#define SYSTEM_SPI2_CLK_EN_V  0x00000001U
#define SYSTEM_SPI2_CLK_EN_S  6
/** SYSTEM_I2C_EXT0_CLK_EN : R/W; bitpos: [7]; default: 0;
 *  Need add description
 */
#define SYSTEM_I2C_EXT0_CLK_EN    (BIT(7))
#define SYSTEM_I2C_EXT0_CLK_EN_M  (SYSTEM_I2C_EXT0_CLK_EN_V << SYSTEM_I2C_EXT0_CLK_EN_S)
#define SYSTEM_I2C_EXT0_CLK_EN_V  0x00000001U
#define SYSTEM_I2C_EXT0_CLK_EN_S  7
/** SYSTEM_UHCI0_CLK_EN : R/W; bitpos: [8]; default: 0;
 *  Need add description
 */
#define SYSTEM_UHCI0_CLK_EN    (BIT(8))
#define SYSTEM_UHCI0_CLK_EN_M  (SYSTEM_UHCI0_CLK_EN_V << SYSTEM_UHCI0_CLK_EN_S)
#define SYSTEM_UHCI0_CLK_EN_V  0x00000001U
#define SYSTEM_UHCI0_CLK_EN_S  8
/** SYSTEM_RMT_CLK_EN : R/W; bitpos: [9]; default: 0;
 *  Need add description
 */
#define SYSTEM_RMT_CLK_EN    (BIT(9))
#define SYSTEM_RMT_CLK_EN_M  (SYSTEM_RMT_CLK_EN_V << SYSTEM_RMT_CLK_EN_S)
#define SYSTEM_RMT_CLK_EN_V  0x00000001U
#define SYSTEM_RMT_CLK_EN_S  9
/** SYSTEM_PCNT_CLK_EN : R/W; bitpos: [10]; default: 0;
 *  Need add description
 */
#define SYSTEM_PCNT_CLK_EN    (BIT(10))
#define SYSTEM_PCNT_CLK_EN_M  (SYSTEM_PCNT_CLK_EN_V << SYSTEM_PCNT_CLK_EN_S)
#define SYSTEM_PCNT_CLK_EN_V  0x00000001U
#define SYSTEM_PCNT_CLK_EN_S  10
/** SYSTEM_LEDC_CLK_EN : R/W; bitpos: [11]; default: 0;
 *  Need add description
 */
#define SYSTEM_LEDC_CLK_EN    (BIT(11))
#define SYSTEM_LEDC_CLK_EN_M  (SYSTEM_LEDC_CLK_EN_V << SYSTEM_LEDC_CLK_EN_S)
#define SYSTEM_LEDC_CLK_EN_V  0x00000001U
#define SYSTEM_LEDC_CLK_EN_S  11
/** SYSTEM_UHCI1_CLK_EN : R/W; bitpos: [12]; default: 0;
 *  Need add description
 */
#define SYSTEM_UHCI1_CLK_EN    (BIT(12))
#define SYSTEM_UHCI1_CLK_EN_M  (SYSTEM_UHCI1_CLK_EN_V << SYSTEM_UHCI1_CLK_EN_S)
#define SYSTEM_UHCI1_CLK_EN_V  0x00000001U
#define SYSTEM_UHCI1_CLK_EN_S  12
/** SYSTEM_TIMERGROUP_CLK_EN : R/W; bitpos: [13]; default: 1;
 *  Need add description
 */
#define SYSTEM_TIMERGROUP_CLK_EN    (BIT(13))
#define SYSTEM_TIMERGROUP_CLK_EN_M  (SYSTEM_TIMERGROUP_CLK_EN_V << SYSTEM_TIMERGROUP_CLK_EN_S)
#define SYSTEM_TIMERGROUP_CLK_EN_V  0x00000001U
#define SYSTEM_TIMERGROUP_CLK_EN_S  13
/** SYSTEM_EFUSE_CLK_EN : R/W; bitpos: [14]; default: 1;
 *  Need add description
 */
#define SYSTEM_EFUSE_CLK_EN    (BIT(14))
#define SYSTEM_EFUSE_CLK_EN_M  (SYSTEM_EFUSE_CLK_EN_V << SYSTEM_EFUSE_CLK_EN_S)
#define SYSTEM_EFUSE_CLK_EN_V  0x00000001U
#define SYSTEM_EFUSE_CLK_EN_S  14
/** SYSTEM_TIMERGROUP1_CLK_EN : R/W; bitpos: [15]; default: 1;
 *  Need add description
 */
#define SYSTEM_TIMERGROUP1_CLK_EN    (BIT(15))
#define SYSTEM_TIMERGROUP1_CLK_EN_M  (SYSTEM_TIMERGROUP1_CLK_EN_V << SYSTEM_TIMERGROUP1_CLK_EN_S)
#define SYSTEM_TIMERGROUP1_CLK_EN_V  0x00000001U
#define SYSTEM_TIMERGROUP1_CLK_EN_S  15
/** SYSTEM_SPI3_CLK_EN : R/W; bitpos: [16]; default: 1;
 *  Need add description
 */
#define SYSTEM_SPI3_CLK_EN    (BIT(16))
#define SYSTEM_SPI3_CLK_EN_M  (SYSTEM_SPI3_CLK_EN_V << SYSTEM_SPI3_CLK_EN_S)
#define SYSTEM_SPI3_CLK_EN_V  0x00000001U
#define SYSTEM_SPI3_CLK_EN_S  16
/** SYSTEM_PWM0_CLK_EN : R/W; bitpos: [17]; default: 0;
 *  Need add description
 */
#define SYSTEM_PWM0_CLK_EN    (BIT(17))
#define SYSTEM_PWM0_CLK_EN_M  (SYSTEM_PWM0_CLK_EN_V << SYSTEM_PWM0_CLK_EN_S)
#define SYSTEM_PWM0_CLK_EN_V  0x00000001U
#define SYSTEM_PWM0_CLK_EN_S  17
/** SYSTEM_I2C_EXT1_CLK_EN : R/W; bitpos: [18]; default: 0;
 *  Need add description
 */
#define SYSTEM_I2C_EXT1_CLK_EN    (BIT(18))
#define SYSTEM_I2C_EXT1_CLK_EN_M  (SYSTEM_I2C_EXT1_CLK_EN_V << SYSTEM_I2C_EXT1_CLK_EN_S)
#define SYSTEM_I2C_EXT1_CLK_EN_V  0x00000001U
#define SYSTEM_I2C_EXT1_CLK_EN_S  18
/** SYSTEM_TWAI_CLK_EN : R/W; bitpos: [19]; default: 0;
 *  Need add description
 */
#define SYSTEM_TWAI_CLK_EN    (BIT(19))
#define SYSTEM_TWAI_CLK_EN_M  (SYSTEM_TWAI_CLK_EN_V << SYSTEM_TWAI_CLK_EN_S)
#define SYSTEM_TWAI_CLK_EN_V  0x00000001U
#define SYSTEM_TWAI_CLK_EN_S  19
/** SYSTEM_PWM1_CLK_EN : R/W; bitpos: [20]; default: 0;
 *  Need add description
 */
#define SYSTEM_PWM1_CLK_EN    (BIT(20))
#define SYSTEM_PWM1_CLK_EN_M  (SYSTEM_PWM1_CLK_EN_V << SYSTEM_PWM1_CLK_EN_S)
#define SYSTEM_PWM1_CLK_EN_V  0x00000001U
#define SYSTEM_PWM1_CLK_EN_S  20
/** SYSTEM_I2S1_CLK_EN : R/W; bitpos: [21]; default: 0;
 *  Need add description
 */
#define SYSTEM_I2S1_CLK_EN    (BIT(21))
#define SYSTEM_I2S1_CLK_EN_M  (SYSTEM_I2S1_CLK_EN_V << SYSTEM_I2S1_CLK_EN_S)
#define SYSTEM_I2S1_CLK_EN_V  0x00000001U
#define SYSTEM_I2S1_CLK_EN_S  21
/** SYSTEM_SPI2_DMA_CLK_EN : R/W; bitpos: [22]; default: 1;
 *  Need add description
 */
#define SYSTEM_SPI2_DMA_CLK_EN    (BIT(22))
#define SYSTEM_SPI2_DMA_CLK_EN_M  (SYSTEM_SPI2_DMA_CLK_EN_V << SYSTEM_SPI2_DMA_CLK_EN_S)
#define SYSTEM_SPI2_DMA_CLK_EN_V  0x00000001U
#define SYSTEM_SPI2_DMA_CLK_EN_S  22
/** SYSTEM_USB_DEVICE_CLK_EN : R/W; bitpos: [23]; default: 1;
 *  Need add description
 */
#define SYSTEM_USB_DEVICE_CLK_EN    (BIT(23))
#define SYSTEM_USB_DEVICE_CLK_EN_M  (SYSTEM_USB_DEVICE_CLK_EN_V << SYSTEM_USB_DEVICE_CLK_EN_S)
#define SYSTEM_USB_DEVICE_CLK_EN_V  0x00000001U
#define SYSTEM_USB_DEVICE_CLK_EN_S  23
/** SYSTEM_UART_MEM_CLK_EN : R/W; bitpos: [24]; default: 1;
 *  Need add description
 */
#define SYSTEM_UART_MEM_CLK_EN    (BIT(24))
#define SYSTEM_UART_MEM_CLK_EN_M  (SYSTEM_UART_MEM_CLK_EN_V << SYSTEM_UART_MEM_CLK_EN_S)
#define SYSTEM_UART_MEM_CLK_EN_V  0x00000001U
#define SYSTEM_UART_MEM_CLK_EN_S  24
/** SYSTEM_PWM2_CLK_EN : R/W; bitpos: [25]; default: 0;
 *  Need add description
 */
#define SYSTEM_PWM2_CLK_EN    (BIT(25))
#define SYSTEM_PWM2_CLK_EN_M  (SYSTEM_PWM2_CLK_EN_V << SYSTEM_PWM2_CLK_EN_S)
#define SYSTEM_PWM2_CLK_EN_V  0x00000001U
#define SYSTEM_PWM2_CLK_EN_S  25
/** SYSTEM_PWM3_CLK_EN : R/W; bitpos: [26]; default: 0;
 *  Need add description
 */
#define SYSTEM_PWM3_CLK_EN    (BIT(26))
#define SYSTEM_PWM3_CLK_EN_M  (SYSTEM_PWM3_CLK_EN_V << SYSTEM_PWM3_CLK_EN_S)
#define SYSTEM_PWM3_CLK_EN_V  0x00000001U
#define SYSTEM_PWM3_CLK_EN_S  26
/** SYSTEM_SPI3_DMA_CLK_EN : R/W; bitpos: [27]; default: 1;
 *  Need add description
 */
#define SYSTEM_SPI3_DMA_CLK_EN    (BIT(27))
#define SYSTEM_SPI3_DMA_CLK_EN_M  (SYSTEM_SPI3_DMA_CLK_EN_V << SYSTEM_SPI3_DMA_CLK_EN_S)
#define SYSTEM_SPI3_DMA_CLK_EN_V  0x00000001U
#define SYSTEM_SPI3_DMA_CLK_EN_S  27
/** SYSTEM_APB_SARADC_CLK_EN : R/W; bitpos: [28]; default: 1;
 *  Need add description
 */
#define SYSTEM_APB_SARADC_CLK_EN    (BIT(28))
#define SYSTEM_APB_SARADC_CLK_EN_M  (SYSTEM_APB_SARADC_CLK_EN_V << SYSTEM_APB_SARADC_CLK_EN_S)
#define SYSTEM_APB_SARADC_CLK_EN_V  0x00000001U
#define SYSTEM_APB_SARADC_CLK_EN_S  28
/** SYSTEM_SYSTIMER_CLK_EN : R/W; bitpos: [29]; default: 1;
 *  Need add description
 */
#define SYSTEM_SYSTIMER_CLK_EN    (BIT(29))
#define SYSTEM_SYSTIMER_CLK_EN_M  (SYSTEM_SYSTIMER_CLK_EN_V << SYSTEM_SYSTIMER_CLK_EN_S)
#define SYSTEM_SYSTIMER_CLK_EN_V  0x00000001U
#define SYSTEM_SYSTIMER_CLK_EN_S  29
/** SYSTEM_ADC2_ARB_CLK_EN : R/W; bitpos: [30]; default: 1;
 *  Need add description
 */
#define SYSTEM_ADC2_ARB_CLK_EN    (BIT(30))
#define SYSTEM_ADC2_ARB_CLK_EN_M  (SYSTEM_ADC2_ARB_CLK_EN_V << SYSTEM_ADC2_ARB_CLK_EN_S)
#define SYSTEM_ADC2_ARB_CLK_EN_V  0x00000001U
#define SYSTEM_ADC2_ARB_CLK_EN_S  30
/** SYSTEM_SPI4_CLK_EN : R/W; bitpos: [31]; default: 1;
 *  Need add description
 */
#define SYSTEM_SPI4_CLK_EN    (BIT(31))
#define SYSTEM_SPI4_CLK_EN_M  (SYSTEM_SPI4_CLK_EN_V << SYSTEM_SPI4_CLK_EN_S)
#define SYSTEM_SPI4_CLK_EN_V  0x00000001U
#define SYSTEM_SPI4_CLK_EN_S  31

/** SYSTEM_PERIP_CLK_EN1_REG register
 *  register description
 */
#define SYSTEM_PERIP_CLK_EN1_REG (DR_REG_CLKRST_BASE + 0x24)
/** SYSTEM_RETENTION_TOP_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Need add description
 */
#define SYSTEM_RETENTION_TOP_CLK_EN    (BIT(0))
#define SYSTEM_RETENTION_TOP_CLK_EN_M  (SYSTEM_RETENTION_TOP_CLK_EN_V << SYSTEM_RETENTION_TOP_CLK_EN_S)
#define SYSTEM_RETENTION_TOP_CLK_EN_V  0x00000001U
#define SYSTEM_RETENTION_TOP_CLK_EN_S  0
/** SYSTEM_CRYPTO_AES_CLK_EN : R/W; bitpos: [1]; default: 0;
 *  Need add description
 */
#define SYSTEM_CRYPTO_AES_CLK_EN    (BIT(1))
#define SYSTEM_CRYPTO_AES_CLK_EN_M  (SYSTEM_CRYPTO_AES_CLK_EN_V << SYSTEM_CRYPTO_AES_CLK_EN_S)
#define SYSTEM_CRYPTO_AES_CLK_EN_V  0x00000001U
#define SYSTEM_CRYPTO_AES_CLK_EN_S  1
/** SYSTEM_CRYPTO_SHA_CLK_EN : R/W; bitpos: [2]; default: 0;
 *  Need add description
 */
#define SYSTEM_CRYPTO_SHA_CLK_EN    (BIT(2))
#define SYSTEM_CRYPTO_SHA_CLK_EN_M  (SYSTEM_CRYPTO_SHA_CLK_EN_V << SYSTEM_CRYPTO_SHA_CLK_EN_S)
#define SYSTEM_CRYPTO_SHA_CLK_EN_V  0x00000001U
#define SYSTEM_CRYPTO_SHA_CLK_EN_S  2
/** SYSTEM_CRYPTO_RSA_CLK_EN : R/W; bitpos: [3]; default: 0;
 *  Need add description
 */
#define SYSTEM_CRYPTO_RSA_CLK_EN    (BIT(3))
#define SYSTEM_CRYPTO_RSA_CLK_EN_M  (SYSTEM_CRYPTO_RSA_CLK_EN_V << SYSTEM_CRYPTO_RSA_CLK_EN_S)
#define SYSTEM_CRYPTO_RSA_CLK_EN_V  0x00000001U
#define SYSTEM_CRYPTO_RSA_CLK_EN_S  3
/** SYSTEM_CRYPTO_DS_CLK_EN : R/W; bitpos: [4]; default: 0;
 *  Need add description
 */
#define SYSTEM_CRYPTO_DS_CLK_EN    (BIT(4))
#define SYSTEM_CRYPTO_DS_CLK_EN_M  (SYSTEM_CRYPTO_DS_CLK_EN_V << SYSTEM_CRYPTO_DS_CLK_EN_S)
#define SYSTEM_CRYPTO_DS_CLK_EN_V  0x00000001U
#define SYSTEM_CRYPTO_DS_CLK_EN_S  4
/** SYSTEM_CRYPTO_HMAC_CLK_EN : R/W; bitpos: [5]; default: 0;
 *  Need add description
 */
#define SYSTEM_CRYPTO_HMAC_CLK_EN    (BIT(5))
#define SYSTEM_CRYPTO_HMAC_CLK_EN_M  (SYSTEM_CRYPTO_HMAC_CLK_EN_V << SYSTEM_CRYPTO_HMAC_CLK_EN_S)
#define SYSTEM_CRYPTO_HMAC_CLK_EN_V  0x00000001U
#define SYSTEM_CRYPTO_HMAC_CLK_EN_S  5
/** SYSTEM_CRYPTO_ECC_CLK_EN : R/W; bitpos: [6]; default: 0;
 *  Need add description
 */
#define SYSTEM_CRYPTO_ECC_CLK_EN    (BIT(6))
#define SYSTEM_CRYPTO_ECC_CLK_EN_M  (SYSTEM_CRYPTO_ECC_CLK_EN_V << SYSTEM_CRYPTO_ECC_CLK_EN_S)
#define SYSTEM_CRYPTO_ECC_CLK_EN_V  0x00000001U
#define SYSTEM_CRYPTO_ECC_CLK_EN_S  6
/** SYSTEM_DMA_CLK_EN : R/W; bitpos: [7]; default: 0;
 *  Need add description
 */
#define SYSTEM_DMA_CLK_EN    (BIT(7))
#define SYSTEM_DMA_CLK_EN_M  (SYSTEM_DMA_CLK_EN_V << SYSTEM_DMA_CLK_EN_S)
#define SYSTEM_DMA_CLK_EN_V  0x00000001U
#define SYSTEM_DMA_CLK_EN_S  7
/** SYSTEM_SDIO_HOST_CLK_EN : R/W; bitpos: [8]; default: 0;
 *  Need add description
 */
#define SYSTEM_SDIO_HOST_CLK_EN    (BIT(8))
#define SYSTEM_SDIO_HOST_CLK_EN_M  (SYSTEM_SDIO_HOST_CLK_EN_V << SYSTEM_SDIO_HOST_CLK_EN_S)
#define SYSTEM_SDIO_HOST_CLK_EN_V  0x00000001U
#define SYSTEM_SDIO_HOST_CLK_EN_S  8
/** SYSTEM_LCD_CAM_CLK_EN : R/W; bitpos: [9]; default: 0;
 *  Need add description
 */
#define SYSTEM_LCD_CAM_CLK_EN    (BIT(9))
#define SYSTEM_LCD_CAM_CLK_EN_M  (SYSTEM_LCD_CAM_CLK_EN_V << SYSTEM_LCD_CAM_CLK_EN_S)
#define SYSTEM_LCD_CAM_CLK_EN_V  0x00000001U
#define SYSTEM_LCD_CAM_CLK_EN_S  9
/** SYSTEM_UART2_CLK_EN : R/W; bitpos: [10]; default: 1;
 *  Need add description
 */
#define SYSTEM_UART2_CLK_EN    (BIT(10))
#define SYSTEM_UART2_CLK_EN_M  (SYSTEM_UART2_CLK_EN_V << SYSTEM_UART2_CLK_EN_S)
#define SYSTEM_UART2_CLK_EN_V  0x00000001U
#define SYSTEM_UART2_CLK_EN_S  10
/** SYSTEM_TSENS_CLK_EN : R/W; bitpos: [11]; default: 0;
 *  Need add description
 */
#define SYSTEM_TSENS_CLK_EN    (BIT(11))
#define SYSTEM_TSENS_CLK_EN_M  (SYSTEM_TSENS_CLK_EN_V << SYSTEM_TSENS_CLK_EN_S)
#define SYSTEM_TSENS_CLK_EN_V  0x00000001U
#define SYSTEM_TSENS_CLK_EN_S  11
/** SYSTEM_ETM_CLK_EN : R/W; bitpos: [12]; default: 0;
 *  Need add description
 */
#define SYSTEM_ETM_CLK_EN    (BIT(12))
#define SYSTEM_ETM_CLK_EN_M  (SYSTEM_ETM_CLK_EN_V << SYSTEM_ETM_CLK_EN_S)
#define SYSTEM_ETM_CLK_EN_V  0x00000001U
#define SYSTEM_ETM_CLK_EN_S  12
/** SYSTEM_TIMERGROUP3_CLK_EN : R/W; bitpos: [13]; default: 0;
 *  Need add description
 */
#define SYSTEM_TIMERGROUP3_CLK_EN    (BIT(13))
#define SYSTEM_TIMERGROUP3_CLK_EN_M  (SYSTEM_TIMERGROUP3_CLK_EN_V << SYSTEM_TIMERGROUP3_CLK_EN_S)
#define SYSTEM_TIMERGROUP3_CLK_EN_V  0x00000001U
#define SYSTEM_TIMERGROUP3_CLK_EN_S  13
/** SYSTEM_REGRET_CLK_EN : R/W; bitpos: [14]; default: 0;
 *  Need add description
 */
#define SYSTEM_REGRET_CLK_EN    (BIT(14))
#define SYSTEM_REGRET_CLK_EN_M  (SYSTEM_REGRET_CLK_EN_V << SYSTEM_REGRET_CLK_EN_S)
#define SYSTEM_REGRET_CLK_EN_V  0x00000001U
#define SYSTEM_REGRET_CLK_EN_S  14
/** SYSTEM_PVT_CLK_EN : R/W; bitpos: [15]; default: 1;
 *  Need add description
 */
#define SYSTEM_PVT_CLK_EN    (BIT(15))
#define SYSTEM_PVT_CLK_EN_M  (SYSTEM_PVT_CLK_EN_V << SYSTEM_PVT_CLK_EN_S)
#define SYSTEM_PVT_CLK_EN_V  0x00000001U
#define SYSTEM_PVT_CLK_EN_S  15

/** SYSTEM_PERIP_RST_EN0_REG register
 *  register description
 */
#define SYSTEM_PERIP_RST_EN0_REG (DR_REG_CLKRST_BASE + 0x28)
/** SYSTEM_TIMERS_RST : R/W; bitpos: [0]; default: 0;
 *  Need add description
 */
#define SYSTEM_TIMERS_RST    (BIT(0))
#define SYSTEM_TIMERS_RST_M  (SYSTEM_TIMERS_RST_V << SYSTEM_TIMERS_RST_S)
#define SYSTEM_TIMERS_RST_V  0x00000001U
#define SYSTEM_TIMERS_RST_S  0
/** SYSTEM_SPI01_RST : R/W; bitpos: [1]; default: 0;
 *  Need add description
 */
#define SYSTEM_SPI01_RST    (BIT(1))
#define SYSTEM_SPI01_RST_M  (SYSTEM_SPI01_RST_V << SYSTEM_SPI01_RST_S)
#define SYSTEM_SPI01_RST_V  0x00000001U
#define SYSTEM_SPI01_RST_S  1
/** SYSTEM_UART_RST : R/W; bitpos: [2]; default: 0;
 *  Need add description
 */
#define SYSTEM_UART_RST    (BIT(2))
#define SYSTEM_UART_RST_M  (SYSTEM_UART_RST_V << SYSTEM_UART_RST_S)
#define SYSTEM_UART_RST_V  0x00000001U
#define SYSTEM_UART_RST_S  2
/** SYSTEM_WDG_RST : R/W; bitpos: [3]; default: 0;
 *  Need add description
 */
#define SYSTEM_WDG_RST    (BIT(3))
#define SYSTEM_WDG_RST_M  (SYSTEM_WDG_RST_V << SYSTEM_WDG_RST_S)
#define SYSTEM_WDG_RST_V  0x00000001U
#define SYSTEM_WDG_RST_S  3
/** SYSTEM_I2S0_RST : R/W; bitpos: [4]; default: 0;
 *  Need add description
 */
#define SYSTEM_I2S0_RST    (BIT(4))
#define SYSTEM_I2S0_RST_M  (SYSTEM_I2S0_RST_V << SYSTEM_I2S0_RST_S)
#define SYSTEM_I2S0_RST_V  0x00000001U
#define SYSTEM_I2S0_RST_S  4
/** SYSTEM_UART1_RST : R/W; bitpos: [5]; default: 0;
 *  Need add description
 */
#define SYSTEM_UART1_RST    (BIT(5))
#define SYSTEM_UART1_RST_M  (SYSTEM_UART1_RST_V << SYSTEM_UART1_RST_S)
#define SYSTEM_UART1_RST_V  0x00000001U
#define SYSTEM_UART1_RST_S  5
/** SYSTEM_SPI2_RST : R/W; bitpos: [6]; default: 0;
 *  Need add description
 */
#define SYSTEM_SPI2_RST    (BIT(6))
#define SYSTEM_SPI2_RST_M  (SYSTEM_SPI2_RST_V << SYSTEM_SPI2_RST_S)
#define SYSTEM_SPI2_RST_V  0x00000001U
#define SYSTEM_SPI2_RST_S  6
/** SYSTEM_I2C_EXT0_RST : R/W; bitpos: [7]; default: 0;
 *  Need add description
 */
#define SYSTEM_I2C_EXT0_RST    (BIT(7))
#define SYSTEM_I2C_EXT0_RST_M  (SYSTEM_I2C_EXT0_RST_V << SYSTEM_I2C_EXT0_RST_S)
#define SYSTEM_I2C_EXT0_RST_V  0x00000001U
#define SYSTEM_I2C_EXT0_RST_S  7
/** SYSTEM_UHCI0_RST : R/W; bitpos: [8]; default: 0;
 *  Need add description
 */
#define SYSTEM_UHCI0_RST    (BIT(8))
#define SYSTEM_UHCI0_RST_M  (SYSTEM_UHCI0_RST_V << SYSTEM_UHCI0_RST_S)
#define SYSTEM_UHCI0_RST_V  0x00000001U
#define SYSTEM_UHCI0_RST_S  8
/** SYSTEM_RMT_RST : R/W; bitpos: [9]; default: 0;
 *  Need add description
 */
#define SYSTEM_RMT_RST    (BIT(9))
#define SYSTEM_RMT_RST_M  (SYSTEM_RMT_RST_V << SYSTEM_RMT_RST_S)
#define SYSTEM_RMT_RST_V  0x00000001U
#define SYSTEM_RMT_RST_S  9
/** SYSTEM_PCNT_RST : R/W; bitpos: [10]; default: 0;
 *  Need add description
 */
#define SYSTEM_PCNT_RST    (BIT(10))
#define SYSTEM_PCNT_RST_M  (SYSTEM_PCNT_RST_V << SYSTEM_PCNT_RST_S)
#define SYSTEM_PCNT_RST_V  0x00000001U
#define SYSTEM_PCNT_RST_S  10
/** SYSTEM_LEDC_RST : R/W; bitpos: [11]; default: 0;
 *  Need add description
 */
#define SYSTEM_LEDC_RST    (BIT(11))
#define SYSTEM_LEDC_RST_M  (SYSTEM_LEDC_RST_V << SYSTEM_LEDC_RST_S)
#define SYSTEM_LEDC_RST_V  0x00000001U
#define SYSTEM_LEDC_RST_S  11
/** SYSTEM_UHCI1_RST : R/W; bitpos: [12]; default: 0;
 *  Need add description
 */
#define SYSTEM_UHCI1_RST    (BIT(12))
#define SYSTEM_UHCI1_RST_M  (SYSTEM_UHCI1_RST_V << SYSTEM_UHCI1_RST_S)
#define SYSTEM_UHCI1_RST_V  0x00000001U
#define SYSTEM_UHCI1_RST_S  12
/** SYSTEM_TIMERGROUP_RST : R/W; bitpos: [13]; default: 0;
 *  Need add description
 */
#define SYSTEM_TIMERGROUP_RST    (BIT(13))
#define SYSTEM_TIMERGROUP_RST_M  (SYSTEM_TIMERGROUP_RST_V << SYSTEM_TIMERGROUP_RST_S)
#define SYSTEM_TIMERGROUP_RST_V  0x00000001U
#define SYSTEM_TIMERGROUP_RST_S  13
/** SYSTEM_EFUSE_RST : R/W; bitpos: [14]; default: 0;
 *  Need add description
 */
#define SYSTEM_EFUSE_RST    (BIT(14))
#define SYSTEM_EFUSE_RST_M  (SYSTEM_EFUSE_RST_V << SYSTEM_EFUSE_RST_S)
#define SYSTEM_EFUSE_RST_V  0x00000001U
#define SYSTEM_EFUSE_RST_S  14
/** SYSTEM_TIMERGROUP1_RST : R/W; bitpos: [15]; default: 0;
 *  Need add description
 */
#define SYSTEM_TIMERGROUP1_RST    (BIT(15))
#define SYSTEM_TIMERGROUP1_RST_M  (SYSTEM_TIMERGROUP1_RST_V << SYSTEM_TIMERGROUP1_RST_S)
#define SYSTEM_TIMERGROUP1_RST_V  0x00000001U
#define SYSTEM_TIMERGROUP1_RST_S  15
/** SYSTEM_SPI3_RST : R/W; bitpos: [16]; default: 0;
 *  Need add description
 */
#define SYSTEM_SPI3_RST    (BIT(16))
#define SYSTEM_SPI3_RST_M  (SYSTEM_SPI3_RST_V << SYSTEM_SPI3_RST_S)
#define SYSTEM_SPI3_RST_V  0x00000001U
#define SYSTEM_SPI3_RST_S  16
/** SYSTEM_PWM0_RST : R/W; bitpos: [17]; default: 0;
 *  Need add description
 */
#define SYSTEM_PWM0_RST    (BIT(17))
#define SYSTEM_PWM0_RST_M  (SYSTEM_PWM0_RST_V << SYSTEM_PWM0_RST_S)
#define SYSTEM_PWM0_RST_V  0x00000001U
#define SYSTEM_PWM0_RST_S  17
/** SYSTEM_I2C_EXT1_RST : R/W; bitpos: [18]; default: 0;
 *  Need add description
 */
#define SYSTEM_I2C_EXT1_RST    (BIT(18))
#define SYSTEM_I2C_EXT1_RST_M  (SYSTEM_I2C_EXT1_RST_V << SYSTEM_I2C_EXT1_RST_S)
#define SYSTEM_I2C_EXT1_RST_V  0x00000001U
#define SYSTEM_I2C_EXT1_RST_S  18
/** SYSTEM_TWAI_RST : R/W; bitpos: [19]; default: 0;
 *  Need add description
 */
#define SYSTEM_TWAI_RST    (BIT(19))
#define SYSTEM_TWAI_RST_M  (SYSTEM_TWAI_RST_V << SYSTEM_TWAI_RST_S)
#define SYSTEM_TWAI_RST_V  0x00000001U
#define SYSTEM_TWAI_RST_S  19
/** SYSTEM_PWM1_RST : R/W; bitpos: [20]; default: 0;
 *  Need add description
 */
#define SYSTEM_PWM1_RST    (BIT(20))
#define SYSTEM_PWM1_RST_M  (SYSTEM_PWM1_RST_V << SYSTEM_PWM1_RST_S)
#define SYSTEM_PWM1_RST_V  0x00000001U
#define SYSTEM_PWM1_RST_S  20
/** SYSTEM_I2S1_RST : R/W; bitpos: [21]; default: 0;
 *  Need add description
 */
#define SYSTEM_I2S1_RST    (BIT(21))
#define SYSTEM_I2S1_RST_M  (SYSTEM_I2S1_RST_V << SYSTEM_I2S1_RST_S)
#define SYSTEM_I2S1_RST_V  0x00000001U
#define SYSTEM_I2S1_RST_S  21
/** SYSTEM_SPI2_DMA_RST : R/W; bitpos: [22]; default: 0;
 *  Need add description
 */
#define SYSTEM_SPI2_DMA_RST    (BIT(22))
#define SYSTEM_SPI2_DMA_RST_M  (SYSTEM_SPI2_DMA_RST_V << SYSTEM_SPI2_DMA_RST_S)
#define SYSTEM_SPI2_DMA_RST_V  0x00000001U
#define SYSTEM_SPI2_DMA_RST_S  22
/** SYSTEM_USB_DEVICE_RST : R/W; bitpos: [23]; default: 0;
 *  Need add description
 */
#define SYSTEM_USB_DEVICE_RST    (BIT(23))
#define SYSTEM_USB_DEVICE_RST_M  (SYSTEM_USB_DEVICE_RST_V << SYSTEM_USB_DEVICE_RST_S)
#define SYSTEM_USB_DEVICE_RST_V  0x00000001U
#define SYSTEM_USB_DEVICE_RST_S  23
/** SYSTEM_UART_MEM_RST : R/W; bitpos: [24]; default: 0;
 *  Need add description
 */
#define SYSTEM_UART_MEM_RST    (BIT(24))
#define SYSTEM_UART_MEM_RST_M  (SYSTEM_UART_MEM_RST_V << SYSTEM_UART_MEM_RST_S)
#define SYSTEM_UART_MEM_RST_V  0x00000001U
#define SYSTEM_UART_MEM_RST_S  24
/** SYSTEM_PWM2_RST : R/W; bitpos: [25]; default: 0;
 *  Need add description
 */
#define SYSTEM_PWM2_RST    (BIT(25))
#define SYSTEM_PWM2_RST_M  (SYSTEM_PWM2_RST_V << SYSTEM_PWM2_RST_S)
#define SYSTEM_PWM2_RST_V  0x00000001U
#define SYSTEM_PWM2_RST_S  25
/** SYSTEM_PWM3_RST : R/W; bitpos: [26]; default: 0;
 *  Need add description
 */
#define SYSTEM_PWM3_RST    (BIT(26))
#define SYSTEM_PWM3_RST_M  (SYSTEM_PWM3_RST_V << SYSTEM_PWM3_RST_S)
#define SYSTEM_PWM3_RST_V  0x00000001U
#define SYSTEM_PWM3_RST_S  26
/** SYSTEM_SPI3_DMA_RST : R/W; bitpos: [27]; default: 0;
 *  Need add description
 */
#define SYSTEM_SPI3_DMA_RST    (BIT(27))
#define SYSTEM_SPI3_DMA_RST_M  (SYSTEM_SPI3_DMA_RST_V << SYSTEM_SPI3_DMA_RST_S)
#define SYSTEM_SPI3_DMA_RST_V  0x00000001U
#define SYSTEM_SPI3_DMA_RST_S  27
/** SYSTEM_APB_SARADC_RST : R/W; bitpos: [28]; default: 0;
 *  Need add description
 */
#define SYSTEM_APB_SARADC_RST    (BIT(28))
#define SYSTEM_APB_SARADC_RST_M  (SYSTEM_APB_SARADC_RST_V << SYSTEM_APB_SARADC_RST_S)
#define SYSTEM_APB_SARADC_RST_V  0x00000001U
#define SYSTEM_APB_SARADC_RST_S  28
/** SYSTEM_SYSTIMER_RST : R/W; bitpos: [29]; default: 0;
 *  Need add description
 */
#define SYSTEM_SYSTIMER_RST    (BIT(29))
#define SYSTEM_SYSTIMER_RST_M  (SYSTEM_SYSTIMER_RST_V << SYSTEM_SYSTIMER_RST_S)
#define SYSTEM_SYSTIMER_RST_V  0x00000001U
#define SYSTEM_SYSTIMER_RST_S  29
/** SYSTEM_ADC2_ARB_RST : R/W; bitpos: [30]; default: 0;
 *  Need add description
 */
#define SYSTEM_ADC2_ARB_RST    (BIT(30))
#define SYSTEM_ADC2_ARB_RST_M  (SYSTEM_ADC2_ARB_RST_V << SYSTEM_ADC2_ARB_RST_S)
#define SYSTEM_ADC2_ARB_RST_V  0x00000001U
#define SYSTEM_ADC2_ARB_RST_S  30
/** SYSTEM_SPI4_RST : R/W; bitpos: [31]; default: 0;
 *  Need add description
 */
#define SYSTEM_SPI4_RST    (BIT(31))
#define SYSTEM_SPI4_RST_M  (SYSTEM_SPI4_RST_V << SYSTEM_SPI4_RST_S)
#define SYSTEM_SPI4_RST_V  0x00000001U
#define SYSTEM_SPI4_RST_S  31

/** SYSTEM_PERIP_RST_EN1_REG register
 *  register description
 */
#define SYSTEM_PERIP_RST_EN1_REG (DR_REG_CLKRST_BASE + 0x2c)
/** SYSTEM_RETENTION_TOP_RST : R/W; bitpos: [0]; default: 0;
 *  Need add description
 */
#define SYSTEM_RETENTION_TOP_RST    (BIT(0))
#define SYSTEM_RETENTION_TOP_RST_M  (SYSTEM_RETENTION_TOP_RST_V << SYSTEM_RETENTION_TOP_RST_S)
#define SYSTEM_RETENTION_TOP_RST_V  0x00000001U
#define SYSTEM_RETENTION_TOP_RST_S  0
/** SYSTEM_CRYPTO_AES_RST : R/W; bitpos: [1]; default: 1;
 *  Need add description
 */
#define SYSTEM_CRYPTO_AES_RST    (BIT(1))
#define SYSTEM_CRYPTO_AES_RST_M  (SYSTEM_CRYPTO_AES_RST_V << SYSTEM_CRYPTO_AES_RST_S)
#define SYSTEM_CRYPTO_AES_RST_V  0x00000001U
#define SYSTEM_CRYPTO_AES_RST_S  1
/** SYSTEM_CRYPTO_SHA_RST : R/W; bitpos: [2]; default: 1;
 *  Need add description
 */
#define SYSTEM_CRYPTO_SHA_RST    (BIT(2))
#define SYSTEM_CRYPTO_SHA_RST_M  (SYSTEM_CRYPTO_SHA_RST_V << SYSTEM_CRYPTO_SHA_RST_S)
#define SYSTEM_CRYPTO_SHA_RST_V  0x00000001U
#define SYSTEM_CRYPTO_SHA_RST_S  2
/** SYSTEM_CRYPTO_RSA_RST : R/W; bitpos: [3]; default: 1;
 *  Need add description
 */
#define SYSTEM_CRYPTO_RSA_RST    (BIT(3))
#define SYSTEM_CRYPTO_RSA_RST_M  (SYSTEM_CRYPTO_RSA_RST_V << SYSTEM_CRYPTO_RSA_RST_S)
#define SYSTEM_CRYPTO_RSA_RST_V  0x00000001U
#define SYSTEM_CRYPTO_RSA_RST_S  3
/** SYSTEM_CRYPTO_DS_RST : R/W; bitpos: [4]; default: 1;
 *  Need add description
 */
#define SYSTEM_CRYPTO_DS_RST    (BIT(4))
#define SYSTEM_CRYPTO_DS_RST_M  (SYSTEM_CRYPTO_DS_RST_V << SYSTEM_CRYPTO_DS_RST_S)
#define SYSTEM_CRYPTO_DS_RST_V  0x00000001U
#define SYSTEM_CRYPTO_DS_RST_S  4
/** SYSTEM_CRYPTO_HMAC_RST : R/W; bitpos: [5]; default: 1;
 *  Need add description
 */
#define SYSTEM_CRYPTO_HMAC_RST    (BIT(5))
#define SYSTEM_CRYPTO_HMAC_RST_M  (SYSTEM_CRYPTO_HMAC_RST_V << SYSTEM_CRYPTO_HMAC_RST_S)
#define SYSTEM_CRYPTO_HMAC_RST_V  0x00000001U
#define SYSTEM_CRYPTO_HMAC_RST_S  5
/** SYSTEM_CRYPTO_ECC_RST : R/W; bitpos: [6]; default: 1;
 *  Need add description
 */
#define SYSTEM_CRYPTO_ECC_RST    (BIT(6))
#define SYSTEM_CRYPTO_ECC_RST_M  (SYSTEM_CRYPTO_ECC_RST_V << SYSTEM_CRYPTO_ECC_RST_S)
#define SYSTEM_CRYPTO_ECC_RST_V  0x00000001U
#define SYSTEM_CRYPTO_ECC_RST_S  6
/** SYSTEM_DMA_RST : R/W; bitpos: [7]; default: 1;
 *  Need add description
 */
#define SYSTEM_DMA_RST    (BIT(7))
#define SYSTEM_DMA_RST_M  (SYSTEM_DMA_RST_V << SYSTEM_DMA_RST_S)
#define SYSTEM_DMA_RST_V  0x00000001U
#define SYSTEM_DMA_RST_S  7
/** SYSTEM_SDIO_HOST_RST : R/W; bitpos: [8]; default: 1;
 *  Need add description
 */
#define SYSTEM_SDIO_HOST_RST    (BIT(8))
#define SYSTEM_SDIO_HOST_RST_M  (SYSTEM_SDIO_HOST_RST_V << SYSTEM_SDIO_HOST_RST_S)
#define SYSTEM_SDIO_HOST_RST_V  0x00000001U
#define SYSTEM_SDIO_HOST_RST_S  8
/** SYSTEM_LCD_CAM_RST : R/W; bitpos: [9]; default: 1;
 *  Need add description
 */
#define SYSTEM_LCD_CAM_RST    (BIT(9))
#define SYSTEM_LCD_CAM_RST_M  (SYSTEM_LCD_CAM_RST_V << SYSTEM_LCD_CAM_RST_S)
#define SYSTEM_LCD_CAM_RST_V  0x00000001U
#define SYSTEM_LCD_CAM_RST_S  9
/** SYSTEM_UART2_RST : R/W; bitpos: [10]; default: 0;
 *  Need add description
 */
#define SYSTEM_UART2_RST    (BIT(10))
#define SYSTEM_UART2_RST_M  (SYSTEM_UART2_RST_V << SYSTEM_UART2_RST_S)
#define SYSTEM_UART2_RST_V  0x00000001U
#define SYSTEM_UART2_RST_S  10
/** SYSTEM_TSENS_RST : R/W; bitpos: [11]; default: 0;
 *  Need add description
 */
#define SYSTEM_TSENS_RST    (BIT(11))
#define SYSTEM_TSENS_RST_M  (SYSTEM_TSENS_RST_V << SYSTEM_TSENS_RST_S)
#define SYSTEM_TSENS_RST_V  0x00000001U
#define SYSTEM_TSENS_RST_S  11
/** SYSTEM_ETM_RST : R/W; bitpos: [12]; default: 0;
 *  Need add description
 */
#define SYSTEM_ETM_RST    (BIT(12))
#define SYSTEM_ETM_RST_M  (SYSTEM_ETM_RST_V << SYSTEM_ETM_RST_S)
#define SYSTEM_ETM_RST_V  0x00000001U
#define SYSTEM_ETM_RST_S  12
/** SYSTEM_TIMERGROUP3_RST : R/W; bitpos: [13]; default: 0;
 *  Need add description
 */
#define SYSTEM_TIMERGROUP3_RST    (BIT(13))
#define SYSTEM_TIMERGROUP3_RST_M  (SYSTEM_TIMERGROUP3_RST_V << SYSTEM_TIMERGROUP3_RST_S)
#define SYSTEM_TIMERGROUP3_RST_V  0x00000001U
#define SYSTEM_TIMERGROUP3_RST_S  13
/** SYSTEM_REGRET_RST : R/W; bitpos: [14]; default: 0;
 *  Need add description
 */
#define SYSTEM_REGRET_RST    (BIT(14))
#define SYSTEM_REGRET_RST_M  (SYSTEM_REGRET_RST_V << SYSTEM_REGRET_RST_S)
#define SYSTEM_REGRET_RST_V  0x00000001U
#define SYSTEM_REGRET_RST_S  14
/** SYSTEM_PVT_RST : R/W; bitpos: [15]; default: 0;
 *  Need add description
 */
#define SYSTEM_PVT_RST    (BIT(15))
#define SYSTEM_PVT_RST_M  (SYSTEM_PVT_RST_V << SYSTEM_PVT_RST_S)
#define SYSTEM_PVT_RST_V  0x00000001U
#define SYSTEM_PVT_RST_S  15

/** SYSTEM_FPGA_DBG_REG register
 *  register description
 */
#define SYSTEM_FPGA_DBG_REG (DR_REG_CLKRST_BASE + 0x30)
/** SYSTEM_FPGA_DEBUG : R/W; bitpos: [31:0]; default: 4294967295;
 *  Need add description
 */
#define SYSTEM_FPGA_DEBUG    0xFFFFFFFFU
#define SYSTEM_FPGA_DEBUG_M  (SYSTEM_FPGA_DEBUG_V << SYSTEM_FPGA_DEBUG_S)
#define SYSTEM_FPGA_DEBUG_V  0xFFFFFFFFU
#define SYSTEM_FPGA_DEBUG_S  0

/** SYSTEM_REGCLK_CONF_REG register
 *  register description
 */
#define SYSTEM_REGCLK_CONF_REG (DR_REG_CLKRST_BASE + 0x34)
/** SYSTEM_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Need add description
 */
#define SYSTEM_CLK_EN    (BIT(0))
#define SYSTEM_CLK_EN_M  (SYSTEM_CLK_EN_V << SYSTEM_CLK_EN_S)
#define SYSTEM_CLK_EN_V  0x00000001U
#define SYSTEM_CLK_EN_S  0

/** SYSTEM_CLKRST_DATE_REG register
 *  register description
 */
#define SYSTEM_CLKRST_DATE_REG (DR_REG_SYSTEM_BASE + 0x38)
/** CLKRST_DATE : R/W; bitpos: [27:0]; default: 34672962;
 *  Need add description
 */
#define CLKRST_DATE    0x0FFFFFFFU
#define CLKRST_DATE_M  (CLKRST_DATE_V << CLKRST_DATE_S)
#define CLKRST_DATE_V  0x0FFFFFFFU
#define CLKRST_DATE_S  0

#ifdef __cplusplus
}
#endif
