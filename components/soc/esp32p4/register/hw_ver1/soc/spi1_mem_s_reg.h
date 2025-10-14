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

/** SPI1_MEM_S_CMD_REG register
 *  SPI1 memory command register
 */
#define SPI1_MEM_S_CMD_REG (DR_REG_PSRAM_MSPI1_BASE + 0x0)
/** SPI1_MEM_S_MST_ST : RO; bitpos: [3:0]; default: 0;
 *  The current status of SPI1 master FSM.
 */
#define SPI1_MEM_S_MST_ST    0x0000000FU
#define SPI1_MEM_S_MST_ST_M  (SPI1_MEM_S_MST_ST_V << SPI1_MEM_S_MST_ST_S)
#define SPI1_MEM_S_MST_ST_V  0x0000000FU
#define SPI1_MEM_S_MST_ST_S  0
/** SPI1_MEM_S_SLV_ST : RO; bitpos: [7:4]; default: 0;
 *  The current status of SPI1 slave FSM: mspi_st. 0: idle state, 1: preparation state,
 *  2: send command state, 3: send address state, 4: wait state, 5: read data state,
 *  6:write data state, 7: done state, 8: read data end state.
 */
#define SPI1_MEM_S_SLV_ST    0x0000000FU
#define SPI1_MEM_S_SLV_ST_M  (SPI1_MEM_S_SLV_ST_V << SPI1_MEM_S_SLV_ST_S)
#define SPI1_MEM_S_SLV_ST_V  0x0000000FU
#define SPI1_MEM_S_SLV_ST_S  4
/** SPI1_MEM_S_FLASH_PE : R/W/SC; bitpos: [17]; default: 0;
 *  In user mode, it is set to indicate that program/erase operation will be triggered.
 *  The bit is combined with spi1_mem_s_usr bit. The bit will be cleared once the
 *  operation done.1: enable 0: disable.
 */
#define SPI1_MEM_S_FLASH_PE    (BIT(17))
#define SPI1_MEM_S_FLASH_PE_M  (SPI1_MEM_S_FLASH_PE_V << SPI1_MEM_S_FLASH_PE_S)
#define SPI1_MEM_S_FLASH_PE_V  0x00000001U
#define SPI1_MEM_S_FLASH_PE_S  17
/** SPI1_MEM_S_USR : R/W/SC; bitpos: [18]; default: 0;
 *  User define command enable.  An operation will be triggered when the bit is set.
 *  The bit will be cleared once the operation done.1: enable 0: disable.
 */
#define SPI1_MEM_S_USR    (BIT(18))
#define SPI1_MEM_S_USR_M  (SPI1_MEM_S_USR_V << SPI1_MEM_S_USR_S)
#define SPI1_MEM_S_USR_V  0x00000001U
#define SPI1_MEM_S_USR_S  18
/** SPI1_MEM_S_FLASH_HPM : R/W/SC; bitpos: [19]; default: 0;
 *  Drive Flash into high performance mode.  The bit will be cleared once the operation
 *  done.1: enable 0: disable.
 */
#define SPI1_MEM_S_FLASH_HPM    (BIT(19))
#define SPI1_MEM_S_FLASH_HPM_M  (SPI1_MEM_S_FLASH_HPM_V << SPI1_MEM_S_FLASH_HPM_S)
#define SPI1_MEM_S_FLASH_HPM_V  0x00000001U
#define SPI1_MEM_S_FLASH_HPM_S  19
/** SPI1_MEM_S_FLASH_RES : R/W/SC; bitpos: [20]; default: 0;
 *  This bit combined with reg_resandres bit releases Flash from the power-down state
 *  or high performance mode and obtains the devices ID. The bit will be cleared once
 *  the operation done.1: enable 0: disable.
 */
#define SPI1_MEM_S_FLASH_RES    (BIT(20))
#define SPI1_MEM_S_FLASH_RES_M  (SPI1_MEM_S_FLASH_RES_V << SPI1_MEM_S_FLASH_RES_S)
#define SPI1_MEM_S_FLASH_RES_V  0x00000001U
#define SPI1_MEM_S_FLASH_RES_S  20
/** SPI1_MEM_S_FLASH_DP : R/W/SC; bitpos: [21]; default: 0;
 *  Drive Flash into power down.  An operation will be triggered when the bit is set.
 *  The bit will be cleared once the operation done.1: enable 0: disable.
 */
#define SPI1_MEM_S_FLASH_DP    (BIT(21))
#define SPI1_MEM_S_FLASH_DP_M  (SPI1_MEM_S_FLASH_DP_V << SPI1_MEM_S_FLASH_DP_S)
#define SPI1_MEM_S_FLASH_DP_V  0x00000001U
#define SPI1_MEM_S_FLASH_DP_S  21
/** SPI1_MEM_S_FLASH_CE : R/W/SC; bitpos: [22]; default: 0;
 *  Chip erase enable. Chip erase operation will be triggered when the bit is set. The
 *  bit will be cleared once the operation done.1: enable 0: disable.
 */
#define SPI1_MEM_S_FLASH_CE    (BIT(22))
#define SPI1_MEM_S_FLASH_CE_M  (SPI1_MEM_S_FLASH_CE_V << SPI1_MEM_S_FLASH_CE_S)
#define SPI1_MEM_S_FLASH_CE_V  0x00000001U
#define SPI1_MEM_S_FLASH_CE_S  22
/** SPI1_MEM_S_FLASH_BE : R/W/SC; bitpos: [23]; default: 0;
 *  Block erase enable(32KB) .  Block erase operation will be triggered when the bit is
 *  set. The bit will be cleared once the operation done.1: enable 0: disable.
 */
#define SPI1_MEM_S_FLASH_BE    (BIT(23))
#define SPI1_MEM_S_FLASH_BE_M  (SPI1_MEM_S_FLASH_BE_V << SPI1_MEM_S_FLASH_BE_S)
#define SPI1_MEM_S_FLASH_BE_V  0x00000001U
#define SPI1_MEM_S_FLASH_BE_S  23
/** SPI1_MEM_S_FLASH_SE : R/W/SC; bitpos: [24]; default: 0;
 *  Sector erase enable(4KB). Sector erase operation will be triggered when the bit is
 *  set. The bit will be cleared once the operation done.1: enable 0: disable.
 */
#define SPI1_MEM_S_FLASH_SE    (BIT(24))
#define SPI1_MEM_S_FLASH_SE_M  (SPI1_MEM_S_FLASH_SE_V << SPI1_MEM_S_FLASH_SE_S)
#define SPI1_MEM_S_FLASH_SE_V  0x00000001U
#define SPI1_MEM_S_FLASH_SE_S  24
/** SPI1_MEM_S_FLASH_PP : R/W/SC; bitpos: [25]; default: 0;
 *  Page program enable(1 byte ~256 bytes data to be programmed). Page program
 *  operation  will be triggered when the bit is set. The bit will be cleared once the
 *  operation done .1: enable 0: disable.
 */
#define SPI1_MEM_S_FLASH_PP    (BIT(25))
#define SPI1_MEM_S_FLASH_PP_M  (SPI1_MEM_S_FLASH_PP_V << SPI1_MEM_S_FLASH_PP_S)
#define SPI1_MEM_S_FLASH_PP_V  0x00000001U
#define SPI1_MEM_S_FLASH_PP_S  25
/** SPI1_MEM_S_FLASH_WRSR : R/W/SC; bitpos: [26]; default: 0;
 *  Write status register enable.   Write status operation  will be triggered when the
 *  bit is set. The bit will be cleared once the operation done.1: enable 0: disable.
 */
#define SPI1_MEM_S_FLASH_WRSR    (BIT(26))
#define SPI1_MEM_S_FLASH_WRSR_M  (SPI1_MEM_S_FLASH_WRSR_V << SPI1_MEM_S_FLASH_WRSR_S)
#define SPI1_MEM_S_FLASH_WRSR_V  0x00000001U
#define SPI1_MEM_S_FLASH_WRSR_S  26
/** SPI1_MEM_S_FLASH_RDSR : R/W/SC; bitpos: [27]; default: 0;
 *  Read status register-1.  Read status operation will be triggered when the bit is
 *  set. The bit will be cleared once the operation done.1: enable 0: disable.
 */
#define SPI1_MEM_S_FLASH_RDSR    (BIT(27))
#define SPI1_MEM_S_FLASH_RDSR_M  (SPI1_MEM_S_FLASH_RDSR_V << SPI1_MEM_S_FLASH_RDSR_S)
#define SPI1_MEM_S_FLASH_RDSR_V  0x00000001U
#define SPI1_MEM_S_FLASH_RDSR_S  27
/** SPI1_MEM_S_FLASH_RDID : R/W/SC; bitpos: [28]; default: 0;
 *  Read JEDEC ID . Read ID command will be sent when the bit is set. The bit will be
 *  cleared once the operation done. 1: enable 0: disable.
 */
#define SPI1_MEM_S_FLASH_RDID    (BIT(28))
#define SPI1_MEM_S_FLASH_RDID_M  (SPI1_MEM_S_FLASH_RDID_V << SPI1_MEM_S_FLASH_RDID_S)
#define SPI1_MEM_S_FLASH_RDID_V  0x00000001U
#define SPI1_MEM_S_FLASH_RDID_S  28
/** SPI1_MEM_S_FLASH_WRDI : R/W/SC; bitpos: [29]; default: 0;
 *  Write flash disable. Write disable command will be sent when the bit is set. The
 *  bit will be cleared once the operation done. 1: enable 0: disable.
 */
#define SPI1_MEM_S_FLASH_WRDI    (BIT(29))
#define SPI1_MEM_S_FLASH_WRDI_M  (SPI1_MEM_S_FLASH_WRDI_V << SPI1_MEM_S_FLASH_WRDI_S)
#define SPI1_MEM_S_FLASH_WRDI_V  0x00000001U
#define SPI1_MEM_S_FLASH_WRDI_S  29
/** SPI1_MEM_S_FLASH_WREN : R/W/SC; bitpos: [30]; default: 0;
 *  Write flash enable.  Write enable command will be sent when the bit is set. The bit
 *  will be cleared once the operation done. 1: enable 0: disable.
 */
#define SPI1_MEM_S_FLASH_WREN    (BIT(30))
#define SPI1_MEM_S_FLASH_WREN_M  (SPI1_MEM_S_FLASH_WREN_V << SPI1_MEM_S_FLASH_WREN_S)
#define SPI1_MEM_S_FLASH_WREN_V  0x00000001U
#define SPI1_MEM_S_FLASH_WREN_S  30
/** SPI1_MEM_S_FLASH_READ : R/W/SC; bitpos: [31]; default: 0;
 *  Read flash enable. Read flash operation will be triggered when the bit is set. The
 *  bit will be cleared once the operation done. 1: enable 0: disable.
 */
#define SPI1_MEM_S_FLASH_READ    (BIT(31))
#define SPI1_MEM_S_FLASH_READ_M  (SPI1_MEM_S_FLASH_READ_V << SPI1_MEM_S_FLASH_READ_S)
#define SPI1_MEM_S_FLASH_READ_V  0x00000001U
#define SPI1_MEM_S_FLASH_READ_S  31

/** SPI1_MEM_S_ADDR_REG register
 *  SPI1 address register
 */
#define SPI1_MEM_S_ADDR_REG (DR_REG_PSRAM_MSPI1_BASE + 0x4)
/** SPI1_MEM_S_USR_ADDR_VALUE : R/W; bitpos: [31:0]; default: 0;
 *  In user mode, it is the memory address. other then the bit0-bit23 is the memory
 *  address, the bit24-bit31 are the byte length of a transfer.
 */
#define SPI1_MEM_S_USR_ADDR_VALUE    0xFFFFFFFFU
#define SPI1_MEM_S_USR_ADDR_VALUE_M  (SPI1_MEM_S_USR_ADDR_VALUE_V << SPI1_MEM_S_USR_ADDR_VALUE_S)
#define SPI1_MEM_S_USR_ADDR_VALUE_V  0xFFFFFFFFU
#define SPI1_MEM_S_USR_ADDR_VALUE_S  0

/** SPI1_MEM_S_CTRL_REG register
 *  SPI1 control register.
 */
#define SPI1_MEM_S_CTRL_REG (DR_REG_PSRAM_MSPI1_BASE + 0x8)
/** SPI1_MEM_S_FDUMMY_RIN : R/W; bitpos: [2]; default: 1;
 *  In the dummy phase of a MSPI read data transfer when accesses to flash, the signal
 *  level of SPI bus is output by the MSPI controller.
 */
#define SPI1_MEM_S_FDUMMY_RIN    (BIT(2))
#define SPI1_MEM_S_FDUMMY_RIN_M  (SPI1_MEM_S_FDUMMY_RIN_V << SPI1_MEM_S_FDUMMY_RIN_S)
#define SPI1_MEM_S_FDUMMY_RIN_V  0x00000001U
#define SPI1_MEM_S_FDUMMY_RIN_S  2
/** SPI1_MEM_S_FDUMMY_WOUT : R/W; bitpos: [3]; default: 1;
 *  In the dummy phase of a MSPI write data transfer when accesses to flash, the signal
 *  level of SPI bus is output by the MSPI controller.
 */
#define SPI1_MEM_S_FDUMMY_WOUT    (BIT(3))
#define SPI1_MEM_S_FDUMMY_WOUT_M  (SPI1_MEM_S_FDUMMY_WOUT_V << SPI1_MEM_S_FDUMMY_WOUT_S)
#define SPI1_MEM_S_FDUMMY_WOUT_V  0x00000001U
#define SPI1_MEM_S_FDUMMY_WOUT_S  3
/** SPI1_MEM_S_FDOUT_OCT : R/W; bitpos: [4]; default: 0;
 *  Apply 8 signals during write-data phase 1:enable 0: disable
 */
#define SPI1_MEM_S_FDOUT_OCT    (BIT(4))
#define SPI1_MEM_S_FDOUT_OCT_M  (SPI1_MEM_S_FDOUT_OCT_V << SPI1_MEM_S_FDOUT_OCT_S)
#define SPI1_MEM_S_FDOUT_OCT_V  0x00000001U
#define SPI1_MEM_S_FDOUT_OCT_S  4
/** SPI1_MEM_S_FDIN_OCT : R/W; bitpos: [5]; default: 0;
 *  Apply 8 signals during read-data phase 1:enable 0: disable
 */
