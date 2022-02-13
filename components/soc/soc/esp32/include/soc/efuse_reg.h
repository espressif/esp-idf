// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _SOC_EFUSE_REG_H_
#define _SOC_EFUSE_REG_H_


#include "soc.h"
#define EFUSE_BLK0_RDATA0_REG          (DR_REG_EFUSE_BASE + 0x000)
/* EFUSE_RD_FLASH_CRYPT_CNT : RO ;bitpos:[26:20] ;default: 7'b0 ; */
/*description: read for flash_crypt_cnt*/
#define EFUSE_RD_FLASH_CRYPT_CNT  0x0000007F
#define EFUSE_RD_FLASH_CRYPT_CNT_M  ((EFUSE_RD_FLASH_CRYPT_CNT_V)<<(EFUSE_RD_FLASH_CRYPT_CNT_S))
#define EFUSE_RD_FLASH_CRYPT_CNT_V  0x7F
#define EFUSE_RD_FLASH_CRYPT_CNT_S  20
/* EFUSE_RD_EFUSE_RD_DIS : RO ;bitpos:[19:16] ;default: 4'b0 ; */
/*description: read for efuse_rd_disable*/
#define EFUSE_RD_EFUSE_RD_DIS  0x0000000F
#define EFUSE_RD_EFUSE_RD_DIS_M  ((EFUSE_RD_EFUSE_RD_DIS_V)<<(EFUSE_RD_EFUSE_RD_DIS_S))
#define EFUSE_RD_EFUSE_RD_DIS_V  0xF
#define EFUSE_RD_EFUSE_RD_DIS_S  16

/* Read disable bits for efuse blocks 1-3 */
#define EFUSE_RD_DIS_BLK1 (1<<16)
#define EFUSE_RD_DIS_BLK2 (1<<17)
#define EFUSE_RD_DIS_BLK3 (1<<18)
/* Read disable FLASH_CRYPT_CONFIG, CODING_SCHEME & KEY_STATUS
   in efuse block 0
*/
#define EFUSE_RD_DIS_BLK0_PARTIAL (1<<19)

/* EFUSE_RD_EFUSE_WR_DIS : RO ;bitpos:[15:0] ;default: 16'b0 ; */
/*description: read for efuse_wr_disable*/
#define EFUSE_RD_EFUSE_WR_DIS  0x0000FFFF
#define EFUSE_RD_EFUSE_WR_DIS_M  ((EFUSE_RD_EFUSE_WR_DIS_V)<<(EFUSE_RD_EFUSE_WR_DIS_S))
#define EFUSE_RD_EFUSE_WR_DIS_V  0xFFFF
#define EFUSE_RD_EFUSE_WR_DIS_S  0

/* Write disable bits */
#define EFUSE_WR_DIS_RD_DIS (1<<0) /*< disable writing read disable reg */
#define EFUSE_WR_DIS_WR_DIS (1<<1) /*< disable writing write disable reg */
#define EFUSE_WR_DIS_FLASH_CRYPT_CNT (1<<2)
#define EFUSE_WR_DIS_MAC_SPI_CONFIG_HD (1<<3) /*< disable writing MAC & SPI config hd efuses */
#define EFUSE_WR_DIS_XPD_SDIO (1<<5) /*< disable writing SDIO config efuses */
#define EFUSE_WR_DIS_SPI_PAD_CONFIG (1<<6) /*< disable writing SPI_PAD_CONFIG efuses */
#define EFUSE_WR_DIS_BLK1 (1<<7) /*< disable writing BLK1 efuses */
#define EFUSE_WR_DIS_BLK2 (1<<8) /*< disable writing BLK2 efuses */
#define EFUSE_WR_DIS_BLK3 (1<<9) /*< disable writing BLK3 efuses */
#define EFUSE_WR_DIS_FLASH_CRYPT_CODING_SCHEME (1<<10) /*< disable writing FLASH_CRYPT_CONFIG and CODING_SCHEME efuses */
#define EFUSE_WR_DIS_ABS_DONE_0 (1<<12) /*< disable writing ABS_DONE_0 efuse */
#define EFUSE_WR_DIS_ABS_DONE_1 (1<<13) /*< disable writing ABS_DONE_1 efuse */
#define EFUSE_WR_DIS_JTAG_DISABLE (1<<14) /*< disable writing JTAG_DISABLE efuse */
#define EFUSE_WR_DIS_CONSOLE_DL_DISABLE (1<<15) /*< disable writing CONSOLE_DEBUG_DISABLE, DISABLE_DL_ENCRYPT, DISABLE_DL_DECRYPT and DISABLE_DL_CACHE efuses */

#define EFUSE_BLK0_RDATA1_REG          (DR_REG_EFUSE_BASE + 0x004)
/* EFUSE_RD_WIFI_MAC_CRC_LOW : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: read for low 32bit WIFI_MAC_Address*/
#define EFUSE_RD_WIFI_MAC_CRC_LOW  0xFFFFFFFF
#define EFUSE_RD_WIFI_MAC_CRC_LOW_M  ((EFUSE_RD_WIFI_MAC_CRC_LOW_V)<<(EFUSE_RD_WIFI_MAC_CRC_LOW_S))
#define EFUSE_RD_WIFI_MAC_CRC_LOW_V  0xFFFFFFFF
#define EFUSE_RD_WIFI_MAC_CRC_LOW_S  0

#define EFUSE_BLK0_RDATA2_REG          (DR_REG_EFUSE_BASE + 0x008)
/* EFUSE_RD_WIFI_MAC_CRC_HIGH : RO ;bitpos:[23:0] ;default: 24'b0 ; */
/*description: read for high 24bit WIFI_MAC_Address*/
#define EFUSE_RD_WIFI_MAC_CRC_HIGH  0x00FFFFFF
#define EFUSE_RD_WIFI_MAC_CRC_HIGH_M  ((EFUSE_RD_WIFI_MAC_CRC_HIGH_V)<<(EFUSE_RD_WIFI_MAC_CRC_HIGH_S))
#define EFUSE_RD_WIFI_MAC_CRC_HIGH_V  0xFFFFFF
#define EFUSE_RD_WIFI_MAC_CRC_HIGH_S  0

#define EFUSE_BLK0_RDATA3_REG          (DR_REG_EFUSE_BASE + 0x00c)
/* EFUSE_RD_CHIP_VER_REV1 : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: bit is set to 1 for rev1 silicon*/
#define EFUSE_RD_CHIP_VER_REV1  (BIT(15))
#define EFUSE_RD_CHIP_VER_REV1_M  ((EFUSE_RD_CHIP_VER_REV1_V)<<(EFUSE_RD_CHIP_VER_REV1_S))
#define EFUSE_RD_CHIP_VER_REV1_V  0x1
#define EFUSE_RD_CHIP_VER_REV1_S  15
/* EFUSE_RD_BLK3_PART_RESERVE : R/W ; bitpos:[14] ; default: 1'b0; */
/*description: If set, this bit indicates that BLOCK3[143:96] is reserved for internal use*/
#define EFUSE_RD_BLK3_PART_RESERVE  (BIT(14))
#define EFUSE_RD_BLK3_PART_RESERVE_M  ((EFUSE_RD_BLK3_PART_RESERVE_V)<<(EFUSE_RD_BLK3_PART_RESERVE_S))
#define EFUSE_RD_BLK3_PART_RESERVE_V  0x1
#define EFUSE_RD_BLK3_PART_RESERVE_S  14
/* EFUSE_RD_CHIP_CPU_FREQ_RATED : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: If set, the ESP32's maximum CPU frequency has been rated*/
#define EFUSE_RD_CHIP_CPU_FREQ_RATED  (BIT(13))
#define EFUSE_RD_CHIP_CPU_FREQ_RATED_M  ((EFUSE_RD_CHIP_CPU_FREQ_RATED_V)<<(EFUSE_RD_CHIP_CPU_FREQ_RATED_S))
#define EFUSE_RD_CHIP_CPU_FREQ_RATED_V  0x1
#define EFUSE_RD_CHIP_CPU_FREQ_RATED_S  13
/* EFUSE_RD_CHIP_CPU_FREQ_LOW : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: If set alongside EFUSE_RD_CHIP_CPU_FREQ_RATED, the ESP32's max CPU frequency is rated for 160MHz. 240MHz otherwise*/
#define EFUSE_RD_CHIP_CPU_FREQ_LOW  (BIT(12))
#define EFUSE_RD_CHIP_CPU_FREQ_LOW_M  ((EFUSE_RD_CHIP_CPU_FREQ_LOW_V)<<(EFUSE_RD_CHIP_CPU_FREQ_LOW_S))
#define EFUSE_RD_CHIP_CPU_FREQ_LOW_V  0x1
#define EFUSE_RD_CHIP_CPU_FREQ_LOW_S  12
/* EFUSE_RD_CHIP_VER_PKG : R/W ;bitpos:[11:9] ;default: 3'b0 ; */
/*description: chip package */
#define EFUSE_RD_CHIP_VER_PKG  0x00000007
#define EFUSE_RD_CHIP_VER_PKG_M  ((EFUSE_RD_CHIP_VER_PKG_V)<<(EFUSE_RD_CHIP_VER_PKG_S))
#define EFUSE_RD_CHIP_VER_PKG_V  0x7
#define EFUSE_RD_CHIP_VER_PKG_S  9
#define EFUSE_RD_CHIP_VER_PKG_ESP32D0WDQ6  0
#define EFUSE_RD_CHIP_VER_PKG_ESP32D0WDQ5  1
#define EFUSE_RD_CHIP_VER_PKG_ESP32D2WDQ5  2
#define EFUSE_RD_CHIP_VER_PKG_ESP32PICOD2  4 /* Deprecated: this chip was never mass produced  */
#define EFUSE_RD_CHIP_VER_PKG_ESP32U4WDH   4
#define EFUSE_RD_CHIP_VER_PKG_ESP32PICOD4  5
#define EFUSE_RD_CHIP_VER_PKG_ESP32PICOV302  6
#define EFUSE_RD_CHIP_VER_PKG_ESP32D0WDR2V3  7
/* EFUSE_RD_SPI_PAD_CONFIG_HD : RO ;bitpos:[8:4] ;default: 5'b0 ; */
/*description: read for SPI_pad_config_hd*/
#define EFUSE_RD_SPI_PAD_CONFIG_HD  0x0000001F
#define EFUSE_RD_SPI_PAD_CONFIG_HD_M  ((EFUSE_RD_SPI_PAD_CONFIG_HD_V)<<(EFUSE_RD_SPI_PAD_CONFIG_HD_S))
#define EFUSE_RD_SPI_PAD_CONFIG_HD_V  0x1F
#define EFUSE_RD_SPI_PAD_CONFIG_HD_S  4
/* EFUSE_RD_CHIP_VER_DIS_CACHE : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_CHIP_VER_DIS_CACHE  (BIT(3))
#define EFUSE_RD_CHIP_VER_DIS_CACHE_M  (BIT(3))
#define EFUSE_RD_CHIP_VER_DIS_CACHE_V  0x1
#define EFUSE_RD_CHIP_VER_DIS_CACHE_S  3
/* EFUSE_RD_CHIP_VER_32PAD : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_CHIP_VER_32PAD  (BIT(2))
#define EFUSE_RD_CHIP_VER_32PAD_M  (BIT(2))
#define EFUSE_RD_CHIP_VER_32PAD_V  0x1
#define EFUSE_RD_CHIP_VER_32PAD_S  2
/* EFUSE_RD_CHIP_VER_DIS_BT : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_CHIP_VER_DIS_BT  (BIT(1))
#define EFUSE_RD_CHIP_VER_DIS_BT_M  (BIT(1))
#define EFUSE_RD_CHIP_VER_DIS_BT_V  0x1
#define EFUSE_RD_CHIP_VER_DIS_BT_S  1
/* EFUSE_RD_CHIP_VER_DIS_APP_CPU : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_CHIP_VER_DIS_APP_CPU  (BIT(0))
#define EFUSE_RD_CHIP_VER_DIS_APP_CPU_M  (BIT(0))
#define EFUSE_RD_CHIP_VER_DIS_APP_CPU_V  0x1
#define EFUSE_RD_CHIP_VER_DIS_APP_CPU_S  0

#define EFUSE_BLK0_RDATA4_REG          (DR_REG_EFUSE_BASE + 0x010)
/* EFUSE_RD_SDIO_FORCE : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: read for sdio_force*/
#define EFUSE_RD_SDIO_FORCE  (BIT(16))
#define EFUSE_RD_SDIO_FORCE_M  (BIT(16))
#define EFUSE_RD_SDIO_FORCE_V  0x1
#define EFUSE_RD_SDIO_FORCE_S  16
/* EFUSE_RD_SDIO_TIEH : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: read for SDIO_TIEH*/
#define EFUSE_RD_SDIO_TIEH  (BIT(15))
#define EFUSE_RD_SDIO_TIEH_M  (BIT(15))
#define EFUSE_RD_SDIO_TIEH_V  0x1
#define EFUSE_RD_SDIO_TIEH_S  15
/* EFUSE_RD_XPD_SDIO_REG : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: read for XPD_SDIO_REG*/
#define EFUSE_RD_XPD_SDIO_REG  (BIT(14))
#define EFUSE_RD_XPD_SDIO_REG_M  (BIT(14))
#define EFUSE_RD_XPD_SDIO_REG_V  0x1
#define EFUSE_RD_XPD_SDIO_REG_S  14
/* EFUSE_RD_ADC_VREF : R/W ;bitpos:[12:8] ;default: 5'b0 ; */
/*description: True ADC reference voltage */
#define EFUSE_RD_ADC_VREF  0x0000001F
#define EFUSE_RD_ADC_VREF_M  ((EFUSE_RD_ADC_VREF_V)<<(EFUSE_RD_ADC_VREF_S))
#define EFUSE_RD_ADC_VREF_V  0x1F
#define EFUSE_RD_ADC_VREF_S  8
/* Note: EFUSE_ADC_VREF and SDIO_DREFH/M/L share the same address space. Newer
 * versions of ESP32 come with EFUSE_ADC_VREF already burned, therefore
 * SDIO_DREFH/M/L is only available in older versions of ESP32 */
