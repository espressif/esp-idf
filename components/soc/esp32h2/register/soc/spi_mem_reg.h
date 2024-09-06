/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/soc.h"

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
e or high performance mode and obtains the devices ID. The bit will be cleared once
the operation done.1: enable 0: disable..*/
#define SPI_MEM_FLASH_RES    (BIT(20))
#define SPI_MEM_FLASH_RES_M  (BIT(20))
#define SPI_MEM_FLASH_RES_V  0x1
#define SPI_MEM_FLASH_RES_S  20
/* SPI_MEM_FLASH_HPM : R/W/SC ;bitpos:[19] ;default: 1'b0 ; */
/*description: Drive Flash into high performance mode.  The bit will be cleared once the operation
done.1: enable 0: disable..*/
#define SPI_MEM_FLASH_HPM    (BIT(19))
#define SPI_MEM_FLASH_HPM_M  (BIT(19))
#define SPI_MEM_FLASH_HPM_V  0x1
#define SPI_MEM_FLASH_HPM_S  19
/* SPI_MEM_USR : HRO ;bitpos:[18] ;default: 1'b0 ; */
/*description: SPI0 USR_CMD start bit, only used when SPI_MEM_AXI_REQ_EN is cleared.  An operation
will be triggered when the bit is set. The bit will be cleared once the operation
done.1: enable 0: disable..*/
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
/* SPI_MEM_SLV_ST : RO ;bitpos:[7:4] ;default: 4'b0 ; */
/*description: The current status of SPI0 slave FSM: mspi_st. 0: idle state, 1: preparation
state, 2: send command state, 3: send address state, 4: wait state, 5: read data state
, 6:write data state, 7: done state, 8: read data end state..*/
#define SPI_MEM_SLV_ST    0x0000000F
#define SPI_MEM_SLV_ST_M  ((SPI_MEM_SLV_ST_V)<<(SPI_MEM_SLV_ST_S))
#define SPI_MEM_SLV_ST_V  0xF
#define SPI_MEM_SLV_ST_S  4
/* SPI_MEM_MST_ST : RO ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: The current status of SPI0 master FSM: spi0_mst_st. 0: idle state, 1:SPI0_GRANT
, 2: program/erase suspend state, 3: SPI0 read data state, 4: wait cache/EDMA se
nt data is stored in SPI0 TX FIFO, 5: SPI0 write data state..*/
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
/* SPI_MEM_DATA_IE_ALWAYS_ON : R/W ;bitpos:[31] ;default: 1'b1 ; */
/*description: When accesses to flash, 1: the IE signals of pads connected to SPI_IO[7:0] are a
lways 1. 0: Others..*/
#define SPI_MEM_DATA_IE_ALWAYS_ON    (BIT(31))
#define SPI_MEM_DATA_IE_ALWAYS_ON_M  (BIT(31))
#define SPI_MEM_DATA_IE_ALWAYS_ON_V  0x1
#define SPI_MEM_DATA_IE_ALWAYS_ON_S  31
/* SPI_MEM_DQS_IE_ALWAYS_ON : HRO ;bitpos:[30] ;default: 1'b0 ; */
/*description: When accesses to flash, 1: the IE signals of pads connected to SPI_DQS are always
s 1. 0: Others..*/
#define SPI_MEM_DQS_IE_ALWAYS_ON    (BIT(30))
#define SPI_MEM_DQS_IE_ALWAYS_ON_M  (BIT(30))
#define SPI_MEM_DQS_IE_ALWAYS_ON_V  0x1
#define SPI_MEM_DQS_IE_ALWAYS_ON_S  30
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
UT and SPI_MEM_FREAD_DOUT. 1: enable 0: disable..*/
#define SPI_MEM_FASTRD_MODE    (BIT(13))
#define SPI_MEM_FASTRD_MODE_M  (BIT(13))
#define SPI_MEM_FASTRD_MODE_V  0x1
#define SPI_MEM_FASTRD_MODE_S  13
/* SPI_MEM_TX_CRC_EN : HRO ;bitpos:[11] ;default: 1'b0 ; */
/*description: For SPI1,  enable crc32 when writing encrypted data to flash. 1: enable 0:disable
e.*/
#define SPI_MEM_TX_CRC_EN    (BIT(11))
#define SPI_MEM_TX_CRC_EN_M  (BIT(11))
#define SPI_MEM_TX_CRC_EN_V  0x1
#define SPI_MEM_TX_CRC_EN_S  11
/* SPI_MEM_FCS_CRC_EN : HRO ;bitpos:[10] ;default: 1'b0 ; */
/*description: For SPI1,  initialize crc32 module before writing encrypted data to flash. Active
e low..*/
#define SPI_MEM_FCS_CRC_EN    (BIT(10))
#define SPI_MEM_FCS_CRC_EN_M  (BIT(10))
#define SPI_MEM_FCS_CRC_EN_V  0x1
#define SPI_MEM_FCS_CRC_EN_S  10
/* SPI_MEM_FCMD_OCT : HRO ;bitpos:[9] ;default: 1'b0 ; */
/*description: Apply 8 signals during command phase 1:enable 0: disable.*/
#define SPI_MEM_FCMD_OCT    (BIT(9))
#define SPI_MEM_FCMD_OCT_M  (BIT(9))
#define SPI_MEM_FCMD_OCT_V  0x1
#define SPI_MEM_FCMD_OCT_S  9
/* SPI_MEM_FCMD_QUAD : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Apply 4 signals during command phase 1:enable 0: disable.*/
#define SPI_MEM_FCMD_QUAD    (BIT(8))
#define SPI_MEM_FCMD_QUAD_M  (BIT(8))
#define SPI_MEM_FCMD_QUAD_V  0x1
#define SPI_MEM_FCMD_QUAD_S  8
/* SPI_MEM_FADDR_OCT : HRO ;bitpos:[6] ;default: 1'b0 ; */
/*description: Apply 8 signals during address phase 1:enable 0: disable.*/
#define SPI_MEM_FADDR_OCT    (BIT(6))
#define SPI_MEM_FADDR_OCT_M  (BIT(6))
#define SPI_MEM_FADDR_OCT_V  0x1
#define SPI_MEM_FADDR_OCT_S  6
/* SPI_MEM_FDIN_OCT : HRO ;bitpos:[5] ;default: 1'b0 ; */
/*description: Apply 8 signals during read-data phase 1:enable 0: disable.*/
#define SPI_MEM_FDIN_OCT    (BIT(5))
#define SPI_MEM_FDIN_OCT_M  (BIT(5))
#define SPI_MEM_FDIN_OCT_V  0x1
#define SPI_MEM_FDIN_OCT_S  5
/* SPI_MEM_FDOUT_OCT : HRO ;bitpos:[4] ;default: 1'b0 ; */
/*description: Apply 8 signals during write-data phase 1:enable 0: disable.*/
#define SPI_MEM_FDOUT_OCT    (BIT(4))
#define SPI_MEM_FDOUT_OCT_M  (BIT(4))
#define SPI_MEM_FDOUT_OCT_V  0x1
#define SPI_MEM_FDOUT_OCT_S  4
/* SPI_MEM_FDUMMY_WOUT : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: In an MSPI write data transfer when accesses to flash, the level of SPI_IO[7:0]
is output by the MSPI controller in the second half part of dummy phase. It is u
sed to pre-drive flash..*/
#define SPI_MEM_FDUMMY_WOUT    (BIT(3))
#define SPI_MEM_FDUMMY_WOUT_M  (BIT(3))
#define SPI_MEM_FDUMMY_WOUT_V  0x1
#define SPI_MEM_FDUMMY_WOUT_S  3
/* SPI_MEM_FDUMMY_RIN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: In an MSPI read data transfer when accesses to flash, the level of SPI_IO[7:0] i
s output by the MSPI controller in the first half part of dummy phase. It is use
d to mask invalid SPI_DQS in the half part of dummy phase..*/
#define SPI_MEM_FDUMMY_RIN    (BIT(2))
#define SPI_MEM_FDUMMY_RIN_M  (BIT(2))
#define SPI_MEM_FDUMMY_RIN_V  0x1
#define SPI_MEM_FDUMMY_RIN_S  2
/* SPI_MEM_WDUMMY_ALWAYS_OUT : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: In the dummy phase of an MSPI write data transfer when accesses to flash, the le
vel of SPI_IO[7:0] is output by the MSPI controller..*/
#define SPI_MEM_WDUMMY_ALWAYS_OUT    (BIT(1))
#define SPI_MEM_WDUMMY_ALWAYS_OUT_M  (BIT(1))
#define SPI_MEM_WDUMMY_ALWAYS_OUT_V  0x1
#define SPI_MEM_WDUMMY_ALWAYS_OUT_S  1
/* SPI_MEM_WDUMMY_DQS_ALWAYS_OUT : HRO ;bitpos:[0] ;default: 1'b0 ; */
/*description: In the dummy phase of an MSPI write data transfer when accesses to flash, the le
vel of SPI_DQS is output by the MSPI controller..*/
#define SPI_MEM_WDUMMY_DQS_ALWAYS_OUT    (BIT(0))
#define SPI_MEM_WDUMMY_DQS_ALWAYS_OUT_M  (BIT(0))
#define SPI_MEM_WDUMMY_DQS_ALWAYS_OUT_V  0x1
#define SPI_MEM_WDUMMY_DQS_ALWAYS_OUT_S  0

#define SPI_MEM_CTRL1_REG(i)          (REG_SPI_MEM_BASE(i) + 0xC)
/* SPI_MEM_TXFIFO_RST : WT ;bitpos:[31] ;default: 1'b0 ; */
/*description: The synchronous reset signal for SPI0 TX AFIFO and all the AES_MSPI SYNC FIFO to
 send signals to AXI. Set this bit to reset these FIFO..*/
#define SPI_MEM_TXFIFO_RST    (BIT(31))
#define SPI_MEM_TXFIFO_RST_M  (BIT(31))
#define SPI_MEM_TXFIFO_RST_V  0x1
#define SPI_MEM_TXFIFO_RST_S  31
/* SPI_MEM_RXFIFO_RST : WT ;bitpos:[30] ;default: 1'b0 ; */
/*description: The synchronous reset signal for SPI0 RX AFIFO and all the AES_MSPI SYNC FIFO to
 receive signals from AXI.  Set this bit to reset these FIFO..*/
#define SPI_MEM_RXFIFO_RST    (BIT(30))
#define SPI_MEM_RXFIFO_RST_M  (BIT(30))
#define SPI_MEM_RXFIFO_RST_V  0x1
#define SPI_MEM_RXFIFO_RST_S  30
/* SPI_MEM_FAST_WRITE_EN : R/W ;bitpos:[29] ;default: 1'b1 ; */
/*description: Set this bit to write data faster, do not wait write data has been stored in tx_
bus_fifo_l2. It will wait 4*T_clk_ctrl to insure the write data has been stored
in  tx_bus_fifo_l2..*/
#define SPI_MEM_FAST_WRITE_EN    (BIT(29))
#define SPI_MEM_FAST_WRITE_EN_M  (BIT(29))
#define SPI_MEM_FAST_WRITE_EN_V  0x1
#define SPI_MEM_FAST_WRITE_EN_S  29
/* SPI_MEM_DUAL_RAM_EN : HRO ;bitpos:[28] ;default: 1'b0 ; */
/*description: Set this bit to enable DUAL-RAM mode, EXT_RAM0 and EXT_RAM1 will be accessed at
the same time..*/
#define SPI_MEM_DUAL_RAM_EN    (BIT(28))
#define SPI_MEM_DUAL_RAM_EN_M  (BIT(28))
#define SPI_MEM_DUAL_RAM_EN_V  0x1
#define SPI_MEM_DUAL_RAM_EN_S  28
/* SPI_MEM_RAM0_EN : HRO ;bitpos:[27] ;default: 1'b1 ; */
/*description: When SPI_MEM_DUAL_RAM_EN is 0 and SPI_MEM_RAM0_EN is 1, only EXT_RAM0 will be ac
cessed. When SPI_MEM_DUAL_RAM_EN is 0 and SPI_MEM_RAM0_EN is 0, only EXT_RAM1 wi
ll be accessed. When SPI_MEM_DUAL_RAM_EN is 1,  EXT_RAM0 and EXT_RAM1 will be ac
cessed at the same time..*/
#define SPI_MEM_RAM0_EN    (BIT(27))
#define SPI_MEM_RAM0_EN_M  (BIT(27))
#define SPI_MEM_RAM0_EN_V  0x1
#define SPI_MEM_RAM0_EN_S  27
/* SPI_MEM_AW_SPLICE_EN : HRO ;bitpos:[26] ;default: 1'b0 ; */
/*description: Set this bit to enable AXI Write Splice-transfer..*/
#define SPI_MEM_AW_SPLICE_EN    (BIT(26))
#define SPI_MEM_AW_SPLICE_EN_M  (BIT(26))
#define SPI_MEM_AW_SPLICE_EN_V  0x1
#define SPI_MEM_AW_SPLICE_EN_S  26
/* SPI_MEM_AR_SPLICE_EN : HRO ;bitpos:[25] ;default: 1'b0 ; */
/*description: Set this bit to enable AXI Read Splice-transfer..*/
#define SPI_MEM_AR_SPLICE_EN    (BIT(25))
#define SPI_MEM_AR_SPLICE_EN_M  (BIT(25))
#define SPI_MEM_AR_SPLICE_EN_V  0x1
#define SPI_MEM_AR_SPLICE_EN_S  25
/* SPI_MEM_RRESP_ECC_ERR_EN : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: 1: RRESP is SLV_ERR when there is a ECC error in AXI read data. 0: RRESP is OKAY
 when there is a ECC error in AXI read data. The ECC error information is record
ed in SPI_MEM_ECC_ERR_ADDR_REG..*/
#define SPI_MEM_RRESP_ECC_ERR_EN    (BIT(24))
#define SPI_MEM_RRESP_ECC_ERR_EN_M  (BIT(24))
#define SPI_MEM_RRESP_ECC_ERR_EN_V  0x1
#define SPI_MEM_RRESP_ECC_ERR_EN_S  24
/* SPI_MEM_AXI_RDATA_BACK_FAST : HRO ;bitpos:[23] ;default: 1'b1 ; */
/*description: 1: Reply AXI read data to AXI bus when one AXI read beat data is available. 0: R
eply AXI read data to AXI bus when all the read data is available..*/
#define SPI_MEM_AXI_RDATA_BACK_FAST    (BIT(23))
#define SPI_MEM_AXI_RDATA_BACK_FAST_M  (BIT(23))
#define SPI_MEM_AXI_RDATA_BACK_FAST_V  0x1
#define SPI_MEM_AXI_RDATA_BACK_FAST_S  23
/* SPI_MEM_AW_SIZE0_1_SUPPORT_EN : R/W ;bitpos:[22] ;default: 1'b1 ; */
/*description: 1: MSPI supports AWSIZE 0~3. 0: When AWSIZE 0~1, MSPI reply SLV_ERR..*/
#define SPI_MEM_AW_SIZE0_1_SUPPORT_EN    (BIT(22))
#define SPI_MEM_AW_SIZE0_1_SUPPORT_EN_M  (BIT(22))
#define SPI_MEM_AW_SIZE0_1_SUPPORT_EN_V  0x1
#define SPI_MEM_AW_SIZE0_1_SUPPORT_EN_S  22
/* SPI_MEM_AR_SIZE0_1_SUPPORT_EN : R/W ;bitpos:[21] ;default: 1'b1 ; */
/*description: 1: MSPI supports ARSIZE 0~3. When ARSIZE =0~2, MSPI read address is 4*n and repl
y the real AXI read data back. 0: When ARSIZE 0~1, MSPI reply SLV_ERR..*/
#define SPI_MEM_AR_SIZE0_1_SUPPORT_EN    (BIT(21))
#define SPI_MEM_AR_SIZE0_1_SUPPORT_EN_M  (BIT(21))
#define SPI_MEM_AR_SIZE0_1_SUPPORT_EN_V  0x1
#define SPI_MEM_AR_SIZE0_1_SUPPORT_EN_S  21
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
ve 3: SPI clock is always on..*/
#define SPI_MEM_CLK_MODE    0x00000003
#define SPI_MEM_CLK_MODE_M  ((SPI_MEM_CLK_MODE_V)<<(SPI_MEM_CLK_MODE_S))
#define SPI_MEM_CLK_MODE_V  0x3
#define SPI_MEM_CLK_MODE_S  0

#define SPI_MEM_CTRL2_REG(i)          (REG_SPI_MEM_BASE(i) + 0x10)
/* SPI_MEM_SYNC_RESET : WT ;bitpos:[31] ;default: 1'b0 ; */
/*description: The spi0_mst_st and spi0_slv_st will be reset..*/
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
/* SPI_MEM_SPLIT_TRANS_EN : HRO ;bitpos:[24] ;default: 1'b0 ; */
/*description: Set this bit to enable SPI0 split one AXI read flash transfer into two SPI trans
fers when one transfer will cross flash or EXT_RAM page corner, valid no matter
whether there is an ECC region or not..*/
#define SPI_MEM_SPLIT_TRANS_EN    (BIT(24))
#define SPI_MEM_SPLIT_TRANS_EN_M  (BIT(24))
#define SPI_MEM_SPLIT_TRANS_EN_V  0x1
#define SPI_MEM_SPLIT_TRANS_EN_S  24
/* SPI_MEM_ECC_16TO18_BYTE_EN : HRO ;bitpos:[14] ;default: 1'b0 ; */
/*description: Set this bit to enable SPI0 and SPI1 ECC 16 bytes data with 2 ECC bytes mode when
accesses flash..*/
#define SPI_MEM_ECC_16TO18_BYTE_EN    (BIT(14))
#define SPI_MEM_ECC_16TO18_BYTE_EN_M  (BIT(14))
#define SPI_MEM_ECC_16TO18_BYTE_EN_V  0x1
#define SPI_MEM_ECC_16TO18_BYTE_EN_S  14
/* SPI_MEM_ECC_SKIP_PAGE_CORNER : HRO ;bitpos:[13] ;default: 1'b1 ; */
/*description: 1: SPI0 and SPI1 skip page corner when accesses flash. 0: Not skip page corner w
hen accesses flash..*/
#define SPI_MEM_ECC_SKIP_PAGE_CORNER    (BIT(13))
#define SPI_MEM_ECC_SKIP_PAGE_CORNER_M  (BIT(13))
#define SPI_MEM_ECC_SKIP_PAGE_CORNER_V  0x1
#define SPI_MEM_ECC_SKIP_PAGE_CORNER_S  13
/* SPI_MEM_ECC_CS_HOLD_TIME : HRO ;bitpos:[12:10] ;default: 3'd3 ; */
/*description: SPI_MEM_CS_HOLD_TIME + SPI_MEM_ECC_CS_HOLD_TIME is the SPI0 CS hold cycle in ECC
 mode when accessed flash..*/
#define SPI_MEM_ECC_CS_HOLD_TIME    0x00000007
#define SPI_MEM_ECC_CS_HOLD_TIME_M  ((SPI_MEM_ECC_CS_HOLD_TIME_V)<<(SPI_MEM_ECC_CS_HOLD_TIME_S))
#define SPI_MEM_ECC_CS_HOLD_TIME_V  0x7
#define SPI_MEM_ECC_CS_HOLD_TIME_S  10
/* SPI_MEM_CS_HOLD_TIME : R/W ;bitpos:[9:5] ;default: 5'h1 ; */
/*description: SPI CS signal is delayed to inactive by SPI bus clock, this bits are combined wi
th SPI_MEM_CS_HOLD bit..*/
#define SPI_MEM_CS_HOLD_TIME    0x0000001F
#define SPI_MEM_CS_HOLD_TIME_M  ((SPI_MEM_CS_HOLD_TIME_V)<<(SPI_MEM_CS_HOLD_TIME_S))
#define SPI_MEM_CS_HOLD_TIME_V  0x1F
#define SPI_MEM_CS_HOLD_TIME_S  5
/* SPI_MEM_CS_SETUP_TIME : R/W ;bitpos:[4:0] ;default: 5'h1 ; */
/*description: (cycles-1) of prepare phase by SPI Bus clock, this bits are combined with SPI_ME
M_CS_SETUP bit..*/
#define SPI_MEM_CS_SETUP_TIME    0x0000001F
#define SPI_MEM_CS_SETUP_TIME_M  ((SPI_MEM_CS_SETUP_TIME_V)<<(SPI_MEM_CS_SETUP_TIME_S))
#define SPI_MEM_CS_SETUP_TIME_V  0x1F
#define SPI_MEM_CS_SETUP_TIME_S  0

