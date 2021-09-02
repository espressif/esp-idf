/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_GPIO_SD_REG_H_
#define _SOC_GPIO_SD_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"

#define GPIO_SIGMADELTA0_REG          (DR_REG_GPIO_SD_BASE + 0x0)
/* GPIO_SD0_PRESCALE : R/W ;bitpos:[15:8] ;default: 8'hff ; */
/*description: This field is used to set a divider value to divide APB clock..*/
#define GPIO_SD0_PRESCALE    0x000000FF
#define GPIO_SD0_PRESCALE_M  ((GPIO_SD0_PRESCALE_V)<<(GPIO_SD0_PRESCALE_S))
#define GPIO_SD0_PRESCALE_V  0xFF
#define GPIO_SD0_PRESCALE_S  8
/* GPIO_SD0_IN : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: This field is used to configure the duty cycle of sigma delta modulation output..*/
#define GPIO_SD0_IN    0x000000FF
#define GPIO_SD0_IN_M  ((GPIO_SD0_IN_V)<<(GPIO_SD0_IN_S))
#define GPIO_SD0_IN_V  0xFF
#define GPIO_SD0_IN_S  0

#define GPIO_SIGMADELTA1_REG          (DR_REG_GPIO_SD_BASE + 0x4)
/* GPIO_SD1_PRESCALE : R/W ;bitpos:[15:8] ;default: 8'hff ; */
/*description: This field is used to set a divider value to divide APB clock..*/
#define GPIO_SD1_PRESCALE    0x000000FF
#define GPIO_SD1_PRESCALE_M  ((GPIO_SD1_PRESCALE_V)<<(GPIO_SD1_PRESCALE_S))
#define GPIO_SD1_PRESCALE_V  0xFF
#define GPIO_SD1_PRESCALE_S  8
/* GPIO_SD1_IN : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: This field is used to configure the duty cycle of sigma delta modulation output..*/
#define GPIO_SD1_IN    0x000000FF
#define GPIO_SD1_IN_M  ((GPIO_SD1_IN_V)<<(GPIO_SD1_IN_S))
#define GPIO_SD1_IN_V  0xFF
#define GPIO_SD1_IN_S  0

#define GPIO_SIGMADELTA2_REG          (DR_REG_GPIO_SD_BASE + 0x8)
/* GPIO_SD2_PRESCALE : R/W ;bitpos:[15:8] ;default: 8'hff ; */
/*description: This field is used to set a divider value to divide APB clock..*/
#define GPIO_SD2_PRESCALE    0x000000FF
#define GPIO_SD2_PRESCALE_M  ((GPIO_SD2_PRESCALE_V)<<(GPIO_SD2_PRESCALE_S))
#define GPIO_SD2_PRESCALE_V  0xFF
#define GPIO_SD2_PRESCALE_S  8
/* GPIO_SD2_IN : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: This field is used to configure the duty cycle of sigma delta modulation output..*/
#define GPIO_SD2_IN    0x000000FF
#define GPIO_SD2_IN_M  ((GPIO_SD2_IN_V)<<(GPIO_SD2_IN_S))
#define GPIO_SD2_IN_V  0xFF
#define GPIO_SD2_IN_S  0

#define GPIO_SIGMADELTA3_REG          (DR_REG_GPIO_SD_BASE + 0xC)
/* GPIO_SD3_PRESCALE : R/W ;bitpos:[15:8] ;default: 8'hff ; */
/*description: This field is used to set a divider value to divide APB clock..*/
#define GPIO_SD3_PRESCALE    0x000000FF
#define GPIO_SD3_PRESCALE_M  ((GPIO_SD3_PRESCALE_V)<<(GPIO_SD3_PRESCALE_S))
#define GPIO_SD3_PRESCALE_V  0xFF
#define GPIO_SD3_PRESCALE_S  8
/* GPIO_SD3_IN : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: This field is used to configure the duty cycle of sigma delta modulation output..*/
#define GPIO_SD3_IN    0x000000FF
#define GPIO_SD3_IN_M  ((GPIO_SD3_IN_V)<<(GPIO_SD3_IN_S))
#define GPIO_SD3_IN_V  0xFF
#define GPIO_SD3_IN_S  0

#define GPIO_SIGMADELTA_CG_REG          (DR_REG_GPIO_SD_BASE + 0x20)
/* GPIO_SD_CLK_EN : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: Clock enable bit of configuration registers for sigma delta modulation..*/
#define GPIO_SD_CLK_EN    (BIT(31))
#define GPIO_SD_CLK_EN_M  (BIT(31))
#define GPIO_SD_CLK_EN_V  0x1
#define GPIO_SD_CLK_EN_S  31

#define GPIO_SIGMADELTA_MISC_REG          (DR_REG_GPIO_SD_BASE + 0x24)
/* GPIO_SPI_SWAP : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: Reserved..*/
#define GPIO_SPI_SWAP    (BIT(31))
#define GPIO_SPI_SWAP_M  (BIT(31))
#define GPIO_SPI_SWAP_V  0x1
#define GPIO_SPI_SWAP_S  31
/* GPIO_FUNCTION_CLK_EN : R/W ;bitpos:[30] ;default: 1'd0 ; */
/*description: Clock enable bit of sigma delta modulation..*/
#define GPIO_FUNCTION_CLK_EN    (BIT(30))
#define GPIO_FUNCTION_CLK_EN_M  (BIT(30))
#define GPIO_FUNCTION_CLK_EN_V  0x1
#define GPIO_FUNCTION_CLK_EN_S  30

