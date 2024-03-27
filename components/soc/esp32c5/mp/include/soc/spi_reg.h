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

/** SPI_CMD_REG register
 *  Command control register
 */
#define SPI_CMD_REG(i) (REG_SPI_BASE(i) + 0x0)
/** SPI_CONF_BITLEN : R/W; bitpos: [17:0]; default: 0;
 *  Configures the SPI_CLK cycles of SPI CONF state.
 *  Measurement unit: SPI_CLK clock cycle.\\
 *  Can be configured in CONF state.
 */
#define SPI_CONF_BITLEN    0x0003FFFFU
#define SPI_CONF_BITLEN_M  (SPI_CONF_BITLEN_V << SPI_CONF_BITLEN_S)
#define SPI_CONF_BITLEN_V  0x0003FFFFU
#define SPI_CONF_BITLEN_S  0
/** SPI_UPDATE : WT; bitpos: [23]; default: 0;
 *  Configures whether or not to synchronize SPI registers from APB clock domain into
 *  SPI module clock domain. \\
 *  0: Not synchronize \\
 *  1: Synchronize \\
 *  This bit is only used in SPI master transfer.
 */
#define SPI_UPDATE    (BIT(23))
#define SPI_UPDATE_M  (SPI_UPDATE_V << SPI_UPDATE_S)
#define SPI_UPDATE_V  0x00000001U
#define SPI_UPDATE_S  23
/** SPI_USR : R/W/SC; bitpos: [24]; default: 0;
 *  Configures whether or not to enable user-defined command. \\
 *  0: Not enable \\
 *  1: Enable \\
 *  An SPI operation will be triggered when the bit is set. This bit will be cleared
 *  once the operation is done. Can not be changed by CONF_buf.
 */
#define SPI_USR    (BIT(24))
#define SPI_USR_M  (SPI_USR_V << SPI_USR_S)
#define SPI_USR_V  0x00000001U
#define SPI_USR_S  24

/** SPI_ADDR_REG register
 *  Address value register
 */
#define SPI_ADDR_REG(i) (REG_SPI_BASE(i) + 0x4)
/** SPI_USR_ADDR_VALUE : R/W; bitpos: [31:0]; default: 0;
 *  Configures the address to slave.
 *  Can be configured in CONF state.
 */
#define SPI_USR_ADDR_VALUE    0xFFFFFFFFU
#define SPI_USR_ADDR_VALUE_M  (SPI_USR_ADDR_VALUE_V << SPI_USR_ADDR_VALUE_S)
#define SPI_USR_ADDR_VALUE_V  0xFFFFFFFFU
#define SPI_USR_ADDR_VALUE_S  0

/** SPI_CTRL_REG register
 *  SPI control register
 */
#define SPI_CTRL_REG(i) (REG_SPI_BASE(i) + 0x8)
/** SPI_DUMMY_OUT : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to output the FSPI bus signals in DUMMY state. \\
 *  0: Not output \\
 *  1: Output \\
 *  Can be configured in CONF state.
 */
#define SPI_DUMMY_OUT    (BIT(3))
#define SPI_DUMMY_OUT_M  (SPI_DUMMY_OUT_V << SPI_DUMMY_OUT_S)
#define SPI_DUMMY_OUT_V  0x00000001U
#define SPI_DUMMY_OUT_S  3
/** SPI_FADDR_DUAL : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable 2-bit mode during address (ADDR) state.\\
 *  0: Disable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_FADDR_DUAL    (BIT(5))
#define SPI_FADDR_DUAL_M  (SPI_FADDR_DUAL_V << SPI_FADDR_DUAL_S)
#define SPI_FADDR_DUAL_V  0x00000001U
#define SPI_FADDR_DUAL_S  5
/** SPI_FADDR_QUAD : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to enable 4-bit mode during address (ADDR) state. \\
 *  0: Disable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_FADDR_QUAD    (BIT(6))
#define SPI_FADDR_QUAD_M  (SPI_FADDR_QUAD_V << SPI_FADDR_QUAD_S)
#define SPI_FADDR_QUAD_V  0x00000001U
#define SPI_FADDR_QUAD_S  6
/** SPI_FADDR_OCT : HRO; bitpos: [7]; default: 0;
 *  Configures whether or not to enable 8-bit mode during address (ADDR) state. \\
 *  0: Disable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_FADDR_OCT    (BIT(7))
#define SPI_FADDR_OCT_M  (SPI_FADDR_OCT_V << SPI_FADDR_OCT_S)
#define SPI_FADDR_OCT_V  0x00000001U
#define SPI_FADDR_OCT_S  7
/** SPI_FCMD_DUAL : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to enable 2-bit mode during command (CMD) state. \\
 *  0: Disable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_FCMD_DUAL    (BIT(8))
#define SPI_FCMD_DUAL_M  (SPI_FCMD_DUAL_V << SPI_FCMD_DUAL_S)
#define SPI_FCMD_DUAL_V  0x00000001U
#define SPI_FCMD_DUAL_S  8
/** SPI_FCMD_QUAD : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable 4-bit mode during command (CMD) state. \\
 *  0: Disable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_FCMD_QUAD    (BIT(9))
#define SPI_FCMD_QUAD_M  (SPI_FCMD_QUAD_V << SPI_FCMD_QUAD_S)
#define SPI_FCMD_QUAD_V  0x00000001U
#define SPI_FCMD_QUAD_S  9
/** SPI_FCMD_OCT : HRO; bitpos: [10]; default: 0;
 *  Configures whether or not to enable 8-bit mode during command (CMD) state. \\
 *  0: Disable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_FCMD_OCT    (BIT(10))
#define SPI_FCMD_OCT_M  (SPI_FCMD_OCT_V << SPI_FCMD_OCT_S)
#define SPI_FCMD_OCT_V  0x00000001U
#define SPI_FCMD_OCT_S  10
/** SPI_FREAD_DUAL : R/W; bitpos: [14]; default: 0;
 *  Configures whether or not to enable the 2-bit mode of read-data (DIN) state in read
 *  operations. \\
 *  0: Disable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_FREAD_DUAL    (BIT(14))
#define SPI_FREAD_DUAL_M  (SPI_FREAD_DUAL_V << SPI_FREAD_DUAL_S)
#define SPI_FREAD_DUAL_V  0x00000001U
#define SPI_FREAD_DUAL_S  14
/** SPI_FREAD_QUAD : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable the 4-bit mode of read-data (DIN) state in read
 *  operations. \\
 *  0: Disable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_FREAD_QUAD    (BIT(15))
#define SPI_FREAD_QUAD_M  (SPI_FREAD_QUAD_V << SPI_FREAD_QUAD_S)
#define SPI_FREAD_QUAD_V  0x00000001U
#define SPI_FREAD_QUAD_S  15
/** SPI_FREAD_OCT : HRO; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the 8-bit mode of read-data (DIN) state in read
 *  operations. \\
 *  0: Disable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_FREAD_OCT    (BIT(16))
#define SPI_FREAD_OCT_M  (SPI_FREAD_OCT_V << SPI_FREAD_OCT_S)
#define SPI_FREAD_OCT_V  0x00000001U
#define SPI_FREAD_OCT_S  16
/** SPI_Q_POL : R/W; bitpos: [18]; default: 1;
 *  Configures MISO line polarity. \\
 *  0: Low \\
 *  1: High \\
 *  Can be configured in CONF state.
 */
#define SPI_Q_POL    (BIT(18))
#define SPI_Q_POL_M  (SPI_Q_POL_V << SPI_Q_POL_S)
#define SPI_Q_POL_V  0x00000001U
#define SPI_Q_POL_S  18
/** SPI_D_POL : R/W; bitpos: [19]; default: 1;
 *  Configures MOSI line polarity. \\
 *  0: Low \\
 *  1: High \\
 *  Can be configured in CONF state.
 */
#define SPI_D_POL    (BIT(19))
#define SPI_D_POL_M  (SPI_D_POL_V << SPI_D_POL_S)
#define SPI_D_POL_V  0x00000001U
#define SPI_D_POL_S  19
/** SPI_HOLD_POL : R/W; bitpos: [20]; default: 1;
 *  Configures SPI_HOLD output value when SPI is in idle. \\
 *  0: Output low \\
 *  1: Output high \\
 *  Can be configured in CONF state.
 */
#define SPI_HOLD_POL    (BIT(20))
#define SPI_HOLD_POL_M  (SPI_HOLD_POL_V << SPI_HOLD_POL_S)
#define SPI_HOLD_POL_V  0x00000001U
#define SPI_HOLD_POL_S  20
/** SPI_WP_POL : R/W; bitpos: [21]; default: 1;
 *  Configures the output value of write-protect signal when SPI is in idle. \\
 *  0: Output low \\
 *  1: Output high \\
 *  Can be configured in CONF state.
 */
#define SPI_WP_POL    (BIT(21))
#define SPI_WP_POL_M  (SPI_WP_POL_V << SPI_WP_POL_S)
#define SPI_WP_POL_V  0x00000001U
#define SPI_WP_POL_S  21
/** SPI_RD_BIT_ORDER : R/W; bitpos: [24:23]; default: 0;
 *  Configures the bit order in read-data (MISO) state. \\
 *  0: MSB first \\
 *  1: LSB first \\
 *  Can be configured in CONF state.
 */
#define SPI_RD_BIT_ORDER    0x00000003U
#define SPI_RD_BIT_ORDER_M  (SPI_RD_BIT_ORDER_V << SPI_RD_BIT_ORDER_S)
#define SPI_RD_BIT_ORDER_V  0x00000003U
#define SPI_RD_BIT_ORDER_S  23
/** SPI_WR_BIT_ORDER : R/W; bitpos: [26:25]; default: 0;
 *  Configures the bit order in command (CMD), address (ADDR), and write-data (MOSI)
 *  states. \\
 *  0: MSB first \\
 *  1: LSB first \\
 *  Can be configured in CONF state.
 */
#define SPI_WR_BIT_ORDER    0x00000003U
#define SPI_WR_BIT_ORDER_M  (SPI_WR_BIT_ORDER_V << SPI_WR_BIT_ORDER_S)
#define SPI_WR_BIT_ORDER_V  0x00000003U
#define SPI_WR_BIT_ORDER_S  25

/** SPI_CLOCK_REG register
 *  SPI clock control register
 */
#define SPI_CLOCK_REG(i) (REG_SPI_BASE(i) + 0xc)
/** SPI_CLKCNT_L : R/W; bitpos: [5:0]; default: 3;
 *  In master transfer, this field must be equal to SPI_CLKCNT_N. In slave mode, it
 *  must be 0. Can be configured in CONF state.
 */
#define SPI_CLKCNT_L    0x0000003FU
#define SPI_CLKCNT_L_M  (SPI_CLKCNT_L_V << SPI_CLKCNT_L_S)
#define SPI_CLKCNT_L_V  0x0000003FU
#define SPI_CLKCNT_L_S  0
/** SPI_CLKCNT_H : R/W; bitpos: [11:6]; default: 1;
 *  Configures the duty cycle of SPI_CLK (high level) in master transfer.
 *  It's recommended to configure this value to floor((SPI_CLKCNT_N + 1)/2 - 1).
 *  floor() here is to round a number down, e.g., floor(2.2) = 2. In slave mode, it
 *  must be 0. \\
 *  Can be configured in CONF state.
 */
#define SPI_CLKCNT_H    0x0000003FU
#define SPI_CLKCNT_H_M  (SPI_CLKCNT_H_V << SPI_CLKCNT_H_S)
#define SPI_CLKCNT_H_V  0x0000003FU
#define SPI_CLKCNT_H_S  6
/** SPI_CLKCNT_N : R/W; bitpos: [17:12]; default: 3;
 *  Configures the divider of SPI_CLK in master transfer.
 *  SPI_CLK frequency is $f_{\textrm{apb_clk}}$/(SPI_CLKDIV_PRE + 1)/(SPI_CLKCNT_N +
 *  1). \\
 *  Can be configured in CONF state.
 */
#define SPI_CLKCNT_N    0x0000003FU
#define SPI_CLKCNT_N_M  (SPI_CLKCNT_N_V << SPI_CLKCNT_N_S)
#define SPI_CLKCNT_N_V  0x0000003FU
#define SPI_CLKCNT_N_S  12
/** SPI_CLKDIV_PRE : R/W; bitpos: [21:18]; default: 0;
 *  Configures the pre-divider of SPI_CLK in master transfer.
 *  Can be configured in CONF state.
 */
#define SPI_CLKDIV_PRE    0x0000000FU
#define SPI_CLKDIV_PRE_M  (SPI_CLKDIV_PRE_V << SPI_CLKDIV_PRE_S)
#define SPI_CLKDIV_PRE_V  0x0000000FU
#define SPI_CLKDIV_PRE_S  18
/** SPI_CLK_EQU_SYSCLK : R/W; bitpos: [31]; default: 1;
 *  Configures whether or not the SPI_CLK is equal to APB_CLK in master transfer.\\
 *  0: SPI_CLK is divided from APB_CLK.\\
 *  1: SPI_CLK is eqaul to APB_CLK.\\
 *  Can be configured in CONF state.
 */
#define SPI_CLK_EQU_SYSCLK    (BIT(31))
#define SPI_CLK_EQU_SYSCLK_M  (SPI_CLK_EQU_SYSCLK_V << SPI_CLK_EQU_SYSCLK_S)
#define SPI_CLK_EQU_SYSCLK_V  0x00000001U
#define SPI_CLK_EQU_SYSCLK_S  31

/** SPI_USER_REG register
 *  SPI USER control register
 */
#define SPI_USER_REG(i) (REG_SPI_BASE(i) + 0x10)
/** SPI_DOUTDIN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable full-duplex communication. \\
 *  0: Disable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_DOUTDIN    (BIT(0))
#define SPI_DOUTDIN_M  (SPI_DOUTDIN_V << SPI_DOUTDIN_S)
#define SPI_DOUTDIN_V  0x00000001U
#define SPI_DOUTDIN_S  0
/** SPI_QPI_MODE : R/W/SS/SC; bitpos: [3]; default: 0;
 *  Configures whether or not to enable QPI mode. \\
 *  0: Disable \\
 *  1: Enable \\
 *  This configuration is applicable when the SPI controller works as master or slave.
 *  Can be configured in CONF state.
 */
#define SPI_QPI_MODE    (BIT(3))
#define SPI_QPI_MODE_M  (SPI_QPI_MODE_V << SPI_QPI_MODE_S)
#define SPI_QPI_MODE_V  0x00000001U
#define SPI_QPI_MODE_S  3
/** SPI_OPI_MODE : HRO; bitpos: [4]; default: 0;
 *  Just for master mode. 1: spi controller is in OPI mode (all in 8-b-m). 0: others.
 *  Can be configured in CONF state.
 */
#define SPI_OPI_MODE    (BIT(4))
#define SPI_OPI_MODE_M  (SPI_OPI_MODE_V << SPI_OPI_MODE_S)
#define SPI_OPI_MODE_V  0x00000001U
#define SPI_OPI_MODE_S  4
/** SPI_TSCK_I_EDGE : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to change the polarity of TSCK in slave transfer. \\
 *  0: TSCK = SPI_CK_I \\
 *  1: TSCK = !SPI_CK_I \\
 */
#define SPI_TSCK_I_EDGE    (BIT(5))
#define SPI_TSCK_I_EDGE_M  (SPI_TSCK_I_EDGE_V << SPI_TSCK_I_EDGE_S)
#define SPI_TSCK_I_EDGE_V  0x00000001U
#define SPI_TSCK_I_EDGE_S  5
/** SPI_CS_HOLD : R/W; bitpos: [6]; default: 1;
 *  Configures whether or not to keep SPI CS low when SPI is in DONE state. \\
 *  0: Not keep low \\
 *  1: Keep low \\
 *  Can be configured in CONF state.
 */
#define SPI_CS_HOLD    (BIT(6))
#define SPI_CS_HOLD_M  (SPI_CS_HOLD_V << SPI_CS_HOLD_S)
#define SPI_CS_HOLD_V  0x00000001U
#define SPI_CS_HOLD_S  6
/** SPI_CS_SETUP : R/W; bitpos: [7]; default: 1;
 *  Configures whether or not to enable SPI CS when SPI is in prepare (PREP) state. \\
 *  0: Disable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_CS_SETUP    (BIT(7))
#define SPI_CS_SETUP_M  (SPI_CS_SETUP_V << SPI_CS_SETUP_S)
#define SPI_CS_SETUP_V  0x00000001U
#define SPI_CS_SETUP_S  7
/** SPI_RSCK_I_EDGE : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to change the polarity of RSCK in slave transfer. \\
 *  0: RSCK = !SPI_CK_I \\
 *  1: RSCK = SPI_CK_I \\
 */
#define SPI_RSCK_I_EDGE    (BIT(8))
#define SPI_RSCK_I_EDGE_M  (SPI_RSCK_I_EDGE_V << SPI_RSCK_I_EDGE_S)
#define SPI_RSCK_I_EDGE_V  0x00000001U
#define SPI_RSCK_I_EDGE_S  8
/** SPI_CK_OUT_EDGE : R/W; bitpos: [9]; default: 0;
 *  Configures SPI clock mode together with SPI_CK_IDLE_EDGE.
 *  Can be configured in CONF state. For more information, see Section <a href="GP-SPI2
 *  master clock control">link</a>.
 */
#define SPI_CK_OUT_EDGE    (BIT(9))
#define SPI_CK_OUT_EDGE_M  (SPI_CK_OUT_EDGE_V << SPI_CK_OUT_EDGE_S)
#define SPI_CK_OUT_EDGE_V  0x00000001U
#define SPI_CK_OUT_EDGE_S  9
/** SPI_FWRITE_DUAL : R/W; bitpos: [12]; default: 0;
 *  Configures whether or not to enable the 2-bit mode of read-data phase in write
 *  operations.\\
 *  0: Not enable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_FWRITE_DUAL    (BIT(12))
#define SPI_FWRITE_DUAL_M  (SPI_FWRITE_DUAL_V << SPI_FWRITE_DUAL_S)
#define SPI_FWRITE_DUAL_V  0x00000001U
#define SPI_FWRITE_DUAL_S  12
/** SPI_FWRITE_QUAD : R/W; bitpos: [13]; default: 0;
 *  Configures whether or not to enable the 4-bit mode of read-data phase in write
 *  operations. \\
 *  0: Not enable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_FWRITE_QUAD    (BIT(13))
#define SPI_FWRITE_QUAD_M  (SPI_FWRITE_QUAD_V << SPI_FWRITE_QUAD_S)
#define SPI_FWRITE_QUAD_V  0x00000001U
#define SPI_FWRITE_QUAD_S  13
/** SPI_FWRITE_OCT : HRO; bitpos: [14]; default: 0;
 *  In the write operations read-data phase apply 8 signals. Can be configured in CONF
 *  state.
 */
