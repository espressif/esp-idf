/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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

/* HYS_EN : R/W; bitpos: [16]; default: 0;
 * Software enables hysteresis function for the pad.
 * 1: Hysteresis enabled. 0: Hysteresis disabled.
 */
#define HYS_EN    (BIT(16))
#define HYS_EN_M  (HYS_EN_V << HYS_EN_S)
#define HYS_EN_V  0x00000001
#define HYS_EN_S  16
/* HYS_SEL : R/W; bitpos: [17]; default: 0;
 * Select enabling signals of the pad from software and efuse hardware.
 * 1: Select enabling siganl from software.
 * 0: Select enabling signal from efuse hardware.
 */
#define HYS_SEL    (BIT(17))
#define HYS_SEL_M  (HYS_SEL_V << HYS_SEL_S)
#define HYS_SEL_V  0x00000001
#define HYS_SEL_S  17

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
#define PIN_HYS_EN_SEL_EFUSE(PIN_NAME)      REG_CLR_BIT(PIN_NAME, HYS_SEL)
#define PIN_HYS_EN_SEL_SOFT(PIN_NAME)       REG_SET_BIT(PIN_NAME, HYS_SEL)
#define PIN_HYS_SOFT_ENABLE(PIN_NAME)       REG_SET_BIT(PIN_NAME, HYS_EN)
#define PIN_HYS_SOFT_DISABLE(PIN_NAME)      REG_CLR_BIT(PIN_NAME, HYS_EN)

#define IO_MUX_GPIO0_REG               PERIPHS_IO_MUX_GPIO0_U
#define IO_MUX_GPIO1_REG               PERIPHS_IO_MUX_GPIO1_U
#define IO_MUX_GPIO2_REG               PERIPHS_IO_MUX_MTMS_U
#define IO_MUX_GPIO3_REG               PERIPHS_IO_MUX_MTDO_U
#define IO_MUX_GPIO4_REG               PERIPHS_IO_MUX_MTCK_U
#define IO_MUX_GPIO5_REG               PERIPHS_IO_MUX_MTDI_U
#define IO_MUX_GPIO6_REG               PERIPHS_IO_MUX_GPIO6_U
#define IO_MUX_GPIO7_REG               PERIPHS_IO_MUX_GPIO7_U
#define IO_MUX_GPIO8_REG               PERIPHS_IO_MUX_GPIO8_U
#define IO_MUX_GPIO9_REG               PERIPHS_IO_MUX_GPIO9_U
#define IO_MUX_GPIO10_REG              PERIPHS_IO_MUX_GPIO10_U
#define IO_MUX_GPIO11_REG              PERIPHS_IO_MUX_GPIO11_U
#define IO_MUX_GPIO12_REG              PERIPHS_IO_MUX_GPIO12_U
#define IO_MUX_GPIO13_REG              PERIPHS_IO_MUX_XTAL_32K_P_U
#define IO_MUX_GPIO14_REG              PERIPHS_IO_MUX_XTAL_32K_N_U
#define IO_MUX_GPIO15_REG              PERIPHS_IO_MUX_SPICS0_U
#define IO_MUX_GPIO16_REG              PERIPHS_IO_MUX_SPIQ_U
#define IO_MUX_GPIO17_REG              PERIPHS_IO_MUX_SPIWP_U
#define IO_MUX_GPIO18_REG              PERIPHS_IO_MUX_SPIHD_U
#define IO_MUX_GPIO19_REG              PERIPHS_IO_MUX_SPICLK_U
#define IO_MUX_GPIO20_REG              PERIPHS_IO_MUX_SPID_U
#define IO_MUX_GPIO21_REG              PERIPHS_IO_MUX_VDD_SPI_U
#define IO_MUX_GPIO22_REG              PERIPHS_IO_MUX_GPIO22_U
#define IO_MUX_GPIO23_REG              PERIPHS_IO_MUX_U0RXD_U
#define IO_MUX_GPIO24_REG              PERIPHS_IO_MUX_U0TXD_U
#define IO_MUX_GPIO25_REG              PERIPHS_IO_MUX_GPIO25_U
#define IO_MUX_GPIO26_REG              PERIPHS_IO_MUX_GPIO26_U
#define IO_MUX_GPIO27_REG              PERIPHS_IO_MUX_GPIO27_U

#define PIN_FUNC_GPIO								1

