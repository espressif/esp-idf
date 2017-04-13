// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#ifndef __SPI_REG_H__
#define __SPI_REG_H__


#include "soc.h"
#define REG_SPI_BASE(i)     (DR_REG_SPI1_BASE + (((i)>1) ? (((i)* 0x1000) + 0x20000) : (((~(i)) & 1)* 0x1000 )))

#define SPI_CMD_REG(i)          (REG_SPI_BASE(i) + 0x0)
/* SPI_FLASH_READ : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: Read flash enable. Read flash operation will be triggered when
 the bit is set. The bit will be cleared once the operation done. 1: enable 0: disable.*/
#define SPI_FLASH_READ  (BIT(31))
#define SPI_FLASH_READ_M  (BIT(31))
#define SPI_FLASH_READ_V  0x1
#define SPI_FLASH_READ_S  31
/* SPI_FLASH_WREN : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: Write flash enable.  Write enable command will be sent when the
 bit is set. The bit will be cleared once the operation done. 1: enable 0: disable.*/
#define SPI_FLASH_WREN  (BIT(30))
#define SPI_FLASH_WREN_M  (BIT(30))
#define SPI_FLASH_WREN_V  0x1
#define SPI_FLASH_WREN_S  30
/* SPI_FLASH_WRDI : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: Write flash disable. Write disable command will be sent when
 the bit is set. The bit will be cleared once the operation done. 1: enable 0: disable.*/
#define SPI_FLASH_WRDI  (BIT(29))
#define SPI_FLASH_WRDI_M  (BIT(29))
#define SPI_FLASH_WRDI_V  0x1
#define SPI_FLASH_WRDI_S  29
/* SPI_FLASH_RDID : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: Read JEDEC ID . Read ID command will be sent when the bit is
 set. The bit will be cleared once the operation done. 1: enable 0: disable.*/
#define SPI_FLASH_RDID  (BIT(28))
#define SPI_FLASH_RDID_M  (BIT(28))
#define SPI_FLASH_RDID_V  0x1
#define SPI_FLASH_RDID_S  28
/* SPI_FLASH_RDSR : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: Read status register-1.  Read status operation will be triggered
 when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_FLASH_RDSR  (BIT(27))
#define SPI_FLASH_RDSR_M  (BIT(27))
#define SPI_FLASH_RDSR_V  0x1
#define SPI_FLASH_RDSR_S  27
/* SPI_FLASH_WRSR : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: Write status register enable.   Write status operation  will
 be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_FLASH_WRSR  (BIT(26))
#define SPI_FLASH_WRSR_M  (BIT(26))
#define SPI_FLASH_WRSR_V  0x1
#define SPI_FLASH_WRSR_S  26
/* SPI_FLASH_PP : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: Page program enable(1 byte ~256 bytes data to be programmed).
 Page program operation  will be triggered when the bit is set. The bit will be cleared once the operation done .1: enable 0: disable.*/
#define SPI_FLASH_PP  (BIT(25))
#define SPI_FLASH_PP_M  (BIT(25))
#define SPI_FLASH_PP_V  0x1
#define SPI_FLASH_PP_S  25
/* SPI_FLASH_SE : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: Sector erase enable. A 4KB sector is erased via SPI command 20H. Sector erase operation will be triggered
 when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_FLASH_SE  (BIT(24))
#define SPI_FLASH_SE_M  (BIT(24))
#define SPI_FLASH_SE_V  0x1
#define SPI_FLASH_SE_S  24
/* SPI_FLASH_BE : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: Block erase enable. A 64KB block is erased via SPI command D8H.  Block erase operation will be triggered
 when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_FLASH_BE  (BIT(23))
#define SPI_FLASH_BE_M  (BIT(23))
#define SPI_FLASH_BE_V  0x1
#define SPI_FLASH_BE_S  23
/* SPI_FLASH_CE : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: Chip erase enable. Chip erase operation will be triggered when
 the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_FLASH_CE  (BIT(22))
#define SPI_FLASH_CE_M  (BIT(22))
#define SPI_FLASH_CE_V  0x1
#define SPI_FLASH_CE_S  22
/* SPI_FLASH_DP : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: Drive Flash into power down.  An operation will be triggered
 when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_FLASH_DP  (BIT(21))
#define SPI_FLASH_DP_M  (BIT(21))
#define SPI_FLASH_DP_V  0x1
#define SPI_FLASH_DP_S  21
/* SPI_FLASH_RES : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: This bit combined with reg_resandres bit releases Flash from
 the power-down state or high performance mode and obtains the devices ID. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_FLASH_RES  (BIT(20))
#define SPI_FLASH_RES_M  (BIT(20))
#define SPI_FLASH_RES_V  0x1
#define SPI_FLASH_RES_S  20
/* SPI_FLASH_HPM : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: Drive Flash into high performance mode.  The bit will be cleared
 once the operation done.1: enable 0: disable.*/
#define SPI_FLASH_HPM  (BIT(19))
#define SPI_FLASH_HPM_M  (BIT(19))
#define SPI_FLASH_HPM_V  0x1
#define SPI_FLASH_HPM_S  19
/* SPI_USR : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: User define command enable.  An operation will be triggered when
 the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_USR  (BIT(18))
#define SPI_USR_M  (BIT(18))
#define SPI_USR_V  0x1
#define SPI_USR_S  18
/* SPI_FLASH_PES : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: program erase suspend bit  program erase suspend operation will
 be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_FLASH_PES  (BIT(17))
#define SPI_FLASH_PES_M  (BIT(17))
#define SPI_FLASH_PES_V  0x1
#define SPI_FLASH_PES_S  17
/* SPI_FLASH_PER : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: program erase resume bit  program erase suspend operation will
 be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_FLASH_PER  (BIT(16))
#define SPI_FLASH_PER_M  (BIT(16))
#define SPI_FLASH_PER_V  0x1
#define SPI_FLASH_PER_S  16

#define SPI_ADDR_REG(i)          (REG_SPI_BASE(i) + 0x4)
//The CSV actually is wrong here. It indicates that the lower 8 bits of this register are reserved. This is not true,
//all 32 bits of SPI_ADDR_REG are usable/used.

#define SPI_CTRL_REG(i)          (REG_SPI_BASE(i) + 0x8)
/* SPI_WR_BIT_ORDER : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: In command address write-data (MOSI) phases 1: LSB firs 0: MSB first*/
#define SPI_WR_BIT_ORDER  (BIT(26))
#define SPI_WR_BIT_ORDER_M  (BIT(26))
#define SPI_WR_BIT_ORDER_V  0x1
#define SPI_WR_BIT_ORDER_S  26
/* SPI_RD_BIT_ORDER : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: In read-data (MISO) phase 1: LSB first 0: MSB first*/
#define SPI_RD_BIT_ORDER  (BIT(25))
#define SPI_RD_BIT_ORDER_M  (BIT(25))
#define SPI_RD_BIT_ORDER_V  0x1
#define SPI_RD_BIT_ORDER_S  25
/* SPI_FREAD_QIO : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: In the read operations address phase and read-data phase apply
 4 signals. 1: enable 0: disable.*/
#define SPI_FREAD_QIO  (BIT(24))
#define SPI_FREAD_QIO_M  (BIT(24))
#define SPI_FREAD_QIO_V  0x1
#define SPI_FREAD_QIO_S  24
/* SPI_FREAD_DIO : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: In the read operations address phase and read-data phase apply
 2 signals. 1: enable 0: disable.*/
#define SPI_FREAD_DIO  (BIT(23))
#define SPI_FREAD_DIO_M  (BIT(23))
#define SPI_FREAD_DIO_V  0x1
#define SPI_FREAD_DIO_S  23
/* SPI_WRSR_2B : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: two bytes data will be written to status register when it is
 set. 1: enable 0: disable.*/
#define SPI_WRSR_2B  (BIT(22))
#define SPI_WRSR_2B_M  (BIT(22))
#define SPI_WRSR_2B_V  0x1
#define SPI_WRSR_2B_S  22
/* SPI_WP_REG : R/W ;bitpos:[21] ;default: 1'b1 ; */
/*description: Write protect signal output when SPI is idle.  1: output high  0: output low.*/
#define SPI_WP_REG  (BIT(21))
#define SPI_WP_REG_M  (BIT(21))
#define SPI_WP_REG_V  0x1
#define SPI_WP_REG_S  21
/* SPI_FREAD_QUAD : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: In the read operations read-data phase apply 4 signals. 1: enable 0: disable.*/
#define SPI_FREAD_QUAD  (BIT(20))
#define SPI_FREAD_QUAD_M  (BIT(20))
#define SPI_FREAD_QUAD_V  0x1
#define SPI_FREAD_QUAD_S  20
/* SPI_RESANDRES : R/W ;bitpos:[15] ;default: 1'b1 ; */
/*description: The Device ID is read out to SPI_RD_STATUS register, this bit
 combine with spi_flash_res bit. 1: enable 0: disable.*/
#define SPI_RESANDRES  (BIT(15))
#define SPI_RESANDRES_M  (BIT(15))
#define SPI_RESANDRES_V  0x1
#define SPI_RESANDRES_S  15
/* SPI_FREAD_DUAL : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: In the read operations  read-data phase apply 2 signals. 1: enable 0: disable.*/
#define SPI_FREAD_DUAL  (BIT(14))
#define SPI_FREAD_DUAL_M  (BIT(14))
#define SPI_FREAD_DUAL_V  0x1
#define SPI_FREAD_DUAL_S  14
/* SPI_FASTRD_MODE : R/W ;bitpos:[13] ;default: 1'b1 ; */
/*description: This bit enable the bits: spi_fread_qio  spi_fread_dio  spi_fread_qout
 and spi_fread_dout. 1: enable 0: disable.*/
#define SPI_FASTRD_MODE  (BIT(13))
#define SPI_FASTRD_MODE_M  (BIT(13))
#define SPI_FASTRD_MODE_V  0x1
#define SPI_FASTRD_MODE_S  13
/* SPI_WAIT_FLASH_IDLE_EN : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: wait flash idle when program flash or erase flash. 1: enable 0: disable.*/
#define SPI_WAIT_FLASH_IDLE_EN  (BIT(12))
#define SPI_WAIT_FLASH_IDLE_EN_M  (BIT(12))
#define SPI_WAIT_FLASH_IDLE_EN_V  0x1
#define SPI_WAIT_FLASH_IDLE_EN_S  12
/* SPI_TX_CRC_EN : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: For SPI1  enable crc32 when writing encrypted data to flash.
 1: enable  0:disable*/
#define SPI_TX_CRC_EN  (BIT(11))
#define SPI_TX_CRC_EN_M  (BIT(11))
#define SPI_TX_CRC_EN_V  0x1
#define SPI_TX_CRC_EN_S  11
/* SPI_FCS_CRC_EN : R/W ;bitpos:[10] ;default: 1'b1 ; */
/*description: For SPI1  initialize crc32 module before writing encrypted data
 to flash. Active low.*/
#define SPI_FCS_CRC_EN  (BIT(10))
#define SPI_FCS_CRC_EN_M  (BIT(10))
#define SPI_FCS_CRC_EN_V  0x1
#define SPI_FCS_CRC_EN_S  10

#define SPI_CTRL1_REG(i)          (REG_SPI_BASE(i) + 0xC)
/* SPI_CS_HOLD_DELAY : R/W ;bitpos:[31:28] ;default: 4'h5 ; */
/*description: SPI cs signal is delayed by spi clock cycles*/
#define SPI_CS_HOLD_DELAY  0x0000000F
#define SPI_CS_HOLD_DELAY_M  ((SPI_CS_HOLD_DELAY_V)<<(SPI_CS_HOLD_DELAY_S))
#define SPI_CS_HOLD_DELAY_V  0xF
#define SPI_CS_HOLD_DELAY_S  28
/* SPI_CS_HOLD_DELAY_RES : R/W ;bitpos:[27:16] ;default: 12'hfff ; */
/*description: Delay cycles of resume Flash when resume Flash is enable by spi clock.*/
#define SPI_CS_HOLD_DELAY_RES  0x00000FFF
#define SPI_CS_HOLD_DELAY_RES_M  ((SPI_CS_HOLD_DELAY_RES_V)<<(SPI_CS_HOLD_DELAY_RES_S))
#define SPI_CS_HOLD_DELAY_RES_V  0xFFF
#define SPI_CS_HOLD_DELAY_RES_S  16

#define SPI_RD_STATUS_REG(i)          (REG_SPI_BASE(i) + 0x10)
/* SPI_STATUS_EXT : R/W ;bitpos:[31:24] ;default: 8'h00 ; */
/*description: In the slave mode,it is the status for master to read out.*/
#define SPI_STATUS_EXT  0x000000FF
#define SPI_STATUS_EXT_M  ((SPI_STATUS_EXT_V)<<(SPI_STATUS_EXT_S))
#define SPI_STATUS_EXT_V  0xFF
#define SPI_STATUS_EXT_S  24
/* SPI_WB_MODE : R/W ;bitpos:[23:16] ;default: 8'h00 ; */
/*description: Mode bits in the flash fast read mode, it is combined with spi_fastrd_mode bit.*/
#define SPI_WB_MODE  0x000000FF
#define SPI_WB_MODE_M  ((SPI_WB_MODE_V)<<(SPI_WB_MODE_S))
#define SPI_WB_MODE_V  0xFF
#define SPI_WB_MODE_S  16
/* SPI_STATUS : R/W ;bitpos:[15:0] ;default: 16'b0 ; */
/*description: In the slave mode, it is the status for master to read out.*/
#define SPI_STATUS  0x0000FFFF
#define SPI_STATUS_M  ((SPI_STATUS_V)<<(SPI_STATUS_S))
#define SPI_STATUS_V  0xFFFF
#define SPI_STATUS_S  0

