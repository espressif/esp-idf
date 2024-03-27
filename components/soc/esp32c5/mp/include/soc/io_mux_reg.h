/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
/* Used to enable sleep mode pin functions */
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

#define PIN_FUNC_GPIO								1

#define GPIO_PAD_PULLUP(num) do{PIN_PULLDWN_DIS(IOMUX_REG_GPIO##num);PIN_PULLUP_EN(IOMUX_REG_GPIO##num);}while(0)
#define GPIO_PAD_PULLDOWN(num) do{PIN_PULLUP_DIS(IOMUX_REG_GPIO##num);PIN_PULLDWN_EN(IOMUX_REG_GPIO##num);}while(0)
#define GPIO_PAD_SET_DRV(num, drv) PIN_SET_DRV(IOMUX_REG_GPIO##num, drv)

// TODO: [ESP32C5] 	IDF-8698 need check
#define SPI_HD_GPIO_NUM              22
#define SPI_WP_GPIO_NUM              20
#define SPI_CS0_GPIO_NUM             18
#define SPI_CLK_GPIO_NUM             23
#define SPI_D_GPIO_NUM               24
#define SPI_Q_GPIO_NUM               19

#define USB_INT_PHY0_DM_GPIO_NUM              25
#define USB_INT_PHY0_DP_GPIO_NUM              26

#define EXT_OSC_SLOW_GPIO_NUM        0

#define MAX_RTC_GPIO_NUM              8
#define MAX_PAD_GPIO_NUM             26
#define MAX_GPIO_NUM                 30
#define DIG_IO_HOLD_BIT_SHIFT        32

#define REG_IO_MUX_BASE DR_REG_IO_MUX_BASE
#define PIN_CTRL                          (REG_IO_MUX_BASE +0x00)

#define CLK_OUT3                                    0x1f
#define CLK_OUT3_V                                  CLK_OUT3
#define CLK_OUT3_S                                  10
#define CLK_OUT3_M                                  (CLK_OUT3_V << CLK_OUT3_S)
#define CLK_OUT2                                    0x1f
#define CLK_OUT2_V                                  CLK_OUT2
#define CLK_OUT2_S                                  5
#define CLK_OUT2_M                                  (CLK_OUT2_V << CLK_OUT2_S)
#define CLK_OUT1                                    0x1f
#define CLK_OUT1_V                                  CLK_OUT1
#define CLK_OUT1_S                                  0
#define CLK_OUT1_M                                  (CLK_OUT1_V << CLK_OUT1_S)
// definitions above are inherited from previous version of code, should double check

// definitions below are generated from pin_txt.csv
#define PERIPHS_IO_MUX_XTAL_32K_P_U                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_XTAL_32K_P_GPIO0_0                                            0
#define FUNC_XTAL_32K_P_GPIO0                                              1

#define PERIPHS_IO_MUX_XTAL_32K_N_U                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_XTAL_32K_N_GPIO1_0                                            0
#define FUNC_XTAL_32K_N_GPIO1                                              1

// Strapping: Boot Mode select
#define PERIPHS_IO_MUX_MTMS_U                        (REG_IO_MUX_BASE + 0x0)
#define FUNC_MTMS_MTMS                                                     0
#define FUNC_MTMS_GPIO2                                                    1
#define FUNC_MTMS_FSPIQ                                                    2

// Strapping: Boot Mode select
#define PERIPHS_IO_MUX_MTDI_U                        (REG_IO_MUX_BASE + 0x0)
#define FUNC_MTDI_MTDI                                                     0
#define FUNC_MTDI_GPIO3                                                    1

#define PERIPHS_IO_MUX_MTCK_U                        (REG_IO_MUX_BASE + 0x0)
#define FUNC_MTCK_MTCK                                                     0
#define FUNC_MTCK_GPIO4                                                    1
#define FUNC_MTCK_FSPIHD                                                   2

#define PERIPHS_IO_MUX_MTDO_U                        (REG_IO_MUX_BASE + 0x0)
#define FUNC_MTDO_MTDO                                                     0
#define FUNC_MTDO_GPIO5                                                    1
#define FUNC_MTDO_FSPIWP                                                   2

#define PERIPHS_IO_MUX_GPIO6_U                       (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO6_GPIO6_0                                                 0
#define FUNC_GPIO6_GPIO6                                                   1
#define FUNC_GPIO6_FSPICLK                                                 2

// Strapping: JTAG sel
#define PERIPHS_IO_MUX_GPIO7_U                       (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO7_GPIO7_0                                                 0
#define FUNC_GPIO7_GPIO7                                                   1
#define FUNC_GPIO7_FSPID                                                   2

#define PERIPHS_IO_MUX_GPIO8_U                       (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO8_GPIO8_0                                                 0
#define FUNC_GPIO8_GPIO8                                                   1

#define PERIPHS_IO_MUX_GPIO9_U                       (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO9_GPIO9_0                                                 0
#define FUNC_GPIO9_GPIO9                                                   1

#define PERIPHS_IO_MUX_GPIO10_U                      (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO10_GPIO10_0                                               0
#define FUNC_GPIO10_GPIO10                                                 1
#define FUNC_GPIO10_FSPICS0                                                2

#define PERIPHS_IO_MUX_U0TXD_U                       (REG_IO_MUX_BASE + 0x0)
#define FUNC_U0TXD_U0TXD                                                   0
#define FUNC_U0TXD_GPIO11                                                  1

#define PERIPHS_IO_MUX_U0RXD_U                       (REG_IO_MUX_BASE + 0x0)
#define FUNC_U0RXD_U0RXD                                                   0
#define FUNC_U0RXD_GPIO12                                                  1

#define PERIPHS_IO_MUX_GPIO13_U                      (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO13_GPIO13_0                                               0
#define FUNC_GPIO13_GPIO13                                                 1

#define PERIPHS_IO_MUX_GPIO14_U                      (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO14_GPIO14_0                                               0
#define FUNC_GPIO14_GPIO14                                                 1

#define PERIPHS_IO_MUX_SPICS1_U                      (REG_IO_MUX_BASE + 0x0)
#define FUNC_SPICS1_SPICS1                                                 0
#define FUNC_SPICS1_GPIO15                                                 1

#define PERIPHS_IO_MUX_SPICS0_U                      (REG_IO_MUX_BASE + 0x0)
#define FUNC_SPICS0_SPICS0                                                 0
#define FUNC_SPICS0_GPIO16                                                 1

#define PERIPHS_IO_MUX_SPIQ_U                        (REG_IO_MUX_BASE + 0x0)
#define FUNC_SPIQ_SPIQ                                                     0
#define FUNC_SPIQ_GPIO17                                                   1

#define PERIPHS_IO_MUX_SPIWP_U                       (REG_IO_MUX_BASE + 0x0)
#define FUNC_SPIWP_SPIWP                                                   0
#define FUNC_SPIWP_GPIO18                                                  1

#define PERIPHS_IO_MUX_VDD_SPI_U                     (REG_IO_MUX_BASE + 0x0)
#define FUNC_VDD_SPI_GPIO19_0                                              0
#define FUNC_VDD_SPI_GPIO19                                                1

#define PERIPHS_IO_MUX_SPIHD_U                       (REG_IO_MUX_BASE + 0x0)
#define FUNC_SPIHD_SPIHD                                                   0
#define FUNC_SPIHD_GPIO20                                                  1

#define PERIPHS_IO_MUX_SPICLK_U                      (REG_IO_MUX_BASE + 0x0)
#define FUNC_SPICLK_SPICLK                                                 0
#define FUNC_SPICLK_GPIO21                                                 1

#define PERIPHS_IO_MUX_SPID_U                        (REG_IO_MUX_BASE + 0x0)
#define FUNC_SPID_SPID                                                     0
#define FUNC_SPID_GPIO22                                                   1

#define PERIPHS_IO_MUX_GPIO23_U                      (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO23_GPIO23_0                                               0
#define FUNC_GPIO23_GPIO23                                                 1

#define PERIPHS_IO_MUX_GPIO24_U                      (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO24_GPIO24_0                                               0
#define FUNC_GPIO24_GPIO24                                                 1

#define PERIPHS_IO_MUX_GPIO25_U                      (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO25_GPIO25_0                                               0
#define FUNC_GPIO25_GPIO25                                                 1

// Strapping: Boot Mode select (analog mode)
#define PERIPHS_IO_MUX_GPIO26_U                      (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO26_GPIO26_0                                               0
#define FUNC_GPIO26_GPIO26                                                 1

// Strapping: Boot Mode select
#define PERIPHS_IO_MUX_GPIO27_U                      (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO27_GPIO27_0                                               0
#define FUNC_GPIO27_GPIO27                                                 1

// Strapping: Boot Mode select
#define PERIPHS_IO_MUX_GPIO28_U                      (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO28_GPIO28_0                                               0
#define FUNC_GPIO28_GPIO28                                                 1


/** IO_MUX_GPIO0_REG register
 *  IO MUX configuration register for GPIO0
 */
#define IO_MUX_GPIO0_REG (DR_REG_IO_MUX_BASE + 0x0)
/** IO_MUX_GPIO0_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO0 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO0_MCU_OE    (BIT(0))
#define IO_MUX_GPIO0_MCU_OE_M  (IO_MUX_GPIO0_MCU_OE_V << IO_MUX_GPIO0_MCU_OE_S)
#define IO_MUX_GPIO0_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO0_MCU_OE_S  0
/** IO_MUX_GPIO0_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO0.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO0_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO0_SLP_SEL_M  (IO_MUX_GPIO0_SLP_SEL_V << IO_MUX_GPIO0_SLP_SEL_S)
#define IO_MUX_GPIO0_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO0_SLP_SEL_S  1
/** IO_MUX_GPIO0_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO0 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO0_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO0_MCU_WPD_M  (IO_MUX_GPIO0_MCU_WPD_V << IO_MUX_GPIO0_MCU_WPD_S)
#define IO_MUX_GPIO0_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO0_MCU_WPD_S  2
/** IO_MUX_GPIO0_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO0 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO0_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO0_MCU_WPU_M  (IO_MUX_GPIO0_MCU_WPU_V << IO_MUX_GPIO0_MCU_WPU_S)
#define IO_MUX_GPIO0_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO0_MCU_WPU_S  3
/** IO_MUX_GPIO0_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO0 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO0_MCU_IE    (BIT(4))
#define IO_MUX_GPIO0_MCU_IE_M  (IO_MUX_GPIO0_MCU_IE_V << IO_MUX_GPIO0_MCU_IE_S)
#define IO_MUX_GPIO0_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO0_MCU_IE_S  4
/** IO_MUX_GPIO0_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO0 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO0_MCU_DRV    0x00000003U
#define IO_MUX_GPIO0_MCU_DRV_M  (IO_MUX_GPIO0_MCU_DRV_V << IO_MUX_GPIO0_MCU_DRV_S)
#define IO_MUX_GPIO0_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO0_MCU_DRV_S  5
/** IO_MUX_GPIO0_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO0.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO0_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO0_FUN_WPD_M  (IO_MUX_GPIO0_FUN_WPD_V << IO_MUX_GPIO0_FUN_WPD_S)
#define IO_MUX_GPIO0_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO0_FUN_WPD_S  7
/** IO_MUX_GPIO0_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO0.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO0_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO0_FUN_WPU_M  (IO_MUX_GPIO0_FUN_WPU_V << IO_MUX_GPIO0_FUN_WPU_S)
#define IO_MUX_GPIO0_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO0_FUN_WPU_S  8
/** IO_MUX_GPIO0_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO0.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO0_FUN_IE    (BIT(9))
#define IO_MUX_GPIO0_FUN_IE_M  (IO_MUX_GPIO0_FUN_IE_V << IO_MUX_GPIO0_FUN_IE_S)
#define IO_MUX_GPIO0_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO0_FUN_IE_S  9
/** IO_MUX_GPIO0_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO0. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO0_FUN_DRV    0x00000003U
#define IO_MUX_GPIO0_FUN_DRV_M  (IO_MUX_GPIO0_FUN_DRV_V << IO_MUX_GPIO0_FUN_DRV_S)
#define IO_MUX_GPIO0_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO0_FUN_DRV_S  10
/** IO_MUX_GPIO0_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO0_MCU_SEL    0x00000007U
#define IO_MUX_GPIO0_MCU_SEL_M  (IO_MUX_GPIO0_MCU_SEL_V << IO_MUX_GPIO0_MCU_SEL_S)
#define IO_MUX_GPIO0_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO0_MCU_SEL_S  12
/** IO_MUX_GPIO0_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO0_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO0_FILTER_EN_M  (IO_MUX_GPIO0_FILTER_EN_V << IO_MUX_GPIO0_FILTER_EN_S)
#define IO_MUX_GPIO0_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO0_FILTER_EN_S  15
/** IO_MUX_GPIO0_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO0_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO0_HYS_EN    (BIT(16))
#define IO_MUX_GPIO0_HYS_EN_M  (IO_MUX_GPIO0_HYS_EN_V << IO_MUX_GPIO0_HYS_EN_S)
#define IO_MUX_GPIO0_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO0_HYS_EN_S  16
/** IO_MUX_GPIO0_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO0. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO0_HYS_EN\\
 */
#define IO_MUX_GPIO0_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO0_HYS_SEL_M  (IO_MUX_GPIO0_HYS_SEL_V << IO_MUX_GPIO0_HYS_SEL_S)
#define IO_MUX_GPIO0_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO0_HYS_SEL_S  17

/** IO_MUX_GPIO1_REG register
 *  IO MUX configuration register for GPIO1
 */
#define IO_MUX_GPIO1_REG (DR_REG_IO_MUX_BASE + 0x4)
/** IO_MUX_GPIO1_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO1 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO1_MCU_OE    (BIT(0))
#define IO_MUX_GPIO1_MCU_OE_M  (IO_MUX_GPIO1_MCU_OE_V << IO_MUX_GPIO1_MCU_OE_S)
#define IO_MUX_GPIO1_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO1_MCU_OE_S  0
/** IO_MUX_GPIO1_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO1.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO1_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO1_SLP_SEL_M  (IO_MUX_GPIO1_SLP_SEL_V << IO_MUX_GPIO1_SLP_SEL_S)
#define IO_MUX_GPIO1_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO1_SLP_SEL_S  1
/** IO_MUX_GPIO1_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO1 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO1_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO1_MCU_WPD_M  (IO_MUX_GPIO1_MCU_WPD_V << IO_MUX_GPIO1_MCU_WPD_S)
#define IO_MUX_GPIO1_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO1_MCU_WPD_S  2
/** IO_MUX_GPIO1_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO1 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO1_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO1_MCU_WPU_M  (IO_MUX_GPIO1_MCU_WPU_V << IO_MUX_GPIO1_MCU_WPU_S)
#define IO_MUX_GPIO1_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO1_MCU_WPU_S  3
/** IO_MUX_GPIO1_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO1 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO1_MCU_IE    (BIT(4))
#define IO_MUX_GPIO1_MCU_IE_M  (IO_MUX_GPIO1_MCU_IE_V << IO_MUX_GPIO1_MCU_IE_S)
#define IO_MUX_GPIO1_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO1_MCU_IE_S  4
/** IO_MUX_GPIO1_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO1 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO1_MCU_DRV    0x00000003U
#define IO_MUX_GPIO1_MCU_DRV_M  (IO_MUX_GPIO1_MCU_DRV_V << IO_MUX_GPIO1_MCU_DRV_S)
#define IO_MUX_GPIO1_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO1_MCU_DRV_S  5
/** IO_MUX_GPIO1_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO1_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO1_FUN_WPD_M  (IO_MUX_GPIO1_FUN_WPD_V << IO_MUX_GPIO1_FUN_WPD_S)
#define IO_MUX_GPIO1_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO1_FUN_WPD_S  7
/** IO_MUX_GPIO1_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO1_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO1_FUN_WPU_M  (IO_MUX_GPIO1_FUN_WPU_V << IO_MUX_GPIO1_FUN_WPU_S)
#define IO_MUX_GPIO1_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO1_FUN_WPU_S  8
/** IO_MUX_GPIO1_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO1_FUN_IE    (BIT(9))
#define IO_MUX_GPIO1_FUN_IE_M  (IO_MUX_GPIO1_FUN_IE_V << IO_MUX_GPIO1_FUN_IE_S)
#define IO_MUX_GPIO1_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO1_FUN_IE_S  9
/** IO_MUX_GPIO1_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO1. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO1_FUN_DRV    0x00000003U
#define IO_MUX_GPIO1_FUN_DRV_M  (IO_MUX_GPIO1_FUN_DRV_V << IO_MUX_GPIO1_FUN_DRV_S)
#define IO_MUX_GPIO1_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO1_FUN_DRV_S  10
/** IO_MUX_GPIO1_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO1_MCU_SEL    0x00000007U
#define IO_MUX_GPIO1_MCU_SEL_M  (IO_MUX_GPIO1_MCU_SEL_V << IO_MUX_GPIO1_MCU_SEL_S)
#define IO_MUX_GPIO1_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO1_MCU_SEL_S  12
/** IO_MUX_GPIO1_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO1_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO1_FILTER_EN_M  (IO_MUX_GPIO1_FILTER_EN_V << IO_MUX_GPIO1_FILTER_EN_S)
#define IO_MUX_GPIO1_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO1_FILTER_EN_S  15
/** IO_MUX_GPIO1_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO1_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO1_HYS_EN    (BIT(16))
#define IO_MUX_GPIO1_HYS_EN_M  (IO_MUX_GPIO1_HYS_EN_V << IO_MUX_GPIO1_HYS_EN_S)
#define IO_MUX_GPIO1_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO1_HYS_EN_S  16
/** IO_MUX_GPIO1_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO1. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO1_HYS_EN\\
 */
#define IO_MUX_GPIO1_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO1_HYS_SEL_M  (IO_MUX_GPIO1_HYS_SEL_V << IO_MUX_GPIO1_HYS_SEL_S)
#define IO_MUX_GPIO1_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO1_HYS_SEL_S  17

