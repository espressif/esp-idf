// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _SOC_SPI_MEM_REG_H_
#define _SOC_SPI_MEM_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"
#define REG_SPI_MEM_BASE(i)     (DR_REG_SPI0_BASE - (i) * 0x1000)

#define SPI_MEM_CMD_REG(i)          (REG_SPI_MEM_BASE(i) + 0x000)
/* SPI_MEM_FLASH_READ : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: Read flash enable. Read flash operation will be triggered when
 the bit is set. The bit will be cleared once the operation done. 1: enable 0: disable.*/
#define SPI_MEM_FLASH_READ  (BIT(31))
#define SPI_MEM_FLASH_READ_M  (BIT(31))
#define SPI_MEM_FLASH_READ_V  0x1
#define SPI_MEM_FLASH_READ_S  31
/* SPI_MEM_FLASH_WREN : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: Write flash enable.  Write enable command will be sent when the
 bit is set. The bit will be cleared once the operation done. 1: enable 0: disable.*/
#define SPI_MEM_FLASH_WREN  (BIT(30))
#define SPI_MEM_FLASH_WREN_M  (BIT(30))
#define SPI_MEM_FLASH_WREN_V  0x1
#define SPI_MEM_FLASH_WREN_S  30
/* SPI_MEM_FLASH_WRDI : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: Write flash disable. Write disable command will be sent when
 the bit is set. The bit will be cleared once the operation done. 1: enable 0: disable.*/
#define SPI_MEM_FLASH_WRDI  (BIT(29))
#define SPI_MEM_FLASH_WRDI_M  (BIT(29))
#define SPI_MEM_FLASH_WRDI_V  0x1
#define SPI_MEM_FLASH_WRDI_S  29
/* SPI_MEM_FLASH_RDID : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: Read JEDEC ID . Read ID command will be sent when the bit is
 set. The bit will be cleared once the operation done. 1: enable 0: disable.*/
#define SPI_MEM_FLASH_RDID  (BIT(28))
#define SPI_MEM_FLASH_RDID_M  (BIT(28))
#define SPI_MEM_FLASH_RDID_V  0x1
#define SPI_MEM_FLASH_RDID_S  28
/* SPI_MEM_FLASH_RDSR : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: Read status register-1.  Read status operation will be triggered
 when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_MEM_FLASH_RDSR  (BIT(27))
#define SPI_MEM_FLASH_RDSR_M  (BIT(27))
#define SPI_MEM_FLASH_RDSR_V  0x1
#define SPI_MEM_FLASH_RDSR_S  27
/* SPI_MEM_FLASH_WRSR : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: Write status register enable.   Write status operation  will
 be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_MEM_FLASH_WRSR  (BIT(26))
#define SPI_MEM_FLASH_WRSR_M  (BIT(26))
#define SPI_MEM_FLASH_WRSR_V  0x1
#define SPI_MEM_FLASH_WRSR_S  26
/* SPI_MEM_FLASH_PP : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: Page program enable(1 byte ~256 bytes data to be programmed).
 Page program operation  will be triggered when the bit is set. The bit will be cleared once the operation done .1: enable 0: disable.*/
#define SPI_MEM_FLASH_PP  (BIT(25))
#define SPI_MEM_FLASH_PP_M  (BIT(25))
#define SPI_MEM_FLASH_PP_V  0x1
#define SPI_MEM_FLASH_PP_S  25
/* SPI_MEM_FLASH_SE : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: Sector erase enable(4KB). Sector erase operation will be triggered
 when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_MEM_FLASH_SE  (BIT(24))
#define SPI_MEM_FLASH_SE_M  (BIT(24))
#define SPI_MEM_FLASH_SE_V  0x1
#define SPI_MEM_FLASH_SE_S  24
/* SPI_MEM_FLASH_BE : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: Block erase enable(32KB) .  Block erase operation will be triggered
 when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_MEM_FLASH_BE  (BIT(23))
#define SPI_MEM_FLASH_BE_M  (BIT(23))
#define SPI_MEM_FLASH_BE_V  0x1
#define SPI_MEM_FLASH_BE_S  23
/* SPI_MEM_FLASH_CE : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: Chip erase enable. Chip erase operation will be triggered when
 the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_MEM_FLASH_CE  (BIT(22))
#define SPI_MEM_FLASH_CE_M  (BIT(22))
#define SPI_MEM_FLASH_CE_V  0x1
#define SPI_MEM_FLASH_CE_S  22
/* SPI_MEM_FLASH_DP : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: Drive Flash into power down.  An operation will be triggered
 when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_MEM_FLASH_DP  (BIT(21))
#define SPI_MEM_FLASH_DP_M  (BIT(21))
#define SPI_MEM_FLASH_DP_V  0x1
#define SPI_MEM_FLASH_DP_S  21
/* SPI_MEM_FLASH_RES : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: This bit combined with reg_resandres bit releases Flash from
 the power-down state or high performance mode and obtains the devices ID. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_MEM_FLASH_RES  (BIT(20))
#define SPI_MEM_FLASH_RES_M  (BIT(20))
#define SPI_MEM_FLASH_RES_V  0x1
#define SPI_MEM_FLASH_RES_S  20
/* SPI_MEM_FLASH_HPM : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: Drive Flash into high performance mode.  The bit will be cleared
 once the operation done.1: enable 0: disable.*/
#define SPI_MEM_FLASH_HPM  (BIT(19))
#define SPI_MEM_FLASH_HPM_M  (BIT(19))
#define SPI_MEM_FLASH_HPM_V  0x1
#define SPI_MEM_FLASH_HPM_S  19
/* SPI_MEM_USR : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: User define command enable.  An operation will be triggered when
 the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_MEM_USR  (BIT(18))
#define SPI_MEM_USR_M  (BIT(18))
#define SPI_MEM_USR_V  0x1
#define SPI_MEM_USR_S  18
/* SPI_MEM_FLASH_PE : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: In user mode  it is set to indicate that program/erase operation
 will be triggered. The bit is combined with spi_mem_usr bit. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_MEM_FLASH_PE  (BIT(17))
#define SPI_MEM_FLASH_PE_M  (BIT(17))
#define SPI_MEM_FLASH_PE_V  0x1
#define SPI_MEM_FLASH_PE_S  17

#define SPI_MEM_ADDR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x004)
/* SPI_MEM_USR_ADDR_VALUE : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: In user mode  it is the memory address. other then the bit0-bit23
 is the memory address  the bit24-bit31 are the byte length of a transfer.*/
#define SPI_MEM_USR_ADDR_VALUE  0xFFFFFFFF
#define SPI_MEM_USR_ADDR_VALUE_M  ((SPI_MEM_USR_ADDR_VALUE_V)<<(SPI_MEM_USR_ADDR_VALUE_S))
#define SPI_MEM_USR_ADDR_VALUE_V  0xFFFFFFFF
#define SPI_MEM_USR_ADDR_VALUE_S  0

#define SPI_MEM_CTRL_REG(i)          (REG_SPI_MEM_BASE(i) + 0x008)
/* SPI_MEM_FREAD_QIO : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: In the read operations address phase and read-data phase apply
 4 signals. 1: enable 0: disable.*/
#define SPI_MEM_FREAD_QIO  (BIT(24))
#define SPI_MEM_FREAD_QIO_M  (BIT(24))
#define SPI_MEM_FREAD_QIO_V  0x1
#define SPI_MEM_FREAD_QIO_S  24
/* SPI_MEM_FREAD_DIO : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: In the read operations address phase and read-data phase apply
 2 signals. 1: enable 0: disable.*/
#define SPI_MEM_FREAD_DIO  (BIT(23))
#define SPI_MEM_FREAD_DIO_M  (BIT(23))
#define SPI_MEM_FREAD_DIO_V  0x1
#define SPI_MEM_FREAD_DIO_S  23
/* SPI_MEM_WRSR_2B : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: two bytes data will be written to status register when it is
 set. 1: enable 0: disable.*/
#define SPI_MEM_WRSR_2B  (BIT(22))
#define SPI_MEM_WRSR_2B_M  (BIT(22))
#define SPI_MEM_WRSR_2B_V  0x1
#define SPI_MEM_WRSR_2B_S  22
/* SPI_MEM_WP_REG : R/W ;bitpos:[21] ;default: 1'b1 ; */
/*description: Write protect signal output when SPI is idle.  1: output high  0: output low.*/
#define SPI_MEM_WP_REG  (BIT(21))
#define SPI_MEM_WP_REG_M  (BIT(21))
#define SPI_MEM_WP_REG_V  0x1
#define SPI_MEM_WP_REG_S  21
/* SPI_MEM_FREAD_QUAD : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: In the read operations read-data phase apply 4 signals. 1: enable 0: disable.*/
#define SPI_MEM_FREAD_QUAD  (BIT(20))
#define SPI_MEM_FREAD_QUAD_M  (BIT(20))
#define SPI_MEM_FREAD_QUAD_V  0x1
#define SPI_MEM_FREAD_QUAD_S  20
/* SPI_MEM_D_POL : R/W ;bitpos:[19] ;default: 1'b1 ; */
/*description: The bit is used to set MOSI line polarity  1: high 0  low*/
#define SPI_MEM_D_POL  (BIT(19))
#define SPI_MEM_D_POL_M  (BIT(19))
#define SPI_MEM_D_POL_V  0x1
#define SPI_MEM_D_POL_S  19
/* SPI_MEM_Q_POL : R/W ;bitpos:[18] ;default: 1'b1 ; */
/*description: The bit is used to set MISO line polarity  1: high 0  low*/
#define SPI_MEM_Q_POL  (BIT(18))
#define SPI_MEM_Q_POL_M  (BIT(18))
#define SPI_MEM_Q_POL_V  0x1
#define SPI_MEM_Q_POL_S  18
/* SPI_MEM_RESANDRES : R/W ;bitpos:[15] ;default: 1'b1 ; */
/*description: The Device ID is read out to SPI_MEM_RD_STATUS register   this
 bit combine with spi_mem_flash_res bit. 1: enable 0: disable.*/
#define SPI_MEM_RESANDRES  (BIT(15))
#define SPI_MEM_RESANDRES_M  (BIT(15))
#define SPI_MEM_RESANDRES_V  0x1
#define SPI_MEM_RESANDRES_S  15
/* SPI_MEM_FREAD_DUAL : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: In the read operations  read-data phase apply 2 signals. 1: enable 0: disable.*/
#define SPI_MEM_FREAD_DUAL  (BIT(14))
#define SPI_MEM_FREAD_DUAL_M  (BIT(14))
#define SPI_MEM_FREAD_DUAL_V  0x1
#define SPI_MEM_FREAD_DUAL_S  14
/* SPI_MEM_FASTRD_MODE : R/W ;bitpos:[13] ;default: 1'b1 ; */
/*description: This bit enable the bits: spi_mem_fread_qio  spi_mem_fread_dio
  spi_mem_fread_qout and spi_mem_fread_dout. 1: enable 0: disable.*/
#define SPI_MEM_FASTRD_MODE  (BIT(13))
#define SPI_MEM_FASTRD_MODE_M  (BIT(13))
#define SPI_MEM_FASTRD_MODE_V  0x1
#define SPI_MEM_FASTRD_MODE_S  13
/* SPI_MEM_TX_CRC_EN : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: For SPI1   enable crc32 when writing encrypted data to flash.
 1: enable 0:disable*/
#define SPI_MEM_TX_CRC_EN  (BIT(11))
#define SPI_MEM_TX_CRC_EN_M  (BIT(11))
#define SPI_MEM_TX_CRC_EN_V  0x1
#define SPI_MEM_TX_CRC_EN_S  11
/* SPI_MEM_FCS_CRC_EN : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: For SPI1   initialize crc32 module before writing encrypted data
 to flash. Active low.*/
#define SPI_MEM_FCS_CRC_EN  (BIT(10))
#define SPI_MEM_FCS_CRC_EN_M  (BIT(10))
#define SPI_MEM_FCS_CRC_EN_V  0x1
#define SPI_MEM_FCS_CRC_EN_S  10
/* SPI_MEM_FCMD_OCT : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: Apply 8 signals during command phase 1:enable 0: disable*/
#define SPI_MEM_FCMD_OCT  (BIT(9))
#define SPI_MEM_FCMD_OCT_M  (BIT(9))
#define SPI_MEM_FCMD_OCT_V  0x1
#define SPI_MEM_FCMD_OCT_S  9
/* SPI_MEM_FCMD_QUAD : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Apply 4 signals during command phase 1:enable 0: disable*/
#define SPI_MEM_FCMD_QUAD  (BIT(8))
#define SPI_MEM_FCMD_QUAD_M  (BIT(8))
#define SPI_MEM_FCMD_QUAD_V  0x1
#define SPI_MEM_FCMD_QUAD_S  8
/* SPI_MEM_FCMD_DUAL : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: Apply 2 signals during command phase 1:enable 0: disable*/
#define SPI_MEM_FCMD_DUAL  (BIT(7))
#define SPI_MEM_FCMD_DUAL_M  (BIT(7))
#define SPI_MEM_FCMD_DUAL_V  0x1
#define SPI_MEM_FCMD_DUAL_S  7
/* SPI_MEM_FADDR_OCT : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Apply 8 signals during address phase 1:enable 0: disable*/
#define SPI_MEM_FADDR_OCT  (BIT(6))
#define SPI_MEM_FADDR_OCT_M  (BIT(6))
#define SPI_MEM_FADDR_OCT_V  0x1
#define SPI_MEM_FADDR_OCT_S  6
/* SPI_MEM_FDIN_OCT : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: Apply 8 signals during read-data phase 1:enable 0: disable*/
#define SPI_MEM_FDIN_OCT  (BIT(5))
#define SPI_MEM_FDIN_OCT_M  (BIT(5))
#define SPI_MEM_FDIN_OCT_V  0x1
#define SPI_MEM_FDIN_OCT_S  5
/* SPI_MEM_FDOUT_OCT : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Apply 8 signals during write-data phase 1:enable 0: disable*/
#define SPI_MEM_FDOUT_OCT  (BIT(4))
#define SPI_MEM_FDOUT_OCT_M  (BIT(4))
#define SPI_MEM_FDOUT_OCT_V  0x1
#define SPI_MEM_FDOUT_OCT_S  4
/* SPI_MEM_FDUMMY_OUT : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: In the dummy phase the signal level of spi is output by the spi controller.*/
#define SPI_MEM_FDUMMY_OUT  (BIT(3))
#define SPI_MEM_FDUMMY_OUT_M  (BIT(3))
#define SPI_MEM_FDUMMY_OUT_V  0x1
#define SPI_MEM_FDUMMY_OUT_S  3