#define SPI_FWRITE_OCT    (BIT(14))
#define SPI_FWRITE_OCT_M  (SPI_FWRITE_OCT_V << SPI_FWRITE_OCT_S)
#define SPI_FWRITE_OCT_V  0x00000001U
#define SPI_FWRITE_OCT_S  14
/** SPI_USR_CONF_NXT : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable the CONF state for the next transaction
 *  (segment) in a configurable segmented transfer. \\
 *  0: this transfer will end after the current transaction (segment) is finished. Or
 *  this is not a configurable segmented transfer. \\
 *  1: this configurable segmented transfer will continue its next transaction
 *  (segment). \\
 *  Can be configured in CONF state.
 */
#define SPI_USR_CONF_NXT    (BIT(15))
#define SPI_USR_CONF_NXT_M  (SPI_USR_CONF_NXT_V << SPI_USR_CONF_NXT_S)
#define SPI_USR_CONF_NXT_V  0x00000001U
#define SPI_USR_CONF_NXT_S  15
/** SPI_SIO : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable 3-line half-duplex communication, where MOSI
 *  and MISO signals share the same pin.\\
 *  0: Disable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_SIO    (BIT(17))
#define SPI_SIO_M  (SPI_SIO_V << SPI_SIO_S)
#define SPI_SIO_V  0x00000001U
#define SPI_SIO_S  17
/** SPI_USR_MISO_HIGHPART : R/W; bitpos: [24]; default: 0;
 *  Configures whether or not to enable high part mode, i.e., only access to high part
 *  of the buffers: SPI_W8_REG ~ SPI_W15_REG in read-data phase. \\
 *  0: Disable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_USR_MISO_HIGHPART    (BIT(24))
#define SPI_USR_MISO_HIGHPART_M  (SPI_USR_MISO_HIGHPART_V << SPI_USR_MISO_HIGHPART_S)
#define SPI_USR_MISO_HIGHPART_V  0x00000001U
#define SPI_USR_MISO_HIGHPART_S  24
/** SPI_USR_MOSI_HIGHPART : R/W; bitpos: [25]; default: 0;
 *  Configures whether or not to enable high part mode, i.e., only access to high part
 *  of the buffers: SPI_W8_REG ~ SPI_W15_REG in write-data phase. \\
 *  0: Disable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_USR_MOSI_HIGHPART    (BIT(25))
#define SPI_USR_MOSI_HIGHPART_M  (SPI_USR_MOSI_HIGHPART_V << SPI_USR_MOSI_HIGHPART_S)
#define SPI_USR_MOSI_HIGHPART_V  0x00000001U
#define SPI_USR_MOSI_HIGHPART_S  25
/** SPI_USR_DUMMY_IDLE : R/W; bitpos: [26]; default: 0;
 *  Configures whether or not to disable SPI clock in DUMMY state. \\
 *  0: Not disable \\
 *  1: Disable \\
 *  Can be configured in CONF state.
 */
#define SPI_USR_DUMMY_IDLE    (BIT(26))
#define SPI_USR_DUMMY_IDLE_M  (SPI_USR_DUMMY_IDLE_V << SPI_USR_DUMMY_IDLE_S)
#define SPI_USR_DUMMY_IDLE_V  0x00000001U
#define SPI_USR_DUMMY_IDLE_S  26
/** SPI_USR_MOSI : R/W; bitpos: [27]; default: 0;
 *  Configures whether or not to enable the write-data (DOUT) state of an operation. \\
 *  0: Disable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_USR_MOSI    (BIT(27))
#define SPI_USR_MOSI_M  (SPI_USR_MOSI_V << SPI_USR_MOSI_S)
#define SPI_USR_MOSI_V  0x00000001U
#define SPI_USR_MOSI_S  27
/** SPI_USR_MISO : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to enable the read-data (DIN) state of an operation. \\
 *  0: Disable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_USR_MISO    (BIT(28))
#define SPI_USR_MISO_M  (SPI_USR_MISO_V << SPI_USR_MISO_S)
#define SPI_USR_MISO_V  0x00000001U
#define SPI_USR_MISO_S  28
/** SPI_USR_DUMMY : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to enable the DUMMY state of an operation. \\
 *  0: Disable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_USR_DUMMY    (BIT(29))
#define SPI_USR_DUMMY_M  (SPI_USR_DUMMY_V << SPI_USR_DUMMY_S)
#define SPI_USR_DUMMY_V  0x00000001U
#define SPI_USR_DUMMY_S  29
/** SPI_USR_ADDR : R/W; bitpos: [30]; default: 0;
 *  Configures whether or not to enable the address (ADDR) state of an operation. \\
 *  0: Disable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_USR_ADDR    (BIT(30))
#define SPI_USR_ADDR_M  (SPI_USR_ADDR_V << SPI_USR_ADDR_S)
#define SPI_USR_ADDR_V  0x00000001U
#define SPI_USR_ADDR_S  30
/** SPI_USR_COMMAND : R/W; bitpos: [31]; default: 1;
 *  Configures whether or not to enable the command (CMD) state of an operation. \\
 *  0: Disable \\
 *  1: Enable \\
 *  Can be configured in CONF state.
 */
#define SPI_USR_COMMAND    (BIT(31))
#define SPI_USR_COMMAND_M  (SPI_USR_COMMAND_V << SPI_USR_COMMAND_S)
#define SPI_USR_COMMAND_V  0x00000001U
#define SPI_USR_COMMAND_S  31

/** SPI_USER1_REG register
 *  SPI USER control register 1
 */
#define SPI_USER1_REG(i) (REG_SPI_BASE(i) + 0x14)
/** SPI_USR_DUMMY_CYCLELEN : R/W; bitpos: [7:0]; default: 7;
 *  Configures the length of DUMMY state.
 *  Measurement unit: SPI_CLK clock cycles.\\
 *  This value is (the expected cycle number - 1). Can be configured in CONF state.
 */
#define SPI_USR_DUMMY_CYCLELEN    0x000000FFU
#define SPI_USR_DUMMY_CYCLELEN_M  (SPI_USR_DUMMY_CYCLELEN_V << SPI_USR_DUMMY_CYCLELEN_S)
#define SPI_USR_DUMMY_CYCLELEN_V  0x000000FFU
#define SPI_USR_DUMMY_CYCLELEN_S  0
/** SPI_MST_WFULL_ERR_END_EN : R/W; bitpos: [16]; default: 1;
 *  Configures whether or not to end the SPI transfer when SPI RX AFIFO wfull error
 *  occurs in master full-/half-duplex transfers. \\
 *  0: Not end \\
 *  1: End \\
 */
#define SPI_MST_WFULL_ERR_END_EN    (BIT(16))
#define SPI_MST_WFULL_ERR_END_EN_M  (SPI_MST_WFULL_ERR_END_EN_V << SPI_MST_WFULL_ERR_END_EN_S)
#define SPI_MST_WFULL_ERR_END_EN_V  0x00000001U
#define SPI_MST_WFULL_ERR_END_EN_S  16
/** SPI_CS_SETUP_TIME : R/W; bitpos: [21:17]; default: 0;
 *  Configures the length of prepare (PREP) state.
 *  Measurement unit: SPI_CLK clock cycles.\\
 *  This value is equal to the expected cycles - 1. This field is used together with
 *  SPI_CS_SETUP. Can be configured in CONF state.
 */
#define SPI_CS_SETUP_TIME    0x0000001FU
#define SPI_CS_SETUP_TIME_M  (SPI_CS_SETUP_TIME_V << SPI_CS_SETUP_TIME_S)
#define SPI_CS_SETUP_TIME_V  0x0000001FU
#define SPI_CS_SETUP_TIME_S  17
/** SPI_CS_HOLD_TIME : R/W; bitpos: [26:22]; default: 1;
 *  Configures the delay cycles of CS pin.
 *  Measurement unit: SPI_CLK clock cycles. \\
 *  This field is used together with SPI_CS_HOLD. Can be configured in CONF state.
 */
#define SPI_CS_HOLD_TIME    0x0000001FU
#define SPI_CS_HOLD_TIME_M  (SPI_CS_HOLD_TIME_V << SPI_CS_HOLD_TIME_S)
#define SPI_CS_HOLD_TIME_V  0x0000001FU
#define SPI_CS_HOLD_TIME_S  22
/** SPI_USR_ADDR_BITLEN : R/W; bitpos: [31:27]; default: 23;
 *  Configures the bit length in address state.
 *  This value is (expected bit number - 1). Can be configured in CONF state.
 */
#define SPI_USR_ADDR_BITLEN    0x0000001FU
#define SPI_USR_ADDR_BITLEN_M  (SPI_USR_ADDR_BITLEN_V << SPI_USR_ADDR_BITLEN_S)
#define SPI_USR_ADDR_BITLEN_V  0x0000001FU
#define SPI_USR_ADDR_BITLEN_S  27

/** SPI_USER2_REG register
 *  SPI USER control register 2
 */
#define SPI_USER2_REG(i) (REG_SPI_BASE(i) + 0x18)
/** SPI_USR_COMMAND_VALUE : R/W; bitpos: [15:0]; default: 0;
 *  Configures the command value.
 *  Can be configured in CONF state.
 */
#define SPI_USR_COMMAND_VALUE    0x0000FFFFU
#define SPI_USR_COMMAND_VALUE_M  (SPI_USR_COMMAND_VALUE_V << SPI_USR_COMMAND_VALUE_S)
#define SPI_USR_COMMAND_VALUE_V  0x0000FFFFU
#define SPI_USR_COMMAND_VALUE_S  0
/** SPI_MST_REMPTY_ERR_END_EN : R/W; bitpos: [27]; default: 1;
 *  Configures whether or not to end the SPI transfer when SPI TX AFIFO read empty
 *  error occurs in master full-/half-duplex transfers. \\
 *  0: Not end \\
 *  1: End \\
 */
#define SPI_MST_REMPTY_ERR_END_EN    (BIT(27))
#define SPI_MST_REMPTY_ERR_END_EN_M  (SPI_MST_REMPTY_ERR_END_EN_V << SPI_MST_REMPTY_ERR_END_EN_S)
#define SPI_MST_REMPTY_ERR_END_EN_V  0x00000001U
#define SPI_MST_REMPTY_ERR_END_EN_S  27
/** SPI_USR_COMMAND_BITLEN : R/W; bitpos: [31:28]; default: 7;
 *  Configures the bit length of command state.
 *  This value is (expected bit number - 1). Can be configured in CONF state.
 */
#define SPI_USR_COMMAND_BITLEN    0x0000000FU
#define SPI_USR_COMMAND_BITLEN_M  (SPI_USR_COMMAND_BITLEN_V << SPI_USR_COMMAND_BITLEN_S)
#define SPI_USR_COMMAND_BITLEN_V  0x0000000FU
#define SPI_USR_COMMAND_BITLEN_S  28

/** SPI_MS_DLEN_REG register
 *  SPI data bit length control register
 */
#define SPI_MS_DLEN_REG(i) (REG_SPI_BASE(i) + 0x1c)
/** SPI_MS_DATA_BITLEN : R/W; bitpos: [17:0]; default: 0;
 *  Configures the data bit length of SPI transfer in DMA-controlled master transfer or
 *  in CPU-controlled master transfer. Or configures the bit length of SPI RX transfer
 *  in DMA-controlled slave transfer.
 *  This value shall be (expected bit_num - 1). Can be configured in CONF state.
 */
#define SPI_MS_DATA_BITLEN    0x0003FFFFU
#define SPI_MS_DATA_BITLEN_M  (SPI_MS_DATA_BITLEN_V << SPI_MS_DATA_BITLEN_S)
#define SPI_MS_DATA_BITLEN_V  0x0003FFFFU
#define SPI_MS_DATA_BITLEN_S  0

/** SPI_MISC_REG register
 *  SPI misc register
 */
#define SPI_MISC_REG(i) (REG_SPI_BASE(i) + 0x20)
/** SPI_CS0_DIS : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to disable SPI_CS$n pin.\\
 *  0: SPI_CS$n signal is from/to SPI_CS$n pin.\\
 *  1: Disable SPI_CS$n pin.\\
 *  Can be configured in CONF state.
 */
#define SPI_CS0_DIS    (BIT(0))
#define SPI_CS0_DIS_M  (SPI_CS0_DIS_V << SPI_CS0_DIS_S)
#define SPI_CS0_DIS_V  0x00000001U
#define SPI_CS0_DIS_S  0
/** SPI_CS1_DIS : R/W; bitpos: [1]; default: 1;
 *  Configures whether or not to disable SPI_CS$n pin.\\
 *  0: SPI_CS$n signal is from/to SPI_CS$n pin.\\
 *  1: Disable SPI_CS$n pin.\\
 *  Can be configured in CONF state.
 */
#define SPI_CS1_DIS    (BIT(1))
#define SPI_CS1_DIS_M  (SPI_CS1_DIS_V << SPI_CS1_DIS_S)
#define SPI_CS1_DIS_V  0x00000001U
#define SPI_CS1_DIS_S  1
/** SPI_CS2_DIS : R/W; bitpos: [2]; default: 1;
 *  Configures whether or not to disable SPI_CS$n pin.\\
 *  0: SPI_CS$n signal is from/to SPI_CS$n pin.\\
 *  1: Disable SPI_CS$n pin.\\
 *  Can be configured in CONF state.
 */
#define SPI_CS2_DIS    (BIT(2))
#define SPI_CS2_DIS_M  (SPI_CS2_DIS_V << SPI_CS2_DIS_S)
#define SPI_CS2_DIS_V  0x00000001U
#define SPI_CS2_DIS_S  2
/** SPI_CS3_DIS : R/W; bitpos: [3]; default: 1;
 *  Configures whether or not to disable SPI_CS$n pin.\\
 *  0: SPI_CS$n signal is from/to SPI_CS$n pin.\\
 *  1: Disable SPI_CS$n pin.\\
 *  Can be configured in CONF state.
 */
#define SPI_CS3_DIS    (BIT(3))
#define SPI_CS3_DIS_M  (SPI_CS3_DIS_V << SPI_CS3_DIS_S)
#define SPI_CS3_DIS_V  0x00000001U
#define SPI_CS3_DIS_S  3
/** SPI_CS4_DIS : R/W; bitpos: [4]; default: 1;
 *  Configures whether or not to disable SPI_CS$n pin.\\
 *  0: SPI_CS$n signal is from/to SPI_CS$n pin.\\
 *  1: Disable SPI_CS$n pin.\\
 *  Can be configured in CONF state.
 */
#define SPI_CS4_DIS    (BIT(4))
#define SPI_CS4_DIS_M  (SPI_CS4_DIS_V << SPI_CS4_DIS_S)
#define SPI_CS4_DIS_V  0x00000001U
#define SPI_CS4_DIS_S  4
/** SPI_CS5_DIS : R/W; bitpos: [5]; default: 1;
 *  Configures whether or not to disable SPI_CS$n pin.\\
 *  0: SPI_CS$n signal is from/to SPI_CS$n pin.\\
 *  1: Disable SPI_CS$n pin.\\
 *  Can be configured in CONF state.
 */
#define SPI_CS5_DIS    (BIT(5))
#define SPI_CS5_DIS_M  (SPI_CS5_DIS_V << SPI_CS5_DIS_S)
#define SPI_CS5_DIS_V  0x00000001U
#define SPI_CS5_DIS_S  5
/** SPI_CK_DIS : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to disable SPI_CLK output.\\
 *  0: Enable\\
 *  1: Disable\\
 *  Can be configured in CONF state.
 */
#define SPI_CK_DIS    (BIT(6))
#define SPI_CK_DIS_M  (SPI_CK_DIS_V << SPI_CK_DIS_S)
#define SPI_CK_DIS_V  0x00000001U
#define SPI_CK_DIS_S  6
/** SPI_MASTER_CS_POL : R/W; bitpos: [12:7]; default: 0;
 *  Configures the polarity of SPI_CS$n ($n = 0-5) line in master transfer.\\
 *  0: SPI_CS$n is low active.\\
 *  1: SPI_CS$n is high active.\\
 *  Can be configured in CONF state.
 */
#define SPI_MASTER_CS_POL    0x0000003FU
#define SPI_MASTER_CS_POL_M  (SPI_MASTER_CS_POL_V << SPI_MASTER_CS_POL_S)
#define SPI_MASTER_CS_POL_V  0x0000003FU
#define SPI_MASTER_CS_POL_S  7
/** SPI_CLK_DATA_DTR_EN : HRO; bitpos: [16]; default: 0;
 *  1: SPI master DTR mode is applied to SPI clk, data and spi_dqs.  0: SPI master DTR
 *  mode is  only applied to spi_dqs. This bit should be used with bit 17/18/19.
 */
#define SPI_CLK_DATA_DTR_EN    (BIT(16))
#define SPI_CLK_DATA_DTR_EN_M  (SPI_CLK_DATA_DTR_EN_V << SPI_CLK_DATA_DTR_EN_S)
#define SPI_CLK_DATA_DTR_EN_V  0x00000001U
#define SPI_CLK_DATA_DTR_EN_S  16
/** SPI_DATA_DTR_EN : HRO; bitpos: [17]; default: 0;
 *  1: SPI clk and data of SPI_DOUT and SPI_DIN state are in DTR mode, including master
 *  1/2/4/8-bm.  0:  SPI clk and data of SPI_DOUT and SPI_DIN state are in STR mode.
 *  Can be configured in CONF state.
 */