/** IO_MUX_GPIO2_REG register
 *  IO MUX configuration register for GPIO2
 */
#define IO_MUX_GPIO2_REG (DR_REG_IO_MUX_BASE + 0x8)
/** IO_MUX_GPIO2_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO2 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO2_MCU_OE    (BIT(0))
#define IO_MUX_GPIO2_MCU_OE_M  (IO_MUX_GPIO2_MCU_OE_V << IO_MUX_GPIO2_MCU_OE_S)
#define IO_MUX_GPIO2_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO2_MCU_OE_S  0
/** IO_MUX_GPIO2_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO2.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO2_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO2_SLP_SEL_M  (IO_MUX_GPIO2_SLP_SEL_V << IO_MUX_GPIO2_SLP_SEL_S)
#define IO_MUX_GPIO2_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO2_SLP_SEL_S  1
/** IO_MUX_GPIO2_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO2 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO2_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO2_MCU_WPD_M  (IO_MUX_GPIO2_MCU_WPD_V << IO_MUX_GPIO2_MCU_WPD_S)
#define IO_MUX_GPIO2_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO2_MCU_WPD_S  2
/** IO_MUX_GPIO2_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO2 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO2_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO2_MCU_WPU_M  (IO_MUX_GPIO2_MCU_WPU_V << IO_MUX_GPIO2_MCU_WPU_S)
#define IO_MUX_GPIO2_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO2_MCU_WPU_S  3
/** IO_MUX_GPIO2_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO2 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO2_MCU_IE    (BIT(4))
#define IO_MUX_GPIO2_MCU_IE_M  (IO_MUX_GPIO2_MCU_IE_V << IO_MUX_GPIO2_MCU_IE_S)
#define IO_MUX_GPIO2_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO2_MCU_IE_S  4
/** IO_MUX_GPIO2_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO2 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO2_MCU_DRV    0x00000003U
#define IO_MUX_GPIO2_MCU_DRV_M  (IO_MUX_GPIO2_MCU_DRV_V << IO_MUX_GPIO2_MCU_DRV_S)
#define IO_MUX_GPIO2_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO2_MCU_DRV_S  5
/** IO_MUX_GPIO2_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO2.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO2_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO2_FUN_WPD_M  (IO_MUX_GPIO2_FUN_WPD_V << IO_MUX_GPIO2_FUN_WPD_S)
#define IO_MUX_GPIO2_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO2_FUN_WPD_S  7
/** IO_MUX_GPIO2_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO2.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO2_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO2_FUN_WPU_M  (IO_MUX_GPIO2_FUN_WPU_V << IO_MUX_GPIO2_FUN_WPU_S)
#define IO_MUX_GPIO2_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO2_FUN_WPU_S  8
/** IO_MUX_GPIO2_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO2.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO2_FUN_IE    (BIT(9))
#define IO_MUX_GPIO2_FUN_IE_M  (IO_MUX_GPIO2_FUN_IE_V << IO_MUX_GPIO2_FUN_IE_S)
#define IO_MUX_GPIO2_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO2_FUN_IE_S  9
/** IO_MUX_GPIO2_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO2. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO2_FUN_DRV    0x00000003U
#define IO_MUX_GPIO2_FUN_DRV_M  (IO_MUX_GPIO2_FUN_DRV_V << IO_MUX_GPIO2_FUN_DRV_S)
#define IO_MUX_GPIO2_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO2_FUN_DRV_S  10
/** IO_MUX_GPIO2_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO2_MCU_SEL    0x00000007U
#define IO_MUX_GPIO2_MCU_SEL_M  (IO_MUX_GPIO2_MCU_SEL_V << IO_MUX_GPIO2_MCU_SEL_S)
#define IO_MUX_GPIO2_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO2_MCU_SEL_S  12
/** IO_MUX_GPIO2_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO2_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO2_FILTER_EN_M  (IO_MUX_GPIO2_FILTER_EN_V << IO_MUX_GPIO2_FILTER_EN_S)
#define IO_MUX_GPIO2_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO2_FILTER_EN_S  15
/** IO_MUX_GPIO2_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO2_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO2_HYS_EN    (BIT(16))
#define IO_MUX_GPIO2_HYS_EN_M  (IO_MUX_GPIO2_HYS_EN_V << IO_MUX_GPIO2_HYS_EN_S)
#define IO_MUX_GPIO2_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO2_HYS_EN_S  16
/** IO_MUX_GPIO2_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO2. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO2_HYS_EN\\
 */
#define IO_MUX_GPIO2_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO2_HYS_SEL_M  (IO_MUX_GPIO2_HYS_SEL_V << IO_MUX_GPIO2_HYS_SEL_S)
#define IO_MUX_GPIO2_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO2_HYS_SEL_S  17

/** IO_MUX_GPIO3_REG register
 *  IO MUX configuration register for GPIO3
 */
#define IO_MUX_GPIO3_REG (DR_REG_IO_MUX_BASE + 0xc)
/** IO_MUX_GPIO3_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO3 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO3_MCU_OE    (BIT(0))
#define IO_MUX_GPIO3_MCU_OE_M  (IO_MUX_GPIO3_MCU_OE_V << IO_MUX_GPIO3_MCU_OE_S)
#define IO_MUX_GPIO3_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO3_MCU_OE_S  0
/** IO_MUX_GPIO3_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO3.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO3_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO3_SLP_SEL_M  (IO_MUX_GPIO3_SLP_SEL_V << IO_MUX_GPIO3_SLP_SEL_S)
#define IO_MUX_GPIO3_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO3_SLP_SEL_S  1
/** IO_MUX_GPIO3_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO3 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO3_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO3_MCU_WPD_M  (IO_MUX_GPIO3_MCU_WPD_V << IO_MUX_GPIO3_MCU_WPD_S)
#define IO_MUX_GPIO3_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO3_MCU_WPD_S  2
/** IO_MUX_GPIO3_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO3 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO3_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO3_MCU_WPU_M  (IO_MUX_GPIO3_MCU_WPU_V << IO_MUX_GPIO3_MCU_WPU_S)
#define IO_MUX_GPIO3_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO3_MCU_WPU_S  3
/** IO_MUX_GPIO3_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO3 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO3_MCU_IE    (BIT(4))
#define IO_MUX_GPIO3_MCU_IE_M  (IO_MUX_GPIO3_MCU_IE_V << IO_MUX_GPIO3_MCU_IE_S)
#define IO_MUX_GPIO3_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO3_MCU_IE_S  4
/** IO_MUX_GPIO3_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO3 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO3_MCU_DRV    0x00000003U
#define IO_MUX_GPIO3_MCU_DRV_M  (IO_MUX_GPIO3_MCU_DRV_V << IO_MUX_GPIO3_MCU_DRV_S)
#define IO_MUX_GPIO3_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO3_MCU_DRV_S  5
/** IO_MUX_GPIO3_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO3.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO3_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO3_FUN_WPD_M  (IO_MUX_GPIO3_FUN_WPD_V << IO_MUX_GPIO3_FUN_WPD_S)
#define IO_MUX_GPIO3_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO3_FUN_WPD_S  7
/** IO_MUX_GPIO3_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO3.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO3_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO3_FUN_WPU_M  (IO_MUX_GPIO3_FUN_WPU_V << IO_MUX_GPIO3_FUN_WPU_S)
#define IO_MUX_GPIO3_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO3_FUN_WPU_S  8
/** IO_MUX_GPIO3_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO3.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO3_FUN_IE    (BIT(9))
#define IO_MUX_GPIO3_FUN_IE_M  (IO_MUX_GPIO3_FUN_IE_V << IO_MUX_GPIO3_FUN_IE_S)
#define IO_MUX_GPIO3_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO3_FUN_IE_S  9
/** IO_MUX_GPIO3_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO3. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO3_FUN_DRV    0x00000003U
#define IO_MUX_GPIO3_FUN_DRV_M  (IO_MUX_GPIO3_FUN_DRV_V << IO_MUX_GPIO3_FUN_DRV_S)
#define IO_MUX_GPIO3_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO3_FUN_DRV_S  10
/** IO_MUX_GPIO3_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO3_MCU_SEL    0x00000007U
#define IO_MUX_GPIO3_MCU_SEL_M  (IO_MUX_GPIO3_MCU_SEL_V << IO_MUX_GPIO3_MCU_SEL_S)
#define IO_MUX_GPIO3_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO3_MCU_SEL_S  12
/** IO_MUX_GPIO3_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO3_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO3_FILTER_EN_M  (IO_MUX_GPIO3_FILTER_EN_V << IO_MUX_GPIO3_FILTER_EN_S)
#define IO_MUX_GPIO3_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO3_FILTER_EN_S  15
/** IO_MUX_GPIO3_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO3_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO3_HYS_EN    (BIT(16))
#define IO_MUX_GPIO3_HYS_EN_M  (IO_MUX_GPIO3_HYS_EN_V << IO_MUX_GPIO3_HYS_EN_S)
#define IO_MUX_GPIO3_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO3_HYS_EN_S  16
/** IO_MUX_GPIO3_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO3. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO3_HYS_EN\\
 */
#define IO_MUX_GPIO3_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO3_HYS_SEL_M  (IO_MUX_GPIO3_HYS_SEL_V << IO_MUX_GPIO3_HYS_SEL_S)
#define IO_MUX_GPIO3_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO3_HYS_SEL_S  17

/** IO_MUX_GPIO4_REG register
 *  IO MUX configuration register for GPIO4
 */
#define IO_MUX_GPIO4_REG (DR_REG_IO_MUX_BASE + 0x10)
/** IO_MUX_GPIO4_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO4 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO4_MCU_OE    (BIT(0))
#define IO_MUX_GPIO4_MCU_OE_M  (IO_MUX_GPIO4_MCU_OE_V << IO_MUX_GPIO4_MCU_OE_S)
#define IO_MUX_GPIO4_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO4_MCU_OE_S  0
/** IO_MUX_GPIO4_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO4.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO4_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO4_SLP_SEL_M  (IO_MUX_GPIO4_SLP_SEL_V << IO_MUX_GPIO4_SLP_SEL_S)
#define IO_MUX_GPIO4_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO4_SLP_SEL_S  1
/** IO_MUX_GPIO4_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO4 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO4_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO4_MCU_WPD_M  (IO_MUX_GPIO4_MCU_WPD_V << IO_MUX_GPIO4_MCU_WPD_S)
#define IO_MUX_GPIO4_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO4_MCU_WPD_S  2
/** IO_MUX_GPIO4_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO4 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO4_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO4_MCU_WPU_M  (IO_MUX_GPIO4_MCU_WPU_V << IO_MUX_GPIO4_MCU_WPU_S)
#define IO_MUX_GPIO4_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO4_MCU_WPU_S  3
/** IO_MUX_GPIO4_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO4 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO4_MCU_IE    (BIT(4))
#define IO_MUX_GPIO4_MCU_IE_M  (IO_MUX_GPIO4_MCU_IE_V << IO_MUX_GPIO4_MCU_IE_S)
#define IO_MUX_GPIO4_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO4_MCU_IE_S  4
/** IO_MUX_GPIO4_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO4 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO4_MCU_DRV    0x00000003U
#define IO_MUX_GPIO4_MCU_DRV_M  (IO_MUX_GPIO4_MCU_DRV_V << IO_MUX_GPIO4_MCU_DRV_S)
#define IO_MUX_GPIO4_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO4_MCU_DRV_S  5
/** IO_MUX_GPIO4_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO4.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO4_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO4_FUN_WPD_M  (IO_MUX_GPIO4_FUN_WPD_V << IO_MUX_GPIO4_FUN_WPD_S)
#define IO_MUX_GPIO4_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO4_FUN_WPD_S  7
/** IO_MUX_GPIO4_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO4.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO4_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO4_FUN_WPU_M  (IO_MUX_GPIO4_FUN_WPU_V << IO_MUX_GPIO4_FUN_WPU_S)
#define IO_MUX_GPIO4_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO4_FUN_WPU_S  8
/** IO_MUX_GPIO4_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO4.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO4_FUN_IE    (BIT(9))
#define IO_MUX_GPIO4_FUN_IE_M  (IO_MUX_GPIO4_FUN_IE_V << IO_MUX_GPIO4_FUN_IE_S)
#define IO_MUX_GPIO4_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO4_FUN_IE_S  9
/** IO_MUX_GPIO4_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO4. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO4_FUN_DRV    0x00000003U
#define IO_MUX_GPIO4_FUN_DRV_M  (IO_MUX_GPIO4_FUN_DRV_V << IO_MUX_GPIO4_FUN_DRV_S)
#define IO_MUX_GPIO4_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO4_FUN_DRV_S  10
/** IO_MUX_GPIO4_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO4_MCU_SEL    0x00000007U
#define IO_MUX_GPIO4_MCU_SEL_M  (IO_MUX_GPIO4_MCU_SEL_V << IO_MUX_GPIO4_MCU_SEL_S)
#define IO_MUX_GPIO4_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO4_MCU_SEL_S  12
/** IO_MUX_GPIO4_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO4_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO4_FILTER_EN_M  (IO_MUX_GPIO4_FILTER_EN_V << IO_MUX_GPIO4_FILTER_EN_S)
#define IO_MUX_GPIO4_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO4_FILTER_EN_S  15
/** IO_MUX_GPIO4_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO4_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO4_HYS_EN    (BIT(16))
#define IO_MUX_GPIO4_HYS_EN_M  (IO_MUX_GPIO4_HYS_EN_V << IO_MUX_GPIO4_HYS_EN_S)
#define IO_MUX_GPIO4_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO4_HYS_EN_S  16
/** IO_MUX_GPIO4_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO4. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO4_HYS_EN\\
 */
#define IO_MUX_GPIO4_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO4_HYS_SEL_M  (IO_MUX_GPIO4_HYS_SEL_V << IO_MUX_GPIO4_HYS_SEL_S)
#define IO_MUX_GPIO4_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO4_HYS_SEL_S  17

/** IO_MUX_GPIO5_REG register
 *  IO MUX configuration register for GPIO5
 */
#define IO_MUX_GPIO5_REG (DR_REG_IO_MUX_BASE + 0x14)
/** IO_MUX_GPIO5_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO5 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO5_MCU_OE    (BIT(0))
#define IO_MUX_GPIO5_MCU_OE_M  (IO_MUX_GPIO5_MCU_OE_V << IO_MUX_GPIO5_MCU_OE_S)
#define IO_MUX_GPIO5_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO5_MCU_OE_S  0
/** IO_MUX_GPIO5_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO5.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO5_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO5_SLP_SEL_M  (IO_MUX_GPIO5_SLP_SEL_V << IO_MUX_GPIO5_SLP_SEL_S)
#define IO_MUX_GPIO5_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO5_SLP_SEL_S  1
/** IO_MUX_GPIO5_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO5 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO5_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO5_MCU_WPD_M  (IO_MUX_GPIO5_MCU_WPD_V << IO_MUX_GPIO5_MCU_WPD_S)
#define IO_MUX_GPIO5_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO5_MCU_WPD_S  2
/** IO_MUX_GPIO5_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO5 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO5_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO5_MCU_WPU_M  (IO_MUX_GPIO5_MCU_WPU_V << IO_MUX_GPIO5_MCU_WPU_S)
#define IO_MUX_GPIO5_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO5_MCU_WPU_S  3
/** IO_MUX_GPIO5_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO5 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO5_MCU_IE    (BIT(4))
#define IO_MUX_GPIO5_MCU_IE_M  (IO_MUX_GPIO5_MCU_IE_V << IO_MUX_GPIO5_MCU_IE_S)
#define IO_MUX_GPIO5_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO5_MCU_IE_S  4
/** IO_MUX_GPIO5_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO5 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO5_MCU_DRV    0x00000003U
#define IO_MUX_GPIO5_MCU_DRV_M  (IO_MUX_GPIO5_MCU_DRV_V << IO_MUX_GPIO5_MCU_DRV_S)
#define IO_MUX_GPIO5_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO5_MCU_DRV_S  5
/** IO_MUX_GPIO5_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO5.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO5_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO5_FUN_WPD_M  (IO_MUX_GPIO5_FUN_WPD_V << IO_MUX_GPIO5_FUN_WPD_S)
#define IO_MUX_GPIO5_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO5_FUN_WPD_S  7
/** IO_MUX_GPIO5_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO5.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO5_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO5_FUN_WPU_M  (IO_MUX_GPIO5_FUN_WPU_V << IO_MUX_GPIO5_FUN_WPU_S)
#define IO_MUX_GPIO5_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO5_FUN_WPU_S  8
/** IO_MUX_GPIO5_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO5.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO5_FUN_IE    (BIT(9))
#define IO_MUX_GPIO5_FUN_IE_M  (IO_MUX_GPIO5_FUN_IE_V << IO_MUX_GPIO5_FUN_IE_S)
#define IO_MUX_GPIO5_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO5_FUN_IE_S  9
/** IO_MUX_GPIO5_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO5. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO5_FUN_DRV    0x00000003U
#define IO_MUX_GPIO5_FUN_DRV_M  (IO_MUX_GPIO5_FUN_DRV_V << IO_MUX_GPIO5_FUN_DRV_S)
#define IO_MUX_GPIO5_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO5_FUN_DRV_S  10
/** IO_MUX_GPIO5_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO5_MCU_SEL    0x00000007U
#define IO_MUX_GPIO5_MCU_SEL_M  (IO_MUX_GPIO5_MCU_SEL_V << IO_MUX_GPIO5_MCU_SEL_S)
#define IO_MUX_GPIO5_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO5_MCU_SEL_S  12
/** IO_MUX_GPIO5_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO5_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO5_FILTER_EN_M  (IO_MUX_GPIO5_FILTER_EN_V << IO_MUX_GPIO5_FILTER_EN_S)
#define IO_MUX_GPIO5_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO5_FILTER_EN_S  15
/** IO_MUX_GPIO5_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO5_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO5_HYS_EN    (BIT(16))
#define IO_MUX_GPIO5_HYS_EN_M  (IO_MUX_GPIO5_HYS_EN_V << IO_MUX_GPIO5_HYS_EN_S)
#define IO_MUX_GPIO5_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO5_HYS_EN_S  16
/** IO_MUX_GPIO5_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO5. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO5_HYS_EN\\
 */