#define SPI_MEM_CTRL1_REG(i)          (REG_SPI_MEM_BASE(i) + 0x00C)
/* SPI_MEM_CS_DLY_EDGE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: The bit is used to select the spi clock edge to modify CS line timing.*/
#define SPI_MEM_CS_DLY_EDGE  (BIT(31))
#define SPI_MEM_CS_DLY_EDGE_M  (BIT(31))
#define SPI_MEM_CS_DLY_EDGE_V  0x1
#define SPI_MEM_CS_DLY_EDGE_S  31
/* SPI_MEM_CS_DLY_MODE : R/W ;bitpos:[30:28] ;default: 3'h0 ; */
/*description: The cs signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk*/
#define SPI_MEM_CS_DLY_MODE  0x00000007
#define SPI_MEM_CS_DLY_MODE_M  ((SPI_MEM_CS_DLY_MODE_V)<<(SPI_MEM_CS_DLY_MODE_S))
#define SPI_MEM_CS_DLY_MODE_V  0x7
#define SPI_MEM_CS_DLY_MODE_S  28
/* SPI_MEM_CS_DLY_NUM : R/W ;bitpos:[27:26] ;default: 2'h0 ; */
/*description: spi_mem_cs signal is delayed by system clock cycles*/
#define SPI_MEM_CS_DLY_NUM  0x00000003
#define SPI_MEM_CS_DLY_NUM_M  ((SPI_MEM_CS_DLY_NUM_V)<<(SPI_MEM_CS_DLY_NUM_S))
#define SPI_MEM_CS_DLY_NUM_V  0x3
#define SPI_MEM_CS_DLY_NUM_S  26
/* SPI_MEM_CS_HOLD_DLY : R/W ;bitpos:[25:14] ;default: 12'h1 ; */
/*description: SPI fsm is delayed to idle by spi clock cycles.*/
#define SPI_MEM_CS_HOLD_DLY  0x00000FFF
#define SPI_MEM_CS_HOLD_DLY_M  ((SPI_MEM_CS_HOLD_DLY_V)<<(SPI_MEM_CS_HOLD_DLY_S))
#define SPI_MEM_CS_HOLD_DLY_V  0xFFF
#define SPI_MEM_CS_HOLD_DLY_S  14
/* SPI_MEM_CS_HOLD_DLY_RES : R/W ;bitpos:[13:2] ;default: 12'hfff ; */
/*description: Delay cycles of resume Flash when resume Flash from standby mode
 is enable by spi clock.*/
#define SPI_MEM_CS_HOLD_DLY_RES  0x00000FFF
#define SPI_MEM_CS_HOLD_DLY_RES_M  ((SPI_MEM_CS_HOLD_DLY_RES_V)<<(SPI_MEM_CS_HOLD_DLY_RES_S))
#define SPI_MEM_CS_HOLD_DLY_RES_V  0xFFF
#define SPI_MEM_CS_HOLD_DLY_RES_S  2
/* SPI_MEM_CLK_MODE : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: SPI clock mode bits. 0: SPI clock is off when CS inactive 1:
 SPI clock is delayed one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3: SPI clock is alwasy on.*/
#define SPI_MEM_CLK_MODE  0x00000003
#define SPI_MEM_CLK_MODE_M  ((SPI_MEM_CLK_MODE_V)<<(SPI_MEM_CLK_MODE_S))
#define SPI_MEM_CLK_MODE_V  0x3
#define SPI_MEM_CLK_MODE_S  0

#define SPI_MEM_CTRL2_REG(i)          (REG_SPI_MEM_BASE(i) + 0x010)
/* SPI_MEM_SYNC_RESET : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: The FSM will be reset.*/
#define SPI_MEM_SYNC_RESET  (BIT(31))
#define SPI_MEM_SYNC_RESET_M  (BIT(31))
#define SPI_MEM_SYNC_RESET_V  0x1
#define SPI_MEM_SYNC_RESET_S  31
/* SPI_MEM_CS_HOLD_TIME : R/W ;bitpos:[25:13] ;default: 13'h1 ; */
/*description: Spi cs signal is delayed to inactive by spi clock this bits are
 combined with spi_mem_cs_hold bit.*/
#define SPI_MEM_CS_HOLD_TIME  0x00001FFF
#define SPI_MEM_CS_HOLD_TIME_M  ((SPI_MEM_CS_HOLD_TIME_V)<<(SPI_MEM_CS_HOLD_TIME_S))
#define SPI_MEM_CS_HOLD_TIME_V  0x1FFF
#define SPI_MEM_CS_HOLD_TIME_S  13
/* SPI_MEM_CS_SETUP_TIME : R/W ;bitpos:[12:0] ;default: 13'h1 ; */
/*description: (cycles-1) of prepare phase by spi clock this bits are combined
 with spi_mem_cs_setup bit.*/
#define SPI_MEM_CS_SETUP_TIME  0x00001FFF
#define SPI_MEM_CS_SETUP_TIME_M  ((SPI_MEM_CS_SETUP_TIME_V)<<(SPI_MEM_CS_SETUP_TIME_S))
#define SPI_MEM_CS_SETUP_TIME_V  0x1FFF
#define SPI_MEM_CS_SETUP_TIME_S  0

#define SPI_MEM_CLOCK_REG(i)          (REG_SPI_MEM_BASE(i) + 0x014)
/* SPI_MEM_CLK_EQU_SYSCLK : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: reserved*/
#define SPI_MEM_CLK_EQU_SYSCLK  (BIT(31))
#define SPI_MEM_CLK_EQU_SYSCLK_M  (BIT(31))
#define SPI_MEM_CLK_EQU_SYSCLK_V  0x1
#define SPI_MEM_CLK_EQU_SYSCLK_S  31
/* SPI_MEM_CLKCNT_N : R/W ;bitpos:[23:16] ;default: 8'h3 ; */
/*description: In the master mode it is the divider of spi_mem_clk. So spi_mem_clk
 frequency is system/(spi_mem_clkcnt_N+1)*/
#define SPI_MEM_CLKCNT_N  0x000000FF
#define SPI_MEM_CLKCNT_N_M  ((SPI_MEM_CLKCNT_N_V)<<(SPI_MEM_CLKCNT_N_S))
#define SPI_MEM_CLKCNT_N_V  0xFF
#define SPI_MEM_CLKCNT_N_S  16
/* SPI_MEM_CLKCNT_H : R/W ;bitpos:[15:8] ;default: 8'h1 ; */
/*description: In the master mode it must be floor((spi_mem_clkcnt_N+1)/2-1).*/
#define SPI_MEM_CLKCNT_H  0x000000FF
#define SPI_MEM_CLKCNT_H_M  ((SPI_MEM_CLKCNT_H_V)<<(SPI_MEM_CLKCNT_H_S))
#define SPI_MEM_CLKCNT_H_V  0xFF
#define SPI_MEM_CLKCNT_H_S  8
/* SPI_MEM_CLKCNT_L : R/W ;bitpos:[7:0] ;default: 8'h3 ; */
/*description: In the master mode it must be equal to spi_mem_clkcnt_N.*/
#define SPI_MEM_CLKCNT_L  0x000000FF
#define SPI_MEM_CLKCNT_L_M  ((SPI_MEM_CLKCNT_L_V)<<(SPI_MEM_CLKCNT_L_S))
#define SPI_MEM_CLKCNT_L_V  0xFF
#define SPI_MEM_CLKCNT_L_S  0

#define SPI_MEM_USER_REG(i)          (REG_SPI_MEM_BASE(i) + 0x018)
/* SPI_MEM_USR_COMMAND : R/W ;bitpos:[31] ;default: 1'b1 ; */
/*description: This bit enable the command phase of an operation.*/
#define SPI_MEM_USR_COMMAND  (BIT(31))
#define SPI_MEM_USR_COMMAND_M  (BIT(31))
#define SPI_MEM_USR_COMMAND_V  0x1
#define SPI_MEM_USR_COMMAND_S  31
/* SPI_MEM_USR_ADDR : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: This bit enable the address phase of an operation.*/
#define SPI_MEM_USR_ADDR  (BIT(30))
#define SPI_MEM_USR_ADDR_M  (BIT(30))
#define SPI_MEM_USR_ADDR_V  0x1
#define SPI_MEM_USR_ADDR_S  30
/* SPI_MEM_USR_DUMMY : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: This bit enable the dummy phase of an operation.*/
#define SPI_MEM_USR_DUMMY  (BIT(29))
#define SPI_MEM_USR_DUMMY_M  (BIT(29))
#define SPI_MEM_USR_DUMMY_V  0x1
#define SPI_MEM_USR_DUMMY_S  29
/* SPI_MEM_USR_MISO : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: This bit enable the read-data phase of an operation.*/
#define SPI_MEM_USR_MISO  (BIT(28))
#define SPI_MEM_USR_MISO_M  (BIT(28))
#define SPI_MEM_USR_MISO_V  0x1
#define SPI_MEM_USR_MISO_S  28
/* SPI_MEM_USR_MOSI : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: This bit enable the write-data phase of an operation.*/
#define SPI_MEM_USR_MOSI  (BIT(27))
#define SPI_MEM_USR_MOSI_M  (BIT(27))
#define SPI_MEM_USR_MOSI_V  0x1
#define SPI_MEM_USR_MOSI_S  27
/* SPI_MEM_USR_DUMMY_IDLE : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: spi clock is disable in dummy phase when the bit is enable.*/
#define SPI_MEM_USR_DUMMY_IDLE  (BIT(26))
#define SPI_MEM_USR_DUMMY_IDLE_M  (BIT(26))
#define SPI_MEM_USR_DUMMY_IDLE_V  0x1
#define SPI_MEM_USR_DUMMY_IDLE_S  26
/* SPI_MEM_USR_MOSI_HIGHPART : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: write-data phase only access to high-part of the buffer spi_mem_w8~spi_mem_w15.
 1: enable 0: disable.*/
#define SPI_MEM_USR_MOSI_HIGHPART  (BIT(25))
#define SPI_MEM_USR_MOSI_HIGHPART_M  (BIT(25))
#define SPI_MEM_USR_MOSI_HIGHPART_V  0x1
#define SPI_MEM_USR_MOSI_HIGHPART_S  25
/* SPI_MEM_USR_MISO_HIGHPART : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: read-data phase only access to high-part of the buffer spi_mem_w8~spi_mem_w15.
 1: enable 0: disable.*/
#define SPI_MEM_USR_MISO_HIGHPART  (BIT(24))
#define SPI_MEM_USR_MISO_HIGHPART_M  (BIT(24))
#define SPI_MEM_USR_MISO_HIGHPART_V  0x1
#define SPI_MEM_USR_MISO_HIGHPART_S  24
/* SPI_MEM_FWRITE_QIO : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: In the write operations address phase and read-data phase apply 4 signals.*/
#define SPI_MEM_FWRITE_QIO  (BIT(15))
#define SPI_MEM_FWRITE_QIO_M  (BIT(15))
#define SPI_MEM_FWRITE_QIO_V  0x1
#define SPI_MEM_FWRITE_QIO_S  15
/* SPI_MEM_FWRITE_DIO : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: In the write operations address phase and read-data phase apply 2 signals.*/
#define SPI_MEM_FWRITE_DIO  (BIT(14))
#define SPI_MEM_FWRITE_DIO_M  (BIT(14))
#define SPI_MEM_FWRITE_DIO_V  0x1
#define SPI_MEM_FWRITE_DIO_S  14
/* SPI_MEM_FWRITE_QUAD : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: In the write operations read-data phase apply 4 signals*/
#define SPI_MEM_FWRITE_QUAD  (BIT(13))
#define SPI_MEM_FWRITE_QUAD_M  (BIT(13))
#define SPI_MEM_FWRITE_QUAD_V  0x1
#define SPI_MEM_FWRITE_QUAD_S  13
/* SPI_MEM_FWRITE_DUAL : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: In the write operations read-data phase apply 2 signals*/
#define SPI_MEM_FWRITE_DUAL  (BIT(12))
#define SPI_MEM_FWRITE_DUAL_M  (BIT(12))
#define SPI_MEM_FWRITE_DUAL_V  0x1
#define SPI_MEM_FWRITE_DUAL_S  12
/* SPI_MEM_CK_OUT_EDGE : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: the bit combined with spi_mem_mosi_delay_mode bits to set mosi
 signal delay mode.*/
#define SPI_MEM_CK_OUT_EDGE  (BIT(9))
#define SPI_MEM_CK_OUT_EDGE_M  (BIT(9))
#define SPI_MEM_CK_OUT_EDGE_V  0x1
#define SPI_MEM_CK_OUT_EDGE_S  9
/* SPI_MEM_CS_SETUP : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: spi cs is enable when spi is in  prepare  phase. 1: enable 0: disable.*/
#define SPI_MEM_CS_SETUP  (BIT(7))
#define SPI_MEM_CS_SETUP_M  (BIT(7))
#define SPI_MEM_CS_SETUP_V  0x1
#define SPI_MEM_CS_SETUP_S  7
/* SPI_MEM_CS_HOLD : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: spi cs keep low when spi is in  done  phase. 1: enable 0: disable.*/
#define SPI_MEM_CS_HOLD  (BIT(6))
#define SPI_MEM_CS_HOLD_M  (BIT(6))
#define SPI_MEM_CS_HOLD_V  0x1
#define SPI_MEM_CS_HOLD_S  6

#define SPI_MEM_USER1_REG(i)          (REG_SPI_MEM_BASE(i) + 0x01C)
/* SPI_MEM_USR_ADDR_BITLEN : R/W ;bitpos:[31:26] ;default: 6'd23 ; */
/*description: The length in bits of address phase. The register value shall be (bit_num-1).*/
#define SPI_MEM_USR_ADDR_BITLEN  0x0000003F
#define SPI_MEM_USR_ADDR_BITLEN_M  ((SPI_MEM_USR_ADDR_BITLEN_V)<<(SPI_MEM_USR_ADDR_BITLEN_S))
#define SPI_MEM_USR_ADDR_BITLEN_V  0x3F
#define SPI_MEM_USR_ADDR_BITLEN_S  26
/* SPI_MEM_USR_DUMMY_CYCLELEN : R/W ;bitpos:[7:0] ;default: 8'd7 ; */
/*description: The length in spi_mem_clk cycles of dummy phase. The register
 value shall be (cycle_num-1).*/
#define SPI_MEM_USR_DUMMY_CYCLELEN  0x000000FF
#define SPI_MEM_USR_DUMMY_CYCLELEN_M  ((SPI_MEM_USR_DUMMY_CYCLELEN_V)<<(SPI_MEM_USR_DUMMY_CYCLELEN_S))
#define SPI_MEM_USR_DUMMY_CYCLELEN_V  0xFF
#define SPI_MEM_USR_DUMMY_CYCLELEN_S  0

#define SPI_MEM_USER2_REG(i)          (REG_SPI_MEM_BASE(i) + 0x020)
/* SPI_MEM_USR_COMMAND_BITLEN : R/W ;bitpos:[31:28] ;default: 4'd7 ; */
/*description: The length in bits of command phase. The register value shall be (bit_num-1)*/
#define SPI_MEM_USR_COMMAND_BITLEN  0x0000000F
#define SPI_MEM_USR_COMMAND_BITLEN_M  ((SPI_MEM_USR_COMMAND_BITLEN_V)<<(SPI_MEM_USR_COMMAND_BITLEN_S))
#define SPI_MEM_USR_COMMAND_BITLEN_V  0xF
#define SPI_MEM_USR_COMMAND_BITLEN_S  28
/* SPI_MEM_USR_COMMAND_VALUE : R/W ;bitpos:[15:0] ;default: 16'b0 ; */
/*description: The value of  command.*/
#define SPI_MEM_USR_COMMAND_VALUE  0x0000FFFF
#define SPI_MEM_USR_COMMAND_VALUE_M  ((SPI_MEM_USR_COMMAND_VALUE_V)<<(SPI_MEM_USR_COMMAND_VALUE_S))
#define SPI_MEM_USR_COMMAND_VALUE_V  0xFFFF
#define SPI_MEM_USR_COMMAND_VALUE_S  0

#define SPI_MEM_MOSI_DLEN_REG(i)          (REG_SPI_MEM_BASE(i) + 0x024)
/* SPI_MEM_USR_MOSI_DBITLEN : R/W ;bitpos:[10:0] ;default: 11'h0 ; */
/*description: The length in bits of write-data. The register value shall be (bit_num-1).*/
#define SPI_MEM_USR_MOSI_DBITLEN  0x000007FF
#define SPI_MEM_USR_MOSI_DBITLEN_M  ((SPI_MEM_USR_MOSI_DBITLEN_V)<<(SPI_MEM_USR_MOSI_DBITLEN_S))
#define SPI_MEM_USR_MOSI_DBITLEN_V  0x7FF
#define SPI_MEM_USR_MOSI_DBITLEN_S  0

