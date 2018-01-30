// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _SOC_IO_MUX_REG_H_
#define _SOC_IO_MUX_REG_H_

#include "soc.h"

/* The following are the bit fields for PERIPHS_IO_MUX_x_U registers */
/* Output enable in sleep mode */
#define SLP_OE (BIT(0))
#define SLP_OE_M (BIT(0))
#define SLP_OE_V 1
#define SLP_OE_S 0
/* Pin used for wakeup from sleep */
#define SLP_SEL (BIT(1))
#define SLP_SEL_M (BIT(1))
#define SLP_SEL_V 1
#define SLP_SEL_S 1
/* Pulldown enable in sleep mode */
#define SLP_PD (BIT(2))
#define SLP_PD_M (BIT(2))
#define SLP_PD_V 1
#define SLP_PD_S 2
/* Pullup enable in sleep mode */
#define SLP_PU (BIT(3))
#define SLP_PU_M (BIT(3))
#define SLP_PU_V 1
#define SLP_PU_S 3
/* Input enable in sleep mode */
#define SLP_IE (BIT(4))
#define SLP_IE_M (BIT(4))
#define SLP_IE_V 1
#define SLP_IE_S 4
/* Drive strength in sleep mode */
#define SLP_DRV 0x3
#define SLP_DRV_M (SLP_DRV_V << SLP_DRV_S)
#define SLP_DRV_V 0x3
#define SLP_DRV_S 5
/* Pulldown enable */
#define FUN_PD (BIT(7))
#define FUN_PD_M (BIT(7))
#define FUN_PD_V 1
#define FUN_PD_S 7
/* Pullup enable */
#define FUN_PU (BIT(8))
#define FUN_PU_M (BIT(8))
#define FUN_PU_V 1
#define FUN_PU_S 8
/* Input enable */
#define FUN_IE (BIT(9))
#define FUN_IE_M (FUN_IE_V << FUN_IE_S)
#define FUN_IE_V 1
#define FUN_IE_S 9
/* Drive strength */
#define FUN_DRV 0x3
#define FUN_DRV_M (FUN_DRV_V << FUN_DRV_S)
#define FUN_DRV_V 0x3
#define FUN_DRV_S 10
/* Function select (possible values are defined for each pin as FUNC_pinname_function below) */
#define MCU_SEL 0x7
#define MCU_SEL_M (MCU_SEL_V << MCU_SEL_S)
#define MCU_SEL_V 0x7
#define MCU_SEL_S 12

#define PIN_INPUT_ENABLE(PIN_NAME)               SET_PERI_REG_MASK(PIN_NAME,FUN_IE)
#define PIN_INPUT_DISABLE(PIN_NAME)              CLEAR_PERI_REG_MASK(PIN_NAME,FUN_IE)
#define PIN_SET_DRV(PIN_NAME, drv)            REG_SET_FIELD(PIN_NAME, FUN_DRV, (drv));

/*
 * @attention
 *     The PIN_PULL[UP|DWN]_[EN|DIS]() functions used to exist as macros in previous SDK versions. 
 *     Unfortunately, however, they do not work for some GPIOs on the ESP32 chip, which needs pullups 
 *     and -downs turned on and off through RTC registers. The functions still exist for compatibility 
 *     with older code, but are marked as deprecated in order to generate a warning.
 *     Please replace them in this fashion: (make sure to include driver/gpio.h as well)
 *     PIN_PULLUP_EN(GPIO_PIN_MUX_REG[x])   ->  gpio_pullup_en(x)
 *     PIN_PULLUP_DIS(GPIO_PIN_MUX_REG[x])  ->  gpio_pullup_dis(x)
 *     PIN_PULLDWN_EN(GPIO_PIN_MUX_REG[x])  ->  gpio_pulldown_en(x)
 *     PIN_PULLDWN_DIS(GPIO_PIN_MUX_REG[x]) ->  gpio_pulldown_dis(x)
 *
*/
static inline void __attribute__ ((deprecated)) PIN_PULLUP_DIS(uint32_t PIN_NAME) 
{
    REG_CLR_BIT(PIN_NAME, FUN_PU);
}

static inline void __attribute__ ((deprecated)) PIN_PULLUP_EN(uint32_t PIN_NAME)
{
    REG_SET_BIT(PIN_NAME, FUN_PU);
}

