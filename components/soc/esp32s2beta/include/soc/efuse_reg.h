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
#ifndef _SOC_EFUSE_REG_H_
#define _SOC_EFUSE_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"
#define EFUSE_PGM_DATA0_REG          (DR_REG_EFUSE_BASE + 0x000)
/* EFUSE_WR_DIS : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define EFUSE_WR_DIS  0xFFFFFFFF
#define EFUSE_WR_DIS_M  ((EFUSE_WR_DIS_V)<<(EFUSE_WR_DIS_S))
#define EFUSE_WR_DIS_V  0xFFFFFFFF
#define EFUSE_WR_DIS_S  0

#define EFUSE_PGM_DATA1_REG          (DR_REG_EFUSE_BASE + 0x004)
/* EFUSE_SDIO_DREFH : R/W ;bitpos:[31:30] ;default: 2'b0 ; */
/*description: */
#define EFUSE_SDIO_DREFH  0x00000003
#define EFUSE_SDIO_DREFH_M  ((EFUSE_SDIO_DREFH_V)<<(EFUSE_SDIO_DREFH_S))
#define EFUSE_SDIO_DREFH_V  0x3
#define EFUSE_SDIO_DREFH_S  30
/* EFUSE_SDIO_MODECURLIM : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SDIO_MODECURLIM  (BIT(29))
#define EFUSE_SDIO_MODECURLIM_M  (BIT(29))
#define EFUSE_SDIO_MODECURLIM_V  0x1
#define EFUSE_SDIO_MODECURLIM_S  29
/* EFUSE_USB_DRES : R/W ;bitpos:[28:27] ;default: 2'b0 ; */
/*description: */
#define EFUSE_USB_DRES  0x00000003
#define EFUSE_USB_DRES_M  ((EFUSE_USB_DRES_V)<<(EFUSE_USB_DRES_S))
#define EFUSE_USB_DRES_V  0x3
#define EFUSE_USB_DRES_S  27
/* EFUSE_USB_FORCE_B : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define EFUSE_USB_FORCE_B  (BIT(26))
#define EFUSE_USB_FORCE_B_M  (BIT(26))
#define EFUSE_USB_FORCE_B_V  0x1
#define EFUSE_USB_FORCE_B_S  26
/* EFUSE_USB_EXT_PHY_ENABLE : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define EFUSE_USB_EXT_PHY_ENABLE  (BIT(25))
#define EFUSE_USB_EXT_PHY_ENABLE_M  (BIT(25))
#define EFUSE_USB_EXT_PHY_ENABLE_V  0x1
#define EFUSE_USB_EXT_PHY_ENABLE_S  25
/* EFUSE_USB_EXCHG_PINS : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define EFUSE_USB_EXCHG_PINS  (BIT(24))
#define EFUSE_USB_EXCHG_PINS_M  (BIT(24))
#define EFUSE_USB_EXCHG_PINS_V  0x1
#define EFUSE_USB_EXCHG_PINS_S  24
/* EFUSE_USB_DREFL : R/W ;bitpos:[23:22] ;default: 2'b0 ; */
/*description: */
#define EFUSE_USB_DREFL  0x00000003
#define EFUSE_USB_DREFL_M  ((EFUSE_USB_DREFL_V)<<(EFUSE_USB_DREFL_S))
#define EFUSE_USB_DREFL_V  0x3
#define EFUSE_USB_DREFL_S  22
/* EFUSE_USB_DREFH : R/W ;bitpos:[21:20] ;default: 2'b0 ; */
/*description: */
#define EFUSE_USB_DREFH  0x00000003
#define EFUSE_USB_DREFH_M  ((EFUSE_USB_DREFH_V)<<(EFUSE_USB_DREFH_S))
#define EFUSE_USB_DREFH_V  0x3
#define EFUSE_USB_DREFH_S  20
/* EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT  (BIT(19))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_M  (BIT(19))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_V  0x1
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_S  19
/* EFUSE_HARD_DIS_JTAG : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define EFUSE_HARD_DIS_JTAG  (BIT(18))
#define EFUSE_HARD_DIS_JTAG_M  (BIT(18))
#define EFUSE_HARD_DIS_JTAG_V  0x1
#define EFUSE_HARD_DIS_JTAG_S  18
/* EFUSE_SOFT_DIS_JTAG : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SOFT_DIS_JTAG  (BIT(17))
#define EFUSE_SOFT_DIS_JTAG_M  (BIT(17))
#define EFUSE_SOFT_DIS_JTAG_V  0x1
#define EFUSE_SOFT_DIS_JTAG_S  17
/* EFUSE_DIS_EFUSE_ATE_WR : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_EFUSE_ATE_WR  (BIT(16))
#define EFUSE_DIS_EFUSE_ATE_WR_M  (BIT(16))
#define EFUSE_DIS_EFUSE_ATE_WR_V  0x1
#define EFUSE_DIS_EFUSE_ATE_WR_S  16
/* EFUSE_DIS_SDIO_ACCESS : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_SDIO_ACCESS  (BIT(15))
#define EFUSE_DIS_SDIO_ACCESS_M  (BIT(15))
#define EFUSE_DIS_SDIO_ACCESS_V  0x1
#define EFUSE_DIS_SDIO_ACCESS_S  15
/* EFUSE_DIS_CAN : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_CAN  (BIT(14))
#define EFUSE_DIS_CAN_M  (BIT(14))
#define EFUSE_DIS_CAN_V  0x1
#define EFUSE_DIS_CAN_S  14
/* EFUSE_DIS_USB : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_USB  (BIT(13))
#define EFUSE_DIS_USB_M  (BIT(13))
#define EFUSE_DIS_USB_V  0x1
#define EFUSE_DIS_USB_S  13
/* EFUSE_DIS_BT : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_BT  (BIT(12))
#define EFUSE_DIS_BT_M  (BIT(12))
#define EFUSE_DIS_BT_V  0x1
#define EFUSE_DIS_BT_S  12
/* EFUSE_DIS_DOWNLOAD_DCACHE : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_DOWNLOAD_DCACHE  (BIT(11))
#define EFUSE_DIS_DOWNLOAD_DCACHE_M  (BIT(11))
#define EFUSE_DIS_DOWNLOAD_DCACHE_V  0x1
#define EFUSE_DIS_DOWNLOAD_DCACHE_S  11
/* EFUSE_DIS_DOWNLOAD_ICACHE : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_DOWNLOAD_ICACHE  (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_M  (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_V  0x1
#define EFUSE_DIS_DOWNLOAD_ICACHE_S  10
/* EFUSE_DIS_DCACHE : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_DCACHE  (BIT(9))
#define EFUSE_DIS_DCACHE_M  (BIT(9))
#define EFUSE_DIS_DCACHE_V  0x1
#define EFUSE_DIS_DCACHE_S  9
/* EFUSE_DIS_ICACHE : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_ICACHE  (BIT(8))
#define EFUSE_DIS_ICACHE_M  (BIT(8))
#define EFUSE_DIS_ICACHE_V  0x1
#define EFUSE_DIS_ICACHE_S  8
/* EFUSE_DIS_RTC_RAM_BOOT : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_RTC_RAM_BOOT  (BIT(7))
#define EFUSE_DIS_RTC_RAM_BOOT_M  (BIT(7))
#define EFUSE_DIS_RTC_RAM_BOOT_V  0x1
#define EFUSE_DIS_RTC_RAM_BOOT_S  7
/* EFUSE_RD_DIS : R/W ;bitpos:[6:0] ;default: 7'h0 ; */
/*description: */
#define EFUSE_RD_DIS  0x0000007F
#define EFUSE_RD_DIS_M  ((EFUSE_RD_DIS_V)<<(EFUSE_RD_DIS_S))
#define EFUSE_RD_DIS_V  0x7F
#define EFUSE_RD_DIS_S  0

#define EFUSE_PGM_DATA2_REG          (DR_REG_EFUSE_BASE + 0x008)
/* EFUSE_KEY_PURPOSE_1 : R/W ;bitpos:[31:28] ;default: 4'b0 ; */
/*description: */
#define EFUSE_KEY_PURPOSE_1  0x0000000F
#define EFUSE_KEY_PURPOSE_1_M  ((EFUSE_KEY_PURPOSE_1_V)<<(EFUSE_KEY_PURPOSE_1_S))
#define EFUSE_KEY_PURPOSE_1_V  0xF
#define EFUSE_KEY_PURPOSE_1_S  28
/* EFUSE_KEY_PURPOSE_0 : R/W ;bitpos:[27:24] ;default: 4'b0 ; */
/*description: */
#define EFUSE_KEY_PURPOSE_0  0x0000000F
#define EFUSE_KEY_PURPOSE_0_M  ((EFUSE_KEY_PURPOSE_0_V)<<(EFUSE_KEY_PURPOSE_0_S))
#define EFUSE_KEY_PURPOSE_0_V  0xF
#define EFUSE_KEY_PURPOSE_0_S  24
/* EFUSE_SECURE_BOOT_KEY_REVOKE2 : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SECURE_BOOT_KEY_REVOKE2  (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_M  (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_S  23
/* EFUSE_SECURE_BOOT_KEY_REVOKE1 : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SECURE_BOOT_KEY_REVOKE1  (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_M  (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_S  22
/* EFUSE_SECURE_BOOT_KEY_REVOKE0 : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SECURE_BOOT_KEY_REVOKE0  (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_M  (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_S  21
/* EFUSE_SPI_BOOT_CRYPT_CNT : R/W ;bitpos:[20:18] ;default: 3'b0 ; */
/*description: */
#define EFUSE_SPI_BOOT_CRYPT_CNT  0x00000007
#define EFUSE_SPI_BOOT_CRYPT_CNT_M  ((EFUSE_SPI_BOOT_CRYPT_CNT_V)<<(EFUSE_SPI_BOOT_CRYPT_CNT_S))
#define EFUSE_SPI_BOOT_CRYPT_CNT_V  0x7
#define EFUSE_SPI_BOOT_CRYPT_CNT_S  18
/* EFUSE_WDT_DELAY_SEL : R/W ;bitpos:[17:16] ;default: 2'b0 ; */
/*description: */
#define EFUSE_WDT_DELAY_SEL  0x00000003
#define EFUSE_WDT_DELAY_SEL_M  ((EFUSE_WDT_DELAY_SEL_V)<<(EFUSE_WDT_DELAY_SEL_S))
#define EFUSE_WDT_DELAY_SEL_V  0x3
#define EFUSE_WDT_DELAY_SEL_S  16
/* EFUSE_SDIO_DCAP : R/W ;bitpos:[15:14] ;default: 2'b0 ; */
/*description: */
#define EFUSE_SDIO_DCAP  0x00000003
#define EFUSE_SDIO_DCAP_M  ((EFUSE_SDIO_DCAP_V)<<(EFUSE_SDIO_DCAP_S))
#define EFUSE_SDIO_DCAP_V  0x3
#define EFUSE_SDIO_DCAP_S  14
/* EFUSE_SDIO_INIT : R/W ;bitpos:[13:12] ;default: 2'b0 ; */
/*description: */
#define EFUSE_SDIO_INIT  0x00000003
#define EFUSE_SDIO_INIT_M  ((EFUSE_SDIO_INIT_V)<<(EFUSE_SDIO_INIT_S))
#define EFUSE_SDIO_INIT_V  0x3
#define EFUSE_SDIO_INIT_S  12
/* EFUSE_SDIO_DCURLIM : R/W ;bitpos:[11:9] ;default: 3'b0 ; */
/*description: */
#define EFUSE_SDIO_DCURLIM  0x00000007
#define EFUSE_SDIO_DCURLIM_M  ((EFUSE_SDIO_DCURLIM_V)<<(EFUSE_SDIO_DCURLIM_S))
#define EFUSE_SDIO_DCURLIM_V  0x7
#define EFUSE_SDIO_DCURLIM_S  9
/* EFUSE_SDIO_ENCURLIM : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SDIO_ENCURLIM  (BIT(8))
#define EFUSE_SDIO_ENCURLIM_M  (BIT(8))
#define EFUSE_SDIO_ENCURLIM_V  0x1
#define EFUSE_SDIO_ENCURLIM_S  8
/* EFUSE_SDIO_EN_INIT : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SDIO_EN_INIT  (BIT(7))
#define EFUSE_SDIO_EN_INIT_M  (BIT(7))
#define EFUSE_SDIO_EN_INIT_V  0x1
#define EFUSE_SDIO_EN_INIT_S  7
/* EFUSE_SDIO_FORCE : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SDIO_FORCE  (BIT(6))
#define EFUSE_SDIO_FORCE_M  (BIT(6))
#define EFUSE_SDIO_FORCE_V  0x1
#define EFUSE_SDIO_FORCE_S  6
/* EFUSE_SDIO_TIEH : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SDIO_TIEH  (BIT(5))
#define EFUSE_SDIO_TIEH_M  (BIT(5))
#define EFUSE_SDIO_TIEH_V  0x1
#define EFUSE_SDIO_TIEH_S  5
/* EFUSE_SDIO_XPD : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SDIO_XPD  (BIT(4))
#define EFUSE_SDIO_XPD_M  (BIT(4))
#define EFUSE_SDIO_XPD_V  0x1
#define EFUSE_SDIO_XPD_S  4
/* EFUSE_SDIO_DREFL : R/W ;bitpos:[3:2] ;default: 2'b0 ; */
/*description: */
#define EFUSE_SDIO_DREFL  0x00000003
#define EFUSE_SDIO_DREFL_M  ((EFUSE_SDIO_DREFL_V)<<(EFUSE_SDIO_DREFL_S))
#define EFUSE_SDIO_DREFL_V  0x3
#define EFUSE_SDIO_DREFL_S  2
/* EFUSE_SDIO_DREFM : R/W ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: */
#define EFUSE_SDIO_DREFM  0x00000003
#define EFUSE_SDIO_DREFM_M  ((EFUSE_SDIO_DREFM_V)<<(EFUSE_SDIO_DREFM_S))
#define EFUSE_SDIO_DREFM_V  0x3
#define EFUSE_SDIO_DREFM_S  0

#define EFUSE_PGM_DATA3_REG          (DR_REG_EFUSE_BASE + 0x00c)
/* EFUSE_FLASH_TPUW : R/W ;bitpos:[31:28] ;default: 4'b0 ; */
/*description: */
#define EFUSE_FLASH_TPUW  0x0000000F
#define EFUSE_FLASH_TPUW_M  ((EFUSE_FLASH_TPUW_V)<<(EFUSE_FLASH_TPUW_S))
#define EFUSE_FLASH_TPUW_V  0xF
#define EFUSE_FLASH_TPUW_S  28
/* EFUSE_XTAL_FREQ : R/W ;bitpos:[27:22] ;default: 6'b0 ; */
/*description: */
#define EFUSE_XTAL_FREQ  0x0000003F
#define EFUSE_XTAL_FREQ_M  ((EFUSE_XTAL_FREQ_V)<<(EFUSE_XTAL_FREQ_S))
#define EFUSE_XTAL_FREQ_V  0x3F
#define EFUSE_XTAL_FREQ_S  22
/* EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE  (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_M  (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_V  0x1
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_S  21
/* EFUSE_SECURE_BOOT_EN : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SECURE_BOOT_EN  (BIT(20))
#define EFUSE_SECURE_BOOT_EN_M  (BIT(20))
#define EFUSE_SECURE_BOOT_EN_V  0x1
#define EFUSE_SECURE_BOOT_EN_S  20
/* EFUSE_KEY_PURPOSE_6 : R/W ;bitpos:[19:16] ;default: 4'b0 ; */
/*description: */
#define EFUSE_KEY_PURPOSE_6  0x0000000F
#define EFUSE_KEY_PURPOSE_6_M  ((EFUSE_KEY_PURPOSE_6_V)<<(EFUSE_KEY_PURPOSE_6_S))
#define EFUSE_KEY_PURPOSE_6_V  0xF
#define EFUSE_KEY_PURPOSE_6_S  16
/* EFUSE_KEY_PURPOSE_5 : R/W ;bitpos:[15:12] ;default: 4'b0 ; */
/*description: */
#define EFUSE_KEY_PURPOSE_5  0x0000000F
#define EFUSE_KEY_PURPOSE_5_M  ((EFUSE_KEY_PURPOSE_5_V)<<(EFUSE_KEY_PURPOSE_5_S))
#define EFUSE_KEY_PURPOSE_5_V  0xF
#define EFUSE_KEY_PURPOSE_5_S  12
/* EFUSE_KEY_PURPOSE_4 : R/W ;bitpos:[11:8] ;default: 4'b0 ; */
/*description: */
#define EFUSE_KEY_PURPOSE_4  0x0000000F
#define EFUSE_KEY_PURPOSE_4_M  ((EFUSE_KEY_PURPOSE_4_V)<<(EFUSE_KEY_PURPOSE_4_S))
#define EFUSE_KEY_PURPOSE_4_V  0xF
#define EFUSE_KEY_PURPOSE_4_S  8
/* EFUSE_KEY_PURPOSE_3 : R/W ;bitpos:[7:4] ;default: 4'b0 ; */
/*description: */
#define EFUSE_KEY_PURPOSE_3  0x0000000F
#define EFUSE_KEY_PURPOSE_3_M  ((EFUSE_KEY_PURPOSE_3_V)<<(EFUSE_KEY_PURPOSE_3_S))
#define EFUSE_KEY_PURPOSE_3_V  0xF
#define EFUSE_KEY_PURPOSE_3_S  4
/* EFUSE_KEY_PURPOSE_2 : R/W ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: */
#define EFUSE_KEY_PURPOSE_2  0x0000000F
#define EFUSE_KEY_PURPOSE_2_M  ((EFUSE_KEY_PURPOSE_2_V)<<(EFUSE_KEY_PURPOSE_2_S))
#define EFUSE_KEY_PURPOSE_2_V  0xF
#define EFUSE_KEY_PURPOSE_2_S  0

