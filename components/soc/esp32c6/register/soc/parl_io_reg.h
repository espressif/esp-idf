/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** PARL_IO_RX_CFG0_REG register
 *  Parallel RX module configuration register0.
 */
#define PARL_IO_RX_CFG0_REG (DR_REG_PARL_IO_BASE + 0x0)
/** PARL_IO_RX_EOF_GEN_SEL : R/W; bitpos: [0]; default: 0;
 *  Write 0 to select eof generated manchnism by configured data byte length. Write 1
 *  to select eof generated manchnism by external enable signal.
 */
#define PARL_IO_RX_EOF_GEN_SEL    (BIT(0))
#define PARL_IO_RX_EOF_GEN_SEL_M  (PARL_IO_RX_EOF_GEN_SEL_V << PARL_IO_RX_EOF_GEN_SEL_S)
#define PARL_IO_RX_EOF_GEN_SEL_V  0x00000001U
#define PARL_IO_RX_EOF_GEN_SEL_S  0
/** PARL_IO_RX_START : R/W; bitpos: [1]; default: 0;
 *  Write 1 to start rx global data sampling.
 */
#define PARL_IO_RX_START    (BIT(1))
#define PARL_IO_RX_START_M  (PARL_IO_RX_START_V << PARL_IO_RX_START_S)
#define PARL_IO_RX_START_V  0x00000001U
#define PARL_IO_RX_START_S  1
/** PARL_IO_RX_DATA_BYTELEN : R/W; bitpos: [17:2]; default: 0;
 *  Configures rx received data byte length.
 */
#define PARL_IO_RX_DATA_BYTELEN    0x0000FFFFU
#define PARL_IO_RX_DATA_BYTELEN_M  (PARL_IO_RX_DATA_BYTELEN_V << PARL_IO_RX_DATA_BYTELEN_S)
#define PARL_IO_RX_DATA_BYTELEN_V  0x0000FFFFU
#define PARL_IO_RX_DATA_BYTELEN_S  2
/** PARL_IO_RX_SW_EN : R/W; bitpos: [18]; default: 0;
 *  Write 1 to enable software data sampling.
 */
#define PARL_IO_RX_SW_EN    (BIT(18))
#define PARL_IO_RX_SW_EN_M  (PARL_IO_RX_SW_EN_V << PARL_IO_RX_SW_EN_S)
#define PARL_IO_RX_SW_EN_V  0x00000001U
#define PARL_IO_RX_SW_EN_S  18
/** PARL_IO_RX_PULSE_SUBMODE_SEL : R/W; bitpos: [22:19]; default: 0;
 *  Pulse submode selection.
 *  0000: positive pulse start(data bit included) &&  positive pulse end(data bit
 *  included)
 *  0001: positive pulse start(data bit included) && positive pulse end (data bit
 *  excluded)
 *  0010: positive pulse start(data bit excluded) && positive pulse end (data bit
 *  included)
 *  0011: positive pulse start(data bit excluded) && positive pulse end (data bit
 *  excluded)
 *  0100: positive pulse start(data bit included) && length end
 *  0101: positive pulse start(data bit excluded) && length end
 *  0110: negative pulse start(data bit included) &&  negative pulse end(data bit
 *  included)
 *  0111: negative pulse start(data bit included) && negative pulse end (data bit
 *  excluded)
 *  1000: negative pulse start(data bit excluded) && negative pulse end (data bit
 *  included)
 *  1001: negative pulse start(data bit excluded) && negative pulse end (data bit
 *  excluded)
 *  1010: negative pulse start(data bit included) && length end
 *  1011: negative pulse start(data bit excluded) && length end
 */
#define PARL_IO_RX_PULSE_SUBMODE_SEL    0x0000000FU
#define PARL_IO_RX_PULSE_SUBMODE_SEL_M  (PARL_IO_RX_PULSE_SUBMODE_SEL_V << PARL_IO_RX_PULSE_SUBMODE_SEL_S)
#define PARL_IO_RX_PULSE_SUBMODE_SEL_V  0x0000000FU
#define PARL_IO_RX_PULSE_SUBMODE_SEL_S  19
/** PARL_IO_RX_LEVEL_SUBMODE_SEL : R/W; bitpos: [23]; default: 0;
 *  Write 0 to sample data at high level of external enable signal. Write 1 to sample
 *  data at low level of external enable signal.
 */