/* EFUSE_RD_SDIO_DREFL : RO ;bitpos:[13:12] ;default: 2'b0 ; */
/*description: */
#define EFUSE_RD_SDIO_DREFL  0x00000003
#define EFUSE_RD_SDIO_DREFL_M  ((EFUSE_RD_SDIO_DREFL_V)<<(EFUSE_RD_SDIO_DREFL_S))
#define EFUSE_RD_SDIO_DREFL_V  0x3
#define EFUSE_RD_SDIO_DREFL_S  12
/* EFUSE_RD_SDIO_DREFM : RO ;bitpos:[11:10] ;default: 2'b0 ; */
/*description: */
#define EFUSE_RD_SDIO_DREFM  0x00000003
#define EFUSE_RD_SDIO_DREFM_M  ((EFUSE_RD_SDIO_DREFM_V)<<(EFUSE_RD_SDIO_DREFM_S))
#define EFUSE_RD_SDIO_DREFM_V  0x3
#define EFUSE_RD_SDIO_DREFM_S  10
/* EFUSE_RD_SDIO_DREFH : RO ;bitpos:[9:8] ;default: 2'b0 ; */
/*description: */
#define EFUSE_RD_SDIO_DREFH  0x00000003
#define EFUSE_RD_SDIO_DREFH_M  ((EFUSE_RD_SDIO_DREFH_V)<<(EFUSE_RD_SDIO_DREFH_S))
#define EFUSE_RD_SDIO_DREFH_V  0x3
#define EFUSE_RD_SDIO_DREFH_S  8
/* EFUSE_RD_CK8M_FREQ : RO ;bitpos:[7:0] ;default: 8'b0 ; */
/*description: */
#define EFUSE_RD_CK8M_FREQ  0x000000FF
#define EFUSE_RD_CK8M_FREQ_M  ((EFUSE_RD_CK8M_FREQ_V)<<(EFUSE_RD_CK8M_FREQ_S))
#define EFUSE_RD_CK8M_FREQ_V  0xFF
#define EFUSE_RD_CK8M_FREQ_S  0

#define EFUSE_BLK0_RDATA5_REG          (DR_REG_EFUSE_BASE + 0x014)
/* EFUSE_RD_FLASH_CRYPT_CONFIG : RO ;bitpos:[31:28] ;default: 4'b0 ; */
/*description: read for flash_crypt_config*/
#define EFUSE_RD_FLASH_CRYPT_CONFIG  0x0000000F
#define EFUSE_RD_FLASH_CRYPT_CONFIG_M  ((EFUSE_RD_FLASH_CRYPT_CONFIG_V)<<(EFUSE_RD_FLASH_CRYPT_CONFIG_S))
#define EFUSE_RD_FLASH_CRYPT_CONFIG_V  0xF
#define EFUSE_RD_FLASH_CRYPT_CONFIG_S  28
/* EFUSE_RD_DIG_VOL_L6: RO; bitpos:[27:24]; */
/*descritpion: This field stores the difference between the digital regulator voltage at level6 and 1.2 V. (RO) 
  BIT[27] is the sign bit, 0: + , 1: -
  BIT[26:24] is the difference value, unit: 0.017V 
  volt_lv6 = BIT[27] ? 1.2 - BIT[26:24] * 0.017 : 1.2 + BIT[26:24] * 0.017     */
#define EFUSE_RD_DIG_VOL_L6          0x0F
#define EFUSE_RD_DIG_VOL_L6_M        ((EFUSE_RD_DIG_VOL_L6_V)<<(EFUSE_RD_DIG_VOL_L6_S))
#define EFUSE_RD_DIG_VOL_L6_V        0x0F
#define EFUSE_RD_DIG_VOL_L6_S        24
/* EFUSE_RD_VOL_LEVEL_HP_INV: RO; bitpos:[23:22] */
/*description: This field stores the voltage level for CPU to run at 240 MHz, or for flash/PSRAM to run at 80 MHz. 
0x0: level 7; 0x1: level 6; 0x2: level 5; 0x3: level 4. (RO)*/
#define EFUSE_RD_VOL_LEVEL_HP_INV    0x03
#define EFUSE_RD_VOL_LEVEL_HP_INV_M  ((EFUSE_RD_VOL_LEVEL_HP_INV_V)<<(EFUSE_RD_VOL_LEVEL_HP_INV_S))
#define EFUSE_RD_VOL_LEVEL_HP_INV_V  0x03
#define EFUSE_RD_VOL_LEVEL_HP_INV_S  22
/* EFUSE_RD_INST_CONFIG : RO ;bitpos:[27:20] ;default: 8'b0 ; */
/* Deprecated */
#define EFUSE_RD_INST_CONFIG  0x000000FF                                              /** Deprecated **/
#define EFUSE_RD_INST_CONFIG_M  ((EFUSE_RD_INST_CONFIG_V)<<(EFUSE_RD_INST_CONFIG_S))  /** Deprecated **/
#define EFUSE_RD_INST_CONFIG_V  0xFF                                                  /** Deprecated **/
#define EFUSE_RD_INST_CONFIG_S  20                                                    /** Deprecated **/
/* EFUSE_RD_SPI_PAD_CONFIG_CS0 : RO ;bitpos:[19:15] ;default: 5'b0 ; */
/*description: read for SPI_pad_config_cs0*/
#define EFUSE_RD_SPI_PAD_CONFIG_CS0  0x0000001F
#define EFUSE_RD_SPI_PAD_CONFIG_CS0_M  ((EFUSE_RD_SPI_PAD_CONFIG_CS0_V)<<(EFUSE_RD_SPI_PAD_CONFIG_CS0_S))
#define EFUSE_RD_SPI_PAD_CONFIG_CS0_V  0x1F
#define EFUSE_RD_SPI_PAD_CONFIG_CS0_S  15
/* EFUSE_RD_SPI_PAD_CONFIG_D : RO ;bitpos:[14:10] ;default: 5'b0 ; */
/*description: read for SPI_pad_config_d*/
#define EFUSE_RD_SPI_PAD_CONFIG_D  0x0000001F
#define EFUSE_RD_SPI_PAD_CONFIG_D_M  ((EFUSE_RD_SPI_PAD_CONFIG_D_V)<<(EFUSE_RD_SPI_PAD_CONFIG_D_S))
#define EFUSE_RD_SPI_PAD_CONFIG_D_V  0x1F
#define EFUSE_RD_SPI_PAD_CONFIG_D_S  10
/* EFUSE_RD_SPI_PAD_CONFIG_Q : RO ;bitpos:[9:5] ;default: 5'b0 ; */
/*description: read for SPI_pad_config_q*/
#define EFUSE_RD_SPI_PAD_CONFIG_Q  0x0000001F
#define EFUSE_RD_SPI_PAD_CONFIG_Q_M  ((EFUSE_RD_SPI_PAD_CONFIG_Q_V)<<(EFUSE_RD_SPI_PAD_CONFIG_Q_S))
#define EFUSE_RD_SPI_PAD_CONFIG_Q_V  0x1F
#define EFUSE_RD_SPI_PAD_CONFIG_Q_S  5
/* EFUSE_RD_SPI_PAD_CONFIG_CLK : RO ;bitpos:[4:0] ;default: 5'b0 ; */
/*description: read for SPI_pad_config_clk*/
#define EFUSE_RD_SPI_PAD_CONFIG_CLK  0x0000001F
#define EFUSE_RD_SPI_PAD_CONFIG_CLK_M  ((EFUSE_RD_SPI_PAD_CONFIG_CLK_V)<<(EFUSE_RD_SPI_PAD_CONFIG_CLK_S))
#define EFUSE_RD_SPI_PAD_CONFIG_CLK_V  0x1F
#define EFUSE_RD_SPI_PAD_CONFIG_CLK_S  0