#define EFUSE_PGM_DATA4_REG          (DR_REG_EFUSE_BASE + 0x010)
/* EFUSE_RESERVE : R/W ;bitpos:[31:8] ;default: 24'h0 ; */
/*description: */
#define EFUSE_RESERVE  0x00FFFFFF
#define EFUSE_RESERVE_M  ((EFUSE_RESERVE_V)<<(EFUSE_RESERVE_S))
#define EFUSE_RESERVE_V  0xFFFFFF
#define EFUSE_RESERVE_S  8
/* EFUSE_UART_PRINT_CONTROL : R/W ;bitpos:[7:6] ;default: 2'b0 ; */
/*description: */
#define EFUSE_UART_PRINT_CONTROL  0x00000003
#define EFUSE_UART_PRINT_CONTROL_M  ((EFUSE_UART_PRINT_CONTROL_V)<<(EFUSE_UART_PRINT_CONTROL_S))
#define EFUSE_UART_PRINT_CONTROL_V  0x3
#define EFUSE_UART_PRINT_CONTROL_S  6
/* EFUSE_ENABLE_SECURITY_DOWNLOAD : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define EFUSE_ENABLE_SECURITY_DOWNLOAD  (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_M  (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_V  0x1
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_S  5
/* EFUSE_DIS_USB_DOWNLOAD_MODE : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_USB_DOWNLOAD_MODE  (BIT(4))
#define EFUSE_DIS_USB_DOWNLOAD_MODE_M  (BIT(4))
#define EFUSE_DIS_USB_DOWNLOAD_MODE_V  0x1
#define EFUSE_DIS_USB_DOWNLOAD_MODE_S  4
/* EFUSE_DIS_TINY_BASIC : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_TINY_BASIC  (BIT(3))
#define EFUSE_DIS_TINY_BASIC_M  (BIT(3))
#define EFUSE_DIS_TINY_BASIC_V  0x1
#define EFUSE_DIS_TINY_BASIC_S  3
/* EFUSE_UART_PRINT_CHANNEL : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define EFUSE_UART_PRINT_CHANNEL  (BIT(2))
#define EFUSE_UART_PRINT_CHANNEL_M  (BIT(2))
#define EFUSE_UART_PRINT_CHANNEL_V  0x1
#define EFUSE_UART_PRINT_CHANNEL_S  2
/* EFUSE_DIS_LEGACY_SPI_BOOT : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_LEGACY_SPI_BOOT  (BIT(1))
#define EFUSE_DIS_LEGACY_SPI_BOOT_M  (BIT(1))
#define EFUSE_DIS_LEGACY_SPI_BOOT_V  0x1
#define EFUSE_DIS_LEGACY_SPI_BOOT_S  1
/* EFUSE_DIS_DOWNLOAD_MODE : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_DOWNLOAD_MODE  (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_M  (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_V  0x1
#define EFUSE_DIS_DOWNLOAD_MODE_S  0

#define EFUSE_PGM_DATA5_REG          (DR_REG_EFUSE_BASE + 0x014)
/* EFUSE_RS_DATA_23 : R/W ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: */
#define EFUSE_RS_DATA_23  0x000000FF
#define EFUSE_RS_DATA_23_M  ((EFUSE_RS_DATA_23_V)<<(EFUSE_RS_DATA_23_S))
#define EFUSE_RS_DATA_23_V  0xFF
#define EFUSE_RS_DATA_23_S  24
/* EFUSE_CHIP_VERSION : R/W ;bitpos:[23:0] ;default: 24'b0 ; */
/*description: */
#define EFUSE_CHIP_VERSION  0x00FFFFFF
#define EFUSE_CHIP_VERSION_M  ((EFUSE_CHIP_VERSION_V)<<(EFUSE_CHIP_VERSION_S))
#define EFUSE_CHIP_VERSION_V  0xFFFFFF
#define EFUSE_CHIP_VERSION_S  0

#define EFUSE_PGM_DATA6_REG          (DR_REG_EFUSE_BASE + 0x018)
/* EFUSE_RS_DATA_24_27 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_RS_DATA_24_27  0xFFFFFFFF
#define EFUSE_RS_DATA_24_27_M  ((EFUSE_RS_DATA_24_27_V)<<(EFUSE_RS_DATA_24_27_S))
#define EFUSE_RS_DATA_24_27_V  0xFFFFFFFF
#define EFUSE_RS_DATA_24_27_S  0

#define EFUSE_PGM_DATA7_REG          (DR_REG_EFUSE_BASE + 0x01c)
/* EFUSE_RS_DATA_28_31 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_RS_DATA_28_31  0xFFFFFFFF
#define EFUSE_RS_DATA_28_31_M  ((EFUSE_RS_DATA_28_31_V)<<(EFUSE_RS_DATA_28_31_S))
#define EFUSE_RS_DATA_28_31_V  0xFFFFFFFF
#define EFUSE_RS_DATA_28_31_S  0

#define EFUSE_PGM_CHECK_VALUE0_REG          (DR_REG_EFUSE_BASE + 0x020)
/* EFUSE_RS_DATA_32_35 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_RS_DATA_32_35  0xFFFFFFFF
#define EFUSE_RS_DATA_32_35_M  ((EFUSE_RS_DATA_32_35_V)<<(EFUSE_RS_DATA_32_35_S))
#define EFUSE_RS_DATA_32_35_V  0xFFFFFFFF
#define EFUSE_RS_DATA_32_35_S  0

#define EFUSE_PGM_CHECK_VALUE1_REG          (DR_REG_EFUSE_BASE + 0x024)
/* EFUSE_RS_DATA_36_39 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_RS_DATA_36_39  0xFFFFFFFF
#define EFUSE_RS_DATA_36_39_M  ((EFUSE_RS_DATA_36_39_V)<<(EFUSE_RS_DATA_36_39_S))
#define EFUSE_RS_DATA_36_39_V  0xFFFFFFFF
#define EFUSE_RS_DATA_36_39_S  0

#define EFUSE_PGM_CHECK_VALUE2_REG          (DR_REG_EFUSE_BASE + 0x028)
/* EFUSE_RS_DATA_40_43 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_RS_DATA_40_43  0xFFFFFFFF
#define EFUSE_RS_DATA_40_43_M  ((EFUSE_RS_DATA_40_43_V)<<(EFUSE_RS_DATA_40_43_S))
#define EFUSE_RS_DATA_40_43_V  0xFFFFFFFF
#define EFUSE_RS_DATA_40_43_S  0

#define EFUSE_RD_WR_DIS_REG          (DR_REG_EFUSE_BASE + 0x02c)
/* EFUSE_WR_DIS : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define EFUSE_WR_DIS  0xFFFFFFFF
#define EFUSE_WR_DIS_M  ((EFUSE_WR_DIS_V)<<(EFUSE_WR_DIS_S))
#define EFUSE_WR_DIS_V  0xFFFFFFFF
#define EFUSE_WR_DIS_S  0

#define EFUSE_RD_REPEAT_DATA0_REG          (DR_REG_EFUSE_BASE + 0x030)
/* EFUSE_SDIO_DREFH : RO ;bitpos:[31:30] ;default: 2'b0 ; */
/*description: */
#define EFUSE_SDIO_DREFH  0x00000003
#define EFUSE_SDIO_DREFH_M  ((EFUSE_SDIO_DREFH_V)<<(EFUSE_SDIO_DREFH_S))
#define EFUSE_SDIO_DREFH_V  0x3
#define EFUSE_SDIO_DREFH_S  30
/* EFUSE_SDIO_MODECURLIM : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SDIO_MODECURLIM  (BIT(29))
#define EFUSE_SDIO_MODECURLIM_M  (BIT(29))
#define EFUSE_SDIO_MODECURLIM_V  0x1
#define EFUSE_SDIO_MODECURLIM_S  29
/* EFUSE_USB_DRES : RO ;bitpos:[28:27] ;default: 2'b0 ; */
/*description: */
#define EFUSE_USB_DRES  0x00000003
#define EFUSE_USB_DRES_M  ((EFUSE_USB_DRES_V)<<(EFUSE_USB_DRES_S))
#define EFUSE_USB_DRES_V  0x3
#define EFUSE_USB_DRES_S  27
/* EFUSE_USB_FORCE_B : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define EFUSE_USB_FORCE_B  (BIT(26))
#define EFUSE_USB_FORCE_B_M  (BIT(26))
#define EFUSE_USB_FORCE_B_V  0x1
#define EFUSE_USB_FORCE_B_S  26
/* EFUSE_USB_EXT_PHY_ENABLE : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define EFUSE_USB_EXT_PHY_ENABLE  (BIT(25))
#define EFUSE_USB_EXT_PHY_ENABLE_M  (BIT(25))
#define EFUSE_USB_EXT_PHY_ENABLE_V  0x1
#define EFUSE_USB_EXT_PHY_ENABLE_S  25
/* EFUSE_USB_EXCHG_PINS : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define EFUSE_USB_EXCHG_PINS  (BIT(24))
#define EFUSE_USB_EXCHG_PINS_M  (BIT(24))
#define EFUSE_USB_EXCHG_PINS_V  0x1
#define EFUSE_USB_EXCHG_PINS_S  24
/* EFUSE_USB_DREFL : RO ;bitpos:[23:22] ;default: 2'b0 ; */
/*description: */
#define EFUSE_USB_DREFL  0x00000003
#define EFUSE_USB_DREFL_M  ((EFUSE_USB_DREFL_V)<<(EFUSE_USB_DREFL_S))
#define EFUSE_USB_DREFL_V  0x3
#define EFUSE_USB_DREFL_S  22
/* EFUSE_USB_DREFH : RO ;bitpos:[21:20] ;default: 2'b0 ; */
/*description: */
#define EFUSE_USB_DREFH  0x00000003
#define EFUSE_USB_DREFH_M  ((EFUSE_USB_DREFH_V)<<(EFUSE_USB_DREFH_S))
#define EFUSE_USB_DREFH_V  0x3
#define EFUSE_USB_DREFH_S  20
/* EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT  (BIT(19))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_M  (BIT(19))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_V  0x1
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_S  19
/* EFUSE_HARD_DIS_JTAG : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define EFUSE_HARD_DIS_JTAG  (BIT(18))
#define EFUSE_HARD_DIS_JTAG_M  (BIT(18))
#define EFUSE_HARD_DIS_JTAG_V  0x1
#define EFUSE_HARD_DIS_JTAG_S  18
/* EFUSE_SOFT_DIS_JTAG : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SOFT_DIS_JTAG  (BIT(17))
#define EFUSE_SOFT_DIS_JTAG_M  (BIT(17))
#define EFUSE_SOFT_DIS_JTAG_V  0x1
#define EFUSE_SOFT_DIS_JTAG_S  17
/* EFUSE_DIS_EFUSE_ATE_WR : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_EFUSE_ATE_WR  (BIT(16))
#define EFUSE_DIS_EFUSE_ATE_WR_M  (BIT(16))
#define EFUSE_DIS_EFUSE_ATE_WR_V  0x1
#define EFUSE_DIS_EFUSE_ATE_WR_S  16
/* EFUSE_DIS_SDIO_ACCESS : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_SDIO_ACCESS  (BIT(15))
#define EFUSE_DIS_SDIO_ACCESS_M  (BIT(15))
#define EFUSE_DIS_SDIO_ACCESS_V  0x1
#define EFUSE_DIS_SDIO_ACCESS_S  15
/* EFUSE_DIS_CAN : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_CAN  (BIT(14))
#define EFUSE_DIS_CAN_M  (BIT(14))
#define EFUSE_DIS_CAN_V  0x1
#define EFUSE_DIS_CAN_S  14
/* EFUSE_DIS_USB : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_USB  (BIT(13))
#define EFUSE_DIS_USB_M  (BIT(13))
#define EFUSE_DIS_USB_V  0x1
#define EFUSE_DIS_USB_S  13
/* EFUSE_DIS_BT : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_BT  (BIT(12))
#define EFUSE_DIS_BT_M  (BIT(12))
#define EFUSE_DIS_BT_V  0x1
#define EFUSE_DIS_BT_S  12
/* EFUSE_DIS_DOWNLOAD_DCACHE : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_DOWNLOAD_DCACHE  (BIT(11))
#define EFUSE_DIS_DOWNLOAD_DCACHE_M  (BIT(11))
#define EFUSE_DIS_DOWNLOAD_DCACHE_V  0x1
#define EFUSE_DIS_DOWNLOAD_DCACHE_S  11
/* EFUSE_DIS_DOWNLOAD_ICACHE : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_DOWNLOAD_ICACHE  (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_M  (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_V  0x1
#define EFUSE_DIS_DOWNLOAD_ICACHE_S  10
/* EFUSE_DIS_DCACHE : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_DCACHE  (BIT(9))
#define EFUSE_DIS_DCACHE_M  (BIT(9))
#define EFUSE_DIS_DCACHE_V  0x1
#define EFUSE_DIS_DCACHE_S  9
/* EFUSE_DIS_ICACHE : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_ICACHE  (BIT(8))
#define EFUSE_DIS_ICACHE_M  (BIT(8))
#define EFUSE_DIS_ICACHE_V  0x1
#define EFUSE_DIS_ICACHE_S  8
/* EFUSE_DIS_RTC_RAM_BOOT : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_RTC_RAM_BOOT  (BIT(7))
#define EFUSE_DIS_RTC_RAM_BOOT_M  (BIT(7))
#define EFUSE_DIS_RTC_RAM_BOOT_V  0x1
#define EFUSE_DIS_RTC_RAM_BOOT_S  7
/* EFUSE_RD_DIS : RO ;bitpos:[6:0] ;default: 7'h0 ; */
/*description: */
#define EFUSE_RD_DIS  0x0000007F
#define EFUSE_RD_DIS_M  ((EFUSE_RD_DIS_V)<<(EFUSE_RD_DIS_S))
#define EFUSE_RD_DIS_V  0x7F
#define EFUSE_RD_DIS_S  0

