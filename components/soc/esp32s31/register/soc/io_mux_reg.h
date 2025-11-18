/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "soc/soc.h"

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
#define PIN_PULLUP_DIS(PIN_NAME)                 REG_CLR_BIT(PIN_NAME, FUN_PU)
#define PIN_PULLUP_EN(PIN_NAME)                  REG_SET_BIT(PIN_NAME, FUN_PU)
#define PIN_PULLDWN_DIS(PIN_NAME)             REG_CLR_BIT(PIN_NAME, FUN_PD)
#define PIN_PULLDWN_EN(PIN_NAME)              REG_SET_BIT(PIN_NAME, FUN_PD)
#define PIN_FUNC_SELECT(PIN_NAME, FUNC)      REG_SET_FIELD(PIN_NAME, MCU_SEL, FUNC)

#define IO_MUX_GPIO0_REG                    PERIPHS_IO_MUX_U_XTAL32K_HYS_P
#define IO_MUX_GPIO1_REG                    PERIPHS_IO_MUX_U_XTAL32K_HYS_N
#define IO_MUX_GPIO2_REG                    PERIPHS_IO_MUX_U_GPIO2
#define IO_MUX_GPIO3_REG                    PERIPHS_IO_MUX_U_GPIO3
#define IO_MUX_GPIO4_REG                    PERIPHS_IO_MUX_U_GPIO4
#define IO_MUX_GPIO5_REG                    PERIPHS_IO_MUX_U_GPIO5
#define IO_MUX_GPIO6_REG                    PERIPHS_IO_MUX_U_TOUCH_GPIO6
#define IO_MUX_GPIO7_REG                    PERIPHS_IO_MUX_U_TOUCH_GPIO7
#define IO_MUX_GPIO8_REG                    PERIPHS_IO_MUX_U_TOUCH_GPIO8
#define IO_MUX_GPIO9_REG                    PERIPHS_IO_MUX_U_TOUCH_GPIO9
#define IO_MUX_GPIO10_REG                   PERIPHS_IO_MUX_U_TOUCH_GPIO10
#define IO_MUX_GPIO11_REG                   PERIPHS_IO_MUX_U_TOUCH_GPIO11
#define IO_MUX_GPIO12_REG                   PERIPHS_IO_MUX_U_TOUCH_GPIO12
#define IO_MUX_GPIO13_REG                   PERIPHS_IO_MUX_U_TOUCH_GPIO13
#define IO_MUX_GPIO14_REG                   PERIPHS_IO_MUX_U_TOUCH_GPIO14
#define IO_MUX_GPIO15_REG                   PERIPHS_IO_MUX_U_TOUCH_GPIO15
#define IO_MUX_GPIO16_REG                   PERIPHS_IO_MUX_U_TOUCH_GPIO16
#define IO_MUX_GPIO17_REG                   PERIPHS_IO_MUX_U_TOUCH_GPIO17
#define IO_MUX_GPIO18_REG                   PERIPHS_IO_MUX_U_TOUCH_GPIO18
#define IO_MUX_GPIO19_REG                   PERIPHS_IO_MUX_U_TOUCH_GPIO19
#define IO_MUX_GPIO20_REG                   PERIPHS_IO_MUX_U_GPIO20
#define IO_MUX_GPIO21_REG                   PERIPHS_IO_MUX_U_GPIO21
#define IO_MUX_GPIO22_REG                   PERIPHS_IO_MUX_U_GPIO22
#define IO_MUX_GPIO23_REG                   PERIPHS_IO_MUX_U_GPIO23
#define IO_MUX_GPIO24_REG                   PERIPHS_IO_MUX_U_GPIO24
#define IO_MUX_GPIO25_REG                   PERIPHS_IO_MUX_U_GPIO25
#define IO_MUX_GPIO26_REG                   PERIPHS_IO_MUX_U_GPIO26
#define IO_MUX_GPIO27_REG                   PERIPHS_IO_MUX_U_GPIO27
#define IO_MUX_GPIO28_REG                   PERIPHS_IO_MUX_U_GPIO28
#define IO_MUX_GPIO29_REG                   PERIPHS_IO_MUX_U_GPIO29
#define IO_MUX_GPIO30_REG                   PERIPHS_IO_MUX_U_GPIO30
#define IO_MUX_GPIO31_REG                   PERIPHS_IO_MUX_U_GPIO31
#define IO_MUX_GPIO32_REG                   PERIPHS_IO_MUX_U_GPIO32
#define IO_MUX_GPIO33_REG                   PERIPHS_IO_MUX_U_USB_GPIO33
#define IO_MUX_GPIO34_REG                   PERIPHS_IO_MUX_U_USB_GPIO34
#define IO_MUX_GPIO35_REG                   PERIPHS_IO_MUX_U_GPIO35
#define IO_MUX_GPIO36_REG                   PERIPHS_IO_MUX_U_GPIO36
#define IO_MUX_GPIO37_REG                   PERIPHS_IO_MUX_U_GPIO37
#define IO_MUX_GPIO38_REG                   PERIPHS_IO_MUX_U_GPIO38
#define IO_MUX_GPIO39_REG                   PERIPHS_IO_MUX_U_GPIO39
#define IO_MUX_GPIO40_REG                   PERIPHS_IO_MUX_U_GPIO40
#define IO_MUX_GPIO41_REG                   PERIPHS_IO_MUX_U_GPIO41
#define IO_MUX_GPIO42_REG                   PERIPHS_IO_MUX_U_ADC_GPIO42
#define IO_MUX_GPIO43_REG                   PERIPHS_IO_MUX_U_ADC_GPIO43
#define IO_MUX_GPIO44_REG                   PERIPHS_IO_MUX_U_ADC_GPIO44
#define IO_MUX_GPIO45_REG                   PERIPHS_IO_MUX_U_ADC_GPIO45
#define IO_MUX_GPIO46_REG                   PERIPHS_IO_MUX_U_ADC_GPIO46
#define IO_MUX_GPIO47_REG                   PERIPHS_IO_MUX_U_ADC_GPIO47
#define IO_MUX_GPIO48_REG                   PERIPHS_IO_MUX_U_ADC_GPIO48
#define IO_MUX_GPIO49_REG                   PERIPHS_IO_MUX_U_ADC_GPIO49
#define IO_MUX_GPIO50_REG                   PERIPHS_IO_MUX_U_ADC_GPIO50
#define IO_MUX_GPIO51_REG                   PERIPHS_IO_MUX_U_ADC_GPIO51
#define IO_MUX_GPIO52_REG                   PERIPHS_IO_MUX_U_ADC_GPIO52
#define IO_MUX_GPIO53_REG                   PERIPHS_IO_MUX_U_ADC_GPIO53
#define IO_MUX_GPIO54_REG                   PERIPHS_IO_MUX_U_ADC_GPIO54
#define IO_MUX_GPIO55_REG                   PERIPHS_IO_MUX_U_ADC_GPIO55
#define IO_MUX_GPIO56_REG                   PERIPHS_IO_MUX_U_ADC_GPIO56
#define IO_MUX_GPIO57_REG                   PERIPHS_IO_MUX_U_ADC_GPIO57
#define IO_MUX_GPIO58_REG                   PERIPHS_IO_MUX_U_GPIO58
#define IO_MUX_GPIO59_REG                   PERIPHS_IO_MUX_U_GPIO59
#define IO_MUX_GPIO60_REG                   PERIPHS_IO_MUX_U_GPIO60
#define IO_MUX_GPIO61_REG                   PERIPHS_IO_MUX_U_GPIO61
#define IO_MUX_GPIO62_REG                   PERIPHS_IO_MUX_U_GPIO62

