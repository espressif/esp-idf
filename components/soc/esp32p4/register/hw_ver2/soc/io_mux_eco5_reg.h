/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

// definitions below are generated from pin_txt.csv
#define PERIPHS_IO_MUX_U_PAD_GPIO0                   (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO0_GPIO0_0                                                 0
#define FUNC_GPIO0_GPIO0                                                   1

#define PERIPHS_IO_MUX_U_PAD_GPIO1                   (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO1_GPIO1_0                                                 0
#define FUNC_GPIO1_GPIO1                                                   1

#define PERIPHS_IO_MUX_U_PAD_GPIO2                   (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO2_MTCK                                                    0
#define FUNC_GPIO2_GPIO2                                                   1

#define PERIPHS_IO_MUX_U_PAD_GPIO3                   (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO3_MTDI                                                    0
#define FUNC_GPIO3_GPIO3                                                   1

#define PERIPHS_IO_MUX_U_PAD_GPIO4                   (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO4_MTMS                                                    0
#define FUNC_GPIO4_GPIO4                                                   1

#define PERIPHS_IO_MUX_U_PAD_GPIO5                   (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO5_MTDO                                                    0
#define FUNC_GPIO5_GPIO5                                                   1

#define PERIPHS_IO_MUX_U_PAD_GPIO6                   (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO6_GPIO6_0                                                 0
#define FUNC_GPIO6_GPIO6                                                   1
#define FUNC_GPIO6_SPI2_HOLD_PAD                                           3

#define PERIPHS_IO_MUX_U_PAD_GPIO7                   (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO7_GPIO7_0                                                 0
#define FUNC_GPIO7_GPIO7                                                   1
#define FUNC_GPIO7_SPI2_CS_PAD                                             3

#define PERIPHS_IO_MUX_U_PAD_GPIO8                   (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO8_GPIO8_0                                                 0
#define FUNC_GPIO8_GPIO8                                                   1
#define FUNC_GPIO8_UART0_RTS_PAD                                           2
#define FUNC_GPIO8_SPI2_D_PAD                                              3

#define PERIPHS_IO_MUX_U_PAD_GPIO9                   (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO9_GPIO9_0                                                 0
#define FUNC_GPIO9_GPIO9                                                   1
#define FUNC_GPIO9_UART0_CTS_PAD                                           2
#define FUNC_GPIO9_SPI2_CK_PAD                                             3

#define PERIPHS_IO_MUX_U_PAD_GPIO10                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO10_GPIO10_0                                               0
#define FUNC_GPIO10_GPIO10                                                 1
#define FUNC_GPIO10_UART1_TXD_PAD                                          2
#define FUNC_GPIO10_SPI2_Q_PAD                                             3

#define PERIPHS_IO_MUX_U_PAD_GPIO11                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO11_GPIO11_0                                               0
#define FUNC_GPIO11_GPIO11                                                 1
#define FUNC_GPIO11_UART1_RXD_PAD                                          2
#define FUNC_GPIO11_SPI2_WP_PAD                                            3

#define PERIPHS_IO_MUX_U_PAD_GPIO12                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO12_GPIO12_0                                               0
#define FUNC_GPIO12_GPIO12                                                 1
#define FUNC_GPIO12_UART1_RTS_PAD                                          2

#define PERIPHS_IO_MUX_U_PAD_GPIO13                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO13_GPIO13_0                                               0
#define FUNC_GPIO13_GPIO13                                                 1
#define FUNC_GPIO13_UART1_CTS_PAD                                          2

#define PERIPHS_IO_MUX_U_PAD_GPIO14                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO14_GPIO14_0                                               0
#define FUNC_GPIO14_GPIO14                                                 1

#define PERIPHS_IO_MUX_U_PAD_GPIO15                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO15_GPIO15_0                                               0
#define FUNC_GPIO15_GPIO15                                                 1

#define PERIPHS_IO_MUX_U_PAD_GPIO16                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO16_GPIO16_0                                               0
#define FUNC_GPIO16_GPIO16                                                 1

#define PERIPHS_IO_MUX_U_PAD_GPIO17                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO17_GPIO17_0                                               0
#define FUNC_GPIO17_GPIO17                                                 1

#define PERIPHS_IO_MUX_U_PAD_GPIO18                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO18_GPIO18_0                                               0
#define FUNC_GPIO18_GPIO18                                                 1

#define PERIPHS_IO_MUX_U_PAD_GPIO19                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO19_GPIO19_0                                               0
#define FUNC_GPIO19_GPIO19                                                 1

#define PERIPHS_IO_MUX_U_PAD_GPIO20                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO20_GPIO20_0                                               0
#define FUNC_GPIO20_GPIO20                                                 1

#define PERIPHS_IO_MUX_U_PAD_GPIO21                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO21_GPIO21_0                                               0
#define FUNC_GPIO21_GPIO21                                                 1

#define PERIPHS_IO_MUX_U_PAD_GPIO22                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO22_GPIO22_0                                               0
#define FUNC_GPIO22_GPIO22                                                 1
#define FUNC_GPIO22_DBG_PSRAM_CK_PAD                                       4

#define PERIPHS_IO_MUX_U_PAD_GPIO23                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO23_GPIO23_0                                               0
#define FUNC_GPIO23_GPIO23                                                 1
#define FUNC_GPIO23_REF_50M_CLK_PAD                                        3
#define FUNC_GPIO23_DBG_PSRAM_CS_PAD                                       4

#define PERIPHS_IO_MUX_U_PAD_GPIO24                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO24_GPIO24_0                                               0
#define FUNC_GPIO24_GPIO24                                                 1

#define PERIPHS_IO_MUX_U_PAD_GPIO25                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO25_GPIO25_0                                               0
#define FUNC_GPIO25_GPIO25                                                 1

#define PERIPHS_IO_MUX_U_PAD_GPIO26                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO26_GPIO26_0                                               0
#define FUNC_GPIO26_GPIO26                                                 1

#define PERIPHS_IO_MUX_U_PAD_GPIO27                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO27_GPIO27_0                                               0
#define FUNC_GPIO27_GPIO27                                                 1

#define PERIPHS_IO_MUX_U_PAD_GPIO28                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO28_GPIO28_0                                               0
#define FUNC_GPIO28_GPIO28                                                 1
#define FUNC_GPIO28_SPI2_CS_PAD                                            2
#define FUNC_GPIO28_GMAC_PHY_RXDV_PAD                                      3
#define FUNC_GPIO28_DBG_PSRAM_D_PAD                                        4

#define PERIPHS_IO_MUX_U_PAD_GPIO29                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO29_GPIO29_0                                               0
#define FUNC_GPIO29_GPIO29                                                 1
#define FUNC_GPIO29_SPI2_D_PAD                                             2
#define FUNC_GPIO29_GMAC_PHY_RXD0_PAD                                      3
#define FUNC_GPIO29_DBG_PSRAM_Q_PAD                                        4

#define PERIPHS_IO_MUX_U_PAD_GPIO30                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO30_GPIO30_0                                               0
#define FUNC_GPIO30_GPIO30                                                 1
#define FUNC_GPIO30_SPI2_CK_PAD                                            2
#define FUNC_GPIO30_GMAC_PHY_RXD1_PAD                                      3
#define FUNC_GPIO30_DBG_PSRAM_WP_PAD                                       4

#define PERIPHS_IO_MUX_U_PAD_GPIO31                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO31_GPIO31_0                                               0
#define FUNC_GPIO31_GPIO31                                                 1
#define FUNC_GPIO31_SPI2_Q_PAD                                             2
#define FUNC_GPIO31_GMAC_PHY_RXER_PAD                                      3
#define FUNC_GPIO31_DBG_PSRAM_HOLD_PAD                                     4

// Strapping: Diag Group Sel1
#define PERIPHS_IO_MUX_U_PAD_GPIO32                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO32_GPIO32_0                                               0
#define FUNC_GPIO32_GPIO32                                                 1
#define FUNC_GPIO32_SPI2_HOLD_PAD                                          2
#define FUNC_GPIO32_GMAC_RMII_CLK_PAD                                      3
#define FUNC_GPIO32_DBG_PSRAM_DQ4_PAD                                      4

// Strapping: Diag Group Sel0
#define PERIPHS_IO_MUX_U_PAD_GPIO33                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO33_GPIO33_0                                               0
#define FUNC_GPIO33_GPIO33                                                 1
#define FUNC_GPIO33_SPI2_WP_PAD                                            2
#define FUNC_GPIO33_GMAC_PHY_TXEN_PAD                                      3
#define FUNC_GPIO33_DBG_PSRAM_DQ5_PAD                                      4

// Strapping: USB2JTAG select: 1->usb2jtag 0-> pad_jtag
#define PERIPHS_IO_MUX_U_PAD_GPIO34                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO34_GPIO34_0                                               0
#define FUNC_GPIO34_GPIO34                                                 1
#define FUNC_GPIO34_SPI2_IO4_PAD                                           2
#define FUNC_GPIO34_GMAC_PHY_TXD0_PAD                                      3
#define FUNC_GPIO34_DBG_PSRAM_DQ6_PAD                                      4

// Strapping: Boot Mode select 3
#define PERIPHS_IO_MUX_U_PAD_GPIO35                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO35_GPIO35_0                                               0
#define FUNC_GPIO35_GPIO35                                                 1
#define FUNC_GPIO35_SPI2_IO5_PAD                                           2
#define FUNC_GPIO35_GMAC_PHY_TXD1_PAD                                      3
#define FUNC_GPIO35_DBG_PSRAM_DQ7_PAD                                      4

// Strapping: Boot Mode select 2
#define PERIPHS_IO_MUX_U_PAD_GPIO36                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO36_GPIO36_0                                               0
#define FUNC_GPIO36_GPIO36                                                 1
#define FUNC_GPIO36_SPI2_IO6_PAD                                           2
#define FUNC_GPIO36_GMAC_PHY_TXER_PAD                                      3
#define FUNC_GPIO36_DBG_PSRAM_DQS_0_PAD                                    4

// Strapping: Boot Mode select 1
#define PERIPHS_IO_MUX_U_PAD_GPIO37                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO37_UART0_TXD_PAD                                          0
#define FUNC_GPIO37_GPIO37                                                 1
#define FUNC_GPIO37_SPI2_IO7_PAD                                           2

// Strapping: Boot Mode select 0
#define PERIPHS_IO_MUX_U_PAD_GPIO38                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO38_UART0_RXD_PAD                                          0
#define FUNC_GPIO38_GPIO38                                                 1
#define FUNC_GPIO38_SPI2_DQS_PAD                                           2

#define PERIPHS_IO_MUX_U_PAD_GPIO39                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO39_SD1_CDATA0_PAD                                         0
#define FUNC_GPIO39_GPIO39                                                 1
#define FUNC_GPIO39_BIST_PAD                                               2
#define FUNC_GPIO39_REF_50M_CLK_PAD                                        3
#define FUNC_GPIO39_DBG_PSRAM_DQ8_PAD                                      4

#define PERIPHS_IO_MUX_U_PAD_GPIO40                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO40_SD1_CDATA1_PAD                                         0
#define FUNC_GPIO40_GPIO40                                                 1
#define FUNC_GPIO40_BIST_PAD                                               2
#define FUNC_GPIO40_GMAC_PHY_TXEN_PAD                                      3
#define FUNC_GPIO40_DBG_PSRAM_DQ9_PAD                                      4

#define PERIPHS_IO_MUX_U_PAD_GPIO41                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO41_SD1_CDATA2_PAD                                         0
#define FUNC_GPIO41_GPIO41                                                 1
#define FUNC_GPIO41_BIST_PAD                                               2
#define FUNC_GPIO41_GMAC_PHY_TXD0_PAD                                      3
#define FUNC_GPIO41_DBG_PSRAM_DQ10_PAD                                     4

#define PERIPHS_IO_MUX_U_PAD_GPIO42                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO42_SD1_CDATA3_PAD                                         0
#define FUNC_GPIO42_GPIO42                                                 1
#define FUNC_GPIO42_BIST_PAD                                               2
#define FUNC_GPIO42_GMAC_PHY_TXD1_PAD                                      3
#define FUNC_GPIO42_DBG_PSRAM_DQ11_PAD                                     4

#define PERIPHS_IO_MUX_U_PAD_GPIO43                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO43_SD1_CCLK_PAD                                           0
#define FUNC_GPIO43_GPIO43                                                 1
#define FUNC_GPIO43_BIST_PAD                                               2
#define FUNC_GPIO43_GMAC_PHY_TXER_PAD                                      3
#define FUNC_GPIO43_DBG_PSRAM_DQ12_PAD                                     4

#define PERIPHS_IO_MUX_U_PAD_GPIO44                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO44_SD1_CCMD_PAD                                           0
#define FUNC_GPIO44_GPIO44                                                 1
#define FUNC_GPIO44_BIST_PAD                                               2
#define FUNC_GPIO44_GMAC_RMII_CLK_PAD                                      3
#define FUNC_GPIO44_DBG_PSRAM_DQ13_PAD                                     4

#define PERIPHS_IO_MUX_U_PAD_GPIO45                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO45_SD1_CDATA4_PAD                                         0
#define FUNC_GPIO45_GPIO45                                                 1
#define FUNC_GPIO45_BIST_PAD                                               2
#define FUNC_GPIO45_GMAC_PHY_RXDV_PAD                                      3
#define FUNC_GPIO45_DBG_PSRAM_DQ14_PAD                                     4

#define PERIPHS_IO_MUX_U_PAD_GPIO46                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO46_SD1_CDATA5_PAD                                         0
#define FUNC_GPIO46_GPIO46                                                 1
#define FUNC_GPIO46_BIST_PAD                                               2
#define FUNC_GPIO46_GMAC_PHY_RXD0_PAD                                      3
#define FUNC_GPIO46_DBG_PSRAM_DQ15_PAD                                     4

#define PERIPHS_IO_MUX_U_PAD_GPIO47                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO47_SD1_CDATA6_PAD                                         0
#define FUNC_GPIO47_GPIO47                                                 1
#define FUNC_GPIO47_BIST_PAD                                               2
#define FUNC_GPIO47_GMAC_PHY_RXD1_PAD                                      3
#define FUNC_GPIO47_DBG_PSRAM_DQS_1_PAD                                    4

#define PERIPHS_IO_MUX_U_PAD_GPIO48                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO48_SD1_CDATA7_PAD                                         0
#define FUNC_GPIO48_GPIO48                                                 1
#define FUNC_GPIO48_BIST_PAD                                               2
#define FUNC_GPIO48_GMAC_PHY_RXER_PAD                                      3

#define PERIPHS_IO_MUX_U_PAD_GPIO49                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO49_GPIO49_0                                               0
#define FUNC_GPIO49_GPIO49                                                 1
#define FUNC_GPIO49_GMAC_PHY_TXEN_PAD                                      3
#define FUNC_GPIO49_DBG_FLASH_CS_PAD                                       4

#define PERIPHS_IO_MUX_U_PAD_GPIO50                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO50_GPIO50_0                                               0
#define FUNC_GPIO50_GPIO50                                                 1
#define FUNC_GPIO50_GMAC_RMII_CLK_PAD                                      3
#define FUNC_GPIO50_DBG_FLASH_Q_PAD                                        4

#define PERIPHS_IO_MUX_U_PAD_GPIO51                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO51_GPIO51_0                                               0
#define FUNC_GPIO51_GPIO51                                                 1
#define FUNC_GPIO51_GMAC_PHY_RXDV_PAD                                      3
#define FUNC_GPIO51_DBG_FLASH_WP_PAD                                       4

#define PERIPHS_IO_MUX_U_PAD_GPIO52                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO52_GPIO52_0                                               0
#define FUNC_GPIO52_GPIO52                                                 1
#define FUNC_GPIO52_GMAC_PHY_RXD0_PAD                                      3
#define FUNC_GPIO52_DBG_FLASH_HOLD_PAD                                     4

#define PERIPHS_IO_MUX_U_PAD_GPIO53                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO53_GPIO53_0                                               0
#define FUNC_GPIO53_GPIO53                                                 1
#define FUNC_GPIO53_GMAC_PHY_RXD1_PAD                                      3
#define FUNC_GPIO53_DBG_FLASH_CK_PAD                                       4

#define PERIPHS_IO_MUX_U_PAD_GPIO54                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO54_GPIO54_0                                               0
#define FUNC_GPIO54_GPIO54                                                 1
#define FUNC_GPIO54_GMAC_PHY_RXER_PAD                                      3
#define FUNC_GPIO54_DBG_FLASH_D_PAD                                        4

#define PERIPHS_IO_MUX_U_PAD_GPIO55                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO55_GPIO55_0                                               0
#define FUNC_GPIO55_GPIO55                                                 1

#define PERIPHS_IO_MUX_U_PAD_GPIO56                  (REG_IO_MUX_BASE + 0x0)
#define FUNC_GPIO56_GPIO56_0                                               0
#define FUNC_GPIO56_GPIO56                                                 1


/** IO_MUX_gpio0_REG register
 *  iomux control register for gpio0
 */
#define IO_MUX_GPIO0_REG (DR_REG_IO_MUX_BASE + 0x4)
/** IO_MUX_GPIO0_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO0_MCU_OE    (BIT(0))
#define IO_MUX_GPIO0_MCU_OE_M  (IO_MUX_GPIO0_MCU_OE_V << IO_MUX_GPIO0_MCU_OE_S)
#define IO_MUX_GPIO0_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO0_MCU_OE_S  0
/** IO_MUX_GPIO0_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO0_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO0_SLP_SEL_M  (IO_MUX_GPIO0_SLP_SEL_V << IO_MUX_GPIO0_SLP_SEL_S)
#define IO_MUX_GPIO0_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO0_SLP_SEL_S  1
/** IO_MUX_GPIO0_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO0_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO0_MCU_WPD_M  (IO_MUX_GPIO0_MCU_WPD_V << IO_MUX_GPIO0_MCU_WPD_S)
#define IO_MUX_GPIO0_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO0_MCU_WPD_S  2
/** IO_MUX_GPIO0_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO0_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO0_MCU_WPU_M  (IO_MUX_GPIO0_MCU_WPU_V << IO_MUX_GPIO0_MCU_WPU_S)
#define IO_MUX_GPIO0_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO0_MCU_WPU_S  3
/** IO_MUX_GPIO0_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO0_MCU_IE    (BIT(4))
#define IO_MUX_GPIO0_MCU_IE_M  (IO_MUX_GPIO0_MCU_IE_V << IO_MUX_GPIO0_MCU_IE_S)
#define IO_MUX_GPIO0_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO0_MCU_IE_S  4
/** IO_MUX_GPIO0_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO0_MCU_DRV    0x00000003U
#define IO_MUX_GPIO0_MCU_DRV_M  (IO_MUX_GPIO0_MCU_DRV_V << IO_MUX_GPIO0_MCU_DRV_S)
#define IO_MUX_GPIO0_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO0_MCU_DRV_S  5
/** IO_MUX_GPIO0_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO0_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO0_FUN_WPD_M  (IO_MUX_GPIO0_FUN_WPD_V << IO_MUX_GPIO0_FUN_WPD_S)
#define IO_MUX_GPIO0_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO0_FUN_WPD_S  7
/** IO_MUX_GPIO0_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO0_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO0_FUN_WPU_M  (IO_MUX_GPIO0_FUN_WPU_V << IO_MUX_GPIO0_FUN_WPU_S)
#define IO_MUX_GPIO0_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO0_FUN_WPU_S  8
/** IO_MUX_GPIO0_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO0_FUN_IE    (BIT(9))
#define IO_MUX_GPIO0_FUN_IE_M  (IO_MUX_GPIO0_FUN_IE_V << IO_MUX_GPIO0_FUN_IE_S)
#define IO_MUX_GPIO0_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO0_FUN_IE_S  9
/** IO_MUX_GPIO0_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO0_FUN_DRV    0x00000003U
#define IO_MUX_GPIO0_FUN_DRV_M  (IO_MUX_GPIO0_FUN_DRV_V << IO_MUX_GPIO0_FUN_DRV_S)
#define IO_MUX_GPIO0_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO0_FUN_DRV_S  10
/** IO_MUX_GPIO0_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO0_MCU_SEL    0x00000007U
#define IO_MUX_GPIO0_MCU_SEL_M  (IO_MUX_GPIO0_MCU_SEL_V << IO_MUX_GPIO0_MCU_SEL_S)
#define IO_MUX_GPIO0_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO0_MCU_SEL_S  12
/** IO_MUX_GPIO0_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO0_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO0_FILTER_EN_M  (IO_MUX_GPIO0_FILTER_EN_V << IO_MUX_GPIO0_FILTER_EN_S)
#define IO_MUX_GPIO0_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO0_FILTER_EN_S  15

/** IO_MUX_gpio1_REG register
 *  iomux control register for gpio1
 */
#define IO_MUX_GPIO1_REG (DR_REG_IO_MUX_BASE + 0x8)
/** IO_MUX_GPIO1_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO1_MCU_OE    (BIT(0))
#define IO_MUX_GPIO1_MCU_OE_M  (IO_MUX_GPIO1_MCU_OE_V << IO_MUX_GPIO1_MCU_OE_S)
#define IO_MUX_GPIO1_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO1_MCU_OE_S  0
/** IO_MUX_GPIO1_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO1_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO1_SLP_SEL_M  (IO_MUX_GPIO1_SLP_SEL_V << IO_MUX_GPIO1_SLP_SEL_S)
#define IO_MUX_GPIO1_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO1_SLP_SEL_S  1
/** IO_MUX_GPIO1_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO1_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO1_MCU_WPD_M  (IO_MUX_GPIO1_MCU_WPD_V << IO_MUX_GPIO1_MCU_WPD_S)
#define IO_MUX_GPIO1_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO1_MCU_WPD_S  2
/** IO_MUX_GPIO1_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO1_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO1_MCU_WPU_M  (IO_MUX_GPIO1_MCU_WPU_V << IO_MUX_GPIO1_MCU_WPU_S)
#define IO_MUX_GPIO1_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO1_MCU_WPU_S  3
/** IO_MUX_GPIO1_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO1_MCU_IE    (BIT(4))
#define IO_MUX_GPIO1_MCU_IE_M  (IO_MUX_GPIO1_MCU_IE_V << IO_MUX_GPIO1_MCU_IE_S)
#define IO_MUX_GPIO1_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO1_MCU_IE_S  4
/** IO_MUX_GPIO1_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO1_MCU_DRV    0x00000003U
#define IO_MUX_GPIO1_MCU_DRV_M  (IO_MUX_GPIO1_MCU_DRV_V << IO_MUX_GPIO1_MCU_DRV_S)
#define IO_MUX_GPIO1_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO1_MCU_DRV_S  5
/** IO_MUX_GPIO1_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO1_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO1_FUN_WPD_M  (IO_MUX_GPIO1_FUN_WPD_V << IO_MUX_GPIO1_FUN_WPD_S)
#define IO_MUX_GPIO1_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO1_FUN_WPD_S  7
/** IO_MUX_GPIO1_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO1_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO1_FUN_WPU_M  (IO_MUX_GPIO1_FUN_WPU_V << IO_MUX_GPIO1_FUN_WPU_S)
#define IO_MUX_GPIO1_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO1_FUN_WPU_S  8
/** IO_MUX_GPIO1_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO1_FUN_IE    (BIT(9))
#define IO_MUX_GPIO1_FUN_IE_M  (IO_MUX_GPIO1_FUN_IE_V << IO_MUX_GPIO1_FUN_IE_S)
#define IO_MUX_GPIO1_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO1_FUN_IE_S  9
/** IO_MUX_GPIO1_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO1_FUN_DRV    0x00000003U
#define IO_MUX_GPIO1_FUN_DRV_M  (IO_MUX_GPIO1_FUN_DRV_V << IO_MUX_GPIO1_FUN_DRV_S)
#define IO_MUX_GPIO1_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO1_FUN_DRV_S  10
/** IO_MUX_GPIO1_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO1_MCU_SEL    0x00000007U
#define IO_MUX_GPIO1_MCU_SEL_M  (IO_MUX_GPIO1_MCU_SEL_V << IO_MUX_GPIO1_MCU_SEL_S)
#define IO_MUX_GPIO1_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO1_MCU_SEL_S  12
/** IO_MUX_GPIO1_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO1_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO1_FILTER_EN_M  (IO_MUX_GPIO1_FILTER_EN_V << IO_MUX_GPIO1_FILTER_EN_S)
#define IO_MUX_GPIO1_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO1_FILTER_EN_S  15

/** IO_MUX_gpio2_REG register
 *  iomux control register for gpio2
 */
#define IO_MUX_GPIO2_REG (DR_REG_IO_MUX_BASE + 0xc)
/** IO_MUX_GPIO2_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO2_MCU_OE    (BIT(0))
#define IO_MUX_GPIO2_MCU_OE_M  (IO_MUX_GPIO2_MCU_OE_V << IO_MUX_GPIO2_MCU_OE_S)
#define IO_MUX_GPIO2_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO2_MCU_OE_S  0
/** IO_MUX_GPIO2_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO2_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO2_SLP_SEL_M  (IO_MUX_GPIO2_SLP_SEL_V << IO_MUX_GPIO2_SLP_SEL_S)
#define IO_MUX_GPIO2_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO2_SLP_SEL_S  1
/** IO_MUX_GPIO2_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO2_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO2_MCU_WPD_M  (IO_MUX_GPIO2_MCU_WPD_V << IO_MUX_GPIO2_MCU_WPD_S)
#define IO_MUX_GPIO2_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO2_MCU_WPD_S  2
/** IO_MUX_GPIO2_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO2_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO2_MCU_WPU_M  (IO_MUX_GPIO2_MCU_WPU_V << IO_MUX_GPIO2_MCU_WPU_S)
#define IO_MUX_GPIO2_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO2_MCU_WPU_S  3
/** IO_MUX_GPIO2_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO2_MCU_IE    (BIT(4))
#define IO_MUX_GPIO2_MCU_IE_M  (IO_MUX_GPIO2_MCU_IE_V << IO_MUX_GPIO2_MCU_IE_S)
#define IO_MUX_GPIO2_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO2_MCU_IE_S  4
/** IO_MUX_GPIO2_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO2_MCU_DRV    0x00000003U
#define IO_MUX_GPIO2_MCU_DRV_M  (IO_MUX_GPIO2_MCU_DRV_V << IO_MUX_GPIO2_MCU_DRV_S)
#define IO_MUX_GPIO2_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO2_MCU_DRV_S  5
/** IO_MUX_GPIO2_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO2_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO2_FUN_WPD_M  (IO_MUX_GPIO2_FUN_WPD_V << IO_MUX_GPIO2_FUN_WPD_S)
#define IO_MUX_GPIO2_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO2_FUN_WPD_S  7
/** IO_MUX_GPIO2_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO2_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO2_FUN_WPU_M  (IO_MUX_GPIO2_FUN_WPU_V << IO_MUX_GPIO2_FUN_WPU_S)
#define IO_MUX_GPIO2_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO2_FUN_WPU_S  8
/** IO_MUX_GPIO2_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO2_FUN_IE    (BIT(9))
#define IO_MUX_GPIO2_FUN_IE_M  (IO_MUX_GPIO2_FUN_IE_V << IO_MUX_GPIO2_FUN_IE_S)
#define IO_MUX_GPIO2_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO2_FUN_IE_S  9
/** IO_MUX_GPIO2_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO2_FUN_DRV    0x00000003U
#define IO_MUX_GPIO2_FUN_DRV_M  (IO_MUX_GPIO2_FUN_DRV_V << IO_MUX_GPIO2_FUN_DRV_S)
#define IO_MUX_GPIO2_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO2_FUN_DRV_S  10
/** IO_MUX_GPIO2_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO2_MCU_SEL    0x00000007U
#define IO_MUX_GPIO2_MCU_SEL_M  (IO_MUX_GPIO2_MCU_SEL_V << IO_MUX_GPIO2_MCU_SEL_S)
#define IO_MUX_GPIO2_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO2_MCU_SEL_S  12
/** IO_MUX_GPIO2_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO2_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO2_FILTER_EN_M  (IO_MUX_GPIO2_FILTER_EN_V << IO_MUX_GPIO2_FILTER_EN_S)
#define IO_MUX_GPIO2_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO2_FILTER_EN_S  15

/** IO_MUX_gpio3_REG register
 *  iomux control register for gpio3
 */
