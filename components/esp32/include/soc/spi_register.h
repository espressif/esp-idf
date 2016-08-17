// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _SOC_SPI_REG_H_
#define _SOC_SPI_REG_H_

#include "soc.h"

#define SPI_CMD( i )                            (REG_SPI_BASE( i ) + 0x0)
#define SPI_FLASH_READ (BIT(31))
#define SPI_FLASH_WREN (BIT(30))
#define SPI_FLASH_WRDI (BIT(29))
#define SPI_FLASH_RDID (BIT(28))
#define SPI_FLASH_RDSR (BIT(27))
#define SPI_FLASH_WRSR (BIT(26))
#define SPI_FLASH_PP (BIT(25))
#define SPI_FLASH_SE (BIT(24))
#define SPI_FLASH_BE (BIT(23))
#define SPI_FLASH_CE (BIT(22))
#define SPI_FLASH_DP (BIT(21))
#define SPI_FLASH_RES (BIT(20))
#define SPI_FLASH_HPM (BIT(19))
#define SPI_USR (BIT(18))
#define SPI_FLASH_PES (BIT(17))
#define SPI_FLASH_PER (BIT(16))
#define SPI_ADDR( i )                           (REG_SPI_BASE( i ) + 0x4)
#define SPI_CTRL( i )                           (REG_SPI_BASE( i ) + 0x8)
#define SPI_WR_BIT_ORDER (BIT(26))
#define SPI_RD_BIT_ORDER (BIT(25))
#define SPI_FREAD_QIO (BIT(24))
#define SPI_FREAD_DIO (BIT(23))
#define SPI_WRSR_2B (BIT(22))
#define SPI_WP_REG (BIT(21))
#define SPI_FREAD_QUAD (BIT(20))
#define SPI_RESANDRES (BIT(15))
#define SPI_FREAD_DUAL (BIT(14))
#define SPI_FASTRD_MODE (BIT(13))
#define SPI_WAIT_FLASH_IDLE_EN (BIT(12))
#define SPI_TX_CRC_EN (BIT(11))
#define SPI_FCS_CRC_EN (BIT(10))
#define SPI_CTRL1( i )                          (REG_SPI_BASE( i ) + 0xC)
#define SPI_CS_HOLD_DELAY 0x0000000F
#define SPI_CS_HOLD_DELAY_S 28
#define SPI_CS_HOLD_DELAY_RES 0x00000FFF
#define SPI_CS_HOLD_DELAY_RES_S 16
#define SPI_RD_STATUS( i )                      (REG_SPI_BASE( i ) + 0x10)
#define SPI_STATUS_EXT 0x000000FF
#define SPI_STATUS_EXT_S 24
#define SPI_WB_MODE 0x000000FF
#define SPI_WB_MODE_S 16
#define SPI_STATUS 0x0000FFFF
#define SPI_STATUS_S 0

#define SPI_CTRL2( i )                          (REG_SPI_BASE( i ) + 0x14)
#define SPI_CS_DELAY_NUM 0x0000000F
#define SPI_CS_DELAY_NUM_S 28
#define SPI_CS_DELAY_MODE 0x00000003
#define SPI_CS_DELAY_MODE_S 26
#define SPI_MOSI_DELAY_NUM 0x00000007
#define SPI_MOSI_DELAY_NUM_S 23
#define SPI_MOSI_DELAY_MODE 0x00000003
#define SPI_MOSI_DELAY_MODE_S 21
#define SPI_MISO_DELAY_NUM 0x00000007
#define SPI_MISO_DELAY_NUM_S 18
#define SPI_MISO_DELAY_MODE 0x00000003
#define SPI_MISO_DELAY_MODE_S 16
#define SPI_CK_OUT_HIGH_MODE 0x0000000F
#define SPI_CK_OUT_HIGH_MODE_S 12
#define SPI_CK_OUT_LOW_MODE 0x0000000F
#define SPI_CK_OUT_LOW_MODE_S 8
#define SPI_HOLD_TIME 0x0000000F
#define SPI_HOLD_TIME_S 4
#define SPI_SETUP_TIME 0x0000000F
#define SPI_SETUP_TIME_S 0