#define FUNC_GPIO_GPIO                              1
#define PIN_FUNC_GPIO								1

#define MAX_RTC_GPIO_NUM             7
#define MAX_PAD_GPIO_NUM             62


#define REG_IO_MUX_BASE                              DR_REG_IO_MUX_BASE

// definitions below are generated from pin_txt.csv
#define PERIPHS_IO_MUX_U_XTAL32K_HYS_P               (REG_IO_MUX_BASE + 0x0)
#define FUNC_XTAL_32K_N_GPIO0_0                                            0
#define FUNC_XTAL_32K_N_GPIO0                                              1

#define PERIPHS_IO_MUX_U_XTAL32K_HYS_N               (REG_IO_MUX_BASE + 0x4)
#define FUNC_XTAL_32K_P_GPIO1_0                                            0
#define FUNC_XTAL_32K_P_GPIO1                                              1

#define PERIPHS_IO_MUX_U_GPIO2                       (REG_IO_MUX_BASE + 0x8)
#define FUNC_GPIO2_GPIO2_0                                                 0
#define FUNC_GPIO2_GPIO2                                                   1
#define FUNC_GPIO2_LCD_DATA19_OUT_PAD                                      3

#define PERIPHS_IO_MUX_U_GPIO3                       (REG_IO_MUX_BASE + 0xc)
#define FUNC_GPIO3_GPIO3_0                                                 0
#define FUNC_GPIO3_GPIO3                                                   1
#define FUNC_GPIO3_LCD_DATA20_OUT_PAD                                      3