#define SPI_CTRL2_REG(i)          (REG_SPI_BASE(i) + 0x14)
/* SPI_CS_DELAY_NUM : R/W ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: spi_cs signal is delayed by system clock cycles*/
#define SPI_CS_DELAY_NUM  0x0000000F
#define SPI_CS_DELAY_NUM_M  ((SPI_CS_DELAY_NUM_V)<<(SPI_CS_DELAY_NUM_S))
#define SPI_CS_DELAY_NUM_V  0xF
#define SPI_CS_DELAY_NUM_S  28
/* SPI_CS_DELAY_MODE : R/W ;bitpos:[27:26] ;default: 2'h0 ; */
/*description: spi_cs signal is delayed by spi_clk . 0: zero  1: if spi_ck_out_edge
 or spi_ck_i_edge is set 1  delayed by half cycle    else delayed by one cycle  2: if spi_ck_out_edge or spi_ck_i_edge is set 1  delayed by one cycle   else delayed by half cycle  3: delayed one cycle*/
#define SPI_CS_DELAY_MODE  0x00000003
#define SPI_CS_DELAY_MODE_M  ((SPI_CS_DELAY_MODE_V)<<(SPI_CS_DELAY_MODE_S))
#define SPI_CS_DELAY_MODE_V  0x3
#define SPI_CS_DELAY_MODE_S  26
/* SPI_MOSI_DELAY_NUM : R/W ;bitpos:[25:23] ;default: 3'h0 ; */
/*description: MOSI signals are delayed by system clock cycles*/
#define SPI_MOSI_DELAY_NUM  0x00000007
#define SPI_MOSI_DELAY_NUM_M  ((SPI_MOSI_DELAY_NUM_V)<<(SPI_MOSI_DELAY_NUM_S))
#define SPI_MOSI_DELAY_NUM_V  0x7
#define SPI_MOSI_DELAY_NUM_S  23
/* SPI_MOSI_DELAY_MODE : R/W ;bitpos:[22:21] ;default: 2'h0 ; */
/*description: MOSI signals are delayed by spi_clk. 0: zero  1: if spi_ck_out_edge
 or spi_ck_i_edge is set 1  delayed by half cycle    else delayed by one cycle  2: if spi_ck_out_edge or spi_ck_i_edge is set 1  delayed by one cycle  else delayed by half cycle  3: delayed one cycle*/
#define SPI_MOSI_DELAY_MODE  0x00000003
#define SPI_MOSI_DELAY_MODE_M  ((SPI_MOSI_DELAY_MODE_V)<<(SPI_MOSI_DELAY_MODE_S))
#define SPI_MOSI_DELAY_MODE_V  0x3
#define SPI_MOSI_DELAY_MODE_S  21
/* SPI_MISO_DELAY_NUM : R/W ;bitpos:[20:18] ;default: 3'h0 ; */
/*description: MISO signals are delayed by system clock cycles*/
#define SPI_MISO_DELAY_NUM  0x00000007
#define SPI_MISO_DELAY_NUM_M  ((SPI_MISO_DELAY_NUM_V)<<(SPI_MISO_DELAY_NUM_S))
#define SPI_MISO_DELAY_NUM_V  0x7
#define SPI_MISO_DELAY_NUM_S  18
/* SPI_MISO_DELAY_MODE : R/W ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: MISO signals are delayed by spi_clk. 0: zero  1: if spi_ck_out_edge
 or spi_ck_i_edge is set 1  delayed by half cycle    else delayed by one cycle  2: if spi_ck_out_edge or spi_ck_i_edge is set 1  delayed by one cycle  else delayed by half cycle  3: delayed one cycle*/
#define SPI_MISO_DELAY_MODE  0x00000003
#define SPI_MISO_DELAY_MODE_M  ((SPI_MISO_DELAY_MODE_V)<<(SPI_MISO_DELAY_MODE_S))
#define SPI_MISO_DELAY_MODE_V  0x3
#define SPI_MISO_DELAY_MODE_S  16
/* SPI_CK_OUT_HIGH_MODE : R/W ;bitpos:[15:12] ;default: 4'h0 ; */
/*description: modify spi clock duty ratio when the value is lager than 8,
 the bits are combined with spi_clkcnt_N bits and spi_clkcnt_H bits.*/
#define SPI_CK_OUT_HIGH_MODE  0x0000000F
#define SPI_CK_OUT_HIGH_MODE_M  ((SPI_CK_OUT_HIGH_MODE_V)<<(SPI_CK_OUT_HIGH_MODE_S))
#define SPI_CK_OUT_HIGH_MODE_V  0xF
#define SPI_CK_OUT_HIGH_MODE_S  12
/* SPI_CK_OUT_LOW_MODE : R/W ;bitpos:[11:8] ;default: 4'h0 ; */
/*description: modify spi clock duty ratio when the value is lager than 8,
 the bits are combined with spi_clkcnt_N bits and spi_clkcnt_L bits.*/
#define SPI_CK_OUT_LOW_MODE  0x0000000F
#define SPI_CK_OUT_LOW_MODE_M  ((SPI_CK_OUT_LOW_MODE_V)<<(SPI_CK_OUT_LOW_MODE_S))
#define SPI_CK_OUT_LOW_MODE_V  0xF
#define SPI_CK_OUT_LOW_MODE_S  8
/* SPI_HOLD_TIME : R/W ;bitpos:[7:4] ;default: 4'h1 ; */
/*description: delay cycles of cs pin by spi clock, this bits combined with spi_cs_hold bit.*/
#define SPI_HOLD_TIME  0x0000000F
#define SPI_HOLD_TIME_M  ((SPI_HOLD_TIME_V)<<(SPI_HOLD_TIME_S))
#define SPI_HOLD_TIME_V  0xF
#define SPI_HOLD_TIME_S  4
/* SPI_SETUP_TIME : R/W ;bitpos:[3:0] ;default: 4'h1 ; */
/*description: (cycles-1) of ¡°prepare¡± phase by spi clock, this bits combined
 with spi_cs_setup bit.*/
#define SPI_SETUP_TIME  0x0000000F
#define SPI_SETUP_TIME_M  ((SPI_SETUP_TIME_V)<<(SPI_SETUP_TIME_S))
#define SPI_SETUP_TIME_V  0xF
#define SPI_SETUP_TIME_S  0

#define SPI_CLOCK_REG(i)          (REG_SPI_BASE(i) + 0x18)
/* SPI_CLK_EQU_SYSCLK : R/W ;bitpos:[31] ;default: 1'b1 ; */
/*description: In the master mode 1: spi_clk is eqaul to system 0: spi_clk is
 divided from system clock.*/
#define SPI_CLK_EQU_SYSCLK  (BIT(31))
#define SPI_CLK_EQU_SYSCLK_M  (BIT(31))
#define SPI_CLK_EQU_SYSCLK_V  0x1
#define SPI_CLK_EQU_SYSCLK_S  31
/* SPI_CLKDIV_PRE : R/W ;bitpos:[30:18] ;default: 13'b0 ; */
/*description: In the master mode it is pre-divider of spi_clk.*/
#define SPI_CLKDIV_PRE  0x00001FFF
#define SPI_CLKDIV_PRE_M  ((SPI_CLKDIV_PRE_V)<<(SPI_CLKDIV_PRE_S))
#define SPI_CLKDIV_PRE_V  0x1FFF
#define SPI_CLKDIV_PRE_S  18
/* SPI_CLKCNT_N : R/W ;bitpos:[17:12] ;default: 6'h3 ; */
/*description: In the master mode it is the divider of spi_clk. So spi_clk frequency
 is system/(spi_clkdiv_pre+1)/(spi_clkcnt_N+1)*/
#define SPI_CLKCNT_N  0x0000003F
#define SPI_CLKCNT_N_M  ((SPI_CLKCNT_N_V)<<(SPI_CLKCNT_N_S))
#define SPI_CLKCNT_N_V  0x3F
#define SPI_CLKCNT_N_S  12
/* SPI_CLKCNT_H : R/W ;bitpos:[11:6] ;default: 6'h1 ; */
/*description: In the master mode it must be floor((spi_clkcnt_N+1)/2-1). In
 the slave mode it must be 0.*/
#define SPI_CLKCNT_H  0x0000003F
#define SPI_CLKCNT_H_M  ((SPI_CLKCNT_H_V)<<(SPI_CLKCNT_H_S))
#define SPI_CLKCNT_H_V  0x3F
#define SPI_CLKCNT_H_S  6
/* SPI_CLKCNT_L : R/W ;bitpos:[5:0] ;default: 6'h3 ; */
/*description: In the master mode it must be equal to spi_clkcnt_N. In the slave
 mode it must be 0.*/
#define SPI_CLKCNT_L  0x0000003F
#define SPI_CLKCNT_L_M  ((SPI_CLKCNT_L_V)<<(SPI_CLKCNT_L_S))
#define SPI_CLKCNT_L_V  0x3F
#define SPI_CLKCNT_L_S  0

#define SPI_USER_REG(i)          (REG_SPI_BASE(i) + 0x1C)
/* SPI_USR_COMMAND : R/W ;bitpos:[31] ;default: 1'b1 ; */
/*description: This bit enable the command phase of an operation.*/
#define SPI_USR_COMMAND  (BIT(31))
#define SPI_USR_COMMAND_M  (BIT(31))
#define SPI_USR_COMMAND_V  0x1
#define SPI_USR_COMMAND_S  31
/* SPI_USR_ADDR : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: This bit enable the address phase of an operation.*/
#define SPI_USR_ADDR  (BIT(30))
#define SPI_USR_ADDR_M  (BIT(30))
#define SPI_USR_ADDR_V  0x1
#define SPI_USR_ADDR_S  30
/* SPI_USR_DUMMY : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: This bit enable the dummy phase of an operation.*/
#define SPI_USR_DUMMY  (BIT(29))
#define SPI_USR_DUMMY_M  (BIT(29))
#define SPI_USR_DUMMY_V  0x1
#define SPI_USR_DUMMY_S  29
/* SPI_USR_MISO : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: This bit enable the read-data phase of an operation.*/
#define SPI_USR_MISO  (BIT(28))
#define SPI_USR_MISO_M  (BIT(28))
#define SPI_USR_MISO_V  0x1
#define SPI_USR_MISO_S  28
/* SPI_USR_MOSI : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: This bit enable the write-data phase of an operation.*/
#define SPI_USR_MOSI  (BIT(27))
#define SPI_USR_MOSI_M  (BIT(27))
#define SPI_USR_MOSI_V  0x1
#define SPI_USR_MOSI_S  27
/* SPI_USR_DUMMY_IDLE : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: spi clock is disable in dummy phase when the bit is enable.*/
#define SPI_USR_DUMMY_IDLE  (BIT(26))
#define SPI_USR_DUMMY_IDLE_M  (BIT(26))
#define SPI_USR_DUMMY_IDLE_V  0x1
#define SPI_USR_DUMMY_IDLE_S  26
/* SPI_USR_MOSI_HIGHPART : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: write-data phase only access to high-part of the buffer spi_w8~spi_w15.
 1: enable 0: disable.*/
#define SPI_USR_MOSI_HIGHPART  (BIT(25))
#define SPI_USR_MOSI_HIGHPART_M  (BIT(25))
#define SPI_USR_MOSI_HIGHPART_V  0x1
#define SPI_USR_MOSI_HIGHPART_S  25
/* SPI_USR_MISO_HIGHPART : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: read-data phase only access to high-part of the buffer spi_w8~spi_w15.
 1: enable 0: disable.*/
#define SPI_USR_MISO_HIGHPART  (BIT(24))
#define SPI_USR_MISO_HIGHPART_M  (BIT(24))
#define SPI_USR_MISO_HIGHPART_V  0x1
#define SPI_USR_MISO_HIGHPART_S  24
/* SPI_USR_PREP_HOLD : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: spi is hold at prepare state  the bit combined with spi_usr_hold_pol bit.*/
#define SPI_USR_PREP_HOLD  (BIT(23))
#define SPI_USR_PREP_HOLD_M  (BIT(23))
#define SPI_USR_PREP_HOLD_V  0x1
#define SPI_USR_PREP_HOLD_S  23
/* SPI_USR_CMD_HOLD : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: spi is hold at command state  the bit combined with spi_usr_hold_pol bit.*/
#define SPI_USR_CMD_HOLD  (BIT(22))
#define SPI_USR_CMD_HOLD_M  (BIT(22))
#define SPI_USR_CMD_HOLD_V  0x1
#define SPI_USR_CMD_HOLD_S  22
/* SPI_USR_ADDR_HOLD : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: spi is hold at address state  the bit combined with spi_usr_hold_pol bit.*/
#define SPI_USR_ADDR_HOLD  (BIT(21))
#define SPI_USR_ADDR_HOLD_M  (BIT(21))
#define SPI_USR_ADDR_HOLD_V  0x1
#define SPI_USR_ADDR_HOLD_S  21
/* SPI_USR_DUMMY_HOLD : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: spi is hold at dummy state  the bit combined with spi_usr_hold_pol bit.*/
#define SPI_USR_DUMMY_HOLD  (BIT(20))
#define SPI_USR_DUMMY_HOLD_M  (BIT(20))
#define SPI_USR_DUMMY_HOLD_V  0x1
#define SPI_USR_DUMMY_HOLD_S  20
/* SPI_USR_DIN_HOLD : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: spi is hold at data in state  the bit combined with spi_usr_hold_pol bit.*/
#define SPI_USR_DIN_HOLD  (BIT(19))
#define SPI_USR_DIN_HOLD_M  (BIT(19))
#define SPI_USR_DIN_HOLD_V  0x1
#define SPI_USR_DIN_HOLD_S  19
/* SPI_USR_DOUT_HOLD : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: spi is hold at data out state  the bit combined with spi_usr_hold_pol bit.*/
#define SPI_USR_DOUT_HOLD  (BIT(18))
#define SPI_USR_DOUT_HOLD_M  (BIT(18))
#define SPI_USR_DOUT_HOLD_V  0x1
#define SPI_USR_DOUT_HOLD_S  18
/* SPI_USR_HOLD_POL : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: It is combined with hold bits to set the polarity of spi hold
 line  1: spi will be held when spi hold line is high  0: spi will be held when spi hold line is low*/