#define SPI1_MEM_S_FDIN_OCT    (BIT(5))
#define SPI1_MEM_S_FDIN_OCT_M  (SPI1_MEM_S_FDIN_OCT_V << SPI1_MEM_S_FDIN_OCT_S)
#define SPI1_MEM_S_FDIN_OCT_V  0x00000001U
#define SPI1_MEM_S_FDIN_OCT_S  5
/** SPI1_MEM_S_FADDR_OCT : R/W; bitpos: [6]; default: 0;
 *  Apply 8 signals during address phase 1:enable 0: disable
 */
#define SPI1_MEM_S_FADDR_OCT    (BIT(6))
#define SPI1_MEM_S_FADDR_OCT_M  (SPI1_MEM_S_FADDR_OCT_V << SPI1_MEM_S_FADDR_OCT_S)
#define SPI1_MEM_S_FADDR_OCT_V  0x00000001U
#define SPI1_MEM_S_FADDR_OCT_S  6
/** SPI1_MEM_S_FCMD_QUAD : R/W; bitpos: [8]; default: 0;
 *  Apply 4 signals during command phase 1:enable 0: disable
 */
#define SPI1_MEM_S_FCMD_QUAD    (BIT(8))
#define SPI1_MEM_S_FCMD_QUAD_M  (SPI1_MEM_S_FCMD_QUAD_V << SPI1_MEM_S_FCMD_QUAD_S)
#define SPI1_MEM_S_FCMD_QUAD_V  0x00000001U
#define SPI1_MEM_S_FCMD_QUAD_S  8
/** SPI1_MEM_S_FCMD_OCT : R/W; bitpos: [9]; default: 0;
 *  Apply 8 signals during command phase 1:enable 0: disable
 */
#define SPI1_MEM_S_FCMD_OCT    (BIT(9))
#define SPI1_MEM_S_FCMD_OCT_M  (SPI1_MEM_S_FCMD_OCT_V << SPI1_MEM_S_FCMD_OCT_S)
#define SPI1_MEM_S_FCMD_OCT_V  0x00000001U
#define SPI1_MEM_S_FCMD_OCT_S  9
/** SPI1_MEM_S_FCS_CRC_EN : R/W; bitpos: [10]; default: 0;
 *  For SPI1,  initialize crc32 module before writing encrypted data to flash. Active
 *  low.
 */
#define SPI1_MEM_S_FCS_CRC_EN    (BIT(10))
#define SPI1_MEM_S_FCS_CRC_EN_M  (SPI1_MEM_S_FCS_CRC_EN_V << SPI1_MEM_S_FCS_CRC_EN_S)
#define SPI1_MEM_S_FCS_CRC_EN_V  0x00000001U
#define SPI1_MEM_S_FCS_CRC_EN_S  10
/** SPI1_MEM_S_TX_CRC_EN : R/W; bitpos: [11]; default: 0;
 *  For SPI1,  enable crc32 when writing encrypted data to flash. 1: enable 0:disable
 */
#define SPI1_MEM_S_TX_CRC_EN    (BIT(11))
#define SPI1_MEM_S_TX_CRC_EN_M  (SPI1_MEM_S_TX_CRC_EN_V << SPI1_MEM_S_TX_CRC_EN_S)
#define SPI1_MEM_S_TX_CRC_EN_V  0x00000001U
#define SPI1_MEM_S_TX_CRC_EN_S  11
/** SPI1_MEM_S_FASTRD_MODE : R/W; bitpos: [13]; default: 1;
 *  This bit enable the bits: spi1_mem_s_fread_qio, spi1_mem_s_fread_dio, spi1_mem_s_fread_qout
 *  and spi1_mem_s_fread_dout. 1: enable 0: disable.
 */
#define SPI1_MEM_S_FASTRD_MODE    (BIT(13))
#define SPI1_MEM_S_FASTRD_MODE_M  (SPI1_MEM_S_FASTRD_MODE_V << SPI1_MEM_S_FASTRD_MODE_S)
#define SPI1_MEM_S_FASTRD_MODE_V  0x00000001U
#define SPI1_MEM_S_FASTRD_MODE_S  13
/** SPI1_MEM_S_FREAD_DUAL : R/W; bitpos: [14]; default: 0;
 *  In the read operations, read-data phase apply 2 signals. 1: enable 0: disable.
 */
#define SPI1_MEM_S_FREAD_DUAL    (BIT(14))
#define SPI1_MEM_S_FREAD_DUAL_M  (SPI1_MEM_S_FREAD_DUAL_V << SPI1_MEM_S_FREAD_DUAL_S)
#define SPI1_MEM_S_FREAD_DUAL_V  0x00000001U
#define SPI1_MEM_S_FREAD_DUAL_S  14
/** SPI1_MEM_S_RESANDRES : R/W; bitpos: [15]; default: 1;
 *  The Device ID is read out to SPI1_MEM_S_RD_STATUS register,  this bit combine with
 *  spi1_mem_s_flash_res bit. 1: enable 0: disable.
 */
#define SPI1_MEM_S_RESANDRES    (BIT(15))
#define SPI1_MEM_S_RESANDRES_M  (SPI1_MEM_S_RESANDRES_V << SPI1_MEM_S_RESANDRES_S)
#define SPI1_MEM_S_RESANDRES_V  0x00000001U
#define SPI1_MEM_S_RESANDRES_S  15
/** SPI1_MEM_S_Q_POL : R/W; bitpos: [18]; default: 1;
 *  The bit is used to set MISO line polarity, 1: high 0, low
 */
#define SPI1_MEM_S_Q_POL    (BIT(18))
#define SPI1_MEM_S_Q_POL_M  (SPI1_MEM_S_Q_POL_V << SPI1_MEM_S_Q_POL_S)
#define SPI1_MEM_S_Q_POL_V  0x00000001U
#define SPI1_MEM_S_Q_POL_S  18
/** SPI1_MEM_S_D_POL : R/W; bitpos: [19]; default: 1;
 *  The bit is used to set MOSI line polarity, 1: high 0, low
 */
#define SPI1_MEM_S_D_POL    (BIT(19))
#define SPI1_MEM_S_D_POL_M  (SPI1_MEM_S_D_POL_V << SPI1_MEM_S_D_POL_S)
#define SPI1_MEM_S_D_POL_V  0x00000001U
#define SPI1_MEM_S_D_POL_S  19
/** SPI1_MEM_S_FREAD_QUAD : R/W; bitpos: [20]; default: 0;
 *  In the read operations read-data phase apply 4 signals. 1: enable 0: disable.
 */
#define SPI1_MEM_S_FREAD_QUAD    (BIT(20))
#define SPI1_MEM_S_FREAD_QUAD_M  (SPI1_MEM_S_FREAD_QUAD_V << SPI1_MEM_S_FREAD_QUAD_S)
#define SPI1_MEM_S_FREAD_QUAD_V  0x00000001U
#define SPI1_MEM_S_FREAD_QUAD_S  20
/** SPI1_MEM_S_WP_REG : R/W; bitpos: [21]; default: 1;
 *  Write protect signal output when SPI is idle.  1: output high, 0: output low.
 */
#define SPI1_MEM_S_WP_REG    (BIT(21))
#define SPI1_MEM_S_WP_REG_M  (SPI1_MEM_S_WP_REG_V << SPI1_MEM_S_WP_REG_S)
#define SPI1_MEM_S_WP_REG_V  0x00000001U
#define SPI1_MEM_S_WP_REG_S  21
/** SPI1_MEM_S_WRSR_2B : R/W; bitpos: [22]; default: 0;
 *  two bytes data will be written to status register when it is set. 1: enable 0:
 *  disable.
 */
#define SPI1_MEM_S_WRSR_2B    (BIT(22))
#define SPI1_MEM_S_WRSR_2B_M  (SPI1_MEM_S_WRSR_2B_V << SPI1_MEM_S_WRSR_2B_S)
#define SPI1_MEM_S_WRSR_2B_V  0x00000001U
#define SPI1_MEM_S_WRSR_2B_S  22
/** SPI1_MEM_S_FREAD_DIO : R/W; bitpos: [23]; default: 0;
 *  In the read operations address phase and read-data phase apply 2 signals. 1: enable
 *  0: disable.
 */
#define SPI1_MEM_S_FREAD_DIO    (BIT(23))
#define SPI1_MEM_S_FREAD_DIO_M  (SPI1_MEM_S_FREAD_DIO_V << SPI1_MEM_S_FREAD_DIO_S)
#define SPI1_MEM_S_FREAD_DIO_V  0x00000001U
#define SPI1_MEM_S_FREAD_DIO_S  23
/** SPI1_MEM_S_FREAD_QIO : R/W; bitpos: [24]; default: 0;
 *  In the read operations address phase and read-data phase apply 4 signals. 1: enable
 *  0: disable.
 */
#define SPI1_MEM_S_FREAD_QIO    (BIT(24))
#define SPI1_MEM_S_FREAD_QIO_M  (SPI1_MEM_S_FREAD_QIO_V << SPI1_MEM_S_FREAD_QIO_S)
#define SPI1_MEM_S_FREAD_QIO_V  0x00000001U
#define SPI1_MEM_S_FREAD_QIO_S  24

/** SPI1_MEM_S_CTRL1_REG register
 *  SPI1 control1 register.
 */
#define SPI1_MEM_S_CTRL1_REG (DR_REG_PSRAM_MSPI1_BASE + 0xc)
/** SPI1_MEM_S_CLK_MODE : R/W; bitpos: [1:0]; default: 0;
 *  SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delayed
 *  one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3:
 *  SPI clock is always on.
 */
#define SPI1_MEM_S_CLK_MODE    0x00000003U
#define SPI1_MEM_S_CLK_MODE_M  (SPI1_MEM_S_CLK_MODE_V << SPI1_MEM_S_CLK_MODE_S)
#define SPI1_MEM_S_CLK_MODE_V  0x00000003U
#define SPI1_MEM_S_CLK_MODE_S  0
/** SPI1_MEM_S_CS_HOLD_DLY_RES : R/W; bitpos: [11:2]; default: 1023;
 *  After RES/DP/HPM command is sent, SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 512)
 *  SPI_CLK cycles.
 */
#define SPI1_MEM_S_CS_HOLD_DLY_RES    0x000003FFU
#define SPI1_MEM_S_CS_HOLD_DLY_RES_M  (SPI1_MEM_S_CS_HOLD_DLY_RES_V << SPI1_MEM_S_CS_HOLD_DLY_RES_S)
#define SPI1_MEM_S_CS_HOLD_DLY_RES_V  0x000003FFU
#define SPI1_MEM_S_CS_HOLD_DLY_RES_S  2

/** SPI1_MEM_S_CTRL2_REG register
 *  SPI1 control2 register.
 */
#define SPI1_MEM_S_CTRL2_REG (DR_REG_PSRAM_MSPI1_BASE + 0x10)
/** SPI1_MEM_S_SYNC_RESET : WT; bitpos: [31]; default: 0;
 *  The FSM will be reset.
 */
#define SPI1_MEM_S_SYNC_RESET    (BIT(31))
#define SPI1_MEM_S_SYNC_RESET_M  (SPI1_MEM_S_SYNC_RESET_V << SPI1_MEM_S_SYNC_RESET_S)
#define SPI1_MEM_S_SYNC_RESET_V  0x00000001U
#define SPI1_MEM_S_SYNC_RESET_S  31

/** SPI1_MEM_S_CLOCK_REG register
 *  SPI1 clock division control register.
 */
#define SPI1_MEM_S_CLOCK_REG (DR_REG_PSRAM_MSPI1_BASE + 0x14)
/** SPI1_MEM_S_CLKCNT_L : R/W; bitpos: [7:0]; default: 3;
 *  In the master mode it must be equal to spi1_mem_s_clkcnt_N.
 */
#define SPI1_MEM_S_CLKCNT_L    0x000000FFU
#define SPI1_MEM_S_CLKCNT_L_M  (SPI1_MEM_S_CLKCNT_L_V << SPI1_MEM_S_CLKCNT_L_S)
#define SPI1_MEM_S_CLKCNT_L_V  0x000000FFU
#define SPI1_MEM_S_CLKCNT_L_S  0
/** SPI1_MEM_S_CLKCNT_H : R/W; bitpos: [15:8]; default: 1;
 *  In the master mode it must be floor((spi1_mem_s_clkcnt_N+1)/2-1).
 */
#define SPI1_MEM_S_CLKCNT_H    0x000000FFU
#define SPI1_MEM_S_CLKCNT_H_M  (SPI1_MEM_S_CLKCNT_H_V << SPI1_MEM_S_CLKCNT_H_S)
#define SPI1_MEM_S_CLKCNT_H_V  0x000000FFU
#define SPI1_MEM_S_CLKCNT_H_S  8
/** SPI1_MEM_S_CLKCNT_N : R/W; bitpos: [23:16]; default: 3;
 *  In the master mode it is the divider of spi1_mem_s_clk. So spi1_mem_s_clk frequency is
 *  system/(spi1_mem_s_clkcnt_N+1)
 */
#define SPI1_MEM_S_CLKCNT_N    0x000000FFU
#define SPI1_MEM_S_CLKCNT_N_M  (SPI1_MEM_S_CLKCNT_N_V << SPI1_MEM_S_CLKCNT_N_S)
#define SPI1_MEM_S_CLKCNT_N_V  0x000000FFU
#define SPI1_MEM_S_CLKCNT_N_S  16
/** SPI1_MEM_S_CLK_EQU_SYSCLK : R/W; bitpos: [31]; default: 0;
 *  reserved
 */
#define SPI1_MEM_S_CLK_EQU_SYSCLK    (BIT(31))
#define SPI1_MEM_S_CLK_EQU_SYSCLK_M  (SPI1_MEM_S_CLK_EQU_SYSCLK_V << SPI1_MEM_S_CLK_EQU_SYSCLK_S)
#define SPI1_MEM_S_CLK_EQU_SYSCLK_V  0x00000001U
#define SPI1_MEM_S_CLK_EQU_SYSCLK_S  31