#define PERIPHS_IO_MUX_U_GPIO4                      (REG_IO_MUX_BASE + 0x10)
#define FUNC_GPIO4_GPIO4_0                                                 0
#define FUNC_GPIO4_GPIO4                                                   1
#define FUNC_GPIO4_LCD_DATA21_OUT_PAD                                      3

#define PERIPHS_IO_MUX_U_GPIO5                      (REG_IO_MUX_BASE + 0x14)
#define FUNC_GPIO5_GPIO5_0                                                 0
#define FUNC_GPIO5_GPIO5                                                   1
#define FUNC_GPIO5_LCD_DATA22_OUT_PAD                                      3

#define PERIPHS_IO_MUX_U_TOUCH_GPIO6                (REG_IO_MUX_BASE + 0x18)
#define FUNC_GPIO6_GPIO6_0                                                 0
#define FUNC_GPIO6_GPIO6                                                   1

#define PERIPHS_IO_MUX_U_TOUCH_GPIO7                (REG_IO_MUX_BASE + 0x1c)
#define FUNC_GPIO7_GPIO7_0                                                 0
#define FUNC_GPIO7_GPIO7                                                   1
#define FUNC_GPIO7_LCD_DATA23_OUT_PAD                                      3

#define PERIPHS_IO_MUX_U_TOUCH_GPIO8                (REG_IO_MUX_BASE + 0x20)
#define FUNC_GPIO8_GPIO8_0                                                 0
#define FUNC_GPIO8_GPIO8                                                   1
#define FUNC_GPIO8_GMAC_PHY_TXD0_PAD                                       2
#define FUNC_GPIO8_LCD_DATA0_OUT_PAD                                       3

#define PERIPHS_IO_MUX_U_TOUCH_GPIO9                (REG_IO_MUX_BASE + 0x24)
#define FUNC_GPIO9_SPI2_HOLD_PAD                                           0
#define FUNC_GPIO9_GPIO9                                                   1
#define FUNC_GPIO9_GMAC_PHY_TXD1_PAD                                       2
#define FUNC_GPIO9_LCD_DATA1_OUT_PAD                                       3
#define FUNC_GPIO9_DBG_PSRAM_CK_PAD                                        4

#define PERIPHS_IO_MUX_U_TOUCH_GPIO10               (REG_IO_MUX_BASE + 0x28)
#define FUNC_GPIO10_SPI2_CS_PAD                                            0
#define FUNC_GPIO10_GPIO10                                                 1
#define FUNC_GPIO10_GMAC_PHY_TXD2_PAD                                      2
#define FUNC_GPIO10_LCD_DATA2_OUT_PAD                                      3
#define FUNC_GPIO10_DBG_PSRAM_CS_PAD                                       4

#define PERIPHS_IO_MUX_U_TOUCH_GPIO11               (REG_IO_MUX_BASE + 0x2c)
#define FUNC_GPIO11_SPI2_D_PAD                                             0
#define FUNC_GPIO11_GPIO11                                                 1
#define FUNC_GPIO11_GMAC_PHY_TXD3_PAD                                      2
#define FUNC_GPIO11_LCD_DATA3_OUT_PAD                                      3
#define FUNC_GPIO11_DBG_PSRAM_D_PAD                                        4

