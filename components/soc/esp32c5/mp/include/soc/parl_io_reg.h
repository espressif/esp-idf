/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** PARL_IO_RX_MODE_CFG_REG register
 *  Parallel RX Sampling mode configuration register.
 */
#define PARL_IO_RX_MODE_CFG_REG (DR_REG_PARL_IO_BASE + 0x0)
/** PARL_IO_RX_EXT_EN_SEL : R/W; bitpos: [24:21]; default: 7;
 *  Configures rx external enable signal selection from IO PAD.
 */
#define PARL_IO_RX_EXT_EN_SEL    0x0000000FU
#define PARL_IO_RX_EXT_EN_SEL_M  (PARL_IO_RX_EXT_EN_SEL_V << PARL_IO_RX_EXT_EN_SEL_S)
#define PARL_IO_RX_EXT_EN_SEL_V  0x0000000FU
#define PARL_IO_RX_EXT_EN_SEL_S  21
/** PARL_IO_RX_SW_EN : R/W; bitpos: [25]; default: 0;
 *  Write 1 to enable data sampling by software.
 */
#define PARL_IO_RX_SW_EN    (BIT(25))
#define PARL_IO_RX_SW_EN_M  (PARL_IO_RX_SW_EN_V << PARL_IO_RX_SW_EN_S)
#define PARL_IO_RX_SW_EN_V  0x00000001U
#define PARL_IO_RX_SW_EN_S  25
/** PARL_IO_RX_EXT_EN_INV : R/W; bitpos: [26]; default: 0;
 *  Write 1 to invert the external enable signal.
 */
#define PARL_IO_RX_EXT_EN_INV    (BIT(26))
#define PARL_IO_RX_EXT_EN_INV_M  (PARL_IO_RX_EXT_EN_INV_V << PARL_IO_RX_EXT_EN_INV_S)
#define PARL_IO_RX_EXT_EN_INV_V  0x00000001U
#define PARL_IO_RX_EXT_EN_INV_S  26
/** PARL_IO_RX_PULSE_SUBMODE_SEL : R/W; bitpos: [29:27]; default: 0;
 *  Configures the rxd pulse sampling submode.
 *  0: positive pulse start(data bit included) &&  positive pulse end(data bit included)
 *  1: positive pulse start(data bit included) && positive pulse end (data bit excluded)
 *  2: positive pulse start(data bit excluded) && positive pulse end (data bit included)
 *  3: positive pulse start(data bit excluded) && positive pulse end (data bit excluded)
 *  4: positive pulse start(data bit included) && length end
 *  5: positive pulse start(data bit excluded) && length end
 */
#define PARL_IO_RX_PULSE_SUBMODE_SEL    0x00000007U
#define PARL_IO_RX_PULSE_SUBMODE_SEL_M  (PARL_IO_RX_PULSE_SUBMODE_SEL_V << PARL_IO_RX_PULSE_SUBMODE_SEL_S)
#define PARL_IO_RX_PULSE_SUBMODE_SEL_V  0x00000007U
#define PARL_IO_RX_PULSE_SUBMODE_SEL_S  27
/** PARL_IO_RX_SMP_MODE_SEL : R/W; bitpos: [31:30]; default: 0;
 *  Configures the rxd sampling mode.
 *  0: external level enable mode
 *  1: external pulse enable mode
 *  2: internal software enable mode
 */
#define PARL_IO_RX_SMP_MODE_SEL    0x00000003U
#define PARL_IO_RX_SMP_MODE_SEL_M  (PARL_IO_RX_SMP_MODE_SEL_V << PARL_IO_RX_SMP_MODE_SEL_S)
#define PARL_IO_RX_SMP_MODE_SEL_V  0x00000003U
#define PARL_IO_RX_SMP_MODE_SEL_S  30

/** PARL_IO_RX_DATA_CFG_REG register
 *  Parallel RX data configuration register.
 */
#define PARL_IO_RX_DATA_CFG_REG (DR_REG_PARL_IO_BASE + 0x4)
/** PARL_IO_RX_BITLEN : R/W; bitpos: [27:9]; default: 0;
 *  Configures expected byte number of received data.
 */
