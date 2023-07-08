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

/** I3C_MST_MEM_COMMAND_BUF_PORT_REG register
 *  NA
 */
#define I3C_MST_MEM_COMMAND_BUF_PORT_REG (DR_REG_I3C_MST_MEM_BASE + 0x8)
/** I3C_MST_MEM_REG_COMMAND : R/W; bitpos: [31:0]; default: 0;
 *  Contains a Command Descriptor structure that depends on the requested transfer
 *  type. Command Descriptor structure is used to schedule the transfers to devices on
 *  I3C bus.
 */
#define I3C_MST_MEM_REG_COMMAND    0xFFFFFFFFU
#define I3C_MST_MEM_REG_COMMAND_M  (I3C_MST_MEM_REG_COMMAND_V << I3C_MST_MEM_REG_COMMAND_S)
#define I3C_MST_MEM_REG_COMMAND_V  0xFFFFFFFFU
#define I3C_MST_MEM_REG_COMMAND_S  0

/** I3C_MST_MEM_RESPONSE_BUF_PORT_REG register
 *  NA
 */
#define I3C_MST_MEM_RESPONSE_BUF_PORT_REG (DR_REG_I3C_MST_MEM_BASE + 0xc)
/** I3C_MST_MEM_RESPONSE : RO; bitpos: [31:0]; default: 0;
 *  The Response Buffer can be read through this register. The response status for each
 *  Command is written into the Response Buffer by the controller if ROC (Response On
 *  Completion) bit is set or if transfer error has occurred. The response buffer can
 *  be read through this register.
 */
#define I3C_MST_MEM_RESPONSE    0xFFFFFFFFU
#define I3C_MST_MEM_RESPONSE_M  (I3C_MST_MEM_RESPONSE_V << I3C_MST_MEM_RESPONSE_S)
#define I3C_MST_MEM_RESPONSE_V  0xFFFFFFFFU
#define I3C_MST_MEM_RESPONSE_S  0

/** I3C_MST_MEM_RX_DATA_PORT_REG register
 *  NA
 */
#define I3C_MST_MEM_RX_DATA_PORT_REG (DR_REG_I3C_MST_MEM_BASE + 0x10)
/** I3C_MST_MEM_RX_DATA_PORT : RO; bitpos: [31:0]; default: 0;
 *  Receive Data Port. Receive data is mapped to the Rx-data buffer and receive data is
 *  always packed in 4-byte aligned data words.  If the length of data transfer is not
 *  aligned to 4-bytes boundary, then there will be extra(unused) bytes(the additional
 *  data bytes have to be ignored) at the end of the transferred data. The valid data
 *  must be identified using the DATA_LENGTH filed in the Response Descriptor.
 */
#define I3C_MST_MEM_RX_DATA_PORT    0xFFFFFFFFU
#define I3C_MST_MEM_RX_DATA_PORT_M  (I3C_MST_MEM_RX_DATA_PORT_V << I3C_MST_MEM_RX_DATA_PORT_S)
#define I3C_MST_MEM_RX_DATA_PORT_V  0xFFFFFFFFU
#define I3C_MST_MEM_RX_DATA_PORT_S  0

/** I3C_MST_MEM_TX_DATA_PORT_REG register
 *  NA
 */
#define I3C_MST_MEM_TX_DATA_PORT_REG (DR_REG_I3C_MST_MEM_BASE + 0x14)
/** I3C_MST_MEM_REG_TX_DATA_PORT : R/W; bitpos: [31:0]; default: 0;
 *  Transmit Data Port. Transmit data is mapped to the Tx-data  buffer and transmit
 *  data is always packed in 4-byte aligned data words.  If the length of data transfer
 *  is not aligned to 4-bytes boundary, then there will be extra(unused) bytes(the
 *  additional data bytes have to be ignored) at the end of the transferred data. The
 *  valid data must be identified using the DATA_LENGTH filed in the Response
 *  Descriptor.
 */
#define I3C_MST_MEM_REG_TX_DATA_PORT    0xFFFFFFFFU
#define I3C_MST_MEM_REG_TX_DATA_PORT_M  (I3C_MST_MEM_REG_TX_DATA_PORT_V << I3C_MST_MEM_REG_TX_DATA_PORT_S)
#define I3C_MST_MEM_REG_TX_DATA_PORT_V  0xFFFFFFFFU
#define I3C_MST_MEM_REG_TX_DATA_PORT_S  0

/** I3C_MST_MEM_IBI_STATUS_BUF_REG register
 *  In-Band Interrupt Buffer Status/Data Register. When receiving an IBI, IBI_PORT is
 *  used to both: Read the IBI Status Read the IBI Data(which is raw/opaque data)
 */
#define I3C_MST_MEM_IBI_STATUS_BUF_REG (DR_REG_I3C_MST_MEM_BASE + 0x18)
/** I3C_MST_MEM_DATA_LENGTH : RO; bitpos: [7:0]; default: 0;
 *  This field represents the length of data received along with IBI, in bytes.
 */
#define I3C_MST_MEM_DATA_LENGTH    0x000000FFU
#define I3C_MST_MEM_DATA_LENGTH_M  (I3C_MST_MEM_DATA_LENGTH_V << I3C_MST_MEM_DATA_LENGTH_S)
#define I3C_MST_MEM_DATA_LENGTH_V  0x000000FFU
#define I3C_MST_MEM_DATA_LENGTH_S  0
/** I3C_MST_MEM_IBI_ID : RO; bitpos: [15:8]; default: 0;
 *  IBI Identifier. The byte received after START which includes the address the R/W
 *  bit: Device  address and R/W bit in case of Slave Interrupt or Master Request.
 */
#define I3C_MST_MEM_IBI_ID    0x000000FFU
#define I3C_MST_MEM_IBI_ID_M  (I3C_MST_MEM_IBI_ID_V << I3C_MST_MEM_IBI_ID_S)
#define I3C_MST_MEM_IBI_ID_V  0x000000FFU
#define I3C_MST_MEM_IBI_ID_S  8
/** I3C_MST_MEM_IBI_STS : RO; bitpos: [28]; default: 0;
 *  IBI received data/status. IBI Data register is mapped to the IBI Buffer. The IBI
 *  Data is always packed in4-byte aligned and put to the IBI Buffer. This register
 *  When read from, reads the data from the IBI buffer. IBI Status register when read
 *  from, returns the data from the IBI Buffer and indicates how the controller
 *  responded to incoming IBI(SIR, MR and HJ).
 */
#define I3C_MST_MEM_IBI_STS    (BIT(28))
#define I3C_MST_MEM_IBI_STS_M  (I3C_MST_MEM_IBI_STS_V << I3C_MST_MEM_IBI_STS_S)
#define I3C_MST_MEM_IBI_STS_V  0x00000001U
#define I3C_MST_MEM_IBI_STS_S  28

/** I3C_MST_MEM_IBI_DATA_BUF_REG register
 *  NA
 */
#define I3C_MST_MEM_IBI_DATA_BUF_REG (DR_REG_I3C_MST_MEM_BASE + 0x40)
/** I3C_MST_MEM_IBI_DATA : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_IBI_DATA    0xFFFFFFFFU
#define I3C_MST_MEM_IBI_DATA_M  (I3C_MST_MEM_IBI_DATA_V << I3C_MST_MEM_IBI_DATA_S)
#define I3C_MST_MEM_IBI_DATA_V  0xFFFFFFFFU
#define I3C_MST_MEM_IBI_DATA_S  0

/** I3C_MST_MEM_DEV_ADDR_TABLE1_LOC_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_ADDR_TABLE1_LOC_REG (DR_REG_I3C_MST_MEM_BASE + 0xc0)
/** I3C_MST_MEM_REG_DAT_DEV1_STATIC_ADDR : R/W; bitpos: [6:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_REG_DAT_DEV1_STATIC_ADDR    0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV1_STATIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV1_STATIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV1_STATIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV1_STATIC_ADDR_V  0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV1_STATIC_ADDR_S  0
/** I3C_MST_MEM_REG_DAT_DEV1_DYNAMIC_ADDR : R/W; bitpos: [23:16]; default: 0;
 *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
 *  parity of dynamic address.
 */
