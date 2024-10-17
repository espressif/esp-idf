/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** GPIO_EXT_CLOCK_GATE_REG register
 *  Clock Gating Configure Register
 */
#define GPIO_EXT_CLOCK_GATE_REG (DR_REG_GPIO_EXT_BASE + 0x0)
/** GPIO_EXT_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Clock enable bit of configuration registers for sigma delta modulation.
 */
#define GPIO_EXT_CLK_EN    (BIT(0))
#define GPIO_EXT_CLK_EN_M  (GPIO_EXT_CLK_EN_V << GPIO_EXT_CLK_EN_S)
#define GPIO_EXT_CLK_EN_V  0x00000001U
#define GPIO_EXT_CLK_EN_S  0

/** GPIO_EXT_SIGMADELTA_MISC_REG register
 *  MISC Register
 */
#define GPIO_EXT_SIGMADELTA_MISC_REG (DR_REG_GPIO_EXT_BASE + 0x4)
/** GPIO_EXT_SIGMADELTA_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the clock for sigma delta modulation.\\
 *  0: Not enable\\
 *  1: Enable\\%\label{fielddesc:GPIOSDSPISWAP}- [GPIOSD_SPI_SWAP] Reserved.
 */
#define GPIO_EXT_SIGMADELTA_CLK_EN    (BIT(0))
#define GPIO_EXT_SIGMADELTA_CLK_EN_M  (GPIO_EXT_SIGMADELTA_CLK_EN_V << GPIO_EXT_SIGMADELTA_CLK_EN_S)
#define GPIO_EXT_SIGMADELTA_CLK_EN_V  0x00000001U
#define GPIO_EXT_SIGMADELTA_CLK_EN_S  0

/** GPIO_EXT_SIGMADELTA0_REG register
 *  Duty cycle configuration register for SDM channel 0
 */
#define GPIO_EXT_SIGMADELTA0_REG (DR_REG_GPIO_EXT_BASE + 0x8)
/** GPIO_EXT_SD0_IN : R/W; bitpos: [7:0]; default: 0;
 *  Configures the duty cycle of sigma delta modulation output. \\
 */
#define GPIO_EXT_SD0_IN    0x000000FFU
#define GPIO_EXT_SD0_IN_M  (GPIO_EXT_SD0_IN_V << GPIO_EXT_SD0_IN_S)
#define GPIO_EXT_SD0_IN_V  0x000000FFU
#define GPIO_EXT_SD0_IN_S  0
/** GPIO_EXT_SD0_PRESCALE : R/W; bitpos: [15:8]; default: 255;
 *  Configures the divider value to divide IO MUX operating clock. \\
 */
#define GPIO_EXT_SD0_PRESCALE    0x000000FFU
#define GPIO_EXT_SD0_PRESCALE_M  (GPIO_EXT_SD0_PRESCALE_V << GPIO_EXT_SD0_PRESCALE_S)
#define GPIO_EXT_SD0_PRESCALE_V  0x000000FFU
#define GPIO_EXT_SD0_PRESCALE_S  8

/** GPIO_EXT_SIGMADELTA1_REG register
 *  Duty cycle configuration register for SDM channel 1
 */
#define GPIO_EXT_SIGMADELTA1_REG (DR_REG_GPIO_EXT_BASE + 0xc)
/** GPIO_EXT_SD1_IN : R/W; bitpos: [7:0]; default: 0;
 *  Configures the duty cycle of sigma delta modulation output. \\
 */
#define GPIO_EXT_SD1_IN    0x000000FFU
#define GPIO_EXT_SD1_IN_M  (GPIO_EXT_SD1_IN_V << GPIO_EXT_SD1_IN_S)
#define GPIO_EXT_SD1_IN_V  0x000000FFU
#define GPIO_EXT_SD1_IN_S  0
/** GPIO_EXT_SD1_PRESCALE : R/W; bitpos: [15:8]; default: 255;
 *  Configures the divider value to divide IO MUX operating clock. \\
 */
#define GPIO_EXT_SD1_PRESCALE    0x000000FFU
#define GPIO_EXT_SD1_PRESCALE_M  (GPIO_EXT_SD1_PRESCALE_V << GPIO_EXT_SD1_PRESCALE_S)
#define GPIO_EXT_SD1_PRESCALE_V  0x000000FFU
#define GPIO_EXT_SD1_PRESCALE_S  8

/** GPIO_EXT_SIGMADELTA2_REG register
 *  Duty cycle configuration register for SDM channel 2
 */
#define GPIO_EXT_SIGMADELTA2_REG (DR_REG_GPIO_EXT_BASE + 0x10)
/** GPIO_EXT_SD2_IN : R/W; bitpos: [7:0]; default: 0;
 *  Configures the duty cycle of sigma delta modulation output. \\
 */
#define GPIO_EXT_SD2_IN    0x000000FFU
#define GPIO_EXT_SD2_IN_M  (GPIO_EXT_SD2_IN_V << GPIO_EXT_SD2_IN_S)
#define GPIO_EXT_SD2_IN_V  0x000000FFU
#define GPIO_EXT_SD2_IN_S  0
/** GPIO_EXT_SD2_PRESCALE : R/W; bitpos: [15:8]; default: 255;
 *  Configures the divider value to divide IO MUX operating clock. \\
 */
#define GPIO_EXT_SD2_PRESCALE    0x000000FFU
#define GPIO_EXT_SD2_PRESCALE_M  (GPIO_EXT_SD2_PRESCALE_V << GPIO_EXT_SD2_PRESCALE_S)
#define GPIO_EXT_SD2_PRESCALE_V  0x000000FFU
#define GPIO_EXT_SD2_PRESCALE_S  8

/** GPIO_EXT_SIGMADELTA3_REG register
 *  Duty cycle configuration register for SDM channel 3
 */
#define GPIO_EXT_SIGMADELTA3_REG (DR_REG_GPIO_EXT_BASE + 0x14)
/** GPIO_EXT_SD3_IN : R/W; bitpos: [7:0]; default: 0;
 *  Configures the duty cycle of sigma delta modulation output. \\
 */
#define GPIO_EXT_SD3_IN    0x000000FFU
#define GPIO_EXT_SD3_IN_M  (GPIO_EXT_SD3_IN_V << GPIO_EXT_SD3_IN_S)
#define GPIO_EXT_SD3_IN_V  0x000000FFU
#define GPIO_EXT_SD3_IN_S  0
/** GPIO_EXT_SD3_PRESCALE : R/W; bitpos: [15:8]; default: 255;
 *  Configures the divider value to divide IO MUX operating clock. \\
 */
#define GPIO_EXT_SD3_PRESCALE    0x000000FFU
#define GPIO_EXT_SD3_PRESCALE_M  (GPIO_EXT_SD3_PRESCALE_V << GPIO_EXT_SD3_PRESCALE_S)
#define GPIO_EXT_SD3_PRESCALE_V  0x000000FFU
#define GPIO_EXT_SD3_PRESCALE_S  8

/** GPIO_EXT_PAD_COMP_CONFIG_0_REG register
 *  Configuration register for zero-crossing detection
 */
#define GPIO_EXT_PAD_COMP_CONFIG_0_REG (DR_REG_GPIO_EXT_BASE + 0x58)
/** GPIO_EXT_XPD_COMP_0 : R/W; bitpos: [0]; default: 0;
 *  Configures whether to enable the function of analog PAD voltage comparator.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define GPIO_EXT_XPD_COMP_0    (BIT(0))
#define GPIO_EXT_XPD_COMP_0_M  (GPIO_EXT_XPD_COMP_0_V << GPIO_EXT_XPD_COMP_0_S)
#define GPIO_EXT_XPD_COMP_0_V  0x00000001U
#define GPIO_EXT_XPD_COMP_0_S  0
/** GPIO_EXT_MODE_COMP_0 : R/W; bitpos: [1]; default: 0;
 *  Configures the reference voltage for analog PAD voltage comparator.. \\
 *  0: Reference voltage is the internal reference voltage, meanwhile GPIO8 PAD can be
 *  used as a regular GPIO\\
 *  1: Reference voltage is the voltage on the GPIO8 PAD\\
 */
#define GPIO_EXT_MODE_COMP_0    (BIT(1))
#define GPIO_EXT_MODE_COMP_0_M  (GPIO_EXT_MODE_COMP_0_V << GPIO_EXT_MODE_COMP_0_S)
#define GPIO_EXT_MODE_COMP_0_V  0x00000001U
#define GPIO_EXT_MODE_COMP_0_S  1
/** GPIO_EXT_DREF_COMP_0 : R/W; bitpos: [4:2]; default: 0;
 *  Configures the internal reference voltage for analog PAD voltage coparator. \\
 *  0: Internal reference voltage is 0 * VDDPST1\\
 *  1: Internal reference voltage is 0.1 * VDDPST1\\
 *  …...\\
 *  6: Internal reference voltage is 0.6 * VDDPST1\\
 *  7: Internal reference voltage is 0.7 * VDDPST1\\
 */
