/** Copyright 2020 Espressif Systems (Shanghai) PTE LTD
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** CP_DMA_INT_RAW_REG register
 *  Raw interrupt status
 */
#define CP_DMA_INT_RAW_REG (DR_REG_CP_BASE + 0x0)
/** CP_DMA_IN_DONE_INT_RAW : RO; bitpos: [0]; default: 0;
 *  This is the interrupt raw bit. Triggered when the last data of frame is received or
 *  the receive buffer is full indicated by inlink descriptor.
 */
#define CP_DMA_IN_DONE_INT_RAW    (BIT(0))
#define CP_DMA_IN_DONE_INT_RAW_M  (CP_DMA_IN_DONE_INT_RAW_V << CP_DMA_IN_DONE_INT_RAW_S)
#define CP_DMA_IN_DONE_INT_RAW_V  0x00000001
#define CP_DMA_IN_DONE_INT_RAW_S  0
/** CP_DMA_IN_SUC_EOF_INT_RAW : RO; bitpos: [1]; default: 0;
 *  This is the interrupt raw bit. Triggered when the last data of one frame is
 *  received.
 */
#define CP_DMA_IN_SUC_EOF_INT_RAW    (BIT(1))
#define CP_DMA_IN_SUC_EOF_INT_RAW_M  (CP_DMA_IN_SUC_EOF_INT_RAW_V << CP_DMA_IN_SUC_EOF_INT_RAW_S)
#define CP_DMA_IN_SUC_EOF_INT_RAW_V  0x00000001
#define CP_DMA_IN_SUC_EOF_INT_RAW_S  1
/** CP_DMA_OUT_DONE_INT_RAW : RO; bitpos: [2]; default: 0;
 *  This is the interrupt raw bit. Triggered when all data indicated by one outlink
 *  descriptor has been pushed into Tx FIFO.
 */
#define CP_DMA_OUT_DONE_INT_RAW    (BIT(2))
#define CP_DMA_OUT_DONE_INT_RAW_M  (CP_DMA_OUT_DONE_INT_RAW_V << CP_DMA_OUT_DONE_INT_RAW_S)
#define CP_DMA_OUT_DONE_INT_RAW_V  0x00000001
#define CP_DMA_OUT_DONE_INT_RAW_S  2
/** CP_DMA_OUT_EOF_INT_RAW : RO; bitpos: [3]; default: 0;
 *  This is the interrupt raw bit. Triggered when the last data with EOF flag has been
 *  pushed into Tx FIFO.
 */
#define CP_DMA_OUT_EOF_INT_RAW    (BIT(3))
#define CP_DMA_OUT_EOF_INT_RAW_M  (CP_DMA_OUT_EOF_INT_RAW_V << CP_DMA_OUT_EOF_INT_RAW_S)
#define CP_DMA_OUT_EOF_INT_RAW_V  0x00000001
#define CP_DMA_OUT_EOF_INT_RAW_S  3
/** CP_DMA_IN_DSCR_ERR_INT_RAW : RO; bitpos: [4]; default: 0;
 *  This is the interrupt raw bit. Triggered when detecting inlink descriptor error,
 *  including owner error, the second and third word error of inlink descriptor.
 */
#define CP_DMA_IN_DSCR_ERR_INT_RAW    (BIT(4))
#define CP_DMA_IN_DSCR_ERR_INT_RAW_M  (CP_DMA_IN_DSCR_ERR_INT_RAW_V << CP_DMA_IN_DSCR_ERR_INT_RAW_S)
#define CP_DMA_IN_DSCR_ERR_INT_RAW_V  0x00000001
#define CP_DMA_IN_DSCR_ERR_INT_RAW_S  4
/** CP_DMA_OUT_DSCR_ERR_INT_RAW : RO; bitpos: [5]; default: 0;
 *  This is the interrupt raw bit. Triggered when detecting outlink descriptor error,
 *  including owner error, the second and third word error of outlink descriptor.
 */
#define CP_DMA_OUT_DSCR_ERR_INT_RAW    (BIT(5))
#define CP_DMA_OUT_DSCR_ERR_INT_RAW_M  (CP_DMA_OUT_DSCR_ERR_INT_RAW_V << CP_DMA_OUT_DSCR_ERR_INT_RAW_S)
#define CP_DMA_OUT_DSCR_ERR_INT_RAW_V  0x00000001
#define CP_DMA_OUT_DSCR_ERR_INT_RAW_S  5
/** CP_DMA_IN_DSCR_EMPTY_INT_RAW : RO; bitpos: [6]; default: 0;
 *  This is the interrupt raw bit. Triggered when receiving data is completed and no
 *  more inlink descriptor.
 */
#define CP_DMA_IN_DSCR_EMPTY_INT_RAW    (BIT(6))
#define CP_DMA_IN_DSCR_EMPTY_INT_RAW_M  (CP_DMA_IN_DSCR_EMPTY_INT_RAW_V << CP_DMA_IN_DSCR_EMPTY_INT_RAW_S)
#define CP_DMA_IN_DSCR_EMPTY_INT_RAW_V  0x00000001
#define CP_DMA_IN_DSCR_EMPTY_INT_RAW_S  6
/** CP_DMA_OUT_TOTAL_EOF_INT_RAW : RO; bitpos: [7]; default: 0;
 *  This is the interrupt raw bit. Triggered when data corresponding to all outlink
 *  descriptor and the last descriptor with valid EOF is transmitted out.
 */
#define CP_DMA_OUT_TOTAL_EOF_INT_RAW    (BIT(7))
#define CP_DMA_OUT_TOTAL_EOF_INT_RAW_M  (CP_DMA_OUT_TOTAL_EOF_INT_RAW_V << CP_DMA_OUT_TOTAL_EOF_INT_RAW_S)
#define CP_DMA_OUT_TOTAL_EOF_INT_RAW_V  0x00000001
#define CP_DMA_OUT_TOTAL_EOF_INT_RAW_S  7
/** CP_DMA_CRC_DONE_INT_RAW : RO; bitpos: [8]; default: 0;
 *  This is the interrupt raw bit. Triggered when crc calculation is done.
 */