#define IO_MUX_GPIO5_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO5_HYS_SEL_M  (IO_MUX_GPIO5_HYS_SEL_V << IO_MUX_GPIO5_HYS_SEL_S)
#define IO_MUX_GPIO5_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO5_HYS_SEL_S  17

/** IO_MUX_GPIO6_REG register
 *  IO MUX configuration register for GPIO6
 */
#define IO_MUX_GPIO6_REG (DR_REG_IO_MUX_BASE + 0x18)
/** IO_MUX_GPIO6_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO6 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO6_MCU_OE    (BIT(0))
#define IO_MUX_GPIO6_MCU_OE_M  (IO_MUX_GPIO6_MCU_OE_V << IO_MUX_GPIO6_MCU_OE_S)
#define IO_MUX_GPIO6_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO6_MCU_OE_S  0
/** IO_MUX_GPIO6_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO6.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO6_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO6_SLP_SEL_M  (IO_MUX_GPIO6_SLP_SEL_V << IO_MUX_GPIO6_SLP_SEL_S)
#define IO_MUX_GPIO6_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO6_SLP_SEL_S  1
/** IO_MUX_GPIO6_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO6 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO6_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO6_MCU_WPD_M  (IO_MUX_GPIO6_MCU_WPD_V << IO_MUX_GPIO6_MCU_WPD_S)
#define IO_MUX_GPIO6_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO6_MCU_WPD_S  2
/** IO_MUX_GPIO6_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO6 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO6_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO6_MCU_WPU_M  (IO_MUX_GPIO6_MCU_WPU_V << IO_MUX_GPIO6_MCU_WPU_S)
#define IO_MUX_GPIO6_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO6_MCU_WPU_S  3
/** IO_MUX_GPIO6_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO6 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO6_MCU_IE    (BIT(4))
#define IO_MUX_GPIO6_MCU_IE_M  (IO_MUX_GPIO6_MCU_IE_V << IO_MUX_GPIO6_MCU_IE_S)
#define IO_MUX_GPIO6_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO6_MCU_IE_S  4
/** IO_MUX_GPIO6_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO6 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO6_MCU_DRV    0x00000003U
#define IO_MUX_GPIO6_MCU_DRV_M  (IO_MUX_GPIO6_MCU_DRV_V << IO_MUX_GPIO6_MCU_DRV_S)
#define IO_MUX_GPIO6_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO6_MCU_DRV_S  5
/** IO_MUX_GPIO6_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO6.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO6_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO6_FUN_WPD_M  (IO_MUX_GPIO6_FUN_WPD_V << IO_MUX_GPIO6_FUN_WPD_S)
#define IO_MUX_GPIO6_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO6_FUN_WPD_S  7
/** IO_MUX_GPIO6_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO6.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO6_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO6_FUN_WPU_M  (IO_MUX_GPIO6_FUN_WPU_V << IO_MUX_GPIO6_FUN_WPU_S)
#define IO_MUX_GPIO6_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO6_FUN_WPU_S  8
/** IO_MUX_GPIO6_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO6.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO6_FUN_IE    (BIT(9))
#define IO_MUX_GPIO6_FUN_IE_M  (IO_MUX_GPIO6_FUN_IE_V << IO_MUX_GPIO6_FUN_IE_S)
#define IO_MUX_GPIO6_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO6_FUN_IE_S  9
/** IO_MUX_GPIO6_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO6. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO6_FUN_DRV    0x00000003U
#define IO_MUX_GPIO6_FUN_DRV_M  (IO_MUX_GPIO6_FUN_DRV_V << IO_MUX_GPIO6_FUN_DRV_S)
#define IO_MUX_GPIO6_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO6_FUN_DRV_S  10
/** IO_MUX_GPIO6_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO6_MCU_SEL    0x00000007U
#define IO_MUX_GPIO6_MCU_SEL_M  (IO_MUX_GPIO6_MCU_SEL_V << IO_MUX_GPIO6_MCU_SEL_S)
#define IO_MUX_GPIO6_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO6_MCU_SEL_S  12
/** IO_MUX_GPIO6_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO6_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO6_FILTER_EN_M  (IO_MUX_GPIO6_FILTER_EN_V << IO_MUX_GPIO6_FILTER_EN_S)
#define IO_MUX_GPIO6_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO6_FILTER_EN_S  15
/** IO_MUX_GPIO6_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO6_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO6_HYS_EN    (BIT(16))
#define IO_MUX_GPIO6_HYS_EN_M  (IO_MUX_GPIO6_HYS_EN_V << IO_MUX_GPIO6_HYS_EN_S)
#define IO_MUX_GPIO6_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO6_HYS_EN_S  16
/** IO_MUX_GPIO6_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO6. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO6_HYS_EN\\
 */
#define IO_MUX_GPIO6_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO6_HYS_SEL_M  (IO_MUX_GPIO6_HYS_SEL_V << IO_MUX_GPIO6_HYS_SEL_S)
#define IO_MUX_GPIO6_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO6_HYS_SEL_S  17

/** IO_MUX_GPIO7_REG register
 *  IO MUX configuration register for GPIO7
 */
#define IO_MUX_GPIO7_REG (DR_REG_IO_MUX_BASE + 0x1c)
/** IO_MUX_GPIO7_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO7 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO7_MCU_OE    (BIT(0))
#define IO_MUX_GPIO7_MCU_OE_M  (IO_MUX_GPIO7_MCU_OE_V << IO_MUX_GPIO7_MCU_OE_S)
#define IO_MUX_GPIO7_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO7_MCU_OE_S  0
/** IO_MUX_GPIO7_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO7.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO7_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO7_SLP_SEL_M  (IO_MUX_GPIO7_SLP_SEL_V << IO_MUX_GPIO7_SLP_SEL_S)
#define IO_MUX_GPIO7_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO7_SLP_SEL_S  1
/** IO_MUX_GPIO7_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO7 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO7_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO7_MCU_WPD_M  (IO_MUX_GPIO7_MCU_WPD_V << IO_MUX_GPIO7_MCU_WPD_S)
#define IO_MUX_GPIO7_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO7_MCU_WPD_S  2
/** IO_MUX_GPIO7_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO7 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO7_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO7_MCU_WPU_M  (IO_MUX_GPIO7_MCU_WPU_V << IO_MUX_GPIO7_MCU_WPU_S)
#define IO_MUX_GPIO7_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO7_MCU_WPU_S  3
/** IO_MUX_GPIO7_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO7 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO7_MCU_IE    (BIT(4))
#define IO_MUX_GPIO7_MCU_IE_M  (IO_MUX_GPIO7_MCU_IE_V << IO_MUX_GPIO7_MCU_IE_S)
#define IO_MUX_GPIO7_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO7_MCU_IE_S  4
/** IO_MUX_GPIO7_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO7 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO7_MCU_DRV    0x00000003U
#define IO_MUX_GPIO7_MCU_DRV_M  (IO_MUX_GPIO7_MCU_DRV_V << IO_MUX_GPIO7_MCU_DRV_S)
#define IO_MUX_GPIO7_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO7_MCU_DRV_S  5
/** IO_MUX_GPIO7_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO7.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO7_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO7_FUN_WPD_M  (IO_MUX_GPIO7_FUN_WPD_V << IO_MUX_GPIO7_FUN_WPD_S)
#define IO_MUX_GPIO7_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO7_FUN_WPD_S  7
/** IO_MUX_GPIO7_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO7.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO7_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO7_FUN_WPU_M  (IO_MUX_GPIO7_FUN_WPU_V << IO_MUX_GPIO7_FUN_WPU_S)
#define IO_MUX_GPIO7_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO7_FUN_WPU_S  8
/** IO_MUX_GPIO7_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO7.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO7_FUN_IE    (BIT(9))
#define IO_MUX_GPIO7_FUN_IE_M  (IO_MUX_GPIO7_FUN_IE_V << IO_MUX_GPIO7_FUN_IE_S)
#define IO_MUX_GPIO7_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO7_FUN_IE_S  9
/** IO_MUX_GPIO7_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO7. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO7_FUN_DRV    0x00000003U
#define IO_MUX_GPIO7_FUN_DRV_M  (IO_MUX_GPIO7_FUN_DRV_V << IO_MUX_GPIO7_FUN_DRV_S)
#define IO_MUX_GPIO7_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO7_FUN_DRV_S  10
/** IO_MUX_GPIO7_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO7_MCU_SEL    0x00000007U
#define IO_MUX_GPIO7_MCU_SEL_M  (IO_MUX_GPIO7_MCU_SEL_V << IO_MUX_GPIO7_MCU_SEL_S)
#define IO_MUX_GPIO7_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO7_MCU_SEL_S  12
/** IO_MUX_GPIO7_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO7_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO7_FILTER_EN_M  (IO_MUX_GPIO7_FILTER_EN_V << IO_MUX_GPIO7_FILTER_EN_S)
#define IO_MUX_GPIO7_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO7_FILTER_EN_S  15
/** IO_MUX_GPIO7_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO7_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO7_HYS_EN    (BIT(16))
#define IO_MUX_GPIO7_HYS_EN_M  (IO_MUX_GPIO7_HYS_EN_V << IO_MUX_GPIO7_HYS_EN_S)
#define IO_MUX_GPIO7_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO7_HYS_EN_S  16
/** IO_MUX_GPIO7_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO7. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO7_HYS_EN\\
 */
#define IO_MUX_GPIO7_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO7_HYS_SEL_M  (IO_MUX_GPIO7_HYS_SEL_V << IO_MUX_GPIO7_HYS_SEL_S)
#define IO_MUX_GPIO7_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO7_HYS_SEL_S  17

/** IO_MUX_GPIO8_REG register
 *  IO MUX configuration register for GPIO8
 */
#define IO_MUX_GPIO8_REG (DR_REG_IO_MUX_BASE + 0x20)
/** IO_MUX_GPIO8_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO8 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO8_MCU_OE    (BIT(0))
#define IO_MUX_GPIO8_MCU_OE_M  (IO_MUX_GPIO8_MCU_OE_V << IO_MUX_GPIO8_MCU_OE_S)
#define IO_MUX_GPIO8_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO8_MCU_OE_S  0
/** IO_MUX_GPIO8_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO8.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO8_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO8_SLP_SEL_M  (IO_MUX_GPIO8_SLP_SEL_V << IO_MUX_GPIO8_SLP_SEL_S)
#define IO_MUX_GPIO8_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO8_SLP_SEL_S  1
/** IO_MUX_GPIO8_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO8 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO8_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO8_MCU_WPD_M  (IO_MUX_GPIO8_MCU_WPD_V << IO_MUX_GPIO8_MCU_WPD_S)
#define IO_MUX_GPIO8_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO8_MCU_WPD_S  2
/** IO_MUX_GPIO8_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO8 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO8_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO8_MCU_WPU_M  (IO_MUX_GPIO8_MCU_WPU_V << IO_MUX_GPIO8_MCU_WPU_S)
#define IO_MUX_GPIO8_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO8_MCU_WPU_S  3
/** IO_MUX_GPIO8_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO8 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO8_MCU_IE    (BIT(4))
#define IO_MUX_GPIO8_MCU_IE_M  (IO_MUX_GPIO8_MCU_IE_V << IO_MUX_GPIO8_MCU_IE_S)
#define IO_MUX_GPIO8_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO8_MCU_IE_S  4
/** IO_MUX_GPIO8_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO8 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO8_MCU_DRV    0x00000003U
#define IO_MUX_GPIO8_MCU_DRV_M  (IO_MUX_GPIO8_MCU_DRV_V << IO_MUX_GPIO8_MCU_DRV_S)
#define IO_MUX_GPIO8_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO8_MCU_DRV_S  5
/** IO_MUX_GPIO8_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO8.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO8_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO8_FUN_WPD_M  (IO_MUX_GPIO8_FUN_WPD_V << IO_MUX_GPIO8_FUN_WPD_S)
#define IO_MUX_GPIO8_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO8_FUN_WPD_S  7
/** IO_MUX_GPIO8_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO8.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO8_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO8_FUN_WPU_M  (IO_MUX_GPIO8_FUN_WPU_V << IO_MUX_GPIO8_FUN_WPU_S)
#define IO_MUX_GPIO8_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO8_FUN_WPU_S  8
/** IO_MUX_GPIO8_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO8.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO8_FUN_IE    (BIT(9))
#define IO_MUX_GPIO8_FUN_IE_M  (IO_MUX_GPIO8_FUN_IE_V << IO_MUX_GPIO8_FUN_IE_S)
#define IO_MUX_GPIO8_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO8_FUN_IE_S  9
/** IO_MUX_GPIO8_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO8. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO8_FUN_DRV    0x00000003U
#define IO_MUX_GPIO8_FUN_DRV_M  (IO_MUX_GPIO8_FUN_DRV_V << IO_MUX_GPIO8_FUN_DRV_S)
#define IO_MUX_GPIO8_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO8_FUN_DRV_S  10
/** IO_MUX_GPIO8_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO8_MCU_SEL    0x00000007U
#define IO_MUX_GPIO8_MCU_SEL_M  (IO_MUX_GPIO8_MCU_SEL_V << IO_MUX_GPIO8_MCU_SEL_S)
#define IO_MUX_GPIO8_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO8_MCU_SEL_S  12
/** IO_MUX_GPIO8_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO8_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO8_FILTER_EN_M  (IO_MUX_GPIO8_FILTER_EN_V << IO_MUX_GPIO8_FILTER_EN_S)
#define IO_MUX_GPIO8_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO8_FILTER_EN_S  15
/** IO_MUX_GPIO8_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO8_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO8_HYS_EN    (BIT(16))
#define IO_MUX_GPIO8_HYS_EN_M  (IO_MUX_GPIO8_HYS_EN_V << IO_MUX_GPIO8_HYS_EN_S)
#define IO_MUX_GPIO8_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO8_HYS_EN_S  16
/** IO_MUX_GPIO8_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO8. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO8_HYS_EN\\
 */
#define IO_MUX_GPIO8_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO8_HYS_SEL_M  (IO_MUX_GPIO8_HYS_SEL_V << IO_MUX_GPIO8_HYS_SEL_S)
#define IO_MUX_GPIO8_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO8_HYS_SEL_S  17

/** IO_MUX_GPIO9_REG register
 *  IO MUX configuration register for GPIO9
 */
#define IO_MUX_GPIO9_REG (DR_REG_IO_MUX_BASE + 0x24)
/** IO_MUX_GPIO9_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO9 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO9_MCU_OE    (BIT(0))
#define IO_MUX_GPIO9_MCU_OE_M  (IO_MUX_GPIO9_MCU_OE_V << IO_MUX_GPIO9_MCU_OE_S)
#define IO_MUX_GPIO9_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO9_MCU_OE_S  0
/** IO_MUX_GPIO9_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO9.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO9_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO9_SLP_SEL_M  (IO_MUX_GPIO9_SLP_SEL_V << IO_MUX_GPIO9_SLP_SEL_S)
#define IO_MUX_GPIO9_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO9_SLP_SEL_S  1
/** IO_MUX_GPIO9_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO9 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO9_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO9_MCU_WPD_M  (IO_MUX_GPIO9_MCU_WPD_V << IO_MUX_GPIO9_MCU_WPD_S)
#define IO_MUX_GPIO9_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO9_MCU_WPD_S  2
/** IO_MUX_GPIO9_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO9 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO9_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO9_MCU_WPU_M  (IO_MUX_GPIO9_MCU_WPU_V << IO_MUX_GPIO9_MCU_WPU_S)
#define IO_MUX_GPIO9_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO9_MCU_WPU_S  3
/** IO_MUX_GPIO9_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO9 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO9_MCU_IE    (BIT(4))
#define IO_MUX_GPIO9_MCU_IE_M  (IO_MUX_GPIO9_MCU_IE_V << IO_MUX_GPIO9_MCU_IE_S)
#define IO_MUX_GPIO9_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO9_MCU_IE_S  4
/** IO_MUX_GPIO9_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO9 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO9_MCU_DRV    0x00000003U
#define IO_MUX_GPIO9_MCU_DRV_M  (IO_MUX_GPIO9_MCU_DRV_V << IO_MUX_GPIO9_MCU_DRV_S)
#define IO_MUX_GPIO9_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO9_MCU_DRV_S  5
/** IO_MUX_GPIO9_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO9.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO9_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO9_FUN_WPD_M  (IO_MUX_GPIO9_FUN_WPD_V << IO_MUX_GPIO9_FUN_WPD_S)
#define IO_MUX_GPIO9_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO9_FUN_WPD_S  7
/** IO_MUX_GPIO9_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO9.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO9_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO9_FUN_WPU_M  (IO_MUX_GPIO9_FUN_WPU_V << IO_MUX_GPIO9_FUN_WPU_S)
#define IO_MUX_GPIO9_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO9_FUN_WPU_S  8
/** IO_MUX_GPIO9_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO9.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO9_FUN_IE    (BIT(9))
#define IO_MUX_GPIO9_FUN_IE_M  (IO_MUX_GPIO9_FUN_IE_V << IO_MUX_GPIO9_FUN_IE_S)
#define IO_MUX_GPIO9_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO9_FUN_IE_S  9
/** IO_MUX_GPIO9_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO9. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO9_FUN_DRV    0x00000003U
#define IO_MUX_GPIO9_FUN_DRV_M  (IO_MUX_GPIO9_FUN_DRV_V << IO_MUX_GPIO9_FUN_DRV_S)
#define IO_MUX_GPIO9_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO9_FUN_DRV_S  10
/** IO_MUX_GPIO9_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO9_MCU_SEL    0x00000007U
#define IO_MUX_GPIO9_MCU_SEL_M  (IO_MUX_GPIO9_MCU_SEL_V << IO_MUX_GPIO9_MCU_SEL_S)
#define IO_MUX_GPIO9_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO9_MCU_SEL_S  12
/** IO_MUX_GPIO9_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO9_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO9_FILTER_EN_M  (IO_MUX_GPIO9_FILTER_EN_V << IO_MUX_GPIO9_FILTER_EN_S)
#define IO_MUX_GPIO9_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO9_FILTER_EN_S  15
/** IO_MUX_GPIO9_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO9_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO9_HYS_EN    (BIT(16))
#define IO_MUX_GPIO9_HYS_EN_M  (IO_MUX_GPIO9_HYS_EN_V << IO_MUX_GPIO9_HYS_EN_S)
#define IO_MUX_GPIO9_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO9_HYS_EN_S  16
/** IO_MUX_GPIO9_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO9. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO9_HYS_EN\\
 */