#define EFUSE_BLK0_RDATA6_REG          (DR_REG_EFUSE_BASE + 0x018)
/* EFUSE_RD_KEY_STATUS : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: read for key_status*/
#define EFUSE_RD_KEY_STATUS  (BIT(10))
#define EFUSE_RD_KEY_STATUS_M  (BIT(10))
#define EFUSE_RD_KEY_STATUS_V  0x1
#define EFUSE_RD_KEY_STATUS_S  10
/* EFUSE_RD_DISABLE_DL_CACHE : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: read for download_dis_cache*/
#define EFUSE_RD_DISABLE_DL_CACHE  (BIT(9))
#define EFUSE_RD_DISABLE_DL_CACHE_M  (BIT(9))
#define EFUSE_RD_DISABLE_DL_CACHE_V  0x1
#define EFUSE_RD_DISABLE_DL_CACHE_S  9
/* EFUSE_RD_DISABLE_DL_DECRYPT : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: read for download_dis_decrypt*/
#define EFUSE_RD_DISABLE_DL_DECRYPT  (BIT(8))
#define EFUSE_RD_DISABLE_DL_DECRYPT_M  (BIT(8))
#define EFUSE_RD_DISABLE_DL_DECRYPT_V  0x1
#define EFUSE_RD_DISABLE_DL_DECRYPT_S  8
/* EFUSE_RD_DISABLE_DL_ENCRYPT : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: read for download_dis_encrypt*/
#define EFUSE_RD_DISABLE_DL_ENCRYPT  (BIT(7))
#define EFUSE_RD_DISABLE_DL_ENCRYPT_M  (BIT(7))
#define EFUSE_RD_DISABLE_DL_ENCRYPT_V  0x1
#define EFUSE_RD_DISABLE_DL_ENCRYPT_S  7
/* EFUSE_RD_DISABLE_JTAG : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: read for JTAG_disable*/
#define EFUSE_RD_DISABLE_JTAG  (BIT(6))
#define EFUSE_RD_DISABLE_JTAG_M  (BIT(6))
#define EFUSE_RD_DISABLE_JTAG_V  0x1
#define EFUSE_RD_DISABLE_JTAG_S  6
/* EFUSE_RD_ABS_DONE_1 : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: read for abstract_done_1*/
#define EFUSE_RD_ABS_DONE_1  (BIT(5))
#define EFUSE_RD_ABS_DONE_1_M  (BIT(5))
#define EFUSE_RD_ABS_DONE_1_V  0x1
#define EFUSE_RD_ABS_DONE_1_S  5
/* EFUSE_RD_ABS_DONE_0 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: read for abstract_done_0*/
#define EFUSE_RD_ABS_DONE_0  (BIT(4))
#define EFUSE_RD_ABS_DONE_0_M  (BIT(4))
#define EFUSE_RD_ABS_DONE_0_V  0x1
#define EFUSE_RD_ABS_DONE_0_S  4
/* EFUSE_RD_DISABLE_SDIO_HOST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define EFUSE_RD_DISABLE_SDIO_HOST  (BIT(3))
#define EFUSE_RD_DISABLE_SDIO_HOST_M  (BIT(3))
#define EFUSE_RD_DISABLE_SDIO_HOST_V  0x1
#define EFUSE_RD_DISABLE_SDIO_HOST_S  3
/* EFUSE_RD_CONSOLE_DEBUG_DISABLE : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: read for console_debug_disable*/
#define EFUSE_RD_CONSOLE_DEBUG_DISABLE  (BIT(2))
#define EFUSE_RD_CONSOLE_DEBUG_DISABLE_M  (BIT(2))
#define EFUSE_RD_CONSOLE_DEBUG_DISABLE_V  0x1
#define EFUSE_RD_CONSOLE_DEBUG_DISABLE_S  2
/* EFUSE_RD_CODING_SCHEME : RO ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: read for coding_scheme*/
#define EFUSE_RD_CODING_SCHEME  0x00000003
#define EFUSE_RD_CODING_SCHEME_M  ((EFUSE_RD_CODING_SCHEME_V)<<(EFUSE_RD_CODING_SCHEME_S))
#define EFUSE_RD_CODING_SCHEME_V  0x3
#define EFUSE_RD_CODING_SCHEME_S  0

#define EFUSE_CODING_SCHEME_VAL_NONE 0x0
#define EFUSE_CODING_SCHEME_VAL_34   0x1
#define EFUSE_CODING_SCHEME_VAL_REPEAT   0x2

#define EFUSE_BLK0_WDATA0_REG          (DR_REG_EFUSE_BASE + 0x01c)
/* EFUSE_FLASH_CRYPT_CNT : R/W ;bitpos:[26:20] ;default: 7'b0 ; */
/*description: program for flash_crypt_cnt*/
#define EFUSE_FLASH_CRYPT_CNT  0x0000007F
#define EFUSE_FLASH_CRYPT_CNT_M  ((EFUSE_FLASH_CRYPT_CNT_V)<<(EFUSE_FLASH_CRYPT_CNT_S))
#define EFUSE_FLASH_CRYPT_CNT_V  0x7F
#define EFUSE_FLASH_CRYPT_CNT_S  20
/* EFUSE_RD_DIS : R/W ;bitpos:[19:16] ;default: 4'b0 ; */
/*description: program for efuse_rd_disable*/
#define EFUSE_RD_DIS  0x0000000F
#define EFUSE_RD_DIS_M  ((EFUSE_RD_DIS_V)<<(EFUSE_RD_DIS_S))
#define EFUSE_RD_DIS_V  0xF
#define EFUSE_RD_DIS_S  16
/* EFUSE_WR_DIS : R/W ;bitpos:[15:0] ;default: 16'b0 ; */
/*description: program for efuse_wr_disable*/
#define EFUSE_WR_DIS  0x0000FFFF
#define EFUSE_WR_DIS_M  ((EFUSE_WR_DIS_V)<<(EFUSE_WR_DIS_S))
#define EFUSE_WR_DIS_V  0xFFFF
#define EFUSE_WR_DIS_S  0

#define EFUSE_BLK0_WDATA1_REG          (DR_REG_EFUSE_BASE + 0x020)
/* EFUSE_WIFI_MAC_CRC_LOW : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: program for low 32bit WIFI_MAC_Address*/
#define EFUSE_WIFI_MAC_CRC_LOW  0xFFFFFFFF
#define EFUSE_WIFI_MAC_CRC_LOW_M  ((EFUSE_WIFI_MAC_CRC_LOW_V)<<(EFUSE_WIFI_MAC_CRC_LOW_S))
#define EFUSE_WIFI_MAC_CRC_LOW_V  0xFFFFFFFF
#define EFUSE_WIFI_MAC_CRC_LOW_S  0

#define EFUSE_BLK0_WDATA2_REG          (DR_REG_EFUSE_BASE + 0x024)
/* EFUSE_WIFI_MAC_CRC_HIGH : R/W ;bitpos:[23:0] ;default: 24'b0 ; */
/*description: program for high 24bit WIFI_MAC_Address*/
#define EFUSE_WIFI_MAC_CRC_HIGH  0x00FFFFFF
#define EFUSE_WIFI_MAC_CRC_HIGH_M  ((EFUSE_WIFI_MAC_CRC_HIGH_V)<<(EFUSE_WIFI_MAC_CRC_HIGH_S))
#define EFUSE_WIFI_MAC_CRC_HIGH_V  0xFFFFFF
#define EFUSE_WIFI_MAC_CRC_HIGH_S  0

#define EFUSE_BLK0_WDATA3_REG          (DR_REG_EFUSE_BASE + 0x028)
/* EFUSE_CHIP_VER_REV1 : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define EFUSE_CHIP_VER_REV1  (BIT(15))
#define EFUSE_CHIP_VER_REV1_M  ((EFUSE_CHIP_VER_REV1_V)<<(EFUSE_CHIP_VER_REV1_S))
#define EFUSE_CHIP_VER_REV1_V  0x1
#define EFUSE_CHIP_VER_REV1_S  15
/* EFUSE_BLK3_PART_RESERVE : R/W ; bitpos:[14] ; default: 1'b0; */
/*description: If set, this bit indicates that BLOCK3[143:96] is reserved for internal use*/
#define EFUSE_BLK3_PART_RESERVE  (BIT(14))
#define EFUSE_BLK3_PART_RESERVE_M  ((EFUSE_BLK3_PART_RESERVE_V)<<(EFUSE_BLK3_PART_RESERVE_S))
#define EFUSE_BLK3_PART_RESERVE_V  0x1
#define EFUSE_BLK3_PART_RESERVE_S  14
/* EFUSE_CHIP_CPU_FREQ_RATED : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: If set, the ESP32's maximum CPU frequency has been rated*/
#define EFUSE_CHIP_CPU_FREQ_RATED  (BIT(13))
#define EFUSE_CHIP_CPU_FREQ_RATED_M  ((EFUSE_CHIP_CPU_FREQ_RATED_V)<<(EFUSE_CHIP_CPU_FREQ_RATED_S))
#define EFUSE_CHIP_CPU_FREQ_RATED_V  0x1
#define EFUSE_CHIP_CPU_FREQ_RATED_S  13
/* EFUSE_CHIP_CPU_FREQ_LOW : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: If set alongside EFUSE_CHIP_CPU_FREQ_RATED, the ESP32's max CPU frequency is rated for 160MHz. 240MHz otherwise*/
#define EFUSE_CHIP_CPU_FREQ_LOW  (BIT(12))
#define EFUSE_CHIP_CPU_FREQ_LOW_M  ((EFUSE_CHIP_CPU_FREQ_LOW_V)<<(EFUSE_CHIP_CPU_FREQ_LOW_S))
#define EFUSE_CHIP_CPU_FREQ_LOW_V  0x1
#define EFUSE_CHIP_CPU_FREQ_LOW_S  12
/* EFUSE_CHIP_VER_PKG : R/W ;bitpos:[11:9] ;default: 3'b0 ; */
/*description: */
#define EFUSE_CHIP_VER_PKG  0x00000007
#define EFUSE_CHIP_VER_PKG_M  ((EFUSE_CHIP_VER_PKG_V)<<(EFUSE_CHIP_VER_PKG_S))
#define EFUSE_CHIP_VER_PKG_V  0x7
#define EFUSE_CHIP_VER_PKG_S  9
#define EFUSE_CHIP_VER_PKG_ESP32D0WDQ6  0
#define EFUSE_CHIP_VER_PKG_ESP32D0WDQ5  1
#define EFUSE_CHIP_VER_PKG_ESP32D2WDQ5  2
#define EFUSE_CHIP_VER_PKG_ESP32PICOD2  4
#define EFUSE_CHIP_VER_PKG_ESP32PICOD4  5
/* EFUSE_SPI_PAD_CONFIG_HD : R/W ;bitpos:[8:4] ;default: 5'b0 ; */
/*description: program for SPI_pad_config_hd*/
#define EFUSE_SPI_PAD_CONFIG_HD  0x0000001F
#define EFUSE_SPI_PAD_CONFIG_HD_M  ((EFUSE_SPI_PAD_CONFIG_HD_V)<<(EFUSE_SPI_PAD_CONFIG_HD_S))
#define EFUSE_SPI_PAD_CONFIG_HD_V  0x1F
#define EFUSE_SPI_PAD_CONFIG_HD_S  4
/* EFUSE_CHIP_VER_DIS_CACHE : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define EFUSE_CHIP_VER_DIS_CACHE  (BIT(3))
#define EFUSE_CHIP_VER_DIS_CACHE_M  (BIT(3))
#define EFUSE_CHIP_VER_DIS_CACHE_V  0x1
#define EFUSE_CHIP_VER_DIS_CACHE_S  3
/* EFUSE_CHIP_VER_32PAD : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define EFUSE_CHIP_VER_32PAD  (BIT(2))
#define EFUSE_CHIP_VER_32PAD_M  (BIT(2))
#define EFUSE_CHIP_VER_32PAD_V  0x1
#define EFUSE_CHIP_VER_32PAD_S  2
/* EFUSE_CHIP_VER_DIS_BT : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define EFUSE_CHIP_VER_DIS_BT  (BIT(1))
#define EFUSE_CHIP_VER_DIS_BT_M  (BIT(1))
#define EFUSE_CHIP_VER_DIS_BT_V  0x1
#define EFUSE_CHIP_VER_DIS_BT_S  1
/* EFUSE_CHIP_VER_DIS_APP_CPU : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define EFUSE_CHIP_VER_DIS_APP_CPU  (BIT(0))
#define EFUSE_CHIP_VER_DIS_APP_CPU_M  (BIT(0))
#define EFUSE_CHIP_VER_DIS_APP_CPU_V  0x1
#define EFUSE_CHIP_VER_DIS_APP_CPU_S  0

#define EFUSE_BLK0_WDATA4_REG          (DR_REG_EFUSE_BASE + 0x02c)
/* EFUSE_SDIO_FORCE : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: program for sdio_force*/
#define EFUSE_SDIO_FORCE  (BIT(16))
#define EFUSE_SDIO_FORCE_M  (BIT(16))
#define EFUSE_SDIO_FORCE_V  0x1
#define EFUSE_SDIO_FORCE_S  16
/* EFUSE_SDIO_TIEH : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: program for SDIO_TIEH*/
#define EFUSE_SDIO_TIEH  (BIT(15))
#define EFUSE_SDIO_TIEH_M  (BIT(15))
#define EFUSE_SDIO_TIEH_V  0x1
#define EFUSE_SDIO_TIEH_S  15
/* EFUSE_XPD_SDIO_REG : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: program for XPD_SDIO_REG*/
#define EFUSE_XPD_SDIO_REG  (BIT(14))
#define EFUSE_XPD_SDIO_REG_M  (BIT(14))
#define EFUSE_XPD_SDIO_REG_V  0x1
#define EFUSE_XPD_SDIO_REG_S  14
/* EFUSE_ADC_VREF : R/W ;bitpos:[12:8] ;default: 5'b0 ; */
/*description: True ADC reference voltage */
#define EFUSE_ADC_VREF  0x0000001F
#define EFUSE_ADC_VREF_M  ((EFUSE_ADC_VREF_V)<<(EFUSE_ADC_VREF_S))
#define EFUSE_ADC_VREF_V  0x1F
#define EFUSE_ADC_VREF_S  8
/* Note: EFUSE_ADC_VREF and SDIO_DREFH/M/L share the same address space. Newer
 * versions of ESP32 come with EFUSE_ADC_VREF already burned, therefore
 * SDIO_DREFH/M/L is only available in older versions of ESP32 */
