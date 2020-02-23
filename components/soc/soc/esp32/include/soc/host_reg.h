// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _SOC_HOST_REG_H_
#define _SOC_HOST_REG_H_


#include "soc.h"
#define HOST_SLCHOST_FUNC2_0_REG          (DR_REG_SLCHOST_BASE + 0x10)
/* HOST_SLC_FUNC2_INT : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC_FUNC2_INT  (BIT(24))
#define HOST_SLC_FUNC2_INT_M  (BIT(24))
#define HOST_SLC_FUNC2_INT_V  0x1
#define HOST_SLC_FUNC2_INT_S  24

#define HOST_SLCHOST_FUNC2_1_REG          (DR_REG_SLCHOST_BASE + 0x14)
/* HOST_SLC_FUNC2_INT_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC_FUNC2_INT_EN  (BIT(0))
#define HOST_SLC_FUNC2_INT_EN_M  (BIT(0))
#define HOST_SLC_FUNC2_INT_EN_V  0x1
#define HOST_SLC_FUNC2_INT_EN_S  0

#define HOST_SLCHOST_FUNC2_2_REG          (DR_REG_SLCHOST_BASE + 0x20)
/* HOST_SLC_FUNC1_MDSTAT : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define HOST_SLC_FUNC1_MDSTAT  (BIT(0))
#define HOST_SLC_FUNC1_MDSTAT_M  (BIT(0))
#define HOST_SLC_FUNC1_MDSTAT_V  0x1
#define HOST_SLC_FUNC1_MDSTAT_S  0

#define HOST_SLCHOST_GPIO_STATUS0_REG          (DR_REG_SLCHOST_BASE + 0x34)
/* HOST_GPIO_SDIO_INT0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define HOST_GPIO_SDIO_INT0  0xFFFFFFFF
#define HOST_GPIO_SDIO_INT0_M  ((HOST_GPIO_SDIO_INT0_V)<<(HOST_GPIO_SDIO_INT0_S))
#define HOST_GPIO_SDIO_INT0_V  0xFFFFFFFF
#define HOST_GPIO_SDIO_INT0_S  0

#define HOST_SLCHOST_GPIO_STATUS1_REG          (DR_REG_SLCHOST_BASE + 0x38)
/* HOST_GPIO_SDIO_INT1 : RO ;bitpos:[7:0] ;default: 8'b0 ; */
/*description: */
#define HOST_GPIO_SDIO_INT1  0x000000FF
#define HOST_GPIO_SDIO_INT1_M  ((HOST_GPIO_SDIO_INT1_V)<<(HOST_GPIO_SDIO_INT1_S))
#define HOST_GPIO_SDIO_INT1_V  0xFF
#define HOST_GPIO_SDIO_INT1_S  0

#define HOST_SLCHOST_GPIO_IN0_REG          (DR_REG_SLCHOST_BASE + 0x3C)
/* HOST_GPIO_SDIO_IN0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define HOST_GPIO_SDIO_IN0  0xFFFFFFFF
#define HOST_GPIO_SDIO_IN0_M  ((HOST_GPIO_SDIO_IN0_V)<<(HOST_GPIO_SDIO_IN0_S))
#define HOST_GPIO_SDIO_IN0_V  0xFFFFFFFF
#define HOST_GPIO_SDIO_IN0_S  0

#define HOST_SLCHOST_GPIO_IN1_REG          (DR_REG_SLCHOST_BASE + 0x40)
/* HOST_GPIO_SDIO_IN1 : RO ;bitpos:[7:0] ;default: 8'b0 ; */
/*description: */
#define HOST_GPIO_SDIO_IN1  0x000000FF
#define HOST_GPIO_SDIO_IN1_M  ((HOST_GPIO_SDIO_IN1_V)<<(HOST_GPIO_SDIO_IN1_S))
#define HOST_GPIO_SDIO_IN1_V  0xFF
#define HOST_GPIO_SDIO_IN1_S  0

#define HOST_SLC0HOST_TOKEN_RDATA_REG          (DR_REG_SLCHOST_BASE + 0x44)
/* HOST_SLC0_RX_PF_EOF : RO ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: */
#define HOST_SLC0_RX_PF_EOF  0x0000000F
#define HOST_SLC0_RX_PF_EOF_M  ((HOST_SLC0_RX_PF_EOF_V)<<(HOST_SLC0_RX_PF_EOF_S))
#define HOST_SLC0_RX_PF_EOF_V  0xF
#define HOST_SLC0_RX_PF_EOF_S  28
/* HOST_HOSTSLC0_TOKEN1 : RO ;bitpos:[27:16] ;default: 12'h0 ; */
/*description: */
#define HOST_HOSTSLC0_TOKEN1  0x00000FFF
#define HOST_HOSTSLC0_TOKEN1_M  ((HOST_HOSTSLC0_TOKEN1_V)<<(HOST_HOSTSLC0_TOKEN1_S))
#define HOST_HOSTSLC0_TOKEN1_V  0xFFF
#define HOST_HOSTSLC0_TOKEN1_S  16
/* HOST_SLC0_RX_PF_VALID : RO ;bitpos:[12] ;default: 4'h0 ; */
/*description: */
#define HOST_SLC0_RX_PF_VALID  (BIT(12))
#define HOST_SLC0_RX_PF_VALID_M  (BIT(12))
#define HOST_SLC0_RX_PF_VALID_V  0x1
#define HOST_SLC0_RX_PF_VALID_S  12
/* HOST_SLC0_TOKEN0 : RO ;bitpos:[11:0] ;default: 12'h0 ; */
/*description: */
#define HOST_SLC0_TOKEN0  0x00000FFF
#define HOST_SLC0_TOKEN0_M  ((HOST_SLC0_TOKEN0_V)<<(HOST_SLC0_TOKEN0_S))
#define HOST_SLC0_TOKEN0_V  0xFFF
#define HOST_SLC0_TOKEN0_S  0