#define GPIO_PAD_PULLUP(num) do{PIN_PULLDWN_DIS(IOMUX_REG_GPIO##num);PIN_PULLUP_EN(IOMUX_REG_GPIO##num);}while(0)
#define GPIO_PAD_PULLDOWN(num) do{PIN_PULLUP_DIS(IOMUX_REG_GPIO##num);PIN_PULLDWN_EN(IOMUX_REG_GPIO##num);}while(0)
#define GPIO_PAD_SET_DRV(num, drv) PIN_SET_DRV(IOMUX_REG_GPIO##num, drv)

#define SPI_HD_GPIO_NUM              18
#define SPI_WP_GPIO_NUM              17
#define SPI_CS0_GPIO_NUM             15
#define SPI_CLK_GPIO_NUM             19
#define SPI_D_GPIO_NUM               20
#define SPI_Q_GPIO_NUM               16

#define USB_DM_GPIO_NUM              26
#define USB_DP_GPIO_NUM              27

#define EXT_OSC_SLOW_GPIO_NUM        13

#define MAX_RTC_GPIO_NUM             14            // GPIO7~14 are the pads with LP function
#define MAX_PAD_GPIO_NUM             27
#define MAX_GPIO_NUM                 31
#define HIGH_IO_HOLD_BIT_SHIFT       32


#define REG_IO_MUX_BASE DR_REG_IO_MUX_BASE
#define PIN_CTRL                          (REG_IO_MUX_BASE +0x00)

#define CLK_OUT3                                    IO_MUX_CLK_OUT3
#define CLK_OUT3_V                                  IO_MUX_CLK_OUT3_V
#define CLK_OUT3_S                                  IO_MUX_CLK_OUT3_S
#define CLK_OUT3_M                                  IO_MUX_CLK_OUT3_M
#define CLK_OUT2                                    IO_MUX_CLK_OUT2
#define CLK_OUT2_V                                  IO_MUX_CLK_OUT2_V
#define CLK_OUT2_S                                  IO_MUX_CLK_OUT2_S
#define CLK_OUT2_M                                  IO_MUX_CLK_OUT2_M
#define CLK_OUT1                                    IO_MUX_CLK_OUT1
#define CLK_OUT1_V                                  IO_MUX_CLK_OUT1_V
#define CLK_OUT1_S                                  IO_MUX_CLK_OUT1_S
#define CLK_OUT1_M                                  IO_MUX_CLK_OUT1_M
// definitions above are inherited from previous version of code, should double check

// definitions below are generated from pin_txt.csv
#define PERIPHS_IO_MUX_GPIO0_U                (REG_IO_MUX_BASE + 0x4)
#define FUNC_GPIO0_FSPIQ                                            2
#define FUNC_GPIO0_GPIO0                                            1
#define FUNC_GPIO0_GPIO0_0                                          0

#define PERIPHS_IO_MUX_GPIO1_U                (REG_IO_MUX_BASE + 0x8)
#define FUNC_GPIO1_FSPICS0                                          2
#define FUNC_GPIO1_GPIO1                                            1
#define FUNC_GPIO1_GPIO1_0                                          0

#define PERIPHS_IO_MUX_MTMS_U                 (REG_IO_MUX_BASE + 0xC)
#define FUNC_MTMS_FSPIWP                                            2
#define FUNC_MTMS_GPIO2                                             1
#define FUNC_MTMS_MTMS                                              0

#define PERIPHS_IO_MUX_MTDO_U                (REG_IO_MUX_BASE + 0x10)
#define FUNC_MTDO_FSPIHD                                            2
#define FUNC_MTDO_GPIO3                                             1
#define FUNC_MTDO_MTDO                                              0

#define PERIPHS_IO_MUX_MTCK_U                (REG_IO_MUX_BASE + 0x14)
#define FUNC_MTCK_FSPICLK                                           2
#define FUNC_MTCK_GPIO4                                             1
#define FUNC_MTCK_MTCK                                              0

#define PERIPHS_IO_MUX_MTDI_U                (REG_IO_MUX_BASE + 0x18)
#define FUNC_MTDI_FSPID                                             2
#define FUNC_MTDI_GPIO5                                             1
#define FUNC_MTDI_MTDI                                              0

#define PERIPHS_IO_MUX_GPIO6_U               (REG_IO_MUX_BASE + 0x1C)
#define FUNC_GPIO6_GPIO6                                            1
#define FUNC_GPIO6_GPIO6_0                                          0