#define IO_MUX_GPIO3_REG (DR_REG_IO_MUX_BASE + 0x10)
/** IO_MUX_GPIO3_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO3_MCU_OE    (BIT(0))
#define IO_MUX_GPIO3_MCU_OE_M  (IO_MUX_GPIO3_MCU_OE_V << IO_MUX_GPIO3_MCU_OE_S)
#define IO_MUX_GPIO3_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO3_MCU_OE_S  0
/** IO_MUX_GPIO3_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO3_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO3_SLP_SEL_M  (IO_MUX_GPIO3_SLP_SEL_V << IO_MUX_GPIO3_SLP_SEL_S)
#define IO_MUX_GPIO3_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO3_SLP_SEL_S  1
/** IO_MUX_GPIO3_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO3_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO3_MCU_WPD_M  (IO_MUX_GPIO3_MCU_WPD_V << IO_MUX_GPIO3_MCU_WPD_S)
#define IO_MUX_GPIO3_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO3_MCU_WPD_S  2
/** IO_MUX_GPIO3_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO3_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO3_MCU_WPU_M  (IO_MUX_GPIO3_MCU_WPU_V << IO_MUX_GPIO3_MCU_WPU_S)
#define IO_MUX_GPIO3_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO3_MCU_WPU_S  3
/** IO_MUX_GPIO3_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO3_MCU_IE    (BIT(4))
#define IO_MUX_GPIO3_MCU_IE_M  (IO_MUX_GPIO3_MCU_IE_V << IO_MUX_GPIO3_MCU_IE_S)
#define IO_MUX_GPIO3_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO3_MCU_IE_S  4
/** IO_MUX_GPIO3_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO3_MCU_DRV    0x00000003U
#define IO_MUX_GPIO3_MCU_DRV_M  (IO_MUX_GPIO3_MCU_DRV_V << IO_MUX_GPIO3_MCU_DRV_S)
#define IO_MUX_GPIO3_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO3_MCU_DRV_S  5
/** IO_MUX_GPIO3_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO3_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO3_FUN_WPD_M  (IO_MUX_GPIO3_FUN_WPD_V << IO_MUX_GPIO3_FUN_WPD_S)
#define IO_MUX_GPIO3_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO3_FUN_WPD_S  7
/** IO_MUX_GPIO3_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO3_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO3_FUN_WPU_M  (IO_MUX_GPIO3_FUN_WPU_V << IO_MUX_GPIO3_FUN_WPU_S)
#define IO_MUX_GPIO3_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO3_FUN_WPU_S  8
/** IO_MUX_GPIO3_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO3_FUN_IE    (BIT(9))
#define IO_MUX_GPIO3_FUN_IE_M  (IO_MUX_GPIO3_FUN_IE_V << IO_MUX_GPIO3_FUN_IE_S)
#define IO_MUX_GPIO3_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO3_FUN_IE_S  9
/** IO_MUX_GPIO3_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO3_FUN_DRV    0x00000003U
#define IO_MUX_GPIO3_FUN_DRV_M  (IO_MUX_GPIO3_FUN_DRV_V << IO_MUX_GPIO3_FUN_DRV_S)
#define IO_MUX_GPIO3_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO3_FUN_DRV_S  10
/** IO_MUX_GPIO3_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO3_MCU_SEL    0x00000007U
#define IO_MUX_GPIO3_MCU_SEL_M  (IO_MUX_GPIO3_MCU_SEL_V << IO_MUX_GPIO3_MCU_SEL_S)
#define IO_MUX_GPIO3_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO3_MCU_SEL_S  12
/** IO_MUX_GPIO3_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO3_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO3_FILTER_EN_M  (IO_MUX_GPIO3_FILTER_EN_V << IO_MUX_GPIO3_FILTER_EN_S)
#define IO_MUX_GPIO3_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO3_FILTER_EN_S  15

/** IO_MUX_gpio4_REG register
 *  iomux control register for gpio4
 */
#define IO_MUX_GPIO4_REG (DR_REG_IO_MUX_BASE + 0x14)
/** IO_MUX_GPIO4_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO4_MCU_OE    (BIT(0))
#define IO_MUX_GPIO4_MCU_OE_M  (IO_MUX_GPIO4_MCU_OE_V << IO_MUX_GPIO4_MCU_OE_S)
#define IO_MUX_GPIO4_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO4_MCU_OE_S  0
/** IO_MUX_GPIO4_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO4_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO4_SLP_SEL_M  (IO_MUX_GPIO4_SLP_SEL_V << IO_MUX_GPIO4_SLP_SEL_S)
#define IO_MUX_GPIO4_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO4_SLP_SEL_S  1
/** IO_MUX_GPIO4_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO4_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO4_MCU_WPD_M  (IO_MUX_GPIO4_MCU_WPD_V << IO_MUX_GPIO4_MCU_WPD_S)
#define IO_MUX_GPIO4_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO4_MCU_WPD_S  2
/** IO_MUX_GPIO4_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO4_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO4_MCU_WPU_M  (IO_MUX_GPIO4_MCU_WPU_V << IO_MUX_GPIO4_MCU_WPU_S)
#define IO_MUX_GPIO4_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO4_MCU_WPU_S  3
/** IO_MUX_GPIO4_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO4_MCU_IE    (BIT(4))
#define IO_MUX_GPIO4_MCU_IE_M  (IO_MUX_GPIO4_MCU_IE_V << IO_MUX_GPIO4_MCU_IE_S)
#define IO_MUX_GPIO4_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO4_MCU_IE_S  4
/** IO_MUX_GPIO4_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO4_MCU_DRV    0x00000003U
#define IO_MUX_GPIO4_MCU_DRV_M  (IO_MUX_GPIO4_MCU_DRV_V << IO_MUX_GPIO4_MCU_DRV_S)
#define IO_MUX_GPIO4_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO4_MCU_DRV_S  5
/** IO_MUX_GPIO4_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO4_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO4_FUN_WPD_M  (IO_MUX_GPIO4_FUN_WPD_V << IO_MUX_GPIO4_FUN_WPD_S)
#define IO_MUX_GPIO4_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO4_FUN_WPD_S  7
/** IO_MUX_GPIO4_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO4_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO4_FUN_WPU_M  (IO_MUX_GPIO4_FUN_WPU_V << IO_MUX_GPIO4_FUN_WPU_S)
#define IO_MUX_GPIO4_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO4_FUN_WPU_S  8
/** IO_MUX_GPIO4_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO4_FUN_IE    (BIT(9))
#define IO_MUX_GPIO4_FUN_IE_M  (IO_MUX_GPIO4_FUN_IE_V << IO_MUX_GPIO4_FUN_IE_S)
#define IO_MUX_GPIO4_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO4_FUN_IE_S  9
/** IO_MUX_GPIO4_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO4_FUN_DRV    0x00000003U
#define IO_MUX_GPIO4_FUN_DRV_M  (IO_MUX_GPIO4_FUN_DRV_V << IO_MUX_GPIO4_FUN_DRV_S)
#define IO_MUX_GPIO4_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO4_FUN_DRV_S  10
/** IO_MUX_GPIO4_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO4_MCU_SEL    0x00000007U
#define IO_MUX_GPIO4_MCU_SEL_M  (IO_MUX_GPIO4_MCU_SEL_V << IO_MUX_GPIO4_MCU_SEL_S)
#define IO_MUX_GPIO4_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO4_MCU_SEL_S  12
/** IO_MUX_GPIO4_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO4_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO4_FILTER_EN_M  (IO_MUX_GPIO4_FILTER_EN_V << IO_MUX_GPIO4_FILTER_EN_S)
#define IO_MUX_GPIO4_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO4_FILTER_EN_S  15

/** IO_MUX_gpio5_REG register
 *  iomux control register for gpio5
 */
#define IO_MUX_GPIO5_REG (DR_REG_IO_MUX_BASE + 0x18)
/** IO_MUX_GPIO5_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO5_MCU_OE    (BIT(0))
#define IO_MUX_GPIO5_MCU_OE_M  (IO_MUX_GPIO5_MCU_OE_V << IO_MUX_GPIO5_MCU_OE_S)
#define IO_MUX_GPIO5_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO5_MCU_OE_S  0
/** IO_MUX_GPIO5_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO5_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO5_SLP_SEL_M  (IO_MUX_GPIO5_SLP_SEL_V << IO_MUX_GPIO5_SLP_SEL_S)
#define IO_MUX_GPIO5_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO5_SLP_SEL_S  1
/** IO_MUX_GPIO5_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO5_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO5_MCU_WPD_M  (IO_MUX_GPIO5_MCU_WPD_V << IO_MUX_GPIO5_MCU_WPD_S)
#define IO_MUX_GPIO5_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO5_MCU_WPD_S  2
/** IO_MUX_GPIO5_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO5_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO5_MCU_WPU_M  (IO_MUX_GPIO5_MCU_WPU_V << IO_MUX_GPIO5_MCU_WPU_S)
#define IO_MUX_GPIO5_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO5_MCU_WPU_S  3
/** IO_MUX_GPIO5_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO5_MCU_IE    (BIT(4))
#define IO_MUX_GPIO5_MCU_IE_M  (IO_MUX_GPIO5_MCU_IE_V << IO_MUX_GPIO5_MCU_IE_S)
#define IO_MUX_GPIO5_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO5_MCU_IE_S  4
/** IO_MUX_GPIO5_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO5_MCU_DRV    0x00000003U
#define IO_MUX_GPIO5_MCU_DRV_M  (IO_MUX_GPIO5_MCU_DRV_V << IO_MUX_GPIO5_MCU_DRV_S)
#define IO_MUX_GPIO5_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO5_MCU_DRV_S  5
/** IO_MUX_GPIO5_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO5_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO5_FUN_WPD_M  (IO_MUX_GPIO5_FUN_WPD_V << IO_MUX_GPIO5_FUN_WPD_S)
#define IO_MUX_GPIO5_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO5_FUN_WPD_S  7
/** IO_MUX_GPIO5_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO5_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO5_FUN_WPU_M  (IO_MUX_GPIO5_FUN_WPU_V << IO_MUX_GPIO5_FUN_WPU_S)
#define IO_MUX_GPIO5_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO5_FUN_WPU_S  8
/** IO_MUX_GPIO5_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO5_FUN_IE    (BIT(9))
#define IO_MUX_GPIO5_FUN_IE_M  (IO_MUX_GPIO5_FUN_IE_V << IO_MUX_GPIO5_FUN_IE_S)
#define IO_MUX_GPIO5_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO5_FUN_IE_S  9
/** IO_MUX_GPIO5_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO5_FUN_DRV    0x00000003U
#define IO_MUX_GPIO5_FUN_DRV_M  (IO_MUX_GPIO5_FUN_DRV_V << IO_MUX_GPIO5_FUN_DRV_S)
#define IO_MUX_GPIO5_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO5_FUN_DRV_S  10
/** IO_MUX_GPIO5_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO5_MCU_SEL    0x00000007U
#define IO_MUX_GPIO5_MCU_SEL_M  (IO_MUX_GPIO5_MCU_SEL_V << IO_MUX_GPIO5_MCU_SEL_S)
#define IO_MUX_GPIO5_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO5_MCU_SEL_S  12
/** IO_MUX_GPIO5_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO5_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO5_FILTER_EN_M  (IO_MUX_GPIO5_FILTER_EN_V << IO_MUX_GPIO5_FILTER_EN_S)
#define IO_MUX_GPIO5_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO5_FILTER_EN_S  15

/** IO_MUX_gpio6_REG register
 *  iomux control register for gpio6
 */
#define IO_MUX_GPIO6_REG (DR_REG_IO_MUX_BASE + 0x1c)
/** IO_MUX_GPIO6_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO6_MCU_OE    (BIT(0))
#define IO_MUX_GPIO6_MCU_OE_M  (IO_MUX_GPIO6_MCU_OE_V << IO_MUX_GPIO6_MCU_OE_S)
#define IO_MUX_GPIO6_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO6_MCU_OE_S  0
/** IO_MUX_GPIO6_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO6_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO6_SLP_SEL_M  (IO_MUX_GPIO6_SLP_SEL_V << IO_MUX_GPIO6_SLP_SEL_S)
#define IO_MUX_GPIO6_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO6_SLP_SEL_S  1
/** IO_MUX_GPIO6_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO6_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO6_MCU_WPD_M  (IO_MUX_GPIO6_MCU_WPD_V << IO_MUX_GPIO6_MCU_WPD_S)
#define IO_MUX_GPIO6_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO6_MCU_WPD_S  2
/** IO_MUX_GPIO6_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO6_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO6_MCU_WPU_M  (IO_MUX_GPIO6_MCU_WPU_V << IO_MUX_GPIO6_MCU_WPU_S)
#define IO_MUX_GPIO6_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO6_MCU_WPU_S  3
/** IO_MUX_GPIO6_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO6_MCU_IE    (BIT(4))
#define IO_MUX_GPIO6_MCU_IE_M  (IO_MUX_GPIO6_MCU_IE_V << IO_MUX_GPIO6_MCU_IE_S)
#define IO_MUX_GPIO6_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO6_MCU_IE_S  4
/** IO_MUX_GPIO6_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO6_MCU_DRV    0x00000003U
#define IO_MUX_GPIO6_MCU_DRV_M  (IO_MUX_GPIO6_MCU_DRV_V << IO_MUX_GPIO6_MCU_DRV_S)
#define IO_MUX_GPIO6_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO6_MCU_DRV_S  5
/** IO_MUX_GPIO6_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO6_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO6_FUN_WPD_M  (IO_MUX_GPIO6_FUN_WPD_V << IO_MUX_GPIO6_FUN_WPD_S)
#define IO_MUX_GPIO6_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO6_FUN_WPD_S  7
/** IO_MUX_GPIO6_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO6_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO6_FUN_WPU_M  (IO_MUX_GPIO6_FUN_WPU_V << IO_MUX_GPIO6_FUN_WPU_S)
#define IO_MUX_GPIO6_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO6_FUN_WPU_S  8
/** IO_MUX_GPIO6_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO6_FUN_IE    (BIT(9))
#define IO_MUX_GPIO6_FUN_IE_M  (IO_MUX_GPIO6_FUN_IE_V << IO_MUX_GPIO6_FUN_IE_S)
#define IO_MUX_GPIO6_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO6_FUN_IE_S  9
/** IO_MUX_GPIO6_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO6_FUN_DRV    0x00000003U
#define IO_MUX_GPIO6_FUN_DRV_M  (IO_MUX_GPIO6_FUN_DRV_V << IO_MUX_GPIO6_FUN_DRV_S)
#define IO_MUX_GPIO6_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO6_FUN_DRV_S  10
/** IO_MUX_GPIO6_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO6_MCU_SEL    0x00000007U
#define IO_MUX_GPIO6_MCU_SEL_M  (IO_MUX_GPIO6_MCU_SEL_V << IO_MUX_GPIO6_MCU_SEL_S)
#define IO_MUX_GPIO6_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO6_MCU_SEL_S  12
/** IO_MUX_GPIO6_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO6_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO6_FILTER_EN_M  (IO_MUX_GPIO6_FILTER_EN_V << IO_MUX_GPIO6_FILTER_EN_S)
#define IO_MUX_GPIO6_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO6_FILTER_EN_S  15

/** IO_MUX_gpio7_REG register
 *  iomux control register for gpio7
 */
#define IO_MUX_GPIO7_REG (DR_REG_IO_MUX_BASE + 0x20)
/** IO_MUX_GPIO7_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO7_MCU_OE    (BIT(0))
#define IO_MUX_GPIO7_MCU_OE_M  (IO_MUX_GPIO7_MCU_OE_V << IO_MUX_GPIO7_MCU_OE_S)
#define IO_MUX_GPIO7_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO7_MCU_OE_S  0
/** IO_MUX_GPIO7_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO7_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO7_SLP_SEL_M  (IO_MUX_GPIO7_SLP_SEL_V << IO_MUX_GPIO7_SLP_SEL_S)
#define IO_MUX_GPIO7_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO7_SLP_SEL_S  1
/** IO_MUX_GPIO7_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO7_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO7_MCU_WPD_M  (IO_MUX_GPIO7_MCU_WPD_V << IO_MUX_GPIO7_MCU_WPD_S)
#define IO_MUX_GPIO7_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO7_MCU_WPD_S  2
/** IO_MUX_GPIO7_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO7_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO7_MCU_WPU_M  (IO_MUX_GPIO7_MCU_WPU_V << IO_MUX_GPIO7_MCU_WPU_S)
#define IO_MUX_GPIO7_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO7_MCU_WPU_S  3
/** IO_MUX_GPIO7_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO7_MCU_IE    (BIT(4))
#define IO_MUX_GPIO7_MCU_IE_M  (IO_MUX_GPIO7_MCU_IE_V << IO_MUX_GPIO7_MCU_IE_S)
#define IO_MUX_GPIO7_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO7_MCU_IE_S  4
/** IO_MUX_GPIO7_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO7_MCU_DRV    0x00000003U
#define IO_MUX_GPIO7_MCU_DRV_M  (IO_MUX_GPIO7_MCU_DRV_V << IO_MUX_GPIO7_MCU_DRV_S)
#define IO_MUX_GPIO7_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO7_MCU_DRV_S  5
/** IO_MUX_GPIO7_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO7_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO7_FUN_WPD_M  (IO_MUX_GPIO7_FUN_WPD_V << IO_MUX_GPIO7_FUN_WPD_S)
#define IO_MUX_GPIO7_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO7_FUN_WPD_S  7
/** IO_MUX_GPIO7_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO7_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO7_FUN_WPU_M  (IO_MUX_GPIO7_FUN_WPU_V << IO_MUX_GPIO7_FUN_WPU_S)
#define IO_MUX_GPIO7_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO7_FUN_WPU_S  8
/** IO_MUX_GPIO7_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO7_FUN_IE    (BIT(9))
#define IO_MUX_GPIO7_FUN_IE_M  (IO_MUX_GPIO7_FUN_IE_V << IO_MUX_GPIO7_FUN_IE_S)
#define IO_MUX_GPIO7_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO7_FUN_IE_S  9
/** IO_MUX_GPIO7_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO7_FUN_DRV    0x00000003U
#define IO_MUX_GPIO7_FUN_DRV_M  (IO_MUX_GPIO7_FUN_DRV_V << IO_MUX_GPIO7_FUN_DRV_S)
#define IO_MUX_GPIO7_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO7_FUN_DRV_S  10
/** IO_MUX_GPIO7_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO7_MCU_SEL    0x00000007U
#define IO_MUX_GPIO7_MCU_SEL_M  (IO_MUX_GPIO7_MCU_SEL_V << IO_MUX_GPIO7_MCU_SEL_S)
#define IO_MUX_GPIO7_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO7_MCU_SEL_S  12
/** IO_MUX_GPIO7_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO7_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO7_FILTER_EN_M  (IO_MUX_GPIO7_FILTER_EN_V << IO_MUX_GPIO7_FILTER_EN_S)
#define IO_MUX_GPIO7_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO7_FILTER_EN_S  15

/** IO_MUX_gpio8_REG register
 *  iomux control register for gpio8
 */
#define IO_MUX_GPIO8_REG (DR_REG_IO_MUX_BASE + 0x24)
/** IO_MUX_GPIO8_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO8_MCU_OE    (BIT(0))
#define IO_MUX_GPIO8_MCU_OE_M  (IO_MUX_GPIO8_MCU_OE_V << IO_MUX_GPIO8_MCU_OE_S)
#define IO_MUX_GPIO8_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO8_MCU_OE_S  0
/** IO_MUX_GPIO8_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO8_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO8_SLP_SEL_M  (IO_MUX_GPIO8_SLP_SEL_V << IO_MUX_GPIO8_SLP_SEL_S)
#define IO_MUX_GPIO8_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO8_SLP_SEL_S  1
/** IO_MUX_GPIO8_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO8_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO8_MCU_WPD_M  (IO_MUX_GPIO8_MCU_WPD_V << IO_MUX_GPIO8_MCU_WPD_S)
#define IO_MUX_GPIO8_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO8_MCU_WPD_S  2
/** IO_MUX_GPIO8_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO8_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO8_MCU_WPU_M  (IO_MUX_GPIO8_MCU_WPU_V << IO_MUX_GPIO8_MCU_WPU_S)
#define IO_MUX_GPIO8_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO8_MCU_WPU_S  3
/** IO_MUX_GPIO8_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO8_MCU_IE    (BIT(4))
#define IO_MUX_GPIO8_MCU_IE_M  (IO_MUX_GPIO8_MCU_IE_V << IO_MUX_GPIO8_MCU_IE_S)
#define IO_MUX_GPIO8_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO8_MCU_IE_S  4
/** IO_MUX_GPIO8_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO8_MCU_DRV    0x00000003U
#define IO_MUX_GPIO8_MCU_DRV_M  (IO_MUX_GPIO8_MCU_DRV_V << IO_MUX_GPIO8_MCU_DRV_S)
#define IO_MUX_GPIO8_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO8_MCU_DRV_S  5
/** IO_MUX_GPIO8_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO8_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO8_FUN_WPD_M  (IO_MUX_GPIO8_FUN_WPD_V << IO_MUX_GPIO8_FUN_WPD_S)
#define IO_MUX_GPIO8_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO8_FUN_WPD_S  7
/** IO_MUX_GPIO8_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO8_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO8_FUN_WPU_M  (IO_MUX_GPIO8_FUN_WPU_V << IO_MUX_GPIO8_FUN_WPU_S)
#define IO_MUX_GPIO8_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO8_FUN_WPU_S  8
/** IO_MUX_GPIO8_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO8_FUN_IE    (BIT(9))
#define IO_MUX_GPIO8_FUN_IE_M  (IO_MUX_GPIO8_FUN_IE_V << IO_MUX_GPIO8_FUN_IE_S)
#define IO_MUX_GPIO8_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO8_FUN_IE_S  9
/** IO_MUX_GPIO8_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO8_FUN_DRV    0x00000003U
#define IO_MUX_GPIO8_FUN_DRV_M  (IO_MUX_GPIO8_FUN_DRV_V << IO_MUX_GPIO8_FUN_DRV_S)
#define IO_MUX_GPIO8_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO8_FUN_DRV_S  10
/** IO_MUX_GPIO8_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO8_MCU_SEL    0x00000007U
#define IO_MUX_GPIO8_MCU_SEL_M  (IO_MUX_GPIO8_MCU_SEL_V << IO_MUX_GPIO8_MCU_SEL_S)
#define IO_MUX_GPIO8_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO8_MCU_SEL_S  12
/** IO_MUX_GPIO8_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO8_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO8_FILTER_EN_M  (IO_MUX_GPIO8_FILTER_EN_V << IO_MUX_GPIO8_FILTER_EN_S)
#define IO_MUX_GPIO8_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO8_FILTER_EN_S  15

/** IO_MUX_gpio9_REG register
 *  iomux control register for gpio9
 */
#define IO_MUX_GPIO9_REG (DR_REG_IO_MUX_BASE + 0x28)
/** IO_MUX_GPIO9_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO9_MCU_OE    (BIT(0))
#define IO_MUX_GPIO9_MCU_OE_M  (IO_MUX_GPIO9_MCU_OE_V << IO_MUX_GPIO9_MCU_OE_S)
#define IO_MUX_GPIO9_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO9_MCU_OE_S  0
/** IO_MUX_GPIO9_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO9_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO9_SLP_SEL_M  (IO_MUX_GPIO9_SLP_SEL_V << IO_MUX_GPIO9_SLP_SEL_S)
#define IO_MUX_GPIO9_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO9_SLP_SEL_S  1
/** IO_MUX_GPIO9_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO9_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO9_MCU_WPD_M  (IO_MUX_GPIO9_MCU_WPD_V << IO_MUX_GPIO9_MCU_WPD_S)
#define IO_MUX_GPIO9_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO9_MCU_WPD_S  2
/** IO_MUX_GPIO9_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO9_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO9_MCU_WPU_M  (IO_MUX_GPIO9_MCU_WPU_V << IO_MUX_GPIO9_MCU_WPU_S)
#define IO_MUX_GPIO9_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO9_MCU_WPU_S  3
/** IO_MUX_GPIO9_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO9_MCU_IE    (BIT(4))
#define IO_MUX_GPIO9_MCU_IE_M  (IO_MUX_GPIO9_MCU_IE_V << IO_MUX_GPIO9_MCU_IE_S)
#define IO_MUX_GPIO9_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO9_MCU_IE_S  4
/** IO_MUX_GPIO9_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO9_MCU_DRV    0x00000003U
#define IO_MUX_GPIO9_MCU_DRV_M  (IO_MUX_GPIO9_MCU_DRV_V << IO_MUX_GPIO9_MCU_DRV_S)
#define IO_MUX_GPIO9_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO9_MCU_DRV_S  5
/** IO_MUX_GPIO9_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO9_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO9_FUN_WPD_M  (IO_MUX_GPIO9_FUN_WPD_V << IO_MUX_GPIO9_FUN_WPD_S)
#define IO_MUX_GPIO9_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO9_FUN_WPD_S  7
/** IO_MUX_GPIO9_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO9_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO9_FUN_WPU_M  (IO_MUX_GPIO9_FUN_WPU_V << IO_MUX_GPIO9_FUN_WPU_S)
#define IO_MUX_GPIO9_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO9_FUN_WPU_S  8
/** IO_MUX_GPIO9_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO9_FUN_IE    (BIT(9))
#define IO_MUX_GPIO9_FUN_IE_M  (IO_MUX_GPIO9_FUN_IE_V << IO_MUX_GPIO9_FUN_IE_S)
#define IO_MUX_GPIO9_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO9_FUN_IE_S  9
/** IO_MUX_GPIO9_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO9_FUN_DRV    0x00000003U
#define IO_MUX_GPIO9_FUN_DRV_M  (IO_MUX_GPIO9_FUN_DRV_V << IO_MUX_GPIO9_FUN_DRV_S)
#define IO_MUX_GPIO9_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO9_FUN_DRV_S  10
/** IO_MUX_GPIO9_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO9_MCU_SEL    0x00000007U
#define IO_MUX_GPIO9_MCU_SEL_M  (IO_MUX_GPIO9_MCU_SEL_V << IO_MUX_GPIO9_MCU_SEL_S)
#define IO_MUX_GPIO9_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO9_MCU_SEL_S  12
/** IO_MUX_GPIO9_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO9_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO9_FILTER_EN_M  (IO_MUX_GPIO9_FILTER_EN_V << IO_MUX_GPIO9_FILTER_EN_S)
#define IO_MUX_GPIO9_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO9_FILTER_EN_S  15

/** IO_MUX_gpio10_REG register
 *  iomux control register for gpio10
 */
#define IO_MUX_GPIO10_REG (DR_REG_IO_MUX_BASE + 0x2c)
/** IO_MUX_GPIO10_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO10_MCU_OE    (BIT(0))
#define IO_MUX_GPIO10_MCU_OE_M  (IO_MUX_GPIO10_MCU_OE_V << IO_MUX_GPIO10_MCU_OE_S)
#define IO_MUX_GPIO10_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO10_MCU_OE_S  0
/** IO_MUX_GPIO10_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO10_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO10_SLP_SEL_M  (IO_MUX_GPIO10_SLP_SEL_V << IO_MUX_GPIO10_SLP_SEL_S)
#define IO_MUX_GPIO10_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO10_SLP_SEL_S  1
/** IO_MUX_GPIO10_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO10_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO10_MCU_WPD_M  (IO_MUX_GPIO10_MCU_WPD_V << IO_MUX_GPIO10_MCU_WPD_S)
#define IO_MUX_GPIO10_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO10_MCU_WPD_S  2
/** IO_MUX_GPIO10_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO10_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO10_MCU_WPU_M  (IO_MUX_GPIO10_MCU_WPU_V << IO_MUX_GPIO10_MCU_WPU_S)
#define IO_MUX_GPIO10_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO10_MCU_WPU_S  3
/** IO_MUX_GPIO10_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO10_MCU_IE    (BIT(4))
#define IO_MUX_GPIO10_MCU_IE_M  (IO_MUX_GPIO10_MCU_IE_V << IO_MUX_GPIO10_MCU_IE_S)
#define IO_MUX_GPIO10_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO10_MCU_IE_S  4
/** IO_MUX_GPIO10_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO10_MCU_DRV    0x00000003U
#define IO_MUX_GPIO10_MCU_DRV_M  (IO_MUX_GPIO10_MCU_DRV_V << IO_MUX_GPIO10_MCU_DRV_S)
#define IO_MUX_GPIO10_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO10_MCU_DRV_S  5
/** IO_MUX_GPIO10_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO10_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO10_FUN_WPD_M  (IO_MUX_GPIO10_FUN_WPD_V << IO_MUX_GPIO10_FUN_WPD_S)
#define IO_MUX_GPIO10_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO10_FUN_WPD_S  7
/** IO_MUX_GPIO10_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO10_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO10_FUN_WPU_M  (IO_MUX_GPIO10_FUN_WPU_V << IO_MUX_GPIO10_FUN_WPU_S)
#define IO_MUX_GPIO10_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO10_FUN_WPU_S  8
/** IO_MUX_GPIO10_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO10_FUN_IE    (BIT(9))
#define IO_MUX_GPIO10_FUN_IE_M  (IO_MUX_GPIO10_FUN_IE_V << IO_MUX_GPIO10_FUN_IE_S)
#define IO_MUX_GPIO10_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO10_FUN_IE_S  9
/** IO_MUX_GPIO10_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO10_FUN_DRV    0x00000003U
#define IO_MUX_GPIO10_FUN_DRV_M  (IO_MUX_GPIO10_FUN_DRV_V << IO_MUX_GPIO10_FUN_DRV_S)
#define IO_MUX_GPIO10_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO10_FUN_DRV_S  10
/** IO_MUX_GPIO10_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO10_MCU_SEL    0x00000007U
#define IO_MUX_GPIO10_MCU_SEL_M  (IO_MUX_GPIO10_MCU_SEL_V << IO_MUX_GPIO10_MCU_SEL_S)
#define IO_MUX_GPIO10_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO10_MCU_SEL_S  12
/** IO_MUX_GPIO10_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO10_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO10_FILTER_EN_M  (IO_MUX_GPIO10_FILTER_EN_V << IO_MUX_GPIO10_FILTER_EN_S)
#define IO_MUX_GPIO10_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO10_FILTER_EN_S  15

/** IO_MUX_gpio11_REG register
 *  iomux control register for gpio11
 */
