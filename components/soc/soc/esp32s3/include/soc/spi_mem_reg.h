// Copyright 2017-2020 Espressif Systems (Shanghai) PTE LTD
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
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "soc.h"

#define SPI_MEM_ADDR_REG(i) (REG_SPI_MEM_BASE(i) + 0x004)
/* SPI_MEM_USR_ADDR_VALUE : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: In user mode  it is the memory address. other then the bit0-bit23
 is the memory address  the bit24-bit31 are the byte length of a transfer.*/
#define SPI_MEM_USR_ADDR_VALUE 0xFFFFFFFF
#define SPI_MEM_USR_ADDR_VALUE_M ((SPI_MEM_USR_ADDR_VALUE_V) << (SPI_MEM_USR_ADDR_VALUE_S))
#define SPI_MEM_USR_ADDR_VALUE_V 0xFFFFFFFF
#define SPI_MEM_USR_ADDR_VALUE_S 0

#define SPI_MEM_CTRL_REG(i) (REG_SPI_MEM_BASE(i) + 0x008)
/* SPI_MEM_FREAD_QIO : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: In the read operations address phase and read-data phase apply
 4 signals. 1: enable 0: disable.*/
#define SPI_MEM_FREAD_QIO (BIT(24))
#define SPI_MEM_FREAD_QIO_M (BIT(24))
#define SPI_MEM_FREAD_QIO_V 0x1
#define SPI_MEM_FREAD_QIO_S 24
/* SPI_MEM_FREAD_DIO : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: In the read operations address phase and read-data phase apply
 2 signals. 1: enable 0: disable.*/
#define SPI_MEM_FREAD_DIO (BIT(23))
#define SPI_MEM_FREAD_DIO_M (BIT(23))
#define SPI_MEM_FREAD_DIO_V 0x1
#define SPI_MEM_FREAD_DIO_S 23
/* SPI_MEM_WP_REG : R/W ;bitpos:[21] ;default: 1'b1 ; */
/*description: Write protect signal output when SPI is idle.  1: output high  0: output low.*/
#define SPI_MEM_WP_REG (BIT(21))
#define SPI_MEM_WP_REG_M (BIT(21))
#define SPI_MEM_WP_REG_V 0x1
#define SPI_MEM_WP_REG_S 21
/* SPI_MEM_FREAD_QUAD : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: In the read operations read-data phase apply 4 signals. 1: enable 0: disable.*/
#define SPI_MEM_FREAD_QUAD (BIT(20))
#define SPI_MEM_FREAD_QUAD_M (BIT(20))
#define SPI_MEM_FREAD_QUAD_V 0x1
#define SPI_MEM_FREAD_QUAD_S 20
/* SPI_MEM_D_POL : R/W ;bitpos:[19] ;default: 1'b1 ; */
/*description: The bit is used to set MOSI line polarity  1: high 0  low*/
#define SPI_MEM_D_POL (BIT(19))
#define SPI_MEM_D_POL_M (BIT(19))
#define SPI_MEM_D_POL_V 0x1
#define SPI_MEM_D_POL_S 19
/* SPI_MEM_Q_POL : R/W ;bitpos:[18] ;default: 1'b1 ; */
/*description: The bit is used to set MISO line polarity  1: high 0  low*/
#define SPI_MEM_Q_POL (BIT(18))
#define SPI_MEM_Q_POL_M (BIT(18))
#define SPI_MEM_Q_POL_V 0x1
#define SPI_MEM_Q_POL_S 18
/* SPI_MEM_FREAD_DUAL : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: In the read operations  read-data phase apply 2 signals. 1: enable 0: disable.*/
#define SPI_MEM_FREAD_DUAL (BIT(14))
#define SPI_MEM_FREAD_DUAL_M (BIT(14))
#define SPI_MEM_FREAD_DUAL_V 0x1
#define SPI_MEM_FREAD_DUAL_S 14
/* SPI_MEM_FASTRD_MODE : R/W ;bitpos:[13] ;default: 1'b1 ; */
/*description: This bit enable the bits: spi_mem_fread_qio  spi_mem_fread_dio
  spi_mem_fread_qout and spi_mem_fread_dout. 1: enable 0: disable.*/
#define SPI_MEM_FASTRD_MODE (BIT(13))
#define SPI_MEM_FASTRD_MODE_M (BIT(13))
#define SPI_MEM_FASTRD_MODE_V 0x1
#define SPI_MEM_FASTRD_MODE_S 13
/* SPI_MEM_TX_CRC_EN : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: For SPI1   enable crc32 when writing encrypted data to flash.
 1: enable 0:disable*/
#define SPI_MEM_TX_CRC_EN (BIT(11))
#define SPI_MEM_TX_CRC_EN_M (BIT(11))
#define SPI_MEM_TX_CRC_EN_V 0x1
#define SPI_MEM_TX_CRC_EN_S 11
/* SPI_MEM_FCS_CRC_EN : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: For SPI1   initialize crc32 module before writing encrypted data
 to flash. Active low.*/
#define SPI_MEM_FCS_CRC_EN (BIT(10))
#define SPI_MEM_FCS_CRC_EN_M (BIT(10))
#define SPI_MEM_FCS_CRC_EN_V 0x1
#define SPI_MEM_FCS_CRC_EN_S 10
/* SPI_MEM_FCMD_OCT : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: Apply 8 signals during command phase 1:enable 0: disable*/
#define SPI_MEM_FCMD_OCT (BIT(9))
#define SPI_MEM_FCMD_OCT_M (BIT(9))
#define SPI_MEM_FCMD_OCT_V 0x1
#define SPI_MEM_FCMD_OCT_S 9
/* SPI_MEM_FCMD_QUAD : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Apply 4 signals during command phase 1:enable 0: disable*/
#define SPI_MEM_FCMD_QUAD (BIT(8))
#define SPI_MEM_FCMD_QUAD_M (BIT(8))
#define SPI_MEM_FCMD_QUAD_V 0x1
#define SPI_MEM_FCMD_QUAD_S 8
/* SPI_MEM_FCMD_DUAL : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: Apply 2 signals during command phase 1:enable 0: disable*/
#define SPI_MEM_FCMD_DUAL (BIT(7))
#define SPI_MEM_FCMD_DUAL_M (BIT(7))
#define SPI_MEM_FCMD_DUAL_V 0x1
#define SPI_MEM_FCMD_DUAL_S 7
/* SPI_MEM_FADDR_OCT : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Apply 8 signals during address phase 1:enable 0: disable*/
#define SPI_MEM_FADDR_OCT (BIT(6))
#define SPI_MEM_FADDR_OCT_M (BIT(6))
#define SPI_MEM_FADDR_OCT_V 0x1
#define SPI_MEM_FADDR_OCT_S 6
/* SPI_MEM_FDIN_OCT : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: Apply 8 signals during read-data phase 1:enable 0: disable*/
#define SPI_MEM_FDIN_OCT (BIT(5))
#define SPI_MEM_FDIN_OCT_M (BIT(5))
#define SPI_MEM_FDIN_OCT_V 0x1
#define SPI_MEM_FDIN_OCT_S 5
/* SPI_MEM_FDOUT_OCT : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Apply 8 signals during write-data phase 1:enable 0: disable*/
#define SPI_MEM_FDOUT_OCT (BIT(4))
#define SPI_MEM_FDOUT_OCT_M (BIT(4))
#define SPI_MEM_FDOUT_OCT_V 0x1
#define SPI_MEM_FDOUT_OCT_S 4
/* SPI_MEM_FDUMMY_OUT : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: In the dummy phase the signal level of spi is output by the spi controller.*/
#define SPI_MEM_FDUMMY_OUT (BIT(3))
#define SPI_MEM_FDUMMY_OUT_M (BIT(3))
#define SPI_MEM_FDUMMY_OUT_V 0x1
#define SPI_MEM_FDUMMY_OUT_S 3

#define SPI_MEM_CTRL1_REG(i) (REG_SPI_MEM_BASE(i) + 0x00C)
/* SPI_MEM_CLK_MODE : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: SPI clock mode bits. 0: SPI clock is off when CS inactive 1:
 SPI clock is delayed one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3: SPI clock is alwasy on.*/
#define SPI_MEM_CLK_MODE 0x00000003
#define SPI_MEM_CLK_MODE_M ((SPI_MEM_CLK_MODE_V) << (SPI_MEM_CLK_MODE_S))
#define SPI_MEM_CLK_MODE_V 0x3
#define SPI_MEM_CLK_MODE_S 0

#define SPI_MEM_CTRL2_REG(i) (REG_SPI_MEM_BASE(i) + 0x010)
/* SPI_MEM_SYNC_RESET : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: The FSM will be reset.*/
#define SPI_MEM_SYNC_RESET (BIT(31))
#define SPI_MEM_SYNC_RESET_M (BIT(31))
#define SPI_MEM_SYNC_RESET_V 0x1
#define SPI_MEM_SYNC_RESET_S 31
/* SPI_MEM_CS_HOLD_DELAY : R/W ;bitpos:[30:25] ;default: 6'd0 ; */
/*description: These bits are used to set the minimum CS high time tSHSL between
 SPI burst transfer when accesses to flash. tSHSL is (SPI_MEM_CS_HOLD_DELAY[5:0] + 1) MSPI core clock cycles.*/
#define SPI_MEM_CS_HOLD_DELAY 0x0000003F
#define SPI_MEM_CS_HOLD_DELAY_M ((SPI_MEM_CS_HOLD_DELAY_V) << (SPI_MEM_CS_HOLD_DELAY_S))
#define SPI_MEM_CS_HOLD_DELAY_V 0x3F
#define SPI_MEM_CS_HOLD_DELAY_S 25
/* SPI_MEM_ECC_16TO18_BYTE_EN : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: Set this bit to enable MSPI ECC 16 bytes data with 2 ECC bytes
 mode when accesses flash.*/
#define SPI_MEM_ECC_16TO18_BYTE_EN (BIT(14))
#define SPI_MEM_ECC_16TO18_BYTE_EN_M (BIT(14))
#define SPI_MEM_ECC_16TO18_BYTE_EN_V 0x1
#define SPI_MEM_ECC_16TO18_BYTE_EN_S 14
/* SPI_MEM_ECC_SKIP_PAGE_CORNER : R/W ;bitpos:[13] ;default: 1'b1 ; */
/*description: 1: MSPI skips page corner when accesses flash. 0: Not skip page
 corner when accesses flash.*/
#define SPI_MEM_ECC_SKIP_PAGE_CORNER (BIT(13))
#define SPI_MEM_ECC_SKIP_PAGE_CORNER_M (BIT(13))
#define SPI_MEM_ECC_SKIP_PAGE_CORNER_V 0x1
#define SPI_MEM_ECC_SKIP_PAGE_CORNER_S 13
/* SPI_MEM_ECC_CS_HOLD_TIME : R/W ;bitpos:[12:10] ;default: 3'd3 ; */
/*description: SPI_MEM_CS_HOLD_TIME + SPI_MEM_ECC_CS_HOLD_TIME is the MSPI CS
 hold cycle in ECC mode when accessed flash.*/
#define SPI_MEM_ECC_CS_HOLD_TIME 0x00000007
#define SPI_MEM_ECC_CS_HOLD_TIME_M ((SPI_MEM_ECC_CS_HOLD_TIME_V) << (SPI_MEM_ECC_CS_HOLD_TIME_S))
#define SPI_MEM_ECC_CS_HOLD_TIME_V 0x7
#define SPI_MEM_ECC_CS_HOLD_TIME_S 10
/* SPI_MEM_CS_HOLD_TIME : R/W ;bitpos:[9:5] ;default: 5'h1 ; */
/*description: Spi cs signal is delayed to inactive by spi clock this bits are
 combined with spi_mem_cs_hold bit.*/
#define SPI_MEM_CS_HOLD_TIME 0x0000001F
#define SPI_MEM_CS_HOLD_TIME_M ((SPI_MEM_CS_HOLD_TIME_V) << (SPI_MEM_CS_HOLD_TIME_S))
#define SPI_MEM_CS_HOLD_TIME_V 0x1F
#define SPI_MEM_CS_HOLD_TIME_S 5
/* SPI_MEM_CS_SETUP_TIME : R/W ;bitpos:[4:0] ;default: 5'h1 ; */
/*description: (cycles-1) of prepare phase by spi clock this bits are combined
 with spi_mem_cs_setup bit.*/
#define SPI_MEM_CS_SETUP_TIME 0x0000001F
#define SPI_MEM_CS_SETUP_TIME_M ((SPI_MEM_CS_SETUP_TIME_V) << (SPI_MEM_CS_SETUP_TIME_S))
#define SPI_MEM_CS_SETUP_TIME_V 0x1F
#define SPI_MEM_CS_SETUP_TIME_S 0

