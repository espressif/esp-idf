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

/** BITSCRAMBLER_TX_INST_CFG0_REG register
 *  Control and configuration registers
 */
#define BITSCRAMBLER_TX_INST_CFG0_REG (DR_REG_BITSCRAMBLER_BASE + 0x0)
/** BITSCRAMBLER_TX_INST_IDX : R/W; bitpos: [2:0]; default: 0;
 *  write this bits to specify the one of 8 instruction
 */
#define BITSCRAMBLER_TX_INST_IDX    0x00000007U
#define BITSCRAMBLER_TX_INST_IDX_M  (BITSCRAMBLER_TX_INST_IDX_V << BITSCRAMBLER_TX_INST_IDX_S)
#define BITSCRAMBLER_TX_INST_IDX_V  0x00000007U
#define BITSCRAMBLER_TX_INST_IDX_S  0
/** BITSCRAMBLER_TX_INST_POS : R/W; bitpos: [6:3]; default: 0;
 *  write this bits to specify the bit position of 257 bit instruction which in units
 *  of 32 bits
 */
#define BITSCRAMBLER_TX_INST_POS    0x0000000FU
#define BITSCRAMBLER_TX_INST_POS_M  (BITSCRAMBLER_TX_INST_POS_V << BITSCRAMBLER_TX_INST_POS_S)
#define BITSCRAMBLER_TX_INST_POS_V  0x0000000FU
#define BITSCRAMBLER_TX_INST_POS_S  3

/** BITSCRAMBLER_TX_INST_CFG1_REG register
 *  Control and configuration registers
 */
#define BITSCRAMBLER_TX_INST_CFG1_REG (DR_REG_BITSCRAMBLER_BASE + 0x4)
/** BITSCRAMBLER_TX_INST : R/W; bitpos: [31:0]; default: 4;
 *  write this bits to update instruction which specified by
 *  BITSCRAMBLER_TX_INST_CFG0_REG, Read this bits to get instruction which specified by
 *  BITSCRAMBLER_TX_INST_CFG0_REG
 */
#define BITSCRAMBLER_TX_INST    0xFFFFFFFFU
#define BITSCRAMBLER_TX_INST_M  (BITSCRAMBLER_TX_INST_V << BITSCRAMBLER_TX_INST_S)
#define BITSCRAMBLER_TX_INST_V  0xFFFFFFFFU
#define BITSCRAMBLER_TX_INST_S  0

/** BITSCRAMBLER_RX_INST_CFG0_REG register
 *  Control and configuration registers
 */
#define BITSCRAMBLER_RX_INST_CFG0_REG (DR_REG_BITSCRAMBLER_BASE + 0x8)
/** BITSCRAMBLER_RX_INST_IDX : R/W; bitpos: [2:0]; default: 0;
 *  write this bits to specify the one of 8 instruction
 */
#define BITSCRAMBLER_RX_INST_IDX    0x00000007U
#define BITSCRAMBLER_RX_INST_IDX_M  (BITSCRAMBLER_RX_INST_IDX_V << BITSCRAMBLER_RX_INST_IDX_S)
#define BITSCRAMBLER_RX_INST_IDX_V  0x00000007U
#define BITSCRAMBLER_RX_INST_IDX_S  0
/** BITSCRAMBLER_RX_INST_POS : R/W; bitpos: [6:3]; default: 0;
 *  write this bits to specify the bit position of 257 bit instruction which in units
 *  of 32 bits
 */
#define BITSCRAMBLER_RX_INST_POS    0x0000000FU
#define BITSCRAMBLER_RX_INST_POS_M  (BITSCRAMBLER_RX_INST_POS_V << BITSCRAMBLER_RX_INST_POS_S)
#define BITSCRAMBLER_RX_INST_POS_V  0x0000000FU
#define BITSCRAMBLER_RX_INST_POS_S  3

/** BITSCRAMBLER_RX_INST_CFG1_REG register
 *  Control and configuration registers
 */
#define BITSCRAMBLER_RX_INST_CFG1_REG (DR_REG_BITSCRAMBLER_BASE + 0xc)
/** BITSCRAMBLER_RX_INST : R/W; bitpos: [31:0]; default: 12;
 *  write this bits to update instruction which specified by
 *  BITSCRAMBLER_RX_INST_CFG0_REG, Read this bits to get instruction which specified by
 *  BITSCRAMBLER_RX_INST_CFG0_REG
 */