#define IO_MUX_GPIO11_REG (DR_REG_IO_MUX_BASE + 0x30)
/** IO_MUX_GPIO11_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO11_MCU_OE    (BIT(0))
#define IO_MUX_GPIO11_MCU_OE_M  (IO_MUX_GPIO11_MCU_OE_V << IO_MUX_GPIO11_MCU_OE_S)
#define IO_MUX_GPIO11_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO11_MCU_OE_S  0
/** IO_MUX_GPIO11_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO11_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO11_SLP_SEL_M  (IO_MUX_GPIO11_SLP_SEL_V << IO_MUX_GPIO11_SLP_SEL_S)
#define IO_MUX_GPIO11_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO11_SLP_SEL_S  1
/** IO_MUX_GPIO11_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO11_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO11_MCU_WPD_M  (IO_MUX_GPIO11_MCU_WPD_V << IO_MUX_GPIO11_MCU_WPD_S)
#define IO_MUX_GPIO11_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO11_MCU_WPD_S  2
/** IO_MUX_GPIO11_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO11_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO11_MCU_WPU_M  (IO_MUX_GPIO11_MCU_WPU_V << IO_MUX_GPIO11_MCU_WPU_S)
#define IO_MUX_GPIO11_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO11_MCU_WPU_S  3
/** IO_MUX_GPIO11_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO11_MCU_IE    (BIT(4))
#define IO_MUX_GPIO11_MCU_IE_M  (IO_MUX_GPIO11_MCU_IE_V << IO_MUX_GPIO11_MCU_IE_S)
#define IO_MUX_GPIO11_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO11_MCU_IE_S  4
/** IO_MUX_GPIO11_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO11_MCU_DRV    0x00000003U
#define IO_MUX_GPIO11_MCU_DRV_M  (IO_MUX_GPIO11_MCU_DRV_V << IO_MUX_GPIO11_MCU_DRV_S)
#define IO_MUX_GPIO11_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO11_MCU_DRV_S  5
/** IO_MUX_GPIO11_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO11_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO11_FUN_WPD_M  (IO_MUX_GPIO11_FUN_WPD_V << IO_MUX_GPIO11_FUN_WPD_S)
#define IO_MUX_GPIO11_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO11_FUN_WPD_S  7
/** IO_MUX_GPIO11_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO11_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO11_FUN_WPU_M  (IO_MUX_GPIO11_FUN_WPU_V << IO_MUX_GPIO11_FUN_WPU_S)
#define IO_MUX_GPIO11_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO11_FUN_WPU_S  8
/** IO_MUX_GPIO11_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO11_FUN_IE    (BIT(9))
#define IO_MUX_GPIO11_FUN_IE_M  (IO_MUX_GPIO11_FUN_IE_V << IO_MUX_GPIO11_FUN_IE_S)
#define IO_MUX_GPIO11_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO11_FUN_IE_S  9
/** IO_MUX_GPIO11_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO11_FUN_DRV    0x00000003U
#define IO_MUX_GPIO11_FUN_DRV_M  (IO_MUX_GPIO11_FUN_DRV_V << IO_MUX_GPIO11_FUN_DRV_S)
#define IO_MUX_GPIO11_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO11_FUN_DRV_S  10
/** IO_MUX_GPIO11_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO11_MCU_SEL    0x00000007U
#define IO_MUX_GPIO11_MCU_SEL_M  (IO_MUX_GPIO11_MCU_SEL_V << IO_MUX_GPIO11_MCU_SEL_S)
#define IO_MUX_GPIO11_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO11_MCU_SEL_S  12
/** IO_MUX_GPIO11_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO11_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO11_FILTER_EN_M  (IO_MUX_GPIO11_FILTER_EN_V << IO_MUX_GPIO11_FILTER_EN_S)
#define IO_MUX_GPIO11_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO11_FILTER_EN_S  15

/** IO_MUX_gpio12_REG register
 *  iomux control register for gpio12
 */
#define IO_MUX_GPIO12_REG (DR_REG_IO_MUX_BASE + 0x34)
/** IO_MUX_GPIO12_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO12_MCU_OE    (BIT(0))
#define IO_MUX_GPIO12_MCU_OE_M  (IO_MUX_GPIO12_MCU_OE_V << IO_MUX_GPIO12_MCU_OE_S)
#define IO_MUX_GPIO12_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO12_MCU_OE_S  0
/** IO_MUX_GPIO12_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO12_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO12_SLP_SEL_M  (IO_MUX_GPIO12_SLP_SEL_V << IO_MUX_GPIO12_SLP_SEL_S)
#define IO_MUX_GPIO12_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO12_SLP_SEL_S  1
/** IO_MUX_GPIO12_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO12_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO12_MCU_WPD_M  (IO_MUX_GPIO12_MCU_WPD_V << IO_MUX_GPIO12_MCU_WPD_S)
#define IO_MUX_GPIO12_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO12_MCU_WPD_S  2
/** IO_MUX_GPIO12_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO12_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO12_MCU_WPU_M  (IO_MUX_GPIO12_MCU_WPU_V << IO_MUX_GPIO12_MCU_WPU_S)
#define IO_MUX_GPIO12_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO12_MCU_WPU_S  3
/** IO_MUX_GPIO12_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO12_MCU_IE    (BIT(4))
#define IO_MUX_GPIO12_MCU_IE_M  (IO_MUX_GPIO12_MCU_IE_V << IO_MUX_GPIO12_MCU_IE_S)
#define IO_MUX_GPIO12_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO12_MCU_IE_S  4
/** IO_MUX_GPIO12_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO12_MCU_DRV    0x00000003U
#define IO_MUX_GPIO12_MCU_DRV_M  (IO_MUX_GPIO12_MCU_DRV_V << IO_MUX_GPIO12_MCU_DRV_S)
#define IO_MUX_GPIO12_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO12_MCU_DRV_S  5
/** IO_MUX_GPIO12_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO12_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO12_FUN_WPD_M  (IO_MUX_GPIO12_FUN_WPD_V << IO_MUX_GPIO12_FUN_WPD_S)
#define IO_MUX_GPIO12_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO12_FUN_WPD_S  7
/** IO_MUX_GPIO12_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO12_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO12_FUN_WPU_M  (IO_MUX_GPIO12_FUN_WPU_V << IO_MUX_GPIO12_FUN_WPU_S)
#define IO_MUX_GPIO12_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO12_FUN_WPU_S  8
/** IO_MUX_GPIO12_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO12_FUN_IE    (BIT(9))
#define IO_MUX_GPIO12_FUN_IE_M  (IO_MUX_GPIO12_FUN_IE_V << IO_MUX_GPIO12_FUN_IE_S)
#define IO_MUX_GPIO12_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO12_FUN_IE_S  9
/** IO_MUX_GPIO12_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO12_FUN_DRV    0x00000003U
#define IO_MUX_GPIO12_FUN_DRV_M  (IO_MUX_GPIO12_FUN_DRV_V << IO_MUX_GPIO12_FUN_DRV_S)
#define IO_MUX_GPIO12_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO12_FUN_DRV_S  10
/** IO_MUX_GPIO12_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO12_MCU_SEL    0x00000007U
#define IO_MUX_GPIO12_MCU_SEL_M  (IO_MUX_GPIO12_MCU_SEL_V << IO_MUX_GPIO12_MCU_SEL_S)
#define IO_MUX_GPIO12_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO12_MCU_SEL_S  12
/** IO_MUX_GPIO12_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO12_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO12_FILTER_EN_M  (IO_MUX_GPIO12_FILTER_EN_V << IO_MUX_GPIO12_FILTER_EN_S)
#define IO_MUX_GPIO12_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO12_FILTER_EN_S  15

/** IO_MUX_gpio13_REG register
 *  iomux control register for gpio13
 */
#define IO_MUX_GPIO13_REG (DR_REG_IO_MUX_BASE + 0x38)
/** IO_MUX_GPIO13_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO13_MCU_OE    (BIT(0))
#define IO_MUX_GPIO13_MCU_OE_M  (IO_MUX_GPIO13_MCU_OE_V << IO_MUX_GPIO13_MCU_OE_S)
#define IO_MUX_GPIO13_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO13_MCU_OE_S  0
/** IO_MUX_GPIO13_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO13_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO13_SLP_SEL_M  (IO_MUX_GPIO13_SLP_SEL_V << IO_MUX_GPIO13_SLP_SEL_S)
#define IO_MUX_GPIO13_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO13_SLP_SEL_S  1
/** IO_MUX_GPIO13_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO13_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO13_MCU_WPD_M  (IO_MUX_GPIO13_MCU_WPD_V << IO_MUX_GPIO13_MCU_WPD_S)
#define IO_MUX_GPIO13_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO13_MCU_WPD_S  2
/** IO_MUX_GPIO13_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO13_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO13_MCU_WPU_M  (IO_MUX_GPIO13_MCU_WPU_V << IO_MUX_GPIO13_MCU_WPU_S)
#define IO_MUX_GPIO13_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO13_MCU_WPU_S  3
/** IO_MUX_GPIO13_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO13_MCU_IE    (BIT(4))
#define IO_MUX_GPIO13_MCU_IE_M  (IO_MUX_GPIO13_MCU_IE_V << IO_MUX_GPIO13_MCU_IE_S)
#define IO_MUX_GPIO13_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO13_MCU_IE_S  4
/** IO_MUX_GPIO13_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO13_MCU_DRV    0x00000003U
#define IO_MUX_GPIO13_MCU_DRV_M  (IO_MUX_GPIO13_MCU_DRV_V << IO_MUX_GPIO13_MCU_DRV_S)
#define IO_MUX_GPIO13_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO13_MCU_DRV_S  5
/** IO_MUX_GPIO13_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO13_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO13_FUN_WPD_M  (IO_MUX_GPIO13_FUN_WPD_V << IO_MUX_GPIO13_FUN_WPD_S)
#define IO_MUX_GPIO13_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO13_FUN_WPD_S  7
/** IO_MUX_GPIO13_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO13_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO13_FUN_WPU_M  (IO_MUX_GPIO13_FUN_WPU_V << IO_MUX_GPIO13_FUN_WPU_S)
#define IO_MUX_GPIO13_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO13_FUN_WPU_S  8
/** IO_MUX_GPIO13_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO13_FUN_IE    (BIT(9))
#define IO_MUX_GPIO13_FUN_IE_M  (IO_MUX_GPIO13_FUN_IE_V << IO_MUX_GPIO13_FUN_IE_S)
#define IO_MUX_GPIO13_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO13_FUN_IE_S  9
/** IO_MUX_GPIO13_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO13_FUN_DRV    0x00000003U
#define IO_MUX_GPIO13_FUN_DRV_M  (IO_MUX_GPIO13_FUN_DRV_V << IO_MUX_GPIO13_FUN_DRV_S)
#define IO_MUX_GPIO13_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO13_FUN_DRV_S  10
/** IO_MUX_GPIO13_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO13_MCU_SEL    0x00000007U
#define IO_MUX_GPIO13_MCU_SEL_M  (IO_MUX_GPIO13_MCU_SEL_V << IO_MUX_GPIO13_MCU_SEL_S)
#define IO_MUX_GPIO13_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO13_MCU_SEL_S  12
/** IO_MUX_GPIO13_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO13_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO13_FILTER_EN_M  (IO_MUX_GPIO13_FILTER_EN_V << IO_MUX_GPIO13_FILTER_EN_S)
#define IO_MUX_GPIO13_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO13_FILTER_EN_S  15

/** IO_MUX_gpio14_REG register
 *  iomux control register for gpio14
 */
#define IO_MUX_GPIO14_REG (DR_REG_IO_MUX_BASE + 0x3c)
/** IO_MUX_GPIO14_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO14_MCU_OE    (BIT(0))
#define IO_MUX_GPIO14_MCU_OE_M  (IO_MUX_GPIO14_MCU_OE_V << IO_MUX_GPIO14_MCU_OE_S)
#define IO_MUX_GPIO14_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO14_MCU_OE_S  0
/** IO_MUX_GPIO14_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO14_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO14_SLP_SEL_M  (IO_MUX_GPIO14_SLP_SEL_V << IO_MUX_GPIO14_SLP_SEL_S)
#define IO_MUX_GPIO14_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO14_SLP_SEL_S  1
/** IO_MUX_GPIO14_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO14_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO14_MCU_WPD_M  (IO_MUX_GPIO14_MCU_WPD_V << IO_MUX_GPIO14_MCU_WPD_S)
#define IO_MUX_GPIO14_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO14_MCU_WPD_S  2
/** IO_MUX_GPIO14_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO14_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO14_MCU_WPU_M  (IO_MUX_GPIO14_MCU_WPU_V << IO_MUX_GPIO14_MCU_WPU_S)
#define IO_MUX_GPIO14_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO14_MCU_WPU_S  3
/** IO_MUX_GPIO14_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO14_MCU_IE    (BIT(4))
#define IO_MUX_GPIO14_MCU_IE_M  (IO_MUX_GPIO14_MCU_IE_V << IO_MUX_GPIO14_MCU_IE_S)
#define IO_MUX_GPIO14_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO14_MCU_IE_S  4
/** IO_MUX_GPIO14_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO14_MCU_DRV    0x00000003U
#define IO_MUX_GPIO14_MCU_DRV_M  (IO_MUX_GPIO14_MCU_DRV_V << IO_MUX_GPIO14_MCU_DRV_S)
#define IO_MUX_GPIO14_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO14_MCU_DRV_S  5
/** IO_MUX_GPIO14_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO14_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO14_FUN_WPD_M  (IO_MUX_GPIO14_FUN_WPD_V << IO_MUX_GPIO14_FUN_WPD_S)
#define IO_MUX_GPIO14_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO14_FUN_WPD_S  7
/** IO_MUX_GPIO14_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO14_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO14_FUN_WPU_M  (IO_MUX_GPIO14_FUN_WPU_V << IO_MUX_GPIO14_FUN_WPU_S)
#define IO_MUX_GPIO14_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO14_FUN_WPU_S  8
/** IO_MUX_GPIO14_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO14_FUN_IE    (BIT(9))
#define IO_MUX_GPIO14_FUN_IE_M  (IO_MUX_GPIO14_FUN_IE_V << IO_MUX_GPIO14_FUN_IE_S)
#define IO_MUX_GPIO14_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO14_FUN_IE_S  9
/** IO_MUX_GPIO14_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO14_FUN_DRV    0x00000003U
#define IO_MUX_GPIO14_FUN_DRV_M  (IO_MUX_GPIO14_FUN_DRV_V << IO_MUX_GPIO14_FUN_DRV_S)
#define IO_MUX_GPIO14_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO14_FUN_DRV_S  10
/** IO_MUX_GPIO14_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO14_MCU_SEL    0x00000007U
#define IO_MUX_GPIO14_MCU_SEL_M  (IO_MUX_GPIO14_MCU_SEL_V << IO_MUX_GPIO14_MCU_SEL_S)
#define IO_MUX_GPIO14_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO14_MCU_SEL_S  12
/** IO_MUX_GPIO14_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO14_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO14_FILTER_EN_M  (IO_MUX_GPIO14_FILTER_EN_V << IO_MUX_GPIO14_FILTER_EN_S)
#define IO_MUX_GPIO14_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO14_FILTER_EN_S  15

/** IO_MUX_gpio15_REG register
 *  iomux control register for gpio15
 */
#define IO_MUX_GPIO15_REG (DR_REG_IO_MUX_BASE + 0x40)
/** IO_MUX_GPIO15_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO15_MCU_OE    (BIT(0))
#define IO_MUX_GPIO15_MCU_OE_M  (IO_MUX_GPIO15_MCU_OE_V << IO_MUX_GPIO15_MCU_OE_S)
#define IO_MUX_GPIO15_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO15_MCU_OE_S  0
/** IO_MUX_GPIO15_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO15_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO15_SLP_SEL_M  (IO_MUX_GPIO15_SLP_SEL_V << IO_MUX_GPIO15_SLP_SEL_S)
#define IO_MUX_GPIO15_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO15_SLP_SEL_S  1
/** IO_MUX_GPIO15_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO15_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO15_MCU_WPD_M  (IO_MUX_GPIO15_MCU_WPD_V << IO_MUX_GPIO15_MCU_WPD_S)
#define IO_MUX_GPIO15_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO15_MCU_WPD_S  2
/** IO_MUX_GPIO15_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO15_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO15_MCU_WPU_M  (IO_MUX_GPIO15_MCU_WPU_V << IO_MUX_GPIO15_MCU_WPU_S)
#define IO_MUX_GPIO15_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO15_MCU_WPU_S  3
/** IO_MUX_GPIO15_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO15_MCU_IE    (BIT(4))
#define IO_MUX_GPIO15_MCU_IE_M  (IO_MUX_GPIO15_MCU_IE_V << IO_MUX_GPIO15_MCU_IE_S)
#define IO_MUX_GPIO15_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO15_MCU_IE_S  4
/** IO_MUX_GPIO15_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO15_MCU_DRV    0x00000003U
#define IO_MUX_GPIO15_MCU_DRV_M  (IO_MUX_GPIO15_MCU_DRV_V << IO_MUX_GPIO15_MCU_DRV_S)
#define IO_MUX_GPIO15_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO15_MCU_DRV_S  5
/** IO_MUX_GPIO15_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO15_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO15_FUN_WPD_M  (IO_MUX_GPIO15_FUN_WPD_V << IO_MUX_GPIO15_FUN_WPD_S)
#define IO_MUX_GPIO15_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO15_FUN_WPD_S  7
/** IO_MUX_GPIO15_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO15_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO15_FUN_WPU_M  (IO_MUX_GPIO15_FUN_WPU_V << IO_MUX_GPIO15_FUN_WPU_S)
#define IO_MUX_GPIO15_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO15_FUN_WPU_S  8
/** IO_MUX_GPIO15_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO15_FUN_IE    (BIT(9))
#define IO_MUX_GPIO15_FUN_IE_M  (IO_MUX_GPIO15_FUN_IE_V << IO_MUX_GPIO15_FUN_IE_S)
#define IO_MUX_GPIO15_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO15_FUN_IE_S  9
/** IO_MUX_GPIO15_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO15_FUN_DRV    0x00000003U
#define IO_MUX_GPIO15_FUN_DRV_M  (IO_MUX_GPIO15_FUN_DRV_V << IO_MUX_GPIO15_FUN_DRV_S)
#define IO_MUX_GPIO15_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO15_FUN_DRV_S  10
/** IO_MUX_GPIO15_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO15_MCU_SEL    0x00000007U
#define IO_MUX_GPIO15_MCU_SEL_M  (IO_MUX_GPIO15_MCU_SEL_V << IO_MUX_GPIO15_MCU_SEL_S)
#define IO_MUX_GPIO15_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO15_MCU_SEL_S  12
/** IO_MUX_GPIO15_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO15_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO15_FILTER_EN_M  (IO_MUX_GPIO15_FILTER_EN_V << IO_MUX_GPIO15_FILTER_EN_S)
#define IO_MUX_GPIO15_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO15_FILTER_EN_S  15

/** IO_MUX_gpio16_REG register
 *  iomux control register for gpio16
 */
#define IO_MUX_GPIO16_REG (DR_REG_IO_MUX_BASE + 0x44)
/** IO_MUX_GPIO16_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO16_MCU_OE    (BIT(0))
#define IO_MUX_GPIO16_MCU_OE_M  (IO_MUX_GPIO16_MCU_OE_V << IO_MUX_GPIO16_MCU_OE_S)
#define IO_MUX_GPIO16_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO16_MCU_OE_S  0
/** IO_MUX_GPIO16_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO16_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO16_SLP_SEL_M  (IO_MUX_GPIO16_SLP_SEL_V << IO_MUX_GPIO16_SLP_SEL_S)
#define IO_MUX_GPIO16_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO16_SLP_SEL_S  1
/** IO_MUX_GPIO16_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO16_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO16_MCU_WPD_M  (IO_MUX_GPIO16_MCU_WPD_V << IO_MUX_GPIO16_MCU_WPD_S)
#define IO_MUX_GPIO16_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO16_MCU_WPD_S  2
/** IO_MUX_GPIO16_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO16_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO16_MCU_WPU_M  (IO_MUX_GPIO16_MCU_WPU_V << IO_MUX_GPIO16_MCU_WPU_S)
#define IO_MUX_GPIO16_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO16_MCU_WPU_S  3
/** IO_MUX_GPIO16_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO16_MCU_IE    (BIT(4))
#define IO_MUX_GPIO16_MCU_IE_M  (IO_MUX_GPIO16_MCU_IE_V << IO_MUX_GPIO16_MCU_IE_S)
#define IO_MUX_GPIO16_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO16_MCU_IE_S  4
/** IO_MUX_GPIO16_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO16_MCU_DRV    0x00000003U
#define IO_MUX_GPIO16_MCU_DRV_M  (IO_MUX_GPIO16_MCU_DRV_V << IO_MUX_GPIO16_MCU_DRV_S)
#define IO_MUX_GPIO16_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO16_MCU_DRV_S  5
/** IO_MUX_GPIO16_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO16_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO16_FUN_WPD_M  (IO_MUX_GPIO16_FUN_WPD_V << IO_MUX_GPIO16_FUN_WPD_S)
#define IO_MUX_GPIO16_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO16_FUN_WPD_S  7
/** IO_MUX_GPIO16_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO16_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO16_FUN_WPU_M  (IO_MUX_GPIO16_FUN_WPU_V << IO_MUX_GPIO16_FUN_WPU_S)
#define IO_MUX_GPIO16_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO16_FUN_WPU_S  8
/** IO_MUX_GPIO16_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO16_FUN_IE    (BIT(9))
#define IO_MUX_GPIO16_FUN_IE_M  (IO_MUX_GPIO16_FUN_IE_V << IO_MUX_GPIO16_FUN_IE_S)
#define IO_MUX_GPIO16_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO16_FUN_IE_S  9
/** IO_MUX_GPIO16_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO16_FUN_DRV    0x00000003U
#define IO_MUX_GPIO16_FUN_DRV_M  (IO_MUX_GPIO16_FUN_DRV_V << IO_MUX_GPIO16_FUN_DRV_S)
#define IO_MUX_GPIO16_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO16_FUN_DRV_S  10
/** IO_MUX_GPIO16_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO16_MCU_SEL    0x00000007U
#define IO_MUX_GPIO16_MCU_SEL_M  (IO_MUX_GPIO16_MCU_SEL_V << IO_MUX_GPIO16_MCU_SEL_S)
#define IO_MUX_GPIO16_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO16_MCU_SEL_S  12
/** IO_MUX_GPIO16_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO16_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO16_FILTER_EN_M  (IO_MUX_GPIO16_FILTER_EN_V << IO_MUX_GPIO16_FILTER_EN_S)
#define IO_MUX_GPIO16_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO16_FILTER_EN_S  15

/** IO_MUX_gpio17_REG register
 *  iomux control register for gpio17
 */
#define IO_MUX_GPIO17_REG (DR_REG_IO_MUX_BASE + 0x48)
/** IO_MUX_GPIO17_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO17_MCU_OE    (BIT(0))
#define IO_MUX_GPIO17_MCU_OE_M  (IO_MUX_GPIO17_MCU_OE_V << IO_MUX_GPIO17_MCU_OE_S)
#define IO_MUX_GPIO17_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO17_MCU_OE_S  0
/** IO_MUX_GPIO17_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO17_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO17_SLP_SEL_M  (IO_MUX_GPIO17_SLP_SEL_V << IO_MUX_GPIO17_SLP_SEL_S)
#define IO_MUX_GPIO17_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO17_SLP_SEL_S  1
/** IO_MUX_GPIO17_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO17_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO17_MCU_WPD_M  (IO_MUX_GPIO17_MCU_WPD_V << IO_MUX_GPIO17_MCU_WPD_S)
#define IO_MUX_GPIO17_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO17_MCU_WPD_S  2
/** IO_MUX_GPIO17_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO17_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO17_MCU_WPU_M  (IO_MUX_GPIO17_MCU_WPU_V << IO_MUX_GPIO17_MCU_WPU_S)
#define IO_MUX_GPIO17_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO17_MCU_WPU_S  3
/** IO_MUX_GPIO17_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO17_MCU_IE    (BIT(4))
#define IO_MUX_GPIO17_MCU_IE_M  (IO_MUX_GPIO17_MCU_IE_V << IO_MUX_GPIO17_MCU_IE_S)
#define IO_MUX_GPIO17_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO17_MCU_IE_S  4
/** IO_MUX_GPIO17_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO17_MCU_DRV    0x00000003U
#define IO_MUX_GPIO17_MCU_DRV_M  (IO_MUX_GPIO17_MCU_DRV_V << IO_MUX_GPIO17_MCU_DRV_S)
#define IO_MUX_GPIO17_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO17_MCU_DRV_S  5
/** IO_MUX_GPIO17_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO17_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO17_FUN_WPD_M  (IO_MUX_GPIO17_FUN_WPD_V << IO_MUX_GPIO17_FUN_WPD_S)
#define IO_MUX_GPIO17_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO17_FUN_WPD_S  7
/** IO_MUX_GPIO17_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO17_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO17_FUN_WPU_M  (IO_MUX_GPIO17_FUN_WPU_V << IO_MUX_GPIO17_FUN_WPU_S)
#define IO_MUX_GPIO17_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO17_FUN_WPU_S  8
/** IO_MUX_GPIO17_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO17_FUN_IE    (BIT(9))
#define IO_MUX_GPIO17_FUN_IE_M  (IO_MUX_GPIO17_FUN_IE_V << IO_MUX_GPIO17_FUN_IE_S)
#define IO_MUX_GPIO17_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO17_FUN_IE_S  9
/** IO_MUX_GPIO17_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO17_FUN_DRV    0x00000003U
#define IO_MUX_GPIO17_FUN_DRV_M  (IO_MUX_GPIO17_FUN_DRV_V << IO_MUX_GPIO17_FUN_DRV_S)
#define IO_MUX_GPIO17_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO17_FUN_DRV_S  10
/** IO_MUX_GPIO17_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO17_MCU_SEL    0x00000007U
#define IO_MUX_GPIO17_MCU_SEL_M  (IO_MUX_GPIO17_MCU_SEL_V << IO_MUX_GPIO17_MCU_SEL_S)
#define IO_MUX_GPIO17_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO17_MCU_SEL_S  12
/** IO_MUX_GPIO17_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO17_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO17_FILTER_EN_M  (IO_MUX_GPIO17_FILTER_EN_V << IO_MUX_GPIO17_FILTER_EN_S)
#define IO_MUX_GPIO17_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO17_FILTER_EN_S  15

/** IO_MUX_gpio18_REG register
 *  iomux control register for gpio18
 */
#define IO_MUX_GPIO18_REG (DR_REG_IO_MUX_BASE + 0x4c)
/** IO_MUX_GPIO18_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO18_MCU_OE    (BIT(0))
#define IO_MUX_GPIO18_MCU_OE_M  (IO_MUX_GPIO18_MCU_OE_V << IO_MUX_GPIO18_MCU_OE_S)
#define IO_MUX_GPIO18_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO18_MCU_OE_S  0
/** IO_MUX_GPIO18_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO18_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO18_SLP_SEL_M  (IO_MUX_GPIO18_SLP_SEL_V << IO_MUX_GPIO18_SLP_SEL_S)
#define IO_MUX_GPIO18_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO18_SLP_SEL_S  1
/** IO_MUX_GPIO18_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO18_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO18_MCU_WPD_M  (IO_MUX_GPIO18_MCU_WPD_V << IO_MUX_GPIO18_MCU_WPD_S)
#define IO_MUX_GPIO18_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO18_MCU_WPD_S  2
/** IO_MUX_GPIO18_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO18_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO18_MCU_WPU_M  (IO_MUX_GPIO18_MCU_WPU_V << IO_MUX_GPIO18_MCU_WPU_S)
#define IO_MUX_GPIO18_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO18_MCU_WPU_S  3
/** IO_MUX_GPIO18_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO18_MCU_IE    (BIT(4))
#define IO_MUX_GPIO18_MCU_IE_M  (IO_MUX_GPIO18_MCU_IE_V << IO_MUX_GPIO18_MCU_IE_S)
#define IO_MUX_GPIO18_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO18_MCU_IE_S  4
/** IO_MUX_GPIO18_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO18_MCU_DRV    0x00000003U
#define IO_MUX_GPIO18_MCU_DRV_M  (IO_MUX_GPIO18_MCU_DRV_V << IO_MUX_GPIO18_MCU_DRV_S)
#define IO_MUX_GPIO18_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO18_MCU_DRV_S  5
/** IO_MUX_GPIO18_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO18_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO18_FUN_WPD_M  (IO_MUX_GPIO18_FUN_WPD_V << IO_MUX_GPIO18_FUN_WPD_S)
#define IO_MUX_GPIO18_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO18_FUN_WPD_S  7
/** IO_MUX_GPIO18_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO18_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO18_FUN_WPU_M  (IO_MUX_GPIO18_FUN_WPU_V << IO_MUX_GPIO18_FUN_WPU_S)
#define IO_MUX_GPIO18_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO18_FUN_WPU_S  8
/** IO_MUX_GPIO18_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO18_FUN_IE    (BIT(9))
#define IO_MUX_GPIO18_FUN_IE_M  (IO_MUX_GPIO18_FUN_IE_V << IO_MUX_GPIO18_FUN_IE_S)
#define IO_MUX_GPIO18_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO18_FUN_IE_S  9
/** IO_MUX_GPIO18_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO18_FUN_DRV    0x00000003U
#define IO_MUX_GPIO18_FUN_DRV_M  (IO_MUX_GPIO18_FUN_DRV_V << IO_MUX_GPIO18_FUN_DRV_S)
#define IO_MUX_GPIO18_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO18_FUN_DRV_S  10
/** IO_MUX_GPIO18_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO18_MCU_SEL    0x00000007U
#define IO_MUX_GPIO18_MCU_SEL_M  (IO_MUX_GPIO18_MCU_SEL_V << IO_MUX_GPIO18_MCU_SEL_S)
#define IO_MUX_GPIO18_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO18_MCU_SEL_S  12
/** IO_MUX_GPIO18_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO18_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO18_FILTER_EN_M  (IO_MUX_GPIO18_FILTER_EN_V << IO_MUX_GPIO18_FILTER_EN_S)
#define IO_MUX_GPIO18_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO18_FILTER_EN_S  15

/** IO_MUX_gpio19_REG register
 *  iomux control register for gpio19
 */
