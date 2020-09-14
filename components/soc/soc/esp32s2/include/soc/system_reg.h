// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _SOC_SYSTEM_REG_H_
#define _SOC_SYSTEM_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"
#define DPORT_ROM_CTRL_0_REG          (DR_REG_SYSTEM_BASE + 0x000)
/* DPORT_ROM_FO : R/W ;bitpos:[1:0] ;default: ~2'b0 ; */
/*description: */
#define DPORT_ROM_FO  0x00000003
#define DPORT_ROM_FO_M  ((DPORT_ROM_FO_V)<<(DPORT_ROM_FO_S))
#define DPORT_ROM_FO_V  0x3
#define DPORT_ROM_FO_S  0

#define DPORT_ROM_CTRL_1_REG          (DR_REG_SYSTEM_BASE + 0x004)
/* DPORT_ROM_FORCE_PU : R/W ;bitpos:[3:2] ;default: 2'b11 ; */
/*description: */
#define DPORT_ROM_FORCE_PU  0x00000003
#define DPORT_ROM_FORCE_PU_M  ((DPORT_ROM_FORCE_PU_V)<<(DPORT_ROM_FORCE_PU_S))
#define DPORT_ROM_FORCE_PU_V  0x3
#define DPORT_ROM_FORCE_PU_S  2
/* DPORT_ROM_FORCE_PD : R/W ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: */
#define DPORT_ROM_FORCE_PD  0x00000003
#define DPORT_ROM_FORCE_PD_M  ((DPORT_ROM_FORCE_PD_V)<<(DPORT_ROM_FORCE_PD_S))
#define DPORT_ROM_FORCE_PD_V  0x3
#define DPORT_ROM_FORCE_PD_S  0

#define DPORT_SRAM_CTRL_0_REG          (DR_REG_SYSTEM_BASE + 0x008)
/* DPORT_SRAM_FO : R/W ;bitpos:[21:0] ;default: ~22'b0 ; */
/*description: */
#define DPORT_SRAM_FO  0x003FFFFF
#define DPORT_SRAM_FO_M  ((DPORT_SRAM_FO_V)<<(DPORT_SRAM_FO_S))
#define DPORT_SRAM_FO_V  0x3FFFFF
#define DPORT_SRAM_FO_S  0

#define DPORT_SRAM_CTRL_1_REG          (DR_REG_SYSTEM_BASE + 0x00C)
/* DPORT_SRAM_FORCE_PD : R/W ;bitpos:[21:0] ;default: 22'b0 ; */
/*description: */
#define DPORT_SRAM_FORCE_PD  0x003FFFFF
#define DPORT_SRAM_FORCE_PD_M  ((DPORT_SRAM_FORCE_PD_V)<<(DPORT_SRAM_FORCE_PD_S))
#define DPORT_SRAM_FORCE_PD_V  0x3FFFFF
#define DPORT_SRAM_FORCE_PD_S  0

#define DPORT_PERI_CLK_EN_REG DPORT_CPU_PERI_CLK_EN_REG
#define DPORT_CPU_PERI_CLK_EN_REG          (DR_REG_SYSTEM_BASE + 0x010)
/* DPORT_CLK_EN_DEDICATED_GPIO : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define DPORT_CLK_EN_DEDICATED_GPIO  (BIT(7))
#define DPORT_CLK_EN_DEDICATED_GPIO_M  (BIT(7))
#define DPORT_CLK_EN_DEDICATED_GPIO_V  0x1
#define DPORT_CLK_EN_DEDICATED_GPIO_S  7
/* DPORT_CLK_EN_ASSIST_DEBUG : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define DPORT_CLK_EN_ASSIST_DEBUG  (BIT(6))
#define DPORT_CLK_EN_ASSIST_DEBUG_M  (BIT(6))
#define DPORT_CLK_EN_ASSIST_DEBUG_V  0x1
#define DPORT_CLK_EN_ASSIST_DEBUG_S  6

/* NB: Digital signature reset will hold AES & RSA in reset */
#define DPORT_PERI_EN_ASSIST_DEBUG DPORT_CLK_EN_ASSIST_DEBUG

#define DPORT_PERI_RST_EN_REG DPORT_CPU_PERI_RST_EN_REG
#define DPORT_CPU_PERI_RST_EN_REG          (DR_REG_SYSTEM_BASE + 0x014)
/* DPORT_RST_EN_DEDICATED_GPIO : R/W ;bitpos:[7] ;default: 1'b1 ; */
/*description: */
#define DPORT_RST_EN_DEDICATED_GPIO  (BIT(7))
#define DPORT_RST_EN_DEDICATED_GPIO_M  (BIT(7))
#define DPORT_RST_EN_DEDICATED_GPIO_V  0x1
#define DPORT_RST_EN_DEDICATED_GPIO_S  7
/* DPORT_RST_EN_ASSIST_DEBUG : R/W ;bitpos:[6] ;default: 1'b1 ; */
/*description: */
#define DPORT_RST_EN_ASSIST_DEBUG  (BIT(6))
#define DPORT_RST_EN_ASSIST_DEBUG_M  (BIT(6))
#define DPORT_RST_EN_ASSIST_DEBUG_V  0x1
#define DPORT_RST_EN_ASSIST_DEBUG_S  6

#define DPORT_CPU_PER_CONF_REG          (DR_REG_SYSTEM_BASE + 0x018)
/* DPORT_CPU_WAITI_DELAY_NUM : R/W ;bitpos:[7:4] ;default: 4'h0 ; */
/*description: */
#define DPORT_CPU_WAITI_DELAY_NUM  0x0000000F
#define DPORT_CPU_WAITI_DELAY_NUM_M  ((DPORT_CPU_WAITI_DELAY_NUM_V)<<(DPORT_CPU_WAITI_DELAY_NUM_S))
#define DPORT_CPU_WAITI_DELAY_NUM_V  0xF
#define DPORT_CPU_WAITI_DELAY_NUM_S  4
/* DPORT_CPU_WAIT_MODE_FORCE_ON : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: */
#define DPORT_CPU_WAIT_MODE_FORCE_ON  (BIT(3))
#define DPORT_CPU_WAIT_MODE_FORCE_ON_M  (BIT(3))
#define DPORT_CPU_WAIT_MODE_FORCE_ON_V  0x1
#define DPORT_CPU_WAIT_MODE_FORCE_ON_S  3
/* DPORT_PLL_FREQ_SEL : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: */
#define DPORT_PLL_FREQ_SEL  (BIT(2))
#define DPORT_PLL_FREQ_SEL_M  (BIT(2))
#define DPORT_PLL_FREQ_SEL_V  0x1
#define DPORT_PLL_FREQ_SEL_S  2
/* DPORT_CPUPERIOD_SEL : R/W ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: */
#define DPORT_CPUPERIOD_SEL  0x00000003
#define DPORT_CPUPERIOD_SEL_M  ((DPORT_CPUPERIOD_SEL_V)<<(DPORT_CPUPERIOD_SEL_S))
#define DPORT_CPUPERIOD_SEL_V  0x3
#define DPORT_CPUPERIOD_SEL_S  0