#define BITSCRAMBLER_RX_INST    0xFFFFFFFFU
#define BITSCRAMBLER_RX_INST_M  (BITSCRAMBLER_RX_INST_V << BITSCRAMBLER_RX_INST_S)
#define BITSCRAMBLER_RX_INST_V  0xFFFFFFFFU
#define BITSCRAMBLER_RX_INST_S  0

/** BITSCRAMBLER_TX_LUT_CFG0_REG register
 *  Control and configuration registers
 */
#define BITSCRAMBLER_TX_LUT_CFG0_REG (DR_REG_BITSCRAMBLER_BASE + 0x10)
/** BITSCRAMBLER_TX_LUT_IDX : R/W; bitpos: [10:0]; default: 0;
 *  write this bits to specify the bytes position of LUT RAM based on
 *  reg_bitscrambler_tx_lut_mode
 */
#define BITSCRAMBLER_TX_LUT_IDX    0x000007FFU
#define BITSCRAMBLER_TX_LUT_IDX_M  (BITSCRAMBLER_TX_LUT_IDX_V << BITSCRAMBLER_TX_LUT_IDX_S)
#define BITSCRAMBLER_TX_LUT_IDX_V  0x000007FFU
#define BITSCRAMBLER_TX_LUT_IDX_S  0
/** BITSCRAMBLER_TX_LUT_MODE : R/W; bitpos: [12:11]; default: 0;
 *  write this bits to specify the bytes mode of LUT RAM, 0: 1 byte,1: 2bytes, 2: 4
 *  bytes
 */
#define BITSCRAMBLER_TX_LUT_MODE    0x00000003U
#define BITSCRAMBLER_TX_LUT_MODE_M  (BITSCRAMBLER_TX_LUT_MODE_V << BITSCRAMBLER_TX_LUT_MODE_S)
#define BITSCRAMBLER_TX_LUT_MODE_V  0x00000003U
#define BITSCRAMBLER_TX_LUT_MODE_S  11

/** BITSCRAMBLER_TX_LUT_CFG1_REG register
 *  Control and configuration registers
 */
#define BITSCRAMBLER_TX_LUT_CFG1_REG (DR_REG_BITSCRAMBLER_BASE + 0x14)
/** BITSCRAMBLER_TX_LUT : R/W; bitpos: [31:0]; default: 20;
 *  write this bits to update LUT which specified by BITSCRAMBLER_TX_LUT_CFG0_REG, Read
 *  this bits to get LUT which specified by BITSCRAMBLER_TX_LUT_CFG0_REG
 */
#define BITSCRAMBLER_TX_LUT    0xFFFFFFFFU
#define BITSCRAMBLER_TX_LUT_M  (BITSCRAMBLER_TX_LUT_V << BITSCRAMBLER_TX_LUT_S)
#define BITSCRAMBLER_TX_LUT_V  0xFFFFFFFFU
#define BITSCRAMBLER_TX_LUT_S  0

/** BITSCRAMBLER_RX_LUT_CFG0_REG register
 *  Control and configuration registers
 */
#define BITSCRAMBLER_RX_LUT_CFG0_REG (DR_REG_BITSCRAMBLER_BASE + 0x18)
/** BITSCRAMBLER_RX_LUT_IDX : R/W; bitpos: [10:0]; default: 0;
 *  write this bits to specify the bytes position of LUT RAM based on
 *  reg_bitscrambler_rx_lut_mode
 */
#define BITSCRAMBLER_RX_LUT_IDX    0x000007FFU
#define BITSCRAMBLER_RX_LUT_IDX_M  (BITSCRAMBLER_RX_LUT_IDX_V << BITSCRAMBLER_RX_LUT_IDX_S)
#define BITSCRAMBLER_RX_LUT_IDX_V  0x000007FFU
#define BITSCRAMBLER_RX_LUT_IDX_S  0
/** BITSCRAMBLER_RX_LUT_MODE : R/W; bitpos: [12:11]; default: 0;
 *  write this bits to specify the bytes mode of LUT RAM, 0: 1 byte,1: 2bytes, 2: 4
 *  bytes
 */
#define BITSCRAMBLER_RX_LUT_MODE    0x00000003U
#define BITSCRAMBLER_RX_LUT_MODE_M  (BITSCRAMBLER_RX_LUT_MODE_V << BITSCRAMBLER_RX_LUT_MODE_S)
#define BITSCRAMBLER_RX_LUT_MODE_V  0x00000003U
#define BITSCRAMBLER_RX_LUT_MODE_S  11

/** BITSCRAMBLER_RX_LUT_CFG1_REG register
 *  Control and configuration registers
 */