#define SPI_CLOCK( i )                          (REG_SPI_BASE( i ) + 0x18)
#define SPI_CLK_EQU_SYSCLK (BIT(31))
#define SPI_CLKDIV_PRE 0x00001FFF
#define SPI_CLKDIV_PRE_S 18
#define SPI_CLKCNT_N 0x0000003F
#define SPI_CLKCNT_N_S 12
#define SPI_CLKCNT_H 0x0000003F
#define SPI_CLKCNT_H_S 6
#define SPI_CLKCNT_L 0x0000003F
#define SPI_CLKCNT_L_S 0

#define SPI_USER( i )                           (REG_SPI_BASE( i ) + 0x1C)
#define SPI_USR_COMMAND (BIT(31))
#define SPI_USR_ADDR (BIT(30))
#define SPI_USR_DUMMY (BIT(29))
#define SPI_USR_MISO (BIT(28))
#define SPI_USR_MOSI (BIT(27))
#define SPI_USR_DUMMY_IDLE (BIT(26))
#define SPI_USR_MOSI_HIGHPART (BIT(25))
#define SPI_USR_MISO_HIGHPART (BIT(24))
#define SPI_USR_PREP_HOLD (BIT(23))
#define SPI_USR_CMD_HOLD (BIT(22))
#define SPI_USR_ADDR_HOLD (BIT(21))
#define SPI_USR_DUMMY_HOLD (BIT(20))
#define SPI_USR_DIN_HOLD (BIT(19))
#define SPI_USR_DOUT_HOLD (BIT(18))
#define SPI_USR_HOLD_POL (BIT(17))
#define SPI_SIO (BIT(16))
#define SPI_FWRITE_QIO (BIT(15))
#define SPI_FWRITE_DIO (BIT(14))
#define SPI_FWRITE_QUAD (BIT(13))
#define SPI_FWRITE_DUAL (BIT(12))
#define SPI_WR_BYTE_ORDER (BIT(11))
#define SPI_RD_BYTE_ORDER (BIT(10))
#define SPI_CK_OUT_EDGE (BIT(7))
#define SPI_CK_I_EDGE (BIT(6))
#define SPI_CS_SETUP (BIT(5))
#define SPI_CS_HOLD (BIT(4))
#define SPI_DOUTDIN (BIT(0))

#define SPI_USER1( i )                          (REG_SPI_BASE( i ) + 0x20)
#define SPI_USR_ADDR_BITLEN 0x0000003F
#define SPI_USR_ADDR_BITLEN_S 26
#define SPI_USR_DUMMY_CYCLELEN 0x000000FF
#define SPI_USR_DUMMY_CYCLELEN_S 0

#define SPI_USER2( i )                          (REG_SPI_BASE( i ) + 0x24)
#define SPI_USR_COMMAND_BITLEN 0x0000000F
#define SPI_USR_COMMAND_BITLEN_S 28
#define SPI_USR_COMMAND_VALUE 0x0000FFFF
#define SPI_USR_COMMAND_VALUE_S 0

#define SPI_MOSI_DLEN( i )                      (REG_SPI_BASE( i ) + 0x28)
#define SPI_USR_MOSI_DBITLEN 0x00FFFFFF
#define SPI_USR_MOSI_DBITLEN_S 0

#define SPI_MISO_DLEN( i )                      (REG_SPI_BASE( i ) + 0x2C)
#define SPI_USR_MISO_DBITLEN 0x00FFFFFF
#define SPI_USR_MISO_DBITLEN_S 0

#define SPI_SLV_WR_STATUS( i )                  (REG_SPI_BASE( i ) + 0x30)
#define SPI_PIN( i )                            (REG_SPI_BASE( i ) + 0x34)
#define SPI_CS_KEEP_ACTIVE (BIT(30))
#define SPI_CK_IDLE_EDGE (BIT(29))
#define SPI_CSCK_SEL (BIT(19))
#define SPI_MASTER_CK_SEL 0x0000001F
#define SPI_MASTER_CK_SEL_S 11
#define SPI_MASTER_CS_POL 0x0000001F
#define SPI_MASTER_CS_POL_S 6
#define SPI_CK_DIS (BIT(5))
#define SPI_CS2_DIS (BIT(2))
#define SPI_CS1_DIS (BIT(1))
#define SPI_CS0_DIS (BIT(0))