#define SPI_MEM_CLOCK_REG(i) (REG_SPI_MEM_BASE(i) + 0x014)
/* SPI_MEM_CLK_EQU_SYSCLK : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: Set this bit in 1-division mode.*/
#define SPI_MEM_CLK_EQU_SYSCLK (BIT(31))
#define SPI_MEM_CLK_EQU_SYSCLK_M (BIT(31))
#define SPI_MEM_CLK_EQU_SYSCLK_V 0x1
#define SPI_MEM_CLK_EQU_SYSCLK_S 31
/* SPI_MEM_CLKCNT_N : R/W ;bitpos:[23:16] ;default: 8'h3 ; */
/*description: In the master mode it is the divider of spi_mem_clk. So spi_mem_clk
 frequency is system/(spi_mem_clkcnt_N+1)*/
#define SPI_MEM_CLKCNT_N 0x000000FF
#define SPI_MEM_CLKCNT_N_M ((SPI_MEM_CLKCNT_N_V) << (SPI_MEM_CLKCNT_N_S))
#define SPI_MEM_CLKCNT_N_V 0xFF
#define SPI_MEM_CLKCNT_N_S 16
/* SPI_MEM_CLKCNT_H : R/W ;bitpos:[15:8] ;default: 8'h1 ; */
/*description: In the master mode it must be floor((spi_mem_clkcnt_N+1)/2-1).*/
#define SPI_MEM_CLKCNT_H 0x000000FF
#define SPI_MEM_CLKCNT_H_M ((SPI_MEM_CLKCNT_H_V) << (SPI_MEM_CLKCNT_H_S))
#define SPI_MEM_CLKCNT_H_V 0xFF
#define SPI_MEM_CLKCNT_H_S 8
/* SPI_MEM_CLKCNT_L : R/W ;bitpos:[7:0] ;default: 8'h3 ; */
/*description: In the master mode it must be equal to spi_mem_clkcnt_N.*/
#define SPI_MEM_CLKCNT_L 0x000000FF
#define SPI_MEM_CLKCNT_L_M ((SPI_MEM_CLKCNT_L_V) << (SPI_MEM_CLKCNT_L_S))
#define SPI_MEM_CLKCNT_L_V 0xFF
#define SPI_MEM_CLKCNT_L_S 0

#define SPI_MEM_USER_REG(i) (REG_SPI_MEM_BASE(i) + 0x018)
/* SPI_MEM_USR_DUMMY : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: This bit enable the dummy phase of an operation.*/
#define SPI_MEM_USR_DUMMY (BIT(29))
#define SPI_MEM_USR_DUMMY_M (BIT(29))
#define SPI_MEM_USR_DUMMY_V 0x1
#define SPI_MEM_USR_DUMMY_S 29
/* SPI_MEM_USR_DUMMY_IDLE : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: spi clock is disable in dummy phase when the bit is enable.*/
#define SPI_MEM_USR_DUMMY_IDLE (BIT(26))
#define SPI_MEM_USR_DUMMY_IDLE_M (BIT(26))
#define SPI_MEM_USR_DUMMY_IDLE_V 0x1
#define SPI_MEM_USR_DUMMY_IDLE_S 26
/* SPI_MEM_CK_OUT_EDGE : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: the bit combined with spi_mem_mosi_delay_mode bits to set mosi
 signal delay mode.*/
#define SPI_MEM_CK_OUT_EDGE (BIT(9))
#define SPI_MEM_CK_OUT_EDGE_M (BIT(9))
#define SPI_MEM_CK_OUT_EDGE_V 0x1
#define SPI_MEM_CK_OUT_EDGE_S 9
/* SPI_MEM_CS_SETUP : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: spi cs is enable when spi is in  prepare  phase. 1: enable 0: disable.*/
#define SPI_MEM_CS_SETUP (BIT(7))
#define SPI_MEM_CS_SETUP_M (BIT(7))
#define SPI_MEM_CS_SETUP_V 0x1
#define SPI_MEM_CS_SETUP_S 7
/* SPI_MEM_CS_HOLD : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: spi cs keep low when spi is in  done  phase. 1: enable 0: disable.*/
#define SPI_MEM_CS_HOLD (BIT(6))
#define SPI_MEM_CS_HOLD_M (BIT(6))
#define SPI_MEM_CS_HOLD_V 0x1
#define SPI_MEM_CS_HOLD_S 6

#define SPI_MEM_USER1_REG(i) (REG_SPI_MEM_BASE(i) + 0x01C)
/* SPI_MEM_USR_ADDR_BITLEN : R/W ;bitpos:[31:26] ;default: 6'd23 ; */
/*description: The length in bits of address phase. The register value shall be (bit_num-1).*/
#define SPI_MEM_USR_ADDR_BITLEN 0x0000003F
#define SPI_MEM_USR_ADDR_BITLEN_M ((SPI_MEM_USR_ADDR_BITLEN_V) << (SPI_MEM_USR_ADDR_BITLEN_S))
#define SPI_MEM_USR_ADDR_BITLEN_V 0x3F
#define SPI_MEM_USR_ADDR_BITLEN_S 26
/* SPI_MEM_USR_DUMMY_CYCLELEN : R/W ;bitpos:[5:0] ;default: 6'd7 ; */
/*description: The length in spi_mem_clk cycles of dummy phase. The register
 value shall be (cycle_num-1).*/
#define SPI_MEM_USR_DUMMY_CYCLELEN 0x0000003F
#define SPI_MEM_USR_DUMMY_CYCLELEN_M ((SPI_MEM_USR_DUMMY_CYCLELEN_V) << (SPI_MEM_USR_DUMMY_CYCLELEN_S))
#define SPI_MEM_USR_DUMMY_CYCLELEN_V 0x3F
#define SPI_MEM_USR_DUMMY_CYCLELEN_S 0

#define SPI_MEM_USER2_REG(i) (REG_SPI_MEM_BASE(i) + 0x020)
/* SPI_MEM_USR_COMMAND_BITLEN : R/W ;bitpos:[31:28] ;default: 4'd7 ; */
/*description: The length in bits of command phase. The register value shall be (bit_num-1)*/
#define SPI_MEM_USR_COMMAND_BITLEN 0x0000000F
#define SPI_MEM_USR_COMMAND_BITLEN_M ((SPI_MEM_USR_COMMAND_BITLEN_V) << (SPI_MEM_USR_COMMAND_BITLEN_S))
#define SPI_MEM_USR_COMMAND_BITLEN_V 0xF
#define SPI_MEM_USR_COMMAND_BITLEN_S 28
/* SPI_MEM_USR_COMMAND_VALUE : R/W ;bitpos:[15:0] ;default: 16'b0 ; */
/*description: The value of  command.*/
#define SPI_MEM_USR_COMMAND_VALUE 0x0000FFFF
#define SPI_MEM_USR_COMMAND_VALUE_M ((SPI_MEM_USR_COMMAND_VALUE_V) << (SPI_MEM_USR_COMMAND_VALUE_S))
#define SPI_MEM_USR_COMMAND_VALUE_V 0xFFFF
#define SPI_MEM_USR_COMMAND_VALUE_S 0

#define SPI_MEM_RD_STATUS_REG(i) (REG_SPI_MEM_BASE(i) + 0x02C)
/* SPI_MEM_WB_MODE : R/W ;bitpos:[23:16] ;default: 8'h00 ; */
/*description: Mode bits in the flash fast read mode  it is combined with spi_mem_fastrd_mode
 bit.*/
#define SPI_MEM_WB_MODE 0x000000FF
#define SPI_MEM_WB_MODE_M ((SPI_MEM_WB_MODE_V) << (SPI_MEM_WB_MODE_S))
#define SPI_MEM_WB_MODE_V 0xFF
#define SPI_MEM_WB_MODE_S 16

#define SPI_MEM_EXT_ADDR_REG(i) (REG_SPI_MEM_BASE(i) + 0x030)
/* SPI_MEM_EXT_ADDR : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The register are the higher 32bits in the 64 bits address mode.*/
#define SPI_MEM_EXT_ADDR 0xFFFFFFFF
#define SPI_MEM_EXT_ADDR_M ((SPI_MEM_EXT_ADDR_V) << (SPI_MEM_EXT_ADDR_S))
#define SPI_MEM_EXT_ADDR_V 0xFFFFFFFF
#define SPI_MEM_EXT_ADDR_S 0

#define SPI_MEM_MISC_REG(i) (REG_SPI_MEM_BASE(i) + 0x034)
/* SPI_MEM_CS_KEEP_ACTIVE : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: spi cs line keep low when the bit is set.*/
#define SPI_MEM_CS_KEEP_ACTIVE (BIT(10))
#define SPI_MEM_CS_KEEP_ACTIVE_M (BIT(10))
#define SPI_MEM_CS_KEEP_ACTIVE_V 0x1
#define SPI_MEM_CS_KEEP_ACTIVE_S 10
/* SPI_MEM_CK_IDLE_EDGE : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: 1: spi clk line is high when idle     0: spi clk line is low when idle*/
#define SPI_MEM_CK_IDLE_EDGE (BIT(9))
#define SPI_MEM_CK_IDLE_EDGE_M (BIT(9))
#define SPI_MEM_CK_IDLE_EDGE_V 0x1
#define SPI_MEM_CK_IDLE_EDGE_S 9
/* SPI_MEM_SSUB_PIN : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: For SPI0   sram is connected to SUBPINs.*/
#define SPI_MEM_SSUB_PIN (BIT(8))
#define SPI_MEM_SSUB_PIN_M (BIT(8))
#define SPI_MEM_SSUB_PIN_V 0x1
#define SPI_MEM_SSUB_PIN_S 8
/* SPI_MEM_FSUB_PIN : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: For SPI0   flash is connected to SUBPINs.*/
#define SPI_MEM_FSUB_PIN (BIT(7))
#define SPI_MEM_FSUB_PIN_M (BIT(7))
#define SPI_MEM_FSUB_PIN_V 0x1
#define SPI_MEM_FSUB_PIN_S 7
/* SPI_MEM_TRANS_END_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to enable the intterrupt of SPI transmitting done.*/
#define SPI_MEM_TRANS_END_INT_ENA (BIT(4))
#define SPI_MEM_TRANS_END_INT_ENA_M (BIT(4))
#define SPI_MEM_TRANS_END_INT_ENA_V 0x1
#define SPI_MEM_TRANS_END_INT_ENA_S 4
/* SPI_MEM_TRANS_END : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to indicate the transimitting is done.*/
#define SPI_MEM_TRANS_END (BIT(3))
#define SPI_MEM_TRANS_END_M (BIT(3))
#define SPI_MEM_TRANS_END_V 0x1
#define SPI_MEM_TRANS_END_S 3

#define SPI_MEM_TX_CRC_REG(i) (REG_SPI_MEM_BASE(i) + 0x038)
/* SPI_MEM_TX_CRC_DATA : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: For SPI1  the value of crc32.*/
#define SPI_MEM_TX_CRC_DATA 0xFFFFFFFF
#define SPI_MEM_TX_CRC_DATA_M ((SPI_MEM_TX_CRC_DATA_V) << (SPI_MEM_TX_CRC_DATA_S))
#define SPI_MEM_TX_CRC_DATA_V 0xFFFFFFFF
#define SPI_MEM_TX_CRC_DATA_S 0

#define SPI_MEM_CACHE_FCTRL_REG(i) (REG_SPI_MEM_BASE(i) + 0x03C)
/* SPI_MEM_FADDR_QUAD : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: For SPI0 flash  address phase apply 4 signals. 1: enable 0: disable.
  The bit is the same with spi_mem_fread_qio.*/
#define SPI_MEM_FADDR_QUAD (BIT(8))
#define SPI_MEM_FADDR_QUAD_M (BIT(8))
#define SPI_MEM_FADDR_QUAD_V 0x1
#define SPI_MEM_FADDR_QUAD_S 8
/* SPI_MEM_FDOUT_QUAD : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: For SPI0 flash  dout phase apply 4 signals. 1: enable 0: disable.
  The bit is the same with spi_mem_fread_qio.*/
#define SPI_MEM_FDOUT_QUAD (BIT(7))
#define SPI_MEM_FDOUT_QUAD_M (BIT(7))
#define SPI_MEM_FDOUT_QUAD_V 0x1
#define SPI_MEM_FDOUT_QUAD_S 7
/* SPI_MEM_FDIN_QUAD : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: For SPI0 flash  din phase apply 4 signals. 1: enable 0: disable.
  The bit is the same with spi_mem_fread_qio.*/
#define SPI_MEM_FDIN_QUAD (BIT(6))
#define SPI_MEM_FDIN_QUAD_M (BIT(6))
#define SPI_MEM_FDIN_QUAD_V 0x1
#define SPI_MEM_FDIN_QUAD_S 6
/* SPI_MEM_FADDR_DUAL : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: For SPI0 flash  address phase apply 2 signals. 1: enable 0: disable.
  The bit is the same with spi_mem_fread_dio.*/