#define CP_DMA_CRC_DONE_INT_RAW    (BIT(8))
#define CP_DMA_CRC_DONE_INT_RAW_M  (CP_DMA_CRC_DONE_INT_RAW_V << CP_DMA_CRC_DONE_INT_RAW_S)
#define CP_DMA_CRC_DONE_INT_RAW_V  0x00000001
#define CP_DMA_CRC_DONE_INT_RAW_S  8

/** CP_DMA_INT_ST_REG register
 *  Masked interrupt status
 */
#define CP_DMA_INT_ST_REG (DR_REG_CP_BASE + 0x4)
/** CP_DMA_IN_DONE_INT_ST : RO; bitpos: [0]; default: 0;
 *  This is the masked interrupt bit for cp_in_done_int interrupt when
 *  cp_in_done_int_ena is set to 1.
 */
#define CP_DMA_IN_DONE_INT_ST    (BIT(0))
#define CP_DMA_IN_DONE_INT_ST_M  (CP_DMA_IN_DONE_INT_ST_V << CP_DMA_IN_DONE_INT_ST_S)
#define CP_DMA_IN_DONE_INT_ST_V  0x00000001
#define CP_DMA_IN_DONE_INT_ST_S  0
/** CP_DMA_IN_SUC_EOF_INT_ST : RO; bitpos: [1]; default: 0;
 *  This is the masked interrupt bit for cp_in_suc_eof_int interrupt when
 *  cp_in_suc_eof_int_ena is set to 1.
 */
#define CP_DMA_IN_SUC_EOF_INT_ST    (BIT(1))
#define CP_DMA_IN_SUC_EOF_INT_ST_M  (CP_DMA_IN_SUC_EOF_INT_ST_V << CP_DMA_IN_SUC_EOF_INT_ST_S)
#define CP_DMA_IN_SUC_EOF_INT_ST_V  0x00000001
#define CP_DMA_IN_SUC_EOF_INT_ST_S  1
/** CP_DMA_OUT_DONE_INT_ST : RO; bitpos: [2]; default: 0;
 *  This is the masked interrupt bit for cp_out_done_int interrupt when
 *  cp_out_done_int_ena is set to 1.
 */
#define CP_DMA_OUT_DONE_INT_ST    (BIT(2))
#define CP_DMA_OUT_DONE_INT_ST_M  (CP_DMA_OUT_DONE_INT_ST_V << CP_DMA_OUT_DONE_INT_ST_S)
#define CP_DMA_OUT_DONE_INT_ST_V  0x00000001
#define CP_DMA_OUT_DONE_INT_ST_S  2
/** CP_DMA_OUT_EOF_INT_ST : RO; bitpos: [3]; default: 0;
 *  This is the masked interrupt bit for cp_out_eof_int interrupt when
 *  cp_out_eof_int_ena is set to 1.
 */
#define CP_DMA_OUT_EOF_INT_ST    (BIT(3))
#define CP_DMA_OUT_EOF_INT_ST_M  (CP_DMA_OUT_EOF_INT_ST_V << CP_DMA_OUT_EOF_INT_ST_S)
#define CP_DMA_OUT_EOF_INT_ST_V  0x00000001
#define CP_DMA_OUT_EOF_INT_ST_S  3
/** CP_DMA_IN_DSCR_ERR_INT_ST : RO; bitpos: [4]; default: 0;
 *  This is the masked interrupt bit for cp_in_dscr_err_int interrupt when
 *  cp_in_dscr_err_int_ena is set to 1.
 */
#define CP_DMA_IN_DSCR_ERR_INT_ST    (BIT(4))
#define CP_DMA_IN_DSCR_ERR_INT_ST_M  (CP_DMA_IN_DSCR_ERR_INT_ST_V << CP_DMA_IN_DSCR_ERR_INT_ST_S)
#define CP_DMA_IN_DSCR_ERR_INT_ST_V  0x00000001
#define CP_DMA_IN_DSCR_ERR_INT_ST_S  4
/** CP_DMA_OUT_DSCR_ERR_INT_ST : RO; bitpos: [5]; default: 0;
 *  This is the masked interrupt bit for cp_out_dscr_err_int interrupt when
 *  cp_out_dscr_err_int_ena is set to 1.
 */
#define CP_DMA_OUT_DSCR_ERR_INT_ST    (BIT(5))
#define CP_DMA_OUT_DSCR_ERR_INT_ST_M  (CP_DMA_OUT_DSCR_ERR_INT_ST_V << CP_DMA_OUT_DSCR_ERR_INT_ST_S)
#define CP_DMA_OUT_DSCR_ERR_INT_ST_V  0x00000001
#define CP_DMA_OUT_DSCR_ERR_INT_ST_S  5
/** CP_DMA_IN_DSCR_EMPTY_INT_ST : RO; bitpos: [6]; default: 0;
 *  This is the masked interrupt bit for cp_in_dscr_empty_int interrupt when
 *  cp_in_dscr_empty_int_ena is set to 1.
 */
#define CP_DMA_IN_DSCR_EMPTY_INT_ST    (BIT(6))
#define CP_DMA_IN_DSCR_EMPTY_INT_ST_M  (CP_DMA_IN_DSCR_EMPTY_INT_ST_V << CP_DMA_IN_DSCR_EMPTY_INT_ST_S)
#define CP_DMA_IN_DSCR_EMPTY_INT_ST_V  0x00000001
#define CP_DMA_IN_DSCR_EMPTY_INT_ST_S  6
/** CP_DMA_OUT_TOTAL_EOF_INT_ST : RO; bitpos: [7]; default: 0;
 *  This is the masked interrupt bit for cp_out_total_eof_int interrupt when
 *  cp_out_total_eof_int_ena is set to 1.
 */
#define CP_DMA_OUT_TOTAL_EOF_INT_ST    (BIT(7))
#define CP_DMA_OUT_TOTAL_EOF_INT_ST_M  (CP_DMA_OUT_TOTAL_EOF_INT_ST_V << CP_DMA_OUT_TOTAL_EOF_INT_ST_S)
#define CP_DMA_OUT_TOTAL_EOF_INT_ST_V  0x00000001
#define CP_DMA_OUT_TOTAL_EOF_INT_ST_S  7
/** CP_DMA_CRC_DONE_INT_ST : RO; bitpos: [8]; default: 0;
 *  This is the masked interrupt bit for cp_crc_done_int interrupt when
 *  cp_crc_done_int_ena is set to 1.
 */
