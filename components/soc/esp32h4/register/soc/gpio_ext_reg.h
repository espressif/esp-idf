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

/** GPIO_EXT_SIGMADELTA_MISC_REG register
 *  MISC Register
 */
#define GPIO_EXT_SIGMADELTA_MISC_REG (DR_REG_GPIO_BASE + 0x4)
/** GPIO_EXT_SIGMADELTA_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the clock for sigma delta modulation.
 *  0: Not enable
 *  1: Enable
 *  %\label{fielddesc:GPIOSDSPISWAP}- [GPIOSD_SPI_SWAP] Reserved.
 */
#define GPIO_EXT_SIGMADELTA_CLK_EN    (BIT(0))
#define GPIO_EXT_SIGMADELTA_CLK_EN_M  (GPIO_EXT_SIGMADELTA_CLK_EN_V << GPIO_EXT_SIGMADELTA_CLK_EN_S)
#define GPIO_EXT_SIGMADELTA_CLK_EN_V  0x00000001U
#define GPIO_EXT_SIGMADELTA_CLK_EN_S  0

/** GPIO_EXT_SIGMADELTA0_REG register
 *  Duty cycle configuration register for SDM channel 0
 */
#define GPIO_EXT_SIGMADELTA0_REG (DR_REG_GPIO_BASE + 0x8)
/** GPIO_EXT_SD0_IN : R/W; bitpos: [7:0]; default: 0;
 *  Configures the duty cycle of sigma delta modulation output.
 */
#define GPIO_EXT_SD0_IN    0x000000FFU
#define GPIO_EXT_SD0_IN_M  (GPIO_EXT_SD0_IN_V << GPIO_EXT_SD0_IN_S)
#define GPIO_EXT_SD0_IN_V  0x000000FFU
#define GPIO_EXT_SD0_IN_S  0
/** GPIO_EXT_SD0_PRESCALE : R/W; bitpos: [15:8]; default: 255;
 *  Configures the divider value to divide IO MUX operating clock.
 */
#define GPIO_EXT_SD0_PRESCALE    0x000000FFU
#define GPIO_EXT_SD0_PRESCALE_M  (GPIO_EXT_SD0_PRESCALE_V << GPIO_EXT_SD0_PRESCALE_S)
#define GPIO_EXT_SD0_PRESCALE_V  0x000000FFU
#define GPIO_EXT_SD0_PRESCALE_S  8

/** GPIO_EXT_SIGMADELTA1_REG register
 *  Duty cycle configuration register for SDM channel 1
 */
#define GPIO_EXT_SIGMADELTA1_REG (DR_REG_GPIO_BASE + 0xc)
/** GPIO_EXT_SD1_IN : R/W; bitpos: [7:0]; default: 0;
 *  Configures the duty cycle of sigma delta modulation output.
 */
#define GPIO_EXT_SD1_IN    0x000000FFU
#define GPIO_EXT_SD1_IN_M  (GPIO_EXT_SD1_IN_V << GPIO_EXT_SD1_IN_S)
#define GPIO_EXT_SD1_IN_V  0x000000FFU
#define GPIO_EXT_SD1_IN_S  0
/** GPIO_EXT_SD1_PRESCALE : R/W; bitpos: [15:8]; default: 255;
 *  Configures the divider value to divide IO MUX operating clock.
 */
#define GPIO_EXT_SD1_PRESCALE    0x000000FFU
#define GPIO_EXT_SD1_PRESCALE_M  (GPIO_EXT_SD1_PRESCALE_V << GPIO_EXT_SD1_PRESCALE_S)
#define GPIO_EXT_SD1_PRESCALE_V  0x000000FFU
#define GPIO_EXT_SD1_PRESCALE_S  8

/** GPIO_EXT_SIGMADELTA2_REG register
 *  Duty cycle configuration register for SDM channel 2
 */
#define GPIO_EXT_SIGMADELTA2_REG (DR_REG_GPIO_BASE + 0x10)
/** GPIO_EXT_SD2_IN : R/W; bitpos: [7:0]; default: 0;
 *  Configures the duty cycle of sigma delta modulation output.
 */
#define GPIO_EXT_SD2_IN    0x000000FFU
#define GPIO_EXT_SD2_IN_M  (GPIO_EXT_SD2_IN_V << GPIO_EXT_SD2_IN_S)
#define GPIO_EXT_SD2_IN_V  0x000000FFU
#define GPIO_EXT_SD2_IN_S  0
/** GPIO_EXT_SD2_PRESCALE : R/W; bitpos: [15:8]; default: 255;
 *  Configures the divider value to divide IO MUX operating clock.
 */
#define GPIO_EXT_SD2_PRESCALE    0x000000FFU
#define GPIO_EXT_SD2_PRESCALE_M  (GPIO_EXT_SD2_PRESCALE_V << GPIO_EXT_SD2_PRESCALE_S)
#define GPIO_EXT_SD2_PRESCALE_V  0x000000FFU
#define GPIO_EXT_SD2_PRESCALE_S  8

/** GPIO_EXT_SIGMADELTA3_REG register
 *  Duty cycle configuration register for SDM channel 3
 */
#define GPIO_EXT_SIGMADELTA3_REG (DR_REG_GPIO_BASE + 0x14)
/** GPIO_EXT_SD3_IN : R/W; bitpos: [7:0]; default: 0;
 *  Configures the duty cycle of sigma delta modulation output.
 */
#define GPIO_EXT_SD3_IN    0x000000FFU
#define GPIO_EXT_SD3_IN_M  (GPIO_EXT_SD3_IN_V << GPIO_EXT_SD3_IN_S)
#define GPIO_EXT_SD3_IN_V  0x000000FFU
#define GPIO_EXT_SD3_IN_S  0
/** GPIO_EXT_SD3_PRESCALE : R/W; bitpos: [15:8]; default: 255;
 *  Configures the divider value to divide IO MUX operating clock.
 */
#define GPIO_EXT_SD3_PRESCALE    0x000000FFU
#define GPIO_EXT_SD3_PRESCALE_M  (GPIO_EXT_SD3_PRESCALE_V << GPIO_EXT_SD3_PRESCALE_S)
#define GPIO_EXT_SD3_PRESCALE_V  0x000000FFU
#define GPIO_EXT_SD3_PRESCALE_S  8

/** GPIO_EXT_GLITCH_FILTER_CH0_REG register
 *  Glitch Filter Configure Register of Channel0
 */
#define GPIO_EXT_GLITCH_FILTER_CH0_REG (DR_REG_GPIO_BASE + 0xd8)
/** GPIO_EXT_FILTER_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable channel 0 of Glitch Filter.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_FILTER_CH0_EN    (BIT(0))
#define GPIO_EXT_FILTER_CH0_EN_M  (GPIO_EXT_FILTER_CH0_EN_V << GPIO_EXT_FILTER_CH0_EN_S)
#define GPIO_EXT_FILTER_CH0_EN_V  0x00000001U
#define GPIO_EXT_FILTER_CH0_EN_S  0
/** GPIO_EXT_FILTER_CH0_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Configures to select the input GPIO for Glitch Filter.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  38: Select GPIO38
 *  39: Select GPIO39
 *  40 ~ 63: Reserved
 */
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM    0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_M  (GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V << GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S)
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S  1
/** GPIO_EXT_FILTER_CH0_WINDOW_THRES : R/W; bitpos: [13:8]; default: 0;
 *  Configures the window threshold for Glitch Filter. The window threshold should be
 *  less than or equal to GPIOSD_FILTER_CH0_WINDOW_WIDTH.
 *  %see DOC-4768
 *  Measurement unit: IO MUX operating clock cycle
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_M  (GPIO_EXT_FILTER_CH0_WINDOW_THRES_V << GPIO_EXT_FILTER_CH0_WINDOW_THRES_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_S  8
/** GPIO_EXT_FILTER_CH0_WINDOW_WIDTH : R/W; bitpos: [19:14]; default: 0;
 *  Configures the window width for Glitch Filter. The effective value of window width
 *  is 0 ~ 63.
 *  Measurement unit: IO MUX operating clock cycle
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_M  (GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V << GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S  14

/** GPIO_EXT_GLITCH_FILTER_CH1_REG register
 *  Glitch Filter Configure Register of Channel1
 */
