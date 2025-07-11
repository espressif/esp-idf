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

/** PAU_REGDMA_CONF_REG register
 *  Peri backup control register
 */
#define PAU_REGDMA_CONF_REG (DR_REG_PAU_BASE + 0x0)
/** PAU_FLOW_ERR : RO; bitpos: [2:0]; default: 0;
 *  backup error type
 */
#define PAU_FLOW_ERR    0x00000007U
#define PAU_FLOW_ERR_M  (PAU_FLOW_ERR_V << PAU_FLOW_ERR_S)
#define PAU_FLOW_ERR_V  0x00000007U
#define PAU_FLOW_ERR_S  0
/** PAU_START : WT; bitpos: [3]; default: 0;
 *  backup start signal
 */
#define PAU_START    (BIT(3))
#define PAU_START_M  (PAU_START_V << PAU_START_S)
#define PAU_START_V  0x00000001U
#define PAU_START_S  3
/** PAU_TO_MEM : R/W; bitpos: [4]; default: 0;
 *  backup direction(reg to mem / mem to reg)
 */
#define PAU_TO_MEM    (BIT(4))
#define PAU_TO_MEM_M  (PAU_TO_MEM_V << PAU_TO_MEM_S)
#define PAU_TO_MEM_V  0x00000001U
#define PAU_TO_MEM_S  4
/** PAU_LINK_SEL : R/W; bitpos: [6:5]; default: 0;
 *  Link select
 */
#define PAU_LINK_SEL    0x00000003U
#define PAU_LINK_SEL_M  (PAU_LINK_SEL_V << PAU_LINK_SEL_S)
#define PAU_LINK_SEL_V  0x00000003U
#define PAU_LINK_SEL_S  5
/** PAU_START_MAC : WT; bitpos: [7]; default: 0;
 *  mac sw backup start signal
 */
#define PAU_START_MAC    (BIT(7))
#define PAU_START_MAC_M  (PAU_START_MAC_V << PAU_START_MAC_S)
#define PAU_START_MAC_V  0x00000001U
#define PAU_START_MAC_S  7
/** PAU_TO_MEM_MAC : R/W; bitpos: [8]; default: 0;
 *  mac sw backup direction(reg to mem / mem to reg)
 */
#define PAU_TO_MEM_MAC    (BIT(8))
#define PAU_TO_MEM_MAC_M  (PAU_TO_MEM_MAC_V << PAU_TO_MEM_MAC_S)
#define PAU_TO_MEM_MAC_V  0x00000001U
#define PAU_TO_MEM_MAC_S  8
/** PAU_SEL_MAC : R/W; bitpos: [9]; default: 0;
 *  mac hw/sw select
 */
#define PAU_SEL_MAC    (BIT(9))
#define PAU_SEL_MAC_M  (PAU_SEL_MAC_V << PAU_SEL_MAC_S)
#define PAU_SEL_MAC_V  0x00000001U
#define PAU_SEL_MAC_S  9

/** PAU_REGDMA_CLK_CONF_REG register
 *  Clock control register
 */
#define PAU_REGDMA_CLK_CONF_REG (DR_REG_PAU_BASE + 0x4)
/** PAU_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  clock enable
 */
#define PAU_CLK_EN    (BIT(0))
#define PAU_CLK_EN_M  (PAU_CLK_EN_V << PAU_CLK_EN_S)
#define PAU_CLK_EN_V  0x00000001U
#define PAU_CLK_EN_S  0

/** PAU_REGDMA_ETM_CTRL_REG register
 *  ETM start ctrl reg
 */
#define PAU_REGDMA_ETM_CTRL_REG (DR_REG_PAU_BASE + 0x8)
/** PAU_ETM_START_0 : WT; bitpos: [0]; default: 0;
 *  etm_start_0 reg
 */
#define PAU_ETM_START_0    (BIT(0))
#define PAU_ETM_START_0_M  (PAU_ETM_START_0_V << PAU_ETM_START_0_S)
#define PAU_ETM_START_0_V  0x00000001U
#define PAU_ETM_START_0_S  0
/** PAU_ETM_START_1 : WT; bitpos: [1]; default: 0;
 *  etm_start_1 reg
 */