#define CP_DMA_CRC_DONE_INT_ST    (BIT(8))
#define CP_DMA_CRC_DONE_INT_ST_M  (CP_DMA_CRC_DONE_INT_ST_V << CP_DMA_CRC_DONE_INT_ST_S)
#define CP_DMA_CRC_DONE_INT_ST_V  0x00000001
#define CP_DMA_CRC_DONE_INT_ST_S  8

/** CP_DMA_INT_ENA_REG register
 *  Interrupt enable bits
 */
#define CP_DMA_INT_ENA_REG (DR_REG_CP_BASE + 0x8)
/** CP_DMA_IN_DONE_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  This is the interrupt enable bit for cp_in_done_int interrupt.
 */
#define CP_DMA_IN_DONE_INT_ENA    (BIT(0))
#define CP_DMA_IN_DONE_INT_ENA_M  (CP_DMA_IN_DONE_INT_ENA_V << CP_DMA_IN_DONE_INT_ENA_S)
#define CP_DMA_IN_DONE_INT_ENA_V  0x00000001
#define CP_DMA_IN_DONE_INT_ENA_S  0
/** CP_DMA_IN_SUC_EOF_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  This is the interrupt enable bit for cp_in_suc_eof_int interrupt.
 */
#define CP_DMA_IN_SUC_EOF_INT_ENA    (BIT(1))
#define CP_DMA_IN_SUC_EOF_INT_ENA_M  (CP_DMA_IN_SUC_EOF_INT_ENA_V << CP_DMA_IN_SUC_EOF_INT_ENA_S)
#define CP_DMA_IN_SUC_EOF_INT_ENA_V  0x00000001
#define CP_DMA_IN_SUC_EOF_INT_ENA_S  1
/** CP_DMA_OUT_DONE_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  This is the interrupt enable bit for cp_out_done_int interrupt.
 */
#define CP_DMA_OUT_DONE_INT_ENA    (BIT(2))
#define CP_DMA_OUT_DONE_INT_ENA_M  (CP_DMA_OUT_DONE_INT_ENA_V << CP_DMA_OUT_DONE_INT_ENA_S)
#define CP_DMA_OUT_DONE_INT_ENA_V  0x00000001
#define CP_DMA_OUT_DONE_INT_ENA_S  2
/** CP_DMA_OUT_EOF_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  This is the interrupt enable bit for cp_out_eof_int interrupt.
 */
#define CP_DMA_OUT_EOF_INT_ENA    (BIT(3))
#define CP_DMA_OUT_EOF_INT_ENA_M  (CP_DMA_OUT_EOF_INT_ENA_V << CP_DMA_OUT_EOF_INT_ENA_S)
#define CP_DMA_OUT_EOF_INT_ENA_V  0x00000001
#define CP_DMA_OUT_EOF_INT_ENA_S  3
/** CP_DMA_IN_DSCR_ERR_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  This is the interrupt enable bit for cp_in_dscr_err_int interrupt.
 */
#define CP_DMA_IN_DSCR_ERR_INT_ENA    (BIT(4))
#define CP_DMA_IN_DSCR_ERR_INT_ENA_M  (CP_DMA_IN_DSCR_ERR_INT_ENA_V << CP_DMA_IN_DSCR_ERR_INT_ENA_S)
#define CP_DMA_IN_DSCR_ERR_INT_ENA_V  0x00000001
#define CP_DMA_IN_DSCR_ERR_INT_ENA_S  4
/** CP_DMA_OUT_DSCR_ERR_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  This is the interrupt enable bit for cp_out_dscr_err_int interrupt.
 */
#define CP_DMA_OUT_DSCR_ERR_INT_ENA    (BIT(5))
#define CP_DMA_OUT_DSCR_ERR_INT_ENA_M  (CP_DMA_OUT_DSCR_ERR_INT_ENA_V << CP_DMA_OUT_DSCR_ERR_INT_ENA_S)
#define CP_DMA_OUT_DSCR_ERR_INT_ENA_V  0x00000001
#define CP_DMA_OUT_DSCR_ERR_INT_ENA_S  5
/** CP_DMA_IN_DSCR_EMPTY_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  This is the interrupt enable bit for cp_in_dscr_empty_int interrupt.
 */
#define CP_DMA_IN_DSCR_EMPTY_INT_ENA    (BIT(6))
#define CP_DMA_IN_DSCR_EMPTY_INT_ENA_M  (CP_DMA_IN_DSCR_EMPTY_INT_ENA_V << CP_DMA_IN_DSCR_EMPTY_INT_ENA_S)
#define CP_DMA_IN_DSCR_EMPTY_INT_ENA_V  0x00000001
#define CP_DMA_IN_DSCR_EMPTY_INT_ENA_S  6
/** CP_DMA_OUT_TOTAL_EOF_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  This is the interrupt enable bit for cp_out_total_eof_int interrupt.
 */
#define CP_DMA_OUT_TOTAL_EOF_INT_ENA    (BIT(7))
#define CP_DMA_OUT_TOTAL_EOF_INT_ENA_M  (CP_DMA_OUT_TOTAL_EOF_INT_ENA_V << CP_DMA_OUT_TOTAL_EOF_INT_ENA_S)
#define CP_DMA_OUT_TOTAL_EOF_INT_ENA_V  0x00000001
#define CP_DMA_OUT_TOTAL_EOF_INT_ENA_S  7
/** CP_DMA_CRC_DONE_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  This is the interrupt enable bit for cp_crc_done_int interrupt.
 */
#define CP_DMA_CRC_DONE_INT_ENA    (BIT(8))
#define CP_DMA_CRC_DONE_INT_ENA_M  (CP_DMA_CRC_DONE_INT_ENA_V << CP_DMA_CRC_DONE_INT_ENA_S)
#define CP_DMA_CRC_DONE_INT_ENA_V  0x00000001
#define CP_DMA_CRC_DONE_INT_ENA_S  8

/** CP_DMA_INT_CLR_REG register
 *  Interrupt clear bits
 */