#define PARL_IO_RX_BITLEN    0x0007FFFFU
#define PARL_IO_RX_BITLEN_M  (PARL_IO_RX_BITLEN_V << PARL_IO_RX_BITLEN_S)
#define PARL_IO_RX_BITLEN_V  0x0007FFFFU
#define PARL_IO_RX_BITLEN_S  9
/** PARL_IO_RX_DATA_ORDER_INV : R/W; bitpos: [28]; default: 0;
 *  Write 1 to invert bit order of one byte sent from RX_FIFO to DMA.
 */
#define PARL_IO_RX_DATA_ORDER_INV    (BIT(28))
#define PARL_IO_RX_DATA_ORDER_INV_M  (PARL_IO_RX_DATA_ORDER_INV_V << PARL_IO_RX_DATA_ORDER_INV_S)
#define PARL_IO_RX_DATA_ORDER_INV_V  0x00000001U
#define PARL_IO_RX_DATA_ORDER_INV_S  28
/** PARL_IO_RX_BUS_WID_SEL : R/W; bitpos: [31:29]; default: 3;
 *  Configures the rxd bus width.
 *  0: bus width is 1.
 *  1: bus width is 2.
 *  2: bus width is 4.
 *  3: bus width is 8.
 */
#define PARL_IO_RX_BUS_WID_SEL    0x00000007U
#define PARL_IO_RX_BUS_WID_SEL_M  (PARL_IO_RX_BUS_WID_SEL_V << PARL_IO_RX_BUS_WID_SEL_S)
#define PARL_IO_RX_BUS_WID_SEL_V  0x00000007U
#define PARL_IO_RX_BUS_WID_SEL_S  29

/** PARL_IO_RX_GENRL_CFG_REG register
 *  Parallel RX general configuration register.
 */
#define PARL_IO_RX_GENRL_CFG_REG (DR_REG_PARL_IO_BASE + 0x8)
/** PARL_IO_RX_GATING_EN : R/W; bitpos: [12]; default: 0;
 *  Write 1 to enable the clock gating of output rx clock.
 */
#define PARL_IO_RX_GATING_EN    (BIT(12))
#define PARL_IO_RX_GATING_EN_M  (PARL_IO_RX_GATING_EN_V << PARL_IO_RX_GATING_EN_S)
#define PARL_IO_RX_GATING_EN_V  0x00000001U
#define PARL_IO_RX_GATING_EN_S  12
/** PARL_IO_RX_TIMEOUT_THRES : R/W; bitpos: [28:13]; default: 4095;
 *  Configures threshold of timeout counter.
 */
#define PARL_IO_RX_TIMEOUT_THRES    0x0000FFFFU
#define PARL_IO_RX_TIMEOUT_THRES_M  (PARL_IO_RX_TIMEOUT_THRES_V << PARL_IO_RX_TIMEOUT_THRES_S)
#define PARL_IO_RX_TIMEOUT_THRES_V  0x0000FFFFU
#define PARL_IO_RX_TIMEOUT_THRES_S  13
/** PARL_IO_RX_TIMEOUT_EN : R/W; bitpos: [29]; default: 1;
 *  Write 1 to enable timeout function to generate error eof.
 */
#define PARL_IO_RX_TIMEOUT_EN    (BIT(29))
#define PARL_IO_RX_TIMEOUT_EN_M  (PARL_IO_RX_TIMEOUT_EN_V << PARL_IO_RX_TIMEOUT_EN_S)
#define PARL_IO_RX_TIMEOUT_EN_V  0x00000001U
#define PARL_IO_RX_TIMEOUT_EN_S  29
/** PARL_IO_RX_EOF_GEN_SEL : R/W; bitpos: [30]; default: 0;
 *  Configures the DMA eof generated mechanism. 1'b0: eof generated by data bit length.
 *  1'b1: eof generated by external enable signal.
 */