#define I3C_MST_MEM_REG_DAT_DEV1_DYNAMIC_ADDR    0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV1_DYNAMIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV1_DYNAMIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV1_DYNAMIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV1_DYNAMIC_ADDR_V  0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV1_DYNAMIC_ADDR_S  16
/** I3C_MST_MEM_REG_DAT_DEV1_NACK_RETRY_CNT : R/W; bitpos: [30:29]; default: 0;
 *  This field is used to set the Device NACK Retry count for the particular device. If
 *  the Device NACK's for the device address, the controller automatically retries the
 *  same device until this count expires. If the Slave does not ACK for the mentioned
 *  number of retries, then controller generates an error response and move to the Halt
 *  state.
 */
#define I3C_MST_MEM_REG_DAT_DEV1_NACK_RETRY_CNT    0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV1_NACK_RETRY_CNT_M  (I3C_MST_MEM_REG_DAT_DEV1_NACK_RETRY_CNT_V << I3C_MST_MEM_REG_DAT_DEV1_NACK_RETRY_CNT_S)
#define I3C_MST_MEM_REG_DAT_DEV1_NACK_RETRY_CNT_V  0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV1_NACK_RETRY_CNT_S  29
/** I3C_MST_MEM_REG_DAT_DEV1_I2C : R/W; bitpos: [31]; default: 0;
 *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
 *  device.
 */
#define I3C_MST_MEM_REG_DAT_DEV1_I2C    (BIT(31))
#define I3C_MST_MEM_REG_DAT_DEV1_I2C_M  (I3C_MST_MEM_REG_DAT_DEV1_I2C_V << I3C_MST_MEM_REG_DAT_DEV1_I2C_S)
#define I3C_MST_MEM_REG_DAT_DEV1_I2C_V  0x00000001U
#define I3C_MST_MEM_REG_DAT_DEV1_I2C_S  31

/** I3C_MST_MEM_DEV_ADDR_TABLE2_LOC_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_ADDR_TABLE2_LOC_REG (DR_REG_I3C_MST_MEM_BASE + 0xc4)
/** I3C_MST_MEM_REG_DAT_DEV2_STATIC_ADDR : R/W; bitpos: [6:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_REG_DAT_DEV2_STATIC_ADDR    0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV2_STATIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV2_STATIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV2_STATIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV2_STATIC_ADDR_V  0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV2_STATIC_ADDR_S  0
/** I3C_MST_MEM_REG_DAT_DEV2_DYNAMIC_ADDR : R/W; bitpos: [23:16]; default: 0;
 *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
 *  parity of dynamic address.
 */
#define I3C_MST_MEM_REG_DAT_DEV2_DYNAMIC_ADDR    0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV2_DYNAMIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV2_DYNAMIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV2_DYNAMIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV2_DYNAMIC_ADDR_V  0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV2_DYNAMIC_ADDR_S  16
/** I3C_MST_MEM_REG_DAT_DEV2_NACK_RETRY_CNT : R/W; bitpos: [30:29]; default: 0;
 *  This field is used to set the Device NACK Retry count for the particular device. If
 *  the Device NACK's for the device address, the controller automatically retries the
 *  same device until this count expires. If the Slave does not ACK for the mentioned
 *  number of retries, then controller generates an error response and move to the Halt
 *  state.
 */
#define I3C_MST_MEM_REG_DAT_DEV2_NACK_RETRY_CNT    0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV2_NACK_RETRY_CNT_M  (I3C_MST_MEM_REG_DAT_DEV2_NACK_RETRY_CNT_V << I3C_MST_MEM_REG_DAT_DEV2_NACK_RETRY_CNT_S)
#define I3C_MST_MEM_REG_DAT_DEV2_NACK_RETRY_CNT_V  0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV2_NACK_RETRY_CNT_S  29
/** I3C_MST_MEM_REG_DAT_DEV2_I2C : R/W; bitpos: [31]; default: 0;
 *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
 *  device.
 */
#define I3C_MST_MEM_REG_DAT_DEV2_I2C    (BIT(31))
#define I3C_MST_MEM_REG_DAT_DEV2_I2C_M  (I3C_MST_MEM_REG_DAT_DEV2_I2C_V << I3C_MST_MEM_REG_DAT_DEV2_I2C_S)
#define I3C_MST_MEM_REG_DAT_DEV2_I2C_V  0x00000001U
#define I3C_MST_MEM_REG_DAT_DEV2_I2C_S  31

/** I3C_MST_MEM_DEV_ADDR_TABLE3_LOC_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_ADDR_TABLE3_LOC_REG (DR_REG_I3C_MST_MEM_BASE + 0xc8)
/** I3C_MST_MEM_REG_DAT_DEV3_STATIC_ADDR : R/W; bitpos: [6:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_REG_DAT_DEV3_STATIC_ADDR    0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV3_STATIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV3_STATIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV3_STATIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV3_STATIC_ADDR_V  0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV3_STATIC_ADDR_S  0
/** I3C_MST_MEM_REG_DAT_DEV3_DYNAMIC_ADDR : R/W; bitpos: [23:16]; default: 0;
 *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
 *  parity of dynamic address.
 */
#define I3C_MST_MEM_REG_DAT_DEV3_DYNAMIC_ADDR    0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV3_DYNAMIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV3_DYNAMIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV3_DYNAMIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV3_DYNAMIC_ADDR_V  0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV3_DYNAMIC_ADDR_S  16
/** I3C_MST_MEM_REG_DAT_DEV3_NACK_RETRY_CNT : R/W; bitpos: [30:29]; default: 0;
 *  This field is used to set the Device NACK Retry count for the particular device. If
 *  the Device NACK's for the device address, the controller automatically retries the
 *  same device until this count expires. If the Slave does not ACK for the mentioned
 *  number of retries, then controller generates an error response and move to the Halt
 *  state.
 */
#define I3C_MST_MEM_REG_DAT_DEV3_NACK_RETRY_CNT    0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV3_NACK_RETRY_CNT_M  (I3C_MST_MEM_REG_DAT_DEV3_NACK_RETRY_CNT_V << I3C_MST_MEM_REG_DAT_DEV3_NACK_RETRY_CNT_S)
#define I3C_MST_MEM_REG_DAT_DEV3_NACK_RETRY_CNT_V  0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV3_NACK_RETRY_CNT_S  29
/** I3C_MST_MEM_REG_DAT_DEV3_I2C : R/W; bitpos: [31]; default: 0;
 *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
 *  device.
 */
#define I3C_MST_MEM_REG_DAT_DEV3_I2C    (BIT(31))
#define I3C_MST_MEM_REG_DAT_DEV3_I2C_M  (I3C_MST_MEM_REG_DAT_DEV3_I2C_V << I3C_MST_MEM_REG_DAT_DEV3_I2C_S)
#define I3C_MST_MEM_REG_DAT_DEV3_I2C_V  0x00000001U
#define I3C_MST_MEM_REG_DAT_DEV3_I2C_S  31