#define SPI_MEM_CLOCK_REG(i)          (REG_SPI_MEM_BASE(i) + 0x14)
/* SPI_MEM_CLK_EQU_SYSCLK : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: 1: 1-division mode, the frequency of SPI bus clock equals to that of MSPI module
 clock..*/
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
/* SPI_MEM_USR_MOSI_HIGHPART : HRO ;bitpos:[25] ;default: 1'b0 ; */
/*description: write-data phase only access to high-part of the buffer spi_mem_w8~spi_mem_w15.
1: enable 0: disable..*/
#define SPI_MEM_USR_MOSI_HIGHPART    (BIT(25))
#define SPI_MEM_USR_MOSI_HIGHPART_M  (BIT(25))
#define SPI_MEM_USR_MOSI_HIGHPART_V  0x1
#define SPI_MEM_USR_MOSI_HIGHPART_S  25
/* SPI_MEM_USR_MISO_HIGHPART : HRO ;bitpos:[24] ;default: 1'b0 ; */
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
/*description: The bit combined with SPI_MEM_CK_IDLE_EDGE bit to control SPI clock mode 0~3..*/
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
/* SPI_MEM_USR_DBYTELEN : HRO ;bitpos:[8:6] ;default: 3'd1 ; */
/*description: SPI0 USR_CMD read or write data byte length -1.*/
#define SPI_MEM_USR_DBYTELEN    0x00000007
#define SPI_MEM_USR_DBYTELEN_M  ((SPI_MEM_USR_DBYTELEN_V)<<(SPI_MEM_USR_DBYTELEN_S))
#define SPI_MEM_USR_DBYTELEN_V  0x7
#define SPI_MEM_USR_DBYTELEN_S  6
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
/*description: SPI_CS line keep low when the bit is set..*/
#define SPI_MEM_CS_KEEP_ACTIVE    (BIT(10))
#define SPI_MEM_CS_KEEP_ACTIVE_M  (BIT(10))
#define SPI_MEM_CS_KEEP_ACTIVE_V  0x1
#define SPI_MEM_CS_KEEP_ACTIVE_S  10
/* SPI_MEM_CK_IDLE_EDGE : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: 1: SPI_CLK line is high when idle     0: spi clk line is low when idle.*/
#define SPI_MEM_CK_IDLE_EDGE    (BIT(9))
#define SPI_MEM_CK_IDLE_EDGE_M  (BIT(9))
#define SPI_MEM_CK_IDLE_EDGE_V  0x1
#define SPI_MEM_CK_IDLE_EDGE_S  9
/* SPI_MEM_SSUB_PIN : HRO ;bitpos:[8] ;default: 1'b0 ; */
/*description: For SPI0,  sram is connected to SUBPINs..*/
#define SPI_MEM_SSUB_PIN    (BIT(8))
#define SPI_MEM_SSUB_PIN_M  (BIT(8))
#define SPI_MEM_SSUB_PIN_V  0x1
#define SPI_MEM_SSUB_PIN_S  8
/* SPI_MEM_FSUB_PIN : HRO ;bitpos:[7] ;default: 1'b0 ; */
/*description: For SPI0,  flash is connected to SUBPINs..*/
#define SPI_MEM_FSUB_PIN    (BIT(7))
#define SPI_MEM_FSUB_PIN_M  (BIT(7))
#define SPI_MEM_FSUB_PIN_V  0x1
#define SPI_MEM_FSUB_PIN_S  7
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
/* SPI_MEM_CLOSE_AXI_INF_EN : R/W ;bitpos:[31] ;default: 1'b1 ; */
/*description: Set this bit to close AXI read/write transfer to MSPI, which means that only SLV
_ERR will be replied to BRESP/RRESP..*/
#define SPI_MEM_CLOSE_AXI_INF_EN    (BIT(31))
#define SPI_MEM_CLOSE_AXI_INF_EN_M  (BIT(31))
#define SPI_MEM_CLOSE_AXI_INF_EN_V  0x1
#define SPI_MEM_CLOSE_AXI_INF_EN_S  31
/* SPI_MEM_SAME_AW_AR_ADDR_CHK_EN : HRO ;bitpos:[30] ;default: 1'b1 ; */
/*description: Set this bit to check AXI read/write the same address region..*/
#define SPI_MEM_SAME_AW_AR_ADDR_CHK_EN    (BIT(30))
#define SPI_MEM_SAME_AW_AR_ADDR_CHK_EN_M  (BIT(30))
#define SPI_MEM_SAME_AW_AR_ADDR_CHK_EN_V  0x1
#define SPI_MEM_SAME_AW_AR_ADDR_CHK_EN_S  30
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
/* SPI_MEM_AXI_REQ_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: For SPI0, AXI master access enable, 1: enable, 0:disable..*/
#define SPI_MEM_AXI_REQ_EN    (BIT(0))
#define SPI_MEM_AXI_REQ_EN_M  (BIT(0))
#define SPI_MEM_AXI_REQ_EN_V  0x1
#define SPI_MEM_AXI_REQ_EN_S  0

#define SPI_MEM_CACHE_SCTRL_REG(i)          (REG_SPI_MEM_BASE(i) + 0x40)
/* SPI_MEM_SRAM_WDUMMY_CYCLELEN : HRO ;bitpos:[27:22] ;default: 6'b1 ; */
/*description: For SPI0, In the external RAM mode, it is the length in bits of write dummy phas
e. The register value shall be (bit_num-1)..*/
#define SPI_MEM_SRAM_WDUMMY_CYCLELEN    0x0000003F
#define SPI_MEM_SRAM_WDUMMY_CYCLELEN_M  ((SPI_MEM_SRAM_WDUMMY_CYCLELEN_V)<<(SPI_MEM_SRAM_WDUMMY_CYCLELEN_S))
#define SPI_MEM_SRAM_WDUMMY_CYCLELEN_V  0x3F
#define SPI_MEM_SRAM_WDUMMY_CYCLELEN_S  22
/* SPI_MEM_SRAM_OCT : HRO ;bitpos:[21] ;default: 1'b0 ; */
/*description: reserved.*/
#define SPI_MEM_SRAM_OCT    (BIT(21))
#define SPI_MEM_SRAM_OCT_M  (BIT(21))
#define SPI_MEM_SRAM_OCT_V  0x1
#define SPI_MEM_SRAM_OCT_S  21
/* SPI_MEM_CACHE_SRAM_USR_WCMD : HRO ;bitpos:[20] ;default: 1'b1 ; */
/*description: For SPI0, In the external RAM mode cache write sram for user define command.*/
#define SPI_MEM_CACHE_SRAM_USR_WCMD    (BIT(20))
#define SPI_MEM_CACHE_SRAM_USR_WCMD_M  (BIT(20))
#define SPI_MEM_CACHE_SRAM_USR_WCMD_V  0x1
#define SPI_MEM_CACHE_SRAM_USR_WCMD_S  20
/* SPI_MEM_SRAM_ADDR_BITLEN : HRO ;bitpos:[19:14] ;default: 6'd23 ; */
/*description: For SPI0, In the external RAM mode, it is the length in bits of address phase. T
he register value shall be (bit_num-1)..*/
#define SPI_MEM_SRAM_ADDR_BITLEN    0x0000003F
#define SPI_MEM_SRAM_ADDR_BITLEN_M  ((SPI_MEM_SRAM_ADDR_BITLEN_V)<<(SPI_MEM_SRAM_ADDR_BITLEN_S))
#define SPI_MEM_SRAM_ADDR_BITLEN_V  0x3F
#define SPI_MEM_SRAM_ADDR_BITLEN_S  14
/* SPI_MEM_SRAM_RDUMMY_CYCLELEN : HRO ;bitpos:[11:6] ;default: 6'b1 ; */
/*description: For SPI0, In the external RAM mode, it is the length in bits of read dummy phase
. The register value shall be (bit_num-1)..*/
#define SPI_MEM_SRAM_RDUMMY_CYCLELEN    0x0000003F
#define SPI_MEM_SRAM_RDUMMY_CYCLELEN_M  ((SPI_MEM_SRAM_RDUMMY_CYCLELEN_V)<<(SPI_MEM_SRAM_RDUMMY_CYCLELEN_S))
#define SPI_MEM_SRAM_RDUMMY_CYCLELEN_V  0x3F
#define SPI_MEM_SRAM_RDUMMY_CYCLELEN_S  6
/* SPI_MEM_CACHE_SRAM_USR_RCMD : HRO ;bitpos:[5] ;default: 1'b1 ; */
/*description: For SPI0, In the external RAM mode cache read external RAM for user define command
..*/
#define SPI_MEM_CACHE_SRAM_USR_RCMD    (BIT(5))
#define SPI_MEM_CACHE_SRAM_USR_RCMD_M  (BIT(5))
#define SPI_MEM_CACHE_SRAM_USR_RCMD_V  0x1
#define SPI_MEM_CACHE_SRAM_USR_RCMD_S  5
/* SPI_MEM_USR_RD_SRAM_DUMMY : HRO ;bitpos:[4] ;default: 1'b1 ; */
/*description: For SPI0, In the external RAM mode, it is the enable bit of dummy phase for read
 operations..*/
#define SPI_MEM_USR_RD_SRAM_DUMMY    (BIT(4))
#define SPI_MEM_USR_RD_SRAM_DUMMY_M  (BIT(4))
#define SPI_MEM_USR_RD_SRAM_DUMMY_V  0x1
#define SPI_MEM_USR_RD_SRAM_DUMMY_S  4
/* SPI_MEM_USR_WR_SRAM_DUMMY : HRO ;bitpos:[3] ;default: 1'b0 ; */
/*description: For SPI0, In the external RAM mode, it is the enable bit of dummy phase for writ
e operations..*/
#define SPI_MEM_USR_WR_SRAM_DUMMY    (BIT(3))
#define SPI_MEM_USR_WR_SRAM_DUMMY_M  (BIT(3))
#define SPI_MEM_USR_WR_SRAM_DUMMY_V  0x1
#define SPI_MEM_USR_WR_SRAM_DUMMY_S  3
/* SPI_MEM_USR_SRAM_QIO : HRO ;bitpos:[2] ;default: 1'b0 ; */
/*description: For SPI0, In the external RAM mode, spi quad I/O mode enable, 1: enable, 0:disab
le.*/
#define SPI_MEM_USR_SRAM_QIO    (BIT(2))
#define SPI_MEM_USR_SRAM_QIO_M  (BIT(2))
#define SPI_MEM_USR_SRAM_QIO_V  0x1
#define SPI_MEM_USR_SRAM_QIO_S  2
/* SPI_MEM_USR_SRAM_DIO : HRO ;bitpos:[1] ;default: 1'b0 ; */
/*description: For SPI0, In the external RAM mode, spi dual I/O mode enable, 1: enable, 0:disab
le.*/
#define SPI_MEM_USR_SRAM_DIO    (BIT(1))
#define SPI_MEM_USR_SRAM_DIO_M  (BIT(1))
#define SPI_MEM_USR_SRAM_DIO_V  0x1
#define SPI_MEM_USR_SRAM_DIO_S  1
/* SPI_MEM_CACHE_USR_SADDR_4BYTE : HRO ;bitpos:[0] ;default: 1'b0 ; */
/*description: For SPI0, In the external RAM mode, cache read flash with 4 bytes command, 1: en
able, 0:disable..*/
#define SPI_MEM_CACHE_USR_SADDR_4BYTE    (BIT(0))
#define SPI_MEM_CACHE_USR_SADDR_4BYTE_M  (BIT(0))
#define SPI_MEM_CACHE_USR_SADDR_4BYTE_V  0x1
#define SPI_MEM_CACHE_USR_SADDR_4BYTE_S  0

#define SPI_MEM_SRAM_CMD_REG(i)          (REG_SPI_MEM_BASE(i) + 0x44)
/* SPI_MEM_SMEM_DATA_IE_ALWAYS_ON : HRO ;bitpos:[31] ;default: 1'b1 ; */
/*description: When accesses to external RAM, 1: the IE signals of pads connected to SPI_IO[7:0
] are always 1. 0: Others..*/
#define SPI_MEM_SMEM_DATA_IE_ALWAYS_ON    (BIT(31))
#define SPI_MEM_SMEM_DATA_IE_ALWAYS_ON_M  (BIT(31))
#define SPI_MEM_SMEM_DATA_IE_ALWAYS_ON_V  0x1
#define SPI_MEM_SMEM_DATA_IE_ALWAYS_ON_S  31
/* SPI_MEM_SMEM_DQS_IE_ALWAYS_ON : HRO ;bitpos:[30] ;default: 1'b1 ; */
/*description: When accesses to external RAM, 1: the IE signals of pads connected to SPI_DQS ar
e always 1. 0: Others..*/
#define SPI_MEM_SMEM_DQS_IE_ALWAYS_ON    (BIT(30))
#define SPI_MEM_SMEM_DQS_IE_ALWAYS_ON_M  (BIT(30))
#define SPI_MEM_SMEM_DQS_IE_ALWAYS_ON_V  0x1
#define SPI_MEM_SMEM_DQS_IE_ALWAYS_ON_S  30
/* SPI_MEM_SMEM_WDUMMY_ALWAYS_OUT : HRO ;bitpos:[25] ;default: 1'b0 ; */
/*description: In the dummy phase of an MSPI write data transfer when accesses to external RAM,
 the level of SPI_IO[7:0] is output by the MSPI controller..*/
#define SPI_MEM_SMEM_WDUMMY_ALWAYS_OUT    (BIT(25))
#define SPI_MEM_SMEM_WDUMMY_ALWAYS_OUT_M  (BIT(25))
#define SPI_MEM_SMEM_WDUMMY_ALWAYS_OUT_V  0x1
#define SPI_MEM_SMEM_WDUMMY_ALWAYS_OUT_S  25
/* SPI_MEM_SMEM_WDUMMY_DQS_ALWAYS_OUT : HRO ;bitpos:[24] ;default: 1'b0 ; */
/*description: In the dummy phase of an MSPI write data transfer when accesses to external RAM,
 the level of SPI_DQS is output by the MSPI controller..*/
#define SPI_MEM_SMEM_WDUMMY_DQS_ALWAYS_OUT    (BIT(24))
#define SPI_MEM_SMEM_WDUMMY_DQS_ALWAYS_OUT_M  (BIT(24))
#define SPI_MEM_SMEM_WDUMMY_DQS_ALWAYS_OUT_V  0x1
#define SPI_MEM_SMEM_WDUMMY_DQS_ALWAYS_OUT_S  24
/* SPI_MEM_SDUMMY_WOUT : HRO ;bitpos:[23] ;default: 1'b0 ; */
/*description: In the dummy phase of a MSPI write data transfer when accesses to external RAM,
the signal level of SPI bus is output by the MSPI controller..*/
#define SPI_MEM_SDUMMY_WOUT    (BIT(23))
#define SPI_MEM_SDUMMY_WOUT_M  (BIT(23))
#define SPI_MEM_SDUMMY_WOUT_V  0x1
#define SPI_MEM_SDUMMY_WOUT_S  23
/* SPI_MEM_SDUMMY_RIN : R/W ;bitpos:[22] ;default: 1'b1 ; */
/*description: In the dummy phase of a MSPI read data transfer when accesses to external RAM, t
he signal level of SPI bus is output by the MSPI controller..*/
#define SPI_MEM_SDUMMY_RIN    (BIT(22))
#define SPI_MEM_SDUMMY_RIN_M  (BIT(22))
#define SPI_MEM_SDUMMY_RIN_V  0x1
#define SPI_MEM_SDUMMY_RIN_S  22
/* SPI_MEM_SCMD_OCT : HRO ;bitpos:[21] ;default: 1'b0 ; */
/*description: For SPI0 external RAM , cmd phase apply 8 signals. 1: enable 0: disable..*/
#define SPI_MEM_SCMD_OCT    (BIT(21))
#define SPI_MEM_SCMD_OCT_M  (BIT(21))
#define SPI_MEM_SCMD_OCT_V  0x1
#define SPI_MEM_SCMD_OCT_S  21
/* SPI_MEM_SADDR_OCT : HRO ;bitpos:[20] ;default: 1'b0 ; */
/*description: For SPI0 external RAM , address phase apply 4 signals. 1: enable 0: disable..*/
#define SPI_MEM_SADDR_OCT    (BIT(20))
#define SPI_MEM_SADDR_OCT_M  (BIT(20))
#define SPI_MEM_SADDR_OCT_V  0x1
#define SPI_MEM_SADDR_OCT_S  20
/* SPI_MEM_SDOUT_OCT : HRO ;bitpos:[19] ;default: 1'b0 ; */
/*description: For SPI0 external RAM , dout phase apply 8 signals. 1: enable 0: disable..*/
#define SPI_MEM_SDOUT_OCT    (BIT(19))
#define SPI_MEM_SDOUT_OCT_M  (BIT(19))
#define SPI_MEM_SDOUT_OCT_V  0x1
#define SPI_MEM_SDOUT_OCT_S  19
/* SPI_MEM_SDIN_OCT : HRO ;bitpos:[18] ;default: 1'b0 ; */
/*description: For SPI0 external RAM , din phase apply 8 signals. 1: enable 0: disable..*/
#define SPI_MEM_SDIN_OCT    (BIT(18))
#define SPI_MEM_SDIN_OCT_M  (BIT(18))
#define SPI_MEM_SDIN_OCT_V  0x1
#define SPI_MEM_SDIN_OCT_S  18
/* SPI_MEM_SCMD_QUAD : HRO ;bitpos:[17] ;default: 1'b0 ; */
/*description: For SPI0 external RAM , cmd phase apply 4 signals. 1: enable 0: disable. The bit
 is the same with spi_mem_usr_sram_qio..*/
#define SPI_MEM_SCMD_QUAD    (BIT(17))
#define SPI_MEM_SCMD_QUAD_M  (BIT(17))
#define SPI_MEM_SCMD_QUAD_V  0x1
#define SPI_MEM_SCMD_QUAD_S  17
/* SPI_MEM_SADDR_QUAD : HRO ;bitpos:[16] ;default: 1'b0 ; */
/*description: For SPI0 external RAM , address phase apply 4 signals. 1: enable 0: disable. The
 bit is the same with spi_mem_usr_sram_qio..*/
#define SPI_MEM_SADDR_QUAD    (BIT(16))
#define SPI_MEM_SADDR_QUAD_M  (BIT(16))
#define SPI_MEM_SADDR_QUAD_V  0x1
#define SPI_MEM_SADDR_QUAD_S  16
/* SPI_MEM_SDOUT_QUAD : HRO ;bitpos:[15] ;default: 1'b0 ; */
/*description: For SPI0 external RAM , dout phase apply 4 signals. 1: enable 0: disable. The bi
t is the same with spi_mem_usr_sram_qio..*/
#define SPI_MEM_SDOUT_QUAD    (BIT(15))
#define SPI_MEM_SDOUT_QUAD_M  (BIT(15))
#define SPI_MEM_SDOUT_QUAD_V  0x1
#define SPI_MEM_SDOUT_QUAD_S  15
/* SPI_MEM_SDIN_QUAD : HRO ;bitpos:[14] ;default: 1'b0 ; */
/*description: For SPI0 external RAM , din phase apply 4 signals. 1: enable 0: disable. The bit
 is the same with spi_mem_usr_sram_qio..*/
#define SPI_MEM_SDIN_QUAD    (BIT(14))
#define SPI_MEM_SDIN_QUAD_M  (BIT(14))
#define SPI_MEM_SDIN_QUAD_V  0x1
#define SPI_MEM_SDIN_QUAD_S  14
/* SPI_MEM_SADDR_DUAL : HRO ;bitpos:[12] ;default: 1'b0 ; */
/*description: For SPI0 external RAM , address phase apply 2 signals. 1: enable 0: disable. The
 bit is the same with spi_mem_usr_sram_dio..*/
#define SPI_MEM_SADDR_DUAL    (BIT(12))
#define SPI_MEM_SADDR_DUAL_M  (BIT(12))
#define SPI_MEM_SADDR_DUAL_V  0x1
#define SPI_MEM_SADDR_DUAL_S  12
/* SPI_MEM_SDOUT_DUAL : HRO ;bitpos:[11] ;default: 1'b0 ; */
/*description: For SPI0 external RAM , dout phase apply 2 signals. 1: enable 0: disable. The bi
t is the same with spi_mem_usr_sram_dio..*/
#define SPI_MEM_SDOUT_DUAL    (BIT(11))
#define SPI_MEM_SDOUT_DUAL_M  (BIT(11))
#define SPI_MEM_SDOUT_DUAL_V  0x1
#define SPI_MEM_SDOUT_DUAL_S  11
/* SPI_MEM_SDIN_DUAL : HRO ;bitpos:[10] ;default: 1'b0 ; */
/*description: For SPI0 external RAM , din phase apply 2 signals. 1: enable 0: disable. The bit
 is the same with spi_mem_usr_sram_dio..*/
#define SPI_MEM_SDIN_DUAL    (BIT(10))
#define SPI_MEM_SDIN_DUAL_M  (BIT(10))
#define SPI_MEM_SDIN_DUAL_V  0x1
#define SPI_MEM_SDIN_DUAL_S  10
/* SPI_MEM_SWB_MODE : HRO ;bitpos:[9:2] ;default: 8'b0 ; */
/*description: Mode bits in the external RAM fast read mode  it is combined with spi_mem_fastrd
_mode bit..*/
#define SPI_MEM_SWB_MODE    0x000000FF
#define SPI_MEM_SWB_MODE_M  ((SPI_MEM_SWB_MODE_V)<<(SPI_MEM_SWB_MODE_S))
#define SPI_MEM_SWB_MODE_V  0xFF
#define SPI_MEM_SWB_MODE_S  2
/* SPI_MEM_SCLK_MODE : HRO ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delaye
d one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inacti
ve 3: SPI clock is always on..*/
#define SPI_MEM_SCLK_MODE    0x00000003
#define SPI_MEM_SCLK_MODE_M  ((SPI_MEM_SCLK_MODE_V)<<(SPI_MEM_SCLK_MODE_S))
#define SPI_MEM_SCLK_MODE_V  0x3
#define SPI_MEM_SCLK_MODE_S  0

#define SPI_MEM_SRAM_DRD_CMD_REG(i)          (REG_SPI_MEM_BASE(i) + 0x48)
/* SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN : HRO ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: For SPI0,When cache mode is enable it is the length in bits of command phase for
 sram. The register value shall be (bit_num-1)..*/
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN    0x0000000F
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_M  ((SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_V)<<(SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_S))
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_V  0xF
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_S  28
/* SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE : HRO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: For SPI0,When cache mode is enable it is the read command value of command phase
 for sram..*/
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE    0x0000FFFF
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_M  ((SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_V)<<(SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_S))
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_V  0xFFFF
#define SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_S  0

#define SPI_MEM_SRAM_DWR_CMD_REG(i)          (REG_SPI_MEM_BASE(i) + 0x4C)
/* SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN : HRO ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: For SPI0,When cache mode is enable it is the in bits of command phase  for sram.
 The register value shall be (bit_num-1)..*/
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN    0x0000000F
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_M  ((SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_V)<<(SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_S))
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_V  0xF
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_S  28
/* SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE : HRO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: For SPI0,When cache mode is enable it is the write command value of command phas
e for sram..*/
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE    0x0000FFFF
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_M  ((SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_V)<<(SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_S))
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_V  0xFFFF
#define SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_S  0