/** SPI1_MEM_S_USER_REG register
 *  SPI1 user register.
 */
#define SPI1_MEM_S_USER_REG (DR_REG_PSRAM_MSPI1_BASE + 0x18)
/** SPI1_MEM_S_CK_OUT_EDGE : R/W; bitpos: [9]; default: 0;
 *  the bit combined with spi1_mem_s_mosi_delay_mode bits to set mosi signal delay mode.
 */
#define SPI1_MEM_S_CK_OUT_EDGE    (BIT(9))
#define SPI1_MEM_S_CK_OUT_EDGE_M  (SPI1_MEM_S_CK_OUT_EDGE_V << SPI1_MEM_S_CK_OUT_EDGE_S)
#define SPI1_MEM_S_CK_OUT_EDGE_V  0x00000001U
#define SPI1_MEM_S_CK_OUT_EDGE_S  9
/** SPI1_MEM_S_FWRITE_DUAL : R/W; bitpos: [12]; default: 0;
 *  In the write operations read-data phase apply 2 signals
 */
#define SPI1_MEM_S_FWRITE_DUAL    (BIT(12))
#define SPI1_MEM_S_FWRITE_DUAL_M  (SPI1_MEM_S_FWRITE_DUAL_V << SPI1_MEM_S_FWRITE_DUAL_S)
#define SPI1_MEM_S_FWRITE_DUAL_V  0x00000001U
#define SPI1_MEM_S_FWRITE_DUAL_S  12
/** SPI1_MEM_S_FWRITE_QUAD : R/W; bitpos: [13]; default: 0;
 *  In the write operations read-data phase apply 4 signals
 */
#define SPI1_MEM_S_FWRITE_QUAD    (BIT(13))
#define SPI1_MEM_S_FWRITE_QUAD_M  (SPI1_MEM_S_FWRITE_QUAD_V << SPI1_MEM_S_FWRITE_QUAD_S)
#define SPI1_MEM_S_FWRITE_QUAD_V  0x00000001U
#define SPI1_MEM_S_FWRITE_QUAD_S  13
/** SPI1_MEM_S_FWRITE_DIO : R/W; bitpos: [14]; default: 0;
 *  In the write operations address phase and read-data phase apply 2 signals.
 */
#define SPI1_MEM_S_FWRITE_DIO    (BIT(14))
#define SPI1_MEM_S_FWRITE_DIO_M  (SPI1_MEM_S_FWRITE_DIO_V << SPI1_MEM_S_FWRITE_DIO_S)
#define SPI1_MEM_S_FWRITE_DIO_V  0x00000001U
#define SPI1_MEM_S_FWRITE_DIO_S  14
/** SPI1_MEM_S_FWRITE_QIO : R/W; bitpos: [15]; default: 0;
 *  In the write operations address phase and read-data phase apply 4 signals.
 */
#define SPI1_MEM_S_FWRITE_QIO    (BIT(15))
#define SPI1_MEM_S_FWRITE_QIO_M  (SPI1_MEM_S_FWRITE_QIO_V << SPI1_MEM_S_FWRITE_QIO_S)
#define SPI1_MEM_S_FWRITE_QIO_V  0x00000001U
#define SPI1_MEM_S_FWRITE_QIO_S  15
/** SPI1_MEM_S_USR_MISO_HIGHPART : R/W; bitpos: [24]; default: 0;
 *  read-data phase only access to high-part of the buffer spi1_mem_s_w8~spi1_mem_s_w15. 1:
 *  enable 0: disable.
 */
#define SPI1_MEM_S_USR_MISO_HIGHPART    (BIT(24))
#define SPI1_MEM_S_USR_MISO_HIGHPART_M  (SPI1_MEM_S_USR_MISO_HIGHPART_V << SPI1_MEM_S_USR_MISO_HIGHPART_S)
#define SPI1_MEM_S_USR_MISO_HIGHPART_V  0x00000001U
#define SPI1_MEM_S_USR_MISO_HIGHPART_S  24
/** SPI1_MEM_S_USR_MOSI_HIGHPART : R/W; bitpos: [25]; default: 0;
 *  write-data phase only access to high-part of the buffer spi1_mem_s_w8~spi1_mem_s_w15. 1:
 *  enable 0: disable.
 */
#define SPI1_MEM_S_USR_MOSI_HIGHPART    (BIT(25))
#define SPI1_MEM_S_USR_MOSI_HIGHPART_M  (SPI1_MEM_S_USR_MOSI_HIGHPART_V << SPI1_MEM_S_USR_MOSI_HIGHPART_S)
#define SPI1_MEM_S_USR_MOSI_HIGHPART_V  0x00000001U
#define SPI1_MEM_S_USR_MOSI_HIGHPART_S  25
/** SPI1_MEM_S_USR_DUMMY_IDLE : R/W; bitpos: [26]; default: 0;
 *  SPI clock is disable in dummy phase when the bit is enable.
 */
#define SPI1_MEM_S_USR_DUMMY_IDLE    (BIT(26))
#define SPI1_MEM_S_USR_DUMMY_IDLE_M  (SPI1_MEM_S_USR_DUMMY_IDLE_V << SPI1_MEM_S_USR_DUMMY_IDLE_S)
#define SPI1_MEM_S_USR_DUMMY_IDLE_V  0x00000001U
#define SPI1_MEM_S_USR_DUMMY_IDLE_S  26
/** SPI1_MEM_S_USR_MOSI : R/W; bitpos: [27]; default: 0;
 *  This bit enable the write-data phase of an operation.
 */
#define SPI1_MEM_S_USR_MOSI    (BIT(27))
#define SPI1_MEM_S_USR_MOSI_M  (SPI1_MEM_S_USR_MOSI_V << SPI1_MEM_S_USR_MOSI_S)
#define SPI1_MEM_S_USR_MOSI_V  0x00000001U
#define SPI1_MEM_S_USR_MOSI_S  27
/** SPI1_MEM_S_USR_MISO : R/W; bitpos: [28]; default: 0;
 *  This bit enable the read-data phase of an operation.
 */
#define SPI1_MEM_S_USR_MISO    (BIT(28))
#define SPI1_MEM_S_USR_MISO_M  (SPI1_MEM_S_USR_MISO_V << SPI1_MEM_S_USR_MISO_S)
#define SPI1_MEM_S_USR_MISO_V  0x00000001U
#define SPI1_MEM_S_USR_MISO_S  28
/** SPI1_MEM_S_USR_DUMMY : R/W; bitpos: [29]; default: 0;
 *  This bit enable the dummy phase of an operation.
 */
#define SPI1_MEM_S_USR_DUMMY    (BIT(29))
#define SPI1_MEM_S_USR_DUMMY_M  (SPI1_MEM_S_USR_DUMMY_V << SPI1_MEM_S_USR_DUMMY_S)
#define SPI1_MEM_S_USR_DUMMY_V  0x00000001U
#define SPI1_MEM_S_USR_DUMMY_S  29
/** SPI1_MEM_S_USR_ADDR : R/W; bitpos: [30]; default: 0;
 *  This bit enable the address phase of an operation.
 */
#define SPI1_MEM_S_USR_ADDR    (BIT(30))
#define SPI1_MEM_S_USR_ADDR_M  (SPI1_MEM_S_USR_ADDR_V << SPI1_MEM_S_USR_ADDR_S)
#define SPI1_MEM_S_USR_ADDR_V  0x00000001U
#define SPI1_MEM_S_USR_ADDR_S  30
/** SPI1_MEM_S_USR_COMMAND : R/W; bitpos: [31]; default: 1;
 *  This bit enable the command phase of an operation.
 */
#define SPI1_MEM_S_USR_COMMAND    (BIT(31))
#define SPI1_MEM_S_USR_COMMAND_M  (SPI1_MEM_S_USR_COMMAND_V << SPI1_MEM_S_USR_COMMAND_S)
#define SPI1_MEM_S_USR_COMMAND_V  0x00000001U
#define SPI1_MEM_S_USR_COMMAND_S  31

/** SPI1_MEM_S_USER1_REG register
 *  SPI1 user1 register.
 */
#define SPI1_MEM_S_USER1_REG (DR_REG_PSRAM_MSPI1_BASE + 0x1c)
/** SPI1_MEM_S_USR_DUMMY_CYCLELEN : R/W; bitpos: [5:0]; default: 7;
 *  The length in spi1_mem_s_clk cycles of dummy phase. The register value shall be
 *  (cycle_num-1).
 */
#define SPI1_MEM_S_USR_DUMMY_CYCLELEN    0x0000003FU
#define SPI1_MEM_S_USR_DUMMY_CYCLELEN_M  (SPI1_MEM_S_USR_DUMMY_CYCLELEN_V << SPI1_MEM_S_USR_DUMMY_CYCLELEN_S)
#define SPI1_MEM_S_USR_DUMMY_CYCLELEN_V  0x0000003FU
#define SPI1_MEM_S_USR_DUMMY_CYCLELEN_S  0
/** SPI1_MEM_S_USR_ADDR_BITLEN : R/W; bitpos: [31:26]; default: 23;
 *  The length in bits of address phase. The register value shall be (bit_num-1).
 */
#define SPI1_MEM_S_USR_ADDR_BITLEN    0x0000003FU
#define SPI1_MEM_S_USR_ADDR_BITLEN_M  (SPI1_MEM_S_USR_ADDR_BITLEN_V << SPI1_MEM_S_USR_ADDR_BITLEN_S)
#define SPI1_MEM_S_USR_ADDR_BITLEN_V  0x0000003FU
#define SPI1_MEM_S_USR_ADDR_BITLEN_S  26

/** SPI1_MEM_S_USER2_REG register
 *  SPI1 user2 register.
 */
#define SPI1_MEM_S_USER2_REG (DR_REG_PSRAM_MSPI1_BASE + 0x20)
/** SPI1_MEM_S_USR_COMMAND_VALUE : R/W; bitpos: [15:0]; default: 0;
 *  The value of  command.
 */
#define SPI1_MEM_S_USR_COMMAND_VALUE    0x0000FFFFU
#define SPI1_MEM_S_USR_COMMAND_VALUE_M  (SPI1_MEM_S_USR_COMMAND_VALUE_V << SPI1_MEM_S_USR_COMMAND_VALUE_S)
#define SPI1_MEM_S_USR_COMMAND_VALUE_V  0x0000FFFFU
#define SPI1_MEM_S_USR_COMMAND_VALUE_S  0
/** SPI1_MEM_S_USR_COMMAND_BITLEN : R/W; bitpos: [31:28]; default: 7;
 *  The length in bits of command phase. The register value shall be (bit_num-1)
 */
#define SPI1_MEM_S_USR_COMMAND_BITLEN    0x0000000FU
#define SPI1_MEM_S_USR_COMMAND_BITLEN_M  (SPI1_MEM_S_USR_COMMAND_BITLEN_V << SPI1_MEM_S_USR_COMMAND_BITLEN_S)
#define SPI1_MEM_S_USR_COMMAND_BITLEN_V  0x0000000FU
#define SPI1_MEM_S_USR_COMMAND_BITLEN_S  28

/** SPI1_MEM_S_MOSI_DLEN_REG register
 *  SPI1 send data bit length control register.
 */
#define SPI1_MEM_S_MOSI_DLEN_REG (DR_REG_PSRAM_MSPI1_BASE + 0x24)
/** SPI1_MEM_S_USR_MOSI_DBITLEN : R/W; bitpos: [9:0]; default: 0;
 *  The length in bits of write-data. The register value shall be (bit_num-1).
 */
#define SPI1_MEM_S_USR_MOSI_DBITLEN    0x000003FFU
#define SPI1_MEM_S_USR_MOSI_DBITLEN_M  (SPI1_MEM_S_USR_MOSI_DBITLEN_V << SPI1_MEM_S_USR_MOSI_DBITLEN_S)
#define SPI1_MEM_S_USR_MOSI_DBITLEN_V  0x000003FFU
#define SPI1_MEM_S_USR_MOSI_DBITLEN_S  0

/** SPI1_MEM_S_MISO_DLEN_REG register
 *  SPI1 receive data bit length control register.
 */
#define SPI1_MEM_S_MISO_DLEN_REG (DR_REG_PSRAM_MSPI1_BASE + 0x28)
/** SPI1_MEM_S_USR_MISO_DBITLEN : R/W; bitpos: [9:0]; default: 0;
 *  The length in bits of  read-data. The register value shall be (bit_num-1).
 */
#define SPI1_MEM_S_USR_MISO_DBITLEN    0x000003FFU
#define SPI1_MEM_S_USR_MISO_DBITLEN_M  (SPI1_MEM_S_USR_MISO_DBITLEN_V << SPI1_MEM_S_USR_MISO_DBITLEN_S)
#define SPI1_MEM_S_USR_MISO_DBITLEN_V  0x000003FFU
#define SPI1_MEM_S_USR_MISO_DBITLEN_S  0

/** SPI1_MEM_S_RD_STATUS_REG register
 *  SPI1 status register.
 */
#define SPI1_MEM_S_RD_STATUS_REG (DR_REG_PSRAM_MSPI1_BASE + 0x2c)
/** SPI1_MEM_S_STATUS : R/W/SS; bitpos: [15:0]; default: 0;
 *  The value is stored when set spi1_mem_s_flash_rdsr bit and spi1_mem_s_flash_res bit.
 */
#define SPI1_MEM_S_STATUS    0x0000FFFFU
#define SPI1_MEM_S_STATUS_M  (SPI1_MEM_S_STATUS_V << SPI1_MEM_S_STATUS_S)
#define SPI1_MEM_S_STATUS_V  0x0000FFFFU
#define SPI1_MEM_S_STATUS_S  0
/** SPI1_MEM_S_WB_MODE : R/W; bitpos: [23:16]; default: 0;
 *  Mode bits in the flash fast read mode  it is combined with spi1_mem_s_fastrd_mode bit.
 */
#define SPI1_MEM_S_WB_MODE    0x000000FFU
#define SPI1_MEM_S_WB_MODE_M  (SPI1_MEM_S_WB_MODE_V << SPI1_MEM_S_WB_MODE_S)
#define SPI1_MEM_S_WB_MODE_V  0x000000FFU
#define SPI1_MEM_S_WB_MODE_S  16

