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
/* Configures whether or not to enable the hysteresis function of the pin when IO_MUX_GPIO$n_HYS_SEL is set to 1. */
#define HYS_EN (BIT(16))
#define HYS_EN_M (HYS_EN_V << HYS_EN_S)
#define HYS_EN_V 1
#define HYS_EN_S 16
/* Configures to choose the signal for enabling the hysteresis function for GPIO$n. */
#define HYS_SEL (BIT(17))
#define HYS_SEL_M (HYS_SEL_V << HYS_SEL_S)
#define HYS_SEL_V 1
#define HYS_SEL_S 17

#define IO_MUX_GPIO0_REG                    PERIPHS_IO_MUX_U_PAD_GPIO0
#define IO_MUX_GPIO1_REG                    PERIPHS_IO_MUX_U_PAD_GPIO1
#define IO_MUX_GPIO2_REG                    PERIPHS_IO_MUX_U_PAD_GPIO2
#define IO_MUX_GPIO3_REG                    PERIPHS_IO_MUX_U_PAD_GPIO3
#define IO_MUX_GPIO4_REG                    PERIPHS_IO_MUX_U_PAD_XTAL_32K_P
#define IO_MUX_GPIO5_REG                    PERIPHS_IO_MUX_U_PAD_XTAL_32K_N
#define IO_MUX_GPIO6_REG                    PERIPHS_IO_MUX_U_PAD_SPICS1
#define IO_MUX_GPIO7_REG                    PERIPHS_IO_MUX_U_PAD_SPICS0
#define IO_MUX_GPIO8_REG                    PERIPHS_IO_MUX_U_PAD_SPIQ
#define IO_MUX_GPIO9_REG                    PERIPHS_IO_MUX_U_PAD_SPIWP
#define IO_MUX_GPIO10_REG                   PERIPHS_IO_MUX_U_PAD_SPIHD
#define IO_MUX_GPIO11_REG                   PERIPHS_IO_MUX_U_PAD_SPICLK
#define IO_MUX_GPIO12_REG                   PERIPHS_IO_MUX_U_PAD_SPID
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
#define IO_MUX_GPIO23_REG                   PERIPHS_IO_MUX_U_PAD_U0RXD
#define IO_MUX_GPIO24_REG                   PERIPHS_IO_MUX_U_PAD_U0TXD
#define IO_MUX_GPIO25_REG                   PERIPHS_IO_MUX_U_PAD_GPIO25
#define IO_MUX_GPIO26_REG                   PERIPHS_IO_MUX_U_PAD_GPIO26
#define IO_MUX_GPIO27_REG                   PERIPHS_IO_MUX_U_PAD_GPIO27
#define IO_MUX_GPIO28_REG                   PERIPHS_IO_MUX_U_PAD_MTMS
#define IO_MUX_GPIO29_REG                   PERIPHS_IO_MUX_U_PAD_MTDI
#define IO_MUX_GPIO30_REG                   PERIPHS_IO_MUX_U_PAD_MTCK
#define IO_MUX_GPIO31_REG                   PERIPHS_IO_MUX_U_PAD_MTDO
#define IO_MUX_GPIO32_REG                   PERIPHS_IO_MUX_U_PAD_GPIO32
#define IO_MUX_GPIO33_REG                   PERIPHS_IO_MUX_U_PAD_GPIO33
#define IO_MUX_GPIO34_REG                   PERIPHS_IO_MUX_U_PAD_GPIO34
#define IO_MUX_GPIO35_REG                   PERIPHS_IO_MUX_U_PAD_GPIO35
#define IO_MUX_GPIO36_REG                   PERIPHS_IO_MUX_U_PAD_GPIO36
#define IO_MUX_GPIO37_REG                   PERIPHS_IO_MUX_U_PAD_GPIO37
#define IO_MUX_GPIO38_REG                   PERIPHS_IO_MUX_U_PAD_GPIO38
#define IO_MUX_GPIO39_REG                   PERIPHS_IO_MUX_U_PAD_GPIO39

#define PIN_FUNC_GPIO                1

#define USB_INT_PHY0_DM_GPIO_NUM     13
#define USB_INT_PHY0_DP_GPIO_NUM     14

#define EXT_OSC_SLOW_GPIO_NUM         0
#define MAX_RTC_GPIO_NUM              5
#define MAX_PAD_GPIO_NUM             39
#define MAX_GPIO_NUM                 39
#define HIGH_IO_HOLD_BIT_SHIFT       32

#define REG_IO_MUX_BASE                             DR_REG_IO_MUX_BASE
// definitions above are inherited from previous version of code, should double check

// definitions below are generated from pin_txt.csv
#define PERIPHS_IO_MUX_U_PAD_GPIO0                 (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO0_GPIO0                                                 1
#define FUNC_GPIO0_GPIO0_0                                               0

