/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** CNNT_IO_MUX_GMAC_RMII_CLK_REG register
 *  GMAC IO MUX configuration register for GMAC_RMII_CLK
 */
#define CNNT_IO_MUX_GMAC_RMII_CLK_REG (DR_REG_CNNT_IO_MUX_BASE + 0x0)
/** CNNT_IO_MUX_GMAC_RMII_CLK_SLP_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enter sleep mode for GMAC_RMII_CLK.
 *  0: Not enter
 *  1: Enter
 */
#define CNNT_IO_MUX_GMAC_RMII_CLK_SLP_SEL    (BIT(0))
#define CNNT_IO_MUX_GMAC_RMII_CLK_SLP_SEL_M  (CNNT_IO_MUX_GMAC_RMII_CLK_SLP_SEL_V << CNNT_IO_MUX_GMAC_RMII_CLK_SLP_SEL_S)
#define CNNT_IO_MUX_GMAC_RMII_CLK_SLP_SEL_V  0x00000001U
#define CNNT_IO_MUX_GMAC_RMII_CLK_SLP_SEL_S  0
/** CNNT_IO_MUX_GMAC_RMII_CLK_MCU_WPD : R/W; bitpos: [1]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GMAC_RMII_CLK in sleep
 *  mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_RMII_CLK_MCU_WPD    (BIT(1))
#define CNNT_IO_MUX_GMAC_RMII_CLK_MCU_WPD_M  (CNNT_IO_MUX_GMAC_RMII_CLK_MCU_WPD_V << CNNT_IO_MUX_GMAC_RMII_CLK_MCU_WPD_S)
#define CNNT_IO_MUX_GMAC_RMII_CLK_MCU_WPD_V  0x00000001U
#define CNNT_IO_MUX_GMAC_RMII_CLK_MCU_WPD_S  1
/** CNNT_IO_MUX_GMAC_RMII_CLK_MCU_WPU : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GMAC_RMII_CLK during sleep
 *  mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_RMII_CLK_MCU_WPU    (BIT(2))
#define CNNT_IO_MUX_GMAC_RMII_CLK_MCU_WPU_M  (CNNT_IO_MUX_GMAC_RMII_CLK_MCU_WPU_V << CNNT_IO_MUX_GMAC_RMII_CLK_MCU_WPU_S)
#define CNNT_IO_MUX_GMAC_RMII_CLK_MCU_WPU_V  0x00000001U
#define CNNT_IO_MUX_GMAC_RMII_CLK_MCU_WPU_S  2
/** CNNT_IO_MUX_GMAC_RMII_CLK_MCU_IE : R/W; bitpos: [3]; default: 1;
 *  Configures whether or not to enable the input of GMAC_RMII_CLK during sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_RMII_CLK_MCU_IE    (BIT(3))
#define CNNT_IO_MUX_GMAC_RMII_CLK_MCU_IE_M  (CNNT_IO_MUX_GMAC_RMII_CLK_MCU_IE_V << CNNT_IO_MUX_GMAC_RMII_CLK_MCU_IE_S)
#define CNNT_IO_MUX_GMAC_RMII_CLK_MCU_IE_V  0x00000001U
#define CNNT_IO_MUX_GMAC_RMII_CLK_MCU_IE_S  3
/** CNNT_IO_MUX_GMAC_RMII_CLK_MCU_DRV : R/W; bitpos: [5:4]; default: 2;
 *  Configures the drive strength of GMAC_RMII_CLK during sleep mode.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_GMAC_RMII_CLK_MCU_DRV    0x00000003U
#define CNNT_IO_MUX_GMAC_RMII_CLK_MCU_DRV_M  (CNNT_IO_MUX_GMAC_RMII_CLK_MCU_DRV_V << CNNT_IO_MUX_GMAC_RMII_CLK_MCU_DRV_S)
#define CNNT_IO_MUX_GMAC_RMII_CLK_MCU_DRV_V  0x00000003U
#define CNNT_IO_MUX_GMAC_RMII_CLK_MCU_DRV_S  4
/** CNNT_IO_MUX_GMAC_RMII_CLK_FUN_WPD : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GMAC_RMII_CLK.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_RMII_CLK_FUN_WPD    (BIT(6))
#define CNNT_IO_MUX_GMAC_RMII_CLK_FUN_WPD_M  (CNNT_IO_MUX_GMAC_RMII_CLK_FUN_WPD_V << CNNT_IO_MUX_GMAC_RMII_CLK_FUN_WPD_S)
#define CNNT_IO_MUX_GMAC_RMII_CLK_FUN_WPD_V  0x00000001U
#define CNNT_IO_MUX_GMAC_RMII_CLK_FUN_WPD_S  6
/** CNNT_IO_MUX_GMAC_RMII_CLK_FUN_WPU : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not enable pull-up resistor of GMAC_RMII_CLK.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_RMII_CLK_FUN_WPU    (BIT(7))
#define CNNT_IO_MUX_GMAC_RMII_CLK_FUN_WPU_M  (CNNT_IO_MUX_GMAC_RMII_CLK_FUN_WPU_V << CNNT_IO_MUX_GMAC_RMII_CLK_FUN_WPU_S)
#define CNNT_IO_MUX_GMAC_RMII_CLK_FUN_WPU_V  0x00000001U
#define CNNT_IO_MUX_GMAC_RMII_CLK_FUN_WPU_S  7
/** CNNT_IO_MUX_GMAC_RMII_CLK_FUN_IE : R/W; bitpos: [8]; default: 1;
 *  Configures whether or not to enable input of GMAC_RMII_CLK.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_RMII_CLK_FUN_IE    (BIT(8))
#define CNNT_IO_MUX_GMAC_RMII_CLK_FUN_IE_M  (CNNT_IO_MUX_GMAC_RMII_CLK_FUN_IE_V << CNNT_IO_MUX_GMAC_RMII_CLK_FUN_IE_S)
#define CNNT_IO_MUX_GMAC_RMII_CLK_FUN_IE_V  0x00000001U
#define CNNT_IO_MUX_GMAC_RMII_CLK_FUN_IE_S  8
/** CNNT_IO_MUX_GMAC_RMII_CLK_FUN_DRV : R/W; bitpos: [10:9]; default: 2;
 *  Configures the drive strength of GMAC_RMII_CLK.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_GMAC_RMII_CLK_FUN_DRV    0x00000003U
#define CNNT_IO_MUX_GMAC_RMII_CLK_FUN_DRV_M  (CNNT_IO_MUX_GMAC_RMII_CLK_FUN_DRV_V << CNNT_IO_MUX_GMAC_RMII_CLK_FUN_DRV_S)
#define CNNT_IO_MUX_GMAC_RMII_CLK_FUN_DRV_V  0x00000003U
#define CNNT_IO_MUX_GMAC_RMII_CLK_FUN_DRV_S  9
/** CNNT_IO_MUX_GMAC_RMII_CLK_HYS_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  CNNT_IO_MUX_GMAC_RMII_CLK_HYS_SEL is set to 1.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_RMII_CLK_HYS_EN    (BIT(11))
#define CNNT_IO_MUX_GMAC_RMII_CLK_HYS_EN_M  (CNNT_IO_MUX_GMAC_RMII_CLK_HYS_EN_V << CNNT_IO_MUX_GMAC_RMII_CLK_HYS_EN_S)
#define CNNT_IO_MUX_GMAC_RMII_CLK_HYS_EN_V  0x00000001U
#define CNNT_IO_MUX_GMAC_RMII_CLK_HYS_EN_S  11
/** CNNT_IO_MUX_GMAC_RMII_CLK_HYS_SEL : R/W; bitpos: [12]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for
 *  GMAC_RMII_CLK.
 *  0: Choose the output enable signal of eFuse
 *  1: Choose the output enable signal of CNNT_IO_MUX_GMAC_RMII_CLK_HYS_EN
 */
#define CNNT_IO_MUX_GMAC_RMII_CLK_HYS_SEL    (BIT(12))
#define CNNT_IO_MUX_GMAC_RMII_CLK_HYS_SEL_M  (CNNT_IO_MUX_GMAC_RMII_CLK_HYS_SEL_V << CNNT_IO_MUX_GMAC_RMII_CLK_HYS_SEL_S)
#define CNNT_IO_MUX_GMAC_RMII_CLK_HYS_SEL_V  0x00000001U
#define CNNT_IO_MUX_GMAC_RMII_CLK_HYS_SEL_S  12

/** CNNT_IO_MUX_GMAC_RX_CLK_REG register
 *  GMAC IO MUX configuration register for GMAC_RX_CLK
 */
#define CNNT_IO_MUX_GMAC_RX_CLK_REG (DR_REG_CNNT_IO_MUX_BASE + 0x4)
/** CNNT_IO_MUX_GMAC_RX_CLK_SLP_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enter sleep mode for GMAC_RX_CLK.
 *  0: Not enter
 *  1: Enter
 */