#define SPI_USR_HOLD_POL  (BIT(17))
#define SPI_USR_HOLD_POL_M  (BIT(17))
#define SPI_USR_HOLD_POL_V  0x1
#define SPI_USR_HOLD_POL_S  17
/* SPI_SIO : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: Set the bit to enable 3-line half duplex communication  mosi
 and miso signals share the same pin. 1: enable 0: disable.*/
#define SPI_SIO  (BIT(16))
#define SPI_SIO_M  (BIT(16))
#define SPI_SIO_V  0x1
#define SPI_SIO_S  16
/* SPI_FWRITE_QIO : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: In the write operations address phase and read-data phase apply 4 signals.*/
#define SPI_FWRITE_QIO  (BIT(15))
#define SPI_FWRITE_QIO_M  (BIT(15))
#define SPI_FWRITE_QIO_V  0x1
#define SPI_FWRITE_QIO_S  15
/* SPI_FWRITE_DIO : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: In the write operations address phase and read-data phase apply 2 signals.*/
#define SPI_FWRITE_DIO  (BIT(14))
#define SPI_FWRITE_DIO_M  (BIT(14))
#define SPI_FWRITE_DIO_V  0x1
#define SPI_FWRITE_DIO_S  14
/* SPI_FWRITE_QUAD : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: In the write operations read-data phase apply 4 signals*/
#define SPI_FWRITE_QUAD  (BIT(13))
#define SPI_FWRITE_QUAD_M  (BIT(13))
#define SPI_FWRITE_QUAD_V  0x1
#define SPI_FWRITE_QUAD_S  13
/* SPI_FWRITE_DUAL : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: In the write operations read-data phase apply 2 signals*/
#define SPI_FWRITE_DUAL  (BIT(12))
#define SPI_FWRITE_DUAL_M  (BIT(12))
#define SPI_FWRITE_DUAL_V  0x1
#define SPI_FWRITE_DUAL_S  12
/* SPI_WR_BYTE_ORDER : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: In command address write-data (MOSI) phases 1: big-endian 0: litte_endian*/
#define SPI_WR_BYTE_ORDER  (BIT(11))
#define SPI_WR_BYTE_ORDER_M  (BIT(11))
#define SPI_WR_BYTE_ORDER_V  0x1
#define SPI_WR_BYTE_ORDER_S  11
/* SPI_RD_BYTE_ORDER : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: In read-data (MISO) phase 1: big-endian 0: little_endian*/
#define SPI_RD_BYTE_ORDER  (BIT(10))
#define SPI_RD_BYTE_ORDER_M  (BIT(10))
#define SPI_RD_BYTE_ORDER_V  0x1
#define SPI_RD_BYTE_ORDER_S  10
/* SPI_CK_OUT_EDGE : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: the bit combined with spi_mosi_delay_mode bits to set mosi signal delay mode.*/
#define SPI_CK_OUT_EDGE  (BIT(7))
#define SPI_CK_OUT_EDGE_M  (BIT(7))
#define SPI_CK_OUT_EDGE_V  0x1
#define SPI_CK_OUT_EDGE_S  7
/* SPI_CK_I_EDGE : R/W ;bitpos:[6] ;default: 1'b1 ; */
/*description: In the slave mode  the bit is same as spi_ck_out_edge in master
 mode. It is combined with  spi_miso_delay_mode bits.*/
#define SPI_CK_I_EDGE  (BIT(6))
#define SPI_CK_I_EDGE_M  (BIT(6))
#define SPI_CK_I_EDGE_V  0x1
#define SPI_CK_I_EDGE_S  6
/* SPI_CS_SETUP : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: spi cs is enable when spi is in ¡°prepare¡± phase. 1: enable 0: disable.*/
#define SPI_CS_SETUP  (BIT(5))
#define SPI_CS_SETUP_M  (BIT(5))
#define SPI_CS_SETUP_V  0x1
#define SPI_CS_SETUP_S  5
/* SPI_CS_HOLD : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: spi cs keep low when spi is in ¡°done¡± phase. 1: enable 0: disable.*/
#define SPI_CS_HOLD  (BIT(4))
#define SPI_CS_HOLD_M  (BIT(4))
#define SPI_CS_HOLD_V  0x1
#define SPI_CS_HOLD_S  4
/* SPI_DOUTDIN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set the bit to enable full duplex communication. 1: enable 0: disable.*/
#define SPI_DOUTDIN  (BIT(0))
#define SPI_DOUTDIN_M  (BIT(0))
#define SPI_DOUTDIN_V  0x1
#define SPI_DOUTDIN_S  0

#define SPI_USER1_REG(i)          (REG_SPI_BASE(i) + 0x20)
/* SPI_USR_ADDR_BITLEN : RO ;bitpos:[31:26] ;default: 6'd23 ; */
/*description: The length in bits of address phase. The register value shall be (bit_num-1).*/
#define SPI_USR_ADDR_BITLEN  0x0000003F
#define SPI_USR_ADDR_BITLEN_M  ((SPI_USR_ADDR_BITLEN_V)<<(SPI_USR_ADDR_BITLEN_S))
#define SPI_USR_ADDR_BITLEN_V  0x3F
#define SPI_USR_ADDR_BITLEN_S  26
/* SPI_USR_DUMMY_CYCLELEN : R/W ;bitpos:[7:0] ;default: 8'd7 ; */
/*description: The length in spi_clk cycles of dummy phase. The register value
 shall be (cycle_num-1).*/
#define SPI_USR_DUMMY_CYCLELEN  0x000000FF
#define SPI_USR_DUMMY_CYCLELEN_M  ((SPI_USR_DUMMY_CYCLELEN_V)<<(SPI_USR_DUMMY_CYCLELEN_S))
#define SPI_USR_DUMMY_CYCLELEN_V  0xFF
#define SPI_USR_DUMMY_CYCLELEN_S  0

#define SPI_USER2_REG(i)          (REG_SPI_BASE(i) + 0x24)
/* SPI_USR_COMMAND_BITLEN : R/W ;bitpos:[31:28] ;default: 4'd7 ; */
/*description: The length in bits of command phase. The register value shall be (bit_num-1)*/
#define SPI_USR_COMMAND_BITLEN  0x0000000F
#define SPI_USR_COMMAND_BITLEN_M  ((SPI_USR_COMMAND_BITLEN_V)<<(SPI_USR_COMMAND_BITLEN_S))
#define SPI_USR_COMMAND_BITLEN_V  0xF
#define SPI_USR_COMMAND_BITLEN_S  28
/* SPI_USR_COMMAND_VALUE : R/W ;bitpos:[15:0] ;default: 16'b0 ; */
/*description: The value of  command.*/
#define SPI_USR_COMMAND_VALUE  0x0000FFFF
#define SPI_USR_COMMAND_VALUE_M  ((SPI_USR_COMMAND_VALUE_V)<<(SPI_USR_COMMAND_VALUE_S))
#define SPI_USR_COMMAND_VALUE_V  0xFFFF
#define SPI_USR_COMMAND_VALUE_S  0

#define SPI_MOSI_DLEN_REG(i)          (REG_SPI_BASE(i) + 0x28)
/* SPI_USR_MOSI_DBITLEN : R/W ;bitpos:[23:0] ;default: 24'h0 ; */
/*description: The length in bits of write-data. The register value shall be (bit_num-1).*/
#define SPI_USR_MOSI_DBITLEN  0x00FFFFFF
#define SPI_USR_MOSI_DBITLEN_M  ((SPI_USR_MOSI_DBITLEN_V)<<(SPI_USR_MOSI_DBITLEN_S))
#define SPI_USR_MOSI_DBITLEN_V  0xFFFFFF
#define SPI_USR_MOSI_DBITLEN_S  0

#define SPI_MISO_DLEN_REG(i)          (REG_SPI_BASE(i) + 0x2C)
/* SPI_USR_MISO_DBITLEN : R/W ;bitpos:[23:0] ;default: 24'h0 ; */
/*description: The length in bits of  read-data. The register value shall be (bit_num-1).*/
#define SPI_USR_MISO_DBITLEN  0x00FFFFFF
#define SPI_USR_MISO_DBITLEN_M  ((SPI_USR_MISO_DBITLEN_V)<<(SPI_USR_MISO_DBITLEN_S))
#define SPI_USR_MISO_DBITLEN_V  0xFFFFFF
#define SPI_USR_MISO_DBITLEN_S  0

#define SPI_SLV_WR_STATUS_REG(i)          (REG_SPI_BASE(i) + 0x30)
/* SPI_SLV_WR_ST : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: In the slave mode this register are the status register for the
 master to write into. In the master mode this register are the higher 32bits in the 64 bits address condition.*/
#define SPI_SLV_WR_ST  0xFFFFFFFF
#define SPI_SLV_WR_ST_M  ((SPI_SLV_WR_ST_V)<<(SPI_SLV_WR_ST_S))
#define SPI_SLV_WR_ST_V  0xFFFFFFFF
#define SPI_SLV_WR_ST_S  0

#define SPI_PIN_REG(i)          (REG_SPI_BASE(i) + 0x34)
/* SPI_CS_KEEP_ACTIVE : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: spi cs line keep low when the bit is set.*/
#define SPI_CS_KEEP_ACTIVE  (BIT(30))
#define SPI_CS_KEEP_ACTIVE_M  (BIT(30))
#define SPI_CS_KEEP_ACTIVE_V  0x1
#define SPI_CS_KEEP_ACTIVE_S  30
/* SPI_CK_IDLE_EDGE : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: 1: spi clk line is high when idle     0: spi clk line is low when idle*/
#define SPI_CK_IDLE_EDGE  (BIT(29))
#define SPI_CK_IDLE_EDGE_M  (BIT(29))
#define SPI_CK_IDLE_EDGE_V  0x1
#define SPI_CK_IDLE_EDGE_S  29
/* SPI_MASTER_CK_SEL : R/W ;bitpos:[13:11] ;default: 3'b0 ; */
/*description: In the master mode  spi cs line is enable as spi clk  it is combined
 with spi_cs0_dis spi_cs1_dis spi_cs2_dis.*/
#define SPI_MASTER_CK_SEL  0x00000007
#define SPI_MASTER_CK_SEL_M  ((SPI_MASTER_CK_SEL_V)<<(SPI_MASTER_CK_SEL_S))
#define SPI_MASTER_CK_SEL_V  0x07
#define SPI_MASTER_CK_SEL_S  11
/* SPI_MASTER_CS_POL : R/W ;bitpos:[8:6] ;default: 3'b0 ; */
/*description: In the master mode  the bits are the polarity of spi cs line
  the value is equivalent to spi_cs ^ spi_master_cs_pol.*/
#define SPI_MASTER_CS_POL  0x00000007
#define SPI_MASTER_CS_POL_M  ((SPI_MASTER_CS_POL_V)<<(SPI_MASTER_CS_POL_S))
#define SPI_MASTER_CS_POL_V  0x7
#define SPI_MASTER_CS_POL_S  6
/* SPI_CK_DIS : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: 1: spi clk out disable  0: spi clk out enable*/
#define SPI_CK_DIS  (BIT(5))
#define SPI_CK_DIS_M  (BIT(5))
#define SPI_CK_DIS_V  0x1
#define SPI_CK_DIS_S  5
/* SPI_CS2_DIS : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: SPI CS2 pin enable, 1: disable CS2, 0: spi_cs2 signal is from/to CS2 pin*/
#define SPI_CS2_DIS  (BIT(2))
#define SPI_CS2_DIS_M  (BIT(2))
#define SPI_CS2_DIS_V  0x1
#define SPI_CS2_DIS_S  2
/* SPI_CS1_DIS : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: SPI CS1 pin enable, 1: disable CS1, 0: spi_cs1 signal is from/to CS1 pin*/
#define SPI_CS1_DIS  (BIT(1))
#define SPI_CS1_DIS_M  (BIT(1))
#define SPI_CS1_DIS_V  0x1
#define SPI_CS1_DIS_S  1
/* SPI_CS0_DIS : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: SPI CS0 pin enable, 1: disable CS0, 0: spi_cs0 signal is from/to CS0 pin*/
#define SPI_CS0_DIS  (BIT(0))
#define SPI_CS0_DIS_M  (BIT(0))
#define SPI_CS0_DIS_V  0x1
#define SPI_CS0_DIS_S  0

#define SPI_SLAVE_REG(i)          (REG_SPI_BASE(i) + 0x38)
/* SPI_SYNC_RESET : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: Software reset enable, reset the spi clock line cs line and data lines.*/
#define SPI_SYNC_RESET  (BIT(31))
#define SPI_SYNC_RESET_M  (BIT(31))
#define SPI_SYNC_RESET_V  0x1
#define SPI_SYNC_RESET_S  31
/* SPI_SLAVE_MODE : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: 1: slave mode 0: master mode.*/
#define SPI_SLAVE_MODE  (BIT(30))
#define SPI_SLAVE_MODE_M  (BIT(30))
#define SPI_SLAVE_MODE_V  0x1
#define SPI_SLAVE_MODE_S  30
/* SPI_SLV_WR_RD_BUF_EN : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: write and read buffer enable in the slave mode*/
#define SPI_SLV_WR_RD_BUF_EN  (BIT(29))
#define SPI_SLV_WR_RD_BUF_EN_M  (BIT(29))
#define SPI_SLV_WR_RD_BUF_EN_V  0x1
#define SPI_SLV_WR_RD_BUF_EN_S  29
/* SPI_SLV_WR_RD_STA_EN : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: write and read status enable  in the slave mode*/
#define SPI_SLV_WR_RD_STA_EN  (BIT(28))
#define SPI_SLV_WR_RD_STA_EN_M  (BIT(28))
#define SPI_SLV_WR_RD_STA_EN_V  0x1
#define SPI_SLV_WR_RD_STA_EN_S  28
/* SPI_SLV_CMD_DEFINE : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: 1: slave mode commands are defined in SPI_SLAVE3.  0: slave mode
 commands are fixed as: 1: write-status 2: write-buffer and 3: read-buffer.*/