#define IO_MUX_GPIO9_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO9_HYS_SEL_M  (IO_MUX_GPIO9_HYS_SEL_V << IO_MUX_GPIO9_HYS_SEL_S)
#define IO_MUX_GPIO9_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO9_HYS_SEL_S  17

/** IO_MUX_GPIO10_REG register
 *  IO MUX configuration register for GPIO10
 */
#define IO_MUX_GPIO10_REG (DR_REG_IO_MUX_BASE + 0x28)
/** IO_MUX_GPIO10_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO10 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO10_MCU_OE    (BIT(0))
#define IO_MUX_GPIO10_MCU_OE_M  (IO_MUX_GPIO10_MCU_OE_V << IO_MUX_GPIO10_MCU_OE_S)
#define IO_MUX_GPIO10_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO10_MCU_OE_S  0
/** IO_MUX_GPIO10_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO10.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO10_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO10_SLP_SEL_M  (IO_MUX_GPIO10_SLP_SEL_V << IO_MUX_GPIO10_SLP_SEL_S)
#define IO_MUX_GPIO10_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO10_SLP_SEL_S  1
/** IO_MUX_GPIO10_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO10 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO10_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO10_MCU_WPD_M  (IO_MUX_GPIO10_MCU_WPD_V << IO_MUX_GPIO10_MCU_WPD_S)
#define IO_MUX_GPIO10_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO10_MCU_WPD_S  2
/** IO_MUX_GPIO10_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO10 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO10_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO10_MCU_WPU_M  (IO_MUX_GPIO10_MCU_WPU_V << IO_MUX_GPIO10_MCU_WPU_S)
#define IO_MUX_GPIO10_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO10_MCU_WPU_S  3
/** IO_MUX_GPIO10_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO10 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO10_MCU_IE    (BIT(4))
#define IO_MUX_GPIO10_MCU_IE_M  (IO_MUX_GPIO10_MCU_IE_V << IO_MUX_GPIO10_MCU_IE_S)
#define IO_MUX_GPIO10_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO10_MCU_IE_S  4
/** IO_MUX_GPIO10_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO10 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO10_MCU_DRV    0x00000003U
#define IO_MUX_GPIO10_MCU_DRV_M  (IO_MUX_GPIO10_MCU_DRV_V << IO_MUX_GPIO10_MCU_DRV_S)
#define IO_MUX_GPIO10_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO10_MCU_DRV_S  5
/** IO_MUX_GPIO10_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO10.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO10_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO10_FUN_WPD_M  (IO_MUX_GPIO10_FUN_WPD_V << IO_MUX_GPIO10_FUN_WPD_S)
#define IO_MUX_GPIO10_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO10_FUN_WPD_S  7
/** IO_MUX_GPIO10_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO10.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO10_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO10_FUN_WPU_M  (IO_MUX_GPIO10_FUN_WPU_V << IO_MUX_GPIO10_FUN_WPU_S)
#define IO_MUX_GPIO10_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO10_FUN_WPU_S  8
/** IO_MUX_GPIO10_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO10.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO10_FUN_IE    (BIT(9))
#define IO_MUX_GPIO10_FUN_IE_M  (IO_MUX_GPIO10_FUN_IE_V << IO_MUX_GPIO10_FUN_IE_S)
#define IO_MUX_GPIO10_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO10_FUN_IE_S  9
/** IO_MUX_GPIO10_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO10. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO10_FUN_DRV    0x00000003U
#define IO_MUX_GPIO10_FUN_DRV_M  (IO_MUX_GPIO10_FUN_DRV_V << IO_MUX_GPIO10_FUN_DRV_S)
#define IO_MUX_GPIO10_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO10_FUN_DRV_S  10
/** IO_MUX_GPIO10_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO10_MCU_SEL    0x00000007U
#define IO_MUX_GPIO10_MCU_SEL_M  (IO_MUX_GPIO10_MCU_SEL_V << IO_MUX_GPIO10_MCU_SEL_S)
#define IO_MUX_GPIO10_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO10_MCU_SEL_S  12
/** IO_MUX_GPIO10_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO10_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO10_FILTER_EN_M  (IO_MUX_GPIO10_FILTER_EN_V << IO_MUX_GPIO10_FILTER_EN_S)
#define IO_MUX_GPIO10_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO10_FILTER_EN_S  15
/** IO_MUX_GPIO10_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO10_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO10_HYS_EN    (BIT(16))
#define IO_MUX_GPIO10_HYS_EN_M  (IO_MUX_GPIO10_HYS_EN_V << IO_MUX_GPIO10_HYS_EN_S)
#define IO_MUX_GPIO10_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO10_HYS_EN_S  16
/** IO_MUX_GPIO10_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO10. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO10_HYS_EN\\
 */
#define IO_MUX_GPIO10_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO10_HYS_SEL_M  (IO_MUX_GPIO10_HYS_SEL_V << IO_MUX_GPIO10_HYS_SEL_S)
#define IO_MUX_GPIO10_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO10_HYS_SEL_S  17

/** IO_MUX_GPIO11_REG register
 *  IO MUX configuration register for GPIO11
 */
#define IO_MUX_GPIO11_REG (DR_REG_IO_MUX_BASE + 0x2c)
/** IO_MUX_GPIO11_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO11 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO11_MCU_OE    (BIT(0))
#define IO_MUX_GPIO11_MCU_OE_M  (IO_MUX_GPIO11_MCU_OE_V << IO_MUX_GPIO11_MCU_OE_S)
#define IO_MUX_GPIO11_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO11_MCU_OE_S  0
/** IO_MUX_GPIO11_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO11.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO11_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO11_SLP_SEL_M  (IO_MUX_GPIO11_SLP_SEL_V << IO_MUX_GPIO11_SLP_SEL_S)
#define IO_MUX_GPIO11_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO11_SLP_SEL_S  1
/** IO_MUX_GPIO11_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO11 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO11_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO11_MCU_WPD_M  (IO_MUX_GPIO11_MCU_WPD_V << IO_MUX_GPIO11_MCU_WPD_S)
#define IO_MUX_GPIO11_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO11_MCU_WPD_S  2
/** IO_MUX_GPIO11_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO11 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO11_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO11_MCU_WPU_M  (IO_MUX_GPIO11_MCU_WPU_V << IO_MUX_GPIO11_MCU_WPU_S)
#define IO_MUX_GPIO11_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO11_MCU_WPU_S  3
/** IO_MUX_GPIO11_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO11 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO11_MCU_IE    (BIT(4))
#define IO_MUX_GPIO11_MCU_IE_M  (IO_MUX_GPIO11_MCU_IE_V << IO_MUX_GPIO11_MCU_IE_S)
#define IO_MUX_GPIO11_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO11_MCU_IE_S  4
/** IO_MUX_GPIO11_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO11 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO11_MCU_DRV    0x00000003U
#define IO_MUX_GPIO11_MCU_DRV_M  (IO_MUX_GPIO11_MCU_DRV_V << IO_MUX_GPIO11_MCU_DRV_S)
#define IO_MUX_GPIO11_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO11_MCU_DRV_S  5
/** IO_MUX_GPIO11_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO11.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO11_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO11_FUN_WPD_M  (IO_MUX_GPIO11_FUN_WPD_V << IO_MUX_GPIO11_FUN_WPD_S)
#define IO_MUX_GPIO11_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO11_FUN_WPD_S  7
/** IO_MUX_GPIO11_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO11.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO11_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO11_FUN_WPU_M  (IO_MUX_GPIO11_FUN_WPU_V << IO_MUX_GPIO11_FUN_WPU_S)
#define IO_MUX_GPIO11_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO11_FUN_WPU_S  8
/** IO_MUX_GPIO11_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO11.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO11_FUN_IE    (BIT(9))
#define IO_MUX_GPIO11_FUN_IE_M  (IO_MUX_GPIO11_FUN_IE_V << IO_MUX_GPIO11_FUN_IE_S)
#define IO_MUX_GPIO11_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO11_FUN_IE_S  9
/** IO_MUX_GPIO11_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO11. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO11_FUN_DRV    0x00000003U
#define IO_MUX_GPIO11_FUN_DRV_M  (IO_MUX_GPIO11_FUN_DRV_V << IO_MUX_GPIO11_FUN_DRV_S)
#define IO_MUX_GPIO11_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO11_FUN_DRV_S  10
/** IO_MUX_GPIO11_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO11_MCU_SEL    0x00000007U
#define IO_MUX_GPIO11_MCU_SEL_M  (IO_MUX_GPIO11_MCU_SEL_V << IO_MUX_GPIO11_MCU_SEL_S)
#define IO_MUX_GPIO11_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO11_MCU_SEL_S  12
/** IO_MUX_GPIO11_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO11_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO11_FILTER_EN_M  (IO_MUX_GPIO11_FILTER_EN_V << IO_MUX_GPIO11_FILTER_EN_S)
#define IO_MUX_GPIO11_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO11_FILTER_EN_S  15
/** IO_MUX_GPIO11_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO11_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO11_HYS_EN    (BIT(16))
#define IO_MUX_GPIO11_HYS_EN_M  (IO_MUX_GPIO11_HYS_EN_V << IO_MUX_GPIO11_HYS_EN_S)
#define IO_MUX_GPIO11_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO11_HYS_EN_S  16
/** IO_MUX_GPIO11_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO11. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO11_HYS_EN\\
 */
#define IO_MUX_GPIO11_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO11_HYS_SEL_M  (IO_MUX_GPIO11_HYS_SEL_V << IO_MUX_GPIO11_HYS_SEL_S)
#define IO_MUX_GPIO11_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO11_HYS_SEL_S  17

/** IO_MUX_GPIO12_REG register
 *  IO MUX configuration register for GPIO12
 */
#define IO_MUX_GPIO12_REG (DR_REG_IO_MUX_BASE + 0x30)
/** IO_MUX_GPIO12_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO12 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO12_MCU_OE    (BIT(0))
#define IO_MUX_GPIO12_MCU_OE_M  (IO_MUX_GPIO12_MCU_OE_V << IO_MUX_GPIO12_MCU_OE_S)
#define IO_MUX_GPIO12_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO12_MCU_OE_S  0
/** IO_MUX_GPIO12_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO12.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO12_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO12_SLP_SEL_M  (IO_MUX_GPIO12_SLP_SEL_V << IO_MUX_GPIO12_SLP_SEL_S)
#define IO_MUX_GPIO12_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO12_SLP_SEL_S  1
/** IO_MUX_GPIO12_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO12 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO12_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO12_MCU_WPD_M  (IO_MUX_GPIO12_MCU_WPD_V << IO_MUX_GPIO12_MCU_WPD_S)
#define IO_MUX_GPIO12_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO12_MCU_WPD_S  2
/** IO_MUX_GPIO12_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO12 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO12_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO12_MCU_WPU_M  (IO_MUX_GPIO12_MCU_WPU_V << IO_MUX_GPIO12_MCU_WPU_S)
#define IO_MUX_GPIO12_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO12_MCU_WPU_S  3
/** IO_MUX_GPIO12_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO12 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO12_MCU_IE    (BIT(4))
#define IO_MUX_GPIO12_MCU_IE_M  (IO_MUX_GPIO12_MCU_IE_V << IO_MUX_GPIO12_MCU_IE_S)
#define IO_MUX_GPIO12_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO12_MCU_IE_S  4
/** IO_MUX_GPIO12_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO12 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO12_MCU_DRV    0x00000003U
#define IO_MUX_GPIO12_MCU_DRV_M  (IO_MUX_GPIO12_MCU_DRV_V << IO_MUX_GPIO12_MCU_DRV_S)
#define IO_MUX_GPIO12_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO12_MCU_DRV_S  5
/** IO_MUX_GPIO12_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO12.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO12_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO12_FUN_WPD_M  (IO_MUX_GPIO12_FUN_WPD_V << IO_MUX_GPIO12_FUN_WPD_S)
#define IO_MUX_GPIO12_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO12_FUN_WPD_S  7
/** IO_MUX_GPIO12_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO12.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO12_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO12_FUN_WPU_M  (IO_MUX_GPIO12_FUN_WPU_V << IO_MUX_GPIO12_FUN_WPU_S)
#define IO_MUX_GPIO12_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO12_FUN_WPU_S  8
/** IO_MUX_GPIO12_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO12.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO12_FUN_IE    (BIT(9))
#define IO_MUX_GPIO12_FUN_IE_M  (IO_MUX_GPIO12_FUN_IE_V << IO_MUX_GPIO12_FUN_IE_S)
#define IO_MUX_GPIO12_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO12_FUN_IE_S  9
/** IO_MUX_GPIO12_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO12. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO12_FUN_DRV    0x00000003U
#define IO_MUX_GPIO12_FUN_DRV_M  (IO_MUX_GPIO12_FUN_DRV_V << IO_MUX_GPIO12_FUN_DRV_S)
#define IO_MUX_GPIO12_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO12_FUN_DRV_S  10
/** IO_MUX_GPIO12_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO12_MCU_SEL    0x00000007U
#define IO_MUX_GPIO12_MCU_SEL_M  (IO_MUX_GPIO12_MCU_SEL_V << IO_MUX_GPIO12_MCU_SEL_S)
#define IO_MUX_GPIO12_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO12_MCU_SEL_S  12
/** IO_MUX_GPIO12_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO12_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO12_FILTER_EN_M  (IO_MUX_GPIO12_FILTER_EN_V << IO_MUX_GPIO12_FILTER_EN_S)
#define IO_MUX_GPIO12_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO12_FILTER_EN_S  15
/** IO_MUX_GPIO12_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO12_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO12_HYS_EN    (BIT(16))
#define IO_MUX_GPIO12_HYS_EN_M  (IO_MUX_GPIO12_HYS_EN_V << IO_MUX_GPIO12_HYS_EN_S)
#define IO_MUX_GPIO12_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO12_HYS_EN_S  16
/** IO_MUX_GPIO12_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO12. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO12_HYS_EN\\
 */
#define IO_MUX_GPIO12_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO12_HYS_SEL_M  (IO_MUX_GPIO12_HYS_SEL_V << IO_MUX_GPIO12_HYS_SEL_S)
#define IO_MUX_GPIO12_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO12_HYS_SEL_S  17

/** IO_MUX_GPIO13_REG register
 *  IO MUX configuration register for GPIO13
 */
#define IO_MUX_GPIO13_REG (DR_REG_IO_MUX_BASE + 0x34)
/** IO_MUX_GPIO13_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO13 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO13_MCU_OE    (BIT(0))
#define IO_MUX_GPIO13_MCU_OE_M  (IO_MUX_GPIO13_MCU_OE_V << IO_MUX_GPIO13_MCU_OE_S)
#define IO_MUX_GPIO13_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO13_MCU_OE_S  0
/** IO_MUX_GPIO13_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO13.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO13_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO13_SLP_SEL_M  (IO_MUX_GPIO13_SLP_SEL_V << IO_MUX_GPIO13_SLP_SEL_S)
#define IO_MUX_GPIO13_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO13_SLP_SEL_S  1
/** IO_MUX_GPIO13_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO13 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO13_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO13_MCU_WPD_M  (IO_MUX_GPIO13_MCU_WPD_V << IO_MUX_GPIO13_MCU_WPD_S)
#define IO_MUX_GPIO13_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO13_MCU_WPD_S  2
/** IO_MUX_GPIO13_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO13 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO13_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO13_MCU_WPU_M  (IO_MUX_GPIO13_MCU_WPU_V << IO_MUX_GPIO13_MCU_WPU_S)
#define IO_MUX_GPIO13_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO13_MCU_WPU_S  3
/** IO_MUX_GPIO13_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO13 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO13_MCU_IE    (BIT(4))
#define IO_MUX_GPIO13_MCU_IE_M  (IO_MUX_GPIO13_MCU_IE_V << IO_MUX_GPIO13_MCU_IE_S)
#define IO_MUX_GPIO13_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO13_MCU_IE_S  4
/** IO_MUX_GPIO13_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO13 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO13_MCU_DRV    0x00000003U
#define IO_MUX_GPIO13_MCU_DRV_M  (IO_MUX_GPIO13_MCU_DRV_V << IO_MUX_GPIO13_MCU_DRV_S)
#define IO_MUX_GPIO13_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO13_MCU_DRV_S  5
/** IO_MUX_GPIO13_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO13.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO13_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO13_FUN_WPD_M  (IO_MUX_GPIO13_FUN_WPD_V << IO_MUX_GPIO13_FUN_WPD_S)
#define IO_MUX_GPIO13_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO13_FUN_WPD_S  7
/** IO_MUX_GPIO13_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO13.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO13_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO13_FUN_WPU_M  (IO_MUX_GPIO13_FUN_WPU_V << IO_MUX_GPIO13_FUN_WPU_S)
#define IO_MUX_GPIO13_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO13_FUN_WPU_S  8
/** IO_MUX_GPIO13_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO13.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO13_FUN_IE    (BIT(9))
#define IO_MUX_GPIO13_FUN_IE_M  (IO_MUX_GPIO13_FUN_IE_V << IO_MUX_GPIO13_FUN_IE_S)
#define IO_MUX_GPIO13_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO13_FUN_IE_S  9
/** IO_MUX_GPIO13_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO13. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO13_FUN_DRV    0x00000003U
#define IO_MUX_GPIO13_FUN_DRV_M  (IO_MUX_GPIO13_FUN_DRV_V << IO_MUX_GPIO13_FUN_DRV_S)
#define IO_MUX_GPIO13_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO13_FUN_DRV_S  10
/** IO_MUX_GPIO13_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO13_MCU_SEL    0x00000007U
#define IO_MUX_GPIO13_MCU_SEL_M  (IO_MUX_GPIO13_MCU_SEL_V << IO_MUX_GPIO13_MCU_SEL_S)
#define IO_MUX_GPIO13_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO13_MCU_SEL_S  12
/** IO_MUX_GPIO13_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO13_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO13_FILTER_EN_M  (IO_MUX_GPIO13_FILTER_EN_V << IO_MUX_GPIO13_FILTER_EN_S)
#define IO_MUX_GPIO13_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO13_FILTER_EN_S  15
/** IO_MUX_GPIO13_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO13_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO13_HYS_EN    (BIT(16))
#define IO_MUX_GPIO13_HYS_EN_M  (IO_MUX_GPIO13_HYS_EN_V << IO_MUX_GPIO13_HYS_EN_S)
#define IO_MUX_GPIO13_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO13_HYS_EN_S  16
/** IO_MUX_GPIO13_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO13. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO13_HYS_EN\\
 */