#define CP_DMA_INT_CLR_REG (DR_REG_CP_BASE + 0xc)
/** CP_DMA_IN_DONE_INT_CLR : WO; bitpos: [0]; default: 0;
 *  Set this bit to clear cp_in_done_int interrupt.
 */
#define CP_DMA_IN_DONE_INT_CLR    (BIT(0))
#define CP_DMA_IN_DONE_INT_CLR_M  (CP_DMA_IN_DONE_INT_CLR_V << CP_DMA_IN_DONE_INT_CLR_S)
#define CP_DMA_IN_DONE_INT_CLR_V  0x00000001
#define CP_DMA_IN_DONE_INT_CLR_S  0
/** CP_DMA_IN_SUC_EOF_INT_CLR : WO; bitpos: [1]; default: 0;
 *  Set this bit to clear cp_in_suc_eof_int interrupt.
 */
#define CP_DMA_IN_SUC_EOF_INT_CLR    (BIT(1))
#define CP_DMA_IN_SUC_EOF_INT_CLR_M  (CP_DMA_IN_SUC_EOF_INT_CLR_V << CP_DMA_IN_SUC_EOF_INT_CLR_S)
#define CP_DMA_IN_SUC_EOF_INT_CLR_V  0x00000001
#define CP_DMA_IN_SUC_EOF_INT_CLR_S  1
/** CP_DMA_OUT_DONE_INT_CLR : WO; bitpos: [2]; default: 0;
 *  Set this bit to clear cp_out_done_int interrupt.
 */
#define CP_DMA_OUT_DONE_INT_CLR    (BIT(2))
#define CP_DMA_OUT_DONE_INT_CLR_M  (CP_DMA_OUT_DONE_INT_CLR_V << CP_DMA_OUT_DONE_INT_CLR_S)
#define CP_DMA_OUT_DONE_INT_CLR_V  0x00000001
#define CP_DMA_OUT_DONE_INT_CLR_S  2
/** CP_DMA_OUT_EOF_INT_CLR : WO; bitpos: [3]; default: 0;
 *  Set this bit to clear cp_out_eof_int interrupt.
 */
#define CP_DMA_OUT_EOF_INT_CLR    (BIT(3))
#define CP_DMA_OUT_EOF_INT_CLR_M  (CP_DMA_OUT_EOF_INT_CLR_V << CP_DMA_OUT_EOF_INT_CLR_S)
#define CP_DMA_OUT_EOF_INT_CLR_V  0x00000001
#define CP_DMA_OUT_EOF_INT_CLR_S  3
/** CP_DMA_IN_DSCR_ERR_INT_CLR : WO; bitpos: [4]; default: 0;
 *  Set this bit to clear cp_in_dscr_err_int interrupt.
 */
#define CP_DMA_IN_DSCR_ERR_INT_CLR    (BIT(4))
#define CP_DMA_IN_DSCR_ERR_INT_CLR_M  (CP_DMA_IN_DSCR_ERR_INT_CLR_V << CP_DMA_IN_DSCR_ERR_INT_CLR_S)
#define CP_DMA_IN_DSCR_ERR_INT_CLR_V  0x00000001
#define CP_DMA_IN_DSCR_ERR_INT_CLR_S  4
/** CP_DMA_OUT_DSCR_ERR_INT_CLR : WO; bitpos: [5]; default: 0;
 *  Set this bit to clear cp_out_dscr_err_int interrupt.
 */
#define CP_DMA_OUT_DSCR_ERR_INT_CLR    (BIT(5))
#define CP_DMA_OUT_DSCR_ERR_INT_CLR_M  (CP_DMA_OUT_DSCR_ERR_INT_CLR_V << CP_DMA_OUT_DSCR_ERR_INT_CLR_S)
#define CP_DMA_OUT_DSCR_ERR_INT_CLR_V  0x00000001
#define CP_DMA_OUT_DSCR_ERR_INT_CLR_S  5
/** CP_DMA_IN_DSCR_EMPTY_INT_CLR : WO; bitpos: [6]; default: 0;
 *  Set this bit to clear cp_in_dscr_empty_int interrupt.
 */
#define CP_DMA_IN_DSCR_EMPTY_INT_CLR    (BIT(6))
#define CP_DMA_IN_DSCR_EMPTY_INT_CLR_M  (CP_DMA_IN_DSCR_EMPTY_INT_CLR_V << CP_DMA_IN_DSCR_EMPTY_INT_CLR_S)
#define CP_DMA_IN_DSCR_EMPTY_INT_CLR_V  0x00000001
#define CP_DMA_IN_DSCR_EMPTY_INT_CLR_S  6
/** CP_DMA_OUT_TOTAL_EOF_INT_CLR : WO; bitpos: [7]; default: 0;
 *  Set this bit to clear cp_out_total_eof_int interrupt.
 */
#define CP_DMA_OUT_TOTAL_EOF_INT_CLR    (BIT(7))
#define CP_DMA_OUT_TOTAL_EOF_INT_CLR_M  (CP_DMA_OUT_TOTAL_EOF_INT_CLR_V << CP_DMA_OUT_TOTAL_EOF_INT_CLR_S)
#define CP_DMA_OUT_TOTAL_EOF_INT_CLR_V  0x00000001
#define CP_DMA_OUT_TOTAL_EOF_INT_CLR_S  7
/** CP_DMA_CRC_DONE_INT_CLR : WO; bitpos: [8]; default: 0;
 *  Set this bit to clear cp_crc_done_int interrupt.
 */
#define CP_DMA_CRC_DONE_INT_CLR    (BIT(8))
#define CP_DMA_CRC_DONE_INT_CLR_M  (CP_DMA_CRC_DONE_INT_CLR_V << CP_DMA_CRC_DONE_INT_CLR_S)
#define CP_DMA_CRC_DONE_INT_CLR_V  0x00000001
#define CP_DMA_CRC_DONE_INT_CLR_S  8

/** CP_DMA_OUT_LINK_REG register
 *  Link descriptor address and control
 */
#define CP_DMA_OUT_LINK_REG (DR_REG_CP_BASE + 0x10)
/** CP_DMA_OUTLINK_ADDR : R/W; bitpos: [19:0]; default: 0;
 *  This register is used to specify the least significant 20 bits of the first outlink
 *  descriptor's address.
 */