#define PAU_ETM_START_1    (BIT(1))
#define PAU_ETM_START_1_M  (PAU_ETM_START_1_V << PAU_ETM_START_1_S)
#define PAU_ETM_START_1_V  0x00000001U
#define PAU_ETM_START_1_S  1
/** PAU_ETM_START_2 : WT; bitpos: [2]; default: 0;
 *  etm_start_2 reg
 */
#define PAU_ETM_START_2    (BIT(2))
#define PAU_ETM_START_2_M  (PAU_ETM_START_2_V << PAU_ETM_START_2_S)
#define PAU_ETM_START_2_V  0x00000001U
#define PAU_ETM_START_2_S  2
/** PAU_ETM_START_3 : WT; bitpos: [3]; default: 0;
 *  etm_start_3 reg
 */
#define PAU_ETM_START_3    (BIT(3))
#define PAU_ETM_START_3_M  (PAU_ETM_START_3_V << PAU_ETM_START_3_S)
#define PAU_ETM_START_3_V  0x00000001U
#define PAU_ETM_START_3_S  3

/** PAU_REGDMA_LINK_0_ADDR_REG register
 *  link_0_addr
 */
#define PAU_REGDMA_LINK_0_ADDR_REG (DR_REG_PAU_BASE + 0xc)
/** PAU_LINK_ADDR_0 : R/W; bitpos: [31:0]; default: 0;
 *  link_0_addr reg
 */
#define PAU_LINK_ADDR_0    0xFFFFFFFFU
#define PAU_LINK_ADDR_0_M  (PAU_LINK_ADDR_0_V << PAU_LINK_ADDR_0_S)
#define PAU_LINK_ADDR_0_V  0xFFFFFFFFU
#define PAU_LINK_ADDR_0_S  0

/** PAU_REGDMA_LINK_1_ADDR_REG register
 *  Link_1_addr
 */
#define PAU_REGDMA_LINK_1_ADDR_REG (DR_REG_PAU_BASE + 0x10)
/** PAU_LINK_ADDR_1 : R/W; bitpos: [31:0]; default: 0;
 *  Link_1_addr reg
 */
#define PAU_LINK_ADDR_1    0xFFFFFFFFU
#define PAU_LINK_ADDR_1_M  (PAU_LINK_ADDR_1_V << PAU_LINK_ADDR_1_S)
#define PAU_LINK_ADDR_1_V  0xFFFFFFFFU
#define PAU_LINK_ADDR_1_S  0

/** PAU_REGDMA_LINK_2_ADDR_REG register
 *  Link_2_addr
 */
#define PAU_REGDMA_LINK_2_ADDR_REG (DR_REG_PAU_BASE + 0x14)
/** PAU_LINK_ADDR_2 : R/W; bitpos: [31:0]; default: 0;
 *  Link_2_addr reg
 */
#define PAU_LINK_ADDR_2    0xFFFFFFFFU
#define PAU_LINK_ADDR_2_M  (PAU_LINK_ADDR_2_V << PAU_LINK_ADDR_2_S)
#define PAU_LINK_ADDR_2_V  0xFFFFFFFFU
#define PAU_LINK_ADDR_2_S  0

/** PAU_REGDMA_LINK_3_ADDR_REG register
 *  Link_3_addr
 */
#define PAU_REGDMA_LINK_3_ADDR_REG (DR_REG_PAU_BASE + 0x18)
/** PAU_LINK_ADDR_3 : R/W; bitpos: [31:0]; default: 0;
 *  Link_3_addr reg
 */
#define PAU_LINK_ADDR_3    0xFFFFFFFFU
#define PAU_LINK_ADDR_3_M  (PAU_LINK_ADDR_3_V << PAU_LINK_ADDR_3_S)
#define PAU_LINK_ADDR_3_V  0xFFFFFFFFU
#define PAU_LINK_ADDR_3_S  0

/** PAU_REGDMA_LINK_MAC_ADDR_REG register
 *  Link_mac_addr
 */
#define PAU_REGDMA_LINK_MAC_ADDR_REG (DR_REG_PAU_BASE + 0x1c)
/** PAU_LINK_ADDR_MAC : R/W; bitpos: [31:0]; default: 0;
 *  Link_mac_addr reg
 */
#define PAU_LINK_ADDR_MAC    0xFFFFFFFFU
#define PAU_LINK_ADDR_MAC_M  (PAU_LINK_ADDR_MAC_V << PAU_LINK_ADDR_MAC_S)
#define PAU_LINK_ADDR_MAC_V  0xFFFFFFFFU
#define PAU_LINK_ADDR_MAC_S  0