#define IO_MUX_GPIO19_REG (DR_REG_IO_MUX_BASE + 0x50)
/** IO_MUX_GPIO19_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO19_MCU_OE    (BIT(0))
#define IO_MUX_GPIO19_MCU_OE_M  (IO_MUX_GPIO19_MCU_OE_V << IO_MUX_GPIO19_MCU_OE_S)
#define IO_MUX_GPIO19_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO19_MCU_OE_S  0
/** IO_MUX_GPIO19_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO19_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO19_SLP_SEL_M  (IO_MUX_GPIO19_SLP_SEL_V << IO_MUX_GPIO19_SLP_SEL_S)
#define IO_MUX_GPIO19_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO19_SLP_SEL_S  1
/** IO_MUX_GPIO19_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO19_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO19_MCU_WPD_M  (IO_MUX_GPIO19_MCU_WPD_V << IO_MUX_GPIO19_MCU_WPD_S)
#define IO_MUX_GPIO19_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO19_MCU_WPD_S  2
/** IO_MUX_GPIO19_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO19_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO19_MCU_WPU_M  (IO_MUX_GPIO19_MCU_WPU_V << IO_MUX_GPIO19_MCU_WPU_S)
#define IO_MUX_GPIO19_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO19_MCU_WPU_S  3
/** IO_MUX_GPIO19_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO19_MCU_IE    (BIT(4))
#define IO_MUX_GPIO19_MCU_IE_M  (IO_MUX_GPIO19_MCU_IE_V << IO_MUX_GPIO19_MCU_IE_S)
#define IO_MUX_GPIO19_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO19_MCU_IE_S  4
/** IO_MUX_GPIO19_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO19_MCU_DRV    0x00000003U
#define IO_MUX_GPIO19_MCU_DRV_M  (IO_MUX_GPIO19_MCU_DRV_V << IO_MUX_GPIO19_MCU_DRV_S)
#define IO_MUX_GPIO19_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO19_MCU_DRV_S  5
/** IO_MUX_GPIO19_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO19_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO19_FUN_WPD_M  (IO_MUX_GPIO19_FUN_WPD_V << IO_MUX_GPIO19_FUN_WPD_S)
#define IO_MUX_GPIO19_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO19_FUN_WPD_S  7
/** IO_MUX_GPIO19_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO19_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO19_FUN_WPU_M  (IO_MUX_GPIO19_FUN_WPU_V << IO_MUX_GPIO19_FUN_WPU_S)
#define IO_MUX_GPIO19_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO19_FUN_WPU_S  8
/** IO_MUX_GPIO19_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO19_FUN_IE    (BIT(9))
#define IO_MUX_GPIO19_FUN_IE_M  (IO_MUX_GPIO19_FUN_IE_V << IO_MUX_GPIO19_FUN_IE_S)
#define IO_MUX_GPIO19_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO19_FUN_IE_S  9
/** IO_MUX_GPIO19_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO19_FUN_DRV    0x00000003U
#define IO_MUX_GPIO19_FUN_DRV_M  (IO_MUX_GPIO19_FUN_DRV_V << IO_MUX_GPIO19_FUN_DRV_S)
#define IO_MUX_GPIO19_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO19_FUN_DRV_S  10
/** IO_MUX_GPIO19_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO19_MCU_SEL    0x00000007U
#define IO_MUX_GPIO19_MCU_SEL_M  (IO_MUX_GPIO19_MCU_SEL_V << IO_MUX_GPIO19_MCU_SEL_S)
#define IO_MUX_GPIO19_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO19_MCU_SEL_S  12
/** IO_MUX_GPIO19_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO19_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO19_FILTER_EN_M  (IO_MUX_GPIO19_FILTER_EN_V << IO_MUX_GPIO19_FILTER_EN_S)
#define IO_MUX_GPIO19_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO19_FILTER_EN_S  15

/** IO_MUX_gpio20_REG register
 *  iomux control register for gpio20
 */
#define IO_MUX_GPIO20_REG (DR_REG_IO_MUX_BASE + 0x54)
/** IO_MUX_GPIO20_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO20_MCU_OE    (BIT(0))
#define IO_MUX_GPIO20_MCU_OE_M  (IO_MUX_GPIO20_MCU_OE_V << IO_MUX_GPIO20_MCU_OE_S)
#define IO_MUX_GPIO20_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO20_MCU_OE_S  0
/** IO_MUX_GPIO20_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO20_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO20_SLP_SEL_M  (IO_MUX_GPIO20_SLP_SEL_V << IO_MUX_GPIO20_SLP_SEL_S)
#define IO_MUX_GPIO20_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO20_SLP_SEL_S  1
/** IO_MUX_GPIO20_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO20_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO20_MCU_WPD_M  (IO_MUX_GPIO20_MCU_WPD_V << IO_MUX_GPIO20_MCU_WPD_S)
#define IO_MUX_GPIO20_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO20_MCU_WPD_S  2
/** IO_MUX_GPIO20_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO20_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO20_MCU_WPU_M  (IO_MUX_GPIO20_MCU_WPU_V << IO_MUX_GPIO20_MCU_WPU_S)
#define IO_MUX_GPIO20_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO20_MCU_WPU_S  3
/** IO_MUX_GPIO20_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO20_MCU_IE    (BIT(4))
#define IO_MUX_GPIO20_MCU_IE_M  (IO_MUX_GPIO20_MCU_IE_V << IO_MUX_GPIO20_MCU_IE_S)
#define IO_MUX_GPIO20_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO20_MCU_IE_S  4
/** IO_MUX_GPIO20_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO20_MCU_DRV    0x00000003U
#define IO_MUX_GPIO20_MCU_DRV_M  (IO_MUX_GPIO20_MCU_DRV_V << IO_MUX_GPIO20_MCU_DRV_S)
#define IO_MUX_GPIO20_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO20_MCU_DRV_S  5
/** IO_MUX_GPIO20_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO20_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO20_FUN_WPD_M  (IO_MUX_GPIO20_FUN_WPD_V << IO_MUX_GPIO20_FUN_WPD_S)
#define IO_MUX_GPIO20_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO20_FUN_WPD_S  7
/** IO_MUX_GPIO20_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO20_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO20_FUN_WPU_M  (IO_MUX_GPIO20_FUN_WPU_V << IO_MUX_GPIO20_FUN_WPU_S)
#define IO_MUX_GPIO20_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO20_FUN_WPU_S  8
/** IO_MUX_GPIO20_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO20_FUN_IE    (BIT(9))
#define IO_MUX_GPIO20_FUN_IE_M  (IO_MUX_GPIO20_FUN_IE_V << IO_MUX_GPIO20_FUN_IE_S)
#define IO_MUX_GPIO20_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO20_FUN_IE_S  9
/** IO_MUX_GPIO20_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO20_FUN_DRV    0x00000003U
#define IO_MUX_GPIO20_FUN_DRV_M  (IO_MUX_GPIO20_FUN_DRV_V << IO_MUX_GPIO20_FUN_DRV_S)
#define IO_MUX_GPIO20_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO20_FUN_DRV_S  10
/** IO_MUX_GPIO20_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO20_MCU_SEL    0x00000007U
#define IO_MUX_GPIO20_MCU_SEL_M  (IO_MUX_GPIO20_MCU_SEL_V << IO_MUX_GPIO20_MCU_SEL_S)
#define IO_MUX_GPIO20_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO20_MCU_SEL_S  12
/** IO_MUX_GPIO20_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO20_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO20_FILTER_EN_M  (IO_MUX_GPIO20_FILTER_EN_V << IO_MUX_GPIO20_FILTER_EN_S)
#define IO_MUX_GPIO20_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO20_FILTER_EN_S  15

/** IO_MUX_gpio21_REG register
 *  iomux control register for gpio21
 */
#define IO_MUX_GPIO21_REG (DR_REG_IO_MUX_BASE + 0x58)
/** IO_MUX_GPIO21_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO21_MCU_OE    (BIT(0))
#define IO_MUX_GPIO21_MCU_OE_M  (IO_MUX_GPIO21_MCU_OE_V << IO_MUX_GPIO21_MCU_OE_S)
#define IO_MUX_GPIO21_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO21_MCU_OE_S  0
/** IO_MUX_GPIO21_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO21_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO21_SLP_SEL_M  (IO_MUX_GPIO21_SLP_SEL_V << IO_MUX_GPIO21_SLP_SEL_S)
#define IO_MUX_GPIO21_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO21_SLP_SEL_S  1
/** IO_MUX_GPIO21_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO21_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO21_MCU_WPD_M  (IO_MUX_GPIO21_MCU_WPD_V << IO_MUX_GPIO21_MCU_WPD_S)
#define IO_MUX_GPIO21_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO21_MCU_WPD_S  2
/** IO_MUX_GPIO21_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO21_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO21_MCU_WPU_M  (IO_MUX_GPIO21_MCU_WPU_V << IO_MUX_GPIO21_MCU_WPU_S)
#define IO_MUX_GPIO21_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO21_MCU_WPU_S  3
/** IO_MUX_GPIO21_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO21_MCU_IE    (BIT(4))
#define IO_MUX_GPIO21_MCU_IE_M  (IO_MUX_GPIO21_MCU_IE_V << IO_MUX_GPIO21_MCU_IE_S)
#define IO_MUX_GPIO21_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO21_MCU_IE_S  4
/** IO_MUX_GPIO21_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO21_MCU_DRV    0x00000003U
#define IO_MUX_GPIO21_MCU_DRV_M  (IO_MUX_GPIO21_MCU_DRV_V << IO_MUX_GPIO21_MCU_DRV_S)
#define IO_MUX_GPIO21_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO21_MCU_DRV_S  5
/** IO_MUX_GPIO21_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO21_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO21_FUN_WPD_M  (IO_MUX_GPIO21_FUN_WPD_V << IO_MUX_GPIO21_FUN_WPD_S)
#define IO_MUX_GPIO21_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO21_FUN_WPD_S  7
/** IO_MUX_GPIO21_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO21_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO21_FUN_WPU_M  (IO_MUX_GPIO21_FUN_WPU_V << IO_MUX_GPIO21_FUN_WPU_S)
#define IO_MUX_GPIO21_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO21_FUN_WPU_S  8
/** IO_MUX_GPIO21_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO21_FUN_IE    (BIT(9))
#define IO_MUX_GPIO21_FUN_IE_M  (IO_MUX_GPIO21_FUN_IE_V << IO_MUX_GPIO21_FUN_IE_S)
#define IO_MUX_GPIO21_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO21_FUN_IE_S  9
/** IO_MUX_GPIO21_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO21_FUN_DRV    0x00000003U
#define IO_MUX_GPIO21_FUN_DRV_M  (IO_MUX_GPIO21_FUN_DRV_V << IO_MUX_GPIO21_FUN_DRV_S)
#define IO_MUX_GPIO21_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO21_FUN_DRV_S  10
/** IO_MUX_GPIO21_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO21_MCU_SEL    0x00000007U
#define IO_MUX_GPIO21_MCU_SEL_M  (IO_MUX_GPIO21_MCU_SEL_V << IO_MUX_GPIO21_MCU_SEL_S)
#define IO_MUX_GPIO21_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO21_MCU_SEL_S  12
/** IO_MUX_GPIO21_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO21_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO21_FILTER_EN_M  (IO_MUX_GPIO21_FILTER_EN_V << IO_MUX_GPIO21_FILTER_EN_S)
#define IO_MUX_GPIO21_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO21_FILTER_EN_S  15

/** IO_MUX_gpio22_REG register
 *  iomux control register for gpio22
 */
#define IO_MUX_GPIO22_REG (DR_REG_IO_MUX_BASE + 0x5c)
/** IO_MUX_GPIO22_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO22_MCU_OE    (BIT(0))
#define IO_MUX_GPIO22_MCU_OE_M  (IO_MUX_GPIO22_MCU_OE_V << IO_MUX_GPIO22_MCU_OE_S)
#define IO_MUX_GPIO22_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO22_MCU_OE_S  0
/** IO_MUX_GPIO22_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO22_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO22_SLP_SEL_M  (IO_MUX_GPIO22_SLP_SEL_V << IO_MUX_GPIO22_SLP_SEL_S)
#define IO_MUX_GPIO22_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO22_SLP_SEL_S  1
/** IO_MUX_GPIO22_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO22_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO22_MCU_WPD_M  (IO_MUX_GPIO22_MCU_WPD_V << IO_MUX_GPIO22_MCU_WPD_S)
#define IO_MUX_GPIO22_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO22_MCU_WPD_S  2
/** IO_MUX_GPIO22_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO22_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO22_MCU_WPU_M  (IO_MUX_GPIO22_MCU_WPU_V << IO_MUX_GPIO22_MCU_WPU_S)
#define IO_MUX_GPIO22_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO22_MCU_WPU_S  3
/** IO_MUX_GPIO22_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO22_MCU_IE    (BIT(4))
#define IO_MUX_GPIO22_MCU_IE_M  (IO_MUX_GPIO22_MCU_IE_V << IO_MUX_GPIO22_MCU_IE_S)
#define IO_MUX_GPIO22_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO22_MCU_IE_S  4
/** IO_MUX_GPIO22_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO22_MCU_DRV    0x00000003U
#define IO_MUX_GPIO22_MCU_DRV_M  (IO_MUX_GPIO22_MCU_DRV_V << IO_MUX_GPIO22_MCU_DRV_S)
#define IO_MUX_GPIO22_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO22_MCU_DRV_S  5
/** IO_MUX_GPIO22_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO22_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO22_FUN_WPD_M  (IO_MUX_GPIO22_FUN_WPD_V << IO_MUX_GPIO22_FUN_WPD_S)
#define IO_MUX_GPIO22_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO22_FUN_WPD_S  7
/** IO_MUX_GPIO22_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO22_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO22_FUN_WPU_M  (IO_MUX_GPIO22_FUN_WPU_V << IO_MUX_GPIO22_FUN_WPU_S)
#define IO_MUX_GPIO22_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO22_FUN_WPU_S  8
/** IO_MUX_GPIO22_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO22_FUN_IE    (BIT(9))
#define IO_MUX_GPIO22_FUN_IE_M  (IO_MUX_GPIO22_FUN_IE_V << IO_MUX_GPIO22_FUN_IE_S)
#define IO_MUX_GPIO22_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO22_FUN_IE_S  9
/** IO_MUX_GPIO22_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO22_FUN_DRV    0x00000003U
#define IO_MUX_GPIO22_FUN_DRV_M  (IO_MUX_GPIO22_FUN_DRV_V << IO_MUX_GPIO22_FUN_DRV_S)
#define IO_MUX_GPIO22_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO22_FUN_DRV_S  10
/** IO_MUX_GPIO22_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO22_MCU_SEL    0x00000007U
#define IO_MUX_GPIO22_MCU_SEL_M  (IO_MUX_GPIO22_MCU_SEL_V << IO_MUX_GPIO22_MCU_SEL_S)
#define IO_MUX_GPIO22_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO22_MCU_SEL_S  12
/** IO_MUX_GPIO22_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO22_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO22_FILTER_EN_M  (IO_MUX_GPIO22_FILTER_EN_V << IO_MUX_GPIO22_FILTER_EN_S)
#define IO_MUX_GPIO22_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO22_FILTER_EN_S  15

/** IO_MUX_gpio23_REG register
 *  iomux control register for gpio23
 */
#define IO_MUX_GPIO23_REG (DR_REG_IO_MUX_BASE + 0x60)
/** IO_MUX_GPIO23_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO23_MCU_OE    (BIT(0))
#define IO_MUX_GPIO23_MCU_OE_M  (IO_MUX_GPIO23_MCU_OE_V << IO_MUX_GPIO23_MCU_OE_S)
#define IO_MUX_GPIO23_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO23_MCU_OE_S  0
/** IO_MUX_GPIO23_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO23_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO23_SLP_SEL_M  (IO_MUX_GPIO23_SLP_SEL_V << IO_MUX_GPIO23_SLP_SEL_S)
#define IO_MUX_GPIO23_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO23_SLP_SEL_S  1
/** IO_MUX_GPIO23_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO23_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO23_MCU_WPD_M  (IO_MUX_GPIO23_MCU_WPD_V << IO_MUX_GPIO23_MCU_WPD_S)
#define IO_MUX_GPIO23_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO23_MCU_WPD_S  2
/** IO_MUX_GPIO23_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO23_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO23_MCU_WPU_M  (IO_MUX_GPIO23_MCU_WPU_V << IO_MUX_GPIO23_MCU_WPU_S)
#define IO_MUX_GPIO23_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO23_MCU_WPU_S  3
/** IO_MUX_GPIO23_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO23_MCU_IE    (BIT(4))
#define IO_MUX_GPIO23_MCU_IE_M  (IO_MUX_GPIO23_MCU_IE_V << IO_MUX_GPIO23_MCU_IE_S)
#define IO_MUX_GPIO23_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO23_MCU_IE_S  4
/** IO_MUX_GPIO23_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO23_MCU_DRV    0x00000003U
#define IO_MUX_GPIO23_MCU_DRV_M  (IO_MUX_GPIO23_MCU_DRV_V << IO_MUX_GPIO23_MCU_DRV_S)
#define IO_MUX_GPIO23_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO23_MCU_DRV_S  5
/** IO_MUX_GPIO23_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO23_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO23_FUN_WPD_M  (IO_MUX_GPIO23_FUN_WPD_V << IO_MUX_GPIO23_FUN_WPD_S)
#define IO_MUX_GPIO23_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO23_FUN_WPD_S  7
/** IO_MUX_GPIO23_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO23_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO23_FUN_WPU_M  (IO_MUX_GPIO23_FUN_WPU_V << IO_MUX_GPIO23_FUN_WPU_S)
#define IO_MUX_GPIO23_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO23_FUN_WPU_S  8
/** IO_MUX_GPIO23_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO23_FUN_IE    (BIT(9))
#define IO_MUX_GPIO23_FUN_IE_M  (IO_MUX_GPIO23_FUN_IE_V << IO_MUX_GPIO23_FUN_IE_S)
#define IO_MUX_GPIO23_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO23_FUN_IE_S  9
/** IO_MUX_GPIO23_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO23_FUN_DRV    0x00000003U
#define IO_MUX_GPIO23_FUN_DRV_M  (IO_MUX_GPIO23_FUN_DRV_V << IO_MUX_GPIO23_FUN_DRV_S)
#define IO_MUX_GPIO23_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO23_FUN_DRV_S  10
/** IO_MUX_GPIO23_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO23_MCU_SEL    0x00000007U
#define IO_MUX_GPIO23_MCU_SEL_M  (IO_MUX_GPIO23_MCU_SEL_V << IO_MUX_GPIO23_MCU_SEL_S)
#define IO_MUX_GPIO23_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO23_MCU_SEL_S  12
/** IO_MUX_GPIO23_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO23_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO23_FILTER_EN_M  (IO_MUX_GPIO23_FILTER_EN_V << IO_MUX_GPIO23_FILTER_EN_S)
#define IO_MUX_GPIO23_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO23_FILTER_EN_S  15

/** IO_MUX_gpio24_REG register
 *  iomux control register for gpio24
 */
#define IO_MUX_GPIO24_REG (DR_REG_IO_MUX_BASE + 0x64)
/** IO_MUX_GPIO24_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO24_MCU_OE    (BIT(0))
#define IO_MUX_GPIO24_MCU_OE_M  (IO_MUX_GPIO24_MCU_OE_V << IO_MUX_GPIO24_MCU_OE_S)
#define IO_MUX_GPIO24_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO24_MCU_OE_S  0
/** IO_MUX_GPIO24_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO24_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO24_SLP_SEL_M  (IO_MUX_GPIO24_SLP_SEL_V << IO_MUX_GPIO24_SLP_SEL_S)
#define IO_MUX_GPIO24_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO24_SLP_SEL_S  1
/** IO_MUX_GPIO24_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO24_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO24_MCU_WPD_M  (IO_MUX_GPIO24_MCU_WPD_V << IO_MUX_GPIO24_MCU_WPD_S)
#define IO_MUX_GPIO24_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO24_MCU_WPD_S  2
/** IO_MUX_GPIO24_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO24_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO24_MCU_WPU_M  (IO_MUX_GPIO24_MCU_WPU_V << IO_MUX_GPIO24_MCU_WPU_S)
#define IO_MUX_GPIO24_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO24_MCU_WPU_S  3
/** IO_MUX_GPIO24_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO24_MCU_IE    (BIT(4))
#define IO_MUX_GPIO24_MCU_IE_M  (IO_MUX_GPIO24_MCU_IE_V << IO_MUX_GPIO24_MCU_IE_S)
#define IO_MUX_GPIO24_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO24_MCU_IE_S  4
/** IO_MUX_GPIO24_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO24_MCU_DRV    0x00000003U
#define IO_MUX_GPIO24_MCU_DRV_M  (IO_MUX_GPIO24_MCU_DRV_V << IO_MUX_GPIO24_MCU_DRV_S)
#define IO_MUX_GPIO24_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO24_MCU_DRV_S  5
/** IO_MUX_GPIO24_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO24_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO24_FUN_WPD_M  (IO_MUX_GPIO24_FUN_WPD_V << IO_MUX_GPIO24_FUN_WPD_S)
#define IO_MUX_GPIO24_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO24_FUN_WPD_S  7
/** IO_MUX_GPIO24_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO24_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO24_FUN_WPU_M  (IO_MUX_GPIO24_FUN_WPU_V << IO_MUX_GPIO24_FUN_WPU_S)
#define IO_MUX_GPIO24_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO24_FUN_WPU_S  8
/** IO_MUX_GPIO24_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO24_FUN_IE    (BIT(9))
#define IO_MUX_GPIO24_FUN_IE_M  (IO_MUX_GPIO24_FUN_IE_V << IO_MUX_GPIO24_FUN_IE_S)
#define IO_MUX_GPIO24_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO24_FUN_IE_S  9
/** IO_MUX_GPIO24_FUN_DRV : R/W; bitpos: [11:10]; default: 3;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO24_FUN_DRV    0x00000003U
#define IO_MUX_GPIO24_FUN_DRV_M  (IO_MUX_GPIO24_FUN_DRV_V << IO_MUX_GPIO24_FUN_DRV_S)
#define IO_MUX_GPIO24_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO24_FUN_DRV_S  10
/** IO_MUX_GPIO24_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO24_MCU_SEL    0x00000007U
#define IO_MUX_GPIO24_MCU_SEL_M  (IO_MUX_GPIO24_MCU_SEL_V << IO_MUX_GPIO24_MCU_SEL_S)
#define IO_MUX_GPIO24_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO24_MCU_SEL_S  12
/** IO_MUX_GPIO24_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO24_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO24_FILTER_EN_M  (IO_MUX_GPIO24_FILTER_EN_V << IO_MUX_GPIO24_FILTER_EN_S)
#define IO_MUX_GPIO24_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO24_FILTER_EN_S  15

/** IO_MUX_gpio25_REG register
 *  iomux control register for gpio25
 */
#define IO_MUX_GPIO25_REG (DR_REG_IO_MUX_BASE + 0x68)
/** IO_MUX_GPIO25_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO25_MCU_OE    (BIT(0))
#define IO_MUX_GPIO25_MCU_OE_M  (IO_MUX_GPIO25_MCU_OE_V << IO_MUX_GPIO25_MCU_OE_S)
#define IO_MUX_GPIO25_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO25_MCU_OE_S  0
/** IO_MUX_GPIO25_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO25_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO25_SLP_SEL_M  (IO_MUX_GPIO25_SLP_SEL_V << IO_MUX_GPIO25_SLP_SEL_S)
#define IO_MUX_GPIO25_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO25_SLP_SEL_S  1
/** IO_MUX_GPIO25_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO25_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO25_MCU_WPD_M  (IO_MUX_GPIO25_MCU_WPD_V << IO_MUX_GPIO25_MCU_WPD_S)
#define IO_MUX_GPIO25_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO25_MCU_WPD_S  2
/** IO_MUX_GPIO25_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO25_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO25_MCU_WPU_M  (IO_MUX_GPIO25_MCU_WPU_V << IO_MUX_GPIO25_MCU_WPU_S)
#define IO_MUX_GPIO25_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO25_MCU_WPU_S  3
/** IO_MUX_GPIO25_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO25_MCU_IE    (BIT(4))
#define IO_MUX_GPIO25_MCU_IE_M  (IO_MUX_GPIO25_MCU_IE_V << IO_MUX_GPIO25_MCU_IE_S)
#define IO_MUX_GPIO25_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO25_MCU_IE_S  4
/** IO_MUX_GPIO25_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO25_MCU_DRV    0x00000003U
#define IO_MUX_GPIO25_MCU_DRV_M  (IO_MUX_GPIO25_MCU_DRV_V << IO_MUX_GPIO25_MCU_DRV_S)
#define IO_MUX_GPIO25_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO25_MCU_DRV_S  5
/** IO_MUX_GPIO25_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO25_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO25_FUN_WPD_M  (IO_MUX_GPIO25_FUN_WPD_V << IO_MUX_GPIO25_FUN_WPD_S)
#define IO_MUX_GPIO25_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO25_FUN_WPD_S  7
/** IO_MUX_GPIO25_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO25_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO25_FUN_WPU_M  (IO_MUX_GPIO25_FUN_WPU_V << IO_MUX_GPIO25_FUN_WPU_S)
#define IO_MUX_GPIO25_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO25_FUN_WPU_S  8
/** IO_MUX_GPIO25_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO25_FUN_IE    (BIT(9))
#define IO_MUX_GPIO25_FUN_IE_M  (IO_MUX_GPIO25_FUN_IE_V << IO_MUX_GPIO25_FUN_IE_S)
#define IO_MUX_GPIO25_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO25_FUN_IE_S  9
/** IO_MUX_GPIO25_FUN_DRV : R/W; bitpos: [11:10]; default: 3;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO25_FUN_DRV    0x00000003U
#define IO_MUX_GPIO25_FUN_DRV_M  (IO_MUX_GPIO25_FUN_DRV_V << IO_MUX_GPIO25_FUN_DRV_S)
#define IO_MUX_GPIO25_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO25_FUN_DRV_S  10
/** IO_MUX_GPIO25_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO25_MCU_SEL    0x00000007U
#define IO_MUX_GPIO25_MCU_SEL_M  (IO_MUX_GPIO25_MCU_SEL_V << IO_MUX_GPIO25_MCU_SEL_S)
#define IO_MUX_GPIO25_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO25_MCU_SEL_S  12
/** IO_MUX_GPIO25_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO25_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO25_FILTER_EN_M  (IO_MUX_GPIO25_FILTER_EN_V << IO_MUX_GPIO25_FILTER_EN_S)
#define IO_MUX_GPIO25_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO25_FILTER_EN_S  15

/** IO_MUX_gpio26_REG register
 *  iomux control register for gpio26
 */
#define IO_MUX_GPIO26_REG (DR_REG_IO_MUX_BASE + 0x6c)
/** IO_MUX_GPIO26_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO26_MCU_OE    (BIT(0))
#define IO_MUX_GPIO26_MCU_OE_M  (IO_MUX_GPIO26_MCU_OE_V << IO_MUX_GPIO26_MCU_OE_S)
#define IO_MUX_GPIO26_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO26_MCU_OE_S  0
/** IO_MUX_GPIO26_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO26_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO26_SLP_SEL_M  (IO_MUX_GPIO26_SLP_SEL_V << IO_MUX_GPIO26_SLP_SEL_S)
#define IO_MUX_GPIO26_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO26_SLP_SEL_S  1
/** IO_MUX_GPIO26_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO26_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO26_MCU_WPD_M  (IO_MUX_GPIO26_MCU_WPD_V << IO_MUX_GPIO26_MCU_WPD_S)
#define IO_MUX_GPIO26_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO26_MCU_WPD_S  2
/** IO_MUX_GPIO26_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO26_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO26_MCU_WPU_M  (IO_MUX_GPIO26_MCU_WPU_V << IO_MUX_GPIO26_MCU_WPU_S)
#define IO_MUX_GPIO26_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO26_MCU_WPU_S  3
/** IO_MUX_GPIO26_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO26_MCU_IE    (BIT(4))
#define IO_MUX_GPIO26_MCU_IE_M  (IO_MUX_GPIO26_MCU_IE_V << IO_MUX_GPIO26_MCU_IE_S)
#define IO_MUX_GPIO26_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO26_MCU_IE_S  4
/** IO_MUX_GPIO26_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO26_MCU_DRV    0x00000003U
#define IO_MUX_GPIO26_MCU_DRV_M  (IO_MUX_GPIO26_MCU_DRV_V << IO_MUX_GPIO26_MCU_DRV_S)
#define IO_MUX_GPIO26_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO26_MCU_DRV_S  5
/** IO_MUX_GPIO26_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO26_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO26_FUN_WPD_M  (IO_MUX_GPIO26_FUN_WPD_V << IO_MUX_GPIO26_FUN_WPD_S)
#define IO_MUX_GPIO26_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO26_FUN_WPD_S  7
/** IO_MUX_GPIO26_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO26_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO26_FUN_WPU_M  (IO_MUX_GPIO26_FUN_WPU_V << IO_MUX_GPIO26_FUN_WPU_S)
#define IO_MUX_GPIO26_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO26_FUN_WPU_S  8
/** IO_MUX_GPIO26_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO26_FUN_IE    (BIT(9))
#define IO_MUX_GPIO26_FUN_IE_M  (IO_MUX_GPIO26_FUN_IE_V << IO_MUX_GPIO26_FUN_IE_S)
#define IO_MUX_GPIO26_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO26_FUN_IE_S  9
/** IO_MUX_GPIO26_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO26_FUN_DRV    0x00000003U
#define IO_MUX_GPIO26_FUN_DRV_M  (IO_MUX_GPIO26_FUN_DRV_V << IO_MUX_GPIO26_FUN_DRV_S)
#define IO_MUX_GPIO26_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO26_FUN_DRV_S  10
/** IO_MUX_GPIO26_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO26_MCU_SEL    0x00000007U
#define IO_MUX_GPIO26_MCU_SEL_M  (IO_MUX_GPIO26_MCU_SEL_V << IO_MUX_GPIO26_MCU_SEL_S)
#define IO_MUX_GPIO26_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO26_MCU_SEL_S  12
/** IO_MUX_GPIO26_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO26_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO26_FILTER_EN_M  (IO_MUX_GPIO26_FILTER_EN_V << IO_MUX_GPIO26_FILTER_EN_S)
#define IO_MUX_GPIO26_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO26_FILTER_EN_S  15

/** IO_MUX_gpio27_REG register
 *  iomux control register for gpio27
 */
