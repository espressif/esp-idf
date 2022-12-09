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
#ifndef _SOC_MCP_REG_H_
#define _SOC_MCP_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"
#define MCP_INT_RAW_REG          (DR_REG_MCP_BASE + 0x0000)
/* MCP_CRC_DONE_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define MCP_CRC_DONE_INT_RAW  (BIT(8))
#define MCP_CRC_DONE_INT_RAW_M  (BIT(8))
#define MCP_CRC_DONE_INT_RAW_V  0x1
#define MCP_CRC_DONE_INT_RAW_S  8
/* MCP_OUT_TOTAL_EOF_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define MCP_OUT_TOTAL_EOF_INT_RAW  (BIT(7))
#define MCP_OUT_TOTAL_EOF_INT_RAW_M  (BIT(7))
#define MCP_OUT_TOTAL_EOF_INT_RAW_V  0x1
#define MCP_OUT_TOTAL_EOF_INT_RAW_S  7
/* MCP_IN_DSCR_EMPTY_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define MCP_IN_DSCR_EMPTY_INT_RAW  (BIT(6))
#define MCP_IN_DSCR_EMPTY_INT_RAW_M  (BIT(6))
#define MCP_IN_DSCR_EMPTY_INT_RAW_V  0x1
#define MCP_IN_DSCR_EMPTY_INT_RAW_S  6
/* MCP_OUT_DSCR_ERR_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define MCP_OUT_DSCR_ERR_INT_RAW  (BIT(5))
#define MCP_OUT_DSCR_ERR_INT_RAW_M  (BIT(5))
#define MCP_OUT_DSCR_ERR_INT_RAW_V  0x1
#define MCP_OUT_DSCR_ERR_INT_RAW_S  5
/* MCP_IN_DSCR_ERR_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define MCP_IN_DSCR_ERR_INT_RAW  (BIT(4))
#define MCP_IN_DSCR_ERR_INT_RAW_M  (BIT(4))
#define MCP_IN_DSCR_ERR_INT_RAW_V  0x1
#define MCP_IN_DSCR_ERR_INT_RAW_S  4
/* MCP_OUT_EOF_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define MCP_OUT_EOF_INT_RAW  (BIT(3))
#define MCP_OUT_EOF_INT_RAW_M  (BIT(3))
#define MCP_OUT_EOF_INT_RAW_V  0x1
#define MCP_OUT_EOF_INT_RAW_S  3
/* MCP_OUT_DONE_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define MCP_OUT_DONE_INT_RAW  (BIT(2))
#define MCP_OUT_DONE_INT_RAW_M  (BIT(2))
#define MCP_OUT_DONE_INT_RAW_V  0x1
#define MCP_OUT_DONE_INT_RAW_S  2
/* MCP_IN_SUC_EOF_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define MCP_IN_SUC_EOF_INT_RAW  (BIT(1))
#define MCP_IN_SUC_EOF_INT_RAW_M  (BIT(1))
#define MCP_IN_SUC_EOF_INT_RAW_V  0x1
#define MCP_IN_SUC_EOF_INT_RAW_S  1
/* MCP_IN_DONE_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define MCP_IN_DONE_INT_RAW  (BIT(0))
#define MCP_IN_DONE_INT_RAW_M  (BIT(0))
#define MCP_IN_DONE_INT_RAW_V  0x1
#define MCP_IN_DONE_INT_RAW_S  0

#define MCP_INT_ST_REG          (DR_REG_MCP_BASE + 0x0004)
/* MCP_CRC_DONE_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define MCP_CRC_DONE_INT_ST  (BIT(8))
#define MCP_CRC_DONE_INT_ST_M  (BIT(8))
#define MCP_CRC_DONE_INT_ST_V  0x1
#define MCP_CRC_DONE_INT_ST_S  8
/* MCP_OUT_TOTAL_EOF_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define MCP_OUT_TOTAL_EOF_INT_ST  (BIT(7))
#define MCP_OUT_TOTAL_EOF_INT_ST_M  (BIT(7))
#define MCP_OUT_TOTAL_EOF_INT_ST_V  0x1
#define MCP_OUT_TOTAL_EOF_INT_ST_S  7
/* MCP_IN_DSCR_EMPTY_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define MCP_IN_DSCR_EMPTY_INT_ST  (BIT(6))
#define MCP_IN_DSCR_EMPTY_INT_ST_M  (BIT(6))
#define MCP_IN_DSCR_EMPTY_INT_ST_V  0x1
#define MCP_IN_DSCR_EMPTY_INT_ST_S  6
/* MCP_OUT_DSCR_ERR_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define MCP_OUT_DSCR_ERR_INT_ST  (BIT(5))
#define MCP_OUT_DSCR_ERR_INT_ST_M  (BIT(5))
#define MCP_OUT_DSCR_ERR_INT_ST_V  0x1
#define MCP_OUT_DSCR_ERR_INT_ST_S  5
/* MCP_IN_DSCR_ERR_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define MCP_IN_DSCR_ERR_INT_ST  (BIT(4))
#define MCP_IN_DSCR_ERR_INT_ST_M  (BIT(4))
#define MCP_IN_DSCR_ERR_INT_ST_V  0x1
#define MCP_IN_DSCR_ERR_INT_ST_S  4
/* MCP_OUT_EOF_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define MCP_OUT_EOF_INT_ST  (BIT(3))
#define MCP_OUT_EOF_INT_ST_M  (BIT(3))
#define MCP_OUT_EOF_INT_ST_V  0x1
#define MCP_OUT_EOF_INT_ST_S  3
/* MCP_OUT_DONE_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define MCP_OUT_DONE_INT_ST  (BIT(2))
#define MCP_OUT_DONE_INT_ST_M  (BIT(2))
#define MCP_OUT_DONE_INT_ST_V  0x1
#define MCP_OUT_DONE_INT_ST_S  2
/* MCP_IN_SUC_EOF_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define MCP_IN_SUC_EOF_INT_ST  (BIT(1))
#define MCP_IN_SUC_EOF_INT_ST_M  (BIT(1))
#define MCP_IN_SUC_EOF_INT_ST_V  0x1
#define MCP_IN_SUC_EOF_INT_ST_S  1
/* MCP_IN_DONE_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define MCP_IN_DONE_INT_ST  (BIT(0))
#define MCP_IN_DONE_INT_ST_M  (BIT(0))
#define MCP_IN_DONE_INT_ST_V  0x1
#define MCP_IN_DONE_INT_ST_S  0

#define MCP_INT_ENA_REG          (DR_REG_MCP_BASE + 0x008)
/* MCP_CRC_DONE_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define MCP_CRC_DONE_INT_ENA  (BIT(8))
#define MCP_CRC_DONE_INT_ENA_M  (BIT(8))
#define MCP_CRC_DONE_INT_ENA_V  0x1
#define MCP_CRC_DONE_INT_ENA_S  8
/* MCP_OUT_TOTAL_EOF_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define MCP_OUT_TOTAL_EOF_INT_ENA  (BIT(7))
#define MCP_OUT_TOTAL_EOF_INT_ENA_M  (BIT(7))
#define MCP_OUT_TOTAL_EOF_INT_ENA_V  0x1
#define MCP_OUT_TOTAL_EOF_INT_ENA_S  7
/* MCP_IN_DSCR_EMPTY_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define MCP_IN_DSCR_EMPTY_INT_ENA  (BIT(6))
#define MCP_IN_DSCR_EMPTY_INT_ENA_M  (BIT(6))
#define MCP_IN_DSCR_EMPTY_INT_ENA_V  0x1
#define MCP_IN_DSCR_EMPTY_INT_ENA_S  6
/* MCP_OUT_DSCR_ERR_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define MCP_OUT_DSCR_ERR_INT_ENA  (BIT(5))
#define MCP_OUT_DSCR_ERR_INT_ENA_M  (BIT(5))
#define MCP_OUT_DSCR_ERR_INT_ENA_V  0x1
#define MCP_OUT_DSCR_ERR_INT_ENA_S  5
/* MCP_IN_DSCR_ERR_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define MCP_IN_DSCR_ERR_INT_ENA  (BIT(4))
#define MCP_IN_DSCR_ERR_INT_ENA_M  (BIT(4))
#define MCP_IN_DSCR_ERR_INT_ENA_V  0x1
#define MCP_IN_DSCR_ERR_INT_ENA_S  4
/* MCP_OUT_EOF_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define MCP_OUT_EOF_INT_ENA  (BIT(3))
#define MCP_OUT_EOF_INT_ENA_M  (BIT(3))
#define MCP_OUT_EOF_INT_ENA_V  0x1
#define MCP_OUT_EOF_INT_ENA_S  3
/* MCP_OUT_DONE_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define MCP_OUT_DONE_INT_ENA  (BIT(2))
#define MCP_OUT_DONE_INT_ENA_M  (BIT(2))
#define MCP_OUT_DONE_INT_ENA_V  0x1
#define MCP_OUT_DONE_INT_ENA_S  2
/* MCP_IN_SUC_EOF_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define MCP_IN_SUC_EOF_INT_ENA  (BIT(1))
#define MCP_IN_SUC_EOF_INT_ENA_M  (BIT(1))
#define MCP_IN_SUC_EOF_INT_ENA_V  0x1
#define MCP_IN_SUC_EOF_INT_ENA_S  1
/* MCP_IN_DONE_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define MCP_IN_DONE_INT_ENA  (BIT(0))
#define MCP_IN_DONE_INT_ENA_M  (BIT(0))
#define MCP_IN_DONE_INT_ENA_V  0x1
#define MCP_IN_DONE_INT_ENA_S  0

#define MCP_INT_CLR_REG          (DR_REG_MCP_BASE + 0x000c)
/* MCP_CRC_DONE_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define MCP_CRC_DONE_INT_CLR  (BIT(8))
#define MCP_CRC_DONE_INT_CLR_M  (BIT(8))
#define MCP_CRC_DONE_INT_CLR_V  0x1
#define MCP_CRC_DONE_INT_CLR_S  8
/* MCP_OUT_TOTAL_EOF_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define MCP_OUT_TOTAL_EOF_INT_CLR  (BIT(7))
#define MCP_OUT_TOTAL_EOF_INT_CLR_M  (BIT(7))
#define MCP_OUT_TOTAL_EOF_INT_CLR_V  0x1
#define MCP_OUT_TOTAL_EOF_INT_CLR_S  7
/* MCP_IN_DSCR_EMPTY_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define MCP_IN_DSCR_EMPTY_INT_CLR  (BIT(6))
#define MCP_IN_DSCR_EMPTY_INT_CLR_M  (BIT(6))
#define MCP_IN_DSCR_EMPTY_INT_CLR_V  0x1
#define MCP_IN_DSCR_EMPTY_INT_CLR_S  6
/* MCP_OUT_DSCR_ERR_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define MCP_OUT_DSCR_ERR_INT_CLR  (BIT(5))
#define MCP_OUT_DSCR_ERR_INT_CLR_M  (BIT(5))
#define MCP_OUT_DSCR_ERR_INT_CLR_V  0x1
#define MCP_OUT_DSCR_ERR_INT_CLR_S  5
/* MCP_IN_DSCR_ERR_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define MCP_IN_DSCR_ERR_INT_CLR  (BIT(4))
#define MCP_IN_DSCR_ERR_INT_CLR_M  (BIT(4))
#define MCP_IN_DSCR_ERR_INT_CLR_V  0x1
#define MCP_IN_DSCR_ERR_INT_CLR_S  4
/* MCP_OUT_EOF_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define MCP_OUT_EOF_INT_CLR  (BIT(3))
#define MCP_OUT_EOF_INT_CLR_M  (BIT(3))
#define MCP_OUT_EOF_INT_CLR_V  0x1
#define MCP_OUT_EOF_INT_CLR_S  3
/* MCP_OUT_DONE_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define MCP_OUT_DONE_INT_CLR  (BIT(2))
#define MCP_OUT_DONE_INT_CLR_M  (BIT(2))
#define MCP_OUT_DONE_INT_CLR_V  0x1
#define MCP_OUT_DONE_INT_CLR_S  2
/* MCP_IN_SUC_EOF_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define MCP_IN_SUC_EOF_INT_CLR  (BIT(1))
#define MCP_IN_SUC_EOF_INT_CLR_M  (BIT(1))
#define MCP_IN_SUC_EOF_INT_CLR_V  0x1
#define MCP_IN_SUC_EOF_INT_CLR_S  1
/* MCP_IN_DONE_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define MCP_IN_DONE_INT_CLR  (BIT(0))
#define MCP_IN_DONE_INT_CLR_M  (BIT(0))
#define MCP_IN_DONE_INT_CLR_V  0x1
#define MCP_IN_DONE_INT_CLR_S  0

#define MCP_OUT_LINK_REG          (DR_REG_MCP_BASE + 0x0010)
/* MCP_OUTLINK_PARK : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define MCP_OUTLINK_PARK  (BIT(31))
#define MCP_OUTLINK_PARK_M  (BIT(31))
#define MCP_OUTLINK_PARK_V  0x1
#define MCP_OUTLINK_PARK_S  31
/* MCP_OUTLINK_RESTART : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define MCP_OUTLINK_RESTART  (BIT(30))
#define MCP_OUTLINK_RESTART_M  (BIT(30))
#define MCP_OUTLINK_RESTART_V  0x1
#define MCP_OUTLINK_RESTART_S  30
/* MCP_OUTLINK_START : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define MCP_OUTLINK_START  (BIT(29))
#define MCP_OUTLINK_START_M  (BIT(29))
#define MCP_OUTLINK_START_V  0x1
#define MCP_OUTLINK_START_S  29
/* MCP_OUTLINK_STOP : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define MCP_OUTLINK_STOP  (BIT(28))
#define MCP_OUTLINK_STOP_M  (BIT(28))
#define MCP_OUTLINK_STOP_V  0x1
#define MCP_OUTLINK_STOP_S  28
/* MCP_OUTLINK_ADDR : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: */
#define MCP_OUTLINK_ADDR  0x000FFFFF
#define MCP_OUTLINK_ADDR_M  ((MCP_OUTLINK_ADDR_V)<<(MCP_OUTLINK_ADDR_S))
#define MCP_OUTLINK_ADDR_V  0xFFFFF
#define MCP_OUTLINK_ADDR_S  0

