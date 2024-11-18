/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_SPI_REG_H_
#define _SOC_SPI_REG_H_

#include "soc/soc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_CMD_REG(i)          (REG_SPI_BASE(i) + 0x0)
/* SPI_USR : R/W/SC ;bitpos:[24] ;default: 1'b0 ; */
/*description: User define command enable.  An operation will be triggered when the bit is set.
 The bit will be cleared once the operation done.1: enable 0: disable. Can not b
e changed by CONF_buf..*/
#define SPI_USR    (BIT(24))
#define SPI_USR_M  (BIT(24))
#define SPI_USR_V  0x1
#define SPI_USR_S  24
/* SPI_UPDATE : WT ;bitpos:[23] ;default: 1'b0 ; */
/*description: Set this bit to synchronize SPI registers from APB clock domain into SPI module
clock domain, which is only used in SPI master mode..*/
#define SPI_UPDATE    (BIT(23))
#define SPI_UPDATE_M  (BIT(23))
#define SPI_UPDATE_V  0x1
#define SPI_UPDATE_S  23
/* SPI_CONF_BITLEN : R/W ;bitpos:[17:0] ;default: 18'd0 ; */
/*description: Define the APB cycles of  SPI_CONF state. Can be configured in CONF state..*/
#define SPI_CONF_BITLEN    0x0003FFFF
#define SPI_CONF_BITLEN_M  ((SPI_CONF_BITLEN_V)<<(SPI_CONF_BITLEN_S))
#define SPI_CONF_BITLEN_V  0x3FFFF
#define SPI_CONF_BITLEN_S  0

#define SPI_ADDR_REG(i)          (REG_SPI_BASE(i) + 0x4)
/* SPI_USR_ADDR_VALUE : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: Address to slave. Can be configured in CONF state..*/
#define SPI_USR_ADDR_VALUE    0xFFFFFFFF
#define SPI_USR_ADDR_VALUE_M  ((SPI_USR_ADDR_VALUE_V)<<(SPI_USR_ADDR_VALUE_S))
#define SPI_USR_ADDR_VALUE_V  0xFFFFFFFF
#define SPI_USR_ADDR_VALUE_S  0

#define SPI_CTRL_REG(i)          (REG_SPI_BASE(i) + 0x8)
/* SPI_WR_BIT_ORDER : R/W ;bitpos:[26:25] ;default: 2'b0 ; */
/*description: In command address write-data (MOSI) phases 1: LSB firs 0: MSB first. Can be con
figured in CONF state..*/
#define SPI_WR_BIT_ORDER    0x00000003
#define SPI_WR_BIT_ORDER_M  ((SPI_WR_BIT_ORDER_V)<<(SPI_WR_BIT_ORDER_S))
#define SPI_WR_BIT_ORDER_V  0x3
#define SPI_WR_BIT_ORDER_S  25
/* SPI_RD_BIT_ORDER : R/W ;bitpos:[24:23] ;default: 2'b0 ; */
/*description: In read-data (MISO) phase 1: LSB first 0: MSB first. Can be configured in CONF s
tate..*/
#define SPI_RD_BIT_ORDER    0x00000003
#define SPI_RD_BIT_ORDER_M  ((SPI_RD_BIT_ORDER_V)<<(SPI_RD_BIT_ORDER_S))
#define SPI_RD_BIT_ORDER_V  0x3
#define SPI_RD_BIT_ORDER_S  23
/* SPI_WP_POL : R/W ;bitpos:[21] ;default: 1'b1 ; */
/*description: Write protect signal output when SPI is idle.  1: output high, 0: output low.  C
an be configured in CONF state..*/
#define SPI_WP_POL    (BIT(21))
#define SPI_WP_POL_M  (BIT(21))
#define SPI_WP_POL_V  0x1
#define SPI_WP_POL_S  21
/* SPI_HOLD_POL : R/W ;bitpos:[20] ;default: 1'b1 ; */
/*description: SPI_HOLD output value when SPI is idle. 1: output high, 0: output low. Can be co
nfigured in CONF state..*/
#define SPI_HOLD_POL    (BIT(20))
#define SPI_HOLD_POL_M  (BIT(20))
#define SPI_HOLD_POL_V  0x1
#define SPI_HOLD_POL_S  20
/* SPI_D_POL : R/W ;bitpos:[19] ;default: 1'b1 ; */
/*description: The bit is used to set MOSI line polarity, 1: high 0, low. Can be configured in
CONF state..*/
#define SPI_D_POL    (BIT(19))
#define SPI_D_POL_M  (BIT(19))
#define SPI_D_POL_V  0x1
#define SPI_D_POL_S  19
/* SPI_Q_POL : R/W ;bitpos:[18] ;default: 1'b1 ; */
/*description: The bit is used to set MISO line polarity, 1: high 0, low. Can be configured in
CONF state..*/
#define SPI_Q_POL    (BIT(18))
#define SPI_Q_POL_M  (BIT(18))
#define SPI_Q_POL_V  0x1
#define SPI_Q_POL_S  18
/* SPI_FREAD_OCT : HRO ;bitpos:[16] ;default: 1'b0 ; */
/*description: In the read operations read-data phase apply 8 signals. 1: enable 0: disable.  C
an be configured in CONF state..*/
#define SPI_FREAD_OCT    (BIT(16))
#define SPI_FREAD_OCT_M  (BIT(16))
#define SPI_FREAD_OCT_V  0x1
#define SPI_FREAD_OCT_S  16
/* SPI_FREAD_QUAD : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: In the read operations read-data phase apply 4 signals. 1: enable 0: disable.  C
an be configured in CONF state..*/
#define SPI_FREAD_QUAD    (BIT(15))
#define SPI_FREAD_QUAD_M  (BIT(15))
#define SPI_FREAD_QUAD_V  0x1
#define SPI_FREAD_QUAD_S  15
/* SPI_FREAD_DUAL : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: In the read operations, read-data phase apply 2 signals. 1: enable 0: disable. C
an be configured in CONF state..*/
#define SPI_FREAD_DUAL    (BIT(14))
#define SPI_FREAD_DUAL_M  (BIT(14))
#define SPI_FREAD_DUAL_V  0x1
#define SPI_FREAD_DUAL_S  14
/* SPI_FCMD_OCT : HRO ;bitpos:[10] ;default: 1'b0 ; */
/*description: Apply 8 signals during command phase 1:enable 0: disable. Can be configured in C
ONF state..*/
#define SPI_FCMD_OCT    (BIT(10))
#define SPI_FCMD_OCT_M  (BIT(10))
#define SPI_FCMD_OCT_V  0x1
#define SPI_FCMD_OCT_S  10
/* SPI_FCMD_QUAD : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: Apply 4 signals during command phase 1:enable 0: disable. Can be configured in C
ONF state..*/
#define SPI_FCMD_QUAD    (BIT(9))
#define SPI_FCMD_QUAD_M  (BIT(9))
#define SPI_FCMD_QUAD_V  0x1
#define SPI_FCMD_QUAD_S  9
/* SPI_FCMD_DUAL : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Apply 2 signals during command phase 1:enable 0: disable. Can be configured in C
ONF state..*/
#define SPI_FCMD_DUAL    (BIT(8))
#define SPI_FCMD_DUAL_M  (BIT(8))
#define SPI_FCMD_DUAL_V  0x1
#define SPI_FCMD_DUAL_S  8
/* SPI_FADDR_OCT : HRO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Apply 8 signals during addr phase 1:enable 0: disable. Can be configured in CONF
 state..*/
#define SPI_FADDR_OCT    (BIT(7))
#define SPI_FADDR_OCT_M  (BIT(7))
#define SPI_FADDR_OCT_V  0x1
#define SPI_FADDR_OCT_S  7
/* SPI_FADDR_QUAD : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Apply 4 signals during addr phase 1:enable 0: disable. Can be configured in CONF
 state..*/
#define SPI_FADDR_QUAD    (BIT(6))
#define SPI_FADDR_QUAD_M  (BIT(6))
#define SPI_FADDR_QUAD_V  0x1
#define SPI_FADDR_QUAD_S  6
/* SPI_FADDR_DUAL : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: Apply 2 signals during addr phase 1:enable 0: disable. Can be configured in CONF
 state..*/
#define SPI_FADDR_DUAL    (BIT(5))
#define SPI_FADDR_DUAL_M  (BIT(5))
#define SPI_FADDR_DUAL_V  0x1
#define SPI_FADDR_DUAL_S  5
/* SPI_DUMMY_OUT : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: 0: In the dummy phase, the FSPI bus signals are not output. 1: In the dummy phas
e, the FSPI bus signals are output. Can be configured in CONF state..*/
#define SPI_DUMMY_OUT    (BIT(3))
#define SPI_DUMMY_OUT_M  (BIT(3))
#define SPI_DUMMY_OUT_V  0x1
#define SPI_DUMMY_OUT_S  3

#define SPI_CLOCK_REG(i)          (REG_SPI_BASE(i) + 0xC)
/* SPI_CLK_EQU_SYSCLK : R/W ;bitpos:[31] ;default: 1'b1 ; */
/*description: In the master mode 1: spi_clk is equal to system 0: spi_clk is divided from syst
em clock. Can be configured in CONF state..*/
#define SPI_CLK_EQU_SYSCLK    (BIT(31))
#define SPI_CLK_EQU_SYSCLK_M  (BIT(31))
#define SPI_CLK_EQU_SYSCLK_V  0x1
#define SPI_CLK_EQU_SYSCLK_S  31
/* SPI_CLKDIV_PRE : R/W ;bitpos:[21:18] ;default: 4'b0 ; */
/*description: In the master mode it is pre-divider of spi_clk.  Can be configured in CONF stat
e..*/
#define SPI_CLKDIV_PRE    0x0000000F
#define SPI_CLKDIV_PRE_M  ((SPI_CLKDIV_PRE_V)<<(SPI_CLKDIV_PRE_S))
#define SPI_CLKDIV_PRE_V  0xF
#define SPI_CLKDIV_PRE_S  18
/* SPI_CLKCNT_N : R/W ;bitpos:[17:12] ;default: 6'h3 ; */
/*description: In the master mode it is the divider of spi_clk. So spi_clk frequency is system/
(spi_clkdiv_pre+1)/(spi_clkcnt_N+1). Can be configured in CONF state..*/
#define SPI_CLKCNT_N    0x0000003F
#define SPI_CLKCNT_N_M  ((SPI_CLKCNT_N_V)<<(SPI_CLKCNT_N_S))
#define SPI_CLKCNT_N_V  0x3F
#define SPI_CLKCNT_N_S  12
/* SPI_CLKCNT_H : R/W ;bitpos:[11:6] ;default: 6'h1 ; */
/*description: In the master mode it must be floor((spi_clkcnt_N+1)/2-1). In the slave mode it
must be 0. Can be configured in CONF state..*/
#define SPI_CLKCNT_H    0x0000003F
#define SPI_CLKCNT_H_M  ((SPI_CLKCNT_H_V)<<(SPI_CLKCNT_H_S))
#define SPI_CLKCNT_H_V  0x3F
#define SPI_CLKCNT_H_S  6
/* SPI_CLKCNT_L : R/W ;bitpos:[5:0] ;default: 6'h3 ; */
/*description: In the master mode it must be equal to spi_clkcnt_N. In the slave mode it must b
e 0. Can be configured in CONF state..*/
#define SPI_CLKCNT_L    0x0000003F
#define SPI_CLKCNT_L_M  ((SPI_CLKCNT_L_V)<<(SPI_CLKCNT_L_S))
#define SPI_CLKCNT_L_V  0x3F
#define SPI_CLKCNT_L_S  0

#define SPI_USER_REG(i)          (REG_SPI_BASE(i) + 0x10)
/* SPI_USR_COMMAND : R/W ;bitpos:[31] ;default: 1'b1 ; */
/*description: This bit enable the command phase of an operation. Can be configured in CONF
state..*/
#define SPI_USR_COMMAND    (BIT(31))
#define SPI_USR_COMMAND_M  (BIT(31))
#define SPI_USR_COMMAND_V  0x1
#define SPI_USR_COMMAND_S  31
/* SPI_USR_ADDR : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: This bit enable the address phase of an operation. Can be configured in CONF
state..*/
#define SPI_USR_ADDR    (BIT(30))
#define SPI_USR_ADDR_M  (BIT(30))
#define SPI_USR_ADDR_V  0x1
#define SPI_USR_ADDR_S  30
/* SPI_USR_DUMMY : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: This bit enable the dummy phase of an operation. Can be configured in CONF state
..*/
#define SPI_USR_DUMMY    (BIT(29))
#define SPI_USR_DUMMY_M  (BIT(29))
#define SPI_USR_DUMMY_V  0x1
#define SPI_USR_DUMMY_S  29
/* SPI_USR_MISO : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: This bit enable the read-data phase of an operation. Can be configured in CONF s
tate..*/
#define SPI_USR_MISO    (BIT(28))
#define SPI_USR_MISO_M  (BIT(28))
#define SPI_USR_MISO_V  0x1
#define SPI_USR_MISO_S  28
/* SPI_USR_MOSI : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: This bit enable the write-data phase of an operation. Can be configured in CONF
state..*/
#define SPI_USR_MOSI    (BIT(27))
#define SPI_USR_MOSI_M  (BIT(27))
#define SPI_USR_MOSI_V  0x1
#define SPI_USR_MOSI_S  27
/* SPI_USR_DUMMY_IDLE : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: spi clock is disable in dummy phase when the bit is enable. Can be configured in
 CONF state..*/
#define SPI_USR_DUMMY_IDLE    (BIT(26))
#define SPI_USR_DUMMY_IDLE_M  (BIT(26))
#define SPI_USR_DUMMY_IDLE_V  0x1
#define SPI_USR_DUMMY_IDLE_S  26
/* SPI_USR_MOSI_HIGHPART : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: write-data phase only access to high-part of the buffer spi_w8~spi_w15. 1: enabl
e 0: disable.  Can be configured in CONF state..*/
#define SPI_USR_MOSI_HIGHPART    (BIT(25))
#define SPI_USR_MOSI_HIGHPART_M  (BIT(25))
#define SPI_USR_MOSI_HIGHPART_V  0x1
#define SPI_USR_MOSI_HIGHPART_S  25
/* SPI_USR_MISO_HIGHPART : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: read-data phase only access to high-part of the buffer spi_w8~spi_w15. 1: enable
 0: disable. Can be configured in CONF state..*/
