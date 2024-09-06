/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration registers */
/** Type of slcconf0 register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc0_tx_rst : R/W; bitpos: [0]; default: 0;
         *  Set 1 to reset tx fsm in dma slc0.
         */
        uint32_t slc0_tx_rst:1;
        /** slc0_rx_rst : R/W; bitpos: [1]; default: 0;
         *  Set 1 to reset rx fsm in dma slc0.
         */
        uint32_t slc0_rx_rst:1;
        /** slc_ahbm_fifo_rst : R/W; bitpos: [2]; default: 0;
         *  reset the command fifo of AHB bus of sdio slave
         */
        uint32_t slc_ahbm_fifo_rst:1;
        /** slc_ahbm_rst : R/W; bitpos: [3]; default: 0;
         *  reset the AHB bus of sdio slave
         */
        uint32_t slc_ahbm_rst:1;
        /** slc0_tx_loop_test : R/W; bitpos: [4]; default: 0;
         *  owner control when slc1 writes back tx descriptor: 0- cpu is owner, 1-dma is owner.
         */
        uint32_t slc0_tx_loop_test:1;
        /** slc0_rx_loop_test : R/W; bitpos: [5]; default: 0;
         *  owner control when slc1 writes back rx descriptor: 0- cpu is owner, 1-dma is owner.
         */
        uint32_t slc0_rx_loop_test:1;
        /** slc0_rx_auto_wrback : R/W; bitpos: [6]; default: 0;
         *  Set 1 to enable change the owner bit of rx link descriptor
         */
        uint32_t slc0_rx_auto_wrback:1;
        /** slc0_rx_no_restart_clr : R/W; bitpos: [7]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_no_restart_clr:1;
        /** slc0_rxdscr_burst_en : R/W; bitpos: [8]; default: 1;
         *  0- AHB burst type is single when slave read rx-descriptor from memory through
         *  slc0,1-AHB burst type is not single when slave read rx-descriptor from memory
         *  through slc0
         */
        uint32_t slc0_rxdscr_burst_en:1;
        /** slc0_rxdata_burst_en : R/W; bitpos: [9]; default: 1;
         *  0- AHB burst type is single when slave receives data from memory,1-AHB burst type
         *  is not single when slave receives data from memory
         */
        uint32_t slc0_rxdata_burst_en:1;
        /** slc0_rxlink_auto_ret : R/W; bitpos: [10]; default: 1;
         *  enable the function that when host reading packet retries, slc1 will automatically
         *  jump to the start descriptor of the previous packet.
         */
        uint32_t slc0_rxlink_auto_ret:1;
        /** slc0_txlink_auto_ret : R/W; bitpos: [11]; default: 1;
         *  enable the function that when host sending packet retries, slc1 will automatically
         *  jump to the start descriptor of the previous packet.
         */
        uint32_t slc0_txlink_auto_ret:1;
        /** slc0_txdscr_burst_en : R/W; bitpos: [12]; default: 1;
         *  0- AHB burst type is single when slave read tx-descriptor from memory through
         *  slc0,1-AHB burst type is not single when slave read tx-descriptor from memory
         *  through slc0
         */
        uint32_t slc0_txdscr_burst_en:1;
        /** slc0_txdata_burst_en : R/W; bitpos: [13]; default: 1;
         *  0- AHB burst type is single when slave send data to memory,1-AHB burst type is not
         *  single when slave send data to memory
         */
        uint32_t slc0_txdata_burst_en:1;
        /** slc0_token_auto_clr : R/W; bitpos: [14]; default: 1;
         *  auto clear slc0_token1 enable
         */
        uint32_t slc0_token_auto_clr:1;
        /** slc0_token_sel : R/W; bitpos: [15]; default: 1;
         *  reserved
         */
        uint32_t slc0_token_sel:1;
        /** slc1_tx_rst : R/W; bitpos: [16]; default: 0;
         *  Set 1 to reset tx fsm in dma slc0.
         */
        uint32_t slc1_tx_rst:1;
        /** slc1_rx_rst : R/W; bitpos: [17]; default: 0;
         *  Set 1 to reset rx fsm in dma slc0.
         */
        uint32_t slc1_rx_rst:1;
        /** slc0_wr_retry_mask_en : R/W; bitpos: [18]; default: 1;
         *  reserved
         */
        uint32_t slc0_wr_retry_mask_en:1;
        /** slc1_wr_retry_mask_en : R/W; bitpos: [19]; default: 1;
         *  reserved
         */
        uint32_t slc1_wr_retry_mask_en:1;
        /** slc1_tx_loop_test : R/W; bitpos: [20]; default: 1;
         *  owner control when slc1 writes back tx descriptor: 0- cpu is owner, 1-dma is owner.
         */
        uint32_t slc1_tx_loop_test:1;
        /** slc1_rx_loop_test : R/W; bitpos: [21]; default: 1;
         *  owner control when slc1 writes back rx descriptor: 0- cpu is owner, 1-dma is owner.
         */
        uint32_t slc1_rx_loop_test:1;
        /** slc1_rx_auto_wrback : R/W; bitpos: [22]; default: 0;
         *  Set 1 to enable change the owner bit of rx link descriptor
         */
        uint32_t slc1_rx_auto_wrback:1;
        /** slc1_rx_no_restart_clr : R/W; bitpos: [23]; default: 0;
         *  ******* Description ***********
         */
        uint32_t slc1_rx_no_restart_clr:1;
        /** slc1_rxdscr_burst_en : R/W; bitpos: [24]; default: 1;
         *  0- AHB burst type is single when slave read rx-descriptor from memory through
         *  slc1,1-AHB burst type is not single when slave read rx-descriptor from memory
         *  through slc1
         */
        uint32_t slc1_rxdscr_burst_en:1;
        /** slc1_rxdata_burst_en : R/W; bitpos: [25]; default: 1;
         *  0- AHB burst type is single when slave receives data from memory,1-AHB burst type
         *  is not single when slave receives data from memory
         */
        uint32_t slc1_rxdata_burst_en:1;
        /** slc1_rxlink_auto_ret : R/W; bitpos: [26]; default: 1;
         *  enable the function that when host reading packet retries, slc1 will automatically
         *  jump to the start descriptor of the previous packet.
         */
        uint32_t slc1_rxlink_auto_ret:1;
        /** slc1_txlink_auto_ret : R/W; bitpos: [27]; default: 1;
         *  enable the function that when host sending packet retries, slc1 will automatically
         *  jump to the start descriptor of the previous packet.
         */
        uint32_t slc1_txlink_auto_ret:1;
        /** slc1_txdscr_burst_en : R/W; bitpos: [28]; default: 1;
         *  0- AHB burst type is single when slave read tx-descriptor from memory through
         *  slc1,1-AHB burst type is not single when slave read tx-descriptor from memory
         *  through slc1
         */
        uint32_t slc1_txdscr_burst_en:1;
        /** slc1_txdata_burst_en : R/W; bitpos: [29]; default: 1;
         *  0- AHB burst type is single when slave send data to memory,1-AHB burst type is not
         *  single when slave send data to memory
         */
        uint32_t slc1_txdata_burst_en:1;
        /** slc1_token_auto_clr : R/W; bitpos: [30]; default: 1;
         *  auto clear slc1_token1 enable
         */
        uint32_t slc1_token_auto_clr:1;
        /** slc1_token_sel : R/W; bitpos: [31]; default: 1;
         *  reserved
         */
        uint32_t slc1_token_sel:1;
    };
    uint32_t val;
} sdio_slcconf0_reg_t;

/** Type of slc0rxfifo_push register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc0_rxfifo_wdata : R/W; bitpos: [8:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_rxfifo_wdata:9;
        uint32_t reserved_9:7;
        /** slc0_rxfifo_push : R/W/SC; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t slc0_rxfifo_push:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} sdio_slc0rxfifo_push_reg_t;

/** Type of slc1rxfifo_push register
 *  reserved
 */
typedef union {
    struct {
        /** slc1_rxfifo_wdata : R/W; bitpos: [8:0]; default: 0;
         *  reserved
         */
        uint32_t slc1_rxfifo_wdata:9;
        uint32_t reserved_9:7;
        /** slc1_rxfifo_push : R/W/SC; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t slc1_rxfifo_push:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} sdio_slc1rxfifo_push_reg_t;

/** Type of slc0rx_link register
 *  reserved
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** slc0_rxlink_stop : R/W/SC; bitpos: [28]; default: 0;
         *  reserved
         */
        uint32_t slc0_rxlink_stop:1;
        /** slc0_rxlink_start : R/W/SC; bitpos: [29]; default: 0;
         *  reserved
         */
        uint32_t slc0_rxlink_start:1;
        /** slc0_rxlink_restart : R/W/SC; bitpos: [30]; default: 0;
         *  reserved
         */
        uint32_t slc0_rxlink_restart:1;
        /** slc0_rxlink_park : RO; bitpos: [31]; default: 1;
         *  reserved
         */
        uint32_t slc0_rxlink_park:1;
    };
    uint32_t val;
} sdio_slc0rx_link_reg_t;

/** Type of slc0rx_link_addr register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_rxlink_addr : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_rxlink_addr:32;
    };
    uint32_t val;
} sdio_slc0rx_link_addr_reg_t;

/** Type of slc0tx_link register
 *  reserved
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** slc0_txlink_stop : R/W/SC; bitpos: [28]; default: 0;
         *  reserved
         */
        uint32_t slc0_txlink_stop:1;
        /** slc0_txlink_start : R/W/SC; bitpos: [29]; default: 0;
         *  reserved
         */
        uint32_t slc0_txlink_start:1;
        /** slc0_txlink_restart : R/W/SC; bitpos: [30]; default: 0;
         *  reserved
         */
        uint32_t slc0_txlink_restart:1;
        /** slc0_txlink_park : RO; bitpos: [31]; default: 1;
         *  reserved
         */
        uint32_t slc0_txlink_park:1;
    };
    uint32_t val;
} sdio_slc0tx_link_reg_t;

/** Type of slc0tx_link_addr register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_txlink_addr : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_txlink_addr:32;
    };
    uint32_t val;
} sdio_slc0tx_link_addr_reg_t;

/** Type of slc1rx_link register
 *  reserved
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** slc1_bt_packet : R/W; bitpos: [20]; default: 1;
         *  reserved
         */
        uint32_t slc1_bt_packet:1;
        uint32_t reserved_21:7;
        /** slc1_rxlink_stop : R/W/SC; bitpos: [28]; default: 0;
         *  reserved
         */
        uint32_t slc1_rxlink_stop:1;
        /** slc1_rxlink_start : R/W/SC; bitpos: [29]; default: 0;
         *  reserved
         */
        uint32_t slc1_rxlink_start:1;
        /** slc1_rxlink_restart : R/W/SC; bitpos: [30]; default: 0;
         *  reserved
         */
        uint32_t slc1_rxlink_restart:1;
        /** slc1_rxlink_park : RO; bitpos: [31]; default: 1;
         *  reserved
         */
        uint32_t slc1_rxlink_park:1;
    };
    uint32_t val;
} sdio_slc1rx_link_reg_t;

/** Type of slc1rx_link_addr register
 *  reserved
 */
typedef union {
    struct {
        /** slc1_rxlink_addr : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc1_rxlink_addr:32;
    };
    uint32_t val;
} sdio_slc1rx_link_addr_reg_t;

/** Type of slc1tx_link register
 *  reserved
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** slc1_txlink_stop : R/W/SC; bitpos: [28]; default: 0;
         *  reserved
         */
        uint32_t slc1_txlink_stop:1;
        /** slc1_txlink_start : R/W/SC; bitpos: [29]; default: 0;
         *  reserved
         */
        uint32_t slc1_txlink_start:1;
        /** slc1_txlink_restart : R/W/SC; bitpos: [30]; default: 0;
         *  reserved
         */
        uint32_t slc1_txlink_restart:1;
        /** slc1_txlink_park : RO; bitpos: [31]; default: 1;
         *  reserved
         */
        uint32_t slc1_txlink_park:1;
    };
    uint32_t val;
} sdio_slc1tx_link_reg_t;