#define EFUSE_RD_REPEAT_DATA1_REG          (DR_REG_EFUSE_BASE + 0x034)
/* EFUSE_KEY_PURPOSE_1 : RO ;bitpos:[31:28] ;default: 4'b0 ; */
/*description: */
#define EFUSE_KEY_PURPOSE_1  0x0000000F
#define EFUSE_KEY_PURPOSE_1_M  ((EFUSE_KEY_PURPOSE_1_V)<<(EFUSE_KEY_PURPOSE_1_S))
#define EFUSE_KEY_PURPOSE_1_V  0xF
#define EFUSE_KEY_PURPOSE_1_S  28
/* EFUSE_KEY_PURPOSE_0 : RO ;bitpos:[27:24] ;default: 4'b0 ; */
/*description: */
#define EFUSE_KEY_PURPOSE_0  0x0000000F
#define EFUSE_KEY_PURPOSE_0_M  ((EFUSE_KEY_PURPOSE_0_V)<<(EFUSE_KEY_PURPOSE_0_S))
#define EFUSE_KEY_PURPOSE_0_V  0xF
#define EFUSE_KEY_PURPOSE_0_S  24
/* EFUSE_SECURE_BOOT_KEY_REVOKE2 : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SECURE_BOOT_KEY_REVOKE2  (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_M  (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_S  23
/* EFUSE_SECURE_BOOT_KEY_REVOKE1 : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SECURE_BOOT_KEY_REVOKE1  (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_M  (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_S  22
/* EFUSE_SECURE_BOOT_KEY_REVOKE0 : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SECURE_BOOT_KEY_REVOKE0  (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_M  (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_S  21
/* EFUSE_SPI_BOOT_CRYPT_CNT : RO ;bitpos:[20:18] ;default: 3'b0 ; */
/*description: */
#define EFUSE_SPI_BOOT_CRYPT_CNT  0x00000007
#define EFUSE_SPI_BOOT_CRYPT_CNT_M  ((EFUSE_SPI_BOOT_CRYPT_CNT_V)<<(EFUSE_SPI_BOOT_CRYPT_CNT_S))
#define EFUSE_SPI_BOOT_CRYPT_CNT_V  0x7
#define EFUSE_SPI_BOOT_CRYPT_CNT_S  18
/* EFUSE_WDT_DELAY_SEL : RO ;bitpos:[17:16] ;default: 2'b0 ; */
/*description: */
#define EFUSE_WDT_DELAY_SEL  0x00000003
#define EFUSE_WDT_DELAY_SEL_M  ((EFUSE_WDT_DELAY_SEL_V)<<(EFUSE_WDT_DELAY_SEL_S))
#define EFUSE_WDT_DELAY_SEL_V  0x3
#define EFUSE_WDT_DELAY_SEL_S  16
/* EFUSE_EUFSE_SDIO_DCAP : RO ;bitpos:[15:14] ;default: 2'b0 ; */
/*description: */
#define EFUSE_EUFSE_SDIO_DCAP  0x00000003
#define EFUSE_EUFSE_SDIO_DCAP_M  ((EFUSE_EUFSE_SDIO_DCAP_V)<<(EFUSE_EUFSE_SDIO_DCAP_S))
#define EFUSE_EUFSE_SDIO_DCAP_V  0x3
#define EFUSE_EUFSE_SDIO_DCAP_S  14
/* EFUSE_SDIO_INIT : RO ;bitpos:[13:12] ;default: 2'b0 ; */
/*description: */
#define EFUSE_SDIO_INIT  0x00000003
#define EFUSE_SDIO_INIT_M  ((EFUSE_SDIO_INIT_V)<<(EFUSE_SDIO_INIT_S))
#define EFUSE_SDIO_INIT_V  0x3
#define EFUSE_SDIO_INIT_S  12
/* EFUSE_SDIO_DCURLIM : RO ;bitpos:[11:9] ;default: 3'b0 ; */
/*description: */
#define EFUSE_SDIO_DCURLIM  0x00000007
#define EFUSE_SDIO_DCURLIM_M  ((EFUSE_SDIO_DCURLIM_V)<<(EFUSE_SDIO_DCURLIM_S))
#define EFUSE_SDIO_DCURLIM_V  0x7
#define EFUSE_SDIO_DCURLIM_S  9
/* EFUSE_SDIO_ENCURLIM : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SDIO_ENCURLIM  (BIT(8))
#define EFUSE_SDIO_ENCURLIM_M  (BIT(8))
#define EFUSE_SDIO_ENCURLIM_V  0x1
#define EFUSE_SDIO_ENCURLIM_S  8
/* EFUSE_SDIO_EN_INIT : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SDIO_EN_INIT  (BIT(7))
#define EFUSE_SDIO_EN_INIT_M  (BIT(7))
#define EFUSE_SDIO_EN_INIT_V  0x1
#define EFUSE_SDIO_EN_INIT_S  7
/* EFUSE_SDIO_FORCE : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SDIO_FORCE  (BIT(6))
#define EFUSE_SDIO_FORCE_M  (BIT(6))
#define EFUSE_SDIO_FORCE_V  0x1
#define EFUSE_SDIO_FORCE_S  6
/* EFUSE_SDIO_TIEH : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SDIO_TIEH  (BIT(5))
#define EFUSE_SDIO_TIEH_M  (BIT(5))
#define EFUSE_SDIO_TIEH_V  0x1
#define EFUSE_SDIO_TIEH_S  5
/* EFUSE_SDIO_XPD : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SDIO_XPD  (BIT(4))
#define EFUSE_SDIO_XPD_M  (BIT(4))
#define EFUSE_SDIO_XPD_V  0x1
#define EFUSE_SDIO_XPD_S  4
/* EFUSE_SDIO_DREFL : RO ;bitpos:[3:2] ;default: 2'b0 ; */
/*description: */
#define EFUSE_SDIO_DREFL  0x00000003
#define EFUSE_SDIO_DREFL_M  ((EFUSE_SDIO_DREFL_V)<<(EFUSE_SDIO_DREFL_S))
#define EFUSE_SDIO_DREFL_V  0x3
#define EFUSE_SDIO_DREFL_S  2
/* EFUSE_SDIO_DREFM : RO ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: */
#define EFUSE_SDIO_DREFM  0x00000003
#define EFUSE_SDIO_DREFM_M  ((EFUSE_SDIO_DREFM_V)<<(EFUSE_SDIO_DREFM_S))
#define EFUSE_SDIO_DREFM_V  0x3
#define EFUSE_SDIO_DREFM_S  0

#define EFUSE_RD_REPEAT_DATA2_REG          (DR_REG_EFUSE_BASE + 0x038)
/* EFUSE_FLASH_TPUW : RO ;bitpos:[31:28] ;default: 4'b0 ; */
/*description: */
#define EFUSE_FLASH_TPUW  0x0000000F
#define EFUSE_FLASH_TPUW_M  ((EFUSE_FLASH_TPUW_V)<<(EFUSE_FLASH_TPUW_S))
#define EFUSE_FLASH_TPUW_V  0xF
#define EFUSE_FLASH_TPUW_S  28
/* EFUSE_XTAL_FREQ : RO ;bitpos:[27:22] ;default: 6'b0 ; */
/*description: */
#define EFUSE_XTAL_FREQ  0x0000003F
#define EFUSE_XTAL_FREQ_M  ((EFUSE_XTAL_FREQ_V)<<(EFUSE_XTAL_FREQ_S))
#define EFUSE_XTAL_FREQ_V  0x3F
#define EFUSE_XTAL_FREQ_S  22
/* EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE  (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_M  (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_V  0x1
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_S  21
/* EFUSE_SECURE_BOOT_EN : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define EFUSE_SECURE_BOOT_EN  (BIT(20))
#define EFUSE_SECURE_BOOT_EN_M  (BIT(20))
#define EFUSE_SECURE_BOOT_EN_V  0x1
#define EFUSE_SECURE_BOOT_EN_S  20
/* EFUSE_KEY_PURPOSE_6 : RO ;bitpos:[19:16] ;default: 4'b0 ; */
/*description: */
#define EFUSE_KEY_PURPOSE_6  0x0000000F
#define EFUSE_KEY_PURPOSE_6_M  ((EFUSE_KEY_PURPOSE_6_V)<<(EFUSE_KEY_PURPOSE_6_S))
#define EFUSE_KEY_PURPOSE_6_V  0xF
#define EFUSE_KEY_PURPOSE_6_S  16
/* EFUSE_KEY_PURPOSE_5 : RO ;bitpos:[15:12] ;default: 4'b0 ; */
/*description: */
#define EFUSE_KEY_PURPOSE_5  0x0000000F
#define EFUSE_KEY_PURPOSE_5_M  ((EFUSE_KEY_PURPOSE_5_V)<<(EFUSE_KEY_PURPOSE_5_S))
#define EFUSE_KEY_PURPOSE_5_V  0xF
#define EFUSE_KEY_PURPOSE_5_S  12
/* EFUSE_KEY_PURPOSE_4 : RO ;bitpos:[11:8] ;default: 4'b0 ; */
/*description: */
#define EFUSE_KEY_PURPOSE_4  0x0000000F
#define EFUSE_KEY_PURPOSE_4_M  ((EFUSE_KEY_PURPOSE_4_V)<<(EFUSE_KEY_PURPOSE_4_S))
#define EFUSE_KEY_PURPOSE_4_V  0xF
#define EFUSE_KEY_PURPOSE_4_S  8
/* EFUSE_KEY_PURPOSE_3 : RO ;bitpos:[7:4] ;default: 4'b0 ; */
/*description: */
#define EFUSE_KEY_PURPOSE_3  0x0000000F
#define EFUSE_KEY_PURPOSE_3_M  ((EFUSE_KEY_PURPOSE_3_V)<<(EFUSE_KEY_PURPOSE_3_S))
#define EFUSE_KEY_PURPOSE_3_V  0xF
#define EFUSE_KEY_PURPOSE_3_S  4
/* EFUSE_KEY_PURPOSE_2 : RO ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: */
#define EFUSE_KEY_PURPOSE_2  0x0000000F
#define EFUSE_KEY_PURPOSE_2_M  ((EFUSE_KEY_PURPOSE_2_V)<<(EFUSE_KEY_PURPOSE_2_S))
#define EFUSE_KEY_PURPOSE_2_V  0xF
#define EFUSE_KEY_PURPOSE_2_S  0

#define EFUSE_RD_REPEAT_DATA3_REG          (DR_REG_EFUSE_BASE + 0x03c)
/* EFUSE_RESERVE : RO ;bitpos:[31:8] ;default: 24'h0 ; */
/*description: */
#define EFUSE_RESERVE  0x00FFFFFF
#define EFUSE_RESERVE_M  ((EFUSE_RESERVE_V)<<(EFUSE_RESERVE_S))
#define EFUSE_RESERVE_V  0xFFFFFF
#define EFUSE_RESERVE_S  8
/* EFUSE_UART_PRINT_CONTROL : RO ;bitpos:[7:6] ;default: 2'b0 ; */
/*description: */
#define EFUSE_UART_PRINT_CONTROL  0x00000003
#define EFUSE_UART_PRINT_CONTROL_M  ((EFUSE_UART_PRINT_CONTROL_V)<<(EFUSE_UART_PRINT_CONTROL_S))
#define EFUSE_UART_PRINT_CONTROL_V  0x3
#define EFUSE_UART_PRINT_CONTROL_S  6
/* EFUSE_ENABLE_SECURITY_DOWNLOAD : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define EFUSE_ENABLE_SECURITY_DOWNLOAD  (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_M  (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_V  0x1
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_S  5
/* EFUSE_DIS_USB_DOWNLOAD_MODE : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_USB_DOWNLOAD_MODE  (BIT(4))
#define EFUSE_DIS_USB_DOWNLOAD_MODE_M  (BIT(4))
#define EFUSE_DIS_USB_DOWNLOAD_MODE_V  0x1
#define EFUSE_DIS_USB_DOWNLOAD_MODE_S  4
/* EFUSE_DIS_TINY_BASIC : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_TINY_BASIC  (BIT(3))
#define EFUSE_DIS_TINY_BASIC_M  (BIT(3))
#define EFUSE_DIS_TINY_BASIC_V  0x1
#define EFUSE_DIS_TINY_BASIC_S  3
/* EFUSE_UART_PRINT_CHANNEL : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define EFUSE_UART_PRINT_CHANNEL  (BIT(2))
#define EFUSE_UART_PRINT_CHANNEL_M  (BIT(2))
#define EFUSE_UART_PRINT_CHANNEL_V  0x1
#define EFUSE_UART_PRINT_CHANNEL_S  2
/* EFUSE_DIS_LEGACY_SPI_BOOT : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_LEGACY_SPI_BOOT  (BIT(1))
#define EFUSE_DIS_LEGACY_SPI_BOOT_M  (BIT(1))
#define EFUSE_DIS_LEGACY_SPI_BOOT_V  0x1
#define EFUSE_DIS_LEGACY_SPI_BOOT_S  1
/* EFUSE_DIS_DOWNLOAD_MODE : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DIS_DOWNLOAD_MODE  (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_M  (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_V  0x1
#define EFUSE_DIS_DOWNLOAD_MODE_S  0

#define EFUSE_RD_REPEAT_DATA4_REG          (DR_REG_EFUSE_BASE + 0x040)
/* EFUSE_CHIP_VERSION : RO ;bitpos:[23:0] ;default: 24'b0 ; */
/*description: */
#define EFUSE_CHIP_VERSION  0x00FFFFFF
#define EFUSE_CHIP_VERSION_M  ((EFUSE_CHIP_VERSION_V)<<(EFUSE_CHIP_VERSION_S))
#define EFUSE_CHIP_VERSION_V  0xFFFFFF
#define EFUSE_CHIP_VERSION_S  0

#define EFUSE_RD_MAC_SPI_8M_0_REG          (DR_REG_EFUSE_BASE + 0x044)
/* EFUSE_MAC_0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_MAC_0  0xFFFFFFFF
#define EFUSE_MAC_0_M  ((EFUSE_MAC_0_V)<<(EFUSE_MAC_0_S))
#define EFUSE_MAC_0_V  0xFFFFFFFF
#define EFUSE_MAC_0_S  0

#define EFUSE_RD_MAC_SPI_8M_1_REG          (DR_REG_EFUSE_BASE + 0x048)
/* EFUSE_SPI_PAD_CONF_0 : RO ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: */
#define EFUSE_SPI_PAD_CONF_0  0x0000FFFF
#define EFUSE_SPI_PAD_CONF_0_M  ((EFUSE_SPI_PAD_CONF_0_V)<<(EFUSE_SPI_PAD_CONF_0_S))
#define EFUSE_SPI_PAD_CONF_0_V  0xFFFF
#define EFUSE_SPI_PAD_CONF_0_S  16
/* EFUSE_MAC_1 : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: */
#define EFUSE_MAC_1  0x0000FFFF
#define EFUSE_MAC_1_M  ((EFUSE_MAC_1_V)<<(EFUSE_MAC_1_S))
#define EFUSE_MAC_1_V  0xFFFF
#define EFUSE_MAC_1_S  0

#define EFUSE_RD_MAC_SPI_8M_2_REG          (DR_REG_EFUSE_BASE + 0x04c)
/* EFUSE_CLK8M_FREQ : RO ;bitpos:[31:20] ;default: 12'h0 ; */
/*description: */
#define EFUSE_CLK8M_FREQ  0x00000FFF
#define EFUSE_CLK8M_FREQ_M  ((EFUSE_CLK8M_FREQ_V)<<(EFUSE_CLK8M_FREQ_S))
#define EFUSE_CLK8M_FREQ_V  0xFFF
#define EFUSE_CLK8M_FREQ_S  20
/* EFUSE_SPI_PAD_CONF_1 : RO ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: */
#define EFUSE_SPI_PAD_CONF_1  0x000FFFFF
#define EFUSE_SPI_PAD_CONF_1_M  ((EFUSE_SPI_PAD_CONF_1_V)<<(EFUSE_SPI_PAD_CONF_1_S))
#define EFUSE_SPI_PAD_CONF_1_V  0xFFFFF
#define EFUSE_SPI_PAD_CONF_1_S  0

