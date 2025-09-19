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

/** LP_SPI_CMD_REG register
 *  Command control register
 */
#define LP_SPI_CMD_REG (DR_REG_LP_SPI_BASE + 0x0)
/** LP_REG_UPDATE : WT; bitpos: [23]; default: 0;
 *  Set this bit to synchronize SPI registers from APB clock domain into SPI module
 *  clock domain, which is only used in SPI master mode.
 */
#define LP_REG_UPDATE    (BIT(23))
#define LP_REG_UPDATE_M  (LP_REG_UPDATE_V << LP_REG_UPDATE_S)
#define LP_REG_UPDATE_V  0x00000001U
#define LP_REG_UPDATE_S  23
/** LP_REG_USR : R/W/SC; bitpos: [24]; default: 0;
 *  User define command enable.  An operation will be triggered when the bit is set.
 *  The bit will be cleared once the operation done.1: enable 0: disable. Can not be
 *  changed by CONF_buf.
 */
#define LP_REG_USR    (BIT(24))
#define LP_REG_USR_M  (LP_REG_USR_V << LP_REG_USR_S)
#define LP_REG_USR_V  0x00000001U
#define LP_REG_USR_S  24

/** LP_SPI_ADDR_REG register
 *  Address value register
 */
#define LP_SPI_ADDR_REG (DR_REG_LP_SPI_BASE + 0x4)
/** LP_REG_USR_ADDR_VALUE : R/W; bitpos: [31:0]; default: 0;
 *  Address to slave. Can be configured in CONF state.
 */
#define LP_REG_USR_ADDR_VALUE    0xFFFFFFFFU
#define LP_REG_USR_ADDR_VALUE_M  (LP_REG_USR_ADDR_VALUE_V << LP_REG_USR_ADDR_VALUE_S)
#define LP_REG_USR_ADDR_VALUE_V  0xFFFFFFFFU
#define LP_REG_USR_ADDR_VALUE_S  0

/** LP_SPI_CTRL_REG register
 *  SPI control register
 */
#define LP_SPI_CTRL_REG (DR_REG_LP_SPI_BASE + 0x8)
/** LP_REG_DUMMY_OUT : R/W; bitpos: [3]; default: 0;
 *  In the dummy phase the signal level of spi is output by the spi controller. Can be
 *  configured in CONF state.
 */
#define LP_REG_DUMMY_OUT    (BIT(3))
#define LP_REG_DUMMY_OUT_M  (LP_REG_DUMMY_OUT_V << LP_REG_DUMMY_OUT_S)
#define LP_REG_DUMMY_OUT_V  0x00000001U
#define LP_REG_DUMMY_OUT_S  3
/** LP_REG_Q_POL : R/W; bitpos: [18]; default: 1;
 *  The bit is used to set MISO line polarity, 1: high 0, low. Can be configured in
 *  CONF state.
 */
#define LP_REG_Q_POL    (BIT(18))
#define LP_REG_Q_POL_M  (LP_REG_Q_POL_V << LP_REG_Q_POL_S)
#define LP_REG_Q_POL_V  0x00000001U
#define LP_REG_Q_POL_S  18
/** LP_REG_D_POL : R/W; bitpos: [19]; default: 1;
 *  The bit is used to set MOSI line polarity, 1: high 0, low. Can be configured in
 *  CONF state.
 */
#define LP_REG_D_POL    (BIT(19))
#define LP_REG_D_POL_M  (LP_REG_D_POL_V << LP_REG_D_POL_S)
#define LP_REG_D_POL_V  0x00000001U
#define LP_REG_D_POL_S  19
/** LP_REG_RD_BIT_ORDER : R/W; bitpos: [25]; default: 0;
 *  In read-data (MISO) phase 1: LSB first 0: MSB first. Can be configured in CONF
 *  state.
 */
#define LP_REG_RD_BIT_ORDER    (BIT(25))
#define LP_REG_RD_BIT_ORDER_M  (LP_REG_RD_BIT_ORDER_V << LP_REG_RD_BIT_ORDER_S)
#define LP_REG_RD_BIT_ORDER_V  0x00000001U
#define LP_REG_RD_BIT_ORDER_S  25
/** LP_REG_WR_BIT_ORDER : R/W; bitpos: [26]; default: 0;
 *  In command address write-data (MOSI) phases 1: LSB firs 0: MSB first. Can be
 *  configured in CONF state.
 */
#define LP_REG_WR_BIT_ORDER    (BIT(26))
#define LP_REG_WR_BIT_ORDER_M  (LP_REG_WR_BIT_ORDER_V << LP_REG_WR_BIT_ORDER_S)
#define LP_REG_WR_BIT_ORDER_V  0x00000001U
#define LP_REG_WR_BIT_ORDER_S  26

/** LP_SPI_CLOCK_REG register
 *  SPI clock control register
 */
#define LP_SPI_CLOCK_REG (DR_REG_LP_SPI_BASE + 0xc)
/** LP_REG_CLKCNT_L : R/W; bitpos: [5:0]; default: 3;
 *  In the master mode it must be equal to spi_clkcnt_N. In the slave mode it must be
 *  0. Can be configured in CONF state.
 */
#define LP_REG_CLKCNT_L    0x0000003FU
#define LP_REG_CLKCNT_L_M  (LP_REG_CLKCNT_L_V << LP_REG_CLKCNT_L_S)
#define LP_REG_CLKCNT_L_V  0x0000003FU
#define LP_REG_CLKCNT_L_S  0
/** LP_REG_CLKCNT_H : R/W; bitpos: [11:6]; default: 1;
 *  In the master mode it must be floor((spi_clkcnt_N+1)/2-1). In the slave mode it
 *  must be 0. Can be configured in CONF state.
 */
#define LP_REG_CLKCNT_H    0x0000003FU
#define LP_REG_CLKCNT_H_M  (LP_REG_CLKCNT_H_V << LP_REG_CLKCNT_H_S)
#define LP_REG_CLKCNT_H_V  0x0000003FU
#define LP_REG_CLKCNT_H_S  6
/** LP_REG_CLKCNT_N : R/W; bitpos: [17:12]; default: 3;
 *  In the master mode it is the divider of spi_clk. So spi_clk frequency is
 *  system/(spi_clkdiv_pre+1)/(spi_clkcnt_N+1). Can be configured in CONF state.
 */
#define LP_REG_CLKCNT_N    0x0000003FU
#define LP_REG_CLKCNT_N_M  (LP_REG_CLKCNT_N_V << LP_REG_CLKCNT_N_S)
#define LP_REG_CLKCNT_N_V  0x0000003FU
#define LP_REG_CLKCNT_N_S  12
/** LP_REG_CLKDIV_PRE : R/W; bitpos: [21:18]; default: 0;
 *  In the master mode it is pre-divider of spi_clk.  Can be configured in CONF state.
 */
#define LP_REG_CLKDIV_PRE    0x0000000FU
#define LP_REG_CLKDIV_PRE_M  (LP_REG_CLKDIV_PRE_V << LP_REG_CLKDIV_PRE_S)
#define LP_REG_CLKDIV_PRE_V  0x0000000FU
#define LP_REG_CLKDIV_PRE_S  18
/** LP_REG_CLK_EQU_SYSCLK : R/W; bitpos: [31]; default: 1;
 *  In the master mode 1: spi_clk is equal to system 0: spi_clk is divided from system
 *  clock. Can be configured in CONF state.
 */
#define LP_REG_CLK_EQU_SYSCLK    (BIT(31))
#define LP_REG_CLK_EQU_SYSCLK_M  (LP_REG_CLK_EQU_SYSCLK_V << LP_REG_CLK_EQU_SYSCLK_S)
#define LP_REG_CLK_EQU_SYSCLK_V  0x00000001U
#define LP_REG_CLK_EQU_SYSCLK_S  31

/** LP_SPI_USER_REG register
 *  SPI USER control register
 */
#define LP_SPI_USER_REG (DR_REG_LP_SPI_BASE + 0x10)
/** LP_REG_DOUTDIN : R/W; bitpos: [0]; default: 0;
 *  Set the bit to enable full duplex communication. 1: enable 0: disable. Can be
 *  configured in CONF state.
 */
#define LP_REG_DOUTDIN    (BIT(0))
#define LP_REG_DOUTDIN_M  (LP_REG_DOUTDIN_V << LP_REG_DOUTDIN_S)
#define LP_REG_DOUTDIN_V  0x00000001U
#define LP_REG_DOUTDIN_S  0
/** LP_REG_TSCK_I_EDGE : R/W; bitpos: [5]; default: 0;
 *  In the slave mode, this bit can be used to change the polarity of tsck. 0: tsck =
 *  spi_ck_i. 1:tsck = !spi_ck_i.
 */
#define LP_REG_TSCK_I_EDGE    (BIT(5))
#define LP_REG_TSCK_I_EDGE_M  (LP_REG_TSCK_I_EDGE_V << LP_REG_TSCK_I_EDGE_S)
#define LP_REG_TSCK_I_EDGE_V  0x00000001U
#define LP_REG_TSCK_I_EDGE_S  5
/** LP_REG_CS_HOLD : R/W; bitpos: [6]; default: 1;
 *  spi cs keep low when spi is in  done  phase. 1: enable 0: disable. Can be
 *  configured in CONF state.
 */
#define LP_REG_CS_HOLD    (BIT(6))
#define LP_REG_CS_HOLD_M  (LP_REG_CS_HOLD_V << LP_REG_CS_HOLD_S)
#define LP_REG_CS_HOLD_V  0x00000001U
#define LP_REG_CS_HOLD_S  6
/** LP_REG_CS_SETUP : R/W; bitpos: [7]; default: 1;
 *  spi cs is enable when spi is in  prepare  phase. 1: enable 0: disable. Can be
 *  configured in CONF state.
 */
#define LP_REG_CS_SETUP    (BIT(7))
#define LP_REG_CS_SETUP_M  (LP_REG_CS_SETUP_V << LP_REG_CS_SETUP_S)
#define LP_REG_CS_SETUP_V  0x00000001U
#define LP_REG_CS_SETUP_S  7
/** LP_REG_RSCK_I_EDGE : R/W; bitpos: [8]; default: 0;
 *  In the slave mode, this bit can be used to change the polarity of rsck. 0: rsck =
 *  !spi_ck_i. 1:rsck = spi_ck_i.
 */
#define LP_REG_RSCK_I_EDGE    (BIT(8))
#define LP_REG_RSCK_I_EDGE_M  (LP_REG_RSCK_I_EDGE_V << LP_REG_RSCK_I_EDGE_S)
#define LP_REG_RSCK_I_EDGE_V  0x00000001U
#define LP_REG_RSCK_I_EDGE_S  8
/** LP_REG_CK_OUT_EDGE : R/W; bitpos: [9]; default: 0;
 *  the bit combined with spi_mosi_delay_mode bits to set mosi signal delay mode. Can
 *  be configured in CONF state.
 */
#define LP_REG_CK_OUT_EDGE    (BIT(9))
#define LP_REG_CK_OUT_EDGE_M  (LP_REG_CK_OUT_EDGE_V << LP_REG_CK_OUT_EDGE_S)
#define LP_REG_CK_OUT_EDGE_V  0x00000001U
#define LP_REG_CK_OUT_EDGE_S  9
/** LP_REG_SIO : R/W; bitpos: [17]; default: 0;
 *  Set the bit to enable 3-line half duplex communication mosi and miso signals share
 *  the same pin. 1: enable 0: disable. Can be configured in CONF state.
 */
