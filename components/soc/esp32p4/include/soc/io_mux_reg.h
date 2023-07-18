/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
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
/* Pin filter (Pulse width shorter than 2 clock cycles will be filtered out) */
#define FILTER_EN (BIT(15))
#define FILTER_EN_M (FILTER_EN_V << FILTER_EN_S)
#define FILTER_EN_V 1
#define FILTER_EN_S 15

#define PIN_SLP_INPUT_ENABLE(PIN_NAME)      SET_PERI_REG_MASK(PIN_NAME,SLP_IE)
#define PIN_SLP_INPUT_DISABLE(PIN_NAME)     CLEAR_PERI_REG_MASK(PIN_NAME,SLP_IE)
#define PIN_SLP_OUTPUT_ENABLE(PIN_NAME)     SET_PERI_REG_MASK(PIN_NAME,SLP_OE)
#define PIN_SLP_OUTPUT_DISABLE(PIN_NAME)    CLEAR_PERI_REG_MASK(PIN_NAME,SLP_OE)
#define PIN_SLP_PULLUP_ENABLE(PIN_NAME)     SET_PERI_REG_MASK(PIN_NAME,SLP_PU)
#define PIN_SLP_PULLUP_DISABLE(PIN_NAME)    CLEAR_PERI_REG_MASK(PIN_NAME,SLP_PU)
#define PIN_SLP_PULLDOWN_ENABLE(PIN_NAME)   SET_PERI_REG_MASK(PIN_NAME,SLP_PD)
#define PIN_SLP_PULLDOWN_DISABLE(PIN_NAME)  CLEAR_PERI_REG_MASK(PIN_NAME,SLP_PD)
#define PIN_SLP_SEL_ENABLE(PIN_NAME)        SET_PERI_REG_MASK(PIN_NAME,SLP_SEL)
#define PIN_SLP_SEL_DISABLE(PIN_NAME)       CLEAR_PERI_REG_MASK(PIN_NAME,SLP_SEL)

#define PIN_INPUT_ENABLE(PIN_NAME)          SET_PERI_REG_MASK(PIN_NAME,FUN_IE)
#define PIN_INPUT_DISABLE(PIN_NAME)         CLEAR_PERI_REG_MASK(PIN_NAME,FUN_IE)
#define PIN_SET_DRV(PIN_NAME, drv)          REG_SET_FIELD(PIN_NAME, FUN_DRV, (drv));
#define PIN_PULLUP_DIS(PIN_NAME)            REG_CLR_BIT(PIN_NAME, FUN_PU)
#define PIN_PULLUP_EN(PIN_NAME)             REG_SET_BIT(PIN_NAME, FUN_PU)
#define PIN_PULLDWN_DIS(PIN_NAME)           REG_CLR_BIT(PIN_NAME, FUN_PD)
#define PIN_PULLDWN_EN(PIN_NAME)            REG_SET_BIT(PIN_NAME, FUN_PD)
#define PIN_FUNC_SELECT(PIN_NAME, FUNC)     REG_SET_FIELD(PIN_NAME, MCU_SEL, FUNC)
#define PIN_FILTER_EN(PIN_NAME)             REG_SET_BIT(PIN_NAME, FILTER_EN)
#define PIN_FILTER_DIS(PIN_NAME)            REG_CLR_BIT(PIN_NAME, FILTER_EN)