#define EFUSE_RD_MAC_SPI_8M_3_REG          (DR_REG_EFUSE_BASE + 0x050)
/* EFUSE_SYS_DATA_PART0_0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_SYS_DATA_PART0_0  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART0_0_M  ((EFUSE_SYS_DATA_PART0_0_V)<<(EFUSE_SYS_DATA_PART0_0_S))
#define EFUSE_SYS_DATA_PART0_0_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART0_0_S  0

#define EFUSE_RD_MAC_SPI_8M_4_REG          (DR_REG_EFUSE_BASE + 0x054)
/* EFUSE_SYS_DATA_PART0_1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_SYS_DATA_PART0_1  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART0_1_M  ((EFUSE_SYS_DATA_PART0_1_V)<<(EFUSE_SYS_DATA_PART0_1_S))
#define EFUSE_SYS_DATA_PART0_1_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART0_1_S  0

#define EFUSE_RD_MAC_SPI_8M_5_REG          (DR_REG_EFUSE_BASE + 0x058)
/* EFUSE_SYS_DATA_PART0_2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_SYS_DATA_PART0_2  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART0_2_M  ((EFUSE_SYS_DATA_PART0_2_V)<<(EFUSE_SYS_DATA_PART0_2_S))
#define EFUSE_SYS_DATA_PART0_2_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART0_2_S  0

#define EFUSE_RD_SYS_DATA0_REG          (DR_REG_EFUSE_BASE + 0x05c)
/* EFUSE_SYS_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_SYS_DATA0  0xFFFFFFFF
#define EFUSE_SYS_DATA0_M  ((EFUSE_SYS_DATA0_V)<<(EFUSE_SYS_DATA0_S))
#define EFUSE_SYS_DATA0_V  0xFFFFFFFF
#define EFUSE_SYS_DATA0_S  0

#define EFUSE_RD_SYS_DATA1_REG          (DR_REG_EFUSE_BASE + 0x060)
/* EFUSE_SYS_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_SYS_DATA1  0xFFFFFFFF
#define EFUSE_SYS_DATA1_M  ((EFUSE_SYS_DATA1_V)<<(EFUSE_SYS_DATA1_S))
#define EFUSE_SYS_DATA1_V  0xFFFFFFFF
#define EFUSE_SYS_DATA1_S  0

#define EFUSE_RD_SYS_DATA2_REG          (DR_REG_EFUSE_BASE + 0x064)
/* EFUSE_SYS_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_SYS_DATA2  0xFFFFFFFF
#define EFUSE_SYS_DATA2_M  ((EFUSE_SYS_DATA2_V)<<(EFUSE_SYS_DATA2_S))
#define EFUSE_SYS_DATA2_V  0xFFFFFFFF
#define EFUSE_SYS_DATA2_S  0

#define EFUSE_RD_SYS_DATA3_REG          (DR_REG_EFUSE_BASE + 0x068)
/* EFUSE_SYS_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_SYS_DATA3  0xFFFFFFFF
#define EFUSE_SYS_DATA3_M  ((EFUSE_SYS_DATA3_V)<<(EFUSE_SYS_DATA3_S))
#define EFUSE_SYS_DATA3_V  0xFFFFFFFF
#define EFUSE_SYS_DATA3_S  0

#define EFUSE_RD_SYS_DATA4_REG          (DR_REG_EFUSE_BASE + 0x06c)
/* EFUSE_SYS_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_SYS_DATA4  0xFFFFFFFF
#define EFUSE_SYS_DATA4_M  ((EFUSE_SYS_DATA4_V)<<(EFUSE_SYS_DATA4_S))
#define EFUSE_SYS_DATA4_V  0xFFFFFFFF
#define EFUSE_SYS_DATA4_S  0

#define EFUSE_RD_SYS_DATA5_REG          (DR_REG_EFUSE_BASE + 0x070)
/* EFUSE_SYS_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_SYS_DATA5  0xFFFFFFFF
#define EFUSE_SYS_DATA5_M  ((EFUSE_SYS_DATA5_V)<<(EFUSE_SYS_DATA5_S))
#define EFUSE_SYS_DATA5_V  0xFFFFFFFF
#define EFUSE_SYS_DATA5_S  0

#define EFUSE_RD_SYS_DATA6_REG          (DR_REG_EFUSE_BASE + 0x074)
/* EFUSE_SYS_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_SYS_DATA6  0xFFFFFFFF
#define EFUSE_SYS_DATA6_M  ((EFUSE_SYS_DATA6_V)<<(EFUSE_SYS_DATA6_S))
#define EFUSE_SYS_DATA6_V  0xFFFFFFFF
#define EFUSE_SYS_DATA6_S  0

#define EFUSE_RD_SYS_DATA7_REG          (DR_REG_EFUSE_BASE + 0x078)
/* EFUSE_SYS_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_SYS_DATA7  0xFFFFFFFF
#define EFUSE_SYS_DATA7_M  ((EFUSE_SYS_DATA7_V)<<(EFUSE_SYS_DATA7_S))
#define EFUSE_SYS_DATA7_V  0xFFFFFFFF
#define EFUSE_SYS_DATA7_S  0

#define EFUSE_RD_USR_DATA0_REG          (DR_REG_EFUSE_BASE + 0x07c)
/* EFUSE_USR_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_USR_DATA0  0xFFFFFFFF
#define EFUSE_USR_DATA0_M  ((EFUSE_USR_DATA0_V)<<(EFUSE_USR_DATA0_S))
#define EFUSE_USR_DATA0_V  0xFFFFFFFF
#define EFUSE_USR_DATA0_S  0

#define EFUSE_RD_USR_DATA1_REG          (DR_REG_EFUSE_BASE + 0x080)
/* EFUSE_USR_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_USR_DATA1  0xFFFFFFFF
#define EFUSE_USR_DATA1_M  ((EFUSE_USR_DATA1_V)<<(EFUSE_USR_DATA1_S))
#define EFUSE_USR_DATA1_V  0xFFFFFFFF
#define EFUSE_USR_DATA1_S  0

#define EFUSE_RD_USR_DATA2_REG          (DR_REG_EFUSE_BASE + 0x084)
/* EFUSE_USR_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_USR_DATA2  0xFFFFFFFF
#define EFUSE_USR_DATA2_M  ((EFUSE_USR_DATA2_V)<<(EFUSE_USR_DATA2_S))
#define EFUSE_USR_DATA2_V  0xFFFFFFFF
#define EFUSE_USR_DATA2_S  0

#define EFUSE_RD_USR_DATA3_REG          (DR_REG_EFUSE_BASE + 0x088)
/* EFUSE_USR_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_USR_DATA3  0xFFFFFFFF
#define EFUSE_USR_DATA3_M  ((EFUSE_USR_DATA3_V)<<(EFUSE_USR_DATA3_S))
#define EFUSE_USR_DATA3_V  0xFFFFFFFF
#define EFUSE_USR_DATA3_S  0

#define EFUSE_RD_USR_DATA4_REG          (DR_REG_EFUSE_BASE + 0x08c)
/* EFUSE_USR_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_USR_DATA4  0xFFFFFFFF
#define EFUSE_USR_DATA4_M  ((EFUSE_USR_DATA4_V)<<(EFUSE_USR_DATA4_S))
#define EFUSE_USR_DATA4_V  0xFFFFFFFF
#define EFUSE_USR_DATA4_S  0

#define EFUSE_RD_USR_DATA5_REG          (DR_REG_EFUSE_BASE + 0x090)
/* EFUSE_USR_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_USR_DATA5  0xFFFFFFFF
#define EFUSE_USR_DATA5_M  ((EFUSE_USR_DATA5_V)<<(EFUSE_USR_DATA5_S))
#define EFUSE_USR_DATA5_V  0xFFFFFFFF
#define EFUSE_USR_DATA5_S  0

#define EFUSE_RD_USR_DATA6_REG          (DR_REG_EFUSE_BASE + 0x094)
/* EFUSE_USR_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_USR_DATA6  0xFFFFFFFF
#define EFUSE_USR_DATA6_M  ((EFUSE_USR_DATA6_V)<<(EFUSE_USR_DATA6_S))
#define EFUSE_USR_DATA6_V  0xFFFFFFFF
#define EFUSE_USR_DATA6_S  0

#define EFUSE_RD_USR_DATA7_REG          (DR_REG_EFUSE_BASE + 0x098)
/* EFUSE_USR_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_USR_DATA7  0xFFFFFFFF
#define EFUSE_USR_DATA7_M  ((EFUSE_USR_DATA7_V)<<(EFUSE_USR_DATA7_S))
#define EFUSE_USR_DATA7_V  0xFFFFFFFF
#define EFUSE_USR_DATA7_S  0

#define EFUSE_RD_KEY0_DATA0_REG          (DR_REG_EFUSE_BASE + 0x09c)
/* EFUSE_KEY0_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY0_DATA0  0xFFFFFFFF
#define EFUSE_KEY0_DATA0_M  ((EFUSE_KEY0_DATA0_V)<<(EFUSE_KEY0_DATA0_S))
#define EFUSE_KEY0_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA0_S  0

#define EFUSE_RD_KEY0_DATA1_REG          (DR_REG_EFUSE_BASE + 0x0a0)
/* EFUSE_KEY0_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY0_DATA1  0xFFFFFFFF
#define EFUSE_KEY0_DATA1_M  ((EFUSE_KEY0_DATA1_V)<<(EFUSE_KEY0_DATA1_S))
#define EFUSE_KEY0_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA1_S  0

#define EFUSE_RD_KEY0_DATA2_REG          (DR_REG_EFUSE_BASE + 0x0a4)
/* EFUSE_KEY0_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY0_DATA2  0xFFFFFFFF
#define EFUSE_KEY0_DATA2_M  ((EFUSE_KEY0_DATA2_V)<<(EFUSE_KEY0_DATA2_S))
#define EFUSE_KEY0_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA2_S  0

#define EFUSE_RD_KEY0_DATA3_REG          (DR_REG_EFUSE_BASE + 0x0a8)
/* EFUSE_KEY0_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY0_DATA3  0xFFFFFFFF
#define EFUSE_KEY0_DATA3_M  ((EFUSE_KEY0_DATA3_V)<<(EFUSE_KEY0_DATA3_S))
#define EFUSE_KEY0_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA3_S  0

#define EFUSE_RD_KEY0_DATA4_REG          (DR_REG_EFUSE_BASE + 0x0ac)
/* EFUSE_KEY0_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY0_DATA4  0xFFFFFFFF
#define EFUSE_KEY0_DATA4_M  ((EFUSE_KEY0_DATA4_V)<<(EFUSE_KEY0_DATA4_S))
#define EFUSE_KEY0_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA4_S  0

#define EFUSE_RD_KEY0_DATA5_REG          (DR_REG_EFUSE_BASE + 0x0b0)
/* EFUSE_KEY0_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY0_DATA5  0xFFFFFFFF
#define EFUSE_KEY0_DATA5_M  ((EFUSE_KEY0_DATA5_V)<<(EFUSE_KEY0_DATA5_S))
#define EFUSE_KEY0_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA5_S  0

#define EFUSE_RD_KEY0_DATA6_REG          (DR_REG_EFUSE_BASE + 0x0b4)
/* EFUSE_KEY0_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY0_DATA6  0xFFFFFFFF
#define EFUSE_KEY0_DATA6_M  ((EFUSE_KEY0_DATA6_V)<<(EFUSE_KEY0_DATA6_S))
#define EFUSE_KEY0_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA6_S  0

#define EFUSE_RD_KEY0_DATA7_REG          (DR_REG_EFUSE_BASE + 0x0b8)
/* EFUSE_KEY0_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY0_DATA7  0xFFFFFFFF
#define EFUSE_KEY0_DATA7_M  ((EFUSE_KEY0_DATA7_V)<<(EFUSE_KEY0_DATA7_S))
#define EFUSE_KEY0_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA7_S  0

#define EFUSE_RD_KEY1_DATA0_REG          (DR_REG_EFUSE_BASE + 0x0bc)
/* EFUSE_KEY1_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY1_DATA0  0xFFFFFFFF
#define EFUSE_KEY1_DATA0_M  ((EFUSE_KEY1_DATA0_V)<<(EFUSE_KEY1_DATA0_S))
#define EFUSE_KEY1_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA0_S  0

#define EFUSE_RD_KEY1_DATA1_REG          (DR_REG_EFUSE_BASE + 0x0c0)
/* EFUSE_KEY1_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY1_DATA1  0xFFFFFFFF
#define EFUSE_KEY1_DATA1_M  ((EFUSE_KEY1_DATA1_V)<<(EFUSE_KEY1_DATA1_S))
#define EFUSE_KEY1_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA1_S  0

#define EFUSE_RD_KEY1_DATA2_REG          (DR_REG_EFUSE_BASE + 0x0c4)
/* EFUSE_KEY1_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY1_DATA2  0xFFFFFFFF
#define EFUSE_KEY1_DATA2_M  ((EFUSE_KEY1_DATA2_V)<<(EFUSE_KEY1_DATA2_S))
#define EFUSE_KEY1_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA2_S  0

#define EFUSE_RD_KEY1_DATA3_REG          (DR_REG_EFUSE_BASE + 0x0c8)
/* EFUSE_KEY1_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY1_DATA3  0xFFFFFFFF
#define EFUSE_KEY1_DATA3_M  ((EFUSE_KEY1_DATA3_V)<<(EFUSE_KEY1_DATA3_S))
#define EFUSE_KEY1_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA3_S  0

#define EFUSE_RD_KEY1_DATA4_REG          (DR_REG_EFUSE_BASE + 0x0cc)
/* EFUSE_KEY1_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY1_DATA4  0xFFFFFFFF
#define EFUSE_KEY1_DATA4_M  ((EFUSE_KEY1_DATA4_V)<<(EFUSE_KEY1_DATA4_S))
#define EFUSE_KEY1_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA4_S  0

#define EFUSE_RD_KEY1_DATA5_REG          (DR_REG_EFUSE_BASE + 0x0d0)
/* EFUSE_KEY1_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY1_DATA5  0xFFFFFFFF
#define EFUSE_KEY1_DATA5_M  ((EFUSE_KEY1_DATA5_V)<<(EFUSE_KEY1_DATA5_S))
#define EFUSE_KEY1_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA5_S  0

#define EFUSE_RD_KEY1_DATA6_REG          (DR_REG_EFUSE_BASE + 0x0d4)
/* EFUSE_KEY1_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY1_DATA6  0xFFFFFFFF
#define EFUSE_KEY1_DATA6_M  ((EFUSE_KEY1_DATA6_V)<<(EFUSE_KEY1_DATA6_S))
#define EFUSE_KEY1_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA6_S  0

#define EFUSE_RD_KEY1_DATA7_REG          (DR_REG_EFUSE_BASE + 0x0d8)
/* EFUSE_KEY1_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY1_DATA7  0xFFFFFFFF
#define EFUSE_KEY1_DATA7_M  ((EFUSE_KEY1_DATA7_V)<<(EFUSE_KEY1_DATA7_S))
#define EFUSE_KEY1_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA7_S  0

#define EFUSE_RD_KEY2_DATA0_REG          (DR_REG_EFUSE_BASE + 0x0dc)
/* EFUSE_KEY2_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY2_DATA0  0xFFFFFFFF
#define EFUSE_KEY2_DATA0_M  ((EFUSE_KEY2_DATA0_V)<<(EFUSE_KEY2_DATA0_S))
#define EFUSE_KEY2_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA0_S  0

#define EFUSE_RD_KEY2_DATA1_REG          (DR_REG_EFUSE_BASE + 0x0e0)
/* EFUSE_KEY2_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY2_DATA1  0xFFFFFFFF
#define EFUSE_KEY2_DATA1_M  ((EFUSE_KEY2_DATA1_V)<<(EFUSE_KEY2_DATA1_S))
#define EFUSE_KEY2_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA1_S  0

#define EFUSE_RD_KEY2_DATA2_REG          (DR_REG_EFUSE_BASE + 0x0e4)
/* EFUSE_KEY2_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY2_DATA2  0xFFFFFFFF
#define EFUSE_KEY2_DATA2_M  ((EFUSE_KEY2_DATA2_V)<<(EFUSE_KEY2_DATA2_S))
#define EFUSE_KEY2_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA2_S  0

#define EFUSE_RD_KEY2_DATA3_REG          (DR_REG_EFUSE_BASE + 0x0e8)
/* EFUSE_KEY2_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY2_DATA3  0xFFFFFFFF
#define EFUSE_KEY2_DATA3_M  ((EFUSE_KEY2_DATA3_V)<<(EFUSE_KEY2_DATA3_S))
#define EFUSE_KEY2_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA3_S  0

#define EFUSE_RD_KEY2_DATA4_REG          (DR_REG_EFUSE_BASE + 0x0ec)
/* EFUSE_KEY2_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY2_DATA4  0xFFFFFFFF
#define EFUSE_KEY2_DATA4_M  ((EFUSE_KEY2_DATA4_V)<<(EFUSE_KEY2_DATA4_S))
#define EFUSE_KEY2_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA4_S  0

#define EFUSE_RD_KEY2_DATA5_REG          (DR_REG_EFUSE_BASE + 0x0f0)
/* EFUSE_KEY2_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY2_DATA5  0xFFFFFFFF
#define EFUSE_KEY2_DATA5_M  ((EFUSE_KEY2_DATA5_V)<<(EFUSE_KEY2_DATA5_S))
#define EFUSE_KEY2_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA5_S  0

#define EFUSE_RD_KEY2_DATA6_REG          (DR_REG_EFUSE_BASE + 0x0f4)
/* EFUSE_KEY2_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY2_DATA6  0xFFFFFFFF
#define EFUSE_KEY2_DATA6_M  ((EFUSE_KEY2_DATA6_V)<<(EFUSE_KEY2_DATA6_S))
#define EFUSE_KEY2_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA6_S  0

#define EFUSE_RD_KEY2_DATA7_REG          (DR_REG_EFUSE_BASE + 0x0f8)
/* EFUSE_KEY2_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY2_DATA7  0xFFFFFFFF
#define EFUSE_KEY2_DATA7_M  ((EFUSE_KEY2_DATA7_V)<<(EFUSE_KEY2_DATA7_S))
#define EFUSE_KEY2_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA7_S  0

#define EFUSE_RD_KEY3_DATA0_REG          (DR_REG_EFUSE_BASE + 0x0fc)
/* EFUSE_KEY3_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY3_DATA0  0xFFFFFFFF
#define EFUSE_KEY3_DATA0_M  ((EFUSE_KEY3_DATA0_V)<<(EFUSE_KEY3_DATA0_S))
#define EFUSE_KEY3_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA0_S  0

#define EFUSE_RD_KEY3_DATA1_REG          (DR_REG_EFUSE_BASE + 0x100)
/* EFUSE_KEY3_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY3_DATA1  0xFFFFFFFF
#define EFUSE_KEY3_DATA1_M  ((EFUSE_KEY3_DATA1_V)<<(EFUSE_KEY3_DATA1_S))
#define EFUSE_KEY3_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA1_S  0

#define EFUSE_RD_KEY3_DATA2_REG          (DR_REG_EFUSE_BASE + 0x104)
/* EFUSE_KEY3_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY3_DATA2  0xFFFFFFFF
#define EFUSE_KEY3_DATA2_M  ((EFUSE_KEY3_DATA2_V)<<(EFUSE_KEY3_DATA2_S))
#define EFUSE_KEY3_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA2_S  0

#define EFUSE_RD_KEY3_DATA3_REG          (DR_REG_EFUSE_BASE + 0x108)
/* EFUSE_KEY3_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY3_DATA3  0xFFFFFFFF
#define EFUSE_KEY3_DATA3_M  ((EFUSE_KEY3_DATA3_V)<<(EFUSE_KEY3_DATA3_S))
#define EFUSE_KEY3_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA3_S  0

#define EFUSE_RD_KEY3_DATA4_REG          (DR_REG_EFUSE_BASE + 0x10c)
/* EFUSE_KEY3_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY3_DATA4  0xFFFFFFFF
#define EFUSE_KEY3_DATA4_M  ((EFUSE_KEY3_DATA4_V)<<(EFUSE_KEY3_DATA4_S))
#define EFUSE_KEY3_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA4_S  0

#define EFUSE_RD_KEY3_DATA5_REG          (DR_REG_EFUSE_BASE + 0x110)
/* EFUSE_KEY3_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY3_DATA5  0xFFFFFFFF
#define EFUSE_KEY3_DATA5_M  ((EFUSE_KEY3_DATA5_V)<<(EFUSE_KEY3_DATA5_S))
#define EFUSE_KEY3_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA5_S  0

#define EFUSE_RD_KEY3_DATA6_REG          (DR_REG_EFUSE_BASE + 0x114)
/* EFUSE_KEY3_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY3_DATA6  0xFFFFFFFF
#define EFUSE_KEY3_DATA6_M  ((EFUSE_KEY3_DATA6_V)<<(EFUSE_KEY3_DATA6_S))
#define EFUSE_KEY3_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA6_S  0

#define EFUSE_RD_KEY3_DATA7_REG          (DR_REG_EFUSE_BASE + 0x118)
/* EFUSE_KEY3_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY3_DATA7  0xFFFFFFFF
#define EFUSE_KEY3_DATA7_M  ((EFUSE_KEY3_DATA7_V)<<(EFUSE_KEY3_DATA7_S))
#define EFUSE_KEY3_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA7_S  0

#define EFUSE_RD_KEY4_DATA0_REG          (DR_REG_EFUSE_BASE + 0x11c)
/* EFUSE_KEY4_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY4_DATA0  0xFFFFFFFF
#define EFUSE_KEY4_DATA0_M  ((EFUSE_KEY4_DATA0_V)<<(EFUSE_KEY4_DATA0_S))
#define EFUSE_KEY4_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA0_S  0

#define EFUSE_RD_KEY4_DATA1_REG          (DR_REG_EFUSE_BASE + 0x120)
/* EFUSE_KEY4_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY4_DATA1  0xFFFFFFFF
#define EFUSE_KEY4_DATA1_M  ((EFUSE_KEY4_DATA1_V)<<(EFUSE_KEY4_DATA1_S))
#define EFUSE_KEY4_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA1_S  0

#define EFUSE_RD_KEY4_DATA2_REG          (DR_REG_EFUSE_BASE + 0x124)
/* EFUSE_KEY4_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY4_DATA2  0xFFFFFFFF
#define EFUSE_KEY4_DATA2_M  ((EFUSE_KEY4_DATA2_V)<<(EFUSE_KEY4_DATA2_S))
#define EFUSE_KEY4_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA2_S  0

#define EFUSE_RD_KEY4_DATA3_REG          (DR_REG_EFUSE_BASE + 0x128)
/* EFUSE_KEY4_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY4_DATA3  0xFFFFFFFF
#define EFUSE_KEY4_DATA3_M  ((EFUSE_KEY4_DATA3_V)<<(EFUSE_KEY4_DATA3_S))
#define EFUSE_KEY4_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA3_S  0

#define EFUSE_RD_KEY4_DATA4_REG          (DR_REG_EFUSE_BASE + 0x12c)
/* EFUSE_KEY4_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY4_DATA4  0xFFFFFFFF
#define EFUSE_KEY4_DATA4_M  ((EFUSE_KEY4_DATA4_V)<<(EFUSE_KEY4_DATA4_S))
#define EFUSE_KEY4_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA4_S  0

#define EFUSE_RD_KEY4_DATA5_REG          (DR_REG_EFUSE_BASE + 0x130)
/* EFUSE_KEY4_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY4_DATA5  0xFFFFFFFF
#define EFUSE_KEY4_DATA5_M  ((EFUSE_KEY4_DATA5_V)<<(EFUSE_KEY4_DATA5_S))
#define EFUSE_KEY4_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA5_S  0

#define EFUSE_RD_KEY4_DATA6_REG          (DR_REG_EFUSE_BASE + 0x134)
/* EFUSE_KEY4_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY4_DATA6  0xFFFFFFFF
#define EFUSE_KEY4_DATA6_M  ((EFUSE_KEY4_DATA6_V)<<(EFUSE_KEY4_DATA6_S))
#define EFUSE_KEY4_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA6_S  0

#define EFUSE_RD_KEY4_DATA7_REG          (DR_REG_EFUSE_BASE + 0x138)
/* EFUSE_KEY4_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY4_DATA7  0xFFFFFFFF
#define EFUSE_KEY4_DATA7_M  ((EFUSE_KEY4_DATA7_V)<<(EFUSE_KEY4_DATA7_S))
#define EFUSE_KEY4_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA7_S  0

#define EFUSE_RD_KEY5_DATA0_REG          (DR_REG_EFUSE_BASE + 0x13c)
/* EFUSE_KEY5_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY5_DATA0  0xFFFFFFFF
#define EFUSE_KEY5_DATA0_M  ((EFUSE_KEY5_DATA0_V)<<(EFUSE_KEY5_DATA0_S))
#define EFUSE_KEY5_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA0_S  0

#define EFUSE_RD_KEY5_DATA1_REG          (DR_REG_EFUSE_BASE + 0x140)
/* EFUSE_KEY5_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY5_DATA1  0xFFFFFFFF
#define EFUSE_KEY5_DATA1_M  ((EFUSE_KEY5_DATA1_V)<<(EFUSE_KEY5_DATA1_S))
#define EFUSE_KEY5_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA1_S  0

#define EFUSE_RD_KEY5_DATA2_REG          (DR_REG_EFUSE_BASE + 0x144)
/* EFUSE_KEY5_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY5_DATA2  0xFFFFFFFF
#define EFUSE_KEY5_DATA2_M  ((EFUSE_KEY5_DATA2_V)<<(EFUSE_KEY5_DATA2_S))
#define EFUSE_KEY5_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA2_S  0

#define EFUSE_RD_KEY5_DATA3_REG          (DR_REG_EFUSE_BASE + 0x148)
/* EFUSE_KEY5_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY5_DATA3  0xFFFFFFFF
#define EFUSE_KEY5_DATA3_M  ((EFUSE_KEY5_DATA3_V)<<(EFUSE_KEY5_DATA3_S))
#define EFUSE_KEY5_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA3_S  0

#define EFUSE_RD_KEY5_DATA4_REG          (DR_REG_EFUSE_BASE + 0x14c)
/* EFUSE_KEY5_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY5_DATA4  0xFFFFFFFF
#define EFUSE_KEY5_DATA4_M  ((EFUSE_KEY5_DATA4_V)<<(EFUSE_KEY5_DATA4_S))
#define EFUSE_KEY5_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA4_S  0

#define EFUSE_RD_KEY5_DATA5_REG          (DR_REG_EFUSE_BASE + 0x150)
/* EFUSE_KEY5_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY5_DATA5  0xFFFFFFFF
#define EFUSE_KEY5_DATA5_M  ((EFUSE_KEY5_DATA5_V)<<(EFUSE_KEY5_DATA5_S))
#define EFUSE_KEY5_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA5_S  0

#define EFUSE_RD_KEY5_DATA6_REG          (DR_REG_EFUSE_BASE + 0x154)
/* EFUSE_KEY5_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY5_DATA6  0xFFFFFFFF
#define EFUSE_KEY5_DATA6_M  ((EFUSE_KEY5_DATA6_V)<<(EFUSE_KEY5_DATA6_S))
#define EFUSE_KEY5_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA6_S  0

#define EFUSE_RD_KEY5_DATA7_REG          (DR_REG_EFUSE_BASE + 0x158)
/* EFUSE_KEY5_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY5_DATA7  0xFFFFFFFF
#define EFUSE_KEY5_DATA7_M  ((EFUSE_KEY5_DATA7_V)<<(EFUSE_KEY5_DATA7_S))
#define EFUSE_KEY5_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA7_S  0

#define EFUSE_RD_KEY6_DATA0_REG          (DR_REG_EFUSE_BASE + 0x15c)
/* EFUSE_KEY6_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY6_DATA0  0xFFFFFFFF
#define EFUSE_KEY6_DATA0_M  ((EFUSE_KEY6_DATA0_V)<<(EFUSE_KEY6_DATA0_S))
#define EFUSE_KEY6_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY6_DATA0_S  0

#define EFUSE_RD_KEY6_DATA1_REG          (DR_REG_EFUSE_BASE + 0x160)
/* EFUSE_KEY6_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY6_DATA1  0xFFFFFFFF
#define EFUSE_KEY6_DATA1_M  ((EFUSE_KEY6_DATA1_V)<<(EFUSE_KEY6_DATA1_S))
#define EFUSE_KEY6_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY6_DATA1_S  0

#define EFUSE_RD_KEY6_DATA2_REG          (DR_REG_EFUSE_BASE + 0x164)
/* EFUSE_KEY6_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY6_DATA2  0xFFFFFFFF
#define EFUSE_KEY6_DATA2_M  ((EFUSE_KEY6_DATA2_V)<<(EFUSE_KEY6_DATA2_S))
#define EFUSE_KEY6_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY6_DATA2_S  0

#define EFUSE_RD_KEY6_DATA3_REG          (DR_REG_EFUSE_BASE + 0x168)
/* EFUSE_KEY6_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY6_DATA3  0xFFFFFFFF
#define EFUSE_KEY6_DATA3_M  ((EFUSE_KEY6_DATA3_V)<<(EFUSE_KEY6_DATA3_S))
#define EFUSE_KEY6_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY6_DATA3_S  0

#define EFUSE_RD_KEY6_DATA4_REG          (DR_REG_EFUSE_BASE + 0x16c)
/* EFUSE_KEY6_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY6_DATA4  0xFFFFFFFF
#define EFUSE_KEY6_DATA4_M  ((EFUSE_KEY6_DATA4_V)<<(EFUSE_KEY6_DATA4_S))
#define EFUSE_KEY6_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY6_DATA4_S  0

#define EFUSE_RD_KEY6_DATA5_REG          (DR_REG_EFUSE_BASE + 0x170)
/* EFUSE_KEY6_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY6_DATA5  0xFFFFFFFF
#define EFUSE_KEY6_DATA5_M  ((EFUSE_KEY6_DATA5_V)<<(EFUSE_KEY6_DATA5_S))
#define EFUSE_KEY6_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY6_DATA5_S  0

#define EFUSE_RD_KEY6_DATA6_REG          (DR_REG_EFUSE_BASE + 0x174)
/* EFUSE_KEY6_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY6_DATA6  0xFFFFFFFF
#define EFUSE_KEY6_DATA6_M  ((EFUSE_KEY6_DATA6_V)<<(EFUSE_KEY6_DATA6_S))
#define EFUSE_KEY6_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY6_DATA6_S  0

#define EFUSE_RD_KEY6_DATA7_REG          (DR_REG_EFUSE_BASE + 0x178)
/* EFUSE_KEY6_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_KEY6_DATA7  0xFFFFFFFF
#define EFUSE_KEY6_DATA7_M  ((EFUSE_KEY6_DATA7_V)<<(EFUSE_KEY6_DATA7_S))
#define EFUSE_KEY6_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY6_DATA7_S  0

#define EFUSE_RD_REPEAT_ERR0_REG          (DR_REG_EFUSE_BASE + 0x17c)
/* EFUSE_RD_SDIO_DREFH_ERR : RO ;bitpos:[31:30] ;default: 2'b0 ; */
/*description: */
#define EFUSE_RD_SDIO_DREFH_ERR  0x00000003
#define EFUSE_RD_SDIO_DREFH_ERR_M  ((EFUSE_RD_SDIO_DREFH_ERR_V)<<(EFUSE_RD_SDIO_DREFH_ERR_S))
#define EFUSE_RD_SDIO_DREFH_ERR_V  0x3
#define EFUSE_RD_SDIO_DREFH_ERR_S  30
/* EFUSE_RD_SDIO_MODECURLIM_ERR : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_SDIO_MODECURLIM_ERR  (BIT(29))
#define EFUSE_RD_SDIO_MODECURLIM_ERR_M  (BIT(29))
#define EFUSE_RD_SDIO_MODECURLIM_ERR_V  0x1
#define EFUSE_RD_SDIO_MODECURLIM_ERR_S  29
/* EFUSE_RD_USB_DRES_ERR : RO ;bitpos:[28:25] ;default: 4'b0 ; */
/*description: */
#define EFUSE_RD_USB_DRES_ERR  0x0000000F
#define EFUSE_RD_USB_DRES_ERR_M  ((EFUSE_RD_USB_DRES_ERR_V)<<(EFUSE_RD_USB_DRES_ERR_S))
#define EFUSE_RD_USB_DRES_ERR_V  0xF
#define EFUSE_RD_USB_DRES_ERR_S  25
/* EFUSE_RD_USB_EXCHG_PINS_ERR : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_USB_EXCHG_PINS_ERR  (BIT(24))
#define EFUSE_RD_USB_EXCHG_PINS_ERR_M  (BIT(24))
#define EFUSE_RD_USB_EXCHG_PINS_ERR_V  0x1
#define EFUSE_RD_USB_EXCHG_PINS_ERR_S  24
/* EFUSE_RD_USB_DREFL_ERR : RO ;bitpos:[23:22] ;default: 2'b0 ; */
/*description: */
#define EFUSE_RD_USB_DREFL_ERR  0x00000003
#define EFUSE_RD_USB_DREFL_ERR_M  ((EFUSE_RD_USB_DREFL_ERR_V)<<(EFUSE_RD_USB_DREFL_ERR_S))
#define EFUSE_RD_USB_DREFL_ERR_V  0x3
#define EFUSE_RD_USB_DREFL_ERR_S  22
/* EFUSE_RD_USB_DREFH_ERR : RO ;bitpos:[21:20] ;default: 2'b0 ; */
/*description: */
#define EFUSE_RD_USB_DREFH_ERR  0x00000003
#define EFUSE_RD_USB_DREFH_ERR_M  ((EFUSE_RD_USB_DREFH_ERR_V)<<(EFUSE_RD_USB_DREFH_ERR_S))
#define EFUSE_RD_USB_DREFH_ERR_V  0x3
#define EFUSE_RD_USB_DREFH_ERR_S  20
/* EFUSE_RD_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR  (BIT(19))
#define EFUSE_RD_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_M  (BIT(19))
#define EFUSE_RD_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_V  0x1
#define EFUSE_RD_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_S  19
/* EFUSE_RD_HARD_DIS_JTAG_ERR : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_HARD_DIS_JTAG_ERR  (BIT(18))
#define EFUSE_RD_HARD_DIS_JTAG_ERR_M  (BIT(18))
#define EFUSE_RD_HARD_DIS_JTAG_ERR_V  0x1
#define EFUSE_RD_HARD_DIS_JTAG_ERR_S  18
/* EFUSE_RD_SOFT_DIS_JTAG_ERR : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_SOFT_DIS_JTAG_ERR  (BIT(17))
#define EFUSE_RD_SOFT_DIS_JTAG_ERR_M  (BIT(17))
#define EFUSE_RD_SOFT_DIS_JTAG_ERR_V  0x1
#define EFUSE_RD_SOFT_DIS_JTAG_ERR_S  17
/* EFUSE_RD_DIS_EFUSE_ATE_WR_ERR : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_DIS_EFUSE_ATE_WR_ERR  (BIT(16))
#define EFUSE_RD_DIS_EFUSE_ATE_WR_ERR_M  (BIT(16))
#define EFUSE_RD_DIS_EFUSE_ATE_WR_ERR_V  0x1
#define EFUSE_RD_DIS_EFUSE_ATE_WR_ERR_S  16
/* EFUSE_RD_DIS_SDIO_ACCESS_ERR : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_DIS_SDIO_ACCESS_ERR  (BIT(15))
#define EFUSE_RD_DIS_SDIO_ACCESS_ERR_M  (BIT(15))
#define EFUSE_RD_DIS_SDIO_ACCESS_ERR_V  0x1
#define EFUSE_RD_DIS_SDIO_ACCESS_ERR_S  15
/* EFUSE_RD_DIS_CAN_ERR : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_DIS_CAN_ERR  (BIT(14))
#define EFUSE_RD_DIS_CAN_ERR_M  (BIT(14))
#define EFUSE_RD_DIS_CAN_ERR_V  0x1
#define EFUSE_RD_DIS_CAN_ERR_S  14
/* EFUSE_RD_DIS_USB_ERR : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_DIS_USB_ERR  (BIT(13))
#define EFUSE_RD_DIS_USB_ERR_M  (BIT(13))
#define EFUSE_RD_DIS_USB_ERR_V  0x1
#define EFUSE_RD_DIS_USB_ERR_S  13
/* EFUSE_RD_DIS_BT_ERR : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_DIS_BT_ERR  (BIT(12))
#define EFUSE_RD_DIS_BT_ERR_M  (BIT(12))
#define EFUSE_RD_DIS_BT_ERR_V  0x1
#define EFUSE_RD_DIS_BT_ERR_S  12
/* EFUSE_RD_DIS_DOWNLOAD_DCACHE_ERR : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_DIS_DOWNLOAD_DCACHE_ERR  (BIT(11))
#define EFUSE_RD_DIS_DOWNLOAD_DCACHE_ERR_M  (BIT(11))
#define EFUSE_RD_DIS_DOWNLOAD_DCACHE_ERR_V  0x1
#define EFUSE_RD_DIS_DOWNLOAD_DCACHE_ERR_S  11
/* EFUSE_RD_DIS_DOWNLOAD_ICACHE_ERR : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_DIS_DOWNLOAD_ICACHE_ERR  (BIT(10))
#define EFUSE_RD_DIS_DOWNLOAD_ICACHE_ERR_M  (BIT(10))
#define EFUSE_RD_DIS_DOWNLOAD_ICACHE_ERR_V  0x1
#define EFUSE_RD_DIS_DOWNLOAD_ICACHE_ERR_S  10
/* EFUSE_RD_DIS_DCACHE_ERR : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_DIS_DCACHE_ERR  (BIT(9))
#define EFUSE_RD_DIS_DCACHE_ERR_M  (BIT(9))
#define EFUSE_RD_DIS_DCACHE_ERR_V  0x1
#define EFUSE_RD_DIS_DCACHE_ERR_S  9
/* EFUSE_RD_DIS_ICACHE_ERR : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_DIS_ICACHE_ERR  (BIT(8))
#define EFUSE_RD_DIS_ICACHE_ERR_M  (BIT(8))
#define EFUSE_RD_DIS_ICACHE_ERR_V  0x1
#define EFUSE_RD_DIS_ICACHE_ERR_S  8
/* EFUSE_RD_DIS_RTC_RAM_BOOT_ERR : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_DIS_RTC_RAM_BOOT_ERR  (BIT(7))
#define EFUSE_RD_DIS_RTC_RAM_BOOT_ERR_M  (BIT(7))
#define EFUSE_RD_DIS_RTC_RAM_BOOT_ERR_V  0x1
#define EFUSE_RD_DIS_RTC_RAM_BOOT_ERR_S  7
/* EFUSE_RD_RD_DIS_ERR : RO ;bitpos:[6:0] ;default: 7'h0 ; */
/*description: */
#define EFUSE_RD_RD_DIS_ERR  0x0000007F
#define EFUSE_RD_RD_DIS_ERR_M  ((EFUSE_RD_RD_DIS_ERR_V)<<(EFUSE_RD_RD_DIS_ERR_S))
#define EFUSE_RD_RD_DIS_ERR_V  0x7F
#define EFUSE_RD_RD_DIS_ERR_S  0