#define SPI_MEM_MISO_DLEN_REG(i)          (REG_SPI_MEM_BASE(i) + 0x028)
/* SPI_MEM_USR_MISO_DBITLEN : R/W ;bitpos:[10:0] ;default: 11'h0 ; */
/*description: The length in bits of  read-data. The register value shall be (bit_num-1).*/
#define SPI_MEM_USR_MISO_DBITLEN  0x000007FF
#define SPI_MEM_USR_MISO_DBITLEN_M  ((SPI_MEM_USR_MISO_DBITLEN_V)<<(SPI_MEM_USR_MISO_DBITLEN_S))
#define SPI_MEM_USR_MISO_DBITLEN_V  0x7FF
#define SPI_MEM_USR_MISO_DBITLEN_S  0

#define SPI_MEM_RD_STATUS_REG(i)          (REG_SPI_MEM_BASE(i) + 0x02C)
/* SPI_MEM_WB_MODE : R/W ;bitpos:[23:16] ;default: 8'h00 ; */
/*description: Mode bits in the flash fast read mode  it is combined with spi_mem_fastrd_mode
 bit.*/
#define SPI_MEM_WB_MODE  0x000000FF
#define SPI_MEM_WB_MODE_M  ((SPI_MEM_WB_MODE_V)<<(SPI_MEM_WB_MODE_S))
#define SPI_MEM_WB_MODE_V  0xFF
#define SPI_MEM_WB_MODE_S  16
/* SPI_MEM_STATUS : R/W ;bitpos:[15:0] ;default: 16'b0 ; */
/*description: The value is stored when set spi_mem_flash_rdsr bit and spi_mem_flash_res bit.*/
#define SPI_MEM_STATUS  0x0000FFFF
#define SPI_MEM_STATUS_M  ((SPI_MEM_STATUS_V)<<(SPI_MEM_STATUS_S))
#define SPI_MEM_STATUS_V  0xFFFF
#define SPI_MEM_STATUS_S  0

#define SPI_MEM_EXT_ADDR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x030)
/* SPI_MEM_EXT_ADDR : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The register are the higher 32bits in the 64 bits address mode.*/
#define SPI_MEM_EXT_ADDR  0xFFFFFFFF
#define SPI_MEM_EXT_ADDR_M  ((SPI_MEM_EXT_ADDR_V)<<(SPI_MEM_EXT_ADDR_S))
#define SPI_MEM_EXT_ADDR_V  0xFFFFFFFF
#define SPI_MEM_EXT_ADDR_S  0

#define SPI_MEM_MISC_REG(i)          (REG_SPI_MEM_BASE(i) + 0x034)
/* SPI_MEM_AUTO_PER : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: reserved*/
#define SPI_MEM_AUTO_PER  (BIT(11))
#define SPI_MEM_AUTO_PER_M  (BIT(11))
#define SPI_MEM_AUTO_PER_V  0x1
#define SPI_MEM_AUTO_PER_S  11
/* SPI_MEM_CS_KEEP_ACTIVE : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: spi cs line keep low when the bit is set.*/
#define SPI_MEM_CS_KEEP_ACTIVE  (BIT(10))
#define SPI_MEM_CS_KEEP_ACTIVE_M  (BIT(10))
#define SPI_MEM_CS_KEEP_ACTIVE_V  0x1
#define SPI_MEM_CS_KEEP_ACTIVE_S  10
/* SPI_MEM_CK_IDLE_EDGE : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: 1: spi clk line is high when idle     0: spi clk line is low when idle*/
#define SPI_MEM_CK_IDLE_EDGE  (BIT(9))
#define SPI_MEM_CK_IDLE_EDGE_M  (BIT(9))
#define SPI_MEM_CK_IDLE_EDGE_V  0x1
#define SPI_MEM_CK_IDLE_EDGE_S  9
/* SPI_MEM_SSUB_PIN : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: For SPI0   sram is connected to SUBPINs.*/
#define SPI_MEM_SSUB_PIN  (BIT(8))
#define SPI_MEM_SSUB_PIN_M  (BIT(8))
#define SPI_MEM_SSUB_PIN_V  0x1
#define SPI_MEM_SSUB_PIN_S  8
/* SPI_MEM_FSUB_PIN : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: For SPI0   flash is connected to SUBPINs.*/
#define SPI_MEM_FSUB_PIN  (BIT(7))
#define SPI_MEM_FSUB_PIN_M  (BIT(7))
#define SPI_MEM_FSUB_PIN_V  0x1
#define SPI_MEM_FSUB_PIN_S  7
/* SPI_MEM_CS_POL : R/W ;bitpos:[6:5] ;default: 2'b0 ; */
/*description: In the master mode the bits are the polarity of spi cs line
 the value is equivalent to spi_mem_cs ^ spi_mem_master_cs_pol.*/
#define SPI_MEM_CS_POL  0x00000003
#define SPI_MEM_CS_POL_M  ((SPI_MEM_CS_POL_V)<<(SPI_MEM_CS_POL_S))
#define SPI_MEM_CS_POL_V  0x3
#define SPI_MEM_CS_POL_S  5
/* SPI_MEM_TRANS_END_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to enable the intterrupt of SPI transmitting done.*/
#define SPI_MEM_TRANS_END_INT_ENA  (BIT(4))
#define SPI_MEM_TRANS_END_INT_ENA_M  (BIT(4))
#define SPI_MEM_TRANS_END_INT_ENA_V  0x1
#define SPI_MEM_TRANS_END_INT_ENA_S  4
/* SPI_MEM_TRANS_END : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to indicate the transimitting is done.*/
#define SPI_MEM_TRANS_END  (BIT(3))
#define SPI_MEM_TRANS_END_M  (BIT(3))
#define SPI_MEM_TRANS_END_V  0x1
#define SPI_MEM_TRANS_END_S  3
/* SPI_MEM_CS1_DIS : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: SPI CS1 pin enable  1: disable CS1  0: spi_mem_cs1 signal is from/to CS1 pin*/
#define SPI_MEM_CS1_DIS  (BIT(1))
#define SPI_MEM_CS1_DIS_M  (BIT(1))
#define SPI_MEM_CS1_DIS_V  0x1
#define SPI_MEM_CS1_DIS_S  1
/* SPI_MEM_CS0_DIS : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: SPI CS0 pin enable  1: disable CS0  0: spi_mem_cs0 signal is from/to CS0 pin*/
#define SPI_MEM_CS0_DIS  (BIT(0))
#define SPI_MEM_CS0_DIS_M  (BIT(0))
#define SPI_MEM_CS0_DIS_V  0x1
#define SPI_MEM_CS0_DIS_S  0

#define SPI_MEM_TX_CRC_REG(i)          (REG_SPI_MEM_BASE(i) + 0x038)
/* SPI_MEM_TX_CRC_DATA : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: For SPI1  the value of crc32.*/
#define SPI_MEM_TX_CRC_DATA  0xFFFFFFFF
#define SPI_MEM_TX_CRC_DATA_M  ((SPI_MEM_TX_CRC_DATA_V)<<(SPI_MEM_TX_CRC_DATA_S))
#define SPI_MEM_TX_CRC_DATA_V  0xFFFFFFFF
#define SPI_MEM_TX_CRC_DATA_S  0

#define SPI_MEM_CACHE_FCTRL_REG(i)          (REG_SPI_MEM_BASE(i) + 0x03C)
/* SPI_MEM_FADDR_QUAD : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: For SPI0 flash  address phase apply 4 signals. 1: enable 0: disable.
  The bit is the same with spi_mem_fread_qio.*/
#define SPI_MEM_FADDR_QUAD  (BIT(8))
#define SPI_MEM_FADDR_QUAD_M  (BIT(8))
#define SPI_MEM_FADDR_QUAD_V  0x1
#define SPI_MEM_FADDR_QUAD_S  8
/* SPI_MEM_FDOUT_QUAD : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: For SPI0 flash  dout phase apply 4 signals. 1: enable 0: disable.
  The bit is the same with spi_mem_fread_qio.*/
#define SPI_MEM_FDOUT_QUAD  (BIT(7))
#define SPI_MEM_FDOUT_QUAD_M  (BIT(7))
#define SPI_MEM_FDOUT_QUAD_V  0x1
#define SPI_MEM_FDOUT_QUAD_S  7
/* SPI_MEM_FDIN_QUAD : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: For SPI0 flash  din phase apply 4 signals. 1: enable 0: disable.
  The bit is the same with spi_mem_fread_qio.*/
#define SPI_MEM_FDIN_QUAD  (BIT(6))
#define SPI_MEM_FDIN_QUAD_M  (BIT(6))
#define SPI_MEM_FDIN_QUAD_V  0x1
#define SPI_MEM_FDIN_QUAD_S  6
/* SPI_MEM_FADDR_DUAL : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: For SPI0 flash  address phase apply 2 signals. 1: enable 0: disable.
  The bit is the same with spi_mem_fread_dio.*/
#define SPI_MEM_FADDR_DUAL  (BIT(5))
#define SPI_MEM_FADDR_DUAL_M  (BIT(5))
#define SPI_MEM_FADDR_DUAL_V  0x1
#define SPI_MEM_FADDR_DUAL_S  5
/* SPI_MEM_FDOUT_DUAL : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: For SPI0 flash  dout phase apply 2 signals. 1: enable 0: disable.
 The bit is the same with spi_mem_fread_dio.*/
#define SPI_MEM_FDOUT_DUAL  (BIT(4))
#define SPI_MEM_FDOUT_DUAL_M  (BIT(4))
#define SPI_MEM_FDOUT_DUAL_V  0x1
#define SPI_MEM_FDOUT_DUAL_S  4
/* SPI_MEM_FDIN_DUAL : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: For SPI0 flash  din phase apply 2 signals. 1: enable 0: disable.
 The bit is the same with spi_mem_fread_dio.*/
#define SPI_MEM_FDIN_DUAL  (BIT(3))
#define SPI_MEM_FDIN_DUAL_M  (BIT(3))
#define SPI_MEM_FDIN_DUAL_V  0x1
#define SPI_MEM_FDIN_DUAL_S  3
/* SPI_MEM_CACHE_FLASH_USR_CMD : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: For SPI0   cache  read flash for user define command  1: enable  0:disable.*/
#define SPI_MEM_CACHE_FLASH_USR_CMD  (BIT(2))
#define SPI_MEM_CACHE_FLASH_USR_CMD_M  (BIT(2))
#define SPI_MEM_CACHE_FLASH_USR_CMD_V  0x1
#define SPI_MEM_CACHE_FLASH_USR_CMD_S  2
/* SPI_MEM_CACHE_USR_CMD_4BYTE : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: For SPI0   cache  read flash with 4 bytes command  1: enable  0:disable.*/
#define SPI_MEM_CACHE_USR_CMD_4BYTE  (BIT(1))
#define SPI_MEM_CACHE_USR_CMD_4BYTE_M  (BIT(1))
#define SPI_MEM_CACHE_USR_CMD_4BYTE_V  0x1
#define SPI_MEM_CACHE_USR_CMD_4BYTE_S  1
/* SPI_MEM_CACHE_REQ_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: For SPI0  Cache access enable  1: enable  0:disable.*/
#define SPI_MEM_CACHE_REQ_EN  (BIT(0))
#define SPI_MEM_CACHE_REQ_EN_M  (BIT(0))
#define SPI_MEM_CACHE_REQ_EN_V  0x1
#define SPI_MEM_CACHE_REQ_EN_S  0

#define SPI_MEM_CACHE_SCTRL_REG(i)          (REG_SPI_MEM_BASE(i) + 0x040)
/* SPI_MEM_SRAM_WDUMMY_CYCLELEN : R/W ;bitpos:[29:22] ;default: 8'b1 ; */
/*description: For SPI0  In the sram mode  it is the length in bits of write
 dummy phase. The register value shall be (bit_num-1).*/
#define SPI_MEM_SRAM_WDUMMY_CYCLELEN  0x000000FF
#define SPI_MEM_SRAM_WDUMMY_CYCLELEN_M  ((SPI_MEM_SRAM_WDUMMY_CYCLELEN_V)<<(SPI_MEM_SRAM_WDUMMY_CYCLELEN_S))
#define SPI_MEM_SRAM_WDUMMY_CYCLELEN_V  0xFF
#define SPI_MEM_SRAM_WDUMMY_CYCLELEN_S  22
/* SPI_MEM_SRAM_OCT : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: reserved*/
#define SPI_MEM_SRAM_OCT  (BIT(21))
#define SPI_MEM_SRAM_OCT_M  (BIT(21))
#define SPI_MEM_SRAM_OCT_V  0x1
#define SPI_MEM_SRAM_OCT_S  21
/* SPI_MEM_CACHE_SRAM_USR_WCMD : R/W ;bitpos:[20] ;default: 1'b1 ; */
/*description: For SPI0  In the spi sram mode cache write sram for user define command*/
#define SPI_MEM_CACHE_SRAM_USR_WCMD  (BIT(20))
#define SPI_MEM_CACHE_SRAM_USR_WCMD_M  (BIT(20))
#define SPI_MEM_CACHE_SRAM_USR_WCMD_V  0x1
#define SPI_MEM_CACHE_SRAM_USR_WCMD_S  20
/* SPI_MEM_SRAM_ADDR_BITLEN : R/W ;bitpos:[19:14] ;default: 6'd23 ; */
/*description: For SPI0  In the sram mode  it is the length in bits of address
 phase. The register value shall be (bit_num-1).*/
#define SPI_MEM_SRAM_ADDR_BITLEN  0x0000003F
#define SPI_MEM_SRAM_ADDR_BITLEN_M  ((SPI_MEM_SRAM_ADDR_BITLEN_V)<<(SPI_MEM_SRAM_ADDR_BITLEN_S))
#define SPI_MEM_SRAM_ADDR_BITLEN_V  0x3F
#define SPI_MEM_SRAM_ADDR_BITLEN_S  14
/* SPI_MEM_SRAM_RDUMMY_CYCLELEN : R/W ;bitpos:[13:6] ;default: 8'b1 ; */
/*description: For SPI0  In the sram mode  it is the length in bits of read
 dummy phase. The register value shall be (bit_num-1).*/
#define SPI_MEM_SRAM_RDUMMY_CYCLELEN  0x000000FF
#define SPI_MEM_SRAM_RDUMMY_CYCLELEN_M  ((SPI_MEM_SRAM_RDUMMY_CYCLELEN_V)<<(SPI_MEM_SRAM_RDUMMY_CYCLELEN_S))
#define SPI_MEM_SRAM_RDUMMY_CYCLELEN_V  0xFF
#define SPI_MEM_SRAM_RDUMMY_CYCLELEN_S  6
/* SPI_MEM_CACHE_SRAM_USR_RCMD : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: For SPI0  In the spi sram mode cache read sram for user define command.*/
#define SPI_MEM_CACHE_SRAM_USR_RCMD  (BIT(5))
#define SPI_MEM_CACHE_SRAM_USR_RCMD_M  (BIT(5))
#define SPI_MEM_CACHE_SRAM_USR_RCMD_V  0x1
#define SPI_MEM_CACHE_SRAM_USR_RCMD_S  5
/* SPI_MEM_USR_RD_SRAM_DUMMY : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: For SPI0  In the spi sram mode  it is the enable bit of dummy
 phase for read operations.*/