#define HOST_SLC0_HOST_PF_REG          (DR_REG_SLCHOST_BASE + 0x48)
/* HOST_SLC0_PF_DATA : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define HOST_SLC0_PF_DATA  0xFFFFFFFF
#define HOST_SLC0_PF_DATA_M  ((HOST_SLC0_PF_DATA_V)<<(HOST_SLC0_PF_DATA_S))
#define HOST_SLC0_PF_DATA_V  0xFFFFFFFF
#define HOST_SLC0_PF_DATA_S  0

#define HOST_SLC1_HOST_PF_REG          (DR_REG_SLCHOST_BASE + 0x4C)
/* HOST_SLC1_PF_DATA : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define HOST_SLC1_PF_DATA  0xFFFFFFFF
#define HOST_SLC1_PF_DATA_M  ((HOST_SLC1_PF_DATA_V)<<(HOST_SLC1_PF_DATA_S))
#define HOST_SLC1_PF_DATA_V  0xFFFFFFFF
#define HOST_SLC1_PF_DATA_S  0

#define HOST_SLC0HOST_INT_RAW_REG          (DR_REG_SLCHOST_BASE + 0x50)
/* HOST_GPIO_SDIO_INT_RAW : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define HOST_GPIO_SDIO_INT_RAW  (BIT(25))
#define HOST_GPIO_SDIO_INT_RAW_M  (BIT(25))
#define HOST_GPIO_SDIO_INT_RAW_V  0x1
#define HOST_GPIO_SDIO_INT_RAW_S  25
/* HOST_SLC0_HOST_RD_RETRY_INT_RAW : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_HOST_RD_RETRY_INT_RAW  (BIT(24))
#define HOST_SLC0_HOST_RD_RETRY_INT_RAW_M  (BIT(24))
#define HOST_SLC0_HOST_RD_RETRY_INT_RAW_V  0x1
#define HOST_SLC0_HOST_RD_RETRY_INT_RAW_S  24
/* HOST_SLC0_RX_NEW_PACKET_INT_RAW : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_RX_NEW_PACKET_INT_RAW  (BIT(23))
#define HOST_SLC0_RX_NEW_PACKET_INT_RAW_M  (BIT(23))
#define HOST_SLC0_RX_NEW_PACKET_INT_RAW_V  0x1
#define HOST_SLC0_RX_NEW_PACKET_INT_RAW_S  23
/* HOST_SLC0_EXT_BIT3_INT_RAW : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_EXT_BIT3_INT_RAW  (BIT(22))
#define HOST_SLC0_EXT_BIT3_INT_RAW_M  (BIT(22))
#define HOST_SLC0_EXT_BIT3_INT_RAW_V  0x1
#define HOST_SLC0_EXT_BIT3_INT_RAW_S  22
/* HOST_SLC0_EXT_BIT2_INT_RAW : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_EXT_BIT2_INT_RAW  (BIT(21))
#define HOST_SLC0_EXT_BIT2_INT_RAW_M  (BIT(21))
#define HOST_SLC0_EXT_BIT2_INT_RAW_V  0x1
#define HOST_SLC0_EXT_BIT2_INT_RAW_S  21
/* HOST_SLC0_EXT_BIT1_INT_RAW : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_EXT_BIT1_INT_RAW  (BIT(20))
#define HOST_SLC0_EXT_BIT1_INT_RAW_M  (BIT(20))
#define HOST_SLC0_EXT_BIT1_INT_RAW_V  0x1
#define HOST_SLC0_EXT_BIT1_INT_RAW_S  20
/* HOST_SLC0_EXT_BIT0_INT_RAW : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_EXT_BIT0_INT_RAW  (BIT(19))
#define HOST_SLC0_EXT_BIT0_INT_RAW_M  (BIT(19))
#define HOST_SLC0_EXT_BIT0_INT_RAW_V  0x1
#define HOST_SLC0_EXT_BIT0_INT_RAW_S  19
/* HOST_SLC0_RX_PF_VALID_INT_RAW : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_RX_PF_VALID_INT_RAW  (BIT(18))
#define HOST_SLC0_RX_PF_VALID_INT_RAW_M  (BIT(18))
#define HOST_SLC0_RX_PF_VALID_INT_RAW_V  0x1
#define HOST_SLC0_RX_PF_VALID_INT_RAW_S  18
/* HOST_SLC0_TX_OVF_INT_RAW : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TX_OVF_INT_RAW  (BIT(17))
#define HOST_SLC0_TX_OVF_INT_RAW_M  (BIT(17))
#define HOST_SLC0_TX_OVF_INT_RAW_V  0x1
#define HOST_SLC0_TX_OVF_INT_RAW_S  17
/* HOST_SLC0_RX_UDF_INT_RAW : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_RX_UDF_INT_RAW  (BIT(16))
#define HOST_SLC0_RX_UDF_INT_RAW_M  (BIT(16))
#define HOST_SLC0_RX_UDF_INT_RAW_V  0x1
#define HOST_SLC0_RX_UDF_INT_RAW_S  16
/* HOST_SLC0HOST_TX_START_INT_RAW : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_TX_START_INT_RAW  (BIT(15))
#define HOST_SLC0HOST_TX_START_INT_RAW_M  (BIT(15))
#define HOST_SLC0HOST_TX_START_INT_RAW_V  0x1
#define HOST_SLC0HOST_TX_START_INT_RAW_S  15
/* HOST_SLC0HOST_RX_START_INT_RAW : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_RX_START_INT_RAW  (BIT(14))
#define HOST_SLC0HOST_RX_START_INT_RAW_M  (BIT(14))
#define HOST_SLC0HOST_RX_START_INT_RAW_V  0x1
#define HOST_SLC0HOST_RX_START_INT_RAW_S  14
/* HOST_SLC0HOST_RX_EOF_INT_RAW : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_RX_EOF_INT_RAW  (BIT(13))
#define HOST_SLC0HOST_RX_EOF_INT_RAW_M  (BIT(13))
#define HOST_SLC0HOST_RX_EOF_INT_RAW_V  0x1
#define HOST_SLC0HOST_RX_EOF_INT_RAW_S  13
/* HOST_SLC0HOST_RX_SOF_INT_RAW : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_RX_SOF_INT_RAW  (BIT(12))
#define HOST_SLC0HOST_RX_SOF_INT_RAW_M  (BIT(12))
#define HOST_SLC0HOST_RX_SOF_INT_RAW_V  0x1
#define HOST_SLC0HOST_RX_SOF_INT_RAW_S  12
/* HOST_SLC0_TOKEN1_0TO1_INT_RAW : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOKEN1_0TO1_INT_RAW  (BIT(11))
#define HOST_SLC0_TOKEN1_0TO1_INT_RAW_M  (BIT(11))
#define HOST_SLC0_TOKEN1_0TO1_INT_RAW_V  0x1
#define HOST_SLC0_TOKEN1_0TO1_INT_RAW_S  11
/* HOST_SLC0_TOKEN0_0TO1_INT_RAW : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOKEN0_0TO1_INT_RAW  (BIT(10))
#define HOST_SLC0_TOKEN0_0TO1_INT_RAW_M  (BIT(10))
#define HOST_SLC0_TOKEN0_0TO1_INT_RAW_V  0x1
#define HOST_SLC0_TOKEN0_0TO1_INT_RAW_S  10
/* HOST_SLC0_TOKEN1_1TO0_INT_RAW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOKEN1_1TO0_INT_RAW  (BIT(9))
#define HOST_SLC0_TOKEN1_1TO0_INT_RAW_M  (BIT(9))
#define HOST_SLC0_TOKEN1_1TO0_INT_RAW_V  0x1
#define HOST_SLC0_TOKEN1_1TO0_INT_RAW_S  9
/* HOST_SLC0_TOKEN0_1TO0_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOKEN0_1TO0_INT_RAW  (BIT(8))
#define HOST_SLC0_TOKEN0_1TO0_INT_RAW_M  (BIT(8))
#define HOST_SLC0_TOKEN0_1TO0_INT_RAW_V  0x1
#define HOST_SLC0_TOKEN0_1TO0_INT_RAW_S  8
/* HOST_SLC0_TOHOST_BIT7_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT7_INT_RAW  (BIT(7))
#define HOST_SLC0_TOHOST_BIT7_INT_RAW_M  (BIT(7))
#define HOST_SLC0_TOHOST_BIT7_INT_RAW_V  0x1
#define HOST_SLC0_TOHOST_BIT7_INT_RAW_S  7
/* HOST_SLC0_TOHOST_BIT6_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT6_INT_RAW  (BIT(6))
#define HOST_SLC0_TOHOST_BIT6_INT_RAW_M  (BIT(6))
#define HOST_SLC0_TOHOST_BIT6_INT_RAW_V  0x1
#define HOST_SLC0_TOHOST_BIT6_INT_RAW_S  6
/* HOST_SLC0_TOHOST_BIT5_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT5_INT_RAW  (BIT(5))
#define HOST_SLC0_TOHOST_BIT5_INT_RAW_M  (BIT(5))
#define HOST_SLC0_TOHOST_BIT5_INT_RAW_V  0x1
#define HOST_SLC0_TOHOST_BIT5_INT_RAW_S  5
/* HOST_SLC0_TOHOST_BIT4_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT4_INT_RAW  (BIT(4))
#define HOST_SLC0_TOHOST_BIT4_INT_RAW_M  (BIT(4))
#define HOST_SLC0_TOHOST_BIT4_INT_RAW_V  0x1
#define HOST_SLC0_TOHOST_BIT4_INT_RAW_S  4
/* HOST_SLC0_TOHOST_BIT3_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT3_INT_RAW  (BIT(3))
#define HOST_SLC0_TOHOST_BIT3_INT_RAW_M  (BIT(3))
#define HOST_SLC0_TOHOST_BIT3_INT_RAW_V  0x1
#define HOST_SLC0_TOHOST_BIT3_INT_RAW_S  3
/* HOST_SLC0_TOHOST_BIT2_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT2_INT_RAW  (BIT(2))
#define HOST_SLC0_TOHOST_BIT2_INT_RAW_M  (BIT(2))
#define HOST_SLC0_TOHOST_BIT2_INT_RAW_V  0x1
#define HOST_SLC0_TOHOST_BIT2_INT_RAW_S  2
/* HOST_SLC0_TOHOST_BIT1_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT1_INT_RAW  (BIT(1))
#define HOST_SLC0_TOHOST_BIT1_INT_RAW_M  (BIT(1))
#define HOST_SLC0_TOHOST_BIT1_INT_RAW_V  0x1
#define HOST_SLC0_TOHOST_BIT1_INT_RAW_S  1
/* HOST_SLC0_TOHOST_BIT0_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT0_INT_RAW  (BIT(0))
#define HOST_SLC0_TOHOST_BIT0_INT_RAW_M  (BIT(0))
#define HOST_SLC0_TOHOST_BIT0_INT_RAW_V  0x1
#define HOST_SLC0_TOHOST_BIT0_INT_RAW_S  0

#define HOST_SLC1HOST_INT_RAW_REG          (DR_REG_SLCHOST_BASE + 0x54)
/* HOST_SLC1_BT_RX_NEW_PACKET_INT_RAW : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_BT_RX_NEW_PACKET_INT_RAW  (BIT(25))
#define HOST_SLC1_BT_RX_NEW_PACKET_INT_RAW_M  (BIT(25))
#define HOST_SLC1_BT_RX_NEW_PACKET_INT_RAW_V  0x1
#define HOST_SLC1_BT_RX_NEW_PACKET_INT_RAW_S  25
/* HOST_SLC1_HOST_RD_RETRY_INT_RAW : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_HOST_RD_RETRY_INT_RAW  (BIT(24))
#define HOST_SLC1_HOST_RD_RETRY_INT_RAW_M  (BIT(24))
#define HOST_SLC1_HOST_RD_RETRY_INT_RAW_V  0x1
#define HOST_SLC1_HOST_RD_RETRY_INT_RAW_S  24
/* HOST_SLC1_WIFI_RX_NEW_PACKET_INT_RAW : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_WIFI_RX_NEW_PACKET_INT_RAW  (BIT(23))
#define HOST_SLC1_WIFI_RX_NEW_PACKET_INT_RAW_M  (BIT(23))
#define HOST_SLC1_WIFI_RX_NEW_PACKET_INT_RAW_V  0x1
#define HOST_SLC1_WIFI_RX_NEW_PACKET_INT_RAW_S  23
/* HOST_SLC1_EXT_BIT3_INT_RAW : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_EXT_BIT3_INT_RAW  (BIT(22))
#define HOST_SLC1_EXT_BIT3_INT_RAW_M  (BIT(22))
#define HOST_SLC1_EXT_BIT3_INT_RAW_V  0x1
#define HOST_SLC1_EXT_BIT3_INT_RAW_S  22
/* HOST_SLC1_EXT_BIT2_INT_RAW : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_EXT_BIT2_INT_RAW  (BIT(21))
#define HOST_SLC1_EXT_BIT2_INT_RAW_M  (BIT(21))
#define HOST_SLC1_EXT_BIT2_INT_RAW_V  0x1
#define HOST_SLC1_EXT_BIT2_INT_RAW_S  21
/* HOST_SLC1_EXT_BIT1_INT_RAW : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_EXT_BIT1_INT_RAW  (BIT(20))
#define HOST_SLC1_EXT_BIT1_INT_RAW_M  (BIT(20))
#define HOST_SLC1_EXT_BIT1_INT_RAW_V  0x1
#define HOST_SLC1_EXT_BIT1_INT_RAW_S  20
/* HOST_SLC1_EXT_BIT0_INT_RAW : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_EXT_BIT0_INT_RAW  (BIT(19))
#define HOST_SLC1_EXT_BIT0_INT_RAW_M  (BIT(19))
#define HOST_SLC1_EXT_BIT0_INT_RAW_V  0x1
#define HOST_SLC1_EXT_BIT0_INT_RAW_S  19
/* HOST_SLC1_RX_PF_VALID_INT_RAW : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_RX_PF_VALID_INT_RAW  (BIT(18))
#define HOST_SLC1_RX_PF_VALID_INT_RAW_M  (BIT(18))
#define HOST_SLC1_RX_PF_VALID_INT_RAW_V  0x1
#define HOST_SLC1_RX_PF_VALID_INT_RAW_S  18
/* HOST_SLC1_TX_OVF_INT_RAW : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TX_OVF_INT_RAW  (BIT(17))
#define HOST_SLC1_TX_OVF_INT_RAW_M  (BIT(17))
#define HOST_SLC1_TX_OVF_INT_RAW_V  0x1
#define HOST_SLC1_TX_OVF_INT_RAW_S  17
/* HOST_SLC1_RX_UDF_INT_RAW : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_RX_UDF_INT_RAW  (BIT(16))
#define HOST_SLC1_RX_UDF_INT_RAW_M  (BIT(16))
#define HOST_SLC1_RX_UDF_INT_RAW_V  0x1
#define HOST_SLC1_RX_UDF_INT_RAW_S  16
/* HOST_SLC1HOST_TX_START_INT_RAW : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_TX_START_INT_RAW  (BIT(15))
#define HOST_SLC1HOST_TX_START_INT_RAW_M  (BIT(15))
#define HOST_SLC1HOST_TX_START_INT_RAW_V  0x1
#define HOST_SLC1HOST_TX_START_INT_RAW_S  15
/* HOST_SLC1HOST_RX_START_INT_RAW : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_RX_START_INT_RAW  (BIT(14))
#define HOST_SLC1HOST_RX_START_INT_RAW_M  (BIT(14))
#define HOST_SLC1HOST_RX_START_INT_RAW_V  0x1
#define HOST_SLC1HOST_RX_START_INT_RAW_S  14
/* HOST_SLC1HOST_RX_EOF_INT_RAW : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_RX_EOF_INT_RAW  (BIT(13))
#define HOST_SLC1HOST_RX_EOF_INT_RAW_M  (BIT(13))
#define HOST_SLC1HOST_RX_EOF_INT_RAW_V  0x1
#define HOST_SLC1HOST_RX_EOF_INT_RAW_S  13
/* HOST_SLC1HOST_RX_SOF_INT_RAW : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_RX_SOF_INT_RAW  (BIT(12))
#define HOST_SLC1HOST_RX_SOF_INT_RAW_M  (BIT(12))
#define HOST_SLC1HOST_RX_SOF_INT_RAW_V  0x1
#define HOST_SLC1HOST_RX_SOF_INT_RAW_S  12
/* HOST_SLC1_TOKEN1_0TO1_INT_RAW : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOKEN1_0TO1_INT_RAW  (BIT(11))
#define HOST_SLC1_TOKEN1_0TO1_INT_RAW_M  (BIT(11))
#define HOST_SLC1_TOKEN1_0TO1_INT_RAW_V  0x1
#define HOST_SLC1_TOKEN1_0TO1_INT_RAW_S  11
/* HOST_SLC1_TOKEN0_0TO1_INT_RAW : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOKEN0_0TO1_INT_RAW  (BIT(10))
#define HOST_SLC1_TOKEN0_0TO1_INT_RAW_M  (BIT(10))
#define HOST_SLC1_TOKEN0_0TO1_INT_RAW_V  0x1
#define HOST_SLC1_TOKEN0_0TO1_INT_RAW_S  10
/* HOST_SLC1_TOKEN1_1TO0_INT_RAW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOKEN1_1TO0_INT_RAW  (BIT(9))
#define HOST_SLC1_TOKEN1_1TO0_INT_RAW_M  (BIT(9))
#define HOST_SLC1_TOKEN1_1TO0_INT_RAW_V  0x1
#define HOST_SLC1_TOKEN1_1TO0_INT_RAW_S  9
/* HOST_SLC1_TOKEN0_1TO0_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOKEN0_1TO0_INT_RAW  (BIT(8))
#define HOST_SLC1_TOKEN0_1TO0_INT_RAW_M  (BIT(8))
#define HOST_SLC1_TOKEN0_1TO0_INT_RAW_V  0x1
#define HOST_SLC1_TOKEN0_1TO0_INT_RAW_S  8
/* HOST_SLC1_TOHOST_BIT7_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT7_INT_RAW  (BIT(7))
#define HOST_SLC1_TOHOST_BIT7_INT_RAW_M  (BIT(7))
#define HOST_SLC1_TOHOST_BIT7_INT_RAW_V  0x1
#define HOST_SLC1_TOHOST_BIT7_INT_RAW_S  7
/* HOST_SLC1_TOHOST_BIT6_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT6_INT_RAW  (BIT(6))
#define HOST_SLC1_TOHOST_BIT6_INT_RAW_M  (BIT(6))
#define HOST_SLC1_TOHOST_BIT6_INT_RAW_V  0x1
#define HOST_SLC1_TOHOST_BIT6_INT_RAW_S  6
/* HOST_SLC1_TOHOST_BIT5_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT5_INT_RAW  (BIT(5))
#define HOST_SLC1_TOHOST_BIT5_INT_RAW_M  (BIT(5))
#define HOST_SLC1_TOHOST_BIT5_INT_RAW_V  0x1
#define HOST_SLC1_TOHOST_BIT5_INT_RAW_S  5
/* HOST_SLC1_TOHOST_BIT4_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT4_INT_RAW  (BIT(4))
#define HOST_SLC1_TOHOST_BIT4_INT_RAW_M  (BIT(4))
#define HOST_SLC1_TOHOST_BIT4_INT_RAW_V  0x1
#define HOST_SLC1_TOHOST_BIT4_INT_RAW_S  4
/* HOST_SLC1_TOHOST_BIT3_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT3_INT_RAW  (BIT(3))
#define HOST_SLC1_TOHOST_BIT3_INT_RAW_M  (BIT(3))
#define HOST_SLC1_TOHOST_BIT3_INT_RAW_V  0x1
#define HOST_SLC1_TOHOST_BIT3_INT_RAW_S  3
/* HOST_SLC1_TOHOST_BIT2_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT2_INT_RAW  (BIT(2))
#define HOST_SLC1_TOHOST_BIT2_INT_RAW_M  (BIT(2))
#define HOST_SLC1_TOHOST_BIT2_INT_RAW_V  0x1
#define HOST_SLC1_TOHOST_BIT2_INT_RAW_S  2
/* HOST_SLC1_TOHOST_BIT1_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT1_INT_RAW  (BIT(1))
#define HOST_SLC1_TOHOST_BIT1_INT_RAW_M  (BIT(1))
#define HOST_SLC1_TOHOST_BIT1_INT_RAW_V  0x1
#define HOST_SLC1_TOHOST_BIT1_INT_RAW_S  1
/* HOST_SLC1_TOHOST_BIT0_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT0_INT_RAW  (BIT(0))
#define HOST_SLC1_TOHOST_BIT0_INT_RAW_M  (BIT(0))
#define HOST_SLC1_TOHOST_BIT0_INT_RAW_V  0x1
#define HOST_SLC1_TOHOST_BIT0_INT_RAW_S  0

#define HOST_SLC0HOST_INT_ST_REG          (DR_REG_SLCHOST_BASE + 0x58)
/* HOST_GPIO_SDIO_INT_ST : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define HOST_GPIO_SDIO_INT_ST  (BIT(25))
#define HOST_GPIO_SDIO_INT_ST_M  (BIT(25))
#define HOST_GPIO_SDIO_INT_ST_V  0x1
#define HOST_GPIO_SDIO_INT_ST_S  25
/* HOST_SLC0_HOST_RD_RETRY_INT_ST : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_HOST_RD_RETRY_INT_ST  (BIT(24))
#define HOST_SLC0_HOST_RD_RETRY_INT_ST_M  (BIT(24))
#define HOST_SLC0_HOST_RD_RETRY_INT_ST_V  0x1
#define HOST_SLC0_HOST_RD_RETRY_INT_ST_S  24
/* HOST_SLC0_RX_NEW_PACKET_INT_ST : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_RX_NEW_PACKET_INT_ST  (BIT(23))
#define HOST_SLC0_RX_NEW_PACKET_INT_ST_M  (BIT(23))
#define HOST_SLC0_RX_NEW_PACKET_INT_ST_V  0x1
#define HOST_SLC0_RX_NEW_PACKET_INT_ST_S  23
/* HOST_SLC0_EXT_BIT3_INT_ST : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_EXT_BIT3_INT_ST  (BIT(22))
#define HOST_SLC0_EXT_BIT3_INT_ST_M  (BIT(22))
#define HOST_SLC0_EXT_BIT3_INT_ST_V  0x1
#define HOST_SLC0_EXT_BIT3_INT_ST_S  22
/* HOST_SLC0_EXT_BIT2_INT_ST : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_EXT_BIT2_INT_ST  (BIT(21))
#define HOST_SLC0_EXT_BIT2_INT_ST_M  (BIT(21))
#define HOST_SLC0_EXT_BIT2_INT_ST_V  0x1
#define HOST_SLC0_EXT_BIT2_INT_ST_S  21
/* HOST_SLC0_EXT_BIT1_INT_ST : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_EXT_BIT1_INT_ST  (BIT(20))
#define HOST_SLC0_EXT_BIT1_INT_ST_M  (BIT(20))
#define HOST_SLC0_EXT_BIT1_INT_ST_V  0x1
#define HOST_SLC0_EXT_BIT1_INT_ST_S  20
/* HOST_SLC0_EXT_BIT0_INT_ST : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_EXT_BIT0_INT_ST  (BIT(19))
#define HOST_SLC0_EXT_BIT0_INT_ST_M  (BIT(19))
#define HOST_SLC0_EXT_BIT0_INT_ST_V  0x1
#define HOST_SLC0_EXT_BIT0_INT_ST_S  19
/* HOST_SLC0_RX_PF_VALID_INT_ST : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_RX_PF_VALID_INT_ST  (BIT(18))
#define HOST_SLC0_RX_PF_VALID_INT_ST_M  (BIT(18))
#define HOST_SLC0_RX_PF_VALID_INT_ST_V  0x1
#define HOST_SLC0_RX_PF_VALID_INT_ST_S  18
/* HOST_SLC0_TX_OVF_INT_ST : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TX_OVF_INT_ST  (BIT(17))
#define HOST_SLC0_TX_OVF_INT_ST_M  (BIT(17))
#define HOST_SLC0_TX_OVF_INT_ST_V  0x1
#define HOST_SLC0_TX_OVF_INT_ST_S  17
/* HOST_SLC0_RX_UDF_INT_ST : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_RX_UDF_INT_ST  (BIT(16))
#define HOST_SLC0_RX_UDF_INT_ST_M  (BIT(16))
#define HOST_SLC0_RX_UDF_INT_ST_V  0x1
#define HOST_SLC0_RX_UDF_INT_ST_S  16
/* HOST_SLC0HOST_TX_START_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_TX_START_INT_ST  (BIT(15))
#define HOST_SLC0HOST_TX_START_INT_ST_M  (BIT(15))
#define HOST_SLC0HOST_TX_START_INT_ST_V  0x1
#define HOST_SLC0HOST_TX_START_INT_ST_S  15
/* HOST_SLC0HOST_RX_START_INT_ST : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_RX_START_INT_ST  (BIT(14))
#define HOST_SLC0HOST_RX_START_INT_ST_M  (BIT(14))
#define HOST_SLC0HOST_RX_START_INT_ST_V  0x1
#define HOST_SLC0HOST_RX_START_INT_ST_S  14
/* HOST_SLC0HOST_RX_EOF_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_RX_EOF_INT_ST  (BIT(13))
#define HOST_SLC0HOST_RX_EOF_INT_ST_M  (BIT(13))
#define HOST_SLC0HOST_RX_EOF_INT_ST_V  0x1
#define HOST_SLC0HOST_RX_EOF_INT_ST_S  13
/* HOST_SLC0HOST_RX_SOF_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_RX_SOF_INT_ST  (BIT(12))
#define HOST_SLC0HOST_RX_SOF_INT_ST_M  (BIT(12))
#define HOST_SLC0HOST_RX_SOF_INT_ST_V  0x1
#define HOST_SLC0HOST_RX_SOF_INT_ST_S  12
/* HOST_SLC0_TOKEN1_0TO1_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOKEN1_0TO1_INT_ST  (BIT(11))
#define HOST_SLC0_TOKEN1_0TO1_INT_ST_M  (BIT(11))
#define HOST_SLC0_TOKEN1_0TO1_INT_ST_V  0x1
#define HOST_SLC0_TOKEN1_0TO1_INT_ST_S  11
/* HOST_SLC0_TOKEN0_0TO1_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOKEN0_0TO1_INT_ST  (BIT(10))
#define HOST_SLC0_TOKEN0_0TO1_INT_ST_M  (BIT(10))
#define HOST_SLC0_TOKEN0_0TO1_INT_ST_V  0x1
#define HOST_SLC0_TOKEN0_0TO1_INT_ST_S  10
/* HOST_SLC0_TOKEN1_1TO0_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOKEN1_1TO0_INT_ST  (BIT(9))
#define HOST_SLC0_TOKEN1_1TO0_INT_ST_M  (BIT(9))
#define HOST_SLC0_TOKEN1_1TO0_INT_ST_V  0x1
#define HOST_SLC0_TOKEN1_1TO0_INT_ST_S  9
/* HOST_SLC0_TOKEN0_1TO0_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOKEN0_1TO0_INT_ST  (BIT(8))
#define HOST_SLC0_TOKEN0_1TO0_INT_ST_M  (BIT(8))
#define HOST_SLC0_TOKEN0_1TO0_INT_ST_V  0x1
#define HOST_SLC0_TOKEN0_1TO0_INT_ST_S  8
/* HOST_SLC0_TOHOST_BIT7_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT7_INT_ST  (BIT(7))
#define HOST_SLC0_TOHOST_BIT7_INT_ST_M  (BIT(7))
#define HOST_SLC0_TOHOST_BIT7_INT_ST_V  0x1
#define HOST_SLC0_TOHOST_BIT7_INT_ST_S  7
/* HOST_SLC0_TOHOST_BIT6_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT6_INT_ST  (BIT(6))
#define HOST_SLC0_TOHOST_BIT6_INT_ST_M  (BIT(6))
#define HOST_SLC0_TOHOST_BIT6_INT_ST_V  0x1
#define HOST_SLC0_TOHOST_BIT6_INT_ST_S  6
/* HOST_SLC0_TOHOST_BIT5_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT5_INT_ST  (BIT(5))
#define HOST_SLC0_TOHOST_BIT5_INT_ST_M  (BIT(5))
#define HOST_SLC0_TOHOST_BIT5_INT_ST_V  0x1
#define HOST_SLC0_TOHOST_BIT5_INT_ST_S  5
/* HOST_SLC0_TOHOST_BIT4_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT4_INT_ST  (BIT(4))
#define HOST_SLC0_TOHOST_BIT4_INT_ST_M  (BIT(4))
#define HOST_SLC0_TOHOST_BIT4_INT_ST_V  0x1
#define HOST_SLC0_TOHOST_BIT4_INT_ST_S  4
/* HOST_SLC0_TOHOST_BIT3_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT3_INT_ST  (BIT(3))
#define HOST_SLC0_TOHOST_BIT3_INT_ST_M  (BIT(3))
#define HOST_SLC0_TOHOST_BIT3_INT_ST_V  0x1
#define HOST_SLC0_TOHOST_BIT3_INT_ST_S  3
/* HOST_SLC0_TOHOST_BIT2_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT2_INT_ST  (BIT(2))
#define HOST_SLC0_TOHOST_BIT2_INT_ST_M  (BIT(2))
#define HOST_SLC0_TOHOST_BIT2_INT_ST_V  0x1
#define HOST_SLC0_TOHOST_BIT2_INT_ST_S  2
/* HOST_SLC0_TOHOST_BIT1_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT1_INT_ST  (BIT(1))
#define HOST_SLC0_TOHOST_BIT1_INT_ST_M  (BIT(1))
#define HOST_SLC0_TOHOST_BIT1_INT_ST_V  0x1
#define HOST_SLC0_TOHOST_BIT1_INT_ST_S  1
/* HOST_SLC0_TOHOST_BIT0_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT0_INT_ST  (BIT(0))
#define HOST_SLC0_TOHOST_BIT0_INT_ST_M  (BIT(0))
#define HOST_SLC0_TOHOST_BIT0_INT_ST_V  0x1
#define HOST_SLC0_TOHOST_BIT0_INT_ST_S  0

#define HOST_SLC1HOST_INT_ST_REG          (DR_REG_SLCHOST_BASE + 0x5C)
/* HOST_SLC1_BT_RX_NEW_PACKET_INT_ST : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_BT_RX_NEW_PACKET_INT_ST  (BIT(25))
#define HOST_SLC1_BT_RX_NEW_PACKET_INT_ST_M  (BIT(25))
#define HOST_SLC1_BT_RX_NEW_PACKET_INT_ST_V  0x1
#define HOST_SLC1_BT_RX_NEW_PACKET_INT_ST_S  25
/* HOST_SLC1_HOST_RD_RETRY_INT_ST : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_HOST_RD_RETRY_INT_ST  (BIT(24))
#define HOST_SLC1_HOST_RD_RETRY_INT_ST_M  (BIT(24))
#define HOST_SLC1_HOST_RD_RETRY_INT_ST_V  0x1
#define HOST_SLC1_HOST_RD_RETRY_INT_ST_S  24
/* HOST_SLC1_WIFI_RX_NEW_PACKET_INT_ST : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_WIFI_RX_NEW_PACKET_INT_ST  (BIT(23))
#define HOST_SLC1_WIFI_RX_NEW_PACKET_INT_ST_M  (BIT(23))
#define HOST_SLC1_WIFI_RX_NEW_PACKET_INT_ST_V  0x1
#define HOST_SLC1_WIFI_RX_NEW_PACKET_INT_ST_S  23
/* HOST_SLC1_EXT_BIT3_INT_ST : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_EXT_BIT3_INT_ST  (BIT(22))
#define HOST_SLC1_EXT_BIT3_INT_ST_M  (BIT(22))
#define HOST_SLC1_EXT_BIT3_INT_ST_V  0x1
#define HOST_SLC1_EXT_BIT3_INT_ST_S  22
/* HOST_SLC1_EXT_BIT2_INT_ST : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_EXT_BIT2_INT_ST  (BIT(21))
#define HOST_SLC1_EXT_BIT2_INT_ST_M  (BIT(21))
#define HOST_SLC1_EXT_BIT2_INT_ST_V  0x1
#define HOST_SLC1_EXT_BIT2_INT_ST_S  21
/* HOST_SLC1_EXT_BIT1_INT_ST : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_EXT_BIT1_INT_ST  (BIT(20))
#define HOST_SLC1_EXT_BIT1_INT_ST_M  (BIT(20))
#define HOST_SLC1_EXT_BIT1_INT_ST_V  0x1
#define HOST_SLC1_EXT_BIT1_INT_ST_S  20
/* HOST_SLC1_EXT_BIT0_INT_ST : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_EXT_BIT0_INT_ST  (BIT(19))
#define HOST_SLC1_EXT_BIT0_INT_ST_M  (BIT(19))
#define HOST_SLC1_EXT_BIT0_INT_ST_V  0x1
#define HOST_SLC1_EXT_BIT0_INT_ST_S  19
/* HOST_SLC1_RX_PF_VALID_INT_ST : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_RX_PF_VALID_INT_ST  (BIT(18))
#define HOST_SLC1_RX_PF_VALID_INT_ST_M  (BIT(18))
#define HOST_SLC1_RX_PF_VALID_INT_ST_V  0x1
#define HOST_SLC1_RX_PF_VALID_INT_ST_S  18
/* HOST_SLC1_TX_OVF_INT_ST : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TX_OVF_INT_ST  (BIT(17))
#define HOST_SLC1_TX_OVF_INT_ST_M  (BIT(17))
#define HOST_SLC1_TX_OVF_INT_ST_V  0x1
#define HOST_SLC1_TX_OVF_INT_ST_S  17
/* HOST_SLC1_RX_UDF_INT_ST : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_RX_UDF_INT_ST  (BIT(16))
#define HOST_SLC1_RX_UDF_INT_ST_M  (BIT(16))
#define HOST_SLC1_RX_UDF_INT_ST_V  0x1
#define HOST_SLC1_RX_UDF_INT_ST_S  16
/* HOST_SLC1HOST_TX_START_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_TX_START_INT_ST  (BIT(15))
#define HOST_SLC1HOST_TX_START_INT_ST_M  (BIT(15))
#define HOST_SLC1HOST_TX_START_INT_ST_V  0x1
#define HOST_SLC1HOST_TX_START_INT_ST_S  15
/* HOST_SLC1HOST_RX_START_INT_ST : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_RX_START_INT_ST  (BIT(14))
#define HOST_SLC1HOST_RX_START_INT_ST_M  (BIT(14))
#define HOST_SLC1HOST_RX_START_INT_ST_V  0x1
#define HOST_SLC1HOST_RX_START_INT_ST_S  14
/* HOST_SLC1HOST_RX_EOF_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_RX_EOF_INT_ST  (BIT(13))
#define HOST_SLC1HOST_RX_EOF_INT_ST_M  (BIT(13))
#define HOST_SLC1HOST_RX_EOF_INT_ST_V  0x1
#define HOST_SLC1HOST_RX_EOF_INT_ST_S  13
/* HOST_SLC1HOST_RX_SOF_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_RX_SOF_INT_ST  (BIT(12))
#define HOST_SLC1HOST_RX_SOF_INT_ST_M  (BIT(12))
#define HOST_SLC1HOST_RX_SOF_INT_ST_V  0x1
#define HOST_SLC1HOST_RX_SOF_INT_ST_S  12
/* HOST_SLC1_TOKEN1_0TO1_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOKEN1_0TO1_INT_ST  (BIT(11))
#define HOST_SLC1_TOKEN1_0TO1_INT_ST_M  (BIT(11))
#define HOST_SLC1_TOKEN1_0TO1_INT_ST_V  0x1
#define HOST_SLC1_TOKEN1_0TO1_INT_ST_S  11
/* HOST_SLC1_TOKEN0_0TO1_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOKEN0_0TO1_INT_ST  (BIT(10))
#define HOST_SLC1_TOKEN0_0TO1_INT_ST_M  (BIT(10))
#define HOST_SLC1_TOKEN0_0TO1_INT_ST_V  0x1
#define HOST_SLC1_TOKEN0_0TO1_INT_ST_S  10
/* HOST_SLC1_TOKEN1_1TO0_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOKEN1_1TO0_INT_ST  (BIT(9))
#define HOST_SLC1_TOKEN1_1TO0_INT_ST_M  (BIT(9))
#define HOST_SLC1_TOKEN1_1TO0_INT_ST_V  0x1
#define HOST_SLC1_TOKEN1_1TO0_INT_ST_S  9
/* HOST_SLC1_TOKEN0_1TO0_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOKEN0_1TO0_INT_ST  (BIT(8))
#define HOST_SLC1_TOKEN0_1TO0_INT_ST_M  (BIT(8))
#define HOST_SLC1_TOKEN0_1TO0_INT_ST_V  0x1
#define HOST_SLC1_TOKEN0_1TO0_INT_ST_S  8
/* HOST_SLC1_TOHOST_BIT7_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT7_INT_ST  (BIT(7))
#define HOST_SLC1_TOHOST_BIT7_INT_ST_M  (BIT(7))
#define HOST_SLC1_TOHOST_BIT7_INT_ST_V  0x1
#define HOST_SLC1_TOHOST_BIT7_INT_ST_S  7
/* HOST_SLC1_TOHOST_BIT6_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT6_INT_ST  (BIT(6))
#define HOST_SLC1_TOHOST_BIT6_INT_ST_M  (BIT(6))
#define HOST_SLC1_TOHOST_BIT6_INT_ST_V  0x1
#define HOST_SLC1_TOHOST_BIT6_INT_ST_S  6
/* HOST_SLC1_TOHOST_BIT5_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT5_INT_ST  (BIT(5))
#define HOST_SLC1_TOHOST_BIT5_INT_ST_M  (BIT(5))
#define HOST_SLC1_TOHOST_BIT5_INT_ST_V  0x1
#define HOST_SLC1_TOHOST_BIT5_INT_ST_S  5
/* HOST_SLC1_TOHOST_BIT4_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT4_INT_ST  (BIT(4))
#define HOST_SLC1_TOHOST_BIT4_INT_ST_M  (BIT(4))
#define HOST_SLC1_TOHOST_BIT4_INT_ST_V  0x1
#define HOST_SLC1_TOHOST_BIT4_INT_ST_S  4
/* HOST_SLC1_TOHOST_BIT3_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT3_INT_ST  (BIT(3))
#define HOST_SLC1_TOHOST_BIT3_INT_ST_M  (BIT(3))
#define HOST_SLC1_TOHOST_BIT3_INT_ST_V  0x1
#define HOST_SLC1_TOHOST_BIT3_INT_ST_S  3
/* HOST_SLC1_TOHOST_BIT2_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT2_INT_ST  (BIT(2))
#define HOST_SLC1_TOHOST_BIT2_INT_ST_M  (BIT(2))
#define HOST_SLC1_TOHOST_BIT2_INT_ST_V  0x1
#define HOST_SLC1_TOHOST_BIT2_INT_ST_S  2
/* HOST_SLC1_TOHOST_BIT1_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT1_INT_ST  (BIT(1))
#define HOST_SLC1_TOHOST_BIT1_INT_ST_M  (BIT(1))
#define HOST_SLC1_TOHOST_BIT1_INT_ST_V  0x1
#define HOST_SLC1_TOHOST_BIT1_INT_ST_S  1
/* HOST_SLC1_TOHOST_BIT0_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT0_INT_ST  (BIT(0))
#define HOST_SLC1_TOHOST_BIT0_INT_ST_M  (BIT(0))
#define HOST_SLC1_TOHOST_BIT0_INT_ST_V  0x1
#define HOST_SLC1_TOHOST_BIT0_INT_ST_S  0

#define HOST_SLCHOST_PKT_LEN_REG          (DR_REG_SLCHOST_BASE + 0x60)
/* HOST_HOSTSLC0_LEN_CHECK : RO ;bitpos:[31:20] ;default: 10'h0 ; */
/*description: */
#define HOST_HOSTSLC0_LEN_CHECK  0x00000FFF
#define HOST_HOSTSLC0_LEN_CHECK_M  ((HOST_HOSTSLC0_LEN_CHECK_V)<<(HOST_HOSTSLC0_LEN_CHECK_S))
#define HOST_HOSTSLC0_LEN_CHECK_V  0xFFF
#define HOST_HOSTSLC0_LEN_CHECK_S  20
/* HOST_HOSTSLC0_LEN : RO ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: */
#define HOST_HOSTSLC0_LEN  0x000FFFFF
#define HOST_HOSTSLC0_LEN_M  ((HOST_HOSTSLC0_LEN_V)<<(HOST_HOSTSLC0_LEN_S))
#define HOST_HOSTSLC0_LEN_V  0xFFFFF
#define HOST_HOSTSLC0_LEN_S  0