#define EFUSE_RD_REPEAT_ERR1_REG          (DR_REG_EFUSE_BASE + 0x180)
/* EFUSE_RD_KEY_PURPOSE_1_ERR : RO ;bitpos:[31:28] ;default: 4'b0 ; */
/*description: */
#define EFUSE_RD_KEY_PURPOSE_1_ERR  0x0000000F
#define EFUSE_RD_KEY_PURPOSE_1_ERR_M  ((EFUSE_RD_KEY_PURPOSE_1_ERR_V)<<(EFUSE_RD_KEY_PURPOSE_1_ERR_S))
#define EFUSE_RD_KEY_PURPOSE_1_ERR_V  0xF
#define EFUSE_RD_KEY_PURPOSE_1_ERR_S  28
/* EFUSE_RD_KEY_PURPOSE_0_ERR : RO ;bitpos:[27:24] ;default: 4'b0 ; */
/*description: */
#define EFUSE_RD_KEY_PURPOSE_0_ERR  0x0000000F
#define EFUSE_RD_KEY_PURPOSE_0_ERR_M  ((EFUSE_RD_KEY_PURPOSE_0_ERR_V)<<(EFUSE_RD_KEY_PURPOSE_0_ERR_S))
#define EFUSE_RD_KEY_PURPOSE_0_ERR_V  0xF
#define EFUSE_RD_KEY_PURPOSE_0_ERR_S  24
/* EFUSE_RD_SECURE_BOOT_KEY_REVOKE2_ERR : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_SECURE_BOOT_KEY_REVOKE2_ERR  (BIT(23))
#define EFUSE_RD_SECURE_BOOT_KEY_REVOKE2_ERR_M  (BIT(23))
#define EFUSE_RD_SECURE_BOOT_KEY_REVOKE2_ERR_V  0x1
#define EFUSE_RD_SECURE_BOOT_KEY_REVOKE2_ERR_S  23
/* EFUSE_RD_SECURE_BOOT_KEY_REVOKE1_ERR : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_SECURE_BOOT_KEY_REVOKE1_ERR  (BIT(22))
#define EFUSE_RD_SECURE_BOOT_KEY_REVOKE1_ERR_M  (BIT(22))
#define EFUSE_RD_SECURE_BOOT_KEY_REVOKE1_ERR_V  0x1
#define EFUSE_RD_SECURE_BOOT_KEY_REVOKE1_ERR_S  22
/* EFUSE_RD_SECURE_BOOT_KEY_REVOKE0_ERR : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_SECURE_BOOT_KEY_REVOKE0_ERR  (BIT(21))
#define EFUSE_RD_SECURE_BOOT_KEY_REVOKE0_ERR_M  (BIT(21))
#define EFUSE_RD_SECURE_BOOT_KEY_REVOKE0_ERR_V  0x1
#define EFUSE_RD_SECURE_BOOT_KEY_REVOKE0_ERR_S  21
/* EFUSE_RD_SPI_BOOT_CRYPT_CNT_ERR : RO ;bitpos:[20:18] ;default: 3'b0 ; */
/*description: */
#define EFUSE_RD_SPI_BOOT_CRYPT_CNT_ERR  0x00000007
#define EFUSE_RD_SPI_BOOT_CRYPT_CNT_ERR_M  ((EFUSE_RD_SPI_BOOT_CRYPT_CNT_ERR_V)<<(EFUSE_RD_SPI_BOOT_CRYPT_CNT_ERR_S))
#define EFUSE_RD_SPI_BOOT_CRYPT_CNT_ERR_V  0x7
#define EFUSE_RD_SPI_BOOT_CRYPT_CNT_ERR_S  18
/* EFUSE_RD_WDT_DELAY_SEL_ERR : RO ;bitpos:[17:16] ;default: 2'b0 ; */
/*description: */
#define EFUSE_RD_WDT_DELAY_SEL_ERR  0x00000003
#define EFUSE_RD_WDT_DELAY_SEL_ERR_M  ((EFUSE_RD_WDT_DELAY_SEL_ERR_V)<<(EFUSE_RD_WDT_DELAY_SEL_ERR_S))
#define EFUSE_RD_WDT_DELAY_SEL_ERR_V  0x3
#define EFUSE_RD_WDT_DELAY_SEL_ERR_S  16
/* EFUSE_RD_SDIO_DCAP_ERR : RO ;bitpos:[15:14] ;default: 2'b0 ; */
/*description: */
#define EFUSE_RD_SDIO_DCAP_ERR  0x00000003
#define EFUSE_RD_SDIO_DCAP_ERR_M  ((EFUSE_RD_SDIO_DCAP_ERR_V)<<(EFUSE_RD_SDIO_DCAP_ERR_S))
#define EFUSE_RD_SDIO_DCAP_ERR_V  0x3
#define EFUSE_RD_SDIO_DCAP_ERR_S  14
/* EFUSE_RD_SDIO_INIT_ERR : RO ;bitpos:[13:12] ;default: 2'b0 ; */
/*description: */
#define EFUSE_RD_SDIO_INIT_ERR  0x00000003
#define EFUSE_RD_SDIO_INIT_ERR_M  ((EFUSE_RD_SDIO_INIT_ERR_V)<<(EFUSE_RD_SDIO_INIT_ERR_S))
#define EFUSE_RD_SDIO_INIT_ERR_V  0x3
#define EFUSE_RD_SDIO_INIT_ERR_S  12
/* EFUSE_RD_SDIO_DCURLIM_ERR : RO ;bitpos:[11:9] ;default: 3'b0 ; */
/*description: */
#define EFUSE_RD_SDIO_DCURLIM_ERR  0x00000007
#define EFUSE_RD_SDIO_DCURLIM_ERR_M  ((EFUSE_RD_SDIO_DCURLIM_ERR_V)<<(EFUSE_RD_SDIO_DCURLIM_ERR_S))
#define EFUSE_RD_SDIO_DCURLIM_ERR_V  0x7
#define EFUSE_RD_SDIO_DCURLIM_ERR_S  9
/* EFUSE_RD_SDIO_ENCURLIM_ERR : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_SDIO_ENCURLIM_ERR  (BIT(8))
#define EFUSE_RD_SDIO_ENCURLIM_ERR_M  (BIT(8))
#define EFUSE_RD_SDIO_ENCURLIM_ERR_V  0x1
#define EFUSE_RD_SDIO_ENCURLIM_ERR_S  8
/* EFUSE_RD_SDIO_EN_INIT_ERR : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_SDIO_EN_INIT_ERR  (BIT(7))
#define EFUSE_RD_SDIO_EN_INIT_ERR_M  (BIT(7))
#define EFUSE_RD_SDIO_EN_INIT_ERR_V  0x1
#define EFUSE_RD_SDIO_EN_INIT_ERR_S  7
/* EFUSE_RD_SDIO_FORCE_ERR : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_SDIO_FORCE_ERR  (BIT(6))
#define EFUSE_RD_SDIO_FORCE_ERR_M  (BIT(6))
#define EFUSE_RD_SDIO_FORCE_ERR_V  0x1
#define EFUSE_RD_SDIO_FORCE_ERR_S  6
/* EFUSE_RD_SDIO_TIEH_ERR : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_SDIO_TIEH_ERR  (BIT(5))
#define EFUSE_RD_SDIO_TIEH_ERR_M  (BIT(5))
#define EFUSE_RD_SDIO_TIEH_ERR_V  0x1
#define EFUSE_RD_SDIO_TIEH_ERR_S  5
/* EFUSE_RD_SDIO_XPD_ERR : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_SDIO_XPD_ERR  (BIT(4))
#define EFUSE_RD_SDIO_XPD_ERR_M  (BIT(4))
#define EFUSE_RD_SDIO_XPD_ERR_V  0x1
#define EFUSE_RD_SDIO_XPD_ERR_S  4
/* EFUSE_RD_SDIO_DREFL_ERR : RO ;bitpos:[3:2] ;default: 2'b0 ; */
/*description: */
#define EFUSE_RD_SDIO_DREFL_ERR  0x00000003
#define EFUSE_RD_SDIO_DREFL_ERR_M  ((EFUSE_RD_SDIO_DREFL_ERR_V)<<(EFUSE_RD_SDIO_DREFL_ERR_S))
#define EFUSE_RD_SDIO_DREFL_ERR_V  0x3
#define EFUSE_RD_SDIO_DREFL_ERR_S  2
/* EFUSE_RD_SDIO_DREFM_ERR : RO ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: */
#define EFUSE_RD_SDIO_DREFM_ERR  0x00000003
#define EFUSE_RD_SDIO_DREFM_ERR_M  ((EFUSE_RD_SDIO_DREFM_ERR_V)<<(EFUSE_RD_SDIO_DREFM_ERR_S))
#define EFUSE_RD_SDIO_DREFM_ERR_V  0x3
#define EFUSE_RD_SDIO_DREFM_ERR_S  0