#define SPI_MEM_FADDR_DUAL (BIT(5))
#define SPI_MEM_FADDR_DUAL_M (BIT(5))
#define SPI_MEM_FADDR_DUAL_V 0x1
#define SPI_MEM_FADDR_DUAL_S 5
/* SPI_MEM_FDOUT_DUAL : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: For SPI0 flash  dout phase apply 2 signals. 1: enable 0: disable.
 The bit is the same with spi_mem_fread_dio.*/
#define SPI_MEM_FDOUT_DUAL (BIT(4))
#define SPI_MEM_FDOUT_DUAL_M (BIT(4))
#define SPI_MEM_FDOUT_DUAL_V 0x1
#define SPI_MEM_FDOUT_DUAL_S 4
/* SPI_MEM_FDIN_DUAL : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: For SPI0 flash  din phase apply 2 signals. 1: enable 0: disable.
 The bit is the same with spi_mem_fread_dio.*/
#define SPI_MEM_FDIN_DUAL (BIT(3))
#define SPI_MEM_FDIN_DUAL_M (BIT(3))
#define SPI_MEM_FDIN_DUAL_V 0x1
#define SPI_MEM_FDIN_DUAL_S 3
/* SPI_MEM_CACHE_FLASH_USR_CMD : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: For SPI0   cache  read flash for user define command  1: enable  0:disable.*/
#define SPI_MEM_CACHE_FLASH_USR_CMD (BIT(2))
#define SPI_MEM_CACHE_FLASH_USR_CMD_M (BIT(2))
#define SPI_MEM_CACHE_FLASH_USR_CMD_V 0x1
#define SPI_MEM_CACHE_FLASH_USR_CMD_S 2
/* SPI_MEM_CACHE_USR_CMD_4BYTE : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: For SPI0   cache  read flash with 4 bytes command  1: enable  0:disable.*/
#define SPI_MEM_CACHE_USR_CMD_4BYTE (BIT(1))
#define SPI_MEM_CACHE_USR_CMD_4BYTE_M (BIT(1))
#define SPI_MEM_CACHE_USR_CMD_4BYTE_V 0x1
#define SPI_MEM_CACHE_USR_CMD_4BYTE_S 1
/* SPI_MEM_CACHE_REQ_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: For SPI0  Cache access enable  1: enable  0:disable.*/
#define SPI_MEM_CACHE_REQ_EN (BIT(0))
#define SPI_MEM_CACHE_REQ_EN_M (BIT(0))
#define SPI_MEM_CACHE_REQ_EN_V 0x1
#define SPI_MEM_CACHE_REQ_EN_S 0

#define SPI_MEM_CACHE_SCTRL_REG(i) (REG_SPI_MEM_BASE(i) + 0x040)
/* SPI_MEM_SRAM_WDUMMY_CYCLELEN : R/W ;bitpos:[27:22] ;default: 6'b1 ; */
/*description: For SPI0  In the sram mode  it is the length in bits of write
 dummy phase. The register value shall be (bit_num-1).*/
#define SPI_MEM_SRAM_WDUMMY_CYCLELEN 0x0000003F
#define SPI_MEM_SRAM_WDUMMY_CYCLELEN_M ((SPI_MEM_SRAM_WDUMMY_CYCLELEN_V) << (SPI_MEM_SRAM_WDUMMY_CYCLELEN_S))
#define SPI_MEM_SRAM_WDUMMY_CYCLELEN_V 0x3F
#define SPI_MEM_SRAM_WDUMMY_CYCLELEN_S 22
/* SPI_MEM_SRAM_OCT : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: reserved*/
#define SPI_MEM_SRAM_OCT (BIT(21))
#define SPI_MEM_SRAM_OCT_M (BIT(21))
#define SPI_MEM_SRAM_OCT_V 0x1
#define SPI_MEM_SRAM_OCT_S 21
/* SPI_MEM_CACHE_SRAM_USR_WCMD : R/W ;bitpos:[20] ;default: 1'b1 ; */
/*description: For SPI0  In the spi sram mode cache write sram for user define command*/
#define SPI_MEM_CACHE_SRAM_USR_WCMD (BIT(20))
#define SPI_MEM_CACHE_SRAM_USR_WCMD_M (BIT(20))
#define SPI_MEM_CACHE_SRAM_USR_WCMD_V 0x1
#define SPI_MEM_CACHE_SRAM_USR_WCMD_S 20
/* SPI_MEM_SRAM_ADDR_BITLEN : R/W ;bitpos:[19:14] ;default: 6'd23 ; */
/*description: For SPI0  In the sram mode  it is the length in bits of address
 phase. The register value shall be (bit_num-1).*/
#define SPI_MEM_SRAM_ADDR_BITLEN 0x0000003F
#define SPI_MEM_SRAM_ADDR_BITLEN_M ((SPI_MEM_SRAM_ADDR_BITLEN_V) << (SPI_MEM_SRAM_ADDR_BITLEN_S))
#define SPI_MEM_SRAM_ADDR_BITLEN_V 0x3F
#define SPI_MEM_SRAM_ADDR_BITLEN_S 14
/* SPI_MEM_SRAM_RDUMMY_CYCLELEN : R/W ;bitpos:[11:6] ;default: 6'b1 ; */
/*description: For SPI0  In the sram mode  it is the length in bits of read
 dummy phase. The register value shall be (bit_num-1).*/
#define SPI_MEM_SRAM_RDUMMY_CYCLELEN 0x0000003F
#define SPI_MEM_SRAM_RDUMMY_CYCLELEN_M ((SPI_MEM_SRAM_RDUMMY_CYCLELEN_V) << (SPI_MEM_SRAM_RDUMMY_CYCLELEN_S))
#define SPI_MEM_SRAM_RDUMMY_CYCLELEN_V 0x3F
#define SPI_MEM_SRAM_RDUMMY_CYCLELEN_S 6
/* SPI_MEM_CACHE_SRAM_USR_RCMD : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: For SPI0  In the spi sram mode cache read sram for user define command.*/
#define SPI_MEM_CACHE_SRAM_USR_RCMD (BIT(5))
#define SPI_MEM_CACHE_SRAM_USR_RCMD_M (BIT(5))
#define SPI_MEM_CACHE_SRAM_USR_RCMD_V 0x1
#define SPI_MEM_CACHE_SRAM_USR_RCMD_S 5
/* SPI_MEM_USR_RD_SRAM_DUMMY : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: For SPI0  In the spi sram mode  it is the enable bit of dummy
 phase for read operations.*/
#define SPI_MEM_USR_RD_SRAM_DUMMY (BIT(4))
#define SPI_MEM_USR_RD_SRAM_DUMMY_M (BIT(4))
#define SPI_MEM_USR_RD_SRAM_DUMMY_V 0x1
#define SPI_MEM_USR_RD_SRAM_DUMMY_S 4
/* SPI_MEM_USR_WR_SRAM_DUMMY : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: For SPI0  In the spi sram mode  it is the enable bit of dummy
 phase for write operations.*/
#define SPI_MEM_USR_WR_SRAM_DUMMY (BIT(3))
#define SPI_MEM_USR_WR_SRAM_DUMMY_M (BIT(3))
#define SPI_MEM_USR_WR_SRAM_DUMMY_V 0x1
#define SPI_MEM_USR_WR_SRAM_DUMMY_S 3
/* SPI_MEM_USR_SRAM_QIO : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: For SPI0  In the spi sram mode  spi quad I/O mode enable  1: enable  0:disable*/
#define SPI_MEM_USR_SRAM_QIO (BIT(2))
#define SPI_MEM_USR_SRAM_QIO_M (BIT(2))
#define SPI_MEM_USR_SRAM_QIO_V 0x1
#define SPI_MEM_USR_SRAM_QIO_S 2
/* SPI_MEM_USR_SRAM_DIO : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: For SPI0  In the spi sram mode  spi dual I/O mode enable  1: enable  0:disable*/
#define SPI_MEM_USR_SRAM_DIO (BIT(1))
#define SPI_MEM_USR_SRAM_DIO_M (BIT(1))
#define SPI_MEM_USR_SRAM_DIO_V 0x1
#define SPI_MEM_USR_SRAM_DIO_S 1
/* SPI_MEM_CACHE_USR_SCMD_4BYTE : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: For SPI0  In the spi sram mode  cache read flash with 4 bytes
 command  1: enable  0:disable.*/
#define SPI_MEM_CACHE_USR_SCMD_4BYTE (BIT(0))
#define SPI_MEM_CACHE_USR_SCMD_4BYTE_M (BIT(0))
#define SPI_MEM_CACHE_USR_SCMD_4BYTE_V 0x1
#define SPI_MEM_CACHE_USR_SCMD_4BYTE_S 0

#define SPI_MEM_SRAM_CMD_REG(i) (REG_SPI_MEM_BASE(i) + 0x044)
/* SPI_MEM_SDUMMY_OUT : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: In the dummy phase the signal level of spi is output by the spi controller.*/
#define SPI_MEM_SDUMMY_OUT (BIT(22))
#define SPI_MEM_SDUMMY_OUT_M (BIT(22))
#define SPI_MEM_SDUMMY_OUT_V 0x1
#define SPI_MEM_SDUMMY_OUT_S 22
/* SPI_MEM_SCMD_OCT : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: For SPI0 sram  cmd phase apply 8 signals. 1: enable 0: disable.*/
#define SPI_MEM_SCMD_OCT (BIT(21))
#define SPI_MEM_SCMD_OCT_M (BIT(21))
#define SPI_MEM_SCMD_OCT_V 0x1
#define SPI_MEM_SCMD_OCT_S 21
/* SPI_MEM_SADDR_OCT : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: For SPI0 sram  address phase apply 4 signals. 1: enable 0: disable.*/
#define SPI_MEM_SADDR_OCT (BIT(20))
#define SPI_MEM_SADDR_OCT_M (BIT(20))
#define SPI_MEM_SADDR_OCT_V 0x1
#define SPI_MEM_SADDR_OCT_S 20
/* SPI_MEM_SDOUT_OCT : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: For SPI0 sram  dout phase apply 8 signals. 1: enable 0: disable.*/
#define SPI_MEM_SDOUT_OCT (BIT(19))
#define SPI_MEM_SDOUT_OCT_M (BIT(19))
#define SPI_MEM_SDOUT_OCT_V 0x1
#define SPI_MEM_SDOUT_OCT_S 19
/* SPI_MEM_SDIN_OCT : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: For SPI0 sram  din phase apply 8 signals. 1: enable 0: disable.*/
#define SPI_MEM_SDIN_OCT (BIT(18))
#define SPI_MEM_SDIN_OCT_M (BIT(18))
#define SPI_MEM_SDIN_OCT_V 0x1
#define SPI_MEM_SDIN_OCT_S 18
/* SPI_MEM_SCMD_QUAD : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: For SPI0 sram  cmd phase apply 4 signals. 1: enable 0: disable.
 The bit is the same with spi_mem_usr_sram_qio.*/
#define SPI_MEM_SCMD_QUAD (BIT(17))
#define SPI_MEM_SCMD_QUAD_M (BIT(17))
#define SPI_MEM_SCMD_QUAD_V 0x1
#define SPI_MEM_SCMD_QUAD_S 17
/* SPI_MEM_SADDR_QUAD : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: For SPI0 sram  address phase apply 4 signals. 1: enable 0: disable.
 The bit is the same with spi_mem_usr_sram_qio.*/
#define SPI_MEM_SADDR_QUAD (BIT(16))
#define SPI_MEM_SADDR_QUAD_M (BIT(16))
#define SPI_MEM_SADDR_QUAD_V 0x1
#define SPI_MEM_SADDR_QUAD_S 16
/* SPI_MEM_SDOUT_QUAD : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: For SPI0 sram  dout phase apply 4 signals. 1: enable 0: disable.
 The bit is the same with spi_mem_usr_sram_qio.*/
#define SPI_MEM_SDOUT_QUAD (BIT(15))
#define SPI_MEM_SDOUT_QUAD_M (BIT(15))
#define SPI_MEM_SDOUT_QUAD_V 0x1
#define SPI_MEM_SDOUT_QUAD_S 15
/* SPI_MEM_SDIN_QUAD : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: For SPI0 sram  din phase apply 4 signals. 1: enable 0: disable.
 The bit is the same with spi_mem_usr_sram_qio.*/
#define SPI_MEM_SDIN_QUAD (BIT(14))
#define SPI_MEM_SDIN_QUAD_M (BIT(14))
#define SPI_MEM_SDIN_QUAD_V 0x1
#define SPI_MEM_SDIN_QUAD_S 14
/* SPI_MEM_SCMD_DUAL : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: For SPI0 sram  cmd phase apply 2 signals. 1: enable 0: disable.
 The bit is the same with spi_mem_usr_sram_dio.*/