#define SPI_DATA_DTR_EN    (BIT(17))
#define SPI_DATA_DTR_EN_M  (SPI_DATA_DTR_EN_V << SPI_DATA_DTR_EN_S)
#define SPI_DATA_DTR_EN_V  0x00000001U
#define SPI_DATA_DTR_EN_S  17
/** SPI_ADDR_DTR_EN : HRO; bitpos: [18]; default: 0;
 *  1: SPI clk and data of SPI_SEND_ADDR state are in DTR mode, including master
 *  1/2/4/8-bm.  0:  SPI clk and data of SPI_SEND_ADDR state are in STR mode. Can be
 *  configured in CONF state.
 */
#define SPI_ADDR_DTR_EN    (BIT(18))
#define SPI_ADDR_DTR_EN_M  (SPI_ADDR_DTR_EN_V << SPI_ADDR_DTR_EN_S)
#define SPI_ADDR_DTR_EN_V  0x00000001U
#define SPI_ADDR_DTR_EN_S  18
/** SPI_CMD_DTR_EN : HRO; bitpos: [19]; default: 0;
 *  1: SPI clk and data of SPI_SEND_CMD state are in DTR mode, including master
 *  1/2/4/8-bm. 0:  SPI clk and data of SPI_SEND_CMD state are in STR mode. Can be
 *  configured in CONF state.
 */
#define SPI_CMD_DTR_EN    (BIT(19))
#define SPI_CMD_DTR_EN_M  (SPI_CMD_DTR_EN_V << SPI_CMD_DTR_EN_S)
#define SPI_CMD_DTR_EN_V  0x00000001U
#define SPI_CMD_DTR_EN_S  19
/** SPI_SLAVE_CS_POL : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not invert SPI slave input CS polarity.\\
 *  0: Not change\\
 *  1: Invert\\
 *  Can be configured in CONF state.
 */
#define SPI_SLAVE_CS_POL    (BIT(23))
#define SPI_SLAVE_CS_POL_M  (SPI_SLAVE_CS_POL_V << SPI_SLAVE_CS_POL_S)
#define SPI_SLAVE_CS_POL_V  0x00000001U
#define SPI_SLAVE_CS_POL_S  23
/** SPI_DQS_IDLE_EDGE : HRO; bitpos: [24]; default: 0;
 *  The default value of spi_dqs. Can be configured in CONF state.
 */
#define SPI_DQS_IDLE_EDGE    (BIT(24))
#define SPI_DQS_IDLE_EDGE_M  (SPI_DQS_IDLE_EDGE_V << SPI_DQS_IDLE_EDGE_S)
#define SPI_DQS_IDLE_EDGE_V  0x00000001U
#define SPI_DQS_IDLE_EDGE_S  24
/** SPI_CK_IDLE_EDGE : R/W; bitpos: [29]; default: 0;
 *  Configures the level of SPI_CLK line when GP-SPI2 is in idle.\\
 *  0: Low\\
 *  1: High\\
 *  Can be configured in CONF state.
 */
#define SPI_CK_IDLE_EDGE    (BIT(29))
#define SPI_CK_IDLE_EDGE_M  (SPI_CK_IDLE_EDGE_V << SPI_CK_IDLE_EDGE_S)
#define SPI_CK_IDLE_EDGE_V  0x00000001U
#define SPI_CK_IDLE_EDGE_S  29
/** SPI_CS_KEEP_ACTIVE : R/W; bitpos: [30]; default: 0;
 *  Configures whether or not to keep the SPI_CS line low.\\
 *  0: Not keep low\\
 *  1: Keep low\\
 *  Can be configured in CONF state.
 */
#define SPI_CS_KEEP_ACTIVE    (BIT(30))
#define SPI_CS_KEEP_ACTIVE_M  (SPI_CS_KEEP_ACTIVE_V << SPI_CS_KEEP_ACTIVE_S)
#define SPI_CS_KEEP_ACTIVE_V  0x00000001U
#define SPI_CS_KEEP_ACTIVE_S  30
/** SPI_QUAD_DIN_PIN_SWAP : R/W; bitpos: [31]; default: 0;
 *  1: SPI quad input swap enable, swap FSPID with FSPIQ, swap FSPIWP with FSPIHD. 0:
 *  spi quad input swap disable. Can be configured in CONF state.
 */
#define SPI_QUAD_DIN_PIN_SWAP    (BIT(31))
#define SPI_QUAD_DIN_PIN_SWAP_M  (SPI_QUAD_DIN_PIN_SWAP_V << SPI_QUAD_DIN_PIN_SWAP_S)
#define SPI_QUAD_DIN_PIN_SWAP_V  0x00000001U
#define SPI_QUAD_DIN_PIN_SWAP_S  31

/** SPI_DIN_MODE_REG register
 *  SPI input delay mode configuration
 */
#define SPI_DIN_MODE_REG(i) (REG_SPI_BASE(i) + 0x24)
/** SPI_DIN0_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the input mode for FSPID signal.\\
 *  0: Input without delay\\
 *  1: Input at the (SPI_DIN0_NUM + 1)th falling edge of clk_spi_mst\\
 *  2: Input at the (SPI_DIN0_NUM + 1)th rising edge of clk_hclk plus one clk_spi_mst
 *  rising edge cycle\\
 *  3: Input at the (SPI_DIN0_NUM + 1)th rising edge of clk_hclk plus one clk_spi_mst
 *  falling edge cycle\\
 *  Can be configured in CONF state.
 */
#define SPI_DIN0_MODE    0x00000003U
#define SPI_DIN0_MODE_M  (SPI_DIN0_MODE_V << SPI_DIN0_MODE_S)
#define SPI_DIN0_MODE_V  0x00000003U
#define SPI_DIN0_MODE_S  0
/** SPI_DIN1_MODE : R/W; bitpos: [3:2]; default: 0;
 *  Configures the input mode for FSPIQ signal.\\
 *  0: Input without delay\\
 *  1: Input at the (SPI_DIN1_NUM+1)th falling edge of clk_spi_mst\\
 *  2: Input at the (SPI_DIN1_NUM + 1)th rising edge of clk_hclk plus one clk_spi_mst
 *  rising edge cycle\\
 *  3: Input at the (SPI_DIN1_NUM + 1)th rising edge of clk_hclk plus one clk_spi_mst
 *  falling edge cycle\\
 *  Can be configured in CONF state.
 */
#define SPI_DIN1_MODE    0x00000003U
#define SPI_DIN1_MODE_M  (SPI_DIN1_MODE_V << SPI_DIN1_MODE_S)
#define SPI_DIN1_MODE_V  0x00000003U
#define SPI_DIN1_MODE_S  2
/** SPI_DIN2_MODE : R/W; bitpos: [5:4]; default: 0;
 *  Configures the input mode for FSPIWP signal.\\
 *  0: Input without delay\\
 *  1: Input at the (SPI_DIN2_NUM + 1)th falling edge of clk_spi_mst\\
 *  2: Input at the (SPI_DIN2_NUM + 1)th rising edge of clk_hclk plus one clk_spi_mst
 *  rising edge cycle\\
 *  3: Input at the (SPI_DIN2_NUM + 1)th rising edge of clk_hclk plus one clk_spi_mst
 *  falling edge cycle\\
 *  Can be configured in CONF state.
 */
#define SPI_DIN2_MODE    0x00000003U
#define SPI_DIN2_MODE_M  (SPI_DIN2_MODE_V << SPI_DIN2_MODE_S)
#define SPI_DIN2_MODE_V  0x00000003U
#define SPI_DIN2_MODE_S  4
/** SPI_DIN3_MODE : R/W; bitpos: [7:6]; default: 0;
 *  Configures the input mode for FSPIHD signal.\\
 *  0: Input without delay\\
 *  1: Input at the (SPI_DIN3_NUM + 1)th falling edge of clk_spi_mst\\
 *  2: Input at the (SPI_DIN3_NUM + 1)th rising edge of clk_hclk plus one clk_spi_mst
 *  rising edge cycle\\
 *  3: Input at the (SPI_DIN3_NUM + 1)th rising edge of clk_hclk plus one clk_spi_mst
 *  falling edge cycle\\
 *  Can be configured in CONF state.
 *
 */
#define SPI_DIN3_MODE    0x00000003U
#define SPI_DIN3_MODE_M  (SPI_DIN3_MODE_V << SPI_DIN3_MODE_S)
#define SPI_DIN3_MODE_V  0x00000003U
#define SPI_DIN3_MODE_S  6
/** SPI_DIN4_MODE : HRO; bitpos: [9:8]; default: 0;
 *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
 *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
 *  with the spi_clk. Can be configured in CONF state.
 */
#define SPI_DIN4_MODE    0x00000003U
#define SPI_DIN4_MODE_M  (SPI_DIN4_MODE_V << SPI_DIN4_MODE_S)
#define SPI_DIN4_MODE_V  0x00000003U
#define SPI_DIN4_MODE_S  8
/** SPI_DIN5_MODE : HRO; bitpos: [11:10]; default: 0;
 *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
 *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
 *  with the spi_clk. Can be configured in CONF state.
 */
#define SPI_DIN5_MODE    0x00000003U
#define SPI_DIN5_MODE_M  (SPI_DIN5_MODE_V << SPI_DIN5_MODE_S)
#define SPI_DIN5_MODE_V  0x00000003U
#define SPI_DIN5_MODE_S  10
/** SPI_DIN6_MODE : HRO; bitpos: [13:12]; default: 0;
 *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
 *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
 *  with the spi_clk. Can be configured in CONF state.
 */
#define SPI_DIN6_MODE    0x00000003U
#define SPI_DIN6_MODE_M  (SPI_DIN6_MODE_V << SPI_DIN6_MODE_S)
#define SPI_DIN6_MODE_V  0x00000003U
#define SPI_DIN6_MODE_S  12
/** SPI_DIN7_MODE : HRO; bitpos: [15:14]; default: 0;
 *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
 *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
 *  with the spi_clk. Can be configured in CONF state.
 */
#define SPI_DIN7_MODE    0x00000003U
#define SPI_DIN7_MODE_M  (SPI_DIN7_MODE_V << SPI_DIN7_MODE_S)
#define SPI_DIN7_MODE_V  0x00000003U
#define SPI_DIN7_MODE_S  14
/** SPI_TIMING_HCLK_ACTIVE : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable HCLK (high-frequency clock) in SPI input timing
 *  module.\\
 *  0: Disable\\
 *  1: Enable\\
 *  Can be configured in CONF state.
 */
#define SPI_TIMING_HCLK_ACTIVE    (BIT(16))
#define SPI_TIMING_HCLK_ACTIVE_M  (SPI_TIMING_HCLK_ACTIVE_V << SPI_TIMING_HCLK_ACTIVE_S)
#define SPI_TIMING_HCLK_ACTIVE_V  0x00000001U
#define SPI_TIMING_HCLK_ACTIVE_S  16

/** SPI_DIN_NUM_REG register
 *  SPI input delay number configuration
 */
#define SPI_DIN_NUM_REG(i) (REG_SPI_BASE(i) + 0x28)
/** SPI_DIN0_NUM : R/W; bitpos: [1:0]; default: 0;
 *  Configures the delays to input signal FSPID based on the setting of SPI_DIN0_MODE.\\
 *  0: Delayed by 1 clock cycle\\
 *  1: Delayed by 2 clock cycles\\
 *  2: Delayed by 3 clock cycles\\
 *  3: Delayed by 4 clock cycles\\
 *  Can be configured in CONF state.
 */
#define SPI_DIN0_NUM    0x00000003U
#define SPI_DIN0_NUM_M  (SPI_DIN0_NUM_V << SPI_DIN0_NUM_S)
#define SPI_DIN0_NUM_V  0x00000003U
#define SPI_DIN0_NUM_S  0
/** SPI_DIN1_NUM : R/W; bitpos: [3:2]; default: 0;
 *  Configures the delays to input signal FSPIQ based on the setting of SPI_DIN1_MODE.\\
 *  0: Delayed by 1 clock cycle\\
 *  1: Delayed by 2 clock cycles\\
 *  2: Delayed by 3 clock cycles\\
 *  3: Delayed by 4 clock cycles\\
 *  Can be configured in CONF state.
 */
#define SPI_DIN1_NUM    0x00000003U
#define SPI_DIN1_NUM_M  (SPI_DIN1_NUM_V << SPI_DIN1_NUM_S)
#define SPI_DIN1_NUM_V  0x00000003U
#define SPI_DIN1_NUM_S  2
/** SPI_DIN2_NUM : R/W; bitpos: [5:4]; default: 0;
 *  Configures the delays to input signal FSPIWP based on the setting of
 *  SPI_DIN2_MODE.\\
 *  0: Delayed by 1 clock cycle\\
 *  1: Delayed by 2 clock cycles\\
 *  2: Delayed by 3 clock cycles\\
 *  3: Delayed by 4 clock cycles\\
 *  Can be configured in CONF state.
 */
#define SPI_DIN2_NUM    0x00000003U
#define SPI_DIN2_NUM_M  (SPI_DIN2_NUM_V << SPI_DIN2_NUM_S)
#define SPI_DIN2_NUM_V  0x00000003U
#define SPI_DIN2_NUM_S  4
/** SPI_DIN3_NUM : R/W; bitpos: [7:6]; default: 0;
 *  Configures the delays to input signal FSPIHD based on the setting of
 *  SPI_DIN3_MODE.\\
 *  0: Delayed by 1 clock cycle\\
 *  1: Delayed by 2 clock cycles\\
 *  2: Delayed by 3 clock cycles\\
 *  3: Delayed by 4 clock cycles\\
 *  Can be configured in CONF state.
 */
#define SPI_DIN3_NUM    0x00000003U
#define SPI_DIN3_NUM_M  (SPI_DIN3_NUM_V << SPI_DIN3_NUM_S)
#define SPI_DIN3_NUM_V  0x00000003U
#define SPI_DIN3_NUM_S  6
/** SPI_DIN4_NUM : HRO; bitpos: [9:8]; default: 0;
 *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
 *  delayed by 2 cycles,...  Can be configured in CONF state.
 */
#define SPI_DIN4_NUM    0x00000003U
#define SPI_DIN4_NUM_M  (SPI_DIN4_NUM_V << SPI_DIN4_NUM_S)
#define SPI_DIN4_NUM_V  0x00000003U
#define SPI_DIN4_NUM_S  8
/** SPI_DIN5_NUM : HRO; bitpos: [11:10]; default: 0;
 *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
 *  delayed by 2 cycles,...  Can be configured in CONF state.
 */
#define SPI_DIN5_NUM    0x00000003U
#define SPI_DIN5_NUM_M  (SPI_DIN5_NUM_V << SPI_DIN5_NUM_S)
#define SPI_DIN5_NUM_V  0x00000003U
#define SPI_DIN5_NUM_S  10
/** SPI_DIN6_NUM : HRO; bitpos: [13:12]; default: 0;
 *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
 *  delayed by 2 cycles,...  Can be configured in CONF state.
 */
#define SPI_DIN6_NUM    0x00000003U
#define SPI_DIN6_NUM_M  (SPI_DIN6_NUM_V << SPI_DIN6_NUM_S)
#define SPI_DIN6_NUM_V  0x00000003U
#define SPI_DIN6_NUM_S  12
/** SPI_DIN7_NUM : HRO; bitpos: [15:14]; default: 0;
 *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
 *  delayed by 2 cycles,...  Can be configured in CONF state.
 */
#define SPI_DIN7_NUM    0x00000003U
#define SPI_DIN7_NUM_M  (SPI_DIN7_NUM_V << SPI_DIN7_NUM_S)
#define SPI_DIN7_NUM_V  0x00000003U
#define SPI_DIN7_NUM_S  14

/** SPI_DOUT_MODE_REG register
 *  SPI output delay mode configuration
 */
#define SPI_DOUT_MODE_REG(i) (REG_SPI_BASE(i) + 0x2c)
/** SPI_DOUT0_MODE : R/W; bitpos: [0]; default: 0;
 *  Configures the output mode for FSPID signal.\\
 *  0: Output without delay\\
 *  1: Output with a delay of a SPI module clock cycle at its falling edge\\
 *  Can be configured in CONF state.
 */
#define SPI_DOUT0_MODE    (BIT(0))
#define SPI_DOUT0_MODE_M  (SPI_DOUT0_MODE_V << SPI_DOUT0_MODE_S)
#define SPI_DOUT0_MODE_V  0x00000001U
#define SPI_DOUT0_MODE_S  0
/** SPI_DOUT1_MODE : R/W; bitpos: [1]; default: 0;
 *  Configures the output mode for FSPIQ signal.\\
 *  0: Output without delay\\
 *  1: Output with a delay of a SPI module clock cycle at its falling edge\\
 *  Can be configured in CONF state.
 */
#define SPI_DOUT1_MODE    (BIT(1))
#define SPI_DOUT1_MODE_M  (SPI_DOUT1_MODE_V << SPI_DOUT1_MODE_S)
#define SPI_DOUT1_MODE_V  0x00000001U
#define SPI_DOUT1_MODE_S  1
/** SPI_DOUT2_MODE : R/W; bitpos: [2]; default: 0;
 *  Configures the output mode for FSPIWP signal.\\
 *  0: Output without delay\\
 *  1: Output with a delay of a SPI module clock cycle at its falling edge\\
 *  Can be configured in CONF state.
 */
#define SPI_DOUT2_MODE    (BIT(2))
#define SPI_DOUT2_MODE_M  (SPI_DOUT2_MODE_V << SPI_DOUT2_MODE_S)
#define SPI_DOUT2_MODE_V  0x00000001U
#define SPI_DOUT2_MODE_S  2
/** SPI_DOUT3_MODE : R/W; bitpos: [3]; default: 0;
 *  Configures the output mode for FSPIHD signal.\\
 *  0: Output without delay\\
 *  1: Output with a delay of a SPI module clock cycle at its falling edge\\
 *  Can be configured in CONF state.
 */
#define SPI_DOUT3_MODE    (BIT(3))
#define SPI_DOUT3_MODE_M  (SPI_DOUT3_MODE_V << SPI_DOUT3_MODE_S)
#define SPI_DOUT3_MODE_V  0x00000001U
#define SPI_DOUT3_MODE_S  3
/** SPI_DOUT4_MODE : HRO; bitpos: [4]; default: 0;
 *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
 *  1: output delay for a SPI module clock cycle at its negative edge. Can be
 *  configured in CONF state.
 */
