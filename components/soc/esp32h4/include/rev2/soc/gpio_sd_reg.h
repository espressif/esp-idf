/**
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/soc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** GPIO_SD_SIGMADELTA0_REG register
 *  Duty Cycle Configure Register of SDM0
 */
#define GPIO_SD_SIGMADELTA0_REG (DR_REG_GPIO_SD_BASE + 0x0)
/** GPIO_SD_SD0_IN : R/W; bitpos: [7:0]; default: 0;
 *  This field is used to configure the duty cycle of sigma delta modulation output.
 */
#define GPIO_SD_SD0_IN    0x000000FFU
#define GPIO_SD_SD0_IN_M  (GPIO_SD_SD0_IN_V << GPIO_SD_SD0_IN_S)
#define GPIO_SD_SD0_IN_V  0x000000FFU
#define GPIO_SD_SD0_IN_S  0
/** GPIO_SD_SD0_PRESCALE : R/W; bitpos: [15:8]; default: 255;
 *  This field is used to set a divider value to divide APB clock.
 */
#define GPIO_SD_SD0_PRESCALE    0x000000FFU
#define GPIO_SD_SD0_PRESCALE_M  (GPIO_SD_SD0_PRESCALE_V << GPIO_SD_SD0_PRESCALE_S)
#define GPIO_SD_SD0_PRESCALE_V  0x000000FFU
#define GPIO_SD_SD0_PRESCALE_S  8

/** GPIO_SD_SIGMADELTA1_REG register
 *  Duty Cycle Configure Register of SDM1
 */
#define GPIO_SD_SIGMADELTA1_REG (DR_REG_GPIO_SD_BASE + 0x4)
/** GPIO_SD_SD1_IN : R/W; bitpos: [7:0]; default: 0;
 *  This field is used to configure the duty cycle of sigma delta modulation output.
 */
#define GPIO_SD_SD1_IN    0x000000FFU
#define GPIO_SD_SD1_IN_M  (GPIO_SD_SD1_IN_V << GPIO_SD_SD1_IN_S)
#define GPIO_SD_SD1_IN_V  0x000000FFU
#define GPIO_SD_SD1_IN_S  0
/** GPIO_SD_SD1_PRESCALE : R/W; bitpos: [15:8]; default: 255;
 *  This field is used to set a divider value to divide APB clock.
 */
#define GPIO_SD_SD1_PRESCALE    0x000000FFU
#define GPIO_SD_SD1_PRESCALE_M  (GPIO_SD_SD1_PRESCALE_V << GPIO_SD_SD1_PRESCALE_S)
#define GPIO_SD_SD1_PRESCALE_V  0x000000FFU
#define GPIO_SD_SD1_PRESCALE_S  8

/** GPIO_SD_SIGMADELTA2_REG register
 *  Duty Cycle Configure Register of SDM2
 */
#define GPIO_SD_SIGMADELTA2_REG (DR_REG_GPIO_SD_BASE + 0x8)
/** GPIO_SD_SD2_IN : R/W; bitpos: [7:0]; default: 0;
 *  This field is used to configure the duty cycle of sigma delta modulation output.
 */
#define GPIO_SD_SD2_IN    0x000000FFU
#define GPIO_SD_SD2_IN_M  (GPIO_SD_SD2_IN_V << GPIO_SD_SD2_IN_S)
#define GPIO_SD_SD2_IN_V  0x000000FFU
#define GPIO_SD_SD2_IN_S  0
/** GPIO_SD_SD2_PRESCALE : R/W; bitpos: [15:8]; default: 255;
 *  This field is used to set a divider value to divide APB clock.
 */
#define GPIO_SD_SD2_PRESCALE    0x000000FFU
#define GPIO_SD_SD2_PRESCALE_M  (GPIO_SD_SD2_PRESCALE_V << GPIO_SD_SD2_PRESCALE_S)
#define GPIO_SD_SD2_PRESCALE_V  0x000000FFU
#define GPIO_SD_SD2_PRESCALE_S  8

/** GPIO_SD_SIGMADELTA3_REG register
 *  Duty Cycle Configure Register of SDM3
 */
#define GPIO_SD_SIGMADELTA3_REG (DR_REG_GPIO_SD_BASE + 0xc)
/** GPIO_SD_SD3_IN : R/W; bitpos: [7:0]; default: 0;
 *  This field is used to configure the duty cycle of sigma delta modulation output.
 */
#define GPIO_SD_SD3_IN    0x000000FFU
#define GPIO_SD_SD3_IN_M  (GPIO_SD_SD3_IN_V << GPIO_SD_SD3_IN_S)
#define GPIO_SD_SD3_IN_V  0x000000FFU
#define GPIO_SD_SD3_IN_S  0
/** GPIO_SD_SD3_PRESCALE : R/W; bitpos: [15:8]; default: 255;
 *  This field is used to set a divider value to divide APB clock.
 */