#define MCP_IN_LINK_REG          (DR_REG_MCP_BASE + 0x0014)
/* MCP_INLINK_PARK : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define MCP_INLINK_PARK  (BIT(31))
#define MCP_INLINK_PARK_M  (BIT(31))
#define MCP_INLINK_PARK_V  0x1
#define MCP_INLINK_PARK_S  31
/* MCP_INLINK_RESTART : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define MCP_INLINK_RESTART  (BIT(30))
#define MCP_INLINK_RESTART_M  (BIT(30))
#define MCP_INLINK_RESTART_V  0x1
#define MCP_INLINK_RESTART_S  30
/* MCP_INLINK_START : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define MCP_INLINK_START  (BIT(29))
#define MCP_INLINK_START_M  (BIT(29))
#define MCP_INLINK_START_V  0x1
#define MCP_INLINK_START_S  29
/* MCP_INLINK_STOP : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define MCP_INLINK_STOP  (BIT(28))
#define MCP_INLINK_STOP_M  (BIT(28))
#define MCP_INLINK_STOP_V  0x1
#define MCP_INLINK_STOP_S  28
/* MCP_INLINK_ADDR : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: */
#define MCP_INLINK_ADDR  0x000FFFFF
#define MCP_INLINK_ADDR_M  ((MCP_INLINK_ADDR_V)<<(MCP_INLINK_ADDR_S))
#define MCP_INLINK_ADDR_V  0xFFFFF
#define MCP_INLINK_ADDR_S  0