/** I3C_MST_MEM_DEV_ADDR_TABLE4_LOC_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_ADDR_TABLE4_LOC_REG (DR_REG_I3C_MST_MEM_BASE + 0xcc)
/** I3C_MST_MEM_REG_DAT_DEV4_STATIC_ADDR : R/W; bitpos: [6:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_REG_DAT_DEV4_STATIC_ADDR    0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV4_STATIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV4_STATIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV4_STATIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV4_STATIC_ADDR_V  0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV4_STATIC_ADDR_S  0
/** I3C_MST_MEM_REG_DAT_DEV4_DYNAMIC_ADDR : R/W; bitpos: [23:16]; default: 0;
 *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
 *  parity of dynamic address.
 */
#define I3C_MST_MEM_REG_DAT_DEV4_DYNAMIC_ADDR    0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV4_DYNAMIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV4_DYNAMIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV4_DYNAMIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV4_DYNAMIC_ADDR_V  0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV4_DYNAMIC_ADDR_S  16
/** I3C_MST_MEM_REG_DAT_DEV4_NACK_RETRY_CNT : R/W; bitpos: [30:29]; default: 0;
 *  This field is used to set the Device NACK Retry count for the particular device. If
 *  the Device NACK's for the device address, the controller automatically retries the
 *  same device until this count expires. If the Slave does not ACK for the mentioned
 *  number of retries, then controller generates an error response and move to the Halt
 *  state.
 */
#define I3C_MST_MEM_REG_DAT_DEV4_NACK_RETRY_CNT    0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV4_NACK_RETRY_CNT_M  (I3C_MST_MEM_REG_DAT_DEV4_NACK_RETRY_CNT_V << I3C_MST_MEM_REG_DAT_DEV4_NACK_RETRY_CNT_S)
#define I3C_MST_MEM_REG_DAT_DEV4_NACK_RETRY_CNT_V  0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV4_NACK_RETRY_CNT_S  29
/** I3C_MST_MEM_REG_DAT_DEV4_I2C : R/W; bitpos: [31]; default: 0;
 *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
 *  device.
 */
#define I3C_MST_MEM_REG_DAT_DEV4_I2C    (BIT(31))
#define I3C_MST_MEM_REG_DAT_DEV4_I2C_M  (I3C_MST_MEM_REG_DAT_DEV4_I2C_V << I3C_MST_MEM_REG_DAT_DEV4_I2C_S)
#define I3C_MST_MEM_REG_DAT_DEV4_I2C_V  0x00000001U
#define I3C_MST_MEM_REG_DAT_DEV4_I2C_S  31

/** I3C_MST_MEM_DEV_ADDR_TABLE5_LOC_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_ADDR_TABLE5_LOC_REG (DR_REG_I3C_MST_MEM_BASE + 0xd0)
/** I3C_MST_MEM_REG_DAT_DEV5_STATIC_ADDR : R/W; bitpos: [6:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_REG_DAT_DEV5_STATIC_ADDR    0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV5_STATIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV5_STATIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV5_STATIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV5_STATIC_ADDR_V  0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV5_STATIC_ADDR_S  0
/** I3C_MST_MEM_REG_DAT_DEV5_DYNAMIC_ADDR : R/W; bitpos: [23:16]; default: 0;
 *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
 *  parity of dynamic address.
 */
#define I3C_MST_MEM_REG_DAT_DEV5_DYNAMIC_ADDR    0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV5_DYNAMIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV5_DYNAMIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV5_DYNAMIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV5_DYNAMIC_ADDR_V  0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV5_DYNAMIC_ADDR_S  16
/** I3C_MST_MEM_REG_DAT_DEV5_NACK_RETRY_CNT : R/W; bitpos: [30:29]; default: 0;
 *  This field is used to set the Device NACK Retry count for the particular device. If
 *  the Device NACK's for the device address, the controller automatically retries the
 *  same device until this count expires. If the Slave does not ACK for the mentioned
 *  number of retries, then controller generates an error response and move to the Halt
 *  state.
 */
#define I3C_MST_MEM_REG_DAT_DEV5_NACK_RETRY_CNT    0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV5_NACK_RETRY_CNT_M  (I3C_MST_MEM_REG_DAT_DEV5_NACK_RETRY_CNT_V << I3C_MST_MEM_REG_DAT_DEV5_NACK_RETRY_CNT_S)
#define I3C_MST_MEM_REG_DAT_DEV5_NACK_RETRY_CNT_V  0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV5_NACK_RETRY_CNT_S  29
/** I3C_MST_MEM_REG_DAT_DEV5_I2C : R/W; bitpos: [31]; default: 0;
 *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
 *  device.
 */
#define I3C_MST_MEM_REG_DAT_DEV5_I2C    (BIT(31))
#define I3C_MST_MEM_REG_DAT_DEV5_I2C_M  (I3C_MST_MEM_REG_DAT_DEV5_I2C_V << I3C_MST_MEM_REG_DAT_DEV5_I2C_S)
#define I3C_MST_MEM_REG_DAT_DEV5_I2C_V  0x00000001U
#define I3C_MST_MEM_REG_DAT_DEV5_I2C_S  31

/** I3C_MST_MEM_DEV_ADDR_TABLE6_LOC_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_ADDR_TABLE6_LOC_REG (DR_REG_I3C_MST_MEM_BASE + 0xd4)
/** I3C_MST_MEM_REG_DAT_DEV6_STATIC_ADDR : R/W; bitpos: [6:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_REG_DAT_DEV6_STATIC_ADDR    0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV6_STATIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV6_STATIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV6_STATIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV6_STATIC_ADDR_V  0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV6_STATIC_ADDR_S  0
/** I3C_MST_MEM_REG_DAT_DEV6_DYNAMIC_ADDR : R/W; bitpos: [23:16]; default: 0;
 *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
 *  parity of dynamic address.
 */
#define I3C_MST_MEM_REG_DAT_DEV6_DYNAMIC_ADDR    0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV6_DYNAMIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV6_DYNAMIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV6_DYNAMIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV6_DYNAMIC_ADDR_V  0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV6_DYNAMIC_ADDR_S  16
/** I3C_MST_MEM_REG_DAT_DEV6_NACK_RETRY_CNT : R/W; bitpos: [30:29]; default: 0;
 *  This field is used to set the Device NACK Retry count for the particular device. If
 *  the Device NACK's for the device address, the controller automatically retries the
 *  same device until this count expires. If the Slave does not ACK for the mentioned
 *  number of retries, then controller generates an error response and move to the Halt
 *  state.
 */
#define I3C_MST_MEM_REG_DAT_DEV6_NACK_RETRY_CNT    0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV6_NACK_RETRY_CNT_M  (I3C_MST_MEM_REG_DAT_DEV6_NACK_RETRY_CNT_V << I3C_MST_MEM_REG_DAT_DEV6_NACK_RETRY_CNT_S)
#define I3C_MST_MEM_REG_DAT_DEV6_NACK_RETRY_CNT_V  0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV6_NACK_RETRY_CNT_S  29
/** I3C_MST_MEM_REG_DAT_DEV6_I2C : R/W; bitpos: [31]; default: 0;
 *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
 *  device.
 */
#define I3C_MST_MEM_REG_DAT_DEV6_I2C    (BIT(31))
#define I3C_MST_MEM_REG_DAT_DEV6_I2C_M  (I3C_MST_MEM_REG_DAT_DEV6_I2C_V << I3C_MST_MEM_REG_DAT_DEV6_I2C_S)
#define I3C_MST_MEM_REG_DAT_DEV6_I2C_V  0x00000001U
#define I3C_MST_MEM_REG_DAT_DEV6_I2C_S  31

