/**
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** GPIO_SIGMADELTA0_REG register
 *  Duty Cycle Configure Register of SDM0
 */
#define GPIO_SIGMADELTA0_REG (DR_REG_GPIO_EXT_BASE + 0x0)
/** GPIO_SD0_IN : R/W; bitpos: [7:0]; default: 0;
 *  This field is used to configure the duty cycle of sigma delta modulation output.
 */
#define GPIO_SD0_IN    0x000000FFU
#define GPIO_SD0_IN_M  (GPIO_SD0_IN_V << GPIO_SD0_IN_S)
#define GPIO_SD0_IN_V  0x000000FFU
#define GPIO_SD0_IN_S  0
/** GPIO_SD0_PRESCALE : R/W; bitpos: [15:8]; default: 255;
 *  This field is used to set a divider value to divide APB clock.
 */
#define GPIO_SD0_PRESCALE    0x000000FFU
#define GPIO_SD0_PRESCALE_M  (GPIO_SD0_PRESCALE_V << GPIO_SD0_PRESCALE_S)
#define GPIO_SD0_PRESCALE_V  0x000000FFU
#define GPIO_SD0_PRESCALE_S  8

/** GPIO_SIGMADELTA1_REG register
 *  Duty Cycle Configure Register of SDM1
 */
#define GPIO_SIGMADELTA1_REG (DR_REG_GPIO_EXT_BASE + 0x4)
/** GPIO_SD1_IN : R/W; bitpos: [7:0]; default: 0;
 *  This field is used to configure the duty cycle of sigma delta modulation output.
 */
#define GPIO_SD1_IN    0x000000FFU
#define GPIO_SD1_IN_M  (GPIO_SD1_IN_V << GPIO_SD1_IN_S)
#define GPIO_SD1_IN_V  0x000000FFU
#define GPIO_SD1_IN_S  0
/** GPIO_SD1_PRESCALE : R/W; bitpos: [15:8]; default: 255;
 *  This field is used to set a divider value to divide APB clock.
 */
#define GPIO_SD1_PRESCALE    0x000000FFU
#define GPIO_SD1_PRESCALE_M  (GPIO_SD1_PRESCALE_V << GPIO_SD1_PRESCALE_S)
#define GPIO_SD1_PRESCALE_V  0x000000FFU
#define GPIO_SD1_PRESCALE_S  8

/** GPIO_SIGMADELTA2_REG register
 *  Duty Cycle Configure Register of SDM2
 */
#define GPIO_SIGMADELTA2_REG (DR_REG_GPIO_EXT_BASE + 0x8)
/** GPIO_SD2_IN : R/W; bitpos: [7:0]; default: 0;
 *  This field is used to configure the duty cycle of sigma delta modulation output.
 */
#define GPIO_SD2_IN    0x000000FFU
#define GPIO_SD2_IN_M  (GPIO_SD2_IN_V << GPIO_SD2_IN_S)
#define GPIO_SD2_IN_V  0x000000FFU
#define GPIO_SD2_IN_S  0
/** GPIO_SD2_PRESCALE : R/W; bitpos: [15:8]; default: 255;
 *  This field is used to set a divider value to divide APB clock.
 */
#define GPIO_SD2_PRESCALE    0x000000FFU
#define GPIO_SD2_PRESCALE_M  (GPIO_SD2_PRESCALE_V << GPIO_SD2_PRESCALE_S)
#define GPIO_SD2_PRESCALE_V  0x000000FFU
#define GPIO_SD2_PRESCALE_S  8

/** GPIO_SIGMADELTA3_REG register
 *  Duty Cycle Configure Register of SDM3
 */
#define GPIO_SIGMADELTA3_REG (DR_REG_GPIO_EXT_BASE + 0xc)
/** GPIO_SD3_IN : R/W; bitpos: [7:0]; default: 0;
 *  This field is used to configure the duty cycle of sigma delta modulation output.
 */
#define GPIO_SD3_IN    0x000000FFU
#define GPIO_SD3_IN_M  (GPIO_SD3_IN_V << GPIO_SD3_IN_S)
#define GPIO_SD3_IN_V  0x000000FFU
#define GPIO_SD3_IN_S  0
/** GPIO_SD3_PRESCALE : R/W; bitpos: [15:8]; default: 255;
 *  This field is used to set a divider value to divide APB clock.
 */
#define GPIO_SD3_PRESCALE    0x000000FFU
#define GPIO_SD3_PRESCALE_M  (GPIO_SD3_PRESCALE_V << GPIO_SD3_PRESCALE_S)
#define GPIO_SD3_PRESCALE_V  0x000000FFU
#define GPIO_SD3_PRESCALE_S  8

/** GPIO_CLOCK_GATE_REG register
 *  Clock Gating Configure Register
 */
#define GPIO_CLOCK_GATE_REG (DR_REG_GPIO_EXT_BASE + 0x20)
/** GPIO_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Clock enable bit of configuration registers for sigma delta modulation.
 */
#define GPIO_CLK_EN    (BIT(0))
#define GPIO_CLK_EN_M  (GPIO_CLK_EN_V << GPIO_CLK_EN_S)
#define GPIO_CLK_EN_V  0x00000001U
#define GPIO_CLK_EN_S  0

/** GPIO_SIGMADELTA_MISC_REG register
 *  MISC Register
 */
#define GPIO_SIGMADELTA_MISC_REG (DR_REG_GPIO_EXT_BASE + 0x24)
/** GPIO_FUNCTION_CLK_EN : R/W; bitpos: [30]; default: 0;
 *  Clock enable bit of sigma delta modulation.
 */
#define GPIO_FUNCTION_CLK_EN    (BIT(30))
#define GPIO_FUNCTION_CLK_EN_M  (GPIO_FUNCTION_CLK_EN_V << GPIO_FUNCTION_CLK_EN_S)
#define GPIO_FUNCTION_CLK_EN_V  0x00000001U
#define GPIO_FUNCTION_CLK_EN_S  30
/** GPIO_SPI_SWAP : R/W; bitpos: [31]; default: 0;
 *  Reserved.
 */
#define GPIO_SPI_SWAP    (BIT(31))
#define GPIO_SPI_SWAP_M  (GPIO_SPI_SWAP_V << GPIO_SPI_SWAP_S)
#define GPIO_SPI_SWAP_V  0x00000001U
#define GPIO_SPI_SWAP_S  31

/** GPIO_PAD_COMP_CONFIG_REG register
 *  PAD Compare configure Register
 */
#define GPIO_PAD_COMP_CONFIG_REG (DR_REG_GPIO_EXT_BASE + 0x28)
/** GPIO_XPD_COMP : R/W; bitpos: [0]; default: 0;
 *  Pad compare enable bit.
 */
#define GPIO_XPD_COMP    (BIT(0))
#define GPIO_XPD_COMP_M  (GPIO_XPD_COMP_V << GPIO_XPD_COMP_S)
#define GPIO_XPD_COMP_V  0x00000001U
#define GPIO_XPD_COMP_S  0
/** GPIO_MODE_COMP : R/W; bitpos: [1]; default: 0;
 *  1 to enable external reference from PAD[0]. 0 to enable internal reference,
 *  meanwhile PAD[0] can be used as a regular GPIO.
 */
#define GPIO_MODE_COMP    (BIT(1))
#define GPIO_MODE_COMP_M  (GPIO_MODE_COMP_V << GPIO_MODE_COMP_S)
#define GPIO_MODE_COMP_V  0x00000001U
#define GPIO_MODE_COMP_S  1
/** GPIO_DREF_COMP : R/W; bitpos: [4:2]; default: 0;
 *  internal reference voltage tuning bit. 0V to 0.7*VDDPST step 0.1*VDDPST.
 */
#define GPIO_DREF_COMP    0x00000007U
#define GPIO_DREF_COMP_M  (GPIO_DREF_COMP_V << GPIO_DREF_COMP_S)
#define GPIO_DREF_COMP_V  0x00000007U
#define GPIO_DREF_COMP_S  2
/** GPIO_ZERO_DET_MODE : R/W; bitpos: [6:5]; default: 0;
 *  Zero Detect mode select.
 */