#define GPIO_EXT_GLITCH_FILTER_CH1_REG (DR_REG_GPIO_BASE + 0xdc)
/** GPIO_EXT_FILTER_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable channel 1 of Glitch Filter.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_FILTER_CH0_EN    (BIT(0))
#define GPIO_EXT_FILTER_CH0_EN_M  (GPIO_EXT_FILTER_CH0_EN_V << GPIO_EXT_FILTER_CH0_EN_S)
#define GPIO_EXT_FILTER_CH0_EN_V  0x00000001U
#define GPIO_EXT_FILTER_CH0_EN_S  0
/** GPIO_EXT_FILTER_CH0_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Configures to select the input GPIO for Glitch Filter.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  38: Select GPIO38
 *  39: Select GPIO39
 *  40 ~ 63: Reserved
 */
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM    0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_M  (GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V << GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S)
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S  1
/** GPIO_EXT_FILTER_CH0_WINDOW_THRES : R/W; bitpos: [13:8]; default: 0;
 *  Configures the window threshold for Glitch Filter. The window threshold should be
 *  less than or equal to GPIOSD_FILTER_CH1_WINDOW_WIDTH.
 *  %see DOC-4768
 *  Measurement unit: IO MUX operating clock cycle
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_M  (GPIO_EXT_FILTER_CH0_WINDOW_THRES_V << GPIO_EXT_FILTER_CH0_WINDOW_THRES_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_S  8
/** GPIO_EXT_FILTER_CH0_WINDOW_WIDTH : R/W; bitpos: [19:14]; default: 0;
 *  Configures the window width for Glitch Filter. The effective value of window width
 *  is 0 ~ 63.
 *  Measurement unit: IO MUX operating clock cycle
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_M  (GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V << GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S  14

/** GPIO_EXT_GLITCH_FILTER_CH2_REG register
 *  Glitch Filter Configure Register of Channel2
 */
#define GPIO_EXT_GLITCH_FILTER_CH2_REG (DR_REG_GPIO_BASE + 0xe0)
/** GPIO_EXT_FILTER_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable channel 2 of Glitch Filter.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_FILTER_CH0_EN    (BIT(0))
#define GPIO_EXT_FILTER_CH0_EN_M  (GPIO_EXT_FILTER_CH0_EN_V << GPIO_EXT_FILTER_CH0_EN_S)
#define GPIO_EXT_FILTER_CH0_EN_V  0x00000001U
#define GPIO_EXT_FILTER_CH0_EN_S  0
/** GPIO_EXT_FILTER_CH0_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Configures to select the input GPIO for Glitch Filter.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  38: Select GPIO38
 *  39: Select GPIO39
 *  40 ~ 63: Reserved
 */
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM    0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_M  (GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V << GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S)
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S  1
/** GPIO_EXT_FILTER_CH0_WINDOW_THRES : R/W; bitpos: [13:8]; default: 0;
 *  Configures the window threshold for Glitch Filter. The window threshold should be
 *  less than or equal to GPIOSD_FILTER_CH2_WINDOW_WIDTH.
 *  %see DOC-4768
 *  Measurement unit: IO MUX operating clock cycle
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_M  (GPIO_EXT_FILTER_CH0_WINDOW_THRES_V << GPIO_EXT_FILTER_CH0_WINDOW_THRES_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_S  8
/** GPIO_EXT_FILTER_CH0_WINDOW_WIDTH : R/W; bitpos: [19:14]; default: 0;
 *  Configures the window width for Glitch Filter. The effective value of window width
 *  is 0 ~ 63.
 *  Measurement unit: IO MUX operating clock cycle
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_M  (GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V << GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S  14

/** GPIO_EXT_GLITCH_FILTER_CH3_REG register
 *  Glitch Filter Configure Register of Channel3
 */
#define GPIO_EXT_GLITCH_FILTER_CH3_REG (DR_REG_GPIO_BASE + 0xe4)
/** GPIO_EXT_FILTER_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable channel 3 of Glitch Filter.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_FILTER_CH0_EN    (BIT(0))
#define GPIO_EXT_FILTER_CH0_EN_M  (GPIO_EXT_FILTER_CH0_EN_V << GPIO_EXT_FILTER_CH0_EN_S)
#define GPIO_EXT_FILTER_CH0_EN_V  0x00000001U
#define GPIO_EXT_FILTER_CH0_EN_S  0
/** GPIO_EXT_FILTER_CH0_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Configures to select the input GPIO for Glitch Filter.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  38: Select GPIO38
 *  39: Select GPIO39
 *  40 ~ 63: Reserved
 */
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM    0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_M  (GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V << GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S)
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S  1
/** GPIO_EXT_FILTER_CH0_WINDOW_THRES : R/W; bitpos: [13:8]; default: 0;
 *  Configures the window threshold for Glitch Filter. The window threshold should be
 *  less than or equal to GPIOSD_FILTER_CH3_WINDOW_WIDTH.
 *  %see DOC-4768
 *  Measurement unit: IO MUX operating clock cycle
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_M  (GPIO_EXT_FILTER_CH0_WINDOW_THRES_V << GPIO_EXT_FILTER_CH0_WINDOW_THRES_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_S  8
/** GPIO_EXT_FILTER_CH0_WINDOW_WIDTH : R/W; bitpos: [19:14]; default: 0;
 *  Configures the window width for Glitch Filter. The effective value of window width
 *  is 0 ~ 63.
 *  Measurement unit: IO MUX operating clock cycle
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_M  (GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V << GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S  14

/** GPIO_EXT_GLITCH_FILTER_CH4_REG register
 *  Glitch Filter Configure Register of Channel4
 */
#define GPIO_EXT_GLITCH_FILTER_CH4_REG (DR_REG_GPIO_BASE + 0xe8)
/** GPIO_EXT_FILTER_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable channel 4 of Glitch Filter.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_FILTER_CH0_EN    (BIT(0))
#define GPIO_EXT_FILTER_CH0_EN_M  (GPIO_EXT_FILTER_CH0_EN_V << GPIO_EXT_FILTER_CH0_EN_S)
#define GPIO_EXT_FILTER_CH0_EN_V  0x00000001U
#define GPIO_EXT_FILTER_CH0_EN_S  0
/** GPIO_EXT_FILTER_CH0_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Configures to select the input GPIO for Glitch Filter.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  38: Select GPIO38
 *  39: Select GPIO39
 *  40 ~ 63: Reserved
 */
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM    0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_M  (GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V << GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S)
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S  1
/** GPIO_EXT_FILTER_CH0_WINDOW_THRES : R/W; bitpos: [13:8]; default: 0;
 *  Configures the window threshold for Glitch Filter. The window threshold should be
 *  less than or equal to GPIOSD_FILTER_CH4_WINDOW_WIDTH.
 *  %see DOC-4768
 *  Measurement unit: IO MUX operating clock cycle
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_M  (GPIO_EXT_FILTER_CH0_WINDOW_THRES_V << GPIO_EXT_FILTER_CH0_WINDOW_THRES_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_S  8
/** GPIO_EXT_FILTER_CH0_WINDOW_WIDTH : R/W; bitpos: [19:14]; default: 0;
 *  Configures the window width for Glitch Filter. The effective value of window width
 *  is 0 ~ 63.
 *  Measurement unit: IO MUX operating clock cycle
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_M  (GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V << GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S  14

/** GPIO_EXT_GLITCH_FILTER_CH5_REG register
 *  Glitch Filter Configure Register of Channel5
 */