#define CNNT_IO_MUX_GMAC_RX_CLK_SLP_SEL    (BIT(0))
#define CNNT_IO_MUX_GMAC_RX_CLK_SLP_SEL_M  (CNNT_IO_MUX_GMAC_RX_CLK_SLP_SEL_V << CNNT_IO_MUX_GMAC_RX_CLK_SLP_SEL_S)
#define CNNT_IO_MUX_GMAC_RX_CLK_SLP_SEL_V  0x00000001U
#define CNNT_IO_MUX_GMAC_RX_CLK_SLP_SEL_S  0
/** CNNT_IO_MUX_GMAC_RX_CLK_MCU_WPD : R/W; bitpos: [1]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GMAC_RX_CLK in sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_RX_CLK_MCU_WPD    (BIT(1))
#define CNNT_IO_MUX_GMAC_RX_CLK_MCU_WPD_M  (CNNT_IO_MUX_GMAC_RX_CLK_MCU_WPD_V << CNNT_IO_MUX_GMAC_RX_CLK_MCU_WPD_S)
#define CNNT_IO_MUX_GMAC_RX_CLK_MCU_WPD_V  0x00000001U
#define CNNT_IO_MUX_GMAC_RX_CLK_MCU_WPD_S  1
/** CNNT_IO_MUX_GMAC_RX_CLK_MCU_WPU : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GMAC_RX_CLK during sleep
 *  mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_RX_CLK_MCU_WPU    (BIT(2))
#define CNNT_IO_MUX_GMAC_RX_CLK_MCU_WPU_M  (CNNT_IO_MUX_GMAC_RX_CLK_MCU_WPU_V << CNNT_IO_MUX_GMAC_RX_CLK_MCU_WPU_S)
#define CNNT_IO_MUX_GMAC_RX_CLK_MCU_WPU_V  0x00000001U
#define CNNT_IO_MUX_GMAC_RX_CLK_MCU_WPU_S  2
/** CNNT_IO_MUX_GMAC_RX_CLK_MCU_IE : R/W; bitpos: [3]; default: 1;
 *  Configures whether or not to enable the input of GMAC_RX_CLK during sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_RX_CLK_MCU_IE    (BIT(3))
#define CNNT_IO_MUX_GMAC_RX_CLK_MCU_IE_M  (CNNT_IO_MUX_GMAC_RX_CLK_MCU_IE_V << CNNT_IO_MUX_GMAC_RX_CLK_MCU_IE_S)
#define CNNT_IO_MUX_GMAC_RX_CLK_MCU_IE_V  0x00000001U
#define CNNT_IO_MUX_GMAC_RX_CLK_MCU_IE_S  3
/** CNNT_IO_MUX_GMAC_RX_CLK_MCU_DRV : R/W; bitpos: [5:4]; default: 2;
 *  Configures the drive strength of GMAC_RX_CLK during sleep mode.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_GMAC_RX_CLK_MCU_DRV    0x00000003U
#define CNNT_IO_MUX_GMAC_RX_CLK_MCU_DRV_M  (CNNT_IO_MUX_GMAC_RX_CLK_MCU_DRV_V << CNNT_IO_MUX_GMAC_RX_CLK_MCU_DRV_S)
#define CNNT_IO_MUX_GMAC_RX_CLK_MCU_DRV_V  0x00000003U
#define CNNT_IO_MUX_GMAC_RX_CLK_MCU_DRV_S  4
/** CNNT_IO_MUX_GMAC_RX_CLK_FUN_WPD : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GMAC_RX_CLK.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_RX_CLK_FUN_WPD    (BIT(6))
#define CNNT_IO_MUX_GMAC_RX_CLK_FUN_WPD_M  (CNNT_IO_MUX_GMAC_RX_CLK_FUN_WPD_V << CNNT_IO_MUX_GMAC_RX_CLK_FUN_WPD_S)
#define CNNT_IO_MUX_GMAC_RX_CLK_FUN_WPD_V  0x00000001U
#define CNNT_IO_MUX_GMAC_RX_CLK_FUN_WPD_S  6
/** CNNT_IO_MUX_GMAC_RX_CLK_FUN_WPU : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not enable pull-up resistor of GMAC_RX_CLK.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_RX_CLK_FUN_WPU    (BIT(7))
#define CNNT_IO_MUX_GMAC_RX_CLK_FUN_WPU_M  (CNNT_IO_MUX_GMAC_RX_CLK_FUN_WPU_V << CNNT_IO_MUX_GMAC_RX_CLK_FUN_WPU_S)
#define CNNT_IO_MUX_GMAC_RX_CLK_FUN_WPU_V  0x00000001U
#define CNNT_IO_MUX_GMAC_RX_CLK_FUN_WPU_S  7
/** CNNT_IO_MUX_GMAC_RX_CLK_FUN_IE : R/W; bitpos: [8]; default: 1;
 *  Configures whether or not to enable input of GMAC_RX_CLK.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_RX_CLK_FUN_IE    (BIT(8))
#define CNNT_IO_MUX_GMAC_RX_CLK_FUN_IE_M  (CNNT_IO_MUX_GMAC_RX_CLK_FUN_IE_V << CNNT_IO_MUX_GMAC_RX_CLK_FUN_IE_S)
#define CNNT_IO_MUX_GMAC_RX_CLK_FUN_IE_V  0x00000001U
#define CNNT_IO_MUX_GMAC_RX_CLK_FUN_IE_S  8
/** CNNT_IO_MUX_GMAC_RX_CLK_FUN_DRV : R/W; bitpos: [10:9]; default: 2;
 *  Configures the drive strength of GMAC_RX_CLK.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_GMAC_RX_CLK_FUN_DRV    0x00000003U
#define CNNT_IO_MUX_GMAC_RX_CLK_FUN_DRV_M  (CNNT_IO_MUX_GMAC_RX_CLK_FUN_DRV_V << CNNT_IO_MUX_GMAC_RX_CLK_FUN_DRV_S)
#define CNNT_IO_MUX_GMAC_RX_CLK_FUN_DRV_V  0x00000003U
#define CNNT_IO_MUX_GMAC_RX_CLK_FUN_DRV_S  9
/** CNNT_IO_MUX_GMAC_RX_CLK_HYS_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  CNNT_IO_MUX_GMAC_RX_CLK_HYS_SEL is set to 1.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_RX_CLK_HYS_EN    (BIT(11))
#define CNNT_IO_MUX_GMAC_RX_CLK_HYS_EN_M  (CNNT_IO_MUX_GMAC_RX_CLK_HYS_EN_V << CNNT_IO_MUX_GMAC_RX_CLK_HYS_EN_S)
#define CNNT_IO_MUX_GMAC_RX_CLK_HYS_EN_V  0x00000001U
#define CNNT_IO_MUX_GMAC_RX_CLK_HYS_EN_S  11
/** CNNT_IO_MUX_GMAC_RX_CLK_HYS_SEL : R/W; bitpos: [12]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for
 *  GMAC_RX_CLK.
 *  0: Choose the output enable signal of eFuse
 *  1: Choose the output enable signal of CNNT_IO_MUX_GMAC_RX_CLK_HYS_EN
 */
#define CNNT_IO_MUX_GMAC_RX_CLK_HYS_SEL    (BIT(12))
#define CNNT_IO_MUX_GMAC_RX_CLK_HYS_SEL_M  (CNNT_IO_MUX_GMAC_RX_CLK_HYS_SEL_V << CNNT_IO_MUX_GMAC_RX_CLK_HYS_SEL_S)
#define CNNT_IO_MUX_GMAC_RX_CLK_HYS_SEL_V  0x00000001U
#define CNNT_IO_MUX_GMAC_RX_CLK_HYS_SEL_S  12

/** CNNT_IO_MUX_GMAC_PHY_RXDV_REG register
 *  GMAC IO MUX configuration register for GMAC_PHY_RXDV
 */
#define CNNT_IO_MUX_GMAC_PHY_RXDV_REG (DR_REG_CNNT_IO_MUX_BASE + 0x8)
/** CNNT_IO_MUX_GMAC_PHY_RXDV_SLP_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enter sleep mode for GMAC_PHY_RXDV.
 *  0: Not enter
 *  1: Enter
 */
#define CNNT_IO_MUX_GMAC_PHY_RXDV_SLP_SEL    (BIT(0))
#define CNNT_IO_MUX_GMAC_PHY_RXDV_SLP_SEL_M  (CNNT_IO_MUX_GMAC_PHY_RXDV_SLP_SEL_V << CNNT_IO_MUX_GMAC_PHY_RXDV_SLP_SEL_S)
#define CNNT_IO_MUX_GMAC_PHY_RXDV_SLP_SEL_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXDV_SLP_SEL_S  0
/** CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_WPD : R/W; bitpos: [1]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GMAC_PHY_RXDV in sleep
 *  mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_WPD    (BIT(1))
#define CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_WPD_M  (CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_WPD_V << CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_WPD_S)
#define CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_WPD_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_WPD_S  1
/** CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_WPU : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GMAC_PHY_RXDV during sleep
 *  mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_WPU    (BIT(2))
#define CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_WPU_M  (CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_WPU_V << CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_WPU_S)
#define CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_WPU_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_WPU_S  2
/** CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_IE : R/W; bitpos: [3]; default: 1;
 *  Configures whether or not to enable the input of GMAC_PHY_RXDV during sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_IE    (BIT(3))
#define CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_IE_M  (CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_IE_V << CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_IE_S)
#define CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_IE_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_IE_S  3
/** CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_DRV : R/W; bitpos: [5:4]; default: 2;
 *  Configures the drive strength of GMAC_PHY_RXDV during sleep mode.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_DRV    0x00000003U
#define CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_DRV_M  (CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_DRV_V << CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_DRV_S)
#define CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_DRV_V  0x00000003U
#define CNNT_IO_MUX_GMAC_PHY_RXDV_MCU_DRV_S  4
/** CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_WPD : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GMAC_PHY_RXDV.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_WPD    (BIT(6))
#define CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_WPD_M  (CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_WPD_V << CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_WPD_S)
#define CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_WPD_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_WPD_S  6
/** CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_WPU : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not enable pull-up resistor of GMAC_PHY_RXDV.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_WPU    (BIT(7))
#define CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_WPU_M  (CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_WPU_V << CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_WPU_S)
#define CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_WPU_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_WPU_S  7
/** CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_IE : R/W; bitpos: [8]; default: 1;
 *  Configures whether or not to enable input of GMAC_PHY_RXDV.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_IE    (BIT(8))
#define CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_IE_M  (CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_IE_V << CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_IE_S)
#define CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_IE_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_IE_S  8
/** CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_DRV : R/W; bitpos: [10:9]; default: 2;
 *  Configures the drive strength of GMAC_PHY_RXDV.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_DRV    0x00000003U
#define CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_DRV_M  (CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_DRV_V << CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_DRV_S)
#define CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_DRV_V  0x00000003U
#define CNNT_IO_MUX_GMAC_PHY_RXDV_FUN_DRV_S  9
/** CNNT_IO_MUX_GMAC_PHY_RXDV_HYS_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  CNNT_IO_MUX_GMAC_PHY_RXDV_HYS_SEL is set to 1.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXDV_HYS_EN    (BIT(11))
#define CNNT_IO_MUX_GMAC_PHY_RXDV_HYS_EN_M  (CNNT_IO_MUX_GMAC_PHY_RXDV_HYS_EN_V << CNNT_IO_MUX_GMAC_PHY_RXDV_HYS_EN_S)
#define CNNT_IO_MUX_GMAC_PHY_RXDV_HYS_EN_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXDV_HYS_EN_S  11
/** CNNT_IO_MUX_GMAC_PHY_RXDV_HYS_SEL : R/W; bitpos: [12]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for
 *  GMAC_PHY_RXDV.
 *  0: Choose the output enable signal of eFuse
 *  1: Choose the output enable signal of CNNT_IO_MUX_GMAC_PHY_RXDV_HYS_EN
 */
#define CNNT_IO_MUX_GMAC_PHY_RXDV_HYS_SEL    (BIT(12))
#define CNNT_IO_MUX_GMAC_PHY_RXDV_HYS_SEL_M  (CNNT_IO_MUX_GMAC_PHY_RXDV_HYS_SEL_V << CNNT_IO_MUX_GMAC_PHY_RXDV_HYS_SEL_S)
#define CNNT_IO_MUX_GMAC_PHY_RXDV_HYS_SEL_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXDV_HYS_SEL_S  12

