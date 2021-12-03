/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_EFUSE_REG_H_
#define _SOC_EFUSE_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"

#define EFUSE_PGM_DATA0_REG          (DR_REG_EFUSE_BASE + 0x0)
/* EFUSE_PGM_DATA_0 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 0th 32-bit data to be programmed..*/
#define EFUSE_PGM_DATA_0    0xFFFFFFFF
#define EFUSE_PGM_DATA_0_M  ((EFUSE_PGM_DATA_0_V)<<(EFUSE_PGM_DATA_0_S))
#define EFUSE_PGM_DATA_0_V  0xFFFFFFFF
#define EFUSE_PGM_DATA_0_S  0

#define EFUSE_PGM_DATA1_REG          (DR_REG_EFUSE_BASE + 0x4)
/* EFUSE_PGM_DATA_1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 1st 32-bit data to be programmed..*/
#define EFUSE_PGM_DATA_1    0xFFFFFFFF
#define EFUSE_PGM_DATA_1_M  ((EFUSE_PGM_DATA_1_V)<<(EFUSE_PGM_DATA_1_S))
#define EFUSE_PGM_DATA_1_V  0xFFFFFFFF
#define EFUSE_PGM_DATA_1_S  0

#define EFUSE_PGM_DATA2_REG          (DR_REG_EFUSE_BASE + 0x8)
/* EFUSE_PGM_DATA_2 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 2nd 32-bit data to be programmed..*/
#define EFUSE_PGM_DATA_2    0xFFFFFFFF
#define EFUSE_PGM_DATA_2_M  ((EFUSE_PGM_DATA_2_V)<<(EFUSE_PGM_DATA_2_S))
#define EFUSE_PGM_DATA_2_V  0xFFFFFFFF
#define EFUSE_PGM_DATA_2_S  0

#define EFUSE_PGM_DATA3_REG          (DR_REG_EFUSE_BASE + 0xC)
/* EFUSE_PGM_DATA_3 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 3rd 32-bit data to be programmed..*/
#define EFUSE_PGM_DATA_3    0xFFFFFFFF
#define EFUSE_PGM_DATA_3_M  ((EFUSE_PGM_DATA_3_V)<<(EFUSE_PGM_DATA_3_S))
#define EFUSE_PGM_DATA_3_V  0xFFFFFFFF
#define EFUSE_PGM_DATA_3_S  0

#define EFUSE_PGM_DATA4_REG          (DR_REG_EFUSE_BASE + 0x10)
/* EFUSE_PGM_DATA_4 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 4th 32-bit data to be programmed..*/
#define EFUSE_PGM_DATA_4    0xFFFFFFFF
#define EFUSE_PGM_DATA_4_M  ((EFUSE_PGM_DATA_4_V)<<(EFUSE_PGM_DATA_4_S))
#define EFUSE_PGM_DATA_4_V  0xFFFFFFFF
#define EFUSE_PGM_DATA_4_S  0

#define EFUSE_PGM_DATA5_REG          (DR_REG_EFUSE_BASE + 0x14)
/* EFUSE_PGM_DATA_5 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 5th 32-bit data to be programmed..*/
#define EFUSE_PGM_DATA_5    0xFFFFFFFF
#define EFUSE_PGM_DATA_5_M  ((EFUSE_PGM_DATA_5_V)<<(EFUSE_PGM_DATA_5_S))
#define EFUSE_PGM_DATA_5_V  0xFFFFFFFF
#define EFUSE_PGM_DATA_5_S  0

#define EFUSE_PGM_DATA6_REG          (DR_REG_EFUSE_BASE + 0x18)
/* EFUSE_PGM_DATA_6 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 6th 32-bit data to be programmed..*/
#define EFUSE_PGM_DATA_6    0xFFFFFFFF
#define EFUSE_PGM_DATA_6_M  ((EFUSE_PGM_DATA_6_V)<<(EFUSE_PGM_DATA_6_S))
#define EFUSE_PGM_DATA_6_V  0xFFFFFFFF
#define EFUSE_PGM_DATA_6_S  0

#define EFUSE_PGM_DATA7_REG          (DR_REG_EFUSE_BASE + 0x1C)
/* EFUSE_PGM_DATA_7 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 7th 32-bit data to be programmed..*/
#define EFUSE_PGM_DATA_7    0xFFFFFFFF
#define EFUSE_PGM_DATA_7_M  ((EFUSE_PGM_DATA_7_V)<<(EFUSE_PGM_DATA_7_S))
#define EFUSE_PGM_DATA_7_V  0xFFFFFFFF
#define EFUSE_PGM_DATA_7_S  0

#define EFUSE_PGM_CHECK_VALUE0_REG          (DR_REG_EFUSE_BASE + 0x20)
/* EFUSE_PGM_RS_DATA_0 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 0th 32-bit RS code to be programmed..*/
#define EFUSE_PGM_RS_DATA_0    0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_0_M  ((EFUSE_PGM_RS_DATA_0_V)<<(EFUSE_PGM_RS_DATA_0_S))
#define EFUSE_PGM_RS_DATA_0_V  0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_0_S  0

#define EFUSE_PGM_CHECK_VALUE1_REG          (DR_REG_EFUSE_BASE + 0x24)
/* EFUSE_PGM_RS_DATA_1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 1st 32-bit RS code to be programmed..*/
#define EFUSE_PGM_RS_DATA_1    0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_1_M  ((EFUSE_PGM_RS_DATA_1_V)<<(EFUSE_PGM_RS_DATA_1_S))
#define EFUSE_PGM_RS_DATA_1_V  0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_1_S  0

#define EFUSE_PGM_CHECK_VALUE2_REG          (DR_REG_EFUSE_BASE + 0x28)
/* EFUSE_PGM_RS_DATA_2 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 2nd 32-bit RS code to be programmed..*/
#define EFUSE_PGM_RS_DATA_2    0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_2_M  ((EFUSE_PGM_RS_DATA_2_V)<<(EFUSE_PGM_RS_DATA_2_S))
#define EFUSE_PGM_RS_DATA_2_V  0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_2_S  0

#define EFUSE_RD_WR_DIS_REG          (DR_REG_EFUSE_BASE + 0x2C)
/* EFUSE_WR_DIS : RO ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: Disable programming of individual eFuses..*/
#define EFUSE_WR_DIS    0x000000FF
#define EFUSE_WR_DIS_M  ((EFUSE_WR_DIS_V)<<(EFUSE_WR_DIS_S))
#define EFUSE_WR_DIS_V  0xFF
#define EFUSE_WR_DIS_S  0