#define PERIPHS_IO_MUX_U_TOUCH_GPIO12               (REG_IO_MUX_BASE + 0x30)
#define FUNC_GPIO12_SPI2_CK_PAD                                            0
#define FUNC_GPIO12_GPIO12                                                 1
#define FUNC_GPIO12_GMAC_PHY_TXEN_PAD                                      2
#define FUNC_GPIO12_LCD_DATA4_OUT_PAD                                      3
#define FUNC_GPIO12_DBG_PSRAM_Q_PAD                                        4

#define PERIPHS_IO_MUX_U_TOUCH_GPIO13               (REG_IO_MUX_BASE + 0x34)
#define FUNC_GPIO13_SPI2_Q_PAD                                             0
#define FUNC_GPIO13_GPIO13                                                 1
#define FUNC_GPIO13_GMAC_RMII_CLK_PAD                                      2
#define FUNC_GPIO13_LCD_DATA5_OUT_PAD                                      3
#define FUNC_GPIO13_DBG_PSRAM_WP_PAD                                       4

#define PERIPHS_IO_MUX_U_TOUCH_GPIO14               (REG_IO_MUX_BASE + 0x38)
#define FUNC_GPIO14_SPI2_WP_PAD                                            0
#define FUNC_GPIO14_GPIO14                                                 1
#define FUNC_GPIO14_GMAC_RX_CLK_PAD                                        2
#define FUNC_GPIO14_LCD_DATA6_OUT_PAD                                      3
#define FUNC_GPIO14_DBG_PSRAM_HOLD_PAD                                     4

#define PERIPHS_IO_MUX_U_TOUCH_GPIO15               (REG_IO_MUX_BASE + 0x3c)
#define FUNC_GPIO15_SPI2_IO4_PAD                                           0
#define FUNC_GPIO15_GPIO15                                                 1
#define FUNC_GPIO15_GMAC_PHY_RXDV_PAD                                      2
#define FUNC_GPIO15_LCD_DATA7_OUT_PAD                                      3
#define FUNC_GPIO15_DBG_PSRAM_DQ4_PAD                                      4

#define PERIPHS_IO_MUX_U_TOUCH_GPIO16               (REG_IO_MUX_BASE + 0x40)
#define FUNC_GPIO16_SPI2_IO5_PAD                                           0
#define FUNC_GPIO16_GPIO16                                                 1
#define FUNC_GPIO16_GMAC_PHY_RXD3_PAD                                      2
#define FUNC_GPIO16_LCD_DATA8_OUT_PAD                                      3
#define FUNC_GPIO16_DBG_PSRAM_DQ5_PAD                                      4

#define PERIPHS_IO_MUX_U_TOUCH_GPIO17               (REG_IO_MUX_BASE + 0x44)
#define FUNC_GPIO17_SPI2_IO6_PAD                                           0
#define FUNC_GPIO17_GPIO17                                                 1
#define FUNC_GPIO17_GMAC_PHY_RXD2_PAD                                      2
#define FUNC_GPIO17_LCD_DATA9_OUT_PAD                                      3
#define FUNC_GPIO17_DBG_PSRAM_DQ6_PAD                                      4

#define PERIPHS_IO_MUX_U_TOUCH_GPIO18               (REG_IO_MUX_BASE + 0x48)
#define FUNC_GPIO18_SPI2_IO7_PAD                                           0
#define FUNC_GPIO18_GPIO18                                                 1
#define FUNC_GPIO18_GMAC_PHY_RXD1_PAD                                      2
#define FUNC_GPIO18_LCD_DATA10_OUT_PAD                                     3
#define FUNC_GPIO18_DBG_PSRAM_DQ7_PAD                                      4

#define PERIPHS_IO_MUX_U_TOUCH_GPIO19               (REG_IO_MUX_BASE + 0x4c)
#define FUNC_GPIO19_SPI2_DQS_PAD                                           0
#define FUNC_GPIO19_GPIO19                                                 1
#define FUNC_GPIO19_GMAC_PHY_RXD0_PAD                                      2
#define FUNC_GPIO19_LCD_DATA11_OUT_PAD                                     3
#define FUNC_GPIO19_DBG_PSRAM_DQS_0_PAD                                    4

