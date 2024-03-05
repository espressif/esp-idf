/**
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define EFUSE_WRITE_OP_CODE 0x5a5a
#define EFUSE_READ_OP_CODE 0x5aa5

#define EFUSE_PKG_VERSION_ESP32S3       0 // QFN56
#define EFUSE_PKG_VERSION_ESP32S3PICO   1 // LGA56

/** EFUSE_RD_MAC_SPI_SYS_3_REG register
 *  BLOCK1 data register 3.
 */
//#define EFUSE_RD_MAC_SPI_SYS_3_REG (DR_REG_EFUSE_BASE + 0x50)
/* EFUSE_SPI_PAD_CONF_2 : RO ;bitpos:[17:0] ;default: 18'h0 ; */
/*description: Stores the second part of SPI_PAD_CONF..*/
#define EFUSE_SPI_PAD_CONF_2    0x0003FFFF
#define EFUSE_SPI_PAD_CONF_2_M  ((EFUSE_SPI_PAD_CONF_2_V)<<(EFUSE_SPI_PAD_CONF_2_S))
#define EFUSE_SPI_PAD_CONF_2_V  0x3FFFF
#define EFUSE_SPI_PAD_CONF_2_S  0

#ifdef __cplusplus
}
#endif