#define SPI_MEM_SCMD_DUAL (BIT(13))
#define SPI_MEM_SCMD_DUAL_M (BIT(13))
#define SPI_MEM_SCMD_DUAL_V 0x1
#define SPI_MEM_SCMD_DUAL_S 13
/* SPI_MEM_SADDR_DUAL : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: For SPI0 sram  address phase apply 2 signals. 1: enable 0: disable.
 The bit is the same with spi_mem_usr_sram_dio.*/
#define SPI_MEM_SADDR_DUAL (BIT(12))
#define SPI_MEM_SADDR_DUAL_M (BIT(12))
#define SPI_MEM_SADDR_DUAL_V 0x1
#define SPI_MEM_SADDR_DUAL_S 12
/* SPI_MEM_SDOUT_DUAL : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: For SPI0 sram  dout phase apply 2 signals. 1: enable 0: disable.
 The bit is the same with spi_mem_usr_sram_dio.*/
#define SPI_MEM_SDOUT_DUAL (BIT(11))
#define SPI_MEM_SDOUT_DUAL_M (BIT(11))
#define SPI_MEM_SDOUT_DUAL_V 0x1
#define SPI_MEM_SDOUT_DUAL_S 11
/* SPI_MEM_SDIN_DUAL : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: For SPI0 sram  din phase apply 2 signals. 1: enable 0: disable.
 The bit is the same with spi_mem_usr_sram_dio.*/
#define SPI_MEM_SDIN_DUAL (BIT(10))
#define SPI_MEM_SDIN_DUAL_M (BIT(10))
#define SPI_MEM_SDIN_DUAL_V 0x1
#define SPI_MEM_SDIN_DUAL_S 10
/* SPI_MEM_SWB_MODE : R/W ;bitpos:[9:2] ;default: 8'b0 ; */
/*description: Mode bits in the psram fast read mode  it is combined with spi_mem_fastrd_mode
 bit.*/
#define SPI_MEM_SWB_MODE 0x000000FF
#define SPI_MEM_SWB_MODE_M ((SPI_MEM_SWB_MODE_V) << (SPI_MEM_SWB_MODE_S))
#define SPI_MEM_SWB_MODE_V 0xFF
#define SPI_MEM_SWB_MODE_S 2
/* SPI_MEM_SCLK_MODE : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: SPI clock mode bits. 0: SPI clock is off when CS inactive 1:
 SPI clock is delayed one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3: SPI clock is alwasy on.*/
#define SPI_MEM_SCLK_MODE 0x00000003
#define SPI_MEM_SCLK_MODE_M ((SPI_MEM_SCLK_MODE_V) << (SPI_MEM_SCLK_MODE_S))
#define SPI_MEM_SCLK_MODE_V 0x3
#define SPI_MEM_SCLK_MODE_S 0

#define SPI_MEM_SRAM_DRD_CMD_REG(i) (REG_SPI_MEM_BASE(i) + 0x048)
/* SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN : R/W ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: For SPI0 When cache mode is enable it is the length in bits of
 command phase for sram. The register value shall be (bit_num-1).*/
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN 0x0000000F
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_M ((SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_V) << (SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_S))
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_V 0xF
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_S 28
/* SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: For SPI0 When cache mode is enable it is the read command value
 of command phase for sram.*/
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE 0x0000FFFF
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_M ((SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_V) << (SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_S))
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_V 0xFFFF
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_S 0

#define SPI_MEM_SRAM_DWR_CMD_REG(i) (REG_SPI_MEM_BASE(i) + 0x04C)
/* SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN : R/W ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: For SPI0 When cache mode is enable it is the in bits of command
 phase  for sram. The register value shall be (bit_num-1).*/
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN 0x0000000F
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_M ((SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_V) << (SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_S))
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_V 0xF
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_S 28
/* SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: For SPI0 When cache mode is enable it is the write command value
 of command phase for sram.*/
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE 0x0000FFFF
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_M ((SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_V) << (SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_S))
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_V 0xFFFF
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_S 0

#define SPI_MEM_SRAM_CLK_REG(i) (REG_SPI_MEM_BASE(i) + 0x050)
/* SPI_MEM_SCLK_EQU_SYSCLK : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: For SPI0 sram interface  1: spi_mem_clk is eqaul to system 0:
 spi_mem_clk is divided from system clock.*/
#define SPI_MEM_SCLK_EQU_SYSCLK (BIT(31))
#define SPI_MEM_SCLK_EQU_SYSCLK_M (BIT(31))
#define SPI_MEM_SCLK_EQU_SYSCLK_V 0x1
#define SPI_MEM_SCLK_EQU_SYSCLK_S 31
/* SPI_MEM_SCLKCNT_N : R/W ;bitpos:[23:16] ;default: 8'h3 ; */
/*description: For SPI0 sram interface  it is the divider of spi_mem_clk. So
 spi_mem_clk frequency is system/(spi_mem_clkcnt_N+1)*/
#define SPI_MEM_SCLKCNT_N 0x000000FF
#define SPI_MEM_SCLKCNT_N_M ((SPI_MEM_SCLKCNT_N_V) << (SPI_MEM_SCLKCNT_N_S))
#define SPI_MEM_SCLKCNT_N_V 0xFF
#define SPI_MEM_SCLKCNT_N_S 16
/* SPI_MEM_SCLKCNT_H : R/W ;bitpos:[15:8] ;default: 8'h1 ; */
/*description: For SPI0 sram interface  it must be floor((spi_mem_clkcnt_N+1)/2-1).*/
#define SPI_MEM_SCLKCNT_H 0x000000FF
#define SPI_MEM_SCLKCNT_H_M ((SPI_MEM_SCLKCNT_H_V) << (SPI_MEM_SCLKCNT_H_S))
#define SPI_MEM_SCLKCNT_H_V 0xFF
#define SPI_MEM_SCLKCNT_H_S 8
/* SPI_MEM_SCLKCNT_L : R/W ;bitpos:[7:0] ;default: 8'h3 ; */
/*description: For SPI0 sram interface  it must be equal to spi_mem_clkcnt_N.*/
#define SPI_MEM_SCLKCNT_L 0x000000FF
#define SPI_MEM_SCLKCNT_L_M ((SPI_MEM_SCLKCNT_L_V) << (SPI_MEM_SCLKCNT_L_S))
#define SPI_MEM_SCLKCNT_L_V 0xFF
#define SPI_MEM_SCLKCNT_L_S 0

#define SPI_MEM_FSM_REG(i) (REG_SPI_MEM_BASE(i) + 0x054)
/* SPI_MEM_ST : RO ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: The status of spi state machine. 0: idle state  1: preparation
 state  2: send command state  3: send data state  4: red data state  5:write data state  6: wait state  7: done state.*/
#define SPI_MEM_ST 0x00000007
#define SPI_MEM_ST_M ((SPI_MEM_ST_V) << (SPI_MEM_ST_S))
#define SPI_MEM_ST_V 0x7
#define SPI_MEM_ST_S 0

#define SPI_MEM_TIMING_CALI_REG(i) (REG_SPI_MEM_BASE(i) + 0x0A8)
/* SPI_MEM_EXTRA_DUMMY_CYCLELEN : R/W ;bitpos:[4:2] ;default: 3'd0 ; */
/*description: add extra dummy spi clock cycle length for spi clock calibration.*/
#define SPI_MEM_EXTRA_DUMMY_CYCLELEN 0x00000007
#define SPI_MEM_EXTRA_DUMMY_CYCLELEN_M ((SPI_MEM_EXTRA_DUMMY_CYCLELEN_V) << (SPI_MEM_EXTRA_DUMMY_CYCLELEN_S))
#define SPI_MEM_EXTRA_DUMMY_CYCLELEN_V 0x7
#define SPI_MEM_EXTRA_DUMMY_CYCLELEN_S 2
/* SPI_MEM_TIMING_CALI : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to enable timing auto-calibration for all reading operations.*/
#define SPI_MEM_TIMING_CALI (BIT(1))
#define SPI_MEM_TIMING_CALI_M (BIT(1))
#define SPI_MEM_TIMING_CALI_V 0x1
#define SPI_MEM_TIMING_CALI_S 1
/* SPI_MEM_TIMING_CLK_ENA : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to enable timing adjust clock for all reading operations.*/
#define SPI_MEM_TIMING_CLK_ENA (BIT(0))
#define SPI_MEM_TIMING_CLK_ENA_M (BIT(0))
#define SPI_MEM_TIMING_CLK_ENA_V 0x1
#define SPI_MEM_TIMING_CLK_ENA_S 0