#define PERIPHS_IO_MUX_GPIO7_U               (REG_IO_MUX_BASE + 0x20)
#define FUNC_GPIO7_GPIO7                                            1
#define FUNC_GPIO7_GPIO7_0                                          0

#define PERIPHS_IO_MUX_GPIO8_U               (REG_IO_MUX_BASE + 0x24)
#define FUNC_GPIO8_GPIO8                                            1
#define FUNC_GPIO8_GPIO8_0                                          0

#define PERIPHS_IO_MUX_GPIO9_U               (REG_IO_MUX_BASE + 0x28)
#define FUNC_GPIO9_GPIO9                                            1
#define FUNC_GPIO9_GPIO9_0                                          0

#define PERIPHS_IO_MUX_GPIO10_U              (REG_IO_MUX_BASE + 0x2C)
#define FUNC_GPIO10_GPIO10                                          1
#define FUNC_GPIO10_GPIO10_0                                        0

#define PERIPHS_IO_MUX_GPIO11_U              (REG_IO_MUX_BASE + 0x30)
#define FUNC_GPIO11_GPIO11                                          1
#define FUNC_GPIO11_GPIO11_0                                        0

#define PERIPHS_IO_MUX_GPIO12_U              (REG_IO_MUX_BASE + 0x34)
#define FUNC_GPIO12_GPIO12                                          1
#define FUNC_GPIO12_GPIO12_0                                        0

#define PERIPHS_IO_MUX_XTAL_32K_P_U          (REG_IO_MUX_BASE + 0x38)
#define FUNC_XTAL_32K_P_GPIO13                                      1
#define FUNC_XTAL_32K_P_GPIO13_0                                    0

#define PERIPHS_IO_MUX_XTAL_32K_N_U          (REG_IO_MUX_BASE + 0x3C)
#define FUNC_XTAL_32K_N_GPIO14                                      1
#define FUNC_XTAL_32K_N_GPIO14_0                                    0

#define PERIPHS_IO_MUX_SPICS0_U              (REG_IO_MUX_BASE + 0x40)
#define FUNC_SPICS0_GPIO15                                          1
#define FUNC_SPICS0_SPICS0                                          0

#define PERIPHS_IO_MUX_SPIQ_U                (REG_IO_MUX_BASE + 0x44)
#define FUNC_SPIQ_GPIO16                                            1
#define FUNC_SPIQ_SPIQ                                              0

#define PERIPHS_IO_MUX_SPIWP_U               (REG_IO_MUX_BASE + 0x48)
#define FUNC_SPIWP_GPIO17                                           1
#define FUNC_SPIWP_SPIWP                                            0

#define PERIPHS_IO_MUX_SPIHD_U               (REG_IO_MUX_BASE + 0x4C)
#define FUNC_SPIHD_GPIO18                                           1
#define FUNC_SPIHD_SPIHD                                            0

#define PERIPHS_IO_MUX_SPICLK_U              (REG_IO_MUX_BASE + 0x50)
#define FUNC_SPICLK_GPIO19                                          1
#define FUNC_SPICLK_SPICLK                                          0

#define PERIPHS_IO_MUX_SPID_U                (REG_IO_MUX_BASE + 0x54)
#define FUNC_SPID_GPIO20                                            1
#define FUNC_SPID_SPID                                              0

#define PERIPHS_IO_MUX_VDD_SPI_U             (REG_IO_MUX_BASE + 0x58)
#define FUNC_VDD_SPI_GPIO21                                         1
#define FUNC_VDD_SPI_GPIO21_0                                       0

#define PERIPHS_IO_MUX_GPIO22_U              (REG_IO_MUX_BASE + 0x5C)
#define FUNC_GPIO22_GPIO22                                          1
#define FUNC_GPIO22_GPIO22_0                                        0

#define PERIPHS_IO_MUX_U0RXD_U               (REG_IO_MUX_BASE + 0x60)
#define FUNC_U0RXD_FSPICS1                                          2
#define FUNC_U0RXD_GPIO23                                           1
#define FUNC_U0RXD_U0RXD                                            0

#define PERIPHS_IO_MUX_U0TXD_U               (REG_IO_MUX_BASE + 0x64)
#define FUNC_U0TXD_FSPICS2                                          2
#define FUNC_U0TXD_GPIO24                                           1
#define FUNC_U0TXD_U0TXD                                            0