#define MCP_OUT_EOF_DES_ADDR_REG          (DR_REG_MCP_BASE + 0x0018)
/* MCP_OUT_EOF_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define MCP_OUT_EOF_DES_ADDR  0xFFFFFFFF
#define MCP_OUT_EOF_DES_ADDR_M  ((MCP_OUT_EOF_DES_ADDR_V)<<(MCP_OUT_EOF_DES_ADDR_S))
#define MCP_OUT_EOF_DES_ADDR_V  0xFFFFFFFF
#define MCP_OUT_EOF_DES_ADDR_S  0

#define MCP_IN_EOF_DES_ADDR_REG          (DR_REG_MCP_BASE + 0x001c)
/* MCP_IN_SUC_EOF_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define MCP_IN_SUC_EOF_DES_ADDR  0xFFFFFFFF
#define MCP_IN_SUC_EOF_DES_ADDR_M  ((MCP_IN_SUC_EOF_DES_ADDR_V)<<(MCP_IN_SUC_EOF_DES_ADDR_S))
#define MCP_IN_SUC_EOF_DES_ADDR_V  0xFFFFFFFF
#define MCP_IN_SUC_EOF_DES_ADDR_S  0

#define MCP_OUT_EOF_BFR_DES_ADDR_REG          (DR_REG_MCP_BASE + 0x0020)
/* MCP_OUT_EOF_BFR_DES_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define MCP_OUT_EOF_BFR_DES_ADDR  0xFFFFFFFF
#define MCP_OUT_EOF_BFR_DES_ADDR_M  ((MCP_OUT_EOF_BFR_DES_ADDR_V)<<(MCP_OUT_EOF_BFR_DES_ADDR_S))
#define MCP_OUT_EOF_BFR_DES_ADDR_V  0xFFFFFFFF
#define MCP_OUT_EOF_BFR_DES_ADDR_S  0

#define MCP_INLINK_DSCR_REG          (DR_REG_MCP_BASE + 0x0024)
/* MCP_INLINK_DSCR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define MCP_INLINK_DSCR  0xFFFFFFFF
#define MCP_INLINK_DSCR_M  ((MCP_INLINK_DSCR_V)<<(MCP_INLINK_DSCR_S))
#define MCP_INLINK_DSCR_V  0xFFFFFFFF
#define MCP_INLINK_DSCR_S  0

#define MCP_INLINK_DSCR_BF0_REG          (DR_REG_MCP_BASE + 0x0028)
/* MCP_INLINK_DSCR_BF0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define MCP_INLINK_DSCR_BF0  0xFFFFFFFF
#define MCP_INLINK_DSCR_BF0_M  ((MCP_INLINK_DSCR_BF0_V)<<(MCP_INLINK_DSCR_BF0_S))
#define MCP_INLINK_DSCR_BF0_V  0xFFFFFFFF
#define MCP_INLINK_DSCR_BF0_S  0

#define MCP_INLINK_DSCR_BF1_REG          (DR_REG_MCP_BASE + 0x002c)
/* MCP_INLINK_DSCR_BF1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define MCP_INLINK_DSCR_BF1  0xFFFFFFFF
#define MCP_INLINK_DSCR_BF1_M  ((MCP_INLINK_DSCR_BF1_V)<<(MCP_INLINK_DSCR_BF1_S))
#define MCP_INLINK_DSCR_BF1_V  0xFFFFFFFF
#define MCP_INLINK_DSCR_BF1_S  0

#define MCP_OUTLINK_DSCR_REG          (DR_REG_MCP_BASE + 0x0030)
/* MCP_OUTLINK_DSCR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define MCP_OUTLINK_DSCR  0xFFFFFFFF
#define MCP_OUTLINK_DSCR_M  ((MCP_OUTLINK_DSCR_V)<<(MCP_OUTLINK_DSCR_S))
#define MCP_OUTLINK_DSCR_V  0xFFFFFFFF
#define MCP_OUTLINK_DSCR_S  0

#define MCP_OUTLINK_DSCR_BF0_REG          (DR_REG_MCP_BASE + 0x0034)
/* MCP_OUTLINK_DSCR_BF0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define MCP_OUTLINK_DSCR_BF0  0xFFFFFFFF
#define MCP_OUTLINK_DSCR_BF0_M  ((MCP_OUTLINK_DSCR_BF0_V)<<(MCP_OUTLINK_DSCR_BF0_S))
#define MCP_OUTLINK_DSCR_BF0_V  0xFFFFFFFF
#define MCP_OUTLINK_DSCR_BF0_S  0

#define MCP_OUTLINK_DSCR_BF1_REG          (DR_REG_MCP_BASE + 0x0038)
/* MCP_OUTLINK_DSCR_BF1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define MCP_OUTLINK_DSCR_BF1  0xFFFFFFFF
#define MCP_OUTLINK_DSCR_BF1_M  ((MCP_OUTLINK_DSCR_BF1_V)<<(MCP_OUTLINK_DSCR_BF1_S))
#define MCP_OUTLINK_DSCR_BF1_V  0xFFFFFFFF
#define MCP_OUTLINK_DSCR_BF1_S  0

#define MCP_CONF_REG          (DR_REG_MCP_BASE + 0x003c)
/* MCP_CLK_EN : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define MCP_CLK_EN  (BIT(31))
#define MCP_CLK_EN_M  (BIT(31))
#define MCP_CLK_EN_V  0x1
#define MCP_CLK_EN_S  31
/* MCP_CRC_OUT_REVERSE_EN : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define MCP_CRC_OUT_REVERSE_EN  (BIT(11))
#define MCP_CRC_OUT_REVERSE_EN_M  (BIT(11))
#define MCP_CRC_OUT_REVERSE_EN_V  0x1
#define MCP_CRC_OUT_REVERSE_EN_S  11
/* MCP_CRC_BIG_ENDIAN_EN : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this bit to reorder the bit of data which will be send to excute crc.*/
#define MCP_CRC_BIG_ENDIAN_EN  (BIT(10))
#define MCP_CRC_BIG_ENDIAN_EN_M  (BIT(10))
#define MCP_CRC_BIG_ENDIAN_EN_V  0x1
#define MCP_CRC_BIG_ENDIAN_EN_S  10
/* MCP_CRC_CAL_EN : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit enable crc calculation function.*/
#define MCP_CRC_CAL_EN  (BIT(9))
#define MCP_CRC_CAL_EN_M  (BIT(9))
#define MCP_CRC_CAL_EN_V  0x1
#define MCP_CRC_CAL_EN_S  9
/* MCP_CRC_CAL_RESET : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to reset crc calculation.*/
#define MCP_CRC_CAL_RESET  (BIT(8))
#define MCP_CRC_CAL_RESET_M  (BIT(8))
#define MCP_CRC_CAL_RESET_V  0x1
#define MCP_CRC_CAL_RESET_S  8
/* MCP_CHECK_OWNER : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to enable owner bit check in descriptor.*/
#define MCP_CHECK_OWNER  (BIT(7))
#define MCP_CHECK_OWNER_M  (BIT(7))
#define MCP_CHECK_OWNER_V  0x1
#define MCP_CHECK_OWNER_S  7
/* MCP_OUT_AUTO_WRBACK : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: this bit is used to write back out descriptor when hardware has
 already used this descriptor.*/