#define SPI_DOUT4_MODE    (BIT(4))
#define SPI_DOUT4_MODE_M  (SPI_DOUT4_MODE_V << SPI_DOUT4_MODE_S)
#define SPI_DOUT4_MODE_V  0x00000001U
#define SPI_DOUT4_MODE_S  4
/** SPI_DOUT5_MODE : HRO; bitpos: [5]; default: 0;
 *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
 *  1: output delay for a SPI module clock cycle at its negative edge. Can be
 *  configured in CONF state.
 */
#define SPI_DOUT5_MODE    (BIT(5))
#define SPI_DOUT5_MODE_M  (SPI_DOUT5_MODE_V << SPI_DOUT5_MODE_S)
#define SPI_DOUT5_MODE_V  0x00000001U
#define SPI_DOUT5_MODE_S  5
/** SPI_DOUT6_MODE : HRO; bitpos: [6]; default: 0;
 *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
 *  1: output delay for a SPI module clock cycle at its negative edge. Can be
 *  configured in CONF state.
 */
#define SPI_DOUT6_MODE    (BIT(6))
#define SPI_DOUT6_MODE_M  (SPI_DOUT6_MODE_V << SPI_DOUT6_MODE_S)
#define SPI_DOUT6_MODE_V  0x00000001U
#define SPI_DOUT6_MODE_S  6
/** SPI_DOUT7_MODE : HRO; bitpos: [7]; default: 0;
 *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
 *  1: output delay for a SPI module clock cycle at its negative edge. Can be
 *  configured in CONF state.
 */
#define SPI_DOUT7_MODE    (BIT(7))
#define SPI_DOUT7_MODE_M  (SPI_DOUT7_MODE_V << SPI_DOUT7_MODE_S)
#define SPI_DOUT7_MODE_V  0x00000001U
#define SPI_DOUT7_MODE_S  7
/** SPI_D_DQS_MODE : HRO; bitpos: [8]; default: 0;
 *  The output signal SPI_DQS is delayed by the SPI module clock, 0: output without
 *  delayed, 1: output delay for a SPI module clock cycle at its negative edge. Can be
 *  configured in CONF state.
 */
#define SPI_D_DQS_MODE    (BIT(8))
#define SPI_D_DQS_MODE_M  (SPI_D_DQS_MODE_V << SPI_D_DQS_MODE_S)
#define SPI_D_DQS_MODE_V  0x00000001U
#define SPI_D_DQS_MODE_S  8

/** SPI_DMA_CONF_REG register
 *  SPI DMA control register
 */
#define SPI_DMA_CONF_REG(i) (REG_SPI_BASE(i) + 0x30)
/** SPI_DMA_OUTFIFO_EMPTY : RO; bitpos: [0]; default: 1;
 *  Represents whether or not the DMA TX FIFO is ready for sending data.\\
 *  0: Ready\\
 *  1: Not ready\\
 */
#define SPI_DMA_OUTFIFO_EMPTY    (BIT(0))
#define SPI_DMA_OUTFIFO_EMPTY_M  (SPI_DMA_OUTFIFO_EMPTY_V << SPI_DMA_OUTFIFO_EMPTY_S)
#define SPI_DMA_OUTFIFO_EMPTY_V  0x00000001U
#define SPI_DMA_OUTFIFO_EMPTY_S  0
/** SPI_DMA_INFIFO_FULL : RO; bitpos: [1]; default: 1;
 *  Represents whether or not the DMA RX FIFO is ready for receiving data.\\
 *  0: Ready\\
 *  1: Not ready\\
 */
#define SPI_DMA_INFIFO_FULL    (BIT(1))
#define SPI_DMA_INFIFO_FULL_M  (SPI_DMA_INFIFO_FULL_V << SPI_DMA_INFIFO_FULL_S)
#define SPI_DMA_INFIFO_FULL_V  0x00000001U
#define SPI_DMA_INFIFO_FULL_S  1
/** SPI_DMA_SLV_SEG_TRANS_EN : R/W; bitpos: [18]; default: 0;
 *  Configures whether or not to enable DMA-controlled segmented transfer in slave
 *  half-duplex communication.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define SPI_DMA_SLV_SEG_TRANS_EN    (BIT(18))
#define SPI_DMA_SLV_SEG_TRANS_EN_M  (SPI_DMA_SLV_SEG_TRANS_EN_V << SPI_DMA_SLV_SEG_TRANS_EN_S)
#define SPI_DMA_SLV_SEG_TRANS_EN_V  0x00000001U
#define SPI_DMA_SLV_SEG_TRANS_EN_S  18
/** SPI_SLV_RX_SEG_TRANS_CLR_EN : R/W; bitpos: [19]; default: 0;
 *  In slave segmented transfer, if the size of the DMA RX buffer is smaller than the
 *  size of the received data, \\1: the data in all the following Wr_DMA transactions
 *  will not be received\\ 0: the data in this Wr_DMA transaction will not be received,
 *  but in the following transactions,\\
 *
 *  - if the size of DMA RX buffer is not 0, the data in following Wr_DMA transactions
 *  will be received.
 *  - if the size of DMA RX buffer is 0, the data in following Wr_DMA transactions will
 *  not be received.
 */
#define SPI_SLV_RX_SEG_TRANS_CLR_EN    (BIT(19))
#define SPI_SLV_RX_SEG_TRANS_CLR_EN_M  (SPI_SLV_RX_SEG_TRANS_CLR_EN_V << SPI_SLV_RX_SEG_TRANS_CLR_EN_S)
#define SPI_SLV_RX_SEG_TRANS_CLR_EN_V  0x00000001U
#define SPI_SLV_RX_SEG_TRANS_CLR_EN_S  19
/** SPI_SLV_TX_SEG_TRANS_CLR_EN : R/W; bitpos: [20]; default: 0;
 *  In slave segmented transfer, if the size of the DMA TX buffer is smaller than the
 *  size of the transmitted data,\\
 *  1: the data in the following transactions will not be updated, i.e. the old data is
 *  transmitted repeatedly.\\
 *  0: the data in this transaction will not be updated. But in the following
 *  transactions,\\
 *
 *  - if new data is filled in DMA TX FIFO, new data will be transmitted.
 *  - if no new data is filled in DMA TX FIFO, no new data will be transmitted.
 */
#define SPI_SLV_TX_SEG_TRANS_CLR_EN    (BIT(20))
#define SPI_SLV_TX_SEG_TRANS_CLR_EN_M  (SPI_SLV_TX_SEG_TRANS_CLR_EN_V << SPI_SLV_TX_SEG_TRANS_CLR_EN_S)
#define SPI_SLV_TX_SEG_TRANS_CLR_EN_V  0x00000001U
#define SPI_SLV_TX_SEG_TRANS_CLR_EN_S  20
/** SPI_RX_EOF_EN : R/W; bitpos: [21]; default: 0;
 *  1: In a DAM-controlled transfer, if the bit number of transferred data is equal to
 *  (SPI_MS_DATA_BITLEN + 1), then GDMA_IN_SUC_EOF_CH\it{n}_INT_RAW will be set by
 *  hardware. 0: GDMA_IN_SUC_EOF_CH\it{n}_INT_RAW is set by SPI_TRANS_DONE_INT event in
 *  a single transfer, or by an SPI_DMA_SEG_TRANS_DONE_INT event in a segmented
 *  transfer.
 */
#define SPI_RX_EOF_EN    (BIT(21))
#define SPI_RX_EOF_EN_M  (SPI_RX_EOF_EN_V << SPI_RX_EOF_EN_S)
#define SPI_RX_EOF_EN_V  0x00000001U
#define SPI_RX_EOF_EN_S  21
/** SPI_DMA_RX_ENA : R/W; bitpos: [27]; default: 0;
 *  Configures whether or not to enable DMA-controlled receive data transfer.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define SPI_DMA_RX_ENA    (BIT(27))
#define SPI_DMA_RX_ENA_M  (SPI_DMA_RX_ENA_V << SPI_DMA_RX_ENA_S)
#define SPI_DMA_RX_ENA_V  0x00000001U
#define SPI_DMA_RX_ENA_S  27
/** SPI_DMA_TX_ENA : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to enable DMA-controlled send data transfer.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define SPI_DMA_TX_ENA    (BIT(28))
#define SPI_DMA_TX_ENA_M  (SPI_DMA_TX_ENA_V << SPI_DMA_TX_ENA_S)
#define SPI_DMA_TX_ENA_V  0x00000001U
#define SPI_DMA_TX_ENA_S  28
/** SPI_RX_AFIFO_RST : WT; bitpos: [29]; default: 0;
 *  Configures whether or not to reset spi_rx_afifo as shown in Figure <a
 *  href="fig:spi-master-data-flow-control">link</a> and in Figure  <a
 *  href="fig:spi-slave-data-flow-control">link</a>.\\
 *  0: Not reset\\
 *  1: Reset\\
 *  spi_rx_afifo is used to receive data in SPI master and slave transfer.
 */
#define SPI_RX_AFIFO_RST    (BIT(29))
#define SPI_RX_AFIFO_RST_M  (SPI_RX_AFIFO_RST_V << SPI_RX_AFIFO_RST_S)
#define SPI_RX_AFIFO_RST_V  0x00000001U
#define SPI_RX_AFIFO_RST_S  29
/** SPI_BUF_AFIFO_RST : WT; bitpos: [30]; default: 0;
 *  Configures whether or not to reset buf_tx_afifo as shown in Figure <a
 *  href="fig:spi-master-data-flow-control">link</a> and in Figure  <a
 *  href="fig:spi-slave-data-flow-control">link</a>.\\
 *  0: Not reset\\
 *  1: Reset\\
 *  buf_tx_afifo is used to send data out in CPU-controlled master and slave transfer.
 */
#define SPI_BUF_AFIFO_RST    (BIT(30))
#define SPI_BUF_AFIFO_RST_M  (SPI_BUF_AFIFO_RST_V << SPI_BUF_AFIFO_RST_S)
#define SPI_BUF_AFIFO_RST_V  0x00000001U
#define SPI_BUF_AFIFO_RST_S  30
/** SPI_DMA_AFIFO_RST : WT; bitpos: [31]; default: 0;
 *  Configures whether or not to reset dma_tx_afifo as shown in Figure <a
 *  href="fig:spi-master-data-flow-control">link</a> and in Figure  <a
 *  href="fig:spi-slave-data-flow-control">link</a>.\\
 *  0: Not reset\\
 *  1: Reset\\
 *  dma_tx_afifo is used to send data out in DMA-controlled slave transfer.
 */
#define SPI_DMA_AFIFO_RST    (BIT(31))
#define SPI_DMA_AFIFO_RST_M  (SPI_DMA_AFIFO_RST_V << SPI_DMA_AFIFO_RST_S)
#define SPI_DMA_AFIFO_RST_V  0x00000001U
#define SPI_DMA_AFIFO_RST_S  31

/** SPI_DMA_INT_ENA_REG register
 *  SPI interrupt enable register
 */
#define SPI_DMA_INT_ENA_REG(i) (REG_SPI_BASE(i) + 0x34)
/** SPI_DMA_INFIFO_FULL_ERR_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Write 1 to enable SPI_DMA_INFIFO_FULL_ERR_INT interrupt.
 */
#define SPI_DMA_INFIFO_FULL_ERR_INT_ENA    (BIT(0))
#define SPI_DMA_INFIFO_FULL_ERR_INT_ENA_M  (SPI_DMA_INFIFO_FULL_ERR_INT_ENA_V << SPI_DMA_INFIFO_FULL_ERR_INT_ENA_S)
#define SPI_DMA_INFIFO_FULL_ERR_INT_ENA_V  0x00000001U
#define SPI_DMA_INFIFO_FULL_ERR_INT_ENA_S  0
/** SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  Write 1 to enable SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt.
 */
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ENA    (BIT(1))
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ENA_M  (SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ENA_V << SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ENA_S)
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ENA_V  0x00000001U
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ENA_S  1
/** SPI_SLV_EX_QPI_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  Write 1 to enable SPI_SLV_EX_QPI_INT interrupt.
 */
#define SPI_SLV_EX_QPI_INT_ENA    (BIT(2))
#define SPI_SLV_EX_QPI_INT_ENA_M  (SPI_SLV_EX_QPI_INT_ENA_V << SPI_SLV_EX_QPI_INT_ENA_S)
#define SPI_SLV_EX_QPI_INT_ENA_V  0x00000001U
#define SPI_SLV_EX_QPI_INT_ENA_S  2
/** SPI_SLV_EN_QPI_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  Write 1 to enable SPI_SLV_EN_QPI_INT interrupt.
 */
#define SPI_SLV_EN_QPI_INT_ENA    (BIT(3))
#define SPI_SLV_EN_QPI_INT_ENA_M  (SPI_SLV_EN_QPI_INT_ENA_V << SPI_SLV_EN_QPI_INT_ENA_S)
#define SPI_SLV_EN_QPI_INT_ENA_V  0x00000001U
#define SPI_SLV_EN_QPI_INT_ENA_S  3
/** SPI_SLV_CMD7_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  Write 1 to enable SPI_SLV_CMD7_INT interrupt.
 */
#define SPI_SLV_CMD7_INT_ENA    (BIT(4))
#define SPI_SLV_CMD7_INT_ENA_M  (SPI_SLV_CMD7_INT_ENA_V << SPI_SLV_CMD7_INT_ENA_S)
#define SPI_SLV_CMD7_INT_ENA_V  0x00000001U
#define SPI_SLV_CMD7_INT_ENA_S  4
/** SPI_SLV_CMD8_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  Write 1 to enable SPI_SLV_CMD8_INT interrupt.
 */
#define SPI_SLV_CMD8_INT_ENA    (BIT(5))
#define SPI_SLV_CMD8_INT_ENA_M  (SPI_SLV_CMD8_INT_ENA_V << SPI_SLV_CMD8_INT_ENA_S)
#define SPI_SLV_CMD8_INT_ENA_V  0x00000001U
#define SPI_SLV_CMD8_INT_ENA_S  5
/** SPI_SLV_CMD9_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  Write 1 to enable SPI_SLV_CMD9_INT interrupt.
 */
#define SPI_SLV_CMD9_INT_ENA    (BIT(6))
#define SPI_SLV_CMD9_INT_ENA_M  (SPI_SLV_CMD9_INT_ENA_V << SPI_SLV_CMD9_INT_ENA_S)
#define SPI_SLV_CMD9_INT_ENA_V  0x00000001U
#define SPI_SLV_CMD9_INT_ENA_S  6
/** SPI_SLV_CMDA_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  Write 1 to enable SPI_SLV_CMDA_INT interrupt.
 */
#define SPI_SLV_CMDA_INT_ENA    (BIT(7))
#define SPI_SLV_CMDA_INT_ENA_M  (SPI_SLV_CMDA_INT_ENA_V << SPI_SLV_CMDA_INT_ENA_S)
#define SPI_SLV_CMDA_INT_ENA_V  0x00000001U
#define SPI_SLV_CMDA_INT_ENA_S  7
/** SPI_SLV_RD_DMA_DONE_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  Write 1 to enable SPI_SLV_RD_DMA_DONE_INT interrupt.
 */
#define SPI_SLV_RD_DMA_DONE_INT_ENA    (BIT(8))
#define SPI_SLV_RD_DMA_DONE_INT_ENA_M  (SPI_SLV_RD_DMA_DONE_INT_ENA_V << SPI_SLV_RD_DMA_DONE_INT_ENA_S)
#define SPI_SLV_RD_DMA_DONE_INT_ENA_V  0x00000001U
#define SPI_SLV_RD_DMA_DONE_INT_ENA_S  8
/** SPI_SLV_WR_DMA_DONE_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  Write 1 to enable SPI_SLV_WR_DMA_DONE_INT interrupt.
 */
#define SPI_SLV_WR_DMA_DONE_INT_ENA    (BIT(9))
#define SPI_SLV_WR_DMA_DONE_INT_ENA_M  (SPI_SLV_WR_DMA_DONE_INT_ENA_V << SPI_SLV_WR_DMA_DONE_INT_ENA_S)
#define SPI_SLV_WR_DMA_DONE_INT_ENA_V  0x00000001U
#define SPI_SLV_WR_DMA_DONE_INT_ENA_S  9
/** SPI_SLV_RD_BUF_DONE_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  Write 1 to enable SPI_SLV_RD_BUF_DONE_INT interrupt.
 */
#define SPI_SLV_RD_BUF_DONE_INT_ENA    (BIT(10))
#define SPI_SLV_RD_BUF_DONE_INT_ENA_M  (SPI_SLV_RD_BUF_DONE_INT_ENA_V << SPI_SLV_RD_BUF_DONE_INT_ENA_S)
#define SPI_SLV_RD_BUF_DONE_INT_ENA_V  0x00000001U
#define SPI_SLV_RD_BUF_DONE_INT_ENA_S  10
/** SPI_SLV_WR_BUF_DONE_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  Write 1 to enable SPI_SLV_WR_BUF_DONE_INT interrupt.
 */
#define SPI_SLV_WR_BUF_DONE_INT_ENA    (BIT(11))
#define SPI_SLV_WR_BUF_DONE_INT_ENA_M  (SPI_SLV_WR_BUF_DONE_INT_ENA_V << SPI_SLV_WR_BUF_DONE_INT_ENA_S)
#define SPI_SLV_WR_BUF_DONE_INT_ENA_V  0x00000001U
#define SPI_SLV_WR_BUF_DONE_INT_ENA_S  11
/** SPI_TRANS_DONE_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  Write 1 to enable SPI_TRANS_DONE_INT interrupt.
 */
#define SPI_TRANS_DONE_INT_ENA    (BIT(12))
#define SPI_TRANS_DONE_INT_ENA_M  (SPI_TRANS_DONE_INT_ENA_V << SPI_TRANS_DONE_INT_ENA_S)
#define SPI_TRANS_DONE_INT_ENA_V  0x00000001U
#define SPI_TRANS_DONE_INT_ENA_S  12
/** SPI_DMA_SEG_TRANS_DONE_INT_ENA : R/W; bitpos: [13]; default: 0;
 *  Write 1 to enable SPI_DMA_SEG_TRANS_DONE_INT interrupt.
 */