#define SPI_MEM_SRAM_CLK_REG(i)          (REG_SPI_MEM_BASE(i) + 0x50)
/* SPI_MEM_SCLK_EQU_SYSCLK : HRO ;bitpos:[31] ;default: 1'b0 ; */
/*description: For SPI0 external RAM  interface, 1: spi_mem_clk is equal to system 0: spi_mem_c
lk is divided from system clock..*/
#define SPI_MEM_SCLK_EQU_SYSCLK    (BIT(31))
#define SPI_MEM_SCLK_EQU_SYSCLK_M  (BIT(31))
#define SPI_MEM_SCLK_EQU_SYSCLK_V  0x1
#define SPI_MEM_SCLK_EQU_SYSCLK_S  31
/* SPI_MEM_SCLKCNT_N : HRO ;bitpos:[23:16] ;default: 8'h3 ; */
/*description: For SPI0 external RAM  interface, it is the divider of spi_mem_clk. So spi_mem_c
lk frequency is system/(spi_mem_clkcnt_N+1).*/
#define SPI_MEM_SCLKCNT_N    0x000000FF
#define SPI_MEM_SCLKCNT_N_M  ((SPI_MEM_SCLKCNT_N_V)<<(SPI_MEM_SCLKCNT_N_S))
#define SPI_MEM_SCLKCNT_N_V  0xFF
#define SPI_MEM_SCLKCNT_N_S  16
/* SPI_MEM_SCLKCNT_H : HRO ;bitpos:[15:8] ;default: 8'h1 ; */
/*description: For SPI0 external RAM  interface, it must be floor((spi_mem_clkcnt_N+1)/2-1)..*/
#define SPI_MEM_SCLKCNT_H    0x000000FF
#define SPI_MEM_SCLKCNT_H_M  ((SPI_MEM_SCLKCNT_H_V)<<(SPI_MEM_SCLKCNT_H_S))
#define SPI_MEM_SCLKCNT_H_V  0xFF
#define SPI_MEM_SCLKCNT_H_S  8
/* SPI_MEM_SCLKCNT_L : HRO ;bitpos:[7:0] ;default: 8'h3 ; */
/*description: For SPI0 external RAM  interface, it must be equal to spi_mem_clkcnt_N..*/
#define SPI_MEM_SCLKCNT_L    0x000000FF
#define SPI_MEM_SCLKCNT_L_M  ((SPI_MEM_SCLKCNT_L_V)<<(SPI_MEM_SCLKCNT_L_S))
#define SPI_MEM_SCLKCNT_L_V  0xFF
#define SPI_MEM_SCLKCNT_L_S  0

#define SPI_MEM_FSM_REG(i)          (REG_SPI_MEM_BASE(i) + 0x54)
/* SPI_MEM_LOCK_DELAY_TIME : R/W ;bitpos:[11:7] ;default: 5'd4 ; */
/*description: The lock delay time of SPI0/1 arbiter by spi0_slv_st, after PER is sent by SPI1..*/
#define SPI_MEM_LOCK_DELAY_TIME    0x0000001F
#define SPI_MEM_LOCK_DELAY_TIME_M  ((SPI_MEM_LOCK_DELAY_TIME_V)<<(SPI_MEM_LOCK_DELAY_TIME_S))
#define SPI_MEM_LOCK_DELAY_TIME_V  0x1F
#define SPI_MEM_LOCK_DELAY_TIME_S  7

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
/* SPI_MEM_WAITI_CMD : R/W ;bitpos:[31:16] ;default: 16'h05 ; */
/*description: The command value to wait flash idle(RDSR)..*/
#define SPI_MEM_WAITI_CMD    0x0000FFFF
#define SPI_MEM_WAITI_CMD_M  ((SPI_MEM_WAITI_CMD_V)<<(SPI_MEM_WAITI_CMD_S))
#define SPI_MEM_WAITI_CMD_V  0xFFFF
#define SPI_MEM_WAITI_CMD_S  16
/* SPI_MEM_WAITI_DUMMY_CYCLELEN : R/W ;bitpos:[15:10] ;default: 6'h0 ; */
/*description: The dummy cycle length when wait flash idle(RDSR)..*/
#define SPI_MEM_WAITI_DUMMY_CYCLELEN    0x0000003F
#define SPI_MEM_WAITI_DUMMY_CYCLELEN_M  ((SPI_MEM_WAITI_DUMMY_CYCLELEN_V)<<(SPI_MEM_WAITI_DUMMY_CYCLELEN_S))
#define SPI_MEM_WAITI_DUMMY_CYCLELEN_V  0x3F
#define SPI_MEM_WAITI_DUMMY_CYCLELEN_S  10
/* SPI_MEM_WAITI_CMD_2B : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: 1:The wait idle command bit length is 16. 0: The wait idle command bit length is
 8..*/
#define SPI_MEM_WAITI_CMD_2B    (BIT(9))
#define SPI_MEM_WAITI_CMD_2B_M  (BIT(9))
#define SPI_MEM_WAITI_CMD_2B_V  0x1
#define SPI_MEM_WAITI_CMD_2B_S  9
/* SPI_MEM_WAITI_ADDR_CYCLELEN : R/W ;bitpos:[4:3] ;default: 2'b0 ; */
/*description: When SPI_MEM_WAITI_ADDR_EN is set, the  cycle length of sent out address is (SPI
_MEM_WAITI_ADDR_CYCLELEN[1:0] + 1) SPI  bus clock cycles. It is not active when
SPI_MEM_WAITI_ADDR_EN is cleared..*/
#define SPI_MEM_WAITI_ADDR_CYCLELEN    0x00000003
#define SPI_MEM_WAITI_ADDR_CYCLELEN_M  ((SPI_MEM_WAITI_ADDR_CYCLELEN_V)<<(SPI_MEM_WAITI_ADDR_CYCLELEN_S))
#define SPI_MEM_WAITI_ADDR_CYCLELEN_V  0x3
#define SPI_MEM_WAITI_ADDR_CYCLELEN_S  3
/* SPI_MEM_WAITI_ADDR_EN : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: 1: Output  address 0 in RDSR or read SUS command transfer. 0: Do not send out ad
dress in RDSR or read SUS command transfer..*/
#define SPI_MEM_WAITI_ADDR_EN    (BIT(2))
#define SPI_MEM_WAITI_ADDR_EN_M  (BIT(2))
#define SPI_MEM_WAITI_ADDR_EN_V  0x1
#define SPI_MEM_WAITI_ADDR_EN_S  2
/* SPI_MEM_WAITI_DUMMY : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The dummy phase enable when wait flash idle (RDSR).*/
#define SPI_MEM_WAITI_DUMMY    (BIT(1))
#define SPI_MEM_WAITI_DUMMY_M  (BIT(1))
#define SPI_MEM_WAITI_DUMMY_V  0x1
#define SPI_MEM_WAITI_DUMMY_S  1
/* SPI_MEM_WAITI_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: 1: The hardware will wait idle after SE/PP/WRSR automatically, and hardware auto
 Suspend/Resume can be enabled. 0: The functions of hardware wait idle and auto
Suspend/Resume are not supported..*/
#define SPI_MEM_WAITI_EN    (BIT(0))
#define SPI_MEM_WAITI_EN_M  (BIT(0))
#define SPI_MEM_WAITI_EN_V  0x1
#define SPI_MEM_WAITI_EN_S  0

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
application should send PER after PES is done..*/
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
/*description: program erase suspend bit, program erase suspend operation will be triggered when
the bit is set. The bit will be cleared once the operation done.1: enable 0: d
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
/* SPI_MEM_FLASH_PES_COMMAND : R/W ;bitpos:[15:0] ;default: 16'h7575 ; */
/*description: Program/Erase suspend command..*/
#define SPI_MEM_FLASH_PES_COMMAND    0x0000FFFF
#define SPI_MEM_FLASH_PES_COMMAND_M  ((SPI_MEM_FLASH_PES_COMMAND_V)<<(SPI_MEM_FLASH_PES_COMMAND_S))
#define SPI_MEM_FLASH_PES_COMMAND_V  0xFFFF
#define SPI_MEM_FLASH_PES_COMMAND_S  0

#define SPI_MEM_SUS_STATUS_REG(i)          (REG_SPI_MEM_BASE(i) + 0xA4)
/* SPI_MEM_FLASH_PER_COMMAND : R/W ;bitpos:[31:16] ;default: 16'h7a7a ; */
/*description: Program/Erase resume command..*/
#define SPI_MEM_FLASH_PER_COMMAND    0x0000FFFF
#define SPI_MEM_FLASH_PER_COMMAND_M  ((SPI_MEM_FLASH_PER_COMMAND_V)<<(SPI_MEM_FLASH_PER_COMMAND_S))
#define SPI_MEM_FLASH_PER_COMMAND_V  0xFFFF
#define SPI_MEM_FLASH_PER_COMMAND_S  16
/* SPI_MEM_FLASH_PESR_CMD_2B : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: 1: The bit length of Program/Erase Suspend/Resume command is 16. 0: The bit leng
th of Program/Erase Suspend/Resume command is 8..*/
#define SPI_MEM_FLASH_PESR_CMD_2B    (BIT(15))
#define SPI_MEM_FLASH_PESR_CMD_2B_M  (BIT(15))
#define SPI_MEM_FLASH_PESR_CMD_2B_V  0x1
#define SPI_MEM_FLASH_PESR_CMD_2B_S  15
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

#define SPI_MEM_INT_ENA_REG(i)          (REG_SPI_MEM_BASE(i) + 0xC0)
/* SPI_MEM_BROWN_OUT_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: The enable bit for SPI_MEM_BROWN_OUT_INT interrupt..*/
#define SPI_MEM_BROWN_OUT_INT_ENA    (BIT(10))
#define SPI_MEM_BROWN_OUT_INT_ENA_M  (BIT(10))
#define SPI_MEM_BROWN_OUT_INT_ENA_V  0x1
#define SPI_MEM_BROWN_OUT_INT_ENA_S  10
/* SPI_MEM_AXI_WADDR_ERR_INT__ENA : HRO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The enable bit for SPI_MEM_AXI_WADDR_ERR_INT interrupt..*/
#define SPI_MEM_AXI_WADDR_ERR_INT__ENA    (BIT(9))
#define SPI_MEM_AXI_WADDR_ERR_INT__ENA_M  (BIT(9))
#define SPI_MEM_AXI_WADDR_ERR_INT__ENA_V  0x1
#define SPI_MEM_AXI_WADDR_ERR_INT__ENA_S  9
/* SPI_MEM_AXI_WR_FLASH_ERR_INT_ENA : HRO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The enable bit for SPI_MEM_AXI_WR_FALSH_ERR_INT interrupt..*/
#define SPI_MEM_AXI_WR_FLASH_ERR_INT_ENA    (BIT(8))
#define SPI_MEM_AXI_WR_FLASH_ERR_INT_ENA_M  (BIT(8))
#define SPI_MEM_AXI_WR_FLASH_ERR_INT_ENA_V  0x1
#define SPI_MEM_AXI_WR_FLASH_ERR_INT_ENA_S  8
/* SPI_MEM_AXI_RADDR_ERR_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The enable bit for SPI_MEM_AXI_RADDR_ERR_INT interrupt..*/
#define SPI_MEM_AXI_RADDR_ERR_INT_ENA    (BIT(7))
#define SPI_MEM_AXI_RADDR_ERR_INT_ENA_M  (BIT(7))
#define SPI_MEM_AXI_RADDR_ERR_INT_ENA_V  0x1
#define SPI_MEM_AXI_RADDR_ERR_INT_ENA_S  7
/* SPI_MEM_PMS_REJECT_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The enable bit for SPI_MEM_PMS_REJECT_INT interrupt..*/
#define SPI_MEM_PMS_REJECT_INT_ENA    (BIT(6))
#define SPI_MEM_PMS_REJECT_INT_ENA_M  (BIT(6))
#define SPI_MEM_PMS_REJECT_INT_ENA_V  0x1
#define SPI_MEM_PMS_REJECT_INT_ENA_S  6
/* SPI_MEM_ECC_ERR_INT_ENA : HRO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The enable bit for SPI_MEM_ECC_ERR_INT interrupt..*/
#define SPI_MEM_ECC_ERR_INT_ENA    (BIT(5))
#define SPI_MEM_ECC_ERR_INT_ENA_M  (BIT(5))
#define SPI_MEM_ECC_ERR_INT_ENA_V  0x1
#define SPI_MEM_ECC_ERR_INT_ENA_S  5
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
/* SPI_MEM_BROWN_OUT_INT_CLR : WT ;bitpos:[10] ;default: 1'b0 ; */
/*description: The status bit for SPI_MEM_BROWN_OUT_INT interrupt..*/
#define SPI_MEM_BROWN_OUT_INT_CLR    (BIT(10))
#define SPI_MEM_BROWN_OUT_INT_CLR_M  (BIT(10))
#define SPI_MEM_BROWN_OUT_INT_CLR_V  0x1
#define SPI_MEM_BROWN_OUT_INT_CLR_S  10
/* SPI_MEM_AXI_WADDR_ERR_INT_CLR : HRO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The clear bit for SPI_MEM_AXI_WADDR_ERR_INT interrupt..*/
#define SPI_MEM_AXI_WADDR_ERR_INT_CLR    (BIT(9))
#define SPI_MEM_AXI_WADDR_ERR_INT_CLR_M  (BIT(9))
#define SPI_MEM_AXI_WADDR_ERR_INT_CLR_V  0x1
#define SPI_MEM_AXI_WADDR_ERR_INT_CLR_S  9
/* SPI_MEM_AXI_WR_FLASH_ERR_INT_CLR : HRO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The clear bit for SPI_MEM_AXI_WR_FALSH_ERR_INT interrupt..*/
#define SPI_MEM_AXI_WR_FLASH_ERR_INT_CLR    (BIT(8))
#define SPI_MEM_AXI_WR_FLASH_ERR_INT_CLR_M  (BIT(8))
#define SPI_MEM_AXI_WR_FLASH_ERR_INT_CLR_V  0x1
#define SPI_MEM_AXI_WR_FLASH_ERR_INT_CLR_S  8
/* SPI_MEM_AXI_RADDR_ERR_INT_CLR : WT ;bitpos:[7] ;default: 1'b0 ; */
/*description: The clear bit for SPI_MEM_AXI_RADDR_ERR_INT interrupt..*/
#define SPI_MEM_AXI_RADDR_ERR_INT_CLR    (BIT(7))
#define SPI_MEM_AXI_RADDR_ERR_INT_CLR_M  (BIT(7))
#define SPI_MEM_AXI_RADDR_ERR_INT_CLR_V  0x1
#define SPI_MEM_AXI_RADDR_ERR_INT_CLR_S  7
/* SPI_MEM_PMS_REJECT_INT_CLR : WT ;bitpos:[6] ;default: 1'b0 ; */
/*description: The clear bit for SPI_MEM_PMS_REJECT_INT interrupt..*/
#define SPI_MEM_PMS_REJECT_INT_CLR    (BIT(6))
#define SPI_MEM_PMS_REJECT_INT_CLR_M  (BIT(6))
#define SPI_MEM_PMS_REJECT_INT_CLR_V  0x1
#define SPI_MEM_PMS_REJECT_INT_CLR_S  6
/* SPI_MEM_ECC_ERR_INT_CLR : HRO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The clear bit for SPI_MEM_ECC_ERR_INT interrupt..*/
#define SPI_MEM_ECC_ERR_INT_CLR    (BIT(5))
#define SPI_MEM_ECC_ERR_INT_CLR_M  (BIT(5))
#define SPI_MEM_ECC_ERR_INT_CLR_V  0x1
#define SPI_MEM_ECC_ERR_INT_CLR_S  5
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
/* SPI_MEM_BROWN_OUT_INT_RAW : R/WTC/SS ;bitpos:[10] ;default: 1'b0 ; */
/*description: The raw bit for SPI_MEM_BROWN_OUT_INT interrupt. 1: Triggered condition is that
chip is losing power and RTC module sends out brown out close flash request to
SPI1. After SPI1 sends out suspend command to flash, this interrupt is triggered
 and MSPI returns to idle state. 0: Others..*/
#define SPI_MEM_BROWN_OUT_INT_RAW    (BIT(10))
#define SPI_MEM_BROWN_OUT_INT_RAW_M  (BIT(10))
#define SPI_MEM_BROWN_OUT_INT_RAW_V  0x1
#define SPI_MEM_BROWN_OUT_INT_RAW_S  10
/* SPI_MEM_AXI_WADDR_ERR_INT_RAW : HRO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw bit for SPI_MEM_AXI_WADDR_ERR_INT interrupt. 1: Triggered when AXI write
 address is invalid by compared to MMU configuration. 0: Others..*/
#define SPI_MEM_AXI_WADDR_ERR_INT_RAW    (BIT(9))
#define SPI_MEM_AXI_WADDR_ERR_INT_RAW_M  (BIT(9))
#define SPI_MEM_AXI_WADDR_ERR_INT_RAW_V  0x1
#define SPI_MEM_AXI_WADDR_ERR_INT_RAW_S  9
/* SPI_MEM_AXI_WR_FLASH_ERR_INT_RAW : HRO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw bit for SPI_MEM_AXI_WR_FALSH_ERR_INT interrupt. 1: Triggered when AXI wr
ite flash request is received. 0: Others..*/
#define SPI_MEM_AXI_WR_FLASH_ERR_INT_RAW    (BIT(8))
#define SPI_MEM_AXI_WR_FLASH_ERR_INT_RAW_M  (BIT(8))
#define SPI_MEM_AXI_WR_FLASH_ERR_INT_RAW_V  0x1
#define SPI_MEM_AXI_WR_FLASH_ERR_INT_RAW_S  8
/* SPI_MEM_AXI_RADDR_ERR_INT_RAW : R/WTC/SS ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw bit for SPI_MEM_AXI_RADDR_ERR_INT interrupt. 1: Triggered when AXI read
address is invalid by compared to MMU configuration. 0: Others..*/
#define SPI_MEM_AXI_RADDR_ERR_INT_RAW    (BIT(7))
#define SPI_MEM_AXI_RADDR_ERR_INT_RAW_M  (BIT(7))
#define SPI_MEM_AXI_RADDR_ERR_INT_RAW_V  0x1
#define SPI_MEM_AXI_RADDR_ERR_INT_RAW_S  7
/* SPI_MEM_PMS_REJECT_INT_RAW : R/WTC/SS ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw bit for SPI_MEM_PMS_REJECT_INT interrupt. 1: Triggered when SPI1 access
is rejected. 0: Others..*/
#define SPI_MEM_PMS_REJECT_INT_RAW    (BIT(6))
#define SPI_MEM_PMS_REJECT_INT_RAW_M  (BIT(6))
#define SPI_MEM_PMS_REJECT_INT_RAW_V  0x1
#define SPI_MEM_PMS_REJECT_INT_RAW_S  6
/* SPI_MEM_ECC_ERR_INT_RAW : HRO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw bit for SPI_MEM_ECC_ERR_INT interrupt. When SPI_FMEM_ECC_ERR_INT_EN is s
et and  SPI_SMEM_ECC_ERR_INT_EN is cleared, this bit is triggered when the error
 times of SPI0/1 ECC read flash are equal or bigger than SPI_MEM_ECC_ERR_INT_NUM
. When SPI_FMEM_ECC_ERR_INT_EN is cleared and  SPI_SMEM_ECC_ERR_INT_EN is set, t
his bit is triggered when the error times of SPI0/1 ECC read external RAM are eq
ual or bigger than SPI_MEM_ECC_ERR_INT_NUM. When SPI_FMEM_ECC_ERR_INT_EN and  SP
I_SMEM_ECC_ERR_INT_EN are set, this bit is triggered when the total error times
of SPI0/1 ECC read external RAM and flash are equal or bigger than SPI_MEM_ECC_E
RR_INT_NUM. When SPI_FMEM_ECC_ERR_INT_EN and  SPI_SMEM_ECC_ERR_INT_EN are clearer,
this bit will not be triggered..*/
#define SPI_MEM_ECC_ERR_INT_RAW    (BIT(5))
#define SPI_MEM_ECC_ERR_INT_RAW_M  (BIT(5))
#define SPI_MEM_ECC_ERR_INT_RAW_V  0x1
#define SPI_MEM_ECC_ERR_INT_RAW_S  5
/* SPI_MEM_MST_ST_END_INT_RAW : R/WTC/SS ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw bit for SPI_MEM_MST_ST_END_INT interrupt. 1: Triggered when spi0_mst_st
is changed from non idle state to idle state. 0: Others..*/
#define SPI_MEM_MST_ST_END_INT_RAW    (BIT(4))
#define SPI_MEM_MST_ST_END_INT_RAW_M  (BIT(4))
#define SPI_MEM_MST_ST_END_INT_RAW_V  0x1
#define SPI_MEM_MST_ST_END_INT_RAW_S  4
/* SPI_MEM_SLV_ST_END_INT_RAW : R/WTC/SS ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw bit for SPI_MEM_SLV_ST_END_INT interrupt. 1: Triggered when spi0_slv_st
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
mand (0x75) is sent and flash is suspended successfully. 0: Others..*/
#define SPI_MEM_PES_END_INT_RAW    (BIT(1))
#define SPI_MEM_PES_END_INT_RAW_M  (BIT(1))
#define SPI_MEM_PES_END_INT_RAW_V  0x1
#define SPI_MEM_PES_END_INT_RAW_S  1
/* SPI_MEM_PER_END_INT_RAW : R/WTC/SS ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw bit for SPI_MEM_PER_END_INT interrupt. 1: Triggered when Auto Resume com
mand (0x7A) is sent and flash is resumed successfully. 0: Others..*/
#define SPI_MEM_PER_END_INT_RAW    (BIT(0))
#define SPI_MEM_PER_END_INT_RAW_M  (BIT(0))
#define SPI_MEM_PER_END_INT_RAW_V  0x1
#define SPI_MEM_PER_END_INT_RAW_S  0

