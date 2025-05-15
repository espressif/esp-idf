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

#define IO_MUX_GPIO0_REG                    PERIPHS_IO_MUX_XTAL_32K_P_U
#define IO_MUX_GPIO1_REG                    PERIPHS_IO_MUX_XTAL_32K_N_U
#define IO_MUX_GPIO2_REG                    PERIPHS_IO_MUX_GPIO2_U
#define IO_MUX_GPIO3_REG                    PERIPHS_IO_MUX_MTMS_U
#define IO_MUX_GPIO4_REG                    PERIPHS_IO_MUX_MTDI_U
#define IO_MUX_GPIO5_REG                    PERIPHS_IO_MUX_MTCK_U
#define IO_MUX_GPIO6_REG                    PERIPHS_IO_MUX_MTDO_U
#define IO_MUX_GPIO7_REG                    PERIPHS_IO_MUX_GPIO7_U
#define IO_MUX_GPIO8_REG                    PERIPHS_IO_MUX_GPIO8_U
#define IO_MUX_GPIO9_REG                    PERIPHS_IO_MUX_GPIO9_U
#define IO_MUX_GPIO10_REG                   PERIPHS_IO_MUX_U0RXD_U
#define IO_MUX_GPIO11_REG                   PERIPHS_IO_MUX_U0TXD_U
#define IO_MUX_GPIO12_REG                   PERIPHS_IO_MUX_GPIO12_U
#define IO_MUX_GPIO13_REG                   PERIPHS_IO_MUX_GPIO13_U
#define IO_MUX_GPIO14_REG                   PERIPHS_IO_MUX_SPICS1_U
#define IO_MUX_GPIO15_REG                   PERIPHS_IO_MUX_SPICS0_U
#define IO_MUX_GPIO16_REG                   PERIPHS_IO_MUX_SPIQ_U
#define IO_MUX_GPIO17_REG                   PERIPHS_IO_MUX_SPIWP_U
#define IO_MUX_GPIO18_REG                   PERIPHS_IO_MUX_VDD_SPI_U
#define IO_MUX_GPIO19_REG                   PERIPHS_IO_MUX_SPIHD_U
#define IO_MUX_GPIO20_REG                   PERIPHS_IO_MUX_SPICLK_U
#define IO_MUX_GPIO21_REG                   PERIPHS_IO_MUX_SPID_U
#define IO_MUX_GPIO22_REG                   PERIPHS_IO_MUX_SDIO_DATA2_U
#define IO_MUX_GPIO23_REG                   PERIPHS_IO_MUX_SDIO_DATA3_U
#define IO_MUX_GPIO24_REG                   PERIPHS_IO_MUX_GPIO24_U
#define IO_MUX_GPIO25_REG                   PERIPHS_IO_MUX_SDIO_CMD_U
#define IO_MUX_GPIO26_REG                   PERIPHS_IO_MUX_SDIO_CLK_U
#define IO_MUX_GPIO27_REG                   PERIPHS_IO_MUX_SDIO_DATA0_U
#define IO_MUX_GPIO28_REG                   PERIPHS_IO_MUX_SDIO_DATA1_U
#define IO_MUX_GPIO29_REG                   PERIPHS_IO_MUX_GPIO29_U

#define PIN_FUNC_GPIO                1

#define USB_INT_PHY0_DM_GPIO_NUM     12
#define USB_INT_PHY0_DP_GPIO_NUM     13

#define EXT_OSC_SLOW_GPIO_NUM        0

#define MAX_RTC_GPIO_NUM              6
#define MAX_PAD_GPIO_NUM             24
#define MAX_GPIO_NUM                 28
#define HIGH_IO_HOLD_BIT_SHIFT       32

#define REG_IO_MUX_BASE                             DR_REG_IO_MUX_BASE
// definitions above are inherited from previous version of code, should double check

// definitions below are generated from pin_txt.csv
#define PERIPHS_IO_MUX_XTAL_32K_P_U                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_XTAL_32K_P_GPIO0_0                                            0
#define FUNC_XTAL_32K_P_GPIO0                                              1

#define PERIPHS_IO_MUX_XTAL_32K_N_U                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_XTAL_32K_N_GPIO1_0                                            0
#define FUNC_XTAL_32K_N_GPIO1                                              1

#define PERIPHS_IO_MUX_GPIO2_U                       (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO2_GPIO2_0                                                 0
#define FUNC_GPIO2_GPIO2                                                   1
#define FUNC_GPIO2_FSPIQ                                                   2

// Strapping: Boot Mode select 0/SDIO input strapping
#define PERIPHS_IO_MUX_MTMS_U                        (REG_IO_MUX_BASE + 0x0)
#define FUNC_MTMS_MTMS                                                     0
#define FUNC_MTMS_GPIO3                                                    1
#define FUNC_MTMS_FSPIHD                                                   2

// Strapping: Boot Mode select 1/SDIO output strapping
#define PERIPHS_IO_MUX_MTDI_U                        (REG_IO_MUX_BASE + 0x0)
#define FUNC_MTDI_MTDI                                                     0
#define FUNC_MTDI_GPIO4                                                    1
#define FUNC_MTDI_FSPIWP                                                   2

#define PERIPHS_IO_MUX_MTCK_U                        (REG_IO_MUX_BASE + 0x0)
#define FUNC_MTCK_MTCK                                                     0
#define FUNC_MTCK_GPIO5                                                    1

#define PERIPHS_IO_MUX_MTDO_U                        (REG_IO_MUX_BASE + 0x0)
#define FUNC_MTDO_MTDO                                                     0
#define FUNC_MTDO_GPIO6                                                    1
#define FUNC_MTDO_FSPICLK                                                  2