#define GPIO_EXT_GLITCH_FILTER_CH5_REG (DR_REG_GPIO_BASE + 0xec)
/** GPIO_EXT_FILTER_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable channel 5 of Glitch Filter.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_FILTER_CH0_EN    (BIT(0))
#define GPIO_EXT_FILTER_CH0_EN_M  (GPIO_EXT_FILTER_CH0_EN_V << GPIO_EXT_FILTER_CH0_EN_S)
#define GPIO_EXT_FILTER_CH0_EN_V  0x00000001U
#define GPIO_EXT_FILTER_CH0_EN_S  0
/** GPIO_EXT_FILTER_CH0_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Configures to select the input GPIO for Glitch Filter.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  38: Select GPIO38
 *  39: Select GPIO39
 *  40 ~ 63: Reserved
 */
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM    0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_M  (GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V << GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S)
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S  1
/** GPIO_EXT_FILTER_CH0_WINDOW_THRES : R/W; bitpos: [13:8]; default: 0;
 *  Configures the window threshold for Glitch Filter. The window threshold should be
 *  less than or equal to GPIOSD_FILTER_CH5_WINDOW_WIDTH.
 *  %see DOC-4768
 *  Measurement unit: IO MUX operating clock cycle
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_M  (GPIO_EXT_FILTER_CH0_WINDOW_THRES_V << GPIO_EXT_FILTER_CH0_WINDOW_THRES_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_S  8
/** GPIO_EXT_FILTER_CH0_WINDOW_WIDTH : R/W; bitpos: [19:14]; default: 0;
 *  Configures the window width for Glitch Filter. The effective value of window width
 *  is 0 ~ 63.
 *  Measurement unit: IO MUX operating clock cycle
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_M  (GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V << GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S  14

/** GPIO_EXT_GLITCH_FILTER_CH6_REG register
 *  Glitch Filter Configure Register of Channel6
 */
#define GPIO_EXT_GLITCH_FILTER_CH6_REG (DR_REG_GPIO_BASE + 0xf0)
/** GPIO_EXT_FILTER_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable channel 6 of Glitch Filter.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_FILTER_CH0_EN    (BIT(0))
#define GPIO_EXT_FILTER_CH0_EN_M  (GPIO_EXT_FILTER_CH0_EN_V << GPIO_EXT_FILTER_CH0_EN_S)
#define GPIO_EXT_FILTER_CH0_EN_V  0x00000001U
#define GPIO_EXT_FILTER_CH0_EN_S  0
/** GPIO_EXT_FILTER_CH0_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Configures to select the input GPIO for Glitch Filter.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  38: Select GPIO38
 *  39: Select GPIO39
 *  40 ~ 63: Reserved
 */
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM    0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_M  (GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V << GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S)
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S  1
/** GPIO_EXT_FILTER_CH0_WINDOW_THRES : R/W; bitpos: [13:8]; default: 0;
 *  Configures the window threshold for Glitch Filter. The window threshold should be
 *  less than or equal to GPIOSD_FILTER_CH6_WINDOW_WIDTH.
 *  %see DOC-4768
 *  Measurement unit: IO MUX operating clock cycle
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_M  (GPIO_EXT_FILTER_CH0_WINDOW_THRES_V << GPIO_EXT_FILTER_CH0_WINDOW_THRES_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_S  8
/** GPIO_EXT_FILTER_CH0_WINDOW_WIDTH : R/W; bitpos: [19:14]; default: 0;
 *  Configures the window width for Glitch Filter. The effective value of window width
 *  is 0 ~ 63.
 *  Measurement unit: IO MUX operating clock cycle
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_M  (GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V << GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S  14

/** GPIO_EXT_GLITCH_FILTER_CH7_REG register
 *  Glitch Filter Configure Register of Channel7
 */
#define GPIO_EXT_GLITCH_FILTER_CH7_REG (DR_REG_GPIO_BASE + 0xf4)
/** GPIO_EXT_FILTER_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable channel 7 of Glitch Filter.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_FILTER_CH0_EN    (BIT(0))
#define GPIO_EXT_FILTER_CH0_EN_M  (GPIO_EXT_FILTER_CH0_EN_V << GPIO_EXT_FILTER_CH0_EN_S)
#define GPIO_EXT_FILTER_CH0_EN_V  0x00000001U
#define GPIO_EXT_FILTER_CH0_EN_S  0
/** GPIO_EXT_FILTER_CH0_INPUT_IO_NUM : R/W; bitpos: [6:1]; default: 0;
 *  Configures to select the input GPIO for Glitch Filter.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  38: Select GPIO38
 *  39: Select GPIO39
 *  40 ~ 63: Reserved
 */
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM    0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_M  (GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V << GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S)
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_INPUT_IO_NUM_S  1
/** GPIO_EXT_FILTER_CH0_WINDOW_THRES : R/W; bitpos: [13:8]; default: 0;
 *  Configures the window threshold for Glitch Filter. The window threshold should be
 *  less than or equal to GPIOSD_FILTER_CH7_WINDOW_WIDTH.
 *  %see DOC-4768
 *  Measurement unit: IO MUX operating clock cycle
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_M  (GPIO_EXT_FILTER_CH0_WINDOW_THRES_V << GPIO_EXT_FILTER_CH0_WINDOW_THRES_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_THRES_S  8
/** GPIO_EXT_FILTER_CH0_WINDOW_WIDTH : R/W; bitpos: [19:14]; default: 0;
 *  Configures the window width for Glitch Filter. The effective value of window width
 *  is 0 ~ 63.
 *  Measurement unit: IO MUX operating clock cycle
 */
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH    0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_M  (GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V << GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S)
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_V  0x0000003FU
#define GPIO_EXT_FILTER_CH0_WINDOW_WIDTH_S  14

/** GPIO_EXT_ETM_EVENT_CH0_CFG_REG register
 *  ETM configuration register for channel 0
 */
#define GPIO_EXT_ETM_EVENT_CH0_CFG_REG (DR_REG_GPIO_BASE + 0x118)
/** GPIO_EXT_ETM_CH0_EVENT_SEL : R/W; bitpos: [5:0]; default: 0;
 *  Configures to select GPIO for ETM event channel.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  38: Select GPIO38
 *  39: Select GPIO39
 *  40 ~ 63: Reserved
 */
#define GPIO_EXT_ETM_CH0_EVENT_SEL    0x0000003FU
#define GPIO_EXT_ETM_CH0_EVENT_SEL_M  (GPIO_EXT_ETM_CH0_EVENT_SEL_V << GPIO_EXT_ETM_CH0_EVENT_SEL_S)
#define GPIO_EXT_ETM_CH0_EVENT_SEL_V  0x0000003FU
#define GPIO_EXT_ETM_CH0_EVENT_SEL_S  0
/** GPIO_EXT_ETM_CH0_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable ETM event send.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_CH0_EVENT_EN    (BIT(7))
#define GPIO_EXT_ETM_CH0_EVENT_EN_M  (GPIO_EXT_ETM_CH0_EVENT_EN_V << GPIO_EXT_ETM_CH0_EVENT_EN_S)
#define GPIO_EXT_ETM_CH0_EVENT_EN_V  0x00000001U
#define GPIO_EXT_ETM_CH0_EVENT_EN_S  7

/** GPIO_EXT_ETM_EVENT_CH1_CFG_REG register
 *  ETM configuration register for channel 1
 */
#define GPIO_EXT_ETM_EVENT_CH1_CFG_REG (DR_REG_GPIO_BASE + 0x11c)
/** GPIO_EXT_ETM_CH1_EVENT_SEL : R/W; bitpos: [5:0]; default: 0;
 *  Configures to select GPIO for ETM event channel.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  38: Select GPIO38
 *  39: Select GPIO39
 *  40 ~ 63: Reserved
 */
#define GPIO_EXT_ETM_CH1_EVENT_SEL    0x0000003FU
#define GPIO_EXT_ETM_CH1_EVENT_SEL_M  (GPIO_EXT_ETM_CH1_EVENT_SEL_V << GPIO_EXT_ETM_CH1_EVENT_SEL_S)
#define GPIO_EXT_ETM_CH1_EVENT_SEL_V  0x0000003FU
#define GPIO_EXT_ETM_CH1_EVENT_SEL_S  0
/** GPIO_EXT_ETM_CH1_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable ETM event send.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_CH1_EVENT_EN    (BIT(7))
#define GPIO_EXT_ETM_CH1_EVENT_EN_M  (GPIO_EXT_ETM_CH1_EVENT_EN_V << GPIO_EXT_ETM_CH1_EVENT_EN_S)
#define GPIO_EXT_ETM_CH1_EVENT_EN_V  0x00000001U
#define GPIO_EXT_ETM_CH1_EVENT_EN_S  7

/** GPIO_EXT_ETM_EVENT_CH2_CFG_REG register
 *  ETM configuration register for channel 2
 */
#define GPIO_EXT_ETM_EVENT_CH2_CFG_REG (DR_REG_GPIO_BASE + 0x120)
/** GPIO_EXT_ETM_CH2_EVENT_SEL : R/W; bitpos: [5:0]; default: 0;
 *  Configures to select GPIO for ETM event channel.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  38: Select GPIO38
 *  39: Select GPIO39
 *  40 ~ 63: Reserved
 */