#define GPIO_EXT_DREF_COMP_0    0x00000007U
#define GPIO_EXT_DREF_COMP_0_M  (GPIO_EXT_DREF_COMP_0_V << GPIO_EXT_DREF_COMP_0_S)
#define GPIO_EXT_DREF_COMP_0_V  0x00000007U
#define GPIO_EXT_DREF_COMP_0_S  2

/** GPIO_EXT_PAD_COMP_FILTER_0_REG register
 *  Configuration register for interrupt source mask period of zero-crossing detection
 */
#define GPIO_EXT_PAD_COMP_FILTER_0_REG (DR_REG_GPIO_EXT_BASE + 0x5c)
/** GPIO_EXT_ZERO_DET_FILTER_CNT_0 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the period of masking new interrupt source foe analog PAD voltage
 *  comparator.\\
 *  Measurement unit: IO MUX operating clock cycle\\
 */
#define GPIO_EXT_ZERO_DET_FILTER_CNT_0    0xFFFFFFFFU
#define GPIO_EXT_ZERO_DET_FILTER_CNT_0_M  (GPIO_EXT_ZERO_DET_FILTER_CNT_0_V << GPIO_EXT_ZERO_DET_FILTER_CNT_0_S)
#define GPIO_EXT_ZERO_DET_FILTER_CNT_0_V  0xFFFFFFFFU
#define GPIO_EXT_ZERO_DET_FILTER_CNT_0_S  0

/** GPIO_EXT_GLITCH_FILTER_CH0_REG register
 *  Glitch Filter Configure Register of Channel0
 */
#define GPIO_EXT_GLITCH_FILTER_CH0_REG (DR_REG_GPIO_EXT_BASE + 0xd8)
/** GPIO_EXT_FILTER_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable channel 0 of Glitch Filter.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_FILTER_CH0_EN    (BIT(0))
#define GPIO_EXT_FILTER_CH0_EN_M  (GPIO_EXT_FILTER_CH0_EN_V << GPIO_EXT_FILTER_CH0_EN_S)
#define GPIO_EXT_FILTER_CH0_EN_V  0x00000001U
#define GPIO_EXT_FILTER_CH0_EN_S  0
/** GPIO_EXT_FILTER_CH0_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Configures to select the input GPIO for Glitch Filter. \\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  29 ~ 63: Reserved\\
 */
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM    0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_M  (GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V << GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S)
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S  1
/** GPIO_EXT_FILTER_CH0_WINDOW_THRES : R/W; bitpos: [13:8]; default: 0;
 *  Configures the window threshold for Glitch Filter. The window threshold should be
 *  less than or equal to GPIOSD_FILTER_CH0_WINDOW_WIDTH.\\  %see DOC-4768\\
 *  Measurement unit: IO MUX operating clock cycle\\
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_M  (GPIO_EXT_FILTER_CH0_WINDOW_THRES_V << GPIO_EXT_FILTER_CH0_WINDOW_THRES_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_S  8
/** GPIO_EXT_FILTER_CH0_WINDOW_WIDTH : R/W; bitpos: [19:14]; default: 0;
 *  Configures the window width for Glitch Filter. The effective value of window width
 *  is 0 ~ 63. \\
 *  Measurement unit: IO MUX operating clock cycle\\
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_M  (GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V << GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S  14

/** GPIO_EXT_GLITCH_FILTER_CH1_REG register
 *  Glitch Filter Configure Register of Channel1
 */
#define GPIO_EXT_GLITCH_FILTER_CH1_REG (DR_REG_GPIO_EXT_BASE + 0xdc)
/** GPIO_EXT_FILTER_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable channel 1 of Glitch Filter.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_FILTER_CH0_EN    (BIT(0))
#define GPIO_EXT_FILTER_CH0_EN_M  (GPIO_EXT_FILTER_CH0_EN_V << GPIO_EXT_FILTER_CH0_EN_S)
#define GPIO_EXT_FILTER_CH0_EN_V  0x00000001U
#define GPIO_EXT_FILTER_CH0_EN_S  0
/** GPIO_EXT_FILTER_CH0_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Configures to select the input GPIO for Glitch Filter. \\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  29 ~ 63: Reserved\\
 */
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM    0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_M  (GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V << GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S)
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S  1
/** GPIO_EXT_FILTER_CH0_WINDOW_THRES : R/W; bitpos: [13:8]; default: 0;
 *  Configures the window threshold for Glitch Filter. The window threshold should be
 *  less than or equal to GPIOSD_FILTER_CH1_WINDOW_WIDTH.\\  %see DOC-4768\\
 *  Measurement unit: IO MUX operating clock cycle\\
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_M  (GPIO_EXT_FILTER_CH0_WINDOW_THRES_V << GPIO_EXT_FILTER_CH0_WINDOW_THRES_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_S  8
/** GPIO_EXT_FILTER_CH0_WINDOW_WIDTH : R/W; bitpos: [19:14]; default: 0;
 *  Configures the window width for Glitch Filter. The effective value of window width
 *  is 0 ~ 63. \\
 *  Measurement unit: IO MUX operating clock cycle\\
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_M  (GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V << GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S  14

/** GPIO_EXT_GLITCH_FILTER_CH2_REG register
 *  Glitch Filter Configure Register of Channel2
 */
#define GPIO_EXT_GLITCH_FILTER_CH2_REG (DR_REG_GPIO_EXT_BASE + 0xe0)
/** GPIO_EXT_FILTER_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable channel 2 of Glitch Filter.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_FILTER_CH0_EN    (BIT(0))
#define GPIO_EXT_FILTER_CH0_EN_M  (GPIO_EXT_FILTER_CH0_EN_V << GPIO_EXT_FILTER_CH0_EN_S)
#define GPIO_EXT_FILTER_CH0_EN_V  0x00000001U
#define GPIO_EXT_FILTER_CH0_EN_S  0
/** GPIO_EXT_FILTER_CH0_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Configures to select the input GPIO for Glitch Filter. \\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  29 ~ 63: Reserved\\
 */
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM    0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_M  (GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V << GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S)
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S  1
/** GPIO_EXT_FILTER_CH0_WINDOW_THRES : R/W; bitpos: [13:8]; default: 0;
 *  Configures the window threshold for Glitch Filter. The window threshold should be
 *  less than or equal to GPIOSD_FILTER_CH2_WINDOW_WIDTH.\\  %see DOC-4768\\
 *  Measurement unit: IO MUX operating clock cycle\\
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_M  (GPIO_EXT_FILTER_CH0_WINDOW_THRES_V << GPIO_EXT_FILTER_CH0_WINDOW_THRES_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_S  8
/** GPIO_EXT_FILTER_CH0_WINDOW_WIDTH : R/W; bitpos: [19:14]; default: 0;
 *  Configures the window width for Glitch Filter. The effective value of window width
 *  is 0 ~ 63. \\
 *  Measurement unit: IO MUX operating clock cycle\\
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_M  (GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V << GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S  14

/** GPIO_EXT_GLITCH_FILTER_CH3_REG register
 *  Glitch Filter Configure Register of Channel3
 */
#define GPIO_EXT_GLITCH_FILTER_CH3_REG (DR_REG_GPIO_EXT_BASE + 0xe4)
/** GPIO_EXT_FILTER_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable channel 3 of Glitch Filter.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_FILTER_CH0_EN    (BIT(0))
#define GPIO_EXT_FILTER_CH0_EN_M  (GPIO_EXT_FILTER_CH0_EN_V << GPIO_EXT_FILTER_CH0_EN_S)
#define GPIO_EXT_FILTER_CH0_EN_V  0x00000001U
#define GPIO_EXT_FILTER_CH0_EN_S  0
/** GPIO_EXT_FILTER_CH0_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Configures to select the input GPIO for Glitch Filter. \\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  29 ~ 63: Reserved\\
 */
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM    0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_M  (GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V << GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S)
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S  1
/** GPIO_EXT_FILTER_CH0_WINDOW_THRES : R/W; bitpos: [13:8]; default: 0;
 *  Configures the window threshold for Glitch Filter. The window threshold should be
 *  less than or equal to GPIOSD_FILTER_CH3_WINDOW_WIDTH.\\  %see DOC-4768\\
 *  Measurement unit: IO MUX operating clock cycle\\
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_M  (GPIO_EXT_FILTER_CH0_WINDOW_THRES_V << GPIO_EXT_FILTER_CH0_WINDOW_THRES_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_S  8
/** GPIO_EXT_FILTER_CH0_WINDOW_WIDTH : R/W; bitpos: [19:14]; default: 0;
 *  Configures the window width for Glitch Filter. The effective value of window width
 *  is 0 ~ 63. \\
 *  Measurement unit: IO MUX operating clock cycle\\
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_M  (GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V << GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S  14

/** GPIO_EXT_GLITCH_FILTER_CH4_REG register
 *  Glitch Filter Configure Register of Channel4
 */