#define LP_REG_SIO    (BIT(17))
#define LP_REG_SIO_M  (LP_REG_SIO_V << LP_REG_SIO_S)
#define LP_REG_SIO_V  0x00000001U
#define LP_REG_SIO_S  17
/** LP_REG_USR_MISO_HIGHPART : R/W; bitpos: [24]; default: 0;
 *  read-data phase only access to high-part of the buffer spi_w8~spi_w15. 1: enable 0:
 *  disable. Can be configured in CONF state.
 */
#define LP_REG_USR_MISO_HIGHPART    (BIT(24))
#define LP_REG_USR_MISO_HIGHPART_M  (LP_REG_USR_MISO_HIGHPART_V << LP_REG_USR_MISO_HIGHPART_S)
#define LP_REG_USR_MISO_HIGHPART_V  0x00000001U
#define LP_REG_USR_MISO_HIGHPART_S  24
/** LP_REG_USR_MOSI_HIGHPART : R/W; bitpos: [25]; default: 0;
 *  write-data phase only access to high-part of the buffer spi_w8~spi_w15. 1: enable
 *  0: disable.  Can be configured in CONF state.
 */
#define LP_REG_USR_MOSI_HIGHPART    (BIT(25))
#define LP_REG_USR_MOSI_HIGHPART_M  (LP_REG_USR_MOSI_HIGHPART_V << LP_REG_USR_MOSI_HIGHPART_S)
#define LP_REG_USR_MOSI_HIGHPART_V  0x00000001U
#define LP_REG_USR_MOSI_HIGHPART_S  25
/** LP_REG_USR_DUMMY_IDLE : R/W; bitpos: [26]; default: 0;
 *  spi clock is disable in dummy phase when the bit is enable. Can be configured in
 *  CONF state.
 */
#define LP_REG_USR_DUMMY_IDLE    (BIT(26))
#define LP_REG_USR_DUMMY_IDLE_M  (LP_REG_USR_DUMMY_IDLE_V << LP_REG_USR_DUMMY_IDLE_S)
#define LP_REG_USR_DUMMY_IDLE_V  0x00000001U
#define LP_REG_USR_DUMMY_IDLE_S  26
/** LP_REG_USR_MOSI : R/W; bitpos: [27]; default: 0;
 *  This bit enable the write-data phase of an operation. Can be configured in CONF
 *  state.
 */
#define LP_REG_USR_MOSI    (BIT(27))
#define LP_REG_USR_MOSI_M  (LP_REG_USR_MOSI_V << LP_REG_USR_MOSI_S)
#define LP_REG_USR_MOSI_V  0x00000001U
#define LP_REG_USR_MOSI_S  27
/** LP_REG_USR_MISO : R/W; bitpos: [28]; default: 0;
 *  This bit enable the read-data phase of an operation. Can be configured in CONF
 *  state.
 */
#define LP_REG_USR_MISO    (BIT(28))
#define LP_REG_USR_MISO_M  (LP_REG_USR_MISO_V << LP_REG_USR_MISO_S)
#define LP_REG_USR_MISO_V  0x00000001U
#define LP_REG_USR_MISO_S  28
/** LP_REG_USR_DUMMY : R/W; bitpos: [29]; default: 0;
 *  This bit enable the dummy phase of an operation. Can be configured in CONF state.
 */
#define LP_REG_USR_DUMMY    (BIT(29))
#define LP_REG_USR_DUMMY_M  (LP_REG_USR_DUMMY_V << LP_REG_USR_DUMMY_S)
#define LP_REG_USR_DUMMY_V  0x00000001U
#define LP_REG_USR_DUMMY_S  29
/** LP_REG_USR_ADDR : R/W; bitpos: [30]; default: 0;
 *  This bit enable the address phase of an operation. Can be configured in CONF state.
 */
#define LP_REG_USR_ADDR    (BIT(30))
#define LP_REG_USR_ADDR_M  (LP_REG_USR_ADDR_V << LP_REG_USR_ADDR_S)
#define LP_REG_USR_ADDR_V  0x00000001U
#define LP_REG_USR_ADDR_S  30
/** LP_REG_USR_COMMAND : R/W; bitpos: [31]; default: 1;
 *  This bit enable the command phase of an operation. Can be configured in CONF state.
 */
#define LP_REG_USR_COMMAND    (BIT(31))
#define LP_REG_USR_COMMAND_M  (LP_REG_USR_COMMAND_V << LP_REG_USR_COMMAND_S)
#define LP_REG_USR_COMMAND_V  0x00000001U
#define LP_REG_USR_COMMAND_S  31

/** LP_SPI_USER1_REG register
 *  SPI USER control register 1
 */
#define LP_SPI_USER1_REG (DR_REG_LP_SPI_BASE + 0x14)
/** LP_REG_USR_DUMMY_CYCLELEN : R/W; bitpos: [7:0]; default: 7;
 *  The length in spi_clk cycles of dummy phase. The register value shall be
 *  (cycle_num-1). Can be configured in CONF state.
 */
#define LP_REG_USR_DUMMY_CYCLELEN    0x000000FFU
#define LP_REG_USR_DUMMY_CYCLELEN_M  (LP_REG_USR_DUMMY_CYCLELEN_V << LP_REG_USR_DUMMY_CYCLELEN_S)
#define LP_REG_USR_DUMMY_CYCLELEN_V  0x000000FFU
#define LP_REG_USR_DUMMY_CYCLELEN_S  0
/** LP_REG_MST_WFULL_ERR_END_EN : R/W; bitpos: [16]; default: 1;
 *  1: SPI transfer is ended when SPI RX AFIFO wfull error is valid in GP-SPI master
 *  FD/HD-mode. 0: SPI transfer is not ended when SPI RX AFIFO wfull error is valid in
 *  GP-SPI master FD/HD-mode.
 */
#define LP_REG_MST_WFULL_ERR_END_EN    (BIT(16))
#define LP_REG_MST_WFULL_ERR_END_EN_M  (LP_REG_MST_WFULL_ERR_END_EN_V << LP_REG_MST_WFULL_ERR_END_EN_S)
#define LP_REG_MST_WFULL_ERR_END_EN_V  0x00000001U
#define LP_REG_MST_WFULL_ERR_END_EN_S  16
/** LP_REG_CS_SETUP_TIME : R/W; bitpos: [21:17]; default: 0;
 *  (cycles+1) of prepare phase by spi clock this bits are combined with spi_cs_setup
 *  bit. Can be configured in CONF state.
 */
#define LP_REG_CS_SETUP_TIME    0x0000001FU
#define LP_REG_CS_SETUP_TIME_M  (LP_REG_CS_SETUP_TIME_V << LP_REG_CS_SETUP_TIME_S)
#define LP_REG_CS_SETUP_TIME_V  0x0000001FU
#define LP_REG_CS_SETUP_TIME_S  17
/** LP_REG_CS_HOLD_TIME : R/W; bitpos: [26:22]; default: 1;
 *  delay cycles of cs pin by spi clock this bits are combined with spi_cs_hold bit.
 *  Can be configured in CONF state.
 */
#define LP_REG_CS_HOLD_TIME    0x0000001FU
#define LP_REG_CS_HOLD_TIME_M  (LP_REG_CS_HOLD_TIME_V << LP_REG_CS_HOLD_TIME_S)
#define LP_REG_CS_HOLD_TIME_V  0x0000001FU
#define LP_REG_CS_HOLD_TIME_S  22
/** LP_REG_USR_ADDR_BITLEN : R/W; bitpos: [31:27]; default: 23;
 *  The length in bits of address phase. The register value shall be (bit_num-1). Can
 *  be configured in CONF state.
 */
#define LP_REG_USR_ADDR_BITLEN    0x0000001FU
#define LP_REG_USR_ADDR_BITLEN_M  (LP_REG_USR_ADDR_BITLEN_V << LP_REG_USR_ADDR_BITLEN_S)
#define LP_REG_USR_ADDR_BITLEN_V  0x0000001FU
#define LP_REG_USR_ADDR_BITLEN_S  27

/** LP_SPI_USER2_REG register
 *  SPI USER control register 2
 */
#define LP_SPI_USER2_REG (DR_REG_LP_SPI_BASE + 0x18)
/** LP_REG_USR_COMMAND_VALUE : R/W; bitpos: [15:0]; default: 0;
 *  The value of  command. Can be configured in CONF state.
 */
#define LP_REG_USR_COMMAND_VALUE    0x0000FFFFU
#define LP_REG_USR_COMMAND_VALUE_M  (LP_REG_USR_COMMAND_VALUE_V << LP_REG_USR_COMMAND_VALUE_S)
#define LP_REG_USR_COMMAND_VALUE_V  0x0000FFFFU
#define LP_REG_USR_COMMAND_VALUE_S  0
/** LP_REG_MST_REMPTY_ERR_END_EN : R/W; bitpos: [27]; default: 1;
 *  1: SPI transfer is ended when SPI TX AFIFO read empty error is valid in GP-SPI
 *  master FD/HD-mode. 0: SPI transfer is not ended when SPI TX AFIFO read empty error
 *  is valid in GP-SPI master FD/HD-mode.
 */
#define LP_REG_MST_REMPTY_ERR_END_EN    (BIT(27))
#define LP_REG_MST_REMPTY_ERR_END_EN_M  (LP_REG_MST_REMPTY_ERR_END_EN_V << LP_REG_MST_REMPTY_ERR_END_EN_S)
#define LP_REG_MST_REMPTY_ERR_END_EN_V  0x00000001U
#define LP_REG_MST_REMPTY_ERR_END_EN_S  27
/** LP_REG_USR_COMMAND_BITLEN : R/W; bitpos: [31:28]; default: 7;
 *  The length in bits of command phase. The register value shall be (bit_num-1). Can
 *  be configured in CONF state.
 */
#define LP_REG_USR_COMMAND_BITLEN    0x0000000FU
#define LP_REG_USR_COMMAND_BITLEN_M  (LP_REG_USR_COMMAND_BITLEN_V << LP_REG_USR_COMMAND_BITLEN_S)
#define LP_REG_USR_COMMAND_BITLEN_V  0x0000000FU
#define LP_REG_USR_COMMAND_BITLEN_S  28

/** LP_SPI_MS_DLEN_REG register
 *  SPI data bit length control register
 */
#define LP_SPI_MS_DLEN_REG (DR_REG_LP_SPI_BASE + 0x1c)
/** LP_REG_MS_DATA_BITLEN : R/W; bitpos: [17:0]; default: 0;
 *  The value of these bits is the configured SPI transmission data bit length in
 *  master mode DMA controlled transfer or CPU controlled transfer. The value is also
 *  the configured bit length in slave mode DMA RX controlled transfer. The register
 *  value shall be (bit_num-1). Can be configured in CONF state.
 */