#define IO_MUX_GPIO13_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO13_HYS_SEL_M  (IO_MUX_GPIO13_HYS_SEL_V << IO_MUX_GPIO13_HYS_SEL_S)
#define IO_MUX_GPIO13_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO13_HYS_SEL_S  17

/** IO_MUX_GPIO14_REG register
 *  IO MUX configuration register for GPIO14
 */
#define IO_MUX_GPIO14_REG (DR_REG_IO_MUX_BASE + 0x38)
/** IO_MUX_GPIO14_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO14 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO14_MCU_OE    (BIT(0))
#define IO_MUX_GPIO14_MCU_OE_M  (IO_MUX_GPIO14_MCU_OE_V << IO_MUX_GPIO14_MCU_OE_S)
#define IO_MUX_GPIO14_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO14_MCU_OE_S  0
/** IO_MUX_GPIO14_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO14.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO14_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO14_SLP_SEL_M  (IO_MUX_GPIO14_SLP_SEL_V << IO_MUX_GPIO14_SLP_SEL_S)
#define IO_MUX_GPIO14_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO14_SLP_SEL_S  1
/** IO_MUX_GPIO14_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO14 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO14_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO14_MCU_WPD_M  (IO_MUX_GPIO14_MCU_WPD_V << IO_MUX_GPIO14_MCU_WPD_S)
#define IO_MUX_GPIO14_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO14_MCU_WPD_S  2
/** IO_MUX_GPIO14_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO14 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO14_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO14_MCU_WPU_M  (IO_MUX_GPIO14_MCU_WPU_V << IO_MUX_GPIO14_MCU_WPU_S)
#define IO_MUX_GPIO14_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO14_MCU_WPU_S  3
/** IO_MUX_GPIO14_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO14 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO14_MCU_IE    (BIT(4))
#define IO_MUX_GPIO14_MCU_IE_M  (IO_MUX_GPIO14_MCU_IE_V << IO_MUX_GPIO14_MCU_IE_S)
#define IO_MUX_GPIO14_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO14_MCU_IE_S  4
/** IO_MUX_GPIO14_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO14 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO14_MCU_DRV    0x00000003U
#define IO_MUX_GPIO14_MCU_DRV_M  (IO_MUX_GPIO14_MCU_DRV_V << IO_MUX_GPIO14_MCU_DRV_S)
#define IO_MUX_GPIO14_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO14_MCU_DRV_S  5
/** IO_MUX_GPIO14_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO14.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO14_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO14_FUN_WPD_M  (IO_MUX_GPIO14_FUN_WPD_V << IO_MUX_GPIO14_FUN_WPD_S)
#define IO_MUX_GPIO14_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO14_FUN_WPD_S  7
/** IO_MUX_GPIO14_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO14.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO14_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO14_FUN_WPU_M  (IO_MUX_GPIO14_FUN_WPU_V << IO_MUX_GPIO14_FUN_WPU_S)
#define IO_MUX_GPIO14_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO14_FUN_WPU_S  8
/** IO_MUX_GPIO14_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO14.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO14_FUN_IE    (BIT(9))
#define IO_MUX_GPIO14_FUN_IE_M  (IO_MUX_GPIO14_FUN_IE_V << IO_MUX_GPIO14_FUN_IE_S)
#define IO_MUX_GPIO14_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO14_FUN_IE_S  9
/** IO_MUX_GPIO14_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO14. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO14_FUN_DRV    0x00000003U
#define IO_MUX_GPIO14_FUN_DRV_M  (IO_MUX_GPIO14_FUN_DRV_V << IO_MUX_GPIO14_FUN_DRV_S)
#define IO_MUX_GPIO14_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO14_FUN_DRV_S  10
/** IO_MUX_GPIO14_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO14_MCU_SEL    0x00000007U
#define IO_MUX_GPIO14_MCU_SEL_M  (IO_MUX_GPIO14_MCU_SEL_V << IO_MUX_GPIO14_MCU_SEL_S)
#define IO_MUX_GPIO14_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO14_MCU_SEL_S  12
/** IO_MUX_GPIO14_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO14_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO14_FILTER_EN_M  (IO_MUX_GPIO14_FILTER_EN_V << IO_MUX_GPIO14_FILTER_EN_S)
#define IO_MUX_GPIO14_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO14_FILTER_EN_S  15
/** IO_MUX_GPIO14_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO14_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO14_HYS_EN    (BIT(16))
#define IO_MUX_GPIO14_HYS_EN_M  (IO_MUX_GPIO14_HYS_EN_V << IO_MUX_GPIO14_HYS_EN_S)
#define IO_MUX_GPIO14_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO14_HYS_EN_S  16
/** IO_MUX_GPIO14_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO14. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO14_HYS_EN\\
 */
#define IO_MUX_GPIO14_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO14_HYS_SEL_M  (IO_MUX_GPIO14_HYS_SEL_V << IO_MUX_GPIO14_HYS_SEL_S)
#define IO_MUX_GPIO14_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO14_HYS_SEL_S  17

/** IO_MUX_GPIO15_REG register
 *  IO MUX configuration register for GPIO15
 */
#define IO_MUX_GPIO15_REG (DR_REG_IO_MUX_BASE + 0x3c)
/** IO_MUX_GPIO15_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO15 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO15_MCU_OE    (BIT(0))
#define IO_MUX_GPIO15_MCU_OE_M  (IO_MUX_GPIO15_MCU_OE_V << IO_MUX_GPIO15_MCU_OE_S)
#define IO_MUX_GPIO15_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO15_MCU_OE_S  0
/** IO_MUX_GPIO15_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO15.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO15_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO15_SLP_SEL_M  (IO_MUX_GPIO15_SLP_SEL_V << IO_MUX_GPIO15_SLP_SEL_S)
#define IO_MUX_GPIO15_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO15_SLP_SEL_S  1
/** IO_MUX_GPIO15_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO15 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO15_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO15_MCU_WPD_M  (IO_MUX_GPIO15_MCU_WPD_V << IO_MUX_GPIO15_MCU_WPD_S)
#define IO_MUX_GPIO15_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO15_MCU_WPD_S  2
/** IO_MUX_GPIO15_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO15 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO15_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO15_MCU_WPU_M  (IO_MUX_GPIO15_MCU_WPU_V << IO_MUX_GPIO15_MCU_WPU_S)
#define IO_MUX_GPIO15_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO15_MCU_WPU_S  3
/** IO_MUX_GPIO15_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO15 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO15_MCU_IE    (BIT(4))
#define IO_MUX_GPIO15_MCU_IE_M  (IO_MUX_GPIO15_MCU_IE_V << IO_MUX_GPIO15_MCU_IE_S)
#define IO_MUX_GPIO15_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO15_MCU_IE_S  4
/** IO_MUX_GPIO15_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO15 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO15_MCU_DRV    0x00000003U
#define IO_MUX_GPIO15_MCU_DRV_M  (IO_MUX_GPIO15_MCU_DRV_V << IO_MUX_GPIO15_MCU_DRV_S)
#define IO_MUX_GPIO15_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO15_MCU_DRV_S  5
/** IO_MUX_GPIO15_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO15.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO15_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO15_FUN_WPD_M  (IO_MUX_GPIO15_FUN_WPD_V << IO_MUX_GPIO15_FUN_WPD_S)
#define IO_MUX_GPIO15_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO15_FUN_WPD_S  7
/** IO_MUX_GPIO15_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO15.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO15_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO15_FUN_WPU_M  (IO_MUX_GPIO15_FUN_WPU_V << IO_MUX_GPIO15_FUN_WPU_S)
#define IO_MUX_GPIO15_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO15_FUN_WPU_S  8
/** IO_MUX_GPIO15_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO15.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO15_FUN_IE    (BIT(9))
#define IO_MUX_GPIO15_FUN_IE_M  (IO_MUX_GPIO15_FUN_IE_V << IO_MUX_GPIO15_FUN_IE_S)
#define IO_MUX_GPIO15_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO15_FUN_IE_S  9
/** IO_MUX_GPIO15_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO15. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO15_FUN_DRV    0x00000003U
#define IO_MUX_GPIO15_FUN_DRV_M  (IO_MUX_GPIO15_FUN_DRV_V << IO_MUX_GPIO15_FUN_DRV_S)
#define IO_MUX_GPIO15_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO15_FUN_DRV_S  10
/** IO_MUX_GPIO15_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO15_MCU_SEL    0x00000007U
#define IO_MUX_GPIO15_MCU_SEL_M  (IO_MUX_GPIO15_MCU_SEL_V << IO_MUX_GPIO15_MCU_SEL_S)
#define IO_MUX_GPIO15_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO15_MCU_SEL_S  12
/** IO_MUX_GPIO15_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO15_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO15_FILTER_EN_M  (IO_MUX_GPIO15_FILTER_EN_V << IO_MUX_GPIO15_FILTER_EN_S)
#define IO_MUX_GPIO15_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO15_FILTER_EN_S  15
/** IO_MUX_GPIO15_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO15_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO15_HYS_EN    (BIT(16))
#define IO_MUX_GPIO15_HYS_EN_M  (IO_MUX_GPIO15_HYS_EN_V << IO_MUX_GPIO15_HYS_EN_S)
#define IO_MUX_GPIO15_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO15_HYS_EN_S  16
/** IO_MUX_GPIO15_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO15. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO15_HYS_EN\\
 */
#define IO_MUX_GPIO15_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO15_HYS_SEL_M  (IO_MUX_GPIO15_HYS_SEL_V << IO_MUX_GPIO15_HYS_SEL_S)
#define IO_MUX_GPIO15_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO15_HYS_SEL_S  17

/** IO_MUX_GPIO16_REG register
 *  IO MUX configuration register for GPIO16
 */
#define IO_MUX_GPIO16_REG (DR_REG_IO_MUX_BASE + 0x40)
/** IO_MUX_GPIO16_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO16 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO16_MCU_OE    (BIT(0))
#define IO_MUX_GPIO16_MCU_OE_M  (IO_MUX_GPIO16_MCU_OE_V << IO_MUX_GPIO16_MCU_OE_S)
#define IO_MUX_GPIO16_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO16_MCU_OE_S  0
/** IO_MUX_GPIO16_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO16.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO16_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO16_SLP_SEL_M  (IO_MUX_GPIO16_SLP_SEL_V << IO_MUX_GPIO16_SLP_SEL_S)
#define IO_MUX_GPIO16_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO16_SLP_SEL_S  1
/** IO_MUX_GPIO16_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO16 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO16_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO16_MCU_WPD_M  (IO_MUX_GPIO16_MCU_WPD_V << IO_MUX_GPIO16_MCU_WPD_S)
#define IO_MUX_GPIO16_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO16_MCU_WPD_S  2
/** IO_MUX_GPIO16_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO16 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO16_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO16_MCU_WPU_M  (IO_MUX_GPIO16_MCU_WPU_V << IO_MUX_GPIO16_MCU_WPU_S)
#define IO_MUX_GPIO16_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO16_MCU_WPU_S  3
/** IO_MUX_GPIO16_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO16 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO16_MCU_IE    (BIT(4))
#define IO_MUX_GPIO16_MCU_IE_M  (IO_MUX_GPIO16_MCU_IE_V << IO_MUX_GPIO16_MCU_IE_S)
#define IO_MUX_GPIO16_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO16_MCU_IE_S  4
/** IO_MUX_GPIO16_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO16 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO16_MCU_DRV    0x00000003U
#define IO_MUX_GPIO16_MCU_DRV_M  (IO_MUX_GPIO16_MCU_DRV_V << IO_MUX_GPIO16_MCU_DRV_S)
#define IO_MUX_GPIO16_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO16_MCU_DRV_S  5
/** IO_MUX_GPIO16_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO16.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO16_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO16_FUN_WPD_M  (IO_MUX_GPIO16_FUN_WPD_V << IO_MUX_GPIO16_FUN_WPD_S)
#define IO_MUX_GPIO16_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO16_FUN_WPD_S  7
/** IO_MUX_GPIO16_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO16.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO16_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO16_FUN_WPU_M  (IO_MUX_GPIO16_FUN_WPU_V << IO_MUX_GPIO16_FUN_WPU_S)
#define IO_MUX_GPIO16_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO16_FUN_WPU_S  8
/** IO_MUX_GPIO16_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO16.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO16_FUN_IE    (BIT(9))
#define IO_MUX_GPIO16_FUN_IE_M  (IO_MUX_GPIO16_FUN_IE_V << IO_MUX_GPIO16_FUN_IE_S)
#define IO_MUX_GPIO16_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO16_FUN_IE_S  9
/** IO_MUX_GPIO16_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO16. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO16_FUN_DRV    0x00000003U
#define IO_MUX_GPIO16_FUN_DRV_M  (IO_MUX_GPIO16_FUN_DRV_V << IO_MUX_GPIO16_FUN_DRV_S)
#define IO_MUX_GPIO16_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO16_FUN_DRV_S  10
/** IO_MUX_GPIO16_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO16_MCU_SEL    0x00000007U
#define IO_MUX_GPIO16_MCU_SEL_M  (IO_MUX_GPIO16_MCU_SEL_V << IO_MUX_GPIO16_MCU_SEL_S)
#define IO_MUX_GPIO16_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO16_MCU_SEL_S  12
/** IO_MUX_GPIO16_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO16_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO16_FILTER_EN_M  (IO_MUX_GPIO16_FILTER_EN_V << IO_MUX_GPIO16_FILTER_EN_S)
#define IO_MUX_GPIO16_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO16_FILTER_EN_S  15
/** IO_MUX_GPIO16_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO16_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO16_HYS_EN    (BIT(16))
#define IO_MUX_GPIO16_HYS_EN_M  (IO_MUX_GPIO16_HYS_EN_V << IO_MUX_GPIO16_HYS_EN_S)
#define IO_MUX_GPIO16_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO16_HYS_EN_S  16
/** IO_MUX_GPIO16_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO16. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO16_HYS_EN\\
 */
#define IO_MUX_GPIO16_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO16_HYS_SEL_M  (IO_MUX_GPIO16_HYS_SEL_V << IO_MUX_GPIO16_HYS_SEL_S)
#define IO_MUX_GPIO16_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO16_HYS_SEL_S  17

/** IO_MUX_GPIO17_REG register
 *  IO MUX configuration register for GPIO17
 */
#define IO_MUX_GPIO17_REG (DR_REG_IO_MUX_BASE + 0x44)
/** IO_MUX_GPIO17_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO17 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO17_MCU_OE    (BIT(0))
#define IO_MUX_GPIO17_MCU_OE_M  (IO_MUX_GPIO17_MCU_OE_V << IO_MUX_GPIO17_MCU_OE_S)
#define IO_MUX_GPIO17_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO17_MCU_OE_S  0
/** IO_MUX_GPIO17_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO17.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO17_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO17_SLP_SEL_M  (IO_MUX_GPIO17_SLP_SEL_V << IO_MUX_GPIO17_SLP_SEL_S)
#define IO_MUX_GPIO17_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO17_SLP_SEL_S  1
/** IO_MUX_GPIO17_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO17 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO17_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO17_MCU_WPD_M  (IO_MUX_GPIO17_MCU_WPD_V << IO_MUX_GPIO17_MCU_WPD_S)
#define IO_MUX_GPIO17_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO17_MCU_WPD_S  2
/** IO_MUX_GPIO17_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO17 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO17_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO17_MCU_WPU_M  (IO_MUX_GPIO17_MCU_WPU_V << IO_MUX_GPIO17_MCU_WPU_S)
#define IO_MUX_GPIO17_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO17_MCU_WPU_S  3
/** IO_MUX_GPIO17_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO17 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO17_MCU_IE    (BIT(4))
#define IO_MUX_GPIO17_MCU_IE_M  (IO_MUX_GPIO17_MCU_IE_V << IO_MUX_GPIO17_MCU_IE_S)
#define IO_MUX_GPIO17_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO17_MCU_IE_S  4
/** IO_MUX_GPIO17_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO17 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO17_MCU_DRV    0x00000003U
#define IO_MUX_GPIO17_MCU_DRV_M  (IO_MUX_GPIO17_MCU_DRV_V << IO_MUX_GPIO17_MCU_DRV_S)
#define IO_MUX_GPIO17_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO17_MCU_DRV_S  5
/** IO_MUX_GPIO17_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO17.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO17_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO17_FUN_WPD_M  (IO_MUX_GPIO17_FUN_WPD_V << IO_MUX_GPIO17_FUN_WPD_S)
#define IO_MUX_GPIO17_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO17_FUN_WPD_S  7
/** IO_MUX_GPIO17_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO17.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO17_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO17_FUN_WPU_M  (IO_MUX_GPIO17_FUN_WPU_V << IO_MUX_GPIO17_FUN_WPU_S)
#define IO_MUX_GPIO17_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO17_FUN_WPU_S  8
/** IO_MUX_GPIO17_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO17.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO17_FUN_IE    (BIT(9))
#define IO_MUX_GPIO17_FUN_IE_M  (IO_MUX_GPIO17_FUN_IE_V << IO_MUX_GPIO17_FUN_IE_S)
#define IO_MUX_GPIO17_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO17_FUN_IE_S  9
/** IO_MUX_GPIO17_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO17. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO17_FUN_DRV    0x00000003U
#define IO_MUX_GPIO17_FUN_DRV_M  (IO_MUX_GPIO17_FUN_DRV_V << IO_MUX_GPIO17_FUN_DRV_S)
#define IO_MUX_GPIO17_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO17_FUN_DRV_S  10
/** IO_MUX_GPIO17_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO17_MCU_SEL    0x00000007U
#define IO_MUX_GPIO17_MCU_SEL_M  (IO_MUX_GPIO17_MCU_SEL_V << IO_MUX_GPIO17_MCU_SEL_S)
#define IO_MUX_GPIO17_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO17_MCU_SEL_S  12
/** IO_MUX_GPIO17_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO17_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO17_FILTER_EN_M  (IO_MUX_GPIO17_FILTER_EN_V << IO_MUX_GPIO17_FILTER_EN_S)
#define IO_MUX_GPIO17_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO17_FILTER_EN_S  15
/** IO_MUX_GPIO17_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO17_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO17_HYS_EN    (BIT(16))
#define IO_MUX_GPIO17_HYS_EN_M  (IO_MUX_GPIO17_HYS_EN_V << IO_MUX_GPIO17_HYS_EN_S)
#define IO_MUX_GPIO17_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO17_HYS_EN_S  16
/** IO_MUX_GPIO17_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO17. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO17_HYS_EN\\
 */