#define BITSCRAMBLER_RX_LUT_CFG1_REG (DR_REG_BITSCRAMBLER_BASE + 0x1c)
/** BITSCRAMBLER_RX_LUT : R/W; bitpos: [31:0]; default: 28;
 *  write this bits to update LUT which specified by BITSCRAMBLER_RX_LUT_CFG0_REG, Read
 *  this bits to get LUT which specified by BITSCRAMBLER_RX_LUT_CFG0_REG
 */
#define BITSCRAMBLER_RX_LUT    0xFFFFFFFFU
#define BITSCRAMBLER_RX_LUT_M  (BITSCRAMBLER_RX_LUT_V << BITSCRAMBLER_RX_LUT_S)
#define BITSCRAMBLER_RX_LUT_V  0xFFFFFFFFU
#define BITSCRAMBLER_RX_LUT_S  0

/** BITSCRAMBLER_TX_TAILING_BITS_REG register
 *  Control and configuration registers
 */
#define BITSCRAMBLER_TX_TAILING_BITS_REG (DR_REG_BITSCRAMBLER_BASE + 0x20)
/** BITSCRAMBLER_TX_TAILING_BITS : R/W; bitpos: [15:0]; default: 0;
 *  write this bits to specify the extra data bit length after getting EOF
 */
#define BITSCRAMBLER_TX_TAILING_BITS    0x0000FFFFU
#define BITSCRAMBLER_TX_TAILING_BITS_M  (BITSCRAMBLER_TX_TAILING_BITS_V << BITSCRAMBLER_TX_TAILING_BITS_S)
#define BITSCRAMBLER_TX_TAILING_BITS_V  0x0000FFFFU
#define BITSCRAMBLER_TX_TAILING_BITS_S  0

/** BITSCRAMBLER_RX_TAILING_BITS_REG register
 *  Control and configuration registers
 */
#define BITSCRAMBLER_RX_TAILING_BITS_REG (DR_REG_BITSCRAMBLER_BASE + 0x24)
/** BITSCRAMBLER_RX_TAILING_BITS : R/W; bitpos: [15:0]; default: 0;
 *  write this bits to specify the extra data bit length after getting EOF
 */
#define BITSCRAMBLER_RX_TAILING_BITS    0x0000FFFFU
#define BITSCRAMBLER_RX_TAILING_BITS_M  (BITSCRAMBLER_RX_TAILING_BITS_V << BITSCRAMBLER_RX_TAILING_BITS_S)
#define BITSCRAMBLER_RX_TAILING_BITS_V  0x0000FFFFU
#define BITSCRAMBLER_RX_TAILING_BITS_S  0

/** BITSCRAMBLER_TX_CTRL_REG register
 *  Control and configuration registers
 */
#define BITSCRAMBLER_TX_CTRL_REG (DR_REG_BITSCRAMBLER_BASE + 0x28)
/** BITSCRAMBLER_TX_ENA : R/W; bitpos: [0]; default: 0;
 *  write this bit to enable the bitscrambler tx
 */
#define BITSCRAMBLER_TX_ENA    (BIT(0))
#define BITSCRAMBLER_TX_ENA_M  (BITSCRAMBLER_TX_ENA_V << BITSCRAMBLER_TX_ENA_S)
#define BITSCRAMBLER_TX_ENA_V  0x00000001U
#define BITSCRAMBLER_TX_ENA_S  0
/** BITSCRAMBLER_TX_PAUSE : R/W; bitpos: [1]; default: 0;
 *  write this bit to pause the bitscrambler tx core
 */
#define BITSCRAMBLER_TX_PAUSE    (BIT(1))
#define BITSCRAMBLER_TX_PAUSE_M  (BITSCRAMBLER_TX_PAUSE_V << BITSCRAMBLER_TX_PAUSE_S)
#define BITSCRAMBLER_TX_PAUSE_V  0x00000001U
#define BITSCRAMBLER_TX_PAUSE_S  1
/** BITSCRAMBLER_TX_HALT : R/W; bitpos: [2]; default: 1;
 *  write this bit to halt the bitscrambler tx core
 */
#define BITSCRAMBLER_TX_HALT    (BIT(2))
#define BITSCRAMBLER_TX_HALT_M  (BITSCRAMBLER_TX_HALT_V << BITSCRAMBLER_TX_HALT_S)
#define BITSCRAMBLER_TX_HALT_V  0x00000001U
#define BITSCRAMBLER_TX_HALT_S  2
/** BITSCRAMBLER_TX_EOF_MODE : R/W; bitpos: [3]; default: 0;
 *  write this bit to ser the bitscrambler tx core EOF signal generating mode which is
 *  combined with reg_bitscrambler_tx_tailing_bits, 0: counter by read dma fifo, 0
 *  counter by write peripheral buffer
 */