#define SPI_MEM_DIN_MODE_REG(i) (REG_SPI_MEM_BASE(i) + 0x0AC)
/* SPI_MEM_DINS_MODE : R/W ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk*/
#define SPI_MEM_DINS_MODE 0x00000003
#define SPI_MEM_DINS_MODE_M ((SPI_MEM_DINS_MODE_V) << (SPI_MEM_DINS_MODE_S))
#define SPI_MEM_DINS_MODE_V 0x3
#define SPI_MEM_DINS_MODE_S 16
/* SPI_MEM_DIN7_MODE : R/W ;bitpos:[15:14] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk*/
#define SPI_MEM_DIN7_MODE 0x00000003
#define SPI_MEM_DIN7_MODE_M ((SPI_MEM_DIN7_MODE_V) << (SPI_MEM_DIN7_MODE_S))
#define SPI_MEM_DIN7_MODE_V 0x3
#define SPI_MEM_DIN7_MODE_S 14
/* SPI_MEM_DIN6_MODE : R/W ;bitpos:[13:12] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk*/
#define SPI_MEM_DIN6_MODE 0x00000003
#define SPI_MEM_DIN6_MODE_M ((SPI_MEM_DIN6_MODE_V) << (SPI_MEM_DIN6_MODE_S))
#define SPI_MEM_DIN6_MODE_V 0x3
#define SPI_MEM_DIN6_MODE_S 12
/* SPI_MEM_DIN5_MODE : R/W ;bitpos:[11:10] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk*/
#define SPI_MEM_DIN5_MODE 0x00000003
#define SPI_MEM_DIN5_MODE_M ((SPI_MEM_DIN5_MODE_V) << (SPI_MEM_DIN5_MODE_S))
#define SPI_MEM_DIN5_MODE_V 0x3
#define SPI_MEM_DIN5_MODE_S 10
/* SPI_MEM_DIN4_MODE : R/W ;bitpos:[9:8] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk*/
#define SPI_MEM_DIN4_MODE 0x00000003
#define SPI_MEM_DIN4_MODE_M ((SPI_MEM_DIN4_MODE_V) << (SPI_MEM_DIN4_MODE_S))
#define SPI_MEM_DIN4_MODE_V 0x3
#define SPI_MEM_DIN4_MODE_S 8
/* SPI_MEM_DIN3_MODE : R/W ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_DIN3_MODE 0x00000003
#define SPI_MEM_DIN3_MODE_M ((SPI_MEM_DIN3_MODE_V) << (SPI_MEM_DIN3_MODE_S))
#define SPI_MEM_DIN3_MODE_V 0x3
#define SPI_MEM_DIN3_MODE_S 6
/* SPI_MEM_DIN2_MODE : R/W ;bitpos:[5:4] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_DIN2_MODE 0x00000003
#define SPI_MEM_DIN2_MODE_M ((SPI_MEM_DIN2_MODE_V) << (SPI_MEM_DIN2_MODE_S))
#define SPI_MEM_DIN2_MODE_V 0x3
#define SPI_MEM_DIN2_MODE_S 4
/* SPI_MEM_DIN1_MODE : R/W ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_DIN1_MODE 0x00000003
#define SPI_MEM_DIN1_MODE_M ((SPI_MEM_DIN1_MODE_V) << (SPI_MEM_DIN1_MODE_S))
#define SPI_MEM_DIN1_MODE_V 0x3
#define SPI_MEM_DIN1_MODE_S 2
/* SPI_MEM_DIN0_MODE : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_DIN0_MODE 0x00000003
#define SPI_MEM_DIN0_MODE_M ((SPI_MEM_DIN0_MODE_V) << (SPI_MEM_DIN0_MODE_S))
#define SPI_MEM_DIN0_MODE_V 0x3
#define SPI_MEM_DIN0_MODE_S 0

#define SPI_MEM_DIN_NUM_REG(i) (REG_SPI_MEM_BASE(i) + 0x0B0)
/* SPI_MEM_DINS_NUM : R/W ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DINS_NUM 0x00000003
#define SPI_MEM_DINS_NUM_M ((SPI_MEM_DINS_NUM_V) << (SPI_MEM_DINS_NUM_S))
#define SPI_MEM_DINS_NUM_V 0x3
#define SPI_MEM_DINS_NUM_S 16
/* SPI_MEM_DIN7_NUM : R/W ;bitpos:[15:14] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DIN7_NUM 0x00000003
#define SPI_MEM_DIN7_NUM_M ((SPI_MEM_DIN7_NUM_V) << (SPI_MEM_DIN7_NUM_S))
#define SPI_MEM_DIN7_NUM_V 0x3
#define SPI_MEM_DIN7_NUM_S 14
/* SPI_MEM_DIN6_NUM : R/W ;bitpos:[13:12] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DIN6_NUM 0x00000003
#define SPI_MEM_DIN6_NUM_M ((SPI_MEM_DIN6_NUM_V) << (SPI_MEM_DIN6_NUM_S))
#define SPI_MEM_DIN6_NUM_V 0x3
#define SPI_MEM_DIN6_NUM_S 12
/* SPI_MEM_DIN5_NUM : R/W ;bitpos:[11:10] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DIN5_NUM 0x00000003
#define SPI_MEM_DIN5_NUM_M ((SPI_MEM_DIN5_NUM_V) << (SPI_MEM_DIN5_NUM_S))
#define SPI_MEM_DIN5_NUM_V 0x3
#define SPI_MEM_DIN5_NUM_S 10
/* SPI_MEM_DIN4_NUM : R/W ;bitpos:[9:8] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DIN4_NUM 0x00000003
#define SPI_MEM_DIN4_NUM_M ((SPI_MEM_DIN4_NUM_V) << (SPI_MEM_DIN4_NUM_S))
#define SPI_MEM_DIN4_NUM_V 0x3
#define SPI_MEM_DIN4_NUM_S 8
/* SPI_MEM_DIN3_NUM : R/W ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DIN3_NUM 0x00000003
#define SPI_MEM_DIN3_NUM_M ((SPI_MEM_DIN3_NUM_V) << (SPI_MEM_DIN3_NUM_S))
#define SPI_MEM_DIN3_NUM_V 0x3
#define SPI_MEM_DIN3_NUM_S 6
/* SPI_MEM_DIN2_NUM : R/W ;bitpos:[5:4] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DIN2_NUM 0x00000003
#define SPI_MEM_DIN2_NUM_M ((SPI_MEM_DIN2_NUM_V) << (SPI_MEM_DIN2_NUM_S))
#define SPI_MEM_DIN2_NUM_V 0x3
#define SPI_MEM_DIN2_NUM_S 4
/* SPI_MEM_DIN1_NUM : R/W ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DIN1_NUM 0x00000003
#define SPI_MEM_DIN1_NUM_M ((SPI_MEM_DIN1_NUM_V) << (SPI_MEM_DIN1_NUM_S))
#define SPI_MEM_DIN1_NUM_V 0x3
#define SPI_MEM_DIN1_NUM_S 2
/* SPI_MEM_DIN0_NUM : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_DIN0_NUM 0x00000003
#define SPI_MEM_DIN0_NUM_M ((SPI_MEM_DIN0_NUM_V) << (SPI_MEM_DIN0_NUM_S))
#define SPI_MEM_DIN0_NUM_V 0x3
#define SPI_MEM_DIN0_NUM_S 0

#define SPI_MEM_DOUT_MODE_REG(i) (REG_SPI_MEM_BASE(i) + 0x0B4)
/* SPI_MEM_DOUTS_MODE : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
#define SPI_MEM_DOUTS_MODE (BIT(8))
#define SPI_MEM_DOUTS_MODE_M (BIT(8))
#define SPI_MEM_DOUTS_MODE_V 0x1
#define SPI_MEM_DOUTS_MODE_S 8
/* SPI_MEM_DOUT7_MODE : R/W ;bitpos:[7] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
#define SPI_MEM_DOUT7_MODE (BIT(7))
#define SPI_MEM_DOUT7_MODE_M (BIT(7))
#define SPI_MEM_DOUT7_MODE_V 0x1
#define SPI_MEM_DOUT7_MODE_S 7
/* SPI_MEM_DOUT6_MODE : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
#define SPI_MEM_DOUT6_MODE (BIT(6))
#define SPI_MEM_DOUT6_MODE_M (BIT(6))
#define SPI_MEM_DOUT6_MODE_V 0x1
#define SPI_MEM_DOUT6_MODE_S 6
/* SPI_MEM_DOUT5_MODE : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
#define SPI_MEM_DOUT5_MODE (BIT(5))
#define SPI_MEM_DOUT5_MODE_M (BIT(5))
#define SPI_MEM_DOUT5_MODE_V 0x1
#define SPI_MEM_DOUT5_MODE_S 5
/* SPI_MEM_DOUT4_MODE : R/W ;bitpos:[4] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
#define SPI_MEM_DOUT4_MODE (BIT(4))
#define SPI_MEM_DOUT4_MODE_M (BIT(4))
#define SPI_MEM_DOUT4_MODE_V 0x1
#define SPI_MEM_DOUT4_MODE_S 4
/* SPI_MEM_DOUT3_MODE : R/W ;bitpos:[3] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_DOUT3_MODE (BIT(3))
#define SPI_MEM_DOUT3_MODE_M (BIT(3))
#define SPI_MEM_DOUT3_MODE_V 0x1
#define SPI_MEM_DOUT3_MODE_S 3
/* SPI_MEM_DOUT2_MODE : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_DOUT2_MODE (BIT(2))
#define SPI_MEM_DOUT2_MODE_M (BIT(2))
#define SPI_MEM_DOUT2_MODE_V 0x1
#define SPI_MEM_DOUT2_MODE_S 2
/* SPI_MEM_DOUT1_MODE : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_DOUT1_MODE (BIT(1))
#define SPI_MEM_DOUT1_MODE_M (BIT(1))
#define SPI_MEM_DOUT1_MODE_V 0x1
#define SPI_MEM_DOUT1_MODE_S 1
/* SPI_MEM_DOUT0_MODE : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_DOUT0_MODE (BIT(0))
#define SPI_MEM_DOUT0_MODE_M (BIT(0))
#define SPI_MEM_DOUT0_MODE_V 0x1
#define SPI_MEM_DOUT0_MODE_S 0

#define SPI_MEM_SPI_SMEM_TIMING_CALI_REG(i) (REG_SPI_MEM_BASE(i) + 0x0BC)
/* SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN : R/W ;bitpos:[4:2] ;default: 3'd0 ; */
/*description: For sram  add extra dummy spi clock cycle length for spi clock calibration.*/
#define SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN 0x00000007
#define SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_M ((SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_V) << (SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_S))
#define SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_V 0x7
#define SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_S 2
/* SPI_MEM_SPI_SMEM_TIMING_CALI : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: For sram  the bit is used to enable timing auto-calibration for
 all reading operations.*/
#define SPI_MEM_SPI_SMEM_TIMING_CALI (BIT(1))
#define SPI_MEM_SPI_SMEM_TIMING_CALI_M (BIT(1))
#define SPI_MEM_SPI_SMEM_TIMING_CALI_V 0x1
#define SPI_MEM_SPI_SMEM_TIMING_CALI_S 1
/* SPI_MEM_SPI_SMEM_TIMING_CLK_ENA : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: For sram  the bit is used to enable timing adjust clock for all
 reading operations.*/
#define SPI_MEM_SPI_SMEM_TIMING_CLK_ENA (BIT(0))
#define SPI_MEM_SPI_SMEM_TIMING_CLK_ENA_M (BIT(0))
#define SPI_MEM_SPI_SMEM_TIMING_CLK_ENA_V 0x1
#define SPI_MEM_SPI_SMEM_TIMING_CLK_ENA_S 0

#define SPI_MEM_SPI_SMEM_DIN_MODE_REG(i) (REG_SPI_MEM_BASE(i) + 0x0C0)
/* SPI_MEM_SPI_SMEM_DINS_MODE : R/W ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DINS_MODE 0x00000003
#define SPI_MEM_SPI_SMEM_DINS_MODE_M ((SPI_MEM_SPI_SMEM_DINS_MODE_V) << (SPI_MEM_SPI_SMEM_DINS_MODE_S))
#define SPI_MEM_SPI_SMEM_DINS_MODE_V 0x3
#define SPI_MEM_SPI_SMEM_DINS_MODE_S 16
/* SPI_MEM_SPI_SMEM_DIN7_MODE : R/W ;bitpos:[15:14] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DIN7_MODE 0x00000003
#define SPI_MEM_SPI_SMEM_DIN7_MODE_M ((SPI_MEM_SPI_SMEM_DIN7_MODE_V) << (SPI_MEM_SPI_SMEM_DIN7_MODE_S))
#define SPI_MEM_SPI_SMEM_DIN7_MODE_V 0x3
#define SPI_MEM_SPI_SMEM_DIN7_MODE_S 14
/* SPI_MEM_SPI_SMEM_DIN6_MODE : R/W ;bitpos:[13:12] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DIN6_MODE 0x00000003
#define SPI_MEM_SPI_SMEM_DIN6_MODE_M ((SPI_MEM_SPI_SMEM_DIN6_MODE_V) << (SPI_MEM_SPI_SMEM_DIN6_MODE_S))
#define SPI_MEM_SPI_SMEM_DIN6_MODE_V 0x3
#define SPI_MEM_SPI_SMEM_DIN6_MODE_S 12
/* SPI_MEM_SPI_SMEM_DIN5_MODE : R/W ;bitpos:[11:10] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DIN5_MODE 0x00000003
#define SPI_MEM_SPI_SMEM_DIN5_MODE_M ((SPI_MEM_SPI_SMEM_DIN5_MODE_V) << (SPI_MEM_SPI_SMEM_DIN5_MODE_S))
#define SPI_MEM_SPI_SMEM_DIN5_MODE_V 0x3
#define SPI_MEM_SPI_SMEM_DIN5_MODE_S 10
/* SPI_MEM_SPI_SMEM_DIN4_MODE : R/W ;bitpos:[9:8] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DIN4_MODE 0x00000003
#define SPI_MEM_SPI_SMEM_DIN4_MODE_M ((SPI_MEM_SPI_SMEM_DIN4_MODE_V) << (SPI_MEM_SPI_SMEM_DIN4_MODE_S))
#define SPI_MEM_SPI_SMEM_DIN4_MODE_V 0x3
#define SPI_MEM_SPI_SMEM_DIN4_MODE_S 8
/* SPI_MEM_SPI_SMEM_DIN3_MODE : R/W ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DIN3_MODE 0x00000003
#define SPI_MEM_SPI_SMEM_DIN3_MODE_M ((SPI_MEM_SPI_SMEM_DIN3_MODE_V) << (SPI_MEM_SPI_SMEM_DIN3_MODE_S))
#define SPI_MEM_SPI_SMEM_DIN3_MODE_V 0x3
#define SPI_MEM_SPI_SMEM_DIN3_MODE_S 6
/* SPI_MEM_SPI_SMEM_DIN2_MODE : R/W ;bitpos:[5:4] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DIN2_MODE 0x00000003
#define SPI_MEM_SPI_SMEM_DIN2_MODE_M ((SPI_MEM_SPI_SMEM_DIN2_MODE_V) << (SPI_MEM_SPI_SMEM_DIN2_MODE_S))
#define SPI_MEM_SPI_SMEM_DIN2_MODE_V 0x3
#define SPI_MEM_SPI_SMEM_DIN2_MODE_S 4
/* SPI_MEM_SPI_SMEM_DIN1_MODE : R/W ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DIN1_MODE 0x00000003
#define SPI_MEM_SPI_SMEM_DIN1_MODE_M ((SPI_MEM_SPI_SMEM_DIN1_MODE_V) << (SPI_MEM_SPI_SMEM_DIN1_MODE_S))
#define SPI_MEM_SPI_SMEM_DIN1_MODE_V 0x3
#define SPI_MEM_SPI_SMEM_DIN1_MODE_S 2
/* SPI_MEM_SPI_SMEM_DIN0_MODE : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DIN0_MODE 0x00000003
#define SPI_MEM_SPI_SMEM_DIN0_MODE_M ((SPI_MEM_SPI_SMEM_DIN0_MODE_V) << (SPI_MEM_SPI_SMEM_DIN0_MODE_S))
#define SPI_MEM_SPI_SMEM_DIN0_MODE_V 0x3
#define SPI_MEM_SPI_SMEM_DIN0_MODE_S 0

#define SPI_MEM_SPI_SMEM_DIN_NUM_REG(i) (REG_SPI_MEM_BASE(i) + 0x0C4)
/* SPI_MEM_SPI_SMEM_DINS_NUM : R/W ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: input
 without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DINS_NUM 0x00000003
#define SPI_MEM_SPI_SMEM_DINS_NUM_M ((SPI_MEM_SPI_SMEM_DINS_NUM_V) << (SPI_MEM_SPI_SMEM_DINS_NUM_S))
#define SPI_MEM_SPI_SMEM_DINS_NUM_V 0x3
#define SPI_MEM_SPI_SMEM_DINS_NUM_S 16
/* SPI_MEM_SPI_SMEM_DIN7_NUM : R/W ;bitpos:[15:14] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_SPI_SMEM_DIN7_NUM 0x00000003
#define SPI_MEM_SPI_SMEM_DIN7_NUM_M ((SPI_MEM_SPI_SMEM_DIN7_NUM_V) << (SPI_MEM_SPI_SMEM_DIN7_NUM_S))
#define SPI_MEM_SPI_SMEM_DIN7_NUM_V 0x3
#define SPI_MEM_SPI_SMEM_DIN7_NUM_S 14
/* SPI_MEM_SPI_SMEM_DIN6_NUM : R/W ;bitpos:[13:12] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_SPI_SMEM_DIN6_NUM 0x00000003
#define SPI_MEM_SPI_SMEM_DIN6_NUM_M ((SPI_MEM_SPI_SMEM_DIN6_NUM_V) << (SPI_MEM_SPI_SMEM_DIN6_NUM_S))
#define SPI_MEM_SPI_SMEM_DIN6_NUM_V 0x3
#define SPI_MEM_SPI_SMEM_DIN6_NUM_S 12
/* SPI_MEM_SPI_SMEM_DIN5_NUM : R/W ;bitpos:[11:10] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_SPI_SMEM_DIN5_NUM 0x00000003
#define SPI_MEM_SPI_SMEM_DIN5_NUM_M ((SPI_MEM_SPI_SMEM_DIN5_NUM_V) << (SPI_MEM_SPI_SMEM_DIN5_NUM_S))
#define SPI_MEM_SPI_SMEM_DIN5_NUM_V 0x3
#define SPI_MEM_SPI_SMEM_DIN5_NUM_S 10
/* SPI_MEM_SPI_SMEM_DIN4_NUM : R/W ;bitpos:[9:8] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_SPI_SMEM_DIN4_NUM 0x00000003
#define SPI_MEM_SPI_SMEM_DIN4_NUM_M ((SPI_MEM_SPI_SMEM_DIN4_NUM_V) << (SPI_MEM_SPI_SMEM_DIN4_NUM_S))
#define SPI_MEM_SPI_SMEM_DIN4_NUM_V 0x3
#define SPI_MEM_SPI_SMEM_DIN4_NUM_S 8
/* SPI_MEM_SPI_SMEM_DIN3_NUM : R/W ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_SPI_SMEM_DIN3_NUM 0x00000003
#define SPI_MEM_SPI_SMEM_DIN3_NUM_M ((SPI_MEM_SPI_SMEM_DIN3_NUM_V) << (SPI_MEM_SPI_SMEM_DIN3_NUM_S))
#define SPI_MEM_SPI_SMEM_DIN3_NUM_V 0x3
#define SPI_MEM_SPI_SMEM_DIN3_NUM_S 6
/* SPI_MEM_SPI_SMEM_DIN2_NUM : R/W ;bitpos:[5:4] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_SPI_SMEM_DIN2_NUM 0x00000003
#define SPI_MEM_SPI_SMEM_DIN2_NUM_M ((SPI_MEM_SPI_SMEM_DIN2_NUM_V) << (SPI_MEM_SPI_SMEM_DIN2_NUM_S))
#define SPI_MEM_SPI_SMEM_DIN2_NUM_V 0x3
#define SPI_MEM_SPI_SMEM_DIN2_NUM_S 4
/* SPI_MEM_SPI_SMEM_DIN1_NUM : R/W ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_SPI_SMEM_DIN1_NUM 0x00000003
#define SPI_MEM_SPI_SMEM_DIN1_NUM_M ((SPI_MEM_SPI_SMEM_DIN1_NUM_V) << (SPI_MEM_SPI_SMEM_DIN1_NUM_S))
#define SPI_MEM_SPI_SMEM_DIN1_NUM_V 0x3
#define SPI_MEM_SPI_SMEM_DIN1_NUM_S 2
/* SPI_MEM_SPI_SMEM_DIN0_NUM : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles  0: delayed
 by 1 cycle  1: delayed by 2 cycles ...*/