#define GPIO_ZERO_DET_MODE    0x00000003U
#define GPIO_ZERO_DET_MODE_M  (GPIO_ZERO_DET_MODE_V << GPIO_ZERO_DET_MODE_S)
#define GPIO_ZERO_DET_MODE_V  0x00000003U
#define GPIO_ZERO_DET_MODE_S  5

/** GPIO_PAD_COMP_FILTER_REG register
 *  Zero Detect filter Register
 */
#define GPIO_PAD_COMP_FILTER_REG (DR_REG_GPIO_EXT_BASE + 0x2c)
/** GPIO_ZERO_DET_FILTER_CNT : R/W; bitpos: [31:0]; default: 0;
 *  Zero Detect filter cycle length
 */
#define GPIO_ZERO_DET_FILTER_CNT    0xFFFFFFFFU
#define GPIO_ZERO_DET_FILTER_CNT_M  (GPIO_ZERO_DET_FILTER_CNT_V << GPIO_ZERO_DET_FILTER_CNT_S)
#define GPIO_ZERO_DET_FILTER_CNT_V  0xFFFFFFFFU
#define GPIO_ZERO_DET_FILTER_CNT_S  0

/** GPIO_GLITCH_FILTER_CH0_REG register
 *  Glitch Filter Configure Register of Channel0
 */
#define GPIO_GLITCH_FILTER_CH0_REG (DR_REG_GPIO_EXT_BASE + 0x30)
/** GPIO_FILTER_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Glitch Filter channel enable bit.
 */
#define GPIO_FILTER_CH0_EN    (BIT(0))
#define GPIO_FILTER_CH0_EN_M  (GPIO_FILTER_CH0_EN_V << GPIO_FILTER_CH0_EN_S)
#define GPIO_FILTER_CH0_EN_V  0x00000001U
#define GPIO_FILTER_CH0_EN_S  0
/** GPIO_FILTER_CH0_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Glitch Filter input io number.
 */
#define GPIO_FILTER_CH0_INPUT_IO_NUM    0x0000003FU
#define GPIO_FILTER_CH0_INPUT_IO_NUM_M  (GPIO_FILTER_CH0_INPUT_IO_NUM_V << GPIO_FILTER_CH0_INPUT_IO_NUM_S)
#define GPIO_FILTER_CH0_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_FILTER_CH0_INPUT_IO_NUM_S  1
/** GPIO_FILTER_CH0_WINDOW_THRES : R/W; bitpos: [12:7]; default: 0;
 *  Glitch Filter window threshold.
 */
#define GPIO_FILTER_CH0_WINDOW_THRES    0x0000003FU
#define GPIO_FILTER_CH0_WINDOW_THRES_M  (GPIO_FILTER_CH0_WINDOW_THRES_V << GPIO_FILTER_CH0_WINDOW_THRES_S)
#define GPIO_FILTER_CH0_WINDOW_THRES_V  0x0000003FU
#define GPIO_FILTER_CH0_WINDOW_THRES_S  7
/** GPIO_FILTER_CH0_WINDOW_WIDTH : R/W; bitpos: [18:13]; default: 0;
 *  Glitch Filter window width.
 */
#define GPIO_FILTER_CH0_WINDOW_WIDTH    0x0000003FU
#define GPIO_FILTER_CH0_WINDOW_WIDTH_M  (GPIO_FILTER_CH0_WINDOW_WIDTH_V << GPIO_FILTER_CH0_WINDOW_WIDTH_S)
#define GPIO_FILTER_CH0_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_FILTER_CH0_WINDOW_WIDTH_S  13

/** GPIO_GLITCH_FILTER_CH1_REG register
 *  Glitch Filter Configure Register of Channel1
 */
#define GPIO_GLITCH_FILTER_CH1_REG (DR_REG_GPIO_EXT_BASE + 0x34)
/** GPIO_FILTER_CH1_EN : R/W; bitpos: [0]; default: 0;
 *  Glitch Filter channel enable bit.
 */
#define GPIO_FILTER_CH1_EN    (BIT(0))
#define GPIO_FILTER_CH1_EN_M  (GPIO_FILTER_CH1_EN_V << GPIO_FILTER_CH1_EN_S)
#define GPIO_FILTER_CH1_EN_V  0x00000001U
#define GPIO_FILTER_CH1_EN_S  0
/** GPIO_FILTER_CH1_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Glitch Filter input io number.
 */
#define GPIO_FILTER_CH1_INPUT_IO_NUM    0x0000003FU
#define GPIO_FILTER_CH1_INPUT_IO_NUM_M  (GPIO_FILTER_CH1_INPUT_IO_NUM_V << GPIO_FILTER_CH1_INPUT_IO_NUM_S)
#define GPIO_FILTER_CH1_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_FILTER_CH1_INPUT_IO_NUM_S  1
/** GPIO_FILTER_CH1_WINDOW_THRES : R/W; bitpos: [12:7]; default: 0;
 *  Glitch Filter window threshold.
 */
#define GPIO_FILTER_CH1_WINDOW_THRES    0x0000003FU
#define GPIO_FILTER_CH1_WINDOW_THRES_M  (GPIO_FILTER_CH1_WINDOW_THRES_V << GPIO_FILTER_CH1_WINDOW_THRES_S)
#define GPIO_FILTER_CH1_WINDOW_THRES_V  0x0000003FU
#define GPIO_FILTER_CH1_WINDOW_THRES_S  7
/** GPIO_FILTER_CH1_WINDOW_WIDTH : R/W; bitpos: [18:13]; default: 0;
 *  Glitch Filter window width.
 */
#define GPIO_FILTER_CH1_WINDOW_WIDTH    0x0000003FU
#define GPIO_FILTER_CH1_WINDOW_WIDTH_M  (GPIO_FILTER_CH1_WINDOW_WIDTH_V << GPIO_FILTER_CH1_WINDOW_WIDTH_S)
#define GPIO_FILTER_CH1_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_FILTER_CH1_WINDOW_WIDTH_S  13

/** GPIO_GLITCH_FILTER_CH2_REG register
 *  Glitch Filter Configure Register of Channel2
 */
#define GPIO_GLITCH_FILTER_CH2_REG (DR_REG_GPIO_EXT_BASE + 0x38)
/** GPIO_FILTER_CH2_EN : R/W; bitpos: [0]; default: 0;
 *  Glitch Filter channel enable bit.
 */
#define GPIO_FILTER_CH2_EN    (BIT(0))
#define GPIO_FILTER_CH2_EN_M  (GPIO_FILTER_CH2_EN_V << GPIO_FILTER_CH2_EN_S)
#define GPIO_FILTER_CH2_EN_V  0x00000001U
#define GPIO_FILTER_CH2_EN_S  0
/** GPIO_FILTER_CH2_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Glitch Filter input io number.
 */
#define GPIO_FILTER_CH2_INPUT_IO_NUM    0x0000003FU
#define GPIO_FILTER_CH2_INPUT_IO_NUM_M  (GPIO_FILTER_CH2_INPUT_IO_NUM_V << GPIO_FILTER_CH2_INPUT_IO_NUM_S)
#define GPIO_FILTER_CH2_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_FILTER_CH2_INPUT_IO_NUM_S  1
/** GPIO_FILTER_CH2_WINDOW_THRES : R/W; bitpos: [12:7]; default: 0;
 *  Glitch Filter window threshold.
 */
#define GPIO_FILTER_CH2_WINDOW_THRES    0x0000003FU
#define GPIO_FILTER_CH2_WINDOW_THRES_M  (GPIO_FILTER_CH2_WINDOW_THRES_V << GPIO_FILTER_CH2_WINDOW_THRES_S)
#define GPIO_FILTER_CH2_WINDOW_THRES_V  0x0000003FU
#define GPIO_FILTER_CH2_WINDOW_THRES_S  7
/** GPIO_FILTER_CH2_WINDOW_WIDTH : R/W; bitpos: [18:13]; default: 0;
 *  Glitch Filter window width.
 */