#define HOST_SLCHOST_STATE_W0_REG          (DR_REG_SLCHOST_BASE + 0x64)
/* HOST_SLCHOST_STATE3 : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_STATE3  0x000000FF
#define HOST_SLCHOST_STATE3_M  ((HOST_SLCHOST_STATE3_V)<<(HOST_SLCHOST_STATE3_S))
#define HOST_SLCHOST_STATE3_V  0xFF
#define HOST_SLCHOST_STATE3_S  24
/* HOST_SLCHOST_STATE2 : RO ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_STATE2  0x000000FF
#define HOST_SLCHOST_STATE2_M  ((HOST_SLCHOST_STATE2_V)<<(HOST_SLCHOST_STATE2_S))
#define HOST_SLCHOST_STATE2_V  0xFF
#define HOST_SLCHOST_STATE2_S  16
/* HOST_SLCHOST_STATE1 : RO ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_STATE1  0x000000FF
#define HOST_SLCHOST_STATE1_M  ((HOST_SLCHOST_STATE1_V)<<(HOST_SLCHOST_STATE1_S))
#define HOST_SLCHOST_STATE1_V  0xFF
#define HOST_SLCHOST_STATE1_S  8
/* HOST_SLCHOST_STATE0 : RO ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_STATE0  0x000000FF
#define HOST_SLCHOST_STATE0_M  ((HOST_SLCHOST_STATE0_V)<<(HOST_SLCHOST_STATE0_S))
#define HOST_SLCHOST_STATE0_V  0xFF
#define HOST_SLCHOST_STATE0_S  0

#define HOST_SLCHOST_STATE_W1_REG          (DR_REG_SLCHOST_BASE + 0x68)
/* HOST_SLCHOST_STATE7 : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_STATE7  0x000000FF
#define HOST_SLCHOST_STATE7_M  ((HOST_SLCHOST_STATE7_V)<<(HOST_SLCHOST_STATE7_S))
#define HOST_SLCHOST_STATE7_V  0xFF
#define HOST_SLCHOST_STATE7_S  24
/* HOST_SLCHOST_STATE6 : RO ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_STATE6  0x000000FF
#define HOST_SLCHOST_STATE6_M  ((HOST_SLCHOST_STATE6_V)<<(HOST_SLCHOST_STATE6_S))
#define HOST_SLCHOST_STATE6_V  0xFF
#define HOST_SLCHOST_STATE6_S  16
/* HOST_SLCHOST_STATE5 : RO ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_STATE5  0x000000FF
#define HOST_SLCHOST_STATE5_M  ((HOST_SLCHOST_STATE5_V)<<(HOST_SLCHOST_STATE5_S))
#define HOST_SLCHOST_STATE5_V  0xFF
#define HOST_SLCHOST_STATE5_S  8
/* HOST_SLCHOST_STATE4 : RO ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_STATE4  0x000000FF
#define HOST_SLCHOST_STATE4_M  ((HOST_SLCHOST_STATE4_V)<<(HOST_SLCHOST_STATE4_S))
#define HOST_SLCHOST_STATE4_V  0xFF
#define HOST_SLCHOST_STATE4_S  0

#define HOST_SLCHOST_CONF_W_REG(pos) (HOST_SLCHOST_CONF_W0_REG+pos+(pos>23?4:0)+(pos>31?12:0))

#define HOST_SLCHOST_CONF_W0_REG          (DR_REG_SLCHOST_BASE + 0x6C)
/* HOST_SLCHOST_CONF3 : R/W ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF3  0x000000FF
#define HOST_SLCHOST_CONF3_M  ((HOST_SLCHOST_CONF3_V)<<(HOST_SLCHOST_CONF3_S))
#define HOST_SLCHOST_CONF3_V  0xFF
#define HOST_SLCHOST_CONF3_S  24
/* HOST_SLCHOST_CONF2 : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF2  0x000000FF
#define HOST_SLCHOST_CONF2_M  ((HOST_SLCHOST_CONF2_V)<<(HOST_SLCHOST_CONF2_S))
#define HOST_SLCHOST_CONF2_V  0xFF
#define HOST_SLCHOST_CONF2_S  16
/* HOST_SLCHOST_CONF1 : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF1  0x000000FF
#define HOST_SLCHOST_CONF1_M  ((HOST_SLCHOST_CONF1_V)<<(HOST_SLCHOST_CONF1_S))
#define HOST_SLCHOST_CONF1_V  0xFF
#define HOST_SLCHOST_CONF1_S  8
/* HOST_SLCHOST_CONF0 : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF0  0x000000FF
#define HOST_SLCHOST_CONF0_M  ((HOST_SLCHOST_CONF0_V)<<(HOST_SLCHOST_CONF0_S))
#define HOST_SLCHOST_CONF0_V  0xFF
#define HOST_SLCHOST_CONF0_S  0

#define HOST_SLCHOST_CONF_W1_REG          (DR_REG_SLCHOST_BASE + 0x70)
/* HOST_SLCHOST_CONF7 : R/W ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF7  0x000000FF
#define HOST_SLCHOST_CONF7_M  ((HOST_SLCHOST_CONF7_V)<<(HOST_SLCHOST_CONF7_S))
#define HOST_SLCHOST_CONF7_V  0xFF
#define HOST_SLCHOST_CONF7_S  24
/* HOST_SLCHOST_CONF6 : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF6  0x000000FF
#define HOST_SLCHOST_CONF6_M  ((HOST_SLCHOST_CONF6_V)<<(HOST_SLCHOST_CONF6_S))
#define HOST_SLCHOST_CONF6_V  0xFF
#define HOST_SLCHOST_CONF6_S  16
/* HOST_SLCHOST_CONF5 : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF5  0x000000FF
#define HOST_SLCHOST_CONF5_M  ((HOST_SLCHOST_CONF5_V)<<(HOST_SLCHOST_CONF5_S))
#define HOST_SLCHOST_CONF5_V  0xFF
#define HOST_SLCHOST_CONF5_S  8
/* HOST_SLCHOST_CONF4 : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF4  0x000000FF
#define HOST_SLCHOST_CONF4_M  ((HOST_SLCHOST_CONF4_V)<<(HOST_SLCHOST_CONF4_S))
#define HOST_SLCHOST_CONF4_V  0xFF
#define HOST_SLCHOST_CONF4_S  0

#define HOST_SLCHOST_CONF_W2_REG          (DR_REG_SLCHOST_BASE + 0x74)
/* HOST_SLCHOST_CONF11 : R/W ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF11  0x000000FF
#define HOST_SLCHOST_CONF11_M  ((HOST_SLCHOST_CONF11_V)<<(HOST_SLCHOST_CONF11_S))
#define HOST_SLCHOST_CONF11_V  0xFF
#define HOST_SLCHOST_CONF11_S  24
/* HOST_SLCHOST_CONF10 : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF10  0x000000FF
#define HOST_SLCHOST_CONF10_M  ((HOST_SLCHOST_CONF10_V)<<(HOST_SLCHOST_CONF10_S))
#define HOST_SLCHOST_CONF10_V  0xFF
#define HOST_SLCHOST_CONF10_S  16
/* HOST_SLCHOST_CONF9 : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF9  0x000000FF
#define HOST_SLCHOST_CONF9_M  ((HOST_SLCHOST_CONF9_V)<<(HOST_SLCHOST_CONF9_S))
#define HOST_SLCHOST_CONF9_V  0xFF
#define HOST_SLCHOST_CONF9_S  8
/* HOST_SLCHOST_CONF8 : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF8  0x000000FF
#define HOST_SLCHOST_CONF8_M  ((HOST_SLCHOST_CONF8_V)<<(HOST_SLCHOST_CONF8_S))
#define HOST_SLCHOST_CONF8_V  0xFF
#define HOST_SLCHOST_CONF8_S  0

#define HOST_SLCHOST_CONF_W3_REG          (DR_REG_SLCHOST_BASE + 0x78)
/* HOST_SLCHOST_CONF15 : R/W ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF15  0x000000FF
#define HOST_SLCHOST_CONF15_M  ((HOST_SLCHOST_CONF15_V)<<(HOST_SLCHOST_CONF15_S))
#define HOST_SLCHOST_CONF15_V  0xFF
#define HOST_SLCHOST_CONF15_S  24
/* HOST_SLCHOST_CONF14 : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF14  0x000000FF
#define HOST_SLCHOST_CONF14_M  ((HOST_SLCHOST_CONF14_V)<<(HOST_SLCHOST_CONF14_S))
#define HOST_SLCHOST_CONF14_V  0xFF
#define HOST_SLCHOST_CONF14_S  16
/* HOST_SLCHOST_CONF13 : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF13  0x000000FF
#define HOST_SLCHOST_CONF13_M  ((HOST_SLCHOST_CONF13_V)<<(HOST_SLCHOST_CONF13_S))
#define HOST_SLCHOST_CONF13_V  0xFF
#define HOST_SLCHOST_CONF13_S  8
/* HOST_SLCHOST_CONF12 : R/W ;bitpos:[7:0] ;default: 8'hc0 ; */
/*description: */
#define HOST_SLCHOST_CONF12  0x000000FF
#define HOST_SLCHOST_CONF12_M  ((HOST_SLCHOST_CONF12_V)<<(HOST_SLCHOST_CONF12_S))
#define HOST_SLCHOST_CONF12_V  0xFF
#define HOST_SLCHOST_CONF12_S  0

#define HOST_SLCHOST_CONF_W4_REG          (DR_REG_SLCHOST_BASE + 0x7C)
/* HOST_SLCHOST_CONF19 : R/W ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: Interrupt to target CPU*/
#define HOST_SLCHOST_CONF19  0x000000FF
#define HOST_SLCHOST_CONF19_M  ((HOST_SLCHOST_CONF19_V)<<(HOST_SLCHOST_CONF19_S))
#define HOST_SLCHOST_CONF19_V  0xFF
#define HOST_SLCHOST_CONF19_S  24
/* HOST_SLCHOST_CONF18 : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF18  0x000000FF
#define HOST_SLCHOST_CONF18_M  ((HOST_SLCHOST_CONF18_V)<<(HOST_SLCHOST_CONF18_S))
#define HOST_SLCHOST_CONF18_V  0xFF
#define HOST_SLCHOST_CONF18_S  16
/* HOST_SLCHOST_CONF17 : R/W ;bitpos:[15:8] ;default: 8'h1 ; */
/*description: SLC timeout enable*/
#define HOST_SLCHOST_CONF17  0x000000FF
#define HOST_SLCHOST_CONF17_M  ((HOST_SLCHOST_CONF17_V)<<(HOST_SLCHOST_CONF17_S))
#define HOST_SLCHOST_CONF17_V  0xFF
#define HOST_SLCHOST_CONF17_S  8
/* HOST_SLCHOST_CONF16 : R/W ;bitpos:[7:0] ;default: 8'hFF ; */
/*description: SLC timeout value*/
#define HOST_SLCHOST_CONF16  0x000000FF
#define HOST_SLCHOST_CONF16_M  ((HOST_SLCHOST_CONF16_V)<<(HOST_SLCHOST_CONF16_S))
#define HOST_SLCHOST_CONF16_V  0xFF
#define HOST_SLCHOST_CONF16_S  0

#define HOST_SLCHOST_CONF_W5_REG          (DR_REG_SLCHOST_BASE + 0x80)
/* HOST_SLCHOST_CONF23 : R/W ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF23  0x000000FF
#define HOST_SLCHOST_CONF23_M  ((HOST_SLCHOST_CONF23_V)<<(HOST_SLCHOST_CONF23_S))
#define HOST_SLCHOST_CONF23_V  0xFF
#define HOST_SLCHOST_CONF23_S  24
/* HOST_SLCHOST_CONF22 : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF22  0x000000FF
#define HOST_SLCHOST_CONF22_M  ((HOST_SLCHOST_CONF22_V)<<(HOST_SLCHOST_CONF22_S))
#define HOST_SLCHOST_CONF22_V  0xFF
#define HOST_SLCHOST_CONF22_S  16
/* HOST_SLCHOST_CONF21 : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF21  0x000000FF
#define HOST_SLCHOST_CONF21_M  ((HOST_SLCHOST_CONF21_V)<<(HOST_SLCHOST_CONF21_S))
#define HOST_SLCHOST_CONF21_V  0xFF
#define HOST_SLCHOST_CONF21_S  8
/* HOST_SLCHOST_CONF20 : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF20  0x000000FF
#define HOST_SLCHOST_CONF20_M  ((HOST_SLCHOST_CONF20_V)<<(HOST_SLCHOST_CONF20_S))
#define HOST_SLCHOST_CONF20_V  0xFF
#define HOST_SLCHOST_CONF20_S  0

#define HOST_SLCHOST_WIN_CMD_REG          (DR_REG_SLCHOST_BASE + 0x84)

#define HOST_SLCHOST_CONF_W6_REG          (DR_REG_SLCHOST_BASE + 0x88)
/* HOST_SLCHOST_CONF27 : R/W ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF27  0x000000FF
#define HOST_SLCHOST_CONF27_M  ((HOST_SLCHOST_CONF27_V)<<(HOST_SLCHOST_CONF27_S))
#define HOST_SLCHOST_CONF27_V  0xFF
#define HOST_SLCHOST_CONF27_S  24
/* HOST_SLCHOST_CONF26 : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF26  0x000000FF
#define HOST_SLCHOST_CONF26_M  ((HOST_SLCHOST_CONF26_V)<<(HOST_SLCHOST_CONF26_S))
#define HOST_SLCHOST_CONF26_V  0xFF
#define HOST_SLCHOST_CONF26_S  16
/* HOST_SLCHOST_CONF25 : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF25  0x000000FF
#define HOST_SLCHOST_CONF25_M  ((HOST_SLCHOST_CONF25_V)<<(HOST_SLCHOST_CONF25_S))
#define HOST_SLCHOST_CONF25_V  0xFF
#define HOST_SLCHOST_CONF25_S  8
/* HOST_SLCHOST_CONF24 : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF24  0x000000FF
#define HOST_SLCHOST_CONF24_M  ((HOST_SLCHOST_CONF24_V)<<(HOST_SLCHOST_CONF24_S))
#define HOST_SLCHOST_CONF24_V  0xFF
#define HOST_SLCHOST_CONF24_S  0

#define HOST_SLCHOST_CONF_W7_REG          (DR_REG_SLCHOST_BASE + 0x8C)
/* HOST_SLCHOST_CONF31 : R/W ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF31  0x000000FF
#define HOST_SLCHOST_CONF31_M  ((HOST_SLCHOST_CONF31_V)<<(HOST_SLCHOST_CONF31_S))
#define HOST_SLCHOST_CONF31_V  0xFF
#define HOST_SLCHOST_CONF31_S  24
/* HOST_SLCHOST_CONF30 : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF30  0x000000FF
#define HOST_SLCHOST_CONF30_M  ((HOST_SLCHOST_CONF30_V)<<(HOST_SLCHOST_CONF30_S))
#define HOST_SLCHOST_CONF30_V  0xFF
#define HOST_SLCHOST_CONF30_S  16
/* HOST_SLCHOST_CONF29 : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF29  0x000000FF
#define HOST_SLCHOST_CONF29_M  ((HOST_SLCHOST_CONF29_V)<<(HOST_SLCHOST_CONF29_S))
#define HOST_SLCHOST_CONF29_V  0xFF
#define HOST_SLCHOST_CONF29_S  8
/* HOST_SLCHOST_CONF28 : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF28  0x000000FF
#define HOST_SLCHOST_CONF28_M  ((HOST_SLCHOST_CONF28_V)<<(HOST_SLCHOST_CONF28_S))
#define HOST_SLCHOST_CONF28_V  0xFF
#define HOST_SLCHOST_CONF28_S  0

#define HOST_SLCHOST_PKT_LEN0_REG          (DR_REG_SLCHOST_BASE + 0x90)
/* HOST_HOSTSLC0_LEN0 : RO ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: */
#define HOST_HOSTSLC0_LEN0  0x000FFFFF
#define HOST_HOSTSLC0_LEN0_M  ((HOST_HOSTSLC0_LEN0_V)<<(HOST_HOSTSLC0_LEN0_S))
#define HOST_HOSTSLC0_LEN0_V  0xFFFFF
#define HOST_HOSTSLC0_LEN0_S  0

#define HOST_SLCHOST_PKT_LEN1_REG          (DR_REG_SLCHOST_BASE + 0x94)
/* HOST_HOSTSLC0_LEN1 : RO ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: */
#define HOST_HOSTSLC0_LEN1  0x000FFFFF
#define HOST_HOSTSLC0_LEN1_M  ((HOST_HOSTSLC0_LEN1_V)<<(HOST_HOSTSLC0_LEN1_S))
#define HOST_HOSTSLC0_LEN1_V  0xFFFFF
#define HOST_HOSTSLC0_LEN1_S  0

#define HOST_SLCHOST_PKT_LEN2_REG          (DR_REG_SLCHOST_BASE + 0x98)
/* HOST_HOSTSLC0_LEN2 : RO ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: */
#define HOST_HOSTSLC0_LEN2  0x000FFFFF
#define HOST_HOSTSLC0_LEN2_M  ((HOST_HOSTSLC0_LEN2_V)<<(HOST_HOSTSLC0_LEN2_S))
#define HOST_HOSTSLC0_LEN2_V  0xFFFFF
#define HOST_HOSTSLC0_LEN2_S  0

#define HOST_SLCHOST_CONF_W8_REG          (DR_REG_SLCHOST_BASE + 0x9C)
/* HOST_SLCHOST_CONF35 : R/W ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF35  0x000000FF
#define HOST_SLCHOST_CONF35_M  ((HOST_SLCHOST_CONF35_V)<<(HOST_SLCHOST_CONF35_S))
#define HOST_SLCHOST_CONF35_V  0xFF
#define HOST_SLCHOST_CONF35_S  24
/* HOST_SLCHOST_CONF34 : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF34  0x000000FF
#define HOST_SLCHOST_CONF34_M  ((HOST_SLCHOST_CONF34_V)<<(HOST_SLCHOST_CONF34_S))
#define HOST_SLCHOST_CONF34_V  0xFF
#define HOST_SLCHOST_CONF34_S  16
/* HOST_SLCHOST_CONF33 : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF33  0x000000FF
#define HOST_SLCHOST_CONF33_M  ((HOST_SLCHOST_CONF33_V)<<(HOST_SLCHOST_CONF33_S))
#define HOST_SLCHOST_CONF33_V  0xFF
#define HOST_SLCHOST_CONF33_S  8
/* HOST_SLCHOST_CONF32 : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF32  0x000000FF
#define HOST_SLCHOST_CONF32_M  ((HOST_SLCHOST_CONF32_V)<<(HOST_SLCHOST_CONF32_S))
#define HOST_SLCHOST_CONF32_V  0xFF
#define HOST_SLCHOST_CONF32_S  0

#define HOST_SLCHOST_CONF_W9_REG          (DR_REG_SLCHOST_BASE + 0xA0)
/* HOST_SLCHOST_CONF39 : R/W ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF39  0x000000FF
#define HOST_SLCHOST_CONF39_M  ((HOST_SLCHOST_CONF39_V)<<(HOST_SLCHOST_CONF39_S))
#define HOST_SLCHOST_CONF39_V  0xFF
#define HOST_SLCHOST_CONF39_S  24
/* HOST_SLCHOST_CONF38 : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF38  0x000000FF
#define HOST_SLCHOST_CONF38_M  ((HOST_SLCHOST_CONF38_V)<<(HOST_SLCHOST_CONF38_S))
#define HOST_SLCHOST_CONF38_V  0xFF
#define HOST_SLCHOST_CONF38_S  16
/* HOST_SLCHOST_CONF37 : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF37  0x000000FF
#define HOST_SLCHOST_CONF37_M  ((HOST_SLCHOST_CONF37_V)<<(HOST_SLCHOST_CONF37_S))
#define HOST_SLCHOST_CONF37_V  0xFF
#define HOST_SLCHOST_CONF37_S  8
/* HOST_SLCHOST_CONF36 : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF36  0x000000FF
#define HOST_SLCHOST_CONF36_M  ((HOST_SLCHOST_CONF36_V)<<(HOST_SLCHOST_CONF36_S))
#define HOST_SLCHOST_CONF36_V  0xFF
#define HOST_SLCHOST_CONF36_S  0

#define HOST_SLCHOST_CONF_W10_REG          (DR_REG_SLCHOST_BASE + 0xA4)
/* HOST_SLCHOST_CONF43 : R/W ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF43  0x000000FF
#define HOST_SLCHOST_CONF43_M  ((HOST_SLCHOST_CONF43_V)<<(HOST_SLCHOST_CONF43_S))
#define HOST_SLCHOST_CONF43_V  0xFF
#define HOST_SLCHOST_CONF43_S  24
/* HOST_SLCHOST_CONF42 : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF42  0x000000FF
#define HOST_SLCHOST_CONF42_M  ((HOST_SLCHOST_CONF42_V)<<(HOST_SLCHOST_CONF42_S))
#define HOST_SLCHOST_CONF42_V  0xFF
#define HOST_SLCHOST_CONF42_S  16
/* HOST_SLCHOST_CONF41 : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF41  0x000000FF
#define HOST_SLCHOST_CONF41_M  ((HOST_SLCHOST_CONF41_V)<<(HOST_SLCHOST_CONF41_S))
#define HOST_SLCHOST_CONF41_V  0xFF
#define HOST_SLCHOST_CONF41_S  8
/* HOST_SLCHOST_CONF40 : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF40  0x000000FF
#define HOST_SLCHOST_CONF40_M  ((HOST_SLCHOST_CONF40_V)<<(HOST_SLCHOST_CONF40_S))
#define HOST_SLCHOST_CONF40_V  0xFF
#define HOST_SLCHOST_CONF40_S  0

#define HOST_SLCHOST_CONF_W11_REG          (DR_REG_SLCHOST_BASE + 0xA8)
/* HOST_SLCHOST_CONF47 : R/W ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF47  0x000000FF
#define HOST_SLCHOST_CONF47_M  ((HOST_SLCHOST_CONF47_V)<<(HOST_SLCHOST_CONF47_S))
#define HOST_SLCHOST_CONF47_V  0xFF
#define HOST_SLCHOST_CONF47_S  24
/* HOST_SLCHOST_CONF46 : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF46  0x000000FF
#define HOST_SLCHOST_CONF46_M  ((HOST_SLCHOST_CONF46_V)<<(HOST_SLCHOST_CONF46_S))
#define HOST_SLCHOST_CONF46_V  0xFF
#define HOST_SLCHOST_CONF46_S  16
/* HOST_SLCHOST_CONF45 : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF45  0x000000FF
#define HOST_SLCHOST_CONF45_M  ((HOST_SLCHOST_CONF45_V)<<(HOST_SLCHOST_CONF45_S))
#define HOST_SLCHOST_CONF45_V  0xFF
#define HOST_SLCHOST_CONF45_S  8
/* HOST_SLCHOST_CONF44 : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF44  0x000000FF
#define HOST_SLCHOST_CONF44_M  ((HOST_SLCHOST_CONF44_V)<<(HOST_SLCHOST_CONF44_S))
#define HOST_SLCHOST_CONF44_V  0xFF
#define HOST_SLCHOST_CONF44_S  0

#define HOST_SLCHOST_CONF_W12_REG          (DR_REG_SLCHOST_BASE + 0xAC)
/* HOST_SLCHOST_CONF51 : R/W ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF51  0x000000FF
#define HOST_SLCHOST_CONF51_M  ((HOST_SLCHOST_CONF51_V)<<(HOST_SLCHOST_CONF51_S))
#define HOST_SLCHOST_CONF51_V  0xFF
#define HOST_SLCHOST_CONF51_S  24
/* HOST_SLCHOST_CONF50 : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF50  0x000000FF
#define HOST_SLCHOST_CONF50_M  ((HOST_SLCHOST_CONF50_V)<<(HOST_SLCHOST_CONF50_S))
#define HOST_SLCHOST_CONF50_V  0xFF
#define HOST_SLCHOST_CONF50_S  16
/* HOST_SLCHOST_CONF49 : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF49  0x000000FF
#define HOST_SLCHOST_CONF49_M  ((HOST_SLCHOST_CONF49_V)<<(HOST_SLCHOST_CONF49_S))
#define HOST_SLCHOST_CONF49_V  0xFF
#define HOST_SLCHOST_CONF49_S  8
/* HOST_SLCHOST_CONF48 : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF48  0x000000FF
#define HOST_SLCHOST_CONF48_M  ((HOST_SLCHOST_CONF48_V)<<(HOST_SLCHOST_CONF48_S))
#define HOST_SLCHOST_CONF48_V  0xFF
#define HOST_SLCHOST_CONF48_S  0

#define HOST_SLCHOST_CONF_W13_REG          (DR_REG_SLCHOST_BASE + 0xB0)
/* HOST_SLCHOST_CONF55 : R/W ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF55  0x000000FF
#define HOST_SLCHOST_CONF55_M  ((HOST_SLCHOST_CONF55_V)<<(HOST_SLCHOST_CONF55_S))
#define HOST_SLCHOST_CONF55_V  0xFF
#define HOST_SLCHOST_CONF55_S  24
/* HOST_SLCHOST_CONF54 : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF54  0x000000FF
#define HOST_SLCHOST_CONF54_M  ((HOST_SLCHOST_CONF54_V)<<(HOST_SLCHOST_CONF54_S))
#define HOST_SLCHOST_CONF54_V  0xFF
#define HOST_SLCHOST_CONF54_S  16
/* HOST_SLCHOST_CONF53 : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF53  0x000000FF
#define HOST_SLCHOST_CONF53_M  ((HOST_SLCHOST_CONF53_V)<<(HOST_SLCHOST_CONF53_S))
#define HOST_SLCHOST_CONF53_V  0xFF
#define HOST_SLCHOST_CONF53_S  8
/* HOST_SLCHOST_CONF52 : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF52  0x000000FF
#define HOST_SLCHOST_CONF52_M  ((HOST_SLCHOST_CONF52_V)<<(HOST_SLCHOST_CONF52_S))
#define HOST_SLCHOST_CONF52_V  0xFF
#define HOST_SLCHOST_CONF52_S  0

#define HOST_SLCHOST_CONF_W14_REG          (DR_REG_SLCHOST_BASE + 0xB4)
/* HOST_SLCHOST_CONF59 : R/W ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF59  0x000000FF
#define HOST_SLCHOST_CONF59_M  ((HOST_SLCHOST_CONF59_V)<<(HOST_SLCHOST_CONF59_S))
#define HOST_SLCHOST_CONF59_V  0xFF
#define HOST_SLCHOST_CONF59_S  24
/* HOST_SLCHOST_CONF58 : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF58  0x000000FF
#define HOST_SLCHOST_CONF58_M  ((HOST_SLCHOST_CONF58_V)<<(HOST_SLCHOST_CONF58_S))
#define HOST_SLCHOST_CONF58_V  0xFF
#define HOST_SLCHOST_CONF58_S  16
/* HOST_SLCHOST_CONF57 : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF57  0x000000FF
#define HOST_SLCHOST_CONF57_M  ((HOST_SLCHOST_CONF57_V)<<(HOST_SLCHOST_CONF57_S))
#define HOST_SLCHOST_CONF57_V  0xFF
#define HOST_SLCHOST_CONF57_S  8
/* HOST_SLCHOST_CONF56 : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF56  0x000000FF
#define HOST_SLCHOST_CONF56_M  ((HOST_SLCHOST_CONF56_V)<<(HOST_SLCHOST_CONF56_S))
#define HOST_SLCHOST_CONF56_V  0xFF
#define HOST_SLCHOST_CONF56_S  0

#define HOST_SLCHOST_CONF_W15_REG          (DR_REG_SLCHOST_BASE + 0xB8)
/* HOST_SLCHOST_CONF63 : R/W ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF63  0x000000FF
#define HOST_SLCHOST_CONF63_M  ((HOST_SLCHOST_CONF63_V)<<(HOST_SLCHOST_CONF63_S))
#define HOST_SLCHOST_CONF63_V  0xFF
#define HOST_SLCHOST_CONF63_S  24
/* HOST_SLCHOST_CONF62 : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF62  0x000000FF
#define HOST_SLCHOST_CONF62_M  ((HOST_SLCHOST_CONF62_V)<<(HOST_SLCHOST_CONF62_S))
#define HOST_SLCHOST_CONF62_V  0xFF
#define HOST_SLCHOST_CONF62_S  16
/* HOST_SLCHOST_CONF61 : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF61  0x000000FF
#define HOST_SLCHOST_CONF61_M  ((HOST_SLCHOST_CONF61_V)<<(HOST_SLCHOST_CONF61_S))
#define HOST_SLCHOST_CONF61_V  0xFF
#define HOST_SLCHOST_CONF61_S  8
/* HOST_SLCHOST_CONF60 : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define HOST_SLCHOST_CONF60  0x000000FF
#define HOST_SLCHOST_CONF60_M  ((HOST_SLCHOST_CONF60_V)<<(HOST_SLCHOST_CONF60_S))
#define HOST_SLCHOST_CONF60_V  0xFF
#define HOST_SLCHOST_CONF60_S  0

#define HOST_SLCHOST_CHECK_SUM0_REG          (DR_REG_SLCHOST_BASE + 0xBC)
/* HOST_SLCHOST_CHECK_SUM0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define HOST_SLCHOST_CHECK_SUM0  0xFFFFFFFF
#define HOST_SLCHOST_CHECK_SUM0_M  ((HOST_SLCHOST_CHECK_SUM0_V)<<(HOST_SLCHOST_CHECK_SUM0_S))
#define HOST_SLCHOST_CHECK_SUM0_V  0xFFFFFFFF
#define HOST_SLCHOST_CHECK_SUM0_S  0

#define HOST_SLCHOST_CHECK_SUM1_REG          (DR_REG_SLCHOST_BASE + 0xC0)
/* HOST_SLCHOST_CHECK_SUM1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define HOST_SLCHOST_CHECK_SUM1  0xFFFFFFFF
#define HOST_SLCHOST_CHECK_SUM1_M  ((HOST_SLCHOST_CHECK_SUM1_V)<<(HOST_SLCHOST_CHECK_SUM1_S))
#define HOST_SLCHOST_CHECK_SUM1_V  0xFFFFFFFF
#define HOST_SLCHOST_CHECK_SUM1_S  0

#define HOST_SLC1HOST_TOKEN_RDATA_REG          (DR_REG_SLCHOST_BASE + 0xC4)
/* HOST_SLC1_RX_PF_EOF : RO ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: */
#define HOST_SLC1_RX_PF_EOF  0x0000000F
#define HOST_SLC1_RX_PF_EOF_M  ((HOST_SLC1_RX_PF_EOF_V)<<(HOST_SLC1_RX_PF_EOF_S))
#define HOST_SLC1_RX_PF_EOF_V  0xF
#define HOST_SLC1_RX_PF_EOF_S  28
/* HOST_HOSTSLC1_TOKEN1 : RO ;bitpos:[27:16] ;default: 12'h0 ; */
/*description: */
#define HOST_HOSTSLC1_TOKEN1  0x00000FFF
#define HOST_HOSTSLC1_TOKEN1_M  ((HOST_HOSTSLC1_TOKEN1_V)<<(HOST_HOSTSLC1_TOKEN1_S))
#define HOST_HOSTSLC1_TOKEN1_V  0xFFF
#define HOST_HOSTSLC1_TOKEN1_S  16
/* HOST_SLC1_RX_PF_VALID : RO ;bitpos:[12] ;default: 1'h0 ; */
/*description: */
#define HOST_SLC1_RX_PF_VALID  (BIT(12))
#define HOST_SLC1_RX_PF_VALID_M  (BIT(12))
#define HOST_SLC1_RX_PF_VALID_V  0x1
#define HOST_SLC1_RX_PF_VALID_S  12
/* HOST_SLC1_TOKEN0 : RO ;bitpos:[11:0] ;default: 12'h0 ; */
/*description: */
#define HOST_SLC1_TOKEN0  0x00000FFF
#define HOST_SLC1_TOKEN0_M  ((HOST_SLC1_TOKEN0_V)<<(HOST_SLC1_TOKEN0_S))
#define HOST_SLC1_TOKEN0_V  0xFFF
#define HOST_SLC1_TOKEN0_S  0