#define SPI_MEM_USR_RD_SRAM_DUMMY  (BIT(4))
#define SPI_MEM_USR_RD_SRAM_DUMMY_M  (BIT(4))
#define SPI_MEM_USR_RD_SRAM_DUMMY_V  0x1
#define SPI_MEM_USR_RD_SRAM_DUMMY_S  4
/* SPI_MEM_USR_WR_SRAM_DUMMY : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: For SPI0  In the spi sram mode  it is the enable bit of dummy
 phase for write operations.*/
#define SPI_MEM_USR_WR_SRAM_DUMMY  (BIT(3))
#define SPI_MEM_USR_WR_SRAM_DUMMY_M  (BIT(3))
#define SPI_MEM_USR_WR_SRAM_DUMMY_V  0x1
#define SPI_MEM_USR_WR_SRAM_DUMMY_S  3
/* SPI_MEM_USR_SRAM_QIO : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: For SPI0  In the spi sram mode  spi quad I/O mode enable  1: enable  0:disable*/
#define SPI_MEM_USR_SRAM_QIO  (BIT(2))
#define SPI_MEM_USR_SRAM_QIO_M  (BIT(2))
#define SPI_MEM_USR_SRAM_QIO_V  0x1
#define SPI_MEM_USR_SRAM_QIO_S  2
/* SPI_MEM_USR_SRAM_DIO : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: For SPI0  In the spi sram mode  spi dual I/O mode enable  1: enable  0:disable*/
#define SPI_MEM_USR_SRAM_DIO  (BIT(1))
#define SPI_MEM_USR_SRAM_DIO_M  (BIT(1))
#define SPI_MEM_USR_SRAM_DIO_V  0x1
#define SPI_MEM_USR_SRAM_DIO_S  1
/* SPI_MEM_CACHE_USR_SCMD_4BYTE : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: For SPI0  In the spi sram mode  cache read flash with 4 bytes
 command  1: enable  0:disable.*/
#define SPI_MEM_CACHE_USR_SCMD_4BYTE  (BIT(0))
#define SPI_MEM_CACHE_USR_SCMD_4BYTE_M  (BIT(0))
#define SPI_MEM_CACHE_USR_SCMD_4BYTE_V  0x1
#define SPI_MEM_CACHE_USR_SCMD_4BYTE_S  0

#define SPI_MEM_SRAM_CMD_REG(i)          (REG_SPI_MEM_BASE(i) + 0x044)
/* SPI_MEM_SDUMMY_OUT : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: In the dummy phase the signal level of spi is output by the spi controller.*/
#define SPI_MEM_SDUMMY_OUT  (BIT(22))
#define SPI_MEM_SDUMMY_OUT_M  (BIT(22))
#define SPI_MEM_SDUMMY_OUT_V  0x1
#define SPI_MEM_SDUMMY_OUT_S  22
/* SPI_MEM_SCMD_OCT : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: For SPI0 sram  cmd phase apply 8 signals. 1: enable 0: disable.*/
#define SPI_MEM_SCMD_OCT  (BIT(21))
#define SPI_MEM_SCMD_OCT_M  (BIT(21))
#define SPI_MEM_SCMD_OCT_V  0x1
#define SPI_MEM_SCMD_OCT_S  21
/* SPI_MEM_SADDR_OCT : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: For SPI0 sram  address phase apply 4 signals. 1: enable 0: disable.*/
#define SPI_MEM_SADDR_OCT  (BIT(20))
#define SPI_MEM_SADDR_OCT_M  (BIT(20))
#define SPI_MEM_SADDR_OCT_V  0x1
#define SPI_MEM_SADDR_OCT_S  20
/* SPI_MEM_SDOUT_OCT : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: For SPI0 sram  dout phase apply 8 signals. 1: enable 0: disable.*/
#define SPI_MEM_SDOUT_OCT  (BIT(19))
#define SPI_MEM_SDOUT_OCT_M  (BIT(19))
#define SPI_MEM_SDOUT_OCT_V  0x1
#define SPI_MEM_SDOUT_OCT_S  19
/* SPI_MEM_SDIN_OCT : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: For SPI0 sram  din phase apply 8 signals. 1: enable 0: disable.*/
#define SPI_MEM_SDIN_OCT  (BIT(18))
#define SPI_MEM_SDIN_OCT_M  (BIT(18))
#define SPI_MEM_SDIN_OCT_V  0x1
#define SPI_MEM_SDIN_OCT_S  18
/* SPI_MEM_SCMD_QUAD : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: For SPI0 sram  cmd phase apply 4 signals. 1: enable 0: disable.
 The bit is the same with spi_mem_usr_sram_qio.*/
#define SPI_MEM_SCMD_QUAD  (BIT(17))
#define SPI_MEM_SCMD_QUAD_M  (BIT(17))
#define SPI_MEM_SCMD_QUAD_V  0x1
#define SPI_MEM_SCMD_QUAD_S  17
/* SPI_MEM_SADDR_QUAD : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: For SPI0 sram  address phase apply 4 signals. 1: enable 0: disable.
 The bit is the same with spi_mem_usr_sram_qio.*/
#define SPI_MEM_SADDR_QUAD  (BIT(16))
#define SPI_MEM_SADDR_QUAD_M  (BIT(16))
#define SPI_MEM_SADDR_QUAD_V  0x1
#define SPI_MEM_SADDR_QUAD_S  16
/* SPI_MEM_SDOUT_QUAD : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: For SPI0 sram  dout phase apply 4 signals. 1: enable 0: disable.
 The bit is the same with spi_mem_usr_sram_qio.*/
#define SPI_MEM_SDOUT_QUAD  (BIT(15))
#define SPI_MEM_SDOUT_QUAD_M  (BIT(15))
#define SPI_MEM_SDOUT_QUAD_V  0x1
#define SPI_MEM_SDOUT_QUAD_S  15
/* SPI_MEM_SDIN_QUAD : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: For SPI0 sram  din phase apply 4 signals. 1: enable 0: disable.
 The bit is the same with spi_mem_usr_sram_qio.*/
#define SPI_MEM_SDIN_QUAD  (BIT(14))
#define SPI_MEM_SDIN_QUAD_M  (BIT(14))
#define SPI_MEM_SDIN_QUAD_V  0x1
#define SPI_MEM_SDIN_QUAD_S  14
/* SPI_MEM_SCMD_DUAL : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: For SPI0 sram  cmd phase apply 2 signals. 1: enable 0: disable.
 The bit is the same with spi_mem_usr_sram_dio.*/
#define SPI_MEM_SCMD_DUAL  (BIT(13))
#define SPI_MEM_SCMD_DUAL_M  (BIT(13))
#define SPI_MEM_SCMD_DUAL_V  0x1
#define SPI_MEM_SCMD_DUAL_S  13
/* SPI_MEM_SADDR_DUAL : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: For SPI0 sram  address phase apply 2 signals. 1: enable 0: disable.
 The bit is the same with spi_mem_usr_sram_dio.*/
#define SPI_MEM_SADDR_DUAL  (BIT(12))
#define SPI_MEM_SADDR_DUAL_M  (BIT(12))
#define SPI_MEM_SADDR_DUAL_V  0x1
#define SPI_MEM_SADDR_DUAL_S  12
/* SPI_MEM_SDOUT_DUAL : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: For SPI0 sram  dout phase apply 2 signals. 1: enable 0: disable.
 The bit is the same with spi_mem_usr_sram_dio.*/
#define SPI_MEM_SDOUT_DUAL  (BIT(11))
#define SPI_MEM_SDOUT_DUAL_M  (BIT(11))
#define SPI_MEM_SDOUT_DUAL_V  0x1
#define SPI_MEM_SDOUT_DUAL_S  11
/* SPI_MEM_SDIN_DUAL : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: For SPI0 sram  din phase apply 2 signals. 1: enable 0: disable.
 The bit is the same with spi_mem_usr_sram_dio.*/
#define SPI_MEM_SDIN_DUAL  (BIT(10))
#define SPI_MEM_SDIN_DUAL_M  (BIT(10))
#define SPI_MEM_SDIN_DUAL_V  0x1
#define SPI_MEM_SDIN_DUAL_S  10
/* SPI_MEM_SWB_MODE : R/W ;bitpos:[9:2] ;default: 8'b0 ; */
/*description: Mode bits in the psram fast read mode  it is combined with spi_mem_fastrd_mode
 bit.*/
#define SPI_MEM_SWB_MODE  0x000000FF
#define SPI_MEM_SWB_MODE_M  ((SPI_MEM_SWB_MODE_V)<<(SPI_MEM_SWB_MODE_S))
#define SPI_MEM_SWB_MODE_V  0xFF
#define SPI_MEM_SWB_MODE_S  2
/* SPI_MEM_SCLK_MODE : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: SPI clock mode bits. 0: SPI clock is off when CS inactive 1:
 SPI clock is delayed one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3: SPI clock is alwasy on.*/
#define SPI_MEM_SCLK_MODE  0x00000003
#define SPI_MEM_SCLK_MODE_M  ((SPI_MEM_SCLK_MODE_V)<<(SPI_MEM_SCLK_MODE_S))
#define SPI_MEM_SCLK_MODE_V  0x3
#define SPI_MEM_SCLK_MODE_S  0

#define SPI_MEM_SRAM_DRD_CMD_REG(i)          (REG_SPI_MEM_BASE(i) + 0x048)
/* SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN : R/W ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: For SPI0 When cache mode is enable it is the length in bits of
 command phase for sram. The register value shall be (bit_num-1).*/
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN  0x0000000F
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_M  ((SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_V)<<(SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_S))
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_V  0xF
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_S  28
/* SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: For SPI0 When cache mode is enable it is the read command value
 of command phase for sram.*/
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE  0x0000FFFF
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_M  ((SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_V)<<(SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_S))
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_V  0xFFFF
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_S  0

#define SPI_MEM_SRAM_DWR_CMD_REG(i)          (REG_SPI_MEM_BASE(i) + 0x04C)
/* SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN : R/W ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: For SPI0 When cache mode is enable it is the in bits of command
 phase  for sram. The register value shall be (bit_num-1).*/
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN  0x0000000F
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_M  ((SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_V)<<(SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_S))
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_V  0xF
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_S  28
/* SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: For SPI0 When cache mode is enable it is the write command value
 of command phase for sram.*/
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE  0x0000FFFF
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_M  ((SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_V)<<(SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_S))
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_V  0xFFFF
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_S  0

#define SPI_MEM_SRAM_CLK_REG(i)          (REG_SPI_MEM_BASE(i) + 0x050)
/* SPI_MEM_SCLK_EQU_SYSCLK : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: For SPI0 sram interface  1: spi_mem_clk is eqaul to system 0:
 spi_mem_clk is divided from system clock.*/
#define SPI_MEM_SCLK_EQU_SYSCLK  (BIT(31))
#define SPI_MEM_SCLK_EQU_SYSCLK_M  (BIT(31))
#define SPI_MEM_SCLK_EQU_SYSCLK_V  0x1
#define SPI_MEM_SCLK_EQU_SYSCLK_S  31
/* SPI_MEM_SCLKCNT_N : R/W ;bitpos:[23:16] ;default: 8'h3 ; */
/*description: For SPI0 sram interface  it is the divider of spi_mem_clk. So
 spi_mem_clk frequency is system/(spi_mem_clkcnt_N+1)*/
#define SPI_MEM_SCLKCNT_N  0x000000FF
#define SPI_MEM_SCLKCNT_N_M  ((SPI_MEM_SCLKCNT_N_V)<<(SPI_MEM_SCLKCNT_N_S))
#define SPI_MEM_SCLKCNT_N_V  0xFF
#define SPI_MEM_SCLKCNT_N_S  16
/* SPI_MEM_SCLKCNT_H : R/W ;bitpos:[15:8] ;default: 8'h1 ; */
/*description: For SPI0 sram interface  it must be floor((spi_mem_clkcnt_N+1)/2-1).*/
#define SPI_MEM_SCLKCNT_H  0x000000FF
#define SPI_MEM_SCLKCNT_H_M  ((SPI_MEM_SCLKCNT_H_V)<<(SPI_MEM_SCLKCNT_H_S))
#define SPI_MEM_SCLKCNT_H_V  0xFF
#define SPI_MEM_SCLKCNT_H_S  8
/* SPI_MEM_SCLKCNT_L : R/W ;bitpos:[7:0] ;default: 8'h3 ; */
/*description: For SPI0 sram interface  it must be equal to spi_mem_clkcnt_N.*/
#define SPI_MEM_SCLKCNT_L  0x000000FF
#define SPI_MEM_SCLKCNT_L_M  ((SPI_MEM_SCLKCNT_L_V)<<(SPI_MEM_SCLKCNT_L_S))
#define SPI_MEM_SCLKCNT_L_V  0xFF
#define SPI_MEM_SCLKCNT_L_S  0

#define SPI_MEM_FSM_REG(i)          (REG_SPI_MEM_BASE(i) + 0x054)
/* SPI_MEM_ST : RO ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: The status of spi state machine. 0: idle state  1: preparation
 state  2: send command state  3: send data state  4: red data state  5:write data state  6: wait state  7: done state.*/
#define SPI_MEM_ST  0x00000007
#define SPI_MEM_ST_M  ((SPI_MEM_ST_V)<<(SPI_MEM_ST_S))
#define SPI_MEM_ST_V  0x7
#define SPI_MEM_ST_S  0