#define BITSCRAMBLER_TX_EOF_MODE    (BIT(3))
#define BITSCRAMBLER_TX_EOF_MODE_M  (BITSCRAMBLER_TX_EOF_MODE_V << BITSCRAMBLER_TX_EOF_MODE_S)
#define BITSCRAMBLER_TX_EOF_MODE_V  0x00000001U
#define BITSCRAMBLER_TX_EOF_MODE_S  3
/** BITSCRAMBLER_TX_COND_MODE : R/W; bitpos: [4]; default: 0;
 *  write this bit to specify the LOOP instruction condition mode of bitscrambler tx
 *  core, 0: use the little than operator to get the condition, 1: use not equal
 *  operator to get the condition
 */
#define BITSCRAMBLER_TX_COND_MODE    (BIT(4))
#define BITSCRAMBLER_TX_COND_MODE_M  (BITSCRAMBLER_TX_COND_MODE_V << BITSCRAMBLER_TX_COND_MODE_S)
#define BITSCRAMBLER_TX_COND_MODE_V  0x00000001U
#define BITSCRAMBLER_TX_COND_MODE_S  4
/** BITSCRAMBLER_TX_FETCH_MODE : R/W; bitpos: [5]; default: 0;
 *  write this bit to set the bitscrambler tx core fetch instruction mode, 0: prefetch
 *  by reset, 1: fetch by instrutions
 */
#define BITSCRAMBLER_TX_FETCH_MODE    (BIT(5))
#define BITSCRAMBLER_TX_FETCH_MODE_M  (BITSCRAMBLER_TX_FETCH_MODE_V << BITSCRAMBLER_TX_FETCH_MODE_S)
#define BITSCRAMBLER_TX_FETCH_MODE_V  0x00000001U
#define BITSCRAMBLER_TX_FETCH_MODE_S  5
/** BITSCRAMBLER_TX_HALT_MODE : R/W; bitpos: [6]; default: 0;
 *  write this bit to set the bitscrambler tx core halt mode when tx_halt is set, 0:
 *  wait write data back done, , 1: ignore write data back
 */
#define BITSCRAMBLER_TX_HALT_MODE    (BIT(6))
#define BITSCRAMBLER_TX_HALT_MODE_M  (BITSCRAMBLER_TX_HALT_MODE_V << BITSCRAMBLER_TX_HALT_MODE_S)
#define BITSCRAMBLER_TX_HALT_MODE_V  0x00000001U
#define BITSCRAMBLER_TX_HALT_MODE_S  6
/** BITSCRAMBLER_TX_RD_DUMMY : R/W; bitpos: [7]; default: 0;
 *  write this bit to set the bitscrambler tx core read data mode when EOF received.0:
 *  wait read data, 1: ignore read data
 */
#define BITSCRAMBLER_TX_RD_DUMMY    (BIT(7))
#define BITSCRAMBLER_TX_RD_DUMMY_M  (BITSCRAMBLER_TX_RD_DUMMY_V << BITSCRAMBLER_TX_RD_DUMMY_S)
#define BITSCRAMBLER_TX_RD_DUMMY_V  0x00000001U
#define BITSCRAMBLER_TX_RD_DUMMY_S  7
/** BITSCRAMBLER_TX_FIFO_RST : WT; bitpos: [8]; default: 0;
 *  write this bit to reset the bitscrambler tx fifo
 */
#define BITSCRAMBLER_TX_FIFO_RST    (BIT(8))
#define BITSCRAMBLER_TX_FIFO_RST_M  (BITSCRAMBLER_TX_FIFO_RST_V << BITSCRAMBLER_TX_FIFO_RST_S)
#define BITSCRAMBLER_TX_FIFO_RST_V  0x00000001U
#define BITSCRAMBLER_TX_FIFO_RST_S  8

/** BITSCRAMBLER_RX_CTRL_REG register
 *  Control and configuration registers
 */
#define BITSCRAMBLER_RX_CTRL_REG (DR_REG_BITSCRAMBLER_BASE + 0x2c)
/** BITSCRAMBLER_RX_ENA : R/W; bitpos: [0]; default: 0;
 *  write this bit to enable the bitscrambler rx
 */