#define EFUSE_RD_REPEAT_DATA0_REG          (DR_REG_EFUSE_BASE + 0x30)
/* EFUSE_RPT4_RESERVED : RO ;bitpos:[31:22] ;default: 10'h0 ; */
/*description: Reserved (used for four backups method)..*/
#define EFUSE_RPT4_RESERVED    0x000003FF
#define EFUSE_RPT4_RESERVED_M  ((EFUSE_RPT4_RESERVED_V)<<(EFUSE_RPT4_RESERVED_S))
#define EFUSE_RPT4_RESERVED_V  0x3FF
#define EFUSE_RPT4_RESERVED_S  22
/* EFUSE_SECURE_BOOT_EN : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: The bit be set to enable secure boot..*/
#define EFUSE_SECURE_BOOT_EN    (BIT(21))
#define EFUSE_SECURE_BOOT_EN_M  (BIT(21))
#define EFUSE_SECURE_BOOT_EN_V  0x1
#define EFUSE_SECURE_BOOT_EN_S  21
/* EFUSE_FLASH_TPUW : RO ;bitpos:[20:17] ;default: 4'h0 ; */
/*description: Configures flash waiting time after power-up, in unit of ms. If the value is les
s than 15, the waiting time is the configurable value.  Otherwise, the waiting t
ime is twice the configurable value..*/
#define EFUSE_FLASH_TPUW    0x0000000F
#define EFUSE_FLASH_TPUW_M  ((EFUSE_FLASH_TPUW_V)<<(EFUSE_FLASH_TPUW_S))
#define EFUSE_FLASH_TPUW_V  0xF
#define EFUSE_FLASH_TPUW_S  17
/* EFUSE_ENABLE_SECURITY_DOWNLOAD : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: Set this bit to enable secure UART download mode..*/
#define EFUSE_ENABLE_SECURITY_DOWNLOAD    (BIT(16))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_M  (BIT(16))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_V  0x1
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_S  16
/* EFUSE_DIS_DIRECT_BOOT : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: This bit set means disable direct_boot mode..*/
#define EFUSE_DIS_DIRECT_BOOT    (BIT(15))
#define EFUSE_DIS_DIRECT_BOOT_M  (BIT(15))
#define EFUSE_DIS_DIRECT_BOOT_V  0x1
#define EFUSE_DIS_DIRECT_BOOT_S  15
/* EFUSE_DIS_DOWNLOAD_MODE : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: Set this bit to disable download mode (boot_mode[3:0] = 0, 1, 2, 4, 5, 6, 7)..*/
#define EFUSE_DIS_DOWNLOAD_MODE    (BIT(14))
#define EFUSE_DIS_DOWNLOAD_MODE_M  (BIT(14))
#define EFUSE_DIS_DOWNLOAD_MODE_V  0x1
#define EFUSE_DIS_DOWNLOAD_MODE_S  14
/* EFUSE_FORCE_SEND_RESUME : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: Set this bit to force ROM code to send a resume command during SPI boot..*/
#define EFUSE_FORCE_SEND_RESUME    (BIT(13))
#define EFUSE_FORCE_SEND_RESUME_M  (BIT(13))
#define EFUSE_FORCE_SEND_RESUME_V  0x1
#define EFUSE_FORCE_SEND_RESUME_S  13
/* EFUSE_UART_PRINT_CONTROL : RO ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: Set this bit to disable usb printing..*/
#define EFUSE_UART_PRINT_CONTROL    0x00000003
#define EFUSE_UART_PRINT_CONTROL_M  ((EFUSE_UART_PRINT_CONTROL_V)<<(EFUSE_UART_PRINT_CONTROL_S))
#define EFUSE_UART_PRINT_CONTROL_V  0x3
#define EFUSE_UART_PRINT_CONTROL_S  11
/* EFUSE_XTS_KEY_LENGTH_256 : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: The bit be set means XTS_AES use the whole 256-bit efuse data in BLOCK3. Otherwi
se, XTS_AES use 128-bit eFuse data in BLOCK3..*/
#define EFUSE_XTS_KEY_LENGTH_256    (BIT(10))
#define EFUSE_XTS_KEY_LENGTH_256_M  (BIT(10))
#define EFUSE_XTS_KEY_LENGTH_256_V  0x1
#define EFUSE_XTS_KEY_LENGTH_256_S  10
/* EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT : RO ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: These bits be set to enable SPI boot encrypt/decrypt. Odd number of 1: enable. e
ven number of 1: disable..*/
#define EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT    0x00000007
#define EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT_M  ((EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT_V)<<(EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT_S))
#define EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT_V  0x7
#define EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT_S  7
/* EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The bit be set to disable manual encryption..*/
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT    (BIT(6))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_M  (BIT(6))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_V  0x1
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_S  6
/* EFUSE_DIS_DOWNLOAD_ICACHE : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The bit be set to disable icache in download mode..*/
#define EFUSE_DIS_DOWNLOAD_ICACHE    (BIT(5))
#define EFUSE_DIS_DOWNLOAD_ICACHE_M  (BIT(5))
#define EFUSE_DIS_DOWNLOAD_ICACHE_V  0x1
#define EFUSE_DIS_DOWNLOAD_ICACHE_S  5
/* EFUSE_DIS_PAD_JTAG : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to disable pad jtag..*/
#define EFUSE_DIS_PAD_JTAG    (BIT(4))
#define EFUSE_DIS_PAD_JTAG_M  (BIT(4))
#define EFUSE_DIS_PAD_JTAG_V  0x1
#define EFUSE_DIS_PAD_JTAG_S  4
/* EFUSE_WDT_DELAY_SEL : RO ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: Selects RTC watchdog timeout threshold, in unit of slow clock cycle. 0: 40000. 1
: 80000. 2: 160000. 3:320000..*/
#define EFUSE_WDT_DELAY_SEL    0x00000003
#define EFUSE_WDT_DELAY_SEL_M  ((EFUSE_WDT_DELAY_SEL_V)<<(EFUSE_WDT_DELAY_SEL_S))
#define EFUSE_WDT_DELAY_SEL_V  0x3
#define EFUSE_WDT_DELAY_SEL_S  2
/* EFUSE_KEY0_RD_DIS : RO ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: The bit be set to disable software read high/low 128-bit of BLK3..*/
#define EFUSE_KEY0_RD_DIS    0x00000003
#define EFUSE_KEY0_RD_DIS_M  ((EFUSE_KEY0_RD_DIS_V)<<(EFUSE_KEY0_RD_DIS_S))
#define EFUSE_KEY0_RD_DIS_V  0x3
#define EFUSE_KEY0_RD_DIS_S  0

#define EFUSE_RD_BLK1_DATA0_REG          (DR_REG_EFUSE_BASE + 0x34)
/* EFUSE_SYSTEM_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the bits [0:31] of system data..*/
#define EFUSE_SYSTEM_DATA0    0xFFFFFFFF
#define EFUSE_SYSTEM_DATA0_M  ((EFUSE_SYSTEM_DATA0_V)<<(EFUSE_SYSTEM_DATA0_S))
#define EFUSE_SYSTEM_DATA0_V  0xFFFFFFFF
#define EFUSE_SYSTEM_DATA0_S  0

#define EFUSE_RD_BLK1_DATA1_REG          (DR_REG_EFUSE_BASE + 0x38)
/* EFUSE_SYSTEM_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the bits [32:63] of system data..*/
#define EFUSE_SYSTEM_DATA1    0xFFFFFFFF
#define EFUSE_SYSTEM_DATA1_M  ((EFUSE_SYSTEM_DATA1_V)<<(EFUSE_SYSTEM_DATA1_S))
#define EFUSE_SYSTEM_DATA1_V  0xFFFFFFFF
#define EFUSE_SYSTEM_DATA1_S  0