#define SPI_MEM_SPI_SMEM_DIN0_NUM 0x00000003
#define SPI_MEM_SPI_SMEM_DIN0_NUM_M ((SPI_MEM_SPI_SMEM_DIN0_NUM_V) << (SPI_MEM_SPI_SMEM_DIN0_NUM_S))
#define SPI_MEM_SPI_SMEM_DIN0_NUM_V 0x3
#define SPI_MEM_SPI_SMEM_DIN0_NUM_S 0

#define SPI_MEM_SPI_SMEM_DOUT_MODE_REG(i) (REG_SPI_MEM_BASE(i) + 0x0C8)
/* SPI_MEM_SPI_SMEM_DOUTS_MODE : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUTS_MODE (BIT(8))
#define SPI_MEM_SPI_SMEM_DOUTS_MODE_M (BIT(8))
#define SPI_MEM_SPI_SMEM_DOUTS_MODE_V 0x1
#define SPI_MEM_SPI_SMEM_DOUTS_MODE_S 8
/* SPI_MEM_SPI_SMEM_DOUT7_MODE : R/W ;bitpos:[7] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUT7_MODE (BIT(7))
#define SPI_MEM_SPI_SMEM_DOUT7_MODE_M (BIT(7))
#define SPI_MEM_SPI_SMEM_DOUT7_MODE_V 0x1
#define SPI_MEM_SPI_SMEM_DOUT7_MODE_S 7
/* SPI_MEM_SPI_SMEM_DOUT6_MODE : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUT6_MODE (BIT(6))
#define SPI_MEM_SPI_SMEM_DOUT6_MODE_M (BIT(6))
#define SPI_MEM_SPI_SMEM_DOUT6_MODE_V 0x1
#define SPI_MEM_SPI_SMEM_DOUT6_MODE_S 6
/* SPI_MEM_SPI_SMEM_DOUT5_MODE : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUT5_MODE (BIT(5))
#define SPI_MEM_SPI_SMEM_DOUT5_MODE_M (BIT(5))
#define SPI_MEM_SPI_SMEM_DOUT5_MODE_V 0x1
#define SPI_MEM_SPI_SMEM_DOUT5_MODE_S 5
/* SPI_MEM_SPI_SMEM_DOUT4_MODE : R/W ;bitpos:[4] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUT4_MODE (BIT(4))
#define SPI_MEM_SPI_SMEM_DOUT4_MODE_M (BIT(4))
#define SPI_MEM_SPI_SMEM_DOUT4_MODE_V 0x1
#define SPI_MEM_SPI_SMEM_DOUT4_MODE_S 4
/* SPI_MEM_SPI_SMEM_DOUT3_MODE : R/W ;bitpos:[3] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUT3_MODE (BIT(3))
#define SPI_MEM_SPI_SMEM_DOUT3_MODE_M (BIT(3))
#define SPI_MEM_SPI_SMEM_DOUT3_MODE_V 0x1
#define SPI_MEM_SPI_SMEM_DOUT3_MODE_S 3
/* SPI_MEM_SPI_SMEM_DOUT2_MODE : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUT2_MODE (BIT(2))
#define SPI_MEM_SPI_SMEM_DOUT2_MODE_M (BIT(2))
#define SPI_MEM_SPI_SMEM_DOUT2_MODE_V 0x1
#define SPI_MEM_SPI_SMEM_DOUT2_MODE_S 2
/* SPI_MEM_SPI_SMEM_DOUT1_MODE : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUT1_MODE (BIT(1))
#define SPI_MEM_SPI_SMEM_DOUT1_MODE_M (BIT(1))
#define SPI_MEM_SPI_SMEM_DOUT1_MODE_V 0x1
#define SPI_MEM_SPI_SMEM_DOUT1_MODE_S 1
/* SPI_MEM_SPI_SMEM_DOUT0_MODE : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles  0: output
 without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
#define SPI_MEM_SPI_SMEM_DOUT0_MODE (BIT(0))
#define SPI_MEM_SPI_SMEM_DOUT0_MODE_M (BIT(0))
#define SPI_MEM_SPI_SMEM_DOUT0_MODE_V 0x1
#define SPI_MEM_SPI_SMEM_DOUT0_MODE_S 0

#define SPI_MEM_SPI_SMEM_AC_REG(i) (REG_SPI_MEM_BASE(i) + 0x0D0)
/* SPI_MEM_SPI_SMEM_CS_HOLD_DELAY : R/W ;bitpos:[30:25] ;default: 6'd0 ; */
/*description: These bits are used to set the minimum CS high time tSHSL between
 SPI burst transfer when accesses to external RAM. tSHSL is (SPI_SMEM_CS_HOLD_DELAY[5:0] + 1) MSPI core clock cycles.*/
#define SPI_MEM_SPI_SMEM_CS_HOLD_DELAY 0x0000003F
#define SPI_MEM_SPI_SMEM_CS_HOLD_DELAY_M ((SPI_MEM_SPI_SMEM_CS_HOLD_DELAY_V) << (SPI_MEM_SPI_SMEM_CS_HOLD_DELAY_S))
#define SPI_MEM_SPI_SMEM_CS_HOLD_DELAY_V 0x3F
#define SPI_MEM_SPI_SMEM_CS_HOLD_DELAY_S 25
/* SPI_MEM_SPI_SMEM_ECC_16TO18_BYTE_EN : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: Set this bit to enable MSPI ECC 16 bytes data with 2 ECC bytes
 mode when accesses external RAM.*/
#define SPI_MEM_SPI_SMEM_ECC_16TO18_BYTE_EN (BIT(16))
#define SPI_MEM_SPI_SMEM_ECC_16TO18_BYTE_EN_M (BIT(16))
#define SPI_MEM_SPI_SMEM_ECC_16TO18_BYTE_EN_V 0x1
#define SPI_MEM_SPI_SMEM_ECC_16TO18_BYTE_EN_S 16
/* SPI_MEM_SPI_SMEM_ECC_SKIP_PAGE_CORNER : R/W ;bitpos:[15] ;default: 1'b1 ; */
/*description: 1: MSPI skips page corner when accesses external RAM. 0: Not
 skip page corner when accesses external RAM.*/
#define SPI_MEM_SPI_SMEM_ECC_SKIP_PAGE_CORNER (BIT(15))
#define SPI_MEM_SPI_SMEM_ECC_SKIP_PAGE_CORNER_M (BIT(15))
#define SPI_MEM_SPI_SMEM_ECC_SKIP_PAGE_CORNER_V 0x1
#define SPI_MEM_SPI_SMEM_ECC_SKIP_PAGE_CORNER_S 15
/* SPI_MEM_SPI_SMEM_ECC_CS_HOLD_TIME : R/W ;bitpos:[14:12] ;default: 3'd3 ; */
/*description: SPI_SMEM_CS_HOLD_TIME + SPI_SMEM_ECC_CS_HOLD_TIME is the MSPI
 CS hold cycles in ECC mode when accessed external RAM.*/
#define SPI_MEM_SPI_SMEM_ECC_CS_HOLD_TIME 0x00000007
#define SPI_MEM_SPI_SMEM_ECC_CS_HOLD_TIME_M ((SPI_MEM_SPI_SMEM_ECC_CS_HOLD_TIME_V) << (SPI_MEM_SPI_SMEM_ECC_CS_HOLD_TIME_S))
#define SPI_MEM_SPI_SMEM_ECC_CS_HOLD_TIME_V 0x7
#define SPI_MEM_SPI_SMEM_ECC_CS_HOLD_TIME_S 12
/* SPI_MEM_SPI_SMEM_CS_HOLD_TIME : R/W ;bitpos:[11:7] ;default: 5'h1 ; */
/*description: For spi0  spi cs signal is delayed to inactive by spi clock this
 bits are combined with spi_mem_cs_hold bit.*/
#define SPI_MEM_SPI_SMEM_CS_HOLD_TIME 0x0000001F
#define SPI_MEM_SPI_SMEM_CS_HOLD_TIME_M ((SPI_MEM_SPI_SMEM_CS_HOLD_TIME_V) << (SPI_MEM_SPI_SMEM_CS_HOLD_TIME_S))
#define SPI_MEM_SPI_SMEM_CS_HOLD_TIME_V 0x1F
#define SPI_MEM_SPI_SMEM_CS_HOLD_TIME_S 7
/* SPI_MEM_SPI_SMEM_CS_SETUP_TIME : R/W ;bitpos:[6:2] ;default: 5'h1 ; */
/*description: For spi0  (cycles-1) of prepare phase by spi clock this bits
 are combined with spi_mem_cs_setup bit.*/
