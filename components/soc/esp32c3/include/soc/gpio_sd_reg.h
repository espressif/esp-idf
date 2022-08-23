/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc.h"

#ifdef __cplusplus
extern "C" {
#endif
#define GPIO_SIGMADELTA0_REG          (DR_REG_GPIO_SD_BASE + 0x0000)
/* GPIO_SD0_PRESCALE : R/W ;bitpos:[15:8] ;default: 8'hff ; */
/*description: */
#define GPIO_SD0_PRESCALE  0x000000FF
#define GPIO_SD0_PRESCALE_M  ((GPIO_SD0_PRESCALE_V)<<(GPIO_SD0_PRESCALE_S))
#define GPIO_SD0_PRESCALE_V  0xFF
#define GPIO_SD0_PRESCALE_S  8
/* GPIO_SD0_IN : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define GPIO_SD0_IN  0x000000FF
#define GPIO_SD0_IN_M  ((GPIO_SD0_IN_V)<<(GPIO_SD0_IN_S))
#define GPIO_SD0_IN_V  0xFF
#define GPIO_SD0_IN_S  0

#define GPIO_SIGMADELTA1_REG          (DR_REG_GPIO_SD_BASE + 0x0004)
/* GPIO_SD1_PRESCALE : R/W ;bitpos:[15:8] ;default: 8'hff ; */
/*description: */
#define GPIO_SD1_PRESCALE  0x000000FF
#define GPIO_SD1_PRESCALE_M  ((GPIO_SD1_PRESCALE_V)<<(GPIO_SD1_PRESCALE_S))
#define GPIO_SD1_PRESCALE_V  0xFF
#define GPIO_SD1_PRESCALE_S  8
/* GPIO_SD1_IN : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define GPIO_SD1_IN  0x000000FF
#define GPIO_SD1_IN_M  ((GPIO_SD1_IN_V)<<(GPIO_SD1_IN_S))
#define GPIO_SD1_IN_V  0xFF
#define GPIO_SD1_IN_S  0

#define GPIO_SIGMADELTA2_REG          (DR_REG_GPIO_SD_BASE + 0x0008)
/* GPIO_SD2_PRESCALE : R/W ;bitpos:[15:8] ;default: 8'hff ; */
/*description: */
#define GPIO_SD2_PRESCALE  0x000000FF
#define GPIO_SD2_PRESCALE_M  ((GPIO_SD2_PRESCALE_V)<<(GPIO_SD2_PRESCALE_S))
#define GPIO_SD2_PRESCALE_V  0xFF
#define GPIO_SD2_PRESCALE_S  8
/* GPIO_SD2_IN : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define GPIO_SD2_IN  0x000000FF
#define GPIO_SD2_IN_M  ((GPIO_SD2_IN_V)<<(GPIO_SD2_IN_S))
#define GPIO_SD2_IN_V  0xFF
#define GPIO_SD2_IN_S  0

#define GPIO_SIGMADELTA3_REG          (DR_REG_GPIO_SD_BASE + 0x000c)
/* GPIO_SD3_PRESCALE : R/W ;bitpos:[15:8] ;default: 8'hff ; */
/*description: */
#define GPIO_SD3_PRESCALE  0x000000FF
#define GPIO_SD3_PRESCALE_M  ((GPIO_SD3_PRESCALE_V)<<(GPIO_SD3_PRESCALE_S))
#define GPIO_SD3_PRESCALE_V  0xFF
#define GPIO_SD3_PRESCALE_S  8
/* GPIO_SD3_IN : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define GPIO_SD3_IN  0x000000FF
#define GPIO_SD3_IN_M  ((GPIO_SD3_IN_V)<<(GPIO_SD3_IN_S))
#define GPIO_SD3_IN_V  0xFF
#define GPIO_SD3_IN_S  0

#define GPIO_SIGMADELTA_CG_REG          (DR_REG_GPIO_SD_BASE + 0x0020)
/* GPIO_SD_CLK_EN : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define GPIO_SD_CLK_EN  (BIT(31))
#define GPIO_SD_CLK_EN_M  (BIT(31))
#define GPIO_SD_CLK_EN_V  0x1
#define GPIO_SD_CLK_EN_S  31

#define GPIO_SIGMADELTA_MISC_REG          (DR_REG_GPIO_SD_BASE + 0x0024)
/* GPIO_SPI_SWAP : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define GPIO_SPI_SWAP  (BIT(31))
#define GPIO_SPI_SWAP_M  (BIT(31))
#define GPIO_SPI_SWAP_V  0x1
#define GPIO_SPI_SWAP_S  31
/* GPIO_FUNCTION_CLK_EN : R/W ;bitpos:[30] ;default: 1'd0 ; */
/*description: */
#define GPIO_FUNCTION_CLK_EN  (BIT(30))
#define GPIO_FUNCTION_CLK_EN_M  (BIT(30))
#define GPIO_FUNCTION_CLK_EN_V  0x1
#define GPIO_FUNCTION_CLK_EN_S  30

#define GPIO_SIGMADELTA_VERSION_REG          (DR_REG_GPIO_SD_BASE + 0x0028)
/* GPIO_SD_DATE : R/W ;bitpos:[27:0] ;default: 28'h2006230 ; */
/*description: */
#define GPIO_SD_DATE  0x0FFFFFFF
#define GPIO_SD_DATE_M  ((GPIO_SD_DATE_V)<<(GPIO_SD_DATE_S))
#define GPIO_SD_DATE_V  0xFFFFFFF
#define GPIO_SD_DATE_S  0

#ifdef __cplusplus
}
#endif