#define IO_MUX_GPIO17_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO17_HYS_SEL_M  (IO_MUX_GPIO17_HYS_SEL_V << IO_MUX_GPIO17_HYS_SEL_S)
#define IO_MUX_GPIO17_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO17_HYS_SEL_S  17

/** IO_MUX_GPIO18_REG register
 *  IO MUX configuration register for GPIO18
 */
#define IO_MUX_GPIO18_REG (DR_REG_IO_MUX_BASE + 0x48)
/** IO_MUX_GPIO18_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO18 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO18_MCU_OE    (BIT(0))
#define IO_MUX_GPIO18_MCU_OE_M  (IO_MUX_GPIO18_MCU_OE_V << IO_MUX_GPIO18_MCU_OE_S)
#define IO_MUX_GPIO18_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO18_MCU_OE_S  0
/** IO_MUX_GPIO18_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO18.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO18_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO18_SLP_SEL_M  (IO_MUX_GPIO18_SLP_SEL_V << IO_MUX_GPIO18_SLP_SEL_S)
#define IO_MUX_GPIO18_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO18_SLP_SEL_S  1
/** IO_MUX_GPIO18_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO18 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO18_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO18_MCU_WPD_M  (IO_MUX_GPIO18_MCU_WPD_V << IO_MUX_GPIO18_MCU_WPD_S)
#define IO_MUX_GPIO18_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO18_MCU_WPD_S  2
/** IO_MUX_GPIO18_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO18 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO18_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO18_MCU_WPU_M  (IO_MUX_GPIO18_MCU_WPU_V << IO_MUX_GPIO18_MCU_WPU_S)
#define IO_MUX_GPIO18_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO18_MCU_WPU_S  3
/** IO_MUX_GPIO18_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO18 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO18_MCU_IE    (BIT(4))
#define IO_MUX_GPIO18_MCU_IE_M  (IO_MUX_GPIO18_MCU_IE_V << IO_MUX_GPIO18_MCU_IE_S)
#define IO_MUX_GPIO18_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO18_MCU_IE_S  4
/** IO_MUX_GPIO18_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO18 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO18_MCU_DRV    0x00000003U
#define IO_MUX_GPIO18_MCU_DRV_M  (IO_MUX_GPIO18_MCU_DRV_V << IO_MUX_GPIO18_MCU_DRV_S)
#define IO_MUX_GPIO18_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO18_MCU_DRV_S  5
/** IO_MUX_GPIO18_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO18.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO18_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO18_FUN_WPD_M  (IO_MUX_GPIO18_FUN_WPD_V << IO_MUX_GPIO18_FUN_WPD_S)
#define IO_MUX_GPIO18_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO18_FUN_WPD_S  7
/** IO_MUX_GPIO18_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO18.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO18_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO18_FUN_WPU_M  (IO_MUX_GPIO18_FUN_WPU_V << IO_MUX_GPIO18_FUN_WPU_S)
#define IO_MUX_GPIO18_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO18_FUN_WPU_S  8
/** IO_MUX_GPIO18_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO18.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO18_FUN_IE    (BIT(9))
#define IO_MUX_GPIO18_FUN_IE_M  (IO_MUX_GPIO18_FUN_IE_V << IO_MUX_GPIO18_FUN_IE_S)
#define IO_MUX_GPIO18_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO18_FUN_IE_S  9
/** IO_MUX_GPIO18_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO18. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO18_FUN_DRV    0x00000003U
#define IO_MUX_GPIO18_FUN_DRV_M  (IO_MUX_GPIO18_FUN_DRV_V << IO_MUX_GPIO18_FUN_DRV_S)
#define IO_MUX_GPIO18_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO18_FUN_DRV_S  10
/** IO_MUX_GPIO18_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO18_MCU_SEL    0x00000007U
#define IO_MUX_GPIO18_MCU_SEL_M  (IO_MUX_GPIO18_MCU_SEL_V << IO_MUX_GPIO18_MCU_SEL_S)
#define IO_MUX_GPIO18_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO18_MCU_SEL_S  12
/** IO_MUX_GPIO18_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO18_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO18_FILTER_EN_M  (IO_MUX_GPIO18_FILTER_EN_V << IO_MUX_GPIO18_FILTER_EN_S)
#define IO_MUX_GPIO18_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO18_FILTER_EN_S  15
/** IO_MUX_GPIO18_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO18_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO18_HYS_EN    (BIT(16))
#define IO_MUX_GPIO18_HYS_EN_M  (IO_MUX_GPIO18_HYS_EN_V << IO_MUX_GPIO18_HYS_EN_S)
#define IO_MUX_GPIO18_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO18_HYS_EN_S  16
/** IO_MUX_GPIO18_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO18. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO18_HYS_EN\\
 */
#define IO_MUX_GPIO18_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO18_HYS_SEL_M  (IO_MUX_GPIO18_HYS_SEL_V << IO_MUX_GPIO18_HYS_SEL_S)
#define IO_MUX_GPIO18_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO18_HYS_SEL_S  17

/** IO_MUX_GPIO19_REG register
 *  IO MUX configuration register for GPIO19
 */
#define IO_MUX_GPIO19_REG (DR_REG_IO_MUX_BASE + 0x4c)
/** IO_MUX_GPIO19_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO19 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO19_MCU_OE    (BIT(0))
#define IO_MUX_GPIO19_MCU_OE_M  (IO_MUX_GPIO19_MCU_OE_V << IO_MUX_GPIO19_MCU_OE_S)
#define IO_MUX_GPIO19_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO19_MCU_OE_S  0
/** IO_MUX_GPIO19_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO19.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO19_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO19_SLP_SEL_M  (IO_MUX_GPIO19_SLP_SEL_V << IO_MUX_GPIO19_SLP_SEL_S)
#define IO_MUX_GPIO19_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO19_SLP_SEL_S  1
/** IO_MUX_GPIO19_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO19 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO19_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO19_MCU_WPD_M  (IO_MUX_GPIO19_MCU_WPD_V << IO_MUX_GPIO19_MCU_WPD_S)
#define IO_MUX_GPIO19_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO19_MCU_WPD_S  2
/** IO_MUX_GPIO19_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO19 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO19_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO19_MCU_WPU_M  (IO_MUX_GPIO19_MCU_WPU_V << IO_MUX_GPIO19_MCU_WPU_S)
#define IO_MUX_GPIO19_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO19_MCU_WPU_S  3
/** IO_MUX_GPIO19_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO19 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO19_MCU_IE    (BIT(4))
#define IO_MUX_GPIO19_MCU_IE_M  (IO_MUX_GPIO19_MCU_IE_V << IO_MUX_GPIO19_MCU_IE_S)
#define IO_MUX_GPIO19_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO19_MCU_IE_S  4
/** IO_MUX_GPIO19_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO19 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO19_MCU_DRV    0x00000003U
#define IO_MUX_GPIO19_MCU_DRV_M  (IO_MUX_GPIO19_MCU_DRV_V << IO_MUX_GPIO19_MCU_DRV_S)
#define IO_MUX_GPIO19_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO19_MCU_DRV_S  5
/** IO_MUX_GPIO19_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO19.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO19_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO19_FUN_WPD_M  (IO_MUX_GPIO19_FUN_WPD_V << IO_MUX_GPIO19_FUN_WPD_S)
#define IO_MUX_GPIO19_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO19_FUN_WPD_S  7
/** IO_MUX_GPIO19_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO19.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO19_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO19_FUN_WPU_M  (IO_MUX_GPIO19_FUN_WPU_V << IO_MUX_GPIO19_FUN_WPU_S)
#define IO_MUX_GPIO19_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO19_FUN_WPU_S  8
/** IO_MUX_GPIO19_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO19.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO19_FUN_IE    (BIT(9))
#define IO_MUX_GPIO19_FUN_IE_M  (IO_MUX_GPIO19_FUN_IE_V << IO_MUX_GPIO19_FUN_IE_S)
#define IO_MUX_GPIO19_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO19_FUN_IE_S  9
/** IO_MUX_GPIO19_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO19. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO19_FUN_DRV    0x00000003U
#define IO_MUX_GPIO19_FUN_DRV_M  (IO_MUX_GPIO19_FUN_DRV_V << IO_MUX_GPIO19_FUN_DRV_S)
#define IO_MUX_GPIO19_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO19_FUN_DRV_S  10
/** IO_MUX_GPIO19_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO19_MCU_SEL    0x00000007U
#define IO_MUX_GPIO19_MCU_SEL_M  (IO_MUX_GPIO19_MCU_SEL_V << IO_MUX_GPIO19_MCU_SEL_S)
#define IO_MUX_GPIO19_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO19_MCU_SEL_S  12
/** IO_MUX_GPIO19_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO19_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO19_FILTER_EN_M  (IO_MUX_GPIO19_FILTER_EN_V << IO_MUX_GPIO19_FILTER_EN_S)
#define IO_MUX_GPIO19_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO19_FILTER_EN_S  15
/** IO_MUX_GPIO19_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO19_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO19_HYS_EN    (BIT(16))
#define IO_MUX_GPIO19_HYS_EN_M  (IO_MUX_GPIO19_HYS_EN_V << IO_MUX_GPIO19_HYS_EN_S)
#define IO_MUX_GPIO19_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO19_HYS_EN_S  16
/** IO_MUX_GPIO19_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO19. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO19_HYS_EN\\
 */
#define IO_MUX_GPIO19_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO19_HYS_SEL_M  (IO_MUX_GPIO19_HYS_SEL_V << IO_MUX_GPIO19_HYS_SEL_S)
#define IO_MUX_GPIO19_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO19_HYS_SEL_S  17

/** IO_MUX_GPIO20_REG register
 *  IO MUX configuration register for GPIO20
 */
#define IO_MUX_GPIO20_REG (DR_REG_IO_MUX_BASE + 0x50)
/** IO_MUX_GPIO20_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO20 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO20_MCU_OE    (BIT(0))
#define IO_MUX_GPIO20_MCU_OE_M  (IO_MUX_GPIO20_MCU_OE_V << IO_MUX_GPIO20_MCU_OE_S)
#define IO_MUX_GPIO20_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO20_MCU_OE_S  0
/** IO_MUX_GPIO20_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO20.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO20_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO20_SLP_SEL_M  (IO_MUX_GPIO20_SLP_SEL_V << IO_MUX_GPIO20_SLP_SEL_S)
#define IO_MUX_GPIO20_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO20_SLP_SEL_S  1
/** IO_MUX_GPIO20_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO20 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO20_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO20_MCU_WPD_M  (IO_MUX_GPIO20_MCU_WPD_V << IO_MUX_GPIO20_MCU_WPD_S)
#define IO_MUX_GPIO20_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO20_MCU_WPD_S  2
/** IO_MUX_GPIO20_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO20 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO20_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO20_MCU_WPU_M  (IO_MUX_GPIO20_MCU_WPU_V << IO_MUX_GPIO20_MCU_WPU_S)
#define IO_MUX_GPIO20_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO20_MCU_WPU_S  3
/** IO_MUX_GPIO20_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO20 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO20_MCU_IE    (BIT(4))
#define IO_MUX_GPIO20_MCU_IE_M  (IO_MUX_GPIO20_MCU_IE_V << IO_MUX_GPIO20_MCU_IE_S)
#define IO_MUX_GPIO20_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO20_MCU_IE_S  4
/** IO_MUX_GPIO20_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO20 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO20_MCU_DRV    0x00000003U
#define IO_MUX_GPIO20_MCU_DRV_M  (IO_MUX_GPIO20_MCU_DRV_V << IO_MUX_GPIO20_MCU_DRV_S)
#define IO_MUX_GPIO20_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO20_MCU_DRV_S  5
/** IO_MUX_GPIO20_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO20.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO20_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO20_FUN_WPD_M  (IO_MUX_GPIO20_FUN_WPD_V << IO_MUX_GPIO20_FUN_WPD_S)
#define IO_MUX_GPIO20_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO20_FUN_WPD_S  7
/** IO_MUX_GPIO20_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO20.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO20_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO20_FUN_WPU_M  (IO_MUX_GPIO20_FUN_WPU_V << IO_MUX_GPIO20_FUN_WPU_S)
#define IO_MUX_GPIO20_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO20_FUN_WPU_S  8
/** IO_MUX_GPIO20_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO20.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO20_FUN_IE    (BIT(9))
#define IO_MUX_GPIO20_FUN_IE_M  (IO_MUX_GPIO20_FUN_IE_V << IO_MUX_GPIO20_FUN_IE_S)
#define IO_MUX_GPIO20_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO20_FUN_IE_S  9
/** IO_MUX_GPIO20_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO20. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO20_FUN_DRV    0x00000003U
#define IO_MUX_GPIO20_FUN_DRV_M  (IO_MUX_GPIO20_FUN_DRV_V << IO_MUX_GPIO20_FUN_DRV_S)
#define IO_MUX_GPIO20_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO20_FUN_DRV_S  10
/** IO_MUX_GPIO20_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO20_MCU_SEL    0x00000007U
#define IO_MUX_GPIO20_MCU_SEL_M  (IO_MUX_GPIO20_MCU_SEL_V << IO_MUX_GPIO20_MCU_SEL_S)
#define IO_MUX_GPIO20_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO20_MCU_SEL_S  12
/** IO_MUX_GPIO20_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO20_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO20_FILTER_EN_M  (IO_MUX_GPIO20_FILTER_EN_V << IO_MUX_GPIO20_FILTER_EN_S)
#define IO_MUX_GPIO20_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO20_FILTER_EN_S  15
/** IO_MUX_GPIO20_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO20_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO20_HYS_EN    (BIT(16))
#define IO_MUX_GPIO20_HYS_EN_M  (IO_MUX_GPIO20_HYS_EN_V << IO_MUX_GPIO20_HYS_EN_S)
#define IO_MUX_GPIO20_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO20_HYS_EN_S  16
/** IO_MUX_GPIO20_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO20. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO20_HYS_EN\\
 */
#define IO_MUX_GPIO20_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO20_HYS_SEL_M  (IO_MUX_GPIO20_HYS_SEL_V << IO_MUX_GPIO20_HYS_SEL_S)
#define IO_MUX_GPIO20_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO20_HYS_SEL_S  17

/** IO_MUX_GPIO21_REG register
 *  IO MUX configuration register for GPIO21
 */