static inline void __attribute__ ((deprecated)) PIN_PULLDWN_DIS(uint32_t PIN_NAME)
{
    REG_CLR_BIT(PIN_NAME, FUN_PD);
}

static inline void __attribute__ ((deprecated)) PIN_PULLDWN_EN(uint32_t PIN_NAME)
{
    REG_SET_BIT(PIN_NAME, FUN_PD);
}


#define PIN_FUNC_SELECT(PIN_NAME, FUNC)      REG_SET_FIELD(PIN_NAME, MCU_SEL, FUNC)

#define PIN_FUNC_GPIO                               2

#define PIN_CTRL                          (DR_REG_IO_MUX_BASE +0x00)
#define CLK_OUT3                                    0xf
#define CLK_OUT3_S                                  8
#define CLK_OUT2                                    0xf
#define CLK_OUT2_S                                  4
#define CLK_OUT1                                    0xf
#define CLK_OUT1_S                                  0

#define PERIPHS_IO_MUX_GPIO0_U            (DR_REG_IO_MUX_BASE +0x44)
#define IO_MUX_GPIO0_REG                  PERIPHS_IO_MUX_GPIO0_U
#define FUNC_GPIO0_EMAC_TX_CLK                      5
#define FUNC_GPIO0_GPIO0                            2
#define FUNC_GPIO0_CLK_OUT1                         1
#define FUNC_GPIO0_GPIO0_0                          0

#define PERIPHS_IO_MUX_U0TXD_U            (DR_REG_IO_MUX_BASE +0x88)
#define IO_MUX_GPIO1_REG                  PERIPHS_IO_MUX_U0TXD_U
#define FUNC_U0TXD_EMAC_RXD2                        5
#define FUNC_U0TXD_GPIO1                            2
#define FUNC_U0TXD_CLK_OUT3                         1
#define FUNC_U0TXD_U0TXD                            0

#define PERIPHS_IO_MUX_GPIO2_U            (DR_REG_IO_MUX_BASE +0x40)
#define IO_MUX_GPIO2_REG                  PERIPHS_IO_MUX_GPIO2_U
#define FUNC_GPIO2_SD_DATA0                         4
#define FUNC_GPIO2_HS2_DATA0                        3
#define FUNC_GPIO2_GPIO2                            2
#define FUNC_GPIO2_HSPIWP                           1
#define FUNC_GPIO2_GPIO2_0                          0

#define PERIPHS_IO_MUX_U0RXD_U            (DR_REG_IO_MUX_BASE +0x84)
#define IO_MUX_GPIO3_REG                    PERIPHS_IO_MUX_U0RXD_U
#define FUNC_U0RXD_GPIO3                            2
#define FUNC_U0RXD_CLK_OUT2                         1
#define FUNC_U0RXD_U0RXD                            0

#define PERIPHS_IO_MUX_GPIO4_U            (DR_REG_IO_MUX_BASE +0x48)
#define IO_MUX_GPIO4_REG                    PERIPHS_IO_MUX_GPIO4_U
#define FUNC_GPIO4_EMAC_TX_ER                       5
#define FUNC_GPIO4_SD_DATA1                         4
#define FUNC_GPIO4_HS2_DATA1                        3
#define FUNC_GPIO4_GPIO4                            2
#define FUNC_GPIO4_HSPIHD                           1
#define FUNC_GPIO4_GPIO4_0                          0

#define PERIPHS_IO_MUX_GPIO5_U            (DR_REG_IO_MUX_BASE +0x6c)
#define IO_MUX_GPIO5_REG                   PERIPHS_IO_MUX_GPIO5_U
#define FUNC_GPIO5_EMAC_RX_CLK                      5
#define FUNC_GPIO5_HS1_DATA6                        3
#define FUNC_GPIO5_GPIO5                            2
#define FUNC_GPIO5_VSPICS0                          1
#define FUNC_GPIO5_GPIO5_0                          0

#define PERIPHS_IO_MUX_SD_CLK_U           (DR_REG_IO_MUX_BASE +0x60)
#define IO_MUX_GPIO6_REG                    PERIPHS_IO_MUX_SD_CLK_U
#define FUNC_SD_CLK_U1CTS                           4
#define FUNC_SD_CLK_HS1_CLK                         3
#define FUNC_SD_CLK_GPIO6                           2
#define FUNC_SD_CLK_SPICLK                          1
#define FUNC_SD_CLK_SD_CLK                          0