#define PERIPHS_IO_MUX_U_GPIO20                     (REG_IO_MUX_BASE + 0x50)
#define FUNC_SDIO_DATA0_GPIO20_0                                           0
#define FUNC_SDIO_DATA0_GPIO20                                             1
#define FUNC_SDIO_DATA0_SPI2_CK_PAD                                        2
#define FUNC_SDIO_DATA0_DBG_FLASH_CK_PAD                                   4

#define PERIPHS_IO_MUX_U_GPIO21                     (REG_IO_MUX_BASE + 0x54)
#define FUNC_SDIO_DATA1_GPIO21_0                                           0
#define FUNC_SDIO_DATA1_GPIO21                                             1
#define FUNC_SDIO_DATA1_SPI2_D_PAD                                         2
#define FUNC_SDIO_DATA1_DBG_FLASH_D_PAD                                    4

#define PERIPHS_IO_MUX_U_GPIO22                     (REG_IO_MUX_BASE + 0x58)
#define FUNC_SDIO_DATA2_GPIO22_0                                           0
#define FUNC_SDIO_DATA2_GPIO22                                             1
#define FUNC_SDIO_DATA2_SPI2_Q_PAD                                         2
#define FUNC_SDIO_DATA2_DBG_FLASH_CS_PAD                                   4

#define PERIPHS_IO_MUX_U_GPIO23                     (REG_IO_MUX_BASE + 0x5c)
#define FUNC_SDIO_DATA3_GPIO23_0                                           0
#define FUNC_SDIO_DATA3_GPIO23                                             1
#define FUNC_SDIO_DATA3_SPI2_CS_PAD                                        2
#define FUNC_SDIO_DATA3_DBG_FLASH_Q_PAD                                    4

#define PERIPHS_IO_MUX_U_GPIO24                     (REG_IO_MUX_BASE + 0x60)
#define FUNC_SDIO_CLK_GPIO24_0                                             0
#define FUNC_SDIO_CLK_GPIO24                                               1
#define FUNC_SDIO_CLK_SPI2_HOLD_PAD                                        2
#define FUNC_SDIO_CLK_DBG_FLASH_WP_PAD                                     4

#define PERIPHS_IO_MUX_U_GPIO25                     (REG_IO_MUX_BASE + 0x64)
#define FUNC_SDIO_CMD_GPIO25_0                                             0
#define FUNC_SDIO_CMD_GPIO25                                               1
#define FUNC_SDIO_CMD_SPI2_WP_PAD                                          2
#define FUNC_SDIO_CMD_DBG_FLASH_HOLD_PAD                                   4

#define PERIPHS_IO_MUX_U_GPIO26                     (REG_IO_MUX_BASE + 0x68)
#define FUNC_SPICS_FLASH_CS_PAD                                            0
#define FUNC_SPICS_GPIO26                                                  1

#define PERIPHS_IO_MUX_U_GPIO27                     (REG_IO_MUX_BASE + 0x6c)
#define FUNC_SPIQ_FLASH_Q_PAD                                              0
#define FUNC_SPIQ_GPIO27                                                   1

#define PERIPHS_IO_MUX_U_GPIO28                     (REG_IO_MUX_BASE + 0x70)
#define FUNC_SPIWP_FLASH_WP_PAD                                            0
#define FUNC_SPIWP_GPIO28                                                  1

#define PERIPHS_IO_MUX_U_GPIO29                     (REG_IO_MUX_BASE + 0x74)
#define FUNC_VDD_SPI_GPIO29_0                                              0
#define FUNC_VDD_SPI_GPIO29                                                1

#define PERIPHS_IO_MUX_U_GPIO30                     (REG_IO_MUX_BASE + 0x78)
#define FUNC_SPIHD_FLASH_HOLD_PAD                                          0
#define FUNC_SPIHD_GPIO30                                                  1

#define PERIPHS_IO_MUX_U_GPIO31                     (REG_IO_MUX_BASE + 0x7c)
#define FUNC_SPICLK_FLASH_CK_PAD                                           0
#define FUNC_SPICLK_GPIO31                                                 1