#define PERIPHS_IO_MUX_GPIO25_U              (REG_IO_MUX_BASE + 0x68)
#define FUNC_GPIO25_FSPICS3                                         2
#define FUNC_GPIO25_GPIO25                                          1
#define FUNC_GPIO25_GPIO25_0                                        0

#define PERIPHS_IO_MUX_GPIO26_U              (REG_IO_MUX_BASE + 0x6C)
#define FUNC_GPIO26_FSPICS4                                         2
#define FUNC_GPIO26_GPIO26                                          1
#define FUNC_GPIO26_GPIO26_0                                        0

#define PERIPHS_IO_MUX_GPIO27_U              (REG_IO_MUX_BASE + 0x70)
#define FUNC_GPIO27_FSPICS5                                         2
#define FUNC_GPIO27_GPIO27                                          1
#define FUNC_GPIO27_GPIO27_0                                        0

/** IO_MUX_PIN_CTRL_REG register
 *  Clock Output Configuration
 *  Register
 */
#define IO_MUX_PIN_CTRL_REG (REG_IO_MUX_BASE + 0x0)
/* IO_MUX_CLK_OUT1 : R/W; bitpos: [5:0]; default: 15;
 * If you want to output clock for I2S to CLK_OUT_out1, set this register
 * to 0x0. CLK_OUT_out1 can be found in peripheral output
 * signals.
 */
#define IO_MUX_CLK_OUT1    0x0000001F
#define IO_MUX_CLK_OUT1_M  (IO_MUX_CLK_OUT1_V << IO_MUX_CLK_OUT1_S)
#define IO_MUX_CLK_OUT1_V  0x0000001F
#define IO_MUX_CLK_OUT1_S  0
/* IO_MUX_CLK_OUT2 : R/W; bitpos: [10:5]; default: 15;
 * If you want to output clock for I2S to CLK_OUT_out2, set this register
 * to 0x0. CLK_OUT_out2 can be found in peripheral output
 * signals.
 */
#define IO_MUX_CLK_OUT2    0x0000001F
#define IO_MUX_CLK_OUT2_M  (IO_MUX_CLK_OUT2_V << IO_MUX_CLK_OUT2_S)
#define IO_MUX_CLK_OUT2_V  0x0000001F
#define IO_MUX_CLK_OUT2_S  5
/* IO_MUX_CLK_OUT3 : R/W; bitpos: [15:10]; default: 7;
 * If you want to output clock for I2S to CLK_OUT_out3, set this register
 * to 0x0. CLK_OUT_out3 can be found in peripheral output
 * signals.
 */
#define IO_MUX_CLK_OUT3    0x0000001F
#define IO_MUX_CLK_OUT3_M  (IO_MUX_CLK_OUT3_V << IO_MUX_CLK_OUT3_S)
#define IO_MUX_CLK_OUT3_V  0x0000001F
#define IO_MUX_CLK_OUT3_S  10

/** IO_MUX_MODEM_DIAG_EN_REG register
 *  GPIO MATRIX Configure Register for modem
 *  diag
 */
#define IO_MUX_MODEM_DIAG_EN_REG (REG_IO_MUX_BASE + 0xbc)
/* IO_MUX_MODEM_DIAG_EN : R/W; bitpos: [32:0]; default: 0;
 * bit i to enable modem_diag[i] into gpio matrix. 1:enable modem_diag[i]
 * into gpio matrix. 0:enable other signals into gpio
 * matrix
 */
#define IO_MUX_MODEM_DIAG_EN    0xFFFFFFFF
#define IO_MUX_MODEM_DIAG_EN_M  (IO_MUX_MODEM_DIAG_EN_V << IO_MUX_MODEM_DIAG_EN_S)
#define IO_MUX_MODEM_DIAG_EN_V  0xFFFFFFFF
#define IO_MUX_MODEM_DIAG_EN_S  0

/** IO_MUX_DATE_REG register
 *  IO MUX Version Control
 *  Register
 */
#define IO_MUX_DATE_REG (REG_IO_MUX_BASE + 0xfc)
/* IO_MUX_REG_DATE : R/W; bitpos: [28:0]; default: 35680880;
 * Version control
 * register
 */
#define IO_MUX_REG_DATE    0x0FFFFFFF
#define IO_MUX_REG_DATE_M  (IO_MUX_REG_DATE_V << IO_MUX_REG_DATE_S)
#define IO_MUX_REG_DATE_V  0x0FFFFFFF
#define IO_MUX_REG_DATE_S  0