#define PERIPHS_IO_MUX_U_PAD_GPIO1                 (REG_IO_MUX_BASE + 0x4)
#define FUNC_GPIO1_GPIO1                                                 1
#define FUNC_GPIO1_GPIO1_0                                               0

#define PERIPHS_IO_MUX_U_PAD_GPIO2                 (REG_IO_MUX_BASE + 0x8)
#define FUNC_GPIO2_GPIO2                                                 1
#define FUNC_GPIO2_GPIO2_0                                               0

#define PERIPHS_IO_MUX_U_PAD_GPIO3                 (REG_IO_MUX_BASE + 0xC)
#define FUNC_GPIO3_GPIO3                                                 1
#define FUNC_GPIO3_GPIO3_0                                               0

#define PERIPHS_IO_MUX_U_PAD_XTAL_32K_N           (REG_IO_MUX_BASE + 0x10)
#define FUNC_XTAL_32K_N_GPIO4                                            1
#define FUNC_XTAL_32K_N_GPIO4_0                                          0

#define PERIPHS_IO_MUX_U_PAD_XTAL_32K_P           (REG_IO_MUX_BASE + 0x14)
#define FUNC_XTAL_32K_P_GPIO5                                            1
#define FUNC_XTAL_32K_P_GPIO5_0                                          0

#define PERIPHS_IO_MUX_U_PAD_SPICS1               (REG_IO_MUX_BASE + 0x18)
#define FUNC_SPICS1_GPIO6                                                1
#define FUNC_SPICS1_SPICS1                                               0

#define PERIPHS_IO_MUX_U_PAD_SPICS0               (REG_IO_MUX_BASE + 0x1C)
#define FUNC_SPICS0_GPIO7                                                1
#define FUNC_SPICS0_SPICS0                                               0

#define PERIPHS_IO_MUX_U_PAD_SPIQ                 (REG_IO_MUX_BASE + 0x20)
#define FUNC_SPIQ_GPIO8                                                  1
#define FUNC_SPIQ_SPIQ                                                   0

#define PERIPHS_IO_MUX_U_PAD_SPIWP                (REG_IO_MUX_BASE + 0x24)
#define FUNC_SPIWP_GPIO9                                                 1
#define FUNC_SPIWP_SPIWP                                                 0

#define PERIPHS_IO_MUX_U_PAD_SPIHD                (REG_IO_MUX_BASE + 0x28)
#define FUNC_SPIHD_GPIO10                                                1
#define FUNC_SPIHD_SPIHD                                                 0

#define PERIPHS_IO_MUX_U_PAD_SPICLK               (REG_IO_MUX_BASE + 0x2C)
#define FUNC_SPICLK_GPIO11                                               1
#define FUNC_SPICLK_SPICLK                                               0

#define PERIPHS_IO_MUX_U_PAD_SPID                 (REG_IO_MUX_BASE + 0x30)
#define FUNC_SPID_GPIO12                                                 1
#define FUNC_SPID_SPID                                                   0

#define PERIPHS_IO_MUX_U_PAD_GPIO13               (REG_IO_MUX_BASE + 0x34)
#define FUNC_GPIO13_GPIO13                                               1
#define FUNC_GPIO13_GPIO13_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO14               (REG_IO_MUX_BASE + 0x38)
#define FUNC_GPIO14_GPIO14                                               1
#define FUNC_GPIO14_GPIO14_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO15               (REG_IO_MUX_BASE + 0x3C)
#define FUNC_GPIO15_FSPIQ                                                2
#define FUNC_GPIO15_GPIO15                                               1
#define FUNC_GPIO15_GPIO15_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO16               (REG_IO_MUX_BASE + 0x40)
#define FUNC_GPIO16_FSPICLK                                              2
#define FUNC_GPIO16_GPIO16                                               1
#define FUNC_GPIO16_GPIO16_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO17               (REG_IO_MUX_BASE + 0x44)
#define FUNC_GPIO17_FSPID                                                2
#define FUNC_GPIO17_GPIO17                                               1
#define FUNC_GPIO17_GPIO17_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO18               (REG_IO_MUX_BASE + 0x48)
#define FUNC_GPIO18_FSPIWP                                               2
#define FUNC_GPIO18_GPIO18                                               1
#define FUNC_GPIO18_GPIO18_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO19               (REG_IO_MUX_BASE + 0x4C)
#define FUNC_GPIO19_FSPIHD                                               2
#define FUNC_GPIO19_GPIO19                                               1
#define FUNC_GPIO19_GPIO19_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO20               (REG_IO_MUX_BASE + 0x50)
#define FUNC_GPIO20_FSPICS0                                              2
#define FUNC_GPIO20_GPIO20                                               1
#define FUNC_GPIO20_GPIO20_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO21               (REG_IO_MUX_BASE + 0x54)
#define FUNC_GPIO21_GPIO21                                               1
#define FUNC_GPIO21_GPIO21_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO22               (REG_IO_MUX_BASE + 0x58)
#define FUNC_GPIO22_GPIO22                                               1
#define FUNC_GPIO22_GPIO22_0                                             0