#define PARL_IO_RX_LEVEL_SUBMODE_SEL    (BIT(23))
#define PARL_IO_RX_LEVEL_SUBMODE_SEL_M  (PARL_IO_RX_LEVEL_SUBMODE_SEL_V << PARL_IO_RX_LEVEL_SUBMODE_SEL_S)
#define PARL_IO_RX_LEVEL_SUBMODE_SEL_V  0x00000001U
#define PARL_IO_RX_LEVEL_SUBMODE_SEL_S  23
/** PARL_IO_RX_SMP_MODE_SEL : R/W; bitpos: [25:24]; default: 0;
 *  Rx data sampling mode selection.
 *  000: external level enable mode
 *  001: external pulse enable mode
 *  010: internal software enable mode
 */
#define PARL_IO_RX_SMP_MODE_SEL    0x00000003U
#define PARL_IO_RX_SMP_MODE_SEL_M  (PARL_IO_RX_SMP_MODE_SEL_V << PARL_IO_RX_SMP_MODE_SEL_S)
#define PARL_IO_RX_SMP_MODE_SEL_V  0x00000003U
#define PARL_IO_RX_SMP_MODE_SEL_S  24
/** PARL_IO_RX_CLK_EDGE_SEL : R/W; bitpos: [26]; default: 0;
 *  Write 0 to enable sampling data on the rising edge of rx clock. Write 0 to enable
 *  sampling data on the falling edge of rx clock.
 */
#define PARL_IO_RX_CLK_EDGE_SEL    (BIT(26))
#define PARL_IO_RX_CLK_EDGE_SEL_M  (PARL_IO_RX_CLK_EDGE_SEL_V << PARL_IO_RX_CLK_EDGE_SEL_S)
#define PARL_IO_RX_CLK_EDGE_SEL_V  0x00000001U
#define PARL_IO_RX_CLK_EDGE_SEL_S  26
/** PARL_IO_RX_BIT_PACK_ORDER : R/W; bitpos: [27]; default: 0;
 *  Write 0 to pack bits into 1byte from MSB when data bus width is 4/2/1 bits. Write 0
 *  to pack bits into 1byte from LSB when data bus width is 4/2/1 bits.
 */
#define PARL_IO_RX_BIT_PACK_ORDER    (BIT(27))
#define PARL_IO_RX_BIT_PACK_ORDER_M  (PARL_IO_RX_BIT_PACK_ORDER_V << PARL_IO_RX_BIT_PACK_ORDER_S)
#define PARL_IO_RX_BIT_PACK_ORDER_V  0x00000001U
#define PARL_IO_RX_BIT_PACK_ORDER_S  27
/** PARL_IO_RX_BUS_WID_SEL : R/W; bitpos: [30:28]; default: 0;
 *  Rx data bus width selection.
 *  100: bus width is 1 bit
 *  011: bus width is 2 bits
 *  010: bus width is 4 bits
 *  001: bus width is 8 bits
 *  000: bus width is 16 bits
 */
#define PARL_IO_RX_BUS_WID_SEL    0x00000007U
#define PARL_IO_RX_BUS_WID_SEL_M  (PARL_IO_RX_BUS_WID_SEL_V << PARL_IO_RX_BUS_WID_SEL_S)
#define PARL_IO_RX_BUS_WID_SEL_V  0x00000007U
#define PARL_IO_RX_BUS_WID_SEL_S  28
/** PARL_IO_RX_FIFO_SRST : R/W; bitpos: [31]; default: 0;
 *  Write 1 to enable soft reset of async fifo in rx module.
 */
#define PARL_IO_RX_FIFO_SRST    (BIT(31))
#define PARL_IO_RX_FIFO_SRST_M  (PARL_IO_RX_FIFO_SRST_V << PARL_IO_RX_FIFO_SRST_S)
#define PARL_IO_RX_FIFO_SRST_V  0x00000001U
#define PARL_IO_RX_FIFO_SRST_S  31

/** PARL_IO_RX_CFG1_REG register
 *  Parallel RX module configuration register1.
 */
#define PARL_IO_RX_CFG1_REG (DR_REG_PARL_IO_BASE + 0x4)
/** PARL_IO_RX_REG_UPDATE : WT; bitpos: [2]; default: 0;
 *  Write 1 to update rx register configuration signals.
 */