#define PERIPHS_IO_MUX_U_GPIO32                     (REG_IO_MUX_BASE + 0x80)
#define FUNC_SPID_FLASH_D_PAD                                              0
#define FUNC_SPID_GPIO32                                                   1

#define PERIPHS_IO_MUX_U_USB_GPIO33                 (REG_IO_MUX_BASE + 0x84)
#define FUNC_GPIO33_GPIO33_0                                               0
#define FUNC_GPIO33_GPIO33                                                 1
#define FUNC_GPIO33_LCD_DATA12_OUT_PAD                                     3

#define PERIPHS_IO_MUX_U_USB_GPIO34                 (REG_IO_MUX_BASE + 0x88)
#define FUNC_GPIO34_GPIO34_0                                               0
#define FUNC_GPIO34_GPIO34                                                 1
#define FUNC_GPIO34_LCD_DATA13_OUT_PAD                                     3

#define PERIPHS_IO_MUX_U_GPIO35                     (REG_IO_MUX_BASE + 0x8c)
#define FUNC_GPIO35_GPIO35_0                                               0
#define FUNC_GPIO35_GPIO35                                                 1
#define FUNC_GPIO35_REF_GMAC_CLK_PAD                                       2
#define FUNC_GPIO35_LCD_DATA14_OUT_PAD                                     3
#define FUNC_GPIO35_SD2_CDATA0_PAD                                         4

// Strapping: LDO sel
#define PERIPHS_IO_MUX_U_GPIO36                     (REG_IO_MUX_BASE + 0x90)
#define FUNC_GPIO36_GPIO36_0                                               0
#define FUNC_GPIO36_GPIO36                                                 1
#define FUNC_GPIO36_GMAC_PHY_RXDV_PAD                                      2
#define FUNC_GPIO36_LCD_DATA15_OUT_PAD                                     3
#define FUNC_GPIO36_SD2_CDATA1_PAD                                         4

// Strapping: USB2JTAG select: 1->usb2jtag 0-> pad_jtag
#define PERIPHS_IO_MUX_U_GPIO37                     (REG_IO_MUX_BASE + 0x94)
#define FUNC_GPIO37_GPIO37_0                                               0
#define FUNC_GPIO37_GPIO37                                                 1
#define FUNC_GPIO37_GMAC_PHY_TXEN_PAD                                      2
#define FUNC_GPIO37_LCD_DATA16_OUT_PAD                                     3
#define FUNC_GPIO37_SD2_CDATA2_PAD                                         4

// Strapping: Boot Mode select 0
#define PERIPHS_IO_MUX_U_GPIO38                     (REG_IO_MUX_BASE + 0x98)
#define FUNC_GPIO38_GPIO38_0                                               0
#define FUNC_GPIO38_GPIO38                                                 1
#define FUNC_GPIO38_GMAC_PHY_RXD3_PAD                                      2
#define FUNC_GPIO38_LCD_DATA17_OUT_PAD                                     3
#define FUNC_GPIO38_SD2_CDATA3_PAD                                         4

// Strapping: Boot Mode select 1
#define PERIPHS_IO_MUX_U_GPIO39                     (REG_IO_MUX_BASE + 0x9c)
#define FUNC_GPIO39_GPIO39_0                                               0
#define FUNC_GPIO39_GPIO39                                                 1
#define FUNC_GPIO39_GMAC_PHY_RXD2_PAD                                      2
#define FUNC_GPIO39_LCD_DATA18_OUT_PAD                                     3
#define FUNC_GPIO39_SD2_CCLK_PAD                                           4

// Strapping: Boot Mode select 2
#define PERIPHS_IO_MUX_U_GPIO40                     (REG_IO_MUX_BASE + 0xa0)
#define FUNC_GPIO40_GPIO40_0                                               0
#define FUNC_GPIO40_GPIO40                                                 1
#define FUNC_GPIO40_GMAC_PHY_RXD1_PAD                                      2
#define FUNC_GPIO40_LCD_PCLK_PAD                                           3
#define FUNC_GPIO40_SD2_CCMD_PAD                                           4