#define PERIPHS_IO_MUX_U_PAD_U0RXD                (REG_IO_MUX_BASE + 0x5C)
#define FUNC_U0RXD_GPIO23                                                1
#define FUNC_U0RXD_U0RXD                                                 0

#define PERIPHS_IO_MUX_U_PAD_U0TXD                (REG_IO_MUX_BASE + 0x60)
#define FUNC_U0TXD_GPIO24                                                1
#define FUNC_U0TXD_U0TXD                                                 0

#define PERIPHS_IO_MUX_U_PAD_GPIO25               (REG_IO_MUX_BASE + 0x64)
#define FUNC_GPIO25_GPIO25                                               1
#define FUNC_GPIO25_GPIO25_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO26               (REG_IO_MUX_BASE + 0x68)
#define FUNC_GPIO26_GPIO26                                               1
#define FUNC_GPIO26_GPIO26_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO27               (REG_IO_MUX_BASE + 0x6C)
#define FUNC_GPIO27_GPIO27                                               1
#define FUNC_GPIO27_GPIO27_0                                             0

#define PERIPHS_IO_MUX_U_PAD_MTMS                 (REG_IO_MUX_BASE + 0x70)
#define FUNC_MTMS_GPIO28                                                 1
#define FUNC_MTMS_MTMS                                                   0

#define PERIPHS_IO_MUX_U_PAD_MTDI                 (REG_IO_MUX_BASE + 0x74)
#define FUNC_MTDI_GPIO29                                                 1
#define FUNC_MTDI_MTDI                                                   0

#define PERIPHS_IO_MUX_U_PAD_MTCK                 (REG_IO_MUX_BASE + 0x78)
#define FUNC_MTCK_GPIO30                                                 1
#define FUNC_MTCK_MTCK                                                   0

#define PERIPHS_IO_MUX_U_PAD_MTDO                 (REG_IO_MUX_BASE + 0x7C)
#define FUNC_MTDO_GPIO31                                                 1
#define FUNC_MTDO_MTDO                                                   0

#define PERIPHS_IO_MUX_U_PAD_GPIO32               (REG_IO_MUX_BASE + 0x80)
#define FUNC_GPIO32_FSPICLK                                              2
#define FUNC_GPIO32_GPIO32                                               1
#define FUNC_GPIO32_GPIO32_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO33               (REG_IO_MUX_BASE + 0x84)
#define FUNC_GPIO33_FSPID                                                2
#define FUNC_GPIO33_GPIO33                                               1
#define FUNC_GPIO33_GPIO33_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO34               (REG_IO_MUX_BASE + 0x88)
#define FUNC_GPIO34_GPIO34                                               1
#define FUNC_GPIO34_GPIO34_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO35               (REG_IO_MUX_BASE + 0x8C)
#define FUNC_GPIO35_GPIO35                                               1
#define FUNC_GPIO35_GPIO35_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO36               (REG_IO_MUX_BASE + 0x90)
#define FUNC_GPIO36_FSPIQ                                                2
#define FUNC_GPIO36_GPIO36                                               1
#define FUNC_GPIO36_GPIO36_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO37               (REG_IO_MUX_BASE + 0x94)
#define FUNC_GPIO37_FSPIWP                                               2
#define FUNC_GPIO37_GPIO37                                               1
#define FUNC_GPIO37_GPIO37_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO38               (REG_IO_MUX_BASE + 0x98)
#define FUNC_GPIO38_FSPIHD                                               2
#define FUNC_GPIO38_GPIO38                                               1
#define FUNC_GPIO38_GPIO38_0                                             0

#define PERIPHS_IO_MUX_U_PAD_GPIO39               (REG_IO_MUX_BASE + 0x9C)
#define FUNC_GPIO39_FSPICS0                                              2
#define FUNC_GPIO39_GPIO39                                               1
#define FUNC_GPIO39_GPIO39_0                                             0

#define IO_MUX_DATE_REG          (REG_IO_MUX_BASE + 0x1FC)
/* IO_MUX_REG_DATE : R/W ;bitpos:[27:0] ;default: 28'h2412160 ; */
/*description: Version control register.*/
#define IO_MUX_REG_DATE    0x0FFFFFFF
#define IO_MUX_REG_DATE_M  ((IO_MUX_REG_DATE_V)<<(IO_MUX_REG_DATE_S))
#define IO_MUX_REG_DATE_V  0xFFFFFFF
#define IO_MUX_REG_DATE_S  0

#ifdef __cplusplus
}
#endif