#define SPI_SLAVE( i )                          (REG_SPI_BASE( i ) + 0x38)
#define SPI_SYNC_RESET (BIT(31))
#define SPI_SLAVE_MODE (BIT(30))
#define SPI_SLV_WR_RD_BUF_EN (BIT(29))
#define SPI_SLV_WR_RD_STA_EN (BIT(28))
#define SPI_SLV_CMD_DEFINE (BIT(27))
#define SPI_TRANS_CNT 0x0000000F
#define SPI_TRANS_CNT_S 23
#define SPI_SLV_LAST_STATE 0x00000007
#define SPI_SLV_LAST_STATE_S 20
#define SPI_SLV_LAST_COMMAND 0x00000007
#define SPI_SLV_LAST_COMMAND_S 17
#define SPI_CS_I_MODE 0x00000003
#define SPI_CS_I_MODE_S 10
#define SPI_INT_EN 0x0000001F
#define SPI_INT_EN_S 5
#define SPI_TRANS_DONE (BIT(4))
#define SPI_SLV_WR_STA_DONE (BIT(3))
#define SPI_SLV_RD_STA_DONE (BIT(2))
#define SPI_SLV_WR_BUF_DONE (BIT(1))
#define SPI_SLV_RD_BUF_DONE (BIT(0))

#define SPI_SLAVE1( i )                         (REG_SPI_BASE( i ) + 0x3C)
#define SPI_SLV_STATUS_BITLEN 0x0000001F
#define SPI_SLV_STATUS_BITLEN_S 27
#define SPI_SLV_STATUS_FAST_EN (BIT(26))
#define SPI_SLV_STATUS_READBACK (BIT(25))
#define SPI_SLV_RD_ADDR_BITLEN 0x0000003F
#define SPI_SLV_RD_ADDR_BITLEN_S 10
#define SPI_SLV_WR_ADDR_BITLEN 0x0000003F
#define SPI_SLV_WR_ADDR_BITLEN_S 4
#define SPI_SLV_WRSTA_DUMMY_EN (BIT(3))
#define SPI_SLV_RDSTA_DUMMY_EN (BIT(2))
#define SPI_SLV_WRBUF_DUMMY_EN (BIT(1))
#define SPI_SLV_RDBUF_DUMMY_EN (BIT(0))

#define SPI_SLAVE2( i )                         (REG_SPI_BASE( i ) + 0x40)
#define SPI_SLV_WRBUF_DUMMY_CYCLELEN 0x000000FF
#define SPI_SLV_WRBUF_DUMMY_CYCLELEN_S 24
#define SPI_SLV_RDBUF_DUMMY_CYCLELEN 0x000000FF
#define SPI_SLV_RDBUF_DUMMY_CYCLELEN_S 16
#define SPI_SLV_WRSTA_DUMMY_CYCLELEN 0x000000FF
#define SPI_SLV_WRSTA_DUMMY_CYCLELEN_S 8
#define SPI_SLV_RDSTA_DUMMY_CYCLELEN 0x000000FF
#define SPI_SLV_RDSTA_DUMMY_CYCLELEN_S 0

#define SPI_SLAVE3( i )                         (REG_SPI_BASE( i ) + 0x44)
#define SPI_SLV_WRSTA_CMD_VALUE 0x000000FF
#define SPI_SLV_WRSTA_CMD_VALUE_S 24
#define SPI_SLV_RDSTA_CMD_VALUE 0x000000FF
#define SPI_SLV_RDSTA_CMD_VALUE_S 16
#define SPI_SLV_WRBUF_CMD_VALUE 0x000000FF
#define SPI_SLV_WRBUF_CMD_VALUE_S 8
#define SPI_SLV_RDBUF_CMD_VALUE 0x000000FF
#define SPI_SLV_RDBUF_CMD_VALUE_S 0

#define SPI_SLV_WRBUF_DLEN( i )                 (REG_SPI_BASE( i ) + 0x48)
#define SPI_SLV_WRBUF_DBITLEN 0x00FFFFFF
#define SPI_SLV_WRBUF_DBITLEN_S 0