#define SPI_SLV_CMD_DEFINE  (BIT(27))
#define SPI_SLV_CMD_DEFINE_M  (BIT(27))
#define SPI_SLV_CMD_DEFINE_V  0x1
#define SPI_SLV_CMD_DEFINE_S  27
/* SPI_TRANS_CNT : RO ;bitpos:[26:23] ;default: 4'b0 ; */
/*description: The operations counter in both the master mode and the slave
 mode. 4: read-status*/
#define SPI_TRANS_CNT  0x0000000F
#define SPI_TRANS_CNT_M  ((SPI_TRANS_CNT_V)<<(SPI_TRANS_CNT_S))
#define SPI_TRANS_CNT_V  0xF
#define SPI_TRANS_CNT_S  23
/* SPI_SLV_LAST_STATE : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: In the slave mode it is the state of spi state machine.*/
#define SPI_SLV_LAST_STATE  0x00000007
#define SPI_SLV_LAST_STATE_M  ((SPI_SLV_LAST_STATE_V)<<(SPI_SLV_LAST_STATE_S))
#define SPI_SLV_LAST_STATE_V  0x7
#define SPI_SLV_LAST_STATE_S  20
/* SPI_SLV_LAST_COMMAND : RO ;bitpos:[19:17] ;default: 3'b0 ; */
/*description: In the slave mode it is the value of command.*/
#define SPI_SLV_LAST_COMMAND  0x00000007
#define SPI_SLV_LAST_COMMAND_M  ((SPI_SLV_LAST_COMMAND_V)<<(SPI_SLV_LAST_COMMAND_S))
#define SPI_SLV_LAST_COMMAND_V  0x7
#define SPI_SLV_LAST_COMMAND_S  17
/* SPI_CS_I_MODE : R/W ;bitpos:[11:10] ;default: 2'b0 ; */
/*description: In the slave mode  this bits used to synchronize the input spi
 cs signal and eliminate spi cs  jitter.*/
#define SPI_CS_I_MODE  0x00000003
#define SPI_CS_I_MODE_M  ((SPI_CS_I_MODE_V)<<(SPI_CS_I_MODE_S))
#define SPI_CS_I_MODE_V  0x3
#define SPI_CS_I_MODE_S  10
/* SPI_INT_EN : R/W ;bitpos:[9:5] ;default: 5'b1_0000 ; */
/*description: Interrupt enable bits for the below 5 sources*/
#define SPI_INT_EN  0x0000001F
#define SPI_INT_EN_M  ((SPI_INT_EN_V)<<(SPI_INT_EN_S))
#define SPI_INT_EN_V  0x1F
#define SPI_INT_EN_S  5
/* SPI_TRANS_DONE : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt raw bit for the completion of any operation in
 both the master mode and the slave mode.*/
#define SPI_TRANS_DONE  (BIT(4))
#define SPI_TRANS_DONE_M  (BIT(4))
#define SPI_TRANS_DONE_V  0x1
#define SPI_TRANS_DONE_S  4
/* SPI_SLV_WR_STA_DONE : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt raw bit for the completion of write-status operation
 in the slave mode.*/
#define SPI_SLV_WR_STA_DONE  (BIT(3))
#define SPI_SLV_WR_STA_DONE_M  (BIT(3))
#define SPI_SLV_WR_STA_DONE_V  0x1
#define SPI_SLV_WR_STA_DONE_S  3
/* SPI_SLV_RD_STA_DONE : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt raw bit for the completion of read-status operation
 in the slave mode.*/
#define SPI_SLV_RD_STA_DONE  (BIT(2))
#define SPI_SLV_RD_STA_DONE_M  (BIT(2))
#define SPI_SLV_RD_STA_DONE_V  0x1
#define SPI_SLV_RD_STA_DONE_S  2
/* SPI_SLV_WR_BUF_DONE : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt raw bit for the completion of write-buffer operation
 in the slave mode.*/
#define SPI_SLV_WR_BUF_DONE  (BIT(1))
#define SPI_SLV_WR_BUF_DONE_M  (BIT(1))
#define SPI_SLV_WR_BUF_DONE_V  0x1
#define SPI_SLV_WR_BUF_DONE_S  1
/* SPI_SLV_RD_BUF_DONE : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt raw bit for the completion of read-buffer operation
 in the slave mode.*/
#define SPI_SLV_RD_BUF_DONE  (BIT(0))
#define SPI_SLV_RD_BUF_DONE_M  (BIT(0))
#define SPI_SLV_RD_BUF_DONE_V  0x1
#define SPI_SLV_RD_BUF_DONE_S  0

#define SPI_SLAVE1_REG(i)          (REG_SPI_BASE(i) + 0x3C)
/* SPI_SLV_STATUS_BITLEN : R/W ;bitpos:[31:27] ;default: 5'b0 ; */
/*description: In the slave mode it is the length of status bit.*/
#define SPI_SLV_STATUS_BITLEN  0x0000001F
#define SPI_SLV_STATUS_BITLEN_M  ((SPI_SLV_STATUS_BITLEN_V)<<(SPI_SLV_STATUS_BITLEN_S))
#define SPI_SLV_STATUS_BITLEN_V  0x1F
#define SPI_SLV_STATUS_BITLEN_S  27
/* SPI_SLV_STATUS_FAST_EN : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: In the slave mode enable fast read status.*/
#define SPI_SLV_STATUS_FAST_EN  (BIT(26))
#define SPI_SLV_STATUS_FAST_EN_M  (BIT(26))
#define SPI_SLV_STATUS_FAST_EN_V  0x1
#define SPI_SLV_STATUS_FAST_EN_S  26
/* SPI_SLV_STATUS_READBACK : R/W ;bitpos:[25] ;default: 1'b1 ; */
/*description: In the slave mode  1:read register of SPI_SLV_WR_STATUS  0: read
 register of SPI_RD_STATUS.*/
#define SPI_SLV_STATUS_READBACK  (BIT(25))
#define SPI_SLV_STATUS_READBACK_M  (BIT(25))
#define SPI_SLV_STATUS_READBACK_V  0x1
#define SPI_SLV_STATUS_READBACK_S  25
/* SPI_SLV_RD_ADDR_BITLEN : R/W ;bitpos:[15:10] ;default: 6'h0 ; */
/*description: In the slave mode it is the address length in bits for read-buffer
 operation. The register value shall be (bit_num-1).*/
#define SPI_SLV_RD_ADDR_BITLEN  0x0000003F
#define SPI_SLV_RD_ADDR_BITLEN_M  ((SPI_SLV_RD_ADDR_BITLEN_V)<<(SPI_SLV_RD_ADDR_BITLEN_S))
#define SPI_SLV_RD_ADDR_BITLEN_V  0x3F
#define SPI_SLV_RD_ADDR_BITLEN_S  10
/* SPI_SLV_WR_ADDR_BITLEN : R/W ;bitpos:[9:4] ;default: 6'h0 ; */
/*description: In the slave mode it is the address length in bits for write-buffer
 operation. The register value shall be (bit_num-1).*/
#define SPI_SLV_WR_ADDR_BITLEN  0x0000003F
#define SPI_SLV_WR_ADDR_BITLEN_M  ((SPI_SLV_WR_ADDR_BITLEN_V)<<(SPI_SLV_WR_ADDR_BITLEN_S))
#define SPI_SLV_WR_ADDR_BITLEN_V  0x3F
#define SPI_SLV_WR_ADDR_BITLEN_S  4
/* SPI_SLV_WRSTA_DUMMY_EN : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: In the slave mode it is the enable bit of dummy phase for write-status
 operations.*/
#define SPI_SLV_WRSTA_DUMMY_EN  (BIT(3))
#define SPI_SLV_WRSTA_DUMMY_EN_M  (BIT(3))
#define SPI_SLV_WRSTA_DUMMY_EN_V  0x1
#define SPI_SLV_WRSTA_DUMMY_EN_S  3
/* SPI_SLV_RDSTA_DUMMY_EN : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: In the slave mode it is the enable bit of dummy phase for read-status
 operations.*/
#define SPI_SLV_RDSTA_DUMMY_EN  (BIT(2))
#define SPI_SLV_RDSTA_DUMMY_EN_M  (BIT(2))
#define SPI_SLV_RDSTA_DUMMY_EN_V  0x1
#define SPI_SLV_RDSTA_DUMMY_EN_S  2
/* SPI_SLV_WRBUF_DUMMY_EN : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: In the slave mode it is the enable bit of dummy phase for write-buffer
 operations.*/
#define SPI_SLV_WRBUF_DUMMY_EN  (BIT(1))
#define SPI_SLV_WRBUF_DUMMY_EN_M  (BIT(1))
#define SPI_SLV_WRBUF_DUMMY_EN_V  0x1
#define SPI_SLV_WRBUF_DUMMY_EN_S  1
/* SPI_SLV_RDBUF_DUMMY_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: In the slave mode it is the enable bit of dummy phase for read-buffer
 operations.*/
#define SPI_SLV_RDBUF_DUMMY_EN  (BIT(0))
#define SPI_SLV_RDBUF_DUMMY_EN_M  (BIT(0))
#define SPI_SLV_RDBUF_DUMMY_EN_V  0x1
#define SPI_SLV_RDBUF_DUMMY_EN_S  0

#define SPI_SLAVE2_REG(i)          (REG_SPI_BASE(i) + 0x40)
/* SPI_SLV_WRBUF_DUMMY_CYCLELEN : R/W ;bitpos:[31:24] ;default: 8'b0 ; */
/*description: In the slave mode it is the length in spi_clk cycles of dummy
 phase for write-buffer operations. The register value shall be (cycle_num-1).*/
#define SPI_SLV_WRBUF_DUMMY_CYCLELEN  0x000000FF
#define SPI_SLV_WRBUF_DUMMY_CYCLELEN_M  ((SPI_SLV_WRBUF_DUMMY_CYCLELEN_V)<<(SPI_SLV_WRBUF_DUMMY_CYCLELEN_S))
#define SPI_SLV_WRBUF_DUMMY_CYCLELEN_V  0xFF
#define SPI_SLV_WRBUF_DUMMY_CYCLELEN_S  24
/* SPI_SLV_RDBUF_DUMMY_CYCLELEN : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: In the slave mode it is the length in spi_clk cycles of dummy
 phase for read-buffer operations. The register value shall be (cycle_num-1).*/
#define SPI_SLV_RDBUF_DUMMY_CYCLELEN  0x000000FF
#define SPI_SLV_RDBUF_DUMMY_CYCLELEN_M  ((SPI_SLV_RDBUF_DUMMY_CYCLELEN_V)<<(SPI_SLV_RDBUF_DUMMY_CYCLELEN_S))
#define SPI_SLV_RDBUF_DUMMY_CYCLELEN_V  0xFF
#define SPI_SLV_RDBUF_DUMMY_CYCLELEN_S  16
/* SPI_SLV_WRSTA_DUMMY_CYCLELEN : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: In the slave mode it is the length in spi_clk cycles of dummy
 phase for write-status operations. The register value shall be (cycle_num-1).*/
#define SPI_SLV_WRSTA_DUMMY_CYCLELEN  0x000000FF
#define SPI_SLV_WRSTA_DUMMY_CYCLELEN_M  ((SPI_SLV_WRSTA_DUMMY_CYCLELEN_V)<<(SPI_SLV_WRSTA_DUMMY_CYCLELEN_S))
#define SPI_SLV_WRSTA_DUMMY_CYCLELEN_V  0xFF
#define SPI_SLV_WRSTA_DUMMY_CYCLELEN_S  8
/* SPI_SLV_RDSTA_DUMMY_CYCLELEN : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: In the slave mode it is the length in spi_clk cycles of dummy
 phase for read-status operations. The register value shall be (cycle_num-1).*/
#define SPI_SLV_RDSTA_DUMMY_CYCLELEN  0x000000FF
#define SPI_SLV_RDSTA_DUMMY_CYCLELEN_M  ((SPI_SLV_RDSTA_DUMMY_CYCLELEN_V)<<(SPI_SLV_RDSTA_DUMMY_CYCLELEN_S))
#define SPI_SLV_RDSTA_DUMMY_CYCLELEN_V  0xFF
#define SPI_SLV_RDSTA_DUMMY_CYCLELEN_S  0

#define SPI_SLAVE3_REG(i)          (REG_SPI_BASE(i) + 0x44)
/* SPI_SLV_WRSTA_CMD_VALUE : R/W ;bitpos:[31:24] ;default: 8'b0 ; */
/*description: In the slave mode it is the value of write-status command.*/
#define SPI_SLV_WRSTA_CMD_VALUE  0x000000FF
#define SPI_SLV_WRSTA_CMD_VALUE_M  ((SPI_SLV_WRSTA_CMD_VALUE_V)<<(SPI_SLV_WRSTA_CMD_VALUE_S))
#define SPI_SLV_WRSTA_CMD_VALUE_V  0xFF
#define SPI_SLV_WRSTA_CMD_VALUE_S  24
/* SPI_SLV_RDSTA_CMD_VALUE : R/W ;bitpos:[23:16] ;default: 8'b0 ; */
/*description: In the slave mode it is the value of read-status command.*/
#define SPI_SLV_RDSTA_CMD_VALUE  0x000000FF
#define SPI_SLV_RDSTA_CMD_VALUE_M  ((SPI_SLV_RDSTA_CMD_VALUE_V)<<(SPI_SLV_RDSTA_CMD_VALUE_S))
#define SPI_SLV_RDSTA_CMD_VALUE_V  0xFF
#define SPI_SLV_RDSTA_CMD_VALUE_S  16
/* SPI_SLV_WRBUF_CMD_VALUE : R/W ;bitpos:[15:8] ;default: 8'b0 ; */
/*description: In the slave mode it is the value of write-buffer command.*/
#define SPI_SLV_WRBUF_CMD_VALUE  0x000000FF
#define SPI_SLV_WRBUF_CMD_VALUE_M  ((SPI_SLV_WRBUF_CMD_VALUE_V)<<(SPI_SLV_WRBUF_CMD_VALUE_S))
#define SPI_SLV_WRBUF_CMD_VALUE_V  0xFF
#define SPI_SLV_WRBUF_CMD_VALUE_S  8
/* SPI_SLV_RDBUF_CMD_VALUE : R/W ;bitpos:[7:0] ;default: 8'b0 ; */
/*description: In the slave mode it is the value of read-buffer command.*/
#define SPI_SLV_RDBUF_CMD_VALUE  0x000000FF
#define SPI_SLV_RDBUF_CMD_VALUE_M  ((SPI_SLV_RDBUF_CMD_VALUE_V)<<(SPI_SLV_RDBUF_CMD_VALUE_S))
#define SPI_SLV_RDBUF_CMD_VALUE_V  0xFF
#define SPI_SLV_RDBUF_CMD_VALUE_S  0