#define SPI_MEM_INT_ST_REG(i)          (REG_SPI_MEM_BASE(i) + 0xCC)
/* SPI_MEM_BROWN_OUT_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: The status bit for SPI_MEM_BROWN_OUT_INT interrupt..*/
#define SPI_MEM_BROWN_OUT_INT_ST    (BIT(10))
#define SPI_MEM_BROWN_OUT_INT_ST_M  (BIT(10))
#define SPI_MEM_BROWN_OUT_INT_ST_V  0x1
#define SPI_MEM_BROWN_OUT_INT_ST_S  10
/* SPI_MEM_AXI_WADDR_ERR_INT_ST : HRO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The enable bit for SPI_MEM_AXI_WADDR_ERR_INT interrupt..*/
#define SPI_MEM_AXI_WADDR_ERR_INT_ST    (BIT(9))
#define SPI_MEM_AXI_WADDR_ERR_INT_ST_M  (BIT(9))
#define SPI_MEM_AXI_WADDR_ERR_INT_ST_V  0x1
#define SPI_MEM_AXI_WADDR_ERR_INT_ST_S  9
/* SPI_MEM_AXI_WR_FLASH_ERR_INT_ST : HRO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The enable bit for SPI_MEM_AXI_WR_FALSH_ERR_INT interrupt..*/
#define SPI_MEM_AXI_WR_FLASH_ERR_INT_ST    (BIT(8))
#define SPI_MEM_AXI_WR_FLASH_ERR_INT_ST_M  (BIT(8))
#define SPI_MEM_AXI_WR_FLASH_ERR_INT_ST_V  0x1
#define SPI_MEM_AXI_WR_FLASH_ERR_INT_ST_S  8
/* SPI_MEM_AXI_RADDR_ERR_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The enable bit for SPI_MEM_AXI_RADDR_ERR_INT interrupt..*/
#define SPI_MEM_AXI_RADDR_ERR_INT_ST    (BIT(7))
#define SPI_MEM_AXI_RADDR_ERR_INT_ST_M  (BIT(7))
#define SPI_MEM_AXI_RADDR_ERR_INT_ST_V  0x1
#define SPI_MEM_AXI_RADDR_ERR_INT_ST_S  7
/* SPI_MEM_PMS_REJECT_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The status bit for SPI_MEM_PMS_REJECT_INT interrupt..*/
#define SPI_MEM_PMS_REJECT_INT_ST    (BIT(6))
#define SPI_MEM_PMS_REJECT_INT_ST_M  (BIT(6))
#define SPI_MEM_PMS_REJECT_INT_ST_V  0x1
#define SPI_MEM_PMS_REJECT_INT_ST_S  6
/* SPI_MEM_ECC_ERR_INT_ST : HRO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The status bit for SPI_MEM_ECC_ERR_INT interrupt..*/
#define SPI_MEM_ECC_ERR_INT_ST    (BIT(5))
#define SPI_MEM_ECC_ERR_INT_ST_M  (BIT(5))
#define SPI_MEM_ECC_ERR_INT_ST_V  0x1
#define SPI_MEM_ECC_ERR_INT_ST_S  5
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

#define SPI_MEM_DDR_REG(i)          (REG_SPI_MEM_BASE(i) + 0xD4)
/* SPI_MEM_FMEM_HYPERBUS_CA : HRO ;bitpos:[30] ;default: 1'b0 ; */
/*description: Set this bit to enable HyperRAM address out when accesses to flash, which means
ADDR_OUT[31:0] = {spi_usr_addr_value[19:4], 13'd0, spi_usr_addr_value[3:1]}..*/
#define SPI_MEM_FMEM_HYPERBUS_CA    (BIT(30))
#define SPI_MEM_FMEM_HYPERBUS_CA_M  (BIT(30))
#define SPI_MEM_FMEM_HYPERBUS_CA_V  0x1
#define SPI_MEM_FMEM_HYPERBUS_CA_S  30
/* SPI_MEM_FMEM_OCTA_RAM_ADDR : HRO ;bitpos:[29] ;default: 1'b0 ; */
/*description: Set this bit to enable octa_ram address out when accesses to flash, which means
ADDR_OUT[31:0] = {spi_usr_addr_value[25:4], 6'd0, spi_usr_addr_value[3:1], 1'b0}
..*/
#define SPI_MEM_FMEM_OCTA_RAM_ADDR    (BIT(29))
#define SPI_MEM_FMEM_OCTA_RAM_ADDR_M  (BIT(29))
#define SPI_MEM_FMEM_OCTA_RAM_ADDR_V  0x1
#define SPI_MEM_FMEM_OCTA_RAM_ADDR_S  29
/* SPI_MEM_FMEM_CLK_DIFF_INV : HRO ;bitpos:[28] ;default: 1'b0 ; */
/*description: Set this bit to invert SPI_DIFF when accesses to flash. ..*/
#define SPI_MEM_FMEM_CLK_DIFF_INV    (BIT(28))
#define SPI_MEM_FMEM_CLK_DIFF_INV_M  (BIT(28))
#define SPI_MEM_FMEM_CLK_DIFF_INV_V  0x1
#define SPI_MEM_FMEM_CLK_DIFF_INV_S  28
/* SPI_MEM_FMEM_HYPERBUS_DUMMY_2X : HRO ;bitpos:[27] ;default: 1'b0 ; */
/*description: Set this bit to enable the vary dummy function in SPI HyperBus mode, when SPI0 a
ccesses flash or SPI1 accesses flash or sram..*/
#define SPI_MEM_FMEM_HYPERBUS_DUMMY_2X    (BIT(27))
#define SPI_MEM_FMEM_HYPERBUS_DUMMY_2X_M  (BIT(27))
#define SPI_MEM_FMEM_HYPERBUS_DUMMY_2X_V  0x1
#define SPI_MEM_FMEM_HYPERBUS_DUMMY_2X_S  27
/* SPI_MEM_FMEM_DQS_CA_IN : HRO ;bitpos:[26] ;default: 1'b0 ; */
/*description: Set this bit to enable the input of SPI_DQS signal in SPI phases of CMD and ADDR
..*/
#define SPI_MEM_FMEM_DQS_CA_IN    (BIT(26))
#define SPI_MEM_FMEM_DQS_CA_IN_M  (BIT(26))
#define SPI_MEM_FMEM_DQS_CA_IN_V  0x1
#define SPI_MEM_FMEM_DQS_CA_IN_S  26
/* SPI_MEM_FMEM_CLK_DIFF_EN : HRO ;bitpos:[24] ;default: 1'b0 ; */
/*description: Set this bit to enable the differential SPI_CLK#..*/
#define SPI_MEM_FMEM_CLK_DIFF_EN    (BIT(24))
#define SPI_MEM_FMEM_CLK_DIFF_EN_M  (BIT(24))
#define SPI_MEM_FMEM_CLK_DIFF_EN_V  0x1
#define SPI_MEM_FMEM_CLK_DIFF_EN_S  24
/* SPI_MEM_FMEM_DDR_DQS_LOOP : HRO ;bitpos:[21] ;default: 1'b0 ; */
/*description: 1: Do not need the input of SPI_DQS signal, SPI0 starts to receive data when spi
0_slv_st is in SPI_MEM_DIN state. It is used when there is no SPI_DQS signal or
SPI_DQS signal is not stable. 0: SPI0 starts to store data at the positive and n
egative edge of SPI_DQS..*/
#define SPI_MEM_FMEM_DDR_DQS_LOOP    (BIT(21))
#define SPI_MEM_FMEM_DDR_DQS_LOOP_M  (BIT(21))
#define SPI_MEM_FMEM_DDR_DQS_LOOP_V  0x1
#define SPI_MEM_FMEM_DDR_DQS_LOOP_S  21
/* SPI_MEM_FMEM_USR_DDR_DQS_THD : HRO ;bitpos:[20:14] ;default: 7'b0 ; */
/*description: The delay number of data strobe which from memory based on SPI clock..*/
#define SPI_MEM_FMEM_USR_DDR_DQS_THD    0x0000007F
#define SPI_MEM_FMEM_USR_DDR_DQS_THD_M  ((SPI_MEM_FMEM_USR_DDR_DQS_THD_V)<<(SPI_MEM_FMEM_USR_DDR_DQS_THD_S))
#define SPI_MEM_FMEM_USR_DDR_DQS_THD_V  0x7F
#define SPI_MEM_FMEM_USR_DDR_DQS_THD_S  14
/* SPI_MEM_FMEM_RX_DDR_MSK_EN : HRO ;bitpos:[13] ;default: 1'h1 ; */
/*description: Set this bit to mask the first or the last byte in SPI0 ECC DDR read mode, when
accesses to flash..*/
#define SPI_MEM_FMEM_RX_DDR_MSK_EN    (BIT(13))
#define SPI_MEM_FMEM_RX_DDR_MSK_EN_M  (BIT(13))
#define SPI_MEM_FMEM_RX_DDR_MSK_EN_V  0x1
#define SPI_MEM_FMEM_RX_DDR_MSK_EN_S  13
/* SPI_MEM_FMEM_TX_DDR_MSK_EN : HRO ;bitpos:[12] ;default: 1'h1 ; */
/*description: Set this bit to mask the first or the last byte in SPI0 ECC DDR write mode, when
 accesses to flash..*/
#define SPI_MEM_FMEM_TX_DDR_MSK_EN    (BIT(12))
#define SPI_MEM_FMEM_TX_DDR_MSK_EN_M  (BIT(12))
#define SPI_MEM_FMEM_TX_DDR_MSK_EN_V  0x1
#define SPI_MEM_FMEM_TX_DDR_MSK_EN_S  12
/* SPI_MEM_FMEM_OUTMINBYTELEN : HRO ;bitpos:[11:5] ;default: 7'b1 ; */
/*description: It is the minimum output data length in the panda device..*/
#define SPI_MEM_FMEM_OUTMINBYTELEN    0x0000007F
#define SPI_MEM_FMEM_OUTMINBYTELEN_M  ((SPI_MEM_FMEM_OUTMINBYTELEN_V)<<(SPI_MEM_FMEM_OUTMINBYTELEN_S))
#define SPI_MEM_FMEM_OUTMINBYTELEN_V  0x7F
#define SPI_MEM_FMEM_OUTMINBYTELEN_S  5
/* SPI_MEM_FMEM_DDR_CMD_DIS : HRO ;bitpos:[4] ;default: 1'b0 ; */
/*description: the bit is used to disable dual edge in command phase when DDR mode..*/
#define SPI_MEM_FMEM_DDR_CMD_DIS    (BIT(4))
#define SPI_MEM_FMEM_DDR_CMD_DIS_M  (BIT(4))
#define SPI_MEM_FMEM_DDR_CMD_DIS_V  0x1
#define SPI_MEM_FMEM_DDR_CMD_DIS_S  4
/* SPI_MEM_FMEM_DDR_WDAT_SWP : HRO ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set the bit to reorder tx data of the word in spi DDR mode..*/
#define SPI_MEM_FMEM_DDR_WDAT_SWP    (BIT(3))
#define SPI_MEM_FMEM_DDR_WDAT_SWP_M  (BIT(3))
#define SPI_MEM_FMEM_DDR_WDAT_SWP_V  0x1
#define SPI_MEM_FMEM_DDR_WDAT_SWP_S  3
/* SPI_MEM_FMEM_DDR_RDAT_SWP : HRO ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set the bit to reorder rx data of the word in spi DDR mode..*/
#define SPI_MEM_FMEM_DDR_RDAT_SWP    (BIT(2))
#define SPI_MEM_FMEM_DDR_RDAT_SWP_M  (BIT(2))
#define SPI_MEM_FMEM_DDR_RDAT_SWP_V  0x1
#define SPI_MEM_FMEM_DDR_RDAT_SWP_S  2
/* SPI_MEM_FMEM_VAR_DUMMY : HRO ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set the bit to enable variable dummy cycle in spi DDR mode..*/
#define SPI_MEM_FMEM_VAR_DUMMY    (BIT(1))
#define SPI_MEM_FMEM_VAR_DUMMY_M  (BIT(1))
#define SPI_MEM_FMEM_VAR_DUMMY_V  0x1
#define SPI_MEM_FMEM_VAR_DUMMY_S  1
/* SPI_MEM_FMEM_DDR_EN : HRO ;bitpos:[0] ;default: 1'b0 ; */
/*description: 1: in DDR mode,  0 in SDR mode.*/
#define SPI_MEM_FMEM_DDR_EN    (BIT(0))
#define SPI_MEM_FMEM_DDR_EN_M  (BIT(0))
#define SPI_MEM_FMEM_DDR_EN_V  0x1
#define SPI_MEM_FMEM_DDR_EN_S  0

#define SPI_MEM_SPI_SMEM_DDR_REG(i)          (REG_SPI_MEM_BASE(i) + 0xD8)
/* SPI_MEM_SMEM_HYPERBUS_CA : HRO ;bitpos:[30] ;default: 1'b0 ; */
/*description: Set this bit to enable HyperRAM address out when accesses to external RAM, which
 means ADDR_OUT[31:0] = {spi_usr_addr_value[19:4], 13'd0, spi_usr_addr_value[3:1
]}..*/
#define SPI_MEM_SMEM_HYPERBUS_CA    (BIT(30))
#define SPI_MEM_SMEM_HYPERBUS_CA_M  (BIT(30))
#define SPI_MEM_SMEM_HYPERBUS_CA_V  0x1
#define SPI_MEM_SMEM_HYPERBUS_CA_S  30
/* SPI_MEM_SMEM_OCTA_RAM_ADDR : HRO ;bitpos:[29] ;default: 1'b0 ; */
/*description: Set this bit to enable octa_ram address out when accesses to external RAM, which
 means ADDR_OUT[31:0] = {spi_usr_addr_value[25:4], 6'd0, spi_usr_addr_value[3:1]
, 1'b0}..*/
#define SPI_MEM_SMEM_OCTA_RAM_ADDR    (BIT(29))
#define SPI_MEM_SMEM_OCTA_RAM_ADDR_M  (BIT(29))
#define SPI_MEM_SMEM_OCTA_RAM_ADDR_V  0x1
#define SPI_MEM_SMEM_OCTA_RAM_ADDR_S  29
/* SPI_MEM_SMEM_CLK_DIFF_INV : HRO ;bitpos:[28] ;default: 1'b0 ; */
/*description: Set this bit to invert SPI_DIFF when accesses to external RAM. ..*/
#define SPI_MEM_SMEM_CLK_DIFF_INV    (BIT(28))
#define SPI_MEM_SMEM_CLK_DIFF_INV_M  (BIT(28))
#define SPI_MEM_SMEM_CLK_DIFF_INV_V  0x1
#define SPI_MEM_SMEM_CLK_DIFF_INV_S  28
/* SPI_MEM_SMEM_HYPERBUS_DUMMY_2X : HRO ;bitpos:[27] ;default: 1'b0 ; */
/*description: Set this bit to enable the vary dummy function in SPI HyperBus mode, when SPI0 a
ccesses flash or SPI1 accesses flash or sram..*/
#define SPI_MEM_SMEM_HYPERBUS_DUMMY_2X    (BIT(27))
#define SPI_MEM_SMEM_HYPERBUS_DUMMY_2X_M  (BIT(27))
#define SPI_MEM_SMEM_HYPERBUS_DUMMY_2X_V  0x1
#define SPI_MEM_SMEM_HYPERBUS_DUMMY_2X_S  27
/* SPI_MEM_SMEM_DQS_CA_IN : HRO ;bitpos:[26] ;default: 1'b0 ; */
/*description: Set this bit to enable the input of SPI_DQS signal in SPI phases of CMD and ADDR
..*/
#define SPI_MEM_SMEM_DQS_CA_IN    (BIT(26))
#define SPI_MEM_SMEM_DQS_CA_IN_M  (BIT(26))
#define SPI_MEM_SMEM_DQS_CA_IN_V  0x1
#define SPI_MEM_SMEM_DQS_CA_IN_S  26
/* SPI_MEM_SMEM_CLK_DIFF_EN : HRO ;bitpos:[24] ;default: 1'b0 ; */
/*description: Set this bit to enable the differential SPI_CLK#..*/
#define SPI_MEM_SMEM_CLK_DIFF_EN    (BIT(24))
#define SPI_MEM_SMEM_CLK_DIFF_EN_M  (BIT(24))
#define SPI_MEM_SMEM_CLK_DIFF_EN_V  0x1
#define SPI_MEM_SMEM_CLK_DIFF_EN_S  24
/* SPI_MEM_SMEM_DDR_DQS_LOOP : HRO ;bitpos:[21] ;default: 1'b0 ; */
/*description: 1: Do not need the input of SPI_DQS signal, SPI0 starts to receive data when spi
0_slv_st is in SPI_MEM_DIN state. It is used when there is no SPI_DQS signal or
SPI_DQS signal is not stable. 0: SPI0 starts to store data at the positive and n
egative edge of SPI_DQS..*/
#define SPI_MEM_SMEM_DDR_DQS_LOOP    (BIT(21))
#define SPI_MEM_SMEM_DDR_DQS_LOOP_M  (BIT(21))
#define SPI_MEM_SMEM_DDR_DQS_LOOP_V  0x1
#define SPI_MEM_SMEM_DDR_DQS_LOOP_S  21
/* SPI_MEM_SMEM_USR_DDR_DQS_THD : HRO ;bitpos:[20:14] ;default: 7'b0 ; */
/*description: The delay number of data strobe which from memory based on SPI clock..*/
#define SPI_MEM_SMEM_USR_DDR_DQS_THD    0x0000007F
#define SPI_MEM_SMEM_USR_DDR_DQS_THD_M  ((SPI_MEM_SMEM_USR_DDR_DQS_THD_V)<<(SPI_MEM_SMEM_USR_DDR_DQS_THD_S))
#define SPI_MEM_SMEM_USR_DDR_DQS_THD_V  0x7F
#define SPI_MEM_SMEM_USR_DDR_DQS_THD_S  14
/* SPI_MEM_SMEM_RX_DDR_MSK_EN : HRO ;bitpos:[13] ;default: 1'h1 ; */
/*description: Set this bit to mask the first or the last byte in SPI0 ECC DDR read mode, when
accesses to external RAM..*/
#define SPI_MEM_SMEM_RX_DDR_MSK_EN    (BIT(13))
#define SPI_MEM_SMEM_RX_DDR_MSK_EN_M  (BIT(13))
#define SPI_MEM_SMEM_RX_DDR_MSK_EN_V  0x1
#define SPI_MEM_SMEM_RX_DDR_MSK_EN_S  13
/* SPI_MEM_SMEM_TX_DDR_MSK_EN : HRO ;bitpos:[12] ;default: 1'h1 ; */
/*description: Set this bit to mask the first or the last byte in SPI0 ECC DDR write mode, when
 accesses to external RAM..*/
#define SPI_MEM_SMEM_TX_DDR_MSK_EN    (BIT(12))
#define SPI_MEM_SMEM_TX_DDR_MSK_EN_M  (BIT(12))
#define SPI_MEM_SMEM_TX_DDR_MSK_EN_V  0x1
#define SPI_MEM_SMEM_TX_DDR_MSK_EN_S  12
/* SPI_MEM_SMEM_OUTMINBYTELEN : HRO ;bitpos:[11:5] ;default: 7'b1 ; */
/*description: It is the minimum output data length in the DDR psram..*/
#define SPI_MEM_SMEM_OUTMINBYTELEN    0x0000007F
#define SPI_MEM_SMEM_OUTMINBYTELEN_M  ((SPI_MEM_SMEM_OUTMINBYTELEN_V)<<(SPI_MEM_SMEM_OUTMINBYTELEN_S))
#define SPI_MEM_SMEM_OUTMINBYTELEN_V  0x7F
#define SPI_MEM_SMEM_OUTMINBYTELEN_S  5
/* SPI_MEM_SMEM_DDR_CMD_DIS : HRO ;bitpos:[4] ;default: 1'b0 ; */
/*description: the bit is used to disable dual edge in command phase when DDR mode..*/
#define SPI_MEM_SMEM_DDR_CMD_DIS    (BIT(4))
#define SPI_MEM_SMEM_DDR_CMD_DIS_M  (BIT(4))
#define SPI_MEM_SMEM_DDR_CMD_DIS_V  0x1
#define SPI_MEM_SMEM_DDR_CMD_DIS_S  4
/* SPI_MEM_SMEM_DDR_WDAT_SWP : HRO ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set the bit to reorder tx data of the word in spi DDR mode..*/
#define SPI_MEM_SMEM_DDR_WDAT_SWP    (BIT(3))
#define SPI_MEM_SMEM_DDR_WDAT_SWP_M  (BIT(3))
#define SPI_MEM_SMEM_DDR_WDAT_SWP_V  0x1
#define SPI_MEM_SMEM_DDR_WDAT_SWP_S  3
/* SPI_MEM_SMEM_DDR_RDAT_SWP : HRO ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set the bit to reorder rx data of the word in spi DDR mode..*/
#define SPI_MEM_SMEM_DDR_RDAT_SWP    (BIT(2))
#define SPI_MEM_SMEM_DDR_RDAT_SWP_M  (BIT(2))
#define SPI_MEM_SMEM_DDR_RDAT_SWP_V  0x1
#define SPI_MEM_SMEM_DDR_RDAT_SWP_S  2
/* SPI_MEM_SMEM_VAR_DUMMY : HRO ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set the bit to enable variable dummy cycle in spi DDR mode..*/
#define SPI_MEM_SMEM_VAR_DUMMY    (BIT(1))
#define SPI_MEM_SMEM_VAR_DUMMY_M  (BIT(1))
#define SPI_MEM_SMEM_VAR_DUMMY_V  0x1
#define SPI_MEM_SMEM_VAR_DUMMY_S  1
/* SPI_MEM_SMEM_DDR_EN : HRO ;bitpos:[0] ;default: 1'b0 ; */
/*description: 1: in DDR mode,  0 in SDR mode.*/
#define SPI_MEM_SMEM_DDR_EN    (BIT(0))
#define SPI_MEM_SMEM_DDR_EN_M  (BIT(0))
#define SPI_MEM_SMEM_DDR_EN_V  0x1
#define SPI_MEM_SMEM_DDR_EN_S  0

#define SPI_MEM_SPI_FMEM_PMS0_ATTR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x100)
/* SPI_MEM_FMEM_PMS0_ECC : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: SPI1 flash ACE section $n ECC mode, 1: enable ECC mode. 0: Disable it. The flash
 ACE section $n is configured by registers SPI_FMEM_PMS$n_ADDR_REG and SPI_FMEM_
PMS$n_SIZE_REG..*/
#define SPI_MEM_FMEM_PMS0_ECC    (BIT(2))
#define SPI_MEM_FMEM_PMS0_ECC_M  (BIT(2))
#define SPI_MEM_FMEM_PMS0_ECC_V  0x1
#define SPI_MEM_FMEM_PMS0_ECC_S  2
/* SPI_MEM_FMEM_PMS0_WR_ATTR : R/W ;bitpos:[1] ;default: 1'h1 ; */
/*description: 1: SPI1 flash ACE section $n write accessible. 0: Not allowed..*/
#define SPI_MEM_FMEM_PMS0_WR_ATTR    (BIT(1))
#define SPI_MEM_FMEM_PMS0_WR_ATTR_M  (BIT(1))
#define SPI_MEM_FMEM_PMS0_WR_ATTR_V  0x1
#define SPI_MEM_FMEM_PMS0_WR_ATTR_S  1
/* SPI_MEM_FMEM_PMS0_RD_ATTR : R/W ;bitpos:[0] ;default: 1'h1 ; */
/*description: 1: SPI1 flash ACE section $n read accessible. 0: Not allowed..*/
#define SPI_MEM_FMEM_PMS0_RD_ATTR    (BIT(0))
#define SPI_MEM_FMEM_PMS0_RD_ATTR_M  (BIT(0))
#define SPI_MEM_FMEM_PMS0_RD_ATTR_V  0x1
#define SPI_MEM_FMEM_PMS0_RD_ATTR_S  0