#define IO_MUX_GPIO0_REG                    PERIPHS_IO_MUX_U_PAD_GPIO0
#define IO_MUX_GPIO1_REG                    PERIPHS_IO_MUX_U_PAD_GPIO1
#define IO_MUX_GPIO2_REG                    PERIPHS_IO_MUX_U_PAD_GPIO2
#define IO_MUX_GPIO3_REG                    PERIPHS_IO_MUX_U_PAD_GPIO3
#define IO_MUX_GPIO4_REG                    PERIPHS_IO_MUX_U_PAD_GPIO4
#define IO_MUX_GPIO5_REG                    PERIPHS_IO_MUX_U_PAD_GPIO5
#define IO_MUX_GPIO6_REG                    PERIPHS_IO_MUX_U_PAD_GPIO6
#define IO_MUX_GPIO7_REG                    PERIPHS_IO_MUX_U_PAD_GPIO7
#define IO_MUX_GPIO8_REG                    PERIPHS_IO_MUX_U_PAD_GPIO8
#define IO_MUX_GPIO9_REG                    PERIPHS_IO_MUX_U_PAD_GPIO9
#define IO_MUX_GPIO10_REG                   PERIPHS_IO_MUX_U_PAD_GPIO10
#define IO_MUX_GPIO11_REG                   PERIPHS_IO_MUX_U_PAD_GPIO11
#define IO_MUX_GPIO12_REG                   PERIPHS_IO_MUX_U_PAD_GPIO12
#define IO_MUX_GPIO13_REG                   PERIPHS_IO_MUX_U_PAD_GPIO13
#define IO_MUX_GPIO14_REG                   PERIPHS_IO_MUX_U_PAD_GPIO14
#define IO_MUX_GPIO15_REG                   PERIPHS_IO_MUX_U_PAD_GPIO15
#define IO_MUX_GPIO16_REG                   PERIPHS_IO_MUX_U_PAD_GPIO16
#define IO_MUX_GPIO17_REG                   PERIPHS_IO_MUX_U_PAD_GPIO17
#define IO_MUX_GPIO18_REG                   PERIPHS_IO_MUX_U_PAD_GPIO18
#define IO_MUX_GPIO19_REG                   PERIPHS_IO_MUX_U_PAD_GPIO19
#define IO_MUX_GPIO20_REG                   PERIPHS_IO_MUX_U_PAD_GPIO20
#define IO_MUX_GPIO21_REG                   PERIPHS_IO_MUX_U_PAD_GPIO21
#define IO_MUX_GPIO22_REG                   PERIPHS_IO_MUX_U_PAD_GPIO22
#define IO_MUX_GPIO23_REG                   PERIPHS_IO_MUX_U_PAD_GPIO23
#define IO_MUX_GPIO24_REG                   PERIPHS_IO_MUX_U_PAD_GPIO24
#define IO_MUX_GPIO25_REG                   PERIPHS_IO_MUX_U_PAD_GPIO25
#define IO_MUX_GPIO26_REG                   PERIPHS_IO_MUX_U_PAD_GPIO26
#define IO_MUX_GPIO27_REG                   PERIPHS_IO_MUX_U_PAD_GPIO27
#define IO_MUX_GPIO28_REG                   PERIPHS_IO_MUX_U_PAD_GPIO28
#define IO_MUX_GPIO29_REG                   PERIPHS_IO_MUX_U_PAD_GPIO29
#define IO_MUX_GPIO30_REG                   PERIPHS_IO_MUX_U_PAD_GPIO30
#define IO_MUX_GPIO31_REG                   PERIPHS_IO_MUX_U_PAD_GPIO31
#define IO_MUX_GPIO32_REG                   PERIPHS_IO_MUX_U_PAD_GPIO32
#define IO_MUX_GPIO33_REG                   PERIPHS_IO_MUX_U_PAD_GPIO33
#define IO_MUX_GPIO34_REG                   PERIPHS_IO_MUX_U_PAD_GPIO34
#define IO_MUX_GPIO35_REG                   PERIPHS_IO_MUX_U_PAD_GPIO35
#define IO_MUX_GPIO36_REG                   PERIPHS_IO_MUX_U_PAD_GPIO36
#define IO_MUX_GPIO37_REG                   PERIPHS_IO_MUX_U_PAD_GPIO37
#define IO_MUX_GPIO38_REG                   PERIPHS_IO_MUX_U_PAD_GPIO38
#define IO_MUX_GPIO39_REG                   PERIPHS_IO_MUX_U_PAD_GPIO39
#define IO_MUX_GPIO40_REG                   PERIPHS_IO_MUX_U_PAD_GPIO40
#define IO_MUX_GPIO41_REG                   PERIPHS_IO_MUX_U_PAD_GPIO41
#define IO_MUX_GPIO42_REG                   PERIPHS_IO_MUX_U_PAD_GPIO42
#define IO_MUX_GPIO43_REG                   PERIPHS_IO_MUX_U_PAD_GPIO43
#define IO_MUX_GPIO44_REG                   PERIPHS_IO_MUX_U_PAD_GPIO44
#define IO_MUX_GPIO45_REG                   PERIPHS_IO_MUX_U_PAD_GPIO45
#define IO_MUX_GPIO46_REG                   PERIPHS_IO_MUX_U_PAD_GPIO46
#define IO_MUX_GPIO47_REG                   PERIPHS_IO_MUX_U_PAD_GPIO47
#define IO_MUX_GPIO48_REG                   PERIPHS_IO_MUX_U_PAD_GPIO48
#define IO_MUX_GPIO49_REG                   PERIPHS_IO_MUX_U_PAD_GPIO49
#define IO_MUX_GPIO50_REG                   PERIPHS_IO_MUX_U_PAD_GPIO50
#define IO_MUX_GPIO51_REG                   PERIPHS_IO_MUX_U_PAD_GPIO51
#define IO_MUX_GPIO52_REG                   PERIPHS_IO_MUX_U_PAD_GPIO52
#define IO_MUX_GPIO53_REG                   PERIPHS_IO_MUX_U_PAD_GPIO53
#define IO_MUX_GPIO54_REG                   PERIPHS_IO_MUX_U_PAD_GPIO54
#define IO_MUX_GPIO55_REG                   PERIPHS_IO_MUX_U_PAD_GPIO55
#define IO_MUX_GPIO56_REG                   PERIPHS_IO_MUX_U_PAD_GPIO56