#define DPORT_JTAG_CTRL_0_REG          (DR_REG_SYSTEM_BASE + 0x01C)
/* DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_0 : WOR ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_0  0xFFFFFFFF
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_0_M  ((DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_0_V)<<(DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_0_S))
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_0_V  0xFFFFFFFF
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_0_S  0

#define DPORT_JTAG_CTRL_1_REG          (DR_REG_SYSTEM_BASE + 0x020)
/* DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_1 : WOR ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_1  0xFFFFFFFF
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_1_M  ((DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_1_V)<<(DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_1_S))
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_1_V  0xFFFFFFFF
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_1_S  0

#define DPORT_JTAG_CTRL_2_REG          (DR_REG_SYSTEM_BASE + 0x024)
/* DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_2 : WOR ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_2  0xFFFFFFFF
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_2_M  ((DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_2_V)<<(DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_2_S))
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_2_V  0xFFFFFFFF
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_2_S  0

#define DPORT_JTAG_CTRL_3_REG          (DR_REG_SYSTEM_BASE + 0x028)
/* DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_3 : WOR ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_3  0xFFFFFFFF
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_3_M  ((DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_3_V)<<(DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_3_S))
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_3_V  0xFFFFFFFF
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_3_S  0

#define DPORT_JTAG_CTRL_4_REG          (DR_REG_SYSTEM_BASE + 0x02C)
/* DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_4 : WOR ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_4  0xFFFFFFFF
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_4_M  ((DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_4_V)<<(DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_4_S))
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_4_V  0xFFFFFFFF
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_4_S  0

#define DPORT_JTAG_CTRL_5_REG          (DR_REG_SYSTEM_BASE + 0x030)
/* DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_5 : WOR ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_5  0xFFFFFFFF
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_5_M  ((DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_5_V)<<(DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_5_S))
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_5_V  0xFFFFFFFF
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_5_S  0

#define DPORT_JTAG_CTRL_6_REG          (DR_REG_SYSTEM_BASE + 0x034)
/* DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_6 : WOR ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_6  0xFFFFFFFF
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_6_M  ((DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_6_V)<<(DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_6_S))
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_6_V  0xFFFFFFFF
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_6_S  0

#define DPORT_JTAG_CTRL_7_REG          (DR_REG_SYSTEM_BASE + 0x038)
/* DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_7 : WOR ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_7  0xFFFFFFFF
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_7_M  ((DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_7_V)<<(DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_7_S))
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_7_V  0xFFFFFFFF
#define DPORT_CANCEL_EFUSE_DISABLE_JTAG_TEMPORARY_7_S  0

#define DPORT_MEM_PD_MASK_REG          (DR_REG_SYSTEM_BASE + 0x03C)
/* DPORT_LSLP_MEM_PD_MASK : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define DPORT_LSLP_MEM_PD_MASK  (BIT(0))
#define DPORT_LSLP_MEM_PD_MASK_M  (BIT(0))
#define DPORT_LSLP_MEM_PD_MASK_V  0x1
#define DPORT_LSLP_MEM_PD_MASK_S  0

#define DPORT_PERIP_CLK_EN_REG DPORT_PERIP_CLK_EN0_REG
#define DPORT_PERIP_CLK_EN0_REG          (DR_REG_SYSTEM_BASE + 0x040)
/* DPORT_ADC2_ARB_CLK_EN : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: */
#define DPORT_ADC2_ARB_CLK_EN  (BIT(30))
#define DPORT_ADC2_ARB_CLK_EN_M  (BIT(30))
#define DPORT_ADC2_ARB_CLK_EN_V  0x1
#define DPORT_ADC2_ARB_CLK_EN_S  30
/* DPORT_SYSTIMER_CLK_EN : R/W ;bitpos:[29] ;default: 1'b1 ; */
/*description: */
#define DPORT_SYSTIMER_CLK_EN  (BIT(29))
#define DPORT_SYSTIMER_CLK_EN_M  (BIT(29))
#define DPORT_SYSTIMER_CLK_EN_V  0x1
#define DPORT_SYSTIMER_CLK_EN_S  29
/* DPORT_APB_SARADC_CLK_EN : R/W ;bitpos:[28] ;default: 1'b1 ; */
/*description: */
#define DPORT_APB_SARADC_CLK_EN  (BIT(28))
#define DPORT_APB_SARADC_CLK_EN_M  (BIT(28))
#define DPORT_APB_SARADC_CLK_EN_V  0x1
#define DPORT_APB_SARADC_CLK_EN_S  28
/* DPORT_SPI3_DMA_CLK_EN : R/W ;bitpos:[27] ;default: 1'b1 ; */
/*description: */
#define DPORT_SPI3_DMA_CLK_EN  (BIT(27))
#define DPORT_SPI3_DMA_CLK_EN_M  (BIT(27))
#define DPORT_SPI3_DMA_CLK_EN_V  0x1
#define DPORT_SPI3_DMA_CLK_EN_S  27
/* DPORT_PWM3_CLK_EN : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define DPORT_PWM3_CLK_EN  (BIT(26))
#define DPORT_PWM3_CLK_EN_M  (BIT(26))
#define DPORT_PWM3_CLK_EN_V  0x1
#define DPORT_PWM3_CLK_EN_S  26
/* DPORT_PWM2_CLK_EN : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define DPORT_PWM2_CLK_EN  (BIT(25))
#define DPORT_PWM2_CLK_EN_M  (BIT(25))
#define DPORT_PWM2_CLK_EN_V  0x1
#define DPORT_PWM2_CLK_EN_S  25
/* DPORT_UART_MEM_CLK_EN : R/W ;bitpos:[24] ;default: 1'b1 ; */
/*description: */
#define DPORT_UART_MEM_CLK_EN  (BIT(24))
#define DPORT_UART_MEM_CLK_EN_M  (BIT(24))
#define DPORT_UART_MEM_CLK_EN_V  0x1
#define DPORT_UART_MEM_CLK_EN_S  24
/* DPORT_USB_CLK_EN : R/W ;bitpos:[23] ;default: 1'b1 ; */
/*description: */
#define DPORT_USB_CLK_EN  (BIT(23))
#define DPORT_USB_CLK_EN_M  (BIT(23))
#define DPORT_USB_CLK_EN_V  0x1
#define DPORT_USB_CLK_EN_S  23
/* DPORT_SPI2_DMA_CLK_EN : R/W ;bitpos:[22] ;default: 1'b1 ; */
/*description: */
#define DPORT_SPI2_DMA_CLK_EN  (BIT(22))
#define DPORT_SPI2_DMA_CLK_EN_M  (BIT(22))
#define DPORT_SPI2_DMA_CLK_EN_V  0x1
#define DPORT_SPI2_DMA_CLK_EN_S  22
/* DPORT_I2S1_CLK_EN : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define DPORT_I2S1_CLK_EN  (BIT(21))
#define DPORT_I2S1_CLK_EN_M  (BIT(21))
#define DPORT_I2S1_CLK_EN_V  0x1
#define DPORT_I2S1_CLK_EN_S  21
/* DPORT_PWM1_CLK_EN : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define DPORT_PWM1_CLK_EN  (BIT(20))
#define DPORT_PWM1_CLK_EN_M  (BIT(20))
#define DPORT_PWM1_CLK_EN_V  0x1
#define DPORT_PWM1_CLK_EN_S  20
/* DPORT_TWAI_CLK_EN : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define DPORT_TWAI_CLK_EN  (BIT(19))
#define DPORT_TWAI_CLK_EN_M  (BIT(19))
#define DPORT_TWAI_CLK_EN_V  0x1
#define DPORT_TWAI_CLK_EN_S  19
/* DPORT_I2C_EXT1_CLK_EN : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define DPORT_I2C_EXT1_CLK_EN  (BIT(18))
#define DPORT_I2C_EXT1_CLK_EN_M  (BIT(18))
#define DPORT_I2C_EXT1_CLK_EN_V  0x1
#define DPORT_I2C_EXT1_CLK_EN_S  18
/* DPORT_PWM0_CLK_EN : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define DPORT_PWM0_CLK_EN  (BIT(17))
#define DPORT_PWM0_CLK_EN_M  (BIT(17))
#define DPORT_PWM0_CLK_EN_V  0x1
#define DPORT_PWM0_CLK_EN_S  17
/* DPORT_SPI3_CLK_EN : R/W ;bitpos:[16] ;default: 1'b1 ; */
/*description: */
#define DPORT_SPI3_CLK_EN  (BIT(16))
#define DPORT_SPI3_CLK_EN_M  (BIT(16))
#define DPORT_SPI3_CLK_EN_V  0x1
#define DPORT_SPI3_CLK_EN_S  16
/* DPORT_TIMERGROUP1_CLK_EN : R/W ;bitpos:[15] ;default: 1'b1 ; */
/*description: */
#define DPORT_TIMERGROUP1_CLK_EN  (BIT(15))
#define DPORT_TIMERGROUP1_CLK_EN_M  (BIT(15))
#define DPORT_TIMERGROUP1_CLK_EN_V  0x1
#define DPORT_TIMERGROUP1_CLK_EN_S  15
/* DPORT_EFUSE_CLK_EN : R/W ;bitpos:[14] ;default: 1'b1 ; */
/*description: */
#define DPORT_EFUSE_CLK_EN  (BIT(14))
#define DPORT_EFUSE_CLK_EN_M  (BIT(14))
#define DPORT_EFUSE_CLK_EN_V  0x1
#define DPORT_EFUSE_CLK_EN_S  14
/* DPORT_TIMERGROUP_CLK_EN : R/W ;bitpos:[13] ;default: 1'b1 ; */
/*description: */
#define DPORT_TIMERGROUP_CLK_EN  (BIT(13))
#define DPORT_TIMERGROUP_CLK_EN_M  (BIT(13))
#define DPORT_TIMERGROUP_CLK_EN_V  0x1
#define DPORT_TIMERGROUP_CLK_EN_S  13
/* DPORT_UHCI1_CLK_EN : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define DPORT_UHCI1_CLK_EN  (BIT(12))
#define DPORT_UHCI1_CLK_EN_M  (BIT(12))
#define DPORT_UHCI1_CLK_EN_V  0x1
#define DPORT_UHCI1_CLK_EN_S  12
/* DPORT_LEDC_CLK_EN : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define DPORT_LEDC_CLK_EN  (BIT(11))
#define DPORT_LEDC_CLK_EN_M  (BIT(11))
#define DPORT_LEDC_CLK_EN_V  0x1
#define DPORT_LEDC_CLK_EN_S  11
/* DPORT_PCNT_CLK_EN : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define DPORT_PCNT_CLK_EN  (BIT(10))
#define DPORT_PCNT_CLK_EN_M  (BIT(10))
#define DPORT_PCNT_CLK_EN_V  0x1
#define DPORT_PCNT_CLK_EN_S  10
/* DPORT_RMT_CLK_EN : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define DPORT_RMT_CLK_EN  (BIT(9))
#define DPORT_RMT_CLK_EN_M  (BIT(9))
#define DPORT_RMT_CLK_EN_V  0x1
#define DPORT_RMT_CLK_EN_S  9
/* DPORT_UHCI0_CLK_EN : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define DPORT_UHCI0_CLK_EN  (BIT(8))
#define DPORT_UHCI0_CLK_EN_M  (BIT(8))
#define DPORT_UHCI0_CLK_EN_V  0x1
#define DPORT_UHCI0_CLK_EN_S  8
/* DPORT_I2C_EXT0_CLK_EN : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define DPORT_I2C_EXT0_CLK_EN  (BIT(7))
#define DPORT_I2C_EXT0_CLK_EN_M  (BIT(7))
#define DPORT_I2C_EXT0_CLK_EN_V  0x1
#define DPORT_I2C_EXT0_CLK_EN_S  7
/* DPORT_SPI2_CLK_EN : R/W ;bitpos:[6] ;default: 1'b1 ; */
/*description: */
#define DPORT_SPI2_CLK_EN  (BIT(6))
#define DPORT_SPI2_CLK_EN_M  (BIT(6))
#define DPORT_SPI2_CLK_EN_V  0x1
#define DPORT_SPI2_CLK_EN_S  6
/* DPORT_UART1_CLK_EN : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: */
#define DPORT_UART1_CLK_EN  (BIT(5))
#define DPORT_UART1_CLK_EN_M  (BIT(5))
#define DPORT_UART1_CLK_EN_V  0x1
#define DPORT_UART1_CLK_EN_S  5
/* DPORT_I2S0_CLK_EN : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define DPORT_I2S0_CLK_EN  (BIT(4))
#define DPORT_I2S0_CLK_EN_M  (BIT(4))
#define DPORT_I2S0_CLK_EN_V  0x1
#define DPORT_I2S0_CLK_EN_S  4
/* DPORT_WDG_CLK_EN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: */
#define DPORT_WDG_CLK_EN  (BIT(3))
#define DPORT_WDG_CLK_EN_M  (BIT(3))
#define DPORT_WDG_CLK_EN_V  0x1
#define DPORT_WDG_CLK_EN_S  3
/* DPORT_UART_CLK_EN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: */
#define DPORT_UART_CLK_EN  (BIT(2))
#define DPORT_UART_CLK_EN_M  (BIT(2))
#define DPORT_UART_CLK_EN_V  0x1
#define DPORT_UART_CLK_EN_S  2
/* DPORT_SPI01_CLK_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: */
#define DPORT_SPI01_CLK_EN  (BIT(1))
#define DPORT_SPI01_CLK_EN_M  (BIT(1))
#define DPORT_SPI01_CLK_EN_V  0x1
#define DPORT_SPI01_CLK_EN_S  1
/* DPORT_TIMERS_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define DPORT_TIMERS_CLK_EN  (BIT(0))
#define DPORT_TIMERS_CLK_EN_M  (BIT(0))
#define DPORT_TIMERS_CLK_EN_V  0x1
#define DPORT_TIMERS_CLK_EN_S  0

#define DPORT_CPU_PERIP_CLK_EN1_REG	DPORT_PERIP_CLK_EN1_REG
#define DPORT_PERIP_CLK_EN1_REG          (DR_REG_SYSTEM_BASE + 0x044)
/* DPORT_CRYPTO_DMA_CLK_EN : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define DPORT_CRYPTO_DMA_CLK_EN  (BIT(6))
#define DPORT_CRYPTO_DMA_CLK_EN_M  (BIT(6))
#define DPORT_CRYPTO_DMA_CLK_EN_V  0x1
#define DPORT_CRYPTO_DMA_CLK_EN_S  6
/* DPORT_CRYPTO_HMAC_CLK_EN : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define DPORT_CRYPTO_HMAC_CLK_EN  (BIT(5))
#define DPORT_CRYPTO_HMAC_CLK_EN_M  (BIT(5))
#define DPORT_CRYPTO_HMAC_CLK_EN_V  0x1
#define DPORT_CRYPTO_HMAC_CLK_EN_S  5
/* DPORT_CRYPTO_DS_CLK_EN : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define DPORT_CRYPTO_DS_CLK_EN  (BIT(4))
#define DPORT_CRYPTO_DS_CLK_EN_M  (BIT(4))
#define DPORT_CRYPTO_DS_CLK_EN_V  0x1
#define DPORT_CRYPTO_DS_CLK_EN_S  4
/* DPORT_CRYPTO_RSA_CLK_EN : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define DPORT_CRYPTO_RSA_CLK_EN  (BIT(3))
#define DPORT_CRYPTO_RSA_CLK_EN_M  (BIT(3))
#define DPORT_CRYPTO_RSA_CLK_EN_V  0x1
#define DPORT_CRYPTO_RSA_CLK_EN_S  3
/* DPORT_CRYPTO_SHA_CLK_EN : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define DPORT_CRYPTO_SHA_CLK_EN  (BIT(2))
#define DPORT_CRYPTO_SHA_CLK_EN_M  (BIT(2))
#define DPORT_CRYPTO_SHA_CLK_EN_V  0x1
#define DPORT_CRYPTO_SHA_CLK_EN_S  2
/* DPORT_CRYPTO_AES_CLK_EN : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define DPORT_CRYPTO_AES_CLK_EN  (BIT(1))
#define DPORT_CRYPTO_AES_CLK_EN_M  (BIT(1))
#define DPORT_CRYPTO_AES_CLK_EN_V  0x1
#define DPORT_CRYPTO_AES_CLK_EN_S  1

#define DPORT_PERIP_RST_EN_REG DPORT_PERIP_RST_EN0_REG
#define DPORT_PERIP_RST_EN0_REG          (DR_REG_SYSTEM_BASE + 0x048)
/* DPORT_ADC2_ARB_RST : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define DPORT_ADC2_ARB_RST  (BIT(30))
#define DPORT_ADC2_ARB_RST_M  (BIT(30))
#define DPORT_ADC2_ARB_RST_V  0x1
#define DPORT_ADC2_ARB_RST_S  30
/* DPORT_SYSTIMER_RST : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define DPORT_SYSTIMER_RST  (BIT(29))
#define DPORT_SYSTIMER_RST_M  (BIT(29))
#define DPORT_SYSTIMER_RST_V  0x1
#define DPORT_SYSTIMER_RST_S  29
/* DPORT_APB_SARADC_RST : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define DPORT_APB_SARADC_RST  (BIT(28))
#define DPORT_APB_SARADC_RST_M  (BIT(28))
#define DPORT_APB_SARADC_RST_V  0x1
#define DPORT_APB_SARADC_RST_S  28
/* DPORT_SPI3_DMA_RST : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: */
#define DPORT_SPI3_DMA_RST  (BIT(27))
#define DPORT_SPI3_DMA_RST_M  (BIT(27))
#define DPORT_SPI3_DMA_RST_V  0x1
#define DPORT_SPI3_DMA_RST_S  27
/* DPORT_PWM3_RST : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define DPORT_PWM3_RST  (BIT(26))
#define DPORT_PWM3_RST_M  (BIT(26))
#define DPORT_PWM3_RST_V  0x1
#define DPORT_PWM3_RST_S  26
/* DPORT_PWM2_RST : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define DPORT_PWM2_RST  (BIT(25))
#define DPORT_PWM2_RST_M  (BIT(25))
#define DPORT_PWM2_RST_V  0x1
#define DPORT_PWM2_RST_S  25
/* DPORT_UART_MEM_RST : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define DPORT_UART_MEM_RST  (BIT(24))
#define DPORT_UART_MEM_RST_M  (BIT(24))
#define DPORT_UART_MEM_RST_V  0x1
#define DPORT_UART_MEM_RST_S  24
/* DPORT_USB_RST : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define DPORT_USB_RST  (BIT(23))
#define DPORT_USB_RST_M  (BIT(23))
#define DPORT_USB_RST_V  0x1
#define DPORT_USB_RST_S  23
/* DPORT_SPI2_DMA_RST : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define DPORT_SPI2_DMA_RST  (BIT(22))
#define DPORT_SPI2_DMA_RST_M  (BIT(22))
#define DPORT_SPI2_DMA_RST_V  0x1
#define DPORT_SPI2_DMA_RST_S  22
/* DPORT_I2S1_RST : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define DPORT_I2S1_RST  (BIT(21))
#define DPORT_I2S1_RST_M  (BIT(21))
#define DPORT_I2S1_RST_V  0x1
#define DPORT_I2S1_RST_S  21
/* DPORT_PWM1_RST : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define DPORT_PWM1_RST  (BIT(20))
#define DPORT_PWM1_RST_M  (BIT(20))
#define DPORT_PWM1_RST_V  0x1
#define DPORT_PWM1_RST_S  20
/* DPORT_TWAI_RST : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define DPORT_TWAI_RST  (BIT(19))
#define DPORT_TWAI_RST_M  (BIT(19))
#define DPORT_TWAI_RST_V  0x1
#define DPORT_TWAI_RST_S  19
/* DPORT_I2C_EXT1_RST : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define DPORT_I2C_EXT1_RST  (BIT(18))
#define DPORT_I2C_EXT1_RST_M  (BIT(18))
#define DPORT_I2C_EXT1_RST_V  0x1
#define DPORT_I2C_EXT1_RST_S  18
/* DPORT_PWM0_RST : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define DPORT_PWM0_RST  (BIT(17))
#define DPORT_PWM0_RST_M  (BIT(17))
#define DPORT_PWM0_RST_V  0x1
#define DPORT_PWM0_RST_S  17
/* DPORT_SPI3_RST : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define DPORT_SPI3_RST  (BIT(16))
#define DPORT_SPI3_RST_M  (BIT(16))
#define DPORT_SPI3_RST_V  0x1
#define DPORT_SPI3_RST_S  16
/* DPORT_TIMERGROUP1_RST : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define DPORT_TIMERGROUP1_RST  (BIT(15))
#define DPORT_TIMERGROUP1_RST_M  (BIT(15))
#define DPORT_TIMERGROUP1_RST_V  0x1
#define DPORT_TIMERGROUP1_RST_S  15
/* DPORT_EFUSE_RST : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define DPORT_EFUSE_RST  (BIT(14))
#define DPORT_EFUSE_RST_M  (BIT(14))
#define DPORT_EFUSE_RST_V  0x1
#define DPORT_EFUSE_RST_S  14
/* DPORT_TIMERGROUP_RST : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define DPORT_TIMERGROUP_RST  (BIT(13))
#define DPORT_TIMERGROUP_RST_M  (BIT(13))
#define DPORT_TIMERGROUP_RST_V  0x1
#define DPORT_TIMERGROUP_RST_S  13
/* DPORT_UHCI1_RST : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define DPORT_UHCI1_RST  (BIT(12))
#define DPORT_UHCI1_RST_M  (BIT(12))
#define DPORT_UHCI1_RST_V  0x1
#define DPORT_UHCI1_RST_S  12
/* DPORT_LEDC_RST : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define DPORT_LEDC_RST  (BIT(11))
#define DPORT_LEDC_RST_M  (BIT(11))
#define DPORT_LEDC_RST_V  0x1
#define DPORT_LEDC_RST_S  11
/* DPORT_PCNT_RST : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define DPORT_PCNT_RST  (BIT(10))
#define DPORT_PCNT_RST_M  (BIT(10))
#define DPORT_PCNT_RST_V  0x1
#define DPORT_PCNT_RST_S  10
/* DPORT_RMT_RST : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define DPORT_RMT_RST  (BIT(9))
#define DPORT_RMT_RST_M  (BIT(9))
#define DPORT_RMT_RST_V  0x1
#define DPORT_RMT_RST_S  9
/* DPORT_UHCI0_RST : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define DPORT_UHCI0_RST  (BIT(8))
#define DPORT_UHCI0_RST_M  (BIT(8))
#define DPORT_UHCI0_RST_V  0x1
#define DPORT_UHCI0_RST_S  8
/* DPORT_I2C_EXT0_RST : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define DPORT_I2C_EXT0_RST  (BIT(7))
#define DPORT_I2C_EXT0_RST_M  (BIT(7))
#define DPORT_I2C_EXT0_RST_V  0x1
#define DPORT_I2C_EXT0_RST_S  7
/* DPORT_SPI2_RST : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define DPORT_SPI2_RST  (BIT(6))
#define DPORT_SPI2_RST_M  (BIT(6))
#define DPORT_SPI2_RST_V  0x1
#define DPORT_SPI2_RST_S  6
/* DPORT_UART1_RST : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define DPORT_UART1_RST  (BIT(5))
#define DPORT_UART1_RST_M  (BIT(5))
#define DPORT_UART1_RST_V  0x1
#define DPORT_UART1_RST_S  5
/* DPORT_I2S0_RST : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define DPORT_I2S0_RST  (BIT(4))
#define DPORT_I2S0_RST_M  (BIT(4))
#define DPORT_I2S0_RST_V  0x1
#define DPORT_I2S0_RST_S  4
/* DPORT_WDG_RST : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define DPORT_WDG_RST  (BIT(3))
#define DPORT_WDG_RST_M  (BIT(3))
#define DPORT_WDG_RST_V  0x1
#define DPORT_WDG_RST_S  3
/* DPORT_UART_RST : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define DPORT_UART_RST  (BIT(2))
#define DPORT_UART_RST_M  (BIT(2))
#define DPORT_UART_RST_V  0x1
#define DPORT_UART_RST_S  2
/* DPORT_SPI01_RST : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define DPORT_SPI01_RST  (BIT(1))
#define DPORT_SPI01_RST_M  (BIT(1))
#define DPORT_SPI01_RST_V  0x1
#define DPORT_SPI01_RST_S  1
/* DPORT_TIMERS_RST : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_TIMERS_RST  (BIT(0))
#define DPORT_TIMERS_RST_M  (BIT(0))
#define DPORT_TIMERS_RST_V  0x1
#define DPORT_TIMERS_RST_S  0

#define DPORT_CPU_PERIP_RST_EN1_REG      DPORT_PERIP_RST_EN1_REG
#define DPORT_PERIP_RST_EN1_REG          (DR_REG_SYSTEM_BASE + 0x04C)
/* DPORT_CRYPTO_DMA_RST : R/W ;bitpos:[6] ;default: 1'b1 ; */
/*description: */
#define DPORT_CRYPTO_DMA_RST  (BIT(6))
#define DPORT_CRYPTO_DMA_RST_M  (BIT(6))
#define DPORT_CRYPTO_DMA_RST_V  0x1
#define DPORT_CRYPTO_DMA_RST_S  6
/* DPORT_CRYPTO_HMAC_RST : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: */
#define DPORT_CRYPTO_HMAC_RST  (BIT(5))
#define DPORT_CRYPTO_HMAC_RST_M  (BIT(5))
#define DPORT_CRYPTO_HMAC_RST_V  0x1
#define DPORT_CRYPTO_HMAC_RST_S  5
/* DPORT_CRYPTO_DS_RST : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: */
#define DPORT_CRYPTO_DS_RST  (BIT(4))
#define DPORT_CRYPTO_DS_RST_M  (BIT(4))
#define DPORT_CRYPTO_DS_RST_V  0x1
#define DPORT_CRYPTO_DS_RST_S  4
/* DPORT_CRYPTO_RSA_RST : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: */
#define DPORT_CRYPTO_RSA_RST  (BIT(3))
#define DPORT_CRYPTO_RSA_RST_M  (BIT(3))
#define DPORT_CRYPTO_RSA_RST_V  0x1
#define DPORT_CRYPTO_RSA_RST_S  3
/* DPORT_CRYPTO_SHA_RST : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: */
#define DPORT_CRYPTO_SHA_RST  (BIT(2))
#define DPORT_CRYPTO_SHA_RST_M  (BIT(2))
#define DPORT_CRYPTO_SHA_RST_V  0x1
#define DPORT_CRYPTO_SHA_RST_S  2
/* DPORT_CRYPTO_AES_RST : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: */
#define DPORT_CRYPTO_AES_RST  (BIT(1))
#define DPORT_CRYPTO_AES_RST_M  (BIT(1))
#define DPORT_CRYPTO_AES_RST_V  0x1
#define DPORT_CRYPTO_AES_RST_S  1

