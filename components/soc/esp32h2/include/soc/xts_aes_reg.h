/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

#define XTS_AES_PLAIN_MEM(i)          (REG_SPI_MEM_BASE(i) + 0x300)
/* XTS_AES_PLAIN : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This field is only used to generate include file in c case. This field is useles
s. Please do not use this field..*/
#define XTS_AES_PLAIN    0xFFFFFFFF
#define XTS_AES_PLAIN_M  ((XTS_AES_PLAIN_V)<<(XTS_AES_PLAIN_S))
#define XTS_AES_PLAIN_V  0xFFFFFFFF
#define XTS_AES_PLAIN_S  0

#define XTS_AES_LINESIZE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x340)
/* XTS_AES_LINESIZE : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: This bits stores the line-size parameter which will be used in manual encryption
 calculation. It decides how many bytes will be encrypted one time. 0: 16-bytes,
 1: 32-bytes, 2: 64-bytes, 3:reserved..*/
#define XTS_AES_LINESIZE    0x00000003
#define XTS_AES_LINESIZE_M  ((XTS_AES_LINESIZE_V)<<(XTS_AES_LINESIZE_S))
#define XTS_AES_LINESIZE_V  0x3
#define XTS_AES_LINESIZE_S  0

#define XTS_AES_DESTINATION_REG(i)          (REG_SPI_MEM_BASE(i) + 0x344)
/* XTS_AES_DESTINATION : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: This bit stores the destination parameter which will be used in manual encryptio
n calculation. 0: flash(default), 1: psram(reserved). Only default value can be
used..*/
#define XTS_AES_DESTINATION    (BIT(0))
#define XTS_AES_DESTINATION_M  (BIT(0))
#define XTS_AES_DESTINATION_V  0x1
#define XTS_AES_DESTINATION_S  0

#define XTS_AES_PHYSICAL_ADDRESS_REG(i)          (REG_SPI_MEM_BASE(i) + 0x348)
/* XTS_AES_PHYSICAL_ADDRESS : R/W ;bitpos:[25:0] ;default: 26'h0 ; */
/*description: This bits stores the physical-address parameter which will be used in manual enc
ryption calculation. This value should aligned with byte number decided by line-
size parameter..*/
#define XTS_AES_PHYSICAL_ADDRESS    0x03FFFFFF
#define XTS_AES_PHYSICAL_ADDRESS_M  ((XTS_AES_PHYSICAL_ADDRESS_V)<<(XTS_AES_PHYSICAL_ADDRESS_S))
#define XTS_AES_PHYSICAL_ADDRESS_V  0x3FFFFFF
#define XTS_AES_PHYSICAL_ADDRESS_S  0

#define XTS_AES_TRIGGER_REG(i)          (REG_SPI_MEM_BASE(i) + 0x34C)
/* XTS_AES_TRIGGER : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to trigger the process of manual encryption calculation. This actio
n should only be asserted when manual encryption status is 0. After this action,
 manual encryption status becomes 1. After calculation is done, manual encryptio
n status becomes 2..*/
#define XTS_AES_TRIGGER    (BIT(0))
#define XTS_AES_TRIGGER_M  (BIT(0))
#define XTS_AES_TRIGGER_V  0x1
#define XTS_AES_TRIGGER_S  0

#define XTS_AES_RELEASE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x350)
/* XTS_AES_RELEASE : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to release encrypted result to mspi. This action should only be ass
erted when manual encryption status is 2. After this action, manual encryption s
tatus will become 3..*/
#define XTS_AES_RELEASE    (BIT(0))
#define XTS_AES_RELEASE_M  (BIT(0))
#define XTS_AES_RELEASE_V  0x1
#define XTS_AES_RELEASE_S  0

#define XTS_AES_DESTROY_REG(i)          (REG_SPI_MEM_BASE(i) + 0x354)
/* XTS_AES_DESTROY : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to destroy encrypted result. This action should be asserted only wh
en manual encryption status is 3. After this action, manual encryption status wi
ll become 0..*/
#define XTS_AES_DESTROY    (BIT(0))
#define XTS_AES_DESTROY_M  (BIT(0))
#define XTS_AES_DESTROY_V  0x1
#define XTS_AES_DESTROY_S  0

#define XTS_AES_STATE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x358)
/* XTS_AES_STATE : RO ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: This bits stores the status of manual encryption. 0: idle, 1: busy of encryption
 calculation, 2: encryption calculation is done but the encrypted result is invi
sible to mspi, 3: the encrypted result is visible to mspi..*/
#define XTS_AES_STATE    0x00000003
#define XTS_AES_STATE_M  ((XTS_AES_STATE_V)<<(XTS_AES_STATE_S))
#define XTS_AES_STATE_V  0x3
#define XTS_AES_STATE_S  0

#define XTS_AES_DATE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x35C)
/* XTS_AES_DATE : R/W ;bitpos:[29:0] ;default: 30'h20201010 ; */
/*description: This bits stores the last modified-time of manual encryption feature..*/
#define XTS_AES_DATE    0x3FFFFFFF
#define XTS_AES_DATE_M  ((XTS_AES_DATE_V)<<(XTS_AES_DATE_S))
#define XTS_AES_DATE_V  0x3FFFFFFF
#define XTS_AES_DATE_S  0

#define XTS_AES_DPA_CTRL_REG(i)          (REG_SPI_MEM_BASE(i) + 0x388)
/* XTS_AES_CRYPT_DPA_SELECT_REGISTER : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: 1: MSPI XTS DPA clock gate is controlled by SPI_CRYPT_CALC_D_DPA_EN and SPI_CRYP
T_SECURITY_LEVEL. 0: Controlled by efuse bits..*/
#define XTS_AES_CRYPT_DPA_SELECT_REGISTER    (BIT(4))
#define XTS_AES_CRYPT_DPA_SELECT_REGISTER_M  (BIT(4))
#define XTS_AES_CRYPT_DPA_SELECT_REGISTER_V  0x1
#define XTS_AES_CRYPT_DPA_SELECT_REGISTER_S  4
/* XTS_AES_CRYPT_CALC_D_DPA_EN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: Only available when SPI_CRYPT_SECURITY_LEVEL is not 0. 1: Enable DPA in the calc
ulation that using key 1 or key 2. 0: Enable DPA only in the calculation that us
ing key 1..*/
#define XTS_AES_CRYPT_CALC_D_DPA_EN    (BIT(3))
#define XTS_AES_CRYPT_CALC_D_DPA_EN_M  (BIT(3))
#define XTS_AES_CRYPT_CALC_D_DPA_EN_V  0x1
#define XTS_AES_CRYPT_CALC_D_DPA_EN_S  3
/* XTS_AES_CRYPT_SECURITY_LEVEL : R/W ;bitpos:[2:0] ;default: 3'd7 ; */
/*description: Set the security level of spi mem cryption. 0: Shut off cryption DPA funtion. 1-
7: The bigger the number is, the more secure the cryption is. (Note that the per
formance of cryption will decrease together with this number increasing).*/
#define XTS_AES_CRYPT_SECURITY_LEVEL    0x00000007
#define XTS_AES_CRYPT_SECURITY_LEVEL_M  ((XTS_AES_CRYPT_SECURITY_LEVEL_V)<<(XTS_AES_CRYPT_SECURITY_LEVEL_S))
#define XTS_AES_CRYPT_SECURITY_LEVEL_V  0x7
#define XTS_AES_CRYPT_SECURITY_LEVEL_S  0

#ifdef __cplusplus
}
#endif