#define PERIPHS_IO_MUX_SD_DATA0_U         (DR_REG_IO_MUX_BASE +0x64)
#define IO_MUX_GPIO7_REG                    PERIPHS_IO_MUX_SD_DATA0_U
#define FUNC_SD_DATA0_U2RTS                         4
#define FUNC_SD_DATA0_HS1_DATA0                     3
#define FUNC_SD_DATA0_GPIO7                         2
#define FUNC_SD_DATA0_SPIQ                          1
#define FUNC_SD_DATA0_SD_DATA0                      0

#define PERIPHS_IO_MUX_SD_DATA1_U         (DR_REG_IO_MUX_BASE +0x68)
#define IO_MUX_GPIO8_REG                    PERIPHS_IO_MUX_SD_DATA1_U
#define FUNC_SD_DATA1_U2CTS                         4
#define FUNC_SD_DATA1_HS1_DATA1                     3
#define FUNC_SD_DATA1_GPIO8                         2
#define FUNC_SD_DATA1_SPID                          1
#define FUNC_SD_DATA1_SD_DATA1                      0

#define PERIPHS_IO_MUX_SD_DATA2_U         (DR_REG_IO_MUX_BASE +0x54)
#define IO_MUX_GPIO9_REG                    PERIPHS_IO_MUX_SD_DATA2_U
#define FUNC_SD_DATA2_U1RXD                         4
#define FUNC_SD_DATA2_HS1_DATA2                     3
#define FUNC_SD_DATA2_GPIO9                         2
#define FUNC_SD_DATA2_SPIHD                         1
#define FUNC_SD_DATA2_SD_DATA2                      0

#define PERIPHS_IO_MUX_SD_DATA3_U         (DR_REG_IO_MUX_BASE +0x58)
#define IO_MUX_GPIO10_REG                   PERIPHS_IO_MUX_SD_DATA3_U
#define FUNC_SD_DATA3_U1TXD                         4
#define FUNC_SD_DATA3_HS1_DATA3                     3
#define FUNC_SD_DATA3_GPIO10                        2
#define FUNC_SD_DATA3_SPIWP                         1
#define FUNC_SD_DATA3_SD_DATA3                      0

#define PERIPHS_IO_MUX_SD_CMD_U           (DR_REG_IO_MUX_BASE +0x5c)
#define IO_MUX_GPIO11_REG                   PERIPHS_IO_MUX_SD_CMD_U
#define FUNC_SD_CMD_U1RTS                           4
#define FUNC_SD_CMD_HS1_CMD                         3
#define FUNC_SD_CMD_GPIO11                          2
#define FUNC_SD_CMD_SPICS0                          1
#define FUNC_SD_CMD_SD_CMD                          0

#define PERIPHS_IO_MUX_MTDI_U             (DR_REG_IO_MUX_BASE +0x34)
#define IO_MUX_GPIO12_REG                    PERIPHS_IO_MUX_MTDI_U
#define FUNC_MTDI_EMAC_TXD3                         5
#define FUNC_MTDI_SD_DATA2                          4
#define FUNC_MTDI_HS2_DATA2                         3
#define FUNC_MTDI_GPIO12                            2
#define FUNC_MTDI_HSPIQ                             1
#define FUNC_MTDI_MTDI                              0

#define PERIPHS_IO_MUX_MTCK_U             (DR_REG_IO_MUX_BASE +0x38)
#define IO_MUX_GPIO13_REG                    PERIPHS_IO_MUX_MTCK_U
#define FUNC_MTCK_EMAC_RX_ER                        5
#define FUNC_MTCK_SD_DATA3                          4
#define FUNC_MTCK_HS2_DATA3                         3
#define FUNC_MTCK_GPIO13                            2
#define FUNC_MTCK_HSPID                             1
#define FUNC_MTCK_MTCK                              0

#define PERIPHS_IO_MUX_MTMS_U             (DR_REG_IO_MUX_BASE +0x30)
#define IO_MUX_GPIO14_REG                    PERIPHS_IO_MUX_MTMS_U
#define FUNC_MTMS_EMAC_TXD2                         5
#define FUNC_MTMS_SD_CLK                            4
#define FUNC_MTMS_HS2_CLK                           3
#define FUNC_MTMS_GPIO14                            2
#define FUNC_MTMS_HSPICLK                           1
#define FUNC_MTMS_MTMS                              0

