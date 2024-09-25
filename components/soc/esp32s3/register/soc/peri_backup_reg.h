/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_PERI_BACKUP_REG_H_
#define _SOC_PERI_BACKUP_REG_H_


#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

#define PERI_BACKUP_CONFIG_REG          (DR_REG_PERI_BACKUP_BASE + 0x0)
/* PERI_BACKUP_ENA : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: .*/
#define PERI_BACKUP_ENA    (BIT(31))
#define PERI_BACKUP_ENA_M  (BIT(31))
#define PERI_BACKUP_ENA_V  0x1
#define PERI_BACKUP_ENA_S  31
/* PERI_BACKUP_TO_MEM : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: .*/
#define PERI_BACKUP_TO_MEM    (BIT(30))
#define PERI_BACKUP_TO_MEM_M  (BIT(30))
#define PERI_BACKUP_TO_MEM_V  0x1
#define PERI_BACKUP_TO_MEM_S  30
/* PERI_BACKUP_START : WT ;bitpos:[29] ;default: 1'b0 ; */
/*description: .*/
#define PERI_BACKUP_START    (BIT(29))
#define PERI_BACKUP_START_M  (BIT(29))
#define PERI_BACKUP_START_V  0x1
#define PERI_BACKUP_START_S  29
/* PERI_BACKUP_SIZE : R/W ;bitpos:[28:19] ;default: 10'd0 ; */
/*description: .*/
#define PERI_BACKUP_SIZE    0x000003FF
#define PERI_BACKUP_SIZE_M  ((PERI_BACKUP_SIZE_V)<<(PERI_BACKUP_SIZE_S))
#define PERI_BACKUP_SIZE_V  0x3FF
#define PERI_BACKUP_SIZE_S  19
/* PERI_BACKUP_TOUT_THRES : R/W ;bitpos:[18:9] ;default: 10'd50 ; */
/*description: .*/
#define PERI_BACKUP_TOUT_THRES    0x000003FF
#define PERI_BACKUP_TOUT_THRES_M  ((PERI_BACKUP_TOUT_THRES_V)<<(PERI_BACKUP_TOUT_THRES_S))
#define PERI_BACKUP_TOUT_THRES_V  0x3FF
#define PERI_BACKUP_TOUT_THRES_S  9
/* PERI_BACKUP_BURST_LIMIT : R/W ;bitpos:[8:4] ;default: 5'd8 ; */
/*description: .*/
#define PERI_BACKUP_BURST_LIMIT    0x0000001F
#define PERI_BACKUP_BURST_LIMIT_M  ((PERI_BACKUP_BURST_LIMIT_V)<<(PERI_BACKUP_BURST_LIMIT_S))
#define PERI_BACKUP_BURST_LIMIT_V  0x1F
#define PERI_BACKUP_BURST_LIMIT_S  4
/* PERI_BACKUP_ADDR_MAP_MODE : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: .*/
#define PERI_BACKUP_ADDR_MAP_MODE    (BIT(3))
#define PERI_BACKUP_ADDR_MAP_MODE_M  (BIT(3))
#define PERI_BACKUP_ADDR_MAP_MODE_V  0x1
#define PERI_BACKUP_ADDR_MAP_MODE_S  3
/* PERI_BACKUP_FLOW_ERR : RO ;bitpos:[2:0] ;default: 3'd0 ; */
/*description: .*/
#define PERI_BACKUP_FLOW_ERR    0x00000007
#define PERI_BACKUP_FLOW_ERR_M  ((PERI_BACKUP_FLOW_ERR_V)<<(PERI_BACKUP_FLOW_ERR_S))
#define PERI_BACKUP_FLOW_ERR_V  0x7
#define PERI_BACKUP_FLOW_ERR_S  0

#define PERI_BACKUP_APB_ADDR_REG          (DR_REG_PERI_BACKUP_BASE + 0x4)
/* PERI_BACKUP_APB_START_ADDR : R/W ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: .*/
#define PERI_BACKUP_APB_START_ADDR    0xFFFFFFFF
#define PERI_BACKUP_APB_START_ADDR_M  ((PERI_BACKUP_APB_START_ADDR_V)<<(PERI_BACKUP_APB_START_ADDR_S))
#define PERI_BACKUP_APB_START_ADDR_V  0xFFFFFFFF
#define PERI_BACKUP_APB_START_ADDR_S  0