#define SPI_USR_MISO_HIGHPART    (BIT(24))
#define SPI_USR_MISO_HIGHPART_M  (BIT(24))
#define SPI_USR_MISO_HIGHPART_V  0x1
#define SPI_USR_MISO_HIGHPART_S  24
/* SPI_SIO : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: Set the bit to enable 3-line half duplex communication mosi and miso signals sha
re the same pin. 1: enable 0: disable. Can be configured in CONF state..*/
#define SPI_SIO    (BIT(17))
#define SPI_SIO_M  (BIT(17))
#define SPI_SIO_V  0x1
#define SPI_SIO_S  17
/* SPI_USR_CONF_NXT : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: 1: Enable the DMA CONF phase of next seg-trans operation, which means seg-trans
will continue. 0: The seg-trans will end after the current SPI seg-trans or this
 is not seg-trans mode. Can be configured in CONF state..*/
#define SPI_USR_CONF_NXT    (BIT(15))
#define SPI_USR_CONF_NXT_M  (BIT(15))
#define SPI_USR_CONF_NXT_V  0x1
#define SPI_USR_CONF_NXT_S  15
/* SPI_FWRITE_OCT : HRO ;bitpos:[14] ;default: 1'b0 ; */
/*description: In the write operations read-data phase apply 8 signals. Can be configured in CO
NF state..*/
#define SPI_FWRITE_OCT    (BIT(14))
#define SPI_FWRITE_OCT_M  (BIT(14))
#define SPI_FWRITE_OCT_V  0x1
#define SPI_FWRITE_OCT_S  14
/* SPI_FWRITE_QUAD : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: In the write operations read-data phase apply 4 signals. Can be configured in CO
NF state..*/
#define SPI_FWRITE_QUAD    (BIT(13))
#define SPI_FWRITE_QUAD_M  (BIT(13))
#define SPI_FWRITE_QUAD_V  0x1
#define SPI_FWRITE_QUAD_S  13
/* SPI_FWRITE_DUAL : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: In the write operations read-data phase apply 2 signals. Can be configured in CO
NF state..*/
#define SPI_FWRITE_DUAL    (BIT(12))
#define SPI_FWRITE_DUAL_M  (BIT(12))
#define SPI_FWRITE_DUAL_V  0x1
#define SPI_FWRITE_DUAL_S  12
/* SPI_CK_OUT_EDGE : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: the bit combined with spi_mosi_delay_mode bits to set mosi signal delay mode. Ca
n be configured in CONF state..*/
#define SPI_CK_OUT_EDGE    (BIT(9))
#define SPI_CK_OUT_EDGE_M  (BIT(9))
#define SPI_CK_OUT_EDGE_V  0x1
#define SPI_CK_OUT_EDGE_S  9
/* SPI_RSCK_I_EDGE : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: In the slave mode, this bit can be used to change the polarity of rsck. 0: rsck
= !spi_ck_i. 1:rsck = spi_ck_i..*/
#define SPI_RSCK_I_EDGE    (BIT(8))
#define SPI_RSCK_I_EDGE_M  (BIT(8))
#define SPI_RSCK_I_EDGE_V  0x1
#define SPI_RSCK_I_EDGE_S  8
/* SPI_CS_SETUP : R/W ;bitpos:[7] ;default: 1'b1 ; */
/*description: spi cs is enable when spi is in  prepare  phase. 1: enable 0: disable. Can be co
nfigured in CONF state..*/
#define SPI_CS_SETUP    (BIT(7))
#define SPI_CS_SETUP_M  (BIT(7))
#define SPI_CS_SETUP_V  0x1
#define SPI_CS_SETUP_S  7
/* SPI_CS_HOLD : R/W ;bitpos:[6] ;default: 1'b1 ; */
/*description: spi cs keep low when spi is in  done  phase. 1: enable 0: disable. Can be config
ured in CONF state..*/
#define SPI_CS_HOLD    (BIT(6))
#define SPI_CS_HOLD_M  (BIT(6))
#define SPI_CS_HOLD_V  0x1
#define SPI_CS_HOLD_S  6
/* SPI_TSCK_I_EDGE : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: In the slave mode, this bit can be used to change the polarity of tsck. 0: tsck
= spi_ck_i. 1:tsck = !spi_ck_i..*/
#define SPI_TSCK_I_EDGE    (BIT(5))
#define SPI_TSCK_I_EDGE_M  (BIT(5))
#define SPI_TSCK_I_EDGE_V  0x1
#define SPI_TSCK_I_EDGE_S  5
/* SPI_OPI_MODE : HRO ;bitpos:[4] ;default: 1'b0 ; */
/*description: Just for master mode. 1: spi controller is in OPI mode (all in 8-b-m). 0: others
. Can be configured in CONF state..*/
#define SPI_OPI_MODE    (BIT(4))
#define SPI_OPI_MODE_M  (BIT(4))
#define SPI_OPI_MODE_V  0x1
#define SPI_OPI_MODE_S  4
/* SPI_QPI_MODE : R/W/SS/SC ;bitpos:[3] ;default: 1'b0 ; */
/*description: Both for master mode and slave mode. 1: spi controller is in QPI mode. 0: others
. Can be configured in CONF state..*/
#define SPI_QPI_MODE    (BIT(3))
#define SPI_QPI_MODE_M  (BIT(3))
#define SPI_QPI_MODE_V  0x1
#define SPI_QPI_MODE_S  3
/* SPI_DOUTDIN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set the bit to enable full duplex communication. 1: enable 0: disable. Can be co
nfigured in CONF state..*/
#define SPI_DOUTDIN    (BIT(0))
#define SPI_DOUTDIN_M  (BIT(0))
#define SPI_DOUTDIN_V  0x1
#define SPI_DOUTDIN_S  0

#define SPI_USER1_REG(i)          (REG_SPI_BASE(i) + 0x14)
/* SPI_USR_ADDR_BITLEN : R/W ;bitpos:[31:27] ;default: 5'd23 ; */
/*description: The length in bits of address phase. The register value shall be (bit_num-1). Ca
n be configured in CONF state..*/
#define SPI_USR_ADDR_BITLEN    0x0000001F
#define SPI_USR_ADDR_BITLEN_M  ((SPI_USR_ADDR_BITLEN_V)<<(SPI_USR_ADDR_BITLEN_S))
#define SPI_USR_ADDR_BITLEN_V  0x1F
#define SPI_USR_ADDR_BITLEN_S  27
/* SPI_CS_HOLD_TIME : R/W ;bitpos:[26:22] ;default: 5'h1 ; */
/*description: delay cycles of cs pin by spi clock this bits are combined with spi_cs_hold bit.
 Can be configured in CONF state..*/
#define SPI_CS_HOLD_TIME    0x0000001F
#define SPI_CS_HOLD_TIME_M  ((SPI_CS_HOLD_TIME_V)<<(SPI_CS_HOLD_TIME_S))
#define SPI_CS_HOLD_TIME_V  0x1F
#define SPI_CS_HOLD_TIME_S  22
/* SPI_CS_SETUP_TIME : R/W ;bitpos:[21:17] ;default: 5'b0 ; */
/*description: (cycles+1) of prepare phase by spi clock this bits are combined with spi_cs_setu
p bit. Can be configured in CONF state..*/
#define SPI_CS_SETUP_TIME    0x0000001F
#define SPI_CS_SETUP_TIME_M  ((SPI_CS_SETUP_TIME_V)<<(SPI_CS_SETUP_TIME_S))
#define SPI_CS_SETUP_TIME_V  0x1F
#define SPI_CS_SETUP_TIME_S  17
/* SPI_MST_WFULL_ERR_END_EN : R/W ;bitpos:[16] ;default: 1'b1 ; */
/*description: 1: SPI transfer is ended when SPI RX AFIFO wfull error is valid in GP-SPI master
 FD/HD-mode. 0: SPI transfer is not ended when SPI RX AFIFO wfull error is valid
 in GP-SPI master FD/HD-mode..*/
#define SPI_MST_WFULL_ERR_END_EN    (BIT(16))
#define SPI_MST_WFULL_ERR_END_EN_M  (BIT(16))
#define SPI_MST_WFULL_ERR_END_EN_V  0x1
#define SPI_MST_WFULL_ERR_END_EN_S  16
/* SPI_USR_DUMMY_CYCLELEN : R/W ;bitpos:[7:0] ;default: 8'd7 ; */
/*description: The length in spi_clk cycles of dummy phase. The register value shall be (cycle_
num-1). Can be configured in CONF state..*/
#define SPI_USR_DUMMY_CYCLELEN    0x000000FF
#define SPI_USR_DUMMY_CYCLELEN_M  ((SPI_USR_DUMMY_CYCLELEN_V)<<(SPI_USR_DUMMY_CYCLELEN_S))
#define SPI_USR_DUMMY_CYCLELEN_V  0xFF
#define SPI_USR_DUMMY_CYCLELEN_S  0

#define SPI_USER2_REG(i)          (REG_SPI_BASE(i) + 0x18)
/* SPI_USR_COMMAND_BITLEN : R/W ;bitpos:[31:28] ;default: 4'd7 ; */
/*description: The length in bits of command phase. The register value shall be (bit_num-1). Ca
n be configured in CONF state..*/
#define SPI_USR_COMMAND_BITLEN    0x0000000F
#define SPI_USR_COMMAND_BITLEN_M  ((SPI_USR_COMMAND_BITLEN_V)<<(SPI_USR_COMMAND_BITLEN_S))
#define SPI_USR_COMMAND_BITLEN_V  0xF
#define SPI_USR_COMMAND_BITLEN_S  28
/* SPI_MST_REMPTY_ERR_END_EN : R/W ;bitpos:[27] ;default: 1'b1 ; */
/*description: 1: SPI transfer is ended when SPI TX AFIFO read empty error is valid in GP-SPI m
aster FD/HD-mode. 0: SPI transfer is not ended when SPI TX AFIFO read empty error
r is valid in GP-SPI master FD/HD-mode..*/
#define SPI_MST_REMPTY_ERR_END_EN    (BIT(27))
#define SPI_MST_REMPTY_ERR_END_EN_M  (BIT(27))
#define SPI_MST_REMPTY_ERR_END_EN_V  0x1
#define SPI_MST_REMPTY_ERR_END_EN_S  27
/* SPI_USR_COMMAND_VALUE : R/W ;bitpos:[15:0] ;default: 16'b0 ; */
/*description: The value of  command. Can be configured in CONF state..*/
#define SPI_USR_COMMAND_VALUE    0x0000FFFF
#define SPI_USR_COMMAND_VALUE_M  ((SPI_USR_COMMAND_VALUE_V)<<(SPI_USR_COMMAND_VALUE_S))
#define SPI_USR_COMMAND_VALUE_V  0xFFFF
#define SPI_USR_COMMAND_VALUE_S  0

#define SPI_MS_DLEN_REG(i)          (REG_SPI_BASE(i) + 0x1C)
/* SPI_MS_DATA_BITLEN : R/W ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: The value of these bits is the configured SPI transmission data bit length in ma
ster mode DMA controlled transfer or CPU controlled transfer. The value is also
the configured bit length in slave mode DMA RX controlled transfer. The register
 value shall be (bit_num-1). Can be configured in CONF state..*/
#define SPI_MS_DATA_BITLEN    0x0003FFFF
#define SPI_MS_DATA_BITLEN_M  ((SPI_MS_DATA_BITLEN_V)<<(SPI_MS_DATA_BITLEN_S))
#define SPI_MS_DATA_BITLEN_V  0x3FFFF
#define SPI_MS_DATA_BITLEN_S  0

