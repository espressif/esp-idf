/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once


#ifdef __cplusplus
extern "C" {
#endif
#include "soc/soc.h"

#define USB_SERIAL_JTAG_EP1_REG          (DR_REG_USB_SERIAL_JTAG_BASE + 0x0)
/* USB_SERIAL_JTAG_RDWR_BYTE : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: Write and read byte data to/from UART Tx/Rx FIFO through this field. When USB_DE
VICE_SERIAL_IN_EMPTY_INT is set, then user can write data (up to 64 bytes) into
UART Tx FIFO. When USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT is set, user can check USB
_DEVICE_OUT_EP1_WR_ADDR USB_SERIAL_JTAG_OUT_EP0_RD_ADDR to know how many data is rece
ived, then read data from UART Rx FIFO..*/
#define USB_SERIAL_JTAG_RDWR_BYTE    0x000000FF
#define USB_SERIAL_JTAG_RDWR_BYTE_M  ((USB_DEVICE_RDWR_BYTE_V)<<(USB_DEVICE_RDWR_BYTE_S))
#define USB_SERIAL_JTAG_RDWR_BYTE_V  0xFF
#define USB_SERIAL_JTAG_RDWR_BYTE_S  0

#define USB_SERIAL_JTAG_EP1_CONF_REG          (DR_REG_USB_SERIAL_JTAG_BASE + 0x4)
/* USB_SERIAL_JTAG_SERIAL_OUT_EP_DATA_AVAIL : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: 1'b1: Indicate there is data in UART Rx FIFO..*/
#define USB_SERIAL_JTAG_SERIAL_OUT_EP_DATA_AVAIL    (BIT(2))
#define USB_SERIAL_JTAG_SERIAL_OUT_EP_DATA_AVAIL_M  (BIT(2))
#define USB_SERIAL_JTAG_SERIAL_OUT_EP_DATA_AVAIL_V  0x1
#define USB_SERIAL_JTAG_SERIAL_OUT_EP_DATA_AVAIL_S  2
/* USB_SERIAL_JTAG_SERIAL_IN_EP_DATA_FREE : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: 1'b1: Indicate UART Tx FIFO is not full and can write data into in. After
writing USB_SERIAL_JTAG_WR_DONE, this bit would be 0 until data in UART Tx FIFO is read by
USB Host..*/
#define USB_SERIAL_JTAG_SERIAL_IN_EP_DATA_FREE    (BIT(1))
#define USB_SERIAL_JTAG_SERIAL_IN_EP_DATA_FREE_M  (BIT(1))
#define USB_SERIAL_JTAG_SERIAL_IN_EP_DATA_FREE_V  0x1
#define USB_SERIAL_JTAG_SERIAL_IN_EP_DATA_FREE_S  1
/* USB_SERIAL_JTAG_WR_DONE : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to indicate writing byte data to UART Tx FIFO is done..*/
#define USB_SERIAL_JTAG_WR_DONE    (BIT(0))
#define USB_SERIAL_JTAG_WR_DONE_M  (BIT(0))
#define USB_SERIAL_JTAG_WR_DONE_V  0x1
#define USB_SERIAL_JTAG_WR_DONE_S  0

#define USB_SERIAL_JTAG_INT_RAW_REG          (DR_REG_USB_SERIAL_JTAG_BASE + 0x8)
/* USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT_RAW : R/WTC/SS ;bitpos:[11] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when OUT endpoint 2 received packet wi
th zero palyload..*/
#define USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT_RAW    (BIT(11))
#define USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT_RAW_M  (BIT(11))
#define USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT_RAW_V  0x1
#define USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT_RAW_S  11
/* USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT_RAW : R/WTC/SS ;bitpos:[10] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when OUT endpoint 1 received packet wi
th zero palyload..*/
#define USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT_RAW    (BIT(10))
#define USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT_RAW_M  (BIT(10))
#define USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT_RAW_V  0x1
#define USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT_RAW_S  10
/* USB_SERIAL_JTAG_USB_BUS_RESET_INT_RAW : R/WTC/SS ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when usb bus reset is detected..*/
#define USB_SERIAL_JTAG_USB_BUS_RESET_INT_RAW    (BIT(9))
#define USB_SERIAL_JTAG_USB_BUS_RESET_INT_RAW_M  (BIT(9))
#define USB_SERIAL_JTAG_USB_BUS_RESET_INT_RAW_V  0x1
#define USB_SERIAL_JTAG_USB_BUS_RESET_INT_RAW_S  9
/* USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT_RAW : R/WTC/SS ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when IN token for IN endpoint 1 is rec
eived..*/
#define USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT_RAW    (BIT(8))
#define USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT_RAW_M  (BIT(8))
#define USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT_RAW_V  0x1
#define USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT_RAW_S  8
/* USB_SERIAL_JTAG_STUFF_ERR_INT_RAW : R/WTC/SS ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when stuff error is detected..*/
#define USB_SERIAL_JTAG_STUFF_ERR_INT_RAW    (BIT(7))
#define USB_SERIAL_JTAG_STUFF_ERR_INT_RAW_M  (BIT(7))
#define USB_SERIAL_JTAG_STUFF_ERR_INT_RAW_V  0x1
#define USB_SERIAL_JTAG_STUFF_ERR_INT_RAW_S  7
/* USB_SERIAL_JTAG_CRC16_ERR_INT_RAW : R/WTC/SS ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when CRC16 error is detected..*/
#define USB_SERIAL_JTAG_CRC16_ERR_INT_RAW    (BIT(6))
#define USB_SERIAL_JTAG_CRC16_ERR_INT_RAW_M  (BIT(6))
#define USB_SERIAL_JTAG_CRC16_ERR_INT_RAW_V  0x1
#define USB_SERIAL_JTAG_CRC16_ERR_INT_RAW_S  6
/* USB_SERIAL_JTAG_CRC5_ERR_INT_RAW : R/WTC/SS ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when CRC5 error is detected..*/
#define USB_SERIAL_JTAG_CRC5_ERR_INT_RAW    (BIT(5))
#define USB_SERIAL_JTAG_CRC5_ERR_INT_RAW_M  (BIT(5))
#define USB_SERIAL_JTAG_CRC5_ERR_INT_RAW_V  0x1
#define USB_SERIAL_JTAG_CRC5_ERR_INT_RAW_S  5
/* USB_SERIAL_JTAG_PID_ERR_INT_RAW : R/WTC/SS ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when pid error is detected..*/
#define USB_SERIAL_JTAG_PID_ERR_INT_RAW    (BIT(4))
#define USB_SERIAL_JTAG_PID_ERR_INT_RAW_M  (BIT(4))
#define USB_SERIAL_JTAG_PID_ERR_INT_RAW_V  0x1
#define USB_SERIAL_JTAG_PID_ERR_INT_RAW_S  4
/* USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT_RAW : R/WTC/SS ;bitpos:[3] ;default: 1'b1 ; */
/*description: The raw interrupt bit turns to high level when Serial Port IN Endpoint is empty..*/
#define USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT_RAW    (BIT(3))
#define USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT_RAW_M  (BIT(3))
#define USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT_RAW_V  0x1
#define USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT_RAW_S  3
/* USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT_RAW : R/WTC/SS ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when Serial Port OUT Endpoint received
 one packet..*/
#define USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT_RAW    (BIT(2))
#define USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT_RAW_M  (BIT(2))
#define USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT_RAW_V  0x1
#define USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT_RAW_S  2
/* USB_SERIAL_JTAG_SOF_INT_RAW : R/WTC/SS ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when SOF frame is received..*/
#define USB_SERIAL_JTAG_SOF_INT_RAW    (BIT(1))
#define USB_SERIAL_JTAG_SOF_INT_RAW_M  (BIT(1))
#define USB_SERIAL_JTAG_SOF_INT_RAW_V  0x1
#define USB_SERIAL_JTAG_SOF_INT_RAW_S  1
/* USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT_RAW : R/WTC/SS ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when flush cmd is received for IN
endpoint 2 of JTAG..*/
#define USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT_RAW    (BIT(0))
#define USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT_RAW_M  (BIT(0))
#define USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT_RAW_V  0x1
#define USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT_RAW_S  0

#define USB_SERIAL_JTAG_INT_ST_REG          (DR_REG_USB_SERIAL_JTAG_BASE + 0xC)
/* USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT interru
pt..*/
#define USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT_ST    (BIT(11))
#define USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT_ST_M  (BIT(11))
#define USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT_ST_V  0x1
#define USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT_ST_S  11
/* USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT interru
pt..*/
#define USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT_ST    (BIT(10))
#define USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT_ST_M  (BIT(10))
#define USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT_ST_V  0x1
#define USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT_ST_S  10
/* USB_SERIAL_JTAG_USB_BUS_RESET_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the USB_SERIAL_JTAG_USB_BUS_RESET_INT interrupt..*/
#define USB_SERIAL_JTAG_USB_BUS_RESET_INT_ST    (BIT(9))
#define USB_SERIAL_JTAG_USB_BUS_RESET_INT_ST_M  (BIT(9))
#define USB_SERIAL_JTAG_USB_BUS_RESET_INT_ST_V  0x1
#define USB_SERIAL_JTAG_USB_BUS_RESET_INT_ST_S  9
/* USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT interrupt
t..*/
#define USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT_ST    (BIT(8))
#define USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT_ST_M  (BIT(8))
#define USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT_ST_V  0x1
#define USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT_ST_S  8
/* USB_SERIAL_JTAG_STUFF_ERR_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the USB_SERIAL_JTAG_STUFF_ERR_INT interrupt..*/
#define USB_SERIAL_JTAG_STUFF_ERR_INT_ST    (BIT(7))
#define USB_SERIAL_JTAG_STUFF_ERR_INT_ST_M  (BIT(7))
#define USB_SERIAL_JTAG_STUFF_ERR_INT_ST_V  0x1
#define USB_SERIAL_JTAG_STUFF_ERR_INT_ST_S  7
/* USB_SERIAL_JTAG_CRC16_ERR_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the USB_SERIAL_JTAG_CRC16_ERR_INT interrupt..*/
#define USB_SERIAL_JTAG_CRC16_ERR_INT_ST    (BIT(6))
#define USB_SERIAL_JTAG_CRC16_ERR_INT_ST_M  (BIT(6))
#define USB_SERIAL_JTAG_CRC16_ERR_INT_ST_V  0x1
#define USB_SERIAL_JTAG_CRC16_ERR_INT_ST_S  6
/* USB_SERIAL_JTAG_CRC5_ERR_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the USB_SERIAL_JTAG_CRC5_ERR_INT interrupt..*/
#define USB_SERIAL_JTAG_CRC5_ERR_INT_ST    (BIT(5))
#define USB_SERIAL_JTAG_CRC5_ERR_INT_ST_M  (BIT(5))
#define USB_SERIAL_JTAG_CRC5_ERR_INT_ST_V  0x1
#define USB_SERIAL_JTAG_CRC5_ERR_INT_ST_S  5
/* USB_SERIAL_JTAG_PID_ERR_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the USB_SERIAL_JTAG_PID_ERR_INT interrupt..*/
#define USB_SERIAL_JTAG_PID_ERR_INT_ST    (BIT(4))
#define USB_SERIAL_JTAG_PID_ERR_INT_ST_M  (BIT(4))
#define USB_SERIAL_JTAG_PID_ERR_INT_ST_V  0x1
#define USB_SERIAL_JTAG_PID_ERR_INT_ST_S  4
/* USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT interrupt..*/
#define USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT_ST    (BIT(3))
#define USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT_ST_M  (BIT(3))
#define USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT_ST_V  0x1
#define USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT_ST_S  3
/* USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT interrupt
t..*/
#define USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT_ST    (BIT(2))
#define USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT_ST_M  (BIT(2))
#define USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT_ST_V  0x1
#define USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT_ST_S  2
/* USB_SERIAL_JTAG_SOF_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the USB_SERIAL_JTAG_SOF_INT interrupt..*/
#define USB_SERIAL_JTAG_SOF_INT_ST    (BIT(1))
#define USB_SERIAL_JTAG_SOF_INT_ST_M  (BIT(1))
#define USB_SERIAL_JTAG_SOF_INT_ST_V  0x1
#define USB_SERIAL_JTAG_SOF_INT_ST_S  1
/* USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT interrupt..*/
#define USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT_ST    (BIT(0))
#define USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT_ST_M  (BIT(0))
#define USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT_ST_V  0x1
#define USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT_ST_S  0

#define USB_SERIAL_JTAG_INT_ENA_REG          (DR_REG_USB_SERIAL_JTAG_BASE + 0x10)
/* USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT interrupt..*/
#define USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT_ENA    (BIT(11))
#define USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT_ENA_M  (BIT(11))
#define USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT_ENA_V  0x1
#define USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT_ENA_S  11
/* USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT interrupt..*/
#define USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT_ENA    (BIT(10))
#define USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT_ENA_M  (BIT(10))
#define USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT_ENA_V  0x1
#define USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT_ENA_S  10
/* USB_SERIAL_JTAG_USB_BUS_RESET_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the USB_SERIAL_JTAG_USB_BUS_RESET_INT interrupt..*/
#define USB_SERIAL_JTAG_USB_BUS_RESET_INT_ENA    (BIT(9))
#define USB_SERIAL_JTAG_USB_BUS_RESET_INT_ENA_M  (BIT(9))
#define USB_SERIAL_JTAG_USB_BUS_RESET_INT_ENA_V  0x1
#define USB_SERIAL_JTAG_USB_BUS_RESET_INT_ENA_S  9
/* USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT interrupt..*/
#define USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT_ENA    (BIT(8))
#define USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT_ENA_M  (BIT(8))
#define USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT_ENA_V  0x1
#define USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT_ENA_S  8
/* USB_SERIAL_JTAG_STUFF_ERR_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the USB_SERIAL_JTAG_STUFF_ERR_INT interrupt..*/
#define USB_SERIAL_JTAG_STUFF_ERR_INT_ENA    (BIT(7))
#define USB_SERIAL_JTAG_STUFF_ERR_INT_ENA_M  (BIT(7))
#define USB_SERIAL_JTAG_STUFF_ERR_INT_ENA_V  0x1
#define USB_SERIAL_JTAG_STUFF_ERR_INT_ENA_S  7
/* USB_SERIAL_JTAG_CRC16_ERR_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the USB_SERIAL_JTAG_CRC16_ERR_INT interrupt..*/
#define USB_SERIAL_JTAG_CRC16_ERR_INT_ENA    (BIT(6))
#define USB_SERIAL_JTAG_CRC16_ERR_INT_ENA_M  (BIT(6))
#define USB_SERIAL_JTAG_CRC16_ERR_INT_ENA_V  0x1
#define USB_SERIAL_JTAG_CRC16_ERR_INT_ENA_S  6
/* USB_SERIAL_JTAG_CRC5_ERR_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the USB_SERIAL_JTAG_CRC5_ERR_INT interrupt..*/
#define USB_SERIAL_JTAG_CRC5_ERR_INT_ENA    (BIT(5))
#define USB_SERIAL_JTAG_CRC5_ERR_INT_ENA_M  (BIT(5))
#define USB_SERIAL_JTAG_CRC5_ERR_INT_ENA_V  0x1
#define USB_SERIAL_JTAG_CRC5_ERR_INT_ENA_S  5
/* USB_SERIAL_JTAG_PID_ERR_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the USB_SERIAL_JTAG_PID_ERR_INT interrupt..*/
#define USB_SERIAL_JTAG_PID_ERR_INT_ENA    (BIT(4))
#define USB_SERIAL_JTAG_PID_ERR_INT_ENA_M  (BIT(4))
#define USB_SERIAL_JTAG_PID_ERR_INT_ENA_V  0x1
#define USB_SERIAL_JTAG_PID_ERR_INT_ENA_S  4
/* USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT interrupt..*/
#define USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT_ENA    (BIT(3))
#define USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT_ENA_M  (BIT(3))
#define USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT_ENA_V  0x1
#define USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT_ENA_S  3
/* USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT interrupt..*/
#define USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT_ENA    (BIT(2))
#define USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT_ENA_M  (BIT(2))
#define USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT_ENA_V  0x1
#define USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT_ENA_S  2
/* USB_SERIAL_JTAG_SOF_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the USB_SERIAL_JTAG_SOF_INT interrupt..*/
#define USB_SERIAL_JTAG_SOF_INT_ENA    (BIT(1))
#define USB_SERIAL_JTAG_SOF_INT_ENA_M  (BIT(1))
#define USB_SERIAL_JTAG_SOF_INT_ENA_V  0x1
#define USB_SERIAL_JTAG_SOF_INT_ENA_S  1
/* USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT interrupt..*/
#define USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT_ENA    (BIT(0))
#define USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT_ENA_M  (BIT(0))
#define USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT_ENA_V  0x1
#define USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT_ENA_S  0

#define USB_SERIAL_JTAG_INT_CLR_REG          (DR_REG_USB_SERIAL_JTAG_BASE + 0x14)
/* USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT_CLR : WT ;bitpos:[11] ;default: 1'b0 ; */
/*description: Set this bit to clear the USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT interrupt..*/
#define USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT_CLR    (BIT(11))
#define USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT_CLR_M  (BIT(11))
#define USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT_CLR_V  0x1
#define USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT_CLR_S  11
/* USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT_CLR : WT ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this bit to clear the USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT interrupt..*/
#define USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT_CLR    (BIT(10))
#define USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT_CLR_M  (BIT(10))
#define USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT_CLR_V  0x1
#define USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT_CLR_S  10
/* USB_SERIAL_JTAG_USB_BUS_RESET_INT_CLR : WT ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to clear the USB_SERIAL_JTAG_USB_BUS_RESET_INT interrupt..*/
#define USB_SERIAL_JTAG_USB_BUS_RESET_INT_CLR    (BIT(9))
#define USB_SERIAL_JTAG_USB_BUS_RESET_INT_CLR_M  (BIT(9))
#define USB_SERIAL_JTAG_USB_BUS_RESET_INT_CLR_V  0x1
#define USB_SERIAL_JTAG_USB_BUS_RESET_INT_CLR_S  9
/* USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT_CLR : WT ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to clear the USB_SERIAL_JTAG_IN_TOKEN_IN_EP1_INT interrupt..*/
#define USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT_CLR    (BIT(8))
#define USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT_CLR_M  (BIT(8))
#define USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT_CLR_V  0x1
#define USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT_CLR_S  8
/* USB_SERIAL_JTAG_STUFF_ERR_INT_CLR : WT ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to clear the USB_SERIAL_JTAG_STUFF_ERR_INT interrupt..*/
#define USB_SERIAL_JTAG_STUFF_ERR_INT_CLR    (BIT(7))
#define USB_SERIAL_JTAG_STUFF_ERR_INT_CLR_M  (BIT(7))
#define USB_SERIAL_JTAG_STUFF_ERR_INT_CLR_V  0x1
#define USB_SERIAL_JTAG_STUFF_ERR_INT_CLR_S  7
/* USB_SERIAL_JTAG_CRC16_ERR_INT_CLR : WT ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to clear the USB_SERIAL_JTAG_CRC16_ERR_INT interrupt..*/
#define USB_SERIAL_JTAG_CRC16_ERR_INT_CLR    (BIT(6))
#define USB_SERIAL_JTAG_CRC16_ERR_INT_CLR_M  (BIT(6))
#define USB_SERIAL_JTAG_CRC16_ERR_INT_CLR_V  0x1
#define USB_SERIAL_JTAG_CRC16_ERR_INT_CLR_S  6
/* USB_SERIAL_JTAG_CRC5_ERR_INT_CLR : WT ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to clear the USB_SERIAL_JTAG_CRC5_ERR_INT interrupt..*/
#define USB_SERIAL_JTAG_CRC5_ERR_INT_CLR    (BIT(5))
#define USB_SERIAL_JTAG_CRC5_ERR_INT_CLR_M  (BIT(5))
#define USB_SERIAL_JTAG_CRC5_ERR_INT_CLR_V  0x1
#define USB_SERIAL_JTAG_CRC5_ERR_INT_CLR_S  5
/* USB_SERIAL_JTAG_PID_ERR_INT_CLR : WT ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to clear the USB_SERIAL_JTAG_PID_ERR_INT interrupt..*/
#define USB_SERIAL_JTAG_PID_ERR_INT_CLR    (BIT(4))
#define USB_SERIAL_JTAG_PID_ERR_INT_CLR_M  (BIT(4))
#define USB_SERIAL_JTAG_PID_ERR_INT_CLR_V  0x1
#define USB_SERIAL_JTAG_PID_ERR_INT_CLR_S  4
/* USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT_CLR : WT ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to clear the USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT interrupt..*/
#define USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT_CLR    (BIT(3))
#define USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT_CLR_M  (BIT(3))
#define USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT_CLR_V  0x1
#define USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT_CLR_S  3
/* USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT_CLR : WT ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to clear the USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT interrupt..*/
#define USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT_CLR    (BIT(2))
#define USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT_CLR_M  (BIT(2))
#define USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT_CLR_V  0x1
#define USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT_CLR_S  2
/* USB_SERIAL_JTAG_SOF_INT_CLR : WT ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to clear the USB_SERIAL_JTAG_JTAG_SOF_INT interrupt..*/
#define USB_SERIAL_JTAG_SOF_INT_CLR    (BIT(1))
#define USB_SERIAL_JTAG_SOF_INT_CLR_M  (BIT(1))
#define USB_SERIAL_JTAG_SOF_INT_CLR_V  0x1
#define USB_SERIAL_JTAG_SOF_INT_CLR_S  1
/* USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT_CLR : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT interrupt..*/
#define USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT_CLR    (BIT(0))
#define USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT_CLR_M  (BIT(0))
#define USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT_CLR_V  0x1
#define USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT_CLR_S  0

#define USB_SERIAL_JTAG_CONF0_REG          (DR_REG_USB_SERIAL_JTAG_BASE + 0x18)
/* USB_SERIAL_JTAG_USB_JTAG_BRIDGE_EN : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: Set this bit usb_jtag, the connection between usb_jtag and internal JTAG is disc
onnected, and MTMS, MTDI, MTCK are output through GPIO Matrix, MTDO is input thr
ough GPIO Matrix..*/
#define USB_SERIAL_JTAG_USB_JTAG_BRIDGE_EN    (BIT(16))
#define USB_SERIAL_JTAG_USB_JTAG_BRIDGE_EN_M  (BIT(16))
#define USB_SERIAL_JTAG_USB_JTAG_BRIDGE_EN_V  0x1
#define USB_SERIAL_JTAG_USB_JTAG_BRIDGE_EN_S  16
/* USB_SERIAL_JTAG_PHY_TX_EDGE_SEL : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: 0: TX output at clock negedge. 1: Tx output at clock posedge..*/
#define USB_SERIAL_JTAG_PHY_TX_EDGE_SEL    (BIT(15))
#define USB_SERIAL_JTAG_PHY_TX_EDGE_SEL_M  (BIT(15))
#define USB_SERIAL_JTAG_PHY_TX_EDGE_SEL_V  0x1
#define USB_SERIAL_JTAG_PHY_TX_EDGE_SEL_S  15
/* USB_SERIAL_JTAG_USB_PAD_ENABLE : R/W ;bitpos:[14] ;default: 1'b1 ; */
/*description: Enable USB pad function..*/
#define USB_SERIAL_JTAG_USB_PAD_ENABLE    (BIT(14))
#define USB_SERIAL_JTAG_USB_PAD_ENABLE_M  (BIT(14))
#define USB_SERIAL_JTAG_USB_PAD_ENABLE_V  0x1
#define USB_SERIAL_JTAG_USB_PAD_ENABLE_S  14
/* USB_SERIAL_JTAG_PULLUP_VALUE : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: Control pull up value..*/
#define USB_SERIAL_JTAG_PULLUP_VALUE    (BIT(13))
#define USB_SERIAL_JTAG_PULLUP_VALUE_M  (BIT(13))
#define USB_SERIAL_JTAG_PULLUP_VALUE_V  0x1
#define USB_SERIAL_JTAG_PULLUP_VALUE_S  13
/* USB_SERIAL_JTAG_DM_PULLDOWN : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Control USB D- pull down..*/
#define USB_SERIAL_JTAG_DM_PULLDOWN    (BIT(12))
#define USB_SERIAL_JTAG_DM_PULLDOWN_M  (BIT(12))
#define USB_SERIAL_JTAG_DM_PULLDOWN_V  0x1
#define USB_SERIAL_JTAG_DM_PULLDOWN_S  12
/* USB_SERIAL_JTAG_DM_PULLUP : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: Control USB D- pull up..*/
#define USB_SERIAL_JTAG_DM_PULLUP    (BIT(11))
#define USB_SERIAL_JTAG_DM_PULLUP_M  (BIT(11))
#define USB_SERIAL_JTAG_DM_PULLUP_V  0x1
#define USB_SERIAL_JTAG_DM_PULLUP_S  11
/* USB_SERIAL_JTAG_DP_PULLDOWN : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: Control USB D+ pull down..*/
#define USB_SERIAL_JTAG_DP_PULLDOWN    (BIT(10))
#define USB_SERIAL_JTAG_DP_PULLDOWN_M  (BIT(10))
#define USB_SERIAL_JTAG_DP_PULLDOWN_V  0x1
#define USB_SERIAL_JTAG_DP_PULLDOWN_S  10
/* USB_SERIAL_JTAG_DP_PULLUP : R/W ;bitpos:[9] ;default: 1'b1 ; */
/*description: Control USB D+ pull up..*/
#define USB_SERIAL_JTAG_DP_PULLUP    (BIT(9))
#define USB_SERIAL_JTAG_DP_PULLUP_M  (BIT(9))
#define USB_SERIAL_JTAG_DP_PULLUP_V  0x1
#define USB_SERIAL_JTAG_DP_PULLUP_S  9
/* USB_SERIAL_JTAG_PAD_PULL_OVERRIDE : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Enable software control USB D+ D- pullup pulldown.*/
#define USB_SERIAL_JTAG_PAD_PULL_OVERRIDE    (BIT(8))
#define USB_SERIAL_JTAG_PAD_PULL_OVERRIDE_M  (BIT(8))
#define USB_SERIAL_JTAG_PAD_PULL_OVERRIDE_V  0x1
#define USB_SERIAL_JTAG_PAD_PULL_OVERRIDE_S  8
/* USB_SERIAL_JTAG_VREF_OVERRIDE : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: Enable software control input  threshold.*/
#define USB_SERIAL_JTAG_VREF_OVERRIDE    (BIT(7))
#define USB_SERIAL_JTAG_VREF_OVERRIDE_M  (BIT(7))
#define USB_SERIAL_JTAG_VREF_OVERRIDE_V  0x1
#define USB_SERIAL_JTAG_VREF_OVERRIDE_S  7
/* USB_SERIAL_JTAG_VREFL : R/W ;bitpos:[6:5] ;default: 2'b0 ; */
/*description: Control single-end input low threshold,0.8V to 1.04V, step 80mV.*/
#define USB_SERIAL_JTAG_VREFL    0x00000003
#define USB_SERIAL_JTAG_VREFL_M  ((USB_DEVICE_VREFL_V)<<(USB_DEVICE_VREFL_S))
#define USB_SERIAL_JTAG_VREFL_V  0x3
#define USB_SERIAL_JTAG_VREFL_S  5
/* USB_SERIAL_JTAG_VREFH : R/W ;bitpos:[4:3] ;default: 2'b0 ; */
/*description: Control single-end input high threshold,1.76V to 2V, step 80mV.*/
#define USB_SERIAL_JTAG_VREFH    0x00000003
#define USB_SERIAL_JTAG_VREFH_M  ((USB_DEVICE_VREFH_V)<<(USB_DEVICE_VREFH_S))
#define USB_SERIAL_JTAG_VREFH_V  0x3
#define USB_SERIAL_JTAG_VREFH_S  3
/* USB_SERIAL_JTAG_EXCHG_PINS : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: USB D+ D- exchange.*/
#define USB_SERIAL_JTAG_EXCHG_PINS    (BIT(2))
#define USB_SERIAL_JTAG_EXCHG_PINS_M  (BIT(2))
#define USB_SERIAL_JTAG_EXCHG_PINS_V  0x1
#define USB_SERIAL_JTAG_EXCHG_PINS_S  2
/* USB_SERIAL_JTAG_EXCHG_PINS_OVERRIDE : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: Enable software control USB D+ D- exchange.*/
#define USB_SERIAL_JTAG_EXCHG_PINS_OVERRIDE    (BIT(1))
#define USB_SERIAL_JTAG_EXCHG_PINS_OVERRIDE_M  (BIT(1))
#define USB_SERIAL_JTAG_EXCHG_PINS_OVERRIDE_V  0x1
#define USB_SERIAL_JTAG_EXCHG_PINS_OVERRIDE_S  1
/* USB_SERIAL_JTAG_PHY_SEL : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Select internal/external PHY.*/
#define USB_SERIAL_JTAG_PHY_SEL    (BIT(0))
#define USB_SERIAL_JTAG_PHY_SEL_M  (BIT(0))
#define USB_SERIAL_JTAG_PHY_SEL_V  0x1
#define USB_SERIAL_JTAG_PHY_SEL_S  0

#define USB_SERIAL_JTAG_TEST_REG          (DR_REG_USB_SERIAL_JTAG_BASE + 0x1C)
/* USB_SERIAL_JTAG_TEST_RX_DM : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: USB D- rx value in test.*/
#define USB_SERIAL_JTAG_TEST_RX_DM    (BIT(6))
#define USB_SERIAL_JTAG_TEST_RX_DM_M  (BIT(6))
#define USB_SERIAL_JTAG_TEST_RX_DM_V  0x1
#define USB_SERIAL_JTAG_TEST_RX_DM_S  6
/* USB_SERIAL_JTAG_TEST_RX_DP : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: USB D+ rx value in test.*/
#define USB_SERIAL_JTAG_TEST_RX_DP    (BIT(5))
#define USB_SERIAL_JTAG_TEST_RX_DP_M  (BIT(5))
#define USB_SERIAL_JTAG_TEST_RX_DP_V  0x1
#define USB_SERIAL_JTAG_TEST_RX_DP_S  5
/* USB_SERIAL_JTAG_TEST_RX_RCV : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: USB differential rx value in test.*/
#define USB_SERIAL_JTAG_TEST_RX_RCV    (BIT(4))
#define USB_SERIAL_JTAG_TEST_RX_RCV_M  (BIT(4))
#define USB_SERIAL_JTAG_TEST_RX_RCV_V  0x1
#define USB_SERIAL_JTAG_TEST_RX_RCV_S  4
/* USB_SERIAL_JTAG_TEST_TX_DM : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: USB D- tx value in test.*/
#define USB_SERIAL_JTAG_TEST_TX_DM    (BIT(3))
#define USB_SERIAL_JTAG_TEST_TX_DM_M  (BIT(3))
#define USB_SERIAL_JTAG_TEST_TX_DM_V  0x1
#define USB_SERIAL_JTAG_TEST_TX_DM_S  3
/* USB_SERIAL_JTAG_TEST_TX_DP : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: USB D+ tx value in test.*/
#define USB_SERIAL_JTAG_TEST_TX_DP    (BIT(2))
#define USB_SERIAL_JTAG_TEST_TX_DP_M  (BIT(2))
#define USB_SERIAL_JTAG_TEST_TX_DP_V  0x1
#define USB_SERIAL_JTAG_TEST_TX_DP_S  2
/* USB_SERIAL_JTAG_TEST_USB_OE : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: USB pad oen in test.*/
#define USB_SERIAL_JTAG_TEST_USB_OE    (BIT(1))
#define USB_SERIAL_JTAG_TEST_USB_OE_M  (BIT(1))
#define USB_SERIAL_JTAG_TEST_USB_OE_V  0x1
#define USB_SERIAL_JTAG_TEST_USB_OE_S  1
/* USB_SERIAL_JTAG_TEST_ENABLE : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Enable test of the USB pad.*/
#define USB_SERIAL_JTAG_TEST_ENABLE    (BIT(0))
#define USB_SERIAL_JTAG_TEST_ENABLE_M  (BIT(0))
#define USB_SERIAL_JTAG_TEST_ENABLE_V  0x1
#define USB_SERIAL_JTAG_TEST_ENABLE_S  0

#define USB_SERIAL_JTAG_JFIFO_ST_REG          (DR_REG_USB_SERIAL_JTAG_BASE + 0x20)
/* USB_SERIAL_JTAG_OUT_FIFO_RESET : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: Write 1 to reset JTAG out fifo..*/
#define USB_SERIAL_JTAG_OUT_FIFO_RESET    (BIT(9))
#define USB_SERIAL_JTAG_OUT_FIFO_RESET_M  (BIT(9))
#define USB_SERIAL_JTAG_OUT_FIFO_RESET_V  0x1
#define USB_SERIAL_JTAG_OUT_FIFO_RESET_S  9
/* USB_SERIAL_JTAG_IN_FIFO_RESET : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Write 1 to reset JTAG in fifo..*/
#define USB_SERIAL_JTAG_IN_FIFO_RESET    (BIT(8))
#define USB_SERIAL_JTAG_IN_FIFO_RESET_M  (BIT(8))
#define USB_SERIAL_JTAG_IN_FIFO_RESET_V  0x1
#define USB_SERIAL_JTAG_IN_FIFO_RESET_S  8
/* USB_SERIAL_JTAG_OUT_FIFO_FULL : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: 1: JTAG out fifo is full..*/
#define USB_SERIAL_JTAG_OUT_FIFO_FULL    (BIT(7))
#define USB_SERIAL_JTAG_OUT_FIFO_FULL_M  (BIT(7))
#define USB_SERIAL_JTAG_OUT_FIFO_FULL_V  0x1
#define USB_SERIAL_JTAG_OUT_FIFO_FULL_S  7
/* USB_SERIAL_JTAG_OUT_FIFO_EMPTY : RO ;bitpos:[6] ;default: 1'b1 ; */
/*description: 1: JTAG out fifo is empty..*/
#define USB_SERIAL_JTAG_OUT_FIFO_EMPTY    (BIT(6))
#define USB_SERIAL_JTAG_OUT_FIFO_EMPTY_M  (BIT(6))
#define USB_SERIAL_JTAG_OUT_FIFO_EMPTY_V  0x1
#define USB_SERIAL_JTAG_OUT_FIFO_EMPTY_S  6
/* USB_SERIAL_JTAG_OUT_FIFO_CNT : RO ;bitpos:[5:4] ;default: 2'd0 ; */
/*description: JTAT out fifo counter..*/
#define USB_SERIAL_JTAG_OUT_FIFO_CNT    0x00000003
#define USB_SERIAL_JTAG_OUT_FIFO_CNT_M  ((USB_DEVICE_OUT_FIFO_CNT_V)<<(USB_DEVICE_OUT_FIFO_CNT_S))
#define USB_SERIAL_JTAG_OUT_FIFO_CNT_V  0x3
#define USB_SERIAL_JTAG_OUT_FIFO_CNT_S  4
/* USB_SERIAL_JTAG_IN_FIFO_FULL : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: 1: JTAG in fifo is full..*/
#define USB_SERIAL_JTAG_IN_FIFO_FULL    (BIT(3))
#define USB_SERIAL_JTAG_IN_FIFO_FULL_M  (BIT(3))
#define USB_SERIAL_JTAG_IN_FIFO_FULL_V  0x1
#define USB_SERIAL_JTAG_IN_FIFO_FULL_S  3
/* USB_SERIAL_JTAG_IN_FIFO_EMPTY : RO ;bitpos:[2] ;default: 1'b1 ; */
/*description: 1: JTAG in fifo is empty..*/
#define USB_SERIAL_JTAG_IN_FIFO_EMPTY    (BIT(2))
#define USB_SERIAL_JTAG_IN_FIFO_EMPTY_M  (BIT(2))
#define USB_SERIAL_JTAG_IN_FIFO_EMPTY_V  0x1
#define USB_SERIAL_JTAG_IN_FIFO_EMPTY_S  2
/* USB_SERIAL_JTAG_IN_FIFO_CNT : RO ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: JTAT in fifo counter..*/
#define USB_SERIAL_JTAG_IN_FIFO_CNT    0x00000003
#define USB_SERIAL_JTAG_IN_FIFO_CNT_M  ((USB_DEVICE_IN_FIFO_CNT_V)<<(USB_DEVICE_IN_FIFO_CNT_S))
#define USB_SERIAL_JTAG_IN_FIFO_CNT_V  0x3
#define USB_SERIAL_JTAG_IN_FIFO_CNT_S  0

#define USB_SERIAL_JTAG_FRAM_NUM_REG          (DR_REG_USB_SERIAL_JTAG_BASE + 0x24)
/* USB_SERIAL_JTAG_SOF_FRAME_INDEX : RO ;bitpos:[10:0] ;default: 11'd0 ; */
/*description: Frame index of received SOF frame..*/
#define USB_SERIAL_JTAG_SOF_FRAME_INDEX    0x000007FF
#define USB_SERIAL_JTAG_SOF_FRAME_INDEX_M  ((USB_DEVICE_SOF_FRAME_INDEX_V)<<(USB_DEVICE_SOF_FRAME_INDEX_S))
#define USB_SERIAL_JTAG_SOF_FRAME_INDEX_V  0x7FF
#define USB_SERIAL_JTAG_SOF_FRAME_INDEX_S  0

#define USB_SERIAL_JTAG_IN_EP0_ST_REG          (DR_REG_USB_SERIAL_JTAG_BASE + 0x28)
/* USB_SERIAL_JTAG_IN_EP0_RD_ADDR : RO ;bitpos:[15:9] ;default: 7'd0 ; */
/*description: Read data address of IN endpoint 0..*/
#define USB_SERIAL_JTAG_IN_EP0_RD_ADDR    0x0000007F
#define USB_SERIAL_JTAG_IN_EP0_RD_ADDR_M  ((USB_DEVICE_IN_EP0_RD_ADDR_V)<<(USB_DEVICE_IN_EP0_RD_ADDR_S))
#define USB_SERIAL_JTAG_IN_EP0_RD_ADDR_V  0x7F
#define USB_SERIAL_JTAG_IN_EP0_RD_ADDR_S  9
/* USB_SERIAL_JTAG_IN_EP0_WR_ADDR : RO ;bitpos:[8:2] ;default: 7'd0 ; */
/*description: Write data address of IN endpoint 0..*/
#define USB_SERIAL_JTAG_IN_EP0_WR_ADDR    0x0000007F
#define USB_SERIAL_JTAG_IN_EP0_WR_ADDR_M  ((USB_DEVICE_IN_EP0_WR_ADDR_V)<<(USB_DEVICE_IN_EP0_WR_ADDR_S))
#define USB_SERIAL_JTAG_IN_EP0_WR_ADDR_V  0x7F
#define USB_SERIAL_JTAG_IN_EP0_WR_ADDR_S  2
/* USB_SERIAL_JTAG_IN_EP0_STATE : RO ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: State of IN Endpoint 0..*/
#define USB_SERIAL_JTAG_IN_EP0_STATE    0x00000003
#define USB_SERIAL_JTAG_IN_EP0_STATE_M  ((USB_DEVICE_IN_EP0_STATE_V)<<(USB_DEVICE_IN_EP0_STATE_S))
#define USB_SERIAL_JTAG_IN_EP0_STATE_V  0x3
#define USB_SERIAL_JTAG_IN_EP0_STATE_S  0

#define USB_SERIAL_JTAG_IN_EP1_ST_REG          (DR_REG_USB_SERIAL_JTAG_BASE + 0x2C)
/* USB_SERIAL_JTAG_IN_EP1_RD_ADDR : RO ;bitpos:[15:9] ;default: 7'd0 ; */
/*description: Read data address of IN endpoint 1..*/
#define USB_SERIAL_JTAG_IN_EP1_RD_ADDR    0x0000007F
#define USB_SERIAL_JTAG_IN_EP1_RD_ADDR_M  ((USB_DEVICE_IN_EP1_RD_ADDR_V)<<(USB_DEVICE_IN_EP1_RD_ADDR_S))
#define USB_SERIAL_JTAG_IN_EP1_RD_ADDR_V  0x7F
#define USB_SERIAL_JTAG_IN_EP1_RD_ADDR_S  9
/* USB_SERIAL_JTAG_IN_EP1_WR_ADDR : RO ;bitpos:[8:2] ;default: 7'd0 ; */
/*description: Write data address of IN endpoint 1..*/
#define USB_SERIAL_JTAG_IN_EP1_WR_ADDR    0x0000007F
#define USB_SERIAL_JTAG_IN_EP1_WR_ADDR_M  ((USB_DEVICE_IN_EP1_WR_ADDR_V)<<(USB_DEVICE_IN_EP1_WR_ADDR_S))
#define USB_SERIAL_JTAG_IN_EP1_WR_ADDR_V  0x7F
#define USB_SERIAL_JTAG_IN_EP1_WR_ADDR_S  2
/* USB_SERIAL_JTAG_IN_EP1_STATE : RO ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: State of IN Endpoint 1..*/
#define USB_SERIAL_JTAG_IN_EP1_STATE    0x00000003
#define USB_SERIAL_JTAG_IN_EP1_STATE_M  ((USB_DEVICE_IN_EP1_STATE_V)<<(USB_DEVICE_IN_EP1_STATE_S))
#define USB_SERIAL_JTAG_IN_EP1_STATE_V  0x3
#define USB_SERIAL_JTAG_IN_EP1_STATE_S  0

#define USB_SERIAL_JTAG_IN_EP2_ST_REG          (DR_REG_USB_SERIAL_JTAG_BASE + 0x30)
/* USB_SERIAL_JTAG_IN_EP2_RD_ADDR : RO ;bitpos:[15:9] ;default: 7'd0 ; */
/*description: Read data address of IN endpoint 2..*/
#define USB_SERIAL_JTAG_IN_EP2_RD_ADDR    0x0000007F
#define USB_SERIAL_JTAG_IN_EP2_RD_ADDR_M  ((USB_DEVICE_IN_EP2_RD_ADDR_V)<<(USB_DEVICE_IN_EP2_RD_ADDR_S))
#define USB_SERIAL_JTAG_IN_EP2_RD_ADDR_V  0x7F
#define USB_SERIAL_JTAG_IN_EP2_RD_ADDR_S  9
/* USB_SERIAL_JTAG_IN_EP2_WR_ADDR : RO ;bitpos:[8:2] ;default: 7'd0 ; */
/*description: Write data address of IN endpoint 2..*/
#define USB_SERIAL_JTAG_IN_EP2_WR_ADDR    0x0000007F
#define USB_SERIAL_JTAG_IN_EP2_WR_ADDR_M  ((USB_DEVICE_IN_EP2_WR_ADDR_V)<<(USB_DEVICE_IN_EP2_WR_ADDR_S))
#define USB_SERIAL_JTAG_IN_EP2_WR_ADDR_V  0x7F
#define USB_SERIAL_JTAG_IN_EP2_WR_ADDR_S  2
/* USB_SERIAL_JTAG_IN_EP2_STATE : RO ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: State of IN Endpoint 2..*/
#define USB_SERIAL_JTAG_IN_EP2_STATE    0x00000003
#define USB_SERIAL_JTAG_IN_EP2_STATE_M  ((USB_DEVICE_IN_EP2_STATE_V)<<(USB_DEVICE_IN_EP2_STATE_S))
#define USB_SERIAL_JTAG_IN_EP2_STATE_V  0x3
#define USB_SERIAL_JTAG_IN_EP2_STATE_S  0

#define USB_SERIAL_JTAG_IN_EP3_ST_REG          (DR_REG_USB_SERIAL_JTAG_BASE + 0x34)
/* USB_SERIAL_JTAG_IN_EP3_RD_ADDR : RO ;bitpos:[15:9] ;default: 7'd0 ; */
/*description: Read data address of IN endpoint 3..*/
#define USB_SERIAL_JTAG_IN_EP3_RD_ADDR    0x0000007F
#define USB_SERIAL_JTAG_IN_EP3_RD_ADDR_M  ((USB_DEVICE_IN_EP3_RD_ADDR_V)<<(USB_DEVICE_IN_EP3_RD_ADDR_S))
#define USB_SERIAL_JTAG_IN_EP3_RD_ADDR_V  0x7F
#define USB_SERIAL_JTAG_IN_EP3_RD_ADDR_S  9
/* USB_SERIAL_JTAG_IN_EP3_WR_ADDR : RO ;bitpos:[8:2] ;default: 7'd0 ; */
/*description: Write data address of IN endpoint 3..*/
#define USB_SERIAL_JTAG_IN_EP3_WR_ADDR    0x0000007F
#define USB_SERIAL_JTAG_IN_EP3_WR_ADDR_M  ((USB_DEVICE_IN_EP3_WR_ADDR_V)<<(USB_DEVICE_IN_EP3_WR_ADDR_S))
#define USB_SERIAL_JTAG_IN_EP3_WR_ADDR_V  0x7F
#define USB_SERIAL_JTAG_IN_EP3_WR_ADDR_S  2
/* USB_SERIAL_JTAG_IN_EP3_STATE : RO ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: State of IN Endpoint 3..*/
#define USB_SERIAL_JTAG_IN_EP3_STATE    0x00000003
#define USB_SERIAL_JTAG_IN_EP3_STATE_M  ((USB_DEVICE_IN_EP3_STATE_V)<<(USB_DEVICE_IN_EP3_STATE_S))
#define USB_SERIAL_JTAG_IN_EP3_STATE_V  0x3
#define USB_SERIAL_JTAG_IN_EP3_STATE_S  0

#define USB_SERIAL_JTAG_OUT_EP0_ST_REG          (DR_REG_USB_SERIAL_JTAG_BASE + 0x38)
/* USB_SERIAL_JTAG_OUT_EP0_RD_ADDR : RO ;bitpos:[15:9] ;default: 7'd0 ; */
/*description: Read data address of OUT endpoint 0..*/
#define USB_SERIAL_JTAG_OUT_EP0_RD_ADDR    0x0000007F
#define USB_SERIAL_JTAG_OUT_EP0_RD_ADDR_M  ((USB_DEVICE_OUT_EP0_RD_ADDR_V)<<(USB_DEVICE_OUT_EP0_RD_ADDR_S))
#define USB_SERIAL_JTAG_OUT_EP0_RD_ADDR_V  0x7F
#define USB_SERIAL_JTAG_OUT_EP0_RD_ADDR_S  9
/* USB_SERIAL_JTAG_OUT_EP0_WR_ADDR : RO ;bitpos:[8:2] ;default: 7'd0 ; */
/*description: Write data address of OUT endpoint 0. When USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT is
 detected, there are USB_SERIAL_JTAG_OUT_EP0_WR_ADDR-2 bytes data in OUT EP0..*/
#define USB_SERIAL_JTAG_OUT_EP0_WR_ADDR    0x0000007F
#define USB_SERIAL_JTAG_OUT_EP0_WR_ADDR_M  ((USB_DEVICE_OUT_EP0_WR_ADDR_V)<<(USB_DEVICE_OUT_EP0_WR_ADDR_S))
#define USB_SERIAL_JTAG_OUT_EP0_WR_ADDR_V  0x7F
#define USB_SERIAL_JTAG_OUT_EP0_WR_ADDR_S  2
/* USB_SERIAL_JTAG_OUT_EP0_STATE : RO ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: State of OUT Endpoint 0..*/
#define USB_SERIAL_JTAG_OUT_EP0_STATE    0x00000003
#define USB_SERIAL_JTAG_OUT_EP0_STATE_M  ((USB_DEVICE_OUT_EP0_STATE_V)<<(USB_DEVICE_OUT_EP0_STATE_S))
#define USB_SERIAL_JTAG_OUT_EP0_STATE_V  0x3
#define USB_SERIAL_JTAG_OUT_EP0_STATE_S  0

#define USB_SERIAL_JTAG_OUT_EP1_ST_REG          (DR_REG_USB_SERIAL_JTAG_BASE + 0x3C)
/* USB_SERIAL_JTAG_OUT_EP1_REC_DATA_CNT : RO ;bitpos:[22:16] ;default: 7'd0 ; */
/*description: Data count in OUT endpoint 1 when one packet is received..*/
#define USB_SERIAL_JTAG_OUT_EP1_REC_DATA_CNT    0x0000007F
#define USB_SERIAL_JTAG_OUT_EP1_REC_DATA_CNT_M  ((USB_DEVICE_OUT_EP1_REC_DATA_CNT_V)<<(USB_DEVICE_OUT_EP1_REC_DATA_CNT_S))
#define USB_SERIAL_JTAG_OUT_EP1_REC_DATA_CNT_V  0x7F
#define USB_SERIAL_JTAG_OUT_EP1_REC_DATA_CNT_S  16
/* USB_SERIAL_JTAG_OUT_EP1_RD_ADDR : RO ;bitpos:[15:9] ;default: 7'd0 ; */
/*description: Read data address of OUT endpoint 1..*/
#define USB_SERIAL_JTAG_OUT_EP1_RD_ADDR    0x0000007F
#define USB_SERIAL_JTAG_OUT_EP1_RD_ADDR_M  ((USB_DEVICE_OUT_EP1_RD_ADDR_V)<<(USB_DEVICE_OUT_EP1_RD_ADDR_S))
#define USB_SERIAL_JTAG_OUT_EP1_RD_ADDR_V  0x7F
#define USB_SERIAL_JTAG_OUT_EP1_RD_ADDR_S  9
/* USB_SERIAL_JTAG_OUT_EP1_WR_ADDR : RO ;bitpos:[8:2] ;default: 7'd0 ; */
/*description: Write data address of OUT endpoint 1. When USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT is
 detected, there are USB_SERIAL_JTAG_OUT_EP1_WR_ADDR-2 bytes data in OUT EP1..*/
#define USB_SERIAL_JTAG_OUT_EP1_WR_ADDR    0x0000007F
#define USB_SERIAL_JTAG_OUT_EP1_WR_ADDR_M  ((USB_DEVICE_OUT_EP1_WR_ADDR_V)<<(USB_DEVICE_OUT_EP1_WR_ADDR_S))
#define USB_SERIAL_JTAG_OUT_EP1_WR_ADDR_V  0x7F
#define USB_SERIAL_JTAG_OUT_EP1_WR_ADDR_S  2
/* USB_SERIAL_JTAG_OUT_EP1_STATE : RO ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: State of OUT Endpoint 1..*/
#define USB_SERIAL_JTAG_OUT_EP1_STATE    0x00000003
#define USB_SERIAL_JTAG_OUT_EP1_STATE_M  ((USB_DEVICE_OUT_EP1_STATE_V)<<(USB_DEVICE_OUT_EP1_STATE_S))
#define USB_SERIAL_JTAG_OUT_EP1_STATE_V  0x3
#define USB_SERIAL_JTAG_OUT_EP1_STATE_S  0

#define USB_SERIAL_JTAG_OUT_EP2_ST_REG          (DR_REG_USB_SERIAL_JTAG_BASE + 0x40)
/* USB_SERIAL_JTAG_OUT_EP2_RD_ADDR : RO ;bitpos:[15:9] ;default: 7'd0 ; */
/*description: Read data address of OUT endpoint 2..*/
#define USB_SERIAL_JTAG_OUT_EP2_RD_ADDR    0x0000007F
#define USB_SERIAL_JTAG_OUT_EP2_RD_ADDR_M  ((USB_DEVICE_OUT_EP2_RD_ADDR_V)<<(USB_DEVICE_OUT_EP2_RD_ADDR_S))
#define USB_SERIAL_JTAG_OUT_EP2_RD_ADDR_V  0x7F
#define USB_SERIAL_JTAG_OUT_EP2_RD_ADDR_S  9
/* USB_SERIAL_JTAG_OUT_EP2_WR_ADDR : RO ;bitpos:[8:2] ;default: 7'd0 ; */
/*description: Write data address of OUT endpoint 2. When USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT is
 detected, there are USB_SERIAL_JTAG_OUT_EP2_WR_ADDR-2 bytes data in OUT EP2..*/
#define USB_SERIAL_JTAG_OUT_EP2_WR_ADDR    0x0000007F
#define USB_SERIAL_JTAG_OUT_EP2_WR_ADDR_M  ((USB_DEVICE_OUT_EP2_WR_ADDR_V)<<(USB_DEVICE_OUT_EP2_WR_ADDR_S))
#define USB_SERIAL_JTAG_OUT_EP2_WR_ADDR_V  0x7F
#define USB_SERIAL_JTAG_OUT_EP2_WR_ADDR_S  2
/* USB_SERIAL_JTAG_OUT_EP2_STATE : RO ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: State of OUT Endpoint 2..*/
#define USB_SERIAL_JTAG_OUT_EP2_STATE    0x00000003
#define USB_SERIAL_JTAG_OUT_EP2_STATE_M  ((USB_DEVICE_OUT_EP2_STATE_V)<<(USB_DEVICE_OUT_EP2_STATE_S))
#define USB_SERIAL_JTAG_OUT_EP2_STATE_V  0x3
#define USB_SERIAL_JTAG_OUT_EP2_STATE_S  0

#define USB_SERIAL_JTAG_MISC_CONF_REG          (DR_REG_USB_SERIAL_JTAG_BASE + 0x44)
/* USB_SERIAL_JTAG_CLK_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: 1'h1: Force clock on for register. 1'h0: Support clock only when application wri
tes registers..*/
#define USB_SERIAL_JTAG_CLK_EN    (BIT(0))
#define USB_SERIAL_JTAG_CLK_EN_M  (BIT(0))
#define USB_SERIAL_JTAG_CLK_EN_V  0x1
#define USB_SERIAL_JTAG_CLK_EN_S  0

#define USB_SERIAL_JTAG_MEM_CONF_REG          (DR_REG_USB_SERIAL_JTAG_BASE + 0x48)
/* USB_SERIAL_JTAG_USB_MEM_CLK_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: 1: Force clock on for usb memory..*/
#define USB_SERIAL_JTAG_USB_MEM_CLK_EN    (BIT(1))
#define USB_SERIAL_JTAG_USB_MEM_CLK_EN_M  (BIT(1))
#define USB_SERIAL_JTAG_USB_MEM_CLK_EN_V  0x1
#define USB_SERIAL_JTAG_USB_MEM_CLK_EN_S  1
/* USB_SERIAL_JTAG_USB_MEM_PD : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: 1: power down usb memory..*/
#define USB_SERIAL_JTAG_USB_MEM_PD    (BIT(0))
#define USB_SERIAL_JTAG_USB_MEM_PD_M  (BIT(0))
#define USB_SERIAL_JTAG_USB_MEM_PD_V  0x1
#define USB_SERIAL_JTAG_USB_MEM_PD_S  0

#define USB_SERIAL_JTAG_DATE_REG          (DR_REG_USB_SERIAL_JTAG_BASE + 0x80)
/* USB_SERIAL_JTAG_DATE : R/W ;bitpos:[31:0] ;default: 32'h2101200 ; */
/*description: register version..*/
#define USB_SERIAL_JTAG_DATE    0xFFFFFFFF
#define USB_SERIAL_JTAG_DATE_M  ((USB_DEVICE_DATE_V)<<(USB_DEVICE_DATE_S))
#define USB_SERIAL_JTAG_DATE_V  0xFFFFFFFF
#define USB_SERIAL_JTAG_DATE_S  0


#ifdef __cplusplus
}
#endif