/** CNNT_IO_MUX_GMAC_PHY_RXD3_REG register
 *  GMAC IO MUX configuration register for GMAC_PHY_RXD3
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD3_REG (DR_REG_CNNT_IO_MUX_BASE + 0xc)
/** CNNT_IO_MUX_GMAC_PHY_RXD3_SLP_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enter sleep mode for GMAC_PHY_RXD3.
 *  0: Not enter
 *  1: Enter
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD3_SLP_SEL    (BIT(0))
#define CNNT_IO_MUX_GMAC_PHY_RXD3_SLP_SEL_M  (CNNT_IO_MUX_GMAC_PHY_RXD3_SLP_SEL_V << CNNT_IO_MUX_GMAC_PHY_RXD3_SLP_SEL_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD3_SLP_SEL_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD3_SLP_SEL_S  0
/** CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_WPD : R/W; bitpos: [1]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GMAC_PHY_RXD3 in sleep
 *  mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_WPD    (BIT(1))
#define CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_WPD_M  (CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_WPD_V << CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_WPD_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_WPD_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_WPD_S  1
/** CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_WPU : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GMAC_PHY_RXD3 during sleep
 *  mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_WPU    (BIT(2))
#define CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_WPU_M  (CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_WPU_V << CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_WPU_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_WPU_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_WPU_S  2
/** CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_IE : R/W; bitpos: [3]; default: 1;
 *  Configures whether or not to enable the input of GMAC_PHY_RXD3 during sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_IE    (BIT(3))
#define CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_IE_M  (CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_IE_V << CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_IE_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_IE_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_IE_S  3
/** CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_DRV : R/W; bitpos: [5:4]; default: 2;
 *  Configures the drive strength of GMAC_PHY_RXD3 during sleep mode.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_DRV    0x00000003U
#define CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_DRV_M  (CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_DRV_V << CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_DRV_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_DRV_V  0x00000003U
#define CNNT_IO_MUX_GMAC_PHY_RXD3_MCU_DRV_S  4
/** CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_WPD : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GMAC_PHY_RXD3.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_WPD    (BIT(6))
#define CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_WPD_M  (CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_WPD_V << CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_WPD_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_WPD_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_WPD_S  6
/** CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_WPU : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not enable pull-up resistor of GMAC_PHY_RXD3.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_WPU    (BIT(7))
#define CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_WPU_M  (CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_WPU_V << CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_WPU_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_WPU_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_WPU_S  7
/** CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_IE : R/W; bitpos: [8]; default: 1;
 *  Configures whether or not to enable input of GMAC_PHY_RXD3.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_IE    (BIT(8))
#define CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_IE_M  (CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_IE_V << CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_IE_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_IE_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_IE_S  8
/** CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_DRV : R/W; bitpos: [10:9]; default: 2;
 *  Configures the drive strength of GMAC_PHY_RXD3.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_DRV    0x00000003U
#define CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_DRV_M  (CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_DRV_V << CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_DRV_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_DRV_V  0x00000003U
#define CNNT_IO_MUX_GMAC_PHY_RXD3_FUN_DRV_S  9
/** CNNT_IO_MUX_GMAC_PHY_RXD3_HYS_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  CNNT_IO_MUX_GMAC_PHY_RXD3_HYS_SEL is set to 1.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD3_HYS_EN    (BIT(11))
#define CNNT_IO_MUX_GMAC_PHY_RXD3_HYS_EN_M  (CNNT_IO_MUX_GMAC_PHY_RXD3_HYS_EN_V << CNNT_IO_MUX_GMAC_PHY_RXD3_HYS_EN_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD3_HYS_EN_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD3_HYS_EN_S  11
/** CNNT_IO_MUX_GMAC_PHY_RXD3_HYS_SEL : R/W; bitpos: [12]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for
 *  GMAC_PHY_RXD3.
 *  0: Choose the output enable signal of eFuse
 *  1: Choose the output enable signal of CNNT_IO_MUX_GMAC_PHY_RXD3_HYS_EN
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD3_HYS_SEL    (BIT(12))
#define CNNT_IO_MUX_GMAC_PHY_RXD3_HYS_SEL_M  (CNNT_IO_MUX_GMAC_PHY_RXD3_HYS_SEL_V << CNNT_IO_MUX_GMAC_PHY_RXD3_HYS_SEL_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD3_HYS_SEL_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD3_HYS_SEL_S  12

/** CNNT_IO_MUX_GMAC_PHY_RXD2_REG register
 *  GMAC IO MUX configuration register for GMAC_PHY_RXD2
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD2_REG (DR_REG_CNNT_IO_MUX_BASE + 0x10)
/** CNNT_IO_MUX_GMAC_PHY_RXD2_SLP_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enter sleep mode for GMAC_PHY_RXD2.
 *  0: Not enter
 *  1: Enter
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD2_SLP_SEL    (BIT(0))
#define CNNT_IO_MUX_GMAC_PHY_RXD2_SLP_SEL_M  (CNNT_IO_MUX_GMAC_PHY_RXD2_SLP_SEL_V << CNNT_IO_MUX_GMAC_PHY_RXD2_SLP_SEL_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD2_SLP_SEL_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD2_SLP_SEL_S  0
/** CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_WPD : R/W; bitpos: [1]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GMAC_PHY_RXD2 in sleep
 *  mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_WPD    (BIT(1))
#define CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_WPD_M  (CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_WPD_V << CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_WPD_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_WPD_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_WPD_S  1
/** CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_WPU : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GMAC_PHY_RXD2 during sleep
 *  mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_WPU    (BIT(2))
#define CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_WPU_M  (CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_WPU_V << CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_WPU_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_WPU_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_WPU_S  2
/** CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_IE : R/W; bitpos: [3]; default: 1;
 *  Configures whether or not to enable the input of GMAC_PHY_RXD2 during sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_IE    (BIT(3))
#define CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_IE_M  (CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_IE_V << CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_IE_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_IE_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_IE_S  3
/** CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_DRV : R/W; bitpos: [5:4]; default: 2;
 *  Configures the drive strength of GMAC_PHY_RXD2 during sleep mode.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_DRV    0x00000003U
#define CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_DRV_M  (CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_DRV_V << CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_DRV_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_DRV_V  0x00000003U
#define CNNT_IO_MUX_GMAC_PHY_RXD2_MCU_DRV_S  4
/** CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_WPD : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GMAC_PHY_RXD2.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_WPD    (BIT(6))
#define CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_WPD_M  (CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_WPD_V << CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_WPD_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_WPD_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_WPD_S  6
/** CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_WPU : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not enable pull-up resistor of GMAC_PHY_RXD2.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_WPU    (BIT(7))
#define CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_WPU_M  (CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_WPU_V << CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_WPU_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_WPU_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_WPU_S  7
/** CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_IE : R/W; bitpos: [8]; default: 1;
 *  Configures whether or not to enable input of GMAC_PHY_RXD2.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_IE    (BIT(8))
#define CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_IE_M  (CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_IE_V << CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_IE_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_IE_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_IE_S  8
/** CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_DRV : R/W; bitpos: [10:9]; default: 2;
 *  Configures the drive strength of GMAC_PHY_RXD2.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_DRV    0x00000003U
#define CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_DRV_M  (CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_DRV_V << CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_DRV_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_DRV_V  0x00000003U
#define CNNT_IO_MUX_GMAC_PHY_RXD2_FUN_DRV_S  9
/** CNNT_IO_MUX_GMAC_PHY_RXD2_HYS_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  CNNT_IO_MUX_GMAC_PHY_RXD2_HYS_SEL is set to 1.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD2_HYS_EN    (BIT(11))
#define CNNT_IO_MUX_GMAC_PHY_RXD2_HYS_EN_M  (CNNT_IO_MUX_GMAC_PHY_RXD2_HYS_EN_V << CNNT_IO_MUX_GMAC_PHY_RXD2_HYS_EN_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD2_HYS_EN_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD2_HYS_EN_S  11
/** CNNT_IO_MUX_GMAC_PHY_RXD2_HYS_SEL : R/W; bitpos: [12]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for
 *  GMAC_PHY_RXD2.
 *  0: Choose the output enable signal of eFuse
 *  1: Choose the output enable signal of CNNT_IO_MUX_GMAC_PHY_RXD2_HYS_EN
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD2_HYS_SEL    (BIT(12))
#define CNNT_IO_MUX_GMAC_PHY_RXD2_HYS_SEL_M  (CNNT_IO_MUX_GMAC_PHY_RXD2_HYS_SEL_V << CNNT_IO_MUX_GMAC_PHY_RXD2_HYS_SEL_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD2_HYS_SEL_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD2_HYS_SEL_S  12

/** CNNT_IO_MUX_GMAC_PHY_RXD1_REG register
 *  GMAC IO MUX configuration register for GMAC_PHY_RXD1
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD1_REG (DR_REG_CNNT_IO_MUX_BASE + 0x14)
/** CNNT_IO_MUX_GMAC_PHY_RXD1_SLP_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enter sleep mode for GMAC_PHY_RXD1.
 *  0: Not enter
 *  1: Enter
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD1_SLP_SEL    (BIT(0))
#define CNNT_IO_MUX_GMAC_PHY_RXD1_SLP_SEL_M  (CNNT_IO_MUX_GMAC_PHY_RXD1_SLP_SEL_V << CNNT_IO_MUX_GMAC_PHY_RXD1_SLP_SEL_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD1_SLP_SEL_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD1_SLP_SEL_S  0
/** CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_WPD : R/W; bitpos: [1]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GMAC_PHY_RXD1 in sleep
 *  mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_WPD    (BIT(1))
#define CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_WPD_M  (CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_WPD_V << CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_WPD_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_WPD_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_WPD_S  1
/** CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_WPU : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GMAC_PHY_RXD1 during sleep
 *  mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_WPU    (BIT(2))
#define CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_WPU_M  (CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_WPU_V << CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_WPU_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_WPU_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_WPU_S  2
/** CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_IE : R/W; bitpos: [3]; default: 1;
 *  Configures whether or not to enable the input of GMAC_PHY_RXD1 during sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_IE    (BIT(3))
#define CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_IE_M  (CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_IE_V << CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_IE_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_IE_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_IE_S  3
/** CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_DRV : R/W; bitpos: [5:4]; default: 2;
 *  Configures the drive strength of GMAC_PHY_RXD1 during sleep mode.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_DRV    0x00000003U
#define CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_DRV_M  (CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_DRV_V << CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_DRV_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_DRV_V  0x00000003U
#define CNNT_IO_MUX_GMAC_PHY_RXD1_MCU_DRV_S  4
/** CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_WPD : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GMAC_PHY_RXD1.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_WPD    (BIT(6))
#define CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_WPD_M  (CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_WPD_V << CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_WPD_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_WPD_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_WPD_S  6
/** CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_WPU : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not enable pull-up resistor of GMAC_PHY_RXD1.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_WPU    (BIT(7))
#define CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_WPU_M  (CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_WPU_V << CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_WPU_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_WPU_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_WPU_S  7
/** CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_IE : R/W; bitpos: [8]; default: 1;
 *  Configures whether or not to enable input of GMAC_PHY_RXD1.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_IE    (BIT(8))
#define CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_IE_M  (CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_IE_V << CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_IE_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_IE_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_IE_S  8
/** CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_DRV : R/W; bitpos: [10:9]; default: 2;
 *  Configures the drive strength of GMAC_PHY_RXD1.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_DRV    0x00000003U
#define CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_DRV_M  (CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_DRV_V << CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_DRV_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_DRV_V  0x00000003U
#define CNNT_IO_MUX_GMAC_PHY_RXD1_FUN_DRV_S  9
/** CNNT_IO_MUX_GMAC_PHY_RXD1_HYS_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  CNNT_IO_MUX_GMAC_PHY_RXD1_HYS_SEL is set to 1.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD1_HYS_EN    (BIT(11))
#define CNNT_IO_MUX_GMAC_PHY_RXD1_HYS_EN_M  (CNNT_IO_MUX_GMAC_PHY_RXD1_HYS_EN_V << CNNT_IO_MUX_GMAC_PHY_RXD1_HYS_EN_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD1_HYS_EN_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD1_HYS_EN_S  11
/** CNNT_IO_MUX_GMAC_PHY_RXD1_HYS_SEL : R/W; bitpos: [12]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for
 *  GMAC_PHY_RXD1.
 *  0: Choose the output enable signal of eFuse
 *  1: Choose the output enable signal of CNNT_IO_MUX_GMAC_PHY_RXD1_HYS_EN
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD1_HYS_SEL    (BIT(12))
#define CNNT_IO_MUX_GMAC_PHY_RXD1_HYS_SEL_M  (CNNT_IO_MUX_GMAC_PHY_RXD1_HYS_SEL_V << CNNT_IO_MUX_GMAC_PHY_RXD1_HYS_SEL_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD1_HYS_SEL_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD1_HYS_SEL_S  12

/** CNNT_IO_MUX_GMAC_PHY_RXD0_REG register
 *  GMAC IO MUX configuration register for GMAC_PHY_RXD0
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD0_REG (DR_REG_CNNT_IO_MUX_BASE + 0x18)
/** CNNT_IO_MUX_GMAC_PHY_RXD0_SLP_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enter sleep mode for GMAC_PHY_RXD0.
 *  0: Not enter
 *  1: Enter
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD0_SLP_SEL    (BIT(0))
#define CNNT_IO_MUX_GMAC_PHY_RXD0_SLP_SEL_M  (CNNT_IO_MUX_GMAC_PHY_RXD0_SLP_SEL_V << CNNT_IO_MUX_GMAC_PHY_RXD0_SLP_SEL_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD0_SLP_SEL_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD0_SLP_SEL_S  0
/** CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_WPD : R/W; bitpos: [1]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GMAC_PHY_RXD0 in sleep
 *  mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_WPD    (BIT(1))
#define CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_WPD_M  (CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_WPD_V << CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_WPD_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_WPD_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_WPD_S  1
/** CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_WPU : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GMAC_PHY_RXD0 during sleep
 *  mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_WPU    (BIT(2))
#define CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_WPU_M  (CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_WPU_V << CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_WPU_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_WPU_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_WPU_S  2
/** CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_IE : R/W; bitpos: [3]; default: 1;
 *  Configures whether or not to enable the input of GMAC_PHY_RXD0 during sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_IE    (BIT(3))
#define CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_IE_M  (CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_IE_V << CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_IE_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_IE_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_IE_S  3
/** CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_DRV : R/W; bitpos: [5:4]; default: 2;
 *  Configures the drive strength of GMAC_PHY_RXD0 during sleep mode.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_DRV    0x00000003U
#define CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_DRV_M  (CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_DRV_V << CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_DRV_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_DRV_V  0x00000003U
#define CNNT_IO_MUX_GMAC_PHY_RXD0_MCU_DRV_S  4
/** CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_WPD : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GMAC_PHY_RXD0.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_WPD    (BIT(6))
#define CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_WPD_M  (CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_WPD_V << CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_WPD_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_WPD_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_WPD_S  6
/** CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_WPU : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not enable pull-up resistor of GMAC_PHY_RXD0.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_WPU    (BIT(7))
#define CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_WPU_M  (CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_WPU_V << CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_WPU_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_WPU_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_WPU_S  7
/** CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_IE : R/W; bitpos: [8]; default: 1;
 *  Configures whether or not to enable input of GMAC_PHY_RXD0.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_IE    (BIT(8))
#define CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_IE_M  (CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_IE_V << CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_IE_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_IE_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_IE_S  8
/** CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_DRV : R/W; bitpos: [10:9]; default: 2;
 *  Configures the drive strength of GMAC_PHY_RXD0.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_DRV    0x00000003U
#define CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_DRV_M  (CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_DRV_V << CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_DRV_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_DRV_V  0x00000003U
#define CNNT_IO_MUX_GMAC_PHY_RXD0_FUN_DRV_S  9
/** CNNT_IO_MUX_GMAC_PHY_RXD0_HYS_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  CNNT_IO_MUX_GMAC_PHY_RXD0_HYS_SEL is set to 1.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD0_HYS_EN    (BIT(11))
#define CNNT_IO_MUX_GMAC_PHY_RXD0_HYS_EN_M  (CNNT_IO_MUX_GMAC_PHY_RXD0_HYS_EN_V << CNNT_IO_MUX_GMAC_PHY_RXD0_HYS_EN_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD0_HYS_EN_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD0_HYS_EN_S  11
/** CNNT_IO_MUX_GMAC_PHY_RXD0_HYS_SEL : R/W; bitpos: [12]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for
 *  GMAC_PHY_RXD0.
 *  0: Choose the output enable signal of eFuse
 *  1: Choose the output enable signal of CNNT_IO_MUX_GMAC_PHY_RXD0_HYS_EN
 */