/** SPI1_MEM_S_MISC_REG register
 *  SPI1 misc register
 */
#define SPI1_MEM_S_MISC_REG (DR_REG_PSRAM_MSPI1_BASE + 0x34)
/** SPI1_MEM_S_CS0_DIS : R/W; bitpos: [0]; default: 0;
 *  SPI_CS0 pin enable, 1: disable SPI_CS0, 0: SPI_CS0 pin is active to select SPI
 *  device, such as flash, external RAM and so on.
 */
#define SPI1_MEM_S_CS0_DIS    (BIT(0))
#define SPI1_MEM_S_CS0_DIS_M  (SPI1_MEM_S_CS0_DIS_V << SPI1_MEM_S_CS0_DIS_S)
#define SPI1_MEM_S_CS0_DIS_V  0x00000001U
#define SPI1_MEM_S_CS0_DIS_S  0
/** SPI1_MEM_S_CS1_DIS : R/W; bitpos: [1]; default: 1;
 *  SPI_CS1 pin enable, 1: disable SPI_CS1, 0: SPI_CS1 pin is active to select SPI
 *  device, such as flash, external RAM and so on.
 */
#define SPI1_MEM_S_CS1_DIS    (BIT(1))
#define SPI1_MEM_S_CS1_DIS_M  (SPI1_MEM_S_CS1_DIS_V << SPI1_MEM_S_CS1_DIS_S)
#define SPI1_MEM_S_CS1_DIS_V  0x00000001U
#define SPI1_MEM_S_CS1_DIS_S  1
/** SPI1_MEM_S_CK_IDLE_EDGE : R/W; bitpos: [9]; default: 0;
 *  1: spi clk line is high when idle     0: spi clk line is low when idle
 */
#define SPI1_MEM_S_CK_IDLE_EDGE    (BIT(9))
#define SPI1_MEM_S_CK_IDLE_EDGE_M  (SPI1_MEM_S_CK_IDLE_EDGE_V << SPI1_MEM_S_CK_IDLE_EDGE_S)
#define SPI1_MEM_S_CK_IDLE_EDGE_V  0x00000001U
#define SPI1_MEM_S_CK_IDLE_EDGE_S  9
/** SPI1_MEM_S_CS_KEEP_ACTIVE : R/W; bitpos: [10]; default: 0;
 *  spi cs line keep low when the bit is set.
 */
#define SPI1_MEM_S_CS_KEEP_ACTIVE    (BIT(10))
#define SPI1_MEM_S_CS_KEEP_ACTIVE_M  (SPI1_MEM_S_CS_KEEP_ACTIVE_V << SPI1_MEM_S_CS_KEEP_ACTIVE_S)
#define SPI1_MEM_S_CS_KEEP_ACTIVE_V  0x00000001U
#define SPI1_MEM_S_CS_KEEP_ACTIVE_S  10

/** SPI1_MEM_S_TX_CRC_REG register
 *  SPI1 TX CRC data register.
 */
#define SPI1_MEM_S_TX_CRC_REG (DR_REG_PSRAM_MSPI1_BASE + 0x38)
/** SPI1_MEM_S_TX_CRC_DATA : RO; bitpos: [31:0]; default: 4294967295;
 *  For SPI1, the value of crc32.
 */
#define SPI1_MEM_S_TX_CRC_DATA    0xFFFFFFFFU
#define SPI1_MEM_S_TX_CRC_DATA_M  (SPI1_MEM_S_TX_CRC_DATA_V << SPI1_MEM_S_TX_CRC_DATA_S)
#define SPI1_MEM_S_TX_CRC_DATA_V  0xFFFFFFFFU
#define SPI1_MEM_S_TX_CRC_DATA_S  0

/** SPI1_MEM_S_CACHE_FCTRL_REG register
 *  SPI1 bit mode control register.
 */
#define SPI1_MEM_S_CACHE_FCTRL_REG (DR_REG_PSRAM_MSPI1_BASE + 0x3c)
/** SPI1_MEM_S_CACHE_USR_ADDR_4BYTE : R/W; bitpos: [1]; default: 0;
 *  For SPI1,  cache  read flash with 4 bytes address, 1: enable, 0:disable.
 */
#define SPI1_MEM_S_CACHE_USR_ADDR_4BYTE    (BIT(1))
#define SPI1_MEM_S_CACHE_USR_ADDR_4BYTE_M  (SPI1_MEM_S_CACHE_USR_ADDR_4BYTE_V << SPI1_MEM_S_CACHE_USR_ADDR_4BYTE_S)
#define SPI1_MEM_S_CACHE_USR_ADDR_4BYTE_V  0x00000001U
#define SPI1_MEM_S_CACHE_USR_ADDR_4BYTE_S  1
/** SPI1_MEM_S_FDIN_DUAL : R/W; bitpos: [3]; default: 0;
 *  For SPI1, din phase apply 2 signals. 1: enable 0: disable. The bit is the same with
 *  spi1_mem_s_fread_dio.
 */
#define SPI1_MEM_S_FDIN_DUAL    (BIT(3))
#define SPI1_MEM_S_FDIN_DUAL_M  (SPI1_MEM_S_FDIN_DUAL_V << SPI1_MEM_S_FDIN_DUAL_S)
#define SPI1_MEM_S_FDIN_DUAL_V  0x00000001U
#define SPI1_MEM_S_FDIN_DUAL_S  3
/** SPI1_MEM_S_FDOUT_DUAL : R/W; bitpos: [4]; default: 0;
 *  For SPI1, dout phase apply 2 signals. 1: enable 0: disable. The bit is the same
 *  with spi1_mem_s_fread_dio.
 */
#define SPI1_MEM_S_FDOUT_DUAL    (BIT(4))
#define SPI1_MEM_S_FDOUT_DUAL_M  (SPI1_MEM_S_FDOUT_DUAL_V << SPI1_MEM_S_FDOUT_DUAL_S)
#define SPI1_MEM_S_FDOUT_DUAL_V  0x00000001U
#define SPI1_MEM_S_FDOUT_DUAL_S  4
/** SPI1_MEM_S_FADDR_DUAL : R/W; bitpos: [5]; default: 0;
 *  For SPI1, address phase apply 2 signals. 1: enable 0: disable.  The bit is the same
 *  with spi1_mem_s_fread_dio.
 */
#define SPI1_MEM_S_FADDR_DUAL    (BIT(5))
#define SPI1_MEM_S_FADDR_DUAL_M  (SPI1_MEM_S_FADDR_DUAL_V << SPI1_MEM_S_FADDR_DUAL_S)
#define SPI1_MEM_S_FADDR_DUAL_V  0x00000001U
#define SPI1_MEM_S_FADDR_DUAL_S  5
/** SPI1_MEM_S_FDIN_QUAD : R/W; bitpos: [6]; default: 0;
 *  For SPI1, din phase apply 4 signals. 1: enable 0: disable.  The bit is the same
 *  with spi1_mem_s_fread_qio.
 */
#define SPI1_MEM_S_FDIN_QUAD    (BIT(6))
#define SPI1_MEM_S_FDIN_QUAD_M  (SPI1_MEM_S_FDIN_QUAD_V << SPI1_MEM_S_FDIN_QUAD_S)
#define SPI1_MEM_S_FDIN_QUAD_V  0x00000001U
#define SPI1_MEM_S_FDIN_QUAD_S  6
/** SPI1_MEM_S_FDOUT_QUAD : R/W; bitpos: [7]; default: 0;
 *  For SPI1, dout phase apply 4 signals. 1: enable 0: disable.  The bit is the same
 *  with spi1_mem_s_fread_qio.
 */
#define SPI1_MEM_S_FDOUT_QUAD    (BIT(7))
#define SPI1_MEM_S_FDOUT_QUAD_M  (SPI1_MEM_S_FDOUT_QUAD_V << SPI1_MEM_S_FDOUT_QUAD_S)
#define SPI1_MEM_S_FDOUT_QUAD_V  0x00000001U
#define SPI1_MEM_S_FDOUT_QUAD_S  7
/** SPI1_MEM_S_FADDR_QUAD : R/W; bitpos: [8]; default: 0;
 *  For SPI1, address phase apply 4 signals. 1: enable 0: disable.  The bit is the same
 *  with spi1_mem_s_fread_qio.
 */
#define SPI1_MEM_S_FADDR_QUAD    (BIT(8))
#define SPI1_MEM_S_FADDR_QUAD_M  (SPI1_MEM_S_FADDR_QUAD_V << SPI1_MEM_S_FADDR_QUAD_S)
#define SPI1_MEM_S_FADDR_QUAD_V  0x00000001U
#define SPI1_MEM_S_FADDR_QUAD_S  8

/** SPI1_MEM_S_W0_REG register
 *  SPI1 memory data buffer0
 */
#define SPI1_MEM_S_W0_REG (DR_REG_PSRAM_MSPI1_BASE + 0x58)
/** SPI1_MEM_S_BUF0 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define SPI1_MEM_S_BUF0    0xFFFFFFFFU
#define SPI1_MEM_S_BUF0_M  (SPI1_MEM_S_BUF0_V << SPI1_MEM_S_BUF0_S)
#define SPI1_MEM_S_BUF0_V  0xFFFFFFFFU
#define SPI1_MEM_S_BUF0_S  0

/** SPI1_MEM_S_W1_REG register
 *  SPI1 memory data buffer1
 */
#define SPI1_MEM_S_W1_REG (DR_REG_PSRAM_MSPI1_BASE + 0x5c)
/** SPI1_MEM_S_BUF1 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define SPI1_MEM_S_BUF1    0xFFFFFFFFU
#define SPI1_MEM_S_BUF1_M  (SPI1_MEM_S_BUF1_V << SPI1_MEM_S_BUF1_S)
#define SPI1_MEM_S_BUF1_V  0xFFFFFFFFU
#define SPI1_MEM_S_BUF1_S  0

/** SPI1_MEM_S_W2_REG register
 *  SPI1 memory data buffer2
 */
#define SPI1_MEM_S_W2_REG (DR_REG_PSRAM_MSPI1_BASE + 0x60)
/** SPI1_MEM_S_BUF2 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define SPI1_MEM_S_BUF2    0xFFFFFFFFU
#define SPI1_MEM_S_BUF2_M  (SPI1_MEM_S_BUF2_V << SPI1_MEM_S_BUF2_S)
#define SPI1_MEM_S_BUF2_V  0xFFFFFFFFU
#define SPI1_MEM_S_BUF2_S  0

/** SPI1_MEM_S_W3_REG register
 *  SPI1 memory data buffer3
 */
#define SPI1_MEM_S_W3_REG (DR_REG_PSRAM_MSPI1_BASE + 0x64)
/** SPI1_MEM_S_BUF3 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define SPI1_MEM_S_BUF3    0xFFFFFFFFU
#define SPI1_MEM_S_BUF3_M  (SPI1_MEM_S_BUF3_V << SPI1_MEM_S_BUF3_S)
#define SPI1_MEM_S_BUF3_V  0xFFFFFFFFU
#define SPI1_MEM_S_BUF3_S  0

/** SPI1_MEM_S_W4_REG register
 *  SPI1 memory data buffer4
 */
#define SPI1_MEM_S_W4_REG (DR_REG_PSRAM_MSPI1_BASE + 0x68)
/** SPI1_MEM_S_BUF4 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define SPI1_MEM_S_BUF4    0xFFFFFFFFU
#define SPI1_MEM_S_BUF4_M  (SPI1_MEM_S_BUF4_V << SPI1_MEM_S_BUF4_S)
#define SPI1_MEM_S_BUF4_V  0xFFFFFFFFU
#define SPI1_MEM_S_BUF4_S  0

/** SPI1_MEM_S_W5_REG register
 *  SPI1 memory data buffer5
 */
#define SPI1_MEM_S_W5_REG (DR_REG_PSRAM_MSPI1_BASE + 0x6c)
/** SPI1_MEM_S_BUF5 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define SPI1_MEM_S_BUF5    0xFFFFFFFFU
#define SPI1_MEM_S_BUF5_M  (SPI1_MEM_S_BUF5_V << SPI1_MEM_S_BUF5_S)
#define SPI1_MEM_S_BUF5_V  0xFFFFFFFFU
#define SPI1_MEM_S_BUF5_S  0

/** SPI1_MEM_S_W6_REG register
 *  SPI1 memory data buffer6
 */
#define SPI1_MEM_S_W6_REG (DR_REG_PSRAM_MSPI1_BASE + 0x70)
/** SPI1_MEM_S_BUF6 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define SPI1_MEM_S_BUF6    0xFFFFFFFFU
#define SPI1_MEM_S_BUF6_M  (SPI1_MEM_S_BUF6_V << SPI1_MEM_S_BUF6_S)
#define SPI1_MEM_S_BUF6_V  0xFFFFFFFFU
#define SPI1_MEM_S_BUF6_S  0

/** SPI1_MEM_S_W7_REG register
 *  SPI1 memory data buffer7
 */
#define SPI1_MEM_S_W7_REG (DR_REG_PSRAM_MSPI1_BASE + 0x74)
/** SPI1_MEM_S_BUF7 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define SPI1_MEM_S_BUF7    0xFFFFFFFFU
#define SPI1_MEM_S_BUF7_M  (SPI1_MEM_S_BUF7_V << SPI1_MEM_S_BUF7_S)
#define SPI1_MEM_S_BUF7_V  0xFFFFFFFFU
#define SPI1_MEM_S_BUF7_S  0

/** SPI1_MEM_S_W8_REG register
 *  SPI1 memory data buffer8
 */
#define SPI1_MEM_S_W8_REG (DR_REG_PSRAM_MSPI1_BASE + 0x78)
/** SPI1_MEM_S_BUF8 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define SPI1_MEM_S_BUF8    0xFFFFFFFFU
#define SPI1_MEM_S_BUF8_M  (SPI1_MEM_S_BUF8_V << SPI1_MEM_S_BUF8_S)
#define SPI1_MEM_S_BUF8_V  0xFFFFFFFFU
#define SPI1_MEM_S_BUF8_S  0

/** SPI1_MEM_S_W9_REG register
 *  SPI1 memory data buffer9
 */
