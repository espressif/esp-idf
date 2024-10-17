/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_CRYPTO_DMA_REG_H_
#define _SOC_CRYPTO_DMA_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc/soc.h"

/* CRYPTO_DMA_CONF0 : RO ;bitpos:[31:14] ;default: 18'h0 ; */
/* CONF0_REG_GEN_CLK_EN : RW ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define CONF0_REG_GEN_CLK_EN   (BIT(13))
#define CONF0_REG_GEN_CLK_EN_M (BIT(13))
#define CONF0_REG_GEN_CLK_EN_V 0x1
#define CONF0_REG_GEN_CLK_EN_S 13
/* CONF0_REG_MEM_TRANS_EN: RW ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define CONF0_REG_MEM_TRANS_EN   (BIT(12))
#define CONF0_REG_MEM_TRANS_EN_M (BIT(12))
#define CONF0_REG_MEM_TRANS_EN_V 0x1
#define CONF0_REG_MEM_TRANS_EN_S 12
/* CONF0_REG_OUT_DATA_BURST_EN: RW ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define CONF0_REG_OUT_DATA_BURST_EN   (BIT(11))
#define CONF0_REG_OUT_DATA_BURST_EN_M (BIT(11))
#define CONF0_REG_OUT_DATA_BURST_EN_V 0x1
#define CONF0_REG_OUT_DATA_BURST_EN_S 11
/* CONF0_REG_INDSCR_BURST_EN: RW ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define CONF0_REG_INDSCR_BURST_EN   (BIT(10))
#define CONF0_REG_INDSCR_BURST_EN_M  (BIT(10))
#define CONF0_REG_INDSCR_BURST_EN_V  0x1
#define CONF0_REG_INDSCR_BURST_EN_S  10
/* CONF0_REG_OUTDSCR_BURST_EN: RW ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define CONF0_REG_OUTDSCR_BURST_EN   (BIT(9))
#define CONF0_REG_OUTDSCR_BURST_EN_M  (BIT(9))
#define CONF0_REG_OUTDSCR_BURST_EN_V  0x1
#define CONF0_REG_OUTDSCR_BURST_EN_S  9
/* CONF0_REG_OUT_EOF_MODE: RW ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define CONF0_REG_OUT_EOF_MODE   (BIT(8))
#define CONF0_REG_OUT_EOF_MODE_M  (BIT(8))
#define CONF0_REG_OUT_EOF_MODE_V  0x1
#define CONF0_REG_OUT_EOF_MODE_S  8
/* CONF0_REG_OUT_NO_RESTART_CLR: RW ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define CONF0_REG_OUT_NO_RESTART_CLR   (BIT(7))
#define CONF0_REG_OUT_NO_RESTART_CLR_M  (BIT(7))
#define CONF0_REG_OUT_NO_RESTART_CLR_V  0x1
#define CONF0_REG_OUT_NO_RESTART_CLR_S  7
/* CONF0_REG_OUT_AUTO_WRBACK: RW ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define CONF0_REG_OUT_AUTO_WRBACK   (BIT(6))
#define CONF0_REG_OUT_AUTO_WRBACK_M  (BIT(6))
#define CONF0_REG_OUT_AUTO_WRBACK_V  0x1
#define CONF0_REG_OUT_AUTO_WRBACK_S  6
/* CONF0_REG_OUT_LOOP_TEST: RW ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define CONF0_REG_OUT_LOOP_TEST   (BIT(5))
#define CONF0_REG_OUT_LOOP_TEST_M  (BIT(5))
#define CONF0_REG_OUT_LOOP_TEST_V  0x1
#define CONF0_REG_OUT_LOOP_TEST_S  5
/* CONF0_REG_IN_LOOP_TEST: RW ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define CONF0_REG_IN_LOOP_TEST   (BIT(4))
#define CONF0_REG_IN_LOOP_TEST_M  (BIT(4))
#define CONF0_REG_IN_LOOP_TEST_V  0x1
#define CONF0_REG_IN_LOOP_TEST_S  4
/* CONF0_REG_AHBM_RST: RW ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define CONF0_REG_AHBM_RST   (BIT(3))
#define CONF0_REG_AHBM_RST_M  (BIT(3))
#define CONF0_REG_AHBM_RST_V  0x1
#define CONF0_REG_AHBM_RST_S  3
/* CONF0_REG_AHBM_FIFO_RST: RW ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define CONF0_REG_AHBM_FIFO_RST   (BIT(2))
#define CONF0_REG_AHBM_FIFO_RST_M  (BIT(2))
#define CONF0_REG_AHBM_FIFO_RST_V  0x1
#define CONF0_REG_AHBM_FIFO_RST_S  2
/* CONF0_REG_OUT_RST: RW ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define CONF0_REG_OUT_RST   (BIT(1))
#define CONF0_REG_OUT_RST_M  (BIT(1))
#define CONF0_REG_OUT_RST_V  0x1
#define CONF0_REG_OUT_RST_S  1
/* CONF0_REG_IN_RST: RW ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define CONF0_REG_IN_RST   (BIT(0))
#define CONF0_REG_IN_RST_M  (BIT(0))
#define CONF0_REG_IN_RST_V  0x1
#define CONF0_REG_IN_RST_S  0
/* CRYPTO_DMA_INT_RAW : RO ;bitpos:[31:10] ;default: 22'h0 ; */
/* INT_RAW_OUT_TOTAL_EOF : RW ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define INT_RAW_OUT_TOTAL_EOF  ( BIT(8))
#define INT_RAW_OUT_TOTAL_EOF_M  (BIT(8))
#define INT_RAW_OUT_TOTAL_EOF_V  0x1
#define INT_RAW_OUT_TOTAL_EOF_S  8
/* INT_RAW_IN_SUC_EOF : RW ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define INT_RAW_IN_SUC_EOF  ( BIT(1))
#define INT_RAW_IN_SUC_EOF_M  (BIT(1))
#define INT_RAW_IN_SUC_EOF_V  0x1
#define INT_RAW_IN_SUC_EOF_S  1
/* CRYPTO_DMA_OUT_LINK : RO ;bitpos:[31] ;default: 1'h0 ; */
/* OUT_LINK_REG_OUTLINK_RESTART : RW ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define OUT_LINK_REG_OUTLINK_RESTART  ( BIT(30))
#define OUT_LINK_REG_OUTLINK_RESTART_M  (BIT(30))
#define OUT_LINK_REG_OUTLINK_RESTART_V  0x1
#define OUT_LINK_REG_OUTLINK_RESTART_S  30
/* OUT_LINK_REG_OUTLINK_START : RW ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define OUT_LINK_REG_OUTLINK_START  ( BIT(29))
#define OUT_LINK_REG_OUTLINK_START_M  (BIT(29))
#define OUT_LINK_REG_OUTLINK_START_V  0x1
#define OUT_LINK_REG_OUTLINK_START_S  29
/* OUT_LINK_REG_OUTLINK_STOP : RW ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define OUT_LINK_REG_OUTLINK_STOP  ( BIT(28))
#define OUT_LINK_REG_OUTLINK_STOP_M  (BIT(28))
#define OUT_LINK_REG_OUTLINK_STOP_V  0x1
#define OUT_LINK_REG_OUTLINK_STOP_S  28
/* OUT_LINK_REG_OUTLINK_ADDR : RW ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: */
#define OUT_LINK_REG_OUTLINK_ADDR  0x000FFFFF
#define OUT_LINK_REG_OUTLINK_ADDR_M  (OUT_LINK_REG_OUTLINK_ADDR_V<<OUT_LINK_REG_OUTLINK_ADDR_S)
#define OUT_LINK_REG_OUTLINK_ADDR_V  0xFFFFF
#define OUT_LINK_REG_OUTLINK_ADDR_S  0
/* CRYPTO_DMA_IN_LINK : RO ;bitpos:[31] ;default: 1'h0 ; */
/* IN_LINK_REG_INLINK_RESTART : RW ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define IN_LINK_REG_INLINK_RESTART  ( BIT(30))
#define IN_LINK_REG_INLINK_RESTART_M  (BIT(30))
#define IN_LINK_REG_INLINK_RESTART_V  0x1
#define IN_LINK_REG_INLINK_RESTART_S  30
/* IN_LINK_REG_INLINK_START : RW ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define IN_LINK_REG_INLINK_START  ( BIT(29))
#define IN_LINK_REG_INLINK_START_M  (BIT(29))
#define IN_LINK_REG_INLINK_START_V  0x1
#define IN_LINK_REG_INLINK_START_S  29
/* IN_LINK_REG_INLINK_STOP : RW ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define IN_LINK_REG_INLINK_STOP  ( BIT(28))
#define IN_LINK_REG_INLINK_STOP_M  (BIT(28))
#define IN_LINK_REG_INLINK_STOP_V  0x1
#define IN_LINK_REG_INLINK_STOP_S  28
/* IN_LINK_REG_INLINK_ADDR : RW ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: */
#define IN_LINK_REG_INLINK_ADDR  0x000FFFFF
#define IN_LINK_REG_INLINK_ADDR_M  (IN_LINK_REG_INLINK_ADDR_V<<IN_LINK_REG_INLINK_ADDR_S)
#define IN_LINK_REG_INLINK_ADDR_V  0xFFFFF
#define IN_LINK_REG_INLINK_ADDR_S  0
/* CRYPTO_DMA_AES_SHA_SELECT : RO ;bitpos:[31:1] ;default: 31'b0 ; */
/* AES_SHA_SELECT : RW ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define AES_SHA_SELECT  ( BIT(0))
#define AES_SHA_SELECT_M  (BIT(0))
#define AES_SHA_SELECT_V  0x1
#define AES_SHA_SELECT_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_CRYPTO_DMA_REG_H_ */