#define EFUSE_RD_REPEAT_ERR2_REG          (DR_REG_EFUSE_BASE + 0x184)
/* EFUSE_RD_FLASH_TPUW_ERR : RO ;bitpos:[31:28] ;default: 4'b0 ; */
/*description: */
#define EFUSE_RD_FLASH_TPUW_ERR  0x0000000F
#define EFUSE_RD_FLASH_TPUW_ERR_M  ((EFUSE_RD_FLASH_TPUW_ERR_V)<<(EFUSE_RD_FLASH_TPUW_ERR_S))
#define EFUSE_RD_FLASH_TPUW_ERR_V  0xF
#define EFUSE_RD_FLASH_TPUW_ERR_S  28
/* EFUSE_RD_XTAL_FREQ_ERR : RO ;bitpos:[27:22] ;default: 6'b0 ; */
/*description: */
#define EFUSE_RD_XTAL_FREQ_ERR  0x0000003F
#define EFUSE_RD_XTAL_FREQ_ERR_M  ((EFUSE_RD_XTAL_FREQ_ERR_V)<<(EFUSE_RD_XTAL_FREQ_ERR_S))
#define EFUSE_RD_XTAL_FREQ_ERR_V  0x3F
#define EFUSE_RD_XTAL_FREQ_ERR_S  22
/* EFUSE_RD_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR  (BIT(21))
#define EFUSE_RD_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_M  (BIT(21))
#define EFUSE_RD_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_V  0x1
#define EFUSE_RD_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_S  21
/* EFUSE_RD_SECURE_BOOT_EN_ERR : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_SECURE_BOOT_EN_ERR  (BIT(20))
#define EFUSE_RD_SECURE_BOOT_EN_ERR_M  (BIT(20))
#define EFUSE_RD_SECURE_BOOT_EN_ERR_V  0x1
#define EFUSE_RD_SECURE_BOOT_EN_ERR_S  20
/* EFUSE_RD_KEY_PURPOSE_6_ERR : RO ;bitpos:[19:16] ;default: 4'b0 ; */
/*description: */
#define EFUSE_RD_KEY_PURPOSE_6_ERR  0x0000000F
#define EFUSE_RD_KEY_PURPOSE_6_ERR_M  ((EFUSE_RD_KEY_PURPOSE_6_ERR_V)<<(EFUSE_RD_KEY_PURPOSE_6_ERR_S))
#define EFUSE_RD_KEY_PURPOSE_6_ERR_V  0xF
#define EFUSE_RD_KEY_PURPOSE_6_ERR_S  16
/* EFUSE_RD_KEY_PURPOSE_5_ERR : RO ;bitpos:[15:12] ;default: 4'b0 ; */
/*description: */
#define EFUSE_RD_KEY_PURPOSE_5_ERR  0x0000000F
#define EFUSE_RD_KEY_PURPOSE_5_ERR_M  ((EFUSE_RD_KEY_PURPOSE_5_ERR_V)<<(EFUSE_RD_KEY_PURPOSE_5_ERR_S))
#define EFUSE_RD_KEY_PURPOSE_5_ERR_V  0xF
#define EFUSE_RD_KEY_PURPOSE_5_ERR_S  12
/* EFUSE_RD_KEY_PURPOSE_4_ERR : RO ;bitpos:[11:8] ;default: 4'b0 ; */
/*description: */
#define EFUSE_RD_KEY_PURPOSE_4_ERR  0x0000000F
#define EFUSE_RD_KEY_PURPOSE_4_ERR_M  ((EFUSE_RD_KEY_PURPOSE_4_ERR_V)<<(EFUSE_RD_KEY_PURPOSE_4_ERR_S))
#define EFUSE_RD_KEY_PURPOSE_4_ERR_V  0xF
#define EFUSE_RD_KEY_PURPOSE_4_ERR_S  8
/* EFUSE_RD_KEY_PURPOSE_3_ERR : RO ;bitpos:[7:4] ;default: 4'b0 ; */
/*description: */
#define EFUSE_RD_KEY_PURPOSE_3_ERR  0x0000000F
#define EFUSE_RD_KEY_PURPOSE_3_ERR_M  ((EFUSE_RD_KEY_PURPOSE_3_ERR_V)<<(EFUSE_RD_KEY_PURPOSE_3_ERR_S))
#define EFUSE_RD_KEY_PURPOSE_3_ERR_V  0xF
#define EFUSE_RD_KEY_PURPOSE_3_ERR_S  4
/* EFUSE_RD_KEY_PURPOSE_2_ERR : RO ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: */
#define EFUSE_RD_KEY_PURPOSE_2_ERR  0x0000000F
#define EFUSE_RD_KEY_PURPOSE_2_ERR_M  ((EFUSE_RD_KEY_PURPOSE_2_ERR_V)<<(EFUSE_RD_KEY_PURPOSE_2_ERR_S))
#define EFUSE_RD_KEY_PURPOSE_2_ERR_V  0xF
#define EFUSE_RD_KEY_PURPOSE_2_ERR_S  0