#define CP_DMA_OUTLINK_ADDR    0x000FFFFF
#define CP_DMA_OUTLINK_ADDR_M  (CP_DMA_OUTLINK_ADDR_V << CP_DMA_OUTLINK_ADDR_S)
#define CP_DMA_OUTLINK_ADDR_V  0x000FFFFF
#define CP_DMA_OUTLINK_ADDR_S  0
/** CP_DMA_OUTLINK_STOP : R/W; bitpos: [28]; default: 0;
 *  Set this bit to stop dealing with the outlink descriptor.
 */
#define CP_DMA_OUTLINK_STOP    (BIT(28))
#define CP_DMA_OUTLINK_STOP_M  (CP_DMA_OUTLINK_STOP_V << CP_DMA_OUTLINK_STOP_S)
#define CP_DMA_OUTLINK_STOP_V  0x00000001
#define CP_DMA_OUTLINK_STOP_S  28
/** CP_DMA_OUTLINK_START : R/W; bitpos: [29]; default: 0;
 *  Set this bit to start a new outlink descriptor.
 */
#define CP_DMA_OUTLINK_START    (BIT(29))
#define CP_DMA_OUTLINK_START_M  (CP_DMA_OUTLINK_START_V << CP_DMA_OUTLINK_START_S)
#define CP_DMA_OUTLINK_START_V  0x00000001
#define CP_DMA_OUTLINK_START_S  29
/** CP_DMA_OUTLINK_RESTART : R/W; bitpos: [30]; default: 0;
 *  Set this bit to restart the outlink descriptpr from the last address.
 */
#define CP_DMA_OUTLINK_RESTART    (BIT(30))
#define CP_DMA_OUTLINK_RESTART_M  (CP_DMA_OUTLINK_RESTART_V << CP_DMA_OUTLINK_RESTART_S)
#define CP_DMA_OUTLINK_RESTART_V  0x00000001
#define CP_DMA_OUTLINK_RESTART_S  30
/** CP_DMA_OUTLINK_PARK : RO; bitpos: [31]; default: 0;
 *  1: the outlink descriptor's FSM is in idle state.
 *  0: the outlink descriptor's FSM is working.
 */
#define CP_DMA_OUTLINK_PARK    (BIT(31))
#define CP_DMA_OUTLINK_PARK_M  (CP_DMA_OUTLINK_PARK_V << CP_DMA_OUTLINK_PARK_S)
#define CP_DMA_OUTLINK_PARK_V  0x00000001
#define CP_DMA_OUTLINK_PARK_S  31

/** CP_DMA_IN_LINK_REG register
 *  Link descriptor address and control
 */
#define CP_DMA_IN_LINK_REG (DR_REG_CP_BASE + 0x14)
/** CP_DMA_INLINK_ADDR : R/W; bitpos: [19:0]; default: 0;
 *  This register is used to specify the least significant 20 bits of the first inlink
 *  descriptor's address.
 */
#define CP_DMA_INLINK_ADDR    0x000FFFFF
#define CP_DMA_INLINK_ADDR_M  (CP_DMA_INLINK_ADDR_V << CP_DMA_INLINK_ADDR_S)
#define CP_DMA_INLINK_ADDR_V  0x000FFFFF
#define CP_DMA_INLINK_ADDR_S  0
/** CP_DMA_INLINK_STOP : R/W; bitpos: [28]; default: 0;
 *  Set this bit to stop dealing with the inlink descriptors.
 */
#define CP_DMA_INLINK_STOP    (BIT(28))
#define CP_DMA_INLINK_STOP_M  (CP_DMA_INLINK_STOP_V << CP_DMA_INLINK_STOP_S)
#define CP_DMA_INLINK_STOP_V  0x00000001
#define CP_DMA_INLINK_STOP_S  28
/** CP_DMA_INLINK_START : R/W; bitpos: [29]; default: 0;
 *  Set this bit to start dealing with the inlink descriptors.
 */
#define CP_DMA_INLINK_START    (BIT(29))
#define CP_DMA_INLINK_START_M  (CP_DMA_INLINK_START_V << CP_DMA_INLINK_START_S)
#define CP_DMA_INLINK_START_V  0x00000001
#define CP_DMA_INLINK_START_S  29
/** CP_DMA_INLINK_RESTART : R/W; bitpos: [30]; default: 0;
 *  Set this bit to restart new inlink descriptors.
 */
#define CP_DMA_INLINK_RESTART    (BIT(30))
#define CP_DMA_INLINK_RESTART_M  (CP_DMA_INLINK_RESTART_V << CP_DMA_INLINK_RESTART_S)
#define CP_DMA_INLINK_RESTART_V  0x00000001
#define CP_DMA_INLINK_RESTART_S  30
/** CP_DMA_INLINK_PARK : RO; bitpos: [31]; default: 0;
 *  1: the inlink descriptor's FSM is in idle state.
 *  0: the inlink descriptor's FSM is working.
 */
#define CP_DMA_INLINK_PARK    (BIT(31))
#define CP_DMA_INLINK_PARK_M  (CP_DMA_INLINK_PARK_V << CP_DMA_INLINK_PARK_S)
#define CP_DMA_INLINK_PARK_V  0x00000001
#define CP_DMA_INLINK_PARK_S  31

/** CP_DMA_OUT_EOF_DES_ADDR_REG register
 *  Outlink descriptor address when EOF occurs
 */
#define CP_DMA_OUT_EOF_DES_ADDR_REG (DR_REG_CP_BASE + 0x18)
/** CP_DMA_OUT_EOF_DES_ADDR : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define CP_DMA_OUT_EOF_DES_ADDR    0xFFFFFFFF
#define CP_DMA_OUT_EOF_DES_ADDR_M  (CP_DMA_OUT_EOF_DES_ADDR_V << CP_DMA_OUT_EOF_DES_ADDR_S)
#define CP_DMA_OUT_EOF_DES_ADDR_V  0xFFFFFFFF
#define CP_DMA_OUT_EOF_DES_ADDR_S  0

/** CP_DMA_IN_EOF_DES_ADDR_REG register
 *  Inlink descriptor address when EOF occurs
 */
#define CP_DMA_IN_EOF_DES_ADDR_REG (DR_REG_CP_BASE + 0x1c)
/** CP_DMA_IN_SUC_EOF_DES_ADDR : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when received successful
 *  EOF.
 */