#define BITSCRAMBLER_RX_ENA    (BIT(0))
#define BITSCRAMBLER_RX_ENA_M  (BITSCRAMBLER_RX_ENA_V << BITSCRAMBLER_RX_ENA_S)
#define BITSCRAMBLER_RX_ENA_V  0x00000001U
#define BITSCRAMBLER_RX_ENA_S  0
/** BITSCRAMBLER_RX_PAUSE : R/W; bitpos: [1]; default: 0;
 *  write this bit to pause the bitscrambler rx core
 */
#define BITSCRAMBLER_RX_PAUSE    (BIT(1))
#define BITSCRAMBLER_RX_PAUSE_M  (BITSCRAMBLER_RX_PAUSE_V << BITSCRAMBLER_RX_PAUSE_S)
#define BITSCRAMBLER_RX_PAUSE_V  0x00000001U
#define BITSCRAMBLER_RX_PAUSE_S  1
/** BITSCRAMBLER_RX_HALT : R/W; bitpos: [2]; default: 1;
 *  write this bit to halt the bitscrambler rx core
 */
#define BITSCRAMBLER_RX_HALT    (BIT(2))
#define BITSCRAMBLER_RX_HALT_M  (BITSCRAMBLER_RX_HALT_V << BITSCRAMBLER_RX_HALT_S)
#define BITSCRAMBLER_RX_HALT_V  0x00000001U
#define BITSCRAMBLER_RX_HALT_S  2
/** BITSCRAMBLER_RX_EOF_MODE : R/W; bitpos: [3]; default: 0;
 *  write this bit to ser the bitscrambler rx core EOF signal generating mode which is
 *  combined with reg_bitscrambler_rx_tailing_bits, 0: counter by read peripheral
 *  buffer, 0 counter by write dma fifo
 */
#define BITSCRAMBLER_RX_EOF_MODE    (BIT(3))
#define BITSCRAMBLER_RX_EOF_MODE_M  (BITSCRAMBLER_RX_EOF_MODE_V << BITSCRAMBLER_RX_EOF_MODE_S)
#define BITSCRAMBLER_RX_EOF_MODE_V  0x00000001U
#define BITSCRAMBLER_RX_EOF_MODE_S  3
/** BITSCRAMBLER_RX_COND_MODE : R/W; bitpos: [4]; default: 0;
 *  write this bit to specify the LOOP instruction condition mode of bitscrambler rx
 *  core, 0: use the little than operator to get the condition, 1: use not equal
 *  operator to get the condition
 */
#define BITSCRAMBLER_RX_COND_MODE    (BIT(4))
#define BITSCRAMBLER_RX_COND_MODE_M  (BITSCRAMBLER_RX_COND_MODE_V << BITSCRAMBLER_RX_COND_MODE_S)
#define BITSCRAMBLER_RX_COND_MODE_V  0x00000001U
#define BITSCRAMBLER_RX_COND_MODE_S  4
/** BITSCRAMBLER_RX_FETCH_MODE : R/W; bitpos: [5]; default: 0;
 *  write this bit to set the bitscrambler rx core fetch instruction mode, 0: prefetch
 *  by reset, 1: fetch by instrutions
 */
#define BITSCRAMBLER_RX_FETCH_MODE    (BIT(5))
#define BITSCRAMBLER_RX_FETCH_MODE_M  (BITSCRAMBLER_RX_FETCH_MODE_V << BITSCRAMBLER_RX_FETCH_MODE_S)
#define BITSCRAMBLER_RX_FETCH_MODE_V  0x00000001U
#define BITSCRAMBLER_RX_FETCH_MODE_S  5
/** BITSCRAMBLER_RX_HALT_MODE : R/W; bitpos: [6]; default: 0;
 *  write this bit to set the bitscrambler rx core halt mode when rx_halt is set, 0:
 *  wait write data back done, , 1: ignore write data back
 */
#define BITSCRAMBLER_RX_HALT_MODE    (BIT(6))
#define BITSCRAMBLER_RX_HALT_MODE_M  (BITSCRAMBLER_RX_HALT_MODE_V << BITSCRAMBLER_RX_HALT_MODE_S)
#define BITSCRAMBLER_RX_HALT_MODE_V  0x00000001U
#define BITSCRAMBLER_RX_HALT_MODE_S  6
/** BITSCRAMBLER_RX_RD_DUMMY : R/W; bitpos: [7]; default: 0;
 *  write this bit to set the bitscrambler rx core read data mode when EOF received.0:
 *  wait read data, 1: ignore read data
 */