#define EFUSE_RD_REPEAT_ERR3_REG          (DR_REG_EFUSE_BASE + 0x188)
/* EFUSE_RD_RESERVE_ERR : RO ;bitpos:[31:8] ;default: 24'h0 ; */
/*description: */
#define EFUSE_RD_RESERVE_ERR  0x00FFFFFF
#define EFUSE_RD_RESERVE_ERR_M  ((EFUSE_RD_RESERVE_ERR_V)<<(EFUSE_RD_RESERVE_ERR_S))
#define EFUSE_RD_RESERVE_ERR_V  0xFFFFFF
#define EFUSE_RD_RESERVE_ERR_S  8
/* EFUSE_RD_UART_PRINT_CONTROL : RO ;bitpos:[7:6] ;default: 2'b0 ; */
/*description: */
#define EFUSE_RD_UART_PRINT_CONTROL  0x00000003
#define EFUSE_RD_UART_PRINT_CONTROL_M  ((EFUSE_RD_UART_PRINT_CONTROL_V)<<(EFUSE_RD_UART_PRINT_CONTROL_S))
#define EFUSE_RD_UART_PRINT_CONTROL_V  0x3
#define EFUSE_RD_UART_PRINT_CONTROL_S  6
/* EFUSE_RD_ENABLE_SECURITY_DOWNLOAD : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_ENABLE_SECURITY_DOWNLOAD  (BIT(5))
#define EFUSE_RD_ENABLE_SECURITY_DOWNLOAD_M  (BIT(5))
#define EFUSE_RD_ENABLE_SECURITY_DOWNLOAD_V  0x1
#define EFUSE_RD_ENABLE_SECURITY_DOWNLOAD_S  5
/* EFUSE_RD_DIS_USB_DOWNLOAD_MODE : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_DIS_USB_DOWNLOAD_MODE  (BIT(4))
#define EFUSE_RD_DIS_USB_DOWNLOAD_MODE_M  (BIT(4))
#define EFUSE_RD_DIS_USB_DOWNLOAD_MODE_V  0x1
#define EFUSE_RD_DIS_USB_DOWNLOAD_MODE_S  4
/* EFUSE_RD_DIS_TINY_BASIC : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_DIS_TINY_BASIC  (BIT(3))
#define EFUSE_RD_DIS_TINY_BASIC_M  (BIT(3))
#define EFUSE_RD_DIS_TINY_BASIC_V  0x1
#define EFUSE_RD_DIS_TINY_BASIC_S  3
/* EFUSE_RD_UART_PRINT_CHANNEL : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_UART_PRINT_CHANNEL  (BIT(2))
#define EFUSE_RD_UART_PRINT_CHANNEL_M  (BIT(2))
#define EFUSE_RD_UART_PRINT_CHANNEL_V  0x1
#define EFUSE_RD_UART_PRINT_CHANNEL_S  2
/* EFUSE_RD_DIS_LEGACY_SPI_BOOT_ERR : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_DIS_LEGACY_SPI_BOOT_ERR  (BIT(1))
#define EFUSE_RD_DIS_LEGACY_SPI_BOOT_ERR_M  (BIT(1))
#define EFUSE_RD_DIS_LEGACY_SPI_BOOT_ERR_V  0x1
#define EFUSE_RD_DIS_LEGACY_SPI_BOOT_ERR_S  1
/* EFUSE_RD_DIS_DOWNLOAD_MODE_ERR : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_DIS_DOWNLOAD_MODE_ERR  (BIT(0))
#define EFUSE_RD_DIS_DOWNLOAD_MODE_ERR_M  (BIT(0))
#define EFUSE_RD_DIS_DOWNLOAD_MODE_ERR_V  0x1
#define EFUSE_RD_DIS_DOWNLOAD_MODE_ERR_S  0

#define EFUSE_RD_REPEAT_ERR4_REG          (DR_REG_EFUSE_BASE + 0x190)
/* EFUSE_RD_CHIP_VERSION_ERR : RO ;bitpos:[23:0] ;default: 24'h0 ; */
/*description: */
#define EFUSE_RD_CHIP_VERSION_ERR  0x00FFFFFF
#define EFUSE_RD_CHIP_VERSION_ERR_M  ((EFUSE_RD_CHIP_VERSION_ERR_V)<<(EFUSE_RD_CHIP_VERSION_ERR_S))
#define EFUSE_RD_CHIP_VERSION_ERR_V  0xFFFFFF
#define EFUSE_RD_CHIP_VERSION_ERR_S  0

#define EFUSE_RD_RS_ERR0_REG          (DR_REG_EFUSE_BASE + 0x194)
/* EFUSE_RD_KEY4_FAIL : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_KEY4_FAIL  (BIT(31))
#define EFUSE_RD_KEY4_FAIL_M  (BIT(31))
#define EFUSE_RD_KEY4_FAIL_V  0x1
#define EFUSE_RD_KEY4_FAIL_S  31
/* EFUSE_RD_KEY4_ERR_NUM : RO ;bitpos:[30:28] ;default: 3'b0 ; */
/*description: */
#define EFUSE_RD_KEY4_ERR_NUM  0x00000007
#define EFUSE_RD_KEY4_ERR_NUM_M  ((EFUSE_RD_KEY4_ERR_NUM_V)<<(EFUSE_RD_KEY4_ERR_NUM_S))
#define EFUSE_RD_KEY4_ERR_NUM_V  0x7
#define EFUSE_RD_KEY4_ERR_NUM_S  28
/* EFUSE_RD_KEY3_FAIL : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_KEY3_FAIL  (BIT(27))
#define EFUSE_RD_KEY3_FAIL_M  (BIT(27))
#define EFUSE_RD_KEY3_FAIL_V  0x1
#define EFUSE_RD_KEY3_FAIL_S  27
/* EFUSE_RD_KEY3_ERR_NUM : RO ;bitpos:[26:24] ;default: 3'b0 ; */
/*description: */
#define EFUSE_RD_KEY3_ERR_NUM  0x00000007
#define EFUSE_RD_KEY3_ERR_NUM_M  ((EFUSE_RD_KEY3_ERR_NUM_V)<<(EFUSE_RD_KEY3_ERR_NUM_S))
#define EFUSE_RD_KEY3_ERR_NUM_V  0x7
#define EFUSE_RD_KEY3_ERR_NUM_S  24
/* EFUSE_RD_KEY2_FAIL : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_KEY2_FAIL  (BIT(23))
#define EFUSE_RD_KEY2_FAIL_M  (BIT(23))
#define EFUSE_RD_KEY2_FAIL_V  0x1
#define EFUSE_RD_KEY2_FAIL_S  23
/* EFUSE_RD_KEY2_ERR_NUM : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: */
#define EFUSE_RD_KEY2_ERR_NUM  0x00000007
#define EFUSE_RD_KEY2_ERR_NUM_M  ((EFUSE_RD_KEY2_ERR_NUM_V)<<(EFUSE_RD_KEY2_ERR_NUM_S))
#define EFUSE_RD_KEY2_ERR_NUM_V  0x7
#define EFUSE_RD_KEY2_ERR_NUM_S  20
/* EFUSE_RD_KEY1_FAIL : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_KEY1_FAIL  (BIT(19))
#define EFUSE_RD_KEY1_FAIL_M  (BIT(19))
#define EFUSE_RD_KEY1_FAIL_V  0x1
#define EFUSE_RD_KEY1_FAIL_S  19
/* EFUSE_RD_KEY1_ERR_NUM : RO ;bitpos:[18:16] ;default: 3'b0 ; */
/*description: */
#define EFUSE_RD_KEY1_ERR_NUM  0x00000007
#define EFUSE_RD_KEY1_ERR_NUM_M  ((EFUSE_RD_KEY1_ERR_NUM_V)<<(EFUSE_RD_KEY1_ERR_NUM_S))
#define EFUSE_RD_KEY1_ERR_NUM_V  0x7
#define EFUSE_RD_KEY1_ERR_NUM_S  16
/* EFUSE_RD_KEY0_FAIL : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_KEY0_FAIL  (BIT(15))
#define EFUSE_RD_KEY0_FAIL_M  (BIT(15))
#define EFUSE_RD_KEY0_FAIL_V  0x1
#define EFUSE_RD_KEY0_FAIL_S  15
/* EFUSE_RD_KEY0_ERR_NUM : RO ;bitpos:[14:12] ;default: 3'b0 ; */
/*description: */
#define EFUSE_RD_KEY0_ERR_NUM  0x00000007
#define EFUSE_RD_KEY0_ERR_NUM_M  ((EFUSE_RD_KEY0_ERR_NUM_V)<<(EFUSE_RD_KEY0_ERR_NUM_S))
#define EFUSE_RD_KEY0_ERR_NUM_V  0x7
#define EFUSE_RD_KEY0_ERR_NUM_S  12
/* EFUSE_RD_USR_DATA_FAIL : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_USR_DATA_FAIL  (BIT(11))
#define EFUSE_RD_USR_DATA_FAIL_M  (BIT(11))
#define EFUSE_RD_USR_DATA_FAIL_V  0x1
#define EFUSE_RD_USR_DATA_FAIL_S  11
/* EFUSE_RD_USR_DATA_ERR_NUM : RO ;bitpos:[10:8] ;default: 3'b0 ; */
/*description: */
#define EFUSE_RD_USR_DATA_ERR_NUM  0x00000007
#define EFUSE_RD_USR_DATA_ERR_NUM_M  ((EFUSE_RD_USR_DATA_ERR_NUM_V)<<(EFUSE_RD_USR_DATA_ERR_NUM_S))
#define EFUSE_RD_USR_DATA_ERR_NUM_V  0x7
#define EFUSE_RD_USR_DATA_ERR_NUM_S  8
/* EFUSE_RD_SYS_ERR_FAIL : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_SYS_ERR_FAIL  (BIT(7))
#define EFUSE_RD_SYS_ERR_FAIL_M  (BIT(7))
#define EFUSE_RD_SYS_ERR_FAIL_V  0x1
#define EFUSE_RD_SYS_ERR_FAIL_S  7
/* EFUSE_RD_SYS_ERR_NUM : RO ;bitpos:[6:4] ;default: 3'b0 ; */
/*description: */
#define EFUSE_RD_SYS_ERR_NUM  0x00000007
#define EFUSE_RD_SYS_ERR_NUM_M  ((EFUSE_RD_SYS_ERR_NUM_V)<<(EFUSE_RD_SYS_ERR_NUM_S))
#define EFUSE_RD_SYS_ERR_NUM_V  0x7
#define EFUSE_RD_SYS_ERR_NUM_S  4
/* EFUSE_RD_MAC_SPI_8M_FAIL : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_MAC_SPI_8M_FAIL  (BIT(3))
#define EFUSE_RD_MAC_SPI_8M_FAIL_M  (BIT(3))
#define EFUSE_RD_MAC_SPI_8M_FAIL_V  0x1
#define EFUSE_RD_MAC_SPI_8M_FAIL_S  3
/* EFUSE_RD_MAC_SPI_8M_ERR_NUM : RO ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: */
#define EFUSE_RD_MAC_SPI_8M_ERR_NUM  0x00000007
#define EFUSE_RD_MAC_SPI_8M_ERR_NUM_M  ((EFUSE_RD_MAC_SPI_8M_ERR_NUM_V)<<(EFUSE_RD_MAC_SPI_8M_ERR_NUM_S))
#define EFUSE_RD_MAC_SPI_8M_ERR_NUM_V  0x7
#define EFUSE_RD_MAC_SPI_8M_ERR_NUM_S  0