#define GPIO_FILTER_CH2_WINDOW_WIDTH    0x0000003FU
#define GPIO_FILTER_CH2_WINDOW_WIDTH_M  (GPIO_FILTER_CH2_WINDOW_WIDTH_V << GPIO_FILTER_CH2_WINDOW_WIDTH_S)
#define GPIO_FILTER_CH2_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_FILTER_CH2_WINDOW_WIDTH_S  13

/** GPIO_GLITCH_FILTER_CH3_REG register
 *  Glitch Filter Configure Register of Channel3
 */
#define GPIO_GLITCH_FILTER_CH3_REG (DR_REG_GPIO_EXT_BASE + 0x3c)
/** GPIO_FILTER_CH3_EN : R/W; bitpos: [0]; default: 0;
 *  Glitch Filter channel enable bit.
 */
#define GPIO_FILTER_CH3_EN    (BIT(0))
#define GPIO_FILTER_CH3_EN_M  (GPIO_FILTER_CH3_EN_V << GPIO_FILTER_CH3_EN_S)
#define GPIO_FILTER_CH3_EN_V  0x00000001U
#define GPIO_FILTER_CH3_EN_S  0
/** GPIO_FILTER_CH3_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Glitch Filter input io number.
 */
#define GPIO_FILTER_CH3_INPUT_IO_NUM    0x0000003FU
#define GPIO_FILTER_CH3_INPUT_IO_NUM_M  (GPIO_FILTER_CH3_INPUT_IO_NUM_V << GPIO_FILTER_CH3_INPUT_IO_NUM_S)
#define GPIO_FILTER_CH3_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_FILTER_CH3_INPUT_IO_NUM_S  1
/** GPIO_FILTER_CH3_WINDOW_THRES : R/W; bitpos: [12:7]; default: 0;
 *  Glitch Filter window threshold.
 */
#define GPIO_FILTER_CH3_WINDOW_THRES    0x0000003FU
#define GPIO_FILTER_CH3_WINDOW_THRES_M  (GPIO_FILTER_CH3_WINDOW_THRES_V << GPIO_FILTER_CH3_WINDOW_THRES_S)
#define GPIO_FILTER_CH3_WINDOW_THRES_V  0x0000003FU
#define GPIO_FILTER_CH3_WINDOW_THRES_S  7
/** GPIO_FILTER_CH3_WINDOW_WIDTH : R/W; bitpos: [18:13]; default: 0;
 *  Glitch Filter window width.
 */
#define GPIO_FILTER_CH3_WINDOW_WIDTH    0x0000003FU
#define GPIO_FILTER_CH3_WINDOW_WIDTH_M  (GPIO_FILTER_CH3_WINDOW_WIDTH_V << GPIO_FILTER_CH3_WINDOW_WIDTH_S)
#define GPIO_FILTER_CH3_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_FILTER_CH3_WINDOW_WIDTH_S  13

/** GPIO_GLITCH_FILTER_CH4_REG register
 *  Glitch Filter Configure Register of Channel4
 */
#define GPIO_GLITCH_FILTER_CH4_REG (DR_REG_GPIO_EXT_BASE + 0x40)
/** GPIO_FILTER_CH4_EN : R/W; bitpos: [0]; default: 0;
 *  Glitch Filter channel enable bit.
 */
#define GPIO_FILTER_CH4_EN    (BIT(0))
#define GPIO_FILTER_CH4_EN_M  (GPIO_FILTER_CH4_EN_V << GPIO_FILTER_CH4_EN_S)
#define GPIO_FILTER_CH4_EN_V  0x00000001U
#define GPIO_FILTER_CH4_EN_S  0
/** GPIO_FILTER_CH4_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Glitch Filter input io number.
 */
#define GPIO_FILTER_CH4_INPUT_IO_NUM    0x0000003FU
#define GPIO_FILTER_CH4_INPUT_IO_NUM_M  (GPIO_FILTER_CH4_INPUT_IO_NUM_V << GPIO_FILTER_CH4_INPUT_IO_NUM_S)
#define GPIO_FILTER_CH4_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_FILTER_CH4_INPUT_IO_NUM_S  1
/** GPIO_FILTER_CH4_WINDOW_THRES : R/W; bitpos: [12:7]; default: 0;
 *  Glitch Filter window threshold.
 */
#define GPIO_FILTER_CH4_WINDOW_THRES    0x0000003FU
#define GPIO_FILTER_CH4_WINDOW_THRES_M  (GPIO_FILTER_CH4_WINDOW_THRES_V << GPIO_FILTER_CH4_WINDOW_THRES_S)
#define GPIO_FILTER_CH4_WINDOW_THRES_V  0x0000003FU
#define GPIO_FILTER_CH4_WINDOW_THRES_S  7
/** GPIO_FILTER_CH4_WINDOW_WIDTH : R/W; bitpos: [18:13]; default: 0;
 *  Glitch Filter window width.
 */
#define GPIO_FILTER_CH4_WINDOW_WIDTH    0x0000003FU
#define GPIO_FILTER_CH4_WINDOW_WIDTH_M  (GPIO_FILTER_CH4_WINDOW_WIDTH_V << GPIO_FILTER_CH4_WINDOW_WIDTH_S)
#define GPIO_FILTER_CH4_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_FILTER_CH4_WINDOW_WIDTH_S  13

/** GPIO_GLITCH_FILTER_CH5_REG register
 *  Glitch Filter Configure Register of Channel5
 */
#define GPIO_GLITCH_FILTER_CH5_REG (DR_REG_GPIO_EXT_BASE + 0x44)
/** GPIO_FILTER_CH5_EN : R/W; bitpos: [0]; default: 0;
 *  Glitch Filter channel enable bit.
 */
#define GPIO_FILTER_CH5_EN    (BIT(0))
#define GPIO_FILTER_CH5_EN_M  (GPIO_FILTER_CH5_EN_V << GPIO_FILTER_CH5_EN_S)
#define GPIO_FILTER_CH5_EN_V  0x00000001U
#define GPIO_FILTER_CH5_EN_S  0
/** GPIO_FILTER_CH5_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Glitch Filter input io number.
 */
#define GPIO_FILTER_CH5_INPUT_IO_NUM    0x0000003FU
#define GPIO_FILTER_CH5_INPUT_IO_NUM_M  (GPIO_FILTER_CH5_INPUT_IO_NUM_V << GPIO_FILTER_CH5_INPUT_IO_NUM_S)
#define GPIO_FILTER_CH5_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_FILTER_CH5_INPUT_IO_NUM_S  1
/** GPIO_FILTER_CH5_WINDOW_THRES : R/W; bitpos: [12:7]; default: 0;
 *  Glitch Filter window threshold.
 */
#define GPIO_FILTER_CH5_WINDOW_THRES    0x0000003FU
#define GPIO_FILTER_CH5_WINDOW_THRES_M  (GPIO_FILTER_CH5_WINDOW_THRES_V << GPIO_FILTER_CH5_WINDOW_THRES_S)
#define GPIO_FILTER_CH5_WINDOW_THRES_V  0x0000003FU
#define GPIO_FILTER_CH5_WINDOW_THRES_S  7
/** GPIO_FILTER_CH5_WINDOW_WIDTH : R/W; bitpos: [18:13]; default: 0;
 *  Glitch Filter window width.
 */
#define GPIO_FILTER_CH5_WINDOW_WIDTH    0x0000003FU
#define GPIO_FILTER_CH5_WINDOW_WIDTH_M  (GPIO_FILTER_CH5_WINDOW_WIDTH_V << GPIO_FILTER_CH5_WINDOW_WIDTH_S)
#define GPIO_FILTER_CH5_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_FILTER_CH5_WINDOW_WIDTH_S  13

/** GPIO_GLITCH_FILTER_CH6_REG register
 *  Glitch Filter Configure Register of Channel6
 */
#define GPIO_GLITCH_FILTER_CH6_REG (DR_REG_GPIO_EXT_BASE + 0x48)
/** GPIO_FILTER_CH6_EN : R/W; bitpos: [0]; default: 0;
 *  Glitch Filter channel enable bit.
 */
