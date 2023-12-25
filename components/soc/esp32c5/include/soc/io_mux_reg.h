/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

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

#define SPI_HD_GPIO_NUM              28
#define SPI_WP_GPIO_NUM              26
#define SPI_CS0_GPIO_NUM             24
#define SPI_CLK_GPIO_NUM             29
#define SPI_D_GPIO_NUM               30
#define SPI_Q_GPIO_NUM               25

#define SD_CLK_GPIO_NUM              19
#define SD_CMD_GPIO_NUM              18
#define SD_DATA0_GPIO_NUM            20
#define SD_DATA1_GPIO_NUM            21
#define SD_DATA2_GPIO_NUM            22
#define SD_DATA3_GPIO_NUM            23

#define USB_INT_PHY0_DM_GPIO_NUM              12
#define USB_INT_PHY0_DP_GPIO_NUM              13

#define EXT_OSC_SLOW_GPIO_NUM        0

#define MAX_RTC_GPIO_NUM              8
#define MAX_PAD_GPIO_NUM             30
#define MAX_GPIO_NUM                 34
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
#define PERIPHS_IO_MUX_XTAL_32K_P_U           (REG_IO_MUX_BASE + 0x4)
#define FUNC_XTAL_32K_P_GPIO0                                       1
#define FUNC_XTAL_32K_P_GPIO0_0                                     0

#define PERIPHS_IO_MUX_XTAL_32K_N_U           (REG_IO_MUX_BASE + 0x8)
#define FUNC_XTAL_32K_N_GPIO1                                       1
#define FUNC_XTAL_32K_N_GPIO1_0                                     0

#define PERIPHS_IO_MUX_GPIO2_U                (REG_IO_MUX_BASE + 0xC)
#define FUNC_GPIO2_FSPIQ                                            2
#define FUNC_GPIO2_GPIO2                                            1
#define FUNC_GPIO2_GPIO2_0                                          0

#define PERIPHS_IO_MUX_GPIO3_U               (REG_IO_MUX_BASE + 0x10)
#define FUNC_GPIO3_GPIO3                                            1
#define FUNC_GPIO3_GPIO3_0                                          0

#define PERIPHS_IO_MUX_MTMS_U                (REG_IO_MUX_BASE + 0x14)
#define FUNC_MTMS_FSPIHD                                            2
#define FUNC_MTMS_GPIO4                                             1
#define FUNC_MTMS_MTMS                                              0

#define PERIPHS_IO_MUX_MTDI_U                (REG_IO_MUX_BASE + 0x18)
#define FUNC_MTDI_FSPIWP                                            2
#define FUNC_MTDI_GPIO5                                             1
#define FUNC_MTDI_MTDI                                              0

#define PERIPHS_IO_MUX_MTCK_U                (REG_IO_MUX_BASE + 0x1C)
#define FUNC_MTCK_FSPICLK                                           2
#define FUNC_MTCK_GPIO6                                             1
#define FUNC_MTCK_MTCK                                              0

#define PERIPHS_IO_MUX_MTDO_U                (REG_IO_MUX_BASE + 0x20)
#define FUNC_MTDO_FSPID                                             2
#define FUNC_MTDO_GPIO7                                             1
#define FUNC_MTDO_MTDO                                              0

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

#define PERIPHS_IO_MUX_GPIO13_U              (REG_IO_MUX_BASE + 0x38)
#define FUNC_GPIO13_GPIO13                                          1
#define FUNC_GPIO13_GPIO13_0                                        0

#define PERIPHS_IO_MUX_GPIO14_U              (REG_IO_MUX_BASE + 0x3C)
#define FUNC_GPIO14_GPIO14                                          1
#define FUNC_GPIO14_GPIO14_0                                        0

#define PERIPHS_IO_MUX_GPIO15_U              (REG_IO_MUX_BASE + 0x40)
#define FUNC_GPIO15_GPIO15                                          1
#define FUNC_GPIO15_GPIO15_0                                        0

#define PERIPHS_IO_MUX_U0TXD_U               (REG_IO_MUX_BASE + 0x44)
#define FUNC_U0TXD_FSPICS0                                          2
#define FUNC_U0TXD_GPIO16                                           1
#define FUNC_U0TXD_U0TXD                                            0

#define PERIPHS_IO_MUX_U0RXD_U               (REG_IO_MUX_BASE + 0x48)
#define FUNC_U0RXD_FSPICS1                                          2
#define FUNC_U0RXD_GPIO17                                           1
#define FUNC_U0RXD_U0RXD                                            0

#define PERIPHS_IO_MUX_SDIO_CMD_U            (REG_IO_MUX_BASE + 0x4C)
#define FUNC_SDIO_CMD_FSPICS2                                       2
#define FUNC_SDIO_CMD_GPIO18                                        1
#define FUNC_SDIO_CMD_SDIO_CMD                                      0

#define PERIPHS_IO_MUX_SDIO_CLK_U            (REG_IO_MUX_BASE + 0x50)
#define FUNC_SDIO_CLK_FSPICS3                                       2
#define FUNC_SDIO_CLK_GPIO19                                        1
#define FUNC_SDIO_CLK_SDIO_CLK                                      0

#define PERIPHS_IO_MUX_SDIO_DATA0_U          (REG_IO_MUX_BASE + 0x54)
#define FUNC_SDIO_DATA0_FSPICS4                                     2
#define FUNC_SDIO_DATA0_GPIO20                                      1
#define FUNC_SDIO_DATA0_SDIO_DATA0                                  0

#define PERIPHS_IO_MUX_SDIO_DATA1_U          (REG_IO_MUX_BASE + 0x58)
#define FUNC_SDIO_DATA1_FSPICS5                                     2
#define FUNC_SDIO_DATA1_GPIO21                                      1
#define FUNC_SDIO_DATA1_SDIO_DATA1                                  0

#define PERIPHS_IO_MUX_SDIO_DATA2_U          (REG_IO_MUX_BASE + 0x5C)
#define FUNC_SDIO_DATA2_GPIO22                                      1
#define FUNC_SDIO_DATA2_SDIO_DATA2                                  0

#define PERIPHS_IO_MUX_SDIO_DATA3_U          (REG_IO_MUX_BASE + 0x60)
#define FUNC_SDIO_DATA3_GPIO23                                      1
#define FUNC_SDIO_DATA3_SDIO_DATA3                                  0

#define PERIPHS_IO_MUX_SPICS0_U              (REG_IO_MUX_BASE + 0x64)
#define FUNC_SPICS0_GPIO24                                          1
#define FUNC_SPICS0_SPICS0                                          0

#define PERIPHS_IO_MUX_SPIQ_U                (REG_IO_MUX_BASE + 0x68)
#define FUNC_SPIQ_GPIO25                                            1
#define FUNC_SPIQ_SPIQ                                              0

#define PERIPHS_IO_MUX_SPIWP_U               (REG_IO_MUX_BASE + 0x6C)
#define FUNC_SPIWP_GPIO26                                           1
#define FUNC_SPIWP_SPIWP                                            0

#define PERIPHS_IO_MUX_VDD_SPI_U             (REG_IO_MUX_BASE + 0x70)
#define FUNC_VDD_SPI_GPIO27                                         1
#define FUNC_VDD_SPI_GPIO27_0                                       0

#define PERIPHS_IO_MUX_SPIHD_U               (REG_IO_MUX_BASE + 0x74)
#define FUNC_SPIHD_GPIO28                                           1
#define FUNC_SPIHD_SPIHD                                            0

#define PERIPHS_IO_MUX_SPICLK_U              (REG_IO_MUX_BASE + 0x78)
#define FUNC_SPICLK_GPIO29                                          1
#define FUNC_SPICLK_SPICLK                                          0

#define PERIPHS_IO_MUX_SPID_U                (REG_IO_MUX_BASE + 0x7C)
#define FUNC_SPID_GPIO30                                            1
#define FUNC_SPID_SPID                                              0

/** IO_MUX_PIN_CTRL_REG register
 *  Clock Output Configuration Register
 */
#define IO_MUX_PIN_CTRL_REG (DR_REG_IO_MUX_BASE + 0x0)
/** IO_MUX_CLK_OUT1 : R/W; bitpos: [4:0]; default: 15;
 *  If you want to output clock for I2S to CLK_OUT_out1, set this register to 0x0.
 *  CLK_OUT_out1 can be found in peripheral output signals.
 */
#define IO_MUX_CLK_OUT1    0x0000001FU
#define IO_MUX_CLK_OUT1_M  (IO_MUX_CLK_OUT1_V << IO_MUX_CLK_OUT1_S)
#define IO_MUX_CLK_OUT1_V  0x0000001FU
#define IO_MUX_CLK_OUT1_S  0
/** IO_MUX_CLK_OUT2 : R/W; bitpos: [9:5]; default: 15;
 *  If you want to output clock for I2S to CLK_OUT_out2, set this register to 0x0.
 *  CLK_OUT_out2 can be found in peripheral output signals.
 */
#define IO_MUX_CLK_OUT2    0x0000001FU
#define IO_MUX_CLK_OUT2_M  (IO_MUX_CLK_OUT2_V << IO_MUX_CLK_OUT2_S)
#define IO_MUX_CLK_OUT2_V  0x0000001FU
#define IO_MUX_CLK_OUT2_S  5
/** IO_MUX_CLK_OUT3 : R/W; bitpos: [14:10]; default: 7;
 *  If you want to output clock for I2S to CLK_OUT_out3, set this register to 0x0.
 *  CLK_OUT_out3 can be found in peripheral output signals.
 */
#define IO_MUX_CLK_OUT3    0x0000001FU
#define IO_MUX_CLK_OUT3_M  (IO_MUX_CLK_OUT3_V << IO_MUX_CLK_OUT3_S)
#define IO_MUX_CLK_OUT3_V  0x0000001FU
#define IO_MUX_CLK_OUT3_S  10