/** PAU_REGDMA_CURRENT_LINK_ADDR_REG register
 *  current link addr
 */
#define PAU_REGDMA_CURRENT_LINK_ADDR_REG (DR_REG_PAU_BASE + 0x20)
/** PAU_CURRENT_LINK_ADDR : RO; bitpos: [31:0]; default: 0;
 *  current link addr reg
 */
#define PAU_CURRENT_LINK_ADDR    0xFFFFFFFFU
#define PAU_CURRENT_LINK_ADDR_M  (PAU_CURRENT_LINK_ADDR_V << PAU_CURRENT_LINK_ADDR_S)
#define PAU_CURRENT_LINK_ADDR_V  0xFFFFFFFFU
#define PAU_CURRENT_LINK_ADDR_S  0

/** PAU_REGDMA_BACKUP_ADDR_REG register
 *  Backup addr
 */
#define PAU_REGDMA_BACKUP_ADDR_REG (DR_REG_PAU_BASE + 0x24)
/** PAU_BACKUP_ADDR : RO; bitpos: [31:0]; default: 0;
 *  backup addr reg
 */
#define PAU_BACKUP_ADDR    0xFFFFFFFFU
#define PAU_BACKUP_ADDR_M  (PAU_BACKUP_ADDR_V << PAU_BACKUP_ADDR_S)
#define PAU_BACKUP_ADDR_V  0xFFFFFFFFU
#define PAU_BACKUP_ADDR_S  0

/** PAU_REGDMA_MEM_ADDR_REG register
 *  mem addr
 */
#define PAU_REGDMA_MEM_ADDR_REG (DR_REG_PAU_BASE + 0x28)
/** PAU_MEM_ADDR : RO; bitpos: [31:0]; default: 0;
 *  mem addr reg
 */
#define PAU_MEM_ADDR    0xFFFFFFFFU
#define PAU_MEM_ADDR_M  (PAU_MEM_ADDR_V << PAU_MEM_ADDR_S)
#define PAU_MEM_ADDR_V  0xFFFFFFFFU
#define PAU_MEM_ADDR_S  0

/** PAU_REGDMA_BKP_CONF_REG register
 *  backup config
 */
#define PAU_REGDMA_BKP_CONF_REG (DR_REG_PAU_BASE + 0x2c)
/** PAU_READ_INTERVAL : R/W; bitpos: [6:0]; default: 32;
 *  Link read_interval
 */
#define PAU_READ_INTERVAL    0x0000007FU
#define PAU_READ_INTERVAL_M  (PAU_READ_INTERVAL_V << PAU_READ_INTERVAL_S)
#define PAU_READ_INTERVAL_V  0x0000007FU
#define PAU_READ_INTERVAL_S  0
/** PAU_LINK_TOUT_THRES : R/W; bitpos: [16:7]; default: 50;
 *  link wait timeout threshold
 */
#define PAU_LINK_TOUT_THRES    0x000003FFU
#define PAU_LINK_TOUT_THRES_M  (PAU_LINK_TOUT_THRES_V << PAU_LINK_TOUT_THRES_S)
#define PAU_LINK_TOUT_THRES_V  0x000003FFU
#define PAU_LINK_TOUT_THRES_S  7
/** PAU_BURST_LIMIT : R/W; bitpos: [21:17]; default: 8;
 *  burst limit
 */
#define PAU_BURST_LIMIT    0x0000001FU
#define PAU_BURST_LIMIT_M  (PAU_BURST_LIMIT_V << PAU_BURST_LIMIT_S)
#define PAU_BURST_LIMIT_V  0x0000001FU
#define PAU_BURST_LIMIT_S  17
/** PAU_BACKUP_TOUT_THRES : R/W; bitpos: [31:22]; default: 500;
 *  Backup timeout threshold
 */
#define PAU_BACKUP_TOUT_THRES    0x000003FFU
#define PAU_BACKUP_TOUT_THRES_M  (PAU_BACKUP_TOUT_THRES_V << PAU_BACKUP_TOUT_THRES_S)
#define PAU_BACKUP_TOUT_THRES_V  0x000003FFU
#define PAU_BACKUP_TOUT_THRES_S  22

/** PAU_INT_ENA_REG register
 *  Read only register for error and done
 */
