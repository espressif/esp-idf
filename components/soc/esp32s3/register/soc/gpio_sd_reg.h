/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_SIGMADELTA0_REG          (DR_REG_GPIO_SD_BASE + 0x0)
/* GPIO_SD0_PRESCALE : R/W ;bitpos:[15:8] ;default: 8'hff ; */
/*description: .*/
#define GPIO_SD0_PRESCALE    0x000000FF
#define GPIO_SD0_PRESCALE_M  ((GPIO_SD0_PRESCALE_V)<<(GPIO_SD0_PRESCALE_S))
#define GPIO_SD0_PRESCALE_V  0xFF
#define GPIO_SD0_PRESCALE_S  8
/* GPIO_SD0_IN : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: .*/
#define GPIO_SD0_IN    0x000000FF
#define GPIO_SD0_IN_M  ((GPIO_SD0_IN_V)<<(GPIO_SD0_IN_S))
#define GPIO_SD0_IN_V  0xFF
#define GPIO_SD0_IN_S  0

#define GPIO_SIGMADELTA1_REG          (DR_REG_GPIO_SD_BASE + 0x4)
/* GPIO_SD1_PRESCALE : R/W ;bitpos:[15:8] ;default: 8'hff ; */
/*description: .*/
#define GPIO_SD1_PRESCALE    0x000000FF
#define GPIO_SD1_PRESCALE_M  ((GPIO_SD1_PRESCALE_V)<<(GPIO_SD1_PRESCALE_S))
#define GPIO_SD1_PRESCALE_V  0xFF
#define GPIO_SD1_PRESCALE_S  8
/* GPIO_SD1_IN : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: .*/
#define GPIO_SD1_IN    0x000000FF
#define GPIO_SD1_IN_M  ((GPIO_SD1_IN_V)<<(GPIO_SD1_IN_S))
#define GPIO_SD1_IN_V  0xFF
#define GPIO_SD1_IN_S  0

#define GPIO_SIGMADELTA2_REG          (DR_REG_GPIO_SD_BASE + 0x8)
/* GPIO_SD2_PRESCALE : R/W ;bitpos:[15:8] ;default: 8'hff ; */
/*description: .*/
#define GPIO_SD2_PRESCALE    0x000000FF
#define GPIO_SD2_PRESCALE_M  ((GPIO_SD2_PRESCALE_V)<<(GPIO_SD2_PRESCALE_S))
#define GPIO_SD2_PRESCALE_V  0xFF
#define GPIO_SD2_PRESCALE_S  8
/* GPIO_SD2_IN : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: .*/
#define GPIO_SD2_IN    0x000000FF
#define GPIO_SD2_IN_M  ((GPIO_SD2_IN_V)<<(GPIO_SD2_IN_S))
#define GPIO_SD2_IN_V  0xFF
#define GPIO_SD2_IN_S  0

#define GPIO_SIGMADELTA3_REG          (DR_REG_GPIO_SD_BASE + 0xC)
/* GPIO_SD3_PRESCALE : R/W ;bitpos:[15:8] ;default: 8'hff ; */
/*description: .*/
#define GPIO_SD3_PRESCALE    0x000000FF
#define GPIO_SD3_PRESCALE_M  ((GPIO_SD3_PRESCALE_V)<<(GPIO_SD3_PRESCALE_S))
#define GPIO_SD3_PRESCALE_V  0xFF
#define GPIO_SD3_PRESCALE_S  8
/* GPIO_SD3_IN : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: .*/
#define GPIO_SD3_IN    0x000000FF
#define GPIO_SD3_IN_M  ((GPIO_SD3_IN_V)<<(GPIO_SD3_IN_S))
#define GPIO_SD3_IN_V  0xFF
#define GPIO_SD3_IN_S  0

#define GPIO_SIGMADELTA4_REG          (DR_REG_GPIO_SD_BASE + 0x10)
/* GPIO_SD4_PRESCALE : R/W ;bitpos:[15:8] ;default: 8'hff ; */
/*description: .*/
#define GPIO_SD4_PRESCALE    0x000000FF
#define GPIO_SD4_PRESCALE_M  ((GPIO_SD4_PRESCALE_V)<<(GPIO_SD4_PRESCALE_S))
#define GPIO_SD4_PRESCALE_V  0xFF
#define GPIO_SD4_PRESCALE_S  8
/* GPIO_SD4_IN : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: .*/
#define GPIO_SD4_IN    0x000000FF
#define GPIO_SD4_IN_M  ((GPIO_SD4_IN_V)<<(GPIO_SD4_IN_S))
#define GPIO_SD4_IN_V  0xFF
#define GPIO_SD4_IN_S  0