#define GPIO_EXT_ETM_CH2_EVENT_SEL    0x0000003FU
#define GPIO_EXT_ETM_CH2_EVENT_SEL_M  (GPIO_EXT_ETM_CH2_EVENT_SEL_V << GPIO_EXT_ETM_CH2_EVENT_SEL_S)
#define GPIO_EXT_ETM_CH2_EVENT_SEL_V  0x0000003FU
#define GPIO_EXT_ETM_CH2_EVENT_SEL_S  0
/** GPIO_EXT_ETM_CH2_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable ETM event send.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_CH2_EVENT_EN    (BIT(7))
#define GPIO_EXT_ETM_CH2_EVENT_EN_M  (GPIO_EXT_ETM_CH2_EVENT_EN_V << GPIO_EXT_ETM_CH2_EVENT_EN_S)
#define GPIO_EXT_ETM_CH2_EVENT_EN_V  0x00000001U
#define GPIO_EXT_ETM_CH2_EVENT_EN_S  7

/** GPIO_EXT_ETM_EVENT_CH3_CFG_REG register
 *  ETM configuration register for channel 3
 */
#define GPIO_EXT_ETM_EVENT_CH3_CFG_REG (DR_REG_GPIO_BASE + 0x124)
/** GPIO_EXT_ETM_CH3_EVENT_SEL : R/W; bitpos: [5:0]; default: 0;
 *  Configures to select GPIO for ETM event channel.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  38: Select GPIO38
 *  39: Select GPIO39
 *  40 ~ 63: Reserved
 */
#define GPIO_EXT_ETM_CH3_EVENT_SEL    0x0000003FU
#define GPIO_EXT_ETM_CH3_EVENT_SEL_M  (GPIO_EXT_ETM_CH3_EVENT_SEL_V << GPIO_EXT_ETM_CH3_EVENT_SEL_S)
#define GPIO_EXT_ETM_CH3_EVENT_SEL_V  0x0000003FU
#define GPIO_EXT_ETM_CH3_EVENT_SEL_S  0
/** GPIO_EXT_ETM_CH3_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable ETM event send.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_CH3_EVENT_EN    (BIT(7))
#define GPIO_EXT_ETM_CH3_EVENT_EN_M  (GPIO_EXT_ETM_CH3_EVENT_EN_V << GPIO_EXT_ETM_CH3_EVENT_EN_S)
#define GPIO_EXT_ETM_CH3_EVENT_EN_V  0x00000001U
#define GPIO_EXT_ETM_CH3_EVENT_EN_S  7

/** GPIO_EXT_ETM_EVENT_CH4_CFG_REG register
 *  ETM configuration register for channel 4
 */
#define GPIO_EXT_ETM_EVENT_CH4_CFG_REG (DR_REG_GPIO_BASE + 0x128)
/** GPIO_EXT_ETM_CH4_EVENT_SEL : R/W; bitpos: [5:0]; default: 0;
 *  Configures to select GPIO for ETM event channel.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  38: Select GPIO38
 *  39: Select GPIO39
 *  40 ~ 63: Reserved
 */
#define GPIO_EXT_ETM_CH4_EVENT_SEL    0x0000003FU
#define GPIO_EXT_ETM_CH4_EVENT_SEL_M  (GPIO_EXT_ETM_CH4_EVENT_SEL_V << GPIO_EXT_ETM_CH4_EVENT_SEL_S)
#define GPIO_EXT_ETM_CH4_EVENT_SEL_V  0x0000003FU
#define GPIO_EXT_ETM_CH4_EVENT_SEL_S  0
/** GPIO_EXT_ETM_CH4_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable ETM event send.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_CH4_EVENT_EN    (BIT(7))
#define GPIO_EXT_ETM_CH4_EVENT_EN_M  (GPIO_EXT_ETM_CH4_EVENT_EN_V << GPIO_EXT_ETM_CH4_EVENT_EN_S)
#define GPIO_EXT_ETM_CH4_EVENT_EN_V  0x00000001U
#define GPIO_EXT_ETM_CH4_EVENT_EN_S  7

/** GPIO_EXT_ETM_EVENT_CH5_CFG_REG register
 *  ETM configuration register for channel 5
 */
#define GPIO_EXT_ETM_EVENT_CH5_CFG_REG (DR_REG_GPIO_BASE + 0x12c)
/** GPIO_EXT_ETM_CH5_EVENT_SEL : R/W; bitpos: [5:0]; default: 0;
 *  Configures to select GPIO for ETM event channel.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  38: Select GPIO38
 *  39: Select GPIO39
 *  40 ~ 63: Reserved
 */
#define GPIO_EXT_ETM_CH5_EVENT_SEL    0x0000003FU
#define GPIO_EXT_ETM_CH5_EVENT_SEL_M  (GPIO_EXT_ETM_CH5_EVENT_SEL_V << GPIO_EXT_ETM_CH5_EVENT_SEL_S)
#define GPIO_EXT_ETM_CH5_EVENT_SEL_V  0x0000003FU
#define GPIO_EXT_ETM_CH5_EVENT_SEL_S  0
/** GPIO_EXT_ETM_CH5_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable ETM event send.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_CH5_EVENT_EN    (BIT(7))
#define GPIO_EXT_ETM_CH5_EVENT_EN_M  (GPIO_EXT_ETM_CH5_EVENT_EN_V << GPIO_EXT_ETM_CH5_EVENT_EN_S)
#define GPIO_EXT_ETM_CH5_EVENT_EN_V  0x00000001U
#define GPIO_EXT_ETM_CH5_EVENT_EN_S  7

/** GPIO_EXT_ETM_EVENT_CH6_CFG_REG register
 *  ETM configuration register for channel 6
 */
#define GPIO_EXT_ETM_EVENT_CH6_CFG_REG (DR_REG_GPIO_BASE + 0x130)
/** GPIO_EXT_ETM_CH6_EVENT_SEL : R/W; bitpos: [5:0]; default: 0;
 *  Configures to select GPIO for ETM event channel.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  38: Select GPIO38
 *  39: Select GPIO39
 *  40 ~ 63: Reserved
 */
#define GPIO_EXT_ETM_CH6_EVENT_SEL    0x0000003FU
#define GPIO_EXT_ETM_CH6_EVENT_SEL_M  (GPIO_EXT_ETM_CH6_EVENT_SEL_V << GPIO_EXT_ETM_CH6_EVENT_SEL_S)
#define GPIO_EXT_ETM_CH6_EVENT_SEL_V  0x0000003FU
#define GPIO_EXT_ETM_CH6_EVENT_SEL_S  0
/** GPIO_EXT_ETM_CH6_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable ETM event send.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_CH6_EVENT_EN    (BIT(7))
#define GPIO_EXT_ETM_CH6_EVENT_EN_M  (GPIO_EXT_ETM_CH6_EVENT_EN_V << GPIO_EXT_ETM_CH6_EVENT_EN_S)
#define GPIO_EXT_ETM_CH6_EVENT_EN_V  0x00000001U
#define GPIO_EXT_ETM_CH6_EVENT_EN_S  7

/** GPIO_EXT_ETM_EVENT_CH7_CFG_REG register
 *  ETM configuration register for channel 7
 */
#define GPIO_EXT_ETM_EVENT_CH7_CFG_REG (DR_REG_GPIO_BASE + 0x134)
/** GPIO_EXT_ETM_CH7_EVENT_SEL : R/W; bitpos: [5:0]; default: 0;
 *  Configures to select GPIO for ETM event channel.
 *  0: Select GPIO0
 *  1: Select GPIO1
 *  ......
 *  38: Select GPIO38
 *  39: Select GPIO39
 *  40 ~ 63: Reserved
 */