#define GPIO_PAD_COMP_CONFIG_REG          (DR_REG_GPIO_SD_BASE + 0x28)
/* GPIO_ZERO_DET_MODE : R/W ;bitpos:[5:4] ;default: 2'h0 ; */
/*description: Zero Detect mode select..*/
#define GPIO_ZERO_DET_MODE    0x00000003
#define GPIO_ZERO_DET_MODE_M  ((GPIO_ZERO_DET_MODE_V)<<(GPIO_ZERO_DET_MODE_S))
#define GPIO_ZERO_DET_MODE_V  0x3
#define GPIO_ZERO_DET_MODE_S  4
/* GPIO_DREF_COMP : R/W ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: internal reference voltage tuning bit. 0V to 0.7*VDDPST step 0.1*VDDPST..*/
#define GPIO_DREF_COMP    0x00000003
#define GPIO_DREF_COMP_M  ((GPIO_DREF_COMP_V)<<(GPIO_DREF_COMP_S))
#define GPIO_DREF_COMP_V  0x3
#define GPIO_DREF_COMP_S  2
/* GPIO_MODE_COMP : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: 1 to enable external reference from PAD[0]. 0 to enable internal reference, mean
while PAD[0] can be used as a regular GPIO..*/
#define GPIO_MODE_COMP    (BIT(1))
#define GPIO_MODE_COMP_M  (BIT(1))
#define GPIO_MODE_COMP_V  0x1
#define GPIO_MODE_COMP_S  1
/* GPIO_XPD_COMP : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: Pad compare enable bit..*/
#define GPIO_XPD_COMP    (BIT(0))
#define GPIO_XPD_COMP_M  (BIT(0))
#define GPIO_XPD_COMP_V  0x1
#define GPIO_XPD_COMP_S  0

#define GPIO_PAD_COMP_FILTER_REG          (DR_REG_GPIO_SD_BASE + 0x2C)
/* GPIO_ZERO_DET_FILTER_CNT : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Zero Detect filter cycle length.*/
#define GPIO_ZERO_DET_FILTER_CNT    0xFFFFFFFF
#define GPIO_ZERO_DET_FILTER_CNT_M  ((GPIO_ZERO_DET_FILTER_CNT_V)<<(GPIO_ZERO_DET_FILTER_CNT_S))
#define GPIO_ZERO_DET_FILTER_CNT_V  0xFFFFFFFF
#define GPIO_ZERO_DET_FILTER_CNT_S  0

#define GPIO_INT_RAW_REG          (DR_REG_GPIO_SD_BASE + 0x80)
/* GPIO_PAD_COMP_INT_RAW : RO/WTC/SS ;bitpos:[0] ;default: 1'b0 ; */
/*description: Pad compare raw interrupt.*/
#define GPIO_PAD_COMP_INT_RAW    (BIT(0))
#define GPIO_PAD_COMP_INT_RAW_M  (BIT(0))
#define GPIO_PAD_COMP_INT_RAW_V  0x1
#define GPIO_PAD_COMP_INT_RAW_S  0

#define GPIO_INT_ST_REG          (DR_REG_GPIO_SD_BASE + 0x84)
/* GPIO_PAD_COMP_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: Pad compare masked interrupt.*/
#define GPIO_PAD_COMP_INT_ST    (BIT(0))
#define GPIO_PAD_COMP_INT_ST_M  (BIT(0))
#define GPIO_PAD_COMP_INT_ST_V  0x1
#define GPIO_PAD_COMP_INT_ST_S  0

#define GPIO_INT_ENA_REG          (DR_REG_GPIO_SD_BASE + 0x88)
/* GPIO_PAD_COMP_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Pad compare interrupt enable.*/
#define GPIO_PAD_COMP_INT_ENA    (BIT(0))
#define GPIO_PAD_COMP_INT_ENA_M  (BIT(0))
#define GPIO_PAD_COMP_INT_ENA_V  0x1
#define GPIO_PAD_COMP_INT_ENA_S  0

#define GPIO_INT_CLR_REG          (DR_REG_GPIO_SD_BASE + 0x8C)
/* GPIO_PAD_COMP_INT_CLR : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: Pad compare interrupt clear.*/
#define GPIO_PAD_COMP_INT_CLR    (BIT(0))
#define GPIO_PAD_COMP_INT_CLR_M  (BIT(0))
#define GPIO_PAD_COMP_INT_CLR_V  0x1
#define GPIO_PAD_COMP_INT_CLR_S  0

#define GPIO_SIGMADELTA_VERSION_REG          (DR_REG_GPIO_SD_BASE + 0xFC)
/* GPIO_SD_DATE : R/W ;bitpos:[27:0] ;default: 28'h2109250 ; */
/*description: Version control register..*/
#define GPIO_SD_DATE    0x0FFFFFFF
#define GPIO_SD_DATE_M  ((GPIO_SD_DATE_V)<<(GPIO_SD_DATE_S))
#define GPIO_SD_DATE_V  0xFFFFFFF
#define GPIO_SD_DATE_S  0


#ifdef __cplusplus
}
#endif



#endif /*_SOC_GPIOSD_REG_H_ */