#define PARL_IO_RX_EOF_GEN_SEL    (BIT(30))
#define PARL_IO_RX_EOF_GEN_SEL_M  (PARL_IO_RX_EOF_GEN_SEL_V << PARL_IO_RX_EOF_GEN_SEL_S)
#define PARL_IO_RX_EOF_GEN_SEL_V  0x00000001U
#define PARL_IO_RX_EOF_GEN_SEL_S  30

/** PARL_IO_RX_START_CFG_REG register
 *  Parallel RX Start configuration register.
 */
#define PARL_IO_RX_START_CFG_REG (DR_REG_PARL_IO_BASE + 0xc)
/** PARL_IO_RX_START : R/W; bitpos: [31]; default: 0;
 *  Write 1 to start rx data sampling.
 */
#define PARL_IO_RX_START    (BIT(31))
#define PARL_IO_RX_START_M  (PARL_IO_RX_START_V << PARL_IO_RX_START_S)
#define PARL_IO_RX_START_V  0x00000001U
#define PARL_IO_RX_START_S  31

/** PARL_IO_TX_DATA_CFG_REG register
 *  Parallel TX data configuration register.
 */
#define PARL_IO_TX_DATA_CFG_REG (DR_REG_PARL_IO_BASE + 0x10)
/** PARL_IO_TX_BITLEN : R/W; bitpos: [27:9]; default: 0;
 *  Configures expected byte number of sent data.
 */
#define PARL_IO_TX_BITLEN    0x0007FFFFU
#define PARL_IO_TX_BITLEN_M  (PARL_IO_TX_BITLEN_V << PARL_IO_TX_BITLEN_S)
#define PARL_IO_TX_BITLEN_V  0x0007FFFFU
#define PARL_IO_TX_BITLEN_S  9
/** PARL_IO_TX_DATA_ORDER_INV : R/W; bitpos: [28]; default: 0;
 *  Write 1 to invert bit order of one byte sent from TX_FIFO to IO data.
 */
#define PARL_IO_TX_DATA_ORDER_INV    (BIT(28))
#define PARL_IO_TX_DATA_ORDER_INV_M  (PARL_IO_TX_DATA_ORDER_INV_V << PARL_IO_TX_DATA_ORDER_INV_S)
#define PARL_IO_TX_DATA_ORDER_INV_V  0x00000001U
#define PARL_IO_TX_DATA_ORDER_INV_S  28
/** PARL_IO_TX_BUS_WID_SEL : R/W; bitpos: [31:29]; default: 3;
 *  Configures the txd bus width.
 *  0: bus width is 1.
 *  1: bus width is 2.
 *  2: bus width is 4.
 *  3: bus width is 8.
 */
#define PARL_IO_TX_BUS_WID_SEL    0x00000007U
#define PARL_IO_TX_BUS_WID_SEL_M  (PARL_IO_TX_BUS_WID_SEL_V << PARL_IO_TX_BUS_WID_SEL_S)
#define PARL_IO_TX_BUS_WID_SEL_V  0x00000007U
#define PARL_IO_TX_BUS_WID_SEL_S  29

/** PARL_IO_TX_START_CFG_REG register
 *  Parallel TX Start configuration register.
 */
#define PARL_IO_TX_START_CFG_REG (DR_REG_PARL_IO_BASE + 0x14)
/** PARL_IO_TX_START : R/W; bitpos: [31]; default: 0;
 *  Write 1 to start tx data transmit.
 */
#define PARL_IO_TX_START    (BIT(31))
#define PARL_IO_TX_START_M  (PARL_IO_TX_START_V << PARL_IO_TX_START_S)
#define PARL_IO_TX_START_V  0x00000001U
#define PARL_IO_TX_START_S  31

/** PARL_IO_TX_GENRL_CFG_REG register
 *  Parallel TX general configuration register.
 */
#define PARL_IO_TX_GENRL_CFG_REG (DR_REG_PARL_IO_BASE + 0x18)
/** PARL_IO_TX_EOF_GEN_SEL : R/W; bitpos: [13]; default: 0;
 *  Configures the tx eof generated mechanism. 1'b0: eof generated by data bit length.
 *  1'b1: eof generated by DMA eof.
 */