#define HOST_SLC0HOST_TOKEN_WDATA_REG          (DR_REG_SLCHOST_BASE + 0xC8)
/* HOST_SLC0HOST_TOKEN1_WD : R/W ;bitpos:[27:16] ;default: 12'h0 ; */
/*description: */
#define HOST_SLC0HOST_TOKEN1_WD  0x00000FFF
#define HOST_SLC0HOST_TOKEN1_WD_M  ((HOST_SLC0HOST_TOKEN1_WD_V)<<(HOST_SLC0HOST_TOKEN1_WD_S))
#define HOST_SLC0HOST_TOKEN1_WD_V  0xFFF
#define HOST_SLC0HOST_TOKEN1_WD_S  16
/* HOST_SLC0HOST_TOKEN0_WD : R/W ;bitpos:[11:0] ;default: 12'h0 ; */
/*description: */
#define HOST_SLC0HOST_TOKEN0_WD  0x00000FFF
#define HOST_SLC0HOST_TOKEN0_WD_M  ((HOST_SLC0HOST_TOKEN0_WD_V)<<(HOST_SLC0HOST_TOKEN0_WD_S))
#define HOST_SLC0HOST_TOKEN0_WD_V  0xFFF
#define HOST_SLC0HOST_TOKEN0_WD_S  0

#define HOST_SLC1HOST_TOKEN_WDATA_REG          (DR_REG_SLCHOST_BASE + 0xCC)
/* HOST_SLC1HOST_TOKEN1_WD : R/W ;bitpos:[27:16] ;default: 12'h0 ; */
/*description: */
#define HOST_SLC1HOST_TOKEN1_WD  0x00000FFF
#define HOST_SLC1HOST_TOKEN1_WD_M  ((HOST_SLC1HOST_TOKEN1_WD_V)<<(HOST_SLC1HOST_TOKEN1_WD_S))
#define HOST_SLC1HOST_TOKEN1_WD_V  0xFFF
#define HOST_SLC1HOST_TOKEN1_WD_S  16
/* HOST_SLC1HOST_TOKEN0_WD : R/W ;bitpos:[11:0] ;default: 12'h0 ; */
/*description: */
#define HOST_SLC1HOST_TOKEN0_WD  0x00000FFF
#define HOST_SLC1HOST_TOKEN0_WD_M  ((HOST_SLC1HOST_TOKEN0_WD_V)<<(HOST_SLC1HOST_TOKEN0_WD_S))
#define HOST_SLC1HOST_TOKEN0_WD_V  0xFFF
#define HOST_SLC1HOST_TOKEN0_WD_S  0