#define CNNT_IO_MUX_GMAC_PHY_RXD0_HYS_SEL    (BIT(12))
#define CNNT_IO_MUX_GMAC_PHY_RXD0_HYS_SEL_M  (CNNT_IO_MUX_GMAC_PHY_RXD0_HYS_SEL_V << CNNT_IO_MUX_GMAC_PHY_RXD0_HYS_SEL_S)
#define CNNT_IO_MUX_GMAC_PHY_RXD0_HYS_SEL_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PHY_RXD0_HYS_SEL_S  12

/** CNNT_IO_MUX_SDIO_DATA0_REG register
 *  SDIO IO MUX configuration register for SDIO_DATA0
 */
#define CNNT_IO_MUX_SDIO_DATA0_REG (DR_REG_CNNT_IO_MUX_BASE + 0x1c)
/** CNNT_IO_MUX_SDIO_DATA0_SLP_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enter sleep mode for SDIO_DATA0.
 *  0: Not enter
 *  1: Enter
 */
#define CNNT_IO_MUX_SDIO_DATA0_SLP_SEL    (BIT(0))
#define CNNT_IO_MUX_SDIO_DATA0_SLP_SEL_M  (CNNT_IO_MUX_SDIO_DATA0_SLP_SEL_V << CNNT_IO_MUX_SDIO_DATA0_SLP_SEL_S)
#define CNNT_IO_MUX_SDIO_DATA0_SLP_SEL_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA0_SLP_SEL_S  0
/** CNNT_IO_MUX_SDIO_DATA0_MCU_WPD : R/W; bitpos: [1]; default: 0;
 *  Configure whether or not to enable pull-down resistor of SDIO_DATA0 in sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA0_MCU_WPD    (BIT(1))
#define CNNT_IO_MUX_SDIO_DATA0_MCU_WPD_M  (CNNT_IO_MUX_SDIO_DATA0_MCU_WPD_V << CNNT_IO_MUX_SDIO_DATA0_MCU_WPD_S)
#define CNNT_IO_MUX_SDIO_DATA0_MCU_WPD_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA0_MCU_WPD_S  1
/** CNNT_IO_MUX_SDIO_DATA0_MCU_WPU : R/W; bitpos: [2]; default: 1;
 *  Configures whether or not to enable pull-up resistor of SDIO_DATA0 during sleep
 *  mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA0_MCU_WPU    (BIT(2))
#define CNNT_IO_MUX_SDIO_DATA0_MCU_WPU_M  (CNNT_IO_MUX_SDIO_DATA0_MCU_WPU_V << CNNT_IO_MUX_SDIO_DATA0_MCU_WPU_S)
#define CNNT_IO_MUX_SDIO_DATA0_MCU_WPU_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA0_MCU_WPU_S  2
/** CNNT_IO_MUX_SDIO_DATA0_MCU_IE : R/W; bitpos: [3]; default: 1;
 *  Configures whether or not to enable the input of SDIO_DATA0 during sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA0_MCU_IE    (BIT(3))
#define CNNT_IO_MUX_SDIO_DATA0_MCU_IE_M  (CNNT_IO_MUX_SDIO_DATA0_MCU_IE_V << CNNT_IO_MUX_SDIO_DATA0_MCU_IE_S)
#define CNNT_IO_MUX_SDIO_DATA0_MCU_IE_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA0_MCU_IE_S  3
/** CNNT_IO_MUX_SDIO_DATA0_MCU_DRV : R/W; bitpos: [5:4]; default: 2;
 *  Configures the drive strength of SDIO_DATA0 during sleep mode.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_SDIO_DATA0_MCU_DRV    0x00000003U
#define CNNT_IO_MUX_SDIO_DATA0_MCU_DRV_M  (CNNT_IO_MUX_SDIO_DATA0_MCU_DRV_V << CNNT_IO_MUX_SDIO_DATA0_MCU_DRV_S)
#define CNNT_IO_MUX_SDIO_DATA0_MCU_DRV_V  0x00000003U
#define CNNT_IO_MUX_SDIO_DATA0_MCU_DRV_S  4
/** CNNT_IO_MUX_SDIO_DATA0_FUN_WPD : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to enable pull-down resistor of SDIO_DATA0
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA0_FUN_WPD    (BIT(6))
#define CNNT_IO_MUX_SDIO_DATA0_FUN_WPD_M  (CNNT_IO_MUX_SDIO_DATA0_FUN_WPD_V << CNNT_IO_MUX_SDIO_DATA0_FUN_WPD_S)
#define CNNT_IO_MUX_SDIO_DATA0_FUN_WPD_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA0_FUN_WPD_S  6
/** CNNT_IO_MUX_SDIO_DATA0_FUN_WPU : R/W; bitpos: [7]; default: 1;
 *  Configures whether or not enable pull-up resistor of SDIO_DATA0.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA0_FUN_WPU    (BIT(7))
#define CNNT_IO_MUX_SDIO_DATA0_FUN_WPU_M  (CNNT_IO_MUX_SDIO_DATA0_FUN_WPU_V << CNNT_IO_MUX_SDIO_DATA0_FUN_WPU_S)
#define CNNT_IO_MUX_SDIO_DATA0_FUN_WPU_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA0_FUN_WPU_S  7
/** CNNT_IO_MUX_SDIO_DATA0_FUN_IE : R/W; bitpos: [8]; default: 1;
 *  Configures whether or not to enable input of SDIO_DATA0.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA0_FUN_IE    (BIT(8))
#define CNNT_IO_MUX_SDIO_DATA0_FUN_IE_M  (CNNT_IO_MUX_SDIO_DATA0_FUN_IE_V << CNNT_IO_MUX_SDIO_DATA0_FUN_IE_S)
#define CNNT_IO_MUX_SDIO_DATA0_FUN_IE_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA0_FUN_IE_S  8
/** CNNT_IO_MUX_SDIO_DATA0_FUN_DRV : R/W; bitpos: [10:9]; default: 2;
 *  Configures the drive strength of SDIO_DATA0.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_SDIO_DATA0_FUN_DRV    0x00000003U
#define CNNT_IO_MUX_SDIO_DATA0_FUN_DRV_M  (CNNT_IO_MUX_SDIO_DATA0_FUN_DRV_V << CNNT_IO_MUX_SDIO_DATA0_FUN_DRV_S)
#define CNNT_IO_MUX_SDIO_DATA0_FUN_DRV_V  0x00000003U
#define CNNT_IO_MUX_SDIO_DATA0_FUN_DRV_S  9
/** CNNT_IO_MUX_SDIO_DATA0_HYS_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  CNNT_IO_MUX_SDIO_DATA0_HYS_SEL is set to 1.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA0_HYS_EN    (BIT(11))
#define CNNT_IO_MUX_SDIO_DATA0_HYS_EN_M  (CNNT_IO_MUX_SDIO_DATA0_HYS_EN_V << CNNT_IO_MUX_SDIO_DATA0_HYS_EN_S)
#define CNNT_IO_MUX_SDIO_DATA0_HYS_EN_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA0_HYS_EN_S  11
/** CNNT_IO_MUX_SDIO_DATA0_HYS_SEL : R/W; bitpos: [12]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for
 *  SDIO_DATA0.
 *  0: Choose the output enable signal of eFuse
 *  1: Choose the output enable signal of CNNT_IO_MUX_SDIO_DATA0_HYS_EN
 */