#define PARL_IO_TX_EOF_GEN_SEL    (BIT(13))
#define PARL_IO_TX_EOF_GEN_SEL_M  (PARL_IO_TX_EOF_GEN_SEL_V << PARL_IO_TX_EOF_GEN_SEL_S)
#define PARL_IO_TX_EOF_GEN_SEL_V  0x00000001U
#define PARL_IO_TX_EOF_GEN_SEL_S  13
/** PARL_IO_TX_IDLE_VALUE : R/W; bitpos: [29:14]; default: 0;
 *  Configures bus value of transmitter in IDLE state.
 */
#define PARL_IO_TX_IDLE_VALUE    0x0000FFFFU
#define PARL_IO_TX_IDLE_VALUE_M  (PARL_IO_TX_IDLE_VALUE_V << PARL_IO_TX_IDLE_VALUE_S)
#define PARL_IO_TX_IDLE_VALUE_V  0x0000FFFFU
#define PARL_IO_TX_IDLE_VALUE_S  14
/** PARL_IO_TX_GATING_EN : R/W; bitpos: [30]; default: 0;
 *  Write 1 to enable the clock gating of output tx clock.
 */
#define PARL_IO_TX_GATING_EN    (BIT(30))
#define PARL_IO_TX_GATING_EN_M  (PARL_IO_TX_GATING_EN_V << PARL_IO_TX_GATING_EN_S)
#define PARL_IO_TX_GATING_EN_V  0x00000001U
#define PARL_IO_TX_GATING_EN_S  30
/** PARL_IO_TX_VALID_OUTPUT_EN : R/W; bitpos: [31]; default: 0;
 *  Write 1 to enable the output of tx data valid signal.
 */
#define PARL_IO_TX_VALID_OUTPUT_EN    (BIT(31))
#define PARL_IO_TX_VALID_OUTPUT_EN_M  (PARL_IO_TX_VALID_OUTPUT_EN_V << PARL_IO_TX_VALID_OUTPUT_EN_S)
#define PARL_IO_TX_VALID_OUTPUT_EN_V  0x00000001U
#define PARL_IO_TX_VALID_OUTPUT_EN_S  31

/** PARL_IO_FIFO_CFG_REG register
 *  Parallel IO FIFO configuration register.
 */
#define PARL_IO_FIFO_CFG_REG (DR_REG_PARL_IO_BASE + 0x1c)
/** PARL_IO_TX_FIFO_SRST : R/W; bitpos: [30]; default: 0;
 *  Write 1 to reset async fifo in tx module.
 */
#define PARL_IO_TX_FIFO_SRST    (BIT(30))
#define PARL_IO_TX_FIFO_SRST_M  (PARL_IO_TX_FIFO_SRST_V << PARL_IO_TX_FIFO_SRST_S)
#define PARL_IO_TX_FIFO_SRST_V  0x00000001U
#define PARL_IO_TX_FIFO_SRST_S  30
/** PARL_IO_RX_FIFO_SRST : R/W; bitpos: [31]; default: 0;
 *  Write 1 to reset async fifo in rx module.
 */
#define PARL_IO_RX_FIFO_SRST    (BIT(31))
#define PARL_IO_RX_FIFO_SRST_M  (PARL_IO_RX_FIFO_SRST_V << PARL_IO_RX_FIFO_SRST_S)
#define PARL_IO_RX_FIFO_SRST_V  0x00000001U
#define PARL_IO_RX_FIFO_SRST_S  31

/** PARL_IO_REG_UPDATE_REG register
 *  Parallel IO FIFO configuration register.
 */
#define PARL_IO_REG_UPDATE_REG (DR_REG_PARL_IO_BASE + 0x20)
/** PARL_IO_RX_REG_UPDATE : WT; bitpos: [31]; default: 0;
 *  Write 1 to update rx register configuration.
 */
#define PARL_IO_RX_REG_UPDATE    (BIT(31))
#define PARL_IO_RX_REG_UPDATE_M  (PARL_IO_RX_REG_UPDATE_V << PARL_IO_RX_REG_UPDATE_S)
#define PARL_IO_RX_REG_UPDATE_V  0x00000001U
#define PARL_IO_RX_REG_UPDATE_S  31