#define SPI_MEM_W0_REG(i)          (REG_SPI_MEM_BASE(i) + 0x058)
/* SPI_MEM_BUF0 : SRW ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_MEM_BUF0  0xFFFFFFFF
#define SPI_MEM_BUF0_M  ((SPI_MEM_BUF0_V)<<(SPI_MEM_BUF0_S))
#define SPI_MEM_BUF0_V  0xFFFFFFFF
#define SPI_MEM_BUF0_S  0

#define SPI_MEM_W1_REG(i)          (REG_SPI_MEM_BASE(i) + 0x05C)
/* SPI_MEM_BUF1 : SRW ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_MEM_BUF1  0xFFFFFFFF
#define SPI_MEM_BUF1_M  ((SPI_MEM_BUF1_V)<<(SPI_MEM_BUF1_S))
#define SPI_MEM_BUF1_V  0xFFFFFFFF
#define SPI_MEM_BUF1_S  0

#define SPI_MEM_W2_REG(i)          (REG_SPI_MEM_BASE(i) + 0x060)
/* SPI_MEM_BUF2 : SRW ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_MEM_BUF2  0xFFFFFFFF
#define SPI_MEM_BUF2_M  ((SPI_MEM_BUF2_V)<<(SPI_MEM_BUF2_S))
#define SPI_MEM_BUF2_V  0xFFFFFFFF
#define SPI_MEM_BUF2_S  0

#define SPI_MEM_W3_REG(i)          (REG_SPI_MEM_BASE(i) + 0x064)
/* SPI_MEM_BUF3 : SRW ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_MEM_BUF3  0xFFFFFFFF
#define SPI_MEM_BUF3_M  ((SPI_MEM_BUF3_V)<<(SPI_MEM_BUF3_S))
#define SPI_MEM_BUF3_V  0xFFFFFFFF
#define SPI_MEM_BUF3_S  0

#define SPI_MEM_W4_REG(i)          (REG_SPI_MEM_BASE(i) + 0x068)
/* SPI_MEM_BUF4 : SRW ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_MEM_BUF4  0xFFFFFFFF
#define SPI_MEM_BUF4_M  ((SPI_MEM_BUF4_V)<<(SPI_MEM_BUF4_S))
#define SPI_MEM_BUF4_V  0xFFFFFFFF
#define SPI_MEM_BUF4_S  0

#define SPI_MEM_W5_REG(i)          (REG_SPI_MEM_BASE(i) + 0x06C)
/* SPI_MEM_BUF5 : SRW ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_MEM_BUF5  0xFFFFFFFF
#define SPI_MEM_BUF5_M  ((SPI_MEM_BUF5_V)<<(SPI_MEM_BUF5_S))
#define SPI_MEM_BUF5_V  0xFFFFFFFF
#define SPI_MEM_BUF5_S  0

#define SPI_MEM_W6_REG(i)          (REG_SPI_MEM_BASE(i) + 0x070)
/* SPI_MEM_BUF6 : SRW ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_MEM_BUF6  0xFFFFFFFF
#define SPI_MEM_BUF6_M  ((SPI_MEM_BUF6_V)<<(SPI_MEM_BUF6_S))
#define SPI_MEM_BUF6_V  0xFFFFFFFF
#define SPI_MEM_BUF6_S  0

#define SPI_MEM_W7_REG(i)          (REG_SPI_MEM_BASE(i) + 0x074)
/* SPI_MEM_BUF7 : SRW ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_MEM_BUF7  0xFFFFFFFF
#define SPI_MEM_BUF7_M  ((SPI_MEM_BUF7_V)<<(SPI_MEM_BUF7_S))
#define SPI_MEM_BUF7_V  0xFFFFFFFF
#define SPI_MEM_BUF7_S  0

#define SPI_MEM_W8_REG(i)          (REG_SPI_MEM_BASE(i) + 0x078)
/* SPI_MEM_BUF8 : SRW ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_MEM_BUF8  0xFFFFFFFF
#define SPI_MEM_BUF8_M  ((SPI_MEM_BUF8_V)<<(SPI_MEM_BUF8_S))
#define SPI_MEM_BUF8_V  0xFFFFFFFF
#define SPI_MEM_BUF8_S  0

#define SPI_MEM_W9_REG(i)          (REG_SPI_MEM_BASE(i) + 0x07C)
/* SPI_MEM_BUF9 : SRW ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_MEM_BUF9  0xFFFFFFFF
#define SPI_MEM_BUF9_M  ((SPI_MEM_BUF9_V)<<(SPI_MEM_BUF9_S))
#define SPI_MEM_BUF9_V  0xFFFFFFFF
#define SPI_MEM_BUF9_S  0

#define SPI_MEM_W10_REG(i)          (REG_SPI_MEM_BASE(i) + 0x080)
/* SPI_MEM_BUF10 : SRW ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_MEM_BUF10  0xFFFFFFFF
#define SPI_MEM_BUF10_M  ((SPI_MEM_BUF10_V)<<(SPI_MEM_BUF10_S))
#define SPI_MEM_BUF10_V  0xFFFFFFFF
#define SPI_MEM_BUF10_S  0

#define SPI_MEM_W11_REG(i)          (REG_SPI_MEM_BASE(i) + 0x084)
/* SPI_MEM_BUF11 : SRW ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_MEM_BUF11  0xFFFFFFFF
#define SPI_MEM_BUF11_M  ((SPI_MEM_BUF11_V)<<(SPI_MEM_BUF11_S))
#define SPI_MEM_BUF11_V  0xFFFFFFFF
#define SPI_MEM_BUF11_S  0

#define SPI_MEM_W12_REG(i)          (REG_SPI_MEM_BASE(i) + 0x088)
/* SPI_MEM_BUF12 : SRW ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_MEM_BUF12  0xFFFFFFFF
#define SPI_MEM_BUF12_M  ((SPI_MEM_BUF12_V)<<(SPI_MEM_BUF12_S))
#define SPI_MEM_BUF12_V  0xFFFFFFFF
#define SPI_MEM_BUF12_S  0

#define SPI_MEM_W13_REG(i)          (REG_SPI_MEM_BASE(i) + 0x08C)
/* SPI_MEM_BUF13 : SRW ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_MEM_BUF13  0xFFFFFFFF
#define SPI_MEM_BUF13_M  ((SPI_MEM_BUF13_V)<<(SPI_MEM_BUF13_S))
#define SPI_MEM_BUF13_V  0xFFFFFFFF
#define SPI_MEM_BUF13_S  0

#define SPI_MEM_W14_REG(i)          (REG_SPI_MEM_BASE(i) + 0x090)
/* SPI_MEM_BUF14 : SRW ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_MEM_BUF14  0xFFFFFFFF
#define SPI_MEM_BUF14_M  ((SPI_MEM_BUF14_V)<<(SPI_MEM_BUF14_S))
#define SPI_MEM_BUF14_V  0xFFFFFFFF
#define SPI_MEM_BUF14_S  0

#define SPI_MEM_W15_REG(i)          (REG_SPI_MEM_BASE(i) + 0x094)
/* SPI_MEM_BUF15 : SRW ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer*/
#define SPI_MEM_BUF15  0xFFFFFFFF
#define SPI_MEM_BUF15_M  ((SPI_MEM_BUF15_V)<<(SPI_MEM_BUF15_S))
#define SPI_MEM_BUF15_V  0xFFFFFFFF
#define SPI_MEM_BUF15_S  0

#define SPI_MEM_FLASH_WAITI_CTRL_REG(i)          (REG_SPI_MEM_BASE(i) + 0x098)
/* SPI_MEM_WAITI_DUMMY_CYCLELEN : R/W ;bitpos:[17:10] ;default: 8'h0 ; */
/*description: The dummy cycle length when auto wait flash idle*/
#define SPI_MEM_WAITI_DUMMY_CYCLELEN  0x000000FF
#define SPI_MEM_WAITI_DUMMY_CYCLELEN_M  ((SPI_MEM_WAITI_DUMMY_CYCLELEN_V)<<(SPI_MEM_WAITI_DUMMY_CYCLELEN_S))
#define SPI_MEM_WAITI_DUMMY_CYCLELEN_V  0xFF
#define SPI_MEM_WAITI_DUMMY_CYCLELEN_S  10
/* SPI_MEM_WAITI_CMD : R/W ;bitpos:[9:2] ;default: 8'h05 ; */
/*description: The command to auto wait idle*/
#define SPI_MEM_WAITI_CMD  0x000000FF
#define SPI_MEM_WAITI_CMD_M  ((SPI_MEM_WAITI_CMD_V)<<(SPI_MEM_WAITI_CMD_S))
#define SPI_MEM_WAITI_CMD_V  0xFF
#define SPI_MEM_WAITI_CMD_S  2
/* SPI_MEM_WAITI_DUMMY : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The dummy phase enable when auto wait flash idle*/
#define SPI_MEM_WAITI_DUMMY  (BIT(1))
#define SPI_MEM_WAITI_DUMMY_M  (BIT(1))
#define SPI_MEM_WAITI_DUMMY_V  0x1
#define SPI_MEM_WAITI_DUMMY_S  1
/* SPI_MEM_WAITI_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: auto-waiting flash idle operation when program flash or erase
 flash. 1: enable 0: disable.*/
#define SPI_MEM_WAITI_EN  (BIT(0))
#define SPI_MEM_WAITI_EN_M  (BIT(0))
#define SPI_MEM_WAITI_EN_V  0x1
#define SPI_MEM_WAITI_EN_S  0

#define SPI_MEM_FLASH_SUS_CMD_REG(i)          (REG_SPI_MEM_BASE(i) + 0x09C)
/* SPI_MEM_FLASH_PES : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: program erase suspend bit  program erase suspend operation will
 be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_MEM_FLASH_PES  (BIT(1))
#define SPI_MEM_FLASH_PES_M  (BIT(1))
#define SPI_MEM_FLASH_PES_V  0x1
#define SPI_MEM_FLASH_PES_S  1
/* SPI_MEM_FLASH_PER : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: program erase resume bit  program erase suspend operation will
 be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
#define SPI_MEM_FLASH_PER  (BIT(0))
#define SPI_MEM_FLASH_PER_M  (BIT(0))
#define SPI_MEM_FLASH_PER_V  0x1
#define SPI_MEM_FLASH_PER_S  0

#define SPI_MEM_FLASH_SUS_CTRL_REG(i)          (REG_SPI_MEM_BASE(i) + 0x0A0)
/* SPI_MEM_FLASH_PES_COMMAND : R/W ;bitpos:[16:9] ;default: 8'h75 ; */
/*description: Program/Erase suspend command.*/
#define SPI_MEM_FLASH_PES_COMMAND  0x000000FF
#define SPI_MEM_FLASH_PES_COMMAND_M  ((SPI_MEM_FLASH_PES_COMMAND_V)<<(SPI_MEM_FLASH_PES_COMMAND_S))
#define SPI_MEM_FLASH_PES_COMMAND_V  0xFF
#define SPI_MEM_FLASH_PES_COMMAND_S  9
/* SPI_MEM_FLASH_PER_COMMAND : R/W ;bitpos:[8:1] ;default: 8'h7a ; */
/*description: Program/Erase resume command.*/
#define SPI_MEM_FLASH_PER_COMMAND  0x000000FF
#define SPI_MEM_FLASH_PER_COMMAND_M  ((SPI_MEM_FLASH_PER_COMMAND_V)<<(SPI_MEM_FLASH_PER_COMMAND_S))
#define SPI_MEM_FLASH_PER_COMMAND_V  0xFF
#define SPI_MEM_FLASH_PER_COMMAND_S  1
/* SPI_MEM_FLASH_PES_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Auto-suspending enable*/
#define SPI_MEM_FLASH_PES_EN  (BIT(0))
#define SPI_MEM_FLASH_PES_EN_M  (BIT(0))
#define SPI_MEM_FLASH_PES_EN_V  0x1
#define SPI_MEM_FLASH_PES_EN_S  0

#define SPI_MEM_SUS_STATUS_REG(i)          (REG_SPI_MEM_BASE(i) + 0x0A4)
/* SPI_MEM_FLASH_SUS : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: The status of flash suspend  only used in SPI1.*/
#define SPI_MEM_FLASH_SUS  (BIT(0))
#define SPI_MEM_FLASH_SUS_M  (BIT(0))
#define SPI_MEM_FLASH_SUS_V  0x1
#define SPI_MEM_FLASH_SUS_S  0

#define SPI_MEM_TIMING_CALI_REG(i)          (REG_SPI_MEM_BASE(i) + 0x0A8)
/* SPI_MEM_EXTRA_DUMMY_CYCLELEN : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: add extra dummy spi clock cycle length for spi clock calibration.*/
#define SPI_MEM_EXTRA_DUMMY_CYCLELEN  0x00000003
#define SPI_MEM_EXTRA_DUMMY_CYCLELEN_M  ((SPI_MEM_EXTRA_DUMMY_CYCLELEN_V)<<(SPI_MEM_EXTRA_DUMMY_CYCLELEN_S))
#define SPI_MEM_EXTRA_DUMMY_CYCLELEN_V  0x3
#define SPI_MEM_EXTRA_DUMMY_CYCLELEN_S  2
/* SPI_MEM_TIMING_CALI : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to enable timing auto-calibration for all reading operations.*/
#define SPI_MEM_TIMING_CALI  (BIT(1))
#define SPI_MEM_TIMING_CALI_M  (BIT(1))
#define SPI_MEM_TIMING_CALI_V  0x1
#define SPI_MEM_TIMING_CALI_S  1
/* SPI_MEM_TIMING_CLK_ENA : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to enable timing adjust clock for all reading operations.*/
#define SPI_MEM_TIMING_CLK_ENA  (BIT(0))
#define SPI_MEM_TIMING_CLK_ENA_M  (BIT(0))
#define SPI_MEM_TIMING_CLK_ENA_V  0x1
#define SPI_MEM_TIMING_CLK_ENA_S  0