#define CNNT_IO_MUX_SDIO_DATA0_HYS_SEL    (BIT(12))
#define CNNT_IO_MUX_SDIO_DATA0_HYS_SEL_M  (CNNT_IO_MUX_SDIO_DATA0_HYS_SEL_V << CNNT_IO_MUX_SDIO_DATA0_HYS_SEL_S)
#define CNNT_IO_MUX_SDIO_DATA0_HYS_SEL_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA0_HYS_SEL_S  12

/** CNNT_IO_MUX_SDIO_DATA1_REG register
 *  SDIO IO MUX configuration register for SDIO_DATA1
 */
#define CNNT_IO_MUX_SDIO_DATA1_REG (DR_REG_CNNT_IO_MUX_BASE + 0x20)
/** CNNT_IO_MUX_SDIO_DATA1_SLP_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enter sleep mode for SDIO_DATA1.
 *  0: Not enter
 *  1: Enter
 */
#define CNNT_IO_MUX_SDIO_DATA1_SLP_SEL    (BIT(0))
#define CNNT_IO_MUX_SDIO_DATA1_SLP_SEL_M  (CNNT_IO_MUX_SDIO_DATA1_SLP_SEL_V << CNNT_IO_MUX_SDIO_DATA1_SLP_SEL_S)
#define CNNT_IO_MUX_SDIO_DATA1_SLP_SEL_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA1_SLP_SEL_S  0
/** CNNT_IO_MUX_SDIO_DATA1_MCU_WPD : R/W; bitpos: [1]; default: 0;
 *  Configure whether or not to enable pull-down resistor of SDIO_DATA1 in sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA1_MCU_WPD    (BIT(1))
#define CNNT_IO_MUX_SDIO_DATA1_MCU_WPD_M  (CNNT_IO_MUX_SDIO_DATA1_MCU_WPD_V << CNNT_IO_MUX_SDIO_DATA1_MCU_WPD_S)
#define CNNT_IO_MUX_SDIO_DATA1_MCU_WPD_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA1_MCU_WPD_S  1
/** CNNT_IO_MUX_SDIO_DATA1_MCU_WPU : R/W; bitpos: [2]; default: 1;
 *  Configures whether or not to enable pull-up resistor of SDIO_DATA1 during sleep
 *  mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA1_MCU_WPU    (BIT(2))
#define CNNT_IO_MUX_SDIO_DATA1_MCU_WPU_M  (CNNT_IO_MUX_SDIO_DATA1_MCU_WPU_V << CNNT_IO_MUX_SDIO_DATA1_MCU_WPU_S)
#define CNNT_IO_MUX_SDIO_DATA1_MCU_WPU_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA1_MCU_WPU_S  2
/** CNNT_IO_MUX_SDIO_DATA1_MCU_IE : R/W; bitpos: [3]; default: 1;
 *  Configures whether or not to enable the input of SDIO_DATA1 during sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA1_MCU_IE    (BIT(3))
#define CNNT_IO_MUX_SDIO_DATA1_MCU_IE_M  (CNNT_IO_MUX_SDIO_DATA1_MCU_IE_V << CNNT_IO_MUX_SDIO_DATA1_MCU_IE_S)
#define CNNT_IO_MUX_SDIO_DATA1_MCU_IE_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA1_MCU_IE_S  3
/** CNNT_IO_MUX_SDIO_DATA1_MCU_DRV : R/W; bitpos: [5:4]; default: 2;
 *  Configures the drive strength of SDIO_DATA1 during sleep mode.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_SDIO_DATA1_MCU_DRV    0x00000003U
#define CNNT_IO_MUX_SDIO_DATA1_MCU_DRV_M  (CNNT_IO_MUX_SDIO_DATA1_MCU_DRV_V << CNNT_IO_MUX_SDIO_DATA1_MCU_DRV_S)
#define CNNT_IO_MUX_SDIO_DATA1_MCU_DRV_V  0x00000003U
#define CNNT_IO_MUX_SDIO_DATA1_MCU_DRV_S  4
/** CNNT_IO_MUX_SDIO_DATA1_FUN_WPD : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to enable pull-down resistor of SDIO_DATA1.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA1_FUN_WPD    (BIT(6))
#define CNNT_IO_MUX_SDIO_DATA1_FUN_WPD_M  (CNNT_IO_MUX_SDIO_DATA1_FUN_WPD_V << CNNT_IO_MUX_SDIO_DATA1_FUN_WPD_S)
#define CNNT_IO_MUX_SDIO_DATA1_FUN_WPD_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA1_FUN_WPD_S  6
/** CNNT_IO_MUX_SDIO_DATA1_FUN_WPU : R/W; bitpos: [7]; default: 1;
 *  Configures whether or not enable pull-up resistor of SDIO_DATA1.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA1_FUN_WPU    (BIT(7))
#define CNNT_IO_MUX_SDIO_DATA1_FUN_WPU_M  (CNNT_IO_MUX_SDIO_DATA1_FUN_WPU_V << CNNT_IO_MUX_SDIO_DATA1_FUN_WPU_S)
#define CNNT_IO_MUX_SDIO_DATA1_FUN_WPU_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA1_FUN_WPU_S  7
/** CNNT_IO_MUX_SDIO_DATA1_FUN_IE : R/W; bitpos: [8]; default: 1;
 *  Configures whether or not to enable input of SDIO_DATA1.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA1_FUN_IE    (BIT(8))
#define CNNT_IO_MUX_SDIO_DATA1_FUN_IE_M  (CNNT_IO_MUX_SDIO_DATA1_FUN_IE_V << CNNT_IO_MUX_SDIO_DATA1_FUN_IE_S)
#define CNNT_IO_MUX_SDIO_DATA1_FUN_IE_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA1_FUN_IE_S  8
/** CNNT_IO_MUX_SDIO_DATA1_FUN_DRV : R/W; bitpos: [10:9]; default: 2;
 *  Configures the drive strength of SDIO_DATA1.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_SDIO_DATA1_FUN_DRV    0x00000003U
#define CNNT_IO_MUX_SDIO_DATA1_FUN_DRV_M  (CNNT_IO_MUX_SDIO_DATA1_FUN_DRV_V << CNNT_IO_MUX_SDIO_DATA1_FUN_DRV_S)
#define CNNT_IO_MUX_SDIO_DATA1_FUN_DRV_V  0x00000003U
#define CNNT_IO_MUX_SDIO_DATA1_FUN_DRV_S  9
/** CNNT_IO_MUX_SDIO_DATA1_HYS_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  CNNT_IO_MUX_SDIO_DATA1_HYS_SEL is set to 1.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA1_HYS_EN    (BIT(11))
#define CNNT_IO_MUX_SDIO_DATA1_HYS_EN_M  (CNNT_IO_MUX_SDIO_DATA1_HYS_EN_V << CNNT_IO_MUX_SDIO_DATA1_HYS_EN_S)
#define CNNT_IO_MUX_SDIO_DATA1_HYS_EN_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA1_HYS_EN_S  11
/** CNNT_IO_MUX_SDIO_DATA1_HYS_SEL : R/W; bitpos: [12]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for
 *  SDIO_DATA1.
 *  0: Choose the output enable signal of eFuse
 *  1: Choose the output enable signal of CNNT_IO_MUX_SDIO_DATA1_HYS_EN
 */
#define CNNT_IO_MUX_SDIO_DATA1_HYS_SEL    (BIT(12))
#define CNNT_IO_MUX_SDIO_DATA1_HYS_SEL_M  (CNNT_IO_MUX_SDIO_DATA1_HYS_SEL_V << CNNT_IO_MUX_SDIO_DATA1_HYS_SEL_S)
#define CNNT_IO_MUX_SDIO_DATA1_HYS_SEL_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA1_HYS_SEL_S  12

/** CNNT_IO_MUX_SDIO_DATA2_REG register
 *  SDIO IO MUX configuration register for SDIO_DATA2
 */
#define CNNT_IO_MUX_SDIO_DATA2_REG (DR_REG_CNNT_IO_MUX_BASE + 0x24)
/** CNNT_IO_MUX_SDIO_DATA2_SLP_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enter sleep mode for SDIO_DATA2.
 *  0: Not enter
 *  1: Enter
 */