/** PARL_IO_ST_REG register
 *  Parallel IO module status register0.
 */
#define PARL_IO_ST_REG (DR_REG_PARL_IO_BASE + 0x24)
/** PARL_IO_TX_READY : RO; bitpos: [31]; default: 0;
 *  Represents the status that tx is ready to transmit.
 */
#define PARL_IO_TX_READY    (BIT(31))
#define PARL_IO_TX_READY_M  (PARL_IO_TX_READY_V << PARL_IO_TX_READY_S)
#define PARL_IO_TX_READY_V  0x00000001U
#define PARL_IO_TX_READY_S  31

/** PARL_IO_INT_ENA_REG register
 *  Parallel IO interrupt enable singal configuration register.
 */
#define PARL_IO_INT_ENA_REG (DR_REG_PARL_IO_BASE + 0x28)
/** PARL_IO_TX_FIFO_REMPTY_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Write 1 to enable TX_FIFO_REMPTY_INT.
 */
#define PARL_IO_TX_FIFO_REMPTY_INT_ENA    (BIT(0))
#define PARL_IO_TX_FIFO_REMPTY_INT_ENA_M  (PARL_IO_TX_FIFO_REMPTY_INT_ENA_V << PARL_IO_TX_FIFO_REMPTY_INT_ENA_S)
#define PARL_IO_TX_FIFO_REMPTY_INT_ENA_V  0x00000001U
#define PARL_IO_TX_FIFO_REMPTY_INT_ENA_S  0
/** PARL_IO_RX_FIFO_WOVF_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  Write 1 to enable RX_FIFO_WOVF_INT.
 */
#define PARL_IO_RX_FIFO_WOVF_INT_ENA    (BIT(1))
#define PARL_IO_RX_FIFO_WOVF_INT_ENA_M  (PARL_IO_RX_FIFO_WOVF_INT_ENA_V << PARL_IO_RX_FIFO_WOVF_INT_ENA_S)
#define PARL_IO_RX_FIFO_WOVF_INT_ENA_V  0x00000001U
#define PARL_IO_RX_FIFO_WOVF_INT_ENA_S  1
/** PARL_IO_TX_EOF_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  Write 1 to enable TX_EOF_INT.
 */
#define PARL_IO_TX_EOF_INT_ENA    (BIT(2))
#define PARL_IO_TX_EOF_INT_ENA_M  (PARL_IO_TX_EOF_INT_ENA_V << PARL_IO_TX_EOF_INT_ENA_S)
#define PARL_IO_TX_EOF_INT_ENA_V  0x00000001U
#define PARL_IO_TX_EOF_INT_ENA_S  2

/** PARL_IO_INT_RAW_REG register
 *  Parallel IO interrupt raw singal status register.
 */
#define PARL_IO_INT_RAW_REG (DR_REG_PARL_IO_BASE + 0x2c)
/** PARL_IO_TX_FIFO_REMPTY_INT_RAW : R/SS/WTC; bitpos: [0]; default: 0;
 *  The raw interrupt status of TX_FIFO_REMPTY_INT.
 */
#define PARL_IO_TX_FIFO_REMPTY_INT_RAW    (BIT(0))
#define PARL_IO_TX_FIFO_REMPTY_INT_RAW_M  (PARL_IO_TX_FIFO_REMPTY_INT_RAW_V << PARL_IO_TX_FIFO_REMPTY_INT_RAW_S)
#define PARL_IO_TX_FIFO_REMPTY_INT_RAW_V  0x00000001U
#define PARL_IO_TX_FIFO_REMPTY_INT_RAW_S  0
/** PARL_IO_RX_FIFO_WOVF_INT_RAW : R/SS/WTC; bitpos: [1]; default: 0;
 *  The raw interrupt status of RX_FIFO_WOVF_INT.
 */