/** I3C_MST_MEM_DEV_ADDR_TABLE7_LOC_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_ADDR_TABLE7_LOC_REG (DR_REG_I3C_MST_MEM_BASE + 0xd8)
/** I3C_MST_MEM_REG_DAT_DEV7_STATIC_ADDR : R/W; bitpos: [6:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_REG_DAT_DEV7_STATIC_ADDR    0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV7_STATIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV7_STATIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV7_STATIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV7_STATIC_ADDR_V  0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV7_STATIC_ADDR_S  0
/** I3C_MST_MEM_REG_DAT_DEV7_DYNAMIC_ADDR : R/W; bitpos: [23:16]; default: 0;
 *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
 *  parity of dynamic address.
 */
#define I3C_MST_MEM_REG_DAT_DEV7_DYNAMIC_ADDR    0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV7_DYNAMIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV7_DYNAMIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV7_DYNAMIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV7_DYNAMIC_ADDR_V  0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV7_DYNAMIC_ADDR_S  16
/** I3C_MST_MEM_REG_DAT_DEV7_NACK_RETRY_CNT : R/W; bitpos: [30:29]; default: 0;
 *  This field is used to set the Device NACK Retry count for the particular device. If
 *  the Device NACK's for the device address, the controller automatically retries the
 *  same device until this count expires. If the Slave does not ACK for the mentioned
 *  number of retries, then controller generates an error response and move to the Halt
 *  state.
 */
#define I3C_MST_MEM_REG_DAT_DEV7_NACK_RETRY_CNT    0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV7_NACK_RETRY_CNT_M  (I3C_MST_MEM_REG_DAT_DEV7_NACK_RETRY_CNT_V << I3C_MST_MEM_REG_DAT_DEV7_NACK_RETRY_CNT_S)
#define I3C_MST_MEM_REG_DAT_DEV7_NACK_RETRY_CNT_V  0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV7_NACK_RETRY_CNT_S  29
/** I3C_MST_MEM_REG_DAT_DEV7_I2C : R/W; bitpos: [31]; default: 0;
 *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
 *  device.
 */
#define I3C_MST_MEM_REG_DAT_DEV7_I2C    (BIT(31))
#define I3C_MST_MEM_REG_DAT_DEV7_I2C_M  (I3C_MST_MEM_REG_DAT_DEV7_I2C_V << I3C_MST_MEM_REG_DAT_DEV7_I2C_S)
#define I3C_MST_MEM_REG_DAT_DEV7_I2C_V  0x00000001U
#define I3C_MST_MEM_REG_DAT_DEV7_I2C_S  31

/** I3C_MST_MEM_DEV_ADDR_TABLE8_LOC_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_ADDR_TABLE8_LOC_REG (DR_REG_I3C_MST_MEM_BASE + 0xdc)
/** I3C_MST_MEM_REG_DAT_DEV8_STATIC_ADDR : R/W; bitpos: [6:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_REG_DAT_DEV8_STATIC_ADDR    0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV8_STATIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV8_STATIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV8_STATIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV8_STATIC_ADDR_V  0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV8_STATIC_ADDR_S  0
/** I3C_MST_MEM_REG_DAT_DEV8_DYNAMIC_ADDR : R/W; bitpos: [23:16]; default: 0;
 *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
 *  parity of dynamic address.
 */
#define I3C_MST_MEM_REG_DAT_DEV8_DYNAMIC_ADDR    0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV8_DYNAMIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV8_DYNAMIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV8_DYNAMIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV8_DYNAMIC_ADDR_V  0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV8_DYNAMIC_ADDR_S  16
/** I3C_MST_MEM_REG_DAT_DEV8_NACK_RETRY_CNT : R/W; bitpos: [30:29]; default: 0;
 *  This field is used to set the Device NACK Retry count for the particular device. If
 *  the Device NACK's for the device address, the controller automatically retries the
 *  same device until this count expires. If the Slave does not ACK for the mentioned
 *  number of retries, then controller generates an error response and move to the Halt
 *  state.
 */
#define I3C_MST_MEM_REG_DAT_DEV8_NACK_RETRY_CNT    0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV8_NACK_RETRY_CNT_M  (I3C_MST_MEM_REG_DAT_DEV8_NACK_RETRY_CNT_V << I3C_MST_MEM_REG_DAT_DEV8_NACK_RETRY_CNT_S)
#define I3C_MST_MEM_REG_DAT_DEV8_NACK_RETRY_CNT_V  0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV8_NACK_RETRY_CNT_S  29
/** I3C_MST_MEM_REG_DAT_DEV8_I2C : R/W; bitpos: [31]; default: 0;
 *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
 *  device.
 */
#define I3C_MST_MEM_REG_DAT_DEV8_I2C    (BIT(31))
#define I3C_MST_MEM_REG_DAT_DEV8_I2C_M  (I3C_MST_MEM_REG_DAT_DEV8_I2C_V << I3C_MST_MEM_REG_DAT_DEV8_I2C_S)
#define I3C_MST_MEM_REG_DAT_DEV8_I2C_V  0x00000001U
#define I3C_MST_MEM_REG_DAT_DEV8_I2C_S  31

/** I3C_MST_MEM_DEV_ADDR_TABLE9_LOC_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_ADDR_TABLE9_LOC_REG (DR_REG_I3C_MST_MEM_BASE + 0xe0)
/** I3C_MST_MEM_REG_DAT_DEV9_STATIC_ADDR : R/W; bitpos: [6:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_REG_DAT_DEV9_STATIC_ADDR    0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV9_STATIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV9_STATIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV9_STATIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV9_STATIC_ADDR_V  0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV9_STATIC_ADDR_S  0
/** I3C_MST_MEM_REG_DAT_DEV9_DYNAMIC_ADDR : R/W; bitpos: [23:16]; default: 0;
 *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
 *  parity of dynamic address.
 */
#define I3C_MST_MEM_REG_DAT_DEV9_DYNAMIC_ADDR    0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV9_DYNAMIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV9_DYNAMIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV9_DYNAMIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV9_DYNAMIC_ADDR_V  0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV9_DYNAMIC_ADDR_S  16
/** I3C_MST_MEM_REG_DAT_DEV9_NACK_RETRY_CNT : R/W; bitpos: [30:29]; default: 0;
 *  This field is used to set the Device NACK Retry count for the particular device. If
 *  the Device NACK's for the device address, the controller automatically retries the
 *  same device until this count expires. If the Slave does not ACK for the mentioned
 *  number of retries, then controller generates an error response and move to the Halt
 *  state.
 */
#define I3C_MST_MEM_REG_DAT_DEV9_NACK_RETRY_CNT    0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV9_NACK_RETRY_CNT_M  (I3C_MST_MEM_REG_DAT_DEV9_NACK_RETRY_CNT_V << I3C_MST_MEM_REG_DAT_DEV9_NACK_RETRY_CNT_S)
#define I3C_MST_MEM_REG_DAT_DEV9_NACK_RETRY_CNT_V  0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV9_NACK_RETRY_CNT_S  29
/** I3C_MST_MEM_REG_DAT_DEV9_I2C : R/W; bitpos: [31]; default: 0;
 *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
 *  device.
 */
#define I3C_MST_MEM_REG_DAT_DEV9_I2C    (BIT(31))
#define I3C_MST_MEM_REG_DAT_DEV9_I2C_M  (I3C_MST_MEM_REG_DAT_DEV9_I2C_V << I3C_MST_MEM_REG_DAT_DEV9_I2C_S)
#define I3C_MST_MEM_REG_DAT_DEV9_I2C_V  0x00000001U
#define I3C_MST_MEM_REG_DAT_DEV9_I2C_S  31