#define PIN_FUNC_GPIO								1

#define GPIO_PAD_PULLUP(num) do{PIN_PULLDWN_DIS(IOMUX_REG_GPIO##num);PIN_PULLUP_EN(IOMUX_REG_GPIO##num);}while(0)
#define GPIO_PAD_PULLDOWN(num) do{PIN_PULLUP_DIS(IOMUX_REG_GPIO##num);PIN_PULLDWN_EN(IOMUX_REG_GPIO##num);}while(0)
#define GPIO_PAD_SET_DRV(num, drv) PIN_SET_DRV(IOMUX_REG_GPIO##num, drv)

// TODO: IDF-7499, IDF-7495
// Pins defined here are all wrong (Ln153-164). On P4, these pins are individual pins, don't use normal GPIO pins anymore.
// Please check iomux_mspi_pin_struct/reg.h
#define SPI_CS1_GPIO_NUM 26
#define SPI_HD_GPIO_NUM  27
#define SPI_WP_GPIO_NUM  28
#define SPI_CS0_GPIO_NUM 29
#define SPI_CLK_GPIO_NUM 30
#define SPI_Q_GPIO_NUM   31
#define SPI_D_GPIO_NUM   32
#define SPI_D4_GPIO_NUM  33
#define SPI_D5_GPIO_NUM  34
#define SPI_D6_GPIO_NUM  35
#define SPI_D7_GPIO_NUM  36
#define SPI_DQS_GPIO_NUM 37

#define PIN_FUNC_SPI_DEBUG        4
#define FLASH_CS_DEBUG_GPIO_NUM   49
#define FLASH_Q_DEBUG_GPIO_NUM    50
#define FLASH_WP_DEBUG_GPIO_NUM   51
#define FLASH_HD_DEBUG_GPIO_NUM   52
#define FLASH_CLK_DEBUG_GPIO_NUM  53
#define FLASH_D_DEBUG_GPIO_NUM    54

#define PSRAM_D_DEBUG_GPIO_NUM    28
#define PSRAM_Q_DEBUG_GPIO_NUM    29
#define PSRAM_WP_DEBUG_GPIO_NUM   30
#define PSRAM_HOLD_DEBUG_GPIO_NUM 31
#define PSRAM_DP4_DEBUG_GPIO_NUM  32
#define PSRAM_DP5_DEBUG_GPIO_NUM  33
#define PSRAM_DP6_DEBUG_GPIO_NUM  34
#define PSRAM_DP7_DEBUG_GPIO_NUM  35
#define PSRAM_DQS0_DEBUG_GPIO_NUM 36
#define PSRAM_CLK_DEBUG_GPIO_NUM  22
#define PSRAM_CS_DEBUG_GPIO_NUM   23
#define PSRAM_DP8_DEBUG_GPIO_NUM  39
#define PSRAM_DP9_DEBUG_GPIO_NUM  40
#define PSRAM_DP10_DEBUG_GPIO_NUM 41
#define PSRAM_DP11_DEBUG_GPIO_NUM 42
#define PSRAM_DP12_DEBUG_GPIO_NUM 43
#define PSRAM_DP13_DEBUG_GPIO_NUM 44
#define PSRAM_DP14_DEBUG_GPIO_NUM 45
#define PSRAM_DP15_DEBUG_GPIO_NUM 46
#define PSRAM_DQS1_DEBUG_GPIO_NUM 47