#define GPIO_FILTER_CH6_EN    (BIT(0))
#define GPIO_FILTER_CH6_EN_M  (GPIO_FILTER_CH6_EN_V << GPIO_FILTER_CH6_EN_S)
#define GPIO_FILTER_CH6_EN_V  0x00000001U
#define GPIO_FILTER_CH6_EN_S  0
/** GPIO_FILTER_CH6_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Glitch Filter input io number.
 */
#define GPIO_FILTER_CH6_INPUT_IO_NUM    0x0000003FU
#define GPIO_FILTER_CH6_INPUT_IO_NUM_M  (GPIO_FILTER_CH6_INPUT_IO_NUM_V << GPIO_FILTER_CH6_INPUT_IO_NUM_S)
#define GPIO_FILTER_CH6_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_FILTER_CH6_INPUT_IO_NUM_S  1
/** GPIO_FILTER_CH6_WINDOW_THRES : R/W; bitpos: [12:7]; default: 0;
 *  Glitch Filter window threshold.
 */
#define GPIO_FILTER_CH6_WINDOW_THRES    0x0000003FU
#define GPIO_FILTER_CH6_WINDOW_THRES_M  (GPIO_FILTER_CH6_WINDOW_THRES_V << GPIO_FILTER_CH6_WINDOW_THRES_S)
#define GPIO_FILTER_CH6_WINDOW_THRES_V  0x0000003FU
#define GPIO_FILTER_CH6_WINDOW_THRES_S  7
/** GPIO_FILTER_CH6_WINDOW_WIDTH : R/W; bitpos: [18:13]; default: 0;
 *  Glitch Filter window width.
 */
#define GPIO_FILTER_CH6_WINDOW_WIDTH    0x0000003FU
#define GPIO_FILTER_CH6_WINDOW_WIDTH_M  (GPIO_FILTER_CH6_WINDOW_WIDTH_V << GPIO_FILTER_CH6_WINDOW_WIDTH_S)
#define GPIO_FILTER_CH6_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_FILTER_CH6_WINDOW_WIDTH_S  13

/** GPIO_GLITCH_FILTER_CH7_REG register
 *  Glitch Filter Configure Register of Channel7
 */
#define GPIO_GLITCH_FILTER_CH7_REG (DR_REG_GPIO_EXT_BASE + 0x4c)
/** GPIO_FILTER_CH7_EN : R/W; bitpos: [0]; default: 0;
 *  Glitch Filter channel enable bit.
 */
#define GPIO_FILTER_CH7_EN    (BIT(0))
#define GPIO_FILTER_CH7_EN_M  (GPIO_FILTER_CH7_EN_V << GPIO_FILTER_CH7_EN_S)
#define GPIO_FILTER_CH7_EN_V  0x00000001U
#define GPIO_FILTER_CH7_EN_S  0
/** GPIO_FILTER_CH7_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Glitch Filter input io number.
 */
#define GPIO_FILTER_CH7_INPUT_IO_NUM    0x0000003FU
#define GPIO_FILTER_CH7_INPUT_IO_NUM_M  (GPIO_FILTER_CH7_INPUT_IO_NUM_V << GPIO_FILTER_CH7_INPUT_IO_NUM_S)
#define GPIO_FILTER_CH7_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_FILTER_CH7_INPUT_IO_NUM_S  1
/** GPIO_FILTER_CH7_WINDOW_THRES : R/W; bitpos: [12:7]; default: 0;
 *  Glitch Filter window threshold.
 */
#define GPIO_FILTER_CH7_WINDOW_THRES    0x0000003FU
#define GPIO_FILTER_CH7_WINDOW_THRES_M  (GPIO_FILTER_CH7_WINDOW_THRES_V << GPIO_FILTER_CH7_WINDOW_THRES_S)
#define GPIO_FILTER_CH7_WINDOW_THRES_V  0x0000003FU
#define GPIO_FILTER_CH7_WINDOW_THRES_S  7
/** GPIO_FILTER_CH7_WINDOW_WIDTH : R/W; bitpos: [18:13]; default: 0;
 *  Glitch Filter window width.
 */
#define GPIO_FILTER_CH7_WINDOW_WIDTH    0x0000003FU
#define GPIO_FILTER_CH7_WINDOW_WIDTH_M  (GPIO_FILTER_CH7_WINDOW_WIDTH_V << GPIO_FILTER_CH7_WINDOW_WIDTH_S)
#define GPIO_FILTER_CH7_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_FILTER_CH7_WINDOW_WIDTH_S  13

/** GPIO_ETM_EVENT_CH0_CFG_REG register
 *  Etm Config register of Channel0
 */
#define GPIO_ETM_EVENT_CH0_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x60)
/** GPIO_ETM_CH0_EVENT_SEL : R/W; bitpos: [4:0]; default: 0;
 *  Etm event channel select gpio.
 */
#define GPIO_ETM_CH0_EVENT_SEL    0x0000001FU
#define GPIO_ETM_CH0_EVENT_SEL_M  (GPIO_ETM_CH0_EVENT_SEL_V << GPIO_ETM_CH0_EVENT_SEL_S)
#define GPIO_ETM_CH0_EVENT_SEL_V  0x0000001FU
#define GPIO_ETM_CH0_EVENT_SEL_S  0
/** GPIO_ETM_CH0_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Etm event send enable bit.
 */
#define GPIO_ETM_CH0_EVENT_EN    (BIT(7))
#define GPIO_ETM_CH0_EVENT_EN_M  (GPIO_ETM_CH0_EVENT_EN_V << GPIO_ETM_CH0_EVENT_EN_S)
#define GPIO_ETM_CH0_EVENT_EN_V  0x00000001U
#define GPIO_ETM_CH0_EVENT_EN_S  7

/** GPIO_ETM_EVENT_CH1_CFG_REG register
 *  Etm Config register of Channel1
 */
#define GPIO_ETM_EVENT_CH1_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x64)
/** GPIO_ETM_CH1_EVENT_SEL : R/W; bitpos: [4:0]; default: 0;
 *  Etm event channel select gpio.
 */
#define GPIO_ETM_CH1_EVENT_SEL    0x0000001FU
#define GPIO_ETM_CH1_EVENT_SEL_M  (GPIO_ETM_CH1_EVENT_SEL_V << GPIO_ETM_CH1_EVENT_SEL_S)
#define GPIO_ETM_CH1_EVENT_SEL_V  0x0000001FU
#define GPIO_ETM_CH1_EVENT_SEL_S  0
/** GPIO_ETM_CH1_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Etm event send enable bit.
 */
#define GPIO_ETM_CH1_EVENT_EN    (BIT(7))
#define GPIO_ETM_CH1_EVENT_EN_M  (GPIO_ETM_CH1_EVENT_EN_V << GPIO_ETM_CH1_EVENT_EN_S)
#define GPIO_ETM_CH1_EVENT_EN_V  0x00000001U
#define GPIO_ETM_CH1_EVENT_EN_S  7

/** GPIO_ETM_EVENT_CH2_CFG_REG register
 *  Etm Config register of Channel2
 */
#define GPIO_ETM_EVENT_CH2_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x68)
/** GPIO_ETM_CH2_EVENT_SEL : R/W; bitpos: [4:0]; default: 0;
 *  Etm event channel select gpio.
 */
#define GPIO_ETM_CH2_EVENT_SEL    0x0000001FU
#define GPIO_ETM_CH2_EVENT_SEL_M  (GPIO_ETM_CH2_EVENT_SEL_V << GPIO_ETM_CH2_EVENT_SEL_S)
#define GPIO_ETM_CH2_EVENT_SEL_V  0x0000001FU
#define GPIO_ETM_CH2_EVENT_SEL_S  0
/** GPIO_ETM_CH2_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Etm event send enable bit.
 */
#define GPIO_ETM_CH2_EVENT_EN    (BIT(7))
#define GPIO_ETM_CH2_EVENT_EN_M  (GPIO_ETM_CH2_EVENT_EN_V << GPIO_ETM_CH2_EVENT_EN_S)
#define GPIO_ETM_CH2_EVENT_EN_V  0x00000001U
#define GPIO_ETM_CH2_EVENT_EN_S  7

/** GPIO_ETM_EVENT_CH3_CFG_REG register
 *  Etm Config register of Channel3
 */