#define SPI_SLV_RDBUF_DLEN( i )                 (REG_SPI_BASE( i ) + 0x4C)
#define SPI_SLV_RDBUF_DBITLEN 0x00FFFFFF
#define SPI_SLV_RDBUF_DBITLEN_S 0

#define SPI_CACHE_FCTRL( i )                    (REG_SPI_BASE( i ) + 0x50)
#define SPI_CACHE_FLASH_PES_EN (BIT(3))
#define SPI_CACHE_FLASH_USR_CMD (BIT(2))
#define SPI_CACHE_USR_CMD_4BYTE (BIT(1))
#define SPI_CACHE_REQ_EN (BIT(0))

#define SPI_CACHE_SCTRL( i )                    (REG_SPI_BASE( i ) + 0x54)
#define SPI_CACHE_SRAM_USR_WCMD (BIT(28))
#define SPI_SRAM_ADDR_BITLEN 0x0000003F
#define SPI_SRAM_ADDR_BITLEN_S 22
#define SPI_SRAM_DUMMY_CYCLELEN 0x000000FF
#define SPI_SRAM_DUMMY_CYCLELEN_S 14
#define SPI_SRAM_BYTES_LEN 0x000000FF
#define SPI_SRAM_BYTES_LEN_S 6
#define SPI_CACHE_SRAM_USR_RCMD (BIT(5))
#define SPI_USR_RD_SRAM_DUMMY (BIT(4))
#define SPI_USR_WR_SRAM_DUMMY (BIT(3))
#define SPI_USR_SRAM_QIO (BIT(2))
#define SPI_USR_SRAM_DIO (BIT(1))
#define SPI_SRAM_CMD( i )                       (REG_SPI_BASE( i ) + 0x58)
#define SPI_SRAM_RSTIO (BIT(4))
#define SPI_SRAM_QIO (BIT(1))
#define SPI_SRAM_DIO (BIT(0))

#define SPI_SRAM_DRD_CMD( i )                   (REG_SPI_BASE( i ) + 0x5C)
#define SPI_CACHE_SRAM_USR_RD_CMD_BITLEN 0x0000000F
#define SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_S 28
#define SPI_CACHE_SRAM_USR_RD_CMD_VALUE 0x0000FFFF
#define SPI_CACHE_SRAM_USR_RD_CMD_VALUE_S 0

#define SPI_SRAM_DWR_CMD( i )                   (REG_SPI_BASE( i ) + 0x60)
#define SPI_CACHE_SRAM_USR_WR_CMD_BITLEN 0x0000000F
#define SPI_CACHE_SRAM_USR_WR_CMD_BITLEN_S 28
#define SPI_CACHE_SRAM_USR_WR_CMD_VALUE 0x0000FFFF
#define SPI_CACHE_SRAM_USR_WR_CMD_VALUE_S 0

#define SPI_SLV_RD_BIT( i )                     (REG_SPI_BASE( i ) + 0x64)
#define SPI_SLV_RDATA_BIT 0x00FFFFFF
#define SPI_SLV_RDATA_BIT_S 0

#define SPI_W0( i )                             (REG_SPI_BASE( i ) + 0x80)
#define SPI_W1( i )                             (REG_SPI_BASE( i ) + 0x84)
#define SPI_W2( i )                             (REG_SPI_BASE( i ) + 0x88)
#define SPI_W3( i )                             (REG_SPI_BASE( i ) + 0x8C)
#define SPI_W4( i )                             (REG_SPI_BASE( i ) + 0x90)
#define SPI_W5( i )                             (REG_SPI_BASE( i ) + 0x94)
#define SPI_W6( i )                             (REG_SPI_BASE( i ) + 0x98)
#define SPI_W7( i )                             (REG_SPI_BASE( i ) + 0x9C)
#define SPI_W8( i )                             (REG_SPI_BASE( i ) + 0xA0)
#define SPI_W9( i )                             (REG_SPI_BASE( i ) + 0xA4)
#define SPI_W10( i )                            (REG_SPI_BASE( i ) + 0xA8)
#define SPI_W11( i )                            (REG_SPI_BASE( i ) + 0xAC)
#define SPI_W12( i )                            (REG_SPI_BASE( i ) + 0xB0)
#define SPI_W13( i )                            (REG_SPI_BASE( i ) + 0xB4)
#define SPI_W14( i )                            (REG_SPI_BASE( i ) + 0xB8)
#define SPI_W15( i )                            (REG_SPI_BASE( i ) + 0xBC)
#define SPI_TX_CRC( i )                         (REG_SPI_BASE( i ) + 0xC0)
#define SPI_EXT0( i )                           (REG_SPI_BASE( i ) + 0xF0)
#define SPI_T_PP_ENA (BIT(31))
#define SPI_T_PP_SHIFT 0x0000000F
#define SPI_T_PP_SHIFT_S 16
#define SPI_T_PP_TIME 0x00000FFF
#define SPI_T_PP_TIME_S 0