#define PERI_BACKUP_MEM_ADDR_REG          (DR_REG_PERI_BACKUP_BASE + 0x8)
/* PERI_BACKUP_MEM_START_ADDR : R/W ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: .*/
#define PERI_BACKUP_MEM_START_ADDR    0xFFFFFFFF
#define PERI_BACKUP_MEM_START_ADDR_M  ((PERI_BACKUP_MEM_START_ADDR_V)<<(PERI_BACKUP_MEM_START_ADDR_S))
#define PERI_BACKUP_MEM_START_ADDR_V  0xFFFFFFFF
#define PERI_BACKUP_MEM_START_ADDR_S  0

#define PERI_BACKUP_REG_MAP0_REG          (DR_REG_PERI_BACKUP_BASE + 0xC)
/* PERI_BACKUP_MAP0 : R/W ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: .*/
#define PERI_BACKUP_MAP0    0xFFFFFFFF
#define PERI_BACKUP_MAP0_M  ((PERI_BACKUP_MAP0_V)<<(PERI_BACKUP_MAP0_S))
#define PERI_BACKUP_MAP0_V  0xFFFFFFFF
#define PERI_BACKUP_MAP0_S  0

#define PERI_BACKUP_REG_MAP1_REG          (DR_REG_PERI_BACKUP_BASE + 0x10)
/* PERI_BACKUP_MAP1 : R/W ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: .*/
#define PERI_BACKUP_MAP1    0xFFFFFFFF
#define PERI_BACKUP_MAP1_M  ((PERI_BACKUP_MAP1_V)<<(PERI_BACKUP_MAP1_S))
#define PERI_BACKUP_MAP1_V  0xFFFFFFFF
#define PERI_BACKUP_MAP1_S  0

#define PERI_BACKUP_REG_MAP2_REG          (DR_REG_PERI_BACKUP_BASE + 0x14)
/* PERI_BACKUP_MAP2 : R/W ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: .*/
#define PERI_BACKUP_MAP2    0xFFFFFFFF
#define PERI_BACKUP_MAP2_M  ((PERI_BACKUP_MAP2_V)<<(PERI_BACKUP_MAP2_S))
#define PERI_BACKUP_MAP2_V  0xFFFFFFFF
#define PERI_BACKUP_MAP2_S  0

#define PERI_BACKUP_REG_MAP3_REG          (DR_REG_PERI_BACKUP_BASE + 0x18)
/* PERI_BACKUP_MAP3 : R/W ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: .*/
#define PERI_BACKUP_MAP3    0xFFFFFFFF
#define PERI_BACKUP_MAP3_M  ((PERI_BACKUP_MAP3_V)<<(PERI_BACKUP_MAP3_S))
#define PERI_BACKUP_MAP3_V  0xFFFFFFFF
#define PERI_BACKUP_MAP3_S  0

#define PERI_BACKUP_INT_RAW_REG          (DR_REG_PERI_BACKUP_BASE + 0x1C)
/* PERI_BACKUP_ERR_INT_RAW : R/SS/WTC ;bitpos:[1] ;default: 1'd0 ; */
/*description: .*/
#define PERI_BACKUP_ERR_INT_RAW    (BIT(1))
#define PERI_BACKUP_ERR_INT_RAW_M  (BIT(1))
#define PERI_BACKUP_ERR_INT_RAW_V  0x1
#define PERI_BACKUP_ERR_INT_RAW_S  1
/* PERI_BACKUP_DONE_INT_RAW : R/SS/WTC ;bitpos:[0] ;default: 1'd0 ; */
/*description: .*/
#define PERI_BACKUP_DONE_INT_RAW    (BIT(0))
#define PERI_BACKUP_DONE_INT_RAW_M  (BIT(0))
#define PERI_BACKUP_DONE_INT_RAW_V  0x1
#define PERI_BACKUP_DONE_INT_RAW_S  0