#define GPIO_ETM_EVENT_CH3_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x6c)
/** GPIO_ETM_CH3_EVENT_SEL : R/W; bitpos: [4:0]; default: 0;
 *  Etm event channel select gpio.
 */
#define GPIO_ETM_CH3_EVENT_SEL    0x0000001FU
#define GPIO_ETM_CH3_EVENT_SEL_M  (GPIO_ETM_CH3_EVENT_SEL_V << GPIO_ETM_CH3_EVENT_SEL_S)
#define GPIO_ETM_CH3_EVENT_SEL_V  0x0000001FU
#define GPIO_ETM_CH3_EVENT_SEL_S  0
/** GPIO_ETM_CH3_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Etm event send enable bit.
 */
#define GPIO_ETM_CH3_EVENT_EN    (BIT(7))
#define GPIO_ETM_CH3_EVENT_EN_M  (GPIO_ETM_CH3_EVENT_EN_V << GPIO_ETM_CH3_EVENT_EN_S)
#define GPIO_ETM_CH3_EVENT_EN_V  0x00000001U
#define GPIO_ETM_CH3_EVENT_EN_S  7

/** GPIO_ETM_EVENT_CH4_CFG_REG register
 *  Etm Config register of Channel4
 */
#define GPIO_ETM_EVENT_CH4_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x70)
/** GPIO_ETM_CH4_EVENT_SEL : R/W; bitpos: [4:0]; default: 0;
 *  Etm event channel select gpio.
 */
#define GPIO_ETM_CH4_EVENT_SEL    0x0000001FU
#define GPIO_ETM_CH4_EVENT_SEL_M  (GPIO_ETM_CH4_EVENT_SEL_V << GPIO_ETM_CH4_EVENT_SEL_S)
#define GPIO_ETM_CH4_EVENT_SEL_V  0x0000001FU
#define GPIO_ETM_CH4_EVENT_SEL_S  0
/** GPIO_ETM_CH4_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Etm event send enable bit.
 */
#define GPIO_ETM_CH4_EVENT_EN    (BIT(7))
#define GPIO_ETM_CH4_EVENT_EN_M  (GPIO_ETM_CH4_EVENT_EN_V << GPIO_ETM_CH4_EVENT_EN_S)
#define GPIO_ETM_CH4_EVENT_EN_V  0x00000001U
#define GPIO_ETM_CH4_EVENT_EN_S  7

/** GPIO_ETM_EVENT_CH5_CFG_REG register
 *  Etm Config register of Channel5
 */
#define GPIO_ETM_EVENT_CH5_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x74)
/** GPIO_ETM_CH5_EVENT_SEL : R/W; bitpos: [4:0]; default: 0;
 *  Etm event channel select gpio.
 */
#define GPIO_ETM_CH5_EVENT_SEL    0x0000001FU
#define GPIO_ETM_CH5_EVENT_SEL_M  (GPIO_ETM_CH5_EVENT_SEL_V << GPIO_ETM_CH5_EVENT_SEL_S)
#define GPIO_ETM_CH5_EVENT_SEL_V  0x0000001FU
#define GPIO_ETM_CH5_EVENT_SEL_S  0
/** GPIO_ETM_CH5_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Etm event send enable bit.
 */
#define GPIO_ETM_CH5_EVENT_EN    (BIT(7))
#define GPIO_ETM_CH5_EVENT_EN_M  (GPIO_ETM_CH5_EVENT_EN_V << GPIO_ETM_CH5_EVENT_EN_S)
#define GPIO_ETM_CH5_EVENT_EN_V  0x00000001U
#define GPIO_ETM_CH5_EVENT_EN_S  7

/** GPIO_ETM_EVENT_CH6_CFG_REG register
 *  Etm Config register of Channel6
 */
#define GPIO_ETM_EVENT_CH6_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x78)
/** GPIO_ETM_CH6_EVENT_SEL : R/W; bitpos: [4:0]; default: 0;
 *  Etm event channel select gpio.
 */
#define GPIO_ETM_CH6_EVENT_SEL    0x0000001FU
#define GPIO_ETM_CH6_EVENT_SEL_M  (GPIO_ETM_CH6_EVENT_SEL_V << GPIO_ETM_CH6_EVENT_SEL_S)
#define GPIO_ETM_CH6_EVENT_SEL_V  0x0000001FU
#define GPIO_ETM_CH6_EVENT_SEL_S  0
/** GPIO_ETM_CH6_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Etm event send enable bit.
 */
#define GPIO_ETM_CH6_EVENT_EN    (BIT(7))
#define GPIO_ETM_CH6_EVENT_EN_M  (GPIO_ETM_CH6_EVENT_EN_V << GPIO_ETM_CH6_EVENT_EN_S)
#define GPIO_ETM_CH6_EVENT_EN_V  0x00000001U
#define GPIO_ETM_CH6_EVENT_EN_S  7

/** GPIO_ETM_EVENT_CH7_CFG_REG register
 *  Etm Config register of Channel7
 */
#define GPIO_ETM_EVENT_CH7_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x7c)
/** GPIO_ETM_CH7_EVENT_SEL : R/W; bitpos: [4:0]; default: 0;
 *  Etm event channel select gpio.
 */
#define GPIO_ETM_CH7_EVENT_SEL    0x0000001FU
#define GPIO_ETM_CH7_EVENT_SEL_M  (GPIO_ETM_CH7_EVENT_SEL_V << GPIO_ETM_CH7_EVENT_SEL_S)
#define GPIO_ETM_CH7_EVENT_SEL_V  0x0000001FU
#define GPIO_ETM_CH7_EVENT_SEL_S  0
/** GPIO_ETM_CH7_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Etm event send enable bit.
 */
#define GPIO_ETM_CH7_EVENT_EN    (BIT(7))
#define GPIO_ETM_CH7_EVENT_EN_M  (GPIO_ETM_CH7_EVENT_EN_V << GPIO_ETM_CH7_EVENT_EN_S)
#define GPIO_ETM_CH7_EVENT_EN_V  0x00000001U
#define GPIO_ETM_CH7_EVENT_EN_S  7

/** GPIO_ETM_TASK_P0_CFG_REG register
 *  Etm Configure Register to decide which GPIO been chosen
 */
#define GPIO_ETM_TASK_P0_CFG_REG (DR_REG_GPIO_EXT_BASE + 0xa0)
/** GPIO_ETM_TASK_GPIO0_EN : R/W; bitpos: [0]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO0_EN    (BIT(0))
#define GPIO_ETM_TASK_GPIO0_EN_M  (GPIO_ETM_TASK_GPIO0_EN_V << GPIO_ETM_TASK_GPIO0_EN_S)
#define GPIO_ETM_TASK_GPIO0_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO0_EN_S  0
/** GPIO_ETM_TASK_GPIO0_SEL : R/W; bitpos: [3:1]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO0_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO0_SEL_M  (GPIO_ETM_TASK_GPIO0_SEL_V << GPIO_ETM_TASK_GPIO0_SEL_S)
#define GPIO_ETM_TASK_GPIO0_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO0_SEL_S  1
/** GPIO_ETM_TASK_GPIO1_EN : R/W; bitpos: [8]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO1_EN    (BIT(8))
#define GPIO_ETM_TASK_GPIO1_EN_M  (GPIO_ETM_TASK_GPIO1_EN_V << GPIO_ETM_TASK_GPIO1_EN_S)
#define GPIO_ETM_TASK_GPIO1_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO1_EN_S  8
/** GPIO_ETM_TASK_GPIO1_SEL : R/W; bitpos: [11:9]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO1_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO1_SEL_M  (GPIO_ETM_TASK_GPIO1_SEL_V << GPIO_ETM_TASK_GPIO1_SEL_S)
#define GPIO_ETM_TASK_GPIO1_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO1_SEL_S  9
/** GPIO_ETM_TASK_GPIO2_EN : R/W; bitpos: [16]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO2_EN    (BIT(16))
#define GPIO_ETM_TASK_GPIO2_EN_M  (GPIO_ETM_TASK_GPIO2_EN_V << GPIO_ETM_TASK_GPIO2_EN_S)
#define GPIO_ETM_TASK_GPIO2_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO2_EN_S  16
/** GPIO_ETM_TASK_GPIO2_SEL : R/W; bitpos: [19:17]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO2_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO2_SEL_M  (GPIO_ETM_TASK_GPIO2_SEL_V << GPIO_ETM_TASK_GPIO2_SEL_S)
#define GPIO_ETM_TASK_GPIO2_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO2_SEL_S  17
/** GPIO_ETM_TASK_GPIO3_EN : R/W; bitpos: [24]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO3_EN    (BIT(24))
#define GPIO_ETM_TASK_GPIO3_EN_M  (GPIO_ETM_TASK_GPIO3_EN_V << GPIO_ETM_TASK_GPIO3_EN_S)
#define GPIO_ETM_TASK_GPIO3_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO3_EN_S  24
/** GPIO_ETM_TASK_GPIO3_SEL : R/W; bitpos: [27:25]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO3_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO3_SEL_M  (GPIO_ETM_TASK_GPIO3_SEL_V << GPIO_ETM_TASK_GPIO3_SEL_S)
#define GPIO_ETM_TASK_GPIO3_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO3_SEL_S  25

/** GPIO_ETM_TASK_P1_CFG_REG register
 *  Etm Configure Register to decide which GPIO been chosen
 */