#define SPI_MISC_REG(i)          (REG_SPI_BASE(i) + 0x20)
/* SPI_QUAD_DIN_PIN_SWAP : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: 1: SPI quad input swap enable, swap FSPID with FSPIQ, swap FSPIWP with FSPIHD. 0
:  spi quad input swap disable. Can be configured in CONF state..*/
#define SPI_QUAD_DIN_PIN_SWAP    (BIT(31))
#define SPI_QUAD_DIN_PIN_SWAP_M  (BIT(31))
#define SPI_QUAD_DIN_PIN_SWAP_V  0x1
#define SPI_QUAD_DIN_PIN_SWAP_S  31
/* SPI_CS_KEEP_ACTIVE : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: spi cs line keep low when the bit is set. Can be configured in CONF state..*/
#define SPI_CS_KEEP_ACTIVE    (BIT(30))
#define SPI_CS_KEEP_ACTIVE_M  (BIT(30))
#define SPI_CS_KEEP_ACTIVE_V  0x1
#define SPI_CS_KEEP_ACTIVE_S  30
/* SPI_CK_IDLE_EDGE : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: 1: spi clk line is high when idle     0: spi clk line is low when idle. Can be c
onfigured in CONF state..*/
#define SPI_CK_IDLE_EDGE    (BIT(29))
#define SPI_CK_IDLE_EDGE_M  (BIT(29))
#define SPI_CK_IDLE_EDGE_V  0x1
#define SPI_CK_IDLE_EDGE_S  29
/* SPI_DQS_IDLE_EDGE : HRO ;bitpos:[24] ;default: 1'b0 ; */
/*description: The default value of spi_dqs. Can be configured in CONF state..*/
#define SPI_DQS_IDLE_EDGE    (BIT(24))
#define SPI_DQS_IDLE_EDGE_M  (BIT(24))
#define SPI_DQS_IDLE_EDGE_V  0x1
#define SPI_DQS_IDLE_EDGE_S  24
/* SPI_SLAVE_CS_POL : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: spi slave input cs polarity select. 1: inv  0: not change. Can be configured in
CONF state..*/
#define SPI_SLAVE_CS_POL    (BIT(23))
#define SPI_SLAVE_CS_POL_M  (BIT(23))
#define SPI_SLAVE_CS_POL_V  0x1
#define SPI_SLAVE_CS_POL_S  23
/* SPI_CMD_DTR_EN : HRO ;bitpos:[19] ;default: 1'b0 ; */
/*description: 1: SPI clk and data of SPI_SEND_CMD state are in DTR mode, including master 1/2/
4/8-bm. 0:  SPI clk and data of SPI_SEND_CMD state are in STR mode. Can be confi
gured in CONF state..*/
#define SPI_CMD_DTR_EN    (BIT(19))
#define SPI_CMD_DTR_EN_M  (BIT(19))
#define SPI_CMD_DTR_EN_V  0x1
#define SPI_CMD_DTR_EN_S  19
/* SPI_ADDR_DTR_EN : HRO ;bitpos:[18] ;default: 1'b0 ; */
/*description: 1: SPI clk and data of SPI_SEND_ADDR state are in DTR mode, including master 1/2
/4/8-bm.  0:  SPI clk and data of SPI_SEND_ADDR state are in STR mode. Can be co
nfigured in CONF state..*/
#define SPI_ADDR_DTR_EN    (BIT(18))
#define SPI_ADDR_DTR_EN_M  (BIT(18))
#define SPI_ADDR_DTR_EN_V  0x1
#define SPI_ADDR_DTR_EN_S  18
/* SPI_DATA_DTR_EN : HRO ;bitpos:[17] ;default: 1'b0 ; */
/*description: 1: SPI clk and data of SPI_DOUT and SPI_DIN state are in DTR mode, including mas
ter 1/2/4/8-bm.  0:  SPI clk and data of SPI_DOUT and SPI_DIN state are in STR m
ode. Can be configured in CONF state..*/
#define SPI_DATA_DTR_EN    (BIT(17))
#define SPI_DATA_DTR_EN_M  (BIT(17))
#define SPI_DATA_DTR_EN_V  0x1
#define SPI_DATA_DTR_EN_S  17
/* SPI_CLK_DATA_DTR_EN : HRO ;bitpos:[16] ;default: 1'b0 ; */
/*description: 1: SPI master DTR mode is applied to SPI clk, data and spi_dqs.  0: SPI master D
TR mode is  only applied to spi_dqs. This bit should be used with bit 17/18/19..*/
#define SPI_CLK_DATA_DTR_EN    (BIT(16))
#define SPI_CLK_DATA_DTR_EN_M  (BIT(16))
#define SPI_CLK_DATA_DTR_EN_V  0x1
#define SPI_CLK_DATA_DTR_EN_S  16
/* SPI_MASTER_CS_POL : R/W ;bitpos:[12:7] ;default: 6'b0 ; */
/*description: In the master mode the bits are the polarity of spi cs line, the value is equiva
lent to spi_cs ^ spi_master_cs_pol. Can be configured in CONF state..*/
#define SPI_MASTER_CS_POL    0x0000003F
#define SPI_MASTER_CS_POL_M  ((SPI_MASTER_CS_POL_V)<<(SPI_MASTER_CS_POL_S))
#define SPI_MASTER_CS_POL_V  0x3F
#define SPI_MASTER_CS_POL_S  7
/* SPI_CK_DIS : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: 1: spi clk out disable,  0: spi clk out enable. Can be configured in CONF state..*/
#define SPI_CK_DIS    (BIT(6))
#define SPI_CK_DIS_M  (BIT(6))
#define SPI_CK_DIS_V  0x1
#define SPI_CK_DIS_S  6
/* SPI_CS5_DIS : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Ca
n be configured in CONF state..*/
#define SPI_CS5_DIS    (BIT(5))
#define SPI_CS5_DIS_M  (BIT(5))
#define SPI_CS5_DIS_V  0x1
#define SPI_CS5_DIS_S  5
/* SPI_CS4_DIS : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Ca
n be configured in CONF state..*/
#define SPI_CS4_DIS    (BIT(4))
#define SPI_CS4_DIS_M  (BIT(4))
#define SPI_CS4_DIS_V  0x1
#define SPI_CS4_DIS_S  4
/* SPI_CS3_DIS : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Ca
n be configured in CONF state..*/
#define SPI_CS3_DIS    (BIT(3))
#define SPI_CS3_DIS_M  (BIT(3))
#define SPI_CS3_DIS_V  0x1
#define SPI_CS3_DIS_S  3
/* SPI_CS2_DIS : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Ca
n be configured in CONF state..*/
#define SPI_CS2_DIS    (BIT(2))
#define SPI_CS2_DIS_M  (BIT(2))
#define SPI_CS2_DIS_V  0x1
#define SPI_CS2_DIS_S  2
/* SPI_CS1_DIS : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Ca
n be configured in CONF state..*/
#define SPI_CS1_DIS    (BIT(1))
#define SPI_CS1_DIS_M  (BIT(1))
#define SPI_CS1_DIS_V  0x1
#define SPI_CS1_DIS_S  1
/* SPI_CS0_DIS : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Ca
n be configured in CONF state..*/
#define SPI_CS0_DIS    (BIT(0))
#define SPI_CS0_DIS_M  (BIT(0))
#define SPI_CS0_DIS_V  0x1
#define SPI_CS0_DIS_S  0

#define SPI_DIN_MODE_REG(i)          (REG_SPI_BASE(i) + 0x24)
/* SPI_TIMING_HCLK_ACTIVE : HRO ;bitpos:[16] ;default: 1'b0 ; */
/*description: 1:enable hclk in SPI input timing module.  0: disable it. Can be configured in C
ONF state..*/
#define SPI_TIMING_HCLK_ACTIVE    (BIT(16))
#define SPI_TIMING_HCLK_ACTIVE_M  (BIT(16))
#define SPI_TIMING_HCLK_ACTIVE_V  0x1
#define SPI_TIMING_HCLK_ACTIVE_S  16
/* SPI_DIN7_MODE : HRO ;bitpos:[15:14] ;default: 2'b0 ; */
/*description: the input signals are delayed by SPI module clock cycles, 0: input without delay
ed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3:
 input with the spi_clk. Can be configured in CONF state..*/
#define SPI_DIN7_MODE    0x00000003
#define SPI_DIN7_MODE_M  ((SPI_DIN7_MODE_V)<<(SPI_DIN7_MODE_S))
#define SPI_DIN7_MODE_V  0x3
#define SPI_DIN7_MODE_S  14
/* SPI_DIN6_MODE : HRO ;bitpos:[13:12] ;default: 2'b0 ; */
/*description: the input signals are delayed by SPI module clock cycles, 0: input without delay
ed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3:
 input with the spi_clk. Can be configured in CONF state..*/
#define SPI_DIN6_MODE    0x00000003
#define SPI_DIN6_MODE_M  ((SPI_DIN6_MODE_V)<<(SPI_DIN6_MODE_S))
#define SPI_DIN6_MODE_V  0x3
#define SPI_DIN6_MODE_S  12
/* SPI_DIN5_MODE : HRO ;bitpos:[11:10] ;default: 2'b0 ; */
/*description: the input signals are delayed by SPI module clock cycles, 0: input without delay
ed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3:
 input with the spi_clk. Can be configured in CONF state..*/
#define SPI_DIN5_MODE    0x00000003
#define SPI_DIN5_MODE_M  ((SPI_DIN5_MODE_V)<<(SPI_DIN5_MODE_S))
#define SPI_DIN5_MODE_V  0x3
#define SPI_DIN5_MODE_S  10
/* SPI_DIN4_MODE : HRO ;bitpos:[9:8] ;default: 2'b0 ; */
/*description: the input signals are delayed by SPI module clock cycles, 0: input without delay
ed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3:
 input with the spi_clk. Can be configured in CONF state..*/
#define SPI_DIN4_MODE    0x00000003
#define SPI_DIN4_MODE_M  ((SPI_DIN4_MODE_V)<<(SPI_DIN4_MODE_S))
#define SPI_DIN4_MODE_V  0x3
#define SPI_DIN4_MODE_S  8
/* SPI_DIN3_MODE : HRO ;bitpos:[7:6] ;default: 2'b0 ; */
/*description: the input signals are delayed by SPI module clock cycles, 0: input without delay
ed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3:
 input with the spi_clk. Can be configured in CONF state..*/
#define SPI_DIN3_MODE    0x00000003
#define SPI_DIN3_MODE_M  ((SPI_DIN3_MODE_V)<<(SPI_DIN3_MODE_S))
#define SPI_DIN3_MODE_V  0x3
#define SPI_DIN3_MODE_S  6
/* SPI_DIN2_MODE : HRO ;bitpos:[5:4] ;default: 2'b0 ; */
/*description: the input signals are delayed by SPI module clock cycles, 0: input without delay
ed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3:
 input with the spi_clk. Can be configured in CONF state..*/
#define SPI_DIN2_MODE    0x00000003
#define SPI_DIN2_MODE_M  ((SPI_DIN2_MODE_V)<<(SPI_DIN2_MODE_S))
#define SPI_DIN2_MODE_V  0x3
#define SPI_DIN2_MODE_S  4
/* SPI_DIN1_MODE : HRO ;bitpos:[3:2] ;default: 2'b0 ; */
/*description: the input signals are delayed by SPI module clock cycles, 0: input without delay
ed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3:
 input with the spi_clk. Can be configured in CONF state..*/
#define SPI_DIN1_MODE    0x00000003
#define SPI_DIN1_MODE_M  ((SPI_DIN1_MODE_V)<<(SPI_DIN1_MODE_S))
#define SPI_DIN1_MODE_V  0x3
#define SPI_DIN1_MODE_S  2
/* SPI_DIN0_MODE : HRO ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: the input signals are delayed by SPI module clock cycles, 0: input without delay
ed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3:
 input with the spi_clk. Can be configured in CONF state..*/
#define SPI_DIN0_MODE    0x00000003
#define SPI_DIN0_MODE_M  ((SPI_DIN0_MODE_V)<<(SPI_DIN0_MODE_S))
#define SPI_DIN0_MODE_V  0x3
#define SPI_DIN0_MODE_S  0

#define SPI_DIN_NUM_REG(i)          (REG_SPI_BASE(i) + 0x28)
/* SPI_DIN7_NUM : HRO ;bitpos:[15:14] ;default: 2'b0 ; */
/*description: the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle,
 1: delayed by 2 cycles,...  Can be configured in CONF state..*/
#define SPI_DIN7_NUM    0x00000003
#define SPI_DIN7_NUM_M  ((SPI_DIN7_NUM_V)<<(SPI_DIN7_NUM_S))
#define SPI_DIN7_NUM_V  0x3
#define SPI_DIN7_NUM_S  14
/* SPI_DIN6_NUM : HRO ;bitpos:[13:12] ;default: 2'b0 ; */
/*description: the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle,
 1: delayed by 2 cycles,...  Can be configured in CONF state..*/
#define SPI_DIN6_NUM    0x00000003
#define SPI_DIN6_NUM_M  ((SPI_DIN6_NUM_V)<<(SPI_DIN6_NUM_S))
#define SPI_DIN6_NUM_V  0x3
#define SPI_DIN6_NUM_S  12
/* SPI_DIN5_NUM : HRO ;bitpos:[11:10] ;default: 2'b0 ; */
/*description: the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle,
 1: delayed by 2 cycles,...  Can be configured in CONF state..*/
#define SPI_DIN5_NUM    0x00000003
#define SPI_DIN5_NUM_M  ((SPI_DIN5_NUM_V)<<(SPI_DIN5_NUM_S))
#define SPI_DIN5_NUM_V  0x3
#define SPI_DIN5_NUM_S  10
/* SPI_DIN4_NUM : HRO ;bitpos:[9:8] ;default: 2'b0 ; */
/*description: the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle,
 1: delayed by 2 cycles,...  Can be configured in CONF state..*/
#define SPI_DIN4_NUM    0x00000003
#define SPI_DIN4_NUM_M  ((SPI_DIN4_NUM_V)<<(SPI_DIN4_NUM_S))
#define SPI_DIN4_NUM_V  0x3
#define SPI_DIN4_NUM_S  8
/* SPI_DIN3_NUM : HRO ;bitpos:[7:6] ;default: 2'b0 ; */
/*description: the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle,
 1: delayed by 2 cycles,...  Can be configured in CONF state..*/
#define SPI_DIN3_NUM    0x00000003
#define SPI_DIN3_NUM_M  ((SPI_DIN3_NUM_V)<<(SPI_DIN3_NUM_S))
#define SPI_DIN3_NUM_V  0x3
#define SPI_DIN3_NUM_S  6
/* SPI_DIN2_NUM : HRO ;bitpos:[5:4] ;default: 2'b0 ; */
/*description: the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle,
 1: delayed by 2 cycles,...  Can be configured in CONF state..*/
#define SPI_DIN2_NUM    0x00000003
#define SPI_DIN2_NUM_M  ((SPI_DIN2_NUM_V)<<(SPI_DIN2_NUM_S))
#define SPI_DIN2_NUM_V  0x3
#define SPI_DIN2_NUM_S  4
/* SPI_DIN1_NUM : HRO ;bitpos:[3:2] ;default: 2'b0 ; */
/*description: the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle,
 1: delayed by 2 cycles,...  Can be configured in CONF state..*/
#define SPI_DIN1_NUM    0x00000003
#define SPI_DIN1_NUM_M  ((SPI_DIN1_NUM_V)<<(SPI_DIN1_NUM_S))
#define SPI_DIN1_NUM_V  0x3
#define SPI_DIN1_NUM_S  2
/* SPI_DIN0_NUM : HRO ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle,
 1: delayed by 2 cycles,...  Can be configured in CONF state..*/
#define SPI_DIN0_NUM    0x00000003
#define SPI_DIN0_NUM_M  ((SPI_DIN0_NUM_V)<<(SPI_DIN0_NUM_S))
#define SPI_DIN0_NUM_V  0x3
#define SPI_DIN0_NUM_S  0