#define GPIO_SIGMADELTA5_REG          (DR_REG_GPIO_SD_BASE + 0x14)
/* GPIO_SD5_PRESCALE : R/W ;bitpos:[15:8] ;default: 8'hff ; */
/*description: .*/
#define GPIO_SD5_PRESCALE    0x000000FF
#define GPIO_SD5_PRESCALE_M  ((GPIO_SD5_PRESCALE_V)<<(GPIO_SD5_PRESCALE_S))
#define GPIO_SD5_PRESCALE_V  0xFF
#define GPIO_SD5_PRESCALE_S  8
/* GPIO_SD5_IN : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: .*/
#define GPIO_SD5_IN    0x000000FF
#define GPIO_SD5_IN_M  ((GPIO_SD5_IN_V)<<(GPIO_SD5_IN_S))
#define GPIO_SD5_IN_V  0xFF
#define GPIO_SD5_IN_S  0

#define GPIO_SIGMADELTA6_REG          (DR_REG_GPIO_SD_BASE + 0x18)
/* GPIO_SD6_PRESCALE : R/W ;bitpos:[15:8] ;default: 8'hff ; */
/*description: .*/
#define GPIO_SD6_PRESCALE    0x000000FF
#define GPIO_SD6_PRESCALE_M  ((GPIO_SD6_PRESCALE_V)<<(GPIO_SD6_PRESCALE_S))
#define GPIO_SD6_PRESCALE_V  0xFF
#define GPIO_SD6_PRESCALE_S  8
/* GPIO_SD6_IN : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: .*/
#define GPIO_SD6_IN    0x000000FF
#define GPIO_SD6_IN_M  ((GPIO_SD6_IN_V)<<(GPIO_SD6_IN_S))
#define GPIO_SD6_IN_V  0xFF
#define GPIO_SD6_IN_S  0

#define GPIO_SIGMADELTA7_REG          (DR_REG_GPIO_SD_BASE + 0x1C)
/* GPIO_SD7_PRESCALE : R/W ;bitpos:[15:8] ;default: 8'hff ; */
/*description: .*/
#define GPIO_SD7_PRESCALE    0x000000FF
#define GPIO_SD7_PRESCALE_M  ((GPIO_SD7_PRESCALE_V)<<(GPIO_SD7_PRESCALE_S))
#define GPIO_SD7_PRESCALE_V  0xFF
#define GPIO_SD7_PRESCALE_S  8
/* GPIO_SD7_IN : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: .*/
#define GPIO_SD7_IN    0x000000FF
#define GPIO_SD7_IN_M  ((GPIO_SD7_IN_V)<<(GPIO_SD7_IN_S))
#define GPIO_SD7_IN_V  0xFF
#define GPIO_SD7_IN_S  0

#define GPIO_SIGMADELTA_CG_REG          (DR_REG_GPIO_SD_BASE + 0x20)
/* GPIO_SD_CLK_EN : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: .*/
#define GPIO_SD_CLK_EN    (BIT(31))
#define GPIO_SD_CLK_EN_M  (BIT(31))
#define GPIO_SD_CLK_EN_V  0x1
#define GPIO_SD_CLK_EN_S  31

#define GPIO_SIGMADELTA_MISC_REG          (DR_REG_GPIO_SD_BASE + 0x24)
/* GPIO_SPI_SWAP : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: .*/
#define GPIO_SPI_SWAP    (BIT(31))
#define GPIO_SPI_SWAP_M  (BIT(31))
#define GPIO_SPI_SWAP_V  0x1
#define GPIO_SPI_SWAP_S  31
/* GPIO_FUNCTION_CLK_EN : R/W ;bitpos:[30] ;default: 1'd0 ; */
/*description: .*/
#define GPIO_FUNCTION_CLK_EN    (BIT(30))
#define GPIO_FUNCTION_CLK_EN_M  (BIT(30))
#define GPIO_FUNCTION_CLK_EN_V  0x1
#define GPIO_FUNCTION_CLK_EN_S  30

#define GPIO_SIGMADELTA_VERSION_REG          (DR_REG_GPIO_SD_BASE + 0x28)
/* GPIO_SD_DATE : R/W ;bitpos:[27:0] ;default: 28'h1802260 ; */
/*description: .*/
#define GPIO_SD_DATE    0x0FFFFFFF
#define GPIO_SD_DATE_M  ((GPIO_SD_DATE_V)<<(GPIO_SD_DATE_S))
#define GPIO_SD_DATE_V  0xFFFFFFF
#define GPIO_SD_DATE_S  0

#ifdef __cplusplus
}
#endif