/* EFUSE_SDIO_DREFL : R/W ;bitpos:[13:12] ;default: 2'b0 ; */
/*description: */
#define EFUSE_SDIO_DREFL  0x00000003
#define EFUSE_SDIO_DREFL_M  ((EFUSE_SDIO_DREFL_V)<<(EFUSE_SDIO_DREFL_S))
#define EFUSE_SDIO_DREFL_V  0x3
#define EFUSE_SDIO_DREFL_S  12
/* EFUSE_SDIO_DREFM : R/W ;bitpos:[11:10] ;default: 2'b0 ; */
/*description: */
#define EFUSE_SDIO_DREFM  0x00000003
#define EFUSE_SDIO_DREFM_M  ((EFUSE_SDIO_DREFM_V)<<(EFUSE_SDIO_DREFM_S))
#define EFUSE_SDIO_DREFM_V  0x3
#define EFUSE_SDIO_DREFM_S  10
/* EFUSE_SDIO_DREFH : R/W ;bitpos:[9:8] ;default: 2'b0 ; */
/*description: */
#define EFUSE_SDIO_DREFH  0x00000003
#define EFUSE_SDIO_DREFH_M  ((EFUSE_SDIO_DREFH_V)<<(EFUSE_SDIO_DREFH_S))
#define EFUSE_SDIO_DREFH_V  0x3
#define EFUSE_SDIO_DREFH_S  8
/* EFUSE_CK8M_FREQ : R/W ;bitpos:[7:0] ;default: 8'b0 ; */
/*description: */
#define EFUSE_CK8M_FREQ  0x000000FF
#define EFUSE_CK8M_FREQ_M  ((EFUSE_CK8M_FREQ_V)<<(EFUSE_CK8M_FREQ_S))
#define EFUSE_CK8M_FREQ_V  0xFF
#define EFUSE_CK8M_FREQ_S  0

#define EFUSE_BLK0_WDATA5_REG          (DR_REG_EFUSE_BASE + 0x030)
/* EFUSE_FLASH_CRYPT_CONFIG : R/W ;bitpos:[31:28] ;default: 4'b0 ; */
/*description: program for flash_crypt_config*/
#define EFUSE_FLASH_CRYPT_CONFIG  0x0000000F
#define EFUSE_FLASH_CRYPT_CONFIG_M  ((EFUSE_FLASH_CRYPT_CONFIG_V)<<(EFUSE_FLASH_CRYPT_CONFIG_S))
#define EFUSE_FLASH_CRYPT_CONFIG_V  0xF
#define EFUSE_FLASH_CRYPT_CONFIG_S  28
/* EFUSE_DIG_VOL_L6: R/W; bitpos:[27:24]; */
/*descritpion: This field stores the difference between the digital regulator voltage at level6 and 1.2 V. (R/W) 
  BIT[27] is the sign bit, 0: + , 1: -
  BIT[26:24] is the difference value, unit: 0.017V 
  volt_lv6 = BIT[27] ? 1.2 - BIT[26:24] * 0.017 : 1.2 + BIT[26:24] * 0.017     */
#define EFUSE_DIG_VOL_L6            0x0F
#define EFUSE_DIG_VOL_L6_M          ((EFUSE_RD_DIG_VOL_L6_V)<<(EFUSE_RD_DIG_VOL_L6_S))
#define EFUSE_DIG_VOL_L6_V          0x0F
#define EFUSE_DIG_VOL_L6_S          24
/* EFUSE_VOL_LEVEL_HP_INV: R/W; bitpos:[23:22] */
/*description: This field stores the voltage level for CPU to run at 240 MHz, or for flash/PSRAM to run at 80 MHz. 
0x0: level 7; 0x1: level 6; 0x2: level 5; 0x3: level 4. (R/W)*/
#define EFUSE_VOL_LEVEL_HP_INV      0x03
#define EFUSE_VOL_LEVEL_HP_INV_M    ((EFUSE_RD_VOL_LEVEL_HP_INV_V)<<(EFUSE_RD_VOL_LEVEL_HP_INV_S))
#define EFUSE_VOL_LEVEL_HP_INV_V    0x03
#define EFUSE_VOL_LEVEL_HP_INV_S    22
/* EFUSE_INST_CONFIG : R/W ;bitpos:[27:20] ;default: 8'b0 ; */
/* Deprecated */
#define EFUSE_INST_CONFIG  0x000000FF                                        /** Deprecated **/
#define EFUSE_INST_CONFIG_M  ((EFUSE_INST_CONFIG_V)<<(EFUSE_INST_CONFIG_S))  /** Deprecated **/
#define EFUSE_INST_CONFIG_V  0xFF                                            /** Deprecated **/
#define EFUSE_INST_CONFIG_S  20                                              /** Deprecated **/
/* EFUSE_SPI_PAD_CONFIG_CS0 : R/W ;bitpos:[19:15] ;default: 5'b0 ; */
/*description: program for SPI_pad_config_cs0*/
#define EFUSE_SPI_PAD_CONFIG_CS0  0x0000001F
#define EFUSE_SPI_PAD_CONFIG_CS0_M  ((EFUSE_SPI_PAD_CONFIG_CS0_V)<<(EFUSE_SPI_PAD_CONFIG_CS0_S))
#define EFUSE_SPI_PAD_CONFIG_CS0_V  0x1F
#define EFUSE_SPI_PAD_CONFIG_CS0_S  15
/* EFUSE_SPI_PAD_CONFIG_D : R/W ;bitpos:[14:10] ;default: 5'b0 ; */
/*description: program for SPI_pad_config_d*/
#define EFUSE_SPI_PAD_CONFIG_D  0x0000001F
#define EFUSE_SPI_PAD_CONFIG_D_M  ((EFUSE_SPI_PAD_CONFIG_D_V)<<(EFUSE_SPI_PAD_CONFIG_D_S))
#define EFUSE_SPI_PAD_CONFIG_D_V  0x1F
#define EFUSE_SPI_PAD_CONFIG_D_S  10
/* EFUSE_SPI_PAD_CONFIG_Q : R/W ;bitpos:[9:5] ;default: 5'b0 ; */
/*description: program for SPI_pad_config_q*/
#define EFUSE_SPI_PAD_CONFIG_Q  0x0000001F
#define EFUSE_SPI_PAD_CONFIG_Q_M  ((EFUSE_SPI_PAD_CONFIG_Q_V)<<(EFUSE_SPI_PAD_CONFIG_Q_S))
#define EFUSE_SPI_PAD_CONFIG_Q_V  0x1F
#define EFUSE_SPI_PAD_CONFIG_Q_S  5
/* EFUSE_SPI_PAD_CONFIG_CLK : R/W ;bitpos:[4:0] ;default: 5'b0 ; */
/*description: program for SPI_pad_config_clk*/
#define EFUSE_SPI_PAD_CONFIG_CLK  0x0000001F
#define EFUSE_SPI_PAD_CONFIG_CLK_M  ((EFUSE_SPI_PAD_CONFIG_CLK_V)<<(EFUSE_SPI_PAD_CONFIG_CLK_S))
#define EFUSE_SPI_PAD_CONFIG_CLK_V  0x1F
#define EFUSE_SPI_PAD_CONFIG_CLK_S  0