#define HOST_SLCHOST_TOKEN_CON_REG          (DR_REG_SLCHOST_BASE + 0xD0)
/* HOST_SLC0HOST_LEN_WR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_LEN_WR  (BIT(8))
#define HOST_SLC0HOST_LEN_WR_M  (BIT(8))
#define HOST_SLC0HOST_LEN_WR_V  0x1
#define HOST_SLC0HOST_LEN_WR_S  8
/* HOST_SLC1HOST_TOKEN1_WR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_TOKEN1_WR  (BIT(7))
#define HOST_SLC1HOST_TOKEN1_WR_M  (BIT(7))
#define HOST_SLC1HOST_TOKEN1_WR_V  0x1
#define HOST_SLC1HOST_TOKEN1_WR_S  7
/* HOST_SLC1HOST_TOKEN0_WR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_TOKEN0_WR  (BIT(6))
#define HOST_SLC1HOST_TOKEN0_WR_M  (BIT(6))
#define HOST_SLC1HOST_TOKEN0_WR_V  0x1
#define HOST_SLC1HOST_TOKEN0_WR_S  6
/* HOST_SLC1HOST_TOKEN1_DEC : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_TOKEN1_DEC  (BIT(5))
#define HOST_SLC1HOST_TOKEN1_DEC_M  (BIT(5))
#define HOST_SLC1HOST_TOKEN1_DEC_V  0x1
#define HOST_SLC1HOST_TOKEN1_DEC_S  5
/* HOST_SLC1HOST_TOKEN0_DEC : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_TOKEN0_DEC  (BIT(4))
#define HOST_SLC1HOST_TOKEN0_DEC_M  (BIT(4))
#define HOST_SLC1HOST_TOKEN0_DEC_V  0x1
#define HOST_SLC1HOST_TOKEN0_DEC_S  4
/* HOST_SLC0HOST_TOKEN1_WR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_TOKEN1_WR  (BIT(3))
#define HOST_SLC0HOST_TOKEN1_WR_M  (BIT(3))
#define HOST_SLC0HOST_TOKEN1_WR_V  0x1
#define HOST_SLC0HOST_TOKEN1_WR_S  3
/* HOST_SLC0HOST_TOKEN0_WR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_TOKEN0_WR  (BIT(2))
#define HOST_SLC0HOST_TOKEN0_WR_M  (BIT(2))
#define HOST_SLC0HOST_TOKEN0_WR_V  0x1
#define HOST_SLC0HOST_TOKEN0_WR_S  2
/* HOST_SLC0HOST_TOKEN1_DEC : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_TOKEN1_DEC  (BIT(1))
#define HOST_SLC0HOST_TOKEN1_DEC_M  (BIT(1))
#define HOST_SLC0HOST_TOKEN1_DEC_V  0x1
#define HOST_SLC0HOST_TOKEN1_DEC_S  1
/* HOST_SLC0HOST_TOKEN0_DEC : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_TOKEN0_DEC  (BIT(0))
#define HOST_SLC0HOST_TOKEN0_DEC_M  (BIT(0))
#define HOST_SLC0HOST_TOKEN0_DEC_V  0x1
#define HOST_SLC0HOST_TOKEN0_DEC_S  0

#define HOST_SLC0HOST_INT_CLR_REG          (DR_REG_SLCHOST_BASE + 0xD4)
/* HOST_GPIO_SDIO_INT_CLR : WO ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define HOST_GPIO_SDIO_INT_CLR  (BIT(25))
#define HOST_GPIO_SDIO_INT_CLR_M  (BIT(25))
#define HOST_GPIO_SDIO_INT_CLR_V  0x1
#define HOST_GPIO_SDIO_INT_CLR_S  25
/* HOST_SLC0_HOST_RD_RETRY_INT_CLR : WO ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_HOST_RD_RETRY_INT_CLR  (BIT(24))
#define HOST_SLC0_HOST_RD_RETRY_INT_CLR_M  (BIT(24))
#define HOST_SLC0_HOST_RD_RETRY_INT_CLR_V  0x1
#define HOST_SLC0_HOST_RD_RETRY_INT_CLR_S  24
/* HOST_SLC0_RX_NEW_PACKET_INT_CLR : WO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_RX_NEW_PACKET_INT_CLR  (BIT(23))
#define HOST_SLC0_RX_NEW_PACKET_INT_CLR_M  (BIT(23))
#define HOST_SLC0_RX_NEW_PACKET_INT_CLR_V  0x1
#define HOST_SLC0_RX_NEW_PACKET_INT_CLR_S  23
/* HOST_SLC0_EXT_BIT3_INT_CLR : WO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_EXT_BIT3_INT_CLR  (BIT(22))
#define HOST_SLC0_EXT_BIT3_INT_CLR_M  (BIT(22))
#define HOST_SLC0_EXT_BIT3_INT_CLR_V  0x1
#define HOST_SLC0_EXT_BIT3_INT_CLR_S  22
/* HOST_SLC0_EXT_BIT2_INT_CLR : WO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_EXT_BIT2_INT_CLR  (BIT(21))
#define HOST_SLC0_EXT_BIT2_INT_CLR_M  (BIT(21))
#define HOST_SLC0_EXT_BIT2_INT_CLR_V  0x1
#define HOST_SLC0_EXT_BIT2_INT_CLR_S  21
/* HOST_SLC0_EXT_BIT1_INT_CLR : WO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_EXT_BIT1_INT_CLR  (BIT(20))
#define HOST_SLC0_EXT_BIT1_INT_CLR_M  (BIT(20))
#define HOST_SLC0_EXT_BIT1_INT_CLR_V  0x1
#define HOST_SLC0_EXT_BIT1_INT_CLR_S  20
/* HOST_SLC0_EXT_BIT0_INT_CLR : WO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_EXT_BIT0_INT_CLR  (BIT(19))
#define HOST_SLC0_EXT_BIT0_INT_CLR_M  (BIT(19))
#define HOST_SLC0_EXT_BIT0_INT_CLR_V  0x1
#define HOST_SLC0_EXT_BIT0_INT_CLR_S  19
/* HOST_SLC0_RX_PF_VALID_INT_CLR : WO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_RX_PF_VALID_INT_CLR  (BIT(18))
#define HOST_SLC0_RX_PF_VALID_INT_CLR_M  (BIT(18))
#define HOST_SLC0_RX_PF_VALID_INT_CLR_V  0x1
#define HOST_SLC0_RX_PF_VALID_INT_CLR_S  18
/* HOST_SLC0_TX_OVF_INT_CLR : WO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TX_OVF_INT_CLR  (BIT(17))
#define HOST_SLC0_TX_OVF_INT_CLR_M  (BIT(17))
#define HOST_SLC0_TX_OVF_INT_CLR_V  0x1
#define HOST_SLC0_TX_OVF_INT_CLR_S  17
/* HOST_SLC0_RX_UDF_INT_CLR : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_RX_UDF_INT_CLR  (BIT(16))
#define HOST_SLC0_RX_UDF_INT_CLR_M  (BIT(16))
#define HOST_SLC0_RX_UDF_INT_CLR_V  0x1
#define HOST_SLC0_RX_UDF_INT_CLR_S  16
/* HOST_SLC0HOST_TX_START_INT_CLR : WO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_TX_START_INT_CLR  (BIT(15))
#define HOST_SLC0HOST_TX_START_INT_CLR_M  (BIT(15))
#define HOST_SLC0HOST_TX_START_INT_CLR_V  0x1
#define HOST_SLC0HOST_TX_START_INT_CLR_S  15
/* HOST_SLC0HOST_RX_START_INT_CLR : WO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_RX_START_INT_CLR  (BIT(14))
#define HOST_SLC0HOST_RX_START_INT_CLR_M  (BIT(14))
#define HOST_SLC0HOST_RX_START_INT_CLR_V  0x1
#define HOST_SLC0HOST_RX_START_INT_CLR_S  14
/* HOST_SLC0HOST_RX_EOF_INT_CLR : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_RX_EOF_INT_CLR  (BIT(13))
#define HOST_SLC0HOST_RX_EOF_INT_CLR_M  (BIT(13))
#define HOST_SLC0HOST_RX_EOF_INT_CLR_V  0x1
#define HOST_SLC0HOST_RX_EOF_INT_CLR_S  13
/* HOST_SLC0HOST_RX_SOF_INT_CLR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_RX_SOF_INT_CLR  (BIT(12))
#define HOST_SLC0HOST_RX_SOF_INT_CLR_M  (BIT(12))
#define HOST_SLC0HOST_RX_SOF_INT_CLR_V  0x1
#define HOST_SLC0HOST_RX_SOF_INT_CLR_S  12
/* HOST_SLC0_TOKEN1_0TO1_INT_CLR : WO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOKEN1_0TO1_INT_CLR  (BIT(11))
#define HOST_SLC0_TOKEN1_0TO1_INT_CLR_M  (BIT(11))
#define HOST_SLC0_TOKEN1_0TO1_INT_CLR_V  0x1
#define HOST_SLC0_TOKEN1_0TO1_INT_CLR_S  11
/* HOST_SLC0_TOKEN0_0TO1_INT_CLR : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOKEN0_0TO1_INT_CLR  (BIT(10))
#define HOST_SLC0_TOKEN0_0TO1_INT_CLR_M  (BIT(10))
#define HOST_SLC0_TOKEN0_0TO1_INT_CLR_V  0x1
#define HOST_SLC0_TOKEN0_0TO1_INT_CLR_S  10
/* HOST_SLC0_TOKEN1_1TO0_INT_CLR : WO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOKEN1_1TO0_INT_CLR  (BIT(9))
#define HOST_SLC0_TOKEN1_1TO0_INT_CLR_M  (BIT(9))
#define HOST_SLC0_TOKEN1_1TO0_INT_CLR_V  0x1
#define HOST_SLC0_TOKEN1_1TO0_INT_CLR_S  9
/* HOST_SLC0_TOKEN0_1TO0_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOKEN0_1TO0_INT_CLR  (BIT(8))
#define HOST_SLC0_TOKEN0_1TO0_INT_CLR_M  (BIT(8))
#define HOST_SLC0_TOKEN0_1TO0_INT_CLR_V  0x1
#define HOST_SLC0_TOKEN0_1TO0_INT_CLR_S  8
/* HOST_SLC0_TOHOST_BIT7_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT7_INT_CLR  (BIT(7))
#define HOST_SLC0_TOHOST_BIT7_INT_CLR_M  (BIT(7))
#define HOST_SLC0_TOHOST_BIT7_INT_CLR_V  0x1
#define HOST_SLC0_TOHOST_BIT7_INT_CLR_S  7
/* HOST_SLC0_TOHOST_BIT6_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT6_INT_CLR  (BIT(6))
#define HOST_SLC0_TOHOST_BIT6_INT_CLR_M  (BIT(6))
#define HOST_SLC0_TOHOST_BIT6_INT_CLR_V  0x1
#define HOST_SLC0_TOHOST_BIT6_INT_CLR_S  6
/* HOST_SLC0_TOHOST_BIT5_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT5_INT_CLR  (BIT(5))
#define HOST_SLC0_TOHOST_BIT5_INT_CLR_M  (BIT(5))
#define HOST_SLC0_TOHOST_BIT5_INT_CLR_V  0x1
#define HOST_SLC0_TOHOST_BIT5_INT_CLR_S  5
/* HOST_SLC0_TOHOST_BIT4_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT4_INT_CLR  (BIT(4))
#define HOST_SLC0_TOHOST_BIT4_INT_CLR_M  (BIT(4))
#define HOST_SLC0_TOHOST_BIT4_INT_CLR_V  0x1
#define HOST_SLC0_TOHOST_BIT4_INT_CLR_S  4
/* HOST_SLC0_TOHOST_BIT3_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT3_INT_CLR  (BIT(3))
#define HOST_SLC0_TOHOST_BIT3_INT_CLR_M  (BIT(3))
#define HOST_SLC0_TOHOST_BIT3_INT_CLR_V  0x1
#define HOST_SLC0_TOHOST_BIT3_INT_CLR_S  3
/* HOST_SLC0_TOHOST_BIT2_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT2_INT_CLR  (BIT(2))
#define HOST_SLC0_TOHOST_BIT2_INT_CLR_M  (BIT(2))
#define HOST_SLC0_TOHOST_BIT2_INT_CLR_V  0x1
#define HOST_SLC0_TOHOST_BIT2_INT_CLR_S  2
/* HOST_SLC0_TOHOST_BIT1_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT1_INT_CLR  (BIT(1))
#define HOST_SLC0_TOHOST_BIT1_INT_CLR_M  (BIT(1))
#define HOST_SLC0_TOHOST_BIT1_INT_CLR_V  0x1
#define HOST_SLC0_TOHOST_BIT1_INT_CLR_S  1
/* HOST_SLC0_TOHOST_BIT0_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT0_INT_CLR  (BIT(0))
#define HOST_SLC0_TOHOST_BIT0_INT_CLR_M  (BIT(0))
#define HOST_SLC0_TOHOST_BIT0_INT_CLR_V  0x1
#define HOST_SLC0_TOHOST_BIT0_INT_CLR_S  0

#define HOST_SLC1HOST_INT_CLR_REG          (DR_REG_SLCHOST_BASE + 0xD8)
/* HOST_SLC1_BT_RX_NEW_PACKET_INT_CLR : WO ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_BT_RX_NEW_PACKET_INT_CLR  (BIT(25))
#define HOST_SLC1_BT_RX_NEW_PACKET_INT_CLR_M  (BIT(25))
#define HOST_SLC1_BT_RX_NEW_PACKET_INT_CLR_V  0x1
#define HOST_SLC1_BT_RX_NEW_PACKET_INT_CLR_S  25
/* HOST_SLC1_HOST_RD_RETRY_INT_CLR : WO ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_HOST_RD_RETRY_INT_CLR  (BIT(24))
#define HOST_SLC1_HOST_RD_RETRY_INT_CLR_M  (BIT(24))
#define HOST_SLC1_HOST_RD_RETRY_INT_CLR_V  0x1
#define HOST_SLC1_HOST_RD_RETRY_INT_CLR_S  24
/* HOST_SLC1_WIFI_RX_NEW_PACKET_INT_CLR : WO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_WIFI_RX_NEW_PACKET_INT_CLR  (BIT(23))
#define HOST_SLC1_WIFI_RX_NEW_PACKET_INT_CLR_M  (BIT(23))
#define HOST_SLC1_WIFI_RX_NEW_PACKET_INT_CLR_V  0x1
#define HOST_SLC1_WIFI_RX_NEW_PACKET_INT_CLR_S  23
/* HOST_SLC1_EXT_BIT3_INT_CLR : WO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_EXT_BIT3_INT_CLR  (BIT(22))
#define HOST_SLC1_EXT_BIT3_INT_CLR_M  (BIT(22))
#define HOST_SLC1_EXT_BIT3_INT_CLR_V  0x1
#define HOST_SLC1_EXT_BIT3_INT_CLR_S  22
/* HOST_SLC1_EXT_BIT2_INT_CLR : WO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_EXT_BIT2_INT_CLR  (BIT(21))
#define HOST_SLC1_EXT_BIT2_INT_CLR_M  (BIT(21))
#define HOST_SLC1_EXT_BIT2_INT_CLR_V  0x1
#define HOST_SLC1_EXT_BIT2_INT_CLR_S  21
/* HOST_SLC1_EXT_BIT1_INT_CLR : WO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_EXT_BIT1_INT_CLR  (BIT(20))
#define HOST_SLC1_EXT_BIT1_INT_CLR_M  (BIT(20))
#define HOST_SLC1_EXT_BIT1_INT_CLR_V  0x1
#define HOST_SLC1_EXT_BIT1_INT_CLR_S  20
/* HOST_SLC1_EXT_BIT0_INT_CLR : WO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_EXT_BIT0_INT_CLR  (BIT(19))
#define HOST_SLC1_EXT_BIT0_INT_CLR_M  (BIT(19))
#define HOST_SLC1_EXT_BIT0_INT_CLR_V  0x1
#define HOST_SLC1_EXT_BIT0_INT_CLR_S  19
/* HOST_SLC1_RX_PF_VALID_INT_CLR : WO ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_RX_PF_VALID_INT_CLR  (BIT(18))
#define HOST_SLC1_RX_PF_VALID_INT_CLR_M  (BIT(18))
#define HOST_SLC1_RX_PF_VALID_INT_CLR_V  0x1
#define HOST_SLC1_RX_PF_VALID_INT_CLR_S  18
/* HOST_SLC1_TX_OVF_INT_CLR : WO ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TX_OVF_INT_CLR  (BIT(17))
#define HOST_SLC1_TX_OVF_INT_CLR_M  (BIT(17))
#define HOST_SLC1_TX_OVF_INT_CLR_V  0x1
#define HOST_SLC1_TX_OVF_INT_CLR_S  17
/* HOST_SLC1_RX_UDF_INT_CLR : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_RX_UDF_INT_CLR  (BIT(16))
#define HOST_SLC1_RX_UDF_INT_CLR_M  (BIT(16))
#define HOST_SLC1_RX_UDF_INT_CLR_V  0x1
#define HOST_SLC1_RX_UDF_INT_CLR_S  16
/* HOST_SLC1HOST_TX_START_INT_CLR : WO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_TX_START_INT_CLR  (BIT(15))
#define HOST_SLC1HOST_TX_START_INT_CLR_M  (BIT(15))
#define HOST_SLC1HOST_TX_START_INT_CLR_V  0x1
#define HOST_SLC1HOST_TX_START_INT_CLR_S  15
/* HOST_SLC1HOST_RX_START_INT_CLR : WO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_RX_START_INT_CLR  (BIT(14))
#define HOST_SLC1HOST_RX_START_INT_CLR_M  (BIT(14))
#define HOST_SLC1HOST_RX_START_INT_CLR_V  0x1
#define HOST_SLC1HOST_RX_START_INT_CLR_S  14
/* HOST_SLC1HOST_RX_EOF_INT_CLR : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_RX_EOF_INT_CLR  (BIT(13))
#define HOST_SLC1HOST_RX_EOF_INT_CLR_M  (BIT(13))
#define HOST_SLC1HOST_RX_EOF_INT_CLR_V  0x1
#define HOST_SLC1HOST_RX_EOF_INT_CLR_S  13
/* HOST_SLC1HOST_RX_SOF_INT_CLR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_RX_SOF_INT_CLR  (BIT(12))
#define HOST_SLC1HOST_RX_SOF_INT_CLR_M  (BIT(12))
#define HOST_SLC1HOST_RX_SOF_INT_CLR_V  0x1
#define HOST_SLC1HOST_RX_SOF_INT_CLR_S  12
/* HOST_SLC1_TOKEN1_0TO1_INT_CLR : WO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOKEN1_0TO1_INT_CLR  (BIT(11))
#define HOST_SLC1_TOKEN1_0TO1_INT_CLR_M  (BIT(11))
#define HOST_SLC1_TOKEN1_0TO1_INT_CLR_V  0x1
#define HOST_SLC1_TOKEN1_0TO1_INT_CLR_S  11
/* HOST_SLC1_TOKEN0_0TO1_INT_CLR : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOKEN0_0TO1_INT_CLR  (BIT(10))
#define HOST_SLC1_TOKEN0_0TO1_INT_CLR_M  (BIT(10))
#define HOST_SLC1_TOKEN0_0TO1_INT_CLR_V  0x1
#define HOST_SLC1_TOKEN0_0TO1_INT_CLR_S  10
/* HOST_SLC1_TOKEN1_1TO0_INT_CLR : WO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOKEN1_1TO0_INT_CLR  (BIT(9))
#define HOST_SLC1_TOKEN1_1TO0_INT_CLR_M  (BIT(9))
#define HOST_SLC1_TOKEN1_1TO0_INT_CLR_V  0x1
#define HOST_SLC1_TOKEN1_1TO0_INT_CLR_S  9
/* HOST_SLC1_TOKEN0_1TO0_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOKEN0_1TO0_INT_CLR  (BIT(8))
#define HOST_SLC1_TOKEN0_1TO0_INT_CLR_M  (BIT(8))
#define HOST_SLC1_TOKEN0_1TO0_INT_CLR_V  0x1
#define HOST_SLC1_TOKEN0_1TO0_INT_CLR_S  8
/* HOST_SLC1_TOHOST_BIT7_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT7_INT_CLR  (BIT(7))
#define HOST_SLC1_TOHOST_BIT7_INT_CLR_M  (BIT(7))
#define HOST_SLC1_TOHOST_BIT7_INT_CLR_V  0x1
#define HOST_SLC1_TOHOST_BIT7_INT_CLR_S  7
/* HOST_SLC1_TOHOST_BIT6_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT6_INT_CLR  (BIT(6))
#define HOST_SLC1_TOHOST_BIT6_INT_CLR_M  (BIT(6))
#define HOST_SLC1_TOHOST_BIT6_INT_CLR_V  0x1
#define HOST_SLC1_TOHOST_BIT6_INT_CLR_S  6
/* HOST_SLC1_TOHOST_BIT5_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT5_INT_CLR  (BIT(5))
#define HOST_SLC1_TOHOST_BIT5_INT_CLR_M  (BIT(5))
#define HOST_SLC1_TOHOST_BIT5_INT_CLR_V  0x1
#define HOST_SLC1_TOHOST_BIT5_INT_CLR_S  5
/* HOST_SLC1_TOHOST_BIT4_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT4_INT_CLR  (BIT(4))
#define HOST_SLC1_TOHOST_BIT4_INT_CLR_M  (BIT(4))
#define HOST_SLC1_TOHOST_BIT4_INT_CLR_V  0x1
#define HOST_SLC1_TOHOST_BIT4_INT_CLR_S  4
/* HOST_SLC1_TOHOST_BIT3_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT3_INT_CLR  (BIT(3))
#define HOST_SLC1_TOHOST_BIT3_INT_CLR_M  (BIT(3))
#define HOST_SLC1_TOHOST_BIT3_INT_CLR_V  0x1
#define HOST_SLC1_TOHOST_BIT3_INT_CLR_S  3
/* HOST_SLC1_TOHOST_BIT2_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT2_INT_CLR  (BIT(2))
#define HOST_SLC1_TOHOST_BIT2_INT_CLR_M  (BIT(2))
#define HOST_SLC1_TOHOST_BIT2_INT_CLR_V  0x1
#define HOST_SLC1_TOHOST_BIT2_INT_CLR_S  2
/* HOST_SLC1_TOHOST_BIT1_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT1_INT_CLR  (BIT(1))
#define HOST_SLC1_TOHOST_BIT1_INT_CLR_M  (BIT(1))
#define HOST_SLC1_TOHOST_BIT1_INT_CLR_V  0x1
#define HOST_SLC1_TOHOST_BIT1_INT_CLR_S  1
/* HOST_SLC1_TOHOST_BIT0_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT0_INT_CLR  (BIT(0))
#define HOST_SLC1_TOHOST_BIT0_INT_CLR_M  (BIT(0))
#define HOST_SLC1_TOHOST_BIT0_INT_CLR_V  0x1
#define HOST_SLC1_TOHOST_BIT0_INT_CLR_S  0

#define HOST_SLC0HOST_FUNC1_INT_ENA_REG          (DR_REG_SLCHOST_BASE + 0xDC)
/* HOST_FN1_GPIO_SDIO_INT_ENA : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_GPIO_SDIO_INT_ENA  (BIT(25))
#define HOST_FN1_GPIO_SDIO_INT_ENA_M  (BIT(25))
#define HOST_FN1_GPIO_SDIO_INT_ENA_V  0x1
#define HOST_FN1_GPIO_SDIO_INT_ENA_S  25
/* HOST_FN1_SLC0_HOST_RD_RETRY_INT_ENA : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_HOST_RD_RETRY_INT_ENA  (BIT(24))
#define HOST_FN1_SLC0_HOST_RD_RETRY_INT_ENA_M  (BIT(24))
#define HOST_FN1_SLC0_HOST_RD_RETRY_INT_ENA_V  0x1
#define HOST_FN1_SLC0_HOST_RD_RETRY_INT_ENA_S  24
/* HOST_FN1_SLC0_RX_NEW_PACKET_INT_ENA : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_RX_NEW_PACKET_INT_ENA  (BIT(23))
#define HOST_FN1_SLC0_RX_NEW_PACKET_INT_ENA_M  (BIT(23))
#define HOST_FN1_SLC0_RX_NEW_PACKET_INT_ENA_V  0x1
#define HOST_FN1_SLC0_RX_NEW_PACKET_INT_ENA_S  23
/* HOST_FN1_SLC0_EXT_BIT3_INT_ENA : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_EXT_BIT3_INT_ENA  (BIT(22))
#define HOST_FN1_SLC0_EXT_BIT3_INT_ENA_M  (BIT(22))
#define HOST_FN1_SLC0_EXT_BIT3_INT_ENA_V  0x1
#define HOST_FN1_SLC0_EXT_BIT3_INT_ENA_S  22
/* HOST_FN1_SLC0_EXT_BIT2_INT_ENA : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_EXT_BIT2_INT_ENA  (BIT(21))
#define HOST_FN1_SLC0_EXT_BIT2_INT_ENA_M  (BIT(21))
#define HOST_FN1_SLC0_EXT_BIT2_INT_ENA_V  0x1
#define HOST_FN1_SLC0_EXT_BIT2_INT_ENA_S  21
/* HOST_FN1_SLC0_EXT_BIT1_INT_ENA : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_EXT_BIT1_INT_ENA  (BIT(20))
#define HOST_FN1_SLC0_EXT_BIT1_INT_ENA_M  (BIT(20))
#define HOST_FN1_SLC0_EXT_BIT1_INT_ENA_V  0x1
#define HOST_FN1_SLC0_EXT_BIT1_INT_ENA_S  20
/* HOST_FN1_SLC0_EXT_BIT0_INT_ENA : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_EXT_BIT0_INT_ENA  (BIT(19))
#define HOST_FN1_SLC0_EXT_BIT0_INT_ENA_M  (BIT(19))
#define HOST_FN1_SLC0_EXT_BIT0_INT_ENA_V  0x1
#define HOST_FN1_SLC0_EXT_BIT0_INT_ENA_S  19
/* HOST_FN1_SLC0_RX_PF_VALID_INT_ENA : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_RX_PF_VALID_INT_ENA  (BIT(18))
#define HOST_FN1_SLC0_RX_PF_VALID_INT_ENA_M  (BIT(18))
#define HOST_FN1_SLC0_RX_PF_VALID_INT_ENA_V  0x1
#define HOST_FN1_SLC0_RX_PF_VALID_INT_ENA_S  18
/* HOST_FN1_SLC0_TX_OVF_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_TX_OVF_INT_ENA  (BIT(17))
#define HOST_FN1_SLC0_TX_OVF_INT_ENA_M  (BIT(17))
#define HOST_FN1_SLC0_TX_OVF_INT_ENA_V  0x1
#define HOST_FN1_SLC0_TX_OVF_INT_ENA_S  17
/* HOST_FN1_SLC0_RX_UDF_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_RX_UDF_INT_ENA  (BIT(16))
#define HOST_FN1_SLC0_RX_UDF_INT_ENA_M  (BIT(16))
#define HOST_FN1_SLC0_RX_UDF_INT_ENA_V  0x1
#define HOST_FN1_SLC0_RX_UDF_INT_ENA_S  16
/* HOST_FN1_SLC0HOST_TX_START_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0HOST_TX_START_INT_ENA  (BIT(15))
#define HOST_FN1_SLC0HOST_TX_START_INT_ENA_M  (BIT(15))
#define HOST_FN1_SLC0HOST_TX_START_INT_ENA_V  0x1
#define HOST_FN1_SLC0HOST_TX_START_INT_ENA_S  15
/* HOST_FN1_SLC0HOST_RX_START_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0HOST_RX_START_INT_ENA  (BIT(14))
#define HOST_FN1_SLC0HOST_RX_START_INT_ENA_M  (BIT(14))
#define HOST_FN1_SLC0HOST_RX_START_INT_ENA_V  0x1
#define HOST_FN1_SLC0HOST_RX_START_INT_ENA_S  14
/* HOST_FN1_SLC0HOST_RX_EOF_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0HOST_RX_EOF_INT_ENA  (BIT(13))
#define HOST_FN1_SLC0HOST_RX_EOF_INT_ENA_M  (BIT(13))
#define HOST_FN1_SLC0HOST_RX_EOF_INT_ENA_V  0x1
#define HOST_FN1_SLC0HOST_RX_EOF_INT_ENA_S  13
/* HOST_FN1_SLC0HOST_RX_SOF_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0HOST_RX_SOF_INT_ENA  (BIT(12))
#define HOST_FN1_SLC0HOST_RX_SOF_INT_ENA_M  (BIT(12))
#define HOST_FN1_SLC0HOST_RX_SOF_INT_ENA_V  0x1
#define HOST_FN1_SLC0HOST_RX_SOF_INT_ENA_S  12
/* HOST_FN1_SLC0_TOKEN1_0TO1_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_TOKEN1_0TO1_INT_ENA  (BIT(11))
#define HOST_FN1_SLC0_TOKEN1_0TO1_INT_ENA_M  (BIT(11))
#define HOST_FN1_SLC0_TOKEN1_0TO1_INT_ENA_V  0x1
#define HOST_FN1_SLC0_TOKEN1_0TO1_INT_ENA_S  11
/* HOST_FN1_SLC0_TOKEN0_0TO1_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_TOKEN0_0TO1_INT_ENA  (BIT(10))
#define HOST_FN1_SLC0_TOKEN0_0TO1_INT_ENA_M  (BIT(10))
#define HOST_FN1_SLC0_TOKEN0_0TO1_INT_ENA_V  0x1
#define HOST_FN1_SLC0_TOKEN0_0TO1_INT_ENA_S  10
/* HOST_FN1_SLC0_TOKEN1_1TO0_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_TOKEN1_1TO0_INT_ENA  (BIT(9))
#define HOST_FN1_SLC0_TOKEN1_1TO0_INT_ENA_M  (BIT(9))
#define HOST_FN1_SLC0_TOKEN1_1TO0_INT_ENA_V  0x1
#define HOST_FN1_SLC0_TOKEN1_1TO0_INT_ENA_S  9
/* HOST_FN1_SLC0_TOKEN0_1TO0_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_TOKEN0_1TO0_INT_ENA  (BIT(8))
#define HOST_FN1_SLC0_TOKEN0_1TO0_INT_ENA_M  (BIT(8))
#define HOST_FN1_SLC0_TOKEN0_1TO0_INT_ENA_V  0x1
#define HOST_FN1_SLC0_TOKEN0_1TO0_INT_ENA_S  8
/* HOST_FN1_SLC0_TOHOST_BIT7_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_TOHOST_BIT7_INT_ENA  (BIT(7))
#define HOST_FN1_SLC0_TOHOST_BIT7_INT_ENA_M  (BIT(7))
#define HOST_FN1_SLC0_TOHOST_BIT7_INT_ENA_V  0x1
#define HOST_FN1_SLC0_TOHOST_BIT7_INT_ENA_S  7
/* HOST_FN1_SLC0_TOHOST_BIT6_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_TOHOST_BIT6_INT_ENA  (BIT(6))
#define HOST_FN1_SLC0_TOHOST_BIT6_INT_ENA_M  (BIT(6))
#define HOST_FN1_SLC0_TOHOST_BIT6_INT_ENA_V  0x1
#define HOST_FN1_SLC0_TOHOST_BIT6_INT_ENA_S  6
/* HOST_FN1_SLC0_TOHOST_BIT5_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_TOHOST_BIT5_INT_ENA  (BIT(5))
#define HOST_FN1_SLC0_TOHOST_BIT5_INT_ENA_M  (BIT(5))
#define HOST_FN1_SLC0_TOHOST_BIT5_INT_ENA_V  0x1
#define HOST_FN1_SLC0_TOHOST_BIT5_INT_ENA_S  5
/* HOST_FN1_SLC0_TOHOST_BIT4_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_TOHOST_BIT4_INT_ENA  (BIT(4))
#define HOST_FN1_SLC0_TOHOST_BIT4_INT_ENA_M  (BIT(4))
#define HOST_FN1_SLC0_TOHOST_BIT4_INT_ENA_V  0x1
#define HOST_FN1_SLC0_TOHOST_BIT4_INT_ENA_S  4
/* HOST_FN1_SLC0_TOHOST_BIT3_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_TOHOST_BIT3_INT_ENA  (BIT(3))
#define HOST_FN1_SLC0_TOHOST_BIT3_INT_ENA_M  (BIT(3))
#define HOST_FN1_SLC0_TOHOST_BIT3_INT_ENA_V  0x1
#define HOST_FN1_SLC0_TOHOST_BIT3_INT_ENA_S  3
/* HOST_FN1_SLC0_TOHOST_BIT2_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_TOHOST_BIT2_INT_ENA  (BIT(2))
#define HOST_FN1_SLC0_TOHOST_BIT2_INT_ENA_M  (BIT(2))
#define HOST_FN1_SLC0_TOHOST_BIT2_INT_ENA_V  0x1
#define HOST_FN1_SLC0_TOHOST_BIT2_INT_ENA_S  2
/* HOST_FN1_SLC0_TOHOST_BIT1_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_TOHOST_BIT1_INT_ENA  (BIT(1))
#define HOST_FN1_SLC0_TOHOST_BIT1_INT_ENA_M  (BIT(1))
#define HOST_FN1_SLC0_TOHOST_BIT1_INT_ENA_V  0x1
#define HOST_FN1_SLC0_TOHOST_BIT1_INT_ENA_S  1
/* HOST_FN1_SLC0_TOHOST_BIT0_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC0_TOHOST_BIT0_INT_ENA  (BIT(0))
#define HOST_FN1_SLC0_TOHOST_BIT0_INT_ENA_M  (BIT(0))
#define HOST_FN1_SLC0_TOHOST_BIT0_INT_ENA_V  0x1
#define HOST_FN1_SLC0_TOHOST_BIT0_INT_ENA_S  0

#define HOST_SLC1HOST_FUNC1_INT_ENA_REG          (DR_REG_SLCHOST_BASE + 0xE0)
/* HOST_FN1_SLC1_BT_RX_NEW_PACKET_INT_ENA : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_BT_RX_NEW_PACKET_INT_ENA  (BIT(25))
#define HOST_FN1_SLC1_BT_RX_NEW_PACKET_INT_ENA_M  (BIT(25))
#define HOST_FN1_SLC1_BT_RX_NEW_PACKET_INT_ENA_V  0x1
#define HOST_FN1_SLC1_BT_RX_NEW_PACKET_INT_ENA_S  25
/* HOST_FN1_SLC1_HOST_RD_RETRY_INT_ENA : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_HOST_RD_RETRY_INT_ENA  (BIT(24))
#define HOST_FN1_SLC1_HOST_RD_RETRY_INT_ENA_M  (BIT(24))
#define HOST_FN1_SLC1_HOST_RD_RETRY_INT_ENA_V  0x1
#define HOST_FN1_SLC1_HOST_RD_RETRY_INT_ENA_S  24
/* HOST_FN1_SLC1_WIFI_RX_NEW_PACKET_INT_ENA : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_WIFI_RX_NEW_PACKET_INT_ENA  (BIT(23))
#define HOST_FN1_SLC1_WIFI_RX_NEW_PACKET_INT_ENA_M  (BIT(23))
#define HOST_FN1_SLC1_WIFI_RX_NEW_PACKET_INT_ENA_V  0x1
#define HOST_FN1_SLC1_WIFI_RX_NEW_PACKET_INT_ENA_S  23
/* HOST_FN1_SLC1_EXT_BIT3_INT_ENA : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_EXT_BIT3_INT_ENA  (BIT(22))
#define HOST_FN1_SLC1_EXT_BIT3_INT_ENA_M  (BIT(22))
#define HOST_FN1_SLC1_EXT_BIT3_INT_ENA_V  0x1
#define HOST_FN1_SLC1_EXT_BIT3_INT_ENA_S  22
/* HOST_FN1_SLC1_EXT_BIT2_INT_ENA : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_EXT_BIT2_INT_ENA  (BIT(21))
#define HOST_FN1_SLC1_EXT_BIT2_INT_ENA_M  (BIT(21))
#define HOST_FN1_SLC1_EXT_BIT2_INT_ENA_V  0x1
#define HOST_FN1_SLC1_EXT_BIT2_INT_ENA_S  21
/* HOST_FN1_SLC1_EXT_BIT1_INT_ENA : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_EXT_BIT1_INT_ENA  (BIT(20))
#define HOST_FN1_SLC1_EXT_BIT1_INT_ENA_M  (BIT(20))
#define HOST_FN1_SLC1_EXT_BIT1_INT_ENA_V  0x1
#define HOST_FN1_SLC1_EXT_BIT1_INT_ENA_S  20
/* HOST_FN1_SLC1_EXT_BIT0_INT_ENA : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_EXT_BIT0_INT_ENA  (BIT(19))
#define HOST_FN1_SLC1_EXT_BIT0_INT_ENA_M  (BIT(19))
#define HOST_FN1_SLC1_EXT_BIT0_INT_ENA_V  0x1
#define HOST_FN1_SLC1_EXT_BIT0_INT_ENA_S  19
/* HOST_FN1_SLC1_RX_PF_VALID_INT_ENA : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_RX_PF_VALID_INT_ENA  (BIT(18))
#define HOST_FN1_SLC1_RX_PF_VALID_INT_ENA_M  (BIT(18))
#define HOST_FN1_SLC1_RX_PF_VALID_INT_ENA_V  0x1
#define HOST_FN1_SLC1_RX_PF_VALID_INT_ENA_S  18
/* HOST_FN1_SLC1_TX_OVF_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_TX_OVF_INT_ENA  (BIT(17))
#define HOST_FN1_SLC1_TX_OVF_INT_ENA_M  (BIT(17))
#define HOST_FN1_SLC1_TX_OVF_INT_ENA_V  0x1
#define HOST_FN1_SLC1_TX_OVF_INT_ENA_S  17
/* HOST_FN1_SLC1_RX_UDF_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_RX_UDF_INT_ENA  (BIT(16))
#define HOST_FN1_SLC1_RX_UDF_INT_ENA_M  (BIT(16))
#define HOST_FN1_SLC1_RX_UDF_INT_ENA_V  0x1
#define HOST_FN1_SLC1_RX_UDF_INT_ENA_S  16
/* HOST_FN1_SLC1HOST_TX_START_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1HOST_TX_START_INT_ENA  (BIT(15))
#define HOST_FN1_SLC1HOST_TX_START_INT_ENA_M  (BIT(15))
#define HOST_FN1_SLC1HOST_TX_START_INT_ENA_V  0x1
#define HOST_FN1_SLC1HOST_TX_START_INT_ENA_S  15
/* HOST_FN1_SLC1HOST_RX_START_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1HOST_RX_START_INT_ENA  (BIT(14))
#define HOST_FN1_SLC1HOST_RX_START_INT_ENA_M  (BIT(14))
#define HOST_FN1_SLC1HOST_RX_START_INT_ENA_V  0x1
#define HOST_FN1_SLC1HOST_RX_START_INT_ENA_S  14
/* HOST_FN1_SLC1HOST_RX_EOF_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1HOST_RX_EOF_INT_ENA  (BIT(13))
#define HOST_FN1_SLC1HOST_RX_EOF_INT_ENA_M  (BIT(13))
#define HOST_FN1_SLC1HOST_RX_EOF_INT_ENA_V  0x1
#define HOST_FN1_SLC1HOST_RX_EOF_INT_ENA_S  13
/* HOST_FN1_SLC1HOST_RX_SOF_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1HOST_RX_SOF_INT_ENA  (BIT(12))
#define HOST_FN1_SLC1HOST_RX_SOF_INT_ENA_M  (BIT(12))
#define HOST_FN1_SLC1HOST_RX_SOF_INT_ENA_V  0x1
#define HOST_FN1_SLC1HOST_RX_SOF_INT_ENA_S  12
/* HOST_FN1_SLC1_TOKEN1_0TO1_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_TOKEN1_0TO1_INT_ENA  (BIT(11))
#define HOST_FN1_SLC1_TOKEN1_0TO1_INT_ENA_M  (BIT(11))
#define HOST_FN1_SLC1_TOKEN1_0TO1_INT_ENA_V  0x1
#define HOST_FN1_SLC1_TOKEN1_0TO1_INT_ENA_S  11
/* HOST_FN1_SLC1_TOKEN0_0TO1_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_TOKEN0_0TO1_INT_ENA  (BIT(10))
#define HOST_FN1_SLC1_TOKEN0_0TO1_INT_ENA_M  (BIT(10))
#define HOST_FN1_SLC1_TOKEN0_0TO1_INT_ENA_V  0x1
#define HOST_FN1_SLC1_TOKEN0_0TO1_INT_ENA_S  10
/* HOST_FN1_SLC1_TOKEN1_1TO0_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_TOKEN1_1TO0_INT_ENA  (BIT(9))
#define HOST_FN1_SLC1_TOKEN1_1TO0_INT_ENA_M  (BIT(9))
#define HOST_FN1_SLC1_TOKEN1_1TO0_INT_ENA_V  0x1
#define HOST_FN1_SLC1_TOKEN1_1TO0_INT_ENA_S  9
/* HOST_FN1_SLC1_TOKEN0_1TO0_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_TOKEN0_1TO0_INT_ENA  (BIT(8))
#define HOST_FN1_SLC1_TOKEN0_1TO0_INT_ENA_M  (BIT(8))
#define HOST_FN1_SLC1_TOKEN0_1TO0_INT_ENA_V  0x1
#define HOST_FN1_SLC1_TOKEN0_1TO0_INT_ENA_S  8
/* HOST_FN1_SLC1_TOHOST_BIT7_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_TOHOST_BIT7_INT_ENA  (BIT(7))
#define HOST_FN1_SLC1_TOHOST_BIT7_INT_ENA_M  (BIT(7))
#define HOST_FN1_SLC1_TOHOST_BIT7_INT_ENA_V  0x1
#define HOST_FN1_SLC1_TOHOST_BIT7_INT_ENA_S  7
/* HOST_FN1_SLC1_TOHOST_BIT6_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_TOHOST_BIT6_INT_ENA  (BIT(6))
#define HOST_FN1_SLC1_TOHOST_BIT6_INT_ENA_M  (BIT(6))
#define HOST_FN1_SLC1_TOHOST_BIT6_INT_ENA_V  0x1
#define HOST_FN1_SLC1_TOHOST_BIT6_INT_ENA_S  6
/* HOST_FN1_SLC1_TOHOST_BIT5_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_TOHOST_BIT5_INT_ENA  (BIT(5))
#define HOST_FN1_SLC1_TOHOST_BIT5_INT_ENA_M  (BIT(5))
#define HOST_FN1_SLC1_TOHOST_BIT5_INT_ENA_V  0x1
#define HOST_FN1_SLC1_TOHOST_BIT5_INT_ENA_S  5
/* HOST_FN1_SLC1_TOHOST_BIT4_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_TOHOST_BIT4_INT_ENA  (BIT(4))
#define HOST_FN1_SLC1_TOHOST_BIT4_INT_ENA_M  (BIT(4))
#define HOST_FN1_SLC1_TOHOST_BIT4_INT_ENA_V  0x1
#define HOST_FN1_SLC1_TOHOST_BIT4_INT_ENA_S  4
/* HOST_FN1_SLC1_TOHOST_BIT3_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_TOHOST_BIT3_INT_ENA  (BIT(3))
#define HOST_FN1_SLC1_TOHOST_BIT3_INT_ENA_M  (BIT(3))
#define HOST_FN1_SLC1_TOHOST_BIT3_INT_ENA_V  0x1
#define HOST_FN1_SLC1_TOHOST_BIT3_INT_ENA_S  3
/* HOST_FN1_SLC1_TOHOST_BIT2_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_TOHOST_BIT2_INT_ENA  (BIT(2))
#define HOST_FN1_SLC1_TOHOST_BIT2_INT_ENA_M  (BIT(2))
#define HOST_FN1_SLC1_TOHOST_BIT2_INT_ENA_V  0x1
#define HOST_FN1_SLC1_TOHOST_BIT2_INT_ENA_S  2
/* HOST_FN1_SLC1_TOHOST_BIT1_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_TOHOST_BIT1_INT_ENA  (BIT(1))
#define HOST_FN1_SLC1_TOHOST_BIT1_INT_ENA_M  (BIT(1))
#define HOST_FN1_SLC1_TOHOST_BIT1_INT_ENA_V  0x1
#define HOST_FN1_SLC1_TOHOST_BIT1_INT_ENA_S  1
/* HOST_FN1_SLC1_TOHOST_BIT0_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define HOST_FN1_SLC1_TOHOST_BIT0_INT_ENA  (BIT(0))
#define HOST_FN1_SLC1_TOHOST_BIT0_INT_ENA_M  (BIT(0))
#define HOST_FN1_SLC1_TOHOST_BIT0_INT_ENA_V  0x1
#define HOST_FN1_SLC1_TOHOST_BIT0_INT_ENA_S  0

#define HOST_SLC0HOST_FUNC2_INT_ENA_REG          (DR_REG_SLCHOST_BASE + 0xE4)
/* HOST_FN2_GPIO_SDIO_INT_ENA : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_GPIO_SDIO_INT_ENA  (BIT(25))
#define HOST_FN2_GPIO_SDIO_INT_ENA_M  (BIT(25))
#define HOST_FN2_GPIO_SDIO_INT_ENA_V  0x1
#define HOST_FN2_GPIO_SDIO_INT_ENA_S  25
/* HOST_FN2_SLC0_HOST_RD_RETRY_INT_ENA : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_HOST_RD_RETRY_INT_ENA  (BIT(24))
#define HOST_FN2_SLC0_HOST_RD_RETRY_INT_ENA_M  (BIT(24))
#define HOST_FN2_SLC0_HOST_RD_RETRY_INT_ENA_V  0x1
#define HOST_FN2_SLC0_HOST_RD_RETRY_INT_ENA_S  24
/* HOST_FN2_SLC0_RX_NEW_PACKET_INT_ENA : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_RX_NEW_PACKET_INT_ENA  (BIT(23))
#define HOST_FN2_SLC0_RX_NEW_PACKET_INT_ENA_M  (BIT(23))
#define HOST_FN2_SLC0_RX_NEW_PACKET_INT_ENA_V  0x1
#define HOST_FN2_SLC0_RX_NEW_PACKET_INT_ENA_S  23
/* HOST_FN2_SLC0_EXT_BIT3_INT_ENA : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_EXT_BIT3_INT_ENA  (BIT(22))
#define HOST_FN2_SLC0_EXT_BIT3_INT_ENA_M  (BIT(22))
#define HOST_FN2_SLC0_EXT_BIT3_INT_ENA_V  0x1
#define HOST_FN2_SLC0_EXT_BIT3_INT_ENA_S  22
/* HOST_FN2_SLC0_EXT_BIT2_INT_ENA : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_EXT_BIT2_INT_ENA  (BIT(21))
#define HOST_FN2_SLC0_EXT_BIT2_INT_ENA_M  (BIT(21))
#define HOST_FN2_SLC0_EXT_BIT2_INT_ENA_V  0x1
#define HOST_FN2_SLC0_EXT_BIT2_INT_ENA_S  21
/* HOST_FN2_SLC0_EXT_BIT1_INT_ENA : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_EXT_BIT1_INT_ENA  (BIT(20))
#define HOST_FN2_SLC0_EXT_BIT1_INT_ENA_M  (BIT(20))
#define HOST_FN2_SLC0_EXT_BIT1_INT_ENA_V  0x1
#define HOST_FN2_SLC0_EXT_BIT1_INT_ENA_S  20
/* HOST_FN2_SLC0_EXT_BIT0_INT_ENA : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_EXT_BIT0_INT_ENA  (BIT(19))
#define HOST_FN2_SLC0_EXT_BIT0_INT_ENA_M  (BIT(19))
#define HOST_FN2_SLC0_EXT_BIT0_INT_ENA_V  0x1
#define HOST_FN2_SLC0_EXT_BIT0_INT_ENA_S  19
/* HOST_FN2_SLC0_RX_PF_VALID_INT_ENA : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_RX_PF_VALID_INT_ENA  (BIT(18))
#define HOST_FN2_SLC0_RX_PF_VALID_INT_ENA_M  (BIT(18))
#define HOST_FN2_SLC0_RX_PF_VALID_INT_ENA_V  0x1
#define HOST_FN2_SLC0_RX_PF_VALID_INT_ENA_S  18
/* HOST_FN2_SLC0_TX_OVF_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_TX_OVF_INT_ENA  (BIT(17))
#define HOST_FN2_SLC0_TX_OVF_INT_ENA_M  (BIT(17))
#define HOST_FN2_SLC0_TX_OVF_INT_ENA_V  0x1
#define HOST_FN2_SLC0_TX_OVF_INT_ENA_S  17
/* HOST_FN2_SLC0_RX_UDF_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_RX_UDF_INT_ENA  (BIT(16))
#define HOST_FN2_SLC0_RX_UDF_INT_ENA_M  (BIT(16))
#define HOST_FN2_SLC0_RX_UDF_INT_ENA_V  0x1
#define HOST_FN2_SLC0_RX_UDF_INT_ENA_S  16
/* HOST_FN2_SLC0HOST_TX_START_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0HOST_TX_START_INT_ENA  (BIT(15))
#define HOST_FN2_SLC0HOST_TX_START_INT_ENA_M  (BIT(15))
#define HOST_FN2_SLC0HOST_TX_START_INT_ENA_V  0x1
#define HOST_FN2_SLC0HOST_TX_START_INT_ENA_S  15
/* HOST_FN2_SLC0HOST_RX_START_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0HOST_RX_START_INT_ENA  (BIT(14))
#define HOST_FN2_SLC0HOST_RX_START_INT_ENA_M  (BIT(14))
#define HOST_FN2_SLC0HOST_RX_START_INT_ENA_V  0x1
#define HOST_FN2_SLC0HOST_RX_START_INT_ENA_S  14
/* HOST_FN2_SLC0HOST_RX_EOF_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0HOST_RX_EOF_INT_ENA  (BIT(13))
#define HOST_FN2_SLC0HOST_RX_EOF_INT_ENA_M  (BIT(13))
#define HOST_FN2_SLC0HOST_RX_EOF_INT_ENA_V  0x1
#define HOST_FN2_SLC0HOST_RX_EOF_INT_ENA_S  13
/* HOST_FN2_SLC0HOST_RX_SOF_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0HOST_RX_SOF_INT_ENA  (BIT(12))
#define HOST_FN2_SLC0HOST_RX_SOF_INT_ENA_M  (BIT(12))
#define HOST_FN2_SLC0HOST_RX_SOF_INT_ENA_V  0x1
#define HOST_FN2_SLC0HOST_RX_SOF_INT_ENA_S  12
/* HOST_FN2_SLC0_TOKEN1_0TO1_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_TOKEN1_0TO1_INT_ENA  (BIT(11))
#define HOST_FN2_SLC0_TOKEN1_0TO1_INT_ENA_M  (BIT(11))
#define HOST_FN2_SLC0_TOKEN1_0TO1_INT_ENA_V  0x1
#define HOST_FN2_SLC0_TOKEN1_0TO1_INT_ENA_S  11
/* HOST_FN2_SLC0_TOKEN0_0TO1_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_TOKEN0_0TO1_INT_ENA  (BIT(10))
#define HOST_FN2_SLC0_TOKEN0_0TO1_INT_ENA_M  (BIT(10))
#define HOST_FN2_SLC0_TOKEN0_0TO1_INT_ENA_V  0x1
#define HOST_FN2_SLC0_TOKEN0_0TO1_INT_ENA_S  10
/* HOST_FN2_SLC0_TOKEN1_1TO0_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_TOKEN1_1TO0_INT_ENA  (BIT(9))
#define HOST_FN2_SLC0_TOKEN1_1TO0_INT_ENA_M  (BIT(9))
#define HOST_FN2_SLC0_TOKEN1_1TO0_INT_ENA_V  0x1
#define HOST_FN2_SLC0_TOKEN1_1TO0_INT_ENA_S  9
/* HOST_FN2_SLC0_TOKEN0_1TO0_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_TOKEN0_1TO0_INT_ENA  (BIT(8))
#define HOST_FN2_SLC0_TOKEN0_1TO0_INT_ENA_M  (BIT(8))
#define HOST_FN2_SLC0_TOKEN0_1TO0_INT_ENA_V  0x1
#define HOST_FN2_SLC0_TOKEN0_1TO0_INT_ENA_S  8
/* HOST_FN2_SLC0_TOHOST_BIT7_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_TOHOST_BIT7_INT_ENA  (BIT(7))
#define HOST_FN2_SLC0_TOHOST_BIT7_INT_ENA_M  (BIT(7))
#define HOST_FN2_SLC0_TOHOST_BIT7_INT_ENA_V  0x1
#define HOST_FN2_SLC0_TOHOST_BIT7_INT_ENA_S  7
/* HOST_FN2_SLC0_TOHOST_BIT6_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_TOHOST_BIT6_INT_ENA  (BIT(6))
#define HOST_FN2_SLC0_TOHOST_BIT6_INT_ENA_M  (BIT(6))
#define HOST_FN2_SLC0_TOHOST_BIT6_INT_ENA_V  0x1
#define HOST_FN2_SLC0_TOHOST_BIT6_INT_ENA_S  6
/* HOST_FN2_SLC0_TOHOST_BIT5_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_TOHOST_BIT5_INT_ENA  (BIT(5))
#define HOST_FN2_SLC0_TOHOST_BIT5_INT_ENA_M  (BIT(5))
#define HOST_FN2_SLC0_TOHOST_BIT5_INT_ENA_V  0x1
#define HOST_FN2_SLC0_TOHOST_BIT5_INT_ENA_S  5
/* HOST_FN2_SLC0_TOHOST_BIT4_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_TOHOST_BIT4_INT_ENA  (BIT(4))
#define HOST_FN2_SLC0_TOHOST_BIT4_INT_ENA_M  (BIT(4))
#define HOST_FN2_SLC0_TOHOST_BIT4_INT_ENA_V  0x1
#define HOST_FN2_SLC0_TOHOST_BIT4_INT_ENA_S  4
/* HOST_FN2_SLC0_TOHOST_BIT3_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_TOHOST_BIT3_INT_ENA  (BIT(3))
#define HOST_FN2_SLC0_TOHOST_BIT3_INT_ENA_M  (BIT(3))
#define HOST_FN2_SLC0_TOHOST_BIT3_INT_ENA_V  0x1
#define HOST_FN2_SLC0_TOHOST_BIT3_INT_ENA_S  3
/* HOST_FN2_SLC0_TOHOST_BIT2_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_TOHOST_BIT2_INT_ENA  (BIT(2))
#define HOST_FN2_SLC0_TOHOST_BIT2_INT_ENA_M  (BIT(2))
#define HOST_FN2_SLC0_TOHOST_BIT2_INT_ENA_V  0x1
#define HOST_FN2_SLC0_TOHOST_BIT2_INT_ENA_S  2
/* HOST_FN2_SLC0_TOHOST_BIT1_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_TOHOST_BIT1_INT_ENA  (BIT(1))
#define HOST_FN2_SLC0_TOHOST_BIT1_INT_ENA_M  (BIT(1))
#define HOST_FN2_SLC0_TOHOST_BIT1_INT_ENA_V  0x1
#define HOST_FN2_SLC0_TOHOST_BIT1_INT_ENA_S  1
/* HOST_FN2_SLC0_TOHOST_BIT0_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC0_TOHOST_BIT0_INT_ENA  (BIT(0))
#define HOST_FN2_SLC0_TOHOST_BIT0_INT_ENA_M  (BIT(0))
#define HOST_FN2_SLC0_TOHOST_BIT0_INT_ENA_V  0x1
#define HOST_FN2_SLC0_TOHOST_BIT0_INT_ENA_S  0

#define HOST_SLC1HOST_FUNC2_INT_ENA_REG          (DR_REG_SLCHOST_BASE + 0xE8)
/* HOST_FN2_SLC1_BT_RX_NEW_PACKET_INT_ENA : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_BT_RX_NEW_PACKET_INT_ENA  (BIT(25))
#define HOST_FN2_SLC1_BT_RX_NEW_PACKET_INT_ENA_M  (BIT(25))
#define HOST_FN2_SLC1_BT_RX_NEW_PACKET_INT_ENA_V  0x1
#define HOST_FN2_SLC1_BT_RX_NEW_PACKET_INT_ENA_S  25
/* HOST_FN2_SLC1_HOST_RD_RETRY_INT_ENA : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_HOST_RD_RETRY_INT_ENA  (BIT(24))
#define HOST_FN2_SLC1_HOST_RD_RETRY_INT_ENA_M  (BIT(24))
#define HOST_FN2_SLC1_HOST_RD_RETRY_INT_ENA_V  0x1
#define HOST_FN2_SLC1_HOST_RD_RETRY_INT_ENA_S  24
/* HOST_FN2_SLC1_WIFI_RX_NEW_PACKET_INT_ENA : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_WIFI_RX_NEW_PACKET_INT_ENA  (BIT(23))
#define HOST_FN2_SLC1_WIFI_RX_NEW_PACKET_INT_ENA_M  (BIT(23))
#define HOST_FN2_SLC1_WIFI_RX_NEW_PACKET_INT_ENA_V  0x1
#define HOST_FN2_SLC1_WIFI_RX_NEW_PACKET_INT_ENA_S  23
/* HOST_FN2_SLC1_EXT_BIT3_INT_ENA : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_EXT_BIT3_INT_ENA  (BIT(22))
#define HOST_FN2_SLC1_EXT_BIT3_INT_ENA_M  (BIT(22))
#define HOST_FN2_SLC1_EXT_BIT3_INT_ENA_V  0x1
#define HOST_FN2_SLC1_EXT_BIT3_INT_ENA_S  22
/* HOST_FN2_SLC1_EXT_BIT2_INT_ENA : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_EXT_BIT2_INT_ENA  (BIT(21))
#define HOST_FN2_SLC1_EXT_BIT2_INT_ENA_M  (BIT(21))
#define HOST_FN2_SLC1_EXT_BIT2_INT_ENA_V  0x1
#define HOST_FN2_SLC1_EXT_BIT2_INT_ENA_S  21
/* HOST_FN2_SLC1_EXT_BIT1_INT_ENA : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_EXT_BIT1_INT_ENA  (BIT(20))
#define HOST_FN2_SLC1_EXT_BIT1_INT_ENA_M  (BIT(20))
#define HOST_FN2_SLC1_EXT_BIT1_INT_ENA_V  0x1
#define HOST_FN2_SLC1_EXT_BIT1_INT_ENA_S  20
/* HOST_FN2_SLC1_EXT_BIT0_INT_ENA : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_EXT_BIT0_INT_ENA  (BIT(19))
#define HOST_FN2_SLC1_EXT_BIT0_INT_ENA_M  (BIT(19))
#define HOST_FN2_SLC1_EXT_BIT0_INT_ENA_V  0x1
#define HOST_FN2_SLC1_EXT_BIT0_INT_ENA_S  19
/* HOST_FN2_SLC1_RX_PF_VALID_INT_ENA : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_RX_PF_VALID_INT_ENA  (BIT(18))
#define HOST_FN2_SLC1_RX_PF_VALID_INT_ENA_M  (BIT(18))
#define HOST_FN2_SLC1_RX_PF_VALID_INT_ENA_V  0x1
#define HOST_FN2_SLC1_RX_PF_VALID_INT_ENA_S  18
/* HOST_FN2_SLC1_TX_OVF_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_TX_OVF_INT_ENA  (BIT(17))
#define HOST_FN2_SLC1_TX_OVF_INT_ENA_M  (BIT(17))
#define HOST_FN2_SLC1_TX_OVF_INT_ENA_V  0x1
#define HOST_FN2_SLC1_TX_OVF_INT_ENA_S  17
/* HOST_FN2_SLC1_RX_UDF_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_RX_UDF_INT_ENA  (BIT(16))
#define HOST_FN2_SLC1_RX_UDF_INT_ENA_M  (BIT(16))
#define HOST_FN2_SLC1_RX_UDF_INT_ENA_V  0x1
#define HOST_FN2_SLC1_RX_UDF_INT_ENA_S  16
/* HOST_FN2_SLC1HOST_TX_START_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1HOST_TX_START_INT_ENA  (BIT(15))
#define HOST_FN2_SLC1HOST_TX_START_INT_ENA_M  (BIT(15))
#define HOST_FN2_SLC1HOST_TX_START_INT_ENA_V  0x1
#define HOST_FN2_SLC1HOST_TX_START_INT_ENA_S  15
/* HOST_FN2_SLC1HOST_RX_START_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1HOST_RX_START_INT_ENA  (BIT(14))
#define HOST_FN2_SLC1HOST_RX_START_INT_ENA_M  (BIT(14))
#define HOST_FN2_SLC1HOST_RX_START_INT_ENA_V  0x1
#define HOST_FN2_SLC1HOST_RX_START_INT_ENA_S  14
/* HOST_FN2_SLC1HOST_RX_EOF_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1HOST_RX_EOF_INT_ENA  (BIT(13))
#define HOST_FN2_SLC1HOST_RX_EOF_INT_ENA_M  (BIT(13))
#define HOST_FN2_SLC1HOST_RX_EOF_INT_ENA_V  0x1
#define HOST_FN2_SLC1HOST_RX_EOF_INT_ENA_S  13
/* HOST_FN2_SLC1HOST_RX_SOF_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1HOST_RX_SOF_INT_ENA  (BIT(12))
#define HOST_FN2_SLC1HOST_RX_SOF_INT_ENA_M  (BIT(12))
#define HOST_FN2_SLC1HOST_RX_SOF_INT_ENA_V  0x1
#define HOST_FN2_SLC1HOST_RX_SOF_INT_ENA_S  12
/* HOST_FN2_SLC1_TOKEN1_0TO1_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_TOKEN1_0TO1_INT_ENA  (BIT(11))
#define HOST_FN2_SLC1_TOKEN1_0TO1_INT_ENA_M  (BIT(11))
#define HOST_FN2_SLC1_TOKEN1_0TO1_INT_ENA_V  0x1
#define HOST_FN2_SLC1_TOKEN1_0TO1_INT_ENA_S  11
/* HOST_FN2_SLC1_TOKEN0_0TO1_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_TOKEN0_0TO1_INT_ENA  (BIT(10))
#define HOST_FN2_SLC1_TOKEN0_0TO1_INT_ENA_M  (BIT(10))
#define HOST_FN2_SLC1_TOKEN0_0TO1_INT_ENA_V  0x1
#define HOST_FN2_SLC1_TOKEN0_0TO1_INT_ENA_S  10
/* HOST_FN2_SLC1_TOKEN1_1TO0_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_TOKEN1_1TO0_INT_ENA  (BIT(9))
#define HOST_FN2_SLC1_TOKEN1_1TO0_INT_ENA_M  (BIT(9))
#define HOST_FN2_SLC1_TOKEN1_1TO0_INT_ENA_V  0x1
#define HOST_FN2_SLC1_TOKEN1_1TO0_INT_ENA_S  9
/* HOST_FN2_SLC1_TOKEN0_1TO0_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_TOKEN0_1TO0_INT_ENA  (BIT(8))
#define HOST_FN2_SLC1_TOKEN0_1TO0_INT_ENA_M  (BIT(8))
#define HOST_FN2_SLC1_TOKEN0_1TO0_INT_ENA_V  0x1
#define HOST_FN2_SLC1_TOKEN0_1TO0_INT_ENA_S  8
/* HOST_FN2_SLC1_TOHOST_BIT7_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_TOHOST_BIT7_INT_ENA  (BIT(7))
#define HOST_FN2_SLC1_TOHOST_BIT7_INT_ENA_M  (BIT(7))
#define HOST_FN2_SLC1_TOHOST_BIT7_INT_ENA_V  0x1
#define HOST_FN2_SLC1_TOHOST_BIT7_INT_ENA_S  7
/* HOST_FN2_SLC1_TOHOST_BIT6_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_TOHOST_BIT6_INT_ENA  (BIT(6))
#define HOST_FN2_SLC1_TOHOST_BIT6_INT_ENA_M  (BIT(6))
#define HOST_FN2_SLC1_TOHOST_BIT6_INT_ENA_V  0x1
#define HOST_FN2_SLC1_TOHOST_BIT6_INT_ENA_S  6
/* HOST_FN2_SLC1_TOHOST_BIT5_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_TOHOST_BIT5_INT_ENA  (BIT(5))
#define HOST_FN2_SLC1_TOHOST_BIT5_INT_ENA_M  (BIT(5))
#define HOST_FN2_SLC1_TOHOST_BIT5_INT_ENA_V  0x1
#define HOST_FN2_SLC1_TOHOST_BIT5_INT_ENA_S  5
/* HOST_FN2_SLC1_TOHOST_BIT4_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_TOHOST_BIT4_INT_ENA  (BIT(4))
#define HOST_FN2_SLC1_TOHOST_BIT4_INT_ENA_M  (BIT(4))
#define HOST_FN2_SLC1_TOHOST_BIT4_INT_ENA_V  0x1
#define HOST_FN2_SLC1_TOHOST_BIT4_INT_ENA_S  4
/* HOST_FN2_SLC1_TOHOST_BIT3_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_TOHOST_BIT3_INT_ENA  (BIT(3))
#define HOST_FN2_SLC1_TOHOST_BIT3_INT_ENA_M  (BIT(3))
#define HOST_FN2_SLC1_TOHOST_BIT3_INT_ENA_V  0x1
#define HOST_FN2_SLC1_TOHOST_BIT3_INT_ENA_S  3
/* HOST_FN2_SLC1_TOHOST_BIT2_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_TOHOST_BIT2_INT_ENA  (BIT(2))
#define HOST_FN2_SLC1_TOHOST_BIT2_INT_ENA_M  (BIT(2))
#define HOST_FN2_SLC1_TOHOST_BIT2_INT_ENA_V  0x1
#define HOST_FN2_SLC1_TOHOST_BIT2_INT_ENA_S  2
/* HOST_FN2_SLC1_TOHOST_BIT1_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_TOHOST_BIT1_INT_ENA  (BIT(1))
#define HOST_FN2_SLC1_TOHOST_BIT1_INT_ENA_M  (BIT(1))
#define HOST_FN2_SLC1_TOHOST_BIT1_INT_ENA_V  0x1
#define HOST_FN2_SLC1_TOHOST_BIT1_INT_ENA_S  1
/* HOST_FN2_SLC1_TOHOST_BIT0_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define HOST_FN2_SLC1_TOHOST_BIT0_INT_ENA  (BIT(0))
#define HOST_FN2_SLC1_TOHOST_BIT0_INT_ENA_M  (BIT(0))
#define HOST_FN2_SLC1_TOHOST_BIT0_INT_ENA_V  0x1
#define HOST_FN2_SLC1_TOHOST_BIT0_INT_ENA_S  0

#define HOST_SLC0HOST_INT_ENA_REG          (DR_REG_SLCHOST_BASE + 0xEC)
/* HOST_GPIO_SDIO_INT_ENA : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define HOST_GPIO_SDIO_INT_ENA  (BIT(25))
#define HOST_GPIO_SDIO_INT_ENA_M  (BIT(25))
#define HOST_GPIO_SDIO_INT_ENA_V  0x1
#define HOST_GPIO_SDIO_INT_ENA_S  25
/* HOST_SLC0_HOST_RD_RETRY_INT_ENA : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_HOST_RD_RETRY_INT_ENA  (BIT(24))
#define HOST_SLC0_HOST_RD_RETRY_INT_ENA_M  (BIT(24))
#define HOST_SLC0_HOST_RD_RETRY_INT_ENA_V  0x1
#define HOST_SLC0_HOST_RD_RETRY_INT_ENA_S  24
/* HOST_SLC0_RX_NEW_PACKET_INT_ENA : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_RX_NEW_PACKET_INT_ENA  (BIT(23))
#define HOST_SLC0_RX_NEW_PACKET_INT_ENA_M  (BIT(23))
#define HOST_SLC0_RX_NEW_PACKET_INT_ENA_V  0x1
#define HOST_SLC0_RX_NEW_PACKET_INT_ENA_S  23
/* HOST_SLC0_EXT_BIT3_INT_ENA : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_EXT_BIT3_INT_ENA  (BIT(22))
#define HOST_SLC0_EXT_BIT3_INT_ENA_M  (BIT(22))
#define HOST_SLC0_EXT_BIT3_INT_ENA_V  0x1
#define HOST_SLC0_EXT_BIT3_INT_ENA_S  22
/* HOST_SLC0_EXT_BIT2_INT_ENA : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_EXT_BIT2_INT_ENA  (BIT(21))
#define HOST_SLC0_EXT_BIT2_INT_ENA_M  (BIT(21))
#define HOST_SLC0_EXT_BIT2_INT_ENA_V  0x1
#define HOST_SLC0_EXT_BIT2_INT_ENA_S  21
/* HOST_SLC0_EXT_BIT1_INT_ENA : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_EXT_BIT1_INT_ENA  (BIT(20))
#define HOST_SLC0_EXT_BIT1_INT_ENA_M  (BIT(20))
#define HOST_SLC0_EXT_BIT1_INT_ENA_V  0x1
#define HOST_SLC0_EXT_BIT1_INT_ENA_S  20
/* HOST_SLC0_EXT_BIT0_INT_ENA : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_EXT_BIT0_INT_ENA  (BIT(19))
#define HOST_SLC0_EXT_BIT0_INT_ENA_M  (BIT(19))
#define HOST_SLC0_EXT_BIT0_INT_ENA_V  0x1
#define HOST_SLC0_EXT_BIT0_INT_ENA_S  19
/* HOST_SLC0_RX_PF_VALID_INT_ENA : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_RX_PF_VALID_INT_ENA  (BIT(18))
#define HOST_SLC0_RX_PF_VALID_INT_ENA_M  (BIT(18))
#define HOST_SLC0_RX_PF_VALID_INT_ENA_V  0x1
#define HOST_SLC0_RX_PF_VALID_INT_ENA_S  18
/* HOST_SLC0_TX_OVF_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TX_OVF_INT_ENA  (BIT(17))
#define HOST_SLC0_TX_OVF_INT_ENA_M  (BIT(17))
#define HOST_SLC0_TX_OVF_INT_ENA_V  0x1
#define HOST_SLC0_TX_OVF_INT_ENA_S  17
/* HOST_SLC0_RX_UDF_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_RX_UDF_INT_ENA  (BIT(16))
#define HOST_SLC0_RX_UDF_INT_ENA_M  (BIT(16))
#define HOST_SLC0_RX_UDF_INT_ENA_V  0x1
#define HOST_SLC0_RX_UDF_INT_ENA_S  16
/* HOST_SLC0HOST_TX_START_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_TX_START_INT_ENA  (BIT(15))
#define HOST_SLC0HOST_TX_START_INT_ENA_M  (BIT(15))
#define HOST_SLC0HOST_TX_START_INT_ENA_V  0x1
#define HOST_SLC0HOST_TX_START_INT_ENA_S  15
/* HOST_SLC0HOST_RX_START_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_RX_START_INT_ENA  (BIT(14))
#define HOST_SLC0HOST_RX_START_INT_ENA_M  (BIT(14))
#define HOST_SLC0HOST_RX_START_INT_ENA_V  0x1
#define HOST_SLC0HOST_RX_START_INT_ENA_S  14
/* HOST_SLC0HOST_RX_EOF_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_RX_EOF_INT_ENA  (BIT(13))
#define HOST_SLC0HOST_RX_EOF_INT_ENA_M  (BIT(13))
#define HOST_SLC0HOST_RX_EOF_INT_ENA_V  0x1
#define HOST_SLC0HOST_RX_EOF_INT_ENA_S  13
/* HOST_SLC0HOST_RX_SOF_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_RX_SOF_INT_ENA  (BIT(12))
#define HOST_SLC0HOST_RX_SOF_INT_ENA_M  (BIT(12))
#define HOST_SLC0HOST_RX_SOF_INT_ENA_V  0x1
#define HOST_SLC0HOST_RX_SOF_INT_ENA_S  12
/* HOST_SLC0_TOKEN1_0TO1_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOKEN1_0TO1_INT_ENA  (BIT(11))
#define HOST_SLC0_TOKEN1_0TO1_INT_ENA_M  (BIT(11))
#define HOST_SLC0_TOKEN1_0TO1_INT_ENA_V  0x1
#define HOST_SLC0_TOKEN1_0TO1_INT_ENA_S  11
/* HOST_SLC0_TOKEN0_0TO1_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOKEN0_0TO1_INT_ENA  (BIT(10))
#define HOST_SLC0_TOKEN0_0TO1_INT_ENA_M  (BIT(10))
#define HOST_SLC0_TOKEN0_0TO1_INT_ENA_V  0x1
#define HOST_SLC0_TOKEN0_0TO1_INT_ENA_S  10
/* HOST_SLC0_TOKEN1_1TO0_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOKEN1_1TO0_INT_ENA  (BIT(9))
#define HOST_SLC0_TOKEN1_1TO0_INT_ENA_M  (BIT(9))
#define HOST_SLC0_TOKEN1_1TO0_INT_ENA_V  0x1
#define HOST_SLC0_TOKEN1_1TO0_INT_ENA_S  9
/* HOST_SLC0_TOKEN0_1TO0_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOKEN0_1TO0_INT_ENA  (BIT(8))
#define HOST_SLC0_TOKEN0_1TO0_INT_ENA_M  (BIT(8))
#define HOST_SLC0_TOKEN0_1TO0_INT_ENA_V  0x1
#define HOST_SLC0_TOKEN0_1TO0_INT_ENA_S  8
/* HOST_SLC0_TOHOST_BIT7_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT7_INT_ENA  (BIT(7))
#define HOST_SLC0_TOHOST_BIT7_INT_ENA_M  (BIT(7))
#define HOST_SLC0_TOHOST_BIT7_INT_ENA_V  0x1
#define HOST_SLC0_TOHOST_BIT7_INT_ENA_S  7
/* HOST_SLC0_TOHOST_BIT6_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT6_INT_ENA  (BIT(6))
#define HOST_SLC0_TOHOST_BIT6_INT_ENA_M  (BIT(6))
#define HOST_SLC0_TOHOST_BIT6_INT_ENA_V  0x1
#define HOST_SLC0_TOHOST_BIT6_INT_ENA_S  6
/* HOST_SLC0_TOHOST_BIT5_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT5_INT_ENA  (BIT(5))
#define HOST_SLC0_TOHOST_BIT5_INT_ENA_M  (BIT(5))
#define HOST_SLC0_TOHOST_BIT5_INT_ENA_V  0x1
#define HOST_SLC0_TOHOST_BIT5_INT_ENA_S  5
/* HOST_SLC0_TOHOST_BIT4_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT4_INT_ENA  (BIT(4))
#define HOST_SLC0_TOHOST_BIT4_INT_ENA_M  (BIT(4))
#define HOST_SLC0_TOHOST_BIT4_INT_ENA_V  0x1
#define HOST_SLC0_TOHOST_BIT4_INT_ENA_S  4
/* HOST_SLC0_TOHOST_BIT3_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT3_INT_ENA  (BIT(3))
#define HOST_SLC0_TOHOST_BIT3_INT_ENA_M  (BIT(3))
#define HOST_SLC0_TOHOST_BIT3_INT_ENA_V  0x1
#define HOST_SLC0_TOHOST_BIT3_INT_ENA_S  3
/* HOST_SLC0_TOHOST_BIT2_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT2_INT_ENA  (BIT(2))
#define HOST_SLC0_TOHOST_BIT2_INT_ENA_M  (BIT(2))
#define HOST_SLC0_TOHOST_BIT2_INT_ENA_V  0x1
#define HOST_SLC0_TOHOST_BIT2_INT_ENA_S  2
/* HOST_SLC0_TOHOST_BIT1_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT1_INT_ENA  (BIT(1))
#define HOST_SLC0_TOHOST_BIT1_INT_ENA_M  (BIT(1))
#define HOST_SLC0_TOHOST_BIT1_INT_ENA_V  0x1
#define HOST_SLC0_TOHOST_BIT1_INT_ENA_S  1
/* HOST_SLC0_TOHOST_BIT0_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT0_INT_ENA  (BIT(0))
#define HOST_SLC0_TOHOST_BIT0_INT_ENA_M  (BIT(0))
#define HOST_SLC0_TOHOST_BIT0_INT_ENA_V  0x1
#define HOST_SLC0_TOHOST_BIT0_INT_ENA_S  0

#define HOST_SLC1HOST_INT_ENA_REG          (DR_REG_SLCHOST_BASE + 0xF0)
/* HOST_SLC1_BT_RX_NEW_PACKET_INT_ENA : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_BT_RX_NEW_PACKET_INT_ENA  (BIT(25))
#define HOST_SLC1_BT_RX_NEW_PACKET_INT_ENA_M  (BIT(25))
#define HOST_SLC1_BT_RX_NEW_PACKET_INT_ENA_V  0x1
#define HOST_SLC1_BT_RX_NEW_PACKET_INT_ENA_S  25
/* HOST_SLC1_HOST_RD_RETRY_INT_ENA : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_HOST_RD_RETRY_INT_ENA  (BIT(24))
#define HOST_SLC1_HOST_RD_RETRY_INT_ENA_M  (BIT(24))
#define HOST_SLC1_HOST_RD_RETRY_INT_ENA_V  0x1
#define HOST_SLC1_HOST_RD_RETRY_INT_ENA_S  24
/* HOST_SLC1_WIFI_RX_NEW_PACKET_INT_ENA : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_WIFI_RX_NEW_PACKET_INT_ENA  (BIT(23))
#define HOST_SLC1_WIFI_RX_NEW_PACKET_INT_ENA_M  (BIT(23))
#define HOST_SLC1_WIFI_RX_NEW_PACKET_INT_ENA_V  0x1
#define HOST_SLC1_WIFI_RX_NEW_PACKET_INT_ENA_S  23
/* HOST_SLC1_EXT_BIT3_INT_ENA : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_EXT_BIT3_INT_ENA  (BIT(22))
#define HOST_SLC1_EXT_BIT3_INT_ENA_M  (BIT(22))
#define HOST_SLC1_EXT_BIT3_INT_ENA_V  0x1
#define HOST_SLC1_EXT_BIT3_INT_ENA_S  22
/* HOST_SLC1_EXT_BIT2_INT_ENA : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_EXT_BIT2_INT_ENA  (BIT(21))
#define HOST_SLC1_EXT_BIT2_INT_ENA_M  (BIT(21))
#define HOST_SLC1_EXT_BIT2_INT_ENA_V  0x1
#define HOST_SLC1_EXT_BIT2_INT_ENA_S  21
/* HOST_SLC1_EXT_BIT1_INT_ENA : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_EXT_BIT1_INT_ENA  (BIT(20))
#define HOST_SLC1_EXT_BIT1_INT_ENA_M  (BIT(20))
#define HOST_SLC1_EXT_BIT1_INT_ENA_V  0x1
#define HOST_SLC1_EXT_BIT1_INT_ENA_S  20
/* HOST_SLC1_EXT_BIT0_INT_ENA : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_EXT_BIT0_INT_ENA  (BIT(19))
#define HOST_SLC1_EXT_BIT0_INT_ENA_M  (BIT(19))
#define HOST_SLC1_EXT_BIT0_INT_ENA_V  0x1
#define HOST_SLC1_EXT_BIT0_INT_ENA_S  19
/* HOST_SLC1_RX_PF_VALID_INT_ENA : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_RX_PF_VALID_INT_ENA  (BIT(18))
#define HOST_SLC1_RX_PF_VALID_INT_ENA_M  (BIT(18))
#define HOST_SLC1_RX_PF_VALID_INT_ENA_V  0x1
#define HOST_SLC1_RX_PF_VALID_INT_ENA_S  18
/* HOST_SLC1_TX_OVF_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TX_OVF_INT_ENA  (BIT(17))
#define HOST_SLC1_TX_OVF_INT_ENA_M  (BIT(17))
#define HOST_SLC1_TX_OVF_INT_ENA_V  0x1
#define HOST_SLC1_TX_OVF_INT_ENA_S  17
/* HOST_SLC1_RX_UDF_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_RX_UDF_INT_ENA  (BIT(16))
#define HOST_SLC1_RX_UDF_INT_ENA_M  (BIT(16))
#define HOST_SLC1_RX_UDF_INT_ENA_V  0x1
#define HOST_SLC1_RX_UDF_INT_ENA_S  16
/* HOST_SLC1HOST_TX_START_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_TX_START_INT_ENA  (BIT(15))
#define HOST_SLC1HOST_TX_START_INT_ENA_M  (BIT(15))
#define HOST_SLC1HOST_TX_START_INT_ENA_V  0x1
#define HOST_SLC1HOST_TX_START_INT_ENA_S  15
/* HOST_SLC1HOST_RX_START_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_RX_START_INT_ENA  (BIT(14))
#define HOST_SLC1HOST_RX_START_INT_ENA_M  (BIT(14))
#define HOST_SLC1HOST_RX_START_INT_ENA_V  0x1
#define HOST_SLC1HOST_RX_START_INT_ENA_S  14
/* HOST_SLC1HOST_RX_EOF_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_RX_EOF_INT_ENA  (BIT(13))
#define HOST_SLC1HOST_RX_EOF_INT_ENA_M  (BIT(13))
#define HOST_SLC1HOST_RX_EOF_INT_ENA_V  0x1
#define HOST_SLC1HOST_RX_EOF_INT_ENA_S  13
/* HOST_SLC1HOST_RX_SOF_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_RX_SOF_INT_ENA  (BIT(12))
#define HOST_SLC1HOST_RX_SOF_INT_ENA_M  (BIT(12))
#define HOST_SLC1HOST_RX_SOF_INT_ENA_V  0x1
#define HOST_SLC1HOST_RX_SOF_INT_ENA_S  12
/* HOST_SLC1_TOKEN1_0TO1_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOKEN1_0TO1_INT_ENA  (BIT(11))
#define HOST_SLC1_TOKEN1_0TO1_INT_ENA_M  (BIT(11))
#define HOST_SLC1_TOKEN1_0TO1_INT_ENA_V  0x1
#define HOST_SLC1_TOKEN1_0TO1_INT_ENA_S  11
/* HOST_SLC1_TOKEN0_0TO1_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOKEN0_0TO1_INT_ENA  (BIT(10))
#define HOST_SLC1_TOKEN0_0TO1_INT_ENA_M  (BIT(10))
#define HOST_SLC1_TOKEN0_0TO1_INT_ENA_V  0x1
#define HOST_SLC1_TOKEN0_0TO1_INT_ENA_S  10
/* HOST_SLC1_TOKEN1_1TO0_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOKEN1_1TO0_INT_ENA  (BIT(9))
#define HOST_SLC1_TOKEN1_1TO0_INT_ENA_M  (BIT(9))
#define HOST_SLC1_TOKEN1_1TO0_INT_ENA_V  0x1
#define HOST_SLC1_TOKEN1_1TO0_INT_ENA_S  9
/* HOST_SLC1_TOKEN0_1TO0_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOKEN0_1TO0_INT_ENA  (BIT(8))
#define HOST_SLC1_TOKEN0_1TO0_INT_ENA_M  (BIT(8))
#define HOST_SLC1_TOKEN0_1TO0_INT_ENA_V  0x1
#define HOST_SLC1_TOKEN0_1TO0_INT_ENA_S  8
/* HOST_SLC1_TOHOST_BIT7_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT7_INT_ENA  (BIT(7))
#define HOST_SLC1_TOHOST_BIT7_INT_ENA_M  (BIT(7))
#define HOST_SLC1_TOHOST_BIT7_INT_ENA_V  0x1
#define HOST_SLC1_TOHOST_BIT7_INT_ENA_S  7
/* HOST_SLC1_TOHOST_BIT6_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT6_INT_ENA  (BIT(6))
#define HOST_SLC1_TOHOST_BIT6_INT_ENA_M  (BIT(6))
#define HOST_SLC1_TOHOST_BIT6_INT_ENA_V  0x1
#define HOST_SLC1_TOHOST_BIT6_INT_ENA_S  6
/* HOST_SLC1_TOHOST_BIT5_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT5_INT_ENA  (BIT(5))
#define HOST_SLC1_TOHOST_BIT5_INT_ENA_M  (BIT(5))
#define HOST_SLC1_TOHOST_BIT5_INT_ENA_V  0x1
#define HOST_SLC1_TOHOST_BIT5_INT_ENA_S  5
/* HOST_SLC1_TOHOST_BIT4_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT4_INT_ENA  (BIT(4))
#define HOST_SLC1_TOHOST_BIT4_INT_ENA_M  (BIT(4))
#define HOST_SLC1_TOHOST_BIT4_INT_ENA_V  0x1
#define HOST_SLC1_TOHOST_BIT4_INT_ENA_S  4
/* HOST_SLC1_TOHOST_BIT3_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT3_INT_ENA  (BIT(3))
#define HOST_SLC1_TOHOST_BIT3_INT_ENA_M  (BIT(3))
#define HOST_SLC1_TOHOST_BIT3_INT_ENA_V  0x1
#define HOST_SLC1_TOHOST_BIT3_INT_ENA_S  3
/* HOST_SLC1_TOHOST_BIT2_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT2_INT_ENA  (BIT(2))
#define HOST_SLC1_TOHOST_BIT2_INT_ENA_M  (BIT(2))
#define HOST_SLC1_TOHOST_BIT2_INT_ENA_V  0x1
#define HOST_SLC1_TOHOST_BIT2_INT_ENA_S  2
/* HOST_SLC1_TOHOST_BIT1_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT1_INT_ENA  (BIT(1))
#define HOST_SLC1_TOHOST_BIT1_INT_ENA_M  (BIT(1))
#define HOST_SLC1_TOHOST_BIT1_INT_ENA_V  0x1
#define HOST_SLC1_TOHOST_BIT1_INT_ENA_S  1
/* HOST_SLC1_TOHOST_BIT0_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT0_INT_ENA  (BIT(0))
#define HOST_SLC1_TOHOST_BIT0_INT_ENA_M  (BIT(0))
#define HOST_SLC1_TOHOST_BIT0_INT_ENA_V  0x1
#define HOST_SLC1_TOHOST_BIT0_INT_ENA_S  0

#define HOST_SLC0HOST_RX_INFOR_REG          (DR_REG_SLCHOST_BASE + 0xF4)
/* HOST_SLC0HOST_RX_INFOR : R/W ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: */
#define HOST_SLC0HOST_RX_INFOR  0x000FFFFF
#define HOST_SLC0HOST_RX_INFOR_M  ((HOST_SLC0HOST_RX_INFOR_V)<<(HOST_SLC0HOST_RX_INFOR_S))
#define HOST_SLC0HOST_RX_INFOR_V  0xFFFFF
#define HOST_SLC0HOST_RX_INFOR_S  0