#define SPI1_MEM_S_W9_REG (DR_REG_PSRAM_MSPI1_BASE + 0x7c)
/** SPI1_MEM_S_BUF9 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define SPI1_MEM_S_BUF9    0xFFFFFFFFU
#define SPI1_MEM_S_BUF9_M  (SPI1_MEM_S_BUF9_V << SPI1_MEM_S_BUF9_S)
#define SPI1_MEM_S_BUF9_V  0xFFFFFFFFU
#define SPI1_MEM_S_BUF9_S  0

/** SPI1_MEM_S_W10_REG register
 *  SPI1 memory data buffer10
 */
#define SPI1_MEM_S_W10_REG (DR_REG_PSRAM_MSPI1_BASE + 0x80)
/** SPI1_MEM_S_BUF10 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define SPI1_MEM_S_BUF10    0xFFFFFFFFU
#define SPI1_MEM_S_BUF10_M  (SPI1_MEM_S_BUF10_V << SPI1_MEM_S_BUF10_S)
#define SPI1_MEM_S_BUF10_V  0xFFFFFFFFU
#define SPI1_MEM_S_BUF10_S  0

/** SPI1_MEM_S_W11_REG register
 *  SPI1 memory data buffer11
 */
#define SPI1_MEM_S_W11_REG (DR_REG_PSRAM_MSPI1_BASE + 0x84)
/** SPI1_MEM_S_BUF11 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define SPI1_MEM_S_BUF11    0xFFFFFFFFU
#define SPI1_MEM_S_BUF11_M  (SPI1_MEM_S_BUF11_V << SPI1_MEM_S_BUF11_S)
#define SPI1_MEM_S_BUF11_V  0xFFFFFFFFU
#define SPI1_MEM_S_BUF11_S  0

/** SPI1_MEM_S_W12_REG register
 *  SPI1 memory data buffer12
 */
#define SPI1_MEM_S_W12_REG (DR_REG_PSRAM_MSPI1_BASE + 0x88)
/** SPI1_MEM_S_BUF12 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define SPI1_MEM_S_BUF12    0xFFFFFFFFU
#define SPI1_MEM_S_BUF12_M  (SPI1_MEM_S_BUF12_V << SPI1_MEM_S_BUF12_S)
#define SPI1_MEM_S_BUF12_V  0xFFFFFFFFU
#define SPI1_MEM_S_BUF12_S  0

/** SPI1_MEM_S_W13_REG register
 *  SPI1 memory data buffer13
 */
#define SPI1_MEM_S_W13_REG (DR_REG_PSRAM_MSPI1_BASE + 0x8c)
/** SPI1_MEM_S_BUF13 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define SPI1_MEM_S_BUF13    0xFFFFFFFFU
#define SPI1_MEM_S_BUF13_M  (SPI1_MEM_S_BUF13_V << SPI1_MEM_S_BUF13_S)
#define SPI1_MEM_S_BUF13_V  0xFFFFFFFFU
#define SPI1_MEM_S_BUF13_S  0

/** SPI1_MEM_S_W14_REG register
 *  SPI1 memory data buffer14
 */
#define SPI1_MEM_S_W14_REG (DR_REG_PSRAM_MSPI1_BASE + 0x90)
/** SPI1_MEM_S_BUF14 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define SPI1_MEM_S_BUF14    0xFFFFFFFFU
#define SPI1_MEM_S_BUF14_M  (SPI1_MEM_S_BUF14_V << SPI1_MEM_S_BUF14_S)
#define SPI1_MEM_S_BUF14_V  0xFFFFFFFFU
#define SPI1_MEM_S_BUF14_S  0

/** SPI1_MEM_S_W15_REG register
 *  SPI1 memory data buffer15
 */
#define SPI1_MEM_S_W15_REG (DR_REG_PSRAM_MSPI1_BASE + 0x94)
/** SPI1_MEM_S_BUF15 : R/W/SS; bitpos: [31:0]; default: 0;
 *  data buffer
 */
#define SPI1_MEM_S_BUF15    0xFFFFFFFFU
#define SPI1_MEM_S_BUF15_M  (SPI1_MEM_S_BUF15_V << SPI1_MEM_S_BUF15_S)
#define SPI1_MEM_S_BUF15_V  0xFFFFFFFFU
#define SPI1_MEM_S_BUF15_S  0

/** SPI1_MEM_S_FLASH_WAITI_CTRL_REG register
 *  SPI1 wait idle control register
 */
#define SPI1_MEM_S_FLASH_WAITI_CTRL_REG (DR_REG_PSRAM_MSPI1_BASE + 0x98)
/** SPI1_MEM_S_WAITI_EN : R/W; bitpos: [0]; default: 1;
 *  1: The hardware will wait idle after SE/PP/WRSR automatically, and hardware auto
 *  Suspend/Resume can be enabled. 0: The functions of hardware wait idle and auto
 *  Suspend/Resume are not supported.
 */
#define SPI1_MEM_S_WAITI_EN    (BIT(0))
#define SPI1_MEM_S_WAITI_EN_M  (SPI1_MEM_S_WAITI_EN_V << SPI1_MEM_S_WAITI_EN_S)
#define SPI1_MEM_S_WAITI_EN_V  0x00000001U
#define SPI1_MEM_S_WAITI_EN_S  0
/** SPI1_MEM_S_WAITI_DUMMY : R/W; bitpos: [1]; default: 0;
 *  The dummy phase enable when wait flash idle (RDSR)
 */
#define SPI1_MEM_S_WAITI_DUMMY    (BIT(1))
#define SPI1_MEM_S_WAITI_DUMMY_M  (SPI1_MEM_S_WAITI_DUMMY_V << SPI1_MEM_S_WAITI_DUMMY_S)
#define SPI1_MEM_S_WAITI_DUMMY_V  0x00000001U
#define SPI1_MEM_S_WAITI_DUMMY_S  1
/** SPI1_MEM_S_WAITI_ADDR_EN : R/W; bitpos: [2]; default: 0;
 *  1: Output  address 0 in RDSR or read SUS command transfer. 0: Do not send out
 *  address in RDSR or read SUS command transfer.
 */
#define SPI1_MEM_S_WAITI_ADDR_EN    (BIT(2))
#define SPI1_MEM_S_WAITI_ADDR_EN_M  (SPI1_MEM_S_WAITI_ADDR_EN_V << SPI1_MEM_S_WAITI_ADDR_EN_S)
#define SPI1_MEM_S_WAITI_ADDR_EN_V  0x00000001U
#define SPI1_MEM_S_WAITI_ADDR_EN_S  2
/** SPI1_MEM_S_WAITI_ADDR_CYCLELEN : R/W; bitpos: [4:3]; default: 0;
 *  When SPI1_MEM_S_WAITI_ADDR_EN is set, the  cycle length of sent out address is
 *  (SPI1_MEM_S_WAITI_ADDR_CYCLELEN[1:0] + 1) SPI  bus clock cycles. It is not active when
 *  SPI1_MEM_S_WAITI_ADDR_EN is cleared.
 */
#define SPI1_MEM_S_WAITI_ADDR_CYCLELEN    0x00000003U
#define SPI1_MEM_S_WAITI_ADDR_CYCLELEN_M  (SPI1_MEM_S_WAITI_ADDR_CYCLELEN_V << SPI1_MEM_S_WAITI_ADDR_CYCLELEN_S)
#define SPI1_MEM_S_WAITI_ADDR_CYCLELEN_V  0x00000003U
#define SPI1_MEM_S_WAITI_ADDR_CYCLELEN_S  3
/** SPI1_MEM_S_WAITI_CMD_2B : R/W; bitpos: [9]; default: 0;
 *  1:The wait idle command bit length is 16. 0: The wait idle command bit length is 8.
 */
#define SPI1_MEM_S_WAITI_CMD_2B    (BIT(9))
#define SPI1_MEM_S_WAITI_CMD_2B_M  (SPI1_MEM_S_WAITI_CMD_2B_V << SPI1_MEM_S_WAITI_CMD_2B_S)
#define SPI1_MEM_S_WAITI_CMD_2B_V  0x00000001U
#define SPI1_MEM_S_WAITI_CMD_2B_S  9
/** SPI1_MEM_S_WAITI_DUMMY_CYCLELEN : R/W; bitpos: [15:10]; default: 0;
 *  The dummy cycle length when wait flash idle(RDSR).
 */
#define SPI1_MEM_S_WAITI_DUMMY_CYCLELEN    0x0000003FU
#define SPI1_MEM_S_WAITI_DUMMY_CYCLELEN_M  (SPI1_MEM_S_WAITI_DUMMY_CYCLELEN_V << SPI1_MEM_S_WAITI_DUMMY_CYCLELEN_S)
#define SPI1_MEM_S_WAITI_DUMMY_CYCLELEN_V  0x0000003FU
#define SPI1_MEM_S_WAITI_DUMMY_CYCLELEN_S  10
/** SPI1_MEM_S_WAITI_CMD : R/W; bitpos: [31:16]; default: 5;
 *  The command value to wait flash idle(RDSR).
 */
#define SPI1_MEM_S_WAITI_CMD    0x0000FFFFU
#define SPI1_MEM_S_WAITI_CMD_M  (SPI1_MEM_S_WAITI_CMD_V << SPI1_MEM_S_WAITI_CMD_S)
#define SPI1_MEM_S_WAITI_CMD_V  0x0000FFFFU
#define SPI1_MEM_S_WAITI_CMD_S  16

/** SPI1_MEM_S_FLASH_SUS_CTRL_REG register
 *  SPI1 flash suspend control register
 */
#define SPI1_MEM_S_FLASH_SUS_CTRL_REG (DR_REG_PSRAM_MSPI1_BASE + 0x9c)
/** SPI1_MEM_S_FLASH_PER : R/W/SC; bitpos: [0]; default: 0;
 *  program erase resume bit, program erase suspend operation will be triggered when
 *  the bit is set. The bit will be cleared once the operation done.1: enable 0:
 *  disable.
 */
#define SPI1_MEM_S_FLASH_PER    (BIT(0))
#define SPI1_MEM_S_FLASH_PER_M  (SPI1_MEM_S_FLASH_PER_V << SPI1_MEM_S_FLASH_PER_S)
#define SPI1_MEM_S_FLASH_PER_V  0x00000001U
#define SPI1_MEM_S_FLASH_PER_S  0
/** SPI1_MEM_S_FLASH_PES : R/W/SC; bitpos: [1]; default: 0;
 *  program erase suspend bit, program erase suspend operation will be triggered when
 *  the bit is set. The bit will be cleared once the operation done.1: enable 0:
 *  disable.
 */
#define SPI1_MEM_S_FLASH_PES    (BIT(1))
#define SPI1_MEM_S_FLASH_PES_M  (SPI1_MEM_S_FLASH_PES_V << SPI1_MEM_S_FLASH_PES_S)
#define SPI1_MEM_S_FLASH_PES_V  0x00000001U
#define SPI1_MEM_S_FLASH_PES_S  1
/** SPI1_MEM_S_FLASH_PER_WAIT_EN : R/W; bitpos: [2]; default: 0;
 *  1: SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 4 or *128) SPI_CLK cycles after
 *  program erase resume command is sent. 0: SPI1 does not wait after program erase
 *  resume command is sent.
 */
#define SPI1_MEM_S_FLASH_PER_WAIT_EN    (BIT(2))
#define SPI1_MEM_S_FLASH_PER_WAIT_EN_M  (SPI1_MEM_S_FLASH_PER_WAIT_EN_V << SPI1_MEM_S_FLASH_PER_WAIT_EN_S)
#define SPI1_MEM_S_FLASH_PER_WAIT_EN_V  0x00000001U
#define SPI1_MEM_S_FLASH_PER_WAIT_EN_S  2
/** SPI1_MEM_S_FLASH_PES_WAIT_EN : R/W; bitpos: [3]; default: 0;
 *  1: SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 4 or *128) SPI_CLK cycles after
 *  program erase suspend command is sent. 0: SPI1 does not wait after program erase
 *  suspend command is sent.
 */
#define SPI1_MEM_S_FLASH_PES_WAIT_EN    (BIT(3))
#define SPI1_MEM_S_FLASH_PES_WAIT_EN_M  (SPI1_MEM_S_FLASH_PES_WAIT_EN_V << SPI1_MEM_S_FLASH_PES_WAIT_EN_S)
#define SPI1_MEM_S_FLASH_PES_WAIT_EN_V  0x00000001U
#define SPI1_MEM_S_FLASH_PES_WAIT_EN_S  3
/** SPI1_MEM_S_PES_PER_EN : R/W; bitpos: [4]; default: 0;
 *  Set this bit to enable PES end triggers PER transfer option. If this bit is 0,
 *  application should send PER after PES is done.
 */
#define SPI1_MEM_S_PES_PER_EN    (BIT(4))
#define SPI1_MEM_S_PES_PER_EN_M  (SPI1_MEM_S_PES_PER_EN_V << SPI1_MEM_S_PES_PER_EN_S)
#define SPI1_MEM_S_PES_PER_EN_V  0x00000001U
#define SPI1_MEM_S_PES_PER_EN_S  4
/** SPI1_MEM_S_FLASH_PES_EN : R/W; bitpos: [5]; default: 0;
 *  Set this bit to enable Auto-suspending function.
 */
#define SPI1_MEM_S_FLASH_PES_EN    (BIT(5))
#define SPI1_MEM_S_FLASH_PES_EN_M  (SPI1_MEM_S_FLASH_PES_EN_V << SPI1_MEM_S_FLASH_PES_EN_S)
#define SPI1_MEM_S_FLASH_PES_EN_V  0x00000001U
#define SPI1_MEM_S_FLASH_PES_EN_S  5
/** SPI1_MEM_S_PESR_END_MSK : R/W; bitpos: [21:6]; default: 128;
 *  The mask value when check SUS/SUS1/SUS2 status bit. If the read status value is
 *  status_in[15:0](only status_in[7:0] is valid when only one byte of data is read
 *  out, status_in[15:0] is valid when two bytes of data are read out), SUS/SUS1/SUS2 =
 *  status_in[15:0]^ SPI1_MEM_S_PESR_END_MSK[15:0].
 */