#define GPIO_EXT_ETM_CH7_EVENT_SEL    0x0000003FU
#define GPIO_EXT_ETM_CH7_EVENT_SEL_M  (GPIO_EXT_ETM_CH7_EVENT_SEL_V << GPIO_EXT_ETM_CH7_EVENT_SEL_S)
#define GPIO_EXT_ETM_CH7_EVENT_SEL_V  0x0000003FU
#define GPIO_EXT_ETM_CH7_EVENT_SEL_S  0
/** GPIO_EXT_ETM_CH7_EVENT_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable ETM event send.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_CH7_EVENT_EN    (BIT(7))
#define GPIO_EXT_ETM_CH7_EVENT_EN_M  (GPIO_EXT_ETM_CH7_EVENT_EN_V << GPIO_EXT_ETM_CH7_EVENT_EN_S)
#define GPIO_EXT_ETM_CH7_EVENT_EN_V  0x00000001U
#define GPIO_EXT_ETM_CH7_EVENT_EN_S  7

/** GPIO_EXT_ETM_TASK_P0_CFG_REG register
 *  GPIO selection register 0 for ETM
 */
#define GPIO_EXT_ETM_TASK_P0_CFG_REG (DR_REG_GPIO_BASE + 0x158)
/** GPIO_EXT_ETM_TASK_GPIO0_SEL : R/W; bitpos: [2:0]; default: 0;
 *  Configures to select an ETM task channel for GPIO0.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO0_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO0_SEL_M  (GPIO_EXT_ETM_TASK_GPIO0_SEL_V << GPIO_EXT_ETM_TASK_GPIO0_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO0_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO0_SEL_S  0
/** GPIO_EXT_ETM_TASK_GPIO0_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable GPIO0 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO0_EN    (BIT(5))
#define GPIO_EXT_ETM_TASK_GPIO0_EN_M  (GPIO_EXT_ETM_TASK_GPIO0_EN_V << GPIO_EXT_ETM_TASK_GPIO0_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO0_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO0_EN_S  5
/** GPIO_EXT_ETM_TASK_GPIO1_SEL : R/W; bitpos: [8:6]; default: 0;
 *  Configures to select an ETM task channel for GPIO1.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO1_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO1_SEL_M  (GPIO_EXT_ETM_TASK_GPIO1_SEL_V << GPIO_EXT_ETM_TASK_GPIO1_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO1_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO1_SEL_S  6
/** GPIO_EXT_ETM_TASK_GPIO1_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable GPIO1 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO1_EN    (BIT(11))
#define GPIO_EXT_ETM_TASK_GPIO1_EN_M  (GPIO_EXT_ETM_TASK_GPIO1_EN_V << GPIO_EXT_ETM_TASK_GPIO1_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO1_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO1_EN_S  11
/** GPIO_EXT_ETM_TASK_GPIO2_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select an ETM task channel for GPIO2.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO2_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO2_SEL_M  (GPIO_EXT_ETM_TASK_GPIO2_SEL_V << GPIO_EXT_ETM_TASK_GPIO2_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO2_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO2_SEL_S  12
/** GPIO_EXT_ETM_TASK_GPIO2_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable GPIO2 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO2_EN    (BIT(17))
#define GPIO_EXT_ETM_TASK_GPIO2_EN_M  (GPIO_EXT_ETM_TASK_GPIO2_EN_V << GPIO_EXT_ETM_TASK_GPIO2_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO2_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO2_EN_S  17
/** GPIO_EXT_ETM_TASK_GPIO3_SEL : R/W; bitpos: [20:18]; default: 0;
 *  Configures to select an ETM task channel for GPIO3.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO3_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO3_SEL_M  (GPIO_EXT_ETM_TASK_GPIO3_SEL_V << GPIO_EXT_ETM_TASK_GPIO3_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO3_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO3_SEL_S  18
/** GPIO_EXT_ETM_TASK_GPIO3_EN : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to enable GPIO3 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO3_EN    (BIT(23))
#define GPIO_EXT_ETM_TASK_GPIO3_EN_M  (GPIO_EXT_ETM_TASK_GPIO3_EN_V << GPIO_EXT_ETM_TASK_GPIO3_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO3_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO3_EN_S  23
/** GPIO_EXT_ETM_TASK_GPIO4_SEL : R/W; bitpos: [26:24]; default: 0;
 *  Configures to select an ETM task channel for GPIO4.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO4_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO4_SEL_M  (GPIO_EXT_ETM_TASK_GPIO4_SEL_V << GPIO_EXT_ETM_TASK_GPIO4_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO4_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO4_SEL_S  24
/** GPIO_EXT_ETM_TASK_GPIO4_EN : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to enable GPIO4 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO4_EN    (BIT(29))
#define GPIO_EXT_ETM_TASK_GPIO4_EN_M  (GPIO_EXT_ETM_TASK_GPIO4_EN_V << GPIO_EXT_ETM_TASK_GPIO4_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO4_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO4_EN_S  29

/** GPIO_EXT_ETM_TASK_P1_CFG_REG register
 *  GPIO selection register 1 for ETM
 */
#define GPIO_EXT_ETM_TASK_P1_CFG_REG (DR_REG_GPIO_BASE + 0x15c)
/** GPIO_EXT_ETM_TASK_GPIO5_SEL : R/W; bitpos: [2:0]; default: 0;
 *  Configures to select an ETM task channel for GPIO5.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO5_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO5_SEL_M  (GPIO_EXT_ETM_TASK_GPIO5_SEL_V << GPIO_EXT_ETM_TASK_GPIO5_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO5_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO5_SEL_S  0
/** GPIO_EXT_ETM_TASK_GPIO5_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable GPIO5 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO5_EN    (BIT(5))
#define GPIO_EXT_ETM_TASK_GPIO5_EN_M  (GPIO_EXT_ETM_TASK_GPIO5_EN_V << GPIO_EXT_ETM_TASK_GPIO5_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO5_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO5_EN_S  5
/** GPIO_EXT_ETM_TASK_GPIO6_SEL : R/W; bitpos: [8:6]; default: 0;
 *  Configures to select an ETM task channel for GPIO6.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO6_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO6_SEL_M  (GPIO_EXT_ETM_TASK_GPIO6_SEL_V << GPIO_EXT_ETM_TASK_GPIO6_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO6_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO6_SEL_S  6
/** GPIO_EXT_ETM_TASK_GPIO6_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable GPIO6 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO6_EN    (BIT(11))
#define GPIO_EXT_ETM_TASK_GPIO6_EN_M  (GPIO_EXT_ETM_TASK_GPIO6_EN_V << GPIO_EXT_ETM_TASK_GPIO6_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO6_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO6_EN_S  11
/** GPIO_EXT_ETM_TASK_GPIO7_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select an ETM task channel for GPIO7.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO7_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO7_SEL_M  (GPIO_EXT_ETM_TASK_GPIO7_SEL_V << GPIO_EXT_ETM_TASK_GPIO7_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO7_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO7_SEL_S  12
/** GPIO_EXT_ETM_TASK_GPIO7_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable GPIO7 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO7_EN    (BIT(17))
#define GPIO_EXT_ETM_TASK_GPIO7_EN_M  (GPIO_EXT_ETM_TASK_GPIO7_EN_V << GPIO_EXT_ETM_TASK_GPIO7_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO7_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO7_EN_S  17
/** GPIO_EXT_ETM_TASK_GPIO8_SEL : R/W; bitpos: [20:18]; default: 0;
 *  Configures to select an ETM task channel for GPIO8.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO8_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO8_SEL_M  (GPIO_EXT_ETM_TASK_GPIO8_SEL_V << GPIO_EXT_ETM_TASK_GPIO8_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO8_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO8_SEL_S  18
/** GPIO_EXT_ETM_TASK_GPIO8_EN : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to enable GPIO8 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO8_EN    (BIT(23))
#define GPIO_EXT_ETM_TASK_GPIO8_EN_M  (GPIO_EXT_ETM_TASK_GPIO8_EN_V << GPIO_EXT_ETM_TASK_GPIO8_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO8_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO8_EN_S  23
/** GPIO_EXT_ETM_TASK_GPIO9_SEL : R/W; bitpos: [26:24]; default: 0;
 *  Configures to select an ETM task channel for GPIO9.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO9_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO9_SEL_M  (GPIO_EXT_ETM_TASK_GPIO9_SEL_V << GPIO_EXT_ETM_TASK_GPIO9_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO9_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO9_SEL_S  24
/** GPIO_EXT_ETM_TASK_GPIO9_EN : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to enable GPIO9 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO9_EN    (BIT(29))
#define GPIO_EXT_ETM_TASK_GPIO9_EN_M  (GPIO_EXT_ETM_TASK_GPIO9_EN_V << GPIO_EXT_ETM_TASK_GPIO9_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO9_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO9_EN_S  29

/** GPIO_EXT_ETM_TASK_P2_CFG_REG register
 *  GPIO selection register 2 for ETM
 */