#define DPORT_BT_LPCK_DIV_INT_REG          (DR_REG_SYSTEM_BASE + 0x050)
/* DPORT_BT_LPCK_DIV_NUM : R/W ;bitpos:[11:0] ;default: 12'd255 ; */
/*description: */
#define DPORT_BT_LPCK_DIV_NUM  0x00000FFF
#define DPORT_BT_LPCK_DIV_NUM_M  ((DPORT_BT_LPCK_DIV_NUM_V)<<(DPORT_BT_LPCK_DIV_NUM_S))
#define DPORT_BT_LPCK_DIV_NUM_V  0xFFF
#define DPORT_BT_LPCK_DIV_NUM_S  0

#define DPORT_BT_LPCK_DIV_FRAC_REG          (DR_REG_SYSTEM_BASE + 0x054)
/* DPORT_LPCLK_RTC_EN : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define DPORT_LPCLK_RTC_EN  (BIT(28))
#define DPORT_LPCLK_RTC_EN_M  (BIT(28))
#define DPORT_LPCLK_RTC_EN_V  0x1
#define DPORT_LPCLK_RTC_EN_S  28
/* DPORT_LPCLK_SEL_XTAL32K : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: */
#define DPORT_LPCLK_SEL_XTAL32K  (BIT(27))
#define DPORT_LPCLK_SEL_XTAL32K_M  (BIT(27))
#define DPORT_LPCLK_SEL_XTAL32K_V  0x1
#define DPORT_LPCLK_SEL_XTAL32K_S  27
/* DPORT_LPCLK_SEL_XTAL : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define DPORT_LPCLK_SEL_XTAL  (BIT(26))
#define DPORT_LPCLK_SEL_XTAL_M  (BIT(26))
#define DPORT_LPCLK_SEL_XTAL_V  0x1
#define DPORT_LPCLK_SEL_XTAL_S  26
/* DPORT_LPCLK_SEL_8M : R/W ;bitpos:[25] ;default: 1'b1 ; */
/*description: */
#define DPORT_LPCLK_SEL_8M  (BIT(25))
#define DPORT_LPCLK_SEL_8M_M  (BIT(25))
#define DPORT_LPCLK_SEL_8M_V  0x1
#define DPORT_LPCLK_SEL_8M_S  25
/* DPORT_LPCLK_SEL_RTC_SLOW : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define DPORT_LPCLK_SEL_RTC_SLOW  (BIT(24))
#define DPORT_LPCLK_SEL_RTC_SLOW_M  (BIT(24))
#define DPORT_LPCLK_SEL_RTC_SLOW_V  0x1
#define DPORT_LPCLK_SEL_RTC_SLOW_S  24
/* DPORT_BT_LPCK_DIV_A : R/W ;bitpos:[23:12] ;default: 12'd1 ; */
/*description: */
#define DPORT_BT_LPCK_DIV_A  0x00000FFF
#define DPORT_BT_LPCK_DIV_A_M  ((DPORT_BT_LPCK_DIV_A_V)<<(DPORT_BT_LPCK_DIV_A_S))
#define DPORT_BT_LPCK_DIV_A_V  0xFFF
#define DPORT_BT_LPCK_DIV_A_S  12
/* DPORT_BT_LPCK_DIV_B : R/W ;bitpos:[11:0] ;default: 12'd1 ; */
/*description: */
#define DPORT_BT_LPCK_DIV_B  0x00000FFF
#define DPORT_BT_LPCK_DIV_B_M  ((DPORT_BT_LPCK_DIV_B_V)<<(DPORT_BT_LPCK_DIV_B_S))
#define DPORT_BT_LPCK_DIV_B_V  0xFFF
#define DPORT_BT_LPCK_DIV_B_S  0