#define EFUSE_RD_BLK1_DATA2_REG          (DR_REG_EFUSE_BASE + 0x3C)
/* EFUSE_SYSTEM_DATA2 : RO ;bitpos:[23:0] ;default: 24'h0 ; */
/*description: Stores the bits [64:87] of system data..*/
#define EFUSE_SYSTEM_DATA2    0x00FFFFFF
#define EFUSE_SYSTEM_DATA2_M  ((EFUSE_SYSTEM_DATA2_V)<<(EFUSE_SYSTEM_DATA2_S))
#define EFUSE_SYSTEM_DATA2_V  0xFFFFFF
#define EFUSE_SYSTEM_DATA2_S  0

#define EFUSE_RD_BLK2_DATA0_REG          (DR_REG_EFUSE_BASE + 0x40)
/* EFUSE_MAC_ID_LOW : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Store the bit [0:31] of MAC..*/
#define EFUSE_MAC_ID_LOW    0xFFFFFFFF
#define EFUSE_MAC_ID_LOW_M  ((EFUSE_MAC_ID_LOW_V)<<(EFUSE_MAC_ID_LOW_S))
#define EFUSE_MAC_ID_LOW_V  0xFFFFFFFF
#define EFUSE_MAC_ID_LOW_S  0

#define EFUSE_RD_BLK2_DATA1_REG          (DR_REG_EFUSE_BASE + 0x44)
/* EFUSE_LDO_VOL_BIAS_CONFIG_LOW : RO ;bitpos:[31:29] ;default: 3'h0 ; */
/*description: Store the bit [0:2] of ido configuration parameters..*/
#define EFUSE_LDO_VOL_BIAS_CONFIG_LOW    0x00000007
#define EFUSE_LDO_VOL_BIAS_CONFIG_LOW_M  ((EFUSE_LDO_VOL_BIAS_CONFIG_LOW_V)<<(EFUSE_LDO_VOL_BIAS_CONFIG_LOW_S))
#define EFUSE_LDO_VOL_BIAS_CONFIG_LOW_V  0x7
#define EFUSE_LDO_VOL_BIAS_CONFIG_LOW_S  29
/* EFUSE_RF_REF_I_BIAS_CONFIG : RO ;bitpos:[28:25] ;default: 4'h0 ; */
/*description: Store rf configuration parameters..*/
#define EFUSE_RF_REF_I_BIAS_CONFIG    0x0000000F
#define EFUSE_RF_REF_I_BIAS_CONFIG_M  ((EFUSE_RF_REF_I_BIAS_CONFIG_V)<<(EFUSE_RF_REF_I_BIAS_CONFIG_S))
#define EFUSE_RF_REF_I_BIAS_CONFIG_V  0xF
#define EFUSE_RF_REF_I_BIAS_CONFIG_S  25
/* EFUSE_BLK2_EFUSE_VERSION : RO ;bitpos:[24:22] ;default: 3'h0 ; */
/*description: Store efuse version..*/
#define EFUSE_BLK2_EFUSE_VERSION    0x00000007
#define EFUSE_BLK2_EFUSE_VERSION_M  ((EFUSE_BLK2_EFUSE_VERSION_V)<<(EFUSE_BLK2_EFUSE_VERSION_S))
#define EFUSE_BLK2_EFUSE_VERSION_V  0x7
#define EFUSE_BLK2_EFUSE_VERSION_S  22
/* EFUSE_PKG_VERSION : RO ;bitpos:[21:19] ;default: 3'h0 ; */
/*description: Store package version..*/
#define EFUSE_PKG_VERSION    0x00000007
#define EFUSE_PKG_VERSION_M  ((EFUSE_PKG_VERSION_V)<<(EFUSE_PKG_VERSION_S))
#define EFUSE_PKG_VERSION_V  0x7
#define EFUSE_PKG_VERSION_S  19
/* EFUSE_WAFER_VERSION : RO ;bitpos:[18:16] ;default: 3'h0 ; */
/*description: Store wafer version..*/
#define EFUSE_WAFER_VERSION    0x00000007
#define EFUSE_WAFER_VERSION_M  ((EFUSE_WAFER_VERSION_V)<<(EFUSE_WAFER_VERSION_S))
#define EFUSE_WAFER_VERSION_V  0x7
#define EFUSE_WAFER_VERSION_S  16
/* EFUSE_MAC_ID_HIGH : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: Store the bit [31:47] of MAC..*/
#define EFUSE_MAC_ID_HIGH    0x0000FFFF
#define EFUSE_MAC_ID_HIGH_M  ((EFUSE_MAC_ID_HIGH_V)<<(EFUSE_MAC_ID_HIGH_S))
#define EFUSE_MAC_ID_HIGH_V  0xFFFF
#define EFUSE_MAC_ID_HIGH_S  0

#define EFUSE_RD_BLK2_DATA2_REG          (DR_REG_EFUSE_BASE + 0x48)
/* EFUSE_PVT_LOW : RO ;bitpos:[31:27] ;default: 5'h0 ; */
/*description: Store the bit [0:4] of pvt..*/
#define EFUSE_PVT_LOW    0x0000001F
#define EFUSE_PVT_LOW_M  ((EFUSE_PVT_LOW_V)<<(EFUSE_PVT_LOW_S))
#define EFUSE_PVT_LOW_V  0x1F
#define EFUSE_PVT_LOW_S  27
/* EFUSE_LDO_VOL_BIAS_CONFIG_HIGH : RO ;bitpos:[26:0] ;default: 27'h0 ; */
/*description: Store the bit [3:29] of ido configuration parameters..*/
#define EFUSE_LDO_VOL_BIAS_CONFIG_HIGH    0x07FFFFFF
#define EFUSE_LDO_VOL_BIAS_CONFIG_HIGH_M  ((EFUSE_LDO_VOL_BIAS_CONFIG_HIGH_V)<<(EFUSE_LDO_VOL_BIAS_CONFIG_HIGH_S))
#define EFUSE_LDO_VOL_BIAS_CONFIG_HIGH_V  0x7FFFFFF
#define EFUSE_LDO_VOL_BIAS_CONFIG_HIGH_S  0

#define EFUSE_RD_BLK2_DATA3_REG          (DR_REG_EFUSE_BASE + 0x4C)
/* EFUSE_ADC_CALIBRATION_0 : RO ;bitpos:[31:10] ;default: 22'h0 ; */
/*description: Store the bit [0:21] of ADC calibration data..*/
#define EFUSE_ADC_CALIBRATION_0    0x003FFFFF
#define EFUSE_ADC_CALIBRATION_0_M  ((EFUSE_ADC_CALIBRATION_0_V)<<(EFUSE_ADC_CALIBRATION_0_S))
#define EFUSE_ADC_CALIBRATION_0_V  0x3FFFFF
#define EFUSE_ADC_CALIBRATION_0_S  10
/* EFUSE_PVT_HIGH : RO ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: Store the bit [5:14] of pvt..*/
#define EFUSE_PVT_HIGH    0x000003FF
#define EFUSE_PVT_HIGH_M  ((EFUSE_PVT_HIGH_V)<<(EFUSE_PVT_HIGH_S))
#define EFUSE_PVT_HIGH_V  0x3FF
#define EFUSE_PVT_HIGH_S  0