#define PERIPHS_IO_MUX_MTDO_U             (DR_REG_IO_MUX_BASE +0x3c)
#define IO_MUX_GPIO15_REG                    PERIPHS_IO_MUX_MTDO_U
#define FUNC_MTDO_EMAC_RXD3                         5
#define FUNC_MTDO_SD_CMD                            4
#define FUNC_MTDO_HS2_CMD                           3
#define FUNC_MTDO_GPIO15                            2
#define FUNC_MTDO_HSPICS0                           1
#define FUNC_MTDO_MTDO                              0

#define PERIPHS_IO_MUX_GPIO16_U           (DR_REG_IO_MUX_BASE +0x4c)
#define IO_MUX_GPIO16_REG                    PERIPHS_IO_MUX_GPIO16_U
#define FUNC_GPIO16_EMAC_CLK_OUT                    5
#define FUNC_GPIO16_U2RXD                           4
#define FUNC_GPIO16_HS1_DATA4                       3
#define FUNC_GPIO16_GPIO16                          2
#define FUNC_GPIO16_GPIO16_0                        0

#define PERIPHS_IO_MUX_GPIO17_U           (DR_REG_IO_MUX_BASE +0x50)
#define IO_MUX_GPIO17_REG                    PERIPHS_IO_MUX_GPIO17_U
#define FUNC_GPIO17_EMAC_CLK_OUT_180                5
#define FUNC_GPIO17_U2TXD                           4
#define FUNC_GPIO17_HS1_DATA5                       3
#define FUNC_GPIO17_GPIO17                          2
#define FUNC_GPIO17_GPIO17_0                        0

#define PERIPHS_IO_MUX_GPIO18_U           (DR_REG_IO_MUX_BASE +0x70)
#define IO_MUX_GPIO18_REG                    PERIPHS_IO_MUX_GPIO18_U
#define FUNC_GPIO18_HS1_DATA7                       3
#define FUNC_GPIO18_GPIO18                          2
#define FUNC_GPIO18_VSPICLK                         1
#define FUNC_GPIO18_GPIO18_0                        0

#define PERIPHS_IO_MUX_GPIO19_U           (DR_REG_IO_MUX_BASE +0x74)
#define IO_MUX_GPIO19_REG                    PERIPHS_IO_MUX_GPIO19_U
#define FUNC_GPIO19_EMAC_TXD0                       5
#define FUNC_GPIO19_U0CTS                           3
#define FUNC_GPIO19_GPIO19                          2
#define FUNC_GPIO19_VSPIQ                           1
#define FUNC_GPIO19_GPIO19_0                        0

#define PERIPHS_IO_MUX_GPIO20_U           (DR_REG_IO_MUX_BASE +0x78)
#define IO_MUX_GPIO20_REG                    PERIPHS_IO_MUX_GPIO20_U
#define FUNC_GPIO20_GPIO20                          2
#define FUNC_GPIO20_GPIO20_0                        0

#define PERIPHS_IO_MUX_GPIO21_U           (DR_REG_IO_MUX_BASE +0x7c)
#define IO_MUX_GPIO21_REG                    PERIPHS_IO_MUX_GPIO21_U
#define FUNC_GPIO21_EMAC_TX_EN                      5
#define FUNC_GPIO21_GPIO21                          2
#define FUNC_GPIO21_VSPIHD                          1
#define FUNC_GPIO21_GPIO21_0                        0

#define PERIPHS_IO_MUX_GPIO22_U           (DR_REG_IO_MUX_BASE +0x80)
#define IO_MUX_GPIO22_REG                    PERIPHS_IO_MUX_GPIO22_U
#define FUNC_GPIO22_EMAC_TXD1                       5
#define FUNC_GPIO22_U0RTS                           3
#define FUNC_GPIO22_GPIO22                          2
#define FUNC_GPIO22_VSPIWP                          1
#define FUNC_GPIO22_GPIO22_0                        0

#define PERIPHS_IO_MUX_GPIO23_U           (DR_REG_IO_MUX_BASE +0x8c)
#define IO_MUX_GPIO23_REG                    PERIPHS_IO_MUX_GPIO23_U
#define FUNC_GPIO23_HS1_STROBE                      3
#define FUNC_GPIO23_GPIO23                          2
#define FUNC_GPIO23_VSPID                           1
#define FUNC_GPIO23_GPIO23_0                        0