#define DPORT_CPU_INTR_FROM_CPU_0_REG          (DR_REG_SYSTEM_BASE + 0x058)
/* DPORT_CPU_INTR_FROM_CPU_0 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_CPU_INTR_FROM_CPU_0  (BIT(0))
#define DPORT_CPU_INTR_FROM_CPU_0_M  (BIT(0))
#define DPORT_CPU_INTR_FROM_CPU_0_V  0x1
#define DPORT_CPU_INTR_FROM_CPU_0_S  0

#define DPORT_CPU_INTR_FROM_CPU_1_REG          (DR_REG_SYSTEM_BASE + 0x05C)
/* DPORT_CPU_INTR_FROM_CPU_1 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_CPU_INTR_FROM_CPU_1  (BIT(0))
#define DPORT_CPU_INTR_FROM_CPU_1_M  (BIT(0))
#define DPORT_CPU_INTR_FROM_CPU_1_V  0x1
#define DPORT_CPU_INTR_FROM_CPU_1_S  0

#define DPORT_CPU_INTR_FROM_CPU_2_REG          (DR_REG_SYSTEM_BASE + 0x060)
/* DPORT_CPU_INTR_FROM_CPU_2 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_CPU_INTR_FROM_CPU_2  (BIT(0))
#define DPORT_CPU_INTR_FROM_CPU_2_M  (BIT(0))
#define DPORT_CPU_INTR_FROM_CPU_2_V  0x1
#define DPORT_CPU_INTR_FROM_CPU_2_S  0

#define DPORT_CPU_INTR_FROM_CPU_3_REG          (DR_REG_SYSTEM_BASE + 0x064)
/* DPORT_CPU_INTR_FROM_CPU_3 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_CPU_INTR_FROM_CPU_3  (BIT(0))
#define DPORT_CPU_INTR_FROM_CPU_3_M  (BIT(0))
#define DPORT_CPU_INTR_FROM_CPU_3_V  0x1
#define DPORT_CPU_INTR_FROM_CPU_3_S  0

#define DPORT_RSA_PD_CTRL_REG          (DR_REG_SYSTEM_BASE + 0x068)
/* DPORT_RSA_MEM_FORCE_PD : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define DPORT_RSA_MEM_FORCE_PD  (BIT(2))
#define DPORT_RSA_MEM_FORCE_PD_M  (BIT(2))
#define DPORT_RSA_MEM_FORCE_PD_V  0x1
#define DPORT_RSA_MEM_FORCE_PD_S  2
/* DPORT_RSA_MEM_FORCE_PU : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define DPORT_RSA_MEM_FORCE_PU  (BIT(1))
#define DPORT_RSA_MEM_FORCE_PU_M  (BIT(1))
#define DPORT_RSA_MEM_FORCE_PU_V  0x1
#define DPORT_RSA_MEM_FORCE_PU_S  1
/* DPORT_RSA_MEM_PD : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define DPORT_RSA_MEM_PD  (BIT(0))
#define DPORT_RSA_MEM_PD_M  (BIT(0))
#define DPORT_RSA_MEM_PD_V  0x1
#define DPORT_RSA_MEM_PD_S  0
#define DPORT_RSA_PD DPORT_RSA_MEM_PD

#define DPORT_BUSTOEXTMEM_ENA_REG          (DR_REG_SYSTEM_BASE + 0x06C)
/* DPORT_BUSTOEXTMEM_ENA : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define DPORT_BUSTOEXTMEM_ENA  (BIT(0))
#define DPORT_BUSTOEXTMEM_ENA_M  (BIT(0))
#define DPORT_BUSTOEXTMEM_ENA_V  0x1
#define DPORT_BUSTOEXTMEM_ENA_S  0

#define DPORT_CACHE_CONTROL_REG          (DR_REG_SYSTEM_BASE + 0x070)
/* DPORT_PRO_CACHE_RESET : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_CACHE_RESET  (BIT(2))
#define DPORT_PRO_CACHE_RESET_M  (BIT(2))
#define DPORT_PRO_CACHE_RESET_V  0x1
#define DPORT_PRO_CACHE_RESET_S  2
/* DPORT_PRO_DCACHE_CLK_ON : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: */
#define DPORT_PRO_DCACHE_CLK_ON  (BIT(1))
#define DPORT_PRO_DCACHE_CLK_ON_M  (BIT(1))
#define DPORT_PRO_DCACHE_CLK_ON_V  0x1
#define DPORT_PRO_DCACHE_CLK_ON_S  1
/* DPORT_PRO_ICACHE_CLK_ON : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define DPORT_PRO_ICACHE_CLK_ON  (BIT(0))
#define DPORT_PRO_ICACHE_CLK_ON_M  (BIT(0))
#define DPORT_PRO_ICACHE_CLK_ON_V  0x1
#define DPORT_PRO_ICACHE_CLK_ON_S  0

#define DPORT_EXTERNAL_DEVICE_ENCRYPT_DECRYPT_CONTROL_REG          (DR_REG_SYSTEM_BASE + 0x074)
/* DPORT_ENABLE_DOWNLOAD_MANUAL_ENCRYPT : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define DPORT_ENABLE_DOWNLOAD_MANUAL_ENCRYPT  (BIT(3))
#define DPORT_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_M  (BIT(3))
#define DPORT_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_V  0x1
#define DPORT_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_S  3
/* DPORT_ENABLE_DOWNLOAD_G0CB_DECRYPT : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define DPORT_ENABLE_DOWNLOAD_G0CB_DECRYPT  (BIT(2))
#define DPORT_ENABLE_DOWNLOAD_G0CB_DECRYPT_M  (BIT(2))
#define DPORT_ENABLE_DOWNLOAD_G0CB_DECRYPT_V  0x1
#define DPORT_ENABLE_DOWNLOAD_G0CB_DECRYPT_S  2
/* DPORT_ENABLE_DOWNLOAD_DB_ENCRYPT : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define DPORT_ENABLE_DOWNLOAD_DB_ENCRYPT  (BIT(1))
#define DPORT_ENABLE_DOWNLOAD_DB_ENCRYPT_M  (BIT(1))
#define DPORT_ENABLE_DOWNLOAD_DB_ENCRYPT_V  0x1
#define DPORT_ENABLE_DOWNLOAD_DB_ENCRYPT_S  1
/* DPORT_ENABLE_SPI_MANUAL_ENCRYPT : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_ENABLE_SPI_MANUAL_ENCRYPT  (BIT(0))
#define DPORT_ENABLE_SPI_MANUAL_ENCRYPT_M  (BIT(0))
#define DPORT_ENABLE_SPI_MANUAL_ENCRYPT_V  0x1
#define DPORT_ENABLE_SPI_MANUAL_ENCRYPT_S  0

#define DPORT_RTC_FASTMEM_CONFIG_REG          (DR_REG_SYSTEM_BASE + 0x078)
/* DPORT_RTC_MEM_CRC_FINISH : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define DPORT_RTC_MEM_CRC_FINISH  (BIT(31))
#define DPORT_RTC_MEM_CRC_FINISH_M  (BIT(31))
#define DPORT_RTC_MEM_CRC_FINISH_V  0x1
#define DPORT_RTC_MEM_CRC_FINISH_S  31
/* DPORT_RTC_MEM_CRC_LEN : R/W ;bitpos:[30:20] ;default: 11'h7ff ; */
/*description: */
#define DPORT_RTC_MEM_CRC_LEN  0x000007FF
#define DPORT_RTC_MEM_CRC_LEN_M  ((DPORT_RTC_MEM_CRC_LEN_V)<<(DPORT_RTC_MEM_CRC_LEN_S))
#define DPORT_RTC_MEM_CRC_LEN_V  0x7FF
#define DPORT_RTC_MEM_CRC_LEN_S  20
/* DPORT_RTC_MEM_CRC_ADDR : R/W ;bitpos:[19:9] ;default: 11'h0 ; */
/*description: */
#define DPORT_RTC_MEM_CRC_ADDR  0x000007FF
#define DPORT_RTC_MEM_CRC_ADDR_M  ((DPORT_RTC_MEM_CRC_ADDR_V)<<(DPORT_RTC_MEM_CRC_ADDR_S))
#define DPORT_RTC_MEM_CRC_ADDR_V  0x7FF
#define DPORT_RTC_MEM_CRC_ADDR_S  9
/* DPORT_RTC_MEM_CRC_START : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define DPORT_RTC_MEM_CRC_START  (BIT(8))
#define DPORT_RTC_MEM_CRC_START_M  (BIT(8))
#define DPORT_RTC_MEM_CRC_START_V  0x1
#define DPORT_RTC_MEM_CRC_START_S  8

#define DPORT_RTC_FASTMEM_CRC_REG          (DR_REG_SYSTEM_BASE + 0x07C)
/* DPORT_RTC_MEM_CRC_RES : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_RTC_MEM_CRC_RES  0xFFFFFFFF
#define DPORT_RTC_MEM_CRC_RES_M  ((DPORT_RTC_MEM_CRC_RES_V)<<(DPORT_RTC_MEM_CRC_RES_S))
#define DPORT_RTC_MEM_CRC_RES_V  0xFFFFFFFF
#define DPORT_RTC_MEM_CRC_RES_S  0

#define DPORT_REDUNDANT_ECO_CTRL_REG          (DR_REG_SYSTEM_BASE + 0x080)
/* DPORT_REDUNDANT_ECO_RESULT : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define DPORT_REDUNDANT_ECO_RESULT  (BIT(1))
#define DPORT_REDUNDANT_ECO_RESULT_M  (BIT(1))
#define DPORT_REDUNDANT_ECO_RESULT_V  0x1
#define DPORT_REDUNDANT_ECO_RESULT_S  1
/* DPORT_REDUNDANT_ECO_DRIVE : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_REDUNDANT_ECO_DRIVE  (BIT(0))
#define DPORT_REDUNDANT_ECO_DRIVE_M  (BIT(0))
#define DPORT_REDUNDANT_ECO_DRIVE_V  0x1
#define DPORT_REDUNDANT_ECO_DRIVE_S  0

#define SYSTEM_CLOCK_GATE_REG          (DR_REG_SYSTEM_BASE + 0x084)
/* SYSTEM_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define SYSTEM_CLK_EN  (BIT(0))
#define SYSTEM_CLK_EN_M  (BIT(0))
#define SYSTEM_CLK_EN_V  0x1
#define SYSTEM_CLK_EN_S  0

#define DPORT_SRAM_CTRL_2_REG          (DR_REG_SYSTEM_BASE + 0x088)
/* DPORT_SRAM_FORCE_PU : R/W ;bitpos:[21:0] ;default: 22'h3fffff ; */
/*description: */
#define DPORT_SRAM_FORCE_PU  0x003FFFFF
#define DPORT_SRAM_FORCE_PU_M  ((DPORT_SRAM_FORCE_PU_V)<<(DPORT_SRAM_FORCE_PU_S))
#define DPORT_SRAM_FORCE_PU_V  0x3FFFFF
#define DPORT_SRAM_FORCE_PU_S  0