#define PARL_IO_RX_FIFO_WOVF_INT_RAW    (BIT(1))
#define PARL_IO_RX_FIFO_WOVF_INT_RAW_M  (PARL_IO_RX_FIFO_WOVF_INT_RAW_V << PARL_IO_RX_FIFO_WOVF_INT_RAW_S)
#define PARL_IO_RX_FIFO_WOVF_INT_RAW_V  0x00000001U
#define PARL_IO_RX_FIFO_WOVF_INT_RAW_S  1
/** PARL_IO_TX_EOF_INT_RAW : R/SS/WTC; bitpos: [2]; default: 0;
 *  The raw interrupt status of TX_EOF_INT.
 */
#define PARL_IO_TX_EOF_INT_RAW    (BIT(2))
#define PARL_IO_TX_EOF_INT_RAW_M  (PARL_IO_TX_EOF_INT_RAW_V << PARL_IO_TX_EOF_INT_RAW_S)
#define PARL_IO_TX_EOF_INT_RAW_V  0x00000001U
#define PARL_IO_TX_EOF_INT_RAW_S  2

/** PARL_IO_INT_ST_REG register
 *  Parallel IO interrupt singal status register.
 */
#define PARL_IO_INT_ST_REG (DR_REG_PARL_IO_BASE + 0x30)
/** PARL_IO_TX_FIFO_REMPTY_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status of TX_FIFO_REMPTY_INT.
 */
#define PARL_IO_TX_FIFO_REMPTY_INT_ST    (BIT(0))
#define PARL_IO_TX_FIFO_REMPTY_INT_ST_M  (PARL_IO_TX_FIFO_REMPTY_INT_ST_V << PARL_IO_TX_FIFO_REMPTY_INT_ST_S)
#define PARL_IO_TX_FIFO_REMPTY_INT_ST_V  0x00000001U
#define PARL_IO_TX_FIFO_REMPTY_INT_ST_S  0
/** PARL_IO_RX_FIFO_WOVF_INT_ST : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status of RX_FIFO_WOVF_INT.
 */
#define PARL_IO_RX_FIFO_WOVF_INT_ST    (BIT(1))
#define PARL_IO_RX_FIFO_WOVF_INT_ST_M  (PARL_IO_RX_FIFO_WOVF_INT_ST_V << PARL_IO_RX_FIFO_WOVF_INT_ST_S)
#define PARL_IO_RX_FIFO_WOVF_INT_ST_V  0x00000001U
#define PARL_IO_RX_FIFO_WOVF_INT_ST_S  1
/** PARL_IO_TX_EOF_INT_ST : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status of TX_EOF_INT.
 */
#define PARL_IO_TX_EOF_INT_ST    (BIT(2))
#define PARL_IO_TX_EOF_INT_ST_M  (PARL_IO_TX_EOF_INT_ST_V << PARL_IO_TX_EOF_INT_ST_S)
#define PARL_IO_TX_EOF_INT_ST_V  0x00000001U
#define PARL_IO_TX_EOF_INT_ST_S  2

/** PARL_IO_INT_CLR_REG register
 *  Parallel IO interrupt  clear singal configuration register.
 */
#define PARL_IO_INT_CLR_REG (DR_REG_PARL_IO_BASE + 0x34)
/** PARL_IO_TX_FIFO_REMPTY_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Write 1 to clear TX_FIFO_REMPTY_INT.
 */
#define PARL_IO_TX_FIFO_REMPTY_INT_CLR    (BIT(0))
#define PARL_IO_TX_FIFO_REMPTY_INT_CLR_M  (PARL_IO_TX_FIFO_REMPTY_INT_CLR_V << PARL_IO_TX_FIFO_REMPTY_INT_CLR_S)
#define PARL_IO_TX_FIFO_REMPTY_INT_CLR_V  0x00000001U
#define PARL_IO_TX_FIFO_REMPTY_INT_CLR_S  0
/** PARL_IO_RX_FIFO_WOVF_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Write 1 to clear RX_FIFO_WOVF_INT.
 */