/** IO_MUX_GPIO0_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO0_REG (DR_REG_IO_MUX_BASE + 0x4)
/** IO_MUX_GPIO0_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO0_MCU_OE    (BIT(0))
#define IO_MUX_GPIO0_MCU_OE_M  (IO_MUX_GPIO0_MCU_OE_V << IO_MUX_GPIO0_MCU_OE_S)
#define IO_MUX_GPIO0_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO0_MCU_OE_S  0
/** IO_MUX_GPIO0_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO0_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO0_SLP_SEL_M  (IO_MUX_GPIO0_SLP_SEL_V << IO_MUX_GPIO0_SLP_SEL_S)
#define IO_MUX_GPIO0_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO0_SLP_SEL_S  1
/** IO_MUX_GPIO0_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO0_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO0_MCU_WPD_M  (IO_MUX_GPIO0_MCU_WPD_V << IO_MUX_GPIO0_MCU_WPD_S)
#define IO_MUX_GPIO0_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO0_MCU_WPD_S  2
/** IO_MUX_GPIO0_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO0_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO0_MCU_WPU_M  (IO_MUX_GPIO0_MCU_WPU_V << IO_MUX_GPIO0_MCU_WPU_S)
#define IO_MUX_GPIO0_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO0_MCU_WPU_S  3
/** IO_MUX_GPIO0_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO0_MCU_IE    (BIT(4))
#define IO_MUX_GPIO0_MCU_IE_M  (IO_MUX_GPIO0_MCU_IE_V << IO_MUX_GPIO0_MCU_IE_S)
#define IO_MUX_GPIO0_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO0_MCU_IE_S  4
/** IO_MUX_GPIO0_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO0_MCU_DRV    0x00000003U
#define IO_MUX_GPIO0_MCU_DRV_M  (IO_MUX_GPIO0_MCU_DRV_V << IO_MUX_GPIO0_MCU_DRV_S)
#define IO_MUX_GPIO0_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO0_MCU_DRV_S  5
/** IO_MUX_GPIO0_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO0_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO0_FUN_WPD_M  (IO_MUX_GPIO0_FUN_WPD_V << IO_MUX_GPIO0_FUN_WPD_S)
#define IO_MUX_GPIO0_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO0_FUN_WPD_S  7
/** IO_MUX_GPIO0_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO0_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO0_FUN_WPU_M  (IO_MUX_GPIO0_FUN_WPU_V << IO_MUX_GPIO0_FUN_WPU_S)
#define IO_MUX_GPIO0_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO0_FUN_WPU_S  8
/** IO_MUX_GPIO0_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO0_FUN_IE    (BIT(9))
#define IO_MUX_GPIO0_FUN_IE_M  (IO_MUX_GPIO0_FUN_IE_V << IO_MUX_GPIO0_FUN_IE_S)
#define IO_MUX_GPIO0_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO0_FUN_IE_S  9
/** IO_MUX_GPIO0_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO0_FUN_DRV    0x00000003U
#define IO_MUX_GPIO0_FUN_DRV_M  (IO_MUX_GPIO0_FUN_DRV_V << IO_MUX_GPIO0_FUN_DRV_S)
#define IO_MUX_GPIO0_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO0_FUN_DRV_S  10
/** IO_MUX_GPIO0_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO0_MCU_SEL    0x00000007U
#define IO_MUX_GPIO0_MCU_SEL_M  (IO_MUX_GPIO0_MCU_SEL_V << IO_MUX_GPIO0_MCU_SEL_S)
#define IO_MUX_GPIO0_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO0_MCU_SEL_S  12
/** IO_MUX_GPIO0_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO0_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO0_FILTER_EN_M  (IO_MUX_GPIO0_FILTER_EN_V << IO_MUX_GPIO0_FILTER_EN_S)
#define IO_MUX_GPIO0_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO0_FILTER_EN_S  15
/** IO_MUX_GPIO0_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO0_HYS_EN    (BIT(16))
#define IO_MUX_GPIO0_HYS_EN_M  (IO_MUX_GPIO0_HYS_EN_V << IO_MUX_GPIO0_HYS_EN_S)
#define IO_MUX_GPIO0_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO0_HYS_EN_S  16
/** IO_MUX_GPIO0_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO0_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO0_HYS_SEL_M  (IO_MUX_GPIO0_HYS_SEL_V << IO_MUX_GPIO0_HYS_SEL_S)
#define IO_MUX_GPIO0_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO0_HYS_SEL_S  17

/** IO_MUX_GPIO1_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO1_REG (DR_REG_IO_MUX_BASE + 0x8)
/** IO_MUX_GPIO1_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO1_MCU_OE    (BIT(0))
#define IO_MUX_GPIO1_MCU_OE_M  (IO_MUX_GPIO1_MCU_OE_V << IO_MUX_GPIO1_MCU_OE_S)
#define IO_MUX_GPIO1_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO1_MCU_OE_S  0
/** IO_MUX_GPIO1_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO1_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO1_SLP_SEL_M  (IO_MUX_GPIO1_SLP_SEL_V << IO_MUX_GPIO1_SLP_SEL_S)
#define IO_MUX_GPIO1_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO1_SLP_SEL_S  1
/** IO_MUX_GPIO1_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO1_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO1_MCU_WPD_M  (IO_MUX_GPIO1_MCU_WPD_V << IO_MUX_GPIO1_MCU_WPD_S)
#define IO_MUX_GPIO1_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO1_MCU_WPD_S  2
/** IO_MUX_GPIO1_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO1_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO1_MCU_WPU_M  (IO_MUX_GPIO1_MCU_WPU_V << IO_MUX_GPIO1_MCU_WPU_S)
#define IO_MUX_GPIO1_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO1_MCU_WPU_S  3
/** IO_MUX_GPIO1_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO1_MCU_IE    (BIT(4))
#define IO_MUX_GPIO1_MCU_IE_M  (IO_MUX_GPIO1_MCU_IE_V << IO_MUX_GPIO1_MCU_IE_S)
#define IO_MUX_GPIO1_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO1_MCU_IE_S  4
/** IO_MUX_GPIO1_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO1_MCU_DRV    0x00000003U
#define IO_MUX_GPIO1_MCU_DRV_M  (IO_MUX_GPIO1_MCU_DRV_V << IO_MUX_GPIO1_MCU_DRV_S)
#define IO_MUX_GPIO1_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO1_MCU_DRV_S  5
/** IO_MUX_GPIO1_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO1_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO1_FUN_WPD_M  (IO_MUX_GPIO1_FUN_WPD_V << IO_MUX_GPIO1_FUN_WPD_S)
#define IO_MUX_GPIO1_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO1_FUN_WPD_S  7
/** IO_MUX_GPIO1_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO1_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO1_FUN_WPU_M  (IO_MUX_GPIO1_FUN_WPU_V << IO_MUX_GPIO1_FUN_WPU_S)
#define IO_MUX_GPIO1_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO1_FUN_WPU_S  8
/** IO_MUX_GPIO1_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO1_FUN_IE    (BIT(9))
#define IO_MUX_GPIO1_FUN_IE_M  (IO_MUX_GPIO1_FUN_IE_V << IO_MUX_GPIO1_FUN_IE_S)
#define IO_MUX_GPIO1_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO1_FUN_IE_S  9
/** IO_MUX_GPIO1_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO1_FUN_DRV    0x00000003U
#define IO_MUX_GPIO1_FUN_DRV_M  (IO_MUX_GPIO1_FUN_DRV_V << IO_MUX_GPIO1_FUN_DRV_S)
#define IO_MUX_GPIO1_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO1_FUN_DRV_S  10
/** IO_MUX_GPIO1_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO1_MCU_SEL    0x00000007U
#define IO_MUX_GPIO1_MCU_SEL_M  (IO_MUX_GPIO1_MCU_SEL_V << IO_MUX_GPIO1_MCU_SEL_S)
#define IO_MUX_GPIO1_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO1_MCU_SEL_S  12
/** IO_MUX_GPIO1_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO1_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO1_FILTER_EN_M  (IO_MUX_GPIO1_FILTER_EN_V << IO_MUX_GPIO1_FILTER_EN_S)
#define IO_MUX_GPIO1_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO1_FILTER_EN_S  15
/** IO_MUX_GPIO1_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO1_HYS_EN    (BIT(16))
#define IO_MUX_GPIO1_HYS_EN_M  (IO_MUX_GPIO1_HYS_EN_V << IO_MUX_GPIO1_HYS_EN_S)
#define IO_MUX_GPIO1_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO1_HYS_EN_S  16
/** IO_MUX_GPIO1_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO1_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO1_HYS_SEL_M  (IO_MUX_GPIO1_HYS_SEL_V << IO_MUX_GPIO1_HYS_SEL_S)
#define IO_MUX_GPIO1_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO1_HYS_SEL_S  17

/** IO_MUX_GPIO2_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO2_REG (DR_REG_IO_MUX_BASE + 0xc)
/** IO_MUX_GPIO2_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO2_MCU_OE    (BIT(0))
#define IO_MUX_GPIO2_MCU_OE_M  (IO_MUX_GPIO2_MCU_OE_V << IO_MUX_GPIO2_MCU_OE_S)
#define IO_MUX_GPIO2_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO2_MCU_OE_S  0
/** IO_MUX_GPIO2_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO2_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO2_SLP_SEL_M  (IO_MUX_GPIO2_SLP_SEL_V << IO_MUX_GPIO2_SLP_SEL_S)
#define IO_MUX_GPIO2_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO2_SLP_SEL_S  1
/** IO_MUX_GPIO2_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO2_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO2_MCU_WPD_M  (IO_MUX_GPIO2_MCU_WPD_V << IO_MUX_GPIO2_MCU_WPD_S)
#define IO_MUX_GPIO2_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO2_MCU_WPD_S  2
/** IO_MUX_GPIO2_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO2_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO2_MCU_WPU_M  (IO_MUX_GPIO2_MCU_WPU_V << IO_MUX_GPIO2_MCU_WPU_S)
#define IO_MUX_GPIO2_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO2_MCU_WPU_S  3
/** IO_MUX_GPIO2_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO2_MCU_IE    (BIT(4))
#define IO_MUX_GPIO2_MCU_IE_M  (IO_MUX_GPIO2_MCU_IE_V << IO_MUX_GPIO2_MCU_IE_S)
#define IO_MUX_GPIO2_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO2_MCU_IE_S  4
/** IO_MUX_GPIO2_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO2_MCU_DRV    0x00000003U
#define IO_MUX_GPIO2_MCU_DRV_M  (IO_MUX_GPIO2_MCU_DRV_V << IO_MUX_GPIO2_MCU_DRV_S)
#define IO_MUX_GPIO2_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO2_MCU_DRV_S  5
/** IO_MUX_GPIO2_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO2_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO2_FUN_WPD_M  (IO_MUX_GPIO2_FUN_WPD_V << IO_MUX_GPIO2_FUN_WPD_S)
#define IO_MUX_GPIO2_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO2_FUN_WPD_S  7
/** IO_MUX_GPIO2_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO2_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO2_FUN_WPU_M  (IO_MUX_GPIO2_FUN_WPU_V << IO_MUX_GPIO2_FUN_WPU_S)
#define IO_MUX_GPIO2_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO2_FUN_WPU_S  8
/** IO_MUX_GPIO2_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO2_FUN_IE    (BIT(9))
#define IO_MUX_GPIO2_FUN_IE_M  (IO_MUX_GPIO2_FUN_IE_V << IO_MUX_GPIO2_FUN_IE_S)
#define IO_MUX_GPIO2_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO2_FUN_IE_S  9
/** IO_MUX_GPIO2_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO2_FUN_DRV    0x00000003U
#define IO_MUX_GPIO2_FUN_DRV_M  (IO_MUX_GPIO2_FUN_DRV_V << IO_MUX_GPIO2_FUN_DRV_S)
#define IO_MUX_GPIO2_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO2_FUN_DRV_S  10
/** IO_MUX_GPIO2_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO2_MCU_SEL    0x00000007U
#define IO_MUX_GPIO2_MCU_SEL_M  (IO_MUX_GPIO2_MCU_SEL_V << IO_MUX_GPIO2_MCU_SEL_S)
#define IO_MUX_GPIO2_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO2_MCU_SEL_S  12
/** IO_MUX_GPIO2_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO2_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO2_FILTER_EN_M  (IO_MUX_GPIO2_FILTER_EN_V << IO_MUX_GPIO2_FILTER_EN_S)
#define IO_MUX_GPIO2_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO2_FILTER_EN_S  15
/** IO_MUX_GPIO2_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO2_HYS_EN    (BIT(16))
#define IO_MUX_GPIO2_HYS_EN_M  (IO_MUX_GPIO2_HYS_EN_V << IO_MUX_GPIO2_HYS_EN_S)
#define IO_MUX_GPIO2_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO2_HYS_EN_S  16
/** IO_MUX_GPIO2_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO2_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO2_HYS_SEL_M  (IO_MUX_GPIO2_HYS_SEL_V << IO_MUX_GPIO2_HYS_SEL_S)
#define IO_MUX_GPIO2_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO2_HYS_SEL_S  17

/** IO_MUX_GPIO3_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO3_REG (DR_REG_IO_MUX_BASE + 0x10)
/** IO_MUX_GPIO3_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO3_MCU_OE    (BIT(0))
#define IO_MUX_GPIO3_MCU_OE_M  (IO_MUX_GPIO3_MCU_OE_V << IO_MUX_GPIO3_MCU_OE_S)
#define IO_MUX_GPIO3_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO3_MCU_OE_S  0
/** IO_MUX_GPIO3_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO3_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO3_SLP_SEL_M  (IO_MUX_GPIO3_SLP_SEL_V << IO_MUX_GPIO3_SLP_SEL_S)
#define IO_MUX_GPIO3_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO3_SLP_SEL_S  1
/** IO_MUX_GPIO3_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO3_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO3_MCU_WPD_M  (IO_MUX_GPIO3_MCU_WPD_V << IO_MUX_GPIO3_MCU_WPD_S)
#define IO_MUX_GPIO3_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO3_MCU_WPD_S  2
/** IO_MUX_GPIO3_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO3_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO3_MCU_WPU_M  (IO_MUX_GPIO3_MCU_WPU_V << IO_MUX_GPIO3_MCU_WPU_S)
#define IO_MUX_GPIO3_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO3_MCU_WPU_S  3
/** IO_MUX_GPIO3_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO3_MCU_IE    (BIT(4))
#define IO_MUX_GPIO3_MCU_IE_M  (IO_MUX_GPIO3_MCU_IE_V << IO_MUX_GPIO3_MCU_IE_S)
#define IO_MUX_GPIO3_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO3_MCU_IE_S  4
/** IO_MUX_GPIO3_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO3_MCU_DRV    0x00000003U
#define IO_MUX_GPIO3_MCU_DRV_M  (IO_MUX_GPIO3_MCU_DRV_V << IO_MUX_GPIO3_MCU_DRV_S)
#define IO_MUX_GPIO3_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO3_MCU_DRV_S  5
/** IO_MUX_GPIO3_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO3_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO3_FUN_WPD_M  (IO_MUX_GPIO3_FUN_WPD_V << IO_MUX_GPIO3_FUN_WPD_S)
#define IO_MUX_GPIO3_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO3_FUN_WPD_S  7
/** IO_MUX_GPIO3_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO3_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO3_FUN_WPU_M  (IO_MUX_GPIO3_FUN_WPU_V << IO_MUX_GPIO3_FUN_WPU_S)
#define IO_MUX_GPIO3_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO3_FUN_WPU_S  8
/** IO_MUX_GPIO3_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO3_FUN_IE    (BIT(9))
#define IO_MUX_GPIO3_FUN_IE_M  (IO_MUX_GPIO3_FUN_IE_V << IO_MUX_GPIO3_FUN_IE_S)
#define IO_MUX_GPIO3_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO3_FUN_IE_S  9
/** IO_MUX_GPIO3_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO3_FUN_DRV    0x00000003U
#define IO_MUX_GPIO3_FUN_DRV_M  (IO_MUX_GPIO3_FUN_DRV_V << IO_MUX_GPIO3_FUN_DRV_S)
#define IO_MUX_GPIO3_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO3_FUN_DRV_S  10
/** IO_MUX_GPIO3_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO3_MCU_SEL    0x00000007U
#define IO_MUX_GPIO3_MCU_SEL_M  (IO_MUX_GPIO3_MCU_SEL_V << IO_MUX_GPIO3_MCU_SEL_S)
#define IO_MUX_GPIO3_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO3_MCU_SEL_S  12
/** IO_MUX_GPIO3_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO3_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO3_FILTER_EN_M  (IO_MUX_GPIO3_FILTER_EN_V << IO_MUX_GPIO3_FILTER_EN_S)
#define IO_MUX_GPIO3_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO3_FILTER_EN_S  15
/** IO_MUX_GPIO3_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO3_HYS_EN    (BIT(16))
#define IO_MUX_GPIO3_HYS_EN_M  (IO_MUX_GPIO3_HYS_EN_V << IO_MUX_GPIO3_HYS_EN_S)
#define IO_MUX_GPIO3_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO3_HYS_EN_S  16
/** IO_MUX_GPIO3_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO3_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO3_HYS_SEL_M  (IO_MUX_GPIO3_HYS_SEL_V << IO_MUX_GPIO3_HYS_SEL_S)
#define IO_MUX_GPIO3_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO3_HYS_SEL_S  17

/** IO_MUX_GPIO4_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO4_REG (DR_REG_IO_MUX_BASE + 0x14)
/** IO_MUX_GPIO4_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO4_MCU_OE    (BIT(0))
#define IO_MUX_GPIO4_MCU_OE_M  (IO_MUX_GPIO4_MCU_OE_V << IO_MUX_GPIO4_MCU_OE_S)
#define IO_MUX_GPIO4_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO4_MCU_OE_S  0
/** IO_MUX_GPIO4_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO4_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO4_SLP_SEL_M  (IO_MUX_GPIO4_SLP_SEL_V << IO_MUX_GPIO4_SLP_SEL_S)
#define IO_MUX_GPIO4_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO4_SLP_SEL_S  1
/** IO_MUX_GPIO4_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO4_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO4_MCU_WPD_M  (IO_MUX_GPIO4_MCU_WPD_V << IO_MUX_GPIO4_MCU_WPD_S)
#define IO_MUX_GPIO4_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO4_MCU_WPD_S  2
/** IO_MUX_GPIO4_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO4_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO4_MCU_WPU_M  (IO_MUX_GPIO4_MCU_WPU_V << IO_MUX_GPIO4_MCU_WPU_S)
#define IO_MUX_GPIO4_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO4_MCU_WPU_S  3
/** IO_MUX_GPIO4_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO4_MCU_IE    (BIT(4))
#define IO_MUX_GPIO4_MCU_IE_M  (IO_MUX_GPIO4_MCU_IE_V << IO_MUX_GPIO4_MCU_IE_S)
#define IO_MUX_GPIO4_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO4_MCU_IE_S  4
/** IO_MUX_GPIO4_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO4_MCU_DRV    0x00000003U
#define IO_MUX_GPIO4_MCU_DRV_M  (IO_MUX_GPIO4_MCU_DRV_V << IO_MUX_GPIO4_MCU_DRV_S)
#define IO_MUX_GPIO4_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO4_MCU_DRV_S  5
/** IO_MUX_GPIO4_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO4_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO4_FUN_WPD_M  (IO_MUX_GPIO4_FUN_WPD_V << IO_MUX_GPIO4_FUN_WPD_S)
#define IO_MUX_GPIO4_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO4_FUN_WPD_S  7
/** IO_MUX_GPIO4_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO4_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO4_FUN_WPU_M  (IO_MUX_GPIO4_FUN_WPU_V << IO_MUX_GPIO4_FUN_WPU_S)
#define IO_MUX_GPIO4_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO4_FUN_WPU_S  8
/** IO_MUX_GPIO4_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO4_FUN_IE    (BIT(9))
#define IO_MUX_GPIO4_FUN_IE_M  (IO_MUX_GPIO4_FUN_IE_V << IO_MUX_GPIO4_FUN_IE_S)
#define IO_MUX_GPIO4_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO4_FUN_IE_S  9
/** IO_MUX_GPIO4_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO4_FUN_DRV    0x00000003U
#define IO_MUX_GPIO4_FUN_DRV_M  (IO_MUX_GPIO4_FUN_DRV_V << IO_MUX_GPIO4_FUN_DRV_S)
#define IO_MUX_GPIO4_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO4_FUN_DRV_S  10
/** IO_MUX_GPIO4_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO4_MCU_SEL    0x00000007U
#define IO_MUX_GPIO4_MCU_SEL_M  (IO_MUX_GPIO4_MCU_SEL_V << IO_MUX_GPIO4_MCU_SEL_S)
#define IO_MUX_GPIO4_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO4_MCU_SEL_S  12
/** IO_MUX_GPIO4_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO4_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO4_FILTER_EN_M  (IO_MUX_GPIO4_FILTER_EN_V << IO_MUX_GPIO4_FILTER_EN_S)
#define IO_MUX_GPIO4_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO4_FILTER_EN_S  15
/** IO_MUX_GPIO4_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO4_HYS_EN    (BIT(16))
#define IO_MUX_GPIO4_HYS_EN_M  (IO_MUX_GPIO4_HYS_EN_V << IO_MUX_GPIO4_HYS_EN_S)
#define IO_MUX_GPIO4_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO4_HYS_EN_S  16
/** IO_MUX_GPIO4_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO4_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO4_HYS_SEL_M  (IO_MUX_GPIO4_HYS_SEL_V << IO_MUX_GPIO4_HYS_SEL_S)
#define IO_MUX_GPIO4_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO4_HYS_SEL_S  17

/** IO_MUX_GPIO5_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO5_REG (DR_REG_IO_MUX_BASE + 0x18)
/** IO_MUX_GPIO5_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO5_MCU_OE    (BIT(0))
#define IO_MUX_GPIO5_MCU_OE_M  (IO_MUX_GPIO5_MCU_OE_V << IO_MUX_GPIO5_MCU_OE_S)
#define IO_MUX_GPIO5_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO5_MCU_OE_S  0
/** IO_MUX_GPIO5_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO5_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO5_SLP_SEL_M  (IO_MUX_GPIO5_SLP_SEL_V << IO_MUX_GPIO5_SLP_SEL_S)
#define IO_MUX_GPIO5_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO5_SLP_SEL_S  1
/** IO_MUX_GPIO5_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO5_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO5_MCU_WPD_M  (IO_MUX_GPIO5_MCU_WPD_V << IO_MUX_GPIO5_MCU_WPD_S)
#define IO_MUX_GPIO5_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO5_MCU_WPD_S  2
/** IO_MUX_GPIO5_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO5_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO5_MCU_WPU_M  (IO_MUX_GPIO5_MCU_WPU_V << IO_MUX_GPIO5_MCU_WPU_S)
#define IO_MUX_GPIO5_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO5_MCU_WPU_S  3
/** IO_MUX_GPIO5_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO5_MCU_IE    (BIT(4))
#define IO_MUX_GPIO5_MCU_IE_M  (IO_MUX_GPIO5_MCU_IE_V << IO_MUX_GPIO5_MCU_IE_S)
#define IO_MUX_GPIO5_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO5_MCU_IE_S  4
/** IO_MUX_GPIO5_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO5_MCU_DRV    0x00000003U
#define IO_MUX_GPIO5_MCU_DRV_M  (IO_MUX_GPIO5_MCU_DRV_V << IO_MUX_GPIO5_MCU_DRV_S)
#define IO_MUX_GPIO5_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO5_MCU_DRV_S  5
/** IO_MUX_GPIO5_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO5_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO5_FUN_WPD_M  (IO_MUX_GPIO5_FUN_WPD_V << IO_MUX_GPIO5_FUN_WPD_S)
#define IO_MUX_GPIO5_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO5_FUN_WPD_S  7
/** IO_MUX_GPIO5_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO5_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO5_FUN_WPU_M  (IO_MUX_GPIO5_FUN_WPU_V << IO_MUX_GPIO5_FUN_WPU_S)
#define IO_MUX_GPIO5_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO5_FUN_WPU_S  8
/** IO_MUX_GPIO5_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO5_FUN_IE    (BIT(9))
#define IO_MUX_GPIO5_FUN_IE_M  (IO_MUX_GPIO5_FUN_IE_V << IO_MUX_GPIO5_FUN_IE_S)
#define IO_MUX_GPIO5_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO5_FUN_IE_S  9
/** IO_MUX_GPIO5_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO5_FUN_DRV    0x00000003U
#define IO_MUX_GPIO5_FUN_DRV_M  (IO_MUX_GPIO5_FUN_DRV_V << IO_MUX_GPIO5_FUN_DRV_S)
#define IO_MUX_GPIO5_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO5_FUN_DRV_S  10
/** IO_MUX_GPIO5_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO5_MCU_SEL    0x00000007U
#define IO_MUX_GPIO5_MCU_SEL_M  (IO_MUX_GPIO5_MCU_SEL_V << IO_MUX_GPIO5_MCU_SEL_S)
#define IO_MUX_GPIO5_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO5_MCU_SEL_S  12
/** IO_MUX_GPIO5_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO5_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO5_FILTER_EN_M  (IO_MUX_GPIO5_FILTER_EN_V << IO_MUX_GPIO5_FILTER_EN_S)
#define IO_MUX_GPIO5_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO5_FILTER_EN_S  15
/** IO_MUX_GPIO5_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO5_HYS_EN    (BIT(16))
#define IO_MUX_GPIO5_HYS_EN_M  (IO_MUX_GPIO5_HYS_EN_V << IO_MUX_GPIO5_HYS_EN_S)
#define IO_MUX_GPIO5_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO5_HYS_EN_S  16
/** IO_MUX_GPIO5_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO5_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO5_HYS_SEL_M  (IO_MUX_GPIO5_HYS_SEL_V << IO_MUX_GPIO5_HYS_SEL_S)
#define IO_MUX_GPIO5_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO5_HYS_SEL_S  17

/** IO_MUX_GPIO6_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO6_REG (DR_REG_IO_MUX_BASE + 0x1c)
/** IO_MUX_GPIO6_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO6_MCU_OE    (BIT(0))
#define IO_MUX_GPIO6_MCU_OE_M  (IO_MUX_GPIO6_MCU_OE_V << IO_MUX_GPIO6_MCU_OE_S)
#define IO_MUX_GPIO6_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO6_MCU_OE_S  0
/** IO_MUX_GPIO6_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO6_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO6_SLP_SEL_M  (IO_MUX_GPIO6_SLP_SEL_V << IO_MUX_GPIO6_SLP_SEL_S)
#define IO_MUX_GPIO6_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO6_SLP_SEL_S  1
/** IO_MUX_GPIO6_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO6_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO6_MCU_WPD_M  (IO_MUX_GPIO6_MCU_WPD_V << IO_MUX_GPIO6_MCU_WPD_S)
#define IO_MUX_GPIO6_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO6_MCU_WPD_S  2
/** IO_MUX_GPIO6_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO6_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO6_MCU_WPU_M  (IO_MUX_GPIO6_MCU_WPU_V << IO_MUX_GPIO6_MCU_WPU_S)
#define IO_MUX_GPIO6_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO6_MCU_WPU_S  3
/** IO_MUX_GPIO6_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO6_MCU_IE    (BIT(4))
#define IO_MUX_GPIO6_MCU_IE_M  (IO_MUX_GPIO6_MCU_IE_V << IO_MUX_GPIO6_MCU_IE_S)
#define IO_MUX_GPIO6_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO6_MCU_IE_S  4
/** IO_MUX_GPIO6_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO6_MCU_DRV    0x00000003U
#define IO_MUX_GPIO6_MCU_DRV_M  (IO_MUX_GPIO6_MCU_DRV_V << IO_MUX_GPIO6_MCU_DRV_S)
#define IO_MUX_GPIO6_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO6_MCU_DRV_S  5
/** IO_MUX_GPIO6_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO6_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO6_FUN_WPD_M  (IO_MUX_GPIO6_FUN_WPD_V << IO_MUX_GPIO6_FUN_WPD_S)
#define IO_MUX_GPIO6_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO6_FUN_WPD_S  7
/** IO_MUX_GPIO6_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO6_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO6_FUN_WPU_M  (IO_MUX_GPIO6_FUN_WPU_V << IO_MUX_GPIO6_FUN_WPU_S)
#define IO_MUX_GPIO6_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO6_FUN_WPU_S  8
/** IO_MUX_GPIO6_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO6_FUN_IE    (BIT(9))
#define IO_MUX_GPIO6_FUN_IE_M  (IO_MUX_GPIO6_FUN_IE_V << IO_MUX_GPIO6_FUN_IE_S)
#define IO_MUX_GPIO6_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO6_FUN_IE_S  9
/** IO_MUX_GPIO6_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO6_FUN_DRV    0x00000003U
#define IO_MUX_GPIO6_FUN_DRV_M  (IO_MUX_GPIO6_FUN_DRV_V << IO_MUX_GPIO6_FUN_DRV_S)
#define IO_MUX_GPIO6_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO6_FUN_DRV_S  10
/** IO_MUX_GPIO6_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO6_MCU_SEL    0x00000007U
#define IO_MUX_GPIO6_MCU_SEL_M  (IO_MUX_GPIO6_MCU_SEL_V << IO_MUX_GPIO6_MCU_SEL_S)
#define IO_MUX_GPIO6_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO6_MCU_SEL_S  12
/** IO_MUX_GPIO6_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO6_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO6_FILTER_EN_M  (IO_MUX_GPIO6_FILTER_EN_V << IO_MUX_GPIO6_FILTER_EN_S)
#define IO_MUX_GPIO6_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO6_FILTER_EN_S  15
/** IO_MUX_GPIO6_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO6_HYS_EN    (BIT(16))
#define IO_MUX_GPIO6_HYS_EN_M  (IO_MUX_GPIO6_HYS_EN_V << IO_MUX_GPIO6_HYS_EN_S)
#define IO_MUX_GPIO6_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO6_HYS_EN_S  16
/** IO_MUX_GPIO6_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO6_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO6_HYS_SEL_M  (IO_MUX_GPIO6_HYS_SEL_V << IO_MUX_GPIO6_HYS_SEL_S)
#define IO_MUX_GPIO6_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO6_HYS_SEL_S  17

/** IO_MUX_GPIO7_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO7_REG (DR_REG_IO_MUX_BASE + 0x20)
/** IO_MUX_GPIO7_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO7_MCU_OE    (BIT(0))
#define IO_MUX_GPIO7_MCU_OE_M  (IO_MUX_GPIO7_MCU_OE_V << IO_MUX_GPIO7_MCU_OE_S)
#define IO_MUX_GPIO7_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO7_MCU_OE_S  0
/** IO_MUX_GPIO7_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO7_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO7_SLP_SEL_M  (IO_MUX_GPIO7_SLP_SEL_V << IO_MUX_GPIO7_SLP_SEL_S)
#define IO_MUX_GPIO7_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO7_SLP_SEL_S  1
/** IO_MUX_GPIO7_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO7_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO7_MCU_WPD_M  (IO_MUX_GPIO7_MCU_WPD_V << IO_MUX_GPIO7_MCU_WPD_S)
#define IO_MUX_GPIO7_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO7_MCU_WPD_S  2
/** IO_MUX_GPIO7_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO7_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO7_MCU_WPU_M  (IO_MUX_GPIO7_MCU_WPU_V << IO_MUX_GPIO7_MCU_WPU_S)
#define IO_MUX_GPIO7_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO7_MCU_WPU_S  3
/** IO_MUX_GPIO7_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO7_MCU_IE    (BIT(4))
#define IO_MUX_GPIO7_MCU_IE_M  (IO_MUX_GPIO7_MCU_IE_V << IO_MUX_GPIO7_MCU_IE_S)
#define IO_MUX_GPIO7_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO7_MCU_IE_S  4
/** IO_MUX_GPIO7_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO7_MCU_DRV    0x00000003U
#define IO_MUX_GPIO7_MCU_DRV_M  (IO_MUX_GPIO7_MCU_DRV_V << IO_MUX_GPIO7_MCU_DRV_S)
#define IO_MUX_GPIO7_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO7_MCU_DRV_S  5
/** IO_MUX_GPIO7_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO7_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO7_FUN_WPD_M  (IO_MUX_GPIO7_FUN_WPD_V << IO_MUX_GPIO7_FUN_WPD_S)
#define IO_MUX_GPIO7_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO7_FUN_WPD_S  7
/** IO_MUX_GPIO7_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO7_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO7_FUN_WPU_M  (IO_MUX_GPIO7_FUN_WPU_V << IO_MUX_GPIO7_FUN_WPU_S)
#define IO_MUX_GPIO7_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO7_FUN_WPU_S  8
/** IO_MUX_GPIO7_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO7_FUN_IE    (BIT(9))
#define IO_MUX_GPIO7_FUN_IE_M  (IO_MUX_GPIO7_FUN_IE_V << IO_MUX_GPIO7_FUN_IE_S)
#define IO_MUX_GPIO7_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO7_FUN_IE_S  9
/** IO_MUX_GPIO7_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO7_FUN_DRV    0x00000003U
#define IO_MUX_GPIO7_FUN_DRV_M  (IO_MUX_GPIO7_FUN_DRV_V << IO_MUX_GPIO7_FUN_DRV_S)
#define IO_MUX_GPIO7_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO7_FUN_DRV_S  10
/** IO_MUX_GPIO7_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO7_MCU_SEL    0x00000007U
#define IO_MUX_GPIO7_MCU_SEL_M  (IO_MUX_GPIO7_MCU_SEL_V << IO_MUX_GPIO7_MCU_SEL_S)
#define IO_MUX_GPIO7_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO7_MCU_SEL_S  12
/** IO_MUX_GPIO7_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO7_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO7_FILTER_EN_M  (IO_MUX_GPIO7_FILTER_EN_V << IO_MUX_GPIO7_FILTER_EN_S)
#define IO_MUX_GPIO7_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO7_FILTER_EN_S  15
/** IO_MUX_GPIO7_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO7_HYS_EN    (BIT(16))
#define IO_MUX_GPIO7_HYS_EN_M  (IO_MUX_GPIO7_HYS_EN_V << IO_MUX_GPIO7_HYS_EN_S)
#define IO_MUX_GPIO7_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO7_HYS_EN_S  16
/** IO_MUX_GPIO7_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO7_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO7_HYS_SEL_M  (IO_MUX_GPIO7_HYS_SEL_V << IO_MUX_GPIO7_HYS_SEL_S)
#define IO_MUX_GPIO7_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO7_HYS_SEL_S  17

/** IO_MUX_GPIO8_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO8_REG (DR_REG_IO_MUX_BASE + 0x24)
/** IO_MUX_GPIO8_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO8_MCU_OE    (BIT(0))
#define IO_MUX_GPIO8_MCU_OE_M  (IO_MUX_GPIO8_MCU_OE_V << IO_MUX_GPIO8_MCU_OE_S)
#define IO_MUX_GPIO8_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO8_MCU_OE_S  0
/** IO_MUX_GPIO8_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO8_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO8_SLP_SEL_M  (IO_MUX_GPIO8_SLP_SEL_V << IO_MUX_GPIO8_SLP_SEL_S)
#define IO_MUX_GPIO8_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO8_SLP_SEL_S  1
/** IO_MUX_GPIO8_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO8_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO8_MCU_WPD_M  (IO_MUX_GPIO8_MCU_WPD_V << IO_MUX_GPIO8_MCU_WPD_S)
#define IO_MUX_GPIO8_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO8_MCU_WPD_S  2
/** IO_MUX_GPIO8_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO8_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO8_MCU_WPU_M  (IO_MUX_GPIO8_MCU_WPU_V << IO_MUX_GPIO8_MCU_WPU_S)
#define IO_MUX_GPIO8_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO8_MCU_WPU_S  3
/** IO_MUX_GPIO8_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO8_MCU_IE    (BIT(4))
#define IO_MUX_GPIO8_MCU_IE_M  (IO_MUX_GPIO8_MCU_IE_V << IO_MUX_GPIO8_MCU_IE_S)
#define IO_MUX_GPIO8_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO8_MCU_IE_S  4
/** IO_MUX_GPIO8_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO8_MCU_DRV    0x00000003U
#define IO_MUX_GPIO8_MCU_DRV_M  (IO_MUX_GPIO8_MCU_DRV_V << IO_MUX_GPIO8_MCU_DRV_S)
#define IO_MUX_GPIO8_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO8_MCU_DRV_S  5
/** IO_MUX_GPIO8_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO8_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO8_FUN_WPD_M  (IO_MUX_GPIO8_FUN_WPD_V << IO_MUX_GPIO8_FUN_WPD_S)
#define IO_MUX_GPIO8_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO8_FUN_WPD_S  7
/** IO_MUX_GPIO8_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO8_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO8_FUN_WPU_M  (IO_MUX_GPIO8_FUN_WPU_V << IO_MUX_GPIO8_FUN_WPU_S)
#define IO_MUX_GPIO8_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO8_FUN_WPU_S  8
/** IO_MUX_GPIO8_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO8_FUN_IE    (BIT(9))
#define IO_MUX_GPIO8_FUN_IE_M  (IO_MUX_GPIO8_FUN_IE_V << IO_MUX_GPIO8_FUN_IE_S)
#define IO_MUX_GPIO8_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO8_FUN_IE_S  9
/** IO_MUX_GPIO8_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO8_FUN_DRV    0x00000003U
#define IO_MUX_GPIO8_FUN_DRV_M  (IO_MUX_GPIO8_FUN_DRV_V << IO_MUX_GPIO8_FUN_DRV_S)
#define IO_MUX_GPIO8_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO8_FUN_DRV_S  10
/** IO_MUX_GPIO8_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO8_MCU_SEL    0x00000007U
#define IO_MUX_GPIO8_MCU_SEL_M  (IO_MUX_GPIO8_MCU_SEL_V << IO_MUX_GPIO8_MCU_SEL_S)
#define IO_MUX_GPIO8_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO8_MCU_SEL_S  12
/** IO_MUX_GPIO8_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO8_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO8_FILTER_EN_M  (IO_MUX_GPIO8_FILTER_EN_V << IO_MUX_GPIO8_FILTER_EN_S)
#define IO_MUX_GPIO8_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO8_FILTER_EN_S  15
/** IO_MUX_GPIO8_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO8_HYS_EN    (BIT(16))
#define IO_MUX_GPIO8_HYS_EN_M  (IO_MUX_GPIO8_HYS_EN_V << IO_MUX_GPIO8_HYS_EN_S)
#define IO_MUX_GPIO8_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO8_HYS_EN_S  16
/** IO_MUX_GPIO8_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO8_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO8_HYS_SEL_M  (IO_MUX_GPIO8_HYS_SEL_V << IO_MUX_GPIO8_HYS_SEL_S)
#define IO_MUX_GPIO8_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO8_HYS_SEL_S  17

/** IO_MUX_GPIO9_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO9_REG (DR_REG_IO_MUX_BASE + 0x28)
/** IO_MUX_GPIO9_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO9_MCU_OE    (BIT(0))
#define IO_MUX_GPIO9_MCU_OE_M  (IO_MUX_GPIO9_MCU_OE_V << IO_MUX_GPIO9_MCU_OE_S)
#define IO_MUX_GPIO9_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO9_MCU_OE_S  0
/** IO_MUX_GPIO9_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO9_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO9_SLP_SEL_M  (IO_MUX_GPIO9_SLP_SEL_V << IO_MUX_GPIO9_SLP_SEL_S)
#define IO_MUX_GPIO9_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO9_SLP_SEL_S  1
/** IO_MUX_GPIO9_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO9_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO9_MCU_WPD_M  (IO_MUX_GPIO9_MCU_WPD_V << IO_MUX_GPIO9_MCU_WPD_S)
#define IO_MUX_GPIO9_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO9_MCU_WPD_S  2
/** IO_MUX_GPIO9_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO9_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO9_MCU_WPU_M  (IO_MUX_GPIO9_MCU_WPU_V << IO_MUX_GPIO9_MCU_WPU_S)
#define IO_MUX_GPIO9_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO9_MCU_WPU_S  3
/** IO_MUX_GPIO9_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO9_MCU_IE    (BIT(4))
#define IO_MUX_GPIO9_MCU_IE_M  (IO_MUX_GPIO9_MCU_IE_V << IO_MUX_GPIO9_MCU_IE_S)
#define IO_MUX_GPIO9_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO9_MCU_IE_S  4
/** IO_MUX_GPIO9_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO9_MCU_DRV    0x00000003U
#define IO_MUX_GPIO9_MCU_DRV_M  (IO_MUX_GPIO9_MCU_DRV_V << IO_MUX_GPIO9_MCU_DRV_S)
#define IO_MUX_GPIO9_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO9_MCU_DRV_S  5
/** IO_MUX_GPIO9_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO9_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO9_FUN_WPD_M  (IO_MUX_GPIO9_FUN_WPD_V << IO_MUX_GPIO9_FUN_WPD_S)
#define IO_MUX_GPIO9_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO9_FUN_WPD_S  7
/** IO_MUX_GPIO9_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO9_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO9_FUN_WPU_M  (IO_MUX_GPIO9_FUN_WPU_V << IO_MUX_GPIO9_FUN_WPU_S)
#define IO_MUX_GPIO9_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO9_FUN_WPU_S  8
/** IO_MUX_GPIO9_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO9_FUN_IE    (BIT(9))
#define IO_MUX_GPIO9_FUN_IE_M  (IO_MUX_GPIO9_FUN_IE_V << IO_MUX_GPIO9_FUN_IE_S)
#define IO_MUX_GPIO9_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO9_FUN_IE_S  9
/** IO_MUX_GPIO9_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO9_FUN_DRV    0x00000003U
#define IO_MUX_GPIO9_FUN_DRV_M  (IO_MUX_GPIO9_FUN_DRV_V << IO_MUX_GPIO9_FUN_DRV_S)
#define IO_MUX_GPIO9_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO9_FUN_DRV_S  10
/** IO_MUX_GPIO9_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO9_MCU_SEL    0x00000007U
#define IO_MUX_GPIO9_MCU_SEL_M  (IO_MUX_GPIO9_MCU_SEL_V << IO_MUX_GPIO9_MCU_SEL_S)
#define IO_MUX_GPIO9_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO9_MCU_SEL_S  12
/** IO_MUX_GPIO9_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO9_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO9_FILTER_EN_M  (IO_MUX_GPIO9_FILTER_EN_V << IO_MUX_GPIO9_FILTER_EN_S)
#define IO_MUX_GPIO9_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO9_FILTER_EN_S  15
/** IO_MUX_GPIO9_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO9_HYS_EN    (BIT(16))
#define IO_MUX_GPIO9_HYS_EN_M  (IO_MUX_GPIO9_HYS_EN_V << IO_MUX_GPIO9_HYS_EN_S)
#define IO_MUX_GPIO9_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO9_HYS_EN_S  16
/** IO_MUX_GPIO9_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO9_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO9_HYS_SEL_M  (IO_MUX_GPIO9_HYS_SEL_V << IO_MUX_GPIO9_HYS_SEL_S)
#define IO_MUX_GPIO9_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO9_HYS_SEL_S  17

/** IO_MUX_GPIO10_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO10_REG (DR_REG_IO_MUX_BASE + 0x2c)
/** IO_MUX_GPIO10_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO10_MCU_OE    (BIT(0))
#define IO_MUX_GPIO10_MCU_OE_M  (IO_MUX_GPIO10_MCU_OE_V << IO_MUX_GPIO10_MCU_OE_S)
#define IO_MUX_GPIO10_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO10_MCU_OE_S  0
/** IO_MUX_GPIO10_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO10_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO10_SLP_SEL_M  (IO_MUX_GPIO10_SLP_SEL_V << IO_MUX_GPIO10_SLP_SEL_S)
#define IO_MUX_GPIO10_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO10_SLP_SEL_S  1
/** IO_MUX_GPIO10_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO10_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO10_MCU_WPD_M  (IO_MUX_GPIO10_MCU_WPD_V << IO_MUX_GPIO10_MCU_WPD_S)
#define IO_MUX_GPIO10_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO10_MCU_WPD_S  2
/** IO_MUX_GPIO10_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO10_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO10_MCU_WPU_M  (IO_MUX_GPIO10_MCU_WPU_V << IO_MUX_GPIO10_MCU_WPU_S)
#define IO_MUX_GPIO10_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO10_MCU_WPU_S  3
/** IO_MUX_GPIO10_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO10_MCU_IE    (BIT(4))
#define IO_MUX_GPIO10_MCU_IE_M  (IO_MUX_GPIO10_MCU_IE_V << IO_MUX_GPIO10_MCU_IE_S)
#define IO_MUX_GPIO10_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO10_MCU_IE_S  4
/** IO_MUX_GPIO10_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO10_MCU_DRV    0x00000003U
#define IO_MUX_GPIO10_MCU_DRV_M  (IO_MUX_GPIO10_MCU_DRV_V << IO_MUX_GPIO10_MCU_DRV_S)
#define IO_MUX_GPIO10_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO10_MCU_DRV_S  5
/** IO_MUX_GPIO10_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO10_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO10_FUN_WPD_M  (IO_MUX_GPIO10_FUN_WPD_V << IO_MUX_GPIO10_FUN_WPD_S)
#define IO_MUX_GPIO10_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO10_FUN_WPD_S  7
/** IO_MUX_GPIO10_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO10_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO10_FUN_WPU_M  (IO_MUX_GPIO10_FUN_WPU_V << IO_MUX_GPIO10_FUN_WPU_S)
#define IO_MUX_GPIO10_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO10_FUN_WPU_S  8
/** IO_MUX_GPIO10_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO10_FUN_IE    (BIT(9))
#define IO_MUX_GPIO10_FUN_IE_M  (IO_MUX_GPIO10_FUN_IE_V << IO_MUX_GPIO10_FUN_IE_S)
#define IO_MUX_GPIO10_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO10_FUN_IE_S  9
/** IO_MUX_GPIO10_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO10_FUN_DRV    0x00000003U
#define IO_MUX_GPIO10_FUN_DRV_M  (IO_MUX_GPIO10_FUN_DRV_V << IO_MUX_GPIO10_FUN_DRV_S)
#define IO_MUX_GPIO10_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO10_FUN_DRV_S  10
/** IO_MUX_GPIO10_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO10_MCU_SEL    0x00000007U
#define IO_MUX_GPIO10_MCU_SEL_M  (IO_MUX_GPIO10_MCU_SEL_V << IO_MUX_GPIO10_MCU_SEL_S)
#define IO_MUX_GPIO10_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO10_MCU_SEL_S  12
/** IO_MUX_GPIO10_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO10_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO10_FILTER_EN_M  (IO_MUX_GPIO10_FILTER_EN_V << IO_MUX_GPIO10_FILTER_EN_S)
#define IO_MUX_GPIO10_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO10_FILTER_EN_S  15
/** IO_MUX_GPIO10_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO10_HYS_EN    (BIT(16))
#define IO_MUX_GPIO10_HYS_EN_M  (IO_MUX_GPIO10_HYS_EN_V << IO_MUX_GPIO10_HYS_EN_S)
#define IO_MUX_GPIO10_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO10_HYS_EN_S  16
/** IO_MUX_GPIO10_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO10_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO10_HYS_SEL_M  (IO_MUX_GPIO10_HYS_SEL_V << IO_MUX_GPIO10_HYS_SEL_S)
#define IO_MUX_GPIO10_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO10_HYS_SEL_S  17

/** IO_MUX_GPIO11_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO11_REG (DR_REG_IO_MUX_BASE + 0x30)
/** IO_MUX_GPIO11_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO11_MCU_OE    (BIT(0))
#define IO_MUX_GPIO11_MCU_OE_M  (IO_MUX_GPIO11_MCU_OE_V << IO_MUX_GPIO11_MCU_OE_S)
#define IO_MUX_GPIO11_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO11_MCU_OE_S  0
/** IO_MUX_GPIO11_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO11_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO11_SLP_SEL_M  (IO_MUX_GPIO11_SLP_SEL_V << IO_MUX_GPIO11_SLP_SEL_S)
#define IO_MUX_GPIO11_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO11_SLP_SEL_S  1
/** IO_MUX_GPIO11_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO11_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO11_MCU_WPD_M  (IO_MUX_GPIO11_MCU_WPD_V << IO_MUX_GPIO11_MCU_WPD_S)
#define IO_MUX_GPIO11_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO11_MCU_WPD_S  2
/** IO_MUX_GPIO11_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO11_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO11_MCU_WPU_M  (IO_MUX_GPIO11_MCU_WPU_V << IO_MUX_GPIO11_MCU_WPU_S)
#define IO_MUX_GPIO11_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO11_MCU_WPU_S  3
/** IO_MUX_GPIO11_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO11_MCU_IE    (BIT(4))
#define IO_MUX_GPIO11_MCU_IE_M  (IO_MUX_GPIO11_MCU_IE_V << IO_MUX_GPIO11_MCU_IE_S)
#define IO_MUX_GPIO11_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO11_MCU_IE_S  4
/** IO_MUX_GPIO11_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO11_MCU_DRV    0x00000003U
#define IO_MUX_GPIO11_MCU_DRV_M  (IO_MUX_GPIO11_MCU_DRV_V << IO_MUX_GPIO11_MCU_DRV_S)
#define IO_MUX_GPIO11_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO11_MCU_DRV_S  5
/** IO_MUX_GPIO11_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO11_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO11_FUN_WPD_M  (IO_MUX_GPIO11_FUN_WPD_V << IO_MUX_GPIO11_FUN_WPD_S)
#define IO_MUX_GPIO11_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO11_FUN_WPD_S  7
/** IO_MUX_GPIO11_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO11_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO11_FUN_WPU_M  (IO_MUX_GPIO11_FUN_WPU_V << IO_MUX_GPIO11_FUN_WPU_S)
#define IO_MUX_GPIO11_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO11_FUN_WPU_S  8
/** IO_MUX_GPIO11_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO11_FUN_IE    (BIT(9))
#define IO_MUX_GPIO11_FUN_IE_M  (IO_MUX_GPIO11_FUN_IE_V << IO_MUX_GPIO11_FUN_IE_S)
#define IO_MUX_GPIO11_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO11_FUN_IE_S  9
/** IO_MUX_GPIO11_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO11_FUN_DRV    0x00000003U
#define IO_MUX_GPIO11_FUN_DRV_M  (IO_MUX_GPIO11_FUN_DRV_V << IO_MUX_GPIO11_FUN_DRV_S)
#define IO_MUX_GPIO11_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO11_FUN_DRV_S  10
/** IO_MUX_GPIO11_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO11_MCU_SEL    0x00000007U
#define IO_MUX_GPIO11_MCU_SEL_M  (IO_MUX_GPIO11_MCU_SEL_V << IO_MUX_GPIO11_MCU_SEL_S)
#define IO_MUX_GPIO11_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO11_MCU_SEL_S  12
/** IO_MUX_GPIO11_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO11_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO11_FILTER_EN_M  (IO_MUX_GPIO11_FILTER_EN_V << IO_MUX_GPIO11_FILTER_EN_S)
#define IO_MUX_GPIO11_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO11_FILTER_EN_S  15
/** IO_MUX_GPIO11_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO11_HYS_EN    (BIT(16))
#define IO_MUX_GPIO11_HYS_EN_M  (IO_MUX_GPIO11_HYS_EN_V << IO_MUX_GPIO11_HYS_EN_S)
#define IO_MUX_GPIO11_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO11_HYS_EN_S  16
/** IO_MUX_GPIO11_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO11_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO11_HYS_SEL_M  (IO_MUX_GPIO11_HYS_SEL_V << IO_MUX_GPIO11_HYS_SEL_S)
#define IO_MUX_GPIO11_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO11_HYS_SEL_S  17

/** IO_MUX_GPIO12_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO12_REG (DR_REG_IO_MUX_BASE + 0x34)
/** IO_MUX_GPIO12_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO12_MCU_OE    (BIT(0))
#define IO_MUX_GPIO12_MCU_OE_M  (IO_MUX_GPIO12_MCU_OE_V << IO_MUX_GPIO12_MCU_OE_S)
#define IO_MUX_GPIO12_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO12_MCU_OE_S  0
/** IO_MUX_GPIO12_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO12_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO12_SLP_SEL_M  (IO_MUX_GPIO12_SLP_SEL_V << IO_MUX_GPIO12_SLP_SEL_S)
#define IO_MUX_GPIO12_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO12_SLP_SEL_S  1
/** IO_MUX_GPIO12_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO12_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO12_MCU_WPD_M  (IO_MUX_GPIO12_MCU_WPD_V << IO_MUX_GPIO12_MCU_WPD_S)
#define IO_MUX_GPIO12_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO12_MCU_WPD_S  2
/** IO_MUX_GPIO12_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO12_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO12_MCU_WPU_M  (IO_MUX_GPIO12_MCU_WPU_V << IO_MUX_GPIO12_MCU_WPU_S)
#define IO_MUX_GPIO12_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO12_MCU_WPU_S  3
/** IO_MUX_GPIO12_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO12_MCU_IE    (BIT(4))
#define IO_MUX_GPIO12_MCU_IE_M  (IO_MUX_GPIO12_MCU_IE_V << IO_MUX_GPIO12_MCU_IE_S)
#define IO_MUX_GPIO12_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO12_MCU_IE_S  4
/** IO_MUX_GPIO12_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO12_MCU_DRV    0x00000003U
#define IO_MUX_GPIO12_MCU_DRV_M  (IO_MUX_GPIO12_MCU_DRV_V << IO_MUX_GPIO12_MCU_DRV_S)
#define IO_MUX_GPIO12_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO12_MCU_DRV_S  5
/** IO_MUX_GPIO12_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO12_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO12_FUN_WPD_M  (IO_MUX_GPIO12_FUN_WPD_V << IO_MUX_GPIO12_FUN_WPD_S)
#define IO_MUX_GPIO12_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO12_FUN_WPD_S  7
/** IO_MUX_GPIO12_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO12_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO12_FUN_WPU_M  (IO_MUX_GPIO12_FUN_WPU_V << IO_MUX_GPIO12_FUN_WPU_S)
#define IO_MUX_GPIO12_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO12_FUN_WPU_S  8
/** IO_MUX_GPIO12_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO12_FUN_IE    (BIT(9))
#define IO_MUX_GPIO12_FUN_IE_M  (IO_MUX_GPIO12_FUN_IE_V << IO_MUX_GPIO12_FUN_IE_S)
#define IO_MUX_GPIO12_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO12_FUN_IE_S  9
/** IO_MUX_GPIO12_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO12_FUN_DRV    0x00000003U
#define IO_MUX_GPIO12_FUN_DRV_M  (IO_MUX_GPIO12_FUN_DRV_V << IO_MUX_GPIO12_FUN_DRV_S)
#define IO_MUX_GPIO12_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO12_FUN_DRV_S  10
/** IO_MUX_GPIO12_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO12_MCU_SEL    0x00000007U
#define IO_MUX_GPIO12_MCU_SEL_M  (IO_MUX_GPIO12_MCU_SEL_V << IO_MUX_GPIO12_MCU_SEL_S)
#define IO_MUX_GPIO12_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO12_MCU_SEL_S  12
/** IO_MUX_GPIO12_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO12_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO12_FILTER_EN_M  (IO_MUX_GPIO12_FILTER_EN_V << IO_MUX_GPIO12_FILTER_EN_S)
#define IO_MUX_GPIO12_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO12_FILTER_EN_S  15
/** IO_MUX_GPIO12_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO12_HYS_EN    (BIT(16))
#define IO_MUX_GPIO12_HYS_EN_M  (IO_MUX_GPIO12_HYS_EN_V << IO_MUX_GPIO12_HYS_EN_S)
#define IO_MUX_GPIO12_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO12_HYS_EN_S  16
/** IO_MUX_GPIO12_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO12_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO12_HYS_SEL_M  (IO_MUX_GPIO12_HYS_SEL_V << IO_MUX_GPIO12_HYS_SEL_S)
#define IO_MUX_GPIO12_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO12_HYS_SEL_S  17

/** IO_MUX_GPIO13_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO13_REG (DR_REG_IO_MUX_BASE + 0x38)
/** IO_MUX_GPIO13_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO13_MCU_OE    (BIT(0))
#define IO_MUX_GPIO13_MCU_OE_M  (IO_MUX_GPIO13_MCU_OE_V << IO_MUX_GPIO13_MCU_OE_S)
#define IO_MUX_GPIO13_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO13_MCU_OE_S  0
/** IO_MUX_GPIO13_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO13_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO13_SLP_SEL_M  (IO_MUX_GPIO13_SLP_SEL_V << IO_MUX_GPIO13_SLP_SEL_S)
#define IO_MUX_GPIO13_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO13_SLP_SEL_S  1
/** IO_MUX_GPIO13_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO13_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO13_MCU_WPD_M  (IO_MUX_GPIO13_MCU_WPD_V << IO_MUX_GPIO13_MCU_WPD_S)
#define IO_MUX_GPIO13_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO13_MCU_WPD_S  2
/** IO_MUX_GPIO13_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO13_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO13_MCU_WPU_M  (IO_MUX_GPIO13_MCU_WPU_V << IO_MUX_GPIO13_MCU_WPU_S)
#define IO_MUX_GPIO13_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO13_MCU_WPU_S  3
/** IO_MUX_GPIO13_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO13_MCU_IE    (BIT(4))
#define IO_MUX_GPIO13_MCU_IE_M  (IO_MUX_GPIO13_MCU_IE_V << IO_MUX_GPIO13_MCU_IE_S)
#define IO_MUX_GPIO13_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO13_MCU_IE_S  4
/** IO_MUX_GPIO13_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO13_MCU_DRV    0x00000003U
#define IO_MUX_GPIO13_MCU_DRV_M  (IO_MUX_GPIO13_MCU_DRV_V << IO_MUX_GPIO13_MCU_DRV_S)
#define IO_MUX_GPIO13_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO13_MCU_DRV_S  5
/** IO_MUX_GPIO13_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO13_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO13_FUN_WPD_M  (IO_MUX_GPIO13_FUN_WPD_V << IO_MUX_GPIO13_FUN_WPD_S)
#define IO_MUX_GPIO13_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO13_FUN_WPD_S  7
/** IO_MUX_GPIO13_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO13_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO13_FUN_WPU_M  (IO_MUX_GPIO13_FUN_WPU_V << IO_MUX_GPIO13_FUN_WPU_S)
#define IO_MUX_GPIO13_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO13_FUN_WPU_S  8
/** IO_MUX_GPIO13_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO13_FUN_IE    (BIT(9))
#define IO_MUX_GPIO13_FUN_IE_M  (IO_MUX_GPIO13_FUN_IE_V << IO_MUX_GPIO13_FUN_IE_S)
#define IO_MUX_GPIO13_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO13_FUN_IE_S  9
/** IO_MUX_GPIO13_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO13_FUN_DRV    0x00000003U
#define IO_MUX_GPIO13_FUN_DRV_M  (IO_MUX_GPIO13_FUN_DRV_V << IO_MUX_GPIO13_FUN_DRV_S)
#define IO_MUX_GPIO13_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO13_FUN_DRV_S  10
/** IO_MUX_GPIO13_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO13_MCU_SEL    0x00000007U
#define IO_MUX_GPIO13_MCU_SEL_M  (IO_MUX_GPIO13_MCU_SEL_V << IO_MUX_GPIO13_MCU_SEL_S)
#define IO_MUX_GPIO13_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO13_MCU_SEL_S  12
/** IO_MUX_GPIO13_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO13_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO13_FILTER_EN_M  (IO_MUX_GPIO13_FILTER_EN_V << IO_MUX_GPIO13_FILTER_EN_S)
#define IO_MUX_GPIO13_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO13_FILTER_EN_S  15
/** IO_MUX_GPIO13_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO13_HYS_EN    (BIT(16))
#define IO_MUX_GPIO13_HYS_EN_M  (IO_MUX_GPIO13_HYS_EN_V << IO_MUX_GPIO13_HYS_EN_S)
#define IO_MUX_GPIO13_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO13_HYS_EN_S  16
/** IO_MUX_GPIO13_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO13_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO13_HYS_SEL_M  (IO_MUX_GPIO13_HYS_SEL_V << IO_MUX_GPIO13_HYS_SEL_S)
#define IO_MUX_GPIO13_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO13_HYS_SEL_S  17

/** IO_MUX_GPIO14_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO14_REG (DR_REG_IO_MUX_BASE + 0x3c)
/** IO_MUX_GPIO14_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO14_MCU_OE    (BIT(0))
#define IO_MUX_GPIO14_MCU_OE_M  (IO_MUX_GPIO14_MCU_OE_V << IO_MUX_GPIO14_MCU_OE_S)
#define IO_MUX_GPIO14_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO14_MCU_OE_S  0
/** IO_MUX_GPIO14_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO14_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO14_SLP_SEL_M  (IO_MUX_GPIO14_SLP_SEL_V << IO_MUX_GPIO14_SLP_SEL_S)
#define IO_MUX_GPIO14_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO14_SLP_SEL_S  1
/** IO_MUX_GPIO14_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO14_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO14_MCU_WPD_M  (IO_MUX_GPIO14_MCU_WPD_V << IO_MUX_GPIO14_MCU_WPD_S)
#define IO_MUX_GPIO14_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO14_MCU_WPD_S  2
/** IO_MUX_GPIO14_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO14_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO14_MCU_WPU_M  (IO_MUX_GPIO14_MCU_WPU_V << IO_MUX_GPIO14_MCU_WPU_S)
#define IO_MUX_GPIO14_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO14_MCU_WPU_S  3
/** IO_MUX_GPIO14_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO14_MCU_IE    (BIT(4))
#define IO_MUX_GPIO14_MCU_IE_M  (IO_MUX_GPIO14_MCU_IE_V << IO_MUX_GPIO14_MCU_IE_S)
#define IO_MUX_GPIO14_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO14_MCU_IE_S  4
/** IO_MUX_GPIO14_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO14_MCU_DRV    0x00000003U
#define IO_MUX_GPIO14_MCU_DRV_M  (IO_MUX_GPIO14_MCU_DRV_V << IO_MUX_GPIO14_MCU_DRV_S)
#define IO_MUX_GPIO14_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO14_MCU_DRV_S  5
/** IO_MUX_GPIO14_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO14_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO14_FUN_WPD_M  (IO_MUX_GPIO14_FUN_WPD_V << IO_MUX_GPIO14_FUN_WPD_S)
#define IO_MUX_GPIO14_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO14_FUN_WPD_S  7
/** IO_MUX_GPIO14_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO14_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO14_FUN_WPU_M  (IO_MUX_GPIO14_FUN_WPU_V << IO_MUX_GPIO14_FUN_WPU_S)
#define IO_MUX_GPIO14_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO14_FUN_WPU_S  8
/** IO_MUX_GPIO14_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO14_FUN_IE    (BIT(9))
#define IO_MUX_GPIO14_FUN_IE_M  (IO_MUX_GPIO14_FUN_IE_V << IO_MUX_GPIO14_FUN_IE_S)
#define IO_MUX_GPIO14_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO14_FUN_IE_S  9
/** IO_MUX_GPIO14_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO14_FUN_DRV    0x00000003U
#define IO_MUX_GPIO14_FUN_DRV_M  (IO_MUX_GPIO14_FUN_DRV_V << IO_MUX_GPIO14_FUN_DRV_S)
#define IO_MUX_GPIO14_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO14_FUN_DRV_S  10
/** IO_MUX_GPIO14_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO14_MCU_SEL    0x00000007U
#define IO_MUX_GPIO14_MCU_SEL_M  (IO_MUX_GPIO14_MCU_SEL_V << IO_MUX_GPIO14_MCU_SEL_S)
#define IO_MUX_GPIO14_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO14_MCU_SEL_S  12
/** IO_MUX_GPIO14_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO14_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO14_FILTER_EN_M  (IO_MUX_GPIO14_FILTER_EN_V << IO_MUX_GPIO14_FILTER_EN_S)
#define IO_MUX_GPIO14_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO14_FILTER_EN_S  15
/** IO_MUX_GPIO14_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO14_HYS_EN    (BIT(16))
#define IO_MUX_GPIO14_HYS_EN_M  (IO_MUX_GPIO14_HYS_EN_V << IO_MUX_GPIO14_HYS_EN_S)
#define IO_MUX_GPIO14_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO14_HYS_EN_S  16
/** IO_MUX_GPIO14_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO14_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO14_HYS_SEL_M  (IO_MUX_GPIO14_HYS_SEL_V << IO_MUX_GPIO14_HYS_SEL_S)
#define IO_MUX_GPIO14_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO14_HYS_SEL_S  17

/** IO_MUX_GPIO15_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO15_REG (DR_REG_IO_MUX_BASE + 0x40)
/** IO_MUX_GPIO15_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO15_MCU_OE    (BIT(0))
#define IO_MUX_GPIO15_MCU_OE_M  (IO_MUX_GPIO15_MCU_OE_V << IO_MUX_GPIO15_MCU_OE_S)
#define IO_MUX_GPIO15_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO15_MCU_OE_S  0
/** IO_MUX_GPIO15_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO15_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO15_SLP_SEL_M  (IO_MUX_GPIO15_SLP_SEL_V << IO_MUX_GPIO15_SLP_SEL_S)
#define IO_MUX_GPIO15_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO15_SLP_SEL_S  1
/** IO_MUX_GPIO15_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO15_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO15_MCU_WPD_M  (IO_MUX_GPIO15_MCU_WPD_V << IO_MUX_GPIO15_MCU_WPD_S)
#define IO_MUX_GPIO15_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO15_MCU_WPD_S  2
/** IO_MUX_GPIO15_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO15_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO15_MCU_WPU_M  (IO_MUX_GPIO15_MCU_WPU_V << IO_MUX_GPIO15_MCU_WPU_S)
#define IO_MUX_GPIO15_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO15_MCU_WPU_S  3
/** IO_MUX_GPIO15_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO15_MCU_IE    (BIT(4))
#define IO_MUX_GPIO15_MCU_IE_M  (IO_MUX_GPIO15_MCU_IE_V << IO_MUX_GPIO15_MCU_IE_S)
#define IO_MUX_GPIO15_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO15_MCU_IE_S  4
/** IO_MUX_GPIO15_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO15_MCU_DRV    0x00000003U
#define IO_MUX_GPIO15_MCU_DRV_M  (IO_MUX_GPIO15_MCU_DRV_V << IO_MUX_GPIO15_MCU_DRV_S)
#define IO_MUX_GPIO15_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO15_MCU_DRV_S  5
/** IO_MUX_GPIO15_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO15_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO15_FUN_WPD_M  (IO_MUX_GPIO15_FUN_WPD_V << IO_MUX_GPIO15_FUN_WPD_S)
#define IO_MUX_GPIO15_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO15_FUN_WPD_S  7
/** IO_MUX_GPIO15_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO15_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO15_FUN_WPU_M  (IO_MUX_GPIO15_FUN_WPU_V << IO_MUX_GPIO15_FUN_WPU_S)
#define IO_MUX_GPIO15_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO15_FUN_WPU_S  8
/** IO_MUX_GPIO15_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO15_FUN_IE    (BIT(9))
#define IO_MUX_GPIO15_FUN_IE_M  (IO_MUX_GPIO15_FUN_IE_V << IO_MUX_GPIO15_FUN_IE_S)
#define IO_MUX_GPIO15_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO15_FUN_IE_S  9
/** IO_MUX_GPIO15_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO15_FUN_DRV    0x00000003U
#define IO_MUX_GPIO15_FUN_DRV_M  (IO_MUX_GPIO15_FUN_DRV_V << IO_MUX_GPIO15_FUN_DRV_S)
#define IO_MUX_GPIO15_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO15_FUN_DRV_S  10
/** IO_MUX_GPIO15_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO15_MCU_SEL    0x00000007U
#define IO_MUX_GPIO15_MCU_SEL_M  (IO_MUX_GPIO15_MCU_SEL_V << IO_MUX_GPIO15_MCU_SEL_S)
#define IO_MUX_GPIO15_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO15_MCU_SEL_S  12
/** IO_MUX_GPIO15_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO15_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO15_FILTER_EN_M  (IO_MUX_GPIO15_FILTER_EN_V << IO_MUX_GPIO15_FILTER_EN_S)
#define IO_MUX_GPIO15_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO15_FILTER_EN_S  15
/** IO_MUX_GPIO15_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO15_HYS_EN    (BIT(16))
#define IO_MUX_GPIO15_HYS_EN_M  (IO_MUX_GPIO15_HYS_EN_V << IO_MUX_GPIO15_HYS_EN_S)
#define IO_MUX_GPIO15_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO15_HYS_EN_S  16
/** IO_MUX_GPIO15_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO15_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO15_HYS_SEL_M  (IO_MUX_GPIO15_HYS_SEL_V << IO_MUX_GPIO15_HYS_SEL_S)
#define IO_MUX_GPIO15_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO15_HYS_SEL_S  17

/** IO_MUX_GPIO16_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO16_REG (DR_REG_IO_MUX_BASE + 0x44)
/** IO_MUX_GPIO16_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO16_MCU_OE    (BIT(0))
#define IO_MUX_GPIO16_MCU_OE_M  (IO_MUX_GPIO16_MCU_OE_V << IO_MUX_GPIO16_MCU_OE_S)
#define IO_MUX_GPIO16_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO16_MCU_OE_S  0
/** IO_MUX_GPIO16_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO16_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO16_SLP_SEL_M  (IO_MUX_GPIO16_SLP_SEL_V << IO_MUX_GPIO16_SLP_SEL_S)
#define IO_MUX_GPIO16_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO16_SLP_SEL_S  1
/** IO_MUX_GPIO16_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO16_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO16_MCU_WPD_M  (IO_MUX_GPIO16_MCU_WPD_V << IO_MUX_GPIO16_MCU_WPD_S)
#define IO_MUX_GPIO16_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO16_MCU_WPD_S  2
/** IO_MUX_GPIO16_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO16_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO16_MCU_WPU_M  (IO_MUX_GPIO16_MCU_WPU_V << IO_MUX_GPIO16_MCU_WPU_S)
#define IO_MUX_GPIO16_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO16_MCU_WPU_S  3
/** IO_MUX_GPIO16_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO16_MCU_IE    (BIT(4))
#define IO_MUX_GPIO16_MCU_IE_M  (IO_MUX_GPIO16_MCU_IE_V << IO_MUX_GPIO16_MCU_IE_S)
#define IO_MUX_GPIO16_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO16_MCU_IE_S  4
/** IO_MUX_GPIO16_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO16_MCU_DRV    0x00000003U
#define IO_MUX_GPIO16_MCU_DRV_M  (IO_MUX_GPIO16_MCU_DRV_V << IO_MUX_GPIO16_MCU_DRV_S)
#define IO_MUX_GPIO16_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO16_MCU_DRV_S  5
/** IO_MUX_GPIO16_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO16_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO16_FUN_WPD_M  (IO_MUX_GPIO16_FUN_WPD_V << IO_MUX_GPIO16_FUN_WPD_S)
#define IO_MUX_GPIO16_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO16_FUN_WPD_S  7
/** IO_MUX_GPIO16_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO16_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO16_FUN_WPU_M  (IO_MUX_GPIO16_FUN_WPU_V << IO_MUX_GPIO16_FUN_WPU_S)
#define IO_MUX_GPIO16_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO16_FUN_WPU_S  8
/** IO_MUX_GPIO16_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO16_FUN_IE    (BIT(9))
#define IO_MUX_GPIO16_FUN_IE_M  (IO_MUX_GPIO16_FUN_IE_V << IO_MUX_GPIO16_FUN_IE_S)
#define IO_MUX_GPIO16_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO16_FUN_IE_S  9
/** IO_MUX_GPIO16_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO16_FUN_DRV    0x00000003U
#define IO_MUX_GPIO16_FUN_DRV_M  (IO_MUX_GPIO16_FUN_DRV_V << IO_MUX_GPIO16_FUN_DRV_S)
#define IO_MUX_GPIO16_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO16_FUN_DRV_S  10
/** IO_MUX_GPIO16_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO16_MCU_SEL    0x00000007U
#define IO_MUX_GPIO16_MCU_SEL_M  (IO_MUX_GPIO16_MCU_SEL_V << IO_MUX_GPIO16_MCU_SEL_S)
#define IO_MUX_GPIO16_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO16_MCU_SEL_S  12
/** IO_MUX_GPIO16_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO16_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO16_FILTER_EN_M  (IO_MUX_GPIO16_FILTER_EN_V << IO_MUX_GPIO16_FILTER_EN_S)
#define IO_MUX_GPIO16_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO16_FILTER_EN_S  15
/** IO_MUX_GPIO16_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO16_HYS_EN    (BIT(16))
#define IO_MUX_GPIO16_HYS_EN_M  (IO_MUX_GPIO16_HYS_EN_V << IO_MUX_GPIO16_HYS_EN_S)
#define IO_MUX_GPIO16_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO16_HYS_EN_S  16
/** IO_MUX_GPIO16_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO16_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO16_HYS_SEL_M  (IO_MUX_GPIO16_HYS_SEL_V << IO_MUX_GPIO16_HYS_SEL_S)
#define IO_MUX_GPIO16_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO16_HYS_SEL_S  17

/** IO_MUX_GPIO17_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO17_REG (DR_REG_IO_MUX_BASE + 0x48)
/** IO_MUX_GPIO17_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO17_MCU_OE    (BIT(0))
#define IO_MUX_GPIO17_MCU_OE_M  (IO_MUX_GPIO17_MCU_OE_V << IO_MUX_GPIO17_MCU_OE_S)
#define IO_MUX_GPIO17_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO17_MCU_OE_S  0
/** IO_MUX_GPIO17_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO17_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO17_SLP_SEL_M  (IO_MUX_GPIO17_SLP_SEL_V << IO_MUX_GPIO17_SLP_SEL_S)
#define IO_MUX_GPIO17_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO17_SLP_SEL_S  1
/** IO_MUX_GPIO17_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO17_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO17_MCU_WPD_M  (IO_MUX_GPIO17_MCU_WPD_V << IO_MUX_GPIO17_MCU_WPD_S)
#define IO_MUX_GPIO17_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO17_MCU_WPD_S  2
/** IO_MUX_GPIO17_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO17_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO17_MCU_WPU_M  (IO_MUX_GPIO17_MCU_WPU_V << IO_MUX_GPIO17_MCU_WPU_S)
#define IO_MUX_GPIO17_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO17_MCU_WPU_S  3
/** IO_MUX_GPIO17_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO17_MCU_IE    (BIT(4))
#define IO_MUX_GPIO17_MCU_IE_M  (IO_MUX_GPIO17_MCU_IE_V << IO_MUX_GPIO17_MCU_IE_S)
#define IO_MUX_GPIO17_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO17_MCU_IE_S  4
/** IO_MUX_GPIO17_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO17_MCU_DRV    0x00000003U
#define IO_MUX_GPIO17_MCU_DRV_M  (IO_MUX_GPIO17_MCU_DRV_V << IO_MUX_GPIO17_MCU_DRV_S)
#define IO_MUX_GPIO17_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO17_MCU_DRV_S  5
/** IO_MUX_GPIO17_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO17_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO17_FUN_WPD_M  (IO_MUX_GPIO17_FUN_WPD_V << IO_MUX_GPIO17_FUN_WPD_S)
#define IO_MUX_GPIO17_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO17_FUN_WPD_S  7
/** IO_MUX_GPIO17_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO17_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO17_FUN_WPU_M  (IO_MUX_GPIO17_FUN_WPU_V << IO_MUX_GPIO17_FUN_WPU_S)
#define IO_MUX_GPIO17_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO17_FUN_WPU_S  8
/** IO_MUX_GPIO17_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO17_FUN_IE    (BIT(9))
#define IO_MUX_GPIO17_FUN_IE_M  (IO_MUX_GPIO17_FUN_IE_V << IO_MUX_GPIO17_FUN_IE_S)
#define IO_MUX_GPIO17_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO17_FUN_IE_S  9
/** IO_MUX_GPIO17_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO17_FUN_DRV    0x00000003U
#define IO_MUX_GPIO17_FUN_DRV_M  (IO_MUX_GPIO17_FUN_DRV_V << IO_MUX_GPIO17_FUN_DRV_S)
#define IO_MUX_GPIO17_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO17_FUN_DRV_S  10
/** IO_MUX_GPIO17_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO17_MCU_SEL    0x00000007U
#define IO_MUX_GPIO17_MCU_SEL_M  (IO_MUX_GPIO17_MCU_SEL_V << IO_MUX_GPIO17_MCU_SEL_S)
#define IO_MUX_GPIO17_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO17_MCU_SEL_S  12
/** IO_MUX_GPIO17_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO17_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO17_FILTER_EN_M  (IO_MUX_GPIO17_FILTER_EN_V << IO_MUX_GPIO17_FILTER_EN_S)
#define IO_MUX_GPIO17_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO17_FILTER_EN_S  15
/** IO_MUX_GPIO17_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO17_HYS_EN    (BIT(16))
#define IO_MUX_GPIO17_HYS_EN_M  (IO_MUX_GPIO17_HYS_EN_V << IO_MUX_GPIO17_HYS_EN_S)
#define IO_MUX_GPIO17_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO17_HYS_EN_S  16
/** IO_MUX_GPIO17_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO17_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO17_HYS_SEL_M  (IO_MUX_GPIO17_HYS_SEL_V << IO_MUX_GPIO17_HYS_SEL_S)
#define IO_MUX_GPIO17_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO17_HYS_SEL_S  17

/** IO_MUX_GPIO18_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO18_REG (DR_REG_IO_MUX_BASE + 0x4c)
/** IO_MUX_GPIO18_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO18_MCU_OE    (BIT(0))
#define IO_MUX_GPIO18_MCU_OE_M  (IO_MUX_GPIO18_MCU_OE_V << IO_MUX_GPIO18_MCU_OE_S)
#define IO_MUX_GPIO18_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO18_MCU_OE_S  0
/** IO_MUX_GPIO18_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO18_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO18_SLP_SEL_M  (IO_MUX_GPIO18_SLP_SEL_V << IO_MUX_GPIO18_SLP_SEL_S)
#define IO_MUX_GPIO18_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO18_SLP_SEL_S  1
/** IO_MUX_GPIO18_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO18_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO18_MCU_WPD_M  (IO_MUX_GPIO18_MCU_WPD_V << IO_MUX_GPIO18_MCU_WPD_S)
#define IO_MUX_GPIO18_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO18_MCU_WPD_S  2
/** IO_MUX_GPIO18_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO18_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO18_MCU_WPU_M  (IO_MUX_GPIO18_MCU_WPU_V << IO_MUX_GPIO18_MCU_WPU_S)
#define IO_MUX_GPIO18_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO18_MCU_WPU_S  3
/** IO_MUX_GPIO18_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO18_MCU_IE    (BIT(4))
#define IO_MUX_GPIO18_MCU_IE_M  (IO_MUX_GPIO18_MCU_IE_V << IO_MUX_GPIO18_MCU_IE_S)
#define IO_MUX_GPIO18_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO18_MCU_IE_S  4
/** IO_MUX_GPIO18_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO18_MCU_DRV    0x00000003U
#define IO_MUX_GPIO18_MCU_DRV_M  (IO_MUX_GPIO18_MCU_DRV_V << IO_MUX_GPIO18_MCU_DRV_S)
#define IO_MUX_GPIO18_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO18_MCU_DRV_S  5
/** IO_MUX_GPIO18_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO18_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO18_FUN_WPD_M  (IO_MUX_GPIO18_FUN_WPD_V << IO_MUX_GPIO18_FUN_WPD_S)
#define IO_MUX_GPIO18_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO18_FUN_WPD_S  7
/** IO_MUX_GPIO18_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO18_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO18_FUN_WPU_M  (IO_MUX_GPIO18_FUN_WPU_V << IO_MUX_GPIO18_FUN_WPU_S)
#define IO_MUX_GPIO18_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO18_FUN_WPU_S  8
/** IO_MUX_GPIO18_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO18_FUN_IE    (BIT(9))
#define IO_MUX_GPIO18_FUN_IE_M  (IO_MUX_GPIO18_FUN_IE_V << IO_MUX_GPIO18_FUN_IE_S)
#define IO_MUX_GPIO18_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO18_FUN_IE_S  9
/** IO_MUX_GPIO18_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO18_FUN_DRV    0x00000003U
#define IO_MUX_GPIO18_FUN_DRV_M  (IO_MUX_GPIO18_FUN_DRV_V << IO_MUX_GPIO18_FUN_DRV_S)
#define IO_MUX_GPIO18_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO18_FUN_DRV_S  10
/** IO_MUX_GPIO18_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO18_MCU_SEL    0x00000007U
#define IO_MUX_GPIO18_MCU_SEL_M  (IO_MUX_GPIO18_MCU_SEL_V << IO_MUX_GPIO18_MCU_SEL_S)
#define IO_MUX_GPIO18_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO18_MCU_SEL_S  12
/** IO_MUX_GPIO18_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO18_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO18_FILTER_EN_M  (IO_MUX_GPIO18_FILTER_EN_V << IO_MUX_GPIO18_FILTER_EN_S)
#define IO_MUX_GPIO18_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO18_FILTER_EN_S  15
/** IO_MUX_GPIO18_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO18_HYS_EN    (BIT(16))
#define IO_MUX_GPIO18_HYS_EN_M  (IO_MUX_GPIO18_HYS_EN_V << IO_MUX_GPIO18_HYS_EN_S)
#define IO_MUX_GPIO18_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO18_HYS_EN_S  16
/** IO_MUX_GPIO18_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO18_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO18_HYS_SEL_M  (IO_MUX_GPIO18_HYS_SEL_V << IO_MUX_GPIO18_HYS_SEL_S)
#define IO_MUX_GPIO18_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO18_HYS_SEL_S  17

/** IO_MUX_GPIO19_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO19_REG (DR_REG_IO_MUX_BASE + 0x50)
/** IO_MUX_GPIO19_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO19_MCU_OE    (BIT(0))
#define IO_MUX_GPIO19_MCU_OE_M  (IO_MUX_GPIO19_MCU_OE_V << IO_MUX_GPIO19_MCU_OE_S)
#define IO_MUX_GPIO19_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO19_MCU_OE_S  0
/** IO_MUX_GPIO19_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO19_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO19_SLP_SEL_M  (IO_MUX_GPIO19_SLP_SEL_V << IO_MUX_GPIO19_SLP_SEL_S)
#define IO_MUX_GPIO19_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO19_SLP_SEL_S  1
/** IO_MUX_GPIO19_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO19_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO19_MCU_WPD_M  (IO_MUX_GPIO19_MCU_WPD_V << IO_MUX_GPIO19_MCU_WPD_S)
#define IO_MUX_GPIO19_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO19_MCU_WPD_S  2
/** IO_MUX_GPIO19_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO19_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO19_MCU_WPU_M  (IO_MUX_GPIO19_MCU_WPU_V << IO_MUX_GPIO19_MCU_WPU_S)
#define IO_MUX_GPIO19_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO19_MCU_WPU_S  3
/** IO_MUX_GPIO19_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO19_MCU_IE    (BIT(4))
#define IO_MUX_GPIO19_MCU_IE_M  (IO_MUX_GPIO19_MCU_IE_V << IO_MUX_GPIO19_MCU_IE_S)
#define IO_MUX_GPIO19_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO19_MCU_IE_S  4
/** IO_MUX_GPIO19_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO19_MCU_DRV    0x00000003U
#define IO_MUX_GPIO19_MCU_DRV_M  (IO_MUX_GPIO19_MCU_DRV_V << IO_MUX_GPIO19_MCU_DRV_S)
#define IO_MUX_GPIO19_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO19_MCU_DRV_S  5
/** IO_MUX_GPIO19_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO19_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO19_FUN_WPD_M  (IO_MUX_GPIO19_FUN_WPD_V << IO_MUX_GPIO19_FUN_WPD_S)
#define IO_MUX_GPIO19_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO19_FUN_WPD_S  7
/** IO_MUX_GPIO19_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO19_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO19_FUN_WPU_M  (IO_MUX_GPIO19_FUN_WPU_V << IO_MUX_GPIO19_FUN_WPU_S)
#define IO_MUX_GPIO19_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO19_FUN_WPU_S  8
/** IO_MUX_GPIO19_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO19_FUN_IE    (BIT(9))
#define IO_MUX_GPIO19_FUN_IE_M  (IO_MUX_GPIO19_FUN_IE_V << IO_MUX_GPIO19_FUN_IE_S)
#define IO_MUX_GPIO19_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO19_FUN_IE_S  9
/** IO_MUX_GPIO19_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO19_FUN_DRV    0x00000003U
#define IO_MUX_GPIO19_FUN_DRV_M  (IO_MUX_GPIO19_FUN_DRV_V << IO_MUX_GPIO19_FUN_DRV_S)
#define IO_MUX_GPIO19_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO19_FUN_DRV_S  10
/** IO_MUX_GPIO19_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO19_MCU_SEL    0x00000007U
#define IO_MUX_GPIO19_MCU_SEL_M  (IO_MUX_GPIO19_MCU_SEL_V << IO_MUX_GPIO19_MCU_SEL_S)
#define IO_MUX_GPIO19_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO19_MCU_SEL_S  12
/** IO_MUX_GPIO19_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO19_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO19_FILTER_EN_M  (IO_MUX_GPIO19_FILTER_EN_V << IO_MUX_GPIO19_FILTER_EN_S)
#define IO_MUX_GPIO19_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO19_FILTER_EN_S  15
/** IO_MUX_GPIO19_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO19_HYS_EN    (BIT(16))
#define IO_MUX_GPIO19_HYS_EN_M  (IO_MUX_GPIO19_HYS_EN_V << IO_MUX_GPIO19_HYS_EN_S)
#define IO_MUX_GPIO19_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO19_HYS_EN_S  16
/** IO_MUX_GPIO19_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO19_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO19_HYS_SEL_M  (IO_MUX_GPIO19_HYS_SEL_V << IO_MUX_GPIO19_HYS_SEL_S)
#define IO_MUX_GPIO19_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO19_HYS_SEL_S  17

/** IO_MUX_GPIO20_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO20_REG (DR_REG_IO_MUX_BASE + 0x54)
/** IO_MUX_GPIO20_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO20_MCU_OE    (BIT(0))
#define IO_MUX_GPIO20_MCU_OE_M  (IO_MUX_GPIO20_MCU_OE_V << IO_MUX_GPIO20_MCU_OE_S)
#define IO_MUX_GPIO20_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO20_MCU_OE_S  0
/** IO_MUX_GPIO20_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO20_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO20_SLP_SEL_M  (IO_MUX_GPIO20_SLP_SEL_V << IO_MUX_GPIO20_SLP_SEL_S)
#define IO_MUX_GPIO20_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO20_SLP_SEL_S  1
/** IO_MUX_GPIO20_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO20_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO20_MCU_WPD_M  (IO_MUX_GPIO20_MCU_WPD_V << IO_MUX_GPIO20_MCU_WPD_S)
#define IO_MUX_GPIO20_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO20_MCU_WPD_S  2
/** IO_MUX_GPIO20_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO20_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO20_MCU_WPU_M  (IO_MUX_GPIO20_MCU_WPU_V << IO_MUX_GPIO20_MCU_WPU_S)
#define IO_MUX_GPIO20_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO20_MCU_WPU_S  3
/** IO_MUX_GPIO20_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO20_MCU_IE    (BIT(4))
#define IO_MUX_GPIO20_MCU_IE_M  (IO_MUX_GPIO20_MCU_IE_V << IO_MUX_GPIO20_MCU_IE_S)
#define IO_MUX_GPIO20_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO20_MCU_IE_S  4
/** IO_MUX_GPIO20_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO20_MCU_DRV    0x00000003U
#define IO_MUX_GPIO20_MCU_DRV_M  (IO_MUX_GPIO20_MCU_DRV_V << IO_MUX_GPIO20_MCU_DRV_S)
#define IO_MUX_GPIO20_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO20_MCU_DRV_S  5
/** IO_MUX_GPIO20_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO20_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO20_FUN_WPD_M  (IO_MUX_GPIO20_FUN_WPD_V << IO_MUX_GPIO20_FUN_WPD_S)
#define IO_MUX_GPIO20_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO20_FUN_WPD_S  7
/** IO_MUX_GPIO20_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO20_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO20_FUN_WPU_M  (IO_MUX_GPIO20_FUN_WPU_V << IO_MUX_GPIO20_FUN_WPU_S)
#define IO_MUX_GPIO20_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO20_FUN_WPU_S  8
/** IO_MUX_GPIO20_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO20_FUN_IE    (BIT(9))
#define IO_MUX_GPIO20_FUN_IE_M  (IO_MUX_GPIO20_FUN_IE_V << IO_MUX_GPIO20_FUN_IE_S)
#define IO_MUX_GPIO20_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO20_FUN_IE_S  9
/** IO_MUX_GPIO20_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO20_FUN_DRV    0x00000003U
#define IO_MUX_GPIO20_FUN_DRV_M  (IO_MUX_GPIO20_FUN_DRV_V << IO_MUX_GPIO20_FUN_DRV_S)
#define IO_MUX_GPIO20_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO20_FUN_DRV_S  10
/** IO_MUX_GPIO20_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO20_MCU_SEL    0x00000007U
#define IO_MUX_GPIO20_MCU_SEL_M  (IO_MUX_GPIO20_MCU_SEL_V << IO_MUX_GPIO20_MCU_SEL_S)
#define IO_MUX_GPIO20_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO20_MCU_SEL_S  12
/** IO_MUX_GPIO20_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO20_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO20_FILTER_EN_M  (IO_MUX_GPIO20_FILTER_EN_V << IO_MUX_GPIO20_FILTER_EN_S)
#define IO_MUX_GPIO20_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO20_FILTER_EN_S  15
/** IO_MUX_GPIO20_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO20_HYS_EN    (BIT(16))
#define IO_MUX_GPIO20_HYS_EN_M  (IO_MUX_GPIO20_HYS_EN_V << IO_MUX_GPIO20_HYS_EN_S)
#define IO_MUX_GPIO20_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO20_HYS_EN_S  16
/** IO_MUX_GPIO20_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO20_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO20_HYS_SEL_M  (IO_MUX_GPIO20_HYS_SEL_V << IO_MUX_GPIO20_HYS_SEL_S)
#define IO_MUX_GPIO20_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO20_HYS_SEL_S  17

/** IO_MUX_GPIO21_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO21_REG (DR_REG_IO_MUX_BASE + 0x58)
/** IO_MUX_GPIO21_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO21_MCU_OE    (BIT(0))
#define IO_MUX_GPIO21_MCU_OE_M  (IO_MUX_GPIO21_MCU_OE_V << IO_MUX_GPIO21_MCU_OE_S)
#define IO_MUX_GPIO21_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO21_MCU_OE_S  0
/** IO_MUX_GPIO21_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO21_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO21_SLP_SEL_M  (IO_MUX_GPIO21_SLP_SEL_V << IO_MUX_GPIO21_SLP_SEL_S)
#define IO_MUX_GPIO21_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO21_SLP_SEL_S  1
/** IO_MUX_GPIO21_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO21_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO21_MCU_WPD_M  (IO_MUX_GPIO21_MCU_WPD_V << IO_MUX_GPIO21_MCU_WPD_S)
#define IO_MUX_GPIO21_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO21_MCU_WPD_S  2
/** IO_MUX_GPIO21_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO21_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO21_MCU_WPU_M  (IO_MUX_GPIO21_MCU_WPU_V << IO_MUX_GPIO21_MCU_WPU_S)
#define IO_MUX_GPIO21_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO21_MCU_WPU_S  3
/** IO_MUX_GPIO21_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO21_MCU_IE    (BIT(4))
#define IO_MUX_GPIO21_MCU_IE_M  (IO_MUX_GPIO21_MCU_IE_V << IO_MUX_GPIO21_MCU_IE_S)
#define IO_MUX_GPIO21_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO21_MCU_IE_S  4
/** IO_MUX_GPIO21_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO21_MCU_DRV    0x00000003U
#define IO_MUX_GPIO21_MCU_DRV_M  (IO_MUX_GPIO21_MCU_DRV_V << IO_MUX_GPIO21_MCU_DRV_S)
#define IO_MUX_GPIO21_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO21_MCU_DRV_S  5
/** IO_MUX_GPIO21_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO21_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO21_FUN_WPD_M  (IO_MUX_GPIO21_FUN_WPD_V << IO_MUX_GPIO21_FUN_WPD_S)
#define IO_MUX_GPIO21_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO21_FUN_WPD_S  7
/** IO_MUX_GPIO21_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO21_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO21_FUN_WPU_M  (IO_MUX_GPIO21_FUN_WPU_V << IO_MUX_GPIO21_FUN_WPU_S)
#define IO_MUX_GPIO21_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO21_FUN_WPU_S  8
/** IO_MUX_GPIO21_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO21_FUN_IE    (BIT(9))
#define IO_MUX_GPIO21_FUN_IE_M  (IO_MUX_GPIO21_FUN_IE_V << IO_MUX_GPIO21_FUN_IE_S)
#define IO_MUX_GPIO21_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO21_FUN_IE_S  9
/** IO_MUX_GPIO21_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO21_FUN_DRV    0x00000003U
#define IO_MUX_GPIO21_FUN_DRV_M  (IO_MUX_GPIO21_FUN_DRV_V << IO_MUX_GPIO21_FUN_DRV_S)
#define IO_MUX_GPIO21_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO21_FUN_DRV_S  10
/** IO_MUX_GPIO21_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO21_MCU_SEL    0x00000007U
#define IO_MUX_GPIO21_MCU_SEL_M  (IO_MUX_GPIO21_MCU_SEL_V << IO_MUX_GPIO21_MCU_SEL_S)
#define IO_MUX_GPIO21_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO21_MCU_SEL_S  12
/** IO_MUX_GPIO21_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO21_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO21_FILTER_EN_M  (IO_MUX_GPIO21_FILTER_EN_V << IO_MUX_GPIO21_FILTER_EN_S)
#define IO_MUX_GPIO21_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO21_FILTER_EN_S  15
/** IO_MUX_GPIO21_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO21_HYS_EN    (BIT(16))
#define IO_MUX_GPIO21_HYS_EN_M  (IO_MUX_GPIO21_HYS_EN_V << IO_MUX_GPIO21_HYS_EN_S)
#define IO_MUX_GPIO21_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO21_HYS_EN_S  16
/** IO_MUX_GPIO21_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO21_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO21_HYS_SEL_M  (IO_MUX_GPIO21_HYS_SEL_V << IO_MUX_GPIO21_HYS_SEL_S)
#define IO_MUX_GPIO21_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO21_HYS_SEL_S  17

/** IO_MUX_GPIO22_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO22_REG (DR_REG_IO_MUX_BASE + 0x5c)
/** IO_MUX_GPIO22_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO22_MCU_OE    (BIT(0))
#define IO_MUX_GPIO22_MCU_OE_M  (IO_MUX_GPIO22_MCU_OE_V << IO_MUX_GPIO22_MCU_OE_S)
#define IO_MUX_GPIO22_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO22_MCU_OE_S  0
/** IO_MUX_GPIO22_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO22_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO22_SLP_SEL_M  (IO_MUX_GPIO22_SLP_SEL_V << IO_MUX_GPIO22_SLP_SEL_S)
#define IO_MUX_GPIO22_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO22_SLP_SEL_S  1
/** IO_MUX_GPIO22_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO22_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO22_MCU_WPD_M  (IO_MUX_GPIO22_MCU_WPD_V << IO_MUX_GPIO22_MCU_WPD_S)
#define IO_MUX_GPIO22_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO22_MCU_WPD_S  2
/** IO_MUX_GPIO22_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO22_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO22_MCU_WPU_M  (IO_MUX_GPIO22_MCU_WPU_V << IO_MUX_GPIO22_MCU_WPU_S)
#define IO_MUX_GPIO22_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO22_MCU_WPU_S  3
/** IO_MUX_GPIO22_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO22_MCU_IE    (BIT(4))
#define IO_MUX_GPIO22_MCU_IE_M  (IO_MUX_GPIO22_MCU_IE_V << IO_MUX_GPIO22_MCU_IE_S)
#define IO_MUX_GPIO22_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO22_MCU_IE_S  4
/** IO_MUX_GPIO22_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO22_MCU_DRV    0x00000003U
#define IO_MUX_GPIO22_MCU_DRV_M  (IO_MUX_GPIO22_MCU_DRV_V << IO_MUX_GPIO22_MCU_DRV_S)
#define IO_MUX_GPIO22_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO22_MCU_DRV_S  5
/** IO_MUX_GPIO22_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO22_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO22_FUN_WPD_M  (IO_MUX_GPIO22_FUN_WPD_V << IO_MUX_GPIO22_FUN_WPD_S)
#define IO_MUX_GPIO22_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO22_FUN_WPD_S  7
/** IO_MUX_GPIO22_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO22_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO22_FUN_WPU_M  (IO_MUX_GPIO22_FUN_WPU_V << IO_MUX_GPIO22_FUN_WPU_S)
#define IO_MUX_GPIO22_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO22_FUN_WPU_S  8
/** IO_MUX_GPIO22_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO22_FUN_IE    (BIT(9))
#define IO_MUX_GPIO22_FUN_IE_M  (IO_MUX_GPIO22_FUN_IE_V << IO_MUX_GPIO22_FUN_IE_S)
#define IO_MUX_GPIO22_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO22_FUN_IE_S  9
/** IO_MUX_GPIO22_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO22_FUN_DRV    0x00000003U
#define IO_MUX_GPIO22_FUN_DRV_M  (IO_MUX_GPIO22_FUN_DRV_V << IO_MUX_GPIO22_FUN_DRV_S)
#define IO_MUX_GPIO22_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO22_FUN_DRV_S  10
/** IO_MUX_GPIO22_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO22_MCU_SEL    0x00000007U
#define IO_MUX_GPIO22_MCU_SEL_M  (IO_MUX_GPIO22_MCU_SEL_V << IO_MUX_GPIO22_MCU_SEL_S)
#define IO_MUX_GPIO22_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO22_MCU_SEL_S  12
/** IO_MUX_GPIO22_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO22_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO22_FILTER_EN_M  (IO_MUX_GPIO22_FILTER_EN_V << IO_MUX_GPIO22_FILTER_EN_S)
#define IO_MUX_GPIO22_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO22_FILTER_EN_S  15
/** IO_MUX_GPIO22_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO22_HYS_EN    (BIT(16))
#define IO_MUX_GPIO22_HYS_EN_M  (IO_MUX_GPIO22_HYS_EN_V << IO_MUX_GPIO22_HYS_EN_S)
#define IO_MUX_GPIO22_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO22_HYS_EN_S  16
/** IO_MUX_GPIO22_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO22_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO22_HYS_SEL_M  (IO_MUX_GPIO22_HYS_SEL_V << IO_MUX_GPIO22_HYS_SEL_S)
#define IO_MUX_GPIO22_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO22_HYS_SEL_S  17

/** IO_MUX_GPIO23_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO23_REG (DR_REG_IO_MUX_BASE + 0x60)
/** IO_MUX_GPIO23_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO23_MCU_OE    (BIT(0))
#define IO_MUX_GPIO23_MCU_OE_M  (IO_MUX_GPIO23_MCU_OE_V << IO_MUX_GPIO23_MCU_OE_S)
#define IO_MUX_GPIO23_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO23_MCU_OE_S  0
/** IO_MUX_GPIO23_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO23_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO23_SLP_SEL_M  (IO_MUX_GPIO23_SLP_SEL_V << IO_MUX_GPIO23_SLP_SEL_S)
#define IO_MUX_GPIO23_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO23_SLP_SEL_S  1
/** IO_MUX_GPIO23_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO23_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO23_MCU_WPD_M  (IO_MUX_GPIO23_MCU_WPD_V << IO_MUX_GPIO23_MCU_WPD_S)
#define IO_MUX_GPIO23_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO23_MCU_WPD_S  2
/** IO_MUX_GPIO23_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO23_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO23_MCU_WPU_M  (IO_MUX_GPIO23_MCU_WPU_V << IO_MUX_GPIO23_MCU_WPU_S)
#define IO_MUX_GPIO23_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO23_MCU_WPU_S  3
/** IO_MUX_GPIO23_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO23_MCU_IE    (BIT(4))
#define IO_MUX_GPIO23_MCU_IE_M  (IO_MUX_GPIO23_MCU_IE_V << IO_MUX_GPIO23_MCU_IE_S)
#define IO_MUX_GPIO23_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO23_MCU_IE_S  4
/** IO_MUX_GPIO23_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO23_MCU_DRV    0x00000003U
#define IO_MUX_GPIO23_MCU_DRV_M  (IO_MUX_GPIO23_MCU_DRV_V << IO_MUX_GPIO23_MCU_DRV_S)
#define IO_MUX_GPIO23_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO23_MCU_DRV_S  5
/** IO_MUX_GPIO23_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO23_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO23_FUN_WPD_M  (IO_MUX_GPIO23_FUN_WPD_V << IO_MUX_GPIO23_FUN_WPD_S)
#define IO_MUX_GPIO23_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO23_FUN_WPD_S  7
/** IO_MUX_GPIO23_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO23_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO23_FUN_WPU_M  (IO_MUX_GPIO23_FUN_WPU_V << IO_MUX_GPIO23_FUN_WPU_S)
#define IO_MUX_GPIO23_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO23_FUN_WPU_S  8
/** IO_MUX_GPIO23_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO23_FUN_IE    (BIT(9))
#define IO_MUX_GPIO23_FUN_IE_M  (IO_MUX_GPIO23_FUN_IE_V << IO_MUX_GPIO23_FUN_IE_S)
#define IO_MUX_GPIO23_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO23_FUN_IE_S  9
/** IO_MUX_GPIO23_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO23_FUN_DRV    0x00000003U
#define IO_MUX_GPIO23_FUN_DRV_M  (IO_MUX_GPIO23_FUN_DRV_V << IO_MUX_GPIO23_FUN_DRV_S)
#define IO_MUX_GPIO23_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO23_FUN_DRV_S  10
/** IO_MUX_GPIO23_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO23_MCU_SEL    0x00000007U
#define IO_MUX_GPIO23_MCU_SEL_M  (IO_MUX_GPIO23_MCU_SEL_V << IO_MUX_GPIO23_MCU_SEL_S)
#define IO_MUX_GPIO23_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO23_MCU_SEL_S  12
/** IO_MUX_GPIO23_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO23_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO23_FILTER_EN_M  (IO_MUX_GPIO23_FILTER_EN_V << IO_MUX_GPIO23_FILTER_EN_S)
#define IO_MUX_GPIO23_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO23_FILTER_EN_S  15
/** IO_MUX_GPIO23_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO23_HYS_EN    (BIT(16))
#define IO_MUX_GPIO23_HYS_EN_M  (IO_MUX_GPIO23_HYS_EN_V << IO_MUX_GPIO23_HYS_EN_S)
#define IO_MUX_GPIO23_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO23_HYS_EN_S  16
/** IO_MUX_GPIO23_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO23_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO23_HYS_SEL_M  (IO_MUX_GPIO23_HYS_SEL_V << IO_MUX_GPIO23_HYS_SEL_S)
#define IO_MUX_GPIO23_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO23_HYS_SEL_S  17

/** IO_MUX_GPIO24_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO24_REG (DR_REG_IO_MUX_BASE + 0x64)
/** IO_MUX_GPIO24_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO24_MCU_OE    (BIT(0))
#define IO_MUX_GPIO24_MCU_OE_M  (IO_MUX_GPIO24_MCU_OE_V << IO_MUX_GPIO24_MCU_OE_S)
#define IO_MUX_GPIO24_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO24_MCU_OE_S  0
/** IO_MUX_GPIO24_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO24_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO24_SLP_SEL_M  (IO_MUX_GPIO24_SLP_SEL_V << IO_MUX_GPIO24_SLP_SEL_S)
#define IO_MUX_GPIO24_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO24_SLP_SEL_S  1
/** IO_MUX_GPIO24_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO24_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO24_MCU_WPD_M  (IO_MUX_GPIO24_MCU_WPD_V << IO_MUX_GPIO24_MCU_WPD_S)
#define IO_MUX_GPIO24_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO24_MCU_WPD_S  2
/** IO_MUX_GPIO24_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO24_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO24_MCU_WPU_M  (IO_MUX_GPIO24_MCU_WPU_V << IO_MUX_GPIO24_MCU_WPU_S)
#define IO_MUX_GPIO24_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO24_MCU_WPU_S  3
/** IO_MUX_GPIO24_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO24_MCU_IE    (BIT(4))
#define IO_MUX_GPIO24_MCU_IE_M  (IO_MUX_GPIO24_MCU_IE_V << IO_MUX_GPIO24_MCU_IE_S)
#define IO_MUX_GPIO24_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO24_MCU_IE_S  4
/** IO_MUX_GPIO24_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO24_MCU_DRV    0x00000003U
#define IO_MUX_GPIO24_MCU_DRV_M  (IO_MUX_GPIO24_MCU_DRV_V << IO_MUX_GPIO24_MCU_DRV_S)
#define IO_MUX_GPIO24_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO24_MCU_DRV_S  5
/** IO_MUX_GPIO24_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO24_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO24_FUN_WPD_M  (IO_MUX_GPIO24_FUN_WPD_V << IO_MUX_GPIO24_FUN_WPD_S)
#define IO_MUX_GPIO24_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO24_FUN_WPD_S  7
/** IO_MUX_GPIO24_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO24_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO24_FUN_WPU_M  (IO_MUX_GPIO24_FUN_WPU_V << IO_MUX_GPIO24_FUN_WPU_S)
#define IO_MUX_GPIO24_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO24_FUN_WPU_S  8
/** IO_MUX_GPIO24_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO24_FUN_IE    (BIT(9))
#define IO_MUX_GPIO24_FUN_IE_M  (IO_MUX_GPIO24_FUN_IE_V << IO_MUX_GPIO24_FUN_IE_S)
#define IO_MUX_GPIO24_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO24_FUN_IE_S  9
/** IO_MUX_GPIO24_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO24_FUN_DRV    0x00000003U
#define IO_MUX_GPIO24_FUN_DRV_M  (IO_MUX_GPIO24_FUN_DRV_V << IO_MUX_GPIO24_FUN_DRV_S)
#define IO_MUX_GPIO24_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO24_FUN_DRV_S  10
/** IO_MUX_GPIO24_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO24_MCU_SEL    0x00000007U
#define IO_MUX_GPIO24_MCU_SEL_M  (IO_MUX_GPIO24_MCU_SEL_V << IO_MUX_GPIO24_MCU_SEL_S)
#define IO_MUX_GPIO24_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO24_MCU_SEL_S  12
/** IO_MUX_GPIO24_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO24_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO24_FILTER_EN_M  (IO_MUX_GPIO24_FILTER_EN_V << IO_MUX_GPIO24_FILTER_EN_S)
#define IO_MUX_GPIO24_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO24_FILTER_EN_S  15
/** IO_MUX_GPIO24_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO24_HYS_EN    (BIT(16))
#define IO_MUX_GPIO24_HYS_EN_M  (IO_MUX_GPIO24_HYS_EN_V << IO_MUX_GPIO24_HYS_EN_S)
#define IO_MUX_GPIO24_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO24_HYS_EN_S  16
/** IO_MUX_GPIO24_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO24_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO24_HYS_SEL_M  (IO_MUX_GPIO24_HYS_SEL_V << IO_MUX_GPIO24_HYS_SEL_S)
#define IO_MUX_GPIO24_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO24_HYS_SEL_S  17

/** IO_MUX_GPIO25_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO25_REG (DR_REG_IO_MUX_BASE + 0x68)
/** IO_MUX_GPIO25_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO25_MCU_OE    (BIT(0))
#define IO_MUX_GPIO25_MCU_OE_M  (IO_MUX_GPIO25_MCU_OE_V << IO_MUX_GPIO25_MCU_OE_S)
#define IO_MUX_GPIO25_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO25_MCU_OE_S  0
/** IO_MUX_GPIO25_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO25_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO25_SLP_SEL_M  (IO_MUX_GPIO25_SLP_SEL_V << IO_MUX_GPIO25_SLP_SEL_S)
#define IO_MUX_GPIO25_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO25_SLP_SEL_S  1
/** IO_MUX_GPIO25_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO25_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO25_MCU_WPD_M  (IO_MUX_GPIO25_MCU_WPD_V << IO_MUX_GPIO25_MCU_WPD_S)
#define IO_MUX_GPIO25_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO25_MCU_WPD_S  2
/** IO_MUX_GPIO25_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO25_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO25_MCU_WPU_M  (IO_MUX_GPIO25_MCU_WPU_V << IO_MUX_GPIO25_MCU_WPU_S)
#define IO_MUX_GPIO25_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO25_MCU_WPU_S  3
/** IO_MUX_GPIO25_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO25_MCU_IE    (BIT(4))
#define IO_MUX_GPIO25_MCU_IE_M  (IO_MUX_GPIO25_MCU_IE_V << IO_MUX_GPIO25_MCU_IE_S)
#define IO_MUX_GPIO25_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO25_MCU_IE_S  4
/** IO_MUX_GPIO25_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO25_MCU_DRV    0x00000003U
#define IO_MUX_GPIO25_MCU_DRV_M  (IO_MUX_GPIO25_MCU_DRV_V << IO_MUX_GPIO25_MCU_DRV_S)
#define IO_MUX_GPIO25_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO25_MCU_DRV_S  5
/** IO_MUX_GPIO25_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO25_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO25_FUN_WPD_M  (IO_MUX_GPIO25_FUN_WPD_V << IO_MUX_GPIO25_FUN_WPD_S)
#define IO_MUX_GPIO25_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO25_FUN_WPD_S  7
/** IO_MUX_GPIO25_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO25_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO25_FUN_WPU_M  (IO_MUX_GPIO25_FUN_WPU_V << IO_MUX_GPIO25_FUN_WPU_S)
#define IO_MUX_GPIO25_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO25_FUN_WPU_S  8
/** IO_MUX_GPIO25_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO25_FUN_IE    (BIT(9))
#define IO_MUX_GPIO25_FUN_IE_M  (IO_MUX_GPIO25_FUN_IE_V << IO_MUX_GPIO25_FUN_IE_S)
#define IO_MUX_GPIO25_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO25_FUN_IE_S  9
/** IO_MUX_GPIO25_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO25_FUN_DRV    0x00000003U
#define IO_MUX_GPIO25_FUN_DRV_M  (IO_MUX_GPIO25_FUN_DRV_V << IO_MUX_GPIO25_FUN_DRV_S)
#define IO_MUX_GPIO25_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO25_FUN_DRV_S  10
/** IO_MUX_GPIO25_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO25_MCU_SEL    0x00000007U
#define IO_MUX_GPIO25_MCU_SEL_M  (IO_MUX_GPIO25_MCU_SEL_V << IO_MUX_GPIO25_MCU_SEL_S)
#define IO_MUX_GPIO25_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO25_MCU_SEL_S  12
/** IO_MUX_GPIO25_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO25_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO25_FILTER_EN_M  (IO_MUX_GPIO25_FILTER_EN_V << IO_MUX_GPIO25_FILTER_EN_S)
#define IO_MUX_GPIO25_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO25_FILTER_EN_S  15
/** IO_MUX_GPIO25_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO25_HYS_EN    (BIT(16))
#define IO_MUX_GPIO25_HYS_EN_M  (IO_MUX_GPIO25_HYS_EN_V << IO_MUX_GPIO25_HYS_EN_S)
#define IO_MUX_GPIO25_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO25_HYS_EN_S  16
/** IO_MUX_GPIO25_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO25_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO25_HYS_SEL_M  (IO_MUX_GPIO25_HYS_SEL_V << IO_MUX_GPIO25_HYS_SEL_S)
#define IO_MUX_GPIO25_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO25_HYS_SEL_S  17

/** IO_MUX_GPIO26_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO26_REG (DR_REG_IO_MUX_BASE + 0x6c)
/** IO_MUX_GPIO26_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO26_MCU_OE    (BIT(0))
#define IO_MUX_GPIO26_MCU_OE_M  (IO_MUX_GPIO26_MCU_OE_V << IO_MUX_GPIO26_MCU_OE_S)
#define IO_MUX_GPIO26_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO26_MCU_OE_S  0
/** IO_MUX_GPIO26_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO26_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO26_SLP_SEL_M  (IO_MUX_GPIO26_SLP_SEL_V << IO_MUX_GPIO26_SLP_SEL_S)
#define IO_MUX_GPIO26_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO26_SLP_SEL_S  1
/** IO_MUX_GPIO26_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO26_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO26_MCU_WPD_M  (IO_MUX_GPIO26_MCU_WPD_V << IO_MUX_GPIO26_MCU_WPD_S)
#define IO_MUX_GPIO26_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO26_MCU_WPD_S  2
/** IO_MUX_GPIO26_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO26_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO26_MCU_WPU_M  (IO_MUX_GPIO26_MCU_WPU_V << IO_MUX_GPIO26_MCU_WPU_S)
#define IO_MUX_GPIO26_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO26_MCU_WPU_S  3
/** IO_MUX_GPIO26_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO26_MCU_IE    (BIT(4))
#define IO_MUX_GPIO26_MCU_IE_M  (IO_MUX_GPIO26_MCU_IE_V << IO_MUX_GPIO26_MCU_IE_S)
#define IO_MUX_GPIO26_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO26_MCU_IE_S  4
/** IO_MUX_GPIO26_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO26_MCU_DRV    0x00000003U
#define IO_MUX_GPIO26_MCU_DRV_M  (IO_MUX_GPIO26_MCU_DRV_V << IO_MUX_GPIO26_MCU_DRV_S)
#define IO_MUX_GPIO26_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO26_MCU_DRV_S  5
/** IO_MUX_GPIO26_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO26_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO26_FUN_WPD_M  (IO_MUX_GPIO26_FUN_WPD_V << IO_MUX_GPIO26_FUN_WPD_S)
#define IO_MUX_GPIO26_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO26_FUN_WPD_S  7
/** IO_MUX_GPIO26_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO26_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO26_FUN_WPU_M  (IO_MUX_GPIO26_FUN_WPU_V << IO_MUX_GPIO26_FUN_WPU_S)
#define IO_MUX_GPIO26_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO26_FUN_WPU_S  8
/** IO_MUX_GPIO26_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO26_FUN_IE    (BIT(9))
#define IO_MUX_GPIO26_FUN_IE_M  (IO_MUX_GPIO26_FUN_IE_V << IO_MUX_GPIO26_FUN_IE_S)
#define IO_MUX_GPIO26_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO26_FUN_IE_S  9
/** IO_MUX_GPIO26_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO26_FUN_DRV    0x00000003U
#define IO_MUX_GPIO26_FUN_DRV_M  (IO_MUX_GPIO26_FUN_DRV_V << IO_MUX_GPIO26_FUN_DRV_S)
#define IO_MUX_GPIO26_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO26_FUN_DRV_S  10
/** IO_MUX_GPIO26_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO26_MCU_SEL    0x00000007U
#define IO_MUX_GPIO26_MCU_SEL_M  (IO_MUX_GPIO26_MCU_SEL_V << IO_MUX_GPIO26_MCU_SEL_S)
#define IO_MUX_GPIO26_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO26_MCU_SEL_S  12
/** IO_MUX_GPIO26_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO26_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO26_FILTER_EN_M  (IO_MUX_GPIO26_FILTER_EN_V << IO_MUX_GPIO26_FILTER_EN_S)
#define IO_MUX_GPIO26_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO26_FILTER_EN_S  15
/** IO_MUX_GPIO26_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO26_HYS_EN    (BIT(16))
#define IO_MUX_GPIO26_HYS_EN_M  (IO_MUX_GPIO26_HYS_EN_V << IO_MUX_GPIO26_HYS_EN_S)
#define IO_MUX_GPIO26_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO26_HYS_EN_S  16
/** IO_MUX_GPIO26_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO26_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO26_HYS_SEL_M  (IO_MUX_GPIO26_HYS_SEL_V << IO_MUX_GPIO26_HYS_SEL_S)
#define IO_MUX_GPIO26_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO26_HYS_SEL_S  17

/** IO_MUX_DATE_REG register
 *  IO MUX Version Control Register
 */
#define IO_MUX_DATE_REG (DR_REG_IO_MUX_BASE + 0xfc)
/** IO_MUX_REG_DATE : R/W; bitpos: [27:0]; default: 36708704;
 *  Version control register
 */
#define IO_MUX_REG_DATE    0x0FFFFFFFU
#define IO_MUX_REG_DATE_M  (IO_MUX_REG_DATE_V << IO_MUX_REG_DATE_S)
#define IO_MUX_REG_DATE_V  0x0FFFFFFFU
#define IO_MUX_REG_DATE_S  0

#ifdef __cplusplus
}
#endif