#define SPI_MEM_DIN_MODE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x0AC)
/* SPI_MEM_DINS_MODE : R/W ;bitpos:[26:24] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk*/
#define SPI_MEM_DINS_MODE  0x00000007
#define SPI_MEM_DINS_MODE_M  ((SPI_MEM_DINS_MODE_V)<<(SPI_MEM_DINS_MODE_S))
#define SPI_MEM_DINS_MODE_V  0x7
#define SPI_MEM_DINS_MODE_S  24
/* SPI_MEM_DIN7_MODE : R/W ;bitpos:[23:21] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk*/
#define SPI_MEM_DIN7_MODE  0x00000007
#define SPI_MEM_DIN7_MODE_M  ((SPI_MEM_DIN7_MODE_V)<<(SPI_MEM_DIN7_MODE_S))
#define SPI_MEM_DIN7_MODE_V  0x7
#define SPI_MEM_DIN7_MODE_S  21
/* SPI_MEM_DIN6_MODE : R/W ;bitpos:[20:18] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk*/
#define SPI_MEM_DIN6_MODE  0x00000007
#define SPI_MEM_DIN6_MODE_M  ((SPI_MEM_DIN6_MODE_V)<<(SPI_MEM_DIN6_MODE_S))
#define SPI_MEM_DIN6_MODE_V  0x7
#define SPI_MEM_DIN6_MODE_S  18
/* SPI_MEM_DIN5_MODE : R/W ;bitpos:[17:15] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk*/
#define SPI_MEM_DIN5_MODE  0x00000007
#define SPI_MEM_DIN5_MODE_M  ((SPI_MEM_DIN5_MODE_V)<<(SPI_MEM_DIN5_MODE_S))
#define SPI_MEM_DIN5_MODE_V  0x7
#define SPI_MEM_DIN5_MODE_S  15
/* SPI_MEM_DIN4_MODE : R/W ;bitpos:[14:12] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk*/
#define SPI_MEM_DIN4_MODE  0x00000007
#define SPI_MEM_DIN4_MODE_M  ((SPI_MEM_DIN4_MODE_V)<<(SPI_MEM_DIN4_MODE_S))
#define SPI_MEM_DIN4_MODE_V  0x7
#define SPI_MEM_DIN4_MODE_S  12
/* SPI_MEM_DIN3_MODE : R/W ;bitpos:[11:9] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_DIN3_MODE  0x00000007
#define SPI_MEM_DIN3_MODE_M  ((SPI_MEM_DIN3_MODE_V)<<(SPI_MEM_DIN3_MODE_S))
#define SPI_MEM_DIN3_MODE_V  0x7
#define SPI_MEM_DIN3_MODE_S  9
/* SPI_MEM_DIN2_MODE : R/W ;bitpos:[8:6] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_DIN2_MODE  0x00000007
#define SPI_MEM_DIN2_MODE_M  ((SPI_MEM_DIN2_MODE_V)<<(SPI_MEM_DIN2_MODE_S))
#define SPI_MEM_DIN2_MODE_V  0x7
#define SPI_MEM_DIN2_MODE_S  6
/* SPI_MEM_DIN1_MODE : R/W ;bitpos:[5:3] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_DIN1_MODE  0x00000007
#define SPI_MEM_DIN1_MODE_M  ((SPI_MEM_DIN1_MODE_V)<<(SPI_MEM_DIN1_MODE_S))
#define SPI_MEM_DIN1_MODE_V  0x7
#define SPI_MEM_DIN1_MODE_S  3
/* SPI_MEM_DIN0_MODE : R/W ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_DIN0_MODE  0x00000007
#define SPI_MEM_DIN0_MODE_M  ((SPI_MEM_DIN0_MODE_V)<<(SPI_MEM_DIN0_MODE_S))
#define SPI_MEM_DIN0_MODE_V  0x7
#define SPI_MEM_DIN0_MODE_S  0

#define SPI_MEM_DIN_NUM_REG(i)          (REG_SPI_MEM_BASE(i) + 0x0B0)
/* SPI_MEM_DINS_NUM : R/W ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DINS_NUM  0x00000003
#define SPI_MEM_DINS_NUM_M  ((SPI_MEM_DINS_NUM_V)<<(SPI_MEM_DINS_NUM_S))
#define SPI_MEM_DINS_NUM_V  0x3
#define SPI_MEM_DINS_NUM_S  16
/* SPI_MEM_DIN7_NUM : R/W ;bitpos:[15:14] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DIN7_NUM  0x00000003
#define SPI_MEM_DIN7_NUM_M  ((SPI_MEM_DIN7_NUM_V)<<(SPI_MEM_DIN7_NUM_S))
#define SPI_MEM_DIN7_NUM_V  0x3
#define SPI_MEM_DIN7_NUM_S  14
/* SPI_MEM_DIN6_NUM : R/W ;bitpos:[13:12] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DIN6_NUM  0x00000003
#define SPI_MEM_DIN6_NUM_M  ((SPI_MEM_DIN6_NUM_V)<<(SPI_MEM_DIN6_NUM_S))
#define SPI_MEM_DIN6_NUM_V  0x3
#define SPI_MEM_DIN6_NUM_S  12
/* SPI_MEM_DIN5_NUM : R/W ;bitpos:[11:10] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DIN5_NUM  0x00000003
#define SPI_MEM_DIN5_NUM_M  ((SPI_MEM_DIN5_NUM_V)<<(SPI_MEM_DIN5_NUM_S))
#define SPI_MEM_DIN5_NUM_V  0x3
#define SPI_MEM_DIN5_NUM_S  10
/* SPI_MEM_DIN4_NUM : R/W ;bitpos:[9:8] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DIN4_NUM  0x00000003
#define SPI_MEM_DIN4_NUM_M  ((SPI_MEM_DIN4_NUM_V)<<(SPI_MEM_DIN4_NUM_S))
#define SPI_MEM_DIN4_NUM_V  0x3
#define SPI_MEM_DIN4_NUM_S  8
/* SPI_MEM_DIN3_NUM : R/W ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DIN3_NUM  0x00000003
#define SPI_MEM_DIN3_NUM_M  ((SPI_MEM_DIN3_NUM_V)<<(SPI_MEM_DIN3_NUM_S))
#define SPI_MEM_DIN3_NUM_V  0x3
#define SPI_MEM_DIN3_NUM_S  6
/* SPI_MEM_DIN2_NUM : R/W ;bitpos:[5:4] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DIN2_NUM  0x00000003
#define SPI_MEM_DIN2_NUM_M  ((SPI_MEM_DIN2_NUM_V)<<(SPI_MEM_DIN2_NUM_S))
#define SPI_MEM_DIN2_NUM_V  0x3
#define SPI_MEM_DIN2_NUM_S  4
/* SPI_MEM_DIN1_NUM : R/W ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DIN1_NUM  0x00000003
#define SPI_MEM_DIN1_NUM_M  ((SPI_MEM_DIN1_NUM_V)<<(SPI_MEM_DIN1_NUM_S))
#define SPI_MEM_DIN1_NUM_V  0x3
#define SPI_MEM_DIN1_NUM_S  2
/* SPI_MEM_DIN0_NUM : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DIN0_NUM  0x00000003
#define SPI_MEM_DIN0_NUM_M  ((SPI_MEM_DIN0_NUM_V)<<(SPI_MEM_DIN0_NUM_S))
#define SPI_MEM_DIN0_NUM_V  0x3
#define SPI_MEM_DIN0_NUM_S  0

#define SPI_MEM_DOUT_MODE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x0B4)
/* SPI_MEM_DOUTS_MODE : R/W ;bitpos:[26:24] ;default: 3'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
#define SPI_MEM_DOUTS_MODE  0x00000007
#define SPI_MEM_DOUTS_MODE_M  ((SPI_MEM_DOUTS_MODE_V)<<(SPI_MEM_DOUTS_MODE_S))
#define SPI_MEM_DOUTS_MODE_V  0x7
#define SPI_MEM_DOUTS_MODE_S  24
/* SPI_MEM_DOUT7_MODE : R/W ;bitpos:[23:21] ;default: 3'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
#define SPI_MEM_DOUT7_MODE  0x00000007
#define SPI_MEM_DOUT7_MODE_M  ((SPI_MEM_DOUT7_MODE_V)<<(SPI_MEM_DOUT7_MODE_S))
#define SPI_MEM_DOUT7_MODE_V  0x7
#define SPI_MEM_DOUT7_MODE_S  21
/* SPI_MEM_DOUT6_MODE : R/W ;bitpos:[20:18] ;default: 3'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
#define SPI_MEM_DOUT6_MODE  0x00000007
#define SPI_MEM_DOUT6_MODE_M  ((SPI_MEM_DOUT6_MODE_V)<<(SPI_MEM_DOUT6_MODE_S))
#define SPI_MEM_DOUT6_MODE_V  0x7
#define SPI_MEM_DOUT6_MODE_S  18
/* SPI_MEM_DOUT5_MODE : R/W ;bitpos:[17:15] ;default: 3'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
#define SPI_MEM_DOUT5_MODE  0x00000007
#define SPI_MEM_DOUT5_MODE_M  ((SPI_MEM_DOUT5_MODE_V)<<(SPI_MEM_DOUT5_MODE_S))
#define SPI_MEM_DOUT5_MODE_V  0x7
#define SPI_MEM_DOUT5_MODE_S  15
/* SPI_MEM_DOUT4_MODE : R/W ;bitpos:[14:12] ;default: 3'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
#define SPI_MEM_DOUT4_MODE  0x00000007
#define SPI_MEM_DOUT4_MODE_M  ((SPI_MEM_DOUT4_MODE_V)<<(SPI_MEM_DOUT4_MODE_S))
#define SPI_MEM_DOUT4_MODE_V  0x7
#define SPI_MEM_DOUT4_MODE_S  12
/* SPI_MEM_DOUT3_MODE : R/W ;bitpos:[11:9] ;default: 3'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_DOUT3_MODE  0x00000007
#define SPI_MEM_DOUT3_MODE_M  ((SPI_MEM_DOUT3_MODE_V)<<(SPI_MEM_DOUT3_MODE_S))
#define SPI_MEM_DOUT3_MODE_V  0x7
#define SPI_MEM_DOUT3_MODE_S  9
/* SPI_MEM_DOUT2_MODE : R/W ;bitpos:[8:6] ;default: 3'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_DOUT2_MODE  0x00000007
#define SPI_MEM_DOUT2_MODE_M  ((SPI_MEM_DOUT2_MODE_V)<<(SPI_MEM_DOUT2_MODE_S))
#define SPI_MEM_DOUT2_MODE_V  0x7
#define SPI_MEM_DOUT2_MODE_S  6
/* SPI_MEM_DOUT1_MODE : R/W ;bitpos:[5:3] ;default: 3'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_DOUT1_MODE  0x00000007
#define SPI_MEM_DOUT1_MODE_M  ((SPI_MEM_DOUT1_MODE_V)<<(SPI_MEM_DOUT1_MODE_S))
#define SPI_MEM_DOUT1_MODE_V  0x7
#define SPI_MEM_DOUT1_MODE_S  3
/* SPI_MEM_DOUT0_MODE : R/W ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_DOUT0_MODE  0x00000007
#define SPI_MEM_DOUT0_MODE_M  ((SPI_MEM_DOUT0_MODE_V)<<(SPI_MEM_DOUT0_MODE_S))
#define SPI_MEM_DOUT0_MODE_V  0x7
#define SPI_MEM_DOUT0_MODE_S  0

#define SPI_MEM_DOUT_NUM_REG(i)          (REG_SPI_MEM_BASE(i) + 0x0B8)
/* SPI_MEM_DOUTS_NUM : R/W ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DOUTS_NUM  0x00000003
#define SPI_MEM_DOUTS_NUM_M  ((SPI_MEM_DOUTS_NUM_V)<<(SPI_MEM_DOUTS_NUM_S))
#define SPI_MEM_DOUTS_NUM_V  0x3
#define SPI_MEM_DOUTS_NUM_S  16
/* SPI_MEM_DOUT7_NUM : R/W ;bitpos:[15:14] ;default: 2'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DOUT7_NUM  0x00000003
#define SPI_MEM_DOUT7_NUM_M  ((SPI_MEM_DOUT7_NUM_V)<<(SPI_MEM_DOUT7_NUM_S))
#define SPI_MEM_DOUT7_NUM_V  0x3
#define SPI_MEM_DOUT7_NUM_S  14
/* SPI_MEM_DOUT6_NUM : R/W ;bitpos:[13:12] ;default: 2'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DOUT6_NUM  0x00000003
#define SPI_MEM_DOUT6_NUM_M  ((SPI_MEM_DOUT6_NUM_V)<<(SPI_MEM_DOUT6_NUM_S))
#define SPI_MEM_DOUT6_NUM_V  0x3
#define SPI_MEM_DOUT6_NUM_S  12
/* SPI_MEM_DOUT5_NUM : R/W ;bitpos:[11:10] ;default: 2'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DOUT5_NUM  0x00000003
#define SPI_MEM_DOUT5_NUM_M  ((SPI_MEM_DOUT5_NUM_V)<<(SPI_MEM_DOUT5_NUM_S))
#define SPI_MEM_DOUT5_NUM_V  0x3
#define SPI_MEM_DOUT5_NUM_S  10
/* SPI_MEM_DOUT4_NUM : R/W ;bitpos:[9:8] ;default: 2'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DOUT4_NUM  0x00000003
#define SPI_MEM_DOUT4_NUM_M  ((SPI_MEM_DOUT4_NUM_V)<<(SPI_MEM_DOUT4_NUM_S))
#define SPI_MEM_DOUT4_NUM_V  0x3
#define SPI_MEM_DOUT4_NUM_S  8
/* SPI_MEM_DOUT3_NUM : R/W ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DOUT3_NUM  0x00000003
#define SPI_MEM_DOUT3_NUM_M  ((SPI_MEM_DOUT3_NUM_V)<<(SPI_MEM_DOUT3_NUM_S))
#define SPI_MEM_DOUT3_NUM_V  0x3
#define SPI_MEM_DOUT3_NUM_S  6
/* SPI_MEM_DOUT2_NUM : R/W ;bitpos:[5:4] ;default: 2'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DOUT2_NUM  0x00000003
#define SPI_MEM_DOUT2_NUM_M  ((SPI_MEM_DOUT2_NUM_V)<<(SPI_MEM_DOUT2_NUM_S))
#define SPI_MEM_DOUT2_NUM_V  0x3
#define SPI_MEM_DOUT2_NUM_S  4
/* SPI_MEM_DOUT1_NUM : R/W ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DOUT1_NUM  0x00000003
#define SPI_MEM_DOUT1_NUM_M  ((SPI_MEM_DOUT1_NUM_V)<<(SPI_MEM_DOUT1_NUM_S))
#define SPI_MEM_DOUT1_NUM_V  0x3
#define SPI_MEM_DOUT1_NUM_S  2
/* SPI_MEM_DOUT0_NUM : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DOUT0_NUM  0x00000003
#define SPI_MEM_DOUT0_NUM_M  ((SPI_MEM_DOUT0_NUM_V)<<(SPI_MEM_DOUT0_NUM_S))
#define SPI_MEM_DOUT0_NUM_V  0x3
#define SPI_MEM_DOUT0_NUM_S  0

#define SPI_MEM_SPI_SMEM_TIMING_CALI_REG(i)          (REG_SPI_MEM_BASE(i) + 0x0BC)
/* SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: For sram  add extra dummy spi clock cycle length for spi clock calibration.*/
#define SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN  0x00000003
#define SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_M  ((SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_V)<<(SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_S))
#define SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_V  0x3
#define SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_S  2
/* SPI_MEM_SPI_SMEM_TIMING_CALI : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: For sram  the bit is used to enable timing auto-calibration for
 all reading operations.*/
#define SPI_MEM_SPI_SMEM_TIMING_CALI  (BIT(1))
#define SPI_MEM_SPI_SMEM_TIMING_CALI_M  (BIT(1))
#define SPI_MEM_SPI_SMEM_TIMING_CALI_V  0x1
#define SPI_MEM_SPI_SMEM_TIMING_CALI_S  1
/* SPI_MEM_SPI_SMEM_TIMING_CLK_ENA : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: For sram  the bit is used to enable timing adjust clock for all
 reading operations.*/
#define SPI_MEM_SPI_SMEM_TIMING_CLK_ENA  (BIT(0))
#define SPI_MEM_SPI_SMEM_TIMING_CLK_ENA_M  (BIT(0))
#define SPI_MEM_SPI_SMEM_TIMING_CLK_ENA_V  0x1
#define SPI_MEM_SPI_SMEM_TIMING_CLK_ENA_S  0