/** Type of slc1tx_link_addr register
 *  reserved
 */
typedef union {
    struct {
        /** slc1_txlink_addr : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc1_txlink_addr:32;
    };
    uint32_t val;
} sdio_slc1tx_link_addr_reg_t;

/** Type of slcintvec_tohost register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_tohost_intvec : WT; bitpos: [7:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_tohost_intvec:8;
        uint32_t reserved_8:8;
        /** slc1_tohost_intvec : WT; bitpos: [23:16]; default: 0;
         *  reserved
         */
        uint32_t slc1_tohost_intvec:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} sdio_slcintvec_tohost_reg_t;

/** Type of slc0token0 register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_token0_wdata : WT; bitpos: [11:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_token0_wdata:12;
        /** slc0_token0_wr : WT; bitpos: [12]; default: 0;
         *  reserved
         */
        uint32_t slc0_token0_wr:1;
        /** slc0_token0_inc : WT; bitpos: [13]; default: 0;
         *  reserved
         */
        uint32_t slc0_token0_inc:1;
        /** slc0_token0_inc_more : WT; bitpos: [14]; default: 0;
         *  reserved
         */
        uint32_t slc0_token0_inc_more:1;
        uint32_t reserved_15:1;
        /** slc0_token0 : RO; bitpos: [27:16]; default: 0;
         *  reserved
         */
        uint32_t slc0_token0:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} sdio_slc0token0_reg_t;

/** Type of slc0token1 register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_token1_wdata : WT; bitpos: [11:0]; default: 0;
         *  slc0 token1 wdata
         */
        uint32_t slc0_token1_wdata:12;
        /** slc0_token1_wr : WT; bitpos: [12]; default: 0;
         *  update slc0_token1_wdata into slc0 token1
         */
        uint32_t slc0_token1_wr:1;
        /** slc0_token1_inc : WT; bitpos: [13]; default: 0;
         *  slc0_token1 becomes to 1 when auto clear slc0_token1, else add 1 to slc0_token1
         */
        uint32_t slc0_token1_inc:1;
        /** slc0_token1_inc_more : WT; bitpos: [14]; default: 0;
         *  slc0_token1 becomes to slc0_token1_wdata when auto clear slc0_token1, else add
         *  slc0_token1_wdata to slc0_token1
         */
        uint32_t slc0_token1_inc_more:1;
        uint32_t reserved_15:1;
        /** slc0_token1 : RO; bitpos: [27:16]; default: 0;
         *  reserved
         */
        uint32_t slc0_token1:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} sdio_slc0token1_reg_t;

/** Type of slc1token0 register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc1_token0_wdata : WT; bitpos: [11:0]; default: 0;
         *  reserved
         */
        uint32_t slc1_token0_wdata:12;
        /** slc1_token0_wr : WT; bitpos: [12]; default: 0;
         *  reserved
         */
        uint32_t slc1_token0_wr:1;
        /** slc1_token0_inc : WT; bitpos: [13]; default: 0;
         *  Add 1 to  slc1_token0
         */
        uint32_t slc1_token0_inc:1;
        /** slc1_token0_inc_more : WT; bitpos: [14]; default: 0;
         *  Add  slc1_token0_wdata to  slc1_token0
         */
        uint32_t slc1_token0_inc_more:1;
        uint32_t reserved_15:1;
        /** slc1_token0 : RO; bitpos: [27:16]; default: 0;
         *  reserved
         */
        uint32_t slc1_token0:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} sdio_slc1token0_reg_t;

/** Type of slc1token1 register
 *  reserved
 */
typedef union {
    struct {
        /** slc1_token1_wdata : WT; bitpos: [11:0]; default: 0;
         *  reserved
         */
        uint32_t slc1_token1_wdata:12;
        /** slc1_token1_wr : WT; bitpos: [12]; default: 0;
         *  update slc1_token1_wdata into slc1 token1
         */
        uint32_t slc1_token1_wr:1;
        /** slc1_token1_inc : WT; bitpos: [13]; default: 0;
         *  reserved
         */
        uint32_t slc1_token1_inc:1;
        /** slc1_token1_inc_more : WT; bitpos: [14]; default: 0;
         *  reserved
         */
        uint32_t slc1_token1_inc_more:1;
        uint32_t reserved_15:1;
        /** slc1_token1 : RO; bitpos: [27:16]; default: 0;
         *  reserved
         */
        uint32_t slc1_token1:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} sdio_slc1token1_reg_t;

/** Type of slcconf1 register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_check_owner : R/W; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t slc0_check_owner:1;
        /** slc0_tx_check_sum_en : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_check_sum_en:1;
        /** slc0_rx_check_sum_en : R/W; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_check_sum_en:1;
        /** sdio_cmd_hold_en : R/W; bitpos: [3]; default: 1;
         *  reserved
         */
        uint32_t sdio_cmd_hold_en:1;
        /** slc0_len_auto_clr : R/W; bitpos: [4]; default: 1;
         *  reserved
         */
        uint32_t slc0_len_auto_clr:1;
        /** slc0_tx_stitch_en : R/W; bitpos: [5]; default: 1;
         *  reserved
         */
        uint32_t slc0_tx_stitch_en:1;
        /** slc0_rx_stitch_en : R/W; bitpos: [6]; default: 1;
         *  reserved
         */
        uint32_t slc0_rx_stitch_en:1;
        uint32_t reserved_7:9;
        /** slc1_check_owner : R/W; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t slc1_check_owner:1;
        /** slc1_tx_check_sum_en : R/W; bitpos: [17]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_check_sum_en:1;
        /** slc1_rx_check_sum_en : R/W; bitpos: [18]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_check_sum_en:1;
        /** host_int_level_sel : R/W; bitpos: [19]; default: 0;
         *  reserved
         */
        uint32_t host_int_level_sel:1;
        /** slc1_tx_stitch_en : R/W; bitpos: [20]; default: 1;
         *  reserved
         */
        uint32_t slc1_tx_stitch_en:1;
        /** slc1_rx_stitch_en : R/W; bitpos: [21]; default: 1;
         *  reserved
         */
        uint32_t slc1_rx_stitch_en:1;
        /** sdio_clk_en : R/W; bitpos: [22]; default: 0;
         *  reserved
         */
        uint32_t sdio_clk_en:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} sdio_slcconf1_reg_t;

/** Type of slcbridge_conf register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc_txeof_ena : R/W; bitpos: [5:0]; default: 32;
         *  reserved
         */
        uint32_t slc_txeof_ena:6;
        uint32_t reserved_6:2;
        /** slc_fifo_map_ena : R/W; bitpos: [11:8]; default: 7;
         *  reserved
         */
        uint32_t slc_fifo_map_ena:4;
        /** slc0_tx_dummy_mode : R/W; bitpos: [12]; default: 1;
         *  reserved
         */
        uint32_t slc0_tx_dummy_mode:1;
        /** slc_hda_map_128k : R/W; bitpos: [13]; default: 1;
         *  reserved
         */
        uint32_t slc_hda_map_128k:1;
        /** slc1_tx_dummy_mode : R/W; bitpos: [14]; default: 1;
         *  reserved
         */
        uint32_t slc1_tx_dummy_mode:1;
        uint32_t reserved_15:1;
        /** slc_tx_push_idle_num : R/W; bitpos: [31:16]; default: 10;
         *  reserved
         */
        uint32_t slc_tx_push_idle_num:16;
    };
    uint32_t val;
} sdio_slcbridge_conf_reg_t;

/** Type of slc0_to_eof_des_addr register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_to_eof_des_addr : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_to_eof_des_addr:32;
    };
    uint32_t val;
} sdio_slc0_to_eof_des_addr_reg_t;

/** Type of slc0_tx_eof_des_addr register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_tx_suc_eof_des_addr : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_suc_eof_des_addr:32;
    };
    uint32_t val;
} sdio_slc0_tx_eof_des_addr_reg_t;

/** Type of slc0_to_eof_bfr_des_addr register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_to_eof_bfr_des_addr : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_to_eof_bfr_des_addr:32;
    };
    uint32_t val;
} sdio_slc0_to_eof_bfr_des_addr_reg_t;

/** Type of slc1_to_eof_des_addr register
 *  reserved
 */