#define SPI_EXT1( i )                           (REG_SPI_BASE( i ) + 0xF4)
#define SPI_T_ERASE_ENA (BIT(31))
#define SPI_T_ERASE_SHIFT 0x0000000F
#define SPI_T_ERASE_SHIFT_S 16
#define SPI_T_ERASE_TIME 0x00000FFF
#define SPI_T_ERASE_TIME_S 0

#define SPI_EXT2( i )                           (REG_SPI_BASE( i ) + 0xF8)
#define SPI_ST 0x00000007
#define SPI_ST_S 0

#define SPI_EXT3( i )                           (REG_SPI_BASE( i ) + 0xFC)
#define SPI_INT_HOLD_ENA 0x00000003
#define SPI_INT_HOLD_ENA_S 0

#define SPI_DMA_CONF( i )                       (REG_SPI_BASE( i ) + 0x100)
#define SPI_DMA_CONTINUE (BIT(16))
#define SPI_DMA_TX_STOP (BIT(15))
#define SPI_DMA_RX_STOP (BIT(14))
#define SPI_OUT_DATA_BURST_EN (BIT(12))
#define SPI_INDSCR_BURST_EN (BIT(11))
#define SPI_OUTDSCR_BURST_EN (BIT(10))
#define SPI_OUT_EOF_MODE (BIT(9))
#define SPI_OUT_AUTO_WRBACK (BIT(8))
#define SPI_OUT_LOOP_TEST (BIT(7))
#define SPI_IN_LOOP_TEST (BIT(6))
#define SPI_AHBM_RST (BIT(5))
#define SPI_AHBM_FIFO_RST (BIT(4))
#define SPI_OUT_RST (BIT(3))
#define SPI_IN_RST (BIT(2))
#define SPI_DMA_OUT_LINK( i )                   (REG_SPI_BASE( i ) + 0x104)
#define SPI_OUTLINK_RESTART (BIT(30))
#define SPI_OUTLINK_START (BIT(29))
#define SPI_OUTLINK_STOP (BIT(28))
#define SPI_OUTLINK_ADDR 0x000FFFFF
#define SPI_OUTLINK_ADDR_S 0

#define SPI_DMA_IN_LINK( i )                    (REG_SPI_BASE( i ) + 0x108)
#define SPI_INLINK_RESTART (BIT(30))
#define SPI_INLINK_START (BIT(29))
#define SPI_INLINK_STOP (BIT(28))
#define SPI_INLINK_AUTO_RET (BIT(20))
#define SPI_INLINK_ADDR 0x000FFFFF
#define SPI_INLINK_ADDR_S 0

#define SPI_DMA_STATUS( i )                     (REG_SPI_BASE( i ) + 0x10C)
#define SPI_DMA_TX_EN (BIT(1))
#define SPI_DMA_RX_EN (BIT(0))

#define SPI_DMA_INT_ENA( i )                    (REG_SPI_BASE( i ) + 0x110)
#define SPI_OUT_TOTAL_EOF_INT_ENA (BIT(8))
#define SPI_OUT_EOF_INT_ENA (BIT(7))
#define SPI_OUT_DONE_INT_ENA (BIT(6))
#define SPI_IN_SUC_EOF_INT_ENA (BIT(5))
#define SPI_IN_ERR_EOF_INT_ENA (BIT(4))
#define SPI_IN_DONE_INT_ENA (BIT(3))
#define SPI_INLINK_DSCR_ERROR_INT_ENA (BIT(2))
#define SPI_OUTLINK_DSCR_ERROR_INT_ENA (BIT(1))
#define SPI_INLINK_DSCR_EMPTY_INT_ENA (BIT(0))