#define EFUSE_RD_BLK2_DATA4_REG          (DR_REG_EFUSE_BASE + 0x50)
/* EFUSE_ADC_CALIBRATION_1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Store the bit [22:53] of ADC calibration data..*/
#define EFUSE_ADC_CALIBRATION_1    0xFFFFFFFF
#define EFUSE_ADC_CALIBRATION_1_M  ((EFUSE_ADC_CALIBRATION_1_V)<<(EFUSE_ADC_CALIBRATION_1_S))
#define EFUSE_ADC_CALIBRATION_1_V  0xFFFFFFFF
#define EFUSE_ADC_CALIBRATION_1_S  0

#define EFUSE_RD_BLK2_DATA5_REG          (DR_REG_EFUSE_BASE + 0x54)
/* EFUSE_ADC_CALIBRATION_2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Store the bit [54:85] of ADC calibration data..*/
#define EFUSE_ADC_CALIBRATION_2    0xFFFFFFFF
#define EFUSE_ADC_CALIBRATION_2_M  ((EFUSE_ADC_CALIBRATION_2_V)<<(EFUSE_ADC_CALIBRATION_2_S))
#define EFUSE_ADC_CALIBRATION_2_V  0xFFFFFFFF
#define EFUSE_ADC_CALIBRATION_2_S  0

#define EFUSE_RD_BLK2_DATA6_REG          (DR_REG_EFUSE_BASE + 0x58)
/* EFUSE_BLK2_RESERVED_DATA_0 : RO ;bitpos:[31:11] ;default: 21'h0 ; */
/*description: Store the bit [0:20] of block2 reserved data..*/
#define EFUSE_BLK2_RESERVED_DATA_0    0x001FFFFF
#define EFUSE_BLK2_RESERVED_DATA_0_M  ((EFUSE_BLK2_RESERVED_DATA_0_V)<<(EFUSE_BLK2_RESERVED_DATA_0_S))
#define EFUSE_BLK2_RESERVED_DATA_0_V  0x1FFFFF
#define EFUSE_BLK2_RESERVED_DATA_0_S  11
/* EFUSE_ADC_CALIBRATION_3 : RO ;bitpos:[10:0] ;default: 11'h0 ; */
/*description: Store the bit [86:96] of ADC calibration data..*/
#define EFUSE_ADC_CALIBRATION_3    0x000007FF
#define EFUSE_ADC_CALIBRATION_3_M  ((EFUSE_ADC_CALIBRATION_3_V)<<(EFUSE_ADC_CALIBRATION_3_S))
#define EFUSE_ADC_CALIBRATION_3_V  0x7FF
#define EFUSE_ADC_CALIBRATION_3_S  0

#define EFUSE_RD_BLK2_DATA7_REG          (DR_REG_EFUSE_BASE + 0x5C)
/* EFUSE_BLK2_RESERVED_DATA_1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Store the bit [21:52] of block2 reserved data..*/
#define EFUSE_BLK2_RESERVED_DATA_1    0xFFFFFFFF
#define EFUSE_BLK2_RESERVED_DATA_1_M  ((EFUSE_BLK2_RESERVED_DATA_1_V)<<(EFUSE_BLK2_RESERVED_DATA_1_S))
#define EFUSE_BLK2_RESERVED_DATA_1_V  0xFFFFFFFF
#define EFUSE_BLK2_RESERVED_DATA_1_S  0

#define EFUSE_RD_BLK3_DATA0_REG          (DR_REG_EFUSE_BASE + 0x60)
/* EFUSE_BLK3_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Store the first 32-bit of Block3..*/
#define EFUSE_BLK3_DATA0    0xFFFFFFFF
#define EFUSE_BLK3_DATA0_M  ((EFUSE_BLK3_DATA0_V)<<(EFUSE_BLK3_DATA0_S))
#define EFUSE_BLK3_DATA0_V  0xFFFFFFFF
#define EFUSE_BLK3_DATA0_S  0

#define EFUSE_RD_BLK3_DATA1_REG          (DR_REG_EFUSE_BASE + 0x64)
/* EFUSE_BLK3_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Store the second 32-bit of Block3..*/
#define EFUSE_BLK3_DATA1    0xFFFFFFFF
#define EFUSE_BLK3_DATA1_M  ((EFUSE_BLK3_DATA1_V)<<(EFUSE_BLK3_DATA1_S))
#define EFUSE_BLK3_DATA1_V  0xFFFFFFFF
#define EFUSE_BLK3_DATA1_S  0

#define EFUSE_RD_BLK3_DATA2_REG          (DR_REG_EFUSE_BASE + 0x68)
/* EFUSE_BLK3_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Store the third 32-bit of Block3..*/
#define EFUSE_BLK3_DATA2    0xFFFFFFFF
#define EFUSE_BLK3_DATA2_M  ((EFUSE_BLK3_DATA2_V)<<(EFUSE_BLK3_DATA2_S))
#define EFUSE_BLK3_DATA2_V  0xFFFFFFFF
#define EFUSE_BLK3_DATA2_S  0

#define EFUSE_RD_BLK3_DATA3_REG          (DR_REG_EFUSE_BASE + 0x6C)
/* EFUSE_BLK3_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Store the fourth 32-bit of Block3..*/
#define EFUSE_BLK3_DATA3    0xFFFFFFFF
#define EFUSE_BLK3_DATA3_M  ((EFUSE_BLK3_DATA3_V)<<(EFUSE_BLK3_DATA3_S))
#define EFUSE_BLK3_DATA3_V  0xFFFFFFFF
#define EFUSE_BLK3_DATA3_S  0

#define EFUSE_RD_BLK3_DATA4_REG          (DR_REG_EFUSE_BASE + 0x70)
/* EFUSE_BLK3_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Store the fifth 32-bit of Block3..*/
#define EFUSE_BLK3_DATA4    0xFFFFFFFF
#define EFUSE_BLK3_DATA4_M  ((EFUSE_BLK3_DATA4_V)<<(EFUSE_BLK3_DATA4_S))
#define EFUSE_BLK3_DATA4_V  0xFFFFFFFF
#define EFUSE_BLK3_DATA4_S  0

#define EFUSE_RD_BLK3_DATA5_REG          (DR_REG_EFUSE_BASE + 0x74)
/* EFUSE_BLK3_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Store the sixth 32-bit of Block3..*/
#define EFUSE_BLK3_DATA5    0xFFFFFFFF
#define EFUSE_BLK3_DATA5_M  ((EFUSE_BLK3_DATA5_V)<<(EFUSE_BLK3_DATA5_S))
#define EFUSE_BLK3_DATA5_V  0xFFFFFFFF
#define EFUSE_BLK3_DATA5_S  0