#define SPI_DMA_SEG_TRANS_DONE_INT_ENA    (BIT(13))
#define SPI_DMA_SEG_TRANS_DONE_INT_ENA_M  (SPI_DMA_SEG_TRANS_DONE_INT_ENA_V << SPI_DMA_SEG_TRANS_DONE_INT_ENA_S)
#define SPI_DMA_SEG_TRANS_DONE_INT_ENA_V  0x00000001U
#define SPI_DMA_SEG_TRANS_DONE_INT_ENA_S  13
/** SPI_SEG_MAGIC_ERR_INT_ENA : R/W; bitpos: [14]; default: 0;
 *  Write 1 to enable SPI_SEG_MAGIC_ERR_INT interrupt.
 */
#define SPI_SEG_MAGIC_ERR_INT_ENA    (BIT(14))
#define SPI_SEG_MAGIC_ERR_INT_ENA_M  (SPI_SEG_MAGIC_ERR_INT_ENA_V << SPI_SEG_MAGIC_ERR_INT_ENA_S)
#define SPI_SEG_MAGIC_ERR_INT_ENA_V  0x00000001U
#define SPI_SEG_MAGIC_ERR_INT_ENA_S  14
/** SPI_SLV_BUF_ADDR_ERR_INT_ENA : R/W; bitpos: [15]; default: 0;
 *  The enable bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.
 */
#define SPI_SLV_BUF_ADDR_ERR_INT_ENA    (BIT(15))
#define SPI_SLV_BUF_ADDR_ERR_INT_ENA_M  (SPI_SLV_BUF_ADDR_ERR_INT_ENA_V << SPI_SLV_BUF_ADDR_ERR_INT_ENA_S)
#define SPI_SLV_BUF_ADDR_ERR_INT_ENA_V  0x00000001U
#define SPI_SLV_BUF_ADDR_ERR_INT_ENA_S  15
/** SPI_SLV_CMD_ERR_INT_ENA : R/W; bitpos: [16]; default: 0;
 *  Write 1 to enable SPI_SLV_CMD_ERR_INT interrupt.
 */
#define SPI_SLV_CMD_ERR_INT_ENA    (BIT(16))
#define SPI_SLV_CMD_ERR_INT_ENA_M  (SPI_SLV_CMD_ERR_INT_ENA_V << SPI_SLV_CMD_ERR_INT_ENA_S)
#define SPI_SLV_CMD_ERR_INT_ENA_V  0x00000001U
#define SPI_SLV_CMD_ERR_INT_ENA_S  16
/** SPI_MST_RX_AFIFO_WFULL_ERR_INT_ENA : R/W; bitpos: [17]; default: 0;
 *  Write 1 to enable SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.
 */
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_ENA    (BIT(17))
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_ENA_M  (SPI_MST_RX_AFIFO_WFULL_ERR_INT_ENA_V << SPI_MST_RX_AFIFO_WFULL_ERR_INT_ENA_S)
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_ENA_V  0x00000001U
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_ENA_S  17
/** SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ENA : R/W; bitpos: [18]; default: 0;
 *  Write 1 to enable SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.
 */
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ENA    (BIT(18))
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ENA_M  (SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ENA_V << SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ENA_S)
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ENA_V  0x00000001U
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ENA_S  18
/** SPI_APP2_INT_ENA : R/W; bitpos: [19]; default: 0;
 *  Write 1 to enable SPI_APP2_INT interrupt.
 */
#define SPI_APP2_INT_ENA    (BIT(19))
#define SPI_APP2_INT_ENA_M  (SPI_APP2_INT_ENA_V << SPI_APP2_INT_ENA_S)
#define SPI_APP2_INT_ENA_V  0x00000001U
#define SPI_APP2_INT_ENA_S  19
/** SPI_APP1_INT_ENA : R/W; bitpos: [20]; default: 0;
 *  Write 1 to enable SPI_APP1_INT interrupt.
 */
#define SPI_APP1_INT_ENA    (BIT(20))
#define SPI_APP1_INT_ENA_M  (SPI_APP1_INT_ENA_V << SPI_APP1_INT_ENA_S)
#define SPI_APP1_INT_ENA_V  0x00000001U
#define SPI_APP1_INT_ENA_S  20

/** SPI_DMA_INT_CLR_REG register
 *  SPI interrupt clear register
 */
#define SPI_DMA_INT_CLR_REG(i) (REG_SPI_BASE(i) + 0x38)
/** SPI_DMA_INFIFO_FULL_ERR_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Write 1 to clear SPI_DMA_INFIFO_FULL_ERR_INT interrupt.
 */
#define SPI_DMA_INFIFO_FULL_ERR_INT_CLR    (BIT(0))
#define SPI_DMA_INFIFO_FULL_ERR_INT_CLR_M  (SPI_DMA_INFIFO_FULL_ERR_INT_CLR_V << SPI_DMA_INFIFO_FULL_ERR_INT_CLR_S)
#define SPI_DMA_INFIFO_FULL_ERR_INT_CLR_V  0x00000001U
#define SPI_DMA_INFIFO_FULL_ERR_INT_CLR_S  0
/** SPI_DMA_OUTFIFO_EMPTY_ERR_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Write 1 to clear SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt.
 */
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_CLR    (BIT(1))
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_CLR_M  (SPI_DMA_OUTFIFO_EMPTY_ERR_INT_CLR_V << SPI_DMA_OUTFIFO_EMPTY_ERR_INT_CLR_S)
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_CLR_V  0x00000001U
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_CLR_S  1
/** SPI_SLV_EX_QPI_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Write 1 to clear SPI_SLV_EX_QPI_INT interrupt.
 */
#define SPI_SLV_EX_QPI_INT_CLR    (BIT(2))
#define SPI_SLV_EX_QPI_INT_CLR_M  (SPI_SLV_EX_QPI_INT_CLR_V << SPI_SLV_EX_QPI_INT_CLR_S)
#define SPI_SLV_EX_QPI_INT_CLR_V  0x00000001U
#define SPI_SLV_EX_QPI_INT_CLR_S  2
/** SPI_SLV_EN_QPI_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Write 1 to clear SPI_SLV_EN_QPI_INT interrupt.
 */
#define SPI_SLV_EN_QPI_INT_CLR    (BIT(3))
#define SPI_SLV_EN_QPI_INT_CLR_M  (SPI_SLV_EN_QPI_INT_CLR_V << SPI_SLV_EN_QPI_INT_CLR_S)
#define SPI_SLV_EN_QPI_INT_CLR_V  0x00000001U
#define SPI_SLV_EN_QPI_INT_CLR_S  3
/** SPI_SLV_CMD7_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Write 1 to clear SPI_SLV_CMD7_INT interrupt.
 */
#define SPI_SLV_CMD7_INT_CLR    (BIT(4))
#define SPI_SLV_CMD7_INT_CLR_M  (SPI_SLV_CMD7_INT_CLR_V << SPI_SLV_CMD7_INT_CLR_S)
#define SPI_SLV_CMD7_INT_CLR_V  0x00000001U
#define SPI_SLV_CMD7_INT_CLR_S  4
/** SPI_SLV_CMD8_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Write 1 to clear SPI_SLV_CMD8_INT interrupt.
 */
#define SPI_SLV_CMD8_INT_CLR    (BIT(5))
#define SPI_SLV_CMD8_INT_CLR_M  (SPI_SLV_CMD8_INT_CLR_V << SPI_SLV_CMD8_INT_CLR_S)
#define SPI_SLV_CMD8_INT_CLR_V  0x00000001U
#define SPI_SLV_CMD8_INT_CLR_S  5
/** SPI_SLV_CMD9_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Write 1 to clear SPI_SLV_CMD9_INT interrupt.
 */
#define SPI_SLV_CMD9_INT_CLR    (BIT(6))
#define SPI_SLV_CMD9_INT_CLR_M  (SPI_SLV_CMD9_INT_CLR_V << SPI_SLV_CMD9_INT_CLR_S)
#define SPI_SLV_CMD9_INT_CLR_V  0x00000001U
#define SPI_SLV_CMD9_INT_CLR_S  6
/** SPI_SLV_CMDA_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Write 1 to clear SPI_SLV_CMDA_INT interrupt.
 */
#define SPI_SLV_CMDA_INT_CLR    (BIT(7))
#define SPI_SLV_CMDA_INT_CLR_M  (SPI_SLV_CMDA_INT_CLR_V << SPI_SLV_CMDA_INT_CLR_S)
#define SPI_SLV_CMDA_INT_CLR_V  0x00000001U
#define SPI_SLV_CMDA_INT_CLR_S  7
/** SPI_SLV_RD_DMA_DONE_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Write 1 to clear SPI_SLV_RD_DMA_DONE_INT interrupt.
 */
#define SPI_SLV_RD_DMA_DONE_INT_CLR    (BIT(8))
#define SPI_SLV_RD_DMA_DONE_INT_CLR_M  (SPI_SLV_RD_DMA_DONE_INT_CLR_V << SPI_SLV_RD_DMA_DONE_INT_CLR_S)
#define SPI_SLV_RD_DMA_DONE_INT_CLR_V  0x00000001U
#define SPI_SLV_RD_DMA_DONE_INT_CLR_S  8
/** SPI_SLV_WR_DMA_DONE_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Write 1 to clear SPI_SLV_WR_DMA_DONE_INT interrupt.
 */
#define SPI_SLV_WR_DMA_DONE_INT_CLR    (BIT(9))
#define SPI_SLV_WR_DMA_DONE_INT_CLR_M  (SPI_SLV_WR_DMA_DONE_INT_CLR_V << SPI_SLV_WR_DMA_DONE_INT_CLR_S)
#define SPI_SLV_WR_DMA_DONE_INT_CLR_V  0x00000001U
#define SPI_SLV_WR_DMA_DONE_INT_CLR_S  9
/** SPI_SLV_RD_BUF_DONE_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Write 1 to clear SPI_SLV_RD_BUF_DONE_INT interrupt.
 */
#define SPI_SLV_RD_BUF_DONE_INT_CLR    (BIT(10))
#define SPI_SLV_RD_BUF_DONE_INT_CLR_M  (SPI_SLV_RD_BUF_DONE_INT_CLR_V << SPI_SLV_RD_BUF_DONE_INT_CLR_S)
#define SPI_SLV_RD_BUF_DONE_INT_CLR_V  0x00000001U
#define SPI_SLV_RD_BUF_DONE_INT_CLR_S  10
/** SPI_SLV_WR_BUF_DONE_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Write 1 to clear SPI_SLV_WR_BUF_DONE_INT interrupt.
 */
#define SPI_SLV_WR_BUF_DONE_INT_CLR    (BIT(11))
#define SPI_SLV_WR_BUF_DONE_INT_CLR_M  (SPI_SLV_WR_BUF_DONE_INT_CLR_V << SPI_SLV_WR_BUF_DONE_INT_CLR_S)
#define SPI_SLV_WR_BUF_DONE_INT_CLR_V  0x00000001U
#define SPI_SLV_WR_BUF_DONE_INT_CLR_S  11
/** SPI_TRANS_DONE_INT_CLR : WT; bitpos: [12]; default: 0;
 *  Write 1 to clear SPI_TRANS_DONE_INT interrupt.
 */
#define SPI_TRANS_DONE_INT_CLR    (BIT(12))
#define SPI_TRANS_DONE_INT_CLR_M  (SPI_TRANS_DONE_INT_CLR_V << SPI_TRANS_DONE_INT_CLR_S)
#define SPI_TRANS_DONE_INT_CLR_V  0x00000001U
#define SPI_TRANS_DONE_INT_CLR_S  12
/** SPI_DMA_SEG_TRANS_DONE_INT_CLR : WT; bitpos: [13]; default: 0;
 *  Write 1 to clear SPI_DMA_SEG_TRANS_DONE_INT interrupt.
 */
#define SPI_DMA_SEG_TRANS_DONE_INT_CLR    (BIT(13))
#define SPI_DMA_SEG_TRANS_DONE_INT_CLR_M  (SPI_DMA_SEG_TRANS_DONE_INT_CLR_V << SPI_DMA_SEG_TRANS_DONE_INT_CLR_S)
#define SPI_DMA_SEG_TRANS_DONE_INT_CLR_V  0x00000001U
#define SPI_DMA_SEG_TRANS_DONE_INT_CLR_S  13
/** SPI_SEG_MAGIC_ERR_INT_CLR : WT; bitpos: [14]; default: 0;
 *  Write 1 to clear SPI_SEG_MAGIC_ERR_INT interrupt.
 */
#define SPI_SEG_MAGIC_ERR_INT_CLR    (BIT(14))
#define SPI_SEG_MAGIC_ERR_INT_CLR_M  (SPI_SEG_MAGIC_ERR_INT_CLR_V << SPI_SEG_MAGIC_ERR_INT_CLR_S)
#define SPI_SEG_MAGIC_ERR_INT_CLR_V  0x00000001U
#define SPI_SEG_MAGIC_ERR_INT_CLR_S  14
/** SPI_SLV_BUF_ADDR_ERR_INT_CLR : WT; bitpos: [15]; default: 0;
 *  The clear bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.
 */
#define SPI_SLV_BUF_ADDR_ERR_INT_CLR    (BIT(15))
#define SPI_SLV_BUF_ADDR_ERR_INT_CLR_M  (SPI_SLV_BUF_ADDR_ERR_INT_CLR_V << SPI_SLV_BUF_ADDR_ERR_INT_CLR_S)
#define SPI_SLV_BUF_ADDR_ERR_INT_CLR_V  0x00000001U
#define SPI_SLV_BUF_ADDR_ERR_INT_CLR_S  15
/** SPI_SLV_CMD_ERR_INT_CLR : WT; bitpos: [16]; default: 0;
 *  Write 1 to clear SPI_SLV_CMD_ERR_INT interrupt.
 */
#define SPI_SLV_CMD_ERR_INT_CLR    (BIT(16))
#define SPI_SLV_CMD_ERR_INT_CLR_M  (SPI_SLV_CMD_ERR_INT_CLR_V << SPI_SLV_CMD_ERR_INT_CLR_S)
#define SPI_SLV_CMD_ERR_INT_CLR_V  0x00000001U
#define SPI_SLV_CMD_ERR_INT_CLR_S  16
/** SPI_MST_RX_AFIFO_WFULL_ERR_INT_CLR : WT; bitpos: [17]; default: 0;
 *  Write 1 to clear SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.
 */
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_CLR    (BIT(17))
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_CLR_M  (SPI_MST_RX_AFIFO_WFULL_ERR_INT_CLR_V << SPI_MST_RX_AFIFO_WFULL_ERR_INT_CLR_S)
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_CLR_V  0x00000001U
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_CLR_S  17
/** SPI_MST_TX_AFIFO_REMPTY_ERR_INT_CLR : WT; bitpos: [18]; default: 0;
 *  Write 1 to clear SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.
 */
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_CLR    (BIT(18))
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_CLR_M  (SPI_MST_TX_AFIFO_REMPTY_ERR_INT_CLR_V << SPI_MST_TX_AFIFO_REMPTY_ERR_INT_CLR_S)
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_CLR_V  0x00000001U
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_CLR_S  18
/** SPI_APP2_INT_CLR : WT; bitpos: [19]; default: 0;
 *  Write 1 to clear SPI_APP2_INT interrupt.
 */
#define SPI_APP2_INT_CLR    (BIT(19))
#define SPI_APP2_INT_CLR_M  (SPI_APP2_INT_CLR_V << SPI_APP2_INT_CLR_S)
#define SPI_APP2_INT_CLR_V  0x00000001U
#define SPI_APP2_INT_CLR_S  19
/** SPI_APP1_INT_CLR : WT; bitpos: [20]; default: 0;
 *  Write 1 to clear SPI_APP1_INT interrupt.
 */
#define SPI_APP1_INT_CLR    (BIT(20))
#define SPI_APP1_INT_CLR_M  (SPI_APP1_INT_CLR_V << SPI_APP1_INT_CLR_S)
#define SPI_APP1_INT_CLR_V  0x00000001U
#define SPI_APP1_INT_CLR_S  20

/** SPI_DMA_INT_RAW_REG register
 *  SPI interrupt raw register
 */
#define SPI_DMA_INT_RAW_REG(i) (REG_SPI_BASE(i) + 0x3c)
/** SPI_DMA_INFIFO_FULL_ERR_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt status of SPI_DMA_INFIFO_FULL_ERR_INT interrupt.
 */
#define SPI_DMA_INFIFO_FULL_ERR_INT_RAW    (BIT(0))
#define SPI_DMA_INFIFO_FULL_ERR_INT_RAW_M  (SPI_DMA_INFIFO_FULL_ERR_INT_RAW_V << SPI_DMA_INFIFO_FULL_ERR_INT_RAW_S)
#define SPI_DMA_INFIFO_FULL_ERR_INT_RAW_V  0x00000001U
#define SPI_DMA_INFIFO_FULL_ERR_INT_RAW_S  0
/** SPI_DMA_OUTFIFO_EMPTY_ERR_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt status of SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt.
 */
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_RAW    (BIT(1))
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_RAW_M  (SPI_DMA_OUTFIFO_EMPTY_ERR_INT_RAW_V << SPI_DMA_OUTFIFO_EMPTY_ERR_INT_RAW_S)
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_RAW_V  0x00000001U
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_RAW_S  1
/** SPI_SLV_EX_QPI_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt status of SPI_SLV_EX_QPI_INT interrupt.
 */
#define SPI_SLV_EX_QPI_INT_RAW    (BIT(2))
#define SPI_SLV_EX_QPI_INT_RAW_M  (SPI_SLV_EX_QPI_INT_RAW_V << SPI_SLV_EX_QPI_INT_RAW_S)
#define SPI_SLV_EX_QPI_INT_RAW_V  0x00000001U
#define SPI_SLV_EX_QPI_INT_RAW_S  2
/** SPI_SLV_EN_QPI_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt status of SPI_SLV_EN_QPI_INT interrupt.
 */
#define SPI_SLV_EN_QPI_INT_RAW    (BIT(3))
#define SPI_SLV_EN_QPI_INT_RAW_M  (SPI_SLV_EN_QPI_INT_RAW_V << SPI_SLV_EN_QPI_INT_RAW_S)
#define SPI_SLV_EN_QPI_INT_RAW_V  0x00000001U
#define SPI_SLV_EN_QPI_INT_RAW_S  3
/** SPI_SLV_CMD7_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt status of SPI_SLV_CMD7_INT interrupt.
 */