#define SPI_DMA_INT_RAW( i )                    (REG_SPI_BASE( i ) + 0x114)
#define SPI_OUT_TOTAL_EOF_INT_RAW (BIT(8))
#define SPI_OUT_EOF_INT_RAW (BIT(7))
#define SPI_OUT_DONE_INT_RAW (BIT(6))
#define SPI_IN_SUC_EOF_INT_RAW (BIT(5))
#define SPI_IN_ERR_EOF_INT_RAW (BIT(4))
#define SPI_IN_DONE_INT_RAW (BIT(3))
#define SPI_INLINK_DSCR_ERROR_INT_RAW (BIT(2))
#define SPI_OUTLINK_DSCR_ERROR_INT_RAW (BIT(1))
#define SPI_INLINK_DSCR_EMPTY_INT_RAW (BIT(0))

#define SPI_DMA_INT_ST( i )                     (REG_SPI_BASE( i ) + 0x118)
#define SPI_OUT_TOTAL_EOF_INT_ST (BIT(8))
#define SPI_OUT_EOF_INT_ST (BIT(7))
#define SPI_OUT_DONE_INT_ST (BIT(6))
#define SPI_IN_SUC_EOF_INT_ST (BIT(5))
#define SPI_IN_ERR_EOF_INT_ST (BIT(4))
#define SPI_IN_DONE_INT_ST (BIT(3))
#define SPI_INLINK_DSCR_ERROR_INT_ST (BIT(2))
#define SPI_OUTLINK_DSCR_ERROR_INT_ST (BIT(1))
#define SPI_INLINK_DSCR_EMPTY_INT_ST (BIT(0))

#define SPI_DMA_INT_CLR( i )                    (REG_SPI_BASE( i ) + 0x11C)
#define SPI_OUT_TOTAL_EOF_INT_CLR (BIT(8))
#define SPI_OUT_EOF_INT_CLR (BIT(7))
#define SPI_OUT_DONE_INT_CLR (BIT(6))
#define SPI_IN_SUC_EOF_INT_CLR (BIT(5))
#define SPI_IN_ERR_EOF_INT_CLR (BIT(4))
#define SPI_IN_DONE_INT_CLR (BIT(3))
#define SPI_INLINK_DSCR_ERROR_INT_CLR (BIT(2))
#define SPI_OUTLINK_DSCR_ERROR_INT_CLR (BIT(1))
#define SPI_INLINK_DSCR_EMPTY_INT_CLR (BIT(0))

#define SPI_IN_ERR_EOF_DES_ADDR( i )            (REG_SPI_BASE( i ) + 0x120)
#define SPI_IN_SUC_EOF_DES_ADDR( i )            (REG_SPI_BASE( i ) + 0x124)
#define SPI_INLINK_DSCR( i )                    (REG_SPI_BASE( i ) + 0x128)
#define SPI_INLINK_DSCR_BF0( i )                (REG_SPI_BASE( i ) + 0x12C)
#define SPI_INLINK_DSCR_BF1( i )                (REG_SPI_BASE( i ) + 0x130)
#define SPI_OUT_EOF_BFR_DES_ADDR( i )           (REG_SPI_BASE( i ) + 0x134)
#define SPI_OUT_EOF_DES_ADDR( i )               (REG_SPI_BASE( i ) + 0x138)
#define SPI_OUTLINK_DSCR( i )                   (REG_SPI_BASE( i ) + 0x13C)
#define SPI_OUTLINK_DSCR_BF0( i )               (REG_SPI_BASE( i ) + 0x140)
#define SPI_OUTLINK_DSCR_BF1( i )               (REG_SPI_BASE( i ) + 0x144)
#define SPI_DMA_RSTATUS( i )                    (REG_SPI_BASE( i ) + 0x148)
#define SPI_DMA_TSTATUS( i )                    (REG_SPI_BASE( i ) + 0x14C)
#define SPI_REG_DATE( i )                       (REG_SPI_BASE( i ) + 0x3FC)
#define SPI_DATE 0x0FFFFFFF
#define SPI_DATE_S 0
#define SPI_DATE_VERSION 0x1604290
 
#endif /* _SOC_SPI_REG_H_ */