#define EFUSE_BLK0_WDATA6_REG          (DR_REG_EFUSE_BASE + 0x034)
/* EFUSE_KEY_STATUS : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: program for key_status*/
#define EFUSE_KEY_STATUS  (BIT(10))
#define EFUSE_KEY_STATUS_M  (BIT(10))
#define EFUSE_KEY_STATUS_V  0x1
#define EFUSE_KEY_STATUS_S  10
/* EFUSE_DISABLE_DL_CACHE : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: program for download_dis_cache*/
#define EFUSE_DISABLE_DL_CACHE  (BIT(9))
#define EFUSE_DISABLE_DL_CACHE_M  (BIT(9))
#define EFUSE_DISABLE_DL_CACHE_V  0x1
#define EFUSE_DISABLE_DL_CACHE_S  9
/* EFUSE_DISABLE_DL_DECRYPT : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: program for download_dis_decrypt*/
#define EFUSE_DISABLE_DL_DECRYPT  (BIT(8))
#define EFUSE_DISABLE_DL_DECRYPT_M  (BIT(8))
#define EFUSE_DISABLE_DL_DECRYPT_V  0x1
#define EFUSE_DISABLE_DL_DECRYPT_S  8
/* EFUSE_DISABLE_DL_ENCRYPT : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: program for download_dis_encrypt*/
#define EFUSE_DISABLE_DL_ENCRYPT  (BIT(7))
#define EFUSE_DISABLE_DL_ENCRYPT_M  (BIT(7))
#define EFUSE_DISABLE_DL_ENCRYPT_V  0x1
#define EFUSE_DISABLE_DL_ENCRYPT_S  7
/* EFUSE_DISABLE_JTAG : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: program for JTAG_disable*/
#define EFUSE_DISABLE_JTAG  (BIT(6))
#define EFUSE_DISABLE_JTAG_M  (BIT(6))
#define EFUSE_DISABLE_JTAG_V  0x1
#define EFUSE_DISABLE_JTAG_S  6
/* EFUSE_ABS_DONE_1 : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: program for abstract_done_1*/
#define EFUSE_ABS_DONE_1  (BIT(5))
#define EFUSE_ABS_DONE_1_M  (BIT(5))
#define EFUSE_ABS_DONE_1_V  0x1
#define EFUSE_ABS_DONE_1_S  5
/* EFUSE_ABS_DONE_0 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: program for abstract_done_0*/
#define EFUSE_ABS_DONE_0  (BIT(4))
#define EFUSE_ABS_DONE_0_M  (BIT(4))
#define EFUSE_ABS_DONE_0_V  0x1
#define EFUSE_ABS_DONE_0_S  4
/* EFUSE_DISABLE_SDIO_HOST : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DISABLE_SDIO_HOST  (BIT(3))
#define EFUSE_DISABLE_SDIO_HOST_M  (BIT(3))
#define EFUSE_DISABLE_SDIO_HOST_V  0x1
#define EFUSE_DISABLE_SDIO_HOST_S  3
/* EFUSE_CONSOLE_DEBUG_DISABLE : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: program for console_debug_disable*/
#define EFUSE_CONSOLE_DEBUG_DISABLE  (BIT(2))
#define EFUSE_CONSOLE_DEBUG_DISABLE_M  (BIT(2))
#define EFUSE_CONSOLE_DEBUG_DISABLE_V  0x1
#define EFUSE_CONSOLE_DEBUG_DISABLE_S  2
/* EFUSE_CODING_SCHEME : R/W ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: program for coding_scheme*/
#define EFUSE_CODING_SCHEME  0x00000003
#define EFUSE_CODING_SCHEME_M  ((EFUSE_CODING_SCHEME_V)<<(EFUSE_CODING_SCHEME_S))
#define EFUSE_CODING_SCHEME_V  0x3
#define EFUSE_CODING_SCHEME_S  0

#define EFUSE_BLK1_RDATA0_REG          (DR_REG_EFUSE_BASE + 0x038)
/* EFUSE_BLK1_DOUT0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK1*/
#define EFUSE_BLK1_DOUT0  0xFFFFFFFF
#define EFUSE_BLK1_DOUT0_M  ((EFUSE_BLK1_DOUT0_V)<<(EFUSE_BLK1_DOUT0_S))
#define EFUSE_BLK1_DOUT0_V  0xFFFFFFFF
#define EFUSE_BLK1_DOUT0_S  0

#define EFUSE_BLK1_RDATA1_REG          (DR_REG_EFUSE_BASE + 0x03c)
/* EFUSE_BLK1_DOUT1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK1*/
#define EFUSE_BLK1_DOUT1  0xFFFFFFFF
#define EFUSE_BLK1_DOUT1_M  ((EFUSE_BLK1_DOUT1_V)<<(EFUSE_BLK1_DOUT1_S))
#define EFUSE_BLK1_DOUT1_V  0xFFFFFFFF
#define EFUSE_BLK1_DOUT1_S  0

#define EFUSE_BLK1_RDATA2_REG          (DR_REG_EFUSE_BASE + 0x040)
/* EFUSE_BLK1_DOUT2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK1*/
#define EFUSE_BLK1_DOUT2  0xFFFFFFFF
#define EFUSE_BLK1_DOUT2_M  ((EFUSE_BLK1_DOUT2_V)<<(EFUSE_BLK1_DOUT2_S))
#define EFUSE_BLK1_DOUT2_V  0xFFFFFFFF
#define EFUSE_BLK1_DOUT2_S  0

#define EFUSE_BLK1_RDATA3_REG          (DR_REG_EFUSE_BASE + 0x044)
/* EFUSE_BLK1_DOUT3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK1*/
#define EFUSE_BLK1_DOUT3  0xFFFFFFFF
#define EFUSE_BLK1_DOUT3_M  ((EFUSE_BLK1_DOUT3_V)<<(EFUSE_BLK1_DOUT3_S))
#define EFUSE_BLK1_DOUT3_V  0xFFFFFFFF
#define EFUSE_BLK1_DOUT3_S  0

#define EFUSE_BLK1_RDATA4_REG          (DR_REG_EFUSE_BASE + 0x048)
/* EFUSE_BLK1_DOUT4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK1*/
#define EFUSE_BLK1_DOUT4  0xFFFFFFFF
#define EFUSE_BLK1_DOUT4_M  ((EFUSE_BLK1_DOUT4_V)<<(EFUSE_BLK1_DOUT4_S))
#define EFUSE_BLK1_DOUT4_V  0xFFFFFFFF
#define EFUSE_BLK1_DOUT4_S  0

#define EFUSE_BLK1_RDATA5_REG          (DR_REG_EFUSE_BASE + 0x04c)
/* EFUSE_BLK1_DOUT5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK1*/
#define EFUSE_BLK1_DOUT5  0xFFFFFFFF
#define EFUSE_BLK1_DOUT5_M  ((EFUSE_BLK1_DOUT5_V)<<(EFUSE_BLK1_DOUT5_S))
#define EFUSE_BLK1_DOUT5_V  0xFFFFFFFF
#define EFUSE_BLK1_DOUT5_S  0

#define EFUSE_BLK1_RDATA6_REG          (DR_REG_EFUSE_BASE + 0x050)
/* EFUSE_BLK1_DOUT6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK1*/
#define EFUSE_BLK1_DOUT6  0xFFFFFFFF
#define EFUSE_BLK1_DOUT6_M  ((EFUSE_BLK1_DOUT6_V)<<(EFUSE_BLK1_DOUT6_S))
#define EFUSE_BLK1_DOUT6_V  0xFFFFFFFF
#define EFUSE_BLK1_DOUT6_S  0

#define EFUSE_BLK1_RDATA7_REG          (DR_REG_EFUSE_BASE + 0x054)
/* EFUSE_BLK1_DOUT7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK1*/
#define EFUSE_BLK1_DOUT7  0xFFFFFFFF
#define EFUSE_BLK1_DOUT7_M  ((EFUSE_BLK1_DOUT7_V)<<(EFUSE_BLK1_DOUT7_S))
#define EFUSE_BLK1_DOUT7_V  0xFFFFFFFF
#define EFUSE_BLK1_DOUT7_S  0

#define EFUSE_BLK2_RDATA0_REG          (DR_REG_EFUSE_BASE + 0x058)
/* EFUSE_BLK2_DOUT0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK2*/
#define EFUSE_BLK2_DOUT0  0xFFFFFFFF
#define EFUSE_BLK2_DOUT0_M  ((EFUSE_BLK2_DOUT0_V)<<(EFUSE_BLK2_DOUT0_S))
#define EFUSE_BLK2_DOUT0_V  0xFFFFFFFF
#define EFUSE_BLK2_DOUT0_S  0

#define EFUSE_BLK2_RDATA1_REG          (DR_REG_EFUSE_BASE + 0x05c)
/* EFUSE_BLK2_DOUT1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK2*/
#define EFUSE_BLK2_DOUT1  0xFFFFFFFF
#define EFUSE_BLK2_DOUT1_M  ((EFUSE_BLK2_DOUT1_V)<<(EFUSE_BLK2_DOUT1_S))
#define EFUSE_BLK2_DOUT1_V  0xFFFFFFFF
#define EFUSE_BLK2_DOUT1_S  0

#define EFUSE_BLK2_RDATA2_REG          (DR_REG_EFUSE_BASE + 0x060)
/* EFUSE_BLK2_DOUT2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK2*/
#define EFUSE_BLK2_DOUT2  0xFFFFFFFF
#define EFUSE_BLK2_DOUT2_M  ((EFUSE_BLK2_DOUT2_V)<<(EFUSE_BLK2_DOUT2_S))
#define EFUSE_BLK2_DOUT2_V  0xFFFFFFFF
#define EFUSE_BLK2_DOUT2_S  0

#define EFUSE_BLK2_RDATA3_REG          (DR_REG_EFUSE_BASE + 0x064)
/* EFUSE_BLK2_DOUT3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK2*/
#define EFUSE_BLK2_DOUT3  0xFFFFFFFF
#define EFUSE_BLK2_DOUT3_M  ((EFUSE_BLK2_DOUT3_V)<<(EFUSE_BLK2_DOUT3_S))
#define EFUSE_BLK2_DOUT3_V  0xFFFFFFFF
#define EFUSE_BLK2_DOUT3_S  0

#define EFUSE_BLK2_RDATA4_REG          (DR_REG_EFUSE_BASE + 0x068)
/* EFUSE_BLK2_DOUT4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK2*/
#define EFUSE_BLK2_DOUT4  0xFFFFFFFF
#define EFUSE_BLK2_DOUT4_M  ((EFUSE_BLK2_DOUT4_V)<<(EFUSE_BLK2_DOUT4_S))
#define EFUSE_BLK2_DOUT4_V  0xFFFFFFFF
#define EFUSE_BLK2_DOUT4_S  0

#define EFUSE_BLK2_RDATA5_REG          (DR_REG_EFUSE_BASE + 0x06c)
/* EFUSE_BLK2_DOUT5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK2*/
#define EFUSE_BLK2_DOUT5  0xFFFFFFFF
#define EFUSE_BLK2_DOUT5_M  ((EFUSE_BLK2_DOUT5_V)<<(EFUSE_BLK2_DOUT5_S))
#define EFUSE_BLK2_DOUT5_V  0xFFFFFFFF
#define EFUSE_BLK2_DOUT5_S  0