#define GPIO_EXT_ETM_TASK_P2_CFG_REG (DR_REG_GPIO_BASE + 0x160)
/** GPIO_EXT_ETM_TASK_GPIO10_SEL : R/W; bitpos: [2:0]; default: 0;
 *  Configures to select an ETM task channel for GPIO10.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO10_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO10_SEL_M  (GPIO_EXT_ETM_TASK_GPIO10_SEL_V << GPIO_EXT_ETM_TASK_GPIO10_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO10_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO10_SEL_S  0
/** GPIO_EXT_ETM_TASK_GPIO10_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable GPIO10 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO10_EN    (BIT(5))
#define GPIO_EXT_ETM_TASK_GPIO10_EN_M  (GPIO_EXT_ETM_TASK_GPIO10_EN_V << GPIO_EXT_ETM_TASK_GPIO10_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO10_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO10_EN_S  5
/** GPIO_EXT_ETM_TASK_GPIO11_SEL : R/W; bitpos: [8:6]; default: 0;
 *  Configures to select an ETM task channel for GPIO11.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO11_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO11_SEL_M  (GPIO_EXT_ETM_TASK_GPIO11_SEL_V << GPIO_EXT_ETM_TASK_GPIO11_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO11_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO11_SEL_S  6
/** GPIO_EXT_ETM_TASK_GPIO11_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable GPIO11 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO11_EN    (BIT(11))
#define GPIO_EXT_ETM_TASK_GPIO11_EN_M  (GPIO_EXT_ETM_TASK_GPIO11_EN_V << GPIO_EXT_ETM_TASK_GPIO11_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO11_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO11_EN_S  11
/** GPIO_EXT_ETM_TASK_GPIO12_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select an ETM task channel for GPIO12.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO12_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO12_SEL_M  (GPIO_EXT_ETM_TASK_GPIO12_SEL_V << GPIO_EXT_ETM_TASK_GPIO12_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO12_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO12_SEL_S  12
/** GPIO_EXT_ETM_TASK_GPIO12_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable GPIO12 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO12_EN    (BIT(17))
#define GPIO_EXT_ETM_TASK_GPIO12_EN_M  (GPIO_EXT_ETM_TASK_GPIO12_EN_V << GPIO_EXT_ETM_TASK_GPIO12_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO12_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO12_EN_S  17
/** GPIO_EXT_ETM_TASK_GPIO13_SEL : R/W; bitpos: [20:18]; default: 0;
 *  Configures to select an ETM task channel for GPIO13.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO13_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO13_SEL_M  (GPIO_EXT_ETM_TASK_GPIO13_SEL_V << GPIO_EXT_ETM_TASK_GPIO13_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO13_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO13_SEL_S  18
/** GPIO_EXT_ETM_TASK_GPIO13_EN : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to enable GPIO13 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO13_EN    (BIT(23))
#define GPIO_EXT_ETM_TASK_GPIO13_EN_M  (GPIO_EXT_ETM_TASK_GPIO13_EN_V << GPIO_EXT_ETM_TASK_GPIO13_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO13_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO13_EN_S  23
/** GPIO_EXT_ETM_TASK_GPIO14_SEL : R/W; bitpos: [26:24]; default: 0;
 *  Configures to select an ETM task channel for GPIO14.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO14_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO14_SEL_M  (GPIO_EXT_ETM_TASK_GPIO14_SEL_V << GPIO_EXT_ETM_TASK_GPIO14_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO14_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO14_SEL_S  24
/** GPIO_EXT_ETM_TASK_GPIO14_EN : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to enable GPIO14 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO14_EN    (BIT(29))
#define GPIO_EXT_ETM_TASK_GPIO14_EN_M  (GPIO_EXT_ETM_TASK_GPIO14_EN_V << GPIO_EXT_ETM_TASK_GPIO14_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO14_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO14_EN_S  29

/** GPIO_EXT_ETM_TASK_P3_CFG_REG register
 *  GPIO selection register 3 for ETM
 */
#define GPIO_EXT_ETM_TASK_P3_CFG_REG (DR_REG_GPIO_BASE + 0x164)
/** GPIO_EXT_ETM_TASK_GPIO15_SEL : R/W; bitpos: [2:0]; default: 0;
 *  Configures to select an ETM task channel for GPIO15.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO15_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO15_SEL_M  (GPIO_EXT_ETM_TASK_GPIO15_SEL_V << GPIO_EXT_ETM_TASK_GPIO15_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO15_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO15_SEL_S  0
/** GPIO_EXT_ETM_TASK_GPIO15_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable GPIO15 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO15_EN    (BIT(5))
#define GPIO_EXT_ETM_TASK_GPIO15_EN_M  (GPIO_EXT_ETM_TASK_GPIO15_EN_V << GPIO_EXT_ETM_TASK_GPIO15_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO15_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO15_EN_S  5
/** GPIO_EXT_ETM_TASK_GPIO16_SEL : R/W; bitpos: [8:6]; default: 0;
 *  Configures to select an ETM task channel for GPIO16.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO16_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO16_SEL_M  (GPIO_EXT_ETM_TASK_GPIO16_SEL_V << GPIO_EXT_ETM_TASK_GPIO16_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO16_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO16_SEL_S  6
/** GPIO_EXT_ETM_TASK_GPIO16_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable GPIO16 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO16_EN    (BIT(11))
#define GPIO_EXT_ETM_TASK_GPIO16_EN_M  (GPIO_EXT_ETM_TASK_GPIO16_EN_V << GPIO_EXT_ETM_TASK_GPIO16_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO16_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO16_EN_S  11
/** GPIO_EXT_ETM_TASK_GPIO17_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select an ETM task channel for GPIO17.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO17_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO17_SEL_M  (GPIO_EXT_ETM_TASK_GPIO17_SEL_V << GPIO_EXT_ETM_TASK_GPIO17_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO17_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO17_SEL_S  12
/** GPIO_EXT_ETM_TASK_GPIO17_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable GPIO17 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO17_EN    (BIT(17))
#define GPIO_EXT_ETM_TASK_GPIO17_EN_M  (GPIO_EXT_ETM_TASK_GPIO17_EN_V << GPIO_EXT_ETM_TASK_GPIO17_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO17_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO17_EN_S  17
/** GPIO_EXT_ETM_TASK_GPIO18_SEL : R/W; bitpos: [20:18]; default: 0;
 *  Configures to select an ETM task channel for GPIO18.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO18_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO18_SEL_M  (GPIO_EXT_ETM_TASK_GPIO18_SEL_V << GPIO_EXT_ETM_TASK_GPIO18_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO18_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO18_SEL_S  18
/** GPIO_EXT_ETM_TASK_GPIO18_EN : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to enable GPIO18 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO18_EN    (BIT(23))
#define GPIO_EXT_ETM_TASK_GPIO18_EN_M  (GPIO_EXT_ETM_TASK_GPIO18_EN_V << GPIO_EXT_ETM_TASK_GPIO18_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO18_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO18_EN_S  23
/** GPIO_EXT_ETM_TASK_GPIO19_SEL : R/W; bitpos: [26:24]; default: 0;
 *  Configures to select an ETM task channel for GPIO19.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO19_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO19_SEL_M  (GPIO_EXT_ETM_TASK_GPIO19_SEL_V << GPIO_EXT_ETM_TASK_GPIO19_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO19_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO19_SEL_S  24
/** GPIO_EXT_ETM_TASK_GPIO19_EN : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to enable GPIO19 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO19_EN    (BIT(29))
#define GPIO_EXT_ETM_TASK_GPIO19_EN_M  (GPIO_EXT_ETM_TASK_GPIO19_EN_V << GPIO_EXT_ETM_TASK_GPIO19_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO19_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO19_EN_S  29

/** GPIO_EXT_ETM_TASK_P4_CFG_REG register
 *  GPIO selection register 4 for ETM
 */