typedef union {
    struct {
        /** slc1_to_eof_des_addr : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc1_to_eof_des_addr:32;
    };
    uint32_t val;
} sdio_slc1_to_eof_des_addr_reg_t;

/** Type of slc1_tx_eof_des_addr register
 *  reserved
 */
typedef union {
    struct {
        /** slc1_tx_suc_eof_des_addr : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_suc_eof_des_addr:32;
    };
    uint32_t val;
} sdio_slc1_tx_eof_des_addr_reg_t;

/** Type of slc1_to_eof_bfr_des_addr register
 *  reserved
 */
typedef union {
    struct {
        /** slc1_to_eof_bfr_des_addr : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc1_to_eof_bfr_des_addr:32;
    };
    uint32_t val;
} sdio_slc1_to_eof_bfr_des_addr_reg_t;

/** Type of slc_rx_dscr_conf register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_token_no_replace : R/W; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t slc0_token_no_replace:1;
        /** slc0_infor_no_replace : R/W; bitpos: [1]; default: 1;
         *  reserved
         */
        uint32_t slc0_infor_no_replace:1;
        /** slc0_rx_fill_mode : R/W; bitpos: [2]; default: 0;
         *  slc0 rx pop end control: 0-automatically end when pop finish, 1- end when the next
         *  pop doesn't occur after 255 cycles since the current pop
         */
        uint32_t slc0_rx_fill_mode:1;
        /** slc0_rx_eof_mode : R/W; bitpos: [3]; default: 1;
         *  0-slc0 rx_push_eof, 1-slc0 rx_pop_eof
         */
        uint32_t slc0_rx_eof_mode:1;
        /** slc0_rx_fill_en : R/W; bitpos: [4]; default: 1;
         *  reserved
         */
        uint32_t slc0_rx_fill_en:1;
        /** slc0_rd_retry_threshold : R/W; bitpos: [15:5]; default: 128;
         *  reserved
         */
        uint32_t slc0_rd_retry_threshold:11;
        /** slc1_token_no_replace : R/W; bitpos: [16]; default: 1;
         *  reserved
         */
        uint32_t slc1_token_no_replace:1;
        /** slc1_infor_no_replace : R/W; bitpos: [17]; default: 1;
         *  reserved
         */
        uint32_t slc1_infor_no_replace:1;
        /** slc1_rx_fill_mode : R/W; bitpos: [18]; default: 0;
         *  slc1 rx pop end control: 0-automatically end when pop finish, 1- end when the next
         *  pop doesn't occur after 255 cycles since the current pop
         */
        uint32_t slc1_rx_fill_mode:1;
        /** slc1_rx_eof_mode : R/W; bitpos: [19]; default: 1;
         *  0-slc1 rx_push_eof, 1-slc1 rx_pop_eof
         */
        uint32_t slc1_rx_eof_mode:1;
        /** slc1_rx_fill_en : R/W; bitpos: [20]; default: 1;
         *  reserved
         */
        uint32_t slc1_rx_fill_en:1;
        /** slc1_rd_retry_threshold : R/W; bitpos: [31:21]; default: 128;
         *  reserved
         */
        uint32_t slc1_rd_retry_threshold:11;
    };
    uint32_t val;
} sdio_slc_rx_dscr_conf_reg_t;

/** Type of slc_tx_dscr_conf register
 *  reserved
 */
typedef union {
    struct {
        /** slc_wr_retry_threshold : R/W; bitpos: [10:0]; default: 128;
         *  reserved
         */
        uint32_t slc_wr_retry_threshold:11;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} sdio_slc_tx_dscr_conf_reg_t;

/** Type of slc0_len_conf register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_len_wdata : WT; bitpos: [19:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_len_wdata:20;
        /** slc0_len_wr : WT; bitpos: [20]; default: 0;
         *  reserved
         */
        uint32_t slc0_len_wr:1;
        /** slc0_len_inc : WT; bitpos: [21]; default: 0;
         *  reserved
         */
        uint32_t slc0_len_inc:1;
        /** slc0_len_inc_more : WT; bitpos: [22]; default: 0;
         *  reserved
         */
        uint32_t slc0_len_inc_more:1;
        /** slc0_rx_packet_load_en : WT; bitpos: [23]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_packet_load_en:1;
        /** slc0_tx_packet_load_en : WT; bitpos: [24]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_packet_load_en:1;
        /** slc0_rx_get_used_dscr : WT; bitpos: [25]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_get_used_dscr:1;
        /** slc0_tx_get_used_dscr : WT; bitpos: [26]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_get_used_dscr:1;
        /** slc0_rx_new_pkt_ind : RO; bitpos: [27]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_new_pkt_ind:1;
        /** slc0_tx_new_pkt_ind : RO; bitpos: [28]; default: 1;
         *  reserved
         */
        uint32_t slc0_tx_new_pkt_ind:1;
        /** slc0_rx_packet_load_en_st : R/WTC/SC; bitpos: [29]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_packet_load_en_st:1;
        /** slc0_tx_packet_load_en_st : R/WTC/SC; bitpos: [30]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_packet_load_en_st:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} sdio_slc0_len_conf_reg_t;

/** Type of slc0_txpkt_h_dscr register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_tx_pkt_h_dscr_addr : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_pkt_h_dscr_addr:32;
    };
    uint32_t val;
} sdio_slc0_txpkt_h_dscr_reg_t;

/** Type of slc0_txpkt_e_dscr register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_tx_pkt_e_dscr_addr : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_pkt_e_dscr_addr:32;
    };
    uint32_t val;
} sdio_slc0_txpkt_e_dscr_reg_t;

/** Type of slc0_rxpkt_h_dscr register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_rx_pkt_h_dscr_addr : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_pkt_h_dscr_addr:32;
    };
    uint32_t val;
} sdio_slc0_rxpkt_h_dscr_reg_t;

/** Type of slc0_rxpkt_e_dscr register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_rx_pkt_e_dscr_addr : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_pkt_e_dscr_addr:32;
    };
    uint32_t val;
} sdio_slc0_rxpkt_e_dscr_reg_t;

/** Type of slc0_txpktu_h_dscr register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_tx_pkt_start_dscr_addr : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_pkt_start_dscr_addr:32;
    };
    uint32_t val;
} sdio_slc0_txpktu_h_dscr_reg_t;

/** Type of slc0_txpktu_e_dscr register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_tx_pkt_end_dscr_addr : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_pkt_end_dscr_addr:32;
    };
    uint32_t val;
} sdio_slc0_txpktu_e_dscr_reg_t;

/** Type of slc0_rxpktu_h_dscr register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_rx_pkt_start_dscr_addr : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_pkt_start_dscr_addr:32;
    };
    uint32_t val;
} sdio_slc0_rxpktu_h_dscr_reg_t;

/** Type of slc0_rxpktu_e_dscr register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_rx_pkt_end_dscr_addr : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_pkt_end_dscr_addr:32;
    };
    uint32_t val;
} sdio_slc0_rxpktu_e_dscr_reg_t;

/** Type of slc_seq_position register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_seq_position : R/W; bitpos: [7:0]; default: 9;
         *  reserved
         */
        uint32_t slc0_seq_position:8;
        /** slc1_seq_position : R/W; bitpos: [15:8]; default: 5;
         *  reserved
         */
        uint32_t slc1_seq_position:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} sdio_slc_seq_position_reg_t;

/** Type of slc0_dscr_rec_conf register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_rx_dscr_rec_lim : R/W; bitpos: [9:0]; default: 1023;
         *  reserved
         */
        uint32_t slc0_rx_dscr_rec_lim:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} sdio_slc0_dscr_rec_conf_reg_t;

/** Type of slc_sdio_crc_st1 register
 *  reserved
 */
typedef union {
    struct {
        /** cmd_crc_err_cnt : RO; bitpos: [7:0]; default: 0;
         *  reserved
         */
        uint32_t cmd_crc_err_cnt:8;
        uint32_t reserved_8:23;
        /** err_cnt_clr : R/W; bitpos: [31]; default: 0;
         *  reserved
         */
        uint32_t err_cnt_clr:1;
    };
    uint32_t val;
} sdio_slc_sdio_crc_st1_reg_t;

/** Type of slc0_len_lim_conf register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc0_len_lim : R/W; bitpos: [19:0]; default: 21504;
         *  reserved
         */
        uint32_t slc0_len_lim:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} sdio_slc0_len_lim_conf_reg_t;

/** Type of slc0_tx_sharemem_start register
 *  reserved
 */
typedef union {
    struct {
        /** sdio_slc0_tx_sharemem_start_addr : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t sdio_slc0_tx_sharemem_start_addr:32;
    };
    uint32_t val;
} sdio_slc0_tx_sharemem_start_reg_t;

/** Type of slc0_tx_sharemem_end register
 *  reserved
 */
typedef union {
    struct {
        /** sdio_slc0_tx_sharemem_end_addr : R/W; bitpos: [31:0]; default: 4294967295;
         *  reserved
         */
        uint32_t sdio_slc0_tx_sharemem_end_addr:32;
    };
    uint32_t val;
} sdio_slc0_tx_sharemem_end_reg_t;

/** Type of slc0_rx_sharemem_start register
 *  reserved
 */
typedef union {
    struct {
        /** sdio_slc0_rx_sharemem_start_addr : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t sdio_slc0_rx_sharemem_start_addr:32;
    };
    uint32_t val;
} sdio_slc0_rx_sharemem_start_reg_t;

/** Type of slc0_rx_sharemem_end register
 *  reserved
 */
typedef union {
    struct {
        /** sdio_slc0_rx_sharemem_end_addr : R/W; bitpos: [31:0]; default: 4294967295;
         *  reserved
         */
        uint32_t sdio_slc0_rx_sharemem_end_addr:32;
    };
    uint32_t val;
} sdio_slc0_rx_sharemem_end_reg_t;

/** Type of slc1_tx_sharemem_start register
 *  reserved
 */
typedef union {
    struct {
        /** sdio_slc1_tx_sharemem_start_addr : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t sdio_slc1_tx_sharemem_start_addr:32;
    };
    uint32_t val;
} sdio_slc1_tx_sharemem_start_reg_t;

/** Type of slc1_tx_sharemem_end register
 *  reserved
 */
typedef union {
    struct {
        /** sdio_slc1_tx_sharemem_end_addr : R/W; bitpos: [31:0]; default: 4294967295;
         *  reserved
         */
        uint32_t sdio_slc1_tx_sharemem_end_addr:32;
    };
    uint32_t val;
} sdio_slc1_tx_sharemem_end_reg_t;

/** Type of slc1_rx_sharemem_start register
 *  reserved
 */
typedef union {
    struct {
        /** sdio_slc1_rx_sharemem_start_addr : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t sdio_slc1_rx_sharemem_start_addr:32;
    };
    uint32_t val;
} sdio_slc1_rx_sharemem_start_reg_t;

/** Type of slc1_rx_sharemem_end register
 *  reserved
 */
typedef union {
    struct {
        /** sdio_slc1_rx_sharemem_end_addr : R/W; bitpos: [31:0]; default: 4294967295;
         *  reserved
         */
        uint32_t sdio_slc1_rx_sharemem_end_addr:32;
    };
    uint32_t val;
} sdio_slc1_rx_sharemem_end_reg_t;

/** Type of hda_tx_sharemem_start register
 *  reserved
 */
typedef union {
    struct {
        /** sdio_hda_tx_sharemem_start_addr : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t sdio_hda_tx_sharemem_start_addr:32;
    };
    uint32_t val;
} sdio_hda_tx_sharemem_start_reg_t;

/** Type of hda_rx_sharemem_start register
 *  reserved
 */
typedef union {
    struct {
        /** sdio_hda_rx_sharemem_start_addr : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t sdio_hda_rx_sharemem_start_addr:32;
    };
    uint32_t val;
} sdio_hda_rx_sharemem_start_reg_t;

/** Type of slc_burst_len register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_txdata_burst_len : R/W; bitpos: [0]; default: 1;
         *  0-incr4,1-incr8
         */
        uint32_t slc0_txdata_burst_len:1;
        /** slc0_rxdata_burst_len : R/W; bitpos: [1]; default: 1;
         *  0-incr4,1-incr8
         */
        uint32_t slc0_rxdata_burst_len:1;
        /** slc1_txdata_burst_len : R/W; bitpos: [2]; default: 1;
         *  0-incr4,1-incr8
         */
        uint32_t slc1_txdata_burst_len:1;
        /** slc1_rxdata_burst_len : R/W; bitpos: [3]; default: 1;
         *  0-incr4,1-incr8
         */
        uint32_t slc1_rxdata_burst_len:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} sdio_slc_burst_len_reg_t;

/** Type of slcid register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc_id : R/W; bitpos: [31:0]; default: 256;
         *  reserved
         */
        uint32_t slc_id:32;
    };
    uint32_t val;
} sdio_slcid_reg_t;


/** Group: Interrupt registers */
/** Type of slc0int_raw register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc_frhost_bit0_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit0_int_raw:1;
        /** slc_frhost_bit1_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit1_int_raw:1;
        /** slc_frhost_bit2_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit2_int_raw:1;
        /** slc_frhost_bit3_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit3_int_raw:1;
        /** slc_frhost_bit4_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit4_int_raw:1;
        /** slc_frhost_bit5_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit5_int_raw:1;
        /** slc_frhost_bit6_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit6_int_raw:1;
        /** slc_frhost_bit7_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit7_int_raw:1;
        /** slc0_rx_start_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_start_int_raw:1;
        /** slc0_tx_start_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_start_int_raw:1;
        /** slc0_rx_udf_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_udf_int_raw:1;
        /** slc0_tx_ovf_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_ovf_int_raw:1;
        /** slc0_token0_1to0_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  reserved
         */
        uint32_t slc0_token0_1to0_int_raw:1;
        /** slc0_token1_1to0_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  reserved
         */
        uint32_t slc0_token1_1to0_int_raw:1;
        /** slc0_tx_done_int_raw : R/WTC/SS; bitpos: [14]; default: 0;
         *  The raw interrupt bit of slc0 finishing receiving data to one buffer
         */
        uint32_t slc0_tx_done_int_raw:1;
        /** slc0_tx_suc_eof_int_raw : R/WTC/SS; bitpos: [15]; default: 0;
         *  The raw interrupt bit of slc0 finishing receiving data
         */
        uint32_t slc0_tx_suc_eof_int_raw:1;
        /** slc0_rx_done_int_raw : R/WTC/SS; bitpos: [16]; default: 0;
         *  The raw interrupt bit of slc0 finishing sending data from one buffer
         */
        uint32_t slc0_rx_done_int_raw:1;
        /** slc0_rx_eof_int_raw : R/WTC/SS; bitpos: [17]; default: 0;
         *  The raw interrupt bit of slc0 finishing sending data
         */
        uint32_t slc0_rx_eof_int_raw:1;
        /** slc0_tohost_int_raw : R/WTC/SS; bitpos: [18]; default: 0;
         *  reserved
         */
        uint32_t slc0_tohost_int_raw:1;
        /** slc0_tx_dscr_err_int_raw : R/WTC/SS; bitpos: [19]; default: 0;
         *  The raw interrupt bit of slc0 tx link descriptor error
         */
        uint32_t slc0_tx_dscr_err_int_raw:1;
        /** slc0_rx_dscr_err_int_raw : R/WTC/SS; bitpos: [20]; default: 0;
         *  The raw interrupt bit of slc0 rx link descriptor error
         */
        uint32_t slc0_rx_dscr_err_int_raw:1;
        /** slc0_tx_dscr_empty_int_raw : R/WTC/SS; bitpos: [21]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_dscr_empty_int_raw:1;
        /** slc0_host_rd_ack_int_raw : R/WTC/SS; bitpos: [22]; default: 0;
         *  reserved
         */
        uint32_t slc0_host_rd_ack_int_raw:1;
        /** slc0_wr_retry_done_int_raw : R/WTC/SS; bitpos: [23]; default: 0;
         *  reserved
         */
        uint32_t slc0_wr_retry_done_int_raw:1;
        /** slc0_tx_err_eof_int_raw : R/WTC/SS; bitpos: [24]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_err_eof_int_raw:1;
        /** cmd_dtc_int_raw : R/WTC/SS; bitpos: [25]; default: 0;
         *  reserved
         */
        uint32_t cmd_dtc_int_raw:1;
        /** slc0_rx_quick_eof_int_raw : R/WTC/SS; bitpos: [26]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_quick_eof_int_raw:1;
        /** slc0_host_pop_eof_err_int_raw : R/WTC/SS; bitpos: [27]; default: 0;
         *  reserved
         */
        uint32_t slc0_host_pop_eof_err_int_raw:1;
        /** hda_recv_done_int_raw : R/WTC/SS; bitpos: [28]; default: 0;
         *  reserved
         */
        uint32_t hda_recv_done_int_raw:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} sdio_slc0int_raw_reg_t;

/** Type of slc0int_st register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc_frhost_bit0_int_st : RO; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit0_int_st:1;
        /** slc_frhost_bit1_int_st : RO; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit1_int_st:1;
        /** slc_frhost_bit2_int_st : RO; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit2_int_st:1;
        /** slc_frhost_bit3_int_st : RO; bitpos: [3]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit3_int_st:1;
        /** slc_frhost_bit4_int_st : RO; bitpos: [4]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit4_int_st:1;
        /** slc_frhost_bit5_int_st : RO; bitpos: [5]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit5_int_st:1;
        /** slc_frhost_bit6_int_st : RO; bitpos: [6]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit6_int_st:1;
        /** slc_frhost_bit7_int_st : RO; bitpos: [7]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit7_int_st:1;
        /** slc0_rx_start_int_st : RO; bitpos: [8]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_start_int_st:1;
        /** slc0_tx_start_int_st : RO; bitpos: [9]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_start_int_st:1;
        /** slc0_rx_udf_int_st : RO; bitpos: [10]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_udf_int_st:1;
        /** slc0_tx_ovf_int_st : RO; bitpos: [11]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_ovf_int_st:1;
        /** slc0_token0_1to0_int_st : RO; bitpos: [12]; default: 0;
         *  reserved
         */
        uint32_t slc0_token0_1to0_int_st:1;
        /** slc0_token1_1to0_int_st : RO; bitpos: [13]; default: 0;
         *  reserved
         */
        uint32_t slc0_token1_1to0_int_st:1;
        /** slc0_tx_done_int_st : RO; bitpos: [14]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_done_int_st:1;
        /** slc0_tx_suc_eof_int_st : RO; bitpos: [15]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_suc_eof_int_st:1;
        /** slc0_rx_done_int_st : RO; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_done_int_st:1;
        /** slc0_rx_eof_int_st : RO; bitpos: [17]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_eof_int_st:1;
        /** slc0_tohost_int_st : RO; bitpos: [18]; default: 0;
         *  reserved
         */
        uint32_t slc0_tohost_int_st:1;
        /** slc0_tx_dscr_err_int_st : RO; bitpos: [19]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_dscr_err_int_st:1;
        /** slc0_rx_dscr_err_int_st : RO; bitpos: [20]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_dscr_err_int_st:1;
        /** slc0_tx_dscr_empty_int_st : RO; bitpos: [21]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_dscr_empty_int_st:1;
        /** slc0_host_rd_ack_int_st : RO; bitpos: [22]; default: 0;
         *  reserved
         */
        uint32_t slc0_host_rd_ack_int_st:1;
        /** slc0_wr_retry_done_int_st : RO; bitpos: [23]; default: 0;
         *  reserved
         */
        uint32_t slc0_wr_retry_done_int_st:1;
        /** slc0_tx_err_eof_int_st : RO; bitpos: [24]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_err_eof_int_st:1;
        /** cmd_dtc_int_st : RO; bitpos: [25]; default: 0;
         *  reserved
         */
        uint32_t cmd_dtc_int_st:1;
        /** slc0_rx_quick_eof_int_st : RO; bitpos: [26]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_quick_eof_int_st:1;
        /** slc0_host_pop_eof_err_int_st : RO; bitpos: [27]; default: 0;
         *  reserved
         */
        uint32_t slc0_host_pop_eof_err_int_st:1;
        /** hda_recv_done_int_st : RO; bitpos: [28]; default: 0;
         *  reserved
         */
        uint32_t hda_recv_done_int_st:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} sdio_slc0int_st_reg_t;

/** Type of slc0int_ena register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc_frhost_bit0_int_ena : R/W; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit0_int_ena:1;
        /** slc_frhost_bit1_int_ena : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit1_int_ena:1;
        /** slc_frhost_bit2_int_ena : R/W; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit2_int_ena:1;
        /** slc_frhost_bit3_int_ena : R/W; bitpos: [3]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit3_int_ena:1;
        /** slc_frhost_bit4_int_ena : R/W; bitpos: [4]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit4_int_ena:1;
        /** slc_frhost_bit5_int_ena : R/W; bitpos: [5]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit5_int_ena:1;
        /** slc_frhost_bit6_int_ena : R/W; bitpos: [6]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit6_int_ena:1;
        /** slc_frhost_bit7_int_ena : R/W; bitpos: [7]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit7_int_ena:1;
        /** slc0_rx_start_int_ena : R/W; bitpos: [8]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_start_int_ena:1;
        /** slc0_tx_start_int_ena : R/W; bitpos: [9]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_start_int_ena:1;
        /** slc0_rx_udf_int_ena : R/W; bitpos: [10]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_udf_int_ena:1;
        /** slc0_tx_ovf_int_ena : R/W; bitpos: [11]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_ovf_int_ena:1;
        /** slc0_token0_1to0_int_ena : R/W; bitpos: [12]; default: 0;
         *  reserved
         */
        uint32_t slc0_token0_1to0_int_ena:1;
        /** slc0_token1_1to0_int_ena : R/W; bitpos: [13]; default: 0;
         *  reserved
         */
        uint32_t slc0_token1_1to0_int_ena:1;
        /** slc0_tx_done_int_ena : R/W; bitpos: [14]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_done_int_ena:1;
        /** slc0_tx_suc_eof_int_ena : R/W; bitpos: [15]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_suc_eof_int_ena:1;
        /** slc0_rx_done_int_ena : R/W; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_done_int_ena:1;
        /** slc0_rx_eof_int_ena : R/W; bitpos: [17]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_eof_int_ena:1;
        /** slc0_tohost_int_ena : R/W; bitpos: [18]; default: 0;
         *  reserved
         */
        uint32_t slc0_tohost_int_ena:1;
        /** slc0_tx_dscr_err_int_ena : R/W; bitpos: [19]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_dscr_err_int_ena:1;
        /** slc0_rx_dscr_err_int_ena : R/W; bitpos: [20]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_dscr_err_int_ena:1;
        /** slc0_tx_dscr_empty_int_ena : R/W; bitpos: [21]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_dscr_empty_int_ena:1;
        /** slc0_host_rd_ack_int_ena : R/W; bitpos: [22]; default: 0;
         *  reserved
         */
        uint32_t slc0_host_rd_ack_int_ena:1;
        /** slc0_wr_retry_done_int_ena : R/W; bitpos: [23]; default: 0;
         *  reserved
         */
        uint32_t slc0_wr_retry_done_int_ena:1;
        /** slc0_tx_err_eof_int_ena : R/W; bitpos: [24]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_err_eof_int_ena:1;
        /** cmd_dtc_int_ena : R/W; bitpos: [25]; default: 0;
         *  reserved
         */
        uint32_t cmd_dtc_int_ena:1;
        /** slc0_rx_quick_eof_int_ena : R/W; bitpos: [26]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_quick_eof_int_ena:1;
        /** slc0_host_pop_eof_err_int_ena : R/W; bitpos: [27]; default: 0;
         *  reserved
         */
        uint32_t slc0_host_pop_eof_err_int_ena:1;
        /** hda_recv_done_int_ena : R/W; bitpos: [28]; default: 0;
         *  reserved
         */
        uint32_t hda_recv_done_int_ena:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} sdio_slc0int_ena_reg_t;

/** Type of slc0int_clr register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc_frhost_bit0_int_clr : WT; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit0_int_clr:1;
        /** slc_frhost_bit1_int_clr : WT; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit1_int_clr:1;
        /** slc_frhost_bit2_int_clr : WT; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit2_int_clr:1;
        /** slc_frhost_bit3_int_clr : WT; bitpos: [3]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit3_int_clr:1;
        /** slc_frhost_bit4_int_clr : WT; bitpos: [4]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit4_int_clr:1;
        /** slc_frhost_bit5_int_clr : WT; bitpos: [5]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit5_int_clr:1;
        /** slc_frhost_bit6_int_clr : WT; bitpos: [6]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit6_int_clr:1;
        /** slc_frhost_bit7_int_clr : WT; bitpos: [7]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit7_int_clr:1;
        /** slc0_rx_start_int_clr : WT; bitpos: [8]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_start_int_clr:1;
        /** slc0_tx_start_int_clr : WT; bitpos: [9]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_start_int_clr:1;
        /** slc0_rx_udf_int_clr : WT; bitpos: [10]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_udf_int_clr:1;
        /** slc0_tx_ovf_int_clr : WT; bitpos: [11]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_ovf_int_clr:1;
        /** slc0_token0_1to0_int_clr : WT; bitpos: [12]; default: 0;
         *  reserved
         */
        uint32_t slc0_token0_1to0_int_clr:1;
        /** slc0_token1_1to0_int_clr : WT; bitpos: [13]; default: 0;
         *  reserved
         */
        uint32_t slc0_token1_1to0_int_clr:1;
        /** slc0_tx_done_int_clr : WT; bitpos: [14]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_done_int_clr:1;
        /** slc0_tx_suc_eof_int_clr : WT; bitpos: [15]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_suc_eof_int_clr:1;
        /** slc0_rx_done_int_clr : WT; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_done_int_clr:1;
        /** slc0_rx_eof_int_clr : WT; bitpos: [17]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_eof_int_clr:1;
        /** slc0_tohost_int_clr : WT; bitpos: [18]; default: 0;
         *  reserved
         */
        uint32_t slc0_tohost_int_clr:1;
        /** slc0_tx_dscr_err_int_clr : WT; bitpos: [19]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_dscr_err_int_clr:1;
        /** slc0_rx_dscr_err_int_clr : WT; bitpos: [20]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_dscr_err_int_clr:1;
        /** slc0_tx_dscr_empty_int_clr : WT; bitpos: [21]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_dscr_empty_int_clr:1;
        /** slc0_host_rd_ack_int_clr : WT; bitpos: [22]; default: 0;
         *  reserved
         */
        uint32_t slc0_host_rd_ack_int_clr:1;
        /** slc0_wr_retry_done_int_clr : WT; bitpos: [23]; default: 0;
         *  reserved
         */
        uint32_t slc0_wr_retry_done_int_clr:1;
        /** slc0_tx_err_eof_int_clr : WT; bitpos: [24]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_err_eof_int_clr:1;
        /** cmd_dtc_int_clr : WT; bitpos: [25]; default: 0;
         *  reserved
         */
        uint32_t cmd_dtc_int_clr:1;
        /** slc0_rx_quick_eof_int_clr : WT; bitpos: [26]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_quick_eof_int_clr:1;
        /** slc0_host_pop_eof_err_int_clr : WT; bitpos: [27]; default: 0;
         *  reserved
         */
        uint32_t slc0_host_pop_eof_err_int_clr:1;
        /** hda_recv_done_int_clr : WT; bitpos: [28]; default: 0;
         *  reserved
         */
        uint32_t hda_recv_done_int_clr:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} sdio_slc0int_clr_reg_t;

/** Type of slc1int_raw register
 *  reserved
 */
typedef union {
    struct {
        /** slc_frhost_bit8_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit8_int_raw:1;
        /** slc_frhost_bit9_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit9_int_raw:1;
        /** slc_frhost_bit10_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit10_int_raw:1;
        /** slc_frhost_bit11_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit11_int_raw:1;
        /** slc_frhost_bit12_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit12_int_raw:1;
        /** slc_frhost_bit13_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit13_int_raw:1;
        /** slc_frhost_bit14_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit14_int_raw:1;
        /** slc_frhost_bit15_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit15_int_raw:1;
        /** slc1_rx_start_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_start_int_raw:1;
        /** slc1_tx_start_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_start_int_raw:1;
        /** slc1_rx_udf_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_udf_int_raw:1;
        /** slc1_tx_ovf_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_ovf_int_raw:1;
        /** slc1_token0_1to0_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  reserved
         */
        uint32_t slc1_token0_1to0_int_raw:1;
        /** slc1_token1_1to0_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  reserved
         */
        uint32_t slc1_token1_1to0_int_raw:1;
        /** slc1_tx_done_int_raw : R/WTC/SS; bitpos: [14]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_done_int_raw:1;
        /** slc1_tx_suc_eof_int_raw : R/WTC/SS; bitpos: [15]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_suc_eof_int_raw:1;
        /** slc1_rx_done_int_raw : R/WTC/SS; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_done_int_raw:1;
        /** slc1_rx_eof_int_raw : R/WTC/SS; bitpos: [17]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_eof_int_raw:1;
        /** slc1_tohost_int_raw : R/WTC/SS; bitpos: [18]; default: 0;
         *  reserved
         */
        uint32_t slc1_tohost_int_raw:1;
        /** slc1_tx_dscr_err_int_raw : R/WTC/SS; bitpos: [19]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_dscr_err_int_raw:1;
        /** slc1_rx_dscr_err_int_raw : R/WTC/SS; bitpos: [20]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_dscr_err_int_raw:1;
        /** slc1_tx_dscr_empty_int_raw : R/WTC/SS; bitpos: [21]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_dscr_empty_int_raw:1;
        /** slc1_host_rd_ack_int_raw : R/WTC/SS; bitpos: [22]; default: 0;
         *  reserved
         */
        uint32_t slc1_host_rd_ack_int_raw:1;
        /** slc1_wr_retry_done_int_raw : R/WTC/SS; bitpos: [23]; default: 0;
         *  reserved
         */
        uint32_t slc1_wr_retry_done_int_raw:1;
        /** slc1_tx_err_eof_int_raw : R/WTC/SS; bitpos: [24]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_err_eof_int_raw:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} sdio_slc1int_raw_reg_t;

/** Type of slc1int_st register
 *  reserved
 */
typedef union {
    struct {
        /** slc_frhost_bit8_int_st : RO; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit8_int_st:1;
        /** slc_frhost_bit9_int_st : RO; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit9_int_st:1;
        /** slc_frhost_bit10_int_st : RO; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit10_int_st:1;
        /** slc_frhost_bit11_int_st : RO; bitpos: [3]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit11_int_st:1;
        /** slc_frhost_bit12_int_st : RO; bitpos: [4]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit12_int_st:1;
        /** slc_frhost_bit13_int_st : RO; bitpos: [5]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit13_int_st:1;
        /** slc_frhost_bit14_int_st : RO; bitpos: [6]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit14_int_st:1;
        /** slc_frhost_bit15_int_st : RO; bitpos: [7]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit15_int_st:1;
        /** slc1_rx_start_int_st : RO; bitpos: [8]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_start_int_st:1;
        /** slc1_tx_start_int_st : RO; bitpos: [9]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_start_int_st:1;
        /** slc1_rx_udf_int_st : RO; bitpos: [10]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_udf_int_st:1;
        /** slc1_tx_ovf_int_st : RO; bitpos: [11]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_ovf_int_st:1;
        /** slc1_token0_1to0_int_st : RO; bitpos: [12]; default: 0;
         *  reserved
         */
        uint32_t slc1_token0_1to0_int_st:1;
        /** slc1_token1_1to0_int_st : RO; bitpos: [13]; default: 0;
         *  reserved
         */
        uint32_t slc1_token1_1to0_int_st:1;
        /** slc1_tx_done_int_st : RO; bitpos: [14]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_done_int_st:1;
        /** slc1_tx_suc_eof_int_st : RO; bitpos: [15]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_suc_eof_int_st:1;
        /** slc1_rx_done_int_st : RO; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_done_int_st:1;
        /** slc1_rx_eof_int_st : RO; bitpos: [17]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_eof_int_st:1;
        /** slc1_tohost_int_st : RO; bitpos: [18]; default: 0;
         *  reserved
         */
        uint32_t slc1_tohost_int_st:1;
        /** slc1_tx_dscr_err_int_st : RO; bitpos: [19]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_dscr_err_int_st:1;
        /** slc1_rx_dscr_err_int_st : RO; bitpos: [20]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_dscr_err_int_st:1;
        /** slc1_tx_dscr_empty_int_st : RO; bitpos: [21]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_dscr_empty_int_st:1;
        /** slc1_host_rd_ack_int_st : RO; bitpos: [22]; default: 0;
         *  reserved
         */
        uint32_t slc1_host_rd_ack_int_st:1;
        /** slc1_wr_retry_done_int_st : RO; bitpos: [23]; default: 0;
         *  reserved
         */
        uint32_t slc1_wr_retry_done_int_st:1;
        /** slc1_tx_err_eof_int_st : RO; bitpos: [24]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_err_eof_int_st:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} sdio_slc1int_st_reg_t;

/** Type of slc1int_ena register
 *  reserved
 */
typedef union {
    struct {
        /** slc_frhost_bit8_int_ena : R/W; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit8_int_ena:1;
        /** slc_frhost_bit9_int_ena : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit9_int_ena:1;
        /** slc_frhost_bit10_int_ena : R/W; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit10_int_ena:1;
        /** slc_frhost_bit11_int_ena : R/W; bitpos: [3]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit11_int_ena:1;
        /** slc_frhost_bit12_int_ena : R/W; bitpos: [4]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit12_int_ena:1;
        /** slc_frhost_bit13_int_ena : R/W; bitpos: [5]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit13_int_ena:1;
        /** slc_frhost_bit14_int_ena : R/W; bitpos: [6]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit14_int_ena:1;
        /** slc_frhost_bit15_int_ena : R/W; bitpos: [7]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit15_int_ena:1;
        /** slc1_rx_start_int_ena : R/W; bitpos: [8]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_start_int_ena:1;
        /** slc1_tx_start_int_ena : R/W; bitpos: [9]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_start_int_ena:1;
        /** slc1_rx_udf_int_ena : R/W; bitpos: [10]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_udf_int_ena:1;
        /** slc1_tx_ovf_int_ena : R/W; bitpos: [11]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_ovf_int_ena:1;
        /** slc1_token0_1to0_int_ena : R/W; bitpos: [12]; default: 0;
         *  reserved
         */
        uint32_t slc1_token0_1to0_int_ena:1;
        /** slc1_token1_1to0_int_ena : R/W; bitpos: [13]; default: 0;
         *  reserved
         */
        uint32_t slc1_token1_1to0_int_ena:1;
        /** slc1_tx_done_int_ena : R/W; bitpos: [14]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_done_int_ena:1;
        /** slc1_tx_suc_eof_int_ena : R/W; bitpos: [15]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_suc_eof_int_ena:1;
        /** slc1_rx_done_int_ena : R/W; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_done_int_ena:1;
        /** slc1_rx_eof_int_ena : R/W; bitpos: [17]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_eof_int_ena:1;
        /** slc1_tohost_int_ena : R/W; bitpos: [18]; default: 0;
         *  reserved
         */
        uint32_t slc1_tohost_int_ena:1;
        /** slc1_tx_dscr_err_int_ena : R/W; bitpos: [19]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_dscr_err_int_ena:1;
        /** slc1_rx_dscr_err_int_ena : R/W; bitpos: [20]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_dscr_err_int_ena:1;
        /** slc1_tx_dscr_empty_int_ena : R/W; bitpos: [21]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_dscr_empty_int_ena:1;
        /** slc1_host_rd_ack_int_ena : R/W; bitpos: [22]; default: 0;
         *  reserved
         */
        uint32_t slc1_host_rd_ack_int_ena:1;
        /** slc1_wr_retry_done_int_ena : R/W; bitpos: [23]; default: 0;
         *  reserved
         */
        uint32_t slc1_wr_retry_done_int_ena:1;
        /** slc1_tx_err_eof_int_ena : R/W; bitpos: [24]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_err_eof_int_ena:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} sdio_slc1int_ena_reg_t;

/** Type of slc1int_clr register
 *  reserved
 */
typedef union {
    struct {
        /** slc_frhost_bit8_int_clr : WT; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit8_int_clr:1;
        /** slc_frhost_bit9_int_clr : WT; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit9_int_clr:1;
        /** slc_frhost_bit10_int_clr : WT; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit10_int_clr:1;
        /** slc_frhost_bit11_int_clr : WT; bitpos: [3]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit11_int_clr:1;
        /** slc_frhost_bit12_int_clr : WT; bitpos: [4]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit12_int_clr:1;
        /** slc_frhost_bit13_int_clr : WT; bitpos: [5]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit13_int_clr:1;
        /** slc_frhost_bit14_int_clr : WT; bitpos: [6]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit14_int_clr:1;
        /** slc_frhost_bit15_int_clr : WT; bitpos: [7]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit15_int_clr:1;
        /** slc1_rx_start_int_clr : WT; bitpos: [8]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_start_int_clr:1;
        /** slc1_tx_start_int_clr : WT; bitpos: [9]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_start_int_clr:1;
        /** slc1_rx_udf_int_clr : WT; bitpos: [10]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_udf_int_clr:1;
        /** slc1_tx_ovf_int_clr : WT; bitpos: [11]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_ovf_int_clr:1;
        /** slc1_token0_1to0_int_clr : WT; bitpos: [12]; default: 0;
         *  reserved
         */
        uint32_t slc1_token0_1to0_int_clr:1;
        /** slc1_token1_1to0_int_clr : WT; bitpos: [13]; default: 0;
         *  reserved
         */
        uint32_t slc1_token1_1to0_int_clr:1;
        /** slc1_tx_done_int_clr : WT; bitpos: [14]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_done_int_clr:1;
        /** slc1_tx_suc_eof_int_clr : WT; bitpos: [15]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_suc_eof_int_clr:1;
        /** slc1_rx_done_int_clr : WT; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_done_int_clr:1;
        /** slc1_rx_eof_int_clr : WT; bitpos: [17]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_eof_int_clr:1;
        /** slc1_tohost_int_clr : WT; bitpos: [18]; default: 0;
         *  reserved
         */
        uint32_t slc1_tohost_int_clr:1;
        /** slc1_tx_dscr_err_int_clr : WT; bitpos: [19]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_dscr_err_int_clr:1;
        /** slc1_rx_dscr_err_int_clr : WT; bitpos: [20]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_dscr_err_int_clr:1;
        /** slc1_tx_dscr_empty_int_clr : WT; bitpos: [21]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_dscr_empty_int_clr:1;
        /** slc1_host_rd_ack_int_clr : WT; bitpos: [22]; default: 0;
         *  reserved
         */
        uint32_t slc1_host_rd_ack_int_clr:1;
        /** slc1_wr_retry_done_int_clr : WT; bitpos: [23]; default: 0;
         *  reserved
         */
        uint32_t slc1_wr_retry_done_int_clr:1;
        /** slc1_tx_err_eof_int_clr : WT; bitpos: [24]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_err_eof_int_clr:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} sdio_slc1int_clr_reg_t;

/** Type of slc0int_st1 register
 *  reserved
 */
typedef union {
    struct {
        /** slc_frhost_bit0_int_st1 : RO; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit0_int_st1:1;
        /** slc_frhost_bit1_int_st1 : RO; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit1_int_st1:1;
        /** slc_frhost_bit2_int_st1 : RO; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit2_int_st1:1;
        /** slc_frhost_bit3_int_st1 : RO; bitpos: [3]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit3_int_st1:1;
        /** slc_frhost_bit4_int_st1 : RO; bitpos: [4]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit4_int_st1:1;
        /** slc_frhost_bit5_int_st1 : RO; bitpos: [5]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit5_int_st1:1;
        /** slc_frhost_bit6_int_st1 : RO; bitpos: [6]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit6_int_st1:1;
        /** slc_frhost_bit7_int_st1 : RO; bitpos: [7]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit7_int_st1:1;
        /** slc0_rx_start_int_st1 : RO; bitpos: [8]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_start_int_st1:1;
        /** slc0_tx_start_int_st1 : RO; bitpos: [9]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_start_int_st1:1;
        /** slc0_rx_udf_int_st1 : RO; bitpos: [10]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_udf_int_st1:1;
        /** slc0_tx_ovf_int_st1 : RO; bitpos: [11]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_ovf_int_st1:1;
        /** slc0_token0_1to0_int_st1 : RO; bitpos: [12]; default: 0;
         *  reserved
         */
        uint32_t slc0_token0_1to0_int_st1:1;
        /** slc0_token1_1to0_int_st1 : RO; bitpos: [13]; default: 0;
         *  reserved
         */
        uint32_t slc0_token1_1to0_int_st1:1;
        /** slc0_tx_done_int_st1 : RO; bitpos: [14]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_done_int_st1:1;
        /** slc0_tx_suc_eof_int_st1 : RO; bitpos: [15]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_suc_eof_int_st1:1;
        /** slc0_rx_done_int_st1 : RO; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_done_int_st1:1;
        /** slc0_rx_eof_int_st1 : RO; bitpos: [17]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_eof_int_st1:1;
        /** slc0_tohost_int_st1 : RO; bitpos: [18]; default: 0;
         *  reserved
         */
        uint32_t slc0_tohost_int_st1:1;
        /** slc0_tx_dscr_err_int_st1 : RO; bitpos: [19]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_dscr_err_int_st1:1;
        /** slc0_rx_dscr_err_int_st1 : RO; bitpos: [20]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_dscr_err_int_st1:1;
        /** slc0_tx_dscr_empty_int_st1 : RO; bitpos: [21]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_dscr_empty_int_st1:1;
        /** slc0_host_rd_ack_int_st1 : RO; bitpos: [22]; default: 0;
         *  reserved
         */
        uint32_t slc0_host_rd_ack_int_st1:1;
        /** slc0_wr_retry_done_int_st1 : RO; bitpos: [23]; default: 0;
         *  reserved
         */
        uint32_t slc0_wr_retry_done_int_st1:1;
        /** slc0_tx_err_eof_int_st1 : RO; bitpos: [24]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_err_eof_int_st1:1;
        /** cmd_dtc_int_st1 : RO; bitpos: [25]; default: 0;
         *  reserved
         */
        uint32_t cmd_dtc_int_st1:1;
        /** slc0_rx_quick_eof_int_st1 : RO; bitpos: [26]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_quick_eof_int_st1:1;
        /** slc0_host_pop_eof_err_int_st1 : RO; bitpos: [27]; default: 0;
         *  reserved
         */
        uint32_t slc0_host_pop_eof_err_int_st1:1;
        /** hda_recv_done_int_st1 : RO; bitpos: [28]; default: 0;
         *  reserved
         */
        uint32_t hda_recv_done_int_st1:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} sdio_slc0int_st1_reg_t;

/** Type of slc0int_ena1 register
 *  reserved
 */
typedef union {
    struct {
        /** slc_frhost_bit0_int_ena1 : R/W; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit0_int_ena1:1;
        /** slc_frhost_bit1_int_ena1 : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit1_int_ena1:1;
        /** slc_frhost_bit2_int_ena1 : R/W; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit2_int_ena1:1;
        /** slc_frhost_bit3_int_ena1 : R/W; bitpos: [3]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit3_int_ena1:1;
        /** slc_frhost_bit4_int_ena1 : R/W; bitpos: [4]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit4_int_ena1:1;
        /** slc_frhost_bit5_int_ena1 : R/W; bitpos: [5]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit5_int_ena1:1;
        /** slc_frhost_bit6_int_ena1 : R/W; bitpos: [6]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit6_int_ena1:1;
        /** slc_frhost_bit7_int_ena1 : R/W; bitpos: [7]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit7_int_ena1:1;
        /** slc0_rx_start_int_ena1 : R/W; bitpos: [8]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_start_int_ena1:1;
        /** slc0_tx_start_int_ena1 : R/W; bitpos: [9]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_start_int_ena1:1;
        /** slc0_rx_udf_int_ena1 : R/W; bitpos: [10]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_udf_int_ena1:1;
        /** slc0_tx_ovf_int_ena1 : R/W; bitpos: [11]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_ovf_int_ena1:1;
        /** slc0_token0_1to0_int_ena1 : R/W; bitpos: [12]; default: 0;
         *  reserved
         */
        uint32_t slc0_token0_1to0_int_ena1:1;
        /** slc0_token1_1to0_int_ena1 : R/W; bitpos: [13]; default: 0;
         *  reserved
         */
        uint32_t slc0_token1_1to0_int_ena1:1;
        /** slc0_tx_done_int_ena1 : R/W; bitpos: [14]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_done_int_ena1:1;
        /** slc0_tx_suc_eof_int_ena1 : R/W; bitpos: [15]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_suc_eof_int_ena1:1;
        /** slc0_rx_done_int_ena1 : R/W; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_done_int_ena1:1;
        /** slc0_rx_eof_int_ena1 : R/W; bitpos: [17]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_eof_int_ena1:1;
        /** slc0_tohost_int_ena1 : R/W; bitpos: [18]; default: 0;
         *  reserved
         */
        uint32_t slc0_tohost_int_ena1:1;
        /** slc0_tx_dscr_err_int_ena1 : R/W; bitpos: [19]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_dscr_err_int_ena1:1;
        /** slc0_rx_dscr_err_int_ena1 : R/W; bitpos: [20]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_dscr_err_int_ena1:1;
        /** slc0_tx_dscr_empty_int_ena1 : R/W; bitpos: [21]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_dscr_empty_int_ena1:1;
        /** slc0_host_rd_ack_int_ena1 : R/W; bitpos: [22]; default: 0;
         *  reserved
         */
        uint32_t slc0_host_rd_ack_int_ena1:1;
        /** slc0_wr_retry_done_int_ena1 : R/W; bitpos: [23]; default: 0;
         *  reserved
         */
        uint32_t slc0_wr_retry_done_int_ena1:1;
        /** slc0_tx_err_eof_int_ena1 : R/W; bitpos: [24]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_err_eof_int_ena1:1;
        /** cmd_dtc_int_ena1 : R/W; bitpos: [25]; default: 0;
         *  reserved
         */
        uint32_t cmd_dtc_int_ena1:1;
        /** slc0_rx_quick_eof_int_ena1 : R/W; bitpos: [26]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_quick_eof_int_ena1:1;
        /** slc0_host_pop_eof_err_int_ena1 : R/W; bitpos: [27]; default: 0;
         *  reserved
         */
        uint32_t slc0_host_pop_eof_err_int_ena1:1;
        /** hda_recv_done_int_ena1 : R/W; bitpos: [28]; default: 0;
         *  reserved
         */
        uint32_t hda_recv_done_int_ena1:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} sdio_slc0int_ena1_reg_t;

/** Type of slc1int_st1 register
 *  reserved
 */
typedef union {
    struct {
        /** slc_frhost_bit8_int_st1 : RO; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit8_int_st1:1;
        /** slc_frhost_bit9_int_st1 : RO; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit9_int_st1:1;
        /** slc_frhost_bit10_int_st1 : RO; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit10_int_st1:1;
        /** slc_frhost_bit11_int_st1 : RO; bitpos: [3]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit11_int_st1:1;
        /** slc_frhost_bit12_int_st1 : RO; bitpos: [4]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit12_int_st1:1;
        /** slc_frhost_bit13_int_st1 : RO; bitpos: [5]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit13_int_st1:1;
        /** slc_frhost_bit14_int_st1 : RO; bitpos: [6]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit14_int_st1:1;
        /** slc_frhost_bit15_int_st1 : RO; bitpos: [7]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit15_int_st1:1;
        /** slc1_rx_start_int_st1 : RO; bitpos: [8]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_start_int_st1:1;
        /** slc1_tx_start_int_st1 : RO; bitpos: [9]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_start_int_st1:1;
        /** slc1_rx_udf_int_st1 : RO; bitpos: [10]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_udf_int_st1:1;
        /** slc1_tx_ovf_int_st1 : RO; bitpos: [11]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_ovf_int_st1:1;
        /** slc1_token0_1to0_int_st1 : RO; bitpos: [12]; default: 0;
         *  reserved
         */
        uint32_t slc1_token0_1to0_int_st1:1;
        /** slc1_token1_1to0_int_st1 : RO; bitpos: [13]; default: 0;
         *  reserved
         */
        uint32_t slc1_token1_1to0_int_st1:1;
        /** slc1_tx_done_int_st1 : RO; bitpos: [14]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_done_int_st1:1;
        /** slc1_tx_suc_eof_int_st1 : RO; bitpos: [15]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_suc_eof_int_st1:1;
        /** slc1_rx_done_int_st1 : RO; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_done_int_st1:1;
        /** slc1_rx_eof_int_st1 : RO; bitpos: [17]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_eof_int_st1:1;
        /** slc1_tohost_int_st1 : RO; bitpos: [18]; default: 0;
         *  reserved
         */
        uint32_t slc1_tohost_int_st1:1;
        /** slc1_tx_dscr_err_int_st1 : RO; bitpos: [19]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_dscr_err_int_st1:1;
        /** slc1_rx_dscr_err_int_st1 : RO; bitpos: [20]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_dscr_err_int_st1:1;
        /** slc1_tx_dscr_empty_int_st1 : RO; bitpos: [21]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_dscr_empty_int_st1:1;
        /** slc1_host_rd_ack_int_st1 : RO; bitpos: [22]; default: 0;
         *  reserved
         */
        uint32_t slc1_host_rd_ack_int_st1:1;
        /** slc1_wr_retry_done_int_st1 : RO; bitpos: [23]; default: 0;
         *  reserved
         */
        uint32_t slc1_wr_retry_done_int_st1:1;
        /** slc1_tx_err_eof_int_st1 : RO; bitpos: [24]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_err_eof_int_st1:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} sdio_slc1int_st1_reg_t;

/** Type of slc1int_ena1 register
 *  reserved
 */
typedef union {
    struct {
        /** slc_frhost_bit8_int_ena1 : R/W; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit8_int_ena1:1;
        /** slc_frhost_bit9_int_ena1 : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit9_int_ena1:1;
        /** slc_frhost_bit10_int_ena1 : R/W; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit10_int_ena1:1;
        /** slc_frhost_bit11_int_ena1 : R/W; bitpos: [3]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit11_int_ena1:1;
        /** slc_frhost_bit12_int_ena1 : R/W; bitpos: [4]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit12_int_ena1:1;
        /** slc_frhost_bit13_int_ena1 : R/W; bitpos: [5]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit13_int_ena1:1;
        /** slc_frhost_bit14_int_ena1 : R/W; bitpos: [6]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit14_int_ena1:1;
        /** slc_frhost_bit15_int_ena1 : R/W; bitpos: [7]; default: 0;
         *  reserved
         */
        uint32_t slc_frhost_bit15_int_ena1:1;
        /** slc1_rx_start_int_ena1 : R/W; bitpos: [8]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_start_int_ena1:1;
        /** slc1_tx_start_int_ena1 : R/W; bitpos: [9]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_start_int_ena1:1;
        /** slc1_rx_udf_int_ena1 : R/W; bitpos: [10]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_udf_int_ena1:1;
        /** slc1_tx_ovf_int_ena1 : R/W; bitpos: [11]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_ovf_int_ena1:1;
        /** slc1_token0_1to0_int_ena1 : R/W; bitpos: [12]; default: 0;
         *  reserved
         */
        uint32_t slc1_token0_1to0_int_ena1:1;
        /** slc1_token1_1to0_int_ena1 : R/W; bitpos: [13]; default: 0;
         *  reserved
         */
        uint32_t slc1_token1_1to0_int_ena1:1;
        /** slc1_tx_done_int_ena1 : R/W; bitpos: [14]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_done_int_ena1:1;
        /** slc1_tx_suc_eof_int_ena1 : R/W; bitpos: [15]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_suc_eof_int_ena1:1;
        /** slc1_rx_done_int_ena1 : R/W; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_done_int_ena1:1;
        /** slc1_rx_eof_int_ena1 : R/W; bitpos: [17]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_eof_int_ena1:1;
        /** slc1_tohost_int_ena1 : R/W; bitpos: [18]; default: 0;
         *  reserved
         */
        uint32_t slc1_tohost_int_ena1:1;
        /** slc1_tx_dscr_err_int_ena1 : R/W; bitpos: [19]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_dscr_err_int_ena1:1;
        /** slc1_rx_dscr_err_int_ena1 : R/W; bitpos: [20]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_dscr_err_int_ena1:1;
        /** slc1_tx_dscr_empty_int_ena1 : R/W; bitpos: [21]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_dscr_empty_int_ena1:1;
        /** slc1_host_rd_ack_int_ena1 : R/W; bitpos: [22]; default: 0;
         *  reserved
         */
        uint32_t slc1_host_rd_ack_int_ena1:1;
        /** slc1_wr_retry_done_int_ena1 : R/W; bitpos: [23]; default: 0;
         *  reserved
         */
        uint32_t slc1_wr_retry_done_int_ena1:1;
        /** slc1_tx_err_eof_int_ena1 : R/W; bitpos: [24]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_err_eof_int_ena1:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} sdio_slc1int_ena1_reg_t;


/** Group: Status registers */
/** Type of slcrx_status register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc0_rx_full : RO; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_full:1;
        /** slc0_rx_empty : RO; bitpos: [1]; default: 1;
         *  reserved
         */
        uint32_t slc0_rx_empty:1;
        /** slc0_rx_buf_len : RO; bitpos: [15:2]; default: 0;
         *  the current buffer length when slc0 reads data from rx link
         */
        uint32_t slc0_rx_buf_len:14;
        /** slc1_rx_full : RO; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t slc1_rx_full:1;
        /** slc1_rx_empty : RO; bitpos: [17]; default: 1;
         *  reserved
         */
        uint32_t slc1_rx_empty:1;
        /** slc1_rx_buf_len : RO; bitpos: [31:18]; default: 0;
         *  the current buffer length when slc1 reads data from rx link
         */
        uint32_t slc1_rx_buf_len:14;
    };
    uint32_t val;
} sdio_slcrx_status_reg_t;

/** Type of slctx_status register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc0_tx_full : RO; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_full:1;
        /** slc0_tx_empty : RO; bitpos: [1]; default: 1;
         *  reserved
         */
        uint32_t slc0_tx_empty:1;
        uint32_t reserved_2:14;
        /** slc1_tx_full : RO; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_full:1;
        /** slc1_tx_empty : RO; bitpos: [17]; default: 1;
         *  reserved
         */
        uint32_t slc1_tx_empty:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} sdio_slctx_status_reg_t;

/** Type of slc0_state0 register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_state0 : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_state0:32;
    };
    uint32_t val;
} sdio_slc0_state0_reg_t;

/** Type of slc0_state1 register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc0_state1 : RO; bitpos: [31:0]; default: 0;
         *  [18:0] the current rx descriptor address, [20:19] rx_dscr fsm state, [23:21]
         *  rx_link fsm state, [30:24] rx_fifo_cnt
         */
        uint32_t slc0_state1:32;
    };
    uint32_t val;
} sdio_slc0_state1_reg_t;

/** Type of slc1_state0 register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc1_state0 : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc1_state0:32;
    };
    uint32_t val;
} sdio_slc1_state0_reg_t;

/** Type of slc1_state1 register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc1_state1 : RO; bitpos: [31:0]; default: 0;
         *  [18:0] the current rx descriptor address, [20:19] rx_dscr fsm state, [23:21]
         *  rx_link fsm state, [30:24] rx_fifo_cnt
         */
        uint32_t slc1_state1:32;
    };
    uint32_t val;
} sdio_slc1_state1_reg_t;

/** Type of slc_sdio_st register
 *  reserved
 */
typedef union {
    struct {
        /** cmd_st : RO; bitpos: [2:0]; default: 0;
         *  reserved
         */
        uint32_t cmd_st:3;
        uint32_t reserved_3:1;
        /** func_st : RO; bitpos: [7:4]; default: 0;
         *  reserved
         */
        uint32_t func_st:4;
        /** sdio_wakeup : RO; bitpos: [8]; default: 0;
         *  reserved
         */
        uint32_t sdio_wakeup:1;
        uint32_t reserved_9:3;
        /** bus_st : RO; bitpos: [14:12]; default: 0;
         *  reserved
         */
        uint32_t bus_st:3;
        uint32_t reserved_15:1;
        /** func1_acc_state : RO; bitpos: [20:16]; default: 0;
         *  reserved
         */
        uint32_t func1_acc_state:5;
        uint32_t reserved_21:3;
        /** func2_acc_state : RO; bitpos: [28:24]; default: 0;
         *  reserved
         */
        uint32_t func2_acc_state:5;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} sdio_slc_sdio_st_reg_t;

/** Type of slc0_txlink_dscr register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc0_txlink_dscr : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_txlink_dscr:32;
    };
    uint32_t val;
} sdio_slc0_txlink_dscr_reg_t;

/** Type of slc0_txlink_dscr_bf0 register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc0_txlink_dscr_bf0 : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_txlink_dscr_bf0:32;
    };
    uint32_t val;
} sdio_slc0_txlink_dscr_bf0_reg_t;

/** Type of slc0_txlink_dscr_bf1 register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_txlink_dscr_bf1 : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_txlink_dscr_bf1:32;
    };
    uint32_t val;
} sdio_slc0_txlink_dscr_bf1_reg_t;

/** Type of slc0_rxlink_dscr register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc0_rxlink_dscr : RO; bitpos: [31:0]; default: 0;
         *  the third word of slc0 link descriptor, or known as the next descriptor address
         */
        uint32_t slc0_rxlink_dscr:32;
    };
    uint32_t val;
} sdio_slc0_rxlink_dscr_reg_t;

/** Type of slc0_rxlink_dscr_bf0 register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc0_rxlink_dscr_bf0 : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_rxlink_dscr_bf0:32;
    };
    uint32_t val;
} sdio_slc0_rxlink_dscr_bf0_reg_t;

/** Type of slc0_rxlink_dscr_bf1 register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_rxlink_dscr_bf1 : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_rxlink_dscr_bf1:32;
    };
    uint32_t val;
} sdio_slc0_rxlink_dscr_bf1_reg_t;

/** Type of slc1_txlink_dscr register
 *  reserved
 */
typedef union {
    struct {
        /** slc1_txlink_dscr : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc1_txlink_dscr:32;
    };
    uint32_t val;
} sdio_slc1_txlink_dscr_reg_t;

/** Type of slc1_txlink_dscr_bf0 register
 *  reserved
 */
typedef union {
    struct {
        /** slc1_txlink_dscr_bf0 : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc1_txlink_dscr_bf0:32;
    };
    uint32_t val;
} sdio_slc1_txlink_dscr_bf0_reg_t;

/** Type of slc1_txlink_dscr_bf1 register
 *  reserved
 */
typedef union {
    struct {
        /** slc1_txlink_dscr_bf1 : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc1_txlink_dscr_bf1:32;
    };
    uint32_t val;
} sdio_slc1_txlink_dscr_bf1_reg_t;

/** Type of slc1_rxlink_dscr register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc1_rxlink_dscr : RO; bitpos: [31:0]; default: 0;
         *  the third word of slc1 link descriptor, or known as the next descriptor address
         */
        uint32_t slc1_rxlink_dscr:32;
    };
    uint32_t val;
} sdio_slc1_rxlink_dscr_reg_t;

/** Type of slc1_rxlink_dscr_bf0 register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc1_rxlink_dscr_bf0 : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc1_rxlink_dscr_bf0:32;
    };
    uint32_t val;
} sdio_slc1_rxlink_dscr_bf0_reg_t;

/** Type of slc1_rxlink_dscr_bf1 register
 *  reserved
 */
typedef union {
    struct {
        /** slc1_rxlink_dscr_bf1 : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc1_rxlink_dscr_bf1:32;
    };
    uint32_t val;
} sdio_slc1_rxlink_dscr_bf1_reg_t;

/** Type of slc0_tx_erreof_des_addr register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_tx_err_eof_des_addr : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_tx_err_eof_des_addr:32;
    };
    uint32_t val;
} sdio_slc0_tx_erreof_des_addr_reg_t;

/** Type of slc1_tx_erreof_des_addr register
 *  reserved
 */
typedef union {
    struct {
        /** slc1_tx_err_eof_des_addr : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc1_tx_err_eof_des_addr:32;
    };
    uint32_t val;
} sdio_slc1_tx_erreof_des_addr_reg_t;

/** Type of slc_token_lat register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_token : RO; bitpos: [11:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_token:12;
        uint32_t reserved_12:4;
        /** slc1_token : RO; bitpos: [27:16]; default: 0;
         *  reserved
         */
        uint32_t slc1_token:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} sdio_slc_token_lat_reg_t;

/** Type of slc_cmd_infor0 register
 *  reserved
 */
typedef union {
    struct {
        /** cmd_content0 : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t cmd_content0:32;
    };
    uint32_t val;
} sdio_slc_cmd_infor0_reg_t;

/** Type of slc_cmd_infor1 register
 *  reserved
 */
typedef union {
    struct {
        /** cmd_content1 : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t cmd_content1:32;
    };
    uint32_t val;
} sdio_slc_cmd_infor1_reg_t;

/** Type of slc0_length register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_len : RO; bitpos: [19:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_len:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} sdio_slc0_length_reg_t;

/** Type of slc_sdio_crc_st0 register
 *  reserved
 */
typedef union {
    struct {
        /** dat0_crc_err_cnt : RO; bitpos: [7:0]; default: 0;
         *  reserved
         */
        uint32_t dat0_crc_err_cnt:8;
        /** dat1_crc_err_cnt : RO; bitpos: [15:8]; default: 0;
         *  reserved
         */
        uint32_t dat1_crc_err_cnt:8;
        /** dat2_crc_err_cnt : RO; bitpos: [23:16]; default: 0;
         *  reserved
         */
        uint32_t dat2_crc_err_cnt:8;
        /** dat3_crc_err_cnt : RO; bitpos: [31:24]; default: 0;
         *  reserved
         */
        uint32_t dat3_crc_err_cnt:8;
    };
    uint32_t val;
} sdio_slc_sdio_crc_st0_reg_t;

/** Type of slc0_eof_start_des register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_eof_start_des_addr : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t slc0_eof_start_des_addr:32;
    };
    uint32_t val;
} sdio_slc0_eof_start_des_reg_t;

/** Type of slc0_push_dscr_addr register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc0_rx_push_dscr_addr : RO; bitpos: [31:0]; default: 0;
         *  the current descriptor address when slc0 gets  a link descriptor, aligned with word
         */
        uint32_t slc0_rx_push_dscr_addr:32;
    };
    uint32_t val;
} sdio_slc0_push_dscr_addr_reg_t;

/** Type of slc0_done_dscr_addr register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc0_rx_done_dscr_addr : RO; bitpos: [31:0]; default: 0;
         *  the current descriptor address when slc0 finishes reading data from one buffer,
         *  aligned with word
         */
        uint32_t slc0_rx_done_dscr_addr:32;
    };
    uint32_t val;
} sdio_slc0_done_dscr_addr_reg_t;

/** Type of slc0_sub_start_des register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc0_sub_pac_start_dscr_addr : RO; bitpos: [31:0]; default: 0;
         *  the current descriptor address when slc0 gets  a link descriptor, aligned with word
         */
        uint32_t slc0_sub_pac_start_dscr_addr:32;
    };
    uint32_t val;
} sdio_slc0_sub_start_des_reg_t;

/** Type of slc0_dscr_cnt register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc0_rx_dscr_cnt_lat : RO; bitpos: [9:0]; default: 0;
         *  the number of descriptors got by slc0 when it tries to read data from memory
         */
        uint32_t slc0_rx_dscr_cnt_lat:10;
        uint32_t reserved_10:6;
        /** slc0_rx_get_eof_occ : RO; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t slc0_rx_get_eof_occ:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} sdio_slc0_dscr_cnt_reg_t;


/** Group: Debud registers */
/** Type of slc0txfifo_pop register
 *  reserved
 */
typedef union {
    struct {
        /** slc0_txfifo_rdata : RO; bitpos: [10:0]; default: 1024;
         *  reserved
         */
        uint32_t slc0_txfifo_rdata:11;
        uint32_t reserved_11:5;
        /** slc0_txfifo_pop : R/W/SC; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t slc0_txfifo_pop:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} sdio_slc0txfifo_pop_reg_t;

/** Type of slc1txfifo_pop register
 *  reserved
 */
typedef union {
    struct {
        /** slc1_txfifo_rdata : RO; bitpos: [10:0]; default: 1024;
         *  reserved
         */
        uint32_t slc1_txfifo_rdata:11;
        uint32_t reserved_11:5;
        /** slc1_txfifo_pop : R/W/SC; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t slc1_txfifo_pop:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} sdio_slc1txfifo_pop_reg_t;

/** Type of slc_ahb_test register
 *  reserved
 */
typedef union {
    struct {
        /** slc_ahb_testmode : R/W; bitpos: [2:0]; default: 0;
         *  reserved
         */
        uint32_t slc_ahb_testmode:3;
        uint32_t reserved_3:1;
        /** slc_ahb_testaddr : R/W; bitpos: [5:4]; default: 0;
         *  reserved
         */
        uint32_t slc_ahb_testaddr:2;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} sdio_slc_ahb_test_reg_t;


/** Group: Version registers */
/** Type of slcdate register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** slc_date : R/W; bitpos: [31:0]; default: 554182400;
         *  reserved
         */
        uint32_t slc_date:32;
    };
    uint32_t val;
} sdio_slcdate_reg_t;


typedef struct slc_dev_t {
    volatile sdio_slcconf0_reg_t slcconf0;
    volatile sdio_slc0int_raw_reg_t slc0int_raw;
    volatile sdio_slc0int_st_reg_t slc0int_st;
    volatile sdio_slc0int_ena_reg_t slc0int_ena;
    volatile sdio_slc0int_clr_reg_t slc0int_clr;
    volatile sdio_slc1int_raw_reg_t slc1int_raw;
    volatile sdio_slc1int_st_reg_t slc1int_st;
    volatile sdio_slc1int_ena_reg_t slc1int_ena;
    volatile sdio_slc1int_clr_reg_t slc1int_clr;
    volatile sdio_slcrx_status_reg_t slcrx_status;
    volatile sdio_slc0rxfifo_push_reg_t slc0rxfifo_push;
    volatile sdio_slc1rxfifo_push_reg_t slc1rxfifo_push;
    volatile sdio_slctx_status_reg_t slctx_status;
    volatile sdio_slc0txfifo_pop_reg_t slc0txfifo_pop;
    volatile sdio_slc1txfifo_pop_reg_t slc1txfifo_pop;
    volatile sdio_slc0rx_link_reg_t slc0rx_link;
    volatile sdio_slc0rx_link_addr_reg_t slc0rx_link_addr;
    volatile sdio_slc0tx_link_reg_t slc0tx_link;
    volatile sdio_slc0tx_link_addr_reg_t slc0tx_link_addr;
    volatile sdio_slc1rx_link_reg_t slc1rx_link;
    volatile sdio_slc1rx_link_addr_reg_t slc1rx_link_addr;
    volatile sdio_slc1tx_link_reg_t slc1tx_link;
    volatile sdio_slc1tx_link_addr_reg_t slc1tx_link_addr;
    volatile sdio_slcintvec_tohost_reg_t slcintvec_tohost;
    volatile sdio_slc0token0_reg_t slc0token0;
    volatile sdio_slc0token1_reg_t slc0token1;
    volatile sdio_slc1token0_reg_t slc1token0;
    volatile sdio_slc1token1_reg_t slc1token1;
    volatile sdio_slcconf1_reg_t slcconf1;
    volatile sdio_slc0_state0_reg_t slc0_state0;
    volatile sdio_slc0_state1_reg_t slc0_state1;
    volatile sdio_slc1_state0_reg_t slc1_state0;
    volatile sdio_slc1_state1_reg_t slc1_state1;
    volatile sdio_slcbridge_conf_reg_t slcbridge_conf;
    volatile sdio_slc0_to_eof_des_addr_reg_t slc0_to_eof_des_addr;
    volatile sdio_slc0_tx_eof_des_addr_reg_t slc0_tx_eof_des_addr;
    volatile sdio_slc0_to_eof_bfr_des_addr_reg_t slc0_to_eof_bfr_des_addr;
    volatile sdio_slc1_to_eof_des_addr_reg_t slc1_to_eof_des_addr;
    volatile sdio_slc1_tx_eof_des_addr_reg_t slc1_tx_eof_des_addr;
    volatile sdio_slc1_to_eof_bfr_des_addr_reg_t slc1_to_eof_bfr_des_addr;
    volatile sdio_slc_ahb_test_reg_t slc_ahb_test;
    volatile sdio_slc_sdio_st_reg_t slc_sdio_st;
    volatile sdio_slc_rx_dscr_conf_reg_t slc_rx_dscr_conf;
    volatile sdio_slc0_txlink_dscr_reg_t slc0_txlink_dscr;
    volatile sdio_slc0_txlink_dscr_bf0_reg_t slc0_txlink_dscr_bf0;
    volatile sdio_slc0_txlink_dscr_bf1_reg_t slc0_txlink_dscr_bf1;
    volatile sdio_slc0_rxlink_dscr_reg_t slc0_rxlink_dscr;
    volatile sdio_slc0_rxlink_dscr_bf0_reg_t slc0_rxlink_dscr_bf0;
    volatile sdio_slc0_rxlink_dscr_bf1_reg_t slc0_rxlink_dscr_bf1;
    volatile sdio_slc1_txlink_dscr_reg_t slc1_txlink_dscr;
    volatile sdio_slc1_txlink_dscr_bf0_reg_t slc1_txlink_dscr_bf0;
    volatile sdio_slc1_txlink_dscr_bf1_reg_t slc1_txlink_dscr_bf1;
    volatile sdio_slc1_rxlink_dscr_reg_t slc1_rxlink_dscr;
    volatile sdio_slc1_rxlink_dscr_bf0_reg_t slc1_rxlink_dscr_bf0;
    volatile sdio_slc1_rxlink_dscr_bf1_reg_t slc1_rxlink_dscr_bf1;
    volatile sdio_slc0_tx_erreof_des_addr_reg_t slc0_tx_erreof_des_addr;
    volatile sdio_slc1_tx_erreof_des_addr_reg_t slc1_tx_erreof_des_addr;
    volatile sdio_slc_token_lat_reg_t slc_token_lat;
    volatile sdio_slc_tx_dscr_conf_reg_t slc_tx_dscr_conf;
    volatile sdio_slc_cmd_infor0_reg_t slc_cmd_infor0;
    volatile sdio_slc_cmd_infor1_reg_t slc_cmd_infor1;
    volatile sdio_slc0_len_conf_reg_t slc0_len_conf;
    volatile sdio_slc0_length_reg_t slc0_length;
    volatile sdio_slc0_txpkt_h_dscr_reg_t slc0_txpkt_h_dscr;
    volatile sdio_slc0_txpkt_e_dscr_reg_t slc0_txpkt_e_dscr;
    volatile sdio_slc0_rxpkt_h_dscr_reg_t slc0_rxpkt_h_dscr;
    volatile sdio_slc0_rxpkt_e_dscr_reg_t slc0_rxpkt_e_dscr;
    volatile sdio_slc0_txpktu_h_dscr_reg_t slc0_txpktu_h_dscr;
    volatile sdio_slc0_txpktu_e_dscr_reg_t slc0_txpktu_e_dscr;
    volatile sdio_slc0_rxpktu_h_dscr_reg_t slc0_rxpktu_h_dscr;
    volatile sdio_slc0_rxpktu_e_dscr_reg_t slc0_rxpktu_e_dscr;
    volatile sdio_slc_seq_position_reg_t slc_seq_position;
    volatile sdio_slc0_dscr_rec_conf_reg_t slc0_dscr_rec_conf;
    volatile sdio_slc_sdio_crc_st0_reg_t slc_sdio_crc_st0;
    volatile sdio_slc_sdio_crc_st1_reg_t slc_sdio_crc_st1;
    volatile sdio_slc0_eof_start_des_reg_t slc0_eof_start_des;
    volatile sdio_slc0_push_dscr_addr_reg_t slc0_push_dscr_addr;
    volatile sdio_slc0_done_dscr_addr_reg_t slc0_done_dscr_addr;
    volatile sdio_slc0_sub_start_des_reg_t slc0_sub_start_des;
    volatile sdio_slc0_dscr_cnt_reg_t slc0_dscr_cnt;
    volatile sdio_slc0_len_lim_conf_reg_t slc0_len_lim_conf;
    volatile sdio_slc0int_st1_reg_t slc0int_st1;
    volatile sdio_slc0int_ena1_reg_t slc0int_ena1;
    volatile sdio_slc1int_st1_reg_t slc1int_st1;
    volatile sdio_slc1int_ena1_reg_t slc1int_ena1;
    volatile sdio_slc0_tx_sharemem_start_reg_t slc0_tx_sharemem_start;
    volatile sdio_slc0_tx_sharemem_end_reg_t slc0_tx_sharemem_end;
    volatile sdio_slc0_rx_sharemem_start_reg_t slc0_rx_sharemem_start;
    volatile sdio_slc0_rx_sharemem_end_reg_t slc0_rx_sharemem_end;
    volatile sdio_slc1_tx_sharemem_start_reg_t slc1_tx_sharemem_start;
    volatile sdio_slc1_tx_sharemem_end_reg_t slc1_tx_sharemem_end;
    volatile sdio_slc1_rx_sharemem_start_reg_t slc1_rx_sharemem_start;
    volatile sdio_slc1_rx_sharemem_end_reg_t slc1_rx_sharemem_end;
    volatile sdio_hda_tx_sharemem_start_reg_t hda_tx_sharemem_start;
    volatile sdio_hda_rx_sharemem_start_reg_t hda_rx_sharemem_start;
    volatile sdio_slc_burst_len_reg_t slc_burst_len;
    uint32_t reserved_180[30];
    volatile sdio_slcdate_reg_t slcdate;
    volatile sdio_slcid_reg_t slcid;
} slc_dev_t;

extern slc_dev_t SLC;

#ifndef __cplusplus
_Static_assert(sizeof(slc_dev_t) == 0x200, "Invalid size of slc_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