#define EFUSE_BLK2_RDATA6_REG          (DR_REG_EFUSE_BASE + 0x070)
/* EFUSE_BLK2_DOUT6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK2*/
#define EFUSE_BLK2_DOUT6  0xFFFFFFFF
#define EFUSE_BLK2_DOUT6_M  ((EFUSE_BLK2_DOUT6_V)<<(EFUSE_BLK2_DOUT6_S))
#define EFUSE_BLK2_DOUT6_V  0xFFFFFFFF
#define EFUSE_BLK2_DOUT6_S  0

#define EFUSE_BLK2_RDATA7_REG          (DR_REG_EFUSE_BASE + 0x074)
/* EFUSE_BLK2_DOUT7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK2*/
#define EFUSE_BLK2_DOUT7  0xFFFFFFFF
#define EFUSE_BLK2_DOUT7_M  ((EFUSE_BLK2_DOUT7_V)<<(EFUSE_BLK2_DOUT7_S))
#define EFUSE_BLK2_DOUT7_V  0xFFFFFFFF
#define EFUSE_BLK2_DOUT7_S  0

#define EFUSE_BLK3_RDATA0_REG          (DR_REG_EFUSE_BASE + 0x078)
/* EFUSE_BLK3_DOUT0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK3*/
#define EFUSE_BLK3_DOUT0  0xFFFFFFFF
#define EFUSE_BLK3_DOUT0_M  ((EFUSE_BLK3_DOUT0_V)<<(EFUSE_BLK3_DOUT0_S))
#define EFUSE_BLK3_DOUT0_V  0xFFFFFFFF
#define EFUSE_BLK3_DOUT0_S  0

#define EFUSE_BLK3_RDATA1_REG          (DR_REG_EFUSE_BASE + 0x07c)
/* EFUSE_BLK3_DOUT1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK3*/
#define EFUSE_BLK3_DOUT1  0xFFFFFFFF
#define EFUSE_BLK3_DOUT1_M  ((EFUSE_BLK3_DOUT1_V)<<(EFUSE_BLK3_DOUT1_S))
#define EFUSE_BLK3_DOUT1_V  0xFFFFFFFF
#define EFUSE_BLK3_DOUT1_S  0

#define EFUSE_BLK3_RDATA2_REG          (DR_REG_EFUSE_BASE + 0x080)
/* EFUSE_BLK3_DOUT2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK3*/
#define EFUSE_BLK3_DOUT2  0xFFFFFFFF
#define EFUSE_BLK3_DOUT2_M  ((EFUSE_BLK3_DOUT2_V)<<(EFUSE_BLK3_DOUT2_S))
#define EFUSE_BLK3_DOUT2_V  0xFFFFFFFF
#define EFUSE_BLK3_DOUT2_S  0

/* Note: Newer ESP32s utilize BLK3_DATA3 and parts of BLK3_DATA4 for calibration
 * purposes. This usage is indicated by the EFUSE_RD_BLK3_PART_RESERVE bit.*/
#define EFUSE_BLK3_RDATA3_REG          (DR_REG_EFUSE_BASE + 0x084)
/* EFUSE_BLK3_DOUT3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK3*/
#define EFUSE_BLK3_DOUT3  0xFFFFFFFF
#define EFUSE_BLK3_DOUT3_M  ((EFUSE_BLK3_DOUT3_V)<<(EFUSE_BLK3_DOUT3_S))
#define EFUSE_BLK3_DOUT3_V  0xFFFFFFFF
#define EFUSE_BLK3_DOUT3_S  0
/* EFUSE_RD_ADC2_TP_HIGH : R/W ;bitpos:[31:23] ;default: 9'b0 ; */
/*description: ADC2 Two Point calibration high point. Only valid if EFUSE_RD_BLK3_PART_RESERVE */
#define EFUSE_RD_ADC2_TP_HIGH  0x1FF
#define EFUSE_RD_ADC2_TP_HIGH_M  ((EFUSE_RD_ADC2_TP_HIGH_V)<<(EFUSE_RD_ADC2_TP_HIGH_S))
#define EFUSE_RD_ADC2_TP_HIGH_V  0x1FF
#define EFUSE_RD_ADC2_TP_HIGH_S  23
/* EFUSE_RD_ADC2_TP_LOW : R/W ;bitpos:[22:16] ;default: 7'b0 ; */
/*description: ADC2 Two Point calibration low point. Only valid if EFUSE_RD_BLK3_PART_RESERVE */
#define EFUSE_RD_ADC2_TP_LOW  0x7F
#define EFUSE_RD_ADC2_TP_LOW_M  ((EFUSE_RD_ADC2_TP_LOW_V)<<(EFUSE_RD_ADC2_TP_LOW_S))
#define EFUSE_RD_ADC2_TP_LOW_V  0x7F
#define EFUSE_RD_ADC2_TP_LOW_S  16
/* EFUSE_RD_ADC1_TP_HIGH : R/W ;bitpos:[15:7] ;default: 9'b0 ; */
/*description: ADC1 Two Point calibration high point. Only valid if EFUSE_RD_BLK3_PART_RESERVE */
#define EFUSE_RD_ADC1_TP_HIGH  0x1FF
#define EFUSE_RD_ADC1_TP_HIGH_M  ((EFUSE_RD_ADC1_TP_HIGH_V)<<(EFUSE_RD_ADC1_TP_HIGH_S))
#define EFUSE_RD_ADC1_TP_HIGH_V  0x1FF
#define EFUSE_RD_ADC1_TP_HIGH_S  7
/* EFUSE_RD_ADC1_TP_LOW : R/W ;bitpos:[6:0] ;default: 7'b0 ; */
/*description: ADC1 Two Point calibration low point. Only valid if EFUSE_RD_BLK3_PART_RESERVE */
#define EFUSE_RD_ADC1_TP_LOW  0x7F
#define EFUSE_RD_ADC1_TP_LOW_M  ((EFUSE_RD_ADC1_TP_LOW_V)<<(EFUSE_RD_ADC1_TP_LOW_S))
#define EFUSE_RD_ADC1_TP_LOW_V  0x7F
#define EFUSE_RD_ADC1_TP_LOW_S  0

#define EFUSE_BLK3_RDATA4_REG          (DR_REG_EFUSE_BASE + 0x088)
/* EFUSE_BLK3_DOUT4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK3*/
#define EFUSE_BLK3_DOUT4  0xFFFFFFFF
#define EFUSE_BLK3_DOUT4_M  ((EFUSE_BLK3_DOUT4_V)<<(EFUSE_BLK3_DOUT4_S))
#define EFUSE_BLK3_DOUT4_V  0xFFFFFFFF
#define EFUSE_BLK3_DOUT4_S  0
/* EFUSE_RD_CAL_RESERVED: R/W ; bitpos:[0:15] ; default : 16'h0 ; */
/*description: Reserved for future calibration use. Indicated by EFUSE_RD_BLK3_PART_RESERVE */
#define EFUSE_RD_CAL_RESERVED  0x0000FFFF
#define EFUSE_RD_CAL_RESERVED_M  ((EFUSE_RD_CAL_RESERVED_V)<<(EFUSE_RD_CAL_RESERVED_S))
#define EFUSE_RD_CAL_RESERVED_V  0xFFFF
#define EFUSE_RD_CAL_RESERVED_S  0

#define EFUSE_BLK3_RDATA5_REG          (DR_REG_EFUSE_BASE + 0x08c)
/* EFUSE_BLK3_DOUT5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK3*/
#define EFUSE_BLK3_DOUT5  0xFFFFFFFF
#define EFUSE_BLK3_DOUT5_M  ((EFUSE_BLK3_DOUT5_V)<<(EFUSE_BLK3_DOUT5_S))
#define EFUSE_BLK3_DOUT5_V  0xFFFFFFFF
#define EFUSE_BLK3_DOUT5_S  0

#define EFUSE_BLK3_RDATA6_REG          (DR_REG_EFUSE_BASE + 0x090)
/* EFUSE_BLK3_DOUT6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK3*/
#define EFUSE_BLK3_DOUT6  0xFFFFFFFF
#define EFUSE_BLK3_DOUT6_M  ((EFUSE_BLK3_DOUT6_V)<<(EFUSE_BLK3_DOUT6_S))
#define EFUSE_BLK3_DOUT6_V  0xFFFFFFFF
#define EFUSE_BLK3_DOUT6_S  0

#define EFUSE_BLK3_RDATA7_REG          (DR_REG_EFUSE_BASE + 0x094)
/* EFUSE_BLK3_DOUT7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: read for BLOCK3*/
#define EFUSE_BLK3_DOUT7  0xFFFFFFFF
#define EFUSE_BLK3_DOUT7_M  ((EFUSE_BLK3_DOUT7_V)<<(EFUSE_BLK3_DOUT7_S))
#define EFUSE_BLK3_DOUT7_V  0xFFFFFFFF
#define EFUSE_BLK3_DOUT7_S  0

#define EFUSE_BLK1_WDATA0_REG          (DR_REG_EFUSE_BASE + 0x098)
/* EFUSE_BLK1_DIN0 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK1*/
#define EFUSE_BLK1_DIN0  0xFFFFFFFF
#define EFUSE_BLK1_DIN0_M  ((EFUSE_BLK1_DIN0_V)<<(EFUSE_BLK1_DIN0_S))
#define EFUSE_BLK1_DIN0_V  0xFFFFFFFF
#define EFUSE_BLK1_DIN0_S  0

#define EFUSE_BLK1_WDATA1_REG          (DR_REG_EFUSE_BASE + 0x09c)
/* EFUSE_BLK1_DIN1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK1*/
#define EFUSE_BLK1_DIN1  0xFFFFFFFF
#define EFUSE_BLK1_DIN1_M  ((EFUSE_BLK1_DIN1_V)<<(EFUSE_BLK1_DIN1_S))
#define EFUSE_BLK1_DIN1_V  0xFFFFFFFF
#define EFUSE_BLK1_DIN1_S  0

#define EFUSE_BLK1_WDATA2_REG          (DR_REG_EFUSE_BASE + 0x0a0)
/* EFUSE_BLK1_DIN2 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK1*/
#define EFUSE_BLK1_DIN2  0xFFFFFFFF
#define EFUSE_BLK1_DIN2_M  ((EFUSE_BLK1_DIN2_V)<<(EFUSE_BLK1_DIN2_S))
#define EFUSE_BLK1_DIN2_V  0xFFFFFFFF
#define EFUSE_BLK1_DIN2_S  0

#define EFUSE_BLK1_WDATA3_REG          (DR_REG_EFUSE_BASE + 0x0a4)
/* EFUSE_BLK1_DIN3 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK1*/
#define EFUSE_BLK1_DIN3  0xFFFFFFFF
#define EFUSE_BLK1_DIN3_M  ((EFUSE_BLK1_DIN3_V)<<(EFUSE_BLK1_DIN3_S))
#define EFUSE_BLK1_DIN3_V  0xFFFFFFFF
#define EFUSE_BLK1_DIN3_S  0