#define LP_REG_MS_DATA_BITLEN    0x0003FFFFU
#define LP_REG_MS_DATA_BITLEN_M  (LP_REG_MS_DATA_BITLEN_V << LP_REG_MS_DATA_BITLEN_S)
#define LP_REG_MS_DATA_BITLEN_V  0x0003FFFFU
#define LP_REG_MS_DATA_BITLEN_S  0

/** LP_SPI_MISC_REG register
 *  SPI misc register
 */
#define LP_SPI_MISC_REG (DR_REG_LP_SPI_BASE + 0x20)
/** LP_REG_CS0_DIS : R/W; bitpos: [0]; default: 0;
 *  SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Can
 *  be configured in CONF state.
 */
#define LP_REG_CS0_DIS    (BIT(0))
#define LP_REG_CS0_DIS_M  (LP_REG_CS0_DIS_V << LP_REG_CS0_DIS_S)
#define LP_REG_CS0_DIS_V  0x00000001U
#define LP_REG_CS0_DIS_S  0
/** LP_REG_CK_DIS : R/W; bitpos: [6]; default: 0;
 *  1: spi clk out disable,  0: spi clk out enable. Can be configured in CONF state.
 */
#define LP_REG_CK_DIS    (BIT(6))
#define LP_REG_CK_DIS_M  (LP_REG_CK_DIS_V << LP_REG_CK_DIS_S)
#define LP_REG_CK_DIS_V  0x00000001U
#define LP_REG_CK_DIS_S  6
/** LP_REG_MASTER_CS_POL : R/W; bitpos: [9:7]; default: 0;
 *  In the master mode the bits are the polarity of spi cs line, the value is
 *  equivalent to spi_cs ^ spi_master_cs_pol. Can be configured in CONF state.
 */
#define LP_REG_MASTER_CS_POL    0x00000007U
#define LP_REG_MASTER_CS_POL_M  (LP_REG_MASTER_CS_POL_V << LP_REG_MASTER_CS_POL_S)
#define LP_REG_MASTER_CS_POL_V  0x00000007U
#define LP_REG_MASTER_CS_POL_S  7
/** LP_REG_SLAVE_CS_POL : R/W; bitpos: [23]; default: 0;
 *  spi slave input cs polarity select. 1: inv  0: not change. Can be configured in
 *  CONF state.
 */
#define LP_REG_SLAVE_CS_POL    (BIT(23))
#define LP_REG_SLAVE_CS_POL_M  (LP_REG_SLAVE_CS_POL_V << LP_REG_SLAVE_CS_POL_S)
#define LP_REG_SLAVE_CS_POL_V  0x00000001U
#define LP_REG_SLAVE_CS_POL_S  23
/** LP_REG_CK_IDLE_EDGE : R/W; bitpos: [29]; default: 0;
 *  1: spi clk line is high when idle     0: spi clk line is low when idle. Can be
 *  configured in CONF state.
 */
#define LP_REG_CK_IDLE_EDGE    (BIT(29))
#define LP_REG_CK_IDLE_EDGE_M  (LP_REG_CK_IDLE_EDGE_V << LP_REG_CK_IDLE_EDGE_S)
#define LP_REG_CK_IDLE_EDGE_V  0x00000001U
#define LP_REG_CK_IDLE_EDGE_S  29
/** LP_REG_CS_KEEP_ACTIVE : R/W; bitpos: [30]; default: 0;
 *  spi cs line keep low when the bit is set. Can be configured in CONF state.
 */
#define LP_REG_CS_KEEP_ACTIVE    (BIT(30))
#define LP_REG_CS_KEEP_ACTIVE_M  (LP_REG_CS_KEEP_ACTIVE_V << LP_REG_CS_KEEP_ACTIVE_S)
#define LP_REG_CS_KEEP_ACTIVE_V  0x00000001U
#define LP_REG_CS_KEEP_ACTIVE_S  30

/** LP_SPI_DIN_MODE_REG register
 *  SPI input delay mode configuration
 */
#define LP_SPI_DIN_MODE_REG (DR_REG_LP_SPI_BASE + 0x24)
/** LP_REG_DIN0_MODE : R/W; bitpos: [1:0]; default: 0;
 *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
 *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
 *  with the spi_clk. Can be configured in CONF state.
 */
#define LP_REG_DIN0_MODE    0x00000003U
#define LP_REG_DIN0_MODE_M  (LP_REG_DIN0_MODE_V << LP_REG_DIN0_MODE_S)
#define LP_REG_DIN0_MODE_V  0x00000003U
#define LP_REG_DIN0_MODE_S  0
/** LP_REG_DIN1_MODE : R/W; bitpos: [3:2]; default: 0;
 *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
 *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
 *  with the spi_clk. Can be configured in CONF state.
 */
#define LP_REG_DIN1_MODE    0x00000003U
#define LP_REG_DIN1_MODE_M  (LP_REG_DIN1_MODE_V << LP_REG_DIN1_MODE_S)
#define LP_REG_DIN1_MODE_V  0x00000003U
#define LP_REG_DIN1_MODE_S  2
/** LP_REG_DIN2_MODE : R/W; bitpos: [5:4]; default: 0;
 *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
 *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
 *  with the spi_clk. Can be configured in CONF state.
 */
#define LP_REG_DIN2_MODE    0x00000003U
#define LP_REG_DIN2_MODE_M  (LP_REG_DIN2_MODE_V << LP_REG_DIN2_MODE_S)
#define LP_REG_DIN2_MODE_V  0x00000003U
#define LP_REG_DIN2_MODE_S  4
/** LP_REG_DIN3_MODE : R/W; bitpos: [7:6]; default: 0;
 *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
 *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
 *  with the spi_clk. Can be configured in CONF state.
 */
#define LP_REG_DIN3_MODE    0x00000003U
#define LP_REG_DIN3_MODE_M  (LP_REG_DIN3_MODE_V << LP_REG_DIN3_MODE_S)
#define LP_REG_DIN3_MODE_V  0x00000003U
#define LP_REG_DIN3_MODE_S  6
/** LP_REG_TIMING_HCLK_ACTIVE : R/W; bitpos: [16]; default: 0;
 *  1:enable hclk in SPI input timing module.  0: disable it. Can be configured in CONF
 *  state.
 */
#define LP_REG_TIMING_HCLK_ACTIVE    (BIT(16))
#define LP_REG_TIMING_HCLK_ACTIVE_M  (LP_REG_TIMING_HCLK_ACTIVE_V << LP_REG_TIMING_HCLK_ACTIVE_S)
#define LP_REG_TIMING_HCLK_ACTIVE_V  0x00000001U
#define LP_REG_TIMING_HCLK_ACTIVE_S  16

/** LP_SPI_DIN_NUM_REG register
 *  SPI input delay number configuration
 */
#define LP_SPI_DIN_NUM_REG (DR_REG_LP_SPI_BASE + 0x28)
/** LP_REG_DIN0_NUM : R/W; bitpos: [1:0]; default: 0;
 *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
 *  delayed by 2 cycles,...  Can be configured in CONF state.
 */
#define LP_REG_DIN0_NUM    0x00000003U
#define LP_REG_DIN0_NUM_M  (LP_REG_DIN0_NUM_V << LP_REG_DIN0_NUM_S)
#define LP_REG_DIN0_NUM_V  0x00000003U
#define LP_REG_DIN0_NUM_S  0
/** LP_REG_DIN1_NUM : R/W; bitpos: [3:2]; default: 0;
 *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
 *  delayed by 2 cycles,...  Can be configured in CONF state.
 */
#define LP_REG_DIN1_NUM    0x00000003U
#define LP_REG_DIN1_NUM_M  (LP_REG_DIN1_NUM_V << LP_REG_DIN1_NUM_S)
#define LP_REG_DIN1_NUM_V  0x00000003U
#define LP_REG_DIN1_NUM_S  2
/** LP_REG_DIN2_NUM : R/W; bitpos: [5:4]; default: 0;
 *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
 *  delayed by 2 cycles,...  Can be configured in CONF state.
 */
#define LP_REG_DIN2_NUM    0x00000003U
#define LP_REG_DIN2_NUM_M  (LP_REG_DIN2_NUM_V << LP_REG_DIN2_NUM_S)
#define LP_REG_DIN2_NUM_V  0x00000003U
#define LP_REG_DIN2_NUM_S  4
/** LP_REG_DIN3_NUM : R/W; bitpos: [7:6]; default: 0;
 *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
 *  delayed by 2 cycles,...  Can be configured in CONF state.
 */
#define LP_REG_DIN3_NUM    0x00000003U
#define LP_REG_DIN3_NUM_M  (LP_REG_DIN3_NUM_V << LP_REG_DIN3_NUM_S)
#define LP_REG_DIN3_NUM_V  0x00000003U
#define LP_REG_DIN3_NUM_S  6

/** LP_SPI_DOUT_MODE_REG register
 *  SPI output delay mode configuration
 */
#define LP_SPI_DOUT_MODE_REG (DR_REG_LP_SPI_BASE + 0x2c)
/** LP_REG_DOUT0_MODE : R/W; bitpos: [0]; default: 0;
 *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
 *  1: output delay for a SPI module clock cycle at its negative edge. Can be
 *  configured in CONF state.
 */
#define LP_REG_DOUT0_MODE    (BIT(0))
#define LP_REG_DOUT0_MODE_M  (LP_REG_DOUT0_MODE_V << LP_REG_DOUT0_MODE_S)
#define LP_REG_DOUT0_MODE_V  0x00000001U
#define LP_REG_DOUT0_MODE_S  0
/** LP_REG_DOUT1_MODE : R/W; bitpos: [1]; default: 0;
 *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
 *  1: output delay for a SPI module clock cycle at its negative edge. Can be
 *  configured in CONF state.
 */
#define LP_REG_DOUT1_MODE    (BIT(1))
#define LP_REG_DOUT1_MODE_M  (LP_REG_DOUT1_MODE_V << LP_REG_DOUT1_MODE_S)
#define LP_REG_DOUT1_MODE_V  0x00000001U
#define LP_REG_DOUT1_MODE_S  1
/** LP_REG_DOUT2_MODE : R/W; bitpos: [2]; default: 0;
 *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
 *  1: output delay for a SPI module clock cycle at its negative edge. Can be
 *  configured in CONF state.
 */
#define LP_REG_DOUT2_MODE    (BIT(2))
#define LP_REG_DOUT2_MODE_M  (LP_REG_DOUT2_MODE_V << LP_REG_DOUT2_MODE_S)
#define LP_REG_DOUT2_MODE_V  0x00000001U
#define LP_REG_DOUT2_MODE_S  2
/** LP_REG_DOUT3_MODE : R/W; bitpos: [3]; default: 0;
 *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
 *  1: output delay for a SPI module clock cycle at its negative edge. Can be
 *  configured in CONF state.
 */
#define LP_REG_DOUT3_MODE    (BIT(3))
#define LP_REG_DOUT3_MODE_M  (LP_REG_DOUT3_MODE_V << LP_REG_DOUT3_MODE_S)
#define LP_REG_DOUT3_MODE_V  0x00000001U
#define LP_REG_DOUT3_MODE_S  3

/** LP_SPI_DMA_CONF_REG register
 *  SPI DMA control register
 */
#define LP_SPI_DMA_CONF_REG (DR_REG_LP_SPI_BASE + 0x30)
/** LP_REG_RX_AFIFO_RST : WT; bitpos: [29]; default: 0;
 *  Set this bit to reset RX AFIFO, which is used to receive data in SPI master and
 *  slave mode transfer.
 */