#define GPIO_SD_SD3_PRESCALE    0x000000FFU
#define GPIO_SD_SD3_PRESCALE_M  (GPIO_SD_SD3_PRESCALE_V << GPIO_SD_SD3_PRESCALE_S)
#define GPIO_SD_SD3_PRESCALE_V  0x000000FFU
#define GPIO_SD_SD3_PRESCALE_S  8

/** GPIO_SD_SIGMADELTA_CG_REG register
 *  Clock Gating Configure Register
 */
#define GPIO_SD_SIGMADELTA_CG_REG (DR_REG_GPIO_SD_BASE + 0x20)
/** GPIO_SD_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  Clock enable bit of configuration registers for sigma delta modulation.
 */
#define GPIO_SD_CLK_EN    (BIT(31))
#define GPIO_SD_CLK_EN_M  (GPIO_SD_CLK_EN_V << GPIO_SD_CLK_EN_S)
#define GPIO_SD_CLK_EN_V  0x00000001U
#define GPIO_SD_CLK_EN_S  31

/** GPIO_SD_SIGMADELTA_MISC_REG register
 *  MISC Register
 */
#define GPIO_SD_SIGMADELTA_MISC_REG (DR_REG_GPIO_SD_BASE + 0x24)
/** GPIO_SD_FUNCTION_CLK_EN : R/W; bitpos: [30]; default: 0;
 *  Clock enable bit of sigma delta modulation.
 */
#define GPIO_SD_FUNCTION_CLK_EN    (BIT(30))
#define GPIO_SD_FUNCTION_CLK_EN_M  (GPIO_SD_FUNCTION_CLK_EN_V << GPIO_SD_FUNCTION_CLK_EN_S)
#define GPIO_SD_FUNCTION_CLK_EN_V  0x00000001U
#define GPIO_SD_FUNCTION_CLK_EN_S  30
/** GPIO_SD_SPI_SWAP : R/W; bitpos: [31]; default: 0;
 *  Reserved.
 */
#define GPIO_SD_SPI_SWAP    (BIT(31))
#define GPIO_SD_SPI_SWAP_M  (GPIO_SD_SPI_SWAP_V << GPIO_SD_SPI_SWAP_S)
#define GPIO_SD_SPI_SWAP_V  0x00000001U
#define GPIO_SD_SPI_SWAP_S  31

/** GPIO_SD_PAD_COMP_CONFIG_REG register
 *  PAD Compare configure Register
 */
#define GPIO_SD_PAD_COMP_CONFIG_REG (DR_REG_GPIO_SD_BASE + 0x28)
/** GPIO_SD_XPD_COMP : R/W; bitpos: [0]; default: 0;
 *  Pad compare enable bit.
 */
#define GPIO_SD_XPD_COMP    (BIT(0))
#define GPIO_SD_XPD_COMP_M  (GPIO_SD_XPD_COMP_V << GPIO_SD_XPD_COMP_S)
#define GPIO_SD_XPD_COMP_V  0x00000001U
#define GPIO_SD_XPD_COMP_S  0
/** GPIO_SD_MODE_COMP : R/W; bitpos: [1]; default: 0;
 *  1 to enable external reference from PAD[0]. 0 to enable internal reference,
 *  meanwhile PAD[0] can be used as a regular GPIO.
 */
#define GPIO_SD_MODE_COMP    (BIT(1))
#define GPIO_SD_MODE_COMP_M  (GPIO_SD_MODE_COMP_V << GPIO_SD_MODE_COMP_S)
#define GPIO_SD_MODE_COMP_V  0x00000001U
#define GPIO_SD_MODE_COMP_S  1
/** GPIO_SD_DREF_COMP : R/W; bitpos: [4:2]; default: 0;
 *  internal reference voltage tuning bit. 0V to 0.7*VDDPST step 0.1*VDDPST.
 */
#define GPIO_SD_DREF_COMP    0x00000007U
#define GPIO_SD_DREF_COMP_M  (GPIO_SD_DREF_COMP_V << GPIO_SD_DREF_COMP_S)
#define GPIO_SD_DREF_COMP_V  0x00000007U
#define GPIO_SD_DREF_COMP_S  2
/** GPIO_SD_ZERO_DET_MODE : R/W; bitpos: [6:5]; default: 0;
 *  Zero Detect mode select.
 */
#define GPIO_SD_ZERO_DET_MODE    0x00000003U
#define GPIO_SD_ZERO_DET_MODE_M  (GPIO_SD_ZERO_DET_MODE_V << GPIO_SD_ZERO_DET_MODE_S)
#define GPIO_SD_ZERO_DET_MODE_V  0x00000003U
#define GPIO_SD_ZERO_DET_MODE_S  5