#define PARL_IO_RX_REG_UPDATE    (BIT(2))
#define PARL_IO_RX_REG_UPDATE_M  (PARL_IO_RX_REG_UPDATE_V << PARL_IO_RX_REG_UPDATE_S)
#define PARL_IO_RX_REG_UPDATE_V  0x00000001U
#define PARL_IO_RX_REG_UPDATE_S  2
/** PARL_IO_RX_TIMEOUT_EN : R/W; bitpos: [3]; default: 1;
 *  Write 1 to enable timeout count to generate error eof.
 */
#define PARL_IO_RX_TIMEOUT_EN    (BIT(3))
#define PARL_IO_RX_TIMEOUT_EN_M  (PARL_IO_RX_TIMEOUT_EN_V << PARL_IO_RX_TIMEOUT_EN_S)
#define PARL_IO_RX_TIMEOUT_EN_V  0x00000001U
#define PARL_IO_RX_TIMEOUT_EN_S  3
/** PARL_IO_RX_EXT_EN_SEL : R/W; bitpos: [15:12]; default: 15;
 *  Configures rx external enable signal selection from 16 data lines.
 */
#define PARL_IO_RX_EXT_EN_SEL    0x0000000FU
#define PARL_IO_RX_EXT_EN_SEL_M  (PARL_IO_RX_EXT_EN_SEL_V << PARL_IO_RX_EXT_EN_SEL_S)
#define PARL_IO_RX_EXT_EN_SEL_V  0x0000000FU
#define PARL_IO_RX_EXT_EN_SEL_S  12
/** PARL_IO_RX_TIMEOUT_THRESHOLD : R/W; bitpos: [31:16]; default: 4095;
 *  Configures rx threshold of timeout counter.
 */
#define PARL_IO_RX_TIMEOUT_THRESHOLD    0x0000FFFFU
#define PARL_IO_RX_TIMEOUT_THRESHOLD_M  (PARL_IO_RX_TIMEOUT_THRESHOLD_V << PARL_IO_RX_TIMEOUT_THRESHOLD_S)
#define PARL_IO_RX_TIMEOUT_THRESHOLD_V  0x0000FFFFU
#define PARL_IO_RX_TIMEOUT_THRESHOLD_S  16

/** PARL_IO_TX_CFG0_REG register
 *  Parallel TX module configuration register0.
 */
#define PARL_IO_TX_CFG0_REG (DR_REG_PARL_IO_BASE + 0x8)
/** PARL_IO_TX_BYTELEN : R/W; bitpos: [17:2]; default: 0;
 *  Configures tx sending data byte length.
 */
#define PARL_IO_TX_BYTELEN    0x0000FFFFU
#define PARL_IO_TX_BYTELEN_M  (PARL_IO_TX_BYTELEN_V << PARL_IO_TX_BYTELEN_S)
#define PARL_IO_TX_BYTELEN_V  0x0000FFFFU
#define PARL_IO_TX_BYTELEN_S  2
/** PARL_IO_TX_GATING_EN : R/W; bitpos: [18]; default: 0;
 *  Write 1 to enable output tx clock gating.
 */
#define PARL_IO_TX_GATING_EN    (BIT(18))
#define PARL_IO_TX_GATING_EN_M  (PARL_IO_TX_GATING_EN_V << PARL_IO_TX_GATING_EN_S)
#define PARL_IO_TX_GATING_EN_V  0x00000001U
#define PARL_IO_TX_GATING_EN_S  18
/** PARL_IO_TX_START : R/W; bitpos: [19]; default: 0;
 *  Write 1 to start tx global data output.
 */
#define PARL_IO_TX_START    (BIT(19))
#define PARL_IO_TX_START_M  (PARL_IO_TX_START_V << PARL_IO_TX_START_S)
#define PARL_IO_TX_START_V  0x00000001U
#define PARL_IO_TX_START_S  19
/** PARL_IO_TX_HW_VALID_EN : R/W; bitpos: [20]; default: 0;
 *  Write 1 to enable tx hardware data valid signal.
 */
#define PARL_IO_TX_HW_VALID_EN    (BIT(20))
#define PARL_IO_TX_HW_VALID_EN_M  (PARL_IO_TX_HW_VALID_EN_V << PARL_IO_TX_HW_VALID_EN_S)
#define PARL_IO_TX_HW_VALID_EN_V  0x00000001U
#define PARL_IO_TX_HW_VALID_EN_S  20
/** PARL_IO_TX_SMP_EDGE_SEL : R/W; bitpos: [25]; default: 0;
 *  Write 0 to enable sampling data on the rising edge of tx clock. Write 0 to enable
 *  sampling data on the falling edge of tx clock.
 */