#define GPIO_EXT_GLITCH_FILTER_CH4_REG (DR_REG_GPIO_EXT_BASE + 0xe8)
/** GPIO_EXT_FILTER_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable channel 4 of Glitch Filter.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_FILTER_CH0_EN    (BIT(0))
#define GPIO_EXT_FILTER_CH0_EN_M  (GPIO_EXT_FILTER_CH0_EN_V << GPIO_EXT_FILTER_CH0_EN_S)
#define GPIO_EXT_FILTER_CH0_EN_V  0x00000001U
#define GPIO_EXT_FILTER_CH0_EN_S  0
/** GPIO_EXT_FILTER_CH0_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Configures to select the input GPIO for Glitch Filter. \\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  29 ~ 63: Reserved\\
 */
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM    0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_M  (GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V << GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S)
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S  1
/** GPIO_EXT_FILTER_CH0_WINDOW_THRES : R/W; bitpos: [13:8]; default: 0;
 *  Configures the window threshold for Glitch Filter. The window threshold should be
 *  less than or equal to GPIOSD_FILTER_CH4_WINDOW_WIDTH.\\  %see DOC-4768\\
 *  Measurement unit: IO MUX operating clock cycle\\
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_M  (GPIO_EXT_FILTER_CH0_WINDOW_THRES_V << GPIO_EXT_FILTER_CH0_WINDOW_THRES_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_S  8
/** GPIO_EXT_FILTER_CH0_WINDOW_WIDTH : R/W; bitpos: [19:14]; default: 0;
 *  Configures the window width for Glitch Filter. The effective value of window width
 *  is 0 ~ 63. \\
 *  Measurement unit: IO MUX operating clock cycle\\
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_M  (GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V << GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S  14

/** GPIO_EXT_GLITCH_FILTER_CH5_REG register
 *  Glitch Filter Configure Register of Channel5
 */
#define GPIO_EXT_GLITCH_FILTER_CH5_REG (DR_REG_GPIO_EXT_BASE + 0xec)
/** GPIO_EXT_FILTER_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable channel 5 of Glitch Filter.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_FILTER_CH0_EN    (BIT(0))
#define GPIO_EXT_FILTER_CH0_EN_M  (GPIO_EXT_FILTER_CH0_EN_V << GPIO_EXT_FILTER_CH0_EN_S)
#define GPIO_EXT_FILTER_CH0_EN_V  0x00000001U
#define GPIO_EXT_FILTER_CH0_EN_S  0
/** GPIO_EXT_FILTER_CH0_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Configures to select the input GPIO for Glitch Filter. \\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  29 ~ 63: Reserved\\
 */
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM    0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_M  (GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V << GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S)
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S  1
/** GPIO_EXT_FILTER_CH0_WINDOW_THRES : R/W; bitpos: [13:8]; default: 0;
 *  Configures the window threshold for Glitch Filter. The window threshold should be
 *  less than or equal to GPIOSD_FILTER_CH5_WINDOW_WIDTH.\\  %see DOC-4768\\
 *  Measurement unit: IO MUX operating clock cycle\\
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_M  (GPIO_EXT_FILTER_CH0_WINDOW_THRES_V << GPIO_EXT_FILTER_CH0_WINDOW_THRES_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_S  8
/** GPIO_EXT_FILTER_CH0_WINDOW_WIDTH : R/W; bitpos: [19:14]; default: 0;
 *  Configures the window width for Glitch Filter. The effective value of window width
 *  is 0 ~ 63. \\
 *  Measurement unit: IO MUX operating clock cycle\\
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_M  (GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V << GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S  14

/** GPIO_EXT_GLITCH_FILTER_CH6_REG register
 *  Glitch Filter Configure Register of Channel6
 */
#define GPIO_EXT_GLITCH_FILTER_CH6_REG (DR_REG_GPIO_EXT_BASE + 0xf0)
/** GPIO_EXT_FILTER_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable channel 6 of Glitch Filter.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_FILTER_CH0_EN    (BIT(0))
#define GPIO_EXT_FILTER_CH0_EN_M  (GPIO_EXT_FILTER_CH0_EN_V << GPIO_EXT_FILTER_CH0_EN_S)
#define GPIO_EXT_FILTER_CH0_EN_V  0x00000001U
#define GPIO_EXT_FILTER_CH0_EN_S  0
/** GPIO_EXT_FILTER_CH0_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Configures to select the input GPIO for Glitch Filter. \\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  29 ~ 63: Reserved\\
 */
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM    0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_M  (GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V << GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S)
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S  1
/** GPIO_EXT_FILTER_CH0_WINDOW_THRES : R/W; bitpos: [13:8]; default: 0;
 *  Configures the window threshold for Glitch Filter. The window threshold should be
 *  less than or equal to GPIOSD_FILTER_CH6_WINDOW_WIDTH.\\  %see DOC-4768\\
 *  Measurement unit: IO MUX operating clock cycle\\
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_M  (GPIO_EXT_FILTER_CH0_WINDOW_THRES_V << GPIO_EXT_FILTER_CH0_WINDOW_THRES_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_S  8
/** GPIO_EXT_FILTER_CH0_WINDOW_WIDTH : R/W; bitpos: [19:14]; default: 0;
 *  Configures the window width for Glitch Filter. The effective value of window width
 *  is 0 ~ 63. \\
 *  Measurement unit: IO MUX operating clock cycle\\
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_M  (GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V << GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S  14

/** GPIO_EXT_GLITCH_FILTER_CH7_REG register
 *  Glitch Filter Configure Register of Channel7
 */
#define GPIO_EXT_GLITCH_FILTER_CH7_REG (DR_REG_GPIO_EXT_BASE + 0xf4)
/** GPIO_EXT_FILTER_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable channel 7 of Glitch Filter.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_FILTER_CH0_EN    (BIT(0))
#define GPIO_EXT_FILTER_CH0_EN_M  (GPIO_EXT_FILTER_CH0_EN_V << GPIO_EXT_FILTER_CH0_EN_S)
#define GPIO_EXT_FILTER_CH0_EN_V  0x00000001U
#define GPIO_EXT_FILTER_CH0_EN_S  0
/** GPIO_EXT_FILTER_CH0_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Configures to select the input GPIO for Glitch Filter. \\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  29 ~ 63: Reserved\\
 */
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM    0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_M  (GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V << GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S)
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S  1
/** GPIO_EXT_FILTER_CH0_WINDOW_THRES : R/W; bitpos: [13:8]; default: 0;
 *  Configures the window threshold for Glitch Filter. The window threshold should be
 *  less than or equal to GPIOSD_FILTER_CH7_WINDOW_WIDTH.\\  %see DOC-4768\\
 *  Measurement unit: IO MUX operating clock cycle\\
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_M  (GPIO_EXT_FILTER_CH0_WINDOW_THRES_V << GPIO_EXT_FILTER_CH0_WINDOW_THRES_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_S  8
/** GPIO_EXT_FILTER_CH0_WINDOW_WIDTH : R/W; bitpos: [19:14]; default: 0;
 *  Configures the window width for Glitch Filter. The effective value of window width
 *  is 0 ~ 63. \\
 *  Measurement unit: IO MUX operating clock cycle\\
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_M  (GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V << GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S  14

/** GPIO_EXT_ETM_EVENT_CH0_CFG_REG register
 *  Etm Config register of Channel0
 */
#define GPIO_EXT_ETM_EVENT_CH0_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x118)
/** GPIO_EXT_ETM_CH0_EVENT_SEL : R/W; bitpos: [5:0]; default: 0;
 *  Configures to select GPIO for ETM event channel.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  29 ~ 63: Reserved\\
 */