#define CNNT_IO_MUX_SDIO_DATA2_SLP_SEL    (BIT(0))
#define CNNT_IO_MUX_SDIO_DATA2_SLP_SEL_M  (CNNT_IO_MUX_SDIO_DATA2_SLP_SEL_V << CNNT_IO_MUX_SDIO_DATA2_SLP_SEL_S)
#define CNNT_IO_MUX_SDIO_DATA2_SLP_SEL_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA2_SLP_SEL_S  0
/** CNNT_IO_MUX_SDIO_DATA2_MCU_WPD : R/W; bitpos: [1]; default: 0;
 *  Configure whether or not to enable pull-down resistor of SDIO_DATA2 in sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA2_MCU_WPD    (BIT(1))
#define CNNT_IO_MUX_SDIO_DATA2_MCU_WPD_M  (CNNT_IO_MUX_SDIO_DATA2_MCU_WPD_V << CNNT_IO_MUX_SDIO_DATA2_MCU_WPD_S)
#define CNNT_IO_MUX_SDIO_DATA2_MCU_WPD_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA2_MCU_WPD_S  1
/** CNNT_IO_MUX_SDIO_DATA2_MCU_WPU : R/W; bitpos: [2]; default: 1;
 *  Configures whether or not to enable pull-up resistor of SDIO_DATA2 during sleep
 *  mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA2_MCU_WPU    (BIT(2))
#define CNNT_IO_MUX_SDIO_DATA2_MCU_WPU_M  (CNNT_IO_MUX_SDIO_DATA2_MCU_WPU_V << CNNT_IO_MUX_SDIO_DATA2_MCU_WPU_S)
#define CNNT_IO_MUX_SDIO_DATA2_MCU_WPU_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA2_MCU_WPU_S  2
/** CNNT_IO_MUX_SDIO_DATA2_MCU_IE : R/W; bitpos: [3]; default: 1;
 *  Configures whether or not to enable the input of SDIO_DATA2 during sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA2_MCU_IE    (BIT(3))
#define CNNT_IO_MUX_SDIO_DATA2_MCU_IE_M  (CNNT_IO_MUX_SDIO_DATA2_MCU_IE_V << CNNT_IO_MUX_SDIO_DATA2_MCU_IE_S)
#define CNNT_IO_MUX_SDIO_DATA2_MCU_IE_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA2_MCU_IE_S  3
/** CNNT_IO_MUX_SDIO_DATA2_MCU_DRV : R/W; bitpos: [5:4]; default: 2;
 *  Configures the drive strength of SDIO_DATA2 during sleep mode.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_SDIO_DATA2_MCU_DRV    0x00000003U
#define CNNT_IO_MUX_SDIO_DATA2_MCU_DRV_M  (CNNT_IO_MUX_SDIO_DATA2_MCU_DRV_V << CNNT_IO_MUX_SDIO_DATA2_MCU_DRV_S)
#define CNNT_IO_MUX_SDIO_DATA2_MCU_DRV_V  0x00000003U
#define CNNT_IO_MUX_SDIO_DATA2_MCU_DRV_S  4
/** CNNT_IO_MUX_SDIO_DATA2_FUN_WPD : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to enable pull-down resistor of SDIO_DATA2.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA2_FUN_WPD    (BIT(6))
#define CNNT_IO_MUX_SDIO_DATA2_FUN_WPD_M  (CNNT_IO_MUX_SDIO_DATA2_FUN_WPD_V << CNNT_IO_MUX_SDIO_DATA2_FUN_WPD_S)
#define CNNT_IO_MUX_SDIO_DATA2_FUN_WPD_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA2_FUN_WPD_S  6
/** CNNT_IO_MUX_SDIO_DATA2_FUN_WPU : R/W; bitpos: [7]; default: 1;
 *  Configures whether or not enable pull-up resistor of SDIO_DATA2.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA2_FUN_WPU    (BIT(7))
#define CNNT_IO_MUX_SDIO_DATA2_FUN_WPU_M  (CNNT_IO_MUX_SDIO_DATA2_FUN_WPU_V << CNNT_IO_MUX_SDIO_DATA2_FUN_WPU_S)
#define CNNT_IO_MUX_SDIO_DATA2_FUN_WPU_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA2_FUN_WPU_S  7
/** CNNT_IO_MUX_SDIO_DATA2_FUN_IE : R/W; bitpos: [8]; default: 1;
 *  Configures whether or not to enable input of SDIO_DATA2.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA2_FUN_IE    (BIT(8))
#define CNNT_IO_MUX_SDIO_DATA2_FUN_IE_M  (CNNT_IO_MUX_SDIO_DATA2_FUN_IE_V << CNNT_IO_MUX_SDIO_DATA2_FUN_IE_S)
#define CNNT_IO_MUX_SDIO_DATA2_FUN_IE_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA2_FUN_IE_S  8
/** CNNT_IO_MUX_SDIO_DATA2_FUN_DRV : R/W; bitpos: [10:9]; default: 2;
 *  Configures the drive strength of SDIO_DATA2.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_SDIO_DATA2_FUN_DRV    0x00000003U
#define CNNT_IO_MUX_SDIO_DATA2_FUN_DRV_M  (CNNT_IO_MUX_SDIO_DATA2_FUN_DRV_V << CNNT_IO_MUX_SDIO_DATA2_FUN_DRV_S)
#define CNNT_IO_MUX_SDIO_DATA2_FUN_DRV_V  0x00000003U
#define CNNT_IO_MUX_SDIO_DATA2_FUN_DRV_S  9
/** CNNT_IO_MUX_SDIO_DATA2_HYS_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  CNNT_IO_MUX_SDIO_DATA2_HYS_SEL is set to 1.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA2_HYS_EN    (BIT(11))
#define CNNT_IO_MUX_SDIO_DATA2_HYS_EN_M  (CNNT_IO_MUX_SDIO_DATA2_HYS_EN_V << CNNT_IO_MUX_SDIO_DATA2_HYS_EN_S)
#define CNNT_IO_MUX_SDIO_DATA2_HYS_EN_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA2_HYS_EN_S  11
/** CNNT_IO_MUX_SDIO_DATA2_HYS_SEL : R/W; bitpos: [12]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for
 *  SDIO_DATA2.
 *  0: Choose the output enable signal of eFuse
 *  1: Choose the output enable signal of CNNT_IO_MUX_SDIO_DATA2_HYS_EN
 */
#define CNNT_IO_MUX_SDIO_DATA2_HYS_SEL    (BIT(12))
#define CNNT_IO_MUX_SDIO_DATA2_HYS_SEL_M  (CNNT_IO_MUX_SDIO_DATA2_HYS_SEL_V << CNNT_IO_MUX_SDIO_DATA2_HYS_SEL_S)
#define CNNT_IO_MUX_SDIO_DATA2_HYS_SEL_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA2_HYS_SEL_S  12

/** CNNT_IO_MUX_SDIO_DATA3_REG register
 *  SDIO IO MUX configuration register for SDIO_DATA3
 */
#define CNNT_IO_MUX_SDIO_DATA3_REG (DR_REG_CNNT_IO_MUX_BASE + 0x28)
/** CNNT_IO_MUX_SDIO_DATA3_SLP_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enter sleep mode for SDIO_DATA3.
 *  0: Not enter
 *  1: Enter
 */
#define CNNT_IO_MUX_SDIO_DATA3_SLP_SEL    (BIT(0))
#define CNNT_IO_MUX_SDIO_DATA3_SLP_SEL_M  (CNNT_IO_MUX_SDIO_DATA3_SLP_SEL_V << CNNT_IO_MUX_SDIO_DATA3_SLP_SEL_S)
#define CNNT_IO_MUX_SDIO_DATA3_SLP_SEL_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA3_SLP_SEL_S  0
/** CNNT_IO_MUX_SDIO_DATA3_MCU_WPD : R/W; bitpos: [1]; default: 0;
 *  Configure whether or not to enable pull-down resistor of SDIO_DATA3 in sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA3_MCU_WPD    (BIT(1))
#define CNNT_IO_MUX_SDIO_DATA3_MCU_WPD_M  (CNNT_IO_MUX_SDIO_DATA3_MCU_WPD_V << CNNT_IO_MUX_SDIO_DATA3_MCU_WPD_S)
#define CNNT_IO_MUX_SDIO_DATA3_MCU_WPD_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA3_MCU_WPD_S  1
/** CNNT_IO_MUX_SDIO_DATA3_MCU_WPU : R/W; bitpos: [2]; default: 1;
 *  Configures whether or not to enable pull-up resistor of SDIO_DATA3 during sleep
 *  mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA3_MCU_WPU    (BIT(2))
#define CNNT_IO_MUX_SDIO_DATA3_MCU_WPU_M  (CNNT_IO_MUX_SDIO_DATA3_MCU_WPU_V << CNNT_IO_MUX_SDIO_DATA3_MCU_WPU_S)
#define CNNT_IO_MUX_SDIO_DATA3_MCU_WPU_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA3_MCU_WPU_S  2
/** CNNT_IO_MUX_SDIO_DATA3_MCU_IE : R/W; bitpos: [3]; default: 1;
 *  Configures whether or not to enable the input of SDIO_DATA3 during sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA3_MCU_IE    (BIT(3))
#define CNNT_IO_MUX_SDIO_DATA3_MCU_IE_M  (CNNT_IO_MUX_SDIO_DATA3_MCU_IE_V << CNNT_IO_MUX_SDIO_DATA3_MCU_IE_S)
#define CNNT_IO_MUX_SDIO_DATA3_MCU_IE_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA3_MCU_IE_S  3
/** CNNT_IO_MUX_SDIO_DATA3_MCU_DRV : R/W; bitpos: [5:4]; default: 2;
 *  Configures the drive strength of SDIO_DATA3 during sleep mode.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_SDIO_DATA3_MCU_DRV    0x00000003U
#define CNNT_IO_MUX_SDIO_DATA3_MCU_DRV_M  (CNNT_IO_MUX_SDIO_DATA3_MCU_DRV_V << CNNT_IO_MUX_SDIO_DATA3_MCU_DRV_S)
#define CNNT_IO_MUX_SDIO_DATA3_MCU_DRV_V  0x00000003U
#define CNNT_IO_MUX_SDIO_DATA3_MCU_DRV_S  4
/** CNNT_IO_MUX_SDIO_DATA3_FUN_WPD : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to enable pull-down resistor of SDIO_DATA3.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA3_FUN_WPD    (BIT(6))
#define CNNT_IO_MUX_SDIO_DATA3_FUN_WPD_M  (CNNT_IO_MUX_SDIO_DATA3_FUN_WPD_V << CNNT_IO_MUX_SDIO_DATA3_FUN_WPD_S)
#define CNNT_IO_MUX_SDIO_DATA3_FUN_WPD_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA3_FUN_WPD_S  6
/** CNNT_IO_MUX_SDIO_DATA3_FUN_WPU : R/W; bitpos: [7]; default: 1;
 *  Configures whether or not enable pull-up resistor of SDIO_DATA3.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA3_FUN_WPU    (BIT(7))
#define CNNT_IO_MUX_SDIO_DATA3_FUN_WPU_M  (CNNT_IO_MUX_SDIO_DATA3_FUN_WPU_V << CNNT_IO_MUX_SDIO_DATA3_FUN_WPU_S)
#define CNNT_IO_MUX_SDIO_DATA3_FUN_WPU_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA3_FUN_WPU_S  7
/** CNNT_IO_MUX_SDIO_DATA3_FUN_IE : R/W; bitpos: [8]; default: 1;
 *  Configures whether or not to enable input of SDIO_DATA3.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA3_FUN_IE    (BIT(8))
#define CNNT_IO_MUX_SDIO_DATA3_FUN_IE_M  (CNNT_IO_MUX_SDIO_DATA3_FUN_IE_V << CNNT_IO_MUX_SDIO_DATA3_FUN_IE_S)
#define CNNT_IO_MUX_SDIO_DATA3_FUN_IE_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA3_FUN_IE_S  8
/** CNNT_IO_MUX_SDIO_DATA3_FUN_DRV : R/W; bitpos: [10:9]; default: 2;
 *  Configures the drive strength of SDIO_DATA3.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_SDIO_DATA3_FUN_DRV    0x00000003U
#define CNNT_IO_MUX_SDIO_DATA3_FUN_DRV_M  (CNNT_IO_MUX_SDIO_DATA3_FUN_DRV_V << CNNT_IO_MUX_SDIO_DATA3_FUN_DRV_S)
#define CNNT_IO_MUX_SDIO_DATA3_FUN_DRV_V  0x00000003U
#define CNNT_IO_MUX_SDIO_DATA3_FUN_DRV_S  9
/** CNNT_IO_MUX_SDIO_DATA3_HYS_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  CNNT_IO_MUX_SDIO_DATA3_HYS_SEL is set to 1.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_DATA3_HYS_EN    (BIT(11))
#define CNNT_IO_MUX_SDIO_DATA3_HYS_EN_M  (CNNT_IO_MUX_SDIO_DATA3_HYS_EN_V << CNNT_IO_MUX_SDIO_DATA3_HYS_EN_S)
#define CNNT_IO_MUX_SDIO_DATA3_HYS_EN_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA3_HYS_EN_S  11
/** CNNT_IO_MUX_SDIO_DATA3_HYS_SEL : R/W; bitpos: [12]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for
 *  SDIO_DATA3.
 *  0: Choose the output enable signal of eFuse
 *  1: Choose the output enable signal of CNNT_IO_MUX_SDIO_DATA3_HYS_EN
 */