#define MCP_OUT_AUTO_WRBACK  (BIT(6))
#define MCP_OUT_AUTO_WRBACK_M  (BIT(6))
#define MCP_OUT_AUTO_WRBACK_V  0x1
#define MCP_OUT_AUTO_WRBACK_S  6
/* MCP_IN_OWNER : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: This is used to configure the owner bit in IN descriptor.*/
#define MCP_IN_OWNER  (BIT(5))
#define MCP_IN_OWNER_M  (BIT(5))
#define MCP_IN_OWNER_V  0x1
#define MCP_IN_OWNER_S  5
/* MCP_OUT_OWNER : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: This is used to configure the owner bit in OUT descriptor. This
 is effective only when you set reg_out_auto_wrback.*/
#define MCP_OUT_OWNER  (BIT(4))
#define MCP_OUT_OWNER_M  (BIT(4))
#define MCP_OUT_OWNER_V  0x1
#define MCP_OUT_OWNER_S  4
/* MCP_FIFO_RST : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define MCP_FIFO_RST  (BIT(3))
#define MCP_FIFO_RST_M  (BIT(3))
#define MCP_FIFO_RST_V  0x1
#define MCP_FIFO_RST_S  3
/* MCP_CMDFIFO_RST : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: set this bit to reset in_cmdfifo and out_cmdfifo.*/
#define MCP_CMDFIFO_RST  (BIT(2))
#define MCP_CMDFIFO_RST_M  (BIT(2))
#define MCP_CMDFIFO_RST_V  0x1
#define MCP_CMDFIFO_RST_S  2
/* MCP_OUT_RST : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: set this bit to reset out_inf state machine.*/
#define MCP_OUT_RST  (BIT(1))
#define MCP_OUT_RST_M  (BIT(1))
#define MCP_OUT_RST_V  0x1
#define MCP_OUT_RST_S  1
/* MCP_IN_RST : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: set this bit to reset in_inf state machine.*/
#define MCP_IN_RST  (BIT(0))
#define MCP_IN_RST_M  (BIT(0))
#define MCP_IN_RST_V  0x1
#define MCP_IN_RST_S  0