#define EFUSE_RD_BLK3_DATA6_REG          (DR_REG_EFUSE_BASE + 0x78)
/* EFUSE_BLK3_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Store the seventh 32-bit of Block3..*/
#define EFUSE_BLK3_DATA6    0xFFFFFFFF
#define EFUSE_BLK3_DATA6_M  ((EFUSE_BLK3_DATA6_V)<<(EFUSE_BLK3_DATA6_S))
#define EFUSE_BLK3_DATA6_V  0xFFFFFFFF
#define EFUSE_BLK3_DATA6_S  0

#define EFUSE_RD_BLK3_DATA7_REG          (DR_REG_EFUSE_BASE + 0x7C)
/* EFUSE_BLK3_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Store the eighth 32-bit of Block3..*/
#define EFUSE_BLK3_DATA7    0xFFFFFFFF
#define EFUSE_BLK3_DATA7_M  ((EFUSE_BLK3_DATA7_V)<<(EFUSE_BLK3_DATA7_S))
#define EFUSE_BLK3_DATA7_V  0xFFFFFFFF
#define EFUSE_BLK3_DATA7_S  0

#define EFUSE_RD_REPEAT_ERR_REG          (DR_REG_EFUSE_BASE + 0x80)
/* EFUSE_RPT4_RESERVED_ERR : RO ;bitpos:[31:22] ;default: 12'h0 ; */
/*description: Reserved..*/
#define EFUSE_RPT4_RESERVED_ERR    0x000003FF
#define EFUSE_RPT4_RESERVED_ERR_M  ((EFUSE_RPT4_RESERVED_ERR_V)<<(EFUSE_RPT4_RESERVED_ERR_S))
#define EFUSE_RPT4_RESERVED_ERR_V  0x3FF
#define EFUSE_RPT4_RESERVED_ERR_S  22
/* EFUSE_SECURE_BOOT_EN_ERR : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: If any bit in this filed is 1, then it indicates a programming error..*/
#define EFUSE_SECURE_BOOT_EN_ERR    (BIT(21))
#define EFUSE_SECURE_BOOT_EN_ERR_M  (BIT(21))
#define EFUSE_SECURE_BOOT_EN_ERR_V  0x1
#define EFUSE_SECURE_BOOT_EN_ERR_S  21
/* EFUSE_FLASH_TPUW_ERR : RO ;bitpos:[20:17] ;default: 4'h0 ; */
/*description: If any bit in this filed is 1, then it indicates a programming error..*/
#define EFUSE_FLASH_TPUW_ERR    0x0000000F
#define EFUSE_FLASH_TPUW_ERR_M  ((EFUSE_FLASH_TPUW_ERR_V)<<(EFUSE_FLASH_TPUW_ERR_S))
#define EFUSE_FLASH_TPUW_ERR_V  0xF
#define EFUSE_FLASH_TPUW_ERR_S  17
/* EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: If any bit in this filed is 1, then it indicates a programming error..*/
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR    (BIT(16))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_M  (BIT(16))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_V  0x1
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_S  16
/* EFUSE_DIS_DIRECT_BOOT_ERR : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: If any bit in this filed is 1, then it indicates a programming error..*/
#define EFUSE_DIS_DIRECT_BOOT_ERR    (BIT(15))
#define EFUSE_DIS_DIRECT_BOOT_ERR_M  (BIT(15))
#define EFUSE_DIS_DIRECT_BOOT_ERR_V  0x1
#define EFUSE_DIS_DIRECT_BOOT_ERR_S  15
/* EFUSE_DIS_DOWNLOAD_MODE_ERR : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: If any bit in this filed is 1, then it indicates a programming error..*/
#define EFUSE_DIS_DOWNLOAD_MODE_ERR    (BIT(14))
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_M  (BIT(14))
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_V  0x1
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_S  14
/* EFUSE_FORCE_SEND_RESUME_ERR : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: If any bit in FORCE_SEND_RESUME is 1, then it indicates a programming error..*/
#define EFUSE_FORCE_SEND_RESUME_ERR    (BIT(13))
#define EFUSE_FORCE_SEND_RESUME_ERR_M  (BIT(13))
#define EFUSE_FORCE_SEND_RESUME_ERR_V  0x1
#define EFUSE_FORCE_SEND_RESUME_ERR_S  13
/* EFUSE_UART_PRINT_CONTROL_ERR : RO ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: If any bit in UART_PRINT_CONTROL is 1, then it indicates a programming error..*/
#define EFUSE_UART_PRINT_CONTROL_ERR    0x00000003
#define EFUSE_UART_PRINT_CONTROL_ERR_M  ((EFUSE_UART_PRINT_CONTROL_ERR_V)<<(EFUSE_UART_PRINT_CONTROL_ERR_S))
#define EFUSE_UART_PRINT_CONTROL_ERR_V  0x3
#define EFUSE_UART_PRINT_CONTROL_ERR_S  11
/* EFUSE_XTS_KEY_LENGTH_256_ERR : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: If any bit in XTS_KEY_LENGTH_256 is 1, then it indicates a programming error..*/
#define EFUSE_XTS_KEY_LENGTH_256_ERR    (BIT(10))
#define EFUSE_XTS_KEY_LENGTH_256_ERR_M  (BIT(10))
#define EFUSE_XTS_KEY_LENGTH_256_ERR_V  0x1
#define EFUSE_XTS_KEY_LENGTH_256_ERR_S  10
/* EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT_ERR : RO ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: If any bit in SPI_BOOT_ENCRYPT_DECRYPT_CNT is 1, then it indicates a programming
 error..*/
#define EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT_ERR    0x00000007
#define EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT_ERR_M  ((EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT_ERR_V)<<(EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT_ERR_S))
#define EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT_ERR_V  0x7
#define EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT_ERR_S  7
/* EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: If any bit in DIS_DOWNLOAD_MANUAL_ENCRYPT is 1, then it indicates a programming
error..*/
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR    (BIT(6))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_M  (BIT(6))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_V  0x1
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_S  6
/* EFUSE_DIS_DOWNLOAD_ICACHE_ERR : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: If any bit in this filed is 1, then it indicates a programming error..*/
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR    (BIT(5))
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_M  (BIT(5))
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_V  0x1
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_S  5
/* EFUSE_DIS_PAD_JTAG_ERR : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: If any bit in DIS_PAD_JTAG is 1, then it indicates a programming error..*/
#define EFUSE_DIS_PAD_JTAG_ERR    (BIT(4))
#define EFUSE_DIS_PAD_JTAG_ERR_M  (BIT(4))
#define EFUSE_DIS_PAD_JTAG_ERR_V  0x1
#define EFUSE_DIS_PAD_JTAG_ERR_S  4
/* EFUSE_WDT_DELAY_SEL_ERR : RO ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: If any bit in WDT_DELAY_SEL is 1, then it indicates a programming error..*/
#define EFUSE_WDT_DELAY_SEL_ERR    0x00000003
#define EFUSE_WDT_DELAY_SEL_ERR_M  ((EFUSE_WDT_DELAY_SEL_ERR_V)<<(EFUSE_WDT_DELAY_SEL_ERR_S))
#define EFUSE_WDT_DELAY_SEL_ERR_V  0x3
#define EFUSE_WDT_DELAY_SEL_ERR_S  2
/* EFUSE_KEY0_RD_DIS_ERR : RO ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: If any bit in RD_DIS is 1, then it indicates a programming error..*/
#define EFUSE_KEY0_RD_DIS_ERR    0x00000003
#define EFUSE_KEY0_RD_DIS_ERR_M  ((EFUSE_KEY0_RD_DIS_ERR_V)<<(EFUSE_KEY0_RD_DIS_ERR_S))
#define EFUSE_KEY0_RD_DIS_ERR_V  0x3
#define EFUSE_KEY0_RD_DIS_ERR_S  0