#define HOST_SLC1HOST_RX_INFOR_REG          (DR_REG_SLCHOST_BASE + 0xF8)
/* HOST_SLC1HOST_RX_INFOR : R/W ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: */
#define HOST_SLC1HOST_RX_INFOR  0x000FFFFF
#define HOST_SLC1HOST_RX_INFOR_M  ((HOST_SLC1HOST_RX_INFOR_V)<<(HOST_SLC1HOST_RX_INFOR_S))
#define HOST_SLC1HOST_RX_INFOR_V  0xFFFFF
#define HOST_SLC1HOST_RX_INFOR_S  0

#define HOST_SLC0HOST_LEN_WD_REG          (DR_REG_SLCHOST_BASE + 0xFC)
/* HOST_SLC0HOST_LEN_WD : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define HOST_SLC0HOST_LEN_WD  0xFFFFFFFF
#define HOST_SLC0HOST_LEN_WD_M  ((HOST_SLC0HOST_LEN_WD_V)<<(HOST_SLC0HOST_LEN_WD_S))
#define HOST_SLC0HOST_LEN_WD_V  0xFFFFFFFF
#define HOST_SLC0HOST_LEN_WD_S  0

#define HOST_SLC_APBWIN_WDATA_REG          (DR_REG_SLCHOST_BASE + 0x100)
/* HOST_SLC_APBWIN_WDATA : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define HOST_SLC_APBWIN_WDATA  0xFFFFFFFF
#define HOST_SLC_APBWIN_WDATA_M  ((HOST_SLC_APBWIN_WDATA_V)<<(HOST_SLC_APBWIN_WDATA_S))
#define HOST_SLC_APBWIN_WDATA_V  0xFFFFFFFF
#define HOST_SLC_APBWIN_WDATA_S  0

#define HOST_SLC_APBWIN_CONF_REG          (DR_REG_SLCHOST_BASE + 0x104)
/* HOST_SLC_APBWIN_START : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC_APBWIN_START  (BIT(29))
#define HOST_SLC_APBWIN_START_M  (BIT(29))
#define HOST_SLC_APBWIN_START_V  0x1
#define HOST_SLC_APBWIN_START_S  29
/* HOST_SLC_APBWIN_WR : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC_APBWIN_WR  (BIT(28))
#define HOST_SLC_APBWIN_WR_M  (BIT(28))
#define HOST_SLC_APBWIN_WR_V  0x1
#define HOST_SLC_APBWIN_WR_S  28
/* HOST_SLC_APBWIN_ADDR : R/W ;bitpos:[27:0] ;default: 28'b0 ; */
/*description: */
#define HOST_SLC_APBWIN_ADDR  0x0FFFFFFF
#define HOST_SLC_APBWIN_ADDR_M  ((HOST_SLC_APBWIN_ADDR_V)<<(HOST_SLC_APBWIN_ADDR_S))
#define HOST_SLC_APBWIN_ADDR_V  0xFFFFFFF
#define HOST_SLC_APBWIN_ADDR_S  0

