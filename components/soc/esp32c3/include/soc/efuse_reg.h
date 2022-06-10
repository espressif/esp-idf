/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_EFUSE_REG_H_
#define _SOC_EFUSE_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"
#define EFUSE_PGM_DATA0_REG          (DR_REG_EFUSE_BASE + 0x000)
/* EFUSE_WR_DIS : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Disable programming of individual eFuses.*/
#define EFUSE_WR_DIS  0xFFFFFFFF
#define EFUSE_WR_DIS_M  ((EFUSE_WR_DIS_V)<<(EFUSE_WR_DIS_S))
#define EFUSE_WR_DIS_V  0xFFFFFFFF
#define EFUSE_WR_DIS_S  0

#define EFUSE_PGM_DATA1_REG          (DR_REG_EFUSE_BASE + 0x004)
/* EFUSE_POWER_GLITCH_DSENSE : R/W ;bitpos:[31:30] ;default: 2'h0 ; */
/*description: Sample delay configuration of power glitch.*/
#define EFUSE_POWER_GLITCH_DSENSE  0x00000003
#define EFUSE_POWER_GLITCH_DSENSE_M  ((EFUSE_POWER_GLITCH_DSENSE_V)<<(EFUSE_POWER_GLITCH_DSENSE_S))
#define EFUSE_POWER_GLITCH_DSENSE_V  0x3
#define EFUSE_POWER_GLITCH_DSENSE_S  30
/* EFUSE_POWERGLITCH_EN : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: Set this bit to enable power glitch function.*/
#define EFUSE_POWERGLITCH_EN  (BIT(29))
#define EFUSE_POWERGLITCH_EN_M  (BIT(29))
#define EFUSE_POWERGLITCH_EN_V  0x1
#define EFUSE_POWERGLITCH_EN_S  29
/* EFUSE_BTLC_GPIO_ENABLE : R/W ;bitpos:[28:27] ;default: 2'h0 ; */
/*description: Enable btlc gpio.*/
#define EFUSE_BTLC_GPIO_ENABLE  0x00000003
#define EFUSE_BTLC_GPIO_ENABLE_M  ((EFUSE_BTLC_GPIO_ENABLE_V)<<(EFUSE_BTLC_GPIO_ENABLE_S))
#define EFUSE_BTLC_GPIO_ENABLE_V  0x3
#define EFUSE_BTLC_GPIO_ENABLE_S  27
/* EFUSE_VDD_SPI_AS_GPIO : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: Set this bit to vdd spi pin function as gpio.*/
#define EFUSE_VDD_SPI_AS_GPIO  (BIT(26))
#define EFUSE_VDD_SPI_AS_GPIO_M  (BIT(26))
#define EFUSE_VDD_SPI_AS_GPIO_V  0x1
#define EFUSE_VDD_SPI_AS_GPIO_S  26
/* EFUSE_USB_EXCHG_PINS : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: Set this bit to exchange USB D+ and D- pins.*/
#define EFUSE_USB_EXCHG_PINS  (BIT(25))
#define EFUSE_USB_EXCHG_PINS_M  (BIT(25))
#define EFUSE_USB_EXCHG_PINS_V  0x1
#define EFUSE_USB_EXCHG_PINS_S  25
/* EFUSE_USB_DREFL : R/W ;bitpos:[24:23] ;default: 2'h0 ; */
/*description: Controls single-end input threshold vrefl  0.8 V to 1.04 V with
 step of 80 mV  stored in eFuse.*/
#define EFUSE_USB_DREFL  0x00000003
#define EFUSE_USB_DREFL_M  ((EFUSE_USB_DREFL_V)<<(EFUSE_USB_DREFL_S))
#define EFUSE_USB_DREFL_V  0x3
#define EFUSE_USB_DREFL_S  23
/* EFUSE_USB_DREFH : R/W ;bitpos:[22:21] ;default: 2'h0 ; */
/*description: Controls single-end input threshold vrefh  1.76 V to 2 V with
 step of 80 mV  stored in eFuse.*/
#define EFUSE_USB_DREFH  0x00000003
#define EFUSE_USB_DREFH_M  ((EFUSE_USB_DREFH_V)<<(EFUSE_USB_DREFH_S))
#define EFUSE_USB_DREFH_V  0x3
#define EFUSE_USB_DREFH_S  21
/* EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: Set this bit to disable flash encryption when in download boot modes.*/
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT  (BIT(20))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_M  (BIT(20))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_V  0x1
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_S  20
/* EFUSE_DIS_PAD_JTAG : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: Set this bit to disable JTAG in the hard way. JTAG is disabled permanently.*/
#define EFUSE_DIS_PAD_JTAG  (BIT(19))
#define EFUSE_DIS_PAD_JTAG_M  (BIT(19))
#define EFUSE_DIS_PAD_JTAG_V  0x1
#define EFUSE_DIS_PAD_JTAG_S  19
/* EFUSE_SOFT_DIS_JTAG : R/W ;bitpos:[18:16] ;default: 3'h0 ; */
/*description: Set these bits to disable JTAG in the soft way (odd number 1
 means disable ). JTAG can be enabled in HMAC module.*/
#define EFUSE_SOFT_DIS_JTAG  0x00000007
#define EFUSE_SOFT_DIS_JTAG_M  ((EFUSE_SOFT_DIS_JTAG_V)<<(EFUSE_SOFT_DIS_JTAG_S))
#define EFUSE_SOFT_DIS_JTAG_V  0x7
#define EFUSE_SOFT_DIS_JTAG_S  16
/* EFUSE_JTAG_SEL_ENABLE : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: Set this bit to enable selection between usb_to_jtag and pad_to_jtag
 through strapping gpio10 when both reg_dis_usb_jtag and reg_dis_pad_jtag are equal to 0.*/
#define EFUSE_JTAG_SEL_ENABLE  (BIT(15))
#define EFUSE_JTAG_SEL_ENABLE_M  (BIT(15))
#define EFUSE_JTAG_SEL_ENABLE_V  0x1
#define EFUSE_JTAG_SEL_ENABLE_S  15
/* EFUSE_DIS_TWAI : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: Set this bit to disable TWAI function.*/
#define EFUSE_DIS_TWAI  (BIT(14))
#define EFUSE_DIS_TWAI_M  (BIT(14))
#define EFUSE_DIS_TWAI_V  0x1
#define EFUSE_DIS_TWAI_S  14
/* EFUSE_RPT4_RESERVED6_ERR : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: Reserved..*/
#define EFUSE_RPT4_RESERVED6    (BIT(13))
#define EFUSE_RPT4_RESERVED6_M  (BIT(13))
#define EFUSE_RPT4_RESERVED6_V  0x1
#define EFUSE_RPT4_RESERVED6_S  13
/* EFUSE_DIS_FORCE_DOWNLOAD : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to disable the function that forces chip into download mode.*/
#define EFUSE_DIS_FORCE_DOWNLOAD  (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_M  (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_V  0x1
#define EFUSE_DIS_FORCE_DOWNLOAD_S  12
/* EFUSE_DIS_USB_DEVICE : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: Set this bit to disable usb device.*/
#define EFUSE_DIS_USB_DEVICE  (BIT(11))
#define EFUSE_DIS_USB_DEVICE_M  (BIT(11))
#define EFUSE_DIS_USB_DEVICE_V  0x1
#define EFUSE_DIS_USB_DEVICE_S  11
/* EFUSE_DIS_DOWNLOAD_ICACHE : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this bit to disable Icache in download mode (boot_mode[3:0]
 is 0  1  2  3  6  7).*/
#define EFUSE_DIS_DOWNLOAD_ICACHE  (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_M  (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_V  0x1
#define EFUSE_DIS_DOWNLOAD_ICACHE_S  10
/* EFUSE_DIS_USB_JTAG : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to disable function of usb switch to jtag in module of usb device.*/
#define EFUSE_DIS_USB_JTAG  (BIT(9))
#define EFUSE_DIS_USB_JTAG_M  (BIT(9))
#define EFUSE_DIS_USB_JTAG_V  0x1
#define EFUSE_DIS_USB_JTAG_S  9
/* EFUSE_DIS_ICACHE : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to disable Icache.*/
#define EFUSE_DIS_ICACHE  (BIT(8))
#define EFUSE_DIS_ICACHE_M  (BIT(8))
#define EFUSE_DIS_ICACHE_V  0x1
#define EFUSE_DIS_ICACHE_S  8
/* EFUSE_RPT4_RESERVED5 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED5  (BIT(7))
#define EFUSE_RPT4_RESERVED5_M  (BIT(7))
#define EFUSE_RPT4_RESERVED5_V  0x1
#define EFUSE_RPT4_RESERVED5_S  7
/* EFUSE_RD_DIS : R/W ;bitpos:[6:0] ;default: 7'h0 ; */
/*description: Set this bit to disable reading from BlOCK4-10.*/
#define EFUSE_RD_DIS  0x0000007F
#define EFUSE_RD_DIS_M  ((EFUSE_RD_DIS_V)<<(EFUSE_RD_DIS_S))
#define EFUSE_RD_DIS_V  0x7F
#define EFUSE_RD_DIS_S  0

#define EFUSE_PGM_DATA2_REG          (DR_REG_EFUSE_BASE + 0x008)
/* EFUSE_KEY_PURPOSE_1 : R/W ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: Purpose of Key1.*/
#define EFUSE_KEY_PURPOSE_1  0x0000000F
#define EFUSE_KEY_PURPOSE_1_M  ((EFUSE_KEY_PURPOSE_1_V)<<(EFUSE_KEY_PURPOSE_1_S))
#define EFUSE_KEY_PURPOSE_1_V  0xF
#define EFUSE_KEY_PURPOSE_1_S  28
/* EFUSE_KEY_PURPOSE_0 : R/W ;bitpos:[27:24] ;default: 4'h0 ; */
/*description: Purpose of Key0.*/
#define EFUSE_KEY_PURPOSE_0  0x0000000F
#define EFUSE_KEY_PURPOSE_0_M  ((EFUSE_KEY_PURPOSE_0_V)<<(EFUSE_KEY_PURPOSE_0_S))
#define EFUSE_KEY_PURPOSE_0_V  0xF
#define EFUSE_KEY_PURPOSE_0_S  24
/* EFUSE_SECURE_BOOT_KEY_REVOKE2 : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: Set this bit to enable revoking third secure boot key.*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE2  (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_M  (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_S  23
/* EFUSE_SECURE_BOOT_KEY_REVOKE1 : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to enable revoking second secure boot key.*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE1  (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_M  (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_S  22
/* EFUSE_SECURE_BOOT_KEY_REVOKE0 : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to enable revoking first secure boot key.*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE0  (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_M  (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_S  21
/* EFUSE_SPI_BOOT_CRYPT_CNT : R/W ;bitpos:[20:18] ;default: 3'h0 ; */
/*description: Set this bit to enable SPI boot encrypt/decrypt. Odd number of
 1: enable. even number of 1: disable.*/
#define EFUSE_SPI_BOOT_CRYPT_CNT  0x00000007
#define EFUSE_SPI_BOOT_CRYPT_CNT_M  ((EFUSE_SPI_BOOT_CRYPT_CNT_V)<<(EFUSE_SPI_BOOT_CRYPT_CNT_S))
#define EFUSE_SPI_BOOT_CRYPT_CNT_V  0x7
#define EFUSE_SPI_BOOT_CRYPT_CNT_S  18
/* EFUSE_WAT_DELAY_SEL : R/W ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: Selects RTC watchdog timeout threshold  in unit of slow clock
 cycle. 0: 40000. 1: 80000. 2: 160000. 3:320000.*/
#define EFUSE_WAT_DELAY_SEL  0x00000003
#define EFUSE_WAT_DELAY_SEL_M  ((EFUSE_WAT_DELAY_SEL_V)<<(EFUSE_WAT_DELAY_SEL_S))
#define EFUSE_WAT_DELAY_SEL_V  0x3
#define EFUSE_WAT_DELAY_SEL_S  16
/* EFUSE_RPT4_RESERVED2 : RO ;bitpos:[15:0] ;default: 2'h0 ; */
/*description: Reserved (used for four backups method).*/
#define EFUSE_RPT4_RESERVED2  0x0000FFFF
#define EFUSE_RPT4_RESERVED2_M  ((EFUSE_RPT4_RESERVED2_V)<<(EFUSE_RPT4_RESERVED2_S))
#define EFUSE_RPT4_RESERVED2_V  0xFFFF
#define EFUSE_RPT4_RESERVED2_S  0

#define EFUSE_PGM_DATA3_REG          (DR_REG_EFUSE_BASE + 0x00C)
/* EFUSE_FLASH_TPUW : R/W ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: Configures flash waiting time after power-up  in unit of ms.
 If the value is less than 15  the waiting time is the configurable value*/