#define SPI1_MEM_S_PESR_END_MSK    0x0000FFFFU
#define SPI1_MEM_S_PESR_END_MSK_M  (SPI1_MEM_S_PESR_END_MSK_V << SPI1_MEM_S_PESR_END_MSK_S)
#define SPI1_MEM_S_PESR_END_MSK_V  0x0000FFFFU
#define SPI1_MEM_S_PESR_END_MSK_S  6
/** SPI1_MEM_S_FMEM_RD_SUS_2B : R/W; bitpos: [22]; default: 0;
 *  1: Read two bytes when check flash SUS/SUS1/SUS2 status bit. 0:  Read one byte when
 *  check flash SUS/SUS1/SUS2 status bit
 */
#define SPI1_MEM_S_FMEM_RD_SUS_2B    (BIT(22))
#define SPI1_MEM_S_FMEM_RD_SUS_2B_M  (SPI1_MEM_S_FMEM_RD_SUS_2B_V << SPI1_MEM_S_FMEM_RD_SUS_2B_S)
#define SPI1_MEM_S_FMEM_RD_SUS_2B_V  0x00000001U
#define SPI1_MEM_S_FMEM_RD_SUS_2B_S  22
/** SPI1_MEM_S_PER_END_EN : R/W; bitpos: [23]; default: 0;
 *  1: Both WIP and SUS/SUS1/SUS2 bits should be checked to insure the resume status of
 *  flash. 0: Only need to check WIP is 0.
 */
#define SPI1_MEM_S_PER_END_EN    (BIT(23))
#define SPI1_MEM_S_PER_END_EN_M  (SPI1_MEM_S_PER_END_EN_V << SPI1_MEM_S_PER_END_EN_S)
#define SPI1_MEM_S_PER_END_EN_V  0x00000001U
#define SPI1_MEM_S_PER_END_EN_S  23
/** SPI1_MEM_S_PES_END_EN : R/W; bitpos: [24]; default: 0;
 *  1: Both WIP and SUS/SUS1/SUS2 bits should be checked to insure the suspend status
 *  of flash. 0: Only need to check WIP is 0.
 */
#define SPI1_MEM_S_PES_END_EN    (BIT(24))
#define SPI1_MEM_S_PES_END_EN_M  (SPI1_MEM_S_PES_END_EN_V << SPI1_MEM_S_PES_END_EN_S)
#define SPI1_MEM_S_PES_END_EN_V  0x00000001U
#define SPI1_MEM_S_PES_END_EN_S  24
/** SPI1_MEM_S_SUS_TIMEOUT_CNT : R/W; bitpos: [31:25]; default: 4;
 *  When SPI1 checks SUS/SUS1/SUS2 bits fail for SPI1_MEM_S_SUS_TIMEOUT_CNT[6:0] times, it
 *  will be treated as check pass.
 */
#define SPI1_MEM_S_SUS_TIMEOUT_CNT    0x0000007FU
#define SPI1_MEM_S_SUS_TIMEOUT_CNT_M  (SPI1_MEM_S_SUS_TIMEOUT_CNT_V << SPI1_MEM_S_SUS_TIMEOUT_CNT_S)
#define SPI1_MEM_S_SUS_TIMEOUT_CNT_V  0x0000007FU
#define SPI1_MEM_S_SUS_TIMEOUT_CNT_S  25

/** SPI1_MEM_S_FLASH_SUS_CMD_REG register
 *  SPI1 flash suspend command register
 */
#define SPI1_MEM_S_FLASH_SUS_CMD_REG (DR_REG_PSRAM_MSPI1_BASE + 0xa0)
/** SPI1_MEM_S_FLASH_PES_COMMAND : R/W; bitpos: [15:0]; default: 30069;
 *  Program/Erase suspend command.
 */
#define SPI1_MEM_S_FLASH_PES_COMMAND    0x0000FFFFU
#define SPI1_MEM_S_FLASH_PES_COMMAND_M  (SPI1_MEM_S_FLASH_PES_COMMAND_V << SPI1_MEM_S_FLASH_PES_COMMAND_S)
#define SPI1_MEM_S_FLASH_PES_COMMAND_V  0x0000FFFFU
#define SPI1_MEM_S_FLASH_PES_COMMAND_S  0
/** SPI1_MEM_S_WAIT_PESR_COMMAND : R/W; bitpos: [31:16]; default: 5;
 *  Flash SUS/SUS1/SUS2 status bit read command. The command should be sent when
 *  SUS/SUS1/SUS2 bit should be checked to insure the suspend or resume status of flash.
 */
#define SPI1_MEM_S_WAIT_PESR_COMMAND    0x0000FFFFU
#define SPI1_MEM_S_WAIT_PESR_COMMAND_M  (SPI1_MEM_S_WAIT_PESR_COMMAND_V << SPI1_MEM_S_WAIT_PESR_COMMAND_S)
#define SPI1_MEM_S_WAIT_PESR_COMMAND_V  0x0000FFFFU
#define SPI1_MEM_S_WAIT_PESR_COMMAND_S  16

/** SPI1_MEM_S_SUS_STATUS_REG register
 *  SPI1 flash suspend status register
 */
#define SPI1_MEM_S_SUS_STATUS_REG (DR_REG_PSRAM_MSPI1_BASE + 0xa4)
/** SPI1_MEM_S_FLASH_SUS : R/W/SS/SC; bitpos: [0]; default: 0;
 *  The status of flash suspend, only used in SPI1.
 */
#define SPI1_MEM_S_FLASH_SUS    (BIT(0))
#define SPI1_MEM_S_FLASH_SUS_M  (SPI1_MEM_S_FLASH_SUS_V << SPI1_MEM_S_FLASH_SUS_S)
#define SPI1_MEM_S_FLASH_SUS_V  0x00000001U
#define SPI1_MEM_S_FLASH_SUS_S  0
/** SPI1_MEM_S_WAIT_PESR_CMD_2B : R/W; bitpos: [1]; default: 0;
 *  1: SPI1 sends out SPI1_MEM_S_WAIT_PESR_COMMAND[15:0] to check SUS/SUS1/SUS2 bit. 0:
 *  SPI1 sends out SPI1_MEM_S_WAIT_PESR_COMMAND[7:0] to check SUS/SUS1/SUS2 bit.
 */
#define SPI1_MEM_S_WAIT_PESR_CMD_2B    (BIT(1))
#define SPI1_MEM_S_WAIT_PESR_CMD_2B_M  (SPI1_MEM_S_WAIT_PESR_CMD_2B_V << SPI1_MEM_S_WAIT_PESR_CMD_2B_S)
#define SPI1_MEM_S_WAIT_PESR_CMD_2B_V  0x00000001U
#define SPI1_MEM_S_WAIT_PESR_CMD_2B_S  1
/** SPI1_MEM_S_FLASH_HPM_DLY_128 : R/W; bitpos: [2]; default: 0;
 *  1: SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 128) SPI_CLK cycles after HPM
 *  command is sent. 0: SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles
 *  after HPM command is sent.
 */
#define SPI1_MEM_S_FLASH_HPM_DLY_128    (BIT(2))
#define SPI1_MEM_S_FLASH_HPM_DLY_128_M  (SPI1_MEM_S_FLASH_HPM_DLY_128_V << SPI1_MEM_S_FLASH_HPM_DLY_128_S)
#define SPI1_MEM_S_FLASH_HPM_DLY_128_V  0x00000001U
#define SPI1_MEM_S_FLASH_HPM_DLY_128_S  2
/** SPI1_MEM_S_FLASH_RES_DLY_128 : R/W; bitpos: [3]; default: 0;
 *  1: SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 128) SPI_CLK cycles after RES
 *  command is sent. 0: SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles
 *  after RES command is sent.
 */
#define SPI1_MEM_S_FLASH_RES_DLY_128    (BIT(3))
#define SPI1_MEM_S_FLASH_RES_DLY_128_M  (SPI1_MEM_S_FLASH_RES_DLY_128_V << SPI1_MEM_S_FLASH_RES_DLY_128_S)
#define SPI1_MEM_S_FLASH_RES_DLY_128_V  0x00000001U
#define SPI1_MEM_S_FLASH_RES_DLY_128_S  3
/** SPI1_MEM_S_FLASH_DP_DLY_128 : R/W; bitpos: [4]; default: 0;
 *  1: SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 128) SPI_CLK cycles after DP
 *  command is sent. 0: SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles
 *  after DP command is sent.
 */
#define SPI1_MEM_S_FLASH_DP_DLY_128    (BIT(4))
#define SPI1_MEM_S_FLASH_DP_DLY_128_M  (SPI1_MEM_S_FLASH_DP_DLY_128_V << SPI1_MEM_S_FLASH_DP_DLY_128_S)
#define SPI1_MEM_S_FLASH_DP_DLY_128_V  0x00000001U
#define SPI1_MEM_S_FLASH_DP_DLY_128_S  4
/** SPI1_MEM_S_FLASH_PER_DLY_128 : R/W; bitpos: [5]; default: 0;
 *  Valid when SPI1_MEM_S_FLASH_PER_WAIT_EN is 1. 1: SPI1 waits
 *  (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 128) SPI_CLK cycles after PER command is sent. 0:
 *  SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after PER command is
 *  sent.
 */
#define SPI1_MEM_S_FLASH_PER_DLY_128    (BIT(5))
#define SPI1_MEM_S_FLASH_PER_DLY_128_M  (SPI1_MEM_S_FLASH_PER_DLY_128_V << SPI1_MEM_S_FLASH_PER_DLY_128_S)
#define SPI1_MEM_S_FLASH_PER_DLY_128_V  0x00000001U
#define SPI1_MEM_S_FLASH_PER_DLY_128_S  5
/** SPI1_MEM_S_FLASH_PES_DLY_128 : R/W; bitpos: [6]; default: 0;
 *  Valid when SPI1_MEM_S_FLASH_PES_WAIT_EN is 1. 1: SPI1 waits
 *  (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 128) SPI_CLK cycles after PES command is sent. 0:
 *  SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after PES command is
 *  sent.
 */
#define SPI1_MEM_S_FLASH_PES_DLY_128    (BIT(6))
#define SPI1_MEM_S_FLASH_PES_DLY_128_M  (SPI1_MEM_S_FLASH_PES_DLY_128_V << SPI1_MEM_S_FLASH_PES_DLY_128_S)
#define SPI1_MEM_S_FLASH_PES_DLY_128_V  0x00000001U
#define SPI1_MEM_S_FLASH_PES_DLY_128_S  6
/** SPI1_MEM_S_SPI0_LOCK_EN : R/W; bitpos: [7]; default: 0;
 *  1: Enable SPI0 lock SPI0/1 arbiter option. 0: Disable it.
 */
#define SPI1_MEM_S_SPI0_LOCK_EN    (BIT(7))
#define SPI1_MEM_S_SPI0_LOCK_EN_M  (SPI1_MEM_S_SPI0_LOCK_EN_V << SPI1_MEM_S_SPI0_LOCK_EN_S)
#define SPI1_MEM_S_SPI0_LOCK_EN_V  0x00000001U
#define SPI1_MEM_S_SPI0_LOCK_EN_S  7
/** SPI1_MEM_S_FLASH_PESR_CMD_2B : R/W; bitpos: [15]; default: 0;
 *  1: The bit length of Program/Erase Suspend/Resume command is 16. 0: The bit length
 *  of Program/Erase Suspend/Resume command is 8.
 */
#define SPI1_MEM_S_FLASH_PESR_CMD_2B    (BIT(15))
#define SPI1_MEM_S_FLASH_PESR_CMD_2B_M  (SPI1_MEM_S_FLASH_PESR_CMD_2B_V << SPI1_MEM_S_FLASH_PESR_CMD_2B_S)
#define SPI1_MEM_S_FLASH_PESR_CMD_2B_V  0x00000001U
#define SPI1_MEM_S_FLASH_PESR_CMD_2B_S  15
/** SPI1_MEM_S_FLASH_PER_COMMAND : R/W; bitpos: [31:16]; default: 31354;
 *  Program/Erase resume command.
 */
#define SPI1_MEM_S_FLASH_PER_COMMAND    0x0000FFFFU
#define SPI1_MEM_S_FLASH_PER_COMMAND_M  (SPI1_MEM_S_FLASH_PER_COMMAND_V << SPI1_MEM_S_FLASH_PER_COMMAND_S)
#define SPI1_MEM_S_FLASH_PER_COMMAND_V  0x0000FFFFU
#define SPI1_MEM_S_FLASH_PER_COMMAND_S  16

/** SPI1_MEM_S_INT_ENA_REG register
 *  SPI1 interrupt enable register
 */
#define SPI1_MEM_S_INT_ENA_REG (DR_REG_PSRAM_MSPI1_BASE + 0xc0)
/** SPI1_MEM_S_PER_END_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The enable bit for SPI1_MEM_S_PER_END_INT interrupt.
 */
#define SPI1_MEM_S_PER_END_INT_ENA    (BIT(0))
#define SPI1_MEM_S_PER_END_INT_ENA_M  (SPI1_MEM_S_PER_END_INT_ENA_V << SPI1_MEM_S_PER_END_INT_ENA_S)
#define SPI1_MEM_S_PER_END_INT_ENA_V  0x00000001U
#define SPI1_MEM_S_PER_END_INT_ENA_S  0
/** SPI1_MEM_S_PES_END_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The enable bit for SPI1_MEM_S_PES_END_INT interrupt.
 */
#define SPI1_MEM_S_PES_END_INT_ENA    (BIT(1))
#define SPI1_MEM_S_PES_END_INT_ENA_M  (SPI1_MEM_S_PES_END_INT_ENA_V << SPI1_MEM_S_PES_END_INT_ENA_S)
#define SPI1_MEM_S_PES_END_INT_ENA_V  0x00000001U
#define SPI1_MEM_S_PES_END_INT_ENA_S  1
/** SPI1_MEM_S_WPE_END_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The enable bit for SPI1_MEM_S_WPE_END_INT interrupt.
 */
#define SPI1_MEM_S_WPE_END_INT_ENA    (BIT(2))
#define SPI1_MEM_S_WPE_END_INT_ENA_M  (SPI1_MEM_S_WPE_END_INT_ENA_V << SPI1_MEM_S_WPE_END_INT_ENA_S)
#define SPI1_MEM_S_WPE_END_INT_ENA_V  0x00000001U
#define SPI1_MEM_S_WPE_END_INT_ENA_S  2
/** SPI1_MEM_S_SLV_ST_END_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The enable bit for SPI1_MEM_S_SLV_ST_END_INT interrupt.
 */