#define EFUSE_BLK1_WDATA4_REG          (DR_REG_EFUSE_BASE + 0x0a8)
/* EFUSE_BLK1_DIN4 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK1*/
#define EFUSE_BLK1_DIN4  0xFFFFFFFF
#define EFUSE_BLK1_DIN4_M  ((EFUSE_BLK1_DIN4_V)<<(EFUSE_BLK1_DIN4_S))
#define EFUSE_BLK1_DIN4_V  0xFFFFFFFF
#define EFUSE_BLK1_DIN4_S  0

#define EFUSE_BLK1_WDATA5_REG          (DR_REG_EFUSE_BASE + 0x0ac)
/* EFUSE_BLK1_DIN5 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK1*/
#define EFUSE_BLK1_DIN5  0xFFFFFFFF
#define EFUSE_BLK1_DIN5_M  ((EFUSE_BLK1_DIN5_V)<<(EFUSE_BLK1_DIN5_S))
#define EFUSE_BLK1_DIN5_V  0xFFFFFFFF
#define EFUSE_BLK1_DIN5_S  0

#define EFUSE_BLK1_WDATA6_REG          (DR_REG_EFUSE_BASE + 0x0b0)
/* EFUSE_BLK1_DIN6 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK1*/
#define EFUSE_BLK1_DIN6  0xFFFFFFFF
#define EFUSE_BLK1_DIN6_M  ((EFUSE_BLK1_DIN6_V)<<(EFUSE_BLK1_DIN6_S))
#define EFUSE_BLK1_DIN6_V  0xFFFFFFFF
#define EFUSE_BLK1_DIN6_S  0

#define EFUSE_BLK1_WDATA7_REG          (DR_REG_EFUSE_BASE + 0x0b4)
/* EFUSE_BLK1_DIN7 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK1*/
#define EFUSE_BLK1_DIN7  0xFFFFFFFF
#define EFUSE_BLK1_DIN7_M  ((EFUSE_BLK1_DIN7_V)<<(EFUSE_BLK1_DIN7_S))
#define EFUSE_BLK1_DIN7_V  0xFFFFFFFF
#define EFUSE_BLK1_DIN7_S  0

#define EFUSE_BLK2_WDATA0_REG          (DR_REG_EFUSE_BASE + 0x0b8)
/* EFUSE_BLK2_DIN0 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK2*/
#define EFUSE_BLK2_DIN0  0xFFFFFFFF
#define EFUSE_BLK2_DIN0_M  ((EFUSE_BLK2_DIN0_V)<<(EFUSE_BLK2_DIN0_S))
#define EFUSE_BLK2_DIN0_V  0xFFFFFFFF
#define EFUSE_BLK2_DIN0_S  0

#define EFUSE_BLK2_WDATA1_REG          (DR_REG_EFUSE_BASE + 0x0bc)
/* EFUSE_BLK2_DIN1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK2*/
#define EFUSE_BLK2_DIN1  0xFFFFFFFF
#define EFUSE_BLK2_DIN1_M  ((EFUSE_BLK2_DIN1_V)<<(EFUSE_BLK2_DIN1_S))
#define EFUSE_BLK2_DIN1_V  0xFFFFFFFF
#define EFUSE_BLK2_DIN1_S  0

#define EFUSE_BLK2_WDATA2_REG          (DR_REG_EFUSE_BASE + 0x0c0)
/* EFUSE_BLK2_DIN2 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK2*/
#define EFUSE_BLK2_DIN2  0xFFFFFFFF
#define EFUSE_BLK2_DIN2_M  ((EFUSE_BLK2_DIN2_V)<<(EFUSE_BLK2_DIN2_S))
#define EFUSE_BLK2_DIN2_V  0xFFFFFFFF
#define EFUSE_BLK2_DIN2_S  0

#define EFUSE_BLK2_WDATA3_REG          (DR_REG_EFUSE_BASE + 0x0c4)
/* EFUSE_BLK2_DIN3 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK2*/
#define EFUSE_BLK2_DIN3  0xFFFFFFFF
#define EFUSE_BLK2_DIN3_M  ((EFUSE_BLK2_DIN3_V)<<(EFUSE_BLK2_DIN3_S))
#define EFUSE_BLK2_DIN3_V  0xFFFFFFFF
#define EFUSE_BLK2_DIN3_S  0

#define EFUSE_BLK2_WDATA4_REG          (DR_REG_EFUSE_BASE + 0x0c8)
/* EFUSE_BLK2_DIN4 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK2*/
#define EFUSE_BLK2_DIN4  0xFFFFFFFF
#define EFUSE_BLK2_DIN4_M  ((EFUSE_BLK2_DIN4_V)<<(EFUSE_BLK2_DIN4_S))
#define EFUSE_BLK2_DIN4_V  0xFFFFFFFF
#define EFUSE_BLK2_DIN4_S  0

#define EFUSE_BLK2_WDATA5_REG          (DR_REG_EFUSE_BASE + 0x0cc)
/* EFUSE_BLK2_DIN5 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK2*/
#define EFUSE_BLK2_DIN5  0xFFFFFFFF
#define EFUSE_BLK2_DIN5_M  ((EFUSE_BLK2_DIN5_V)<<(EFUSE_BLK2_DIN5_S))
#define EFUSE_BLK2_DIN5_V  0xFFFFFFFF
#define EFUSE_BLK2_DIN5_S  0

#define EFUSE_BLK2_WDATA6_REG          (DR_REG_EFUSE_BASE + 0x0d0)
/* EFUSE_BLK2_DIN6 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK2*/
#define EFUSE_BLK2_DIN6  0xFFFFFFFF
#define EFUSE_BLK2_DIN6_M  ((EFUSE_BLK2_DIN6_V)<<(EFUSE_BLK2_DIN6_S))
#define EFUSE_BLK2_DIN6_V  0xFFFFFFFF
#define EFUSE_BLK2_DIN6_S  0

#define EFUSE_BLK2_WDATA7_REG          (DR_REG_EFUSE_BASE + 0x0d4)
/* EFUSE_BLK2_DIN7 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK2*/
#define EFUSE_BLK2_DIN7  0xFFFFFFFF
#define EFUSE_BLK2_DIN7_M  ((EFUSE_BLK2_DIN7_V)<<(EFUSE_BLK2_DIN7_S))
#define EFUSE_BLK2_DIN7_V  0xFFFFFFFF
#define EFUSE_BLK2_DIN7_S  0

#define EFUSE_BLK3_WDATA0_REG          (DR_REG_EFUSE_BASE + 0x0d8)
/* EFUSE_BLK3_DIN0 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK3*/
#define EFUSE_BLK3_DIN0  0xFFFFFFFF
#define EFUSE_BLK3_DIN0_M  ((EFUSE_BLK3_DIN0_V)<<(EFUSE_BLK3_DIN0_S))
#define EFUSE_BLK3_DIN0_V  0xFFFFFFFF
#define EFUSE_BLK3_DIN0_S  0

#define EFUSE_BLK3_WDATA1_REG          (DR_REG_EFUSE_BASE + 0x0dc)
/* EFUSE_BLK3_DIN1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK3*/
#define EFUSE_BLK3_DIN1  0xFFFFFFFF
#define EFUSE_BLK3_DIN1_M  ((EFUSE_BLK3_DIN1_V)<<(EFUSE_BLK3_DIN1_S))
#define EFUSE_BLK3_DIN1_V  0xFFFFFFFF
#define EFUSE_BLK3_DIN1_S  0

#define EFUSE_BLK3_WDATA2_REG          (DR_REG_EFUSE_BASE + 0x0e0)
/* EFUSE_BLK3_DIN2 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK3*/
#define EFUSE_BLK3_DIN2  0xFFFFFFFF
#define EFUSE_BLK3_DIN2_M  ((EFUSE_BLK3_DIN2_V)<<(EFUSE_BLK3_DIN2_S))
#define EFUSE_BLK3_DIN2_V  0xFFFFFFFF
#define EFUSE_BLK3_DIN2_S  0

/* Note: Newer ESP32s utilize BLK3_DATA3 and parts of BLK3_DATA4 for calibration
 * purposes. This usage is indicated by the EFUSE_RD_BLK3_PART_RESERVE bit.*/
#define EFUSE_BLK3_WDATA3_REG          (DR_REG_EFUSE_BASE + 0x0e4)
/* EFUSE_BLK3_DIN3 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK3*/
#define EFUSE_BLK3_DIN3  0xFFFFFFFF
#define EFUSE_BLK3_DIN3_M  ((EFUSE_BLK3_DIN3_V)<<(EFUSE_BLK3_DIN3_S))
#define EFUSE_BLK3_DIN3_V  0xFFFFFFFF
#define EFUSE_BLK3_DIN3_S  0
/* EFUSE_ADC2_TP_HIGH : R/W ;bitpos:[31:23] ;default: 9'b0 ; */
/*description: ADC2 Two Point calibration high point. Only valid if EFUSE_RD_BLK3_PART_RESERVE */
#define EFUSE_ADC2_TP_HIGH  0x1FF
#define EFUSE_ADC2_TP_HIGH_M  ((EFUSE_ADC2_TP_HIGH_V)<<(EFUSE_ADC2_TP_HIGH_S))
#define EFUSE_ADC2_TP_HIGH_V  0x1FF
#define EFUSE_ADC2_TP_HIGH_S  23
/* EFUSE_ADC2_TP_LOW : R/W ;bitpos:[22:16] ;default: 7'b0 ; */
/*description: ADC2 Two Point calibration low point. Only valid if EFUSE_RD_BLK3_PART_RESERVE */
#define EFUSE_ADC2_TP_LOW  0x7F
#define EFUSE_ADC2_TP_LOW_M  ((EFUSE_ADC2_TP_LOW_V)<<(EFUSE_ADC2_TP_LOW_S))
#define EFUSE_ADC2_TP_LOW_V  0x7F
#define EFUSE_ADC2_TP_LOW_S  16
/* EFUSE_ADC1_TP_HIGH : R/W ;bitpos:[15:7] ;default: 9'b0 ; */
/*description: ADC1 Two Point calibration high point. Only valid if EFUSE_RD_BLK3_PART_RESERVE */
#define EFUSE_ADC1_TP_HIGH  0x1FF
#define EFUSE_ADC1_TP_HIGH_M  ((EFUSE_ADC1_TP_HIGH_V)<<(EFUSE_ADC1_TP_HIGH_S))
#define EFUSE_ADC1_TP_HIGH_V  0x1FF
#define EFUSE_ADC1_TP_HIGH_S  7
/* EFUSE_ADC1_TP_LOW : R/W ;bitpos:[6:0] ;default: 7'b0 ; */
/*description: ADC1 Two Point calibration low point. Only valid if EFUSE_RD_BLK3_PART_RESERVE */
#define EFUSE_ADC1_TP_LOW  0x7F
#define EFUSE_ADC1_TP_LOW_M  ((EFUSE_ADC1_TP_LOW_V)<<(EFUSE_ADC1_TP_LOW_S))
#define EFUSE_ADC1_TP_LOW_V  0x7F
#define EFUSE_ADC1_TP_LOW_S  0