#define DPORT_SYSCLK_CONF_REG          (DR_REG_SYSTEM_BASE + 0x08C)
/* DPORT_CLK_DIV_EN : RO ;bitpos:[19] ;default: 1'd0 ; */
/*description: */
#define DPORT_CLK_DIV_EN  (BIT(19))
#define DPORT_CLK_DIV_EN_M  (BIT(19))
#define DPORT_CLK_DIV_EN_V  0x1
#define DPORT_CLK_DIV_EN_S  19
/* DPORT_CLK_XTAL_FREQ : RO ;bitpos:[18:12] ;default: 7'd0 ; */
/*description: */
#define DPORT_CLK_XTAL_FREQ  0x0000007F
#define DPORT_CLK_XTAL_FREQ_M  ((DPORT_CLK_XTAL_FREQ_V)<<(DPORT_CLK_XTAL_FREQ_S))
#define DPORT_CLK_XTAL_FREQ_V  0x7F
#define DPORT_CLK_XTAL_FREQ_S  12
/* DPORT_SOC_CLK_SEL : R/W ;bitpos:[11:10] ;default: 2'd0 ; */
/*description: */
#define DPORT_SOC_CLK_SEL  0x00000003
#define DPORT_SOC_CLK_SEL_M  ((DPORT_SOC_CLK_SEL_V)<<(DPORT_SOC_CLK_SEL_S))
#define DPORT_SOC_CLK_SEL_V  0x3
#define DPORT_SOC_CLK_SEL_S  10
#define DPORT_SOC_CLK_SEL_XTL    0
#define DPORT_SOC_CLK_SEL_PLL    1
#define DPORT_SOC_CLK_SEL_8M     2
#define DPORT_SOC_CLK_SEL_APLL   3
/* DPORT_PRE_DIV_CNT : R/W ;bitpos:[9:0] ;default: 10'h1 ; */
/*description: */
#define DPORT_PRE_DIV_CNT  0x000003FF
#define DPORT_PRE_DIV_CNT_M  ((DPORT_PRE_DIV_CNT_V)<<(DPORT_PRE_DIV_CNT_S))
#define DPORT_PRE_DIV_CNT_V  0x3FF
#define DPORT_PRE_DIV_CNT_S  0

#define SYSTEM_DATE_REG          (DR_REG_SYSTEM_BASE + 0xFFC)
/* SYSTEM_DATE : R/W ;bitpos:[27:0] ;default: 28'h1908020 ; */
/*description: */
#define SYSTEM_DATE  0x0FFFFFFF
#define SYSTEM_DATE_M  ((SYSTEM_DATE_V)<<(SYSTEM_DATE_S))
#define SYSTEM_DATE_V  0xFFFFFFF
#define SYSTEM_DATE_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_SYSTEM_REG_H_ */