#define HOST_SLC_APBWIN_RDATA_REG          (DR_REG_SLCHOST_BASE + 0x108)
/* HOST_SLC_APBWIN_RDATA : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define HOST_SLC_APBWIN_RDATA  0xFFFFFFFF
#define HOST_SLC_APBWIN_RDATA_M  ((HOST_SLC_APBWIN_RDATA_V)<<(HOST_SLC_APBWIN_RDATA_S))
#define HOST_SLC_APBWIN_RDATA_V  0xFFFFFFFF
#define HOST_SLC_APBWIN_RDATA_S  0

#define HOST_SLCHOST_RDCLR0_REG          (DR_REG_SLCHOST_BASE + 0x10C)
/* HOST_SLCHOST_SLC0_BIT6_CLRADDR : R/W ;bitpos:[17:9] ;default: 9'h1e0 ; */
/*description: */
#define HOST_SLCHOST_SLC0_BIT6_CLRADDR  0x000001FF
#define HOST_SLCHOST_SLC0_BIT6_CLRADDR_M  ((HOST_SLCHOST_SLC0_BIT6_CLRADDR_V)<<(HOST_SLCHOST_SLC0_BIT6_CLRADDR_S))
#define HOST_SLCHOST_SLC0_BIT6_CLRADDR_V  0x1FF
#define HOST_SLCHOST_SLC0_BIT6_CLRADDR_S  9
/* HOST_SLCHOST_SLC0_BIT7_CLRADDR : R/W ;bitpos:[8:0] ;default: 9'h44 ; */
/*description: */
#define HOST_SLCHOST_SLC0_BIT7_CLRADDR  0x000001FF
#define HOST_SLCHOST_SLC0_BIT7_CLRADDR_M  ((HOST_SLCHOST_SLC0_BIT7_CLRADDR_V)<<(HOST_SLCHOST_SLC0_BIT7_CLRADDR_S))
#define HOST_SLCHOST_SLC0_BIT7_CLRADDR_V  0x1FF
#define HOST_SLCHOST_SLC0_BIT7_CLRADDR_S  0

#define HOST_SLCHOST_RDCLR1_REG          (DR_REG_SLCHOST_BASE + 0x110)
/* HOST_SLCHOST_SLC1_BIT6_CLRADDR : R/W ;bitpos:[17:9] ;default: 9'h1e0 ; */
/*description: */
#define HOST_SLCHOST_SLC1_BIT6_CLRADDR  0x000001FF
#define HOST_SLCHOST_SLC1_BIT6_CLRADDR_M  ((HOST_SLCHOST_SLC1_BIT6_CLRADDR_V)<<(HOST_SLCHOST_SLC1_BIT6_CLRADDR_S))
#define HOST_SLCHOST_SLC1_BIT6_CLRADDR_V  0x1FF
#define HOST_SLCHOST_SLC1_BIT6_CLRADDR_S  9
/* HOST_SLCHOST_SLC1_BIT7_CLRADDR : R/W ;bitpos:[8:0] ;default: 9'h1e0 ; */
/*description: */
#define HOST_SLCHOST_SLC1_BIT7_CLRADDR  0x000001FF
#define HOST_SLCHOST_SLC1_BIT7_CLRADDR_M  ((HOST_SLCHOST_SLC1_BIT7_CLRADDR_V)<<(HOST_SLCHOST_SLC1_BIT7_CLRADDR_S))
#define HOST_SLCHOST_SLC1_BIT7_CLRADDR_V  0x1FF
#define HOST_SLCHOST_SLC1_BIT7_CLRADDR_S  0