#define PERIPHS_IO_MUX_GPIO24_U           (DR_REG_IO_MUX_BASE +0x90)
#define IO_MUX_GPIO24_REG                    PERIPHS_IO_MUX_GPIO24_U
#define FUNC_GPIO24_GPIO24                          2
#define FUNC_GPIO24_GPIO24_0                        0

#define PERIPHS_IO_MUX_GPIO25_U           (DR_REG_IO_MUX_BASE +0x24)
#define IO_MUX_GPIO25_REG                    PERIPHS_IO_MUX_GPIO25_U
#define FUNC_GPIO25_EMAC_RXD0                       5
#define FUNC_GPIO25_GPIO25                          2
#define FUNC_GPIO25_GPIO25_0                        0

#define PERIPHS_IO_MUX_GPIO26_U           (DR_REG_IO_MUX_BASE +0x28)
#define IO_MUX_GPIO26_REG                    PERIPHS_IO_MUX_GPIO26_U
#define FUNC_GPIO26_EMAC_RXD1                       5
#define FUNC_GPIO26_GPIO26                          2
#define FUNC_GPIO26_GPIO26_0                        0

#define PERIPHS_IO_MUX_GPIO27_U           (DR_REG_IO_MUX_BASE +0x2c)
#define IO_MUX_GPIO27_REG                    PERIPHS_IO_MUX_GPIO27_U
#define FUNC_GPIO27_EMAC_RX_DV                      5
#define FUNC_GPIO27_GPIO27                          2
#define FUNC_GPIO27_GPIO27_0                        0

#define PERIPHS_IO_MUX_GPIO32_U           (DR_REG_IO_MUX_BASE +0x1c)
#define IO_MUX_GPIO32_REG                    PERIPHS_IO_MUX_GPIO32_U
#define FUNC_GPIO32_GPIO32                          2
#define FUNC_GPIO32_GPIO32_0                        0

#define PERIPHS_IO_MUX_GPIO33_U           (DR_REG_IO_MUX_BASE +0x20)
#define IO_MUX_GPIO33_REG                    PERIPHS_IO_MUX_GPIO33_U
#define FUNC_GPIO33_GPIO33                          2
#define FUNC_GPIO33_GPIO33_0                        0

#define PERIPHS_IO_MUX_GPIO34_U           (DR_REG_IO_MUX_BASE +0x14)
#define IO_MUX_GPIO34_REG                    PERIPHS_IO_MUX_GPIO34_U
#define FUNC_GPIO34_GPIO34                          2
#define FUNC_GPIO34_GPIO34_0                        0

#define PERIPHS_IO_MUX_GPIO35_U           (DR_REG_IO_MUX_BASE +0x18)
#define IO_MUX_GPIO35_REG                    PERIPHS_IO_MUX_GPIO35_U
#define FUNC_GPIO35_GPIO35                          2
#define FUNC_GPIO35_GPIO35_0                        0

#define PERIPHS_IO_MUX_GPIO36_U           (DR_REG_IO_MUX_BASE +0x04)
#define IO_MUX_GPIO36_REG                    PERIPHS_IO_MUX_GPIO36_U
#define FUNC_GPIO36_GPIO36                          2
#define FUNC_GPIO36_GPIO36_0                        0

#define PERIPHS_IO_MUX_GPIO37_U           (DR_REG_IO_MUX_BASE +0x08)
#define IO_MUX_GPIO37_REG                    PERIPHS_IO_MUX_GPIO37_U
#define FUNC_GPIO37_GPIO37                          2
#define FUNC_GPIO37_GPIO37_0                        0

#define PERIPHS_IO_MUX_GPIO38_U           (DR_REG_IO_MUX_BASE +0x0c)
#define IO_MUX_GPIO38_REG                    PERIPHS_IO_MUX_GPIO38_U
#define FUNC_GPIO38_GPIO38                          2
#define FUNC_GPIO38_GPIO38_0                        0

#define PERIPHS_IO_MUX_GPIO39_U           (DR_REG_IO_MUX_BASE +0x10)
#define IO_MUX_GPIO39_REG                    PERIPHS_IO_MUX_GPIO39_U
#define FUNC_GPIO39_GPIO39                          2
#define FUNC_GPIO39_GPIO39_0                        0

#endif /* _SOC_IO_MUX_REG_H_ */