#define BITSCRAMBLER_RX_RD_DUMMY    (BIT(7))
#define BITSCRAMBLER_RX_RD_DUMMY_M  (BITSCRAMBLER_RX_RD_DUMMY_V << BITSCRAMBLER_RX_RD_DUMMY_S)
#define BITSCRAMBLER_RX_RD_DUMMY_V  0x00000001U
#define BITSCRAMBLER_RX_RD_DUMMY_S  7
/** BITSCRAMBLER_RX_FIFO_RST : WT; bitpos: [8]; default: 0;
 *  write this bit to reset the bitscrambler rx fifo
 */
#define BITSCRAMBLER_RX_FIFO_RST    (BIT(8))
#define BITSCRAMBLER_RX_FIFO_RST_M  (BITSCRAMBLER_RX_FIFO_RST_V << BITSCRAMBLER_RX_FIFO_RST_S)
#define BITSCRAMBLER_RX_FIFO_RST_V  0x00000001U
#define BITSCRAMBLER_RX_FIFO_RST_S  8

/** BITSCRAMBLER_TX_STATE_REG register
 *  Status registers
 */
#define BITSCRAMBLER_TX_STATE_REG (DR_REG_BITSCRAMBLER_BASE + 0x30)
/** BITSCRAMBLER_TX_IN_IDLE : RO; bitpos: [0]; default: 1;
 *  represents the bitscrambler tx core in halt mode
 */
#define BITSCRAMBLER_TX_IN_IDLE    (BIT(0))
#define BITSCRAMBLER_TX_IN_IDLE_M  (BITSCRAMBLER_TX_IN_IDLE_V << BITSCRAMBLER_TX_IN_IDLE_S)
#define BITSCRAMBLER_TX_IN_IDLE_V  0x00000001U
#define BITSCRAMBLER_TX_IN_IDLE_S  0
/** BITSCRAMBLER_TX_IN_RUN : RO; bitpos: [1]; default: 0;
 *  represents the bitscrambler tx core in run mode
 */
#define BITSCRAMBLER_TX_IN_RUN    (BIT(1))
#define BITSCRAMBLER_TX_IN_RUN_M  (BITSCRAMBLER_TX_IN_RUN_V << BITSCRAMBLER_TX_IN_RUN_S)
#define BITSCRAMBLER_TX_IN_RUN_V  0x00000001U
#define BITSCRAMBLER_TX_IN_RUN_S  1
/** BITSCRAMBLER_TX_IN_WAIT : RO; bitpos: [2]; default: 0;
 *  represents the bitscrambler tx core in wait mode to wait write back done
 */
#define BITSCRAMBLER_TX_IN_WAIT    (BIT(2))
#define BITSCRAMBLER_TX_IN_WAIT_M  (BITSCRAMBLER_TX_IN_WAIT_V << BITSCRAMBLER_TX_IN_WAIT_S)
#define BITSCRAMBLER_TX_IN_WAIT_V  0x00000001U
#define BITSCRAMBLER_TX_IN_WAIT_S  2
/** BITSCRAMBLER_TX_IN_PAUSE : RO; bitpos: [3]; default: 0;
 *  represents the bitscrambler tx core in pause mode
 */
#define BITSCRAMBLER_TX_IN_PAUSE    (BIT(3))
#define BITSCRAMBLER_TX_IN_PAUSE_M  (BITSCRAMBLER_TX_IN_PAUSE_V << BITSCRAMBLER_TX_IN_PAUSE_S)
#define BITSCRAMBLER_TX_IN_PAUSE_V  0x00000001U
#define BITSCRAMBLER_TX_IN_PAUSE_S  3
/** BITSCRAMBLER_TX_FIFO_EMPTY : RO; bitpos: [4]; default: 0;
 *  represents the bitscrambler tx fifo in empty state
 */
#define BITSCRAMBLER_TX_FIFO_EMPTY    (BIT(4))
#define BITSCRAMBLER_TX_FIFO_EMPTY_M  (BITSCRAMBLER_TX_FIFO_EMPTY_V << BITSCRAMBLER_TX_FIFO_EMPTY_S)
#define BITSCRAMBLER_TX_FIFO_EMPTY_V  0x00000001U
#define BITSCRAMBLER_TX_FIFO_EMPTY_S  4
/** BITSCRAMBLER_TX_EOF_GET_CNT : RO; bitpos: [29:16]; default: 0;
 *  represents the bytes numbers of bitscrambler tx core when get EOF
 */