#define CP_DMA_IN_SUC_EOF_DES_ADDR    0xFFFFFFFF
#define CP_DMA_IN_SUC_EOF_DES_ADDR_M  (CP_DMA_IN_SUC_EOF_DES_ADDR_V << CP_DMA_IN_SUC_EOF_DES_ADDR_S)
#define CP_DMA_IN_SUC_EOF_DES_ADDR_V  0xFFFFFFFF
#define CP_DMA_IN_SUC_EOF_DES_ADDR_S  0

/** CP_DMA_OUT_EOF_BFR_DES_ADDR_REG register
 *  Outlink descriptor address before the last outlink descriptor
 */
#define CP_DMA_OUT_EOF_BFR_DES_ADDR_REG (DR_REG_CP_BASE + 0x20)
/** CP_DMA_OUT_EOF_BFR_DES_ADDR : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor before the last outlink
 *  descriptor.
 */
#define CP_DMA_OUT_EOF_BFR_DES_ADDR    0xFFFFFFFF
#define CP_DMA_OUT_EOF_BFR_DES_ADDR_M  (CP_DMA_OUT_EOF_BFR_DES_ADDR_V << CP_DMA_OUT_EOF_BFR_DES_ADDR_S)
#define CP_DMA_OUT_EOF_BFR_DES_ADDR_V  0xFFFFFFFF
#define CP_DMA_OUT_EOF_BFR_DES_ADDR_S  0

/** CP_DMA_INLINK_DSCR_REG register
 *  Address of current inlink descriptor
 */
#define CP_DMA_INLINK_DSCR_REG (DR_REG_CP_BASE + 0x24)
/** CP_DMA_INLINK_DSCR : RO; bitpos: [31:0]; default: 0;
 *  The address of the current inlink descriptor x.
 */
#define CP_DMA_INLINK_DSCR    0xFFFFFFFF
#define CP_DMA_INLINK_DSCR_M  (CP_DMA_INLINK_DSCR_V << CP_DMA_INLINK_DSCR_S)
#define CP_DMA_INLINK_DSCR_V  0xFFFFFFFF
#define CP_DMA_INLINK_DSCR_S  0

/** CP_DMA_INLINK_DSCR_BF0_REG register
 *  Address of last inlink descriptor
 */
#define CP_DMA_INLINK_DSCR_BF0_REG (DR_REG_CP_BASE + 0x28)
/** CP_DMA_INLINK_DSCR_BF0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last inlink descriptor x-1.
 */
#define CP_DMA_INLINK_DSCR_BF0    0xFFFFFFFF
#define CP_DMA_INLINK_DSCR_BF0_M  (CP_DMA_INLINK_DSCR_BF0_V << CP_DMA_INLINK_DSCR_BF0_S)
#define CP_DMA_INLINK_DSCR_BF0_V  0xFFFFFFFF
#define CP_DMA_INLINK_DSCR_BF0_S  0

/** CP_DMA_INLINK_DSCR_BF1_REG register
 *  Address of the second-to-last inlink descriptor
 */
#define CP_DMA_INLINK_DSCR_BF1_REG (DR_REG_CP_BASE + 0x2c)
/** CP_DMA_INLINK_DSCR_BF1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor x-2.
 */
#define CP_DMA_INLINK_DSCR_BF1    0xFFFFFFFF
#define CP_DMA_INLINK_DSCR_BF1_M  (CP_DMA_INLINK_DSCR_BF1_V << CP_DMA_INLINK_DSCR_BF1_S)
#define CP_DMA_INLINK_DSCR_BF1_V  0xFFFFFFFF
#define CP_DMA_INLINK_DSCR_BF1_S  0

/** CP_DMA_OUTLINK_DSCR_REG register
 *  Address of current outlink descriptor
 */
#define CP_DMA_OUTLINK_DSCR_REG (DR_REG_CP_BASE + 0x30)
/** CP_DMA_OUTLINK_DSCR : RO; bitpos: [31:0]; default: 0;
 *  The address of the current outlink descriptor y.
 */
#define CP_DMA_OUTLINK_DSCR    0xFFFFFFFF
#define CP_DMA_OUTLINK_DSCR_M  (CP_DMA_OUTLINK_DSCR_V << CP_DMA_OUTLINK_DSCR_S)
#define CP_DMA_OUTLINK_DSCR_V  0xFFFFFFFF
#define CP_DMA_OUTLINK_DSCR_S  0

/** CP_DMA_OUTLINK_DSCR_BF0_REG register
 *  Address of last outlink descriptor
 */
#define CP_DMA_OUTLINK_DSCR_BF0_REG (DR_REG_CP_BASE + 0x34)
/** CP_DMA_OUTLINK_DSCR_BF0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last outlink descriptor y-1.
 */
#define CP_DMA_OUTLINK_DSCR_BF0    0xFFFFFFFF
#define CP_DMA_OUTLINK_DSCR_BF0_M  (CP_DMA_OUTLINK_DSCR_BF0_V << CP_DMA_OUTLINK_DSCR_BF0_S)
#define CP_DMA_OUTLINK_DSCR_BF0_V  0xFFFFFFFF
#define CP_DMA_OUTLINK_DSCR_BF0_S  0

/** CP_DMA_OUTLINK_DSCR_BF1_REG register
 *  Address of the second-to-last outlink descriptor
 */
#define CP_DMA_OUTLINK_DSCR_BF1_REG (DR_REG_CP_BASE + 0x38)
/** CP_DMA_OUTLINK_DSCR_BF1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last outlink descriptor y-2.
 */
#define CP_DMA_OUTLINK_DSCR_BF1    0xFFFFFFFF
#define CP_DMA_OUTLINK_DSCR_BF1_M  (CP_DMA_OUTLINK_DSCR_BF1_V << CP_DMA_OUTLINK_DSCR_BF1_S)
#define CP_DMA_OUTLINK_DSCR_BF1_V  0xFFFFFFFF
#define CP_DMA_OUTLINK_DSCR_BF1_S  0

/** CP_DMA_CONF_REG register
 *  Copy DMA configuration register
 */