#define SPI1_MEM_S_SLV_ST_END_INT_ENA    (BIT(3))
#define SPI1_MEM_S_SLV_ST_END_INT_ENA_M  (SPI1_MEM_S_SLV_ST_END_INT_ENA_V << SPI1_MEM_S_SLV_ST_END_INT_ENA_S)
#define SPI1_MEM_S_SLV_ST_END_INT_ENA_V  0x00000001U
#define SPI1_MEM_S_SLV_ST_END_INT_ENA_S  3
/** SPI1_MEM_S_MST_ST_END_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The enable bit for SPI1_MEM_S_MST_ST_END_INT interrupt.
 */
#define SPI1_MEM_S_MST_ST_END_INT_ENA    (BIT(4))
#define SPI1_MEM_S_MST_ST_END_INT_ENA_M  (SPI1_MEM_S_MST_ST_END_INT_ENA_V << SPI1_MEM_S_MST_ST_END_INT_ENA_S)
#define SPI1_MEM_S_MST_ST_END_INT_ENA_V  0x00000001U
#define SPI1_MEM_S_MST_ST_END_INT_ENA_S  4
/** SPI1_MEM_S_BROWN_OUT_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  The enable bit for SPI1_MEM_S_BROWN_OUT_INT interrupt.
 */
#define SPI1_MEM_S_BROWN_OUT_INT_ENA    (BIT(10))
#define SPI1_MEM_S_BROWN_OUT_INT_ENA_M  (SPI1_MEM_S_BROWN_OUT_INT_ENA_V << SPI1_MEM_S_BROWN_OUT_INT_ENA_S)
#define SPI1_MEM_S_BROWN_OUT_INT_ENA_V  0x00000001U
#define SPI1_MEM_S_BROWN_OUT_INT_ENA_S  10

/** SPI1_MEM_S_INT_CLR_REG register
 *  SPI1 interrupt clear register
 */
#define SPI1_MEM_S_INT_CLR_REG (DR_REG_PSRAM_MSPI1_BASE + 0xc4)
/** SPI1_MEM_S_PER_END_INT_CLR : WT; bitpos: [0]; default: 0;
 *  The clear bit for SPI1_MEM_S_PER_END_INT interrupt.
 */
#define SPI1_MEM_S_PER_END_INT_CLR    (BIT(0))
#define SPI1_MEM_S_PER_END_INT_CLR_M  (SPI1_MEM_S_PER_END_INT_CLR_V << SPI1_MEM_S_PER_END_INT_CLR_S)
#define SPI1_MEM_S_PER_END_INT_CLR_V  0x00000001U
#define SPI1_MEM_S_PER_END_INT_CLR_S  0
/** SPI1_MEM_S_PES_END_INT_CLR : WT; bitpos: [1]; default: 0;
 *  The clear bit for SPI1_MEM_S_PES_END_INT interrupt.
 */
#define SPI1_MEM_S_PES_END_INT_CLR    (BIT(1))
#define SPI1_MEM_S_PES_END_INT_CLR_M  (SPI1_MEM_S_PES_END_INT_CLR_V << SPI1_MEM_S_PES_END_INT_CLR_S)
#define SPI1_MEM_S_PES_END_INT_CLR_V  0x00000001U
#define SPI1_MEM_S_PES_END_INT_CLR_S  1
/** SPI1_MEM_S_WPE_END_INT_CLR : WT; bitpos: [2]; default: 0;
 *  The clear bit for SPI1_MEM_S_WPE_END_INT interrupt.
 */
#define SPI1_MEM_S_WPE_END_INT_CLR    (BIT(2))
#define SPI1_MEM_S_WPE_END_INT_CLR_M  (SPI1_MEM_S_WPE_END_INT_CLR_V << SPI1_MEM_S_WPE_END_INT_CLR_S)
#define SPI1_MEM_S_WPE_END_INT_CLR_V  0x00000001U
#define SPI1_MEM_S_WPE_END_INT_CLR_S  2
/** SPI1_MEM_S_SLV_ST_END_INT_CLR : WT; bitpos: [3]; default: 0;
 *  The clear bit for SPI1_MEM_S_SLV_ST_END_INT interrupt.
 */
#define SPI1_MEM_S_SLV_ST_END_INT_CLR    (BIT(3))
#define SPI1_MEM_S_SLV_ST_END_INT_CLR_M  (SPI1_MEM_S_SLV_ST_END_INT_CLR_V << SPI1_MEM_S_SLV_ST_END_INT_CLR_S)
#define SPI1_MEM_S_SLV_ST_END_INT_CLR_V  0x00000001U
#define SPI1_MEM_S_SLV_ST_END_INT_CLR_S  3
/** SPI1_MEM_S_MST_ST_END_INT_CLR : WT; bitpos: [4]; default: 0;
 *  The clear bit for SPI1_MEM_S_MST_ST_END_INT interrupt.
 */
#define SPI1_MEM_S_MST_ST_END_INT_CLR    (BIT(4))
#define SPI1_MEM_S_MST_ST_END_INT_CLR_M  (SPI1_MEM_S_MST_ST_END_INT_CLR_V << SPI1_MEM_S_MST_ST_END_INT_CLR_S)
#define SPI1_MEM_S_MST_ST_END_INT_CLR_V  0x00000001U
#define SPI1_MEM_S_MST_ST_END_INT_CLR_S  4
/** SPI1_MEM_S_BROWN_OUT_INT_CLR : WT; bitpos: [10]; default: 0;
 *  The status bit for SPI1_MEM_S_BROWN_OUT_INT interrupt.
 */
#define SPI1_MEM_S_BROWN_OUT_INT_CLR    (BIT(10))
#define SPI1_MEM_S_BROWN_OUT_INT_CLR_M  (SPI1_MEM_S_BROWN_OUT_INT_CLR_V << SPI1_MEM_S_BROWN_OUT_INT_CLR_S)
#define SPI1_MEM_S_BROWN_OUT_INT_CLR_V  0x00000001U
#define SPI1_MEM_S_BROWN_OUT_INT_CLR_S  10

/** SPI1_MEM_S_INT_RAW_REG register
 *  SPI1 interrupt raw register
 */
#define SPI1_MEM_S_INT_RAW_REG (DR_REG_PSRAM_MSPI1_BASE + 0xc8)
/** SPI1_MEM_S_PER_END_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw bit for SPI1_MEM_S_PER_END_INT interrupt. 1: Triggered when Auto Resume
 *  command (0x7A) is sent and flash is resumed successfully. 0: Others.
 */
#define SPI1_MEM_S_PER_END_INT_RAW    (BIT(0))
#define SPI1_MEM_S_PER_END_INT_RAW_M  (SPI1_MEM_S_PER_END_INT_RAW_V << SPI1_MEM_S_PER_END_INT_RAW_S)
#define SPI1_MEM_S_PER_END_INT_RAW_V  0x00000001U
#define SPI1_MEM_S_PER_END_INT_RAW_S  0
/** SPI1_MEM_S_PES_END_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw bit for SPI1_MEM_S_PES_END_INT interrupt.1: Triggered when Auto Suspend
 *  command (0x75) is sent and flash is suspended successfully. 0: Others.
 */
#define SPI1_MEM_S_PES_END_INT_RAW    (BIT(1))
#define SPI1_MEM_S_PES_END_INT_RAW_M  (SPI1_MEM_S_PES_END_INT_RAW_V << SPI1_MEM_S_PES_END_INT_RAW_S)
#define SPI1_MEM_S_PES_END_INT_RAW_V  0x00000001U
#define SPI1_MEM_S_PES_END_INT_RAW_S  1
/** SPI1_MEM_S_WPE_END_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw bit for SPI1_MEM_S_WPE_END_INT interrupt. 1: Triggered when WRSR/PP/SE/BE/CE
 *  is sent and flash is already idle. 0: Others.
 */
#define SPI1_MEM_S_WPE_END_INT_RAW    (BIT(2))
#define SPI1_MEM_S_WPE_END_INT_RAW_M  (SPI1_MEM_S_WPE_END_INT_RAW_V << SPI1_MEM_S_WPE_END_INT_RAW_S)
#define SPI1_MEM_S_WPE_END_INT_RAW_V  0x00000001U
#define SPI1_MEM_S_WPE_END_INT_RAW_S  2
/** SPI1_MEM_S_SLV_ST_END_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw bit for SPI1_MEM_S_SLV_ST_END_INT interrupt. 1: Triggered when spi1_slv_st is
 *  changed from non idle state to idle state. It means that SPI_CS raises high. 0:
 *  Others
 */
#define SPI1_MEM_S_SLV_ST_END_INT_RAW    (BIT(3))
#define SPI1_MEM_S_SLV_ST_END_INT_RAW_M  (SPI1_MEM_S_SLV_ST_END_INT_RAW_V << SPI1_MEM_S_SLV_ST_END_INT_RAW_S)
#define SPI1_MEM_S_SLV_ST_END_INT_RAW_V  0x00000001U
#define SPI1_MEM_S_SLV_ST_END_INT_RAW_S  3
/** SPI1_MEM_S_MST_ST_END_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw bit for SPI1_MEM_S_MST_ST_END_INT interrupt. 1: Triggered when spi1_mst_st is
 *  changed from non idle state to idle state. 0: Others.
 */
#define SPI1_MEM_S_MST_ST_END_INT_RAW    (BIT(4))
#define SPI1_MEM_S_MST_ST_END_INT_RAW_M  (SPI1_MEM_S_MST_ST_END_INT_RAW_V << SPI1_MEM_S_MST_ST_END_INT_RAW_S)
#define SPI1_MEM_S_MST_ST_END_INT_RAW_V  0x00000001U
#define SPI1_MEM_S_MST_ST_END_INT_RAW_S  4
/** SPI1_MEM_S_BROWN_OUT_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  The raw bit for SPI1_MEM_S_BROWN_OUT_INT interrupt. 1: Triggered condition is that
 *  chip is losing power and RTC module sends out brown out close flash request to
 *  SPI1. After SPI1 sends out suspend command to flash, this interrupt is triggered
 *  and MSPI returns to idle state. 0: Others.
 */
#define SPI1_MEM_S_BROWN_OUT_INT_RAW    (BIT(10))
#define SPI1_MEM_S_BROWN_OUT_INT_RAW_M  (SPI1_MEM_S_BROWN_OUT_INT_RAW_V << SPI1_MEM_S_BROWN_OUT_INT_RAW_S)
#define SPI1_MEM_S_BROWN_OUT_INT_RAW_V  0x00000001U
#define SPI1_MEM_S_BROWN_OUT_INT_RAW_S  10

/** SPI1_MEM_S_INT_ST_REG register
 *  SPI1 interrupt status register
 */
#define SPI1_MEM_S_INT_ST_REG (DR_REG_PSRAM_MSPI1_BASE + 0xcc)
/** SPI1_MEM_S_PER_END_INT_ST : RO; bitpos: [0]; default: 0;
 *  The status bit for SPI1_MEM_S_PER_END_INT interrupt.
 */
#define SPI1_MEM_S_PER_END_INT_ST    (BIT(0))
#define SPI1_MEM_S_PER_END_INT_ST_M  (SPI1_MEM_S_PER_END_INT_ST_V << SPI1_MEM_S_PER_END_INT_ST_S)
#define SPI1_MEM_S_PER_END_INT_ST_V  0x00000001U
#define SPI1_MEM_S_PER_END_INT_ST_S  0
/** SPI1_MEM_S_PES_END_INT_ST : RO; bitpos: [1]; default: 0;
 *  The status bit for SPI1_MEM_S_PES_END_INT interrupt.
 */
#define SPI1_MEM_S_PES_END_INT_ST    (BIT(1))
#define SPI1_MEM_S_PES_END_INT_ST_M  (SPI1_MEM_S_PES_END_INT_ST_V << SPI1_MEM_S_PES_END_INT_ST_S)
#define SPI1_MEM_S_PES_END_INT_ST_V  0x00000001U
#define SPI1_MEM_S_PES_END_INT_ST_S  1
/** SPI1_MEM_S_WPE_END_INT_ST : RO; bitpos: [2]; default: 0;
 *  The status bit for SPI1_MEM_S_WPE_END_INT interrupt.
 */
#define SPI1_MEM_S_WPE_END_INT_ST    (BIT(2))
#define SPI1_MEM_S_WPE_END_INT_ST_M  (SPI1_MEM_S_WPE_END_INT_ST_V << SPI1_MEM_S_WPE_END_INT_ST_S)
#define SPI1_MEM_S_WPE_END_INT_ST_V  0x00000001U
#define SPI1_MEM_S_WPE_END_INT_ST_S  2
/** SPI1_MEM_S_SLV_ST_END_INT_ST : RO; bitpos: [3]; default: 0;
 *  The status bit for SPI1_MEM_S_SLV_ST_END_INT interrupt.
 */
#define SPI1_MEM_S_SLV_ST_END_INT_ST    (BIT(3))
#define SPI1_MEM_S_SLV_ST_END_INT_ST_M  (SPI1_MEM_S_SLV_ST_END_INT_ST_V << SPI1_MEM_S_SLV_ST_END_INT_ST_S)
#define SPI1_MEM_S_SLV_ST_END_INT_ST_V  0x00000001U
#define SPI1_MEM_S_SLV_ST_END_INT_ST_S  3
/** SPI1_MEM_S_MST_ST_END_INT_ST : RO; bitpos: [4]; default: 0;
 *  The status bit for SPI1_MEM_S_MST_ST_END_INT interrupt.
 */
#define SPI1_MEM_S_MST_ST_END_INT_ST    (BIT(4))
#define SPI1_MEM_S_MST_ST_END_INT_ST_M  (SPI1_MEM_S_MST_ST_END_INT_ST_V << SPI1_MEM_S_MST_ST_END_INT_ST_S)
#define SPI1_MEM_S_MST_ST_END_INT_ST_V  0x00000001U
#define SPI1_MEM_S_MST_ST_END_INT_ST_S  4
/** SPI1_MEM_S_BROWN_OUT_INT_ST : RO; bitpos: [10]; default: 0;
 *  The status bit for SPI1_MEM_S_BROWN_OUT_INT interrupt.
 */
