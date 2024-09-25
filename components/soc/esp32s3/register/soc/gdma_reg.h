/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_GDMA_REG_H_
#define _SOC_GDMA_REG_H_


#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

#define GDMA_IN_CONF0_CH0_REG          (DR_REG_GDMA_BASE + 0x0)
/* GDMA_MEM_TRANS_EN_CH0 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit 1 to enable automatic transmitting data from memory to memory via DMA..*/
#define GDMA_MEM_TRANS_EN_CH0    (BIT(4))
#define GDMA_MEM_TRANS_EN_CH0_M  (BIT(4))
#define GDMA_MEM_TRANS_EN_CH0_V  0x1
#define GDMA_MEM_TRANS_EN_CH0_S  4
/* GDMA_IN_DATA_BURST_EN_CH0 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Rx channel 0 receiving data
when accessing internal SRAM. .*/
#define GDMA_IN_DATA_BURST_EN_CH0    (BIT(3))
#define GDMA_IN_DATA_BURST_EN_CH0_M  (BIT(3))
#define GDMA_IN_DATA_BURST_EN_CH0_V  0x1
#define GDMA_IN_DATA_BURST_EN_CH0_S  3
/* GDMA_INDSCR_BURST_EN_CH0 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Rx channel 0 reading link
descriptor when accessing internal SRAM. .*/
#define GDMA_INDSCR_BURST_EN_CH0    (BIT(2))
#define GDMA_INDSCR_BURST_EN_CH0_M  (BIT(2))
#define GDMA_INDSCR_BURST_EN_CH0_V  0x1
#define GDMA_INDSCR_BURST_EN_CH0_S  2
/* GDMA_IN_LOOP_TEST_CH0 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: reserved.*/
#define GDMA_IN_LOOP_TEST_CH0    (BIT(1))
#define GDMA_IN_LOOP_TEST_CH0_M  (BIT(1))
#define GDMA_IN_LOOP_TEST_CH0_V  0x1
#define GDMA_IN_LOOP_TEST_CH0_S  1
/* GDMA_IN_RST_CH0 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: This bit is used to reset DMA channel 0 Rx FSM and Rx FIFO pointer..*/
#define GDMA_IN_RST_CH0    (BIT(0))
#define GDMA_IN_RST_CH0_M  (BIT(0))
#define GDMA_IN_RST_CH0_V  0x1
#define GDMA_IN_RST_CH0_S  0

#define GDMA_IN_CONF1_CH0_REG          (DR_REG_GDMA_BASE + 0x4)
/* GDMA_IN_EXT_MEM_BK_SIZE_CH0 : R/W ;bitpos:[14:13] ;default: 2'b0 ; */
/*description: Block size of Rx channel 0 when DMA access external SRAM. 0: 16 bytes      1: 32
 bytes    2/3:reserved.*/
#define GDMA_IN_EXT_MEM_BK_SIZE_CH0    0x00000003
#define GDMA_IN_EXT_MEM_BK_SIZE_CH0_M  ((GDMA_IN_EXT_MEM_BK_SIZE_CH0_V)<<(GDMA_IN_EXT_MEM_BK_SIZE_CH0_S))
#define GDMA_IN_EXT_MEM_BK_SIZE_CH0_V  0x3
#define GDMA_IN_EXT_MEM_BK_SIZE_CH0_S  13
/* GDMA_IN_CHECK_OWNER_CH0 : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to enable checking the owner attribute of the link descriptor..*/
#define GDMA_IN_CHECK_OWNER_CH0    (BIT(12))
#define GDMA_IN_CHECK_OWNER_CH0_M  (BIT(12))
#define GDMA_IN_CHECK_OWNER_CH0_V  0x1
#define GDMA_IN_CHECK_OWNER_CH0_S  12
/* GDMA_DMA_INFIFO_FULL_THRS_CH0 : R/W ;bitpos:[11:0] ;default: 12'hc ; */
/*description: This register is used to generate the INFIFO_FULL_WM_INT interrupt when Rx
channel 0 received byte number in Rx FIFO is up to the value of the register..*/
#define GDMA_DMA_INFIFO_FULL_THRS_CH0    0x00000FFF
#define GDMA_DMA_INFIFO_FULL_THRS_CH0_M  ((GDMA_DMA_INFIFO_FULL_THRS_CH0_V)<<(GDMA_DMA_INFIFO_FULL_THRS_CH0_S))
#define GDMA_DMA_INFIFO_FULL_THRS_CH0_V  0xFFF
#define GDMA_DMA_INFIFO_FULL_THRS_CH0_S  0

#define GDMA_IN_INT_RAW_CH0_REG          (DR_REG_GDMA_BASE + 0x8)
/* GDMA_INFIFO_UDF_L3_CH0_INT_RAW : R/WTC/SS ;bitpos:[9] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo of Rx channel 0 is
underflow. .*/
#define GDMA_INFIFO_UDF_L3_CH0_INT_RAW    (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH0_INT_RAW_M  (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH0_INT_RAW_V  0x1
#define GDMA_INFIFO_UDF_L3_CH0_INT_RAW_S  9
/* GDMA_INFIFO_OVF_L3_CH0_INT_RAW : R/WTC/SS ;bitpos:[8] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo of Rx channel 0 is
overflow. .*/
#define GDMA_INFIFO_OVF_L3_CH0_INT_RAW    (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH0_INT_RAW_M  (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH0_INT_RAW_V  0x1
#define GDMA_INFIFO_OVF_L3_CH0_INT_RAW_S  8
/* GDMA_INFIFO_UDF_L1_CH0_INT_RAW : R/WTC/SS ;bitpos:[7] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
underflow. .*/
#define GDMA_INFIFO_UDF_L1_CH0_INT_RAW    (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH0_INT_RAW_M  (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH0_INT_RAW_V  0x1
#define GDMA_INFIFO_UDF_L1_CH0_INT_RAW_S  7
/* GDMA_INFIFO_OVF_L1_CH0_INT_RAW : R/WTC/SS ;bitpos:[6] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
overflow. .*/
#define GDMA_INFIFO_OVF_L1_CH0_INT_RAW    (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH0_INT_RAW_M  (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH0_INT_RAW_V  0x1
#define GDMA_INFIFO_OVF_L1_CH0_INT_RAW_S  6
/* GDMA_INFIFO_FULL_WM_CH0_INT_RAW : R/WTC/SS ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when received data byte number is up
to threshold configured by REG_DMA_INFIFO_FULL_THRS_CH0 in Rx FIFO of channel 0..*/
#define GDMA_INFIFO_FULL_WM_CH0_INT_RAW    (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH0_INT_RAW_M  (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH0_INT_RAW_V  0x1
#define GDMA_INFIFO_FULL_WM_CH0_INT_RAW_S  5
/* GDMA_IN_DSCR_EMPTY_CH0_INT_RAW : R/WTC/SS ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when Rx buffer pointed by inlink is
full and receiving data is not completed, but there is no more inlink for Rx chann
el 0..*/
#define GDMA_IN_DSCR_EMPTY_CH0_INT_RAW    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH0_INT_RAW_M  (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH0_INT_RAW_V  0x1
#define GDMA_IN_DSCR_EMPTY_CH0_INT_RAW_S  4
/* GDMA_IN_DSCR_ERR_CH0_INT_RAW : R/WTC/SS ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when detecting inlink descriptor error
, including owner error, the second and third word error of inlink descriptor
for Rx channel 0..*/
#define GDMA_IN_DSCR_ERR_CH0_INT_RAW    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH0_INT_RAW_M  (BIT(3))
#define GDMA_IN_DSCR_ERR_CH0_INT_RAW_V  0x1
#define GDMA_IN_DSCR_ERR_CH0_INT_RAW_S  3
/* GDMA_IN_ERR_EOF_CH0_INT_RAW : R/WTC/SS ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when data error is detected only in
the case that the peripheral is UHCI0 for Rx channel 0. For other peripherals,
this raw interrupt is reserved..*/
#define GDMA_IN_ERR_EOF_CH0_INT_RAW    (BIT(2))
#define GDMA_IN_ERR_EOF_CH0_INT_RAW_M  (BIT(2))
#define GDMA_IN_ERR_EOF_CH0_INT_RAW_V  0x1
#define GDMA_IN_ERR_EOF_CH0_INT_RAW_S  2
/* GDMA_IN_SUC_EOF_CH0_INT_RAW : R/WTC/SS ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data pointed by
oneinlink descriptor has been received for Rx channel 0. For UHCI0, the raw interrupt b
it turns to high level when the last data pointed by one inlink descriptor has b
een received and no data error is detected for Rx channel 0..*/
#define GDMA_IN_SUC_EOF_CH0_INT_RAW    (BIT(1))
#define GDMA_IN_SUC_EOF_CH0_INT_RAW_M  (BIT(1))
#define GDMA_IN_SUC_EOF_CH0_INT_RAW_V  0x1
#define GDMA_IN_SUC_EOF_CH0_INT_RAW_S  1
/* GDMA_IN_DONE_CH0_INT_RAW : R/WTC/SS ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data pointed by one
inlink descriptor has been received for Rx channel 0..*/
#define GDMA_IN_DONE_CH0_INT_RAW    (BIT(0))
#define GDMA_IN_DONE_CH0_INT_RAW_M  (BIT(0))
#define GDMA_IN_DONE_CH0_INT_RAW_V  0x1
#define GDMA_IN_DONE_CH0_INT_RAW_S  0

#define GDMA_IN_INT_ST_CH0_REG          (DR_REG_GDMA_BASE + 0xC)
/* GDMA_INFIFO_UDF_L3_CH0_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L3_CH0_INT_ST    (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH0_INT_ST_M  (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH0_INT_ST_V  0x1
#define GDMA_INFIFO_UDF_L3_CH0_INT_ST_S  9
/* GDMA_INFIFO_OVF_L3_CH0_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L3_CH0_INT_ST    (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH0_INT_ST_M  (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH0_INT_ST_V  0x1
#define GDMA_INFIFO_OVF_L3_CH0_INT_ST_S  8
/* GDMA_INFIFO_UDF_L1_CH0_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L1_CH0_INT_ST    (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH0_INT_ST_M  (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH0_INT_ST_V  0x1
#define GDMA_INFIFO_UDF_L1_CH0_INT_ST_S  7
/* GDMA_INFIFO_OVF_L1_CH0_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L1_CH0_INT_ST    (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH0_INT_ST_M  (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH0_INT_ST_V  0x1
#define GDMA_INFIFO_OVF_L1_CH0_INT_ST_S  6
/* GDMA_INFIFO_FULL_WM_CH0_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_FULL_WM_CH_INT interrupt..*/
#define GDMA_INFIFO_FULL_WM_CH0_INT_ST    (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH0_INT_ST_M  (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH0_INT_ST_V  0x1
#define GDMA_INFIFO_FULL_WM_CH0_INT_ST_S  5
/* GDMA_IN_DSCR_EMPTY_CH0_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt..*/
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ST    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ST_M  (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ST_V  0x1
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ST_S  4
/* GDMA_IN_DSCR_ERR_CH0_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_IN_DSCR_ERR_CH0_INT_ST    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH0_INT_ST_M  (BIT(3))
#define GDMA_IN_DSCR_ERR_CH0_INT_ST_V  0x1
#define GDMA_IN_DSCR_ERR_CH0_INT_ST_S  3
/* GDMA_IN_ERR_EOF_CH0_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt..*/
#define GDMA_IN_ERR_EOF_CH0_INT_ST    (BIT(2))
#define GDMA_IN_ERR_EOF_CH0_INT_ST_M  (BIT(2))
#define GDMA_IN_ERR_EOF_CH0_INT_ST_V  0x1
#define GDMA_IN_ERR_EOF_CH0_INT_ST_S  2
/* GDMA_IN_SUC_EOF_CH0_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt..*/
#define GDMA_IN_SUC_EOF_CH0_INT_ST    (BIT(1))
#define GDMA_IN_SUC_EOF_CH0_INT_ST_M  (BIT(1))
#define GDMA_IN_SUC_EOF_CH0_INT_ST_V  0x1
#define GDMA_IN_SUC_EOF_CH0_INT_ST_S  1
/* GDMA_IN_DONE_CH0_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DONE_CH_INT interrupt..*/
#define GDMA_IN_DONE_CH0_INT_ST    (BIT(0))
#define GDMA_IN_DONE_CH0_INT_ST_M  (BIT(0))
#define GDMA_IN_DONE_CH0_INT_ST_V  0x1
#define GDMA_IN_DONE_CH0_INT_ST_S  0

#define GDMA_IN_INT_ENA_CH0_REG          (DR_REG_GDMA_BASE + 0x10)
/* GDMA_INFIFO_UDF_L3_CH0_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L3_CH0_INT_ENA    (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH0_INT_ENA_M  (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH0_INT_ENA_V  0x1
#define GDMA_INFIFO_UDF_L3_CH0_INT_ENA_S  9
/* GDMA_INFIFO_OVF_L3_CH0_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L3_CH0_INT_ENA    (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH0_INT_ENA_M  (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH0_INT_ENA_V  0x1
#define GDMA_INFIFO_OVF_L3_CH0_INT_ENA_S  8
/* GDMA_INFIFO_UDF_L1_CH0_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L1_CH0_INT_ENA    (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH0_INT_ENA_M  (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH0_INT_ENA_V  0x1
#define GDMA_INFIFO_UDF_L1_CH0_INT_ENA_S  7
/* GDMA_INFIFO_OVF_L1_CH0_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L1_CH0_INT_ENA    (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH0_INT_ENA_M  (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH0_INT_ENA_V  0x1
#define GDMA_INFIFO_OVF_L1_CH0_INT_ENA_S  6
/* GDMA_INFIFO_FULL_WM_CH0_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_FULL_WM_CH_INT interrupt..*/
#define GDMA_INFIFO_FULL_WM_CH0_INT_ENA    (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH0_INT_ENA_M  (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH0_INT_ENA_V  0x1
#define GDMA_INFIFO_FULL_WM_CH0_INT_ENA_S  5
/* GDMA_IN_DSCR_EMPTY_CH0_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt..*/
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ENA    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ENA_M  (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ENA_V  0x1
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ENA_S  4
/* GDMA_IN_DSCR_ERR_CH0_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_IN_DSCR_ERR_CH0_INT_ENA    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH0_INT_ENA_M  (BIT(3))
#define GDMA_IN_DSCR_ERR_CH0_INT_ENA_V  0x1
#define GDMA_IN_DSCR_ERR_CH0_INT_ENA_S  3
/* GDMA_IN_ERR_EOF_CH0_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt..*/
#define GDMA_IN_ERR_EOF_CH0_INT_ENA    (BIT(2))
#define GDMA_IN_ERR_EOF_CH0_INT_ENA_M  (BIT(2))
#define GDMA_IN_ERR_EOF_CH0_INT_ENA_V  0x1
#define GDMA_IN_ERR_EOF_CH0_INT_ENA_S  2
/* GDMA_IN_SUC_EOF_CH0_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt..*/
#define GDMA_IN_SUC_EOF_CH0_INT_ENA    (BIT(1))
#define GDMA_IN_SUC_EOF_CH0_INT_ENA_M  (BIT(1))
#define GDMA_IN_SUC_EOF_CH0_INT_ENA_V  0x1
#define GDMA_IN_SUC_EOF_CH0_INT_ENA_S  1
/* GDMA_IN_DONE_CH0_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DONE_CH_INT interrupt..*/
#define GDMA_IN_DONE_CH0_INT_ENA    (BIT(0))
#define GDMA_IN_DONE_CH0_INT_ENA_M  (BIT(0))
#define GDMA_IN_DONE_CH0_INT_ENA_V  0x1
#define GDMA_IN_DONE_CH0_INT_ENA_S  0

#define GDMA_IN_INT_CLR_CH0_REG          (DR_REG_GDMA_BASE + 0x14)
/* GDMA_INFIFO_UDF_L3_CH0_INT_CLR : WT ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L3_CH0_INT_CLR    (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH0_INT_CLR_M  (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH0_INT_CLR_V  0x1
#define GDMA_INFIFO_UDF_L3_CH0_INT_CLR_S  9
/* GDMA_INFIFO_OVF_L3_CH0_INT_CLR : WT ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L3_CH0_INT_CLR    (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH0_INT_CLR_M  (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH0_INT_CLR_V  0x1
#define GDMA_INFIFO_OVF_L3_CH0_INT_CLR_S  8
/* GDMA_INFIFO_UDF_L1_CH0_INT_CLR : WT ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L1_CH0_INT_CLR    (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH0_INT_CLR_M  (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH0_INT_CLR_V  0x1
#define GDMA_INFIFO_UDF_L1_CH0_INT_CLR_S  7
/* GDMA_INFIFO_OVF_L1_CH0_INT_CLR : WT ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L1_CH0_INT_CLR    (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH0_INT_CLR_M  (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH0_INT_CLR_V  0x1
#define GDMA_INFIFO_OVF_L1_CH0_INT_CLR_S  6
/* GDMA_DMA_INFIFO_FULL_WM_CH0_INT_CLR : WT ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_FULL_WM_CH_INT interrupt..*/
#define GDMA_DMA_INFIFO_FULL_WM_CH0_INT_CLR    (BIT(5))
#define GDMA_DMA_INFIFO_FULL_WM_CH0_INT_CLR_M  (BIT(5))
#define GDMA_DMA_INFIFO_FULL_WM_CH0_INT_CLR_V  0x1
#define GDMA_DMA_INFIFO_FULL_WM_CH0_INT_CLR_S  5
/* GDMA_IN_DSCR_EMPTY_CH0_INT_CLR : WT ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt..*/
#define GDMA_IN_DSCR_EMPTY_CH0_INT_CLR    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH0_INT_CLR_M  (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH0_INT_CLR_V  0x1
#define GDMA_IN_DSCR_EMPTY_CH0_INT_CLR_S  4
/* GDMA_IN_DSCR_ERR_CH0_INT_CLR : WT ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_IN_DSCR_ERR_CH0_INT_CLR    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH0_INT_CLR_M  (BIT(3))
#define GDMA_IN_DSCR_ERR_CH0_INT_CLR_V  0x1
#define GDMA_IN_DSCR_ERR_CH0_INT_CLR_S  3
/* GDMA_IN_ERR_EOF_CH0_INT_CLR : WT ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_ERR_EOF_CH_INT interrupt..*/
#define GDMA_IN_ERR_EOF_CH0_INT_CLR    (BIT(2))
#define GDMA_IN_ERR_EOF_CH0_INT_CLR_M  (BIT(2))
#define GDMA_IN_ERR_EOF_CH0_INT_CLR_V  0x1
#define GDMA_IN_ERR_EOF_CH0_INT_CLR_S  2
/* GDMA_IN_SUC_EOF_CH0_INT_CLR : WT ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_SUC_EOF_CH_INT interrupt..*/
#define GDMA_IN_SUC_EOF_CH0_INT_CLR    (BIT(1))
#define GDMA_IN_SUC_EOF_CH0_INT_CLR_M  (BIT(1))
#define GDMA_IN_SUC_EOF_CH0_INT_CLR_V  0x1
#define GDMA_IN_SUC_EOF_CH0_INT_CLR_S  1
/* GDMA_IN_DONE_CH0_INT_CLR : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DONE_CH_INT interrupt..*/
#define GDMA_IN_DONE_CH0_INT_CLR    (BIT(0))
#define GDMA_IN_DONE_CH0_INT_CLR_M  (BIT(0))
#define GDMA_IN_DONE_CH0_INT_CLR_V  0x1
#define GDMA_IN_DONE_CH0_INT_CLR_S  0

#define GDMA_INFIFO_STATUS_CH0_REG          (DR_REG_GDMA_BASE + 0x18)
/* GDMA_IN_BUF_HUNGRY_CH0 : RO ;bitpos:[28] ;default: 1'b0 ; */
/*description: reserved.*/
#define GDMA_IN_BUF_HUNGRY_CH0    (BIT(28))
#define GDMA_IN_BUF_HUNGRY_CH0_M  (BIT(28))
#define GDMA_IN_BUF_HUNGRY_CH0_V  0x1
#define GDMA_IN_BUF_HUNGRY_CH0_S  28
/* GDMA_IN_REMAIN_UNDER_4B_L3_CH0 : RO ;bitpos:[27] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_IN_REMAIN_UNDER_4B_L3_CH0    (BIT(27))
#define GDMA_IN_REMAIN_UNDER_4B_L3_CH0_M  (BIT(27))
#define GDMA_IN_REMAIN_UNDER_4B_L3_CH0_V  0x1
#define GDMA_IN_REMAIN_UNDER_4B_L3_CH0_S  27
/* GDMA_IN_REMAIN_UNDER_3B_L3_CH0 : RO ;bitpos:[26] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_IN_REMAIN_UNDER_3B_L3_CH0    (BIT(26))
#define GDMA_IN_REMAIN_UNDER_3B_L3_CH0_M  (BIT(26))
#define GDMA_IN_REMAIN_UNDER_3B_L3_CH0_V  0x1
#define GDMA_IN_REMAIN_UNDER_3B_L3_CH0_S  26
/* GDMA_IN_REMAIN_UNDER_2B_L3_CH0 : RO ;bitpos:[25] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_IN_REMAIN_UNDER_2B_L3_CH0    (BIT(25))
#define GDMA_IN_REMAIN_UNDER_2B_L3_CH0_M  (BIT(25))
#define GDMA_IN_REMAIN_UNDER_2B_L3_CH0_V  0x1
#define GDMA_IN_REMAIN_UNDER_2B_L3_CH0_S  25
/* GDMA_IN_REMAIN_UNDER_1B_L3_CH0 : RO ;bitpos:[24] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_IN_REMAIN_UNDER_1B_L3_CH0    (BIT(24))
#define GDMA_IN_REMAIN_UNDER_1B_L3_CH0_M  (BIT(24))
#define GDMA_IN_REMAIN_UNDER_1B_L3_CH0_V  0x1
#define GDMA_IN_REMAIN_UNDER_1B_L3_CH0_S  24
/* GDMA_INFIFO_CNT_L3_CH0 : RO ;bitpos:[23:19] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L3 Rx FIFO for Rx channel 0..*/
#define GDMA_INFIFO_CNT_L3_CH0    0x0000001F
#define GDMA_INFIFO_CNT_L3_CH0_M  ((GDMA_INFIFO_CNT_L3_CH0_V)<<(GDMA_INFIFO_CNT_L3_CH0_S))
#define GDMA_INFIFO_CNT_L3_CH0_V  0x1F
#define GDMA_INFIFO_CNT_L3_CH0_S  19
/* GDMA_INFIFO_CNT_L2_CH0 : RO ;bitpos:[18:12] ;default: 7'b0 ; */
/*description: The register stores the byte number of the data in L2 Rx FIFO for Rx channel 0..*/
#define GDMA_INFIFO_CNT_L2_CH0    0x0000007F
#define GDMA_INFIFO_CNT_L2_CH0_M  ((GDMA_INFIFO_CNT_L2_CH0_V)<<(GDMA_INFIFO_CNT_L2_CH0_S))
#define GDMA_INFIFO_CNT_L2_CH0_V  0x7F
#define GDMA_INFIFO_CNT_L2_CH0_S  12
/* GDMA_INFIFO_CNT_L1_CH0 : RO ;bitpos:[11:6] ;default: 6'b0 ; */
/*description: The register stores the byte number of the data in L1 Rx FIFO for Rx channel 0..*/
#define GDMA_INFIFO_CNT_L1_CH0    0x0000003F
#define GDMA_INFIFO_CNT_L1_CH0_M  ((GDMA_INFIFO_CNT_L1_CH0_V)<<(GDMA_INFIFO_CNT_L1_CH0_S))
#define GDMA_INFIFO_CNT_L1_CH0_V  0x3F
#define GDMA_INFIFO_CNT_L1_CH0_S  6
/* GDMA_INFIFO_EMPTY_L3_CH0 : RO ;bitpos:[5] ;default: 1'b1 ; */
/*description: L3 Rx FIFO empty signal for Rx channel 0..*/
#define GDMA_INFIFO_EMPTY_L3_CH0    (BIT(5))
#define GDMA_INFIFO_EMPTY_L3_CH0_M  (BIT(5))
#define GDMA_INFIFO_EMPTY_L3_CH0_V  0x1
#define GDMA_INFIFO_EMPTY_L3_CH0_S  5
/* GDMA_INFIFO_FULL_L3_CH0 : RO ;bitpos:[4] ;default: 1'b1 ; */
/*description: L3 Rx FIFO full signal for Rx channel 0..*/
#define GDMA_INFIFO_FULL_L3_CH0    (BIT(4))
#define GDMA_INFIFO_FULL_L3_CH0_M  (BIT(4))
#define GDMA_INFIFO_FULL_L3_CH0_V  0x1
#define GDMA_INFIFO_FULL_L3_CH0_S  4
/* GDMA_INFIFO_EMPTY_L2_CH0 : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: L2 Rx FIFO empty signal for Rx channel 0..*/
#define GDMA_INFIFO_EMPTY_L2_CH0    (BIT(3))
#define GDMA_INFIFO_EMPTY_L2_CH0_M  (BIT(3))
#define GDMA_INFIFO_EMPTY_L2_CH0_V  0x1
#define GDMA_INFIFO_EMPTY_L2_CH0_S  3
/* GDMA_INFIFO_FULL_L2_CH0 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: L2 Rx FIFO full signal for Rx channel 0..*/
#define GDMA_INFIFO_FULL_L2_CH0    (BIT(2))
#define GDMA_INFIFO_FULL_L2_CH0_M  (BIT(2))
#define GDMA_INFIFO_FULL_L2_CH0_V  0x1
#define GDMA_INFIFO_FULL_L2_CH0_S  2
/* GDMA_INFIFO_EMPTY_L1_CH0 : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: L1 Rx FIFO empty signal for Rx channel 0..*/
#define GDMA_INFIFO_EMPTY_L1_CH0    (BIT(1))
#define GDMA_INFIFO_EMPTY_L1_CH0_M  (BIT(1))
#define GDMA_INFIFO_EMPTY_L1_CH0_V  0x1
#define GDMA_INFIFO_EMPTY_L1_CH0_S  1
/* GDMA_INFIFO_FULL_L1_CH0 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: L1 Rx FIFO full signal for Rx channel 0..*/
#define GDMA_INFIFO_FULL_L1_CH0    (BIT(0))
#define GDMA_INFIFO_FULL_L1_CH0_M  (BIT(0))
#define GDMA_INFIFO_FULL_L1_CH0_V  0x1
#define GDMA_INFIFO_FULL_L1_CH0_S  0

#define GDMA_IN_POP_CH0_REG          (DR_REG_GDMA_BASE + 0x1C)
/* GDMA_INFIFO_POP_CH0 : R/W/SC ;bitpos:[12] ;default: 1'h0 ; */
/*description: Set this bit to pop data from DMA FIFO..*/
#define GDMA_INFIFO_POP_CH0    (BIT(12))
#define GDMA_INFIFO_POP_CH0_M  (BIT(12))
#define GDMA_INFIFO_POP_CH0_V  0x1
#define GDMA_INFIFO_POP_CH0_S  12
/* GDMA_INFIFO_RDATA_CH0 : RO ;bitpos:[11:0] ;default: 12'h800 ; */
/*description: This register stores the data popping from DMA FIFO..*/
#define GDMA_INFIFO_RDATA_CH0    0x00000FFF
#define GDMA_INFIFO_RDATA_CH0_M  ((GDMA_INFIFO_RDATA_CH0_V)<<(GDMA_INFIFO_RDATA_CH0_S))
#define GDMA_INFIFO_RDATA_CH0_V  0xFFF
#define GDMA_INFIFO_RDATA_CH0_S  0

#define GDMA_IN_LINK_CH0_REG          (DR_REG_GDMA_BASE + 0x20)
/* GDMA_INLINK_PARK_CH0 : RO ;bitpos:[24] ;default: 1'h1 ; */
/*description: 1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM
 is working..*/
#define GDMA_INLINK_PARK_CH0    (BIT(24))
#define GDMA_INLINK_PARK_CH0_M  (BIT(24))
#define GDMA_INLINK_PARK_CH0_V  0x1
#define GDMA_INLINK_PARK_CH0_S  24
/* GDMA_INLINK_RESTART_CH0 : R/W/SC ;bitpos:[23] ;default: 1'b0 ; */
/*description: Set this bit to mount a new inlink descriptor..*/
#define GDMA_INLINK_RESTART_CH0    (BIT(23))
#define GDMA_INLINK_RESTART_CH0_M  (BIT(23))
#define GDMA_INLINK_RESTART_CH0_V  0x1
#define GDMA_INLINK_RESTART_CH0_S  23
/* GDMA_INLINK_START_CH0 : R/W/SC ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to start dealing with the inlink descriptors..*/
#define GDMA_INLINK_START_CH0    (BIT(22))
#define GDMA_INLINK_START_CH0_M  (BIT(22))
#define GDMA_INLINK_START_CH0_V  0x1
#define GDMA_INLINK_START_CH0_S  22
/* GDMA_INLINK_STOP_CH0 : R/W/SC ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to stop dealing with the inlink descriptors..*/
#define GDMA_INLINK_STOP_CH0    (BIT(21))
#define GDMA_INLINK_STOP_CH0_M  (BIT(21))
#define GDMA_INLINK_STOP_CH0_V  0x1
#define GDMA_INLINK_STOP_CH0_S  21
/* GDMA_INLINK_AUTO_RET_CH0 : R/W ;bitpos:[20] ;default: 1'b1 ; */
/*description: Set this bit to return to current inlink descriptor's address, when there are
some errors in current receiving data..*/
#define GDMA_INLINK_AUTO_RET_CH0    (BIT(20))
#define GDMA_INLINK_AUTO_RET_CH0_M  (BIT(20))
#define GDMA_INLINK_AUTO_RET_CH0_V  0x1
#define GDMA_INLINK_AUTO_RET_CH0_S  20
/* GDMA_INLINK_ADDR_CH0 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: This register stores the 20 least significant bits of the first inlink
descriptor's address..*/
#define GDMA_INLINK_ADDR_CH0    0x000FFFFF
#define GDMA_INLINK_ADDR_CH0_M  ((GDMA_INLINK_ADDR_CH0_V)<<(GDMA_INLINK_ADDR_CH0_S))
#define GDMA_INLINK_ADDR_CH0_V  0xFFFFF
#define GDMA_INLINK_ADDR_CH0_S  0

#define GDMA_IN_STATE_CH0_REG          (DR_REG_GDMA_BASE + 0x24)
/* GDMA_IN_STATE_CH0 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: reserved.*/
#define GDMA_IN_STATE_CH0    0x00000007
#define GDMA_IN_STATE_CH0_M  ((GDMA_IN_STATE_CH0_V)<<(GDMA_IN_STATE_CH0_S))
#define GDMA_IN_STATE_CH0_V  0x7
#define GDMA_IN_STATE_CH0_S  20
/* GDMA_IN_DSCR_STATE_CH0 : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: reserved.*/
#define GDMA_IN_DSCR_STATE_CH0    0x00000003
#define GDMA_IN_DSCR_STATE_CH0_M  ((GDMA_IN_DSCR_STATE_CH0_V)<<(GDMA_IN_DSCR_STATE_CH0_S))
#define GDMA_IN_DSCR_STATE_CH0_V  0x3
#define GDMA_IN_DSCR_STATE_CH0_S  18
/* GDMA_INLINK_DSCR_ADDR_CH0 : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: This register stores the current inlink descriptor's address..*/
#define GDMA_INLINK_DSCR_ADDR_CH0    0x0003FFFF
#define GDMA_INLINK_DSCR_ADDR_CH0_M  ((GDMA_INLINK_DSCR_ADDR_CH0_V)<<(GDMA_INLINK_DSCR_ADDR_CH0_S))
#define GDMA_INLINK_DSCR_ADDR_CH0_V  0x3FFFF
#define GDMA_INLINK_DSCR_ADDR_CH0_S  0

#define GDMA_IN_SUC_EOF_DES_ADDR_CH0_REG          (DR_REG_GDMA_BASE + 0x28)
/* GDMA_IN_SUC_EOF_DES_ADDR_CH0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the inlink descriptor when the EOF bit in
this descriptor is 1..*/
#define GDMA_IN_SUC_EOF_DES_ADDR_CH0    0xFFFFFFFF
#define GDMA_IN_SUC_EOF_DES_ADDR_CH0_M  ((GDMA_IN_SUC_EOF_DES_ADDR_CH0_V)<<(GDMA_IN_SUC_EOF_DES_ADDR_CH0_S))
#define GDMA_IN_SUC_EOF_DES_ADDR_CH0_V  0xFFFFFFFF
#define GDMA_IN_SUC_EOF_DES_ADDR_CH0_S  0

#define GDMA_IN_ERR_EOF_DES_ADDR_CH0_REG          (DR_REG_GDMA_BASE + 0x2C)
/* GDMA_IN_ERR_EOF_DES_ADDR_CH0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the inlink descriptor when there are some
errors in current receiving data. Only used when peripheral is UHCI0..*/
#define GDMA_IN_ERR_EOF_DES_ADDR_CH0    0xFFFFFFFF
#define GDMA_IN_ERR_EOF_DES_ADDR_CH0_M  ((GDMA_IN_ERR_EOF_DES_ADDR_CH0_V)<<(GDMA_IN_ERR_EOF_DES_ADDR_CH0_S))
#define GDMA_IN_ERR_EOF_DES_ADDR_CH0_V  0xFFFFFFFF
#define GDMA_IN_ERR_EOF_DES_ADDR_CH0_S  0

#define GDMA_IN_DSCR_CH0_REG          (DR_REG_GDMA_BASE + 0x30)
/* GDMA_INLINK_DSCR_CH0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the current inlink descriptor x..*/
#define GDMA_INLINK_DSCR_CH0    0xFFFFFFFF
#define GDMA_INLINK_DSCR_CH0_M  ((GDMA_INLINK_DSCR_CH0_V)<<(GDMA_INLINK_DSCR_CH0_S))
#define GDMA_INLINK_DSCR_CH0_V  0xFFFFFFFF
#define GDMA_INLINK_DSCR_CH0_S  0

#define GDMA_IN_DSCR_BF0_CH0_REG          (DR_REG_GDMA_BASE + 0x34)
/* GDMA_INLINK_DSCR_BF0_CH0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the last inlink descriptor x-1..*/
#define GDMA_INLINK_DSCR_BF0_CH0    0xFFFFFFFF
#define GDMA_INLINK_DSCR_BF0_CH0_M  ((GDMA_INLINK_DSCR_BF0_CH0_V)<<(GDMA_INLINK_DSCR_BF0_CH0_S))
#define GDMA_INLINK_DSCR_BF0_CH0_V  0xFFFFFFFF
#define GDMA_INLINK_DSCR_BF0_CH0_S  0

#define GDMA_IN_DSCR_BF1_CH0_REG          (DR_REG_GDMA_BASE + 0x38)
/* GDMA_INLINK_DSCR_BF1_CH0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the second-to-last inlink descriptor x-2..*/
#define GDMA_INLINK_DSCR_BF1_CH0    0xFFFFFFFF
#define GDMA_INLINK_DSCR_BF1_CH0_M  ((GDMA_INLINK_DSCR_BF1_CH0_V)<<(GDMA_INLINK_DSCR_BF1_CH0_S))
#define GDMA_INLINK_DSCR_BF1_CH0_V  0xFFFFFFFF
#define GDMA_INLINK_DSCR_BF1_CH0_S  0

#define GDMA_IN_WEIGHT_CH0_REG          (DR_REG_GDMA_BASE + 0x3C)
/* GDMA_RX_WEIGHT_CH0 : R/W ;bitpos:[11:8] ;default: 4'hf ; */
/*description: The weight of Rx channel 0. .*/
#define GDMA_RX_WEIGHT_CH0    0x0000000F
#define GDMA_RX_WEIGHT_CH0_M  ((GDMA_RX_WEIGHT_CH0_V)<<(GDMA_RX_WEIGHT_CH0_S))
#define GDMA_RX_WEIGHT_CH0_V  0xF
#define GDMA_RX_WEIGHT_CH0_S  8

#define GDMA_IN_PRI_CH0_REG          (DR_REG_GDMA_BASE + 0x44)
/* GDMA_RX_PRI_CH0 : R/W ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: The priority of Rx channel 0. The larger of the value, the higher of the
priority..*/
#define GDMA_RX_PRI_CH0    0x0000000F
#define GDMA_RX_PRI_CH0_M  ((GDMA_RX_PRI_CH0_V)<<(GDMA_RX_PRI_CH0_S))
#define GDMA_RX_PRI_CH0_V  0xF
#define GDMA_RX_PRI_CH0_S  0

#define GDMA_IN_PERI_SEL_CH0_REG          (DR_REG_GDMA_BASE + 0x48)
/* GDMA_PERI_IN_SEL_CH0 : R/W ;bitpos:[5:0] ;default: 6'h3f ; */
/*description: This register is used to select peripheral for Rx channel 0. 0:SPI2. 1: SPI3. 2:
 UHCI0. 3: I2S0. 4: I2S1. 5: LCD_CAM. 6: AES. 7: SHA. 8: ADC_DAC. 9: RMT..*/
#define GDMA_PERI_IN_SEL_CH0    0x0000003F
#define GDMA_PERI_IN_SEL_CH0_M  ((GDMA_PERI_IN_SEL_CH0_V)<<(GDMA_PERI_IN_SEL_CH0_S))
#define GDMA_PERI_IN_SEL_CH0_V  0x3F
#define GDMA_PERI_IN_SEL_CH0_S  0

#define GDMA_OUT_CONF0_CH0_REG          (DR_REG_GDMA_BASE + 0x60)
/* GDMA_OUT_DATA_BURST_EN_CH0 : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Tx channel 0 transmitting
data when accessing internal SRAM. .*/
#define GDMA_OUT_DATA_BURST_EN_CH0    (BIT(5))
#define GDMA_OUT_DATA_BURST_EN_CH0_M  (BIT(5))
#define GDMA_OUT_DATA_BURST_EN_CH0_V  0x1
#define GDMA_OUT_DATA_BURST_EN_CH0_S  5
/* GDMA_OUTDSCR_BURST_EN_CH0 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Tx channel 0 reading link
descriptor when accessing internal SRAM. .*/
#define GDMA_OUTDSCR_BURST_EN_CH0    (BIT(4))
#define GDMA_OUTDSCR_BURST_EN_CH0_M  (BIT(4))
#define GDMA_OUTDSCR_BURST_EN_CH0_V  0x1
#define GDMA_OUTDSCR_BURST_EN_CH0_S  4
/* GDMA_OUT_EOF_MODE_CH0 : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel 0 is
 generated when data need to transmit has been popped from FIFO in DMA.*/
#define GDMA_OUT_EOF_MODE_CH0    (BIT(3))
#define GDMA_OUT_EOF_MODE_CH0_M  (BIT(3))
#define GDMA_OUT_EOF_MODE_CH0_V  0x1
#define GDMA_OUT_EOF_MODE_CH0_S  3
/* GDMA_OUT_AUTO_WRBACK_CH0 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to enable automatic outlink-writeback when all the data in tx
buffer has been transmitted..*/
#define GDMA_OUT_AUTO_WRBACK_CH0    (BIT(2))
#define GDMA_OUT_AUTO_WRBACK_CH0_M  (BIT(2))
#define GDMA_OUT_AUTO_WRBACK_CH0_V  0x1
#define GDMA_OUT_AUTO_WRBACK_CH0_S  2
/* GDMA_OUT_LOOP_TEST_CH0 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: reserved.*/
#define GDMA_OUT_LOOP_TEST_CH0    (BIT(1))
#define GDMA_OUT_LOOP_TEST_CH0_M  (BIT(1))
#define GDMA_OUT_LOOP_TEST_CH0_V  0x1
#define GDMA_OUT_LOOP_TEST_CH0_S  1
/* GDMA_OUT_RST_CH0 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: This bit is used to reset DMA channel 0 Tx FSM and Tx FIFO pointer..*/
#define GDMA_OUT_RST_CH0    (BIT(0))
#define GDMA_OUT_RST_CH0_M  (BIT(0))
#define GDMA_OUT_RST_CH0_V  0x1
#define GDMA_OUT_RST_CH0_S  0

#define GDMA_OUT_CONF1_CH0_REG          (DR_REG_GDMA_BASE + 0x64)
/* GDMA_OUT_EXT_MEM_BK_SIZE_CH0 : R/W ;bitpos:[14:13] ;default: 2'b0 ; */
/*description: Block size of Tx channel 0 when DMA access external SRAM. 0: 16 bytes      1: 32
 bytes    2/3:reserved.*/
#define GDMA_OUT_EXT_MEM_BK_SIZE_CH0    0x00000003
#define GDMA_OUT_EXT_MEM_BK_SIZE_CH0_M  ((GDMA_OUT_EXT_MEM_BK_SIZE_CH0_V)<<(GDMA_OUT_EXT_MEM_BK_SIZE_CH0_S))
#define GDMA_OUT_EXT_MEM_BK_SIZE_CH0_V  0x3
#define GDMA_OUT_EXT_MEM_BK_SIZE_CH0_S  13
/* GDMA_OUT_CHECK_OWNER_CH0 : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to enable checking the owner attribute of the link descriptor..*/
#define GDMA_OUT_CHECK_OWNER_CH0    (BIT(12))
#define GDMA_OUT_CHECK_OWNER_CH0_M  (BIT(12))
#define GDMA_OUT_CHECK_OWNER_CH0_V  0x1
#define GDMA_OUT_CHECK_OWNER_CH0_S  12

#define GDMA_OUT_INT_RAW_CH0_REG          (DR_REG_GDMA_BASE + 0x68)
/* GDMA_OUTFIFO_UDF_L3_CH0_INT_RAW : R/WTC/SS ;bitpos:[7] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo of Tx channel 0 is
underflow. .*/
#define GDMA_OUTFIFO_UDF_L3_CH0_INT_RAW    (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH0_INT_RAW_M  (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH0_INT_RAW_V  0x1
#define GDMA_OUTFIFO_UDF_L3_CH0_INT_RAW_S  7
/* GDMA_OUTFIFO_OVF_L3_CH0_INT_RAW : R/WTC/SS ;bitpos:[6] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo of Tx channel 0 is
overflow. .*/
#define GDMA_OUTFIFO_OVF_L3_CH0_INT_RAW    (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH0_INT_RAW_M  (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH0_INT_RAW_V  0x1
#define GDMA_OUTFIFO_OVF_L3_CH0_INT_RAW_S  6
/* GDMA_OUTFIFO_UDF_L1_CH0_INT_RAW : R/WTC/SS ;bitpos:[5] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo of Tx channel 0 is
underflow. .*/
#define GDMA_OUTFIFO_UDF_L1_CH0_INT_RAW    (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH0_INT_RAW_M  (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH0_INT_RAW_V  0x1
#define GDMA_OUTFIFO_UDF_L1_CH0_INT_RAW_S  5
/* GDMA_OUTFIFO_OVF_L1_CH0_INT_RAW : R/WTC/SS ;bitpos:[4] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo of Tx channel 0 is
overflow. .*/
#define GDMA_OUTFIFO_OVF_L1_CH0_INT_RAW    (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH0_INT_RAW_M  (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH0_INT_RAW_V  0x1
#define GDMA_OUTFIFO_OVF_L1_CH0_INT_RAW_S  4
/* GDMA_OUT_TOTAL_EOF_CH0_INT_RAW : R/WTC/SS ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when data corresponding a outlink
(includes one link descriptor or few link descriptors) is transmitted out for Tx cha
nnel 0..*/
#define GDMA_OUT_TOTAL_EOF_CH0_INT_RAW    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH0_INT_RAW_M  (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH0_INT_RAW_V  0x1
#define GDMA_OUT_TOTAL_EOF_CH0_INT_RAW_S  3
/* GDMA_OUT_DSCR_ERR_CH0_INT_RAW : R/WTC/SS ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when detecting outlink descriptor
error, including owner error, the second and third word error of outlink descriptor
for Tx channel 0..*/
#define GDMA_OUT_DSCR_ERR_CH0_INT_RAW    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH0_INT_RAW_M  (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH0_INT_RAW_V  0x1
#define GDMA_OUT_DSCR_ERR_CH0_INT_RAW_S  2
/* GDMA_OUT_EOF_CH0_INT_RAW : R/WTC/SS ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data pointed by one
outlink descriptor has been read from memory for Tx channel 0. .*/
#define GDMA_OUT_EOF_CH0_INT_RAW    (BIT(1))
#define GDMA_OUT_EOF_CH0_INT_RAW_M  (BIT(1))
#define GDMA_OUT_EOF_CH0_INT_RAW_V  0x1
#define GDMA_OUT_EOF_CH0_INT_RAW_S  1
/* GDMA_OUT_DONE_CH0_INT_RAW : R/WTC/SS ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data pointed by one
outlink descriptor has been transmitted to peripherals for Tx channel 0..*/
#define GDMA_OUT_DONE_CH0_INT_RAW    (BIT(0))
#define GDMA_OUT_DONE_CH0_INT_RAW_M  (BIT(0))
#define GDMA_OUT_DONE_CH0_INT_RAW_V  0x1
#define GDMA_OUT_DONE_CH0_INT_RAW_S  0

#define GDMA_OUT_INT_ST_CH0_REG          (DR_REG_GDMA_BASE + 0x6C)
/* GDMA_OUTFIFO_UDF_L3_CH0_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L3_CH0_INT_ST    (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH0_INT_ST_M  (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH0_INT_ST_V  0x1
#define GDMA_OUTFIFO_UDF_L3_CH0_INT_ST_S  7
/* GDMA_OUTFIFO_OVF_L3_CH0_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L3_CH0_INT_ST    (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH0_INT_ST_M  (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH0_INT_ST_V  0x1
#define GDMA_OUTFIFO_OVF_L3_CH0_INT_ST_S  6
/* GDMA_OUTFIFO_UDF_L1_CH0_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L1_CH0_INT_ST    (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH0_INT_ST_M  (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH0_INT_ST_V  0x1
#define GDMA_OUTFIFO_UDF_L1_CH0_INT_ST_S  5
/* GDMA_OUTFIFO_OVF_L1_CH0_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L1_CH0_INT_ST    (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH0_INT_ST_M  (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH0_INT_ST_V  0x1
#define GDMA_OUTFIFO_OVF_L1_CH0_INT_ST_S  4
/* GDMA_OUT_TOTAL_EOF_CH0_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt..*/
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ST    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ST_M  (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ST_V  0x1
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ST_S  3
/* GDMA_OUT_DSCR_ERR_CH0_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_OUT_DSCR_ERR_CH0_INT_ST    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH0_INT_ST_M  (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH0_INT_ST_V  0x1
#define GDMA_OUT_DSCR_ERR_CH0_INT_ST_S  2
/* GDMA_OUT_EOF_CH0_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_EOF_CH_INT interrupt..*/
#define GDMA_OUT_EOF_CH0_INT_ST    (BIT(1))
#define GDMA_OUT_EOF_CH0_INT_ST_M  (BIT(1))
#define GDMA_OUT_EOF_CH0_INT_ST_V  0x1
#define GDMA_OUT_EOF_CH0_INT_ST_S  1
/* GDMA_OUT_DONE_CH0_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_DONE_CH_INT interrupt..*/
#define GDMA_OUT_DONE_CH0_INT_ST    (BIT(0))
#define GDMA_OUT_DONE_CH0_INT_ST_M  (BIT(0))
#define GDMA_OUT_DONE_CH0_INT_ST_V  0x1
#define GDMA_OUT_DONE_CH0_INT_ST_S  0

#define GDMA_OUT_INT_ENA_CH0_REG          (DR_REG_GDMA_BASE + 0x70)
/* GDMA_OUTFIFO_UDF_L3_CH0_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L3_CH0_INT_ENA    (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH0_INT_ENA_M  (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH0_INT_ENA_V  0x1
#define GDMA_OUTFIFO_UDF_L3_CH0_INT_ENA_S  7
/* GDMA_OUTFIFO_OVF_L3_CH0_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L3_CH0_INT_ENA    (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH0_INT_ENA_M  (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH0_INT_ENA_V  0x1
#define GDMA_OUTFIFO_OVF_L3_CH0_INT_ENA_S  6
/* GDMA_OUTFIFO_UDF_L1_CH0_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L1_CH0_INT_ENA    (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH0_INT_ENA_M  (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH0_INT_ENA_V  0x1
#define GDMA_OUTFIFO_UDF_L1_CH0_INT_ENA_S  5
/* GDMA_OUTFIFO_OVF_L1_CH0_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L1_CH0_INT_ENA    (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH0_INT_ENA_M  (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH0_INT_ENA_V  0x1
#define GDMA_OUTFIFO_OVF_L1_CH0_INT_ENA_S  4
/* GDMA_OUT_TOTAL_EOF_CH0_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt..*/
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ENA    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ENA_M  (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ENA_V  0x1
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ENA_S  3
/* GDMA_OUT_DSCR_ERR_CH0_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_OUT_DSCR_ERR_CH0_INT_ENA    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH0_INT_ENA_M  (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH0_INT_ENA_V  0x1
#define GDMA_OUT_DSCR_ERR_CH0_INT_ENA_S  2
/* GDMA_OUT_EOF_CH0_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_EOF_CH_INT interrupt..*/
#define GDMA_OUT_EOF_CH0_INT_ENA    (BIT(1))
#define GDMA_OUT_EOF_CH0_INT_ENA_M  (BIT(1))
#define GDMA_OUT_EOF_CH0_INT_ENA_V  0x1
#define GDMA_OUT_EOF_CH0_INT_ENA_S  1
/* GDMA_OUT_DONE_CH0_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_DONE_CH_INT interrupt..*/
#define GDMA_OUT_DONE_CH0_INT_ENA    (BIT(0))
#define GDMA_OUT_DONE_CH0_INT_ENA_M  (BIT(0))
#define GDMA_OUT_DONE_CH0_INT_ENA_V  0x1
#define GDMA_OUT_DONE_CH0_INT_ENA_S  0

#define GDMA_OUT_INT_CLR_CH0_REG          (DR_REG_GDMA_BASE + 0x74)
/* GDMA_OUTFIFO_UDF_L3_CH0_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L3_CH0_INT_CLR    (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH0_INT_CLR_M  (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH0_INT_CLR_V  0x1
#define GDMA_OUTFIFO_UDF_L3_CH0_INT_CLR_S  7
/* GDMA_OUTFIFO_OVF_L3_CH0_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L3_CH0_INT_CLR    (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH0_INT_CLR_M  (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH0_INT_CLR_V  0x1
#define GDMA_OUTFIFO_OVF_L3_CH0_INT_CLR_S  6
/* GDMA_OUTFIFO_UDF_L1_CH0_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L1_CH0_INT_CLR    (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH0_INT_CLR_M  (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH0_INT_CLR_V  0x1
#define GDMA_OUTFIFO_UDF_L1_CH0_INT_CLR_S  5
/* GDMA_OUTFIFO_OVF_L1_CH0_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L1_CH0_INT_CLR    (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH0_INT_CLR_M  (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH0_INT_CLR_V  0x1
#define GDMA_OUTFIFO_OVF_L1_CH0_INT_CLR_S  4
/* GDMA_OUT_TOTAL_EOF_CH0_INT_CLR : WT ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt..*/
#define GDMA_OUT_TOTAL_EOF_CH0_INT_CLR    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH0_INT_CLR_M  (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH0_INT_CLR_V  0x1
#define GDMA_OUT_TOTAL_EOF_CH0_INT_CLR_S  3
/* GDMA_OUT_DSCR_ERR_CH0_INT_CLR : WT ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_OUT_DSCR_ERR_CH0_INT_CLR    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH0_INT_CLR_M  (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH0_INT_CLR_V  0x1
#define GDMA_OUT_DSCR_ERR_CH0_INT_CLR_S  2
/* GDMA_OUT_EOF_CH0_INT_CLR : WT ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_EOF_CH_INT interrupt..*/
#define GDMA_OUT_EOF_CH0_INT_CLR    (BIT(1))
#define GDMA_OUT_EOF_CH0_INT_CLR_M  (BIT(1))
#define GDMA_OUT_EOF_CH0_INT_CLR_V  0x1
#define GDMA_OUT_EOF_CH0_INT_CLR_S  1
/* GDMA_OUT_DONE_CH0_INT_CLR : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_DONE_CH_INT interrupt..*/
#define GDMA_OUT_DONE_CH0_INT_CLR    (BIT(0))
#define GDMA_OUT_DONE_CH0_INT_CLR_M  (BIT(0))
#define GDMA_OUT_DONE_CH0_INT_CLR_V  0x1
#define GDMA_OUT_DONE_CH0_INT_CLR_S  0

#define GDMA_OUTFIFO_STATUS_CH0_REG          (DR_REG_GDMA_BASE + 0x78)
/* GDMA_OUT_REMAIN_UNDER_4B_L3_CH0 : RO ;bitpos:[26] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_OUT_REMAIN_UNDER_4B_L3_CH0    (BIT(26))
#define GDMA_OUT_REMAIN_UNDER_4B_L3_CH0_M  (BIT(26))
#define GDMA_OUT_REMAIN_UNDER_4B_L3_CH0_V  0x1
#define GDMA_OUT_REMAIN_UNDER_4B_L3_CH0_S  26
/* GDMA_OUT_REMAIN_UNDER_3B_L3_CH0 : RO ;bitpos:[25] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_OUT_REMAIN_UNDER_3B_L3_CH0    (BIT(25))
#define GDMA_OUT_REMAIN_UNDER_3B_L3_CH0_M  (BIT(25))
#define GDMA_OUT_REMAIN_UNDER_3B_L3_CH0_V  0x1
#define GDMA_OUT_REMAIN_UNDER_3B_L3_CH0_S  25
/* GDMA_OUT_REMAIN_UNDER_2B_L3_CH0 : RO ;bitpos:[24] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_OUT_REMAIN_UNDER_2B_L3_CH0    (BIT(24))
#define GDMA_OUT_REMAIN_UNDER_2B_L3_CH0_M  (BIT(24))
#define GDMA_OUT_REMAIN_UNDER_2B_L3_CH0_V  0x1
#define GDMA_OUT_REMAIN_UNDER_2B_L3_CH0_S  24
/* GDMA_OUT_REMAIN_UNDER_1B_L3_CH0 : RO ;bitpos:[23] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_OUT_REMAIN_UNDER_1B_L3_CH0    (BIT(23))
#define GDMA_OUT_REMAIN_UNDER_1B_L3_CH0_M  (BIT(23))
#define GDMA_OUT_REMAIN_UNDER_1B_L3_CH0_V  0x1
#define GDMA_OUT_REMAIN_UNDER_1B_L3_CH0_S  23
/* GDMA_OUTFIFO_CNT_L3_CH0 : RO ;bitpos:[22:18] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L3 Tx FIFO for Tx channel 0..*/
#define GDMA_OUTFIFO_CNT_L3_CH0    0x0000001F
#define GDMA_OUTFIFO_CNT_L3_CH0_M  ((GDMA_OUTFIFO_CNT_L3_CH0_V)<<(GDMA_OUTFIFO_CNT_L3_CH0_S))
#define GDMA_OUTFIFO_CNT_L3_CH0_V  0x1F
#define GDMA_OUTFIFO_CNT_L3_CH0_S  18
/* GDMA_OUTFIFO_CNT_L2_CH0 : RO ;bitpos:[17:11] ;default: 7'b0 ; */
/*description: The register stores the byte number of the data in L2 Tx FIFO for Tx channel 0..*/
#define GDMA_OUTFIFO_CNT_L2_CH0    0x0000007F
#define GDMA_OUTFIFO_CNT_L2_CH0_M  ((GDMA_OUTFIFO_CNT_L2_CH0_V)<<(GDMA_OUTFIFO_CNT_L2_CH0_S))
#define GDMA_OUTFIFO_CNT_L2_CH0_V  0x7F
#define GDMA_OUTFIFO_CNT_L2_CH0_S  11
/* GDMA_OUTFIFO_CNT_L1_CH0 : RO ;bitpos:[10:6] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L1 Tx FIFO for Tx channel 0..*/
#define GDMA_OUTFIFO_CNT_L1_CH0    0x0000001F
#define GDMA_OUTFIFO_CNT_L1_CH0_M  ((GDMA_OUTFIFO_CNT_L1_CH0_V)<<(GDMA_OUTFIFO_CNT_L1_CH0_S))
#define GDMA_OUTFIFO_CNT_L1_CH0_V  0x1F
#define GDMA_OUTFIFO_CNT_L1_CH0_S  6
/* GDMA_OUTFIFO_EMPTY_L3_CH0 : RO ;bitpos:[5] ;default: 1'b1 ; */
/*description: L3 Tx FIFO empty signal for Tx channel 0..*/
#define GDMA_OUTFIFO_EMPTY_L3_CH0    (BIT(5))
#define GDMA_OUTFIFO_EMPTY_L3_CH0_M  (BIT(5))
#define GDMA_OUTFIFO_EMPTY_L3_CH0_V  0x1
#define GDMA_OUTFIFO_EMPTY_L3_CH0_S  5
/* GDMA_OUTFIFO_FULL_L3_CH0 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: L3 Tx FIFO full signal for Tx channel 0..*/
#define GDMA_OUTFIFO_FULL_L3_CH0    (BIT(4))
#define GDMA_OUTFIFO_FULL_L3_CH0_M  (BIT(4))
#define GDMA_OUTFIFO_FULL_L3_CH0_V  0x1
#define GDMA_OUTFIFO_FULL_L3_CH0_S  4
/* GDMA_OUTFIFO_EMPTY_L2_CH0 : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: L2 Tx FIFO empty signal for Tx channel 0..*/
#define GDMA_OUTFIFO_EMPTY_L2_CH0    (BIT(3))
#define GDMA_OUTFIFO_EMPTY_L2_CH0_M  (BIT(3))
#define GDMA_OUTFIFO_EMPTY_L2_CH0_V  0x1
#define GDMA_OUTFIFO_EMPTY_L2_CH0_S  3
/* GDMA_OUTFIFO_FULL_L2_CH0 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: L2 Tx FIFO full signal for Tx channel 0..*/
#define GDMA_OUTFIFO_FULL_L2_CH0    (BIT(2))
#define GDMA_OUTFIFO_FULL_L2_CH0_M  (BIT(2))
#define GDMA_OUTFIFO_FULL_L2_CH0_V  0x1
#define GDMA_OUTFIFO_FULL_L2_CH0_S  2
/* GDMA_OUTFIFO_EMPTY_L1_CH0 : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: L1 Tx FIFO empty signal for Tx channel 0..*/
#define GDMA_OUTFIFO_EMPTY_L1_CH0    (BIT(1))
#define GDMA_OUTFIFO_EMPTY_L1_CH0_M  (BIT(1))
#define GDMA_OUTFIFO_EMPTY_L1_CH0_V  0x1
#define GDMA_OUTFIFO_EMPTY_L1_CH0_S  1
/* GDMA_OUTFIFO_FULL_L1_CH0 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: L1 Tx FIFO full signal for Tx channel 0..*/
#define GDMA_OUTFIFO_FULL_L1_CH0    (BIT(0))
#define GDMA_OUTFIFO_FULL_L1_CH0_M  (BIT(0))
#define GDMA_OUTFIFO_FULL_L1_CH0_V  0x1
#define GDMA_OUTFIFO_FULL_L1_CH0_S  0

#define GDMA_OUT_PUSH_CH0_REG          (DR_REG_GDMA_BASE + 0x7C)
/* GDMA_OUTFIFO_PUSH_CH0 : R/W/SC ;bitpos:[9] ;default: 1'h0 ; */
/*description: Set this bit to push data into DMA FIFO..*/
#define GDMA_OUTFIFO_PUSH_CH0    (BIT(9))
#define GDMA_OUTFIFO_PUSH_CH0_M  (BIT(9))
#define GDMA_OUTFIFO_PUSH_CH0_V  0x1
#define GDMA_OUTFIFO_PUSH_CH0_S  9
/* GDMA_OUTFIFO_WDATA_CH0 : R/W ;bitpos:[8:0] ;default: 9'h0 ; */
/*description: This register stores the data that need to be pushed into DMA FIFO..*/
#define GDMA_OUTFIFO_WDATA_CH0    0x000001FF
#define GDMA_OUTFIFO_WDATA_CH0_M  ((GDMA_OUTFIFO_WDATA_CH0_V)<<(GDMA_OUTFIFO_WDATA_CH0_S))
#define GDMA_OUTFIFO_WDATA_CH0_V  0x1FF
#define GDMA_OUTFIFO_WDATA_CH0_S  0

#define GDMA_OUT_LINK_CH0_REG          (DR_REG_GDMA_BASE + 0x80)
/* GDMA_OUTLINK_PARK_CH0 : RO ;bitpos:[23] ;default: 1'h1 ; */
/*description: 1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's
FSM is working..*/
#define GDMA_OUTLINK_PARK_CH0    (BIT(23))
#define GDMA_OUTLINK_PARK_CH0_M  (BIT(23))
#define GDMA_OUTLINK_PARK_CH0_V  0x1
#define GDMA_OUTLINK_PARK_CH0_S  23
/* GDMA_OUTLINK_RESTART_CH0 : R/W/SC ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to restart a new outlink from the last address. .*/
#define GDMA_OUTLINK_RESTART_CH0    (BIT(22))
#define GDMA_OUTLINK_RESTART_CH0_M  (BIT(22))
#define GDMA_OUTLINK_RESTART_CH0_V  0x1
#define GDMA_OUTLINK_RESTART_CH0_S  22
/* GDMA_OUTLINK_START_CH0 : R/W/SC ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to start dealing with the outlink descriptors..*/
#define GDMA_OUTLINK_START_CH0    (BIT(21))
#define GDMA_OUTLINK_START_CH0_M  (BIT(21))
#define GDMA_OUTLINK_START_CH0_V  0x1
#define GDMA_OUTLINK_START_CH0_S  21
/* GDMA_OUTLINK_STOP_CH0 : R/W/SC ;bitpos:[20] ;default: 1'b0 ; */
/*description: Set this bit to stop dealing with the outlink descriptors..*/
#define GDMA_OUTLINK_STOP_CH0    (BIT(20))
#define GDMA_OUTLINK_STOP_CH0_M  (BIT(20))
#define GDMA_OUTLINK_STOP_CH0_V  0x1
#define GDMA_OUTLINK_STOP_CH0_S  20
/* GDMA_OUTLINK_ADDR_CH0 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: This register stores the 20 least significant bits of the first outlink
descriptor's address..*/
#define GDMA_OUTLINK_ADDR_CH0    0x000FFFFF
#define GDMA_OUTLINK_ADDR_CH0_M  ((GDMA_OUTLINK_ADDR_CH0_V)<<(GDMA_OUTLINK_ADDR_CH0_S))
#define GDMA_OUTLINK_ADDR_CH0_V  0xFFFFF
#define GDMA_OUTLINK_ADDR_CH0_S  0

#define GDMA_OUT_STATE_CH0_REG          (DR_REG_GDMA_BASE + 0x84)
/* GDMA_OUT_STATE_CH0 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: reserved.*/
#define GDMA_OUT_STATE_CH0    0x00000007
#define GDMA_OUT_STATE_CH0_M  ((GDMA_OUT_STATE_CH0_V)<<(GDMA_OUT_STATE_CH0_S))
#define GDMA_OUT_STATE_CH0_V  0x7
#define GDMA_OUT_STATE_CH0_S  20
/* GDMA_OUT_DSCR_STATE_CH0 : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: reserved.*/
#define GDMA_OUT_DSCR_STATE_CH0    0x00000003
#define GDMA_OUT_DSCR_STATE_CH0_M  ((GDMA_OUT_DSCR_STATE_CH0_V)<<(GDMA_OUT_DSCR_STATE_CH0_S))
#define GDMA_OUT_DSCR_STATE_CH0_V  0x3
#define GDMA_OUT_DSCR_STATE_CH0_S  18
/* GDMA_OUTLINK_DSCR_ADDR_CH0 : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: This register stores the current outlink descriptor's address..*/
#define GDMA_OUTLINK_DSCR_ADDR_CH0    0x0003FFFF
#define GDMA_OUTLINK_DSCR_ADDR_CH0_M  ((GDMA_OUTLINK_DSCR_ADDR_CH0_V)<<(GDMA_OUTLINK_DSCR_ADDR_CH0_S))
#define GDMA_OUTLINK_DSCR_ADDR_CH0_V  0x3FFFF
#define GDMA_OUTLINK_DSCR_ADDR_CH0_S  0

#define GDMA_OUT_EOF_DES_ADDR_CH0_REG          (DR_REG_GDMA_BASE + 0x88)
/* GDMA_OUT_EOF_DES_ADDR_CH0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the outlink descriptor when the EOF bit in
this descriptor is 1..*/
#define GDMA_OUT_EOF_DES_ADDR_CH0    0xFFFFFFFF
#define GDMA_OUT_EOF_DES_ADDR_CH0_M  ((GDMA_OUT_EOF_DES_ADDR_CH0_V)<<(GDMA_OUT_EOF_DES_ADDR_CH0_S))
#define GDMA_OUT_EOF_DES_ADDR_CH0_V  0xFFFFFFFF
#define GDMA_OUT_EOF_DES_ADDR_CH0_S  0

#define GDMA_OUT_EOF_BFR_DES_ADDR_CH0_REG          (DR_REG_GDMA_BASE + 0x8C)
/* GDMA_OUT_EOF_BFR_DES_ADDR_CH0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the outlink descriptor before the last
outlink descriptor..*/
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH0    0xFFFFFFFF
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH0_M  ((GDMA_OUT_EOF_BFR_DES_ADDR_CH0_V)<<(GDMA_OUT_EOF_BFR_DES_ADDR_CH0_S))
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH0_V  0xFFFFFFFF
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH0_S  0

#define GDMA_OUT_DSCR_CH0_REG          (DR_REG_GDMA_BASE + 0x90)
/* GDMA_OUTLINK_DSCR_CH0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the current outlink descriptor y..*/
#define GDMA_OUTLINK_DSCR_CH0    0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_CH0_M  ((GDMA_OUTLINK_DSCR_CH0_V)<<(GDMA_OUTLINK_DSCR_CH0_S))
#define GDMA_OUTLINK_DSCR_CH0_V  0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_CH0_S  0

#define GDMA_OUT_DSCR_BF0_CH0_REG          (DR_REG_GDMA_BASE + 0x94)
/* GDMA_OUTLINK_DSCR_BF0_CH0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the last outlink descriptor y-1..*/
#define GDMA_OUTLINK_DSCR_BF0_CH0    0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_BF0_CH0_M  ((GDMA_OUTLINK_DSCR_BF0_CH0_V)<<(GDMA_OUTLINK_DSCR_BF0_CH0_S))
#define GDMA_OUTLINK_DSCR_BF0_CH0_V  0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_BF0_CH0_S  0

#define GDMA_OUT_DSCR_BF1_CH0_REG          (DR_REG_GDMA_BASE + 0x98)
/* GDMA_OUTLINK_DSCR_BF1_CH0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the second-to-last inlink descriptor x-2..*/
#define GDMA_OUTLINK_DSCR_BF1_CH0    0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_BF1_CH0_M  ((GDMA_OUTLINK_DSCR_BF1_CH0_V)<<(GDMA_OUTLINK_DSCR_BF1_CH0_S))
#define GDMA_OUTLINK_DSCR_BF1_CH0_V  0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_BF1_CH0_S  0

#define GDMA_OUT_WEIGHT_CH0_REG          (DR_REG_GDMA_BASE + 0x9C)
/* GDMA_TX_WEIGHT_CH0 : R/W ;bitpos:[11:8] ;default: 4'hf ; */
/*description: The weight of Tx channel 0. .*/
#define GDMA_TX_WEIGHT_CH0    0x0000000F
#define GDMA_TX_WEIGHT_CH0_M  ((GDMA_TX_WEIGHT_CH0_V)<<(GDMA_TX_WEIGHT_CH0_S))
#define GDMA_TX_WEIGHT_CH0_V  0xF
#define GDMA_TX_WEIGHT_CH0_S  8

#define GDMA_OUT_PRI_CH0_REG          (DR_REG_GDMA_BASE + 0xA4)
/* GDMA_TX_PRI_CH0 : R/W ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: The priority of Tx channel 0. The larger of the value, the higher of the priorit
y..*/
#define GDMA_TX_PRI_CH0    0x0000000F
#define GDMA_TX_PRI_CH0_M  ((GDMA_TX_PRI_CH0_V)<<(GDMA_TX_PRI_CH0_S))
#define GDMA_TX_PRI_CH0_V  0xF
#define GDMA_TX_PRI_CH0_S  0

#define GDMA_OUT_PERI_SEL_CH0_REG          (DR_REG_GDMA_BASE + 0xA8)
/* GDMA_PERI_OUT_SEL_CH0 : R/W ;bitpos:[5:0] ;default: 6'h3f ; */
/*description: This register is used to select peripheral for Tx channel 0. 0:SPI2. 1: SPI3. 2:
 UHCI0. 3: I2S0. 4: I2S1. 5: LCD_CAM. 6: AES. 7: SHA. 8: ADC_DAC. 9: RMT.; 7: AE
S. 8: SHA. 9: ADC_DAC..*/
#define GDMA_PERI_OUT_SEL_CH0    0x0000003F
#define GDMA_PERI_OUT_SEL_CH0_M  ((GDMA_PERI_OUT_SEL_CH0_V)<<(GDMA_PERI_OUT_SEL_CH0_S))
#define GDMA_PERI_OUT_SEL_CH0_V  0x3F
#define GDMA_PERI_OUT_SEL_CH0_S  0

#define GDMA_IN_CONF0_CH1_REG          (DR_REG_GDMA_BASE + 0xC0)
/* GDMA_MEM_TRANS_EN_CH1 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit 1 to enable automatic transmitting data from memory to memory via
DMA..*/
#define GDMA_MEM_TRANS_EN_CH1    (BIT(4))
#define GDMA_MEM_TRANS_EN_CH1_M  (BIT(4))
#define GDMA_MEM_TRANS_EN_CH1_V  0x1
#define GDMA_MEM_TRANS_EN_CH1_S  4
/* GDMA_IN_DATA_BURST_EN_CH1 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Rx channel 1 receiving data
when accessing internal SRAM. .*/
#define GDMA_IN_DATA_BURST_EN_CH1    (BIT(3))
#define GDMA_IN_DATA_BURST_EN_CH1_M  (BIT(3))
#define GDMA_IN_DATA_BURST_EN_CH1_V  0x1
#define GDMA_IN_DATA_BURST_EN_CH1_S  3
/* GDMA_INDSCR_BURST_EN_CH1 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Rx channel 1 reading link
descriptor when accessing internal SRAM. .*/
#define GDMA_INDSCR_BURST_EN_CH1    (BIT(2))
#define GDMA_INDSCR_BURST_EN_CH1_M  (BIT(2))
#define GDMA_INDSCR_BURST_EN_CH1_V  0x1
#define GDMA_INDSCR_BURST_EN_CH1_S  2
/* GDMA_IN_LOOP_TEST_CH1 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: reserved.*/
#define GDMA_IN_LOOP_TEST_CH1    (BIT(1))
#define GDMA_IN_LOOP_TEST_CH1_M  (BIT(1))
#define GDMA_IN_LOOP_TEST_CH1_V  0x1
#define GDMA_IN_LOOP_TEST_CH1_S  1
/* GDMA_IN_RST_CH1 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: This bit is used to reset DMA channel 1 Rx FSM and Rx FIFO pointer..*/
#define GDMA_IN_RST_CH1    (BIT(0))
#define GDMA_IN_RST_CH1_M  (BIT(0))
#define GDMA_IN_RST_CH1_V  0x1
#define GDMA_IN_RST_CH1_S  0

#define GDMA_IN_CONF1_CH1_REG          (DR_REG_GDMA_BASE + 0xC4)
/* GDMA_IN_EXT_MEM_BK_SIZE_CH1 : R/W ;bitpos:[14:13] ;default: 2'b0 ; */
/*description: Block size of Rx channel 1 when DMA access external SRAM. 0: 16 bytes      1: 32
 bytes    2/3:reserved.*/
#define GDMA_IN_EXT_MEM_BK_SIZE_CH1    0x00000003
#define GDMA_IN_EXT_MEM_BK_SIZE_CH1_M  ((GDMA_IN_EXT_MEM_BK_SIZE_CH1_V)<<(GDMA_IN_EXT_MEM_BK_SIZE_CH1_S))
#define GDMA_IN_EXT_MEM_BK_SIZE_CH1_V  0x3
#define GDMA_IN_EXT_MEM_BK_SIZE_CH1_S  13
/* GDMA_IN_CHECK_OWNER_CH1 : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to enable checking the owner attribute of the link descriptor..*/
#define GDMA_IN_CHECK_OWNER_CH1    (BIT(12))
#define GDMA_IN_CHECK_OWNER_CH1_M  (BIT(12))
#define GDMA_IN_CHECK_OWNER_CH1_V  0x1
#define GDMA_IN_CHECK_OWNER_CH1_S  12
/* GDMA_DMA_INFIFO_FULL_THRS_CH1 : R/W ;bitpos:[11:0] ;default: 12'hc ; */
/*description: This register is used to generate the INFIFO_FULL_WM_INT interrupt when Rx
channel 0 received byte number in Rx FIFO is up to the value of the register..*/
#define GDMA_DMA_INFIFO_FULL_THRS_CH1    0x00000FFF
#define GDMA_DMA_INFIFO_FULL_THRS_CH1_M  ((GDMA_DMA_INFIFO_FULL_THRS_CH1_V)<<(GDMA_DMA_INFIFO_FULL_THRS_CH1_S))
#define GDMA_DMA_INFIFO_FULL_THRS_CH1_V  0xFFF
#define GDMA_DMA_INFIFO_FULL_THRS_CH1_S  0

#define GDMA_IN_INT_RAW_CH1_REG          (DR_REG_GDMA_BASE + 0xC8)
/* GDMA_INFIFO_UDF_L3_CH1_INT_RAW : R/WTC/SS ;bitpos:[9] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo of Rx channel 1 is
underflow. .*/
#define GDMA_INFIFO_UDF_L3_CH1_INT_RAW    (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH1_INT_RAW_M  (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH1_INT_RAW_V  0x1
#define GDMA_INFIFO_UDF_L3_CH1_INT_RAW_S  9
/* GDMA_INFIFO_OVF_L3_CH1_INT_RAW : R/WTC/SS ;bitpos:[8] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo of Rx channel 1 is
overflow. .*/
#define GDMA_INFIFO_OVF_L3_CH1_INT_RAW    (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH1_INT_RAW_M  (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH1_INT_RAW_V  0x1
#define GDMA_INFIFO_OVF_L3_CH1_INT_RAW_S  8
/* GDMA_INFIFO_UDF_L1_CH1_INT_RAW : R/WTC/SS ;bitpos:[7] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo of Rx channel 1 is
underflow. .*/
#define GDMA_INFIFO_UDF_L1_CH1_INT_RAW    (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH1_INT_RAW_M  (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH1_INT_RAW_V  0x1
#define GDMA_INFIFO_UDF_L1_CH1_INT_RAW_S  7
/* GDMA_INFIFO_OVF_L1_CH1_INT_RAW : R/WTC/SS ;bitpos:[6] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo of Rx channel 1 is
overflow. .*/
#define GDMA_INFIFO_OVF_L1_CH1_INT_RAW    (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH1_INT_RAW_M  (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH1_INT_RAW_V  0x1
#define GDMA_INFIFO_OVF_L1_CH1_INT_RAW_S  6
/* GDMA_INFIFO_FULL_WM_CH1_INT_RAW : R/WTC/SS ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when received data byte number is up
to threshold configured by REG_DMA_INFIFO_FULL_THRS_CH0 in Rx FIFO of channel 1..*/
#define GDMA_INFIFO_FULL_WM_CH1_INT_RAW    (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH1_INT_RAW_M  (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH1_INT_RAW_V  0x1
#define GDMA_INFIFO_FULL_WM_CH1_INT_RAW_S  5
/* GDMA_IN_DSCR_EMPTY_CH1_INT_RAW : R/WTC/SS ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when Rx buffer pointed by inlink is
full and receiving data is not completed, but there is no more inlink for Rx chann
el 1..*/
#define GDMA_IN_DSCR_EMPTY_CH1_INT_RAW    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH1_INT_RAW_M  (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH1_INT_RAW_V  0x1
#define GDMA_IN_DSCR_EMPTY_CH1_INT_RAW_S  4
/* GDMA_IN_DSCR_ERR_CH1_INT_RAW : R/WTC/SS ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when detecting inlink descriptor error
, including owner error, the second and third word error of inlink descriptor
for Rx channel 1..*/
#define GDMA_IN_DSCR_ERR_CH1_INT_RAW    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH1_INT_RAW_M  (BIT(3))
#define GDMA_IN_DSCR_ERR_CH1_INT_RAW_V  0x1
#define GDMA_IN_DSCR_ERR_CH1_INT_RAW_S  3
/* GDMA_IN_ERR_EOF_CH1_INT_RAW : R/WTC/SS ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when data error is detected only in
the case that the peripheral is UHCI0 for Rx channel 1. For other peripherals,
this raw interrupt is reserved..*/
#define GDMA_IN_ERR_EOF_CH1_INT_RAW    (BIT(2))
#define GDMA_IN_ERR_EOF_CH1_INT_RAW_M  (BIT(2))
#define GDMA_IN_ERR_EOF_CH1_INT_RAW_V  0x1
#define GDMA_IN_ERR_EOF_CH1_INT_RAW_S  2
/* GDMA_IN_SUC_EOF_CH1_INT_RAW : R/WTC/SS ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data pointed by one
inlink descriptor has been received for Rx channel 1. For UHCI0, the raw interrupt
bit turns to high level when the last data pointed by one inlink descriptor has
been received and no data error is detected for Rx channel 0..*/
#define GDMA_IN_SUC_EOF_CH1_INT_RAW    (BIT(1))
#define GDMA_IN_SUC_EOF_CH1_INT_RAW_M  (BIT(1))
#define GDMA_IN_SUC_EOF_CH1_INT_RAW_V  0x1
#define GDMA_IN_SUC_EOF_CH1_INT_RAW_S  1
/* GDMA_IN_DONE_CH1_INT_RAW : R/WTC/SS ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data pointed by one
inlink descriptor has been received for Rx channel 1..*/
#define GDMA_IN_DONE_CH1_INT_RAW    (BIT(0))
#define GDMA_IN_DONE_CH1_INT_RAW_M  (BIT(0))
#define GDMA_IN_DONE_CH1_INT_RAW_V  0x1
#define GDMA_IN_DONE_CH1_INT_RAW_S  0

#define GDMA_IN_INT_ST_CH1_REG          (DR_REG_GDMA_BASE + 0xCC)
/* GDMA_INFIFO_UDF_L3_CH1_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L3_CH1_INT_ST    (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH1_INT_ST_M  (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH1_INT_ST_V  0x1
#define GDMA_INFIFO_UDF_L3_CH1_INT_ST_S  9
/* GDMA_INFIFO_OVF_L3_CH1_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L3_CH1_INT_ST    (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH1_INT_ST_M  (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH1_INT_ST_V  0x1
#define GDMA_INFIFO_OVF_L3_CH1_INT_ST_S  8
/* GDMA_INFIFO_UDF_L1_CH1_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L1_CH1_INT_ST    (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH1_INT_ST_M  (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH1_INT_ST_V  0x1
#define GDMA_INFIFO_UDF_L1_CH1_INT_ST_S  7
/* GDMA_INFIFO_OVF_L1_CH1_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L1_CH1_INT_ST    (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH1_INT_ST_M  (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH1_INT_ST_V  0x1
#define GDMA_INFIFO_OVF_L1_CH1_INT_ST_S  6
/* GDMA_INFIFO_FULL_WM_CH1_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_FULL_WM_CH_INT interrupt..*/
#define GDMA_INFIFO_FULL_WM_CH1_INT_ST    (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH1_INT_ST_M  (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH1_INT_ST_V  0x1
#define GDMA_INFIFO_FULL_WM_CH1_INT_ST_S  5
/* GDMA_IN_DSCR_EMPTY_CH1_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt..*/
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ST    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ST_M  (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ST_V  0x1
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ST_S  4
/* GDMA_IN_DSCR_ERR_CH1_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_IN_DSCR_ERR_CH1_INT_ST    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH1_INT_ST_M  (BIT(3))
#define GDMA_IN_DSCR_ERR_CH1_INT_ST_V  0x1
#define GDMA_IN_DSCR_ERR_CH1_INT_ST_S  3
/* GDMA_IN_ERR_EOF_CH1_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt..*/
#define GDMA_IN_ERR_EOF_CH1_INT_ST    (BIT(2))
#define GDMA_IN_ERR_EOF_CH1_INT_ST_M  (BIT(2))
#define GDMA_IN_ERR_EOF_CH1_INT_ST_V  0x1
#define GDMA_IN_ERR_EOF_CH1_INT_ST_S  2
/* GDMA_IN_SUC_EOF_CH1_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt..*/
#define GDMA_IN_SUC_EOF_CH1_INT_ST    (BIT(1))
#define GDMA_IN_SUC_EOF_CH1_INT_ST_M  (BIT(1))
#define GDMA_IN_SUC_EOF_CH1_INT_ST_V  0x1
#define GDMA_IN_SUC_EOF_CH1_INT_ST_S  1
/* GDMA_IN_DONE_CH1_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DONE_CH_INT interrupt..*/
#define GDMA_IN_DONE_CH1_INT_ST    (BIT(0))
#define GDMA_IN_DONE_CH1_INT_ST_M  (BIT(0))
#define GDMA_IN_DONE_CH1_INT_ST_V  0x1
#define GDMA_IN_DONE_CH1_INT_ST_S  0

#define GDMA_IN_INT_ENA_CH1_REG          (DR_REG_GDMA_BASE + 0xD0)
/* GDMA_INFIFO_UDF_L3_CH1_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L3_CH1_INT_ENA    (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH1_INT_ENA_M  (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH1_INT_ENA_V  0x1
#define GDMA_INFIFO_UDF_L3_CH1_INT_ENA_S  9
/* GDMA_INFIFO_OVF_L3_CH1_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L3_CH1_INT_ENA    (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH1_INT_ENA_M  (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH1_INT_ENA_V  0x1
#define GDMA_INFIFO_OVF_L3_CH1_INT_ENA_S  8
/* GDMA_INFIFO_UDF_L1_CH1_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L1_CH1_INT_ENA    (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH1_INT_ENA_M  (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH1_INT_ENA_V  0x1
#define GDMA_INFIFO_UDF_L1_CH1_INT_ENA_S  7
/* GDMA_INFIFO_OVF_L1_CH1_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L1_CH1_INT_ENA    (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH1_INT_ENA_M  (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH1_INT_ENA_V  0x1
#define GDMA_INFIFO_OVF_L1_CH1_INT_ENA_S  6
/* GDMA_INFIFO_FULL_WM_CH1_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_FULL_WM_CH_INT interrupt..*/
#define GDMA_INFIFO_FULL_WM_CH1_INT_ENA    (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH1_INT_ENA_M  (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH1_INT_ENA_V  0x1
#define GDMA_INFIFO_FULL_WM_CH1_INT_ENA_S  5
/* GDMA_IN_DSCR_EMPTY_CH1_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt..*/
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ENA    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ENA_M  (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ENA_V  0x1
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ENA_S  4
/* GDMA_IN_DSCR_ERR_CH1_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_IN_DSCR_ERR_CH1_INT_ENA    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH1_INT_ENA_M  (BIT(3))
#define GDMA_IN_DSCR_ERR_CH1_INT_ENA_V  0x1
#define GDMA_IN_DSCR_ERR_CH1_INT_ENA_S  3
/* GDMA_IN_ERR_EOF_CH1_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt..*/
#define GDMA_IN_ERR_EOF_CH1_INT_ENA    (BIT(2))
#define GDMA_IN_ERR_EOF_CH1_INT_ENA_M  (BIT(2))
#define GDMA_IN_ERR_EOF_CH1_INT_ENA_V  0x1
#define GDMA_IN_ERR_EOF_CH1_INT_ENA_S  2
/* GDMA_IN_SUC_EOF_CH1_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt..*/
#define GDMA_IN_SUC_EOF_CH1_INT_ENA    (BIT(1))
#define GDMA_IN_SUC_EOF_CH1_INT_ENA_M  (BIT(1))
#define GDMA_IN_SUC_EOF_CH1_INT_ENA_V  0x1
#define GDMA_IN_SUC_EOF_CH1_INT_ENA_S  1
/* GDMA_IN_DONE_CH1_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DONE_CH_INT interrupt..*/
#define GDMA_IN_DONE_CH1_INT_ENA    (BIT(0))
#define GDMA_IN_DONE_CH1_INT_ENA_M  (BIT(0))
#define GDMA_IN_DONE_CH1_INT_ENA_V  0x1
#define GDMA_IN_DONE_CH1_INT_ENA_S  0

#define GDMA_IN_INT_CLR_CH1_REG          (DR_REG_GDMA_BASE + 0xD4)
/* GDMA_INFIFO_UDF_L3_CH1_INT_CLR : WT ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L3_CH1_INT_CLR    (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH1_INT_CLR_M  (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH1_INT_CLR_V  0x1
#define GDMA_INFIFO_UDF_L3_CH1_INT_CLR_S  9
/* GDMA_INFIFO_OVF_L3_CH1_INT_CLR : WT ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L3_CH1_INT_CLR    (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH1_INT_CLR_M  (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH1_INT_CLR_V  0x1
#define GDMA_INFIFO_OVF_L3_CH1_INT_CLR_S  8
/* GDMA_INFIFO_UDF_L1_CH1_INT_CLR : WT ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L1_CH1_INT_CLR    (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH1_INT_CLR_M  (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH1_INT_CLR_V  0x1
#define GDMA_INFIFO_UDF_L1_CH1_INT_CLR_S  7
/* GDMA_INFIFO_OVF_L1_CH1_INT_CLR : WT ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L1_CH1_INT_CLR    (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH1_INT_CLR_M  (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH1_INT_CLR_V  0x1
#define GDMA_INFIFO_OVF_L1_CH1_INT_CLR_S  6
/* GDMA_DMA_INFIFO_FULL_WM_CH1_INT_CLR : WT ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_FULL_WM_CH_INT interrupt..*/
#define GDMA_DMA_INFIFO_FULL_WM_CH1_INT_CLR    (BIT(5))
#define GDMA_DMA_INFIFO_FULL_WM_CH1_INT_CLR_M  (BIT(5))
#define GDMA_DMA_INFIFO_FULL_WM_CH1_INT_CLR_V  0x1
#define GDMA_DMA_INFIFO_FULL_WM_CH1_INT_CLR_S  5
/* GDMA_IN_DSCR_EMPTY_CH1_INT_CLR : WT ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt..*/
#define GDMA_IN_DSCR_EMPTY_CH1_INT_CLR    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH1_INT_CLR_M  (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH1_INT_CLR_V  0x1
#define GDMA_IN_DSCR_EMPTY_CH1_INT_CLR_S  4
/* GDMA_IN_DSCR_ERR_CH1_INT_CLR : WT ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_IN_DSCR_ERR_CH1_INT_CLR    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH1_INT_CLR_M  (BIT(3))
#define GDMA_IN_DSCR_ERR_CH1_INT_CLR_V  0x1
#define GDMA_IN_DSCR_ERR_CH1_INT_CLR_S  3
/* GDMA_IN_ERR_EOF_CH1_INT_CLR : WT ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_ERR_EOF_CH_INT interrupt..*/
#define GDMA_IN_ERR_EOF_CH1_INT_CLR    (BIT(2))
#define GDMA_IN_ERR_EOF_CH1_INT_CLR_M  (BIT(2))
#define GDMA_IN_ERR_EOF_CH1_INT_CLR_V  0x1
#define GDMA_IN_ERR_EOF_CH1_INT_CLR_S  2
/* GDMA_IN_SUC_EOF_CH1_INT_CLR : WT ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_SUC_EOF_CH_INT interrupt..*/
#define GDMA_IN_SUC_EOF_CH1_INT_CLR    (BIT(1))
#define GDMA_IN_SUC_EOF_CH1_INT_CLR_M  (BIT(1))
#define GDMA_IN_SUC_EOF_CH1_INT_CLR_V  0x1
#define GDMA_IN_SUC_EOF_CH1_INT_CLR_S  1
/* GDMA_IN_DONE_CH1_INT_CLR : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DONE_CH_INT interrupt..*/
#define GDMA_IN_DONE_CH1_INT_CLR    (BIT(0))
#define GDMA_IN_DONE_CH1_INT_CLR_M  (BIT(0))
#define GDMA_IN_DONE_CH1_INT_CLR_V  0x1
#define GDMA_IN_DONE_CH1_INT_CLR_S  0

#define GDMA_INFIFO_STATUS_CH1_REG          (DR_REG_GDMA_BASE + 0xD8)
/* GDMA_IN_BUF_HUNGRY_CH1 : RO ;bitpos:[28] ;default: 1'b0 ; */
/*description: reserved.*/
#define GDMA_IN_BUF_HUNGRY_CH1    (BIT(28))
#define GDMA_IN_BUF_HUNGRY_CH1_M  (BIT(28))
#define GDMA_IN_BUF_HUNGRY_CH1_V  0x1
#define GDMA_IN_BUF_HUNGRY_CH1_S  28
/* GDMA_IN_REMAIN_UNDER_4B_L3_CH1 : RO ;bitpos:[27] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_IN_REMAIN_UNDER_4B_L3_CH1    (BIT(27))
#define GDMA_IN_REMAIN_UNDER_4B_L3_CH1_M  (BIT(27))
#define GDMA_IN_REMAIN_UNDER_4B_L3_CH1_V  0x1
#define GDMA_IN_REMAIN_UNDER_4B_L3_CH1_S  27
/* GDMA_IN_REMAIN_UNDER_3B_L3_CH1 : RO ;bitpos:[26] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_IN_REMAIN_UNDER_3B_L3_CH1    (BIT(26))
#define GDMA_IN_REMAIN_UNDER_3B_L3_CH1_M  (BIT(26))
#define GDMA_IN_REMAIN_UNDER_3B_L3_CH1_V  0x1
#define GDMA_IN_REMAIN_UNDER_3B_L3_CH1_S  26
/* GDMA_IN_REMAIN_UNDER_2B_L3_CH1 : RO ;bitpos:[25] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_IN_REMAIN_UNDER_2B_L3_CH1    (BIT(25))
#define GDMA_IN_REMAIN_UNDER_2B_L3_CH1_M  (BIT(25))
#define GDMA_IN_REMAIN_UNDER_2B_L3_CH1_V  0x1
#define GDMA_IN_REMAIN_UNDER_2B_L3_CH1_S  25
/* GDMA_IN_REMAIN_UNDER_1B_L3_CH1 : RO ;bitpos:[24] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_IN_REMAIN_UNDER_1B_L3_CH1    (BIT(24))
#define GDMA_IN_REMAIN_UNDER_1B_L3_CH1_M  (BIT(24))
#define GDMA_IN_REMAIN_UNDER_1B_L3_CH1_V  0x1
#define GDMA_IN_REMAIN_UNDER_1B_L3_CH1_S  24
/* GDMA_INFIFO_CNT_L3_CH1 : RO ;bitpos:[23:19] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L3 Rx FIFO for Rx channel 1..*/
#define GDMA_INFIFO_CNT_L3_CH1    0x0000001F
#define GDMA_INFIFO_CNT_L3_CH1_M  ((GDMA_INFIFO_CNT_L3_CH1_V)<<(GDMA_INFIFO_CNT_L3_CH1_S))
#define GDMA_INFIFO_CNT_L3_CH1_V  0x1F
#define GDMA_INFIFO_CNT_L3_CH1_S  19
/* GDMA_INFIFO_CNT_L2_CH1 : RO ;bitpos:[18:12] ;default: 7'b0 ; */
/*description: The register stores the byte number of the data in L2 Rx FIFO for Rx channel 1..*/
#define GDMA_INFIFO_CNT_L2_CH1    0x0000007F
#define GDMA_INFIFO_CNT_L2_CH1_M  ((GDMA_INFIFO_CNT_L2_CH1_V)<<(GDMA_INFIFO_CNT_L2_CH1_S))
#define GDMA_INFIFO_CNT_L2_CH1_V  0x7F
#define GDMA_INFIFO_CNT_L2_CH1_S  12
/* GDMA_INFIFO_CNT_L1_CH1 : RO ;bitpos:[11:6] ;default: 6'b0 ; */
/*description: The register stores the byte number of the data in L1 Rx FIFO for Rx channel 1..*/
#define GDMA_INFIFO_CNT_L1_CH1    0x0000003F
#define GDMA_INFIFO_CNT_L1_CH1_M  ((GDMA_INFIFO_CNT_L1_CH1_V)<<(GDMA_INFIFO_CNT_L1_CH1_S))
#define GDMA_INFIFO_CNT_L1_CH1_V  0x3F
#define GDMA_INFIFO_CNT_L1_CH1_S  6
/* GDMA_INFIFO_EMPTY_L3_CH1 : RO ;bitpos:[5] ;default: 1'b1 ; */
/*description: L3 Rx FIFO empty signal for Rx channel 1..*/
#define GDMA_INFIFO_EMPTY_L3_CH1    (BIT(5))
#define GDMA_INFIFO_EMPTY_L3_CH1_M  (BIT(5))
#define GDMA_INFIFO_EMPTY_L3_CH1_V  0x1
#define GDMA_INFIFO_EMPTY_L3_CH1_S  5
/* GDMA_INFIFO_FULL_L3_CH1 : RO ;bitpos:[4] ;default: 1'b1 ; */
/*description: L3 Rx FIFO full signal for Rx channel 1..*/
#define GDMA_INFIFO_FULL_L3_CH1    (BIT(4))
#define GDMA_INFIFO_FULL_L3_CH1_M  (BIT(4))
#define GDMA_INFIFO_FULL_L3_CH1_V  0x1
#define GDMA_INFIFO_FULL_L3_CH1_S  4
/* GDMA_INFIFO_EMPTY_L2_CH1 : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: L2 Rx FIFO empty signal for Rx channel 1..*/
#define GDMA_INFIFO_EMPTY_L2_CH1    (BIT(3))
#define GDMA_INFIFO_EMPTY_L2_CH1_M  (BIT(3))
#define GDMA_INFIFO_EMPTY_L2_CH1_V  0x1
#define GDMA_INFIFO_EMPTY_L2_CH1_S  3
/* GDMA_INFIFO_FULL_L2_CH1 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: L2 Rx FIFO full signal for Rx channel 1..*/
#define GDMA_INFIFO_FULL_L2_CH1    (BIT(2))
#define GDMA_INFIFO_FULL_L2_CH1_M  (BIT(2))
#define GDMA_INFIFO_FULL_L2_CH1_V  0x1
#define GDMA_INFIFO_FULL_L2_CH1_S  2
/* GDMA_INFIFO_EMPTY_L1_CH1 : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: L1 Rx FIFO empty signal for Rx channel 1..*/
#define GDMA_INFIFO_EMPTY_L1_CH1    (BIT(1))
#define GDMA_INFIFO_EMPTY_L1_CH1_M  (BIT(1))
#define GDMA_INFIFO_EMPTY_L1_CH1_V  0x1
#define GDMA_INFIFO_EMPTY_L1_CH1_S  1
/* GDMA_INFIFO_FULL_L1_CH1 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: L1 Rx FIFO full signal for Rx channel 1..*/
#define GDMA_INFIFO_FULL_L1_CH1    (BIT(0))
#define GDMA_INFIFO_FULL_L1_CH1_M  (BIT(0))
#define GDMA_INFIFO_FULL_L1_CH1_V  0x1
#define GDMA_INFIFO_FULL_L1_CH1_S  0

#define GDMA_IN_POP_CH1_REG          (DR_REG_GDMA_BASE + 0xDC)
/* GDMA_INFIFO_POP_CH1 : R/W/SC ;bitpos:[12] ;default: 1'h0 ; */
/*description: Set this bit to pop data from DMA FIFO..*/
#define GDMA_INFIFO_POP_CH1    (BIT(12))
#define GDMA_INFIFO_POP_CH1_M  (BIT(12))
#define GDMA_INFIFO_POP_CH1_V  0x1
#define GDMA_INFIFO_POP_CH1_S  12
/* GDMA_INFIFO_RDATA_CH1 : RO ;bitpos:[11:0] ;default: 12'h800 ; */
/*description: This register stores the data popping from DMA FIFO..*/
#define GDMA_INFIFO_RDATA_CH1    0x00000FFF
#define GDMA_INFIFO_RDATA_CH1_M  ((GDMA_INFIFO_RDATA_CH1_V)<<(GDMA_INFIFO_RDATA_CH1_S))
#define GDMA_INFIFO_RDATA_CH1_V  0xFFF
#define GDMA_INFIFO_RDATA_CH1_S  0

#define GDMA_IN_LINK_CH1_REG          (DR_REG_GDMA_BASE + 0xE0)
/* GDMA_INLINK_PARK_CH1 : RO ;bitpos:[24] ;default: 1'h1 ; */
/*description: 1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM
 is working..*/
#define GDMA_INLINK_PARK_CH1    (BIT(24))
#define GDMA_INLINK_PARK_CH1_M  (BIT(24))
#define GDMA_INLINK_PARK_CH1_V  0x1
#define GDMA_INLINK_PARK_CH1_S  24
/* GDMA_INLINK_RESTART_CH1 : R/W/SC ;bitpos:[23] ;default: 1'b0 ; */
/*description: Set this bit to mount a new inlink descriptor..*/
#define GDMA_INLINK_RESTART_CH1    (BIT(23))
#define GDMA_INLINK_RESTART_CH1_M  (BIT(23))
#define GDMA_INLINK_RESTART_CH1_V  0x1
#define GDMA_INLINK_RESTART_CH1_S  23
/* GDMA_INLINK_START_CH1 : R/W/SC ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to start dealing with the inlink descriptors..*/
#define GDMA_INLINK_START_CH1    (BIT(22))
#define GDMA_INLINK_START_CH1_M  (BIT(22))
#define GDMA_INLINK_START_CH1_V  0x1
#define GDMA_INLINK_START_CH1_S  22
/* GDMA_INLINK_STOP_CH1 : R/W/SC ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to stop dealing with the inlink descriptors..*/
#define GDMA_INLINK_STOP_CH1    (BIT(21))
#define GDMA_INLINK_STOP_CH1_M  (BIT(21))
#define GDMA_INLINK_STOP_CH1_V  0x1
#define GDMA_INLINK_STOP_CH1_S  21
/* GDMA_INLINK_AUTO_RET_CH1 : R/W ;bitpos:[20] ;default: 1'b1 ; */
/*description: Set this bit to return to current inlink descriptor's address, when there are
some errors in current receiving data..*/
#define GDMA_INLINK_AUTO_RET_CH1    (BIT(20))
#define GDMA_INLINK_AUTO_RET_CH1_M  (BIT(20))
#define GDMA_INLINK_AUTO_RET_CH1_V  0x1
#define GDMA_INLINK_AUTO_RET_CH1_S  20
/* GDMA_INLINK_ADDR_CH1 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: This register stores the 20 least significant bits of the first inlink
descriptor's address..*/
#define GDMA_INLINK_ADDR_CH1    0x000FFFFF
#define GDMA_INLINK_ADDR_CH1_M  ((GDMA_INLINK_ADDR_CH1_V)<<(GDMA_INLINK_ADDR_CH1_S))
#define GDMA_INLINK_ADDR_CH1_V  0xFFFFF
#define GDMA_INLINK_ADDR_CH1_S  0

#define GDMA_IN_STATE_CH1_REG          (DR_REG_GDMA_BASE + 0xE4)
/* GDMA_IN_STATE_CH1 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: reserved.*/
#define GDMA_IN_STATE_CH1    0x00000007
#define GDMA_IN_STATE_CH1_M  ((GDMA_IN_STATE_CH1_V)<<(GDMA_IN_STATE_CH1_S))
#define GDMA_IN_STATE_CH1_V  0x7
#define GDMA_IN_STATE_CH1_S  20
/* GDMA_IN_DSCR_STATE_CH1 : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: reserved.*/
#define GDMA_IN_DSCR_STATE_CH1    0x00000003
#define GDMA_IN_DSCR_STATE_CH1_M  ((GDMA_IN_DSCR_STATE_CH1_V)<<(GDMA_IN_DSCR_STATE_CH1_S))
#define GDMA_IN_DSCR_STATE_CH1_V  0x3
#define GDMA_IN_DSCR_STATE_CH1_S  18
/* GDMA_INLINK_DSCR_ADDR_CH1 : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: This register stores the current inlink descriptor's address..*/
#define GDMA_INLINK_DSCR_ADDR_CH1    0x0003FFFF
#define GDMA_INLINK_DSCR_ADDR_CH1_M  ((GDMA_INLINK_DSCR_ADDR_CH1_V)<<(GDMA_INLINK_DSCR_ADDR_CH1_S))
#define GDMA_INLINK_DSCR_ADDR_CH1_V  0x3FFFF
#define GDMA_INLINK_DSCR_ADDR_CH1_S  0

#define GDMA_IN_SUC_EOF_DES_ADDR_CH1_REG          (DR_REG_GDMA_BASE + 0xE8)
/* GDMA_IN_SUC_EOF_DES_ADDR_CH1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the inlink descriptor when the EOF bit in
this descriptor is 1..*/
#define GDMA_IN_SUC_EOF_DES_ADDR_CH1    0xFFFFFFFF
#define GDMA_IN_SUC_EOF_DES_ADDR_CH1_M  ((GDMA_IN_SUC_EOF_DES_ADDR_CH1_V)<<(GDMA_IN_SUC_EOF_DES_ADDR_CH1_S))
#define GDMA_IN_SUC_EOF_DES_ADDR_CH1_V  0xFFFFFFFF
#define GDMA_IN_SUC_EOF_DES_ADDR_CH1_S  0

#define GDMA_IN_ERR_EOF_DES_ADDR_CH1_REG          (DR_REG_GDMA_BASE + 0xEC)
/* GDMA_IN_ERR_EOF_DES_ADDR_CH1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the inlink descriptor when there are some
errors in current receiving data. Only used when peripheral is UHCI0..*/
#define GDMA_IN_ERR_EOF_DES_ADDR_CH1    0xFFFFFFFF
#define GDMA_IN_ERR_EOF_DES_ADDR_CH1_M  ((GDMA_IN_ERR_EOF_DES_ADDR_CH1_V)<<(GDMA_IN_ERR_EOF_DES_ADDR_CH1_S))
#define GDMA_IN_ERR_EOF_DES_ADDR_CH1_V  0xFFFFFFFF
#define GDMA_IN_ERR_EOF_DES_ADDR_CH1_S  0

#define GDMA_IN_DSCR_CH1_REG          (DR_REG_GDMA_BASE + 0xF0)
/* GDMA_INLINK_DSCR_CH1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the current inlink descriptor x..*/
#define GDMA_INLINK_DSCR_CH1    0xFFFFFFFF
#define GDMA_INLINK_DSCR_CH1_M  ((GDMA_INLINK_DSCR_CH1_V)<<(GDMA_INLINK_DSCR_CH1_S))
#define GDMA_INLINK_DSCR_CH1_V  0xFFFFFFFF
#define GDMA_INLINK_DSCR_CH1_S  0

#define GDMA_IN_DSCR_BF0_CH1_REG          (DR_REG_GDMA_BASE + 0xF4)
/* GDMA_INLINK_DSCR_BF0_CH1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the last inlink descriptor x-1..*/
#define GDMA_INLINK_DSCR_BF0_CH1    0xFFFFFFFF
#define GDMA_INLINK_DSCR_BF0_CH1_M  ((GDMA_INLINK_DSCR_BF0_CH1_V)<<(GDMA_INLINK_DSCR_BF0_CH1_S))
#define GDMA_INLINK_DSCR_BF0_CH1_V  0xFFFFFFFF
#define GDMA_INLINK_DSCR_BF0_CH1_S  0

#define GDMA_IN_DSCR_BF1_CH1_REG          (DR_REG_GDMA_BASE + 0xF8)
/* GDMA_INLINK_DSCR_BF1_CH1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the second-to-last inlink descriptor x-2..*/
#define GDMA_INLINK_DSCR_BF1_CH1    0xFFFFFFFF
#define GDMA_INLINK_DSCR_BF1_CH1_M  ((GDMA_INLINK_DSCR_BF1_CH1_V)<<(GDMA_INLINK_DSCR_BF1_CH1_S))
#define GDMA_INLINK_DSCR_BF1_CH1_V  0xFFFFFFFF
#define GDMA_INLINK_DSCR_BF1_CH1_S  0

#define GDMA_IN_WEIGHT_CH1_REG          (DR_REG_GDMA_BASE + 0xFC)
/* GDMA_RX_WEIGHT_CH1 : R/W ;bitpos:[11:8] ;default: 4'hf ; */
/*description: The weight of Rx channel 1. .*/
#define GDMA_RX_WEIGHT_CH1    0x0000000F
#define GDMA_RX_WEIGHT_CH1_M  ((GDMA_RX_WEIGHT_CH1_V)<<(GDMA_RX_WEIGHT_CH1_S))
#define GDMA_RX_WEIGHT_CH1_V  0xF
#define GDMA_RX_WEIGHT_CH1_S  8

#define GDMA_IN_PRI_CH1_REG          (DR_REG_GDMA_BASE + 0x104)
/* GDMA_RX_PRI_CH1 : R/W ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: The priority of Rx channel 1. The larger of the value, the higher of the
priority..*/
#define GDMA_RX_PRI_CH1    0x0000000F
#define GDMA_RX_PRI_CH1_M  ((GDMA_RX_PRI_CH1_V)<<(GDMA_RX_PRI_CH1_S))
#define GDMA_RX_PRI_CH1_V  0xF
#define GDMA_RX_PRI_CH1_S  0

#define GDMA_IN_PERI_SEL_CH1_REG          (DR_REG_GDMA_BASE + 0x108)
/* GDMA_PERI_IN_SEL_CH1 : R/W ;bitpos:[5:0] ;default: 6'h3f ; */
/*description: This register is used to select peripheral for Rx channel 1. 0:SPI2. 1: SPI3. 2:
 UHCI0. 3: I2S0. 4: I2S1. 5: LCD_CAM. 6: AES. 7: SHA. 8: ADC_DAC. 9: RMT..*/
#define GDMA_PERI_IN_SEL_CH1    0x0000003F
#define GDMA_PERI_IN_SEL_CH1_M  ((GDMA_PERI_IN_SEL_CH1_V)<<(GDMA_PERI_IN_SEL_CH1_S))
#define GDMA_PERI_IN_SEL_CH1_V  0x3F
#define GDMA_PERI_IN_SEL_CH1_S  0

#define GDMA_OUT_CONF0_CH1_REG          (DR_REG_GDMA_BASE + 0x120)
/* GDMA_OUT_DATA_BURST_EN_CH1 : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Tx channel 1 transmitting
data when accessing internal SRAM. .*/
#define GDMA_OUT_DATA_BURST_EN_CH1    (BIT(5))
#define GDMA_OUT_DATA_BURST_EN_CH1_M  (BIT(5))
#define GDMA_OUT_DATA_BURST_EN_CH1_V  0x1
#define GDMA_OUT_DATA_BURST_EN_CH1_S  5
/* GDMA_OUTDSCR_BURST_EN_CH1 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Tx channel 1 reading link
descriptor when accessing internal SRAM. .*/
#define GDMA_OUTDSCR_BURST_EN_CH1    (BIT(4))
#define GDMA_OUTDSCR_BURST_EN_CH1_M  (BIT(4))
#define GDMA_OUTDSCR_BURST_EN_CH1_V  0x1
#define GDMA_OUTDSCR_BURST_EN_CH1_S  4
/* GDMA_OUT_EOF_MODE_CH1 : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel 0 is
 generated when data need to transmit has been popped from FIFO in DMA.*/
#define GDMA_OUT_EOF_MODE_CH1    (BIT(3))
#define GDMA_OUT_EOF_MODE_CH1_M  (BIT(3))
#define GDMA_OUT_EOF_MODE_CH1_V  0x1
#define GDMA_OUT_EOF_MODE_CH1_S  3
/* GDMA_OUT_AUTO_WRBACK_CH1 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to enable automatic outlink-writeback when all the data in tx
buffer has been transmitted..*/
#define GDMA_OUT_AUTO_WRBACK_CH1    (BIT(2))
#define GDMA_OUT_AUTO_WRBACK_CH1_M  (BIT(2))
#define GDMA_OUT_AUTO_WRBACK_CH1_V  0x1
#define GDMA_OUT_AUTO_WRBACK_CH1_S  2
/* GDMA_OUT_LOOP_TEST_CH1 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: reserved.*/
#define GDMA_OUT_LOOP_TEST_CH1    (BIT(1))
#define GDMA_OUT_LOOP_TEST_CH1_M  (BIT(1))
#define GDMA_OUT_LOOP_TEST_CH1_V  0x1
#define GDMA_OUT_LOOP_TEST_CH1_S  1
/* GDMA_OUT_RST_CH1 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: This bit is used to reset DMA channel 0 Tx FSM and Tx FIFO pointer..*/
#define GDMA_OUT_RST_CH1    (BIT(0))
#define GDMA_OUT_RST_CH1_M  (BIT(0))
#define GDMA_OUT_RST_CH1_V  0x1
#define GDMA_OUT_RST_CH1_S  0

#define GDMA_OUT_CONF1_CH1_REG          (DR_REG_GDMA_BASE + 0x124)
/* GDMA_OUT_EXT_MEM_BK_SIZE_CH1 : R/W ;bitpos:[14:13] ;default: 2'b0 ; */
/*description: Block size of Tx channel 1 when DMA access external SRAM. 0: 16 bytes      1: 32
 bytes    2/3:reserved.*/
#define GDMA_OUT_EXT_MEM_BK_SIZE_CH1    0x00000003
#define GDMA_OUT_EXT_MEM_BK_SIZE_CH1_M  ((GDMA_OUT_EXT_MEM_BK_SIZE_CH1_V)<<(GDMA_OUT_EXT_MEM_BK_SIZE_CH1_S))
#define GDMA_OUT_EXT_MEM_BK_SIZE_CH1_V  0x3
#define GDMA_OUT_EXT_MEM_BK_SIZE_CH1_S  13
/* GDMA_OUT_CHECK_OWNER_CH1 : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to enable checking the owner attribute of the link descriptor..*/
#define GDMA_OUT_CHECK_OWNER_CH1    (BIT(12))
#define GDMA_OUT_CHECK_OWNER_CH1_M  (BIT(12))
#define GDMA_OUT_CHECK_OWNER_CH1_V  0x1
#define GDMA_OUT_CHECK_OWNER_CH1_S  12

#define GDMA_OUT_INT_RAW_CH1_REG          (DR_REG_GDMA_BASE + 0x128)
/* GDMA_OUTFIFO_UDF_L3_CH1_INT_RAW : R/WTC/SS ;bitpos:[7] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo of Tx channel 1 is
underflow. .*/
#define GDMA_OUTFIFO_UDF_L3_CH1_INT_RAW    (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH1_INT_RAW_M  (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH1_INT_RAW_V  0x1
#define GDMA_OUTFIFO_UDF_L3_CH1_INT_RAW_S  7
/* GDMA_OUTFIFO_OVF_L3_CH1_INT_RAW : R/WTC/SS ;bitpos:[6] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo of Tx channel 1 is
overflow. .*/
#define GDMA_OUTFIFO_OVF_L3_CH1_INT_RAW    (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH1_INT_RAW_M  (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH1_INT_RAW_V  0x1
#define GDMA_OUTFIFO_OVF_L3_CH1_INT_RAW_S  6
/* GDMA_OUTFIFO_UDF_L1_CH1_INT_RAW : R/WTC/SS ;bitpos:[5] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo of Tx channel 1 is
underflow. .*/
#define GDMA_OUTFIFO_UDF_L1_CH1_INT_RAW    (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH1_INT_RAW_M  (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH1_INT_RAW_V  0x1
#define GDMA_OUTFIFO_UDF_L1_CH1_INT_RAW_S  5
/* GDMA_OUTFIFO_OVF_L1_CH1_INT_RAW : R/WTC/SS ;bitpos:[4] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo of Tx channel 1 is
overflow. .*/
#define GDMA_OUTFIFO_OVF_L1_CH1_INT_RAW    (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH1_INT_RAW_M  (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH1_INT_RAW_V  0x1
#define GDMA_OUTFIFO_OVF_L1_CH1_INT_RAW_S  4
/* GDMA_OUT_TOTAL_EOF_CH1_INT_RAW : R/WTC/SS ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when data corresponding a outlink '
(includes one link descriptor or few link descriptors) is transmitted out for Tx cha
nnel 1..*/
#define GDMA_OUT_TOTAL_EOF_CH1_INT_RAW    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH1_INT_RAW_M  (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH1_INT_RAW_V  0x1
#define GDMA_OUT_TOTAL_EOF_CH1_INT_RAW_S  3
/* GDMA_OUT_DSCR_ERR_CH1_INT_RAW : R/WTC/SS ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when detecting outlink descriptor
error, including owner error, the second and third word error of outlink descriptor
for Tx channel 1..*/
#define GDMA_OUT_DSCR_ERR_CH1_INT_RAW    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH1_INT_RAW_M  (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH1_INT_RAW_V  0x1
#define GDMA_OUT_DSCR_ERR_CH1_INT_RAW_S  2
/* GDMA_OUT_EOF_CH1_INT_RAW : R/WTC/SS ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data pointed by one
outlink descriptor has been read from memory for Tx channel 1. .*/
#define GDMA_OUT_EOF_CH1_INT_RAW    (BIT(1))
#define GDMA_OUT_EOF_CH1_INT_RAW_M  (BIT(1))
#define GDMA_OUT_EOF_CH1_INT_RAW_V  0x1
#define GDMA_OUT_EOF_CH1_INT_RAW_S  1
/* GDMA_OUT_DONE_CH1_INT_RAW : R/WTC/SS ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data pointed by one
ouink descriptor has been transmitted to peripherals for Tx channel 1..*/
#define GDMA_OUT_DONE_CH1_INT_RAW    (BIT(0))
#define GDMA_OUT_DONE_CH1_INT_RAW_M  (BIT(0))
#define GDMA_OUT_DONE_CH1_INT_RAW_V  0x1
#define GDMA_OUT_DONE_CH1_INT_RAW_S  0

#define GDMA_OUT_INT_ST_CH1_REG          (DR_REG_GDMA_BASE + 0x12C)
/* GDMA_OUTFIFO_UDF_L3_CH1_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L3_CH1_INT_ST    (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH1_INT_ST_M  (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH1_INT_ST_V  0x1
#define GDMA_OUTFIFO_UDF_L3_CH1_INT_ST_S  7
/* GDMA_OUTFIFO_OVF_L3_CH1_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L3_CH1_INT_ST    (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH1_INT_ST_M  (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH1_INT_ST_V  0x1
#define GDMA_OUTFIFO_OVF_L3_CH1_INT_ST_S  6
/* GDMA_OUTFIFO_UDF_L1_CH1_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L1_CH1_INT_ST    (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH1_INT_ST_M  (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH1_INT_ST_V  0x1
#define GDMA_OUTFIFO_UDF_L1_CH1_INT_ST_S  5
/* GDMA_OUTFIFO_OVF_L1_CH1_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L1_CH1_INT_ST    (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH1_INT_ST_M  (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH1_INT_ST_V  0x1
#define GDMA_OUTFIFO_OVF_L1_CH1_INT_ST_S  4
/* GDMA_OUT_TOTAL_EOF_CH1_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt..*/
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ST    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ST_M  (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ST_V  0x1
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ST_S  3
/* GDMA_OUT_DSCR_ERR_CH1_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_OUT_DSCR_ERR_CH1_INT_ST    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH1_INT_ST_M  (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH1_INT_ST_V  0x1
#define GDMA_OUT_DSCR_ERR_CH1_INT_ST_S  2
/* GDMA_OUT_EOF_CH1_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_EOF_CH_INT interrupt..*/
#define GDMA_OUT_EOF_CH1_INT_ST    (BIT(1))
#define GDMA_OUT_EOF_CH1_INT_ST_M  (BIT(1))
#define GDMA_OUT_EOF_CH1_INT_ST_V  0x1
#define GDMA_OUT_EOF_CH1_INT_ST_S  1
/* GDMA_OUT_DONE_CH1_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_DONE_CH_INT interrupt..*/
#define GDMA_OUT_DONE_CH1_INT_ST    (BIT(0))
#define GDMA_OUT_DONE_CH1_INT_ST_M  (BIT(0))
#define GDMA_OUT_DONE_CH1_INT_ST_V  0x1
#define GDMA_OUT_DONE_CH1_INT_ST_S  0

#define GDMA_OUT_INT_ENA_CH1_REG          (DR_REG_GDMA_BASE + 0x130)
/* GDMA_OUTFIFO_UDF_L3_CH1_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L3_CH1_INT_ENA    (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH1_INT_ENA_M  (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH1_INT_ENA_V  0x1
#define GDMA_OUTFIFO_UDF_L3_CH1_INT_ENA_S  7
/* GDMA_OUTFIFO_OVF_L3_CH1_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L3_CH1_INT_ENA    (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH1_INT_ENA_M  (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH1_INT_ENA_V  0x1
#define GDMA_OUTFIFO_OVF_L3_CH1_INT_ENA_S  6
/* GDMA_OUTFIFO_UDF_L1_CH1_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L1_CH1_INT_ENA    (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH1_INT_ENA_M  (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH1_INT_ENA_V  0x1
#define GDMA_OUTFIFO_UDF_L1_CH1_INT_ENA_S  5
/* GDMA_OUTFIFO_OVF_L1_CH1_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L1_CH1_INT_ENA    (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH1_INT_ENA_M  (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH1_INT_ENA_V  0x1
#define GDMA_OUTFIFO_OVF_L1_CH1_INT_ENA_S  4
/* GDMA_OUT_TOTAL_EOF_CH1_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt..*/
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ENA    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ENA_M  (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ENA_V  0x1
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ENA_S  3
/* GDMA_OUT_DSCR_ERR_CH1_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_OUT_DSCR_ERR_CH1_INT_ENA    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH1_INT_ENA_M  (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH1_INT_ENA_V  0x1
#define GDMA_OUT_DSCR_ERR_CH1_INT_ENA_S  2
/* GDMA_OUT_EOF_CH1_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_EOF_CH_INT interrupt..*/
#define GDMA_OUT_EOF_CH1_INT_ENA    (BIT(1))
#define GDMA_OUT_EOF_CH1_INT_ENA_M  (BIT(1))
#define GDMA_OUT_EOF_CH1_INT_ENA_V  0x1
#define GDMA_OUT_EOF_CH1_INT_ENA_S  1
/* GDMA_OUT_DONE_CH1_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_DONE_CH_INT interrupt..*/
#define GDMA_OUT_DONE_CH1_INT_ENA    (BIT(0))
#define GDMA_OUT_DONE_CH1_INT_ENA_M  (BIT(0))
#define GDMA_OUT_DONE_CH1_INT_ENA_V  0x1
#define GDMA_OUT_DONE_CH1_INT_ENA_S  0

#define GDMA_OUT_INT_CLR_CH1_REG          (DR_REG_GDMA_BASE + 0x134)
/* GDMA_OUTFIFO_UDF_L3_CH1_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L3_CH1_INT_CLR    (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH1_INT_CLR_M  (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH1_INT_CLR_V  0x1
#define GDMA_OUTFIFO_UDF_L3_CH1_INT_CLR_S  7
/* GDMA_OUTFIFO_OVF_L3_CH1_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L3_CH1_INT_CLR    (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH1_INT_CLR_M  (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH1_INT_CLR_V  0x1
#define GDMA_OUTFIFO_OVF_L3_CH1_INT_CLR_S  6
/* GDMA_OUTFIFO_UDF_L1_CH1_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L1_CH1_INT_CLR    (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH1_INT_CLR_M  (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH1_INT_CLR_V  0x1
#define GDMA_OUTFIFO_UDF_L1_CH1_INT_CLR_S  5
/* GDMA_OUTFIFO_OVF_L1_CH1_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L1_CH1_INT_CLR    (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH1_INT_CLR_M  (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH1_INT_CLR_V  0x1
#define GDMA_OUTFIFO_OVF_L1_CH1_INT_CLR_S  4
/* GDMA_OUT_TOTAL_EOF_CH1_INT_CLR : WT ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt..*/
#define GDMA_OUT_TOTAL_EOF_CH1_INT_CLR    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH1_INT_CLR_M  (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH1_INT_CLR_V  0x1
#define GDMA_OUT_TOTAL_EOF_CH1_INT_CLR_S  3
/* GDMA_OUT_DSCR_ERR_CH1_INT_CLR : WT ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_OUT_DSCR_ERR_CH1_INT_CLR    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH1_INT_CLR_M  (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH1_INT_CLR_V  0x1
#define GDMA_OUT_DSCR_ERR_CH1_INT_CLR_S  2
/* GDMA_OUT_EOF_CH1_INT_CLR : WT ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_EOF_CH_INT interrupt..*/
#define GDMA_OUT_EOF_CH1_INT_CLR    (BIT(1))
#define GDMA_OUT_EOF_CH1_INT_CLR_M  (BIT(1))
#define GDMA_OUT_EOF_CH1_INT_CLR_V  0x1
#define GDMA_OUT_EOF_CH1_INT_CLR_S  1
/* GDMA_OUT_DONE_CH1_INT_CLR : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_DONE_CH_INT interrupt..*/
#define GDMA_OUT_DONE_CH1_INT_CLR    (BIT(0))
#define GDMA_OUT_DONE_CH1_INT_CLR_M  (BIT(0))
#define GDMA_OUT_DONE_CH1_INT_CLR_V  0x1
#define GDMA_OUT_DONE_CH1_INT_CLR_S  0

#define GDMA_OUTFIFO_STATUS_CH1_REG          (DR_REG_GDMA_BASE + 0x138)
/* GDMA_OUT_REMAIN_UNDER_4B_L3_CH1 : RO ;bitpos:[26] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_OUT_REMAIN_UNDER_4B_L3_CH1    (BIT(26))
#define GDMA_OUT_REMAIN_UNDER_4B_L3_CH1_M  (BIT(26))
#define GDMA_OUT_REMAIN_UNDER_4B_L3_CH1_V  0x1
#define GDMA_OUT_REMAIN_UNDER_4B_L3_CH1_S  26
/* GDMA_OUT_REMAIN_UNDER_3B_L3_CH1 : RO ;bitpos:[25] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_OUT_REMAIN_UNDER_3B_L3_CH1    (BIT(25))
#define GDMA_OUT_REMAIN_UNDER_3B_L3_CH1_M  (BIT(25))
#define GDMA_OUT_REMAIN_UNDER_3B_L3_CH1_V  0x1
#define GDMA_OUT_REMAIN_UNDER_3B_L3_CH1_S  25
/* GDMA_OUT_REMAIN_UNDER_2B_L3_CH1 : RO ;bitpos:[24] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_OUT_REMAIN_UNDER_2B_L3_CH1    (BIT(24))
#define GDMA_OUT_REMAIN_UNDER_2B_L3_CH1_M  (BIT(24))
#define GDMA_OUT_REMAIN_UNDER_2B_L3_CH1_V  0x1
#define GDMA_OUT_REMAIN_UNDER_2B_L3_CH1_S  24
/* GDMA_OUT_REMAIN_UNDER_1B_L3_CH1 : RO ;bitpos:[23] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_OUT_REMAIN_UNDER_1B_L3_CH1    (BIT(23))
#define GDMA_OUT_REMAIN_UNDER_1B_L3_CH1_M  (BIT(23))
#define GDMA_OUT_REMAIN_UNDER_1B_L3_CH1_V  0x1
#define GDMA_OUT_REMAIN_UNDER_1B_L3_CH1_S  23
/* GDMA_OUTFIFO_CNT_L3_CH1 : RO ;bitpos:[22:18] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L3 Tx FIFO for Tx channel 1..*/
#define GDMA_OUTFIFO_CNT_L3_CH1    0x0000001F
#define GDMA_OUTFIFO_CNT_L3_CH1_M  ((GDMA_OUTFIFO_CNT_L3_CH1_V)<<(GDMA_OUTFIFO_CNT_L3_CH1_S))
#define GDMA_OUTFIFO_CNT_L3_CH1_V  0x1F
#define GDMA_OUTFIFO_CNT_L3_CH1_S  18
/* GDMA_OUTFIFO_CNT_L2_CH1 : RO ;bitpos:[17:11] ;default: 7'b0 ; */
/*description: The register stores the byte number of the data in L2 Tx FIFO for Tx channel 1..*/
#define GDMA_OUTFIFO_CNT_L2_CH1    0x0000007F
#define GDMA_OUTFIFO_CNT_L2_CH1_M  ((GDMA_OUTFIFO_CNT_L2_CH1_V)<<(GDMA_OUTFIFO_CNT_L2_CH1_S))
#define GDMA_OUTFIFO_CNT_L2_CH1_V  0x7F
#define GDMA_OUTFIFO_CNT_L2_CH1_S  11
/* GDMA_OUTFIFO_CNT_L1_CH1 : RO ;bitpos:[10:6] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L1 Tx FIFO for Tx channel 1..*/
#define GDMA_OUTFIFO_CNT_L1_CH1    0x0000001F
#define GDMA_OUTFIFO_CNT_L1_CH1_M  ((GDMA_OUTFIFO_CNT_L1_CH1_V)<<(GDMA_OUTFIFO_CNT_L1_CH1_S))
#define GDMA_OUTFIFO_CNT_L1_CH1_V  0x1F
#define GDMA_OUTFIFO_CNT_L1_CH1_S  6
/* GDMA_OUTFIFO_EMPTY_L3_CH1 : RO ;bitpos:[5] ;default: 1'b1 ; */
/*description: L3 Tx FIFO empty signal for Tx channel 1..*/
#define GDMA_OUTFIFO_EMPTY_L3_CH1    (BIT(5))
#define GDMA_OUTFIFO_EMPTY_L3_CH1_M  (BIT(5))
#define GDMA_OUTFIFO_EMPTY_L3_CH1_V  0x1
#define GDMA_OUTFIFO_EMPTY_L3_CH1_S  5
/* GDMA_OUTFIFO_FULL_L3_CH1 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: L3 Tx FIFO full signal for Tx channel 1..*/
#define GDMA_OUTFIFO_FULL_L3_CH1    (BIT(4))
#define GDMA_OUTFIFO_FULL_L3_CH1_M  (BIT(4))
#define GDMA_OUTFIFO_FULL_L3_CH1_V  0x1
#define GDMA_OUTFIFO_FULL_L3_CH1_S  4
/* GDMA_OUTFIFO_EMPTY_L2_CH1 : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: L2 Tx FIFO empty signal for Tx channel 1..*/
#define GDMA_OUTFIFO_EMPTY_L2_CH1    (BIT(3))
#define GDMA_OUTFIFO_EMPTY_L2_CH1_M  (BIT(3))
#define GDMA_OUTFIFO_EMPTY_L2_CH1_V  0x1
#define GDMA_OUTFIFO_EMPTY_L2_CH1_S  3
/* GDMA_OUTFIFO_FULL_L2_CH1 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: L2 Tx FIFO full signal for Tx channel 1..*/
#define GDMA_OUTFIFO_FULL_L2_CH1    (BIT(2))
#define GDMA_OUTFIFO_FULL_L2_CH1_M  (BIT(2))
#define GDMA_OUTFIFO_FULL_L2_CH1_V  0x1
#define GDMA_OUTFIFO_FULL_L2_CH1_S  2
/* GDMA_OUTFIFO_EMPTY_L1_CH1 : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: L1 Tx FIFO empty signal for Tx channel 1..*/
#define GDMA_OUTFIFO_EMPTY_L1_CH1    (BIT(1))
#define GDMA_OUTFIFO_EMPTY_L1_CH1_M  (BIT(1))
#define GDMA_OUTFIFO_EMPTY_L1_CH1_V  0x1
#define GDMA_OUTFIFO_EMPTY_L1_CH1_S  1
/* GDMA_OUTFIFO_FULL_L1_CH1 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: L1 Tx FIFO full signal for Tx channel 1..*/
#define GDMA_OUTFIFO_FULL_L1_CH1    (BIT(0))
#define GDMA_OUTFIFO_FULL_L1_CH1_M  (BIT(0))
#define GDMA_OUTFIFO_FULL_L1_CH1_V  0x1
#define GDMA_OUTFIFO_FULL_L1_CH1_S  0

#define GDMA_OUT_PUSH_CH1_REG          (DR_REG_GDMA_BASE + 0x13C)
/* GDMA_OUTFIFO_PUSH_CH1 : R/W/SC ;bitpos:[9] ;default: 1'h0 ; */
/*description: Set this bit to push data into DMA FIFO..*/
#define GDMA_OUTFIFO_PUSH_CH1    (BIT(9))
#define GDMA_OUTFIFO_PUSH_CH1_M  (BIT(9))
#define GDMA_OUTFIFO_PUSH_CH1_V  0x1
#define GDMA_OUTFIFO_PUSH_CH1_S  9
/* GDMA_OUTFIFO_WDATA_CH1 : R/W ;bitpos:[8:0] ;default: 9'h0 ; */
/*description: This register stores the data that need to be pushed into DMA FIFO..*/
#define GDMA_OUTFIFO_WDATA_CH1    0x000001FF
#define GDMA_OUTFIFO_WDATA_CH1_M  ((GDMA_OUTFIFO_WDATA_CH1_V)<<(GDMA_OUTFIFO_WDATA_CH1_S))
#define GDMA_OUTFIFO_WDATA_CH1_V  0x1FF
#define GDMA_OUTFIFO_WDATA_CH1_S  0

#define GDMA_OUT_LINK_CH1_REG          (DR_REG_GDMA_BASE + 0x140)
/* GDMA_OUTLINK_PARK_CH1 : RO ;bitpos:[23] ;default: 1'h1 ; */
/*description: 1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's
FSM is working..*/
#define GDMA_OUTLINK_PARK_CH1    (BIT(23))
#define GDMA_OUTLINK_PARK_CH1_M  (BIT(23))
#define GDMA_OUTLINK_PARK_CH1_V  0x1
#define GDMA_OUTLINK_PARK_CH1_S  23
/* GDMA_OUTLINK_RESTART_CH1 : R/W/SC ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to restart a new outlink from the last address. .*/
#define GDMA_OUTLINK_RESTART_CH1    (BIT(22))
#define GDMA_OUTLINK_RESTART_CH1_M  (BIT(22))
#define GDMA_OUTLINK_RESTART_CH1_V  0x1
#define GDMA_OUTLINK_RESTART_CH1_S  22
/* GDMA_OUTLINK_START_CH1 : R/W/SC ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to start dealing with the outlink descriptors..*/
#define GDMA_OUTLINK_START_CH1    (BIT(21))
#define GDMA_OUTLINK_START_CH1_M  (BIT(21))
#define GDMA_OUTLINK_START_CH1_V  0x1
#define GDMA_OUTLINK_START_CH1_S  21
/* GDMA_OUTLINK_STOP_CH1 : R/W/SC ;bitpos:[20] ;default: 1'b0 ; */
/*description: Set this bit to stop dealing with the outlink descriptors..*/
#define GDMA_OUTLINK_STOP_CH1    (BIT(20))
#define GDMA_OUTLINK_STOP_CH1_M  (BIT(20))
#define GDMA_OUTLINK_STOP_CH1_V  0x1
#define GDMA_OUTLINK_STOP_CH1_S  20
/* GDMA_OUTLINK_ADDR_CH1 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: This register stores the 20 least significant bits of the first outlink
descriptor's address..*/
#define GDMA_OUTLINK_ADDR_CH1    0x000FFFFF
#define GDMA_OUTLINK_ADDR_CH1_M  ((GDMA_OUTLINK_ADDR_CH1_V)<<(GDMA_OUTLINK_ADDR_CH1_S))
#define GDMA_OUTLINK_ADDR_CH1_V  0xFFFFF
#define GDMA_OUTLINK_ADDR_CH1_S  0

#define GDMA_OUT_STATE_CH1_REG          (DR_REG_GDMA_BASE + 0x144)
/* GDMA_OUT_STATE_CH1 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: reserved.*/
#define GDMA_OUT_STATE_CH1    0x00000007
#define GDMA_OUT_STATE_CH1_M  ((GDMA_OUT_STATE_CH1_V)<<(GDMA_OUT_STATE_CH1_S))
#define GDMA_OUT_STATE_CH1_V  0x7
#define GDMA_OUT_STATE_CH1_S  20
/* GDMA_OUT_DSCR_STATE_CH1 : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: reserved.*/
#define GDMA_OUT_DSCR_STATE_CH1    0x00000003
#define GDMA_OUT_DSCR_STATE_CH1_M  ((GDMA_OUT_DSCR_STATE_CH1_V)<<(GDMA_OUT_DSCR_STATE_CH1_S))
#define GDMA_OUT_DSCR_STATE_CH1_V  0x3
#define GDMA_OUT_DSCR_STATE_CH1_S  18
/* GDMA_OUTLINK_DSCR_ADDR_CH1 : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: This register stores the current outlink descriptor's address..*/
#define GDMA_OUTLINK_DSCR_ADDR_CH1    0x0003FFFF
#define GDMA_OUTLINK_DSCR_ADDR_CH1_M  ((GDMA_OUTLINK_DSCR_ADDR_CH1_V)<<(GDMA_OUTLINK_DSCR_ADDR_CH1_S))
#define GDMA_OUTLINK_DSCR_ADDR_CH1_V  0x3FFFF
#define GDMA_OUTLINK_DSCR_ADDR_CH1_S  0

#define GDMA_OUT_EOF_DES_ADDR_CH1_REG          (DR_REG_GDMA_BASE + 0x148)
/* GDMA_OUT_EOF_DES_ADDR_CH1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the outlink descriptor when the EOF bit in
this descriptor is 1..*/
#define GDMA_OUT_EOF_DES_ADDR_CH1    0xFFFFFFFF
#define GDMA_OUT_EOF_DES_ADDR_CH1_M  ((GDMA_OUT_EOF_DES_ADDR_CH1_V)<<(GDMA_OUT_EOF_DES_ADDR_CH1_S))
#define GDMA_OUT_EOF_DES_ADDR_CH1_V  0xFFFFFFFF
#define GDMA_OUT_EOF_DES_ADDR_CH1_S  0

#define GDMA_OUT_EOF_BFR_DES_ADDR_CH1_REG          (DR_REG_GDMA_BASE + 0x14C)
/* GDMA_OUT_EOF_BFR_DES_ADDR_CH1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the outlink descriptor before the last
outlink descriptor..*/
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH1    0xFFFFFFFF
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH1_M  ((GDMA_OUT_EOF_BFR_DES_ADDR_CH1_V)<<(GDMA_OUT_EOF_BFR_DES_ADDR_CH1_S))
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH1_V  0xFFFFFFFF
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH1_S  0

#define GDMA_OUT_DSCR_CH1_REG          (DR_REG_GDMA_BASE + 0x150)
/* GDMA_OUTLINK_DSCR_CH1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the current outlink descriptor y..*/
#define GDMA_OUTLINK_DSCR_CH1    0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_CH1_M  ((GDMA_OUTLINK_DSCR_CH1_V)<<(GDMA_OUTLINK_DSCR_CH1_S))
#define GDMA_OUTLINK_DSCR_CH1_V  0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_CH1_S  0

#define GDMA_OUT_DSCR_BF0_CH1_REG          (DR_REG_GDMA_BASE + 0x154)
/* GDMA_OUTLINK_DSCR_BF0_CH1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the last outlink descriptor y-1..*/
#define GDMA_OUTLINK_DSCR_BF0_CH1    0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_BF0_CH1_M  ((GDMA_OUTLINK_DSCR_BF0_CH1_V)<<(GDMA_OUTLINK_DSCR_BF0_CH1_S))
#define GDMA_OUTLINK_DSCR_BF0_CH1_V  0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_BF0_CH1_S  0

#define GDMA_OUT_DSCR_BF1_CH1_REG          (DR_REG_GDMA_BASE + 0x158)
/* GDMA_OUTLINK_DSCR_BF1_CH1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the second-to-last inlink descriptor x-2..*/
#define GDMA_OUTLINK_DSCR_BF1_CH1    0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_BF1_CH1_M  ((GDMA_OUTLINK_DSCR_BF1_CH1_V)<<(GDMA_OUTLINK_DSCR_BF1_CH1_S))
#define GDMA_OUTLINK_DSCR_BF1_CH1_V  0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_BF1_CH1_S  0

#define GDMA_OUT_WEIGHT_CH1_REG          (DR_REG_GDMA_BASE + 0x15C)
/* GDMA_TX_WEIGHT_CH1 : R/W ;bitpos:[11:8] ;default: 4'hf ; */
/*description: The weight of Tx channel 1. .*/
#define GDMA_TX_WEIGHT_CH1    0x0000000F
#define GDMA_TX_WEIGHT_CH1_M  ((GDMA_TX_WEIGHT_CH1_V)<<(GDMA_TX_WEIGHT_CH1_S))
#define GDMA_TX_WEIGHT_CH1_V  0xF
#define GDMA_TX_WEIGHT_CH1_S  8

#define GDMA_OUT_PRI_CH1_REG          (DR_REG_GDMA_BASE + 0x164)
/* GDMA_TX_PRI_CH1 : R/W ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: The priority of Tx channel 1. The larger of the value, the higher of the
priority..*/
#define GDMA_TX_PRI_CH1    0x0000000F
#define GDMA_TX_PRI_CH1_M  ((GDMA_TX_PRI_CH1_V)<<(GDMA_TX_PRI_CH1_S))
#define GDMA_TX_PRI_CH1_V  0xF
#define GDMA_TX_PRI_CH1_S  0

#define GDMA_OUT_PERI_SEL_CH1_REG          (DR_REG_GDMA_BASE + 0x168)
/* GDMA_PERI_OUT_SEL_CH1 : R/W ;bitpos:[5:0] ;default: 6'h3f ; */
/*description: This register is used to select peripheral for Tx channel 1. 0:SPI2. 1: SPI3. 2:
 UHCI0. 3: I2S0. 4: I2S1. 5: LCD_CAM. 6: AES. 7: SHA. 8: ADC_DAC. 9: RMT.; 7: AE
S. 8: SHA. 9: ADC_DAC..*/
#define GDMA_PERI_OUT_SEL_CH1    0x0000003F
#define GDMA_PERI_OUT_SEL_CH1_M  ((GDMA_PERI_OUT_SEL_CH1_V)<<(GDMA_PERI_OUT_SEL_CH1_S))
#define GDMA_PERI_OUT_SEL_CH1_V  0x3F
#define GDMA_PERI_OUT_SEL_CH1_S  0

#define GDMA_IN_CONF0_CH2_REG          (DR_REG_GDMA_BASE + 0x180)
/* GDMA_MEM_TRANS_EN_CH2 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit 1 to enable automatic transmitting data from memory to memory via
DMA..*/
#define GDMA_MEM_TRANS_EN_CH2    (BIT(4))
#define GDMA_MEM_TRANS_EN_CH2_M  (BIT(4))
#define GDMA_MEM_TRANS_EN_CH2_V  0x1
#define GDMA_MEM_TRANS_EN_CH2_S  4
/* GDMA_IN_DATA_BURST_EN_CH2 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Rx channel 2 receiving data
when accessing internal SRAM. .*/
#define GDMA_IN_DATA_BURST_EN_CH2    (BIT(3))
#define GDMA_IN_DATA_BURST_EN_CH2_M  (BIT(3))
#define GDMA_IN_DATA_BURST_EN_CH2_V  0x1
#define GDMA_IN_DATA_BURST_EN_CH2_S  3
/* GDMA_INDSCR_BURST_EN_CH2 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Rx channel 2 reading link
descriptor when accessing internal SRAM. .*/
#define GDMA_INDSCR_BURST_EN_CH2    (BIT(2))
#define GDMA_INDSCR_BURST_EN_CH2_M  (BIT(2))
#define GDMA_INDSCR_BURST_EN_CH2_V  0x1
#define GDMA_INDSCR_BURST_EN_CH2_S  2
/* GDMA_IN_LOOP_TEST_CH2 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: reserved.*/
#define GDMA_IN_LOOP_TEST_CH2    (BIT(1))
#define GDMA_IN_LOOP_TEST_CH2_M  (BIT(1))
#define GDMA_IN_LOOP_TEST_CH2_V  0x1
#define GDMA_IN_LOOP_TEST_CH2_S  1
/* GDMA_IN_RST_CH2 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: This bit is used to reset DMA channel 2 Rx FSM and Rx FIFO pointer..*/
#define GDMA_IN_RST_CH2    (BIT(0))
#define GDMA_IN_RST_CH2_M  (BIT(0))
#define GDMA_IN_RST_CH2_V  0x1
#define GDMA_IN_RST_CH2_S  0

#define GDMA_IN_CONF1_CH2_REG          (DR_REG_GDMA_BASE + 0x184)
/* GDMA_IN_EXT_MEM_BK_SIZE_CH2 : R/W ;bitpos:[14:13] ;default: 2'b0 ; */
/*description: Block size of Rx channel 2 when DMA access external SRAM. 0: 16 bytes      1: 32
 bytes    2/3:reserved.*/
#define GDMA_IN_EXT_MEM_BK_SIZE_CH2    0x00000003
#define GDMA_IN_EXT_MEM_BK_SIZE_CH2_M  ((GDMA_IN_EXT_MEM_BK_SIZE_CH2_V)<<(GDMA_IN_EXT_MEM_BK_SIZE_CH2_S))
#define GDMA_IN_EXT_MEM_BK_SIZE_CH2_V  0x3
#define GDMA_IN_EXT_MEM_BK_SIZE_CH2_S  13
/* GDMA_IN_CHECK_OWNER_CH2 : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to enable checking the owner attribute of the link descriptor..*/
#define GDMA_IN_CHECK_OWNER_CH2    (BIT(12))
#define GDMA_IN_CHECK_OWNER_CH2_M  (BIT(12))
#define GDMA_IN_CHECK_OWNER_CH2_V  0x1
#define GDMA_IN_CHECK_OWNER_CH2_S  12
/* GDMA_DMA_INFIFO_FULL_THRS_CH2 : R/W ;bitpos:[11:0] ;default: 12'hc ; */
/*description: This register is used to generate the INFIFO_FULL_WM_INT interrupt when Rx
channel 2 received byte number in Rx FIFO is up to the value of the register..*/
#define GDMA_DMA_INFIFO_FULL_THRS_CH2    0x00000FFF
#define GDMA_DMA_INFIFO_FULL_THRS_CH2_M  ((GDMA_DMA_INFIFO_FULL_THRS_CH2_V)<<(GDMA_DMA_INFIFO_FULL_THRS_CH2_S))
#define GDMA_DMA_INFIFO_FULL_THRS_CH2_V  0xFFF
#define GDMA_DMA_INFIFO_FULL_THRS_CH2_S  0

#define GDMA_IN_INT_RAW_CH2_REG          (DR_REG_GDMA_BASE + 0x188)
/* GDMA_INFIFO_UDF_L3_CH2_INT_RAW : R/WTC/SS ;bitpos:[9] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo of Rx channel 2 is
underflow. .*/
#define GDMA_INFIFO_UDF_L3_CH2_INT_RAW    (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH2_INT_RAW_M  (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH2_INT_RAW_V  0x1
#define GDMA_INFIFO_UDF_L3_CH2_INT_RAW_S  9
/* GDMA_INFIFO_OVF_L3_CH2_INT_RAW : R/WTC/SS ;bitpos:[8] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo of Rx channel 2 is
overflow. .*/
#define GDMA_INFIFO_OVF_L3_CH2_INT_RAW    (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH2_INT_RAW_M  (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH2_INT_RAW_V  0x1
#define GDMA_INFIFO_OVF_L3_CH2_INT_RAW_S  8
/* GDMA_INFIFO_UDF_L1_CH2_INT_RAW : R/WTC/SS ;bitpos:[7] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo of Rx channel 2 is
underflow. .*/
#define GDMA_INFIFO_UDF_L1_CH2_INT_RAW    (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH2_INT_RAW_M  (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH2_INT_RAW_V  0x1
#define GDMA_INFIFO_UDF_L1_CH2_INT_RAW_S  7
/* GDMA_INFIFO_OVF_L1_CH2_INT_RAW : R/WTC/SS ;bitpos:[6] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo of Rx channel 2 is
overflow. .*/
#define GDMA_INFIFO_OVF_L1_CH2_INT_RAW    (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH2_INT_RAW_M  (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH2_INT_RAW_V  0x1
#define GDMA_INFIFO_OVF_L1_CH2_INT_RAW_S  6
/* GDMA_INFIFO_FULL_WM_CH2_INT_RAW : R/WTC/SS ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when received data byte number is up
to threshold configured by REG_DMA_INFIFO_FULL_THRS_CH0 in Rx FIFO of channel 2..*/
#define GDMA_INFIFO_FULL_WM_CH2_INT_RAW    (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH2_INT_RAW_M  (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH2_INT_RAW_V  0x1
#define GDMA_INFIFO_FULL_WM_CH2_INT_RAW_S  5
/* GDMA_IN_DSCR_EMPTY_CH2_INT_RAW : R/WTC/SS ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when Rx buffer pointed by inlink is
full and receiving data is not completed, but there is no more inlink for Rx chann
el 2..*/
#define GDMA_IN_DSCR_EMPTY_CH2_INT_RAW    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH2_INT_RAW_M  (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH2_INT_RAW_V  0x1
#define GDMA_IN_DSCR_EMPTY_CH2_INT_RAW_S  4
/* GDMA_IN_DSCR_ERR_CH2_INT_RAW : R/WTC/SS ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when detecting inlink descriptor error
, including owner error, the second and third word error of inlink descriptor
for Rx channel 2..*/
#define GDMA_IN_DSCR_ERR_CH2_INT_RAW    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH2_INT_RAW_M  (BIT(3))
#define GDMA_IN_DSCR_ERR_CH2_INT_RAW_V  0x1
#define GDMA_IN_DSCR_ERR_CH2_INT_RAW_S  3
/* GDMA_IN_ERR_EOF_CH2_INT_RAW : R/WTC/SS ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when data error is detected only in
the case that the peripheral is UHCI0 for Rx channel 2. For other peripherals,
this raw interrupt is reserved..*/
#define GDMA_IN_ERR_EOF_CH2_INT_RAW    (BIT(2))
#define GDMA_IN_ERR_EOF_CH2_INT_RAW_M  (BIT(2))
#define GDMA_IN_ERR_EOF_CH2_INT_RAW_V  0x1
#define GDMA_IN_ERR_EOF_CH2_INT_RAW_S  2
/* GDMA_IN_SUC_EOF_CH2_INT_RAW : R/WTC/SS ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data pointed by one inli
nk descriptor has been received for Rx channel 2. For UHCI0, the raw interrupt b
it turns to high level when the last data pointed by one inlink descriptor has b
een received and no data error is detected for Rx channel 0..*/
#define GDMA_IN_SUC_EOF_CH2_INT_RAW    (BIT(1))
#define GDMA_IN_SUC_EOF_CH2_INT_RAW_M  (BIT(1))
#define GDMA_IN_SUC_EOF_CH2_INT_RAW_V  0x1
#define GDMA_IN_SUC_EOF_CH2_INT_RAW_S  1
/* GDMA_IN_DONE_CH2_INT_RAW : R/WTC/SS ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data pointed by one inli
nk descriptor has been received for Rx channel 2..*/
#define GDMA_IN_DONE_CH2_INT_RAW    (BIT(0))
#define GDMA_IN_DONE_CH2_INT_RAW_M  (BIT(0))
#define GDMA_IN_DONE_CH2_INT_RAW_V  0x1
#define GDMA_IN_DONE_CH2_INT_RAW_S  0

#define GDMA_IN_INT_ST_CH2_REG          (DR_REG_GDMA_BASE + 0x18C)
/* GDMA_INFIFO_UDF_L3_CH2_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L3_CH2_INT_ST    (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH2_INT_ST_M  (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH2_INT_ST_V  0x1
#define GDMA_INFIFO_UDF_L3_CH2_INT_ST_S  9
/* GDMA_INFIFO_OVF_L3_CH2_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L3_CH2_INT_ST    (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH2_INT_ST_M  (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH2_INT_ST_V  0x1
#define GDMA_INFIFO_OVF_L3_CH2_INT_ST_S  8
/* GDMA_INFIFO_UDF_L1_CH2_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L1_CH2_INT_ST    (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH2_INT_ST_M  (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH2_INT_ST_V  0x1
#define GDMA_INFIFO_UDF_L1_CH2_INT_ST_S  7
/* GDMA_INFIFO_OVF_L1_CH2_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L1_CH2_INT_ST    (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH2_INT_ST_M  (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH2_INT_ST_V  0x1
#define GDMA_INFIFO_OVF_L1_CH2_INT_ST_S  6
/* GDMA_INFIFO_FULL_WM_CH2_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_FULL_WM_CH_INT interrupt..*/
#define GDMA_INFIFO_FULL_WM_CH2_INT_ST    (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH2_INT_ST_M  (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH2_INT_ST_V  0x1
#define GDMA_INFIFO_FULL_WM_CH2_INT_ST_S  5
/* GDMA_IN_DSCR_EMPTY_CH2_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt..*/
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ST    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ST_M  (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ST_V  0x1
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ST_S  4
/* GDMA_IN_DSCR_ERR_CH2_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_IN_DSCR_ERR_CH2_INT_ST    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH2_INT_ST_M  (BIT(3))
#define GDMA_IN_DSCR_ERR_CH2_INT_ST_V  0x1
#define GDMA_IN_DSCR_ERR_CH2_INT_ST_S  3
/* GDMA_IN_ERR_EOF_CH2_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt..*/
#define GDMA_IN_ERR_EOF_CH2_INT_ST    (BIT(2))
#define GDMA_IN_ERR_EOF_CH2_INT_ST_M  (BIT(2))
#define GDMA_IN_ERR_EOF_CH2_INT_ST_V  0x1
#define GDMA_IN_ERR_EOF_CH2_INT_ST_S  2
/* GDMA_IN_SUC_EOF_CH2_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt..*/
#define GDMA_IN_SUC_EOF_CH2_INT_ST    (BIT(1))
#define GDMA_IN_SUC_EOF_CH2_INT_ST_M  (BIT(1))
#define GDMA_IN_SUC_EOF_CH2_INT_ST_V  0x1
#define GDMA_IN_SUC_EOF_CH2_INT_ST_S  1
/* GDMA_IN_DONE_CH2_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DONE_CH_INT interrupt..*/
#define GDMA_IN_DONE_CH2_INT_ST    (BIT(0))
#define GDMA_IN_DONE_CH2_INT_ST_M  (BIT(0))
#define GDMA_IN_DONE_CH2_INT_ST_V  0x1
#define GDMA_IN_DONE_CH2_INT_ST_S  0

#define GDMA_IN_INT_ENA_CH2_REG          (DR_REG_GDMA_BASE + 0x190)
/* GDMA_INFIFO_UDF_L3_CH2_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L3_CH2_INT_ENA    (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH2_INT_ENA_M  (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH2_INT_ENA_V  0x1
#define GDMA_INFIFO_UDF_L3_CH2_INT_ENA_S  9
/* GDMA_INFIFO_OVF_L3_CH2_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L3_CH2_INT_ENA    (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH2_INT_ENA_M  (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH2_INT_ENA_V  0x1
#define GDMA_INFIFO_OVF_L3_CH2_INT_ENA_S  8
/* GDMA_INFIFO_UDF_L1_CH2_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L1_CH2_INT_ENA    (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH2_INT_ENA_M  (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH2_INT_ENA_V  0x1
#define GDMA_INFIFO_UDF_L1_CH2_INT_ENA_S  7
/* GDMA_INFIFO_OVF_L1_CH2_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L1_CH2_INT_ENA    (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH2_INT_ENA_M  (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH2_INT_ENA_V  0x1
#define GDMA_INFIFO_OVF_L1_CH2_INT_ENA_S  6
/* GDMA_INFIFO_FULL_WM_CH2_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_FULL_WM_CH_INT interrupt..*/
#define GDMA_INFIFO_FULL_WM_CH2_INT_ENA    (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH2_INT_ENA_M  (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH2_INT_ENA_V  0x1
#define GDMA_INFIFO_FULL_WM_CH2_INT_ENA_S  5
/* GDMA_IN_DSCR_EMPTY_CH2_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt..*/
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ENA    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ENA_M  (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ENA_V  0x1
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ENA_S  4
/* GDMA_IN_DSCR_ERR_CH2_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_IN_DSCR_ERR_CH2_INT_ENA    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH2_INT_ENA_M  (BIT(3))
#define GDMA_IN_DSCR_ERR_CH2_INT_ENA_V  0x1
#define GDMA_IN_DSCR_ERR_CH2_INT_ENA_S  3
/* GDMA_IN_ERR_EOF_CH2_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt..*/
#define GDMA_IN_ERR_EOF_CH2_INT_ENA    (BIT(2))
#define GDMA_IN_ERR_EOF_CH2_INT_ENA_M  (BIT(2))
#define GDMA_IN_ERR_EOF_CH2_INT_ENA_V  0x1
#define GDMA_IN_ERR_EOF_CH2_INT_ENA_S  2
/* GDMA_IN_SUC_EOF_CH2_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt..*/
#define GDMA_IN_SUC_EOF_CH2_INT_ENA    (BIT(1))
#define GDMA_IN_SUC_EOF_CH2_INT_ENA_M  (BIT(1))
#define GDMA_IN_SUC_EOF_CH2_INT_ENA_V  0x1
#define GDMA_IN_SUC_EOF_CH2_INT_ENA_S  1
/* GDMA_IN_DONE_CH2_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DONE_CH_INT interrupt..*/
#define GDMA_IN_DONE_CH2_INT_ENA    (BIT(0))
#define GDMA_IN_DONE_CH2_INT_ENA_M  (BIT(0))
#define GDMA_IN_DONE_CH2_INT_ENA_V  0x1
#define GDMA_IN_DONE_CH2_INT_ENA_S  0

#define GDMA_IN_INT_CLR_CH2_REG          (DR_REG_GDMA_BASE + 0x194)
/* GDMA_INFIFO_UDF_L3_CH2_INT_CLR : WT ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L3_CH2_INT_CLR    (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH2_INT_CLR_M  (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH2_INT_CLR_V  0x1
#define GDMA_INFIFO_UDF_L3_CH2_INT_CLR_S  9
/* GDMA_INFIFO_OVF_L3_CH2_INT_CLR : WT ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L3_CH2_INT_CLR    (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH2_INT_CLR_M  (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH2_INT_CLR_V  0x1
#define GDMA_INFIFO_OVF_L3_CH2_INT_CLR_S  8
/* GDMA_INFIFO_UDF_L1_CH2_INT_CLR : WT ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L1_CH2_INT_CLR    (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH2_INT_CLR_M  (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH2_INT_CLR_V  0x1
#define GDMA_INFIFO_UDF_L1_CH2_INT_CLR_S  7
/* GDMA_INFIFO_OVF_L1_CH2_INT_CLR : WT ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L1_CH2_INT_CLR    (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH2_INT_CLR_M  (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH2_INT_CLR_V  0x1
#define GDMA_INFIFO_OVF_L1_CH2_INT_CLR_S  6
/* GDMA_DMA_INFIFO_FULL_WM_CH2_INT_CLR : WT ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_FULL_WM_CH_INT interrupt..*/
#define GDMA_DMA_INFIFO_FULL_WM_CH2_INT_CLR    (BIT(5))
#define GDMA_DMA_INFIFO_FULL_WM_CH2_INT_CLR_M  (BIT(5))
#define GDMA_DMA_INFIFO_FULL_WM_CH2_INT_CLR_V  0x1
#define GDMA_DMA_INFIFO_FULL_WM_CH2_INT_CLR_S  5
/* GDMA_IN_DSCR_EMPTY_CH2_INT_CLR : WT ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt..*/
#define GDMA_IN_DSCR_EMPTY_CH2_INT_CLR    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH2_INT_CLR_M  (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH2_INT_CLR_V  0x1
#define GDMA_IN_DSCR_EMPTY_CH2_INT_CLR_S  4
/* GDMA_IN_DSCR_ERR_CH2_INT_CLR : WT ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_IN_DSCR_ERR_CH2_INT_CLR    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH2_INT_CLR_M  (BIT(3))
#define GDMA_IN_DSCR_ERR_CH2_INT_CLR_V  0x1
#define GDMA_IN_DSCR_ERR_CH2_INT_CLR_S  3
/* GDMA_IN_ERR_EOF_CH2_INT_CLR : WT ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_ERR_EOF_CH_INT interrupt..*/
#define GDMA_IN_ERR_EOF_CH2_INT_CLR    (BIT(2))
#define GDMA_IN_ERR_EOF_CH2_INT_CLR_M  (BIT(2))
#define GDMA_IN_ERR_EOF_CH2_INT_CLR_V  0x1
#define GDMA_IN_ERR_EOF_CH2_INT_CLR_S  2
/* GDMA_IN_SUC_EOF_CH2_INT_CLR : WT ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_SUC_EOF_CH_INT interrupt..*/
#define GDMA_IN_SUC_EOF_CH2_INT_CLR    (BIT(1))
#define GDMA_IN_SUC_EOF_CH2_INT_CLR_M  (BIT(1))
#define GDMA_IN_SUC_EOF_CH2_INT_CLR_V  0x1
#define GDMA_IN_SUC_EOF_CH2_INT_CLR_S  1
/* GDMA_IN_DONE_CH2_INT_CLR : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DONE_CH_INT interrupt..*/
#define GDMA_IN_DONE_CH2_INT_CLR    (BIT(0))
#define GDMA_IN_DONE_CH2_INT_CLR_M  (BIT(0))
#define GDMA_IN_DONE_CH2_INT_CLR_V  0x1
#define GDMA_IN_DONE_CH2_INT_CLR_S  0

#define GDMA_INFIFO_STATUS_CH2_REG          (DR_REG_GDMA_BASE + 0x198)
/* GDMA_IN_BUF_HUNGRY_CH2 : RO ;bitpos:[28] ;default: 1'b0 ; */
/*description: reserved.*/
#define GDMA_IN_BUF_HUNGRY_CH2    (BIT(28))
#define GDMA_IN_BUF_HUNGRY_CH2_M  (BIT(28))
#define GDMA_IN_BUF_HUNGRY_CH2_V  0x1
#define GDMA_IN_BUF_HUNGRY_CH2_S  28
/* GDMA_IN_REMAIN_UNDER_4B_L3_CH2 : RO ;bitpos:[27] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_IN_REMAIN_UNDER_4B_L3_CH2    (BIT(27))
#define GDMA_IN_REMAIN_UNDER_4B_L3_CH2_M  (BIT(27))
#define GDMA_IN_REMAIN_UNDER_4B_L3_CH2_V  0x1
#define GDMA_IN_REMAIN_UNDER_4B_L3_CH2_S  27
/* GDMA_IN_REMAIN_UNDER_3B_L3_CH2 : RO ;bitpos:[26] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_IN_REMAIN_UNDER_3B_L3_CH2    (BIT(26))
#define GDMA_IN_REMAIN_UNDER_3B_L3_CH2_M  (BIT(26))
#define GDMA_IN_REMAIN_UNDER_3B_L3_CH2_V  0x1
#define GDMA_IN_REMAIN_UNDER_3B_L3_CH2_S  26
/* GDMA_IN_REMAIN_UNDER_2B_L3_CH2 : RO ;bitpos:[25] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_IN_REMAIN_UNDER_2B_L3_CH2    (BIT(25))
#define GDMA_IN_REMAIN_UNDER_2B_L3_CH2_M  (BIT(25))
#define GDMA_IN_REMAIN_UNDER_2B_L3_CH2_V  0x1
#define GDMA_IN_REMAIN_UNDER_2B_L3_CH2_S  25
/* GDMA_IN_REMAIN_UNDER_1B_L3_CH2 : RO ;bitpos:[24] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_IN_REMAIN_UNDER_1B_L3_CH2    (BIT(24))
#define GDMA_IN_REMAIN_UNDER_1B_L3_CH2_M  (BIT(24))
#define GDMA_IN_REMAIN_UNDER_1B_L3_CH2_V  0x1
#define GDMA_IN_REMAIN_UNDER_1B_L3_CH2_S  24
/* GDMA_INFIFO_CNT_L3_CH2 : RO ;bitpos:[23:19] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L3 Rx FIFO for Rx channel 2..*/
#define GDMA_INFIFO_CNT_L3_CH2    0x0000001F
#define GDMA_INFIFO_CNT_L3_CH2_M  ((GDMA_INFIFO_CNT_L3_CH2_V)<<(GDMA_INFIFO_CNT_L3_CH2_S))
#define GDMA_INFIFO_CNT_L3_CH2_V  0x1F
#define GDMA_INFIFO_CNT_L3_CH2_S  19
/* GDMA_INFIFO_CNT_L2_CH2 : RO ;bitpos:[18:12] ;default: 7'b0 ; */
/*description: The register stores the byte number of the data in L2 Rx FIFO for Rx channel 2..*/
#define GDMA_INFIFO_CNT_L2_CH2    0x0000007F
#define GDMA_INFIFO_CNT_L2_CH2_M  ((GDMA_INFIFO_CNT_L2_CH2_V)<<(GDMA_INFIFO_CNT_L2_CH2_S))
#define GDMA_INFIFO_CNT_L2_CH2_V  0x7F
#define GDMA_INFIFO_CNT_L2_CH2_S  12
/* GDMA_INFIFO_CNT_L1_CH2 : RO ;bitpos:[11:6] ;default: 6'b0 ; */
/*description: The register stores the byte number of the data in L1 Rx FIFO for Rx channel 2..*/
#define GDMA_INFIFO_CNT_L1_CH2    0x0000003F
#define GDMA_INFIFO_CNT_L1_CH2_M  ((GDMA_INFIFO_CNT_L1_CH2_V)<<(GDMA_INFIFO_CNT_L1_CH2_S))
#define GDMA_INFIFO_CNT_L1_CH2_V  0x3F
#define GDMA_INFIFO_CNT_L1_CH2_S  6
/* GDMA_INFIFO_EMPTY_L3_CH2 : RO ;bitpos:[5] ;default: 1'b1 ; */
/*description: L3 Rx FIFO empty signal for Rx channel 2..*/
#define GDMA_INFIFO_EMPTY_L3_CH2    (BIT(5))
#define GDMA_INFIFO_EMPTY_L3_CH2_M  (BIT(5))
#define GDMA_INFIFO_EMPTY_L3_CH2_V  0x1
#define GDMA_INFIFO_EMPTY_L3_CH2_S  5
/* GDMA_INFIFO_FULL_L3_CH2 : RO ;bitpos:[4] ;default: 1'b1 ; */
/*description: L3 Rx FIFO full signal for Rx channel 2..*/
#define GDMA_INFIFO_FULL_L3_CH2    (BIT(4))
#define GDMA_INFIFO_FULL_L3_CH2_M  (BIT(4))
#define GDMA_INFIFO_FULL_L3_CH2_V  0x1
#define GDMA_INFIFO_FULL_L3_CH2_S  4
/* GDMA_INFIFO_EMPTY_L2_CH2 : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: L2 Rx FIFO empty signal for Rx channel 2..*/
#define GDMA_INFIFO_EMPTY_L2_CH2    (BIT(3))
#define GDMA_INFIFO_EMPTY_L2_CH2_M  (BIT(3))
#define GDMA_INFIFO_EMPTY_L2_CH2_V  0x1
#define GDMA_INFIFO_EMPTY_L2_CH2_S  3
/* GDMA_INFIFO_FULL_L2_CH2 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: L2 Rx FIFO full signal for Rx channel 2..*/
#define GDMA_INFIFO_FULL_L2_CH2    (BIT(2))
#define GDMA_INFIFO_FULL_L2_CH2_M  (BIT(2))
#define GDMA_INFIFO_FULL_L2_CH2_V  0x1
#define GDMA_INFIFO_FULL_L2_CH2_S  2
/* GDMA_INFIFO_EMPTY_L1_CH2 : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: L1 Rx FIFO empty signal for Rx channel 2..*/
#define GDMA_INFIFO_EMPTY_L1_CH2    (BIT(1))
#define GDMA_INFIFO_EMPTY_L1_CH2_M  (BIT(1))
#define GDMA_INFIFO_EMPTY_L1_CH2_V  0x1
#define GDMA_INFIFO_EMPTY_L1_CH2_S  1
/* GDMA_INFIFO_FULL_L1_CH2 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: L1 Rx FIFO full signal for Rx channel 2..*/
#define GDMA_INFIFO_FULL_L1_CH2    (BIT(0))
#define GDMA_INFIFO_FULL_L1_CH2_M  (BIT(0))
#define GDMA_INFIFO_FULL_L1_CH2_V  0x1
#define GDMA_INFIFO_FULL_L1_CH2_S  0

#define GDMA_IN_POP_CH2_REG          (DR_REG_GDMA_BASE + 0x19C)
/* GDMA_INFIFO_POP_CH2 : R/W/SC ;bitpos:[12] ;default: 1'h0 ; */
/*description: Set this bit to pop data from DMA FIFO..*/
#define GDMA_INFIFO_POP_CH2    (BIT(12))
#define GDMA_INFIFO_POP_CH2_M  (BIT(12))
#define GDMA_INFIFO_POP_CH2_V  0x1
#define GDMA_INFIFO_POP_CH2_S  12
/* GDMA_INFIFO_RDATA_CH2 : RO ;bitpos:[11:0] ;default: 12'h800 ; */
/*description: This register stores the data popping from DMA FIFO..*/
#define GDMA_INFIFO_RDATA_CH2    0x00000FFF
#define GDMA_INFIFO_RDATA_CH2_M  ((GDMA_INFIFO_RDATA_CH2_V)<<(GDMA_INFIFO_RDATA_CH2_S))
#define GDMA_INFIFO_RDATA_CH2_V  0xFFF
#define GDMA_INFIFO_RDATA_CH2_S  0

#define GDMA_IN_LINK_CH2_REG          (DR_REG_GDMA_BASE + 0x1A0)
/* GDMA_INLINK_PARK_CH2 : RO ;bitpos:[24] ;default: 1'h1 ; */
/*description: 1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM
 is working..*/
#define GDMA_INLINK_PARK_CH2    (BIT(24))
#define GDMA_INLINK_PARK_CH2_M  (BIT(24))
#define GDMA_INLINK_PARK_CH2_V  0x1
#define GDMA_INLINK_PARK_CH2_S  24
/* GDMA_INLINK_RESTART_CH2 : R/W/SC ;bitpos:[23] ;default: 1'b0 ; */
/*description: Set this bit to mount a new inlink descriptor..*/
#define GDMA_INLINK_RESTART_CH2    (BIT(23))
#define GDMA_INLINK_RESTART_CH2_M  (BIT(23))
#define GDMA_INLINK_RESTART_CH2_V  0x1
#define GDMA_INLINK_RESTART_CH2_S  23
/* GDMA_INLINK_START_CH2 : R/W/SC ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to start dealing with the inlink descriptors..*/
#define GDMA_INLINK_START_CH2    (BIT(22))
#define GDMA_INLINK_START_CH2_M  (BIT(22))
#define GDMA_INLINK_START_CH2_V  0x1
#define GDMA_INLINK_START_CH2_S  22
/* GDMA_INLINK_STOP_CH2 : R/W/SC ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to stop dealing with the inlink descriptors..*/
#define GDMA_INLINK_STOP_CH2    (BIT(21))
#define GDMA_INLINK_STOP_CH2_M  (BIT(21))
#define GDMA_INLINK_STOP_CH2_V  0x1
#define GDMA_INLINK_STOP_CH2_S  21
/* GDMA_INLINK_AUTO_RET_CH2 : R/W ;bitpos:[20] ;default: 1'b1 ; */
/*description: Set this bit to return to current inlink descriptor's address, when there are
some errors in current receiving data..*/
#define GDMA_INLINK_AUTO_RET_CH2    (BIT(20))
#define GDMA_INLINK_AUTO_RET_CH2_M  (BIT(20))
#define GDMA_INLINK_AUTO_RET_CH2_V  0x1
#define GDMA_INLINK_AUTO_RET_CH2_S  20
/* GDMA_INLINK_ADDR_CH2 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: This register stores the 20 least significant bits of the first inlink descriptor
r's address..*/
#define GDMA_INLINK_ADDR_CH2    0x000FFFFF
#define GDMA_INLINK_ADDR_CH2_M  ((GDMA_INLINK_ADDR_CH2_V)<<(GDMA_INLINK_ADDR_CH2_S))
#define GDMA_INLINK_ADDR_CH2_V  0xFFFFF
#define GDMA_INLINK_ADDR_CH2_S  0

#define GDMA_IN_STATE_CH2_REG          (DR_REG_GDMA_BASE + 0x1A4)
/* GDMA_IN_STATE_CH2 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: reserved.*/
#define GDMA_IN_STATE_CH2    0x00000007
#define GDMA_IN_STATE_CH2_M  ((GDMA_IN_STATE_CH2_V)<<(GDMA_IN_STATE_CH2_S))
#define GDMA_IN_STATE_CH2_V  0x7
#define GDMA_IN_STATE_CH2_S  20
/* GDMA_IN_DSCR_STATE_CH2 : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: reserved.*/
#define GDMA_IN_DSCR_STATE_CH2    0x00000003
#define GDMA_IN_DSCR_STATE_CH2_M  ((GDMA_IN_DSCR_STATE_CH2_V)<<(GDMA_IN_DSCR_STATE_CH2_S))
#define GDMA_IN_DSCR_STATE_CH2_V  0x3
#define GDMA_IN_DSCR_STATE_CH2_S  18
/* GDMA_INLINK_DSCR_ADDR_CH2 : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: This register stores the current inlink descriptor's address..*/
#define GDMA_INLINK_DSCR_ADDR_CH2    0x0003FFFF
#define GDMA_INLINK_DSCR_ADDR_CH2_M  ((GDMA_INLINK_DSCR_ADDR_CH2_V)<<(GDMA_INLINK_DSCR_ADDR_CH2_S))
#define GDMA_INLINK_DSCR_ADDR_CH2_V  0x3FFFF
#define GDMA_INLINK_DSCR_ADDR_CH2_S  0

#define GDMA_IN_SUC_EOF_DES_ADDR_CH2_REG          (DR_REG_GDMA_BASE + 0x1A8)
/* GDMA_IN_SUC_EOF_DES_ADDR_CH2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the inlink descriptor when the EOF bit in
this descriptor is 1..*/
#define GDMA_IN_SUC_EOF_DES_ADDR_CH2    0xFFFFFFFF
#define GDMA_IN_SUC_EOF_DES_ADDR_CH2_M  ((GDMA_IN_SUC_EOF_DES_ADDR_CH2_V)<<(GDMA_IN_SUC_EOF_DES_ADDR_CH2_S))
#define GDMA_IN_SUC_EOF_DES_ADDR_CH2_V  0xFFFFFFFF
#define GDMA_IN_SUC_EOF_DES_ADDR_CH2_S  0

#define GDMA_IN_ERR_EOF_DES_ADDR_CH2_REG          (DR_REG_GDMA_BASE + 0x1AC)
/* GDMA_IN_ERR_EOF_DES_ADDR_CH2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the inlink descriptor when there are some
errors in current receiving data. Only used when peripheral is UHCI0..*/
#define GDMA_IN_ERR_EOF_DES_ADDR_CH2    0xFFFFFFFF
#define GDMA_IN_ERR_EOF_DES_ADDR_CH2_M  ((GDMA_IN_ERR_EOF_DES_ADDR_CH2_V)<<(GDMA_IN_ERR_EOF_DES_ADDR_CH2_S))
#define GDMA_IN_ERR_EOF_DES_ADDR_CH2_V  0xFFFFFFFF
#define GDMA_IN_ERR_EOF_DES_ADDR_CH2_S  0

#define GDMA_IN_DSCR_CH2_REG          (DR_REG_GDMA_BASE + 0x1B0)
/* GDMA_INLINK_DSCR_CH2 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the current inlink descriptor x..*/
#define GDMA_INLINK_DSCR_CH2    0xFFFFFFFF
#define GDMA_INLINK_DSCR_CH2_M  ((GDMA_INLINK_DSCR_CH2_V)<<(GDMA_INLINK_DSCR_CH2_S))
#define GDMA_INLINK_DSCR_CH2_V  0xFFFFFFFF
#define GDMA_INLINK_DSCR_CH2_S  0

#define GDMA_IN_DSCR_BF0_CH2_REG          (DR_REG_GDMA_BASE + 0x1B4)
/* GDMA_INLINK_DSCR_BF0_CH2 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the last inlink descriptor x-1..*/
#define GDMA_INLINK_DSCR_BF0_CH2    0xFFFFFFFF
#define GDMA_INLINK_DSCR_BF0_CH2_M  ((GDMA_INLINK_DSCR_BF0_CH2_V)<<(GDMA_INLINK_DSCR_BF0_CH2_S))
#define GDMA_INLINK_DSCR_BF0_CH2_V  0xFFFFFFFF
#define GDMA_INLINK_DSCR_BF0_CH2_S  0

#define GDMA_IN_DSCR_BF1_CH2_REG          (DR_REG_GDMA_BASE + 0x1B8)
/* GDMA_INLINK_DSCR_BF1_CH2 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the second-to-last inlink descriptor x-2..*/
#define GDMA_INLINK_DSCR_BF1_CH2    0xFFFFFFFF
#define GDMA_INLINK_DSCR_BF1_CH2_M  ((GDMA_INLINK_DSCR_BF1_CH2_V)<<(GDMA_INLINK_DSCR_BF1_CH2_S))
#define GDMA_INLINK_DSCR_BF1_CH2_V  0xFFFFFFFF
#define GDMA_INLINK_DSCR_BF1_CH2_S  0

#define GDMA_IN_WEIGHT_CH2_REG          (DR_REG_GDMA_BASE + 0x1BC)
/* GDMA_RX_WEIGHT_CH2 : R/W ;bitpos:[11:8] ;default: 4'hf ; */
/*description: The weight of Rx channel 2. .*/
#define GDMA_RX_WEIGHT_CH2    0x0000000F
#define GDMA_RX_WEIGHT_CH2_M  ((GDMA_RX_WEIGHT_CH2_V)<<(GDMA_RX_WEIGHT_CH2_S))
#define GDMA_RX_WEIGHT_CH2_V  0xF
#define GDMA_RX_WEIGHT_CH2_S  8

#define GDMA_IN_PRI_CH2_REG          (DR_REG_GDMA_BASE + 0x1C4)
/* GDMA_RX_PRI_CH2 : R/W ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: The priority of Rx channel 2. The larger of the value, the higher of the
priority..*/
#define GDMA_RX_PRI_CH2    0x0000000F
#define GDMA_RX_PRI_CH2_M  ((GDMA_RX_PRI_CH2_V)<<(GDMA_RX_PRI_CH2_S))
#define GDMA_RX_PRI_CH2_V  0xF
#define GDMA_RX_PRI_CH2_S  0

#define GDMA_IN_PERI_SEL_CH2_REG          (DR_REG_GDMA_BASE + 0x1C8)
/* GDMA_PERI_IN_SEL_CH2 : R/W ;bitpos:[5:0] ;default: 6'h3f ; */
/*description: This register is used to select peripheral for Rx channel 2. 0:SPI2. 1: SPI3. 2:
 UHCI0. 3: I2S0. 4: I2S1. 5: LCD_CAM. 6: AES. 7: SHA. 8: ADC_DAC. 9: RMT..*/
#define GDMA_PERI_IN_SEL_CH2    0x0000003F
#define GDMA_PERI_IN_SEL_CH2_M  ((GDMA_PERI_IN_SEL_CH2_V)<<(GDMA_PERI_IN_SEL_CH2_S))
#define GDMA_PERI_IN_SEL_CH2_V  0x3F
#define GDMA_PERI_IN_SEL_CH2_S  0

#define GDMA_OUT_CONF0_CH2_REG          (DR_REG_GDMA_BASE + 0x1E0)
/* GDMA_OUT_DATA_BURST_EN_CH2 : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Tx channel 2 transmitting
data when accessing internal SRAM. .*/
#define GDMA_OUT_DATA_BURST_EN_CH2    (BIT(5))
#define GDMA_OUT_DATA_BURST_EN_CH2_M  (BIT(5))
#define GDMA_OUT_DATA_BURST_EN_CH2_V  0x1
#define GDMA_OUT_DATA_BURST_EN_CH2_S  5
/* GDMA_OUTDSCR_BURST_EN_CH2 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Tx channel 2 reading link
descriptor when accessing internal SRAM. .*/
#define GDMA_OUTDSCR_BURST_EN_CH2    (BIT(4))
#define GDMA_OUTDSCR_BURST_EN_CH2_M  (BIT(4))
#define GDMA_OUTDSCR_BURST_EN_CH2_V  0x1
#define GDMA_OUTDSCR_BURST_EN_CH2_S  4
/* GDMA_OUT_EOF_MODE_CH2 : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel 0 is
 generated when data need to transmit has been popped from FIFO in DMA.*/
#define GDMA_OUT_EOF_MODE_CH2    (BIT(3))
#define GDMA_OUT_EOF_MODE_CH2_M  (BIT(3))
#define GDMA_OUT_EOF_MODE_CH2_V  0x1
#define GDMA_OUT_EOF_MODE_CH2_S  3
/* GDMA_OUT_AUTO_WRBACK_CH2 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to enable automatic outlink-writeback when all the data in tx
buffer has been transmitted..*/
#define GDMA_OUT_AUTO_WRBACK_CH2    (BIT(2))
#define GDMA_OUT_AUTO_WRBACK_CH2_M  (BIT(2))
#define GDMA_OUT_AUTO_WRBACK_CH2_V  0x1
#define GDMA_OUT_AUTO_WRBACK_CH2_S  2
/* GDMA_OUT_LOOP_TEST_CH2 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: reserved.*/
#define GDMA_OUT_LOOP_TEST_CH2    (BIT(1))
#define GDMA_OUT_LOOP_TEST_CH2_M  (BIT(1))
#define GDMA_OUT_LOOP_TEST_CH2_V  0x1
#define GDMA_OUT_LOOP_TEST_CH2_S  1
/* GDMA_OUT_RST_CH2 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: This bit is used to reset DMA channel 2 Tx FSM and Tx FIFO pointer..*/
#define GDMA_OUT_RST_CH2    (BIT(0))
#define GDMA_OUT_RST_CH2_M  (BIT(0))
#define GDMA_OUT_RST_CH2_V  0x1
#define GDMA_OUT_RST_CH2_S  0

#define GDMA_OUT_CONF1_CH2_REG          (DR_REG_GDMA_BASE + 0x1E4)
/* GDMA_OUT_EXT_MEM_BK_SIZE_CH2 : R/W ;bitpos:[14:13] ;default: 2'b0 ; */
/*description: Block size of Tx channel 2 when DMA access external SRAM. 0: 16 bytes      1: 32
 bytes    2/3:reserved.*/
#define GDMA_OUT_EXT_MEM_BK_SIZE_CH2    0x00000003
#define GDMA_OUT_EXT_MEM_BK_SIZE_CH2_M  ((GDMA_OUT_EXT_MEM_BK_SIZE_CH2_V)<<(GDMA_OUT_EXT_MEM_BK_SIZE_CH2_S))
#define GDMA_OUT_EXT_MEM_BK_SIZE_CH2_V  0x3
#define GDMA_OUT_EXT_MEM_BK_SIZE_CH2_S  13
/* GDMA_OUT_CHECK_OWNER_CH2 : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to enable checking the owner attribute of the link descriptor..*/
#define GDMA_OUT_CHECK_OWNER_CH2    (BIT(12))
#define GDMA_OUT_CHECK_OWNER_CH2_M  (BIT(12))
#define GDMA_OUT_CHECK_OWNER_CH2_V  0x1
#define GDMA_OUT_CHECK_OWNER_CH2_S  12

#define GDMA_OUT_INT_RAW_CH2_REG          (DR_REG_GDMA_BASE + 0x1E8)
/* GDMA_OUTFIFO_UDF_L3_CH2_INT_RAW : R/WTC/SS ;bitpos:[7] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo of Tx channel 2 is
underflow. .*/
#define GDMA_OUTFIFO_UDF_L3_CH2_INT_RAW    (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH2_INT_RAW_M  (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH2_INT_RAW_V  0x1
#define GDMA_OUTFIFO_UDF_L3_CH2_INT_RAW_S  7
/* GDMA_OUTFIFO_OVF_L3_CH2_INT_RAW : R/WTC/SS ;bitpos:[6] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo of Tx channel 2 is
overflow. .*/
#define GDMA_OUTFIFO_OVF_L3_CH2_INT_RAW    (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH2_INT_RAW_M  (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH2_INT_RAW_V  0x1
#define GDMA_OUTFIFO_OVF_L3_CH2_INT_RAW_S  6
/* GDMA_OUTFIFO_UDF_L1_CH2_INT_RAW : R/WTC/SS ;bitpos:[5] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo of Tx channel 2 is
underflow. .*/
#define GDMA_OUTFIFO_UDF_L1_CH2_INT_RAW    (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH2_INT_RAW_M  (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH2_INT_RAW_V  0x1
#define GDMA_OUTFIFO_UDF_L1_CH2_INT_RAW_S  5
/* GDMA_OUTFIFO_OVF_L1_CH2_INT_RAW : R/WTC/SS ;bitpos:[4] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo of Tx channel 2 is
overflow. .*/
#define GDMA_OUTFIFO_OVF_L1_CH2_INT_RAW    (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH2_INT_RAW_M  (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH2_INT_RAW_V  0x1
#define GDMA_OUTFIFO_OVF_L1_CH2_INT_RAW_S  4
/* GDMA_OUT_TOTAL_EOF_CH2_INT_RAW : R/WTC/SS ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when data corresponding a outlink
(includes one link descriptor or few link descriptors) is transmitted out for Tx cha
nnel 2..*/
#define GDMA_OUT_TOTAL_EOF_CH2_INT_RAW    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH2_INT_RAW_M  (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH2_INT_RAW_V  0x1
#define GDMA_OUT_TOTAL_EOF_CH2_INT_RAW_S  3
/* GDMA_OUT_DSCR_ERR_CH2_INT_RAW : R/WTC/SS ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when detecting outlink descriptor
error, including owner error, the second and third word error of outlink descriptor
for Tx channel 2..*/
#define GDMA_OUT_DSCR_ERR_CH2_INT_RAW    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH2_INT_RAW_M  (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH2_INT_RAW_V  0x1
#define GDMA_OUT_DSCR_ERR_CH2_INT_RAW_S  2
/* GDMA_OUT_EOF_CH2_INT_RAW : R/WTC/SS ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data pointed by one
outlink descriptor has been read from memory for Tx channel 2. .*/
#define GDMA_OUT_EOF_CH2_INT_RAW    (BIT(1))
#define GDMA_OUT_EOF_CH2_INT_RAW_M  (BIT(1))
#define GDMA_OUT_EOF_CH2_INT_RAW_V  0x1
#define GDMA_OUT_EOF_CH2_INT_RAW_S  1
/* GDMA_OUT_DONE_CH2_INT_RAW : R/WTC/SS ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data pointed by one
outlink descriptor has been transmitted to peripherals for Tx channel 2..*/
#define GDMA_OUT_DONE_CH2_INT_RAW    (BIT(0))
#define GDMA_OUT_DONE_CH2_INT_RAW_M  (BIT(0))
#define GDMA_OUT_DONE_CH2_INT_RAW_V  0x1
#define GDMA_OUT_DONE_CH2_INT_RAW_S  0

#define GDMA_OUT_INT_ST_CH2_REG          (DR_REG_GDMA_BASE + 0x1EC)
/* GDMA_OUTFIFO_UDF_L3_CH2_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L3_CH2_INT_ST    (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH2_INT_ST_M  (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH2_INT_ST_V  0x1
#define GDMA_OUTFIFO_UDF_L3_CH2_INT_ST_S  7
/* GDMA_OUTFIFO_OVF_L3_CH2_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L3_CH2_INT_ST    (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH2_INT_ST_M  (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH2_INT_ST_V  0x1
#define GDMA_OUTFIFO_OVF_L3_CH2_INT_ST_S  6
/* GDMA_OUTFIFO_UDF_L1_CH2_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L1_CH2_INT_ST    (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH2_INT_ST_M  (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH2_INT_ST_V  0x1
#define GDMA_OUTFIFO_UDF_L1_CH2_INT_ST_S  5
/* GDMA_OUTFIFO_OVF_L1_CH2_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L1_CH2_INT_ST    (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH2_INT_ST_M  (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH2_INT_ST_V  0x1
#define GDMA_OUTFIFO_OVF_L1_CH2_INT_ST_S  4
/* GDMA_OUT_TOTAL_EOF_CH2_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt..*/
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ST    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ST_M  (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ST_V  0x1
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ST_S  3
/* GDMA_OUT_DSCR_ERR_CH2_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_OUT_DSCR_ERR_CH2_INT_ST    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH2_INT_ST_M  (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH2_INT_ST_V  0x1
#define GDMA_OUT_DSCR_ERR_CH2_INT_ST_S  2
/* GDMA_OUT_EOF_CH2_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_EOF_CH_INT interrupt..*/
#define GDMA_OUT_EOF_CH2_INT_ST    (BIT(1))
#define GDMA_OUT_EOF_CH2_INT_ST_M  (BIT(1))
#define GDMA_OUT_EOF_CH2_INT_ST_V  0x1
#define GDMA_OUT_EOF_CH2_INT_ST_S  1
/* GDMA_OUT_DONE_CH2_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_DONE_CH_INT interrupt..*/
#define GDMA_OUT_DONE_CH2_INT_ST    (BIT(0))
#define GDMA_OUT_DONE_CH2_INT_ST_M  (BIT(0))
#define GDMA_OUT_DONE_CH2_INT_ST_V  0x1
#define GDMA_OUT_DONE_CH2_INT_ST_S  0

#define GDMA_OUT_INT_ENA_CH2_REG          (DR_REG_GDMA_BASE + 0x1F0)
/* GDMA_OUTFIFO_UDF_L3_CH2_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L3_CH2_INT_ENA    (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH2_INT_ENA_M  (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH2_INT_ENA_V  0x1
#define GDMA_OUTFIFO_UDF_L3_CH2_INT_ENA_S  7
/* GDMA_OUTFIFO_OVF_L3_CH2_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L3_CH2_INT_ENA    (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH2_INT_ENA_M  (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH2_INT_ENA_V  0x1
#define GDMA_OUTFIFO_OVF_L3_CH2_INT_ENA_S  6
/* GDMA_OUTFIFO_UDF_L1_CH2_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L1_CH2_INT_ENA    (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH2_INT_ENA_M  (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH2_INT_ENA_V  0x1
#define GDMA_OUTFIFO_UDF_L1_CH2_INT_ENA_S  5
/* GDMA_OUTFIFO_OVF_L1_CH2_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L1_CH2_INT_ENA    (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH2_INT_ENA_M  (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH2_INT_ENA_V  0x1
#define GDMA_OUTFIFO_OVF_L1_CH2_INT_ENA_S  4
/* GDMA_OUT_TOTAL_EOF_CH2_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt..*/
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ENA    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ENA_M  (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ENA_V  0x1
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ENA_S  3
/* GDMA_OUT_DSCR_ERR_CH2_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_OUT_DSCR_ERR_CH2_INT_ENA    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH2_INT_ENA_M  (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH2_INT_ENA_V  0x1
#define GDMA_OUT_DSCR_ERR_CH2_INT_ENA_S  2
/* GDMA_OUT_EOF_CH2_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_EOF_CH_INT interrupt..*/
#define GDMA_OUT_EOF_CH2_INT_ENA    (BIT(1))
#define GDMA_OUT_EOF_CH2_INT_ENA_M  (BIT(1))
#define GDMA_OUT_EOF_CH2_INT_ENA_V  0x1
#define GDMA_OUT_EOF_CH2_INT_ENA_S  1
/* GDMA_OUT_DONE_CH2_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_DONE_CH_INT interrupt..*/
#define GDMA_OUT_DONE_CH2_INT_ENA    (BIT(0))
#define GDMA_OUT_DONE_CH2_INT_ENA_M  (BIT(0))
#define GDMA_OUT_DONE_CH2_INT_ENA_V  0x1
#define GDMA_OUT_DONE_CH2_INT_ENA_S  0

#define GDMA_OUT_INT_CLR_CH2_REG          (DR_REG_GDMA_BASE + 0x1F4)
/* GDMA_OUTFIFO_UDF_L3_CH2_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L3_CH2_INT_CLR    (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH2_INT_CLR_M  (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH2_INT_CLR_V  0x1
#define GDMA_OUTFIFO_UDF_L3_CH2_INT_CLR_S  7
/* GDMA_OUTFIFO_OVF_L3_CH2_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L3_CH2_INT_CLR    (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH2_INT_CLR_M  (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH2_INT_CLR_V  0x1
#define GDMA_OUTFIFO_OVF_L3_CH2_INT_CLR_S  6
/* GDMA_OUTFIFO_UDF_L1_CH2_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L1_CH2_INT_CLR    (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH2_INT_CLR_M  (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH2_INT_CLR_V  0x1
#define GDMA_OUTFIFO_UDF_L1_CH2_INT_CLR_S  5
/* GDMA_OUTFIFO_OVF_L1_CH2_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L1_CH2_INT_CLR    (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH2_INT_CLR_M  (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH2_INT_CLR_V  0x1
#define GDMA_OUTFIFO_OVF_L1_CH2_INT_CLR_S  4
/* GDMA_OUT_TOTAL_EOF_CH2_INT_CLR : WT ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt..*/
#define GDMA_OUT_TOTAL_EOF_CH2_INT_CLR    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH2_INT_CLR_M  (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH2_INT_CLR_V  0x1
#define GDMA_OUT_TOTAL_EOF_CH2_INT_CLR_S  3
/* GDMA_OUT_DSCR_ERR_CH2_INT_CLR : WT ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_OUT_DSCR_ERR_CH2_INT_CLR    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH2_INT_CLR_M  (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH2_INT_CLR_V  0x1
#define GDMA_OUT_DSCR_ERR_CH2_INT_CLR_S  2
/* GDMA_OUT_EOF_CH2_INT_CLR : WT ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_EOF_CH_INT interrupt..*/
#define GDMA_OUT_EOF_CH2_INT_CLR    (BIT(1))
#define GDMA_OUT_EOF_CH2_INT_CLR_M  (BIT(1))
#define GDMA_OUT_EOF_CH2_INT_CLR_V  0x1
#define GDMA_OUT_EOF_CH2_INT_CLR_S  1
/* GDMA_OUT_DONE_CH2_INT_CLR : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_DONE_CH_INT interrupt..*/
#define GDMA_OUT_DONE_CH2_INT_CLR    (BIT(0))
#define GDMA_OUT_DONE_CH2_INT_CLR_M  (BIT(0))
#define GDMA_OUT_DONE_CH2_INT_CLR_V  0x1
#define GDMA_OUT_DONE_CH2_INT_CLR_S  0

#define GDMA_OUTFIFO_STATUS_CH2_REG          (DR_REG_GDMA_BASE + 0x1F8)
/* GDMA_OUT_REMAIN_UNDER_4B_L3_CH2 : RO ;bitpos:[26] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_OUT_REMAIN_UNDER_4B_L3_CH2    (BIT(26))
#define GDMA_OUT_REMAIN_UNDER_4B_L3_CH2_M  (BIT(26))
#define GDMA_OUT_REMAIN_UNDER_4B_L3_CH2_V  0x1
#define GDMA_OUT_REMAIN_UNDER_4B_L3_CH2_S  26
/* GDMA_OUT_REMAIN_UNDER_3B_L3_CH2 : RO ;bitpos:[25] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_OUT_REMAIN_UNDER_3B_L3_CH2    (BIT(25))
#define GDMA_OUT_REMAIN_UNDER_3B_L3_CH2_M  (BIT(25))
#define GDMA_OUT_REMAIN_UNDER_3B_L3_CH2_V  0x1
#define GDMA_OUT_REMAIN_UNDER_3B_L3_CH2_S  25
/* GDMA_OUT_REMAIN_UNDER_2B_L3_CH2 : RO ;bitpos:[24] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_OUT_REMAIN_UNDER_2B_L3_CH2    (BIT(24))
#define GDMA_OUT_REMAIN_UNDER_2B_L3_CH2_M  (BIT(24))
#define GDMA_OUT_REMAIN_UNDER_2B_L3_CH2_V  0x1
#define GDMA_OUT_REMAIN_UNDER_2B_L3_CH2_S  24
/* GDMA_OUT_REMAIN_UNDER_1B_L3_CH2 : RO ;bitpos:[23] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_OUT_REMAIN_UNDER_1B_L3_CH2    (BIT(23))
#define GDMA_OUT_REMAIN_UNDER_1B_L3_CH2_M  (BIT(23))
#define GDMA_OUT_REMAIN_UNDER_1B_L3_CH2_V  0x1
#define GDMA_OUT_REMAIN_UNDER_1B_L3_CH2_S  23
/* GDMA_OUTFIFO_CNT_L3_CH2 : RO ;bitpos:[22:18] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L3 Tx FIFO for Tx channel 2..*/
#define GDMA_OUTFIFO_CNT_L3_CH2    0x0000001F
#define GDMA_OUTFIFO_CNT_L3_CH2_M  ((GDMA_OUTFIFO_CNT_L3_CH2_V)<<(GDMA_OUTFIFO_CNT_L3_CH2_S))
#define GDMA_OUTFIFO_CNT_L3_CH2_V  0x1F
#define GDMA_OUTFIFO_CNT_L3_CH2_S  18
/* GDMA_OUTFIFO_CNT_L2_CH2 : RO ;bitpos:[17:11] ;default: 7'b0 ; */
/*description: The register stores the byte number of the data in L2 Tx FIFO for Tx channel 2..*/
#define GDMA_OUTFIFO_CNT_L2_CH2    0x0000007F
#define GDMA_OUTFIFO_CNT_L2_CH2_M  ((GDMA_OUTFIFO_CNT_L2_CH2_V)<<(GDMA_OUTFIFO_CNT_L2_CH2_S))
#define GDMA_OUTFIFO_CNT_L2_CH2_V  0x7F
#define GDMA_OUTFIFO_CNT_L2_CH2_S  11
/* GDMA_OUTFIFO_CNT_L1_CH2 : RO ;bitpos:[10:6] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L1 Tx FIFO for Tx channel 2..*/
#define GDMA_OUTFIFO_CNT_L1_CH2    0x0000001F
#define GDMA_OUTFIFO_CNT_L1_CH2_M  ((GDMA_OUTFIFO_CNT_L1_CH2_V)<<(GDMA_OUTFIFO_CNT_L1_CH2_S))
#define GDMA_OUTFIFO_CNT_L1_CH2_V  0x1F
#define GDMA_OUTFIFO_CNT_L1_CH2_S  6
/* GDMA_OUTFIFO_EMPTY_L3_CH2 : RO ;bitpos:[5] ;default: 1'b1 ; */
/*description: L3 Tx FIFO empty signal for Tx channel 2..*/
#define GDMA_OUTFIFO_EMPTY_L3_CH2    (BIT(5))
#define GDMA_OUTFIFO_EMPTY_L3_CH2_M  (BIT(5))
#define GDMA_OUTFIFO_EMPTY_L3_CH2_V  0x1
#define GDMA_OUTFIFO_EMPTY_L3_CH2_S  5
/* GDMA_OUTFIFO_FULL_L3_CH2 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: L3 Tx FIFO full signal for Tx channel 2..*/
#define GDMA_OUTFIFO_FULL_L3_CH2    (BIT(4))
#define GDMA_OUTFIFO_FULL_L3_CH2_M  (BIT(4))
#define GDMA_OUTFIFO_FULL_L3_CH2_V  0x1
#define GDMA_OUTFIFO_FULL_L3_CH2_S  4
/* GDMA_OUTFIFO_EMPTY_L2_CH2 : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: L2 Tx FIFO empty signal for Tx channel 2..*/
#define GDMA_OUTFIFO_EMPTY_L2_CH2    (BIT(3))
#define GDMA_OUTFIFO_EMPTY_L2_CH2_M  (BIT(3))
#define GDMA_OUTFIFO_EMPTY_L2_CH2_V  0x1
#define GDMA_OUTFIFO_EMPTY_L2_CH2_S  3
/* GDMA_OUTFIFO_FULL_L2_CH2 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: L2 Tx FIFO full signal for Tx channel 2..*/
#define GDMA_OUTFIFO_FULL_L2_CH2    (BIT(2))
#define GDMA_OUTFIFO_FULL_L2_CH2_M  (BIT(2))
#define GDMA_OUTFIFO_FULL_L2_CH2_V  0x1
#define GDMA_OUTFIFO_FULL_L2_CH2_S  2
/* GDMA_OUTFIFO_EMPTY_L1_CH2 : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: L1 Tx FIFO empty signal for Tx channel 2..*/
#define GDMA_OUTFIFO_EMPTY_L1_CH2    (BIT(1))
#define GDMA_OUTFIFO_EMPTY_L1_CH2_M  (BIT(1))
#define GDMA_OUTFIFO_EMPTY_L1_CH2_V  0x1
#define GDMA_OUTFIFO_EMPTY_L1_CH2_S  1
/* GDMA_OUTFIFO_FULL_L1_CH2 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: L1 Tx FIFO full signal for Tx channel 2..*/
#define GDMA_OUTFIFO_FULL_L1_CH2    (BIT(0))
#define GDMA_OUTFIFO_FULL_L1_CH2_M  (BIT(0))
#define GDMA_OUTFIFO_FULL_L1_CH2_V  0x1
#define GDMA_OUTFIFO_FULL_L1_CH2_S  0

#define GDMA_OUT_PUSH_CH2_REG          (DR_REG_GDMA_BASE + 0x1FC)
/* GDMA_OUTFIFO_PUSH_CH2 : R/W/SC ;bitpos:[9] ;default: 1'h0 ; */
/*description: Set this bit to push data into DMA FIFO..*/
#define GDMA_OUTFIFO_PUSH_CH2    (BIT(9))
#define GDMA_OUTFIFO_PUSH_CH2_M  (BIT(9))
#define GDMA_OUTFIFO_PUSH_CH2_V  0x1
#define GDMA_OUTFIFO_PUSH_CH2_S  9
/* GDMA_OUTFIFO_WDATA_CH2 : R/W ;bitpos:[8:0] ;default: 9'h0 ; */
/*description: This register stores the data that need to be pushed into DMA FIFO..*/
#define GDMA_OUTFIFO_WDATA_CH2    0x000001FF
#define GDMA_OUTFIFO_WDATA_CH2_M  ((GDMA_OUTFIFO_WDATA_CH2_V)<<(GDMA_OUTFIFO_WDATA_CH2_S))
#define GDMA_OUTFIFO_WDATA_CH2_V  0x1FF
#define GDMA_OUTFIFO_WDATA_CH2_S  0

#define GDMA_OUT_LINK_CH2_REG          (DR_REG_GDMA_BASE + 0x200)
/* GDMA_OUTLINK_PARK_CH2 : RO ;bitpos:[23] ;default: 1'h1 ; */
/*description: 1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's
FSM is working..*/
#define GDMA_OUTLINK_PARK_CH2    (BIT(23))
#define GDMA_OUTLINK_PARK_CH2_M  (BIT(23))
#define GDMA_OUTLINK_PARK_CH2_V  0x1
#define GDMA_OUTLINK_PARK_CH2_S  23
/* GDMA_OUTLINK_RESTART_CH2 : R/W/SC ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to restart a new outlink from the last address. .*/
#define GDMA_OUTLINK_RESTART_CH2    (BIT(22))
#define GDMA_OUTLINK_RESTART_CH2_M  (BIT(22))
#define GDMA_OUTLINK_RESTART_CH2_V  0x1
#define GDMA_OUTLINK_RESTART_CH2_S  22
/* GDMA_OUTLINK_START_CH2 : R/W/SC ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to start dealing with the outlink descriptors..*/
#define GDMA_OUTLINK_START_CH2    (BIT(21))
#define GDMA_OUTLINK_START_CH2_M  (BIT(21))
#define GDMA_OUTLINK_START_CH2_V  0x1
#define GDMA_OUTLINK_START_CH2_S  21
/* GDMA_OUTLINK_STOP_CH2 : R/W/SC ;bitpos:[20] ;default: 1'b0 ; */
/*description: Set this bit to stop dealing with the outlink descriptors..*/
#define GDMA_OUTLINK_STOP_CH2    (BIT(20))
#define GDMA_OUTLINK_STOP_CH2_M  (BIT(20))
#define GDMA_OUTLINK_STOP_CH2_V  0x1
#define GDMA_OUTLINK_STOP_CH2_S  20
/* GDMA_OUTLINK_ADDR_CH2 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: This register stores the 20 least significant bits of the first outlink
descriptor's address..*/
#define GDMA_OUTLINK_ADDR_CH2    0x000FFFFF
#define GDMA_OUTLINK_ADDR_CH2_M  ((GDMA_OUTLINK_ADDR_CH2_V)<<(GDMA_OUTLINK_ADDR_CH2_S))
#define GDMA_OUTLINK_ADDR_CH2_V  0xFFFFF
#define GDMA_OUTLINK_ADDR_CH2_S  0

#define GDMA_OUT_STATE_CH2_REG          (DR_REG_GDMA_BASE + 0x204)
/* GDMA_OUT_STATE_CH2 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: reserved.*/
#define GDMA_OUT_STATE_CH2    0x00000007
#define GDMA_OUT_STATE_CH2_M  ((GDMA_OUT_STATE_CH2_V)<<(GDMA_OUT_STATE_CH2_S))
#define GDMA_OUT_STATE_CH2_V  0x7
#define GDMA_OUT_STATE_CH2_S  20
/* GDMA_OUT_DSCR_STATE_CH2 : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: reserved.*/
#define GDMA_OUT_DSCR_STATE_CH2    0x00000003
#define GDMA_OUT_DSCR_STATE_CH2_M  ((GDMA_OUT_DSCR_STATE_CH2_V)<<(GDMA_OUT_DSCR_STATE_CH2_S))
#define GDMA_OUT_DSCR_STATE_CH2_V  0x3
#define GDMA_OUT_DSCR_STATE_CH2_S  18
/* GDMA_OUTLINK_DSCR_ADDR_CH2 : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: This register stores the current outlink descriptor's address..*/
#define GDMA_OUTLINK_DSCR_ADDR_CH2    0x0003FFFF
#define GDMA_OUTLINK_DSCR_ADDR_CH2_M  ((GDMA_OUTLINK_DSCR_ADDR_CH2_V)<<(GDMA_OUTLINK_DSCR_ADDR_CH2_S))
#define GDMA_OUTLINK_DSCR_ADDR_CH2_V  0x3FFFF
#define GDMA_OUTLINK_DSCR_ADDR_CH2_S  0

#define GDMA_OUT_EOF_DES_ADDR_CH2_REG          (DR_REG_GDMA_BASE + 0x208)
/* GDMA_OUT_EOF_DES_ADDR_CH2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the outlink descriptor when the EOF bit in
this descriptor is 1..*/
#define GDMA_OUT_EOF_DES_ADDR_CH2    0xFFFFFFFF
#define GDMA_OUT_EOF_DES_ADDR_CH2_M  ((GDMA_OUT_EOF_DES_ADDR_CH2_V)<<(GDMA_OUT_EOF_DES_ADDR_CH2_S))
#define GDMA_OUT_EOF_DES_ADDR_CH2_V  0xFFFFFFFF
#define GDMA_OUT_EOF_DES_ADDR_CH2_S  0

#define GDMA_OUT_EOF_BFR_DES_ADDR_CH2_REG          (DR_REG_GDMA_BASE + 0x20C)
/* GDMA_OUT_EOF_BFR_DES_ADDR_CH2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the outlink descriptor before the last
outlink descriptor..*/
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH2    0xFFFFFFFF
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH2_M  ((GDMA_OUT_EOF_BFR_DES_ADDR_CH2_V)<<(GDMA_OUT_EOF_BFR_DES_ADDR_CH2_S))
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH2_V  0xFFFFFFFF
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH2_S  0

#define GDMA_OUT_DSCR_CH2_REG          (DR_REG_GDMA_BASE + 0x210)
/* GDMA_OUTLINK_DSCR_CH2 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the current outlink descriptor y..*/
#define GDMA_OUTLINK_DSCR_CH2    0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_CH2_M  ((GDMA_OUTLINK_DSCR_CH2_V)<<(GDMA_OUTLINK_DSCR_CH2_S))
#define GDMA_OUTLINK_DSCR_CH2_V  0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_CH2_S  0

#define GDMA_OUT_DSCR_BF0_CH2_REG          (DR_REG_GDMA_BASE + 0x214)
/* GDMA_OUTLINK_DSCR_BF0_CH2 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the last outlink descriptor y-1..*/
#define GDMA_OUTLINK_DSCR_BF0_CH2    0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_BF0_CH2_M  ((GDMA_OUTLINK_DSCR_BF0_CH2_V)<<(GDMA_OUTLINK_DSCR_BF0_CH2_S))
#define GDMA_OUTLINK_DSCR_BF0_CH2_V  0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_BF0_CH2_S  0

#define GDMA_OUT_DSCR_BF1_CH2_REG          (DR_REG_GDMA_BASE + 0x218)
/* GDMA_OUTLINK_DSCR_BF1_CH2 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the second-to-last inlink descriptor x-2..*/
#define GDMA_OUTLINK_DSCR_BF1_CH2    0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_BF1_CH2_M  ((GDMA_OUTLINK_DSCR_BF1_CH2_V)<<(GDMA_OUTLINK_DSCR_BF1_CH2_S))
#define GDMA_OUTLINK_DSCR_BF1_CH2_V  0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_BF1_CH2_S  0

#define GDMA_OUT_WEIGHT_CH2_REG          (DR_REG_GDMA_BASE + 0x21C)
/* GDMA_TX_WEIGHT_CH2 : R/W ;bitpos:[11:8] ;default: 4'hf ; */
/*description: The weight of Tx channel 2. .*/
#define GDMA_TX_WEIGHT_CH2    0x0000000F
#define GDMA_TX_WEIGHT_CH2_M  ((GDMA_TX_WEIGHT_CH2_V)<<(GDMA_TX_WEIGHT_CH2_S))
#define GDMA_TX_WEIGHT_CH2_V  0xF
#define GDMA_TX_WEIGHT_CH2_S  8

#define GDMA_OUT_PRI_CH2_REG          (DR_REG_GDMA_BASE + 0x224)
/* GDMA_TX_PRI_CH2 : R/W ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: The priority of Tx channel 2. The larger of the value, the higher of the
priority..*/
#define GDMA_TX_PRI_CH2    0x0000000F
#define GDMA_TX_PRI_CH2_M  ((GDMA_TX_PRI_CH2_V)<<(GDMA_TX_PRI_CH2_S))
#define GDMA_TX_PRI_CH2_V  0xF
#define GDMA_TX_PRI_CH2_S  0

#define GDMA_OUT_PERI_SEL_CH2_REG          (DR_REG_GDMA_BASE + 0x228)
/* GDMA_PERI_OUT_SEL_CH2 : R/W ;bitpos:[5:0] ;default: 6'h3f ; */
/*description: This register is used to select peripheral for Tx channel 2. 0:SPI2. 1: SPI3. 2:
 UHCI0. 3: I2S0. 4: I2S1. 5: LCD_CAM. 6: AES. 7: SHA. 8: ADC_DAC. 9: RMT.; 7: AE
S. 8: SHA. 9: ADC_DAC..*/
#define GDMA_PERI_OUT_SEL_CH2    0x0000003F
#define GDMA_PERI_OUT_SEL_CH2_M  ((GDMA_PERI_OUT_SEL_CH2_V)<<(GDMA_PERI_OUT_SEL_CH2_S))
#define GDMA_PERI_OUT_SEL_CH2_V  0x3F
#define GDMA_PERI_OUT_SEL_CH2_S  0

#define GDMA_IN_CONF0_CH3_REG          (DR_REG_GDMA_BASE + 0x240)
/* GDMA_MEM_TRANS_EN_CH3 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit 1 to enable automatic transmitting data from memory to memory via
DMA..*/
#define GDMA_MEM_TRANS_EN_CH3    (BIT(4))
#define GDMA_MEM_TRANS_EN_CH3_M  (BIT(4))
#define GDMA_MEM_TRANS_EN_CH3_V  0x1
#define GDMA_MEM_TRANS_EN_CH3_S  4
/* GDMA_IN_DATA_BURST_EN_CH3 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Rx channel 2 receiving data
when accessing internal SRAM. .*/
#define GDMA_IN_DATA_BURST_EN_CH3    (BIT(3))
#define GDMA_IN_DATA_BURST_EN_CH3_M  (BIT(3))
#define GDMA_IN_DATA_BURST_EN_CH3_V  0x1
#define GDMA_IN_DATA_BURST_EN_CH3_S  3
/* GDMA_INDSCR_BURST_EN_CH3 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Rx channel 2 reading link
descriptor when accessing internal SRAM. .*/
#define GDMA_INDSCR_BURST_EN_CH3    (BIT(2))
#define GDMA_INDSCR_BURST_EN_CH3_M  (BIT(2))
#define GDMA_INDSCR_BURST_EN_CH3_V  0x1
#define GDMA_INDSCR_BURST_EN_CH3_S  2
/* GDMA_IN_LOOP_TEST_CH3 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: reserved.*/
#define GDMA_IN_LOOP_TEST_CH3    (BIT(1))
#define GDMA_IN_LOOP_TEST_CH3_M  (BIT(1))
#define GDMA_IN_LOOP_TEST_CH3_V  0x1
#define GDMA_IN_LOOP_TEST_CH3_S  1
/* GDMA_IN_RST_CH3 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: This bit is used to reset DMA channel 3 Rx FSM and Rx FIFO pointer..*/
#define GDMA_IN_RST_CH3    (BIT(0))
#define GDMA_IN_RST_CH3_M  (BIT(0))
#define GDMA_IN_RST_CH3_V  0x1
#define GDMA_IN_RST_CH3_S  0

#define GDMA_IN_CONF1_CH3_REG          (DR_REG_GDMA_BASE + 0x244)
/* GDMA_IN_EXT_MEM_BK_SIZE_CH3 : R/W ;bitpos:[14:13] ;default: 2'b0 ; */
/*description: Block size of Rx channel 3 when DMA access external SRAM. 0: 16 bytes      1: 32
 bytes    2/3:reserved.*/
#define GDMA_IN_EXT_MEM_BK_SIZE_CH3    0x00000003
#define GDMA_IN_EXT_MEM_BK_SIZE_CH3_M  ((GDMA_IN_EXT_MEM_BK_SIZE_CH3_V)<<(GDMA_IN_EXT_MEM_BK_SIZE_CH3_S))
#define GDMA_IN_EXT_MEM_BK_SIZE_CH3_V  0x3
#define GDMA_IN_EXT_MEM_BK_SIZE_CH3_S  13
/* GDMA_IN_CHECK_OWNER_CH3 : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to enable checking the owner attribute of the link descriptor..*/
#define GDMA_IN_CHECK_OWNER_CH3    (BIT(12))
#define GDMA_IN_CHECK_OWNER_CH3_M  (BIT(12))
#define GDMA_IN_CHECK_OWNER_CH3_V  0x1
#define GDMA_IN_CHECK_OWNER_CH3_S  12
/* GDMA_DMA_INFIFO_FULL_THRS_CH3 : R/W ;bitpos:[11:0] ;default: 12'hc ; */
/*description: This register is used to generate the INFIFO_FULL_WM_INT interrupt when Rx
channel 3 received byte number in Rx FIFO is up to the value of the register..*/
#define GDMA_DMA_INFIFO_FULL_THRS_CH3    0x00000FFF
#define GDMA_DMA_INFIFO_FULL_THRS_CH3_M  ((GDMA_DMA_INFIFO_FULL_THRS_CH3_V)<<(GDMA_DMA_INFIFO_FULL_THRS_CH3_S))
#define GDMA_DMA_INFIFO_FULL_THRS_CH3_V  0xFFF
#define GDMA_DMA_INFIFO_FULL_THRS_CH3_S  0

#define GDMA_IN_INT_RAW_CH3_REG          (DR_REG_GDMA_BASE + 0x248)
/* GDMA_INFIFO_UDF_L3_CH3_INT_RAW : R/WTC/SS ;bitpos:[9] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo of Rx channel 3 is
underflow. .*/
#define GDMA_INFIFO_UDF_L3_CH3_INT_RAW    (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH3_INT_RAW_M  (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH3_INT_RAW_V  0x1
#define GDMA_INFIFO_UDF_L3_CH3_INT_RAW_S  9
/* GDMA_INFIFO_OVF_L3_CH3_INT_RAW : R/WTC/SS ;bitpos:[8] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo of Rx channel 3 is
overflow. .*/
#define GDMA_INFIFO_OVF_L3_CH3_INT_RAW    (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH3_INT_RAW_M  (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH3_INT_RAW_V  0x1
#define GDMA_INFIFO_OVF_L3_CH3_INT_RAW_S  8
/* GDMA_INFIFO_UDF_L1_CH3_INT_RAW : R/WTC/SS ;bitpos:[7] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo of Rx channel 3 is
underflow. .*/
#define GDMA_INFIFO_UDF_L1_CH3_INT_RAW    (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH3_INT_RAW_M  (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH3_INT_RAW_V  0x1
#define GDMA_INFIFO_UDF_L1_CH3_INT_RAW_S  7
/* GDMA_INFIFO_OVF_L1_CH3_INT_RAW : R/WTC/SS ;bitpos:[6] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo of Rx channel 3 is
overflow. .*/
#define GDMA_INFIFO_OVF_L1_CH3_INT_RAW    (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH3_INT_RAW_M  (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH3_INT_RAW_V  0x1
#define GDMA_INFIFO_OVF_L1_CH3_INT_RAW_S  6
/* GDMA_INFIFO_FULL_WM_CH3_INT_RAW : R/WTC/SS ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when received data byte number is up
to threshold configured by REG_DMA_INFIFO_FULL_THRS_CH0 in Rx FIFO of channel 3..*/
#define GDMA_INFIFO_FULL_WM_CH3_INT_RAW    (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH3_INT_RAW_M  (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH3_INT_RAW_V  0x1
#define GDMA_INFIFO_FULL_WM_CH3_INT_RAW_S  5
/* GDMA_IN_DSCR_EMPTY_CH3_INT_RAW : R/WTC/SS ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when Rx buffer pointed by inlink is
full and receiving data is not completed, but there is no more inlink for Rx chann
el 3..*/
#define GDMA_IN_DSCR_EMPTY_CH3_INT_RAW    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH3_INT_RAW_M  (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH3_INT_RAW_V  0x1
#define GDMA_IN_DSCR_EMPTY_CH3_INT_RAW_S  4
/* GDMA_IN_DSCR_ERR_CH3_INT_RAW : R/WTC/SS ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when detecting inlink descriptor error
, including owner error, the second and third word error of inlink descriptor
for Rx channel 3..*/
#define GDMA_IN_DSCR_ERR_CH3_INT_RAW    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH3_INT_RAW_M  (BIT(3))
#define GDMA_IN_DSCR_ERR_CH3_INT_RAW_V  0x1
#define GDMA_IN_DSCR_ERR_CH3_INT_RAW_S  3
/* GDMA_IN_ERR_EOF_CH3_INT_RAW : R/WTC/SS ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when data error is detected only in
the case that the peripheral is UHCI0 for Rx channel 3. For other peripherals,
this raw interrupt is reserved..*/
#define GDMA_IN_ERR_EOF_CH3_INT_RAW    (BIT(2))
#define GDMA_IN_ERR_EOF_CH3_INT_RAW_M  (BIT(2))
#define GDMA_IN_ERR_EOF_CH3_INT_RAW_V  0x1
#define GDMA_IN_ERR_EOF_CH3_INT_RAW_S  2
/* GDMA_IN_SUC_EOF_CH3_INT_RAW : R/WTC/SS ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data pointed by one
inlink descriptor has been received for Rx channel 3. For UHCI0, the raw interrupt
bit turns to high level when the last data pointed by one inlink descriptor has
been received and no data error is detected for Rx channel 0..*/
#define GDMA_IN_SUC_EOF_CH3_INT_RAW    (BIT(1))
#define GDMA_IN_SUC_EOF_CH3_INT_RAW_M  (BIT(1))
#define GDMA_IN_SUC_EOF_CH3_INT_RAW_V  0x1
#define GDMA_IN_SUC_EOF_CH3_INT_RAW_S  1
/* GDMA_IN_DONE_CH3_INT_RAW : R/WTC/SS ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data pointed by one
inlink descriptor has been received for Rx channel 3..*/
#define GDMA_IN_DONE_CH3_INT_RAW    (BIT(0))
#define GDMA_IN_DONE_CH3_INT_RAW_M  (BIT(0))
#define GDMA_IN_DONE_CH3_INT_RAW_V  0x1
#define GDMA_IN_DONE_CH3_INT_RAW_S  0

#define GDMA_IN_INT_ST_CH3_REG          (DR_REG_GDMA_BASE + 0x24C)
/* GDMA_INFIFO_UDF_L3_CH3_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L3_CH3_INT_ST    (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH3_INT_ST_M  (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH3_INT_ST_V  0x1
#define GDMA_INFIFO_UDF_L3_CH3_INT_ST_S  9
/* GDMA_INFIFO_OVF_L3_CH3_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L3_CH3_INT_ST    (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH3_INT_ST_M  (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH3_INT_ST_V  0x1
#define GDMA_INFIFO_OVF_L3_CH3_INT_ST_S  8
/* GDMA_INFIFO_UDF_L1_CH3_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L1_CH3_INT_ST    (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH3_INT_ST_M  (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH3_INT_ST_V  0x1
#define GDMA_INFIFO_UDF_L1_CH3_INT_ST_S  7
/* GDMA_INFIFO_OVF_L1_CH3_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L1_CH3_INT_ST    (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH3_INT_ST_M  (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH3_INT_ST_V  0x1
#define GDMA_INFIFO_OVF_L1_CH3_INT_ST_S  6
/* GDMA_INFIFO_FULL_WM_CH3_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_FULL_WM_CH_INT interrupt..*/
#define GDMA_INFIFO_FULL_WM_CH3_INT_ST    (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH3_INT_ST_M  (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH3_INT_ST_V  0x1
#define GDMA_INFIFO_FULL_WM_CH3_INT_ST_S  5
/* GDMA_IN_DSCR_EMPTY_CH3_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt..*/
#define GDMA_IN_DSCR_EMPTY_CH3_INT_ST    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH3_INT_ST_M  (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH3_INT_ST_V  0x1
#define GDMA_IN_DSCR_EMPTY_CH3_INT_ST_S  4
/* GDMA_IN_DSCR_ERR_CH3_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_IN_DSCR_ERR_CH3_INT_ST    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH3_INT_ST_M  (BIT(3))
#define GDMA_IN_DSCR_ERR_CH3_INT_ST_V  0x1
#define GDMA_IN_DSCR_ERR_CH3_INT_ST_S  3
/* GDMA_IN_ERR_EOF_CH3_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt..*/
#define GDMA_IN_ERR_EOF_CH3_INT_ST    (BIT(2))
#define GDMA_IN_ERR_EOF_CH3_INT_ST_M  (BIT(2))
#define GDMA_IN_ERR_EOF_CH3_INT_ST_V  0x1
#define GDMA_IN_ERR_EOF_CH3_INT_ST_S  2
/* GDMA_IN_SUC_EOF_CH3_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt..*/
#define GDMA_IN_SUC_EOF_CH3_INT_ST    (BIT(1))
#define GDMA_IN_SUC_EOF_CH3_INT_ST_M  (BIT(1))
#define GDMA_IN_SUC_EOF_CH3_INT_ST_V  0x1
#define GDMA_IN_SUC_EOF_CH3_INT_ST_S  1
/* GDMA_IN_DONE_CH3_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DONE_CH_INT interrupt..*/
#define GDMA_IN_DONE_CH3_INT_ST    (BIT(0))
#define GDMA_IN_DONE_CH3_INT_ST_M  (BIT(0))
#define GDMA_IN_DONE_CH3_INT_ST_V  0x1
#define GDMA_IN_DONE_CH3_INT_ST_S  0

#define GDMA_IN_INT_ENA_CH3_REG          (DR_REG_GDMA_BASE + 0x250)
/* GDMA_INFIFO_UDF_L3_CH3_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L3_CH3_INT_ENA    (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH3_INT_ENA_M  (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH3_INT_ENA_V  0x1
#define GDMA_INFIFO_UDF_L3_CH3_INT_ENA_S  9
/* GDMA_INFIFO_OVF_L3_CH3_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L3_CH3_INT_ENA    (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH3_INT_ENA_M  (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH3_INT_ENA_V  0x1
#define GDMA_INFIFO_OVF_L3_CH3_INT_ENA_S  8
/* GDMA_INFIFO_UDF_L1_CH3_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L1_CH3_INT_ENA    (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH3_INT_ENA_M  (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH3_INT_ENA_V  0x1
#define GDMA_INFIFO_UDF_L1_CH3_INT_ENA_S  7
/* GDMA_INFIFO_OVF_L1_CH3_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L1_CH3_INT_ENA    (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH3_INT_ENA_M  (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH3_INT_ENA_V  0x1
#define GDMA_INFIFO_OVF_L1_CH3_INT_ENA_S  6
/* GDMA_INFIFO_FULL_WM_CH3_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_FULL_WM_CH_INT interrupt..*/
#define GDMA_INFIFO_FULL_WM_CH3_INT_ENA    (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH3_INT_ENA_M  (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH3_INT_ENA_V  0x1
#define GDMA_INFIFO_FULL_WM_CH3_INT_ENA_S  5
/* GDMA_IN_DSCR_EMPTY_CH3_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt..*/
#define GDMA_IN_DSCR_EMPTY_CH3_INT_ENA    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH3_INT_ENA_M  (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH3_INT_ENA_V  0x1
#define GDMA_IN_DSCR_EMPTY_CH3_INT_ENA_S  4
/* GDMA_IN_DSCR_ERR_CH3_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_IN_DSCR_ERR_CH3_INT_ENA    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH3_INT_ENA_M  (BIT(3))
#define GDMA_IN_DSCR_ERR_CH3_INT_ENA_V  0x1
#define GDMA_IN_DSCR_ERR_CH3_INT_ENA_S  3
/* GDMA_IN_ERR_EOF_CH3_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt..*/
#define GDMA_IN_ERR_EOF_CH3_INT_ENA    (BIT(2))
#define GDMA_IN_ERR_EOF_CH3_INT_ENA_M  (BIT(2))
#define GDMA_IN_ERR_EOF_CH3_INT_ENA_V  0x1
#define GDMA_IN_ERR_EOF_CH3_INT_ENA_S  2
/* GDMA_IN_SUC_EOF_CH3_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt..*/
#define GDMA_IN_SUC_EOF_CH3_INT_ENA    (BIT(1))
#define GDMA_IN_SUC_EOF_CH3_INT_ENA_M  (BIT(1))
#define GDMA_IN_SUC_EOF_CH3_INT_ENA_V  0x1
#define GDMA_IN_SUC_EOF_CH3_INT_ENA_S  1
/* GDMA_IN_DONE_CH3_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DONE_CH_INT interrupt..*/
#define GDMA_IN_DONE_CH3_INT_ENA    (BIT(0))
#define GDMA_IN_DONE_CH3_INT_ENA_M  (BIT(0))
#define GDMA_IN_DONE_CH3_INT_ENA_V  0x1
#define GDMA_IN_DONE_CH3_INT_ENA_S  0

#define GDMA_IN_INT_CLR_CH3_REG          (DR_REG_GDMA_BASE + 0x254)
/* GDMA_INFIFO_UDF_L3_CH3_INT_CLR : WT ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L3_CH3_INT_CLR    (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH3_INT_CLR_M  (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH3_INT_CLR_V  0x1
#define GDMA_INFIFO_UDF_L3_CH3_INT_CLR_S  9
/* GDMA_INFIFO_OVF_L3_CH3_INT_CLR : WT ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L3_CH3_INT_CLR    (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH3_INT_CLR_M  (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH3_INT_CLR_V  0x1
#define GDMA_INFIFO_OVF_L3_CH3_INT_CLR_S  8
/* GDMA_INFIFO_UDF_L1_CH3_INT_CLR : WT ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L1_CH3_INT_CLR    (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH3_INT_CLR_M  (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH3_INT_CLR_V  0x1
#define GDMA_INFIFO_UDF_L1_CH3_INT_CLR_S  7
/* GDMA_INFIFO_OVF_L1_CH3_INT_CLR : WT ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L1_CH3_INT_CLR    (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH3_INT_CLR_M  (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH3_INT_CLR_V  0x1
#define GDMA_INFIFO_OVF_L1_CH3_INT_CLR_S  6
/* GDMA_DMA_INFIFO_FULL_WM_CH3_INT_CLR : WT ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_FULL_WM_CH_INT interrupt..*/
#define GDMA_DMA_INFIFO_FULL_WM_CH3_INT_CLR    (BIT(5))
#define GDMA_DMA_INFIFO_FULL_WM_CH3_INT_CLR_M  (BIT(5))
#define GDMA_DMA_INFIFO_FULL_WM_CH3_INT_CLR_V  0x1
#define GDMA_DMA_INFIFO_FULL_WM_CH3_INT_CLR_S  5
/* GDMA_IN_DSCR_EMPTY_CH3_INT_CLR : WT ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt..*/
#define GDMA_IN_DSCR_EMPTY_CH3_INT_CLR    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH3_INT_CLR_M  (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH3_INT_CLR_V  0x1
#define GDMA_IN_DSCR_EMPTY_CH3_INT_CLR_S  4
/* GDMA_IN_DSCR_ERR_CH3_INT_CLR : WT ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_IN_DSCR_ERR_CH3_INT_CLR    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH3_INT_CLR_M  (BIT(3))
#define GDMA_IN_DSCR_ERR_CH3_INT_CLR_V  0x1
#define GDMA_IN_DSCR_ERR_CH3_INT_CLR_S  3
/* GDMA_IN_ERR_EOF_CH3_INT_CLR : WT ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_ERR_EOF_CH_INT interrupt..*/
#define GDMA_IN_ERR_EOF_CH3_INT_CLR    (BIT(2))
#define GDMA_IN_ERR_EOF_CH3_INT_CLR_M  (BIT(2))
#define GDMA_IN_ERR_EOF_CH3_INT_CLR_V  0x1
#define GDMA_IN_ERR_EOF_CH3_INT_CLR_S  2
/* GDMA_IN_SUC_EOF_CH3_INT_CLR : WT ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_SUC_EOF_CH_INT interrupt..*/
#define GDMA_IN_SUC_EOF_CH3_INT_CLR    (BIT(1))
#define GDMA_IN_SUC_EOF_CH3_INT_CLR_M  (BIT(1))
#define GDMA_IN_SUC_EOF_CH3_INT_CLR_V  0x1
#define GDMA_IN_SUC_EOF_CH3_INT_CLR_S  1
/* GDMA_IN_DONE_CH3_INT_CLR : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DONE_CH_INT interrupt..*/
#define GDMA_IN_DONE_CH3_INT_CLR    (BIT(0))
#define GDMA_IN_DONE_CH3_INT_CLR_M  (BIT(0))
#define GDMA_IN_DONE_CH3_INT_CLR_V  0x1
#define GDMA_IN_DONE_CH3_INT_CLR_S  0

#define GDMA_INFIFO_STATUS_CH3_REG          (DR_REG_GDMA_BASE + 0x258)
/* GDMA_IN_BUF_HUNGRY_CH3 : RO ;bitpos:[28] ;default: 1'b0 ; */
/*description: reserved.*/
#define GDMA_IN_BUF_HUNGRY_CH3    (BIT(28))
#define GDMA_IN_BUF_HUNGRY_CH3_M  (BIT(28))
#define GDMA_IN_BUF_HUNGRY_CH3_V  0x1
#define GDMA_IN_BUF_HUNGRY_CH3_S  28
/* GDMA_IN_REMAIN_UNDER_4B_L3_CH3 : RO ;bitpos:[27] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_IN_REMAIN_UNDER_4B_L3_CH3    (BIT(27))
#define GDMA_IN_REMAIN_UNDER_4B_L3_CH3_M  (BIT(27))
#define GDMA_IN_REMAIN_UNDER_4B_L3_CH3_V  0x1
#define GDMA_IN_REMAIN_UNDER_4B_L3_CH3_S  27
/* GDMA_IN_REMAIN_UNDER_3B_L3_CH3 : RO ;bitpos:[26] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_IN_REMAIN_UNDER_3B_L3_CH3    (BIT(26))
#define GDMA_IN_REMAIN_UNDER_3B_L3_CH3_M  (BIT(26))
#define GDMA_IN_REMAIN_UNDER_3B_L3_CH3_V  0x1
#define GDMA_IN_REMAIN_UNDER_3B_L3_CH3_S  26
/* GDMA_IN_REMAIN_UNDER_2B_L3_CH3 : RO ;bitpos:[25] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_IN_REMAIN_UNDER_2B_L3_CH3    (BIT(25))
#define GDMA_IN_REMAIN_UNDER_2B_L3_CH3_M  (BIT(25))
#define GDMA_IN_REMAIN_UNDER_2B_L3_CH3_V  0x1
#define GDMA_IN_REMAIN_UNDER_2B_L3_CH3_S  25
/* GDMA_IN_REMAIN_UNDER_1B_L3_CH3 : RO ;bitpos:[24] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_IN_REMAIN_UNDER_1B_L3_CH3    (BIT(24))
#define GDMA_IN_REMAIN_UNDER_1B_L3_CH3_M  (BIT(24))
#define GDMA_IN_REMAIN_UNDER_1B_L3_CH3_V  0x1
#define GDMA_IN_REMAIN_UNDER_1B_L3_CH3_S  24
/* GDMA_INFIFO_CNT_L3_CH3 : RO ;bitpos:[23:19] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L3 Rx FIFO for Rx channel 3..*/
#define GDMA_INFIFO_CNT_L3_CH3    0x0000001F
#define GDMA_INFIFO_CNT_L3_CH3_M  ((GDMA_INFIFO_CNT_L3_CH3_V)<<(GDMA_INFIFO_CNT_L3_CH3_S))
#define GDMA_INFIFO_CNT_L3_CH3_V  0x1F
#define GDMA_INFIFO_CNT_L3_CH3_S  19
/* GDMA_INFIFO_CNT_L2_CH3 : RO ;bitpos:[18:12] ;default: 7'b0 ; */
/*description: The register stores the byte number of the data in L2 Rx FIFO for Rx channel 3..*/
#define GDMA_INFIFO_CNT_L2_CH3    0x0000007F
#define GDMA_INFIFO_CNT_L2_CH3_M  ((GDMA_INFIFO_CNT_L2_CH3_V)<<(GDMA_INFIFO_CNT_L2_CH3_S))
#define GDMA_INFIFO_CNT_L2_CH3_V  0x7F
#define GDMA_INFIFO_CNT_L2_CH3_S  12
/* GDMA_INFIFO_CNT_L1_CH3 : RO ;bitpos:[11:6] ;default: 6'b0 ; */
/*description: The register stores the byte number of the data in L1 Rx FIFO for Rx channel 3..*/
#define GDMA_INFIFO_CNT_L1_CH3    0x0000003F
#define GDMA_INFIFO_CNT_L1_CH3_M  ((GDMA_INFIFO_CNT_L1_CH3_V)<<(GDMA_INFIFO_CNT_L1_CH3_S))
#define GDMA_INFIFO_CNT_L1_CH3_V  0x3F
#define GDMA_INFIFO_CNT_L1_CH3_S  6
/* GDMA_INFIFO_EMPTY_L3_CH3 : RO ;bitpos:[5] ;default: 1'b1 ; */
/*description: L3 Rx FIFO empty signal for Rx channel 3..*/
#define GDMA_INFIFO_EMPTY_L3_CH3    (BIT(5))
#define GDMA_INFIFO_EMPTY_L3_CH3_M  (BIT(5))
#define GDMA_INFIFO_EMPTY_L3_CH3_V  0x1
#define GDMA_INFIFO_EMPTY_L3_CH3_S  5
/* GDMA_INFIFO_FULL_L3_CH3 : RO ;bitpos:[4] ;default: 1'b1 ; */
/*description: L3 Rx FIFO full signal for Rx channel 3..*/
#define GDMA_INFIFO_FULL_L3_CH3    (BIT(4))
#define GDMA_INFIFO_FULL_L3_CH3_M  (BIT(4))
#define GDMA_INFIFO_FULL_L3_CH3_V  0x1
#define GDMA_INFIFO_FULL_L3_CH3_S  4
/* GDMA_INFIFO_EMPTY_L2_CH3 : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: L2 Rx FIFO empty signal for Rx channel 3..*/
#define GDMA_INFIFO_EMPTY_L2_CH3    (BIT(3))
#define GDMA_INFIFO_EMPTY_L2_CH3_M  (BIT(3))
#define GDMA_INFIFO_EMPTY_L2_CH3_V  0x1
#define GDMA_INFIFO_EMPTY_L2_CH3_S  3
/* GDMA_INFIFO_FULL_L2_CH3 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: L2 Rx FIFO full signal for Rx channel 3..*/
#define GDMA_INFIFO_FULL_L2_CH3    (BIT(2))
#define GDMA_INFIFO_FULL_L2_CH3_M  (BIT(2))
#define GDMA_INFIFO_FULL_L2_CH3_V  0x1
#define GDMA_INFIFO_FULL_L2_CH3_S  2
/* GDMA_INFIFO_EMPTY_L1_CH3 : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: L1 Rx FIFO empty signal for Rx channel 3..*/
#define GDMA_INFIFO_EMPTY_L1_CH3    (BIT(1))
#define GDMA_INFIFO_EMPTY_L1_CH3_M  (BIT(1))
#define GDMA_INFIFO_EMPTY_L1_CH3_V  0x1
#define GDMA_INFIFO_EMPTY_L1_CH3_S  1
/* GDMA_INFIFO_FULL_L1_CH3 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: L1 Rx FIFO full signal for Rx channel 3..*/
#define GDMA_INFIFO_FULL_L1_CH3    (BIT(0))
#define GDMA_INFIFO_FULL_L1_CH3_M  (BIT(0))
#define GDMA_INFIFO_FULL_L1_CH3_V  0x1
#define GDMA_INFIFO_FULL_L1_CH3_S  0

#define GDMA_IN_POP_CH3_REG          (DR_REG_GDMA_BASE + 0x25C)
/* GDMA_INFIFO_POP_CH3 : R/W/SC ;bitpos:[12] ;default: 1'h0 ; */
/*description: Set this bit to pop data from DMA FIFO..*/
#define GDMA_INFIFO_POP_CH3    (BIT(12))
#define GDMA_INFIFO_POP_CH3_M  (BIT(12))
#define GDMA_INFIFO_POP_CH3_V  0x1
#define GDMA_INFIFO_POP_CH3_S  12
/* GDMA_INFIFO_RDATA_CH3 : RO ;bitpos:[11:0] ;default: 12'h800 ; */
/*description: This register stores the data popping from DMA FIFO..*/
#define GDMA_INFIFO_RDATA_CH3    0x00000FFF
#define GDMA_INFIFO_RDATA_CH3_M  ((GDMA_INFIFO_RDATA_CH3_V)<<(GDMA_INFIFO_RDATA_CH3_S))
#define GDMA_INFIFO_RDATA_CH3_V  0xFFF
#define GDMA_INFIFO_RDATA_CH3_S  0

#define GDMA_IN_LINK_CH3_REG          (DR_REG_GDMA_BASE + 0x260)
/* GDMA_INLINK_PARK_CH3 : RO ;bitpos:[24] ;default: 1'h1 ; */
/*description: 1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM
 is working..*/
#define GDMA_INLINK_PARK_CH3    (BIT(24))
#define GDMA_INLINK_PARK_CH3_M  (BIT(24))
#define GDMA_INLINK_PARK_CH3_V  0x1
#define GDMA_INLINK_PARK_CH3_S  24
/* GDMA_INLINK_RESTART_CH3 : R/W/SC ;bitpos:[23] ;default: 1'b0 ; */
/*description: Set this bit to mount a new inlink descriptor..*/
#define GDMA_INLINK_RESTART_CH3    (BIT(23))
#define GDMA_INLINK_RESTART_CH3_M  (BIT(23))
#define GDMA_INLINK_RESTART_CH3_V  0x1
#define GDMA_INLINK_RESTART_CH3_S  23
/* GDMA_INLINK_START_CH3 : R/W/SC ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to start dealing with the inlink descriptors..*/
#define GDMA_INLINK_START_CH3    (BIT(22))
#define GDMA_INLINK_START_CH3_M  (BIT(22))
#define GDMA_INLINK_START_CH3_V  0x1
#define GDMA_INLINK_START_CH3_S  22
/* GDMA_INLINK_STOP_CH3 : R/W/SC ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to stop dealing with the inlink descriptors..*/
#define GDMA_INLINK_STOP_CH3    (BIT(21))
#define GDMA_INLINK_STOP_CH3_M  (BIT(21))
#define GDMA_INLINK_STOP_CH3_V  0x1
#define GDMA_INLINK_STOP_CH3_S  21
/* GDMA_INLINK_AUTO_RET_CH3 : R/W ;bitpos:[20] ;default: 1'b1 ; */
/*description: Set this bit to return to current inlink descriptor's address, when there are
some errors in current receiving data..*/
#define GDMA_INLINK_AUTO_RET_CH3    (BIT(20))
#define GDMA_INLINK_AUTO_RET_CH3_M  (BIT(20))
#define GDMA_INLINK_AUTO_RET_CH3_V  0x1
#define GDMA_INLINK_AUTO_RET_CH3_S  20
/* GDMA_INLINK_ADDR_CH3 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: This register stores the 20 least significant bits of the first inlink
descriptor's address..*/
#define GDMA_INLINK_ADDR_CH3    0x000FFFFF
#define GDMA_INLINK_ADDR_CH3_M  ((GDMA_INLINK_ADDR_CH3_V)<<(GDMA_INLINK_ADDR_CH3_S))
#define GDMA_INLINK_ADDR_CH3_V  0xFFFFF
#define GDMA_INLINK_ADDR_CH3_S  0

#define GDMA_IN_STATE_CH3_REG          (DR_REG_GDMA_BASE + 0x264)
/* GDMA_IN_STATE_CH3 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: reserved.*/
#define GDMA_IN_STATE_CH3    0x00000007
#define GDMA_IN_STATE_CH3_M  ((GDMA_IN_STATE_CH3_V)<<(GDMA_IN_STATE_CH3_S))
#define GDMA_IN_STATE_CH3_V  0x7
#define GDMA_IN_STATE_CH3_S  20
/* GDMA_IN_DSCR_STATE_CH3 : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: reserved.*/
#define GDMA_IN_DSCR_STATE_CH3    0x00000003
#define GDMA_IN_DSCR_STATE_CH3_M  ((GDMA_IN_DSCR_STATE_CH3_V)<<(GDMA_IN_DSCR_STATE_CH3_S))
#define GDMA_IN_DSCR_STATE_CH3_V  0x3
#define GDMA_IN_DSCR_STATE_CH3_S  18
/* GDMA_INLINK_DSCR_ADDR_CH3 : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: This register stores the current inlink descriptor's address..*/
#define GDMA_INLINK_DSCR_ADDR_CH3    0x0003FFFF
#define GDMA_INLINK_DSCR_ADDR_CH3_M  ((GDMA_INLINK_DSCR_ADDR_CH3_V)<<(GDMA_INLINK_DSCR_ADDR_CH3_S))
#define GDMA_INLINK_DSCR_ADDR_CH3_V  0x3FFFF
#define GDMA_INLINK_DSCR_ADDR_CH3_S  0

#define GDMA_IN_SUC_EOF_DES_ADDR_CH3_REG          (DR_REG_GDMA_BASE + 0x268)
/* GDMA_IN_SUC_EOF_DES_ADDR_CH3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the inlink descriptor when the EOF bit in
this descriptor is 1..*/
#define GDMA_IN_SUC_EOF_DES_ADDR_CH3    0xFFFFFFFF
#define GDMA_IN_SUC_EOF_DES_ADDR_CH3_M  ((GDMA_IN_SUC_EOF_DES_ADDR_CH3_V)<<(GDMA_IN_SUC_EOF_DES_ADDR_CH3_S))
#define GDMA_IN_SUC_EOF_DES_ADDR_CH3_V  0xFFFFFFFF
#define GDMA_IN_SUC_EOF_DES_ADDR_CH3_S  0

#define GDMA_IN_ERR_EOF_DES_ADDR_CH3_REG          (DR_REG_GDMA_BASE + 0x26C)
/* GDMA_IN_ERR_EOF_DES_ADDR_CH3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the inlink descriptor when there are some
errors in current receiving data. Only used when peripheral is UHCI0..*/
#define GDMA_IN_ERR_EOF_DES_ADDR_CH3    0xFFFFFFFF
#define GDMA_IN_ERR_EOF_DES_ADDR_CH3_M  ((GDMA_IN_ERR_EOF_DES_ADDR_CH3_V)<<(GDMA_IN_ERR_EOF_DES_ADDR_CH3_S))
#define GDMA_IN_ERR_EOF_DES_ADDR_CH3_V  0xFFFFFFFF
#define GDMA_IN_ERR_EOF_DES_ADDR_CH3_S  0

#define GDMA_IN_DSCR_CH3_REG          (DR_REG_GDMA_BASE + 0x270)
/* GDMA_INLINK_DSCR_CH3 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the current inlink descriptor x..*/
#define GDMA_INLINK_DSCR_CH3    0xFFFFFFFF
#define GDMA_INLINK_DSCR_CH3_M  ((GDMA_INLINK_DSCR_CH3_V)<<(GDMA_INLINK_DSCR_CH3_S))
#define GDMA_INLINK_DSCR_CH3_V  0xFFFFFFFF
#define GDMA_INLINK_DSCR_CH3_S  0

#define GDMA_IN_DSCR_BF0_CH3_REG          (DR_REG_GDMA_BASE + 0x274)
/* GDMA_INLINK_DSCR_BF0_CH3 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the last inlink descriptor x-1..*/
#define GDMA_INLINK_DSCR_BF0_CH3    0xFFFFFFFF
#define GDMA_INLINK_DSCR_BF0_CH3_M  ((GDMA_INLINK_DSCR_BF0_CH3_V)<<(GDMA_INLINK_DSCR_BF0_CH3_S))
#define GDMA_INLINK_DSCR_BF0_CH3_V  0xFFFFFFFF
#define GDMA_INLINK_DSCR_BF0_CH3_S  0

#define GDMA_IN_DSCR_BF1_CH3_REG          (DR_REG_GDMA_BASE + 0x278)
/* GDMA_INLINK_DSCR_BF1_CH3 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the second-to-last inlink descriptor x-2..*/
#define GDMA_INLINK_DSCR_BF1_CH3    0xFFFFFFFF
#define GDMA_INLINK_DSCR_BF1_CH3_M  ((GDMA_INLINK_DSCR_BF1_CH3_V)<<(GDMA_INLINK_DSCR_BF1_CH3_S))
#define GDMA_INLINK_DSCR_BF1_CH3_V  0xFFFFFFFF
#define GDMA_INLINK_DSCR_BF1_CH3_S  0

#define GDMA_IN_WEIGHT_CH3_REG          (DR_REG_GDMA_BASE + 0x27C)
/* GDMA_RX_WEIGHT_CH3 : R/W ;bitpos:[11:8] ;default: 4'hf ; */
/*description: The weight of Rx channel 3. .*/
#define GDMA_RX_WEIGHT_CH3    0x0000000F
#define GDMA_RX_WEIGHT_CH3_M  ((GDMA_RX_WEIGHT_CH3_V)<<(GDMA_RX_WEIGHT_CH3_S))
#define GDMA_RX_WEIGHT_CH3_V  0xF
#define GDMA_RX_WEIGHT_CH3_S  8

#define GDMA_IN_PRI_CH3_REG          (DR_REG_GDMA_BASE + 0x284)
/* GDMA_RX_PRI_CH3 : R/W ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: The priority of Rx channel 3. The larger of the value, the higher of the
priority..*/
#define GDMA_RX_PRI_CH3    0x0000000F
#define GDMA_RX_PRI_CH3_M  ((GDMA_RX_PRI_CH3_V)<<(GDMA_RX_PRI_CH3_S))
#define GDMA_RX_PRI_CH3_V  0xF
#define GDMA_RX_PRI_CH3_S  0

#define GDMA_IN_PERI_SEL_CH3_REG          (DR_REG_GDMA_BASE + 0x288)
/* GDMA_PERI_IN_SEL_CH3 : R/W ;bitpos:[5:0] ;default: 6'h3f ; */
/*description: This register is used to select peripheral for Rx channel 3. 0:SPI2. 1: SPI3. 2:
 UHCI0. 3: I2S0. 4: I2S1. 5: LCD_CAM. 6: AES. 7: SHA. 8: ADC_DAC. 9: RMT..*/
#define GDMA_PERI_IN_SEL_CH3    0x0000003F
#define GDMA_PERI_IN_SEL_CH3_M  ((GDMA_PERI_IN_SEL_CH3_V)<<(GDMA_PERI_IN_SEL_CH3_S))
#define GDMA_PERI_IN_SEL_CH3_V  0x3F
#define GDMA_PERI_IN_SEL_CH3_S  0

#define GDMA_OUT_CONF0_CH3_REG          (DR_REG_GDMA_BASE + 0x2A0)
/* GDMA_OUT_DATA_BURST_EN_CH3 : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Tx channel 3 transmitting
data when accessing internal SRAM. .*/
#define GDMA_OUT_DATA_BURST_EN_CH3    (BIT(5))
#define GDMA_OUT_DATA_BURST_EN_CH3_M  (BIT(5))
#define GDMA_OUT_DATA_BURST_EN_CH3_V  0x1
#define GDMA_OUT_DATA_BURST_EN_CH3_S  5
/* GDMA_OUTDSCR_BURST_EN_CH3 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Tx channel 3 reading link
descriptor when accessing internal SRAM. .*/
#define GDMA_OUTDSCR_BURST_EN_CH3    (BIT(4))
#define GDMA_OUTDSCR_BURST_EN_CH3_M  (BIT(4))
#define GDMA_OUTDSCR_BURST_EN_CH3_V  0x1
#define GDMA_OUTDSCR_BURST_EN_CH3_S  4
/* GDMA_OUT_EOF_MODE_CH3 : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel 3 is
 generated when data need to transmit has been popped from FIFO in DMA.*/
#define GDMA_OUT_EOF_MODE_CH3    (BIT(3))
#define GDMA_OUT_EOF_MODE_CH3_M  (BIT(3))
#define GDMA_OUT_EOF_MODE_CH3_V  0x1
#define GDMA_OUT_EOF_MODE_CH3_S  3
/* GDMA_OUT_AUTO_WRBACK_CH3 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to enable automatic outlink-writeback when all the data in tx
buffer has been transmitted..*/
#define GDMA_OUT_AUTO_WRBACK_CH3    (BIT(2))
#define GDMA_OUT_AUTO_WRBACK_CH3_M  (BIT(2))
#define GDMA_OUT_AUTO_WRBACK_CH3_V  0x1
#define GDMA_OUT_AUTO_WRBACK_CH3_S  2
/* GDMA_OUT_LOOP_TEST_CH3 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: reserved.*/
#define GDMA_OUT_LOOP_TEST_CH3    (BIT(1))
#define GDMA_OUT_LOOP_TEST_CH3_M  (BIT(1))
#define GDMA_OUT_LOOP_TEST_CH3_V  0x1
#define GDMA_OUT_LOOP_TEST_CH3_S  1
/* GDMA_OUT_RST_CH3 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: This bit is used to reset DMA channel 3 Tx FSM and Tx FIFO pointer..*/
#define GDMA_OUT_RST_CH3    (BIT(0))
#define GDMA_OUT_RST_CH3_M  (BIT(0))
#define GDMA_OUT_RST_CH3_V  0x1
#define GDMA_OUT_RST_CH3_S  0

#define GDMA_OUT_CONF1_CH3_REG          (DR_REG_GDMA_BASE + 0x2A4)
/* GDMA_OUT_EXT_MEM_BK_SIZE_CH3 : R/W ;bitpos:[14:13] ;default: 2'b0 ; */
/*description: Block size of Tx channel 3 when DMA access external SRAM. 0: 16 bytes      1: 32
 bytes    2/3:reserved.*/
#define GDMA_OUT_EXT_MEM_BK_SIZE_CH3    0x00000003
#define GDMA_OUT_EXT_MEM_BK_SIZE_CH3_M  ((GDMA_OUT_EXT_MEM_BK_SIZE_CH3_V)<<(GDMA_OUT_EXT_MEM_BK_SIZE_CH3_S))
#define GDMA_OUT_EXT_MEM_BK_SIZE_CH3_V  0x3
#define GDMA_OUT_EXT_MEM_BK_SIZE_CH3_S  13
/* GDMA_OUT_CHECK_OWNER_CH3 : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to enable checking the owner attribute of the link descriptor..*/
#define GDMA_OUT_CHECK_OWNER_CH3    (BIT(12))
#define GDMA_OUT_CHECK_OWNER_CH3_M  (BIT(12))
#define GDMA_OUT_CHECK_OWNER_CH3_V  0x1
#define GDMA_OUT_CHECK_OWNER_CH3_S  12

#define GDMA_OUT_INT_RAW_CH3_REG          (DR_REG_GDMA_BASE + 0x2A8)
/* GDMA_OUTFIFO_UDF_L3_CH3_INT_RAW : R/WTC/SS ;bitpos:[7] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo of Tx channel 3 is
underflow. .*/
#define GDMA_OUTFIFO_UDF_L3_CH3_INT_RAW    (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH3_INT_RAW_M  (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH3_INT_RAW_V  0x1
#define GDMA_OUTFIFO_UDF_L3_CH3_INT_RAW_S  7
/* GDMA_OUTFIFO_OVF_L3_CH3_INT_RAW : R/WTC/SS ;bitpos:[6] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo of Tx channel 3 is
overflow. .*/
#define GDMA_OUTFIFO_OVF_L3_CH3_INT_RAW    (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH3_INT_RAW_M  (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH3_INT_RAW_V  0x1
#define GDMA_OUTFIFO_OVF_L3_CH3_INT_RAW_S  6
/* GDMA_OUTFIFO_UDF_L1_CH3_INT_RAW : R/WTC/SS ;bitpos:[5] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo of Tx channel 3 is
underflow. .*/
#define GDMA_OUTFIFO_UDF_L1_CH3_INT_RAW    (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH3_INT_RAW_M  (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH3_INT_RAW_V  0x1
#define GDMA_OUTFIFO_UDF_L1_CH3_INT_RAW_S  5
/* GDMA_OUTFIFO_OVF_L1_CH3_INT_RAW : R/WTC/SS ;bitpos:[4] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo of Tx channel 3 is
overflow. .*/
#define GDMA_OUTFIFO_OVF_L1_CH3_INT_RAW    (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH3_INT_RAW_M  (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH3_INT_RAW_V  0x1
#define GDMA_OUTFIFO_OVF_L1_CH3_INT_RAW_S  4
/* GDMA_OUT_TOTAL_EOF_CH3_INT_RAW : R/WTC/SS ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when data corresponding a outlink
(includes one link descriptor or few link descriptors) is transmitted out for Tx cha
nnel 3..*/
#define GDMA_OUT_TOTAL_EOF_CH3_INT_RAW    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH3_INT_RAW_M  (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH3_INT_RAW_V  0x1
#define GDMA_OUT_TOTAL_EOF_CH3_INT_RAW_S  3
/* GDMA_OUT_DSCR_ERR_CH3_INT_RAW : R/WTC/SS ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when detecting outlink descriptor error
r, including owner error, the second and third word error of outlink descriptor
for Tx channel 3..*/
#define GDMA_OUT_DSCR_ERR_CH3_INT_RAW    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH3_INT_RAW_M  (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH3_INT_RAW_V  0x1
#define GDMA_OUT_DSCR_ERR_CH3_INT_RAW_S  2
/* GDMA_OUT_EOF_CH3_INT_RAW : R/WTC/SS ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data pointed by one
outlink descriptor has been read from memory for Tx channel 3. .*/
#define GDMA_OUT_EOF_CH3_INT_RAW    (BIT(1))
#define GDMA_OUT_EOF_CH3_INT_RAW_M  (BIT(1))
#define GDMA_OUT_EOF_CH3_INT_RAW_V  0x1
#define GDMA_OUT_EOF_CH3_INT_RAW_S  1
/* GDMA_OUT_DONE_CH3_INT_RAW : R/WTC/SS ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data pointed by one
outlink descriptor has been transmitted to peripherals for Tx channel 3..*/
#define GDMA_OUT_DONE_CH3_INT_RAW    (BIT(0))
#define GDMA_OUT_DONE_CH3_INT_RAW_M  (BIT(0))
#define GDMA_OUT_DONE_CH3_INT_RAW_V  0x1
#define GDMA_OUT_DONE_CH3_INT_RAW_S  0

#define GDMA_OUT_INT_ST_CH3_REG          (DR_REG_GDMA_BASE + 0x2AC)
/* GDMA_OUTFIFO_UDF_L3_CH3_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L3_CH3_INT_ST    (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH3_INT_ST_M  (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH3_INT_ST_V  0x1
#define GDMA_OUTFIFO_UDF_L3_CH3_INT_ST_S  7
/* GDMA_OUTFIFO_OVF_L3_CH3_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L3_CH3_INT_ST    (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH3_INT_ST_M  (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH3_INT_ST_V  0x1
#define GDMA_OUTFIFO_OVF_L3_CH3_INT_ST_S  6
/* GDMA_OUTFIFO_UDF_L1_CH3_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L1_CH3_INT_ST    (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH3_INT_ST_M  (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH3_INT_ST_V  0x1
#define GDMA_OUTFIFO_UDF_L1_CH3_INT_ST_S  5
/* GDMA_OUTFIFO_OVF_L1_CH3_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L1_CH3_INT_ST    (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH3_INT_ST_M  (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH3_INT_ST_V  0x1
#define GDMA_OUTFIFO_OVF_L1_CH3_INT_ST_S  4
/* GDMA_OUT_TOTAL_EOF_CH3_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt..*/
#define GDMA_OUT_TOTAL_EOF_CH3_INT_ST    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH3_INT_ST_M  (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH3_INT_ST_V  0x1
#define GDMA_OUT_TOTAL_EOF_CH3_INT_ST_S  3
/* GDMA_OUT_DSCR_ERR_CH3_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_OUT_DSCR_ERR_CH3_INT_ST    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH3_INT_ST_M  (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH3_INT_ST_V  0x1
#define GDMA_OUT_DSCR_ERR_CH3_INT_ST_S  2
/* GDMA_OUT_EOF_CH3_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_EOF_CH_INT interrupt..*/
#define GDMA_OUT_EOF_CH3_INT_ST    (BIT(1))
#define GDMA_OUT_EOF_CH3_INT_ST_M  (BIT(1))
#define GDMA_OUT_EOF_CH3_INT_ST_V  0x1
#define GDMA_OUT_EOF_CH3_INT_ST_S  1
/* GDMA_OUT_DONE_CH3_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_DONE_CH_INT interrupt..*/
#define GDMA_OUT_DONE_CH3_INT_ST    (BIT(0))
#define GDMA_OUT_DONE_CH3_INT_ST_M  (BIT(0))
#define GDMA_OUT_DONE_CH3_INT_ST_V  0x1
#define GDMA_OUT_DONE_CH3_INT_ST_S  0

#define GDMA_OUT_INT_ENA_CH3_REG          (DR_REG_GDMA_BASE + 0x2B0)
/* GDMA_OUTFIFO_UDF_L3_CH3_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L3_CH3_INT_ENA    (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH3_INT_ENA_M  (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH3_INT_ENA_V  0x1
#define GDMA_OUTFIFO_UDF_L3_CH3_INT_ENA_S  7
/* GDMA_OUTFIFO_OVF_L3_CH3_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L3_CH3_INT_ENA    (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH3_INT_ENA_M  (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH3_INT_ENA_V  0x1
#define GDMA_OUTFIFO_OVF_L3_CH3_INT_ENA_S  6
/* GDMA_OUTFIFO_UDF_L1_CH3_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L1_CH3_INT_ENA    (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH3_INT_ENA_M  (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH3_INT_ENA_V  0x1
#define GDMA_OUTFIFO_UDF_L1_CH3_INT_ENA_S  5
/* GDMA_OUTFIFO_OVF_L1_CH3_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L1_CH3_INT_ENA    (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH3_INT_ENA_M  (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH3_INT_ENA_V  0x1
#define GDMA_OUTFIFO_OVF_L1_CH3_INT_ENA_S  4
/* GDMA_OUT_TOTAL_EOF_CH3_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt..*/
#define GDMA_OUT_TOTAL_EOF_CH3_INT_ENA    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH3_INT_ENA_M  (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH3_INT_ENA_V  0x1
#define GDMA_OUT_TOTAL_EOF_CH3_INT_ENA_S  3
/* GDMA_OUT_DSCR_ERR_CH3_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_OUT_DSCR_ERR_CH3_INT_ENA    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH3_INT_ENA_M  (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH3_INT_ENA_V  0x1
#define GDMA_OUT_DSCR_ERR_CH3_INT_ENA_S  2
/* GDMA_OUT_EOF_CH3_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_EOF_CH_INT interrupt..*/
#define GDMA_OUT_EOF_CH3_INT_ENA    (BIT(1))
#define GDMA_OUT_EOF_CH3_INT_ENA_M  (BIT(1))
#define GDMA_OUT_EOF_CH3_INT_ENA_V  0x1
#define GDMA_OUT_EOF_CH3_INT_ENA_S  1
/* GDMA_OUT_DONE_CH3_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_DONE_CH_INT interrupt..*/
#define GDMA_OUT_DONE_CH3_INT_ENA    (BIT(0))
#define GDMA_OUT_DONE_CH3_INT_ENA_M  (BIT(0))
#define GDMA_OUT_DONE_CH3_INT_ENA_V  0x1
#define GDMA_OUT_DONE_CH3_INT_ENA_S  0

#define GDMA_OUT_INT_CLR_CH3_REG          (DR_REG_GDMA_BASE + 0x2B4)
/* GDMA_OUTFIFO_UDF_L3_CH3_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L3_CH3_INT_CLR    (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH3_INT_CLR_M  (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH3_INT_CLR_V  0x1
#define GDMA_OUTFIFO_UDF_L3_CH3_INT_CLR_S  7
/* GDMA_OUTFIFO_OVF_L3_CH3_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L3_CH3_INT_CLR    (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH3_INT_CLR_M  (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH3_INT_CLR_V  0x1
#define GDMA_OUTFIFO_OVF_L3_CH3_INT_CLR_S  6
/* GDMA_OUTFIFO_UDF_L1_CH3_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L1_CH3_INT_CLR    (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH3_INT_CLR_M  (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH3_INT_CLR_V  0x1
#define GDMA_OUTFIFO_UDF_L1_CH3_INT_CLR_S  5
/* GDMA_OUTFIFO_OVF_L1_CH3_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L1_CH3_INT_CLR    (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH3_INT_CLR_M  (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH3_INT_CLR_V  0x1
#define GDMA_OUTFIFO_OVF_L1_CH3_INT_CLR_S  4
/* GDMA_OUT_TOTAL_EOF_CH3_INT_CLR : WT ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt..*/
#define GDMA_OUT_TOTAL_EOF_CH3_INT_CLR    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH3_INT_CLR_M  (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH3_INT_CLR_V  0x1
#define GDMA_OUT_TOTAL_EOF_CH3_INT_CLR_S  3
/* GDMA_OUT_DSCR_ERR_CH3_INT_CLR : WT ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_OUT_DSCR_ERR_CH3_INT_CLR    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH3_INT_CLR_M  (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH3_INT_CLR_V  0x1
#define GDMA_OUT_DSCR_ERR_CH3_INT_CLR_S  2
/* GDMA_OUT_EOF_CH3_INT_CLR : WT ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_EOF_CH_INT interrupt..*/
#define GDMA_OUT_EOF_CH3_INT_CLR    (BIT(1))
#define GDMA_OUT_EOF_CH3_INT_CLR_M  (BIT(1))
#define GDMA_OUT_EOF_CH3_INT_CLR_V  0x1
#define GDMA_OUT_EOF_CH3_INT_CLR_S  1
/* GDMA_OUT_DONE_CH3_INT_CLR : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_DONE_CH_INT interrupt..*/
#define GDMA_OUT_DONE_CH3_INT_CLR    (BIT(0))
#define GDMA_OUT_DONE_CH3_INT_CLR_M  (BIT(0))
#define GDMA_OUT_DONE_CH3_INT_CLR_V  0x1
#define GDMA_OUT_DONE_CH3_INT_CLR_S  0

#define GDMA_OUTFIFO_STATUS_CH3_REG          (DR_REG_GDMA_BASE + 0x2B8)
/* GDMA_OUT_REMAIN_UNDER_4B_L3_CH3 : RO ;bitpos:[26] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_OUT_REMAIN_UNDER_4B_L3_CH3    (BIT(26))
#define GDMA_OUT_REMAIN_UNDER_4B_L3_CH3_M  (BIT(26))
#define GDMA_OUT_REMAIN_UNDER_4B_L3_CH3_V  0x1
#define GDMA_OUT_REMAIN_UNDER_4B_L3_CH3_S  26
/* GDMA_OUT_REMAIN_UNDER_3B_L3_CH3 : RO ;bitpos:[25] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_OUT_REMAIN_UNDER_3B_L3_CH3    (BIT(25))
#define GDMA_OUT_REMAIN_UNDER_3B_L3_CH3_M  (BIT(25))
#define GDMA_OUT_REMAIN_UNDER_3B_L3_CH3_V  0x1
#define GDMA_OUT_REMAIN_UNDER_3B_L3_CH3_S  25
/* GDMA_OUT_REMAIN_UNDER_2B_L3_CH3 : RO ;bitpos:[24] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_OUT_REMAIN_UNDER_2B_L3_CH3    (BIT(24))
#define GDMA_OUT_REMAIN_UNDER_2B_L3_CH3_M  (BIT(24))
#define GDMA_OUT_REMAIN_UNDER_2B_L3_CH3_V  0x1
#define GDMA_OUT_REMAIN_UNDER_2B_L3_CH3_S  24
/* GDMA_OUT_REMAIN_UNDER_1B_L3_CH3 : RO ;bitpos:[23] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_OUT_REMAIN_UNDER_1B_L3_CH3    (BIT(23))
#define GDMA_OUT_REMAIN_UNDER_1B_L3_CH3_M  (BIT(23))
#define GDMA_OUT_REMAIN_UNDER_1B_L3_CH3_V  0x1
#define GDMA_OUT_REMAIN_UNDER_1B_L3_CH3_S  23
/* GDMA_OUTFIFO_CNT_L3_CH3 : RO ;bitpos:[22:18] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L3 Tx FIFO for Tx channel 3..*/
#define GDMA_OUTFIFO_CNT_L3_CH3    0x0000001F
#define GDMA_OUTFIFO_CNT_L3_CH3_M  ((GDMA_OUTFIFO_CNT_L3_CH3_V)<<(GDMA_OUTFIFO_CNT_L3_CH3_S))
#define GDMA_OUTFIFO_CNT_L3_CH3_V  0x1F
#define GDMA_OUTFIFO_CNT_L3_CH3_S  18
/* GDMA_OUTFIFO_CNT_L2_CH3 : RO ;bitpos:[17:11] ;default: 7'b0 ; */
/*description: The register stores the byte number of the data in L2 Tx FIFO for Tx channel 3..*/
#define GDMA_OUTFIFO_CNT_L2_CH3    0x0000007F
#define GDMA_OUTFIFO_CNT_L2_CH3_M  ((GDMA_OUTFIFO_CNT_L2_CH3_V)<<(GDMA_OUTFIFO_CNT_L2_CH3_S))
#define GDMA_OUTFIFO_CNT_L2_CH3_V  0x7F
#define GDMA_OUTFIFO_CNT_L2_CH3_S  11
/* GDMA_OUTFIFO_CNT_L1_CH3 : RO ;bitpos:[10:6] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L1 Tx FIFO for Tx channel 3..*/
#define GDMA_OUTFIFO_CNT_L1_CH3    0x0000001F
#define GDMA_OUTFIFO_CNT_L1_CH3_M  ((GDMA_OUTFIFO_CNT_L1_CH3_V)<<(GDMA_OUTFIFO_CNT_L1_CH3_S))
#define GDMA_OUTFIFO_CNT_L1_CH3_V  0x1F
#define GDMA_OUTFIFO_CNT_L1_CH3_S  6
/* GDMA_OUTFIFO_EMPTY_L3_CH3 : RO ;bitpos:[5] ;default: 1'b1 ; */
/*description: L3 Tx FIFO empty signal for Tx channel 3..*/
#define GDMA_OUTFIFO_EMPTY_L3_CH3    (BIT(5))
#define GDMA_OUTFIFO_EMPTY_L3_CH3_M  (BIT(5))
#define GDMA_OUTFIFO_EMPTY_L3_CH3_V  0x1
#define GDMA_OUTFIFO_EMPTY_L3_CH3_S  5
/* GDMA_OUTFIFO_FULL_L3_CH3 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: L3 Tx FIFO full signal for Tx channel 3..*/
#define GDMA_OUTFIFO_FULL_L3_CH3    (BIT(4))
#define GDMA_OUTFIFO_FULL_L3_CH3_M  (BIT(4))
#define GDMA_OUTFIFO_FULL_L3_CH3_V  0x1
#define GDMA_OUTFIFO_FULL_L3_CH3_S  4
/* GDMA_OUTFIFO_EMPTY_L2_CH3 : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: L2 Tx FIFO empty signal for Tx channel 3..*/
#define GDMA_OUTFIFO_EMPTY_L2_CH3    (BIT(3))
#define GDMA_OUTFIFO_EMPTY_L2_CH3_M  (BIT(3))
#define GDMA_OUTFIFO_EMPTY_L2_CH3_V  0x1
#define GDMA_OUTFIFO_EMPTY_L2_CH3_S  3
/* GDMA_OUTFIFO_FULL_L2_CH3 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: L2 Tx FIFO full signal for Tx channel 3..*/
#define GDMA_OUTFIFO_FULL_L2_CH3    (BIT(2))
#define GDMA_OUTFIFO_FULL_L2_CH3_M  (BIT(2))
#define GDMA_OUTFIFO_FULL_L2_CH3_V  0x1
#define GDMA_OUTFIFO_FULL_L2_CH3_S  2
/* GDMA_OUTFIFO_EMPTY_L1_CH3 : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: L1 Tx FIFO empty signal for Tx channel 3..*/
#define GDMA_OUTFIFO_EMPTY_L1_CH3    (BIT(1))
#define GDMA_OUTFIFO_EMPTY_L1_CH3_M  (BIT(1))
#define GDMA_OUTFIFO_EMPTY_L1_CH3_V  0x1
#define GDMA_OUTFIFO_EMPTY_L1_CH3_S  1
/* GDMA_OUTFIFO_FULL_L1_CH3 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: L1 Tx FIFO full signal for Tx channel 3..*/
#define GDMA_OUTFIFO_FULL_L1_CH3    (BIT(0))
#define GDMA_OUTFIFO_FULL_L1_CH3_M  (BIT(0))
#define GDMA_OUTFIFO_FULL_L1_CH3_V  0x1
#define GDMA_OUTFIFO_FULL_L1_CH3_S  0

#define GDMA_OUT_PUSH_CH3_REG          (DR_REG_GDMA_BASE + 0x2BC)
/* GDMA_OUTFIFO_PUSH_CH3 : R/W/SC ;bitpos:[9] ;default: 1'h0 ; */
/*description: Set this bit to push data into DMA FIFO..*/
#define GDMA_OUTFIFO_PUSH_CH3    (BIT(9))
#define GDMA_OUTFIFO_PUSH_CH3_M  (BIT(9))
#define GDMA_OUTFIFO_PUSH_CH3_V  0x1
#define GDMA_OUTFIFO_PUSH_CH3_S  9
/* GDMA_OUTFIFO_WDATA_CH3 : R/W ;bitpos:[8:0] ;default: 9'h0 ; */
/*description: This register stores the data that need to be pushed into DMA FIFO..*/
#define GDMA_OUTFIFO_WDATA_CH3    0x000001FF
#define GDMA_OUTFIFO_WDATA_CH3_M  ((GDMA_OUTFIFO_WDATA_CH3_V)<<(GDMA_OUTFIFO_WDATA_CH3_S))
#define GDMA_OUTFIFO_WDATA_CH3_V  0x1FF
#define GDMA_OUTFIFO_WDATA_CH3_S  0

#define GDMA_OUT_LINK_CH3_REG          (DR_REG_GDMA_BASE + 0x2C0)
/* GDMA_OUTLINK_PARK_CH3 : RO ;bitpos:[23] ;default: 1'h1 ; */
/*description: 1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's
FSM is working..*/
#define GDMA_OUTLINK_PARK_CH3    (BIT(23))
#define GDMA_OUTLINK_PARK_CH3_M  (BIT(23))
#define GDMA_OUTLINK_PARK_CH3_V  0x1
#define GDMA_OUTLINK_PARK_CH3_S  23
/* GDMA_OUTLINK_RESTART_CH3 : R/W/SC ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to restart a new outlink from the last address. .*/
#define GDMA_OUTLINK_RESTART_CH3    (BIT(22))
#define GDMA_OUTLINK_RESTART_CH3_M  (BIT(22))
#define GDMA_OUTLINK_RESTART_CH3_V  0x1
#define GDMA_OUTLINK_RESTART_CH3_S  22
/* GDMA_OUTLINK_START_CH3 : R/W/SC ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to start dealing with the outlink descriptors..*/
#define GDMA_OUTLINK_START_CH3    (BIT(21))
#define GDMA_OUTLINK_START_CH3_M  (BIT(21))
#define GDMA_OUTLINK_START_CH3_V  0x1
#define GDMA_OUTLINK_START_CH3_S  21
/* GDMA_OUTLINK_STOP_CH3 : R/W/SC ;bitpos:[20] ;default: 1'b0 ; */
/*description: Set this bit to stop dealing with the outlink descriptors..*/
#define GDMA_OUTLINK_STOP_CH3    (BIT(20))
#define GDMA_OUTLINK_STOP_CH3_M  (BIT(20))
#define GDMA_OUTLINK_STOP_CH3_V  0x1
#define GDMA_OUTLINK_STOP_CH3_S  20
/* GDMA_OUTLINK_ADDR_CH3 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: This register stores the 20 least significant bits of the first outlink
descriptor's address..*/
#define GDMA_OUTLINK_ADDR_CH3    0x000FFFFF
#define GDMA_OUTLINK_ADDR_CH3_M  ((GDMA_OUTLINK_ADDR_CH3_V)<<(GDMA_OUTLINK_ADDR_CH3_S))
#define GDMA_OUTLINK_ADDR_CH3_V  0xFFFFF
#define GDMA_OUTLINK_ADDR_CH3_S  0

#define GDMA_OUT_STATE_CH3_REG          (DR_REG_GDMA_BASE + 0x2C4)
/* GDMA_OUT_STATE_CH3 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: reserved.*/
#define GDMA_OUT_STATE_CH3    0x00000007
#define GDMA_OUT_STATE_CH3_M  ((GDMA_OUT_STATE_CH3_V)<<(GDMA_OUT_STATE_CH3_S))
#define GDMA_OUT_STATE_CH3_V  0x7
#define GDMA_OUT_STATE_CH3_S  20
/* GDMA_OUT_DSCR_STATE_CH3 : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: reserved.*/
#define GDMA_OUT_DSCR_STATE_CH3    0x00000003
#define GDMA_OUT_DSCR_STATE_CH3_M  ((GDMA_OUT_DSCR_STATE_CH3_V)<<(GDMA_OUT_DSCR_STATE_CH3_S))
#define GDMA_OUT_DSCR_STATE_CH3_V  0x3
#define GDMA_OUT_DSCR_STATE_CH3_S  18
/* GDMA_OUTLINK_DSCR_ADDR_CH3 : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: This register stores the current outlink descriptor's address..*/
#define GDMA_OUTLINK_DSCR_ADDR_CH3    0x0003FFFF
#define GDMA_OUTLINK_DSCR_ADDR_CH3_M  ((GDMA_OUTLINK_DSCR_ADDR_CH3_V)<<(GDMA_OUTLINK_DSCR_ADDR_CH3_S))
#define GDMA_OUTLINK_DSCR_ADDR_CH3_V  0x3FFFF
#define GDMA_OUTLINK_DSCR_ADDR_CH3_S  0

#define GDMA_OUT_EOF_DES_ADDR_CH3_REG          (DR_REG_GDMA_BASE + 0x2C8)
/* GDMA_OUT_EOF_DES_ADDR_CH3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the outlink descriptor when the EOF bit in
this descriptor is 1..*/
#define GDMA_OUT_EOF_DES_ADDR_CH3    0xFFFFFFFF
#define GDMA_OUT_EOF_DES_ADDR_CH3_M  ((GDMA_OUT_EOF_DES_ADDR_CH3_V)<<(GDMA_OUT_EOF_DES_ADDR_CH3_S))
#define GDMA_OUT_EOF_DES_ADDR_CH3_V  0xFFFFFFFF
#define GDMA_OUT_EOF_DES_ADDR_CH3_S  0

#define GDMA_OUT_EOF_BFR_DES_ADDR_CH3_REG          (DR_REG_GDMA_BASE + 0x2CC)
/* GDMA_OUT_EOF_BFR_DES_ADDR_CH3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the outlink descriptor before the last
outlink descriptor..*/
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH3    0xFFFFFFFF
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH3_M  ((GDMA_OUT_EOF_BFR_DES_ADDR_CH3_V)<<(GDMA_OUT_EOF_BFR_DES_ADDR_CH3_S))
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH3_V  0xFFFFFFFF
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH3_S  0

#define GDMA_OUT_DSCR_CH3_REG          (DR_REG_GDMA_BASE + 0x2D0)
/* GDMA_OUTLINK_DSCR_CH3 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the current outlink descriptor y..*/
#define GDMA_OUTLINK_DSCR_CH3    0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_CH3_M  ((GDMA_OUTLINK_DSCR_CH3_V)<<(GDMA_OUTLINK_DSCR_CH3_S))
#define GDMA_OUTLINK_DSCR_CH3_V  0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_CH3_S  0

#define GDMA_OUT_DSCR_BF0_CH3_REG          (DR_REG_GDMA_BASE + 0x2D4)
/* GDMA_OUTLINK_DSCR_BF0_CH3 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the last outlink descriptor y-1..*/
#define GDMA_OUTLINK_DSCR_BF0_CH3    0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_BF0_CH3_M  ((GDMA_OUTLINK_DSCR_BF0_CH3_V)<<(GDMA_OUTLINK_DSCR_BF0_CH3_S))
#define GDMA_OUTLINK_DSCR_BF0_CH3_V  0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_BF0_CH3_S  0

#define GDMA_OUT_DSCR_BF1_CH3_REG          (DR_REG_GDMA_BASE + 0x2D8)
/* GDMA_OUTLINK_DSCR_BF1_CH3 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the second-to-last inlink descriptor x-2..*/
#define GDMA_OUTLINK_DSCR_BF1_CH3    0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_BF1_CH3_M  ((GDMA_OUTLINK_DSCR_BF1_CH3_V)<<(GDMA_OUTLINK_DSCR_BF1_CH3_S))
#define GDMA_OUTLINK_DSCR_BF1_CH3_V  0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_BF1_CH3_S  0

#define GDMA_OUT_WEIGHT_CH3_REG          (DR_REG_GDMA_BASE + 0x2DC)
/* GDMA_TX_WEIGHT_CH3 : R/W ;bitpos:[11:8] ;default: 4'hf ; */
/*description: The weight of Tx channel 3. .*/
#define GDMA_TX_WEIGHT_CH3    0x0000000F
#define GDMA_TX_WEIGHT_CH3_M  ((GDMA_TX_WEIGHT_CH3_V)<<(GDMA_TX_WEIGHT_CH3_S))
#define GDMA_TX_WEIGHT_CH3_V  0xF
#define GDMA_TX_WEIGHT_CH3_S  8

#define GDMA_OUT_PRI_CH3_REG          (DR_REG_GDMA_BASE + 0x2E4)
/* GDMA_TX_PRI_CH3 : R/W ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: The priority of Tx channel 3. The larger of the value, the higher of the
priority..*/
#define GDMA_TX_PRI_CH3    0x0000000F
#define GDMA_TX_PRI_CH3_M  ((GDMA_TX_PRI_CH3_V)<<(GDMA_TX_PRI_CH3_S))
#define GDMA_TX_PRI_CH3_V  0xF
#define GDMA_TX_PRI_CH3_S  0

#define GDMA_OUT_PERI_SEL_CH3_REG          (DR_REG_GDMA_BASE + 0x2E8)
/* GDMA_PERI_OUT_SEL_CH3 : R/W ;bitpos:[5:0] ;default: 6'h3f ; */
/*description: This register is used to select peripheral for Tx channel 3. 0:SPI2. 1: SPI3. 2:
 UHCI0. 3: I2S0. 4: I2S1. 5: LCD_CAM. 6: AES. 7: SHA. 8: ADC_DAC. 9: RMT.; 7: AE
S. 8: SHA. 9: ADC_DAC..*/
#define GDMA_PERI_OUT_SEL_CH3    0x0000003F
#define GDMA_PERI_OUT_SEL_CH3_M  ((GDMA_PERI_OUT_SEL_CH3_V)<<(GDMA_PERI_OUT_SEL_CH3_S))
#define GDMA_PERI_OUT_SEL_CH3_V  0x3F
#define GDMA_PERI_OUT_SEL_CH3_S  0

#define GDMA_IN_CONF0_CH4_REG          (DR_REG_GDMA_BASE + 0x300)
/* GDMA_MEM_TRANS_EN_CH4 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit 1 to enable automatic transmitting data from memory to memory via
DMA..*/
#define GDMA_MEM_TRANS_EN_CH4    (BIT(4))
#define GDMA_MEM_TRANS_EN_CH4_M  (BIT(4))
#define GDMA_MEM_TRANS_EN_CH4_V  0x1
#define GDMA_MEM_TRANS_EN_CH4_S  4
/* GDMA_IN_DATA_BURST_EN_CH4 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Rx channel 2 receiving data
when accessing internal SRAM. .*/
#define GDMA_IN_DATA_BURST_EN_CH4    (BIT(3))
#define GDMA_IN_DATA_BURST_EN_CH4_M  (BIT(3))
#define GDMA_IN_DATA_BURST_EN_CH4_V  0x1
#define GDMA_IN_DATA_BURST_EN_CH4_S  3
/* GDMA_INDSCR_BURST_EN_CH4 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Rx channel 2 reading link
descriptor when accessing internal SRAM. .*/
#define GDMA_INDSCR_BURST_EN_CH4    (BIT(2))
#define GDMA_INDSCR_BURST_EN_CH4_M  (BIT(2))
#define GDMA_INDSCR_BURST_EN_CH4_V  0x1
#define GDMA_INDSCR_BURST_EN_CH4_S  2
/* GDMA_IN_LOOP_TEST_CH4 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: reserved.*/
#define GDMA_IN_LOOP_TEST_CH4    (BIT(1))
#define GDMA_IN_LOOP_TEST_CH4_M  (BIT(1))
#define GDMA_IN_LOOP_TEST_CH4_V  0x1
#define GDMA_IN_LOOP_TEST_CH4_S  1
/* GDMA_IN_RST_CH4 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: This bit is used to reset DMA channel 4 Rx FSM and Rx FIFO pointer..*/
#define GDMA_IN_RST_CH4    (BIT(0))
#define GDMA_IN_RST_CH4_M  (BIT(0))
#define GDMA_IN_RST_CH4_V  0x1
#define GDMA_IN_RST_CH4_S  0

#define GDMA_IN_CONF1_CH4_REG          (DR_REG_GDMA_BASE + 0x304)
/* GDMA_IN_EXT_MEM_BK_SIZE_CH4 : R/W ;bitpos:[14:13] ;default: 2'b0 ; */
/*description: Block size of Rx channel 4 when DMA access external SRAM. 0: 16 bytes      1: 32
 bytes    2/3:reserved.*/
#define GDMA_IN_EXT_MEM_BK_SIZE_CH4    0x00000003
#define GDMA_IN_EXT_MEM_BK_SIZE_CH4_M  ((GDMA_IN_EXT_MEM_BK_SIZE_CH4_V)<<(GDMA_IN_EXT_MEM_BK_SIZE_CH4_S))
#define GDMA_IN_EXT_MEM_BK_SIZE_CH4_V  0x3
#define GDMA_IN_EXT_MEM_BK_SIZE_CH4_S  13
/* GDMA_IN_CHECK_OWNER_CH4 : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to enable checking the owner attribute of the link descriptor..*/
#define GDMA_IN_CHECK_OWNER_CH4    (BIT(12))
#define GDMA_IN_CHECK_OWNER_CH4_M  (BIT(12))
#define GDMA_IN_CHECK_OWNER_CH4_V  0x1
#define GDMA_IN_CHECK_OWNER_CH4_S  12
/* GDMA_DMA_INFIFO_FULL_THRS_CH4 : R/W ;bitpos:[11:0] ;default: 12'hc ; */
/*description: This register is used to generate the INFIFO_FULL_WM_INT interrupt when Rx
channel 4 received byte number in Rx FIFO is up to the value of the register..*/
#define GDMA_DMA_INFIFO_FULL_THRS_CH4    0x00000FFF
#define GDMA_DMA_INFIFO_FULL_THRS_CH4_M  ((GDMA_DMA_INFIFO_FULL_THRS_CH4_V)<<(GDMA_DMA_INFIFO_FULL_THRS_CH4_S))
#define GDMA_DMA_INFIFO_FULL_THRS_CH4_V  0xFFF
#define GDMA_DMA_INFIFO_FULL_THRS_CH4_S  0

#define GDMA_IN_INT_RAW_CH4_REG          (DR_REG_GDMA_BASE + 0x308)
/* GDMA_INFIFO_UDF_L3_CH4_INT_RAW : R/WTC/SS ;bitpos:[9] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo of Rx channel 4 is
underflow. .*/
#define GDMA_INFIFO_UDF_L3_CH4_INT_RAW    (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH4_INT_RAW_M  (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH4_INT_RAW_V  0x1
#define GDMA_INFIFO_UDF_L3_CH4_INT_RAW_S  9
/* GDMA_INFIFO_OVF_L3_CH4_INT_RAW : R/WTC/SS ;bitpos:[8] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo of Rx channel 4 is
overflow. .*/
#define GDMA_INFIFO_OVF_L3_CH4_INT_RAW    (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH4_INT_RAW_M  (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH4_INT_RAW_V  0x1
#define GDMA_INFIFO_OVF_L3_CH4_INT_RAW_S  8
/* GDMA_INFIFO_UDF_L1_CH4_INT_RAW : R/WTC/SS ;bitpos:[7] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo of Rx channel 4 is
underflow. .*/
#define GDMA_INFIFO_UDF_L1_CH4_INT_RAW    (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH4_INT_RAW_M  (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH4_INT_RAW_V  0x1
#define GDMA_INFIFO_UDF_L1_CH4_INT_RAW_S  7
/* GDMA_INFIFO_OVF_L1_CH4_INT_RAW : R/WTC/SS ;bitpos:[6] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo of Rx channel 4 is
overflow. .*/
#define GDMA_INFIFO_OVF_L1_CH4_INT_RAW    (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH4_INT_RAW_M  (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH4_INT_RAW_V  0x1
#define GDMA_INFIFO_OVF_L1_CH4_INT_RAW_S  6
/* GDMA_INFIFO_FULL_WM_CH4_INT_RAW : R/WTC/SS ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when received data byte number is up
to threshold configured by REG_DMA_INFIFO_FULL_THRS_CH0 in Rx FIFO of channel 4..*/
#define GDMA_INFIFO_FULL_WM_CH4_INT_RAW    (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH4_INT_RAW_M  (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH4_INT_RAW_V  0x1
#define GDMA_INFIFO_FULL_WM_CH4_INT_RAW_S  5
/* GDMA_IN_DSCR_EMPTY_CH4_INT_RAW : R/WTC/SS ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when Rx buffer pointed by inlink is
full and receiving data is not completed, but there is no more inlink for Rx
channel 4..*/
#define GDMA_IN_DSCR_EMPTY_CH4_INT_RAW    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH4_INT_RAW_M  (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH4_INT_RAW_V  0x1
#define GDMA_IN_DSCR_EMPTY_CH4_INT_RAW_S  4
/* GDMA_IN_DSCR_ERR_CH4_INT_RAW : R/WTC/SS ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when detecting inlink descriptor error
, including owner error, the second and third word error of inlink descriptor
for Rx channel 4..*/
#define GDMA_IN_DSCR_ERR_CH4_INT_RAW    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH4_INT_RAW_M  (BIT(3))
#define GDMA_IN_DSCR_ERR_CH4_INT_RAW_V  0x1
#define GDMA_IN_DSCR_ERR_CH4_INT_RAW_S  3
/* GDMA_IN_ERR_EOF_CH4_INT_RAW : R/WTC/SS ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when data error is detected only in
the case that the peripheral is UHCI0 for Rx channel 4. For other peripherals,
this raw interrupt is reserved..*/
#define GDMA_IN_ERR_EOF_CH4_INT_RAW    (BIT(2))
#define GDMA_IN_ERR_EOF_CH4_INT_RAW_M  (BIT(2))
#define GDMA_IN_ERR_EOF_CH4_INT_RAW_V  0x1
#define GDMA_IN_ERR_EOF_CH4_INT_RAW_S  2
/* GDMA_IN_SUC_EOF_CH4_INT_RAW : R/WTC/SS ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data pointed by one
inlink descriptor has been received for Rx channel 4. For UHCI0, the raw interrupt
bit turns to high level when the last data pointed by one inlink descriptor has
been received and no data error is detected for Rx channel 0..*/
#define GDMA_IN_SUC_EOF_CH4_INT_RAW    (BIT(1))
#define GDMA_IN_SUC_EOF_CH4_INT_RAW_M  (BIT(1))
#define GDMA_IN_SUC_EOF_CH4_INT_RAW_V  0x1
#define GDMA_IN_SUC_EOF_CH4_INT_RAW_S  1
/* GDMA_IN_DONE_CH4_INT_RAW : R/WTC/SS ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data pointed by one
inlink descriptor has been received for Rx channel 4..*/
#define GDMA_IN_DONE_CH4_INT_RAW    (BIT(0))
#define GDMA_IN_DONE_CH4_INT_RAW_M  (BIT(0))
#define GDMA_IN_DONE_CH4_INT_RAW_V  0x1
#define GDMA_IN_DONE_CH4_INT_RAW_S  0

#define GDMA_IN_INT_ST_CH4_REG          (DR_REG_GDMA_BASE + 0x30C)
/* GDMA_INFIFO_UDF_L3_CH4_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L3_CH4_INT_ST    (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH4_INT_ST_M  (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH4_INT_ST_V  0x1
#define GDMA_INFIFO_UDF_L3_CH4_INT_ST_S  9
/* GDMA_INFIFO_OVF_L3_CH4_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L3_CH4_INT_ST    (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH4_INT_ST_M  (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH4_INT_ST_V  0x1
#define GDMA_INFIFO_OVF_L3_CH4_INT_ST_S  8
/* GDMA_INFIFO_UDF_L1_CH4_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L1_CH4_INT_ST    (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH4_INT_ST_M  (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH4_INT_ST_V  0x1
#define GDMA_INFIFO_UDF_L1_CH4_INT_ST_S  7
/* GDMA_INFIFO_OVF_L1_CH4_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L1_CH4_INT_ST    (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH4_INT_ST_M  (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH4_INT_ST_V  0x1
#define GDMA_INFIFO_OVF_L1_CH4_INT_ST_S  6
/* GDMA_INFIFO_FULL_WM_CH4_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_FULL_WM_CH_INT interrupt..*/
#define GDMA_INFIFO_FULL_WM_CH4_INT_ST    (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH4_INT_ST_M  (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH4_INT_ST_V  0x1
#define GDMA_INFIFO_FULL_WM_CH4_INT_ST_S  5
/* GDMA_IN_DSCR_EMPTY_CH4_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt..*/
#define GDMA_IN_DSCR_EMPTY_CH4_INT_ST    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH4_INT_ST_M  (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH4_INT_ST_V  0x1
#define GDMA_IN_DSCR_EMPTY_CH4_INT_ST_S  4
/* GDMA_IN_DSCR_ERR_CH4_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_IN_DSCR_ERR_CH4_INT_ST    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH4_INT_ST_M  (BIT(3))
#define GDMA_IN_DSCR_ERR_CH4_INT_ST_V  0x1
#define GDMA_IN_DSCR_ERR_CH4_INT_ST_S  3
/* GDMA_IN_ERR_EOF_CH4_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt..*/
#define GDMA_IN_ERR_EOF_CH4_INT_ST    (BIT(2))
#define GDMA_IN_ERR_EOF_CH4_INT_ST_M  (BIT(2))
#define GDMA_IN_ERR_EOF_CH4_INT_ST_V  0x1
#define GDMA_IN_ERR_EOF_CH4_INT_ST_S  2
/* GDMA_IN_SUC_EOF_CH4_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt..*/
#define GDMA_IN_SUC_EOF_CH4_INT_ST    (BIT(1))
#define GDMA_IN_SUC_EOF_CH4_INT_ST_M  (BIT(1))
#define GDMA_IN_SUC_EOF_CH4_INT_ST_V  0x1
#define GDMA_IN_SUC_EOF_CH4_INT_ST_S  1
/* GDMA_IN_DONE_CH4_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DONE_CH_INT interrupt..*/
#define GDMA_IN_DONE_CH4_INT_ST    (BIT(0))
#define GDMA_IN_DONE_CH4_INT_ST_M  (BIT(0))
#define GDMA_IN_DONE_CH4_INT_ST_V  0x1
#define GDMA_IN_DONE_CH4_INT_ST_S  0

#define GDMA_IN_INT_ENA_CH4_REG          (DR_REG_GDMA_BASE + 0x310)
/* GDMA_INFIFO_UDF_L3_CH4_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L3_CH4_INT_ENA    (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH4_INT_ENA_M  (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH4_INT_ENA_V  0x1
#define GDMA_INFIFO_UDF_L3_CH4_INT_ENA_S  9
/* GDMA_INFIFO_OVF_L3_CH4_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L3_CH4_INT_ENA    (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH4_INT_ENA_M  (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH4_INT_ENA_V  0x1
#define GDMA_INFIFO_OVF_L3_CH4_INT_ENA_S  8
/* GDMA_INFIFO_UDF_L1_CH4_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L1_CH4_INT_ENA    (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH4_INT_ENA_M  (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH4_INT_ENA_V  0x1
#define GDMA_INFIFO_UDF_L1_CH4_INT_ENA_S  7
/* GDMA_INFIFO_OVF_L1_CH4_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L1_CH4_INT_ENA    (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH4_INT_ENA_M  (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH4_INT_ENA_V  0x1
#define GDMA_INFIFO_OVF_L1_CH4_INT_ENA_S  6
/* GDMA_INFIFO_FULL_WM_CH4_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_FULL_WM_CH_INT interrupt..*/
#define GDMA_INFIFO_FULL_WM_CH4_INT_ENA    (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH4_INT_ENA_M  (BIT(5))
#define GDMA_INFIFO_FULL_WM_CH4_INT_ENA_V  0x1
#define GDMA_INFIFO_FULL_WM_CH4_INT_ENA_S  5
/* GDMA_IN_DSCR_EMPTY_CH4_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt..*/
#define GDMA_IN_DSCR_EMPTY_CH4_INT_ENA    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH4_INT_ENA_M  (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH4_INT_ENA_V  0x1
#define GDMA_IN_DSCR_EMPTY_CH4_INT_ENA_S  4
/* GDMA_IN_DSCR_ERR_CH4_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_IN_DSCR_ERR_CH4_INT_ENA    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH4_INT_ENA_M  (BIT(3))
#define GDMA_IN_DSCR_ERR_CH4_INT_ENA_V  0x1
#define GDMA_IN_DSCR_ERR_CH4_INT_ENA_S  3
/* GDMA_IN_ERR_EOF_CH4_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt..*/
#define GDMA_IN_ERR_EOF_CH4_INT_ENA    (BIT(2))
#define GDMA_IN_ERR_EOF_CH4_INT_ENA_M  (BIT(2))
#define GDMA_IN_ERR_EOF_CH4_INT_ENA_V  0x1
#define GDMA_IN_ERR_EOF_CH4_INT_ENA_S  2
/* GDMA_IN_SUC_EOF_CH4_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt..*/
#define GDMA_IN_SUC_EOF_CH4_INT_ENA    (BIT(1))
#define GDMA_IN_SUC_EOF_CH4_INT_ENA_M  (BIT(1))
#define GDMA_IN_SUC_EOF_CH4_INT_ENA_V  0x1
#define GDMA_IN_SUC_EOF_CH4_INT_ENA_S  1
/* GDMA_IN_DONE_CH4_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DONE_CH_INT interrupt..*/
#define GDMA_IN_DONE_CH4_INT_ENA    (BIT(0))
#define GDMA_IN_DONE_CH4_INT_ENA_M  (BIT(0))
#define GDMA_IN_DONE_CH4_INT_ENA_V  0x1
#define GDMA_IN_DONE_CH4_INT_ENA_S  0

#define GDMA_IN_INT_CLR_CH4_REG          (DR_REG_GDMA_BASE + 0x314)
/* GDMA_INFIFO_UDF_L3_CH4_INT_CLR : WT ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L3_CH4_INT_CLR    (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH4_INT_CLR_M  (BIT(9))
#define GDMA_INFIFO_UDF_L3_CH4_INT_CLR_V  0x1
#define GDMA_INFIFO_UDF_L3_CH4_INT_CLR_S  9
/* GDMA_INFIFO_OVF_L3_CH4_INT_CLR : WT ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L3_CH4_INT_CLR    (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH4_INT_CLR_M  (BIT(8))
#define GDMA_INFIFO_OVF_L3_CH4_INT_CLR_V  0x1
#define GDMA_INFIFO_OVF_L3_CH4_INT_CLR_S  8
/* GDMA_INFIFO_UDF_L1_CH4_INT_CLR : WT ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_UDF_L1_CH4_INT_CLR    (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH4_INT_CLR_M  (BIT(7))
#define GDMA_INFIFO_UDF_L1_CH4_INT_CLR_V  0x1
#define GDMA_INFIFO_UDF_L1_CH4_INT_CLR_S  7
/* GDMA_INFIFO_OVF_L1_CH4_INT_CLR : WT ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_INFIFO_OVF_L1_CH4_INT_CLR    (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH4_INT_CLR_M  (BIT(6))
#define GDMA_INFIFO_OVF_L1_CH4_INT_CLR_V  0x1
#define GDMA_INFIFO_OVF_L1_CH4_INT_CLR_S  6
/* GDMA_DMA_INFIFO_FULL_WM_CH4_INT_CLR : WT ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_FULL_WM_CH_INT interrupt..*/
#define GDMA_DMA_INFIFO_FULL_WM_CH4_INT_CLR    (BIT(5))
#define GDMA_DMA_INFIFO_FULL_WM_CH4_INT_CLR_M  (BIT(5))
#define GDMA_DMA_INFIFO_FULL_WM_CH4_INT_CLR_V  0x1
#define GDMA_DMA_INFIFO_FULL_WM_CH4_INT_CLR_S  5
/* GDMA_IN_DSCR_EMPTY_CH4_INT_CLR : WT ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt..*/
#define GDMA_IN_DSCR_EMPTY_CH4_INT_CLR    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH4_INT_CLR_M  (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH4_INT_CLR_V  0x1
#define GDMA_IN_DSCR_EMPTY_CH4_INT_CLR_S  4
/* GDMA_IN_DSCR_ERR_CH4_INT_CLR : WT ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_IN_DSCR_ERR_CH4_INT_CLR    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH4_INT_CLR_M  (BIT(3))
#define GDMA_IN_DSCR_ERR_CH4_INT_CLR_V  0x1
#define GDMA_IN_DSCR_ERR_CH4_INT_CLR_S  3
/* GDMA_IN_ERR_EOF_CH4_INT_CLR : WT ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_ERR_EOF_CH_INT interrupt..*/
#define GDMA_IN_ERR_EOF_CH4_INT_CLR    (BIT(2))
#define GDMA_IN_ERR_EOF_CH4_INT_CLR_M  (BIT(2))
#define GDMA_IN_ERR_EOF_CH4_INT_CLR_V  0x1
#define GDMA_IN_ERR_EOF_CH4_INT_CLR_S  2
/* GDMA_IN_SUC_EOF_CH4_INT_CLR : WT ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_SUC_EOF_CH_INT interrupt..*/
#define GDMA_IN_SUC_EOF_CH4_INT_CLR    (BIT(1))
#define GDMA_IN_SUC_EOF_CH4_INT_CLR_M  (BIT(1))
#define GDMA_IN_SUC_EOF_CH4_INT_CLR_V  0x1
#define GDMA_IN_SUC_EOF_CH4_INT_CLR_S  1
/* GDMA_IN_DONE_CH4_INT_CLR : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DONE_CH_INT interrupt..*/
#define GDMA_IN_DONE_CH4_INT_CLR    (BIT(0))
#define GDMA_IN_DONE_CH4_INT_CLR_M  (BIT(0))
#define GDMA_IN_DONE_CH4_INT_CLR_V  0x1
#define GDMA_IN_DONE_CH4_INT_CLR_S  0

#define GDMA_INFIFO_STATUS_CH4_REG          (DR_REG_GDMA_BASE + 0x318)
/* GDMA_IN_BUF_HUNGRY_CH4 : RO ;bitpos:[28] ;default: 1'b0 ; */
/*description: reserved.*/
#define GDMA_IN_BUF_HUNGRY_CH4    (BIT(28))
#define GDMA_IN_BUF_HUNGRY_CH4_M  (BIT(28))
#define GDMA_IN_BUF_HUNGRY_CH4_V  0x1
#define GDMA_IN_BUF_HUNGRY_CH4_S  28
/* GDMA_IN_REMAIN_UNDER_4B_L3_CH4 : RO ;bitpos:[27] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_IN_REMAIN_UNDER_4B_L3_CH4    (BIT(27))
#define GDMA_IN_REMAIN_UNDER_4B_L3_CH4_M  (BIT(27))
#define GDMA_IN_REMAIN_UNDER_4B_L3_CH4_V  0x1
#define GDMA_IN_REMAIN_UNDER_4B_L3_CH4_S  27
/* GDMA_IN_REMAIN_UNDER_3B_L3_CH4 : RO ;bitpos:[26] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_IN_REMAIN_UNDER_3B_L3_CH4    (BIT(26))
#define GDMA_IN_REMAIN_UNDER_3B_L3_CH4_M  (BIT(26))
#define GDMA_IN_REMAIN_UNDER_3B_L3_CH4_V  0x1
#define GDMA_IN_REMAIN_UNDER_3B_L3_CH4_S  26
/* GDMA_IN_REMAIN_UNDER_2B_L3_CH4 : RO ;bitpos:[25] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_IN_REMAIN_UNDER_2B_L3_CH4    (BIT(25))
#define GDMA_IN_REMAIN_UNDER_2B_L3_CH4_M  (BIT(25))
#define GDMA_IN_REMAIN_UNDER_2B_L3_CH4_V  0x1
#define GDMA_IN_REMAIN_UNDER_2B_L3_CH4_S  25
/* GDMA_IN_REMAIN_UNDER_1B_L3_CH4 : RO ;bitpos:[24] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_IN_REMAIN_UNDER_1B_L3_CH4    (BIT(24))
#define GDMA_IN_REMAIN_UNDER_1B_L3_CH4_M  (BIT(24))
#define GDMA_IN_REMAIN_UNDER_1B_L3_CH4_V  0x1
#define GDMA_IN_REMAIN_UNDER_1B_L3_CH4_S  24
/* GDMA_INFIFO_CNT_L3_CH4 : RO ;bitpos:[23:19] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L3 Rx FIFO for Rx channel 4..*/
#define GDMA_INFIFO_CNT_L3_CH4    0x0000001F
#define GDMA_INFIFO_CNT_L3_CH4_M  ((GDMA_INFIFO_CNT_L3_CH4_V)<<(GDMA_INFIFO_CNT_L3_CH4_S))
#define GDMA_INFIFO_CNT_L3_CH4_V  0x1F
#define GDMA_INFIFO_CNT_L3_CH4_S  19
/* GDMA_INFIFO_CNT_L2_CH4 : RO ;bitpos:[18:12] ;default: 7'b0 ; */
/*description: The register stores the byte number of the data in L2 Rx FIFO for Rx channel 4..*/
#define GDMA_INFIFO_CNT_L2_CH4    0x0000007F
#define GDMA_INFIFO_CNT_L2_CH4_M  ((GDMA_INFIFO_CNT_L2_CH4_V)<<(GDMA_INFIFO_CNT_L2_CH4_S))
#define GDMA_INFIFO_CNT_L2_CH4_V  0x7F
#define GDMA_INFIFO_CNT_L2_CH4_S  12
/* GDMA_INFIFO_CNT_L1_CH4 : RO ;bitpos:[11:6] ;default: 6'b0 ; */
/*description: The register stores the byte number of the data in L1 Rx FIFO for Rx channel 4..*/
#define GDMA_INFIFO_CNT_L1_CH4    0x0000003F
#define GDMA_INFIFO_CNT_L1_CH4_M  ((GDMA_INFIFO_CNT_L1_CH4_V)<<(GDMA_INFIFO_CNT_L1_CH4_S))
#define GDMA_INFIFO_CNT_L1_CH4_V  0x3F
#define GDMA_INFIFO_CNT_L1_CH4_S  6
/* GDMA_INFIFO_EMPTY_L3_CH4 : RO ;bitpos:[5] ;default: 1'b1 ; */
/*description: L3 Rx FIFO empty signal for Rx channel 4..*/
#define GDMA_INFIFO_EMPTY_L3_CH4    (BIT(5))
#define GDMA_INFIFO_EMPTY_L3_CH4_M  (BIT(5))
#define GDMA_INFIFO_EMPTY_L3_CH4_V  0x1
#define GDMA_INFIFO_EMPTY_L3_CH4_S  5
/* GDMA_INFIFO_FULL_L3_CH4 : RO ;bitpos:[4] ;default: 1'b1 ; */
/*description: L3 Rx FIFO full signal for Rx channel 4..*/
#define GDMA_INFIFO_FULL_L3_CH4    (BIT(4))
#define GDMA_INFIFO_FULL_L3_CH4_M  (BIT(4))
#define GDMA_INFIFO_FULL_L3_CH4_V  0x1
#define GDMA_INFIFO_FULL_L3_CH4_S  4
/* GDMA_INFIFO_EMPTY_L2_CH4 : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: L2 Rx FIFO empty signal for Rx channel 4..*/
#define GDMA_INFIFO_EMPTY_L2_CH4    (BIT(3))
#define GDMA_INFIFO_EMPTY_L2_CH4_M  (BIT(3))
#define GDMA_INFIFO_EMPTY_L2_CH4_V  0x1
#define GDMA_INFIFO_EMPTY_L2_CH4_S  3
/* GDMA_INFIFO_FULL_L2_CH4 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: L2 Rx FIFO full signal for Rx channel 4..*/
#define GDMA_INFIFO_FULL_L2_CH4    (BIT(2))
#define GDMA_INFIFO_FULL_L2_CH4_M  (BIT(2))
#define GDMA_INFIFO_FULL_L2_CH4_V  0x1
#define GDMA_INFIFO_FULL_L2_CH4_S  2
/* GDMA_INFIFO_EMPTY_L1_CH4 : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: L1 Rx FIFO empty signal for Rx channel 4..*/
#define GDMA_INFIFO_EMPTY_L1_CH4    (BIT(1))
#define GDMA_INFIFO_EMPTY_L1_CH4_M  (BIT(1))
#define GDMA_INFIFO_EMPTY_L1_CH4_V  0x1
#define GDMA_INFIFO_EMPTY_L1_CH4_S  1
/* GDMA_INFIFO_FULL_L1_CH4 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: L1 Rx FIFO full signal for Rx channel 4..*/
#define GDMA_INFIFO_FULL_L1_CH4    (BIT(0))
#define GDMA_INFIFO_FULL_L1_CH4_M  (BIT(0))
#define GDMA_INFIFO_FULL_L1_CH4_V  0x1
#define GDMA_INFIFO_FULL_L1_CH4_S  0

#define GDMA_IN_POP_CH4_REG          (DR_REG_GDMA_BASE + 0x31C)
/* GDMA_INFIFO_POP_CH4 : R/W/SC ;bitpos:[12] ;default: 1'h0 ; */
/*description: Set this bit to pop data from DMA FIFO..*/
#define GDMA_INFIFO_POP_CH4    (BIT(12))
#define GDMA_INFIFO_POP_CH4_M  (BIT(12))
#define GDMA_INFIFO_POP_CH4_V  0x1
#define GDMA_INFIFO_POP_CH4_S  12
/* GDMA_INFIFO_RDATA_CH4 : RO ;bitpos:[11:0] ;default: 12'h800 ; */
/*description: This register stores the data popping from DMA FIFO..*/
#define GDMA_INFIFO_RDATA_CH4    0x00000FFF
#define GDMA_INFIFO_RDATA_CH4_M  ((GDMA_INFIFO_RDATA_CH4_V)<<(GDMA_INFIFO_RDATA_CH4_S))
#define GDMA_INFIFO_RDATA_CH4_V  0xFFF
#define GDMA_INFIFO_RDATA_CH4_S  0

#define GDMA_IN_LINK_CH4_REG          (DR_REG_GDMA_BASE + 0x320)
/* GDMA_INLINK_PARK_CH4 : RO ;bitpos:[24] ;default: 1'h1 ; */
/*description: 1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM
 is working..*/
#define GDMA_INLINK_PARK_CH4    (BIT(24))
#define GDMA_INLINK_PARK_CH4_M  (BIT(24))
#define GDMA_INLINK_PARK_CH4_V  0x1
#define GDMA_INLINK_PARK_CH4_S  24
/* GDMA_INLINK_RESTART_CH4 : R/W/SC ;bitpos:[23] ;default: 1'b0 ; */
/*description: Set this bit to mount a new inlink descriptor..*/
#define GDMA_INLINK_RESTART_CH4    (BIT(23))
#define GDMA_INLINK_RESTART_CH4_M  (BIT(23))
#define GDMA_INLINK_RESTART_CH4_V  0x1
#define GDMA_INLINK_RESTART_CH4_S  23
/* GDMA_INLINK_START_CH4 : R/W/SC ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to start dealing with the inlink descriptors..*/
#define GDMA_INLINK_START_CH4    (BIT(22))
#define GDMA_INLINK_START_CH4_M  (BIT(22))
#define GDMA_INLINK_START_CH4_V  0x1
#define GDMA_INLINK_START_CH4_S  22
/* GDMA_INLINK_STOP_CH4 : R/W/SC ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to stop dealing with the inlink descriptors..*/
#define GDMA_INLINK_STOP_CH4    (BIT(21))
#define GDMA_INLINK_STOP_CH4_M  (BIT(21))
#define GDMA_INLINK_STOP_CH4_V  0x1
#define GDMA_INLINK_STOP_CH4_S  21
/* GDMA_INLINK_AUTO_RET_CH4 : R/W ;bitpos:[20] ;default: 1'b1 ; */
/*description: Set this bit to return to current inlink descriptor's address, when there are
some errors in current receiving data..*/
#define GDMA_INLINK_AUTO_RET_CH4    (BIT(20))
#define GDMA_INLINK_AUTO_RET_CH4_M  (BIT(20))
#define GDMA_INLINK_AUTO_RET_CH4_V  0x1
#define GDMA_INLINK_AUTO_RET_CH4_S  20
/* GDMA_INLINK_ADDR_CH4 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: This register stores the 20 least significant bits of the first inlink
descriptor's address..*/
#define GDMA_INLINK_ADDR_CH4    0x000FFFFF
#define GDMA_INLINK_ADDR_CH4_M  ((GDMA_INLINK_ADDR_CH4_V)<<(GDMA_INLINK_ADDR_CH4_S))
#define GDMA_INLINK_ADDR_CH4_V  0xFFFFF
#define GDMA_INLINK_ADDR_CH4_S  0

#define GDMA_IN_STATE_CH4_REG          (DR_REG_GDMA_BASE + 0x324)
/* GDMA_IN_STATE_CH4 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: reserved.*/
#define GDMA_IN_STATE_CH4    0x00000007
#define GDMA_IN_STATE_CH4_M  ((GDMA_IN_STATE_CH4_V)<<(GDMA_IN_STATE_CH4_S))
#define GDMA_IN_STATE_CH4_V  0x7
#define GDMA_IN_STATE_CH4_S  20
/* GDMA_IN_DSCR_STATE_CH4 : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: reserved.*/
#define GDMA_IN_DSCR_STATE_CH4    0x00000003
#define GDMA_IN_DSCR_STATE_CH4_M  ((GDMA_IN_DSCR_STATE_CH4_V)<<(GDMA_IN_DSCR_STATE_CH4_S))
#define GDMA_IN_DSCR_STATE_CH4_V  0x3
#define GDMA_IN_DSCR_STATE_CH4_S  18
/* GDMA_INLINK_DSCR_ADDR_CH4 : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: This register stores the current inlink descriptor's address..*/
#define GDMA_INLINK_DSCR_ADDR_CH4    0x0003FFFF
#define GDMA_INLINK_DSCR_ADDR_CH4_M  ((GDMA_INLINK_DSCR_ADDR_CH4_V)<<(GDMA_INLINK_DSCR_ADDR_CH4_S))
#define GDMA_INLINK_DSCR_ADDR_CH4_V  0x3FFFF
#define GDMA_INLINK_DSCR_ADDR_CH4_S  0

#define GDMA_IN_SUC_EOF_DES_ADDR_CH4_REG          (DR_REG_GDMA_BASE + 0x328)
/* GDMA_IN_SUC_EOF_DES_ADDR_CH4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the inlink descriptor when the EOF bit in
this descriptor is 1..*/
#define GDMA_IN_SUC_EOF_DES_ADDR_CH4    0xFFFFFFFF
#define GDMA_IN_SUC_EOF_DES_ADDR_CH4_M  ((GDMA_IN_SUC_EOF_DES_ADDR_CH4_V)<<(GDMA_IN_SUC_EOF_DES_ADDR_CH4_S))
#define GDMA_IN_SUC_EOF_DES_ADDR_CH4_V  0xFFFFFFFF
#define GDMA_IN_SUC_EOF_DES_ADDR_CH4_S  0

#define GDMA_IN_ERR_EOF_DES_ADDR_CH4_REG          (DR_REG_GDMA_BASE + 0x32C)
/* GDMA_IN_ERR_EOF_DES_ADDR_CH4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the inlink descriptor when there are some
errors in current receiving data. Only used when peripheral is UHCI0..*/
#define GDMA_IN_ERR_EOF_DES_ADDR_CH4    0xFFFFFFFF
#define GDMA_IN_ERR_EOF_DES_ADDR_CH4_M  ((GDMA_IN_ERR_EOF_DES_ADDR_CH4_V)<<(GDMA_IN_ERR_EOF_DES_ADDR_CH4_S))
#define GDMA_IN_ERR_EOF_DES_ADDR_CH4_V  0xFFFFFFFF
#define GDMA_IN_ERR_EOF_DES_ADDR_CH4_S  0

#define GDMA_IN_DSCR_CH4_REG          (DR_REG_GDMA_BASE + 0x330)
/* GDMA_INLINK_DSCR_CH4 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the current inlink descriptor x..*/
#define GDMA_INLINK_DSCR_CH4    0xFFFFFFFF
#define GDMA_INLINK_DSCR_CH4_M  ((GDMA_INLINK_DSCR_CH4_V)<<(GDMA_INLINK_DSCR_CH4_S))
#define GDMA_INLINK_DSCR_CH4_V  0xFFFFFFFF
#define GDMA_INLINK_DSCR_CH4_S  0

#define GDMA_IN_DSCR_BF0_CH4_REG          (DR_REG_GDMA_BASE + 0x334)
/* GDMA_INLINK_DSCR_BF0_CH4 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the last inlink descriptor x-1..*/
#define GDMA_INLINK_DSCR_BF0_CH4    0xFFFFFFFF
#define GDMA_INLINK_DSCR_BF0_CH4_M  ((GDMA_INLINK_DSCR_BF0_CH4_V)<<(GDMA_INLINK_DSCR_BF0_CH4_S))
#define GDMA_INLINK_DSCR_BF0_CH4_V  0xFFFFFFFF
#define GDMA_INLINK_DSCR_BF0_CH4_S  0

#define GDMA_IN_DSCR_BF1_CH4_REG          (DR_REG_GDMA_BASE + 0x338)
/* GDMA_INLINK_DSCR_BF1_CH4 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the second-to-last inlink descriptor x-2..*/
#define GDMA_INLINK_DSCR_BF1_CH4    0xFFFFFFFF
#define GDMA_INLINK_DSCR_BF1_CH4_M  ((GDMA_INLINK_DSCR_BF1_CH4_V)<<(GDMA_INLINK_DSCR_BF1_CH4_S))
#define GDMA_INLINK_DSCR_BF1_CH4_V  0xFFFFFFFF
#define GDMA_INLINK_DSCR_BF1_CH4_S  0

#define GDMA_IN_WEIGHT_CH4_REG          (DR_REG_GDMA_BASE + 0x33C)
/* GDMA_RX_WEIGHT_CH4 : R/W ;bitpos:[11:8] ;default: 4'hf ; */
/*description: The weight of Rx channel 4. .*/
#define GDMA_RX_WEIGHT_CH4    0x0000000F
#define GDMA_RX_WEIGHT_CH4_M  ((GDMA_RX_WEIGHT_CH4_V)<<(GDMA_RX_WEIGHT_CH4_S))
#define GDMA_RX_WEIGHT_CH4_V  0xF
#define GDMA_RX_WEIGHT_CH4_S  8

#define GDMA_IN_PRI_CH4_REG          (DR_REG_GDMA_BASE + 0x344)
/* GDMA_RX_PRI_CH4 : R/W ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: The priority of Rx channel 4. The larger of the value, the higher of the priorit
y..*/
#define GDMA_RX_PRI_CH4    0x0000000F
#define GDMA_RX_PRI_CH4_M  ((GDMA_RX_PRI_CH4_V)<<(GDMA_RX_PRI_CH4_S))
#define GDMA_RX_PRI_CH4_V  0xF
#define GDMA_RX_PRI_CH4_S  0

#define GDMA_IN_PERI_SEL_CH4_REG          (DR_REG_GDMA_BASE + 0x348)
/* GDMA_PERI_IN_SEL_CH4 : R/W ;bitpos:[5:0] ;default: 6'h3f ; */
/*description: This register is used to select peripheral for Rx channel 5. 0:SPI2. 1: SPI3. 2:
 UHCI0. 3: I2S0. 4: I2S1. 5: LCD_CAM. 6: AES. 7: SHA. 8: ADC_DAC. 9: RMT..*/
#define GDMA_PERI_IN_SEL_CH4    0x0000003F
#define GDMA_PERI_IN_SEL_CH4_M  ((GDMA_PERI_IN_SEL_CH4_V)<<(GDMA_PERI_IN_SEL_CH4_S))
#define GDMA_PERI_IN_SEL_CH4_V  0x3F
#define GDMA_PERI_IN_SEL_CH4_S  0

#define GDMA_OUT_CONF0_CH4_REG          (DR_REG_GDMA_BASE + 0x360)
/* GDMA_OUT_DATA_BURST_EN_CH4 : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Tx channel 4 transmitting
data when accessing internal SRAM. .*/
#define GDMA_OUT_DATA_BURST_EN_CH4    (BIT(5))
#define GDMA_OUT_DATA_BURST_EN_CH4_M  (BIT(5))
#define GDMA_OUT_DATA_BURST_EN_CH4_V  0x1
#define GDMA_OUT_DATA_BURST_EN_CH4_S  5
/* GDMA_OUTDSCR_BURST_EN_CH4 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Tx channel 4 reading link
descriptor when accessing internal SRAM. .*/
#define GDMA_OUTDSCR_BURST_EN_CH4    (BIT(4))
#define GDMA_OUTDSCR_BURST_EN_CH4_M  (BIT(4))
#define GDMA_OUTDSCR_BURST_EN_CH4_V  0x1
#define GDMA_OUTDSCR_BURST_EN_CH4_S  4
/* GDMA_OUT_EOF_MODE_CH4 : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel 4 is
 generated when data need to transmit has been popped from FIFO in DMA.*/
#define GDMA_OUT_EOF_MODE_CH4    (BIT(3))
#define GDMA_OUT_EOF_MODE_CH4_M  (BIT(3))
#define GDMA_OUT_EOF_MODE_CH4_V  0x1
#define GDMA_OUT_EOF_MODE_CH4_S  3
/* GDMA_OUT_AUTO_WRBACK_CH4 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to enable automatic outlink-writeback when all the data in tx
buffer has been transmitted..*/
#define GDMA_OUT_AUTO_WRBACK_CH4    (BIT(2))
#define GDMA_OUT_AUTO_WRBACK_CH4_M  (BIT(2))
#define GDMA_OUT_AUTO_WRBACK_CH4_V  0x1
#define GDMA_OUT_AUTO_WRBACK_CH4_S  2
/* GDMA_OUT_LOOP_TEST_CH4 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: reserved.*/
#define GDMA_OUT_LOOP_TEST_CH4    (BIT(1))
#define GDMA_OUT_LOOP_TEST_CH4_M  (BIT(1))
#define GDMA_OUT_LOOP_TEST_CH4_V  0x1
#define GDMA_OUT_LOOP_TEST_CH4_S  1
/* GDMA_OUT_RST_CH4 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: This bit is used to reset DMA channel 4 Tx FSM and Tx FIFO pointer..*/
#define GDMA_OUT_RST_CH4    (BIT(0))
#define GDMA_OUT_RST_CH4_M  (BIT(0))
#define GDMA_OUT_RST_CH4_V  0x1
#define GDMA_OUT_RST_CH4_S  0

#define GDMA_OUT_CONF1_CH4_REG          (DR_REG_GDMA_BASE + 0x364)
/* GDMA_OUT_EXT_MEM_BK_SIZE_CH4 : R/W ;bitpos:[14:13] ;default: 2'b0 ; */
/*description: Block size of Tx channel 4 when DMA access external SRAM. 0: 16 bytes      1: 32
 bytes    2/3:reserved.*/
#define GDMA_OUT_EXT_MEM_BK_SIZE_CH4    0x00000003
#define GDMA_OUT_EXT_MEM_BK_SIZE_CH4_M  ((GDMA_OUT_EXT_MEM_BK_SIZE_CH4_V)<<(GDMA_OUT_EXT_MEM_BK_SIZE_CH4_S))
#define GDMA_OUT_EXT_MEM_BK_SIZE_CH4_V  0x3
#define GDMA_OUT_EXT_MEM_BK_SIZE_CH4_S  13
/* GDMA_OUT_CHECK_OWNER_CH4 : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to enable checking the owner attribute of the link descriptor..*/
#define GDMA_OUT_CHECK_OWNER_CH4    (BIT(12))
#define GDMA_OUT_CHECK_OWNER_CH4_M  (BIT(12))
#define GDMA_OUT_CHECK_OWNER_CH4_V  0x1
#define GDMA_OUT_CHECK_OWNER_CH4_S  12

#define GDMA_OUT_INT_RAW_CH4_REG          (DR_REG_GDMA_BASE + 0x368)
/* GDMA_OUTFIFO_UDF_L3_CH4_INT_RAW : R/WTC/SS ;bitpos:[7] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo of Tx channel 4 is
underflow. .*/
#define GDMA_OUTFIFO_UDF_L3_CH4_INT_RAW    (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH4_INT_RAW_M  (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH4_INT_RAW_V  0x1
#define GDMA_OUTFIFO_UDF_L3_CH4_INT_RAW_S  7
/* GDMA_OUTFIFO_OVF_L3_CH4_INT_RAW : R/WTC/SS ;bitpos:[6] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo of Tx channel 4 is
overflow. .*/
#define GDMA_OUTFIFO_OVF_L3_CH4_INT_RAW    (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH4_INT_RAW_M  (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH4_INT_RAW_V  0x1
#define GDMA_OUTFIFO_OVF_L3_CH4_INT_RAW_S  6
/* GDMA_OUTFIFO_UDF_L1_CH4_INT_RAW : R/WTC/SS ;bitpos:[5] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo of Tx channel 4 is
underflow. .*/
#define GDMA_OUTFIFO_UDF_L1_CH4_INT_RAW    (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH4_INT_RAW_M  (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH4_INT_RAW_V  0x1
#define GDMA_OUTFIFO_UDF_L1_CH4_INT_RAW_S  5
/* GDMA_OUTFIFO_OVF_L1_CH4_INT_RAW : R/WTC/SS ;bitpos:[4] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo of Tx channel 4 is
overflow. .*/
#define GDMA_OUTFIFO_OVF_L1_CH4_INT_RAW    (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH4_INT_RAW_M  (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH4_INT_RAW_V  0x1
#define GDMA_OUTFIFO_OVF_L1_CH4_INT_RAW_S  4
/* GDMA_OUT_TOTAL_EOF_CH4_INT_RAW : R/WTC/SS ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when data corresponding a outlink
(includes one link descriptor or few link descriptors) is transmitted out for Tx cha
nnel 4..*/
#define GDMA_OUT_TOTAL_EOF_CH4_INT_RAW    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH4_INT_RAW_M  (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH4_INT_RAW_V  0x1
#define GDMA_OUT_TOTAL_EOF_CH4_INT_RAW_S  3
/* GDMA_OUT_DSCR_ERR_CH4_INT_RAW : R/WTC/SS ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when detecting outlink descriptor error
r, including owner error, the second and third word error of outlink descriptor
for Tx channel 4..*/
#define GDMA_OUT_DSCR_ERR_CH4_INT_RAW    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH4_INT_RAW_M  (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH4_INT_RAW_V  0x1
#define GDMA_OUT_DSCR_ERR_CH4_INT_RAW_S  2
/* GDMA_OUT_EOF_CH4_INT_RAW : R/WTC/SS ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data pointed by one
outlink descriptor has been read from memory for Tx channel 4. .*/
#define GDMA_OUT_EOF_CH4_INT_RAW    (BIT(1))
#define GDMA_OUT_EOF_CH4_INT_RAW_M  (BIT(1))
#define GDMA_OUT_EOF_CH4_INT_RAW_V  0x1
#define GDMA_OUT_EOF_CH4_INT_RAW_S  1
/* GDMA_OUT_DONE_CH4_INT_RAW : R/WTC/SS ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data pointed by one
outlink descriptor has been transmitted to peripherals for Tx channel 4..*/
#define GDMA_OUT_DONE_CH4_INT_RAW    (BIT(0))
#define GDMA_OUT_DONE_CH4_INT_RAW_M  (BIT(0))
#define GDMA_OUT_DONE_CH4_INT_RAW_V  0x1
#define GDMA_OUT_DONE_CH4_INT_RAW_S  0

#define GDMA_OUT_INT_ST_CH4_REG          (DR_REG_GDMA_BASE + 0x36C)
/* GDMA_OUTFIFO_UDF_L3_CH4_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L3_CH4_INT_ST    (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH4_INT_ST_M  (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH4_INT_ST_V  0x1
#define GDMA_OUTFIFO_UDF_L3_CH4_INT_ST_S  7
/* GDMA_OUTFIFO_OVF_L3_CH4_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L3_CH4_INT_ST    (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH4_INT_ST_M  (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH4_INT_ST_V  0x1
#define GDMA_OUTFIFO_OVF_L3_CH4_INT_ST_S  6
/* GDMA_OUTFIFO_UDF_L1_CH4_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L1_CH4_INT_ST    (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH4_INT_ST_M  (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH4_INT_ST_V  0x1
#define GDMA_OUTFIFO_UDF_L1_CH4_INT_ST_S  5
/* GDMA_OUTFIFO_OVF_L1_CH4_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L1_CH4_INT_ST    (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH4_INT_ST_M  (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH4_INT_ST_V  0x1
#define GDMA_OUTFIFO_OVF_L1_CH4_INT_ST_S  4
/* GDMA_OUT_TOTAL_EOF_CH4_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt..*/
#define GDMA_OUT_TOTAL_EOF_CH4_INT_ST    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH4_INT_ST_M  (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH4_INT_ST_V  0x1
#define GDMA_OUT_TOTAL_EOF_CH4_INT_ST_S  3
/* GDMA_OUT_DSCR_ERR_CH4_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_OUT_DSCR_ERR_CH4_INT_ST    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH4_INT_ST_M  (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH4_INT_ST_V  0x1
#define GDMA_OUT_DSCR_ERR_CH4_INT_ST_S  2
/* GDMA_OUT_EOF_CH4_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_EOF_CH_INT interrupt..*/
#define GDMA_OUT_EOF_CH4_INT_ST    (BIT(1))
#define GDMA_OUT_EOF_CH4_INT_ST_M  (BIT(1))
#define GDMA_OUT_EOF_CH4_INT_ST_V  0x1
#define GDMA_OUT_EOF_CH4_INT_ST_S  1
/* GDMA_OUT_DONE_CH4_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_DONE_CH_INT interrupt..*/
#define GDMA_OUT_DONE_CH4_INT_ST    (BIT(0))
#define GDMA_OUT_DONE_CH4_INT_ST_M  (BIT(0))
#define GDMA_OUT_DONE_CH4_INT_ST_V  0x1
#define GDMA_OUT_DONE_CH4_INT_ST_S  0

#define GDMA_OUT_INT_ENA_CH4_REG          (DR_REG_GDMA_BASE + 0x370)
/* GDMA_OUTFIFO_UDF_L3_CH4_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L3_CH4_INT_ENA    (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH4_INT_ENA_M  (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH4_INT_ENA_V  0x1
#define GDMA_OUTFIFO_UDF_L3_CH4_INT_ENA_S  7
/* GDMA_OUTFIFO_OVF_L3_CH4_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L3_CH4_INT_ENA    (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH4_INT_ENA_M  (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH4_INT_ENA_V  0x1
#define GDMA_OUTFIFO_OVF_L3_CH4_INT_ENA_S  6
/* GDMA_OUTFIFO_UDF_L1_CH4_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L1_CH4_INT_ENA    (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH4_INT_ENA_M  (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH4_INT_ENA_V  0x1
#define GDMA_OUTFIFO_UDF_L1_CH4_INT_ENA_S  5
/* GDMA_OUTFIFO_OVF_L1_CH4_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L1_CH4_INT_ENA    (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH4_INT_ENA_M  (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH4_INT_ENA_V  0x1
#define GDMA_OUTFIFO_OVF_L1_CH4_INT_ENA_S  4
/* GDMA_OUT_TOTAL_EOF_CH4_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt..*/
#define GDMA_OUT_TOTAL_EOF_CH4_INT_ENA    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH4_INT_ENA_M  (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH4_INT_ENA_V  0x1
#define GDMA_OUT_TOTAL_EOF_CH4_INT_ENA_S  3
/* GDMA_OUT_DSCR_ERR_CH4_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_OUT_DSCR_ERR_CH4_INT_ENA    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH4_INT_ENA_M  (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH4_INT_ENA_V  0x1
#define GDMA_OUT_DSCR_ERR_CH4_INT_ENA_S  2
/* GDMA_OUT_EOF_CH4_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_EOF_CH_INT interrupt..*/
#define GDMA_OUT_EOF_CH4_INT_ENA    (BIT(1))
#define GDMA_OUT_EOF_CH4_INT_ENA_M  (BIT(1))
#define GDMA_OUT_EOF_CH4_INT_ENA_V  0x1
#define GDMA_OUT_EOF_CH4_INT_ENA_S  1
/* GDMA_OUT_DONE_CH4_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_DONE_CH_INT interrupt..*/
#define GDMA_OUT_DONE_CH4_INT_ENA    (BIT(0))
#define GDMA_OUT_DONE_CH4_INT_ENA_M  (BIT(0))
#define GDMA_OUT_DONE_CH4_INT_ENA_V  0x1
#define GDMA_OUT_DONE_CH4_INT_ENA_S  0

#define GDMA_OUT_INT_CLR_CH4_REG          (DR_REG_GDMA_BASE + 0x374)
/* GDMA_OUTFIFO_UDF_L3_CH4_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L3_CH4_INT_CLR    (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH4_INT_CLR_M  (BIT(7))
#define GDMA_OUTFIFO_UDF_L3_CH4_INT_CLR_V  0x1
#define GDMA_OUTFIFO_UDF_L3_CH4_INT_CLR_S  7
/* GDMA_OUTFIFO_OVF_L3_CH4_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L3_CH4_INT_CLR    (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH4_INT_CLR_M  (BIT(6))
#define GDMA_OUTFIFO_OVF_L3_CH4_INT_CLR_V  0x1
#define GDMA_OUTFIFO_OVF_L3_CH4_INT_CLR_S  6
/* GDMA_OUTFIFO_UDF_L1_CH4_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_UDF_L1_CH4_INT_CLR    (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH4_INT_CLR_M  (BIT(5))
#define GDMA_OUTFIFO_UDF_L1_CH4_INT_CLR_V  0x1
#define GDMA_OUTFIFO_UDF_L1_CH4_INT_CLR_S  5
/* GDMA_OUTFIFO_OVF_L1_CH4_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt..*/
#define GDMA_OUTFIFO_OVF_L1_CH4_INT_CLR    (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH4_INT_CLR_M  (BIT(4))
#define GDMA_OUTFIFO_OVF_L1_CH4_INT_CLR_V  0x1
#define GDMA_OUTFIFO_OVF_L1_CH4_INT_CLR_S  4
/* GDMA_OUT_TOTAL_EOF_CH4_INT_CLR : WT ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt..*/
#define GDMA_OUT_TOTAL_EOF_CH4_INT_CLR    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH4_INT_CLR_M  (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH4_INT_CLR_V  0x1
#define GDMA_OUT_TOTAL_EOF_CH4_INT_CLR_S  3
/* GDMA_OUT_DSCR_ERR_CH4_INT_CLR : WT ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt..*/
#define GDMA_OUT_DSCR_ERR_CH4_INT_CLR    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH4_INT_CLR_M  (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH4_INT_CLR_V  0x1
#define GDMA_OUT_DSCR_ERR_CH4_INT_CLR_S  2
/* GDMA_OUT_EOF_CH4_INT_CLR : WT ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_EOF_CH_INT interrupt..*/
#define GDMA_OUT_EOF_CH4_INT_CLR    (BIT(1))
#define GDMA_OUT_EOF_CH4_INT_CLR_M  (BIT(1))
#define GDMA_OUT_EOF_CH4_INT_CLR_V  0x1
#define GDMA_OUT_EOF_CH4_INT_CLR_S  1
/* GDMA_OUT_DONE_CH4_INT_CLR : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_DONE_CH_INT interrupt..*/
#define GDMA_OUT_DONE_CH4_INT_CLR    (BIT(0))
#define GDMA_OUT_DONE_CH4_INT_CLR_M  (BIT(0))
#define GDMA_OUT_DONE_CH4_INT_CLR_V  0x1
#define GDMA_OUT_DONE_CH4_INT_CLR_S  0

#define GDMA_OUTFIFO_STATUS_CH4_REG          (DR_REG_GDMA_BASE + 0x378)
/* GDMA_OUT_REMAIN_UNDER_4B_L3_CH4 : RO ;bitpos:[26] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_OUT_REMAIN_UNDER_4B_L3_CH4    (BIT(26))
#define GDMA_OUT_REMAIN_UNDER_4B_L3_CH4_M  (BIT(26))
#define GDMA_OUT_REMAIN_UNDER_4B_L3_CH4_V  0x1
#define GDMA_OUT_REMAIN_UNDER_4B_L3_CH4_S  26
/* GDMA_OUT_REMAIN_UNDER_3B_L3_CH4 : RO ;bitpos:[25] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_OUT_REMAIN_UNDER_3B_L3_CH4    (BIT(25))
#define GDMA_OUT_REMAIN_UNDER_3B_L3_CH4_M  (BIT(25))
#define GDMA_OUT_REMAIN_UNDER_3B_L3_CH4_V  0x1
#define GDMA_OUT_REMAIN_UNDER_3B_L3_CH4_S  25
/* GDMA_OUT_REMAIN_UNDER_2B_L3_CH4 : RO ;bitpos:[24] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_OUT_REMAIN_UNDER_2B_L3_CH4    (BIT(24))
#define GDMA_OUT_REMAIN_UNDER_2B_L3_CH4_M  (BIT(24))
#define GDMA_OUT_REMAIN_UNDER_2B_L3_CH4_V  0x1
#define GDMA_OUT_REMAIN_UNDER_2B_L3_CH4_S  24
/* GDMA_OUT_REMAIN_UNDER_1B_L3_CH4 : RO ;bitpos:[23] ;default: 1'b1 ; */
/*description: reserved.*/
#define GDMA_OUT_REMAIN_UNDER_1B_L3_CH4    (BIT(23))
#define GDMA_OUT_REMAIN_UNDER_1B_L3_CH4_M  (BIT(23))
#define GDMA_OUT_REMAIN_UNDER_1B_L3_CH4_V  0x1
#define GDMA_OUT_REMAIN_UNDER_1B_L3_CH4_S  23
/* GDMA_OUTFIFO_CNT_L3_CH4 : RO ;bitpos:[22:18] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L3 Tx FIFO for Tx channel 4..*/
#define GDMA_OUTFIFO_CNT_L3_CH4    0x0000001F
#define GDMA_OUTFIFO_CNT_L3_CH4_M  ((GDMA_OUTFIFO_CNT_L3_CH4_V)<<(GDMA_OUTFIFO_CNT_L3_CH4_S))
#define GDMA_OUTFIFO_CNT_L3_CH4_V  0x1F
#define GDMA_OUTFIFO_CNT_L3_CH4_S  18
/* GDMA_OUTFIFO_CNT_L2_CH4 : RO ;bitpos:[17:11] ;default: 7'b0 ; */
/*description: The register stores the byte number of the data in L2 Tx FIFO for Tx channel 4..*/
#define GDMA_OUTFIFO_CNT_L2_CH4    0x0000007F
#define GDMA_OUTFIFO_CNT_L2_CH4_M  ((GDMA_OUTFIFO_CNT_L2_CH4_V)<<(GDMA_OUTFIFO_CNT_L2_CH4_S))
#define GDMA_OUTFIFO_CNT_L2_CH4_V  0x7F
#define GDMA_OUTFIFO_CNT_L2_CH4_S  11
/* GDMA_OUTFIFO_CNT_L1_CH4 : RO ;bitpos:[10:6] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L1 Tx FIFO for Tx channel 4..*/
#define GDMA_OUTFIFO_CNT_L1_CH4    0x0000001F
#define GDMA_OUTFIFO_CNT_L1_CH4_M  ((GDMA_OUTFIFO_CNT_L1_CH4_V)<<(GDMA_OUTFIFO_CNT_L1_CH4_S))
#define GDMA_OUTFIFO_CNT_L1_CH4_V  0x1F
#define GDMA_OUTFIFO_CNT_L1_CH4_S  6
/* GDMA_OUTFIFO_EMPTY_L3_CH4 : RO ;bitpos:[5] ;default: 1'b1 ; */
/*description: L3 Tx FIFO empty signal for Tx channel 4..*/
#define GDMA_OUTFIFO_EMPTY_L3_CH4    (BIT(5))
#define GDMA_OUTFIFO_EMPTY_L3_CH4_M  (BIT(5))
#define GDMA_OUTFIFO_EMPTY_L3_CH4_V  0x1
#define GDMA_OUTFIFO_EMPTY_L3_CH4_S  5
/* GDMA_OUTFIFO_FULL_L3_CH4 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: L3 Tx FIFO full signal for Tx channel 4..*/
#define GDMA_OUTFIFO_FULL_L3_CH4    (BIT(4))
#define GDMA_OUTFIFO_FULL_L3_CH4_M  (BIT(4))
#define GDMA_OUTFIFO_FULL_L3_CH4_V  0x1
#define GDMA_OUTFIFO_FULL_L3_CH4_S  4
/* GDMA_OUTFIFO_EMPTY_L2_CH4 : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: L2 Tx FIFO empty signal for Tx channel 4..*/
#define GDMA_OUTFIFO_EMPTY_L2_CH4    (BIT(3))
#define GDMA_OUTFIFO_EMPTY_L2_CH4_M  (BIT(3))
#define GDMA_OUTFIFO_EMPTY_L2_CH4_V  0x1
#define GDMA_OUTFIFO_EMPTY_L2_CH4_S  3
/* GDMA_OUTFIFO_FULL_L2_CH4 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: L2 Tx FIFO full signal for Tx channel 4..*/
#define GDMA_OUTFIFO_FULL_L2_CH4    (BIT(2))
#define GDMA_OUTFIFO_FULL_L2_CH4_M  (BIT(2))
#define GDMA_OUTFIFO_FULL_L2_CH4_V  0x1
#define GDMA_OUTFIFO_FULL_L2_CH4_S  2
/* GDMA_OUTFIFO_EMPTY_L1_CH4 : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: L1 Tx FIFO empty signal for Tx channel 4..*/
#define GDMA_OUTFIFO_EMPTY_L1_CH4    (BIT(1))
#define GDMA_OUTFIFO_EMPTY_L1_CH4_M  (BIT(1))
#define GDMA_OUTFIFO_EMPTY_L1_CH4_V  0x1
#define GDMA_OUTFIFO_EMPTY_L1_CH4_S  1
/* GDMA_OUTFIFO_FULL_L1_CH4 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: L1 Tx FIFO full signal for Tx channel 4..*/
#define GDMA_OUTFIFO_FULL_L1_CH4    (BIT(0))
#define GDMA_OUTFIFO_FULL_L1_CH4_M  (BIT(0))
#define GDMA_OUTFIFO_FULL_L1_CH4_V  0x1
#define GDMA_OUTFIFO_FULL_L1_CH4_S  0

#define GDMA_OUT_PUSH_CH4_REG          (DR_REG_GDMA_BASE + 0x37C)
/* GDMA_OUTFIFO_PUSH_CH4 : R/W/SC ;bitpos:[9] ;default: 1'h0 ; */
/*description: Set this bit to push data into DMA FIFO..*/
#define GDMA_OUTFIFO_PUSH_CH4    (BIT(9))
#define GDMA_OUTFIFO_PUSH_CH4_M  (BIT(9))
#define GDMA_OUTFIFO_PUSH_CH4_V  0x1
#define GDMA_OUTFIFO_PUSH_CH4_S  9
/* GDMA_OUTFIFO_WDATA_CH4 : R/W ;bitpos:[8:0] ;default: 9'h0 ; */
/*description: This register stores the data that need to be pushed into DMA FIFO..*/
#define GDMA_OUTFIFO_WDATA_CH4    0x000001FF
#define GDMA_OUTFIFO_WDATA_CH4_M  ((GDMA_OUTFIFO_WDATA_CH4_V)<<(GDMA_OUTFIFO_WDATA_CH4_S))
#define GDMA_OUTFIFO_WDATA_CH4_V  0x1FF
#define GDMA_OUTFIFO_WDATA_CH4_S  0

#define GDMA_OUT_LINK_CH4_REG          (DR_REG_GDMA_BASE + 0x380)
/* GDMA_OUTLINK_PARK_CH4 : RO ;bitpos:[23] ;default: 1'h1 ; */
/*description: 1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's
FSM is working..*/
#define GDMA_OUTLINK_PARK_CH4    (BIT(23))
#define GDMA_OUTLINK_PARK_CH4_M  (BIT(23))
#define GDMA_OUTLINK_PARK_CH4_V  0x1
#define GDMA_OUTLINK_PARK_CH4_S  23
/* GDMA_OUTLINK_RESTART_CH4 : R/W/SC ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to restart a new outlink from the last address. .*/
#define GDMA_OUTLINK_RESTART_CH4    (BIT(22))
#define GDMA_OUTLINK_RESTART_CH4_M  (BIT(22))
#define GDMA_OUTLINK_RESTART_CH4_V  0x1
#define GDMA_OUTLINK_RESTART_CH4_S  22
/* GDMA_OUTLINK_START_CH4 : R/W/SC ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to start dealing with the outlink descriptors..*/
#define GDMA_OUTLINK_START_CH4    (BIT(21))
#define GDMA_OUTLINK_START_CH4_M  (BIT(21))
#define GDMA_OUTLINK_START_CH4_V  0x1
#define GDMA_OUTLINK_START_CH4_S  21
/* GDMA_OUTLINK_STOP_CH4 : R/W/SC ;bitpos:[20] ;default: 1'b0 ; */
/*description: Set this bit to stop dealing with the outlink descriptors..*/
#define GDMA_OUTLINK_STOP_CH4    (BIT(20))
#define GDMA_OUTLINK_STOP_CH4_M  (BIT(20))
#define GDMA_OUTLINK_STOP_CH4_V  0x1
#define GDMA_OUTLINK_STOP_CH4_S  20
/* GDMA_OUTLINK_ADDR_CH4 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: This register stores the 20 least significant bits of the first outlink
descriptor's address..*/
#define GDMA_OUTLINK_ADDR_CH4    0x000FFFFF
#define GDMA_OUTLINK_ADDR_CH4_M  ((GDMA_OUTLINK_ADDR_CH4_V)<<(GDMA_OUTLINK_ADDR_CH4_S))
#define GDMA_OUTLINK_ADDR_CH4_V  0xFFFFF
#define GDMA_OUTLINK_ADDR_CH4_S  0

#define GDMA_OUT_STATE_CH4_REG          (DR_REG_GDMA_BASE + 0x384)
/* GDMA_OUT_STATE_CH4 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: reserved.*/
#define GDMA_OUT_STATE_CH4    0x00000007
#define GDMA_OUT_STATE_CH4_M  ((GDMA_OUT_STATE_CH4_V)<<(GDMA_OUT_STATE_CH4_S))
#define GDMA_OUT_STATE_CH4_V  0x7
#define GDMA_OUT_STATE_CH4_S  20
/* GDMA_OUT_DSCR_STATE_CH4 : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: reserved.*/
#define GDMA_OUT_DSCR_STATE_CH4    0x00000003
#define GDMA_OUT_DSCR_STATE_CH4_M  ((GDMA_OUT_DSCR_STATE_CH4_V)<<(GDMA_OUT_DSCR_STATE_CH4_S))
#define GDMA_OUT_DSCR_STATE_CH4_V  0x3
#define GDMA_OUT_DSCR_STATE_CH4_S  18
/* GDMA_OUTLINK_DSCR_ADDR_CH4 : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: This register stores the current outlink descriptor's address..*/
#define GDMA_OUTLINK_DSCR_ADDR_CH4    0x0003FFFF
#define GDMA_OUTLINK_DSCR_ADDR_CH4_M  ((GDMA_OUTLINK_DSCR_ADDR_CH4_V)<<(GDMA_OUTLINK_DSCR_ADDR_CH4_S))
#define GDMA_OUTLINK_DSCR_ADDR_CH4_V  0x3FFFF
#define GDMA_OUTLINK_DSCR_ADDR_CH4_S  0

#define GDMA_OUT_EOF_DES_ADDR_CH4_REG          (DR_REG_GDMA_BASE + 0x388)
/* GDMA_OUT_EOF_DES_ADDR_CH4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the outlink descriptor when the EOF bit in
this descriptor is 1..*/
#define GDMA_OUT_EOF_DES_ADDR_CH4    0xFFFFFFFF
#define GDMA_OUT_EOF_DES_ADDR_CH4_M  ((GDMA_OUT_EOF_DES_ADDR_CH4_V)<<(GDMA_OUT_EOF_DES_ADDR_CH4_S))
#define GDMA_OUT_EOF_DES_ADDR_CH4_V  0xFFFFFFFF
#define GDMA_OUT_EOF_DES_ADDR_CH4_S  0

#define GDMA_OUT_EOF_BFR_DES_ADDR_CH4_REG          (DR_REG_GDMA_BASE + 0x38C)
/* GDMA_OUT_EOF_BFR_DES_ADDR_CH4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the outlink descriptor before the last
outlink descriptor..*/
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH4    0xFFFFFFFF
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH4_M  ((GDMA_OUT_EOF_BFR_DES_ADDR_CH4_V)<<(GDMA_OUT_EOF_BFR_DES_ADDR_CH4_S))
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH4_V  0xFFFFFFFF
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH4_S  0

#define GDMA_OUT_DSCR_CH4_REG          (DR_REG_GDMA_BASE + 0x390)
/* GDMA_OUTLINK_DSCR_CH4 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the current outlink descriptor y..*/
#define GDMA_OUTLINK_DSCR_CH4    0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_CH4_M  ((GDMA_OUTLINK_DSCR_CH4_V)<<(GDMA_OUTLINK_DSCR_CH4_S))
#define GDMA_OUTLINK_DSCR_CH4_V  0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_CH4_S  0

#define GDMA_OUT_DSCR_BF0_CH4_REG          (DR_REG_GDMA_BASE + 0x394)
/* GDMA_OUTLINK_DSCR_BF0_CH4 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the last outlink descriptor y-1..*/
#define GDMA_OUTLINK_DSCR_BF0_CH4    0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_BF0_CH4_M  ((GDMA_OUTLINK_DSCR_BF0_CH4_V)<<(GDMA_OUTLINK_DSCR_BF0_CH4_S))
#define GDMA_OUTLINK_DSCR_BF0_CH4_V  0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_BF0_CH4_S  0

#define GDMA_OUT_DSCR_BF1_CH4_REG          (DR_REG_GDMA_BASE + 0x398)
/* GDMA_OUTLINK_DSCR_BF1_CH4 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the second-to-last inlink descriptor x-2..*/
#define GDMA_OUTLINK_DSCR_BF1_CH4    0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_BF1_CH4_M  ((GDMA_OUTLINK_DSCR_BF1_CH4_V)<<(GDMA_OUTLINK_DSCR_BF1_CH4_S))
#define GDMA_OUTLINK_DSCR_BF1_CH4_V  0xFFFFFFFF
#define GDMA_OUTLINK_DSCR_BF1_CH4_S  0

#define GDMA_OUT_WEIGHT_CH4_REG          (DR_REG_GDMA_BASE + 0x39C)
/* GDMA_TX_WEIGHT_CH4 : R/W ;bitpos:[11:8] ;default: 4'hf ; */
/*description: The weight of Tx channel 4. .*/
#define GDMA_TX_WEIGHT_CH4    0x0000000F
#define GDMA_TX_WEIGHT_CH4_M  ((GDMA_TX_WEIGHT_CH4_V)<<(GDMA_TX_WEIGHT_CH4_S))
#define GDMA_TX_WEIGHT_CH4_V  0xF
#define GDMA_TX_WEIGHT_CH4_S  8

#define GDMA_OUT_PRI_CH4_REG          (DR_REG_GDMA_BASE + 0x3A4)
/* GDMA_TX_PRI_CH4 : R/W ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: The priority of Tx channel 4. The larger of the value, the higher of the
priority..*/
#define GDMA_TX_PRI_CH4    0x0000000F
#define GDMA_TX_PRI_CH4_M  ((GDMA_TX_PRI_CH4_V)<<(GDMA_TX_PRI_CH4_S))
#define GDMA_TX_PRI_CH4_V  0xF
#define GDMA_TX_PRI_CH4_S  0

#define GDMA_OUT_PERI_SEL_CH4_REG          (DR_REG_GDMA_BASE + 0x3A8)
/* GDMA_PERI_OUT_SEL_CH4 : R/W ;bitpos:[5:0] ;default: 6'h3f ; */
/*description: This register is used to select peripheral for Tx channel 4. 0:SPI2. 1: SPI3. 2:
 UHCI0. 3: I2S0. 4: I2S1. 5: LCD_CAM. 6: AES. 7: SHA. 8: ADC_DAC. 9: RMT.; 7: AE
S. 8: SHA. 9: ADC_DAC..*/
#define GDMA_PERI_OUT_SEL_CH4    0x0000003F
#define GDMA_PERI_OUT_SEL_CH4_M  ((GDMA_PERI_OUT_SEL_CH4_V)<<(GDMA_PERI_OUT_SEL_CH4_S))
#define GDMA_PERI_OUT_SEL_CH4_V  0x3F
#define GDMA_PERI_OUT_SEL_CH4_S  0

#define GDMA_AHB_TEST_REG          (DR_REG_GDMA_BASE + 0x3C0)
/* GDMA_AHB_TESTADDR : R/W ;bitpos:[5:4] ;default: 2'b0 ; */
/*description: reserved.*/
#define GDMA_AHB_TESTADDR    0x00000003
#define GDMA_AHB_TESTADDR_M  ((GDMA_AHB_TESTADDR_V)<<(GDMA_AHB_TESTADDR_S))
#define GDMA_AHB_TESTADDR_V  0x3
#define GDMA_AHB_TESTADDR_S  4
/* GDMA_AHB_TESTMODE : R/W ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: reserved.*/
#define GDMA_AHB_TESTMODE    0x00000007
#define GDMA_AHB_TESTMODE_M  ((GDMA_AHB_TESTMODE_V)<<(GDMA_AHB_TESTMODE_S))
#define GDMA_AHB_TESTMODE_V  0x7
#define GDMA_AHB_TESTMODE_S  0

#define GDMA_PD_CONF_REG          (DR_REG_GDMA_BASE + 0x3C4)
/* GDMA_DMA_RAM_CLK_FO : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: 1: Force to open the clock and bypass the gate-clock when accessing the RAM in
DMA. 0: A gate-clock will be used when accessing the RAM in DMA..*/
#define GDMA_DMA_RAM_CLK_FO    (BIT(6))
#define GDMA_DMA_RAM_CLK_FO_M  (BIT(6))
#define GDMA_DMA_RAM_CLK_FO_V  0x1
#define GDMA_DMA_RAM_CLK_FO_S  6
/* GDMA_DMA_RAM_FORCE_PU : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: .*/
#define GDMA_DMA_RAM_FORCE_PU    (BIT(5))
#define GDMA_DMA_RAM_FORCE_PU_M  (BIT(5))
#define GDMA_DMA_RAM_FORCE_PU_V  0x1
#define GDMA_DMA_RAM_FORCE_PU_S  5
/* GDMA_DMA_RAM_FORCE_PD : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: power down.*/
#define GDMA_DMA_RAM_FORCE_PD    (BIT(4))
#define GDMA_DMA_RAM_FORCE_PD_M  (BIT(4))
#define GDMA_DMA_RAM_FORCE_PD_V  0x1
#define GDMA_DMA_RAM_FORCE_PD_S  4

#define GDMA_MISC_CONF_REG          (DR_REG_GDMA_BASE + 0x3C8)
/* GDMA_CLK_EN : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: .*/
#define GDMA_CLK_EN    (BIT(4))
#define GDMA_CLK_EN_M  (BIT(4))
#define GDMA_CLK_EN_V  0x1
#define GDMA_CLK_EN_S  4
/* GDMA_ARB_PRI_DIS : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to disable priority arbitration function..*/
#define GDMA_ARB_PRI_DIS    (BIT(2))
#define GDMA_ARB_PRI_DIS_M  (BIT(2))
#define GDMA_ARB_PRI_DIS_V  0x1
#define GDMA_ARB_PRI_DIS_S  2
/* GDMA_AHBM_RST_EXTER : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit, then clear this bit to reset the external ahb FSM..*/
#define GDMA_AHBM_RST_EXTER    (BIT(1))
#define GDMA_AHBM_RST_EXTER_M  (BIT(1))
#define GDMA_AHBM_RST_EXTER_V  0x1
#define GDMA_AHBM_RST_EXTER_S  1
/* GDMA_AHBM_RST_INTER : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit, then clear this bit to reset the internal ahb FSM..*/
#define GDMA_AHBM_RST_INTER    (BIT(0))
#define GDMA_AHBM_RST_INTER_M  (BIT(0))
#define GDMA_AHBM_RST_INTER_V  0x1
#define GDMA_AHBM_RST_INTER_S  0

#define GDMA_IN_SRAM_SIZE_CH0_REG          (DR_REG_GDMA_BASE + 0x3CC)
/* GDMA_IN_SIZE_CH0 : R/W ;bitpos:[6:0] ;default: 7'd14 ; */
/*description: This register is used to configure the size of L2 Tx FIFO for Rx channel 0. 0:16
 bytes. 1:24 bytes. 2:32 bytes. 3: 40 bytes. 4: 48 bytes. 5:56 bytes. 6: 64 byte
s. 7: 72 bytes. 8: 80 bytes..*/
#define GDMA_IN_SIZE_CH0    0x0000007F
#define GDMA_IN_SIZE_CH0_M  ((GDMA_IN_SIZE_CH0_V)<<(GDMA_IN_SIZE_CH0_S))
#define GDMA_IN_SIZE_CH0_V  0x7F
#define GDMA_IN_SIZE_CH0_S  0

#define GDMA_OUT_SRAM_SIZE_CH0_REG          (DR_REG_GDMA_BASE + 0x3D0)
/* GDMA_OUT_SIZE_CH0 : R/W ;bitpos:[6:0] ;default: 7'd14 ; */
/*description: This register is used to configure the size of L2 Tx FIFO for Tx channel 0. 0:16
 bytes. 1:24 bytes. 2:32 bytes. 3: 40 bytes. 4: 48 bytes. 5:56 bytes. 6: 64 byte
s. 7: 72 bytes. 8: 80 bytes..*/
#define GDMA_OUT_SIZE_CH0    0x0000007F
#define GDMA_OUT_SIZE_CH0_M  ((GDMA_OUT_SIZE_CH0_V)<<(GDMA_OUT_SIZE_CH0_S))
#define GDMA_OUT_SIZE_CH0_V  0x7F
#define GDMA_OUT_SIZE_CH0_S  0

#define GDMA_IN_SRAM_SIZE_CH1_REG          (DR_REG_GDMA_BASE + 0x3D4)
/* GDMA_IN_SIZE_CH1 : R/W ;bitpos:[6:0] ;default: 7'd14 ; */
/*description: This register is used to configure the size of L2 Tx FIFO for Rx channel 1. 0:16
 bytes. 1:24 bytes. 2:32 bytes. 3: 40 bytes. 4: 48 bytes. 5:56 bytes. 6: 64 byte
s. 7: 72 bytes. 8: 80 bytes..*/
#define GDMA_IN_SIZE_CH1    0x0000007F
#define GDMA_IN_SIZE_CH1_M  ((GDMA_IN_SIZE_CH1_V)<<(GDMA_IN_SIZE_CH1_S))
#define GDMA_IN_SIZE_CH1_V  0x7F
#define GDMA_IN_SIZE_CH1_S  0

#define GDMA_OUT_SRAM_SIZE_CH1_REG          (DR_REG_GDMA_BASE + 0x3D8)
/* GDMA_OUT_SIZE_CH1 : R/W ;bitpos:[6:0] ;default: 7'd14 ; */
/*description: This register is used to configure the size of L2 Tx FIFO for Tx channel 1. 0:16
 bytes. 1:24 bytes. 2:32 bytes. 3: 40 bytes. 4: 48 bytes. 5:56 bytes. 6: 64 byte
s. 7: 72 bytes. 8: 80 bytes..*/
#define GDMA_OUT_SIZE_CH1    0x0000007F
#define GDMA_OUT_SIZE_CH1_M  ((GDMA_OUT_SIZE_CH1_V)<<(GDMA_OUT_SIZE_CH1_S))
#define GDMA_OUT_SIZE_CH1_V  0x7F
#define GDMA_OUT_SIZE_CH1_S  0

#define GDMA_IN_SRAM_SIZE_CH2_REG          (DR_REG_GDMA_BASE + 0x3DC)
/* GDMA_IN_SIZE_CH2 : R/W ;bitpos:[6:0] ;default: 7'd14 ; */
/*description: This register is used to configure the size of L2 Tx FIFO for Rx channel 2. 0:16
 bytes. 1:24 bytes. 2:32 bytes. 3: 40 bytes. 4: 48 bytes. 5:56 bytes. 6: 64 byte
s. 7: 72 bytes. 8: 80 bytes..*/
#define GDMA_IN_SIZE_CH2    0x0000007F
#define GDMA_IN_SIZE_CH2_M  ((GDMA_IN_SIZE_CH2_V)<<(GDMA_IN_SIZE_CH2_S))
#define GDMA_IN_SIZE_CH2_V  0x7F
#define GDMA_IN_SIZE_CH2_S  0

#define GDMA_OUT_SRAM_SIZE_CH2_REG          (DR_REG_GDMA_BASE + 0x3E0)
/* GDMA_OUT_SIZE_CH2 : R/W ;bitpos:[6:0] ;default: 7'd14 ; */
/*description: This register is used to configure the size of L2 Tx FIFO for Tx channel 2. 0:16
 bytes. 1:24 bytes. 2:32 bytes. 3: 40 bytes. 4: 48 bytes. 5:56 bytes. 6: 64 byte
s. 7: 72 bytes. 8: 80 bytes..*/
#define GDMA_OUT_SIZE_CH2    0x0000007F
#define GDMA_OUT_SIZE_CH2_M  ((GDMA_OUT_SIZE_CH2_V)<<(GDMA_OUT_SIZE_CH2_S))
#define GDMA_OUT_SIZE_CH2_V  0x7F
#define GDMA_OUT_SIZE_CH2_S  0

#define GDMA_IN_SRAM_SIZE_CH3_REG          (DR_REG_GDMA_BASE + 0x3E4)
/* GDMA_IN_SIZE_CH3 : R/W ;bitpos:[6:0] ;default: 7'd14 ; */
/*description: This register is used to configure the size of L2 Tx FIFO for Rx channel 3. 0:16
 bytes. 1:24 bytes. 2:32 bytes. 3: 40 bytes. 4: 48 bytes. 5:56 bytes. 6: 64 byte
s. 7: 72 bytes. 8: 80 bytes..*/
#define GDMA_IN_SIZE_CH3    0x0000007F
#define GDMA_IN_SIZE_CH3_M  ((GDMA_IN_SIZE_CH3_V)<<(GDMA_IN_SIZE_CH3_S))
#define GDMA_IN_SIZE_CH3_V  0x7F
#define GDMA_IN_SIZE_CH3_S  0

#define GDMA_OUT_SRAM_SIZE_CH3_REG          (DR_REG_GDMA_BASE + 0x3E8)
/* GDMA_OUT_SIZE_CH3 : R/W ;bitpos:[6:0] ;default: 7'd14 ; */
/*description: This register is used to configure the size of L2 Tx FIFO for Tx channel 3. 0:16
 bytes. 1:24 bytes. 2:32 bytes. 3: 40 bytes. 4: 48 bytes. 5:56 bytes. 6: 64 byte
s. 7: 72 bytes. 8: 80 bytes..*/
#define GDMA_OUT_SIZE_CH3    0x0000007F
#define GDMA_OUT_SIZE_CH3_M  ((GDMA_OUT_SIZE_CH3_V)<<(GDMA_OUT_SIZE_CH3_S))
#define GDMA_OUT_SIZE_CH3_V  0x7F
#define GDMA_OUT_SIZE_CH3_S  0

#define GDMA_IN_SRAM_SIZE_CH4_REG          (DR_REG_GDMA_BASE + 0x3EC)
/* GDMA_IN_SIZE_CH4 : R/W ;bitpos:[6:0] ;default: 7'd14 ; */
/*description: This register is used to configure the size of L2 Tx FIFO for Rx channel 4. 0:16
 bytes. 1:24 bytes. 2:32 bytes. 3: 40 bytes. 4: 48 bytes. 5:56 bytes. 6: 64 byte
s. 7: 72 bytes. 8: 80 bytes..*/
#define GDMA_IN_SIZE_CH4    0x0000007F
#define GDMA_IN_SIZE_CH4_M  ((GDMA_IN_SIZE_CH4_V)<<(GDMA_IN_SIZE_CH4_S))
#define GDMA_IN_SIZE_CH4_V  0x7F
#define GDMA_IN_SIZE_CH4_S  0

#define GDMA_OUT_SRAM_SIZE_CH4_REG          (DR_REG_GDMA_BASE + 0x3F0)
/* GDMA_OUT_SIZE_CH4 : R/W ;bitpos:[6:0] ;default: 7'd14 ; */
/*description: This register is used to configure the size of L2 Tx FIFO for Tx channel 4. 0:16
 bytes. 1:24 bytes. 2:32 bytes. 3: 40 bytes. 4: 48 bytes. 5:56 bytes. 6: 64 byte
s. 7: 72 bytes. 8: 80 bytes..*/
#define GDMA_OUT_SIZE_CH4    0x0000007F
#define GDMA_OUT_SIZE_CH4_M  ((GDMA_OUT_SIZE_CH4_V)<<(GDMA_OUT_SIZE_CH4_S))
#define GDMA_OUT_SIZE_CH4_V  0x7F
#define GDMA_OUT_SIZE_CH4_S  0

#define GDMA_EXTMEM_REJECT_ADDR_REG          (DR_REG_GDMA_BASE + 0x3F4)
/* GDMA_EXTMEM_REJECT_ADDR : RO ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: This register store the first address rejected by permission control when
accessing external RAM..*/
#define GDMA_EXTMEM_REJECT_ADDR    0xFFFFFFFF
#define GDMA_EXTMEM_REJECT_ADDR_M  ((GDMA_EXTMEM_REJECT_ADDR_V)<<(GDMA_EXTMEM_REJECT_ADDR_S))
#define GDMA_EXTMEM_REJECT_ADDR_V  0xFFFFFFFF
#define GDMA_EXTMEM_REJECT_ADDR_S  0

#define GDMA_EXTMEM_REJECT_ST_REG          (DR_REG_GDMA_BASE + 0x3F8)
/* GDMA_EXTMEM_REJECT_PERI_NUM : RO ;bitpos:[11:6] ;default: 6'b0 ; */
/*description: This register indicate reject accessing from which peripheral..*/
#define GDMA_EXTMEM_REJECT_PERI_NUM    0x0000003F
#define GDMA_EXTMEM_REJECT_PERI_NUM_M  ((GDMA_EXTMEM_REJECT_PERI_NUM_V)<<(GDMA_EXTMEM_REJECT_PERI_NUM_S))
#define GDMA_EXTMEM_REJECT_PERI_NUM_V  0x3F
#define GDMA_EXTMEM_REJECT_PERI_NUM_S  6
/* GDMA_EXTMEM_REJECT_CHANNEL_NUM : RO ;bitpos:[5:2] ;default: 4'b0 ; */
/*description: The register indicate the reject accessing from which channel..*/
#define GDMA_EXTMEM_REJECT_CHANNEL_NUM    0x0000000F
#define GDMA_EXTMEM_REJECT_CHANNEL_NUM_M  ((GDMA_EXTMEM_REJECT_CHANNEL_NUM_V)<<(GDMA_EXTMEM_REJECT_CHANNEL_NUM_S))
#define GDMA_EXTMEM_REJECT_CHANNEL_NUM_V  0xF
#define GDMA_EXTMEM_REJECT_CHANNEL_NUM_S  2
/* GDMA_EXTMEM_REJECT_ATTR : RO ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: The reject accessing. Bit 0: if this bit is 1, the rejected accessing is READ.
Bit 1: if this bit is 1, the rejected accessing is WRITE..*/
#define GDMA_EXTMEM_REJECT_ATTR    0x00000003
#define GDMA_EXTMEM_REJECT_ATTR_M  ((GDMA_EXTMEM_REJECT_ATTR_V)<<(GDMA_EXTMEM_REJECT_ATTR_S))
#define GDMA_EXTMEM_REJECT_ATTR_V  0x3
#define GDMA_EXTMEM_REJECT_ATTR_S  0

#define GDMA_EXTMEM_REJECT_INT_RAW_REG          (DR_REG_GDMA_BASE + 0x3FC)
/* GDMA_EXTMEM_REJECT_INT_RAW : R/WTC/SS ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when accessing external RAM is
rejected by permission control..*/
#define GDMA_EXTMEM_REJECT_INT_RAW    (BIT(0))
#define GDMA_EXTMEM_REJECT_INT_RAW_M  (BIT(0))
#define GDMA_EXTMEM_REJECT_INT_RAW_V  0x1
#define GDMA_EXTMEM_REJECT_INT_RAW_S  0

#define GDMA_EXTMEM_REJECT_INT_ST_REG          (DR_REG_GDMA_BASE + 0x400)
/* GDMA_EXTMEM_REJECT_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the EXTMEM_REJECT_INT interrupt..*/
#define GDMA_EXTMEM_REJECT_INT_ST    (BIT(0))
#define GDMA_EXTMEM_REJECT_INT_ST_M  (BIT(0))
#define GDMA_EXTMEM_REJECT_INT_ST_V  0x1
#define GDMA_EXTMEM_REJECT_INT_ST_S  0

#define GDMA_EXTMEM_REJECT_INT_ENA_REG          (DR_REG_GDMA_BASE + 0x404)
/* GDMA_EXTMEM_REJECT_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the EXTMEM_REJECT_INT interrupt..*/
#define GDMA_EXTMEM_REJECT_INT_ENA    (BIT(0))
#define GDMA_EXTMEM_REJECT_INT_ENA_M  (BIT(0))
#define GDMA_EXTMEM_REJECT_INT_ENA_V  0x1
#define GDMA_EXTMEM_REJECT_INT_ENA_S  0

#define GDMA_EXTMEM_REJECT_INT_CLR_REG          (DR_REG_GDMA_BASE + 0x408)
/* GDMA_EXTMEM_REJECT_INT_CLR : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the EXTMEM_REJECT_INT interrupt..*/
#define GDMA_EXTMEM_REJECT_INT_CLR    (BIT(0))
#define GDMA_EXTMEM_REJECT_INT_CLR_M  (BIT(0))
#define GDMA_EXTMEM_REJECT_INT_CLR_V  0x1
#define GDMA_EXTMEM_REJECT_INT_CLR_S  0

#define GDMA_DATE_REG          (DR_REG_GDMA_BASE + 0x40C)
/* GDMA_DATE : R/W ;bitpos:[31:0] ;default: 32'h2101180 ; */
/*description: register version..*/
#define GDMA_DATE    0xFFFFFFFF
#define GDMA_DATE_M  ((GDMA_DATE_V)<<(GDMA_DATE_S))
#define GDMA_DATE_V  0xFFFFFFFF
#define GDMA_DATE_S  0


#ifdef __cplusplus
}
#endif



#endif /*_SOC_GDMA_REG_H_ */