#define PAU_INT_ENA_REG (DR_REG_PAU_BASE + 0x30)
/** PAU_DONE_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  backup done flag
 */
#define PAU_DONE_INT_ENA    (BIT(0))
#define PAU_DONE_INT_ENA_M  (PAU_DONE_INT_ENA_V << PAU_DONE_INT_ENA_S)
#define PAU_DONE_INT_ENA_V  0x00000001U
#define PAU_DONE_INT_ENA_S  0
/** PAU_ERROR_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  error flag
 */
#define PAU_ERROR_INT_ENA    (BIT(1))
#define PAU_ERROR_INT_ENA_M  (PAU_ERROR_INT_ENA_V << PAU_ERROR_INT_ENA_S)
#define PAU_ERROR_INT_ENA_V  0x00000001U
#define PAU_ERROR_INT_ENA_S  1

/** PAU_INT_RAW_REG register
 *  Read only register for error and done
 */
#define PAU_INT_RAW_REG (DR_REG_PAU_BASE + 0x34)
/** PAU_DONE_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  backup done flag
 */
#define PAU_DONE_INT_RAW    (BIT(0))
#define PAU_DONE_INT_RAW_M  (PAU_DONE_INT_RAW_V << PAU_DONE_INT_RAW_S)
#define PAU_DONE_INT_RAW_V  0x00000001U
#define PAU_DONE_INT_RAW_S  0
/** PAU_ERROR_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  error flag
 */
#define PAU_ERROR_INT_RAW    (BIT(1))
#define PAU_ERROR_INT_RAW_M  (PAU_ERROR_INT_RAW_V << PAU_ERROR_INT_RAW_S)
#define PAU_ERROR_INT_RAW_V  0x00000001U
#define PAU_ERROR_INT_RAW_S  1

/** PAU_INT_CLR_REG register
 *  Read only register for error and done
 */
#define PAU_INT_CLR_REG (DR_REG_PAU_BASE + 0x38)
/** PAU_DONE_INT_CLR : WT; bitpos: [0]; default: 0;
 *  backup done flag
 */
#define PAU_DONE_INT_CLR    (BIT(0))
#define PAU_DONE_INT_CLR_M  (PAU_DONE_INT_CLR_V << PAU_DONE_INT_CLR_S)
#define PAU_DONE_INT_CLR_V  0x00000001U
#define PAU_DONE_INT_CLR_S  0
/** PAU_ERROR_INT_CLR : WT; bitpos: [1]; default: 0;
 *  error flag
 */
#define PAU_ERROR_INT_CLR    (BIT(1))
#define PAU_ERROR_INT_CLR_M  (PAU_ERROR_INT_CLR_V << PAU_ERROR_INT_CLR_S)
#define PAU_ERROR_INT_CLR_V  0x00000001U
#define PAU_ERROR_INT_CLR_S  1

/** PAU_INT_ST_REG register
 *  Read only register for error and done
 */
#define PAU_INT_ST_REG (DR_REG_PAU_BASE + 0x3c)
/** PAU_DONE_INT_ST : RO; bitpos: [0]; default: 0;
 *  backup done flag
 */
#define PAU_DONE_INT_ST    (BIT(0))
#define PAU_DONE_INT_ST_M  (PAU_DONE_INT_ST_V << PAU_DONE_INT_ST_S)
#define PAU_DONE_INT_ST_V  0x00000001U
#define PAU_DONE_INT_ST_S  0
/** PAU_ERROR_INT_ST : RO; bitpos: [1]; default: 0;
 *  error flag
 */
#define PAU_ERROR_INT_ST    (BIT(1))
#define PAU_ERROR_INT_ST_M  (PAU_ERROR_INT_ST_V << PAU_ERROR_INT_ST_S)
#define PAU_ERROR_INT_ST_V  0x00000001U
#define PAU_ERROR_INT_ST_S  1

/** PAU_DATE_REG register
 *  Date register.
 */
#define PAU_DATE_REG (DR_REG_PAU_BASE + 0x3fc)
/** PAU_DATE : R/W; bitpos: [27:0]; default: 35663984;
 *  REGDMA date information/ REGDMA version information.
 */
#define PAU_DATE    0x0FFFFFFFU
#define PAU_DATE_M  (PAU_DATE_V << PAU_DATE_S)
#define PAU_DATE_V  0x0FFFFFFFU
#define PAU_DATE_S  0

#ifdef __cplusplus
}
#endif