#define GPIO_EXT_ETM_CH0_EVENT_SEL    0x0000003FU
#define GPIO_EXT_ETM_CH0_EVENT_SEL_M  (GPIO_EXT_ETM_CH0_EVENT_SEL_V << GPIO_EXT_ETM_CH0_EVENT_SEL_S)
#define GPIO_EXT_ETM_CH0_EVENT_SEL_V  0x0000003FU
#define GPIO_EXT_ETM_CH0_EVENT_SEL_S  0
/** GPIO_EXT_ETM_CH0_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable ETM event send.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_CH0_EVENT_EN    (BIT(7))
#define GPIO_EXT_ETM_CH0_EVENT_EN_M  (GPIO_EXT_ETM_CH0_EVENT_EN_V << GPIO_EXT_ETM_CH0_EVENT_EN_S)
#define GPIO_EXT_ETM_CH0_EVENT_EN_V  0x00000001U
#define GPIO_EXT_ETM_CH0_EVENT_EN_S  7

/** GPIO_EXT_ETM_EVENT_CH1_CFG_REG register
 *  Etm Config register of Channel1
 */
#define GPIO_EXT_ETM_EVENT_CH1_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x11c)
/** GPIO_EXT_ETM_CH0_EVENT_SEL : R/W; bitpos: [5:0]; default: 0;
 *  Configures to select GPIO for ETM event channel.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  29 ~ 63: Reserved\\
 */
#define GPIO_EXT_ETM_CH0_EVENT_SEL    0x0000003FU
#define GPIO_EXT_ETM_CH0_EVENT_SEL_M  (GPIO_EXT_ETM_CH0_EVENT_SEL_V << GPIO_EXT_ETM_CH0_EVENT_SEL_S)
#define GPIO_EXT_ETM_CH0_EVENT_SEL_V  0x0000003FU
#define GPIO_EXT_ETM_CH0_EVENT_SEL_S  0
/** GPIO_EXT_ETM_CH0_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable ETM event send.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_CH0_EVENT_EN    (BIT(7))
#define GPIO_EXT_ETM_CH0_EVENT_EN_M  (GPIO_EXT_ETM_CH0_EVENT_EN_V << GPIO_EXT_ETM_CH0_EVENT_EN_S)
#define GPIO_EXT_ETM_CH0_EVENT_EN_V  0x00000001U
#define GPIO_EXT_ETM_CH0_EVENT_EN_S  7

/** GPIO_EXT_ETM_EVENT_CH2_CFG_REG register
 *  Etm Config register of Channel2
 */
#define GPIO_EXT_ETM_EVENT_CH2_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x120)
/** GPIO_EXT_ETM_CH0_EVENT_SEL : R/W; bitpos: [5:0]; default: 0;
 *  Configures to select GPIO for ETM event channel.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  29 ~ 63: Reserved\\
 */
#define GPIO_EXT_ETM_CH0_EVENT_SEL    0x0000003FU
#define GPIO_EXT_ETM_CH0_EVENT_SEL_M  (GPIO_EXT_ETM_CH0_EVENT_SEL_V << GPIO_EXT_ETM_CH0_EVENT_SEL_S)
#define GPIO_EXT_ETM_CH0_EVENT_SEL_V  0x0000003FU
#define GPIO_EXT_ETM_CH0_EVENT_SEL_S  0
/** GPIO_EXT_ETM_CH0_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable ETM event send.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_CH0_EVENT_EN    (BIT(7))
#define GPIO_EXT_ETM_CH0_EVENT_EN_M  (GPIO_EXT_ETM_CH0_EVENT_EN_V << GPIO_EXT_ETM_CH0_EVENT_EN_S)
#define GPIO_EXT_ETM_CH0_EVENT_EN_V  0x00000001U
#define GPIO_EXT_ETM_CH0_EVENT_EN_S  7

/** GPIO_EXT_ETM_EVENT_CH3_CFG_REG register
 *  Etm Config register of Channel3
 */
#define GPIO_EXT_ETM_EVENT_CH3_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x124)
/** GPIO_EXT_ETM_CH0_EVENT_SEL : R/W; bitpos: [5:0]; default: 0;
 *  Configures to select GPIO for ETM event channel.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  29 ~ 63: Reserved\\
 */
#define GPIO_EXT_ETM_CH0_EVENT_SEL    0x0000003FU
#define GPIO_EXT_ETM_CH0_EVENT_SEL_M  (GPIO_EXT_ETM_CH0_EVENT_SEL_V << GPIO_EXT_ETM_CH0_EVENT_SEL_S)
#define GPIO_EXT_ETM_CH0_EVENT_SEL_V  0x0000003FU
#define GPIO_EXT_ETM_CH0_EVENT_SEL_S  0
/** GPIO_EXT_ETM_CH0_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable ETM event send.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_CH0_EVENT_EN    (BIT(7))
#define GPIO_EXT_ETM_CH0_EVENT_EN_M  (GPIO_EXT_ETM_CH0_EVENT_EN_V << GPIO_EXT_ETM_CH0_EVENT_EN_S)
#define GPIO_EXT_ETM_CH0_EVENT_EN_V  0x00000001U
#define GPIO_EXT_ETM_CH0_EVENT_EN_S  7

/** GPIO_EXT_ETM_EVENT_CH4_CFG_REG register
 *  Etm Config register of Channel4
 */
#define GPIO_EXT_ETM_EVENT_CH4_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x128)
/** GPIO_EXT_ETM_CH0_EVENT_SEL : R/W; bitpos: [5:0]; default: 0;
 *  Configures to select GPIO for ETM event channel.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  29 ~ 63: Reserved\\
 */
#define GPIO_EXT_ETM_CH0_EVENT_SEL    0x0000003FU
#define GPIO_EXT_ETM_CH0_EVENT_SEL_M  (GPIO_EXT_ETM_CH0_EVENT_SEL_V << GPIO_EXT_ETM_CH0_EVENT_SEL_S)
#define GPIO_EXT_ETM_CH0_EVENT_SEL_V  0x0000003FU
#define GPIO_EXT_ETM_CH0_EVENT_SEL_S  0
/** GPIO_EXT_ETM_CH0_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable ETM event send.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_CH0_EVENT_EN    (BIT(7))
#define GPIO_EXT_ETM_CH0_EVENT_EN_M  (GPIO_EXT_ETM_CH0_EVENT_EN_V << GPIO_EXT_ETM_CH0_EVENT_EN_S)
#define GPIO_EXT_ETM_CH0_EVENT_EN_V  0x00000001U
#define GPIO_EXT_ETM_CH0_EVENT_EN_S  7

/** GPIO_EXT_ETM_EVENT_CH5_CFG_REG register
 *  Etm Config register of Channel5
 */
#define GPIO_EXT_ETM_EVENT_CH5_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x12c)
/** GPIO_EXT_ETM_CH0_EVENT_SEL : R/W; bitpos: [5:0]; default: 0;
 *  Configures to select GPIO for ETM event channel.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  29 ~ 63: Reserved\\
 */
#define GPIO_EXT_ETM_CH0_EVENT_SEL    0x0000003FU
#define GPIO_EXT_ETM_CH0_EVENT_SEL_M  (GPIO_EXT_ETM_CH0_EVENT_SEL_V << GPIO_EXT_ETM_CH0_EVENT_SEL_S)
#define GPIO_EXT_ETM_CH0_EVENT_SEL_V  0x0000003FU
#define GPIO_EXT_ETM_CH0_EVENT_SEL_S  0
/** GPIO_EXT_ETM_CH0_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable ETM event send.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_CH0_EVENT_EN    (BIT(7))
#define GPIO_EXT_ETM_CH0_EVENT_EN_M  (GPIO_EXT_ETM_CH0_EVENT_EN_V << GPIO_EXT_ETM_CH0_EVENT_EN_S)
#define GPIO_EXT_ETM_CH0_EVENT_EN_V  0x00000001U
#define GPIO_EXT_ETM_CH0_EVENT_EN_S  7

/** GPIO_EXT_ETM_EVENT_CH6_CFG_REG register
 *  Etm Config register of Channel6
 */
#define GPIO_EXT_ETM_EVENT_CH6_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x130)
/** GPIO_EXT_ETM_CH0_EVENT_SEL : R/W; bitpos: [5:0]; default: 0;
 *  Configures to select GPIO for ETM event channel.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  29 ~ 63: Reserved\\
 */
#define GPIO_EXT_ETM_CH0_EVENT_SEL    0x0000003FU
#define GPIO_EXT_ETM_CH0_EVENT_SEL_M  (GPIO_EXT_ETM_CH0_EVENT_SEL_V << GPIO_EXT_ETM_CH0_EVENT_SEL_S)
#define GPIO_EXT_ETM_CH0_EVENT_SEL_V  0x0000003FU
#define GPIO_EXT_ETM_CH0_EVENT_SEL_S  0
/** GPIO_EXT_ETM_CH0_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable ETM event send.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_CH0_EVENT_EN    (BIT(7))
#define GPIO_EXT_ETM_CH0_EVENT_EN_M  (GPIO_EXT_ETM_CH0_EVENT_EN_V << GPIO_EXT_ETM_CH0_EVENT_EN_S)
#define GPIO_EXT_ETM_CH0_EVENT_EN_V  0x00000001U
#define GPIO_EXT_ETM_CH0_EVENT_EN_S  7

/** GPIO_EXT_ETM_EVENT_CH7_CFG_REG register
 *  Etm Config register of Channel7
 */