#define CNNT_IO_MUX_SDIO_DATA3_HYS_SEL    (BIT(12))
#define CNNT_IO_MUX_SDIO_DATA3_HYS_SEL_M  (CNNT_IO_MUX_SDIO_DATA3_HYS_SEL_V << CNNT_IO_MUX_SDIO_DATA3_HYS_SEL_S)
#define CNNT_IO_MUX_SDIO_DATA3_HYS_SEL_V  0x00000001U
#define CNNT_IO_MUX_SDIO_DATA3_HYS_SEL_S  12

/** CNNT_IO_MUX_SDIO_CLK_REG register
 *  SDIO IO MUX configuration register for SDIO_CLK
 */
#define CNNT_IO_MUX_SDIO_CLK_REG (DR_REG_CNNT_IO_MUX_BASE + 0x2c)
/** CNNT_IO_MUX_SDIO_CLK_SLP_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enter sleep mode for SDIO_CLK.
 *  0: Not enter
 *  1: Enter
 */
#define CNNT_IO_MUX_SDIO_CLK_SLP_SEL    (BIT(0))
#define CNNT_IO_MUX_SDIO_CLK_SLP_SEL_M  (CNNT_IO_MUX_SDIO_CLK_SLP_SEL_V << CNNT_IO_MUX_SDIO_CLK_SLP_SEL_S)
#define CNNT_IO_MUX_SDIO_CLK_SLP_SEL_V  0x00000001U
#define CNNT_IO_MUX_SDIO_CLK_SLP_SEL_S  0
/** CNNT_IO_MUX_SDIO_CLK_MCU_WPD : R/W; bitpos: [1]; default: 0;
 *  Configure whether or not to enable pull-down resistor of SDIO_CLK in sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_CLK_MCU_WPD    (BIT(1))
#define CNNT_IO_MUX_SDIO_CLK_MCU_WPD_M  (CNNT_IO_MUX_SDIO_CLK_MCU_WPD_V << CNNT_IO_MUX_SDIO_CLK_MCU_WPD_S)
#define CNNT_IO_MUX_SDIO_CLK_MCU_WPD_V  0x00000001U
#define CNNT_IO_MUX_SDIO_CLK_MCU_WPD_S  1
/** CNNT_IO_MUX_SDIO_CLK_MCU_WPU : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable pull-up resistor of SDIO_CLK during sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_CLK_MCU_WPU    (BIT(2))
#define CNNT_IO_MUX_SDIO_CLK_MCU_WPU_M  (CNNT_IO_MUX_SDIO_CLK_MCU_WPU_V << CNNT_IO_MUX_SDIO_CLK_MCU_WPU_S)
#define CNNT_IO_MUX_SDIO_CLK_MCU_WPU_V  0x00000001U
#define CNNT_IO_MUX_SDIO_CLK_MCU_WPU_S  2
/** CNNT_IO_MUX_SDIO_CLK_MCU_IE : R/W; bitpos: [3]; default: 1;
 *  Configures whether or not to enable the input of SDIO_CLK during sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_CLK_MCU_IE    (BIT(3))
#define CNNT_IO_MUX_SDIO_CLK_MCU_IE_M  (CNNT_IO_MUX_SDIO_CLK_MCU_IE_V << CNNT_IO_MUX_SDIO_CLK_MCU_IE_S)
#define CNNT_IO_MUX_SDIO_CLK_MCU_IE_V  0x00000001U
#define CNNT_IO_MUX_SDIO_CLK_MCU_IE_S  3
/** CNNT_IO_MUX_SDIO_CLK_MCU_DRV : R/W; bitpos: [5:4]; default: 2;
 *  Configures the drive strength of SDIO_CLK during sleep mode.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_SDIO_CLK_MCU_DRV    0x00000003U
#define CNNT_IO_MUX_SDIO_CLK_MCU_DRV_M  (CNNT_IO_MUX_SDIO_CLK_MCU_DRV_V << CNNT_IO_MUX_SDIO_CLK_MCU_DRV_S)
#define CNNT_IO_MUX_SDIO_CLK_MCU_DRV_V  0x00000003U
#define CNNT_IO_MUX_SDIO_CLK_MCU_DRV_S  4
/** CNNT_IO_MUX_SDIO_CLK_FUN_WPD : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to enable pull-down resistor of SDIO_CLK.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_CLK_FUN_WPD    (BIT(6))
#define CNNT_IO_MUX_SDIO_CLK_FUN_WPD_M  (CNNT_IO_MUX_SDIO_CLK_FUN_WPD_V << CNNT_IO_MUX_SDIO_CLK_FUN_WPD_S)
#define CNNT_IO_MUX_SDIO_CLK_FUN_WPD_V  0x00000001U
#define CNNT_IO_MUX_SDIO_CLK_FUN_WPD_S  6
/** CNNT_IO_MUX_SDIO_CLK_FUN_WPU : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not enable pull-up resistor of SDIO_CLK.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_CLK_FUN_WPU    (BIT(7))
#define CNNT_IO_MUX_SDIO_CLK_FUN_WPU_M  (CNNT_IO_MUX_SDIO_CLK_FUN_WPU_V << CNNT_IO_MUX_SDIO_CLK_FUN_WPU_S)
#define CNNT_IO_MUX_SDIO_CLK_FUN_WPU_V  0x00000001U
#define CNNT_IO_MUX_SDIO_CLK_FUN_WPU_S  7
/** CNNT_IO_MUX_SDIO_CLK_FUN_IE : R/W; bitpos: [8]; default: 1;
 *  Configures whether or not to enable input of SDIO_CLK.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_CLK_FUN_IE    (BIT(8))
#define CNNT_IO_MUX_SDIO_CLK_FUN_IE_M  (CNNT_IO_MUX_SDIO_CLK_FUN_IE_V << CNNT_IO_MUX_SDIO_CLK_FUN_IE_S)
#define CNNT_IO_MUX_SDIO_CLK_FUN_IE_V  0x00000001U
#define CNNT_IO_MUX_SDIO_CLK_FUN_IE_S  8
/** CNNT_IO_MUX_SDIO_CLK_FUN_DRV : R/W; bitpos: [10:9]; default: 2;
 *  Configures the drive strength of SDIO_CLK.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_SDIO_CLK_FUN_DRV    0x00000003U
#define CNNT_IO_MUX_SDIO_CLK_FUN_DRV_M  (CNNT_IO_MUX_SDIO_CLK_FUN_DRV_V << CNNT_IO_MUX_SDIO_CLK_FUN_DRV_S)
#define CNNT_IO_MUX_SDIO_CLK_FUN_DRV_V  0x00000003U
#define CNNT_IO_MUX_SDIO_CLK_FUN_DRV_S  9
/** CNNT_IO_MUX_SDIO_CLK_HYS_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  CNNT_IO_MUX_SDIO_CLK_HYS_SEL is set to 1.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_CLK_HYS_EN    (BIT(11))
#define CNNT_IO_MUX_SDIO_CLK_HYS_EN_M  (CNNT_IO_MUX_SDIO_CLK_HYS_EN_V << CNNT_IO_MUX_SDIO_CLK_HYS_EN_S)
#define CNNT_IO_MUX_SDIO_CLK_HYS_EN_V  0x00000001U
#define CNNT_IO_MUX_SDIO_CLK_HYS_EN_S  11
/** CNNT_IO_MUX_SDIO_CLK_HYS_SEL : R/W; bitpos: [12]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for SDIO_CLK.
 *  0: Choose the output enable signal of eFuse
 *  1: Choose the output enable signal of CNNT_IO_MUX_SDIO_CLK_HYS_EN
 */
#define CNNT_IO_MUX_SDIO_CLK_HYS_SEL    (BIT(12))
#define CNNT_IO_MUX_SDIO_CLK_HYS_SEL_M  (CNNT_IO_MUX_SDIO_CLK_HYS_SEL_V << CNNT_IO_MUX_SDIO_CLK_HYS_SEL_S)
#define CNNT_IO_MUX_SDIO_CLK_HYS_SEL_V  0x00000001U
#define CNNT_IO_MUX_SDIO_CLK_HYS_SEL_S  12

/** CNNT_IO_MUX_SDIO_CMD_REG register
 *  SDIO IO MUX configuration register for SDIO_CMD
 */
#define CNNT_IO_MUX_SDIO_CMD_REG (DR_REG_CNNT_IO_MUX_BASE + 0x30)
/** CNNT_IO_MUX_SDIO_CMD_SLP_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enter sleep mode for SDIO_CMD.
 *  0: Not enter
 *  1: Enter
 */