#define GPIO_ETM_TASK_P1_CFG_REG (DR_REG_GPIO_EXT_BASE + 0xa4)
/** GPIO_ETM_TASK_GPIO4_EN : R/W; bitpos: [0]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO4_EN    (BIT(0))
#define GPIO_ETM_TASK_GPIO4_EN_M  (GPIO_ETM_TASK_GPIO4_EN_V << GPIO_ETM_TASK_GPIO4_EN_S)
#define GPIO_ETM_TASK_GPIO4_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO4_EN_S  0
/** GPIO_ETM_TASK_GPIO4_SEL : R/W; bitpos: [3:1]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO4_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO4_SEL_M  (GPIO_ETM_TASK_GPIO4_SEL_V << GPIO_ETM_TASK_GPIO4_SEL_S)
#define GPIO_ETM_TASK_GPIO4_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO4_SEL_S  1
/** GPIO_ETM_TASK_GPIO5_EN : R/W; bitpos: [8]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO5_EN    (BIT(8))
#define GPIO_ETM_TASK_GPIO5_EN_M  (GPIO_ETM_TASK_GPIO5_EN_V << GPIO_ETM_TASK_GPIO5_EN_S)
#define GPIO_ETM_TASK_GPIO5_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO5_EN_S  8
/** GPIO_ETM_TASK_GPIO5_SEL : R/W; bitpos: [11:9]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO5_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO5_SEL_M  (GPIO_ETM_TASK_GPIO5_SEL_V << GPIO_ETM_TASK_GPIO5_SEL_S)
#define GPIO_ETM_TASK_GPIO5_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO5_SEL_S  9
/** GPIO_ETM_TASK_GPIO6_EN : R/W; bitpos: [16]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO6_EN    (BIT(16))
#define GPIO_ETM_TASK_GPIO6_EN_M  (GPIO_ETM_TASK_GPIO6_EN_V << GPIO_ETM_TASK_GPIO6_EN_S)
#define GPIO_ETM_TASK_GPIO6_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO6_EN_S  16
/** GPIO_ETM_TASK_GPIO6_SEL : R/W; bitpos: [19:17]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO6_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO6_SEL_M  (GPIO_ETM_TASK_GPIO6_SEL_V << GPIO_ETM_TASK_GPIO6_SEL_S)
#define GPIO_ETM_TASK_GPIO6_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO6_SEL_S  17
/** GPIO_ETM_TASK_GPIO7_EN : R/W; bitpos: [24]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO7_EN    (BIT(24))
#define GPIO_ETM_TASK_GPIO7_EN_M  (GPIO_ETM_TASK_GPIO7_EN_V << GPIO_ETM_TASK_GPIO7_EN_S)
#define GPIO_ETM_TASK_GPIO7_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO7_EN_S  24
/** GPIO_ETM_TASK_GPIO7_SEL : R/W; bitpos: [27:25]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO7_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO7_SEL_M  (GPIO_ETM_TASK_GPIO7_SEL_V << GPIO_ETM_TASK_GPIO7_SEL_S)
#define GPIO_ETM_TASK_GPIO7_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO7_SEL_S  25

/** GPIO_ETM_TASK_P2_CFG_REG register
 *  Etm Configure Register to decide which GPIO been chosen
 */
#define GPIO_ETM_TASK_P2_CFG_REG (DR_REG_GPIO_EXT_BASE + 0xa8)
/** GPIO_ETM_TASK_GPIO8_EN : R/W; bitpos: [0]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO8_EN    (BIT(0))
#define GPIO_ETM_TASK_GPIO8_EN_M  (GPIO_ETM_TASK_GPIO8_EN_V << GPIO_ETM_TASK_GPIO8_EN_S)
#define GPIO_ETM_TASK_GPIO8_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO8_EN_S  0
/** GPIO_ETM_TASK_GPIO8_SEL : R/W; bitpos: [3:1]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO8_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO8_SEL_M  (GPIO_ETM_TASK_GPIO8_SEL_V << GPIO_ETM_TASK_GPIO8_SEL_S)
#define GPIO_ETM_TASK_GPIO8_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO8_SEL_S  1
/** GPIO_ETM_TASK_GPIO9_EN : R/W; bitpos: [8]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO9_EN    (BIT(8))
#define GPIO_ETM_TASK_GPIO9_EN_M  (GPIO_ETM_TASK_GPIO9_EN_V << GPIO_ETM_TASK_GPIO9_EN_S)
#define GPIO_ETM_TASK_GPIO9_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO9_EN_S  8
/** GPIO_ETM_TASK_GPIO9_SEL : R/W; bitpos: [11:9]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO9_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO9_SEL_M  (GPIO_ETM_TASK_GPIO9_SEL_V << GPIO_ETM_TASK_GPIO9_SEL_S)
#define GPIO_ETM_TASK_GPIO9_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO9_SEL_S  9
/** GPIO_ETM_TASK_GPIO10_EN : R/W; bitpos: [16]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO10_EN    (BIT(16))
#define GPIO_ETM_TASK_GPIO10_EN_M  (GPIO_ETM_TASK_GPIO10_EN_V << GPIO_ETM_TASK_GPIO10_EN_S)
#define GPIO_ETM_TASK_GPIO10_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO10_EN_S  16
/** GPIO_ETM_TASK_GPIO10_SEL : R/W; bitpos: [19:17]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO10_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO10_SEL_M  (GPIO_ETM_TASK_GPIO10_SEL_V << GPIO_ETM_TASK_GPIO10_SEL_S)
#define GPIO_ETM_TASK_GPIO10_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO10_SEL_S  17
/** GPIO_ETM_TASK_GPIO11_EN : R/W; bitpos: [24]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO11_EN    (BIT(24))
#define GPIO_ETM_TASK_GPIO11_EN_M  (GPIO_ETM_TASK_GPIO11_EN_V << GPIO_ETM_TASK_GPIO11_EN_S)
#define GPIO_ETM_TASK_GPIO11_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO11_EN_S  24
/** GPIO_ETM_TASK_GPIO11_SEL : R/W; bitpos: [27:25]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO11_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO11_SEL_M  (GPIO_ETM_TASK_GPIO11_SEL_V << GPIO_ETM_TASK_GPIO11_SEL_S)
#define GPIO_ETM_TASK_GPIO11_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO11_SEL_S  25

/** GPIO_ETM_TASK_P3_CFG_REG register
 *  Etm Configure Register to decide which GPIO been chosen
 */