#define EFUSE_RD_RS_ERR_REG          (DR_REG_EFUSE_BASE + 0x84)
/* EFUSE_BLK3_FAIL : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the block3 data is reliable 1: Means that programmi
ng user data failed and the number of error bytes is over 6..*/
#define EFUSE_BLK3_FAIL    (BIT(11))
#define EFUSE_BLK3_FAIL_M  (BIT(11))
#define EFUSE_BLK3_FAIL_V  0x1
#define EFUSE_BLK3_FAIL_S  11
/* EFUSE_BLK3_ERR_NUM : RO ;bitpos:[10:8] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes in block3..*/
#define EFUSE_BLK3_ERR_NUM    0x00000007
#define EFUSE_BLK3_ERR_NUM_M  ((EFUSE_BLK3_ERR_NUM_V)<<(EFUSE_BLK3_ERR_NUM_S))
#define EFUSE_BLK3_ERR_NUM_V  0x7
#define EFUSE_BLK3_ERR_NUM_S  8
/* EFUSE_BLK2_FAIL : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of block2 is reliable 1: Means that progra
mming user data failed and the number of error bytes is over 6..*/
#define EFUSE_BLK2_FAIL    (BIT(7))
#define EFUSE_BLK2_FAIL_M  (BIT(7))
#define EFUSE_BLK2_FAIL_V  0x1
#define EFUSE_BLK2_FAIL_S  7
/* EFUSE_BLK2_ERR_NUM : RO ;bitpos:[6:4] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes in block2..*/
#define EFUSE_BLK2_ERR_NUM    0x00000007
#define EFUSE_BLK2_ERR_NUM_M  ((EFUSE_BLK2_ERR_NUM_V)<<(EFUSE_BLK2_ERR_NUM_S))
#define EFUSE_BLK2_ERR_NUM_V  0x7
#define EFUSE_BLK2_ERR_NUM_S  4
/* EFUSE_BLK1_FAIL : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of block1 is reliable 1: Means that progra
mming user data failed and the number of error bytes is over 6..*/
#define EFUSE_BLK1_FAIL    (BIT(3))
#define EFUSE_BLK1_FAIL_M  (BIT(3))
#define EFUSE_BLK1_FAIL_V  0x1
#define EFUSE_BLK1_FAIL_S  3
/* EFUSE_BLK1_ERR_NUM : RO ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes in block1..*/
#define EFUSE_BLK1_ERR_NUM    0x00000007
#define EFUSE_BLK1_ERR_NUM_M  ((EFUSE_BLK1_ERR_NUM_V)<<(EFUSE_BLK1_ERR_NUM_S))
#define EFUSE_BLK1_ERR_NUM_V  0x7
#define EFUSE_BLK1_ERR_NUM_S  0

#define EFUSE_CLK_REG          (DR_REG_EFUSE_BASE + 0x88)
/* EFUSE_CLK_EN : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: Set this bit and force to enable clock signal of eFuse memory..*/
#define EFUSE_CLK_EN    (BIT(16))
#define EFUSE_CLK_EN_M  (BIT(16))
#define EFUSE_CLK_EN_V  0x1
#define EFUSE_CLK_EN_S  16
/* EFUSE_EFUSE_MEM_FORCE_PU : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to force eFuse SRAM into working mode..*/
#define EFUSE_EFUSE_MEM_FORCE_PU    (BIT(2))
#define EFUSE_EFUSE_MEM_FORCE_PU_M  (BIT(2))
#define EFUSE_EFUSE_MEM_FORCE_PU_V  0x1
#define EFUSE_EFUSE_MEM_FORCE_PU_S  2
/* EFUSE_MEM_CLK_FORCE_ON : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: Set this bit and force to activate clock signal of eFuse SRAM..*/
#define EFUSE_MEM_CLK_FORCE_ON    (BIT(1))
#define EFUSE_MEM_CLK_FORCE_ON_M  (BIT(1))
#define EFUSE_MEM_CLK_FORCE_ON_V  0x1
#define EFUSE_MEM_CLK_FORCE_ON_S  1
/* EFUSE_EFUSE_MEM_FORCE_PD : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to force eFuse SRAM into power-saving mode..*/
#define EFUSE_EFUSE_MEM_FORCE_PD    (BIT(0))
#define EFUSE_EFUSE_MEM_FORCE_PD_M  (BIT(0))
#define EFUSE_EFUSE_MEM_FORCE_PD_V  0x1
#define EFUSE_EFUSE_MEM_FORCE_PD_S  0

#define EFUSE_CONF_REG          (DR_REG_EFUSE_BASE + 0x8C)
/* EFUSE_OP_CODE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: 0x5A5A: Operate programming command 0x5AA5: Operate read command..*/
#define EFUSE_OP_CODE    0x0000FFFF
#define EFUSE_OP_CODE_M  ((EFUSE_OP_CODE_V)<<(EFUSE_OP_CODE_S))
#define EFUSE_OP_CODE_V  0xFFFF
#define EFUSE_OP_CODE_S  0