#define LP_REG_RX_AFIFO_RST    (BIT(29))
#define LP_REG_RX_AFIFO_RST_M  (LP_REG_RX_AFIFO_RST_V << LP_REG_RX_AFIFO_RST_S)
#define LP_REG_RX_AFIFO_RST_V  0x00000001U
#define LP_REG_RX_AFIFO_RST_S  29
/** LP_REG_BUF_AFIFO_RST : WT; bitpos: [30]; default: 0;
 *  Set this bit to reset BUF TX AFIFO, which is used send data out in SPI slave CPU
 *  controlled mode transfer and master mode transfer.
 */
#define LP_REG_BUF_AFIFO_RST    (BIT(30))
#define LP_REG_BUF_AFIFO_RST_M  (LP_REG_BUF_AFIFO_RST_V << LP_REG_BUF_AFIFO_RST_S)
#define LP_REG_BUF_AFIFO_RST_V  0x00000001U
#define LP_REG_BUF_AFIFO_RST_S  30

/** LP_SPI_DMA_INT_ENA_REG register
 *  SPI DMA interrupt enable register
 */
#define LP_SPI_DMA_INT_ENA_REG (DR_REG_LP_SPI_BASE + 0x34)
/** LP_REG_SLV_RD_BUF_DONE_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  The enable bit for SPI_SLV_RD_BUF_DONE_INT interrupt.
 */
#define LP_REG_SLV_RD_BUF_DONE_INT_ENA    (BIT(10))
#define LP_REG_SLV_RD_BUF_DONE_INT_ENA_M  (LP_REG_SLV_RD_BUF_DONE_INT_ENA_V << LP_REG_SLV_RD_BUF_DONE_INT_ENA_S)
#define LP_REG_SLV_RD_BUF_DONE_INT_ENA_V  0x00000001U
#define LP_REG_SLV_RD_BUF_DONE_INT_ENA_S  10
/** LP_REG_SLV_WR_BUF_DONE_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  The enable bit for SPI_SLV_WR_BUF_DONE_INT interrupt.
 */
#define LP_REG_SLV_WR_BUF_DONE_INT_ENA    (BIT(11))
#define LP_REG_SLV_WR_BUF_DONE_INT_ENA_M  (LP_REG_SLV_WR_BUF_DONE_INT_ENA_V << LP_REG_SLV_WR_BUF_DONE_INT_ENA_S)
#define LP_REG_SLV_WR_BUF_DONE_INT_ENA_V  0x00000001U
#define LP_REG_SLV_WR_BUF_DONE_INT_ENA_S  11
/** LP_REG_TRANS_DONE_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  The enable bit for SPI_TRANS_DONE_INT interrupt.
 */
#define LP_REG_TRANS_DONE_INT_ENA    (BIT(12))
#define LP_REG_TRANS_DONE_INT_ENA_M  (LP_REG_TRANS_DONE_INT_ENA_V << LP_REG_TRANS_DONE_INT_ENA_S)
#define LP_REG_TRANS_DONE_INT_ENA_V  0x00000001U
#define LP_REG_TRANS_DONE_INT_ENA_S  12
/** LP_REG_SPI_WAKEUP_INT_ENA : R/W; bitpos: [14]; default: 0;
 *  The enable bit for SPI_WAKEUP_INT interrupt
 */
#define LP_REG_SPI_WAKEUP_INT_ENA    (BIT(14))
#define LP_REG_SPI_WAKEUP_INT_ENA_M  (LP_REG_SPI_WAKEUP_INT_ENA_V << LP_REG_SPI_WAKEUP_INT_ENA_S)
#define LP_REG_SPI_WAKEUP_INT_ENA_V  0x00000001U
#define LP_REG_SPI_WAKEUP_INT_ENA_S  14
/** LP_REG_SLV_BUF_ADDR_ERR_INT_ENA : R/W; bitpos: [15]; default: 0;
 *  The enable bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.
 */
#define LP_REG_SLV_BUF_ADDR_ERR_INT_ENA    (BIT(15))
#define LP_REG_SLV_BUF_ADDR_ERR_INT_ENA_M  (LP_REG_SLV_BUF_ADDR_ERR_INT_ENA_V << LP_REG_SLV_BUF_ADDR_ERR_INT_ENA_S)
#define LP_REG_SLV_BUF_ADDR_ERR_INT_ENA_V  0x00000001U
#define LP_REG_SLV_BUF_ADDR_ERR_INT_ENA_S  15
/** LP_REG_SLV_CMD_ERR_INT_ENA : R/W; bitpos: [16]; default: 0;
 *  The enable bit for SPI_SLV_CMD_ERR_INT interrupt.
 */
#define LP_REG_SLV_CMD_ERR_INT_ENA    (BIT(16))
#define LP_REG_SLV_CMD_ERR_INT_ENA_M  (LP_REG_SLV_CMD_ERR_INT_ENA_V << LP_REG_SLV_CMD_ERR_INT_ENA_S)
#define LP_REG_SLV_CMD_ERR_INT_ENA_V  0x00000001U
#define LP_REG_SLV_CMD_ERR_INT_ENA_S  16
/** LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_ENA : R/W; bitpos: [17]; default: 0;
 *  The enable bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.
 */
#define LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_ENA    (BIT(17))
#define LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_ENA_M  (LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_ENA_V << LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_ENA_S)
#define LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_ENA_V  0x00000001U
#define LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_ENA_S  17
/** LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_ENA : R/W; bitpos: [18]; default: 0;
 *  The enable bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.
 */
#define LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_ENA    (BIT(18))
#define LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_ENA_M  (LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_ENA_V << LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_ENA_S)
#define LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_ENA_V  0x00000001U
#define LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_ENA_S  18
/** LP_REG_APP2_INT_ENA : R/W; bitpos: [19]; default: 0;
 *  The enable bit for SPI_APP2_INT interrupt.
 */
#define LP_REG_APP2_INT_ENA    (BIT(19))
#define LP_REG_APP2_INT_ENA_M  (LP_REG_APP2_INT_ENA_V << LP_REG_APP2_INT_ENA_S)
#define LP_REG_APP2_INT_ENA_V  0x00000001U
#define LP_REG_APP2_INT_ENA_S  19
/** LP_REG_APP1_INT_ENA : R/W; bitpos: [20]; default: 0;
 *  The enable bit for SPI_APP1_INT interrupt.
 */
#define LP_REG_APP1_INT_ENA    (BIT(20))
#define LP_REG_APP1_INT_ENA_M  (LP_REG_APP1_INT_ENA_V << LP_REG_APP1_INT_ENA_S)
#define LP_REG_APP1_INT_ENA_V  0x00000001U
#define LP_REG_APP1_INT_ENA_S  20

/** LP_SPI_DMA_INT_CLR_REG register
 *  SPI DMA interrupt clear register
 */
#define LP_SPI_DMA_INT_CLR_REG (DR_REG_LP_SPI_BASE + 0x38)
/** LP_REG_SLV_RD_BUF_DONE_INT_CLR : WT; bitpos: [10]; default: 0;
 *  The clear bit for SPI_SLV_RD_BUF_DONE_INT interrupt.
 */
#define LP_REG_SLV_RD_BUF_DONE_INT_CLR    (BIT(10))
#define LP_REG_SLV_RD_BUF_DONE_INT_CLR_M  (LP_REG_SLV_RD_BUF_DONE_INT_CLR_V << LP_REG_SLV_RD_BUF_DONE_INT_CLR_S)
#define LP_REG_SLV_RD_BUF_DONE_INT_CLR_V  0x00000001U
#define LP_REG_SLV_RD_BUF_DONE_INT_CLR_S  10
/** LP_REG_SLV_WR_BUF_DONE_INT_CLR : WT; bitpos: [11]; default: 0;
 *  The clear bit for SPI_SLV_WR_BUF_DONE_INT interrupt.
 */
#define LP_REG_SLV_WR_BUF_DONE_INT_CLR    (BIT(11))
#define LP_REG_SLV_WR_BUF_DONE_INT_CLR_M  (LP_REG_SLV_WR_BUF_DONE_INT_CLR_V << LP_REG_SLV_WR_BUF_DONE_INT_CLR_S)
#define LP_REG_SLV_WR_BUF_DONE_INT_CLR_V  0x00000001U
#define LP_REG_SLV_WR_BUF_DONE_INT_CLR_S  11
/** LP_REG_TRANS_DONE_INT_CLR : WT; bitpos: [12]; default: 0;
 *  The clear bit for SPI_TRANS_DONE_INT interrupt.
 */
#define LP_REG_TRANS_DONE_INT_CLR    (BIT(12))
#define LP_REG_TRANS_DONE_INT_CLR_M  (LP_REG_TRANS_DONE_INT_CLR_V << LP_REG_TRANS_DONE_INT_CLR_S)
#define LP_REG_TRANS_DONE_INT_CLR_V  0x00000001U
#define LP_REG_TRANS_DONE_INT_CLR_S  12
/** LP_REG_SPI_WAKEUP_INT_CLR : WT; bitpos: [14]; default: 0;
 *  The clear bit for SPI_WAKEUP_INT interrupt
 */
#define LP_REG_SPI_WAKEUP_INT_CLR    (BIT(14))
#define LP_REG_SPI_WAKEUP_INT_CLR_M  (LP_REG_SPI_WAKEUP_INT_CLR_V << LP_REG_SPI_WAKEUP_INT_CLR_S)
#define LP_REG_SPI_WAKEUP_INT_CLR_V  0x00000001U
#define LP_REG_SPI_WAKEUP_INT_CLR_S  14
/** LP_REG_SLV_BUF_ADDR_ERR_INT_CLR : WT; bitpos: [15]; default: 0;
 *  The clear bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.
 */
#define LP_REG_SLV_BUF_ADDR_ERR_INT_CLR    (BIT(15))
#define LP_REG_SLV_BUF_ADDR_ERR_INT_CLR_M  (LP_REG_SLV_BUF_ADDR_ERR_INT_CLR_V << LP_REG_SLV_BUF_ADDR_ERR_INT_CLR_S)
#define LP_REG_SLV_BUF_ADDR_ERR_INT_CLR_V  0x00000001U
#define LP_REG_SLV_BUF_ADDR_ERR_INT_CLR_S  15
/** LP_REG_SLV_CMD_ERR_INT_CLR : WT; bitpos: [16]; default: 0;
 *  The clear bit for SPI_SLV_CMD_ERR_INT interrupt.
 */
#define LP_REG_SLV_CMD_ERR_INT_CLR    (BIT(16))
#define LP_REG_SLV_CMD_ERR_INT_CLR_M  (LP_REG_SLV_CMD_ERR_INT_CLR_V << LP_REG_SLV_CMD_ERR_INT_CLR_S)
#define LP_REG_SLV_CMD_ERR_INT_CLR_V  0x00000001U
#define LP_REG_SLV_CMD_ERR_INT_CLR_S  16
/** LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_CLR : WT; bitpos: [17]; default: 0;
 *  The clear bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.
 */