#define SPI_SLV_CMD7_INT_RAW    (BIT(4))
#define SPI_SLV_CMD7_INT_RAW_M  (SPI_SLV_CMD7_INT_RAW_V << SPI_SLV_CMD7_INT_RAW_S)
#define SPI_SLV_CMD7_INT_RAW_V  0x00000001U
#define SPI_SLV_CMD7_INT_RAW_S  4
/** SPI_SLV_CMD8_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt status of SPI_SLV_CMD8_INT interrupt.
 */
#define SPI_SLV_CMD8_INT_RAW    (BIT(5))
#define SPI_SLV_CMD8_INT_RAW_M  (SPI_SLV_CMD8_INT_RAW_V << SPI_SLV_CMD8_INT_RAW_S)
#define SPI_SLV_CMD8_INT_RAW_V  0x00000001U
#define SPI_SLV_CMD8_INT_RAW_S  5
/** SPI_SLV_CMD9_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt status of SPI_SLV_CMD9_INT interrupt.
 */
#define SPI_SLV_CMD9_INT_RAW    (BIT(6))
#define SPI_SLV_CMD9_INT_RAW_M  (SPI_SLV_CMD9_INT_RAW_V << SPI_SLV_CMD9_INT_RAW_S)
#define SPI_SLV_CMD9_INT_RAW_V  0x00000001U
#define SPI_SLV_CMD9_INT_RAW_S  6
/** SPI_SLV_CMDA_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt status of SPI_SLV_CMDA_INT interrupt.
 */
#define SPI_SLV_CMDA_INT_RAW    (BIT(7))
#define SPI_SLV_CMDA_INT_RAW_M  (SPI_SLV_CMDA_INT_RAW_V << SPI_SLV_CMDA_INT_RAW_S)
#define SPI_SLV_CMDA_INT_RAW_V  0x00000001U
#define SPI_SLV_CMDA_INT_RAW_S  7
/** SPI_SLV_RD_DMA_DONE_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt status of SPI_SLV_RD_DMA_DONE_INT interrupt.
 */
#define SPI_SLV_RD_DMA_DONE_INT_RAW    (BIT(8))
#define SPI_SLV_RD_DMA_DONE_INT_RAW_M  (SPI_SLV_RD_DMA_DONE_INT_RAW_V << SPI_SLV_RD_DMA_DONE_INT_RAW_S)
#define SPI_SLV_RD_DMA_DONE_INT_RAW_V  0x00000001U
#define SPI_SLV_RD_DMA_DONE_INT_RAW_S  8
/** SPI_SLV_WR_DMA_DONE_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  The raw interrupt status of SPI_SLV_WR_DMA_DONE_INT interrupt.
 */
#define SPI_SLV_WR_DMA_DONE_INT_RAW    (BIT(9))
#define SPI_SLV_WR_DMA_DONE_INT_RAW_M  (SPI_SLV_WR_DMA_DONE_INT_RAW_V << SPI_SLV_WR_DMA_DONE_INT_RAW_S)
#define SPI_SLV_WR_DMA_DONE_INT_RAW_V  0x00000001U
#define SPI_SLV_WR_DMA_DONE_INT_RAW_S  9
/** SPI_SLV_RD_BUF_DONE_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  The raw interrupt status of SPI_SLV_RD_BUF_DONE_INT interrupt.
 */
#define SPI_SLV_RD_BUF_DONE_INT_RAW    (BIT(10))
#define SPI_SLV_RD_BUF_DONE_INT_RAW_M  (SPI_SLV_RD_BUF_DONE_INT_RAW_V << SPI_SLV_RD_BUF_DONE_INT_RAW_S)
#define SPI_SLV_RD_BUF_DONE_INT_RAW_V  0x00000001U
#define SPI_SLV_RD_BUF_DONE_INT_RAW_S  10
/** SPI_SLV_WR_BUF_DONE_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  The raw interrupt status of SPI_SLV_WR_BUF_DONE_INT interrupt.
 */
#define SPI_SLV_WR_BUF_DONE_INT_RAW    (BIT(11))
#define SPI_SLV_WR_BUF_DONE_INT_RAW_M  (SPI_SLV_WR_BUF_DONE_INT_RAW_V << SPI_SLV_WR_BUF_DONE_INT_RAW_S)
#define SPI_SLV_WR_BUF_DONE_INT_RAW_V  0x00000001U
#define SPI_SLV_WR_BUF_DONE_INT_RAW_S  11
/** SPI_TRANS_DONE_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  The raw interrupt status of SPI_TRANS_DONE_INT interrupt.
 */
#define SPI_TRANS_DONE_INT_RAW    (BIT(12))
#define SPI_TRANS_DONE_INT_RAW_M  (SPI_TRANS_DONE_INT_RAW_V << SPI_TRANS_DONE_INT_RAW_S)
#define SPI_TRANS_DONE_INT_RAW_V  0x00000001U
#define SPI_TRANS_DONE_INT_RAW_S  12
/** SPI_DMA_SEG_TRANS_DONE_INT_RAW : R/WTC/SS; bitpos: [13]; default: 0;
 *  The raw interrupt status of SPI_DMA_SEG_TRANS_DONE_INT interrupt.
 */
#define SPI_DMA_SEG_TRANS_DONE_INT_RAW    (BIT(13))
#define SPI_DMA_SEG_TRANS_DONE_INT_RAW_M  (SPI_DMA_SEG_TRANS_DONE_INT_RAW_V << SPI_DMA_SEG_TRANS_DONE_INT_RAW_S)
#define SPI_DMA_SEG_TRANS_DONE_INT_RAW_V  0x00000001U
#define SPI_DMA_SEG_TRANS_DONE_INT_RAW_S  13
/** SPI_SEG_MAGIC_ERR_INT_RAW : R/WTC/SS; bitpos: [14]; default: 0;
 *  The raw interrupt status of SPI_SEG_MAGIC_ERR_INT interrupt.
 */
#define SPI_SEG_MAGIC_ERR_INT_RAW    (BIT(14))
#define SPI_SEG_MAGIC_ERR_INT_RAW_M  (SPI_SEG_MAGIC_ERR_INT_RAW_V << SPI_SEG_MAGIC_ERR_INT_RAW_S)
#define SPI_SEG_MAGIC_ERR_INT_RAW_V  0x00000001U
#define SPI_SEG_MAGIC_ERR_INT_RAW_S  14
/** SPI_SLV_BUF_ADDR_ERR_INT_RAW : R/WTC/SS; bitpos: [15]; default: 0;
 *  The raw bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt. 1: The accessing data address
 *  of the current SPI slave mode CPU controlled FD, Wr_BUF or Rd_BUF transmission is
 *  bigger than 63. 0: Others.
 */
#define SPI_SLV_BUF_ADDR_ERR_INT_RAW    (BIT(15))
#define SPI_SLV_BUF_ADDR_ERR_INT_RAW_M  (SPI_SLV_BUF_ADDR_ERR_INT_RAW_V << SPI_SLV_BUF_ADDR_ERR_INT_RAW_S)
#define SPI_SLV_BUF_ADDR_ERR_INT_RAW_V  0x00000001U
#define SPI_SLV_BUF_ADDR_ERR_INT_RAW_S  15
/** SPI_SLV_CMD_ERR_INT_RAW : R/WTC/SS; bitpos: [16]; default: 0;
 *  The raw interrupt status of SPI_SLV_CMD_ERR_INT interrupt.
 */
#define SPI_SLV_CMD_ERR_INT_RAW    (BIT(16))
#define SPI_SLV_CMD_ERR_INT_RAW_M  (SPI_SLV_CMD_ERR_INT_RAW_V << SPI_SLV_CMD_ERR_INT_RAW_S)
#define SPI_SLV_CMD_ERR_INT_RAW_V  0x00000001U
#define SPI_SLV_CMD_ERR_INT_RAW_S  16
/** SPI_MST_RX_AFIFO_WFULL_ERR_INT_RAW : R/WTC/SS; bitpos: [17]; default: 0;
 *  The raw interrupt status of SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.
 */
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_RAW    (BIT(17))
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_RAW_M  (SPI_MST_RX_AFIFO_WFULL_ERR_INT_RAW_V << SPI_MST_RX_AFIFO_WFULL_ERR_INT_RAW_S)
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_RAW_V  0x00000001U
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_RAW_S  17
/** SPI_MST_TX_AFIFO_REMPTY_ERR_INT_RAW : R/WTC/SS; bitpos: [18]; default: 0;
 *  The raw interrupt status of SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.
 */
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_RAW    (BIT(18))
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_RAW_M  (SPI_MST_TX_AFIFO_REMPTY_ERR_INT_RAW_V << SPI_MST_TX_AFIFO_REMPTY_ERR_INT_RAW_S)
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_RAW_V  0x00000001U
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_RAW_S  18
/** SPI_APP2_INT_RAW : R/WTC/SS; bitpos: [19]; default: 0;
 *  The raw interrupt status of SPI_APP2_INT interrupt. The value is only controlled by
 *  the application.
 */
#define SPI_APP2_INT_RAW    (BIT(19))
#define SPI_APP2_INT_RAW_M  (SPI_APP2_INT_RAW_V << SPI_APP2_INT_RAW_S)
#define SPI_APP2_INT_RAW_V  0x00000001U
#define SPI_APP2_INT_RAW_S  19
/** SPI_APP1_INT_RAW : R/WTC/SS; bitpos: [20]; default: 0;
 *  The raw interrupt status of SPI_APP1_INT interrupt. The value is only controlled by
 *  the application.
 */
#define SPI_APP1_INT_RAW    (BIT(20))
#define SPI_APP1_INT_RAW_M  (SPI_APP1_INT_RAW_V << SPI_APP1_INT_RAW_S)
#define SPI_APP1_INT_RAW_V  0x00000001U
#define SPI_APP1_INT_RAW_S  20

/** SPI_DMA_INT_ST_REG register
 *  SPI interrupt status register
 */
#define SPI_DMA_INT_ST_REG(i) (REG_SPI_BASE(i) + 0x40)
/** SPI_DMA_INFIFO_FULL_ERR_INT_ST : RO; bitpos: [0]; default: 0;
 *  The interrupt status of SPI_DMA_INFIFO_FULL_ERR_INT interrupt.
 */
#define SPI_DMA_INFIFO_FULL_ERR_INT_ST    (BIT(0))
#define SPI_DMA_INFIFO_FULL_ERR_INT_ST_M  (SPI_DMA_INFIFO_FULL_ERR_INT_ST_V << SPI_DMA_INFIFO_FULL_ERR_INT_ST_S)
#define SPI_DMA_INFIFO_FULL_ERR_INT_ST_V  0x00000001U
#define SPI_DMA_INFIFO_FULL_ERR_INT_ST_S  0
/** SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ST : RO; bitpos: [1]; default: 0;
 *  The interrupt status of SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt.
 */
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ST    (BIT(1))
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ST_M  (SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ST_V << SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ST_S)
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ST_V  0x00000001U
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ST_S  1
/** SPI_SLV_EX_QPI_INT_ST : RO; bitpos: [2]; default: 0;
 *  The interrupt status of SPI_SLV_EX_QPI_INT interrupt.
 */
#define SPI_SLV_EX_QPI_INT_ST    (BIT(2))
#define SPI_SLV_EX_QPI_INT_ST_M  (SPI_SLV_EX_QPI_INT_ST_V << SPI_SLV_EX_QPI_INT_ST_S)
#define SPI_SLV_EX_QPI_INT_ST_V  0x00000001U
#define SPI_SLV_EX_QPI_INT_ST_S  2
/** SPI_SLV_EN_QPI_INT_ST : RO; bitpos: [3]; default: 0;
 *  The interrupt status of SPI_SLV_EN_QPI_INT interrupt.
 */
#define SPI_SLV_EN_QPI_INT_ST    (BIT(3))
#define SPI_SLV_EN_QPI_INT_ST_M  (SPI_SLV_EN_QPI_INT_ST_V << SPI_SLV_EN_QPI_INT_ST_S)
#define SPI_SLV_EN_QPI_INT_ST_V  0x00000001U
#define SPI_SLV_EN_QPI_INT_ST_S  3
/** SPI_SLV_CMD7_INT_ST : RO; bitpos: [4]; default: 0;
 *  The interrupt status of SPI_SLV_CMD7_INT interrupt.
 */
#define SPI_SLV_CMD7_INT_ST    (BIT(4))
#define SPI_SLV_CMD7_INT_ST_M  (SPI_SLV_CMD7_INT_ST_V << SPI_SLV_CMD7_INT_ST_S)
#define SPI_SLV_CMD7_INT_ST_V  0x00000001U
#define SPI_SLV_CMD7_INT_ST_S  4
/** SPI_SLV_CMD8_INT_ST : RO; bitpos: [5]; default: 0;
 *  The interrupt status of SPI_SLV_CMD8_INT interrupt.
 */
#define SPI_SLV_CMD8_INT_ST    (BIT(5))
#define SPI_SLV_CMD8_INT_ST_M  (SPI_SLV_CMD8_INT_ST_V << SPI_SLV_CMD8_INT_ST_S)
#define SPI_SLV_CMD8_INT_ST_V  0x00000001U
#define SPI_SLV_CMD8_INT_ST_S  5
/** SPI_SLV_CMD9_INT_ST : RO; bitpos: [6]; default: 0;
 *  The interrupt status of SPI_SLV_CMD9_INT interrupt.
 */
#define SPI_SLV_CMD9_INT_ST    (BIT(6))
#define SPI_SLV_CMD9_INT_ST_M  (SPI_SLV_CMD9_INT_ST_V << SPI_SLV_CMD9_INT_ST_S)
#define SPI_SLV_CMD9_INT_ST_V  0x00000001U
#define SPI_SLV_CMD9_INT_ST_S  6
/** SPI_SLV_CMDA_INT_ST : RO; bitpos: [7]; default: 0;
 *  The interrupt status of SPI_SLV_CMDA_INT interrupt.
 */
#define SPI_SLV_CMDA_INT_ST    (BIT(7))
#define SPI_SLV_CMDA_INT_ST_M  (SPI_SLV_CMDA_INT_ST_V << SPI_SLV_CMDA_INT_ST_S)
#define SPI_SLV_CMDA_INT_ST_V  0x00000001U
#define SPI_SLV_CMDA_INT_ST_S  7
/** SPI_SLV_RD_DMA_DONE_INT_ST : RO; bitpos: [8]; default: 0;
 *  The interrupt status of SPI_SLV_RD_DMA_DONE_INT interrupt.
 */
#define SPI_SLV_RD_DMA_DONE_INT_ST    (BIT(8))
#define SPI_SLV_RD_DMA_DONE_INT_ST_M  (SPI_SLV_RD_DMA_DONE_INT_ST_V << SPI_SLV_RD_DMA_DONE_INT_ST_S)
#define SPI_SLV_RD_DMA_DONE_INT_ST_V  0x00000001U
#define SPI_SLV_RD_DMA_DONE_INT_ST_S  8
/** SPI_SLV_WR_DMA_DONE_INT_ST : RO; bitpos: [9]; default: 0;
 *  The interrupt status of SPI_SLV_WR_DMA_DONE_INT interrupt.
 */
#define SPI_SLV_WR_DMA_DONE_INT_ST    (BIT(9))
#define SPI_SLV_WR_DMA_DONE_INT_ST_M  (SPI_SLV_WR_DMA_DONE_INT_ST_V << SPI_SLV_WR_DMA_DONE_INT_ST_S)
#define SPI_SLV_WR_DMA_DONE_INT_ST_V  0x00000001U
#define SPI_SLV_WR_DMA_DONE_INT_ST_S  9
/** SPI_SLV_RD_BUF_DONE_INT_ST : RO; bitpos: [10]; default: 0;
 *  The interrupt status of SPI_SLV_RD_BUF_DONE_INT interrupt.
 */
#define SPI_SLV_RD_BUF_DONE_INT_ST    (BIT(10))
#define SPI_SLV_RD_BUF_DONE_INT_ST_M  (SPI_SLV_RD_BUF_DONE_INT_ST_V << SPI_SLV_RD_BUF_DONE_INT_ST_S)
#define SPI_SLV_RD_BUF_DONE_INT_ST_V  0x00000001U
#define SPI_SLV_RD_BUF_DONE_INT_ST_S  10
/** SPI_SLV_WR_BUF_DONE_INT_ST : RO; bitpos: [11]; default: 0;
 *  The interrupt status of SPI_SLV_WR_BUF_DONE_INT interrupt.
 */
#define SPI_SLV_WR_BUF_DONE_INT_ST    (BIT(11))
#define SPI_SLV_WR_BUF_DONE_INT_ST_M  (SPI_SLV_WR_BUF_DONE_INT_ST_V << SPI_SLV_WR_BUF_DONE_INT_ST_S)
#define SPI_SLV_WR_BUF_DONE_INT_ST_V  0x00000001U
#define SPI_SLV_WR_BUF_DONE_INT_ST_S  11
/** SPI_TRANS_DONE_INT_ST : RO; bitpos: [12]; default: 0;
 *  The interrupt status of SPI_TRANS_DONE_INT interrupt.
 */
#define SPI_TRANS_DONE_INT_ST    (BIT(12))
#define SPI_TRANS_DONE_INT_ST_M  (SPI_TRANS_DONE_INT_ST_V << SPI_TRANS_DONE_INT_ST_S)
#define SPI_TRANS_DONE_INT_ST_V  0x00000001U
#define SPI_TRANS_DONE_INT_ST_S  12
/** SPI_DMA_SEG_TRANS_DONE_INT_ST : RO; bitpos: [13]; default: 0;
 *  The interrupt status of SPI_DMA_SEG_TRANS_DONE_INT interrupt.
 */
#define SPI_DMA_SEG_TRANS_DONE_INT_ST    (BIT(13))
#define SPI_DMA_SEG_TRANS_DONE_INT_ST_M  (SPI_DMA_SEG_TRANS_DONE_INT_ST_V << SPI_DMA_SEG_TRANS_DONE_INT_ST_S)
#define SPI_DMA_SEG_TRANS_DONE_INT_ST_V  0x00000001U
#define SPI_DMA_SEG_TRANS_DONE_INT_ST_S  13
/** SPI_SEG_MAGIC_ERR_INT_ST : RO; bitpos: [14]; default: 0;
 *  The interrupt status of SPI_SEG_MAGIC_ERR_INT interrupt.
 */