// Strapping: JTAG select
#define PERIPHS_IO_MUX_GPIO7_U                       (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO7_GPIO7_0                                                 0
#define FUNC_GPIO7_GPIO7                                                   1
#define FUNC_GPIO7_FSPID                                                   2

// Strapping: Boot Mode select 2
#define PERIPHS_IO_MUX_GPIO8_U                       (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO8_GPIO8_0                                                 0
#define FUNC_GPIO8_GPIO8                                                   1
#define FUNC_GPIO8_FSPICS0                                                 2

// Strapping: Boot Mode select 3
#define PERIPHS_IO_MUX_GPIO9_U                       (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO9_GPIO9_0                                                 0
#define FUNC_GPIO9_GPIO9                                                   1

#define PERIPHS_IO_MUX_U0RXD_U                       (REG_IO_MUX_BASE + 0x0)
#define FUNC_U0RXD_U0RXD                                                   0
#define FUNC_U0RXD_GPIO10                                                  1

#define PERIPHS_IO_MUX_U0TXD_U                       (REG_IO_MUX_BASE + 0x0)
#define FUNC_U0TXD_U0TXD                                                   0
#define FUNC_U0TXD_GPIO11                                                  1

#define PERIPHS_IO_MUX_GPIO12_U                      (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO12_GPIO12_0                                               0
#define FUNC_GPIO12_GPIO12                                                 1

#define PERIPHS_IO_MUX_GPIO13_U                      (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO13_GPIO13_0                                               0
#define FUNC_GPIO13_GPIO13                                                 1

#define PERIPHS_IO_MUX_SPICS1_U                      (REG_IO_MUX_BASE + 0x0)
#define FUNC_SPICS1_SPICS1                                                 0
#define FUNC_SPICS1_GPIO14                                                 1

#define PERIPHS_IO_MUX_SPICS0_U                      (REG_IO_MUX_BASE + 0x0)
#define FUNC_SPICS0_SPICS0                                                 0
#define FUNC_SPICS0_GPIO15                                                 1

#define PERIPHS_IO_MUX_SPIQ_U                        (REG_IO_MUX_BASE + 0x0)
#define FUNC_SPIQ_SPIQ                                                     0
#define FUNC_SPIQ_GPIO16                                                   1

#define PERIPHS_IO_MUX_SPIWP_U                       (REG_IO_MUX_BASE + 0x0)
#define FUNC_SPIWP_SPIWP                                                   0
#define FUNC_SPIWP_GPIO17                                                  1

#define PERIPHS_IO_MUX_VDD_SPI_U                     (REG_IO_MUX_BASE + 0x0)
#define FUNC_VDD_SPI_GPIO18_0                                              0
#define FUNC_VDD_SPI_GPIO18                                                1

#define PERIPHS_IO_MUX_SPIHD_U                       (REG_IO_MUX_BASE + 0x0)
#define FUNC_SPIHD_SPIHD                                                   0
#define FUNC_SPIHD_GPIO19                                                  1

#define PERIPHS_IO_MUX_SPICLK_U                      (REG_IO_MUX_BASE + 0x0)
#define FUNC_SPICLK_SPICLK                                                 0
#define FUNC_SPICLK_GPIO20                                                 1

#define PERIPHS_IO_MUX_SPID_U                        (REG_IO_MUX_BASE + 0x0)
#define FUNC_SPID_SPID                                                     0
#define FUNC_SPID_GPIO21                                                   1

#define PERIPHS_IO_MUX_SDIO_DATA2_U                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_SDIO_DATA2_SDIO_DATA2                                         0
#define FUNC_SDIO_DATA2_GPIO22                                             1

#define PERIPHS_IO_MUX_SDIO_DATA3_U                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_SDIO_DATA3_SDIO_DATA3                                         0
#define FUNC_SDIO_DATA3_GPIO23                                             1

#define PERIPHS_IO_MUX_GPIO24_U                      (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO24_GPIO24_0                                               0
#define FUNC_GPIO24_GPIO24                                                 1

#define PERIPHS_IO_MUX_SDIO_CMD_U                    (REG_IO_MUX_BASE + 0x0)
#define FUNC_SDIO_CMD_SDIO_CMD                                             0
#define FUNC_SDIO_CMD_GPIO25                                               1

#define PERIPHS_IO_MUX_SDIO_CLK_U                    (REG_IO_MUX_BASE + 0x0)
#define FUNC_SDIO_CLK_SDIO_CLK                                             0
#define FUNC_SDIO_CLK_GPIO26                                               1

#define PERIPHS_IO_MUX_SDIO_DATA0_U                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_SDIO_DATA0_SDIO_DATA0                                         0
#define FUNC_SDIO_DATA0_GPIO27                                             1

#define PERIPHS_IO_MUX_SDIO_DATA1_U                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_SDIO_DATA1_SDIO_DATA1                                         0
#define FUNC_SDIO_DATA1_GPIO28                                             1

#define PERIPHS_IO_MUX_GPIO29_U                      (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO29_GPIO29_0                                               0
#define FUNC_GPIO29_GPIO29                                                 1

/** IO_MUX_DATE_REG register
 *  Version control register
 */
#define IO_MUX_DATE_REG (DR_REG_IO_MUX_BASE + 0x1fc)
/** IO_MUX_REG_DATE : R/W; bitpos: [27:0]; default: 36774288;
 *  Version control register
 */
#define IO_MUX_REG_DATE    0x0FFFFFFFU
#define IO_MUX_REG_DATE_M  (IO_MUX_REG_DATE_V << IO_MUX_REG_DATE_S)
#define IO_MUX_REG_DATE_V  0x0FFFFFFFU
#define IO_MUX_REG_DATE_S  0

#ifdef __cplusplus
}
#endif