#define PERIPHS_IO_MUX_U_GPIO41                     (REG_IO_MUX_BASE + 0xa4)
#define FUNC_GPIO41_GPIO41_0                                               0
#define FUNC_GPIO41_GPIO41                                                 1
#define FUNC_GPIO41_GMAC_PHY_RXD0_PAD                                      2

#define PERIPHS_IO_MUX_U_ADC_GPIO42                 (REG_IO_MUX_BASE + 0xa8)
#define FUNC_GPIO42_GPIO42_0                                               0
#define FUNC_GPIO42_GPIO42                                                 1
#define FUNC_GPIO42_GMAC_RX_CLK_PAD                                        2

#define PERIPHS_IO_MUX_U_ADC_GPIO43                 (REG_IO_MUX_BASE + 0xac)
#define FUNC_GPIO43_GPIO43_0                                               0
#define FUNC_GPIO43_GPIO43                                                 1
#define FUNC_GPIO43_GMAC_RMII_CLK_PAD                                      2
#define FUNC_GPIO43_LCD_H_ENABLE_PAD                                       3

#define PERIPHS_IO_MUX_U_ADC_GPIO44                 (REG_IO_MUX_BASE + 0xb0)
#define FUNC_GPIO44_GPIO44_0                                               0
#define FUNC_GPIO44_GPIO44                                                 1
#define FUNC_GPIO44_GMAC_PHY_TXD0_PAD                                      2
#define FUNC_GPIO44_LCD_H_SYNC_PAD                                         3

#define PERIPHS_IO_MUX_U_ADC_GPIO45                 (REG_IO_MUX_BASE + 0xb4)
#define FUNC_GPIO45_GPIO45_0                                               0
#define FUNC_GPIO45_GPIO45                                                 1
#define FUNC_GPIO45_GMAC_PHY_TXD1_PAD                                      2
#define FUNC_GPIO45_LCD_V_SYNC_PAD                                         3

#define PERIPHS_IO_MUX_U_ADC_GPIO46                 (REG_IO_MUX_BASE + 0xb8)
#define FUNC_GPIO46_GPIO46_0                                               0
#define FUNC_GPIO46_GPIO46                                                 1
#define FUNC_GPIO46_GMAC_PHY_TXD2_PAD                                      2
#define FUNC_GPIO46_CAM_DATA0_IN_PAD                                       3

#define PERIPHS_IO_MUX_U_ADC_GPIO47                 (REG_IO_MUX_BASE + 0xbc)
#define FUNC_GPIO47_GPIO47_0                                               0
#define FUNC_GPIO47_GPIO47                                                 1
#define FUNC_GPIO47_GMAC_PHY_TXD3_PAD                                      2
#define FUNC_GPIO47_CAM_DATA1_IN_PAD                                       3

#define PERIPHS_IO_MUX_U_ADC_GPIO48                 (REG_IO_MUX_BASE + 0xc0)
#define FUNC_GPIO48_GPIO48_0                                               0
#define FUNC_GPIO48_GPIO48                                                 1
#define FUNC_GPIO48_CAM_DATA2_IN_PAD                                       3

#define PERIPHS_IO_MUX_U_ADC_GPIO49                 (REG_IO_MUX_BASE + 0xc4)
#define FUNC_GPIO49_GPIO49_0                                               0
#define FUNC_GPIO49_GPIO49                                                 1
#define FUNC_GPIO49_CAM_DATA3_IN_PAD                                       3

#define PERIPHS_IO_MUX_U_ADC_GPIO50                 (REG_IO_MUX_BASE + 0xc8)
#define FUNC_GPIO50_GPIO50_0                                               0
#define FUNC_GPIO50_GPIO50                                                 1
#define FUNC_GPIO50_CAM_DATA4_IN_PAD                                       3

#define PERIPHS_IO_MUX_U_ADC_GPIO51                 (REG_IO_MUX_BASE + 0xcc)
#define FUNC_GPIO51_GPIO51_0                                               0
#define FUNC_GPIO51_GPIO51                                                 1
#define FUNC_GPIO51_CAM_DATA5_IN_PAD                                       3