#define BITSCRAMBLER_TX_EOF_GET_CNT    0x00003FFFU
#define BITSCRAMBLER_TX_EOF_GET_CNT_M  (BITSCRAMBLER_TX_EOF_GET_CNT_V << BITSCRAMBLER_TX_EOF_GET_CNT_S)
#define BITSCRAMBLER_TX_EOF_GET_CNT_V  0x00003FFFU
#define BITSCRAMBLER_TX_EOF_GET_CNT_S  16
/** BITSCRAMBLER_TX_EOF_OVERLOAD : RO; bitpos: [30]; default: 0;
 *  represents the some EOFs will be lost for bitscrambler tx core
 */
#define BITSCRAMBLER_TX_EOF_OVERLOAD    (BIT(30))
#define BITSCRAMBLER_TX_EOF_OVERLOAD_M  (BITSCRAMBLER_TX_EOF_OVERLOAD_V << BITSCRAMBLER_TX_EOF_OVERLOAD_S)
#define BITSCRAMBLER_TX_EOF_OVERLOAD_V  0x00000001U
#define BITSCRAMBLER_TX_EOF_OVERLOAD_S  30
/** BITSCRAMBLER_TX_EOF_TRACE_CLR : WT; bitpos: [31]; default: 0;
 *  write this bit to clear reg_bitscrambler_tx_eof_overload and
 *  reg_bitscrambler_tx_eof_get_cnt registers
 */
#define BITSCRAMBLER_TX_EOF_TRACE_CLR    (BIT(31))
#define BITSCRAMBLER_TX_EOF_TRACE_CLR_M  (BITSCRAMBLER_TX_EOF_TRACE_CLR_V << BITSCRAMBLER_TX_EOF_TRACE_CLR_S)
#define BITSCRAMBLER_TX_EOF_TRACE_CLR_V  0x00000001U
#define BITSCRAMBLER_TX_EOF_TRACE_CLR_S  31

/** BITSCRAMBLER_RX_STATE_REG register
 *  Status registers
 */
#define BITSCRAMBLER_RX_STATE_REG (DR_REG_BITSCRAMBLER_BASE + 0x34)
/** BITSCRAMBLER_RX_IN_IDLE : RO; bitpos: [0]; default: 1;
 *  represents the bitscrambler rx core in halt mode
 */
#define BITSCRAMBLER_RX_IN_IDLE    (BIT(0))
#define BITSCRAMBLER_RX_IN_IDLE_M  (BITSCRAMBLER_RX_IN_IDLE_V << BITSCRAMBLER_RX_IN_IDLE_S)
#define BITSCRAMBLER_RX_IN_IDLE_V  0x00000001U
#define BITSCRAMBLER_RX_IN_IDLE_S  0
/** BITSCRAMBLER_RX_IN_RUN : RO; bitpos: [1]; default: 0;
 *  represents the bitscrambler rx core in run mode
 */
#define BITSCRAMBLER_RX_IN_RUN    (BIT(1))
#define BITSCRAMBLER_RX_IN_RUN_M  (BITSCRAMBLER_RX_IN_RUN_V << BITSCRAMBLER_RX_IN_RUN_S)
#define BITSCRAMBLER_RX_IN_RUN_V  0x00000001U
#define BITSCRAMBLER_RX_IN_RUN_S  1
/** BITSCRAMBLER_RX_IN_WAIT : RO; bitpos: [2]; default: 0;
 *  represents the bitscrambler rx core in wait mode to wait write back done
 */
#define BITSCRAMBLER_RX_IN_WAIT    (BIT(2))
#define BITSCRAMBLER_RX_IN_WAIT_M  (BITSCRAMBLER_RX_IN_WAIT_V << BITSCRAMBLER_RX_IN_WAIT_S)
#define BITSCRAMBLER_RX_IN_WAIT_V  0x00000001U
#define BITSCRAMBLER_RX_IN_WAIT_S  2
/** BITSCRAMBLER_RX_IN_PAUSE : RO; bitpos: [3]; default: 0;
 *  represents the bitscrambler rx core in pause mode
 */
#define BITSCRAMBLER_RX_IN_PAUSE    (BIT(3))
#define BITSCRAMBLER_RX_IN_PAUSE_M  (BITSCRAMBLER_RX_IN_PAUSE_V << BITSCRAMBLER_RX_IN_PAUSE_S)
#define BITSCRAMBLER_RX_IN_PAUSE_V  0x00000001U
#define BITSCRAMBLER_RX_IN_PAUSE_S  3
/** BITSCRAMBLER_RX_FIFO_FULL : RO; bitpos: [4]; default: 0;
 *  represents the bitscrambler rx fifo in full state
 */