#define IO_MUX_GPIO27_REG (DR_REG_IO_MUX_BASE + 0x70)
/** IO_MUX_GPIO27_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO27_MCU_OE    (BIT(0))
#define IO_MUX_GPIO27_MCU_OE_M  (IO_MUX_GPIO27_MCU_OE_V << IO_MUX_GPIO27_MCU_OE_S)
#define IO_MUX_GPIO27_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO27_MCU_OE_S  0
/** IO_MUX_GPIO27_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO27_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO27_SLP_SEL_M  (IO_MUX_GPIO27_SLP_SEL_V << IO_MUX_GPIO27_SLP_SEL_S)
#define IO_MUX_GPIO27_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO27_SLP_SEL_S  1
/** IO_MUX_GPIO27_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO27_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO27_MCU_WPD_M  (IO_MUX_GPIO27_MCU_WPD_V << IO_MUX_GPIO27_MCU_WPD_S)
#define IO_MUX_GPIO27_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO27_MCU_WPD_S  2
/** IO_MUX_GPIO27_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO27_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO27_MCU_WPU_M  (IO_MUX_GPIO27_MCU_WPU_V << IO_MUX_GPIO27_MCU_WPU_S)
#define IO_MUX_GPIO27_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO27_MCU_WPU_S  3
/** IO_MUX_GPIO27_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO27_MCU_IE    (BIT(4))
#define IO_MUX_GPIO27_MCU_IE_M  (IO_MUX_GPIO27_MCU_IE_V << IO_MUX_GPIO27_MCU_IE_S)
#define IO_MUX_GPIO27_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO27_MCU_IE_S  4
/** IO_MUX_GPIO27_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO27_MCU_DRV    0x00000003U
#define IO_MUX_GPIO27_MCU_DRV_M  (IO_MUX_GPIO27_MCU_DRV_V << IO_MUX_GPIO27_MCU_DRV_S)
#define IO_MUX_GPIO27_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO27_MCU_DRV_S  5
/** IO_MUX_GPIO27_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO27_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO27_FUN_WPD_M  (IO_MUX_GPIO27_FUN_WPD_V << IO_MUX_GPIO27_FUN_WPD_S)
#define IO_MUX_GPIO27_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO27_FUN_WPD_S  7
/** IO_MUX_GPIO27_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO27_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO27_FUN_WPU_M  (IO_MUX_GPIO27_FUN_WPU_V << IO_MUX_GPIO27_FUN_WPU_S)
#define IO_MUX_GPIO27_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO27_FUN_WPU_S  8
/** IO_MUX_GPIO27_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO27_FUN_IE    (BIT(9))
#define IO_MUX_GPIO27_FUN_IE_M  (IO_MUX_GPIO27_FUN_IE_V << IO_MUX_GPIO27_FUN_IE_S)
#define IO_MUX_GPIO27_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO27_FUN_IE_S  9
/** IO_MUX_GPIO27_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO27_FUN_DRV    0x00000003U
#define IO_MUX_GPIO27_FUN_DRV_M  (IO_MUX_GPIO27_FUN_DRV_V << IO_MUX_GPIO27_FUN_DRV_S)
#define IO_MUX_GPIO27_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO27_FUN_DRV_S  10
/** IO_MUX_GPIO27_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO27_MCU_SEL    0x00000007U
#define IO_MUX_GPIO27_MCU_SEL_M  (IO_MUX_GPIO27_MCU_SEL_V << IO_MUX_GPIO27_MCU_SEL_S)
#define IO_MUX_GPIO27_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO27_MCU_SEL_S  12
/** IO_MUX_GPIO27_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO27_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO27_FILTER_EN_M  (IO_MUX_GPIO27_FILTER_EN_V << IO_MUX_GPIO27_FILTER_EN_S)
#define IO_MUX_GPIO27_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO27_FILTER_EN_S  15

/** IO_MUX_gpio28_REG register
 *  iomux control register for gpio28
 */
#define IO_MUX_GPIO28_REG (DR_REG_IO_MUX_BASE + 0x74)
/** IO_MUX_GPIO28_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO28_MCU_OE    (BIT(0))
#define IO_MUX_GPIO28_MCU_OE_M  (IO_MUX_GPIO28_MCU_OE_V << IO_MUX_GPIO28_MCU_OE_S)
#define IO_MUX_GPIO28_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO28_MCU_OE_S  0
/** IO_MUX_GPIO28_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO28_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO28_SLP_SEL_M  (IO_MUX_GPIO28_SLP_SEL_V << IO_MUX_GPIO28_SLP_SEL_S)
#define IO_MUX_GPIO28_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO28_SLP_SEL_S  1
/** IO_MUX_GPIO28_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO28_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO28_MCU_WPD_M  (IO_MUX_GPIO28_MCU_WPD_V << IO_MUX_GPIO28_MCU_WPD_S)
#define IO_MUX_GPIO28_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO28_MCU_WPD_S  2
/** IO_MUX_GPIO28_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO28_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO28_MCU_WPU_M  (IO_MUX_GPIO28_MCU_WPU_V << IO_MUX_GPIO28_MCU_WPU_S)
#define IO_MUX_GPIO28_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO28_MCU_WPU_S  3
/** IO_MUX_GPIO28_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO28_MCU_IE    (BIT(4))
#define IO_MUX_GPIO28_MCU_IE_M  (IO_MUX_GPIO28_MCU_IE_V << IO_MUX_GPIO28_MCU_IE_S)
#define IO_MUX_GPIO28_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO28_MCU_IE_S  4
/** IO_MUX_GPIO28_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO28_MCU_DRV    0x00000003U
#define IO_MUX_GPIO28_MCU_DRV_M  (IO_MUX_GPIO28_MCU_DRV_V << IO_MUX_GPIO28_MCU_DRV_S)
#define IO_MUX_GPIO28_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO28_MCU_DRV_S  5
/** IO_MUX_GPIO28_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO28_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO28_FUN_WPD_M  (IO_MUX_GPIO28_FUN_WPD_V << IO_MUX_GPIO28_FUN_WPD_S)
#define IO_MUX_GPIO28_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO28_FUN_WPD_S  7
/** IO_MUX_GPIO28_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO28_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO28_FUN_WPU_M  (IO_MUX_GPIO28_FUN_WPU_V << IO_MUX_GPIO28_FUN_WPU_S)
#define IO_MUX_GPIO28_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO28_FUN_WPU_S  8
/** IO_MUX_GPIO28_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO28_FUN_IE    (BIT(9))
#define IO_MUX_GPIO28_FUN_IE_M  (IO_MUX_GPIO28_FUN_IE_V << IO_MUX_GPIO28_FUN_IE_S)
#define IO_MUX_GPIO28_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO28_FUN_IE_S  9
/** IO_MUX_GPIO28_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO28_FUN_DRV    0x00000003U
#define IO_MUX_GPIO28_FUN_DRV_M  (IO_MUX_GPIO28_FUN_DRV_V << IO_MUX_GPIO28_FUN_DRV_S)
#define IO_MUX_GPIO28_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO28_FUN_DRV_S  10
/** IO_MUX_GPIO28_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO28_MCU_SEL    0x00000007U
#define IO_MUX_GPIO28_MCU_SEL_M  (IO_MUX_GPIO28_MCU_SEL_V << IO_MUX_GPIO28_MCU_SEL_S)
#define IO_MUX_GPIO28_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO28_MCU_SEL_S  12
/** IO_MUX_GPIO28_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO28_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO28_FILTER_EN_M  (IO_MUX_GPIO28_FILTER_EN_V << IO_MUX_GPIO28_FILTER_EN_S)
#define IO_MUX_GPIO28_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO28_FILTER_EN_S  15

/** IO_MUX_gpio29_REG register
 *  iomux control register for gpio29
 */
#define IO_MUX_GPIO29_REG (DR_REG_IO_MUX_BASE + 0x78)
/** IO_MUX_GPIO29_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO29_MCU_OE    (BIT(0))
#define IO_MUX_GPIO29_MCU_OE_M  (IO_MUX_GPIO29_MCU_OE_V << IO_MUX_GPIO29_MCU_OE_S)
#define IO_MUX_GPIO29_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO29_MCU_OE_S  0
/** IO_MUX_GPIO29_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO29_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO29_SLP_SEL_M  (IO_MUX_GPIO29_SLP_SEL_V << IO_MUX_GPIO29_SLP_SEL_S)
#define IO_MUX_GPIO29_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO29_SLP_SEL_S  1
/** IO_MUX_GPIO29_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO29_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO29_MCU_WPD_M  (IO_MUX_GPIO29_MCU_WPD_V << IO_MUX_GPIO29_MCU_WPD_S)
#define IO_MUX_GPIO29_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO29_MCU_WPD_S  2
/** IO_MUX_GPIO29_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO29_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO29_MCU_WPU_M  (IO_MUX_GPIO29_MCU_WPU_V << IO_MUX_GPIO29_MCU_WPU_S)
#define IO_MUX_GPIO29_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO29_MCU_WPU_S  3
/** IO_MUX_GPIO29_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO29_MCU_IE    (BIT(4))
#define IO_MUX_GPIO29_MCU_IE_M  (IO_MUX_GPIO29_MCU_IE_V << IO_MUX_GPIO29_MCU_IE_S)
#define IO_MUX_GPIO29_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO29_MCU_IE_S  4
/** IO_MUX_GPIO29_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO29_MCU_DRV    0x00000003U
#define IO_MUX_GPIO29_MCU_DRV_M  (IO_MUX_GPIO29_MCU_DRV_V << IO_MUX_GPIO29_MCU_DRV_S)
#define IO_MUX_GPIO29_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO29_MCU_DRV_S  5
/** IO_MUX_GPIO29_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO29_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO29_FUN_WPD_M  (IO_MUX_GPIO29_FUN_WPD_V << IO_MUX_GPIO29_FUN_WPD_S)
#define IO_MUX_GPIO29_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO29_FUN_WPD_S  7
/** IO_MUX_GPIO29_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO29_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO29_FUN_WPU_M  (IO_MUX_GPIO29_FUN_WPU_V << IO_MUX_GPIO29_FUN_WPU_S)
#define IO_MUX_GPIO29_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO29_FUN_WPU_S  8
/** IO_MUX_GPIO29_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO29_FUN_IE    (BIT(9))
#define IO_MUX_GPIO29_FUN_IE_M  (IO_MUX_GPIO29_FUN_IE_V << IO_MUX_GPIO29_FUN_IE_S)
#define IO_MUX_GPIO29_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO29_FUN_IE_S  9
/** IO_MUX_GPIO29_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO29_FUN_DRV    0x00000003U
#define IO_MUX_GPIO29_FUN_DRV_M  (IO_MUX_GPIO29_FUN_DRV_V << IO_MUX_GPIO29_FUN_DRV_S)
#define IO_MUX_GPIO29_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO29_FUN_DRV_S  10
/** IO_MUX_GPIO29_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO29_MCU_SEL    0x00000007U
#define IO_MUX_GPIO29_MCU_SEL_M  (IO_MUX_GPIO29_MCU_SEL_V << IO_MUX_GPIO29_MCU_SEL_S)
#define IO_MUX_GPIO29_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO29_MCU_SEL_S  12
/** IO_MUX_GPIO29_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO29_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO29_FILTER_EN_M  (IO_MUX_GPIO29_FILTER_EN_V << IO_MUX_GPIO29_FILTER_EN_S)
#define IO_MUX_GPIO29_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO29_FILTER_EN_S  15

/** IO_MUX_gpio30_REG register
 *  iomux control register for gpio30
 */
#define IO_MUX_GPIO30_REG (DR_REG_IO_MUX_BASE + 0x7c)
/** IO_MUX_GPIO30_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO30_MCU_OE    (BIT(0))
#define IO_MUX_GPIO30_MCU_OE_M  (IO_MUX_GPIO30_MCU_OE_V << IO_MUX_GPIO30_MCU_OE_S)
#define IO_MUX_GPIO30_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO30_MCU_OE_S  0
/** IO_MUX_GPIO30_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO30_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO30_SLP_SEL_M  (IO_MUX_GPIO30_SLP_SEL_V << IO_MUX_GPIO30_SLP_SEL_S)
#define IO_MUX_GPIO30_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO30_SLP_SEL_S  1
/** IO_MUX_GPIO30_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO30_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO30_MCU_WPD_M  (IO_MUX_GPIO30_MCU_WPD_V << IO_MUX_GPIO30_MCU_WPD_S)
#define IO_MUX_GPIO30_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO30_MCU_WPD_S  2
/** IO_MUX_GPIO30_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO30_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO30_MCU_WPU_M  (IO_MUX_GPIO30_MCU_WPU_V << IO_MUX_GPIO30_MCU_WPU_S)
#define IO_MUX_GPIO30_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO30_MCU_WPU_S  3
/** IO_MUX_GPIO30_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO30_MCU_IE    (BIT(4))
#define IO_MUX_GPIO30_MCU_IE_M  (IO_MUX_GPIO30_MCU_IE_V << IO_MUX_GPIO30_MCU_IE_S)
#define IO_MUX_GPIO30_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO30_MCU_IE_S  4
/** IO_MUX_GPIO30_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO30_MCU_DRV    0x00000003U
#define IO_MUX_GPIO30_MCU_DRV_M  (IO_MUX_GPIO30_MCU_DRV_V << IO_MUX_GPIO30_MCU_DRV_S)
#define IO_MUX_GPIO30_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO30_MCU_DRV_S  5
/** IO_MUX_GPIO30_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO30_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO30_FUN_WPD_M  (IO_MUX_GPIO30_FUN_WPD_V << IO_MUX_GPIO30_FUN_WPD_S)
#define IO_MUX_GPIO30_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO30_FUN_WPD_S  7
/** IO_MUX_GPIO30_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO30_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO30_FUN_WPU_M  (IO_MUX_GPIO30_FUN_WPU_V << IO_MUX_GPIO30_FUN_WPU_S)
#define IO_MUX_GPIO30_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO30_FUN_WPU_S  8
/** IO_MUX_GPIO30_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO30_FUN_IE    (BIT(9))
#define IO_MUX_GPIO30_FUN_IE_M  (IO_MUX_GPIO30_FUN_IE_V << IO_MUX_GPIO30_FUN_IE_S)
#define IO_MUX_GPIO30_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO30_FUN_IE_S  9
/** IO_MUX_GPIO30_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO30_FUN_DRV    0x00000003U
#define IO_MUX_GPIO30_FUN_DRV_M  (IO_MUX_GPIO30_FUN_DRV_V << IO_MUX_GPIO30_FUN_DRV_S)
#define IO_MUX_GPIO30_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO30_FUN_DRV_S  10
/** IO_MUX_GPIO30_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO30_MCU_SEL    0x00000007U
#define IO_MUX_GPIO30_MCU_SEL_M  (IO_MUX_GPIO30_MCU_SEL_V << IO_MUX_GPIO30_MCU_SEL_S)
#define IO_MUX_GPIO30_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO30_MCU_SEL_S  12
/** IO_MUX_GPIO30_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO30_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO30_FILTER_EN_M  (IO_MUX_GPIO30_FILTER_EN_V << IO_MUX_GPIO30_FILTER_EN_S)
#define IO_MUX_GPIO30_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO30_FILTER_EN_S  15

/** IO_MUX_gpio31_REG register
 *  iomux control register for gpio31
 */
#define IO_MUX_GPIO31_REG (DR_REG_IO_MUX_BASE + 0x80)
/** IO_MUX_GPIO31_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO31_MCU_OE    (BIT(0))
#define IO_MUX_GPIO31_MCU_OE_M  (IO_MUX_GPIO31_MCU_OE_V << IO_MUX_GPIO31_MCU_OE_S)
#define IO_MUX_GPIO31_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO31_MCU_OE_S  0
/** IO_MUX_GPIO31_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO31_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO31_SLP_SEL_M  (IO_MUX_GPIO31_SLP_SEL_V << IO_MUX_GPIO31_SLP_SEL_S)
#define IO_MUX_GPIO31_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO31_SLP_SEL_S  1
/** IO_MUX_GPIO31_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO31_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO31_MCU_WPD_M  (IO_MUX_GPIO31_MCU_WPD_V << IO_MUX_GPIO31_MCU_WPD_S)
#define IO_MUX_GPIO31_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO31_MCU_WPD_S  2
/** IO_MUX_GPIO31_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO31_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO31_MCU_WPU_M  (IO_MUX_GPIO31_MCU_WPU_V << IO_MUX_GPIO31_MCU_WPU_S)
#define IO_MUX_GPIO31_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO31_MCU_WPU_S  3
/** IO_MUX_GPIO31_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO31_MCU_IE    (BIT(4))
#define IO_MUX_GPIO31_MCU_IE_M  (IO_MUX_GPIO31_MCU_IE_V << IO_MUX_GPIO31_MCU_IE_S)
#define IO_MUX_GPIO31_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO31_MCU_IE_S  4
/** IO_MUX_GPIO31_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO31_MCU_DRV    0x00000003U
#define IO_MUX_GPIO31_MCU_DRV_M  (IO_MUX_GPIO31_MCU_DRV_V << IO_MUX_GPIO31_MCU_DRV_S)
#define IO_MUX_GPIO31_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO31_MCU_DRV_S  5
/** IO_MUX_GPIO31_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO31_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO31_FUN_WPD_M  (IO_MUX_GPIO31_FUN_WPD_V << IO_MUX_GPIO31_FUN_WPD_S)
#define IO_MUX_GPIO31_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO31_FUN_WPD_S  7
/** IO_MUX_GPIO31_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO31_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO31_FUN_WPU_M  (IO_MUX_GPIO31_FUN_WPU_V << IO_MUX_GPIO31_FUN_WPU_S)
#define IO_MUX_GPIO31_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO31_FUN_WPU_S  8
/** IO_MUX_GPIO31_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO31_FUN_IE    (BIT(9))
#define IO_MUX_GPIO31_FUN_IE_M  (IO_MUX_GPIO31_FUN_IE_V << IO_MUX_GPIO31_FUN_IE_S)
#define IO_MUX_GPIO31_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO31_FUN_IE_S  9
/** IO_MUX_GPIO31_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO31_FUN_DRV    0x00000003U
#define IO_MUX_GPIO31_FUN_DRV_M  (IO_MUX_GPIO31_FUN_DRV_V << IO_MUX_GPIO31_FUN_DRV_S)
#define IO_MUX_GPIO31_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO31_FUN_DRV_S  10
/** IO_MUX_GPIO31_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO31_MCU_SEL    0x00000007U
#define IO_MUX_GPIO31_MCU_SEL_M  (IO_MUX_GPIO31_MCU_SEL_V << IO_MUX_GPIO31_MCU_SEL_S)
#define IO_MUX_GPIO31_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO31_MCU_SEL_S  12
/** IO_MUX_GPIO31_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO31_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO31_FILTER_EN_M  (IO_MUX_GPIO31_FILTER_EN_V << IO_MUX_GPIO31_FILTER_EN_S)
#define IO_MUX_GPIO31_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO31_FILTER_EN_S  15

/** IO_MUX_gpio32_REG register
 *  iomux control register for gpio32
 */
#define IO_MUX_GPIO32_REG (DR_REG_IO_MUX_BASE + 0x84)
/** IO_MUX_GPIO32_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO32_MCU_OE    (BIT(0))
#define IO_MUX_GPIO32_MCU_OE_M  (IO_MUX_GPIO32_MCU_OE_V << IO_MUX_GPIO32_MCU_OE_S)
#define IO_MUX_GPIO32_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO32_MCU_OE_S  0
/** IO_MUX_GPIO32_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO32_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO32_SLP_SEL_M  (IO_MUX_GPIO32_SLP_SEL_V << IO_MUX_GPIO32_SLP_SEL_S)
#define IO_MUX_GPIO32_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO32_SLP_SEL_S  1
/** IO_MUX_GPIO32_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO32_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO32_MCU_WPD_M  (IO_MUX_GPIO32_MCU_WPD_V << IO_MUX_GPIO32_MCU_WPD_S)
#define IO_MUX_GPIO32_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO32_MCU_WPD_S  2
/** IO_MUX_GPIO32_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO32_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO32_MCU_WPU_M  (IO_MUX_GPIO32_MCU_WPU_V << IO_MUX_GPIO32_MCU_WPU_S)
#define IO_MUX_GPIO32_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO32_MCU_WPU_S  3
/** IO_MUX_GPIO32_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO32_MCU_IE    (BIT(4))
#define IO_MUX_GPIO32_MCU_IE_M  (IO_MUX_GPIO32_MCU_IE_V << IO_MUX_GPIO32_MCU_IE_S)
#define IO_MUX_GPIO32_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO32_MCU_IE_S  4
/** IO_MUX_GPIO32_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO32_MCU_DRV    0x00000003U
#define IO_MUX_GPIO32_MCU_DRV_M  (IO_MUX_GPIO32_MCU_DRV_V << IO_MUX_GPIO32_MCU_DRV_S)
#define IO_MUX_GPIO32_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO32_MCU_DRV_S  5
/** IO_MUX_GPIO32_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO32_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO32_FUN_WPD_M  (IO_MUX_GPIO32_FUN_WPD_V << IO_MUX_GPIO32_FUN_WPD_S)
#define IO_MUX_GPIO32_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO32_FUN_WPD_S  7
/** IO_MUX_GPIO32_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO32_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO32_FUN_WPU_M  (IO_MUX_GPIO32_FUN_WPU_V << IO_MUX_GPIO32_FUN_WPU_S)
#define IO_MUX_GPIO32_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO32_FUN_WPU_S  8
/** IO_MUX_GPIO32_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO32_FUN_IE    (BIT(9))
#define IO_MUX_GPIO32_FUN_IE_M  (IO_MUX_GPIO32_FUN_IE_V << IO_MUX_GPIO32_FUN_IE_S)
#define IO_MUX_GPIO32_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO32_FUN_IE_S  9
/** IO_MUX_GPIO32_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO32_FUN_DRV    0x00000003U
#define IO_MUX_GPIO32_FUN_DRV_M  (IO_MUX_GPIO32_FUN_DRV_V << IO_MUX_GPIO32_FUN_DRV_S)
#define IO_MUX_GPIO32_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO32_FUN_DRV_S  10
/** IO_MUX_GPIO32_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO32_MCU_SEL    0x00000007U
#define IO_MUX_GPIO32_MCU_SEL_M  (IO_MUX_GPIO32_MCU_SEL_V << IO_MUX_GPIO32_MCU_SEL_S)
#define IO_MUX_GPIO32_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO32_MCU_SEL_S  12
/** IO_MUX_GPIO32_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO32_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO32_FILTER_EN_M  (IO_MUX_GPIO32_FILTER_EN_V << IO_MUX_GPIO32_FILTER_EN_S)
#define IO_MUX_GPIO32_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO32_FILTER_EN_S  15
/** IO_MUX_GPIO32_RUE_I3C : R/W; bitpos: [16]; default: 0;
 *  NA
 */
#define IO_MUX_GPIO32_RUE_I3C    (BIT(16))
#define IO_MUX_GPIO32_RUE_I3C_M  (IO_MUX_GPIO32_RUE_I3C_V << IO_MUX_GPIO32_RUE_I3C_S)
#define IO_MUX_GPIO32_RUE_I3C_V  0x00000001U
#define IO_MUX_GPIO32_RUE_I3C_S  16
/** IO_MUX_GPIO32_RU_I3C : R/W; bitpos: [18:17]; default: 0;
 *  NA
 */
#define IO_MUX_GPIO32_RU_I3C    0x00000003U
#define IO_MUX_GPIO32_RU_I3C_M  (IO_MUX_GPIO32_RU_I3C_V << IO_MUX_GPIO32_RU_I3C_S)
#define IO_MUX_GPIO32_RU_I3C_V  0x00000003U
#define IO_MUX_GPIO32_RU_I3C_S  17
/** IO_MUX_GPIO32_RUE_SEL_I3C : R/W; bitpos: [19]; default: 0;
 *  NA
 */
#define IO_MUX_GPIO32_RUE_SEL_I3C    (BIT(19))
#define IO_MUX_GPIO32_RUE_SEL_I3C_M  (IO_MUX_GPIO32_RUE_SEL_I3C_V << IO_MUX_GPIO32_RUE_SEL_I3C_S)
#define IO_MUX_GPIO32_RUE_SEL_I3C_V  0x00000001U
#define IO_MUX_GPIO32_RUE_SEL_I3C_S  19

/** IO_MUX_gpio33_REG register
 *  iomux control register for gpio33
 */
#define IO_MUX_GPIO33_REG (DR_REG_IO_MUX_BASE + 0x88)
/** IO_MUX_GPIO33_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO33_MCU_OE    (BIT(0))
#define IO_MUX_GPIO33_MCU_OE_M  (IO_MUX_GPIO33_MCU_OE_V << IO_MUX_GPIO33_MCU_OE_S)
#define IO_MUX_GPIO33_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO33_MCU_OE_S  0
/** IO_MUX_GPIO33_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO33_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO33_SLP_SEL_M  (IO_MUX_GPIO33_SLP_SEL_V << IO_MUX_GPIO33_SLP_SEL_S)
#define IO_MUX_GPIO33_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO33_SLP_SEL_S  1
/** IO_MUX_GPIO33_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO33_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO33_MCU_WPD_M  (IO_MUX_GPIO33_MCU_WPD_V << IO_MUX_GPIO33_MCU_WPD_S)
#define IO_MUX_GPIO33_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO33_MCU_WPD_S  2
/** IO_MUX_GPIO33_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO33_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO33_MCU_WPU_M  (IO_MUX_GPIO33_MCU_WPU_V << IO_MUX_GPIO33_MCU_WPU_S)
#define IO_MUX_GPIO33_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO33_MCU_WPU_S  3
/** IO_MUX_GPIO33_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO33_MCU_IE    (BIT(4))
#define IO_MUX_GPIO33_MCU_IE_M  (IO_MUX_GPIO33_MCU_IE_V << IO_MUX_GPIO33_MCU_IE_S)
#define IO_MUX_GPIO33_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO33_MCU_IE_S  4
/** IO_MUX_GPIO33_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO33_MCU_DRV    0x00000003U
#define IO_MUX_GPIO33_MCU_DRV_M  (IO_MUX_GPIO33_MCU_DRV_V << IO_MUX_GPIO33_MCU_DRV_S)
#define IO_MUX_GPIO33_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO33_MCU_DRV_S  5
/** IO_MUX_GPIO33_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO33_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO33_FUN_WPD_M  (IO_MUX_GPIO33_FUN_WPD_V << IO_MUX_GPIO33_FUN_WPD_S)
#define IO_MUX_GPIO33_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO33_FUN_WPD_S  7
/** IO_MUX_GPIO33_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO33_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO33_FUN_WPU_M  (IO_MUX_GPIO33_FUN_WPU_V << IO_MUX_GPIO33_FUN_WPU_S)
#define IO_MUX_GPIO33_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO33_FUN_WPU_S  8
/** IO_MUX_GPIO33_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO33_FUN_IE    (BIT(9))
#define IO_MUX_GPIO33_FUN_IE_M  (IO_MUX_GPIO33_FUN_IE_V << IO_MUX_GPIO33_FUN_IE_S)
#define IO_MUX_GPIO33_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO33_FUN_IE_S  9
/** IO_MUX_GPIO33_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO33_FUN_DRV    0x00000003U
#define IO_MUX_GPIO33_FUN_DRV_M  (IO_MUX_GPIO33_FUN_DRV_V << IO_MUX_GPIO33_FUN_DRV_S)
#define IO_MUX_GPIO33_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO33_FUN_DRV_S  10
/** IO_MUX_GPIO33_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO33_MCU_SEL    0x00000007U
#define IO_MUX_GPIO33_MCU_SEL_M  (IO_MUX_GPIO33_MCU_SEL_V << IO_MUX_GPIO33_MCU_SEL_S)
#define IO_MUX_GPIO33_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO33_MCU_SEL_S  12
/** IO_MUX_GPIO33_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO33_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO33_FILTER_EN_M  (IO_MUX_GPIO33_FILTER_EN_V << IO_MUX_GPIO33_FILTER_EN_S)
#define IO_MUX_GPIO33_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO33_FILTER_EN_S  15
/** IO_MUX_GPIO33_RUE_I3C : R/W; bitpos: [16]; default: 0;
 *  NA
 */
#define IO_MUX_GPIO33_RUE_I3C    (BIT(16))
#define IO_MUX_GPIO33_RUE_I3C_M  (IO_MUX_GPIO33_RUE_I3C_V << IO_MUX_GPIO33_RUE_I3C_S)
#define IO_MUX_GPIO33_RUE_I3C_V  0x00000001U
#define IO_MUX_GPIO33_RUE_I3C_S  16
/** IO_MUX_GPIO33_RU_I3C : R/W; bitpos: [18:17]; default: 0;
 *  NA
 */