#define SPI_SEG_MAGIC_ERR_INT_ST    (BIT(14))
#define SPI_SEG_MAGIC_ERR_INT_ST_M  (SPI_SEG_MAGIC_ERR_INT_ST_V << SPI_SEG_MAGIC_ERR_INT_ST_S)
#define SPI_SEG_MAGIC_ERR_INT_ST_V  0x00000001U
#define SPI_SEG_MAGIC_ERR_INT_ST_S  14
/** SPI_SLV_BUF_ADDR_ERR_INT_ST : RO; bitpos: [15]; default: 0;
 *  The status bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.
 */
#define SPI_SLV_BUF_ADDR_ERR_INT_ST    (BIT(15))
#define SPI_SLV_BUF_ADDR_ERR_INT_ST_M  (SPI_SLV_BUF_ADDR_ERR_INT_ST_V << SPI_SLV_BUF_ADDR_ERR_INT_ST_S)
#define SPI_SLV_BUF_ADDR_ERR_INT_ST_V  0x00000001U
#define SPI_SLV_BUF_ADDR_ERR_INT_ST_S  15
/** SPI_SLV_CMD_ERR_INT_ST : RO; bitpos: [16]; default: 0;
 *  The interrupt status of SPI_SLV_CMD_ERR_INT interrupt.
 */
#define SPI_SLV_CMD_ERR_INT_ST    (BIT(16))
#define SPI_SLV_CMD_ERR_INT_ST_M  (SPI_SLV_CMD_ERR_INT_ST_V << SPI_SLV_CMD_ERR_INT_ST_S)
#define SPI_SLV_CMD_ERR_INT_ST_V  0x00000001U
#define SPI_SLV_CMD_ERR_INT_ST_S  16
/** SPI_MST_RX_AFIFO_WFULL_ERR_INT_ST : RO; bitpos: [17]; default: 0;
 *  The interrupt status of SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.
 */
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_ST    (BIT(17))
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_ST_M  (SPI_MST_RX_AFIFO_WFULL_ERR_INT_ST_V << SPI_MST_RX_AFIFO_WFULL_ERR_INT_ST_S)
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_ST_V  0x00000001U
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_ST_S  17
/** SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ST : RO; bitpos: [18]; default: 0;
 *  The interrupt status of SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.
 */
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ST    (BIT(18))
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ST_M  (SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ST_V << SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ST_S)
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ST_V  0x00000001U
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ST_S  18
/** SPI_APP2_INT_ST : RO; bitpos: [19]; default: 0;
 *  The interrupt status of SPI_APP2_INT interrupt.
 */
#define SPI_APP2_INT_ST    (BIT(19))
#define SPI_APP2_INT_ST_M  (SPI_APP2_INT_ST_V << SPI_APP2_INT_ST_S)
#define SPI_APP2_INT_ST_V  0x00000001U
#define SPI_APP2_INT_ST_S  19
/** SPI_APP1_INT_ST : RO; bitpos: [20]; default: 0;
 *  The interrupt status of SPI_APP1_INT interrupt.
 */
#define SPI_APP1_INT_ST    (BIT(20))
#define SPI_APP1_INT_ST_M  (SPI_APP1_INT_ST_V << SPI_APP1_INT_ST_S)
#define SPI_APP1_INT_ST_V  0x00000001U
#define SPI_APP1_INT_ST_S  20

/** SPI_DMA_INT_SET_REG register
 *  SPI interrupt software set register
 */
#define SPI_DMA_INT_SET_REG(i) (REG_SPI_BASE(i) + 0x44)
/** SPI_DMA_INFIFO_FULL_ERR_INT_SET : WT; bitpos: [0]; default: 0;
 *  Write 1 to set SPI_DMA_INFIFO_FULL_ERR_INT interrupt.
 */
#define SPI_DMA_INFIFO_FULL_ERR_INT_SET    (BIT(0))
#define SPI_DMA_INFIFO_FULL_ERR_INT_SET_M  (SPI_DMA_INFIFO_FULL_ERR_INT_SET_V << SPI_DMA_INFIFO_FULL_ERR_INT_SET_S)
#define SPI_DMA_INFIFO_FULL_ERR_INT_SET_V  0x00000001U
#define SPI_DMA_INFIFO_FULL_ERR_INT_SET_S  0
/** SPI_DMA_OUTFIFO_EMPTY_ERR_INT_SET : WT; bitpos: [1]; default: 0;
 *  Write 1 to set SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt.
 */
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_SET    (BIT(1))
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_SET_M  (SPI_DMA_OUTFIFO_EMPTY_ERR_INT_SET_V << SPI_DMA_OUTFIFO_EMPTY_ERR_INT_SET_S)
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_SET_V  0x00000001U
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_SET_S  1
/** SPI_SLV_EX_QPI_INT_SET : WT; bitpos: [2]; default: 0;
 *  Write 1 to set SPI_SLV_EX_QPI_INT interrupt.
 */
#define SPI_SLV_EX_QPI_INT_SET    (BIT(2))
#define SPI_SLV_EX_QPI_INT_SET_M  (SPI_SLV_EX_QPI_INT_SET_V << SPI_SLV_EX_QPI_INT_SET_S)
#define SPI_SLV_EX_QPI_INT_SET_V  0x00000001U
#define SPI_SLV_EX_QPI_INT_SET_S  2
/** SPI_SLV_EN_QPI_INT_SET : WT; bitpos: [3]; default: 0;
 *  Write 1 to set SPI_SLV_EN_QPI_INT interrupt.
 */
#define SPI_SLV_EN_QPI_INT_SET    (BIT(3))
#define SPI_SLV_EN_QPI_INT_SET_M  (SPI_SLV_EN_QPI_INT_SET_V << SPI_SLV_EN_QPI_INT_SET_S)
#define SPI_SLV_EN_QPI_INT_SET_V  0x00000001U
#define SPI_SLV_EN_QPI_INT_SET_S  3
/** SPI_SLV_CMD7_INT_SET : WT; bitpos: [4]; default: 0;
 *  Write 1 to set SPI_SLV_CMD7_INT interrupt.
 */
#define SPI_SLV_CMD7_INT_SET    (BIT(4))
#define SPI_SLV_CMD7_INT_SET_M  (SPI_SLV_CMD7_INT_SET_V << SPI_SLV_CMD7_INT_SET_S)
#define SPI_SLV_CMD7_INT_SET_V  0x00000001U
#define SPI_SLV_CMD7_INT_SET_S  4
/** SPI_SLV_CMD8_INT_SET : WT; bitpos: [5]; default: 0;
 *  Write 1 to set SPI_SLV_CMD8_INT interrupt.
 */
#define SPI_SLV_CMD8_INT_SET    (BIT(5))
#define SPI_SLV_CMD8_INT_SET_M  (SPI_SLV_CMD8_INT_SET_V << SPI_SLV_CMD8_INT_SET_S)
#define SPI_SLV_CMD8_INT_SET_V  0x00000001U
#define SPI_SLV_CMD8_INT_SET_S  5
/** SPI_SLV_CMD9_INT_SET : WT; bitpos: [6]; default: 0;
 *  Write 1 to set SPI_SLV_CMD9_INT interrupt.
 */
#define SPI_SLV_CMD9_INT_SET    (BIT(6))
#define SPI_SLV_CMD9_INT_SET_M  (SPI_SLV_CMD9_INT_SET_V << SPI_SLV_CMD9_INT_SET_S)
#define SPI_SLV_CMD9_INT_SET_V  0x00000001U
#define SPI_SLV_CMD9_INT_SET_S  6
/** SPI_SLV_CMDA_INT_SET : WT; bitpos: [7]; default: 0;
 *  Write 1 to set SPI_SLV_CMDA_INT interrupt.
 */
#define SPI_SLV_CMDA_INT_SET    (BIT(7))
#define SPI_SLV_CMDA_INT_SET_M  (SPI_SLV_CMDA_INT_SET_V << SPI_SLV_CMDA_INT_SET_S)
#define SPI_SLV_CMDA_INT_SET_V  0x00000001U
#define SPI_SLV_CMDA_INT_SET_S  7
/** SPI_SLV_RD_DMA_DONE_INT_SET : WT; bitpos: [8]; default: 0;
 *  Write 1 to set SPI_SLV_RD_DMA_DONE_INT interrupt.
 */
#define SPI_SLV_RD_DMA_DONE_INT_SET    (BIT(8))
#define SPI_SLV_RD_DMA_DONE_INT_SET_M  (SPI_SLV_RD_DMA_DONE_INT_SET_V << SPI_SLV_RD_DMA_DONE_INT_SET_S)
#define SPI_SLV_RD_DMA_DONE_INT_SET_V  0x00000001U
#define SPI_SLV_RD_DMA_DONE_INT_SET_S  8
/** SPI_SLV_WR_DMA_DONE_INT_SET : WT; bitpos: [9]; default: 0;
 *  Write 1 to set SPI_SLV_WR_DMA_DONE_INT interrupt.
 */
#define SPI_SLV_WR_DMA_DONE_INT_SET    (BIT(9))
#define SPI_SLV_WR_DMA_DONE_INT_SET_M  (SPI_SLV_WR_DMA_DONE_INT_SET_V << SPI_SLV_WR_DMA_DONE_INT_SET_S)
#define SPI_SLV_WR_DMA_DONE_INT_SET_V  0x00000001U
#define SPI_SLV_WR_DMA_DONE_INT_SET_S  9
/** SPI_SLV_RD_BUF_DONE_INT_SET : WT; bitpos: [10]; default: 0;
 *  Write 1 to set SPI_SLV_RD_BUF_DONE_INT interrupt.
 */
#define SPI_SLV_RD_BUF_DONE_INT_SET    (BIT(10))
#define SPI_SLV_RD_BUF_DONE_INT_SET_M  (SPI_SLV_RD_BUF_DONE_INT_SET_V << SPI_SLV_RD_BUF_DONE_INT_SET_S)
#define SPI_SLV_RD_BUF_DONE_INT_SET_V  0x00000001U
#define SPI_SLV_RD_BUF_DONE_INT_SET_S  10
/** SPI_SLV_WR_BUF_DONE_INT_SET : WT; bitpos: [11]; default: 0;
 *  Write 1 to set SPI_SLV_WR_BUF_DONE_INT interrupt.
 */
#define SPI_SLV_WR_BUF_DONE_INT_SET    (BIT(11))
#define SPI_SLV_WR_BUF_DONE_INT_SET_M  (SPI_SLV_WR_BUF_DONE_INT_SET_V << SPI_SLV_WR_BUF_DONE_INT_SET_S)
#define SPI_SLV_WR_BUF_DONE_INT_SET_V  0x00000001U
#define SPI_SLV_WR_BUF_DONE_INT_SET_S  11
/** SPI_TRANS_DONE_INT_SET : WT; bitpos: [12]; default: 0;
 *  Write 1 to set SPI_TRANS_DONE_INT interrupt.
 */
#define SPI_TRANS_DONE_INT_SET    (BIT(12))
#define SPI_TRANS_DONE_INT_SET_M  (SPI_TRANS_DONE_INT_SET_V << SPI_TRANS_DONE_INT_SET_S)
#define SPI_TRANS_DONE_INT_SET_V  0x00000001U
#define SPI_TRANS_DONE_INT_SET_S  12
/** SPI_DMA_SEG_TRANS_DONE_INT_SET : WT; bitpos: [13]; default: 0;
 *  Write 1 to set SPI_DMA_SEG_TRANS_DONE_INT interrupt.
 */
#define SPI_DMA_SEG_TRANS_DONE_INT_SET    (BIT(13))
#define SPI_DMA_SEG_TRANS_DONE_INT_SET_M  (SPI_DMA_SEG_TRANS_DONE_INT_SET_V << SPI_DMA_SEG_TRANS_DONE_INT_SET_S)
#define SPI_DMA_SEG_TRANS_DONE_INT_SET_V  0x00000001U
#define SPI_DMA_SEG_TRANS_DONE_INT_SET_S  13
/** SPI_SEG_MAGIC_ERR_INT_SET : WT; bitpos: [14]; default: 0;
 *  Write 1 to set SPI_SEG_MAGIC_ERR_INT interrupt.
 */
#define SPI_SEG_MAGIC_ERR_INT_SET    (BIT(14))
#define SPI_SEG_MAGIC_ERR_INT_SET_M  (SPI_SEG_MAGIC_ERR_INT_SET_V << SPI_SEG_MAGIC_ERR_INT_SET_S)
#define SPI_SEG_MAGIC_ERR_INT_SET_V  0x00000001U
#define SPI_SEG_MAGIC_ERR_INT_SET_S  14
/** SPI_SLV_BUF_ADDR_ERR_INT_SET : WT; bitpos: [15]; default: 0;
 *  The software set bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.
 */
#define SPI_SLV_BUF_ADDR_ERR_INT_SET    (BIT(15))
#define SPI_SLV_BUF_ADDR_ERR_INT_SET_M  (SPI_SLV_BUF_ADDR_ERR_INT_SET_V << SPI_SLV_BUF_ADDR_ERR_INT_SET_S)
#define SPI_SLV_BUF_ADDR_ERR_INT_SET_V  0x00000001U
#define SPI_SLV_BUF_ADDR_ERR_INT_SET_S  15
/** SPI_SLV_CMD_ERR_INT_SET : WT; bitpos: [16]; default: 0;
 *  Write 1 to set SPI_SLV_CMD_ERR_INT interrupt.
 */
#define SPI_SLV_CMD_ERR_INT_SET    (BIT(16))
#define SPI_SLV_CMD_ERR_INT_SET_M  (SPI_SLV_CMD_ERR_INT_SET_V << SPI_SLV_CMD_ERR_INT_SET_S)
#define SPI_SLV_CMD_ERR_INT_SET_V  0x00000001U
#define SPI_SLV_CMD_ERR_INT_SET_S  16
/** SPI_MST_RX_AFIFO_WFULL_ERR_INT_SET : WT; bitpos: [17]; default: 0;
 *  Write 1 to set SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.
 */
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_SET    (BIT(17))
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_SET_M  (SPI_MST_RX_AFIFO_WFULL_ERR_INT_SET_V << SPI_MST_RX_AFIFO_WFULL_ERR_INT_SET_S)
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_SET_V  0x00000001U
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_SET_S  17
/** SPI_MST_TX_AFIFO_REMPTY_ERR_INT_SET : WT; bitpos: [18]; default: 0;
 *  Write 1 to set SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.
 */
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_SET    (BIT(18))
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_SET_M  (SPI_MST_TX_AFIFO_REMPTY_ERR_INT_SET_V << SPI_MST_TX_AFIFO_REMPTY_ERR_INT_SET_S)
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_SET_V  0x00000001U
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_SET_S  18
/** SPI_APP2_INT_SET : WT; bitpos: [19]; default: 0;
 *  Write 1 to set SPI_APP2_INT interrupt.
 */
#define SPI_APP2_INT_SET    (BIT(19))
#define SPI_APP2_INT_SET_M  (SPI_APP2_INT_SET_V << SPI_APP2_INT_SET_S)
#define SPI_APP2_INT_SET_V  0x00000001U
#define SPI_APP2_INT_SET_S  19
/** SPI_APP1_INT_SET : WT; bitpos: [20]; default: 0;
 *  Write 1 to set SPI_APP1_INT interrupt.
 */
#define SPI_APP1_INT_SET    (BIT(20))
#define SPI_APP1_INT_SET_M  (SPI_APP1_INT_SET_V << SPI_APP1_INT_SET_S)
#define SPI_APP1_INT_SET_V  0x00000001U
#define SPI_APP1_INT_SET_S  20

/** SPI_W0_REG register
 *  SPI CPU-controlled buffer0
 */
#define SPI_W0_REG(i) (REG_SPI_BASE(i) + 0x98)
/** SPI_BUF0 : R/W/SS; bitpos: [31:0]; default: 0;
 *  32-bit data buffer $n.
 */
#define SPI_BUF0    0xFFFFFFFFU
#define SPI_BUF0_M  (SPI_BUF0_V << SPI_BUF0_S)
#define SPI_BUF0_V  0xFFFFFFFFU
#define SPI_BUF0_S  0

/** SPI_W1_REG register
 *  SPI CPU-controlled buffer1
 */
#define SPI_W1_REG(i) (REG_SPI_BASE(i) + 0x9c)
/** SPI_BUF1 : R/W/SS; bitpos: [31:0]; default: 0;
 *  32-bit data buffer $n.
 */
#define SPI_BUF1    0xFFFFFFFFU
#define SPI_BUF1_M  (SPI_BUF1_V << SPI_BUF1_S)
#define SPI_BUF1_V  0xFFFFFFFFU
#define SPI_BUF1_S  0

/** SPI_W2_REG register
 *  SPI CPU-controlled buffer2
 */
#define SPI_W2_REG(i) (REG_SPI_BASE(i) + 0xa0)
/** SPI_BUF2 : R/W/SS; bitpos: [31:0]; default: 0;
 *  32-bit data buffer $n.
 */
#define SPI_BUF2    0xFFFFFFFFU
#define SPI_BUF2_M  (SPI_BUF2_V << SPI_BUF2_S)
#define SPI_BUF2_V  0xFFFFFFFFU
#define SPI_BUF2_S  0

/** SPI_W3_REG register
 *  SPI CPU-controlled buffer3
 */
#define SPI_W3_REG(i) (REG_SPI_BASE(i) + 0xa4)
/** SPI_BUF3 : R/W/SS; bitpos: [31:0]; default: 0;
 *  32-bit data buffer $n.
 */
#define SPI_BUF3    0xFFFFFFFFU
#define SPI_BUF3_M  (SPI_BUF3_V << SPI_BUF3_S)
#define SPI_BUF3_V  0xFFFFFFFFU
#define SPI_BUF3_S  0

/** SPI_W4_REG register
 *  SPI CPU-controlled buffer4
 */
#define SPI_W4_REG(i) (REG_SPI_BASE(i) + 0xa8)
/** SPI_BUF4 : R/W/SS; bitpos: [31:0]; default: 0;
 *  32-bit data buffer $n.
 */