#define SPI_DOUT_MODE_REG(i)          (REG_SPI_BASE(i) + 0x2C)
/* SPI_D_DQS_MODE : HRO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The output signal SPI_DQS is delayed by the SPI module clock, 0: output without
delayed, 1: output delay for a SPI module clock cycle at its negative edge. Can
be configured in CONF state..*/
#define SPI_D_DQS_MODE    (BIT(8))
#define SPI_D_DQS_MODE_M  (BIT(8))
#define SPI_D_DQS_MODE_V  0x1
#define SPI_D_DQS_MODE_S  8
/* SPI_DOUT7_MODE : HRO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The output signal $n is delayed by the SPI module clock, 0: output without delay
ed, 1: output delay for a SPI module clock cycle at its negative edge. Can be co
nfigured in CONF state..*/
#define SPI_DOUT7_MODE    (BIT(7))
#define SPI_DOUT7_MODE_M  (BIT(7))
#define SPI_DOUT7_MODE_V  0x1
#define SPI_DOUT7_MODE_S  7
/* SPI_DOUT6_MODE : HRO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The output signal $n is delayed by the SPI module clock, 0: output without delay
ed, 1: output delay for a SPI module clock cycle at its negative edge. Can be co
nfigured in CONF state..*/
#define SPI_DOUT6_MODE    (BIT(6))
#define SPI_DOUT6_MODE_M  (BIT(6))
#define SPI_DOUT6_MODE_V  0x1
#define SPI_DOUT6_MODE_S  6
/* SPI_DOUT5_MODE : HRO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The output signal $n is delayed by the SPI module clock, 0: output without delay
ed, 1: output delay for a SPI module clock cycle at its negative edge. Can be co
nfigured in CONF state..*/
#define SPI_DOUT5_MODE    (BIT(5))
#define SPI_DOUT5_MODE_M  (BIT(5))
#define SPI_DOUT5_MODE_V  0x1
#define SPI_DOUT5_MODE_S  5
/* SPI_DOUT4_MODE : HRO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The output signal $n is delayed by the SPI module clock, 0: output without delay
ed, 1: output delay for a SPI module clock cycle at its negative edge. Can be co
nfigured in CONF state..*/
#define SPI_DOUT4_MODE    (BIT(4))
#define SPI_DOUT4_MODE_M  (BIT(4))
#define SPI_DOUT4_MODE_V  0x1
#define SPI_DOUT4_MODE_S  4
/* SPI_DOUT3_MODE : HRO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The output signal $n is delayed by the SPI module clock, 0: output without delay
ed, 1: output delay for a SPI module clock cycle at its negative edge. Can be co
nfigured in CONF state..*/
#define SPI_DOUT3_MODE    (BIT(3))
#define SPI_DOUT3_MODE_M  (BIT(3))
#define SPI_DOUT3_MODE_V  0x1
#define SPI_DOUT3_MODE_S  3
/* SPI_DOUT2_MODE : HRO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The output signal $n is delayed by the SPI module clock, 0: output without delay
ed, 1: output delay for a SPI module clock cycle at its negative edge. Can be co
nfigured in CONF state..*/
#define SPI_DOUT2_MODE    (BIT(2))
#define SPI_DOUT2_MODE_M  (BIT(2))
#define SPI_DOUT2_MODE_V  0x1
#define SPI_DOUT2_MODE_S  2
/* SPI_DOUT1_MODE : HRO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The output signal $n is delayed by the SPI module clock, 0: output without delay
ed, 1: output delay for a SPI module clock cycle at its negative edge. Can be co
nfigured in CONF state..*/
#define SPI_DOUT1_MODE    (BIT(1))
#define SPI_DOUT1_MODE_M  (BIT(1))
#define SPI_DOUT1_MODE_V  0x1
#define SPI_DOUT1_MODE_S  1
/* SPI_DOUT0_MODE : HRO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The output signal $n is delayed by the SPI module clock, 0: output without delay
ed, 1: output delay for a SPI module clock cycle at its negative edge. Can be co
nfigured in CONF state..*/
#define SPI_DOUT0_MODE    (BIT(0))
#define SPI_DOUT0_MODE_M  (BIT(0))
#define SPI_DOUT0_MODE_V  0x1
#define SPI_DOUT0_MODE_S  0

#define SPI_DMA_CONF_REG(i)          (REG_SPI_BASE(i) + 0x30)
/* SPI_DMA_AFIFO_RST : WT ;bitpos:[31] ;default: 1'b0 ; */
/*description: Set this bit to reset DMA TX AFIFO, which is used to send data out in SPI slave
DMA controlled mode transfer..*/
#define SPI_DMA_AFIFO_RST    (BIT(31))
#define SPI_DMA_AFIFO_RST_M  (BIT(31))
#define SPI_DMA_AFIFO_RST_V  0x1
#define SPI_DMA_AFIFO_RST_S  31
/* SPI_BUF_AFIFO_RST : WT ;bitpos:[30] ;default: 1'b0 ; */
/*description: Set this bit to reset BUF TX AFIFO, which is used send data out in SPI slave CPU
 controlled mode transfer and master mode transfer..*/
#define SPI_BUF_AFIFO_RST    (BIT(30))
#define SPI_BUF_AFIFO_RST_M  (BIT(30))
#define SPI_BUF_AFIFO_RST_V  0x1
#define SPI_BUF_AFIFO_RST_S  30
/* SPI_RX_AFIFO_RST : WT ;bitpos:[29] ;default: 1'b0 ; */
/*description: Set this bit to reset RX AFIFO, which is used to receive data in SPI master and
slave mode transfer..*/
#define SPI_RX_AFIFO_RST    (BIT(29))
#define SPI_RX_AFIFO_RST_M  (BIT(29))
#define SPI_RX_AFIFO_RST_V  0x1
#define SPI_RX_AFIFO_RST_S  29
/* SPI_DMA_TX_ENA : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: Set this bit to enable SPI DMA controlled send data mode..*/
#define SPI_DMA_TX_ENA    (BIT(28))
#define SPI_DMA_TX_ENA_M  (BIT(28))
#define SPI_DMA_TX_ENA_V  0x1
#define SPI_DMA_TX_ENA_S  28
/* SPI_DMA_RX_ENA : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: Set this bit to enable SPI DMA controlled receive data mode..*/
#define SPI_DMA_RX_ENA    (BIT(27))
#define SPI_DMA_RX_ENA_M  (BIT(27))
#define SPI_DMA_RX_ENA_V  0x1
#define SPI_DMA_RX_ENA_S  27
/* SPI_RX_EOF_EN : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: 1: spi_dma_inlink_eof is set when the number of dma pushed data bytes is equal t
o the value of spi_slv/mst_dma_rd_bytelen[19:0] in spi dma transition.  0: spi_d
ma_inlink_eof is set by spi_trans_done in non-seg-trans or spi_dma_seg_trans_don
e in seg-trans..*/
#define SPI_RX_EOF_EN    (BIT(21))
#define SPI_RX_EOF_EN_M  (BIT(21))
#define SPI_RX_EOF_EN_V  0x1
#define SPI_RX_EOF_EN_S  21
/* SPI_SLV_TX_SEG_TRANS_CLR_EN : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: 1: spi_dma_outfifo_empty_vld is cleared by spi slave cmd 6. 0: spi_dma_outfifo_e
mpty_vld is cleared by spi_trans_done..*/
#define SPI_SLV_TX_SEG_TRANS_CLR_EN    (BIT(20))
#define SPI_SLV_TX_SEG_TRANS_CLR_EN_M  (BIT(20))
#define SPI_SLV_TX_SEG_TRANS_CLR_EN_V  0x1
#define SPI_SLV_TX_SEG_TRANS_CLR_EN_S  20
/* SPI_SLV_RX_SEG_TRANS_CLR_EN : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: 1: spi_dma_infifo_full_vld is cleared by spi slave cmd 5. 0: spi_dma_infifo_full
_vld is cleared by spi_trans_done..*/
#define SPI_SLV_RX_SEG_TRANS_CLR_EN    (BIT(19))
#define SPI_SLV_RX_SEG_TRANS_CLR_EN_M  (BIT(19))
#define SPI_SLV_RX_SEG_TRANS_CLR_EN_V  0x1
#define SPI_SLV_RX_SEG_TRANS_CLR_EN_S  19
/* SPI_DMA_SLV_SEG_TRANS_EN : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: Enable dma segment transfer in spi dma half slave mode. 1: enable. 0: disable..*/
#define SPI_DMA_SLV_SEG_TRANS_EN    (BIT(18))
#define SPI_DMA_SLV_SEG_TRANS_EN_M  (BIT(18))
#define SPI_DMA_SLV_SEG_TRANS_EN_V  0x1
#define SPI_DMA_SLV_SEG_TRANS_EN_S  18
/* SPI_DMA_INFIFO_FULL : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: Records the status of DMA RX FIFO. 1: DMA RX FIFO is not ready for receiving dat
a. 0: DMA RX FIFO is ready for receiving data..*/
#define SPI_DMA_INFIFO_FULL    (BIT(1))
#define SPI_DMA_INFIFO_FULL_M  (BIT(1))
#define SPI_DMA_INFIFO_FULL_V  0x1
#define SPI_DMA_INFIFO_FULL_S  1
/* SPI_DMA_OUTFIFO_EMPTY : RO ;bitpos:[0] ;default: 1'b1 ; */
/*description: Records the status of DMA TX FIFO. 1: DMA TX FIFO is not ready for sending data.
 0: DMA TX FIFO is ready for sending data..*/
#define SPI_DMA_OUTFIFO_EMPTY    (BIT(0))
#define SPI_DMA_OUTFIFO_EMPTY_M  (BIT(0))
#define SPI_DMA_OUTFIFO_EMPTY_V  0x1
#define SPI_DMA_OUTFIFO_EMPTY_S  0

#define SPI_DMA_INT_ENA_REG(i)          (REG_SPI_BASE(i) + 0x34)
/* SPI_APP1_INT_ENA : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: The enable bit for SPI_APP1_INT interrupt..*/
#define SPI_APP1_INT_ENA    (BIT(20))
#define SPI_APP1_INT_ENA_M  (BIT(20))
#define SPI_APP1_INT_ENA_V  0x1
#define SPI_APP1_INT_ENA_S  20
/* SPI_APP2_INT_ENA : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: The enable bit for SPI_APP2_INT interrupt..*/
#define SPI_APP2_INT_ENA    (BIT(19))
#define SPI_APP2_INT_ENA_M  (BIT(19))
#define SPI_APP2_INT_ENA_V  0x1
#define SPI_APP2_INT_ENA_S  19
/* SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ENA : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: The enable bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt..*/
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ENA    (BIT(18))
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ENA_M  (BIT(18))
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ENA_V  0x1
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ENA_S  18
/* SPI_MST_RX_AFIFO_WFULL_ERR_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: The enable bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt..*/
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_ENA    (BIT(17))
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_ENA_M  (BIT(17))
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_ENA_V  0x1
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_ENA_S  17
/* SPI_SLV_CMD_ERR_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: The enable bit for SPI_SLV_CMD_ERR_INT interrupt..*/
#define SPI_SLV_CMD_ERR_INT_ENA    (BIT(16))
#define SPI_SLV_CMD_ERR_INT_ENA_M  (BIT(16))
#define SPI_SLV_CMD_ERR_INT_ENA_V  0x1
#define SPI_SLV_CMD_ERR_INT_ENA_S  16
/* SPI_SLV_BUF_ADDR_ERR_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: The enable bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt..*/
#define SPI_SLV_BUF_ADDR_ERR_INT_ENA    (BIT(15))
#define SPI_SLV_BUF_ADDR_ERR_INT_ENA_M  (BIT(15))
#define SPI_SLV_BUF_ADDR_ERR_INT_ENA_V  0x1
#define SPI_SLV_BUF_ADDR_ERR_INT_ENA_S  15
/* SPI_SEG_MAGIC_ERR_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: The enable bit for SPI_SEG_MAGIC_ERR_INT interrupt..*/
#define SPI_SEG_MAGIC_ERR_INT_ENA    (BIT(14))
#define SPI_SEG_MAGIC_ERR_INT_ENA_M  (BIT(14))
#define SPI_SEG_MAGIC_ERR_INT_ENA_V  0x1
#define SPI_SEG_MAGIC_ERR_INT_ENA_S  14
/* SPI_DMA_SEG_TRANS_DONE_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: The enable bit for SPI_DMA_SEG_TRANS_DONE_INT interrupt..*/
#define SPI_DMA_SEG_TRANS_DONE_INT_ENA    (BIT(13))
#define SPI_DMA_SEG_TRANS_DONE_INT_ENA_M  (BIT(13))
#define SPI_DMA_SEG_TRANS_DONE_INT_ENA_V  0x1
#define SPI_DMA_SEG_TRANS_DONE_INT_ENA_S  13
/* SPI_TRANS_DONE_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: The enable bit for SPI_TRANS_DONE_INT interrupt..*/
#define SPI_TRANS_DONE_INT_ENA    (BIT(12))
#define SPI_TRANS_DONE_INT_ENA_M  (BIT(12))
#define SPI_TRANS_DONE_INT_ENA_V  0x1
#define SPI_TRANS_DONE_INT_ENA_S  12
/* SPI_SLV_WR_BUF_DONE_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: The enable bit for SPI_SLV_WR_BUF_DONE_INT interrupt..*/
#define SPI_SLV_WR_BUF_DONE_INT_ENA    (BIT(11))
#define SPI_SLV_WR_BUF_DONE_INT_ENA_M  (BIT(11))
#define SPI_SLV_WR_BUF_DONE_INT_ENA_V  0x1
#define SPI_SLV_WR_BUF_DONE_INT_ENA_S  11
/* SPI_SLV_RD_BUF_DONE_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: The enable bit for SPI_SLV_RD_BUF_DONE_INT interrupt..*/
#define SPI_SLV_RD_BUF_DONE_INT_ENA    (BIT(10))
#define SPI_SLV_RD_BUF_DONE_INT_ENA_M  (BIT(10))
#define SPI_SLV_RD_BUF_DONE_INT_ENA_V  0x1
#define SPI_SLV_RD_BUF_DONE_INT_ENA_S  10
/* SPI_SLV_WR_DMA_DONE_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: The enable bit for SPI_SLV_WR_DMA_DONE_INT interrupt..*/
#define SPI_SLV_WR_DMA_DONE_INT_ENA    (BIT(9))
#define SPI_SLV_WR_DMA_DONE_INT_ENA_M  (BIT(9))
#define SPI_SLV_WR_DMA_DONE_INT_ENA_V  0x1
#define SPI_SLV_WR_DMA_DONE_INT_ENA_S  9
/* SPI_SLV_RD_DMA_DONE_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: The enable bit for SPI_SLV_RD_DMA_DONE_INT interrupt..*/
#define SPI_SLV_RD_DMA_DONE_INT_ENA    (BIT(8))
#define SPI_SLV_RD_DMA_DONE_INT_ENA_M  (BIT(8))
#define SPI_SLV_RD_DMA_DONE_INT_ENA_V  0x1
#define SPI_SLV_RD_DMA_DONE_INT_ENA_S  8
/* SPI_SLV_CMDA_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The enable bit for SPI slave CMDA interrupt..*/
#define SPI_SLV_CMDA_INT_ENA    (BIT(7))
#define SPI_SLV_CMDA_INT_ENA_M  (BIT(7))
#define SPI_SLV_CMDA_INT_ENA_V  0x1
#define SPI_SLV_CMDA_INT_ENA_S  7
/* SPI_SLV_CMD9_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The enable bit for SPI slave CMD9 interrupt..*/
#define SPI_SLV_CMD9_INT_ENA    (BIT(6))
#define SPI_SLV_CMD9_INT_ENA_M  (BIT(6))
#define SPI_SLV_CMD9_INT_ENA_V  0x1
#define SPI_SLV_CMD9_INT_ENA_S  6
/* SPI_SLV_CMD8_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The enable bit for SPI slave CMD8 interrupt..*/
#define SPI_SLV_CMD8_INT_ENA    (BIT(5))
#define SPI_SLV_CMD8_INT_ENA_M  (BIT(5))
#define SPI_SLV_CMD8_INT_ENA_V  0x1
#define SPI_SLV_CMD8_INT_ENA_S  5
/* SPI_SLV_CMD7_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The enable bit for SPI slave CMD7 interrupt..*/
#define SPI_SLV_CMD7_INT_ENA    (BIT(4))
#define SPI_SLV_CMD7_INT_ENA_M  (BIT(4))
#define SPI_SLV_CMD7_INT_ENA_V  0x1
#define SPI_SLV_CMD7_INT_ENA_S  4
/* SPI_SLV_EN_QPI_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The enable bit for SPI slave En_QPI interrupt..*/
#define SPI_SLV_EN_QPI_INT_ENA    (BIT(3))
#define SPI_SLV_EN_QPI_INT_ENA_M  (BIT(3))
#define SPI_SLV_EN_QPI_INT_ENA_V  0x1
#define SPI_SLV_EN_QPI_INT_ENA_S  3
/* SPI_SLV_EX_QPI_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The enable bit for SPI slave Ex_QPI interrupt..*/
#define SPI_SLV_EX_QPI_INT_ENA    (BIT(2))
#define SPI_SLV_EX_QPI_INT_ENA_M  (BIT(2))
#define SPI_SLV_EX_QPI_INT_ENA_V  0x1
#define SPI_SLV_EX_QPI_INT_ENA_S  2
/* SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The enable bit for SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt..*/
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ENA    (BIT(1))
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ENA_M  (BIT(1))
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ENA_V  0x1
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ENA_S  1
/* SPI_DMA_INFIFO_FULL_ERR_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The enable bit for SPI_DMA_INFIFO_FULL_ERR_INT interrupt..*/
#define SPI_DMA_INFIFO_FULL_ERR_INT_ENA    (BIT(0))
#define SPI_DMA_INFIFO_FULL_ERR_INT_ENA_M  (BIT(0))
#define SPI_DMA_INFIFO_FULL_ERR_INT_ENA_V  0x1
#define SPI_DMA_INFIFO_FULL_ERR_INT_ENA_S  0