#define GPIO_EXT_ETM_EVENT_CH7_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x134)
/** GPIO_EXT_ETM_CH0_EVENT_SEL : R/W; bitpos: [5:0]; default: 0;
 *  Configures to select GPIO for ETM event channel.\\
 *  0: Select GPIO0\\
 *  1: Select GPIO1\\
 *  ......\\
 *  27: Select GPIO27\\
 *  28: Select GPIO28\\
 *  29 ~ 63: Reserved\\
 */
#define GPIO_EXT_ETM_CH0_EVENT_SEL    0x0000003FU
#define GPIO_EXT_ETM_CH0_EVENT_SEL_M  (GPIO_EXT_ETM_CH0_EVENT_SEL_V << GPIO_EXT_ETM_CH0_EVENT_SEL_S)
#define GPIO_EXT_ETM_CH0_EVENT_SEL_V  0x0000003FU
#define GPIO_EXT_ETM_CH0_EVENT_SEL_S  0
/** GPIO_EXT_ETM_CH0_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable ETM event send.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_CH0_EVENT_EN    (BIT(7))
#define GPIO_EXT_ETM_CH0_EVENT_EN_M  (GPIO_EXT_ETM_CH0_EVENT_EN_V << GPIO_EXT_ETM_CH0_EVENT_EN_S)
#define GPIO_EXT_ETM_CH0_EVENT_EN_V  0x00000001U
#define GPIO_EXT_ETM_CH0_EVENT_EN_S  7

/** GPIO_EXT_ETM_TASK_P0_CFG_REG register
 *  GPIO selection register 0 for ETM
 */
#define GPIO_EXT_ETM_TASK_P0_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x158)
/** GPIO_EXT_ETM_TASK_GPIO0_SEL : R/W; bitpos: [2:0]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO0_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO0_SEL_M  (GPIO_EXT_ETM_TASK_GPIO0_SEL_V << GPIO_EXT_ETM_TASK_GPIO0_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO0_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO0_SEL_S  0
/** GPIO_EXT_ETM_TASK_GPIO0_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO0_EN    (BIT(5))
#define GPIO_EXT_ETM_TASK_GPIO0_EN_M  (GPIO_EXT_ETM_TASK_GPIO0_EN_V << GPIO_EXT_ETM_TASK_GPIO0_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO0_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO0_EN_S  5
/** GPIO_EXT_ETM_TASK_GPIO1_SEL : R/W; bitpos: [8:6]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO1_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO1_SEL_M  (GPIO_EXT_ETM_TASK_GPIO1_SEL_V << GPIO_EXT_ETM_TASK_GPIO1_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO1_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO1_SEL_S  6
/** GPIO_EXT_ETM_TASK_GPIO1_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO1_EN    (BIT(11))
#define GPIO_EXT_ETM_TASK_GPIO1_EN_M  (GPIO_EXT_ETM_TASK_GPIO1_EN_V << GPIO_EXT_ETM_TASK_GPIO1_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO1_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO1_EN_S  11
/** GPIO_EXT_ETM_TASK_GPIO2_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO2_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO2_SEL_M  (GPIO_EXT_ETM_TASK_GPIO2_SEL_V << GPIO_EXT_ETM_TASK_GPIO2_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO2_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO2_SEL_S  12
/** GPIO_EXT_ETM_TASK_GPIO2_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO2_EN    (BIT(17))
#define GPIO_EXT_ETM_TASK_GPIO2_EN_M  (GPIO_EXT_ETM_TASK_GPIO2_EN_V << GPIO_EXT_ETM_TASK_GPIO2_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO2_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO2_EN_S  17
/** GPIO_EXT_ETM_TASK_GPIO3_SEL : R/W; bitpos: [20:18]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO3_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO3_SEL_M  (GPIO_EXT_ETM_TASK_GPIO3_SEL_V << GPIO_EXT_ETM_TASK_GPIO3_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO3_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO3_SEL_S  18
/** GPIO_EXT_ETM_TASK_GPIO3_EN : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO3_EN    (BIT(23))
#define GPIO_EXT_ETM_TASK_GPIO3_EN_M  (GPIO_EXT_ETM_TASK_GPIO3_EN_V << GPIO_EXT_ETM_TASK_GPIO3_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO3_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO3_EN_S  23
/** GPIO_EXT_ETM_TASK_GPIO4_SEL : R/W; bitpos: [26:24]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO4_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO4_SEL_M  (GPIO_EXT_ETM_TASK_GPIO4_SEL_V << GPIO_EXT_ETM_TASK_GPIO4_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO4_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO4_SEL_S  24
/** GPIO_EXT_ETM_TASK_GPIO4_EN : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO4_EN    (BIT(29))
#define GPIO_EXT_ETM_TASK_GPIO4_EN_M  (GPIO_EXT_ETM_TASK_GPIO4_EN_V << GPIO_EXT_ETM_TASK_GPIO4_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO4_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO4_EN_S  29

/** GPIO_EXT_ETM_TASK_P1_CFG_REG register
 *  GPIO selection register 1 for ETM
 */
#define GPIO_EXT_ETM_TASK_P1_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x15c)
/** GPIO_EXT_ETM_TASK_GPIO5_SEL : R/W; bitpos: [2:0]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO5_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO5_SEL_M  (GPIO_EXT_ETM_TASK_GPIO5_SEL_V << GPIO_EXT_ETM_TASK_GPIO5_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO5_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO5_SEL_S  0
/** GPIO_EXT_ETM_TASK_GPIO5_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO5_EN    (BIT(5))
#define GPIO_EXT_ETM_TASK_GPIO5_EN_M  (GPIO_EXT_ETM_TASK_GPIO5_EN_V << GPIO_EXT_ETM_TASK_GPIO5_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO5_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO5_EN_S  5
/** GPIO_EXT_ETM_TASK_GPIO6_SEL : R/W; bitpos: [8:6]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO6_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO6_SEL_M  (GPIO_EXT_ETM_TASK_GPIO6_SEL_V << GPIO_EXT_ETM_TASK_GPIO6_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO6_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO6_SEL_S  6
/** GPIO_EXT_ETM_TASK_GPIO6_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO6_EN    (BIT(11))
#define GPIO_EXT_ETM_TASK_GPIO6_EN_M  (GPIO_EXT_ETM_TASK_GPIO6_EN_V << GPIO_EXT_ETM_TASK_GPIO6_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO6_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO6_EN_S  11
/** GPIO_EXT_ETM_TASK_GPIO7_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO7_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO7_SEL_M  (GPIO_EXT_ETM_TASK_GPIO7_SEL_V << GPIO_EXT_ETM_TASK_GPIO7_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO7_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO7_SEL_S  12
/** GPIO_EXT_ETM_TASK_GPIO7_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO7_EN    (BIT(17))
#define GPIO_EXT_ETM_TASK_GPIO7_EN_M  (GPIO_EXT_ETM_TASK_GPIO7_EN_V << GPIO_EXT_ETM_TASK_GPIO7_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO7_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO7_EN_S  17
/** GPIO_EXT_ETM_TASK_GPIO8_SEL : R/W; bitpos: [20:18]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO8_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO8_SEL_M  (GPIO_EXT_ETM_TASK_GPIO8_SEL_V << GPIO_EXT_ETM_TASK_GPIO8_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO8_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO8_SEL_S  18
/** GPIO_EXT_ETM_TASK_GPIO8_EN : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO8_EN    (BIT(23))
#define GPIO_EXT_ETM_TASK_GPIO8_EN_M  (GPIO_EXT_ETM_TASK_GPIO8_EN_V << GPIO_EXT_ETM_TASK_GPIO8_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO8_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO8_EN_S  23
/** GPIO_EXT_ETM_TASK_GPIO9_SEL : R/W; bitpos: [26:24]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO9_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO9_SEL_M  (GPIO_EXT_ETM_TASK_GPIO9_SEL_V << GPIO_EXT_ETM_TASK_GPIO9_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO9_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO9_SEL_S  24
/** GPIO_EXT_ETM_TASK_GPIO9_EN : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO9_EN    (BIT(29))
#define GPIO_EXT_ETM_TASK_GPIO9_EN_M  (GPIO_EXT_ETM_TASK_GPIO9_EN_V << GPIO_EXT_ETM_TASK_GPIO9_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO9_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO9_EN_S  29

/** GPIO_EXT_ETM_TASK_P2_CFG_REG register
 *  GPIO selection register 2 for ETM
 */