#define IO_MUX_GPIO33_RU_I3C    0x00000003U
#define IO_MUX_GPIO33_RU_I3C_M  (IO_MUX_GPIO33_RU_I3C_V << IO_MUX_GPIO33_RU_I3C_S)
#define IO_MUX_GPIO33_RU_I3C_V  0x00000003U
#define IO_MUX_GPIO33_RU_I3C_S  17
/** IO_MUX_GPIO33_RUE_SEL_I3C : R/W; bitpos: [19]; default: 0;
 *  NA
 */
#define IO_MUX_GPIO33_RUE_SEL_I3C    (BIT(19))
#define IO_MUX_GPIO33_RUE_SEL_I3C_M  (IO_MUX_GPIO33_RUE_SEL_I3C_V << IO_MUX_GPIO33_RUE_SEL_I3C_S)
#define IO_MUX_GPIO33_RUE_SEL_I3C_V  0x00000001U
#define IO_MUX_GPIO33_RUE_SEL_I3C_S  19

/** IO_MUX_gpio34_REG register
 *  iomux control register for gpio34
 */
#define IO_MUX_GPIO34_REG (DR_REG_IO_MUX_BASE + 0x8c)
/** IO_MUX_GPIO34_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO34_MCU_OE    (BIT(0))
#define IO_MUX_GPIO34_MCU_OE_M  (IO_MUX_GPIO34_MCU_OE_V << IO_MUX_GPIO34_MCU_OE_S)
#define IO_MUX_GPIO34_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO34_MCU_OE_S  0
/** IO_MUX_GPIO34_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO34_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO34_SLP_SEL_M  (IO_MUX_GPIO34_SLP_SEL_V << IO_MUX_GPIO34_SLP_SEL_S)
#define IO_MUX_GPIO34_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO34_SLP_SEL_S  1
/** IO_MUX_GPIO34_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO34_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO34_MCU_WPD_M  (IO_MUX_GPIO34_MCU_WPD_V << IO_MUX_GPIO34_MCU_WPD_S)
#define IO_MUX_GPIO34_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO34_MCU_WPD_S  2
/** IO_MUX_GPIO34_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO34_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO34_MCU_WPU_M  (IO_MUX_GPIO34_MCU_WPU_V << IO_MUX_GPIO34_MCU_WPU_S)
#define IO_MUX_GPIO34_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO34_MCU_WPU_S  3
/** IO_MUX_GPIO34_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO34_MCU_IE    (BIT(4))
#define IO_MUX_GPIO34_MCU_IE_M  (IO_MUX_GPIO34_MCU_IE_V << IO_MUX_GPIO34_MCU_IE_S)
#define IO_MUX_GPIO34_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO34_MCU_IE_S  4
/** IO_MUX_GPIO34_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO34_MCU_DRV    0x00000003U
#define IO_MUX_GPIO34_MCU_DRV_M  (IO_MUX_GPIO34_MCU_DRV_V << IO_MUX_GPIO34_MCU_DRV_S)
#define IO_MUX_GPIO34_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO34_MCU_DRV_S  5
/** IO_MUX_GPIO34_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO34_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO34_FUN_WPD_M  (IO_MUX_GPIO34_FUN_WPD_V << IO_MUX_GPIO34_FUN_WPD_S)
#define IO_MUX_GPIO34_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO34_FUN_WPD_S  7
/** IO_MUX_GPIO34_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO34_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO34_FUN_WPU_M  (IO_MUX_GPIO34_FUN_WPU_V << IO_MUX_GPIO34_FUN_WPU_S)
#define IO_MUX_GPIO34_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO34_FUN_WPU_S  8
/** IO_MUX_GPIO34_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO34_FUN_IE    (BIT(9))
#define IO_MUX_GPIO34_FUN_IE_M  (IO_MUX_GPIO34_FUN_IE_V << IO_MUX_GPIO34_FUN_IE_S)
#define IO_MUX_GPIO34_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO34_FUN_IE_S  9
/** IO_MUX_GPIO34_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO34_FUN_DRV    0x00000003U
#define IO_MUX_GPIO34_FUN_DRV_M  (IO_MUX_GPIO34_FUN_DRV_V << IO_MUX_GPIO34_FUN_DRV_S)
#define IO_MUX_GPIO34_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO34_FUN_DRV_S  10
/** IO_MUX_GPIO34_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO34_MCU_SEL    0x00000007U
#define IO_MUX_GPIO34_MCU_SEL_M  (IO_MUX_GPIO34_MCU_SEL_V << IO_MUX_GPIO34_MCU_SEL_S)
#define IO_MUX_GPIO34_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO34_MCU_SEL_S  12
/** IO_MUX_GPIO34_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO34_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO34_FILTER_EN_M  (IO_MUX_GPIO34_FILTER_EN_V << IO_MUX_GPIO34_FILTER_EN_S)
#define IO_MUX_GPIO34_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO34_FILTER_EN_S  15

/** IO_MUX_gpio35_REG register
 *  iomux control register for gpio35
 */
#define IO_MUX_GPIO35_REG (DR_REG_IO_MUX_BASE + 0x90)
/** IO_MUX_GPIO35_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO35_MCU_OE    (BIT(0))
#define IO_MUX_GPIO35_MCU_OE_M  (IO_MUX_GPIO35_MCU_OE_V << IO_MUX_GPIO35_MCU_OE_S)
#define IO_MUX_GPIO35_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO35_MCU_OE_S  0
/** IO_MUX_GPIO35_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO35_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO35_SLP_SEL_M  (IO_MUX_GPIO35_SLP_SEL_V << IO_MUX_GPIO35_SLP_SEL_S)
#define IO_MUX_GPIO35_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO35_SLP_SEL_S  1
/** IO_MUX_GPIO35_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO35_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO35_MCU_WPD_M  (IO_MUX_GPIO35_MCU_WPD_V << IO_MUX_GPIO35_MCU_WPD_S)
#define IO_MUX_GPIO35_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO35_MCU_WPD_S  2
/** IO_MUX_GPIO35_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO35_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO35_MCU_WPU_M  (IO_MUX_GPIO35_MCU_WPU_V << IO_MUX_GPIO35_MCU_WPU_S)
#define IO_MUX_GPIO35_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO35_MCU_WPU_S  3
/** IO_MUX_GPIO35_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO35_MCU_IE    (BIT(4))
#define IO_MUX_GPIO35_MCU_IE_M  (IO_MUX_GPIO35_MCU_IE_V << IO_MUX_GPIO35_MCU_IE_S)
#define IO_MUX_GPIO35_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO35_MCU_IE_S  4
/** IO_MUX_GPIO35_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO35_MCU_DRV    0x00000003U
#define IO_MUX_GPIO35_MCU_DRV_M  (IO_MUX_GPIO35_MCU_DRV_V << IO_MUX_GPIO35_MCU_DRV_S)
#define IO_MUX_GPIO35_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO35_MCU_DRV_S  5
/** IO_MUX_GPIO35_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO35_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO35_FUN_WPD_M  (IO_MUX_GPIO35_FUN_WPD_V << IO_MUX_GPIO35_FUN_WPD_S)
#define IO_MUX_GPIO35_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO35_FUN_WPD_S  7
/** IO_MUX_GPIO35_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO35_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO35_FUN_WPU_M  (IO_MUX_GPIO35_FUN_WPU_V << IO_MUX_GPIO35_FUN_WPU_S)
#define IO_MUX_GPIO35_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO35_FUN_WPU_S  8
/** IO_MUX_GPIO35_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO35_FUN_IE    (BIT(9))
#define IO_MUX_GPIO35_FUN_IE_M  (IO_MUX_GPIO35_FUN_IE_V << IO_MUX_GPIO35_FUN_IE_S)
#define IO_MUX_GPIO35_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO35_FUN_IE_S  9
/** IO_MUX_GPIO35_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO35_FUN_DRV    0x00000003U
#define IO_MUX_GPIO35_FUN_DRV_M  (IO_MUX_GPIO35_FUN_DRV_V << IO_MUX_GPIO35_FUN_DRV_S)
#define IO_MUX_GPIO35_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO35_FUN_DRV_S  10
/** IO_MUX_GPIO35_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO35_MCU_SEL    0x00000007U
#define IO_MUX_GPIO35_MCU_SEL_M  (IO_MUX_GPIO35_MCU_SEL_V << IO_MUX_GPIO35_MCU_SEL_S)
#define IO_MUX_GPIO35_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO35_MCU_SEL_S  12
/** IO_MUX_GPIO35_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO35_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO35_FILTER_EN_M  (IO_MUX_GPIO35_FILTER_EN_V << IO_MUX_GPIO35_FILTER_EN_S)
#define IO_MUX_GPIO35_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO35_FILTER_EN_S  15

/** IO_MUX_gpio36_REG register
 *  iomux control register for gpio36
 */
#define IO_MUX_GPIO36_REG (DR_REG_IO_MUX_BASE + 0x94)
/** IO_MUX_GPIO36_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO36_MCU_OE    (BIT(0))
#define IO_MUX_GPIO36_MCU_OE_M  (IO_MUX_GPIO36_MCU_OE_V << IO_MUX_GPIO36_MCU_OE_S)
#define IO_MUX_GPIO36_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO36_MCU_OE_S  0
/** IO_MUX_GPIO36_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO36_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO36_SLP_SEL_M  (IO_MUX_GPIO36_SLP_SEL_V << IO_MUX_GPIO36_SLP_SEL_S)
#define IO_MUX_GPIO36_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO36_SLP_SEL_S  1
/** IO_MUX_GPIO36_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO36_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO36_MCU_WPD_M  (IO_MUX_GPIO36_MCU_WPD_V << IO_MUX_GPIO36_MCU_WPD_S)
#define IO_MUX_GPIO36_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO36_MCU_WPD_S  2
/** IO_MUX_GPIO36_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO36_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO36_MCU_WPU_M  (IO_MUX_GPIO36_MCU_WPU_V << IO_MUX_GPIO36_MCU_WPU_S)
#define IO_MUX_GPIO36_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO36_MCU_WPU_S  3
/** IO_MUX_GPIO36_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO36_MCU_IE    (BIT(4))
#define IO_MUX_GPIO36_MCU_IE_M  (IO_MUX_GPIO36_MCU_IE_V << IO_MUX_GPIO36_MCU_IE_S)
#define IO_MUX_GPIO36_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO36_MCU_IE_S  4
/** IO_MUX_GPIO36_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO36_MCU_DRV    0x00000003U
#define IO_MUX_GPIO36_MCU_DRV_M  (IO_MUX_GPIO36_MCU_DRV_V << IO_MUX_GPIO36_MCU_DRV_S)
#define IO_MUX_GPIO36_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO36_MCU_DRV_S  5
/** IO_MUX_GPIO36_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO36_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO36_FUN_WPD_M  (IO_MUX_GPIO36_FUN_WPD_V << IO_MUX_GPIO36_FUN_WPD_S)
#define IO_MUX_GPIO36_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO36_FUN_WPD_S  7
/** IO_MUX_GPIO36_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO36_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO36_FUN_WPU_M  (IO_MUX_GPIO36_FUN_WPU_V << IO_MUX_GPIO36_FUN_WPU_S)
#define IO_MUX_GPIO36_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO36_FUN_WPU_S  8
/** IO_MUX_GPIO36_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO36_FUN_IE    (BIT(9))
#define IO_MUX_GPIO36_FUN_IE_M  (IO_MUX_GPIO36_FUN_IE_V << IO_MUX_GPIO36_FUN_IE_S)
#define IO_MUX_GPIO36_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO36_FUN_IE_S  9
/** IO_MUX_GPIO36_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO36_FUN_DRV    0x00000003U
#define IO_MUX_GPIO36_FUN_DRV_M  (IO_MUX_GPIO36_FUN_DRV_V << IO_MUX_GPIO36_FUN_DRV_S)
#define IO_MUX_GPIO36_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO36_FUN_DRV_S  10
/** IO_MUX_GPIO36_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO36_MCU_SEL    0x00000007U
#define IO_MUX_GPIO36_MCU_SEL_M  (IO_MUX_GPIO36_MCU_SEL_V << IO_MUX_GPIO36_MCU_SEL_S)
#define IO_MUX_GPIO36_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO36_MCU_SEL_S  12
/** IO_MUX_GPIO36_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO36_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO36_FILTER_EN_M  (IO_MUX_GPIO36_FILTER_EN_V << IO_MUX_GPIO36_FILTER_EN_S)
#define IO_MUX_GPIO36_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO36_FILTER_EN_S  15

/** IO_MUX_gpio37_REG register
 *  iomux control register for gpio37
 */
#define IO_MUX_GPIO37_REG (DR_REG_IO_MUX_BASE + 0x98)
/** IO_MUX_GPIO37_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO37_MCU_OE    (BIT(0))
#define IO_MUX_GPIO37_MCU_OE_M  (IO_MUX_GPIO37_MCU_OE_V << IO_MUX_GPIO37_MCU_OE_S)
#define IO_MUX_GPIO37_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO37_MCU_OE_S  0
/** IO_MUX_GPIO37_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO37_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO37_SLP_SEL_M  (IO_MUX_GPIO37_SLP_SEL_V << IO_MUX_GPIO37_SLP_SEL_S)
#define IO_MUX_GPIO37_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO37_SLP_SEL_S  1
/** IO_MUX_GPIO37_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO37_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO37_MCU_WPD_M  (IO_MUX_GPIO37_MCU_WPD_V << IO_MUX_GPIO37_MCU_WPD_S)
#define IO_MUX_GPIO37_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO37_MCU_WPD_S  2
/** IO_MUX_GPIO37_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO37_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO37_MCU_WPU_M  (IO_MUX_GPIO37_MCU_WPU_V << IO_MUX_GPIO37_MCU_WPU_S)
#define IO_MUX_GPIO37_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO37_MCU_WPU_S  3
/** IO_MUX_GPIO37_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO37_MCU_IE    (BIT(4))
#define IO_MUX_GPIO37_MCU_IE_M  (IO_MUX_GPIO37_MCU_IE_V << IO_MUX_GPIO37_MCU_IE_S)
#define IO_MUX_GPIO37_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO37_MCU_IE_S  4
/** IO_MUX_GPIO37_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO37_MCU_DRV    0x00000003U
#define IO_MUX_GPIO37_MCU_DRV_M  (IO_MUX_GPIO37_MCU_DRV_V << IO_MUX_GPIO37_MCU_DRV_S)
#define IO_MUX_GPIO37_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO37_MCU_DRV_S  5
/** IO_MUX_GPIO37_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO37_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO37_FUN_WPD_M  (IO_MUX_GPIO37_FUN_WPD_V << IO_MUX_GPIO37_FUN_WPD_S)
#define IO_MUX_GPIO37_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO37_FUN_WPD_S  7
/** IO_MUX_GPIO37_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO37_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO37_FUN_WPU_M  (IO_MUX_GPIO37_FUN_WPU_V << IO_MUX_GPIO37_FUN_WPU_S)
#define IO_MUX_GPIO37_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO37_FUN_WPU_S  8
/** IO_MUX_GPIO37_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO37_FUN_IE    (BIT(9))
#define IO_MUX_GPIO37_FUN_IE_M  (IO_MUX_GPIO37_FUN_IE_V << IO_MUX_GPIO37_FUN_IE_S)
#define IO_MUX_GPIO37_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO37_FUN_IE_S  9
/** IO_MUX_GPIO37_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO37_FUN_DRV    0x00000003U
#define IO_MUX_GPIO37_FUN_DRV_M  (IO_MUX_GPIO37_FUN_DRV_V << IO_MUX_GPIO37_FUN_DRV_S)
#define IO_MUX_GPIO37_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO37_FUN_DRV_S  10
/** IO_MUX_GPIO37_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO37_MCU_SEL    0x00000007U
#define IO_MUX_GPIO37_MCU_SEL_M  (IO_MUX_GPIO37_MCU_SEL_V << IO_MUX_GPIO37_MCU_SEL_S)
#define IO_MUX_GPIO37_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO37_MCU_SEL_S  12
/** IO_MUX_GPIO37_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO37_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO37_FILTER_EN_M  (IO_MUX_GPIO37_FILTER_EN_V << IO_MUX_GPIO37_FILTER_EN_S)
#define IO_MUX_GPIO37_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO37_FILTER_EN_S  15

/** IO_MUX_gpio38_REG register
 *  iomux control register for gpio38
 */
#define IO_MUX_GPIO38_REG (DR_REG_IO_MUX_BASE + 0x9c)
/** IO_MUX_GPIO38_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO38_MCU_OE    (BIT(0))
#define IO_MUX_GPIO38_MCU_OE_M  (IO_MUX_GPIO38_MCU_OE_V << IO_MUX_GPIO38_MCU_OE_S)
#define IO_MUX_GPIO38_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO38_MCU_OE_S  0
/** IO_MUX_GPIO38_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO38_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO38_SLP_SEL_M  (IO_MUX_GPIO38_SLP_SEL_V << IO_MUX_GPIO38_SLP_SEL_S)
#define IO_MUX_GPIO38_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO38_SLP_SEL_S  1
/** IO_MUX_GPIO38_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO38_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO38_MCU_WPD_M  (IO_MUX_GPIO38_MCU_WPD_V << IO_MUX_GPIO38_MCU_WPD_S)
#define IO_MUX_GPIO38_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO38_MCU_WPD_S  2
/** IO_MUX_GPIO38_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO38_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO38_MCU_WPU_M  (IO_MUX_GPIO38_MCU_WPU_V << IO_MUX_GPIO38_MCU_WPU_S)
#define IO_MUX_GPIO38_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO38_MCU_WPU_S  3
/** IO_MUX_GPIO38_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO38_MCU_IE    (BIT(4))
#define IO_MUX_GPIO38_MCU_IE_M  (IO_MUX_GPIO38_MCU_IE_V << IO_MUX_GPIO38_MCU_IE_S)
#define IO_MUX_GPIO38_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO38_MCU_IE_S  4
/** IO_MUX_GPIO38_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO38_MCU_DRV    0x00000003U
#define IO_MUX_GPIO38_MCU_DRV_M  (IO_MUX_GPIO38_MCU_DRV_V << IO_MUX_GPIO38_MCU_DRV_S)
#define IO_MUX_GPIO38_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO38_MCU_DRV_S  5
/** IO_MUX_GPIO38_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO38_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO38_FUN_WPD_M  (IO_MUX_GPIO38_FUN_WPD_V << IO_MUX_GPIO38_FUN_WPD_S)
#define IO_MUX_GPIO38_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO38_FUN_WPD_S  7
/** IO_MUX_GPIO38_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO38_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO38_FUN_WPU_M  (IO_MUX_GPIO38_FUN_WPU_V << IO_MUX_GPIO38_FUN_WPU_S)
#define IO_MUX_GPIO38_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO38_FUN_WPU_S  8
/** IO_MUX_GPIO38_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO38_FUN_IE    (BIT(9))
#define IO_MUX_GPIO38_FUN_IE_M  (IO_MUX_GPIO38_FUN_IE_V << IO_MUX_GPIO38_FUN_IE_S)
#define IO_MUX_GPIO38_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO38_FUN_IE_S  9
/** IO_MUX_GPIO38_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO38_FUN_DRV    0x00000003U
#define IO_MUX_GPIO38_FUN_DRV_M  (IO_MUX_GPIO38_FUN_DRV_V << IO_MUX_GPIO38_FUN_DRV_S)
#define IO_MUX_GPIO38_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO38_FUN_DRV_S  10
/** IO_MUX_GPIO38_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO38_MCU_SEL    0x00000007U
#define IO_MUX_GPIO38_MCU_SEL_M  (IO_MUX_GPIO38_MCU_SEL_V << IO_MUX_GPIO38_MCU_SEL_S)
#define IO_MUX_GPIO38_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO38_MCU_SEL_S  12
/** IO_MUX_GPIO38_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO38_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO38_FILTER_EN_M  (IO_MUX_GPIO38_FILTER_EN_V << IO_MUX_GPIO38_FILTER_EN_S)
#define IO_MUX_GPIO38_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO38_FILTER_EN_S  15

/** IO_MUX_gpio39_REG register
 *  iomux control register for gpio39
 */
#define IO_MUX_GPIO39_REG (DR_REG_IO_MUX_BASE + 0xa0)
/** IO_MUX_GPIO39_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO39_MCU_OE    (BIT(0))
#define IO_MUX_GPIO39_MCU_OE_M  (IO_MUX_GPIO39_MCU_OE_V << IO_MUX_GPIO39_MCU_OE_S)
#define IO_MUX_GPIO39_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO39_MCU_OE_S  0
/** IO_MUX_GPIO39_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO39_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO39_SLP_SEL_M  (IO_MUX_GPIO39_SLP_SEL_V << IO_MUX_GPIO39_SLP_SEL_S)
#define IO_MUX_GPIO39_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO39_SLP_SEL_S  1
/** IO_MUX_GPIO39_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO39_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO39_MCU_WPD_M  (IO_MUX_GPIO39_MCU_WPD_V << IO_MUX_GPIO39_MCU_WPD_S)
#define IO_MUX_GPIO39_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO39_MCU_WPD_S  2
/** IO_MUX_GPIO39_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO39_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO39_MCU_WPU_M  (IO_MUX_GPIO39_MCU_WPU_V << IO_MUX_GPIO39_MCU_WPU_S)
#define IO_MUX_GPIO39_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO39_MCU_WPU_S  3
/** IO_MUX_GPIO39_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO39_MCU_IE    (BIT(4))
#define IO_MUX_GPIO39_MCU_IE_M  (IO_MUX_GPIO39_MCU_IE_V << IO_MUX_GPIO39_MCU_IE_S)
#define IO_MUX_GPIO39_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO39_MCU_IE_S  4
/** IO_MUX_GPIO39_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO39_MCU_DRV    0x00000003U
#define IO_MUX_GPIO39_MCU_DRV_M  (IO_MUX_GPIO39_MCU_DRV_V << IO_MUX_GPIO39_MCU_DRV_S)
#define IO_MUX_GPIO39_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO39_MCU_DRV_S  5
/** IO_MUX_GPIO39_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO39_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO39_FUN_WPD_M  (IO_MUX_GPIO39_FUN_WPD_V << IO_MUX_GPIO39_FUN_WPD_S)
#define IO_MUX_GPIO39_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO39_FUN_WPD_S  7
/** IO_MUX_GPIO39_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO39_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO39_FUN_WPU_M  (IO_MUX_GPIO39_FUN_WPU_V << IO_MUX_GPIO39_FUN_WPU_S)
#define IO_MUX_GPIO39_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO39_FUN_WPU_S  8
/** IO_MUX_GPIO39_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO39_FUN_IE    (BIT(9))
#define IO_MUX_GPIO39_FUN_IE_M  (IO_MUX_GPIO39_FUN_IE_V << IO_MUX_GPIO39_FUN_IE_S)
#define IO_MUX_GPIO39_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO39_FUN_IE_S  9
/** IO_MUX_GPIO39_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO39_FUN_DRV    0x00000003U
#define IO_MUX_GPIO39_FUN_DRV_M  (IO_MUX_GPIO39_FUN_DRV_V << IO_MUX_GPIO39_FUN_DRV_S)
#define IO_MUX_GPIO39_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO39_FUN_DRV_S  10
/** IO_MUX_GPIO39_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO39_MCU_SEL    0x00000007U
#define IO_MUX_GPIO39_MCU_SEL_M  (IO_MUX_GPIO39_MCU_SEL_V << IO_MUX_GPIO39_MCU_SEL_S)
#define IO_MUX_GPIO39_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO39_MCU_SEL_S  12
/** IO_MUX_GPIO39_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO39_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO39_FILTER_EN_M  (IO_MUX_GPIO39_FILTER_EN_V << IO_MUX_GPIO39_FILTER_EN_S)
#define IO_MUX_GPIO39_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO39_FILTER_EN_S  15

/** IO_MUX_gpio40_REG register
 *  iomux control register for gpio40
 */
#define IO_MUX_GPIO40_REG (DR_REG_IO_MUX_BASE + 0xa4)
/** IO_MUX_GPIO40_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO40_MCU_OE    (BIT(0))
#define IO_MUX_GPIO40_MCU_OE_M  (IO_MUX_GPIO40_MCU_OE_V << IO_MUX_GPIO40_MCU_OE_S)
#define IO_MUX_GPIO40_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO40_MCU_OE_S  0
/** IO_MUX_GPIO40_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO40_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO40_SLP_SEL_M  (IO_MUX_GPIO40_SLP_SEL_V << IO_MUX_GPIO40_SLP_SEL_S)
#define IO_MUX_GPIO40_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO40_SLP_SEL_S  1
/** IO_MUX_GPIO40_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO40_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO40_MCU_WPD_M  (IO_MUX_GPIO40_MCU_WPD_V << IO_MUX_GPIO40_MCU_WPD_S)
#define IO_MUX_GPIO40_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO40_MCU_WPD_S  2
/** IO_MUX_GPIO40_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO40_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO40_MCU_WPU_M  (IO_MUX_GPIO40_MCU_WPU_V << IO_MUX_GPIO40_MCU_WPU_S)
#define IO_MUX_GPIO40_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO40_MCU_WPU_S  3
/** IO_MUX_GPIO40_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO40_MCU_IE    (BIT(4))
#define IO_MUX_GPIO40_MCU_IE_M  (IO_MUX_GPIO40_MCU_IE_V << IO_MUX_GPIO40_MCU_IE_S)
#define IO_MUX_GPIO40_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO40_MCU_IE_S  4
/** IO_MUX_GPIO40_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO40_MCU_DRV    0x00000003U
#define IO_MUX_GPIO40_MCU_DRV_M  (IO_MUX_GPIO40_MCU_DRV_V << IO_MUX_GPIO40_MCU_DRV_S)
#define IO_MUX_GPIO40_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO40_MCU_DRV_S  5
/** IO_MUX_GPIO40_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO40_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO40_FUN_WPD_M  (IO_MUX_GPIO40_FUN_WPD_V << IO_MUX_GPIO40_FUN_WPD_S)
#define IO_MUX_GPIO40_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO40_FUN_WPD_S  7
/** IO_MUX_GPIO40_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO40_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO40_FUN_WPU_M  (IO_MUX_GPIO40_FUN_WPU_V << IO_MUX_GPIO40_FUN_WPU_S)
#define IO_MUX_GPIO40_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO40_FUN_WPU_S  8
/** IO_MUX_GPIO40_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO40_FUN_IE    (BIT(9))
#define IO_MUX_GPIO40_FUN_IE_M  (IO_MUX_GPIO40_FUN_IE_V << IO_MUX_GPIO40_FUN_IE_S)
#define IO_MUX_GPIO40_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO40_FUN_IE_S  9
/** IO_MUX_GPIO40_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO40_FUN_DRV    0x00000003U
#define IO_MUX_GPIO40_FUN_DRV_M  (IO_MUX_GPIO40_FUN_DRV_V << IO_MUX_GPIO40_FUN_DRV_S)
#define IO_MUX_GPIO40_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO40_FUN_DRV_S  10
/** IO_MUX_GPIO40_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO40_MCU_SEL    0x00000007U
#define IO_MUX_GPIO40_MCU_SEL_M  (IO_MUX_GPIO40_MCU_SEL_V << IO_MUX_GPIO40_MCU_SEL_S)
#define IO_MUX_GPIO40_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO40_MCU_SEL_S  12
/** IO_MUX_GPIO40_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO40_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO40_FILTER_EN_M  (IO_MUX_GPIO40_FILTER_EN_V << IO_MUX_GPIO40_FILTER_EN_S)
#define IO_MUX_GPIO40_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO40_FILTER_EN_S  15

/** IO_MUX_gpio41_REG register
 *  iomux control register for gpio41
 */
#define IO_MUX_GPIO41_REG (DR_REG_IO_MUX_BASE + 0xa8)
/** IO_MUX_GPIO41_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO41_MCU_OE    (BIT(0))
#define IO_MUX_GPIO41_MCU_OE_M  (IO_MUX_GPIO41_MCU_OE_V << IO_MUX_GPIO41_MCU_OE_S)
#define IO_MUX_GPIO41_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO41_MCU_OE_S  0
/** IO_MUX_GPIO41_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO41_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO41_SLP_SEL_M  (IO_MUX_GPIO41_SLP_SEL_V << IO_MUX_GPIO41_SLP_SEL_S)
#define IO_MUX_GPIO41_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO41_SLP_SEL_S  1
/** IO_MUX_GPIO41_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO41_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO41_MCU_WPD_M  (IO_MUX_GPIO41_MCU_WPD_V << IO_MUX_GPIO41_MCU_WPD_S)
#define IO_MUX_GPIO41_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO41_MCU_WPD_S  2
/** IO_MUX_GPIO41_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO41_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO41_MCU_WPU_M  (IO_MUX_GPIO41_MCU_WPU_V << IO_MUX_GPIO41_MCU_WPU_S)
#define IO_MUX_GPIO41_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO41_MCU_WPU_S  3
/** IO_MUX_GPIO41_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO41_MCU_IE    (BIT(4))
#define IO_MUX_GPIO41_MCU_IE_M  (IO_MUX_GPIO41_MCU_IE_V << IO_MUX_GPIO41_MCU_IE_S)
#define IO_MUX_GPIO41_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO41_MCU_IE_S  4
/** IO_MUX_GPIO41_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO41_MCU_DRV    0x00000003U
#define IO_MUX_GPIO41_MCU_DRV_M  (IO_MUX_GPIO41_MCU_DRV_V << IO_MUX_GPIO41_MCU_DRV_S)
#define IO_MUX_GPIO41_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO41_MCU_DRV_S  5
/** IO_MUX_GPIO41_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO41_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO41_FUN_WPD_M  (IO_MUX_GPIO41_FUN_WPD_V << IO_MUX_GPIO41_FUN_WPD_S)
#define IO_MUX_GPIO41_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO41_FUN_WPD_S  7
/** IO_MUX_GPIO41_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO41_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO41_FUN_WPU_M  (IO_MUX_GPIO41_FUN_WPU_V << IO_MUX_GPIO41_FUN_WPU_S)
#define IO_MUX_GPIO41_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO41_FUN_WPU_S  8
/** IO_MUX_GPIO41_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO41_FUN_IE    (BIT(9))
#define IO_MUX_GPIO41_FUN_IE_M  (IO_MUX_GPIO41_FUN_IE_V << IO_MUX_GPIO41_FUN_IE_S)
#define IO_MUX_GPIO41_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO41_FUN_IE_S  9
/** IO_MUX_GPIO41_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO41_FUN_DRV    0x00000003U
#define IO_MUX_GPIO41_FUN_DRV_M  (IO_MUX_GPIO41_FUN_DRV_V << IO_MUX_GPIO41_FUN_DRV_S)
#define IO_MUX_GPIO41_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO41_FUN_DRV_S  10
/** IO_MUX_GPIO41_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO41_MCU_SEL    0x00000007U
#define IO_MUX_GPIO41_MCU_SEL_M  (IO_MUX_GPIO41_MCU_SEL_V << IO_MUX_GPIO41_MCU_SEL_S)
#define IO_MUX_GPIO41_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO41_MCU_SEL_S  12
/** IO_MUX_GPIO41_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO41_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO41_FILTER_EN_M  (IO_MUX_GPIO41_FILTER_EN_V << IO_MUX_GPIO41_FILTER_EN_S)
#define IO_MUX_GPIO41_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO41_FILTER_EN_S  15

/** IO_MUX_gpio42_REG register
 *  iomux control register for gpio42
 */