#define EFUSE_STATUS_REG          (DR_REG_EFUSE_BASE + 0x90)
/* EFUSE_BLK0_VALID_BIT_CNT : RO ;bitpos:[15:10] ;default: 8'h0 ; */
/*description: Record the number of bit '1' in BLOCK0..*/
#define EFUSE_BLK0_VALID_BIT_CNT    0x0000003F
#define EFUSE_BLK0_VALID_BIT_CNT_M  ((EFUSE_BLK0_VALID_BIT_CNT_V)<<(EFUSE_BLK0_VALID_BIT_CNT_S))
#define EFUSE_BLK0_VALID_BIT_CNT_V  0x3F
#define EFUSE_BLK0_VALID_BIT_CNT_S  10
/* EFUSE_OTP_VDDQ_IS_SW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The value of OTP_VDDQ_IS_SW..*/
#define EFUSE_OTP_VDDQ_IS_SW    (BIT(9))
#define EFUSE_OTP_VDDQ_IS_SW_M  (BIT(9))
#define EFUSE_OTP_VDDQ_IS_SW_V  0x1
#define EFUSE_OTP_VDDQ_IS_SW_S  9
/* EFUSE_OTP_PGENB_SW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The value of OTP_PGENB_SW..*/
#define EFUSE_OTP_PGENB_SW    (BIT(8))
#define EFUSE_OTP_PGENB_SW_M  (BIT(8))
#define EFUSE_OTP_PGENB_SW_V  0x1
#define EFUSE_OTP_PGENB_SW_S  8
/* EFUSE_OTP_CSB_SW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The value of OTP_CSB_SW..*/
#define EFUSE_OTP_CSB_SW    (BIT(7))
#define EFUSE_OTP_CSB_SW_M  (BIT(7))
#define EFUSE_OTP_CSB_SW_V  0x1
#define EFUSE_OTP_CSB_SW_S  7
/* EFUSE_OTP_STROBE_SW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The value of OTP_STROBE_SW..*/
#define EFUSE_OTP_STROBE_SW    (BIT(6))
#define EFUSE_OTP_STROBE_SW_M  (BIT(6))
#define EFUSE_OTP_STROBE_SW_V  0x1
#define EFUSE_OTP_STROBE_SW_S  6
/* EFUSE_OTP_VDDQ_C_SYNC2 : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The value of OTP_VDDQ_C_SYNC2..*/
#define EFUSE_OTP_VDDQ_C_SYNC2    (BIT(5))
#define EFUSE_OTP_VDDQ_C_SYNC2_M  (BIT(5))
#define EFUSE_OTP_VDDQ_C_SYNC2_V  0x1
#define EFUSE_OTP_VDDQ_C_SYNC2_S  5
/* EFUSE_OTP_LOAD_SW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The value of OTP_LOAD_SW..*/
#define EFUSE_OTP_LOAD_SW    (BIT(4))
#define EFUSE_OTP_LOAD_SW_M  (BIT(4))
#define EFUSE_OTP_LOAD_SW_V  0x1
#define EFUSE_OTP_LOAD_SW_S  4
/* EFUSE_STATE : RO ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: Indicates the state of the eFuse state machine..*/
#define EFUSE_STATE    0x0000000F
#define EFUSE_STATE_M  ((EFUSE_STATE_V)<<(EFUSE_STATE_S))
#define EFUSE_STATE_V  0xF
#define EFUSE_STATE_S  0

#define EFUSE_CMD_REG          (DR_REG_EFUSE_BASE + 0x94)
/* EFUSE_BLK_NUM : R/W ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: The serial number of the block to be programmed. Value 0-3 corresponds to block
number 0-3, respectively..*/
#define EFUSE_BLK_NUM    0x00000003
#define EFUSE_BLK_NUM_M  ((EFUSE_BLK_NUM_V)<<(EFUSE_BLK_NUM_S))
#define EFUSE_BLK_NUM_V  0x3
#define EFUSE_BLK_NUM_S  2
/* EFUSE_PGM_CMD : R/W/SC ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to send programming command..*/
#define EFUSE_PGM_CMD    (BIT(1))
#define EFUSE_PGM_CMD_M  (BIT(1))
#define EFUSE_PGM_CMD_V  0x1
#define EFUSE_PGM_CMD_S  1
/* EFUSE_READ_CMD : R/W/SC ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to send read command..*/
#define EFUSE_READ_CMD    (BIT(0))
#define EFUSE_READ_CMD_M  (BIT(0))
#define EFUSE_READ_CMD_V  0x1
#define EFUSE_READ_CMD_S  0

#define EFUSE_INT_RAW_REG          (DR_REG_EFUSE_BASE + 0x98)
/* EFUSE_PGM_DONE_INT_RAW : R/WTC/SS ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw bit signal for pgm_done interrupt..*/
#define EFUSE_PGM_DONE_INT_RAW    (BIT(1))
#define EFUSE_PGM_DONE_INT_RAW_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_RAW_V  0x1
#define EFUSE_PGM_DONE_INT_RAW_S  1
/* EFUSE_READ_DONE_INT_RAW : R/WTC/SS ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw bit signal for read_done interrupt..*/
#define EFUSE_READ_DONE_INT_RAW    (BIT(0))
#define EFUSE_READ_DONE_INT_RAW_M  (BIT(0))
#define EFUSE_READ_DONE_INT_RAW_V  0x1
#define EFUSE_READ_DONE_INT_RAW_S  0

#define EFUSE_INT_ST_REG          (DR_REG_EFUSE_BASE + 0x9C)
/* EFUSE_PGM_DONE_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The status signal for pgm_done interrupt..*/
#define EFUSE_PGM_DONE_INT_ST    (BIT(1))
#define EFUSE_PGM_DONE_INT_ST_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_ST_V  0x1
#define EFUSE_PGM_DONE_INT_ST_S  1
/* EFUSE_READ_DONE_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The status signal for read_done interrupt..*/
#define EFUSE_READ_DONE_INT_ST    (BIT(0))
#define EFUSE_READ_DONE_INT_ST_M  (BIT(0))
#define EFUSE_READ_DONE_INT_ST_V  0x1
#define EFUSE_READ_DONE_INT_ST_S  0

#define EFUSE_INT_ENA_REG          (DR_REG_EFUSE_BASE + 0x100)
/* EFUSE_PGM_DONE_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The enable signal for pgm_done interrupt..*/
#define EFUSE_PGM_DONE_INT_ENA    (BIT(1))
#define EFUSE_PGM_DONE_INT_ENA_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_ENA_V  0x1
#define EFUSE_PGM_DONE_INT_ENA_S  1
/* EFUSE_READ_DONE_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The enable signal for read_done interrupt..*/
#define EFUSE_READ_DONE_INT_ENA    (BIT(0))
#define EFUSE_READ_DONE_INT_ENA_M  (BIT(0))
#define EFUSE_READ_DONE_INT_ENA_V  0x1
#define EFUSE_READ_DONE_INT_ENA_S  0

#define EFUSE_INT_CLR_REG          (DR_REG_EFUSE_BASE + 0x104)
/* EFUSE_PGM_DONE_INT_CLR : WT ;bitpos:[1] ;default: 1'b0 ; */
/*description: The clear signal for pgm_done interrupt..*/
#define EFUSE_PGM_DONE_INT_CLR    (BIT(1))
#define EFUSE_PGM_DONE_INT_CLR_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_CLR_V  0x1
#define EFUSE_PGM_DONE_INT_CLR_S  1
/* EFUSE_READ_DONE_INT_CLR : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: The clear signal for read_done interrupt..*/
#define EFUSE_READ_DONE_INT_CLR    (BIT(0))
#define EFUSE_READ_DONE_INT_CLR_M  (BIT(0))
#define EFUSE_READ_DONE_INT_CLR_V  0x1
#define EFUSE_READ_DONE_INT_CLR_S  0