#define SPI_MEM_SPI_FMEM_PMS1_ATTR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x104)
/* SPI_MEM_FMEM_PMS1_ECC : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: SPI1 flash ACE section $n ECC mode, 1: enable ECC mode. 0: Disable it. The flash
 ACE section $n is configured by registers SPI_FMEM_PMS$n_ADDR_REG and SPI_FMEM_
PMS$n_SIZE_REG..*/
#define SPI_MEM_FMEM_PMS1_ECC    (BIT(2))
#define SPI_MEM_FMEM_PMS1_ECC_M  (BIT(2))
#define SPI_MEM_FMEM_PMS1_ECC_V  0x1
#define SPI_MEM_FMEM_PMS1_ECC_S  2
/* SPI_MEM_FMEM_PMS1_WR_ATTR : R/W ;bitpos:[1] ;default: 1'h1 ; */
/*description: 1: SPI1 flash ACE section $n write accessible. 0: Not allowed..*/
#define SPI_MEM_FMEM_PMS1_WR_ATTR    (BIT(1))
#define SPI_MEM_FMEM_PMS1_WR_ATTR_M  (BIT(1))
#define SPI_MEM_FMEM_PMS1_WR_ATTR_V  0x1
#define SPI_MEM_FMEM_PMS1_WR_ATTR_S  1
/* SPI_MEM_FMEM_PMS1_RD_ATTR : R/W ;bitpos:[0] ;default: 1'h1 ; */
/*description: 1: SPI1 flash ACE section $n read accessible. 0: Not allowed..*/
#define SPI_MEM_FMEM_PMS1_RD_ATTR    (BIT(0))
#define SPI_MEM_FMEM_PMS1_RD_ATTR_M  (BIT(0))
#define SPI_MEM_FMEM_PMS1_RD_ATTR_V  0x1
#define SPI_MEM_FMEM_PMS1_RD_ATTR_S  0

#define SPI_MEM_SPI_FMEM_PMS2_ATTR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x108)
/* SPI_MEM_FMEM_PMS2_ECC : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: SPI1 flash ACE section $n ECC mode, 1: enable ECC mode. 0: Disable it. The flash
 ACE section $n is configured by registers SPI_FMEM_PMS$n_ADDR_REG and SPI_FMEM_
PMS$n_SIZE_REG..*/
#define SPI_MEM_FMEM_PMS2_ECC    (BIT(2))
#define SPI_MEM_FMEM_PMS2_ECC_M  (BIT(2))
#define SPI_MEM_FMEM_PMS2_ECC_V  0x1
#define SPI_MEM_FMEM_PMS2_ECC_S  2
/* SPI_MEM_FMEM_PMS2_WR_ATTR : R/W ;bitpos:[1] ;default: 1'h1 ; */
/*description: 1: SPI1 flash ACE section $n write accessible. 0: Not allowed..*/
#define SPI_MEM_FMEM_PMS2_WR_ATTR    (BIT(1))
#define SPI_MEM_FMEM_PMS2_WR_ATTR_M  (BIT(1))
#define SPI_MEM_FMEM_PMS2_WR_ATTR_V  0x1
#define SPI_MEM_FMEM_PMS2_WR_ATTR_S  1
/* SPI_MEM_FMEM_PMS2_RD_ATTR : R/W ;bitpos:[0] ;default: 1'h1 ; */
/*description: 1: SPI1 flash ACE section $n read accessible. 0: Not allowed..*/
#define SPI_MEM_FMEM_PMS2_RD_ATTR    (BIT(0))
#define SPI_MEM_FMEM_PMS2_RD_ATTR_M  (BIT(0))
#define SPI_MEM_FMEM_PMS2_RD_ATTR_V  0x1
#define SPI_MEM_FMEM_PMS2_RD_ATTR_S  0

#define SPI_MEM_SPI_FMEM_PMS3_ATTR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x10C)
/* SPI_MEM_FMEM_PMS3_ECC : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: SPI1 flash ACE section $n ECC mode, 1: enable ECC mode. 0: Disable it. The flash
 ACE section $n is configured by registers SPI_FMEM_PMS$n_ADDR_REG and SPI_FMEM_
PMS$n_SIZE_REG..*/
#define SPI_MEM_FMEM_PMS3_ECC    (BIT(2))
#define SPI_MEM_FMEM_PMS3_ECC_M  (BIT(2))
#define SPI_MEM_FMEM_PMS3_ECC_V  0x1
#define SPI_MEM_FMEM_PMS3_ECC_S  2
/* SPI_MEM_FMEM_PMS3_WR_ATTR : R/W ;bitpos:[1] ;default: 1'h1 ; */
/*description: 1: SPI1 flash ACE section $n write accessible. 0: Not allowed..*/
#define SPI_MEM_FMEM_PMS3_WR_ATTR    (BIT(1))
#define SPI_MEM_FMEM_PMS3_WR_ATTR_M  (BIT(1))
#define SPI_MEM_FMEM_PMS3_WR_ATTR_V  0x1
#define SPI_MEM_FMEM_PMS3_WR_ATTR_S  1
/* SPI_MEM_FMEM_PMS3_RD_ATTR : R/W ;bitpos:[0] ;default: 1'h1 ; */
/*description: 1: SPI1 flash ACE section $n read accessible. 0: Not allowed..*/
#define SPI_MEM_FMEM_PMS3_RD_ATTR    (BIT(0))
#define SPI_MEM_FMEM_PMS3_RD_ATTR_M  (BIT(0))
#define SPI_MEM_FMEM_PMS3_RD_ATTR_V  0x1
#define SPI_MEM_FMEM_PMS3_RD_ATTR_S  0

#define SPI_MEM_SPI_FMEM_PMS0_ADDR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x110)
/* SPI_MEM_FMEM_PMS0_ADDR_S : R/W ;bitpos:[25:0] ;default: 26'h0 ; */
/*description: SPI1 flash ACE section $n start address value.*/
#define SPI_MEM_FMEM_PMS0_ADDR_S    0x03FFFFFF
#define SPI_MEM_FMEM_PMS0_ADDR_S_M  ((SPI_MEM_FMEM_PMS0_ADDR_S_V)<<(SPI_MEM_FMEM_PMS0_ADDR_S_S))
#define SPI_MEM_FMEM_PMS0_ADDR_S_V  0x3FFFFFF
#define SPI_MEM_FMEM_PMS0_ADDR_S_S  0

#define SPI_MEM_SPI_FMEM_PMS1_ADDR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x114)
/* SPI_MEM_FMEM_PMS1_ADDR_S : R/W ;bitpos:[25:0] ;default: 26'hffffff ; */
/*description: SPI1 flash ACE section $n start address value.*/
#define SPI_MEM_FMEM_PMS1_ADDR_S    0x03FFFFFF
#define SPI_MEM_FMEM_PMS1_ADDR_S_M  ((SPI_MEM_FMEM_PMS1_ADDR_S_V)<<(SPI_MEM_FMEM_PMS1_ADDR_S_S))
#define SPI_MEM_FMEM_PMS1_ADDR_S_V  0x3FFFFFF
#define SPI_MEM_FMEM_PMS1_ADDR_S_S  0

#define SPI_MEM_SPI_FMEM_PMS2_ADDR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x118)
/* SPI_MEM_FMEM_PMS2_ADDR_S : R/W ;bitpos:[25:0] ;default: 26'h1ffffff ; */
/*description: SPI1 flash ACE section $n start address value.*/
#define SPI_MEM_FMEM_PMS2_ADDR_S    0x03FFFFFF
#define SPI_MEM_FMEM_PMS2_ADDR_S_M  ((SPI_MEM_FMEM_PMS2_ADDR_S_V)<<(SPI_MEM_FMEM_PMS2_ADDR_S_S))
#define SPI_MEM_FMEM_PMS2_ADDR_S_V  0x3FFFFFF
#define SPI_MEM_FMEM_PMS2_ADDR_S_S  0

#define SPI_MEM_SPI_FMEM_PMS3_ADDR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x11C)
/* SPI_MEM_FMEM_PMS3_ADDR_S : R/W ;bitpos:[25:0] ;default: 26'h2ffffff ; */
/*description: SPI1 flash ACE section $n start address value.*/
#define SPI_MEM_FMEM_PMS3_ADDR_S    0x03FFFFFF
#define SPI_MEM_FMEM_PMS3_ADDR_S_M  ((SPI_MEM_FMEM_PMS3_ADDR_S_V)<<(SPI_MEM_FMEM_PMS3_ADDR_S_S))
#define SPI_MEM_FMEM_PMS3_ADDR_S_V  0x3FFFFFF
#define SPI_MEM_FMEM_PMS3_ADDR_S_S  0

#define SPI_MEM_SPI_FMEM_PMS0_SIZE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x120)
/* SPI_MEM_FMEM_PMS0_SIZE : R/W ;bitpos:[13:0] ;default: 14'h1000 ; */
/*description: SPI1 flash ACE section $n address region is (SPI_FMEM_PMS$n_ADDR_S, SPI_FMEM_PMS
$n_ADDR_S + SPI_FMEM_PMS$n_SIZE).*/
#define SPI_MEM_FMEM_PMS0_SIZE    0x00003FFF
#define SPI_MEM_FMEM_PMS0_SIZE_M  ((SPI_MEM_FMEM_PMS0_SIZE_V)<<(SPI_MEM_FMEM_PMS0_SIZE_S))
#define SPI_MEM_FMEM_PMS0_SIZE_V  0x3FFF
#define SPI_MEM_FMEM_PMS0_SIZE_S  0

#define SPI_MEM_SPI_FMEM_PMS1_SIZE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x124)
/* SPI_MEM_FMEM_PMS1_SIZE : R/W ;bitpos:[13:0] ;default: 14'h1000 ; */
/*description: SPI1 flash ACE section $n address region is (SPI_FMEM_PMS$n_ADDR_S, SPI_FMEM_PMS
$n_ADDR_S + SPI_FMEM_PMS$n_SIZE).*/
#define SPI_MEM_FMEM_PMS1_SIZE    0x00003FFF
#define SPI_MEM_FMEM_PMS1_SIZE_M  ((SPI_MEM_FMEM_PMS1_SIZE_V)<<(SPI_MEM_FMEM_PMS1_SIZE_S))
#define SPI_MEM_FMEM_PMS1_SIZE_V  0x3FFF
#define SPI_MEM_FMEM_PMS1_SIZE_S  0

#define SPI_MEM_SPI_FMEM_PMS2_SIZE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x128)
/* SPI_MEM_FMEM_PMS2_SIZE : R/W ;bitpos:[13:0] ;default: 14'h1000 ; */
/*description: SPI1 flash ACE section $n address region is (SPI_FMEM_PMS$n_ADDR_S, SPI_FMEM_PMS
$n_ADDR_S + SPI_FMEM_PMS$n_SIZE).*/
#define SPI_MEM_FMEM_PMS2_SIZE    0x00003FFF
#define SPI_MEM_FMEM_PMS2_SIZE_M  ((SPI_MEM_FMEM_PMS2_SIZE_V)<<(SPI_MEM_FMEM_PMS2_SIZE_S))
#define SPI_MEM_FMEM_PMS2_SIZE_V  0x3FFF
#define SPI_MEM_FMEM_PMS2_SIZE_S  0

#define SPI_MEM_SPI_FMEM_PMS3_SIZE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x12C)
/* SPI_MEM_FMEM_PMS3_SIZE : R/W ;bitpos:[13:0] ;default: 14'h1000 ; */
/*description: SPI1 flash ACE section $n address region is (SPI_FMEM_PMS$n_ADDR_S, SPI_FMEM_PMS
$n_ADDR_S + SPI_FMEM_PMS$n_SIZE).*/
#define SPI_MEM_FMEM_PMS3_SIZE    0x00003FFF
#define SPI_MEM_FMEM_PMS3_SIZE_M  ((SPI_MEM_FMEM_PMS3_SIZE_V)<<(SPI_MEM_FMEM_PMS3_SIZE_S))
#define SPI_MEM_FMEM_PMS3_SIZE_V  0x3FFF
#define SPI_MEM_FMEM_PMS3_SIZE_S  0

#define SPI_MEM_SPI_SMEM_PMS0_ATTR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x130)
/* SPI_MEM_SMEM_PMS0_ECC : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: SPI1 external RAM ACE section $n ECC mode, 1: enable ECC mode. 0: Disable it. Th
e external RAM ACE section $n is configured by registers SPI_SMEM_PMS$n_ADDR_REG
 and SPI_SMEM_PMS$n_SIZE_REG..*/
#define SPI_MEM_SMEM_PMS0_ECC    (BIT(2))
#define SPI_MEM_SMEM_PMS0_ECC_M  (BIT(2))
#define SPI_MEM_SMEM_PMS0_ECC_V  0x1
#define SPI_MEM_SMEM_PMS0_ECC_S  2
/* SPI_MEM_SMEM_PMS0_WR_ATTR : R/W ;bitpos:[1] ;default: 1'h1 ; */
/*description: 1: SPI1 external RAM ACE section $n write accessible. 0: Not allowed..*/
#define SPI_MEM_SMEM_PMS0_WR_ATTR    (BIT(1))
#define SPI_MEM_SMEM_PMS0_WR_ATTR_M  (BIT(1))
#define SPI_MEM_SMEM_PMS0_WR_ATTR_V  0x1
#define SPI_MEM_SMEM_PMS0_WR_ATTR_S  1
/* SPI_MEM_SMEM_PMS0_RD_ATTR : R/W ;bitpos:[0] ;default: 1'h1 ; */
/*description: 1: SPI1 external RAM ACE section $n read accessible. 0: Not allowed..*/
#define SPI_MEM_SMEM_PMS0_RD_ATTR    (BIT(0))
#define SPI_MEM_SMEM_PMS0_RD_ATTR_M  (BIT(0))
#define SPI_MEM_SMEM_PMS0_RD_ATTR_V  0x1
#define SPI_MEM_SMEM_PMS0_RD_ATTR_S  0

#define SPI_MEM_SPI_SMEM_PMS1_ATTR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x134)
/* SPI_MEM_SMEM_PMS1_ECC : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: SPI1 external RAM ACE section $n ECC mode, 1: enable ECC mode. 0: Disable it. Th
e external RAM ACE section $n is configured by registers SPI_SMEM_PMS$n_ADDR_REG
 and SPI_SMEM_PMS$n_SIZE_REG..*/
#define SPI_MEM_SMEM_PMS1_ECC    (BIT(2))
#define SPI_MEM_SMEM_PMS1_ECC_M  (BIT(2))
#define SPI_MEM_SMEM_PMS1_ECC_V  0x1
#define SPI_MEM_SMEM_PMS1_ECC_S  2
/* SPI_MEM_SMEM_PMS1_WR_ATTR : R/W ;bitpos:[1] ;default: 1'h1 ; */
/*description: 1: SPI1 external RAM ACE section $n write accessible. 0: Not allowed..*/
#define SPI_MEM_SMEM_PMS1_WR_ATTR    (BIT(1))
#define SPI_MEM_SMEM_PMS1_WR_ATTR_M  (BIT(1))
#define SPI_MEM_SMEM_PMS1_WR_ATTR_V  0x1
#define SPI_MEM_SMEM_PMS1_WR_ATTR_S  1
/* SPI_MEM_SMEM_PMS1_RD_ATTR : R/W ;bitpos:[0] ;default: 1'h1 ; */
/*description: 1: SPI1 external RAM ACE section $n read accessible. 0: Not allowed..*/
#define SPI_MEM_SMEM_PMS1_RD_ATTR    (BIT(0))
#define SPI_MEM_SMEM_PMS1_RD_ATTR_M  (BIT(0))
#define SPI_MEM_SMEM_PMS1_RD_ATTR_V  0x1
#define SPI_MEM_SMEM_PMS1_RD_ATTR_S  0

#define SPI_MEM_SPI_SMEM_PMS2_ATTR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x138)
/* SPI_MEM_SMEM_PMS2_ECC : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: SPI1 external RAM ACE section $n ECC mode, 1: enable ECC mode. 0: Disable it. Th
e external RAM ACE section $n is configured by registers SPI_SMEM_PMS$n_ADDR_REG
 and SPI_SMEM_PMS$n_SIZE_REG..*/
#define SPI_MEM_SMEM_PMS2_ECC    (BIT(2))
#define SPI_MEM_SMEM_PMS2_ECC_M  (BIT(2))
#define SPI_MEM_SMEM_PMS2_ECC_V  0x1
#define SPI_MEM_SMEM_PMS2_ECC_S  2
/* SPI_MEM_SMEM_PMS2_WR_ATTR : R/W ;bitpos:[1] ;default: 1'h1 ; */
/*description: 1: SPI1 external RAM ACE section $n write accessible. 0: Not allowed..*/
#define SPI_MEM_SMEM_PMS2_WR_ATTR    (BIT(1))
#define SPI_MEM_SMEM_PMS2_WR_ATTR_M  (BIT(1))
#define SPI_MEM_SMEM_PMS2_WR_ATTR_V  0x1
#define SPI_MEM_SMEM_PMS2_WR_ATTR_S  1
/* SPI_MEM_SMEM_PMS2_RD_ATTR : R/W ;bitpos:[0] ;default: 1'h1 ; */
/*description: 1: SPI1 external RAM ACE section $n read accessible. 0: Not allowed..*/
#define SPI_MEM_SMEM_PMS2_RD_ATTR    (BIT(0))
#define SPI_MEM_SMEM_PMS2_RD_ATTR_M  (BIT(0))
#define SPI_MEM_SMEM_PMS2_RD_ATTR_V  0x1
#define SPI_MEM_SMEM_PMS2_RD_ATTR_S  0

#define SPI_MEM_SPI_SMEM_PMS3_ATTR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x13C)
/* SPI_MEM_SMEM_PMS3_ECC : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: SPI1 external RAM ACE section $n ECC mode, 1: enable ECC mode. 0: Disable it. Th
e external RAM ACE section $n is configured by registers SPI_SMEM_PMS$n_ADDR_REG
 and SPI_SMEM_PMS$n_SIZE_REG..*/
#define SPI_MEM_SMEM_PMS3_ECC    (BIT(2))
#define SPI_MEM_SMEM_PMS3_ECC_M  (BIT(2))
#define SPI_MEM_SMEM_PMS3_ECC_V  0x1
#define SPI_MEM_SMEM_PMS3_ECC_S  2
/* SPI_MEM_SMEM_PMS3_WR_ATTR : R/W ;bitpos:[1] ;default: 1'h1 ; */
/*description: 1: SPI1 external RAM ACE section $n write accessible. 0: Not allowed..*/
#define SPI_MEM_SMEM_PMS3_WR_ATTR    (BIT(1))
#define SPI_MEM_SMEM_PMS3_WR_ATTR_M  (BIT(1))
#define SPI_MEM_SMEM_PMS3_WR_ATTR_V  0x1
#define SPI_MEM_SMEM_PMS3_WR_ATTR_S  1
/* SPI_MEM_SMEM_PMS3_RD_ATTR : R/W ;bitpos:[0] ;default: 1'h1 ; */
/*description: 1: SPI1 external RAM ACE section $n read accessible. 0: Not allowed..*/
#define SPI_MEM_SMEM_PMS3_RD_ATTR    (BIT(0))
#define SPI_MEM_SMEM_PMS3_RD_ATTR_M  (BIT(0))
#define SPI_MEM_SMEM_PMS3_RD_ATTR_V  0x1
#define SPI_MEM_SMEM_PMS3_RD_ATTR_S  0

#define SPI_MEM_SPI_SMEM_PMS0_ADDR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x140)
/* SPI_MEM_SMEM_PMS0_ADDR_S : R/W ;bitpos:[25:0] ;default: 26'h0 ; */
/*description: SPI1 external RAM ACE section $n start address value.*/
#define SPI_MEM_SMEM_PMS0_ADDR_S    0x03FFFFFF
#define SPI_MEM_SMEM_PMS0_ADDR_S_M  ((SPI_MEM_SMEM_PMS0_ADDR_S_V)<<(SPI_MEM_SMEM_PMS0_ADDR_S_S))
#define SPI_MEM_SMEM_PMS0_ADDR_S_V  0x3FFFFFF
#define SPI_MEM_SMEM_PMS0_ADDR_S_S  0

#define SPI_MEM_SPI_SMEM_PMS1_ADDR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x144)
/* SPI_MEM_SMEM_PMS1_ADDR_S : R/W ;bitpos:[25:0] ;default: 26'hffffff ; */
/*description: SPI1 external RAM ACE section $n start address value.*/
#define SPI_MEM_SMEM_PMS1_ADDR_S    0x03FFFFFF
#define SPI_MEM_SMEM_PMS1_ADDR_S_M  ((SPI_MEM_SMEM_PMS1_ADDR_S_V)<<(SPI_MEM_SMEM_PMS1_ADDR_S_S))
#define SPI_MEM_SMEM_PMS1_ADDR_S_V  0x3FFFFFF
#define SPI_MEM_SMEM_PMS1_ADDR_S_S  0

#define SPI_MEM_SPI_SMEM_PMS2_ADDR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x148)
/* SPI_MEM_SMEM_PMS2_ADDR_S : R/W ;bitpos:[25:0] ;default: 26'h1ffffff ; */
/*description: SPI1 external RAM ACE section $n start address value.*/
#define SPI_MEM_SMEM_PMS2_ADDR_S    0x03FFFFFF
#define SPI_MEM_SMEM_PMS2_ADDR_S_M  ((SPI_MEM_SMEM_PMS2_ADDR_S_V)<<(SPI_MEM_SMEM_PMS2_ADDR_S_S))
#define SPI_MEM_SMEM_PMS2_ADDR_S_V  0x3FFFFFF
#define SPI_MEM_SMEM_PMS2_ADDR_S_S  0