#define IO_MUX_GPIO42_REG (DR_REG_IO_MUX_BASE + 0xac)
/** IO_MUX_GPIO42_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO42_MCU_OE    (BIT(0))
#define IO_MUX_GPIO42_MCU_OE_M  (IO_MUX_GPIO42_MCU_OE_V << IO_MUX_GPIO42_MCU_OE_S)
#define IO_MUX_GPIO42_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO42_MCU_OE_S  0
/** IO_MUX_GPIO42_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO42_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO42_SLP_SEL_M  (IO_MUX_GPIO42_SLP_SEL_V << IO_MUX_GPIO42_SLP_SEL_S)
#define IO_MUX_GPIO42_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO42_SLP_SEL_S  1
/** IO_MUX_GPIO42_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO42_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO42_MCU_WPD_M  (IO_MUX_GPIO42_MCU_WPD_V << IO_MUX_GPIO42_MCU_WPD_S)
#define IO_MUX_GPIO42_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO42_MCU_WPD_S  2
/** IO_MUX_GPIO42_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO42_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO42_MCU_WPU_M  (IO_MUX_GPIO42_MCU_WPU_V << IO_MUX_GPIO42_MCU_WPU_S)
#define IO_MUX_GPIO42_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO42_MCU_WPU_S  3
/** IO_MUX_GPIO42_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO42_MCU_IE    (BIT(4))
#define IO_MUX_GPIO42_MCU_IE_M  (IO_MUX_GPIO42_MCU_IE_V << IO_MUX_GPIO42_MCU_IE_S)
#define IO_MUX_GPIO42_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO42_MCU_IE_S  4
/** IO_MUX_GPIO42_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO42_MCU_DRV    0x00000003U
#define IO_MUX_GPIO42_MCU_DRV_M  (IO_MUX_GPIO42_MCU_DRV_V << IO_MUX_GPIO42_MCU_DRV_S)
#define IO_MUX_GPIO42_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO42_MCU_DRV_S  5
/** IO_MUX_GPIO42_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO42_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO42_FUN_WPD_M  (IO_MUX_GPIO42_FUN_WPD_V << IO_MUX_GPIO42_FUN_WPD_S)
#define IO_MUX_GPIO42_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO42_FUN_WPD_S  7
/** IO_MUX_GPIO42_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO42_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO42_FUN_WPU_M  (IO_MUX_GPIO42_FUN_WPU_V << IO_MUX_GPIO42_FUN_WPU_S)
#define IO_MUX_GPIO42_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO42_FUN_WPU_S  8
/** IO_MUX_GPIO42_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO42_FUN_IE    (BIT(9))
#define IO_MUX_GPIO42_FUN_IE_M  (IO_MUX_GPIO42_FUN_IE_V << IO_MUX_GPIO42_FUN_IE_S)
#define IO_MUX_GPIO42_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO42_FUN_IE_S  9
/** IO_MUX_GPIO42_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO42_FUN_DRV    0x00000003U
#define IO_MUX_GPIO42_FUN_DRV_M  (IO_MUX_GPIO42_FUN_DRV_V << IO_MUX_GPIO42_FUN_DRV_S)
#define IO_MUX_GPIO42_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO42_FUN_DRV_S  10
/** IO_MUX_GPIO42_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO42_MCU_SEL    0x00000007U
#define IO_MUX_GPIO42_MCU_SEL_M  (IO_MUX_GPIO42_MCU_SEL_V << IO_MUX_GPIO42_MCU_SEL_S)
#define IO_MUX_GPIO42_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO42_MCU_SEL_S  12
/** IO_MUX_GPIO42_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO42_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO42_FILTER_EN_M  (IO_MUX_GPIO42_FILTER_EN_V << IO_MUX_GPIO42_FILTER_EN_S)
#define IO_MUX_GPIO42_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO42_FILTER_EN_S  15

/** IO_MUX_gpio43_REG register
 *  iomux control register for gpio43
 */
#define IO_MUX_GPIO43_REG (DR_REG_IO_MUX_BASE + 0xb0)
/** IO_MUX_GPIO43_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO43_MCU_OE    (BIT(0))
#define IO_MUX_GPIO43_MCU_OE_M  (IO_MUX_GPIO43_MCU_OE_V << IO_MUX_GPIO43_MCU_OE_S)
#define IO_MUX_GPIO43_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO43_MCU_OE_S  0
/** IO_MUX_GPIO43_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO43_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO43_SLP_SEL_M  (IO_MUX_GPIO43_SLP_SEL_V << IO_MUX_GPIO43_SLP_SEL_S)
#define IO_MUX_GPIO43_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO43_SLP_SEL_S  1
/** IO_MUX_GPIO43_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO43_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO43_MCU_WPD_M  (IO_MUX_GPIO43_MCU_WPD_V << IO_MUX_GPIO43_MCU_WPD_S)
#define IO_MUX_GPIO43_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO43_MCU_WPD_S  2
/** IO_MUX_GPIO43_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO43_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO43_MCU_WPU_M  (IO_MUX_GPIO43_MCU_WPU_V << IO_MUX_GPIO43_MCU_WPU_S)
#define IO_MUX_GPIO43_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO43_MCU_WPU_S  3
/** IO_MUX_GPIO43_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO43_MCU_IE    (BIT(4))
#define IO_MUX_GPIO43_MCU_IE_M  (IO_MUX_GPIO43_MCU_IE_V << IO_MUX_GPIO43_MCU_IE_S)
#define IO_MUX_GPIO43_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO43_MCU_IE_S  4
/** IO_MUX_GPIO43_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO43_MCU_DRV    0x00000003U
#define IO_MUX_GPIO43_MCU_DRV_M  (IO_MUX_GPIO43_MCU_DRV_V << IO_MUX_GPIO43_MCU_DRV_S)
#define IO_MUX_GPIO43_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO43_MCU_DRV_S  5
/** IO_MUX_GPIO43_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO43_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO43_FUN_WPD_M  (IO_MUX_GPIO43_FUN_WPD_V << IO_MUX_GPIO43_FUN_WPD_S)
#define IO_MUX_GPIO43_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO43_FUN_WPD_S  7
/** IO_MUX_GPIO43_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO43_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO43_FUN_WPU_M  (IO_MUX_GPIO43_FUN_WPU_V << IO_MUX_GPIO43_FUN_WPU_S)
#define IO_MUX_GPIO43_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO43_FUN_WPU_S  8
/** IO_MUX_GPIO43_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO43_FUN_IE    (BIT(9))
#define IO_MUX_GPIO43_FUN_IE_M  (IO_MUX_GPIO43_FUN_IE_V << IO_MUX_GPIO43_FUN_IE_S)
#define IO_MUX_GPIO43_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO43_FUN_IE_S  9
/** IO_MUX_GPIO43_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO43_FUN_DRV    0x00000003U
#define IO_MUX_GPIO43_FUN_DRV_M  (IO_MUX_GPIO43_FUN_DRV_V << IO_MUX_GPIO43_FUN_DRV_S)
#define IO_MUX_GPIO43_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO43_FUN_DRV_S  10
/** IO_MUX_GPIO43_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO43_MCU_SEL    0x00000007U
#define IO_MUX_GPIO43_MCU_SEL_M  (IO_MUX_GPIO43_MCU_SEL_V << IO_MUX_GPIO43_MCU_SEL_S)
#define IO_MUX_GPIO43_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO43_MCU_SEL_S  12
/** IO_MUX_GPIO43_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO43_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO43_FILTER_EN_M  (IO_MUX_GPIO43_FILTER_EN_V << IO_MUX_GPIO43_FILTER_EN_S)
#define IO_MUX_GPIO43_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO43_FILTER_EN_S  15

/** IO_MUX_gpio44_REG register
 *  iomux control register for gpio44
 */
#define IO_MUX_GPIO44_REG (DR_REG_IO_MUX_BASE + 0xb4)
/** IO_MUX_GPIO44_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO44_MCU_OE    (BIT(0))
#define IO_MUX_GPIO44_MCU_OE_M  (IO_MUX_GPIO44_MCU_OE_V << IO_MUX_GPIO44_MCU_OE_S)
#define IO_MUX_GPIO44_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO44_MCU_OE_S  0
/** IO_MUX_GPIO44_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO44_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO44_SLP_SEL_M  (IO_MUX_GPIO44_SLP_SEL_V << IO_MUX_GPIO44_SLP_SEL_S)
#define IO_MUX_GPIO44_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO44_SLP_SEL_S  1
/** IO_MUX_GPIO44_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO44_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO44_MCU_WPD_M  (IO_MUX_GPIO44_MCU_WPD_V << IO_MUX_GPIO44_MCU_WPD_S)
#define IO_MUX_GPIO44_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO44_MCU_WPD_S  2
/** IO_MUX_GPIO44_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO44_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO44_MCU_WPU_M  (IO_MUX_GPIO44_MCU_WPU_V << IO_MUX_GPIO44_MCU_WPU_S)
#define IO_MUX_GPIO44_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO44_MCU_WPU_S  3
/** IO_MUX_GPIO44_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO44_MCU_IE    (BIT(4))
#define IO_MUX_GPIO44_MCU_IE_M  (IO_MUX_GPIO44_MCU_IE_V << IO_MUX_GPIO44_MCU_IE_S)
#define IO_MUX_GPIO44_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO44_MCU_IE_S  4
/** IO_MUX_GPIO44_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO44_MCU_DRV    0x00000003U
#define IO_MUX_GPIO44_MCU_DRV_M  (IO_MUX_GPIO44_MCU_DRV_V << IO_MUX_GPIO44_MCU_DRV_S)
#define IO_MUX_GPIO44_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO44_MCU_DRV_S  5
/** IO_MUX_GPIO44_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO44_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO44_FUN_WPD_M  (IO_MUX_GPIO44_FUN_WPD_V << IO_MUX_GPIO44_FUN_WPD_S)
#define IO_MUX_GPIO44_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO44_FUN_WPD_S  7
/** IO_MUX_GPIO44_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO44_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO44_FUN_WPU_M  (IO_MUX_GPIO44_FUN_WPU_V << IO_MUX_GPIO44_FUN_WPU_S)
#define IO_MUX_GPIO44_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO44_FUN_WPU_S  8
/** IO_MUX_GPIO44_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO44_FUN_IE    (BIT(9))
#define IO_MUX_GPIO44_FUN_IE_M  (IO_MUX_GPIO44_FUN_IE_V << IO_MUX_GPIO44_FUN_IE_S)
#define IO_MUX_GPIO44_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO44_FUN_IE_S  9
/** IO_MUX_GPIO44_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO44_FUN_DRV    0x00000003U
#define IO_MUX_GPIO44_FUN_DRV_M  (IO_MUX_GPIO44_FUN_DRV_V << IO_MUX_GPIO44_FUN_DRV_S)
#define IO_MUX_GPIO44_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO44_FUN_DRV_S  10
/** IO_MUX_GPIO44_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO44_MCU_SEL    0x00000007U
#define IO_MUX_GPIO44_MCU_SEL_M  (IO_MUX_GPIO44_MCU_SEL_V << IO_MUX_GPIO44_MCU_SEL_S)
#define IO_MUX_GPIO44_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO44_MCU_SEL_S  12
/** IO_MUX_GPIO44_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO44_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO44_FILTER_EN_M  (IO_MUX_GPIO44_FILTER_EN_V << IO_MUX_GPIO44_FILTER_EN_S)
#define IO_MUX_GPIO44_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO44_FILTER_EN_S  15

/** IO_MUX_gpio45_REG register
 *  iomux control register for gpio45
 */
#define IO_MUX_GPIO45_REG (DR_REG_IO_MUX_BASE + 0xb8)
/** IO_MUX_GPIO45_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO45_MCU_OE    (BIT(0))
#define IO_MUX_GPIO45_MCU_OE_M  (IO_MUX_GPIO45_MCU_OE_V << IO_MUX_GPIO45_MCU_OE_S)
#define IO_MUX_GPIO45_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO45_MCU_OE_S  0
/** IO_MUX_GPIO45_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO45_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO45_SLP_SEL_M  (IO_MUX_GPIO45_SLP_SEL_V << IO_MUX_GPIO45_SLP_SEL_S)
#define IO_MUX_GPIO45_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO45_SLP_SEL_S  1
/** IO_MUX_GPIO45_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO45_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO45_MCU_WPD_M  (IO_MUX_GPIO45_MCU_WPD_V << IO_MUX_GPIO45_MCU_WPD_S)
#define IO_MUX_GPIO45_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO45_MCU_WPD_S  2
/** IO_MUX_GPIO45_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO45_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO45_MCU_WPU_M  (IO_MUX_GPIO45_MCU_WPU_V << IO_MUX_GPIO45_MCU_WPU_S)
#define IO_MUX_GPIO45_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO45_MCU_WPU_S  3
/** IO_MUX_GPIO45_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO45_MCU_IE    (BIT(4))
#define IO_MUX_GPIO45_MCU_IE_M  (IO_MUX_GPIO45_MCU_IE_V << IO_MUX_GPIO45_MCU_IE_S)
#define IO_MUX_GPIO45_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO45_MCU_IE_S  4
/** IO_MUX_GPIO45_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO45_MCU_DRV    0x00000003U
#define IO_MUX_GPIO45_MCU_DRV_M  (IO_MUX_GPIO45_MCU_DRV_V << IO_MUX_GPIO45_MCU_DRV_S)
#define IO_MUX_GPIO45_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO45_MCU_DRV_S  5
/** IO_MUX_GPIO45_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO45_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO45_FUN_WPD_M  (IO_MUX_GPIO45_FUN_WPD_V << IO_MUX_GPIO45_FUN_WPD_S)
#define IO_MUX_GPIO45_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO45_FUN_WPD_S  7
/** IO_MUX_GPIO45_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO45_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO45_FUN_WPU_M  (IO_MUX_GPIO45_FUN_WPU_V << IO_MUX_GPIO45_FUN_WPU_S)
#define IO_MUX_GPIO45_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO45_FUN_WPU_S  8
/** IO_MUX_GPIO45_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO45_FUN_IE    (BIT(9))
#define IO_MUX_GPIO45_FUN_IE_M  (IO_MUX_GPIO45_FUN_IE_V << IO_MUX_GPIO45_FUN_IE_S)
#define IO_MUX_GPIO45_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO45_FUN_IE_S  9
/** IO_MUX_GPIO45_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO45_FUN_DRV    0x00000003U
#define IO_MUX_GPIO45_FUN_DRV_M  (IO_MUX_GPIO45_FUN_DRV_V << IO_MUX_GPIO45_FUN_DRV_S)
#define IO_MUX_GPIO45_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO45_FUN_DRV_S  10
/** IO_MUX_GPIO45_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO45_MCU_SEL    0x00000007U
#define IO_MUX_GPIO45_MCU_SEL_M  (IO_MUX_GPIO45_MCU_SEL_V << IO_MUX_GPIO45_MCU_SEL_S)
#define IO_MUX_GPIO45_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO45_MCU_SEL_S  12
/** IO_MUX_GPIO45_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO45_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO45_FILTER_EN_M  (IO_MUX_GPIO45_FILTER_EN_V << IO_MUX_GPIO45_FILTER_EN_S)
#define IO_MUX_GPIO45_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO45_FILTER_EN_S  15

/** IO_MUX_gpio46_REG register
 *  iomux control register for gpio46
 */
#define IO_MUX_GPIO46_REG (DR_REG_IO_MUX_BASE + 0xbc)
/** IO_MUX_GPIO46_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO46_MCU_OE    (BIT(0))
#define IO_MUX_GPIO46_MCU_OE_M  (IO_MUX_GPIO46_MCU_OE_V << IO_MUX_GPIO46_MCU_OE_S)
#define IO_MUX_GPIO46_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO46_MCU_OE_S  0
/** IO_MUX_GPIO46_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO46_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO46_SLP_SEL_M  (IO_MUX_GPIO46_SLP_SEL_V << IO_MUX_GPIO46_SLP_SEL_S)
#define IO_MUX_GPIO46_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO46_SLP_SEL_S  1
/** IO_MUX_GPIO46_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO46_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO46_MCU_WPD_M  (IO_MUX_GPIO46_MCU_WPD_V << IO_MUX_GPIO46_MCU_WPD_S)
#define IO_MUX_GPIO46_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO46_MCU_WPD_S  2
/** IO_MUX_GPIO46_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO46_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO46_MCU_WPU_M  (IO_MUX_GPIO46_MCU_WPU_V << IO_MUX_GPIO46_MCU_WPU_S)
#define IO_MUX_GPIO46_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO46_MCU_WPU_S  3
/** IO_MUX_GPIO46_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO46_MCU_IE    (BIT(4))
#define IO_MUX_GPIO46_MCU_IE_M  (IO_MUX_GPIO46_MCU_IE_V << IO_MUX_GPIO46_MCU_IE_S)
#define IO_MUX_GPIO46_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO46_MCU_IE_S  4
/** IO_MUX_GPIO46_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO46_MCU_DRV    0x00000003U
#define IO_MUX_GPIO46_MCU_DRV_M  (IO_MUX_GPIO46_MCU_DRV_V << IO_MUX_GPIO46_MCU_DRV_S)
#define IO_MUX_GPIO46_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO46_MCU_DRV_S  5
/** IO_MUX_GPIO46_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO46_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO46_FUN_WPD_M  (IO_MUX_GPIO46_FUN_WPD_V << IO_MUX_GPIO46_FUN_WPD_S)
#define IO_MUX_GPIO46_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO46_FUN_WPD_S  7
/** IO_MUX_GPIO46_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO46_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO46_FUN_WPU_M  (IO_MUX_GPIO46_FUN_WPU_V << IO_MUX_GPIO46_FUN_WPU_S)
#define IO_MUX_GPIO46_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO46_FUN_WPU_S  8
/** IO_MUX_GPIO46_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO46_FUN_IE    (BIT(9))
#define IO_MUX_GPIO46_FUN_IE_M  (IO_MUX_GPIO46_FUN_IE_V << IO_MUX_GPIO46_FUN_IE_S)
#define IO_MUX_GPIO46_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO46_FUN_IE_S  9
/** IO_MUX_GPIO46_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO46_FUN_DRV    0x00000003U
#define IO_MUX_GPIO46_FUN_DRV_M  (IO_MUX_GPIO46_FUN_DRV_V << IO_MUX_GPIO46_FUN_DRV_S)
#define IO_MUX_GPIO46_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO46_FUN_DRV_S  10
/** IO_MUX_GPIO46_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO46_MCU_SEL    0x00000007U
#define IO_MUX_GPIO46_MCU_SEL_M  (IO_MUX_GPIO46_MCU_SEL_V << IO_MUX_GPIO46_MCU_SEL_S)
#define IO_MUX_GPIO46_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO46_MCU_SEL_S  12
/** IO_MUX_GPIO46_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO46_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO46_FILTER_EN_M  (IO_MUX_GPIO46_FILTER_EN_V << IO_MUX_GPIO46_FILTER_EN_S)
#define IO_MUX_GPIO46_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO46_FILTER_EN_S  15

/** IO_MUX_gpio47_REG register
 *  iomux control register for gpio47
 */
#define IO_MUX_GPIO47_REG (DR_REG_IO_MUX_BASE + 0xc0)
/** IO_MUX_GPIO47_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO47_MCU_OE    (BIT(0))
#define IO_MUX_GPIO47_MCU_OE_M  (IO_MUX_GPIO47_MCU_OE_V << IO_MUX_GPIO47_MCU_OE_S)
#define IO_MUX_GPIO47_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO47_MCU_OE_S  0
/** IO_MUX_GPIO47_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO47_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO47_SLP_SEL_M  (IO_MUX_GPIO47_SLP_SEL_V << IO_MUX_GPIO47_SLP_SEL_S)
#define IO_MUX_GPIO47_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO47_SLP_SEL_S  1
/** IO_MUX_GPIO47_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO47_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO47_MCU_WPD_M  (IO_MUX_GPIO47_MCU_WPD_V << IO_MUX_GPIO47_MCU_WPD_S)
#define IO_MUX_GPIO47_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO47_MCU_WPD_S  2
/** IO_MUX_GPIO47_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO47_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO47_MCU_WPU_M  (IO_MUX_GPIO47_MCU_WPU_V << IO_MUX_GPIO47_MCU_WPU_S)
#define IO_MUX_GPIO47_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO47_MCU_WPU_S  3
/** IO_MUX_GPIO47_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO47_MCU_IE    (BIT(4))
#define IO_MUX_GPIO47_MCU_IE_M  (IO_MUX_GPIO47_MCU_IE_V << IO_MUX_GPIO47_MCU_IE_S)
#define IO_MUX_GPIO47_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO47_MCU_IE_S  4
/** IO_MUX_GPIO47_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO47_MCU_DRV    0x00000003U
#define IO_MUX_GPIO47_MCU_DRV_M  (IO_MUX_GPIO47_MCU_DRV_V << IO_MUX_GPIO47_MCU_DRV_S)
#define IO_MUX_GPIO47_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO47_MCU_DRV_S  5
/** IO_MUX_GPIO47_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO47_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO47_FUN_WPD_M  (IO_MUX_GPIO47_FUN_WPD_V << IO_MUX_GPIO47_FUN_WPD_S)
#define IO_MUX_GPIO47_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO47_FUN_WPD_S  7
/** IO_MUX_GPIO47_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO47_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO47_FUN_WPU_M  (IO_MUX_GPIO47_FUN_WPU_V << IO_MUX_GPIO47_FUN_WPU_S)
#define IO_MUX_GPIO47_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO47_FUN_WPU_S  8
/** IO_MUX_GPIO47_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO47_FUN_IE    (BIT(9))
#define IO_MUX_GPIO47_FUN_IE_M  (IO_MUX_GPIO47_FUN_IE_V << IO_MUX_GPIO47_FUN_IE_S)
#define IO_MUX_GPIO47_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO47_FUN_IE_S  9
/** IO_MUX_GPIO47_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO47_FUN_DRV    0x00000003U
#define IO_MUX_GPIO47_FUN_DRV_M  (IO_MUX_GPIO47_FUN_DRV_V << IO_MUX_GPIO47_FUN_DRV_S)
#define IO_MUX_GPIO47_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO47_FUN_DRV_S  10
/** IO_MUX_GPIO47_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO47_MCU_SEL    0x00000007U
#define IO_MUX_GPIO47_MCU_SEL_M  (IO_MUX_GPIO47_MCU_SEL_V << IO_MUX_GPIO47_MCU_SEL_S)
#define IO_MUX_GPIO47_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO47_MCU_SEL_S  12
/** IO_MUX_GPIO47_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO47_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO47_FILTER_EN_M  (IO_MUX_GPIO47_FILTER_EN_V << IO_MUX_GPIO47_FILTER_EN_S)
#define IO_MUX_GPIO47_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO47_FILTER_EN_S  15

/** IO_MUX_gpio48_REG register
 *  iomux control register for gpio48
 */
#define IO_MUX_GPIO48_REG (DR_REG_IO_MUX_BASE + 0xc4)
/** IO_MUX_GPIO48_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO48_MCU_OE    (BIT(0))
#define IO_MUX_GPIO48_MCU_OE_M  (IO_MUX_GPIO48_MCU_OE_V << IO_MUX_GPIO48_MCU_OE_S)
#define IO_MUX_GPIO48_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO48_MCU_OE_S  0
/** IO_MUX_GPIO48_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO48_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO48_SLP_SEL_M  (IO_MUX_GPIO48_SLP_SEL_V << IO_MUX_GPIO48_SLP_SEL_S)
#define IO_MUX_GPIO48_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO48_SLP_SEL_S  1
/** IO_MUX_GPIO48_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO48_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO48_MCU_WPD_M  (IO_MUX_GPIO48_MCU_WPD_V << IO_MUX_GPIO48_MCU_WPD_S)
#define IO_MUX_GPIO48_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO48_MCU_WPD_S  2
/** IO_MUX_GPIO48_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO48_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO48_MCU_WPU_M  (IO_MUX_GPIO48_MCU_WPU_V << IO_MUX_GPIO48_MCU_WPU_S)
#define IO_MUX_GPIO48_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO48_MCU_WPU_S  3
/** IO_MUX_GPIO48_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO48_MCU_IE    (BIT(4))
#define IO_MUX_GPIO48_MCU_IE_M  (IO_MUX_GPIO48_MCU_IE_V << IO_MUX_GPIO48_MCU_IE_S)
#define IO_MUX_GPIO48_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO48_MCU_IE_S  4
/** IO_MUX_GPIO48_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO48_MCU_DRV    0x00000003U
#define IO_MUX_GPIO48_MCU_DRV_M  (IO_MUX_GPIO48_MCU_DRV_V << IO_MUX_GPIO48_MCU_DRV_S)
#define IO_MUX_GPIO48_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO48_MCU_DRV_S  5
/** IO_MUX_GPIO48_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO48_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO48_FUN_WPD_M  (IO_MUX_GPIO48_FUN_WPD_V << IO_MUX_GPIO48_FUN_WPD_S)
#define IO_MUX_GPIO48_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO48_FUN_WPD_S  7
/** IO_MUX_GPIO48_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO48_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO48_FUN_WPU_M  (IO_MUX_GPIO48_FUN_WPU_V << IO_MUX_GPIO48_FUN_WPU_S)
#define IO_MUX_GPIO48_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO48_FUN_WPU_S  8
/** IO_MUX_GPIO48_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO48_FUN_IE    (BIT(9))
#define IO_MUX_GPIO48_FUN_IE_M  (IO_MUX_GPIO48_FUN_IE_V << IO_MUX_GPIO48_FUN_IE_S)
#define IO_MUX_GPIO48_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO48_FUN_IE_S  9
/** IO_MUX_GPIO48_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO48_FUN_DRV    0x00000003U
#define IO_MUX_GPIO48_FUN_DRV_M  (IO_MUX_GPIO48_FUN_DRV_V << IO_MUX_GPIO48_FUN_DRV_S)
#define IO_MUX_GPIO48_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO48_FUN_DRV_S  10
/** IO_MUX_GPIO48_MCU_SEL : R/W; bitpos: [14:12]; default: 1;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO48_MCU_SEL    0x00000007U
#define IO_MUX_GPIO48_MCU_SEL_M  (IO_MUX_GPIO48_MCU_SEL_V << IO_MUX_GPIO48_MCU_SEL_S)
#define IO_MUX_GPIO48_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO48_MCU_SEL_S  12
/** IO_MUX_GPIO48_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO48_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO48_FILTER_EN_M  (IO_MUX_GPIO48_FILTER_EN_V << IO_MUX_GPIO48_FILTER_EN_S)
#define IO_MUX_GPIO48_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO48_FILTER_EN_S  15

/** IO_MUX_gpio49_REG register
 *  iomux control register for gpio49
 */
