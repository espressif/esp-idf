/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** SDIO_SLC_CONF0_REG register
 *  Sdio slave DMA configuration register.
 */
#define SDIO_SLC_CONF0_REG (DR_REG_SLC_BASE + 0x0)
/** SDIO_SLC0_TX_RST : R/W; bitpos: [0]; default: 0;
 *  Set 1 to reset receiving fsm in dma channel0.
 */
#define SDIO_SLC0_TX_RST    (BIT(0))
#define SDIO_SLC0_TX_RST_M  (SDIO_SLC0_TX_RST_V << SDIO_SLC0_TX_RST_S)
#define SDIO_SLC0_TX_RST_V  0x00000001U
#define SDIO_SLC0_TX_RST_S  0
/** SDIO_SLC0_RX_RST : R/W; bitpos: [1]; default: 0;
 *  Set 1 to reset transmitting fsm in dma channel0.
 */
#define SDIO_SLC0_RX_RST    (BIT(1))
#define SDIO_SLC0_RX_RST_M  (SDIO_SLC0_RX_RST_V << SDIO_SLC0_RX_RST_S)
#define SDIO_SLC0_RX_RST_V  0x00000001U
#define SDIO_SLC0_RX_RST_S  1
/** SDIO_SLC_AHBM_FIFO_RST : R/W; bitpos: [2]; default: 0;
 *  reset the command fifo of AHB bus of sdio slave
 */
#define SDIO_SLC_AHBM_FIFO_RST    (BIT(2))
#define SDIO_SLC_AHBM_FIFO_RST_M  (SDIO_SLC_AHBM_FIFO_RST_V << SDIO_SLC_AHBM_FIFO_RST_S)
#define SDIO_SLC_AHBM_FIFO_RST_V  0x00000001U
#define SDIO_SLC_AHBM_FIFO_RST_S  2
/** SDIO_SLC_AHBM_RST : R/W; bitpos: [3]; default: 0;
 *  reset the AHB bus of sdio slave
 */
#define SDIO_SLC_AHBM_RST    (BIT(3))
#define SDIO_SLC_AHBM_RST_M  (SDIO_SLC_AHBM_RST_V << SDIO_SLC_AHBM_RST_S)
#define SDIO_SLC_AHBM_RST_V  0x00000001U
#define SDIO_SLC_AHBM_RST_S  3
/** SDIO_SLC0_TX_LOOP_TEST : R/W; bitpos: [4]; default: 0;
 *  Loop around when the slave buffer finishes receiving packets. When set to 1,
 *  hardware will not change the owner bit in the linked list.
 */
#define SDIO_SLC0_TX_LOOP_TEST    (BIT(4))
#define SDIO_SLC0_TX_LOOP_TEST_M  (SDIO_SLC0_TX_LOOP_TEST_V << SDIO_SLC0_TX_LOOP_TEST_S)
#define SDIO_SLC0_TX_LOOP_TEST_V  0x00000001U
#define SDIO_SLC0_TX_LOOP_TEST_S  4
/** SDIO_SLC0_RX_LOOP_TEST : R/W; bitpos: [5]; default: 0;
 *  Loop around when the slave buffer finishes sending packets. When set to 1, hardware
 *  will not change the owner bit in the linked list.
 */
#define SDIO_SLC0_RX_LOOP_TEST    (BIT(5))
#define SDIO_SLC0_RX_LOOP_TEST_M  (SDIO_SLC0_RX_LOOP_TEST_V << SDIO_SLC0_RX_LOOP_TEST_S)
#define SDIO_SLC0_RX_LOOP_TEST_V  0x00000001U
#define SDIO_SLC0_RX_LOOP_TEST_S  5
/** SDIO_SLC0_RX_AUTO_WRBACK : R/W; bitpos: [6]; default: 0;
 *  Set 1 to enable change the owner bit of rx link descriptor
 */
#define SDIO_SLC0_RX_AUTO_WRBACK    (BIT(6))
#define SDIO_SLC0_RX_AUTO_WRBACK_M  (SDIO_SLC0_RX_AUTO_WRBACK_V << SDIO_SLC0_RX_AUTO_WRBACK_S)
#define SDIO_SLC0_RX_AUTO_WRBACK_V  0x00000001U
#define SDIO_SLC0_RX_AUTO_WRBACK_S  6
/** SDIO_SLC0_RX_NO_RESTART_CLR : R/W; bitpos: [7]; default: 0;
 *  Set this bit to disable the function that when slave starts to send data to host,
 *  the slc0_bt_rx_new_packet_int_raw or slc0_host_rd_retry_int_raw will be
 *  automatically cleared.
 */
#define SDIO_SLC0_RX_NO_RESTART_CLR    (BIT(7))
#define SDIO_SLC0_RX_NO_RESTART_CLR_M  (SDIO_SLC0_RX_NO_RESTART_CLR_V << SDIO_SLC0_RX_NO_RESTART_CLR_S)
#define SDIO_SLC0_RX_NO_RESTART_CLR_V  0x00000001U
#define SDIO_SLC0_RX_NO_RESTART_CLR_S  7
/** SDIO_SLC0_RXDSCR_BURST_EN : R/W; bitpos: [8]; default: 1;
 *  0- AHB burst type is single when slave read rx-descriptor from memory through
 *  slc0,1-AHB burst type is not single when slave read rx-descriptor from memory
 *  through slc0
 */
#define SDIO_SLC0_RXDSCR_BURST_EN    (BIT(8))
#define SDIO_SLC0_RXDSCR_BURST_EN_M  (SDIO_SLC0_RXDSCR_BURST_EN_V << SDIO_SLC0_RXDSCR_BURST_EN_S)
#define SDIO_SLC0_RXDSCR_BURST_EN_V  0x00000001U
#define SDIO_SLC0_RXDSCR_BURST_EN_S  8
/** SDIO_SLC0_RXDATA_BURST_EN : R/W; bitpos: [9]; default: 1;
 *  0- AHB burst type is single when slave receives data from memory,1-AHB burst type
 *  is not single when slave receives data from memory
 */
#define SDIO_SLC0_RXDATA_BURST_EN    (BIT(9))
#define SDIO_SLC0_RXDATA_BURST_EN_M  (SDIO_SLC0_RXDATA_BURST_EN_V << SDIO_SLC0_RXDATA_BURST_EN_S)
#define SDIO_SLC0_RXDATA_BURST_EN_V  0x00000001U
#define SDIO_SLC0_RXDATA_BURST_EN_S  9
/** SDIO_SLC0_RXLINK_AUTO_RET : R/W; bitpos: [10]; default: 1;
 *  enable the function that when host reading packet retries, slc1 will automatically
 *  jump to the start descriptor of the previous packet.
 */
#define SDIO_SLC0_RXLINK_AUTO_RET    (BIT(10))
#define SDIO_SLC0_RXLINK_AUTO_RET_M  (SDIO_SLC0_RXLINK_AUTO_RET_V << SDIO_SLC0_RXLINK_AUTO_RET_S)
#define SDIO_SLC0_RXLINK_AUTO_RET_V  0x00000001U
#define SDIO_SLC0_RXLINK_AUTO_RET_S  10
/** SDIO_SLC0_TXLINK_AUTO_RET : R/W; bitpos: [11]; default: 1;
 *  enable the function that when host sending packet retries, slc1 will automatically
 *  jump to the start descriptor of the previous packet.
 */
#define SDIO_SLC0_TXLINK_AUTO_RET    (BIT(11))
#define SDIO_SLC0_TXLINK_AUTO_RET_M  (SDIO_SLC0_TXLINK_AUTO_RET_V << SDIO_SLC0_TXLINK_AUTO_RET_S)
#define SDIO_SLC0_TXLINK_AUTO_RET_V  0x00000001U
#define SDIO_SLC0_TXLINK_AUTO_RET_S  11
/** SDIO_SLC0_TXDSCR_BURST_EN : R/W; bitpos: [12]; default: 1;
 *  0- AHB burst type is single when slave read tx-descriptor from memory through
 *  slc0,1-AHB burst type is not single when slave read tx-descriptor from memory
 *  through slc0
 */
#define SDIO_SLC0_TXDSCR_BURST_EN    (BIT(12))
#define SDIO_SLC0_TXDSCR_BURST_EN_M  (SDIO_SLC0_TXDSCR_BURST_EN_V << SDIO_SLC0_TXDSCR_BURST_EN_S)
#define SDIO_SLC0_TXDSCR_BURST_EN_V  0x00000001U
#define SDIO_SLC0_TXDSCR_BURST_EN_S  12
/** SDIO_SLC0_TXDATA_BURST_EN : R/W; bitpos: [13]; default: 1;
 *  0- AHB burst type is single when slave send data to memory,1-AHB burst type is not
 *  single when slave send data to memory
 */
#define SDIO_SLC0_TXDATA_BURST_EN    (BIT(13))
#define SDIO_SLC0_TXDATA_BURST_EN_M  (SDIO_SLC0_TXDATA_BURST_EN_V << SDIO_SLC0_TXDATA_BURST_EN_S)
#define SDIO_SLC0_TXDATA_BURST_EN_V  0x00000001U
#define SDIO_SLC0_TXDATA_BURST_EN_S  13
/** SDIO_SLC0_TOKEN_AUTO_CLR : R/W; bitpos: [14]; default: 1;
 *  auto clear slc0_token1 enable
 */
#define SDIO_SLC0_TOKEN_AUTO_CLR    (BIT(14))
#define SDIO_SLC0_TOKEN_AUTO_CLR_M  (SDIO_SLC0_TOKEN_AUTO_CLR_V << SDIO_SLC0_TOKEN_AUTO_CLR_S)
#define SDIO_SLC0_TOKEN_AUTO_CLR_V  0x00000001U
#define SDIO_SLC0_TOKEN_AUTO_CLR_S  14
/** SDIO_SLC0_TOKEN_SEL : R/W; bitpos: [15]; default: 1;
 *  0: choose to save slc0_token0's value, 1: choose to save the accumulative value of
 *  slc0_token1
 */
#define SDIO_SLC0_TOKEN_SEL    (BIT(15))
#define SDIO_SLC0_TOKEN_SEL_M  (SDIO_SLC0_TOKEN_SEL_V << SDIO_SLC0_TOKEN_SEL_S)
#define SDIO_SLC0_TOKEN_SEL_V  0x00000001U
#define SDIO_SLC0_TOKEN_SEL_S  15
/** SDIO_SLC1_TX_RST : R/W; bitpos: [16]; default: 0;
 *  Set 1 to reset receiving fsm in dma slc0.
 */
#define SDIO_SLC1_TX_RST    (BIT(16))
#define SDIO_SLC1_TX_RST_M  (SDIO_SLC1_TX_RST_V << SDIO_SLC1_TX_RST_S)
#define SDIO_SLC1_TX_RST_V  0x00000001U
#define SDIO_SLC1_TX_RST_S  16
/** SDIO_SLC1_RX_RST : R/W; bitpos: [17]; default: 0;
 *  Set 1 to reset sending fsm in dma slc0.
 */
#define SDIO_SLC1_RX_RST    (BIT(17))
#define SDIO_SLC1_RX_RST_M  (SDIO_SLC1_RX_RST_V << SDIO_SLC1_RX_RST_S)
#define SDIO_SLC1_RX_RST_V  0x00000001U
#define SDIO_SLC1_RX_RST_S  17
/** SDIO_SLC0_WR_RETRY_MASK_EN : R/W; bitpos: [18]; default: 1;
 *  Set this bit to generate an interrupt when host sending retry finishes and prevent
 *  host still retrying after one successful retry through dma channel0
 */
#define SDIO_SLC0_WR_RETRY_MASK_EN    (BIT(18))
#define SDIO_SLC0_WR_RETRY_MASK_EN_M  (SDIO_SLC0_WR_RETRY_MASK_EN_V << SDIO_SLC0_WR_RETRY_MASK_EN_S)
#define SDIO_SLC0_WR_RETRY_MASK_EN_V  0x00000001U
#define SDIO_SLC0_WR_RETRY_MASK_EN_S  18
/** SDIO_SLC1_WR_RETRY_MASK_EN : R/W; bitpos: [19]; default: 1;
 *  Set this bit to generate an interrupt when host sending retry finishes and prevent
 *  host still retrying after one successful retry through dma channel0
 */
#define SDIO_SLC1_WR_RETRY_MASK_EN    (BIT(19))
#define SDIO_SLC1_WR_RETRY_MASK_EN_M  (SDIO_SLC1_WR_RETRY_MASK_EN_V << SDIO_SLC1_WR_RETRY_MASK_EN_S)
#define SDIO_SLC1_WR_RETRY_MASK_EN_V  0x00000001U
#define SDIO_SLC1_WR_RETRY_MASK_EN_S  19
/** SDIO_SLC1_TX_LOOP_TEST : R/W; bitpos: [20]; default: 1;
 *  owner control when slc1 writes back tx descriptor: 0- cpu is owner, 1-dma is owner.
 */
#define SDIO_SLC1_TX_LOOP_TEST    (BIT(20))
#define SDIO_SLC1_TX_LOOP_TEST_M  (SDIO_SLC1_TX_LOOP_TEST_V << SDIO_SLC1_TX_LOOP_TEST_S)
#define SDIO_SLC1_TX_LOOP_TEST_V  0x00000001U
#define SDIO_SLC1_TX_LOOP_TEST_S  20
/** SDIO_SLC1_RX_LOOP_TEST : R/W; bitpos: [21]; default: 1;
 *  owner control when slc1 writes back rx descriptor: 0- cpu is owner, 1-dma is owner.
 */
#define SDIO_SLC1_RX_LOOP_TEST    (BIT(21))
#define SDIO_SLC1_RX_LOOP_TEST_M  (SDIO_SLC1_RX_LOOP_TEST_V << SDIO_SLC1_RX_LOOP_TEST_S)
#define SDIO_SLC1_RX_LOOP_TEST_V  0x00000001U
#define SDIO_SLC1_RX_LOOP_TEST_S  21
/** SDIO_SLC1_RX_AUTO_WRBACK : R/W; bitpos: [22]; default: 0;
 *  Set 1 to enable change the owner bit of the transmitting buffer's linked list when
 *  transmitting data.
 */
#define SDIO_SLC1_RX_AUTO_WRBACK    (BIT(22))
#define SDIO_SLC1_RX_AUTO_WRBACK_M  (SDIO_SLC1_RX_AUTO_WRBACK_V << SDIO_SLC1_RX_AUTO_WRBACK_S)
#define SDIO_SLC1_RX_AUTO_WRBACK_V  0x00000001U
#define SDIO_SLC1_RX_AUTO_WRBACK_S  22
/** SDIO_SLC1_RX_NO_RESTART_CLR : R/W; bitpos: [23]; default: 0;
 *  Set this bit to disable the function that when slave starts to send data to host,
 *  the slc1_bt_rx_new_packet_int_raw, slc1_wifi_rx_new_packet_int_raw or
 *  slc1_host_rd_retry_int_raw will be automatically cleared.
 */
#define SDIO_SLC1_RX_NO_RESTART_CLR    (BIT(23))
#define SDIO_SLC1_RX_NO_RESTART_CLR_M  (SDIO_SLC1_RX_NO_RESTART_CLR_V << SDIO_SLC1_RX_NO_RESTART_CLR_S)
#define SDIO_SLC1_RX_NO_RESTART_CLR_V  0x00000001U
#define SDIO_SLC1_RX_NO_RESTART_CLR_S  23
/** SDIO_SLC1_RXDSCR_BURST_EN : R/W; bitpos: [24]; default: 1;
 *  0- AHB burst type is single when dma channel1 fetches linked list for sending
 *  data,1-AHB burst type is not single when dma channel1 fetches linked list for
 *  sending data
 */
#define SDIO_SLC1_RXDSCR_BURST_EN    (BIT(24))
#define SDIO_SLC1_RXDSCR_BURST_EN_M  (SDIO_SLC1_RXDSCR_BURST_EN_V << SDIO_SLC1_RXDSCR_BURST_EN_S)
#define SDIO_SLC1_RXDSCR_BURST_EN_V  0x00000001U
#define SDIO_SLC1_RXDSCR_BURST_EN_S  24
/** SDIO_SLC1_RXDATA_BURST_EN : R/W; bitpos: [25]; default: 1;
 *  0- AHB burst type is single when slave receives data through dma channel1,1-AHB
 *  burst type is not single when slave receives data through dma channel1
 */
#define SDIO_SLC1_RXDATA_BURST_EN    (BIT(25))
#define SDIO_SLC1_RXDATA_BURST_EN_M  (SDIO_SLC1_RXDATA_BURST_EN_V << SDIO_SLC1_RXDATA_BURST_EN_S)
#define SDIO_SLC1_RXDATA_BURST_EN_V  0x00000001U
#define SDIO_SLC1_RXDATA_BURST_EN_S  25
/** SDIO_SLC1_RXLINK_AUTO_RET : R/W; bitpos: [26]; default: 1;
 *  enable the function that when host reading packet retries, dma channel1 will
 *  automatically jump to the start descriptor of the previous packet.
 */
#define SDIO_SLC1_RXLINK_AUTO_RET    (BIT(26))
#define SDIO_SLC1_RXLINK_AUTO_RET_M  (SDIO_SLC1_RXLINK_AUTO_RET_V << SDIO_SLC1_RXLINK_AUTO_RET_S)
#define SDIO_SLC1_RXLINK_AUTO_RET_V  0x00000001U
#define SDIO_SLC1_RXLINK_AUTO_RET_S  26
/** SDIO_SLC1_TXLINK_AUTO_RET : R/W; bitpos: [27]; default: 1;
 *  enable the function that when host sending packet retries, dma channel1 will
 *  automatically jump to the start descriptor of the previous packet.
 */
#define SDIO_SLC1_TXLINK_AUTO_RET    (BIT(27))
#define SDIO_SLC1_TXLINK_AUTO_RET_M  (SDIO_SLC1_TXLINK_AUTO_RET_V << SDIO_SLC1_TXLINK_AUTO_RET_S)
#define SDIO_SLC1_TXLINK_AUTO_RET_V  0x00000001U
#define SDIO_SLC1_TXLINK_AUTO_RET_S  27
/** SDIO_SLC1_TXDSCR_BURST_EN : R/W; bitpos: [28]; default: 1;
 *  0- AHB burst type is single when dma channel1 fetches linked list for receiving
 *  data, 1-AHB burst type is not single when dma channel1 fetches linked list for
 *  receiving data
 */
#define SDIO_SLC1_TXDSCR_BURST_EN    (BIT(28))
#define SDIO_SLC1_TXDSCR_BURST_EN_M  (SDIO_SLC1_TXDSCR_BURST_EN_V << SDIO_SLC1_TXDSCR_BURST_EN_S)
#define SDIO_SLC1_TXDSCR_BURST_EN_V  0x00000001U
#define SDIO_SLC1_TXDSCR_BURST_EN_S  28
/** SDIO_SLC1_TXDATA_BURST_EN : R/W; bitpos: [29]; default: 1;
 *  0- AHB burst type is single when slave sends data to memory through dma channel1,
 *  1-AHB burst type is not single when slave sends data to memory through dma channel1.
 */
#define SDIO_SLC1_TXDATA_BURST_EN    (BIT(29))
#define SDIO_SLC1_TXDATA_BURST_EN_M  (SDIO_SLC1_TXDATA_BURST_EN_V << SDIO_SLC1_TXDATA_BURST_EN_S)
#define SDIO_SLC1_TXDATA_BURST_EN_V  0x00000001U
#define SDIO_SLC1_TXDATA_BURST_EN_S  29
/** SDIO_SLC1_TOKEN_SEL : R/W; bitpos: [31]; default: 1;
 *  reserved
 */
#define SDIO_SLC1_TOKEN_SEL    (BIT(31))
#define SDIO_SLC1_TOKEN_SEL_M  (SDIO_SLC1_TOKEN_SEL_V << SDIO_SLC1_TOKEN_SEL_S)
#define SDIO_SLC1_TOKEN_SEL_V  0x00000001U
#define SDIO_SLC1_TOKEN_SEL_S  31

/** SDIO_SLC_SLC0INT_RAW_REG register
 *  Sdio slave DMA channel0 raw interrupt status register.
 */
#define SDIO_SLC_SLC0INT_RAW_REG (DR_REG_SLC_BASE + 0x4)
/** SDIO_SLC_FRHOST_BIT0_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit 0 for host to interrupt slave.
 */
#define SDIO_SLC_FRHOST_BIT0_INT_RAW    (BIT(0))
#define SDIO_SLC_FRHOST_BIT0_INT_RAW_M  (SDIO_SLC_FRHOST_BIT0_INT_RAW_V << SDIO_SLC_FRHOST_BIT0_INT_RAW_S)
#define SDIO_SLC_FRHOST_BIT0_INT_RAW_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT0_INT_RAW_S  0
/** SDIO_SLC_FRHOST_BIT1_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit 1 for host to interrupt slave.
 */
#define SDIO_SLC_FRHOST_BIT1_INT_RAW    (BIT(1))
#define SDIO_SLC_FRHOST_BIT1_INT_RAW_M  (SDIO_SLC_FRHOST_BIT1_INT_RAW_V << SDIO_SLC_FRHOST_BIT1_INT_RAW_S)
#define SDIO_SLC_FRHOST_BIT1_INT_RAW_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT1_INT_RAW_S  1
/** SDIO_SLC_FRHOST_BIT2_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit 2 for host to interrupt slave.
 */
#define SDIO_SLC_FRHOST_BIT2_INT_RAW    (BIT(2))
#define SDIO_SLC_FRHOST_BIT2_INT_RAW_M  (SDIO_SLC_FRHOST_BIT2_INT_RAW_V << SDIO_SLC_FRHOST_BIT2_INT_RAW_S)
#define SDIO_SLC_FRHOST_BIT2_INT_RAW_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT2_INT_RAW_S  2
/** SDIO_SLC_FRHOST_BIT3_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit 3 for host to interrupt slave.
 */
#define SDIO_SLC_FRHOST_BIT3_INT_RAW    (BIT(3))
#define SDIO_SLC_FRHOST_BIT3_INT_RAW_M  (SDIO_SLC_FRHOST_BIT3_INT_RAW_V << SDIO_SLC_FRHOST_BIT3_INT_RAW_S)
#define SDIO_SLC_FRHOST_BIT3_INT_RAW_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT3_INT_RAW_S  3
/** SDIO_SLC_FRHOST_BIT4_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit 4 for host to interrupt slave.
 */
#define SDIO_SLC_FRHOST_BIT4_INT_RAW    (BIT(4))
#define SDIO_SLC_FRHOST_BIT4_INT_RAW_M  (SDIO_SLC_FRHOST_BIT4_INT_RAW_V << SDIO_SLC_FRHOST_BIT4_INT_RAW_S)
#define SDIO_SLC_FRHOST_BIT4_INT_RAW_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT4_INT_RAW_S  4
/** SDIO_SLC_FRHOST_BIT5_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt bit 5 for host to interrupt slave.
 */
#define SDIO_SLC_FRHOST_BIT5_INT_RAW    (BIT(5))
#define SDIO_SLC_FRHOST_BIT5_INT_RAW_M  (SDIO_SLC_FRHOST_BIT5_INT_RAW_V << SDIO_SLC_FRHOST_BIT5_INT_RAW_S)
#define SDIO_SLC_FRHOST_BIT5_INT_RAW_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT5_INT_RAW_S  5
/** SDIO_SLC_FRHOST_BIT6_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt bit 6 for host to interrupt slave.
 */
#define SDIO_SLC_FRHOST_BIT6_INT_RAW    (BIT(6))
#define SDIO_SLC_FRHOST_BIT6_INT_RAW_M  (SDIO_SLC_FRHOST_BIT6_INT_RAW_V << SDIO_SLC_FRHOST_BIT6_INT_RAW_S)
#define SDIO_SLC_FRHOST_BIT6_INT_RAW_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT6_INT_RAW_S  6
/** SDIO_SLC_FRHOST_BIT7_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt bit 7 for host to interrupt slave.
 */
#define SDIO_SLC_FRHOST_BIT7_INT_RAW    (BIT(7))
#define SDIO_SLC_FRHOST_BIT7_INT_RAW_M  (SDIO_SLC_FRHOST_BIT7_INT_RAW_V << SDIO_SLC_FRHOST_BIT7_INT_RAW_S)
#define SDIO_SLC_FRHOST_BIT7_INT_RAW_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT7_INT_RAW_S  7
/** SDIO_SLC0_RX_START_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit of registering dma channel0 sending initialization
 */
#define SDIO_SLC0_RX_START_INT_RAW    (BIT(8))
#define SDIO_SLC0_RX_START_INT_RAW_M  (SDIO_SLC0_RX_START_INT_RAW_V << SDIO_SLC0_RX_START_INT_RAW_S)
#define SDIO_SLC0_RX_START_INT_RAW_V  0x00000001U
#define SDIO_SLC0_RX_START_INT_RAW_S  8
/** SDIO_SLC0_TX_START_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  The raw interrupt bit of registering dma channel0 receiving initialization
 */
#define SDIO_SLC0_TX_START_INT_RAW    (BIT(9))
#define SDIO_SLC0_TX_START_INT_RAW_M  (SDIO_SLC0_TX_START_INT_RAW_V << SDIO_SLC0_TX_START_INT_RAW_S)
#define SDIO_SLC0_TX_START_INT_RAW_V  0x00000001U
#define SDIO_SLC0_TX_START_INT_RAW_S  9
/** SDIO_SLC0_RX_UDF_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  The raw interrupt bit of dma channel0 sending buffer underflow.
 */
#define SDIO_SLC0_RX_UDF_INT_RAW    (BIT(10))
#define SDIO_SLC0_RX_UDF_INT_RAW_M  (SDIO_SLC0_RX_UDF_INT_RAW_V << SDIO_SLC0_RX_UDF_INT_RAW_S)
#define SDIO_SLC0_RX_UDF_INT_RAW_V  0x00000001U
#define SDIO_SLC0_RX_UDF_INT_RAW_S  10
/** SDIO_SLC0_TX_OVF_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  The raw interrupt bit of dma channel0 receiving buffer overflow.
 */
#define SDIO_SLC0_TX_OVF_INT_RAW    (BIT(11))
#define SDIO_SLC0_TX_OVF_INT_RAW_M  (SDIO_SLC0_TX_OVF_INT_RAW_V << SDIO_SLC0_TX_OVF_INT_RAW_S)
#define SDIO_SLC0_TX_OVF_INT_RAW_V  0x00000001U
#define SDIO_SLC0_TX_OVF_INT_RAW_S  11
/** SDIO_SLC0_TOKEN0_1TO0_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  The raw interrupt bit for the value of slc0_token0 becoming to zero.
 */
#define SDIO_SLC0_TOKEN0_1TO0_INT_RAW    (BIT(12))
#define SDIO_SLC0_TOKEN0_1TO0_INT_RAW_M  (SDIO_SLC0_TOKEN0_1TO0_INT_RAW_V << SDIO_SLC0_TOKEN0_1TO0_INT_RAW_S)
#define SDIO_SLC0_TOKEN0_1TO0_INT_RAW_V  0x00000001U
#define SDIO_SLC0_TOKEN0_1TO0_INT_RAW_S  12
/** SDIO_SLC0_TOKEN1_1TO0_INT_RAW : R/WTC/SS; bitpos: [13]; default: 0;
 *  The raw interrupt bit for the value of slc0_token1 becoming to zero.
 */
#define SDIO_SLC0_TOKEN1_1TO0_INT_RAW    (BIT(13))
#define SDIO_SLC0_TOKEN1_1TO0_INT_RAW_M  (SDIO_SLC0_TOKEN1_1TO0_INT_RAW_V << SDIO_SLC0_TOKEN1_1TO0_INT_RAW_S)
#define SDIO_SLC0_TOKEN1_1TO0_INT_RAW_V  0x00000001U
#define SDIO_SLC0_TOKEN1_1TO0_INT_RAW_S  13
/** SDIO_SLC0_TX_DONE_INT_RAW : R/WTC/SS; bitpos: [14]; default: 0;
 *  The raw interrupt bit of dma channel0 finishing receiving data to one single buffer.
 */
#define SDIO_SLC0_TX_DONE_INT_RAW    (BIT(14))
#define SDIO_SLC0_TX_DONE_INT_RAW_M  (SDIO_SLC0_TX_DONE_INT_RAW_V << SDIO_SLC0_TX_DONE_INT_RAW_S)
#define SDIO_SLC0_TX_DONE_INT_RAW_V  0x00000001U
#define SDIO_SLC0_TX_DONE_INT_RAW_S  14
/** SDIO_SLC0_TX_SUC_EOF_INT_RAW : R/WTC/SS; bitpos: [15]; default: 0;
 *  The raw interrupt bit of dma channel0 finishing receiving data
 */
#define SDIO_SLC0_TX_SUC_EOF_INT_RAW    (BIT(15))
#define SDIO_SLC0_TX_SUC_EOF_INT_RAW_M  (SDIO_SLC0_TX_SUC_EOF_INT_RAW_V << SDIO_SLC0_TX_SUC_EOF_INT_RAW_S)
#define SDIO_SLC0_TX_SUC_EOF_INT_RAW_V  0x00000001U
#define SDIO_SLC0_TX_SUC_EOF_INT_RAW_S  15
/** SDIO_SLC0_RX_DONE_INT_RAW : R/WTC/SS; bitpos: [16]; default: 0;
 *  The raw interrupt bit of dma channel0 finishing sending data from one single buffer
 */
#define SDIO_SLC0_RX_DONE_INT_RAW    (BIT(16))
#define SDIO_SLC0_RX_DONE_INT_RAW_M  (SDIO_SLC0_RX_DONE_INT_RAW_V << SDIO_SLC0_RX_DONE_INT_RAW_S)
#define SDIO_SLC0_RX_DONE_INT_RAW_V  0x00000001U
#define SDIO_SLC0_RX_DONE_INT_RAW_S  16
/** SDIO_SLC0_RX_EOF_INT_RAW : R/WTC/SS; bitpos: [17]; default: 0;
 *  The raw interrupt bit of dma channel0 finishing sending data
 */
#define SDIO_SLC0_RX_EOF_INT_RAW    (BIT(17))
#define SDIO_SLC0_RX_EOF_INT_RAW_M  (SDIO_SLC0_RX_EOF_INT_RAW_V << SDIO_SLC0_RX_EOF_INT_RAW_S)
#define SDIO_SLC0_RX_EOF_INT_RAW_V  0x00000001U
#define SDIO_SLC0_RX_EOF_INT_RAW_S  17
/** SDIO_SLC0_TOHOST_INT_RAW : R/WTC/SS; bitpos: [18]; default: 0;
 *  The raw interrupt bit for slave generating interrupt to host.
 */
#define SDIO_SLC0_TOHOST_INT_RAW    (BIT(18))
#define SDIO_SLC0_TOHOST_INT_RAW_M  (SDIO_SLC0_TOHOST_INT_RAW_V << SDIO_SLC0_TOHOST_INT_RAW_S)
#define SDIO_SLC0_TOHOST_INT_RAW_V  0x00000001U
#define SDIO_SLC0_TOHOST_INT_RAW_S  18
/** SDIO_SLC0_TX_DSCR_ERR_INT_RAW : R/WTC/SS; bitpos: [19]; default: 0;
 *  The raw interrupt bit of dma channel0 receiving descriptor error.
 */
#define SDIO_SLC0_TX_DSCR_ERR_INT_RAW    (BIT(19))
#define SDIO_SLC0_TX_DSCR_ERR_INT_RAW_M  (SDIO_SLC0_TX_DSCR_ERR_INT_RAW_V << SDIO_SLC0_TX_DSCR_ERR_INT_RAW_S)
#define SDIO_SLC0_TX_DSCR_ERR_INT_RAW_V  0x00000001U
#define SDIO_SLC0_TX_DSCR_ERR_INT_RAW_S  19
/** SDIO_SLC0_RX_DSCR_ERR_INT_RAW : R/WTC/SS; bitpos: [20]; default: 0;
 *  The raw interrupt bit of dma channel0 sending descriptor error.
 */
#define SDIO_SLC0_RX_DSCR_ERR_INT_RAW    (BIT(20))
#define SDIO_SLC0_RX_DSCR_ERR_INT_RAW_M  (SDIO_SLC0_RX_DSCR_ERR_INT_RAW_V << SDIO_SLC0_RX_DSCR_ERR_INT_RAW_S)
#define SDIO_SLC0_RX_DSCR_ERR_INT_RAW_V  0x00000001U
#define SDIO_SLC0_RX_DSCR_ERR_INT_RAW_S  20
/** SDIO_SLC0_TX_DSCR_EMPTY_INT_RAW : R/WTC/SS; bitpos: [21]; default: 0;
 *  The raw interrupt bit of  not enough buffer for slave receiving data from host
 *  through dma channel0.
 */
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_RAW    (BIT(21))
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_RAW_M  (SDIO_SLC0_TX_DSCR_EMPTY_INT_RAW_V << SDIO_SLC0_TX_DSCR_EMPTY_INT_RAW_S)
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_RAW_V  0x00000001U
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_RAW_S  21
/** SDIO_SLC0_HOST_RD_ACK_INT_RAW : R/WTC/SS; bitpos: [22]; default: 0;
 *  The raw interrupt bit for host to acknowledge retrying receiving data from slave
 *  successfully through dma channel0.
 */
#define SDIO_SLC0_HOST_RD_ACK_INT_RAW    (BIT(22))
#define SDIO_SLC0_HOST_RD_ACK_INT_RAW_M  (SDIO_SLC0_HOST_RD_ACK_INT_RAW_V << SDIO_SLC0_HOST_RD_ACK_INT_RAW_S)
#define SDIO_SLC0_HOST_RD_ACK_INT_RAW_V  0x00000001U
#define SDIO_SLC0_HOST_RD_ACK_INT_RAW_S  22
/** SDIO_SLC0_WR_RETRY_DONE_INT_RAW : R/WTC/SS; bitpos: [23]; default: 0;
 *  The raw interrupt bit of host finishing retrying sending data to slave through dma
 *  channel0.
 */
#define SDIO_SLC0_WR_RETRY_DONE_INT_RAW    (BIT(23))
#define SDIO_SLC0_WR_RETRY_DONE_INT_RAW_M  (SDIO_SLC0_WR_RETRY_DONE_INT_RAW_V << SDIO_SLC0_WR_RETRY_DONE_INT_RAW_S)
#define SDIO_SLC0_WR_RETRY_DONE_INT_RAW_V  0x00000001U
#define SDIO_SLC0_WR_RETRY_DONE_INT_RAW_S  23
/** SDIO_SLC0_TX_ERR_EOF_INT_RAW : R/WTC/SS; bitpos: [24]; default: 0;
 *  The raw interrupt bit of data error during host sending data to slave through dma
 *  channel0.
 */
#define SDIO_SLC0_TX_ERR_EOF_INT_RAW    (BIT(24))
#define SDIO_SLC0_TX_ERR_EOF_INT_RAW_M  (SDIO_SLC0_TX_ERR_EOF_INT_RAW_V << SDIO_SLC0_TX_ERR_EOF_INT_RAW_S)
#define SDIO_SLC0_TX_ERR_EOF_INT_RAW_V  0x00000001U
#define SDIO_SLC0_TX_ERR_EOF_INT_RAW_S  24
/** SDIO_CMD_DTC_INT_RAW : R/WTC/SS; bitpos: [25]; default: 0;
 *  The raw interrupt bit for sdio slave to detect command from host.
 */
#define SDIO_CMD_DTC_INT_RAW    (BIT(25))
#define SDIO_CMD_DTC_INT_RAW_M  (SDIO_CMD_DTC_INT_RAW_V << SDIO_CMD_DTC_INT_RAW_S)
#define SDIO_CMD_DTC_INT_RAW_V  0x00000001U
#define SDIO_CMD_DTC_INT_RAW_S  25
/** SDIO_SLC0_RX_QUICK_EOF_INT_RAW : R/WTC/SS; bitpos: [26]; default: 0;
 *  The raw interrupt for the linked list used reaches the number of
 *  slc0_rx_dscr_rec_lim when in stitch mode, or for every sending data transfer done.
 */
#define SDIO_SLC0_RX_QUICK_EOF_INT_RAW    (BIT(26))
#define SDIO_SLC0_RX_QUICK_EOF_INT_RAW_M  (SDIO_SLC0_RX_QUICK_EOF_INT_RAW_V << SDIO_SLC0_RX_QUICK_EOF_INT_RAW_S)
#define SDIO_SLC0_RX_QUICK_EOF_INT_RAW_V  0x00000001U
#define SDIO_SLC0_RX_QUICK_EOF_INT_RAW_S  26
/** SDIO_SLC0_HOST_POP_EOF_ERR_INT_RAW : R/WTC/SS; bitpos: [27]; default: 0;
 *  The raw interrupt bit for the eof bit error of slave receiving data through dma
 *  channel0.
 */
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_RAW    (BIT(27))
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_RAW_M  (SDIO_SLC0_HOST_POP_EOF_ERR_INT_RAW_V << SDIO_SLC0_HOST_POP_EOF_ERR_INT_RAW_S)
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_RAW_V  0x00000001U
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_RAW_S  27
/** SDIO_HDA_RECV_DONE_INT_RAW : R/WTC/SS; bitpos: [28]; default: 0;
 *  The raw interrupt bit for slave finishes receiving one cmd53 function's all block
 *  data.
 */
#define SDIO_HDA_RECV_DONE_INT_RAW    (BIT(28))
#define SDIO_HDA_RECV_DONE_INT_RAW_M  (SDIO_HDA_RECV_DONE_INT_RAW_V << SDIO_HDA_RECV_DONE_INT_RAW_S)
#define SDIO_HDA_RECV_DONE_INT_RAW_V  0x00000001U
#define SDIO_HDA_RECV_DONE_INT_RAW_S  28

/** SDIO_SLC_SLC0INT_ST_REG register
 *  Sdio slave DMA channel0 masked interrupt status register.
 */
#define SDIO_SLC_SLC0INT_ST_REG (DR_REG_SLC_BASE + 0x8)
/** SDIO_SLC_FRHOST_BIT0_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status bit for SLC_FRHOST_BIT0_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT0_INT_ST    (BIT(0))
#define SDIO_SLC_FRHOST_BIT0_INT_ST_M  (SDIO_SLC_FRHOST_BIT0_INT_ST_V << SDIO_SLC_FRHOST_BIT0_INT_ST_S)
#define SDIO_SLC_FRHOST_BIT0_INT_ST_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT0_INT_ST_S  0
/** SDIO_SLC_FRHOST_BIT1_INT_ST : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status bit for SLC_FRHOST_BIT1_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT1_INT_ST    (BIT(1))
#define SDIO_SLC_FRHOST_BIT1_INT_ST_M  (SDIO_SLC_FRHOST_BIT1_INT_ST_V << SDIO_SLC_FRHOST_BIT1_INT_ST_S)
#define SDIO_SLC_FRHOST_BIT1_INT_ST_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT1_INT_ST_S  1
/** SDIO_SLC_FRHOST_BIT2_INT_ST : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status bit for SLC_FRHOST_BIT2_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT2_INT_ST    (BIT(2))
#define SDIO_SLC_FRHOST_BIT2_INT_ST_M  (SDIO_SLC_FRHOST_BIT2_INT_ST_V << SDIO_SLC_FRHOST_BIT2_INT_ST_S)
#define SDIO_SLC_FRHOST_BIT2_INT_ST_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT2_INT_ST_S  2
/** SDIO_SLC_FRHOST_BIT3_INT_ST : RO; bitpos: [3]; default: 0;
 *  The masked interrupt status bit for SLC_FRHOST_BIT3_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT3_INT_ST    (BIT(3))
#define SDIO_SLC_FRHOST_BIT3_INT_ST_M  (SDIO_SLC_FRHOST_BIT3_INT_ST_V << SDIO_SLC_FRHOST_BIT3_INT_ST_S)
#define SDIO_SLC_FRHOST_BIT3_INT_ST_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT3_INT_ST_S  3
/** SDIO_SLC_FRHOST_BIT4_INT_ST : RO; bitpos: [4]; default: 0;
 *  The masked interrupt status bit for SLC_FRHOST_BIT4_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT4_INT_ST    (BIT(4))
#define SDIO_SLC_FRHOST_BIT4_INT_ST_M  (SDIO_SLC_FRHOST_BIT4_INT_ST_V << SDIO_SLC_FRHOST_BIT4_INT_ST_S)
#define SDIO_SLC_FRHOST_BIT4_INT_ST_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT4_INT_ST_S  4
/** SDIO_SLC_FRHOST_BIT5_INT_ST : RO; bitpos: [5]; default: 0;
 *  The masked interrupt status bit for SLC_FRHOST_BIT5_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT5_INT_ST    (BIT(5))
#define SDIO_SLC_FRHOST_BIT5_INT_ST_M  (SDIO_SLC_FRHOST_BIT5_INT_ST_V << SDIO_SLC_FRHOST_BIT5_INT_ST_S)
#define SDIO_SLC_FRHOST_BIT5_INT_ST_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT5_INT_ST_S  5
/** SDIO_SLC_FRHOST_BIT6_INT_ST : RO; bitpos: [6]; default: 0;
 *  The masked interrupt status bit for SLC_FRHOST_BIT6_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT6_INT_ST    (BIT(6))
#define SDIO_SLC_FRHOST_BIT6_INT_ST_M  (SDIO_SLC_FRHOST_BIT6_INT_ST_V << SDIO_SLC_FRHOST_BIT6_INT_ST_S)
#define SDIO_SLC_FRHOST_BIT6_INT_ST_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT6_INT_ST_S  6
/** SDIO_SLC_FRHOST_BIT7_INT_ST : RO; bitpos: [7]; default: 0;
 *  The masked interrupt status bit for SLC_FRHOST_BIT7_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT7_INT_ST    (BIT(7))
#define SDIO_SLC_FRHOST_BIT7_INT_ST_M  (SDIO_SLC_FRHOST_BIT7_INT_ST_V << SDIO_SLC_FRHOST_BIT7_INT_ST_S)
#define SDIO_SLC_FRHOST_BIT7_INT_ST_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT7_INT_ST_S  7
/** SDIO_SLC0_RX_START_INT_ST : RO; bitpos: [8]; default: 0;
 *  The masked interrupt status bit for SLC0_RX_START_INT_ST interrupt.
 */
#define SDIO_SLC0_RX_START_INT_ST    (BIT(8))
#define SDIO_SLC0_RX_START_INT_ST_M  (SDIO_SLC0_RX_START_INT_ST_V << SDIO_SLC0_RX_START_INT_ST_S)
#define SDIO_SLC0_RX_START_INT_ST_V  0x00000001U
#define SDIO_SLC0_RX_START_INT_ST_S  8
/** SDIO_SLC0_TX_START_INT_ST : RO; bitpos: [9]; default: 0;
 *  The masked interrupt status bit for SLC0_TX_START_INT_ST interrupt.
 */
#define SDIO_SLC0_TX_START_INT_ST    (BIT(9))
#define SDIO_SLC0_TX_START_INT_ST_M  (SDIO_SLC0_TX_START_INT_ST_V << SDIO_SLC0_TX_START_INT_ST_S)
#define SDIO_SLC0_TX_START_INT_ST_V  0x00000001U
#define SDIO_SLC0_TX_START_INT_ST_S  9
/** SDIO_SLC0_RX_UDF_INT_ST : RO; bitpos: [10]; default: 0;
 *  The masked interrupt status bit for SLC0_RX_UDF_INT interrupt.
 */
#define SDIO_SLC0_RX_UDF_INT_ST    (BIT(10))
#define SDIO_SLC0_RX_UDF_INT_ST_M  (SDIO_SLC0_RX_UDF_INT_ST_V << SDIO_SLC0_RX_UDF_INT_ST_S)
#define SDIO_SLC0_RX_UDF_INT_ST_V  0x00000001U
#define SDIO_SLC0_RX_UDF_INT_ST_S  10
/** SDIO_SLC0_TX_OVF_INT_ST : RO; bitpos: [11]; default: 0;
 *  The masked interrupt status bit for SLC0_TX_OVF_INT interrupt.
 */
#define SDIO_SLC0_TX_OVF_INT_ST    (BIT(11))
#define SDIO_SLC0_TX_OVF_INT_ST_M  (SDIO_SLC0_TX_OVF_INT_ST_V << SDIO_SLC0_TX_OVF_INT_ST_S)
#define SDIO_SLC0_TX_OVF_INT_ST_V  0x00000001U
#define SDIO_SLC0_TX_OVF_INT_ST_S  11
/** SDIO_SLC0_TOKEN0_1TO0_INT_ST : RO; bitpos: [12]; default: 0;
 *  The masked interrupt status bit for SLC0_TOKEN0_1TO0_INT interrupt.
 */
#define SDIO_SLC0_TOKEN0_1TO0_INT_ST    (BIT(12))
#define SDIO_SLC0_TOKEN0_1TO0_INT_ST_M  (SDIO_SLC0_TOKEN0_1TO0_INT_ST_V << SDIO_SLC0_TOKEN0_1TO0_INT_ST_S)
#define SDIO_SLC0_TOKEN0_1TO0_INT_ST_V  0x00000001U
#define SDIO_SLC0_TOKEN0_1TO0_INT_ST_S  12
/** SDIO_SLC0_TOKEN1_1TO0_INT_ST : RO; bitpos: [13]; default: 0;
 *  The masked interrupt status bit for SLC0_TOKEN1_1TO0_INT interrupt.
 */
#define SDIO_SLC0_TOKEN1_1TO0_INT_ST    (BIT(13))
#define SDIO_SLC0_TOKEN1_1TO0_INT_ST_M  (SDIO_SLC0_TOKEN1_1TO0_INT_ST_V << SDIO_SLC0_TOKEN1_1TO0_INT_ST_S)
#define SDIO_SLC0_TOKEN1_1TO0_INT_ST_V  0x00000001U
#define SDIO_SLC0_TOKEN1_1TO0_INT_ST_S  13
/** SDIO_SLC0_TX_DONE_INT_ST : RO; bitpos: [14]; default: 0;
 *  The masked interrupt status bit for SLC0_TX_DONE_INT_ST interrupt.
 */
#define SDIO_SLC0_TX_DONE_INT_ST    (BIT(14))
#define SDIO_SLC0_TX_DONE_INT_ST_M  (SDIO_SLC0_TX_DONE_INT_ST_V << SDIO_SLC0_TX_DONE_INT_ST_S)
#define SDIO_SLC0_TX_DONE_INT_ST_V  0x00000001U
#define SDIO_SLC0_TX_DONE_INT_ST_S  14
/** SDIO_SLC0_TX_SUC_EOF_INT_ST : RO; bitpos: [15]; default: 0;
 *  The masked interrupt status bit for SLC0_TX_SUC_EOF_INT interrupt.
 */
#define SDIO_SLC0_TX_SUC_EOF_INT_ST    (BIT(15))
#define SDIO_SLC0_TX_SUC_EOF_INT_ST_M  (SDIO_SLC0_TX_SUC_EOF_INT_ST_V << SDIO_SLC0_TX_SUC_EOF_INT_ST_S)
#define SDIO_SLC0_TX_SUC_EOF_INT_ST_V  0x00000001U
#define SDIO_SLC0_TX_SUC_EOF_INT_ST_S  15
/** SDIO_SLC0_RX_DONE_INT_ST : RO; bitpos: [16]; default: 0;
 *  The masked interrupt status bit for SLC0_RX_DONE_INT interrupt.
 */
#define SDIO_SLC0_RX_DONE_INT_ST    (BIT(16))
#define SDIO_SLC0_RX_DONE_INT_ST_M  (SDIO_SLC0_RX_DONE_INT_ST_V << SDIO_SLC0_RX_DONE_INT_ST_S)
#define SDIO_SLC0_RX_DONE_INT_ST_V  0x00000001U
#define SDIO_SLC0_RX_DONE_INT_ST_S  16
/** SDIO_SLC0_RX_EOF_INT_ST : RO; bitpos: [17]; default: 0;
 *  The masked interrupt status bit for SLC0_RX_EOF_INT interrupt.
 */
#define SDIO_SLC0_RX_EOF_INT_ST    (BIT(17))
#define SDIO_SLC0_RX_EOF_INT_ST_M  (SDIO_SLC0_RX_EOF_INT_ST_V << SDIO_SLC0_RX_EOF_INT_ST_S)
#define SDIO_SLC0_RX_EOF_INT_ST_V  0x00000001U
#define SDIO_SLC0_RX_EOF_INT_ST_S  17
/** SDIO_SLC0_TOHOST_INT_ST : RO; bitpos: [18]; default: 0;
 *  The masked interrupt status bit for SLC0_TOHOST_INT interrupt.
 */
#define SDIO_SLC0_TOHOST_INT_ST    (BIT(18))
#define SDIO_SLC0_TOHOST_INT_ST_M  (SDIO_SLC0_TOHOST_INT_ST_V << SDIO_SLC0_TOHOST_INT_ST_S)
#define SDIO_SLC0_TOHOST_INT_ST_V  0x00000001U
#define SDIO_SLC0_TOHOST_INT_ST_S  18
/** SDIO_SLC0_TX_DSCR_ERR_INT_ST : RO; bitpos: [19]; default: 0;
 *  The masked interrupt status bit for SLC0_TX_DSCR_ERR_INT interrupt.
 */
#define SDIO_SLC0_TX_DSCR_ERR_INT_ST    (BIT(19))
#define SDIO_SLC0_TX_DSCR_ERR_INT_ST_M  (SDIO_SLC0_TX_DSCR_ERR_INT_ST_V << SDIO_SLC0_TX_DSCR_ERR_INT_ST_S)
#define SDIO_SLC0_TX_DSCR_ERR_INT_ST_V  0x00000001U
#define SDIO_SLC0_TX_DSCR_ERR_INT_ST_S  19
/** SDIO_SLC0_RX_DSCR_ERR_INT_ST : RO; bitpos: [20]; default: 0;
 *  The masked interrupt status bit for SLC0_RX_DSCR_ERR_INT interrupt.
 */
#define SDIO_SLC0_RX_DSCR_ERR_INT_ST    (BIT(20))
#define SDIO_SLC0_RX_DSCR_ERR_INT_ST_M  (SDIO_SLC0_RX_DSCR_ERR_INT_ST_V << SDIO_SLC0_RX_DSCR_ERR_INT_ST_S)
#define SDIO_SLC0_RX_DSCR_ERR_INT_ST_V  0x00000001U
#define SDIO_SLC0_RX_DSCR_ERR_INT_ST_S  20
/** SDIO_SLC0_TX_DSCR_EMPTY_INT_ST : RO; bitpos: [21]; default: 0;
 *  The masked interrupt status bit for SLC0_TX_DSCR_EMPTY_INT interrupt.
 */
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_ST    (BIT(21))
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_ST_M  (SDIO_SLC0_TX_DSCR_EMPTY_INT_ST_V << SDIO_SLC0_TX_DSCR_EMPTY_INT_ST_S)
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_ST_V  0x00000001U
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_ST_S  21
/** SDIO_SLC0_HOST_RD_ACK_INT_ST : RO; bitpos: [22]; default: 0;
 *  The masked interrupt status bit for SLC0_HOST_RD_ACK_INT interrupt.
 */
#define SDIO_SLC0_HOST_RD_ACK_INT_ST    (BIT(22))
#define SDIO_SLC0_HOST_RD_ACK_INT_ST_M  (SDIO_SLC0_HOST_RD_ACK_INT_ST_V << SDIO_SLC0_HOST_RD_ACK_INT_ST_S)
#define SDIO_SLC0_HOST_RD_ACK_INT_ST_V  0x00000001U
#define SDIO_SLC0_HOST_RD_ACK_INT_ST_S  22
/** SDIO_SLC0_WR_RETRY_DONE_INT_ST : RO; bitpos: [23]; default: 0;
 *  The masked interrupt status bit for SLC0_WR_RETRY_DONE_INT interrupt.
 */
#define SDIO_SLC0_WR_RETRY_DONE_INT_ST    (BIT(23))
#define SDIO_SLC0_WR_RETRY_DONE_INT_ST_M  (SDIO_SLC0_WR_RETRY_DONE_INT_ST_V << SDIO_SLC0_WR_RETRY_DONE_INT_ST_S)
#define SDIO_SLC0_WR_RETRY_DONE_INT_ST_V  0x00000001U
#define SDIO_SLC0_WR_RETRY_DONE_INT_ST_S  23
/** SDIO_SLC0_TX_ERR_EOF_INT_ST : RO; bitpos: [24]; default: 0;
 *  The masked interrupt status bit for SLC0_TX_ERR_EOF_INT interrupt.
 */
#define SDIO_SLC0_TX_ERR_EOF_INT_ST    (BIT(24))
#define SDIO_SLC0_TX_ERR_EOF_INT_ST_M  (SDIO_SLC0_TX_ERR_EOF_INT_ST_V << SDIO_SLC0_TX_ERR_EOF_INT_ST_S)
#define SDIO_SLC0_TX_ERR_EOF_INT_ST_V  0x00000001U
#define SDIO_SLC0_TX_ERR_EOF_INT_ST_S  24
/** SDIO_CMD_DTC_INT_ST : RO; bitpos: [25]; default: 0;
 *  The masked interrupt status bit for CMD_DTC_INT interrupt.
 */
#define SDIO_CMD_DTC_INT_ST    (BIT(25))
#define SDIO_CMD_DTC_INT_ST_M  (SDIO_CMD_DTC_INT_ST_V << SDIO_CMD_DTC_INT_ST_S)
#define SDIO_CMD_DTC_INT_ST_V  0x00000001U
#define SDIO_CMD_DTC_INT_ST_S  25
/** SDIO_SLC0_RX_QUICK_EOF_INT_ST : RO; bitpos: [26]; default: 0;
 *  The masked interrupt status bit for SLC0_RX_QUICK_EOF_INT interrupt.
 */
#define SDIO_SLC0_RX_QUICK_EOF_INT_ST    (BIT(26))
#define SDIO_SLC0_RX_QUICK_EOF_INT_ST_M  (SDIO_SLC0_RX_QUICK_EOF_INT_ST_V << SDIO_SLC0_RX_QUICK_EOF_INT_ST_S)
#define SDIO_SLC0_RX_QUICK_EOF_INT_ST_V  0x00000001U
#define SDIO_SLC0_RX_QUICK_EOF_INT_ST_S  26
/** SDIO_SLC0_HOST_POP_EOF_ERR_INT_ST : RO; bitpos: [27]; default: 0;
 *  The masked interrupt status bit for SLC0_HOST_POP_EOF_ERR_INT interrupt.
 */
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_ST    (BIT(27))
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_ST_M  (SDIO_SLC0_HOST_POP_EOF_ERR_INT_ST_V << SDIO_SLC0_HOST_POP_EOF_ERR_INT_ST_S)
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_ST_V  0x00000001U
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_ST_S  27
/** SDIO_HDA_RECV_DONE_INT_ST : RO; bitpos: [28]; default: 0;
 *  The masked interrupt status bit for HDA_RECV_DONE_INT interrupt.
 */
#define SDIO_HDA_RECV_DONE_INT_ST    (BIT(28))
#define SDIO_HDA_RECV_DONE_INT_ST_M  (SDIO_HDA_RECV_DONE_INT_ST_V << SDIO_HDA_RECV_DONE_INT_ST_S)
#define SDIO_HDA_RECV_DONE_INT_ST_V  0x00000001U
#define SDIO_HDA_RECV_DONE_INT_ST_S  28

/** SDIO_SLC_SLC0INT_ENA_REG register
 *  Sdio slave DMA channel0 interrupt enable register.
 */
#define SDIO_SLC_SLC0INT_ENA_REG (DR_REG_SLC_BASE + 0xc)
/** SDIO_SLC_FRHOST_BIT0_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for SLC_FRHOST_BIT0_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT0_INT_ENA    (BIT(0))
#define SDIO_SLC_FRHOST_BIT0_INT_ENA_M  (SDIO_SLC_FRHOST_BIT0_INT_ENA_V << SDIO_SLC_FRHOST_BIT0_INT_ENA_S)
#define SDIO_SLC_FRHOST_BIT0_INT_ENA_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT0_INT_ENA_S  0
/** SDIO_SLC_FRHOST_BIT1_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for SLC_FRHOST_BIT1_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT1_INT_ENA    (BIT(1))
#define SDIO_SLC_FRHOST_BIT1_INT_ENA_M  (SDIO_SLC_FRHOST_BIT1_INT_ENA_V << SDIO_SLC_FRHOST_BIT1_INT_ENA_S)
#define SDIO_SLC_FRHOST_BIT1_INT_ENA_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT1_INT_ENA_S  1
/** SDIO_SLC_FRHOST_BIT2_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for SLC_FRHOST_BIT2_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT2_INT_ENA    (BIT(2))
#define SDIO_SLC_FRHOST_BIT2_INT_ENA_M  (SDIO_SLC_FRHOST_BIT2_INT_ENA_V << SDIO_SLC_FRHOST_BIT2_INT_ENA_S)
#define SDIO_SLC_FRHOST_BIT2_INT_ENA_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT2_INT_ENA_S  2
/** SDIO_SLC_FRHOST_BIT3_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for SLC_FRHOST_BIT3_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT3_INT_ENA    (BIT(3))
#define SDIO_SLC_FRHOST_BIT3_INT_ENA_M  (SDIO_SLC_FRHOST_BIT3_INT_ENA_V << SDIO_SLC_FRHOST_BIT3_INT_ENA_S)
#define SDIO_SLC_FRHOST_BIT3_INT_ENA_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT3_INT_ENA_S  3
/** SDIO_SLC_FRHOST_BIT4_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for SLC_FRHOST_BIT4_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT4_INT_ENA    (BIT(4))
#define SDIO_SLC_FRHOST_BIT4_INT_ENA_M  (SDIO_SLC_FRHOST_BIT4_INT_ENA_V << SDIO_SLC_FRHOST_BIT4_INT_ENA_S)
#define SDIO_SLC_FRHOST_BIT4_INT_ENA_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT4_INT_ENA_S  4
/** SDIO_SLC_FRHOST_BIT5_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for SLC_FRHOST_BIT5_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT5_INT_ENA    (BIT(5))
#define SDIO_SLC_FRHOST_BIT5_INT_ENA_M  (SDIO_SLC_FRHOST_BIT5_INT_ENA_V << SDIO_SLC_FRHOST_BIT5_INT_ENA_S)
#define SDIO_SLC_FRHOST_BIT5_INT_ENA_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT5_INT_ENA_S  5
/** SDIO_SLC_FRHOST_BIT6_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for SLC_FRHOST_BIT6_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT6_INT_ENA    (BIT(6))
#define SDIO_SLC_FRHOST_BIT6_INT_ENA_M  (SDIO_SLC_FRHOST_BIT6_INT_ENA_V << SDIO_SLC_FRHOST_BIT6_INT_ENA_S)
#define SDIO_SLC_FRHOST_BIT6_INT_ENA_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT6_INT_ENA_S  6
/** SDIO_SLC_FRHOST_BIT7_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for SLC_FRHOST_BIT7_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT7_INT_ENA    (BIT(7))
#define SDIO_SLC_FRHOST_BIT7_INT_ENA_M  (SDIO_SLC_FRHOST_BIT7_INT_ENA_V << SDIO_SLC_FRHOST_BIT7_INT_ENA_S)
#define SDIO_SLC_FRHOST_BIT7_INT_ENA_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT7_INT_ENA_S  7
/** SDIO_SLC0_RX_START_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for SLC0_RX_START_INT_ST interrupt.
 */
#define SDIO_SLC0_RX_START_INT_ENA    (BIT(8))
#define SDIO_SLC0_RX_START_INT_ENA_M  (SDIO_SLC0_RX_START_INT_ENA_V << SDIO_SLC0_RX_START_INT_ENA_S)
#define SDIO_SLC0_RX_START_INT_ENA_V  0x00000001U
#define SDIO_SLC0_RX_START_INT_ENA_S  8
/** SDIO_SLC0_TX_START_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for SLC0_TX_START_INT_ST interrupt.
 */
#define SDIO_SLC0_TX_START_INT_ENA    (BIT(9))
#define SDIO_SLC0_TX_START_INT_ENA_M  (SDIO_SLC0_TX_START_INT_ENA_V << SDIO_SLC0_TX_START_INT_ENA_S)
#define SDIO_SLC0_TX_START_INT_ENA_V  0x00000001U
#define SDIO_SLC0_TX_START_INT_ENA_S  9
/** SDIO_SLC0_RX_UDF_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  The interrupt enable bit for SLC0_RX_UDF_INT interrupt.
 */
#define SDIO_SLC0_RX_UDF_INT_ENA    (BIT(10))
#define SDIO_SLC0_RX_UDF_INT_ENA_M  (SDIO_SLC0_RX_UDF_INT_ENA_V << SDIO_SLC0_RX_UDF_INT_ENA_S)
#define SDIO_SLC0_RX_UDF_INT_ENA_V  0x00000001U
#define SDIO_SLC0_RX_UDF_INT_ENA_S  10
/** SDIO_SLC0_TX_OVF_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  The interrupt enable bit for SLC0_TX_OVF_INT interrupt.
 */
#define SDIO_SLC0_TX_OVF_INT_ENA    (BIT(11))
#define SDIO_SLC0_TX_OVF_INT_ENA_M  (SDIO_SLC0_TX_OVF_INT_ENA_V << SDIO_SLC0_TX_OVF_INT_ENA_S)
#define SDIO_SLC0_TX_OVF_INT_ENA_V  0x00000001U
#define SDIO_SLC0_TX_OVF_INT_ENA_S  11
/** SDIO_SLC0_TOKEN0_1TO0_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  The interrupt enable bit for SLC0_TOKEN0_1TO0_INT interrupt.
 */
#define SDIO_SLC0_TOKEN0_1TO0_INT_ENA    (BIT(12))
#define SDIO_SLC0_TOKEN0_1TO0_INT_ENA_M  (SDIO_SLC0_TOKEN0_1TO0_INT_ENA_V << SDIO_SLC0_TOKEN0_1TO0_INT_ENA_S)
#define SDIO_SLC0_TOKEN0_1TO0_INT_ENA_V  0x00000001U
#define SDIO_SLC0_TOKEN0_1TO0_INT_ENA_S  12
/** SDIO_SLC0_TOKEN1_1TO0_INT_ENA : R/W; bitpos: [13]; default: 0;
 *  The interrupt enable bit for SLC0_TOKEN1_1TO0_INT interrupt.
 */
#define SDIO_SLC0_TOKEN1_1TO0_INT_ENA    (BIT(13))
#define SDIO_SLC0_TOKEN1_1TO0_INT_ENA_M  (SDIO_SLC0_TOKEN1_1TO0_INT_ENA_V << SDIO_SLC0_TOKEN1_1TO0_INT_ENA_S)
#define SDIO_SLC0_TOKEN1_1TO0_INT_ENA_V  0x00000001U
#define SDIO_SLC0_TOKEN1_1TO0_INT_ENA_S  13
/** SDIO_SLC0_TX_DONE_INT_ENA : R/W; bitpos: [14]; default: 0;
 *  The interrupt enable bit for SLC0_TX_DONE_INT_ST interrupt.
 */
#define SDIO_SLC0_TX_DONE_INT_ENA    (BIT(14))
#define SDIO_SLC0_TX_DONE_INT_ENA_M  (SDIO_SLC0_TX_DONE_INT_ENA_V << SDIO_SLC0_TX_DONE_INT_ENA_S)
#define SDIO_SLC0_TX_DONE_INT_ENA_V  0x00000001U
#define SDIO_SLC0_TX_DONE_INT_ENA_S  14
/** SDIO_SLC0_TX_SUC_EOF_INT_ENA : R/W; bitpos: [15]; default: 0;
 *  The interrupt enable bit for SLC0_TX_SUC_EOF_INT interrupt.
 */
#define SDIO_SLC0_TX_SUC_EOF_INT_ENA    (BIT(15))
#define SDIO_SLC0_TX_SUC_EOF_INT_ENA_M  (SDIO_SLC0_TX_SUC_EOF_INT_ENA_V << SDIO_SLC0_TX_SUC_EOF_INT_ENA_S)
#define SDIO_SLC0_TX_SUC_EOF_INT_ENA_V  0x00000001U
#define SDIO_SLC0_TX_SUC_EOF_INT_ENA_S  15
/** SDIO_SLC0_RX_DONE_INT_ENA : R/W; bitpos: [16]; default: 0;
 *  The interrupt enable bit for SLC0_RX_DONE_INT interrupt.
 */
#define SDIO_SLC0_RX_DONE_INT_ENA    (BIT(16))
#define SDIO_SLC0_RX_DONE_INT_ENA_M  (SDIO_SLC0_RX_DONE_INT_ENA_V << SDIO_SLC0_RX_DONE_INT_ENA_S)
#define SDIO_SLC0_RX_DONE_INT_ENA_V  0x00000001U
#define SDIO_SLC0_RX_DONE_INT_ENA_S  16
/** SDIO_SLC0_RX_EOF_INT_ENA : R/W; bitpos: [17]; default: 0;
 *  The interrupt enable bit for SLC0_RX_EOF_INT interrupt.
 */
#define SDIO_SLC0_RX_EOF_INT_ENA    (BIT(17))
#define SDIO_SLC0_RX_EOF_INT_ENA_M  (SDIO_SLC0_RX_EOF_INT_ENA_V << SDIO_SLC0_RX_EOF_INT_ENA_S)
#define SDIO_SLC0_RX_EOF_INT_ENA_V  0x00000001U
#define SDIO_SLC0_RX_EOF_INT_ENA_S  17
/** SDIO_SLC0_TOHOST_INT_ENA : R/W; bitpos: [18]; default: 0;
 *  The interrupt enable bit for SLC0_TOHOST_INT interrupt.
 */
#define SDIO_SLC0_TOHOST_INT_ENA    (BIT(18))
#define SDIO_SLC0_TOHOST_INT_ENA_M  (SDIO_SLC0_TOHOST_INT_ENA_V << SDIO_SLC0_TOHOST_INT_ENA_S)
#define SDIO_SLC0_TOHOST_INT_ENA_V  0x00000001U
#define SDIO_SLC0_TOHOST_INT_ENA_S  18
/** SDIO_SLC0_TX_DSCR_ERR_INT_ENA : R/W; bitpos: [19]; default: 0;
 *  The interrupt enable bit for SLC0_TX_DSCR_ERR_INT interrupt.
 */
#define SDIO_SLC0_TX_DSCR_ERR_INT_ENA    (BIT(19))
#define SDIO_SLC0_TX_DSCR_ERR_INT_ENA_M  (SDIO_SLC0_TX_DSCR_ERR_INT_ENA_V << SDIO_SLC0_TX_DSCR_ERR_INT_ENA_S)
#define SDIO_SLC0_TX_DSCR_ERR_INT_ENA_V  0x00000001U
#define SDIO_SLC0_TX_DSCR_ERR_INT_ENA_S  19
/** SDIO_SLC0_RX_DSCR_ERR_INT_ENA : R/W; bitpos: [20]; default: 0;
 *  The interrupt enable bit for SLC0_RX_DSCR_ERR_INT interrupt.
 */
#define SDIO_SLC0_RX_DSCR_ERR_INT_ENA    (BIT(20))
#define SDIO_SLC0_RX_DSCR_ERR_INT_ENA_M  (SDIO_SLC0_RX_DSCR_ERR_INT_ENA_V << SDIO_SLC0_RX_DSCR_ERR_INT_ENA_S)
#define SDIO_SLC0_RX_DSCR_ERR_INT_ENA_V  0x00000001U
#define SDIO_SLC0_RX_DSCR_ERR_INT_ENA_S  20
/** SDIO_SLC0_TX_DSCR_EMPTY_INT_ENA : R/W; bitpos: [21]; default: 0;
 *  The interrupt enable bit for SLC0_TX_DSCR_EMPTY_INT interrupt.
 */
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_ENA    (BIT(21))
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_ENA_M  (SDIO_SLC0_TX_DSCR_EMPTY_INT_ENA_V << SDIO_SLC0_TX_DSCR_EMPTY_INT_ENA_S)
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_ENA_V  0x00000001U
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_ENA_S  21
/** SDIO_SLC0_HOST_RD_ACK_INT_ENA : R/W; bitpos: [22]; default: 0;
 *  The interrupt enable bit for SLC0_HOST_RD_ACK_INT interrupt.
 */
#define SDIO_SLC0_HOST_RD_ACK_INT_ENA    (BIT(22))
#define SDIO_SLC0_HOST_RD_ACK_INT_ENA_M  (SDIO_SLC0_HOST_RD_ACK_INT_ENA_V << SDIO_SLC0_HOST_RD_ACK_INT_ENA_S)
#define SDIO_SLC0_HOST_RD_ACK_INT_ENA_V  0x00000001U
#define SDIO_SLC0_HOST_RD_ACK_INT_ENA_S  22
/** SDIO_SLC0_WR_RETRY_DONE_INT_ENA : R/W; bitpos: [23]; default: 0;
 *  The interrupt enable bit for SLC0_WR_RETRY_DONE_INT interrupt.
 */
#define SDIO_SLC0_WR_RETRY_DONE_INT_ENA    (BIT(23))
#define SDIO_SLC0_WR_RETRY_DONE_INT_ENA_M  (SDIO_SLC0_WR_RETRY_DONE_INT_ENA_V << SDIO_SLC0_WR_RETRY_DONE_INT_ENA_S)
#define SDIO_SLC0_WR_RETRY_DONE_INT_ENA_V  0x00000001U
#define SDIO_SLC0_WR_RETRY_DONE_INT_ENA_S  23
/** SDIO_SLC0_TX_ERR_EOF_INT_ENA : R/W; bitpos: [24]; default: 0;
 *  The interrupt enable bit for SLC0_TX_ERR_EOF_INT interrupt.
 */
#define SDIO_SLC0_TX_ERR_EOF_INT_ENA    (BIT(24))
#define SDIO_SLC0_TX_ERR_EOF_INT_ENA_M  (SDIO_SLC0_TX_ERR_EOF_INT_ENA_V << SDIO_SLC0_TX_ERR_EOF_INT_ENA_S)
#define SDIO_SLC0_TX_ERR_EOF_INT_ENA_V  0x00000001U
#define SDIO_SLC0_TX_ERR_EOF_INT_ENA_S  24
/** SDIO_CMD_DTC_INT_ENA : R/W; bitpos: [25]; default: 0;
 *  The interrupt enable bit for CMD_DTC_INT interrupt.
 */
#define SDIO_CMD_DTC_INT_ENA    (BIT(25))
#define SDIO_CMD_DTC_INT_ENA_M  (SDIO_CMD_DTC_INT_ENA_V << SDIO_CMD_DTC_INT_ENA_S)
#define SDIO_CMD_DTC_INT_ENA_V  0x00000001U
#define SDIO_CMD_DTC_INT_ENA_S  25
/** SDIO_SLC0_RX_QUICK_EOF_INT_ENA : R/W; bitpos: [26]; default: 0;
 *  The interrupt enable bit for SLC0_RX_QUICK_EOF_INT interrupt.
 */
#define SDIO_SLC0_RX_QUICK_EOF_INT_ENA    (BIT(26))
#define SDIO_SLC0_RX_QUICK_EOF_INT_ENA_M  (SDIO_SLC0_RX_QUICK_EOF_INT_ENA_V << SDIO_SLC0_RX_QUICK_EOF_INT_ENA_S)
#define SDIO_SLC0_RX_QUICK_EOF_INT_ENA_V  0x00000001U
#define SDIO_SLC0_RX_QUICK_EOF_INT_ENA_S  26
/** SDIO_SLC0_HOST_POP_EOF_ERR_INT_ENA : R/W; bitpos: [27]; default: 0;
 *  The interrupt enable bit for SLC0_HOST_POP_EOF_ERR_INT interrupt.
 */
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_ENA    (BIT(27))
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_ENA_M  (SDIO_SLC0_HOST_POP_EOF_ERR_INT_ENA_V << SDIO_SLC0_HOST_POP_EOF_ERR_INT_ENA_S)
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_ENA_V  0x00000001U
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_ENA_S  27
/** SDIO_HDA_RECV_DONE_INT_ENA : R/W; bitpos: [28]; default: 0;
 *  The interrupt enable bit for HDA_RECV_DONE_INT interrupt.
 */
#define SDIO_HDA_RECV_DONE_INT_ENA    (BIT(28))
#define SDIO_HDA_RECV_DONE_INT_ENA_M  (SDIO_HDA_RECV_DONE_INT_ENA_V << SDIO_HDA_RECV_DONE_INT_ENA_S)
#define SDIO_HDA_RECV_DONE_INT_ENA_V  0x00000001U
#define SDIO_HDA_RECV_DONE_INT_ENA_S  28

/** SDIO_SLC_SLC0INT_CLR_REG register
 *  Sdio slave DMA channel0 interrupt clear register.
 */
#define SDIO_SLC_SLC0INT_CLR_REG (DR_REG_SLC_BASE + 0x10)
/** SDIO_SLC_FRHOST_BIT0_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear SLC_FRHOST_BIT0_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT0_INT_CLR    (BIT(0))
#define SDIO_SLC_FRHOST_BIT0_INT_CLR_M  (SDIO_SLC_FRHOST_BIT0_INT_CLR_V << SDIO_SLC_FRHOST_BIT0_INT_CLR_S)
#define SDIO_SLC_FRHOST_BIT0_INT_CLR_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT0_INT_CLR_S  0
/** SDIO_SLC_FRHOST_BIT1_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear SLC_FRHOST_BIT1_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT1_INT_CLR    (BIT(1))
#define SDIO_SLC_FRHOST_BIT1_INT_CLR_M  (SDIO_SLC_FRHOST_BIT1_INT_CLR_V << SDIO_SLC_FRHOST_BIT1_INT_CLR_S)
#define SDIO_SLC_FRHOST_BIT1_INT_CLR_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT1_INT_CLR_S  1
/** SDIO_SLC_FRHOST_BIT2_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear SLC_FRHOST_BIT2_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT2_INT_CLR    (BIT(2))
#define SDIO_SLC_FRHOST_BIT2_INT_CLR_M  (SDIO_SLC_FRHOST_BIT2_INT_CLR_V << SDIO_SLC_FRHOST_BIT2_INT_CLR_S)
#define SDIO_SLC_FRHOST_BIT2_INT_CLR_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT2_INT_CLR_S  2
/** SDIO_SLC_FRHOST_BIT3_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear SLC_FRHOST_BIT3_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT3_INT_CLR    (BIT(3))
#define SDIO_SLC_FRHOST_BIT3_INT_CLR_M  (SDIO_SLC_FRHOST_BIT3_INT_CLR_V << SDIO_SLC_FRHOST_BIT3_INT_CLR_S)
#define SDIO_SLC_FRHOST_BIT3_INT_CLR_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT3_INT_CLR_S  3
/** SDIO_SLC_FRHOST_BIT4_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear SLC_FRHOST_BIT4_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT4_INT_CLR    (BIT(4))
#define SDIO_SLC_FRHOST_BIT4_INT_CLR_M  (SDIO_SLC_FRHOST_BIT4_INT_CLR_V << SDIO_SLC_FRHOST_BIT4_INT_CLR_S)
#define SDIO_SLC_FRHOST_BIT4_INT_CLR_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT4_INT_CLR_S  4
/** SDIO_SLC_FRHOST_BIT5_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear SLC_FRHOST_BIT5_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT5_INT_CLR    (BIT(5))
#define SDIO_SLC_FRHOST_BIT5_INT_CLR_M  (SDIO_SLC_FRHOST_BIT5_INT_CLR_V << SDIO_SLC_FRHOST_BIT5_INT_CLR_S)
#define SDIO_SLC_FRHOST_BIT5_INT_CLR_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT5_INT_CLR_S  5
/** SDIO_SLC_FRHOST_BIT6_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear SLC_FRHOST_BIT6_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT6_INT_CLR    (BIT(6))
#define SDIO_SLC_FRHOST_BIT6_INT_CLR_M  (SDIO_SLC_FRHOST_BIT6_INT_CLR_V << SDIO_SLC_FRHOST_BIT6_INT_CLR_S)
#define SDIO_SLC_FRHOST_BIT6_INT_CLR_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT6_INT_CLR_S  6
/** SDIO_SLC_FRHOST_BIT7_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear SLC_FRHOST_BIT7_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT7_INT_CLR    (BIT(7))
#define SDIO_SLC_FRHOST_BIT7_INT_CLR_M  (SDIO_SLC_FRHOST_BIT7_INT_CLR_V << SDIO_SLC_FRHOST_BIT7_INT_CLR_S)
#define SDIO_SLC_FRHOST_BIT7_INT_CLR_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT7_INT_CLR_S  7
/** SDIO_SLC0_RX_START_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear SLC0_RX_START_INT_ST interrupt.
 */
#define SDIO_SLC0_RX_START_INT_CLR    (BIT(8))
#define SDIO_SLC0_RX_START_INT_CLR_M  (SDIO_SLC0_RX_START_INT_CLR_V << SDIO_SLC0_RX_START_INT_CLR_S)
#define SDIO_SLC0_RX_START_INT_CLR_V  0x00000001U
#define SDIO_SLC0_RX_START_INT_CLR_S  8
/** SDIO_SLC0_TX_START_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear SLC0_TX_START_INT_ST interrupt.
 */
#define SDIO_SLC0_TX_START_INT_CLR    (BIT(9))
#define SDIO_SLC0_TX_START_INT_CLR_M  (SDIO_SLC0_TX_START_INT_CLR_V << SDIO_SLC0_TX_START_INT_CLR_S)
#define SDIO_SLC0_TX_START_INT_CLR_V  0x00000001U
#define SDIO_SLC0_TX_START_INT_CLR_S  9
/** SDIO_SLC0_RX_UDF_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Set this bit to clear SLC0_RX_UDF_INT interrupt.
 */
#define SDIO_SLC0_RX_UDF_INT_CLR    (BIT(10))
#define SDIO_SLC0_RX_UDF_INT_CLR_M  (SDIO_SLC0_RX_UDF_INT_CLR_V << SDIO_SLC0_RX_UDF_INT_CLR_S)
#define SDIO_SLC0_RX_UDF_INT_CLR_V  0x00000001U
#define SDIO_SLC0_RX_UDF_INT_CLR_S  10
/** SDIO_SLC0_TX_OVF_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Set this bit to clear SLC0_TX_OVF_INT interrupt.
 */
#define SDIO_SLC0_TX_OVF_INT_CLR    (BIT(11))
#define SDIO_SLC0_TX_OVF_INT_CLR_M  (SDIO_SLC0_TX_OVF_INT_CLR_V << SDIO_SLC0_TX_OVF_INT_CLR_S)
#define SDIO_SLC0_TX_OVF_INT_CLR_V  0x00000001U
#define SDIO_SLC0_TX_OVF_INT_CLR_S  11
/** SDIO_SLC0_TOKEN0_1TO0_INT_CLR : WT; bitpos: [12]; default: 0;
 *  Set this bit to clear SLC0_TOKEN0_1TO0_INT interrupt.
 */
#define SDIO_SLC0_TOKEN0_1TO0_INT_CLR    (BIT(12))
#define SDIO_SLC0_TOKEN0_1TO0_INT_CLR_M  (SDIO_SLC0_TOKEN0_1TO0_INT_CLR_V << SDIO_SLC0_TOKEN0_1TO0_INT_CLR_S)
#define SDIO_SLC0_TOKEN0_1TO0_INT_CLR_V  0x00000001U
#define SDIO_SLC0_TOKEN0_1TO0_INT_CLR_S  12
/** SDIO_SLC0_TOKEN1_1TO0_INT_CLR : WT; bitpos: [13]; default: 0;
 *  Set this bit to clear SLC0_TOKEN1_1TO0_INT interrupt.
 */
#define SDIO_SLC0_TOKEN1_1TO0_INT_CLR    (BIT(13))
#define SDIO_SLC0_TOKEN1_1TO0_INT_CLR_M  (SDIO_SLC0_TOKEN1_1TO0_INT_CLR_V << SDIO_SLC0_TOKEN1_1TO0_INT_CLR_S)
#define SDIO_SLC0_TOKEN1_1TO0_INT_CLR_V  0x00000001U
#define SDIO_SLC0_TOKEN1_1TO0_INT_CLR_S  13
/** SDIO_SLC0_TX_DONE_INT_CLR : WT; bitpos: [14]; default: 0;
 *  Set this bit to clear SLC0_TX_DONE_INT_ST interrupt.
 */
#define SDIO_SLC0_TX_DONE_INT_CLR    (BIT(14))
#define SDIO_SLC0_TX_DONE_INT_CLR_M  (SDIO_SLC0_TX_DONE_INT_CLR_V << SDIO_SLC0_TX_DONE_INT_CLR_S)
#define SDIO_SLC0_TX_DONE_INT_CLR_V  0x00000001U
#define SDIO_SLC0_TX_DONE_INT_CLR_S  14
/** SDIO_SLC0_TX_SUC_EOF_INT_CLR : WT; bitpos: [15]; default: 0;
 *  Set this bit to clear SLC0_TX_SUC_EOF_INT interrupt.
 */
#define SDIO_SLC0_TX_SUC_EOF_INT_CLR    (BIT(15))
#define SDIO_SLC0_TX_SUC_EOF_INT_CLR_M  (SDIO_SLC0_TX_SUC_EOF_INT_CLR_V << SDIO_SLC0_TX_SUC_EOF_INT_CLR_S)
#define SDIO_SLC0_TX_SUC_EOF_INT_CLR_V  0x00000001U
#define SDIO_SLC0_TX_SUC_EOF_INT_CLR_S  15
/** SDIO_SLC0_RX_DONE_INT_CLR : WT; bitpos: [16]; default: 0;
 *  Set this bit to clear SLC0_RX_DONE_INT interrupt.
 */
#define SDIO_SLC0_RX_DONE_INT_CLR    (BIT(16))
#define SDIO_SLC0_RX_DONE_INT_CLR_M  (SDIO_SLC0_RX_DONE_INT_CLR_V << SDIO_SLC0_RX_DONE_INT_CLR_S)
#define SDIO_SLC0_RX_DONE_INT_CLR_V  0x00000001U
#define SDIO_SLC0_RX_DONE_INT_CLR_S  16
/** SDIO_SLC0_RX_EOF_INT_CLR : WT; bitpos: [17]; default: 0;
 *  Set this bit to clear SLC0_RX_EOF_INT interrupt.
 */
#define SDIO_SLC0_RX_EOF_INT_CLR    (BIT(17))
#define SDIO_SLC0_RX_EOF_INT_CLR_M  (SDIO_SLC0_RX_EOF_INT_CLR_V << SDIO_SLC0_RX_EOF_INT_CLR_S)
#define SDIO_SLC0_RX_EOF_INT_CLR_V  0x00000001U
#define SDIO_SLC0_RX_EOF_INT_CLR_S  17
/** SDIO_SLC0_TOHOST_INT_CLR : WT; bitpos: [18]; default: 0;
 *  Set this bit to clear SLC0_TOHOST_INT interrupt.
 */
#define SDIO_SLC0_TOHOST_INT_CLR    (BIT(18))
#define SDIO_SLC0_TOHOST_INT_CLR_M  (SDIO_SLC0_TOHOST_INT_CLR_V << SDIO_SLC0_TOHOST_INT_CLR_S)
#define SDIO_SLC0_TOHOST_INT_CLR_V  0x00000001U
#define SDIO_SLC0_TOHOST_INT_CLR_S  18
/** SDIO_SLC0_TX_DSCR_ERR_INT_CLR : WT; bitpos: [19]; default: 0;
 *  Set this bit to clear SLC0_TX_DSCR_ERR_INT interrupt.
 */
#define SDIO_SLC0_TX_DSCR_ERR_INT_CLR    (BIT(19))
#define SDIO_SLC0_TX_DSCR_ERR_INT_CLR_M  (SDIO_SLC0_TX_DSCR_ERR_INT_CLR_V << SDIO_SLC0_TX_DSCR_ERR_INT_CLR_S)
#define SDIO_SLC0_TX_DSCR_ERR_INT_CLR_V  0x00000001U
#define SDIO_SLC0_TX_DSCR_ERR_INT_CLR_S  19
/** SDIO_SLC0_RX_DSCR_ERR_INT_CLR : WT; bitpos: [20]; default: 0;
 *  Set this bit to clear SLC0_RX_DSCR_ERR_INT interrupt.
 */
#define SDIO_SLC0_RX_DSCR_ERR_INT_CLR    (BIT(20))
#define SDIO_SLC0_RX_DSCR_ERR_INT_CLR_M  (SDIO_SLC0_RX_DSCR_ERR_INT_CLR_V << SDIO_SLC0_RX_DSCR_ERR_INT_CLR_S)
#define SDIO_SLC0_RX_DSCR_ERR_INT_CLR_V  0x00000001U
#define SDIO_SLC0_RX_DSCR_ERR_INT_CLR_S  20
/** SDIO_SLC0_TX_DSCR_EMPTY_INT_CLR : WT; bitpos: [21]; default: 0;
 *  Set this bit to clear SLC0_TX_DSCR_EMPTY_INT interrupt.
 */
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_CLR    (BIT(21))
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_CLR_M  (SDIO_SLC0_TX_DSCR_EMPTY_INT_CLR_V << SDIO_SLC0_TX_DSCR_EMPTY_INT_CLR_S)
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_CLR_V  0x00000001U
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_CLR_S  21
/** SDIO_SLC0_HOST_RD_ACK_INT_CLR : WT; bitpos: [22]; default: 0;
 *  Set this bit to clear SLC0_HOST_RD_ACK_INT interrupt.
 */
#define SDIO_SLC0_HOST_RD_ACK_INT_CLR    (BIT(22))
#define SDIO_SLC0_HOST_RD_ACK_INT_CLR_M  (SDIO_SLC0_HOST_RD_ACK_INT_CLR_V << SDIO_SLC0_HOST_RD_ACK_INT_CLR_S)
#define SDIO_SLC0_HOST_RD_ACK_INT_CLR_V  0x00000001U
#define SDIO_SLC0_HOST_RD_ACK_INT_CLR_S  22
/** SDIO_SLC0_WR_RETRY_DONE_INT_CLR : WT; bitpos: [23]; default: 0;
 *  Set this bit to clear SLC0_WR_RETRY_DONE_INT interrupt.
 */
#define SDIO_SLC0_WR_RETRY_DONE_INT_CLR    (BIT(23))
#define SDIO_SLC0_WR_RETRY_DONE_INT_CLR_M  (SDIO_SLC0_WR_RETRY_DONE_INT_CLR_V << SDIO_SLC0_WR_RETRY_DONE_INT_CLR_S)
#define SDIO_SLC0_WR_RETRY_DONE_INT_CLR_V  0x00000001U
#define SDIO_SLC0_WR_RETRY_DONE_INT_CLR_S  23
/** SDIO_SLC0_TX_ERR_EOF_INT_CLR : WT; bitpos: [24]; default: 0;
 *  Set this bit to clear SLC0_TX_ERR_EOF_INT interrupt.
 */
#define SDIO_SLC0_TX_ERR_EOF_INT_CLR    (BIT(24))
#define SDIO_SLC0_TX_ERR_EOF_INT_CLR_M  (SDIO_SLC0_TX_ERR_EOF_INT_CLR_V << SDIO_SLC0_TX_ERR_EOF_INT_CLR_S)
#define SDIO_SLC0_TX_ERR_EOF_INT_CLR_V  0x00000001U
#define SDIO_SLC0_TX_ERR_EOF_INT_CLR_S  24
/** SDIO_CMD_DTC_INT_CLR : WT; bitpos: [25]; default: 0;
 *  Set this bit to clear CMD_DTC_INT interrupt.
 */
#define SDIO_CMD_DTC_INT_CLR    (BIT(25))
#define SDIO_CMD_DTC_INT_CLR_M  (SDIO_CMD_DTC_INT_CLR_V << SDIO_CMD_DTC_INT_CLR_S)
#define SDIO_CMD_DTC_INT_CLR_V  0x00000001U
#define SDIO_CMD_DTC_INT_CLR_S  25
/** SDIO_SLC0_RX_QUICK_EOF_INT_CLR : WT; bitpos: [26]; default: 0;
 *  Set this bit to clear SLC0_RX_QUICK_EOF_INT interrupt.
 */
#define SDIO_SLC0_RX_QUICK_EOF_INT_CLR    (BIT(26))
#define SDIO_SLC0_RX_QUICK_EOF_INT_CLR_M  (SDIO_SLC0_RX_QUICK_EOF_INT_CLR_V << SDIO_SLC0_RX_QUICK_EOF_INT_CLR_S)
#define SDIO_SLC0_RX_QUICK_EOF_INT_CLR_V  0x00000001U
#define SDIO_SLC0_RX_QUICK_EOF_INT_CLR_S  26
/** SDIO_SLC0_HOST_POP_EOF_ERR_INT_CLR : WT; bitpos: [27]; default: 0;
 *  Set this bit to clear SLC0_HOST_POP_EOF_ERR_INT interrupt.
 */
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_CLR    (BIT(27))
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_CLR_M  (SDIO_SLC0_HOST_POP_EOF_ERR_INT_CLR_V << SDIO_SLC0_HOST_POP_EOF_ERR_INT_CLR_S)
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_CLR_V  0x00000001U
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_CLR_S  27
/** SDIO_HDA_RECV_DONE_INT_CLR : WT; bitpos: [28]; default: 0;
 *  Set this bit to clear HDA_RECV_DONE_INT interrupt.
 */
#define SDIO_HDA_RECV_DONE_INT_CLR    (BIT(28))
#define SDIO_HDA_RECV_DONE_INT_CLR_M  (SDIO_HDA_RECV_DONE_INT_CLR_V << SDIO_HDA_RECV_DONE_INT_CLR_S)
#define SDIO_HDA_RECV_DONE_INT_CLR_V  0x00000001U
#define SDIO_HDA_RECV_DONE_INT_CLR_S  28

/** SDIO_SLC_SLC1INT_RAW_REG register
 *  Sdio slave DMA channel1 raw interrupt status register.
 */
#define SDIO_SLC_SLC1INT_RAW_REG (DR_REG_SLC_BASE + 0x14)
/** SDIO_SLC_FRHOST_BIT8_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit 8 for host to interrupt slave.
 */
#define SDIO_SLC_FRHOST_BIT8_INT_RAW    (BIT(0))
#define SDIO_SLC_FRHOST_BIT8_INT_RAW_M  (SDIO_SLC_FRHOST_BIT8_INT_RAW_V << SDIO_SLC_FRHOST_BIT8_INT_RAW_S)
#define SDIO_SLC_FRHOST_BIT8_INT_RAW_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT8_INT_RAW_S  0
/** SDIO_SLC_FRHOST_BIT9_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit 9 for host to interrupt slave.
 */
#define SDIO_SLC_FRHOST_BIT9_INT_RAW    (BIT(1))
#define SDIO_SLC_FRHOST_BIT9_INT_RAW_M  (SDIO_SLC_FRHOST_BIT9_INT_RAW_V << SDIO_SLC_FRHOST_BIT9_INT_RAW_S)
#define SDIO_SLC_FRHOST_BIT9_INT_RAW_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT9_INT_RAW_S  1
/** SDIO_SLC_FRHOST_BIT10_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit 10 for host to interrupt slave.
 */
#define SDIO_SLC_FRHOST_BIT10_INT_RAW    (BIT(2))
#define SDIO_SLC_FRHOST_BIT10_INT_RAW_M  (SDIO_SLC_FRHOST_BIT10_INT_RAW_V << SDIO_SLC_FRHOST_BIT10_INT_RAW_S)
#define SDIO_SLC_FRHOST_BIT10_INT_RAW_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT10_INT_RAW_S  2
/** SDIO_SLC_FRHOST_BIT11_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit 11 for host to interrupt slave.
 */
#define SDIO_SLC_FRHOST_BIT11_INT_RAW    (BIT(3))
#define SDIO_SLC_FRHOST_BIT11_INT_RAW_M  (SDIO_SLC_FRHOST_BIT11_INT_RAW_V << SDIO_SLC_FRHOST_BIT11_INT_RAW_S)
#define SDIO_SLC_FRHOST_BIT11_INT_RAW_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT11_INT_RAW_S  3
/** SDIO_SLC_FRHOST_BIT12_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit 12 for host to interrupt slave.
 */
#define SDIO_SLC_FRHOST_BIT12_INT_RAW    (BIT(4))
#define SDIO_SLC_FRHOST_BIT12_INT_RAW_M  (SDIO_SLC_FRHOST_BIT12_INT_RAW_V << SDIO_SLC_FRHOST_BIT12_INT_RAW_S)
#define SDIO_SLC_FRHOST_BIT12_INT_RAW_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT12_INT_RAW_S  4
/** SDIO_SLC_FRHOST_BIT13_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt bit 13 for host to interrupt slave.
 */
#define SDIO_SLC_FRHOST_BIT13_INT_RAW    (BIT(5))
#define SDIO_SLC_FRHOST_BIT13_INT_RAW_M  (SDIO_SLC_FRHOST_BIT13_INT_RAW_V << SDIO_SLC_FRHOST_BIT13_INT_RAW_S)
#define SDIO_SLC_FRHOST_BIT13_INT_RAW_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT13_INT_RAW_S  5
/** SDIO_SLC_FRHOST_BIT14_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt bit 14 for host to interrupt slave.
 */
#define SDIO_SLC_FRHOST_BIT14_INT_RAW    (BIT(6))
#define SDIO_SLC_FRHOST_BIT14_INT_RAW_M  (SDIO_SLC_FRHOST_BIT14_INT_RAW_V << SDIO_SLC_FRHOST_BIT14_INT_RAW_S)
#define SDIO_SLC_FRHOST_BIT14_INT_RAW_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT14_INT_RAW_S  6
/** SDIO_SLC_FRHOST_BIT15_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt bit 15 for host to interrupt slave.
 */
#define SDIO_SLC_FRHOST_BIT15_INT_RAW    (BIT(7))
#define SDIO_SLC_FRHOST_BIT15_INT_RAW_M  (SDIO_SLC_FRHOST_BIT15_INT_RAW_V << SDIO_SLC_FRHOST_BIT15_INT_RAW_S)
#define SDIO_SLC_FRHOST_BIT15_INT_RAW_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT15_INT_RAW_S  7
/** SDIO_SLC1_RX_START_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit of registering dma channel1 sending initialization
 */
#define SDIO_SLC1_RX_START_INT_RAW    (BIT(8))
#define SDIO_SLC1_RX_START_INT_RAW_M  (SDIO_SLC1_RX_START_INT_RAW_V << SDIO_SLC1_RX_START_INT_RAW_S)
#define SDIO_SLC1_RX_START_INT_RAW_V  0x00000001U
#define SDIO_SLC1_RX_START_INT_RAW_S  8
/** SDIO_SLC1_TX_START_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  The raw interrupt bit of registering dma channel1 receiving initialization
 */
#define SDIO_SLC1_TX_START_INT_RAW    (BIT(9))
#define SDIO_SLC1_TX_START_INT_RAW_M  (SDIO_SLC1_TX_START_INT_RAW_V << SDIO_SLC1_TX_START_INT_RAW_S)
#define SDIO_SLC1_TX_START_INT_RAW_V  0x00000001U
#define SDIO_SLC1_TX_START_INT_RAW_S  9
/** SDIO_SLC1_RX_UDF_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  The raw interrupt bit of dma channel1 sending buffer underflow.
 */
#define SDIO_SLC1_RX_UDF_INT_RAW    (BIT(10))
#define SDIO_SLC1_RX_UDF_INT_RAW_M  (SDIO_SLC1_RX_UDF_INT_RAW_V << SDIO_SLC1_RX_UDF_INT_RAW_S)
#define SDIO_SLC1_RX_UDF_INT_RAW_V  0x00000001U
#define SDIO_SLC1_RX_UDF_INT_RAW_S  10
/** SDIO_SLC1_TX_OVF_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  The raw interrupt bit of dma channel1 receiving buffer overflow.
 */
#define SDIO_SLC1_TX_OVF_INT_RAW    (BIT(11))
#define SDIO_SLC1_TX_OVF_INT_RAW_M  (SDIO_SLC1_TX_OVF_INT_RAW_V << SDIO_SLC1_TX_OVF_INT_RAW_S)
#define SDIO_SLC1_TX_OVF_INT_RAW_V  0x00000001U
#define SDIO_SLC1_TX_OVF_INT_RAW_S  11
/** SDIO_SLC1_TOKEN0_1TO0_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  The raw interrupt bit for the value of slc0_token0 becoming to zero.
 */
#define SDIO_SLC1_TOKEN0_1TO0_INT_RAW    (BIT(12))
#define SDIO_SLC1_TOKEN0_1TO0_INT_RAW_M  (SDIO_SLC1_TOKEN0_1TO0_INT_RAW_V << SDIO_SLC1_TOKEN0_1TO0_INT_RAW_S)
#define SDIO_SLC1_TOKEN0_1TO0_INT_RAW_V  0x00000001U
#define SDIO_SLC1_TOKEN0_1TO0_INT_RAW_S  12
/** SDIO_SLC1_TOKEN1_1TO0_INT_RAW : R/WTC/SS; bitpos: [13]; default: 0;
 *  The raw interrupt bit for the value of slc0_token1 becoming to zero.
 */
#define SDIO_SLC1_TOKEN1_1TO0_INT_RAW    (BIT(13))
#define SDIO_SLC1_TOKEN1_1TO0_INT_RAW_M  (SDIO_SLC1_TOKEN1_1TO0_INT_RAW_V << SDIO_SLC1_TOKEN1_1TO0_INT_RAW_S)
#define SDIO_SLC1_TOKEN1_1TO0_INT_RAW_V  0x00000001U
#define SDIO_SLC1_TOKEN1_1TO0_INT_RAW_S  13
/** SDIO_SLC1_TX_DONE_INT_RAW : R/WTC/SS; bitpos: [14]; default: 0;
 *  The raw interrupt bit of dma channel1 finishing receiving data to one single buffer.
 */
#define SDIO_SLC1_TX_DONE_INT_RAW    (BIT(14))
#define SDIO_SLC1_TX_DONE_INT_RAW_M  (SDIO_SLC1_TX_DONE_INT_RAW_V << SDIO_SLC1_TX_DONE_INT_RAW_S)
#define SDIO_SLC1_TX_DONE_INT_RAW_V  0x00000001U
#define SDIO_SLC1_TX_DONE_INT_RAW_S  14
/** SDIO_SLC1_TX_SUC_EOF_INT_RAW : R/WTC/SS; bitpos: [15]; default: 0;
 *  The raw interrupt bit of dma channel1 finishing receiving data
 */
#define SDIO_SLC1_TX_SUC_EOF_INT_RAW    (BIT(15))
#define SDIO_SLC1_TX_SUC_EOF_INT_RAW_M  (SDIO_SLC1_TX_SUC_EOF_INT_RAW_V << SDIO_SLC1_TX_SUC_EOF_INT_RAW_S)
#define SDIO_SLC1_TX_SUC_EOF_INT_RAW_V  0x00000001U
#define SDIO_SLC1_TX_SUC_EOF_INT_RAW_S  15
/** SDIO_SLC1_RX_DONE_INT_RAW : R/WTC/SS; bitpos: [16]; default: 0;
 *  The raw interrupt bit of dma channel1 finishing sending data from one single buffer
 */
#define SDIO_SLC1_RX_DONE_INT_RAW    (BIT(16))
#define SDIO_SLC1_RX_DONE_INT_RAW_M  (SDIO_SLC1_RX_DONE_INT_RAW_V << SDIO_SLC1_RX_DONE_INT_RAW_S)
#define SDIO_SLC1_RX_DONE_INT_RAW_V  0x00000001U
#define SDIO_SLC1_RX_DONE_INT_RAW_S  16
/** SDIO_SLC1_RX_EOF_INT_RAW : R/WTC/SS; bitpos: [17]; default: 0;
 *  The raw interrupt bit of dma channel1 finishing sending data
 */
#define SDIO_SLC1_RX_EOF_INT_RAW    (BIT(17))
#define SDIO_SLC1_RX_EOF_INT_RAW_M  (SDIO_SLC1_RX_EOF_INT_RAW_V << SDIO_SLC1_RX_EOF_INT_RAW_S)
#define SDIO_SLC1_RX_EOF_INT_RAW_V  0x00000001U
#define SDIO_SLC1_RX_EOF_INT_RAW_S  17
/** SDIO_SLC1_TOHOST_INT_RAW : R/WTC/SS; bitpos: [18]; default: 0;
 *  The raw interrupt bit for slave generating interrupt to host.
 */
#define SDIO_SLC1_TOHOST_INT_RAW    (BIT(18))
#define SDIO_SLC1_TOHOST_INT_RAW_M  (SDIO_SLC1_TOHOST_INT_RAW_V << SDIO_SLC1_TOHOST_INT_RAW_S)
#define SDIO_SLC1_TOHOST_INT_RAW_V  0x00000001U
#define SDIO_SLC1_TOHOST_INT_RAW_S  18
/** SDIO_SLC1_TX_DSCR_ERR_INT_RAW : R/WTC/SS; bitpos: [19]; default: 0;
 *  The raw interrupt bit of dma channel1 receiving descriptor error.
 */
#define SDIO_SLC1_TX_DSCR_ERR_INT_RAW    (BIT(19))
#define SDIO_SLC1_TX_DSCR_ERR_INT_RAW_M  (SDIO_SLC1_TX_DSCR_ERR_INT_RAW_V << SDIO_SLC1_TX_DSCR_ERR_INT_RAW_S)
#define SDIO_SLC1_TX_DSCR_ERR_INT_RAW_V  0x00000001U
#define SDIO_SLC1_TX_DSCR_ERR_INT_RAW_S  19
/** SDIO_SLC1_RX_DSCR_ERR_INT_RAW : R/WTC/SS; bitpos: [20]; default: 0;
 *  The raw interrupt bit of dma channel1 sending descriptor error.
 */
#define SDIO_SLC1_RX_DSCR_ERR_INT_RAW    (BIT(20))
#define SDIO_SLC1_RX_DSCR_ERR_INT_RAW_M  (SDIO_SLC1_RX_DSCR_ERR_INT_RAW_V << SDIO_SLC1_RX_DSCR_ERR_INT_RAW_S)
#define SDIO_SLC1_RX_DSCR_ERR_INT_RAW_V  0x00000001U
#define SDIO_SLC1_RX_DSCR_ERR_INT_RAW_S  20
/** SDIO_SLC1_TX_DSCR_EMPTY_INT_RAW : R/WTC/SS; bitpos: [21]; default: 0;
 *  The raw interrupt bit of  not enough buffer for slave receiving data from host
 *  through dma channel1.
 */
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_RAW    (BIT(21))
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_RAW_M  (SDIO_SLC1_TX_DSCR_EMPTY_INT_RAW_V << SDIO_SLC1_TX_DSCR_EMPTY_INT_RAW_S)
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_RAW_V  0x00000001U
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_RAW_S  21
/** SDIO_SLC1_HOST_RD_ACK_INT_RAW : R/WTC/SS; bitpos: [22]; default: 0;
 *  The raw interrupt bit for host to acknowledge retrying receiving data from slave
 *  successfully through dma channel1.
 */
#define SDIO_SLC1_HOST_RD_ACK_INT_RAW    (BIT(22))
#define SDIO_SLC1_HOST_RD_ACK_INT_RAW_M  (SDIO_SLC1_HOST_RD_ACK_INT_RAW_V << SDIO_SLC1_HOST_RD_ACK_INT_RAW_S)
#define SDIO_SLC1_HOST_RD_ACK_INT_RAW_V  0x00000001U
#define SDIO_SLC1_HOST_RD_ACK_INT_RAW_S  22
/** SDIO_SLC1_WR_RETRY_DONE_INT_RAW : R/WTC/SS; bitpos: [23]; default: 0;
 *  The raw interrupt bit of host finishing retrying sending data to slave through dma
 *  channel1.
 */
#define SDIO_SLC1_WR_RETRY_DONE_INT_RAW    (BIT(23))
#define SDIO_SLC1_WR_RETRY_DONE_INT_RAW_M  (SDIO_SLC1_WR_RETRY_DONE_INT_RAW_V << SDIO_SLC1_WR_RETRY_DONE_INT_RAW_S)
#define SDIO_SLC1_WR_RETRY_DONE_INT_RAW_V  0x00000001U
#define SDIO_SLC1_WR_RETRY_DONE_INT_RAW_S  23
/** SDIO_SLC1_TX_ERR_EOF_INT_RAW : R/WTC/SS; bitpos: [24]; default: 0;
 *  The raw interrupt bit of data error during host sending data to slave through dma
 *  channel1.
 */
#define SDIO_SLC1_TX_ERR_EOF_INT_RAW    (BIT(24))
#define SDIO_SLC1_TX_ERR_EOF_INT_RAW_M  (SDIO_SLC1_TX_ERR_EOF_INT_RAW_V << SDIO_SLC1_TX_ERR_EOF_INT_RAW_S)
#define SDIO_SLC1_TX_ERR_EOF_INT_RAW_V  0x00000001U
#define SDIO_SLC1_TX_ERR_EOF_INT_RAW_S  24

/** SDIO_SLC_SLC1INT_ST_REG register
 *  Sdio slave DMA channel1 masked interrupt status register.
 */
#define SDIO_SLC_SLC1INT_ST_REG (DR_REG_SLC_BASE + 0x18)
/** SDIO_SLC_FRHOST_BIT8_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status bit for SLC_FRHOST_BIT0_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT8_INT_ST    (BIT(0))
#define SDIO_SLC_FRHOST_BIT8_INT_ST_M  (SDIO_SLC_FRHOST_BIT8_INT_ST_V << SDIO_SLC_FRHOST_BIT8_INT_ST_S)
#define SDIO_SLC_FRHOST_BIT8_INT_ST_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT8_INT_ST_S  0
/** SDIO_SLC_FRHOST_BIT9_INT_ST : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status bit for SLC_FRHOST_BIT1_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT9_INT_ST    (BIT(1))
#define SDIO_SLC_FRHOST_BIT9_INT_ST_M  (SDIO_SLC_FRHOST_BIT9_INT_ST_V << SDIO_SLC_FRHOST_BIT9_INT_ST_S)
#define SDIO_SLC_FRHOST_BIT9_INT_ST_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT9_INT_ST_S  1
/** SDIO_SLC_FRHOST_BIT10_INT_ST : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status bit for SLC_FRHOST_BIT2_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT10_INT_ST    (BIT(2))
#define SDIO_SLC_FRHOST_BIT10_INT_ST_M  (SDIO_SLC_FRHOST_BIT10_INT_ST_V << SDIO_SLC_FRHOST_BIT10_INT_ST_S)
#define SDIO_SLC_FRHOST_BIT10_INT_ST_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT10_INT_ST_S  2
/** SDIO_SLC_FRHOST_BIT11_INT_ST : RO; bitpos: [3]; default: 0;
 *  The masked interrupt status bit for SLC_FRHOST_BIT3_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT11_INT_ST    (BIT(3))
#define SDIO_SLC_FRHOST_BIT11_INT_ST_M  (SDIO_SLC_FRHOST_BIT11_INT_ST_V << SDIO_SLC_FRHOST_BIT11_INT_ST_S)
#define SDIO_SLC_FRHOST_BIT11_INT_ST_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT11_INT_ST_S  3
/** SDIO_SLC_FRHOST_BIT12_INT_ST : RO; bitpos: [4]; default: 0;
 *  The masked interrupt status bit for SLC_FRHOST_BIT4_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT12_INT_ST    (BIT(4))
#define SDIO_SLC_FRHOST_BIT12_INT_ST_M  (SDIO_SLC_FRHOST_BIT12_INT_ST_V << SDIO_SLC_FRHOST_BIT12_INT_ST_S)
#define SDIO_SLC_FRHOST_BIT12_INT_ST_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT12_INT_ST_S  4
/** SDIO_SLC_FRHOST_BIT13_INT_ST : RO; bitpos: [5]; default: 0;
 *  The masked interrupt status bit for SLC_FRHOST_BIT5_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT13_INT_ST    (BIT(5))
#define SDIO_SLC_FRHOST_BIT13_INT_ST_M  (SDIO_SLC_FRHOST_BIT13_INT_ST_V << SDIO_SLC_FRHOST_BIT13_INT_ST_S)
#define SDIO_SLC_FRHOST_BIT13_INT_ST_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT13_INT_ST_S  5
/** SDIO_SLC_FRHOST_BIT14_INT_ST : RO; bitpos: [6]; default: 0;
 *  The masked interrupt status bit for SLC_FRHOST_BIT6_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT14_INT_ST    (BIT(6))
#define SDIO_SLC_FRHOST_BIT14_INT_ST_M  (SDIO_SLC_FRHOST_BIT14_INT_ST_V << SDIO_SLC_FRHOST_BIT14_INT_ST_S)
#define SDIO_SLC_FRHOST_BIT14_INT_ST_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT14_INT_ST_S  6
/** SDIO_SLC_FRHOST_BIT15_INT_ST : RO; bitpos: [7]; default: 0;
 *  The masked interrupt status bit for SLC_FRHOST_BIT7_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT15_INT_ST    (BIT(7))
#define SDIO_SLC_FRHOST_BIT15_INT_ST_M  (SDIO_SLC_FRHOST_BIT15_INT_ST_V << SDIO_SLC_FRHOST_BIT15_INT_ST_S)
#define SDIO_SLC_FRHOST_BIT15_INT_ST_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT15_INT_ST_S  7
/** SDIO_SLC1_RX_START_INT_ST : RO; bitpos: [8]; default: 0;
 *  The masked interrupt status bit for SLC1_RX_START_INT_ST interrupt.
 */
#define SDIO_SLC1_RX_START_INT_ST    (BIT(8))
#define SDIO_SLC1_RX_START_INT_ST_M  (SDIO_SLC1_RX_START_INT_ST_V << SDIO_SLC1_RX_START_INT_ST_S)
#define SDIO_SLC1_RX_START_INT_ST_V  0x00000001U
#define SDIO_SLC1_RX_START_INT_ST_S  8
/** SDIO_SLC1_TX_START_INT_ST : RO; bitpos: [9]; default: 0;
 *  The masked interrupt status bit for SLC1_TX_START_INT_ST interrupt.
 */
#define SDIO_SLC1_TX_START_INT_ST    (BIT(9))
#define SDIO_SLC1_TX_START_INT_ST_M  (SDIO_SLC1_TX_START_INT_ST_V << SDIO_SLC1_TX_START_INT_ST_S)
#define SDIO_SLC1_TX_START_INT_ST_V  0x00000001U
#define SDIO_SLC1_TX_START_INT_ST_S  9
/** SDIO_SLC1_RX_UDF_INT_ST : RO; bitpos: [10]; default: 0;
 *  The masked interrupt status bit for SLC1_RX_UDF_INT interrupt.
 */
#define SDIO_SLC1_RX_UDF_INT_ST    (BIT(10))
#define SDIO_SLC1_RX_UDF_INT_ST_M  (SDIO_SLC1_RX_UDF_INT_ST_V << SDIO_SLC1_RX_UDF_INT_ST_S)
#define SDIO_SLC1_RX_UDF_INT_ST_V  0x00000001U
#define SDIO_SLC1_RX_UDF_INT_ST_S  10
/** SDIO_SLC1_TX_OVF_INT_ST : RO; bitpos: [11]; default: 0;
 *  The masked interrupt status bit for SLC1_TX_OVF_INT interrupt.
 */
#define SDIO_SLC1_TX_OVF_INT_ST    (BIT(11))
#define SDIO_SLC1_TX_OVF_INT_ST_M  (SDIO_SLC1_TX_OVF_INT_ST_V << SDIO_SLC1_TX_OVF_INT_ST_S)
#define SDIO_SLC1_TX_OVF_INT_ST_V  0x00000001U
#define SDIO_SLC1_TX_OVF_INT_ST_S  11
/** SDIO_SLC1_TOKEN0_1TO0_INT_ST : RO; bitpos: [12]; default: 0;
 *  The masked interrupt status bit for SLC1_TOKEN0_1TO0_INT interrupt.
 */
#define SDIO_SLC1_TOKEN0_1TO0_INT_ST    (BIT(12))
#define SDIO_SLC1_TOKEN0_1TO0_INT_ST_M  (SDIO_SLC1_TOKEN0_1TO0_INT_ST_V << SDIO_SLC1_TOKEN0_1TO0_INT_ST_S)
#define SDIO_SLC1_TOKEN0_1TO0_INT_ST_V  0x00000001U
#define SDIO_SLC1_TOKEN0_1TO0_INT_ST_S  12
/** SDIO_SLC1_TOKEN1_1TO0_INT_ST : RO; bitpos: [13]; default: 0;
 *  The masked interrupt status bit for SLC1_TOKEN1_1TO0_INT interrupt.
 */
#define SDIO_SLC1_TOKEN1_1TO0_INT_ST    (BIT(13))
#define SDIO_SLC1_TOKEN1_1TO0_INT_ST_M  (SDIO_SLC1_TOKEN1_1TO0_INT_ST_V << SDIO_SLC1_TOKEN1_1TO0_INT_ST_S)
#define SDIO_SLC1_TOKEN1_1TO0_INT_ST_V  0x00000001U
#define SDIO_SLC1_TOKEN1_1TO0_INT_ST_S  13
/** SDIO_SLC1_TX_DONE_INT_ST : RO; bitpos: [14]; default: 0;
 *  The masked interrupt status bit for SLC1_TX_DONE_INT_ST interrupt.
 */
#define SDIO_SLC1_TX_DONE_INT_ST    (BIT(14))
#define SDIO_SLC1_TX_DONE_INT_ST_M  (SDIO_SLC1_TX_DONE_INT_ST_V << SDIO_SLC1_TX_DONE_INT_ST_S)
#define SDIO_SLC1_TX_DONE_INT_ST_V  0x00000001U
#define SDIO_SLC1_TX_DONE_INT_ST_S  14
/** SDIO_SLC1_TX_SUC_EOF_INT_ST : RO; bitpos: [15]; default: 0;
 *  The masked interrupt status bit for SLC1_TX_SUC_EOF_INT interrupt.
 */
#define SDIO_SLC1_TX_SUC_EOF_INT_ST    (BIT(15))
#define SDIO_SLC1_TX_SUC_EOF_INT_ST_M  (SDIO_SLC1_TX_SUC_EOF_INT_ST_V << SDIO_SLC1_TX_SUC_EOF_INT_ST_S)
#define SDIO_SLC1_TX_SUC_EOF_INT_ST_V  0x00000001U
#define SDIO_SLC1_TX_SUC_EOF_INT_ST_S  15
/** SDIO_SLC1_RX_DONE_INT_ST : RO; bitpos: [16]; default: 0;
 *  The masked interrupt status bit for SLC1_RX_DONE_INT interrupt.
 */
#define SDIO_SLC1_RX_DONE_INT_ST    (BIT(16))
#define SDIO_SLC1_RX_DONE_INT_ST_M  (SDIO_SLC1_RX_DONE_INT_ST_V << SDIO_SLC1_RX_DONE_INT_ST_S)
#define SDIO_SLC1_RX_DONE_INT_ST_V  0x00000001U
#define SDIO_SLC1_RX_DONE_INT_ST_S  16
/** SDIO_SLC1_RX_EOF_INT_ST : RO; bitpos: [17]; default: 0;
 *  The masked interrupt status bit for SLC1_RX_EOF_INT interrupt.
 */
#define SDIO_SLC1_RX_EOF_INT_ST    (BIT(17))
#define SDIO_SLC1_RX_EOF_INT_ST_M  (SDIO_SLC1_RX_EOF_INT_ST_V << SDIO_SLC1_RX_EOF_INT_ST_S)
#define SDIO_SLC1_RX_EOF_INT_ST_V  0x00000001U
#define SDIO_SLC1_RX_EOF_INT_ST_S  17
/** SDIO_SLC1_TOHOST_INT_ST : RO; bitpos: [18]; default: 0;
 *  The masked interrupt status bit for SLC1_TOHOST_INT interrupt.
 */
#define SDIO_SLC1_TOHOST_INT_ST    (BIT(18))
#define SDIO_SLC1_TOHOST_INT_ST_M  (SDIO_SLC1_TOHOST_INT_ST_V << SDIO_SLC1_TOHOST_INT_ST_S)
#define SDIO_SLC1_TOHOST_INT_ST_V  0x00000001U
#define SDIO_SLC1_TOHOST_INT_ST_S  18
/** SDIO_SLC1_TX_DSCR_ERR_INT_ST : RO; bitpos: [19]; default: 0;
 *  The masked interrupt status bit for SLC1_TX_DSCR_ERR_INT interrupt.
 */
#define SDIO_SLC1_TX_DSCR_ERR_INT_ST    (BIT(19))
#define SDIO_SLC1_TX_DSCR_ERR_INT_ST_M  (SDIO_SLC1_TX_DSCR_ERR_INT_ST_V << SDIO_SLC1_TX_DSCR_ERR_INT_ST_S)
#define SDIO_SLC1_TX_DSCR_ERR_INT_ST_V  0x00000001U
#define SDIO_SLC1_TX_DSCR_ERR_INT_ST_S  19
/** SDIO_SLC1_RX_DSCR_ERR_INT_ST : RO; bitpos: [20]; default: 0;
 *  The masked interrupt status bit for SLC1_RX_DSCR_ERR_INT interrupt.
 */
#define SDIO_SLC1_RX_DSCR_ERR_INT_ST    (BIT(20))
#define SDIO_SLC1_RX_DSCR_ERR_INT_ST_M  (SDIO_SLC1_RX_DSCR_ERR_INT_ST_V << SDIO_SLC1_RX_DSCR_ERR_INT_ST_S)
#define SDIO_SLC1_RX_DSCR_ERR_INT_ST_V  0x00000001U
#define SDIO_SLC1_RX_DSCR_ERR_INT_ST_S  20
/** SDIO_SLC1_TX_DSCR_EMPTY_INT_ST : RO; bitpos: [21]; default: 0;
 *  The masked interrupt status bit for SLC1_TX_DSCR_EMPTY_INT interrupt.
 */
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_ST    (BIT(21))
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_ST_M  (SDIO_SLC1_TX_DSCR_EMPTY_INT_ST_V << SDIO_SLC1_TX_DSCR_EMPTY_INT_ST_S)
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_ST_V  0x00000001U
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_ST_S  21
/** SDIO_SLC1_HOST_RD_ACK_INT_ST : RO; bitpos: [22]; default: 0;
 *  The masked interrupt status bit for SLC1_HOST_RD_ACK_INT interrupt.
 */
#define SDIO_SLC1_HOST_RD_ACK_INT_ST    (BIT(22))
#define SDIO_SLC1_HOST_RD_ACK_INT_ST_M  (SDIO_SLC1_HOST_RD_ACK_INT_ST_V << SDIO_SLC1_HOST_RD_ACK_INT_ST_S)
#define SDIO_SLC1_HOST_RD_ACK_INT_ST_V  0x00000001U
#define SDIO_SLC1_HOST_RD_ACK_INT_ST_S  22
/** SDIO_SLC1_WR_RETRY_DONE_INT_ST : RO; bitpos: [23]; default: 0;
 *  The masked interrupt status bit for SLC1_WR_RETRY_DONE_INT interrupt.
 */
#define SDIO_SLC1_WR_RETRY_DONE_INT_ST    (BIT(23))
#define SDIO_SLC1_WR_RETRY_DONE_INT_ST_M  (SDIO_SLC1_WR_RETRY_DONE_INT_ST_V << SDIO_SLC1_WR_RETRY_DONE_INT_ST_S)
#define SDIO_SLC1_WR_RETRY_DONE_INT_ST_V  0x00000001U
#define SDIO_SLC1_WR_RETRY_DONE_INT_ST_S  23
/** SDIO_SLC1_TX_ERR_EOF_INT_ST : RO; bitpos: [24]; default: 0;
 *  The masked interrupt status bit for SLC1_TX_ERR_EOF_INT interrupt.
 */
#define SDIO_SLC1_TX_ERR_EOF_INT_ST    (BIT(24))
#define SDIO_SLC1_TX_ERR_EOF_INT_ST_M  (SDIO_SLC1_TX_ERR_EOF_INT_ST_V << SDIO_SLC1_TX_ERR_EOF_INT_ST_S)
#define SDIO_SLC1_TX_ERR_EOF_INT_ST_V  0x00000001U
#define SDIO_SLC1_TX_ERR_EOF_INT_ST_S  24

/** SDIO_SLC_SLC1INT_ENA_REG register
 *  Sdio slave DMA channel1 interrupt enable register.
 */
#define SDIO_SLC_SLC1INT_ENA_REG (DR_REG_SLC_BASE + 0x1c)
/** SDIO_SLC_FRHOST_BIT8_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for SLC_FRHOST_BIT0_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT8_INT_ENA    (BIT(0))
#define SDIO_SLC_FRHOST_BIT8_INT_ENA_M  (SDIO_SLC_FRHOST_BIT8_INT_ENA_V << SDIO_SLC_FRHOST_BIT8_INT_ENA_S)
#define SDIO_SLC_FRHOST_BIT8_INT_ENA_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT8_INT_ENA_S  0
/** SDIO_SLC_FRHOST_BIT9_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for SLC_FRHOST_BIT1_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT9_INT_ENA    (BIT(1))
#define SDIO_SLC_FRHOST_BIT9_INT_ENA_M  (SDIO_SLC_FRHOST_BIT9_INT_ENA_V << SDIO_SLC_FRHOST_BIT9_INT_ENA_S)
#define SDIO_SLC_FRHOST_BIT9_INT_ENA_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT9_INT_ENA_S  1
/** SDIO_SLC_FRHOST_BIT10_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for SLC_FRHOST_BIT2_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT10_INT_ENA    (BIT(2))
#define SDIO_SLC_FRHOST_BIT10_INT_ENA_M  (SDIO_SLC_FRHOST_BIT10_INT_ENA_V << SDIO_SLC_FRHOST_BIT10_INT_ENA_S)
#define SDIO_SLC_FRHOST_BIT10_INT_ENA_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT10_INT_ENA_S  2
/** SDIO_SLC_FRHOST_BIT11_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for SLC_FRHOST_BIT3_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT11_INT_ENA    (BIT(3))
#define SDIO_SLC_FRHOST_BIT11_INT_ENA_M  (SDIO_SLC_FRHOST_BIT11_INT_ENA_V << SDIO_SLC_FRHOST_BIT11_INT_ENA_S)
#define SDIO_SLC_FRHOST_BIT11_INT_ENA_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT11_INT_ENA_S  3
/** SDIO_SLC_FRHOST_BIT12_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for SLC_FRHOST_BIT4_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT12_INT_ENA    (BIT(4))
#define SDIO_SLC_FRHOST_BIT12_INT_ENA_M  (SDIO_SLC_FRHOST_BIT12_INT_ENA_V << SDIO_SLC_FRHOST_BIT12_INT_ENA_S)
#define SDIO_SLC_FRHOST_BIT12_INT_ENA_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT12_INT_ENA_S  4
/** SDIO_SLC_FRHOST_BIT13_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for SLC_FRHOST_BIT5_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT13_INT_ENA    (BIT(5))
#define SDIO_SLC_FRHOST_BIT13_INT_ENA_M  (SDIO_SLC_FRHOST_BIT13_INT_ENA_V << SDIO_SLC_FRHOST_BIT13_INT_ENA_S)
#define SDIO_SLC_FRHOST_BIT13_INT_ENA_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT13_INT_ENA_S  5
/** SDIO_SLC_FRHOST_BIT14_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for SLC_FRHOST_BIT6_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT14_INT_ENA    (BIT(6))
#define SDIO_SLC_FRHOST_BIT14_INT_ENA_M  (SDIO_SLC_FRHOST_BIT14_INT_ENA_V << SDIO_SLC_FRHOST_BIT14_INT_ENA_S)
#define SDIO_SLC_FRHOST_BIT14_INT_ENA_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT14_INT_ENA_S  6
/** SDIO_SLC_FRHOST_BIT15_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for SLC_FRHOST_BIT7_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT15_INT_ENA    (BIT(7))
#define SDIO_SLC_FRHOST_BIT15_INT_ENA_M  (SDIO_SLC_FRHOST_BIT15_INT_ENA_V << SDIO_SLC_FRHOST_BIT15_INT_ENA_S)
#define SDIO_SLC_FRHOST_BIT15_INT_ENA_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT15_INT_ENA_S  7
/** SDIO_SLC1_RX_START_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for SCL1_RX_START_INT_ST interrupt.
 */
#define SDIO_SLC1_RX_START_INT_ENA    (BIT(8))
#define SDIO_SLC1_RX_START_INT_ENA_M  (SDIO_SLC1_RX_START_INT_ENA_V << SDIO_SLC1_RX_START_INT_ENA_S)
#define SDIO_SLC1_RX_START_INT_ENA_V  0x00000001U
#define SDIO_SLC1_RX_START_INT_ENA_S  8
/** SDIO_SLC1_TX_START_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for SCL1_TX_START_INT_ST interrupt.
 */
#define SDIO_SLC1_TX_START_INT_ENA    (BIT(9))
#define SDIO_SLC1_TX_START_INT_ENA_M  (SDIO_SLC1_TX_START_INT_ENA_V << SDIO_SLC1_TX_START_INT_ENA_S)
#define SDIO_SLC1_TX_START_INT_ENA_V  0x00000001U
#define SDIO_SLC1_TX_START_INT_ENA_S  9
/** SDIO_SLC1_RX_UDF_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  The interrupt enable bit for SCL1_RX_UDF_INT interrupt.
 */
#define SDIO_SLC1_RX_UDF_INT_ENA    (BIT(10))
#define SDIO_SLC1_RX_UDF_INT_ENA_M  (SDIO_SLC1_RX_UDF_INT_ENA_V << SDIO_SLC1_RX_UDF_INT_ENA_S)
#define SDIO_SLC1_RX_UDF_INT_ENA_V  0x00000001U
#define SDIO_SLC1_RX_UDF_INT_ENA_S  10
/** SDIO_SLC1_TX_OVF_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  The interrupt enable bit for SCL1_TX_OVF_INT interrupt.
 */
#define SDIO_SLC1_TX_OVF_INT_ENA    (BIT(11))
#define SDIO_SLC1_TX_OVF_INT_ENA_M  (SDIO_SLC1_TX_OVF_INT_ENA_V << SDIO_SLC1_TX_OVF_INT_ENA_S)
#define SDIO_SLC1_TX_OVF_INT_ENA_V  0x00000001U
#define SDIO_SLC1_TX_OVF_INT_ENA_S  11
/** SDIO_SLC1_TOKEN0_1TO0_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  The interrupt enable bit for SCL1_TOKEN0_1TO0_INT interrupt.
 */
#define SDIO_SLC1_TOKEN0_1TO0_INT_ENA    (BIT(12))
#define SDIO_SLC1_TOKEN0_1TO0_INT_ENA_M  (SDIO_SLC1_TOKEN0_1TO0_INT_ENA_V << SDIO_SLC1_TOKEN0_1TO0_INT_ENA_S)
#define SDIO_SLC1_TOKEN0_1TO0_INT_ENA_V  0x00000001U
#define SDIO_SLC1_TOKEN0_1TO0_INT_ENA_S  12
/** SDIO_SLC1_TOKEN1_1TO0_INT_ENA : R/W; bitpos: [13]; default: 0;
 *  The interrupt enable bit for SCL1_TOKEN1_1TO0_INT interrupt.
 */
#define SDIO_SLC1_TOKEN1_1TO0_INT_ENA    (BIT(13))
#define SDIO_SLC1_TOKEN1_1TO0_INT_ENA_M  (SDIO_SLC1_TOKEN1_1TO0_INT_ENA_V << SDIO_SLC1_TOKEN1_1TO0_INT_ENA_S)
#define SDIO_SLC1_TOKEN1_1TO0_INT_ENA_V  0x00000001U
#define SDIO_SLC1_TOKEN1_1TO0_INT_ENA_S  13
/** SDIO_SLC1_TX_DONE_INT_ENA : R/W; bitpos: [14]; default: 0;
 *  The interrupt enable bit for SCL1_TX_DONE_INT_ST interrupt.
 */
#define SDIO_SLC1_TX_DONE_INT_ENA    (BIT(14))
#define SDIO_SLC1_TX_DONE_INT_ENA_M  (SDIO_SLC1_TX_DONE_INT_ENA_V << SDIO_SLC1_TX_DONE_INT_ENA_S)
#define SDIO_SLC1_TX_DONE_INT_ENA_V  0x00000001U
#define SDIO_SLC1_TX_DONE_INT_ENA_S  14
/** SDIO_SLC1_TX_SUC_EOF_INT_ENA : R/W; bitpos: [15]; default: 0;
 *  The interrupt enable bit for SCL1_TX_SUC_EOF_INT interrupt.
 */
#define SDIO_SLC1_TX_SUC_EOF_INT_ENA    (BIT(15))
#define SDIO_SLC1_TX_SUC_EOF_INT_ENA_M  (SDIO_SLC1_TX_SUC_EOF_INT_ENA_V << SDIO_SLC1_TX_SUC_EOF_INT_ENA_S)
#define SDIO_SLC1_TX_SUC_EOF_INT_ENA_V  0x00000001U
#define SDIO_SLC1_TX_SUC_EOF_INT_ENA_S  15
/** SDIO_SLC1_RX_DONE_INT_ENA : R/W; bitpos: [16]; default: 0;
 *  The interrupt enable bit for SCL1_RX_DONE_INT interrupt.
 */
#define SDIO_SLC1_RX_DONE_INT_ENA    (BIT(16))
#define SDIO_SLC1_RX_DONE_INT_ENA_M  (SDIO_SLC1_RX_DONE_INT_ENA_V << SDIO_SLC1_RX_DONE_INT_ENA_S)
#define SDIO_SLC1_RX_DONE_INT_ENA_V  0x00000001U
#define SDIO_SLC1_RX_DONE_INT_ENA_S  16
/** SDIO_SLC1_RX_EOF_INT_ENA : R/W; bitpos: [17]; default: 0;
 *  The interrupt enable bit for SCL1_RX_EOF_INT interrupt.
 */
#define SDIO_SLC1_RX_EOF_INT_ENA    (BIT(17))
#define SDIO_SLC1_RX_EOF_INT_ENA_M  (SDIO_SLC1_RX_EOF_INT_ENA_V << SDIO_SLC1_RX_EOF_INT_ENA_S)
#define SDIO_SLC1_RX_EOF_INT_ENA_V  0x00000001U
#define SDIO_SLC1_RX_EOF_INT_ENA_S  17
/** SDIO_SLC1_TOHOST_INT_ENA : R/W; bitpos: [18]; default: 0;
 *  The interrupt enable bit for SCL1_TOHOST_INT interrupt.
 */
#define SDIO_SLC1_TOHOST_INT_ENA    (BIT(18))
#define SDIO_SLC1_TOHOST_INT_ENA_M  (SDIO_SLC1_TOHOST_INT_ENA_V << SDIO_SLC1_TOHOST_INT_ENA_S)
#define SDIO_SLC1_TOHOST_INT_ENA_V  0x00000001U
#define SDIO_SLC1_TOHOST_INT_ENA_S  18
/** SDIO_SLC1_TX_DSCR_ERR_INT_ENA : R/W; bitpos: [19]; default: 0;
 *  The interrupt enable bit for SCL1_TX_DSCR_ERR_INT interrupt.
 */
#define SDIO_SLC1_TX_DSCR_ERR_INT_ENA    (BIT(19))
#define SDIO_SLC1_TX_DSCR_ERR_INT_ENA_M  (SDIO_SLC1_TX_DSCR_ERR_INT_ENA_V << SDIO_SLC1_TX_DSCR_ERR_INT_ENA_S)
#define SDIO_SLC1_TX_DSCR_ERR_INT_ENA_V  0x00000001U
#define SDIO_SLC1_TX_DSCR_ERR_INT_ENA_S  19
/** SDIO_SLC1_RX_DSCR_ERR_INT_ENA : R/W; bitpos: [20]; default: 0;
 *  The interrupt enable bit for SCL1_RX_DSCR_ERR_INT interrupt.
 */
#define SDIO_SLC1_RX_DSCR_ERR_INT_ENA    (BIT(20))
#define SDIO_SLC1_RX_DSCR_ERR_INT_ENA_M  (SDIO_SLC1_RX_DSCR_ERR_INT_ENA_V << SDIO_SLC1_RX_DSCR_ERR_INT_ENA_S)
#define SDIO_SLC1_RX_DSCR_ERR_INT_ENA_V  0x00000001U
#define SDIO_SLC1_RX_DSCR_ERR_INT_ENA_S  20
/** SDIO_SLC1_TX_DSCR_EMPTY_INT_ENA : R/W; bitpos: [21]; default: 0;
 *  The interrupt enable bit for SCL1_TX_DSCR_EMPTY_INT interrupt.
 */
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_ENA    (BIT(21))
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_ENA_M  (SDIO_SLC1_TX_DSCR_EMPTY_INT_ENA_V << SDIO_SLC1_TX_DSCR_EMPTY_INT_ENA_S)
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_ENA_V  0x00000001U
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_ENA_S  21
/** SDIO_SLC1_HOST_RD_ACK_INT_ENA : R/W; bitpos: [22]; default: 0;
 *  The interrupt enable bit for SCL1_HOST_RD_ACK_INT interrupt.
 */
#define SDIO_SLC1_HOST_RD_ACK_INT_ENA    (BIT(22))
#define SDIO_SLC1_HOST_RD_ACK_INT_ENA_M  (SDIO_SLC1_HOST_RD_ACK_INT_ENA_V << SDIO_SLC1_HOST_RD_ACK_INT_ENA_S)
#define SDIO_SLC1_HOST_RD_ACK_INT_ENA_V  0x00000001U
#define SDIO_SLC1_HOST_RD_ACK_INT_ENA_S  22
/** SDIO_SLC1_WR_RETRY_DONE_INT_ENA : R/W; bitpos: [23]; default: 0;
 *  The interrupt enable bit for SCL1_WR_RETRY_DONE_INT interrupt.
 */
#define SDIO_SLC1_WR_RETRY_DONE_INT_ENA    (BIT(23))
#define SDIO_SLC1_WR_RETRY_DONE_INT_ENA_M  (SDIO_SLC1_WR_RETRY_DONE_INT_ENA_V << SDIO_SLC1_WR_RETRY_DONE_INT_ENA_S)
#define SDIO_SLC1_WR_RETRY_DONE_INT_ENA_V  0x00000001U
#define SDIO_SLC1_WR_RETRY_DONE_INT_ENA_S  23
/** SDIO_SLC1_TX_ERR_EOF_INT_ENA : R/W; bitpos: [24]; default: 0;
 *  The interrupt enable bit for SCL1_TX_ERR_EOF_INT interrupt.
 */
#define SDIO_SLC1_TX_ERR_EOF_INT_ENA    (BIT(24))
#define SDIO_SLC1_TX_ERR_EOF_INT_ENA_M  (SDIO_SLC1_TX_ERR_EOF_INT_ENA_V << SDIO_SLC1_TX_ERR_EOF_INT_ENA_S)
#define SDIO_SLC1_TX_ERR_EOF_INT_ENA_V  0x00000001U
#define SDIO_SLC1_TX_ERR_EOF_INT_ENA_S  24

/** SDIO_SLC_SLC1INT_CLR_REG register
 *  Sdio slave DMA channel1 interrupt clear register.
 */
#define SDIO_SLC_SLC1INT_CLR_REG (DR_REG_SLC_BASE + 0x20)
/** SDIO_SLC_FRHOST_BIT8_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear SLC_FRHOST_BIT0_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT8_INT_CLR    (BIT(0))
#define SDIO_SLC_FRHOST_BIT8_INT_CLR_M  (SDIO_SLC_FRHOST_BIT8_INT_CLR_V << SDIO_SLC_FRHOST_BIT8_INT_CLR_S)
#define SDIO_SLC_FRHOST_BIT8_INT_CLR_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT8_INT_CLR_S  0
/** SDIO_SLC_FRHOST_BIT9_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear SLC_FRHOST_BIT1_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT9_INT_CLR    (BIT(1))
#define SDIO_SLC_FRHOST_BIT9_INT_CLR_M  (SDIO_SLC_FRHOST_BIT9_INT_CLR_V << SDIO_SLC_FRHOST_BIT9_INT_CLR_S)
#define SDIO_SLC_FRHOST_BIT9_INT_CLR_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT9_INT_CLR_S  1
/** SDIO_SLC_FRHOST_BIT10_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear SLC_FRHOST_BIT2_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT10_INT_CLR    (BIT(2))
#define SDIO_SLC_FRHOST_BIT10_INT_CLR_M  (SDIO_SLC_FRHOST_BIT10_INT_CLR_V << SDIO_SLC_FRHOST_BIT10_INT_CLR_S)
#define SDIO_SLC_FRHOST_BIT10_INT_CLR_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT10_INT_CLR_S  2
/** SDIO_SLC_FRHOST_BIT11_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear SLC_FRHOST_BIT3_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT11_INT_CLR    (BIT(3))
#define SDIO_SLC_FRHOST_BIT11_INT_CLR_M  (SDIO_SLC_FRHOST_BIT11_INT_CLR_V << SDIO_SLC_FRHOST_BIT11_INT_CLR_S)
#define SDIO_SLC_FRHOST_BIT11_INT_CLR_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT11_INT_CLR_S  3
/** SDIO_SLC_FRHOST_BIT12_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear SLC_FRHOST_BIT4_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT12_INT_CLR    (BIT(4))
#define SDIO_SLC_FRHOST_BIT12_INT_CLR_M  (SDIO_SLC_FRHOST_BIT12_INT_CLR_V << SDIO_SLC_FRHOST_BIT12_INT_CLR_S)
#define SDIO_SLC_FRHOST_BIT12_INT_CLR_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT12_INT_CLR_S  4
/** SDIO_SLC_FRHOST_BIT13_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear SLC_FRHOST_BIT5_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT13_INT_CLR    (BIT(5))
#define SDIO_SLC_FRHOST_BIT13_INT_CLR_M  (SDIO_SLC_FRHOST_BIT13_INT_CLR_V << SDIO_SLC_FRHOST_BIT13_INT_CLR_S)
#define SDIO_SLC_FRHOST_BIT13_INT_CLR_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT13_INT_CLR_S  5
/** SDIO_SLC_FRHOST_BIT14_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear SLC_FRHOST_BIT6_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT14_INT_CLR    (BIT(6))
#define SDIO_SLC_FRHOST_BIT14_INT_CLR_M  (SDIO_SLC_FRHOST_BIT14_INT_CLR_V << SDIO_SLC_FRHOST_BIT14_INT_CLR_S)
#define SDIO_SLC_FRHOST_BIT14_INT_CLR_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT14_INT_CLR_S  6
/** SDIO_SLC_FRHOST_BIT15_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear SLC_FRHOST_BIT7_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT15_INT_CLR    (BIT(7))
#define SDIO_SLC_FRHOST_BIT15_INT_CLR_M  (SDIO_SLC_FRHOST_BIT15_INT_CLR_V << SDIO_SLC_FRHOST_BIT15_INT_CLR_S)
#define SDIO_SLC_FRHOST_BIT15_INT_CLR_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT15_INT_CLR_S  7
/** SDIO_SLC1_RX_START_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear SLC1_RX_START_INT_ST interrupt.
 */
#define SDIO_SLC1_RX_START_INT_CLR    (BIT(8))
#define SDIO_SLC1_RX_START_INT_CLR_M  (SDIO_SLC1_RX_START_INT_CLR_V << SDIO_SLC1_RX_START_INT_CLR_S)
#define SDIO_SLC1_RX_START_INT_CLR_V  0x00000001U
#define SDIO_SLC1_RX_START_INT_CLR_S  8
/** SDIO_SLC1_TX_START_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear SLC1_TX_START_INT_ST interrupt.
 */
#define SDIO_SLC1_TX_START_INT_CLR    (BIT(9))
#define SDIO_SLC1_TX_START_INT_CLR_M  (SDIO_SLC1_TX_START_INT_CLR_V << SDIO_SLC1_TX_START_INT_CLR_S)
#define SDIO_SLC1_TX_START_INT_CLR_V  0x00000001U
#define SDIO_SLC1_TX_START_INT_CLR_S  9
/** SDIO_SLC1_RX_UDF_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Set this bit to clear SLC1_RX_UDF_INT interrupt.
 */
#define SDIO_SLC1_RX_UDF_INT_CLR    (BIT(10))
#define SDIO_SLC1_RX_UDF_INT_CLR_M  (SDIO_SLC1_RX_UDF_INT_CLR_V << SDIO_SLC1_RX_UDF_INT_CLR_S)
#define SDIO_SLC1_RX_UDF_INT_CLR_V  0x00000001U
#define SDIO_SLC1_RX_UDF_INT_CLR_S  10
/** SDIO_SLC1_TX_OVF_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Set this bit to clear SLC1_TX_OVF_INT interrupt.
 */
#define SDIO_SLC1_TX_OVF_INT_CLR    (BIT(11))
#define SDIO_SLC1_TX_OVF_INT_CLR_M  (SDIO_SLC1_TX_OVF_INT_CLR_V << SDIO_SLC1_TX_OVF_INT_CLR_S)
#define SDIO_SLC1_TX_OVF_INT_CLR_V  0x00000001U
#define SDIO_SLC1_TX_OVF_INT_CLR_S  11
/** SDIO_SLC1_TOKEN0_1TO0_INT_CLR : WT; bitpos: [12]; default: 0;
 *  Set this bit to clear SLC1_TOKEN0_1TO0_INT interrupt.
 */
#define SDIO_SLC1_TOKEN0_1TO0_INT_CLR    (BIT(12))
#define SDIO_SLC1_TOKEN0_1TO0_INT_CLR_M  (SDIO_SLC1_TOKEN0_1TO0_INT_CLR_V << SDIO_SLC1_TOKEN0_1TO0_INT_CLR_S)
#define SDIO_SLC1_TOKEN0_1TO0_INT_CLR_V  0x00000001U
#define SDIO_SLC1_TOKEN0_1TO0_INT_CLR_S  12
/** SDIO_SLC1_TOKEN1_1TO0_INT_CLR : WT; bitpos: [13]; default: 0;
 *  Set this bit to clear SLC1_TOKEN1_1TO0_INT interrupt.
 */
#define SDIO_SLC1_TOKEN1_1TO0_INT_CLR    (BIT(13))
#define SDIO_SLC1_TOKEN1_1TO0_INT_CLR_M  (SDIO_SLC1_TOKEN1_1TO0_INT_CLR_V << SDIO_SLC1_TOKEN1_1TO0_INT_CLR_S)
#define SDIO_SLC1_TOKEN1_1TO0_INT_CLR_V  0x00000001U
#define SDIO_SLC1_TOKEN1_1TO0_INT_CLR_S  13
/** SDIO_SLC1_TX_DONE_INT_CLR : WT; bitpos: [14]; default: 0;
 *  Set this bit to clear SLC1_TX_DONE_INT_ST interrupt.
 */
#define SDIO_SLC1_TX_DONE_INT_CLR    (BIT(14))
#define SDIO_SLC1_TX_DONE_INT_CLR_M  (SDIO_SLC1_TX_DONE_INT_CLR_V << SDIO_SLC1_TX_DONE_INT_CLR_S)
#define SDIO_SLC1_TX_DONE_INT_CLR_V  0x00000001U
#define SDIO_SLC1_TX_DONE_INT_CLR_S  14
/** SDIO_SLC1_TX_SUC_EOF_INT_CLR : WT; bitpos: [15]; default: 0;
 *  Set this bit to clear SLC1_TX_SUC_EOF_INT interrupt.
 */
#define SDIO_SLC1_TX_SUC_EOF_INT_CLR    (BIT(15))
#define SDIO_SLC1_TX_SUC_EOF_INT_CLR_M  (SDIO_SLC1_TX_SUC_EOF_INT_CLR_V << SDIO_SLC1_TX_SUC_EOF_INT_CLR_S)
#define SDIO_SLC1_TX_SUC_EOF_INT_CLR_V  0x00000001U
#define SDIO_SLC1_TX_SUC_EOF_INT_CLR_S  15
/** SDIO_SLC1_RX_DONE_INT_CLR : WT; bitpos: [16]; default: 0;
 *  Set this bit to clear SLC1_RX_DONE_INT interrupt.
 */
#define SDIO_SLC1_RX_DONE_INT_CLR    (BIT(16))
#define SDIO_SLC1_RX_DONE_INT_CLR_M  (SDIO_SLC1_RX_DONE_INT_CLR_V << SDIO_SLC1_RX_DONE_INT_CLR_S)
#define SDIO_SLC1_RX_DONE_INT_CLR_V  0x00000001U
#define SDIO_SLC1_RX_DONE_INT_CLR_S  16
/** SDIO_SLC1_RX_EOF_INT_CLR : WT; bitpos: [17]; default: 0;
 *  Set this bit to clear SLC1_RX_EOF_INT interrupt.
 */
#define SDIO_SLC1_RX_EOF_INT_CLR    (BIT(17))
#define SDIO_SLC1_RX_EOF_INT_CLR_M  (SDIO_SLC1_RX_EOF_INT_CLR_V << SDIO_SLC1_RX_EOF_INT_CLR_S)
#define SDIO_SLC1_RX_EOF_INT_CLR_V  0x00000001U
#define SDIO_SLC1_RX_EOF_INT_CLR_S  17
/** SDIO_SLC1_TOHOST_INT_CLR : WT; bitpos: [18]; default: 0;
 *  Set this bit to clear SLC1_TOHOST_INT interrupt.
 */
#define SDIO_SLC1_TOHOST_INT_CLR    (BIT(18))
#define SDIO_SLC1_TOHOST_INT_CLR_M  (SDIO_SLC1_TOHOST_INT_CLR_V << SDIO_SLC1_TOHOST_INT_CLR_S)
#define SDIO_SLC1_TOHOST_INT_CLR_V  0x00000001U
#define SDIO_SLC1_TOHOST_INT_CLR_S  18
/** SDIO_SLC1_TX_DSCR_ERR_INT_CLR : WT; bitpos: [19]; default: 0;
 *  Set this bit to clear SLC1_TX_DSCR_ERR_INT interrupt.
 */
#define SDIO_SLC1_TX_DSCR_ERR_INT_CLR    (BIT(19))
#define SDIO_SLC1_TX_DSCR_ERR_INT_CLR_M  (SDIO_SLC1_TX_DSCR_ERR_INT_CLR_V << SDIO_SLC1_TX_DSCR_ERR_INT_CLR_S)
#define SDIO_SLC1_TX_DSCR_ERR_INT_CLR_V  0x00000001U
#define SDIO_SLC1_TX_DSCR_ERR_INT_CLR_S  19
/** SDIO_SLC1_RX_DSCR_ERR_INT_CLR : WT; bitpos: [20]; default: 0;
 *  Set this bit to clear SLC1_RX_DSCR_ERR_INT interrupt.
 */
#define SDIO_SLC1_RX_DSCR_ERR_INT_CLR    (BIT(20))
#define SDIO_SLC1_RX_DSCR_ERR_INT_CLR_M  (SDIO_SLC1_RX_DSCR_ERR_INT_CLR_V << SDIO_SLC1_RX_DSCR_ERR_INT_CLR_S)
#define SDIO_SLC1_RX_DSCR_ERR_INT_CLR_V  0x00000001U
#define SDIO_SLC1_RX_DSCR_ERR_INT_CLR_S  20
/** SDIO_SLC1_TX_DSCR_EMPTY_INT_CLR : WT; bitpos: [21]; default: 0;
 *  Set this bit to clear SLC1_TX_DSCR_EMPTY_INT interrupt.
 */
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_CLR    (BIT(21))
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_CLR_M  (SDIO_SLC1_TX_DSCR_EMPTY_INT_CLR_V << SDIO_SLC1_TX_DSCR_EMPTY_INT_CLR_S)
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_CLR_V  0x00000001U
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_CLR_S  21
/** SDIO_SLC1_HOST_RD_ACK_INT_CLR : WT; bitpos: [22]; default: 0;
 *  Set this bit to clear SLC1_HOST_RD_ACK_INT interrupt.
 */
#define SDIO_SLC1_HOST_RD_ACK_INT_CLR    (BIT(22))
#define SDIO_SLC1_HOST_RD_ACK_INT_CLR_M  (SDIO_SLC1_HOST_RD_ACK_INT_CLR_V << SDIO_SLC1_HOST_RD_ACK_INT_CLR_S)
#define SDIO_SLC1_HOST_RD_ACK_INT_CLR_V  0x00000001U
#define SDIO_SLC1_HOST_RD_ACK_INT_CLR_S  22
/** SDIO_SLC1_WR_RETRY_DONE_INT_CLR : WT; bitpos: [23]; default: 0;
 *  Set this bit to clear SLC1_WR_RETRY_DONE_INT interrupt.
 */
#define SDIO_SLC1_WR_RETRY_DONE_INT_CLR    (BIT(23))
#define SDIO_SLC1_WR_RETRY_DONE_INT_CLR_M  (SDIO_SLC1_WR_RETRY_DONE_INT_CLR_V << SDIO_SLC1_WR_RETRY_DONE_INT_CLR_S)
#define SDIO_SLC1_WR_RETRY_DONE_INT_CLR_V  0x00000001U
#define SDIO_SLC1_WR_RETRY_DONE_INT_CLR_S  23
/** SDIO_SLC1_TX_ERR_EOF_INT_CLR : WT; bitpos: [24]; default: 0;
 *  Set this bit to clear SLC1_TX_ERR_EOF_INT interrupt.
 */
#define SDIO_SLC1_TX_ERR_EOF_INT_CLR    (BIT(24))
#define SDIO_SLC1_TX_ERR_EOF_INT_CLR_M  (SDIO_SLC1_TX_ERR_EOF_INT_CLR_V << SDIO_SLC1_TX_ERR_EOF_INT_CLR_S)
#define SDIO_SLC1_TX_ERR_EOF_INT_CLR_V  0x00000001U
#define SDIO_SLC1_TX_ERR_EOF_INT_CLR_S  24

/** SDIO_SLC_RX_STATUS_REG register
 *  Sdio slave DMA rxfifo status register.
 */
#define SDIO_SLC_RX_STATUS_REG (DR_REG_SLC_BASE + 0x24)
/** SDIO_SLC0_RX_FULL : RO; bitpos: [0]; default: 0;
 *  The status bit for rxfifo full during slave sending data to host through dma
 *  channel0.
 */
#define SDIO_SLC0_RX_FULL    (BIT(0))
#define SDIO_SLC0_RX_FULL_M  (SDIO_SLC0_RX_FULL_V << SDIO_SLC0_RX_FULL_S)
#define SDIO_SLC0_RX_FULL_V  0x00000001U
#define SDIO_SLC0_RX_FULL_S  0
/** SDIO_SLC0_RX_EMPTY : RO; bitpos: [1]; default: 1;
 *  The status bit for rxfifo empty during slave sending data to host through dma
 *  channel0.
 */
#define SDIO_SLC0_RX_EMPTY    (BIT(1))
#define SDIO_SLC0_RX_EMPTY_M  (SDIO_SLC0_RX_EMPTY_V << SDIO_SLC0_RX_EMPTY_S)
#define SDIO_SLC0_RX_EMPTY_V  0x00000001U
#define SDIO_SLC0_RX_EMPTY_S  1
/** SDIO_SLC0_RX_BUF_LEN : RO; bitpos: [15:2]; default: 0;
 *  the current buffer length during slave sending data to host through dma channel0.
 */
#define SDIO_SLC0_RX_BUF_LEN    0x00003FFFU
#define SDIO_SLC0_RX_BUF_LEN_M  (SDIO_SLC0_RX_BUF_LEN_V << SDIO_SLC0_RX_BUF_LEN_S)
#define SDIO_SLC0_RX_BUF_LEN_V  0x00003FFFU
#define SDIO_SLC0_RX_BUF_LEN_S  2
/** SDIO_SLC1_RX_FULL : RO; bitpos: [16]; default: 0;
 *  The status bit for rxfifo full during slave sending data to host through dma
 *  channel1.
 */
#define SDIO_SLC1_RX_FULL    (BIT(16))
#define SDIO_SLC1_RX_FULL_M  (SDIO_SLC1_RX_FULL_V << SDIO_SLC1_RX_FULL_S)
#define SDIO_SLC1_RX_FULL_V  0x00000001U
#define SDIO_SLC1_RX_FULL_S  16
/** SDIO_SLC1_RX_EMPTY : RO; bitpos: [17]; default: 1;
 *  The status bit for rxfifo empty during slave sending data to host through dma
 *  channel1.
 */
#define SDIO_SLC1_RX_EMPTY    (BIT(17))
#define SDIO_SLC1_RX_EMPTY_M  (SDIO_SLC1_RX_EMPTY_V << SDIO_SLC1_RX_EMPTY_S)
#define SDIO_SLC1_RX_EMPTY_V  0x00000001U
#define SDIO_SLC1_RX_EMPTY_S  17
/** SDIO_SLC1_RX_BUF_LEN : RO; bitpos: [31:18]; default: 0;
 *  the current buffer length during slave sending data to host through dma channel1.
 */
#define SDIO_SLC1_RX_BUF_LEN    0x00003FFFU
#define SDIO_SLC1_RX_BUF_LEN_M  (SDIO_SLC1_RX_BUF_LEN_V << SDIO_SLC1_RX_BUF_LEN_S)
#define SDIO_SLC1_RX_BUF_LEN_V  0x00003FFFU
#define SDIO_SLC1_RX_BUF_LEN_S  18

/** SDIO_SLC_SLC0RXFIFO_PUSH_REG register
 *  Sdio slave DMA channel0 rxfifo debug register.
 */
#define SDIO_SLC_SLC0RXFIFO_PUSH_REG (DR_REG_SLC_BASE + 0x28)
/** SDIO_SLC0_RXFIFO_WDATA : R/W; bitpos: [8:0]; default: 0;
 *  The data used to sent into rxfifo through APB bus.
 */
#define SDIO_SLC0_RXFIFO_WDATA    0x000001FFU
#define SDIO_SLC0_RXFIFO_WDATA_M  (SDIO_SLC0_RXFIFO_WDATA_V << SDIO_SLC0_RXFIFO_WDATA_S)
#define SDIO_SLC0_RXFIFO_WDATA_V  0x000001FFU
#define SDIO_SLC0_RXFIFO_WDATA_S  0
/** SDIO_SLC0_RXFIFO_PUSH : R/W/SC; bitpos: [16]; default: 0;
 *  Set this bit to send slc0_rxfifo_wdata into rxfifo through APB bus. This bit is
 *  cleared automatically after sending one data to slc0_rxfifo through APB bus
 *  successfully.
 */
#define SDIO_SLC0_RXFIFO_PUSH    (BIT(16))
#define SDIO_SLC0_RXFIFO_PUSH_M  (SDIO_SLC0_RXFIFO_PUSH_V << SDIO_SLC0_RXFIFO_PUSH_S)
#define SDIO_SLC0_RXFIFO_PUSH_V  0x00000001U
#define SDIO_SLC0_RXFIFO_PUSH_S  16

/** SDIO_SLC_SLC1RXFIFO_PUSH_REG register
 *  Sdio slave DMA channel1 rxfifo debug register.
 */
#define SDIO_SLC_SLC1RXFIFO_PUSH_REG (DR_REG_SLC_BASE + 0x2c)
/** SDIO_SLC1_RXFIFO_WDATA : R/W; bitpos: [8:0]; default: 0;
 *  The data used to sent into rxfifo through APB bus.
 */
#define SDIO_SLC1_RXFIFO_WDATA    0x000001FFU
#define SDIO_SLC1_RXFIFO_WDATA_M  (SDIO_SLC1_RXFIFO_WDATA_V << SDIO_SLC1_RXFIFO_WDATA_S)
#define SDIO_SLC1_RXFIFO_WDATA_V  0x000001FFU
#define SDIO_SLC1_RXFIFO_WDATA_S  0
/** SDIO_SLC1_RXFIFO_PUSH : R/W/SC; bitpos: [16]; default: 0;
 *  Set this bit to send slc1_rxfifo_wdata into rxfifo through APB bus. This bit is
 *  cleared automatically after sending one data to slc1_rxfifo through APB bus
 *  successfully.
 */
#define SDIO_SLC1_RXFIFO_PUSH    (BIT(16))
#define SDIO_SLC1_RXFIFO_PUSH_M  (SDIO_SLC1_RXFIFO_PUSH_V << SDIO_SLC1_RXFIFO_PUSH_S)
#define SDIO_SLC1_RXFIFO_PUSH_V  0x00000001U
#define SDIO_SLC1_RXFIFO_PUSH_S  16

/** SDIO_SLC_TX_STATUS_REG register
 *  Sdio slave DMA txfifo status register.
 */
#define SDIO_SLC_TX_STATUS_REG (DR_REG_SLC_BASE + 0x30)
/** SDIO_SLC0_TX_FULL : RO; bitpos: [0]; default: 0;
 *  The status bit for txfifo full during slave receiving data from host through dma
 *  channel0.
 */
#define SDIO_SLC0_TX_FULL    (BIT(0))
#define SDIO_SLC0_TX_FULL_M  (SDIO_SLC0_TX_FULL_V << SDIO_SLC0_TX_FULL_S)
#define SDIO_SLC0_TX_FULL_V  0x00000001U
#define SDIO_SLC0_TX_FULL_S  0
/** SDIO_SLC0_TX_EMPTY : RO; bitpos: [1]; default: 1;
 *  The status bit for txfifo empty during slave receiving data from host through dma
 *  channel0.
 */
#define SDIO_SLC0_TX_EMPTY    (BIT(1))
#define SDIO_SLC0_TX_EMPTY_M  (SDIO_SLC0_TX_EMPTY_V << SDIO_SLC0_TX_EMPTY_S)
#define SDIO_SLC0_TX_EMPTY_V  0x00000001U
#define SDIO_SLC0_TX_EMPTY_S  1
/** SDIO_SLC1_TX_FULL : RO; bitpos: [16]; default: 0;
 *  The status bit for txfifo full during slave receiving data from host through dma
 *  channel1.
 */
#define SDIO_SLC1_TX_FULL    (BIT(16))
#define SDIO_SLC1_TX_FULL_M  (SDIO_SLC1_TX_FULL_V << SDIO_SLC1_TX_FULL_S)
#define SDIO_SLC1_TX_FULL_V  0x00000001U
#define SDIO_SLC1_TX_FULL_S  16
/** SDIO_SLC1_TX_EMPTY : RO; bitpos: [17]; default: 1;
 *  The status bit for txfifo empty during slave receiving data from host through dma
 *  channel1.
 */
#define SDIO_SLC1_TX_EMPTY    (BIT(17))
#define SDIO_SLC1_TX_EMPTY_M  (SDIO_SLC1_TX_EMPTY_V << SDIO_SLC1_TX_EMPTY_S)
#define SDIO_SLC1_TX_EMPTY_V  0x00000001U
#define SDIO_SLC1_TX_EMPTY_S  17

/** SDIO_SLC_SLC0TXFIFO_POP_REG register
 *  Sdio slave DMA channel0 txfifo debug register.
 */
#define SDIO_SLC_SLC0TXFIFO_POP_REG (DR_REG_SLC_BASE + 0x34)
/** SDIO_SLC0_TXFIFO_RDATA : RO; bitpos: [10:0]; default: 1024;
 *  This field saves the data read from txfifo through APB bus.
 */
#define SDIO_SLC0_TXFIFO_RDATA    0x000007FFU
#define SDIO_SLC0_TXFIFO_RDATA_M  (SDIO_SLC0_TXFIFO_RDATA_V << SDIO_SLC0_TXFIFO_RDATA_S)
#define SDIO_SLC0_TXFIFO_RDATA_V  0x000007FFU
#define SDIO_SLC0_TXFIFO_RDATA_S  0
/** SDIO_SLC0_TXFIFO_POP : R/W/SC; bitpos: [16]; default: 0;
 *  Set this bit to read data from txfifo through APB bus. This bit is cleared
 *  automatically after reading one data from slc0_txfifo through APB bus successfully.
 */
#define SDIO_SLC0_TXFIFO_POP    (BIT(16))
#define SDIO_SLC0_TXFIFO_POP_M  (SDIO_SLC0_TXFIFO_POP_V << SDIO_SLC0_TXFIFO_POP_S)
#define SDIO_SLC0_TXFIFO_POP_V  0x00000001U
#define SDIO_SLC0_TXFIFO_POP_S  16

/** SDIO_SLC_SLC1TXFIFO_POP_REG register
 *  Sdio slave DMA channel1 txfifo debug register.
 */
#define SDIO_SLC_SLC1TXFIFO_POP_REG (DR_REG_SLC_BASE + 0x38)
/** SDIO_SLC1_TXFIFO_RDATA : RO; bitpos: [10:0]; default: 1024;
 *  This field saves the data read from txfifo through APB bus.
 */
#define SDIO_SLC1_TXFIFO_RDATA    0x000007FFU
#define SDIO_SLC1_TXFIFO_RDATA_M  (SDIO_SLC1_TXFIFO_RDATA_V << SDIO_SLC1_TXFIFO_RDATA_S)
#define SDIO_SLC1_TXFIFO_RDATA_V  0x000007FFU
#define SDIO_SLC1_TXFIFO_RDATA_S  0
/** SDIO_SLC1_TXFIFO_POP : R/W/SC; bitpos: [16]; default: 0;
 *  Set this bit to read data from txfifo through APB bus. This bit is cleared
 *  automatically after reading one data from slc1_txfifo through APB bus successfully.
 */
#define SDIO_SLC1_TXFIFO_POP    (BIT(16))
#define SDIO_SLC1_TXFIFO_POP_M  (SDIO_SLC1_TXFIFO_POP_V << SDIO_SLC1_TXFIFO_POP_S)
#define SDIO_SLC1_TXFIFO_POP_V  0x00000001U
#define SDIO_SLC1_TXFIFO_POP_S  16

/** SDIO_SLC_SLC0RX_LINK_REG register
 *  Sdio slave DMA channel0 configuration and status register of link list for slave
 *  sending data.
 */
#define SDIO_SLC_SLC0RX_LINK_REG (DR_REG_SLC_BASE + 0x3c)
/** SDIO_SLC0_RXLINK_STOP : R/W/SC; bitpos: [28]; default: 0;
 *  Set 1 to stop the linked list operation for dma channel0 sending packets.
 */
#define SDIO_SLC0_RXLINK_STOP    (BIT(28))
#define SDIO_SLC0_RXLINK_STOP_M  (SDIO_SLC0_RXLINK_STOP_V << SDIO_SLC0_RXLINK_STOP_S)
#define SDIO_SLC0_RXLINK_STOP_V  0x00000001U
#define SDIO_SLC0_RXLINK_STOP_S  28
/** SDIO_SLC0_RXLINK_START : R/W/SC; bitpos: [29]; default: 0;
 *  Set 1 to start the linked list operation for dma channel0 sending packets. Sending
 *  will start from address indicated by SLC0_RXLINK_ADDR.
 */
#define SDIO_SLC0_RXLINK_START    (BIT(29))
#define SDIO_SLC0_RXLINK_START_M  (SDIO_SLC0_RXLINK_START_V << SDIO_SLC0_RXLINK_START_S)
#define SDIO_SLC0_RXLINK_START_V  0x00000001U
#define SDIO_SLC0_RXLINK_START_S  29
/** SDIO_SLC0_RXLINK_RESTART : R/W/SC; bitpos: [30]; default: 0;
 *  Set 1 to restart and continue the linked list operation for dma channel0 sending
 *  packets.
 */
#define SDIO_SLC0_RXLINK_RESTART    (BIT(30))
#define SDIO_SLC0_RXLINK_RESTART_M  (SDIO_SLC0_RXLINK_RESTART_V << SDIO_SLC0_RXLINK_RESTART_S)
#define SDIO_SLC0_RXLINK_RESTART_V  0x00000001U
#define SDIO_SLC0_RXLINK_RESTART_S  30
/** SDIO_SLC0_RXLINK_PARK : RO; bitpos: [31]; default: 1;
 *  The status bit for sending link list fsm of dma channel0 in idle state
 */
#define SDIO_SLC0_RXLINK_PARK    (BIT(31))
#define SDIO_SLC0_RXLINK_PARK_M  (SDIO_SLC0_RXLINK_PARK_V << SDIO_SLC0_RXLINK_PARK_S)
#define SDIO_SLC0_RXLINK_PARK_V  0x00000001U
#define SDIO_SLC0_RXLINK_PARK_S  31

/** SDIO_SLC_SLC0RX_LINK_ADDR_REG register
 *  Sdio slave DMA channel0 address of first descriptor of sending link list.
 */
#define SDIO_SLC_SLC0RX_LINK_ADDR_REG (DR_REG_SLC_BASE + 0x40)
/** SDIO_SLC0_RXLINK_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  The initial address of dma channel0's sending linked list.
 */
#define SDIO_SLC0_RXLINK_ADDR    0xFFFFFFFFU
#define SDIO_SLC0_RXLINK_ADDR_M  (SDIO_SLC0_RXLINK_ADDR_V << SDIO_SLC0_RXLINK_ADDR_S)
#define SDIO_SLC0_RXLINK_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC0_RXLINK_ADDR_S  0

/** SDIO_SLC_SLC0TX_LINK_REG register
 *  Sdio slave DMA channel0 configuration and status register of link list for slave
 *  receiving data.
 */
#define SDIO_SLC_SLC0TX_LINK_REG (DR_REG_SLC_BASE + 0x44)
/** SDIO_SLC0_TXLINK_STOP : R/W/SC; bitpos: [28]; default: 0;
 *  Set 1 to stop the linked list operation for dma channel0 receiving packets.
 */
#define SDIO_SLC0_TXLINK_STOP    (BIT(28))
#define SDIO_SLC0_TXLINK_STOP_M  (SDIO_SLC0_TXLINK_STOP_V << SDIO_SLC0_TXLINK_STOP_S)
#define SDIO_SLC0_TXLINK_STOP_V  0x00000001U
#define SDIO_SLC0_TXLINK_STOP_S  28
/** SDIO_SLC0_TXLINK_START : R/W/SC; bitpos: [29]; default: 0;
 *  Set 1 to start the linked list operation for dma channel0 receiving packets.
 *  Sending will start from address indicated by SLC0_TXLINK_ADDR.
 */
#define SDIO_SLC0_TXLINK_START    (BIT(29))
#define SDIO_SLC0_TXLINK_START_M  (SDIO_SLC0_TXLINK_START_V << SDIO_SLC0_TXLINK_START_S)
#define SDIO_SLC0_TXLINK_START_V  0x00000001U
#define SDIO_SLC0_TXLINK_START_S  29
/** SDIO_SLC0_TXLINK_RESTART : R/W/SC; bitpos: [30]; default: 0;
 *  Set 1 to restart and continue the linked list operation for dma channel0 receiving
 *  packets.
 */
#define SDIO_SLC0_TXLINK_RESTART    (BIT(30))
#define SDIO_SLC0_TXLINK_RESTART_M  (SDIO_SLC0_TXLINK_RESTART_V << SDIO_SLC0_TXLINK_RESTART_S)
#define SDIO_SLC0_TXLINK_RESTART_V  0x00000001U
#define SDIO_SLC0_TXLINK_RESTART_S  30
/** SDIO_SLC0_TXLINK_PARK : RO; bitpos: [31]; default: 1;
 *  The status bit for receiving link list fsm of dma channel0 in idle state
 */
#define SDIO_SLC0_TXLINK_PARK    (BIT(31))
#define SDIO_SLC0_TXLINK_PARK_M  (SDIO_SLC0_TXLINK_PARK_V << SDIO_SLC0_TXLINK_PARK_S)
#define SDIO_SLC0_TXLINK_PARK_V  0x00000001U
#define SDIO_SLC0_TXLINK_PARK_S  31

/** SDIO_SLC_SLC0TX_LINK_ADDR_REG register
 *  Sdio slave DMA channel0 address of first descriptor of receiving linked list.
 */
#define SDIO_SLC_SLC0TX_LINK_ADDR_REG (DR_REG_SLC_BASE + 0x48)
/** SDIO_SLC0_TXLINK_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  The initial address of dma channel0's receiving linked list.
 */
#define SDIO_SLC0_TXLINK_ADDR    0xFFFFFFFFU
#define SDIO_SLC0_TXLINK_ADDR_M  (SDIO_SLC0_TXLINK_ADDR_V << SDIO_SLC0_TXLINK_ADDR_S)
#define SDIO_SLC0_TXLINK_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC0_TXLINK_ADDR_S  0

/** SDIO_SLC_SLC1RX_LINK_REG register
 *  Sdio slave DMA channel1 configuration and status register of link list for slave
 *  sending data.
 */
#define SDIO_SLC_SLC1RX_LINK_REG (DR_REG_SLC_BASE + 0x4c)
/** SDIO_SLC1_BT_PACKET : R/W; bitpos: [20]; default: 1;
 *  Configure the packet type for slave to send: 0- wifi packet, 1- blue-tooth packet.
 */
#define SDIO_SLC1_BT_PACKET    (BIT(20))
#define SDIO_SLC1_BT_PACKET_M  (SDIO_SLC1_BT_PACKET_V << SDIO_SLC1_BT_PACKET_S)
#define SDIO_SLC1_BT_PACKET_V  0x00000001U
#define SDIO_SLC1_BT_PACKET_S  20
/** SDIO_SLC1_RXLINK_STOP : R/W/SC; bitpos: [28]; default: 0;
 *  Set 1 to stop the linked list operation for dma channel1 sending packets.
 */
#define SDIO_SLC1_RXLINK_STOP    (BIT(28))
#define SDIO_SLC1_RXLINK_STOP_M  (SDIO_SLC1_RXLINK_STOP_V << SDIO_SLC1_RXLINK_STOP_S)
#define SDIO_SLC1_RXLINK_STOP_V  0x00000001U
#define SDIO_SLC1_RXLINK_STOP_S  28
/** SDIO_SLC1_RXLINK_START : R/W/SC; bitpos: [29]; default: 0;
 *  Set 1 to start the linked list operation for dma channel1 sending packets. Sending
 *  will start from address indicated by SLC0_RXLINK_ADDR.
 */
#define SDIO_SLC1_RXLINK_START    (BIT(29))
#define SDIO_SLC1_RXLINK_START_M  (SDIO_SLC1_RXLINK_START_V << SDIO_SLC1_RXLINK_START_S)
#define SDIO_SLC1_RXLINK_START_V  0x00000001U
#define SDIO_SLC1_RXLINK_START_S  29
/** SDIO_SLC1_RXLINK_RESTART : R/W/SC; bitpos: [30]; default: 0;
 *  Set 1 to restart and continue the linked list operation for dma channel1 sending
 *  packets.
 */
#define SDIO_SLC1_RXLINK_RESTART    (BIT(30))
#define SDIO_SLC1_RXLINK_RESTART_M  (SDIO_SLC1_RXLINK_RESTART_V << SDIO_SLC1_RXLINK_RESTART_S)
#define SDIO_SLC1_RXLINK_RESTART_V  0x00000001U
#define SDIO_SLC1_RXLINK_RESTART_S  30
/** SDIO_SLC1_RXLINK_PARK : RO; bitpos: [31]; default: 1;
 *  The status bit for sending link list fsm of dma channel1 in idle state
 */
#define SDIO_SLC1_RXLINK_PARK    (BIT(31))
#define SDIO_SLC1_RXLINK_PARK_M  (SDIO_SLC1_RXLINK_PARK_V << SDIO_SLC1_RXLINK_PARK_S)
#define SDIO_SLC1_RXLINK_PARK_V  0x00000001U
#define SDIO_SLC1_RXLINK_PARK_S  31

/** SDIO_SLC_SLC1RX_LINK_ADDR_REG register
 *  Sdio slave DMA channel1 address of first descriptor of sending link list.
 */
#define SDIO_SLC_SLC1RX_LINK_ADDR_REG (DR_REG_SLC_BASE + 0x50)
/** SDIO_SLC1_RXLINK_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  The initial address of dma channel1's sending linked list.
 */
#define SDIO_SLC1_RXLINK_ADDR    0xFFFFFFFFU
#define SDIO_SLC1_RXLINK_ADDR_M  (SDIO_SLC1_RXLINK_ADDR_V << SDIO_SLC1_RXLINK_ADDR_S)
#define SDIO_SLC1_RXLINK_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC1_RXLINK_ADDR_S  0

/** SDIO_SLC_SLC1TX_LINK_REG register
 *  Sdio slave DMA channel1 configuration and status register of link list for slave
 *  receiving data.
 */
#define SDIO_SLC_SLC1TX_LINK_REG (DR_REG_SLC_BASE + 0x54)
/** SDIO_SLC1_TXLINK_STOP : R/W/SC; bitpos: [28]; default: 0;
 *  Set 1 to stop the linked list operation for dma channel1 receiving packets.
 */
#define SDIO_SLC1_TXLINK_STOP    (BIT(28))
#define SDIO_SLC1_TXLINK_STOP_M  (SDIO_SLC1_TXLINK_STOP_V << SDIO_SLC1_TXLINK_STOP_S)
#define SDIO_SLC1_TXLINK_STOP_V  0x00000001U
#define SDIO_SLC1_TXLINK_STOP_S  28
/** SDIO_SLC1_TXLINK_START : R/W/SC; bitpos: [29]; default: 0;
 *  Set 1 to start the linked list operation for dma channel1 receiving packets.
 *  Sending will start from address indicated by SLC0_TXLINK_ADDR.
 */
#define SDIO_SLC1_TXLINK_START    (BIT(29))
#define SDIO_SLC1_TXLINK_START_M  (SDIO_SLC1_TXLINK_START_V << SDIO_SLC1_TXLINK_START_S)
#define SDIO_SLC1_TXLINK_START_V  0x00000001U
#define SDIO_SLC1_TXLINK_START_S  29
/** SDIO_SLC1_TXLINK_RESTART : R/W/SC; bitpos: [30]; default: 0;
 *  Set 1 to restart and continue the linked list operation for dma channel1 receiving
 *  packets.
 */
#define SDIO_SLC1_TXLINK_RESTART    (BIT(30))
#define SDIO_SLC1_TXLINK_RESTART_M  (SDIO_SLC1_TXLINK_RESTART_V << SDIO_SLC1_TXLINK_RESTART_S)
#define SDIO_SLC1_TXLINK_RESTART_V  0x00000001U
#define SDIO_SLC1_TXLINK_RESTART_S  30
/** SDIO_SLC1_TXLINK_PARK : RO; bitpos: [31]; default: 1;
 *  The status bit for receiving link list fsm of dma channel1 in idle state
 */
#define SDIO_SLC1_TXLINK_PARK    (BIT(31))
#define SDIO_SLC1_TXLINK_PARK_M  (SDIO_SLC1_TXLINK_PARK_V << SDIO_SLC1_TXLINK_PARK_S)
#define SDIO_SLC1_TXLINK_PARK_V  0x00000001U
#define SDIO_SLC1_TXLINK_PARK_S  31

/** SDIO_SLC_SLC1TX_LINK_ADDR_REG register
 *  Sdio slave DMA channel1 address of first descriptor of receiving linked list.
 */
#define SDIO_SLC_SLC1TX_LINK_ADDR_REG (DR_REG_SLC_BASE + 0x58)
/** SDIO_SLC1_TXLINK_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  The initial address of dma channel1's receiving linked list.
 */
#define SDIO_SLC1_TXLINK_ADDR    0xFFFFFFFFU
#define SDIO_SLC1_TXLINK_ADDR_M  (SDIO_SLC1_TXLINK_ADDR_V << SDIO_SLC1_TXLINK_ADDR_S)
#define SDIO_SLC1_TXLINK_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC1_TXLINK_ADDR_S  0

/** SDIO_SLC_SLCINTVEC_TOHOST_REG register
 *  Sdio slave DMA channel0 address of first descriptor of receiving linked list.
 */
#define SDIO_SLC_SLCINTVEC_TOHOST_REG (DR_REG_SLC_BASE + 0x5c)
/** SDIO_SLC0_TOHOST_INTVEC : WT; bitpos: [7:0]; default: 0;
 *  The interrupt vector for slave to interrupt host through dma channel0.
 */
#define SDIO_SLC0_TOHOST_INTVEC    0x000000FFU
#define SDIO_SLC0_TOHOST_INTVEC_M  (SDIO_SLC0_TOHOST_INTVEC_V << SDIO_SLC0_TOHOST_INTVEC_S)
#define SDIO_SLC0_TOHOST_INTVEC_V  0x000000FFU
#define SDIO_SLC0_TOHOST_INTVEC_S  0
/** SDIO_SLC1_TOHOST_INTVEC : WT; bitpos: [23:16]; default: 0;
 *  The interrupt vector for slave to interrupt host through dma channel1.
 */
#define SDIO_SLC1_TOHOST_INTVEC    0x000000FFU
#define SDIO_SLC1_TOHOST_INTVEC_M  (SDIO_SLC1_TOHOST_INTVEC_V << SDIO_SLC1_TOHOST_INTVEC_S)
#define SDIO_SLC1_TOHOST_INTVEC_V  0x000000FFU
#define SDIO_SLC1_TOHOST_INTVEC_S  16

/** SDIO_SLC_SLC0TOKEN0_REG register
 *  Sdio slave DMA channel0 receiving buffer configuration and status register.
 */
#define SDIO_SLC_SLC0TOKEN0_REG (DR_REG_SLC_BASE + 0x60)
/** SDIO_SLC0_TOKEN0_WDATA : WT; bitpos: [11:0]; default: 0;
 *  The number of available receiving buffers.
 */
#define SDIO_SLC0_TOKEN0_WDATA    0x00000FFFU
#define SDIO_SLC0_TOKEN0_WDATA_M  (SDIO_SLC0_TOKEN0_WDATA_V << SDIO_SLC0_TOKEN0_WDATA_S)
#define SDIO_SLC0_TOKEN0_WDATA_V  0x00000FFFU
#define SDIO_SLC0_TOKEN0_WDATA_S  0
/** SDIO_SLC0_TOKEN0_WR : WT; bitpos: [12]; default: 0;
 *  update slc0_token0_wdata into slc0 token0.
 */
#define SDIO_SLC0_TOKEN0_WR    (BIT(12))
#define SDIO_SLC0_TOKEN0_WR_M  (SDIO_SLC0_TOKEN0_WR_V << SDIO_SLC0_TOKEN0_WR_S)
#define SDIO_SLC0_TOKEN0_WR_V  0x00000001U
#define SDIO_SLC0_TOKEN0_WR_S  12
/** SDIO_SLC0_TOKEN0_INC : WT; bitpos: [13]; default: 0;
 *  Add 1 to slc0_token0.
 */
#define SDIO_SLC0_TOKEN0_INC    (BIT(13))
#define SDIO_SLC0_TOKEN0_INC_M  (SDIO_SLC0_TOKEN0_INC_V << SDIO_SLC0_TOKEN0_INC_S)
#define SDIO_SLC0_TOKEN0_INC_V  0x00000001U
#define SDIO_SLC0_TOKEN0_INC_S  13
/** SDIO_SLC0_TOKEN0_INC_MORE : WT; bitpos: [14]; default: 0;
 *  Set this bit to add the value of SDIO_SLC0TOKEN0_SLC0_TOKEN0_WDATA to
 *  SDIO_SLC0TOKEN0_SLC0_TOKEN0.
 */
#define SDIO_SLC0_TOKEN0_INC_MORE    (BIT(14))
#define SDIO_SLC0_TOKEN0_INC_MORE_M  (SDIO_SLC0_TOKEN0_INC_MORE_V << SDIO_SLC0_TOKEN0_INC_MORE_S)
#define SDIO_SLC0_TOKEN0_INC_MORE_V  0x00000001U
#define SDIO_SLC0_TOKEN0_INC_MORE_S  14
/** SDIO_SLC0_TOKEN0 : RO; bitpos: [27:16]; default: 0;
 *  The accumulated number of buffers for receiving packets through dma channel0.
 */
#define SDIO_SLC0_TOKEN0    0x00000FFFU
#define SDIO_SLC0_TOKEN0_M  (SDIO_SLC0_TOKEN0_V << SDIO_SLC0_TOKEN0_S)
#define SDIO_SLC0_TOKEN0_V  0x00000FFFU
#define SDIO_SLC0_TOKEN0_S  16

/** SDIO_SLC_SLC0TOKEN1_REG register
 *  Sdio slave DMA channel0 receiving buffer configuration and status register1.
 */
#define SDIO_SLC_SLC0TOKEN1_REG (DR_REG_SLC_BASE + 0x64)
/** SDIO_SLC0_TOKEN1_WDATA : WT; bitpos: [11:0]; default: 0;
 *  The number of available receiving buffers.
 */
#define SDIO_SLC0_TOKEN1_WDATA    0x00000FFFU
#define SDIO_SLC0_TOKEN1_WDATA_M  (SDIO_SLC0_TOKEN1_WDATA_V << SDIO_SLC0_TOKEN1_WDATA_S)
#define SDIO_SLC0_TOKEN1_WDATA_V  0x00000FFFU
#define SDIO_SLC0_TOKEN1_WDATA_S  0
/** SDIO_SLC0_TOKEN1_WR : WT; bitpos: [12]; default: 0;
 *  update slc0_token1_wdata into slc0 token1
 */
#define SDIO_SLC0_TOKEN1_WR    (BIT(12))
#define SDIO_SLC0_TOKEN1_WR_M  (SDIO_SLC0_TOKEN1_WR_V << SDIO_SLC0_TOKEN1_WR_S)
#define SDIO_SLC0_TOKEN1_WR_V  0x00000001U
#define SDIO_SLC0_TOKEN1_WR_S  12
/** SDIO_SLC0_TOKEN1_INC : WT; bitpos: [13]; default: 0;
 *  slc0_token1 becomes to 1 when auto clear slc0_token1, else add 1 to slc0_token1
 */
#define SDIO_SLC0_TOKEN1_INC    (BIT(13))
#define SDIO_SLC0_TOKEN1_INC_M  (SDIO_SLC0_TOKEN1_INC_V << SDIO_SLC0_TOKEN1_INC_S)
#define SDIO_SLC0_TOKEN1_INC_V  0x00000001U
#define SDIO_SLC0_TOKEN1_INC_S  13
/** SDIO_SLC0_TOKEN1_INC_MORE : WT; bitpos: [14]; default: 0;
 *  Set this bit to add the value of SDIO_SLC0TOKEN1_SLC0_TOKEN1_WDATA to
 *  SDIO_SLC0TOKEN1_SLC0_TOKEN1.
 */
#define SDIO_SLC0_TOKEN1_INC_MORE    (BIT(14))
#define SDIO_SLC0_TOKEN1_INC_MORE_M  (SDIO_SLC0_TOKEN1_INC_MORE_V << SDIO_SLC0_TOKEN1_INC_MORE_S)
#define SDIO_SLC0_TOKEN1_INC_MORE_V  0x00000001U
#define SDIO_SLC0_TOKEN1_INC_MORE_S  14
/** SDIO_SLC0_TOKEN1 : RO; bitpos: [27:16]; default: 0;
 *  The accumulated number of buffers for receiving packets through dma channel0.
 */
#define SDIO_SLC0_TOKEN1    0x00000FFFU
#define SDIO_SLC0_TOKEN1_M  (SDIO_SLC0_TOKEN1_V << SDIO_SLC0_TOKEN1_S)
#define SDIO_SLC0_TOKEN1_V  0x00000FFFU
#define SDIO_SLC0_TOKEN1_S  16

/** SDIO_SLC_SLC1TOKEN0_REG register
 *  Sdio slave DMA channel1 receiving buffer configuration and status register.
 */
#define SDIO_SLC_SLC1TOKEN0_REG (DR_REG_SLC_BASE + 0x68)
/** SDIO_SLC1_TOKEN0_WDATA : WT; bitpos: [11:0]; default: 0;
 *  The number of available receiving buffers.
 */
#define SDIO_SLC1_TOKEN0_WDATA    0x00000FFFU
#define SDIO_SLC1_TOKEN0_WDATA_M  (SDIO_SLC1_TOKEN0_WDATA_V << SDIO_SLC1_TOKEN0_WDATA_S)
#define SDIO_SLC1_TOKEN0_WDATA_V  0x00000FFFU
#define SDIO_SLC1_TOKEN0_WDATA_S  0
/** SDIO_SLC1_TOKEN0_WR : WT; bitpos: [12]; default: 0;
 *  update SLC1_token0_wdata into SLC1 token0.
 */
#define SDIO_SLC1_TOKEN0_WR    (BIT(12))
#define SDIO_SLC1_TOKEN0_WR_M  (SDIO_SLC1_TOKEN0_WR_V << SDIO_SLC1_TOKEN0_WR_S)
#define SDIO_SLC1_TOKEN0_WR_V  0x00000001U
#define SDIO_SLC1_TOKEN0_WR_S  12
/** SDIO_SLC1_TOKEN0_INC : WT; bitpos: [13]; default: 0;
 *  Add 1 to SLC1_token0.
 */
#define SDIO_SLC1_TOKEN0_INC    (BIT(13))
#define SDIO_SLC1_TOKEN0_INC_M  (SDIO_SLC1_TOKEN0_INC_V << SDIO_SLC1_TOKEN0_INC_S)
#define SDIO_SLC1_TOKEN0_INC_V  0x00000001U
#define SDIO_SLC1_TOKEN0_INC_S  13
/** SDIO_SLC1_TOKEN0_INC_MORE : WT; bitpos: [14]; default: 0;
 *  Set this bit to add the value of SDIO_SLC1TOKEN0_SLC1_TOKEN0_WDATA to
 *  SDIO_SLC1TOKEN0_SLC1_TOKEN0.
 */
#define SDIO_SLC1_TOKEN0_INC_MORE    (BIT(14))
#define SDIO_SLC1_TOKEN0_INC_MORE_M  (SDIO_SLC1_TOKEN0_INC_MORE_V << SDIO_SLC1_TOKEN0_INC_MORE_S)
#define SDIO_SLC1_TOKEN0_INC_MORE_V  0x00000001U
#define SDIO_SLC1_TOKEN0_INC_MORE_S  14
/** SDIO_SLC1_TOKEN0 : RO; bitpos: [27:16]; default: 0;
 *  The accumulated number of buffers for receiving packets through dma channel1.
 */
#define SDIO_SLC1_TOKEN0    0x00000FFFU
#define SDIO_SLC1_TOKEN0_M  (SDIO_SLC1_TOKEN0_V << SDIO_SLC1_TOKEN0_S)
#define SDIO_SLC1_TOKEN0_V  0x00000FFFU
#define SDIO_SLC1_TOKEN0_S  16

/** SDIO_SLC_SLC1TOKEN1_REG register
 *  Sdio slave DMA channel1 receiving buffer configuration and status register1.
 */
#define SDIO_SLC_SLC1TOKEN1_REG (DR_REG_SLC_BASE + 0x6c)
/** SDIO_SLC1_TOKEN1_WDATA : WT; bitpos: [11:0]; default: 0;
 *  The number of available receiving buffers.
 */
#define SDIO_SLC1_TOKEN1_WDATA    0x00000FFFU
#define SDIO_SLC1_TOKEN1_WDATA_M  (SDIO_SLC1_TOKEN1_WDATA_V << SDIO_SLC1_TOKEN1_WDATA_S)
#define SDIO_SLC1_TOKEN1_WDATA_V  0x00000FFFU
#define SDIO_SLC1_TOKEN1_WDATA_S  0
/** SDIO_SLC1_TOKEN1_WR : WT; bitpos: [12]; default: 0;
 *  update SLC1_token1_wdata into SLC1 token1
 */
#define SDIO_SLC1_TOKEN1_WR    (BIT(12))
#define SDIO_SLC1_TOKEN1_WR_M  (SDIO_SLC1_TOKEN1_WR_V << SDIO_SLC1_TOKEN1_WR_S)
#define SDIO_SLC1_TOKEN1_WR_V  0x00000001U
#define SDIO_SLC1_TOKEN1_WR_S  12
/** SDIO_SLC1_TOKEN1_INC : WT; bitpos: [13]; default: 0;
 *  SLC1_token1 becomes to 1 when auto clear SLC1_token1, else add 1 to SLC1_token1
 */
#define SDIO_SLC1_TOKEN1_INC    (BIT(13))
#define SDIO_SLC1_TOKEN1_INC_M  (SDIO_SLC1_TOKEN1_INC_V << SDIO_SLC1_TOKEN1_INC_S)
#define SDIO_SLC1_TOKEN1_INC_V  0x00000001U
#define SDIO_SLC1_TOKEN1_INC_S  13
/** SDIO_SLC1_TOKEN1_INC_MORE : WT; bitpos: [14]; default: 0;
 *  Set this bit to add the value of SDIO_SLC1TOKEN1_SLC1_TOKEN1_WDATA to
 *  SDIO_SLC1TOKEN1_SLC1_TOKEN1.
 */
#define SDIO_SLC1_TOKEN1_INC_MORE    (BIT(14))
#define SDIO_SLC1_TOKEN1_INC_MORE_M  (SDIO_SLC1_TOKEN1_INC_MORE_V << SDIO_SLC1_TOKEN1_INC_MORE_S)
#define SDIO_SLC1_TOKEN1_INC_MORE_V  0x00000001U
#define SDIO_SLC1_TOKEN1_INC_MORE_S  14
/** SDIO_SLC1_TOKEN1 : RO; bitpos: [27:16]; default: 0;
 *  The accumulated number of buffers for receiving packets through dma channel1.
 */
#define SDIO_SLC1_TOKEN1    0x00000FFFU
#define SDIO_SLC1_TOKEN1_M  (SDIO_SLC1_TOKEN1_V << SDIO_SLC1_TOKEN1_S)
#define SDIO_SLC1_TOKEN1_V  0x00000FFFU
#define SDIO_SLC1_TOKEN1_S  16

/** SDIO_SLC_CONF1_REG register
 *  Sdio slave DMA configuration1 register.
 */
#define SDIO_SLC_CONF1_REG (DR_REG_SLC_BASE + 0x70)
/** SDIO_SLC0_CHECK_OWNER : R/W; bitpos: [0]; default: 0;
 *  Set 1 to check the owner bit of descriptors  used by dma channel0.
 */
#define SDIO_SLC0_CHECK_OWNER    (BIT(0))
#define SDIO_SLC0_CHECK_OWNER_M  (SDIO_SLC0_CHECK_OWNER_V << SDIO_SLC0_CHECK_OWNER_S)
#define SDIO_SLC0_CHECK_OWNER_V  0x00000001U
#define SDIO_SLC0_CHECK_OWNER_S  0
/** SDIO_SLC0_TX_CHECK_SUM_EN : R/W; bitpos: [1]; default: 0;
 *  Set 1 to enable check sum function during slave receiving packets through dma
 *  channel0.
 */
#define SDIO_SLC0_TX_CHECK_SUM_EN    (BIT(1))
#define SDIO_SLC0_TX_CHECK_SUM_EN_M  (SDIO_SLC0_TX_CHECK_SUM_EN_V << SDIO_SLC0_TX_CHECK_SUM_EN_S)
#define SDIO_SLC0_TX_CHECK_SUM_EN_V  0x00000001U
#define SDIO_SLC0_TX_CHECK_SUM_EN_S  1
/** SDIO_SLC0_RX_CHECK_SUM_EN : R/W; bitpos: [2]; default: 0;
 *  Set 1 to enable check sum function during slave sending packets through dma
 *  channel0.
 */
#define SDIO_SLC0_RX_CHECK_SUM_EN    (BIT(2))
#define SDIO_SLC0_RX_CHECK_SUM_EN_M  (SDIO_SLC0_RX_CHECK_SUM_EN_V << SDIO_SLC0_RX_CHECK_SUM_EN_S)
#define SDIO_SLC0_RX_CHECK_SUM_EN_V  0x00000001U
#define SDIO_SLC0_RX_CHECK_SUM_EN_S  2
/** SDIO_SDIO_CMD_HOLD_EN : R/W; bitpos: [3]; default: 1;
 *  Allow host to send another cmd53 during last cmd53 operating time.
 */
#define SDIO_SDIO_CMD_HOLD_EN    (BIT(3))
#define SDIO_SDIO_CMD_HOLD_EN_M  (SDIO_SDIO_CMD_HOLD_EN_V << SDIO_SDIO_CMD_HOLD_EN_S)
#define SDIO_SDIO_CMD_HOLD_EN_V  0x00000001U
#define SDIO_SDIO_CMD_HOLD_EN_S  3
/** SDIO_SLC0_LEN_AUTO_CLR : R/W; bitpos: [4]; default: 1;
 *  Set 1 to automatically clear the length of buffers used to send packets ot host
 *  after host reads this length. (not recommended to use,should be set to 0)
 */
#define SDIO_SLC0_LEN_AUTO_CLR    (BIT(4))
#define SDIO_SLC0_LEN_AUTO_CLR_M  (SDIO_SLC0_LEN_AUTO_CLR_V << SDIO_SLC0_LEN_AUTO_CLR_S)
#define SDIO_SLC0_LEN_AUTO_CLR_V  0x00000001U
#define SDIO_SLC0_LEN_AUTO_CLR_S  4
/** SDIO_SLC0_TX_STITCH_EN : R/W; bitpos: [5]; default: 1;
 *  Set 1 to enable the stitch mode of dma channel0 receiving linked list .
 */
#define SDIO_SLC0_TX_STITCH_EN    (BIT(5))
#define SDIO_SLC0_TX_STITCH_EN_M  (SDIO_SLC0_TX_STITCH_EN_V << SDIO_SLC0_TX_STITCH_EN_S)
#define SDIO_SLC0_TX_STITCH_EN_V  0x00000001U
#define SDIO_SLC0_TX_STITCH_EN_S  5
/** SDIO_SLC0_RX_STITCH_EN : R/W; bitpos: [6]; default: 1;
 *  Set 1 to enable the stitch mode of dma channel0 sending linked list .
 */
#define SDIO_SLC0_RX_STITCH_EN    (BIT(6))
#define SDIO_SLC0_RX_STITCH_EN_M  (SDIO_SLC0_RX_STITCH_EN_V << SDIO_SLC0_RX_STITCH_EN_S)
#define SDIO_SLC0_RX_STITCH_EN_V  0x00000001U
#define SDIO_SLC0_RX_STITCH_EN_S  6
/** SDIO_SLC1_CHECK_OWNER : R/W; bitpos: [16]; default: 0;
 *  Set 1 to check the owner bit of descriptors  used by dma channel1.
 */
#define SDIO_SLC1_CHECK_OWNER    (BIT(16))
#define SDIO_SLC1_CHECK_OWNER_M  (SDIO_SLC1_CHECK_OWNER_V << SDIO_SLC1_CHECK_OWNER_S)
#define SDIO_SLC1_CHECK_OWNER_V  0x00000001U
#define SDIO_SLC1_CHECK_OWNER_S  16
/** SDIO_SLC1_TX_CHECK_SUM_EN : R/W; bitpos: [17]; default: 0;
 *  Set 1 to enable check sum function during slave receiving packets through dma
 *  channel1.
 */
#define SDIO_SLC1_TX_CHECK_SUM_EN    (BIT(17))
#define SDIO_SLC1_TX_CHECK_SUM_EN_M  (SDIO_SLC1_TX_CHECK_SUM_EN_V << SDIO_SLC1_TX_CHECK_SUM_EN_S)
#define SDIO_SLC1_TX_CHECK_SUM_EN_V  0x00000001U
#define SDIO_SLC1_TX_CHECK_SUM_EN_S  17
/** SDIO_SLC1_RX_CHECK_SUM_EN : R/W; bitpos: [18]; default: 0;
 *  Set 1 to enable check sum function during slave sending packets through dma
 *  channel1.
 */
#define SDIO_SLC1_RX_CHECK_SUM_EN    (BIT(18))
#define SDIO_SLC1_RX_CHECK_SUM_EN_M  (SDIO_SLC1_RX_CHECK_SUM_EN_V << SDIO_SLC1_RX_CHECK_SUM_EN_S)
#define SDIO_SLC1_RX_CHECK_SUM_EN_V  0x00000001U
#define SDIO_SLC1_RX_CHECK_SUM_EN_S  18
/** SDIO_HOST_INT_LEVEL_SEL : R/W; bitpos: [19]; default: 0;
 *  Set the polarity of interrupt generated to host: 0-low active, 1-high active.
 */
#define SDIO_HOST_INT_LEVEL_SEL    (BIT(19))
#define SDIO_HOST_INT_LEVEL_SEL_M  (SDIO_HOST_INT_LEVEL_SEL_V << SDIO_HOST_INT_LEVEL_SEL_S)
#define SDIO_HOST_INT_LEVEL_SEL_V  0x00000001U
#define SDIO_HOST_INT_LEVEL_SEL_S  19
/** SDIO_SDIO_CLK_EN : R/W; bitpos: [22]; default: 0;
 *  Sdio slave dma registers' clock gating enable bit: 0-the sdio slave dma registers'
 *  clock is on  during apb operation, 1- the sdio slave dma registers' clock is always
 *  on.
 */
#define SDIO_SDIO_CLK_EN    (BIT(22))
#define SDIO_SDIO_CLK_EN_M  (SDIO_SDIO_CLK_EN_V << SDIO_SDIO_CLK_EN_S)
#define SDIO_SDIO_CLK_EN_V  0x00000001U
#define SDIO_SDIO_CLK_EN_S  22

/** SDIO_SLC_SLC0_STATE0_REG register
 *  Sdio slave DMA channel0 status register.
 */
#define SDIO_SLC_SLC0_STATE0_REG (DR_REG_SLC_BASE + 0x74)
/** SDIO_SLC0_STATE0 : RO; bitpos: [31:0]; default: 0;
 *  [18:0] the current receiving descriptor address, [20:19] receiving descriptor fsm
 *  state, [23:21] receiving fsm state, [30:24] the count value of data received in
 *  txfifo
 */
#define SDIO_SLC0_STATE0    0xFFFFFFFFU
#define SDIO_SLC0_STATE0_M  (SDIO_SLC0_STATE0_V << SDIO_SLC0_STATE0_S)
#define SDIO_SLC0_STATE0_V  0xFFFFFFFFU
#define SDIO_SLC0_STATE0_S  0

/** SDIO_SLC_SLC0_STATE1_REG register
 *  Sdio slave DMA channel0 status1 register.
 */
#define SDIO_SLC_SLC0_STATE1_REG (DR_REG_SLC_BASE + 0x78)
/** SDIO_SLC0_STATE1 : RO; bitpos: [31:0]; default: 0;
 *  [18:0] the current sending descriptor address, [20:19] sending descriptor fsm
 *  state, [23:21] sending fsm state, [30:24] the count value of data to be sent in
 *  rxfifo
 */
#define SDIO_SLC0_STATE1    0xFFFFFFFFU
#define SDIO_SLC0_STATE1_M  (SDIO_SLC0_STATE1_V << SDIO_SLC0_STATE1_S)
#define SDIO_SLC0_STATE1_V  0xFFFFFFFFU
#define SDIO_SLC0_STATE1_S  0

/** SDIO_SLC_SLC1_STATE0_REG register
 *  Sdio slave DMA channel1 status register.
 */
#define SDIO_SLC_SLC1_STATE0_REG (DR_REG_SLC_BASE + 0x7c)
/** SDIO_SLC1_STATE0 : RO; bitpos: [31:0]; default: 0;
 *  [18:0] the current receiving descriptor address, [20:19] receiving descriptor fsm
 *  state, [23:21] receiving fsm state, [30:24] the count value of data received in
 *  txfifo
 */
#define SDIO_SLC1_STATE0    0xFFFFFFFFU
#define SDIO_SLC1_STATE0_M  (SDIO_SLC1_STATE0_V << SDIO_SLC1_STATE0_S)
#define SDIO_SLC1_STATE0_V  0xFFFFFFFFU
#define SDIO_SLC1_STATE0_S  0

/** SDIO_SLC_SLC1_STATE1_REG register
 *  Sdio slave DMA channel1 status1 register.
 */
#define SDIO_SLC_SLC1_STATE1_REG (DR_REG_SLC_BASE + 0x80)
/** SDIO_SLC1_STATE1 : RO; bitpos: [31:0]; default: 0;
 *  [18:0] the current sending descriptor address, [20:19] sending descriptor fsm
 *  state, [23:21] sending fsm state, [30:24] the count value of data to be sent in
 *  rxfifo
 */
#define SDIO_SLC1_STATE1    0xFFFFFFFFU
#define SDIO_SLC1_STATE1_M  (SDIO_SLC1_STATE1_V << SDIO_SLC1_STATE1_S)
#define SDIO_SLC1_STATE1_V  0xFFFFFFFFU
#define SDIO_SLC1_STATE1_S  0

/** SDIO_SLC_BRIDGE_CONF_REG register
 *  Sdio slave DMA bridge configuration register.
 */
#define SDIO_SLC_BRIDGE_CONF_REG (DR_REG_SLC_BASE + 0x84)
/** SDIO_SLC_TXEOF_ENA : R/W; bitpos: [5:0]; default: 32;
 *  Configure the end-of-frame address of receiving packet. The priority from high to
 *  low is: if txeof_ena[0] is 1, end-of-frame address is 0xfff,  if txeof_ena[1] is 1,
 *  end-of-frame address is 0x1fff, if txeof_ena[2] is 1, end-of-frame address is
 *  0x3fff, if txeof_ena[3] is 1, end-of-frame address is 0x7fff, if txeof_ena[4] is 1,
 *  end-of-frame address is 0xffff, if txeof_ena[5] is 1, end-of-frame address is
 *  0x1f7ff. Make sure the end-of-frame address is within the address range set by
 *  fifo_map_ena, otherwise the packet will be transferred without end-of-frame flag,
 *  which can cause unpredictable consequence.
 */
#define SDIO_SLC_TXEOF_ENA    0x0000003FU
#define SDIO_SLC_TXEOF_ENA_M  (SDIO_SLC_TXEOF_ENA_V << SDIO_SLC_TXEOF_ENA_S)
#define SDIO_SLC_TXEOF_ENA_V  0x0000003FU
#define SDIO_SLC_TXEOF_ENA_S  0
/** SDIO_SLC_FIFO_MAP_ENA : R/W; bitpos: [11:8]; default: 7;
 *  Configure the address range allocated to transfer packets using SDIO func1 and
 *  func2. The address range is from 0x400 to 0-0x3fff, 1-0x7fff, 3-0xffff, 7-0x1ffff,
 *  others-not allowed.
 */
#define SDIO_SLC_FIFO_MAP_ENA    0x0000000FU
#define SDIO_SLC_FIFO_MAP_ENA_M  (SDIO_SLC_FIFO_MAP_ENA_V << SDIO_SLC_FIFO_MAP_ENA_S)
#define SDIO_SLC_FIFO_MAP_ENA_V  0x0000000FU
#define SDIO_SLC_FIFO_MAP_ENA_S  8
/** SDIO_SLC0_TX_DUMMY_MODE : R/W; bitpos: [12]; default: 1;
 *  Set 1 not to transfer check sum data into memory through dma channel0.
 */
#define SDIO_SLC0_TX_DUMMY_MODE    (BIT(12))
#define SDIO_SLC0_TX_DUMMY_MODE_M  (SDIO_SLC0_TX_DUMMY_MODE_V << SDIO_SLC0_TX_DUMMY_MODE_S)
#define SDIO_SLC0_TX_DUMMY_MODE_V  0x00000001U
#define SDIO_SLC0_TX_DUMMY_MODE_S  12
/** SDIO_SLC_HDA_MAP_128K : R/W; bitpos: [13]; default: 1;
 *  If hda_map_128k is 0, map high 4 bit address to hda_4msb. When hda_map_128k is 1,
 *  if hda_word_en (slchost_conf12[0]) is 0, map high 4 bit address to {hda_4msb[3:1],
 *  addr_ext_func[16]}. Else if hda_word_en is 1, map high 4 bit address to
 *  {hda_4msb[3:1], addr_ext_func[14]}.
 */
#define SDIO_SLC_HDA_MAP_128K    (BIT(13))
#define SDIO_SLC_HDA_MAP_128K_M  (SDIO_SLC_HDA_MAP_128K_V << SDIO_SLC_HDA_MAP_128K_S)
#define SDIO_SLC_HDA_MAP_128K_V  0x00000001U
#define SDIO_SLC_HDA_MAP_128K_S  13
/** SDIO_SLC1_TX_DUMMY_MODE : R/W; bitpos: [14]; default: 1;
 *  Set 1 not to transfer check sum data into memory through dma channel1.
 */
#define SDIO_SLC1_TX_DUMMY_MODE    (BIT(14))
#define SDIO_SLC1_TX_DUMMY_MODE_M  (SDIO_SLC1_TX_DUMMY_MODE_V << SDIO_SLC1_TX_DUMMY_MODE_S)
#define SDIO_SLC1_TX_DUMMY_MODE_V  0x00000001U
#define SDIO_SLC1_TX_DUMMY_MODE_S  14
/** SDIO_SLC_TX_PUSH_IDLE_NUM : R/W; bitpos: [31:16]; default: 10;
 *  reserved
 */
#define SDIO_SLC_TX_PUSH_IDLE_NUM    0x0000FFFFU
#define SDIO_SLC_TX_PUSH_IDLE_NUM_M  (SDIO_SLC_TX_PUSH_IDLE_NUM_V << SDIO_SLC_TX_PUSH_IDLE_NUM_S)
#define SDIO_SLC_TX_PUSH_IDLE_NUM_V  0x0000FFFFU
#define SDIO_SLC_TX_PUSH_IDLE_NUM_S  16

/** SDIO_SLC_SLC0_TO_EOF_DES_ADDR_REG register
 *  Sdio slave DMA channel0 start address of sending linked list register.
 */
#define SDIO_SLC_SLC0_TO_EOF_DES_ADDR_REG (DR_REG_SLC_BASE + 0x88)
/** SDIO_SLC0_TO_EOF_DES_ADDR : RO; bitpos: [31:0]; default: 0;
 *  The descriptor address of the last sent packet through dma channel0.
 */
#define SDIO_SLC0_TO_EOF_DES_ADDR    0xFFFFFFFFU
#define SDIO_SLC0_TO_EOF_DES_ADDR_M  (SDIO_SLC0_TO_EOF_DES_ADDR_V << SDIO_SLC0_TO_EOF_DES_ADDR_S)
#define SDIO_SLC0_TO_EOF_DES_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC0_TO_EOF_DES_ADDR_S  0

/** SDIO_SLC_SLC0_TX_EOF_DES_ADDR_REG register
 *  Sdio slave DMA channel0 receiving packet status register.
 */
#define SDIO_SLC_SLC0_TX_EOF_DES_ADDR_REG (DR_REG_SLC_BASE + 0x8c)
/** SDIO_SLC0_TX_SUC_EOF_DES_ADDR : RO; bitpos: [31:0]; default: 0;
 *  The descriptor address of the last successfully received packet through dma
 *  channel0.
 */
#define SDIO_SLC0_TX_SUC_EOF_DES_ADDR    0xFFFFFFFFU
#define SDIO_SLC0_TX_SUC_EOF_DES_ADDR_M  (SDIO_SLC0_TX_SUC_EOF_DES_ADDR_V << SDIO_SLC0_TX_SUC_EOF_DES_ADDR_S)
#define SDIO_SLC0_TX_SUC_EOF_DES_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC0_TX_SUC_EOF_DES_ADDR_S  0

/** SDIO_SLC_SLC0_TO_EOF_BFR_DES_ADDR_REG register
 *  Sdio slave DMA channel0 start address of sending linked list register.
 */
#define SDIO_SLC_SLC0_TO_EOF_BFR_DES_ADDR_REG (DR_REG_SLC_BASE + 0x90)
/** SDIO_SLC0_TO_EOF_BFR_DES_ADDR : RO; bitpos: [31:0]; default: 0;
 *  The descriptor address of the last successfully sent buffer packet through dma
 *  channel0.
 */
#define SDIO_SLC0_TO_EOF_BFR_DES_ADDR    0xFFFFFFFFU
#define SDIO_SLC0_TO_EOF_BFR_DES_ADDR_M  (SDIO_SLC0_TO_EOF_BFR_DES_ADDR_V << SDIO_SLC0_TO_EOF_BFR_DES_ADDR_S)
#define SDIO_SLC0_TO_EOF_BFR_DES_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC0_TO_EOF_BFR_DES_ADDR_S  0

/** SDIO_SLC_SLC1_TO_EOF_DES_ADDR_REG register
 *  Sdio slave DMA channel1 start address of sending linked list register.
 */
#define SDIO_SLC_SLC1_TO_EOF_DES_ADDR_REG (DR_REG_SLC_BASE + 0x94)
/** SDIO_SLC1_TO_EOF_DES_ADDR : RO; bitpos: [31:0]; default: 0;
 *  The descriptor address of the last sent packet through dma channel1.
 */
#define SDIO_SLC1_TO_EOF_DES_ADDR    0xFFFFFFFFU
#define SDIO_SLC1_TO_EOF_DES_ADDR_M  (SDIO_SLC1_TO_EOF_DES_ADDR_V << SDIO_SLC1_TO_EOF_DES_ADDR_S)
#define SDIO_SLC1_TO_EOF_DES_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC1_TO_EOF_DES_ADDR_S  0

/** SDIO_SLC_SLC1_TX_EOF_DES_ADDR_REG register
 *  Sdio slave DMA channel1 receiving packet status register.
 */
#define SDIO_SLC_SLC1_TX_EOF_DES_ADDR_REG (DR_REG_SLC_BASE + 0x98)
/** SDIO_SLC1_TX_SUC_EOF_DES_ADDR : RO; bitpos: [31:0]; default: 0;
 *  The descriptor address of the last successfully received packet through dma
 *  channel1.
 */
#define SDIO_SLC1_TX_SUC_EOF_DES_ADDR    0xFFFFFFFFU
#define SDIO_SLC1_TX_SUC_EOF_DES_ADDR_M  (SDIO_SLC1_TX_SUC_EOF_DES_ADDR_V << SDIO_SLC1_TX_SUC_EOF_DES_ADDR_S)
#define SDIO_SLC1_TX_SUC_EOF_DES_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC1_TX_SUC_EOF_DES_ADDR_S  0

/** SDIO_SLC_SLC1_TO_EOF_BFR_DES_ADDR_REG register
 *  Sdio slave DMA channel1 start address of sending linked list register.
 */
#define SDIO_SLC_SLC1_TO_EOF_BFR_DES_ADDR_REG (DR_REG_SLC_BASE + 0x9c)
/** SDIO_SLC1_TO_EOF_BFR_DES_ADDR : RO; bitpos: [31:0]; default: 0;
 *  The descriptor address of the last successfully sent buffer packet through dma
 *  channel1.
 */
#define SDIO_SLC1_TO_EOF_BFR_DES_ADDR    0xFFFFFFFFU
#define SDIO_SLC1_TO_EOF_BFR_DES_ADDR_M  (SDIO_SLC1_TO_EOF_BFR_DES_ADDR_V << SDIO_SLC1_TO_EOF_BFR_DES_ADDR_S)
#define SDIO_SLC1_TO_EOF_BFR_DES_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC1_TO_EOF_BFR_DES_ADDR_S  0

/** SDIO_SLC_AHB_TEST_REG register
 *  Sdio slave AHB test register.
 */
#define SDIO_SLC_AHB_TEST_REG (DR_REG_SLC_BASE + 0xa0)
/** SDIO_SLC_AHB_TESTMODE : R/W; bitpos: [2:0]; default: 0;
 *  set bit2 to 1 to enable sdio slave ahb test.
 */
#define SDIO_SLC_AHB_TESTMODE    0x00000007U
#define SDIO_SLC_AHB_TESTMODE_M  (SDIO_SLC_AHB_TESTMODE_V << SDIO_SLC_AHB_TESTMODE_S)
#define SDIO_SLC_AHB_TESTMODE_V  0x00000007U
#define SDIO_SLC_AHB_TESTMODE_S  0
/** SDIO_SLC_AHB_TESTADDR : R/W; bitpos: [5:4]; default: 0;
 *  Configure AHB test address.
 */
#define SDIO_SLC_AHB_TESTADDR    0x00000003U
#define SDIO_SLC_AHB_TESTADDR_M  (SDIO_SLC_AHB_TESTADDR_V << SDIO_SLC_AHB_TESTADDR_S)
#define SDIO_SLC_AHB_TESTADDR_V  0x00000003U
#define SDIO_SLC_AHB_TESTADDR_S  4

/** SDIO_SLC_SDIO_ST_REG register
 *  Sdio slave function status register.
 */
#define SDIO_SLC_SDIO_ST_REG (DR_REG_SLC_BASE + 0xa4)
/** SDIO_CMD_ST : RO; bitpos: [2:0]; default: 0;
 *  The sdio command fsm state.
 */
#define SDIO_CMD_ST    0x00000007U
#define SDIO_CMD_ST_M  (SDIO_CMD_ST_V << SDIO_CMD_ST_S)
#define SDIO_CMD_ST_V  0x00000007U
#define SDIO_CMD_ST_S  0
/** SDIO_FUNC_ST : RO; bitpos: [7:4]; default: 0;
 *  The sdio cmd52/53 function fsm state.
 */
#define SDIO_FUNC_ST    0x0000000FU
#define SDIO_FUNC_ST_M  (SDIO_FUNC_ST_V << SDIO_FUNC_ST_S)
#define SDIO_FUNC_ST_V  0x0000000FU
#define SDIO_FUNC_ST_S  4
/** SDIO_SDIO_WAKEUP : RO; bitpos: [8]; default: 0;
 *  reserved
 */
#define SDIO_SDIO_WAKEUP    (BIT(8))
#define SDIO_SDIO_WAKEUP_M  (SDIO_SDIO_WAKEUP_V << SDIO_SDIO_WAKEUP_S)
#define SDIO_SDIO_WAKEUP_V  0x00000001U
#define SDIO_SDIO_WAKEUP_S  8
/** SDIO_BUS_ST : RO; bitpos: [14:12]; default: 0;
 *  The sdio bus fsm state.
 */
#define SDIO_BUS_ST    0x00000007U
#define SDIO_BUS_ST_M  (SDIO_BUS_ST_V << SDIO_BUS_ST_S)
#define SDIO_BUS_ST_V  0x00000007U
#define SDIO_BUS_ST_S  12
/** SDIO_FUNC1_ACC_STATE : RO; bitpos: [20:16]; default: 0;
 *  The sdio func1 fsm state.
 */
#define SDIO_FUNC1_ACC_STATE    0x0000001FU
#define SDIO_FUNC1_ACC_STATE_M  (SDIO_FUNC1_ACC_STATE_V << SDIO_FUNC1_ACC_STATE_S)
#define SDIO_FUNC1_ACC_STATE_V  0x0000001FU
#define SDIO_FUNC1_ACC_STATE_S  16
/** SDIO_FUNC2_ACC_STATE : RO; bitpos: [28:24]; default: 0;
 *  The sdio func2 fsm state.
 */
#define SDIO_FUNC2_ACC_STATE    0x0000001FU
#define SDIO_FUNC2_ACC_STATE_M  (SDIO_FUNC2_ACC_STATE_V << SDIO_FUNC2_ACC_STATE_S)
#define SDIO_FUNC2_ACC_STATE_V  0x0000001FU
#define SDIO_FUNC2_ACC_STATE_S  24

/** SDIO_SLC_RX_DSCR_CONF_REG register
 *  Sdio slave sending descriptor configuration register.
 */
#define SDIO_SLC_RX_DSCR_CONF_REG (DR_REG_SLC_BASE + 0xa8)
/** SDIO_SLC0_TOKEN_NO_REPLACE : R/W; bitpos: [0]; default: 0;
 *  reserved
 */
#define SDIO_SLC0_TOKEN_NO_REPLACE    (BIT(0))
#define SDIO_SLC0_TOKEN_NO_REPLACE_M  (SDIO_SLC0_TOKEN_NO_REPLACE_V << SDIO_SLC0_TOKEN_NO_REPLACE_S)
#define SDIO_SLC0_TOKEN_NO_REPLACE_V  0x00000001U
#define SDIO_SLC0_TOKEN_NO_REPLACE_S  0
/** SDIO_SLC0_INFOR_NO_REPLACE : R/W; bitpos: [1]; default: 1;
 *  reserved
 */
#define SDIO_SLC0_INFOR_NO_REPLACE    (BIT(1))
#define SDIO_SLC0_INFOR_NO_REPLACE_M  (SDIO_SLC0_INFOR_NO_REPLACE_V << SDIO_SLC0_INFOR_NO_REPLACE_S)
#define SDIO_SLC0_INFOR_NO_REPLACE_V  0x00000001U
#define SDIO_SLC0_INFOR_NO_REPLACE_S  1
/** SDIO_SLC0_RX_FILL_MODE : R/W; bitpos: [2]; default: 0;
 *  slc0 rx pop end control: 0-automatically end when pop finish, 1- end when the next
 *  pop doesn't occur after 255 cycles since the current pop
 */
#define SDIO_SLC0_RX_FILL_MODE    (BIT(2))
#define SDIO_SLC0_RX_FILL_MODE_M  (SDIO_SLC0_RX_FILL_MODE_V << SDIO_SLC0_RX_FILL_MODE_S)
#define SDIO_SLC0_RX_FILL_MODE_V  0x00000001U
#define SDIO_SLC0_RX_FILL_MODE_S  2
/** SDIO_SLC0_RX_EOF_MODE : R/W; bitpos: [3]; default: 1;
 *  0-slc0 rx_push_eof, 1-slc0 rx_pop_eof
 */
#define SDIO_SLC0_RX_EOF_MODE    (BIT(3))
#define SDIO_SLC0_RX_EOF_MODE_M  (SDIO_SLC0_RX_EOF_MODE_V << SDIO_SLC0_RX_EOF_MODE_S)
#define SDIO_SLC0_RX_EOF_MODE_V  0x00000001U
#define SDIO_SLC0_RX_EOF_MODE_S  3
/** SDIO_SLC0_RX_FILL_EN : R/W; bitpos: [4]; default: 1;
 *  reserved
 */
#define SDIO_SLC0_RX_FILL_EN    (BIT(4))
#define SDIO_SLC0_RX_FILL_EN_M  (SDIO_SLC0_RX_FILL_EN_V << SDIO_SLC0_RX_FILL_EN_S)
#define SDIO_SLC0_RX_FILL_EN_V  0x00000001U
#define SDIO_SLC0_RX_FILL_EN_S  4
/** SDIO_SLC0_RD_RETRY_THRESHOLD : R/W; bitpos: [15:5]; default: 128;
 *  reserved
 */
#define SDIO_SLC0_RD_RETRY_THRESHOLD    0x000007FFU
#define SDIO_SLC0_RD_RETRY_THRESHOLD_M  (SDIO_SLC0_RD_RETRY_THRESHOLD_V << SDIO_SLC0_RD_RETRY_THRESHOLD_S)
#define SDIO_SLC0_RD_RETRY_THRESHOLD_V  0x000007FFU
#define SDIO_SLC0_RD_RETRY_THRESHOLD_S  5
/** SDIO_SLC1_TOKEN_NO_REPLACE : R/W; bitpos: [16]; default: 1;
 *  reserved
 */
#define SDIO_SLC1_TOKEN_NO_REPLACE    (BIT(16))
#define SDIO_SLC1_TOKEN_NO_REPLACE_M  (SDIO_SLC1_TOKEN_NO_REPLACE_V << SDIO_SLC1_TOKEN_NO_REPLACE_S)
#define SDIO_SLC1_TOKEN_NO_REPLACE_V  0x00000001U
#define SDIO_SLC1_TOKEN_NO_REPLACE_S  16
/** SDIO_SLC1_INFOR_NO_REPLACE : R/W; bitpos: [17]; default: 1;
 *  reserved
 */
#define SDIO_SLC1_INFOR_NO_REPLACE    (BIT(17))
#define SDIO_SLC1_INFOR_NO_REPLACE_M  (SDIO_SLC1_INFOR_NO_REPLACE_V << SDIO_SLC1_INFOR_NO_REPLACE_S)
#define SDIO_SLC1_INFOR_NO_REPLACE_V  0x00000001U
#define SDIO_SLC1_INFOR_NO_REPLACE_S  17
/** SDIO_SLC1_RX_FILL_MODE : R/W; bitpos: [18]; default: 0;
 *  slc1 rx pop end control: 0-automatically end when pop finish, 1- end when the next
 *  pop doesn't occur after 255 cycles since the current pop
 */
#define SDIO_SLC1_RX_FILL_MODE    (BIT(18))
#define SDIO_SLC1_RX_FILL_MODE_M  (SDIO_SLC1_RX_FILL_MODE_V << SDIO_SLC1_RX_FILL_MODE_S)
#define SDIO_SLC1_RX_FILL_MODE_V  0x00000001U
#define SDIO_SLC1_RX_FILL_MODE_S  18
/** SDIO_SLC1_RX_EOF_MODE : R/W; bitpos: [19]; default: 1;
 *  0-slc1 rx_push_eof, 1-slc1 rx_pop_eof
 */
#define SDIO_SLC1_RX_EOF_MODE    (BIT(19))
#define SDIO_SLC1_RX_EOF_MODE_M  (SDIO_SLC1_RX_EOF_MODE_V << SDIO_SLC1_RX_EOF_MODE_S)
#define SDIO_SLC1_RX_EOF_MODE_V  0x00000001U
#define SDIO_SLC1_RX_EOF_MODE_S  19
/** SDIO_SLC1_RX_FILL_EN : R/W; bitpos: [20]; default: 1;
 *  reserved
 */
#define SDIO_SLC1_RX_FILL_EN    (BIT(20))
#define SDIO_SLC1_RX_FILL_EN_M  (SDIO_SLC1_RX_FILL_EN_V << SDIO_SLC1_RX_FILL_EN_S)
#define SDIO_SLC1_RX_FILL_EN_V  0x00000001U
#define SDIO_SLC1_RX_FILL_EN_S  20
/** SDIO_SLC1_RD_RETRY_THRESHOLD : R/W; bitpos: [31:21]; default: 128;
 *  reserved
 */
#define SDIO_SLC1_RD_RETRY_THRESHOLD    0x000007FFU
#define SDIO_SLC1_RD_RETRY_THRESHOLD_M  (SDIO_SLC1_RD_RETRY_THRESHOLD_V << SDIO_SLC1_RD_RETRY_THRESHOLD_S)
#define SDIO_SLC1_RD_RETRY_THRESHOLD_V  0x000007FFU
#define SDIO_SLC1_RD_RETRY_THRESHOLD_S  21

/** SDIO_SLC_SLC0_TXLINK_DSCR_REG register
 *  ******* Description ***********
 */
#define SDIO_SLC_SLC0_TXLINK_DSCR_REG (DR_REG_SLC_BASE + 0xac)
/** SDIO_SLC0_TXLINK_DSCR : RO; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define SDIO_SLC0_TXLINK_DSCR    0xFFFFFFFFU
#define SDIO_SLC0_TXLINK_DSCR_M  (SDIO_SLC0_TXLINK_DSCR_V << SDIO_SLC0_TXLINK_DSCR_S)
#define SDIO_SLC0_TXLINK_DSCR_V  0xFFFFFFFFU
#define SDIO_SLC0_TXLINK_DSCR_S  0

/** SDIO_SLC_SLC0_TXLINK_DSCR_BF0_REG register
 *  ******* Description ***********
 */
#define SDIO_SLC_SLC0_TXLINK_DSCR_BF0_REG (DR_REG_SLC_BASE + 0xb0)
/** SDIO_SLC0_TXLINK_DSCR_BF0 : RO; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define SDIO_SLC0_TXLINK_DSCR_BF0    0xFFFFFFFFU
#define SDIO_SLC0_TXLINK_DSCR_BF0_M  (SDIO_SLC0_TXLINK_DSCR_BF0_V << SDIO_SLC0_TXLINK_DSCR_BF0_S)
#define SDIO_SLC0_TXLINK_DSCR_BF0_V  0xFFFFFFFFU
#define SDIO_SLC0_TXLINK_DSCR_BF0_S  0

/** SDIO_SLC_SLC0_TXLINK_DSCR_BF1_REG register
 *  reserved
 */
#define SDIO_SLC_SLC0_TXLINK_DSCR_BF1_REG (DR_REG_SLC_BASE + 0xb4)
/** SDIO_SLC0_TXLINK_DSCR_BF1 : RO; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define SDIO_SLC0_TXLINK_DSCR_BF1    0xFFFFFFFFU
#define SDIO_SLC0_TXLINK_DSCR_BF1_M  (SDIO_SLC0_TXLINK_DSCR_BF1_V << SDIO_SLC0_TXLINK_DSCR_BF1_S)
#define SDIO_SLC0_TXLINK_DSCR_BF1_V  0xFFFFFFFFU
#define SDIO_SLC0_TXLINK_DSCR_BF1_S  0

/** SDIO_SLC_SLC0_RXLINK_DSCR_REG register
 *  ******* Description ***********
 */
#define SDIO_SLC_SLC0_RXLINK_DSCR_REG (DR_REG_SLC_BASE + 0xb8)
/** SDIO_SLC0_RXLINK_DSCR : RO; bitpos: [31:0]; default: 0;
 *  the third word of slc0 link descriptor, or known as the next descriptor address
 */
#define SDIO_SLC0_RXLINK_DSCR    0xFFFFFFFFU
#define SDIO_SLC0_RXLINK_DSCR_M  (SDIO_SLC0_RXLINK_DSCR_V << SDIO_SLC0_RXLINK_DSCR_S)
#define SDIO_SLC0_RXLINK_DSCR_V  0xFFFFFFFFU
#define SDIO_SLC0_RXLINK_DSCR_S  0

/** SDIO_SLC_SLC0_RXLINK_DSCR_BF0_REG register
 *  ******* Description ***********
 */
#define SDIO_SLC_SLC0_RXLINK_DSCR_BF0_REG (DR_REG_SLC_BASE + 0xbc)
/** SDIO_SLC0_RXLINK_DSCR_BF0 : RO; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define SDIO_SLC0_RXLINK_DSCR_BF0    0xFFFFFFFFU
#define SDIO_SLC0_RXLINK_DSCR_BF0_M  (SDIO_SLC0_RXLINK_DSCR_BF0_V << SDIO_SLC0_RXLINK_DSCR_BF0_S)
#define SDIO_SLC0_RXLINK_DSCR_BF0_V  0xFFFFFFFFU
#define SDIO_SLC0_RXLINK_DSCR_BF0_S  0

/** SDIO_SLC_SLC0_RXLINK_DSCR_BF1_REG register
 *  reserved
 */
#define SDIO_SLC_SLC0_RXLINK_DSCR_BF1_REG (DR_REG_SLC_BASE + 0xc0)
/** SDIO_SLC0_RXLINK_DSCR_BF1 : RO; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define SDIO_SLC0_RXLINK_DSCR_BF1    0xFFFFFFFFU
#define SDIO_SLC0_RXLINK_DSCR_BF1_M  (SDIO_SLC0_RXLINK_DSCR_BF1_V << SDIO_SLC0_RXLINK_DSCR_BF1_S)
#define SDIO_SLC0_RXLINK_DSCR_BF1_V  0xFFFFFFFFU
#define SDIO_SLC0_RXLINK_DSCR_BF1_S  0

/** SDIO_SLC_SLC1_TXLINK_DSCR_REG register
 *  reserved
 */
#define SDIO_SLC_SLC1_TXLINK_DSCR_REG (DR_REG_SLC_BASE + 0xc4)
/** SDIO_SLC1_TXLINK_DSCR : RO; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define SDIO_SLC1_TXLINK_DSCR    0xFFFFFFFFU
#define SDIO_SLC1_TXLINK_DSCR_M  (SDIO_SLC1_TXLINK_DSCR_V << SDIO_SLC1_TXLINK_DSCR_S)
#define SDIO_SLC1_TXLINK_DSCR_V  0xFFFFFFFFU
#define SDIO_SLC1_TXLINK_DSCR_S  0

/** SDIO_SLC_SLC1_TXLINK_DSCR_BF0_REG register
 *  reserved
 */
#define SDIO_SLC_SLC1_TXLINK_DSCR_BF0_REG (DR_REG_SLC_BASE + 0xc8)
/** SDIO_SLC1_TXLINK_DSCR_BF0 : RO; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define SDIO_SLC1_TXLINK_DSCR_BF0    0xFFFFFFFFU
#define SDIO_SLC1_TXLINK_DSCR_BF0_M  (SDIO_SLC1_TXLINK_DSCR_BF0_V << SDIO_SLC1_TXLINK_DSCR_BF0_S)
#define SDIO_SLC1_TXLINK_DSCR_BF0_V  0xFFFFFFFFU
#define SDIO_SLC1_TXLINK_DSCR_BF0_S  0

/** SDIO_SLC_SLC1_TXLINK_DSCR_BF1_REG register
 *  reserved
 */
#define SDIO_SLC_SLC1_TXLINK_DSCR_BF1_REG (DR_REG_SLC_BASE + 0xcc)
/** SDIO_SLC1_TXLINK_DSCR_BF1 : RO; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define SDIO_SLC1_TXLINK_DSCR_BF1    0xFFFFFFFFU
#define SDIO_SLC1_TXLINK_DSCR_BF1_M  (SDIO_SLC1_TXLINK_DSCR_BF1_V << SDIO_SLC1_TXLINK_DSCR_BF1_S)
#define SDIO_SLC1_TXLINK_DSCR_BF1_V  0xFFFFFFFFU
#define SDIO_SLC1_TXLINK_DSCR_BF1_S  0

/** SDIO_SLC_SLC1_RXLINK_DSCR_REG register
 *  ******* Description ***********
 */
#define SDIO_SLC_SLC1_RXLINK_DSCR_REG (DR_REG_SLC_BASE + 0xd0)
/** SDIO_SLC1_RXLINK_DSCR : RO; bitpos: [31:0]; default: 0;
 *  the third word of slc1 link descriptor, or known as the next descriptor address
 */
#define SDIO_SLC1_RXLINK_DSCR    0xFFFFFFFFU
#define SDIO_SLC1_RXLINK_DSCR_M  (SDIO_SLC1_RXLINK_DSCR_V << SDIO_SLC1_RXLINK_DSCR_S)
#define SDIO_SLC1_RXLINK_DSCR_V  0xFFFFFFFFU
#define SDIO_SLC1_RXLINK_DSCR_S  0

/** SDIO_SLC_SLC1_RXLINK_DSCR_BF0_REG register
 *  ******* Description ***********
 */
#define SDIO_SLC_SLC1_RXLINK_DSCR_BF0_REG (DR_REG_SLC_BASE + 0xd4)
/** SDIO_SLC1_RXLINK_DSCR_BF0 : RO; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define SDIO_SLC1_RXLINK_DSCR_BF0    0xFFFFFFFFU
#define SDIO_SLC1_RXLINK_DSCR_BF0_M  (SDIO_SLC1_RXLINK_DSCR_BF0_V << SDIO_SLC1_RXLINK_DSCR_BF0_S)
#define SDIO_SLC1_RXLINK_DSCR_BF0_V  0xFFFFFFFFU
#define SDIO_SLC1_RXLINK_DSCR_BF0_S  0

/** SDIO_SLC_SLC1_RXLINK_DSCR_BF1_REG register
 *  reserved
 */
#define SDIO_SLC_SLC1_RXLINK_DSCR_BF1_REG (DR_REG_SLC_BASE + 0xd8)
/** SDIO_SLC1_RXLINK_DSCR_BF1 : RO; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define SDIO_SLC1_RXLINK_DSCR_BF1    0xFFFFFFFFU
#define SDIO_SLC1_RXLINK_DSCR_BF1_M  (SDIO_SLC1_RXLINK_DSCR_BF1_V << SDIO_SLC1_RXLINK_DSCR_BF1_S)
#define SDIO_SLC1_RXLINK_DSCR_BF1_V  0xFFFFFFFFU
#define SDIO_SLC1_RXLINK_DSCR_BF1_S  0

/** SDIO_SLC_SLC0_TX_ERREOF_DES_ADDR_REG register
 *  reserved
 */
#define SDIO_SLC_SLC0_TX_ERREOF_DES_ADDR_REG (DR_REG_SLC_BASE + 0xdc)
/** SDIO_SLC0_TX_ERR_EOF_DES_ADDR : RO; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define SDIO_SLC0_TX_ERR_EOF_DES_ADDR    0xFFFFFFFFU
#define SDIO_SLC0_TX_ERR_EOF_DES_ADDR_M  (SDIO_SLC0_TX_ERR_EOF_DES_ADDR_V << SDIO_SLC0_TX_ERR_EOF_DES_ADDR_S)
#define SDIO_SLC0_TX_ERR_EOF_DES_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC0_TX_ERR_EOF_DES_ADDR_S  0

/** SDIO_SLC_SLC1_TX_ERREOF_DES_ADDR_REG register
 *  reserved
 */
#define SDIO_SLC_SLC1_TX_ERREOF_DES_ADDR_REG (DR_REG_SLC_BASE + 0xe0)
/** SDIO_SLC1_TX_ERR_EOF_DES_ADDR : RO; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define SDIO_SLC1_TX_ERR_EOF_DES_ADDR    0xFFFFFFFFU
#define SDIO_SLC1_TX_ERR_EOF_DES_ADDR_M  (SDIO_SLC1_TX_ERR_EOF_DES_ADDR_V << SDIO_SLC1_TX_ERR_EOF_DES_ADDR_S)
#define SDIO_SLC1_TX_ERR_EOF_DES_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC1_TX_ERR_EOF_DES_ADDR_S  0

/** SDIO_SLC_TOKEN_LAT_REG register
 *  reserved
 */
#define SDIO_SLC_TOKEN_LAT_REG (DR_REG_SLC_BASE + 0xe4)
/** SDIO_SLC0_TOKEN : RO; bitpos: [11:0]; default: 0;
 *  reserved
 */
#define SDIO_SLC0_TOKEN    0x00000FFFU
#define SDIO_SLC0_TOKEN_M  (SDIO_SLC0_TOKEN_V << SDIO_SLC0_TOKEN_S)
#define SDIO_SLC0_TOKEN_V  0x00000FFFU
#define SDIO_SLC0_TOKEN_S  0
/** SDIO_SLC1_TOKEN : RO; bitpos: [27:16]; default: 0;
 *  reserved
 */
#define SDIO_SLC1_TOKEN    0x00000FFFU
#define SDIO_SLC1_TOKEN_M  (SDIO_SLC1_TOKEN_V << SDIO_SLC1_TOKEN_S)
#define SDIO_SLC1_TOKEN_V  0x00000FFFU
#define SDIO_SLC1_TOKEN_S  16

/** SDIO_SLC_TX_DSCR_CONF_REG register
 *  reserved
 */
#define SDIO_SLC_TX_DSCR_CONF_REG (DR_REG_SLC_BASE + 0xe8)
/** SDIO_SLC_WR_RETRY_THRESHOLD : R/W; bitpos: [10:0]; default: 128;
 *  reserved
 */
#define SDIO_SLC_WR_RETRY_THRESHOLD    0x000007FFU
#define SDIO_SLC_WR_RETRY_THRESHOLD_M  (SDIO_SLC_WR_RETRY_THRESHOLD_V << SDIO_SLC_WR_RETRY_THRESHOLD_S)
#define SDIO_SLC_WR_RETRY_THRESHOLD_V  0x000007FFU
#define SDIO_SLC_WR_RETRY_THRESHOLD_S  0

/** SDIO_SLC_CMD_INFOR0_REG register
 *  reserved
 */
#define SDIO_SLC_CMD_INFOR0_REG (DR_REG_SLC_BASE + 0xec)
/** SDIO_CMD_CONTENT0 : RO; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define SDIO_CMD_CONTENT0    0xFFFFFFFFU
#define SDIO_CMD_CONTENT0_M  (SDIO_CMD_CONTENT0_V << SDIO_CMD_CONTENT0_S)
#define SDIO_CMD_CONTENT0_V  0xFFFFFFFFU
#define SDIO_CMD_CONTENT0_S  0

/** SDIO_SLC_CMD_INFOR1_REG register
 *  reserved
 */
#define SDIO_SLC_CMD_INFOR1_REG (DR_REG_SLC_BASE + 0xf0)
/** SDIO_CMD_CONTENT1 : RO; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define SDIO_CMD_CONTENT1    0xFFFFFFFFU
#define SDIO_CMD_CONTENT1_M  (SDIO_CMD_CONTENT1_V << SDIO_CMD_CONTENT1_S)
#define SDIO_CMD_CONTENT1_V  0xFFFFFFFFU
#define SDIO_CMD_CONTENT1_S  0

/** SDIO_SLC_SLC0_LEN_CONF_REG register
 *  reserved
 */
#define SDIO_SLC_SLC0_LEN_CONF_REG (DR_REG_SLC_BASE + 0xf4)
/** SDIO_SLC0_LEN_WDATA : WT; bitpos: [19:0]; default: 0;
 *  The packet length sent by dma channel0.
 */
#define SDIO_SLC0_LEN_WDATA    0x000FFFFFU
#define SDIO_SLC0_LEN_WDATA_M  (SDIO_SLC0_LEN_WDATA_V << SDIO_SLC0_LEN_WDATA_S)
#define SDIO_SLC0_LEN_WDATA_V  0x000FFFFFU
#define SDIO_SLC0_LEN_WDATA_S  0
/** SDIO_SLC0_LEN_WR : WT; bitpos: [20]; default: 0;
 *  Set this bit to set SLC0_LEN as SDIO_SLC0_LEN_WDATA.
 */
#define SDIO_SLC0_LEN_WR    (BIT(20))
#define SDIO_SLC0_LEN_WR_M  (SDIO_SLC0_LEN_WR_V << SDIO_SLC0_LEN_WR_S)
#define SDIO_SLC0_LEN_WR_V  0x00000001U
#define SDIO_SLC0_LEN_WR_S  20
/** SDIO_SLC0_LEN_INC : WT; bitpos: [21]; default: 0;
 *  Set this bit to add 1 to SLC0_LEN.
 */
#define SDIO_SLC0_LEN_INC    (BIT(21))
#define SDIO_SLC0_LEN_INC_M  (SDIO_SLC0_LEN_INC_V << SDIO_SLC0_LEN_INC_S)
#define SDIO_SLC0_LEN_INC_V  0x00000001U
#define SDIO_SLC0_LEN_INC_S  21
/** SDIO_SLC0_LEN_INC_MORE : WT; bitpos: [22]; default: 0;
 *  Set this bit to add the value of SDIO_SLC0_LEN_WDATA to SLC0_LEN.
 */
#define SDIO_SLC0_LEN_INC_MORE    (BIT(22))
#define SDIO_SLC0_LEN_INC_MORE_M  (SDIO_SLC0_LEN_INC_MORE_V << SDIO_SLC0_LEN_INC_MORE_S)
#define SDIO_SLC0_LEN_INC_MORE_V  0x00000001U
#define SDIO_SLC0_LEN_INC_MORE_S  22
/** SDIO_SLC0_RX_PACKET_LOAD_EN : WT; bitpos: [23]; default: 0;
 *  Write 1 to stitch new linked lists to current linked lists when dma channel0
 *  sending packets.
 */
#define SDIO_SLC0_RX_PACKET_LOAD_EN    (BIT(23))
#define SDIO_SLC0_RX_PACKET_LOAD_EN_M  (SDIO_SLC0_RX_PACKET_LOAD_EN_V << SDIO_SLC0_RX_PACKET_LOAD_EN_S)
#define SDIO_SLC0_RX_PACKET_LOAD_EN_V  0x00000001U
#define SDIO_SLC0_RX_PACKET_LOAD_EN_S  23
/** SDIO_SLC0_TX_PACKET_LOAD_EN : WT; bitpos: [24]; default: 0;
 *  Write 1 to stitch new linked lists to current linked lists when dma channel0
 *  receiving packets.
 */
#define SDIO_SLC0_TX_PACKET_LOAD_EN    (BIT(24))
#define SDIO_SLC0_TX_PACKET_LOAD_EN_M  (SDIO_SLC0_TX_PACKET_LOAD_EN_V << SDIO_SLC0_TX_PACKET_LOAD_EN_S)
#define SDIO_SLC0_TX_PACKET_LOAD_EN_V  0x00000001U
#define SDIO_SLC0_TX_PACKET_LOAD_EN_S  24
/** SDIO_SLC0_RX_GET_USED_DSCR : WT; bitpos: [25]; default: 0;
 *  Write 1 to get the current descriptor address when dma channel0 sending packets.
 */
#define SDIO_SLC0_RX_GET_USED_DSCR    (BIT(25))
#define SDIO_SLC0_RX_GET_USED_DSCR_M  (SDIO_SLC0_RX_GET_USED_DSCR_V << SDIO_SLC0_RX_GET_USED_DSCR_S)
#define SDIO_SLC0_RX_GET_USED_DSCR_V  0x00000001U
#define SDIO_SLC0_RX_GET_USED_DSCR_S  25
/** SDIO_SLC0_TX_GET_USED_DSCR : WT; bitpos: [26]; default: 0;
 *  Write 1 to get the current descriptor address when dma channel0 receiving packets.
 */
#define SDIO_SLC0_TX_GET_USED_DSCR    (BIT(26))
#define SDIO_SLC0_TX_GET_USED_DSCR_M  (SDIO_SLC0_TX_GET_USED_DSCR_V << SDIO_SLC0_TX_GET_USED_DSCR_S)
#define SDIO_SLC0_TX_GET_USED_DSCR_V  0x00000001U
#define SDIO_SLC0_TX_GET_USED_DSCR_S  26
/** SDIO_SLC0_RX_NEW_PKT_IND : RO; bitpos: [27]; default: 0;
 *  Represents a new packet is sent by dma channel0.
 */
#define SDIO_SLC0_RX_NEW_PKT_IND    (BIT(27))
#define SDIO_SLC0_RX_NEW_PKT_IND_M  (SDIO_SLC0_RX_NEW_PKT_IND_V << SDIO_SLC0_RX_NEW_PKT_IND_S)
#define SDIO_SLC0_RX_NEW_PKT_IND_V  0x00000001U
#define SDIO_SLC0_RX_NEW_PKT_IND_S  27
/** SDIO_SLC0_TX_NEW_PKT_IND : RO; bitpos: [28]; default: 1;
 *  Represents a new packet is received by dma channel0.
 */
#define SDIO_SLC0_TX_NEW_PKT_IND    (BIT(28))
#define SDIO_SLC0_TX_NEW_PKT_IND_M  (SDIO_SLC0_TX_NEW_PKT_IND_V << SDIO_SLC0_TX_NEW_PKT_IND_S)
#define SDIO_SLC0_TX_NEW_PKT_IND_V  0x00000001U
#define SDIO_SLC0_TX_NEW_PKT_IND_S  28
/** SDIO_SLC0_RX_PACKET_LOAD_EN_ST : R/WTC/SC; bitpos: [29]; default: 0;
 *  Represents whether the dma channel0 can be stitched to new linked lists when
 *  sending packets. 0- allowed, 1- not allowed.
 */
#define SDIO_SLC0_RX_PACKET_LOAD_EN_ST    (BIT(29))
#define SDIO_SLC0_RX_PACKET_LOAD_EN_ST_M  (SDIO_SLC0_RX_PACKET_LOAD_EN_ST_V << SDIO_SLC0_RX_PACKET_LOAD_EN_ST_S)
#define SDIO_SLC0_RX_PACKET_LOAD_EN_ST_V  0x00000001U
#define SDIO_SLC0_RX_PACKET_LOAD_EN_ST_S  29
/** SDIO_SLC0_TX_PACKET_LOAD_EN_ST : R/WTC/SC; bitpos: [30]; default: 0;
 *  Represents whether the dma channel0 can be stitched to new linked lists when
 *  receiving packets. 0- allowed, 1- not allowed.
 */
#define SDIO_SLC0_TX_PACKET_LOAD_EN_ST    (BIT(30))
#define SDIO_SLC0_TX_PACKET_LOAD_EN_ST_M  (SDIO_SLC0_TX_PACKET_LOAD_EN_ST_V << SDIO_SLC0_TX_PACKET_LOAD_EN_ST_S)
#define SDIO_SLC0_TX_PACKET_LOAD_EN_ST_V  0x00000001U
#define SDIO_SLC0_TX_PACKET_LOAD_EN_ST_S  30

/** SDIO_SLC_SLC0_LENGTH_REG register
 *  reserved
 */
#define SDIO_SLC_SLC0_LENGTH_REG (DR_REG_SLC_BASE + 0xf8)
/** SDIO_SLC0_LEN : RO; bitpos: [19:0]; default: 0;
 *  Indicates the paclet length sent by the slave through dma channel0.
 */
#define SDIO_SLC0_LEN    0x000FFFFFU
#define SDIO_SLC0_LEN_M  (SDIO_SLC0_LEN_V << SDIO_SLC0_LEN_S)
#define SDIO_SLC0_LEN_V  0x000FFFFFU
#define SDIO_SLC0_LEN_S  0

/** SDIO_SLC_SLC0_TXPKT_H_DSCR_REG register
 *  Sdio slave DMA channel0 start address of receiving linked list register.
 */
#define SDIO_SLC_SLC0_TXPKT_H_DSCR_REG (DR_REG_SLC_BASE + 0xfc)
/** SDIO_SLC0_TX_PKT_H_DSCR_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Configure the start address of receiving linked list to be stitched to the current
 *  linked list of dma channel0.
 */
#define SDIO_SLC0_TX_PKT_H_DSCR_ADDR    0xFFFFFFFFU
#define SDIO_SLC0_TX_PKT_H_DSCR_ADDR_M  (SDIO_SLC0_TX_PKT_H_DSCR_ADDR_V << SDIO_SLC0_TX_PKT_H_DSCR_ADDR_S)
#define SDIO_SLC0_TX_PKT_H_DSCR_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC0_TX_PKT_H_DSCR_ADDR_S  0

/** SDIO_SLC_SLC0_TXPKT_E_DSCR_REG register
 *  Sdio slave DMA channel0 end address of receiving linked list register.
 */
#define SDIO_SLC_SLC0_TXPKT_E_DSCR_REG (DR_REG_SLC_BASE + 0x100)
/** SDIO_SLC0_TX_PKT_E_DSCR_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Configure end address of receiving linked list to be stitched to the current linked
 *  list of dma channel0.
 */
#define SDIO_SLC0_TX_PKT_E_DSCR_ADDR    0xFFFFFFFFU
#define SDIO_SLC0_TX_PKT_E_DSCR_ADDR_M  (SDIO_SLC0_TX_PKT_E_DSCR_ADDR_V << SDIO_SLC0_TX_PKT_E_DSCR_ADDR_S)
#define SDIO_SLC0_TX_PKT_E_DSCR_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC0_TX_PKT_E_DSCR_ADDR_S  0

/** SDIO_SLC_SLC0_RXPKT_H_DSCR_REG register
 *  Sdio slave DMA channel0 start address of sending linked list register.
 */
#define SDIO_SLC_SLC0_RXPKT_H_DSCR_REG (DR_REG_SLC_BASE + 0x104)
/** SDIO_SLC0_RX_PKT_H_DSCR_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Configure the start address of sending linked list to be stitched to the current
 *  linked list of dma channel0.
 */
#define SDIO_SLC0_RX_PKT_H_DSCR_ADDR    0xFFFFFFFFU
#define SDIO_SLC0_RX_PKT_H_DSCR_ADDR_M  (SDIO_SLC0_RX_PKT_H_DSCR_ADDR_V << SDIO_SLC0_RX_PKT_H_DSCR_ADDR_S)
#define SDIO_SLC0_RX_PKT_H_DSCR_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC0_RX_PKT_H_DSCR_ADDR_S  0

/** SDIO_SLC_SLC0_RXPKT_E_DSCR_REG register
 *  Sdio slave DMA channel0 end address of sending linked list register.
 */
#define SDIO_SLC_SLC0_RXPKT_E_DSCR_REG (DR_REG_SLC_BASE + 0x108)
/** SDIO_SLC0_RX_PKT_E_DSCR_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Configure the end address of sending linked list to be stitched to the current
 *  linked list of dma channel0.
 */
#define SDIO_SLC0_RX_PKT_E_DSCR_ADDR    0xFFFFFFFFU
#define SDIO_SLC0_RX_PKT_E_DSCR_ADDR_M  (SDIO_SLC0_RX_PKT_E_DSCR_ADDR_V << SDIO_SLC0_RX_PKT_E_DSCR_ADDR_S)
#define SDIO_SLC0_RX_PKT_E_DSCR_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC0_RX_PKT_E_DSCR_ADDR_S  0

/** SDIO_SLC_SLC0_TXPKTU_H_DSCR_REG register
 *  Sdio slave DMA channel0 start address of receiving linked list status register.
 */
#define SDIO_SLC_SLC0_TXPKTU_H_DSCR_REG (DR_REG_SLC_BASE + 0x10c)
/** SDIO_SLC0_TX_PKT_START_DSCR_ADDR : RO; bitpos: [31:0]; default: 0;
 *  The start address of the linked list of the last received packet through dma
 *  channel0.
 */
#define SDIO_SLC0_TX_PKT_START_DSCR_ADDR    0xFFFFFFFFU
#define SDIO_SLC0_TX_PKT_START_DSCR_ADDR_M  (SDIO_SLC0_TX_PKT_START_DSCR_ADDR_V << SDIO_SLC0_TX_PKT_START_DSCR_ADDR_S)
#define SDIO_SLC0_TX_PKT_START_DSCR_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC0_TX_PKT_START_DSCR_ADDR_S  0

/** SDIO_SLC_SLC0_TXPKTU_E_DSCR_REG register
 *  Sdio slave DMA channel0 end address of receiving linked list status register.
 */
#define SDIO_SLC_SLC0_TXPKTU_E_DSCR_REG (DR_REG_SLC_BASE + 0x110)
/** SDIO_SLC0_TX_PKT_END_DSCR_ADDR : RO; bitpos: [31:0]; default: 0;
 *  The end address of the linked list of the last received packet through dma channel0.
 */
#define SDIO_SLC0_TX_PKT_END_DSCR_ADDR    0xFFFFFFFFU
#define SDIO_SLC0_TX_PKT_END_DSCR_ADDR_M  (SDIO_SLC0_TX_PKT_END_DSCR_ADDR_V << SDIO_SLC0_TX_PKT_END_DSCR_ADDR_S)
#define SDIO_SLC0_TX_PKT_END_DSCR_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC0_TX_PKT_END_DSCR_ADDR_S  0

/** SDIO_SLC_SLC0_RXPKTU_H_DSCR_REG register
 *  Sdio slave DMA channel0 start address of sending linked list status register.
 */
#define SDIO_SLC_SLC0_RXPKTU_H_DSCR_REG (DR_REG_SLC_BASE + 0x114)
/** SDIO_SLC0_RX_PKT_START_DSCR_ADDR : RO; bitpos: [31:0]; default: 0;
 *  The start address of the linked list of the last sent packet through dma channel0.
 */
#define SDIO_SLC0_RX_PKT_START_DSCR_ADDR    0xFFFFFFFFU
#define SDIO_SLC0_RX_PKT_START_DSCR_ADDR_M  (SDIO_SLC0_RX_PKT_START_DSCR_ADDR_V << SDIO_SLC0_RX_PKT_START_DSCR_ADDR_S)
#define SDIO_SLC0_RX_PKT_START_DSCR_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC0_RX_PKT_START_DSCR_ADDR_S  0

/** SDIO_SLC_SLC0_RXPKTU_E_DSCR_REG register
 *  Sdio slave DMA channel0 end address of sending linked list status register.
 */
#define SDIO_SLC_SLC0_RXPKTU_E_DSCR_REG (DR_REG_SLC_BASE + 0x118)
/** SDIO_SLC0_RX_PKT_END_DSCR_ADDR : RO; bitpos: [31:0]; default: 0;
 *  The end address of the linked list of the last sent packet through dma channel0.
 */
#define SDIO_SLC0_RX_PKT_END_DSCR_ADDR    0xFFFFFFFFU
#define SDIO_SLC0_RX_PKT_END_DSCR_ADDR_M  (SDIO_SLC0_RX_PKT_END_DSCR_ADDR_V << SDIO_SLC0_RX_PKT_END_DSCR_ADDR_S)
#define SDIO_SLC0_RX_PKT_END_DSCR_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC0_RX_PKT_END_DSCR_ADDR_S  0

/** SDIO_SLC_SEQ_POSITION_REG register
 *  The position of receiving packet configuration register.
 */
#define SDIO_SLC_SEQ_POSITION_REG (DR_REG_SLC_BASE + 0x11c)
/** SDIO_SLC0_SEQ_POSITION : R/W; bitpos: [7:0]; default: 9;
 *  Configure the position of the receiving packet through dma channel0.
 */
#define SDIO_SLC0_SEQ_POSITION    0x000000FFU
#define SDIO_SLC0_SEQ_POSITION_M  (SDIO_SLC0_SEQ_POSITION_V << SDIO_SLC0_SEQ_POSITION_S)
#define SDIO_SLC0_SEQ_POSITION_V  0x000000FFU
#define SDIO_SLC0_SEQ_POSITION_S  0
/** SDIO_SLC1_SEQ_POSITION : R/W; bitpos: [15:8]; default: 5;
 *  Configure the position of the receiving packet through dma channel1.
 */
#define SDIO_SLC1_SEQ_POSITION    0x000000FFU
#define SDIO_SLC1_SEQ_POSITION_M  (SDIO_SLC1_SEQ_POSITION_V << SDIO_SLC1_SEQ_POSITION_S)
#define SDIO_SLC1_SEQ_POSITION_V  0x000000FFU
#define SDIO_SLC1_SEQ_POSITION_S  8

/** SDIO_SLC_SLC0_DSCR_REC_CONF_REG register
 *  Sdio slave DMA sending linked list limit configuration register.
 */
#define SDIO_SLC_SLC0_DSCR_REC_CONF_REG (DR_REG_SLC_BASE + 0x120)
/** SDIO_SLC0_RX_DSCR_REC_LIM : R/W; bitpos: [9:0]; default: 1023;
 *  Configure the limit number of descriptors used to send one packet through dma
 *  channel0 in stitch mode.
 */
#define SDIO_SLC0_RX_DSCR_REC_LIM    0x000003FFU
#define SDIO_SLC0_RX_DSCR_REC_LIM_M  (SDIO_SLC0_RX_DSCR_REC_LIM_V << SDIO_SLC0_RX_DSCR_REC_LIM_S)
#define SDIO_SLC0_RX_DSCR_REC_LIM_V  0x000003FFU
#define SDIO_SLC0_RX_DSCR_REC_LIM_S  0

/** SDIO_SLC_SDIO_CRC_ST0_REG register
 *  Sdio slave receiving packet data crc status register.
 */
#define SDIO_SLC_SDIO_CRC_ST0_REG (DR_REG_SLC_BASE + 0x124)
/** SDIO_DAT0_CRC_ERR_CNT : RO; bitpos: [7:0]; default: 0;
 *  The count value of received packet data crc errors on sd_data0 line.
 */
#define SDIO_DAT0_CRC_ERR_CNT    0x000000FFU
#define SDIO_DAT0_CRC_ERR_CNT_M  (SDIO_DAT0_CRC_ERR_CNT_V << SDIO_DAT0_CRC_ERR_CNT_S)
#define SDIO_DAT0_CRC_ERR_CNT_V  0x000000FFU
#define SDIO_DAT0_CRC_ERR_CNT_S  0
/** SDIO_DAT1_CRC_ERR_CNT : RO; bitpos: [15:8]; default: 0;
 *  The count value of received packet data crc errors on sd_data1 line.
 */
#define SDIO_DAT1_CRC_ERR_CNT    0x000000FFU
#define SDIO_DAT1_CRC_ERR_CNT_M  (SDIO_DAT1_CRC_ERR_CNT_V << SDIO_DAT1_CRC_ERR_CNT_S)
#define SDIO_DAT1_CRC_ERR_CNT_V  0x000000FFU
#define SDIO_DAT1_CRC_ERR_CNT_S  8
/** SDIO_DAT2_CRC_ERR_CNT : RO; bitpos: [23:16]; default: 0;
 *  The count value of received packet data crc errors on sd_data2 line.
 */
#define SDIO_DAT2_CRC_ERR_CNT    0x000000FFU
#define SDIO_DAT2_CRC_ERR_CNT_M  (SDIO_DAT2_CRC_ERR_CNT_V << SDIO_DAT2_CRC_ERR_CNT_S)
#define SDIO_DAT2_CRC_ERR_CNT_V  0x000000FFU
#define SDIO_DAT2_CRC_ERR_CNT_S  16
/** SDIO_DAT3_CRC_ERR_CNT : RO; bitpos: [31:24]; default: 0;
 *  The count value of received packet data crc errors on sd_data3 line.
 */
#define SDIO_DAT3_CRC_ERR_CNT    0x000000FFU
#define SDIO_DAT3_CRC_ERR_CNT_M  (SDIO_DAT3_CRC_ERR_CNT_V << SDIO_DAT3_CRC_ERR_CNT_S)
#define SDIO_DAT3_CRC_ERR_CNT_V  0x000000FFU
#define SDIO_DAT3_CRC_ERR_CNT_S  24

/** SDIO_SLC_SDIO_CRC_ST1_REG register
 *  Sdio slave receiving command crc status register.
 */
#define SDIO_SLC_SDIO_CRC_ST1_REG (DR_REG_SLC_BASE + 0x128)
/** SDIO_CMD_CRC_ERR_CNT : RO; bitpos: [7:0]; default: 0;
 *  The count value of received command crc errors.
 */
#define SDIO_CMD_CRC_ERR_CNT    0x000000FFU
#define SDIO_CMD_CRC_ERR_CNT_M  (SDIO_CMD_CRC_ERR_CNT_V << SDIO_CMD_CRC_ERR_CNT_S)
#define SDIO_CMD_CRC_ERR_CNT_V  0x000000FFU
#define SDIO_CMD_CRC_ERR_CNT_S  0
/** SDIO_ERR_CNT_CLR : R/W; bitpos: [31]; default: 0;
 *  Set 1 to clear the cmd_crc_err_cnt, dat0_crc_err_cnt, dat1_crc_err_cnt,
 *  dat2_crc_err_cnt and dat3_crc_err_cnt.
 */
#define SDIO_ERR_CNT_CLR    (BIT(31))
#define SDIO_ERR_CNT_CLR_M  (SDIO_ERR_CNT_CLR_V << SDIO_ERR_CNT_CLR_S)
#define SDIO_ERR_CNT_CLR_V  0x00000001U
#define SDIO_ERR_CNT_CLR_S  31

/** SDIO_SLC_SLC0_EOF_START_DES_REG register
 *  Sdio slave DMA channel0 start address of sending linked list status register.
 */
#define SDIO_SLC_SLC0_EOF_START_DES_REG (DR_REG_SLC_BASE + 0x12c)
/** SDIO_SLC0_EOF_START_DES_ADDR : RO; bitpos: [31:0]; default: 0;
 *  The start address of the linked list of the last sent packet through dma channel0.
 */
#define SDIO_SLC0_EOF_START_DES_ADDR    0xFFFFFFFFU
#define SDIO_SLC0_EOF_START_DES_ADDR_M  (SDIO_SLC0_EOF_START_DES_ADDR_V << SDIO_SLC0_EOF_START_DES_ADDR_S)
#define SDIO_SLC0_EOF_START_DES_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC0_EOF_START_DES_ADDR_S  0

/** SDIO_SLC_SLC0_PUSH_DSCR_ADDR_REG register
 *  Sdio slave DMA channel0 address of sending linked list status register.
 */
#define SDIO_SLC_SLC0_PUSH_DSCR_ADDR_REG (DR_REG_SLC_BASE + 0x130)
/** SDIO_SLC0_RX_PUSH_DSCR_ADDR : RO; bitpos: [31:0]; default: 0;
 *  the current descriptor address when slc0 gets a link descriptor, aligned with word
 */
#define SDIO_SLC0_RX_PUSH_DSCR_ADDR    0xFFFFFFFFU
#define SDIO_SLC0_RX_PUSH_DSCR_ADDR_M  (SDIO_SLC0_RX_PUSH_DSCR_ADDR_V << SDIO_SLC0_RX_PUSH_DSCR_ADDR_S)
#define SDIO_SLC0_RX_PUSH_DSCR_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC0_RX_PUSH_DSCR_ADDR_S  0

/** SDIO_SLC_SLC0_DONE_DSCR_ADDR_REG register
 *  Sdio slave DMA channel0 address of sending linked list status register1.
 */
#define SDIO_SLC_SLC0_DONE_DSCR_ADDR_REG (DR_REG_SLC_BASE + 0x134)
/** SDIO_SLC0_RX_DONE_DSCR_ADDR : RO; bitpos: [31:0]; default: 0;
 *  the current descriptor address when slc0 finishes reading data from one buffer,
 *  aligned with word
 */
#define SDIO_SLC0_RX_DONE_DSCR_ADDR    0xFFFFFFFFU
#define SDIO_SLC0_RX_DONE_DSCR_ADDR_M  (SDIO_SLC0_RX_DONE_DSCR_ADDR_V << SDIO_SLC0_RX_DONE_DSCR_ADDR_S)
#define SDIO_SLC0_RX_DONE_DSCR_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC0_RX_DONE_DSCR_ADDR_S  0

/** SDIO_SLC_SLC0_SUB_START_DES_REG register
 *  Sdio slave DMA channel0 start address of sending linked list status register.
 */
#define SDIO_SLC_SLC0_SUB_START_DES_REG (DR_REG_SLC_BASE + 0x138)
/** SDIO_SLC0_SUB_PAC_START_DSCR_ADDR : RO; bitpos: [31:0]; default: 0;
 *  The start address of the linked list of the last sent packet through dma channel0.
 */
#define SDIO_SLC0_SUB_PAC_START_DSCR_ADDR    0xFFFFFFFFU
#define SDIO_SLC0_SUB_PAC_START_DSCR_ADDR_M  (SDIO_SLC0_SUB_PAC_START_DSCR_ADDR_V << SDIO_SLC0_SUB_PAC_START_DSCR_ADDR_S)
#define SDIO_SLC0_SUB_PAC_START_DSCR_ADDR_V  0xFFFFFFFFU
#define SDIO_SLC0_SUB_PAC_START_DSCR_ADDR_S  0

/** SDIO_SLC_SLC0_DSCR_CNT_REG register
 *  Sdio slave DMA channel0 descriptor status register.
 */
#define SDIO_SLC_SLC0_DSCR_CNT_REG (DR_REG_SLC_BASE + 0x13c)
/** SDIO_SLC0_RX_DSCR_CNT_LAT : RO; bitpos: [9:0]; default: 0;
 *  The number of descriptors got by dma channel0 for sending packets.
 */
#define SDIO_SLC0_RX_DSCR_CNT_LAT    0x000003FFU
#define SDIO_SLC0_RX_DSCR_CNT_LAT_M  (SDIO_SLC0_RX_DSCR_CNT_LAT_V << SDIO_SLC0_RX_DSCR_CNT_LAT_S)
#define SDIO_SLC0_RX_DSCR_CNT_LAT_V  0x000003FFU
#define SDIO_SLC0_RX_DSCR_CNT_LAT_S  0
/** SDIO_SLC0_RX_GET_EOF_OCC : RO; bitpos: [16]; default: 0;
 *  The status of last packet finishing sending to host.
 */
#define SDIO_SLC0_RX_GET_EOF_OCC    (BIT(16))
#define SDIO_SLC0_RX_GET_EOF_OCC_M  (SDIO_SLC0_RX_GET_EOF_OCC_V << SDIO_SLC0_RX_GET_EOF_OCC_S)
#define SDIO_SLC0_RX_GET_EOF_OCC_V  0x00000001U
#define SDIO_SLC0_RX_GET_EOF_OCC_S  16

/** SDIO_SLC_SLC0_LEN_LIM_CONF_REG register
 *  Sdio slave DMA channel0 buffer limit configuration register.
 */
#define SDIO_SLC_SLC0_LEN_LIM_CONF_REG (DR_REG_SLC_BASE + 0x140)
/** SDIO_SLC0_LEN_LIM : R/W; bitpos: [19:0]; default: 21504;
 *  Every time when the number of buffers loaded for sending packets reaches the number
 *  of slc0_len_lim, the current number of buffers loaded is saved into the registers
 *  of slc0_len0, slc0_len1 and slc0_len2.
 */
#define SDIO_SLC0_LEN_LIM    0x000FFFFFU
#define SDIO_SLC0_LEN_LIM_M  (SDIO_SLC0_LEN_LIM_V << SDIO_SLC0_LEN_LIM_S)
#define SDIO_SLC0_LEN_LIM_V  0x000FFFFFU
#define SDIO_SLC0_LEN_LIM_S  0

/** SDIO_SLC_SLC0INT_ST1_REG register
 *  Sdio slave DMA channel0 masked interrupt status register1.
 */
#define SDIO_SLC_SLC0INT_ST1_REG (DR_REG_SLC_BASE + 0x144)
/** SDIO_SLC_FRHOST_BIT0_INT_ST1 : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status bit1 for SLC_FRHOST_bit10_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT0_INT_ST1    (BIT(0))
#define SDIO_SLC_FRHOST_BIT0_INT_ST1_M  (SDIO_SLC_FRHOST_BIT0_INT_ST1_V << SDIO_SLC_FRHOST_BIT0_INT_ST1_S)
#define SDIO_SLC_FRHOST_BIT0_INT_ST1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT0_INT_ST1_S  0
/** SDIO_SLC_FRHOST_BIT1_INT_ST1 : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status bit1 for SLC_FRHOST_bit11_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT1_INT_ST1    (BIT(1))
#define SDIO_SLC_FRHOST_BIT1_INT_ST1_M  (SDIO_SLC_FRHOST_BIT1_INT_ST1_V << SDIO_SLC_FRHOST_BIT1_INT_ST1_S)
#define SDIO_SLC_FRHOST_BIT1_INT_ST1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT1_INT_ST1_S  1
/** SDIO_SLC_FRHOST_BIT2_INT_ST1 : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status bit1 for SLC_FRHOST_bit12_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT2_INT_ST1    (BIT(2))
#define SDIO_SLC_FRHOST_BIT2_INT_ST1_M  (SDIO_SLC_FRHOST_BIT2_INT_ST1_V << SDIO_SLC_FRHOST_BIT2_INT_ST1_S)
#define SDIO_SLC_FRHOST_BIT2_INT_ST1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT2_INT_ST1_S  2
/** SDIO_SLC_FRHOST_BIT3_INT_ST1 : RO; bitpos: [3]; default: 0;
 *  The masked interrupt status bit1 for SLC_FRHOST_bit13_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT3_INT_ST1    (BIT(3))
#define SDIO_SLC_FRHOST_BIT3_INT_ST1_M  (SDIO_SLC_FRHOST_BIT3_INT_ST1_V << SDIO_SLC_FRHOST_BIT3_INT_ST1_S)
#define SDIO_SLC_FRHOST_BIT3_INT_ST1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT3_INT_ST1_S  3
/** SDIO_SLC_FRHOST_BIT4_INT_ST1 : RO; bitpos: [4]; default: 0;
 *  The masked interrupt status bit1 for SLC_FRHOST_bit14_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT4_INT_ST1    (BIT(4))
#define SDIO_SLC_FRHOST_BIT4_INT_ST1_M  (SDIO_SLC_FRHOST_BIT4_INT_ST1_V << SDIO_SLC_FRHOST_BIT4_INT_ST1_S)
#define SDIO_SLC_FRHOST_BIT4_INT_ST1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT4_INT_ST1_S  4
/** SDIO_SLC_FRHOST_BIT5_INT_ST1 : RO; bitpos: [5]; default: 0;
 *  The masked interrupt status bit1 for SLC_FRHOST_bit15_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT5_INT_ST1    (BIT(5))
#define SDIO_SLC_FRHOST_BIT5_INT_ST1_M  (SDIO_SLC_FRHOST_BIT5_INT_ST1_V << SDIO_SLC_FRHOST_BIT5_INT_ST1_S)
#define SDIO_SLC_FRHOST_BIT5_INT_ST1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT5_INT_ST1_S  5
/** SDIO_SLC_FRHOST_BIT6_INT_ST1 : RO; bitpos: [6]; default: 0;
 *  The masked interrupt status bit1 for SLC_FRHOST_bit16_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT6_INT_ST1    (BIT(6))
#define SDIO_SLC_FRHOST_BIT6_INT_ST1_M  (SDIO_SLC_FRHOST_BIT6_INT_ST1_V << SDIO_SLC_FRHOST_BIT6_INT_ST1_S)
#define SDIO_SLC_FRHOST_BIT6_INT_ST1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT6_INT_ST1_S  6
/** SDIO_SLC_FRHOST_BIT7_INT_ST1 : RO; bitpos: [7]; default: 0;
 *  The masked interrupt status bit1 for SLC_FRHOST_bit17_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT7_INT_ST1    (BIT(7))
#define SDIO_SLC_FRHOST_BIT7_INT_ST1_M  (SDIO_SLC_FRHOST_BIT7_INT_ST1_V << SDIO_SLC_FRHOST_BIT7_INT_ST1_S)
#define SDIO_SLC_FRHOST_BIT7_INT_ST1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT7_INT_ST1_S  7
/** SDIO_SLC0_RX_START_INT_ST1 : RO; bitpos: [8]; default: 0;
 *  The masked interrupt status bit1 for SLC0_RX_START_INT_ST interrupt.
 */
#define SDIO_SLC0_RX_START_INT_ST1    (BIT(8))
#define SDIO_SLC0_RX_START_INT_ST1_M  (SDIO_SLC0_RX_START_INT_ST1_V << SDIO_SLC0_RX_START_INT_ST1_S)
#define SDIO_SLC0_RX_START_INT_ST1_V  0x00000001U
#define SDIO_SLC0_RX_START_INT_ST1_S  8
/** SDIO_SLC0_TX_START_INT_ST1 : RO; bitpos: [9]; default: 0;
 *  The masked interrupt status bit1 for SLC0_TX_START_INT_ST interrupt.
 */
#define SDIO_SLC0_TX_START_INT_ST1    (BIT(9))
#define SDIO_SLC0_TX_START_INT_ST1_M  (SDIO_SLC0_TX_START_INT_ST1_V << SDIO_SLC0_TX_START_INT_ST1_S)
#define SDIO_SLC0_TX_START_INT_ST1_V  0x00000001U
#define SDIO_SLC0_TX_START_INT_ST1_S  9
/** SDIO_SLC0_RX_UDF_INT_ST1 : RO; bitpos: [10]; default: 0;
 *  The masked interrupt status bit1 for SLC0_RX_UDF_INT interrupt.
 */
#define SDIO_SLC0_RX_UDF_INT_ST1    (BIT(10))
#define SDIO_SLC0_RX_UDF_INT_ST1_M  (SDIO_SLC0_RX_UDF_INT_ST1_V << SDIO_SLC0_RX_UDF_INT_ST1_S)
#define SDIO_SLC0_RX_UDF_INT_ST1_V  0x00000001U
#define SDIO_SLC0_RX_UDF_INT_ST1_S  10
/** SDIO_SLC0_TX_OVF_INT_ST1 : RO; bitpos: [11]; default: 0;
 *  The masked interrupt status bit1 for SLC0_TX_OVF_INT interrupt.
 */
#define SDIO_SLC0_TX_OVF_INT_ST1    (BIT(11))
#define SDIO_SLC0_TX_OVF_INT_ST1_M  (SDIO_SLC0_TX_OVF_INT_ST1_V << SDIO_SLC0_TX_OVF_INT_ST1_S)
#define SDIO_SLC0_TX_OVF_INT_ST1_V  0x00000001U
#define SDIO_SLC0_TX_OVF_INT_ST1_S  11
/** SDIO_SLC0_TOKEN0_1TO0_INT_ST1 : RO; bitpos: [12]; default: 0;
 *  The masked interrupt status bit1 for SLC0_TOKEN0_1TO0_INT interrupt.
 */
#define SDIO_SLC0_TOKEN0_1TO0_INT_ST1    (BIT(12))
#define SDIO_SLC0_TOKEN0_1TO0_INT_ST1_M  (SDIO_SLC0_TOKEN0_1TO0_INT_ST1_V << SDIO_SLC0_TOKEN0_1TO0_INT_ST1_S)
#define SDIO_SLC0_TOKEN0_1TO0_INT_ST1_V  0x00000001U
#define SDIO_SLC0_TOKEN0_1TO0_INT_ST1_S  12
/** SDIO_SLC0_TOKEN1_1TO0_INT_ST1 : RO; bitpos: [13]; default: 0;
 *  The masked interrupt status bit1 for SLC0_TOKEN1_1TO0_INT interrupt.
 */
#define SDIO_SLC0_TOKEN1_1TO0_INT_ST1    (BIT(13))
#define SDIO_SLC0_TOKEN1_1TO0_INT_ST1_M  (SDIO_SLC0_TOKEN1_1TO0_INT_ST1_V << SDIO_SLC0_TOKEN1_1TO0_INT_ST1_S)
#define SDIO_SLC0_TOKEN1_1TO0_INT_ST1_V  0x00000001U
#define SDIO_SLC0_TOKEN1_1TO0_INT_ST1_S  13
/** SDIO_SLC0_TX_DONE_INT_ST1 : RO; bitpos: [14]; default: 0;
 *  The masked interrupt status bit1 for SLC0_TX_DONE_INT_ST interrupt.
 */
#define SDIO_SLC0_TX_DONE_INT_ST1    (BIT(14))
#define SDIO_SLC0_TX_DONE_INT_ST1_M  (SDIO_SLC0_TX_DONE_INT_ST1_V << SDIO_SLC0_TX_DONE_INT_ST1_S)
#define SDIO_SLC0_TX_DONE_INT_ST1_V  0x00000001U
#define SDIO_SLC0_TX_DONE_INT_ST1_S  14
/** SDIO_SLC0_TX_SUC_EOF_INT_ST1 : RO; bitpos: [15]; default: 0;
 *  The masked interrupt status bit1 for SLC0_TX_SUC_EOF_INT interrupt.
 */
#define SDIO_SLC0_TX_SUC_EOF_INT_ST1    (BIT(15))
#define SDIO_SLC0_TX_SUC_EOF_INT_ST1_M  (SDIO_SLC0_TX_SUC_EOF_INT_ST1_V << SDIO_SLC0_TX_SUC_EOF_INT_ST1_S)
#define SDIO_SLC0_TX_SUC_EOF_INT_ST1_V  0x00000001U
#define SDIO_SLC0_TX_SUC_EOF_INT_ST1_S  15
/** SDIO_SLC0_RX_DONE_INT_ST1 : RO; bitpos: [16]; default: 0;
 *  The masked interrupt status bit1 for SLC0_RX_DONE_INT interrupt.
 */
#define SDIO_SLC0_RX_DONE_INT_ST1    (BIT(16))
#define SDIO_SLC0_RX_DONE_INT_ST1_M  (SDIO_SLC0_RX_DONE_INT_ST1_V << SDIO_SLC0_RX_DONE_INT_ST1_S)
#define SDIO_SLC0_RX_DONE_INT_ST1_V  0x00000001U
#define SDIO_SLC0_RX_DONE_INT_ST1_S  16
/** SDIO_SLC0_RX_EOF_INT_ST1 : RO; bitpos: [17]; default: 0;
 *  The masked interrupt status bit1 for SLC0_RX_EOF_INT interrupt.
 */
#define SDIO_SLC0_RX_EOF_INT_ST1    (BIT(17))
#define SDIO_SLC0_RX_EOF_INT_ST1_M  (SDIO_SLC0_RX_EOF_INT_ST1_V << SDIO_SLC0_RX_EOF_INT_ST1_S)
#define SDIO_SLC0_RX_EOF_INT_ST1_V  0x00000001U
#define SDIO_SLC0_RX_EOF_INT_ST1_S  17
/** SDIO_SLC0_TOHOST_INT_ST1 : RO; bitpos: [18]; default: 0;
 *  The masked interrupt status bit1 for SLC0_TOHOST_INT interrupt.
 */
#define SDIO_SLC0_TOHOST_INT_ST1    (BIT(18))
#define SDIO_SLC0_TOHOST_INT_ST1_M  (SDIO_SLC0_TOHOST_INT_ST1_V << SDIO_SLC0_TOHOST_INT_ST1_S)
#define SDIO_SLC0_TOHOST_INT_ST1_V  0x00000001U
#define SDIO_SLC0_TOHOST_INT_ST1_S  18
/** SDIO_SLC0_TX_DSCR_ERR_INT_ST1 : RO; bitpos: [19]; default: 0;
 *  The masked interrupt status bit1 for SLC0_TX_DSCR_ERR_INT interrupt.
 */
#define SDIO_SLC0_TX_DSCR_ERR_INT_ST1    (BIT(19))
#define SDIO_SLC0_TX_DSCR_ERR_INT_ST1_M  (SDIO_SLC0_TX_DSCR_ERR_INT_ST1_V << SDIO_SLC0_TX_DSCR_ERR_INT_ST1_S)
#define SDIO_SLC0_TX_DSCR_ERR_INT_ST1_V  0x00000001U
#define SDIO_SLC0_TX_DSCR_ERR_INT_ST1_S  19
/** SDIO_SLC0_RX_DSCR_ERR_INT_ST1 : RO; bitpos: [20]; default: 0;
 *  The masked interrupt status bit1 for SLC0_RX_DSCR_ERR_INT interrupt.
 */
#define SDIO_SLC0_RX_DSCR_ERR_INT_ST1    (BIT(20))
#define SDIO_SLC0_RX_DSCR_ERR_INT_ST1_M  (SDIO_SLC0_RX_DSCR_ERR_INT_ST1_V << SDIO_SLC0_RX_DSCR_ERR_INT_ST1_S)
#define SDIO_SLC0_RX_DSCR_ERR_INT_ST1_V  0x00000001U
#define SDIO_SLC0_RX_DSCR_ERR_INT_ST1_S  20
/** SDIO_SLC0_TX_DSCR_EMPTY_INT_ST1 : RO; bitpos: [21]; default: 0;
 *  The masked interrupt status bit1 for SLC0_TX_DSCR_EMPTY_INT interrupt.
 */
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_ST1    (BIT(21))
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_ST1_M  (SDIO_SLC0_TX_DSCR_EMPTY_INT_ST1_V << SDIO_SLC0_TX_DSCR_EMPTY_INT_ST1_S)
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_ST1_V  0x00000001U
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_ST1_S  21
/** SDIO_SLC0_HOST_RD_ACK_INT_ST1 : RO; bitpos: [22]; default: 0;
 *  The masked interrupt status bit1 for SLC0_HOST_RD_ACK_INT interrupt.
 */
#define SDIO_SLC0_HOST_RD_ACK_INT_ST1    (BIT(22))
#define SDIO_SLC0_HOST_RD_ACK_INT_ST1_M  (SDIO_SLC0_HOST_RD_ACK_INT_ST1_V << SDIO_SLC0_HOST_RD_ACK_INT_ST1_S)
#define SDIO_SLC0_HOST_RD_ACK_INT_ST1_V  0x00000001U
#define SDIO_SLC0_HOST_RD_ACK_INT_ST1_S  22
/** SDIO_SLC0_WR_RETRY_DONE_INT_ST1 : RO; bitpos: [23]; default: 0;
 *  The masked interrupt status bit1 for SLC0_WR_RETRY_DONE_INT interrupt.
 */
#define SDIO_SLC0_WR_RETRY_DONE_INT_ST1    (BIT(23))
#define SDIO_SLC0_WR_RETRY_DONE_INT_ST1_M  (SDIO_SLC0_WR_RETRY_DONE_INT_ST1_V << SDIO_SLC0_WR_RETRY_DONE_INT_ST1_S)
#define SDIO_SLC0_WR_RETRY_DONE_INT_ST1_V  0x00000001U
#define SDIO_SLC0_WR_RETRY_DONE_INT_ST1_S  23
/** SDIO_SLC0_TX_ERR_EOF_INT_ST1 : RO; bitpos: [24]; default: 0;
 *  The masked interrupt status bit1 for SLC0_TX_ERR_EOF_INT interrupt.
 */
#define SDIO_SLC0_TX_ERR_EOF_INT_ST1    (BIT(24))
#define SDIO_SLC0_TX_ERR_EOF_INT_ST1_M  (SDIO_SLC0_TX_ERR_EOF_INT_ST1_V << SDIO_SLC0_TX_ERR_EOF_INT_ST1_S)
#define SDIO_SLC0_TX_ERR_EOF_INT_ST1_V  0x00000001U
#define SDIO_SLC0_TX_ERR_EOF_INT_ST1_S  24
/** SDIO_CMD_DTC_INT_ST1 : RO; bitpos: [25]; default: 0;
 *  The masked interrupt status bit1 for CMD_DTC_INT interrupt.
 */
#define SDIO_CMD_DTC_INT_ST1    (BIT(25))
#define SDIO_CMD_DTC_INT_ST1_M  (SDIO_CMD_DTC_INT_ST1_V << SDIO_CMD_DTC_INT_ST1_S)
#define SDIO_CMD_DTC_INT_ST1_V  0x00000001U
#define SDIO_CMD_DTC_INT_ST1_S  25
/** SDIO_SLC0_RX_QUICK_EOF_INT_ST1 : RO; bitpos: [26]; default: 0;
 *  The masked interrupt status bit1 for SLC0_RX_QUICK_EOF_INT interrupt.
 */
#define SDIO_SLC0_RX_QUICK_EOF_INT_ST1    (BIT(26))
#define SDIO_SLC0_RX_QUICK_EOF_INT_ST1_M  (SDIO_SLC0_RX_QUICK_EOF_INT_ST1_V << SDIO_SLC0_RX_QUICK_EOF_INT_ST1_S)
#define SDIO_SLC0_RX_QUICK_EOF_INT_ST1_V  0x00000001U
#define SDIO_SLC0_RX_QUICK_EOF_INT_ST1_S  26
/** SDIO_SLC0_HOST_POP_EOF_ERR_INT_ST1 : RO; bitpos: [27]; default: 0;
 *  The masked interrupt status bit1 for SLC0_HOST_POP_EOF_ERR_INT interrupt.
 */
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_ST1    (BIT(27))
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_ST1_M  (SDIO_SLC0_HOST_POP_EOF_ERR_INT_ST1_V << SDIO_SLC0_HOST_POP_EOF_ERR_INT_ST1_S)
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_ST1_V  0x00000001U
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_ST1_S  27
/** SDIO_HDA_RECV_DONE_INT_ST1 : RO; bitpos: [28]; default: 0;
 *  The masked interrupt status bit1 for HDA_RECV_DONE_INT interrupt.
 */
#define SDIO_HDA_RECV_DONE_INT_ST1    (BIT(28))
#define SDIO_HDA_RECV_DONE_INT_ST1_M  (SDIO_HDA_RECV_DONE_INT_ST1_V << SDIO_HDA_RECV_DONE_INT_ST1_S)
#define SDIO_HDA_RECV_DONE_INT_ST1_V  0x00000001U
#define SDIO_HDA_RECV_DONE_INT_ST1_S  28

/** SDIO_SLC_SLC0INT_ENA1_REG register
 *  Sdio slave DMA channel0 interrupt enable register1.
 */
#define SDIO_SLC_SLC0INT_ENA1_REG (DR_REG_SLC_BASE + 0x148)
/** SDIO_SLC_FRHOST_BIT0_INT_ENA1 : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit1 for SLC_FRHOST_bit10_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT0_INT_ENA1    (BIT(0))
#define SDIO_SLC_FRHOST_BIT0_INT_ENA1_M  (SDIO_SLC_FRHOST_BIT0_INT_ENA1_V << SDIO_SLC_FRHOST_BIT0_INT_ENA1_S)
#define SDIO_SLC_FRHOST_BIT0_INT_ENA1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT0_INT_ENA1_S  0
/** SDIO_SLC_FRHOST_BIT1_INT_ENA1 : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit1 for SLC_FRHOST_bit11_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT1_INT_ENA1    (BIT(1))
#define SDIO_SLC_FRHOST_BIT1_INT_ENA1_M  (SDIO_SLC_FRHOST_BIT1_INT_ENA1_V << SDIO_SLC_FRHOST_BIT1_INT_ENA1_S)
#define SDIO_SLC_FRHOST_BIT1_INT_ENA1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT1_INT_ENA1_S  1
/** SDIO_SLC_FRHOST_BIT2_INT_ENA1 : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit1 for SLC_FRHOST_bit12_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT2_INT_ENA1    (BIT(2))
#define SDIO_SLC_FRHOST_BIT2_INT_ENA1_M  (SDIO_SLC_FRHOST_BIT2_INT_ENA1_V << SDIO_SLC_FRHOST_BIT2_INT_ENA1_S)
#define SDIO_SLC_FRHOST_BIT2_INT_ENA1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT2_INT_ENA1_S  2
/** SDIO_SLC_FRHOST_BIT3_INT_ENA1 : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit1 for SLC_FRHOST_bit13_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT3_INT_ENA1    (BIT(3))
#define SDIO_SLC_FRHOST_BIT3_INT_ENA1_M  (SDIO_SLC_FRHOST_BIT3_INT_ENA1_V << SDIO_SLC_FRHOST_BIT3_INT_ENA1_S)
#define SDIO_SLC_FRHOST_BIT3_INT_ENA1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT3_INT_ENA1_S  3
/** SDIO_SLC_FRHOST_BIT4_INT_ENA1 : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit1 for SLC_FRHOST_bit14_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT4_INT_ENA1    (BIT(4))
#define SDIO_SLC_FRHOST_BIT4_INT_ENA1_M  (SDIO_SLC_FRHOST_BIT4_INT_ENA1_V << SDIO_SLC_FRHOST_BIT4_INT_ENA1_S)
#define SDIO_SLC_FRHOST_BIT4_INT_ENA1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT4_INT_ENA1_S  4
/** SDIO_SLC_FRHOST_BIT5_INT_ENA1 : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit1 for SLC_FRHOST_bit15_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT5_INT_ENA1    (BIT(5))
#define SDIO_SLC_FRHOST_BIT5_INT_ENA1_M  (SDIO_SLC_FRHOST_BIT5_INT_ENA1_V << SDIO_SLC_FRHOST_BIT5_INT_ENA1_S)
#define SDIO_SLC_FRHOST_BIT5_INT_ENA1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT5_INT_ENA1_S  5
/** SDIO_SLC_FRHOST_BIT6_INT_ENA1 : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit1 for SLC_FRHOST_bit16_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT6_INT_ENA1    (BIT(6))
#define SDIO_SLC_FRHOST_BIT6_INT_ENA1_M  (SDIO_SLC_FRHOST_BIT6_INT_ENA1_V << SDIO_SLC_FRHOST_BIT6_INT_ENA1_S)
#define SDIO_SLC_FRHOST_BIT6_INT_ENA1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT6_INT_ENA1_S  6
/** SDIO_SLC_FRHOST_BIT7_INT_ENA1 : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit1 for SLC_FRHOST_bit17_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT7_INT_ENA1    (BIT(7))
#define SDIO_SLC_FRHOST_BIT7_INT_ENA1_M  (SDIO_SLC_FRHOST_BIT7_INT_ENA1_V << SDIO_SLC_FRHOST_BIT7_INT_ENA1_S)
#define SDIO_SLC_FRHOST_BIT7_INT_ENA1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT7_INT_ENA1_S  7
/** SDIO_SLC0_RX_START_INT_ENA1 : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit1 for SLC0_RX_START_INT_ST interrupt.
 */
#define SDIO_SLC0_RX_START_INT_ENA1    (BIT(8))
#define SDIO_SLC0_RX_START_INT_ENA1_M  (SDIO_SLC0_RX_START_INT_ENA1_V << SDIO_SLC0_RX_START_INT_ENA1_S)
#define SDIO_SLC0_RX_START_INT_ENA1_V  0x00000001U
#define SDIO_SLC0_RX_START_INT_ENA1_S  8
/** SDIO_SLC0_TX_START_INT_ENA1 : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit1 for SLC0_TX_START_INT_ST interrupt.
 */
#define SDIO_SLC0_TX_START_INT_ENA1    (BIT(9))
#define SDIO_SLC0_TX_START_INT_ENA1_M  (SDIO_SLC0_TX_START_INT_ENA1_V << SDIO_SLC0_TX_START_INT_ENA1_S)
#define SDIO_SLC0_TX_START_INT_ENA1_V  0x00000001U
#define SDIO_SLC0_TX_START_INT_ENA1_S  9
/** SDIO_SLC0_RX_UDF_INT_ENA1 : R/W; bitpos: [10]; default: 0;
 *  The interrupt enable bit1 for SLC0_RX_UDF_INT interrupt.
 */
#define SDIO_SLC0_RX_UDF_INT_ENA1    (BIT(10))
#define SDIO_SLC0_RX_UDF_INT_ENA1_M  (SDIO_SLC0_RX_UDF_INT_ENA1_V << SDIO_SLC0_RX_UDF_INT_ENA1_S)
#define SDIO_SLC0_RX_UDF_INT_ENA1_V  0x00000001U
#define SDIO_SLC0_RX_UDF_INT_ENA1_S  10
/** SDIO_SLC0_TX_OVF_INT_ENA1 : R/W; bitpos: [11]; default: 0;
 *  The interrupt enable bit1 for SLC0_TX_OVF_INT interrupt.
 */
#define SDIO_SLC0_TX_OVF_INT_ENA1    (BIT(11))
#define SDIO_SLC0_TX_OVF_INT_ENA1_M  (SDIO_SLC0_TX_OVF_INT_ENA1_V << SDIO_SLC0_TX_OVF_INT_ENA1_S)
#define SDIO_SLC0_TX_OVF_INT_ENA1_V  0x00000001U
#define SDIO_SLC0_TX_OVF_INT_ENA1_S  11
/** SDIO_SLC0_TOKEN0_1TO0_INT_ENA1 : R/W; bitpos: [12]; default: 0;
 *  The interrupt enable bit1 for SLC0_TOKEN0_1TO0_INT interrupt.
 */
#define SDIO_SLC0_TOKEN0_1TO0_INT_ENA1    (BIT(12))
#define SDIO_SLC0_TOKEN0_1TO0_INT_ENA1_M  (SDIO_SLC0_TOKEN0_1TO0_INT_ENA1_V << SDIO_SLC0_TOKEN0_1TO0_INT_ENA1_S)
#define SDIO_SLC0_TOKEN0_1TO0_INT_ENA1_V  0x00000001U
#define SDIO_SLC0_TOKEN0_1TO0_INT_ENA1_S  12
/** SDIO_SLC0_TOKEN1_1TO0_INT_ENA1 : R/W; bitpos: [13]; default: 0;
 *  The interrupt enable bit1 for SLC0_TOKEN1_1TO0_INT interrupt.
 */
#define SDIO_SLC0_TOKEN1_1TO0_INT_ENA1    (BIT(13))
#define SDIO_SLC0_TOKEN1_1TO0_INT_ENA1_M  (SDIO_SLC0_TOKEN1_1TO0_INT_ENA1_V << SDIO_SLC0_TOKEN1_1TO0_INT_ENA1_S)
#define SDIO_SLC0_TOKEN1_1TO0_INT_ENA1_V  0x00000001U
#define SDIO_SLC0_TOKEN1_1TO0_INT_ENA1_S  13
/** SDIO_SLC0_TX_DONE_INT_ENA1 : R/W; bitpos: [14]; default: 0;
 *  The interrupt enable bit1 for SLC0_TX_DONE_INT_ST interrupt.
 */
#define SDIO_SLC0_TX_DONE_INT_ENA1    (BIT(14))
#define SDIO_SLC0_TX_DONE_INT_ENA1_M  (SDIO_SLC0_TX_DONE_INT_ENA1_V << SDIO_SLC0_TX_DONE_INT_ENA1_S)
#define SDIO_SLC0_TX_DONE_INT_ENA1_V  0x00000001U
#define SDIO_SLC0_TX_DONE_INT_ENA1_S  14
/** SDIO_SLC0_TX_SUC_EOF_INT_ENA1 : R/W; bitpos: [15]; default: 0;
 *  The interrupt enable bit1 for SLC0_TX_SUC_EOF_INT interrupt.
 */
#define SDIO_SLC0_TX_SUC_EOF_INT_ENA1    (BIT(15))
#define SDIO_SLC0_TX_SUC_EOF_INT_ENA1_M  (SDIO_SLC0_TX_SUC_EOF_INT_ENA1_V << SDIO_SLC0_TX_SUC_EOF_INT_ENA1_S)
#define SDIO_SLC0_TX_SUC_EOF_INT_ENA1_V  0x00000001U
#define SDIO_SLC0_TX_SUC_EOF_INT_ENA1_S  15
/** SDIO_SLC0_RX_DONE_INT_ENA1 : R/W; bitpos: [16]; default: 0;
 *  The interrupt enable bit1 for SLC0_RX_DONE_INT interrupt.
 */
#define SDIO_SLC0_RX_DONE_INT_ENA1    (BIT(16))
#define SDIO_SLC0_RX_DONE_INT_ENA1_M  (SDIO_SLC0_RX_DONE_INT_ENA1_V << SDIO_SLC0_RX_DONE_INT_ENA1_S)
#define SDIO_SLC0_RX_DONE_INT_ENA1_V  0x00000001U
#define SDIO_SLC0_RX_DONE_INT_ENA1_S  16
/** SDIO_SLC0_RX_EOF_INT_ENA1 : R/W; bitpos: [17]; default: 0;
 *  The interrupt enable bit1 for SLC0_RX_EOF_INT interrupt.
 */
#define SDIO_SLC0_RX_EOF_INT_ENA1    (BIT(17))
#define SDIO_SLC0_RX_EOF_INT_ENA1_M  (SDIO_SLC0_RX_EOF_INT_ENA1_V << SDIO_SLC0_RX_EOF_INT_ENA1_S)
#define SDIO_SLC0_RX_EOF_INT_ENA1_V  0x00000001U
#define SDIO_SLC0_RX_EOF_INT_ENA1_S  17
/** SDIO_SLC0_TOHOST_INT_ENA1 : R/W; bitpos: [18]; default: 0;
 *  The interrupt enable bit1 for SLC0_TOHOST_INT interrupt.
 */
#define SDIO_SLC0_TOHOST_INT_ENA1    (BIT(18))
#define SDIO_SLC0_TOHOST_INT_ENA1_M  (SDIO_SLC0_TOHOST_INT_ENA1_V << SDIO_SLC0_TOHOST_INT_ENA1_S)
#define SDIO_SLC0_TOHOST_INT_ENA1_V  0x00000001U
#define SDIO_SLC0_TOHOST_INT_ENA1_S  18
/** SDIO_SLC0_TX_DSCR_ERR_INT_ENA1 : R/W; bitpos: [19]; default: 0;
 *  The interrupt enable bit1 for SLC0_TX_DSCR_ERR_INT interrupt.
 */
#define SDIO_SLC0_TX_DSCR_ERR_INT_ENA1    (BIT(19))
#define SDIO_SLC0_TX_DSCR_ERR_INT_ENA1_M  (SDIO_SLC0_TX_DSCR_ERR_INT_ENA1_V << SDIO_SLC0_TX_DSCR_ERR_INT_ENA1_S)
#define SDIO_SLC0_TX_DSCR_ERR_INT_ENA1_V  0x00000001U
#define SDIO_SLC0_TX_DSCR_ERR_INT_ENA1_S  19
/** SDIO_SLC0_RX_DSCR_ERR_INT_ENA1 : R/W; bitpos: [20]; default: 0;
 *  The interrupt enable bit1 for SLC0_RX_DSCR_ERR_INT interrupt.
 */
#define SDIO_SLC0_RX_DSCR_ERR_INT_ENA1    (BIT(20))
#define SDIO_SLC0_RX_DSCR_ERR_INT_ENA1_M  (SDIO_SLC0_RX_DSCR_ERR_INT_ENA1_V << SDIO_SLC0_RX_DSCR_ERR_INT_ENA1_S)
#define SDIO_SLC0_RX_DSCR_ERR_INT_ENA1_V  0x00000001U
#define SDIO_SLC0_RX_DSCR_ERR_INT_ENA1_S  20
/** SDIO_SLC0_TX_DSCR_EMPTY_INT_ENA1 : R/W; bitpos: [21]; default: 0;
 *  The interrupt enable bit1 for SLC0_TX_DSCR_EMPTY_INT interrupt.
 */
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_ENA1    (BIT(21))
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_ENA1_M  (SDIO_SLC0_TX_DSCR_EMPTY_INT_ENA1_V << SDIO_SLC0_TX_DSCR_EMPTY_INT_ENA1_S)
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_ENA1_V  0x00000001U
#define SDIO_SLC0_TX_DSCR_EMPTY_INT_ENA1_S  21
/** SDIO_SLC0_HOST_RD_ACK_INT_ENA1 : R/W; bitpos: [22]; default: 0;
 *  The interrupt enable bit1 for SLC0_HOST_RD_ACK_INT interrupt.
 */
#define SDIO_SLC0_HOST_RD_ACK_INT_ENA1    (BIT(22))
#define SDIO_SLC0_HOST_RD_ACK_INT_ENA1_M  (SDIO_SLC0_HOST_RD_ACK_INT_ENA1_V << SDIO_SLC0_HOST_RD_ACK_INT_ENA1_S)
#define SDIO_SLC0_HOST_RD_ACK_INT_ENA1_V  0x00000001U
#define SDIO_SLC0_HOST_RD_ACK_INT_ENA1_S  22
/** SDIO_SLC0_WR_RETRY_DONE_INT_ENA1 : R/W; bitpos: [23]; default: 0;
 *  The interrupt enable bit1 for SLC0_WR_RETRY_DONE_INT interrupt.
 */
#define SDIO_SLC0_WR_RETRY_DONE_INT_ENA1    (BIT(23))
#define SDIO_SLC0_WR_RETRY_DONE_INT_ENA1_M  (SDIO_SLC0_WR_RETRY_DONE_INT_ENA1_V << SDIO_SLC0_WR_RETRY_DONE_INT_ENA1_S)
#define SDIO_SLC0_WR_RETRY_DONE_INT_ENA1_V  0x00000001U
#define SDIO_SLC0_WR_RETRY_DONE_INT_ENA1_S  23
/** SDIO_SLC0_TX_ERR_EOF_INT_ENA1 : R/W; bitpos: [24]; default: 0;
 *  The interrupt enable bit1 for SLC0_TX_ERR_EOF_INT interrupt.
 */
#define SDIO_SLC0_TX_ERR_EOF_INT_ENA1    (BIT(24))
#define SDIO_SLC0_TX_ERR_EOF_INT_ENA1_M  (SDIO_SLC0_TX_ERR_EOF_INT_ENA1_V << SDIO_SLC0_TX_ERR_EOF_INT_ENA1_S)
#define SDIO_SLC0_TX_ERR_EOF_INT_ENA1_V  0x00000001U
#define SDIO_SLC0_TX_ERR_EOF_INT_ENA1_S  24
/** SDIO_CMD_DTC_INT_ENA1 : R/W; bitpos: [25]; default: 0;
 *  The interrupt enable bit1 for CMD_DTC_INT interrupt.
 */
#define SDIO_CMD_DTC_INT_ENA1    (BIT(25))
#define SDIO_CMD_DTC_INT_ENA1_M  (SDIO_CMD_DTC_INT_ENA1_V << SDIO_CMD_DTC_INT_ENA1_S)
#define SDIO_CMD_DTC_INT_ENA1_V  0x00000001U
#define SDIO_CMD_DTC_INT_ENA1_S  25
/** SDIO_SLC0_RX_QUICK_EOF_INT_ENA1 : R/W; bitpos: [26]; default: 0;
 *  The interrupt enable bit1 for SLC0_RX_QUICK_EOF_INT interrupt.
 */
#define SDIO_SLC0_RX_QUICK_EOF_INT_ENA1    (BIT(26))
#define SDIO_SLC0_RX_QUICK_EOF_INT_ENA1_M  (SDIO_SLC0_RX_QUICK_EOF_INT_ENA1_V << SDIO_SLC0_RX_QUICK_EOF_INT_ENA1_S)
#define SDIO_SLC0_RX_QUICK_EOF_INT_ENA1_V  0x00000001U
#define SDIO_SLC0_RX_QUICK_EOF_INT_ENA1_S  26
/** SDIO_SLC0_HOST_POP_EOF_ERR_INT_ENA1 : R/W; bitpos: [27]; default: 0;
 *  The interrupt enable bit1 for SLC0_HOST_POP_EOF_ERR_INT interrupt.
 */
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_ENA1    (BIT(27))
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_ENA1_M  (SDIO_SLC0_HOST_POP_EOF_ERR_INT_ENA1_V << SDIO_SLC0_HOST_POP_EOF_ERR_INT_ENA1_S)
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_ENA1_V  0x00000001U
#define SDIO_SLC0_HOST_POP_EOF_ERR_INT_ENA1_S  27
/** SDIO_HDA_RECV_DONE_INT_ENA1 : R/W; bitpos: [28]; default: 0;
 *  The interrupt enable bit1 for HDA_RECV_DONE_INT interrupt.
 */
#define SDIO_HDA_RECV_DONE_INT_ENA1    (BIT(28))
#define SDIO_HDA_RECV_DONE_INT_ENA1_M  (SDIO_HDA_RECV_DONE_INT_ENA1_V << SDIO_HDA_RECV_DONE_INT_ENA1_S)
#define SDIO_HDA_RECV_DONE_INT_ENA1_V  0x00000001U
#define SDIO_HDA_RECV_DONE_INT_ENA1_S  28

/** SDIO_SLC_SLC1INT_ST1_REG register
 *  Sdio slave DMA channel1 masked interrupt status register1.
 */
#define SDIO_SLC_SLC1INT_ST1_REG (DR_REG_SLC_BASE + 0x14c)
/** SDIO_SLC_FRHOST_BIT8_INT_ST1 : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status bit1 for SLC_FRHOST_bit10_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT8_INT_ST1    (BIT(0))
#define SDIO_SLC_FRHOST_BIT8_INT_ST1_M  (SDIO_SLC_FRHOST_BIT8_INT_ST1_V << SDIO_SLC_FRHOST_BIT8_INT_ST1_S)
#define SDIO_SLC_FRHOST_BIT8_INT_ST1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT8_INT_ST1_S  0
/** SDIO_SLC_FRHOST_BIT9_INT_ST1 : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status bit1 for SLC_FRHOST_bit11_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT9_INT_ST1    (BIT(1))
#define SDIO_SLC_FRHOST_BIT9_INT_ST1_M  (SDIO_SLC_FRHOST_BIT9_INT_ST1_V << SDIO_SLC_FRHOST_BIT9_INT_ST1_S)
#define SDIO_SLC_FRHOST_BIT9_INT_ST1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT9_INT_ST1_S  1
/** SDIO_SLC_FRHOST_BIT10_INT_ST1 : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status bit1 for SLC_FRHOST_bit12_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT10_INT_ST1    (BIT(2))
#define SDIO_SLC_FRHOST_BIT10_INT_ST1_M  (SDIO_SLC_FRHOST_BIT10_INT_ST1_V << SDIO_SLC_FRHOST_BIT10_INT_ST1_S)
#define SDIO_SLC_FRHOST_BIT10_INT_ST1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT10_INT_ST1_S  2
/** SDIO_SLC_FRHOST_BIT11_INT_ST1 : RO; bitpos: [3]; default: 0;
 *  The masked interrupt status bit1 for SLC_FRHOST_bit13_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT11_INT_ST1    (BIT(3))
#define SDIO_SLC_FRHOST_BIT11_INT_ST1_M  (SDIO_SLC_FRHOST_BIT11_INT_ST1_V << SDIO_SLC_FRHOST_BIT11_INT_ST1_S)
#define SDIO_SLC_FRHOST_BIT11_INT_ST1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT11_INT_ST1_S  3
/** SDIO_SLC_FRHOST_BIT12_INT_ST1 : RO; bitpos: [4]; default: 0;
 *  The masked interrupt status bit1 for SLC_FRHOST_bit14_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT12_INT_ST1    (BIT(4))
#define SDIO_SLC_FRHOST_BIT12_INT_ST1_M  (SDIO_SLC_FRHOST_BIT12_INT_ST1_V << SDIO_SLC_FRHOST_BIT12_INT_ST1_S)
#define SDIO_SLC_FRHOST_BIT12_INT_ST1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT12_INT_ST1_S  4
/** SDIO_SLC_FRHOST_BIT13_INT_ST1 : RO; bitpos: [5]; default: 0;
 *  The masked interrupt status bit1 for SLC_FRHOST_bit15_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT13_INT_ST1    (BIT(5))
#define SDIO_SLC_FRHOST_BIT13_INT_ST1_M  (SDIO_SLC_FRHOST_BIT13_INT_ST1_V << SDIO_SLC_FRHOST_BIT13_INT_ST1_S)
#define SDIO_SLC_FRHOST_BIT13_INT_ST1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT13_INT_ST1_S  5
/** SDIO_SLC_FRHOST_BIT14_INT_ST1 : RO; bitpos: [6]; default: 0;
 *  The masked interrupt status bit1 for SLC_FRHOST_bit16_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT14_INT_ST1    (BIT(6))
#define SDIO_SLC_FRHOST_BIT14_INT_ST1_M  (SDIO_SLC_FRHOST_BIT14_INT_ST1_V << SDIO_SLC_FRHOST_BIT14_INT_ST1_S)
#define SDIO_SLC_FRHOST_BIT14_INT_ST1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT14_INT_ST1_S  6
/** SDIO_SLC_FRHOST_BIT15_INT_ST1 : RO; bitpos: [7]; default: 0;
 *  The masked interrupt status bit1 for SLC_FRHOST_bit17_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT15_INT_ST1    (BIT(7))
#define SDIO_SLC_FRHOST_BIT15_INT_ST1_M  (SDIO_SLC_FRHOST_BIT15_INT_ST1_V << SDIO_SLC_FRHOST_BIT15_INT_ST1_S)
#define SDIO_SLC_FRHOST_BIT15_INT_ST1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT15_INT_ST1_S  7
/** SDIO_SLC1_RX_START_INT_ST1 : RO; bitpos: [8]; default: 0;
 *  The masked interrupt status bit1 for SLC1_RX_START_INT_ST interrupt.
 */
#define SDIO_SLC1_RX_START_INT_ST1    (BIT(8))
#define SDIO_SLC1_RX_START_INT_ST1_M  (SDIO_SLC1_RX_START_INT_ST1_V << SDIO_SLC1_RX_START_INT_ST1_S)
#define SDIO_SLC1_RX_START_INT_ST1_V  0x00000001U
#define SDIO_SLC1_RX_START_INT_ST1_S  8
/** SDIO_SLC1_TX_START_INT_ST1 : RO; bitpos: [9]; default: 0;
 *  The masked interrupt status bit1 for SLC1_TX_START_INT_ST interrupt.
 */
#define SDIO_SLC1_TX_START_INT_ST1    (BIT(9))
#define SDIO_SLC1_TX_START_INT_ST1_M  (SDIO_SLC1_TX_START_INT_ST1_V << SDIO_SLC1_TX_START_INT_ST1_S)
#define SDIO_SLC1_TX_START_INT_ST1_V  0x00000001U
#define SDIO_SLC1_TX_START_INT_ST1_S  9
/** SDIO_SLC1_RX_UDF_INT_ST1 : RO; bitpos: [10]; default: 0;
 *  The masked interrupt status bit1 for SLC1_RX_UDF_INT interrupt.
 */
#define SDIO_SLC1_RX_UDF_INT_ST1    (BIT(10))
#define SDIO_SLC1_RX_UDF_INT_ST1_M  (SDIO_SLC1_RX_UDF_INT_ST1_V << SDIO_SLC1_RX_UDF_INT_ST1_S)
#define SDIO_SLC1_RX_UDF_INT_ST1_V  0x00000001U
#define SDIO_SLC1_RX_UDF_INT_ST1_S  10
/** SDIO_SLC1_TX_OVF_INT_ST1 : RO; bitpos: [11]; default: 0;
 *  The masked interrupt status bit1 for SLC1_TX_OVF_INT interrupt.
 */
#define SDIO_SLC1_TX_OVF_INT_ST1    (BIT(11))
#define SDIO_SLC1_TX_OVF_INT_ST1_M  (SDIO_SLC1_TX_OVF_INT_ST1_V << SDIO_SLC1_TX_OVF_INT_ST1_S)
#define SDIO_SLC1_TX_OVF_INT_ST1_V  0x00000001U
#define SDIO_SLC1_TX_OVF_INT_ST1_S  11
/** SDIO_SLC1_TOKEN0_1TO0_INT_ST1 : RO; bitpos: [12]; default: 0;
 *  The masked interrupt status bit1 for SLC1_TOKEN0_1TO0_INT interrupt.
 */
#define SDIO_SLC1_TOKEN0_1TO0_INT_ST1    (BIT(12))
#define SDIO_SLC1_TOKEN0_1TO0_INT_ST1_M  (SDIO_SLC1_TOKEN0_1TO0_INT_ST1_V << SDIO_SLC1_TOKEN0_1TO0_INT_ST1_S)
#define SDIO_SLC1_TOKEN0_1TO0_INT_ST1_V  0x00000001U
#define SDIO_SLC1_TOKEN0_1TO0_INT_ST1_S  12
/** SDIO_SLC1_TOKEN1_1TO0_INT_ST1 : RO; bitpos: [13]; default: 0;
 *  The masked interrupt status bit1 for SLC1_TOKEN1_1TO0_INT interrupt.
 */
#define SDIO_SLC1_TOKEN1_1TO0_INT_ST1    (BIT(13))
#define SDIO_SLC1_TOKEN1_1TO0_INT_ST1_M  (SDIO_SLC1_TOKEN1_1TO0_INT_ST1_V << SDIO_SLC1_TOKEN1_1TO0_INT_ST1_S)
#define SDIO_SLC1_TOKEN1_1TO0_INT_ST1_V  0x00000001U
#define SDIO_SLC1_TOKEN1_1TO0_INT_ST1_S  13
/** SDIO_SLC1_TX_DONE_INT_ST1 : RO; bitpos: [14]; default: 0;
 *  The masked interrupt status bit1 for SLC1_TX_DONE_INT_ST interrupt.
 */
#define SDIO_SLC1_TX_DONE_INT_ST1    (BIT(14))
#define SDIO_SLC1_TX_DONE_INT_ST1_M  (SDIO_SLC1_TX_DONE_INT_ST1_V << SDIO_SLC1_TX_DONE_INT_ST1_S)
#define SDIO_SLC1_TX_DONE_INT_ST1_V  0x00000001U
#define SDIO_SLC1_TX_DONE_INT_ST1_S  14
/** SDIO_SLC1_TX_SUC_EOF_INT_ST1 : RO; bitpos: [15]; default: 0;
 *  The masked interrupt status bit1 for SLC1_TX_SUC_EOF_INT interrupt.
 */
#define SDIO_SLC1_TX_SUC_EOF_INT_ST1    (BIT(15))
#define SDIO_SLC1_TX_SUC_EOF_INT_ST1_M  (SDIO_SLC1_TX_SUC_EOF_INT_ST1_V << SDIO_SLC1_TX_SUC_EOF_INT_ST1_S)
#define SDIO_SLC1_TX_SUC_EOF_INT_ST1_V  0x00000001U
#define SDIO_SLC1_TX_SUC_EOF_INT_ST1_S  15
/** SDIO_SLC1_RX_DONE_INT_ST1 : RO; bitpos: [16]; default: 0;
 *  The masked interrupt status bit1 for SLC1_RX_DONE_INT interrupt.
 */
#define SDIO_SLC1_RX_DONE_INT_ST1    (BIT(16))
#define SDIO_SLC1_RX_DONE_INT_ST1_M  (SDIO_SLC1_RX_DONE_INT_ST1_V << SDIO_SLC1_RX_DONE_INT_ST1_S)
#define SDIO_SLC1_RX_DONE_INT_ST1_V  0x00000001U
#define SDIO_SLC1_RX_DONE_INT_ST1_S  16
/** SDIO_SLC1_RX_EOF_INT_ST1 : RO; bitpos: [17]; default: 0;
 *  The masked interrupt status bit1 for SLC1_RX_EOF_INT interrupt.
 */
#define SDIO_SLC1_RX_EOF_INT_ST1    (BIT(17))
#define SDIO_SLC1_RX_EOF_INT_ST1_M  (SDIO_SLC1_RX_EOF_INT_ST1_V << SDIO_SLC1_RX_EOF_INT_ST1_S)
#define SDIO_SLC1_RX_EOF_INT_ST1_V  0x00000001U
#define SDIO_SLC1_RX_EOF_INT_ST1_S  17
/** SDIO_SLC1_TOHOST_INT_ST1 : RO; bitpos: [18]; default: 0;
 *  The masked interrupt status bit1 for SLC1_TOHOST_INT interrupt.
 */
#define SDIO_SLC1_TOHOST_INT_ST1    (BIT(18))
#define SDIO_SLC1_TOHOST_INT_ST1_M  (SDIO_SLC1_TOHOST_INT_ST1_V << SDIO_SLC1_TOHOST_INT_ST1_S)
#define SDIO_SLC1_TOHOST_INT_ST1_V  0x00000001U
#define SDIO_SLC1_TOHOST_INT_ST1_S  18
/** SDIO_SLC1_TX_DSCR_ERR_INT_ST1 : RO; bitpos: [19]; default: 0;
 *  The masked interrupt status bit1 for SLC1_TX_DSCR_ERR_INT interrupt.
 */
#define SDIO_SLC1_TX_DSCR_ERR_INT_ST1    (BIT(19))
#define SDIO_SLC1_TX_DSCR_ERR_INT_ST1_M  (SDIO_SLC1_TX_DSCR_ERR_INT_ST1_V << SDIO_SLC1_TX_DSCR_ERR_INT_ST1_S)
#define SDIO_SLC1_TX_DSCR_ERR_INT_ST1_V  0x00000001U
#define SDIO_SLC1_TX_DSCR_ERR_INT_ST1_S  19
/** SDIO_SLC1_RX_DSCR_ERR_INT_ST1 : RO; bitpos: [20]; default: 0;
 *  The masked interrupt status bit1 for SLC1_RX_DSCR_ERR_INT interrupt.
 */
#define SDIO_SLC1_RX_DSCR_ERR_INT_ST1    (BIT(20))
#define SDIO_SLC1_RX_DSCR_ERR_INT_ST1_M  (SDIO_SLC1_RX_DSCR_ERR_INT_ST1_V << SDIO_SLC1_RX_DSCR_ERR_INT_ST1_S)
#define SDIO_SLC1_RX_DSCR_ERR_INT_ST1_V  0x00000001U
#define SDIO_SLC1_RX_DSCR_ERR_INT_ST1_S  20
/** SDIO_SLC1_TX_DSCR_EMPTY_INT_ST1 : RO; bitpos: [21]; default: 0;
 *  The masked interrupt status bit1 for SLC1_TX_DSCR_EMPTY_INT interrupt.
 */
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_ST1    (BIT(21))
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_ST1_M  (SDIO_SLC1_TX_DSCR_EMPTY_INT_ST1_V << SDIO_SLC1_TX_DSCR_EMPTY_INT_ST1_S)
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_ST1_V  0x00000001U
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_ST1_S  21
/** SDIO_SLC1_HOST_RD_ACK_INT_ST1 : RO; bitpos: [22]; default: 0;
 *  The masked interrupt status bit1 for SLC1_HOST_RD_ACK_INT interrupt.
 */
#define SDIO_SLC1_HOST_RD_ACK_INT_ST1    (BIT(22))
#define SDIO_SLC1_HOST_RD_ACK_INT_ST1_M  (SDIO_SLC1_HOST_RD_ACK_INT_ST1_V << SDIO_SLC1_HOST_RD_ACK_INT_ST1_S)
#define SDIO_SLC1_HOST_RD_ACK_INT_ST1_V  0x00000001U
#define SDIO_SLC1_HOST_RD_ACK_INT_ST1_S  22
/** SDIO_SLC1_WR_RETRY_DONE_INT_ST1 : RO; bitpos: [23]; default: 0;
 *  The masked interrupt status bit1 for SLC1_WR_RETRY_DONE_INT interrupt.
 */
#define SDIO_SLC1_WR_RETRY_DONE_INT_ST1    (BIT(23))
#define SDIO_SLC1_WR_RETRY_DONE_INT_ST1_M  (SDIO_SLC1_WR_RETRY_DONE_INT_ST1_V << SDIO_SLC1_WR_RETRY_DONE_INT_ST1_S)
#define SDIO_SLC1_WR_RETRY_DONE_INT_ST1_V  0x00000001U
#define SDIO_SLC1_WR_RETRY_DONE_INT_ST1_S  23
/** SDIO_SLC1_TX_ERR_EOF_INT_ST1 : RO; bitpos: [24]; default: 0;
 *  The masked interrupt status bit1 for SLC1_TX_ERR_EOF_INT interrupt.
 */
#define SDIO_SLC1_TX_ERR_EOF_INT_ST1    (BIT(24))
#define SDIO_SLC1_TX_ERR_EOF_INT_ST1_M  (SDIO_SLC1_TX_ERR_EOF_INT_ST1_V << SDIO_SLC1_TX_ERR_EOF_INT_ST1_S)
#define SDIO_SLC1_TX_ERR_EOF_INT_ST1_V  0x00000001U
#define SDIO_SLC1_TX_ERR_EOF_INT_ST1_S  24

/** SDIO_SLC_SLC1INT_ENA1_REG register
 *  Sdio slave DMA channel1 interrupt enable register1.
 */
#define SDIO_SLC_SLC1INT_ENA1_REG (DR_REG_SLC_BASE + 0x150)
/** SDIO_SLC_FRHOST_BIT8_INT_ENA1 : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit1 for SLC_FRHOST_bit10_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT8_INT_ENA1    (BIT(0))
#define SDIO_SLC_FRHOST_BIT8_INT_ENA1_M  (SDIO_SLC_FRHOST_BIT8_INT_ENA1_V << SDIO_SLC_FRHOST_BIT8_INT_ENA1_S)
#define SDIO_SLC_FRHOST_BIT8_INT_ENA1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT8_INT_ENA1_S  0
/** SDIO_SLC_FRHOST_BIT9_INT_ENA1 : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit1 for SLC_FRHOST_bit11_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT9_INT_ENA1    (BIT(1))
#define SDIO_SLC_FRHOST_BIT9_INT_ENA1_M  (SDIO_SLC_FRHOST_BIT9_INT_ENA1_V << SDIO_SLC_FRHOST_BIT9_INT_ENA1_S)
#define SDIO_SLC_FRHOST_BIT9_INT_ENA1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT9_INT_ENA1_S  1
/** SDIO_SLC_FRHOST_BIT10_INT_ENA1 : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit1 for SLC_FRHOST_bit12_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT10_INT_ENA1    (BIT(2))
#define SDIO_SLC_FRHOST_BIT10_INT_ENA1_M  (SDIO_SLC_FRHOST_BIT10_INT_ENA1_V << SDIO_SLC_FRHOST_BIT10_INT_ENA1_S)
#define SDIO_SLC_FRHOST_BIT10_INT_ENA1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT10_INT_ENA1_S  2
/** SDIO_SLC_FRHOST_BIT11_INT_ENA1 : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit1 for SLC_FRHOST_bit13_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT11_INT_ENA1    (BIT(3))
#define SDIO_SLC_FRHOST_BIT11_INT_ENA1_M  (SDIO_SLC_FRHOST_BIT11_INT_ENA1_V << SDIO_SLC_FRHOST_BIT11_INT_ENA1_S)
#define SDIO_SLC_FRHOST_BIT11_INT_ENA1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT11_INT_ENA1_S  3
/** SDIO_SLC_FRHOST_BIT12_INT_ENA1 : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit1 for SLC_FRHOST_bit14_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT12_INT_ENA1    (BIT(4))
#define SDIO_SLC_FRHOST_BIT12_INT_ENA1_M  (SDIO_SLC_FRHOST_BIT12_INT_ENA1_V << SDIO_SLC_FRHOST_BIT12_INT_ENA1_S)
#define SDIO_SLC_FRHOST_BIT12_INT_ENA1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT12_INT_ENA1_S  4
/** SDIO_SLC_FRHOST_BIT13_INT_ENA1 : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit1 for SLC_FRHOST_bit15_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT13_INT_ENA1    (BIT(5))
#define SDIO_SLC_FRHOST_BIT13_INT_ENA1_M  (SDIO_SLC_FRHOST_BIT13_INT_ENA1_V << SDIO_SLC_FRHOST_BIT13_INT_ENA1_S)
#define SDIO_SLC_FRHOST_BIT13_INT_ENA1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT13_INT_ENA1_S  5
/** SDIO_SLC_FRHOST_BIT14_INT_ENA1 : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit1 for SLC_FRHOST_bit16_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT14_INT_ENA1    (BIT(6))
#define SDIO_SLC_FRHOST_BIT14_INT_ENA1_M  (SDIO_SLC_FRHOST_BIT14_INT_ENA1_V << SDIO_SLC_FRHOST_BIT14_INT_ENA1_S)
#define SDIO_SLC_FRHOST_BIT14_INT_ENA1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT14_INT_ENA1_S  6
/** SDIO_SLC_FRHOST_BIT15_INT_ENA1 : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit1 for SLC_FRHOST_bit17_INT interrupt.
 */
#define SDIO_SLC_FRHOST_BIT15_INT_ENA1    (BIT(7))
#define SDIO_SLC_FRHOST_BIT15_INT_ENA1_M  (SDIO_SLC_FRHOST_BIT15_INT_ENA1_V << SDIO_SLC_FRHOST_BIT15_INT_ENA1_S)
#define SDIO_SLC_FRHOST_BIT15_INT_ENA1_V  0x00000001U
#define SDIO_SLC_FRHOST_BIT15_INT_ENA1_S  7
/** SDIO_SLC1_RX_START_INT_ENA1 : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit1 for SCL1_RX_START_INT_ST interrupt.
 */
#define SDIO_SLC1_RX_START_INT_ENA1    (BIT(8))
#define SDIO_SLC1_RX_START_INT_ENA1_M  (SDIO_SLC1_RX_START_INT_ENA1_V << SDIO_SLC1_RX_START_INT_ENA1_S)
#define SDIO_SLC1_RX_START_INT_ENA1_V  0x00000001U
#define SDIO_SLC1_RX_START_INT_ENA1_S  8
/** SDIO_SLC1_TX_START_INT_ENA1 : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit1 for SCL1_TX_START_INT_ST interrupt.
 */
#define SDIO_SLC1_TX_START_INT_ENA1    (BIT(9))
#define SDIO_SLC1_TX_START_INT_ENA1_M  (SDIO_SLC1_TX_START_INT_ENA1_V << SDIO_SLC1_TX_START_INT_ENA1_S)
#define SDIO_SLC1_TX_START_INT_ENA1_V  0x00000001U
#define SDIO_SLC1_TX_START_INT_ENA1_S  9
/** SDIO_SLC1_RX_UDF_INT_ENA1 : R/W; bitpos: [10]; default: 0;
 *  The interrupt enable bit1 for SCL1_RX_UDF_INT interrupt.
 */
#define SDIO_SLC1_RX_UDF_INT_ENA1    (BIT(10))
#define SDIO_SLC1_RX_UDF_INT_ENA1_M  (SDIO_SLC1_RX_UDF_INT_ENA1_V << SDIO_SLC1_RX_UDF_INT_ENA1_S)
#define SDIO_SLC1_RX_UDF_INT_ENA1_V  0x00000001U
#define SDIO_SLC1_RX_UDF_INT_ENA1_S  10
/** SDIO_SLC1_TX_OVF_INT_ENA1 : R/W; bitpos: [11]; default: 0;
 *  The interrupt enable bit1 for SCL1_TX_OVF_INT interrupt.
 */
#define SDIO_SLC1_TX_OVF_INT_ENA1    (BIT(11))
#define SDIO_SLC1_TX_OVF_INT_ENA1_M  (SDIO_SLC1_TX_OVF_INT_ENA1_V << SDIO_SLC1_TX_OVF_INT_ENA1_S)
#define SDIO_SLC1_TX_OVF_INT_ENA1_V  0x00000001U
#define SDIO_SLC1_TX_OVF_INT_ENA1_S  11
/** SDIO_SLC1_TOKEN0_1TO0_INT_ENA1 : R/W; bitpos: [12]; default: 0;
 *  The interrupt enable bit1 for SCL1_TOKEN0_1TO0_INT interrupt.
 */
#define SDIO_SLC1_TOKEN0_1TO0_INT_ENA1    (BIT(12))
#define SDIO_SLC1_TOKEN0_1TO0_INT_ENA1_M  (SDIO_SLC1_TOKEN0_1TO0_INT_ENA1_V << SDIO_SLC1_TOKEN0_1TO0_INT_ENA1_S)
#define SDIO_SLC1_TOKEN0_1TO0_INT_ENA1_V  0x00000001U
#define SDIO_SLC1_TOKEN0_1TO0_INT_ENA1_S  12
/** SDIO_SLC1_TOKEN1_1TO0_INT_ENA1 : R/W; bitpos: [13]; default: 0;
 *  The interrupt enable bit1 for SCL1_TOKEN1_1TO0_INT interrupt.
 */
#define SDIO_SLC1_TOKEN1_1TO0_INT_ENA1    (BIT(13))
#define SDIO_SLC1_TOKEN1_1TO0_INT_ENA1_M  (SDIO_SLC1_TOKEN1_1TO0_INT_ENA1_V << SDIO_SLC1_TOKEN1_1TO0_INT_ENA1_S)
#define SDIO_SLC1_TOKEN1_1TO0_INT_ENA1_V  0x00000001U
#define SDIO_SLC1_TOKEN1_1TO0_INT_ENA1_S  13
/** SDIO_SLC1_TX_DONE_INT_ENA1 : R/W; bitpos: [14]; default: 0;
 *  The interrupt enable bit1 for SCL1_TX_DONE_INT_ST interrupt.
 */
#define SDIO_SLC1_TX_DONE_INT_ENA1    (BIT(14))
#define SDIO_SLC1_TX_DONE_INT_ENA1_M  (SDIO_SLC1_TX_DONE_INT_ENA1_V << SDIO_SLC1_TX_DONE_INT_ENA1_S)
#define SDIO_SLC1_TX_DONE_INT_ENA1_V  0x00000001U
#define SDIO_SLC1_TX_DONE_INT_ENA1_S  14
/** SDIO_SLC1_TX_SUC_EOF_INT_ENA1 : R/W; bitpos: [15]; default: 0;
 *  The interrupt enable bit1 for SCL1_TX_SUC_EOF_INT interrupt.
 */
#define SDIO_SLC1_TX_SUC_EOF_INT_ENA1    (BIT(15))
#define SDIO_SLC1_TX_SUC_EOF_INT_ENA1_M  (SDIO_SLC1_TX_SUC_EOF_INT_ENA1_V << SDIO_SLC1_TX_SUC_EOF_INT_ENA1_S)
#define SDIO_SLC1_TX_SUC_EOF_INT_ENA1_V  0x00000001U
#define SDIO_SLC1_TX_SUC_EOF_INT_ENA1_S  15
/** SDIO_SLC1_RX_DONE_INT_ENA1 : R/W; bitpos: [16]; default: 0;
 *  The interrupt enable bit1 for SCL1_RX_DONE_INT interrupt.
 */
#define SDIO_SLC1_RX_DONE_INT_ENA1    (BIT(16))
#define SDIO_SLC1_RX_DONE_INT_ENA1_M  (SDIO_SLC1_RX_DONE_INT_ENA1_V << SDIO_SLC1_RX_DONE_INT_ENA1_S)
#define SDIO_SLC1_RX_DONE_INT_ENA1_V  0x00000001U
#define SDIO_SLC1_RX_DONE_INT_ENA1_S  16
/** SDIO_SLC1_RX_EOF_INT_ENA1 : R/W; bitpos: [17]; default: 0;
 *  The interrupt enable bit1 for SCL1_RX_EOF_INT interrupt.
 */
#define SDIO_SLC1_RX_EOF_INT_ENA1    (BIT(17))
#define SDIO_SLC1_RX_EOF_INT_ENA1_M  (SDIO_SLC1_RX_EOF_INT_ENA1_V << SDIO_SLC1_RX_EOF_INT_ENA1_S)
#define SDIO_SLC1_RX_EOF_INT_ENA1_V  0x00000001U
#define SDIO_SLC1_RX_EOF_INT_ENA1_S  17
/** SDIO_SLC1_TOHOST_INT_ENA1 : R/W; bitpos: [18]; default: 0;
 *  The interrupt enable bit1 for SCL1_TOHOST_INT interrupt.
 */
#define SDIO_SLC1_TOHOST_INT_ENA1    (BIT(18))
#define SDIO_SLC1_TOHOST_INT_ENA1_M  (SDIO_SLC1_TOHOST_INT_ENA1_V << SDIO_SLC1_TOHOST_INT_ENA1_S)
#define SDIO_SLC1_TOHOST_INT_ENA1_V  0x00000001U
#define SDIO_SLC1_TOHOST_INT_ENA1_S  18
/** SDIO_SLC1_TX_DSCR_ERR_INT_ENA1 : R/W; bitpos: [19]; default: 0;
 *  The interrupt enable bit1 for SCL1_TX_DSCR_ERR_INT interrupt.
 */
#define SDIO_SLC1_TX_DSCR_ERR_INT_ENA1    (BIT(19))
#define SDIO_SLC1_TX_DSCR_ERR_INT_ENA1_M  (SDIO_SLC1_TX_DSCR_ERR_INT_ENA1_V << SDIO_SLC1_TX_DSCR_ERR_INT_ENA1_S)
#define SDIO_SLC1_TX_DSCR_ERR_INT_ENA1_V  0x00000001U
#define SDIO_SLC1_TX_DSCR_ERR_INT_ENA1_S  19
/** SDIO_SLC1_RX_DSCR_ERR_INT_ENA1 : R/W; bitpos: [20]; default: 0;
 *  The interrupt enable bit1 for SCL1_RX_DSCR_ERR_INT interrupt.
 */
#define SDIO_SLC1_RX_DSCR_ERR_INT_ENA1    (BIT(20))
#define SDIO_SLC1_RX_DSCR_ERR_INT_ENA1_M  (SDIO_SLC1_RX_DSCR_ERR_INT_ENA1_V << SDIO_SLC1_RX_DSCR_ERR_INT_ENA1_S)
#define SDIO_SLC1_RX_DSCR_ERR_INT_ENA1_V  0x00000001U
#define SDIO_SLC1_RX_DSCR_ERR_INT_ENA1_S  20
/** SDIO_SLC1_TX_DSCR_EMPTY_INT_ENA1 : R/W; bitpos: [21]; default: 0;
 *  The interrupt enable bit1 for SCL1_TX_DSCR_EMPTY_INT interrupt.
 */
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_ENA1    (BIT(21))
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_ENA1_M  (SDIO_SLC1_TX_DSCR_EMPTY_INT_ENA1_V << SDIO_SLC1_TX_DSCR_EMPTY_INT_ENA1_S)
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_ENA1_V  0x00000001U
#define SDIO_SLC1_TX_DSCR_EMPTY_INT_ENA1_S  21
/** SDIO_SLC1_HOST_RD_ACK_INT_ENA1 : R/W; bitpos: [22]; default: 0;
 *  The interrupt enable bit1 for SCL1_HOST_RD_ACK_INT interrupt.
 */
#define SDIO_SLC1_HOST_RD_ACK_INT_ENA1    (BIT(22))
#define SDIO_SLC1_HOST_RD_ACK_INT_ENA1_M  (SDIO_SLC1_HOST_RD_ACK_INT_ENA1_V << SDIO_SLC1_HOST_RD_ACK_INT_ENA1_S)
#define SDIO_SLC1_HOST_RD_ACK_INT_ENA1_V  0x00000001U
#define SDIO_SLC1_HOST_RD_ACK_INT_ENA1_S  22
/** SDIO_SLC1_WR_RETRY_DONE_INT_ENA1 : R/W; bitpos: [23]; default: 0;
 *  The interrupt enable bit1 for SCL1_WR_RETRY_DONE_INT interrupt.
 */
#define SDIO_SLC1_WR_RETRY_DONE_INT_ENA1    (BIT(23))
#define SDIO_SLC1_WR_RETRY_DONE_INT_ENA1_M  (SDIO_SLC1_WR_RETRY_DONE_INT_ENA1_V << SDIO_SLC1_WR_RETRY_DONE_INT_ENA1_S)
#define SDIO_SLC1_WR_RETRY_DONE_INT_ENA1_V  0x00000001U
#define SDIO_SLC1_WR_RETRY_DONE_INT_ENA1_S  23
/** SDIO_SLC1_TX_ERR_EOF_INT_ENA1 : R/W; bitpos: [24]; default: 0;
 *  The interrupt enable bit1 for SCL1_TX_ERR_EOF_INT interrupt.
 */
#define SDIO_SLC1_TX_ERR_EOF_INT_ENA1    (BIT(24))
#define SDIO_SLC1_TX_ERR_EOF_INT_ENA1_M  (SDIO_SLC1_TX_ERR_EOF_INT_ENA1_V << SDIO_SLC1_TX_ERR_EOF_INT_ENA1_S)
#define SDIO_SLC1_TX_ERR_EOF_INT_ENA1_V  0x00000001U
#define SDIO_SLC1_TX_ERR_EOF_INT_ENA1_S  24

/** SDIO_SLC_SLC0_TX_SHAREMEM_START_REG register
 *  Sdio slave DMA channel0 address range of receiving linked list register.
 */
#define SDIO_SLC_SLC0_TX_SHAREMEM_START_REG (DR_REG_SLC_BASE + 0x154)
/** SDIO_SDIO_SLC0_TX_SHAREMEM_START_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  The start address boundary of descriptor can be used to receive packet through dma
 *  channel0.
 */
#define SDIO_SDIO_SLC0_TX_SHAREMEM_START_ADDR    0xFFFFFFFFU
#define SDIO_SDIO_SLC0_TX_SHAREMEM_START_ADDR_M  (SDIO_SDIO_SLC0_TX_SHAREMEM_START_ADDR_V << SDIO_SDIO_SLC0_TX_SHAREMEM_START_ADDR_S)
#define SDIO_SDIO_SLC0_TX_SHAREMEM_START_ADDR_V  0xFFFFFFFFU
#define SDIO_SDIO_SLC0_TX_SHAREMEM_START_ADDR_S  0

/** SDIO_SLC_SLC0_TX_SHAREMEM_END_REG register
 *  Sdio slave DMA channel0 address range of receiving linked list register1.
 */
#define SDIO_SLC_SLC0_TX_SHAREMEM_END_REG (DR_REG_SLC_BASE + 0x158)
/** SDIO_SDIO_SLC0_TX_SHAREMEM_END_ADDR : R/W; bitpos: [31:0]; default: 4294967295;
 *  The end address boundary of descriptor can be used to receive packet through dma
 *  channel0.
 */
#define SDIO_SDIO_SLC0_TX_SHAREMEM_END_ADDR    0xFFFFFFFFU
#define SDIO_SDIO_SLC0_TX_SHAREMEM_END_ADDR_M  (SDIO_SDIO_SLC0_TX_SHAREMEM_END_ADDR_V << SDIO_SDIO_SLC0_TX_SHAREMEM_END_ADDR_S)
#define SDIO_SDIO_SLC0_TX_SHAREMEM_END_ADDR_V  0xFFFFFFFFU
#define SDIO_SDIO_SLC0_TX_SHAREMEM_END_ADDR_S  0

/** SDIO_SLC_SLC0_RX_SHAREMEM_START_REG register
 *  Sdio slave DMA channel0 address range of sending linked list register.
 */
#define SDIO_SLC_SLC0_RX_SHAREMEM_START_REG (DR_REG_SLC_BASE + 0x15c)
/** SDIO_SDIO_SLC0_RX_SHAREMEM_START_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  The start address boundary of descriptor can be used to send packet through dma
 *  channel0.
 */
#define SDIO_SDIO_SLC0_RX_SHAREMEM_START_ADDR    0xFFFFFFFFU
#define SDIO_SDIO_SLC0_RX_SHAREMEM_START_ADDR_M  (SDIO_SDIO_SLC0_RX_SHAREMEM_START_ADDR_V << SDIO_SDIO_SLC0_RX_SHAREMEM_START_ADDR_S)
#define SDIO_SDIO_SLC0_RX_SHAREMEM_START_ADDR_V  0xFFFFFFFFU
#define SDIO_SDIO_SLC0_RX_SHAREMEM_START_ADDR_S  0

/** SDIO_SLC_SLC0_RX_SHAREMEM_END_REG register
 *  Sdio slave DMA channel0 address range of sending linked list register1.
 */
#define SDIO_SLC_SLC0_RX_SHAREMEM_END_REG (DR_REG_SLC_BASE + 0x160)
/** SDIO_SDIO_SLC0_RX_SHAREMEM_END_ADDR : R/W; bitpos: [31:0]; default: 4294967295;
 *  The end address boundary of descriptor can be used to send packet through dma
 *  channel0.
 */
#define SDIO_SDIO_SLC0_RX_SHAREMEM_END_ADDR    0xFFFFFFFFU
#define SDIO_SDIO_SLC0_RX_SHAREMEM_END_ADDR_M  (SDIO_SDIO_SLC0_RX_SHAREMEM_END_ADDR_V << SDIO_SDIO_SLC0_RX_SHAREMEM_END_ADDR_S)
#define SDIO_SDIO_SLC0_RX_SHAREMEM_END_ADDR_V  0xFFFFFFFFU
#define SDIO_SDIO_SLC0_RX_SHAREMEM_END_ADDR_S  0

/** SDIO_SLC_SLC1_TX_SHAREMEM_START_REG register
 *  Sdio slave DMA channel1 address range of receiving linked list register.
 */
#define SDIO_SLC_SLC1_TX_SHAREMEM_START_REG (DR_REG_SLC_BASE + 0x164)
/** SDIO_SDIO_SLC1_TX_SHAREMEM_START_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  The start address boundary of descriptor can be used to receive packet through dma
 *  channel1.
 */
#define SDIO_SDIO_SLC1_TX_SHAREMEM_START_ADDR    0xFFFFFFFFU
#define SDIO_SDIO_SLC1_TX_SHAREMEM_START_ADDR_M  (SDIO_SDIO_SLC1_TX_SHAREMEM_START_ADDR_V << SDIO_SDIO_SLC1_TX_SHAREMEM_START_ADDR_S)
#define SDIO_SDIO_SLC1_TX_SHAREMEM_START_ADDR_V  0xFFFFFFFFU
#define SDIO_SDIO_SLC1_TX_SHAREMEM_START_ADDR_S  0

/** SDIO_SLC_SLC1_TX_SHAREMEM_END_REG register
 *  Sdio slave DMA channel1 address range of receiving linked list register1.
 */
#define SDIO_SLC_SLC1_TX_SHAREMEM_END_REG (DR_REG_SLC_BASE + 0x168)
/** SDIO_SDIO_SLC1_TX_SHAREMEM_END_ADDR : R/W; bitpos: [31:0]; default: 4294967295;
 *  The end address boundary of descriptor can be used to receive packet through dma
 *  channel1.
 */
#define SDIO_SDIO_SLC1_TX_SHAREMEM_END_ADDR    0xFFFFFFFFU
#define SDIO_SDIO_SLC1_TX_SHAREMEM_END_ADDR_M  (SDIO_SDIO_SLC1_TX_SHAREMEM_END_ADDR_V << SDIO_SDIO_SLC1_TX_SHAREMEM_END_ADDR_S)
#define SDIO_SDIO_SLC1_TX_SHAREMEM_END_ADDR_V  0xFFFFFFFFU
#define SDIO_SDIO_SLC1_TX_SHAREMEM_END_ADDR_S  0

/** SDIO_SLC_SLC1_RX_SHAREMEM_START_REG register
 *  Sdio slave DMA channel1 address range of sending linked list register.
 */
#define SDIO_SLC_SLC1_RX_SHAREMEM_START_REG (DR_REG_SLC_BASE + 0x16c)
/** SDIO_SDIO_SLC1_RX_SHAREMEM_START_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  The start address boundary of descriptor can be used to send packet through dma
 *  channel1.
 */
#define SDIO_SDIO_SLC1_RX_SHAREMEM_START_ADDR    0xFFFFFFFFU
#define SDIO_SDIO_SLC1_RX_SHAREMEM_START_ADDR_M  (SDIO_SDIO_SLC1_RX_SHAREMEM_START_ADDR_V << SDIO_SDIO_SLC1_RX_SHAREMEM_START_ADDR_S)
#define SDIO_SDIO_SLC1_RX_SHAREMEM_START_ADDR_V  0xFFFFFFFFU
#define SDIO_SDIO_SLC1_RX_SHAREMEM_START_ADDR_S  0

/** SDIO_SLC_SLC1_RX_SHAREMEM_END_REG register
 *  Sdio slave DMA channel1 address range of sending linked list register1.
 */
#define SDIO_SLC_SLC1_RX_SHAREMEM_END_REG (DR_REG_SLC_BASE + 0x170)
/** SDIO_SDIO_SLC1_RX_SHAREMEM_END_ADDR : R/W; bitpos: [31:0]; default: 4294967295;
 *  The end address boundary of descriptor can be used to send packet through dma
 *  channel1.
 */
#define SDIO_SDIO_SLC1_RX_SHAREMEM_END_ADDR    0xFFFFFFFFU
#define SDIO_SDIO_SLC1_RX_SHAREMEM_END_ADDR_M  (SDIO_SDIO_SLC1_RX_SHAREMEM_END_ADDR_V << SDIO_SDIO_SLC1_RX_SHAREMEM_END_ADDR_S)
#define SDIO_SDIO_SLC1_RX_SHAREMEM_END_ADDR_V  0xFFFFFFFFU
#define SDIO_SDIO_SLC1_RX_SHAREMEM_END_ADDR_S  0

/** SDIO_SLC_HDA_TX_SHAREMEM_START_REG register
 *  Sdio slave HAD receiving address range register.
 */
#define SDIO_SLC_HDA_TX_SHAREMEM_START_REG (DR_REG_SLC_BASE + 0x174)
/** SDIO_SDIO_HDA_TX_SHAREMEM_START_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Use 31 to 20 bit of this field as the higher 20 bits of 32-bit address for HAD
 *  receiving packets.
 */
#define SDIO_SDIO_HDA_TX_SHAREMEM_START_ADDR    0xFFFFFFFFU
#define SDIO_SDIO_HDA_TX_SHAREMEM_START_ADDR_M  (SDIO_SDIO_HDA_TX_SHAREMEM_START_ADDR_V << SDIO_SDIO_HDA_TX_SHAREMEM_START_ADDR_S)
#define SDIO_SDIO_HDA_TX_SHAREMEM_START_ADDR_V  0xFFFFFFFFU
#define SDIO_SDIO_HDA_TX_SHAREMEM_START_ADDR_S  0

/** SDIO_SLC_HDA_RX_SHAREMEM_START_REG register
 *  Sdio slave HAD sending address range register.
 */
#define SDIO_SLC_HDA_RX_SHAREMEM_START_REG (DR_REG_SLC_BASE + 0x178)
/** SDIO_SDIO_HDA_RX_SHAREMEM_START_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  Use 31 to 20 bit of this field as the higher 20 bits of 32-bit address for HAD
 *  sending packets.
 */
#define SDIO_SDIO_HDA_RX_SHAREMEM_START_ADDR    0xFFFFFFFFU
#define SDIO_SDIO_HDA_RX_SHAREMEM_START_ADDR_M  (SDIO_SDIO_HDA_RX_SHAREMEM_START_ADDR_V << SDIO_SDIO_HDA_RX_SHAREMEM_START_ADDR_S)
#define SDIO_SDIO_HDA_RX_SHAREMEM_START_ADDR_V  0xFFFFFFFFU
#define SDIO_SDIO_HDA_RX_SHAREMEM_START_ADDR_S  0

/** SDIO_SLC_BURST_LEN_REG register
 *  Sdio slave AHB burst type configuration register.
 */
#define SDIO_SLC_BURST_LEN_REG (DR_REG_SLC_BASE + 0x17c)
/** SDIO_SLC0_TXDATA_BURST_LEN : R/W; bitpos: [0]; default: 1;
 *  AHB burst type: 0-incr4,1-incr8
 */
#define SDIO_SLC0_TXDATA_BURST_LEN    (BIT(0))
#define SDIO_SLC0_TXDATA_BURST_LEN_M  (SDIO_SLC0_TXDATA_BURST_LEN_V << SDIO_SLC0_TXDATA_BURST_LEN_S)
#define SDIO_SLC0_TXDATA_BURST_LEN_V  0x00000001U
#define SDIO_SLC0_TXDATA_BURST_LEN_S  0
/** SDIO_SLC0_RXDATA_BURST_LEN : R/W; bitpos: [1]; default: 1;
 *  AHB burst type: 0-incr4,1-incr8
 */
#define SDIO_SLC0_RXDATA_BURST_LEN    (BIT(1))
#define SDIO_SLC0_RXDATA_BURST_LEN_M  (SDIO_SLC0_RXDATA_BURST_LEN_V << SDIO_SLC0_RXDATA_BURST_LEN_S)
#define SDIO_SLC0_RXDATA_BURST_LEN_V  0x00000001U
#define SDIO_SLC0_RXDATA_BURST_LEN_S  1
/** SDIO_SLC1_TXDATA_BURST_LEN : R/W; bitpos: [2]; default: 1;
 *  AHB burst type: 0-incr4,1-incr8
 */
#define SDIO_SLC1_TXDATA_BURST_LEN    (BIT(2))
#define SDIO_SLC1_TXDATA_BURST_LEN_M  (SDIO_SLC1_TXDATA_BURST_LEN_V << SDIO_SLC1_TXDATA_BURST_LEN_S)
#define SDIO_SLC1_TXDATA_BURST_LEN_V  0x00000001U
#define SDIO_SLC1_TXDATA_BURST_LEN_S  2
/** SDIO_SLC1_RXDATA_BURST_LEN : R/W; bitpos: [3]; default: 1;
 *  AHB burst type: 0-incr4,1-incr8
 */
#define SDIO_SLC1_RXDATA_BURST_LEN    (BIT(3))
#define SDIO_SLC1_RXDATA_BURST_LEN_M  (SDIO_SLC1_RXDATA_BURST_LEN_V << SDIO_SLC1_RXDATA_BURST_LEN_S)
#define SDIO_SLC1_RXDATA_BURST_LEN_V  0x00000001U
#define SDIO_SLC1_RXDATA_BURST_LEN_S  3

/** SDIO_SLC_DATE_REG register
 *  Sdio slave DMA version register.
 */
#define SDIO_SLC_DATE_REG (DR_REG_SLC_BASE + 0x1f8)
/** SDIO_SLC_DATE : R/W; bitpos: [31:0]; default: 554182400;
 *  Sdio slave DMA version register.
 */
#define SDIO_SLC_DATE    0xFFFFFFFFU
#define SDIO_SLC_DATE_M  (SDIO_SLC_DATE_V << SDIO_SLC_DATE_S)
#define SDIO_SLC_DATE_V  0xFFFFFFFFU
#define SDIO_SLC_DATE_S  0

/** SDIO_SLC_ID_REG register
 *  Sdio slave ID register.
 */
#define SDIO_SLC_ID_REG (DR_REG_SLC_BASE + 0x1fc)
/** SDIO_SLC_ID : R/W; bitpos: [31:0]; default: 256;
 *  reserved
 */
#define SDIO_SLC_ID    0xFFFFFFFFU
#define SDIO_SLC_ID_M  (SDIO_SLC_ID_V << SDIO_SLC_ID_S)
#define SDIO_SLC_ID_V  0xFFFFFFFFU
#define SDIO_SLC_ID_S  0

#ifdef __cplusplus
}
#endif