#define SPI_MEM_SPI_SMEM_CS_SETUP_TIME 0x0000001F
#define SPI_MEM_SPI_SMEM_CS_SETUP_TIME_M ((SPI_MEM_SPI_SMEM_CS_SETUP_TIME_V) << (SPI_MEM_SPI_SMEM_CS_SETUP_TIME_S))
#define SPI_MEM_SPI_SMEM_CS_SETUP_TIME_V 0x1F
#define SPI_MEM_SPI_SMEM_CS_SETUP_TIME_S 2
/* SPI_MEM_SPI_SMEM_CS_HOLD : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: For spi0  spi cs keep low when spi is in done phase. 1: enable 0: disable.*/
#define SPI_MEM_SPI_SMEM_CS_HOLD (BIT(1))
#define SPI_MEM_SPI_SMEM_CS_HOLD_M (BIT(1))
#define SPI_MEM_SPI_SMEM_CS_HOLD_V 0x1
#define SPI_MEM_SPI_SMEM_CS_HOLD_S 1
/* SPI_MEM_SPI_SMEM_CS_SETUP : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: For spi0  spi cs is enable when spi is in prepare phase. 1: enable 0: disable.*/
#define SPI_MEM_SPI_SMEM_CS_SETUP (BIT(0))
#define SPI_MEM_SPI_SMEM_CS_SETUP_M (BIT(0))
#define SPI_MEM_SPI_SMEM_CS_SETUP_V 0x1
#define SPI_MEM_SPI_SMEM_CS_SETUP_S 0

#define SPI_MEM_DDR_REG(i) (REG_SPI_MEM_BASE(i) + 0x0D4)
/* SPI_MEM_SPI_FMEM_HYPERBUS_CA : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: Set this bit to enable HyperRAM address out when accesses to
 flash  which means ADDR_OUT[31:0] = {spi_usr_addr_value[19:4]  13'd0  spi_usr_addr_value[3:1]}.*/
#define SPI_MEM_SPI_FMEM_HYPERBUS_CA (BIT(30))
#define SPI_MEM_SPI_FMEM_HYPERBUS_CA_M (BIT(30))
#define SPI_MEM_SPI_FMEM_HYPERBUS_CA_V 0x1
#define SPI_MEM_SPI_FMEM_HYPERBUS_CA_S 30
/* SPI_MEM_SPI_FMEM_OCTA_RAM_ADDR : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: Set this bit to enable octa_ram address out when accesses to
 flash  which means ADDR_OUT[31:0] = {spi_usr_addr_value[25:4]  6'd0  spi_usr_addr_value[3:1]  1'b0}.*/
#define SPI_MEM_SPI_FMEM_OCTA_RAM_ADDR (BIT(29))
#define SPI_MEM_SPI_FMEM_OCTA_RAM_ADDR_M (BIT(29))
#define SPI_MEM_SPI_FMEM_OCTA_RAM_ADDR_V 0x1
#define SPI_MEM_SPI_FMEM_OCTA_RAM_ADDR_S 29
/* SPI_MEM_SPI_FMEM_CLK_DIFF_INV : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: Set this bit to invert SPI_DIFF when accesses to flash. .*/
#define SPI_MEM_SPI_FMEM_CLK_DIFF_INV (BIT(28))
#define SPI_MEM_SPI_FMEM_CLK_DIFF_INV_M (BIT(28))
#define SPI_MEM_SPI_FMEM_CLK_DIFF_INV_V 0x1
#define SPI_MEM_SPI_FMEM_CLK_DIFF_INV_S 28
/* SPI_MEM_SPI_FMEM_HYPERBUS_DUMMY_2X : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: Set this bit to enable the vary dummy function in SPI HyperBus
 mode  when SPI0 accesses flash or SPI1 accesses flash or sram.*/
#define SPI_MEM_SPI_FMEM_HYPERBUS_DUMMY_2X (BIT(27))
#define SPI_MEM_SPI_FMEM_HYPERBUS_DUMMY_2X_M (BIT(27))
#define SPI_MEM_SPI_FMEM_HYPERBUS_DUMMY_2X_V 0x1
#define SPI_MEM_SPI_FMEM_HYPERBUS_DUMMY_2X_S 27
/* SPI_MEM_SPI_FMEM_DQS_CA_IN : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: Set this bit to enable the input of SPI_DQS signal in SPI phases
 of CMD and ADDR.*/
#define SPI_MEM_SPI_FMEM_DQS_CA_IN (BIT(26))
#define SPI_MEM_SPI_FMEM_DQS_CA_IN_M (BIT(26))
#define SPI_MEM_SPI_FMEM_DQS_CA_IN_V 0x1
#define SPI_MEM_SPI_FMEM_DQS_CA_IN_S 26
/* SPI_MEM_SPI_FMEM_HYPERBUS_MODE : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: Set this bit to enable the SPI HyperBus mode.*/
#define SPI_MEM_SPI_FMEM_HYPERBUS_MODE (BIT(25))
#define SPI_MEM_SPI_FMEM_HYPERBUS_MODE_M (BIT(25))
#define SPI_MEM_SPI_FMEM_HYPERBUS_MODE_V 0x1
#define SPI_MEM_SPI_FMEM_HYPERBUS_MODE_S 25
/* SPI_MEM_SPI_FMEM_CLK_DIFF_EN : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: Set this bit to enable the differential SPI_CLK#.*/
#define SPI_MEM_SPI_FMEM_CLK_DIFF_EN (BIT(24))
#define SPI_MEM_SPI_FMEM_CLK_DIFF_EN_M (BIT(24))
#define SPI_MEM_SPI_FMEM_CLK_DIFF_EN_V 0x1
#define SPI_MEM_SPI_FMEM_CLK_DIFF_EN_S 24
/* SPI_MEM_SPI_FMEM_DDR_DQS_LOOP_MODE : R/W ;bitpos:[23:22] ;default: 2'b0 ; */
/*description: the bits are combined with the bit spi_fmem_ddr_fdqs_loop which
 used to select data strobe generating mode in ddr mode.*/
#define SPI_MEM_SPI_FMEM_DDR_DQS_LOOP_MODE 0x00000003
#define SPI_MEM_SPI_FMEM_DDR_DQS_LOOP_MODE_M ((SPI_MEM_SPI_FMEM_DDR_DQS_LOOP_MODE_V) << (SPI_MEM_SPI_FMEM_DDR_DQS_LOOP_MODE_S))
#define SPI_MEM_SPI_FMEM_DDR_DQS_LOOP_MODE_V 0x3
#define SPI_MEM_SPI_FMEM_DDR_DQS_LOOP_MODE_S 22
/* SPI_MEM_SPI_FMEM_DDR_DQS_LOOP : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: the data strobe is generated by SPI.*/
#define SPI_MEM_SPI_FMEM_DDR_DQS_LOOP (BIT(21))
#define SPI_MEM_SPI_FMEM_DDR_DQS_LOOP_M (BIT(21))
#define SPI_MEM_SPI_FMEM_DDR_DQS_LOOP_V 0x1
#define SPI_MEM_SPI_FMEM_DDR_DQS_LOOP_S 21
/* SPI_MEM_SPI_FMEM_USR_DDR_DQS_THD : R/W ;bitpos:[20:14] ;default: 7'b0 ; */
/*description: The delay number of data strobe which from memory based on SPI clock.*/
#define SPI_MEM_SPI_FMEM_USR_DDR_DQS_THD 0x0000007F
#define SPI_MEM_SPI_FMEM_USR_DDR_DQS_THD_M ((SPI_MEM_SPI_FMEM_USR_DDR_DQS_THD_V) << (SPI_MEM_SPI_FMEM_USR_DDR_DQS_THD_S))
#define SPI_MEM_SPI_FMEM_USR_DDR_DQS_THD_V 0x7F
#define SPI_MEM_SPI_FMEM_USR_DDR_DQS_THD_S 14
/* SPI_MEM_SPI_FMEM_RX_DDR_MSK_EN : R/W ;bitpos:[13] ;default: 1'h1 ; */
/*description: Set this bit to mask the first or the last byte in MSPI ECC DDR
 read mode  when accesses to flash.*/
#define SPI_MEM_SPI_FMEM_RX_DDR_MSK_EN (BIT(13))
#define SPI_MEM_SPI_FMEM_RX_DDR_MSK_EN_M (BIT(13))
#define SPI_MEM_SPI_FMEM_RX_DDR_MSK_EN_V 0x1
#define SPI_MEM_SPI_FMEM_RX_DDR_MSK_EN_S 13
/* SPI_MEM_SPI_FMEM_TX_DDR_MSK_EN : R/W ;bitpos:[12] ;default: 1'h1 ; */
/*description: Set this bit to mask the first or the last byte in MSPI ECC DDR
 write mode  when accesses to flash.*/
#define SPI_MEM_SPI_FMEM_TX_DDR_MSK_EN (BIT(12))
#define SPI_MEM_SPI_FMEM_TX_DDR_MSK_EN_M (BIT(12))
#define SPI_MEM_SPI_FMEM_TX_DDR_MSK_EN_V 0x1
#define SPI_MEM_SPI_FMEM_TX_DDR_MSK_EN_S 12
/* SPI_MEM_SPI_FMEM_OUTMINBYTELEN : R/W ;bitpos:[11:5] ;default: 7'b1 ; */
/*description: It is the minimum output data length in the panda device.*/
#define SPI_MEM_SPI_FMEM_OUTMINBYTELEN 0x0000007F
#define SPI_MEM_SPI_FMEM_OUTMINBYTELEN_M ((SPI_MEM_SPI_FMEM_OUTMINBYTELEN_V) << (SPI_MEM_SPI_FMEM_OUTMINBYTELEN_S))
#define SPI_MEM_SPI_FMEM_OUTMINBYTELEN_V 0x7F
#define SPI_MEM_SPI_FMEM_OUTMINBYTELEN_S 5
/* SPI_MEM_SPI_FMEM_DDR_CMD_DIS : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: the bit is used to disable dual edge in command phase when ddr mode.*/
#define SPI_MEM_SPI_FMEM_DDR_CMD_DIS (BIT(4))
#define SPI_MEM_SPI_FMEM_DDR_CMD_DIS_M (BIT(4))
#define SPI_MEM_SPI_FMEM_DDR_CMD_DIS_V 0x1
#define SPI_MEM_SPI_FMEM_DDR_CMD_DIS_S 4
/* SPI_MEM_SPI_FMEM_DDR_WDAT_SWP : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set the bit to reorder tx data of the word in spi ddr mode.*/
#define SPI_MEM_SPI_FMEM_DDR_WDAT_SWP (BIT(3))
#define SPI_MEM_SPI_FMEM_DDR_WDAT_SWP_M (BIT(3))
#define SPI_MEM_SPI_FMEM_DDR_WDAT_SWP_V 0x1
#define SPI_MEM_SPI_FMEM_DDR_WDAT_SWP_S 3
/* SPI_MEM_SPI_FMEM_DDR_RDAT_SWP : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set the bit to reorder rx data of the word in spi ddr mode.*/
#define SPI_MEM_SPI_FMEM_DDR_RDAT_SWP (BIT(2))
#define SPI_MEM_SPI_FMEM_DDR_RDAT_SWP_M (BIT(2))
#define SPI_MEM_SPI_FMEM_DDR_RDAT_SWP_V 0x1
#define SPI_MEM_SPI_FMEM_DDR_RDAT_SWP_S 2
/* SPI_MEM_SPI_FMEM_VAR_DUMMY : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set the bit to enable variable dummy cycle in spi ddr mode.*/
#define SPI_MEM_SPI_FMEM_VAR_DUMMY (BIT(1))
#define SPI_MEM_SPI_FMEM_VAR_DUMMY_M (BIT(1))
#define SPI_MEM_SPI_FMEM_VAR_DUMMY_V 0x1
#define SPI_MEM_SPI_FMEM_VAR_DUMMY_S 1
/* SPI_MEM_SPI_FMEM_DDR_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: 1: in ddr mode   0 in sdr mode*/
#define SPI_MEM_SPI_FMEM_DDR_EN (BIT(0))
#define SPI_MEM_SPI_FMEM_DDR_EN_M (BIT(0))
#define SPI_MEM_SPI_FMEM_DDR_EN_V 0x1
#define SPI_MEM_SPI_FMEM_DDR_EN_S 0

#define SPI_MEM_SPI_SMEM_DDR_REG(i) (REG_SPI_MEM_BASE(i) + 0x0D8)
/* SPI_MEM_SPI_SMEM_HYPERBUS_CA : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: Set this bit to enable HyperRAM address out when accesses to
 external RAM  which means ADDR_OUT[31:0] = {spi_usr_addr_value[19:4]  13'd0  spi_usr_addr_value[3:1]}.*/