#define IO_MUX_GPIO49_REG (DR_REG_IO_MUX_BASE + 0xc8)
/** IO_MUX_GPIO49_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO49_MCU_OE    (BIT(0))
#define IO_MUX_GPIO49_MCU_OE_M  (IO_MUX_GPIO49_MCU_OE_V << IO_MUX_GPIO49_MCU_OE_S)
#define IO_MUX_GPIO49_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO49_MCU_OE_S  0
/** IO_MUX_GPIO49_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO49_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO49_SLP_SEL_M  (IO_MUX_GPIO49_SLP_SEL_V << IO_MUX_GPIO49_SLP_SEL_S)
#define IO_MUX_GPIO49_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO49_SLP_SEL_S  1
/** IO_MUX_GPIO49_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO49_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO49_MCU_WPD_M  (IO_MUX_GPIO49_MCU_WPD_V << IO_MUX_GPIO49_MCU_WPD_S)
#define IO_MUX_GPIO49_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO49_MCU_WPD_S  2
/** IO_MUX_GPIO49_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO49_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO49_MCU_WPU_M  (IO_MUX_GPIO49_MCU_WPU_V << IO_MUX_GPIO49_MCU_WPU_S)
#define IO_MUX_GPIO49_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO49_MCU_WPU_S  3
/** IO_MUX_GPIO49_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO49_MCU_IE    (BIT(4))
#define IO_MUX_GPIO49_MCU_IE_M  (IO_MUX_GPIO49_MCU_IE_V << IO_MUX_GPIO49_MCU_IE_S)
#define IO_MUX_GPIO49_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO49_MCU_IE_S  4
/** IO_MUX_GPIO49_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO49_MCU_DRV    0x00000003U
#define IO_MUX_GPIO49_MCU_DRV_M  (IO_MUX_GPIO49_MCU_DRV_V << IO_MUX_GPIO49_MCU_DRV_S)
#define IO_MUX_GPIO49_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO49_MCU_DRV_S  5
/** IO_MUX_GPIO49_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO49_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO49_FUN_WPD_M  (IO_MUX_GPIO49_FUN_WPD_V << IO_MUX_GPIO49_FUN_WPD_S)
#define IO_MUX_GPIO49_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO49_FUN_WPD_S  7
/** IO_MUX_GPIO49_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO49_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO49_FUN_WPU_M  (IO_MUX_GPIO49_FUN_WPU_V << IO_MUX_GPIO49_FUN_WPU_S)
#define IO_MUX_GPIO49_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO49_FUN_WPU_S  8
/** IO_MUX_GPIO49_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO49_FUN_IE    (BIT(9))
#define IO_MUX_GPIO49_FUN_IE_M  (IO_MUX_GPIO49_FUN_IE_V << IO_MUX_GPIO49_FUN_IE_S)
#define IO_MUX_GPIO49_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO49_FUN_IE_S  9
/** IO_MUX_GPIO49_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO49_FUN_DRV    0x00000003U
#define IO_MUX_GPIO49_FUN_DRV_M  (IO_MUX_GPIO49_FUN_DRV_V << IO_MUX_GPIO49_FUN_DRV_S)
#define IO_MUX_GPIO49_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO49_FUN_DRV_S  10
/** IO_MUX_GPIO49_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO49_MCU_SEL    0x00000007U
#define IO_MUX_GPIO49_MCU_SEL_M  (IO_MUX_GPIO49_MCU_SEL_V << IO_MUX_GPIO49_MCU_SEL_S)
#define IO_MUX_GPIO49_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO49_MCU_SEL_S  12
/** IO_MUX_GPIO49_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO49_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO49_FILTER_EN_M  (IO_MUX_GPIO49_FILTER_EN_V << IO_MUX_GPIO49_FILTER_EN_S)
#define IO_MUX_GPIO49_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO49_FILTER_EN_S  15

/** IO_MUX_gpio50_REG register
 *  iomux control register for gpio50
 */
#define IO_MUX_GPIO50_REG (DR_REG_IO_MUX_BASE + 0xcc)
/** IO_MUX_GPIO50_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO50_MCU_OE    (BIT(0))
#define IO_MUX_GPIO50_MCU_OE_M  (IO_MUX_GPIO50_MCU_OE_V << IO_MUX_GPIO50_MCU_OE_S)
#define IO_MUX_GPIO50_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO50_MCU_OE_S  0
/** IO_MUX_GPIO50_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO50_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO50_SLP_SEL_M  (IO_MUX_GPIO50_SLP_SEL_V << IO_MUX_GPIO50_SLP_SEL_S)
#define IO_MUX_GPIO50_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO50_SLP_SEL_S  1
/** IO_MUX_GPIO50_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO50_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO50_MCU_WPD_M  (IO_MUX_GPIO50_MCU_WPD_V << IO_MUX_GPIO50_MCU_WPD_S)
#define IO_MUX_GPIO50_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO50_MCU_WPD_S  2
/** IO_MUX_GPIO50_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO50_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO50_MCU_WPU_M  (IO_MUX_GPIO50_MCU_WPU_V << IO_MUX_GPIO50_MCU_WPU_S)
#define IO_MUX_GPIO50_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO50_MCU_WPU_S  3
/** IO_MUX_GPIO50_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO50_MCU_IE    (BIT(4))
#define IO_MUX_GPIO50_MCU_IE_M  (IO_MUX_GPIO50_MCU_IE_V << IO_MUX_GPIO50_MCU_IE_S)
#define IO_MUX_GPIO50_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO50_MCU_IE_S  4
/** IO_MUX_GPIO50_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO50_MCU_DRV    0x00000003U
#define IO_MUX_GPIO50_MCU_DRV_M  (IO_MUX_GPIO50_MCU_DRV_V << IO_MUX_GPIO50_MCU_DRV_S)
#define IO_MUX_GPIO50_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO50_MCU_DRV_S  5
/** IO_MUX_GPIO50_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO50_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO50_FUN_WPD_M  (IO_MUX_GPIO50_FUN_WPD_V << IO_MUX_GPIO50_FUN_WPD_S)
#define IO_MUX_GPIO50_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO50_FUN_WPD_S  7
/** IO_MUX_GPIO50_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO50_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO50_FUN_WPU_M  (IO_MUX_GPIO50_FUN_WPU_V << IO_MUX_GPIO50_FUN_WPU_S)
#define IO_MUX_GPIO50_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO50_FUN_WPU_S  8
/** IO_MUX_GPIO50_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO50_FUN_IE    (BIT(9))
#define IO_MUX_GPIO50_FUN_IE_M  (IO_MUX_GPIO50_FUN_IE_V << IO_MUX_GPIO50_FUN_IE_S)
#define IO_MUX_GPIO50_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO50_FUN_IE_S  9
/** IO_MUX_GPIO50_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO50_FUN_DRV    0x00000003U
#define IO_MUX_GPIO50_FUN_DRV_M  (IO_MUX_GPIO50_FUN_DRV_V << IO_MUX_GPIO50_FUN_DRV_S)
#define IO_MUX_GPIO50_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO50_FUN_DRV_S  10
/** IO_MUX_GPIO50_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO50_MCU_SEL    0x00000007U
#define IO_MUX_GPIO50_MCU_SEL_M  (IO_MUX_GPIO50_MCU_SEL_V << IO_MUX_GPIO50_MCU_SEL_S)
#define IO_MUX_GPIO50_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO50_MCU_SEL_S  12
/** IO_MUX_GPIO50_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO50_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO50_FILTER_EN_M  (IO_MUX_GPIO50_FILTER_EN_V << IO_MUX_GPIO50_FILTER_EN_S)
#define IO_MUX_GPIO50_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO50_FILTER_EN_S  15

/** IO_MUX_gpio51_REG register
 *  iomux control register for gpio51
 */
#define IO_MUX_GPIO51_REG (DR_REG_IO_MUX_BASE + 0xd0)
/** IO_MUX_GPIO51_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO51_MCU_OE    (BIT(0))
#define IO_MUX_GPIO51_MCU_OE_M  (IO_MUX_GPIO51_MCU_OE_V << IO_MUX_GPIO51_MCU_OE_S)
#define IO_MUX_GPIO51_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO51_MCU_OE_S  0
/** IO_MUX_GPIO51_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO51_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO51_SLP_SEL_M  (IO_MUX_GPIO51_SLP_SEL_V << IO_MUX_GPIO51_SLP_SEL_S)
#define IO_MUX_GPIO51_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO51_SLP_SEL_S  1
/** IO_MUX_GPIO51_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO51_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO51_MCU_WPD_M  (IO_MUX_GPIO51_MCU_WPD_V << IO_MUX_GPIO51_MCU_WPD_S)
#define IO_MUX_GPIO51_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO51_MCU_WPD_S  2
/** IO_MUX_GPIO51_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO51_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO51_MCU_WPU_M  (IO_MUX_GPIO51_MCU_WPU_V << IO_MUX_GPIO51_MCU_WPU_S)
#define IO_MUX_GPIO51_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO51_MCU_WPU_S  3
/** IO_MUX_GPIO51_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO51_MCU_IE    (BIT(4))
#define IO_MUX_GPIO51_MCU_IE_M  (IO_MUX_GPIO51_MCU_IE_V << IO_MUX_GPIO51_MCU_IE_S)
#define IO_MUX_GPIO51_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO51_MCU_IE_S  4
/** IO_MUX_GPIO51_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO51_MCU_DRV    0x00000003U
#define IO_MUX_GPIO51_MCU_DRV_M  (IO_MUX_GPIO51_MCU_DRV_V << IO_MUX_GPIO51_MCU_DRV_S)
#define IO_MUX_GPIO51_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO51_MCU_DRV_S  5
/** IO_MUX_GPIO51_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO51_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO51_FUN_WPD_M  (IO_MUX_GPIO51_FUN_WPD_V << IO_MUX_GPIO51_FUN_WPD_S)
#define IO_MUX_GPIO51_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO51_FUN_WPD_S  7
/** IO_MUX_GPIO51_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO51_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO51_FUN_WPU_M  (IO_MUX_GPIO51_FUN_WPU_V << IO_MUX_GPIO51_FUN_WPU_S)
#define IO_MUX_GPIO51_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO51_FUN_WPU_S  8
/** IO_MUX_GPIO51_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO51_FUN_IE    (BIT(9))
#define IO_MUX_GPIO51_FUN_IE_M  (IO_MUX_GPIO51_FUN_IE_V << IO_MUX_GPIO51_FUN_IE_S)
#define IO_MUX_GPIO51_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO51_FUN_IE_S  9
/** IO_MUX_GPIO51_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO51_FUN_DRV    0x00000003U
#define IO_MUX_GPIO51_FUN_DRV_M  (IO_MUX_GPIO51_FUN_DRV_V << IO_MUX_GPIO51_FUN_DRV_S)
#define IO_MUX_GPIO51_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO51_FUN_DRV_S  10
/** IO_MUX_GPIO51_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO51_MCU_SEL    0x00000007U
#define IO_MUX_GPIO51_MCU_SEL_M  (IO_MUX_GPIO51_MCU_SEL_V << IO_MUX_GPIO51_MCU_SEL_S)
#define IO_MUX_GPIO51_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO51_MCU_SEL_S  12
/** IO_MUX_GPIO51_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO51_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO51_FILTER_EN_M  (IO_MUX_GPIO51_FILTER_EN_V << IO_MUX_GPIO51_FILTER_EN_S)
#define IO_MUX_GPIO51_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO51_FILTER_EN_S  15

/** IO_MUX_gpio52_REG register
 *  iomux control register for gpio52
 */
#define IO_MUX_GPIO52_REG (DR_REG_IO_MUX_BASE + 0xd4)
/** IO_MUX_GPIO52_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO52_MCU_OE    (BIT(0))
#define IO_MUX_GPIO52_MCU_OE_M  (IO_MUX_GPIO52_MCU_OE_V << IO_MUX_GPIO52_MCU_OE_S)
#define IO_MUX_GPIO52_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO52_MCU_OE_S  0
/** IO_MUX_GPIO52_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO52_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO52_SLP_SEL_M  (IO_MUX_GPIO52_SLP_SEL_V << IO_MUX_GPIO52_SLP_SEL_S)
#define IO_MUX_GPIO52_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO52_SLP_SEL_S  1
/** IO_MUX_GPIO52_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO52_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO52_MCU_WPD_M  (IO_MUX_GPIO52_MCU_WPD_V << IO_MUX_GPIO52_MCU_WPD_S)
#define IO_MUX_GPIO52_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO52_MCU_WPD_S  2
/** IO_MUX_GPIO52_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO52_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO52_MCU_WPU_M  (IO_MUX_GPIO52_MCU_WPU_V << IO_MUX_GPIO52_MCU_WPU_S)
#define IO_MUX_GPIO52_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO52_MCU_WPU_S  3
/** IO_MUX_GPIO52_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO52_MCU_IE    (BIT(4))
#define IO_MUX_GPIO52_MCU_IE_M  (IO_MUX_GPIO52_MCU_IE_V << IO_MUX_GPIO52_MCU_IE_S)
#define IO_MUX_GPIO52_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO52_MCU_IE_S  4
/** IO_MUX_GPIO52_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO52_MCU_DRV    0x00000003U
#define IO_MUX_GPIO52_MCU_DRV_M  (IO_MUX_GPIO52_MCU_DRV_V << IO_MUX_GPIO52_MCU_DRV_S)
#define IO_MUX_GPIO52_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO52_MCU_DRV_S  5
/** IO_MUX_GPIO52_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO52_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO52_FUN_WPD_M  (IO_MUX_GPIO52_FUN_WPD_V << IO_MUX_GPIO52_FUN_WPD_S)
#define IO_MUX_GPIO52_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO52_FUN_WPD_S  7
/** IO_MUX_GPIO52_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO52_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO52_FUN_WPU_M  (IO_MUX_GPIO52_FUN_WPU_V << IO_MUX_GPIO52_FUN_WPU_S)
#define IO_MUX_GPIO52_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO52_FUN_WPU_S  8
/** IO_MUX_GPIO52_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO52_FUN_IE    (BIT(9))
#define IO_MUX_GPIO52_FUN_IE_M  (IO_MUX_GPIO52_FUN_IE_V << IO_MUX_GPIO52_FUN_IE_S)
#define IO_MUX_GPIO52_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO52_FUN_IE_S  9
/** IO_MUX_GPIO52_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO52_FUN_DRV    0x00000003U
#define IO_MUX_GPIO52_FUN_DRV_M  (IO_MUX_GPIO52_FUN_DRV_V << IO_MUX_GPIO52_FUN_DRV_S)
#define IO_MUX_GPIO52_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO52_FUN_DRV_S  10
/** IO_MUX_GPIO52_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO52_MCU_SEL    0x00000007U
#define IO_MUX_GPIO52_MCU_SEL_M  (IO_MUX_GPIO52_MCU_SEL_V << IO_MUX_GPIO52_MCU_SEL_S)
#define IO_MUX_GPIO52_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO52_MCU_SEL_S  12
/** IO_MUX_GPIO52_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO52_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO52_FILTER_EN_M  (IO_MUX_GPIO52_FILTER_EN_V << IO_MUX_GPIO52_FILTER_EN_S)
#define IO_MUX_GPIO52_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO52_FILTER_EN_S  15

/** IO_MUX_gpio53_REG register
 *  iomux control register for gpio53
 */
#define IO_MUX_GPIO53_REG (DR_REG_IO_MUX_BASE + 0xd8)
/** IO_MUX_GPIO53_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO53_MCU_OE    (BIT(0))
#define IO_MUX_GPIO53_MCU_OE_M  (IO_MUX_GPIO53_MCU_OE_V << IO_MUX_GPIO53_MCU_OE_S)
#define IO_MUX_GPIO53_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO53_MCU_OE_S  0
/** IO_MUX_GPIO53_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO53_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO53_SLP_SEL_M  (IO_MUX_GPIO53_SLP_SEL_V << IO_MUX_GPIO53_SLP_SEL_S)
#define IO_MUX_GPIO53_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO53_SLP_SEL_S  1
/** IO_MUX_GPIO53_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO53_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO53_MCU_WPD_M  (IO_MUX_GPIO53_MCU_WPD_V << IO_MUX_GPIO53_MCU_WPD_S)
#define IO_MUX_GPIO53_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO53_MCU_WPD_S  2
/** IO_MUX_GPIO53_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO53_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO53_MCU_WPU_M  (IO_MUX_GPIO53_MCU_WPU_V << IO_MUX_GPIO53_MCU_WPU_S)
#define IO_MUX_GPIO53_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO53_MCU_WPU_S  3
/** IO_MUX_GPIO53_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO53_MCU_IE    (BIT(4))
#define IO_MUX_GPIO53_MCU_IE_M  (IO_MUX_GPIO53_MCU_IE_V << IO_MUX_GPIO53_MCU_IE_S)
#define IO_MUX_GPIO53_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO53_MCU_IE_S  4
/** IO_MUX_GPIO53_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO53_MCU_DRV    0x00000003U
#define IO_MUX_GPIO53_MCU_DRV_M  (IO_MUX_GPIO53_MCU_DRV_V << IO_MUX_GPIO53_MCU_DRV_S)
#define IO_MUX_GPIO53_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO53_MCU_DRV_S  5
/** IO_MUX_GPIO53_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO53_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO53_FUN_WPD_M  (IO_MUX_GPIO53_FUN_WPD_V << IO_MUX_GPIO53_FUN_WPD_S)
#define IO_MUX_GPIO53_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO53_FUN_WPD_S  7
/** IO_MUX_GPIO53_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO53_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO53_FUN_WPU_M  (IO_MUX_GPIO53_FUN_WPU_V << IO_MUX_GPIO53_FUN_WPU_S)
#define IO_MUX_GPIO53_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO53_FUN_WPU_S  8
/** IO_MUX_GPIO53_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO53_FUN_IE    (BIT(9))
#define IO_MUX_GPIO53_FUN_IE_M  (IO_MUX_GPIO53_FUN_IE_V << IO_MUX_GPIO53_FUN_IE_S)
#define IO_MUX_GPIO53_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO53_FUN_IE_S  9
/** IO_MUX_GPIO53_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO53_FUN_DRV    0x00000003U
#define IO_MUX_GPIO53_FUN_DRV_M  (IO_MUX_GPIO53_FUN_DRV_V << IO_MUX_GPIO53_FUN_DRV_S)
#define IO_MUX_GPIO53_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO53_FUN_DRV_S  10
/** IO_MUX_GPIO53_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO53_MCU_SEL    0x00000007U
#define IO_MUX_GPIO53_MCU_SEL_M  (IO_MUX_GPIO53_MCU_SEL_V << IO_MUX_GPIO53_MCU_SEL_S)
#define IO_MUX_GPIO53_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO53_MCU_SEL_S  12
/** IO_MUX_GPIO53_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO53_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO53_FILTER_EN_M  (IO_MUX_GPIO53_FILTER_EN_V << IO_MUX_GPIO53_FILTER_EN_S)
#define IO_MUX_GPIO53_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO53_FILTER_EN_S  15

/** IO_MUX_gpio54_REG register
 *  iomux control register for gpio54
 */
#define IO_MUX_GPIO54_REG (DR_REG_IO_MUX_BASE + 0xdc)
/** IO_MUX_GPIO54_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO54_MCU_OE    (BIT(0))
#define IO_MUX_GPIO54_MCU_OE_M  (IO_MUX_GPIO54_MCU_OE_V << IO_MUX_GPIO54_MCU_OE_S)
#define IO_MUX_GPIO54_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO54_MCU_OE_S  0
/** IO_MUX_GPIO54_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO54_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO54_SLP_SEL_M  (IO_MUX_GPIO54_SLP_SEL_V << IO_MUX_GPIO54_SLP_SEL_S)
#define IO_MUX_GPIO54_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO54_SLP_SEL_S  1
/** IO_MUX_GPIO54_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO54_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO54_MCU_WPD_M  (IO_MUX_GPIO54_MCU_WPD_V << IO_MUX_GPIO54_MCU_WPD_S)
#define IO_MUX_GPIO54_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO54_MCU_WPD_S  2
/** IO_MUX_GPIO54_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO54_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO54_MCU_WPU_M  (IO_MUX_GPIO54_MCU_WPU_V << IO_MUX_GPIO54_MCU_WPU_S)
#define IO_MUX_GPIO54_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO54_MCU_WPU_S  3
/** IO_MUX_GPIO54_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO54_MCU_IE    (BIT(4))
#define IO_MUX_GPIO54_MCU_IE_M  (IO_MUX_GPIO54_MCU_IE_V << IO_MUX_GPIO54_MCU_IE_S)
#define IO_MUX_GPIO54_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO54_MCU_IE_S  4
/** IO_MUX_GPIO54_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO54_MCU_DRV    0x00000003U
#define IO_MUX_GPIO54_MCU_DRV_M  (IO_MUX_GPIO54_MCU_DRV_V << IO_MUX_GPIO54_MCU_DRV_S)
#define IO_MUX_GPIO54_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO54_MCU_DRV_S  5
/** IO_MUX_GPIO54_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO54_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO54_FUN_WPD_M  (IO_MUX_GPIO54_FUN_WPD_V << IO_MUX_GPIO54_FUN_WPD_S)
#define IO_MUX_GPIO54_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO54_FUN_WPD_S  7
/** IO_MUX_GPIO54_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO54_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO54_FUN_WPU_M  (IO_MUX_GPIO54_FUN_WPU_V << IO_MUX_GPIO54_FUN_WPU_S)
#define IO_MUX_GPIO54_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO54_FUN_WPU_S  8
/** IO_MUX_GPIO54_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO54_FUN_IE    (BIT(9))
#define IO_MUX_GPIO54_FUN_IE_M  (IO_MUX_GPIO54_FUN_IE_V << IO_MUX_GPIO54_FUN_IE_S)
#define IO_MUX_GPIO54_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO54_FUN_IE_S  9
/** IO_MUX_GPIO54_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO54_FUN_DRV    0x00000003U
#define IO_MUX_GPIO54_FUN_DRV_M  (IO_MUX_GPIO54_FUN_DRV_V << IO_MUX_GPIO54_FUN_DRV_S)
#define IO_MUX_GPIO54_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO54_FUN_DRV_S  10
/** IO_MUX_GPIO54_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO54_MCU_SEL    0x00000007U
#define IO_MUX_GPIO54_MCU_SEL_M  (IO_MUX_GPIO54_MCU_SEL_V << IO_MUX_GPIO54_MCU_SEL_S)
#define IO_MUX_GPIO54_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO54_MCU_SEL_S  12
/** IO_MUX_GPIO54_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO54_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO54_FILTER_EN_M  (IO_MUX_GPIO54_FILTER_EN_V << IO_MUX_GPIO54_FILTER_EN_S)
#define IO_MUX_GPIO54_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO54_FILTER_EN_S  15

/** IO_MUX_gpio55_REG register
 *  iomux control register for gpio55
 */
#define IO_MUX_GPIO55_REG (DR_REG_IO_MUX_BASE + 0xe0)
/** IO_MUX_GPIO55_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO55_MCU_OE    (BIT(0))
#define IO_MUX_GPIO55_MCU_OE_M  (IO_MUX_GPIO55_MCU_OE_V << IO_MUX_GPIO55_MCU_OE_S)
#define IO_MUX_GPIO55_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO55_MCU_OE_S  0
/** IO_MUX_GPIO55_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO55_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO55_SLP_SEL_M  (IO_MUX_GPIO55_SLP_SEL_V << IO_MUX_GPIO55_SLP_SEL_S)
#define IO_MUX_GPIO55_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO55_SLP_SEL_S  1
/** IO_MUX_GPIO55_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO55_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO55_MCU_WPD_M  (IO_MUX_GPIO55_MCU_WPD_V << IO_MUX_GPIO55_MCU_WPD_S)
#define IO_MUX_GPIO55_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO55_MCU_WPD_S  2
/** IO_MUX_GPIO55_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO55_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO55_MCU_WPU_M  (IO_MUX_GPIO55_MCU_WPU_V << IO_MUX_GPIO55_MCU_WPU_S)
#define IO_MUX_GPIO55_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO55_MCU_WPU_S  3
/** IO_MUX_GPIO55_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO55_MCU_IE    (BIT(4))
#define IO_MUX_GPIO55_MCU_IE_M  (IO_MUX_GPIO55_MCU_IE_V << IO_MUX_GPIO55_MCU_IE_S)
#define IO_MUX_GPIO55_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO55_MCU_IE_S  4
/** IO_MUX_GPIO55_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO55_MCU_DRV    0x00000003U
#define IO_MUX_GPIO55_MCU_DRV_M  (IO_MUX_GPIO55_MCU_DRV_V << IO_MUX_GPIO55_MCU_DRV_S)
#define IO_MUX_GPIO55_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO55_MCU_DRV_S  5
/** IO_MUX_GPIO55_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO55_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO55_FUN_WPD_M  (IO_MUX_GPIO55_FUN_WPD_V << IO_MUX_GPIO55_FUN_WPD_S)
#define IO_MUX_GPIO55_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO55_FUN_WPD_S  7
/** IO_MUX_GPIO55_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO55_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO55_FUN_WPU_M  (IO_MUX_GPIO55_FUN_WPU_V << IO_MUX_GPIO55_FUN_WPU_S)
#define IO_MUX_GPIO55_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO55_FUN_WPU_S  8
/** IO_MUX_GPIO55_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO55_FUN_IE    (BIT(9))
#define IO_MUX_GPIO55_FUN_IE_M  (IO_MUX_GPIO55_FUN_IE_V << IO_MUX_GPIO55_FUN_IE_S)
#define IO_MUX_GPIO55_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO55_FUN_IE_S  9
/** IO_MUX_GPIO55_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO55_FUN_DRV    0x00000003U
#define IO_MUX_GPIO55_FUN_DRV_M  (IO_MUX_GPIO55_FUN_DRV_V << IO_MUX_GPIO55_FUN_DRV_S)
#define IO_MUX_GPIO55_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO55_FUN_DRV_S  10
/** IO_MUX_GPIO55_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO55_MCU_SEL    0x00000007U
#define IO_MUX_GPIO55_MCU_SEL_M  (IO_MUX_GPIO55_MCU_SEL_V << IO_MUX_GPIO55_MCU_SEL_S)
#define IO_MUX_GPIO55_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO55_MCU_SEL_S  12
/** IO_MUX_GPIO55_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO55_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO55_FILTER_EN_M  (IO_MUX_GPIO55_FILTER_EN_V << IO_MUX_GPIO55_FILTER_EN_S)
#define IO_MUX_GPIO55_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO55_FILTER_EN_S  15

/** IO_MUX_gpio56_REG register
 *  iomux control register for gpio56
 */
#define IO_MUX_GPIO56_REG (DR_REG_IO_MUX_BASE + 0xe4)
/** IO_MUX_GPIO56_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO56_MCU_OE    (BIT(0))
#define IO_MUX_GPIO56_MCU_OE_M  (IO_MUX_GPIO56_MCU_OE_V << IO_MUX_GPIO56_MCU_OE_S)
#define IO_MUX_GPIO56_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO56_MCU_OE_S  0
/** IO_MUX_GPIO56_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO56_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO56_SLP_SEL_M  (IO_MUX_GPIO56_SLP_SEL_V << IO_MUX_GPIO56_SLP_SEL_S)
#define IO_MUX_GPIO56_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO56_SLP_SEL_S  1
/** IO_MUX_GPIO56_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO56_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO56_MCU_WPD_M  (IO_MUX_GPIO56_MCU_WPD_V << IO_MUX_GPIO56_MCU_WPD_S)
#define IO_MUX_GPIO56_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO56_MCU_WPD_S  2
/** IO_MUX_GPIO56_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO56_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO56_MCU_WPU_M  (IO_MUX_GPIO56_MCU_WPU_V << IO_MUX_GPIO56_MCU_WPU_S)
#define IO_MUX_GPIO56_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO56_MCU_WPU_S  3
/** IO_MUX_GPIO56_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO56_MCU_IE    (BIT(4))
#define IO_MUX_GPIO56_MCU_IE_M  (IO_MUX_GPIO56_MCU_IE_V << IO_MUX_GPIO56_MCU_IE_S)
#define IO_MUX_GPIO56_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO56_MCU_IE_S  4
/** IO_MUX_GPIO56_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strength on sleep mode
 */
#define IO_MUX_GPIO56_MCU_DRV    0x00000003U
#define IO_MUX_GPIO56_MCU_DRV_M  (IO_MUX_GPIO56_MCU_DRV_V << IO_MUX_GPIO56_MCU_DRV_S)
#define IO_MUX_GPIO56_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO56_MCU_DRV_S  5
/** IO_MUX_GPIO56_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO56_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO56_FUN_WPD_M  (IO_MUX_GPIO56_FUN_WPD_V << IO_MUX_GPIO56_FUN_WPD_S)
#define IO_MUX_GPIO56_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO56_FUN_WPD_S  7
/** IO_MUX_GPIO56_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO56_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO56_FUN_WPU_M  (IO_MUX_GPIO56_FUN_WPU_V << IO_MUX_GPIO56_FUN_WPU_S)
#define IO_MUX_GPIO56_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO56_FUN_WPU_S  8
/** IO_MUX_GPIO56_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO56_FUN_IE    (BIT(9))
#define IO_MUX_GPIO56_FUN_IE_M  (IO_MUX_GPIO56_FUN_IE_V << IO_MUX_GPIO56_FUN_IE_S)
#define IO_MUX_GPIO56_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO56_FUN_IE_S  9
/** IO_MUX_GPIO56_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO56_FUN_DRV    0x00000003U
#define IO_MUX_GPIO56_FUN_DRV_M  (IO_MUX_GPIO56_FUN_DRV_V << IO_MUX_GPIO56_FUN_DRV_S)
#define IO_MUX_GPIO56_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO56_FUN_DRV_S  10
/** IO_MUX_GPIO56_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO56_MCU_SEL    0x00000007U
#define IO_MUX_GPIO56_MCU_SEL_M  (IO_MUX_GPIO56_MCU_SEL_V << IO_MUX_GPIO56_MCU_SEL_S)
#define IO_MUX_GPIO56_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO56_MCU_SEL_S  12
/** IO_MUX_GPIO56_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO56_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO56_FILTER_EN_M  (IO_MUX_GPIO56_FILTER_EN_V << IO_MUX_GPIO56_FILTER_EN_S)
#define IO_MUX_GPIO56_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO56_FILTER_EN_S  15

/** IO_MUX_DATE_REG register
 *  iomux version
 */
#define IO_MUX_DATE_REG (DR_REG_IO_MUX_BASE + 0x104)
/** IO_MUX_DATE : R/W; bitpos: [27:0]; default: 2101794;
 *  csv date
 */
#define IO_MUX_DATE    0x0FFFFFFFU
#define IO_MUX_DATE_M  (IO_MUX_DATE_V << IO_MUX_DATE_S)
#define IO_MUX_DATE_V  0x0FFFFFFFU
#define IO_MUX_DATE_S  0

#ifdef __cplusplus
}
#endif