#define CP_DMA_CONF_REG (DR_REG_CP_BASE + 0x3c)
/** CP_DMA_IN_RST : R/W; bitpos: [0]; default: 0;
 *  Set this bit to reset in_inf state machine.
 */
#define CP_DMA_IN_RST    (BIT(0))
#define CP_DMA_IN_RST_M  (CP_DMA_IN_RST_V << CP_DMA_IN_RST_S)
#define CP_DMA_IN_RST_V  0x00000001
#define CP_DMA_IN_RST_S  0
/** CP_DMA_OUT_RST : R/W; bitpos: [1]; default: 0;
 *  Set this bit to reset out_inf state machine.
 */
#define CP_DMA_OUT_RST    (BIT(1))
#define CP_DMA_OUT_RST_M  (CP_DMA_OUT_RST_V << CP_DMA_OUT_RST_S)
#define CP_DMA_OUT_RST_V  0x00000001
#define CP_DMA_OUT_RST_S  1
/** CP_DMA_CMDFIFO_RST : R/W; bitpos: [2]; default: 0;
 *  Set this bit to reset in_cmd fifo and out_cmd fifo.
 */
#define CP_DMA_CMDFIFO_RST    (BIT(2))
#define CP_DMA_CMDFIFO_RST_M  (CP_DMA_CMDFIFO_RST_V << CP_DMA_CMDFIFO_RST_S)
#define CP_DMA_CMDFIFO_RST_V  0x00000001
#define CP_DMA_CMDFIFO_RST_S  2
/** CP_DMA_FIFO_RST : R/W; bitpos: [3]; default: 0;
 *  Set this bit to reset data in receive FIFO.
 */
#define CP_DMA_FIFO_RST    (BIT(3))
#define CP_DMA_FIFO_RST_M  (CP_DMA_FIFO_RST_V << CP_DMA_FIFO_RST_S)
#define CP_DMA_FIFO_RST_V  0x00000001
#define CP_DMA_FIFO_RST_S  3
/** CP_DMA_OUT_OWNER : R/W; bitpos: [4]; default: 0;
 *  This is used to configure the owner bit in OUT descriptor. This is effective only
 *  when you set reg_out_auto_wrback.
 */
#define CP_DMA_OUT_OWNER    (BIT(4))
#define CP_DMA_OUT_OWNER_M  (CP_DMA_OUT_OWNER_V << CP_DMA_OUT_OWNER_S)
#define CP_DMA_OUT_OWNER_V  0x00000001
#define CP_DMA_OUT_OWNER_S  4
/** CP_DMA_IN_OWNER : R/W; bitpos: [5]; default: 0;
 *  This is used to configure the owner bit in IN descriptor.
 */
#define CP_DMA_IN_OWNER    (BIT(5))
#define CP_DMA_IN_OWNER_M  (CP_DMA_IN_OWNER_V << CP_DMA_IN_OWNER_S)
#define CP_DMA_IN_OWNER_V  0x00000001
#define CP_DMA_IN_OWNER_S  5
/** CP_DMA_OUT_AUTO_WRBACK : R/W; bitpos: [6]; default: 0;
 *  This bit is used to write back out descriptor when hardware has already used this
 *  descriptor.
 */
#define CP_DMA_OUT_AUTO_WRBACK    (BIT(6))
#define CP_DMA_OUT_AUTO_WRBACK_M  (CP_DMA_OUT_AUTO_WRBACK_V << CP_DMA_OUT_AUTO_WRBACK_S)
#define CP_DMA_OUT_AUTO_WRBACK_V  0x00000001
#define CP_DMA_OUT_AUTO_WRBACK_S  6
/** CP_DMA_CHECK_OWNER : R/W; bitpos: [7]; default: 0;
 *  Set this bit to enable owner bit check in descriptor.
 */
#define CP_DMA_CHECK_OWNER    (BIT(7))
#define CP_DMA_CHECK_OWNER_M  (CP_DMA_CHECK_OWNER_V << CP_DMA_CHECK_OWNER_S)
#define CP_DMA_CHECK_OWNER_V  0x00000001
#define CP_DMA_CHECK_OWNER_S  7
/** CP_DMA_CRC_CAL_RESET : R/W; bitpos: [8]; default: 0;
 *  Set this bit to reset crc calculation.
 */
#define CP_DMA_CRC_CAL_RESET    (BIT(8))
#define CP_DMA_CRC_CAL_RESET_M  (CP_DMA_CRC_CAL_RESET_V << CP_DMA_CRC_CAL_RESET_S)
#define CP_DMA_CRC_CAL_RESET_V  0x00000001
#define CP_DMA_CRC_CAL_RESET_S  8
/** CP_DMA_CRC_CAL_EN : R/W; bitpos: [9]; default: 0;
 *  Set this bit enable crc calculation function.
 */
#define CP_DMA_CRC_CAL_EN    (BIT(9))
#define CP_DMA_CRC_CAL_EN_M  (CP_DMA_CRC_CAL_EN_V << CP_DMA_CRC_CAL_EN_S)
#define CP_DMA_CRC_CAL_EN_V  0x00000001
#define CP_DMA_CRC_CAL_EN_S  9
/** CP_DMA_CRC_BIG_ENDIAN_EN : R/W; bitpos: [10]; default: 0;
 *  Set this bit to reorder the bit of data which will be send to excute crc.
 */
#define CP_DMA_CRC_BIG_ENDIAN_EN    (BIT(10))
#define CP_DMA_CRC_BIG_ENDIAN_EN_M  (CP_DMA_CRC_BIG_ENDIAN_EN_V << CP_DMA_CRC_BIG_ENDIAN_EN_S)
#define CP_DMA_CRC_BIG_ENDIAN_EN_V  0x00000001
#define CP_DMA_CRC_BIG_ENDIAN_EN_S  10
/** CP_DMA_CRC_OUT_REVERSE_EN : R/W; bitpos: [11]; default: 0;
 *  Set this bit to reverse the crc calculation result.
 */
#define CP_DMA_CRC_OUT_REVERSE_EN    (BIT(11))
#define CP_DMA_CRC_OUT_REVERSE_EN_M  (CP_DMA_CRC_OUT_REVERSE_EN_V << CP_DMA_CRC_OUT_REVERSE_EN_S)
#define CP_DMA_CRC_OUT_REVERSE_EN_V  0x00000001
#define CP_DMA_CRC_OUT_REVERSE_EN_S  11
/** CP_DMA_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  1'b1: Force clock on for register. 1'b0: Support clock only when application writes
 *  registers.
 */