#define GPIO_EXT_ETM_TASK_P4_CFG_REG (DR_REG_GPIO_BASE + 0x168)
/** GPIO_EXT_ETM_TASK_GPIO20_SEL : R/W; bitpos: [2:0]; default: 0;
 *  Configures to select an ETM task channel for GPIO20.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO20_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO20_SEL_M  (GPIO_EXT_ETM_TASK_GPIO20_SEL_V << GPIO_EXT_ETM_TASK_GPIO20_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO20_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO20_SEL_S  0
/** GPIO_EXT_ETM_TASK_GPIO20_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable GPIO20 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO20_EN    (BIT(5))
#define GPIO_EXT_ETM_TASK_GPIO20_EN_M  (GPIO_EXT_ETM_TASK_GPIO20_EN_V << GPIO_EXT_ETM_TASK_GPIO20_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO20_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO20_EN_S  5
/** GPIO_EXT_ETM_TASK_GPIO21_SEL : R/W; bitpos: [8:6]; default: 0;
 *  Configures to select an ETM task channel for GPIO21.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO21_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO21_SEL_M  (GPIO_EXT_ETM_TASK_GPIO21_SEL_V << GPIO_EXT_ETM_TASK_GPIO21_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO21_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO21_SEL_S  6
/** GPIO_EXT_ETM_TASK_GPIO21_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable GPIO21 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO21_EN    (BIT(11))
#define GPIO_EXT_ETM_TASK_GPIO21_EN_M  (GPIO_EXT_ETM_TASK_GPIO21_EN_V << GPIO_EXT_ETM_TASK_GPIO21_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO21_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO21_EN_S  11
/** GPIO_EXT_ETM_TASK_GPIO22_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select an ETM task channel for GPIO22.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO22_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO22_SEL_M  (GPIO_EXT_ETM_TASK_GPIO22_SEL_V << GPIO_EXT_ETM_TASK_GPIO22_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO22_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO22_SEL_S  12
/** GPIO_EXT_ETM_TASK_GPIO22_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable GPIO22 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO22_EN    (BIT(17))
#define GPIO_EXT_ETM_TASK_GPIO22_EN_M  (GPIO_EXT_ETM_TASK_GPIO22_EN_V << GPIO_EXT_ETM_TASK_GPIO22_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO22_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO22_EN_S  17
/** GPIO_EXT_ETM_TASK_GPIO23_SEL : R/W; bitpos: [20:18]; default: 0;
 *  Configures to select an ETM task channel for GPIO23.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO23_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO23_SEL_M  (GPIO_EXT_ETM_TASK_GPIO23_SEL_V << GPIO_EXT_ETM_TASK_GPIO23_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO23_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO23_SEL_S  18
/** GPIO_EXT_ETM_TASK_GPIO23_EN : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to enable GPIO23 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO23_EN    (BIT(23))
#define GPIO_EXT_ETM_TASK_GPIO23_EN_M  (GPIO_EXT_ETM_TASK_GPIO23_EN_V << GPIO_EXT_ETM_TASK_GPIO23_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO23_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO23_EN_S  23
/** GPIO_EXT_ETM_TASK_GPIO24_SEL : R/W; bitpos: [26:24]; default: 0;
 *  Configures to select an ETM task channel for GPIO24.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO24_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO24_SEL_M  (GPIO_EXT_ETM_TASK_GPIO24_SEL_V << GPIO_EXT_ETM_TASK_GPIO24_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO24_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO24_SEL_S  24
/** GPIO_EXT_ETM_TASK_GPIO24_EN : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to enable GPIO24 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO24_EN    (BIT(29))
#define GPIO_EXT_ETM_TASK_GPIO24_EN_M  (GPIO_EXT_ETM_TASK_GPIO24_EN_V << GPIO_EXT_ETM_TASK_GPIO24_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO24_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO24_EN_S  29

/** GPIO_EXT_ETM_TASK_P5_CFG_REG register
 *  GPIO selection register 5 for ETM
 */
#define GPIO_EXT_ETM_TASK_P5_CFG_REG (DR_REG_GPIO_BASE + 0x16c)
/** GPIO_EXT_ETM_TASK_GPIO25_SEL : R/W; bitpos: [2:0]; default: 0;
 *  Configures to select an ETM task channel for GPIO25.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO25_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO25_SEL_M  (GPIO_EXT_ETM_TASK_GPIO25_SEL_V << GPIO_EXT_ETM_TASK_GPIO25_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO25_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO25_SEL_S  0
/** GPIO_EXT_ETM_TASK_GPIO25_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable GPIO25 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO25_EN    (BIT(5))
#define GPIO_EXT_ETM_TASK_GPIO25_EN_M  (GPIO_EXT_ETM_TASK_GPIO25_EN_V << GPIO_EXT_ETM_TASK_GPIO25_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO25_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO25_EN_S  5
/** GPIO_EXT_ETM_TASK_GPIO26_SEL : R/W; bitpos: [8:6]; default: 0;
 *  Configures to select an ETM task channel for GPIO26.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO26_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO26_SEL_M  (GPIO_EXT_ETM_TASK_GPIO26_SEL_V << GPIO_EXT_ETM_TASK_GPIO26_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO26_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO26_SEL_S  6
/** GPIO_EXT_ETM_TASK_GPIO26_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable GPIO26 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO26_EN    (BIT(11))
#define GPIO_EXT_ETM_TASK_GPIO26_EN_M  (GPIO_EXT_ETM_TASK_GPIO26_EN_V << GPIO_EXT_ETM_TASK_GPIO26_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO26_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO26_EN_S  11
/** GPIO_EXT_ETM_TASK_GPIO27_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select an ETM task channel for GPIO27.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO27_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO27_SEL_M  (GPIO_EXT_ETM_TASK_GPIO27_SEL_V << GPIO_EXT_ETM_TASK_GPIO27_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO27_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO27_SEL_S  12
/** GPIO_EXT_ETM_TASK_GPIO27_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable GPIO27 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO27_EN    (BIT(17))
#define GPIO_EXT_ETM_TASK_GPIO27_EN_M  (GPIO_EXT_ETM_TASK_GPIO27_EN_V << GPIO_EXT_ETM_TASK_GPIO27_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO27_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO27_EN_S  17
/** GPIO_EXT_ETM_TASK_GPIO28_SEL : R/W; bitpos: [20:18]; default: 0;
 *  Configures to select an ETM task channel for GPIO28.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO28_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO28_SEL_M  (GPIO_EXT_ETM_TASK_GPIO28_SEL_V << GPIO_EXT_ETM_TASK_GPIO28_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO28_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO28_SEL_S  18
/** GPIO_EXT_ETM_TASK_GPIO28_EN : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to enable GPIO28 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO28_EN    (BIT(23))
#define GPIO_EXT_ETM_TASK_GPIO28_EN_M  (GPIO_EXT_ETM_TASK_GPIO28_EN_V << GPIO_EXT_ETM_TASK_GPIO28_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO28_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO28_EN_S  23
/** GPIO_EXT_ETM_TASK_GPIO29_SEL : R/W; bitpos: [26:24]; default: 0;
 *  Configures to select an ETM task channel for GPIO29.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO29_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO29_SEL_M  (GPIO_EXT_ETM_TASK_GPIO29_SEL_V << GPIO_EXT_ETM_TASK_GPIO29_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO29_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO29_SEL_S  24
/** GPIO_EXT_ETM_TASK_GPIO29_EN : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to enable GPIO29 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO29_EN    (BIT(29))
#define GPIO_EXT_ETM_TASK_GPIO29_EN_M  (GPIO_EXT_ETM_TASK_GPIO29_EN_V << GPIO_EXT_ETM_TASK_GPIO29_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO29_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO29_EN_S  29

/** GPIO_EXT_ETM_TASK_P6_CFG_REG register
 *  GPIO selection register 6 for ETM
 */