#define BITSCRAMBLER_RX_FIFO_FULL    (BIT(4))
#define BITSCRAMBLER_RX_FIFO_FULL_M  (BITSCRAMBLER_RX_FIFO_FULL_V << BITSCRAMBLER_RX_FIFO_FULL_S)
#define BITSCRAMBLER_RX_FIFO_FULL_V  0x00000001U
#define BITSCRAMBLER_RX_FIFO_FULL_S  4
/** BITSCRAMBLER_RX_EOF_GET_CNT : RO; bitpos: [29:16]; default: 0;
 *  represents the bytes numbers of bitscrambler rx core when get EOF
 */
#define BITSCRAMBLER_RX_EOF_GET_CNT    0x00003FFFU
#define BITSCRAMBLER_RX_EOF_GET_CNT_M  (BITSCRAMBLER_RX_EOF_GET_CNT_V << BITSCRAMBLER_RX_EOF_GET_CNT_S)
#define BITSCRAMBLER_RX_EOF_GET_CNT_V  0x00003FFFU
#define BITSCRAMBLER_RX_EOF_GET_CNT_S  16
/** BITSCRAMBLER_RX_EOF_OVERLOAD : RO; bitpos: [30]; default: 0;
 *  represents the some EOFs will be lost for bitscrambler rx core
 */
#define BITSCRAMBLER_RX_EOF_OVERLOAD    (BIT(30))
#define BITSCRAMBLER_RX_EOF_OVERLOAD_M  (BITSCRAMBLER_RX_EOF_OVERLOAD_V << BITSCRAMBLER_RX_EOF_OVERLOAD_S)
#define BITSCRAMBLER_RX_EOF_OVERLOAD_V  0x00000001U
#define BITSCRAMBLER_RX_EOF_OVERLOAD_S  30
/** BITSCRAMBLER_RX_EOF_TRACE_CLR : WT; bitpos: [31]; default: 0;
 *  write this bit to clear reg_bitscrambler_rx_eof_overload and
 *  reg_bitscrambler_rx_eof_get_cnt registers
 */
#define BITSCRAMBLER_RX_EOF_TRACE_CLR    (BIT(31))
#define BITSCRAMBLER_RX_EOF_TRACE_CLR_M  (BITSCRAMBLER_RX_EOF_TRACE_CLR_V << BITSCRAMBLER_RX_EOF_TRACE_CLR_S)
#define BITSCRAMBLER_RX_EOF_TRACE_CLR_V  0x00000001U
#define BITSCRAMBLER_RX_EOF_TRACE_CLR_S  31

/** BITSCRAMBLER_SYS_REG register
 *  Control and configuration registers
 */
#define BITSCRAMBLER_SYS_REG (DR_REG_BITSCRAMBLER_BASE + 0xf8)
/** BITSCRAMBLER_LOOP_MODE : R/W; bitpos: [0]; default: 0;
 *  write this bit to set the bitscrambler tx loop back to DMA rx
 */
#define BITSCRAMBLER_LOOP_MODE    (BIT(0))
#define BITSCRAMBLER_LOOP_MODE_M  (BITSCRAMBLER_LOOP_MODE_V << BITSCRAMBLER_LOOP_MODE_S)
#define BITSCRAMBLER_LOOP_MODE_V  0x00000001U
#define BITSCRAMBLER_LOOP_MODE_S  0
/** BITSCRAMBLER_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  Reserved
 */
#define BITSCRAMBLER_CLK_EN    (BIT(31))
#define BITSCRAMBLER_CLK_EN_M  (BITSCRAMBLER_CLK_EN_V << BITSCRAMBLER_CLK_EN_S)
#define BITSCRAMBLER_CLK_EN_V  0x00000001U
#define BITSCRAMBLER_CLK_EN_S  31

/** BITSCRAMBLER_VERSION_REG register
 *  Control and configuration registers
 */
#define BITSCRAMBLER_VERSION_REG (DR_REG_BITSCRAMBLER_BASE + 0xfc)
/** BITSCRAMBLER_BITSCRAMBLER_VER : R/W; bitpos: [27:0]; default: 36713024;
 *  Reserved
 */
#define BITSCRAMBLER_BITSCRAMBLER_VER    0x0FFFFFFFU
#define BITSCRAMBLER_BITSCRAMBLER_VER_M  (BITSCRAMBLER_BITSCRAMBLER_VER_V << BITSCRAMBLER_BITSCRAMBLER_VER_S)
#define BITSCRAMBLER_BITSCRAMBLER_VER_V  0x0FFFFFFFU
#define BITSCRAMBLER_BITSCRAMBLER_VER_S  0

#ifdef __cplusplus
}
#endif
