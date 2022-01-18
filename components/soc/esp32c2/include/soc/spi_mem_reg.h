/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_SPI_MEM_REG_H_
#define _SOC_SPI_MEM_REG_H_


#include "soc.h"
#ifdef __cplusplus
extern "C" {
#endif

#define SPI_MEM_CMD_REG(i)          (REG_SPI_MEM_BASE(i) + 0x0)
/* SPI_MEM_FLASH_READ : R/W/SC ;bitpos:[31] ;default: 1'b0 ; */
/*description: Read flash enable. Read flash operation will be triggered when the bit is set. T
he bit will be cleared once the operation done. 1: enable 0: disable..*/
#define SPI_MEM_FLASH_READ    (BIT(31))
#define SPI_MEM_FLASH_READ_M  (BIT(31))
#define SPI_MEM_FLASH_READ_V  0x1
#define SPI_MEM_FLASH_READ_S  31
/* SPI_MEM_FLASH_WREN : R/W/SC ;bitpos:[30] ;default: 1'b0 ; */
/*description: Write flash enable.  Write enable command will be sent when the bit is set. The
bit will be cleared once the operation done. 1: enable 0: disable..*/
#define SPI_MEM_FLASH_WREN    (BIT(30))
#define SPI_MEM_FLASH_WREN_M  (BIT(30))
#define SPI_MEM_FLASH_WREN_V  0x1
#define SPI_MEM_FLASH_WREN_S  30
/* SPI_MEM_FLASH_WRDI : R/W/SC ;bitpos:[29] ;default: 1'b0 ; */
/*description: Write flash disable. Write disable command will be sent when the bit is set. The
 bit will be cleared once the operation done. 1: enable 0: disable..*/
#define SPI_MEM_FLASH_WRDI    (BIT(29))
#define SPI_MEM_FLASH_WRDI_M  (BIT(29))
#define SPI_MEM_FLASH_WRDI_V  0x1
#define SPI_MEM_FLASH_WRDI_S  29
/* SPI_MEM_FLASH_RDID : R/W/SC ;bitpos:[28] ;default: 1'b0 ; */
/*description: Read JEDEC ID . Read ID command will be sent when the bit is set. The bit will b
e cleared once the operation done. 1: enable 0: disable..*/
#define SPI_MEM_FLASH_RDID    (BIT(28))
#define SPI_MEM_FLASH_RDID_M  (BIT(28))
#define SPI_MEM_FLASH_RDID_V  0x1
#define SPI_MEM_FLASH_RDID_S  28
/* SPI_MEM_FLASH_RDSR : R/W/SC ;bitpos:[27] ;default: 1'b0 ; */
/*description: Read status register-1.  Read status operation will be triggered when the bit is
 set. The bit will be cleared once the operation done.1: enable 0: disable..*/
#define SPI_MEM_FLASH_RDSR    (BIT(27))
#define SPI_MEM_FLASH_RDSR_M  (BIT(27))
#define SPI_MEM_FLASH_RDSR_V  0x1
#define SPI_MEM_FLASH_RDSR_S  27
/* SPI_MEM_FLASH_WRSR : R/W/SC ;bitpos:[26] ;default: 1'b0 ; */
/*description: Write status register enable.   Write status operation  will be triggered when t
he bit is set. The bit will be cleared once the operation done.1: enable 0: disa
ble..*/
#define SPI_MEM_FLASH_WRSR    (BIT(26))
#define SPI_MEM_FLASH_WRSR_M  (BIT(26))
#define SPI_MEM_FLASH_WRSR_V  0x1
#define SPI_MEM_FLASH_WRSR_S  26
/* SPI_MEM_FLASH_PP : R/W/SC ;bitpos:[25] ;default: 1'b0 ; */
/*description: Page program enable(1 byte ~256 bytes data to be programmed). Page program opera
tion  will be triggered when the bit is set. The bit will be cleared once the op
eration done .1: enable 0: disable..*/
#define SPI_MEM_FLASH_PP    (BIT(25))
#define SPI_MEM_FLASH_PP_M  (BIT(25))
#define SPI_MEM_FLASH_PP_V  0x1
#define SPI_MEM_FLASH_PP_S  25
/* SPI_MEM_FLASH_SE : R/W/SC ;bitpos:[24] ;default: 1'b0 ; */
/*description: Sector erase enable(4KB). Sector erase operation will be triggered when the bit
is set. The bit will be cleared once the operation done.1: enable 0: disable..*/
#define SPI_MEM_FLASH_SE    (BIT(24))
#define SPI_MEM_FLASH_SE_M  (BIT(24))
#define SPI_MEM_FLASH_SE_V  0x1
#define SPI_MEM_FLASH_SE_S  24
/* SPI_MEM_FLASH_BE : R/W/SC ;bitpos:[23] ;default: 1'b0 ; */
/*description: Block erase enable(32KB) .  Block erase operation will be triggered when the bit
 is set. The bit will be cleared once the operation done.1: enable 0: disable..*/
#define SPI_MEM_FLASH_BE    (BIT(23))
#define SPI_MEM_FLASH_BE_M  (BIT(23))
#define SPI_MEM_FLASH_BE_V  0x1
#define SPI_MEM_FLASH_BE_S  23
/* SPI_MEM_FLASH_CE : R/W/SC ;bitpos:[22] ;default: 1'b0 ; */
/*description: Chip erase enable. Chip erase operation will be triggered when the bit is set. T
he bit will be cleared once the operation done.1: enable 0: disable..*/
#define SPI_MEM_FLASH_CE    (BIT(22))
#define SPI_MEM_FLASH_CE_M  (BIT(22))
#define SPI_MEM_FLASH_CE_V  0x1
#define SPI_MEM_FLASH_CE_S  22
/* SPI_MEM_FLASH_DP : R/W/SC ;bitpos:[21] ;default: 1'b0 ; */
/*description: Drive Flash into power down.  An operation will be triggered when the bit is set
. The bit will be cleared once the operation done.1: enable 0: disable..*/
#define SPI_MEM_FLASH_DP    (BIT(21))
#define SPI_MEM_FLASH_DP_M  (BIT(21))
#define SPI_MEM_FLASH_DP_V  0x1
#define SPI_MEM_FLASH_DP_S  21
/* SPI_MEM_FLASH_RES : R/W/SC ;bitpos:[20] ;default: 1'b0 ; */
/*description: This bit combined with reg_resandres bit releases Flash from the power-down stat
e or high performance mode and obtains the devices ID. The bit will be cleared o
nce the operation done.1: enable 0: disable..*/
#define SPI_MEM_FLASH_RES    (BIT(20))
#define SPI_MEM_FLASH_RES_M  (BIT(20))
#define SPI_MEM_FLASH_RES_V  0x1
#define SPI_MEM_FLASH_RES_S  20
/* SPI_MEM_FLASH_HPM : R/W/SC ;bitpos:[19] ;default: 1'b0 ; */
/*description: Drive Flash into high performance mode.  The bit will be cleared once the operat
ion done.1: enable 0: disable..*/
#define SPI_MEM_FLASH_HPM    (BIT(19))
#define SPI_MEM_FLASH_HPM_M  (BIT(19))
#define SPI_MEM_FLASH_HPM_V  0x1
#define SPI_MEM_FLASH_HPM_S  19
/* SPI_MEM_USR : R/W/SC ;bitpos:[18] ;default: 1'b0 ; */
/*description: User define command enable.  An operation will be triggered when the bit is set.
 The bit will be cleared once the operation done.1: enable 0: disable..*/
#define SPI_MEM_USR    (BIT(18))
#define SPI_MEM_USR_M  (BIT(18))
#define SPI_MEM_USR_V  0x1
#define SPI_MEM_USR_S  18
/* SPI_MEM_FLASH_PE : R/W/SC ;bitpos:[17] ;default: 1'b0 ; */
/*description: In user mode, it is set to indicate that program/erase operation will be trigger
ed. The bit is combined with spi_mem_usr bit. The bit will be cleared once the o
peration done.1: enable 0: disable..*/
#define SPI_MEM_FLASH_PE    (BIT(17))
#define SPI_MEM_FLASH_PE_M  (BIT(17))
#define SPI_MEM_FLASH_PE_V  0x1
#define SPI_MEM_FLASH_PE_S  17
/* SPI_MEM_MSPI_ST : RO ;bitpos:[7:4] ;default: 4'b0 ; */
/*description: The current status of SPI1 slave FSM: mspi_st. 0: idle state, 1: preparation sta
te, 2: send command state, 3: send address state, 4: wait state, 5: read data st
ate, 6:write data state, 7: done state, 8: read data end state..*/
#define SPI_MEM_MSPI_ST    0x0000000F
#define SPI_MEM_MSPI_ST_M  ((SPI_MEM_MSPI_ST_V)<<(SPI_MEM_MSPI_ST_S))
#define SPI_MEM_MSPI_ST_V  0xF
#define SPI_MEM_MSPI_ST_S  4
/* SPI_MEM_MST_ST : RO ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: The current status of SPI1 master FSM..*/
#define SPI_MEM_MST_ST    0x0000000F
#define SPI_MEM_MST_ST_M  ((SPI_MEM_MST_ST_V)<<(SPI_MEM_MST_ST_S))
#define SPI_MEM_MST_ST_V  0xF
#define SPI_MEM_MST_ST_S  0

#define SPI_MEM_ADDR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x4)
/* SPI_MEM_USR_ADDR_VALUE : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: In user mode, it is the memory address. other then the bit0-bit23 is the memory
address, the bit24-bit31 are the byte length of a transfer..*/
#define SPI_MEM_USR_ADDR_VALUE    0xFFFFFFFF
#define SPI_MEM_USR_ADDR_VALUE_M  ((SPI_MEM_USR_ADDR_VALUE_V)<<(SPI_MEM_USR_ADDR_VALUE_S))
#define SPI_MEM_USR_ADDR_VALUE_V  0xFFFFFFFF
#define SPI_MEM_USR_ADDR_VALUE_S  0

#define SPI_MEM_CTRL_REG(i)          (REG_SPI_MEM_BASE(i) + 0x8)
/* SPI_MEM_FREAD_QIO : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: In the read operations address phase and read-data phase apply 4 signals. 1: ena
ble 0: disable..*/
#define SPI_MEM_FREAD_QIO    (BIT(24))
#define SPI_MEM_FREAD_QIO_M  (BIT(24))
#define SPI_MEM_FREAD_QIO_V  0x1
#define SPI_MEM_FREAD_QIO_S  24
/* SPI_MEM_FREAD_DIO : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: In the read operations address phase and read-data phase apply 2 signals. 1: ena
ble 0: disable..*/
#define SPI_MEM_FREAD_DIO    (BIT(23))
#define SPI_MEM_FREAD_DIO_M  (BIT(23))
#define SPI_MEM_FREAD_DIO_V  0x1
#define SPI_MEM_FREAD_DIO_S  23
/* SPI_MEM_WRSR_2B : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: two bytes data will be written to status register when it is set. 1: enable 0: d
isable..*/
#define SPI_MEM_WRSR_2B    (BIT(22))
#define SPI_MEM_WRSR_2B_M  (BIT(22))
#define SPI_MEM_WRSR_2B_V  0x1
#define SPI_MEM_WRSR_2B_S  22
/* SPI_MEM_WP_REG : R/W ;bitpos:[21] ;default: 1'b1 ; */
/*description: Write protect signal output when SPI is idle.  1: output high, 0: output low..*/
#define SPI_MEM_WP_REG    (BIT(21))
#define SPI_MEM_WP_REG_M  (BIT(21))
#define SPI_MEM_WP_REG_V  0x1
#define SPI_MEM_WP_REG_S  21
/* SPI_MEM_FREAD_QUAD : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: In the read operations read-data phase apply 4 signals. 1: enable 0: disable..*/
#define SPI_MEM_FREAD_QUAD    (BIT(20))
#define SPI_MEM_FREAD_QUAD_M  (BIT(20))
#define SPI_MEM_FREAD_QUAD_V  0x1
#define SPI_MEM_FREAD_QUAD_S  20
/* SPI_MEM_D_POL : R/W ;bitpos:[19] ;default: 1'b1 ; */
/*description: The bit is used to set MOSI line polarity, 1: high 0, low.*/
#define SPI_MEM_D_POL    (BIT(19))
#define SPI_MEM_D_POL_M  (BIT(19))
#define SPI_MEM_D_POL_V  0x1
#define SPI_MEM_D_POL_S  19
/* SPI_MEM_Q_POL : R/W ;bitpos:[18] ;default: 1'b1 ; */
/*description: The bit is used to set MISO line polarity, 1: high 0, low.*/
#define SPI_MEM_Q_POL    (BIT(18))
#define SPI_MEM_Q_POL_M  (BIT(18))
#define SPI_MEM_Q_POL_V  0x1
#define SPI_MEM_Q_POL_S  18
/* SPI_MEM_RESANDRES : R/W ;bitpos:[15] ;default: 1'b1 ; */
/*description: The Device ID is read out to SPI_MEM_RD_STATUS register,  this bit combine with
spi_mem_flash_res bit. 1: enable 0: disable..*/
#define SPI_MEM_RESANDRES    (BIT(15))
#define SPI_MEM_RESANDRES_M  (BIT(15))
#define SPI_MEM_RESANDRES_V  0x1
#define SPI_MEM_RESANDRES_S  15
/* SPI_MEM_FREAD_DUAL : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: In the read operations, read-data phase apply 2 signals. 1: enable 0: disable..*/
#define SPI_MEM_FREAD_DUAL    (BIT(14))
#define SPI_MEM_FREAD_DUAL_M  (BIT(14))
#define SPI_MEM_FREAD_DUAL_V  0x1
#define SPI_MEM_FREAD_DUAL_S  14
/* SPI_MEM_FASTRD_MODE : R/W ;bitpos:[13] ;default: 1'b1 ; */
/*description: This bit enable the bits: SPI_MEM_FREAD_QIO, SPI_MEM_FREAD_DIO, SPI_MEM_FREAD_QO
UT AND SPI_MEM_FREAD_DOUT. 1: enable 0: disable..*/
#define SPI_MEM_FASTRD_MODE    (BIT(13))
#define SPI_MEM_FASTRD_MODE_M  (BIT(13))
#define SPI_MEM_FASTRD_MODE_V  0x1
#define SPI_MEM_FASTRD_MODE_S  13
/* SPI_MEM_TX_CRC_EN : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: For SPI1,  enable crc32 when writing encrypted data to flash. 1: enable 0:disabl
e.*/
#define SPI_MEM_TX_CRC_EN    (BIT(11))
#define SPI_MEM_TX_CRC_EN_M  (BIT(11))
#define SPI_MEM_TX_CRC_EN_V  0x1
#define SPI_MEM_TX_CRC_EN_S  11
/* SPI_MEM_FCS_CRC_EN : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: For SPI1,  initialize crc32 module before writing encrypted data to flash. Activ
e low..*/
#define SPI_MEM_FCS_CRC_EN    (BIT(10))
#define SPI_MEM_FCS_CRC_EN_M  (BIT(10))
#define SPI_MEM_FCS_CRC_EN_V  0x1
#define SPI_MEM_FCS_CRC_EN_S  10
/* SPI_MEM_FCMD_QUAD : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Apply 4 signals during command phase 1:enable 0: disable.*/
#define SPI_MEM_FCMD_QUAD    (BIT(8))
#define SPI_MEM_FCMD_QUAD_M  (BIT(8))
#define SPI_MEM_FCMD_QUAD_V  0x1
#define SPI_MEM_FCMD_QUAD_S  8
/* SPI_MEM_FCMD_DUAL : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: Apply 2 signals during command phase 1:enable 0: disable.*/
#define SPI_MEM_FCMD_DUAL    (BIT(7))
#define SPI_MEM_FCMD_DUAL_M  (BIT(7))
#define SPI_MEM_FCMD_DUAL_V  0x1
#define SPI_MEM_FCMD_DUAL_S  7
/* SPI_MEM_FDUMMY_OUT : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: In the dummy phase the signal level of spi is output by the spi controller..*/
#define SPI_MEM_FDUMMY_OUT    (BIT(3))
#define SPI_MEM_FDUMMY_OUT_M  (BIT(3))
#define SPI_MEM_FDUMMY_OUT_V  0x1
#define SPI_MEM_FDUMMY_OUT_S  3

#define SPI_MEM_CTRL1_REG(i)          (REG_SPI_MEM_BASE(i) + 0xC)
/* SPI_MEM_RXFIFO_RST : WT ;bitpos:[30] ;default: 1'b0 ; */
/*description: SPI0 RX FIFO reset signal..*/
#define SPI_MEM_RXFIFO_RST    (BIT(30))
#define SPI_MEM_RXFIFO_RST_M  (BIT(30))
#define SPI_MEM_RXFIFO_RST_V  0x1
#define SPI_MEM_RXFIFO_RST_S  30
/* SPI_MEM_CS_HOLD_DLY_RES : R/W ;bitpos:[11:2] ;default: 10'h3ff ; */
/*description: After RES/DP/HPM command is sent, SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 5
12) SPI_CLK cycles..*/
#define SPI_MEM_CS_HOLD_DLY_RES    0x000003FF
#define SPI_MEM_CS_HOLD_DLY_RES_M  ((SPI_MEM_CS_HOLD_DLY_RES_V)<<(SPI_MEM_CS_HOLD_DLY_RES_S))
#define SPI_MEM_CS_HOLD_DLY_RES_V  0x3FF
#define SPI_MEM_CS_HOLD_DLY_RES_S  2
/* SPI_MEM_CLK_MODE : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delaye
d one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inacti
ve 3: SPI clock is alwasy on..*/
#define SPI_MEM_CLK_MODE    0x00000003
#define SPI_MEM_CLK_MODE_M  ((SPI_MEM_CLK_MODE_V)<<(SPI_MEM_CLK_MODE_S))
#define SPI_MEM_CLK_MODE_V  0x3
#define SPI_MEM_CLK_MODE_S  0

#define SPI_MEM_CTRL2_REG(i)          (REG_SPI_MEM_BASE(i) + 0x10)
/* SPI_MEM_SYNC_RESET : WT ;bitpos:[31] ;default: 1'b0 ; */
/*description: The FSM will be reset..*/
#define SPI_MEM_SYNC_RESET    (BIT(31))
#define SPI_MEM_SYNC_RESET_M  (BIT(31))
#define SPI_MEM_SYNC_RESET_V  0x1
#define SPI_MEM_SYNC_RESET_S  31
/* SPI_MEM_CS_HOLD_DELAY : R/W ;bitpos:[30:25] ;default: 6'd0 ; */
/*description: These bits are used to set the minimum CS high time tSHSL between SPI burst tran
sfer when accesses to flash. tSHSL is (SPI_MEM_CS_HOLD_DELAY[5:0] + 1) MSPI core
 clock cycles..*/
#define SPI_MEM_CS_HOLD_DELAY    0x0000003F
#define SPI_MEM_CS_HOLD_DELAY_M  ((SPI_MEM_CS_HOLD_DELAY_V)<<(SPI_MEM_CS_HOLD_DELAY_S))
#define SPI_MEM_CS_HOLD_DELAY_V  0x3F
#define SPI_MEM_CS_HOLD_DELAY_S  25
/* SPI_MEM_CS_HOLD_TIME : R/W ;bitpos:[9:5] ;default: 5'h1 ; */
/*description: Spi cs signal is delayed to inactive by spi clock this bits are combined with sp
i_mem_cs_hold bit..*/
#define SPI_MEM_CS_HOLD_TIME    0x0000001F
#define SPI_MEM_CS_HOLD_TIME_M  ((SPI_MEM_CS_HOLD_TIME_V)<<(SPI_MEM_CS_HOLD_TIME_S))
#define SPI_MEM_CS_HOLD_TIME_V  0x1F
#define SPI_MEM_CS_HOLD_TIME_S  5
/* SPI_MEM_CS_SETUP_TIME : R/W ;bitpos:[4:0] ;default: 5'h1 ; */
/*description: (cycles-1) of prepare phase by spi clock this bits are combined with spi_mem_cs_
setup bit..*/
#define SPI_MEM_CS_SETUP_TIME    0x0000001F
#define SPI_MEM_CS_SETUP_TIME_M  ((SPI_MEM_CS_SETUP_TIME_V)<<(SPI_MEM_CS_SETUP_TIME_S))
#define SPI_MEM_CS_SETUP_TIME_V  0x1F
#define SPI_MEM_CS_SETUP_TIME_S  0

#define SPI_MEM_CLOCK_REG(i)          (REG_SPI_MEM_BASE(i) + 0x14)
/* SPI_MEM_CLK_EQU_SYSCLK : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: Set this bit in 1-division mode..*/
#define SPI_MEM_CLK_EQU_SYSCLK    (BIT(31))
#define SPI_MEM_CLK_EQU_SYSCLK_M  (BIT(31))
#define SPI_MEM_CLK_EQU_SYSCLK_V  0x1
#define SPI_MEM_CLK_EQU_SYSCLK_S  31
/* SPI_MEM_CLKCNT_N : R/W ;bitpos:[23:16] ;default: 8'h3 ; */
/*description: In the master mode it is the divider of spi_mem_clk. So spi_mem_clk frequency is
 system/(spi_mem_clkcnt_N+1).*/
#define SPI_MEM_CLKCNT_N    0x000000FF
#define SPI_MEM_CLKCNT_N_M  ((SPI_MEM_CLKCNT_N_V)<<(SPI_MEM_CLKCNT_N_S))
#define SPI_MEM_CLKCNT_N_V  0xFF
#define SPI_MEM_CLKCNT_N_S  16
/* SPI_MEM_CLKCNT_H : R/W ;bitpos:[15:8] ;default: 8'h1 ; */
/*description: In the master mode it must be floor((spi_mem_clkcnt_N+1)/2-1)..*/
#define SPI_MEM_CLKCNT_H    0x000000FF
#define SPI_MEM_CLKCNT_H_M  ((SPI_MEM_CLKCNT_H_V)<<(SPI_MEM_CLKCNT_H_S))
#define SPI_MEM_CLKCNT_H_V  0xFF
#define SPI_MEM_CLKCNT_H_S  8
/* SPI_MEM_CLKCNT_L : R/W ;bitpos:[7:0] ;default: 8'h3 ; */
/*description: In the master mode it must be equal to spi_mem_clkcnt_N..*/
#define SPI_MEM_CLKCNT_L    0x000000FF
#define SPI_MEM_CLKCNT_L_M  ((SPI_MEM_CLKCNT_L_V)<<(SPI_MEM_CLKCNT_L_S))
#define SPI_MEM_CLKCNT_L_V  0xFF
#define SPI_MEM_CLKCNT_L_S  0

#define SPI_MEM_USER_REG(i)          (REG_SPI_MEM_BASE(i) + 0x18)
/* SPI_MEM_USR_COMMAND : R/W ;bitpos:[31] ;default: 1'b1 ; */
/*description: This bit enable the command phase of an operation..*/
#define SPI_MEM_USR_COMMAND    (BIT(31))
#define SPI_MEM_USR_COMMAND_M  (BIT(31))
#define SPI_MEM_USR_COMMAND_V  0x1
#define SPI_MEM_USR_COMMAND_S  31
/* SPI_MEM_USR_ADDR : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: This bit enable the address phase of an operation..*/
#define SPI_MEM_USR_ADDR    (BIT(30))
#define SPI_MEM_USR_ADDR_M  (BIT(30))
#define SPI_MEM_USR_ADDR_V  0x1
#define SPI_MEM_USR_ADDR_S  30
/* SPI_MEM_USR_DUMMY : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: This bit enable the dummy phase of an operation..*/
#define SPI_MEM_USR_DUMMY    (BIT(29))
#define SPI_MEM_USR_DUMMY_M  (BIT(29))
#define SPI_MEM_USR_DUMMY_V  0x1
#define SPI_MEM_USR_DUMMY_S  29
/* SPI_MEM_USR_MISO : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: This bit enable the read-data phase of an operation..*/
#define SPI_MEM_USR_MISO    (BIT(28))
#define SPI_MEM_USR_MISO_M  (BIT(28))
#define SPI_MEM_USR_MISO_V  0x1
#define SPI_MEM_USR_MISO_S  28
/* SPI_MEM_USR_MOSI : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: This bit enable the write-data phase of an operation..*/
#define SPI_MEM_USR_MOSI    (BIT(27))
#define SPI_MEM_USR_MOSI_M  (BIT(27))
#define SPI_MEM_USR_MOSI_V  0x1
#define SPI_MEM_USR_MOSI_S  27
/* SPI_MEM_USR_DUMMY_IDLE : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: spi clock is disable in dummy phase when the bit is enable..*/
#define SPI_MEM_USR_DUMMY_IDLE    (BIT(26))
#define SPI_MEM_USR_DUMMY_IDLE_M  (BIT(26))
#define SPI_MEM_USR_DUMMY_IDLE_V  0x1
#define SPI_MEM_USR_DUMMY_IDLE_S  26
/* SPI_MEM_USR_MOSI_HIGHPART : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: write-data phase only access to high-part of the buffer spi_mem_w8~spi_mem_w15.
1: enable 0: disable..*/
#define SPI_MEM_USR_MOSI_HIGHPART    (BIT(25))
#define SPI_MEM_USR_MOSI_HIGHPART_M  (BIT(25))
#define SPI_MEM_USR_MOSI_HIGHPART_V  0x1
#define SPI_MEM_USR_MOSI_HIGHPART_S  25
/* SPI_MEM_USR_MISO_HIGHPART : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: read-data phase only access to high-part of the buffer spi_mem_w8~spi_mem_w15. 1
: enable 0: disable..*/
#define SPI_MEM_USR_MISO_HIGHPART    (BIT(24))
#define SPI_MEM_USR_MISO_HIGHPART_M  (BIT(24))
#define SPI_MEM_USR_MISO_HIGHPART_V  0x1
#define SPI_MEM_USR_MISO_HIGHPART_S  24
/* SPI_MEM_FWRITE_QIO : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: In the write operations address phase and read-data phase apply 4 signals..*/
#define SPI_MEM_FWRITE_QIO    (BIT(15))
#define SPI_MEM_FWRITE_QIO_M  (BIT(15))
#define SPI_MEM_FWRITE_QIO_V  0x1
#define SPI_MEM_FWRITE_QIO_S  15
/* SPI_MEM_FWRITE_DIO : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: In the write operations address phase and read-data phase apply 2 signals..*/
#define SPI_MEM_FWRITE_DIO    (BIT(14))
#define SPI_MEM_FWRITE_DIO_M  (BIT(14))
#define SPI_MEM_FWRITE_DIO_V  0x1
#define SPI_MEM_FWRITE_DIO_S  14
/* SPI_MEM_FWRITE_QUAD : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: In the write operations read-data phase apply 4 signals.*/
#define SPI_MEM_FWRITE_QUAD    (BIT(13))
#define SPI_MEM_FWRITE_QUAD_M  (BIT(13))
#define SPI_MEM_FWRITE_QUAD_V  0x1
#define SPI_MEM_FWRITE_QUAD_S  13
/* SPI_MEM_FWRITE_DUAL : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: In the write operations read-data phase apply 2 signals.*/
#define SPI_MEM_FWRITE_DUAL    (BIT(12))
#define SPI_MEM_FWRITE_DUAL_M  (BIT(12))
#define SPI_MEM_FWRITE_DUAL_V  0x1
#define SPI_MEM_FWRITE_DUAL_S  12
/* SPI_MEM_CK_OUT_EDGE : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: the bit combined with spi_mem_mosi_delay_mode bits to set mosi signal delay mode
..*/
#define SPI_MEM_CK_OUT_EDGE    (BIT(9))
#define SPI_MEM_CK_OUT_EDGE_M  (BIT(9))
#define SPI_MEM_CK_OUT_EDGE_V  0x1
#define SPI_MEM_CK_OUT_EDGE_S  9
/* SPI_MEM_CS_SETUP : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: spi cs is enable when spi is in  prepare  phase. 1: enable 0: disable..*/
#define SPI_MEM_CS_SETUP    (BIT(7))
#define SPI_MEM_CS_SETUP_M  (BIT(7))
#define SPI_MEM_CS_SETUP_V  0x1
#define SPI_MEM_CS_SETUP_S  7
/* SPI_MEM_CS_HOLD : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: spi cs keep low when spi is in  done  phase. 1: enable 0: disable..*/
#define SPI_MEM_CS_HOLD    (BIT(6))
#define SPI_MEM_CS_HOLD_M  (BIT(6))
#define SPI_MEM_CS_HOLD_V  0x1
#define SPI_MEM_CS_HOLD_S  6

#define SPI_MEM_USER1_REG(i)          (REG_SPI_MEM_BASE(i) + 0x1C)
/* SPI_MEM_USR_ADDR_BITLEN : R/W ;bitpos:[31:26] ;default: 6'd23 ; */
/*description: The length in bits of address phase. The register value shall be (bit_num-1)..*/
#define SPI_MEM_USR_ADDR_BITLEN    0x0000003F
#define SPI_MEM_USR_ADDR_BITLEN_M  ((SPI_MEM_USR_ADDR_BITLEN_V)<<(SPI_MEM_USR_ADDR_BITLEN_S))
#define SPI_MEM_USR_ADDR_BITLEN_V  0x3F
#define SPI_MEM_USR_ADDR_BITLEN_S  26
/* SPI_MEM_USR_DUMMY_CYCLELEN : R/W ;bitpos:[5:0] ;default: 6'd7 ; */
/*description: The length in spi_mem_clk cycles of dummy phase. The register value shall be (cy
cle_num-1)..*/
#define SPI_MEM_USR_DUMMY_CYCLELEN    0x0000003F
#define SPI_MEM_USR_DUMMY_CYCLELEN_M  ((SPI_MEM_USR_DUMMY_CYCLELEN_V)<<(SPI_MEM_USR_DUMMY_CYCLELEN_S))
#define SPI_MEM_USR_DUMMY_CYCLELEN_V  0x3F
#define SPI_MEM_USR_DUMMY_CYCLELEN_S  0

#define SPI_MEM_USER2_REG(i)          (REG_SPI_MEM_BASE(i) + 0x20)
/* SPI_MEM_USR_COMMAND_BITLEN : R/W ;bitpos:[31:28] ;default: 4'd7 ; */
/*description: The length in bits of command phase. The register value shall be (bit_num-1).*/
#define SPI_MEM_USR_COMMAND_BITLEN    0x0000000F
#define SPI_MEM_USR_COMMAND_BITLEN_M  ((SPI_MEM_USR_COMMAND_BITLEN_V)<<(SPI_MEM_USR_COMMAND_BITLEN_S))
#define SPI_MEM_USR_COMMAND_BITLEN_V  0xF
#define SPI_MEM_USR_COMMAND_BITLEN_S  28
/* SPI_MEM_USR_COMMAND_VALUE : R/W ;bitpos:[15:0] ;default: 16'b0 ; */
/*description: The value of  command..*/
#define SPI_MEM_USR_COMMAND_VALUE    0x0000FFFF
#define SPI_MEM_USR_COMMAND_VALUE_M  ((SPI_MEM_USR_COMMAND_VALUE_V)<<(SPI_MEM_USR_COMMAND_VALUE_S))
#define SPI_MEM_USR_COMMAND_VALUE_V  0xFFFF
#define SPI_MEM_USR_COMMAND_VALUE_S  0

#define SPI_MEM_MOSI_DLEN_REG(i)          (REG_SPI_MEM_BASE(i) + 0x24)
/* SPI_MEM_USR_MOSI_DBITLEN : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: The length in bits of write-data. The register value shall be (bit_num-1)..*/
#define SPI_MEM_USR_MOSI_DBITLEN    0x000003FF
#define SPI_MEM_USR_MOSI_DBITLEN_M  ((SPI_MEM_USR_MOSI_DBITLEN_V)<<(SPI_MEM_USR_MOSI_DBITLEN_S))
#define SPI_MEM_USR_MOSI_DBITLEN_V  0x3FF
#define SPI_MEM_USR_MOSI_DBITLEN_S  0

#define SPI_MEM_MISO_DLEN_REG(i)          (REG_SPI_MEM_BASE(i) + 0x28)
/* SPI_MEM_USR_MISO_DBITLEN : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: The length in bits of  read-data. The register value shall be (bit_num-1)..*/
#define SPI_MEM_USR_MISO_DBITLEN    0x000003FF
#define SPI_MEM_USR_MISO_DBITLEN_M  ((SPI_MEM_USR_MISO_DBITLEN_V)<<(SPI_MEM_USR_MISO_DBITLEN_S))
#define SPI_MEM_USR_MISO_DBITLEN_V  0x3FF
#define SPI_MEM_USR_MISO_DBITLEN_S  0

#define SPI_MEM_RD_STATUS_REG(i)          (REG_SPI_MEM_BASE(i) + 0x2C)
/* SPI_MEM_WB_MODE : R/W ;bitpos:[23:16] ;default: 8'h00 ; */
/*description: Mode bits in the flash fast read mode  it is combined with spi_mem_fastrd_mode b
it..*/
#define SPI_MEM_WB_MODE    0x000000FF
#define SPI_MEM_WB_MODE_M  ((SPI_MEM_WB_MODE_V)<<(SPI_MEM_WB_MODE_S))
#define SPI_MEM_WB_MODE_V  0xFF
#define SPI_MEM_WB_MODE_S  16
/* SPI_MEM_STATUS : R/W/SS ;bitpos:[15:0] ;default: 16'b0 ; */
/*description: The value is stored when set spi_mem_flash_rdsr bit and spi_mem_flash_res bit..*/
#define SPI_MEM_STATUS    0x0000FFFF
#define SPI_MEM_STATUS_M  ((SPI_MEM_STATUS_V)<<(SPI_MEM_STATUS_S))
#define SPI_MEM_STATUS_V  0xFFFF
#define SPI_MEM_STATUS_S  0

#define SPI_MEM_MISC_REG(i)          (REG_SPI_MEM_BASE(i) + 0x34)
/* SPI_MEM_CS_KEEP_ACTIVE : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: spi cs line keep low when the bit is set..*/
#define SPI_MEM_CS_KEEP_ACTIVE    (BIT(10))
#define SPI_MEM_CS_KEEP_ACTIVE_M  (BIT(10))
#define SPI_MEM_CS_KEEP_ACTIVE_V  0x1
#define SPI_MEM_CS_KEEP_ACTIVE_S  10
/* SPI_MEM_CK_IDLE_EDGE : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: 1: spi clk line is high when idle     0: spi clk line is low when idle.*/
#define SPI_MEM_CK_IDLE_EDGE    (BIT(9))
#define SPI_MEM_CK_IDLE_EDGE_M  (BIT(9))
#define SPI_MEM_CK_IDLE_EDGE_V  0x1
#define SPI_MEM_CK_IDLE_EDGE_S  9
/* SPI_MEM_CSPI_ST_TRANS_END_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The bit is used to enable the interrupt of spi0_slv_st controlled transmitting i
s done..*/
#define SPI_MEM_CSPI_ST_TRANS_END_INT_ENA    (BIT(6))
#define SPI_MEM_CSPI_ST_TRANS_END_INT_ENA_M  (BIT(6))
#define SPI_MEM_CSPI_ST_TRANS_END_INT_ENA_V  0x1
#define SPI_MEM_CSPI_ST_TRANS_END_INT_ENA_S  6
/* SPI_MEM_CSPI_ST_TRANS_END : R/W/SS ;bitpos:[5] ;default: 1'b0 ; */
/*description: The bit is used to indicate the  spi0_slv_st controlled transmitting is done..*/
#define SPI_MEM_CSPI_ST_TRANS_END    (BIT(5))
#define SPI_MEM_CSPI_ST_TRANS_END_M  (BIT(5))
#define SPI_MEM_CSPI_ST_TRANS_END_V  0x1
#define SPI_MEM_CSPI_ST_TRANS_END_S  5
/* SPI_MEM_TRANS_END_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The bit is used to enable the interrupt of  spi0_mst_st controlled transmitting
is done..*/
#define SPI_MEM_TRANS_END_INT_ENA    (BIT(4))
#define SPI_MEM_TRANS_END_INT_ENA_M  (BIT(4))
#define SPI_MEM_TRANS_END_INT_ENA_V  0x1
#define SPI_MEM_TRANS_END_INT_ENA_S  4
/* SPI_MEM_TRANS_END : R/W/SS ;bitpos:[3] ;default: 1'b0 ; */
/*description: The bit is used to indicate the  spi0_mst_st controlled transmitting is done..*/
#define SPI_MEM_TRANS_END    (BIT(3))
#define SPI_MEM_TRANS_END_M  (BIT(3))
#define SPI_MEM_TRANS_END_V  0x1
#define SPI_MEM_TRANS_END_S  3
/* SPI_MEM_CS1_DIS : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: SPI_CS1 pin enable, 1: disable SPI_CS1, 0: SPI_CS1 pin is active to select SPI d
evice, such as flash, external RAM and so on..*/
#define SPI_MEM_CS1_DIS    (BIT(1))
#define SPI_MEM_CS1_DIS_M  (BIT(1))
#define SPI_MEM_CS1_DIS_V  0x1
#define SPI_MEM_CS1_DIS_S  1
/* SPI_MEM_CS0_DIS : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: SPI_CS0 pin enable, 1: disable SPI_CS0, 0: SPI_CS0 pin is active to select SPI d
evice, such as flash, external RAM and so on..*/
#define SPI_MEM_CS0_DIS    (BIT(0))
#define SPI_MEM_CS0_DIS_M  (BIT(0))
#define SPI_MEM_CS0_DIS_V  0x1
#define SPI_MEM_CS0_DIS_S  0

#define SPI_MEM_TX_CRC_REG(i)          (REG_SPI_MEM_BASE(i) + 0x38)
/* SPI_MEM_TX_CRC_DATA : RO ;bitpos:[31:0] ;default: 32'hffffffff ; */
/*description: For SPI1, the value of crc32..*/
#define SPI_MEM_TX_CRC_DATA    0xFFFFFFFF
#define SPI_MEM_TX_CRC_DATA_M  ((SPI_MEM_TX_CRC_DATA_V)<<(SPI_MEM_TX_CRC_DATA_S))
#define SPI_MEM_TX_CRC_DATA_V  0xFFFFFFFF
#define SPI_MEM_TX_CRC_DATA_S  0

#define SPI_MEM_CACHE_FCTRL_REG(i)          (REG_SPI_MEM_BASE(i) + 0x3C)
/* SPI_MEM_FADDR_QUAD : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: For SPI0 flash, address phase apply 4 signals. 1: enable 0: disable.  The bit is
 the same with spi_mem_fread_qio..*/
#define SPI_MEM_FADDR_QUAD    (BIT(8))
#define SPI_MEM_FADDR_QUAD_M  (BIT(8))
#define SPI_MEM_FADDR_QUAD_V  0x1
#define SPI_MEM_FADDR_QUAD_S  8
/* SPI_MEM_FDOUT_QUAD : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: For SPI0 flash, dout phase apply 4 signals. 1: enable 0: disable.  The bit is th
e same with spi_mem_fread_qio..*/
#define SPI_MEM_FDOUT_QUAD    (BIT(7))
#define SPI_MEM_FDOUT_QUAD_M  (BIT(7))
#define SPI_MEM_FDOUT_QUAD_V  0x1
#define SPI_MEM_FDOUT_QUAD_S  7
/* SPI_MEM_FDIN_QUAD : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: For SPI0 flash, din phase apply 4 signals. 1: enable 0: disable.  The bit is the
 same with spi_mem_fread_qio..*/
#define SPI_MEM_FDIN_QUAD    (BIT(6))
#define SPI_MEM_FDIN_QUAD_M  (BIT(6))
#define SPI_MEM_FDIN_QUAD_V  0x1
#define SPI_MEM_FDIN_QUAD_S  6
/* SPI_MEM_FADDR_DUAL : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: For SPI0 flash, address phase apply 2 signals. 1: enable 0: disable.  The bit is
 the same with spi_mem_fread_dio..*/
#define SPI_MEM_FADDR_DUAL    (BIT(5))
#define SPI_MEM_FADDR_DUAL_M  (BIT(5))
#define SPI_MEM_FADDR_DUAL_V  0x1
#define SPI_MEM_FADDR_DUAL_S  5
/* SPI_MEM_FDOUT_DUAL : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: For SPI0 flash, dout phase apply 2 signals. 1: enable 0: disable. The bit is the
 same with spi_mem_fread_dio..*/
#define SPI_MEM_FDOUT_DUAL    (BIT(4))
#define SPI_MEM_FDOUT_DUAL_M  (BIT(4))
#define SPI_MEM_FDOUT_DUAL_V  0x1
#define SPI_MEM_FDOUT_DUAL_S  4
/* SPI_MEM_FDIN_DUAL : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: For SPI0 flash, din phase apply 2 signals. 1: enable 0: disable. The bit is the
same with spi_mem_fread_dio..*/
#define SPI_MEM_FDIN_DUAL    (BIT(3))
#define SPI_MEM_FDIN_DUAL_M  (BIT(3))
#define SPI_MEM_FDIN_DUAL_V  0x1
#define SPI_MEM_FDIN_DUAL_S  3
/* SPI_MEM_CACHE_FLASH_USR_CMD : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: For SPI0,  cache  read flash for user define command, 1: enable, 0:disable..*/
#define SPI_MEM_CACHE_FLASH_USR_CMD    (BIT(2))
#define SPI_MEM_CACHE_FLASH_USR_CMD_M  (BIT(2))
#define SPI_MEM_CACHE_FLASH_USR_CMD_V  0x1
#define SPI_MEM_CACHE_FLASH_USR_CMD_S  2
/* SPI_MEM_CACHE_USR_ADDR_4BYTE : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: For SPI0,  cache  read flash with 4 bytes address, 1: enable, 0:disable..*/
#define SPI_MEM_CACHE_USR_ADDR_4BYTE    (BIT(1))
#define SPI_MEM_CACHE_USR_ADDR_4BYTE_M  (BIT(1))
#define SPI_MEM_CACHE_USR_ADDR_4BYTE_V  0x1
#define SPI_MEM_CACHE_USR_ADDR_4BYTE_S  1
/* SPI_MEM_CACHE_REQ_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: For SPI0, Cache access enable, 1: enable, 0:disable..*/
#define SPI_MEM_CACHE_REQ_EN    (BIT(0))
#define SPI_MEM_CACHE_REQ_EN_M  (BIT(0))
#define SPI_MEM_CACHE_REQ_EN_V  0x1
#define SPI_MEM_CACHE_REQ_EN_S  0

#define SPI_MEM_FSM_REG(i)          (REG_SPI_MEM_BASE(i) + 0x54)
/* SPI_MEM_CSPI_LOCK_DELAY_TIME : R/W ;bitpos:[11:7] ;default: 5'd4 ; */
/*description: The lock delay time of SPI0/1 arbiter by spi0_slv_st, after PER is sent by SPI1..*/
#define SPI_MEM_CSPI_LOCK_DELAY_TIME    0x0000001F
#define SPI_MEM_CSPI_LOCK_DELAY_TIME_M  ((SPI_MEM_CSPI_LOCK_DELAY_TIME_V)<<(SPI_MEM_CSPI_LOCK_DELAY_TIME_S))
#define SPI_MEM_CSPI_LOCK_DELAY_TIME_V  0x1F
#define SPI_MEM_CSPI_LOCK_DELAY_TIME_S  7
/* SPI_MEM_EM_ST : RO ;bitpos:[6:4] ;default: 3'h0 ; */
/*description: The current status of SPI0 master FSM: spi0_mst_st. 0: idle state, 1:EM_CACHE_GR
ANT , 2: program/erase suspend state, 3: SPI0 read data state, 4: wait cache/EDM
A sent data is stored in SPI0 TX FIFO, 5: SPI0 write data state..*/
#define SPI_MEM_EM_ST    0x00000007
#define SPI_MEM_EM_ST_M  ((SPI_MEM_EM_ST_V)<<(SPI_MEM_EM_ST_S))
#define SPI_MEM_EM_ST_V  0x7
#define SPI_MEM_EM_ST_S  4
/* SPI_MEM_CSPI_ST : RO ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: The current status of SPI0 slave FSM: spi0_slv_st. 0: idle state, 1: preparation
 state, 2: send command state, 3: send address state, 4: wait state, 5: read dat
a state, 6:write data state, 7: done state, 8: read data end state..*/
#define SPI_MEM_CSPI_ST    0x0000000F
#define SPI_MEM_CSPI_ST_M  ((SPI_MEM_CSPI_ST_V)<<(SPI_MEM_CSPI_ST_S))
#define SPI_MEM_CSPI_ST_V  0xF
#define SPI_MEM_CSPI_ST_S  0

#define SPI_MEM_W0_REG(i)          (REG_SPI_MEM_BASE(i) + 0x58)
/* SPI_MEM_BUF0 : R/W/SS ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer.*/
#define SPI_MEM_BUF0    0xFFFFFFFF
#define SPI_MEM_BUF0_M  ((SPI_MEM_BUF0_V)<<(SPI_MEM_BUF0_S))
#define SPI_MEM_BUF0_V  0xFFFFFFFF
#define SPI_MEM_BUF0_S  0

#define SPI_MEM_W1_REG(i)          (REG_SPI_MEM_BASE(i) + 0x5C)
/* SPI_MEM_BUF1 : R/W/SS ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer.*/
#define SPI_MEM_BUF1    0xFFFFFFFF
#define SPI_MEM_BUF1_M  ((SPI_MEM_BUF1_V)<<(SPI_MEM_BUF1_S))
#define SPI_MEM_BUF1_V  0xFFFFFFFF
#define SPI_MEM_BUF1_S  0

#define SPI_MEM_W2_REG(i)          (REG_SPI_MEM_BASE(i) + 0x60)
/* SPI_MEM_BUF2 : R/W/SS ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer.*/
#define SPI_MEM_BUF2    0xFFFFFFFF
#define SPI_MEM_BUF2_M  ((SPI_MEM_BUF2_V)<<(SPI_MEM_BUF2_S))
#define SPI_MEM_BUF2_V  0xFFFFFFFF
#define SPI_MEM_BUF2_S  0

#define SPI_MEM_W3_REG(i)          (REG_SPI_MEM_BASE(i) + 0x64)
/* SPI_MEM_BUF3 : R/W/SS ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer.*/
#define SPI_MEM_BUF3    0xFFFFFFFF
#define SPI_MEM_BUF3_M  ((SPI_MEM_BUF3_V)<<(SPI_MEM_BUF3_S))
#define SPI_MEM_BUF3_V  0xFFFFFFFF
#define SPI_MEM_BUF3_S  0

#define SPI_MEM_W4_REG(i)          (REG_SPI_MEM_BASE(i) + 0x68)
/* SPI_MEM_BUF4 : R/W/SS ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer.*/
#define SPI_MEM_BUF4    0xFFFFFFFF
#define SPI_MEM_BUF4_M  ((SPI_MEM_BUF4_V)<<(SPI_MEM_BUF4_S))
#define SPI_MEM_BUF4_V  0xFFFFFFFF
#define SPI_MEM_BUF4_S  0

#define SPI_MEM_W5_REG(i)          (REG_SPI_MEM_BASE(i) + 0x6C)
/* SPI_MEM_BUF5 : R/W/SS ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer.*/
#define SPI_MEM_BUF5    0xFFFFFFFF
#define SPI_MEM_BUF5_M  ((SPI_MEM_BUF5_V)<<(SPI_MEM_BUF5_S))
#define SPI_MEM_BUF5_V  0xFFFFFFFF
#define SPI_MEM_BUF5_S  0

#define SPI_MEM_W6_REG(i)          (REG_SPI_MEM_BASE(i) + 0x70)
/* SPI_MEM_BUF6 : R/W/SS ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer.*/
#define SPI_MEM_BUF6    0xFFFFFFFF
#define SPI_MEM_BUF6_M  ((SPI_MEM_BUF6_V)<<(SPI_MEM_BUF6_S))
#define SPI_MEM_BUF6_V  0xFFFFFFFF
#define SPI_MEM_BUF6_S  0

#define SPI_MEM_W7_REG(i)          (REG_SPI_MEM_BASE(i) + 0x74)
/* SPI_MEM_BUF7 : R/W/SS ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer.*/
#define SPI_MEM_BUF7    0xFFFFFFFF
#define SPI_MEM_BUF7_M  ((SPI_MEM_BUF7_V)<<(SPI_MEM_BUF7_S))
#define SPI_MEM_BUF7_V  0xFFFFFFFF
#define SPI_MEM_BUF7_S  0

#define SPI_MEM_W8_REG(i)          (REG_SPI_MEM_BASE(i) + 0x78)
/* SPI_MEM_BUF8 : R/W/SS ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer.*/
#define SPI_MEM_BUF8    0xFFFFFFFF
#define SPI_MEM_BUF8_M  ((SPI_MEM_BUF8_V)<<(SPI_MEM_BUF8_S))
#define SPI_MEM_BUF8_V  0xFFFFFFFF
#define SPI_MEM_BUF8_S  0

#define SPI_MEM_W9_REG(i)          (REG_SPI_MEM_BASE(i) + 0x7C)
/* SPI_MEM_BUF9 : R/W/SS ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer.*/
#define SPI_MEM_BUF9    0xFFFFFFFF
#define SPI_MEM_BUF9_M  ((SPI_MEM_BUF9_V)<<(SPI_MEM_BUF9_S))
#define SPI_MEM_BUF9_V  0xFFFFFFFF
#define SPI_MEM_BUF9_S  0

#define SPI_MEM_W10_REG(i)          (REG_SPI_MEM_BASE(i) + 0x80)
/* SPI_MEM_BUF10 : R/W/SS ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer.*/
#define SPI_MEM_BUF10    0xFFFFFFFF
#define SPI_MEM_BUF10_M  ((SPI_MEM_BUF10_V)<<(SPI_MEM_BUF10_S))
#define SPI_MEM_BUF10_V  0xFFFFFFFF
#define SPI_MEM_BUF10_S  0

#define SPI_MEM_W11_REG(i)          (REG_SPI_MEM_BASE(i) + 0x84)
/* SPI_MEM_BUF11 : R/W/SS ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer.*/
#define SPI_MEM_BUF11    0xFFFFFFFF
#define SPI_MEM_BUF11_M  ((SPI_MEM_BUF11_V)<<(SPI_MEM_BUF11_S))
#define SPI_MEM_BUF11_V  0xFFFFFFFF
#define SPI_MEM_BUF11_S  0

#define SPI_MEM_W12_REG(i)          (REG_SPI_MEM_BASE(i) + 0x88)
/* SPI_MEM_BUF12 : R/W/SS ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer.*/
#define SPI_MEM_BUF12    0xFFFFFFFF
#define SPI_MEM_BUF12_M  ((SPI_MEM_BUF12_V)<<(SPI_MEM_BUF12_S))
#define SPI_MEM_BUF12_V  0xFFFFFFFF
#define SPI_MEM_BUF12_S  0

#define SPI_MEM_W13_REG(i)          (REG_SPI_MEM_BASE(i) + 0x8C)
/* SPI_MEM_BUF13 : R/W/SS ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer.*/
#define SPI_MEM_BUF13    0xFFFFFFFF
#define SPI_MEM_BUF13_M  ((SPI_MEM_BUF13_V)<<(SPI_MEM_BUF13_S))
#define SPI_MEM_BUF13_V  0xFFFFFFFF
#define SPI_MEM_BUF13_S  0

#define SPI_MEM_W14_REG(i)          (REG_SPI_MEM_BASE(i) + 0x90)
/* SPI_MEM_BUF14 : R/W/SS ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer.*/
#define SPI_MEM_BUF14    0xFFFFFFFF
#define SPI_MEM_BUF14_M  ((SPI_MEM_BUF14_V)<<(SPI_MEM_BUF14_S))
#define SPI_MEM_BUF14_V  0xFFFFFFFF
#define SPI_MEM_BUF14_S  0

#define SPI_MEM_W15_REG(i)          (REG_SPI_MEM_BASE(i) + 0x94)
/* SPI_MEM_BUF15 : R/W/SS ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: data buffer.*/
#define SPI_MEM_BUF15    0xFFFFFFFF
#define SPI_MEM_BUF15_M  ((SPI_MEM_BUF15_V)<<(SPI_MEM_BUF15_S))
#define SPI_MEM_BUF15_V  0xFFFFFFFF
#define SPI_MEM_BUF15_S  0

#define SPI_MEM_FLASH_WAITI_CTRL_REG(i)          (REG_SPI_MEM_BASE(i) + 0x98)
/* SPI_MEM_WAITI_DUMMY_CYCLELEN : R/W ;bitpos:[15:10] ;default: 6'h0 ; */
/*description: The dummy cycle length when wait flash idle(RDSR)..*/
#define SPI_MEM_WAITI_DUMMY_CYCLELEN    0x0000003F
#define SPI_MEM_WAITI_DUMMY_CYCLELEN_M  ((SPI_MEM_WAITI_DUMMY_CYCLELEN_V)<<(SPI_MEM_WAITI_DUMMY_CYCLELEN_S))
#define SPI_MEM_WAITI_DUMMY_CYCLELEN_V  0x3F
#define SPI_MEM_WAITI_DUMMY_CYCLELEN_S  10
/* SPI_MEM_WAITI_CMD : R/W ;bitpos:[9:2] ;default: 8'h05 ; */
/*description: The command to wait flash idle(RDSR)..*/
#define SPI_MEM_WAITI_CMD    0x000000FF
#define SPI_MEM_WAITI_CMD_M  ((SPI_MEM_WAITI_CMD_V)<<(SPI_MEM_WAITI_CMD_S))
#define SPI_MEM_WAITI_CMD_V  0xFF
#define SPI_MEM_WAITI_CMD_S  2
/* SPI_MEM_WAITI_DUMMY : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The dummy phase enable when wait flash idle (RDSR).*/
#define SPI_MEM_WAITI_DUMMY    (BIT(1))
#define SPI_MEM_WAITI_DUMMY_M  (BIT(1))
#define SPI_MEM_WAITI_DUMMY_V  0x1
#define SPI_MEM_WAITI_DUMMY_S  1

#define SPI_MEM_FLASH_SUS_CTRL_REG(i)          (REG_SPI_MEM_BASE(i) + 0x9C)
/* SPI_MEM_SUS_TIMEOUT_CNT : R/W ;bitpos:[31:25] ;default: 7'h4 ; */
/*description: When SPI1 checks SUS/SUS1/SUS2 bits fail for SPI_MEM_SUS_TIMEOUT_CNT[6:0] times,
 it will be treated as check pass..*/
#define SPI_MEM_SUS_TIMEOUT_CNT    0x0000007F
#define SPI_MEM_SUS_TIMEOUT_CNT_M  ((SPI_MEM_SUS_TIMEOUT_CNT_V)<<(SPI_MEM_SUS_TIMEOUT_CNT_S))
#define SPI_MEM_SUS_TIMEOUT_CNT_V  0x7F
#define SPI_MEM_SUS_TIMEOUT_CNT_S  25
/* SPI_MEM_PES_END_EN : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: 1: Both WIP and SUS/SUS1/SUS2 bits should be checked to insure the suspend statu
s of flash. 0: Only need to check WIP is 0..*/
#define SPI_MEM_PES_END_EN    (BIT(24))
#define SPI_MEM_PES_END_EN_M  (BIT(24))
#define SPI_MEM_PES_END_EN_V  0x1
#define SPI_MEM_PES_END_EN_S  24
/* SPI_MEM_PER_END_EN : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: 1: Both WIP and SUS/SUS1/SUS2 bits should be checked to insure the resume status
 of flash. 0: Only need to check WIP is 0..*/
#define SPI_MEM_PER_END_EN    (BIT(23))
#define SPI_MEM_PER_END_EN_M  (BIT(23))
#define SPI_MEM_PER_END_EN_V  0x1
#define SPI_MEM_PER_END_EN_S  23
/* SPI_MEM_FMEM_RD_SUS_2B : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: 1: Read two bytes when check flash SUS/SUS1/SUS2 status bit. 0:  Read one byte w
hen check flash SUS/SUS1/SUS2 status bit.*/
#define SPI_MEM_FMEM_RD_SUS_2B    (BIT(22))
#define SPI_MEM_FMEM_RD_SUS_2B_M  (BIT(22))
#define SPI_MEM_FMEM_RD_SUS_2B_V  0x1
#define SPI_MEM_FMEM_RD_SUS_2B_S  22
/* SPI_MEM_PESR_END_MSK : R/W ;bitpos:[21:6] ;default: 16'h80 ; */
/*description: The mask value when check SUS/SUS1/SUS2 status bit. If the read status value is
status_in[15:0](only status_in[7:0] is valid when only one byte of data is read
out, status_in[15:0] is valid when two bytes of data are read out), SUS/SUS1/SUS
2 = status_in[15:0]^ SPI_MEM_PESR_END_MSK[15:0]..*/
#define SPI_MEM_PESR_END_MSK    0x0000FFFF
#define SPI_MEM_PESR_END_MSK_M  ((SPI_MEM_PESR_END_MSK_V)<<(SPI_MEM_PESR_END_MSK_S))
#define SPI_MEM_PESR_END_MSK_V  0xFFFF
#define SPI_MEM_PESR_END_MSK_S  6
/* SPI_MEM_FLASH_PES_EN : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to enable Auto-suspending function..*/
#define SPI_MEM_FLASH_PES_EN    (BIT(5))
#define SPI_MEM_FLASH_PES_EN_M  (BIT(5))
#define SPI_MEM_FLASH_PES_EN_V  0x1
#define SPI_MEM_FLASH_PES_EN_S  5
/* SPI_MEM_PES_PER_EN : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to enable PES end triggers PER transfer option. If this bit is 0, a
pplication should send PER after PES is done..*/
#define SPI_MEM_PES_PER_EN    (BIT(4))
#define SPI_MEM_PES_PER_EN_M  (BIT(4))
#define SPI_MEM_PES_PER_EN_V  0x1
#define SPI_MEM_PES_PER_EN_S  4
/* SPI_MEM_FLASH_PES_WAIT_EN : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: 1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4 or *128) SPI_CLK cycles after
program erase suspend command is sent. 0: SPI1 does not wait after program erase
 suspend command is sent..*/
#define SPI_MEM_FLASH_PES_WAIT_EN    (BIT(3))
#define SPI_MEM_FLASH_PES_WAIT_EN_M  (BIT(3))
#define SPI_MEM_FLASH_PES_WAIT_EN_V  0x1
#define SPI_MEM_FLASH_PES_WAIT_EN_S  3
/* SPI_MEM_FLASH_PER_WAIT_EN : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: 1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4 or *128) SPI_CLK cycles after
program erase resume command is sent. 0: SPI1 does not wait after program erase
resume command is sent..*/
#define SPI_MEM_FLASH_PER_WAIT_EN    (BIT(2))
#define SPI_MEM_FLASH_PER_WAIT_EN_M  (BIT(2))
#define SPI_MEM_FLASH_PER_WAIT_EN_V  0x1
#define SPI_MEM_FLASH_PER_WAIT_EN_S  2
/* SPI_MEM_FLASH_PES : R/W/SC ;bitpos:[1] ;default: 1'b0 ; */
/*description: program erase suspend bit, program erase suspend operation will be triggered whe
n the bit is set. The bit will be cleared once the operation done.1: enable 0: d
isable..*/
#define SPI_MEM_FLASH_PES    (BIT(1))
#define SPI_MEM_FLASH_PES_M  (BIT(1))
#define SPI_MEM_FLASH_PES_V  0x1
#define SPI_MEM_FLASH_PES_S  1
/* SPI_MEM_FLASH_PER : R/W/SC ;bitpos:[0] ;default: 1'b0 ; */
/*description: program erase resume bit, program erase suspend operation will be triggered when
 the bit is set. The bit will be cleared once the operation done.1: enable 0: di
sable..*/
#define SPI_MEM_FLASH_PER    (BIT(0))
#define SPI_MEM_FLASH_PER_M  (BIT(0))
#define SPI_MEM_FLASH_PER_V  0x1
#define SPI_MEM_FLASH_PER_S  0

#define SPI_MEM_FLASH_SUS_CMD_REG(i)          (REG_SPI_MEM_BASE(i) + 0xA0)
/* SPI_MEM_WAIT_PESR_COMMAND : R/W ;bitpos:[31:16] ;default: 16'h05 ; */
/*description: Flash SUS/SUS1/SUS2 status bit read command. The command should be sent when SUS
/SUS1/SUS2 bit should be checked to insure the suspend or resume status of flash
..*/
#define SPI_MEM_WAIT_PESR_COMMAND    0x0000FFFF
#define SPI_MEM_WAIT_PESR_COMMAND_M  ((SPI_MEM_WAIT_PESR_COMMAND_V)<<(SPI_MEM_WAIT_PESR_COMMAND_S))
#define SPI_MEM_WAIT_PESR_COMMAND_V  0xFFFF
#define SPI_MEM_WAIT_PESR_COMMAND_S  16
/* SPI_MEM_FLASH_PES_COMMAND : R/W ;bitpos:[15:8] ;default: 8'h75 ; */
/*description: Program/Erase suspend command..*/
#define SPI_MEM_FLASH_PES_COMMAND    0x000000FF
#define SPI_MEM_FLASH_PES_COMMAND_M  ((SPI_MEM_FLASH_PES_COMMAND_V)<<(SPI_MEM_FLASH_PES_COMMAND_S))
#define SPI_MEM_FLASH_PES_COMMAND_V  0xFF
#define SPI_MEM_FLASH_PES_COMMAND_S  8
/* SPI_MEM_FLASH_PER_COMMAND : R/W ;bitpos:[7:0] ;default: 8'h7a ; */
/*description: Program/Erase resume command..*/
#define SPI_MEM_FLASH_PER_COMMAND    0x000000FF
#define SPI_MEM_FLASH_PER_COMMAND_M  ((SPI_MEM_FLASH_PER_COMMAND_V)<<(SPI_MEM_FLASH_PER_COMMAND_S))
#define SPI_MEM_FLASH_PER_COMMAND_V  0xFF
#define SPI_MEM_FLASH_PER_COMMAND_S  0

#define SPI_MEM_SUS_STATUS_REG(i)          (REG_SPI_MEM_BASE(i) + 0xA4)
/* SPI_MEM_SPI0_LOCK_EN : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: 1: Enable SPI0 lock SPI0/1 arbiter option. 0: Disable it..*/
#define SPI_MEM_SPI0_LOCK_EN    (BIT(7))
#define SPI_MEM_SPI0_LOCK_EN_M  (BIT(7))
#define SPI_MEM_SPI0_LOCK_EN_V  0x1
#define SPI_MEM_SPI0_LOCK_EN_S  7
/* SPI_MEM_FLASH_PES_DLY_128 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Valid when SPI_MEM_FLASH_PES_WAIT_EN is 1. 1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_
RES[9:0] * 128) SPI_CLK cycles after PES command is sent. 0: SPI1 waits (SPI_MEM
_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after PES command is sent..*/
#define SPI_MEM_FLASH_PES_DLY_128    (BIT(6))
#define SPI_MEM_FLASH_PES_DLY_128_M  (BIT(6))
#define SPI_MEM_FLASH_PES_DLY_128_V  0x1
#define SPI_MEM_FLASH_PES_DLY_128_S  6
/* SPI_MEM_FLASH_PER_DLY_128 : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: Valid when SPI_MEM_FLASH_PER_WAIT_EN is 1. 1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_
RES[9:0] * 128) SPI_CLK cycles after PER command is sent. 0: SPI1 waits (SPI_MEM
_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after PER command is sent..*/
#define SPI_MEM_FLASH_PER_DLY_128    (BIT(5))
#define SPI_MEM_FLASH_PER_DLY_128_M  (BIT(5))
#define SPI_MEM_FLASH_PER_DLY_128_V  0x1
#define SPI_MEM_FLASH_PER_DLY_128_S  5
/* SPI_MEM_FLASH_DP_DLY_128 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: 1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 128) SPI_CLK cycles after DP com
mand is sent. 0: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles
after DP command is sent..*/
#define SPI_MEM_FLASH_DP_DLY_128    (BIT(4))
#define SPI_MEM_FLASH_DP_DLY_128_M  (BIT(4))
#define SPI_MEM_FLASH_DP_DLY_128_V  0x1
#define SPI_MEM_FLASH_DP_DLY_128_S  4
/* SPI_MEM_FLASH_RES_DLY_128 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: 1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 128) SPI_CLK cycles after RES co
mmand is sent. 0: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles
 after RES command is sent..*/
#define SPI_MEM_FLASH_RES_DLY_128    (BIT(3))
#define SPI_MEM_FLASH_RES_DLY_128_M  (BIT(3))
#define SPI_MEM_FLASH_RES_DLY_128_V  0x1
#define SPI_MEM_FLASH_RES_DLY_128_S  3
/* SPI_MEM_FLASH_HPM_DLY_128 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: 1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 128) SPI_CLK cycles after HPM co
mmand is sent. 0: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles
 after HPM command is sent..*/
#define SPI_MEM_FLASH_HPM_DLY_128    (BIT(2))
#define SPI_MEM_FLASH_HPM_DLY_128_M  (BIT(2))
#define SPI_MEM_FLASH_HPM_DLY_128_V  0x1
#define SPI_MEM_FLASH_HPM_DLY_128_S  2
/* SPI_MEM_WAIT_PESR_CMD_2B : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: 1: SPI1 sends out SPI_MEM_WAIT_PESR_COMMAND[15:0] to check SUS/SUS1/SUS2 bit. 0:
 SPI1 sends out SPI_MEM_WAIT_PESR_COMMAND[7:0] to check SUS/SUS1/SUS2 bit..*/
#define SPI_MEM_WAIT_PESR_CMD_2B    (BIT(1))
#define SPI_MEM_WAIT_PESR_CMD_2B_M  (BIT(1))
#define SPI_MEM_WAIT_PESR_CMD_2B_V  0x1
#define SPI_MEM_WAIT_PESR_CMD_2B_S  1
/* SPI_MEM_FLASH_SUS : R/W/SS/SC ;bitpos:[0] ;default: 1'h0 ; */
/*description: The status of flash suspend, only used in SPI1..*/
#define SPI_MEM_FLASH_SUS    (BIT(0))
#define SPI_MEM_FLASH_SUS_M  (BIT(0))
#define SPI_MEM_FLASH_SUS_V  0x1
#define SPI_MEM_FLASH_SUS_S  0

#define SPI_MEM_TIMING_CALI_REG(i)          (REG_SPI_MEM_BASE(i) + 0xA8)
/* SPI_MEM_EXTRA_DUMMY_CYCLELEN : HRO ;bitpos:[4:2] ;default: 3'd0 ; */
/*description: add extra dummy spi clock cycle length for spi clock calibration..*/
#define SPI_MEM_EXTRA_DUMMY_CYCLELEN    0x00000007
#define SPI_MEM_EXTRA_DUMMY_CYCLELEN_M  ((SPI_MEM_EXTRA_DUMMY_CYCLELEN_V)<<(SPI_MEM_EXTRA_DUMMY_CYCLELEN_S))
#define SPI_MEM_EXTRA_DUMMY_CYCLELEN_V  0x7
#define SPI_MEM_EXTRA_DUMMY_CYCLELEN_S  2
/* SPI_MEM_TIMING_CALI : HRO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to enable timing auto-calibration for all reading operations..*/
#define SPI_MEM_TIMING_CALI    (BIT(1))
#define SPI_MEM_TIMING_CALI_M  (BIT(1))
#define SPI_MEM_TIMING_CALI_V  0x1
#define SPI_MEM_TIMING_CALI_S  1
/* SPI_MEM_TIMING_CLK_ENA : HRO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The bit is used to enable timing adjust clock for all reading operations..*/
#define SPI_MEM_TIMING_CLK_ENA    (BIT(0))
#define SPI_MEM_TIMING_CLK_ENA_M  (BIT(0))
#define SPI_MEM_TIMING_CLK_ENA_V  0x1
#define SPI_MEM_TIMING_CLK_ENA_S  0

#define SPI_MEM_DIN_MODE_REG(i)          (REG_SPI_MEM_BASE(i) + 0xAC)
/* SPI_MEM_DIN3_MODE : HRO ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: in
put with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input w
ith the spi_clk high edge,  6: input with the spi_clk low edge.*/
#define SPI_MEM_DIN3_MODE    0x00000003
#define SPI_MEM_DIN3_MODE_M  ((SPI_MEM_DIN3_MODE_V)<<(SPI_MEM_DIN3_MODE_S))
#define SPI_MEM_DIN3_MODE_V  0x3
#define SPI_MEM_DIN3_MODE_S  6
/* SPI_MEM_DIN2_MODE : HRO ;bitpos:[5:4] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: in
put with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input w
ith the spi_clk high edge,  6: input with the spi_clk low edge.*/
#define SPI_MEM_DIN2_MODE    0x00000003
#define SPI_MEM_DIN2_MODE_M  ((SPI_MEM_DIN2_MODE_V)<<(SPI_MEM_DIN2_MODE_S))
#define SPI_MEM_DIN2_MODE_V  0x3
#define SPI_MEM_DIN2_MODE_S  4
/* SPI_MEM_DIN1_MODE : HRO ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: in
put with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input w
ith the spi_clk high edge,  6: input with the spi_clk low edge.*/
#define SPI_MEM_DIN1_MODE    0x00000003
#define SPI_MEM_DIN1_MODE_M  ((SPI_MEM_DIN1_MODE_V)<<(SPI_MEM_DIN1_MODE_S))
#define SPI_MEM_DIN1_MODE_V  0x3
#define SPI_MEM_DIN1_MODE_S  2
/* SPI_MEM_DIN0_MODE : HRO ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: in
put with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input w
ith the spi_clk high edge,  6: input with the spi_clk low edge.*/
#define SPI_MEM_DIN0_MODE    0x00000003
#define SPI_MEM_DIN0_MODE_M  ((SPI_MEM_DIN0_MODE_V)<<(SPI_MEM_DIN0_MODE_S))
#define SPI_MEM_DIN0_MODE_V  0x3
#define SPI_MEM_DIN0_MODE_S  0

#define SPI_MEM_DIN_NUM_REG(i)          (REG_SPI_MEM_BASE(i) + 0xB0)
/* SPI_MEM_DIN3_NUM : HRO ;bitpos:[3] ;default: 1'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_DIN3_NUM    (BIT(3))
#define SPI_MEM_DIN3_NUM_M  (BIT(3))
#define SPI_MEM_DIN3_NUM_V  0x1
#define SPI_MEM_DIN3_NUM_S  3
/* SPI_MEM_DIN2_NUM : HRO ;bitpos:[2] ;default: 1'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_DIN2_NUM    (BIT(2))
#define SPI_MEM_DIN2_NUM_M  (BIT(2))
#define SPI_MEM_DIN2_NUM_V  0x1
#define SPI_MEM_DIN2_NUM_S  2
/* SPI_MEM_DIN1_NUM : HRO ;bitpos:[1] ;default: 1'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_DIN1_NUM    (BIT(1))
#define SPI_MEM_DIN1_NUM_M  (BIT(1))
#define SPI_MEM_DIN1_NUM_V  0x1
#define SPI_MEM_DIN1_NUM_S  1
/* SPI_MEM_DIN0_NUM : HRO ;bitpos:[0] ;default: 1'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_DIN0_NUM    (BIT(0))
#define SPI_MEM_DIN0_NUM_M  (BIT(0))
#define SPI_MEM_DIN0_NUM_V  0x1
#define SPI_MEM_DIN0_NUM_S  0

#define SPI_MEM_DOUT_MODE_REG(i)          (REG_SPI_MEM_BASE(i) + 0xB4)
/* SPI_MEM_DOUT3_MODE : HRO ;bitpos:[3] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the posedge of clk_160,4 output with the negedge of clk_160,5: outp
ut with the spi_clk high edge ,6: output with the spi_clk low edge.*/
#define SPI_MEM_DOUT3_MODE    (BIT(3))
#define SPI_MEM_DOUT3_MODE_M  (BIT(3))
#define SPI_MEM_DOUT3_MODE_V  0x1
#define SPI_MEM_DOUT3_MODE_S  3
/* SPI_MEM_DOUT2_MODE : HRO ;bitpos:[2] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the posedge of clk_160,4 output with the negedge of clk_160,5: outp
ut with the spi_clk high edge ,6: output with the spi_clk low edge.*/
#define SPI_MEM_DOUT2_MODE    (BIT(2))
#define SPI_MEM_DOUT2_MODE_M  (BIT(2))
#define SPI_MEM_DOUT2_MODE_V  0x1
#define SPI_MEM_DOUT2_MODE_S  2
/* SPI_MEM_DOUT1_MODE : HRO ;bitpos:[1] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the posedge of clk_160,4 output with the negedge of clk_160,5: outp
ut with the spi_clk high edge ,6: output with the spi_clk low edge.*/
#define SPI_MEM_DOUT1_MODE    (BIT(1))
#define SPI_MEM_DOUT1_MODE_M  (BIT(1))
#define SPI_MEM_DOUT1_MODE_V  0x1
#define SPI_MEM_DOUT1_MODE_S  1
/* SPI_MEM_DOUT0_MODE : HRO ;bitpos:[0] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the posedge of clk_160,4 output with the negedge of clk_160,5: outp
ut with the spi_clk high edge ,6: output with the spi_clk low edge.*/
#define SPI_MEM_DOUT0_MODE    (BIT(0))
#define SPI_MEM_DOUT0_MODE_M  (BIT(0))
#define SPI_MEM_DOUT0_MODE_V  0x1
#define SPI_MEM_DOUT0_MODE_S  0

#define SPI_MEM_INT_ENA_REG(i)          (REG_SPI_MEM_BASE(i) + 0xC0)
/* SPI_MEM_BROWN_OUT_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The enable bit for SPI_MEM_BROWN_OUT_INT interrupt..*/
#define SPI_MEM_BROWN_OUT_INT_ENA    (BIT(5))
#define SPI_MEM_BROWN_OUT_INT_ENA_M  (BIT(5))
#define SPI_MEM_BROWN_OUT_INT_ENA_V  0x1
#define SPI_MEM_BROWN_OUT_INT_ENA_S  5
/* SPI_MEM_MST_ST_END_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The enable bit for SPI_MEM_MST_ST_END_INT interrupt..*/
#define SPI_MEM_MST_ST_END_INT_ENA    (BIT(4))
#define SPI_MEM_MST_ST_END_INT_ENA_M  (BIT(4))
#define SPI_MEM_MST_ST_END_INT_ENA_V  0x1
#define SPI_MEM_MST_ST_END_INT_ENA_S  4
/* SPI_MEM_SLV_ST_END_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The enable bit for SPI_MEM_SLV_ST_END_INT interrupt..*/
#define SPI_MEM_SLV_ST_END_INT_ENA    (BIT(3))
#define SPI_MEM_SLV_ST_END_INT_ENA_M  (BIT(3))
#define SPI_MEM_SLV_ST_END_INT_ENA_V  0x1
#define SPI_MEM_SLV_ST_END_INT_ENA_S  3
/* SPI_MEM_WPE_END_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The enable bit for SPI_MEM_WPE_END_INT interrupt..*/
#define SPI_MEM_WPE_END_INT_ENA    (BIT(2))
#define SPI_MEM_WPE_END_INT_ENA_M  (BIT(2))
#define SPI_MEM_WPE_END_INT_ENA_V  0x1
#define SPI_MEM_WPE_END_INT_ENA_S  2
/* SPI_MEM_PES_END_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The enable bit for SPI_MEM_PES_END_INT interrupt..*/
#define SPI_MEM_PES_END_INT_ENA    (BIT(1))
#define SPI_MEM_PES_END_INT_ENA_M  (BIT(1))
#define SPI_MEM_PES_END_INT_ENA_V  0x1
#define SPI_MEM_PES_END_INT_ENA_S  1
/* SPI_MEM_PER_END_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The enable bit for SPI_MEM_PER_END_INT interrupt..*/
#define SPI_MEM_PER_END_INT_ENA    (BIT(0))
#define SPI_MEM_PER_END_INT_ENA_M  (BIT(0))
#define SPI_MEM_PER_END_INT_ENA_V  0x1
#define SPI_MEM_PER_END_INT_ENA_S  0

#define SPI_MEM_INT_CLR_REG(i)          (REG_SPI_MEM_BASE(i) + 0xC4)
/* SPI_MEM_BROWN_OUT_INT_CLR : WT ;bitpos:[5] ;default: 1'b0 ; */
/*description: The status bit for SPI_MEM_BROWN_OUT_INT interrupt..*/
#define SPI_MEM_BROWN_OUT_INT_CLR    (BIT(5))
#define SPI_MEM_BROWN_OUT_INT_CLR_M  (BIT(5))
#define SPI_MEM_BROWN_OUT_INT_CLR_V  0x1
#define SPI_MEM_BROWN_OUT_INT_CLR_S  5
/* SPI_MEM_MST_ST_END_INT_CLR : WT ;bitpos:[4] ;default: 1'b0 ; */
/*description: The clear bit for SPI_MEM_MST_ST_END_INT interrupt..*/
#define SPI_MEM_MST_ST_END_INT_CLR    (BIT(4))
#define SPI_MEM_MST_ST_END_INT_CLR_M  (BIT(4))
#define SPI_MEM_MST_ST_END_INT_CLR_V  0x1
#define SPI_MEM_MST_ST_END_INT_CLR_S  4
/* SPI_MEM_SLV_ST_END_INT_CLR : WT ;bitpos:[3] ;default: 1'b0 ; */
/*description: The clear bit for SPI_MEM_SLV_ST_END_INT interrupt..*/
#define SPI_MEM_SLV_ST_END_INT_CLR    (BIT(3))
#define SPI_MEM_SLV_ST_END_INT_CLR_M  (BIT(3))
#define SPI_MEM_SLV_ST_END_INT_CLR_V  0x1
#define SPI_MEM_SLV_ST_END_INT_CLR_S  3
/* SPI_MEM_WPE_END_INT_CLR : WT ;bitpos:[2] ;default: 1'b0 ; */
/*description: The clear bit for SPI_MEM_WPE_END_INT interrupt..*/
#define SPI_MEM_WPE_END_INT_CLR    (BIT(2))
#define SPI_MEM_WPE_END_INT_CLR_M  (BIT(2))
#define SPI_MEM_WPE_END_INT_CLR_V  0x1
#define SPI_MEM_WPE_END_INT_CLR_S  2
/* SPI_MEM_PES_END_INT_CLR : WT ;bitpos:[1] ;default: 1'b0 ; */
/*description: The clear bit for SPI_MEM_PES_END_INT interrupt..*/
#define SPI_MEM_PES_END_INT_CLR    (BIT(1))
#define SPI_MEM_PES_END_INT_CLR_M  (BIT(1))
#define SPI_MEM_PES_END_INT_CLR_V  0x1
#define SPI_MEM_PES_END_INT_CLR_S  1
/* SPI_MEM_PER_END_INT_CLR : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: The clear bit for SPI_MEM_PER_END_INT interrupt..*/
#define SPI_MEM_PER_END_INT_CLR    (BIT(0))
#define SPI_MEM_PER_END_INT_CLR_M  (BIT(0))
#define SPI_MEM_PER_END_INT_CLR_V  0x1
#define SPI_MEM_PER_END_INT_CLR_S  0

#define SPI_MEM_INT_RAW_REG(i)          (REG_SPI_MEM_BASE(i) + 0xC8)
/* SPI_MEM_BROWN_OUT_INT_RAW : R/WTC/SS ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw bit for SPI_MEM_BROWN_OUT_INT interrupt. 1: Triggered condition is that
chip is loosing power and RTC module sends out brown out close flash request to
SPI1. After SPI1 sends out suspend command to flash, this interrupt is triggered
 and MSPI returns to idle state. 0: Others..*/
#define SPI_MEM_BROWN_OUT_INT_RAW    (BIT(5))
#define SPI_MEM_BROWN_OUT_INT_RAW_M  (BIT(5))
#define SPI_MEM_BROWN_OUT_INT_RAW_V  0x1
#define SPI_MEM_BROWN_OUT_INT_RAW_S  5
/* SPI_MEM_MST_ST_END_INT_RAW : R/WTC/SS ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw bit for SPI_MEM_MST_ST_END_INT interrupt. 1: Triggered when spi1_mst_st
is changed from non idle state to idle state. 0: Others..*/
#define SPI_MEM_MST_ST_END_INT_RAW    (BIT(4))
#define SPI_MEM_MST_ST_END_INT_RAW_M  (BIT(4))
#define SPI_MEM_MST_ST_END_INT_RAW_V  0x1
#define SPI_MEM_MST_ST_END_INT_RAW_S  4
/* SPI_MEM_SLV_ST_END_INT_RAW : R/WTC/SS ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw bit for SPI_MEM_SLV_ST_END_INT interrupt. 1: Triggered when spi1_slv_st
is changed from non idle state to idle state. It means that SPI_CS raises high.
0: Others.*/
#define SPI_MEM_SLV_ST_END_INT_RAW    (BIT(3))
#define SPI_MEM_SLV_ST_END_INT_RAW_M  (BIT(3))
#define SPI_MEM_SLV_ST_END_INT_RAW_V  0x1
#define SPI_MEM_SLV_ST_END_INT_RAW_S  3
/* SPI_MEM_WPE_END_INT_RAW : R/WTC/SS ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw bit for SPI_MEM_WPE_END_INT interrupt. 1: Triggered when WRSR/PP/SE/BE/C
E is sent and flash is already idle. 0: Others..*/
#define SPI_MEM_WPE_END_INT_RAW    (BIT(2))
#define SPI_MEM_WPE_END_INT_RAW_M  (BIT(2))
#define SPI_MEM_WPE_END_INT_RAW_V  0x1
#define SPI_MEM_WPE_END_INT_RAW_S  2
/* SPI_MEM_PES_END_INT_RAW : R/WTC/SS ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw bit for SPI_MEM_PES_END_INT interrupt.1: Triggered when Auto Suspend com
mand (0x75) is sent and flash is suspended. 0: Others..*/
#define SPI_MEM_PES_END_INT_RAW    (BIT(1))
#define SPI_MEM_PES_END_INT_RAW_M  (BIT(1))
#define SPI_MEM_PES_END_INT_RAW_V  0x1
#define SPI_MEM_PES_END_INT_RAW_S  1
/* SPI_MEM_PER_END_INT_RAW : R/WTC/SS ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw bit for SPI_MEM_PER_END_INT interrupt. 1: Triggered when Auto Resume com
mand (0x7A) is sent and flash is resumed. 0: Others..*/
#define SPI_MEM_PER_END_INT_RAW    (BIT(0))
#define SPI_MEM_PER_END_INT_RAW_M  (BIT(0))
#define SPI_MEM_PER_END_INT_RAW_V  0x1
#define SPI_MEM_PER_END_INT_RAW_S  0

#define SPI_MEM_INT_ST_REG(i)          (REG_SPI_MEM_BASE(i) + 0xCC)
/* SPI_MEM_BROWN_OUT_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The status bit for SPI_MEM_BROWN_OUT_INT interrupt..*/
#define SPI_MEM_BROWN_OUT_INT_ST    (BIT(5))
#define SPI_MEM_BROWN_OUT_INT_ST_M  (BIT(5))
#define SPI_MEM_BROWN_OUT_INT_ST_V  0x1
#define SPI_MEM_BROWN_OUT_INT_ST_S  5
/* SPI_MEM_MST_ST_END_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The status bit for SPI_MEM_MST_ST_END_INT interrupt..*/
#define SPI_MEM_MST_ST_END_INT_ST    (BIT(4))
#define SPI_MEM_MST_ST_END_INT_ST_M  (BIT(4))
#define SPI_MEM_MST_ST_END_INT_ST_V  0x1
#define SPI_MEM_MST_ST_END_INT_ST_S  4
/* SPI_MEM_SLV_ST_END_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The status bit for SPI_MEM_SLV_ST_END_INT interrupt..*/
#define SPI_MEM_SLV_ST_END_INT_ST    (BIT(3))
#define SPI_MEM_SLV_ST_END_INT_ST_M  (BIT(3))
#define SPI_MEM_SLV_ST_END_INT_ST_V  0x1
#define SPI_MEM_SLV_ST_END_INT_ST_S  3
/* SPI_MEM_WPE_END_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The status bit for SPI_MEM_WPE_END_INT interrupt..*/
#define SPI_MEM_WPE_END_INT_ST    (BIT(2))
#define SPI_MEM_WPE_END_INT_ST_M  (BIT(2))
#define SPI_MEM_WPE_END_INT_ST_V  0x1
#define SPI_MEM_WPE_END_INT_ST_S  2
/* SPI_MEM_PES_END_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The status bit for SPI_MEM_PES_END_INT interrupt..*/
#define SPI_MEM_PES_END_INT_ST    (BIT(1))
#define SPI_MEM_PES_END_INT_ST_M  (BIT(1))
#define SPI_MEM_PES_END_INT_ST_V  0x1
#define SPI_MEM_PES_END_INT_ST_S  1
/* SPI_MEM_PER_END_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The status bit for SPI_MEM_PER_END_INT interrupt..*/
#define SPI_MEM_PER_END_INT_ST    (BIT(0))
#define SPI_MEM_PER_END_INT_ST_M  (BIT(0))
#define SPI_MEM_PER_END_INT_ST_V  0x1
#define SPI_MEM_PER_END_INT_ST_S  0

#define SPI_MEM_CLOCK_GATE_REG(i)          (REG_SPI_MEM_BASE(i) + 0xDC)
/* SPI_MEM_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: Register clock gate enable signal. 1: Enable. 0: Disable..*/
#define SPI_MEM_CLK_EN    (BIT(0))
#define SPI_MEM_CLK_EN_M  (BIT(0))
#define SPI_MEM_CLK_EN_V  0x1
#define SPI_MEM_CLK_EN_S  0

#define SPI_MEM_CORE_CLK_SEL_REG(i)          (REG_SPI_MEM_BASE(i) + 0xE0)
/* SPI_MEM_SPI01_CLK_SEL : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: When the digital system clock selects PLL clock and the frequency of PLL clock i
s 480MHz, the value of reg_spi01_clk_sel:  0: SPI0/1 module clock (clk) is 80MHz
. 1: SPI0/1 module clock (clk) is 120MHz.  2: SPI0/1 module clock (clk) 160MHz.
3: Not used. When the digital system clock selects PLL clock and the frequency o
f PLL clock is 320MHz, the value of reg_spi01_clk_sel:  0: SPI0/1 module clock (
clk) is 80MHz. 1: SPI0/1 module clock (clk) is 80MHz.  2: SPI0/1 module clock (c
lk) 160MHz. 3: Not used..*/
#define SPI_MEM_SPI01_CLK_SEL    0x00000003
#define SPI_MEM_SPI01_CLK_SEL_M  ((SPI_MEM_SPI01_CLK_SEL_V)<<(SPI_MEM_SPI01_CLK_SEL_S))
#define SPI_MEM_SPI01_CLK_SEL_V  0x3
#define SPI_MEM_SPI01_CLK_SEL_S  0

#define SPI_MEM_DATE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x3FC)
/* SPI_MEM_DATE : R/W ;bitpos:[27:0] ;default: 28'h2106191 ; */
/*description: SPI register version..*/
#define SPI_MEM_DATE    0x0FFFFFFF
#define SPI_MEM_DATE_M  ((SPI_MEM_DATE_V)<<(SPI_MEM_DATE_S))
#define SPI_MEM_DATE_V  0xFFFFFFF
#define SPI_MEM_DATE_S  0


#ifdef __cplusplus
}
#endif



#endif /*_SOC_SPI_MEM_REG_H_ */