/** GPIO_SD_PAD_COMP_FILTER_REG register
 *  Zero Detect filter Register
 */
#define GPIO_SD_PAD_COMP_FILTER_REG (DR_REG_GPIO_SD_BASE + 0x2c)
/** GPIO_SD_ZERO_DET_FILTER_CNT : R/W; bitpos: [31:0]; default: 0;
 *  Zero Detect filter cycle length
 */
#define GPIO_SD_ZERO_DET_FILTER_CNT    0xFFFFFFFFU
#define GPIO_SD_ZERO_DET_FILTER_CNT_M  (GPIO_SD_ZERO_DET_FILTER_CNT_V << GPIO_SD_ZERO_DET_FILTER_CNT_S)
#define GPIO_SD_ZERO_DET_FILTER_CNT_V  0xFFFFFFFFU
#define GPIO_SD_ZERO_DET_FILTER_CNT_S  0

/** GPIO_SD_INT_RAW_REG register
 *  GPIO_SD interrupt raw register
 */
#define GPIO_SD_INT_RAW_REG (DR_REG_GPIO_SD_BASE + 0x80)
/** GPIO_SD_PAD_COMP_INT_RAW : RO/WTC/SS; bitpos: [0]; default: 0;
 *  Pad compare raw interrupt
 */
#define GPIO_SD_PAD_COMP_INT_RAW    (BIT(0))
#define GPIO_SD_PAD_COMP_INT_RAW_M  (GPIO_SD_PAD_COMP_INT_RAW_V << GPIO_SD_PAD_COMP_INT_RAW_S)
#define GPIO_SD_PAD_COMP_INT_RAW_V  0x00000001U
#define GPIO_SD_PAD_COMP_INT_RAW_S  0

/** GPIO_SD_INT_ST_REG register
 *  GPIO_SD interrupt masked register
 */
#define GPIO_SD_INT_ST_REG (DR_REG_GPIO_SD_BASE + 0x84)
/** GPIO_SD_PAD_COMP_INT_ST : RO; bitpos: [0]; default: 0;
 *  Pad compare masked interrupt
 */
#define GPIO_SD_PAD_COMP_INT_ST    (BIT(0))
#define GPIO_SD_PAD_COMP_INT_ST_M  (GPIO_SD_PAD_COMP_INT_ST_V << GPIO_SD_PAD_COMP_INT_ST_S)
#define GPIO_SD_PAD_COMP_INT_ST_V  0x00000001U
#define GPIO_SD_PAD_COMP_INT_ST_S  0

/** GPIO_SD_INT_ENA_REG register
 *  GPIO_SD interrupt enable register
 */
#define GPIO_SD_INT_ENA_REG (DR_REG_GPIO_SD_BASE + 0x88)
/** GPIO_SD_PAD_COMP_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Pad compare interrupt enable
 */
#define GPIO_SD_PAD_COMP_INT_ENA    (BIT(0))
#define GPIO_SD_PAD_COMP_INT_ENA_M  (GPIO_SD_PAD_COMP_INT_ENA_V << GPIO_SD_PAD_COMP_INT_ENA_S)
#define GPIO_SD_PAD_COMP_INT_ENA_V  0x00000001U
#define GPIO_SD_PAD_COMP_INT_ENA_S  0

/** GPIO_SD_INT_CLR_REG register
 *  GPIO_SD interrupt clear register
 */
#define GPIO_SD_INT_CLR_REG (DR_REG_GPIO_SD_BASE + 0x8c)
/** GPIO_SD_PAD_COMP_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Pad compare interrupt clear
 */
#define GPIO_SD_PAD_COMP_INT_CLR    (BIT(0))
#define GPIO_SD_PAD_COMP_INT_CLR_M  (GPIO_SD_PAD_COMP_INT_CLR_V << GPIO_SD_PAD_COMP_INT_CLR_S)
#define GPIO_SD_PAD_COMP_INT_CLR_V  0x00000001U
#define GPIO_SD_PAD_COMP_INT_CLR_S  0

/** GPIO_SD_SIGMADELTA_VERSION_REG register
 *  Version Control Register
 */
#define GPIO_SD_SIGMADELTA_VERSION_REG (DR_REG_GPIO_SD_BASE + 0xfc)
/** GPIO_SD_GPIO_SD_DATE : R/W; bitpos: [27:0]; default: 34668848;
 *  Version control register.
 */
#define GPIO_SD_GPIO_SD_DATE    0x0FFFFFFFU
#define GPIO_SD_GPIO_SD_DATE_M  (GPIO_SD_GPIO_SD_DATE_V << GPIO_SD_GPIO_SD_DATE_S)
#define GPIO_SD_GPIO_SD_DATE_V  0x0FFFFFFFU
#define GPIO_SD_GPIO_SD_DATE_S  0

#ifdef __cplusplus
}
#endif