#define SPI_SLV_WRBUF_DLEN_REG(i)          (REG_SPI_BASE(i) + 0x48)
/* SPI_SLV_WRBUF_DBITLEN : R/W ;bitpos:[23:0] ;default: 24'h0 ; */
/*description: In the slave mode it is the length in bits for write-buffer operations.
 The register value shall be (bit_num-1).*/
#define SPI_SLV_WRBUF_DBITLEN  0x00FFFFFF
#define SPI_SLV_WRBUF_DBITLEN_M  ((SPI_SLV_WRBUF_DBITLEN_V)<<(SPI_SLV_WRBUF_DBITLEN_S))
#define SPI_SLV_WRBUF_DBITLEN_V  0xFFFFFF
#define SPI_SLV_WRBUF_DBITLEN_S  0

#define SPI_SLV_RDBUF_DLEN_REG(i)          (REG_SPI_BASE(i) + 0x4C)
/* SPI_SLV_RDBUF_DBITLEN : R/W ;bitpos:[23:0] ;default: 24'h0 ; */
/*description: In the slave mode it is the length in bits for read-buffer operations.
 The register value shall be (bit_num-1).*/
#define SPI_SLV_RDBUF_DBITLEN  0x00FFFFFF
#define SPI_SLV_RDBUF_DBITLEN_M  ((SPI_SLV_RDBUF_DBITLEN_V)<<(SPI_SLV_RDBUF_DBITLEN_S))
#define SPI_SLV_RDBUF_DBITLEN_V  0xFFFFFF
#define SPI_SLV_RDBUF_DBITLEN_S  0

#define SPI_CACHE_FCTRL_REG(i)          (REG_SPI_BASE(i) + 0x50)
/* SPI_CACHE_FLASH_PES_EN : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: For SPI0  spi1 send suspend command before cache read flash 
 1: enable  0:disable.*/
#define SPI_CACHE_FLASH_PES_EN  (BIT(3))
#define SPI_CACHE_FLASH_PES_EN_M  (BIT(3))
#define SPI_CACHE_FLASH_PES_EN_V  0x1
#define SPI_CACHE_FLASH_PES_EN_S  3
/* SPI_CACHE_FLASH_USR_CMD : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: For SPI0  cache  read flash for user define command  1: enable  0:disable.*/
#define SPI_CACHE_FLASH_USR_CMD  (BIT(2))
#define SPI_CACHE_FLASH_USR_CMD_M  (BIT(2))
#define SPI_CACHE_FLASH_USR_CMD_V  0x1
#define SPI_CACHE_FLASH_USR_CMD_S  2
/* SPI_CACHE_USR_CMD_4BYTE : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: For SPI0  cache  read flash with 4 bytes command  1: enable  0:disable.*/
#define SPI_CACHE_USR_CMD_4BYTE  (BIT(1))
#define SPI_CACHE_USR_CMD_4BYTE_M  (BIT(1))
#define SPI_CACHE_USR_CMD_4BYTE_V  0x1
#define SPI_CACHE_USR_CMD_4BYTE_S  1
/* SPI_CACHE_REQ_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: For SPI0  Cache access enable  1: enable  0:disable.*/
#define SPI_CACHE_REQ_EN  (BIT(0))
#define SPI_CACHE_REQ_EN_M  (BIT(0))
#define SPI_CACHE_REQ_EN_V  0x1
#define SPI_CACHE_REQ_EN_S  0

#define SPI_CACHE_SCTRL_REG(i)          (REG_SPI_BASE(i) + 0x54)
/* SPI_CACHE_SRAM_USR_WCMD : R/W ;bitpos:[28] ;default: 1'b1 ; */
/*description: For SPI0  In the spi sram mode cache write sram for user define command*/
#define SPI_CACHE_SRAM_USR_WCMD  (BIT(28))
#define SPI_CACHE_SRAM_USR_WCMD_M  (BIT(28))
#define SPI_CACHE_SRAM_USR_WCMD_V  0x1
#define SPI_CACHE_SRAM_USR_WCMD_S  28
/* SPI_SRAM_ADDR_BITLEN : R/W ;bitpos:[27:22] ;default: 6'd23 ; */
/*description: For SPI0  In the sram mode  it is the length in bits of address
 phase. The register value shall be (bit_num-1).*/
#define SPI_SRAM_ADDR_BITLEN  0x0000003F
#define SPI_SRAM_ADDR_BITLEN_M  ((SPI_SRAM_ADDR_BITLEN_V)<<(SPI_SRAM_ADDR_BITLEN_S))
#define SPI_SRAM_ADDR_BITLEN_V  0x3F
#define SPI_SRAM_ADDR_BITLEN_S  22
/* SPI_SRAM_DUMMY_CYCLELEN : R/W ;bitpos:[21:14] ;default: 8'b1 ; */
/*description: For SPI0  In the sram mode  it is the length in bits of address
 phase. The register value shall be (bit_num-1).*/
#define SPI_SRAM_DUMMY_CYCLELEN  0x000000FF
#define SPI_SRAM_DUMMY_CYCLELEN_M  ((SPI_SRAM_DUMMY_CYCLELEN_V)<<(SPI_SRAM_DUMMY_CYCLELEN_S))
#define SPI_SRAM_DUMMY_CYCLELEN_V  0xFF
#define SPI_SRAM_DUMMY_CYCLELEN_S  14
/* SPI_SRAM_BYTES_LEN : R/W ;bitpos:[13:6] ;default: 8'b32 ; */
/*description: For SPI0  In the sram mode  it is the byte length of spi read sram data.*/
#define SPI_SRAM_BYTES_LEN  0x000000FF
#define SPI_SRAM_BYTES_LEN_M  ((SPI_SRAM_BYTES_LEN_V)<<(SPI_SRAM_BYTES_LEN_S))
#define SPI_SRAM_BYTES_LEN_V  0xFF
#define SPI_SRAM_BYTES_LEN_S  6
/* SPI_CACHE_SRAM_USR_RCMD : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: For SPI0  In the spi sram mode cache read sram for user define command.*/
#define SPI_CACHE_SRAM_USR_RCMD  (BIT(5))
#define SPI_CACHE_SRAM_USR_RCMD_M  (BIT(5))
#define SPI_CACHE_SRAM_USR_RCMD_V  0x1
#define SPI_CACHE_SRAM_USR_RCMD_S  5
/* SPI_USR_RD_SRAM_DUMMY : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: For SPI0  In the spi sram mode  it is the enable bit of dummy
 phase for read operations.*/
#define SPI_USR_RD_SRAM_DUMMY  (BIT(4))
#define SPI_USR_RD_SRAM_DUMMY_M  (BIT(4))
#define SPI_USR_RD_SRAM_DUMMY_V  0x1
#define SPI_USR_RD_SRAM_DUMMY_S  4
/* SPI_USR_WR_SRAM_DUMMY : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: For SPI0  In the spi sram mode  it is the enable bit of dummy
 phase for write operations.*/
#define SPI_USR_WR_SRAM_DUMMY  (BIT(3))
#define SPI_USR_WR_SRAM_DUMMY_M  (BIT(3))
#define SPI_USR_WR_SRAM_DUMMY_V  0x1
#define SPI_USR_WR_SRAM_DUMMY_S  3
/* SPI_USR_SRAM_QIO : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: For SPI0  In the spi sram mode  spi quad I/O mode enable  1: enable  0:disable*/
#define SPI_USR_SRAM_QIO  (BIT(2))
#define SPI_USR_SRAM_QIO_M  (BIT(2))
#define SPI_USR_SRAM_QIO_V  0x1
#define SPI_USR_SRAM_QIO_S  2
/* SPI_USR_SRAM_DIO : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: For SPI0  In the spi sram mode  spi dual I/O mode enable  1: enable  0:disable*/
#define SPI_USR_SRAM_DIO  (BIT(1))
#define SPI_USR_SRAM_DIO_M  (BIT(1))
#define SPI_USR_SRAM_DIO_V  0x1
#define SPI_USR_SRAM_DIO_S  1

#define SPI_SRAM_CMD_REG(i)          (REG_SPI_BASE(i) + 0x58)
/* SPI_SRAM_RSTIO : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: For SPI0 SRAM IO mode reset enable. SRAM IO mode reset operation
 will be triggered when the bit is set. The bit will be cleared once the operation done*/
#define SPI_SRAM_RSTIO  (BIT(4))
#define SPI_SRAM_RSTIO_M  (BIT(4))
#define SPI_SRAM_RSTIO_V  0x1
#define SPI_SRAM_RSTIO_S  4
/* SPI_SRAM_QIO : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: For SPI0 SRAM QIO mode enable .  SRAM QIO enable command will
 be send when the bit is set. The bit will be cleared once the operation done.*/
#define SPI_SRAM_QIO  (BIT(1))
#define SPI_SRAM_QIO_M  (BIT(1))
#define SPI_SRAM_QIO_V  0x1
#define SPI_SRAM_QIO_S  1
/* SPI_SRAM_DIO : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: For SPI0 SRAM DIO mode enable .  SRAM DIO enable command will
 be send when the bit is set. The bit will be cleared once the operation done.*/
#define SPI_SRAM_DIO  (BIT(0))
#define SPI_SRAM_DIO_M  (BIT(0))
#define SPI_SRAM_DIO_V  0x1
#define SPI_SRAM_DIO_S  0

#define SPI_SRAM_DRD_CMD_REG(i)          (REG_SPI_BASE(i) + 0x5C)
/* SPI_CACHE_SRAM_USR_RD_CMD_BITLEN : R/W ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: For SPI0 When cache mode is enable it is the length in bits of
 command phase for SRAM. The register value shall be (bit_num-1).*/
#define SPI_CACHE_SRAM_USR_RD_CMD_BITLEN  0x0000000F
#define SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_M  ((SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_V)<<(SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_S))
#define SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_V  0xF
#define SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_S  28
/* SPI_CACHE_SRAM_USR_RD_CMD_VALUE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: For SPI0 When cache mode is enable it is the read command value
 of command phase for SRAM.*/
#define SPI_CACHE_SRAM_USR_RD_CMD_VALUE  0x0000FFFF
#define SPI_CACHE_SRAM_USR_RD_CMD_VALUE_M  ((SPI_CACHE_SRAM_USR_RD_CMD_VALUE_V)<<(SPI_CACHE_SRAM_USR_RD_CMD_VALUE_S))
#define SPI_CACHE_SRAM_USR_RD_CMD_VALUE_V  0xFFFF
#define SPI_CACHE_SRAM_USR_RD_CMD_VALUE_S  0

#define SPI_SRAM_DWR_CMD_REG(i)          (REG_SPI_BASE(i) + 0x60)
/* SPI_CACHE_SRAM_USR_WR_CMD_BITLEN : R/W ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: For SPI0 When cache mode is enable it is the in bits of command
 phase  for SRAM. The register value shall be (bit_num-1).*/
#define SPI_CACHE_SRAM_USR_WR_CMD_BITLEN  0x0000000F
#define SPI_CACHE_SRAM_USR_WR_CMD_BITLEN_M  ((SPI_CACHE_SRAM_USR_WR_CMD_BITLEN_V)<<(SPI_CACHE_SRAM_USR_WR_CMD_BITLEN_S))
#define SPI_CACHE_SRAM_USR_WR_CMD_BITLEN_V  0xF
#define SPI_CACHE_SRAM_USR_WR_CMD_BITLEN_S  28
/* SPI_CACHE_SRAM_USR_WR_CMD_VALUE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: For SPI0 When cache mode is enable it is the write command value
 of command phase for SRAM.*/
#define SPI_CACHE_SRAM_USR_WR_CMD_VALUE  0x0000FFFF
#define SPI_CACHE_SRAM_USR_WR_CMD_VALUE_M  ((SPI_CACHE_SRAM_USR_WR_CMD_VALUE_V)<<(SPI_CACHE_SRAM_USR_WR_CMD_VALUE_S))
#define SPI_CACHE_SRAM_USR_WR_CMD_VALUE_V  0xFFFF
#define SPI_CACHE_SRAM_USR_WR_CMD_VALUE_S  0

#define SPI_SLV_RD_BIT_REG(i)          (REG_SPI_BASE(i) + 0x64)
/* SPI_SLV_RDATA_BIT : RW ;bitpos:[23:0] ;default: 24'b0 ; */
/*description: In the slave mode it is the bit length of read data. The value
 is the length - 1.*/
#define SPI_SLV_RDATA_BIT  0x00FFFFFF
#define SPI_SLV_RDATA_BIT_M  ((SPI_SLV_RDATA_BIT_V)<<(SPI_SLV_RDATA_BIT_S))
#define SPI_SLV_RDATA_BIT_V  0xFFFFFF
#define SPI_SLV_RDATA_BIT_S  0