#define PARL_IO_TX_SMP_EDGE_SEL    (BIT(25))
#define PARL_IO_TX_SMP_EDGE_SEL_M  (PARL_IO_TX_SMP_EDGE_SEL_V << PARL_IO_TX_SMP_EDGE_SEL_S)
#define PARL_IO_TX_SMP_EDGE_SEL_V  0x00000001U
#define PARL_IO_TX_SMP_EDGE_SEL_S  25
/** PARL_IO_TX_BIT_UNPACK_ORDER : R/W; bitpos: [26]; default: 0;
 *  Write 0 to unpack bits from 1byte from MSB when data bus width is 4/2/1 bits. Write
 *  0 to unpack bits from 1byte from LSB when data bus width is 4/2/1 bits.
 */
#define PARL_IO_TX_BIT_UNPACK_ORDER    (BIT(26))
#define PARL_IO_TX_BIT_UNPACK_ORDER_M  (PARL_IO_TX_BIT_UNPACK_ORDER_V << PARL_IO_TX_BIT_UNPACK_ORDER_S)
#define PARL_IO_TX_BIT_UNPACK_ORDER_V  0x00000001U
#define PARL_IO_TX_BIT_UNPACK_ORDER_S  26
/** PARL_IO_TX_BUS_WID_SEL : R/W; bitpos: [29:27]; default: 0;
 *  Tx data bus width selection.
 *  100: bus width is 1 bit
 *  011: bus width is 2 bits
 *  010: bus width is 4 bits
 *  001: bus width is 8 bits
 *  000: bus width is 16 bits
 */
#define PARL_IO_TX_BUS_WID_SEL    0x00000007U
#define PARL_IO_TX_BUS_WID_SEL_M  (PARL_IO_TX_BUS_WID_SEL_V << PARL_IO_TX_BUS_WID_SEL_S)
#define PARL_IO_TX_BUS_WID_SEL_V  0x00000007U
#define PARL_IO_TX_BUS_WID_SEL_S  27
/** PARL_IO_TX_FIFO_SRST : R/W; bitpos: [30]; default: 0;
 *  Write 1 to enable soft reset of async fifo in tx module.
 */
#define PARL_IO_TX_FIFO_SRST    (BIT(30))
#define PARL_IO_TX_FIFO_SRST_M  (PARL_IO_TX_FIFO_SRST_V << PARL_IO_TX_FIFO_SRST_S)
#define PARL_IO_TX_FIFO_SRST_V  0x00000001U
#define PARL_IO_TX_FIFO_SRST_S  30

/** PARL_IO_TX_CFG1_REG register
 *  Parallel TX module configuration register1.
 */
#define PARL_IO_TX_CFG1_REG (DR_REG_PARL_IO_BASE + 0xc)
/** PARL_IO_TX_IDLE_VALUE : R/W; bitpos: [31:16]; default: 0;
 *  Configures data value on tx bus when IDLE state.
 */
#define PARL_IO_TX_IDLE_VALUE    0x0000FFFFU
#define PARL_IO_TX_IDLE_VALUE_M  (PARL_IO_TX_IDLE_VALUE_V << PARL_IO_TX_IDLE_VALUE_S)
#define PARL_IO_TX_IDLE_VALUE_V  0x0000FFFFU
#define PARL_IO_TX_IDLE_VALUE_S  16

/** PARL_IO_ST_REG register
 *  Parallel IO module status register0.
 */
#define PARL_IO_ST_REG (DR_REG_PARL_IO_BASE + 0x10)
/** PARL_IO_TX_READY : RO; bitpos: [31]; default: 0;
 *  Represents the status that tx is ready.
 */
#define PARL_IO_TX_READY    (BIT(31))
#define PARL_IO_TX_READY_M  (PARL_IO_TX_READY_V << PARL_IO_TX_READY_S)
#define PARL_IO_TX_READY_V  0x00000001U
#define PARL_IO_TX_READY_S  31

/** PARL_IO_INT_ENA_REG register
 *  Parallel IO interrupt enable signal configuration register.
 */
#define PARL_IO_INT_ENA_REG (DR_REG_PARL_IO_BASE + 0x14)
/** PARL_IO_TX_FIFO_REMPTY_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Write 1 to enable TX_FIFO_REMPTY_INTR.
 */