#define GPIO_EXT_ETM_TASK_P2_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x160)
/** GPIO_EXT_ETM_TASK_GPIO10_SEL : R/W; bitpos: [2:0]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO10_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO10_SEL_M  (GPIO_EXT_ETM_TASK_GPIO10_SEL_V << GPIO_EXT_ETM_TASK_GPIO10_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO10_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO10_SEL_S  0
/** GPIO_EXT_ETM_TASK_GPIO10_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO10_EN    (BIT(5))
#define GPIO_EXT_ETM_TASK_GPIO10_EN_M  (GPIO_EXT_ETM_TASK_GPIO10_EN_V << GPIO_EXT_ETM_TASK_GPIO10_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO10_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO10_EN_S  5
/** GPIO_EXT_ETM_TASK_GPIO11_SEL : R/W; bitpos: [8:6]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO11_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO11_SEL_M  (GPIO_EXT_ETM_TASK_GPIO11_SEL_V << GPIO_EXT_ETM_TASK_GPIO11_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO11_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO11_SEL_S  6
/** GPIO_EXT_ETM_TASK_GPIO11_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO11_EN    (BIT(11))
#define GPIO_EXT_ETM_TASK_GPIO11_EN_M  (GPIO_EXT_ETM_TASK_GPIO11_EN_V << GPIO_EXT_ETM_TASK_GPIO11_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO11_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO11_EN_S  11
/** GPIO_EXT_ETM_TASK_GPIO12_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO12_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO12_SEL_M  (GPIO_EXT_ETM_TASK_GPIO12_SEL_V << GPIO_EXT_ETM_TASK_GPIO12_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO12_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO12_SEL_S  12
/** GPIO_EXT_ETM_TASK_GPIO12_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO12_EN    (BIT(17))
#define GPIO_EXT_ETM_TASK_GPIO12_EN_M  (GPIO_EXT_ETM_TASK_GPIO12_EN_V << GPIO_EXT_ETM_TASK_GPIO12_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO12_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO12_EN_S  17
/** GPIO_EXT_ETM_TASK_GPIO13_SEL : R/W; bitpos: [20:18]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO13_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO13_SEL_M  (GPIO_EXT_ETM_TASK_GPIO13_SEL_V << GPIO_EXT_ETM_TASK_GPIO13_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO13_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO13_SEL_S  18
/** GPIO_EXT_ETM_TASK_GPIO13_EN : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO13_EN    (BIT(23))
#define GPIO_EXT_ETM_TASK_GPIO13_EN_M  (GPIO_EXT_ETM_TASK_GPIO13_EN_V << GPIO_EXT_ETM_TASK_GPIO13_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO13_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO13_EN_S  23
/** GPIO_EXT_ETM_TASK_GPIO14_SEL : R/W; bitpos: [26:24]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO14_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO14_SEL_M  (GPIO_EXT_ETM_TASK_GPIO14_SEL_V << GPIO_EXT_ETM_TASK_GPIO14_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO14_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO14_SEL_S  24
/** GPIO_EXT_ETM_TASK_GPIO14_EN : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO14_EN    (BIT(29))
#define GPIO_EXT_ETM_TASK_GPIO14_EN_M  (GPIO_EXT_ETM_TASK_GPIO14_EN_V << GPIO_EXT_ETM_TASK_GPIO14_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO14_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO14_EN_S  29

/** GPIO_EXT_ETM_TASK_P3_CFG_REG register
 *  GPIO selection register 3 for ETM
 */
#define GPIO_EXT_ETM_TASK_P3_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x164)
/** GPIO_EXT_ETM_TASK_GPIO15_SEL : R/W; bitpos: [2:0]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO15_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO15_SEL_M  (GPIO_EXT_ETM_TASK_GPIO15_SEL_V << GPIO_EXT_ETM_TASK_GPIO15_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO15_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO15_SEL_S  0
/** GPIO_EXT_ETM_TASK_GPIO15_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO15_EN    (BIT(5))
#define GPIO_EXT_ETM_TASK_GPIO15_EN_M  (GPIO_EXT_ETM_TASK_GPIO15_EN_V << GPIO_EXT_ETM_TASK_GPIO15_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO15_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO15_EN_S  5
/** GPIO_EXT_ETM_TASK_GPIO16_SEL : R/W; bitpos: [8:6]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO16_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO16_SEL_M  (GPIO_EXT_ETM_TASK_GPIO16_SEL_V << GPIO_EXT_ETM_TASK_GPIO16_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO16_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO16_SEL_S  6
/** GPIO_EXT_ETM_TASK_GPIO16_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO16_EN    (BIT(11))
#define GPIO_EXT_ETM_TASK_GPIO16_EN_M  (GPIO_EXT_ETM_TASK_GPIO16_EN_V << GPIO_EXT_ETM_TASK_GPIO16_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO16_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO16_EN_S  11
/** GPIO_EXT_ETM_TASK_GPIO17_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO17_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO17_SEL_M  (GPIO_EXT_ETM_TASK_GPIO17_SEL_V << GPIO_EXT_ETM_TASK_GPIO17_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO17_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO17_SEL_S  12
/** GPIO_EXT_ETM_TASK_GPIO17_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO17_EN    (BIT(17))
#define GPIO_EXT_ETM_TASK_GPIO17_EN_M  (GPIO_EXT_ETM_TASK_GPIO17_EN_V << GPIO_EXT_ETM_TASK_GPIO17_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO17_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO17_EN_S  17
/** GPIO_EXT_ETM_TASK_GPIO18_SEL : R/W; bitpos: [20:18]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO18_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO18_SEL_M  (GPIO_EXT_ETM_TASK_GPIO18_SEL_V << GPIO_EXT_ETM_TASK_GPIO18_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO18_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO18_SEL_S  18
/** GPIO_EXT_ETM_TASK_GPIO18_EN : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO18_EN    (BIT(23))
#define GPIO_EXT_ETM_TASK_GPIO18_EN_M  (GPIO_EXT_ETM_TASK_GPIO18_EN_V << GPIO_EXT_ETM_TASK_GPIO18_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO18_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO18_EN_S  23
/** GPIO_EXT_ETM_TASK_GPIO19_SEL : R/W; bitpos: [26:24]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO19_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO19_SEL_M  (GPIO_EXT_ETM_TASK_GPIO19_SEL_V << GPIO_EXT_ETM_TASK_GPIO19_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO19_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO19_SEL_S  24
/** GPIO_EXT_ETM_TASK_GPIO19_EN : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO19_EN    (BIT(29))
#define GPIO_EXT_ETM_TASK_GPIO19_EN_M  (GPIO_EXT_ETM_TASK_GPIO19_EN_V << GPIO_EXT_ETM_TASK_GPIO19_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO19_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO19_EN_S  29

/** GPIO_EXT_ETM_TASK_P4_CFG_REG register
 *  GPIO selection register 4 for ETM
 */