#define CNNT_IO_MUX_SDIO_CMD_SLP_SEL    (BIT(0))
#define CNNT_IO_MUX_SDIO_CMD_SLP_SEL_M  (CNNT_IO_MUX_SDIO_CMD_SLP_SEL_V << CNNT_IO_MUX_SDIO_CMD_SLP_SEL_S)
#define CNNT_IO_MUX_SDIO_CMD_SLP_SEL_V  0x00000001U
#define CNNT_IO_MUX_SDIO_CMD_SLP_SEL_S  0
/** CNNT_IO_MUX_SDIO_CMD_MCU_WPD : R/W; bitpos: [1]; default: 0;
 *  Configure whether or not to enable pull-down resistor of SDIO_CMD in sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_CMD_MCU_WPD    (BIT(1))
#define CNNT_IO_MUX_SDIO_CMD_MCU_WPD_M  (CNNT_IO_MUX_SDIO_CMD_MCU_WPD_V << CNNT_IO_MUX_SDIO_CMD_MCU_WPD_S)
#define CNNT_IO_MUX_SDIO_CMD_MCU_WPD_V  0x00000001U
#define CNNT_IO_MUX_SDIO_CMD_MCU_WPD_S  1
/** CNNT_IO_MUX_SDIO_CMD_MCU_WPU : R/W; bitpos: [2]; default: 1;
 *  Configures whether or not to enable pull-up resistor of SDIO_CMD during sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_CMD_MCU_WPU    (BIT(2))
#define CNNT_IO_MUX_SDIO_CMD_MCU_WPU_M  (CNNT_IO_MUX_SDIO_CMD_MCU_WPU_V << CNNT_IO_MUX_SDIO_CMD_MCU_WPU_S)
#define CNNT_IO_MUX_SDIO_CMD_MCU_WPU_V  0x00000001U
#define CNNT_IO_MUX_SDIO_CMD_MCU_WPU_S  2
/** CNNT_IO_MUX_SDIO_CMD_MCU_IE : R/W; bitpos: [3]; default: 1;
 *  Configures whether or not to enable the input of SDIO_CMD during sleep mode.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_CMD_MCU_IE    (BIT(3))
#define CNNT_IO_MUX_SDIO_CMD_MCU_IE_M  (CNNT_IO_MUX_SDIO_CMD_MCU_IE_V << CNNT_IO_MUX_SDIO_CMD_MCU_IE_S)
#define CNNT_IO_MUX_SDIO_CMD_MCU_IE_V  0x00000001U
#define CNNT_IO_MUX_SDIO_CMD_MCU_IE_S  3
/** CNNT_IO_MUX_SDIO_CMD_MCU_DRV : R/W; bitpos: [5:4]; default: 2;
 *  Configures the drive strength of SDIO_CMD during sleep mode.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_SDIO_CMD_MCU_DRV    0x00000003U
#define CNNT_IO_MUX_SDIO_CMD_MCU_DRV_M  (CNNT_IO_MUX_SDIO_CMD_MCU_DRV_V << CNNT_IO_MUX_SDIO_CMD_MCU_DRV_S)
#define CNNT_IO_MUX_SDIO_CMD_MCU_DRV_V  0x00000003U
#define CNNT_IO_MUX_SDIO_CMD_MCU_DRV_S  4
/** CNNT_IO_MUX_SDIO_CMD_FUN_WPD : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to enable pull-down resistor of SDIO_CMD.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_CMD_FUN_WPD    (BIT(6))
#define CNNT_IO_MUX_SDIO_CMD_FUN_WPD_M  (CNNT_IO_MUX_SDIO_CMD_FUN_WPD_V << CNNT_IO_MUX_SDIO_CMD_FUN_WPD_S)
#define CNNT_IO_MUX_SDIO_CMD_FUN_WPD_V  0x00000001U
#define CNNT_IO_MUX_SDIO_CMD_FUN_WPD_S  6
/** CNNT_IO_MUX_SDIO_CMD_FUN_WPU : R/W; bitpos: [7]; default: 1;
 *  Configures whether or not enable pull-up resistor of SDIO_CMD.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_CMD_FUN_WPU    (BIT(7))
#define CNNT_IO_MUX_SDIO_CMD_FUN_WPU_M  (CNNT_IO_MUX_SDIO_CMD_FUN_WPU_V << CNNT_IO_MUX_SDIO_CMD_FUN_WPU_S)
#define CNNT_IO_MUX_SDIO_CMD_FUN_WPU_V  0x00000001U
#define CNNT_IO_MUX_SDIO_CMD_FUN_WPU_S  7
/** CNNT_IO_MUX_SDIO_CMD_FUN_IE : R/W; bitpos: [8]; default: 1;
 *  Configures whether or not to enable input of SDIO_CMD.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_CMD_FUN_IE    (BIT(8))
#define CNNT_IO_MUX_SDIO_CMD_FUN_IE_M  (CNNT_IO_MUX_SDIO_CMD_FUN_IE_V << CNNT_IO_MUX_SDIO_CMD_FUN_IE_S)
#define CNNT_IO_MUX_SDIO_CMD_FUN_IE_V  0x00000001U
#define CNNT_IO_MUX_SDIO_CMD_FUN_IE_S  8
/** CNNT_IO_MUX_SDIO_CMD_FUN_DRV : R/W; bitpos: [10:9]; default: 2;
 *  Configures the drive strength of SDIO_CMD.
 *  0: ~5 mA
 *  1: ~10 mA
 *  2: ~20 mA
 *  3: ~40 mA
 */
#define CNNT_IO_MUX_SDIO_CMD_FUN_DRV    0x00000003U
#define CNNT_IO_MUX_SDIO_CMD_FUN_DRV_M  (CNNT_IO_MUX_SDIO_CMD_FUN_DRV_V << CNNT_IO_MUX_SDIO_CMD_FUN_DRV_S)
#define CNNT_IO_MUX_SDIO_CMD_FUN_DRV_V  0x00000003U
#define CNNT_IO_MUX_SDIO_CMD_FUN_DRV_S  9
/** CNNT_IO_MUX_SDIO_CMD_HYS_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  CNNT_IO_MUX_SDIO_CMD_HYS_SEL is set to 1.
 *  0: Disable
 *  1: Enable
 */
#define CNNT_IO_MUX_SDIO_CMD_HYS_EN    (BIT(11))
#define CNNT_IO_MUX_SDIO_CMD_HYS_EN_M  (CNNT_IO_MUX_SDIO_CMD_HYS_EN_V << CNNT_IO_MUX_SDIO_CMD_HYS_EN_S)
#define CNNT_IO_MUX_SDIO_CMD_HYS_EN_V  0x00000001U
#define CNNT_IO_MUX_SDIO_CMD_HYS_EN_S  11
/** CNNT_IO_MUX_SDIO_CMD_HYS_SEL : R/W; bitpos: [12]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for SDIO_CMD.
 *  0: Choose the output enable signal of eFuse
 *  1: Choose the output enable signal of CNNT_IO_MUX_SDIO_CMD_HYS_EN
 */
#define CNNT_IO_MUX_SDIO_CMD_HYS_SEL    (BIT(12))
#define CNNT_IO_MUX_SDIO_CMD_HYS_SEL_M  (CNNT_IO_MUX_SDIO_CMD_HYS_SEL_V << CNNT_IO_MUX_SDIO_CMD_HYS_SEL_S)
#define CNNT_IO_MUX_SDIO_CMD_HYS_SEL_V  0x00000001U
#define CNNT_IO_MUX_SDIO_CMD_HYS_SEL_S  12

/** CNNT_IO_MUX_CTRL_REG register
 *  SDIO IO MUX configuration register for ctrl sel
 */
#define CNNT_IO_MUX_CTRL_REG (DR_REG_CNNT_IO_MUX_BASE + 0x3f4)
/** CNNT_IO_MUX_SDIO_PAD_PIN_CTRL_DED_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to use dedicated ctrl signals for SDIO PAD.
 *  0: Not use dedicated ctrl signals
 *  1: Use dedicated ctrl signals
 */
#define CNNT_IO_MUX_SDIO_PAD_PIN_CTRL_DED_SEL    (BIT(0))
#define CNNT_IO_MUX_SDIO_PAD_PIN_CTRL_DED_SEL_M  (CNNT_IO_MUX_SDIO_PAD_PIN_CTRL_DED_SEL_V << CNNT_IO_MUX_SDIO_PAD_PIN_CTRL_DED_SEL_S)
#define CNNT_IO_MUX_SDIO_PAD_PIN_CTRL_DED_SEL_V  0x00000001U
#define CNNT_IO_MUX_SDIO_PAD_PIN_CTRL_DED_SEL_S  0
/** CNNT_IO_MUX_GMAC_PAD_PIN_CTRL_DED_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to use dedicated ctrl signals for GMAC PAD.
 *  0: Not use dedicated ctrl signals
 *  1: Use dedicated ctrl signals
 */
#define CNNT_IO_MUX_GMAC_PAD_PIN_CTRL_DED_SEL    (BIT(1))
#define CNNT_IO_MUX_GMAC_PAD_PIN_CTRL_DED_SEL_M  (CNNT_IO_MUX_GMAC_PAD_PIN_CTRL_DED_SEL_V << CNNT_IO_MUX_GMAC_PAD_PIN_CTRL_DED_SEL_S)
#define CNNT_IO_MUX_GMAC_PAD_PIN_CTRL_DED_SEL_V  0x00000001U
#define CNNT_IO_MUX_GMAC_PAD_PIN_CTRL_DED_SEL_S  1

/** CNNT_IO_MUX_CLOCK_GATE_REG register
 *  CNNT_IO_MUX clock gate register
 */
#define CNNT_IO_MUX_CLOCK_GATE_REG (DR_REG_CNNT_IO_MUX_BASE + 0x3f8)
/** CNNT_IO_MUX_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether or not to enable clock gate.
 *  0: Not enable
 *  1: Enable, the clock is free running.
 */
#define CNNT_IO_MUX_CLK_EN    (BIT(0))
#define CNNT_IO_MUX_CLK_EN_M  (CNNT_IO_MUX_CLK_EN_V << CNNT_IO_MUX_CLK_EN_S)
#define CNNT_IO_MUX_CLK_EN_V  0x00000001U
#define CNNT_IO_MUX_CLK_EN_S  0

/** CNNT_IO_MUX_DATE_REG register
 *  Version control register
 */
#define CNNT_IO_MUX_DATE_REG (DR_REG_CNNT_IO_MUX_BASE + 0x3fc)
/** CNNT_IO_MUX_REG_DATE : R/W; bitpos: [27:0]; default: 38834464;
 *  Version control register
 */
#define CNNT_IO_MUX_REG_DATE    0x0FFFFFFFU
#define CNNT_IO_MUX_REG_DATE_M  (CNNT_IO_MUX_REG_DATE_V << CNNT_IO_MUX_REG_DATE_S)
#define CNNT_IO_MUX_REG_DATE_V  0x0FFFFFFFU
#define CNNT_IO_MUX_REG_DATE_S  0

#ifdef __cplusplus
}
#endif