#define CP_DMA_CLK_EN    (BIT(31))
#define CP_DMA_CLK_EN_M  (CP_DMA_CLK_EN_V << CP_DMA_CLK_EN_S)
#define CP_DMA_CLK_EN_V  0x00000001
#define CP_DMA_CLK_EN_S  31

/** CP_DMA_IN_ST_REG register
 *  Status register of receiving data
 */
#define CP_DMA_IN_ST_REG (DR_REG_CP_BASE + 0x40)
/** CP_DMA_INLINK_DSCR_ADDR : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current inlink descriptor's address.
 */
#define CP_DMA_INLINK_DSCR_ADDR    0x0003FFFF
#define CP_DMA_INLINK_DSCR_ADDR_M  (CP_DMA_INLINK_DSCR_ADDR_V << CP_DMA_INLINK_DSCR_ADDR_S)
#define CP_DMA_INLINK_DSCR_ADDR_V  0x0003FFFF
#define CP_DMA_INLINK_DSCR_ADDR_S  0
/** CP_DMA_IN_DSCR_STATE : RO; bitpos: [19:18]; default: 0;
 *  Reserved.
 */
#define CP_DMA_IN_DSCR_STATE    0x00000003
#define CP_DMA_IN_DSCR_STATE_M  (CP_DMA_IN_DSCR_STATE_V << CP_DMA_IN_DSCR_STATE_S)
#define CP_DMA_IN_DSCR_STATE_V  0x00000003
#define CP_DMA_IN_DSCR_STATE_S  18
/** CP_DMA_IN_STATE : RO; bitpos: [22:20]; default: 0;
 *  Reserved.
 */
#define CP_DMA_IN_STATE    0x00000007
#define CP_DMA_IN_STATE_M  (CP_DMA_IN_STATE_V << CP_DMA_IN_STATE_S)
#define CP_DMA_IN_STATE_V  0x00000007
#define CP_DMA_IN_STATE_S  20
/** CP_DMA_FIFO_EMPTY : RO; bitpos: [23]; default: 0;
 *  Copy DMA FIFO empty signal.
 */
#define CP_DMA_FIFO_EMPTY    (BIT(23))
#define CP_DMA_FIFO_EMPTY_M  (CP_DMA_FIFO_EMPTY_V << CP_DMA_FIFO_EMPTY_S)
#define CP_DMA_FIFO_EMPTY_V  0x00000001
#define CP_DMA_FIFO_EMPTY_S  23

/** CP_DMA_OUT_ST_REG register
 *  Status register of trasmitting data
 */
#define CP_DMA_OUT_ST_REG (DR_REG_CP_BASE + 0x44)
/** CP_DMA_OUTLINK_DSCR_ADDR : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current outlink descriptor's address.
 */
#define CP_DMA_OUTLINK_DSCR_ADDR    0x0003FFFF
#define CP_DMA_OUTLINK_DSCR_ADDR_M  (CP_DMA_OUTLINK_DSCR_ADDR_V << CP_DMA_OUTLINK_DSCR_ADDR_S)
#define CP_DMA_OUTLINK_DSCR_ADDR_V  0x0003FFFF
#define CP_DMA_OUTLINK_DSCR_ADDR_S  0
/** CP_DMA_OUT_DSCR_STATE : RO; bitpos: [19:18]; default: 0;
 *  Reserved.
 */
#define CP_DMA_OUT_DSCR_STATE    0x00000003
#define CP_DMA_OUT_DSCR_STATE_M  (CP_DMA_OUT_DSCR_STATE_V << CP_DMA_OUT_DSCR_STATE_S)
#define CP_DMA_OUT_DSCR_STATE_V  0x00000003
#define CP_DMA_OUT_DSCR_STATE_S  18
/** CP_DMA_OUT_STATE : RO; bitpos: [22:20]; default: 0;
 *  Reserved.
 */
#define CP_DMA_OUT_STATE    0x00000007
#define CP_DMA_OUT_STATE_M  (CP_DMA_OUT_STATE_V << CP_DMA_OUT_STATE_S)
#define CP_DMA_OUT_STATE_V  0x00000007
#define CP_DMA_OUT_STATE_S  20
/** CP_DMA_FIFO_FULL : RO; bitpos: [23]; default: 0;
 *  Copy DMA FIFO full signal.
 */
#define CP_DMA_FIFO_FULL    (BIT(23))
#define CP_DMA_FIFO_FULL_M  (CP_DMA_FIFO_FULL_V << CP_DMA_FIFO_FULL_S)
#define CP_DMA_FIFO_FULL_V  0x00000001
#define CP_DMA_FIFO_FULL_S  23

/** CP_DMA_CRC_OUT_REG register
 *  CRC result register
 */
#define CP_DMA_CRC_OUT_REG (DR_REG_CP_BASE + 0x48)
/** CP_DMA_CRC_RESULT : RO; bitpos: [31:0]; default: 0;
 *  This register stores the result of CRC.
 */
#define CP_DMA_CRC_RESULT    0xFFFFFFFF
#define CP_DMA_CRC_RESULT_M  (CP_DMA_CRC_RESULT_V << CP_DMA_CRC_RESULT_S)
#define CP_DMA_CRC_RESULT_V  0xFFFFFFFF
#define CP_DMA_CRC_RESULT_S  0

/** CP_DMA_DATE_REG register
 *  Copy DMA version register
 */
#define CP_DMA_DATE_REG (DR_REG_CP_BASE + 0xfc)
/** CP_DMA_DMA_DATE : R/W; bitpos: [31:0]; default: 403185664;
 *  This is the version register.
 */
#define CP_DMA_DMA_DATE    0xFFFFFFFF
#define CP_DMA_DMA_DATE_M  (CP_DMA_DMA_DATE_V << CP_DMA_DMA_DATE_S)
#define CP_DMA_DMA_DATE_V  0xFFFFFFFF
#define CP_DMA_DMA_DATE_S  0

#ifdef __cplusplus
}
#endif