#define LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_CLR    (BIT(17))
#define LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_CLR_M  (LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_CLR_V << LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_CLR_S)
#define LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_CLR_V  0x00000001U
#define LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_CLR_S  17
/** LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_CLR : WT; bitpos: [18]; default: 0;
 *  The clear bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.
 */
#define LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_CLR    (BIT(18))
#define LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_CLR_M  (LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_CLR_V << LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_CLR_S)
#define LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_CLR_V  0x00000001U
#define LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_CLR_S  18
/** LP_REG_APP2_INT_CLR : WT; bitpos: [19]; default: 0;
 *  The clear bit for SPI_APP2_INT interrupt.
 */
#define LP_REG_APP2_INT_CLR    (BIT(19))
#define LP_REG_APP2_INT_CLR_M  (LP_REG_APP2_INT_CLR_V << LP_REG_APP2_INT_CLR_S)
#define LP_REG_APP2_INT_CLR_V  0x00000001U
#define LP_REG_APP2_INT_CLR_S  19
/** LP_REG_APP1_INT_CLR : WT; bitpos: [20]; default: 0;
 *  The clear bit for SPI_APP1_INT interrupt.
 */
#define LP_REG_APP1_INT_CLR    (BIT(20))
#define LP_REG_APP1_INT_CLR_M  (LP_REG_APP1_INT_CLR_V << LP_REG_APP1_INT_CLR_S)
#define LP_REG_APP1_INT_CLR_V  0x00000001U
#define LP_REG_APP1_INT_CLR_S  20

/** LP_SPI_DMA_INT_RAW_REG register
 *  SPI DMA interrupt raw register
 */
#define LP_SPI_DMA_INT_RAW_REG (DR_REG_LP_SPI_BASE + 0x3c)
/** LP_REG_SLV_RD_BUF_DONE_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  The raw bit for SPI_SLV_RD_BUF_DONE_INT interrupt. 1: SPI slave mode Rd_BUF
 *  transmission is ended. 0: Others.
 */
#define LP_REG_SLV_RD_BUF_DONE_INT_RAW    (BIT(10))
#define LP_REG_SLV_RD_BUF_DONE_INT_RAW_M  (LP_REG_SLV_RD_BUF_DONE_INT_RAW_V << LP_REG_SLV_RD_BUF_DONE_INT_RAW_S)
#define LP_REG_SLV_RD_BUF_DONE_INT_RAW_V  0x00000001U
#define LP_REG_SLV_RD_BUF_DONE_INT_RAW_S  10
/** LP_REG_SLV_WR_BUF_DONE_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  The raw bit for SPI_SLV_WR_BUF_DONE_INT interrupt. 1: SPI slave mode Wr_BUF
 *  transmission is ended. 0: Others.
 */
#define LP_REG_SLV_WR_BUF_DONE_INT_RAW    (BIT(11))
#define LP_REG_SLV_WR_BUF_DONE_INT_RAW_M  (LP_REG_SLV_WR_BUF_DONE_INT_RAW_V << LP_REG_SLV_WR_BUF_DONE_INT_RAW_S)
#define LP_REG_SLV_WR_BUF_DONE_INT_RAW_V  0x00000001U
#define LP_REG_SLV_WR_BUF_DONE_INT_RAW_S  11
/** LP_REG_TRANS_DONE_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  The raw bit for SPI_TRANS_DONE_INT interrupt. 1: SPI master mode transmission is
 *  ended. 0: others.
 */
#define LP_REG_TRANS_DONE_INT_RAW    (BIT(12))
#define LP_REG_TRANS_DONE_INT_RAW_M  (LP_REG_TRANS_DONE_INT_RAW_V << LP_REG_TRANS_DONE_INT_RAW_S)
#define LP_REG_TRANS_DONE_INT_RAW_V  0x00000001U
#define LP_REG_TRANS_DONE_INT_RAW_S  12
/** LP_REG_SPI_WAKEUP_INT_RAW : R/WTC/SS; bitpos: [14]; default: 0;
 *  The raw bit for SPI_SPI_WAKEUP_INT interrupt. 1: There is a  wake up signal when
 *  low power mode. 0: Others.
 */
#define LP_REG_SPI_WAKEUP_INT_RAW    (BIT(14))
#define LP_REG_SPI_WAKEUP_INT_RAW_M  (LP_REG_SPI_WAKEUP_INT_RAW_V << LP_REG_SPI_WAKEUP_INT_RAW_S)
#define LP_REG_SPI_WAKEUP_INT_RAW_V  0x00000001U
#define LP_REG_SPI_WAKEUP_INT_RAW_S  14
/** LP_REG_SLV_BUF_ADDR_ERR_INT_RAW : R/WTC/SS; bitpos: [15]; default: 0;
 *  The raw bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt. 1: The accessing data address
 *  of the current SPI slave mode CPU controlled FD, Wr_BUF or Rd_BUF transmission is
 *  bigger than 63. 0: Others.
 */
#define LP_REG_SLV_BUF_ADDR_ERR_INT_RAW    (BIT(15))
#define LP_REG_SLV_BUF_ADDR_ERR_INT_RAW_M  (LP_REG_SLV_BUF_ADDR_ERR_INT_RAW_V << LP_REG_SLV_BUF_ADDR_ERR_INT_RAW_S)
#define LP_REG_SLV_BUF_ADDR_ERR_INT_RAW_V  0x00000001U
#define LP_REG_SLV_BUF_ADDR_ERR_INT_RAW_S  15
/** LP_REG_SLV_CMD_ERR_INT_RAW : R/WTC/SS; bitpos: [16]; default: 0;
 *  The raw bit for SPI_SLV_CMD_ERR_INT interrupt. 1: The slave command value in the
 *  current SPI slave HD mode transmission is not supported. 0: Others.
 */
#define LP_REG_SLV_CMD_ERR_INT_RAW    (BIT(16))
#define LP_REG_SLV_CMD_ERR_INT_RAW_M  (LP_REG_SLV_CMD_ERR_INT_RAW_V << LP_REG_SLV_CMD_ERR_INT_RAW_S)
#define LP_REG_SLV_CMD_ERR_INT_RAW_V  0x00000001U
#define LP_REG_SLV_CMD_ERR_INT_RAW_S  16
/** LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_RAW : R/WTC/SS; bitpos: [17]; default: 0;
 *  The raw bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt. 1: There is a RX AFIFO
 *  write-full error when SPI inputs data in master mode. 0: Others.
 */
#define LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_RAW    (BIT(17))
#define LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_RAW_M  (LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_RAW_V << LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_RAW_S)
#define LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_RAW_V  0x00000001U
#define LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_RAW_S  17
/** LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_RAW : R/WTC/SS; bitpos: [18]; default: 0;
 *  The raw bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt. 1: There is a TX BUF
 *  AFIFO read-empty error when SPI outputs data in master mode. 0: Others.
 */
#define LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_RAW    (BIT(18))
#define LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_RAW_M  (LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_RAW_V << LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_RAW_S)
#define LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_RAW_V  0x00000001U
#define LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_RAW_S  18
/** LP_REG_APP2_INT_RAW : R/WTC/SS; bitpos: [19]; default: 0;
 *  The raw bit for SPI_APP2_INT interrupt. The value is only controlled by application.
 */
#define LP_REG_APP2_INT_RAW    (BIT(19))
#define LP_REG_APP2_INT_RAW_M  (LP_REG_APP2_INT_RAW_V << LP_REG_APP2_INT_RAW_S)
#define LP_REG_APP2_INT_RAW_V  0x00000001U
#define LP_REG_APP2_INT_RAW_S  19
/** LP_REG_APP1_INT_RAW : R/WTC/SS; bitpos: [20]; default: 0;
 *  The raw bit for SPI_APP1_INT interrupt. The value is only controlled by application.
 */
#define LP_REG_APP1_INT_RAW    (BIT(20))
#define LP_REG_APP1_INT_RAW_M  (LP_REG_APP1_INT_RAW_V << LP_REG_APP1_INT_RAW_S)
#define LP_REG_APP1_INT_RAW_V  0x00000001U
#define LP_REG_APP1_INT_RAW_S  20

/** LP_SPI_DMA_INT_ST_REG register
 *  SPI DMA interrupt status register
 */
#define LP_SPI_DMA_INT_ST_REG (DR_REG_LP_SPI_BASE + 0x40)
/** LP_REG_SLV_RD_BUF_DONE_INT_ST : RO; bitpos: [10]; default: 0;
 *  The status bit for SPI_SLV_RD_BUF_DONE_INT interrupt.
 */
#define LP_REG_SLV_RD_BUF_DONE_INT_ST    (BIT(10))
#define LP_REG_SLV_RD_BUF_DONE_INT_ST_M  (LP_REG_SLV_RD_BUF_DONE_INT_ST_V << LP_REG_SLV_RD_BUF_DONE_INT_ST_S)
#define LP_REG_SLV_RD_BUF_DONE_INT_ST_V  0x00000001U
#define LP_REG_SLV_RD_BUF_DONE_INT_ST_S  10
/** LP_REG_SLV_WR_BUF_DONE_INT_ST : RO; bitpos: [11]; default: 0;
 *  The status bit for SPI_SLV_WR_BUF_DONE_INT interrupt.
 */
#define LP_REG_SLV_WR_BUF_DONE_INT_ST    (BIT(11))
#define LP_REG_SLV_WR_BUF_DONE_INT_ST_M  (LP_REG_SLV_WR_BUF_DONE_INT_ST_V << LP_REG_SLV_WR_BUF_DONE_INT_ST_S)
#define LP_REG_SLV_WR_BUF_DONE_INT_ST_V  0x00000001U
#define LP_REG_SLV_WR_BUF_DONE_INT_ST_S  11
/** LP_REG_TRANS_DONE_INT_ST : RO; bitpos: [12]; default: 0;
 *  The status bit for SPI_TRANS_DONE_INT interrupt.
 */
#define LP_REG_TRANS_DONE_INT_ST    (BIT(12))
#define LP_REG_TRANS_DONE_INT_ST_M  (LP_REG_TRANS_DONE_INT_ST_V << LP_REG_TRANS_DONE_INT_ST_S)
#define LP_REG_TRANS_DONE_INT_ST_V  0x00000001U
#define LP_REG_TRANS_DONE_INT_ST_S  12
/** LP_REG_SPI_WAKEUP_INT_ST : RO; bitpos: [14]; default: 0;
 *  reserved
 */
#define LP_REG_SPI_WAKEUP_INT_ST    (BIT(14))
#define LP_REG_SPI_WAKEUP_INT_ST_M  (LP_REG_SPI_WAKEUP_INT_ST_V << LP_REG_SPI_WAKEUP_INT_ST_S)
#define LP_REG_SPI_WAKEUP_INT_ST_V  0x00000001U
#define LP_REG_SPI_WAKEUP_INT_ST_S  14
/** LP_REG_SLV_BUF_ADDR_ERR_INT_ST : RO; bitpos: [15]; default: 0;
 *  The status bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.
 */