#define SPI_W0_REG(i)          (REG_SPI_BASE(i) + 0x80)
/* SPI_BUF0 : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_BUF0  0xFFFFFFFF
#define SPI_BUF0_M  ((SPI_BUF0_V)<<(SPI_BUF0_S))
#define SPI_BUF0_V  0xFFFFFFFF
#define SPI_BUF0_S  0

#define SPI_W1_REG(i)          (REG_SPI_BASE(i) + 0x84)
/* SPI_BUF1 : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_BUF1  0xFFFFFFFF
#define SPI_BUF1_M  ((SPI_BUF1_V)<<(SPI_BUF1_S))
#define SPI_BUF1_V  0xFFFFFFFF
#define SPI_BUF1_S  0

#define SPI_W2_REG(i)          (REG_SPI_BASE(i) + 0x88)
/* SPI_BUF2 : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_BUF2  0xFFFFFFFF
#define SPI_BUF2_M  ((SPI_BUF2_V)<<(SPI_BUF2_S))
#define SPI_BUF2_V  0xFFFFFFFF
#define SPI_BUF2_S  0

#define SPI_W3_REG(i)          (REG_SPI_BASE(i) + 0x8C)
/* SPI_BUF3 : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_BUF3  0xFFFFFFFF
#define SPI_BUF3_M  ((SPI_BUF3_V)<<(SPI_BUF3_S))
#define SPI_BUF3_V  0xFFFFFFFF
#define SPI_BUF3_S  0

#define SPI_W4_REG(i)          (REG_SPI_BASE(i) + 0x90)
/* SPI_BUF4 : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_BUF4  0xFFFFFFFF
#define SPI_BUF4_M  ((SPI_BUF4_V)<<(SPI_BUF4_S))
#define SPI_BUF4_V  0xFFFFFFFF
#define SPI_BUF4_S  0

#define SPI_W5_REG(i)          (REG_SPI_BASE(i) + 0x94)
/* SPI_BUF5 : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_BUF5  0xFFFFFFFF
#define SPI_BUF5_M  ((SPI_BUF5_V)<<(SPI_BUF5_S))
#define SPI_BUF5_V  0xFFFFFFFF
#define SPI_BUF5_S  0

#define SPI_W6_REG(i)          (REG_SPI_BASE(i) + 0x98)
/* SPI_BUF6 : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_BUF6  0xFFFFFFFF
#define SPI_BUF6_M  ((SPI_BUF6_V)<<(SPI_BUF6_S))
#define SPI_BUF6_V  0xFFFFFFFF
#define SPI_BUF6_S  0

#define SPI_W7_REG(i)          (REG_SPI_BASE(i) + 0x9C)
/* SPI_BUF7 : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_BUF7  0xFFFFFFFF
#define SPI_BUF7_M  ((SPI_BUF7_V)<<(SPI_BUF7_S))
#define SPI_BUF7_V  0xFFFFFFFF
#define SPI_BUF7_S  0

#define SPI_W8_REG(i)          (REG_SPI_BASE(i) + 0xA0)
/* SPI_BUF8 : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_BUF8  0xFFFFFFFF
#define SPI_BUF8_M  ((SPI_BUF8_V)<<(SPI_BUF8_S))
#define SPI_BUF8_V  0xFFFFFFFF
#define SPI_BUF8_S  0

#define SPI_W9_REG(i)          (REG_SPI_BASE(i) + 0xA4)
/* SPI_BUF9 : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_BUF9  0xFFFFFFFF
#define SPI_BUF9_M  ((SPI_BUF9_V)<<(SPI_BUF9_S))
#define SPI_BUF9_V  0xFFFFFFFF
#define SPI_BUF9_S  0

#define SPI_W10_REG(i)          (REG_SPI_BASE(i) + 0xA8)
/* SPI_BUF10 : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_BUF10  0xFFFFFFFF
#define SPI_BUF10_M  ((SPI_BUF10_V)<<(SPI_BUF10_S))
#define SPI_BUF10_V  0xFFFFFFFF
#define SPI_BUF10_S  0

#define SPI_W11_REG(i)          (REG_SPI_BASE(i) + 0xAC)
/* SPI_BUF11 : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_BUF11  0xFFFFFFFF
#define SPI_BUF11_M  ((SPI_BUF11_V)<<(SPI_BUF11_S))
#define SPI_BUF11_V  0xFFFFFFFF
#define SPI_BUF11_S  0

#define SPI_W12_REG(i)          (REG_SPI_BASE(i) + 0xB0)
/* SPI_BUF12 : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_BUF12  0xFFFFFFFF
#define SPI_BUF12_M  ((SPI_BUF12_V)<<(SPI_BUF12_S))
#define SPI_BUF12_V  0xFFFFFFFF
#define SPI_BUF12_S  0

#define SPI_W13_REG(i)          (REG_SPI_BASE(i) + 0xB4)
/* SPI_BUF13 : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_BUF13  0xFFFFFFFF
#define SPI_BUF13_M  ((SPI_BUF13_V)<<(SPI_BUF13_S))
#define SPI_BUF13_V  0xFFFFFFFF
#define SPI_BUF13_S  0

#define SPI_W14_REG(i)          (REG_SPI_BASE(i) + 0xB8)
/* SPI_BUF14 : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_BUF14  0xFFFFFFFF
#define SPI_BUF14_M  ((SPI_BUF14_V)<<(SPI_BUF14_S))
#define SPI_BUF14_V  0xFFFFFFFF
#define SPI_BUF14_S  0

#define SPI_W15_REG(i)          (REG_SPI_BASE(i) + 0xBC)
/* SPI_BUF15 : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_BUF15  0xFFFFFFFF
#define SPI_BUF15_M  ((SPI_BUF15_V)<<(SPI_BUF15_S))
#define SPI_BUF15_V  0xFFFFFFFF
#define SPI_BUF15_S  0

#define SPI_TX_CRC_REG(i)          (REG_SPI_BASE(i) + 0xC0)
/* SPI_TX_CRC_DATA : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: For SPI1  the value of crc32 for 256 bits data.*/
#define SPI_TX_CRC_DATA  0xFFFFFFFF
#define SPI_TX_CRC_DATA_M  ((SPI_TX_CRC_DATA_V)<<(SPI_TX_CRC_DATA_S))
#define SPI_TX_CRC_DATA_V  0xFFFFFFFF
#define SPI_TX_CRC_DATA_S  0

#define SPI_EXT0_REG(i)          (REG_SPI_BASE(i) + 0xF0)
/* SPI_T_PP_ENA : R/W ;bitpos:[31] ;default: 1'b1 ; */
/*description: page program delay enable.*/
#define SPI_T_PP_ENA  (BIT(31))
#define SPI_T_PP_ENA_M  (BIT(31))
#define SPI_T_PP_ENA_V  0x1
#define SPI_T_PP_ENA_S  31
/* SPI_T_PP_SHIFT : R/W ;bitpos:[19:16] ;default: 4'd10 ; */
/*description: page program delay time shift .*/
#define SPI_T_PP_SHIFT  0x0000000F
#define SPI_T_PP_SHIFT_M  ((SPI_T_PP_SHIFT_V)<<(SPI_T_PP_SHIFT_S))
#define SPI_T_PP_SHIFT_V  0xF
#define SPI_T_PP_SHIFT_S  16
/* SPI_T_PP_TIME : R/W ;bitpos:[11:0] ;default: 12'd80 ; */
/*description: page program delay time  by system clock.*/
#define SPI_T_PP_TIME  0x00000FFF
#define SPI_T_PP_TIME_M  ((SPI_T_PP_TIME_V)<<(SPI_T_PP_TIME_S))
#define SPI_T_PP_TIME_V  0xFFF
#define SPI_T_PP_TIME_S  0

#define SPI_EXT1_REG(i)          (REG_SPI_BASE(i) + 0xF4)
/* SPI_T_ERASE_ENA : R/W ;bitpos:[31] ;default: 1'b1 ; */
/*description: erase flash delay enable.*/
#define SPI_T_ERASE_ENA  (BIT(31))
#define SPI_T_ERASE_ENA_M  (BIT(31))
#define SPI_T_ERASE_ENA_V  0x1
#define SPI_T_ERASE_ENA_S  31
/* SPI_T_ERASE_SHIFT : R/W ;bitpos:[19:16] ;default: 4'd15 ; */
/*description: erase flash delay time shift.*/
#define SPI_T_ERASE_SHIFT  0x0000000F
#define SPI_T_ERASE_SHIFT_M  ((SPI_T_ERASE_SHIFT_V)<<(SPI_T_ERASE_SHIFT_S))
#define SPI_T_ERASE_SHIFT_V  0xF
#define SPI_T_ERASE_SHIFT_S  16
/* SPI_T_ERASE_TIME : R/W ;bitpos:[11:0] ;default: 12'd0 ; */
/*description: erase flash delay time by system clock.*/
#define SPI_T_ERASE_TIME  0x00000FFF
#define SPI_T_ERASE_TIME_M  ((SPI_T_ERASE_TIME_V)<<(SPI_T_ERASE_TIME_S))
#define SPI_T_ERASE_TIME_V  0xFFF
#define SPI_T_ERASE_TIME_S  0

#define SPI_EXT2_REG(i)          (REG_SPI_BASE(i) + 0xF8)
/* SPI_ST : RO ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: The status of spi state machine .*/
#define SPI_ST  0x00000007
#define SPI_ST_M  ((SPI_ST_V)<<(SPI_ST_S))
#define SPI_ST_V  0x7
#define SPI_ST_S  0

#define SPI_EXT3_REG(i)          (REG_SPI_BASE(i) + 0xFC)
/* SPI_INT_HOLD_ENA : R/W ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: This register is for two SPI masters to share the same cs clock
 and data signals. The bits of one SPI are set  if the other SPI is busy  the SPI will be hold. 1(3): hold at ¡°idle¡± phase 2: hold at ¡°prepare¡± phase.*/
#define SPI_INT_HOLD_ENA  0x00000003
#define SPI_INT_HOLD_ENA_M  ((SPI_INT_HOLD_ENA_V)<<(SPI_INT_HOLD_ENA_S))
#define SPI_INT_HOLD_ENA_V  0x3
#define SPI_INT_HOLD_ENA_S  0

#define SPI_DMA_CONF_REG(i)          (REG_SPI_BASE(i) + 0x100)
/* SPI_DMA_CONTINUE : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: spi dma continue tx/rx data.*/
#define SPI_DMA_CONTINUE  (BIT(16))
#define SPI_DMA_CONTINUE_M  (BIT(16))
#define SPI_DMA_CONTINUE_V  0x1
#define SPI_DMA_CONTINUE_S  16
/* SPI_DMA_TX_STOP : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: spi dma write data stop when in continue tx/rx mode.*/
#define SPI_DMA_TX_STOP  (BIT(15))
#define SPI_DMA_TX_STOP_M  (BIT(15))
#define SPI_DMA_TX_STOP_V  0x1
#define SPI_DMA_TX_STOP_S  15
/* SPI_DMA_RX_STOP : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: spi dma read data stop  when in continue tx/rx mode.*/
#define SPI_DMA_RX_STOP  (BIT(14))
#define SPI_DMA_RX_STOP_M  (BIT(14))
#define SPI_DMA_RX_STOP_V  0x1
#define SPI_DMA_RX_STOP_S  14
/* SPI_OUT_DATA_BURST_EN : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: spi dma read data from memory in burst mode.*/
#define SPI_OUT_DATA_BURST_EN  (BIT(12))
#define SPI_OUT_DATA_BURST_EN_M  (BIT(12))
#define SPI_OUT_DATA_BURST_EN_V  0x1
#define SPI_OUT_DATA_BURST_EN_S  12
/* SPI_INDSCR_BURST_EN : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: read descriptor use burst mode when write data to memory.*/
#define SPI_INDSCR_BURST_EN  (BIT(11))
#define SPI_INDSCR_BURST_EN_M  (BIT(11))
#define SPI_INDSCR_BURST_EN_V  0x1
#define SPI_INDSCR_BURST_EN_S  11
/* SPI_OUTDSCR_BURST_EN : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: read descriptor use burst mode when read data for memory.*/
#define SPI_OUTDSCR_BURST_EN  (BIT(10))
#define SPI_OUTDSCR_BURST_EN_M  (BIT(10))
#define SPI_OUTDSCR_BURST_EN_V  0x1
#define SPI_OUTDSCR_BURST_EN_S  10
/* SPI_OUT_EOF_MODE : R/W ;bitpos:[9] ;default: 1'b1 ; */
/*description: out eof flag generation mode . 1: when dma pop all data from
 fifo  0:when ahb push all data to fifo.*/
#define SPI_OUT_EOF_MODE  (BIT(9))
#define SPI_OUT_EOF_MODE_M  (BIT(9))
#define SPI_OUT_EOF_MODE_V  0x1
#define SPI_OUT_EOF_MODE_S  9
/* SPI_OUT_AUTO_WRBACK : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: when the link is empty   jump to next automatically.*/
#define SPI_OUT_AUTO_WRBACK  (BIT(8))
#define SPI_OUT_AUTO_WRBACK_M  (BIT(8))
#define SPI_OUT_AUTO_WRBACK_V  0x1
#define SPI_OUT_AUTO_WRBACK_S  8
/* SPI_OUT_LOOP_TEST : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set bit to test out link.*/
#define SPI_OUT_LOOP_TEST  (BIT(7))
#define SPI_OUT_LOOP_TEST_M  (BIT(7))
#define SPI_OUT_LOOP_TEST_V  0x1
#define SPI_OUT_LOOP_TEST_S  7
/* SPI_IN_LOOP_TEST : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set bit to test in link.*/
#define SPI_IN_LOOP_TEST  (BIT(6))
#define SPI_IN_LOOP_TEST_M  (BIT(6))
#define SPI_IN_LOOP_TEST_V  0x1
#define SPI_IN_LOOP_TEST_S  6
/* SPI_AHBM_RST : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: reset spi dma ahb master.*/
#define SPI_AHBM_RST  (BIT(5))
#define SPI_AHBM_RST_M  (BIT(5))
#define SPI_AHBM_RST_V  0x1
#define SPI_AHBM_RST_S  5
/* SPI_AHBM_FIFO_RST : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: reset spi dma ahb master fifo pointer.*/
#define SPI_AHBM_FIFO_RST  (BIT(4))
#define SPI_AHBM_FIFO_RST_M  (BIT(4))
#define SPI_AHBM_FIFO_RST_V  0x1
#define SPI_AHBM_FIFO_RST_S  4
/* SPI_OUT_RST : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to reset out dma fsm and out data fifo pointer.*/
#define SPI_OUT_RST  (BIT(3))
#define SPI_OUT_RST_M  (BIT(3))
#define SPI_OUT_RST_V  0x1
#define SPI_OUT_RST_S  3
/* SPI_IN_RST : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The bit is used to reset in dma fsm and in data fifo pointer.*/
#define SPI_IN_RST  (BIT(2))
#define SPI_IN_RST_M  (BIT(2))
#define SPI_IN_RST_V  0x1
#define SPI_IN_RST_S  2