#define PERI_BACKUP_INT_ST_REG          (DR_REG_PERI_BACKUP_BASE + 0x20)
/* PERI_BACKUP_ERR_INT_ST : RO ;bitpos:[1] ;default: 1'd0 ; */
/*description: .*/
#define PERI_BACKUP_ERR_INT_ST    (BIT(1))
#define PERI_BACKUP_ERR_INT_ST_M  (BIT(1))
#define PERI_BACKUP_ERR_INT_ST_V  0x1
#define PERI_BACKUP_ERR_INT_ST_S  1
/* PERI_BACKUP_DONE_INT_ST : RO ;bitpos:[0] ;default: 1'd0 ; */
/*description: .*/
#define PERI_BACKUP_DONE_INT_ST    (BIT(0))
#define PERI_BACKUP_DONE_INT_ST_M  (BIT(0))
#define PERI_BACKUP_DONE_INT_ST_V  0x1
#define PERI_BACKUP_DONE_INT_ST_S  0

#define PERI_BACKUP_INT_ENA_REG          (DR_REG_PERI_BACKUP_BASE + 0x24)
/* PERI_BACKUP_ERR_INT_ENA : R/W ;bitpos:[1] ;default: 1'd0 ; */
/*description: .*/
#define PERI_BACKUP_ERR_INT_ENA    (BIT(1))
#define PERI_BACKUP_ERR_INT_ENA_M  (BIT(1))
#define PERI_BACKUP_ERR_INT_ENA_V  0x1
#define PERI_BACKUP_ERR_INT_ENA_S  1
/* PERI_BACKUP_DONE_INT_ENA : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: .*/
#define PERI_BACKUP_DONE_INT_ENA    (BIT(0))
#define PERI_BACKUP_DONE_INT_ENA_M  (BIT(0))
#define PERI_BACKUP_DONE_INT_ENA_V  0x1
#define PERI_BACKUP_DONE_INT_ENA_S  0

#define PERI_BACKUP_INT_CLR_REG          (DR_REG_PERI_BACKUP_BASE + 0x28)
/* PERI_BACKUP_ERR_INT_CLR : WT ;bitpos:[1] ;default: 1'd0 ; */
/*description: .*/
#define PERI_BACKUP_ERR_INT_CLR    (BIT(1))
#define PERI_BACKUP_ERR_INT_CLR_M  (BIT(1))
#define PERI_BACKUP_ERR_INT_CLR_V  0x1
#define PERI_BACKUP_ERR_INT_CLR_S  1
/* PERI_BACKUP_DONE_INT_CLR : WT ;bitpos:[0] ;default: 1'd0 ; */
/*description: .*/
#define PERI_BACKUP_DONE_INT_CLR    (BIT(0))
#define PERI_BACKUP_DONE_INT_CLR_M  (BIT(0))
#define PERI_BACKUP_DONE_INT_CLR_V  0x1
#define PERI_BACKUP_DONE_INT_CLR_S  0

#define PERI_BACKUP_DATE_REG          (DR_REG_PERI_BACKUP_BASE + 0xFC)
/* PERI_BACKUP_CLK_EN : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: register file clk gating.*/
#define PERI_BACKUP_CLK_EN    (BIT(31))
#define PERI_BACKUP_CLK_EN_M  (BIT(31))
#define PERI_BACKUP_CLK_EN_V  0x1
#define PERI_BACKUP_CLK_EN_S  31
/* PERI_BACKUP_DATE : R/W ;bitpos:[27:0] ;default: 28'h2012300 ; */
/*description: .*/
#define PERI_BACKUP_DATE    0x0FFFFFFF
#define PERI_BACKUP_DATE_M  ((PERI_BACKUP_DATE_V)<<(PERI_BACKUP_DATE_S))
#define PERI_BACKUP_DATE_V  0xFFFFFFF
#define PERI_BACKUP_DATE_S  0


#ifdef __cplusplus
}
#endif



#endif /*_SOC_PERI_BACKUP_REG_H_ */