#define LP_REG_SLV_BUF_ADDR_ERR_INT_ST    (BIT(15))
#define LP_REG_SLV_BUF_ADDR_ERR_INT_ST_M  (LP_REG_SLV_BUF_ADDR_ERR_INT_ST_V << LP_REG_SLV_BUF_ADDR_ERR_INT_ST_S)
#define LP_REG_SLV_BUF_ADDR_ERR_INT_ST_V  0x00000001U
#define LP_REG_SLV_BUF_ADDR_ERR_INT_ST_S  15
/** LP_REG_SLV_CMD_ERR_INT_ST : RO; bitpos: [16]; default: 0;
 *  The status bit for SPI_SLV_CMD_ERR_INT interrupt.
 */
#define LP_REG_SLV_CMD_ERR_INT_ST    (BIT(16))
#define LP_REG_SLV_CMD_ERR_INT_ST_M  (LP_REG_SLV_CMD_ERR_INT_ST_V << LP_REG_SLV_CMD_ERR_INT_ST_S)
#define LP_REG_SLV_CMD_ERR_INT_ST_V  0x00000001U
#define LP_REG_SLV_CMD_ERR_INT_ST_S  16
/** LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_ST : RO; bitpos: [17]; default: 0;
 *  The status bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.
 */
#define LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_ST    (BIT(17))
#define LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_ST_M  (LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_ST_V << LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_ST_S)
#define LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_ST_V  0x00000001U
#define LP_REG_MST_RX_AFIFO_WFULL_ERR_INT_ST_S  17
/** LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_ST : RO; bitpos: [18]; default: 0;
 *  The status bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.
 */
#define LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_ST    (BIT(18))
#define LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_ST_M  (LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_ST_V << LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_ST_S)
#define LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_ST_V  0x00000001U
#define LP_REG_MST_TX_AFIFO_REMPTY_ERR_INT_ST_S  18
/** LP_REG_APP2_INT_ST : RO; bitpos: [19]; default: 0;
 *  The status bit for SPI_APP2_INT interrupt.
 */
#define LP_REG_APP2_INT_ST    (BIT(19))
#define LP_REG_APP2_INT_ST_M  (LP_REG_APP2_INT_ST_V << LP_REG_APP2_INT_ST_S)
#define LP_REG_APP2_INT_ST_V  0x00000001U
#define LP_REG_APP2_INT_ST_S  19
/** LP_REG_APP1_INT_ST : RO; bitpos: [20]; default: 0;
 *  The status bit for SPI_APP1_INT interrupt.
 */
#define LP_REG_APP1_INT_ST    (BIT(20))
#define LP_REG_APP1_INT_ST_M  (LP_REG_APP1_INT_ST_V << LP_REG_APP1_INT_ST_S)
#define LP_REG_APP1_INT_ST_V  0x00000001U
#define LP_REG_APP1_INT_ST_S  20

/** LP_SPI_SLEEP_CONF0_REG register
 *  NA
 */
#define LP_SPI_SLEEP_CONF0_REG (DR_REG_LP_SPI_BASE + 0x44)
/** LP_REG_SLV_WK_CHAR0 : R/W; bitpos: [7:0]; default: 10;
 *  NA
 */
#define LP_REG_SLV_WK_CHAR0    0x000000FFU
#define LP_REG_SLV_WK_CHAR0_M  (LP_REG_SLV_WK_CHAR0_V << LP_REG_SLV_WK_CHAR0_S)
#define LP_REG_SLV_WK_CHAR0_V  0x000000FFU
#define LP_REG_SLV_WK_CHAR0_S  0
/** LP_REG_SLV_WK_CHAR_NUM : R/W; bitpos: [10:8]; default: 0;
 *  NA
 */
#define LP_REG_SLV_WK_CHAR_NUM    0x00000007U
#define LP_REG_SLV_WK_CHAR_NUM_M  (LP_REG_SLV_WK_CHAR_NUM_V << LP_REG_SLV_WK_CHAR_NUM_S)
#define LP_REG_SLV_WK_CHAR_NUM_V  0x00000007U
#define LP_REG_SLV_WK_CHAR_NUM_S  8
/** LP_REG_SLV_WK_CHAR_MASK : R/W; bitpos: [15:11]; default: 0;
 *  NA
 */
#define LP_REG_SLV_WK_CHAR_MASK    0x0000001FU
#define LP_REG_SLV_WK_CHAR_MASK_M  (LP_REG_SLV_WK_CHAR_MASK_V << LP_REG_SLV_WK_CHAR_MASK_S)
#define LP_REG_SLV_WK_CHAR_MASK_V  0x0000001FU
#define LP_REG_SLV_WK_CHAR_MASK_S  11
/** LP_REG_SLV_WK_MODE_SEL : R/W; bitpos: [16]; default: 0;
 *  NA
 */
#define LP_REG_SLV_WK_MODE_SEL    (BIT(16))
#define LP_REG_SLV_WK_MODE_SEL_M  (LP_REG_SLV_WK_MODE_SEL_V << LP_REG_SLV_WK_MODE_SEL_S)
#define LP_REG_SLV_WK_MODE_SEL_V  0x00000001U
#define LP_REG_SLV_WK_MODE_SEL_S  16
/** LP_REG_SLEEP_EN : R/W; bitpos: [17]; default: 0;
 *  NA
 */
#define LP_REG_SLEEP_EN    (BIT(17))
#define LP_REG_SLEEP_EN_M  (LP_REG_SLEEP_EN_V << LP_REG_SLEEP_EN_S)
#define LP_REG_SLEEP_EN_V  0x00000001U
#define LP_REG_SLEEP_EN_S  17
/** LP_REG_SLEEP_DIS_RXFIFO_WR_EN : R/W; bitpos: [18]; default: 0;
 *  NA
 */
#define LP_REG_SLEEP_DIS_RXFIFO_WR_EN    (BIT(18))
#define LP_REG_SLEEP_DIS_RXFIFO_WR_EN_M  (LP_REG_SLEEP_DIS_RXFIFO_WR_EN_V << LP_REG_SLEEP_DIS_RXFIFO_WR_EN_S)
#define LP_REG_SLEEP_DIS_RXFIFO_WR_EN_V  0x00000001U
#define LP_REG_SLEEP_DIS_RXFIFO_WR_EN_S  18
/** LP_REG_SLEEP_WK_DATA_SEL : R/W; bitpos: [19]; default: 0;
 *  NA
 */
#define LP_REG_SLEEP_WK_DATA_SEL    (BIT(19))
#define LP_REG_SLEEP_WK_DATA_SEL_M  (LP_REG_SLEEP_WK_DATA_SEL_V << LP_REG_SLEEP_WK_DATA_SEL_S)
#define LP_REG_SLEEP_WK_DATA_SEL_V  0x00000001U
#define LP_REG_SLEEP_WK_DATA_SEL_S  19

/** LP_SPI_SLEEP_CONF1_REG register
 *  NA
 */
#define LP_SPI_SLEEP_CONF1_REG (DR_REG_LP_SPI_BASE + 0x48)
/** LP_REG_SLV_WK_CHAR1 : R/W; bitpos: [7:0]; default: 11;
 *  NA
 */
#define LP_REG_SLV_WK_CHAR1    0x000000FFU
#define LP_REG_SLV_WK_CHAR1_M  (LP_REG_SLV_WK_CHAR1_V << LP_REG_SLV_WK_CHAR1_S)
#define LP_REG_SLV_WK_CHAR1_V  0x000000FFU
#define LP_REG_SLV_WK_CHAR1_S  0
/** LP_REG_SLV_WK_CHAR2 : R/W; bitpos: [15:8]; default: 12;
 *  NA
 */
#define LP_REG_SLV_WK_CHAR2    0x000000FFU
#define LP_REG_SLV_WK_CHAR2_M  (LP_REG_SLV_WK_CHAR2_V << LP_REG_SLV_WK_CHAR2_S)
#define LP_REG_SLV_WK_CHAR2_V  0x000000FFU
#define LP_REG_SLV_WK_CHAR2_S  8
/** LP_REG_SLV_WK_CHAR3 : R/W; bitpos: [23:16]; default: 13;
 *  NA
 */
#define LP_REG_SLV_WK_CHAR3    0x000000FFU
#define LP_REG_SLV_WK_CHAR3_M  (LP_REG_SLV_WK_CHAR3_V << LP_REG_SLV_WK_CHAR3_S)
#define LP_REG_SLV_WK_CHAR3_V  0x000000FFU
#define LP_REG_SLV_WK_CHAR3_S  16
/** LP_REG_SLV_WK_CHAR4 : R/W; bitpos: [31:24]; default: 14;
 *  NA
 */
#define LP_REG_SLV_WK_CHAR4    0x000000FFU
#define LP_REG_SLV_WK_CHAR4_M  (LP_REG_SLV_WK_CHAR4_V << LP_REG_SLV_WK_CHAR4_S)
#define LP_REG_SLV_WK_CHAR4_V  0x000000FFU
#define LP_REG_SLV_WK_CHAR4_S  24

/** LP_SPI_DMA_INT_SET_REG register
 *  SPI interrupt software set register
 */
#define LP_SPI_DMA_INT_SET_REG (DR_REG_LP_SPI_BASE + 0x4c)
/** LP_SPI_SLV_RD_BUF_DONE_INT_SET : WT; bitpos: [10]; default: 0;
 *  The software set bit for SPI_SLV_RD_BUF_DONE_INT interrupt.
 */
#define LP_SPI_SLV_RD_BUF_DONE_INT_SET    (BIT(10))
#define LP_SPI_SLV_RD_BUF_DONE_INT_SET_M  (LP_SPI_SLV_RD_BUF_DONE_INT_SET_V << LP_SPI_SLV_RD_BUF_DONE_INT_SET_S)
#define LP_SPI_SLV_RD_BUF_DONE_INT_SET_V  0x00000001U
#define LP_SPI_SLV_RD_BUF_DONE_INT_SET_S  10
/** LP_SPI_SLV_WR_BUF_DONE_INT_SET : WT; bitpos: [11]; default: 0;
 *  The software set bit for SPI_SLV_WR_BUF_DONE_INT interrupt.
 */
#define LP_SPI_SLV_WR_BUF_DONE_INT_SET    (BIT(11))
#define LP_SPI_SLV_WR_BUF_DONE_INT_SET_M  (LP_SPI_SLV_WR_BUF_DONE_INT_SET_V << LP_SPI_SLV_WR_BUF_DONE_INT_SET_S)
#define LP_SPI_SLV_WR_BUF_DONE_INT_SET_V  0x00000001U
#define LP_SPI_SLV_WR_BUF_DONE_INT_SET_S  11
/** LP_SPI_TRANS_DONE_INT_SET : WT; bitpos: [12]; default: 0;
 *  The software set bit for SPI_TRANS_DONE_INT interrupt.
 */
#define LP_SPI_TRANS_DONE_INT_SET    (BIT(12))
#define LP_SPI_TRANS_DONE_INT_SET_M  (LP_SPI_TRANS_DONE_INT_SET_V << LP_SPI_TRANS_DONE_INT_SET_S)
#define LP_SPI_TRANS_DONE_INT_SET_V  0x00000001U
#define LP_SPI_TRANS_DONE_INT_SET_S  12
/** LP_SPI_SLV_BUF_ADDR_ERR_INT_SET : WT; bitpos: [15]; default: 0;
 *  The software set bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.
 */