#define GPIO_EXT_ETM_TASK_P6_CFG_REG (DR_REG_GPIO_BASE + 0x170)
/** GPIO_EXT_ETM_TASK_GPIO30_SEL : R/W; bitpos: [2:0]; default: 0;
 *  Configures to select an ETM task channel for GPIO30.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO30_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO30_SEL_M  (GPIO_EXT_ETM_TASK_GPIO30_SEL_V << GPIO_EXT_ETM_TASK_GPIO30_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO30_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO30_SEL_S  0
/** GPIO_EXT_ETM_TASK_GPIO30_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable GPIO30 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO30_EN    (BIT(5))
#define GPIO_EXT_ETM_TASK_GPIO30_EN_M  (GPIO_EXT_ETM_TASK_GPIO30_EN_V << GPIO_EXT_ETM_TASK_GPIO30_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO30_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO30_EN_S  5
/** GPIO_EXT_ETM_TASK_GPIO31_SEL : R/W; bitpos: [8:6]; default: 0;
 *  Configures to select an ETM task channel for GPIO31.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO31_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO31_SEL_M  (GPIO_EXT_ETM_TASK_GPIO31_SEL_V << GPIO_EXT_ETM_TASK_GPIO31_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO31_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO31_SEL_S  6
/** GPIO_EXT_ETM_TASK_GPIO31_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable GPIO31 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO31_EN    (BIT(11))
#define GPIO_EXT_ETM_TASK_GPIO31_EN_M  (GPIO_EXT_ETM_TASK_GPIO31_EN_V << GPIO_EXT_ETM_TASK_GPIO31_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO31_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO31_EN_S  11
/** GPIO_EXT_ETM_TASK_GPIO32_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select an ETM task channel for GPIO32.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO32_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO32_SEL_M  (GPIO_EXT_ETM_TASK_GPIO32_SEL_V << GPIO_EXT_ETM_TASK_GPIO32_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO32_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO32_SEL_S  12
/** GPIO_EXT_ETM_TASK_GPIO32_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable GPIO32 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO32_EN    (BIT(17))
#define GPIO_EXT_ETM_TASK_GPIO32_EN_M  (GPIO_EXT_ETM_TASK_GPIO32_EN_V << GPIO_EXT_ETM_TASK_GPIO32_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO32_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO32_EN_S  17
/** GPIO_EXT_ETM_TASK_GPIO33_SEL : R/W; bitpos: [20:18]; default: 0;
 *  Configures to select an ETM task channel for GPIO33.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO33_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO33_SEL_M  (GPIO_EXT_ETM_TASK_GPIO33_SEL_V << GPIO_EXT_ETM_TASK_GPIO33_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO33_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO33_SEL_S  18
/** GPIO_EXT_ETM_TASK_GPIO33_EN : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to enable GPIO33 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO33_EN    (BIT(23))
#define GPIO_EXT_ETM_TASK_GPIO33_EN_M  (GPIO_EXT_ETM_TASK_GPIO33_EN_V << GPIO_EXT_ETM_TASK_GPIO33_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO33_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO33_EN_S  23
/** GPIO_EXT_ETM_TASK_GPIO34_SEL : R/W; bitpos: [26:24]; default: 0;
 *  Configures to select an ETM task channel for GPIO34.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO34_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO34_SEL_M  (GPIO_EXT_ETM_TASK_GPIO34_SEL_V << GPIO_EXT_ETM_TASK_GPIO34_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO34_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO34_SEL_S  24
/** GPIO_EXT_ETM_TASK_GPIO34_EN : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to enable GPIO34 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO34_EN    (BIT(29))
#define GPIO_EXT_ETM_TASK_GPIO34_EN_M  (GPIO_EXT_ETM_TASK_GPIO34_EN_V << GPIO_EXT_ETM_TASK_GPIO34_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO34_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO34_EN_S  29

/** GPIO_EXT_ETM_TASK_P7_CFG_REG register
 *  GPIO selection register 7 for ETM
 */
#define GPIO_EXT_ETM_TASK_P7_CFG_REG (DR_REG_GPIO_BASE + 0x174)
/** GPIO_EXT_ETM_TASK_GPIO35_SEL : R/W; bitpos: [2:0]; default: 0;
 *  Configures to select an ETM task channel for GPIO35.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO35_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO35_SEL_M  (GPIO_EXT_ETM_TASK_GPIO35_SEL_V << GPIO_EXT_ETM_TASK_GPIO35_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO35_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO35_SEL_S  0
/** GPIO_EXT_ETM_TASK_GPIO35_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable GPIO35 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO35_EN    (BIT(5))
#define GPIO_EXT_ETM_TASK_GPIO35_EN_M  (GPIO_EXT_ETM_TASK_GPIO35_EN_V << GPIO_EXT_ETM_TASK_GPIO35_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO35_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO35_EN_S  5
/** GPIO_EXT_ETM_TASK_GPIO36_SEL : R/W; bitpos: [8:6]; default: 0;
 *  Configures to select an ETM task channel for GPIO36.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO36_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO36_SEL_M  (GPIO_EXT_ETM_TASK_GPIO36_SEL_V << GPIO_EXT_ETM_TASK_GPIO36_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO36_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO36_SEL_S  6
/** GPIO_EXT_ETM_TASK_GPIO36_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable GPIO36 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO36_EN    (BIT(11))
#define GPIO_EXT_ETM_TASK_GPIO36_EN_M  (GPIO_EXT_ETM_TASK_GPIO36_EN_V << GPIO_EXT_ETM_TASK_GPIO36_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO36_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO36_EN_S  11
/** GPIO_EXT_ETM_TASK_GPIO37_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select an ETM task channel for GPIO37.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO37_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO37_SEL_M  (GPIO_EXT_ETM_TASK_GPIO37_SEL_V << GPIO_EXT_ETM_TASK_GPIO37_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO37_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO37_SEL_S  12
/** GPIO_EXT_ETM_TASK_GPIO37_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable GPIO37 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO37_EN    (BIT(17))
#define GPIO_EXT_ETM_TASK_GPIO37_EN_M  (GPIO_EXT_ETM_TASK_GPIO37_EN_V << GPIO_EXT_ETM_TASK_GPIO37_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO37_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO37_EN_S  17
/** GPIO_EXT_ETM_TASK_GPIO38_SEL : R/W; bitpos: [20:18]; default: 0;
 *  Configures to select an ETM task channel for GPIO38.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO38_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO38_SEL_M  (GPIO_EXT_ETM_TASK_GPIO38_SEL_V << GPIO_EXT_ETM_TASK_GPIO38_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO38_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO38_SEL_S  18
/** GPIO_EXT_ETM_TASK_GPIO38_EN : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to enable GPIO38 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO38_EN    (BIT(23))
#define GPIO_EXT_ETM_TASK_GPIO38_EN_M  (GPIO_EXT_ETM_TASK_GPIO38_EN_V << GPIO_EXT_ETM_TASK_GPIO38_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO38_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO38_EN_S  23
/** GPIO_EXT_ETM_TASK_GPIO39_SEL : R/W; bitpos: [26:24]; default: 0;
 *  Configures to select an ETM task channel for GPIO39.
 *  0: Select channel 0
 *  1: Select channel 1
 *  ......
 *  7: Select channel 7
 */
#define GPIO_EXT_ETM_TASK_GPIO39_SEL    0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO39_SEL_M  (GPIO_EXT_ETM_TASK_GPIO39_SEL_V << GPIO_EXT_ETM_TASK_GPIO39_SEL_S)
#define GPIO_EXT_ETM_TASK_GPIO39_SEL_V  0x00000007U
#define GPIO_EXT_ETM_TASK_GPIO39_SEL_S  24
/** GPIO_EXT_ETM_TASK_GPIO39_EN : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to enable GPIO39 to response ETM task.
 *  0: Not enable
 *  1: Enable
 */
#define GPIO_EXT_ETM_TASK_GPIO39_EN    (BIT(29))
#define GPIO_EXT_ETM_TASK_GPIO39_EN_M  (GPIO_EXT_ETM_TASK_GPIO39_EN_V << GPIO_EXT_ETM_TASK_GPIO39_EN_S)
#define GPIO_EXT_ETM_TASK_GPIO39_EN_V  0x00000001U
#define GPIO_EXT_ETM_TASK_GPIO39_EN_S  29

/** GPIO_EXT_VERSION_REG register
 *  Version control register
 */
#define GPIO_EXT_VERSION_REG (DR_REG_GPIO_BASE + 0x1fc)
/** GPIO_EXT_DATE : R/W; bitpos: [27:0]; default: 37818704;
 *  Version control register.
 */
#define GPIO_EXT_DATE    0x0FFFFFFFU
#define GPIO_EXT_DATE_M  (GPIO_EXT_DATE_V << GPIO_EXT_DATE_S)
#define GPIO_EXT_DATE_V  0x0FFFFFFFU
#define GPIO_EXT_DATE_S  0

#ifdef __cplusplus
}
#endif