#define GPIO_ETM_TASK_P3_CFG_REG (DR_REG_GPIO_EXT_BASE + 0xac)
/** GPIO_ETM_TASK_GPIO12_EN : R/W; bitpos: [0]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO12_EN    (BIT(0))
#define GPIO_ETM_TASK_GPIO12_EN_M  (GPIO_ETM_TASK_GPIO12_EN_V << GPIO_ETM_TASK_GPIO12_EN_S)
#define GPIO_ETM_TASK_GPIO12_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO12_EN_S  0
/** GPIO_ETM_TASK_GPIO12_SEL : R/W; bitpos: [3:1]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO12_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO12_SEL_M  (GPIO_ETM_TASK_GPIO12_SEL_V << GPIO_ETM_TASK_GPIO12_SEL_S)
#define GPIO_ETM_TASK_GPIO12_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO12_SEL_S  1
/** GPIO_ETM_TASK_GPIO13_EN : R/W; bitpos: [8]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO13_EN    (BIT(8))
#define GPIO_ETM_TASK_GPIO13_EN_M  (GPIO_ETM_TASK_GPIO13_EN_V << GPIO_ETM_TASK_GPIO13_EN_S)
#define GPIO_ETM_TASK_GPIO13_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO13_EN_S  8
/** GPIO_ETM_TASK_GPIO13_SEL : R/W; bitpos: [11:9]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO13_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO13_SEL_M  (GPIO_ETM_TASK_GPIO13_SEL_V << GPIO_ETM_TASK_GPIO13_SEL_S)
#define GPIO_ETM_TASK_GPIO13_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO13_SEL_S  9
/** GPIO_ETM_TASK_GPIO14_EN : R/W; bitpos: [16]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO14_EN    (BIT(16))
#define GPIO_ETM_TASK_GPIO14_EN_M  (GPIO_ETM_TASK_GPIO14_EN_V << GPIO_ETM_TASK_GPIO14_EN_S)
#define GPIO_ETM_TASK_GPIO14_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO14_EN_S  16
/** GPIO_ETM_TASK_GPIO14_SEL : R/W; bitpos: [19:17]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO14_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO14_SEL_M  (GPIO_ETM_TASK_GPIO14_SEL_V << GPIO_ETM_TASK_GPIO14_SEL_S)
#define GPIO_ETM_TASK_GPIO14_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO14_SEL_S  17
/** GPIO_ETM_TASK_GPIO15_EN : R/W; bitpos: [24]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO15_EN    (BIT(24))
#define GPIO_ETM_TASK_GPIO15_EN_M  (GPIO_ETM_TASK_GPIO15_EN_V << GPIO_ETM_TASK_GPIO15_EN_S)
#define GPIO_ETM_TASK_GPIO15_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO15_EN_S  24
/** GPIO_ETM_TASK_GPIO15_SEL : R/W; bitpos: [27:25]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO15_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO15_SEL_M  (GPIO_ETM_TASK_GPIO15_SEL_V << GPIO_ETM_TASK_GPIO15_SEL_S)
#define GPIO_ETM_TASK_GPIO15_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO15_SEL_S  25

/** GPIO_ETM_TASK_P4_CFG_REG register
 *  Etm Configure Register to decide which GPIO been chosen
 */
#define GPIO_ETM_TASK_P4_CFG_REG (DR_REG_GPIO_EXT_BASE + 0xb0)
/** GPIO_ETM_TASK_GPIO16_EN : R/W; bitpos: [0]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO16_EN    (BIT(0))
#define GPIO_ETM_TASK_GPIO16_EN_M  (GPIO_ETM_TASK_GPIO16_EN_V << GPIO_ETM_TASK_GPIO16_EN_S)
#define GPIO_ETM_TASK_GPIO16_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO16_EN_S  0
/** GPIO_ETM_TASK_GPIO16_SEL : R/W; bitpos: [3:1]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO16_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO16_SEL_M  (GPIO_ETM_TASK_GPIO16_SEL_V << GPIO_ETM_TASK_GPIO16_SEL_S)
#define GPIO_ETM_TASK_GPIO16_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO16_SEL_S  1
/** GPIO_ETM_TASK_GPIO17_EN : R/W; bitpos: [8]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO17_EN    (BIT(8))
#define GPIO_ETM_TASK_GPIO17_EN_M  (GPIO_ETM_TASK_GPIO17_EN_V << GPIO_ETM_TASK_GPIO17_EN_S)
#define GPIO_ETM_TASK_GPIO17_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO17_EN_S  8
/** GPIO_ETM_TASK_GPIO17_SEL : R/W; bitpos: [11:9]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO17_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO17_SEL_M  (GPIO_ETM_TASK_GPIO17_SEL_V << GPIO_ETM_TASK_GPIO17_SEL_S)
#define GPIO_ETM_TASK_GPIO17_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO17_SEL_S  9
/** GPIO_ETM_TASK_GPIO18_EN : R/W; bitpos: [16]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO18_EN    (BIT(16))
#define GPIO_ETM_TASK_GPIO18_EN_M  (GPIO_ETM_TASK_GPIO18_EN_V << GPIO_ETM_TASK_GPIO18_EN_S)
#define GPIO_ETM_TASK_GPIO18_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO18_EN_S  16
/** GPIO_ETM_TASK_GPIO18_SEL : R/W; bitpos: [19:17]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO18_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO18_SEL_M  (GPIO_ETM_TASK_GPIO18_SEL_V << GPIO_ETM_TASK_GPIO18_SEL_S)
#define GPIO_ETM_TASK_GPIO18_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO18_SEL_S  17
/** GPIO_ETM_TASK_GPIO19_EN : R/W; bitpos: [24]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO19_EN    (BIT(24))
#define GPIO_ETM_TASK_GPIO19_EN_M  (GPIO_ETM_TASK_GPIO19_EN_V << GPIO_ETM_TASK_GPIO19_EN_S)
#define GPIO_ETM_TASK_GPIO19_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO19_EN_S  24
/** GPIO_ETM_TASK_GPIO19_SEL : R/W; bitpos: [27:25]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO19_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO19_SEL_M  (GPIO_ETM_TASK_GPIO19_SEL_V << GPIO_ETM_TASK_GPIO19_SEL_S)
#define GPIO_ETM_TASK_GPIO19_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO19_SEL_S  25

/** GPIO_ETM_TASK_P5_CFG_REG register
 *  Etm Configure Register to decide which GPIO been chosen
 */
#define GPIO_ETM_TASK_P5_CFG_REG (DR_REG_GPIO_EXT_BASE + 0xb4)
/** GPIO_ETM_TASK_GPIO20_EN : R/W; bitpos: [0]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO20_EN    (BIT(0))
#define GPIO_ETM_TASK_GPIO20_EN_M  (GPIO_ETM_TASK_GPIO20_EN_V << GPIO_ETM_TASK_GPIO20_EN_S)
#define GPIO_ETM_TASK_GPIO20_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO20_EN_S  0
/** GPIO_ETM_TASK_GPIO20_SEL : R/W; bitpos: [3:1]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO20_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO20_SEL_M  (GPIO_ETM_TASK_GPIO20_SEL_V << GPIO_ETM_TASK_GPIO20_SEL_S)
#define GPIO_ETM_TASK_GPIO20_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO20_SEL_S  1
/** GPIO_ETM_TASK_GPIO21_EN : R/W; bitpos: [8]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO21_EN    (BIT(8))
#define GPIO_ETM_TASK_GPIO21_EN_M  (GPIO_ETM_TASK_GPIO21_EN_V << GPIO_ETM_TASK_GPIO21_EN_S)
#define GPIO_ETM_TASK_GPIO21_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO21_EN_S  8
/** GPIO_ETM_TASK_GPIO21_SEL : R/W; bitpos: [11:9]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO21_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO21_SEL_M  (GPIO_ETM_TASK_GPIO21_SEL_V << GPIO_ETM_TASK_GPIO21_SEL_S)
#define GPIO_ETM_TASK_GPIO21_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO21_SEL_S  9
/** GPIO_ETM_TASK_GPIO22_EN : R/W; bitpos: [16]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO22_EN    (BIT(16))
#define GPIO_ETM_TASK_GPIO22_EN_M  (GPIO_ETM_TASK_GPIO22_EN_V << GPIO_ETM_TASK_GPIO22_EN_S)
#define GPIO_ETM_TASK_GPIO22_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO22_EN_S  16
/** GPIO_ETM_TASK_GPIO22_SEL : R/W; bitpos: [19:17]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO22_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO22_SEL_M  (GPIO_ETM_TASK_GPIO22_SEL_V << GPIO_ETM_TASK_GPIO22_SEL_S)
#define GPIO_ETM_TASK_GPIO22_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO22_SEL_S  17
/** GPIO_ETM_TASK_GPIO23_EN : R/W; bitpos: [24]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO23_EN    (BIT(24))
#define GPIO_ETM_TASK_GPIO23_EN_M  (GPIO_ETM_TASK_GPIO23_EN_V << GPIO_ETM_TASK_GPIO23_EN_S)
#define GPIO_ETM_TASK_GPIO23_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO23_EN_S  24
/** GPIO_ETM_TASK_GPIO23_SEL : R/W; bitpos: [27:25]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO23_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO23_SEL_M  (GPIO_ETM_TASK_GPIO23_SEL_V << GPIO_ETM_TASK_GPIO23_SEL_S)
#define GPIO_ETM_TASK_GPIO23_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO23_SEL_S  25

/** GPIO_ETM_TASK_P6_CFG_REG register
 *  Etm Configure Register to decide which GPIO been chosen
 */