#define EFUSE_RD_RS_ERR1_REG          (DR_REG_EFUSE_BASE + 0x198)
/* EFUSE_RD_KEY6_FAIL : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_KEY6_FAIL  (BIT(7))
#define EFUSE_RD_KEY6_FAIL_M  (BIT(7))
#define EFUSE_RD_KEY6_FAIL_V  0x1
#define EFUSE_RD_KEY6_FAIL_S  7
/* EFUSE_RD_KEY6_ERR_NUM : RO ;bitpos:[6:4] ;default: 3'b0 ; */
/*description: */
#define EFUSE_RD_KEY6_ERR_NUM  0x00000007
#define EFUSE_RD_KEY6_ERR_NUM_M  ((EFUSE_RD_KEY6_ERR_NUM_V)<<(EFUSE_RD_KEY6_ERR_NUM_S))
#define EFUSE_RD_KEY6_ERR_NUM_V  0x7
#define EFUSE_RD_KEY6_ERR_NUM_S  4
/* EFUSE_RD_KEY5_FAIL : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_KEY5_FAIL  (BIT(3))
#define EFUSE_RD_KEY5_FAIL_M  (BIT(3))
#define EFUSE_RD_KEY5_FAIL_V  0x1
#define EFUSE_RD_KEY5_FAIL_S  3
/* EFUSE_RD_KEY5_ERR_NUM : RO ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: */
#define EFUSE_RD_KEY5_ERR_NUM  0x00000007
#define EFUSE_RD_KEY5_ERR_NUM_M  ((EFUSE_RD_KEY5_ERR_NUM_V)<<(EFUSE_RD_KEY5_ERR_NUM_S))
#define EFUSE_RD_KEY5_ERR_NUM_V  0x7
#define EFUSE_RD_KEY5_ERR_NUM_S  0

#define EFUSE_CLK_REG          (DR_REG_EFUSE_BASE + 0x19c)
/* EFUSE_CLK_EN : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define EFUSE_CLK_EN  (BIT(16))
#define EFUSE_CLK_EN_M  (BIT(16))
#define EFUSE_CLK_EN_V  0x1
#define EFUSE_CLK_EN_S  16
/* EFUSE_MEM_PD : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define EFUSE_MEM_PD  (BIT(0))
#define EFUSE_MEM_PD_M  (BIT(0))
#define EFUSE_MEM_PD_V  0x1
#define EFUSE_MEM_PD_S  0

#define EFUSE_CONF_REG          (DR_REG_EFUSE_BASE + 0x1a0)
/* EFUSE_OP_CODE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: */
#define EFUSE_OP_CODE  0x0000FFFF
#define EFUSE_OP_CODE_M  ((EFUSE_OP_CODE_V)<<(EFUSE_OP_CODE_S))
#define EFUSE_OP_CODE_V  0xFFFF
#define EFUSE_OP_CODE_S  0

#define EFUSE_STATUS_REG          (DR_REG_EFUSE_BASE + 0x1a4)
/* EFUSE_REPEAT_ERR_CNT : RO ;bitpos:[16:9] ;default: 8'h0 ; */
/*description: */
#define EFUSE_REPEAT_ERR_CNT  0x000000FF
#define EFUSE_REPEAT_ERR_CNT_M  ((EFUSE_REPEAT_ERR_CNT_V)<<(EFUSE_REPEAT_ERR_CNT_S))
#define EFUSE_REPEAT_ERR_CNT_V  0xFF
#define EFUSE_REPEAT_ERR_CNT_S  9
/* EFUSE_OTP_VDDQ_IS_SW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define EFUSE_OTP_VDDQ_IS_SW  (BIT(8))
#define EFUSE_OTP_VDDQ_IS_SW_M  (BIT(8))
#define EFUSE_OTP_VDDQ_IS_SW_V  0x1
#define EFUSE_OTP_VDDQ_IS_SW_S  8
/* EFUSE_OTP_PGENB_SW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define EFUSE_OTP_PGENB_SW  (BIT(7))
#define EFUSE_OTP_PGENB_SW_M  (BIT(7))
#define EFUSE_OTP_PGENB_SW_V  0x1
#define EFUSE_OTP_PGENB_SW_S  7
/* EFUSE_OTP_CSB_SW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define EFUSE_OTP_CSB_SW  (BIT(6))
#define EFUSE_OTP_CSB_SW_M  (BIT(6))
#define EFUSE_OTP_CSB_SW_V  0x1
#define EFUSE_OTP_CSB_SW_S  6
/* EFUSE_OTP_STROBE_SW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define EFUSE_OTP_STROBE_SW  (BIT(5))
#define EFUSE_OTP_STROBE_SW_M  (BIT(5))
#define EFUSE_OTP_STROBE_SW_V  0x1
#define EFUSE_OTP_STROBE_SW_S  5
/* EFUSE_OTP_VDDQ_C_SYNC2 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define EFUSE_OTP_VDDQ_C_SYNC2  (BIT(4))
#define EFUSE_OTP_VDDQ_C_SYNC2_M  (BIT(4))
#define EFUSE_OTP_VDDQ_C_SYNC2_V  0x1
#define EFUSE_OTP_VDDQ_C_SYNC2_S  4
/* EFUSE_OTP_LOAD_SW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define EFUSE_OTP_LOAD_SW  (BIT(3))
#define EFUSE_OTP_LOAD_SW_M  (BIT(3))
#define EFUSE_OTP_LOAD_SW_V  0x1
#define EFUSE_OTP_LOAD_SW_S  3
/* EFUSE_STATE : RO ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: */
#define EFUSE_STATE  0x00000007
#define EFUSE_STATE_M  ((EFUSE_STATE_V)<<(EFUSE_STATE_S))
#define EFUSE_STATE_V  0x7
#define EFUSE_STATE_S  0

#define EFUSE_CMD_REG          (DR_REG_EFUSE_BASE + 0x1a8)
/* EFUSE_BLK_NUM : R/W ;bitpos:[5:2] ;default: 4'b0 ; */
/*description: */
#define EFUSE_BLK_NUM  0x0000000F
#define EFUSE_BLK_NUM_M  ((EFUSE_BLK_NUM_V)<<(EFUSE_BLK_NUM_S))
#define EFUSE_BLK_NUM_V  0xF
#define EFUSE_BLK_NUM_S  2
/* EFUSE_PGM_CMD : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define EFUSE_PGM_CMD  (BIT(1))
#define EFUSE_PGM_CMD_M  (BIT(1))
#define EFUSE_PGM_CMD_V  0x1
#define EFUSE_PGM_CMD_S  1
/* EFUSE_READ_CMD : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define EFUSE_READ_CMD  (BIT(0))
#define EFUSE_READ_CMD_M  (BIT(0))
#define EFUSE_READ_CMD_V  0x1
#define EFUSE_READ_CMD_S  0

#define EFUSE_INT_RAW_REG          (DR_REG_EFUSE_BASE + 0x1ac)
/* EFUSE_PGM_DONE_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define EFUSE_PGM_DONE_INT_RAW  (BIT(1))
#define EFUSE_PGM_DONE_INT_RAW_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_RAW_V  0x1
#define EFUSE_PGM_DONE_INT_RAW_S  1
/* EFUSE_READ_DONE_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define EFUSE_READ_DONE_INT_RAW  (BIT(0))
#define EFUSE_READ_DONE_INT_RAW_M  (BIT(0))
#define EFUSE_READ_DONE_INT_RAW_V  0x1
#define EFUSE_READ_DONE_INT_RAW_S  0

#define EFUSE_INT_ST_REG          (DR_REG_EFUSE_BASE + 0x1b0)
/* EFUSE_PGM_DONE_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define EFUSE_PGM_DONE_INT_ST  (BIT(1))
#define EFUSE_PGM_DONE_INT_ST_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_ST_V  0x1
#define EFUSE_PGM_DONE_INT_ST_S  1
/* EFUSE_READ_DONE_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define EFUSE_READ_DONE_INT_ST  (BIT(0))
#define EFUSE_READ_DONE_INT_ST_M  (BIT(0))
#define EFUSE_READ_DONE_INT_ST_V  0x1
#define EFUSE_READ_DONE_INT_ST_S  0

#define EFUSE_INT_ENA_REG          (DR_REG_EFUSE_BASE + 0x1b4)
/* EFUSE_PGM_DONE_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define EFUSE_PGM_DONE_INT_ENA  (BIT(1))
#define EFUSE_PGM_DONE_INT_ENA_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_ENA_V  0x1
#define EFUSE_PGM_DONE_INT_ENA_S  1
/* EFUSE_READ_DONE_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define EFUSE_READ_DONE_INT_ENA  (BIT(0))
#define EFUSE_READ_DONE_INT_ENA_M  (BIT(0))
#define EFUSE_READ_DONE_INT_ENA_V  0x1
#define EFUSE_READ_DONE_INT_ENA_S  0

#define EFUSE_INT_CLR_REG          (DR_REG_EFUSE_BASE + 0x1b8)
/* EFUSE_PGM_DONE_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define EFUSE_PGM_DONE_INT_CLR  (BIT(1))
#define EFUSE_PGM_DONE_INT_CLR_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_CLR_V  0x1
#define EFUSE_PGM_DONE_INT_CLR_S  1
/* EFUSE_READ_DONE_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define EFUSE_READ_DONE_INT_CLR  (BIT(0))
#define EFUSE_READ_DONE_INT_CLR_M  (BIT(0))
#define EFUSE_READ_DONE_INT_CLR_V  0x1
#define EFUSE_READ_DONE_INT_CLR_S  0

#define EFUSE_DAC_CONF_REG          (DR_REG_EFUSE_BASE + 0x1bc)
/* EFUSE_DAC_CLK_PAD_SEL : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DAC_CLK_PAD_SEL  (BIT(8))
#define EFUSE_DAC_CLK_PAD_SEL_M  (BIT(8))
#define EFUSE_DAC_CLK_PAD_SEL_V  0x1
#define EFUSE_DAC_CLK_PAD_SEL_S  8
/* EFUSE_DAC_CLK_DIV : R/W ;bitpos:[7:0] ;default: 8'd40 ; */
/*description: */
#define EFUSE_DAC_CLK_DIV  0x000000FF
#define EFUSE_DAC_CLK_DIV_M  ((EFUSE_DAC_CLK_DIV_V)<<(EFUSE_DAC_CLK_DIV_S))
#define EFUSE_DAC_CLK_DIV_V  0xFF
#define EFUSE_DAC_CLK_DIV_S  0

#define EFUSE_RD_TIM_CONF_REG          (DR_REG_EFUSE_BASE + 0x1c0)
/* EFUSE_READ_INIT_NUM : R/W ;bitpos:[31:24] ;default: 8'h12 ; */
/*description: */
#define EFUSE_READ_INIT_NUM  0x000000FF
#define EFUSE_READ_INIT_NUM_M  ((EFUSE_READ_INIT_NUM_V)<<(EFUSE_READ_INIT_NUM_S))
#define EFUSE_READ_INIT_NUM_V  0xFF
#define EFUSE_READ_INIT_NUM_S  24
/* EFUSE_TSUR_A : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define EFUSE_TSUR_A  0x000000FF
#define EFUSE_TSUR_A_M  ((EFUSE_TSUR_A_V)<<(EFUSE_TSUR_A_S))
#define EFUSE_TSUR_A_V  0xFF
#define EFUSE_TSUR_A_S  16
/* EFUSE_TRD : R/W ;bitpos:[15:8] ;default: 8'h2 ; */
/*description: */
#define EFUSE_TRD  0x000000FF
#define EFUSE_TRD_M  ((EFUSE_TRD_V)<<(EFUSE_TRD_S))
#define EFUSE_TRD_V  0xFF
#define EFUSE_TRD_S  8
/* EFUSE_THR_A : R/W ;bitpos:[7:0] ;default: 8'h1 ; */
/*description: */
#define EFUSE_THR_A  0x000000FF
#define EFUSE_THR_A_M  ((EFUSE_THR_A_V)<<(EFUSE_THR_A_S))
#define EFUSE_THR_A_V  0xFF
#define EFUSE_THR_A_S  0

#define EFUSE_WR_TIM_CONF0_REG          (DR_REG_EFUSE_BASE + 0x1c4)
/* EFUSE_TPGM : R/W ;bitpos:[31:16] ;default: 16'h190 ; */
/*description: */
#define EFUSE_TPGM  0x0000FFFF
#define EFUSE_TPGM_M  ((EFUSE_TPGM_V)<<(EFUSE_TPGM_S))
#define EFUSE_TPGM_V  0xFFFF
#define EFUSE_TPGM_S  16
/* EFUSE_TPGM_INACTIVE : R/W ;bitpos:[15:8] ;default: 8'h2 ; */
/*description: */
#define EFUSE_TPGM_INACTIVE  0x000000FF
#define EFUSE_TPGM_INACTIVE_M  ((EFUSE_TPGM_INACTIVE_V)<<(EFUSE_TPGM_INACTIVE_S))
#define EFUSE_TPGM_INACTIVE_V  0xFF
#define EFUSE_TPGM_INACTIVE_S  8
/* EFUSE_THP_A : R/W ;bitpos:[7:0] ;default: 8'h1 ; */
/*description: */
#define EFUSE_THP_A  0x000000FF
#define EFUSE_THP_A_M  ((EFUSE_THP_A_V)<<(EFUSE_THP_A_S))
#define EFUSE_THP_A_V  0xFF
#define EFUSE_THP_A_S  0

#define EFUSE_WR_TIM_CONF1_REG          (DR_REG_EFUSE_BASE + 0x1c8)
/* EFUSE_PWR_ON_NUM : R/W ;bitpos:[23:8] ;default: 16'h7000 ; */
/*description: */
#define EFUSE_PWR_ON_NUM  0x0000FFFF
#define EFUSE_PWR_ON_NUM_M  ((EFUSE_PWR_ON_NUM_V)<<(EFUSE_PWR_ON_NUM_S))
#define EFUSE_PWR_ON_NUM_V  0xFFFF
#define EFUSE_PWR_ON_NUM_S  8
/* EFUSE_TSUP_A : R/W ;bitpos:[7:0] ;default: 8'h1 ; */
/*description: */
#define EFUSE_TSUP_A  0x000000FF
#define EFUSE_TSUP_A_M  ((EFUSE_TSUP_A_V)<<(EFUSE_TSUP_A_S))
#define EFUSE_TSUP_A_V  0xFF
#define EFUSE_TSUP_A_S  0

#define EFUSE_DATE_REG          (DR_REG_EFUSE_BASE + 0x1FC)
/* EFUSE_DATE : R/W ;bitpos:[31:0] ;default: 32'h18101600 ; */
/*description: */
#define EFUSE_DATE  0xFFFFFFFF
#define EFUSE_DATE_M  ((EFUSE_DATE_V)<<(EFUSE_DATE_S))
#define EFUSE_DATE_V  0xFFFFFFFF
#define EFUSE_DATE_S  0

#ifdef __cplusplus
}
#endif

#endif /*_SOC_EFUSE_REG_H_ */