#define IO_MUX_GPIO21_REG (DR_REG_IO_MUX_BASE + 0x54)
/** IO_MUX_GPIO21_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO21 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO21_MCU_OE    (BIT(0))
#define IO_MUX_GPIO21_MCU_OE_M  (IO_MUX_GPIO21_MCU_OE_V << IO_MUX_GPIO21_MCU_OE_S)
#define IO_MUX_GPIO21_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO21_MCU_OE_S  0
/** IO_MUX_GPIO21_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO21.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO21_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO21_SLP_SEL_M  (IO_MUX_GPIO21_SLP_SEL_V << IO_MUX_GPIO21_SLP_SEL_S)
#define IO_MUX_GPIO21_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO21_SLP_SEL_S  1
/** IO_MUX_GPIO21_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO21 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO21_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO21_MCU_WPD_M  (IO_MUX_GPIO21_MCU_WPD_V << IO_MUX_GPIO21_MCU_WPD_S)
#define IO_MUX_GPIO21_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO21_MCU_WPD_S  2
/** IO_MUX_GPIO21_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO21 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO21_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO21_MCU_WPU_M  (IO_MUX_GPIO21_MCU_WPU_V << IO_MUX_GPIO21_MCU_WPU_S)
#define IO_MUX_GPIO21_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO21_MCU_WPU_S  3
/** IO_MUX_GPIO21_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO21 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO21_MCU_IE    (BIT(4))
#define IO_MUX_GPIO21_MCU_IE_M  (IO_MUX_GPIO21_MCU_IE_V << IO_MUX_GPIO21_MCU_IE_S)
#define IO_MUX_GPIO21_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO21_MCU_IE_S  4
/** IO_MUX_GPIO21_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO21 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO21_MCU_DRV    0x00000003U
#define IO_MUX_GPIO21_MCU_DRV_M  (IO_MUX_GPIO21_MCU_DRV_V << IO_MUX_GPIO21_MCU_DRV_S)
#define IO_MUX_GPIO21_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO21_MCU_DRV_S  5
/** IO_MUX_GPIO21_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO21.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO21_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO21_FUN_WPD_M  (IO_MUX_GPIO21_FUN_WPD_V << IO_MUX_GPIO21_FUN_WPD_S)
#define IO_MUX_GPIO21_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO21_FUN_WPD_S  7
/** IO_MUX_GPIO21_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO21.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO21_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO21_FUN_WPU_M  (IO_MUX_GPIO21_FUN_WPU_V << IO_MUX_GPIO21_FUN_WPU_S)
#define IO_MUX_GPIO21_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO21_FUN_WPU_S  8
/** IO_MUX_GPIO21_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO21.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO21_FUN_IE    (BIT(9))
#define IO_MUX_GPIO21_FUN_IE_M  (IO_MUX_GPIO21_FUN_IE_V << IO_MUX_GPIO21_FUN_IE_S)
#define IO_MUX_GPIO21_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO21_FUN_IE_S  9
/** IO_MUX_GPIO21_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO21. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO21_FUN_DRV    0x00000003U
#define IO_MUX_GPIO21_FUN_DRV_M  (IO_MUX_GPIO21_FUN_DRV_V << IO_MUX_GPIO21_FUN_DRV_S)
#define IO_MUX_GPIO21_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO21_FUN_DRV_S  10
/** IO_MUX_GPIO21_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO21_MCU_SEL    0x00000007U
#define IO_MUX_GPIO21_MCU_SEL_M  (IO_MUX_GPIO21_MCU_SEL_V << IO_MUX_GPIO21_MCU_SEL_S)
#define IO_MUX_GPIO21_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO21_MCU_SEL_S  12
/** IO_MUX_GPIO21_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO21_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO21_FILTER_EN_M  (IO_MUX_GPIO21_FILTER_EN_V << IO_MUX_GPIO21_FILTER_EN_S)
#define IO_MUX_GPIO21_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO21_FILTER_EN_S  15
/** IO_MUX_GPIO21_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO21_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO21_HYS_EN    (BIT(16))
#define IO_MUX_GPIO21_HYS_EN_M  (IO_MUX_GPIO21_HYS_EN_V << IO_MUX_GPIO21_HYS_EN_S)
#define IO_MUX_GPIO21_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO21_HYS_EN_S  16
/** IO_MUX_GPIO21_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO21. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO21_HYS_EN\\
 */
#define IO_MUX_GPIO21_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO21_HYS_SEL_M  (IO_MUX_GPIO21_HYS_SEL_V << IO_MUX_GPIO21_HYS_SEL_S)
#define IO_MUX_GPIO21_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO21_HYS_SEL_S  17

/** IO_MUX_GPIO22_REG register
 *  IO MUX configuration register for GPIO22
 */
#define IO_MUX_GPIO22_REG (DR_REG_IO_MUX_BASE + 0x58)
/** IO_MUX_GPIO22_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO22 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO22_MCU_OE    (BIT(0))
#define IO_MUX_GPIO22_MCU_OE_M  (IO_MUX_GPIO22_MCU_OE_V << IO_MUX_GPIO22_MCU_OE_S)
#define IO_MUX_GPIO22_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO22_MCU_OE_S  0
/** IO_MUX_GPIO22_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO22.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO22_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO22_SLP_SEL_M  (IO_MUX_GPIO22_SLP_SEL_V << IO_MUX_GPIO22_SLP_SEL_S)
#define IO_MUX_GPIO22_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO22_SLP_SEL_S  1
/** IO_MUX_GPIO22_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO22 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO22_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO22_MCU_WPD_M  (IO_MUX_GPIO22_MCU_WPD_V << IO_MUX_GPIO22_MCU_WPD_S)
#define IO_MUX_GPIO22_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO22_MCU_WPD_S  2
/** IO_MUX_GPIO22_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO22 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO22_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO22_MCU_WPU_M  (IO_MUX_GPIO22_MCU_WPU_V << IO_MUX_GPIO22_MCU_WPU_S)
#define IO_MUX_GPIO22_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO22_MCU_WPU_S  3
/** IO_MUX_GPIO22_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO22 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO22_MCU_IE    (BIT(4))
#define IO_MUX_GPIO22_MCU_IE_M  (IO_MUX_GPIO22_MCU_IE_V << IO_MUX_GPIO22_MCU_IE_S)
#define IO_MUX_GPIO22_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO22_MCU_IE_S  4
/** IO_MUX_GPIO22_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO22 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO22_MCU_DRV    0x00000003U
#define IO_MUX_GPIO22_MCU_DRV_M  (IO_MUX_GPIO22_MCU_DRV_V << IO_MUX_GPIO22_MCU_DRV_S)
#define IO_MUX_GPIO22_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO22_MCU_DRV_S  5
/** IO_MUX_GPIO22_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO22.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO22_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO22_FUN_WPD_M  (IO_MUX_GPIO22_FUN_WPD_V << IO_MUX_GPIO22_FUN_WPD_S)
#define IO_MUX_GPIO22_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO22_FUN_WPD_S  7
/** IO_MUX_GPIO22_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO22.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO22_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO22_FUN_WPU_M  (IO_MUX_GPIO22_FUN_WPU_V << IO_MUX_GPIO22_FUN_WPU_S)
#define IO_MUX_GPIO22_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO22_FUN_WPU_S  8
/** IO_MUX_GPIO22_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO22.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO22_FUN_IE    (BIT(9))
#define IO_MUX_GPIO22_FUN_IE_M  (IO_MUX_GPIO22_FUN_IE_V << IO_MUX_GPIO22_FUN_IE_S)
#define IO_MUX_GPIO22_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO22_FUN_IE_S  9
/** IO_MUX_GPIO22_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO22. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO22_FUN_DRV    0x00000003U
#define IO_MUX_GPIO22_FUN_DRV_M  (IO_MUX_GPIO22_FUN_DRV_V << IO_MUX_GPIO22_FUN_DRV_S)
#define IO_MUX_GPIO22_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO22_FUN_DRV_S  10
/** IO_MUX_GPIO22_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO22_MCU_SEL    0x00000007U
#define IO_MUX_GPIO22_MCU_SEL_M  (IO_MUX_GPIO22_MCU_SEL_V << IO_MUX_GPIO22_MCU_SEL_S)
#define IO_MUX_GPIO22_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO22_MCU_SEL_S  12
/** IO_MUX_GPIO22_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO22_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO22_FILTER_EN_M  (IO_MUX_GPIO22_FILTER_EN_V << IO_MUX_GPIO22_FILTER_EN_S)
#define IO_MUX_GPIO22_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO22_FILTER_EN_S  15
/** IO_MUX_GPIO22_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO22_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO22_HYS_EN    (BIT(16))
#define IO_MUX_GPIO22_HYS_EN_M  (IO_MUX_GPIO22_HYS_EN_V << IO_MUX_GPIO22_HYS_EN_S)
#define IO_MUX_GPIO22_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO22_HYS_EN_S  16
/** IO_MUX_GPIO22_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO22. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO22_HYS_EN\\
 */
#define IO_MUX_GPIO22_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO22_HYS_SEL_M  (IO_MUX_GPIO22_HYS_SEL_V << IO_MUX_GPIO22_HYS_SEL_S)
#define IO_MUX_GPIO22_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO22_HYS_SEL_S  17

/** IO_MUX_GPIO23_REG register
 *  IO MUX configuration register for GPIO23
 */
#define IO_MUX_GPIO23_REG (DR_REG_IO_MUX_BASE + 0x5c)
/** IO_MUX_GPIO23_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO23 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO23_MCU_OE    (BIT(0))
#define IO_MUX_GPIO23_MCU_OE_M  (IO_MUX_GPIO23_MCU_OE_V << IO_MUX_GPIO23_MCU_OE_S)
#define IO_MUX_GPIO23_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO23_MCU_OE_S  0
/** IO_MUX_GPIO23_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO23.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO23_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO23_SLP_SEL_M  (IO_MUX_GPIO23_SLP_SEL_V << IO_MUX_GPIO23_SLP_SEL_S)
#define IO_MUX_GPIO23_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO23_SLP_SEL_S  1
/** IO_MUX_GPIO23_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO23 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO23_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO23_MCU_WPD_M  (IO_MUX_GPIO23_MCU_WPD_V << IO_MUX_GPIO23_MCU_WPD_S)
#define IO_MUX_GPIO23_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO23_MCU_WPD_S  2
/** IO_MUX_GPIO23_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO23 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO23_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO23_MCU_WPU_M  (IO_MUX_GPIO23_MCU_WPU_V << IO_MUX_GPIO23_MCU_WPU_S)
#define IO_MUX_GPIO23_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO23_MCU_WPU_S  3
/** IO_MUX_GPIO23_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO23 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO23_MCU_IE    (BIT(4))
#define IO_MUX_GPIO23_MCU_IE_M  (IO_MUX_GPIO23_MCU_IE_V << IO_MUX_GPIO23_MCU_IE_S)
#define IO_MUX_GPIO23_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO23_MCU_IE_S  4
/** IO_MUX_GPIO23_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO23 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO23_MCU_DRV    0x00000003U
#define IO_MUX_GPIO23_MCU_DRV_M  (IO_MUX_GPIO23_MCU_DRV_V << IO_MUX_GPIO23_MCU_DRV_S)
#define IO_MUX_GPIO23_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO23_MCU_DRV_S  5
/** IO_MUX_GPIO23_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO23.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO23_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO23_FUN_WPD_M  (IO_MUX_GPIO23_FUN_WPD_V << IO_MUX_GPIO23_FUN_WPD_S)
#define IO_MUX_GPIO23_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO23_FUN_WPD_S  7
/** IO_MUX_GPIO23_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO23.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO23_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO23_FUN_WPU_M  (IO_MUX_GPIO23_FUN_WPU_V << IO_MUX_GPIO23_FUN_WPU_S)
#define IO_MUX_GPIO23_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO23_FUN_WPU_S  8
/** IO_MUX_GPIO23_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO23.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO23_FUN_IE    (BIT(9))
#define IO_MUX_GPIO23_FUN_IE_M  (IO_MUX_GPIO23_FUN_IE_V << IO_MUX_GPIO23_FUN_IE_S)
#define IO_MUX_GPIO23_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO23_FUN_IE_S  9
/** IO_MUX_GPIO23_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO23. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO23_FUN_DRV    0x00000003U
#define IO_MUX_GPIO23_FUN_DRV_M  (IO_MUX_GPIO23_FUN_DRV_V << IO_MUX_GPIO23_FUN_DRV_S)
#define IO_MUX_GPIO23_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO23_FUN_DRV_S  10
/** IO_MUX_GPIO23_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO23_MCU_SEL    0x00000007U
#define IO_MUX_GPIO23_MCU_SEL_M  (IO_MUX_GPIO23_MCU_SEL_V << IO_MUX_GPIO23_MCU_SEL_S)
#define IO_MUX_GPIO23_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO23_MCU_SEL_S  12
/** IO_MUX_GPIO23_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO23_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO23_FILTER_EN_M  (IO_MUX_GPIO23_FILTER_EN_V << IO_MUX_GPIO23_FILTER_EN_S)
#define IO_MUX_GPIO23_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO23_FILTER_EN_S  15
/** IO_MUX_GPIO23_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO23_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO23_HYS_EN    (BIT(16))
#define IO_MUX_GPIO23_HYS_EN_M  (IO_MUX_GPIO23_HYS_EN_V << IO_MUX_GPIO23_HYS_EN_S)
#define IO_MUX_GPIO23_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO23_HYS_EN_S  16
/** IO_MUX_GPIO23_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO23. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO23_HYS_EN\\
 */
#define IO_MUX_GPIO23_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO23_HYS_SEL_M  (IO_MUX_GPIO23_HYS_SEL_V << IO_MUX_GPIO23_HYS_SEL_S)
#define IO_MUX_GPIO23_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO23_HYS_SEL_S  17

/** IO_MUX_GPIO24_REG register
 *  IO MUX configuration register for GPIO24
 */
#define IO_MUX_GPIO24_REG (DR_REG_IO_MUX_BASE + 0x60)
/** IO_MUX_GPIO24_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO24 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO24_MCU_OE    (BIT(0))
#define IO_MUX_GPIO24_MCU_OE_M  (IO_MUX_GPIO24_MCU_OE_V << IO_MUX_GPIO24_MCU_OE_S)
#define IO_MUX_GPIO24_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO24_MCU_OE_S  0
/** IO_MUX_GPIO24_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO24.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO24_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO24_SLP_SEL_M  (IO_MUX_GPIO24_SLP_SEL_V << IO_MUX_GPIO24_SLP_SEL_S)
#define IO_MUX_GPIO24_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO24_SLP_SEL_S  1
/** IO_MUX_GPIO24_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO24 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO24_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO24_MCU_WPD_M  (IO_MUX_GPIO24_MCU_WPD_V << IO_MUX_GPIO24_MCU_WPD_S)
#define IO_MUX_GPIO24_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO24_MCU_WPD_S  2
/** IO_MUX_GPIO24_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO24 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO24_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO24_MCU_WPU_M  (IO_MUX_GPIO24_MCU_WPU_V << IO_MUX_GPIO24_MCU_WPU_S)
#define IO_MUX_GPIO24_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO24_MCU_WPU_S  3
/** IO_MUX_GPIO24_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO24 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO24_MCU_IE    (BIT(4))
#define IO_MUX_GPIO24_MCU_IE_M  (IO_MUX_GPIO24_MCU_IE_V << IO_MUX_GPIO24_MCU_IE_S)
#define IO_MUX_GPIO24_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO24_MCU_IE_S  4
/** IO_MUX_GPIO24_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO24 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO24_MCU_DRV    0x00000003U
#define IO_MUX_GPIO24_MCU_DRV_M  (IO_MUX_GPIO24_MCU_DRV_V << IO_MUX_GPIO24_MCU_DRV_S)
#define IO_MUX_GPIO24_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO24_MCU_DRV_S  5
/** IO_MUX_GPIO24_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO24.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO24_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO24_FUN_WPD_M  (IO_MUX_GPIO24_FUN_WPD_V << IO_MUX_GPIO24_FUN_WPD_S)
#define IO_MUX_GPIO24_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO24_FUN_WPD_S  7
/** IO_MUX_GPIO24_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO24.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO24_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO24_FUN_WPU_M  (IO_MUX_GPIO24_FUN_WPU_V << IO_MUX_GPIO24_FUN_WPU_S)
#define IO_MUX_GPIO24_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO24_FUN_WPU_S  8
/** IO_MUX_GPIO24_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO24.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO24_FUN_IE    (BIT(9))
#define IO_MUX_GPIO24_FUN_IE_M  (IO_MUX_GPIO24_FUN_IE_V << IO_MUX_GPIO24_FUN_IE_S)
#define IO_MUX_GPIO24_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO24_FUN_IE_S  9
/** IO_MUX_GPIO24_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO24. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO24_FUN_DRV    0x00000003U
#define IO_MUX_GPIO24_FUN_DRV_M  (IO_MUX_GPIO24_FUN_DRV_V << IO_MUX_GPIO24_FUN_DRV_S)
#define IO_MUX_GPIO24_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO24_FUN_DRV_S  10
/** IO_MUX_GPIO24_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO24_MCU_SEL    0x00000007U
#define IO_MUX_GPIO24_MCU_SEL_M  (IO_MUX_GPIO24_MCU_SEL_V << IO_MUX_GPIO24_MCU_SEL_S)
#define IO_MUX_GPIO24_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO24_MCU_SEL_S  12
/** IO_MUX_GPIO24_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO24_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO24_FILTER_EN_M  (IO_MUX_GPIO24_FILTER_EN_V << IO_MUX_GPIO24_FILTER_EN_S)
#define IO_MUX_GPIO24_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO24_FILTER_EN_S  15
/** IO_MUX_GPIO24_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO24_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO24_HYS_EN    (BIT(16))
#define IO_MUX_GPIO24_HYS_EN_M  (IO_MUX_GPIO24_HYS_EN_V << IO_MUX_GPIO24_HYS_EN_S)
#define IO_MUX_GPIO24_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO24_HYS_EN_S  16
/** IO_MUX_GPIO24_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO24. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO24_HYS_EN\\
 */
#define IO_MUX_GPIO24_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO24_HYS_SEL_M  (IO_MUX_GPIO24_HYS_SEL_V << IO_MUX_GPIO24_HYS_SEL_S)
#define IO_MUX_GPIO24_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO24_HYS_SEL_S  17

/** IO_MUX_GPIO25_REG register
 *  IO MUX configuration register for GPIO25
 */