#define SPI_DMA_INT_CLR_REG(i)          (REG_SPI_BASE(i) + 0x38)
/* SPI_APP1_INT_CLR : WT ;bitpos:[20] ;default: 1'b0 ; */
/*description: The clear bit for SPI_APP1_INT interrupt..*/
#define SPI_APP1_INT_CLR    (BIT(20))
#define SPI_APP1_INT_CLR_M  (BIT(20))
#define SPI_APP1_INT_CLR_V  0x1
#define SPI_APP1_INT_CLR_S  20
/* SPI_APP2_INT_CLR : WT ;bitpos:[19] ;default: 1'b0 ; */
/*description: The clear bit for SPI_APP2_INT interrupt..*/
#define SPI_APP2_INT_CLR    (BIT(19))
#define SPI_APP2_INT_CLR_M  (BIT(19))
#define SPI_APP2_INT_CLR_V  0x1
#define SPI_APP2_INT_CLR_S  19
/* SPI_MST_TX_AFIFO_REMPTY_ERR_INT_CLR : WT ;bitpos:[18] ;default: 1'b0 ; */
/*description: The clear bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt..*/
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_CLR    (BIT(18))
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_CLR_M  (BIT(18))
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_CLR_V  0x1
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_CLR_S  18
/* SPI_MST_RX_AFIFO_WFULL_ERR_INT_CLR : WT ;bitpos:[17] ;default: 1'b0 ; */
/*description: The clear bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt..*/
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_CLR    (BIT(17))
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_CLR_M  (BIT(17))
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_CLR_V  0x1
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_CLR_S  17
/* SPI_SLV_CMD_ERR_INT_CLR : WT ;bitpos:[16] ;default: 1'b0 ; */
/*description: The clear bit for SPI_SLV_CMD_ERR_INT interrupt..*/
#define SPI_SLV_CMD_ERR_INT_CLR    (BIT(16))
#define SPI_SLV_CMD_ERR_INT_CLR_M  (BIT(16))
#define SPI_SLV_CMD_ERR_INT_CLR_V  0x1
#define SPI_SLV_CMD_ERR_INT_CLR_S  16
/* SPI_SLV_BUF_ADDR_ERR_INT_CLR : WT ;bitpos:[15] ;default: 1'b0 ; */
/*description: The clear bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt..*/
#define SPI_SLV_BUF_ADDR_ERR_INT_CLR    (BIT(15))
#define SPI_SLV_BUF_ADDR_ERR_INT_CLR_M  (BIT(15))
#define SPI_SLV_BUF_ADDR_ERR_INT_CLR_V  0x1
#define SPI_SLV_BUF_ADDR_ERR_INT_CLR_S  15
/* SPI_SEG_MAGIC_ERR_INT_CLR : WT ;bitpos:[14] ;default: 1'b0 ; */
/*description: The clear bit for SPI_SEG_MAGIC_ERR_INT interrupt..*/
#define SPI_SEG_MAGIC_ERR_INT_CLR    (BIT(14))
#define SPI_SEG_MAGIC_ERR_INT_CLR_M  (BIT(14))
#define SPI_SEG_MAGIC_ERR_INT_CLR_V  0x1
#define SPI_SEG_MAGIC_ERR_INT_CLR_S  14
/* SPI_DMA_SEG_TRANS_DONE_INT_CLR : WT ;bitpos:[13] ;default: 1'b0 ; */
/*description: The clear bit for SPI_DMA_SEG_TRANS_DONE_INT interrupt..*/
#define SPI_DMA_SEG_TRANS_DONE_INT_CLR    (BIT(13))
#define SPI_DMA_SEG_TRANS_DONE_INT_CLR_M  (BIT(13))
#define SPI_DMA_SEG_TRANS_DONE_INT_CLR_V  0x1
#define SPI_DMA_SEG_TRANS_DONE_INT_CLR_S  13
/* SPI_TRANS_DONE_INT_CLR : WT ;bitpos:[12] ;default: 1'b0 ; */
/*description: The clear bit for SPI_TRANS_DONE_INT interrupt..*/
#define SPI_TRANS_DONE_INT_CLR    (BIT(12))
#define SPI_TRANS_DONE_INT_CLR_M  (BIT(12))
#define SPI_TRANS_DONE_INT_CLR_V  0x1
#define SPI_TRANS_DONE_INT_CLR_S  12
/* SPI_SLV_WR_BUF_DONE_INT_CLR : WT ;bitpos:[11] ;default: 1'b0 ; */
/*description: The clear bit for SPI_SLV_WR_BUF_DONE_INT interrupt..*/
#define SPI_SLV_WR_BUF_DONE_INT_CLR    (BIT(11))
#define SPI_SLV_WR_BUF_DONE_INT_CLR_M  (BIT(11))
#define SPI_SLV_WR_BUF_DONE_INT_CLR_V  0x1
#define SPI_SLV_WR_BUF_DONE_INT_CLR_S  11
/* SPI_SLV_RD_BUF_DONE_INT_CLR : WT ;bitpos:[10] ;default: 1'b0 ; */
/*description: The clear bit for SPI_SLV_RD_BUF_DONE_INT interrupt..*/
#define SPI_SLV_RD_BUF_DONE_INT_CLR    (BIT(10))
#define SPI_SLV_RD_BUF_DONE_INT_CLR_M  (BIT(10))
#define SPI_SLV_RD_BUF_DONE_INT_CLR_V  0x1
#define SPI_SLV_RD_BUF_DONE_INT_CLR_S  10
/* SPI_SLV_WR_DMA_DONE_INT_CLR : WT ;bitpos:[9] ;default: 1'b0 ; */
/*description: The clear bit for SPI_SLV_WR_DMA_DONE_INT interrupt..*/
#define SPI_SLV_WR_DMA_DONE_INT_CLR    (BIT(9))
#define SPI_SLV_WR_DMA_DONE_INT_CLR_M  (BIT(9))
#define SPI_SLV_WR_DMA_DONE_INT_CLR_V  0x1
#define SPI_SLV_WR_DMA_DONE_INT_CLR_S  9
/* SPI_SLV_RD_DMA_DONE_INT_CLR : WT ;bitpos:[8] ;default: 1'b0 ; */
/*description: The clear bit for SPI_SLV_RD_DMA_DONE_INT interrupt..*/
#define SPI_SLV_RD_DMA_DONE_INT_CLR    (BIT(8))
#define SPI_SLV_RD_DMA_DONE_INT_CLR_M  (BIT(8))
#define SPI_SLV_RD_DMA_DONE_INT_CLR_V  0x1
#define SPI_SLV_RD_DMA_DONE_INT_CLR_S  8
/* SPI_SLV_CMDA_INT_CLR : WT ;bitpos:[7] ;default: 1'b0 ; */
/*description: The clear bit for SPI slave CMDA interrupt..*/
#define SPI_SLV_CMDA_INT_CLR    (BIT(7))
#define SPI_SLV_CMDA_INT_CLR_M  (BIT(7))
#define SPI_SLV_CMDA_INT_CLR_V  0x1
#define SPI_SLV_CMDA_INT_CLR_S  7
/* SPI_SLV_CMD9_INT_CLR : WT ;bitpos:[6] ;default: 1'b0 ; */
/*description: The clear bit for SPI slave CMD9 interrupt..*/
#define SPI_SLV_CMD9_INT_CLR    (BIT(6))
#define SPI_SLV_CMD9_INT_CLR_M  (BIT(6))
#define SPI_SLV_CMD9_INT_CLR_V  0x1
#define SPI_SLV_CMD9_INT_CLR_S  6
/* SPI_SLV_CMD8_INT_CLR : WT ;bitpos:[5] ;default: 1'b0 ; */
/*description: The clear bit for SPI slave CMD8 interrupt..*/
#define SPI_SLV_CMD8_INT_CLR    (BIT(5))
#define SPI_SLV_CMD8_INT_CLR_M  (BIT(5))
#define SPI_SLV_CMD8_INT_CLR_V  0x1
#define SPI_SLV_CMD8_INT_CLR_S  5
/* SPI_SLV_CMD7_INT_CLR : WT ;bitpos:[4] ;default: 1'b0 ; */
/*description: The clear bit for SPI slave CMD7 interrupt..*/
#define SPI_SLV_CMD7_INT_CLR    (BIT(4))
#define SPI_SLV_CMD7_INT_CLR_M  (BIT(4))
#define SPI_SLV_CMD7_INT_CLR_V  0x1
#define SPI_SLV_CMD7_INT_CLR_S  4
/* SPI_SLV_EN_QPI_INT_CLR : WT ;bitpos:[3] ;default: 1'b0 ; */
/*description: The clear bit for SPI slave En_QPI interrupt..*/
#define SPI_SLV_EN_QPI_INT_CLR    (BIT(3))
#define SPI_SLV_EN_QPI_INT_CLR_M  (BIT(3))
#define SPI_SLV_EN_QPI_INT_CLR_V  0x1
#define SPI_SLV_EN_QPI_INT_CLR_S  3
/* SPI_SLV_EX_QPI_INT_CLR : WT ;bitpos:[2] ;default: 1'b0 ; */
/*description: The clear bit for SPI slave Ex_QPI interrupt..*/
#define SPI_SLV_EX_QPI_INT_CLR    (BIT(2))
#define SPI_SLV_EX_QPI_INT_CLR_M  (BIT(2))
#define SPI_SLV_EX_QPI_INT_CLR_V  0x1
#define SPI_SLV_EX_QPI_INT_CLR_S  2
/* SPI_DMA_OUTFIFO_EMPTY_ERR_INT_CLR : WT ;bitpos:[1] ;default: 1'b0 ; */
/*description: The clear bit for SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt..*/
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_CLR    (BIT(1))
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_CLR_M  (BIT(1))
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_CLR_V  0x1
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_CLR_S  1
/* SPI_DMA_INFIFO_FULL_ERR_INT_CLR : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: The clear bit for SPI_DMA_INFIFO_FULL_ERR_INT interrupt..*/
#define SPI_DMA_INFIFO_FULL_ERR_INT_CLR    (BIT(0))
#define SPI_DMA_INFIFO_FULL_ERR_INT_CLR_M  (BIT(0))
#define SPI_DMA_INFIFO_FULL_ERR_INT_CLR_V  0x1
#define SPI_DMA_INFIFO_FULL_ERR_INT_CLR_S  0

#define SPI_DMA_INT_RAW_REG(i)          (REG_SPI_BASE(i) + 0x3C)
/* SPI_APP1_INT_RAW : R/WTC/SS ;bitpos:[20] ;default: 1'b0 ; */
/*description: The raw bit for SPI_APP1_INT interrupt. The value is only controlled by software
..*/
#define SPI_APP1_INT_RAW    (BIT(20))
#define SPI_APP1_INT_RAW_M  (BIT(20))
#define SPI_APP1_INT_RAW_V  0x1
#define SPI_APP1_INT_RAW_S  20
/* SPI_APP2_INT_RAW : R/WTC/SS ;bitpos:[19] ;default: 1'b0 ; */
/*description: The raw bit for SPI_APP2_INT interrupt. The value is only controlled by software
..*/
#define SPI_APP2_INT_RAW    (BIT(19))
#define SPI_APP2_INT_RAW_M  (BIT(19))
#define SPI_APP2_INT_RAW_V  0x1
#define SPI_APP2_INT_RAW_S  19
/* SPI_MST_TX_AFIFO_REMPTY_ERR_INT_RAW : R/WTC/SS ;bitpos:[18] ;default: 1'b0 ; */
/*description: The raw bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt. 1: There is a TX BUF
AFIFO read-empty error when SPI outputs data in master mode. 0: Others..*/
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_RAW    (BIT(18))
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_RAW_M  (BIT(18))
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_RAW_V  0x1
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_RAW_S  18
/* SPI_MST_RX_AFIFO_WFULL_ERR_INT_RAW : R/WTC/SS ;bitpos:[17] ;default: 1'b0 ; */
/*description: The raw bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt. 1: There is a RX AFIFO
 write-full error when SPI inputs data in master mode. 0: Others..*/
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_RAW    (BIT(17))
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_RAW_M  (BIT(17))
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_RAW_V  0x1
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_RAW_S  17
/* SPI_SLV_CMD_ERR_INT_RAW : R/WTC/SS ;bitpos:[16] ;default: 1'b0 ; */
/*description: The raw bit for SPI_SLV_CMD_ERR_INT interrupt. 1: The slave command value in the
 current SPI slave HD mode transmission is not supported. 0: Others..*/