#define SD_CLK_GPIO_NUM           43
#define SD_CMD_GPIO_NUM           44
#define SD_DATA0_GPIO_NUM         39
#define SD_DATA1_GPIO_NUM         40
#define SD_DATA2_GPIO_NUM         41
#define SD_DATA3_GPIO_NUM         42
#define SD_DATA4_GPIO_NUM         45
#define SD_DATA5_GPIO_NUM         46
#define SD_DATA6_GPIO_NUM         47
#define SD_DATA7_GPIO_NUM         48

#define USB_INT_PHY0_DM_GPIO_NUM  24
#define USB_INT_PHY0_DP_GPIO_NUM  25
#define USB_INT_PHY1_DM_GPIO_NUM  26
#define USB_INT_PHY1_DP_GPIO_NUM  27

// We would fix the USB PHY usage on P4: PHY0 -> USJ, PHY1 -> USB_OTG
#define USB_USJ_INT_PHY_DM_GPIO_NUM     USB_INT_PHY0_DM_GPIO_NUM
#define USB_USJ_INT_PHY_DP_GPIO_NUM     USB_INT_PHY0_DP_GPIO_NUM
#define USB_OTG_INT_PHY_DM_GPIO_NUM     USB_INT_PHY1_DM_GPIO_NUM
#define USB_OTG_INT_PHY_DP_GPIO_NUM     USB_INT_PHY1_DP_GPIO_NUM

// #define EXT_OSC_SLOW_GPIO_NUM     1 // TODO: IDF-7526

#define MAX_RTC_GPIO_NUM             16
#define MAX_PAD_GPIO_NUM             56
#define MAX_GPIO_NUM                 56
#define HIGH_IO_HOLD_BIT_SHIFT       32


#define REG_IO_MUX_BASE DR_REG_IO_MUX_BASE

// definitions below are generated from pin_txt.csv
#define PERIPHS_IO_MUX_U_PAD_GPIO0                 (REG_IO_MUX_BASE + 0x4)
#define FUNC_GPIO0_GPIO0                                                 1
#define FUNC_GPIO0_GPIO0_0                                               0

#define PERIPHS_IO_MUX_U_PAD_GPIO1                 (REG_IO_MUX_BASE + 0x8)
#define FUNC_GPIO1_GPIO1                                                 1
#define FUNC_GPIO1_GPIO1_0                                               0

#define PERIPHS_IO_MUX_U_PAD_GPIO2                 (REG_IO_MUX_BASE + 0xC)
#define FUNC_GPIO2_GPIO2                                                 1
#define FUNC_GPIO2_MTCK                                                  0

#define PERIPHS_IO_MUX_U_PAD_GPIO3                (REG_IO_MUX_BASE + 0x10)
#define FUNC_GPIO3_GPIO3                                                 1
#define FUNC_GPIO3_MTDI                                                  0

#define PERIPHS_IO_MUX_U_PAD_GPIO4                (REG_IO_MUX_BASE + 0x14)
#define FUNC_GPIO4_GPIO4                                                 1
#define FUNC_GPIO4_MTMS                                                  0

#define PERIPHS_IO_MUX_U_PAD_GPIO5                (REG_IO_MUX_BASE + 0x18)
#define FUNC_GPIO5_GPIO5                                                 1
#define FUNC_GPIO5_MTDO                                                  0

#define PERIPHS_IO_MUX_U_PAD_GPIO6                (REG_IO_MUX_BASE + 0x1C)
#define FUNC_GPIO6_SPI2_HOLD_PAD                                         3
#define FUNC_GPIO6_GPIO6                                                 1
#define FUNC_GPIO6_GPIO6_0                                               0

#define PERIPHS_IO_MUX_U_PAD_GPIO7                (REG_IO_MUX_BASE + 0x20)
#define FUNC_GPIO7_SPI2_CS_PAD                                           3
#define FUNC_GPIO7_GPIO7                                                 1
#define FUNC_GPIO7_GPIO7_0                                               0