/** I3C_MST_MEM_DEV_ADDR_TABLE10_LOC_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_ADDR_TABLE10_LOC_REG (DR_REG_I3C_MST_MEM_BASE + 0xe4)
/** I3C_MST_MEM_REG_DAT_DEV10_STATIC_ADDR : R/W; bitpos: [6:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_REG_DAT_DEV10_STATIC_ADDR    0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV10_STATIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV10_STATIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV10_STATIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV10_STATIC_ADDR_V  0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV10_STATIC_ADDR_S  0
/** I3C_MST_MEM_REG_DAT_DEV10_DYNAMIC_ADDR : R/W; bitpos: [23:16]; default: 0;
 *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
 *  parity of dynamic address.
 */
#define I3C_MST_MEM_REG_DAT_DEV10_DYNAMIC_ADDR    0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV10_DYNAMIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV10_DYNAMIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV10_DYNAMIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV10_DYNAMIC_ADDR_V  0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV10_DYNAMIC_ADDR_S  16
/** I3C_MST_MEM_REG_DAT_DEV10_NACK_RETRY_CNT : R/W; bitpos: [30:29]; default: 0;
 *  This field is used to set the Device NACK Retry count for the particular device. If
 *  the Device NACK's for the device address, the controller automatically retries the
 *  same device until this count expires. If the Slave does not ACK for the mentioned
 *  number of retries, then controller generates an error response and move to the Halt
 *  state.
 */
#define I3C_MST_MEM_REG_DAT_DEV10_NACK_RETRY_CNT    0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV10_NACK_RETRY_CNT_M  (I3C_MST_MEM_REG_DAT_DEV10_NACK_RETRY_CNT_V << I3C_MST_MEM_REG_DAT_DEV10_NACK_RETRY_CNT_S)
#define I3C_MST_MEM_REG_DAT_DEV10_NACK_RETRY_CNT_V  0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV10_NACK_RETRY_CNT_S  29
/** I3C_MST_MEM_REG_DAT_DEV10_I2C : R/W; bitpos: [31]; default: 0;
 *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
 *  device.
 */
#define I3C_MST_MEM_REG_DAT_DEV10_I2C    (BIT(31))
#define I3C_MST_MEM_REG_DAT_DEV10_I2C_M  (I3C_MST_MEM_REG_DAT_DEV10_I2C_V << I3C_MST_MEM_REG_DAT_DEV10_I2C_S)
#define I3C_MST_MEM_REG_DAT_DEV10_I2C_V  0x00000001U
#define I3C_MST_MEM_REG_DAT_DEV10_I2C_S  31

/** I3C_MST_MEM_DEV_ADDR_TABLE11_LOC_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_ADDR_TABLE11_LOC_REG (DR_REG_I3C_MST_MEM_BASE + 0xe8)
/** I3C_MST_MEM_REG_DAT_DEV11_STATIC_ADDR : R/W; bitpos: [6:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_REG_DAT_DEV11_STATIC_ADDR    0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV11_STATIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV11_STATIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV11_STATIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV11_STATIC_ADDR_V  0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV11_STATIC_ADDR_S  0
/** I3C_MST_MEM_REG_DAT_DEV11_DYNAMIC_ADDR : R/W; bitpos: [23:16]; default: 0;
 *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
 *  parity of dynamic address.
 */
#define I3C_MST_MEM_REG_DAT_DEV11_DYNAMIC_ADDR    0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV11_DYNAMIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV11_DYNAMIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV11_DYNAMIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV11_DYNAMIC_ADDR_V  0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV11_DYNAMIC_ADDR_S  16
/** I3C_MST_MEM_REG_DAT_DEV11_NACK_RETRY_CNT : R/W; bitpos: [30:29]; default: 0;
 *  This field is used to set the Device NACK Retry count for the particular device. If
 *  the Device NACK's for the device address, the controller automatically retries the
 *  same device until this count expires. If the Slave does not ACK for the mentioned
 *  number of retries, then controller generates an error response and move to the Halt
 *  state.
 */
#define I3C_MST_MEM_REG_DAT_DEV11_NACK_RETRY_CNT    0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV11_NACK_RETRY_CNT_M  (I3C_MST_MEM_REG_DAT_DEV11_NACK_RETRY_CNT_V << I3C_MST_MEM_REG_DAT_DEV11_NACK_RETRY_CNT_S)
#define I3C_MST_MEM_REG_DAT_DEV11_NACK_RETRY_CNT_V  0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV11_NACK_RETRY_CNT_S  29
/** I3C_MST_MEM_REG_DAT_DEV11_I2C : R/W; bitpos: [31]; default: 0;
 *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
 *  device.
 */
#define I3C_MST_MEM_REG_DAT_DEV11_I2C    (BIT(31))
#define I3C_MST_MEM_REG_DAT_DEV11_I2C_M  (I3C_MST_MEM_REG_DAT_DEV11_I2C_V << I3C_MST_MEM_REG_DAT_DEV11_I2C_S)
#define I3C_MST_MEM_REG_DAT_DEV11_I2C_V  0x00000001U
#define I3C_MST_MEM_REG_DAT_DEV11_I2C_S  31

/** I3C_MST_MEM_DEV_ADDR_TABLE12_LOC_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_ADDR_TABLE12_LOC_REG (DR_REG_I3C_MST_MEM_BASE + 0xec)
/** I3C_MST_MEM_REG_DAT_DEV12_STATIC_ADDR : R/W; bitpos: [6:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_REG_DAT_DEV12_STATIC_ADDR    0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV12_STATIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV12_STATIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV12_STATIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV12_STATIC_ADDR_V  0x0000007FU
#define I3C_MST_MEM_REG_DAT_DEV12_STATIC_ADDR_S  0
/** I3C_MST_MEM_REG_DAT_DEV12_DYNAMIC_ADDR : R/W; bitpos: [23:16]; default: 0;
 *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
 *  parity of dynamic address.
 */
#define I3C_MST_MEM_REG_DAT_DEV12_DYNAMIC_ADDR    0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV12_DYNAMIC_ADDR_M  (I3C_MST_MEM_REG_DAT_DEV12_DYNAMIC_ADDR_V << I3C_MST_MEM_REG_DAT_DEV12_DYNAMIC_ADDR_S)
#define I3C_MST_MEM_REG_DAT_DEV12_DYNAMIC_ADDR_V  0x000000FFU
#define I3C_MST_MEM_REG_DAT_DEV12_DYNAMIC_ADDR_S  16
/** I3C_MST_MEM_REG_DAT_DEV12_NACK_RETRY_CNT : R/W; bitpos: [30:29]; default: 0;
 *  This field is used to set the Device NACK Retry count for the particular device. If
 *  the Device NACK's for the device address, the controller automatically retries the
 *  same device until this count expires. If the Slave does not ACK for the mentioned
 *  number of retries, then controller generates an error response and move to the Halt
 *  state.
 */
#define I3C_MST_MEM_REG_DAT_DEV12_NACK_RETRY_CNT    0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV12_NACK_RETRY_CNT_M  (I3C_MST_MEM_REG_DAT_DEV12_NACK_RETRY_CNT_V << I3C_MST_MEM_REG_DAT_DEV12_NACK_RETRY_CNT_S)
#define I3C_MST_MEM_REG_DAT_DEV12_NACK_RETRY_CNT_V  0x00000003U
#define I3C_MST_MEM_REG_DAT_DEV12_NACK_RETRY_CNT_S  29
/** I3C_MST_MEM_REG_DAT_DEV12_I2C : R/W; bitpos: [31]; default: 0;
 *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
 *  device.
 */
#define I3C_MST_MEM_REG_DAT_DEV12_I2C    (BIT(31))
#define I3C_MST_MEM_REG_DAT_DEV12_I2C_M  (I3C_MST_MEM_REG_DAT_DEV12_I2C_V << I3C_MST_MEM_REG_DAT_DEV12_I2C_S)
#define I3C_MST_MEM_REG_DAT_DEV12_I2C_V  0x00000001U
#define I3C_MST_MEM_REG_DAT_DEV12_I2C_S  31