#define SPI_BUF4    0xFFFFFFFFU
#define SPI_BUF4_M  (SPI_BUF4_V << SPI_BUF4_S)
#define SPI_BUF4_V  0xFFFFFFFFU
#define SPI_BUF4_S  0

/** SPI_W5_REG register
 *  SPI CPU-controlled buffer5
 */
#define SPI_W5_REG(i) (REG_SPI_BASE(i) + 0xac)
/** SPI_BUF5 : R/W/SS; bitpos: [31:0]; default: 0;
 *  32-bit data buffer $n.
 */
#define SPI_BUF5    0xFFFFFFFFU
#define SPI_BUF5_M  (SPI_BUF5_V << SPI_BUF5_S)
#define SPI_BUF5_V  0xFFFFFFFFU
#define SPI_BUF5_S  0

/** SPI_W6_REG register
 *  SPI CPU-controlled buffer6
 */
#define SPI_W6_REG(i) (REG_SPI_BASE(i) + 0xb0)
/** SPI_BUF6 : R/W/SS; bitpos: [31:0]; default: 0;
 *  32-bit data buffer $n.
 */
#define SPI_BUF6    0xFFFFFFFFU
#define SPI_BUF6_M  (SPI_BUF6_V << SPI_BUF6_S)
#define SPI_BUF6_V  0xFFFFFFFFU
#define SPI_BUF6_S  0

/** SPI_W7_REG register
 *  SPI CPU-controlled buffer7
 */
#define SPI_W7_REG(i) (REG_SPI_BASE(i) + 0xb4)
/** SPI_BUF7 : R/W/SS; bitpos: [31:0]; default: 0;
 *  32-bit data buffer $n.
 */
#define SPI_BUF7    0xFFFFFFFFU
#define SPI_BUF7_M  (SPI_BUF7_V << SPI_BUF7_S)
#define SPI_BUF7_V  0xFFFFFFFFU
#define SPI_BUF7_S  0

/** SPI_W8_REG register
 *  SPI CPU-controlled buffer8
 */
#define SPI_W8_REG(i) (REG_SPI_BASE(i) + 0xb8)
/** SPI_BUF8 : R/W/SS; bitpos: [31:0]; default: 0;
 *  32-bit data buffer $n.
 */
#define SPI_BUF8    0xFFFFFFFFU
#define SPI_BUF8_M  (SPI_BUF8_V << SPI_BUF8_S)
#define SPI_BUF8_V  0xFFFFFFFFU
#define SPI_BUF8_S  0

/** SPI_W9_REG register
 *  SPI CPU-controlled buffer9
 */
#define SPI_W9_REG(i) (REG_SPI_BASE(i) + 0xbc)
/** SPI_BUF9 : R/W/SS; bitpos: [31:0]; default: 0;
 *  32-bit data buffer $n.
 */
#define SPI_BUF9    0xFFFFFFFFU
#define SPI_BUF9_M  (SPI_BUF9_V << SPI_BUF9_S)
#define SPI_BUF9_V  0xFFFFFFFFU
#define SPI_BUF9_S  0

/** SPI_W10_REG register
 *  SPI CPU-controlled buffer10
 */
#define SPI_W10_REG(i) (REG_SPI_BASE(i) + 0xc0)
/** SPI_BUF10 : R/W/SS; bitpos: [31:0]; default: 0;
 *  32-bit data buffer $n.
 */
#define SPI_BUF10    0xFFFFFFFFU
#define SPI_BUF10_M  (SPI_BUF10_V << SPI_BUF10_S)
#define SPI_BUF10_V  0xFFFFFFFFU
#define SPI_BUF10_S  0

/** SPI_W11_REG register
 *  SPI CPU-controlled buffer11
 */
#define SPI_W11_REG(i) (REG_SPI_BASE(i) + 0xc4)
/** SPI_BUF11 : R/W/SS; bitpos: [31:0]; default: 0;
 *  32-bit data buffer $n.
 */
#define SPI_BUF11    0xFFFFFFFFU
#define SPI_BUF11_M  (SPI_BUF11_V << SPI_BUF11_S)
#define SPI_BUF11_V  0xFFFFFFFFU
#define SPI_BUF11_S  0

/** SPI_W12_REG register
 *  SPI CPU-controlled buffer12
 */
#define SPI_W12_REG(i) (REG_SPI_BASE(i) + 0xc8)
/** SPI_BUF12 : R/W/SS; bitpos: [31:0]; default: 0;
 *  32-bit data buffer $n.
 */
#define SPI_BUF12    0xFFFFFFFFU
#define SPI_BUF12_M  (SPI_BUF12_V << SPI_BUF12_S)
#define SPI_BUF12_V  0xFFFFFFFFU
#define SPI_BUF12_S  0

/** SPI_W13_REG register
 *  SPI CPU-controlled buffer13
 */
#define SPI_W13_REG(i) (REG_SPI_BASE(i) + 0xcc)
/** SPI_BUF13 : R/W/SS; bitpos: [31:0]; default: 0;
 *  32-bit data buffer $n.
 */
#define SPI_BUF13    0xFFFFFFFFU
#define SPI_BUF13_M  (SPI_BUF13_V << SPI_BUF13_S)
#define SPI_BUF13_V  0xFFFFFFFFU
#define SPI_BUF13_S  0

/** SPI_W14_REG register
 *  SPI CPU-controlled buffer14
 */
#define SPI_W14_REG(i) (REG_SPI_BASE(i) + 0xd0)
/** SPI_BUF14 : R/W/SS; bitpos: [31:0]; default: 0;
 *  32-bit data buffer $n.
 */
#define SPI_BUF14    0xFFFFFFFFU
#define SPI_BUF14_M  (SPI_BUF14_V << SPI_BUF14_S)
#define SPI_BUF14_V  0xFFFFFFFFU
#define SPI_BUF14_S  0

/** SPI_W15_REG register
 *  SPI CPU-controlled buffer15
 */
#define SPI_W15_REG(i) (REG_SPI_BASE(i) + 0xd4)
/** SPI_BUF15 : R/W/SS; bitpos: [31:0]; default: 0;
 *  32-bit data buffer $n.
 */
#define SPI_BUF15    0xFFFFFFFFU
#define SPI_BUF15_M  (SPI_BUF15_V << SPI_BUF15_S)
#define SPI_BUF15_V  0xFFFFFFFFU
#define SPI_BUF15_S  0

/** SPI_SLAVE_REG register
 *  SPI slave control register
 */
#define SPI_SLAVE_REG(i) (REG_SPI_BASE(i) + 0xe0)
/** SPI_CLK_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures SPI clock mode.\\
 *  0: SPI clock is off when CS becomes inactive.\\
 *  1: SPI clock is delayed one cycle after CS becomes inactive.\\
 *  2: SPI clock is delayed two cycles after CS becomes inactive.\\
 *  3: SPI clock is always on.\\
 *  Can be configured in CONF state.
 */
#define SPI_CLK_MODE    0x00000003U
#define SPI_CLK_MODE_M  (SPI_CLK_MODE_V << SPI_CLK_MODE_S)
#define SPI_CLK_MODE_V  0x00000003U
#define SPI_CLK_MODE_S  0
/** SPI_CLK_MODE_13 : R/W; bitpos: [2]; default: 0;
 *  Configure clock mode.\\
 *  0: Support SPI clock mode 0 or 2. See Table <a
 *  href="table:clock_pol_slave">link</a>.\\
 *  1: Support SPI clock mode 1 or 3. See Table <a
 *  href="table:clock_pol_slave">link</a>.\\
 */
#define SPI_CLK_MODE_13    (BIT(2))
#define SPI_CLK_MODE_13_M  (SPI_CLK_MODE_13_V << SPI_CLK_MODE_13_S)
#define SPI_CLK_MODE_13_V  0x00000001U
#define SPI_CLK_MODE_13_S  2
/** SPI_RSCK_DATA_OUT : R/W; bitpos: [3]; default: 0;
 *  Configures the edge of output data.\\
 *  0: Output data at TSCK rising edge.\\
 *  1: Output data at RSCK rising edge.\\
 */
#define SPI_RSCK_DATA_OUT    (BIT(3))
#define SPI_RSCK_DATA_OUT_M  (SPI_RSCK_DATA_OUT_V << SPI_RSCK_DATA_OUT_S)
#define SPI_RSCK_DATA_OUT_V  0x00000001U
#define SPI_RSCK_DATA_OUT_S  3
/** SPI_SLV_RDDMA_BITLEN_EN : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to use SPI_SLV_DATA_BITLEN to store the data bit length
 *  of Rd_DMA transfer.\\
 *  0: Not use\\
 *  1: Use\\
 */
#define SPI_SLV_RDDMA_BITLEN_EN    (BIT(8))
#define SPI_SLV_RDDMA_BITLEN_EN_M  (SPI_SLV_RDDMA_BITLEN_EN_V << SPI_SLV_RDDMA_BITLEN_EN_S)
#define SPI_SLV_RDDMA_BITLEN_EN_V  0x00000001U
#define SPI_SLV_RDDMA_BITLEN_EN_S  8
/** SPI_SLV_WRDMA_BITLEN_EN : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to use SPI_SLV_DATA_BITLEN to store the data bit length
 *  of Wr_DMA transfer.\\
 *  0: Not use\\
 *  1: Use\\
 */
#define SPI_SLV_WRDMA_BITLEN_EN    (BIT(9))
#define SPI_SLV_WRDMA_BITLEN_EN_M  (SPI_SLV_WRDMA_BITLEN_EN_V << SPI_SLV_WRDMA_BITLEN_EN_S)
#define SPI_SLV_WRDMA_BITLEN_EN_V  0x00000001U
#define SPI_SLV_WRDMA_BITLEN_EN_S  9
/** SPI_SLV_RDBUF_BITLEN_EN : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to use SPI_SLV_DATA_BITLEN to store the data bit length
 *  of Rd_BUF transfer.\\
 *  0: Not use\\
 *  1: Use\\
 */
#define SPI_SLV_RDBUF_BITLEN_EN    (BIT(10))
#define SPI_SLV_RDBUF_BITLEN_EN_M  (SPI_SLV_RDBUF_BITLEN_EN_V << SPI_SLV_RDBUF_BITLEN_EN_S)
#define SPI_SLV_RDBUF_BITLEN_EN_V  0x00000001U
#define SPI_SLV_RDBUF_BITLEN_EN_S  10
/** SPI_SLV_WRBUF_BITLEN_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to use SPI_SLV_DATA_BITLEN to store the data bit length
 *  of Wr_BUF transfer.\\
 *  0: Not use\\
 *  1: Use\\
 */
#define SPI_SLV_WRBUF_BITLEN_EN    (BIT(11))
#define SPI_SLV_WRBUF_BITLEN_EN_M  (SPI_SLV_WRBUF_BITLEN_EN_V << SPI_SLV_WRBUF_BITLEN_EN_S)
#define SPI_SLV_WRBUF_BITLEN_EN_V  0x00000001U
#define SPI_SLV_WRBUF_BITLEN_EN_S  11
/** SPI_SLV_LAST_BYTE_STRB : R/SS; bitpos: [19:12]; default: 0;
 *  Represents the effective bit of the last received data byte in SPI slave FD and HD
 *  mode.
 */
#define SPI_SLV_LAST_BYTE_STRB    0x000000FFU
#define SPI_SLV_LAST_BYTE_STRB_M  (SPI_SLV_LAST_BYTE_STRB_V << SPI_SLV_LAST_BYTE_STRB_S)
#define SPI_SLV_LAST_BYTE_STRB_V  0x000000FFU
#define SPI_SLV_LAST_BYTE_STRB_S  12
/** SPI_DMA_SEG_MAGIC_VALUE : R/W; bitpos: [25:22]; default: 10;
 *  Configures the magic value of BM table in DMA-controlled configurable segmented
 *  transfer.
 */
#define SPI_DMA_SEG_MAGIC_VALUE    0x0000000FU
#define SPI_DMA_SEG_MAGIC_VALUE_M  (SPI_DMA_SEG_MAGIC_VALUE_V << SPI_DMA_SEG_MAGIC_VALUE_S)
#define SPI_DMA_SEG_MAGIC_VALUE_V  0x0000000FU
#define SPI_DMA_SEG_MAGIC_VALUE_S  22
/** SPI_SLAVE_MODE : R/W; bitpos: [26]; default: 0;
 *  Configures SPI work mode.\\
 *  0: Master\\
 *  1: Slave\\
 */
#define SPI_SLAVE_MODE    (BIT(26))
#define SPI_SLAVE_MODE_M  (SPI_SLAVE_MODE_V << SPI_SLAVE_MODE_S)
#define SPI_SLAVE_MODE_V  0x00000001U
#define SPI_SLAVE_MODE_S  26
/** SPI_SOFT_RESET : WT; bitpos: [27]; default: 0;
 *  Configures whether to reset the SPI clock line, CS line, and data line via
 *  software.\\
 *  0: Not reset\\
 *  1: Reset\\
 *  Can be configured in CONF state.
 */
#define SPI_SOFT_RESET    (BIT(27))
#define SPI_SOFT_RESET_M  (SPI_SOFT_RESET_V << SPI_SOFT_RESET_S)
#define SPI_SOFT_RESET_V  0x00000001U
#define SPI_SOFT_RESET_S  27
/** SPI_USR_CONF : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to enable the CONF state of current DMA-controlled
 *  configurable segmented transfer.\\
 *  0: No effect, which means the current transfer is not a configurable segmented
 *  transfer.\\
 *  1: Enable, which means a configurable segmented transfer is started.\\
 */
#define SPI_USR_CONF    (BIT(28))
#define SPI_USR_CONF_M  (SPI_USR_CONF_V << SPI_USR_CONF_S)
#define SPI_USR_CONF_V  0x00000001U
#define SPI_USR_CONF_S  28
/** SPI_MST_FD_WAIT_DMA_TX_DATA : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to wait DMA TX data gets ready before starting SPI
 *  transfer in master full-duplex transfer.\\
 *  0: Not wait\\
 *  1: Wait\\
 */
#define SPI_MST_FD_WAIT_DMA_TX_DATA    (BIT(29))
#define SPI_MST_FD_WAIT_DMA_TX_DATA_M  (SPI_MST_FD_WAIT_DMA_TX_DATA_V << SPI_MST_FD_WAIT_DMA_TX_DATA_S)
#define SPI_MST_FD_WAIT_DMA_TX_DATA_V  0x00000001U
#define SPI_MST_FD_WAIT_DMA_TX_DATA_S  29

/** SPI_SLAVE1_REG register
 *  SPI slave control register 1
 */
#define SPI_SLAVE1_REG(i) (REG_SPI_BASE(i) + 0xe4)
/** SPI_SLV_DATA_BITLEN : R/W/SS; bitpos: [17:0]; default: 0;
 *  Configures the transferred data bit length in SPI slave full-/half-duplex modes.
 */
#define SPI_SLV_DATA_BITLEN    0x0003FFFFU
#define SPI_SLV_DATA_BITLEN_M  (SPI_SLV_DATA_BITLEN_V << SPI_SLV_DATA_BITLEN_S)
#define SPI_SLV_DATA_BITLEN_V  0x0003FFFFU
#define SPI_SLV_DATA_BITLEN_S  0
/** SPI_SLV_LAST_COMMAND : R/W/SS; bitpos: [25:18]; default: 0;
 *  Configures the command value in slave mode.
 */
#define SPI_SLV_LAST_COMMAND    0x000000FFU
#define SPI_SLV_LAST_COMMAND_M  (SPI_SLV_LAST_COMMAND_V << SPI_SLV_LAST_COMMAND_S)
#define SPI_SLV_LAST_COMMAND_V  0x000000FFU
#define SPI_SLV_LAST_COMMAND_S  18
/** SPI_SLV_LAST_ADDR : R/W/SS; bitpos: [31:26]; default: 0;
 *  Configures the address value in slave mode.
 */
#define SPI_SLV_LAST_ADDR    0x0000003FU
#define SPI_SLV_LAST_ADDR_M  (SPI_SLV_LAST_ADDR_V << SPI_SLV_LAST_ADDR_S)
#define SPI_SLV_LAST_ADDR_V  0x0000003FU
#define SPI_SLV_LAST_ADDR_S  26

/** SPI_CLK_GATE_REG register
 *  SPI module clock and register clock control
 */
#define SPI_CLK_GATE_REG(i) (REG_SPI_BASE(i) + 0xe8)
/** SPI_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable clock gate.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define SPI_CLK_EN    (BIT(0))
#define SPI_CLK_EN_M  (SPI_CLK_EN_V << SPI_CLK_EN_S)
#define SPI_CLK_EN_V  0x00000001U
#define SPI_CLK_EN_S  0
/** SPI_MST_CLK_ACTIVE : R/W; bitpos: [1]; default: 0;
 *  Set this bit to power on the SPI module clock.
 */
#define SPI_MST_CLK_ACTIVE    (BIT(1))
#define SPI_MST_CLK_ACTIVE_M  (SPI_MST_CLK_ACTIVE_V << SPI_MST_CLK_ACTIVE_S)
#define SPI_MST_CLK_ACTIVE_V  0x00000001U
#define SPI_MST_CLK_ACTIVE_S  1
/** SPI_MST_CLK_SEL : R/W; bitpos: [2]; default: 0;
 *  This bit is used to select SPI module clock source in master mode. 1: PLL_CLK_80M.
 *  0: XTAL CLK.
 */
#define SPI_MST_CLK_SEL    (BIT(2))
#define SPI_MST_CLK_SEL_M  (SPI_MST_CLK_SEL_V << SPI_MST_CLK_SEL_S)
#define SPI_MST_CLK_SEL_V  0x00000001U
#define SPI_MST_CLK_SEL_S  2

/** SPI_DATE_REG register
 *  Version control
 */
#define SPI_DATE_REG(i) (REG_SPI_BASE(i) + 0xf0)
/** SPI_DATE : R/W; bitpos: [27:0]; default: 36716931;
 *  Version control register.
 */
#define SPI_DATE    0x0FFFFFFFU
#define SPI_DATE_M  (SPI_DATE_V << SPI_DATE_S)
#define SPI_DATE_V  0x0FFFFFFFU
#define SPI_DATE_S  0

#ifdef __cplusplus
}
#endif