#define PERIPHS_IO_MUX_U_PAD_GPIO8                (REG_IO_MUX_BASE + 0x24)
#define FUNC_GPIO8_SPI2_D_PAD                                            3
#define FUNC_GPIO8_UART0_RTS_PAD                                         2
#define FUNC_GPIO8_GPIO8                                                 1
#define FUNC_GPIO8_GPIO8_0                                               0

#define PERIPHS_IO_MUX_U_PAD_GPIO9                (REG_IO_MUX_BASE + 0x28)
#define FUNC_GPIO9_SPI2_CK_PAD                                           3
#define FUNC_GPIO9_UART0_CTS_PAD                                         2
#define FUNC_GPIO9_GPIO9                                                 1
#define FUNC_GPIO9_GPIO9_0                                               0

#define PERIPHS_IO_MUX_U_PAD_GPIO10               (REG_IO_MUX_BASE + 0x2C)
#define FUNC_GPIO10_SPI2_Q_PAD                                           3
#define FUNC_GPIO10_UART1_TXD_PAD                                        2
#define FUNC_GPIO10_GPIO10                                               1
#define FUNC_GPIO10_GPIO10_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO11               (REG_IO_MUX_BASE + 0x30)
#define FUNC_GPIO11_SPI2_WP_PAD                                          3
#define FUNC_GPIO11_UART1_RXD_PAD                                        2
#define FUNC_GPIO11_GPIO11                                               1
#define FUNC_GPIO11_GPIO11_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO12               (REG_IO_MUX_BASE + 0x34)
#define FUNC_GPIO12_UART1_RTS_PAD                                        2
#define FUNC_GPIO12_GPIO12                                               1
#define FUNC_GPIO12_GPIO12_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO13               (REG_IO_MUX_BASE + 0x38)
#define FUNC_GPIO13_UART1_CTS_PAD                                        2
#define FUNC_GPIO13_GPIO13                                               1
#define FUNC_GPIO13_GPIO13_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO14               (REG_IO_MUX_BASE + 0x3C)
#define FUNC_GPIO14_GPIO14                                               1
#define FUNC_GPIO14_GPIO14_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO15               (REG_IO_MUX_BASE + 0x40)
#define FUNC_GPIO15_GPIO15                                               1
#define FUNC_GPIO15_GPIO15_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO16               (REG_IO_MUX_BASE + 0x44)
#define FUNC_GPIO16_GPIO16                                               1
#define FUNC_GPIO16_GPIO16_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO17               (REG_IO_MUX_BASE + 0x48)
#define FUNC_GPIO17_GPIO17                                               1
#define FUNC_GPIO17_GPIO17_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO18               (REG_IO_MUX_BASE + 0x4C)
#define FUNC_GPIO18_GPIO18                                               1
#define FUNC_GPIO18_GPIO18_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO19               (REG_IO_MUX_BASE + 0x50)
#define FUNC_GPIO19_GPIO19                                               1
#define FUNC_GPIO19_GPIO19_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO20               (REG_IO_MUX_BASE + 0x54)
#define FUNC_GPIO20_GPIO20                                               1
#define FUNC_GPIO20_GPIO20_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO21               (REG_IO_MUX_BASE + 0x58)
#define FUNC_GPIO21_GPIO21                                               1
#define FUNC_GPIO21_GPIO21_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO22               (REG_IO_MUX_BASE + 0x5C)
#define FUNC_GPIO22_DBG_PSRAM_CK_PAD                                     4
#define FUNC_GPIO22_GPIO22                                               1
#define FUNC_GPIO22_GPIO22_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO23               (REG_IO_MUX_BASE + 0x60)
#define FUNC_GPIO23_DBG_PSRAM_CS_PAD                                     4
#define FUNC_GPIO23_REF_50M_CLK_PAD                                      3
#define FUNC_GPIO23_GPIO23                                               1
#define FUNC_GPIO23_GPIO23_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO24               (REG_IO_MUX_BASE + 0x64)
#define FUNC_GPIO24_GPIO24                                               1
#define FUNC_GPIO24_GPIO24_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO25               (REG_IO_MUX_BASE + 0x68)
#define FUNC_GPIO25_GPIO25                                               1
#define FUNC_GPIO25_GPIO25_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO26               (REG_IO_MUX_BASE + 0x6C)
#define FUNC_GPIO26_GPIO26                                               1
#define FUNC_GPIO26_GPIO26_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO27               (REG_IO_MUX_BASE + 0x70)
#define FUNC_GPIO27_GPIO27                                               1
#define FUNC_GPIO27_GPIO27_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO28               (REG_IO_MUX_BASE + 0x74)
#define FUNC_GPIO28_DBG_PSRAM_D_PAD                                      4
#define FUNC_GPIO28_GMAC_PHY_RXDV_PAD                                    3
#define FUNC_GPIO28_SPI2_CS_PAD                                          2
#define FUNC_GPIO28_GPIO28                                               1
#define FUNC_GPIO28_GPIO28_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO29               (REG_IO_MUX_BASE + 0x78)
#define FUNC_GPIO29_DBG_PSRAM_Q_PAD                                      4
#define FUNC_GPIO29_GMAC_PHY_RXD0_PAD                                    3
#define FUNC_GPIO29_SPI2_D_PAD                                           2
#define FUNC_GPIO29_GPIO29                                               1
#define FUNC_GPIO29_GPIO29_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO30               (REG_IO_MUX_BASE + 0x7C)
#define FUNC_GPIO30_DBG_PSRAM_WP_PAD                                     4
#define FUNC_GPIO30_GMAC_PHY_RXD1_PAD                                    3
#define FUNC_GPIO30_SPI2_CK_PAD                                          2
#define FUNC_GPIO30_GPIO30                                               1
#define FUNC_GPIO30_GPIO30_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO31               (REG_IO_MUX_BASE + 0x80)
#define FUNC_GPIO31_DBG_PSRAM_HOLD_PAD                                   4
#define FUNC_GPIO31_GMAC_PHY_RXER_PAD                                    3
#define FUNC_GPIO31_SPI2_Q_PAD                                           2
#define FUNC_GPIO31_GPIO31                                               1
#define FUNC_GPIO31_GPIO31_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO32               (REG_IO_MUX_BASE + 0x84)
#define FUNC_GPIO32_DBG_PSRAM_DQ4_PAD                                    4
#define FUNC_GPIO32_GMAC_RMII_CLK_PAD                                    3
#define FUNC_GPIO32_SPI2_HOLD_PAD                                        2
#define FUNC_GPIO32_GPIO32                                               1
#define FUNC_GPIO32_GPIO32_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO33               (REG_IO_MUX_BASE + 0x88)
#define FUNC_GPIO33_DBG_PSRAM_DQ5_PAD                                    4
#define FUNC_GPIO33_GMAC_PHY_TXEN_PAD                                    3
#define FUNC_GPIO33_SPI2_WP_PAD                                          2
#define FUNC_GPIO33_GPIO33                                               1
#define FUNC_GPIO33_GPIO33_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO34               (REG_IO_MUX_BASE + 0x8C)
#define FUNC_GPIO34_DBG_PSRAM_DQ6_PAD                                    4
#define FUNC_GPIO34_GMAC_PHY_TXD0_PAD                                    3
#define FUNC_GPIO34_SPI2_IO4_PAD                                         2
#define FUNC_GPIO34_GPIO34                                               1
#define FUNC_GPIO34_GPIO34_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO35               (REG_IO_MUX_BASE + 0x90)
#define FUNC_GPIO35_DBG_PSRAM_DQ7_PAD                                    4
#define FUNC_GPIO35_GMAC_PHY_TXD1_PAD                                    3
#define FUNC_GPIO35_SPI2_IO5_PAD                                         2
#define FUNC_GPIO35_GPIO35                                               1
#define FUNC_GPIO35_GPIO35_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO36               (REG_IO_MUX_BASE + 0x94)
#define FUNC_GPIO36_DBG_PSRAM_DQS_0_PAD                                  4
#define FUNC_GPIO36_GMAC_PHY_TXER_PAD                                    3
#define FUNC_GPIO36_SPI2_IO6_PAD                                         2
#define FUNC_GPIO36_GPIO36                                               1
#define FUNC_GPIO36_GPIO36_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO37               (REG_IO_MUX_BASE + 0x98)
#define FUNC_GPIO37_SPI2_IO7_PAD                                         2
#define FUNC_GPIO37_GPIO37                                               1
#define FUNC_GPIO37_UART0_TXD_PAD                                        0