#define PARL_IO_RX_FIFO_WOVF_INT_CLR    (BIT(1))
#define PARL_IO_RX_FIFO_WOVF_INT_CLR_M  (PARL_IO_RX_FIFO_WOVF_INT_CLR_V << PARL_IO_RX_FIFO_WOVF_INT_CLR_S)
#define PARL_IO_RX_FIFO_WOVF_INT_CLR_V  0x00000001U
#define PARL_IO_RX_FIFO_WOVF_INT_CLR_S  1
/** PARL_IO_TX_EOF_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Write 1 to clear TX_EOF_INT.
 */
#define PARL_IO_TX_EOF_INT_CLR    (BIT(2))
#define PARL_IO_TX_EOF_INT_CLR_M  (PARL_IO_TX_EOF_INT_CLR_V << PARL_IO_TX_EOF_INT_CLR_S)
#define PARL_IO_TX_EOF_INT_CLR_V  0x00000001U
#define PARL_IO_TX_EOF_INT_CLR_S  2

/** PARL_IO_RX_ST0_REG register
 *  Parallel IO RX status register0
 */
#define PARL_IO_RX_ST0_REG (DR_REG_PARL_IO_BASE + 0x38)
/** PARL_IO_RX_CNT : RO; bitpos: [12:8]; default: 0;
 *  Indicates the cycle number of reading Rx FIFO.
 */
#define PARL_IO_RX_CNT    0x0000001FU
#define PARL_IO_RX_CNT_M  (PARL_IO_RX_CNT_V << PARL_IO_RX_CNT_S)
#define PARL_IO_RX_CNT_V  0x0000001FU
#define PARL_IO_RX_CNT_S  8
/** PARL_IO_RX_FIFO_WR_BIT_CNT : RO; bitpos: [31:13]; default: 0;
 *  Indicates the current written bit number into Rx FIFO.
 */
#define PARL_IO_RX_FIFO_WR_BIT_CNT    0x0007FFFFU
#define PARL_IO_RX_FIFO_WR_BIT_CNT_M  (PARL_IO_RX_FIFO_WR_BIT_CNT_V << PARL_IO_RX_FIFO_WR_BIT_CNT_S)
#define PARL_IO_RX_FIFO_WR_BIT_CNT_V  0x0007FFFFU
#define PARL_IO_RX_FIFO_WR_BIT_CNT_S  13

/** PARL_IO_RX_ST1_REG register
 *  Parallel IO RX status register1
 */
#define PARL_IO_RX_ST1_REG (DR_REG_PARL_IO_BASE + 0x3c)
/** PARL_IO_RX_FIFO_RD_BIT_CNT : RO; bitpos: [31:13]; default: 0;
 *  Indicates the current read bit number from Rx FIFO.
 */
#define PARL_IO_RX_FIFO_RD_BIT_CNT    0x0007FFFFU
#define PARL_IO_RX_FIFO_RD_BIT_CNT_M  (PARL_IO_RX_FIFO_RD_BIT_CNT_V << PARL_IO_RX_FIFO_RD_BIT_CNT_S)
#define PARL_IO_RX_FIFO_RD_BIT_CNT_V  0x0007FFFFU
#define PARL_IO_RX_FIFO_RD_BIT_CNT_S  13

/** PARL_IO_TX_ST0_REG register
 *  Parallel IO TX status register0
 */
#define PARL_IO_TX_ST0_REG (DR_REG_PARL_IO_BASE + 0x40)
/** PARL_IO_TX_CNT : RO; bitpos: [12:6]; default: 0;
 *  Indicates the cycle number of reading Tx FIFO.
 */
#define PARL_IO_TX_CNT    0x0000007FU
#define PARL_IO_TX_CNT_M  (PARL_IO_TX_CNT_V << PARL_IO_TX_CNT_S)
#define PARL_IO_TX_CNT_V  0x0000007FU
#define PARL_IO_TX_CNT_S  6
/** PARL_IO_TX_FIFO_RD_BIT_CNT : RO; bitpos: [31:13]; default: 0;
 *  Indicates the current read bit number from Tx FIFO.
 */