#define MCP_IN_ST_REG          (DR_REG_MCP_BASE + 0x0040)
/* MCP_FIFO_EMPTY : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define MCP_FIFO_EMPTY  (BIT(23))
#define MCP_FIFO_EMPTY_M  (BIT(23))
#define MCP_FIFO_EMPTY_V  0x1
#define MCP_FIFO_EMPTY_S  23
/* MCP_IN_STATE : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: */
#define MCP_IN_STATE  0x00000007
#define MCP_IN_STATE_M  ((MCP_IN_STATE_V)<<(MCP_IN_STATE_S))
#define MCP_IN_STATE_V  0x7
#define MCP_IN_STATE_S  20
/* MCP_IN_DSCR_STATE : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: */
#define MCP_IN_DSCR_STATE  0x00000003
#define MCP_IN_DSCR_STATE_M  ((MCP_IN_DSCR_STATE_V)<<(MCP_IN_DSCR_STATE_S))
#define MCP_IN_DSCR_STATE_V  0x3
#define MCP_IN_DSCR_STATE_S  18
/* MCP_INLINK_DSCR_ADDR : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: */
#define MCP_INLINK_DSCR_ADDR  0x0003FFFF
#define MCP_INLINK_DSCR_ADDR_M  ((MCP_INLINK_DSCR_ADDR_V)<<(MCP_INLINK_DSCR_ADDR_S))
#define MCP_INLINK_DSCR_ADDR_V  0x3FFFF
#define MCP_INLINK_DSCR_ADDR_S  0