#define PERIPHS_IO_MUX_U_PAD_GPIO38               (REG_IO_MUX_BASE + 0x9C)
#define FUNC_GPIO38_SPI2_DQS_PAD                                         2
#define FUNC_GPIO38_GPIO38                                               1
#define FUNC_GPIO38_UART0_RXD_PAD                                        0

#define PERIPHS_IO_MUX_U_PAD_GPIO39               (REG_IO_MUX_BASE + 0xA0)
#define FUNC_GPIO39_DBG_PSRAM_DQ8_PAD                                    4
#define FUNC_GPIO39_REF_50M_CLK_PAD                                      3
#define FUNC_GPIO39_BIST_PAD                                             2
#define FUNC_GPIO39_GPIO39                                               1
#define FUNC_GPIO39_SD1_CDATA0_PAD                                       0

#define PERIPHS_IO_MUX_U_PAD_GPIO40               (REG_IO_MUX_BASE + 0xA4)
#define FUNC_GPIO40_DBG_PSRAM_DQ9_PAD                                    4
#define FUNC_GPIO40_GMAC_PHY_TXEN_PAD                                    3
#define FUNC_GPIO40_BIST_PAD                                             2
#define FUNC_GPIO40_GPIO40                                               1
#define FUNC_GPIO40_SD1_CDATA1_PAD                                       0