#define EFUSE_DAC_CONF_REG          (DR_REG_EFUSE_BASE + 0x108)
/* EFUSE_OE_CLR : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: Reduces the power supply of the programming voltage..*/
#define EFUSE_OE_CLR    (BIT(17))
#define EFUSE_OE_CLR_M  (BIT(17))
#define EFUSE_OE_CLR_V  0x1
#define EFUSE_OE_CLR_S  17
/* EFUSE_DAC_NUM : R/W ;bitpos:[16:9] ;default: 8'd255 ; */
/*description: Controls the rising period of the programming voltage..*/
#define EFUSE_DAC_NUM    0x000000FF
#define EFUSE_DAC_NUM_M  ((EFUSE_DAC_NUM_V)<<(EFUSE_DAC_NUM_S))
#define EFUSE_DAC_NUM_V  0xFF
#define EFUSE_DAC_NUM_S  9
/* EFUSE_DAC_CLK_PAD_SEL : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Don't care..*/
#define EFUSE_DAC_CLK_PAD_SEL    (BIT(8))
#define EFUSE_DAC_CLK_PAD_SEL_M  (BIT(8))
#define EFUSE_DAC_CLK_PAD_SEL_V  0x1
#define EFUSE_DAC_CLK_PAD_SEL_S  8
/* EFUSE_DAC_CLK_DIV : R/W ;bitpos:[7:0] ;default: 8'd28 ; */
/*description: Controls the division factor of the rising clock of the programming voltage..*/
#define EFUSE_DAC_CLK_DIV    0x000000FF
#define EFUSE_DAC_CLK_DIV_M  ((EFUSE_DAC_CLK_DIV_V)<<(EFUSE_DAC_CLK_DIV_S))
#define EFUSE_DAC_CLK_DIV_V  0xFF
#define EFUSE_DAC_CLK_DIV_S  0

#define EFUSE_RD_TIM_CONF_REG          (DR_REG_EFUSE_BASE + 0x10C)
/* EFUSE_READ_INIT_NUM : R/W ;bitpos:[31:24] ;default: 8'h12 ; */
/*description: Configures the initial read time of eFuse..*/
#define EFUSE_READ_INIT_NUM    0x000000FF
#define EFUSE_READ_INIT_NUM_M  ((EFUSE_READ_INIT_NUM_V)<<(EFUSE_READ_INIT_NUM_S))
#define EFUSE_READ_INIT_NUM_V  0xFF
#define EFUSE_READ_INIT_NUM_S  24
/* EFUSE_TSUR_A : R/W ;bitpos:[23:16] ;default: 8'h1 ; */
/*description: Configures setup time for efuse read..*/
#define EFUSE_TSUR_A    0x000000FF
#define EFUSE_TSUR_A_M  ((EFUSE_TSUR_A_V)<<(EFUSE_TSUR_A_S))
#define EFUSE_TSUR_A_V  0xFF
#define EFUSE_TSUR_A_S  16
/* EFUSE_TRD : R/W ;bitpos:[15:8] ;default: 8'h2 ; */
/*description: Configures pulse time for efuse read..*/
#define EFUSE_TRD    0x000000FF
#define EFUSE_TRD_M  ((EFUSE_TRD_V)<<(EFUSE_TRD_S))
#define EFUSE_TRD_V  0xFF
#define EFUSE_TRD_S  8
/* EFUSE_THR_A : R/W ;bitpos:[7:0] ;default: 8'h1 ; */
/*description: Configures hold time for efuse read..*/
#define EFUSE_THR_A    0x000000FF
#define EFUSE_THR_A_M  ((EFUSE_THR_A_V)<<(EFUSE_THR_A_S))
#define EFUSE_THR_A_V  0xFF
#define EFUSE_THR_A_S  0

#define EFUSE_WR_TIM_CONF0_REG          (DR_REG_EFUSE_BASE + 0x110)
/* EFUSE_TPGM : R/W ;bitpos:[31:16] ;default: 16'hc8 ; */
/*description: Configures pulse time for burning '1' bit..*/
#define EFUSE_TPGM    0x0000FFFF
#define EFUSE_TPGM_M  ((EFUSE_TPGM_V)<<(EFUSE_TPGM_S))
#define EFUSE_TPGM_V  0xFFFF
#define EFUSE_TPGM_S  16
/* EFUSE_TPGM_INACTIVE : R/W ;bitpos:[15:8] ;default: 8'h1 ; */
/*description: Configures pulse time for burning '0' bit..*/
#define EFUSE_TPGM_INACTIVE    0x000000FF
#define EFUSE_TPGM_INACTIVE_M  ((EFUSE_TPGM_INACTIVE_V)<<(EFUSE_TPGM_INACTIVE_S))
#define EFUSE_TPGM_INACTIVE_V  0xFF
#define EFUSE_TPGM_INACTIVE_S  8
/* EFUSE_THP_A : R/W ;bitpos:[7:0] ;default: 8'h1 ; */
/*description: Configures hold time for efuse program..*/
#define EFUSE_THP_A    0x000000FF
#define EFUSE_THP_A_M  ((EFUSE_THP_A_V)<<(EFUSE_THP_A_S))
#define EFUSE_THP_A_V  0xFF
#define EFUSE_THP_A_S  0

#define EFUSE_WR_TIM_CONF1_REG          (DR_REG_EFUSE_BASE + 0x114)
/* EFUSE_PWR_ON_NUM : R/W ;bitpos:[23:8] ;default: 16'h3000 ; */
/*description: Configures the power up time for VDDQ..*/
#define EFUSE_PWR_ON_NUM    0x0000FFFF
#define EFUSE_PWR_ON_NUM_M  ((EFUSE_PWR_ON_NUM_V)<<(EFUSE_PWR_ON_NUM_S))
#define EFUSE_PWR_ON_NUM_V  0xFFFF
#define EFUSE_PWR_ON_NUM_S  8
/* EFUSE_TSUP_A : R/W ;bitpos:[7:0] ;default: 8'h1 ; */
/*description: Configures setup time for efuse program..*/
#define EFUSE_TSUP_A    0x000000FF
#define EFUSE_TSUP_A_M  ((EFUSE_TSUP_A_V)<<(EFUSE_TSUP_A_S))
#define EFUSE_TSUP_A_V  0xFF
#define EFUSE_TSUP_A_S  0

#define EFUSE_WR_TIM_CONF2_REG          (DR_REG_EFUSE_BASE + 0x118)
/* EFUSE_PWR_OFF_NUM : R/W ;bitpos:[15:0] ;default: 16'h190 ; */
/*description: Configures the power outage time for VDDQ..*/
#define EFUSE_PWR_OFF_NUM    0x0000FFFF
#define EFUSE_PWR_OFF_NUM_M  ((EFUSE_PWR_OFF_NUM_V)<<(EFUSE_PWR_OFF_NUM_S))
#define EFUSE_PWR_OFF_NUM_V  0xFFFF
#define EFUSE_PWR_OFF_NUM_S  0

#define EFUSE_DATE_REG          (DR_REG_EFUSE_BASE + 0x1FC)
/* EFUSE_DATE : R/W ;bitpos:[27:0] ;default: 28'h2108190 ; */
/*description: Stores eFuse version..*/
#define EFUSE_DATE    0x0FFFFFFF
#define EFUSE_DATE_M  ((EFUSE_DATE_V)<<(EFUSE_DATE_S))
#define EFUSE_DATE_V  0xFFFFFFF
#define EFUSE_DATE_S  0


#ifdef __cplusplus
}
#endif



#endif /*_SOC_EFUSE_REG_H_ */