#define MCP_OUT_ST_REG          (DR_REG_MCP_BASE + 0x0044)
/* MCP_FIFO_FULL : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define MCP_FIFO_FULL  (BIT(23))
#define MCP_FIFO_FULL_M  (BIT(23))
#define MCP_FIFO_FULL_V  0x1
#define MCP_FIFO_FULL_S  23
/* MCP_OUT_STATE : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: */
#define MCP_OUT_STATE  0x00000007
#define MCP_OUT_STATE_M  ((MCP_OUT_STATE_V)<<(MCP_OUT_STATE_S))
#define MCP_OUT_STATE_V  0x7
#define MCP_OUT_STATE_S  20
/* MCP_OUT_DSCR_STATE : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: */
#define MCP_OUT_DSCR_STATE  0x00000003
#define MCP_OUT_DSCR_STATE_M  ((MCP_OUT_DSCR_STATE_V)<<(MCP_OUT_DSCR_STATE_S))
#define MCP_OUT_DSCR_STATE_V  0x3
#define MCP_OUT_DSCR_STATE_S  18
/* MCP_OUTLINK_DSCR_ADDR : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: */
#define MCP_OUTLINK_DSCR_ADDR  0x0003FFFF
#define MCP_OUTLINK_DSCR_ADDR_M  ((MCP_OUTLINK_DSCR_ADDR_V)<<(MCP_OUTLINK_DSCR_ADDR_S))
#define MCP_OUTLINK_DSCR_ADDR_V  0x3FFFF
#define MCP_OUTLINK_DSCR_ADDR_S  0

#define MCP_CRC_OUT_REG          (DR_REG_MCP_BASE + 0x0048)
/* MCP_CRC_RESULT : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define MCP_CRC_RESULT  0xFFFFFFFF
#define MCP_CRC_RESULT_M  ((MCP_CRC_RESULT_V)<<(MCP_CRC_RESULT_S))
#define MCP_CRC_RESULT_V  0xFFFFFFFF
#define MCP_CRC_RESULT_S  0

#define MCP_DATE_REG          (DR_REG_MCP_BASE + 0x00fc)
/* MCP_DMA_DATE : R/W ;bitpos:[31:0] ;default: 32'h18082000 ; */
/*description: */
#define MCP_DMA_DATE  0xFFFFFFFF
#define MCP_DMA_DATE_M  ((MCP_DMA_DATE_V)<<(MCP_DMA_DATE_S))
#define MCP_DMA_DATE_V  0xFFFFFFFF
#define MCP_DMA_DATE_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_MCP_REG_H_ */