#define PERIPHS_IO_MUX_U_PAD_GPIO41               (REG_IO_MUX_BASE + 0xA8)
#define FUNC_GPIO41_DBG_PSRAM_DQ10_PAD                                   4
#define FUNC_GPIO41_GMAC_PHY_TXD0_PAD                                    3
#define FUNC_GPIO41_BIST_PAD                                             2
#define FUNC_GPIO41_GPIO41                                               1
#define FUNC_GPIO41_SD1_CDATA2_PAD                                       0

#define PERIPHS_IO_MUX_U_PAD_GPIO42               (REG_IO_MUX_BASE + 0xAC)
#define FUNC_GPIO42_DBG_PSRAM_DQ11_PAD                                   4
#define FUNC_GPIO42_GMAC_PHY_TXD1_PAD                                    3
#define FUNC_GPIO42_BIST_PAD                                             2
#define FUNC_GPIO42_GPIO42                                               1
#define FUNC_GPIO42_SD1_CDATA3_PAD                                       0

#define PERIPHS_IO_MUX_U_PAD_GPIO43               (REG_IO_MUX_BASE + 0xB0)
#define FUNC_GPIO43_DBG_PSRAM_DQ12_PAD                                   4
#define FUNC_GPIO43_GMAC_PHY_TXER_PAD                                    3
#define FUNC_GPIO43_BIST_PAD                                             2
#define FUNC_GPIO43_GPIO43                                               1
#define FUNC_GPIO43_SD1_CCLK_PAD                                         0

#define PERIPHS_IO_MUX_U_PAD_GPIO44               (REG_IO_MUX_BASE + 0xB4)
#define FUNC_GPIO44_DBG_PSRAM_DQ13_PAD                                   4
#define FUNC_GPIO44_GMAC_RMII_CLK_PAD                                    3
#define FUNC_GPIO44_BIST_PAD                                             2
#define FUNC_GPIO44_GPIO44                                               1
#define FUNC_GPIO44_SD1_CCMD_PAD                                         0

#define PERIPHS_IO_MUX_U_PAD_GPIO45               (REG_IO_MUX_BASE + 0xB8)
#define FUNC_GPIO45_DBG_PSRAM_DQ14_PAD                                   4
#define FUNC_GPIO45_GMAC_PHY_RXDV_PAD                                    3
#define FUNC_GPIO45_BIST_PAD                                             2
#define FUNC_GPIO45_GPIO45                                               1
#define FUNC_GPIO45_SD1_CDATA4_PAD                                       0