#define SPI_MEM_SPI_SMEM_DIN_MODE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x0C0)
/* SPI_MEM_SPI_SMEM_DINS_MODE : R/W ;bitpos:[26:24] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DINS_MODE  0x00000007
#define SPI_MEM_SPI_SMEM_DINS_MODE_M  ((SPI_MEM_SPI_SMEM_DINS_MODE_V)<<(SPI_MEM_SPI_SMEM_DINS_MODE_S))
#define SPI_MEM_SPI_SMEM_DINS_MODE_V  0x7
#define SPI_MEM_SPI_SMEM_DINS_MODE_S  24
/* SPI_MEM_SPI_SMEM_DIN7_MODE : R/W ;bitpos:[23:21] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DIN7_MODE  0x00000007
#define SPI_MEM_SPI_SMEM_DIN7_MODE_M  ((SPI_MEM_SPI_SMEM_DIN7_MODE_V)<<(SPI_MEM_SPI_SMEM_DIN7_MODE_S))
#define SPI_MEM_SPI_SMEM_DIN7_MODE_V  0x7
#define SPI_MEM_SPI_SMEM_DIN7_MODE_S  21
/* SPI_MEM_SPI_SMEM_DIN6_MODE : R/W ;bitpos:[20:18] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DIN6_MODE  0x00000007
#define SPI_MEM_SPI_SMEM_DIN6_MODE_M  ((SPI_MEM_SPI_SMEM_DIN6_MODE_V)<<(SPI_MEM_SPI_SMEM_DIN6_MODE_S))
#define SPI_MEM_SPI_SMEM_DIN6_MODE_V  0x7
#define SPI_MEM_SPI_SMEM_DIN6_MODE_S  18
/* SPI_MEM_SPI_SMEM_DIN5_MODE : R/W ;bitpos:[17:15] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DIN5_MODE  0x00000007
#define SPI_MEM_SPI_SMEM_DIN5_MODE_M  ((SPI_MEM_SPI_SMEM_DIN5_MODE_V)<<(SPI_MEM_SPI_SMEM_DIN5_MODE_S))
#define SPI_MEM_SPI_SMEM_DIN5_MODE_V  0x7
#define SPI_MEM_SPI_SMEM_DIN5_MODE_S  15
/* SPI_MEM_SPI_SMEM_DIN4_MODE : R/W ;bitpos:[14:12] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DIN4_MODE  0x00000007
#define SPI_MEM_SPI_SMEM_DIN4_MODE_M  ((SPI_MEM_SPI_SMEM_DIN4_MODE_V)<<(SPI_MEM_SPI_SMEM_DIN4_MODE_S))
#define SPI_MEM_SPI_SMEM_DIN4_MODE_V  0x7
#define SPI_MEM_SPI_SMEM_DIN4_MODE_S  12
/* SPI_MEM_SPI_SMEM_DIN3_MODE : R/W ;bitpos:[11:9] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DIN3_MODE  0x00000007
#define SPI_MEM_SPI_SMEM_DIN3_MODE_M  ((SPI_MEM_SPI_SMEM_DIN3_MODE_V)<<(SPI_MEM_SPI_SMEM_DIN3_MODE_S))
#define SPI_MEM_SPI_SMEM_DIN3_MODE_V  0x7
#define SPI_MEM_SPI_SMEM_DIN3_MODE_S  9
/* SPI_MEM_SPI_SMEM_DIN2_MODE : R/W ;bitpos:[8:6] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DIN2_MODE  0x00000007
#define SPI_MEM_SPI_SMEM_DIN2_MODE_M  ((SPI_MEM_SPI_SMEM_DIN2_MODE_V)<<(SPI_MEM_SPI_SMEM_DIN2_MODE_S))
#define SPI_MEM_SPI_SMEM_DIN2_MODE_V  0x7
#define SPI_MEM_SPI_SMEM_DIN2_MODE_S  6
/* SPI_MEM_SPI_SMEM_DIN1_MODE : R/W ;bitpos:[5:3] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DIN1_MODE  0x00000007
#define SPI_MEM_SPI_SMEM_DIN1_MODE_M  ((SPI_MEM_SPI_SMEM_DIN1_MODE_V)<<(SPI_MEM_SPI_SMEM_DIN1_MODE_S))
#define SPI_MEM_SPI_SMEM_DIN1_MODE_V  0x7
#define SPI_MEM_SPI_SMEM_DIN1_MODE_S  3
/* SPI_MEM_SPI_SMEM_DIN0_MODE : R/W ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DIN0_MODE  0x00000007
#define SPI_MEM_SPI_SMEM_DIN0_MODE_M  ((SPI_MEM_SPI_SMEM_DIN0_MODE_V)<<(SPI_MEM_SPI_SMEM_DIN0_MODE_S))
#define SPI_MEM_SPI_SMEM_DIN0_MODE_V  0x7
#define SPI_MEM_SPI_SMEM_DIN0_MODE_S  0

#define SPI_MEM_SPI_SMEM_DIN_NUM_REG(i)          (REG_SPI_MEM_BASE(i) + 0x0C4)
/* SPI_MEM_SPI_SMEM_DINS_NUM : R/W ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DINS_NUM  0x00000003
#define SPI_MEM_SPI_SMEM_DINS_NUM_M  ((SPI_MEM_SPI_SMEM_DINS_NUM_V)<<(SPI_MEM_SPI_SMEM_DINS_NUM_S))
#define SPI_MEM_SPI_SMEM_DINS_NUM_V  0x3
#define SPI_MEM_SPI_SMEM_DINS_NUM_S  16
/* SPI_MEM_SPI_SMEM_DIN7_NUM : R/W ;bitpos:[15:14] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_SPI_SMEM_DIN7_NUM  0x00000003
#define SPI_MEM_SPI_SMEM_DIN7_NUM_M  ((SPI_MEM_SPI_SMEM_DIN7_NUM_V)<<(SPI_MEM_SPI_SMEM_DIN7_NUM_S))
#define SPI_MEM_SPI_SMEM_DIN7_NUM_V  0x3
#define SPI_MEM_SPI_SMEM_DIN7_NUM_S  14
/* SPI_MEM_SPI_SMEM_DIN6_NUM : R/W ;bitpos:[13:12] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_SPI_SMEM_DIN6_NUM  0x00000003
#define SPI_MEM_SPI_SMEM_DIN6_NUM_M  ((SPI_MEM_SPI_SMEM_DIN6_NUM_V)<<(SPI_MEM_SPI_SMEM_DIN6_NUM_S))
#define SPI_MEM_SPI_SMEM_DIN6_NUM_V  0x3
#define SPI_MEM_SPI_SMEM_DIN6_NUM_S  12
/* SPI_MEM_SPI_SMEM_DIN5_NUM : R/W ;bitpos:[11:10] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_SPI_SMEM_DIN5_NUM  0x00000003
#define SPI_MEM_SPI_SMEM_DIN5_NUM_M  ((SPI_MEM_SPI_SMEM_DIN5_NUM_V)<<(SPI_MEM_SPI_SMEM_DIN5_NUM_S))
#define SPI_MEM_SPI_SMEM_DIN5_NUM_V  0x3
#define SPI_MEM_SPI_SMEM_DIN5_NUM_S  10
/* SPI_MEM_SPI_SMEM_DIN4_NUM : R/W ;bitpos:[9:8] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_SPI_SMEM_DIN4_NUM  0x00000003
#define SPI_MEM_SPI_SMEM_DIN4_NUM_M  ((SPI_MEM_SPI_SMEM_DIN4_NUM_V)<<(SPI_MEM_SPI_SMEM_DIN4_NUM_S))
#define SPI_MEM_SPI_SMEM_DIN4_NUM_V  0x3
#define SPI_MEM_SPI_SMEM_DIN4_NUM_S  8
/* SPI_MEM_SPI_SMEM_DIN3_NUM : R/W ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_SPI_SMEM_DIN3_NUM  0x00000003
#define SPI_MEM_SPI_SMEM_DIN3_NUM_M  ((SPI_MEM_SPI_SMEM_DIN3_NUM_V)<<(SPI_MEM_SPI_SMEM_DIN3_NUM_S))
#define SPI_MEM_SPI_SMEM_DIN3_NUM_V  0x3
#define SPI_MEM_SPI_SMEM_DIN3_NUM_S  6
/* SPI_MEM_SPI_SMEM_DIN2_NUM : R/W ;bitpos:[5:4] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_SPI_SMEM_DIN2_NUM  0x00000003
#define SPI_MEM_SPI_SMEM_DIN2_NUM_M  ((SPI_MEM_SPI_SMEM_DIN2_NUM_V)<<(SPI_MEM_SPI_SMEM_DIN2_NUM_S))
#define SPI_MEM_SPI_SMEM_DIN2_NUM_V  0x3
#define SPI_MEM_SPI_SMEM_DIN2_NUM_S  4
/* SPI_MEM_SPI_SMEM_DIN1_NUM : R/W ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_SPI_SMEM_DIN1_NUM  0x00000003
#define SPI_MEM_SPI_SMEM_DIN1_NUM_M  ((SPI_MEM_SPI_SMEM_DIN1_NUM_V)<<(SPI_MEM_SPI_SMEM_DIN1_NUM_S))
#define SPI_MEM_SPI_SMEM_DIN1_NUM_V  0x3
#define SPI_MEM_SPI_SMEM_DIN1_NUM_S  2
/* SPI_MEM_SPI_SMEM_DIN0_NUM : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_SPI_SMEM_DIN0_NUM  0x00000003
#define SPI_MEM_SPI_SMEM_DIN0_NUM_M  ((SPI_MEM_SPI_SMEM_DIN0_NUM_V)<<(SPI_MEM_SPI_SMEM_DIN0_NUM_S))
#define SPI_MEM_SPI_SMEM_DIN0_NUM_V  0x3
#define SPI_MEM_SPI_SMEM_DIN0_NUM_S  0

#define SPI_MEM_SPI_SMEM_DOUT_MODE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x0C8)
/* SPI_MEM_SPI_SMEM_DOUTS_MODE : R/W ;bitpos:[26:24] ;default: 3'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUTS_MODE  0x00000007
#define SPI_MEM_SPI_SMEM_DOUTS_MODE_M  ((SPI_MEM_SPI_SMEM_DOUTS_MODE_V)<<(SPI_MEM_SPI_SMEM_DOUTS_MODE_S))
#define SPI_MEM_SPI_SMEM_DOUTS_MODE_V  0x7
#define SPI_MEM_SPI_SMEM_DOUTS_MODE_S  24
/* SPI_MEM_SPI_SMEM_DOUT7_MODE : R/W ;bitpos:[23:21] ;default: 3'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUT7_MODE  0x00000007
#define SPI_MEM_SPI_SMEM_DOUT7_MODE_M  ((SPI_MEM_SPI_SMEM_DOUT7_MODE_V)<<(SPI_MEM_SPI_SMEM_DOUT7_MODE_S))
#define SPI_MEM_SPI_SMEM_DOUT7_MODE_V  0x7
#define SPI_MEM_SPI_SMEM_DOUT7_MODE_S  21
/* SPI_MEM_SPI_SMEM_DOUT6_MODE : R/W ;bitpos:[20:18] ;default: 3'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUT6_MODE  0x00000007
#define SPI_MEM_SPI_SMEM_DOUT6_MODE_M  ((SPI_MEM_SPI_SMEM_DOUT6_MODE_V)<<(SPI_MEM_SPI_SMEM_DOUT6_MODE_S))
#define SPI_MEM_SPI_SMEM_DOUT6_MODE_V  0x7
#define SPI_MEM_SPI_SMEM_DOUT6_MODE_S  18
/* SPI_MEM_SPI_SMEM_DOUT5_MODE : R/W ;bitpos:[17:15] ;default: 3'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUT5_MODE  0x00000007
#define SPI_MEM_SPI_SMEM_DOUT5_MODE_M  ((SPI_MEM_SPI_SMEM_DOUT5_MODE_V)<<(SPI_MEM_SPI_SMEM_DOUT5_MODE_S))
#define SPI_MEM_SPI_SMEM_DOUT5_MODE_V  0x7
#define SPI_MEM_SPI_SMEM_DOUT5_MODE_S  15
/* SPI_MEM_SPI_SMEM_DOUT4_MODE : R/W ;bitpos:[14:12] ;default: 3'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUT4_MODE  0x00000007
#define SPI_MEM_SPI_SMEM_DOUT4_MODE_M  ((SPI_MEM_SPI_SMEM_DOUT4_MODE_V)<<(SPI_MEM_SPI_SMEM_DOUT4_MODE_S))
#define SPI_MEM_SPI_SMEM_DOUT4_MODE_V  0x7
#define SPI_MEM_SPI_SMEM_DOUT4_MODE_S  12
/* SPI_MEM_SPI_SMEM_DOUT3_MODE : R/W ;bitpos:[11:9] ;default: 3'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUT3_MODE  0x00000007
#define SPI_MEM_SPI_SMEM_DOUT3_MODE_M  ((SPI_MEM_SPI_SMEM_DOUT3_MODE_V)<<(SPI_MEM_SPI_SMEM_DOUT3_MODE_S))
#define SPI_MEM_SPI_SMEM_DOUT3_MODE_V  0x7
#define SPI_MEM_SPI_SMEM_DOUT3_MODE_S  9
/* SPI_MEM_SPI_SMEM_DOUT2_MODE : R/W ;bitpos:[8:6] ;default: 3'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUT2_MODE  0x00000007
#define SPI_MEM_SPI_SMEM_DOUT2_MODE_M  ((SPI_MEM_SPI_SMEM_DOUT2_MODE_V)<<(SPI_MEM_SPI_SMEM_DOUT2_MODE_S))
#define SPI_MEM_SPI_SMEM_DOUT2_MODE_V  0x7
#define SPI_MEM_SPI_SMEM_DOUT2_MODE_S  6
/* SPI_MEM_SPI_SMEM_DOUT1_MODE : R/W ;bitpos:[5:3] ;default: 3'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUT1_MODE  0x00000007
#define SPI_MEM_SPI_SMEM_DOUT1_MODE_M  ((SPI_MEM_SPI_SMEM_DOUT1_MODE_V)<<(SPI_MEM_SPI_SMEM_DOUT1_MODE_S))
#define SPI_MEM_SPI_SMEM_DOUT1_MODE_V  0x7
#define SPI_MEM_SPI_SMEM_DOUT1_MODE_S  3
/* SPI_MEM_SPI_SMEM_DOUT0_MODE : R/W ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUT0_MODE  0x00000007
#define SPI_MEM_SPI_SMEM_DOUT0_MODE_M  ((SPI_MEM_SPI_SMEM_DOUT0_MODE_V)<<(SPI_MEM_SPI_SMEM_DOUT0_MODE_S))
#define SPI_MEM_SPI_SMEM_DOUT0_MODE_V  0x7
#define SPI_MEM_SPI_SMEM_DOUT0_MODE_S  0

#define SPI_MEM_SPI_SMEM_DOUT_NUM_REG(i)          (REG_SPI_MEM_BASE(i) + 0x0CC)
/* SPI_MEM_SPI_SMEM_DOUTS_NUM : R/W ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUTS_NUM  0x00000003
#define SPI_MEM_SPI_SMEM_DOUTS_NUM_M  ((SPI_MEM_SPI_SMEM_DOUTS_NUM_V)<<(SPI_MEM_SPI_SMEM_DOUTS_NUM_S))
#define SPI_MEM_SPI_SMEM_DOUTS_NUM_V  0x3
#define SPI_MEM_SPI_SMEM_DOUTS_NUM_S  16
/* SPI_MEM_SPI_SMEM_DOUT7_NUM : R/W ;bitpos:[15:14] ;default: 2'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUT7_NUM  0x00000003
#define SPI_MEM_SPI_SMEM_DOUT7_NUM_M  ((SPI_MEM_SPI_SMEM_DOUT7_NUM_V)<<(SPI_MEM_SPI_SMEM_DOUT7_NUM_S))
#define SPI_MEM_SPI_SMEM_DOUT7_NUM_V  0x3
#define SPI_MEM_SPI_SMEM_DOUT7_NUM_S  14
/* SPI_MEM_SPI_SMEM_DOUT6_NUM : R/W ;bitpos:[13:12] ;default: 2'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUT6_NUM  0x00000003
#define SPI_MEM_SPI_SMEM_DOUT6_NUM_M  ((SPI_MEM_SPI_SMEM_DOUT6_NUM_V)<<(SPI_MEM_SPI_SMEM_DOUT6_NUM_S))
#define SPI_MEM_SPI_SMEM_DOUT6_NUM_V  0x3
#define SPI_MEM_SPI_SMEM_DOUT6_NUM_S  12
/* SPI_MEM_SPI_SMEM_DOUT5_NUM : R/W ;bitpos:[11:10] ;default: 2'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUT5_NUM  0x00000003
#define SPI_MEM_SPI_SMEM_DOUT5_NUM_M  ((SPI_MEM_SPI_SMEM_DOUT5_NUM_V)<<(SPI_MEM_SPI_SMEM_DOUT5_NUM_S))
#define SPI_MEM_SPI_SMEM_DOUT5_NUM_V  0x3
#define SPI_MEM_SPI_SMEM_DOUT5_NUM_S  10
/* SPI_MEM_SPI_SMEM_DOUT4_NUM : R/W ;bitpos:[9:8] ;default: 2'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUT4_NUM  0x00000003
#define SPI_MEM_SPI_SMEM_DOUT4_NUM_M  ((SPI_MEM_SPI_SMEM_DOUT4_NUM_V)<<(SPI_MEM_SPI_SMEM_DOUT4_NUM_S))
#define SPI_MEM_SPI_SMEM_DOUT4_NUM_V  0x3
#define SPI_MEM_SPI_SMEM_DOUT4_NUM_S  8
/* SPI_MEM_SPI_SMEM_DOUT3_NUM : R/W ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_SPI_SMEM_DOUT3_NUM  0x00000003
#define SPI_MEM_SPI_SMEM_DOUT3_NUM_M  ((SPI_MEM_SPI_SMEM_DOUT3_NUM_V)<<(SPI_MEM_SPI_SMEM_DOUT3_NUM_S))
#define SPI_MEM_SPI_SMEM_DOUT3_NUM_V  0x3
#define SPI_MEM_SPI_SMEM_DOUT3_NUM_S  6
/* SPI_MEM_SPI_SMEM_DOUT2_NUM : R/W ;bitpos:[5:4] ;default: 2'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_SPI_SMEM_DOUT2_NUM  0x00000003
#define SPI_MEM_SPI_SMEM_DOUT2_NUM_M  ((SPI_MEM_SPI_SMEM_DOUT2_NUM_V)<<(SPI_MEM_SPI_SMEM_DOUT2_NUM_S))
#define SPI_MEM_SPI_SMEM_DOUT2_NUM_V  0x3
#define SPI_MEM_SPI_SMEM_DOUT2_NUM_S  4
/* SPI_MEM_SPI_SMEM_DOUT1_NUM : R/W ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_SPI_SMEM_DOUT1_NUM  0x00000003
#define SPI_MEM_SPI_SMEM_DOUT1_NUM_M  ((SPI_MEM_SPI_SMEM_DOUT1_NUM_V)<<(SPI_MEM_SPI_SMEM_DOUT1_NUM_S))
#define SPI_MEM_SPI_SMEM_DOUT1_NUM_V  0x3
#define SPI_MEM_SPI_SMEM_DOUT1_NUM_S  2
/* SPI_MEM_SPI_SMEM_DOUT0_NUM : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_SPI_SMEM_DOUT0_NUM  0x00000003
#define SPI_MEM_SPI_SMEM_DOUT0_NUM_M  ((SPI_MEM_SPI_SMEM_DOUT0_NUM_V)<<(SPI_MEM_SPI_SMEM_DOUT0_NUM_S))
#define SPI_MEM_SPI_SMEM_DOUT0_NUM_V  0x3
#define SPI_MEM_SPI_SMEM_DOUT0_NUM_S  0

#define SPI_MEM_SPI_SMEM_AC_REG(i)          (REG_SPI_MEM_BASE(i) + 0x0D0)
/* SPI_MEM_SPI_SMEM_CS_HOLD_TIME : R/W ;bitpos:[27:15] ;default: 13'h1 ; */
/*description: For spi0  spi cs signal is delayed to inactive by spi clock this
 bits are combined with spi_mem_cs_hold bit.*/