#define LP_SPI_SLV_BUF_ADDR_ERR_INT_SET    (BIT(15))
#define LP_SPI_SLV_BUF_ADDR_ERR_INT_SET_M  (LP_SPI_SLV_BUF_ADDR_ERR_INT_SET_V << LP_SPI_SLV_BUF_ADDR_ERR_INT_SET_S)
#define LP_SPI_SLV_BUF_ADDR_ERR_INT_SET_V  0x00000001U
#define LP_SPI_SLV_BUF_ADDR_ERR_INT_SET_S  15
/** LP_SPI_SLV_CMD_ERR_INT_SET : WT; bitpos: [16]; default: 0;
 *  The software set bit for SPI_SLV_CMD_ERR_INT interrupt.
 */
#define LP_SPI_SLV_CMD_ERR_INT_SET    (BIT(16))
#define LP_SPI_SLV_CMD_ERR_INT_SET_M  (LP_SPI_SLV_CMD_ERR_INT_SET_V << LP_SPI_SLV_CMD_ERR_INT_SET_S)
#define LP_SPI_SLV_CMD_ERR_INT_SET_V  0x00000001U
#define LP_SPI_SLV_CMD_ERR_INT_SET_S  16
/** LP_SPI_MST_RX_AFIFO_WFULL_ERR_INT_SET : WT; bitpos: [17]; default: 0;
 *  The software set bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.
 */
#define LP_SPI_MST_RX_AFIFO_WFULL_ERR_INT_SET    (BIT(17))
#define LP_SPI_MST_RX_AFIFO_WFULL_ERR_INT_SET_M  (LP_SPI_MST_RX_AFIFO_WFULL_ERR_INT_SET_V << LP_SPI_MST_RX_AFIFO_WFULL_ERR_INT_SET_S)
#define LP_SPI_MST_RX_AFIFO_WFULL_ERR_INT_SET_V  0x00000001U
#define LP_SPI_MST_RX_AFIFO_WFULL_ERR_INT_SET_S  17
/** LP_SPI_MST_TX_AFIFO_REMPTY_ERR_INT_SET : WT; bitpos: [18]; default: 0;
 *  The software set bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.
 */
#define LP_SPI_MST_TX_AFIFO_REMPTY_ERR_INT_SET    (BIT(18))
#define LP_SPI_MST_TX_AFIFO_REMPTY_ERR_INT_SET_M  (LP_SPI_MST_TX_AFIFO_REMPTY_ERR_INT_SET_V << LP_SPI_MST_TX_AFIFO_REMPTY_ERR_INT_SET_S)
#define LP_SPI_MST_TX_AFIFO_REMPTY_ERR_INT_SET_V  0x00000001U
#define LP_SPI_MST_TX_AFIFO_REMPTY_ERR_INT_SET_S  18
/** LP_SPI_APP2_INT_SET : WT; bitpos: [19]; default: 0;
 *  The software set bit for SPI_APP2_INT interrupt.
 */
#define LP_SPI_APP2_INT_SET    (BIT(19))
#define LP_SPI_APP2_INT_SET_M  (LP_SPI_APP2_INT_SET_V << LP_SPI_APP2_INT_SET_S)
#define LP_SPI_APP2_INT_SET_V  0x00000001U
#define LP_SPI_APP2_INT_SET_S  19
/** LP_SPI_APP1_INT_SET : WT; bitpos: [20]; default: 0;
 *  The software set bit for SPI_APP1_INT interrupt.
 */
#define LP_SPI_APP1_INT_SET    (BIT(20))
#define LP_SPI_APP1_INT_SET_M  (LP_SPI_APP1_INT_SET_V << LP_SPI_APP1_INT_SET_S)
#define LP_SPI_APP1_INT_SET_V  0x00000001U
#define LP_SPI_APP1_INT_SET_S  20

/** LP_SPI_W0_REG register
 *  SPI CPU-controlled buffer0
 */
#define LP_SPI_W0_REG (DR_REG_LP_SPI_BASE + 0x98)
/** LP_REG_BUF0 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define LP_REG_BUF0    0xFFFFFFFFU
#define LP_REG_BUF0_M  (LP_REG_BUF0_V << LP_REG_BUF0_S)
#define LP_REG_BUF0_V  0xFFFFFFFFU
#define LP_REG_BUF0_S  0

/** LP_SPI_W1_REG register
 *  SPI CPU-controlled buffer1
 */
#define LP_SPI_W1_REG (DR_REG_LP_SPI_BASE + 0x9c)
/** LP_REG_BUF1 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define LP_REG_BUF1    0xFFFFFFFFU
#define LP_REG_BUF1_M  (LP_REG_BUF1_V << LP_REG_BUF1_S)
#define LP_REG_BUF1_V  0xFFFFFFFFU
#define LP_REG_BUF1_S  0

/** LP_SPI_W2_REG register
 *  SPI CPU-controlled buffer2
 */
#define LP_SPI_W2_REG (DR_REG_LP_SPI_BASE + 0xa0)
/** LP_REG_BUF2 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define LP_REG_BUF2    0xFFFFFFFFU
#define LP_REG_BUF2_M  (LP_REG_BUF2_V << LP_REG_BUF2_S)
#define LP_REG_BUF2_V  0xFFFFFFFFU
#define LP_REG_BUF2_S  0

/** LP_SPI_W3_REG register
 *  SPI CPU-controlled buffer3
 */
#define LP_SPI_W3_REG (DR_REG_LP_SPI_BASE + 0xa4)
/** LP_REG_BUF3 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define LP_REG_BUF3    0xFFFFFFFFU
#define LP_REG_BUF3_M  (LP_REG_BUF3_V << LP_REG_BUF3_S)
#define LP_REG_BUF3_V  0xFFFFFFFFU
#define LP_REG_BUF3_S  0

/** LP_SPI_W4_REG register
 *  SPI CPU-controlled buffer4
 */
#define LP_SPI_W4_REG (DR_REG_LP_SPI_BASE + 0xa8)
/** LP_REG_BUF4 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define LP_REG_BUF4    0xFFFFFFFFU
#define LP_REG_BUF4_M  (LP_REG_BUF4_V << LP_REG_BUF4_S)
#define LP_REG_BUF4_V  0xFFFFFFFFU
#define LP_REG_BUF4_S  0

/** LP_SPI_W5_REG register
 *  SPI CPU-controlled buffer5
 */
#define LP_SPI_W5_REG (DR_REG_LP_SPI_BASE + 0xac)
/** LP_REG_BUF5 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define LP_REG_BUF5    0xFFFFFFFFU
#define LP_REG_BUF5_M  (LP_REG_BUF5_V << LP_REG_BUF5_S)
#define LP_REG_BUF5_V  0xFFFFFFFFU
#define LP_REG_BUF5_S  0

/** LP_SPI_W6_REG register
 *  SPI CPU-controlled buffer6
 */
#define LP_SPI_W6_REG (DR_REG_LP_SPI_BASE + 0xb0)
/** LP_REG_BUF6 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define LP_REG_BUF6    0xFFFFFFFFU
#define LP_REG_BUF6_M  (LP_REG_BUF6_V << LP_REG_BUF6_S)
#define LP_REG_BUF6_V  0xFFFFFFFFU
#define LP_REG_BUF6_S  0

/** LP_SPI_W7_REG register
 *  SPI CPU-controlled buffer7
 */
#define LP_SPI_W7_REG (DR_REG_LP_SPI_BASE + 0xb4)
/** LP_REG_BUF7 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define LP_REG_BUF7    0xFFFFFFFFU
#define LP_REG_BUF7_M  (LP_REG_BUF7_V << LP_REG_BUF7_S)
#define LP_REG_BUF7_V  0xFFFFFFFFU
#define LP_REG_BUF7_S  0

/** LP_SPI_W8_REG register
 *  SPI CPU-controlled buffer8
 */
#define LP_SPI_W8_REG (DR_REG_LP_SPI_BASE + 0xb8)
/** LP_REG_BUF8 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define LP_REG_BUF8    0xFFFFFFFFU
#define LP_REG_BUF8_M  (LP_REG_BUF8_V << LP_REG_BUF8_S)
#define LP_REG_BUF8_V  0xFFFFFFFFU
#define LP_REG_BUF8_S  0

/** LP_SPI_W9_REG register
 *  SPI CPU-controlled buffer9
 */
#define LP_SPI_W9_REG (DR_REG_LP_SPI_BASE + 0xbc)
/** LP_REG_BUF9 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define LP_REG_BUF9    0xFFFFFFFFU
#define LP_REG_BUF9_M  (LP_REG_BUF9_V << LP_REG_BUF9_S)
#define LP_REG_BUF9_V  0xFFFFFFFFU
#define LP_REG_BUF9_S  0

/** LP_SPI_W10_REG register
 *  SPI CPU-controlled buffer10
 */
#define LP_SPI_W10_REG (DR_REG_LP_SPI_BASE + 0xc0)
/** LP_REG_BUF10 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define LP_REG_BUF10    0xFFFFFFFFU
#define LP_REG_BUF10_M  (LP_REG_BUF10_V << LP_REG_BUF10_S)
#define LP_REG_BUF10_V  0xFFFFFFFFU
#define LP_REG_BUF10_S  0

/** LP_SPI_W11_REG register
 *  SPI CPU-controlled buffer11
 */
#define LP_SPI_W11_REG (DR_REG_LP_SPI_BASE + 0xc4)
/** LP_REG_BUF11 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define LP_REG_BUF11    0xFFFFFFFFU
#define LP_REG_BUF11_M  (LP_REG_BUF11_V << LP_REG_BUF11_S)
#define LP_REG_BUF11_V  0xFFFFFFFFU
#define LP_REG_BUF11_S  0

/** LP_SPI_W12_REG register
 *  SPI CPU-controlled buffer12
 */
#define LP_SPI_W12_REG (DR_REG_LP_SPI_BASE + 0xc8)
/** LP_REG_BUF12 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define LP_REG_BUF12    0xFFFFFFFFU
#define LP_REG_BUF12_M  (LP_REG_BUF12_V << LP_REG_BUF12_S)
#define LP_REG_BUF12_V  0xFFFFFFFFU
#define LP_REG_BUF12_S  0

/** LP_SPI_W13_REG register
 *  SPI CPU-controlled buffer13
 */
#define LP_SPI_W13_REG (DR_REG_LP_SPI_BASE + 0xcc)
/** LP_REG_BUF13 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define LP_REG_BUF13    0xFFFFFFFFU
#define LP_REG_BUF13_M  (LP_REG_BUF13_V << LP_REG_BUF13_S)
#define LP_REG_BUF13_V  0xFFFFFFFFU
#define LP_REG_BUF13_S  0

/** LP_SPI_W14_REG register
 *  SPI CPU-controlled buffer14
 */
#define LP_SPI_W14_REG (DR_REG_LP_SPI_BASE + 0xd0)
/** LP_REG_BUF14 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define LP_REG_BUF14    0xFFFFFFFFU
#define LP_REG_BUF14_M  (LP_REG_BUF14_V << LP_REG_BUF14_S)
#define LP_REG_BUF14_V  0xFFFFFFFFU
#define LP_REG_BUF14_S  0

/** LP_SPI_W15_REG register
 *  SPI CPU-controlled buffer15
 */