#define EFUSE_BLK3_WDATA4_REG          (DR_REG_EFUSE_BASE + 0x0e8)
/* EFUSE_BLK3_DIN4 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK3*/
#define EFUSE_BLK3_DIN4  0xFFFFFFFF
#define EFUSE_BLK3_DIN4_M  ((EFUSE_BLK3_DIN4_V)<<(EFUSE_BLK3_DIN4_S))
#define EFUSE_BLK3_DIN4_V  0xFFFFFFFF
#define EFUSE_BLK3_DIN4_S  0
/* EFUSE_CAL_RESERVED: R/W ; bitpos:[0:15] ; default : 16'h0 ; */
/*description: Reserved for future calibration use. Indicated by EFUSE_BLK3_PART_RESERVE */
#define EFUSE_CAL_RESERVED  0x0000FFFF
#define EFUSE_CAL_RESERVED_M  ((EFUSE_CAL_RESERVED_V)<<(EFUSE_CAL_RESERVED_S))
#define EFUSE_CAL_RESERVED_V  0xFFFF
#define EFUSE_CAL_RESERVED_S  0

#define EFUSE_BLK3_WDATA5_REG          (DR_REG_EFUSE_BASE + 0x0ec)
/* EFUSE_BLK3_DIN5 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK3*/
#define EFUSE_BLK3_DIN5  0xFFFFFFFF
#define EFUSE_BLK3_DIN5_M  ((EFUSE_BLK3_DIN5_V)<<(EFUSE_BLK3_DIN5_S))
#define EFUSE_BLK3_DIN5_V  0xFFFFFFFF
#define EFUSE_BLK3_DIN5_S  0

#define EFUSE_BLK3_WDATA6_REG          (DR_REG_EFUSE_BASE + 0x0f0)
/* EFUSE_BLK3_DIN6 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK3*/
#define EFUSE_BLK3_DIN6  0xFFFFFFFF
#define EFUSE_BLK3_DIN6_M  ((EFUSE_BLK3_DIN6_V)<<(EFUSE_BLK3_DIN6_S))
#define EFUSE_BLK3_DIN6_V  0xFFFFFFFF
#define EFUSE_BLK3_DIN6_S  0

#define EFUSE_BLK3_WDATA7_REG          (DR_REG_EFUSE_BASE + 0x0f4)
/* EFUSE_BLK3_DIN7 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: program for BLOCK3*/
#define EFUSE_BLK3_DIN7  0xFFFFFFFF
#define EFUSE_BLK3_DIN7_M  ((EFUSE_BLK3_DIN7_V)<<(EFUSE_BLK3_DIN7_S))
#define EFUSE_BLK3_DIN7_V  0xFFFFFFFF
#define EFUSE_BLK3_DIN7_S  0

#define EFUSE_CLK_REG          (DR_REG_EFUSE_BASE + 0x0f8)
/* EFUSE_CLK_EN : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define EFUSE_CLK_EN  (BIT(16))
#define EFUSE_CLK_EN_M  (BIT(16))
#define EFUSE_CLK_EN_V  0x1
#define EFUSE_CLK_EN_S  16
/* EFUSE_CLK_SEL1 : R/W ;bitpos:[15:8] ;default: 8'h40 ; */
/*description: efuse timing configure*/
#define EFUSE_CLK_SEL1  0x000000FF
#define EFUSE_CLK_SEL1_M  ((EFUSE_CLK_SEL1_V)<<(EFUSE_CLK_SEL1_S))
#define EFUSE_CLK_SEL1_V  0xFF
#define EFUSE_CLK_SEL1_S  8
/* EFUSE_CLK_SEL0 : R/W ;bitpos:[7:0] ;default: 8'h52 ; */
/*description: efuse timing configure*/
#define EFUSE_CLK_SEL0  0x000000FF
#define EFUSE_CLK_SEL0_M  ((EFUSE_CLK_SEL0_V)<<(EFUSE_CLK_SEL0_S))
#define EFUSE_CLK_SEL0_V  0xFF
#define EFUSE_CLK_SEL0_S  0

#define EFUSE_CONF_REG          (DR_REG_EFUSE_BASE + 0x0fc)
/* EFUSE_FORCE_NO_WR_RD_DIS : R/W ;bitpos:[16] ;default: 1'h1 ; */
/*description: */
#define EFUSE_FORCE_NO_WR_RD_DIS  (BIT(16))
#define EFUSE_FORCE_NO_WR_RD_DIS_M  (BIT(16))
#define EFUSE_FORCE_NO_WR_RD_DIS_V  0x1
#define EFUSE_FORCE_NO_WR_RD_DIS_S  16
/* EFUSE_OP_CODE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: efuse operation code*/
#define EFUSE_OP_CODE  0x0000FFFF
#define EFUSE_OP_CODE_M  ((EFUSE_OP_CODE_V)<<(EFUSE_OP_CODE_S))
#define EFUSE_OP_CODE_V  0xFFFF
#define EFUSE_OP_CODE_S  0

#define EFUSE_STATUS_REG          (DR_REG_EFUSE_BASE + 0x100)
/* EFUSE_DEBUG : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define EFUSE_DEBUG  0xFFFFFFFF
#define EFUSE_DEBUG_M  ((EFUSE_DEBUG_V)<<(EFUSE_DEBUG_S))
#define EFUSE_DEBUG_V  0xFFFFFFFF
#define EFUSE_DEBUG_S  0

#define EFUSE_CMD_REG          (DR_REG_EFUSE_BASE + 0x104)
/* EFUSE_PGM_CMD : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: command for program*/
#define EFUSE_PGM_CMD  (BIT(1))
#define EFUSE_PGM_CMD_M  (BIT(1))
#define EFUSE_PGM_CMD_V  0x1
#define EFUSE_PGM_CMD_S  1
/* EFUSE_READ_CMD : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: command for read*/
#define EFUSE_READ_CMD  (BIT(0))
#define EFUSE_READ_CMD_M  (BIT(0))
#define EFUSE_READ_CMD_V  0x1
#define EFUSE_READ_CMD_S  0

#define EFUSE_INT_RAW_REG          (DR_REG_EFUSE_BASE + 0x108)
/* EFUSE_PGM_DONE_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: program done interrupt raw status*/
#define EFUSE_PGM_DONE_INT_RAW  (BIT(1))
#define EFUSE_PGM_DONE_INT_RAW_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_RAW_V  0x1
#define EFUSE_PGM_DONE_INT_RAW_S  1
/* EFUSE_READ_DONE_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: read done interrupt raw status*/
#define EFUSE_READ_DONE_INT_RAW  (BIT(0))
#define EFUSE_READ_DONE_INT_RAW_M  (BIT(0))
#define EFUSE_READ_DONE_INT_RAW_V  0x1
#define EFUSE_READ_DONE_INT_RAW_S  0

#define EFUSE_INT_ST_REG          (DR_REG_EFUSE_BASE + 0x10c)
/* EFUSE_PGM_DONE_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: program done interrupt status*/
#define EFUSE_PGM_DONE_INT_ST  (BIT(1))
#define EFUSE_PGM_DONE_INT_ST_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_ST_V  0x1
#define EFUSE_PGM_DONE_INT_ST_S  1
/* EFUSE_READ_DONE_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: read done interrupt status*/
#define EFUSE_READ_DONE_INT_ST  (BIT(0))
#define EFUSE_READ_DONE_INT_ST_M  (BIT(0))
#define EFUSE_READ_DONE_INT_ST_V  0x1
#define EFUSE_READ_DONE_INT_ST_S  0

#define EFUSE_INT_ENA_REG          (DR_REG_EFUSE_BASE + 0x110)
/* EFUSE_PGM_DONE_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: program done interrupt enable*/
#define EFUSE_PGM_DONE_INT_ENA  (BIT(1))
#define EFUSE_PGM_DONE_INT_ENA_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_ENA_V  0x1
#define EFUSE_PGM_DONE_INT_ENA_S  1
/* EFUSE_READ_DONE_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: read done interrupt enable*/
#define EFUSE_READ_DONE_INT_ENA  (BIT(0))
#define EFUSE_READ_DONE_INT_ENA_M  (BIT(0))
#define EFUSE_READ_DONE_INT_ENA_V  0x1
#define EFUSE_READ_DONE_INT_ENA_S  0

#define EFUSE_INT_CLR_REG          (DR_REG_EFUSE_BASE + 0x114)
/* EFUSE_PGM_DONE_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: program done interrupt clear*/
#define EFUSE_PGM_DONE_INT_CLR  (BIT(1))
#define EFUSE_PGM_DONE_INT_CLR_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_CLR_V  0x1
#define EFUSE_PGM_DONE_INT_CLR_S  1
/* EFUSE_READ_DONE_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: read done interrupt clear*/
#define EFUSE_READ_DONE_INT_CLR  (BIT(0))
#define EFUSE_READ_DONE_INT_CLR_M  (BIT(0))
#define EFUSE_READ_DONE_INT_CLR_V  0x1
#define EFUSE_READ_DONE_INT_CLR_S  0

#define EFUSE_DAC_CONF_REG          (DR_REG_EFUSE_BASE + 0x118)
/* EFUSE_DAC_CLK_PAD_SEL : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define EFUSE_DAC_CLK_PAD_SEL  (BIT(8))
#define EFUSE_DAC_CLK_PAD_SEL_M  (BIT(8))
#define EFUSE_DAC_CLK_PAD_SEL_V  0x1
#define EFUSE_DAC_CLK_PAD_SEL_S  8
/* EFUSE_DAC_CLK_DIV : R/W ;bitpos:[7:0] ;default: 8'd40 ; */
/*description: efuse timing configure*/
#define EFUSE_DAC_CLK_DIV  0x000000FF
#define EFUSE_DAC_CLK_DIV_M  ((EFUSE_DAC_CLK_DIV_V)<<(EFUSE_DAC_CLK_DIV_S))
#define EFUSE_DAC_CLK_DIV_V  0xFF
#define EFUSE_DAC_CLK_DIV_S  0

#define EFUSE_DEC_STATUS_REG          (DR_REG_EFUSE_BASE + 0x11c)
/* EFUSE_DEC_WARNINGS : RO ;bitpos:[11:0] ;default: 12'b0 ; */
/*description: the decode result of 3/4 coding scheme has warning*/
#define EFUSE_DEC_WARNINGS  0x00000FFF
#define EFUSE_DEC_WARNINGS_M  ((EFUSE_DEC_WARNINGS_V)<<(EFUSE_DEC_WARNINGS_S))
#define EFUSE_DEC_WARNINGS_V  0xFFF
#define EFUSE_DEC_WARNINGS_S  0

#define EFUSE_DATE_REG          (DR_REG_EFUSE_BASE + 0x1FC)
/* EFUSE_DATE : R/W ;bitpos:[31:0] ;default: 32'h16042600 ; */
/*description: */
#define EFUSE_DATE  0xFFFFFFFF
#define EFUSE_DATE_M  ((EFUSE_DATE_V)<<(EFUSE_DATE_S))
#define EFUSE_DATE_V  0xFFFFFFFF
#define EFUSE_DATE_S  0




#endif /*_SOC_EFUSE_REG_H_ */