#define GPIO_EXT_ETM_TASK_P4_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x168)
/** GPIO_EXT_ETM_TASK_GPIO20_SEL : R/W; bitpos: [2:0]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO20_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO20_SEL_M  (GPIO_EXT_ETM_TASK_GPIO20_SEL_V << GPIO_EXT_ETM_TASK_GPIO20_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO20_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO20_SEL_S  0
/** GPIO_EXT_ETM_TASK_GPIO20_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO20_EN    (BIT(5))
#define GPIO_EXT_ETM_TASK_GPIO20_EN_M  (GPIO_EXT_ETM_TASK_GPIO20_EN_V << GPIO_EXT_ETM_TASK_GPIO20_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO20_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO20_EN_S  5
/** GPIO_EXT_ETM_TASK_GPIO21_SEL : R/W; bitpos: [8:6]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO21_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO21_SEL_M  (GPIO_EXT_ETM_TASK_GPIO21_SEL_V << GPIO_EXT_ETM_TASK_GPIO21_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO21_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO21_SEL_S  6
/** GPIO_EXT_ETM_TASK_GPIO21_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO21_EN    (BIT(11))
#define GPIO_EXT_ETM_TASK_GPIO21_EN_M  (GPIO_EXT_ETM_TASK_GPIO21_EN_V << GPIO_EXT_ETM_TASK_GPIO21_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO21_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO21_EN_S  11
/** GPIO_EXT_ETM_TASK_GPIO22_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO22_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO22_SEL_M  (GPIO_EXT_ETM_TASK_GPIO22_SEL_V << GPIO_EXT_ETM_TASK_GPIO22_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO22_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO22_SEL_S  12
/** GPIO_EXT_ETM_TASK_GPIO22_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO22_EN    (BIT(17))
#define GPIO_EXT_ETM_TASK_GPIO22_EN_M  (GPIO_EXT_ETM_TASK_GPIO22_EN_V << GPIO_EXT_ETM_TASK_GPIO22_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO22_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO22_EN_S  17
/** GPIO_EXT_ETM_TASK_GPIO23_SEL : R/W; bitpos: [20:18]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO23_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO23_SEL_M  (GPIO_EXT_ETM_TASK_GPIO23_SEL_V << GPIO_EXT_ETM_TASK_GPIO23_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO23_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO23_SEL_S  18
/** GPIO_EXT_ETM_TASK_GPIO23_EN : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO23_EN    (BIT(23))
#define GPIO_EXT_ETM_TASK_GPIO23_EN_M  (GPIO_EXT_ETM_TASK_GPIO23_EN_V << GPIO_EXT_ETM_TASK_GPIO23_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO23_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO23_EN_S  23
/** GPIO_EXT_ETM_TASK_GPIO24_SEL : R/W; bitpos: [26:24]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO24_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO24_SEL_M  (GPIO_EXT_ETM_TASK_GPIO24_SEL_V << GPIO_EXT_ETM_TASK_GPIO24_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO24_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO24_SEL_S  24
/** GPIO_EXT_ETM_TASK_GPIO24_EN : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO24_EN    (BIT(29))
#define GPIO_EXT_ETM_TASK_GPIO24_EN_M  (GPIO_EXT_ETM_TASK_GPIO24_EN_V << GPIO_EXT_ETM_TASK_GPIO24_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO24_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO24_EN_S  29

/** GPIO_EXT_ETM_TASK_P5_CFG_REG register
 *  GPIO selection register 5 for ETM
 */
#define GPIO_EXT_ETM_TASK_P5_CFG_REG (DR_REG_GPIO_EXT_BASE + 0x16c)
/** GPIO_EXT_ETM_TASK_GPIO25_SEL : R/W; bitpos: [2:0]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO25_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO25_SEL_M  (GPIO_EXT_ETM_TASK_GPIO25_SEL_V << GPIO_EXT_ETM_TASK_GPIO25_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO25_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO25_SEL_S  0
/** GPIO_EXT_ETM_TASK_GPIO25_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO25_EN    (BIT(5))
#define GPIO_EXT_ETM_TASK_GPIO25_EN_M  (GPIO_EXT_ETM_TASK_GPIO25_EN_V << GPIO_EXT_ETM_TASK_GPIO25_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO25_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO25_EN_S  5
/** GPIO_EXT_ETM_TASK_GPIO26_SEL : R/W; bitpos: [8:6]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO26_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO26_SEL_M  (GPIO_EXT_ETM_TASK_GPIO26_SEL_V << GPIO_EXT_ETM_TASK_GPIO26_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO26_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO26_SEL_S  6
/** GPIO_EXT_ETM_TASK_GPIO26_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO26_EN    (BIT(11))
#define GPIO_EXT_ETM_TASK_GPIO26_EN_M  (GPIO_EXT_ETM_TASK_GPIO26_EN_V << GPIO_EXT_ETM_TASK_GPIO26_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO26_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO26_EN_S  11
/** GPIO_EXT_ETM_TASK_GPIO27_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO27_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO27_SEL_M  (GPIO_EXT_ETM_TASK_GPIO27_SEL_V << GPIO_EXT_ETM_TASK_GPIO27_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO27_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO27_SEL_S  12
/** GPIO_EXT_ETM_TASK_GPIO27_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO27_EN    (BIT(17))
#define GPIO_EXT_ETM_TASK_GPIO27_EN_M  (GPIO_EXT_ETM_TASK_GPIO27_EN_V << GPIO_EXT_ETM_TASK_GPIO27_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO27_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO27_EN_S  17
/** GPIO_EXT_ETM_TASK_GPIO28_SEL : R/W; bitpos: [20:18]; default: 0;
 *  Configures to select an ETM task channel for GPIO$n.\\
 *  0: Select channel 0\\
 *  1: Select channel 1\\
 *  ......\\
 *  7: Select channel 7\\%\label{fielddesc:GPIOSDETMTASKGPIO1EN}-
 *  [GPIOSD_ETM_TASK_GPIO1_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO1SEL}- [GPIOSD_ETM_TASK_GPIO1_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO2EN}-
 *  [GPIOSD_ETM_TASK_GPIO2_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO2SEL}- [GPIOSD_ETM_TASK_GPIO2_SEL] GPIO choose a
 *  etm task channel. %\label{fielddesc:GPIOSDETMTASKGPIO3EN}\item
 *  [GPIOSD_ETM_TASK_GPIO3_EN] Enable bit of GPIO response etm task.
 *  %\label{fielddesc:GPIOSDETMTASKGPIO3SEL}\item [GPIOSD_ETM_TASK_GPIO3_SEL] GPIO
 *  choose a etm task channel.
 */
#define GPIO_EXT_ETM_TASK_GPIO28_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO28_SEL_M  (GPIO_EXT_ETM_TASK_GPIO28_SEL_V << GPIO_EXT_ETM_TASK_GPIO28_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO28_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO28_SEL_S  18
/** GPIO_EXT_ETM_TASK_GPIO28_EN : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to enable GPIO$n to response ETM task.\\
 *  0: Not enable\\
 *  1: Enable\\
 */
#define GPIO_EXT_ETM_TASK_GPIO28_EN    (BIT(23))
#define GPIO_EXT_ETM_TASK_GPIO28_EN_M  (GPIO_EXT_ETM_TASK_GPIO28_EN_V << GPIO_EXT_ETM_TASK_GPIO28_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO28_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO28_EN_S  23

/** GPIO_EXT_INT_RAW_REG register
 *  GPIO_EXT interrupt raw register
 */
#define GPIO_EXT_INT_RAW_REG (DR_REG_GPIO_EXT_BASE + 0x1d0)
/** GPIO_EXT_COMP_NEG_0_INT_RAW : RO/WTC/SS; bitpos: [0]; default: 0;
 *  analog comparator pos edge interrupt raw
 */
#define GPIO_EXT_COMP_NEG_0_INT_RAW    (BIT(0))
#define GPIO_EXT_COMP_NEG_0_INT_RAW_M  (GPIO_EXT_COMP_NEG_0_INT_RAW_V << GPIO_EXT_COMP_NEG_0_INT_RAW_S)
#define GPIO_EXT_COMP_NEG_0_INT_RAW_V  0x00000001U
#define GPIO_EXT_COMP_NEG_0_INT_RAW_S  0
/** GPIO_EXT_COMP_POS_0_INT_RAW : RO/WTC/SS; bitpos: [1]; default: 0;
 *  analog comparator neg edge interrupt raw
 */
#define GPIO_EXT_COMP_POS_0_INT_RAW    (BIT(1))
#define GPIO_EXT_COMP_POS_0_INT_RAW_M  (GPIO_EXT_COMP_POS_0_INT_RAW_V << GPIO_EXT_COMP_POS_0_INT_RAW_S)
#define GPIO_EXT_COMP_POS_0_INT_RAW_V  0x00000001U
#define GPIO_EXT_COMP_POS_0_INT_RAW_S  1
/** GPIO_EXT_COMP_ALL_0_INT_RAW : RO/WTC/SS; bitpos: [2]; default: 0;
 *  analog comparator neg or pos edge interrupt raw
 */
#define GPIO_EXT_COMP_ALL_0_INT_RAW    (BIT(2))
#define GPIO_EXT_COMP_ALL_0_INT_RAW_M  (GPIO_EXT_COMP_ALL_0_INT_RAW_V << GPIO_EXT_COMP_ALL_0_INT_RAW_S)
#define GPIO_EXT_COMP_ALL_0_INT_RAW_V  0x00000001U
#define GPIO_EXT_COMP_ALL_0_INT_RAW_S  2

/** GPIO_EXT_INT_ST_REG register
 *  GPIO_EXT interrupt masked register
 */
#define GPIO_EXT_INT_ST_REG (DR_REG_GPIO_EXT_BASE + 0x1d4)
/** GPIO_EXT_COMP_NEG_0_INT_ST : RO; bitpos: [0]; default: 0;
 *  analog comparator pos edge interrupt status
 */