#define PARL_IO_TX_FIFO_RD_BIT_CNT    0x0007FFFFU
#define PARL_IO_TX_FIFO_RD_BIT_CNT_M  (PARL_IO_TX_FIFO_RD_BIT_CNT_V << PARL_IO_TX_FIFO_RD_BIT_CNT_S)
#define PARL_IO_TX_FIFO_RD_BIT_CNT_V  0x0007FFFFU
#define PARL_IO_TX_FIFO_RD_BIT_CNT_S  13

/** PARL_IO_RX_CLK_CFG_REG register
 *  Parallel IO RX clk configuration register
 */
#define PARL_IO_RX_CLK_CFG_REG (DR_REG_PARL_IO_BASE + 0x44)
/** PARL_IO_RX_CLK_I_INV : R/W; bitpos: [30]; default: 0;
 *  Write 1 to invert the input Rx core clock.
 */
#define PARL_IO_RX_CLK_I_INV    (BIT(30))
#define PARL_IO_RX_CLK_I_INV_M  (PARL_IO_RX_CLK_I_INV_V << PARL_IO_RX_CLK_I_INV_S)
#define PARL_IO_RX_CLK_I_INV_V  0x00000001U
#define PARL_IO_RX_CLK_I_INV_S  30
/** PARL_IO_RX_CLK_O_INV : R/W; bitpos: [31]; default: 0;
 *  Write 1 to invert the output Rx core clock.
 */
#define PARL_IO_RX_CLK_O_INV    (BIT(31))
#define PARL_IO_RX_CLK_O_INV_M  (PARL_IO_RX_CLK_O_INV_V << PARL_IO_RX_CLK_O_INV_S)
#define PARL_IO_RX_CLK_O_INV_V  0x00000001U
#define PARL_IO_RX_CLK_O_INV_S  31

/** PARL_IO_TX_CLK_CFG_REG register
 *  Parallel IO TX clk configuration register
 */
#define PARL_IO_TX_CLK_CFG_REG (DR_REG_PARL_IO_BASE + 0x48)
/** PARL_IO_TX_CLK_I_INV : R/W; bitpos: [30]; default: 0;
 *  Write 1 to invert the input Tx core clock.
 */
#define PARL_IO_TX_CLK_I_INV    (BIT(30))
#define PARL_IO_TX_CLK_I_INV_M  (PARL_IO_TX_CLK_I_INV_V << PARL_IO_TX_CLK_I_INV_S)
#define PARL_IO_TX_CLK_I_INV_V  0x00000001U
#define PARL_IO_TX_CLK_I_INV_S  30
/** PARL_IO_TX_CLK_O_INV : R/W; bitpos: [31]; default: 0;
 *  Write 1 to invert the output Tx core clock.
 */
#define PARL_IO_TX_CLK_O_INV    (BIT(31))
#define PARL_IO_TX_CLK_O_INV_M  (PARL_IO_TX_CLK_O_INV_V << PARL_IO_TX_CLK_O_INV_S)
#define PARL_IO_TX_CLK_O_INV_V  0x00000001U
#define PARL_IO_TX_CLK_O_INV_S  31

/** PARL_IO_CLK_REG register
 *  Parallel IO clk configuration register
 */
#define PARL_IO_CLK_REG (DR_REG_PARL_IO_BASE + 0x120)
/** PARL_IO_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  Force clock on for this register file
 */
#define PARL_IO_CLK_EN    (BIT(31))
#define PARL_IO_CLK_EN_M  (PARL_IO_CLK_EN_V << PARL_IO_CLK_EN_S)
#define PARL_IO_CLK_EN_V  0x00000001U
#define PARL_IO_CLK_EN_S  31

/** PARL_IO_VERSION_REG register
 *  Version register.
 */
#define PARL_IO_VERSION_REG (DR_REG_PARL_IO_BASE + 0x3fc)
/** PARL_IO_DATE : R/W; bitpos: [27:0]; default: 35725920;
 *  Version of this register file
 */
#define PARL_IO_DATE    0x0FFFFFFFU
#define PARL_IO_DATE_M  (PARL_IO_DATE_V << PARL_IO_DATE_S)
#define PARL_IO_DATE_V  0x0FFFFFFFU
#define PARL_IO_DATE_S  0

#ifdef __cplusplus
}
#endif