#define SPI_SLV_CMD_ERR_INT_RAW    (BIT(16))
#define SPI_SLV_CMD_ERR_INT_RAW_M  (BIT(16))
#define SPI_SLV_CMD_ERR_INT_RAW_V  0x1
#define SPI_SLV_CMD_ERR_INT_RAW_S  16
/* SPI_SLV_BUF_ADDR_ERR_INT_RAW : R/WTC/SS ;bitpos:[15] ;default: 1'b0 ; */
/*description: The raw bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt. 1: The accessing data address
s of the current SPI slave mode CPU controlled FD, Wr_BUF or Rd_BUF transmission
 is bigger than 63. 0: Others..*/
#define SPI_SLV_BUF_ADDR_ERR_INT_RAW    (BIT(15))
#define SPI_SLV_BUF_ADDR_ERR_INT_RAW_M  (BIT(15))
#define SPI_SLV_BUF_ADDR_ERR_INT_RAW_V  0x1
#define SPI_SLV_BUF_ADDR_ERR_INT_RAW_S  15
/* SPI_SEG_MAGIC_ERR_INT_RAW : R/WTC/SS ;bitpos:[14] ;default: 1'b0 ; */
/*description: The raw bit for SPI_SEG_MAGIC_ERR_INT interrupt. 1: The magic value in CONF buff
er is error in the DMA seg-conf-trans. 0: others..*/
#define SPI_SEG_MAGIC_ERR_INT_RAW    (BIT(14))
#define SPI_SEG_MAGIC_ERR_INT_RAW_M  (BIT(14))
#define SPI_SEG_MAGIC_ERR_INT_RAW_V  0x1
#define SPI_SEG_MAGIC_ERR_INT_RAW_S  14
/* SPI_DMA_SEG_TRANS_DONE_INT_RAW : R/WTC/SS ;bitpos:[13] ;default: 1'b0 ; */
/*description: The raw bit for SPI_DMA_SEG_TRANS_DONE_INT interrupt. 1:  spi master DMA full-du
plex/half-duplex seg-conf-trans ends or slave half-duplex seg-trans ends. And da
ta has been pushed to corresponding memory.  0:  seg-conf-trans or seg-trans is
not ended or not occurred..*/
#define SPI_DMA_SEG_TRANS_DONE_INT_RAW    (BIT(13))
#define SPI_DMA_SEG_TRANS_DONE_INT_RAW_M  (BIT(13))
#define SPI_DMA_SEG_TRANS_DONE_INT_RAW_V  0x1
#define SPI_DMA_SEG_TRANS_DONE_INT_RAW_S  13
/* SPI_TRANS_DONE_INT_RAW : R/WTC/SS ;bitpos:[12] ;default: 1'b0 ; */
/*description: The raw bit for SPI_TRANS_DONE_INT interrupt. 1: SPI master mode transmission is
 ended. 0: others..*/
#define SPI_TRANS_DONE_INT_RAW    (BIT(12))
#define SPI_TRANS_DONE_INT_RAW_M  (BIT(12))
#define SPI_TRANS_DONE_INT_RAW_V  0x1
#define SPI_TRANS_DONE_INT_RAW_S  12
/* SPI_SLV_WR_BUF_DONE_INT_RAW : R/WTC/SS ;bitpos:[11] ;default: 1'b0 ; */
/*description: The raw bit for SPI_SLV_WR_BUF_DONE_INT interrupt. 1: SPI slave mode Wr_BUF tran
smission is ended. 0: Others..*/
#define SPI_SLV_WR_BUF_DONE_INT_RAW    (BIT(11))
#define SPI_SLV_WR_BUF_DONE_INT_RAW_M  (BIT(11))
#define SPI_SLV_WR_BUF_DONE_INT_RAW_V  0x1
#define SPI_SLV_WR_BUF_DONE_INT_RAW_S  11
/* SPI_SLV_RD_BUF_DONE_INT_RAW : R/WTC/SS ;bitpos:[10] ;default: 1'b0 ; */
/*description: The raw bit for SPI_SLV_RD_BUF_DONE_INT interrupt. 1: SPI slave mode Rd_BUF tran
smission is ended. 0: Others..*/
#define SPI_SLV_RD_BUF_DONE_INT_RAW    (BIT(10))
#define SPI_SLV_RD_BUF_DONE_INT_RAW_M  (BIT(10))
#define SPI_SLV_RD_BUF_DONE_INT_RAW_V  0x1
#define SPI_SLV_RD_BUF_DONE_INT_RAW_S  10
/* SPI_SLV_WR_DMA_DONE_INT_RAW : R/WTC/SS ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw bit for SPI_SLV_WR_DMA_DONE_INT interrupt. 1: SPI slave mode Wr_DMA tran
smission is ended. 0: Others..*/
#define SPI_SLV_WR_DMA_DONE_INT_RAW    (BIT(9))
#define SPI_SLV_WR_DMA_DONE_INT_RAW_M  (BIT(9))
#define SPI_SLV_WR_DMA_DONE_INT_RAW_V  0x1
#define SPI_SLV_WR_DMA_DONE_INT_RAW_S  9
/* SPI_SLV_RD_DMA_DONE_INT_RAW : R/WTC/SS ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw bit for SPI_SLV_RD_DMA_DONE_INT interrupt. 1: SPI slave mode Rd_DMA tran
smission is ended. 0: Others..*/
#define SPI_SLV_RD_DMA_DONE_INT_RAW    (BIT(8))
#define SPI_SLV_RD_DMA_DONE_INT_RAW_M  (BIT(8))
#define SPI_SLV_RD_DMA_DONE_INT_RAW_V  0x1
#define SPI_SLV_RD_DMA_DONE_INT_RAW_S  8
/* SPI_SLV_CMDA_INT_RAW : R/WTC/SS ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw bit for SPI slave CMDA interrupt. 1: SPI slave mode CMDA transmission is
 ended. 0: Others..*/
#define SPI_SLV_CMDA_INT_RAW    (BIT(7))
#define SPI_SLV_CMDA_INT_RAW_M  (BIT(7))
#define SPI_SLV_CMDA_INT_RAW_V  0x1
#define SPI_SLV_CMDA_INT_RAW_S  7
/* SPI_SLV_CMD9_INT_RAW : R/WTC/SS ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw bit for SPI slave CMD9 interrupt. 1: SPI slave mode CMD9 transmission is
 ended. 0: Others..*/
#define SPI_SLV_CMD9_INT_RAW    (BIT(6))
#define SPI_SLV_CMD9_INT_RAW_M  (BIT(6))
#define SPI_SLV_CMD9_INT_RAW_V  0x1
#define SPI_SLV_CMD9_INT_RAW_S  6
/* SPI_SLV_CMD8_INT_RAW : R/WTC/SS ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw bit for SPI slave CMD8 interrupt. 1: SPI slave mode CMD8 transmission is
 ended. 0: Others..*/
#define SPI_SLV_CMD8_INT_RAW    (BIT(5))
#define SPI_SLV_CMD8_INT_RAW_M  (BIT(5))
#define SPI_SLV_CMD8_INT_RAW_V  0x1
#define SPI_SLV_CMD8_INT_RAW_S  5
/* SPI_SLV_CMD7_INT_RAW : R/WTC/SS ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw bit for SPI slave CMD7 interrupt. 1: SPI slave mode CMD7 transmission is
 ended. 0: Others..*/
#define SPI_SLV_CMD7_INT_RAW    (BIT(4))
#define SPI_SLV_CMD7_INT_RAW_M  (BIT(4))
#define SPI_SLV_CMD7_INT_RAW_V  0x1
#define SPI_SLV_CMD7_INT_RAW_S  4
/* SPI_SLV_EN_QPI_INT_RAW : R/WTC/SS ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw bit for SPI slave En_QPI interrupt. 1: SPI slave mode En_QPI transmissio
n is ended. 0: Others..*/
#define SPI_SLV_EN_QPI_INT_RAW    (BIT(3))
#define SPI_SLV_EN_QPI_INT_RAW_M  (BIT(3))
#define SPI_SLV_EN_QPI_INT_RAW_V  0x1
#define SPI_SLV_EN_QPI_INT_RAW_S  3
/* SPI_SLV_EX_QPI_INT_RAW : R/WTC/SS ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw bit for SPI slave Ex_QPI interrupt. 1: SPI slave mode Ex_QPI transmissio
n is ended. 0: Others..*/
#define SPI_SLV_EX_QPI_INT_RAW    (BIT(2))
#define SPI_SLV_EX_QPI_INT_RAW_M  (BIT(2))
#define SPI_SLV_EX_QPI_INT_RAW_V  0x1
#define SPI_SLV_EX_QPI_INT_RAW_S  2
/* SPI_DMA_OUTFIFO_EMPTY_ERR_INT_RAW : R/WTC/SS ;bitpos:[1] ;default: 1'b0 ; */
/*description: 1: The current data rate of DMA TX is smaller than that of SPI. SPI will stop in
 master mode and send out all 0 in slave mode.  0: Others..*/
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_RAW    (BIT(1))
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_RAW_M  (BIT(1))
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_RAW_V  0x1
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_RAW_S  1
/* SPI_DMA_INFIFO_FULL_ERR_INT_RAW : R/WTC/SS ;bitpos:[0] ;default: 1'b0 ; */
/*description: 1: The current data rate of DMA Rx is smaller than that of SPI, which will lose
the receive data.  0: Others..*/
#define SPI_DMA_INFIFO_FULL_ERR_INT_RAW    (BIT(0))
#define SPI_DMA_INFIFO_FULL_ERR_INT_RAW_M  (BIT(0))
#define SPI_DMA_INFIFO_FULL_ERR_INT_RAW_V  0x1
#define SPI_DMA_INFIFO_FULL_ERR_INT_RAW_S  0

#define SPI_DMA_INT_ST_REG(i)          (REG_SPI_BASE(i) + 0x40)
/* SPI_APP1_INT_ST : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: The status bit for SPI_APP1_INT interrupt..*/
#define SPI_APP1_INT_ST    (BIT(20))
#define SPI_APP1_INT_ST_M  (BIT(20))
#define SPI_APP1_INT_ST_V  0x1
#define SPI_APP1_INT_ST_S  20
/* SPI_APP2_INT_ST : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: The status bit for SPI_APP2_INT interrupt..*/
#define SPI_APP2_INT_ST    (BIT(19))
#define SPI_APP2_INT_ST_M  (BIT(19))
#define SPI_APP2_INT_ST_V  0x1
#define SPI_APP2_INT_ST_S  19
/* SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ST : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: The status bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt..*/
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ST    (BIT(18))
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ST_M  (BIT(18))
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ST_V  0x1
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_ST_S  18
/* SPI_MST_RX_AFIFO_WFULL_ERR_INT_ST : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: The status bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt..*/
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_ST    (BIT(17))
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_ST_M  (BIT(17))
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_ST_V  0x1
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_ST_S  17
/* SPI_SLV_CMD_ERR_INT_ST : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: The status bit for SPI_SLV_CMD_ERR_INT interrupt..*/
#define SPI_SLV_CMD_ERR_INT_ST    (BIT(16))
#define SPI_SLV_CMD_ERR_INT_ST_M  (BIT(16))
#define SPI_SLV_CMD_ERR_INT_ST_V  0x1
#define SPI_SLV_CMD_ERR_INT_ST_S  16
/* SPI_SLV_BUF_ADDR_ERR_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: The status bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt..*/
#define SPI_SLV_BUF_ADDR_ERR_INT_ST    (BIT(15))
#define SPI_SLV_BUF_ADDR_ERR_INT_ST_M  (BIT(15))
#define SPI_SLV_BUF_ADDR_ERR_INT_ST_V  0x1
#define SPI_SLV_BUF_ADDR_ERR_INT_ST_S  15
/* SPI_SEG_MAGIC_ERR_INT_ST : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: The status bit for SPI_SEG_MAGIC_ERR_INT interrupt..*/
#define SPI_SEG_MAGIC_ERR_INT_ST    (BIT(14))
#define SPI_SEG_MAGIC_ERR_INT_ST_M  (BIT(14))
#define SPI_SEG_MAGIC_ERR_INT_ST_V  0x1
#define SPI_SEG_MAGIC_ERR_INT_ST_S  14
/* SPI_DMA_SEG_TRANS_DONE_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: The status bit for SPI_DMA_SEG_TRANS_DONE_INT interrupt..*/
#define SPI_DMA_SEG_TRANS_DONE_INT_ST    (BIT(13))
#define SPI_DMA_SEG_TRANS_DONE_INT_ST_M  (BIT(13))
#define SPI_DMA_SEG_TRANS_DONE_INT_ST_V  0x1
#define SPI_DMA_SEG_TRANS_DONE_INT_ST_S  13
/* SPI_TRANS_DONE_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: The status bit for SPI_TRANS_DONE_INT interrupt..*/
#define SPI_TRANS_DONE_INT_ST    (BIT(12))
#define SPI_TRANS_DONE_INT_ST_M  (BIT(12))
#define SPI_TRANS_DONE_INT_ST_V  0x1
#define SPI_TRANS_DONE_INT_ST_S  12
/* SPI_SLV_WR_BUF_DONE_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: The status bit for SPI_SLV_WR_BUF_DONE_INT interrupt..*/
#define SPI_SLV_WR_BUF_DONE_INT_ST    (BIT(11))
#define SPI_SLV_WR_BUF_DONE_INT_ST_M  (BIT(11))
#define SPI_SLV_WR_BUF_DONE_INT_ST_V  0x1
#define SPI_SLV_WR_BUF_DONE_INT_ST_S  11
/* SPI_SLV_RD_BUF_DONE_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: The status bit for SPI_SLV_RD_BUF_DONE_INT interrupt..*/
#define SPI_SLV_RD_BUF_DONE_INT_ST    (BIT(10))
#define SPI_SLV_RD_BUF_DONE_INT_ST_M  (BIT(10))
#define SPI_SLV_RD_BUF_DONE_INT_ST_V  0x1
#define SPI_SLV_RD_BUF_DONE_INT_ST_S  10
/* SPI_SLV_WR_DMA_DONE_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The status bit for SPI_SLV_WR_DMA_DONE_INT interrupt..*/
#define SPI_SLV_WR_DMA_DONE_INT_ST    (BIT(9))
#define SPI_SLV_WR_DMA_DONE_INT_ST_M  (BIT(9))
#define SPI_SLV_WR_DMA_DONE_INT_ST_V  0x1
#define SPI_SLV_WR_DMA_DONE_INT_ST_S  9
/* SPI_SLV_RD_DMA_DONE_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The status bit for SPI_SLV_RD_DMA_DONE_INT interrupt..*/
#define SPI_SLV_RD_DMA_DONE_INT_ST    (BIT(8))
#define SPI_SLV_RD_DMA_DONE_INT_ST_M  (BIT(8))
#define SPI_SLV_RD_DMA_DONE_INT_ST_V  0x1
#define SPI_SLV_RD_DMA_DONE_INT_ST_S  8
/* SPI_SLV_CMDA_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The status bit for SPI slave CMDA interrupt..*/
#define SPI_SLV_CMDA_INT_ST    (BIT(7))
#define SPI_SLV_CMDA_INT_ST_M  (BIT(7))
#define SPI_SLV_CMDA_INT_ST_V  0x1
#define SPI_SLV_CMDA_INT_ST_S  7
/* SPI_SLV_CMD9_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The status bit for SPI slave CMD9 interrupt..*/
#define SPI_SLV_CMD9_INT_ST    (BIT(6))
#define SPI_SLV_CMD9_INT_ST_M  (BIT(6))
#define SPI_SLV_CMD9_INT_ST_V  0x1
#define SPI_SLV_CMD9_INT_ST_S  6
/* SPI_SLV_CMD8_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The status bit for SPI slave CMD8 interrupt..*/
#define SPI_SLV_CMD8_INT_ST    (BIT(5))
#define SPI_SLV_CMD8_INT_ST_M  (BIT(5))
#define SPI_SLV_CMD8_INT_ST_V  0x1
#define SPI_SLV_CMD8_INT_ST_S  5
/* SPI_SLV_CMD7_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The status bit for SPI slave CMD7 interrupt..*/
#define SPI_SLV_CMD7_INT_ST    (BIT(4))
#define SPI_SLV_CMD7_INT_ST_M  (BIT(4))
#define SPI_SLV_CMD7_INT_ST_V  0x1
#define SPI_SLV_CMD7_INT_ST_S  4
/* SPI_SLV_EN_QPI_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The status bit for SPI slave En_QPI interrupt..*/
#define SPI_SLV_EN_QPI_INT_ST    (BIT(3))
#define SPI_SLV_EN_QPI_INT_ST_M  (BIT(3))
#define SPI_SLV_EN_QPI_INT_ST_V  0x1
#define SPI_SLV_EN_QPI_INT_ST_S  3
/* SPI_SLV_EX_QPI_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The status bit for SPI slave Ex_QPI interrupt..*/
#define SPI_SLV_EX_QPI_INT_ST    (BIT(2))
#define SPI_SLV_EX_QPI_INT_ST_M  (BIT(2))
#define SPI_SLV_EX_QPI_INT_ST_V  0x1
#define SPI_SLV_EX_QPI_INT_ST_S  2
/* SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The status bit for SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt..*/
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ST    (BIT(1))
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ST_M  (BIT(1))
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ST_V  0x1
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_ST_S  1
/* SPI_DMA_INFIFO_FULL_ERR_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The status bit for SPI_DMA_INFIFO_FULL_ERR_INT interrupt..*/
#define SPI_DMA_INFIFO_FULL_ERR_INT_ST    (BIT(0))
#define SPI_DMA_INFIFO_FULL_ERR_INT_ST_M  (BIT(0))
#define SPI_DMA_INFIFO_FULL_ERR_INT_ST_V  0x1
#define SPI_DMA_INFIFO_FULL_ERR_INT_ST_S  0