#define SPI_MEM_SPI_SMEM_PMS3_ADDR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x14C)
/* SPI_MEM_SMEM_PMS3_ADDR_S : R/W ;bitpos:[25:0] ;default: 26'h2ffffff ; */
/*description: SPI1 external RAM ACE section $n start address value.*/
#define SPI_MEM_SMEM_PMS3_ADDR_S    0x03FFFFFF
#define SPI_MEM_SMEM_PMS3_ADDR_S_M  ((SPI_MEM_SMEM_PMS3_ADDR_S_V)<<(SPI_MEM_SMEM_PMS3_ADDR_S_S))
#define SPI_MEM_SMEM_PMS3_ADDR_S_V  0x3FFFFFF
#define SPI_MEM_SMEM_PMS3_ADDR_S_S  0

#define SPI_MEM_SPI_SMEM_PMS0_SIZE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x150)
/* SPI_MEM_SMEM_PMS0_SIZE : R/W ;bitpos:[13:0] ;default: 14'h1000 ; */
/*description: SPI1 external RAM ACE section $n address region is (SPI_SMEM_PMS$n_ADDR_S, SPI_S
MEM_PMS$n_ADDR_S + SPI_SMEM_PMS$n_SIZE).*/
#define SPI_MEM_SMEM_PMS0_SIZE    0x00003FFF
#define SPI_MEM_SMEM_PMS0_SIZE_M  ((SPI_MEM_SMEM_PMS0_SIZE_V)<<(SPI_MEM_SMEM_PMS0_SIZE_S))
#define SPI_MEM_SMEM_PMS0_SIZE_V  0x3FFF
#define SPI_MEM_SMEM_PMS0_SIZE_S  0

#define SPI_MEM_SPI_SMEM_PMS1_SIZE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x154)
/* SPI_MEM_SMEM_PMS1_SIZE : R/W ;bitpos:[13:0] ;default: 14'h1000 ; */
/*description: SPI1 external RAM ACE section $n address region is (SPI_SMEM_PMS$n_ADDR_S, SPI_S
MEM_PMS$n_ADDR_S + SPI_SMEM_PMS$n_SIZE).*/
#define SPI_MEM_SMEM_PMS1_SIZE    0x00003FFF
#define SPI_MEM_SMEM_PMS1_SIZE_M  ((SPI_MEM_SMEM_PMS1_SIZE_V)<<(SPI_MEM_SMEM_PMS1_SIZE_S))
#define SPI_MEM_SMEM_PMS1_SIZE_V  0x3FFF
#define SPI_MEM_SMEM_PMS1_SIZE_S  0

#define SPI_MEM_SPI_SMEM_PMS2_SIZE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x158)
/* SPI_MEM_SMEM_PMS2_SIZE : R/W ;bitpos:[13:0] ;default: 14'h1000 ; */
/*description: SPI1 external RAM ACE section $n address region is (SPI_SMEM_PMS$n_ADDR_S, SPI_S
MEM_PMS$n_ADDR_S + SPI_SMEM_PMS$n_SIZE).*/
#define SPI_MEM_SMEM_PMS2_SIZE    0x00003FFF
#define SPI_MEM_SMEM_PMS2_SIZE_M  ((SPI_MEM_SMEM_PMS2_SIZE_V)<<(SPI_MEM_SMEM_PMS2_SIZE_S))
#define SPI_MEM_SMEM_PMS2_SIZE_V  0x3FFF
#define SPI_MEM_SMEM_PMS2_SIZE_S  0

#define SPI_MEM_SPI_SMEM_PMS3_SIZE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x15C)
/* SPI_MEM_SMEM_PMS3_SIZE : R/W ;bitpos:[13:0] ;default: 14'h1000 ; */
/*description: SPI1 external RAM ACE section $n address region is (SPI_SMEM_PMS$n_ADDR_S, SPI_S
MEM_PMS$n_ADDR_S + SPI_SMEM_PMS$n_SIZE).*/
#define SPI_MEM_SMEM_PMS3_SIZE    0x00003FFF
#define SPI_MEM_SMEM_PMS3_SIZE_M  ((SPI_MEM_SMEM_PMS3_SIZE_V)<<(SPI_MEM_SMEM_PMS3_SIZE_S))
#define SPI_MEM_SMEM_PMS3_SIZE_V  0x3FFF
#define SPI_MEM_SMEM_PMS3_SIZE_S  0

#define SPI_MEM_PMS_REJECT_REG(i)          (REG_SPI_MEM_BASE(i) + 0x164)
/* SPI_MEM_PMS_IVD : R/SS/WTC ;bitpos:[31] ;default: 1'h0 ; */
/*description: 1: SPI1 access is rejected because of address multi-hit. 0: No address multi-hit
 error. It is cleared by when  SPI_MEM_PMS_REJECT_INT_CLR bit is set..*/
#define SPI_MEM_PMS_IVD    (BIT(31))
#define SPI_MEM_PMS_IVD_M  (BIT(31))
#define SPI_MEM_PMS_IVD_V  0x1
#define SPI_MEM_PMS_IVD_S  31
/* SPI_MEM_PMS_MULTI_HIT : R/SS/WTC ;bitpos:[30] ;default: 1'b0 ; */
/*description: 1: SPI1 access is rejected because of address miss. 0: No address miss error. It
 is cleared by when  SPI_MEM_PMS_REJECT_INT_CLR bit is set..*/
#define SPI_MEM_PMS_MULTI_HIT    (BIT(30))
#define SPI_MEM_PMS_MULTI_HIT_M  (BIT(30))
#define SPI_MEM_PMS_MULTI_HIT_V  0x1
#define SPI_MEM_PMS_MULTI_HIT_S  30
/* SPI_MEM_PMS_ST : R/SS/WTC ;bitpos:[29] ;default: 1'b0 ; */
/*description: 1: SPI1 read access error. 0: No read access error. It is cleared by when  SPI_M
EM_PMS_REJECT_INT_CLR bit is set..*/
#define SPI_MEM_PMS_ST    (BIT(29))
#define SPI_MEM_PMS_ST_M  (BIT(29))
#define SPI_MEM_PMS_ST_V  0x1
#define SPI_MEM_PMS_ST_S  29
/* SPI_MEM_PMS_LD : R/SS/WTC ;bitpos:[28] ;default: 1'b0 ; */
/*description: 1: SPI1 write access error. 0: No write access error. It is cleared by when  SPI
_MEM_PMS_REJECT_INT_CLR bit is set..*/
#define SPI_MEM_PMS_LD    (BIT(28))
#define SPI_MEM_PMS_LD_M  (BIT(28))
#define SPI_MEM_PMS_LD_V  0x1
#define SPI_MEM_PMS_LD_S  28
/* SPI_MEM_PM_EN : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: Set this bit to enable SPI0/1 transfer permission control function..*/
#define SPI_MEM_PM_EN    (BIT(26))
#define SPI_MEM_PM_EN_M  (BIT(26))
#define SPI_MEM_PM_EN_V  0x1
#define SPI_MEM_PM_EN_S  26
/* SPI_MEM_REJECT_ADDR : R/SS/WTC ;bitpos:[25:0] ;default: 26'h0 ; */
/*description: This bits show the first SPI1 access error address. It is cleared by when  SPI_M
EM_PMS_REJECT_INT_CLR bit is set..*/
#define SPI_MEM_REJECT_ADDR    0x03FFFFFF
#define SPI_MEM_REJECT_ADDR_M  ((SPI_MEM_REJECT_ADDR_V)<<(SPI_MEM_REJECT_ADDR_S))
#define SPI_MEM_REJECT_ADDR_V  0x3FFFFFF
#define SPI_MEM_REJECT_ADDR_S  0

#define SPI_MEM_ECC_CTRL_REG(i)          (REG_SPI_MEM_BASE(i) + 0x168)
/* SPI_MEM_ECC_ERR_BITS : HRO ;bitpos:[31:25] ;default: 7'd0 ; */
/*description: Records the first ECC error bit number in the 16 bytes(From 0~127, corresponding
 to byte 0 bit 0 to byte 15 bit 7).*/
#define SPI_MEM_ECC_ERR_BITS    0x0000007F
#define SPI_MEM_ECC_ERR_BITS_M  ((SPI_MEM_ECC_ERR_BITS_V)<<(SPI_MEM_ECC_ERR_BITS_S))
#define SPI_MEM_ECC_ERR_BITS_V  0x7F
#define SPI_MEM_ECC_ERR_BITS_S  25
/* SPI_MEM_ECC_CONTINUE_RECORD_ERR_EN : HRO ;bitpos:[24] ;default: 1'b1 ; */
/*description: 1: The error information in SPI_MEM_ECC_ERR_BITS and SPI_MEM_ECC_ERR_ADDR is upd
ated when there is an ECC error. 0: SPI_MEM_ECC_ERR_BITS and SPI_MEM_ECC_ERR_ADD
R record the first ECC error information..*/
#define SPI_MEM_ECC_CONTINUE_RECORD_ERR_EN    (BIT(24))
#define SPI_MEM_ECC_CONTINUE_RECORD_ERR_EN_M  (BIT(24))
#define SPI_MEM_ECC_CONTINUE_RECORD_ERR_EN_V  0x1
#define SPI_MEM_ECC_CONTINUE_RECORD_ERR_EN_S  24
/* SPI_MEM_USR_ECC_ADDR_EN : HRO ;bitpos:[21] ;default: 1'd0 ; */
/*description: Set this bit to enable ECC address convert in SPI0/1 USR_CMD transfer..*/
#define SPI_MEM_USR_ECC_ADDR_EN    (BIT(21))
#define SPI_MEM_USR_ECC_ADDR_EN_M  (BIT(21))
#define SPI_MEM_USR_ECC_ADDR_EN_V  0x1
#define SPI_MEM_USR_ECC_ADDR_EN_S  21
/* SPI_MEM_FMEM_ECC_ADDR_EN : HRO ;bitpos:[20] ;default: 1'd0 ; */
/*description: Set this bit to enable MSPI ECC address conversion, no matter MSPI accesses to t
he ECC region or non-ECC region of flash. If there is no ECC region in flash, th
is bit should be 0. Otherwise, this bit should be 1..*/
#define SPI_MEM_FMEM_ECC_ADDR_EN    (BIT(20))
#define SPI_MEM_FMEM_ECC_ADDR_EN_M  (BIT(20))
#define SPI_MEM_FMEM_ECC_ADDR_EN_V  0x1
#define SPI_MEM_FMEM_ECC_ADDR_EN_S  20
/* SPI_MEM_FMEM_PAGE_SIZE : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: Set the page size of the flash accessed by MSPI. 0: 256 bytes. 1: 512 bytes. 2:
1024 bytes. 3: 2048 bytes..*/
#define SPI_MEM_FMEM_PAGE_SIZE    0x00000003
#define SPI_MEM_FMEM_PAGE_SIZE_M  ((SPI_MEM_FMEM_PAGE_SIZE_V)<<(SPI_MEM_FMEM_PAGE_SIZE_S))
#define SPI_MEM_FMEM_PAGE_SIZE_V  0x3
#define SPI_MEM_FMEM_PAGE_SIZE_S  18
/* SPI_MEM_FMEM_ECC_ERR_INT_EN : HRO ;bitpos:[17] ;default: 1'b0 ; */
/*description: Set this bit to calculate the error times of MSPI ECC read when accesses to flas
h..*/
#define SPI_MEM_FMEM_ECC_ERR_INT_EN    (BIT(17))
#define SPI_MEM_FMEM_ECC_ERR_INT_EN_M  (BIT(17))
#define SPI_MEM_FMEM_ECC_ERR_INT_EN_V  0x1
#define SPI_MEM_FMEM_ECC_ERR_INT_EN_S  17
/* SPI_MEM_FMEM_ECC_ERR_INT_NUM : HRO ;bitpos:[16:11] ;default: 6'd10 ; */
/*description: Set the error times of MSPI ECC read to generate MSPI SPI_MEM_ECC_ERR_INT interr
upt..*/
#define SPI_MEM_FMEM_ECC_ERR_INT_NUM    0x0000003F
#define SPI_MEM_FMEM_ECC_ERR_INT_NUM_M  ((SPI_MEM_FMEM_ECC_ERR_INT_NUM_V)<<(SPI_MEM_FMEM_ECC_ERR_INT_NUM_S))
#define SPI_MEM_FMEM_ECC_ERR_INT_NUM_V  0x3F
#define SPI_MEM_FMEM_ECC_ERR_INT_NUM_S  11

#define SPI_MEM_ECC_ERR_ADDR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x16C)
/* SPI_MEM_ECC_ERR_CNT : HRO ;bitpos:[31:26] ;default: 6'd0 ; */
/*description: This bits show the error times of MSPI ECC read. It is cleared by when  SPI_MEM_
ECC_ERR_INT_CLR bit is set..*/
#define SPI_MEM_ECC_ERR_CNT    0x0000003F
#define SPI_MEM_ECC_ERR_CNT_M  ((SPI_MEM_ECC_ERR_CNT_V)<<(SPI_MEM_ECC_ERR_CNT_S))
#define SPI_MEM_ECC_ERR_CNT_V  0x3F
#define SPI_MEM_ECC_ERR_CNT_S  26
/* SPI_MEM_ECC_ERR_ADDR : HRO ;bitpos:[25:0] ;default: 26'h0 ; */
/*description: This bits show the first MSPI ECC error address. It is cleared by when  SPI_MEM_
ECC_ERR_INT_CLR bit is set..*/
#define SPI_MEM_ECC_ERR_ADDR    0x03FFFFFF
#define SPI_MEM_ECC_ERR_ADDR_M  ((SPI_MEM_ECC_ERR_ADDR_V)<<(SPI_MEM_ECC_ERR_ADDR_S))
#define SPI_MEM_ECC_ERR_ADDR_V  0x3FFFFFF
#define SPI_MEM_ECC_ERR_ADDR_S  0

#define SPI_MEM_AXI_ERR_ADDR_REG(i)          (REG_SPI_MEM_BASE(i) + 0x170)
/* SPI_MEM_ALL_AXI_TRANS_AFIFO_EMPTY : RO ;bitpos:[31] ;default: 1'b1 ; */
/*description: This bit is set when WADDR_AFIFO, WBLEN_AFIFO, WDATA_AFIFO, AXI_RADDR_CTL_AFIFO
and RDATA_AFIFO are empty and spi0_mst_st is IDLE..*/
#define SPI_MEM_ALL_AXI_TRANS_AFIFO_EMPTY    (BIT(31))
#define SPI_MEM_ALL_AXI_TRANS_AFIFO_EMPTY_M  (BIT(31))
#define SPI_MEM_ALL_AXI_TRANS_AFIFO_EMPTY_V  0x1
#define SPI_MEM_ALL_AXI_TRANS_AFIFO_EMPTY_S  31
/* SPI_MEM_WBLEN_AFIFO_REMPTY : RO ;bitpos:[30] ;default: 1'b1 ; */
/*description: 1: WBLEN_AFIFO is empty. 0: At least one AXI write transfer is pending..*/
#define SPI_MEM_WBLEN_AFIFO_REMPTY    (BIT(30))
#define SPI_MEM_WBLEN_AFIFO_REMPTY_M  (BIT(30))
#define SPI_MEM_WBLEN_AFIFO_REMPTY_V  0x1
#define SPI_MEM_WBLEN_AFIFO_REMPTY_S  30
/* SPI_MEM_WDATA_AFIFO_REMPTY : RO ;bitpos:[29] ;default: 1'b1 ; */
/*description: 1: WDATA_AFIFO is empty. 0: At least one AXI write transfer is pending..*/
#define SPI_MEM_WDATA_AFIFO_REMPTY    (BIT(29))
#define SPI_MEM_WDATA_AFIFO_REMPTY_M  (BIT(29))
#define SPI_MEM_WDATA_AFIFO_REMPTY_V  0x1
#define SPI_MEM_WDATA_AFIFO_REMPTY_S  29
/* SPI_MEM_RADDR_AFIFO_REMPTY : RO ;bitpos:[28] ;default: 1'b1 ; */
/*description: 1: AXI_RADDR_CTL_AFIFO is empty. 0: At least one AXI read transfer is pending..*/
#define SPI_MEM_RADDR_AFIFO_REMPTY    (BIT(28))
#define SPI_MEM_RADDR_AFIFO_REMPTY_M  (BIT(28))
#define SPI_MEM_RADDR_AFIFO_REMPTY_V  0x1
#define SPI_MEM_RADDR_AFIFO_REMPTY_S  28
/* SPI_MEM_RDATA_AFIFO_REMPTY : RO ;bitpos:[27] ;default: 1'b1 ; */
/*description: 1: RDATA_AFIFO is empty. 0: At least one AXI read transfer is pending..*/
#define SPI_MEM_RDATA_AFIFO_REMPTY    (BIT(27))
#define SPI_MEM_RDATA_AFIFO_REMPTY_M  (BIT(27))
#define SPI_MEM_RDATA_AFIFO_REMPTY_V  0x1
#define SPI_MEM_RDATA_AFIFO_REMPTY_S  27
/* SPI_MEM_ALL_FIFO_EMPTY : RO ;bitpos:[26] ;default: 1'b1 ; */
/*description: The empty status of all AFIFO and SYNC_FIFO in MSPI module. 1: All AXI transfers
 and SPI0 transfers are done. 0: Others..*/
#define SPI_MEM_ALL_FIFO_EMPTY    (BIT(26))
#define SPI_MEM_ALL_FIFO_EMPTY_M  (BIT(26))
#define SPI_MEM_ALL_FIFO_EMPTY_V  0x1
#define SPI_MEM_ALL_FIFO_EMPTY_S  26
/* SPI_MEM_AXI_ERR_ADDR : R/SS/WTC ;bitpos:[25:0] ;default: 26'h0 ; */
/*description: This bits show the first AXI write/read invalid error or AXI write flash error
address. It is cleared by when SPI_MEM_AXI_WADDR_ERR_INT_CLR, SPI_MEM_AXI_WR_FLASH_ERR_IN_CLR
or SPI_MEM_AXI_RADDR_ERR_IN_CLR bit is set..*/
#define SPI_MEM_AXI_ERR_ADDR    0x03FFFFFF
#define SPI_MEM_AXI_ERR_ADDR_M  ((SPI_MEM_AXI_ERR_ADDR_V)<<(SPI_MEM_AXI_ERR_ADDR_S))
#define SPI_MEM_AXI_ERR_ADDR_V  0x3FFFFFF
#define SPI_MEM_AXI_ERR_ADDR_S  0

#define SPI_MEM_SPI_SMEM_ECC_CTRL_REG(i)          (REG_SPI_MEM_BASE(i) + 0x174)
/* SPI_MEM_SMEM_ECC_ADDR_EN : HRO ;bitpos:[20] ;default: 1'd0 ; */
/*description: Set this bit to enable MSPI ECC address conversion, no matter MSPI accesses to t
he ECC region or non-ECC region of external RAM. If there is no ECC region in ex
ternal RAM, this bit should be 0. Otherwise, this bit should be 1..*/
#define SPI_MEM_SMEM_ECC_ADDR_EN    (BIT(20))
#define SPI_MEM_SMEM_ECC_ADDR_EN_M  (BIT(20))
#define SPI_MEM_SMEM_ECC_ADDR_EN_V  0x1
#define SPI_MEM_SMEM_ECC_ADDR_EN_S  20
/* SPI_MEM_SMEM_PAGE_SIZE : HRO ;bitpos:[19:18] ;default: 2'd2 ; */
/*description: Set the page size of the external RAM accessed by MSPI. 0: 256 bytes. 1: 512 byt
es. 2: 1024 bytes. 3: 2048 bytes..*/
#define SPI_MEM_SMEM_PAGE_SIZE    0x00000003
#define SPI_MEM_SMEM_PAGE_SIZE_M  ((SPI_MEM_SMEM_PAGE_SIZE_V)<<(SPI_MEM_SMEM_PAGE_SIZE_S))
#define SPI_MEM_SMEM_PAGE_SIZE_V  0x3
#define SPI_MEM_SMEM_PAGE_SIZE_S  18
/* SPI_MEM_SMEM_ECC_ERR_INT_EN : HRO ;bitpos:[17] ;default: 1'b0 ; */
/*description: Set this bit to calculate the error times of MSPI ECC read when accesses to exte
rnal RAM..*/
#define SPI_MEM_SMEM_ECC_ERR_INT_EN    (BIT(17))
#define SPI_MEM_SMEM_ECC_ERR_INT_EN_M  (BIT(17))
#define SPI_MEM_SMEM_ECC_ERR_INT_EN_V  0x1
#define SPI_MEM_SMEM_ECC_ERR_INT_EN_S  17