#define SPI_DMA_OUT_LINK_REG(i)          (REG_SPI_BASE(i) + 0x104)
/* SPI_OUTLINK_RESTART : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: Set the bit to mount on new outlink descriptors.*/
#define SPI_OUTLINK_RESTART  (BIT(30))
#define SPI_OUTLINK_RESTART_M  (BIT(30))
#define SPI_OUTLINK_RESTART_V  0x1
#define SPI_OUTLINK_RESTART_S  30
/* SPI_OUTLINK_START : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: Set the bit to start to use outlink descriptor.*/
#define SPI_OUTLINK_START  (BIT(29))
#define SPI_OUTLINK_START_M  (BIT(29))
#define SPI_OUTLINK_START_V  0x1
#define SPI_OUTLINK_START_S  29
/* SPI_OUTLINK_STOP : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: Set the bit to stop to use outlink descriptor.*/
#define SPI_OUTLINK_STOP  (BIT(28))
#define SPI_OUTLINK_STOP_M  (BIT(28))
#define SPI_OUTLINK_STOP_V  0x1
#define SPI_OUTLINK_STOP_S  28
/* SPI_OUTLINK_ADDR : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: The address of the first outlink descriptor.*/
#define SPI_OUTLINK_ADDR  0x000FFFFF
#define SPI_OUTLINK_ADDR_M  ((SPI_OUTLINK_ADDR_V)<<(SPI_OUTLINK_ADDR_S))
#define SPI_OUTLINK_ADDR_V  0xFFFFF
#define SPI_OUTLINK_ADDR_S  0

#define SPI_DMA_IN_LINK_REG(i)          (REG_SPI_BASE(i) + 0x108)
/* SPI_INLINK_RESTART : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: Set the bit to mount on new inlink descriptors.*/
#define SPI_INLINK_RESTART  (BIT(30))
#define SPI_INLINK_RESTART_M  (BIT(30))
#define SPI_INLINK_RESTART_V  0x1
#define SPI_INLINK_RESTART_S  30
/* SPI_INLINK_START : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: Set the bit to start to use inlink descriptor.*/
#define SPI_INLINK_START  (BIT(29))
#define SPI_INLINK_START_M  (BIT(29))
#define SPI_INLINK_START_V  0x1
#define SPI_INLINK_START_S  29
/* SPI_INLINK_STOP : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: Set the bit to stop to use inlink descriptor.*/
#define SPI_INLINK_STOP  (BIT(28))
#define SPI_INLINK_STOP_M  (BIT(28))
#define SPI_INLINK_STOP_V  0x1
#define SPI_INLINK_STOP_S  28
/* SPI_INLINK_AUTO_RET : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: when the bit is set  inlink descriptor returns to the next descriptor
 while a packet is wrong*/
#define SPI_INLINK_AUTO_RET  (BIT(20))
#define SPI_INLINK_AUTO_RET_M  (BIT(20))
#define SPI_INLINK_AUTO_RET_V  0x1
#define SPI_INLINK_AUTO_RET_S  20
/* SPI_INLINK_ADDR : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: The address of the first inlink descriptor.*/
#define SPI_INLINK_ADDR  0x000FFFFF
#define SPI_INLINK_ADDR_M  ((SPI_INLINK_ADDR_V)<<(SPI_INLINK_ADDR_S))
#define SPI_INLINK_ADDR_V  0xFFFFF
#define SPI_INLINK_ADDR_S  0

#define SPI_DMA_STATUS_REG(i)          (REG_SPI_BASE(i) + 0x10C)
/* SPI_DMA_TX_EN : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: spi dma write data status bit.*/
#define SPI_DMA_TX_EN  (BIT(1))
#define SPI_DMA_TX_EN_M  (BIT(1))
#define SPI_DMA_TX_EN_V  0x1
#define SPI_DMA_TX_EN_S  1
/* SPI_DMA_RX_EN : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: spi dma read data status bit.*/
#define SPI_DMA_RX_EN  (BIT(0))
#define SPI_DMA_RX_EN_M  (BIT(0))
#define SPI_DMA_RX_EN_V  0x1
#define SPI_DMA_RX_EN_S  0

#define SPI_DMA_INT_ENA_REG(i)          (REG_SPI_BASE(i) + 0x110)
/* SPI_OUT_TOTAL_EOF_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: The enable bit for sending all the packets to host done.*/
#define SPI_OUT_TOTAL_EOF_INT_ENA  (BIT(8))
#define SPI_OUT_TOTAL_EOF_INT_ENA_M  (BIT(8))
#define SPI_OUT_TOTAL_EOF_INT_ENA_V  0x1
#define SPI_OUT_TOTAL_EOF_INT_ENA_S  8
/* SPI_OUT_EOF_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The enable bit for sending a packet to host done.*/
#define SPI_OUT_EOF_INT_ENA  (BIT(7))
#define SPI_OUT_EOF_INT_ENA_M  (BIT(7))
#define SPI_OUT_EOF_INT_ENA_V  0x1
#define SPI_OUT_EOF_INT_ENA_S  7
/* SPI_OUT_DONE_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The enable bit for completing usage of a outlink descriptor .*/
#define SPI_OUT_DONE_INT_ENA  (BIT(6))
#define SPI_OUT_DONE_INT_ENA_M  (BIT(6))
#define SPI_OUT_DONE_INT_ENA_V  0x1
#define SPI_OUT_DONE_INT_ENA_S  6
/* SPI_IN_SUC_EOF_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The enable bit for completing receiving all the packets from host.*/
#define SPI_IN_SUC_EOF_INT_ENA  (BIT(5))
#define SPI_IN_SUC_EOF_INT_ENA_M  (BIT(5))
#define SPI_IN_SUC_EOF_INT_ENA_V  0x1
#define SPI_IN_SUC_EOF_INT_ENA_S  5
/* SPI_IN_ERR_EOF_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The enable bit for receiving error.*/
#define SPI_IN_ERR_EOF_INT_ENA  (BIT(4))
#define SPI_IN_ERR_EOF_INT_ENA_M  (BIT(4))
#define SPI_IN_ERR_EOF_INT_ENA_V  0x1
#define SPI_IN_ERR_EOF_INT_ENA_S  4
/* SPI_IN_DONE_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The enable bit for completing usage of a inlink descriptor.*/
#define SPI_IN_DONE_INT_ENA  (BIT(3))
#define SPI_IN_DONE_INT_ENA_M  (BIT(3))
#define SPI_IN_DONE_INT_ENA_V  0x1
#define SPI_IN_DONE_INT_ENA_S  3
/* SPI_INLINK_DSCR_ERROR_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The enable bit for inlink descriptor error.*/
#define SPI_INLINK_DSCR_ERROR_INT_ENA  (BIT(2))
#define SPI_INLINK_DSCR_ERROR_INT_ENA_M  (BIT(2))
#define SPI_INLINK_DSCR_ERROR_INT_ENA_V  0x1
#define SPI_INLINK_DSCR_ERROR_INT_ENA_S  2
/* SPI_OUTLINK_DSCR_ERROR_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The enable bit for outlink descriptor error.*/
#define SPI_OUTLINK_DSCR_ERROR_INT_ENA  (BIT(1))
#define SPI_OUTLINK_DSCR_ERROR_INT_ENA_M  (BIT(1))
#define SPI_OUTLINK_DSCR_ERROR_INT_ENA_V  0x1
#define SPI_OUTLINK_DSCR_ERROR_INT_ENA_S  1
/* SPI_INLINK_DSCR_EMPTY_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The enable bit for lack of enough inlink descriptors.*/
#define SPI_INLINK_DSCR_EMPTY_INT_ENA  (BIT(0))
#define SPI_INLINK_DSCR_EMPTY_INT_ENA_M  (BIT(0))
#define SPI_INLINK_DSCR_EMPTY_INT_ENA_V  0x1
#define SPI_INLINK_DSCR_EMPTY_INT_ENA_S  0

#define SPI_DMA_INT_RAW_REG(i)          (REG_SPI_BASE(i) + 0x114)
/* SPI_OUT_TOTAL_EOF_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw bit for sending all the packets to host done.*/
#define SPI_OUT_TOTAL_EOF_INT_RAW  (BIT(8))
#define SPI_OUT_TOTAL_EOF_INT_RAW_M  (BIT(8))
#define SPI_OUT_TOTAL_EOF_INT_RAW_V  0x1
#define SPI_OUT_TOTAL_EOF_INT_RAW_S  8
/* SPI_OUT_EOF_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw bit for sending a packet to host done.*/
#define SPI_OUT_EOF_INT_RAW  (BIT(7))
#define SPI_OUT_EOF_INT_RAW_M  (BIT(7))
#define SPI_OUT_EOF_INT_RAW_V  0x1
#define SPI_OUT_EOF_INT_RAW_S  7
/* SPI_OUT_DONE_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw bit for completing usage of a outlink descriptor.*/
#define SPI_OUT_DONE_INT_RAW  (BIT(6))
#define SPI_OUT_DONE_INT_RAW_M  (BIT(6))
#define SPI_OUT_DONE_INT_RAW_V  0x1
#define SPI_OUT_DONE_INT_RAW_S  6
/* SPI_IN_SUC_EOF_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw bit for completing receiving all the packets from host.*/
#define SPI_IN_SUC_EOF_INT_RAW  (BIT(5))
#define SPI_IN_SUC_EOF_INT_RAW_M  (BIT(5))
#define SPI_IN_SUC_EOF_INT_RAW_V  0x1
#define SPI_IN_SUC_EOF_INT_RAW_S  5
/* SPI_IN_ERR_EOF_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw bit for receiving error.*/
#define SPI_IN_ERR_EOF_INT_RAW  (BIT(4))
#define SPI_IN_ERR_EOF_INT_RAW_M  (BIT(4))
#define SPI_IN_ERR_EOF_INT_RAW_V  0x1
#define SPI_IN_ERR_EOF_INT_RAW_S  4
/* SPI_IN_DONE_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw bit for completing usage of a inlink descriptor.*/
#define SPI_IN_DONE_INT_RAW  (BIT(3))
#define SPI_IN_DONE_INT_RAW_M  (BIT(3))
#define SPI_IN_DONE_INT_RAW_V  0x1
#define SPI_IN_DONE_INT_RAW_S  3
/* SPI_INLINK_DSCR_ERROR_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw bit for inlink descriptor error.*/
#define SPI_INLINK_DSCR_ERROR_INT_RAW  (BIT(2))
#define SPI_INLINK_DSCR_ERROR_INT_RAW_M  (BIT(2))
#define SPI_INLINK_DSCR_ERROR_INT_RAW_V  0x1
#define SPI_INLINK_DSCR_ERROR_INT_RAW_S  2
/* SPI_OUTLINK_DSCR_ERROR_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw bit for outlink descriptor error.*/
#define SPI_OUTLINK_DSCR_ERROR_INT_RAW  (BIT(1))
#define SPI_OUTLINK_DSCR_ERROR_INT_RAW_M  (BIT(1))
#define SPI_OUTLINK_DSCR_ERROR_INT_RAW_V  0x1
#define SPI_OUTLINK_DSCR_ERROR_INT_RAW_S  1
/* SPI_INLINK_DSCR_EMPTY_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw bit for lack of enough inlink descriptors.*/
#define SPI_INLINK_DSCR_EMPTY_INT_RAW  (BIT(0))
#define SPI_INLINK_DSCR_EMPTY_INT_RAW_M  (BIT(0))
#define SPI_INLINK_DSCR_EMPTY_INT_RAW_V  0x1
#define SPI_INLINK_DSCR_EMPTY_INT_RAW_S  0

#define SPI_DMA_INT_ST_REG(i)          (REG_SPI_BASE(i) + 0x118)
/* SPI_OUT_TOTAL_EOF_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The status bit for sending all the packets to host done.*/
#define SPI_OUT_TOTAL_EOF_INT_ST  (BIT(8))
#define SPI_OUT_TOTAL_EOF_INT_ST_M  (BIT(8))
#define SPI_OUT_TOTAL_EOF_INT_ST_V  0x1
#define SPI_OUT_TOTAL_EOF_INT_ST_S  8
/* SPI_OUT_EOF_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The status bit for sending a packet to host done.*/
#define SPI_OUT_EOF_INT_ST  (BIT(7))
#define SPI_OUT_EOF_INT_ST_M  (BIT(7))
#define SPI_OUT_EOF_INT_ST_V  0x1
#define SPI_OUT_EOF_INT_ST_S  7
/* SPI_OUT_DONE_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The status bit for completing usage of a outlink descriptor.*/
#define SPI_OUT_DONE_INT_ST  (BIT(6))
#define SPI_OUT_DONE_INT_ST_M  (BIT(6))
#define SPI_OUT_DONE_INT_ST_V  0x1
#define SPI_OUT_DONE_INT_ST_S  6
/* SPI_IN_SUC_EOF_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The status bit for completing receiving all the packets from host.*/
#define SPI_IN_SUC_EOF_INT_ST  (BIT(5))
#define SPI_IN_SUC_EOF_INT_ST_M  (BIT(5))
#define SPI_IN_SUC_EOF_INT_ST_V  0x1
#define SPI_IN_SUC_EOF_INT_ST_S  5
/* SPI_IN_ERR_EOF_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The status bit for receiving error.*/
#define SPI_IN_ERR_EOF_INT_ST  (BIT(4))
#define SPI_IN_ERR_EOF_INT_ST_M  (BIT(4))
#define SPI_IN_ERR_EOF_INT_ST_V  0x1
#define SPI_IN_ERR_EOF_INT_ST_S  4
/* SPI_IN_DONE_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The status bit for completing usage of a inlink descriptor.*/
#define SPI_IN_DONE_INT_ST  (BIT(3))
#define SPI_IN_DONE_INT_ST_M  (BIT(3))
#define SPI_IN_DONE_INT_ST_V  0x1
#define SPI_IN_DONE_INT_ST_S  3
/* SPI_INLINK_DSCR_ERROR_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The status bit for inlink descriptor error.*/
#define SPI_INLINK_DSCR_ERROR_INT_ST  (BIT(2))
#define SPI_INLINK_DSCR_ERROR_INT_ST_M  (BIT(2))
#define SPI_INLINK_DSCR_ERROR_INT_ST_V  0x1
#define SPI_INLINK_DSCR_ERROR_INT_ST_S  2
/* SPI_OUTLINK_DSCR_ERROR_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The status bit for outlink descriptor error.*/
#define SPI_OUTLINK_DSCR_ERROR_INT_ST  (BIT(1))
#define SPI_OUTLINK_DSCR_ERROR_INT_ST_M  (BIT(1))
#define SPI_OUTLINK_DSCR_ERROR_INT_ST_V  0x1
#define SPI_OUTLINK_DSCR_ERROR_INT_ST_S  1
/* SPI_INLINK_DSCR_EMPTY_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The status bit for lack of enough inlink descriptors.*/
#define SPI_INLINK_DSCR_EMPTY_INT_ST  (BIT(0))
#define SPI_INLINK_DSCR_EMPTY_INT_ST_M  (BIT(0))
#define SPI_INLINK_DSCR_EMPTY_INT_ST_V  0x1
#define SPI_INLINK_DSCR_EMPTY_INT_ST_S  0