/** I3C_MST_MEM_DEV_CHAR_TABLE1_LOC1_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE1_LOC1_REG (DR_REG_I3C_MST_MEM_BASE + 0x100)
/** I3C_MST_MEM_DCT_DEV1_LOC1 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV1_LOC1    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV1_LOC1_M  (I3C_MST_MEM_DCT_DEV1_LOC1_V << I3C_MST_MEM_DCT_DEV1_LOC1_S)
#define I3C_MST_MEM_DCT_DEV1_LOC1_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV1_LOC1_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE1_LOC2_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE1_LOC2_REG (DR_REG_I3C_MST_MEM_BASE + 0x104)
/** I3C_MST_MEM_DCT_DEV1_LOC2 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV1_LOC2    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV1_LOC2_M  (I3C_MST_MEM_DCT_DEV1_LOC2_V << I3C_MST_MEM_DCT_DEV1_LOC2_S)
#define I3C_MST_MEM_DCT_DEV1_LOC2_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV1_LOC2_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE1_LOC3_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE1_LOC3_REG (DR_REG_I3C_MST_MEM_BASE + 0x108)
/** I3C_MST_MEM_DCT_DEV1_LOC3 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV1_LOC3    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV1_LOC3_M  (I3C_MST_MEM_DCT_DEV1_LOC3_V << I3C_MST_MEM_DCT_DEV1_LOC3_S)
#define I3C_MST_MEM_DCT_DEV1_LOC3_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV1_LOC3_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE1_LOC4_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE1_LOC4_REG (DR_REG_I3C_MST_MEM_BASE + 0x10c)
/** I3C_MST_MEM_DCT_DEV1_LOC4 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV1_LOC4    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV1_LOC4_M  (I3C_MST_MEM_DCT_DEV1_LOC4_V << I3C_MST_MEM_DCT_DEV1_LOC4_S)
#define I3C_MST_MEM_DCT_DEV1_LOC4_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV1_LOC4_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE2_LOC1_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE2_LOC1_REG (DR_REG_I3C_MST_MEM_BASE + 0x110)
/** I3C_MST_MEM_DCT_DEV2_LOC1 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV2_LOC1    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV2_LOC1_M  (I3C_MST_MEM_DCT_DEV2_LOC1_V << I3C_MST_MEM_DCT_DEV2_LOC1_S)
#define I3C_MST_MEM_DCT_DEV2_LOC1_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV2_LOC1_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE2_LOC2_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE2_LOC2_REG (DR_REG_I3C_MST_MEM_BASE + 0x114)
/** I3C_MST_MEM_DCT_DEV2_LOC2 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV2_LOC2    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV2_LOC2_M  (I3C_MST_MEM_DCT_DEV2_LOC2_V << I3C_MST_MEM_DCT_DEV2_LOC2_S)
#define I3C_MST_MEM_DCT_DEV2_LOC2_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV2_LOC2_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE2_LOC3_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE2_LOC3_REG (DR_REG_I3C_MST_MEM_BASE + 0x118)
/** I3C_MST_MEM_DCT_DEV2_LOC3 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV2_LOC3    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV2_LOC3_M  (I3C_MST_MEM_DCT_DEV2_LOC3_V << I3C_MST_MEM_DCT_DEV2_LOC3_S)
#define I3C_MST_MEM_DCT_DEV2_LOC3_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV2_LOC3_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE2_LOC4_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE2_LOC4_REG (DR_REG_I3C_MST_MEM_BASE + 0x11c)
/** I3C_MST_MEM_DCT_DEV2_LOC4 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV2_LOC4    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV2_LOC4_M  (I3C_MST_MEM_DCT_DEV2_LOC4_V << I3C_MST_MEM_DCT_DEV2_LOC4_S)
#define I3C_MST_MEM_DCT_DEV2_LOC4_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV2_LOC4_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE3_LOC1_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE3_LOC1_REG (DR_REG_I3C_MST_MEM_BASE + 0x120)
/** I3C_MST_MEM_DCT_DEV3_LOC1 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV3_LOC1    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV3_LOC1_M  (I3C_MST_MEM_DCT_DEV3_LOC1_V << I3C_MST_MEM_DCT_DEV3_LOC1_S)
#define I3C_MST_MEM_DCT_DEV3_LOC1_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV3_LOC1_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE3_LOC2_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE3_LOC2_REG (DR_REG_I3C_MST_MEM_BASE + 0x124)
/** I3C_MST_MEM_DCT_DEV3_LOC2 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV3_LOC2    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV3_LOC2_M  (I3C_MST_MEM_DCT_DEV3_LOC2_V << I3C_MST_MEM_DCT_DEV3_LOC2_S)
#define I3C_MST_MEM_DCT_DEV3_LOC2_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV3_LOC2_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE3_LOC3_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE3_LOC3_REG (DR_REG_I3C_MST_MEM_BASE + 0x128)
/** I3C_MST_MEM_DCT_DEV3_LOC3 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV3_LOC3    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV3_LOC3_M  (I3C_MST_MEM_DCT_DEV3_LOC3_V << I3C_MST_MEM_DCT_DEV3_LOC3_S)
#define I3C_MST_MEM_DCT_DEV3_LOC3_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV3_LOC3_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE3_LOC4_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE3_LOC4_REG (DR_REG_I3C_MST_MEM_BASE + 0x12c)
/** I3C_MST_MEM_DCT_DEV3_LOC4 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV3_LOC4    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV3_LOC4_M  (I3C_MST_MEM_DCT_DEV3_LOC4_V << I3C_MST_MEM_DCT_DEV3_LOC4_S)
#define I3C_MST_MEM_DCT_DEV3_LOC4_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV3_LOC4_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE4_LOC1_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE4_LOC1_REG (DR_REG_I3C_MST_MEM_BASE + 0x130)
/** I3C_MST_MEM_DCT_DEV4_LOC1 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV4_LOC1    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV4_LOC1_M  (I3C_MST_MEM_DCT_DEV4_LOC1_V << I3C_MST_MEM_DCT_DEV4_LOC1_S)
#define I3C_MST_MEM_DCT_DEV4_LOC1_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV4_LOC1_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE4_LOC2_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE4_LOC2_REG (DR_REG_I3C_MST_MEM_BASE + 0x134)
/** I3C_MST_MEM_DCT_DEV4_LOC2 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV4_LOC2    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV4_LOC2_M  (I3C_MST_MEM_DCT_DEV4_LOC2_V << I3C_MST_MEM_DCT_DEV4_LOC2_S)
#define I3C_MST_MEM_DCT_DEV4_LOC2_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV4_LOC2_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE4_LOC3_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE4_LOC3_REG (DR_REG_I3C_MST_MEM_BASE + 0x138)
/** I3C_MST_MEM_DCT_DEV4_LOC3 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV4_LOC3    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV4_LOC3_M  (I3C_MST_MEM_DCT_DEV4_LOC3_V << I3C_MST_MEM_DCT_DEV4_LOC3_S)
#define I3C_MST_MEM_DCT_DEV4_LOC3_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV4_LOC3_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE4_LOC4_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE4_LOC4_REG (DR_REG_I3C_MST_MEM_BASE + 0x13c)
/** I3C_MST_MEM_DCT_DEV4_LOC4 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV4_LOC4    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV4_LOC4_M  (I3C_MST_MEM_DCT_DEV4_LOC4_V << I3C_MST_MEM_DCT_DEV4_LOC4_S)
#define I3C_MST_MEM_DCT_DEV4_LOC4_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV4_LOC4_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE5_LOC1_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE5_LOC1_REG (DR_REG_I3C_MST_MEM_BASE + 0x140)
/** I3C_MST_MEM_DCT_DEV5_LOC1 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV5_LOC1    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV5_LOC1_M  (I3C_MST_MEM_DCT_DEV5_LOC1_V << I3C_MST_MEM_DCT_DEV5_LOC1_S)
#define I3C_MST_MEM_DCT_DEV5_LOC1_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV5_LOC1_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE5_LOC2_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE5_LOC2_REG (DR_REG_I3C_MST_MEM_BASE + 0x144)
/** I3C_MST_MEM_DCT_DEV5_LOC2 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV5_LOC2    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV5_LOC2_M  (I3C_MST_MEM_DCT_DEV5_LOC2_V << I3C_MST_MEM_DCT_DEV5_LOC2_S)
#define I3C_MST_MEM_DCT_DEV5_LOC2_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV5_LOC2_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE5_LOC3_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE5_LOC3_REG (DR_REG_I3C_MST_MEM_BASE + 0x148)
/** I3C_MST_MEM_DCT_DEV5_LOC3 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV5_LOC3    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV5_LOC3_M  (I3C_MST_MEM_DCT_DEV5_LOC3_V << I3C_MST_MEM_DCT_DEV5_LOC3_S)
#define I3C_MST_MEM_DCT_DEV5_LOC3_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV5_LOC3_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE5_LOC4_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE5_LOC4_REG (DR_REG_I3C_MST_MEM_BASE + 0x14c)
/** I3C_MST_MEM_DCT_DEV5_LOC4 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV5_LOC4    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV5_LOC4_M  (I3C_MST_MEM_DCT_DEV5_LOC4_V << I3C_MST_MEM_DCT_DEV5_LOC4_S)
#define I3C_MST_MEM_DCT_DEV5_LOC4_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV5_LOC4_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE6_LOC1_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE6_LOC1_REG (DR_REG_I3C_MST_MEM_BASE + 0x150)
/** I3C_MST_MEM_DCT_DEV6_LOC1 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV6_LOC1    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV6_LOC1_M  (I3C_MST_MEM_DCT_DEV6_LOC1_V << I3C_MST_MEM_DCT_DEV6_LOC1_S)
#define I3C_MST_MEM_DCT_DEV6_LOC1_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV6_LOC1_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE6_LOC2_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE6_LOC2_REG (DR_REG_I3C_MST_MEM_BASE + 0x154)
/** I3C_MST_MEM_DCT_DEV6_LOC2 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV6_LOC2    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV6_LOC2_M  (I3C_MST_MEM_DCT_DEV6_LOC2_V << I3C_MST_MEM_DCT_DEV6_LOC2_S)
#define I3C_MST_MEM_DCT_DEV6_LOC2_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV6_LOC2_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE6_LOC3_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE6_LOC3_REG (DR_REG_I3C_MST_MEM_BASE + 0x158)
/** I3C_MST_MEM_DCT_DEV6_LOC3 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV6_LOC3    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV6_LOC3_M  (I3C_MST_MEM_DCT_DEV6_LOC3_V << I3C_MST_MEM_DCT_DEV6_LOC3_S)
#define I3C_MST_MEM_DCT_DEV6_LOC3_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV6_LOC3_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE6_LOC4_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE6_LOC4_REG (DR_REG_I3C_MST_MEM_BASE + 0x15c)
/** I3C_MST_MEM_DCT_DEV6_LOC4 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV6_LOC4    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV6_LOC4_M  (I3C_MST_MEM_DCT_DEV6_LOC4_V << I3C_MST_MEM_DCT_DEV6_LOC4_S)
#define I3C_MST_MEM_DCT_DEV6_LOC4_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV6_LOC4_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE7_LOC1_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE7_LOC1_REG (DR_REG_I3C_MST_MEM_BASE + 0x160)
/** I3C_MST_MEM_DCT_DEV7_LOC1 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV7_LOC1    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV7_LOC1_M  (I3C_MST_MEM_DCT_DEV7_LOC1_V << I3C_MST_MEM_DCT_DEV7_LOC1_S)
#define I3C_MST_MEM_DCT_DEV7_LOC1_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV7_LOC1_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE7_LOC2_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE7_LOC2_REG (DR_REG_I3C_MST_MEM_BASE + 0x164)
/** I3C_MST_MEM_DCT_DEV7_LOC2 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV7_LOC2    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV7_LOC2_M  (I3C_MST_MEM_DCT_DEV7_LOC2_V << I3C_MST_MEM_DCT_DEV7_LOC2_S)
#define I3C_MST_MEM_DCT_DEV7_LOC2_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV7_LOC2_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE7_LOC3_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE7_LOC3_REG (DR_REG_I3C_MST_MEM_BASE + 0x168)
/** I3C_MST_MEM_DCT_DEV7_LOC3 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV7_LOC3    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV7_LOC3_M  (I3C_MST_MEM_DCT_DEV7_LOC3_V << I3C_MST_MEM_DCT_DEV7_LOC3_S)
#define I3C_MST_MEM_DCT_DEV7_LOC3_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV7_LOC3_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE7_LOC4_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE7_LOC4_REG (DR_REG_I3C_MST_MEM_BASE + 0x16c)
/** I3C_MST_MEM_DCT_DEV7_LOC4 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV7_LOC4    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV7_LOC4_M  (I3C_MST_MEM_DCT_DEV7_LOC4_V << I3C_MST_MEM_DCT_DEV7_LOC4_S)
#define I3C_MST_MEM_DCT_DEV7_LOC4_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV7_LOC4_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE8_LOC1_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE8_LOC1_REG (DR_REG_I3C_MST_MEM_BASE + 0x170)
/** I3C_MST_MEM_DCT_DEV8_LOC1 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV8_LOC1    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV8_LOC1_M  (I3C_MST_MEM_DCT_DEV8_LOC1_V << I3C_MST_MEM_DCT_DEV8_LOC1_S)
#define I3C_MST_MEM_DCT_DEV8_LOC1_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV8_LOC1_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE8_LOC2_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE8_LOC2_REG (DR_REG_I3C_MST_MEM_BASE + 0x174)
/** I3C_MST_MEM_DCT_DEV8_LOC2 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV8_LOC2    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV8_LOC2_M  (I3C_MST_MEM_DCT_DEV8_LOC2_V << I3C_MST_MEM_DCT_DEV8_LOC2_S)
#define I3C_MST_MEM_DCT_DEV8_LOC2_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV8_LOC2_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE8_LOC3_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE8_LOC3_REG (DR_REG_I3C_MST_MEM_BASE + 0x178)
/** I3C_MST_MEM_DCT_DEV8_LOC3 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV8_LOC3    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV8_LOC3_M  (I3C_MST_MEM_DCT_DEV8_LOC3_V << I3C_MST_MEM_DCT_DEV8_LOC3_S)
#define I3C_MST_MEM_DCT_DEV8_LOC3_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV8_LOC3_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE8_LOC4_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE8_LOC4_REG (DR_REG_I3C_MST_MEM_BASE + 0x17c)
/** I3C_MST_MEM_DCT_DEV8_LOC4 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV8_LOC4    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV8_LOC4_M  (I3C_MST_MEM_DCT_DEV8_LOC4_V << I3C_MST_MEM_DCT_DEV8_LOC4_S)
#define I3C_MST_MEM_DCT_DEV8_LOC4_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV8_LOC4_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE9_LOC1_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE9_LOC1_REG (DR_REG_I3C_MST_MEM_BASE + 0x180)
/** I3C_MST_MEM_DCT_DEV9_LOC1 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV9_LOC1    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV9_LOC1_M  (I3C_MST_MEM_DCT_DEV9_LOC1_V << I3C_MST_MEM_DCT_DEV9_LOC1_S)
#define I3C_MST_MEM_DCT_DEV9_LOC1_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV9_LOC1_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE9_LOC2_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE9_LOC2_REG (DR_REG_I3C_MST_MEM_BASE + 0x184)
/** I3C_MST_MEM_DCT_DEV9_LOC2 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV9_LOC2    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV9_LOC2_M  (I3C_MST_MEM_DCT_DEV9_LOC2_V << I3C_MST_MEM_DCT_DEV9_LOC2_S)
#define I3C_MST_MEM_DCT_DEV9_LOC2_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV9_LOC2_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE9_LOC3_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE9_LOC3_REG (DR_REG_I3C_MST_MEM_BASE + 0x188)
/** I3C_MST_MEM_DCT_DEV9_LOC3 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV9_LOC3    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV9_LOC3_M  (I3C_MST_MEM_DCT_DEV9_LOC3_V << I3C_MST_MEM_DCT_DEV9_LOC3_S)
#define I3C_MST_MEM_DCT_DEV9_LOC3_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV9_LOC3_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE9_LOC4_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE9_LOC4_REG (DR_REG_I3C_MST_MEM_BASE + 0x18c)
/** I3C_MST_MEM_DCT_DEV9_LOC4 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV9_LOC4    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV9_LOC4_M  (I3C_MST_MEM_DCT_DEV9_LOC4_V << I3C_MST_MEM_DCT_DEV9_LOC4_S)
#define I3C_MST_MEM_DCT_DEV9_LOC4_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV9_LOC4_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE10_LOC1_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE10_LOC1_REG (DR_REG_I3C_MST_MEM_BASE + 0x190)
/** I3C_MST_MEM_DCT_DEV10_LOC1 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV10_LOC1    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV10_LOC1_M  (I3C_MST_MEM_DCT_DEV10_LOC1_V << I3C_MST_MEM_DCT_DEV10_LOC1_S)
#define I3C_MST_MEM_DCT_DEV10_LOC1_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV10_LOC1_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE10_LOC2_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE10_LOC2_REG (DR_REG_I3C_MST_MEM_BASE + 0x194)
/** I3C_MST_MEM_DCT_DEV10_LOC2 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV10_LOC2    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV10_LOC2_M  (I3C_MST_MEM_DCT_DEV10_LOC2_V << I3C_MST_MEM_DCT_DEV10_LOC2_S)
#define I3C_MST_MEM_DCT_DEV10_LOC2_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV10_LOC2_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE10_LOC3_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE10_LOC3_REG (DR_REG_I3C_MST_MEM_BASE + 0x198)
/** I3C_MST_MEM_DCT_DEV10_LOC3 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV10_LOC3    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV10_LOC3_M  (I3C_MST_MEM_DCT_DEV10_LOC3_V << I3C_MST_MEM_DCT_DEV10_LOC3_S)
#define I3C_MST_MEM_DCT_DEV10_LOC3_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV10_LOC3_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE10_LOC4_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE10_LOC4_REG (DR_REG_I3C_MST_MEM_BASE + 0x19c)
/** I3C_MST_MEM_DCT_DEV10_LOC4 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV10_LOC4    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV10_LOC4_M  (I3C_MST_MEM_DCT_DEV10_LOC4_V << I3C_MST_MEM_DCT_DEV10_LOC4_S)
#define I3C_MST_MEM_DCT_DEV10_LOC4_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV10_LOC4_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE11_LOC1_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE11_LOC1_REG (DR_REG_I3C_MST_MEM_BASE + 0x1a0)
/** I3C_MST_MEM_DCT_DEV11_LOC1 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV11_LOC1    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV11_LOC1_M  (I3C_MST_MEM_DCT_DEV11_LOC1_V << I3C_MST_MEM_DCT_DEV11_LOC1_S)
#define I3C_MST_MEM_DCT_DEV11_LOC1_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV11_LOC1_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE11_LOC2_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE11_LOC2_REG (DR_REG_I3C_MST_MEM_BASE + 0x1a4)
/** I3C_MST_MEM_DCT_DEV11_LOC2 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV11_LOC2    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV11_LOC2_M  (I3C_MST_MEM_DCT_DEV11_LOC2_V << I3C_MST_MEM_DCT_DEV11_LOC2_S)
#define I3C_MST_MEM_DCT_DEV11_LOC2_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV11_LOC2_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE11_LOC3_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE11_LOC3_REG (DR_REG_I3C_MST_MEM_BASE + 0x1a8)
/** I3C_MST_MEM_DCT_DEV11_LOC3 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV11_LOC3    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV11_LOC3_M  (I3C_MST_MEM_DCT_DEV11_LOC3_V << I3C_MST_MEM_DCT_DEV11_LOC3_S)
#define I3C_MST_MEM_DCT_DEV11_LOC3_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV11_LOC3_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE11_LOC4_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE11_LOC4_REG (DR_REG_I3C_MST_MEM_BASE + 0x1ac)
/** I3C_MST_MEM_DCT_DEV11_LOC4 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV11_LOC4    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV11_LOC4_M  (I3C_MST_MEM_DCT_DEV11_LOC4_V << I3C_MST_MEM_DCT_DEV11_LOC4_S)
#define I3C_MST_MEM_DCT_DEV11_LOC4_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV11_LOC4_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE12_LOC1_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE12_LOC1_REG (DR_REG_I3C_MST_MEM_BASE + 0x1b0)
/** I3C_MST_MEM_DCT_DEV12_LOC1 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV12_LOC1    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV12_LOC1_M  (I3C_MST_MEM_DCT_DEV12_LOC1_V << I3C_MST_MEM_DCT_DEV12_LOC1_S)
#define I3C_MST_MEM_DCT_DEV12_LOC1_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV12_LOC1_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE12_LOC2_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE12_LOC2_REG (DR_REG_I3C_MST_MEM_BASE + 0x1b4)
/** I3C_MST_MEM_DCT_DEV12_LOC2 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV12_LOC2    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV12_LOC2_M  (I3C_MST_MEM_DCT_DEV12_LOC2_V << I3C_MST_MEM_DCT_DEV12_LOC2_S)
#define I3C_MST_MEM_DCT_DEV12_LOC2_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV12_LOC2_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE12_LOC3_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE12_LOC3_REG (DR_REG_I3C_MST_MEM_BASE + 0x1b8)
/** I3C_MST_MEM_DCT_DEV12_LOC3 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV12_LOC3    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV12_LOC3_M  (I3C_MST_MEM_DCT_DEV12_LOC3_V << I3C_MST_MEM_DCT_DEV12_LOC3_S)
#define I3C_MST_MEM_DCT_DEV12_LOC3_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV12_LOC3_S  0

/** I3C_MST_MEM_DEV_CHAR_TABLE12_LOC4_REG register
 *  NA
 */
#define I3C_MST_MEM_DEV_CHAR_TABLE12_LOC4_REG (DR_REG_I3C_MST_MEM_BASE + 0x1bc)
/** I3C_MST_MEM_DCT_DEV12_LOC4 : RO; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_MST_MEM_DCT_DEV12_LOC4    0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV12_LOC4_M  (I3C_MST_MEM_DCT_DEV12_LOC4_V << I3C_MST_MEM_DCT_DEV12_LOC4_S)
#define I3C_MST_MEM_DCT_DEV12_LOC4_V  0xFFFFFFFFU
#define I3C_MST_MEM_DCT_DEV12_LOC4_S  0

#ifdef __cplusplus
}
#endif