#define GPIO_ETM_TASK_P6_CFG_REG (DR_REG_GPIO_EXT_BASE + 0xb8)
/** GPIO_ETM_TASK_GPIO24_EN : R/W; bitpos: [0]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO24_EN    (BIT(0))
#define GPIO_ETM_TASK_GPIO24_EN_M  (GPIO_ETM_TASK_GPIO24_EN_V << GPIO_ETM_TASK_GPIO24_EN_S)
#define GPIO_ETM_TASK_GPIO24_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO24_EN_S  0
/** GPIO_ETM_TASK_GPIO24_SEL : R/W; bitpos: [3:1]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO24_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO24_SEL_M  (GPIO_ETM_TASK_GPIO24_SEL_V << GPIO_ETM_TASK_GPIO24_SEL_S)
#define GPIO_ETM_TASK_GPIO24_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO24_SEL_S  1
/** GPIO_ETM_TASK_GPIO25_EN : R/W; bitpos: [8]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO25_EN    (BIT(8))
#define GPIO_ETM_TASK_GPIO25_EN_M  (GPIO_ETM_TASK_GPIO25_EN_V << GPIO_ETM_TASK_GPIO25_EN_S)
#define GPIO_ETM_TASK_GPIO25_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO25_EN_S  8
/** GPIO_ETM_TASK_GPIO25_SEL : R/W; bitpos: [11:9]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO25_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO25_SEL_M  (GPIO_ETM_TASK_GPIO25_SEL_V << GPIO_ETM_TASK_GPIO25_SEL_S)
#define GPIO_ETM_TASK_GPIO25_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO25_SEL_S  9
/** GPIO_ETM_TASK_GPIO26_EN : R/W; bitpos: [16]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO26_EN    (BIT(16))
#define GPIO_ETM_TASK_GPIO26_EN_M  (GPIO_ETM_TASK_GPIO26_EN_V << GPIO_ETM_TASK_GPIO26_EN_S)
#define GPIO_ETM_TASK_GPIO26_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO26_EN_S  16
/** GPIO_ETM_TASK_GPIO26_SEL : R/W; bitpos: [19:17]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO26_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO26_SEL_M  (GPIO_ETM_TASK_GPIO26_SEL_V << GPIO_ETM_TASK_GPIO26_SEL_S)
#define GPIO_ETM_TASK_GPIO26_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO26_SEL_S  17
/** GPIO_ETM_TASK_GPIO27_EN : R/W; bitpos: [24]; default: 0;
 *  Enable bit of GPIO response etm task.
 */
#define GPIO_ETM_TASK_GPIO27_EN    (BIT(24))
#define GPIO_ETM_TASK_GPIO27_EN_M  (GPIO_ETM_TASK_GPIO27_EN_V << GPIO_ETM_TASK_GPIO27_EN_S)
#define GPIO_ETM_TASK_GPIO27_EN_V  0x00000001U
#define GPIO_ETM_TASK_GPIO27_EN_S  24
/** GPIO_ETM_TASK_GPIO27_SEL : R/W; bitpos: [27:25]; default: 0;
 *  GPIO choose a etm task channel.
 */
#define GPIO_ETM_TASK_GPIO27_SEL    0x00000007U
#define GPIO_ETM_TASK_GPIO27_SEL_M  (GPIO_ETM_TASK_GPIO27_SEL_V << GPIO_ETM_TASK_GPIO27_SEL_S)
#define GPIO_ETM_TASK_GPIO27_SEL_V  0x00000007U
#define GPIO_ETM_TASK_GPIO27_SEL_S  25

/** GPIO_INT_RAW_REG register
 *  GPIO interrupt raw register
 */
#define GPIO_INT_RAW_REG (DR_REG_GPIO_EXT_BASE + 0xe0)
/** GPIO_PAD_COMP_INT_RAW : RO/WTC/SS; bitpos: [0]; default: 0;
 *  Pad compare raw interrupt
 */
#define GPIO_PAD_COMP_INT_RAW    (BIT(0))
#define GPIO_PAD_COMP_INT_RAW_M  (GPIO_PAD_COMP_INT_RAW_V << GPIO_PAD_COMP_INT_RAW_S)
#define GPIO_PAD_COMP_INT_RAW_V  0x00000001U
#define GPIO_PAD_COMP_INT_RAW_S  0

/** GPIO_INT_ST_REG register
 *  GPIO interrupt masked register
 */
#define GPIO_INT_ST_REG (DR_REG_GPIO_EXT_BASE + 0xe4)
/** GPIO_PAD_COMP_INT_ST : RO; bitpos: [0]; default: 0;
 *  Pad compare masked interrupt
 */
#define GPIO_PAD_COMP_INT_ST    (BIT(0))
#define GPIO_PAD_COMP_INT_ST_M  (GPIO_PAD_COMP_INT_ST_V << GPIO_PAD_COMP_INT_ST_S)
#define GPIO_PAD_COMP_INT_ST_V  0x00000001U
#define GPIO_PAD_COMP_INT_ST_S  0

/** GPIO_INT_ENA_REG register
 *  GPIO interrupt enable register
 */
#define GPIO_INT_ENA_REG (DR_REG_GPIO_EXT_BASE + 0xe8)
/** GPIO_PAD_COMP_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Pad compare interrupt enable
 */
#define GPIO_PAD_COMP_INT_ENA    (BIT(0))
#define GPIO_PAD_COMP_INT_ENA_M  (GPIO_PAD_COMP_INT_ENA_V << GPIO_PAD_COMP_INT_ENA_S)
#define GPIO_PAD_COMP_INT_ENA_V  0x00000001U
#define GPIO_PAD_COMP_INT_ENA_S  0

/** GPIO_INT_CLR_REG register
 *  GPIO interrupt clear register
 */
#define GPIO_INT_CLR_REG (DR_REG_GPIO_EXT_BASE + 0xec)
/** GPIO_PAD_COMP_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Pad compare interrupt clear
 */
#define GPIO_PAD_COMP_INT_CLR    (BIT(0))
#define GPIO_PAD_COMP_INT_CLR_M  (GPIO_PAD_COMP_INT_CLR_V << GPIO_PAD_COMP_INT_CLR_S)
#define GPIO_PAD_COMP_INT_CLR_V  0x00000001U
#define GPIO_PAD_COMP_INT_CLR_S  0

/** GPIO_VERSION_REG register
 *  Version Control Register
 */
#define GPIO_VERSION_REG (DR_REG_GPIO_EXT_BASE + 0xfc)
/** GPIO_GPIO_SD_DATE : R/W; bitpos: [27:0]; default: 35684640;
 *  Version control register.
 */
#define GPIO_GPIO_SD_DATE    0x0FFFFFFFU
#define GPIO_GPIO_SD_DATE_M  (GPIO_GPIO_SD_DATE_V << GPIO_GPIO_SD_DATE_S)
#define GPIO_GPIO_SD_DATE_V  0x0FFFFFFFU
#define GPIO_GPIO_SD_DATE_S  0

#ifdef __cplusplus
}
#endif