#define SPI_DMA_INT_CLR_REG(i)          (REG_SPI_BASE(i) + 0x11C)
/* SPI_OUT_TOTAL_EOF_INT_CLR : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: The clear bit for sending all the packets to host done.*/
#define SPI_OUT_TOTAL_EOF_INT_CLR  (BIT(8))
#define SPI_OUT_TOTAL_EOF_INT_CLR_M  (BIT(8))
#define SPI_OUT_TOTAL_EOF_INT_CLR_V  0x1
#define SPI_OUT_TOTAL_EOF_INT_CLR_S  8
/* SPI_OUT_EOF_INT_CLR : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The clear bit for sending a packet to host done.*/
#define SPI_OUT_EOF_INT_CLR  (BIT(7))
#define SPI_OUT_EOF_INT_CLR_M  (BIT(7))
#define SPI_OUT_EOF_INT_CLR_V  0x1
#define SPI_OUT_EOF_INT_CLR_S  7
/* SPI_OUT_DONE_INT_CLR : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The clear bit for completing usage of a outlink descriptor.*/
#define SPI_OUT_DONE_INT_CLR  (BIT(6))
#define SPI_OUT_DONE_INT_CLR_M  (BIT(6))
#define SPI_OUT_DONE_INT_CLR_V  0x1
#define SPI_OUT_DONE_INT_CLR_S  6
/* SPI_IN_SUC_EOF_INT_CLR : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The clear bit for completing receiving all the packets from host.*/
#define SPI_IN_SUC_EOF_INT_CLR  (BIT(5))
#define SPI_IN_SUC_EOF_INT_CLR_M  (BIT(5))
#define SPI_IN_SUC_EOF_INT_CLR_V  0x1
#define SPI_IN_SUC_EOF_INT_CLR_S  5
/* SPI_IN_ERR_EOF_INT_CLR : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The clear bit for receiving error.*/
#define SPI_IN_ERR_EOF_INT_CLR  (BIT(4))
#define SPI_IN_ERR_EOF_INT_CLR_M  (BIT(4))
#define SPI_IN_ERR_EOF_INT_CLR_V  0x1
#define SPI_IN_ERR_EOF_INT_CLR_S  4
/* SPI_IN_DONE_INT_CLR : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The clear bit for completing usage of a inlink descriptor.*/
#define SPI_IN_DONE_INT_CLR  (BIT(3))
#define SPI_IN_DONE_INT_CLR_M  (BIT(3))
#define SPI_IN_DONE_INT_CLR_V  0x1
#define SPI_IN_DONE_INT_CLR_S  3
/* SPI_INLINK_DSCR_ERROR_INT_CLR : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The clear bit for inlink descriptor error.*/
#define SPI_INLINK_DSCR_ERROR_INT_CLR  (BIT(2))
#define SPI_INLINK_DSCR_ERROR_INT_CLR_M  (BIT(2))
#define SPI_INLINK_DSCR_ERROR_INT_CLR_V  0x1
#define SPI_INLINK_DSCR_ERROR_INT_CLR_S  2
/* SPI_OUTLINK_DSCR_ERROR_INT_CLR : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The clear bit for outlink descriptor error.*/
#define SPI_OUTLINK_DSCR_ERROR_INT_CLR  (BIT(1))
#define SPI_OUTLINK_DSCR_ERROR_INT_CLR_M  (BIT(1))
#define SPI_OUTLINK_DSCR_ERROR_INT_CLR_V  0x1
#define SPI_OUTLINK_DSCR_ERROR_INT_CLR_S  1
/* SPI_INLINK_DSCR_EMPTY_INT_CLR : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The clear bit for lack of enough inlink descriptors.*/
#define SPI_INLINK_DSCR_EMPTY_INT_CLR  (BIT(0))
#define SPI_INLINK_DSCR_EMPTY_INT_CLR_M  (BIT(0))
#define SPI_INLINK_DSCR_EMPTY_INT_CLR_V  0x1
#define SPI_INLINK_DSCR_EMPTY_INT_CLR_S  0

#define SPI_IN_ERR_EOF_DES_ADDR_REG(i)          (REG_SPI_BASE(i) + 0x120)
/* SPI_DMA_IN_ERR_EOF_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The inlink descriptor address when spi dma produce receiving error.*/
#define SPI_DMA_IN_ERR_EOF_DES_ADDR  0xFFFFFFFF
#define SPI_DMA_IN_ERR_EOF_DES_ADDR_M  ((SPI_DMA_IN_ERR_EOF_DES_ADDR_V)<<(SPI_DMA_IN_ERR_EOF_DES_ADDR_S))
#define SPI_DMA_IN_ERR_EOF_DES_ADDR_V  0xFFFFFFFF
#define SPI_DMA_IN_ERR_EOF_DES_ADDR_S  0

#define SPI_IN_SUC_EOF_DES_ADDR_REG(i)          (REG_SPI_BASE(i) + 0x124)
/* SPI_DMA_IN_SUC_EOF_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The last inlink descriptor address when spi dma produce from_suc_eof.*/
#define SPI_DMA_IN_SUC_EOF_DES_ADDR  0xFFFFFFFF
#define SPI_DMA_IN_SUC_EOF_DES_ADDR_M  ((SPI_DMA_IN_SUC_EOF_DES_ADDR_V)<<(SPI_DMA_IN_SUC_EOF_DES_ADDR_S))
#define SPI_DMA_IN_SUC_EOF_DES_ADDR_V  0xFFFFFFFF
#define SPI_DMA_IN_SUC_EOF_DES_ADDR_S  0

#define SPI_INLINK_DSCR_REG(i)          (REG_SPI_BASE(i) + 0x128)
/* SPI_DMA_INLINK_DSCR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The content of current in descriptor pointer.*/
#define SPI_DMA_INLINK_DSCR  0xFFFFFFFF
#define SPI_DMA_INLINK_DSCR_M  ((SPI_DMA_INLINK_DSCR_V)<<(SPI_DMA_INLINK_DSCR_S))
#define SPI_DMA_INLINK_DSCR_V  0xFFFFFFFF
#define SPI_DMA_INLINK_DSCR_S  0

#define SPI_INLINK_DSCR_BF0_REG(i)          (REG_SPI_BASE(i) + 0x12C)
/* SPI_DMA_INLINK_DSCR_BF0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The content of next in descriptor pointer.*/
#define SPI_DMA_INLINK_DSCR_BF0  0xFFFFFFFF
#define SPI_DMA_INLINK_DSCR_BF0_M  ((SPI_DMA_INLINK_DSCR_BF0_V)<<(SPI_DMA_INLINK_DSCR_BF0_S))
#define SPI_DMA_INLINK_DSCR_BF0_V  0xFFFFFFFF
#define SPI_DMA_INLINK_DSCR_BF0_S  0

#define SPI_INLINK_DSCR_BF1_REG(i)          (REG_SPI_BASE(i) + 0x130)
/* SPI_DMA_INLINK_DSCR_BF1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The content of current in descriptor data buffer pointer.*/
#define SPI_DMA_INLINK_DSCR_BF1  0xFFFFFFFF
#define SPI_DMA_INLINK_DSCR_BF1_M  ((SPI_DMA_INLINK_DSCR_BF1_V)<<(SPI_DMA_INLINK_DSCR_BF1_S))
#define SPI_DMA_INLINK_DSCR_BF1_V  0xFFFFFFFF
#define SPI_DMA_INLINK_DSCR_BF1_S  0

#define SPI_OUT_EOF_BFR_DES_ADDR_REG(i)          (REG_SPI_BASE(i) + 0x134)
/* SPI_DMA_OUT_EOF_BFR_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of buffer relative to the outlink descriptor that produce eof.*/
#define SPI_DMA_OUT_EOF_BFR_DES_ADDR  0xFFFFFFFF
#define SPI_DMA_OUT_EOF_BFR_DES_ADDR_M  ((SPI_DMA_OUT_EOF_BFR_DES_ADDR_V)<<(SPI_DMA_OUT_EOF_BFR_DES_ADDR_S))
#define SPI_DMA_OUT_EOF_BFR_DES_ADDR_V  0xFFFFFFFF
#define SPI_DMA_OUT_EOF_BFR_DES_ADDR_S  0

#define SPI_OUT_EOF_DES_ADDR_REG(i)          (REG_SPI_BASE(i) + 0x138)
/* SPI_DMA_OUT_EOF_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The last outlink descriptor address when spi dma produce to_eof.*/
#define SPI_DMA_OUT_EOF_DES_ADDR  0xFFFFFFFF
#define SPI_DMA_OUT_EOF_DES_ADDR_M  ((SPI_DMA_OUT_EOF_DES_ADDR_V)<<(SPI_DMA_OUT_EOF_DES_ADDR_S))
#define SPI_DMA_OUT_EOF_DES_ADDR_V  0xFFFFFFFF
#define SPI_DMA_OUT_EOF_DES_ADDR_S  0

#define SPI_OUTLINK_DSCR_REG(i)          (REG_SPI_BASE(i) + 0x13C)
/* SPI_DMA_OUTLINK_DSCR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The content of current out descriptor pointer.*/
#define SPI_DMA_OUTLINK_DSCR  0xFFFFFFFF
#define SPI_DMA_OUTLINK_DSCR_M  ((SPI_DMA_OUTLINK_DSCR_V)<<(SPI_DMA_OUTLINK_DSCR_S))
#define SPI_DMA_OUTLINK_DSCR_V  0xFFFFFFFF
#define SPI_DMA_OUTLINK_DSCR_S  0

#define SPI_OUTLINK_DSCR_BF0_REG(i)          (REG_SPI_BASE(i) + 0x140)
/* SPI_DMA_OUTLINK_DSCR_BF0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The content of next out descriptor pointer.*/
#define SPI_DMA_OUTLINK_DSCR_BF0  0xFFFFFFFF
#define SPI_DMA_OUTLINK_DSCR_BF0_M  ((SPI_DMA_OUTLINK_DSCR_BF0_V)<<(SPI_DMA_OUTLINK_DSCR_BF0_S))
#define SPI_DMA_OUTLINK_DSCR_BF0_V  0xFFFFFFFF
#define SPI_DMA_OUTLINK_DSCR_BF0_S  0

#define SPI_OUTLINK_DSCR_BF1_REG(i)          (REG_SPI_BASE(i) + 0x144)
/* SPI_DMA_OUTLINK_DSCR_BF1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The content of current out descriptor data buffer pointer.*/
#define SPI_DMA_OUTLINK_DSCR_BF1  0xFFFFFFFF
#define SPI_DMA_OUTLINK_DSCR_BF1_M  ((SPI_DMA_OUTLINK_DSCR_BF1_V)<<(SPI_DMA_OUTLINK_DSCR_BF1_S))
#define SPI_DMA_OUTLINK_DSCR_BF1_V  0xFFFFFFFF
#define SPI_DMA_OUTLINK_DSCR_BF1_S  0

#define SPI_DMA_RSTATUS_REG(i)          (REG_SPI_BASE(i) + 0x148)
/* SPI_DMA_OUT_STATUS : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: spi dma read data from memory status.*/
#define SPI_DMA_OUT_STATUS  0xFFFFFFFF
#define SPI_DMA_OUT_STATUS_M  ((SPI_DMA_OUT_STATUS_V)<<(SPI_DMA_OUT_STATUS_S))
#define SPI_DMA_OUT_STATUS_V  0xFFFFFFFF
#define SPI_DMA_OUT_STATUS_S  0

#define SPI_DMA_TSTATUS_REG(i)          (REG_SPI_BASE(i) + 0x14C)
/* SPI_DMA_IN_STATUS : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: spi dma write data to memory status.*/
#define SPI_DMA_IN_STATUS  0xFFFFFFFF
#define SPI_DMA_IN_STATUS_M  ((SPI_DMA_IN_STATUS_V)<<(SPI_DMA_IN_STATUS_S))
#define SPI_DMA_IN_STATUS_V  0xFFFFFFFF
#define SPI_DMA_IN_STATUS_S  0

#define SPI_DATE_REG(i)          (REG_SPI_BASE(i) + 0x3FC)
/* SPI_DATE : RO ;bitpos:[27:0] ;default: 32'h1604270 ; */
/*description: SPI register version.*/
#define SPI_DATE  0x0FFFFFFF
#define SPI_DATE_M  ((SPI_DATE_V)<<(SPI_DATE_S))
#define SPI_DATE_V  0xFFFFFFF
#define SPI_DATE_S  0




#endif /*__SPI_REG_H__ */