#define PERIPHS_IO_MUX_U_ADC_GPIO52                 (REG_IO_MUX_BASE + 0xd0)
#define FUNC_GPIO52_GPIO52_0                                               0
#define FUNC_GPIO52_GPIO52                                                 1
#define FUNC_GPIO52_SPI2_CS_PAD                                            2
#define FUNC_GPIO52_CAM_DATA6_IN_PAD                                       3

#define PERIPHS_IO_MUX_U_ADC_GPIO53                 (REG_IO_MUX_BASE + 0xd4)
#define FUNC_GPIO53_GPIO53_0                                               0
#define FUNC_GPIO53_GPIO53                                                 1
#define FUNC_GPIO53_SPI2_CK_PAD                                            2
#define FUNC_GPIO53_CAM_DATA7_IN_PAD                                       3

#define PERIPHS_IO_MUX_U_ADC_GPIO54                 (REG_IO_MUX_BASE + 0xd8)
#define FUNC_MTDO_MTDO                                                     0
#define FUNC_MTDO_GPIO54                                                   1
#define FUNC_MTDO_SPI2_D_PAD                                               2
#define FUNC_MTDO_CAM_PCLK_PAD                                             3

#define PERIPHS_IO_MUX_U_ADC_GPIO55                 (REG_IO_MUX_BASE + 0xdc)
#define FUNC_MTCK_MTCK                                                     0
#define FUNC_MTCK_GPIO55                                                   1
#define FUNC_MTCK_SPI2_Q_PAD                                               2
#define FUNC_MTCK_CAM_XCLK_PAD                                             3

#define PERIPHS_IO_MUX_U_ADC_GPIO56                 (REG_IO_MUX_BASE + 0xe0)
#define FUNC_MTDI_MTDI                                                     0
#define FUNC_MTDI_GPIO56                                                   1
#define FUNC_MTDI_SPI2_HOLD_PAD                                            2
#define FUNC_MTDI_CAM_V_SYNC_PAD                                           3

#define PERIPHS_IO_MUX_U_ADC_GPIO57                 (REG_IO_MUX_BASE + 0xe4)
#define FUNC_MTMS_MTMS                                                     0
#define FUNC_MTMS_GPIO57                                                   1
#define FUNC_MTMS_SPI2_WP_PAD                                              2
#define FUNC_MTMS_CAM_H_SYNC_PAD                                           3

#define PERIPHS_IO_MUX_U_GPIO58                     (REG_IO_MUX_BASE + 0xe8)
#define FUNC_GPIO58_UART0_TXD_PAD                                          0
#define FUNC_GPIO58_GPIO58                                                 1

#define PERIPHS_IO_MUX_U_GPIO59                     (REG_IO_MUX_BASE + 0xec)
#define FUNC_GPIO59_UART0_RXD_PAD                                          0
#define FUNC_GPIO59_GPIO59                                                 1

// Strapping: Boot Mode select 3
#define PERIPHS_IO_MUX_U_GPIO60                     (REG_IO_MUX_BASE + 0xf0)
#define FUNC_GPIO60_GPIO60_0                                               0
#define FUNC_GPIO60_GPIO60                                                 1

// Strapping: Boot Mode select 4
#define PERIPHS_IO_MUX_U_GPIO61                     (REG_IO_MUX_BASE + 0xf4)
#define FUNC_GPIO61_GPIO61_0                                               0
#define FUNC_GPIO61_GPIO61                                                 1

#define PERIPHS_IO_MUX_U_GPIO62                     (REG_IO_MUX_BASE + 0xf8)
#define FUNC_GPIO62_GPIO62_0                                               0
#define FUNC_GPIO62_GPIO62                                                 1


#define IO_MUX_DATE_REG          (REG_IO_MUX_BASE + 0x1FC)
/* IO_MUX_REG_DATE : R/W ;bitpos:[27:0] ;default: 28'h2502070 ; */
/*description: Version control register.*/
#define IO_MUX_REG_DATE    0x0FFFFFFF
#define IO_MUX_REG_DATE_M  ((IO_MUX_REG_DATE_V)<<(IO_MUX_REG_DATE_S))
#define IO_MUX_REG_DATE_V  0xFFFFFFF
#define IO_MUX_REG_DATE_S  0