#define SPI_MEM_SPI_SMEM_CS_HOLD_TIME  0x00001FFF
#define SPI_MEM_SPI_SMEM_CS_HOLD_TIME_M  ((SPI_MEM_SPI_SMEM_CS_HOLD_TIME_V)<<(SPI_MEM_SPI_SMEM_CS_HOLD_TIME_S))
#define SPI_MEM_SPI_SMEM_CS_HOLD_TIME_V  0x1FFF
#define SPI_MEM_SPI_SMEM_CS_HOLD_TIME_S  15
/* SPI_MEM_SPI_SMEM_CS_SETUP_TIME : R/W ;bitpos:[14:2] ;default: 13'h1 ; */
/*description: For spi0  (cycles-1) of prepare phase by spi clock this bits
 are combined with spi_mem_cs_setup bit.*/
#define SPI_MEM_SPI_SMEM_CS_SETUP_TIME  0x00001FFF
#define SPI_MEM_SPI_SMEM_CS_SETUP_TIME_M  ((SPI_MEM_SPI_SMEM_CS_SETUP_TIME_V)<<(SPI_MEM_SPI_SMEM_CS_SETUP_TIME_S))
#define SPI_MEM_SPI_SMEM_CS_SETUP_TIME_V  0x1FFF
#define SPI_MEM_SPI_SMEM_CS_SETUP_TIME_S  2
/* SPI_MEM_SPI_SMEM_CS_HOLD : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: For spi0  spi cs keep low when spi is in done phase. 1: enable 0: disable.*/
#define SPI_MEM_SPI_SMEM_CS_HOLD  (BIT(1))
#define SPI_MEM_SPI_SMEM_CS_HOLD_M  (BIT(1))
#define SPI_MEM_SPI_SMEM_CS_HOLD_V  0x1
#define SPI_MEM_SPI_SMEM_CS_HOLD_S  1
/* SPI_MEM_SPI_SMEM_CS_SETUP : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: For spi0  spi cs is enable when spi is in prepare phase. 1: enable 0: disable.*/
#define SPI_MEM_SPI_SMEM_CS_SETUP  (BIT(0))
#define SPI_MEM_SPI_SMEM_CS_SETUP_M  (BIT(0))
#define SPI_MEM_SPI_SMEM_CS_SETUP_V  0x1
#define SPI_MEM_SPI_SMEM_CS_SETUP_S  0

#define SPI_MEM_DDR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x0D4)
/* SPI_MEM_SPI_FMEM_DDR_DQS_LOOP_MODE : R/W ;bitpos:[23:22] ;default: 2'b0 ; */
/*description: the bits are combined with the bit spi_fmem_ddr_fdqs_loop which
 used to select data strobe generating mode in ddr mode.*/
#define SPI_MEM_SPI_FMEM_DDR_DQS_LOOP_MODE  0x00000003
#define SPI_MEM_SPI_FMEM_DDR_DQS_LOOP_MODE_M  ((SPI_MEM_SPI_FMEM_DDR_DQS_LOOP_MODE_V)<<(SPI_MEM_SPI_FMEM_DDR_DQS_LOOP_MODE_S))
#define SPI_MEM_SPI_FMEM_DDR_DQS_LOOP_MODE_V  0x3
#define SPI_MEM_SPI_FMEM_DDR_DQS_LOOP_MODE_S  22
/* SPI_MEM_SPI_FMEM_DDR_DQS_LOOP : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: the data strobe is generated by SPI.*/
#define SPI_MEM_SPI_FMEM_DDR_DQS_LOOP  (BIT(21))
#define SPI_MEM_SPI_FMEM_DDR_DQS_LOOP_M  (BIT(21))
#define SPI_MEM_SPI_FMEM_DDR_DQS_LOOP_V  0x1
#define SPI_MEM_SPI_FMEM_DDR_DQS_LOOP_S  21
/* SPI_MEM_SPI_FMEM_USR_DDR_DQS_THD : R/W ;bitpos:[20:13] ;default: 8'b0 ; */
/*description: The delay number of data strobe which from memory based on SPI clock.*/
#define SPI_MEM_SPI_FMEM_USR_DDR_DQS_THD  0x000000FF
#define SPI_MEM_SPI_FMEM_USR_DDR_DQS_THD_M  ((SPI_MEM_SPI_FMEM_USR_DDR_DQS_THD_V)<<(SPI_MEM_SPI_FMEM_USR_DDR_DQS_THD_S))
#define SPI_MEM_SPI_FMEM_USR_DDR_DQS_THD_V  0xFF
#define SPI_MEM_SPI_FMEM_USR_DDR_DQS_THD_S  13
/* SPI_MEM_SPI_FMEM_OUTMINBYTELEN : R/W ;bitpos:[12:5] ;default: 8'b1 ; */
/*description: It is the minimum output data length in the panda device.*/
#define SPI_MEM_SPI_FMEM_OUTMINBYTELEN  0x000000FF
#define SPI_MEM_SPI_FMEM_OUTMINBYTELEN_M  ((SPI_MEM_SPI_FMEM_OUTMINBYTELEN_V)<<(SPI_MEM_SPI_FMEM_OUTMINBYTELEN_S))
#define SPI_MEM_SPI_FMEM_OUTMINBYTELEN_V  0xFF
#define SPI_MEM_SPI_FMEM_OUTMINBYTELEN_S  5
/* SPI_MEM_SPI_FMEM_DDR_CMD_DIS : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: the bit is used to disable dual edge in command phase when ddr mode.*/
#define SPI_MEM_SPI_FMEM_DDR_CMD_DIS  (BIT(4))
#define SPI_MEM_SPI_FMEM_DDR_CMD_DIS_M  (BIT(4))
#define SPI_MEM_SPI_FMEM_DDR_CMD_DIS_V  0x1
#define SPI_MEM_SPI_FMEM_DDR_CMD_DIS_S  4
/* SPI_MEM_SPI_FMEM_DDR_WDAT_SWP : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set the bit to reorder tx data of the word in spi ddr mode.*/
#define SPI_MEM_SPI_FMEM_DDR_WDAT_SWP  (BIT(3))
#define SPI_MEM_SPI_FMEM_DDR_WDAT_SWP_M  (BIT(3))
#define SPI_MEM_SPI_FMEM_DDR_WDAT_SWP_V  0x1
#define SPI_MEM_SPI_FMEM_DDR_WDAT_SWP_S  3
/* SPI_MEM_SPI_FMEM_DDR_RDAT_SWP : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set the bit to reorder rx data of the word in spi ddr mode.*/
#define SPI_MEM_SPI_FMEM_DDR_RDAT_SWP  (BIT(2))
#define SPI_MEM_SPI_FMEM_DDR_RDAT_SWP_M  (BIT(2))
#define SPI_MEM_SPI_FMEM_DDR_RDAT_SWP_V  0x1
#define SPI_MEM_SPI_FMEM_DDR_RDAT_SWP_S  2
/* SPI_MEM_SPI_FMEM_VAR_DUMMY : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set the bit to enable variable dummy cycle in spi ddr mode.*/
#define SPI_MEM_SPI_FMEM_VAR_DUMMY  (BIT(1))
#define SPI_MEM_SPI_FMEM_VAR_DUMMY_M  (BIT(1))
#define SPI_MEM_SPI_FMEM_VAR_DUMMY_V  0x1
#define SPI_MEM_SPI_FMEM_VAR_DUMMY_S  1
/* SPI_MEM_SPI_FMEM_DDR_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: 1: in ddr mode   0 in sdr mode*/
#define SPI_MEM_SPI_FMEM_DDR_EN  (BIT(0))
#define SPI_MEM_SPI_FMEM_DDR_EN_M  (BIT(0))
#define SPI_MEM_SPI_FMEM_DDR_EN_V  0x1
#define SPI_MEM_SPI_FMEM_DDR_EN_S  0

#define SPI_MEM_SPI_SMEM_DDR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x0D8)
/* SPI_MEM_SPI_SMEM_DDR_DQS_LOOP_MODE : R/W ;bitpos:[23:22] ;default: 2'b0 ; */
/*description: the bits are combined with the bit spi_smem_ddr_fdqs_loop which
 used to select data strobe generating mode in ddr mode.*/
#define SPI_MEM_SPI_SMEM_DDR_DQS_LOOP_MODE  0x00000003
#define SPI_MEM_SPI_SMEM_DDR_DQS_LOOP_MODE_M  ((SPI_MEM_SPI_SMEM_DDR_DQS_LOOP_MODE_V)<<(SPI_MEM_SPI_SMEM_DDR_DQS_LOOP_MODE_S))
#define SPI_MEM_SPI_SMEM_DDR_DQS_LOOP_MODE_V  0x3
#define SPI_MEM_SPI_SMEM_DDR_DQS_LOOP_MODE_S  22
/* SPI_MEM_SPI_SMEM_DDR_DQS_LOOP : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: the data strobe is generated by SPI.*/
#define SPI_MEM_SPI_SMEM_DDR_DQS_LOOP  (BIT(21))
#define SPI_MEM_SPI_SMEM_DDR_DQS_LOOP_M  (BIT(21))
#define SPI_MEM_SPI_SMEM_DDR_DQS_LOOP_V  0x1
#define SPI_MEM_SPI_SMEM_DDR_DQS_LOOP_S  21
/* SPI_MEM_SPI_SMEM_USR_DDR_DQS_THD : R/W ;bitpos:[20:13] ;default: 8'b0 ; */
/*description: The delay number of data strobe which from memory based on SPI clock.*/
#define SPI_MEM_SPI_SMEM_USR_DDR_DQS_THD  0x000000FF
#define SPI_MEM_SPI_SMEM_USR_DDR_DQS_THD_M  ((SPI_MEM_SPI_SMEM_USR_DDR_DQS_THD_V)<<(SPI_MEM_SPI_SMEM_USR_DDR_DQS_THD_S))
#define SPI_MEM_SPI_SMEM_USR_DDR_DQS_THD_V  0xFF
#define SPI_MEM_SPI_SMEM_USR_DDR_DQS_THD_S  13
/* SPI_MEM_SPI_SMEM_OUTMINBYTELEN : R/W ;bitpos:[12:5] ;default: 8'b1 ; */
/*description: It is the minimum output data length in the ddr psram.*/
#define SPI_MEM_SPI_SMEM_OUTMINBYTELEN  0x000000FF
#define SPI_MEM_SPI_SMEM_OUTMINBYTELEN_M  ((SPI_MEM_SPI_SMEM_OUTMINBYTELEN_V)<<(SPI_MEM_SPI_SMEM_OUTMINBYTELEN_S))
#define SPI_MEM_SPI_SMEM_OUTMINBYTELEN_V  0xFF
#define SPI_MEM_SPI_SMEM_OUTMINBYTELEN_S  5
/* SPI_MEM_SPI_SMEM_DDR_CMD_DIS : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: the bit is used to disable dual edge in command phase when ddr mode.*/
#define SPI_MEM_SPI_SMEM_DDR_CMD_DIS  (BIT(4))
#define SPI_MEM_SPI_SMEM_DDR_CMD_DIS_M  (BIT(4))
#define SPI_MEM_SPI_SMEM_DDR_CMD_DIS_V  0x1
#define SPI_MEM_SPI_SMEM_DDR_CMD_DIS_S  4
/* SPI_MEM_SPI_SMEM_DDR_WDAT_SWP : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set the bit to reorder tx data of the word in spi ddr mode.*/
#define SPI_MEM_SPI_SMEM_DDR_WDAT_SWP  (BIT(3))
#define SPI_MEM_SPI_SMEM_DDR_WDAT_SWP_M  (BIT(3))
#define SPI_MEM_SPI_SMEM_DDR_WDAT_SWP_V  0x1
#define SPI_MEM_SPI_SMEM_DDR_WDAT_SWP_S  3
/* SPI_MEM_SPI_SMEM_DDR_RDAT_SWP : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set the bit to reorder rx data of the word in spi ddr mode.*/
#define SPI_MEM_SPI_SMEM_DDR_RDAT_SWP  (BIT(2))
#define SPI_MEM_SPI_SMEM_DDR_RDAT_SWP_M  (BIT(2))
#define SPI_MEM_SPI_SMEM_DDR_RDAT_SWP_V  0x1
#define SPI_MEM_SPI_SMEM_DDR_RDAT_SWP_S  2
/* SPI_MEM_SPI_SMEM_VAR_DUMMY : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set the bit to enable variable dummy cycle in spi ddr mode.*/
#define SPI_MEM_SPI_SMEM_VAR_DUMMY  (BIT(1))
#define SPI_MEM_SPI_SMEM_VAR_DUMMY_M  (BIT(1))
#define SPI_MEM_SPI_SMEM_VAR_DUMMY_V  0x1
#define SPI_MEM_SPI_SMEM_VAR_DUMMY_S  1
/* SPI_MEM_SPI_SMEM_DDR_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: 1: in ddr mode   0 in sdr mode*/
#define SPI_MEM_SPI_SMEM_DDR_EN  (BIT(0))
#define SPI_MEM_SPI_SMEM_DDR_EN_M  (BIT(0))
#define SPI_MEM_SPI_SMEM_DDR_EN_V  0x1
#define SPI_MEM_SPI_SMEM_DDR_EN_S  0

#define SPI_MEM_CLOCK_GATE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x0DC)
/* SPI_MEM_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define SPI_MEM_CLK_EN  (BIT(0))
#define SPI_MEM_CLK_EN_M  (BIT(0))
#define SPI_MEM_CLK_EN_V  0x1
#define SPI_MEM_CLK_EN_S  0

#define SPI_MEM_DATE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x3FC)
/* SPI_MEM_DATE : R/W ;bitpos:[27:0] ;default: 28'h1905270 ; */
/*description: SPI register version.*/
#define SPI_MEM_DATE  0x0FFFFFFF
#define SPI_MEM_DATE_M  ((SPI_MEM_DATE_V)<<(SPI_MEM_DATE_S))
#define SPI_MEM_DATE_V  0xFFFFFFF
#define SPI_MEM_DATE_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_SPI_MEM_REG_H_ */