#define LP_SPI_W15_REG (DR_REG_LP_SPI_BASE + 0xd4)
/** LP_REG_BUF15 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define LP_REG_BUF15    0xFFFFFFFFU
#define LP_REG_BUF15_M  (LP_REG_BUF15_V << LP_REG_BUF15_S)
#define LP_REG_BUF15_V  0xFFFFFFFFU
#define LP_REG_BUF15_S  0

/** LP_SPI_SLAVE_REG register
 *  SPI slave control register
 */
#define LP_SPI_SLAVE_REG (DR_REG_LP_SPI_BASE + 0xe0)
/** LP_REG_CLK_MODE : R/W; bitpos: [1:0]; default: 0;
 *  SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delayed
 *  one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3:
 *  SPI clock is always on. Can be configured in CONF state.
 */
#define LP_REG_CLK_MODE    0x00000003U
#define LP_REG_CLK_MODE_M  (LP_REG_CLK_MODE_V << LP_REG_CLK_MODE_S)
#define LP_REG_CLK_MODE_V  0x00000003U
#define LP_REG_CLK_MODE_S  0
/** LP_REG_CLK_MODE_13 : R/W; bitpos: [2]; default: 0;
 *  {CPOL, CPHA},1: support spi clk mode 1 and 3, first edge output data B[0]/B[7].  0:
 *  support spi clk mode 0 and 2, first edge output data B[1]/B[6].
 */
#define LP_REG_CLK_MODE_13    (BIT(2))
#define LP_REG_CLK_MODE_13_M  (LP_REG_CLK_MODE_13_V << LP_REG_CLK_MODE_13_S)
#define LP_REG_CLK_MODE_13_V  0x00000001U
#define LP_REG_CLK_MODE_13_S  2
/** LP_REG_RSCK_DATA_OUT : R/W; bitpos: [3]; default: 0;
 *  It saves half a cycle when tsck is the same as rsck. 1: output data at rsck posedge
 *  0: output data at tsck posedge
 */
#define LP_REG_RSCK_DATA_OUT    (BIT(3))
#define LP_REG_RSCK_DATA_OUT_M  (LP_REG_RSCK_DATA_OUT_V << LP_REG_RSCK_DATA_OUT_S)
#define LP_REG_RSCK_DATA_OUT_V  0x00000001U
#define LP_REG_RSCK_DATA_OUT_S  3
/** LP_REG_SLV_RDBUF_BITLEN_EN : R/W; bitpos: [10]; default: 0;
 *  1: SPI_SLV_DATA_BITLEN stores data bit length of master-read-slave data length in
 *  CPU controlled mode(Rd_BUF). 0: others
 */
#define LP_REG_SLV_RDBUF_BITLEN_EN    (BIT(10))
#define LP_REG_SLV_RDBUF_BITLEN_EN_M  (LP_REG_SLV_RDBUF_BITLEN_EN_V << LP_REG_SLV_RDBUF_BITLEN_EN_S)
#define LP_REG_SLV_RDBUF_BITLEN_EN_V  0x00000001U
#define LP_REG_SLV_RDBUF_BITLEN_EN_S  10
/** LP_REG_SLV_WRBUF_BITLEN_EN : R/W; bitpos: [11]; default: 0;
 *  1: SPI_SLV_DATA_BITLEN stores data bit length of master-write-to-slave data length
 *  in CPU controlled mode(Wr_BUF). 0: others
 */
#define LP_REG_SLV_WRBUF_BITLEN_EN    (BIT(11))
#define LP_REG_SLV_WRBUF_BITLEN_EN_M  (LP_REG_SLV_WRBUF_BITLEN_EN_V << LP_REG_SLV_WRBUF_BITLEN_EN_S)
#define LP_REG_SLV_WRBUF_BITLEN_EN_V  0x00000001U
#define LP_REG_SLV_WRBUF_BITLEN_EN_S  11
/** LP_REG_SLAVE_MODE : R/W; bitpos: [26]; default: 0;
 *  Set SPI work mode. 1: slave mode 0: master mode.
 */
#define LP_REG_SLAVE_MODE    (BIT(26))
#define LP_REG_SLAVE_MODE_M  (LP_REG_SLAVE_MODE_V << LP_REG_SLAVE_MODE_S)
#define LP_REG_SLAVE_MODE_V  0x00000001U
#define LP_REG_SLAVE_MODE_S  26
/** LP_REG_SOFT_RESET : WT; bitpos: [27]; default: 0;
 *  Software reset enable, reset the spi clock line cs line and data lines. Can be
 *  configured in CONF state.
 */
#define LP_REG_SOFT_RESET    (BIT(27))
#define LP_REG_SOFT_RESET_M  (LP_REG_SOFT_RESET_V << LP_REG_SOFT_RESET_S)
#define LP_REG_SOFT_RESET_V  0x00000001U
#define LP_REG_SOFT_RESET_S  27

/** LP_SPI_SLAVE1_REG register
 *  SPI slave control register 1
 */
#define LP_SPI_SLAVE1_REG (DR_REG_LP_SPI_BASE + 0xe4)
/** LP_REG_SLV_DATA_BITLEN : R/W/SS; bitpos: [17:0]; default: 0;
 *  The transferred data bit length in SPI slave FD and HD mode.
 */
#define LP_REG_SLV_DATA_BITLEN    0x0003FFFFU
#define LP_REG_SLV_DATA_BITLEN_M  (LP_REG_SLV_DATA_BITLEN_V << LP_REG_SLV_DATA_BITLEN_S)
#define LP_REG_SLV_DATA_BITLEN_V  0x0003FFFFU
#define LP_REG_SLV_DATA_BITLEN_S  0
/** LP_REG_SLV_LAST_COMMAND : R/W/SS; bitpos: [25:18]; default: 0;
 *  In the slave mode it is the value of command.
 */
#define LP_REG_SLV_LAST_COMMAND    0x000000FFU
#define LP_REG_SLV_LAST_COMMAND_M  (LP_REG_SLV_LAST_COMMAND_V << LP_REG_SLV_LAST_COMMAND_S)
#define LP_REG_SLV_LAST_COMMAND_V  0x000000FFU
#define LP_REG_SLV_LAST_COMMAND_S  18
/** LP_REG_SLV_LAST_ADDR : R/W/SS; bitpos: [31:26]; default: 0;
 *  In the slave mode it is the value of address.
 */
#define LP_REG_SLV_LAST_ADDR    0x0000003FU
#define LP_REG_SLV_LAST_ADDR_M  (LP_REG_SLV_LAST_ADDR_V << LP_REG_SLV_LAST_ADDR_S)
#define LP_REG_SLV_LAST_ADDR_V  0x0000003FU
#define LP_REG_SLV_LAST_ADDR_S  26

/** LP_SPI_CLK_GATE_REG register
 *  SPI module clock and register clock control
 */
#define LP_SPI_CLK_GATE_REG (DR_REG_LP_SPI_BASE + 0xe8)
/** LP_REG_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set this bit to enable clk gate
 */
#define LP_REG_CLK_EN    (BIT(0))
#define LP_REG_CLK_EN_M  (LP_REG_CLK_EN_V << LP_REG_CLK_EN_S)
#define LP_REG_CLK_EN_V  0x00000001U
#define LP_REG_CLK_EN_S  0
/** LP_REG_MST_CLK_ACTIVE : R/W; bitpos: [1]; default: 0;
 *  Set this bit to power on the SPI module clock.
 */
#define LP_REG_MST_CLK_ACTIVE    (BIT(1))
#define LP_REG_MST_CLK_ACTIVE_M  (LP_REG_MST_CLK_ACTIVE_V << LP_REG_MST_CLK_ACTIVE_S)
#define LP_REG_MST_CLK_ACTIVE_V  0x00000001U
#define LP_REG_MST_CLK_ACTIVE_S  1
/** LP_REG_MST_CLK_SEL : R/W; bitpos: [2]; default: 0;
 *  This bit is used to select SPI module clock source in master mode. 1: PLL_CLK_80M.
 *  0: XTAL CLK.
 */
#define LP_REG_MST_CLK_SEL    (BIT(2))
#define LP_REG_MST_CLK_SEL_M  (LP_REG_MST_CLK_SEL_V << LP_REG_MST_CLK_SEL_S)
#define LP_REG_MST_CLK_SEL_V  0x00000001U
#define LP_REG_MST_CLK_SEL_S  2

/** LP_SPI_DATE_REG register
 *  Version control
 */
#define LP_SPI_DATE_REG (DR_REG_LP_SPI_BASE + 0xf0)
/** LP_REG_DATE : R/W; bitpos: [27:0]; default: 33591360;
 *  SPI register version.
 */
#define LP_REG_DATE    0x0FFFFFFFU
#define LP_REG_DATE_M  (LP_REG_DATE_V << LP_REG_DATE_S)
#define LP_REG_DATE_V  0x0FFFFFFFU
#define LP_REG_DATE_S  0

/** LP_RND_ECO_CS_REG register
 *  NA
 */
#define LP_RND_ECO_CS_REG (DR_REG_LP_SPI_BASE + 0xf4)
/** LP_REG_RND_ECO_EN : R/W; bitpos: [0]; default: 0;
 *  NA
 */
#define LP_REG_RND_ECO_EN    (BIT(0))
#define LP_REG_RND_ECO_EN_M  (LP_REG_RND_ECO_EN_V << LP_REG_RND_ECO_EN_S)
#define LP_REG_RND_ECO_EN_V  0x00000001U
#define LP_REG_RND_ECO_EN_S  0
/** LP_RND_ECO_RESULT : RO; bitpos: [1]; default: 0;
 *  NA
 */
#define LP_RND_ECO_RESULT    (BIT(1))
#define LP_RND_ECO_RESULT_M  (LP_RND_ECO_RESULT_V << LP_RND_ECO_RESULT_S)
#define LP_RND_ECO_RESULT_V  0x00000001U
#define LP_RND_ECO_RESULT_S  1

/** LP_RND_ECO_LOW_REG register
 *  NA
 */
#define LP_RND_ECO_LOW_REG (DR_REG_LP_SPI_BASE + 0xf8)
/** LP_REG_RND_ECO_LOW : R/W; bitpos: [31:0]; default: 0;
 *  NA
 */
#define LP_REG_RND_ECO_LOW    0xFFFFFFFFU
#define LP_REG_RND_ECO_LOW_M  (LP_REG_RND_ECO_LOW_V << LP_REG_RND_ECO_LOW_S)
#define LP_REG_RND_ECO_LOW_V  0xFFFFFFFFU
#define LP_REG_RND_ECO_LOW_S  0

/** LP_RND_ECO_HIGH_REG register
 *  NA
 */
#define LP_RND_ECO_HIGH_REG (DR_REG_LP_SPI_BASE + 0xfc)
/** LP_REG_RND_ECO_HIGH : R/W; bitpos: [31:0]; default: 65535;
 *  NA
 */
#define LP_REG_RND_ECO_HIGH    0xFFFFFFFFU
#define LP_REG_RND_ECO_HIGH_M  (LP_REG_RND_ECO_HIGH_V << LP_REG_RND_ECO_HIGH_S)
#define LP_REG_RND_ECO_HIGH_V  0xFFFFFFFFU
#define LP_REG_RND_ECO_HIGH_S  0

#ifdef __cplusplus
}
#endif