#define PARL_IO_TX_FIFO_REMPTY_INT_ENA    (BIT(0))
#define PARL_IO_TX_FIFO_REMPTY_INT_ENA_M  (PARL_IO_TX_FIFO_REMPTY_INT_ENA_V << PARL_IO_TX_FIFO_REMPTY_INT_ENA_S)
#define PARL_IO_TX_FIFO_REMPTY_INT_ENA_V  0x00000001U
#define PARL_IO_TX_FIFO_REMPTY_INT_ENA_S  0
/** PARL_IO_RX_FIFO_WFULL_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  Write 1 to enable RX_FIFO_WFULL_INTR.
 */
#define PARL_IO_RX_FIFO_WFULL_INT_ENA    (BIT(1))
#define PARL_IO_RX_FIFO_WFULL_INT_ENA_M  (PARL_IO_RX_FIFO_WFULL_INT_ENA_V << PARL_IO_RX_FIFO_WFULL_INT_ENA_S)
#define PARL_IO_RX_FIFO_WFULL_INT_ENA_V  0x00000001U
#define PARL_IO_RX_FIFO_WFULL_INT_ENA_S  1
/** PARL_IO_TX_EOF_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  Write 1 to enable TX_EOF_INTR.
 */
#define PARL_IO_TX_EOF_INT_ENA    (BIT(2))
#define PARL_IO_TX_EOF_INT_ENA_M  (PARL_IO_TX_EOF_INT_ENA_V << PARL_IO_TX_EOF_INT_ENA_S)
#define PARL_IO_TX_EOF_INT_ENA_V  0x00000001U
#define PARL_IO_TX_EOF_INT_ENA_S  2

/** PARL_IO_INT_RAW_REG register
 *  Parallel IO interrupt raw signal status register.
 */
#define PARL_IO_INT_RAW_REG (DR_REG_PARL_IO_BASE + 0x18)
/** PARL_IO_TX_FIFO_REMPTY_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt status of TX_FIFO_REMPTY_INTR.
 */
#define PARL_IO_TX_FIFO_REMPTY_INT_RAW    (BIT(0))
#define PARL_IO_TX_FIFO_REMPTY_INT_RAW_M  (PARL_IO_TX_FIFO_REMPTY_INT_RAW_V << PARL_IO_TX_FIFO_REMPTY_INT_RAW_S)
#define PARL_IO_TX_FIFO_REMPTY_INT_RAW_V  0x00000001U
#define PARL_IO_TX_FIFO_REMPTY_INT_RAW_S  0
/** PARL_IO_RX_FIFO_WFULL_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt status of RX_FIFO_WFULL_INTR.
 */
#define PARL_IO_RX_FIFO_WFULL_INT_RAW    (BIT(1))
#define PARL_IO_RX_FIFO_WFULL_INT_RAW_M  (PARL_IO_RX_FIFO_WFULL_INT_RAW_V << PARL_IO_RX_FIFO_WFULL_INT_RAW_S)
#define PARL_IO_RX_FIFO_WFULL_INT_RAW_V  0x00000001U
#define PARL_IO_RX_FIFO_WFULL_INT_RAW_S  1
/** PARL_IO_TX_EOF_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt status of TX_EOF_INTR.
 */
#define PARL_IO_TX_EOF_INT_RAW    (BIT(2))
#define PARL_IO_TX_EOF_INT_RAW_M  (PARL_IO_TX_EOF_INT_RAW_V << PARL_IO_TX_EOF_INT_RAW_S)
#define PARL_IO_TX_EOF_INT_RAW_V  0x00000001U
#define PARL_IO_TX_EOF_INT_RAW_S  2

/** PARL_IO_INT_ST_REG register
 *  Parallel IO interrupt signal status register.
 */
#define PARL_IO_INT_ST_REG (DR_REG_PARL_IO_BASE + 0x1c)
/** PARL_IO_TX_FIFO_REMPTY_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status of TX_FIFO_REMPTY_INTR.
 */
#define PARL_IO_TX_FIFO_REMPTY_INT_ST    (BIT(0))
#define PARL_IO_TX_FIFO_REMPTY_INT_ST_M  (PARL_IO_TX_FIFO_REMPTY_INT_ST_V << PARL_IO_TX_FIFO_REMPTY_INT_ST_S)
#define PARL_IO_TX_FIFO_REMPTY_INT_ST_V  0x00000001U
#define PARL_IO_TX_FIFO_REMPTY_INT_ST_S  0
/** PARL_IO_RX_FIFO_WFULL_INT_ST : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status of RX_FIFO_WFULL_INTR.
 */