#define SPI_MEM_TIMING_CALI_REG(i)          (REG_SPI_MEM_BASE(i) + 0x180)
/* SPI_MEM_TIMING_CALI_UPDATE : WT ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to update delay mode, delay num and extra dummy in MSPI..*/
#define SPI_MEM_TIMING_CALI_UPDATE    (BIT(6))
#define SPI_MEM_TIMING_CALI_UPDATE_M  (BIT(6))
#define SPI_MEM_TIMING_CALI_UPDATE_V  0x1
#define SPI_MEM_TIMING_CALI_UPDATE_S  6
/* SPI_MEM_DLL_TIMING_CALI : HRO ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to enable DLL for timing calibration in DDR mode when accessed to f
lash..*/
#define SPI_MEM_DLL_TIMING_CALI    (BIT(5))
#define SPI_MEM_DLL_TIMING_CALI_M  (BIT(5))
#define SPI_MEM_DLL_TIMING_CALI_V  0x1
#define SPI_MEM_DLL_TIMING_CALI_S  5
/* SPI_MEM_EXTRA_DUMMY_CYCLELEN : R/W ;bitpos:[4:2] ;default: 3'd0 ; */
/*description: add extra dummy spi clock cycle length for spi clock calibration..*/
#define SPI_MEM_EXTRA_DUMMY_CYCLELEN    0x00000007
#define SPI_MEM_EXTRA_DUMMY_CYCLELEN_M  ((SPI_MEM_EXTRA_DUMMY_CYCLELEN_V)<<(SPI_MEM_EXTRA_DUMMY_CYCLELEN_S))
#define SPI_MEM_EXTRA_DUMMY_CYCLELEN_V  0x7
#define SPI_MEM_EXTRA_DUMMY_CYCLELEN_S  2
/* SPI_MEM_TIMING_CALI : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The bit is used to enable timing auto-calibration for all reading operations..*/
#define SPI_MEM_TIMING_CALI    (BIT(1))
#define SPI_MEM_TIMING_CALI_M  (BIT(1))
#define SPI_MEM_TIMING_CALI_V  0x1
#define SPI_MEM_TIMING_CALI_S  1
/* SPI_MEM_TIMING_CLK_ENA : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: The bit is used to enable timing adjust clock for all reading operations..*/
#define SPI_MEM_TIMING_CLK_ENA    (BIT(0))
#define SPI_MEM_TIMING_CLK_ENA_M  (BIT(0))
#define SPI_MEM_TIMING_CLK_ENA_V  0x1
#define SPI_MEM_TIMING_CLK_ENA_S  0

#define SPI_MEM_DIN_MODE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x184)
/* SPI_MEM_DINS_MODE : R/W ;bitpos:[26:24] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: inp
ut with the spi_clk.*/
#define SPI_MEM_DINS_MODE    0x00000007
#define SPI_MEM_DINS_MODE_M  ((SPI_MEM_DINS_MODE_V)<<(SPI_MEM_DINS_MODE_S))
#define SPI_MEM_DINS_MODE_V  0x7
#define SPI_MEM_DINS_MODE_S  24
/* SPI_MEM_DIN7_MODE : R/W ;bitpos:[23:21] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: inp
ut with the spi_clk.*/
#define SPI_MEM_DIN7_MODE    0x00000007
#define SPI_MEM_DIN7_MODE_M  ((SPI_MEM_DIN7_MODE_V)<<(SPI_MEM_DIN7_MODE_S))
#define SPI_MEM_DIN7_MODE_V  0x7
#define SPI_MEM_DIN7_MODE_S  21
/* SPI_MEM_DIN6_MODE : R/W ;bitpos:[20:18] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: inp
ut with the spi_clk.*/
#define SPI_MEM_DIN6_MODE    0x00000007
#define SPI_MEM_DIN6_MODE_M  ((SPI_MEM_DIN6_MODE_V)<<(SPI_MEM_DIN6_MODE_S))
#define SPI_MEM_DIN6_MODE_V  0x7
#define SPI_MEM_DIN6_MODE_S  18
/* SPI_MEM_DIN5_MODE : R/W ;bitpos:[17:15] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: inp
ut with the spi_clk.*/
#define SPI_MEM_DIN5_MODE    0x00000007
#define SPI_MEM_DIN5_MODE_M  ((SPI_MEM_DIN5_MODE_V)<<(SPI_MEM_DIN5_MODE_S))
#define SPI_MEM_DIN5_MODE_V  0x7
#define SPI_MEM_DIN5_MODE_S  15
/* SPI_MEM_DIN4_MODE : R/W ;bitpos:[14:12] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: inp
ut with the spi_clk.*/
#define SPI_MEM_DIN4_MODE    0x00000007
#define SPI_MEM_DIN4_MODE_M  ((SPI_MEM_DIN4_MODE_V)<<(SPI_MEM_DIN4_MODE_S))
#define SPI_MEM_DIN4_MODE_V  0x7
#define SPI_MEM_DIN4_MODE_S  12
/* SPI_MEM_DIN3_MODE : R/W ;bitpos:[11:9] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: in
put with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input w
ith the spi_clk high edge,  6: input with the spi_clk low edge.*/
#define SPI_MEM_DIN3_MODE    0x00000007
#define SPI_MEM_DIN3_MODE_M  ((SPI_MEM_DIN3_MODE_V)<<(SPI_MEM_DIN3_MODE_S))
#define SPI_MEM_DIN3_MODE_V  0x7
#define SPI_MEM_DIN3_MODE_S  9
/* SPI_MEM_DIN2_MODE : R/W ;bitpos:[8:6] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: in
put with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input w
ith the spi_clk high edge,  6: input with the spi_clk low edge.*/
#define SPI_MEM_DIN2_MODE    0x00000007
#define SPI_MEM_DIN2_MODE_M  ((SPI_MEM_DIN2_MODE_V)<<(SPI_MEM_DIN2_MODE_S))
#define SPI_MEM_DIN2_MODE_V  0x7
#define SPI_MEM_DIN2_MODE_S  6
/* SPI_MEM_DIN1_MODE : R/W ;bitpos:[5:3] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: in
put with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input w
ith the spi_clk high edge,  6: input with the spi_clk low edge.*/
#define SPI_MEM_DIN1_MODE    0x00000007
#define SPI_MEM_DIN1_MODE_M  ((SPI_MEM_DIN1_MODE_V)<<(SPI_MEM_DIN1_MODE_S))
#define SPI_MEM_DIN1_MODE_V  0x7
#define SPI_MEM_DIN1_MODE_S  3
/* SPI_MEM_DIN0_MODE : R/W ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: in
put with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input w
ith the spi_clk high edge,  6: input with the spi_clk low edge.*/
#define SPI_MEM_DIN0_MODE    0x00000007
#define SPI_MEM_DIN0_MODE_M  ((SPI_MEM_DIN0_MODE_V)<<(SPI_MEM_DIN0_MODE_S))
#define SPI_MEM_DIN0_MODE_V  0x7
#define SPI_MEM_DIN0_MODE_S  0

#define SPI_MEM_DIN_NUM_REG(i)          (REG_SPI_MEM_BASE(i) + 0x188)
/* SPI_MEM_DINS_NUM : R/W ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_DINS_NUM    0x00000003
#define SPI_MEM_DINS_NUM_M  ((SPI_MEM_DINS_NUM_V)<<(SPI_MEM_DINS_NUM_S))
#define SPI_MEM_DINS_NUM_V  0x3
#define SPI_MEM_DINS_NUM_S  16
/* SPI_MEM_DIN7_NUM : R/W ;bitpos:[15:14] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_DIN7_NUM    0x00000003
#define SPI_MEM_DIN7_NUM_M  ((SPI_MEM_DIN7_NUM_V)<<(SPI_MEM_DIN7_NUM_S))
#define SPI_MEM_DIN7_NUM_V  0x3
#define SPI_MEM_DIN7_NUM_S  14
/* SPI_MEM_DIN6_NUM : R/W ;bitpos:[13:12] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_DIN6_NUM    0x00000003
#define SPI_MEM_DIN6_NUM_M  ((SPI_MEM_DIN6_NUM_V)<<(SPI_MEM_DIN6_NUM_S))
#define SPI_MEM_DIN6_NUM_V  0x3
#define SPI_MEM_DIN6_NUM_S  12
/* SPI_MEM_DIN5_NUM : R/W ;bitpos:[11:10] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_DIN5_NUM    0x00000003
#define SPI_MEM_DIN5_NUM_M  ((SPI_MEM_DIN5_NUM_V)<<(SPI_MEM_DIN5_NUM_S))
#define SPI_MEM_DIN5_NUM_V  0x3
#define SPI_MEM_DIN5_NUM_S  10
/* SPI_MEM_DIN4_NUM : R/W ;bitpos:[9:8] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_DIN4_NUM    0x00000003
#define SPI_MEM_DIN4_NUM_M  ((SPI_MEM_DIN4_NUM_V)<<(SPI_MEM_DIN4_NUM_S))
#define SPI_MEM_DIN4_NUM_V  0x3
#define SPI_MEM_DIN4_NUM_S  8
/* SPI_MEM_DIN3_NUM : R/W ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_DIN3_NUM    0x00000003
#define SPI_MEM_DIN3_NUM_M  ((SPI_MEM_DIN3_NUM_V)<<(SPI_MEM_DIN3_NUM_S))
#define SPI_MEM_DIN3_NUM_V  0x3
#define SPI_MEM_DIN3_NUM_S  6
/* SPI_MEM_DIN2_NUM : R/W ;bitpos:[5:4] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_DIN2_NUM    0x00000003
#define SPI_MEM_DIN2_NUM_M  ((SPI_MEM_DIN2_NUM_V)<<(SPI_MEM_DIN2_NUM_S))
#define SPI_MEM_DIN2_NUM_V  0x3
#define SPI_MEM_DIN2_NUM_S  4
/* SPI_MEM_DIN1_NUM : R/W ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_DIN1_NUM    0x00000003
#define SPI_MEM_DIN1_NUM_M  ((SPI_MEM_DIN1_NUM_V)<<(SPI_MEM_DIN1_NUM_S))
#define SPI_MEM_DIN1_NUM_V  0x3
#define SPI_MEM_DIN1_NUM_S  2
/* SPI_MEM_DIN0_NUM : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_DIN0_NUM    0x00000003
#define SPI_MEM_DIN0_NUM_M  ((SPI_MEM_DIN0_NUM_V)<<(SPI_MEM_DIN0_NUM_S))
#define SPI_MEM_DIN0_NUM_V  0x3
#define SPI_MEM_DIN0_NUM_S  0

#define SPI_MEM_DOUT_MODE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x18C)
/* SPI_MEM_DOUTS_MODE : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the spi_clk.*/
#define SPI_MEM_DOUTS_MODE    (BIT(8))
#define SPI_MEM_DOUTS_MODE_M  (BIT(8))
#define SPI_MEM_DOUTS_MODE_V  0x1
#define SPI_MEM_DOUTS_MODE_S  8
/* SPI_MEM_DOUT7_MODE : R/W ;bitpos:[7] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the spi_clk.*/
#define SPI_MEM_DOUT7_MODE    (BIT(7))
#define SPI_MEM_DOUT7_MODE_M  (BIT(7))
#define SPI_MEM_DOUT7_MODE_V  0x1
#define SPI_MEM_DOUT7_MODE_S  7
/* SPI_MEM_DOUT6_MODE : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the spi_clk.*/
#define SPI_MEM_DOUT6_MODE    (BIT(6))
#define SPI_MEM_DOUT6_MODE_M  (BIT(6))
#define SPI_MEM_DOUT6_MODE_V  0x1
#define SPI_MEM_DOUT6_MODE_S  6
/* SPI_MEM_DOUT5_MODE : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the spi_clk.*/
#define SPI_MEM_DOUT5_MODE    (BIT(5))
#define SPI_MEM_DOUT5_MODE_M  (BIT(5))
#define SPI_MEM_DOUT5_MODE_V  0x1
#define SPI_MEM_DOUT5_MODE_S  5
/* SPI_MEM_DOUT4_MODE : R/W ;bitpos:[4] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the spi_clk.*/
#define SPI_MEM_DOUT4_MODE    (BIT(4))
#define SPI_MEM_DOUT4_MODE_M  (BIT(4))
#define SPI_MEM_DOUT4_MODE_V  0x1
#define SPI_MEM_DOUT4_MODE_S  4
/* SPI_MEM_DOUT3_MODE : R/W ;bitpos:[3] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the posedge of clk_160,4 output with the negedge of clk_160,5: outp
ut with the spi_clk high edge ,6: output with the spi_clk low edge.*/
#define SPI_MEM_DOUT3_MODE    (BIT(3))
#define SPI_MEM_DOUT3_MODE_M  (BIT(3))
#define SPI_MEM_DOUT3_MODE_V  0x1
#define SPI_MEM_DOUT3_MODE_S  3
/* SPI_MEM_DOUT2_MODE : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the posedge of clk_160,4 output with the negedge of clk_160,5: outp
ut with the spi_clk high edge ,6: output with the spi_clk low edge.*/
#define SPI_MEM_DOUT2_MODE    (BIT(2))
#define SPI_MEM_DOUT2_MODE_M  (BIT(2))
#define SPI_MEM_DOUT2_MODE_V  0x1
#define SPI_MEM_DOUT2_MODE_S  2
/* SPI_MEM_DOUT1_MODE : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the posedge of clk_160,4 output with the negedge of clk_160,5: outp
ut with the spi_clk high edge ,6: output with the spi_clk low edge.*/
#define SPI_MEM_DOUT1_MODE    (BIT(1))
#define SPI_MEM_DOUT1_MODE_M  (BIT(1))
#define SPI_MEM_DOUT1_MODE_V  0x1
#define SPI_MEM_DOUT1_MODE_S  1
/* SPI_MEM_DOUT0_MODE : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the posedge of clk_160,4 output with the negedge of clk_160,5: outp
ut with the spi_clk high edge ,6: output with the spi_clk low edge.*/
#define SPI_MEM_DOUT0_MODE    (BIT(0))
#define SPI_MEM_DOUT0_MODE_M  (BIT(0))
#define SPI_MEM_DOUT0_MODE_V  0x1
#define SPI_MEM_DOUT0_MODE_S  0

#define SPI_MEM_SPI_SMEM_TIMING_CALI_REG(i)          (REG_SPI_MEM_BASE(i) + 0x190)
/* SPI_MEM_SMEM_DLL_TIMING_CALI : HRO ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to enable DLL for timing calibration in DDR mode when accessed to E
XT_RAM..*/
#define SPI_MEM_SMEM_DLL_TIMING_CALI    (BIT(5))
#define SPI_MEM_SMEM_DLL_TIMING_CALI_M  (BIT(5))
#define SPI_MEM_SMEM_DLL_TIMING_CALI_V  0x1
#define SPI_MEM_SMEM_DLL_TIMING_CALI_S  5
/* SPI_MEM_SMEM_EXTRA_DUMMY_CYCLELEN : HRO ;bitpos:[4:2] ;default: 3'd0 ; */
/*description: For sram, add extra dummy spi clock cycle length for spi clock calibration..*/
#define SPI_MEM_SMEM_EXTRA_DUMMY_CYCLELEN    0x00000007
#define SPI_MEM_SMEM_EXTRA_DUMMY_CYCLELEN_M  ((SPI_MEM_SMEM_EXTRA_DUMMY_CYCLELEN_V)<<(SPI_MEM_SMEM_EXTRA_DUMMY_CYCLELEN_S))
#define SPI_MEM_SMEM_EXTRA_DUMMY_CYCLELEN_V  0x7
#define SPI_MEM_SMEM_EXTRA_DUMMY_CYCLELEN_S  2
/* SPI_MEM_SMEM_TIMING_CALI : HRO ;bitpos:[1] ;default: 1'b0 ; */
/*description: For sram, the bit is used to enable timing auto-calibration for all reading oper
ations..*/
#define SPI_MEM_SMEM_TIMING_CALI    (BIT(1))
#define SPI_MEM_SMEM_TIMING_CALI_M  (BIT(1))
#define SPI_MEM_SMEM_TIMING_CALI_V  0x1
#define SPI_MEM_SMEM_TIMING_CALI_S  1
/* SPI_MEM_SMEM_TIMING_CLK_ENA : HRO ;bitpos:[0] ;default: 1'b1 ; */
/*description: For sram, the bit is used to enable timing adjust clock for all reading operation
ns..*/
#define SPI_MEM_SMEM_TIMING_CLK_ENA    (BIT(0))
#define SPI_MEM_SMEM_TIMING_CLK_ENA_M  (BIT(0))
#define SPI_MEM_SMEM_TIMING_CLK_ENA_V  0x1
#define SPI_MEM_SMEM_TIMING_CLK_ENA_S  0

#define SPI_MEM_SPI_SMEM_DIN_MODE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x194)
/* SPI_MEM_SMEM_DINS_MODE : HRO ;bitpos:[26:24] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: in
put with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input w
ith the spi_clk high edge,  6: input with the spi_clk low edge.*/
#define SPI_MEM_SMEM_DINS_MODE    0x00000007
#define SPI_MEM_SMEM_DINS_MODE_M  ((SPI_MEM_SMEM_DINS_MODE_V)<<(SPI_MEM_SMEM_DINS_MODE_S))
#define SPI_MEM_SMEM_DINS_MODE_V  0x7
#define SPI_MEM_SMEM_DINS_MODE_S  24
/* SPI_MEM_SMEM_DIN7_MODE : HRO ;bitpos:[23:21] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: in
put with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input w
ith the spi_clk high edge,  6: input with the spi_clk low edge.*/
#define SPI_MEM_SMEM_DIN7_MODE    0x00000007
#define SPI_MEM_SMEM_DIN7_MODE_M  ((SPI_MEM_SMEM_DIN7_MODE_V)<<(SPI_MEM_SMEM_DIN7_MODE_S))
#define SPI_MEM_SMEM_DIN7_MODE_V  0x7
#define SPI_MEM_SMEM_DIN7_MODE_S  21
/* SPI_MEM_SMEM_DIN6_MODE : HRO ;bitpos:[20:18] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: in
put with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input w
ith the spi_clk high edge,  6: input with the spi_clk low edge.*/
#define SPI_MEM_SMEM_DIN6_MODE    0x00000007
#define SPI_MEM_SMEM_DIN6_MODE_M  ((SPI_MEM_SMEM_DIN6_MODE_V)<<(SPI_MEM_SMEM_DIN6_MODE_S))
#define SPI_MEM_SMEM_DIN6_MODE_V  0x7
#define SPI_MEM_SMEM_DIN6_MODE_S  18
/* SPI_MEM_SMEM_DIN5_MODE : HRO ;bitpos:[17:15] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: in
put with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input w
ith the spi_clk high edge,  6: input with the spi_clk low edge.*/
#define SPI_MEM_SMEM_DIN5_MODE    0x00000007
#define SPI_MEM_SMEM_DIN5_MODE_M  ((SPI_MEM_SMEM_DIN5_MODE_V)<<(SPI_MEM_SMEM_DIN5_MODE_S))
#define SPI_MEM_SMEM_DIN5_MODE_V  0x7
#define SPI_MEM_SMEM_DIN5_MODE_S  15
/* SPI_MEM_SMEM_DIN4_MODE : HRO ;bitpos:[14:12] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: in
put with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input w
ith the spi_clk high edge,  6: input with the spi_clk low edge.*/
#define SPI_MEM_SMEM_DIN4_MODE    0x00000007
#define SPI_MEM_SMEM_DIN4_MODE_M  ((SPI_MEM_SMEM_DIN4_MODE_V)<<(SPI_MEM_SMEM_DIN4_MODE_S))
#define SPI_MEM_SMEM_DIN4_MODE_V  0x7
#define SPI_MEM_SMEM_DIN4_MODE_S  12
/* SPI_MEM_SMEM_DIN3_MODE : HRO ;bitpos:[11:9] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: in
put with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input w
ith the spi_clk high edge,  6: input with the spi_clk low edge.*/
#define SPI_MEM_SMEM_DIN3_MODE    0x00000007
#define SPI_MEM_SMEM_DIN3_MODE_M  ((SPI_MEM_SMEM_DIN3_MODE_V)<<(SPI_MEM_SMEM_DIN3_MODE_S))
#define SPI_MEM_SMEM_DIN3_MODE_V  0x7
#define SPI_MEM_SMEM_DIN3_MODE_S  9
/* SPI_MEM_SMEM_DIN2_MODE : HRO ;bitpos:[8:6] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: in
put with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input w
ith the spi_clk high edge,  6: input with the spi_clk low edge.*/
#define SPI_MEM_SMEM_DIN2_MODE    0x00000007
#define SPI_MEM_SMEM_DIN2_MODE_M  ((SPI_MEM_SMEM_DIN2_MODE_V)<<(SPI_MEM_SMEM_DIN2_MODE_S))
#define SPI_MEM_SMEM_DIN2_MODE_V  0x7
#define SPI_MEM_SMEM_DIN2_MODE_S  6
/* SPI_MEM_SMEM_DIN1_MODE : HRO ;bitpos:[5:3] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: in
put with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input w
ith the spi_clk high edge,  6: input with the spi_clk low edge.*/
#define SPI_MEM_SMEM_DIN1_MODE    0x00000007
#define SPI_MEM_SMEM_DIN1_MODE_M  ((SPI_MEM_SMEM_DIN1_MODE_V)<<(SPI_MEM_SMEM_DIN1_MODE_S))
#define SPI_MEM_SMEM_DIN1_MODE_V  0x7
#define SPI_MEM_SMEM_DIN1_MODE_S  3
/* SPI_MEM_SMEM_DIN0_MODE : HRO ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: in
put with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input w
ith the spi_clk high edge,  6: input with the spi_clk low edge.*/
#define SPI_MEM_SMEM_DIN0_MODE    0x00000007
#define SPI_MEM_SMEM_DIN0_MODE_M  ((SPI_MEM_SMEM_DIN0_MODE_V)<<(SPI_MEM_SMEM_DIN0_MODE_S))
#define SPI_MEM_SMEM_DIN0_MODE_V  0x7
#define SPI_MEM_SMEM_DIN0_MODE_S  0