#define GPIO_EXT_COMP_NEG_0_INT_ST    (BIT(0))
#define GPIO_EXT_COMP_NEG_0_INT_ST_M  (GPIO_EXT_COMP_NEG_0_INT_ST_V << GPIO_EXT_COMP_NEG_0_INT_ST_S)
#define GPIO_EXT_COMP_NEG_0_INT_ST_V  0x00000001U
#define GPIO_EXT_COMP_NEG_0_INT_ST_S  0
/** GPIO_EXT_COMP_POS_0_INT_ST : RO; bitpos: [1]; default: 0;
 *  analog comparator neg edge interrupt status
 */
#define GPIO_EXT_COMP_POS_0_INT_ST    (BIT(1))
#define GPIO_EXT_COMP_POS_0_INT_ST_M  (GPIO_EXT_COMP_POS_0_INT_ST_V << GPIO_EXT_COMP_POS_0_INT_ST_S)
#define GPIO_EXT_COMP_POS_0_INT_ST_V  0x00000001U
#define GPIO_EXT_COMP_POS_0_INT_ST_S  1
/** GPIO_EXT_COMP_ALL_0_INT_ST : RO; bitpos: [2]; default: 0;
 *  analog comparator neg or pos edge interrupt status
 */
#define GPIO_EXT_COMP_ALL_0_INT_ST    (BIT(2))
#define GPIO_EXT_COMP_ALL_0_INT_ST_M  (GPIO_EXT_COMP_ALL_0_INT_ST_V << GPIO_EXT_COMP_ALL_0_INT_ST_S)
#define GPIO_EXT_COMP_ALL_0_INT_ST_V  0x00000001U
#define GPIO_EXT_COMP_ALL_0_INT_ST_S  2

/** GPIO_EXT_INT_ENA_REG register
 *  GPIO_EXT interrupt enable register
 */
#define GPIO_EXT_INT_ENA_REG (DR_REG_GPIO_EXT_BASE + 0x1d8)
/** GPIO_EXT_COMP_NEG_0_INT_ENA : R/W; bitpos: [0]; default: 1;
 *  analog comparator pos edge interrupt enable
 */
#define GPIO_EXT_COMP_NEG_0_INT_ENA    (BIT(0))
#define GPIO_EXT_COMP_NEG_0_INT_ENA_M  (GPIO_EXT_COMP_NEG_0_INT_ENA_V << GPIO_EXT_COMP_NEG_0_INT_ENA_S)
#define GPIO_EXT_COMP_NEG_0_INT_ENA_V  0x00000001U
#define GPIO_EXT_COMP_NEG_0_INT_ENA_S  0
/** GPIO_EXT_COMP_POS_0_INT_ENA : R/W; bitpos: [1]; default: 1;
 *  analog comparator neg edge interrupt enable
 */
#define GPIO_EXT_COMP_POS_0_INT_ENA    (BIT(1))
#define GPIO_EXT_COMP_POS_0_INT_ENA_M  (GPIO_EXT_COMP_POS_0_INT_ENA_V << GPIO_EXT_COMP_POS_0_INT_ENA_S)
#define GPIO_EXT_COMP_POS_0_INT_ENA_V  0x00000001U
#define GPIO_EXT_COMP_POS_0_INT_ENA_S  1
/** GPIO_EXT_COMP_ALL_0_INT_ENA : R/W; bitpos: [2]; default: 1;
 *  analog comparator neg or pos edge interrupt enable
 */
#define GPIO_EXT_COMP_ALL_0_INT_ENA    (BIT(2))
#define GPIO_EXT_COMP_ALL_0_INT_ENA_M  (GPIO_EXT_COMP_ALL_0_INT_ENA_V << GPIO_EXT_COMP_ALL_0_INT_ENA_S)
#define GPIO_EXT_COMP_ALL_0_INT_ENA_V  0x00000001U
#define GPIO_EXT_COMP_ALL_0_INT_ENA_S  2

/** GPIO_EXT_INT_CLR_REG register
 *  GPIO_EXT interrupt clear register
 */
#define GPIO_EXT_INT_CLR_REG (DR_REG_GPIO_EXT_BASE + 0x1dc)
/** GPIO_EXT_COMP_NEG_0_INT_CLR : WT; bitpos: [0]; default: 0;
 *  analog comparator pos edge interrupt clear
 */
#define GPIO_EXT_COMP_NEG_0_INT_CLR    (BIT(0))
#define GPIO_EXT_COMP_NEG_0_INT_CLR_M  (GPIO_EXT_COMP_NEG_0_INT_CLR_V << GPIO_EXT_COMP_NEG_0_INT_CLR_S)
#define GPIO_EXT_COMP_NEG_0_INT_CLR_V  0x00000001U
#define GPIO_EXT_COMP_NEG_0_INT_CLR_S  0
/** GPIO_EXT_COMP_POS_0_INT_CLR : WT; bitpos: [1]; default: 0;
 *  analog comparator neg edge interrupt clear
 */
#define GPIO_EXT_COMP_POS_0_INT_CLR    (BIT(1))
#define GPIO_EXT_COMP_POS_0_INT_CLR_M  (GPIO_EXT_COMP_POS_0_INT_CLR_V << GPIO_EXT_COMP_POS_0_INT_CLR_S)
#define GPIO_EXT_COMP_POS_0_INT_CLR_V  0x00000001U
#define GPIO_EXT_COMP_POS_0_INT_CLR_S  1
/** GPIO_EXT_COMP_ALL_0_INT_CLR : WT; bitpos: [2]; default: 0;
 *  analog comparator neg or pos edge interrupt clear
 */
#define GPIO_EXT_COMP_ALL_0_INT_CLR    (BIT(2))
#define GPIO_EXT_COMP_ALL_0_INT_CLR_M  (GPIO_EXT_COMP_ALL_0_INT_CLR_V << GPIO_EXT_COMP_ALL_0_INT_CLR_S)
#define GPIO_EXT_COMP_ALL_0_INT_CLR_V  0x00000001U
#define GPIO_EXT_COMP_ALL_0_INT_CLR_S  2

/** GPIO_EXT_PIN_CTRL_REG register
 *  Clock Output Configuration Register
 */
#define GPIO_EXT_PIN_CTRL_REG (DR_REG_GPIO_EXT_BASE + 0x1e0)
/** GPIO_EXT_CLK_OUT1 : R/W; bitpos: [4:0]; default: 0;
 *  If you want to output clock for I2S to CLK_OUT_out1, set this register to 0x0.
 *  CLK_OUT_out1 can be found in peripheral output signals.
 */
#define GPIO_EXT_CLK_OUT1    0x0000001FU
#define GPIO_EXT_CLK_OUT1_M  (GPIO_EXT_CLK_OUT1_V << GPIO_EXT_CLK_OUT1_S)
#define GPIO_EXT_CLK_OUT1_V  0x0000001FU
#define GPIO_EXT_CLK_OUT1_S  0
/** GPIO_EXT_CLK_OUT2 : R/W; bitpos: [9:5]; default: 0;
 *  If you want to output clock for I2S to CLK_OUT_out2, set this register to 0x0.
 *  CLK_OUT_out2 can be found in peripheral output signals.
 */
#define GPIO_EXT_CLK_OUT2    0x0000001FU
#define GPIO_EXT_CLK_OUT2_M  (GPIO_EXT_CLK_OUT2_V << GPIO_EXT_CLK_OUT2_S)
#define GPIO_EXT_CLK_OUT2_V  0x0000001FU
#define GPIO_EXT_CLK_OUT2_S  5
/** GPIO_EXT_CLK_OUT3 : R/W; bitpos: [14:10]; default: 0;
 *  If you want to output clock for I2S to CLK_OUT_out3, set this register to 0x0.
 *  CLK_OUT_out3 can be found in peripheral output signals.
 */
#define GPIO_EXT_CLK_OUT3    0x0000001FU
#define GPIO_EXT_CLK_OUT3_M  (GPIO_EXT_CLK_OUT3_V << GPIO_EXT_CLK_OUT3_S)
#define GPIO_EXT_CLK_OUT3_V  0x0000001FU
#define GPIO_EXT_CLK_OUT3_S  10

/** GPIO_EXT_VERSION_REG register
 *  Version Control Register
 */
#define GPIO_EXT_VERSION_REG (DR_REG_GPIO_EXT_BASE + 0x1fc)
/** GPIO_EXT_DATE : R/W; bitpos: [27:0]; default: 36774208;
 *  Version control register.
 */
#define GPIO_EXT_DATE    0x0FFFFFFFU
#define GPIO_EXT_DATE_M  (GPIO_EXT_DATE_V << GPIO_EXT_DATE_S)
#define GPIO_EXT_DATE_V  0x0FFFFFFFU
#define GPIO_EXT_DATE_S  0

#ifdef __cplusplus
}
#endif