#define EFUSE_FLASH_TPUW  0x0000000F
#define EFUSE_FLASH_TPUW_M  ((EFUSE_FLASH_TPUW_V)<<(EFUSE_FLASH_TPUW_S))
#define EFUSE_FLASH_TPUW_V  0xF
#define EFUSE_FLASH_TPUW_S  28
/* EFUSE_RPT4_RESERVED0 : RO ;bitpos:[27:22] ;default: 6'h0 ; */
/*description: Reserved (used for four backups method).*/
#define EFUSE_RPT4_RESERVED0  0x0000003F
#define EFUSE_RPT4_RESERVED0_M  ((EFUSE_RPT4_RESERVED0_V)<<(EFUSE_RPT4_RESERVED0_S))
#define EFUSE_RPT4_RESERVED0_V  0x3F
#define EFUSE_RPT4_RESERVED0_S  22
/* EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to enable revoking aggressive secure boot.*/
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE  (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_M  (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_V  0x1
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_S  21
/* EFUSE_SECURE_BOOT_EN : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: Set this bit to enable secure boot.*/
#define EFUSE_SECURE_BOOT_EN  (BIT(20))
#define EFUSE_SECURE_BOOT_EN_M  (BIT(20))
#define EFUSE_SECURE_BOOT_EN_V  0x1
#define EFUSE_SECURE_BOOT_EN_S  20
/* EFUSE_RPT4_RESERVED3 : RO ;bitpos:[19:16] ;default: 4'h0 ; */
/*description: Reserved (used for four backups method).*/
#define EFUSE_RPT4_RESERVED3  0x0000000F
#define EFUSE_RPT4_RESERVED3_M  ((EFUSE_RPT4_RESERVED3_V)<<(EFUSE_RPT4_RESERVED3_S))
#define EFUSE_RPT4_RESERVED3_V  0xF
#define EFUSE_RPT4_RESERVED3_S  16
/* EFUSE_KEY_PURPOSE_5 : R/W ;bitpos:[15:12] ;default: 4'h0 ; */
/*description: Purpose of Key5.*/
#define EFUSE_KEY_PURPOSE_5  0x0000000F
#define EFUSE_KEY_PURPOSE_5_M  ((EFUSE_KEY_PURPOSE_5_V)<<(EFUSE_KEY_PURPOSE_5_S))
#define EFUSE_KEY_PURPOSE_5_V  0xF
#define EFUSE_KEY_PURPOSE_5_S  12
/* EFUSE_KEY_PURPOSE_4 : R/W ;bitpos:[11:8] ;default: 4'h0 ; */
/*description: Purpose of Key4.*/
#define EFUSE_KEY_PURPOSE_4  0x0000000F
#define EFUSE_KEY_PURPOSE_4_M  ((EFUSE_KEY_PURPOSE_4_V)<<(EFUSE_KEY_PURPOSE_4_S))
#define EFUSE_KEY_PURPOSE_4_V  0xF
#define EFUSE_KEY_PURPOSE_4_S  8
/* EFUSE_KEY_PURPOSE_3 : R/W ;bitpos:[7:4] ;default: 4'h0 ; */
/*description: Purpose of Key3.*/
#define EFUSE_KEY_PURPOSE_3  0x0000000F
#define EFUSE_KEY_PURPOSE_3_M  ((EFUSE_KEY_PURPOSE_3_V)<<(EFUSE_KEY_PURPOSE_3_S))
#define EFUSE_KEY_PURPOSE_3_V  0xF
#define EFUSE_KEY_PURPOSE_3_S  4
/* EFUSE_KEY_PURPOSE_2 : R/W ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: Purpose of Key2.*/
#define EFUSE_KEY_PURPOSE_2  0x0000000F
#define EFUSE_KEY_PURPOSE_2_M  ((EFUSE_KEY_PURPOSE_2_V)<<(EFUSE_KEY_PURPOSE_2_S))
#define EFUSE_KEY_PURPOSE_2_V  0xF
#define EFUSE_KEY_PURPOSE_2_S  0

#define EFUSE_PGM_DATA4_REG          (DR_REG_EFUSE_BASE + 0x010)
/* EFUSE_ERR_RST_ENABLE : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: Use BLOCK0 to check error record registers, 0 - without check.*/
#define EFUSE_ERR_RST_ENABLE  (BIT(31))
#define EFUSE_ERR_RST_ENABLE_M  (BIT(31))
#define EFUSE_ERR_RST_ENABLE_V  0x1
#define EFUSE_ERR_RST_ENABLE_S  31
/* EFUSE_RPT4_RESERVED1 : RO ;bitpos:[30] ;default: 1'h0 ; */
/*description: Reserved (used for four backups method).*/
#define EFUSE_RPT4_RESERVED1  (BIT(30))
#define EFUSE_RPT4_RESERVED1_M  (BIT(30))
#define EFUSE_RPT4_RESERVED1_V  0x1
#define EFUSE_RPT4_RESERVED1_S  30
/* EFUSE_SECURE_VERSION : R/W ;bitpos:[29:14] ;default: 16'h0 ; */
/*description: Secure version (used by ESP-IDF anti-rollback feature).*/
#define EFUSE_SECURE_VERSION  0x0000FFFF
#define EFUSE_SECURE_VERSION_M  ((EFUSE_SECURE_VERSION_V)<<(EFUSE_SECURE_VERSION_S))
#define EFUSE_SECURE_VERSION_V  0xFFFF
#define EFUSE_SECURE_VERSION_S  14
/* EFUSE_FORCE_SEND_RESUME : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: Set this bit to force ROM code to send a resume command during SPI boot.*/
#define EFUSE_FORCE_SEND_RESUME  (BIT(13))
#define EFUSE_FORCE_SEND_RESUME_M  (BIT(13))
#define EFUSE_FORCE_SEND_RESUME_V  0x1
#define EFUSE_FORCE_SEND_RESUME_S  13
/* EFUSE_RPT4_RESERVED7 : R/W ;bitpos:[12:8] ;default: 5'h0 ; */
/*description: Reserved (used for four backups method).*/
#define EFUSE_RPT4_RESERVED7  0x0000001F
#define EFUSE_RPT4_RESERVED7_M  ((EFUSE_RPT4_RESERVED7_V)<<(EFUSE_RPT4_RESERVED7_S))
#define EFUSE_RPT4_RESERVED7_V  0x1F
#define EFUSE_RPT4_RESERVED7_S  8
/* EFUSE_UART_PRINT_CONTROL : R/W ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: Set the default UARTboot message output mode. 00: Enabled. 01:
 Enabled when GPIO8 is low at reset. 10: Enabled when GPIO8 is high at reset. 11:disabled.*/
#define EFUSE_UART_PRINT_CONTROL  0x00000003
#define EFUSE_UART_PRINT_CONTROL_M  ((EFUSE_UART_PRINT_CONTROL_V)<<(EFUSE_UART_PRINT_CONTROL_S))
#define EFUSE_UART_PRINT_CONTROL_V  0x3
#define EFUSE_UART_PRINT_CONTROL_S  6
/* EFUSE_ENABLE_SECURITY_DOWNLOAD : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to enable secure UART download mode.*/
#define EFUSE_ENABLE_SECURITY_DOWNLOAD  (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_M  (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_V  0x1
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_S  5
/* EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to disable download through USB-Serial-JTAG.*/
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE  (BIT(4))
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_M  (BIT(4))
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_V  0x1
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_S  4
/* EFUSE_RPT4_RESERVED8 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Reserved (used for four backups method).*/
#define EFUSE_RPT4_RESERVED8  (BIT(3))
#define EFUSE_RPT4_RESERVED8_M  (BIT(3))
#define EFUSE_RPT4_RESERVED8_V  0x1
#define EFUSE_RPT4_RESERVED8_S  3
/* EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Disable USB-Serial-JTAG print during rom boot.*/
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT  (BIT(2))
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_M  (BIT(2))
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_V  0x1
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_S  2
/* EFUSE_DIS_DIRECT_BOOT : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to disable direct boot*/
#define EFUSE_DIS_DIRECT_BOOT  (BIT(1))
#define EFUSE_DIS_DIRECT_BOOT_M  (BIT(1))
#define EFUSE_DIS_DIRECT_BOOT_V  0x1
#define EFUSE_DIS_DIRECT_BOOT_S  1
/* EFUSE_DIS_DOWNLOAD_MODE : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to disable download mode (boot_mode[3:0] = 0  1  2  3  6  7).*/
#define EFUSE_DIS_DOWNLOAD_MODE  (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_M  (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_V  0x1
#define EFUSE_DIS_DOWNLOAD_MODE_S  0

#define EFUSE_PGM_DATA5_REG          (DR_REG_EFUSE_BASE + 0x014)
/* EFUSE_RPT4_RESERVED4 : RO ;bitpos:[23:0] ;default: 24'h0 ; */
/*description: Reserved (used for four backups method).*/
#define EFUSE_RPT4_RESERVED4  0x00FFFFFF
#define EFUSE_RPT4_RESERVED4_M  ((EFUSE_RPT4_RESERVED4_V)<<(EFUSE_RPT4_RESERVED4_S))
#define EFUSE_RPT4_RESERVED4_V  0xFFFFFF
#define EFUSE_RPT4_RESERVED4_S  0

#define EFUSE_PGM_DATA6_REG          (DR_REG_EFUSE_BASE + 0x018)
/* EFUSE_PGM_DATA_6 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the sixth 32-bit data to be programmed.*/
#define EFUSE_PGM_DATA_6  0xFFFFFFFF
#define EFUSE_PGM_DATA_6_M  ((EFUSE_PGM_DATA_6_V)<<(EFUSE_PGM_DATA_6_S))
#define EFUSE_PGM_DATA_6_V  0xFFFFFFFF
#define EFUSE_PGM_DATA_6_S  0

#define EFUSE_PGM_DATA7_REG          (DR_REG_EFUSE_BASE + 0x01C)
/* EFUSE_PGM_DATA_7 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the seventh 32-bit data to be programmed.*/
#define EFUSE_PGM_DATA_7  0xFFFFFFFF
#define EFUSE_PGM_DATA_7_M  ((EFUSE_PGM_DATA_7_V)<<(EFUSE_PGM_DATA_7_S))
#define EFUSE_PGM_DATA_7_V  0xFFFFFFFF
#define EFUSE_PGM_DATA_7_S  0

#define EFUSE_PGM_CHECK_VALUE0_REG          (DR_REG_EFUSE_BASE + 0x020)
/* EFUSE_PGM_RS_DATA_0 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 0th 32-bit RS code to be programmed.*/
#define EFUSE_PGM_RS_DATA_0  0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_0_M  ((EFUSE_PGM_RS_DATA_0_V)<<(EFUSE_PGM_RS_DATA_0_S))
#define EFUSE_PGM_RS_DATA_0_V  0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_0_S  0

#define EFUSE_PGM_CHECK_VALUE1_REG          (DR_REG_EFUSE_BASE + 0x024)
/* EFUSE_PGM_RS_DATA_1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the first 32-bit RS code to be programmed.*/
#define EFUSE_PGM_RS_DATA_1  0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_1_M  ((EFUSE_PGM_RS_DATA_1_V)<<(EFUSE_PGM_RS_DATA_1_S))
#define EFUSE_PGM_RS_DATA_1_V  0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_1_S  0

#define EFUSE_PGM_CHECK_VALUE2_REG          (DR_REG_EFUSE_BASE + 0x028)
/* EFUSE_PGM_RS_DATA_2 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the second 32-bit RS code to be programmed.*/
#define EFUSE_PGM_RS_DATA_2  0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_2_M  ((EFUSE_PGM_RS_DATA_2_V)<<(EFUSE_PGM_RS_DATA_2_S))
#define EFUSE_PGM_RS_DATA_2_V  0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_2_S  0

#define EFUSE_RD_WR_DIS_REG          (DR_REG_EFUSE_BASE + 0x02C)
/* EFUSE_WR_DIS : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The value of WR_DIS.*/
#define EFUSE_WR_DIS  0xFFFFFFFF
#define EFUSE_WR_DIS_M  ((EFUSE_WR_DIS_V)<<(EFUSE_WR_DIS_S))
#define EFUSE_WR_DIS_V  0xFFFFFFFF
#define EFUSE_WR_DIS_S  0

#define EFUSE_RD_REPEAT_DATA0_REG          (DR_REG_EFUSE_BASE + 0x030)
/* EFUSE_POWER_GLITCH_DSENSE : RO ;bitpos:[31:30] ;default: 2'h0 ; */
/*description: The value of POWER_GLITCH_DSENSE.*/
#define EFUSE_POWER_GLITCH_DSENSE  0x00000003
#define EFUSE_POWER_GLITCH_DSENSE_M  ((EFUSE_POWER_GLITCH_DSENSE_V)<<(EFUSE_POWER_GLITCH_DSENSE_S))
#define EFUSE_POWER_GLITCH_DSENSE_V  0x3
#define EFUSE_POWER_GLITCH_DSENSE_S  30
/* EFUSE_POWERGLITCH_EN : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: The value of POWERGLITCH_EN.*/
#define EFUSE_POWERGLITCH_EN  (BIT(29))
#define EFUSE_POWERGLITCH_EN_M  (BIT(29))
#define EFUSE_POWERGLITCH_EN_V  0x1
#define EFUSE_POWERGLITCH_EN_S  29
/* EFUSE_BTLC_GPIO_ENABLE : RO ;bitpos:[28:27] ;default: 2'h0 ; */
/*description: The value of BTLC_GPIO_ENABLE.*/
#define EFUSE_BTLC_GPIO_ENABLE  0x00000003
#define EFUSE_BTLC_GPIO_ENABLE_M  ((EFUSE_BTLC_GPIO_ENABLE_V)<<(EFUSE_BTLC_GPIO_ENABLE_S))
#define EFUSE_BTLC_GPIO_ENABLE_V  0x3
#define EFUSE_BTLC_GPIO_ENABLE_S  27
/* EFUSE_VDD_SPI_AS_GPIO : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: The value of VDD_SPI_AS_GPIO.*/
#define EFUSE_VDD_SPI_AS_GPIO  (BIT(26))
#define EFUSE_VDD_SPI_AS_GPIO_M  (BIT(26))
#define EFUSE_VDD_SPI_AS_GPIO_V  0x1
#define EFUSE_VDD_SPI_AS_GPIO_S  26
/* EFUSE_USB_EXCHG_PINS : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: The value of USB_EXCHG_PINS.*/
#define EFUSE_USB_EXCHG_PINS  (BIT(25))
#define EFUSE_USB_EXCHG_PINS_M  (BIT(25))
#define EFUSE_USB_EXCHG_PINS_V  0x1
#define EFUSE_USB_EXCHG_PINS_S  25
/* EFUSE_USB_DREFL : RO ;bitpos:[24:23] ;default: 2'h0 ; */
/*description: The value of USB_DREFL.*/
#define EFUSE_USB_DREFL  0x00000003
#define EFUSE_USB_DREFL_M  ((EFUSE_USB_DREFL_V)<<(EFUSE_USB_DREFL_S))
#define EFUSE_USB_DREFL_V  0x3
#define EFUSE_USB_DREFL_S  23
/* EFUSE_USB_DREFH : RO ;bitpos:[22:21] ;default: 2'h0 ; */
/*description: The value of USB_DREFH.*/
#define EFUSE_USB_DREFH  0x00000003
#define EFUSE_USB_DREFH_M  ((EFUSE_USB_DREFH_V)<<(EFUSE_USB_DREFH_S))
#define EFUSE_USB_DREFH_V  0x3
#define EFUSE_USB_DREFH_S  21
/* EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: The value of DIS_DOWNLOAD_MANUAL_ENCRYPT.*/
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT  (BIT(20))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_M  (BIT(20))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_V  0x1
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_S  20
/* EFUSE_DIS_PAD_JTAG : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: The value of DIS_PAD_JTAG.*/
#define EFUSE_DIS_PAD_JTAG  (BIT(19))
#define EFUSE_DIS_PAD_JTAG_M  (BIT(19))
#define EFUSE_DIS_PAD_JTAG_V  0x1
#define EFUSE_DIS_PAD_JTAG_S  19
/* EFUSE_SOFT_DIS_JTAG : RO ;bitpos:[18:16] ;default: 3'h0 ; */
/*description: The value of SOFT_DIS_JTAG.*/
#define EFUSE_SOFT_DIS_JTAG  0x00000007
#define EFUSE_SOFT_DIS_JTAG_M  ((EFUSE_SOFT_DIS_JTAG_V)<<(EFUSE_SOFT_DIS_JTAG_S))
#define EFUSE_SOFT_DIS_JTAG_V  0x7
#define EFUSE_SOFT_DIS_JTAG_S  16
/* EFUSE_JTAG_SEL_ENABLE : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: The value of JTAG_SEL_ENABLE.*/
#define EFUSE_JTAG_SEL_ENABLE  (BIT(15))
#define EFUSE_JTAG_SEL_ENABLE_M  (BIT(15))
#define EFUSE_JTAG_SEL_ENABLE_V  0x1
#define EFUSE_JTAG_SEL_ENABLE_S  15
/* EFUSE_DIS_TWAI : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: The value of DIS_TWAI.*/
#define EFUSE_DIS_TWAI  (BIT(14))
#define EFUSE_DIS_TWAI_M  (BIT(14))
#define EFUSE_DIS_TWAI_V  0x1
#define EFUSE_DIS_TWAI_S  14
/* EFUSE_RPT4_RESERVED6 : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: Reserved (used for four backups method)..*/
#define EFUSE_RPT4_RESERVED6    (BIT(13))
#define EFUSE_RPT4_RESERVED6_M  (BIT(13))
#define EFUSE_RPT4_RESERVED6_V  0x1
#define EFUSE_RPT4_RESERVED6_S  13
/* EFUSE_DIS_FORCE_DOWNLOAD : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: The value of DIS_FORCE_DOWNLOAD.*/
#define EFUSE_DIS_FORCE_DOWNLOAD  (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_M  (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_V  0x1
#define EFUSE_DIS_FORCE_DOWNLOAD_S  12
/* EFUSE_DIS_USB_DEVICE : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: The value of DIS_USB_DEVICE.*/
#define EFUSE_DIS_USB_DEVICE  (BIT(11))
#define EFUSE_DIS_USB_DEVICE_M  (BIT(11))
#define EFUSE_DIS_USB_DEVICE_V  0x1
#define EFUSE_DIS_USB_DEVICE_S  11
/* EFUSE_DIS_DOWNLOAD_ICACHE : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: The value of DIS_DOWNLOAD_ICACHE.*/
#define EFUSE_DIS_DOWNLOAD_ICACHE  (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_M  (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_V  0x1
#define EFUSE_DIS_DOWNLOAD_ICACHE_S  10
/* EFUSE_DIS_USB_JTAG : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The value of DIS_USB_JTAG.*/
#define EFUSE_DIS_USB_JTAG  (BIT(9))
#define EFUSE_DIS_USB_JTAG_M  (BIT(9))
#define EFUSE_DIS_USB_JTAG_V  0x1
#define EFUSE_DIS_USB_JTAG_S  9
/* EFUSE_DIS_ICACHE : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The value of DIS_ICACHE.*/
#define EFUSE_DIS_ICACHE  (BIT(8))
#define EFUSE_DIS_ICACHE_M  (BIT(8))
#define EFUSE_DIS_ICACHE_V  0x1
#define EFUSE_DIS_ICACHE_S  8
/* EFUSE_RPT4_RESERVED5 : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED5  (BIT(7))
#define EFUSE_RPT4_RESERVED5_M  (BIT(7))
#define EFUSE_RPT4_RESERVED5_V  0x1
#define EFUSE_RPT4_RESERVED5_S  7
/* EFUSE_RD_DIS : RO ;bitpos:[6:0] ;default: 7'h0 ; */
/*description: The value of RD_DIS.*/
#define EFUSE_RD_DIS  0x0000007F
#define EFUSE_RD_DIS_M  ((EFUSE_RD_DIS_V)<<(EFUSE_RD_DIS_S))
#define EFUSE_RD_DIS_V  0x7F
#define EFUSE_RD_DIS_S  0

#define EFUSE_RD_REPEAT_DATA1_REG          (DR_REG_EFUSE_BASE + 0x034)
/* EFUSE_KEY_PURPOSE_1 : RO ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: The value of KEY_PURPOSE_1.*/
#define EFUSE_KEY_PURPOSE_1  0x0000000F
#define EFUSE_KEY_PURPOSE_1_M  ((EFUSE_KEY_PURPOSE_1_V)<<(EFUSE_KEY_PURPOSE_1_S))
#define EFUSE_KEY_PURPOSE_1_V  0xF
#define EFUSE_KEY_PURPOSE_1_S  28
/* EFUSE_KEY_PURPOSE_0 : RO ;bitpos:[27:24] ;default: 4'h0 ; */
/*description: The value of KEY_PURPOSE_0.*/
#define EFUSE_KEY_PURPOSE_0  0x0000000F
#define EFUSE_KEY_PURPOSE_0_M  ((EFUSE_KEY_PURPOSE_0_V)<<(EFUSE_KEY_PURPOSE_0_S))
#define EFUSE_KEY_PURPOSE_0_V  0xF
#define EFUSE_KEY_PURPOSE_0_S  24
/* EFUSE_SECURE_BOOT_KEY_REVOKE2 : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: The value of SECURE_BOOT_KEY_REVOKE2.*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE2  (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_M  (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_S  23
/* EFUSE_SECURE_BOOT_KEY_REVOKE1 : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: The value of SECURE_BOOT_KEY_REVOKE1.*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE1  (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_M  (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_S  22
/* EFUSE_SECURE_BOOT_KEY_REVOKE0 : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: The value of SECURE_BOOT_KEY_REVOKE0.*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE0  (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_M  (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_S  21
/* EFUSE_SPI_BOOT_CRYPT_CNT : RO ;bitpos:[20:18] ;default: 3'h0 ; */
/*description: The value of SPI_BOOT_CRYPT_CNT.*/
#define EFUSE_SPI_BOOT_CRYPT_CNT  0x00000007
#define EFUSE_SPI_BOOT_CRYPT_CNT_M  ((EFUSE_SPI_BOOT_CRYPT_CNT_V)<<(EFUSE_SPI_BOOT_CRYPT_CNT_S))
#define EFUSE_SPI_BOOT_CRYPT_CNT_V  0x7
#define EFUSE_SPI_BOOT_CRYPT_CNT_S  18
/* EFUSE_WDT_DELAY_SEL : RO ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: The value of WDT_DELAY_SEL.*/
#define EFUSE_WDT_DELAY_SEL  0x00000003
#define EFUSE_WDT_DELAY_SEL_M  ((EFUSE_WDT_DELAY_SEL_V)<<(EFUSE_WDT_DELAY_SEL_S))
#define EFUSE_WDT_DELAY_SEL_V  0x3
#define EFUSE_WDT_DELAY_SEL_S  16
/* EFUSE_RPT4_RESERVED2 : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED2  0x0000FFFF
#define EFUSE_RPT4_RESERVED2_M  ((EFUSE_RPT4_RESERVED2_V)<<(EFUSE_RPT4_RESERVED2_S))
#define EFUSE_RPT4_RESERVED2_V  0xFFFF
#define EFUSE_RPT4_RESERVED2_S  0

#define EFUSE_RD_REPEAT_DATA2_REG          (DR_REG_EFUSE_BASE + 0x038)
/* EFUSE_FLASH_TPUW : RO ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: The value of FLASH_TPUW.*/
#define EFUSE_FLASH_TPUW  0x0000000F
#define EFUSE_FLASH_TPUW_M  ((EFUSE_FLASH_TPUW_V)<<(EFUSE_FLASH_TPUW_S))
#define EFUSE_FLASH_TPUW_V  0xF
#define EFUSE_FLASH_TPUW_S  28
/* EFUSE_RPT4_RESERVED0 : RO ;bitpos:[27:22] ;default: 6'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED0  0x0000003F
#define EFUSE_RPT4_RESERVED0_M  ((EFUSE_RPT4_RESERVED0_V)<<(EFUSE_RPT4_RESERVED0_S))
#define EFUSE_RPT4_RESERVED0_V  0x3F
#define EFUSE_RPT4_RESERVED0_S  22
/* EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: The value of SECURE_BOOT_AGGRESSIVE_REVOKE.*/
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE  (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_M  (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_V  0x1
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_S  21
/* EFUSE_SECURE_BOOT_EN : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: The value of SECURE_BOOT_EN.*/
#define EFUSE_SECURE_BOOT_EN  (BIT(20))
#define EFUSE_SECURE_BOOT_EN_M  (BIT(20))
#define EFUSE_SECURE_BOOT_EN_V  0x1
#define EFUSE_SECURE_BOOT_EN_S  20
/* EFUSE_RPT4_RESERVED3 : RO ;bitpos:[19:16] ;default: 4'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED3  0x0000000F
#define EFUSE_RPT4_RESERVED3_M  ((EFUSE_RPT4_RESERVED3_V)<<(EFUSE_RPT4_RESERVED3_S))
#define EFUSE_RPT4_RESERVED3_V  0xF
#define EFUSE_RPT4_RESERVED3_S  16
/* EFUSE_KEY_PURPOSE_5 : RO ;bitpos:[15:12] ;default: 4'h0 ; */
/*description: The value of KEY_PURPOSE_5.*/
#define EFUSE_KEY_PURPOSE_5  0x0000000F
#define EFUSE_KEY_PURPOSE_5_M  ((EFUSE_KEY_PURPOSE_5_V)<<(EFUSE_KEY_PURPOSE_5_S))
#define EFUSE_KEY_PURPOSE_5_V  0xF
#define EFUSE_KEY_PURPOSE_5_S  12
/* EFUSE_KEY_PURPOSE_4 : RO ;bitpos:[11:8] ;default: 4'h0 ; */
/*description: The value of KEY_PURPOSE_4.*/
#define EFUSE_KEY_PURPOSE_4  0x0000000F
#define EFUSE_KEY_PURPOSE_4_M  ((EFUSE_KEY_PURPOSE_4_V)<<(EFUSE_KEY_PURPOSE_4_S))
#define EFUSE_KEY_PURPOSE_4_V  0xF
#define EFUSE_KEY_PURPOSE_4_S  8
/* EFUSE_KEY_PURPOSE_3 : RO ;bitpos:[7:4] ;default: 4'h0 ; */
/*description: The value of KEY_PURPOSE_3.*/
#define EFUSE_KEY_PURPOSE_3  0x0000000F
#define EFUSE_KEY_PURPOSE_3_M  ((EFUSE_KEY_PURPOSE_3_V)<<(EFUSE_KEY_PURPOSE_3_S))
#define EFUSE_KEY_PURPOSE_3_V  0xF
#define EFUSE_KEY_PURPOSE_3_S  4
/* EFUSE_KEY_PURPOSE_2 : RO ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: The value of KEY_PURPOSE_2.*/
#define EFUSE_KEY_PURPOSE_2  0x0000000F
#define EFUSE_KEY_PURPOSE_2_M  ((EFUSE_KEY_PURPOSE_2_V)<<(EFUSE_KEY_PURPOSE_2_S))
#define EFUSE_KEY_PURPOSE_2_V  0xF
#define EFUSE_KEY_PURPOSE_2_S  0

#define EFUSE_RD_REPEAT_DATA3_REG          (DR_REG_EFUSE_BASE + 0x03C)
/* EFUSE_ERR_RST_ENABLE : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: Use BLOCK0 to check error record registers, 0 - without check.*/
#define EFUSE_ERR_RST_ENABLE  (BIT(31))
#define EFUSE_ERR_RST_ENABLE_M  (BIT(31))
#define EFUSE_ERR_RST_ENABLE_V  0x1
#define EFUSE_ERR_RST_ENABLE_S  31
/* EFUSE_RPT4_RESERVED1 : RO ;bitpos:[30] ;default: 1'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED1  (BIT(30))
#define EFUSE_RPT4_RESERVED1_M  (BIT(30))
#define EFUSE_RPT4_RESERVED1_V  0x1
#define EFUSE_RPT4_RESERVED1_S  30
/* EFUSE_SECURE_VERSION : RO ;bitpos:[29:14] ;default: 16'h0 ; */
/*description: The value of SECURE_VERSION.*/
#define EFUSE_SECURE_VERSION  0x0000FFFF
#define EFUSE_SECURE_VERSION_M  ((EFUSE_SECURE_VERSION_V)<<(EFUSE_SECURE_VERSION_S))
#define EFUSE_SECURE_VERSION_V  0xFFFF
#define EFUSE_SECURE_VERSION_S  14
/* EFUSE_FORCE_SEND_RESUME : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: The value of FORCE_SEND_RESUME.*/
#define EFUSE_FORCE_SEND_RESUME  (BIT(13))
#define EFUSE_FORCE_SEND_RESUME_M  (BIT(13))
#define EFUSE_FORCE_SEND_RESUME_V  0x1
#define EFUSE_FORCE_SEND_RESUME_S  13
/* EFUSE_RPT4_RESERVED7 : RO ;bitpos:[12:8] ;default: 5'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED7  0x0000001F
#define EFUSE_RPT4_RESERVED7_M  ((EFUSE_RPT4_RESERVED7_V)<<(EFUSE_RPT4_RESERVED7_S))
#define EFUSE_RPT4_RESERVED7_V  0x1F
#define EFUSE_RPT4_RESERVED7_S  8
/* EFUSE_UART_PRINT_CONTROL : RO ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: The value of UART_PRINT_CONTROL.*/
#define EFUSE_UART_PRINT_CONTROL  0x00000003
#define EFUSE_UART_PRINT_CONTROL_M  ((EFUSE_UART_PRINT_CONTROL_V)<<(EFUSE_UART_PRINT_CONTROL_S))
#define EFUSE_UART_PRINT_CONTROL_V  0x3
#define EFUSE_UART_PRINT_CONTROL_S  6
/* EFUSE_ENABLE_SECURITY_DOWNLOAD : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The value of ENABLE_SECURITY_DOWNLOAD.*/
#define EFUSE_ENABLE_SECURITY_DOWNLOAD  (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_M  (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_V  0x1
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_S  5
/* EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The value of DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE.*/
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE  (BIT(4))
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_M  (BIT(4))
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_V  0x1
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_S  4
/* EFUSE_RPT4_RESERVED8 : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED8  (BIT(3))
#define EFUSE_RPT4_RESERVED8_M  (BIT(3))
#define EFUSE_RPT4_RESERVED8_V  0x1
#define EFUSE_RPT4_RESERVED8_S  3
/* EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The value of DIS_USB_SERIAL_JTAG_ROM_PRINT.*/
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT  (BIT(2))
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_M  (BIT(2))
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_V  0x1
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_S  2
/* EFUSE_DIS_DIRECT_BOOT : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The value of DIS_DIRECT_BOOT.*/
#define EFUSE_DIS_DIRECT_BOOT  (BIT(1))
#define EFUSE_DIS_DIRECT_BOOT_M  (BIT(1))
#define EFUSE_DIS_DIRECT_BOOT_V  0x1
#define EFUSE_DIS_DIRECT_BOOT_S  1
/* EFUSE_DIS_DOWNLOAD_MODE : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The value of DIS_DOWNLOAD_MODE.*/
#define EFUSE_DIS_DOWNLOAD_MODE  (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_M  (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_V  0x1
#define EFUSE_DIS_DOWNLOAD_MODE_S  0

#define EFUSE_RD_REPEAT_DATA4_REG          (DR_REG_EFUSE_BASE + 0x040)
/* EFUSE_RPT4_RESERVED4 : RO ;bitpos:[23:0] ;default: 24'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED4  0x00FFFFFF
#define EFUSE_RPT4_RESERVED4_M  ((EFUSE_RPT4_RESERVED4_V)<<(EFUSE_RPT4_RESERVED4_S))
#define EFUSE_RPT4_RESERVED4_V  0xFFFFFF
#define EFUSE_RPT4_RESERVED4_S  0

#define EFUSE_RD_MAC_SPI_SYS_0_REG          (DR_REG_EFUSE_BASE + 0x044)
/* EFUSE_MAC_0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the low 32 bits of MAC address.*/
#define EFUSE_MAC_0  0xFFFFFFFF
#define EFUSE_MAC_0_M  ((EFUSE_MAC_0_V)<<(EFUSE_MAC_0_S))
#define EFUSE_MAC_0_V  0xFFFFFFFF
#define EFUSE_MAC_0_S  0

#define EFUSE_RD_MAC_SPI_SYS_1_REG          (DR_REG_EFUSE_BASE + 0x048)
/* EFUSE_SPI_PAD_CONF_0 : RO ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: Stores the zeroth part of SPI_PAD_CONF.*/
#define EFUSE_SPI_PAD_CONF_0  0x0000FFFF
#define EFUSE_SPI_PAD_CONF_0_M  ((EFUSE_SPI_PAD_CONF_0_V)<<(EFUSE_SPI_PAD_CONF_0_S))
#define EFUSE_SPI_PAD_CONF_0_V  0xFFFF
#define EFUSE_SPI_PAD_CONF_0_S  16
/* EFUSE_MAC_1 : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: Stores the high 16 bits of MAC address.*/
#define EFUSE_MAC_1  0x0000FFFF
#define EFUSE_MAC_1_M  ((EFUSE_MAC_1_V)<<(EFUSE_MAC_1_S))
#define EFUSE_MAC_1_V  0xFFFF
#define EFUSE_MAC_1_S  0

#define EFUSE_RD_MAC_SPI_SYS_2_REG          (DR_REG_EFUSE_BASE + 0x04C)
/* EFUSE_SPI_PAD_CONF_1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first part of SPI_PAD_CONF.*/
#define EFUSE_SPI_PAD_CONF_1  0xFFFFFFFF
#define EFUSE_SPI_PAD_CONF_1_M  ((EFUSE_SPI_PAD_CONF_1_V)<<(EFUSE_SPI_PAD_CONF_1_S))
#define EFUSE_SPI_PAD_CONF_1_V  0xFFFFFFFF
#define EFUSE_SPI_PAD_CONF_1_S  0

#define EFUSE_RD_MAC_SPI_SYS_3_REG          (DR_REG_EFUSE_BASE + 0x050)
/* EFUSE_SYS_DATA_PART0_0 : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: Stores the fist 8 bits of the zeroth part of system data.*/
#define EFUSE_SYS_DATA_PART0_0  0x000000FF
#define EFUSE_SYS_DATA_PART0_0_M  ((EFUSE_SYS_DATA_PART0_0_V)<<(EFUSE_SYS_DATA_PART0_0_S))
#define EFUSE_SYS_DATA_PART0_0_V  0xFF
#define EFUSE_SYS_DATA_PART0_0_S  25
/* EFUSE_PKG_VERSION : RO ;bitpos:[23:21] ;default: 3'h0 ; */
/*description: Package version 0:ESP32-C3 */
#define EFUSE_PKG_VERSION  0x00000007
#define EFUSE_PKG_VERSION_M  ((EFUSE_PKG_VERSION_V)<<(EFUSE_PKG_VERSION_S))
#define EFUSE_PKG_VERSION_V  0x7
#define EFUSE_PKG_VERSION_S  21
/* EFUSE_WAFER_VERSION : RO ;bitpos:[20:18] ;default: 3'h0 ; */
/*description: WAFER version 0:A */
#define EFUSE_WAFER_VERSION  0x00000007
#define EFUSE_WAFER_VERSION_M  ((EFUSE_WAFER_VERSION_V)<<(EFUSE_WAFER_VERSION_S))
#define EFUSE_WAFER_VERSION_V  0x7
#define EFUSE_WAFER_VERSION_S  18
/* EFUSE_SPI_PAD_CONF_2 : RO ;bitpos:[17:0] ;default: 18'h0 ; */
/*description: Stores the second part of SPI_PAD_CONF.*/
#define EFUSE_SPI_PAD_CONF_2  0x0003FFFF
#define EFUSE_SPI_PAD_CONF_2_M  ((EFUSE_SPI_PAD_CONF_2_V)<<(EFUSE_SPI_PAD_CONF_2_S))
#define EFUSE_SPI_PAD_CONF_2_V  0x3FFFF
#define EFUSE_SPI_PAD_CONF_2_S  0

#define EFUSE_RD_MAC_SPI_SYS_4_REG          (DR_REG_EFUSE_BASE + 0x054)
/* EFUSE_SYS_DATA_PART0_1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fist 32 bits of the zeroth part of system data.*/
#define EFUSE_SYS_DATA_PART0_1  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART0_1_M  ((EFUSE_SYS_DATA_PART0_1_V)<<(EFUSE_SYS_DATA_PART0_1_S))
#define EFUSE_SYS_DATA_PART0_1_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART0_1_S  0

#define EFUSE_RD_MAC_SPI_SYS_5_REG          (DR_REG_EFUSE_BASE + 0x058)
/* EFUSE_SYS_DATA_PART0_2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of the zeroth part of system data.*/
#define EFUSE_SYS_DATA_PART0_2  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART0_2_M  ((EFUSE_SYS_DATA_PART0_2_V)<<(EFUSE_SYS_DATA_PART0_2_S))
#define EFUSE_SYS_DATA_PART0_2_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART0_2_S  0

#define EFUSE_RD_SYS_PART1_DATA0_REG          (DR_REG_EFUSE_BASE + 0x05C)
/* EFUSE_SYS_DATA_PART1_0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of the first part of system data.*/
#define EFUSE_SYS_DATA_PART1_0  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_0_M  ((EFUSE_SYS_DATA_PART1_0_V)<<(EFUSE_SYS_DATA_PART1_0_S))
#define EFUSE_SYS_DATA_PART1_0_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_0_S  0

#define EFUSE_RD_SYS_PART1_DATA1_REG          (DR_REG_EFUSE_BASE + 0x060)
/* EFUSE_SYS_DATA_PART1_1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of the first part of system data.*/
#define EFUSE_SYS_DATA_PART1_1  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_1_M  ((EFUSE_SYS_DATA_PART1_1_V)<<(EFUSE_SYS_DATA_PART1_1_S))
#define EFUSE_SYS_DATA_PART1_1_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_1_S  0

#define EFUSE_RD_SYS_PART1_DATA2_REG          (DR_REG_EFUSE_BASE + 0x064)
/* EFUSE_SYS_DATA_PART1_2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of the first part of system data.*/
#define EFUSE_SYS_DATA_PART1_2  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_2_M  ((EFUSE_SYS_DATA_PART1_2_V)<<(EFUSE_SYS_DATA_PART1_2_S))
#define EFUSE_SYS_DATA_PART1_2_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_2_S  0

#define EFUSE_RD_SYS_PART1_DATA3_REG          (DR_REG_EFUSE_BASE + 0x068)
/* EFUSE_SYS_DATA_PART1_3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of the first part of system data.*/
#define EFUSE_SYS_DATA_PART1_3  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_3_M  ((EFUSE_SYS_DATA_PART1_3_V)<<(EFUSE_SYS_DATA_PART1_3_S))
#define EFUSE_SYS_DATA_PART1_3_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_3_S  0

#define EFUSE_RD_SYS_PART1_DATA4_REG          (DR_REG_EFUSE_BASE + 0x06C)
/* EFUSE_SYS_DATA_PART1_4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of the first part of system data.*/
#define EFUSE_SYS_DATA_PART1_4  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_4_M  ((EFUSE_SYS_DATA_PART1_4_V)<<(EFUSE_SYS_DATA_PART1_4_S))
#define EFUSE_SYS_DATA_PART1_4_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_4_S  0

#define EFUSE_RD_SYS_PART1_DATA5_REG          (DR_REG_EFUSE_BASE + 0x070)
/* EFUSE_SYS_DATA_PART1_5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of the first part of system data.*/
#define EFUSE_SYS_DATA_PART1_5  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_5_M  ((EFUSE_SYS_DATA_PART1_5_V)<<(EFUSE_SYS_DATA_PART1_5_S))
#define EFUSE_SYS_DATA_PART1_5_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_5_S  0

#define EFUSE_RD_SYS_PART1_DATA6_REG          (DR_REG_EFUSE_BASE + 0x074)
/* EFUSE_SYS_DATA_PART1_6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of the first part of system data.*/
#define EFUSE_SYS_DATA_PART1_6  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_6_M  ((EFUSE_SYS_DATA_PART1_6_V)<<(EFUSE_SYS_DATA_PART1_6_S))
#define EFUSE_SYS_DATA_PART1_6_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_6_S  0

#define EFUSE_RD_SYS_PART1_DATA7_REG          (DR_REG_EFUSE_BASE + 0x078)
/* EFUSE_SYS_DATA_PART1_7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of the first part of system data.*/
#define EFUSE_SYS_DATA_PART1_7  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_7_M  ((EFUSE_SYS_DATA_PART1_7_V)<<(EFUSE_SYS_DATA_PART1_7_S))
#define EFUSE_SYS_DATA_PART1_7_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_7_S  0

#define EFUSE_RD_USR_DATA0_REG          (DR_REG_EFUSE_BASE + 0x07C)
/* EFUSE_USR_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of BLOCK3 (user).*/
#define EFUSE_USR_DATA0  0xFFFFFFFF
#define EFUSE_USR_DATA0_M  ((EFUSE_USR_DATA0_V)<<(EFUSE_USR_DATA0_S))
#define EFUSE_USR_DATA0_V  0xFFFFFFFF
#define EFUSE_USR_DATA0_S  0

#define EFUSE_RD_USR_DATA1_REG          (DR_REG_EFUSE_BASE + 0x080)
/* EFUSE_USR_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of BLOCK3 (user).*/
#define EFUSE_USR_DATA1  0xFFFFFFFF
#define EFUSE_USR_DATA1_M  ((EFUSE_USR_DATA1_V)<<(EFUSE_USR_DATA1_S))
#define EFUSE_USR_DATA1_V  0xFFFFFFFF
#define EFUSE_USR_DATA1_S  0

#define EFUSE_RD_USR_DATA2_REG          (DR_REG_EFUSE_BASE + 0x084)
/* EFUSE_USR_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of BLOCK3 (user).*/
#define EFUSE_USR_DATA2  0xFFFFFFFF
#define EFUSE_USR_DATA2_M  ((EFUSE_USR_DATA2_V)<<(EFUSE_USR_DATA2_S))
#define EFUSE_USR_DATA2_V  0xFFFFFFFF
#define EFUSE_USR_DATA2_S  0

#define EFUSE_RD_USR_DATA3_REG          (DR_REG_EFUSE_BASE + 0x088)
/* EFUSE_USR_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of BLOCK3 (user).*/
#define EFUSE_USR_DATA3  0xFFFFFFFF
#define EFUSE_USR_DATA3_M  ((EFUSE_USR_DATA3_V)<<(EFUSE_USR_DATA3_S))
#define EFUSE_USR_DATA3_V  0xFFFFFFFF
#define EFUSE_USR_DATA3_S  0

#define EFUSE_RD_USR_DATA4_REG          (DR_REG_EFUSE_BASE + 0x08C)
/* EFUSE_USR_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of BLOCK3 (user).*/
#define EFUSE_USR_DATA4  0xFFFFFFFF
#define EFUSE_USR_DATA4_M  ((EFUSE_USR_DATA4_V)<<(EFUSE_USR_DATA4_S))
#define EFUSE_USR_DATA4_V  0xFFFFFFFF
#define EFUSE_USR_DATA4_S  0

#define EFUSE_RD_USR_DATA5_REG          (DR_REG_EFUSE_BASE + 0x090)
/* EFUSE_USR_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of BLOCK3 (user).*/
#define EFUSE_USR_DATA5  0xFFFFFFFF
#define EFUSE_USR_DATA5_M  ((EFUSE_USR_DATA5_V)<<(EFUSE_USR_DATA5_S))
#define EFUSE_USR_DATA5_V  0xFFFFFFFF
#define EFUSE_USR_DATA5_S  0

#define EFUSE_RD_USR_DATA6_REG          (DR_REG_EFUSE_BASE + 0x094)
/* EFUSE_USR_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of BLOCK3 (user).*/
#define EFUSE_USR_DATA6  0xFFFFFFFF
#define EFUSE_USR_DATA6_M  ((EFUSE_USR_DATA6_V)<<(EFUSE_USR_DATA6_S))
#define EFUSE_USR_DATA6_V  0xFFFFFFFF
#define EFUSE_USR_DATA6_S  0

#define EFUSE_RD_USR_DATA7_REG          (DR_REG_EFUSE_BASE + 0x098)
/* EFUSE_USR_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of BLOCK3 (user).*/
#define EFUSE_USR_DATA7  0xFFFFFFFF
#define EFUSE_USR_DATA7_M  ((EFUSE_USR_DATA7_V)<<(EFUSE_USR_DATA7_S))
#define EFUSE_USR_DATA7_V  0xFFFFFFFF
#define EFUSE_USR_DATA7_S  0

#define EFUSE_RD_KEY0_DATA0_REG          (DR_REG_EFUSE_BASE + 0x09C)
/* EFUSE_KEY0_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of KEY0.*/
#define EFUSE_KEY0_DATA0  0xFFFFFFFF
#define EFUSE_KEY0_DATA0_M  ((EFUSE_KEY0_DATA0_V)<<(EFUSE_KEY0_DATA0_S))
#define EFUSE_KEY0_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA0_S  0

#define EFUSE_RD_KEY0_DATA1_REG          (DR_REG_EFUSE_BASE + 0x0A0)
/* EFUSE_KEY0_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of KEY0.*/
#define EFUSE_KEY0_DATA1  0xFFFFFFFF
#define EFUSE_KEY0_DATA1_M  ((EFUSE_KEY0_DATA1_V)<<(EFUSE_KEY0_DATA1_S))
#define EFUSE_KEY0_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA1_S  0

#define EFUSE_RD_KEY0_DATA2_REG          (DR_REG_EFUSE_BASE + 0x0A4)
/* EFUSE_KEY0_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of KEY0.*/
#define EFUSE_KEY0_DATA2  0xFFFFFFFF
#define EFUSE_KEY0_DATA2_M  ((EFUSE_KEY0_DATA2_V)<<(EFUSE_KEY0_DATA2_S))
#define EFUSE_KEY0_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA2_S  0

#define EFUSE_RD_KEY0_DATA3_REG          (DR_REG_EFUSE_BASE + 0x0A8)
/* EFUSE_KEY0_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of KEY0.*/
#define EFUSE_KEY0_DATA3  0xFFFFFFFF
#define EFUSE_KEY0_DATA3_M  ((EFUSE_KEY0_DATA3_V)<<(EFUSE_KEY0_DATA3_S))
#define EFUSE_KEY0_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA3_S  0

#define EFUSE_RD_KEY0_DATA4_REG          (DR_REG_EFUSE_BASE + 0x0AC)
/* EFUSE_KEY0_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of KEY0.*/
#define EFUSE_KEY0_DATA4  0xFFFFFFFF
#define EFUSE_KEY0_DATA4_M  ((EFUSE_KEY0_DATA4_V)<<(EFUSE_KEY0_DATA4_S))
#define EFUSE_KEY0_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA4_S  0

#define EFUSE_RD_KEY0_DATA5_REG          (DR_REG_EFUSE_BASE + 0x0B0)
/* EFUSE_KEY0_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of KEY0.*/
#define EFUSE_KEY0_DATA5  0xFFFFFFFF
#define EFUSE_KEY0_DATA5_M  ((EFUSE_KEY0_DATA5_V)<<(EFUSE_KEY0_DATA5_S))
#define EFUSE_KEY0_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA5_S  0

#define EFUSE_RD_KEY0_DATA6_REG          (DR_REG_EFUSE_BASE + 0x0B4)
/* EFUSE_KEY0_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of KEY0.*/
#define EFUSE_KEY0_DATA6  0xFFFFFFFF
#define EFUSE_KEY0_DATA6_M  ((EFUSE_KEY0_DATA6_V)<<(EFUSE_KEY0_DATA6_S))
#define EFUSE_KEY0_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA6_S  0

#define EFUSE_RD_KEY0_DATA7_REG          (DR_REG_EFUSE_BASE + 0x0B8)
/* EFUSE_KEY0_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of KEY0.*/
#define EFUSE_KEY0_DATA7  0xFFFFFFFF
#define EFUSE_KEY0_DATA7_M  ((EFUSE_KEY0_DATA7_V)<<(EFUSE_KEY0_DATA7_S))
#define EFUSE_KEY0_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA7_S  0

#define EFUSE_RD_KEY1_DATA0_REG          (DR_REG_EFUSE_BASE + 0x0BC)
/* EFUSE_KEY1_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of KEY1.*/
#define EFUSE_KEY1_DATA0  0xFFFFFFFF
#define EFUSE_KEY1_DATA0_M  ((EFUSE_KEY1_DATA0_V)<<(EFUSE_KEY1_DATA0_S))
#define EFUSE_KEY1_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA0_S  0

#define EFUSE_RD_KEY1_DATA1_REG          (DR_REG_EFUSE_BASE + 0x0C0)
/* EFUSE_KEY1_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of KEY1.*/
#define EFUSE_KEY1_DATA1  0xFFFFFFFF
#define EFUSE_KEY1_DATA1_M  ((EFUSE_KEY1_DATA1_V)<<(EFUSE_KEY1_DATA1_S))
#define EFUSE_KEY1_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA1_S  0

#define EFUSE_RD_KEY1_DATA2_REG          (DR_REG_EFUSE_BASE + 0x0C4)
/* EFUSE_KEY1_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of KEY1.*/
#define EFUSE_KEY1_DATA2  0xFFFFFFFF
#define EFUSE_KEY1_DATA2_M  ((EFUSE_KEY1_DATA2_V)<<(EFUSE_KEY1_DATA2_S))
#define EFUSE_KEY1_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA2_S  0

#define EFUSE_RD_KEY1_DATA3_REG          (DR_REG_EFUSE_BASE + 0x0C8)
/* EFUSE_KEY1_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of KEY1.*/
#define EFUSE_KEY1_DATA3  0xFFFFFFFF
#define EFUSE_KEY1_DATA3_M  ((EFUSE_KEY1_DATA3_V)<<(EFUSE_KEY1_DATA3_S))
#define EFUSE_KEY1_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA3_S  0

#define EFUSE_RD_KEY1_DATA4_REG          (DR_REG_EFUSE_BASE + 0x0CC)
/* EFUSE_KEY1_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of KEY1.*/
#define EFUSE_KEY1_DATA4  0xFFFFFFFF
#define EFUSE_KEY1_DATA4_M  ((EFUSE_KEY1_DATA4_V)<<(EFUSE_KEY1_DATA4_S))
#define EFUSE_KEY1_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA4_S  0

#define EFUSE_RD_KEY1_DATA5_REG          (DR_REG_EFUSE_BASE + 0x0D0)
/* EFUSE_KEY1_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of KEY1.*/
#define EFUSE_KEY1_DATA5  0xFFFFFFFF
#define EFUSE_KEY1_DATA5_M  ((EFUSE_KEY1_DATA5_V)<<(EFUSE_KEY1_DATA5_S))
#define EFUSE_KEY1_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA5_S  0

#define EFUSE_RD_KEY1_DATA6_REG          (DR_REG_EFUSE_BASE + 0x0D4)
/* EFUSE_KEY1_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of KEY1.*/
#define EFUSE_KEY1_DATA6  0xFFFFFFFF
#define EFUSE_KEY1_DATA6_M  ((EFUSE_KEY1_DATA6_V)<<(EFUSE_KEY1_DATA6_S))
#define EFUSE_KEY1_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA6_S  0

#define EFUSE_RD_KEY1_DATA7_REG          (DR_REG_EFUSE_BASE + 0x0D8)
/* EFUSE_KEY1_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of KEY1.*/
#define EFUSE_KEY1_DATA7  0xFFFFFFFF
#define EFUSE_KEY1_DATA7_M  ((EFUSE_KEY1_DATA7_V)<<(EFUSE_KEY1_DATA7_S))
#define EFUSE_KEY1_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA7_S  0

#define EFUSE_RD_KEY2_DATA0_REG          (DR_REG_EFUSE_BASE + 0x0DC)
/* EFUSE_KEY2_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of KEY2.*/
#define EFUSE_KEY2_DATA0  0xFFFFFFFF
#define EFUSE_KEY2_DATA0_M  ((EFUSE_KEY2_DATA0_V)<<(EFUSE_KEY2_DATA0_S))
#define EFUSE_KEY2_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA0_S  0

#define EFUSE_RD_KEY2_DATA1_REG          (DR_REG_EFUSE_BASE + 0x0E0)
/* EFUSE_KEY2_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of KEY2.*/
#define EFUSE_KEY2_DATA1  0xFFFFFFFF
#define EFUSE_KEY2_DATA1_M  ((EFUSE_KEY2_DATA1_V)<<(EFUSE_KEY2_DATA1_S))
#define EFUSE_KEY2_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA1_S  0

#define EFUSE_RD_KEY2_DATA2_REG          (DR_REG_EFUSE_BASE + 0x0E4)
/* EFUSE_KEY2_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of KEY2.*/
#define EFUSE_KEY2_DATA2  0xFFFFFFFF
#define EFUSE_KEY2_DATA2_M  ((EFUSE_KEY2_DATA2_V)<<(EFUSE_KEY2_DATA2_S))
#define EFUSE_KEY2_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA2_S  0

#define EFUSE_RD_KEY2_DATA3_REG          (DR_REG_EFUSE_BASE + 0x0E8)
/* EFUSE_KEY2_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of KEY2.*/
#define EFUSE_KEY2_DATA3  0xFFFFFFFF
#define EFUSE_KEY2_DATA3_M  ((EFUSE_KEY2_DATA3_V)<<(EFUSE_KEY2_DATA3_S))
#define EFUSE_KEY2_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA3_S  0

#define EFUSE_RD_KEY2_DATA4_REG          (DR_REG_EFUSE_BASE + 0x0EC)
/* EFUSE_KEY2_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of KEY2.*/
#define EFUSE_KEY2_DATA4  0xFFFFFFFF
#define EFUSE_KEY2_DATA4_M  ((EFUSE_KEY2_DATA4_V)<<(EFUSE_KEY2_DATA4_S))
#define EFUSE_KEY2_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA4_S  0

#define EFUSE_RD_KEY2_DATA5_REG          (DR_REG_EFUSE_BASE + 0x0F0)
/* EFUSE_KEY2_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of KEY2.*/
#define EFUSE_KEY2_DATA5  0xFFFFFFFF
#define EFUSE_KEY2_DATA5_M  ((EFUSE_KEY2_DATA5_V)<<(EFUSE_KEY2_DATA5_S))
#define EFUSE_KEY2_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA5_S  0

#define EFUSE_RD_KEY2_DATA6_REG          (DR_REG_EFUSE_BASE + 0x0F4)
/* EFUSE_KEY2_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of KEY2.*/
#define EFUSE_KEY2_DATA6  0xFFFFFFFF
#define EFUSE_KEY2_DATA6_M  ((EFUSE_KEY2_DATA6_V)<<(EFUSE_KEY2_DATA6_S))
#define EFUSE_KEY2_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA6_S  0

#define EFUSE_RD_KEY2_DATA7_REG          (DR_REG_EFUSE_BASE + 0x0F8)
/* EFUSE_KEY2_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of KEY2.*/
#define EFUSE_KEY2_DATA7  0xFFFFFFFF
#define EFUSE_KEY2_DATA7_M  ((EFUSE_KEY2_DATA7_V)<<(EFUSE_KEY2_DATA7_S))
#define EFUSE_KEY2_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA7_S  0

#define EFUSE_RD_KEY3_DATA0_REG          (DR_REG_EFUSE_BASE + 0x0FC)
/* EFUSE_KEY3_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of KEY3.*/
#define EFUSE_KEY3_DATA0  0xFFFFFFFF
#define EFUSE_KEY3_DATA0_M  ((EFUSE_KEY3_DATA0_V)<<(EFUSE_KEY3_DATA0_S))
#define EFUSE_KEY3_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA0_S  0

#define EFUSE_RD_KEY3_DATA1_REG          (DR_REG_EFUSE_BASE + 0x100)
/* EFUSE_KEY3_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of KEY3.*/
#define EFUSE_KEY3_DATA1  0xFFFFFFFF
#define EFUSE_KEY3_DATA1_M  ((EFUSE_KEY3_DATA1_V)<<(EFUSE_KEY3_DATA1_S))
#define EFUSE_KEY3_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA1_S  0

#define EFUSE_RD_KEY3_DATA2_REG          (DR_REG_EFUSE_BASE + 0x104)
/* EFUSE_KEY3_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of KEY3.*/
#define EFUSE_KEY3_DATA2  0xFFFFFFFF
#define EFUSE_KEY3_DATA2_M  ((EFUSE_KEY3_DATA2_V)<<(EFUSE_KEY3_DATA2_S))
#define EFUSE_KEY3_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA2_S  0

#define EFUSE_RD_KEY3_DATA3_REG          (DR_REG_EFUSE_BASE + 0x108)
/* EFUSE_KEY3_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of KEY3.*/
#define EFUSE_KEY3_DATA3  0xFFFFFFFF
#define EFUSE_KEY3_DATA3_M  ((EFUSE_KEY3_DATA3_V)<<(EFUSE_KEY3_DATA3_S))
#define EFUSE_KEY3_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA3_S  0

#define EFUSE_RD_KEY3_DATA4_REG          (DR_REG_EFUSE_BASE + 0x10C)
/* EFUSE_KEY3_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of KEY3.*/
#define EFUSE_KEY3_DATA4  0xFFFFFFFF
#define EFUSE_KEY3_DATA4_M  ((EFUSE_KEY3_DATA4_V)<<(EFUSE_KEY3_DATA4_S))
#define EFUSE_KEY3_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA4_S  0

#define EFUSE_RD_KEY3_DATA5_REG          (DR_REG_EFUSE_BASE + 0x110)
/* EFUSE_KEY3_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of KEY3.*/
#define EFUSE_KEY3_DATA5  0xFFFFFFFF
#define EFUSE_KEY3_DATA5_M  ((EFUSE_KEY3_DATA5_V)<<(EFUSE_KEY3_DATA5_S))
#define EFUSE_KEY3_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA5_S  0

#define EFUSE_RD_KEY3_DATA6_REG          (DR_REG_EFUSE_BASE + 0x114)
/* EFUSE_KEY3_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of KEY3.*/
#define EFUSE_KEY3_DATA6  0xFFFFFFFF
#define EFUSE_KEY3_DATA6_M  ((EFUSE_KEY3_DATA6_V)<<(EFUSE_KEY3_DATA6_S))
#define EFUSE_KEY3_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA6_S  0

#define EFUSE_RD_KEY3_DATA7_REG          (DR_REG_EFUSE_BASE + 0x118)
/* EFUSE_KEY3_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of KEY3.*/
#define EFUSE_KEY3_DATA7  0xFFFFFFFF
#define EFUSE_KEY3_DATA7_M  ((EFUSE_KEY3_DATA7_V)<<(EFUSE_KEY3_DATA7_S))
#define EFUSE_KEY3_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA7_S  0

#define EFUSE_RD_KEY4_DATA0_REG          (DR_REG_EFUSE_BASE + 0x11C)
/* EFUSE_KEY4_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of KEY4.*/
#define EFUSE_KEY4_DATA0  0xFFFFFFFF
#define EFUSE_KEY4_DATA0_M  ((EFUSE_KEY4_DATA0_V)<<(EFUSE_KEY4_DATA0_S))
#define EFUSE_KEY4_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA0_S  0

#define EFUSE_RD_KEY4_DATA1_REG          (DR_REG_EFUSE_BASE + 0x120)
/* EFUSE_KEY4_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of KEY4.*/
#define EFUSE_KEY4_DATA1  0xFFFFFFFF
#define EFUSE_KEY4_DATA1_M  ((EFUSE_KEY4_DATA1_V)<<(EFUSE_KEY4_DATA1_S))
#define EFUSE_KEY4_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA1_S  0

#define EFUSE_RD_KEY4_DATA2_REG          (DR_REG_EFUSE_BASE + 0x124)
/* EFUSE_KEY4_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of KEY4.*/
#define EFUSE_KEY4_DATA2  0xFFFFFFFF
#define EFUSE_KEY4_DATA2_M  ((EFUSE_KEY4_DATA2_V)<<(EFUSE_KEY4_DATA2_S))
#define EFUSE_KEY4_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA2_S  0

#define EFUSE_RD_KEY4_DATA3_REG          (DR_REG_EFUSE_BASE + 0x128)
/* EFUSE_KEY4_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of KEY4.*/
#define EFUSE_KEY4_DATA3  0xFFFFFFFF
#define EFUSE_KEY4_DATA3_M  ((EFUSE_KEY4_DATA3_V)<<(EFUSE_KEY4_DATA3_S))
#define EFUSE_KEY4_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA3_S  0

#define EFUSE_RD_KEY4_DATA4_REG          (DR_REG_EFUSE_BASE + 0x12C)
/* EFUSE_KEY4_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of KEY4.*/
#define EFUSE_KEY4_DATA4  0xFFFFFFFF
#define EFUSE_KEY4_DATA4_M  ((EFUSE_KEY4_DATA4_V)<<(EFUSE_KEY4_DATA4_S))
#define EFUSE_KEY4_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA4_S  0

#define EFUSE_RD_KEY4_DATA5_REG          (DR_REG_EFUSE_BASE + 0x130)
/* EFUSE_KEY4_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of KEY4.*/
#define EFUSE_KEY4_DATA5  0xFFFFFFFF
#define EFUSE_KEY4_DATA5_M  ((EFUSE_KEY4_DATA5_V)<<(EFUSE_KEY4_DATA5_S))
#define EFUSE_KEY4_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA5_S  0

#define EFUSE_RD_KEY4_DATA6_REG          (DR_REG_EFUSE_BASE + 0x134)
/* EFUSE_KEY4_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of KEY4.*/
#define EFUSE_KEY4_DATA6  0xFFFFFFFF
#define EFUSE_KEY4_DATA6_M  ((EFUSE_KEY4_DATA6_V)<<(EFUSE_KEY4_DATA6_S))
#define EFUSE_KEY4_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA6_S  0

#define EFUSE_RD_KEY4_DATA7_REG          (DR_REG_EFUSE_BASE + 0x138)
/* EFUSE_KEY4_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of KEY4.*/
#define EFUSE_KEY4_DATA7  0xFFFFFFFF
#define EFUSE_KEY4_DATA7_M  ((EFUSE_KEY4_DATA7_V)<<(EFUSE_KEY4_DATA7_S))
#define EFUSE_KEY4_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA7_S  0

#define EFUSE_RD_KEY5_DATA0_REG          (DR_REG_EFUSE_BASE + 0x13C)
/* EFUSE_KEY5_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of KEY5.*/
#define EFUSE_KEY5_DATA0  0xFFFFFFFF
#define EFUSE_KEY5_DATA0_M  ((EFUSE_KEY5_DATA0_V)<<(EFUSE_KEY5_DATA0_S))
#define EFUSE_KEY5_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA0_S  0

#define EFUSE_RD_KEY5_DATA1_REG          (DR_REG_EFUSE_BASE + 0x140)
/* EFUSE_KEY5_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of KEY5.*/
#define EFUSE_KEY5_DATA1  0xFFFFFFFF
#define EFUSE_KEY5_DATA1_M  ((EFUSE_KEY5_DATA1_V)<<(EFUSE_KEY5_DATA1_S))
#define EFUSE_KEY5_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA1_S  0

#define EFUSE_RD_KEY5_DATA2_REG          (DR_REG_EFUSE_BASE + 0x144)
/* EFUSE_KEY5_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of KEY5.*/
#define EFUSE_KEY5_DATA2  0xFFFFFFFF
#define EFUSE_KEY5_DATA2_M  ((EFUSE_KEY5_DATA2_V)<<(EFUSE_KEY5_DATA2_S))
#define EFUSE_KEY5_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA2_S  0

#define EFUSE_RD_KEY5_DATA3_REG          (DR_REG_EFUSE_BASE + 0x148)
/* EFUSE_KEY5_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of KEY5.*/
#define EFUSE_KEY5_DATA3  0xFFFFFFFF
#define EFUSE_KEY5_DATA3_M  ((EFUSE_KEY5_DATA3_V)<<(EFUSE_KEY5_DATA3_S))
#define EFUSE_KEY5_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA3_S  0

#define EFUSE_RD_KEY5_DATA4_REG          (DR_REG_EFUSE_BASE + 0x14C)
/* EFUSE_KEY5_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of KEY5.*/
#define EFUSE_KEY5_DATA4  0xFFFFFFFF
#define EFUSE_KEY5_DATA4_M  ((EFUSE_KEY5_DATA4_V)<<(EFUSE_KEY5_DATA4_S))
#define EFUSE_KEY5_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA4_S  0

#define EFUSE_RD_KEY5_DATA5_REG          (DR_REG_EFUSE_BASE + 0x150)
/* EFUSE_KEY5_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of KEY5.*/
#define EFUSE_KEY5_DATA5  0xFFFFFFFF
#define EFUSE_KEY5_DATA5_M  ((EFUSE_KEY5_DATA5_V)<<(EFUSE_KEY5_DATA5_S))
#define EFUSE_KEY5_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA5_S  0

#define EFUSE_RD_KEY5_DATA6_REG          (DR_REG_EFUSE_BASE + 0x154)
/* EFUSE_KEY5_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of KEY5.*/
#define EFUSE_KEY5_DATA6  0xFFFFFFFF
#define EFUSE_KEY5_DATA6_M  ((EFUSE_KEY5_DATA6_V)<<(EFUSE_KEY5_DATA6_S))
#define EFUSE_KEY5_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA6_S  0

#define EFUSE_RD_KEY5_DATA7_REG          (DR_REG_EFUSE_BASE + 0x158)
/* EFUSE_KEY5_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of KEY5.*/
#define EFUSE_KEY5_DATA7  0xFFFFFFFF
#define EFUSE_KEY5_DATA7_M  ((EFUSE_KEY5_DATA7_V)<<(EFUSE_KEY5_DATA7_S))
#define EFUSE_KEY5_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA7_S  0

#define EFUSE_RD_SYS_PART2_DATA0_REG          (DR_REG_EFUSE_BASE + 0x15C)
/* EFUSE_SYS_DATA_PART2_0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the $nth 32 bits of the 2nd part of system data.*/
#define EFUSE_SYS_DATA_PART2_0  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_0_M  ((EFUSE_SYS_DATA_PART2_0_V)<<(EFUSE_SYS_DATA_PART2_0_S))
#define EFUSE_SYS_DATA_PART2_0_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_0_S  0

#define EFUSE_RD_SYS_PART2_DATA1_REG          (DR_REG_EFUSE_BASE + 0x160)
/* EFUSE_SYS_DATA_PART2_1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the $nth 32 bits of the 2nd part of system data.*/
#define EFUSE_SYS_DATA_PART2_1  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_1_M  ((EFUSE_SYS_DATA_PART2_1_V)<<(EFUSE_SYS_DATA_PART2_1_S))
#define EFUSE_SYS_DATA_PART2_1_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_1_S  0

#define EFUSE_RD_SYS_PART2_DATA2_REG          (DR_REG_EFUSE_BASE + 0x164)
/* EFUSE_SYS_DATA_PART2_2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the $nth 32 bits of the 2nd part of system data.*/
#define EFUSE_SYS_DATA_PART2_2  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_2_M  ((EFUSE_SYS_DATA_PART2_2_V)<<(EFUSE_SYS_DATA_PART2_2_S))
#define EFUSE_SYS_DATA_PART2_2_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_2_S  0

#define EFUSE_RD_SYS_PART2_DATA3_REG          (DR_REG_EFUSE_BASE + 0x168)
/* EFUSE_SYS_DATA_PART2_3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the $nth 32 bits of the 2nd part of system data.*/
#define EFUSE_SYS_DATA_PART2_3  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_3_M  ((EFUSE_SYS_DATA_PART2_3_V)<<(EFUSE_SYS_DATA_PART2_3_S))
#define EFUSE_SYS_DATA_PART2_3_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_3_S  0

#define EFUSE_RD_SYS_PART2_DATA4_REG          (DR_REG_EFUSE_BASE + 0x16C)
/* EFUSE_SYS_DATA_PART2_4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the $nth 32 bits of the 2nd part of system data.*/
#define EFUSE_SYS_DATA_PART2_4  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_4_M  ((EFUSE_SYS_DATA_PART2_4_V)<<(EFUSE_SYS_DATA_PART2_4_S))
#define EFUSE_SYS_DATA_PART2_4_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_4_S  0

#define EFUSE_RD_SYS_PART2_DATA5_REG          (DR_REG_EFUSE_BASE + 0x170)
/* EFUSE_SYS_DATA_PART2_5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the $nth 32 bits of the 2nd part of system data.*/
#define EFUSE_SYS_DATA_PART2_5  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_5_M  ((EFUSE_SYS_DATA_PART2_5_V)<<(EFUSE_SYS_DATA_PART2_5_S))
#define EFUSE_SYS_DATA_PART2_5_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_5_S  0

#define EFUSE_RD_SYS_PART2_DATA6_REG          (DR_REG_EFUSE_BASE + 0x174)
/* EFUSE_SYS_DATA_PART2_6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the $nth 32 bits of the 2nd part of system data.*/
#define EFUSE_SYS_DATA_PART2_6  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_6_M  ((EFUSE_SYS_DATA_PART2_6_V)<<(EFUSE_SYS_DATA_PART2_6_S))
#define EFUSE_SYS_DATA_PART2_6_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_6_S  0

#define EFUSE_RD_SYS_PART2_DATA7_REG          (DR_REG_EFUSE_BASE + 0x178)
/* EFUSE_SYS_DATA_PART2_7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the $nth 32 bits of the 2nd part of system data.*/
#define EFUSE_SYS_DATA_PART2_7  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_7_M  ((EFUSE_SYS_DATA_PART2_7_V)<<(EFUSE_SYS_DATA_PART2_7_S))
#define EFUSE_SYS_DATA_PART2_7_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_7_S  0

#define EFUSE_RD_REPEAT_ERR0_REG          (DR_REG_EFUSE_BASE + 0x17C)
/* EFUSE_POWER_GLITCH_DSENSE_ERR : RO ;bitpos:[31:30] ;default: 2'h0 ; */
/*description: If any bit in POWER_GLITCH_DSENSE is 1  then it indicates a programming error.*/
#define EFUSE_POWER_GLITCH_DSENSE_ERR  0x00000003
#define EFUSE_POWER_GLITCH_DSENSE_ERR_M  ((EFUSE_POWER_GLITCH_DSENSE_ERR_V)<<(EFUSE_POWER_GLITCH_DSENSE_ERR_S))
#define EFUSE_POWER_GLITCH_DSENSE_ERR_V  0x3
#define EFUSE_POWER_GLITCH_DSENSE_ERR_S  30
/* EFUSE_POWERGLITCH_EN_ERR : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: If POWERGLITCH_EN is 1  then it indicates a programming error.*/
#define EFUSE_POWERGLITCH_EN_ERR  (BIT(29))
#define EFUSE_POWERGLITCH_EN_ERR_M  (BIT(29))
#define EFUSE_POWERGLITCH_EN_ERR_V  0x1
#define EFUSE_POWERGLITCH_EN_ERR_S  29
/* EFUSE_BTLC_GPIO_ENABLE_ERR : RO ;bitpos:[28:27] ;default: 2'h0 ; */
/*description: If any bit in BTLC_GPIO_ENABLE is 1  then it indicates a programming error.*/
#define EFUSE_BTLC_GPIO_ENABLE_ERR  0x00000003
#define EFUSE_BTLC_GPIO_ENABLE_ERR_M  ((EFUSE_BTLC_GPIO_ENABLE_ERR_V)<<(EFUSE_BTLC_GPIO_ENABLE_ERR_S))
#define EFUSE_BTLC_GPIO_ENABLE_ERR_V  0x3
#define EFUSE_BTLC_GPIO_ENABLE_ERR_S  27
/* EFUSE_VDD_SPI_AS_GPIO_ERR : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: If VDD_SPI_AS_GPIO is 1  then it indicates a programming error.*/
#define EFUSE_VDD_SPI_AS_GPIO_ERR  (BIT(26))
#define EFUSE_VDD_SPI_AS_GPIO_ERR_M  (BIT(26))
#define EFUSE_VDD_SPI_AS_GPIO_ERR_V  0x1
#define EFUSE_VDD_SPI_AS_GPIO_ERR_S  26
/* EFUSE_USB_EXCHG_PINS_ERR : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: If USB_EXCHG_PINS is 1  then it indicates a programming error.*/
#define EFUSE_USB_EXCHG_PINS_ERR  (BIT(25))
#define EFUSE_USB_EXCHG_PINS_ERR_M  (BIT(25))
#define EFUSE_USB_EXCHG_PINS_ERR_V  0x1
#define EFUSE_USB_EXCHG_PINS_ERR_S  25
/* EFUSE_USB_DREFL_ERR : RO ;bitpos:[24:23] ;default: 2'h0 ; */
/*description: If any bit in USB_DREFL is 1  then it indicates a programming error.*/
#define EFUSE_USB_DREFL_ERR  0x00000003
#define EFUSE_USB_DREFL_ERR_M  ((EFUSE_USB_DREFL_ERR_V)<<(EFUSE_USB_DREFL_ERR_S))
#define EFUSE_USB_DREFL_ERR_V  0x3
#define EFUSE_USB_DREFL_ERR_S  23
/* EFUSE_USB_DREFH_ERR : RO ;bitpos:[22:21] ;default: 2'h0 ; */
/*description: If any bit in USB_DREFH is 1  then it indicates a programming error.*/
#define EFUSE_USB_DREFH_ERR  0x00000003
#define EFUSE_USB_DREFH_ERR_M  ((EFUSE_USB_DREFH_ERR_V)<<(EFUSE_USB_DREFH_ERR_S))
#define EFUSE_USB_DREFH_ERR_V  0x3
#define EFUSE_USB_DREFH_ERR_S  21
/* EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: If DIS_DOWNLOAD_MANUAL_ENCRYPT is 1  then it indicates a programming error.*/
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR  (BIT(20))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_M  (BIT(20))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_V  0x1
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_S  20
/* EFUSE_DIS_PAD_JTAG_ERR : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: If DIS_PAD_JTAG is 1  then it indicates a programming error.*/
#define EFUSE_DIS_PAD_JTAG_ERR  (BIT(19))
#define EFUSE_DIS_PAD_JTAG_ERR_M  (BIT(19))
#define EFUSE_DIS_PAD_JTAG_ERR_V  0x1
#define EFUSE_DIS_PAD_JTAG_ERR_S  19
/* EFUSE_SOFT_DIS_JTAG_ERR : RO ;bitpos:[18:16] ;default: 3'h0 ; */
/*description: If SOFT_DIS_JTAG is 1  then it indicates a programming error.*/
#define EFUSE_SOFT_DIS_JTAG_ERR  0x00000007
#define EFUSE_SOFT_DIS_JTAG_ERR_M  ((EFUSE_SOFT_DIS_JTAG_ERR_V)<<(EFUSE_SOFT_DIS_JTAG_ERR_S))
#define EFUSE_SOFT_DIS_JTAG_ERR_V  0x7
#define EFUSE_SOFT_DIS_JTAG_ERR_S  16
/* EFUSE_JTAG_SEL_ENABLE_ERR : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: If JTAG_SEL_ENABLE is 1  then it indicates a programming error.*/
#define EFUSE_JTAG_SEL_ENABLE_ERR  (BIT(15))
#define EFUSE_JTAG_SEL_ENABLE_ERR_M  (BIT(15))
#define EFUSE_JTAG_SEL_ENABLE_ERR_V  0x1
#define EFUSE_JTAG_SEL_ENABLE_ERR_S  15
/* EFUSE_DIS_TWAI_ERR : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: If DIS_TWAI is 1  then it indicates a programming error.*/
#define EFUSE_DIS_TWAI_ERR  (BIT(14))
#define EFUSE_DIS_TWAI_ERR_M  (BIT(14))
#define EFUSE_DIS_TWAI_ERR_V  0x1
#define EFUSE_DIS_TWAI_ERR_S  14
/* EFUSE_RPT4_RESERVED6_ERR : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: Reserved..*/
#define EFUSE_RPT4_RESERVED6_ERR    (BIT(13))
#define EFUSE_RPT4_RESERVED6_ERR_M  (BIT(13))
#define EFUSE_RPT4_RESERVED6_ERR_V  0x1
#define EFUSE_RPT4_RESERVED6_ERR_S  13
/* EFUSE_DIS_FORCE_DOWNLOAD_ERR : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: If DIS_FORCE_DOWNLOAD is 1  then it indicates a programming error.*/
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR  (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR_M  (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR_V  0x1
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR_S  12
/* EFUSE_DIS_USB_DEVICE_ERR : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: If DIS_USB_DEVICE is 1  then it indicates a programming error.*/
#define EFUSE_DIS_USB_DEVICE_ERR  (BIT(11))
#define EFUSE_DIS_USB_DEVICE_ERR_M  (BIT(11))
#define EFUSE_DIS_USB_DEVICE_ERR_V  0x1
#define EFUSE_DIS_USB_DEVICE_ERR_S  11
/* EFUSE_DIS_DOWNLOAD_ICACHE_ERR : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: If DIS_DOWNLOAD_ICACHE is 1  then it indicates a programming error.*/
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR  (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_M  (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_V  0x1
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_S  10
/* EFUSE_DIS_USB_JTAG_ERR : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: If DIS_USB_JTAG is 1  then it indicates a programming error.*/
#define EFUSE_DIS_USB_JTAG_ERR  (BIT(9))
#define EFUSE_DIS_USB_JTAG_ERR_M  (BIT(9))
#define EFUSE_DIS_USB_JTAG_ERR_V  0x1
#define EFUSE_DIS_USB_JTAG_ERR_S  9
/* EFUSE_DIS_ICACHE_ERR : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: If DIS_ICACHE is 1  then it indicates a programming error.*/
#define EFUSE_DIS_ICACHE_ERR  (BIT(8))
#define EFUSE_DIS_ICACHE_ERR_M  (BIT(8))
#define EFUSE_DIS_ICACHE_ERR_V  0x1
#define EFUSE_DIS_ICACHE_ERR_S  8
/* EFUSE_RPT4_RESERVED5_ERR : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Reserved..*/
#define EFUSE_RPT4_RESERVED5_ERR  (BIT(7))
#define EFUSE_RPT4_RESERVED5_ERR_M  (BIT(7))
#define EFUSE_RPT4_RESERVED5_ERR_V  0x1
#define EFUSE_RPT4_RESERVED5_ERR_S  7
/* EFUSE_RD_DIS_ERR : RO ;bitpos:[6:0] ;default: 7'h0 ; */
/*description: If any bit in RD_DIS is 1  then it indicates a programming error.*/
#define EFUSE_RD_DIS_ERR  0x0000007F
#define EFUSE_RD_DIS_ERR_M  ((EFUSE_RD_DIS_ERR_V)<<(EFUSE_RD_DIS_ERR_S))
#define EFUSE_RD_DIS_ERR_V  0x7F
#define EFUSE_RD_DIS_ERR_S  0

#define EFUSE_RD_REPEAT_ERR1_REG          (DR_REG_EFUSE_BASE + 0x180)
/* EFUSE_KEY_PURPOSE_1_ERR : RO ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: If any bit in KEY_PURPOSE_1 is 1  then it indicates a programming error.*/
#define EFUSE_KEY_PURPOSE_1_ERR  0x0000000F
#define EFUSE_KEY_PURPOSE_1_ERR_M  ((EFUSE_KEY_PURPOSE_1_ERR_V)<<(EFUSE_KEY_PURPOSE_1_ERR_S))
#define EFUSE_KEY_PURPOSE_1_ERR_V  0xF
#define EFUSE_KEY_PURPOSE_1_ERR_S  28
/* EFUSE_KEY_PURPOSE_0_ERR : RO ;bitpos:[27:24] ;default: 4'h0 ; */
/*description: If any bit in KEY_PURPOSE_0 is 1  then it indicates a programming error.*/
#define EFUSE_KEY_PURPOSE_0_ERR  0x0000000F
#define EFUSE_KEY_PURPOSE_0_ERR_M  ((EFUSE_KEY_PURPOSE_0_ERR_V)<<(EFUSE_KEY_PURPOSE_0_ERR_S))
#define EFUSE_KEY_PURPOSE_0_ERR_V  0xF
#define EFUSE_KEY_PURPOSE_0_ERR_S  24
/* EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: If SECURE_BOOT_KEY_REVOKE2 is 1  then it indicates a programming error.*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR  (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_M  (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_S  23
/* EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: If SECURE_BOOT_KEY_REVOKE1 is 1  then it indicates a programming error.*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR  (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_M  (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_S  22
/* EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: If SECURE_BOOT_KEY_REVOKE0 is 1  then it indicates a programming error.*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR  (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_M  (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_S  21
/* EFUSE_SPI_BOOT_CRYPT_CNT_ERR : RO ;bitpos:[20:18] ;default: 3'h0 ; */
/*description: If any bit in SPI_BOOT_CRYPT_CNT is 1  then it indicates a programming error.*/
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR  0x00000007
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR_M  ((EFUSE_SPI_BOOT_CRYPT_CNT_ERR_V)<<(EFUSE_SPI_BOOT_CRYPT_CNT_ERR_S))
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR_V  0x7
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR_S  18
/* EFUSE_WDT_DELAY_SEL_ERR : RO ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: If any bit in WDT_DELAY_SEL is 1  then it indicates a programming error.*/
#define EFUSE_WDT_DELAY_SEL_ERR  0x00000003
#define EFUSE_WDT_DELAY_SEL_ERR_M  ((EFUSE_WDT_DELAY_SEL_ERR_V)<<(EFUSE_WDT_DELAY_SEL_ERR_S))
#define EFUSE_WDT_DELAY_SEL_ERR_V  0x3
#define EFUSE_WDT_DELAY_SEL_ERR_S  16
/* EFUSE_RPT4_RESERVED2_ERR : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED2_ERR  0x0000FFFF
#define EFUSE_RPT4_RESERVED2_ERR_M  ((EFUSE_RPT4_RESERVED2_ERR_V)<<(EFUSE_RPT4_RESERVED2_ERR_S))
#define EFUSE_RPT4_RESERVED2_ERR_V  0xFFFF
#define EFUSE_RPT4_RESERVED2_ERR_S  0

#define EFUSE_RD_REPEAT_ERR2_REG          (DR_REG_EFUSE_BASE + 0x184)
/* EFUSE_FLASH_TPUW_ERR : RO ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: If any bit in FLASH_TPUM is 1  then it indicates a programming error.*/
#define EFUSE_FLASH_TPUW_ERR  0x0000000F
#define EFUSE_FLASH_TPUW_ERR_M  ((EFUSE_FLASH_TPUW_ERR_V)<<(EFUSE_FLASH_TPUW_ERR_S))
#define EFUSE_FLASH_TPUW_ERR_V  0xF
#define EFUSE_FLASH_TPUW_ERR_S  28
/* EFUSE_RPT4_RESERVED0_ERR : RO ;bitpos:[27:22] ;default: 6'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED0_ERR  0x0000003F
#define EFUSE_RPT4_RESERVED0_ERR_M  ((EFUSE_RPT4_RESERVED0_ERR_V)<<(EFUSE_RPT4_RESERVED0_ERR_S))
#define EFUSE_RPT4_RESERVED0_ERR_V  0x3F
#define EFUSE_RPT4_RESERVED0_ERR_S  22
/* EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: If SECURE_BOOT_AGGRESSIVE_REVOKE is 1  then it indicates a programming error.*/
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR  (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_M  (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_V  0x1
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_S  21
/* EFUSE_SECURE_BOOT_EN_ERR : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: If SECURE_BOOT_EN is 1  then it indicates a programming error.*/
#define EFUSE_SECURE_BOOT_EN_ERR  (BIT(20))
#define EFUSE_SECURE_BOOT_EN_ERR_M  (BIT(20))
#define EFUSE_SECURE_BOOT_EN_ERR_V  0x1
#define EFUSE_SECURE_BOOT_EN_ERR_S  20
/* EFUSE_RPT4_RESERVED3_ERR : RO ;bitpos:[19:16] ;default: 4'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED3_ERR  0x0000000F
#define EFUSE_RPT4_RESERVED3_ERR_M  ((EFUSE_RPT4_RESERVED3_ERR_V)<<(EFUSE_RPT4_RESERVED3_ERR_S))
#define EFUSE_RPT4_RESERVED3_ERR_V  0xF
#define EFUSE_RPT4_RESERVED3_ERR_S  16
/* EFUSE_KEY_PURPOSE_5_ERR : RO ;bitpos:[15:12] ;default: 4'h0 ; */
/*description: If any bit in KEY_PURPOSE_5 is 1  then it indicates a programming error.*/
#define EFUSE_KEY_PURPOSE_5_ERR  0x0000000F
#define EFUSE_KEY_PURPOSE_5_ERR_M  ((EFUSE_KEY_PURPOSE_5_ERR_V)<<(EFUSE_KEY_PURPOSE_5_ERR_S))
#define EFUSE_KEY_PURPOSE_5_ERR_V  0xF
#define EFUSE_KEY_PURPOSE_5_ERR_S  12
/* EFUSE_KEY_PURPOSE_4_ERR : RO ;bitpos:[11:8] ;default: 4'h0 ; */
/*description: If any bit in KEY_PURPOSE_4 is 1  then it indicates a programming error.*/
#define EFUSE_KEY_PURPOSE_4_ERR  0x0000000F
#define EFUSE_KEY_PURPOSE_4_ERR_M  ((EFUSE_KEY_PURPOSE_4_ERR_V)<<(EFUSE_KEY_PURPOSE_4_ERR_S))
#define EFUSE_KEY_PURPOSE_4_ERR_V  0xF
#define EFUSE_KEY_PURPOSE_4_ERR_S  8
/* EFUSE_KEY_PURPOSE_3_ERR : RO ;bitpos:[7:4] ;default: 4'h0 ; */
/*description: If any bit in KEY_PURPOSE_3 is 1  then it indicates a programming error.*/
#define EFUSE_KEY_PURPOSE_3_ERR  0x0000000F
#define EFUSE_KEY_PURPOSE_3_ERR_M  ((EFUSE_KEY_PURPOSE_3_ERR_V)<<(EFUSE_KEY_PURPOSE_3_ERR_S))
#define EFUSE_KEY_PURPOSE_3_ERR_V  0xF
#define EFUSE_KEY_PURPOSE_3_ERR_S  4
/* EFUSE_KEY_PURPOSE_2_ERR : RO ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: If any bit in KEY_PURPOSE_2 is 1  then it indicates a programming error.*/
#define EFUSE_KEY_PURPOSE_2_ERR  0x0000000F
#define EFUSE_KEY_PURPOSE_2_ERR_M  ((EFUSE_KEY_PURPOSE_2_ERR_V)<<(EFUSE_KEY_PURPOSE_2_ERR_S))
#define EFUSE_KEY_PURPOSE_2_ERR_V  0xF
#define EFUSE_KEY_PURPOSE_2_ERR_S  0

#define EFUSE_RD_REPEAT_ERR3_REG          (DR_REG_EFUSE_BASE + 0x188)
/* EFUSE_ERR_RST_ENABLE_ERR : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: Use BLOCK0 to check error record registers, 0 - without check.*/
#define EFUSE_ERR_RST_ENABLE_ERR  (BIT(31))
#define EFUSE_ERR_RST_ENABLE_ERR_M  (BIT(31))
#define EFUSE_ERR_RST_ENABLE_ERR_V  0x1
#define EFUSE_ERR_RST_ENABLE_ERR_S  31
/* EFUSE_RPT4_RESERVED1_ERR : RO ;bitpos:[30] ;default: 1'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED1_ERR  (BIT(30))
#define EFUSE_RPT4_RESERVED1_ERR_M  (BIT(30))
#define EFUSE_RPT4_RESERVED1_ERR_V  0x1
#define EFUSE_RPT4_RESERVED1_ERR_S  30
/* EFUSE_SECURE_VERSION_ERR : RO ;bitpos:[29:14] ;default: 16'h0 ; */
/*description: If any bit in SECURE_VERSION is 1  then it indicates a programming error.*/
#define EFUSE_SECURE_VERSION_ERR  0x0000FFFF
#define EFUSE_SECURE_VERSION_ERR_M  ((EFUSE_SECURE_VERSION_ERR_V)<<(EFUSE_SECURE_VERSION_ERR_S))
#define EFUSE_SECURE_VERSION_ERR_V  0xFFFF
#define EFUSE_SECURE_VERSION_ERR_S  14
/* EFUSE_FORCE_SEND_RESUME_ERR : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: If FORCE_SEND_RESUME is 1  then it indicates a programming error.*/
#define EFUSE_FORCE_SEND_RESUME_ERR  (BIT(13))
#define EFUSE_FORCE_SEND_RESUME_ERR_M  (BIT(13))
#define EFUSE_FORCE_SEND_RESUME_ERR_V  0x1
#define EFUSE_FORCE_SEND_RESUME_ERR_S  13
/* EFUSE_RPT4_RESERVED7_ERR : RO ;bitpos:[12:8] ;default: 5'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED7_ERR  0x0000001F
#define EFUSE_RPT4_RESERVED7_ERR_M  ((EFUSE_RPT4_RESERVED7_ERR_V)<<(EFUSE_RPT4_RESERVED7_ERR_S))
#define EFUSE_RPT4_RESERVED7_ERR_V  0x1F
#define EFUSE_RPT4_RESERVED7_ERR_S  8
/* EFUSE_UART_PRINT_CONTROL_ERR : RO ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: If any bit in UART_PRINT_CONTROL is 1  then it indicates a programming error.*/
#define EFUSE_UART_PRINT_CONTROL_ERR  0x00000003
#define EFUSE_UART_PRINT_CONTROL_ERR_M  ((EFUSE_UART_PRINT_CONTROL_ERR_V)<<(EFUSE_UART_PRINT_CONTROL_ERR_S))
#define EFUSE_UART_PRINT_CONTROL_ERR_V  0x3
#define EFUSE_UART_PRINT_CONTROL_ERR_S  6
/* EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: If ENABLE_SECURITY_DOWNLOAD is 1  then it indicates a programming error.*/
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR  (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_M  (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_V  0x1
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_S  5
/* EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: If DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE is 1  then it indicates a programming error.*/
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR  (BIT(4))
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR_M  (BIT(4))
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR_V  0x1
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR_S  4
/* EFUSE_RPT4_RESERVED8_ERR : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED8_ERR  (BIT(3))
#define EFUSE_RPT4_RESERVED8_ERR_M  (BIT(3))
#define EFUSE_RPT4_RESERVED8_ERR_V  0x1
#define EFUSE_RPT4_RESERVED8_ERR_S  3
/* EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: If DIS_USB_SERIAL_JTAG_ROM_PRINT is 1  then it indicates a programming error.*/
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR  (BIT(2))
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR_M  (BIT(2))
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR_V  0x1
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR_S  2
/* EFUSE_DIS_DIRECT_BOOT_ERR : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: If DIS_DIRECT_BOOT is 1  then it indicates a programming error.*/
#define EFUSE_DIS_DIRECT_BOOT_ERR  (BIT(1))
#define EFUSE_DIS_DIRECT_BOOT_ERR_M  (BIT(1))
#define EFUSE_DIS_DIRECT_BOOT_ERR_V  0x1
#define EFUSE_DIS_DIRECT_BOOT_ERR_S  1
/* EFUSE_DIS_DOWNLOAD_MODE_ERR : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: If DIS_DOWNLOAD_MODE is 1  then it indicates a programming error.*/
#define EFUSE_DIS_DOWNLOAD_MODE_ERR  (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_M  (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_V  0x1
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_S  0

#define EFUSE_RD_REPEAT_ERR4_REG          (DR_REG_EFUSE_BASE + 0x18C)
/* EFUSE_RPT4_RESERVED4_ERR : RO ;bitpos:[23:0] ;default: 24'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED4_ERR  0x00FFFFFF
#define EFUSE_RPT4_RESERVED4_ERR_M  ((EFUSE_RPT4_RESERVED4_ERR_V)<<(EFUSE_RPT4_RESERVED4_ERR_S))
#define EFUSE_RPT4_RESERVED4_ERR_V  0xFFFFFF
#define EFUSE_RPT4_RESERVED4_ERR_S  0

#define EFUSE_RD_RS_ERR0_REG          (DR_REG_EFUSE_BASE + 0x1C0)
/* EFUSE_KEY3_FAIL : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of key3 is reliable 1:
 Means that programming key3 failed and the number of error bytes is over 6.*/
#define EFUSE_KEY3_FAIL  (BIT(31))
#define EFUSE_KEY3_FAIL_M  (BIT(31))
#define EFUSE_KEY3_FAIL_V  0x1
#define EFUSE_KEY3_FAIL_S  31
/* EFUSE_KEY4_ERR_NUM : RO ;bitpos:[30:28] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_KEY4_ERR_NUM  0x00000007
#define EFUSE_KEY4_ERR_NUM_M  ((EFUSE_KEY4_ERR_NUM_V)<<(EFUSE_KEY4_ERR_NUM_S))
#define EFUSE_KEY4_ERR_NUM_V  0x7
#define EFUSE_KEY4_ERR_NUM_S  28
/* EFUSE_KEY2_FAIL : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of key2 is reliable 1:
 Means that programming key2 failed and the number of error bytes is over 6.*/
#define EFUSE_KEY2_FAIL  (BIT(27))
#define EFUSE_KEY2_FAIL_M  (BIT(27))
#define EFUSE_KEY2_FAIL_V  0x1
#define EFUSE_KEY2_FAIL_S  27
/* EFUSE_KEY3_ERR_NUM : RO ;bitpos:[26:24] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_KEY3_ERR_NUM  0x00000007
#define EFUSE_KEY3_ERR_NUM_M  ((EFUSE_KEY3_ERR_NUM_V)<<(EFUSE_KEY3_ERR_NUM_S))
#define EFUSE_KEY3_ERR_NUM_V  0x7
#define EFUSE_KEY3_ERR_NUM_S  24
/* EFUSE_KEY1_FAIL : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of key1 is reliable 1:
 Means that programming key1 failed and the number of error bytes is over 6.*/
#define EFUSE_KEY1_FAIL  (BIT(23))
#define EFUSE_KEY1_FAIL_M  (BIT(23))
#define EFUSE_KEY1_FAIL_V  0x1
#define EFUSE_KEY1_FAIL_S  23
/* EFUSE_KEY2_ERR_NUM : RO ;bitpos:[22:20] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_KEY2_ERR_NUM  0x00000007
#define EFUSE_KEY2_ERR_NUM_M  ((EFUSE_KEY2_ERR_NUM_V)<<(EFUSE_KEY2_ERR_NUM_S))
#define EFUSE_KEY2_ERR_NUM_V  0x7
#define EFUSE_KEY2_ERR_NUM_S  20
/* EFUSE_KEY0_FAIL : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of key0 is reliable 1:
 Means that programming key0 failed and the number of error bytes is over 6.*/
#define EFUSE_KEY0_FAIL  (BIT(19))
#define EFUSE_KEY0_FAIL_M  (BIT(19))
#define EFUSE_KEY0_FAIL_V  0x1
#define EFUSE_KEY0_FAIL_S  19
/* EFUSE_KEY1_ERR_NUM : RO ;bitpos:[18:16] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_KEY1_ERR_NUM  0x00000007
#define EFUSE_KEY1_ERR_NUM_M  ((EFUSE_KEY1_ERR_NUM_V)<<(EFUSE_KEY1_ERR_NUM_S))
#define EFUSE_KEY1_ERR_NUM_V  0x7
#define EFUSE_KEY1_ERR_NUM_S  16
/* EFUSE_USR_DATA_FAIL : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of user data is reliable
 1: Means that programming user data failed and the number of error bytes is over 6.*/
#define EFUSE_USR_DATA_FAIL  (BIT(15))
#define EFUSE_USR_DATA_FAIL_M  (BIT(15))
#define EFUSE_USR_DATA_FAIL_V  0x1
#define EFUSE_USR_DATA_FAIL_S  15
/* EFUSE_KEY0_ERR_NUM : RO ;bitpos:[14:12] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_KEY0_ERR_NUM  0x00000007
#define EFUSE_KEY0_ERR_NUM_M  ((EFUSE_KEY0_ERR_NUM_V)<<(EFUSE_KEY0_ERR_NUM_S))
#define EFUSE_KEY0_ERR_NUM_V  0x7
#define EFUSE_KEY0_ERR_NUM_S  12
/* EFUSE_SYS_PART1_FAIL : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of system part1 is reliable
 1: Means that programming data of system part1 failed and the number of error bytes is over 6.*/
#define EFUSE_SYS_PART1_FAIL  (BIT(11))
#define EFUSE_SYS_PART1_FAIL_M  (BIT(11))
#define EFUSE_SYS_PART1_FAIL_V  0x1
#define EFUSE_SYS_PART1_FAIL_S  11
/* EFUSE_USR_DATA_ERR_NUM : RO ;bitpos:[10:8] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_USR_DATA_ERR_NUM  0x00000007
#define EFUSE_USR_DATA_ERR_NUM_M  ((EFUSE_USR_DATA_ERR_NUM_V)<<(EFUSE_USR_DATA_ERR_NUM_S))
#define EFUSE_USR_DATA_ERR_NUM_V  0x7
#define EFUSE_USR_DATA_ERR_NUM_S  8
/* EFUSE_MAC_SPI_8M_FAIL : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of MAC_SPI_8M is reliable
 1: Means that programming MAC_SPI_8M failed and the number of error bytes is over 6.*/
#define EFUSE_MAC_SPI_8M_FAIL  (BIT(7))
#define EFUSE_MAC_SPI_8M_FAIL_M  (BIT(7))
#define EFUSE_MAC_SPI_8M_FAIL_V  0x1
#define EFUSE_MAC_SPI_8M_FAIL_S  7
/* EFUSE_SYS_PART1_NUM : RO ;bitpos:[6:4] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_SYS_PART1_NUM  0x00000007
#define EFUSE_SYS_PART1_NUM_M  ((EFUSE_SYS_PART1_NUM_V)<<(EFUSE_SYS_PART1_NUM_S))
#define EFUSE_SYS_PART1_NUM_V  0x7
#define EFUSE_SYS_PART1_NUM_S  4
/* EFUSE_MAC_SPI_8M_ERR_NUM : RO ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_MAC_SPI_8M_ERR_NUM  0x00000007
#define EFUSE_MAC_SPI_8M_ERR_NUM_M  ((EFUSE_MAC_SPI_8M_ERR_NUM_V)<<(EFUSE_MAC_SPI_8M_ERR_NUM_S))
#define EFUSE_MAC_SPI_8M_ERR_NUM_V  0x7
#define EFUSE_MAC_SPI_8M_ERR_NUM_S  0

#define EFUSE_RD_RS_ERR1_REG          (DR_REG_EFUSE_BASE + 0x1C4)
/* EFUSE_KEY5_FAIL : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of KEY5 is reliable 1:
 Means that programming KEY5 failed and the number of error bytes is over 6.*/
#define EFUSE_KEY5_FAIL  (BIT(7))
#define EFUSE_KEY5_FAIL_M  (BIT(7))
#define EFUSE_KEY5_FAIL_V  0x1
#define EFUSE_KEY5_FAIL_S  7
/* EFUSE_SYS_PART2_ERR_NUM : RO ;bitpos:[6:4] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_SYS_PART2_ERR_NUM  0x00000007
#define EFUSE_SYS_PART2_ERR_NUM_M  ((EFUSE_SYS_PART2_ERR_NUM_V)<<(EFUSE_SYS_PART2_ERR_NUM_S))
#define EFUSE_SYS_PART2_ERR_NUM_V  0x7
#define EFUSE_SYS_PART2_ERR_NUM_S  4
/* EFUSE_KEY4_FAIL : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of KEY4 is reliable 1:
 Means that programming KEY4 failed and the number of error bytes is over 6.*/
#define EFUSE_KEY4_FAIL  (BIT(3))
#define EFUSE_KEY4_FAIL_M  (BIT(3))
#define EFUSE_KEY4_FAIL_V  0x1
#define EFUSE_KEY4_FAIL_S  3
/* EFUSE_KEY5_ERR_NUM : RO ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_KEY5_ERR_NUM  0x00000007
#define EFUSE_KEY5_ERR_NUM_M  ((EFUSE_KEY5_ERR_NUM_V)<<(EFUSE_KEY5_ERR_NUM_S))
#define EFUSE_KEY5_ERR_NUM_V  0x7
#define EFUSE_KEY5_ERR_NUM_S  0

#define EFUSE_CLK_REG          (DR_REG_EFUSE_BASE + 0x1C8)
/* EFUSE_CLK_EN : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: Set this bit and force to enable clock signal of eFuse memory.*/
#define EFUSE_CLK_EN  (BIT(16))
#define EFUSE_CLK_EN_M  (BIT(16))
#define EFUSE_CLK_EN_V  0x1
#define EFUSE_CLK_EN_S  16
/* EFUSE_MEM_FORCE_PU : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to force eFuse SRAM into working mode.*/
#define EFUSE_MEM_FORCE_PU  (BIT(2))
#define EFUSE_MEM_FORCE_PU_M  (BIT(2))
#define EFUSE_MEM_FORCE_PU_V  0x1
#define EFUSE_MEM_FORCE_PU_S  2
/* EFUSE_MEM_CLK_FORCE_ON : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: Set this bit and force to activate clock signal of eFuse SRAM.*/
#define EFUSE_MEM_CLK_FORCE_ON  (BIT(1))
#define EFUSE_MEM_CLK_FORCE_ON_M  (BIT(1))
#define EFUSE_MEM_CLK_FORCE_ON_V  0x1
#define EFUSE_MEM_CLK_FORCE_ON_S  1
/* EFUSE_MEM_FORCE_PD : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to force eFuse SRAM into power-saving mode.*/
#define EFUSE_MEM_FORCE_PD  (BIT(0))
#define EFUSE_MEM_FORCE_PD_M  (BIT(0))
#define EFUSE_MEM_FORCE_PD_V  0x1
#define EFUSE_MEM_FORCE_PD_S  0

#define EFUSE_CONF_REG          (DR_REG_EFUSE_BASE + 0x1CC)
/* EFUSE_OP_CODE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: 0x5A5A: Operate programming command 0x5AA5: Operate read command.*/
#define EFUSE_OP_CODE  0x0000FFFF
#define EFUSE_OP_CODE_M  ((EFUSE_OP_CODE_V)<<(EFUSE_OP_CODE_S))
#define EFUSE_OP_CODE_V  0xFFFF
#define EFUSE_OP_CODE_S  0

#define EFUSE_WRITE_OP_CODE 0x5a5a
#define EFUSE_READ_OP_CODE  0x5aa5

#define EFUSE_STATUS_REG          (DR_REG_EFUSE_BASE + 0x1D0)
/* EFUSE_REPEAT_ERR_CNT : RO ;bitpos:[17:10] ;default: 8'h0 ; */
/*description: Indicates the number of error bits during programming BLOCK0.*/
#define EFUSE_REPEAT_ERR_CNT  0x000000FF
#define EFUSE_REPEAT_ERR_CNT_M  ((EFUSE_REPEAT_ERR_CNT_V)<<(EFUSE_REPEAT_ERR_CNT_S))
#define EFUSE_REPEAT_ERR_CNT_V  0xFF
#define EFUSE_REPEAT_ERR_CNT_S  10
/* EFUSE_OTP_VDDQ_IS_SW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The value of OTP_VDDQ_IS_SW.*/
#define EFUSE_OTP_VDDQ_IS_SW  (BIT(9))
#define EFUSE_OTP_VDDQ_IS_SW_M  (BIT(9))
#define EFUSE_OTP_VDDQ_IS_SW_V  0x1
#define EFUSE_OTP_VDDQ_IS_SW_S  9
/* EFUSE_OTP_PGENB_SW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The value of OTP_PGENB_SW.*/
#define EFUSE_OTP_PGENB_SW  (BIT(8))
#define EFUSE_OTP_PGENB_SW_M  (BIT(8))
#define EFUSE_OTP_PGENB_SW_V  0x1
#define EFUSE_OTP_PGENB_SW_S  8
/* EFUSE_OTP_CSB_SW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The value of OTP_CSB_SW.*/
#define EFUSE_OTP_CSB_SW  (BIT(7))
#define EFUSE_OTP_CSB_SW_M  (BIT(7))
#define EFUSE_OTP_CSB_SW_V  0x1
#define EFUSE_OTP_CSB_SW_S  7
/* EFUSE_OTP_STROBE_SW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The value of OTP_STROBE_SW.*/
#define EFUSE_OTP_STROBE_SW  (BIT(6))
#define EFUSE_OTP_STROBE_SW_M  (BIT(6))
#define EFUSE_OTP_STROBE_SW_V  0x1
#define EFUSE_OTP_STROBE_SW_S  6
/* EFUSE_OTP_VDDQ_C_SYNC2 : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The value of OTP_VDDQ_C_SYNC2.*/
#define EFUSE_OTP_VDDQ_C_SYNC2  (BIT(5))
#define EFUSE_OTP_VDDQ_C_SYNC2_M  (BIT(5))
#define EFUSE_OTP_VDDQ_C_SYNC2_V  0x1
#define EFUSE_OTP_VDDQ_C_SYNC2_S  5
/* EFUSE_OTP_LOAD_SW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The value of OTP_LOAD_SW.*/
#define EFUSE_OTP_LOAD_SW  (BIT(4))
#define EFUSE_OTP_LOAD_SW_M  (BIT(4))
#define EFUSE_OTP_LOAD_SW_V  0x1
#define EFUSE_OTP_LOAD_SW_S  4
/* EFUSE_STATE : RO ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: Indicates the state of the eFuse state machine.*/
#define EFUSE_STATE  0x0000000F
#define EFUSE_STATE_M  ((EFUSE_STATE_V)<<(EFUSE_STATE_S))
#define EFUSE_STATE_V  0xF
#define EFUSE_STATE_S  0

#define EFUSE_CMD_REG          (DR_REG_EFUSE_BASE + 0x1D4)
/* EFUSE_BLK_NUM : R/W ;bitpos:[5:2] ;default: 4'h0 ; */
/*description: The serial number of the block to be programmed. Value 0-10 corresponds
 to block number 0-10  respectively.*/
#define EFUSE_BLK_NUM  0x0000000F
#define EFUSE_BLK_NUM_M  ((EFUSE_BLK_NUM_V)<<(EFUSE_BLK_NUM_S))
#define EFUSE_BLK_NUM_V  0xF
#define EFUSE_BLK_NUM_S  2
/* EFUSE_PGM_CMD : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to send programming command.*/
#define EFUSE_PGM_CMD  (BIT(1))
#define EFUSE_PGM_CMD_M  (BIT(1))
#define EFUSE_PGM_CMD_V  0x1
#define EFUSE_PGM_CMD_S  1
/* EFUSE_READ_CMD : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to send read command.*/
#define EFUSE_READ_CMD  (BIT(0))
#define EFUSE_READ_CMD_M  (BIT(0))
#define EFUSE_READ_CMD_V  0x1
#define EFUSE_READ_CMD_S  0

#define EFUSE_INT_RAW_REG          (DR_REG_EFUSE_BASE + 0x1D8)
/* EFUSE_PGM_DONE_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw bit signal for pgm_done interrupt.*/
#define EFUSE_PGM_DONE_INT_RAW  (BIT(1))
#define EFUSE_PGM_DONE_INT_RAW_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_RAW_V  0x1
#define EFUSE_PGM_DONE_INT_RAW_S  1
/* EFUSE_READ_DONE_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw bit signal for read_done interrupt.*/
#define EFUSE_READ_DONE_INT_RAW  (BIT(0))
#define EFUSE_READ_DONE_INT_RAW_M  (BIT(0))
#define EFUSE_READ_DONE_INT_RAW_V  0x1
#define EFUSE_READ_DONE_INT_RAW_S  0

#define EFUSE_INT_ST_REG          (DR_REG_EFUSE_BASE + 0x1DC)
/* EFUSE_PGM_DONE_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The status signal for pgm_done interrupt.*/
#define EFUSE_PGM_DONE_INT_ST  (BIT(1))
#define EFUSE_PGM_DONE_INT_ST_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_ST_V  0x1
#define EFUSE_PGM_DONE_INT_ST_S  1
/* EFUSE_READ_DONE_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The status signal for read_done interrupt.*/
#define EFUSE_READ_DONE_INT_ST  (BIT(0))
#define EFUSE_READ_DONE_INT_ST_M  (BIT(0))
#define EFUSE_READ_DONE_INT_ST_V  0x1
#define EFUSE_READ_DONE_INT_ST_S  0

#define EFUSE_INT_ENA_REG          (DR_REG_EFUSE_BASE + 0x1E0)
/* EFUSE_PGM_DONE_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The enable signal for pgm_done interrupt.*/
#define EFUSE_PGM_DONE_INT_ENA  (BIT(1))
#define EFUSE_PGM_DONE_INT_ENA_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_ENA_V  0x1
#define EFUSE_PGM_DONE_INT_ENA_S  1
/* EFUSE_READ_DONE_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The enable signal for read_done interrupt.*/
#define EFUSE_READ_DONE_INT_ENA  (BIT(0))
#define EFUSE_READ_DONE_INT_ENA_M  (BIT(0))
#define EFUSE_READ_DONE_INT_ENA_V  0x1
#define EFUSE_READ_DONE_INT_ENA_S  0

#define EFUSE_INT_CLR_REG          (DR_REG_EFUSE_BASE + 0x1E4)
/* EFUSE_PGM_DONE_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The clear signal for pgm_done interrupt.*/
#define EFUSE_PGM_DONE_INT_CLR  (BIT(1))
#define EFUSE_PGM_DONE_INT_CLR_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_CLR_V  0x1
#define EFUSE_PGM_DONE_INT_CLR_S  1
/* EFUSE_READ_DONE_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The clear signal for read_done interrupt.*/
#define EFUSE_READ_DONE_INT_CLR  (BIT(0))
#define EFUSE_READ_DONE_INT_CLR_M  (BIT(0))
#define EFUSE_READ_DONE_INT_CLR_V  0x1
#define EFUSE_READ_DONE_INT_CLR_S  0

#define EFUSE_DAC_CONF_REG          (DR_REG_EFUSE_BASE + 0x1E8)
/* EFUSE_OE_CLR : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: Reduces the power supply of the programming voltage.*/
#define EFUSE_OE_CLR  (BIT(17))
#define EFUSE_OE_CLR_M  (BIT(17))
#define EFUSE_OE_CLR_V  0x1
#define EFUSE_OE_CLR_S  17
/* EFUSE_DAC_NUM : R/W ;bitpos:[16:9] ;default: 8'd255 ; */
/*description: Controls the rising period of the programming voltage.*/
#define EFUSE_DAC_NUM  0x000000FF
#define EFUSE_DAC_NUM_M  ((EFUSE_DAC_NUM_V)<<(EFUSE_DAC_NUM_S))
#define EFUSE_DAC_NUM_V  0xFF
#define EFUSE_DAC_NUM_S  9
/* EFUSE_DAC_CLK_PAD_SEL : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Don't care.*/
#define EFUSE_DAC_CLK_PAD_SEL  (BIT(8))
#define EFUSE_DAC_CLK_PAD_SEL_M  (BIT(8))
#define EFUSE_DAC_CLK_PAD_SEL_V  0x1
#define EFUSE_DAC_CLK_PAD_SEL_S  8
/* EFUSE_DAC_CLK_DIV : R/W ;bitpos:[7:0] ;default: 8'd28 ; */
/*description: Controls the division factor of the rising clock of the programming voltage.*/
#define EFUSE_DAC_CLK_DIV  0x000000FF
#define EFUSE_DAC_CLK_DIV_M  ((EFUSE_DAC_CLK_DIV_V)<<(EFUSE_DAC_CLK_DIV_S))
#define EFUSE_DAC_CLK_DIV_V  0xFF
#define EFUSE_DAC_CLK_DIV_S  0

#define EFUSE_RD_TIM_CONF_REG          (DR_REG_EFUSE_BASE + 0x1EC)
/* EFUSE_READ_INIT_NUM : R/W ;bitpos:[31:24] ;default: 8'h12 ; */
/*description: Configures the initial read time of eFuse.*/
#define EFUSE_READ_INIT_NUM  0x000000FF
#define EFUSE_READ_INIT_NUM_M  ((EFUSE_READ_INIT_NUM_V)<<(EFUSE_READ_INIT_NUM_S))
#define EFUSE_READ_INIT_NUM_V  0xFF
#define EFUSE_READ_INIT_NUM_S  24

#define EFUSE_WR_TIM_CONF1_REG          (DR_REG_EFUSE_BASE + 0x1F0)
/* EFUSE_PWR_ON_NUM : R/W ;bitpos:[23:8] ;default: 16'h2880 ; */
/*description: Configures the power up time for VDDQ.*/
#define EFUSE_PWR_ON_NUM  0x0000FFFF
#define EFUSE_PWR_ON_NUM_M  ((EFUSE_PWR_ON_NUM_V)<<(EFUSE_PWR_ON_NUM_S))
#define EFUSE_PWR_ON_NUM_V  0xFFFF
#define EFUSE_PWR_ON_NUM_S  8

#define EFUSE_WR_TIM_CONF2_REG          (DR_REG_EFUSE_BASE + 0x1F4)
/* EFUSE_PWR_OFF_NUM : R/W ;bitpos:[15:0] ;default: 16'h190 ; */
/*description: Configures the power outage time for VDDQ.*/
#define EFUSE_PWR_OFF_NUM  0x0000FFFF
#define EFUSE_PWR_OFF_NUM_M  ((EFUSE_PWR_OFF_NUM_V)<<(EFUSE_PWR_OFF_NUM_S))
#define EFUSE_PWR_OFF_NUM_V  0xFFFF
#define EFUSE_PWR_OFF_NUM_S  0

#define EFUSE_DATE_REG          (DR_REG_EFUSE_BASE + 0x1FC)
/* EFUSE_DATE : R/W ;bitpos:[27:0] ;default: 28'h2007200 ; */
/*description: Stores eFuse version.*/
#define EFUSE_DATE  0x0FFFFFFF
#define EFUSE_DATE_M  ((EFUSE_DATE_V)<<(EFUSE_DATE_S))
#define EFUSE_DATE_V  0xFFFFFFF
#define EFUSE_DATE_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_EFUSE_REG_H_ */