#define SPI_DMA_INT_SET_REG(i)          (REG_SPI_BASE(i) + 0x44)
/* SPI_APP1_INT_SET : WT ;bitpos:[20] ;default: 1'b0 ; */
/*description: The software set bit for SPI_APP1_INT interrupt..*/
#define SPI_APP1_INT_SET    (BIT(20))
#define SPI_APP1_INT_SET_M  (BIT(20))
#define SPI_APP1_INT_SET_V  0x1
#define SPI_APP1_INT_SET_S  20
/* SPI_APP2_INT_SET : WT ;bitpos:[19] ;default: 1'b0 ; */
/*description: The software set bit for SPI_APP2_INT interrupt..*/
#define SPI_APP2_INT_SET    (BIT(19))
#define SPI_APP2_INT_SET_M  (BIT(19))
#define SPI_APP2_INT_SET_V  0x1
#define SPI_APP2_INT_SET_S  19
/* SPI_MST_TX_AFIFO_REMPTY_ERR_INT_SET : WT ;bitpos:[18] ;default: 1'b0 ; */
/*description: The software set bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt..*/
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_SET    (BIT(18))
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_SET_M  (BIT(18))
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_SET_V  0x1
#define SPI_MST_TX_AFIFO_REMPTY_ERR_INT_SET_S  18
/* SPI_MST_RX_AFIFO_WFULL_ERR_INT_SET : WT ;bitpos:[17] ;default: 1'b0 ; */
/*description: The software set bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt..*/
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_SET    (BIT(17))
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_SET_M  (BIT(17))
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_SET_V  0x1
#define SPI_MST_RX_AFIFO_WFULL_ERR_INT_SET_S  17
/* SPI_SLV_CMD_ERR_INT_SET : WT ;bitpos:[16] ;default: 1'b0 ; */
/*description: The software set bit for SPI_SLV_CMD_ERR_INT interrupt..*/
#define SPI_SLV_CMD_ERR_INT_SET    (BIT(16))
#define SPI_SLV_CMD_ERR_INT_SET_M  (BIT(16))
#define SPI_SLV_CMD_ERR_INT_SET_V  0x1
#define SPI_SLV_CMD_ERR_INT_SET_S  16
/* SPI_SLV_BUF_ADDR_ERR_INT_SET : WT ;bitpos:[15] ;default: 1'b0 ; */
/*description: The software set bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt..*/
#define SPI_SLV_BUF_ADDR_ERR_INT_SET    (BIT(15))
#define SPI_SLV_BUF_ADDR_ERR_INT_SET_M  (BIT(15))
#define SPI_SLV_BUF_ADDR_ERR_INT_SET_V  0x1
#define SPI_SLV_BUF_ADDR_ERR_INT_SET_S  15
/* SPI_SEG_MAGIC_ERR_INT_SET : WT ;bitpos:[14] ;default: 1'b0 ; */
/*description: The software set bit for SPI_SEG_MAGIC_ERR_INT interrupt..*/
#define SPI_SEG_MAGIC_ERR_INT_SET    (BIT(14))
#define SPI_SEG_MAGIC_ERR_INT_SET_M  (BIT(14))
#define SPI_SEG_MAGIC_ERR_INT_SET_V  0x1
#define SPI_SEG_MAGIC_ERR_INT_SET_S  14
/* SPI_DMA_SEG_TRANS_DONE_INT_SET : WT ;bitpos:[13] ;default: 1'b0 ; */
/*description: The software set bit for SPI_DMA_SEG_TRANS_DONE_INT interrupt..*/
#define SPI_DMA_SEG_TRANS_DONE_INT_SET    (BIT(13))
#define SPI_DMA_SEG_TRANS_DONE_INT_SET_M  (BIT(13))
#define SPI_DMA_SEG_TRANS_DONE_INT_SET_V  0x1
#define SPI_DMA_SEG_TRANS_DONE_INT_SET_S  13
/* SPI_TRANS_DONE_INT_SET : WT ;bitpos:[12] ;default: 1'b0 ; */
/*description: The software set bit for SPI_TRANS_DONE_INT interrupt..*/
#define SPI_TRANS_DONE_INT_SET    (BIT(12))
#define SPI_TRANS_DONE_INT_SET_M  (BIT(12))
#define SPI_TRANS_DONE_INT_SET_V  0x1
#define SPI_TRANS_DONE_INT_SET_S  12
/* SPI_SLV_WR_BUF_DONE_INT_SET : WT ;bitpos:[11] ;default: 1'b0 ; */
/*description: The software set bit for SPI_SLV_WR_BUF_DONE_INT interrupt..*/
#define SPI_SLV_WR_BUF_DONE_INT_SET    (BIT(11))
#define SPI_SLV_WR_BUF_DONE_INT_SET_M  (BIT(11))
#define SPI_SLV_WR_BUF_DONE_INT_SET_V  0x1
#define SPI_SLV_WR_BUF_DONE_INT_SET_S  11
/* SPI_SLV_RD_BUF_DONE_INT_SET : WT ;bitpos:[10] ;default: 1'b0 ; */
/*description: The software set bit for SPI_SLV_RD_BUF_DONE_INT interrupt..*/
#define SPI_SLV_RD_BUF_DONE_INT_SET    (BIT(10))
#define SPI_SLV_RD_BUF_DONE_INT_SET_M  (BIT(10))
#define SPI_SLV_RD_BUF_DONE_INT_SET_V  0x1
#define SPI_SLV_RD_BUF_DONE_INT_SET_S  10
/* SPI_SLV_WR_DMA_DONE_INT_SET : WT ;bitpos:[9] ;default: 1'b0 ; */
/*description: The software set bit for SPI_SLV_WR_DMA_DONE_INT interrupt..*/
#define SPI_SLV_WR_DMA_DONE_INT_SET    (BIT(9))
#define SPI_SLV_WR_DMA_DONE_INT_SET_M  (BIT(9))
#define SPI_SLV_WR_DMA_DONE_INT_SET_V  0x1
#define SPI_SLV_WR_DMA_DONE_INT_SET_S  9
/* SPI_SLV_RD_DMA_DONE_INT_SET : WT ;bitpos:[8] ;default: 1'b0 ; */
/*description: The software set bit for SPI_SLV_RD_DMA_DONE_INT interrupt..*/
#define SPI_SLV_RD_DMA_DONE_INT_SET    (BIT(8))
#define SPI_SLV_RD_DMA_DONE_INT_SET_M  (BIT(8))
#define SPI_SLV_RD_DMA_DONE_INT_SET_V  0x1
#define SPI_SLV_RD_DMA_DONE_INT_SET_S  8
/* SPI_SLV_CMDA_INT_SET : WT ;bitpos:[7] ;default: 1'b0 ; */
/*description: The software set bit for SPI slave CMDA interrupt..*/
#define SPI_SLV_CMDA_INT_SET    (BIT(7))
#define SPI_SLV_CMDA_INT_SET_M  (BIT(7))
#define SPI_SLV_CMDA_INT_SET_V  0x1
#define SPI_SLV_CMDA_INT_SET_S  7
/* SPI_SLV_CMD9_INT_SET : WT ;bitpos:[6] ;default: 1'b0 ; */
/*description: The software set bit for SPI slave CMD9 interrupt..*/
#define SPI_SLV_CMD9_INT_SET    (BIT(6))
#define SPI_SLV_CMD9_INT_SET_M  (BIT(6))
#define SPI_SLV_CMD9_INT_SET_V  0x1
#define SPI_SLV_CMD9_INT_SET_S  6
/* SPI_SLV_CMD8_INT_SET : WT ;bitpos:[5] ;default: 1'b0 ; */
/*description: The software set bit for SPI slave CMD8 interrupt..*/
#define SPI_SLV_CMD8_INT_SET    (BIT(5))
#define SPI_SLV_CMD8_INT_SET_M  (BIT(5))
#define SPI_SLV_CMD8_INT_SET_V  0x1
#define SPI_SLV_CMD8_INT_SET_S  5
/* SPI_SLV_CMD7_INT_SET : WT ;bitpos:[4] ;default: 1'b0 ; */
/*description: The software set bit for SPI slave CMD7 interrupt..*/
#define SPI_SLV_CMD7_INT_SET    (BIT(4))
#define SPI_SLV_CMD7_INT_SET_M  (BIT(4))
#define SPI_SLV_CMD7_INT_SET_V  0x1
#define SPI_SLV_CMD7_INT_SET_S  4
/* SPI_SLV_EN_QPI_INT_SET : WT ;bitpos:[3] ;default: 1'b0 ; */
/*description: The software set bit for SPI slave En_QPI interrupt..*/
#define SPI_SLV_EN_QPI_INT_SET    (BIT(3))
#define SPI_SLV_EN_QPI_INT_SET_M  (BIT(3))
#define SPI_SLV_EN_QPI_INT_SET_V  0x1
#define SPI_SLV_EN_QPI_INT_SET_S  3
/* SPI_SLV_EX_QPI_INT_SET : WT ;bitpos:[2] ;default: 1'b0 ; */
/*description: The software set bit for SPI slave Ex_QPI interrupt..*/
#define SPI_SLV_EX_QPI_INT_SET    (BIT(2))
#define SPI_SLV_EX_QPI_INT_SET_M  (BIT(2))
#define SPI_SLV_EX_QPI_INT_SET_V  0x1
#define SPI_SLV_EX_QPI_INT_SET_S  2
/* SPI_DMA_OUTFIFO_EMPTY_ERR_INT_SET : WT ;bitpos:[1] ;default: 1'b0 ; */
/*description: The software set bit for SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt..*/
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_SET    (BIT(1))
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_SET_M  (BIT(1))
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_SET_V  0x1
#define SPI_DMA_OUTFIFO_EMPTY_ERR_INT_SET_S  1
/* SPI_DMA_INFIFO_FULL_ERR_INT_SET : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: The software set bit for SPI_DMA_INFIFO_FULL_ERR_INT interrupt..*/
#define SPI_DMA_INFIFO_FULL_ERR_INT_SET    (BIT(0))
#define SPI_DMA_INFIFO_FULL_ERR_INT_SET_M  (BIT(0))
#define SPI_DMA_INFIFO_FULL_ERR_INT_SET_V  0x1
#define SPI_DMA_INFIFO_FULL_ERR_INT_SET_S  0

#define SPI_W0_REG(i)          (REG_SPI_BASE(i) + 0x98)
/* SPI_BUF0 : R/W/SS ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: data buffer.*/
#define SPI_BUF0    0xFFFFFFFF
#define SPI_BUF0_M  ((SPI_BUF0_V)<<(SPI_BUF0_S))
#define SPI_BUF0_V  0xFFFFFFFF
#define SPI_BUF0_S  0

#define SPI_W1_REG(i)          (REG_SPI_BASE(i) + 0x9C)
/* SPI_BUF1 : R/W/SS ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: data buffer.*/
#define SPI_BUF1    0xFFFFFFFF
#define SPI_BUF1_M  ((SPI_BUF1_V)<<(SPI_BUF1_S))
#define SPI_BUF1_V  0xFFFFFFFF
#define SPI_BUF1_S  0