#define HOST_SLC0HOST_INT_ENA1_REG          (DR_REG_SLCHOST_BASE + 0x114)
/* HOST_GPIO_SDIO_INT_ENA1 : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define HOST_GPIO_SDIO_INT_ENA1  (BIT(25))
#define HOST_GPIO_SDIO_INT_ENA1_M  (BIT(25))
#define HOST_GPIO_SDIO_INT_ENA1_V  0x1
#define HOST_GPIO_SDIO_INT_ENA1_S  25
/* HOST_SLC0_HOST_RD_RETRY_INT_ENA1 : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_HOST_RD_RETRY_INT_ENA1  (BIT(24))
#define HOST_SLC0_HOST_RD_RETRY_INT_ENA1_M  (BIT(24))
#define HOST_SLC0_HOST_RD_RETRY_INT_ENA1_V  0x1
#define HOST_SLC0_HOST_RD_RETRY_INT_ENA1_S  24
/* HOST_SLC0_RX_NEW_PACKET_INT_ENA1 : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_RX_NEW_PACKET_INT_ENA1  (BIT(23))
#define HOST_SLC0_RX_NEW_PACKET_INT_ENA1_M  (BIT(23))
#define HOST_SLC0_RX_NEW_PACKET_INT_ENA1_V  0x1
#define HOST_SLC0_RX_NEW_PACKET_INT_ENA1_S  23
/* HOST_SLC0_EXT_BIT3_INT_ENA1 : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_EXT_BIT3_INT_ENA1  (BIT(22))
#define HOST_SLC0_EXT_BIT3_INT_ENA1_M  (BIT(22))
#define HOST_SLC0_EXT_BIT3_INT_ENA1_V  0x1
#define HOST_SLC0_EXT_BIT3_INT_ENA1_S  22
/* HOST_SLC0_EXT_BIT2_INT_ENA1 : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_EXT_BIT2_INT_ENA1  (BIT(21))
#define HOST_SLC0_EXT_BIT2_INT_ENA1_M  (BIT(21))
#define HOST_SLC0_EXT_BIT2_INT_ENA1_V  0x1
#define HOST_SLC0_EXT_BIT2_INT_ENA1_S  21
/* HOST_SLC0_EXT_BIT1_INT_ENA1 : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_EXT_BIT1_INT_ENA1  (BIT(20))
#define HOST_SLC0_EXT_BIT1_INT_ENA1_M  (BIT(20))
#define HOST_SLC0_EXT_BIT1_INT_ENA1_V  0x1
#define HOST_SLC0_EXT_BIT1_INT_ENA1_S  20
/* HOST_SLC0_EXT_BIT0_INT_ENA1 : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_EXT_BIT0_INT_ENA1  (BIT(19))
#define HOST_SLC0_EXT_BIT0_INT_ENA1_M  (BIT(19))
#define HOST_SLC0_EXT_BIT0_INT_ENA1_V  0x1
#define HOST_SLC0_EXT_BIT0_INT_ENA1_S  19
/* HOST_SLC0_RX_PF_VALID_INT_ENA1 : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_RX_PF_VALID_INT_ENA1  (BIT(18))
#define HOST_SLC0_RX_PF_VALID_INT_ENA1_M  (BIT(18))
#define HOST_SLC0_RX_PF_VALID_INT_ENA1_V  0x1
#define HOST_SLC0_RX_PF_VALID_INT_ENA1_S  18
/* HOST_SLC0_TX_OVF_INT_ENA1 : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TX_OVF_INT_ENA1  (BIT(17))
#define HOST_SLC0_TX_OVF_INT_ENA1_M  (BIT(17))
#define HOST_SLC0_TX_OVF_INT_ENA1_V  0x1
#define HOST_SLC0_TX_OVF_INT_ENA1_S  17
/* HOST_SLC0_RX_UDF_INT_ENA1 : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_RX_UDF_INT_ENA1  (BIT(16))
#define HOST_SLC0_RX_UDF_INT_ENA1_M  (BIT(16))
#define HOST_SLC0_RX_UDF_INT_ENA1_V  0x1
#define HOST_SLC0_RX_UDF_INT_ENA1_S  16
/* HOST_SLC0HOST_TX_START_INT_ENA1 : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_TX_START_INT_ENA1  (BIT(15))
#define HOST_SLC0HOST_TX_START_INT_ENA1_M  (BIT(15))
#define HOST_SLC0HOST_TX_START_INT_ENA1_V  0x1
#define HOST_SLC0HOST_TX_START_INT_ENA1_S  15
/* HOST_SLC0HOST_RX_START_INT_ENA1 : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_RX_START_INT_ENA1  (BIT(14))
#define HOST_SLC0HOST_RX_START_INT_ENA1_M  (BIT(14))
#define HOST_SLC0HOST_RX_START_INT_ENA1_V  0x1
#define HOST_SLC0HOST_RX_START_INT_ENA1_S  14
/* HOST_SLC0HOST_RX_EOF_INT_ENA1 : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_RX_EOF_INT_ENA1  (BIT(13))
#define HOST_SLC0HOST_RX_EOF_INT_ENA1_M  (BIT(13))
#define HOST_SLC0HOST_RX_EOF_INT_ENA1_V  0x1
#define HOST_SLC0HOST_RX_EOF_INT_ENA1_S  13
/* HOST_SLC0HOST_RX_SOF_INT_ENA1 : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0HOST_RX_SOF_INT_ENA1  (BIT(12))
#define HOST_SLC0HOST_RX_SOF_INT_ENA1_M  (BIT(12))
#define HOST_SLC0HOST_RX_SOF_INT_ENA1_V  0x1
#define HOST_SLC0HOST_RX_SOF_INT_ENA1_S  12
/* HOST_SLC0_TOKEN1_0TO1_INT_ENA1 : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOKEN1_0TO1_INT_ENA1  (BIT(11))
#define HOST_SLC0_TOKEN1_0TO1_INT_ENA1_M  (BIT(11))
#define HOST_SLC0_TOKEN1_0TO1_INT_ENA1_V  0x1
#define HOST_SLC0_TOKEN1_0TO1_INT_ENA1_S  11
/* HOST_SLC0_TOKEN0_0TO1_INT_ENA1 : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOKEN0_0TO1_INT_ENA1  (BIT(10))
#define HOST_SLC0_TOKEN0_0TO1_INT_ENA1_M  (BIT(10))
#define HOST_SLC0_TOKEN0_0TO1_INT_ENA1_V  0x1
#define HOST_SLC0_TOKEN0_0TO1_INT_ENA1_S  10
/* HOST_SLC0_TOKEN1_1TO0_INT_ENA1 : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOKEN1_1TO0_INT_ENA1  (BIT(9))
#define HOST_SLC0_TOKEN1_1TO0_INT_ENA1_M  (BIT(9))
#define HOST_SLC0_TOKEN1_1TO0_INT_ENA1_V  0x1
#define HOST_SLC0_TOKEN1_1TO0_INT_ENA1_S  9
/* HOST_SLC0_TOKEN0_1TO0_INT_ENA1 : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOKEN0_1TO0_INT_ENA1  (BIT(8))
#define HOST_SLC0_TOKEN0_1TO0_INT_ENA1_M  (BIT(8))
#define HOST_SLC0_TOKEN0_1TO0_INT_ENA1_V  0x1
#define HOST_SLC0_TOKEN0_1TO0_INT_ENA1_S  8
/* HOST_SLC0_TOHOST_BIT7_INT_ENA1 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT7_INT_ENA1  (BIT(7))
#define HOST_SLC0_TOHOST_BIT7_INT_ENA1_M  (BIT(7))
#define HOST_SLC0_TOHOST_BIT7_INT_ENA1_V  0x1
#define HOST_SLC0_TOHOST_BIT7_INT_ENA1_S  7
/* HOST_SLC0_TOHOST_BIT6_INT_ENA1 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT6_INT_ENA1  (BIT(6))
#define HOST_SLC0_TOHOST_BIT6_INT_ENA1_M  (BIT(6))
#define HOST_SLC0_TOHOST_BIT6_INT_ENA1_V  0x1
#define HOST_SLC0_TOHOST_BIT6_INT_ENA1_S  6
/* HOST_SLC0_TOHOST_BIT5_INT_ENA1 : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT5_INT_ENA1  (BIT(5))
#define HOST_SLC0_TOHOST_BIT5_INT_ENA1_M  (BIT(5))
#define HOST_SLC0_TOHOST_BIT5_INT_ENA1_V  0x1
#define HOST_SLC0_TOHOST_BIT5_INT_ENA1_S  5
/* HOST_SLC0_TOHOST_BIT4_INT_ENA1 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT4_INT_ENA1  (BIT(4))
#define HOST_SLC0_TOHOST_BIT4_INT_ENA1_M  (BIT(4))
#define HOST_SLC0_TOHOST_BIT4_INT_ENA1_V  0x1
#define HOST_SLC0_TOHOST_BIT4_INT_ENA1_S  4
/* HOST_SLC0_TOHOST_BIT3_INT_ENA1 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT3_INT_ENA1  (BIT(3))
#define HOST_SLC0_TOHOST_BIT3_INT_ENA1_M  (BIT(3))
#define HOST_SLC0_TOHOST_BIT3_INT_ENA1_V  0x1
#define HOST_SLC0_TOHOST_BIT3_INT_ENA1_S  3
/* HOST_SLC0_TOHOST_BIT2_INT_ENA1 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT2_INT_ENA1  (BIT(2))
#define HOST_SLC0_TOHOST_BIT2_INT_ENA1_M  (BIT(2))
#define HOST_SLC0_TOHOST_BIT2_INT_ENA1_V  0x1
#define HOST_SLC0_TOHOST_BIT2_INT_ENA1_S  2
/* HOST_SLC0_TOHOST_BIT1_INT_ENA1 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT1_INT_ENA1  (BIT(1))
#define HOST_SLC0_TOHOST_BIT1_INT_ENA1_M  (BIT(1))
#define HOST_SLC0_TOHOST_BIT1_INT_ENA1_V  0x1
#define HOST_SLC0_TOHOST_BIT1_INT_ENA1_S  1
/* HOST_SLC0_TOHOST_BIT0_INT_ENA1 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC0_TOHOST_BIT0_INT_ENA1  (BIT(0))
#define HOST_SLC0_TOHOST_BIT0_INT_ENA1_M  (BIT(0))
#define HOST_SLC0_TOHOST_BIT0_INT_ENA1_V  0x1
#define HOST_SLC0_TOHOST_BIT0_INT_ENA1_S  0

#define HOST_SLC1HOST_INT_ENA1_REG          (DR_REG_SLCHOST_BASE + 0x118)
/* HOST_SLC1_BT_RX_NEW_PACKET_INT_ENA1 : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_BT_RX_NEW_PACKET_INT_ENA1  (BIT(25))
#define HOST_SLC1_BT_RX_NEW_PACKET_INT_ENA1_M  (BIT(25))
#define HOST_SLC1_BT_RX_NEW_PACKET_INT_ENA1_V  0x1
#define HOST_SLC1_BT_RX_NEW_PACKET_INT_ENA1_S  25
/* HOST_SLC1_HOST_RD_RETRY_INT_ENA1 : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_HOST_RD_RETRY_INT_ENA1  (BIT(24))
#define HOST_SLC1_HOST_RD_RETRY_INT_ENA1_M  (BIT(24))
#define HOST_SLC1_HOST_RD_RETRY_INT_ENA1_V  0x1
#define HOST_SLC1_HOST_RD_RETRY_INT_ENA1_S  24
/* HOST_SLC1_WIFI_RX_NEW_PACKET_INT_ENA1 : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_WIFI_RX_NEW_PACKET_INT_ENA1  (BIT(23))
#define HOST_SLC1_WIFI_RX_NEW_PACKET_INT_ENA1_M  (BIT(23))
#define HOST_SLC1_WIFI_RX_NEW_PACKET_INT_ENA1_V  0x1
#define HOST_SLC1_WIFI_RX_NEW_PACKET_INT_ENA1_S  23
/* HOST_SLC1_EXT_BIT3_INT_ENA1 : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_EXT_BIT3_INT_ENA1  (BIT(22))
#define HOST_SLC1_EXT_BIT3_INT_ENA1_M  (BIT(22))
#define HOST_SLC1_EXT_BIT3_INT_ENA1_V  0x1
#define HOST_SLC1_EXT_BIT3_INT_ENA1_S  22
/* HOST_SLC1_EXT_BIT2_INT_ENA1 : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_EXT_BIT2_INT_ENA1  (BIT(21))
#define HOST_SLC1_EXT_BIT2_INT_ENA1_M  (BIT(21))
#define HOST_SLC1_EXT_BIT2_INT_ENA1_V  0x1
#define HOST_SLC1_EXT_BIT2_INT_ENA1_S  21
/* HOST_SLC1_EXT_BIT1_INT_ENA1 : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_EXT_BIT1_INT_ENA1  (BIT(20))
#define HOST_SLC1_EXT_BIT1_INT_ENA1_M  (BIT(20))
#define HOST_SLC1_EXT_BIT1_INT_ENA1_V  0x1
#define HOST_SLC1_EXT_BIT1_INT_ENA1_S  20
/* HOST_SLC1_EXT_BIT0_INT_ENA1 : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_EXT_BIT0_INT_ENA1  (BIT(19))
#define HOST_SLC1_EXT_BIT0_INT_ENA1_M  (BIT(19))
#define HOST_SLC1_EXT_BIT0_INT_ENA1_V  0x1
#define HOST_SLC1_EXT_BIT0_INT_ENA1_S  19
/* HOST_SLC1_RX_PF_VALID_INT_ENA1 : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_RX_PF_VALID_INT_ENA1  (BIT(18))
#define HOST_SLC1_RX_PF_VALID_INT_ENA1_M  (BIT(18))
#define HOST_SLC1_RX_PF_VALID_INT_ENA1_V  0x1
#define HOST_SLC1_RX_PF_VALID_INT_ENA1_S  18
/* HOST_SLC1_TX_OVF_INT_ENA1 : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TX_OVF_INT_ENA1  (BIT(17))
#define HOST_SLC1_TX_OVF_INT_ENA1_M  (BIT(17))
#define HOST_SLC1_TX_OVF_INT_ENA1_V  0x1
#define HOST_SLC1_TX_OVF_INT_ENA1_S  17
/* HOST_SLC1_RX_UDF_INT_ENA1 : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_RX_UDF_INT_ENA1  (BIT(16))
#define HOST_SLC1_RX_UDF_INT_ENA1_M  (BIT(16))
#define HOST_SLC1_RX_UDF_INT_ENA1_V  0x1
#define HOST_SLC1_RX_UDF_INT_ENA1_S  16
/* HOST_SLC1HOST_TX_START_INT_ENA1 : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_TX_START_INT_ENA1  (BIT(15))
#define HOST_SLC1HOST_TX_START_INT_ENA1_M  (BIT(15))
#define HOST_SLC1HOST_TX_START_INT_ENA1_V  0x1
#define HOST_SLC1HOST_TX_START_INT_ENA1_S  15
/* HOST_SLC1HOST_RX_START_INT_ENA1 : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_RX_START_INT_ENA1  (BIT(14))
#define HOST_SLC1HOST_RX_START_INT_ENA1_M  (BIT(14))
#define HOST_SLC1HOST_RX_START_INT_ENA1_V  0x1
#define HOST_SLC1HOST_RX_START_INT_ENA1_S  14
/* HOST_SLC1HOST_RX_EOF_INT_ENA1 : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_RX_EOF_INT_ENA1  (BIT(13))
#define HOST_SLC1HOST_RX_EOF_INT_ENA1_M  (BIT(13))
#define HOST_SLC1HOST_RX_EOF_INT_ENA1_V  0x1
#define HOST_SLC1HOST_RX_EOF_INT_ENA1_S  13
/* HOST_SLC1HOST_RX_SOF_INT_ENA1 : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1HOST_RX_SOF_INT_ENA1  (BIT(12))
#define HOST_SLC1HOST_RX_SOF_INT_ENA1_M  (BIT(12))
#define HOST_SLC1HOST_RX_SOF_INT_ENA1_V  0x1
#define HOST_SLC1HOST_RX_SOF_INT_ENA1_S  12
/* HOST_SLC1_TOKEN1_0TO1_INT_ENA1 : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOKEN1_0TO1_INT_ENA1  (BIT(11))
#define HOST_SLC1_TOKEN1_0TO1_INT_ENA1_M  (BIT(11))
#define HOST_SLC1_TOKEN1_0TO1_INT_ENA1_V  0x1
#define HOST_SLC1_TOKEN1_0TO1_INT_ENA1_S  11
/* HOST_SLC1_TOKEN0_0TO1_INT_ENA1 : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOKEN0_0TO1_INT_ENA1  (BIT(10))
#define HOST_SLC1_TOKEN0_0TO1_INT_ENA1_M  (BIT(10))
#define HOST_SLC1_TOKEN0_0TO1_INT_ENA1_V  0x1
#define HOST_SLC1_TOKEN0_0TO1_INT_ENA1_S  10
/* HOST_SLC1_TOKEN1_1TO0_INT_ENA1 : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOKEN1_1TO0_INT_ENA1  (BIT(9))
#define HOST_SLC1_TOKEN1_1TO0_INT_ENA1_M  (BIT(9))
#define HOST_SLC1_TOKEN1_1TO0_INT_ENA1_V  0x1
#define HOST_SLC1_TOKEN1_1TO0_INT_ENA1_S  9
/* HOST_SLC1_TOKEN0_1TO0_INT_ENA1 : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOKEN0_1TO0_INT_ENA1  (BIT(8))
#define HOST_SLC1_TOKEN0_1TO0_INT_ENA1_M  (BIT(8))
#define HOST_SLC1_TOKEN0_1TO0_INT_ENA1_V  0x1
#define HOST_SLC1_TOKEN0_1TO0_INT_ENA1_S  8
/* HOST_SLC1_TOHOST_BIT7_INT_ENA1 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT7_INT_ENA1  (BIT(7))
#define HOST_SLC1_TOHOST_BIT7_INT_ENA1_M  (BIT(7))
#define HOST_SLC1_TOHOST_BIT7_INT_ENA1_V  0x1
#define HOST_SLC1_TOHOST_BIT7_INT_ENA1_S  7
/* HOST_SLC1_TOHOST_BIT6_INT_ENA1 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT6_INT_ENA1  (BIT(6))
#define HOST_SLC1_TOHOST_BIT6_INT_ENA1_M  (BIT(6))
#define HOST_SLC1_TOHOST_BIT6_INT_ENA1_V  0x1
#define HOST_SLC1_TOHOST_BIT6_INT_ENA1_S  6
/* HOST_SLC1_TOHOST_BIT5_INT_ENA1 : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT5_INT_ENA1  (BIT(5))
#define HOST_SLC1_TOHOST_BIT5_INT_ENA1_M  (BIT(5))
#define HOST_SLC1_TOHOST_BIT5_INT_ENA1_V  0x1
#define HOST_SLC1_TOHOST_BIT5_INT_ENA1_S  5
/* HOST_SLC1_TOHOST_BIT4_INT_ENA1 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT4_INT_ENA1  (BIT(4))
#define HOST_SLC1_TOHOST_BIT4_INT_ENA1_M  (BIT(4))
#define HOST_SLC1_TOHOST_BIT4_INT_ENA1_V  0x1
#define HOST_SLC1_TOHOST_BIT4_INT_ENA1_S  4
/* HOST_SLC1_TOHOST_BIT3_INT_ENA1 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT3_INT_ENA1  (BIT(3))
#define HOST_SLC1_TOHOST_BIT3_INT_ENA1_M  (BIT(3))
#define HOST_SLC1_TOHOST_BIT3_INT_ENA1_V  0x1
#define HOST_SLC1_TOHOST_BIT3_INT_ENA1_S  3
/* HOST_SLC1_TOHOST_BIT2_INT_ENA1 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT2_INT_ENA1  (BIT(2))
#define HOST_SLC1_TOHOST_BIT2_INT_ENA1_M  (BIT(2))
#define HOST_SLC1_TOHOST_BIT2_INT_ENA1_V  0x1
#define HOST_SLC1_TOHOST_BIT2_INT_ENA1_S  2
/* HOST_SLC1_TOHOST_BIT1_INT_ENA1 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT1_INT_ENA1  (BIT(1))
#define HOST_SLC1_TOHOST_BIT1_INT_ENA1_M  (BIT(1))
#define HOST_SLC1_TOHOST_BIT1_INT_ENA1_V  0x1
#define HOST_SLC1_TOHOST_BIT1_INT_ENA1_S  1
/* HOST_SLC1_TOHOST_BIT0_INT_ENA1 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define HOST_SLC1_TOHOST_BIT0_INT_ENA1  (BIT(0))
#define HOST_SLC1_TOHOST_BIT0_INT_ENA1_M  (BIT(0))
#define HOST_SLC1_TOHOST_BIT0_INT_ENA1_V  0x1
#define HOST_SLC1_TOHOST_BIT0_INT_ENA1_S  0

#define HOST_SLCHOSTDATE_REG          (DR_REG_SLCHOST_BASE + 0x178)
/* HOST_SLCHOST_DATE : R/W ;bitpos:[31:0] ;default: 32'h16022500 ; */
/*description: */
#define HOST_SLCHOST_DATE  0xFFFFFFFF
#define HOST_SLCHOST_DATE_M  ((HOST_SLCHOST_DATE_V)<<(HOST_SLCHOST_DATE_S))
#define HOST_SLCHOST_DATE_V  0xFFFFFFFF
#define HOST_SLCHOST_DATE_S  0

#define HOST_SLCHOSTID_REG          (DR_REG_SLCHOST_BASE + 0x17C)
/* HOST_SLCHOST_ID : R/W ;bitpos:[31:0] ;default: 32'h0600 ; */
/*description: */
#define HOST_SLCHOST_ID  0xFFFFFFFF
#define HOST_SLCHOST_ID_M  ((HOST_SLCHOST_ID_V)<<(HOST_SLCHOST_ID_S))
#define HOST_SLCHOST_ID_V  0xFFFFFFFF
#define HOST_SLCHOST_ID_S  0

#define HOST_SLCHOST_CONF_REG          (DR_REG_SLCHOST_BASE + 0x1F0)
/* HOST_HSPEED_CON_EN : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: */
#define HOST_HSPEED_CON_EN  (BIT(27))
#define HOST_HSPEED_CON_EN_M  (BIT(27))
#define HOST_HSPEED_CON_EN_V  0x1
#define HOST_HSPEED_CON_EN_S  27
/* HOST_SDIO_PAD_PULLUP : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define HOST_SDIO_PAD_PULLUP  (BIT(26))
#define HOST_SDIO_PAD_PULLUP_M  (BIT(26))
#define HOST_SDIO_PAD_PULLUP_V  0x1
#define HOST_SDIO_PAD_PULLUP_S  26
/* HOST_SDIO20_INT_DELAY : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define HOST_SDIO20_INT_DELAY  (BIT(25))
#define HOST_SDIO20_INT_DELAY_M  (BIT(25))
#define HOST_SDIO20_INT_DELAY_V  0x1
#define HOST_SDIO20_INT_DELAY_S  25
/* HOST_FRC_QUICK_IN : R/W ;bitpos:[24:20] ;default: 5'b0 ; */
/*description: */
#define HOST_FRC_QUICK_IN  0x0000001F
#define HOST_FRC_QUICK_IN_M  ((HOST_FRC_QUICK_IN_V)<<(HOST_FRC_QUICK_IN_S))
#define HOST_FRC_QUICK_IN_V  0x1F
#define HOST_FRC_QUICK_IN_S  20
/* HOST_FRC_POS_SAMP : R/W ;bitpos:[19:15] ;default: 5'b0 ; */
/*description: */
#define HOST_FRC_POS_SAMP  0x0000001F
#define HOST_FRC_POS_SAMP_M  ((HOST_FRC_POS_SAMP_V)<<(HOST_FRC_POS_SAMP_S))
#define HOST_FRC_POS_SAMP_V  0x1F
#define HOST_FRC_POS_SAMP_S  15
/* HOST_FRC_NEG_SAMP : R/W ;bitpos:[14:10] ;default: 5'b0 ; */
/*description: */
#define HOST_FRC_NEG_SAMP  0x0000001F
#define HOST_FRC_NEG_SAMP_M  ((HOST_FRC_NEG_SAMP_V)<<(HOST_FRC_NEG_SAMP_S))
#define HOST_FRC_NEG_SAMP_V  0x1F
#define HOST_FRC_NEG_SAMP_S  10
/* HOST_FRC_SDIO20 : R/W ;bitpos:[9:5] ;default: 5'b0 ; */
/*description: */
#define HOST_FRC_SDIO20  0x0000001F
#define HOST_FRC_SDIO20_M  ((HOST_FRC_SDIO20_V)<<(HOST_FRC_SDIO20_S))
#define HOST_FRC_SDIO20_V  0x1F
#define HOST_FRC_SDIO20_S  5
/* HOST_FRC_SDIO11 : R/W ;bitpos:[4:0] ;default: 5'b0 ; */
/*description: */
#define HOST_FRC_SDIO11  0x0000001F
#define HOST_FRC_SDIO11_M  ((HOST_FRC_SDIO11_V)<<(HOST_FRC_SDIO11_S))
#define HOST_FRC_SDIO11_V  0x1F
#define HOST_FRC_SDIO11_S  0

#define HOST_SLCHOST_INF_ST_REG          (DR_REG_SLCHOST_BASE + 0x1F4)
/* HOST_SDIO_QUICK_IN : RO ;bitpos:[14:10] ;default: 5'b0 ; */
/*description: */
#define HOST_SDIO_QUICK_IN  0x0000001F
#define HOST_SDIO_QUICK_IN_M  ((HOST_SDIO_QUICK_IN_V)<<(HOST_SDIO_QUICK_IN_S))
#define HOST_SDIO_QUICK_IN_V  0x1F
#define HOST_SDIO_QUICK_IN_S  10
/* HOST_SDIO_NEG_SAMP : RO ;bitpos:[9:5] ;default: 5'b0 ; */
/*description: */
#define HOST_SDIO_NEG_SAMP  0x0000001F
#define HOST_SDIO_NEG_SAMP_M  ((HOST_SDIO_NEG_SAMP_V)<<(HOST_SDIO_NEG_SAMP_S))
#define HOST_SDIO_NEG_SAMP_V  0x1F
#define HOST_SDIO_NEG_SAMP_S  5
/* HOST_SDIO20_MODE : RO ;bitpos:[4:0] ;default: 5'b0 ; */
/*description: */
#define HOST_SDIO20_MODE  0x0000001F
#define HOST_SDIO20_MODE_M  ((HOST_SDIO20_MODE_V)<<(HOST_SDIO20_MODE_S))
#define HOST_SDIO20_MODE_V  0x1F
#define HOST_SDIO20_MODE_S  0




#endif /*_SOC_HOST_REG_H_ */