#define SPI1_MEM_S_BROWN_OUT_INT_ST    (BIT(10))
#define SPI1_MEM_S_BROWN_OUT_INT_ST_M  (SPI1_MEM_S_BROWN_OUT_INT_ST_V << SPI1_MEM_S_BROWN_OUT_INT_ST_S)
#define SPI1_MEM_S_BROWN_OUT_INT_ST_V  0x00000001U
#define SPI1_MEM_S_BROWN_OUT_INT_ST_S  10

/** SPI1_MEM_S_DDR_REG register
 *  SPI1 DDR control register
 */
#define SPI1_MEM_S_DDR_REG (DR_REG_PSRAM_MSPI1_BASE + 0xd4)
/** SPI1_MEM_S_FMEM_DDR_EN : R/W; bitpos: [0]; default: 0;
 *  1: in ddr mode,  0 in sdr mode
 */
#define SPI1_MEM_S_FMEM_DDR_EN    (BIT(0))
#define SPI1_MEM_S_FMEM_DDR_EN_M  (SPI1_MEM_S_FMEM_DDR_EN_V << SPI1_MEM_S_FMEM_DDR_EN_S)
#define SPI1_MEM_S_FMEM_DDR_EN_V  0x00000001U
#define SPI1_MEM_S_FMEM_DDR_EN_S  0
/** SPI1_MEM_S_FMEM_VAR_DUMMY : R/W; bitpos: [1]; default: 0;
 *  Set the bit to enable variable dummy cycle in spi ddr mode.
 */
#define SPI1_MEM_S_FMEM_VAR_DUMMY    (BIT(1))
#define SPI1_MEM_S_FMEM_VAR_DUMMY_M  (SPI1_MEM_S_FMEM_VAR_DUMMY_V << SPI1_MEM_S_FMEM_VAR_DUMMY_S)
#define SPI1_MEM_S_FMEM_VAR_DUMMY_V  0x00000001U
#define SPI1_MEM_S_FMEM_VAR_DUMMY_S  1
/** SPI1_MEM_S_FMEM_DDR_RDAT_SWP : R/W; bitpos: [2]; default: 0;
 *  Set the bit to reorder rx data of the word in spi ddr mode.
 */
#define SPI1_MEM_S_FMEM_DDR_RDAT_SWP    (BIT(2))
#define SPI1_MEM_S_FMEM_DDR_RDAT_SWP_M  (SPI1_MEM_S_FMEM_DDR_RDAT_SWP_V << SPI1_MEM_S_FMEM_DDR_RDAT_SWP_S)
#define SPI1_MEM_S_FMEM_DDR_RDAT_SWP_V  0x00000001U
#define SPI1_MEM_S_FMEM_DDR_RDAT_SWP_S  2
/** SPI1_MEM_S_FMEM_DDR_WDAT_SWP : R/W; bitpos: [3]; default: 0;
 *  Set the bit to reorder tx data of the word in spi ddr mode.
 */
#define SPI1_MEM_S_FMEM_DDR_WDAT_SWP    (BIT(3))
#define SPI1_MEM_S_FMEM_DDR_WDAT_SWP_M  (SPI1_MEM_S_FMEM_DDR_WDAT_SWP_V << SPI1_MEM_S_FMEM_DDR_WDAT_SWP_S)
#define SPI1_MEM_S_FMEM_DDR_WDAT_SWP_V  0x00000001U
#define SPI1_MEM_S_FMEM_DDR_WDAT_SWP_S  3
/** SPI1_MEM_S_FMEM_DDR_CMD_DIS : R/W; bitpos: [4]; default: 0;
 *  the bit is used to disable dual edge in command phase when ddr mode.
 */
#define SPI1_MEM_S_FMEM_DDR_CMD_DIS    (BIT(4))
#define SPI1_MEM_S_FMEM_DDR_CMD_DIS_M  (SPI1_MEM_S_FMEM_DDR_CMD_DIS_V << SPI1_MEM_S_FMEM_DDR_CMD_DIS_S)
#define SPI1_MEM_S_FMEM_DDR_CMD_DIS_V  0x00000001U
#define SPI1_MEM_S_FMEM_DDR_CMD_DIS_S  4
/** SPI1_MEM_S_FMEM_OUTMINBYTELEN : R/W; bitpos: [11:5]; default: 1;
 *  It is the minimum output data length in the panda device.
 */
#define SPI1_MEM_S_FMEM_OUTMINBYTELEN    0x0000007FU
#define SPI1_MEM_S_FMEM_OUTMINBYTELEN_M  (SPI1_MEM_S_FMEM_OUTMINBYTELEN_V << SPI1_MEM_S_FMEM_OUTMINBYTELEN_S)
#define SPI1_MEM_S_FMEM_OUTMINBYTELEN_V  0x0000007FU
#define SPI1_MEM_S_FMEM_OUTMINBYTELEN_S  5
/** SPI1_MEM_S_FMEM_USR_DDR_DQS_THD : R/W; bitpos: [20:14]; default: 0;
 *  The delay number of data strobe which from memory based on SPI clock.
 */
#define SPI1_MEM_S_FMEM_USR_DDR_DQS_THD    0x0000007FU
#define SPI1_MEM_S_FMEM_USR_DDR_DQS_THD_M  (SPI1_MEM_S_FMEM_USR_DDR_DQS_THD_V << SPI1_MEM_S_FMEM_USR_DDR_DQS_THD_S)
#define SPI1_MEM_S_FMEM_USR_DDR_DQS_THD_V  0x0000007FU
#define SPI1_MEM_S_FMEM_USR_DDR_DQS_THD_S  14
/** SPI1_MEM_S_FMEM_DDR_DQS_LOOP : R/W; bitpos: [21]; default: 0;
 *  1: Do not need the input of SPI_DQS signal, SPI0 starts to receive data when
 *  spi0_slv_st is in SPI1_MEM_S_DIN state. It is used when there is no SPI_DQS signal or
 *  SPI_DQS signal is not stable. 0: SPI0 starts to store data at the positive and
 *  negative edge of SPI_DQS.
 */
#define SPI1_MEM_S_FMEM_DDR_DQS_LOOP    (BIT(21))
#define SPI1_MEM_S_FMEM_DDR_DQS_LOOP_M  (SPI1_MEM_S_FMEM_DDR_DQS_LOOP_V << SPI1_MEM_S_FMEM_DDR_DQS_LOOP_S)
#define SPI1_MEM_S_FMEM_DDR_DQS_LOOP_V  0x00000001U
#define SPI1_MEM_S_FMEM_DDR_DQS_LOOP_S  21
/** SPI1_MEM_S_FMEM_CLK_DIFF_EN : R/W; bitpos: [24]; default: 0;
 *  Set this bit to enable the differential SPI_CLK#.
 */
#define SPI1_MEM_S_FMEM_CLK_DIFF_EN    (BIT(24))
#define SPI1_MEM_S_FMEM_CLK_DIFF_EN_M  (SPI1_MEM_S_FMEM_CLK_DIFF_EN_V << SPI1_MEM_S_FMEM_CLK_DIFF_EN_S)
#define SPI1_MEM_S_FMEM_CLK_DIFF_EN_V  0x00000001U
#define SPI1_MEM_S_FMEM_CLK_DIFF_EN_S  24
/** SPI1_MEM_S_FMEM_DQS_CA_IN : R/W; bitpos: [26]; default: 0;
 *  Set this bit to enable the input of SPI_DQS signal in SPI phases of CMD and ADDR.
 */
#define SPI1_MEM_S_FMEM_DQS_CA_IN    (BIT(26))
#define SPI1_MEM_S_FMEM_DQS_CA_IN_M  (SPI1_MEM_S_FMEM_DQS_CA_IN_V << SPI1_MEM_S_FMEM_DQS_CA_IN_S)
#define SPI1_MEM_S_FMEM_DQS_CA_IN_V  0x00000001U
#define SPI1_MEM_S_FMEM_DQS_CA_IN_S  26
/** SPI1_MEM_S_FMEM_HYPERBUS_DUMMY_2X : R/W; bitpos: [27]; default: 0;
 *  Set this bit to enable the vary dummy function in SPI HyperBus mode, when SPI0
 *  accesses flash or SPI1 accesses flash or sram.
 */
#define SPI1_MEM_S_FMEM_HYPERBUS_DUMMY_2X    (BIT(27))
#define SPI1_MEM_S_FMEM_HYPERBUS_DUMMY_2X_M  (SPI1_MEM_S_FMEM_HYPERBUS_DUMMY_2X_V << SPI1_MEM_S_FMEM_HYPERBUS_DUMMY_2X_S)
#define SPI1_MEM_S_FMEM_HYPERBUS_DUMMY_2X_V  0x00000001U
#define SPI1_MEM_S_FMEM_HYPERBUS_DUMMY_2X_S  27
/** SPI1_MEM_S_FMEM_CLK_DIFF_INV : R/W; bitpos: [28]; default: 0;
 *  Set this bit to invert SPI_DIFF when accesses to flash. .
 */
#define SPI1_MEM_S_FMEM_CLK_DIFF_INV    (BIT(28))
#define SPI1_MEM_S_FMEM_CLK_DIFF_INV_M  (SPI1_MEM_S_FMEM_CLK_DIFF_INV_V << SPI1_MEM_S_FMEM_CLK_DIFF_INV_S)
#define SPI1_MEM_S_FMEM_CLK_DIFF_INV_V  0x00000001U
#define SPI1_MEM_S_FMEM_CLK_DIFF_INV_S  28
/** SPI1_MEM_S_FMEM_OCTA_RAM_ADDR : R/W; bitpos: [29]; default: 0;
 *  Set this bit to enable octa_ram address out when accesses to flash, which means
 *  ADDR_OUT[31:0] = {spi_usr_addr_value[25:4], 6'd0, spi_usr_addr_value[3:1], 1'b0}.
 */
#define SPI1_MEM_S_FMEM_OCTA_RAM_ADDR    (BIT(29))
#define SPI1_MEM_S_FMEM_OCTA_RAM_ADDR_M  (SPI1_MEM_S_FMEM_OCTA_RAM_ADDR_V << SPI1_MEM_S_FMEM_OCTA_RAM_ADDR_S)
#define SPI1_MEM_S_FMEM_OCTA_RAM_ADDR_V  0x00000001U
#define SPI1_MEM_S_FMEM_OCTA_RAM_ADDR_S  29
/** SPI1_MEM_S_FMEM_HYPERBUS_CA : R/W; bitpos: [30]; default: 0;
 *  Set this bit to enable HyperRAM address out when accesses to flash, which means
 *  ADDR_OUT[31:0] = {spi_usr_addr_value[19:4], 13'd0, spi_usr_addr_value[3:1]}.
 */
#define SPI1_MEM_S_FMEM_HYPERBUS_CA    (BIT(30))
#define SPI1_MEM_S_FMEM_HYPERBUS_CA_M  (SPI1_MEM_S_FMEM_HYPERBUS_CA_V << SPI1_MEM_S_FMEM_HYPERBUS_CA_S)
#define SPI1_MEM_S_FMEM_HYPERBUS_CA_V  0x00000001U
#define SPI1_MEM_S_FMEM_HYPERBUS_CA_S  30

/** SPI1_MEM_S_TIMING_CALI_REG register
 *  SPI1 timing control register
 */
#define SPI1_MEM_S_TIMING_CALI_REG (DR_REG_PSRAM_MSPI1_BASE + 0x180)
/** SPI1_MEM_S_TIMING_CALI : R/W; bitpos: [1]; default: 0;
 *  The bit is used to enable timing auto-calibration for all reading operations.
 */
#define SPI1_MEM_S_TIMING_CALI    (BIT(1))
#define SPI1_MEM_S_TIMING_CALI_M  (SPI1_MEM_S_TIMING_CALI_V << SPI1_MEM_S_TIMING_CALI_S)
#define SPI1_MEM_S_TIMING_CALI_V  0x00000001U
#define SPI1_MEM_S_TIMING_CALI_S  1
/** SPI1_MEM_S_EXTRA_DUMMY_CYCLELEN : R/W; bitpos: [4:2]; default: 0;
 *  add extra dummy spi clock cycle length for spi clock calibration.
 */
#define SPI1_MEM_S_EXTRA_DUMMY_CYCLELEN    0x00000007U
#define SPI1_MEM_S_EXTRA_DUMMY_CYCLELEN_M  (SPI1_MEM_S_EXTRA_DUMMY_CYCLELEN_V << SPI1_MEM_S_EXTRA_DUMMY_CYCLELEN_S)
#define SPI1_MEM_S_EXTRA_DUMMY_CYCLELEN_V  0x00000007U
#define SPI1_MEM_S_EXTRA_DUMMY_CYCLELEN_S  2

/** SPI1_MEM_S_CLOCK_GATE_REG register
 *  SPI1 clk_gate register
 */
#define SPI1_MEM_S_CLOCK_GATE_REG (DR_REG_PSRAM_MSPI1_BASE + 0x200)
/** SPI1_MEM_S_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Register clock gate enable signal. 1: Enable. 0: Disable.
 */
#define SPI1_MEM_S_CLK_EN    (BIT(0))
#define SPI1_MEM_S_CLK_EN_M  (SPI1_MEM_S_CLK_EN_V << SPI1_MEM_S_CLK_EN_S)
#define SPI1_MEM_S_CLK_EN_V  0x00000001U
#define SPI1_MEM_S_CLK_EN_S  0

/** SPI1_MEM_S_DATE_REG register
 *  Version control register
 */
#define SPI1_MEM_S_DATE_REG (DR_REG_PSRAM_MSPI1_BASE + 0x3fc)
/** SPI1_MEM_S_DATE : R/W; bitpos: [27:0]; default: 34673216;
 *  Version control register
 */
#define SPI1_MEM_S_DATE    0x0FFFFFFFU
#define SPI1_MEM_S_DATE_M  (SPI1_MEM_S_DATE_V << SPI1_MEM_S_DATE_S)
#define SPI1_MEM_S_DATE_V  0x0FFFFFFFU
#define SPI1_MEM_S_DATE_S  0

#ifdef __cplusplus
}
#endif