#define SPI_W2_REG(i)          (REG_SPI_BASE(i) + 0xA0)
/* SPI_BUF2 : R/W/SS ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: data buffer.*/
#define SPI_BUF2    0xFFFFFFFF
#define SPI_BUF2_M  ((SPI_BUF2_V)<<(SPI_BUF2_S))
#define SPI_BUF2_V  0xFFFFFFFF
#define SPI_BUF2_S  0

#define SPI_W3_REG(i)          (REG_SPI_BASE(i) + 0xA4)
/* SPI_BUF3 : R/W/SS ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: data buffer.*/
#define SPI_BUF3    0xFFFFFFFF
#define SPI_BUF3_M  ((SPI_BUF3_V)<<(SPI_BUF3_S))
#define SPI_BUF3_V  0xFFFFFFFF
#define SPI_BUF3_S  0

#define SPI_W4_REG(i)          (REG_SPI_BASE(i) + 0xA8)
/* SPI_BUF4 : R/W/SS ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: data buffer.*/
#define SPI_BUF4    0xFFFFFFFF
#define SPI_BUF4_M  ((SPI_BUF4_V)<<(SPI_BUF4_S))
#define SPI_BUF4_V  0xFFFFFFFF
#define SPI_BUF4_S  0

#define SPI_W5_REG(i)          (REG_SPI_BASE(i) + 0xAC)
/* SPI_BUF5 : R/W/SS ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: data buffer.*/
#define SPI_BUF5    0xFFFFFFFF
#define SPI_BUF5_M  ((SPI_BUF5_V)<<(SPI_BUF5_S))
#define SPI_BUF5_V  0xFFFFFFFF
#define SPI_BUF5_S  0

#define SPI_W6_REG(i)          (REG_SPI_BASE(i) + 0xB0)
/* SPI_BUF6 : R/W/SS ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: data buffer.*/
#define SPI_BUF6    0xFFFFFFFF
#define SPI_BUF6_M  ((SPI_BUF6_V)<<(SPI_BUF6_S))
#define SPI_BUF6_V  0xFFFFFFFF
#define SPI_BUF6_S  0

#define SPI_W7_REG(i)          (REG_SPI_BASE(i) + 0xB4)
/* SPI_BUF7 : R/W/SS ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: data buffer.*/
#define SPI_BUF7    0xFFFFFFFF
#define SPI_BUF7_M  ((SPI_BUF7_V)<<(SPI_BUF7_S))
#define SPI_BUF7_V  0xFFFFFFFF
#define SPI_BUF7_S  0

#define SPI_W8_REG(i)          (REG_SPI_BASE(i) + 0xB8)
/* SPI_BUF8 : R/W/SS ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: data buffer.*/
#define SPI_BUF8    0xFFFFFFFF
#define SPI_BUF8_M  ((SPI_BUF8_V)<<(SPI_BUF8_S))
#define SPI_BUF8_V  0xFFFFFFFF
#define SPI_BUF8_S  0

#define SPI_W9_REG(i)          (REG_SPI_BASE(i) + 0xBC)
/* SPI_BUF9 : R/W/SS ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: data buffer.*/
#define SPI_BUF9    0xFFFFFFFF
#define SPI_BUF9_M  ((SPI_BUF9_V)<<(SPI_BUF9_S))
#define SPI_BUF9_V  0xFFFFFFFF
#define SPI_BUF9_S  0

#define SPI_W10_REG(i)          (REG_SPI_BASE(i) + 0xC0)
/* SPI_BUF10 : R/W/SS ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: data buffer.*/
#define SPI_BUF10    0xFFFFFFFF
#define SPI_BUF10_M  ((SPI_BUF10_V)<<(SPI_BUF10_S))
#define SPI_BUF10_V  0xFFFFFFFF
#define SPI_BUF10_S  0

#define SPI_W11_REG(i)          (REG_SPI_BASE(i) + 0xC4)
/* SPI_BUF11 : R/W/SS ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: data buffer.*/
#define SPI_BUF11    0xFFFFFFFF
#define SPI_BUF11_M  ((SPI_BUF11_V)<<(SPI_BUF11_S))
#define SPI_BUF11_V  0xFFFFFFFF
#define SPI_BUF11_S  0

#define SPI_W12_REG(i)          (REG_SPI_BASE(i) + 0xC8)
/* SPI_BUF12 : R/W/SS ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: data buffer.*/
#define SPI_BUF12    0xFFFFFFFF
#define SPI_BUF12_M  ((SPI_BUF12_V)<<(SPI_BUF12_S))
#define SPI_BUF12_V  0xFFFFFFFF
#define SPI_BUF12_S  0

#define SPI_W13_REG(i)          (REG_SPI_BASE(i) + 0xCC)
/* SPI_BUF13 : R/W/SS ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: data buffer.*/
#define SPI_BUF13    0xFFFFFFFF
#define SPI_BUF13_M  ((SPI_BUF13_V)<<(SPI_BUF13_S))
#define SPI_BUF13_V  0xFFFFFFFF
#define SPI_BUF13_S  0

#define SPI_W14_REG(i)          (REG_SPI_BASE(i) + 0xD0)
/* SPI_BUF14 : R/W/SS ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: data buffer.*/
#define SPI_BUF14    0xFFFFFFFF
#define SPI_BUF14_M  ((SPI_BUF14_V)<<(SPI_BUF14_S))
#define SPI_BUF14_V  0xFFFFFFFF
#define SPI_BUF14_S  0

#define SPI_W15_REG(i)          (REG_SPI_BASE(i) + 0xD4)
/* SPI_BUF15 : R/W/SS ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: data buffer.*/
#define SPI_BUF15    0xFFFFFFFF
#define SPI_BUF15_M  ((SPI_BUF15_V)<<(SPI_BUF15_S))
#define SPI_BUF15_V  0xFFFFFFFF
#define SPI_BUF15_S  0

#define SPI_SLAVE_REG(i)          (REG_SPI_BASE(i) + 0xE0)
/* SPI_USR_CONF : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: 1: Enable the DMA CONF phase of current seg-trans operation, which means seg-tra
ns will start. 0: This is not seg-trans mode..*/
#define SPI_USR_CONF    (BIT(28))
#define SPI_USR_CONF_M  (BIT(28))
#define SPI_USR_CONF_V  0x1
#define SPI_USR_CONF_S  28
/* SPI_SOFT_RESET : WT ;bitpos:[27] ;default: 1'b0 ; */
/*description: Software reset enable, reset the spi clock line cs line and data lines. Can be c
onfigured in CONF state..*/
#define SPI_SOFT_RESET    (BIT(27))
#define SPI_SOFT_RESET_M  (BIT(27))
#define SPI_SOFT_RESET_V  0x1
#define SPI_SOFT_RESET_S  27
/* SPI_SLAVE_MODE : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: Set SPI work mode. 1: slave mode 0: master mode..*/
#define SPI_SLAVE_MODE    (BIT(26))
#define SPI_SLAVE_MODE_M  (BIT(26))
#define SPI_SLAVE_MODE_V  0x1
#define SPI_SLAVE_MODE_S  26
/* SPI_DMA_SEG_MAGIC_VALUE : R/W ;bitpos:[25:22] ;default: 4'd10 ; */
/*description: The magic value of BM table in master DMA seg-trans..*/
#define SPI_DMA_SEG_MAGIC_VALUE    0x0000000F
#define SPI_DMA_SEG_MAGIC_VALUE_M  ((SPI_DMA_SEG_MAGIC_VALUE_V)<<(SPI_DMA_SEG_MAGIC_VALUE_S))
#define SPI_DMA_SEG_MAGIC_VALUE_V  0xF
#define SPI_DMA_SEG_MAGIC_VALUE_S  22
/* SPI_SLV_WRBUF_BITLEN_EN : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: 1: SPI_SLV_DATA_BITLEN stores data bit length of master-write-to-slave data leng
th in CPU controlled mode(Wr_BUF). 0: others.*/
#define SPI_SLV_WRBUF_BITLEN_EN    (BIT(11))
#define SPI_SLV_WRBUF_BITLEN_EN_M  (BIT(11))
#define SPI_SLV_WRBUF_BITLEN_EN_V  0x1
#define SPI_SLV_WRBUF_BITLEN_EN_S  11
/* SPI_SLV_RDBUF_BITLEN_EN : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: 1: SPI_SLV_DATA_BITLEN stores data bit length of master-read-slave data length i
n CPU controlled mode(Rd_BUF). 0: others.*/
#define SPI_SLV_RDBUF_BITLEN_EN    (BIT(10))
#define SPI_SLV_RDBUF_BITLEN_EN_M  (BIT(10))
#define SPI_SLV_RDBUF_BITLEN_EN_V  0x1
#define SPI_SLV_RDBUF_BITLEN_EN_S  10
/* SPI_SLV_WRDMA_BITLEN_EN : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: 1: SPI_SLV_DATA_BITLEN stores data bit length of master-write-to-slave data leng
th in DMA controlled mode(Wr_DMA). 0: others.*/
#define SPI_SLV_WRDMA_BITLEN_EN    (BIT(9))
#define SPI_SLV_WRDMA_BITLEN_EN_M  (BIT(9))
#define SPI_SLV_WRDMA_BITLEN_EN_V  0x1
#define SPI_SLV_WRDMA_BITLEN_EN_S  9
/* SPI_SLV_RDDMA_BITLEN_EN : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: 1: SPI_SLV_DATA_BITLEN stores data bit length of master-read-slave data length i
n DMA controlled mode(Rd_DMA). 0: others.*/
#define SPI_SLV_RDDMA_BITLEN_EN    (BIT(8))
#define SPI_SLV_RDDMA_BITLEN_EN_M  (BIT(8))
#define SPI_SLV_RDDMA_BITLEN_EN_V  0x1
#define SPI_SLV_RDDMA_BITLEN_EN_S  8
/* SPI_RSCK_DATA_OUT : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: It saves half a cycle when tsck is the same as rsck. 1: output data at rsck pose
dge   0: output data at tsck posedge.*/
#define SPI_RSCK_DATA_OUT    (BIT(3))
#define SPI_RSCK_DATA_OUT_M  (BIT(3))
#define SPI_RSCK_DATA_OUT_V  0x1
#define SPI_RSCK_DATA_OUT_S  3
/* SPI_CLK_MODE_13 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: {CPOL, CPHA},1: support spi clk mode 1 and 3, first edge output data B[0]/B[7].
 0: support spi clk mode 0 and 2, first edge output data B[1]/B[6]..*/
#define SPI_CLK_MODE_13    (BIT(2))
#define SPI_CLK_MODE_13_M  (BIT(2))
#define SPI_CLK_MODE_13_V  0x1
#define SPI_CLK_MODE_13_S  2
/* SPI_CLK_MODE : R/W ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delaye
d one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inacti
ve 3: SPI clock is always on. Can be configured in CONF state..*/
#define SPI_CLK_MODE    0x00000003
#define SPI_CLK_MODE_M  ((SPI_CLK_MODE_V)<<(SPI_CLK_MODE_S))
#define SPI_CLK_MODE_V  0x3
#define SPI_CLK_MODE_S  0

#define SPI_SLAVE1_REG(i)          (REG_SPI_BASE(i) + 0xE4)
/* SPI_SLV_LAST_ADDR : R/W/SS ;bitpos:[31:26] ;default: 6'd0 ; */
/*description: In the slave mode it is the value of address..*/
#define SPI_SLV_LAST_ADDR    0x0000003F
#define SPI_SLV_LAST_ADDR_M  ((SPI_SLV_LAST_ADDR_V)<<(SPI_SLV_LAST_ADDR_S))
#define SPI_SLV_LAST_ADDR_V  0x3F
#define SPI_SLV_LAST_ADDR_S  26
/* SPI_SLV_LAST_COMMAND : R/W/SS ;bitpos:[25:18] ;default: 8'b0 ; */
/*description: In the slave mode it is the value of command..*/
#define SPI_SLV_LAST_COMMAND    0x000000FF
#define SPI_SLV_LAST_COMMAND_M  ((SPI_SLV_LAST_COMMAND_V)<<(SPI_SLV_LAST_COMMAND_S))
#define SPI_SLV_LAST_COMMAND_V  0xFF
#define SPI_SLV_LAST_COMMAND_S  18
/* SPI_SLV_DATA_BITLEN : R/W/SS ;bitpos:[17:0] ;default: 18'd0 ; */
/*description: The transferred data bit length in SPI slave FD and HD mode..*/
#define SPI_SLV_DATA_BITLEN    0x0003FFFF
#define SPI_SLV_DATA_BITLEN_M  ((SPI_SLV_DATA_BITLEN_V)<<(SPI_SLV_DATA_BITLEN_S))
#define SPI_SLV_DATA_BITLEN_V  0x3FFFF
#define SPI_SLV_DATA_BITLEN_S  0

#define SPI_CLK_GATE_REG(i)          (REG_SPI_BASE(i) + 0xE8)
/* SPI_MST_CLK_SEL : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: This bit is used to select SPI module clock source in master mode. 1: PLL_CLK_80
M. 0: XTAL CLK..*/
#define SPI_MST_CLK_SEL    (BIT(2))
#define SPI_MST_CLK_SEL_M  (BIT(2))
#define SPI_MST_CLK_SEL_V  0x1
#define SPI_MST_CLK_SEL_S  2
/* SPI_MST_CLK_ACTIVE : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to power on the SPI module clock..*/
#define SPI_MST_CLK_ACTIVE    (BIT(1))
#define SPI_MST_CLK_ACTIVE_M  (BIT(1))
#define SPI_MST_CLK_ACTIVE_V  0x1
#define SPI_MST_CLK_ACTIVE_S  1
/* SPI_CLK_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to enable clk gate.*/
#define SPI_CLK_EN    (BIT(0))
#define SPI_CLK_EN_M  (BIT(0))
#define SPI_CLK_EN_V  0x1
#define SPI_CLK_EN_S  0

#define SPI_DATE_REG(i)          (REG_SPI_BASE(i) + 0xF0)
/* SPI_DATE : R/W ;bitpos:[27:0] ;default: 28'h2106070 ; */
/*description: SPI register version..*/
#define SPI_DATE    0x0FFFFFFF
#define SPI_DATE_M  ((SPI_DATE_V)<<(SPI_DATE_S))
#define SPI_DATE_V  0xFFFFFFF
#define SPI_DATE_S  0


#ifdef __cplusplus
}
#endif



#endif /*_SOC_SPI_REG_H_ */