#define PERIPHS_IO_MUX_U_PAD_GPIO46               (REG_IO_MUX_BASE + 0xBC)
#define FUNC_GPIO46_DBG_PSRAM_DQ15_PAD                                   4
#define FUNC_GPIO46_GMAC_PHY_RXD0_PAD                                    3
#define FUNC_GPIO46_BIST_PAD                                             2
#define FUNC_GPIO46_GPIO46                                               1
#define FUNC_GPIO46_SD1_CDATA5_PAD                                       0

#define PERIPHS_IO_MUX_U_PAD_GPIO47               (REG_IO_MUX_BASE + 0xC0)
#define FUNC_GPIO47_DBG_PSRAM_DQS_1_PAD                                  4
#define FUNC_GPIO47_GMAC_PHY_RXD1_PAD                                    3
#define FUNC_GPIO47_BIST_PAD                                             2
#define FUNC_GPIO47_GPIO47                                               1
#define FUNC_GPIO47_SD1_CDATA6_PAD                                       0

#define PERIPHS_IO_MUX_U_PAD_GPIO48               (REG_IO_MUX_BASE + 0xC4)
#define FUNC_GPIO48_GMAC_PHY_RXER_PAD                                    3
#define FUNC_GPIO48_BIST_PAD                                             2
#define FUNC_GPIO48_GPIO48                                               1
#define FUNC_GPIO48_SD1_CDATA7_PAD                                       0

#define PERIPHS_IO_MUX_U_PAD_GPIO49               (REG_IO_MUX_BASE + 0xC8)
#define FUNC_GPIO49_DBG_FLASH_CS_PAD                                      4
#define FUNC_GPIO49_GMAC_PHY_TXEN_PAD                                    3
#define FUNC_GPIO49_GPIO49                                               1
#define FUNC_GPIO49_GPIO49_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO50               (REG_IO_MUX_BASE + 0xCC)
#define FUNC_GPIO50_DBG_FLASH_Q_PAD                                       4
#define FUNC_GPIO50_GMAC_RMII_CLK_PAD                                    3
#define FUNC_GPIO50_GPIO50                                               1
#define FUNC_GPIO50_GPIO50_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO51               (REG_IO_MUX_BASE + 0xD0)
#define FUNC_GPIO51_DBG_FLASH_WP_PAD                                     4
#define FUNC_GPIO51_GMAC_PHY_RXDV_PAD                                    3
#define FUNC_GPIO51_GPIO51                                               1
#define FUNC_GPIO51_GPIO51_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO52               (REG_IO_MUX_BASE + 0xD4)
#define FUNC_GPIO52_DBG_FLASH_HOLD_PAD                                   4
#define FUNC_GPIO52_GMAC_PHY_RXD0_PAD                                    3
#define FUNC_GPIO52_GPIO52                                               1
#define FUNC_GPIO52_GPIO52_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO53               (REG_IO_MUX_BASE + 0xD8)
#define FUNC_GPIO53_DBG_FLASH_CK_PAD                                      4
#define FUNC_GPIO53_GMAC_PHY_RXD1_PAD                                    3
#define FUNC_GPIO53_GPIO53                                               1
#define FUNC_GPIO53_GPIO53_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO54               (REG_IO_MUX_BASE + 0xDC)
#define FUNC_GPIO54_DBG_FLASH_D_PAD                                       4
#define FUNC_GPIO54_GMAC_PHY_RXER_PAD                                    3
#define FUNC_GPIO54_GPIO54                                               1
#define FUNC_GPIO54_GPIO54_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO55               (REG_IO_MUX_BASE + 0xE0)
#define FUNC_GPIO55_GPIO55                                               1
#define FUNC_GPIO55_GPIO55_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO56               (REG_IO_MUX_BASE + 0xE4)
#define FUNC_GPIO56_GPIO56                                               1
#define FUNC_GPIO56_GPIO56_0                                             0

#define IO_MUX_DATE_REG          (REG_IO_MUX_BASE + 0x104)
/* IO_MUX_DATE : R/W ;bitpos:[27:0] ;default: 27'h0201222 ; */
/*description: csv date.*/
#define IO_MUX_DATE    0x0FFFFFFF
#define IO_MUX_DATE_M  ((IO_MUX_DATE_V)<<(IO_MUX_DATE_S))
#define IO_MUX_DATE_V  0xFFFFFFF
#define IO_MUX_DATE_S  0