#define IO_MUX_GPIO25_REG (DR_REG_IO_MUX_BASE + 0x64)
/** IO_MUX_GPIO25_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO25 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO25_MCU_OE    (BIT(0))
#define IO_MUX_GPIO25_MCU_OE_M  (IO_MUX_GPIO25_MCU_OE_V << IO_MUX_GPIO25_MCU_OE_S)
#define IO_MUX_GPIO25_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO25_MCU_OE_S  0
/** IO_MUX_GPIO25_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO25.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO25_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO25_SLP_SEL_M  (IO_MUX_GPIO25_SLP_SEL_V << IO_MUX_GPIO25_SLP_SEL_S)
#define IO_MUX_GPIO25_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO25_SLP_SEL_S  1
/** IO_MUX_GPIO25_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO25 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO25_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO25_MCU_WPD_M  (IO_MUX_GPIO25_MCU_WPD_V << IO_MUX_GPIO25_MCU_WPD_S)
#define IO_MUX_GPIO25_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO25_MCU_WPD_S  2
/** IO_MUX_GPIO25_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO25 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO25_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO25_MCU_WPU_M  (IO_MUX_GPIO25_MCU_WPU_V << IO_MUX_GPIO25_MCU_WPU_S)
#define IO_MUX_GPIO25_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO25_MCU_WPU_S  3
/** IO_MUX_GPIO25_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO25 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO25_MCU_IE    (BIT(4))
#define IO_MUX_GPIO25_MCU_IE_M  (IO_MUX_GPIO25_MCU_IE_V << IO_MUX_GPIO25_MCU_IE_S)
#define IO_MUX_GPIO25_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO25_MCU_IE_S  4
/** IO_MUX_GPIO25_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO25 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO25_MCU_DRV    0x00000003U
#define IO_MUX_GPIO25_MCU_DRV_M  (IO_MUX_GPIO25_MCU_DRV_V << IO_MUX_GPIO25_MCU_DRV_S)
#define IO_MUX_GPIO25_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO25_MCU_DRV_S  5
/** IO_MUX_GPIO25_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO25.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO25_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO25_FUN_WPD_M  (IO_MUX_GPIO25_FUN_WPD_V << IO_MUX_GPIO25_FUN_WPD_S)
#define IO_MUX_GPIO25_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO25_FUN_WPD_S  7
/** IO_MUX_GPIO25_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO25.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO25_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO25_FUN_WPU_M  (IO_MUX_GPIO25_FUN_WPU_V << IO_MUX_GPIO25_FUN_WPU_S)
#define IO_MUX_GPIO25_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO25_FUN_WPU_S  8
/** IO_MUX_GPIO25_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO25.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO25_FUN_IE    (BIT(9))
#define IO_MUX_GPIO25_FUN_IE_M  (IO_MUX_GPIO25_FUN_IE_V << IO_MUX_GPIO25_FUN_IE_S)
#define IO_MUX_GPIO25_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO25_FUN_IE_S  9
/** IO_MUX_GPIO25_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO25. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO25_FUN_DRV    0x00000003U
#define IO_MUX_GPIO25_FUN_DRV_M  (IO_MUX_GPIO25_FUN_DRV_V << IO_MUX_GPIO25_FUN_DRV_S)
#define IO_MUX_GPIO25_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO25_FUN_DRV_S  10
/** IO_MUX_GPIO25_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO25_MCU_SEL    0x00000007U
#define IO_MUX_GPIO25_MCU_SEL_M  (IO_MUX_GPIO25_MCU_SEL_V << IO_MUX_GPIO25_MCU_SEL_S)
#define IO_MUX_GPIO25_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO25_MCU_SEL_S  12
/** IO_MUX_GPIO25_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO25_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO25_FILTER_EN_M  (IO_MUX_GPIO25_FILTER_EN_V << IO_MUX_GPIO25_FILTER_EN_S)
#define IO_MUX_GPIO25_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO25_FILTER_EN_S  15
/** IO_MUX_GPIO25_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO25_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO25_HYS_EN    (BIT(16))
#define IO_MUX_GPIO25_HYS_EN_M  (IO_MUX_GPIO25_HYS_EN_V << IO_MUX_GPIO25_HYS_EN_S)
#define IO_MUX_GPIO25_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO25_HYS_EN_S  16
/** IO_MUX_GPIO25_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO25. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO25_HYS_EN\\
 */
#define IO_MUX_GPIO25_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO25_HYS_SEL_M  (IO_MUX_GPIO25_HYS_SEL_V << IO_MUX_GPIO25_HYS_SEL_S)
#define IO_MUX_GPIO25_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO25_HYS_SEL_S  17

/** IO_MUX_GPIO26_REG register
 *  IO MUX configuration register for GPIO26
 */
#define IO_MUX_GPIO26_REG (DR_REG_IO_MUX_BASE + 0x68)
/** IO_MUX_GPIO26_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO26 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO26_MCU_OE    (BIT(0))
#define IO_MUX_GPIO26_MCU_OE_M  (IO_MUX_GPIO26_MCU_OE_V << IO_MUX_GPIO26_MCU_OE_S)
#define IO_MUX_GPIO26_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO26_MCU_OE_S  0
/** IO_MUX_GPIO26_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO26.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO26_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO26_SLP_SEL_M  (IO_MUX_GPIO26_SLP_SEL_V << IO_MUX_GPIO26_SLP_SEL_S)
#define IO_MUX_GPIO26_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO26_SLP_SEL_S  1
/** IO_MUX_GPIO26_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO26 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO26_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO26_MCU_WPD_M  (IO_MUX_GPIO26_MCU_WPD_V << IO_MUX_GPIO26_MCU_WPD_S)
#define IO_MUX_GPIO26_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO26_MCU_WPD_S  2
/** IO_MUX_GPIO26_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO26 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO26_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO26_MCU_WPU_M  (IO_MUX_GPIO26_MCU_WPU_V << IO_MUX_GPIO26_MCU_WPU_S)
#define IO_MUX_GPIO26_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO26_MCU_WPU_S  3
/** IO_MUX_GPIO26_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO26 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO26_MCU_IE    (BIT(4))
#define IO_MUX_GPIO26_MCU_IE_M  (IO_MUX_GPIO26_MCU_IE_V << IO_MUX_GPIO26_MCU_IE_S)
#define IO_MUX_GPIO26_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO26_MCU_IE_S  4
/** IO_MUX_GPIO26_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO26 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO26_MCU_DRV    0x00000003U
#define IO_MUX_GPIO26_MCU_DRV_M  (IO_MUX_GPIO26_MCU_DRV_V << IO_MUX_GPIO26_MCU_DRV_S)
#define IO_MUX_GPIO26_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO26_MCU_DRV_S  5
/** IO_MUX_GPIO26_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO26.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO26_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO26_FUN_WPD_M  (IO_MUX_GPIO26_FUN_WPD_V << IO_MUX_GPIO26_FUN_WPD_S)
#define IO_MUX_GPIO26_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO26_FUN_WPD_S  7
/** IO_MUX_GPIO26_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO26.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO26_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO26_FUN_WPU_M  (IO_MUX_GPIO26_FUN_WPU_V << IO_MUX_GPIO26_FUN_WPU_S)
#define IO_MUX_GPIO26_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO26_FUN_WPU_S  8
/** IO_MUX_GPIO26_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO26.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO26_FUN_IE    (BIT(9))
#define IO_MUX_GPIO26_FUN_IE_M  (IO_MUX_GPIO26_FUN_IE_V << IO_MUX_GPIO26_FUN_IE_S)
#define IO_MUX_GPIO26_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO26_FUN_IE_S  9
/** IO_MUX_GPIO26_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO26. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO26_FUN_DRV    0x00000003U
#define IO_MUX_GPIO26_FUN_DRV_M  (IO_MUX_GPIO26_FUN_DRV_V << IO_MUX_GPIO26_FUN_DRV_S)
#define IO_MUX_GPIO26_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO26_FUN_DRV_S  10
/** IO_MUX_GPIO26_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO26_MCU_SEL    0x00000007U
#define IO_MUX_GPIO26_MCU_SEL_M  (IO_MUX_GPIO26_MCU_SEL_V << IO_MUX_GPIO26_MCU_SEL_S)
#define IO_MUX_GPIO26_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO26_MCU_SEL_S  12
/** IO_MUX_GPIO26_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO26_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO26_FILTER_EN_M  (IO_MUX_GPIO26_FILTER_EN_V << IO_MUX_GPIO26_FILTER_EN_S)
#define IO_MUX_GPIO26_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO26_FILTER_EN_S  15
/** IO_MUX_GPIO26_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO26_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO26_HYS_EN    (BIT(16))
#define IO_MUX_GPIO26_HYS_EN_M  (IO_MUX_GPIO26_HYS_EN_V << IO_MUX_GPIO26_HYS_EN_S)
#define IO_MUX_GPIO26_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO26_HYS_EN_S  16
/** IO_MUX_GPIO26_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO26. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO26_HYS_EN\\
 */
#define IO_MUX_GPIO26_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO26_HYS_SEL_M  (IO_MUX_GPIO26_HYS_SEL_V << IO_MUX_GPIO26_HYS_SEL_S)
#define IO_MUX_GPIO26_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO26_HYS_SEL_S  17

/** IO_MUX_GPIO27_REG register
 *  IO MUX configuration register for GPIO27
 */
#define IO_MUX_GPIO27_REG (DR_REG_IO_MUX_BASE + 0x6c)
/** IO_MUX_GPIO27_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO27 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO27_MCU_OE    (BIT(0))
#define IO_MUX_GPIO27_MCU_OE_M  (IO_MUX_GPIO27_MCU_OE_V << IO_MUX_GPIO27_MCU_OE_S)
#define IO_MUX_GPIO27_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO27_MCU_OE_S  0
/** IO_MUX_GPIO27_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO27.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO27_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO27_SLP_SEL_M  (IO_MUX_GPIO27_SLP_SEL_V << IO_MUX_GPIO27_SLP_SEL_S)
#define IO_MUX_GPIO27_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO27_SLP_SEL_S  1
/** IO_MUX_GPIO27_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO27 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO27_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO27_MCU_WPD_M  (IO_MUX_GPIO27_MCU_WPD_V << IO_MUX_GPIO27_MCU_WPD_S)
#define IO_MUX_GPIO27_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO27_MCU_WPD_S  2
/** IO_MUX_GPIO27_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO27 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO27_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO27_MCU_WPU_M  (IO_MUX_GPIO27_MCU_WPU_V << IO_MUX_GPIO27_MCU_WPU_S)
#define IO_MUX_GPIO27_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO27_MCU_WPU_S  3
/** IO_MUX_GPIO27_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO27 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO27_MCU_IE    (BIT(4))
#define IO_MUX_GPIO27_MCU_IE_M  (IO_MUX_GPIO27_MCU_IE_V << IO_MUX_GPIO27_MCU_IE_S)
#define IO_MUX_GPIO27_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO27_MCU_IE_S  4
/** IO_MUX_GPIO27_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO27 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO27_MCU_DRV    0x00000003U
#define IO_MUX_GPIO27_MCU_DRV_M  (IO_MUX_GPIO27_MCU_DRV_V << IO_MUX_GPIO27_MCU_DRV_S)
#define IO_MUX_GPIO27_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO27_MCU_DRV_S  5
/** IO_MUX_GPIO27_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO27.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO27_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO27_FUN_WPD_M  (IO_MUX_GPIO27_FUN_WPD_V << IO_MUX_GPIO27_FUN_WPD_S)
#define IO_MUX_GPIO27_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO27_FUN_WPD_S  7
/** IO_MUX_GPIO27_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO27.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO27_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO27_FUN_WPU_M  (IO_MUX_GPIO27_FUN_WPU_V << IO_MUX_GPIO27_FUN_WPU_S)
#define IO_MUX_GPIO27_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO27_FUN_WPU_S  8
/** IO_MUX_GPIO27_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO27.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO27_FUN_IE    (BIT(9))
#define IO_MUX_GPIO27_FUN_IE_M  (IO_MUX_GPIO27_FUN_IE_V << IO_MUX_GPIO27_FUN_IE_S)
#define IO_MUX_GPIO27_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO27_FUN_IE_S  9
/** IO_MUX_GPIO27_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO27. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO27_FUN_DRV    0x00000003U
#define IO_MUX_GPIO27_FUN_DRV_M  (IO_MUX_GPIO27_FUN_DRV_V << IO_MUX_GPIO27_FUN_DRV_S)
#define IO_MUX_GPIO27_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO27_FUN_DRV_S  10
/** IO_MUX_GPIO27_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO27_MCU_SEL    0x00000007U
#define IO_MUX_GPIO27_MCU_SEL_M  (IO_MUX_GPIO27_MCU_SEL_V << IO_MUX_GPIO27_MCU_SEL_S)
#define IO_MUX_GPIO27_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO27_MCU_SEL_S  12
/** IO_MUX_GPIO27_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO27_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO27_FILTER_EN_M  (IO_MUX_GPIO27_FILTER_EN_V << IO_MUX_GPIO27_FILTER_EN_S)
#define IO_MUX_GPIO27_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO27_FILTER_EN_S  15
/** IO_MUX_GPIO27_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO27_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO27_HYS_EN    (BIT(16))
#define IO_MUX_GPIO27_HYS_EN_M  (IO_MUX_GPIO27_HYS_EN_V << IO_MUX_GPIO27_HYS_EN_S)
#define IO_MUX_GPIO27_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO27_HYS_EN_S  16
/** IO_MUX_GPIO27_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO27. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO27_HYS_EN\\
 */
#define IO_MUX_GPIO27_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO27_HYS_SEL_M  (IO_MUX_GPIO27_HYS_SEL_V << IO_MUX_GPIO27_HYS_SEL_S)
#define IO_MUX_GPIO27_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO27_HYS_SEL_S  17

/** IO_MUX_GPIO28_REG register
 *  IO MUX configuration register for GPIO28
 */
#define IO_MUX_GPIO28_REG (DR_REG_IO_MUX_BASE + 0x70)
/** IO_MUX_GPIO28_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of GPIO28 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO28_MCU_OE    (BIT(0))
#define IO_MUX_GPIO28_MCU_OE_M  (IO_MUX_GPIO28_MCU_OE_V << IO_MUX_GPIO28_MCU_OE_S)
#define IO_MUX_GPIO28_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO28_MCU_OE_S  0
/** IO_MUX_GPIO28_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for GPIO28.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define IO_MUX_GPIO28_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO28_SLP_SEL_M  (IO_MUX_GPIO28_SLP_SEL_V << IO_MUX_GPIO28_SLP_SEL_S)
#define IO_MUX_GPIO28_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO28_SLP_SEL_S  1
/** IO_MUX_GPIO28_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of GPIO28 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO28_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO28_MCU_WPD_M  (IO_MUX_GPIO28_MCU_WPD_V << IO_MUX_GPIO28_MCU_WPD_S)
#define IO_MUX_GPIO28_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO28_MCU_WPD_S  2
/** IO_MUX_GPIO28_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of GPIO28 during sleep mode. \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO28_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO28_MCU_WPU_M  (IO_MUX_GPIO28_MCU_WPU_V << IO_MUX_GPIO28_MCU_WPU_S)
#define IO_MUX_GPIO28_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO28_MCU_WPU_S  3
/** IO_MUX_GPIO28_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of GPIO28 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO28_MCU_IE    (BIT(4))
#define IO_MUX_GPIO28_MCU_IE_M  (IO_MUX_GPIO28_MCU_IE_V << IO_MUX_GPIO28_MCU_IE_S)
#define IO_MUX_GPIO28_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO28_MCU_IE_S  4
/** IO_MUX_GPIO28_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of GPIO28 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO28_MCU_DRV    0x00000003U
#define IO_MUX_GPIO28_MCU_DRV_M  (IO_MUX_GPIO28_MCU_DRV_V << IO_MUX_GPIO28_MCU_DRV_S)
#define IO_MUX_GPIO28_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO28_MCU_DRV_S  5
/** IO_MUX_GPIO28_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of GPIO28.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO28_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO28_FUN_WPD_M  (IO_MUX_GPIO28_FUN_WPD_V << IO_MUX_GPIO28_FUN_WPD_S)
#define IO_MUX_GPIO28_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO28_FUN_WPD_S  7
/** IO_MUX_GPIO28_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of GPIO28.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO28_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO28_FUN_WPU_M  (IO_MUX_GPIO28_FUN_WPU_V << IO_MUX_GPIO28_FUN_WPU_S)
#define IO_MUX_GPIO28_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO28_FUN_WPU_S  8
/** IO_MUX_GPIO28_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of GPIO28.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO28_FUN_IE    (BIT(9))
#define IO_MUX_GPIO28_FUN_IE_M  (IO_MUX_GPIO28_FUN_IE_V << IO_MUX_GPIO28_FUN_IE_S)
#define IO_MUX_GPIO28_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO28_FUN_IE_S  9
/** IO_MUX_GPIO28_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of GPIO28. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define IO_MUX_GPIO28_FUN_DRV    0x00000003U
#define IO_MUX_GPIO28_FUN_DRV_M  (IO_MUX_GPIO28_FUN_DRV_V << IO_MUX_GPIO28_FUN_DRV_S)
#define IO_MUX_GPIO28_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO28_FUN_DRV_S  10
/** IO_MUX_GPIO28_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  Configures to select IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define IO_MUX_GPIO28_MCU_SEL    0x00000007U
#define IO_MUX_GPIO28_MCU_SEL_M  (IO_MUX_GPIO28_MCU_SEL_V << IO_MUX_GPIO28_MCU_SEL_S)
#define IO_MUX_GPIO28_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO28_MCU_SEL_S  12
/** IO_MUX_GPIO28_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO28_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO28_FILTER_EN_M  (IO_MUX_GPIO28_FILTER_EN_V << IO_MUX_GPIO28_FILTER_EN_S)
#define IO_MUX_GPIO28_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO28_FILTER_EN_S  15
/** IO_MUX_GPIO28_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  IO_MUX_GPIO28_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define IO_MUX_GPIO28_HYS_EN    (BIT(16))
#define IO_MUX_GPIO28_HYS_EN_M  (IO_MUX_GPIO28_HYS_EN_V << IO_MUX_GPIO28_HYS_EN_S)
#define IO_MUX_GPIO28_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO28_HYS_EN_S  16
/** IO_MUX_GPIO28_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for GPIO28. \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of IO_MUX_GPIO28_HYS_EN\\
 */
#define IO_MUX_GPIO28_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO28_HYS_SEL_M  (IO_MUX_GPIO28_HYS_SEL_V << IO_MUX_GPIO28_HYS_SEL_S)
#define IO_MUX_GPIO28_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO28_HYS_SEL_S  17

/** IO_MUX_DATE_REG register
 *  Version control register
 */
#define IO_MUX_DATE_REG (DR_REG_IO_MUX_BASE + 0x1fc)
/** IO_MUX_REG_DATE : R/W; bitpos: [27:0]; default: 36770416;
 *  Version control register
 */
#define IO_MUX_REG_DATE    0x0FFFFFFFU
#define IO_MUX_REG_DATE_M  (IO_MUX_REG_DATE_V << IO_MUX_REG_DATE_S)
#define IO_MUX_REG_DATE_V  0x0FFFFFFFU
#define IO_MUX_REG_DATE_S  0

#ifdef __cplusplus
}
#endif