#define PARL_IO_RX_FIFO_WFULL_INT_ST    (BIT(1))
#define PARL_IO_RX_FIFO_WFULL_INT_ST_M  (PARL_IO_RX_FIFO_WFULL_INT_ST_V << PARL_IO_RX_FIFO_WFULL_INT_ST_S)
#define PARL_IO_RX_FIFO_WFULL_INT_ST_V  0x00000001U
#define PARL_IO_RX_FIFO_WFULL_INT_ST_S  1
/** PARL_IO_TX_EOF_INT_ST : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status of TX_EOF_INTR.
 */
#define PARL_IO_TX_EOF_INT_ST    (BIT(2))
#define PARL_IO_TX_EOF_INT_ST_M  (PARL_IO_TX_EOF_INT_ST_V << PARL_IO_TX_EOF_INT_ST_S)
#define PARL_IO_TX_EOF_INT_ST_V  0x00000001U
#define PARL_IO_TX_EOF_INT_ST_S  2

/** PARL_IO_INT_CLR_REG register
 *  Parallel IO interrupt  clear signal configuration register.
 */
#define PARL_IO_INT_CLR_REG (DR_REG_PARL_IO_BASE + 0x20)
/** PARL_IO_TX_FIFO_REMPTY_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Write 1 to clear TX_FIFO_REMPTY_INTR.
 */
#define PARL_IO_TX_FIFO_REMPTY_INT_CLR    (BIT(0))
#define PARL_IO_TX_FIFO_REMPTY_INT_CLR_M  (PARL_IO_TX_FIFO_REMPTY_INT_CLR_V << PARL_IO_TX_FIFO_REMPTY_INT_CLR_S)
#define PARL_IO_TX_FIFO_REMPTY_INT_CLR_V  0x00000001U
#define PARL_IO_TX_FIFO_REMPTY_INT_CLR_S  0
/** PARL_IO_RX_FIFO_WFULL_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Write 1 to clear RX_FIFO_WFULL_INTR.
 */
#define PARL_IO_RX_FIFO_WFULL_INT_CLR    (BIT(1))
#define PARL_IO_RX_FIFO_WFULL_INT_CLR_M  (PARL_IO_RX_FIFO_WFULL_INT_CLR_V << PARL_IO_RX_FIFO_WFULL_INT_CLR_S)
#define PARL_IO_RX_FIFO_WFULL_INT_CLR_V  0x00000001U
#define PARL_IO_RX_FIFO_WFULL_INT_CLR_S  1
/** PARL_IO_TX_EOF_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Write 1 to clear TX_EOF_INTR.
 */
#define PARL_IO_TX_EOF_INT_CLR    (BIT(2))
#define PARL_IO_TX_EOF_INT_CLR_M  (PARL_IO_TX_EOF_INT_CLR_V << PARL_IO_TX_EOF_INT_CLR_S)
#define PARL_IO_TX_EOF_INT_CLR_V  0x00000001U
#define PARL_IO_TX_EOF_INT_CLR_S  2

/** PARL_IO_CLK_REG register
 *  Parallel IO clk configuration register
 */
#define PARL_IO_CLK_REG (DR_REG_PARL_IO_BASE + 0x120)
/** PARL_IO_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Force clock on for this register file
 */
#define PARL_IO_CLK_EN    (BIT(0))
#define PARL_IO_CLK_EN_M  (PARL_IO_CLK_EN_V << PARL_IO_CLK_EN_S)
#define PARL_IO_CLK_EN_V  0x00000001U
#define PARL_IO_CLK_EN_S  0

/** PARL_IO_VERSION_REG register
 *  Version register.
 */
#define PARL_IO_VERSION_REG (DR_REG_PARL_IO_BASE + 0x3fc)
/** PARL_IO_DATE : R/W; bitpos: [27:0]; default: 35660352;
 *  Version of this register file
 */
#define PARL_IO_DATE    0x0FFFFFFFU
#define PARL_IO_DATE_M  (PARL_IO_DATE_V << PARL_IO_DATE_S)
#define PARL_IO_DATE_V  0x0FFFFFFFU
#define PARL_IO_DATE_S  0

#ifdef __cplusplus
}
#endif