#define SPI_MEM_SPI_SMEM_DIN_NUM_REG(i)          (REG_SPI_MEM_BASE(i) + 0x198)
/* SPI_MEM_SMEM_DINS_NUM : HRO ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: input without delayed,
1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: in
put with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input w
ith the spi_clk high edge,  6: input with the spi_clk low edge.*/
#define SPI_MEM_SMEM_DINS_NUM    0x00000003
#define SPI_MEM_SMEM_DINS_NUM_M  ((SPI_MEM_SMEM_DINS_NUM_V)<<(SPI_MEM_SMEM_DINS_NUM_S))
#define SPI_MEM_SMEM_DINS_NUM_V  0x3
#define SPI_MEM_SMEM_DINS_NUM_S  16
/* SPI_MEM_SMEM_DIN7_NUM : HRO ;bitpos:[15:14] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_SMEM_DIN7_NUM    0x00000003
#define SPI_MEM_SMEM_DIN7_NUM_M  ((SPI_MEM_SMEM_DIN7_NUM_V)<<(SPI_MEM_SMEM_DIN7_NUM_S))
#define SPI_MEM_SMEM_DIN7_NUM_V  0x3
#define SPI_MEM_SMEM_DIN7_NUM_S  14
/* SPI_MEM_SMEM_DIN6_NUM : HRO ;bitpos:[13:12] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_SMEM_DIN6_NUM    0x00000003
#define SPI_MEM_SMEM_DIN6_NUM_M  ((SPI_MEM_SMEM_DIN6_NUM_V)<<(SPI_MEM_SMEM_DIN6_NUM_S))
#define SPI_MEM_SMEM_DIN6_NUM_V  0x3
#define SPI_MEM_SMEM_DIN6_NUM_S  12
/* SPI_MEM_SMEM_DIN5_NUM : HRO ;bitpos:[11:10] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_SMEM_DIN5_NUM    0x00000003
#define SPI_MEM_SMEM_DIN5_NUM_M  ((SPI_MEM_SMEM_DIN5_NUM_V)<<(SPI_MEM_SMEM_DIN5_NUM_S))
#define SPI_MEM_SMEM_DIN5_NUM_V  0x3
#define SPI_MEM_SMEM_DIN5_NUM_S  10
/* SPI_MEM_SMEM_DIN4_NUM : HRO ;bitpos:[9:8] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_SMEM_DIN4_NUM    0x00000003
#define SPI_MEM_SMEM_DIN4_NUM_M  ((SPI_MEM_SMEM_DIN4_NUM_V)<<(SPI_MEM_SMEM_DIN4_NUM_S))
#define SPI_MEM_SMEM_DIN4_NUM_V  0x3
#define SPI_MEM_SMEM_DIN4_NUM_S  8
/* SPI_MEM_SMEM_DIN3_NUM : HRO ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_SMEM_DIN3_NUM    0x00000003
#define SPI_MEM_SMEM_DIN3_NUM_M  ((SPI_MEM_SMEM_DIN3_NUM_V)<<(SPI_MEM_SMEM_DIN3_NUM_S))
#define SPI_MEM_SMEM_DIN3_NUM_V  0x3
#define SPI_MEM_SMEM_DIN3_NUM_S  6
/* SPI_MEM_SMEM_DIN2_NUM : HRO ;bitpos:[5:4] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_SMEM_DIN2_NUM    0x00000003
#define SPI_MEM_SMEM_DIN2_NUM_M  ((SPI_MEM_SMEM_DIN2_NUM_V)<<(SPI_MEM_SMEM_DIN2_NUM_S))
#define SPI_MEM_SMEM_DIN2_NUM_V  0x3
#define SPI_MEM_SMEM_DIN2_NUM_S  4
/* SPI_MEM_SMEM_DIN1_NUM : HRO ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_SMEM_DIN1_NUM    0x00000003
#define SPI_MEM_SMEM_DIN1_NUM_M  ((SPI_MEM_SMEM_DIN1_NUM_V)<<(SPI_MEM_SMEM_DIN1_NUM_S))
#define SPI_MEM_SMEM_DIN1_NUM_V  0x3
#define SPI_MEM_SMEM_DIN1_NUM_S  2
/* SPI_MEM_SMEM_DIN0_NUM : HRO ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
delayed by 2 cycles,....*/
#define SPI_MEM_SMEM_DIN0_NUM    0x00000003
#define SPI_MEM_SMEM_DIN0_NUM_M  ((SPI_MEM_SMEM_DIN0_NUM_V)<<(SPI_MEM_SMEM_DIN0_NUM_S))
#define SPI_MEM_SMEM_DIN0_NUM_V  0x3
#define SPI_MEM_SMEM_DIN0_NUM_S  0

#define SPI_MEM_SPI_SMEM_DOUT_MODE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x19C)
/* SPI_MEM_SMEM_DOUTS_MODE : HRO ;bitpos:[8] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the posedge of clk_160,4 output with the negedge of clk_160,5: outp
ut with the spi_clk high edge ,6: output with the spi_clk low edge.*/
#define SPI_MEM_SMEM_DOUTS_MODE    (BIT(8))
#define SPI_MEM_SMEM_DOUTS_MODE_M  (BIT(8))
#define SPI_MEM_SMEM_DOUTS_MODE_V  0x1
#define SPI_MEM_SMEM_DOUTS_MODE_S  8
/* SPI_MEM_SMEM_DOUT7_MODE : HRO ;bitpos:[7] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the posedge of clk_160,4 output with the negedge of clk_160,5: outp
ut with the spi_clk high edge ,6: output with the spi_clk low edge.*/
#define SPI_MEM_SMEM_DOUT7_MODE    (BIT(7))
#define SPI_MEM_SMEM_DOUT7_MODE_M  (BIT(7))
#define SPI_MEM_SMEM_DOUT7_MODE_V  0x1
#define SPI_MEM_SMEM_DOUT7_MODE_S  7
/* SPI_MEM_SMEM_DOUT6_MODE : HRO ;bitpos:[6] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the posedge of clk_160,4 output with the negedge of clk_160,5: outp
ut with the spi_clk high edge ,6: output with the spi_clk low edge.*/
#define SPI_MEM_SMEM_DOUT6_MODE    (BIT(6))
#define SPI_MEM_SMEM_DOUT6_MODE_M  (BIT(6))
#define SPI_MEM_SMEM_DOUT6_MODE_V  0x1
#define SPI_MEM_SMEM_DOUT6_MODE_S  6
/* SPI_MEM_SMEM_DOUT5_MODE : HRO ;bitpos:[5] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the posedge of clk_160,4 output with the negedge of clk_160,5: outp
ut with the spi_clk high edge ,6: output with the spi_clk low edge.*/
#define SPI_MEM_SMEM_DOUT5_MODE    (BIT(5))
#define SPI_MEM_SMEM_DOUT5_MODE_M  (BIT(5))
#define SPI_MEM_SMEM_DOUT5_MODE_V  0x1
#define SPI_MEM_SMEM_DOUT5_MODE_S  5
/* SPI_MEM_SMEM_DOUT4_MODE : HRO ;bitpos:[4] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the posedge of clk_160,4 output with the negedge of clk_160,5: outp
ut with the spi_clk high edge ,6: output with the spi_clk low edge.*/
#define SPI_MEM_SMEM_DOUT4_MODE    (BIT(4))
#define SPI_MEM_SMEM_DOUT4_MODE_M  (BIT(4))
#define SPI_MEM_SMEM_DOUT4_MODE_V  0x1
#define SPI_MEM_SMEM_DOUT4_MODE_S  4
/* SPI_MEM_SMEM_DOUT3_MODE : HRO ;bitpos:[3] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the posedge of clk_160,4 output with the negedge of clk_160,5: outp
ut with the spi_clk high edge ,6: output with the spi_clk low edge.*/
#define SPI_MEM_SMEM_DOUT3_MODE    (BIT(3))
#define SPI_MEM_SMEM_DOUT3_MODE_M  (BIT(3))
#define SPI_MEM_SMEM_DOUT3_MODE_V  0x1
#define SPI_MEM_SMEM_DOUT3_MODE_S  3
/* SPI_MEM_SMEM_DOUT2_MODE : HRO ;bitpos:[2] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the posedge of clk_160,4 output with the negedge of clk_160,5: outp
ut with the spi_clk high edge ,6: output with the spi_clk low edge.*/
#define SPI_MEM_SMEM_DOUT2_MODE    (BIT(2))
#define SPI_MEM_SMEM_DOUT2_MODE_M  (BIT(2))
#define SPI_MEM_SMEM_DOUT2_MODE_V  0x1
#define SPI_MEM_SMEM_DOUT2_MODE_S  2
/* SPI_MEM_SMEM_DOUT1_MODE : HRO ;bitpos:[1] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the posedge of clk_160,4 output with the negedge of clk_160,5: outp
ut with the spi_clk high edge ,6: output with the spi_clk low edge.*/
#define SPI_MEM_SMEM_DOUT1_MODE    (BIT(1))
#define SPI_MEM_SMEM_DOUT1_MODE_M  (BIT(1))
#define SPI_MEM_SMEM_DOUT1_MODE_V  0x1
#define SPI_MEM_SMEM_DOUT1_MODE_S  1
/* SPI_MEM_SMEM_DOUT0_MODE : HRO ;bitpos:[0] ;default: 1'h0 ; */
/*description: the output signals are delayed by system clock cycles, 0: output without delayed
, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
 output with the posedge of clk_160,4 output with the negedge of clk_160,5: outp
ut with the spi_clk high edge ,6: output with the spi_clk low edge.*/
#define SPI_MEM_SMEM_DOUT0_MODE    (BIT(0))
#define SPI_MEM_SMEM_DOUT0_MODE_M  (BIT(0))
#define SPI_MEM_SMEM_DOUT0_MODE_V  0x1
#define SPI_MEM_SMEM_DOUT0_MODE_S  0

#define SPI_MEM_SPI_SMEM_AC_REG(i)          (REG_SPI_MEM_BASE(i) + 0x1A0)
/* SPI_MEM_SMEM_SPLIT_TRANS_EN : HRO ;bitpos:[31] ;default: 1'b1 ; */
/*description: Set this bit to enable SPI0 split one AXI accesses EXT_RAM transfer into two SPI
 transfers when one transfer will cross flash/EXT_RAM page corner, valid no matt
er whether there is an ECC region or not..*/
#define SPI_MEM_SMEM_SPLIT_TRANS_EN    (BIT(31))
#define SPI_MEM_SMEM_SPLIT_TRANS_EN_M  (BIT(31))
#define SPI_MEM_SMEM_SPLIT_TRANS_EN_V  0x1
#define SPI_MEM_SMEM_SPLIT_TRANS_EN_S  31
/* SPI_MEM_SMEM_CS_HOLD_DELAY : HRO ;bitpos:[30:25] ;default: 6'd0 ; */
/*description: These bits are used to set the minimum CS high time tSHSL between SPI burst tran
sfer when accesses to external RAM. tSHSL is (SPI_SMEM_CS_HOLD_DELAY[5:0] + 1) M
SPI core clock cycles..*/
#define SPI_MEM_SMEM_CS_HOLD_DELAY    0x0000003F
#define SPI_MEM_SMEM_CS_HOLD_DELAY_M  ((SPI_MEM_SMEM_CS_HOLD_DELAY_V)<<(SPI_MEM_SMEM_CS_HOLD_DELAY_S))
#define SPI_MEM_SMEM_CS_HOLD_DELAY_V  0x3F
#define SPI_MEM_SMEM_CS_HOLD_DELAY_S  25
/* SPI_MEM_SMEM_ECC_16TO18_BYTE_EN : HRO ;bitpos:[16] ;default: 1'b0 ; */
/*description: Set this bit to enable SPI0 and SPI1 ECC 16 bytes data with 2 ECC bytes mode when
accesses external RAM..*/
#define SPI_MEM_SMEM_ECC_16TO18_BYTE_EN    (BIT(16))
#define SPI_MEM_SMEM_ECC_16TO18_BYTE_EN_M  (BIT(16))
#define SPI_MEM_SMEM_ECC_16TO18_BYTE_EN_V  0x1
#define SPI_MEM_SMEM_ECC_16TO18_BYTE_EN_S  16
/* SPI_MEM_SMEM_ECC_SKIP_PAGE_CORNER : HRO ;bitpos:[15] ;default: 1'b1 ; */
/*description: 1: SPI0 skips page corner when accesses external RAM. 0: Not skip page corner wh
en accesses external RAM..*/
#define SPI_MEM_SMEM_ECC_SKIP_PAGE_CORNER    (BIT(15))
#define SPI_MEM_SMEM_ECC_SKIP_PAGE_CORNER_M  (BIT(15))
#define SPI_MEM_SMEM_ECC_SKIP_PAGE_CORNER_V  0x1
#define SPI_MEM_SMEM_ECC_SKIP_PAGE_CORNER_S  15
/* SPI_MEM_SMEM_ECC_CS_HOLD_TIME : HRO ;bitpos:[14:12] ;default: 3'd3 ; */
/*description: SPI_SMEM_CS_HOLD_TIME + SPI_SMEM_ECC_CS_HOLD_TIME is the SPI0 and SPI1 CS hold c
ycles in ECC mode when accessed external RAM..*/
#define SPI_MEM_SMEM_ECC_CS_HOLD_TIME    0x00000007
#define SPI_MEM_SMEM_ECC_CS_HOLD_TIME_M  ((SPI_MEM_SMEM_ECC_CS_HOLD_TIME_V)<<(SPI_MEM_SMEM_ECC_CS_HOLD_TIME_S))
#define SPI_MEM_SMEM_ECC_CS_HOLD_TIME_V  0x7
#define SPI_MEM_SMEM_ECC_CS_HOLD_TIME_S  12
/* SPI_MEM_SMEM_CS_HOLD_TIME : HRO ;bitpos:[11:7] ;default: 5'h1 ; */
/*description: For SPI0 and SPI1, spi cs signal is delayed to inactive by spi clock this bits a
re combined with spi_mem_cs_hold bit..*/
#define SPI_MEM_SMEM_CS_HOLD_TIME    0x0000001F
#define SPI_MEM_SMEM_CS_HOLD_TIME_M  ((SPI_MEM_SMEM_CS_HOLD_TIME_V)<<(SPI_MEM_SMEM_CS_HOLD_TIME_S))
#define SPI_MEM_SMEM_CS_HOLD_TIME_V  0x1F
#define SPI_MEM_SMEM_CS_HOLD_TIME_S  7
/* SPI_MEM_SMEM_CS_SETUP_TIME : HRO ;bitpos:[6:2] ;default: 5'h1 ; */
/*description: For spi0, (cycles-1) of prepare phase by spi clock this bits are combined with s
pi_mem_cs_setup bit..*/
#define SPI_MEM_SMEM_CS_SETUP_TIME    0x0000001F
#define SPI_MEM_SMEM_CS_SETUP_TIME_M  ((SPI_MEM_SMEM_CS_SETUP_TIME_V)<<(SPI_MEM_SMEM_CS_SETUP_TIME_S))
#define SPI_MEM_SMEM_CS_SETUP_TIME_V  0x1F
#define SPI_MEM_SMEM_CS_SETUP_TIME_S  2
/* SPI_MEM_SMEM_CS_HOLD : HRO ;bitpos:[1] ;default: 1'b0 ; */
/*description: For SPI0 and SPI1, spi cs keep low when spi is in done phase. 1: enable 0: disab
le..*/
#define SPI_MEM_SMEM_CS_HOLD    (BIT(1))
#define SPI_MEM_SMEM_CS_HOLD_M  (BIT(1))
#define SPI_MEM_SMEM_CS_HOLD_V  0x1
#define SPI_MEM_SMEM_CS_HOLD_S  1
/* SPI_MEM_SMEM_CS_SETUP : HRO ;bitpos:[0] ;default: 1'b0 ; */
/*description: For SPI0 and SPI1, spi cs is enable when spi is in prepare phase. 1: enable 0: d
isable..*/
#define SPI_MEM_SMEM_CS_SETUP    (BIT(0))
#define SPI_MEM_SMEM_CS_SETUP_M  (BIT(0))
#define SPI_MEM_SMEM_CS_SETUP_V  0x1
#define SPI_MEM_SMEM_CS_SETUP_S  0

#define SPI_MEM_CLOCK_GATE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x200)
/* SPI_MEM_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: Register clock gate enable signal. 1: Enable. 0: Disable..*/
#define SPI_MEM_CLK_EN    (BIT(0))
#define SPI_MEM_CLK_EN_M  (BIT(0))
#define SPI_MEM_CLK_EN_V  0x1
#define SPI_MEM_CLK_EN_S  0

#define SPI_MEM_MMU_ITEM_CONTENT_REG(i)          (REG_SPI_MEM_BASE(i) + 0x37C)
/* SPI_MEM_MMU_ITEM_CONTENT : R/W ;bitpos:[31:0] ;default: 32'h037c ; */
/*description: MSPI-MMU item content.*/
#define SPI_MEM_MMU_ITEM_CONTENT    0xFFFFFFFF
#define SPI_MEM_MMU_ITEM_CONTENT_M  ((SPI_MEM_MMU_ITEM_CONTENT_V)<<(SPI_MEM_MMU_ITEM_CONTENT_S))
#define SPI_MEM_MMU_ITEM_CONTENT_V  0xFFFFFFFF
#define SPI_MEM_MMU_ITEM_CONTENT_S  0

#define SPI_MEM_MMU_ITEM_INDEX_REG(i)          (REG_SPI_MEM_BASE(i) + 0x380)
/* SPI_MEM_MMU_ITEM_INDEX : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: MSPI-MMU item index.*/
#define SPI_MEM_MMU_ITEM_INDEX    0xFFFFFFFF
#define SPI_MEM_MMU_ITEM_INDEX_M  ((SPI_MEM_MMU_ITEM_INDEX_V)<<(SPI_MEM_MMU_ITEM_INDEX_S))
#define SPI_MEM_MMU_ITEM_INDEX_V  0xFFFFFFFF
#define SPI_MEM_MMU_ITEM_INDEX_S  0

#define SPI_MEM_MMU_POWER_CTRL_REG(i)          (REG_SPI_MEM_BASE(i) + 0x384)
/* SPI_MEM_RDN_RESULT : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: MSPI module clock domain and AXI clock domain ECO register result register.*/
#define SPI_MEM_RDN_RESULT    (BIT(31))
#define SPI_MEM_RDN_RESULT_M  (BIT(31))
#define SPI_MEM_RDN_RESULT_V  0x1
#define SPI_MEM_RDN_RESULT_S  31
/* SPI_MEM_RDN_ENA : HRO ;bitpos:[30] ;default: 1'b0 ; */
/*description: ECO register enable bit.*/
#define SPI_MEM_RDN_ENA    (BIT(30))
#define SPI_MEM_RDN_ENA_M  (BIT(30))
#define SPI_MEM_RDN_ENA_V  0x1
#define SPI_MEM_RDN_ENA_S  30
/* SPI_MEM_AUX_CTRL : HRO ;bitpos:[29:16] ;default: 14'h1320 ; */
/*description: MMU PSRAM aux control register.*/
#define SPI_MEM_AUX_CTRL    0x00003FFF
#define SPI_MEM_AUX_CTRL_M  ((SPI_MEM_AUX_CTRL_V)<<(SPI_MEM_AUX_CTRL_S))
#define SPI_MEM_AUX_CTRL_V  0x3FFF
#define SPI_MEM_AUX_CTRL_S  16
/* SPI_MEM_MMU_PAGE_SIZE : R/W ;bitpos:[4:3] ;default: 2'd0 ; */
/*description: 0: Max page size , 1: Max page size/2 , 2: Max page size/4, 3: Max page size/8.*/
#define SPI_MEM_MMU_PAGE_SIZE    0x00000003
#define SPI_MEM_MMU_PAGE_SIZE_M  ((SPI_MEM_MMU_PAGE_SIZE_V)<<(SPI_MEM_MMU_PAGE_SIZE_S))
#define SPI_MEM_MMU_PAGE_SIZE_V  0x3
#define SPI_MEM_MMU_PAGE_SIZE_S  3
/* SPI_MEM_MMU_MEM_FORCE_PU : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: Set this bit to force mmu-memory powerup, in this case, the power should also be
 controlled by rtc..*/
#define SPI_MEM_MMU_MEM_FORCE_PU    (BIT(2))
#define SPI_MEM_MMU_MEM_FORCE_PU_M  (BIT(2))
#define SPI_MEM_MMU_MEM_FORCE_PU_V  0x1
#define SPI_MEM_MMU_MEM_FORCE_PU_S  2
/* SPI_MEM_MMU_MEM_FORCE_PD : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to force mmu-memory powerdown.*/
#define SPI_MEM_MMU_MEM_FORCE_PD    (BIT(1))
#define SPI_MEM_MMU_MEM_FORCE_PD_M  (BIT(1))
#define SPI_MEM_MMU_MEM_FORCE_PD_V  0x1
#define SPI_MEM_MMU_MEM_FORCE_PD_S  1
/* SPI_MEM_MMU_MEM_FORCE_ON : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to enable mmu-memory clock force on.*/
#define SPI_MEM_MMU_MEM_FORCE_ON    (BIT(0))
#define SPI_MEM_MMU_MEM_FORCE_ON_M  (BIT(0))
#define SPI_MEM_MMU_MEM_FORCE_ON_V  0x1
#define SPI_MEM_MMU_MEM_FORCE_ON_S  0

#define SPI_MEM_REGISTERRND_ECO_HIGH_REG(i)          (REG_SPI_MEM_BASE(i) + 0x3F0)
/* SPI_MEM_REGISTERRND_ECO_HIGH : RO ;bitpos:[31:0] ;default: 32'h037c ; */
/*description: ECO high register.*/
#define SPI_MEM_REGISTERRND_ECO_HIGH    0xFFFFFFFF
#define SPI_MEM_REGISTERRND_ECO_HIGH_M  ((SPI_MEM_REGISTERRND_ECO_HIGH_V)<<(SPI_MEM_REGISTERRND_ECO_HIGH_S))
#define SPI_MEM_REGISTERRND_ECO_HIGH_V  0xFFFFFFFF
#define SPI_MEM_REGISTERRND_ECO_HIGH_S  0

#define SPI_MEM_REGISTERRND_ECO_LOW_REG(i)          (REG_SPI_MEM_BASE(i) + 0x3F4)
/* SPI_MEM_REGISTERRND_ECO_LOW : RO ;bitpos:[31:0] ;default: 32'h037c ; */
/*description: ECO low register.*/
#define SPI_MEM_REGISTERRND_ECO_LOW    0xFFFFFFFF
#define SPI_MEM_REGISTERRND_ECO_LOW_M  ((SPI_MEM_REGISTERRND_ECO_LOW_V)<<(SPI_MEM_REGISTERRND_ECO_LOW_S))
#define SPI_MEM_REGISTERRND_ECO_LOW_V  0xFFFFFFFF
#define SPI_MEM_REGISTERRND_ECO_LOW_S  0

#define SPI_MEM_DATE_REG(i)          (REG_SPI_MEM_BASE(i) + 0x3FC)
/* SPI_MEM_DATE : R/W ;bitpos:[27:0] ;default: 28'h2203030 ; */
/*description: SPI0 register version..*/
#define SPI_MEM_DATE    0x0FFFFFFF
#define SPI_MEM_DATE_M  ((SPI_MEM_DATE_V)<<(SPI_MEM_DATE_S))
#define SPI_MEM_DATE_V  0xFFFFFFF
#define SPI_MEM_DATE_S  0


#ifdef __cplusplus
}
#endif