#define SPI_MEM_SPI_SMEM_HYPERBUS_CA (BIT(30))
#define SPI_MEM_SPI_SMEM_HYPERBUS_CA_M (BIT(30))
#define SPI_MEM_SPI_SMEM_HYPERBUS_CA_V 0x1
#define SPI_MEM_SPI_SMEM_HYPERBUS_CA_S 30
/* SPI_MEM_SPI_SMEM_OCTA_RAM_ADDR : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: Set this bit to enable octa_ram address out when accesses to
 external RAM  which means ADDR_OUT[31:0] = {spi_usr_addr_value[25:4]  6'd0  spi_usr_addr_value[3:1]  1'b0}.*/
#define SPI_MEM_SPI_SMEM_OCTA_RAM_ADDR (BIT(29))
#define SPI_MEM_SPI_SMEM_OCTA_RAM_ADDR_M (BIT(29))
#define SPI_MEM_SPI_SMEM_OCTA_RAM_ADDR_V 0x1
#define SPI_MEM_SPI_SMEM_OCTA_RAM_ADDR_S 29
/* SPI_MEM_SPI_SMEM_CLK_DIFF_INV : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: Set this bit to invert SPI_DIFF when accesses to external RAM. .*/
#define SPI_MEM_SPI_SMEM_CLK_DIFF_INV (BIT(28))
#define SPI_MEM_SPI_SMEM_CLK_DIFF_INV_M (BIT(28))
#define SPI_MEM_SPI_SMEM_CLK_DIFF_INV_V 0x1
#define SPI_MEM_SPI_SMEM_CLK_DIFF_INV_S 28
/* SPI_MEM_SPI_SMEM_HYPERBUS_DUMMY_2X : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: Set this bit to enable the vary dummy function in SPI HyperBus
 mode  when SPI0 accesses flash or SPI1 accesses flash or sram.*/
#define SPI_MEM_SPI_SMEM_HYPERBUS_DUMMY_2X (BIT(27))
#define SPI_MEM_SPI_SMEM_HYPERBUS_DUMMY_2X_M (BIT(27))
#define SPI_MEM_SPI_SMEM_HYPERBUS_DUMMY_2X_V 0x1
#define SPI_MEM_SPI_SMEM_HYPERBUS_DUMMY_2X_S 27
/* SPI_MEM_SPI_SMEM_DQS_CA_IN : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: Set this bit to enable the input of SPI_DQS signal in SPI phases
 of CMD and ADDR.*/
#define SPI_MEM_SPI_SMEM_DQS_CA_IN (BIT(26))
#define SPI_MEM_SPI_SMEM_DQS_CA_IN_M (BIT(26))
#define SPI_MEM_SPI_SMEM_DQS_CA_IN_V 0x1
#define SPI_MEM_SPI_SMEM_DQS_CA_IN_S 26
/* SPI_MEM_SPI_SMEM_HYPERBUS_MODE : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: Set this bit to enable the SPI HyperBus mode.*/
#define SPI_MEM_SPI_SMEM_HYPERBUS_MODE (BIT(25))
#define SPI_MEM_SPI_SMEM_HYPERBUS_MODE_M (BIT(25))
#define SPI_MEM_SPI_SMEM_HYPERBUS_MODE_V 0x1
#define SPI_MEM_SPI_SMEM_HYPERBUS_MODE_S 25
/* SPI_MEM_SPI_SMEM_CLK_DIFF_EN : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: Set this bit to enable the differential SPI_CLK#.*/
#define SPI_MEM_SPI_SMEM_CLK_DIFF_EN (BIT(24))
#define SPI_MEM_SPI_SMEM_CLK_DIFF_EN_M (BIT(24))
#define SPI_MEM_SPI_SMEM_CLK_DIFF_EN_V 0x1
#define SPI_MEM_SPI_SMEM_CLK_DIFF_EN_S 24
/* SPI_MEM_SPI_SMEM_DDR_DQS_LOOP_MODE : R/W ;bitpos:[23:22] ;default: 2'b0 ; */
/*description: the bits are combined with the bit spi_smem_ddr_fdqs_loop which
 used to select data strobe generating mode in ddr mode.*/
#define SPI_MEM_SPI_SMEM_DDR_DQS_LOOP_MODE 0x00000003
#define SPI_MEM_SPI_SMEM_DDR_DQS_LOOP_MODE_M ((SPI_MEM_SPI_SMEM_DDR_DQS_LOOP_MODE_V) << (SPI_MEM_SPI_SMEM_DDR_DQS_LOOP_MODE_S))
#define SPI_MEM_SPI_SMEM_DDR_DQS_LOOP_MODE_V 0x3
#define SPI_MEM_SPI_SMEM_DDR_DQS_LOOP_MODE_S 22
/* SPI_MEM_SPI_SMEM_DDR_DQS_LOOP : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: the data strobe is generated by SPI.*/
#define SPI_MEM_SPI_SMEM_DDR_DQS_LOOP (BIT(21))
#define SPI_MEM_SPI_SMEM_DDR_DQS_LOOP_M (BIT(21))
#define SPI_MEM_SPI_SMEM_DDR_DQS_LOOP_V 0x1
#define SPI_MEM_SPI_SMEM_DDR_DQS_LOOP_S 21
/* SPI_MEM_SPI_SMEM_USR_DDR_DQS_THD : R/W ;bitpos:[20:14] ;default: 7'b0 ; */
/*description: The delay number of data strobe which from memory based on SPI clock.*/
#define SPI_MEM_SPI_SMEM_USR_DDR_DQS_THD 0x0000007F
#define SPI_MEM_SPI_SMEM_USR_DDR_DQS_THD_M ((SPI_MEM_SPI_SMEM_USR_DDR_DQS_THD_V) << (SPI_MEM_SPI_SMEM_USR_DDR_DQS_THD_S))
#define SPI_MEM_SPI_SMEM_USR_DDR_DQS_THD_V 0x7F
#define SPI_MEM_SPI_SMEM_USR_DDR_DQS_THD_S 14
/* SPI_MEM_SPI_SMEM_RX_DDR_MSK_EN : R/W ;bitpos:[13] ;default: 1'h1 ; */
/*description: Set this bit to mask the first or the last byte in MSPI ECC DDR
 read mode  when accesses to external RAM.*/
#define SPI_MEM_SPI_SMEM_RX_DDR_MSK_EN (BIT(13))
#define SPI_MEM_SPI_SMEM_RX_DDR_MSK_EN_M (BIT(13))
#define SPI_MEM_SPI_SMEM_RX_DDR_MSK_EN_V 0x1
#define SPI_MEM_SPI_SMEM_RX_DDR_MSK_EN_S 13
/* SPI_MEM_SPI_SMEM_TX_DDR_MSK_EN : R/W ;bitpos:[12] ;default: 1'h1 ; */
/*description: Set this bit to mask the first or the last byte in MSPI ECC DDR
 write mode  when accesses to external RAM.*/
#define SPI_MEM_SPI_SMEM_TX_DDR_MSK_EN (BIT(12))
#define SPI_MEM_SPI_SMEM_TX_DDR_MSK_EN_M (BIT(12))
#define SPI_MEM_SPI_SMEM_TX_DDR_MSK_EN_V 0x1
#define SPI_MEM_SPI_SMEM_TX_DDR_MSK_EN_S 12
/* SPI_MEM_SPI_SMEM_OUTMINBYTELEN : R/W ;bitpos:[11:5] ;default: 7'b1 ; */
/*description: It is the minimum output data length in the ddr psram.*/
#define SPI_MEM_SPI_SMEM_OUTMINBYTELEN 0x0000007F
#define SPI_MEM_SPI_SMEM_OUTMINBYTELEN_M ((SPI_MEM_SPI_SMEM_OUTMINBYTELEN_V) << (SPI_MEM_SPI_SMEM_OUTMINBYTELEN_S))
#define SPI_MEM_SPI_SMEM_OUTMINBYTELEN_V 0x7F
#define SPI_MEM_SPI_SMEM_OUTMINBYTELEN_S 5
/* SPI_MEM_SPI_SMEM_DDR_CMD_DIS : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: the bit is used to disable dual edge in command phase when ddr mode.*/
#define SPI_MEM_SPI_SMEM_DDR_CMD_DIS (BIT(4))
#define SPI_MEM_SPI_SMEM_DDR_CMD_DIS_M (BIT(4))
#define SPI_MEM_SPI_SMEM_DDR_CMD_DIS_V 0x1
#define SPI_MEM_SPI_SMEM_DDR_CMD_DIS_S 4
/* SPI_MEM_SPI_SMEM_DDR_WDAT_SWP : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set the bit to reorder tx data of the word in spi ddr mode.*/
#define SPI_MEM_SPI_SMEM_DDR_WDAT_SWP (BIT(3))
#define SPI_MEM_SPI_SMEM_DDR_WDAT_SWP_M (BIT(3))
#define SPI_MEM_SPI_SMEM_DDR_WDAT_SWP_V 0x1
#define SPI_MEM_SPI_SMEM_DDR_WDAT_SWP_S 3
/* SPI_MEM_SPI_SMEM_DDR_RDAT_SWP : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set the bit to reorder rx data of the word in spi ddr mode.*/
#define SPI_MEM_SPI_SMEM_DDR_RDAT_SWP (BIT(2))
#define SPI_MEM_SPI_SMEM_DDR_RDAT_SWP_M (BIT(2))
#define SPI_MEM_SPI_SMEM_DDR_RDAT_SWP_V 0x1
#define SPI_MEM_SPI_SMEM_DDR_RDAT_SWP_S 2
/* SPI_MEM_SPI_SMEM_VAR_DUMMY : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set the bit to enable variable dummy cycle in spi ddr mode.*/
#define SPI_MEM_SPI_SMEM_VAR_DUMMY (BIT(1))
#define SPI_MEM_SPI_SMEM_VAR_DUMMY_M (BIT(1))
#define SPI_MEM_SPI_SMEM_VAR_DUMMY_V 0x1
#define SPI_MEM_SPI_SMEM_VAR_DUMMY_S 1
/* SPI_MEM_SPI_SMEM_DDR_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: 1: in ddr mode   0 in sdr mode*/
#define SPI_MEM_SPI_SMEM_DDR_EN (BIT(0))
#define SPI_MEM_SPI_SMEM_DDR_EN_M (BIT(0))
#define SPI_MEM_SPI_SMEM_DDR_EN_V 0x1
#define SPI_MEM_SPI_SMEM_DDR_EN_S 0

#define SPI_MEM_CLOCK_GATE_REG(i) (REG_SPI_MEM_BASE(i) + 0x0DC)
/* SPI_MEM_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: Register clock gate enable signal. 1: Enable. 0: Disable.*/
#define SPI_MEM_CLK_EN (BIT(0))
#define SPI_MEM_CLK_EN_M (BIT(0))
#define SPI_MEM_CLK_EN_V 0x1
#define SPI_MEM_CLK_EN_S 0

#define SPI_MEM_CORE_CLK_SEL_REG(i) (REG_SPI_MEM_BASE(i) + 0x0E0)
/* SPI_MEM_SPI01_CLK_SEL : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: When the digital system clock selects PLL clock and the frequency
 of PLL clock is 480MHz  the value of reg_spi01_clk_sel:  0: SPI0/1 module clock (clk) is 80MHz. 1: SPI0/1 module clock (clk) is 120MHz.  2: SPI0/1 module clock (clk) 160MHz. 3: Not used. When the digital system clock selects PLL clock and the frequency of PLL clock is 320MHz  the value of reg_spi01_clk_sel:  0: SPI0/1 module clock (clk) is 80MHz. 1: SPI0/1 module clock (clk) is 80MHz.  2: SPI0/1 module clock (clk) 160MHz. 3: Not used.*/
#define SPI_MEM_SPI01_CLK_SEL 0x00000003
#define SPI_MEM_SPI01_CLK_SEL_M ((SPI_MEM_SPI01_CLK_SEL_V) << (SPI_MEM_SPI01_CLK_SEL_S))
#define SPI_MEM_SPI01_CLK_SEL_V 0x3
#define SPI_MEM_SPI01_CLK_SEL_S 0

#define SPI_MEM_DATE_REG(i) (REG_SPI_MEM_BASE(i) + 0x3FC)
/* SPI_MEM_DATE : R/W ;bitpos:[27:0] ;default: 28'h2004010 ; */
/*description: SPI register version.*/
#define SPI_MEM_DATE 0x0FFFFFFF
#define SPI_MEM_DATE_M ((SPI_MEM_DATE_V) << (SPI_MEM_DATE_S))
#define SPI_MEM_DATE_V 0xFFFFFFF
#define SPI_MEM_DATE_S 0

#ifdef __cplusplus
}
#endif
