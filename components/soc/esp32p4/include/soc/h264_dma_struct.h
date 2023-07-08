/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Registers */
/** Type of out_conf0_ch0 register
 *  TX CH0 config0 register
 */
typedef union {
    struct {
        /** out_auto_wrback_ch0 : R/W; bitpos: [0]; default: 0;
         *  Set this bit to enable automatic outlink-writeback when all the data pointed by
         *  outlink descriptor has been received.
         */
        uint32_t out_auto_wrback_ch0:1;
        /** out_eof_mode_ch0 : R/W; bitpos: [1]; default: 1;
         *  EOF flag generation mode when receiving data. 1: EOF flag for Tx channel 0 is
         *  generated when data need to read has been popped from FIFO in DMA
         */
        uint32_t out_eof_mode_ch0:1;
        /** outdscr_burst_en_ch0 : R/W; bitpos: [2]; default: 0;
         *  Set this bit to 1 to enable INCR burst transfer for Tx channel 0 reading link
         *  descriptor when accessing internal SRAM.
         */
        uint32_t outdscr_burst_en_ch0:1;
        /** out_ecc_aes_en_ch0 : R/W; bitpos: [3]; default: 0;
         *  When access address space is ecc/aes area, this bit should be set to 1. In this
         *  case, the start address of square should be 16-bit aligned. The width of square
         *  multiply byte number of one pixel should be 16-bit aligned.
         */
        uint32_t out_ecc_aes_en_ch0:1;
        /** out_check_owner_ch0 : R/W; bitpos: [4]; default: 0;
         *  Set this bit to enable checking the owner attribute of the link descriptor.
         */
        uint32_t out_check_owner_ch0:1;
        uint32_t reserved_5:1;
        /** out_mem_burst_length_ch0 : R/W; bitpos: [8:6]; default: 0;
         *  Block size of Tx channel 0. 0: single      1: 16 bytes      2: 32 bytes    3: 64
         *  bytes    4: 128 bytes
         */
        uint32_t out_mem_burst_length_ch0:3;
        uint32_t reserved_9:3;
        /** out_page_bound_en_ch0 : R/W; bitpos: [12]; default: 0;
         *  Set this bit to 1 to make sure AXI read data don't cross the address boundary which
         *  define by mem_burst_length
         */
        uint32_t out_page_bound_en_ch0:1;
        uint32_t reserved_13:3;
        /** out_reorder_en_ch0 : R/W; bitpos: [16]; default: 0;
         *  Enable TX channel 0 macro block reorder when set to 1, only channel0 have this
         *  selection
         */
        uint32_t out_reorder_en_ch0:1;
        uint32_t reserved_17:7;
        /** out_rst_ch0 : R/W; bitpos: [24]; default: 0;
         *  Write 1 then write 0 to this bit to reset TX channel
         */
        uint32_t out_rst_ch0:1;
        /** out_cmd_disable_ch0 : R/W; bitpos: [25]; default: 0;
         *  Write 1 before reset and write 0 after reset
         */
        uint32_t out_cmd_disable_ch0:1;
        /** out_arb_weight_opt_dis_ch0 : R/W; bitpos: [26]; default: 0;
         *  Set this bit to 1 to disable arbiter optimum weight function.
         */
        uint32_t out_arb_weight_opt_dis_ch0:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} h264_dma_out_conf0_ch0_reg_t;

/** Type of out_push_ch0 register
 *  TX CH0 outfifo push register
 */
typedef union {
    struct {
        /** outfifo_wdata_ch0 : R/W; bitpos: [9:0]; default: 0;
         *  This register stores the data that need to be pushed into DMA Tx FIFO.
         */
        uint32_t outfifo_wdata_ch0:10;
        /** outfifo_push_ch0 : R/W/SC; bitpos: [10]; default: 0;
         *  Set this bit to push data into DMA Tx FIFO.
         */
        uint32_t outfifo_push_ch0:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} h264_dma_out_push_ch0_reg_t;

/** Type of out_link_conf_ch0 register
 *  TX CH0 out_link dscr ctrl register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** outlink_stop_ch0 : R/W/SC; bitpos: [20]; default: 0;
         *  Set this bit to stop dealing with the outlink descriptors.
         */
        uint32_t outlink_stop_ch0:1;
        /** outlink_start_ch0 : R/W/SC; bitpos: [21]; default: 0;
         *  Set this bit to start dealing with the outlink descriptors.
         */
        uint32_t outlink_start_ch0:1;
        /** outlink_restart_ch0 : R/W/SC; bitpos: [22]; default: 0;
         *  Set this bit to restart a new outlink from the last address.
         */
        uint32_t outlink_restart_ch0:1;
        /** outlink_park_ch0 : RO; bitpos: [23]; default: 1;
         *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
         *  is working.
         */
        uint32_t outlink_park_ch0:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} h264_dma_out_link_conf_ch0_reg_t;

/** Type of out_ro_pd_conf_ch0 register
 *  TX CH0 reorder power config register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** out_ro_ram_force_pd_ch0 : R/W; bitpos: [4]; default: 0;
         *  dma reorder ram power down
         */
        uint32_t out_ro_ram_force_pd_ch0:1;
        /** out_ro_ram_force_pu_ch0 : R/W; bitpos: [5]; default: 1;
         *  dma reorder ram power up
         */
        uint32_t out_ro_ram_force_pu_ch0:1;
        /** out_ro_ram_clk_fo_ch0 : R/W; bitpos: [6]; default: 0;
         *  1: Force to open the clock and bypass the gate-clock when accessing the RAM in DMA.
         *  0: A gate-clock will be used when accessing the RAM in DMA.
         */
        uint32_t out_ro_ram_clk_fo_ch0:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} h264_dma_out_ro_pd_conf_ch0_reg_t;

/** Type of out_push_ch1 register
 *  TX CH1 outfifo push register
 */
typedef union {
    struct {
        /** outfifo_wdata_ch1 : R/W; bitpos: [9:0]; default: 0;
         *  This register stores the data that need to be pushed into DMA Tx FIFO.
         */
        uint32_t outfifo_wdata_ch1:10;
        /** outfifo_push_ch1 : R/W/SC; bitpos: [10]; default: 0;
         *  Set this bit to push data into DMA Tx FIFO.
         */
        uint32_t outfifo_push_ch1:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} h264_dma_out_push_ch1_reg_t;

/** Type of out_link_conf_ch1 register
 *  TX CH1 out_link dscr ctrl register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** outlink_stop_ch1 : R/W/SC; bitpos: [20]; default: 0;
         *  Set this bit to stop dealing with the outlink descriptors.
         */
        uint32_t outlink_stop_ch1:1;
        /** outlink_start_ch1 : R/W/SC; bitpos: [21]; default: 0;
         *  Set this bit to start dealing with the outlink descriptors.
         */
        uint32_t outlink_start_ch1:1;
        /** outlink_restart_ch1 : R/W/SC; bitpos: [22]; default: 0;
         *  Set this bit to restart a new outlink from the last address.
         */
        uint32_t outlink_restart_ch1:1;
        /** outlink_park_ch1 : RO; bitpos: [23]; default: 1;
         *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
         *  is working.
         */
        uint32_t outlink_park_ch1:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} h264_dma_out_link_conf_ch1_reg_t;

/** Type of out_push_ch2 register
 *  TX CH2 outfifo push register
 */
typedef union {
    struct {
        /** outfifo_wdata_ch2 : R/W; bitpos: [9:0]; default: 0;
         *  This register stores the data that need to be pushed into DMA Tx FIFO.
         */
        uint32_t outfifo_wdata_ch2:10;
        /** outfifo_push_ch2 : R/W/SC; bitpos: [10]; default: 0;
         *  Set this bit to push data into DMA Tx FIFO.
         */
        uint32_t outfifo_push_ch2:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} h264_dma_out_push_ch2_reg_t;

/** Type of out_link_conf_ch2 register
 *  TX CH2 out_link dscr ctrl register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** outlink_stop_ch2 : R/W/SC; bitpos: [20]; default: 0;
         *  Set this bit to stop dealing with the outlink descriptors.
         */
        uint32_t outlink_stop_ch2:1;
        /** outlink_start_ch2 : R/W/SC; bitpos: [21]; default: 0;
         *  Set this bit to start dealing with the outlink descriptors.
         */
        uint32_t outlink_start_ch2:1;
        /** outlink_restart_ch2 : R/W/SC; bitpos: [22]; default: 0;
         *  Set this bit to restart a new outlink from the last address.
         */
        uint32_t outlink_restart_ch2:1;
        /** outlink_park_ch2 : RO; bitpos: [23]; default: 1;
         *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
         *  is working.
         */
        uint32_t outlink_park_ch2:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} h264_dma_out_link_conf_ch2_reg_t;

/** Type of out_push_ch3 register
 *  TX CH3 outfifo push register
 */
typedef union {
    struct {
        /** outfifo_wdata_ch3 : R/W; bitpos: [9:0]; default: 0;
         *  This register stores the data that need to be pushed into DMA Tx FIFO.
         */
        uint32_t outfifo_wdata_ch3:10;
        /** outfifo_push_ch3 : R/W/SC; bitpos: [10]; default: 0;
         *  Set this bit to push data into DMA Tx FIFO.
         */
        uint32_t outfifo_push_ch3:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} h264_dma_out_push_ch3_reg_t;

/** Type of out_link_conf_ch3 register
 *  TX CH3 out_link dscr ctrl register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** outlink_stop_ch3 : R/W/SC; bitpos: [20]; default: 0;
         *  Set this bit to stop dealing with the outlink descriptors.
         */
        uint32_t outlink_stop_ch3:1;
        /** outlink_start_ch3 : R/W/SC; bitpos: [21]; default: 0;
         *  Set this bit to start dealing with the outlink descriptors.
         */
        uint32_t outlink_start_ch3:1;
        /** outlink_restart_ch3 : R/W/SC; bitpos: [22]; default: 0;
         *  Set this bit to restart a new outlink from the last address.
         */
        uint32_t outlink_restart_ch3:1;
        /** outlink_park_ch3 : RO; bitpos: [23]; default: 1;
         *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
         *  is working.
         */
        uint32_t outlink_park_ch3:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} h264_dma_out_link_conf_ch3_reg_t;

/** Type of out_push_ch4 register
 *  TX CH4 outfifo push register
 */
typedef union {
    struct {
        /** outfifo_wdata_ch4 : R/W; bitpos: [9:0]; default: 0;
         *  This register stores the data that need to be pushed into DMA Tx FIFO.
         */
        uint32_t outfifo_wdata_ch4:10;
        /** outfifo_push_ch4 : R/W/SC; bitpos: [10]; default: 0;
         *  Set this bit to push data into DMA Tx FIFO.
         */
        uint32_t outfifo_push_ch4:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} h264_dma_out_push_ch4_reg_t;

/** Type of out_link_conf_ch4 register
 *  TX CH4 out_link dscr ctrl register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** outlink_stop_ch4 : R/W/SC; bitpos: [20]; default: 0;
         *  Set this bit to stop dealing with the outlink descriptors.
         */
        uint32_t outlink_stop_ch4:1;
        /** outlink_start_ch4 : R/W/SC; bitpos: [21]; default: 0;
         *  Set this bit to start dealing with the outlink descriptors.
         */
        uint32_t outlink_start_ch4:1;
        /** outlink_restart_ch4 : R/W/SC; bitpos: [22]; default: 0;
         *  Set this bit to restart a new outlink from the last address.
         */
        uint32_t outlink_restart_ch4:1;
        /** outlink_park_ch4 : RO; bitpos: [23]; default: 1;
         *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
         *  is working.
         */
        uint32_t outlink_park_ch4:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} h264_dma_out_link_conf_ch4_reg_t;

/** Type of in_conf0_ch0 register
 *  RX CH0 config0 register
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** indscr_burst_en_ch0 : R/W; bitpos: [2]; default: 0;
         *  Set this bit to 1 to enable INCR burst transfer for Rx transmitting link descriptor
         *  when accessing SRAM.
         */
        uint32_t indscr_burst_en_ch0:1;
        /** in_ecc_aes_en_ch0 : R/W; bitpos: [3]; default: 0;
         *  When access address space is ecc/aes area, this bit should be set to 1. In this
         *  case, the start address of square should be 16-bit aligned. The width of square
         *  multiply byte number of one pixel should be 16-bit aligned.
         */
        uint32_t in_ecc_aes_en_ch0:1;
        /** in_check_owner_ch0 : R/W; bitpos: [4]; default: 0;
         *  Set this bit to enable checking the owner attribute of the link descriptor.
         */
        uint32_t in_check_owner_ch0:1;
        uint32_t reserved_5:1;
        /** in_mem_burst_length_ch0 : R/W; bitpos: [8:6]; default: 0;
         *  Block size of Rx channel 0. 0: single      1: 16 bytes      2: 32 bytes    3: 64
         *  bytes    4: 128 bytes
         */
        uint32_t in_mem_burst_length_ch0:3;
        uint32_t reserved_9:3;
        /** in_page_bound_en_ch0 : R/W; bitpos: [12]; default: 0;
         *  Set this bit to 1 to make sure AXI write data don't cross the address boundary
         *  which define by mem_burst_length
         */
        uint32_t in_page_bound_en_ch0:1;
        uint32_t reserved_13:11;
        /** in_rst_ch0 : R/W; bitpos: [24]; default: 0;
         *  Write 1 then write 0 to this bit to reset Rx channel
         */
        uint32_t in_rst_ch0:1;
        /** in_cmd_disable_ch0 : R/W; bitpos: [25]; default: 0;
         *  Write 1 before reset and write 0 after reset
         */
        uint32_t in_cmd_disable_ch0:1;
        /** in_arb_weight_opt_dis_ch0 : R/W; bitpos: [26]; default: 0;
         *  Set this bit to 1 to disable arbiter optimum weight function.
         */
        uint32_t in_arb_weight_opt_dis_ch0:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} h264_dma_in_conf0_ch0_reg_t;

/** Type of in_pop_ch0 register
 *  RX CH0 INFIFO pop register
 */
typedef union {
    struct {
        /** infifo_rdata_ch0 : RO; bitpos: [10:0]; default: 1024;
         *  This register stores the data popping from DMA Rx FIFO.
         */
        uint32_t infifo_rdata_ch0:11;
        /** infifo_pop_ch0 : R/W/SC; bitpos: [11]; default: 0;
         *  Set this bit to pop data from DMA Rx FIFO.
         */
        uint32_t infifo_pop_ch0:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} h264_dma_in_pop_ch0_reg_t;

/** Type of in_link_conf_ch0 register
 *  RX CH0 in_link dscr ctrl register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** inlink_auto_ret_ch0 : R/W; bitpos: [20]; default: 1;
         *  Set this bit to return to current inlink descriptor's address, when there are some
         *  errors in current receiving data.
         */
        uint32_t inlink_auto_ret_ch0:1;
        /** inlink_stop_ch0 : R/W/SC; bitpos: [21]; default: 0;
         *  Set this bit to stop dealing with the inlink descriptors.
         */
        uint32_t inlink_stop_ch0:1;
        /** inlink_start_ch0 : R/W/SC; bitpos: [22]; default: 0;
         *  Set this bit to start dealing with the inlink descriptors.
         */
        uint32_t inlink_start_ch0:1;
        /** inlink_restart_ch0 : R/W/SC; bitpos: [23]; default: 0;
         *  Set this bit to mount a new inlink descriptor.
         */
        uint32_t inlink_restart_ch0:1;
        /** inlink_park_ch0 : RO; bitpos: [24]; default: 1;
         *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
         *  working.
         */
        uint32_t inlink_park_ch0:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} h264_dma_in_link_conf_ch0_reg_t;

/** Type of in_ro_pd_conf_ch0 register
 *  RX CH0 reorder power config register
 */
typedef union {
    struct {
        uint32_t reserved_0:6;
        /** in_ro_ram_clk_fo_ch0 : R/W; bitpos: [6]; default: 0;
         *  1: Force to open the clock and bypass the gate-clock when accessing the RAM in DMA.
         *  0: A gate-clock will be used when accessing the RAM in DMA.
         */
        uint32_t in_ro_ram_clk_fo_ch0:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} h264_dma_in_ro_pd_conf_ch0_reg_t;

/** Type of in_conf0_ch1 register
 *  RX CH1 config0 register
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** indscr_burst_en_ch1 : R/W; bitpos: [2]; default: 0;
         *  Set this bit to 1 to enable INCR burst transfer for Rx transmitting link descriptor
         *  when accessing SRAM.
         */
        uint32_t indscr_burst_en_ch1:1;
        /** in_ecc_aes_en_ch1 : R/W; bitpos: [3]; default: 0;
         *  When access address space is ecc/aes area, this bit should be set to 1. In this
         *  case, the start address of square should be 16-bit aligned. The width of square
         *  multiply byte number of one pixel should be 16-bit aligned.
         */
        uint32_t in_ecc_aes_en_ch1:1;
        /** in_check_owner_ch1 : R/W; bitpos: [4]; default: 0;
         *  Set this bit to enable checking the owner attribute of the link descriptor.
         */
        uint32_t in_check_owner_ch1:1;
        uint32_t reserved_5:1;
        /** in_mem_burst_length_ch1 : R/W; bitpos: [8:6]; default: 0;
         *  Block size of Rx channel 1. 0: single      1: 16 bytes      2: 32 bytes    3: 64
         *  bytes    4: 128 bytes
         */
        uint32_t in_mem_burst_length_ch1:3;
        uint32_t reserved_9:3;
        /** in_page_bound_en_ch1 : R/W; bitpos: [12]; default: 0;
         *  Set this bit to 1 to make sure AXI write data don't cross the address boundary
         *  which define by mem_burst_length
         */
        uint32_t in_page_bound_en_ch1:1;
        uint32_t reserved_13:11;
        /** in_rst_ch1 : R/W; bitpos: [24]; default: 0;
         *  Write 1 then write 0 to this bit to reset Rx channel
         */
        uint32_t in_rst_ch1:1;
        /** in_cmd_disable_ch1 : R/W; bitpos: [25]; default: 0;
         *  Write 1 before reset and write 0 after reset
         */
        uint32_t in_cmd_disable_ch1:1;
        /** in_arb_weight_opt_dis_ch1 : R/W; bitpos: [26]; default: 0;
         *  Set this bit to 1 to disable arbiter optimum weight function.
         */
        uint32_t in_arb_weight_opt_dis_ch1:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} h264_dma_in_conf0_ch1_reg_t;

/** Type of in_pop_ch1 register
 *  RX CH1 INFIFO pop register
 */
typedef union {
    struct {
        /** infifo_rdata_ch1 : RO; bitpos: [10:0]; default: 1024;
         *  This register stores the data popping from DMA Rx FIFO.
         */
        uint32_t infifo_rdata_ch1:11;
        /** infifo_pop_ch1 : R/W/SC; bitpos: [11]; default: 0;
         *  Set this bit to pop data from DMA Rx FIFO.
         */
        uint32_t infifo_pop_ch1:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} h264_dma_in_pop_ch1_reg_t;

/** Type of in_link_conf_ch1 register
 *  RX CH1 in_link dscr ctrl register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** inlink_auto_ret_ch1 : R/W; bitpos: [20]; default: 1;
         *  Set this bit to return to current inlink descriptor's address, when there are some
         *  errors in current receiving data.
         */
        uint32_t inlink_auto_ret_ch1:1;
        /** inlink_stop_ch1 : R/W/SC; bitpos: [21]; default: 0;
         *  Set this bit to stop dealing with the inlink descriptors.
         */
        uint32_t inlink_stop_ch1:1;
        /** inlink_start_ch1 : R/W/SC; bitpos: [22]; default: 0;
         *  Set this bit to start dealing with the inlink descriptors.
         */
        uint32_t inlink_start_ch1:1;
        /** inlink_restart_ch1 : R/W/SC; bitpos: [23]; default: 0;
         *  Set this bit to mount a new inlink descriptor.
         */
        uint32_t inlink_restart_ch1:1;
        /** inlink_park_ch1 : RO; bitpos: [24]; default: 1;
         *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
         *  working.
         */
        uint32_t inlink_park_ch1:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} h264_dma_in_link_conf_ch1_reg_t;

/** Type of in_conf0_ch2 register
 *  RX CH2 config0 register
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** indscr_burst_en_ch2 : R/W; bitpos: [2]; default: 0;
         *  Set this bit to 1 to enable INCR burst transfer for Rx transmitting link descriptor
         *  when accessing SRAM.
         */
        uint32_t indscr_burst_en_ch2:1;
        /** in_ecc_aes_en_ch2 : R/W; bitpos: [3]; default: 0;
         *  When access address space is ecc/aes area, this bit should be set to 1. In this
         *  case, the start address of square should be 16-bit aligned. The width of square
         *  multiply byte number of one pixel should be 16-bit aligned.
         */
        uint32_t in_ecc_aes_en_ch2:1;
        /** in_check_owner_ch2 : R/W; bitpos: [4]; default: 0;
         *  Set this bit to enable checking the owner attribute of the link descriptor.
         */
        uint32_t in_check_owner_ch2:1;
        uint32_t reserved_5:1;
        /** in_mem_burst_length_ch2 : R/W; bitpos: [8:6]; default: 0;
         *  Block size of Rx channel 2. 0: single      1: 16 bytes      2: 32 bytes    3: 64
         *  bytes    4: 128 bytes
         */
        uint32_t in_mem_burst_length_ch2:3;
        uint32_t reserved_9:3;
        /** in_page_bound_en_ch2 : R/W; bitpos: [12]; default: 0;
         *  Set this bit to 1 to make sure AXI write data don't cross the address boundary
         *  which define by mem_burst_length
         */
        uint32_t in_page_bound_en_ch2:1;
        uint32_t reserved_13:11;
        /** in_rst_ch2 : R/W; bitpos: [24]; default: 0;
         *  Write 1 then write 0 to this bit to reset Rx channel
         */
        uint32_t in_rst_ch2:1;
        /** in_cmd_disable_ch2 : R/W; bitpos: [25]; default: 0;
         *  Write 1 before reset and write 0 after reset
         */
        uint32_t in_cmd_disable_ch2:1;
        /** in_arb_weight_opt_dis_ch2 : R/W; bitpos: [26]; default: 0;
         *  Set this bit to 1 to disable arbiter optimum weight function.
         */
        uint32_t in_arb_weight_opt_dis_ch2:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} h264_dma_in_conf0_ch2_reg_t;

/** Type of in_pop_ch2 register
 *  RX CH2 INFIFO pop register
 */
typedef union {
    struct {
        /** infifo_rdata_ch2 : RO; bitpos: [10:0]; default: 1024;
         *  This register stores the data popping from DMA Rx FIFO.
         */
        uint32_t infifo_rdata_ch2:11;
        /** infifo_pop_ch2 : R/W/SC; bitpos: [11]; default: 0;
         *  Set this bit to pop data from DMA Rx FIFO.
         */
        uint32_t infifo_pop_ch2:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} h264_dma_in_pop_ch2_reg_t;

/** Type of in_link_conf_ch2 register
 *  RX CH2 in_link dscr ctrl register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** inlink_auto_ret_ch2 : R/W; bitpos: [20]; default: 1;
         *  Set this bit to return to current inlink descriptor's address, when there are some
         *  errors in current receiving data.
         */
        uint32_t inlink_auto_ret_ch2:1;
        /** inlink_stop_ch2 : R/W/SC; bitpos: [21]; default: 0;
         *  Set this bit to stop dealing with the inlink descriptors.
         */
        uint32_t inlink_stop_ch2:1;
        /** inlink_start_ch2 : R/W/SC; bitpos: [22]; default: 0;
         *  Set this bit to start dealing with the inlink descriptors.
         */
        uint32_t inlink_start_ch2:1;
        /** inlink_restart_ch2 : R/W/SC; bitpos: [23]; default: 0;
         *  Set this bit to mount a new inlink descriptor.
         */
        uint32_t inlink_restart_ch2:1;
        /** inlink_park_ch2 : RO; bitpos: [24]; default: 1;
         *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
         *  working.
         */
        uint32_t inlink_park_ch2:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} h264_dma_in_link_conf_ch2_reg_t;

/** Type of in_conf0_ch3 register
 *  RX CH3 config0 register
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** indscr_burst_en_ch3 : R/W; bitpos: [2]; default: 0;
         *  Set this bit to 1 to enable INCR burst transfer for Rx transmitting link descriptor
         *  when accessing SRAM.
         */
        uint32_t indscr_burst_en_ch3:1;
        /** in_ecc_aes_en_ch3 : R/W; bitpos: [3]; default: 0;
         *  When access address space is ecc/aes area, this bit should be set to 1. In this
         *  case, the start address of square should be 16-bit aligned. The width of square
         *  multiply byte number of one pixel should be 16-bit aligned.
         */
        uint32_t in_ecc_aes_en_ch3:1;
        /** in_check_owner_ch3 : R/W; bitpos: [4]; default: 0;
         *  Set this bit to enable checking the owner attribute of the link descriptor.
         */
        uint32_t in_check_owner_ch3:1;
        uint32_t reserved_5:1;
        /** in_mem_burst_length_ch3 : R/W; bitpos: [8:6]; default: 0;
         *  Block size of Rx channel 1. 0: single      1: 16 bytes      2: 32 bytes    3: 64
         *  bytes    4: 128 bytes
         */
        uint32_t in_mem_burst_length_ch3:3;
        uint32_t reserved_9:3;
        /** in_page_bound_en_ch3 : R/W; bitpos: [12]; default: 0;
         *  Set this bit to 1 to make sure AXI write data don't cross the address boundary
         *  which define by mem_burst_length
         */
        uint32_t in_page_bound_en_ch3:1;
        uint32_t reserved_13:11;
        /** in_rst_ch3 : R/W; bitpos: [24]; default: 0;
         *  Write 1 then write 0 to this bit to reset Rx channel
         */
        uint32_t in_rst_ch3:1;
        /** in_cmd_disable_ch3 : R/W; bitpos: [25]; default: 0;
         *  Write 1 before reset and write 0 after reset
         */
        uint32_t in_cmd_disable_ch3:1;
        /** in_arb_weight_opt_dis_ch3 : R/W; bitpos: [26]; default: 0;
         *  Set this bit to 1 to disable arbiter optimum weight function.
         */
        uint32_t in_arb_weight_opt_dis_ch3:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} h264_dma_in_conf0_ch3_reg_t;

/** Type of in_pop_ch3 register
 *  RX CH3 INFIFO pop register
 */
typedef union {
    struct {
        /** infifo_rdata_ch3 : RO; bitpos: [10:0]; default: 1024;
         *  This register stores the data popping from DMA Rx FIFO.
         */
        uint32_t infifo_rdata_ch3:11;
        /** infifo_pop_ch3 : R/W/SC; bitpos: [11]; default: 0;
         *  Set this bit to pop data from DMA Rx FIFO.
         */
        uint32_t infifo_pop_ch3:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} h264_dma_in_pop_ch3_reg_t;

/** Type of in_link_conf_ch3 register
 *  RX CH3 in_link dscr ctrl register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** inlink_auto_ret_ch3 : R/W; bitpos: [20]; default: 1;
         *  Set this bit to return to current inlink descriptor's address, when there are some
         *  errors in current receiving data.
         */
        uint32_t inlink_auto_ret_ch3:1;
        /** inlink_stop_ch3 : R/W/SC; bitpos: [21]; default: 0;
         *  Set this bit to stop dealing with the inlink descriptors.
         */
        uint32_t inlink_stop_ch3:1;
        /** inlink_start_ch3 : R/W/SC; bitpos: [22]; default: 0;
         *  Set this bit to start dealing with the inlink descriptors.
         */
        uint32_t inlink_start_ch3:1;
        /** inlink_restart_ch3 : R/W/SC; bitpos: [23]; default: 0;
         *  Set this bit to mount a new inlink descriptor.
         */
        uint32_t inlink_restart_ch3:1;
        /** inlink_park_ch3 : RO; bitpos: [24]; default: 1;
         *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
         *  working.
         */
        uint32_t inlink_park_ch3:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} h264_dma_in_link_conf_ch3_reg_t;

/** Type of in_conf0_ch4 register
 *  RX CH4 config0 register
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** indscr_burst_en_ch4 : R/W; bitpos: [2]; default: 0;
         *  Set this bit to 1 to enable INCR burst transfer for Rx transmitting link descriptor
         *  when accessing SRAM.
         */
        uint32_t indscr_burst_en_ch4:1;
        /** in_ecc_aes_en_ch4 : R/W; bitpos: [3]; default: 0;
         *  When access address space is ecc/aes area, this bit should be set to 1. In this
         *  case, the start address of square should be 16-bit aligned. The width of square
         *  multiply byte number of one pixel should be 16-bit aligned.
         */
        uint32_t in_ecc_aes_en_ch4:1;
        /** in_check_owner_ch4 : R/W; bitpos: [4]; default: 0;
         *  Set this bit to enable checking the owner attribute of the link descriptor.
         */
        uint32_t in_check_owner_ch4:1;
        uint32_t reserved_5:1;
        /** in_mem_burst_length_ch4 : R/W; bitpos: [8:6]; default: 0;
         *  Block size of Rx channel 1. 0: single      1: 16 bytes      2: 32 bytes    3: 64
         *  bytes    4: 128 bytes
         */
        uint32_t in_mem_burst_length_ch4:3;
        uint32_t reserved_9:3;
        /** in_page_bound_en_ch4 : R/W; bitpos: [12]; default: 0;
         *  Set this bit to 1 to make sure AXI write data don't cross the address boundary
         *  which define by mem_burst_length
         */
        uint32_t in_page_bound_en_ch4:1;
        uint32_t reserved_13:11;
        /** in_rst_ch4 : R/W; bitpos: [24]; default: 0;
         *  Write 1 then write 0 to this bit to reset Rx channel
         */
        uint32_t in_rst_ch4:1;
        /** in_cmd_disable_ch4 : R/W; bitpos: [25]; default: 0;
         *  Write 1 before reset and write 0 after reset
         */
        uint32_t in_cmd_disable_ch4:1;
        /** in_arb_weight_opt_dis_ch4 : R/W; bitpos: [26]; default: 0;
         *  Set this bit to 1 to disable arbiter optimum weight function.
         */
        uint32_t in_arb_weight_opt_dis_ch4:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} h264_dma_in_conf0_ch4_reg_t;

/** Type of in_pop_ch4 register
 *  RX CH4 INFIFO pop register
 */
typedef union {
    struct {
        /** infifo_rdata_ch4 : RO; bitpos: [10:0]; default: 1024;
         *  This register stores the data popping from DMA Rx FIFO.
         */
        uint32_t infifo_rdata_ch4:11;
        /** infifo_pop_ch4 : R/W/SC; bitpos: [11]; default: 0;
         *  Set this bit to pop data from DMA Rx FIFO.
         */
        uint32_t infifo_pop_ch4:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} h264_dma_in_pop_ch4_reg_t;

/** Type of in_link_conf_ch4 register
 *  RX CH4 in_link dscr ctrl register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** inlink_auto_ret_ch4 : R/W; bitpos: [20]; default: 1;
         *  Set this bit to return to current inlink descriptor's address, when there are some
         *  errors in current receiving data.
         */
        uint32_t inlink_auto_ret_ch4:1;
        /** inlink_stop_ch4 : R/W/SC; bitpos: [21]; default: 0;
         *  Set this bit to stop dealing with the inlink descriptors.
         */
        uint32_t inlink_stop_ch4:1;
        /** inlink_start_ch4 : R/W/SC; bitpos: [22]; default: 0;
         *  Set this bit to start dealing with the inlink descriptors.
         */
        uint32_t inlink_start_ch4:1;
        /** inlink_restart_ch4 : R/W/SC; bitpos: [23]; default: 0;
         *  Set this bit to mount a new inlink descriptor.
         */
        uint32_t inlink_restart_ch4:1;
        /** inlink_park_ch4 : RO; bitpos: [24]; default: 1;
         *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
         *  working.
         */
        uint32_t inlink_park_ch4:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} h264_dma_in_link_conf_ch4_reg_t;

/** Type of in_conf0_ch5 register
 *  RX CH5 config0 register
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
        /** in_ecc_aes_en_ch5 : R/W; bitpos: [3]; default: 0;
         *  When access address space is ecc/aes area, this bit should be set to 1. In this
         *  case, the start address of square should be 16-bit aligned. The width of square
         *  multiply byte number of one pixel should be 16-bit aligned.
         */
        uint32_t in_ecc_aes_en_ch5:1;
        uint32_t reserved_4:2;
        /** in_mem_burst_length_ch5 : R/W; bitpos: [8:6]; default: 0;
         *  Block size of Rx channel 1. 0: single      1: 16 bytes      2: 32 bytes    3: 64
         *  bytes    4: 128 bytes
         */
        uint32_t in_mem_burst_length_ch5:3;
        uint32_t reserved_9:3;
        /** in_page_bound_en_ch5 : R/W; bitpos: [12]; default: 0;
         *  Set this bit to 1 to make sure AXI write data don't cross the address boundary
         *  which define by mem_burst_length
         */
        uint32_t in_page_bound_en_ch5:1;
        uint32_t reserved_13:11;
        /** in_rst_ch5 : R/W; bitpos: [24]; default: 0;
         *  Write 1 then write 0 to this bit to reset Rx channel
         */
        uint32_t in_rst_ch5:1;
        /** in_cmd_disable_ch5 : R/W; bitpos: [25]; default: 0;
         *  Write 1 before reset and write 0 after reset
         */
        uint32_t in_cmd_disable_ch5:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} h264_dma_in_conf0_ch5_reg_t;

/** Type of in_pop_ch5 register
 *  RX CH5 INFIFO pop register
 */
typedef union {
    struct {
        /** infifo_rdata_ch5 : RO; bitpos: [10:0]; default: 1024;
         *  This register stores the data popping from DMA Rx FIFO.
         */
        uint32_t infifo_rdata_ch5:11;
        /** infifo_pop_ch5 : R/W/SC; bitpos: [11]; default: 0;
         *  Set this bit to pop data from DMA Rx FIFO.
         */
        uint32_t infifo_pop_ch5:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} h264_dma_in_pop_ch5_reg_t;

/** Type of rst_conf register
 *  axi reset config register
 */
typedef union {
    struct {
        /** inter_axim_rd_rst : R/W; bitpos: [0]; default: 0;
         *  Write 1 then write 0  to this bit to reset axi master read data FIFO.
         */
        uint32_t inter_axim_rd_rst:1;
        /** inter_axim_wr_rst : R/W; bitpos: [1]; default: 0;
         *  Write 1 then write 0  to this bit to reset axi master write data FIFO.
         */
        uint32_t inter_axim_wr_rst:1;
        /** exter_axim_rd_rst : R/W; bitpos: [2]; default: 0;
         *  Write 1 then write 0  to this bit to reset axi master read data FIFO.
         */
        uint32_t exter_axim_rd_rst:1;
        /** exter_axim_wr_rst : R/W; bitpos: [3]; default: 0;
         *  Write 1 then write 0  to this bit to reset axi master write data FIFO.
         */
        uint32_t exter_axim_wr_rst:1;
        /** clk_en : R/W; bitpos: [4]; default: 0;
         *  1'h1: Force clock on for register. 1'h0: Support clock only when application writes
         *  registers.
         */
        uint32_t clk_en:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} h264_dma_rst_conf_reg_t;


/** Group: Interrupt Registers */
/** Type of out_int_raw_ch0 register
 *  TX CH0  interrupt raw register
 */
typedef union {
    struct {
        /** out_done_ch0_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one outlink
         *  descriptor has been transmitted to peripherals for Tx channel 0.
         */
        uint32_t out_done_ch0_int_raw:1;
        /** out_eof_ch0_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one outlink
         *  descriptor has been read from memory for Tx channel 0.
         */
        uint32_t out_eof_ch0_int_raw:1;
        /** out_dscr_err_ch0_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt bit turns to high level when detecting outlink descriptor error,
         *  including owner error, the second and third word error of outlink descriptor for Tx
         *  channel 0.
         */
        uint32_t out_dscr_err_ch0_int_raw:1;
        /** out_total_eof_ch0_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt bit turns to high level when data corresponding a outlink
         *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
         *  channel 0.
         */
        uint32_t out_total_eof_ch0_int_raw:1;
        /** outfifo_ovf_l1_ch0_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is overflow.
         */
        uint32_t outfifo_ovf_l1_ch0_int_raw:1;
        /** outfifo_udf_l1_ch0_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is underflow.
         */
        uint32_t outfifo_udf_l1_ch0_int_raw:1;
        /** outfifo_ovf_l2_ch0_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is overflow.
         */
        uint32_t outfifo_ovf_l2_ch0_int_raw:1;
        /** outfifo_udf_l2_ch0_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is underflow.
         */
        uint32_t outfifo_udf_l2_ch0_int_raw:1;
        /** out_dscr_task_ovf_ch0_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
         */
        uint32_t out_dscr_task_ovf_ch0_int_raw:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_out_int_raw_ch0_reg_t;

/** Type of out_int_ena_ch0 register
 *  TX CH0  interrupt ena register
 */
typedef union {
    struct {
        /** out_done_ch0_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_ch0_int_ena:1;
        /** out_eof_ch0_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_ch0_int_ena:1;
        /** out_dscr_err_ch0_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_ch0_int_ena:1;
        /** out_total_eof_ch0_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_ch0_int_ena:1;
        /** outfifo_ovf_l1_ch0_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l1_ch0_int_ena:1;
        /** outfifo_udf_l1_ch0_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l1_ch0_int_ena:1;
        /** outfifo_ovf_l2_ch0_int_ena : R/W; bitpos: [6]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l2_ch0_int_ena:1;
        /** outfifo_udf_l2_ch0_int_ena : R/W; bitpos: [7]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l2_ch0_int_ena:1;
        /** out_dscr_task_ovf_ch0_int_ena : R/W; bitpos: [8]; default: 0;
         *  The interrupt enable bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t out_dscr_task_ovf_ch0_int_ena:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_out_int_ena_ch0_reg_t;

/** Type of out_int_st_ch0 register
 *  TX CH0  interrupt st register
 */
typedef union {
    struct {
        /** out_done_ch0_int_st : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_ch0_int_st:1;
        /** out_eof_ch0_int_st : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_ch0_int_st:1;
        /** out_dscr_err_ch0_int_st : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_ch0_int_st:1;
        /** out_total_eof_ch0_int_st : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_ch0_int_st:1;
        /** outfifo_ovf_l1_ch0_int_st : RO; bitpos: [4]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l1_ch0_int_st:1;
        /** outfifo_udf_l1_ch0_int_st : RO; bitpos: [5]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l1_ch0_int_st:1;
        /** outfifo_ovf_l2_ch0_int_st : RO; bitpos: [6]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l2_ch0_int_st:1;
        /** outfifo_udf_l2_ch0_int_st : RO; bitpos: [7]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l2_ch0_int_st:1;
        /** out_dscr_task_ovf_ch0_int_st : RO; bitpos: [8]; default: 0;
         *  The raw interrupt status bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t out_dscr_task_ovf_ch0_int_st:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_out_int_st_ch0_reg_t;

/** Type of out_int_clr_ch0 register
 *  TX CH0  interrupt clr register
 */
typedef union {
    struct {
        /** out_done_ch0_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_ch0_int_clr:1;
        /** out_eof_ch0_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_ch0_int_clr:1;
        /** out_dscr_err_ch0_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_ch0_int_clr:1;
        /** out_total_eof_ch0_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_ch0_int_clr:1;
        /** outfifo_ovf_l1_ch0_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l1_ch0_int_clr:1;
        /** outfifo_udf_l1_ch0_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l1_ch0_int_clr:1;
        /** outfifo_ovf_l2_ch0_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l2_ch0_int_clr:1;
        /** outfifo_udf_l2_ch0_int_clr : WT; bitpos: [7]; default: 0;
         *  Set this bit to clear the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l2_ch0_int_clr:1;
        /** out_dscr_task_ovf_ch0_int_clr : WT; bitpos: [8]; default: 0;
         *  Set this bit to clear the OUT_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t out_dscr_task_ovf_ch0_int_clr:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_out_int_clr_ch0_reg_t;

/** Type of out_int_raw_ch1 register
 *  TX CH1  interrupt raw register
 */
typedef union {
    struct {
        /** out_done_ch1_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one outlink
         *  descriptor has been transmitted to peripherals for Tx channel 0.
         */
        uint32_t out_done_ch1_int_raw:1;
        /** out_eof_ch1_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one outlink
         *  descriptor has been read from memory for Tx channel 0.
         */
        uint32_t out_eof_ch1_int_raw:1;
        /** out_dscr_err_ch1_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt bit turns to high level when detecting outlink descriptor error,
         *  including owner error, the second and third word error of outlink descriptor for Tx
         *  channel 0.
         */
        uint32_t out_dscr_err_ch1_int_raw:1;
        /** out_total_eof_ch1_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt bit turns to high level when data corresponding a outlink
         *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
         *  channel 0.
         */
        uint32_t out_total_eof_ch1_int_raw:1;
        /** outfifo_ovf_l1_ch1_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is overflow.
         */
        uint32_t outfifo_ovf_l1_ch1_int_raw:1;
        /** outfifo_udf_l1_ch1_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is underflow.
         */
        uint32_t outfifo_udf_l1_ch1_int_raw:1;
        /** outfifo_ovf_l2_ch1_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is overflow.
         */
        uint32_t outfifo_ovf_l2_ch1_int_raw:1;
        /** outfifo_udf_l2_ch1_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is underflow.
         */
        uint32_t outfifo_udf_l2_ch1_int_raw:1;
        /** out_dscr_task_ovf_ch1_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
         */
        uint32_t out_dscr_task_ovf_ch1_int_raw:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_out_int_raw_ch1_reg_t;

/** Type of out_int_ena_ch1 register
 *  TX CH1  interrupt ena register
 */
typedef union {
    struct {
        /** out_done_ch1_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_ch1_int_ena:1;
        /** out_eof_ch1_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_ch1_int_ena:1;
        /** out_dscr_err_ch1_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_ch1_int_ena:1;
        /** out_total_eof_ch1_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_ch1_int_ena:1;
        /** outfifo_ovf_l1_ch1_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l1_ch1_int_ena:1;
        /** outfifo_udf_l1_ch1_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l1_ch1_int_ena:1;
        /** outfifo_ovf_l2_ch1_int_ena : R/W; bitpos: [6]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l2_ch1_int_ena:1;
        /** outfifo_udf_l2_ch1_int_ena : R/W; bitpos: [7]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l2_ch1_int_ena:1;
        /** out_dscr_task_ovf_ch1_int_ena : R/W; bitpos: [8]; default: 0;
         *  The interrupt enable bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t out_dscr_task_ovf_ch1_int_ena:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_out_int_ena_ch1_reg_t;

/** Type of out_int_st_ch1 register
 *  TX CH1  interrupt st register
 */
typedef union {
    struct {
        /** out_done_ch1_int_st : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_ch1_int_st:1;
        /** out_eof_ch1_int_st : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_ch1_int_st:1;
        /** out_dscr_err_ch1_int_st : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_ch1_int_st:1;
        /** out_total_eof_ch1_int_st : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_ch1_int_st:1;
        /** outfifo_ovf_l1_ch1_int_st : RO; bitpos: [4]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l1_ch1_int_st:1;
        /** outfifo_udf_l1_ch1_int_st : RO; bitpos: [5]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l1_ch1_int_st:1;
        /** outfifo_ovf_l2_ch1_int_st : RO; bitpos: [6]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l2_ch1_int_st:1;
        /** outfifo_udf_l2_ch1_int_st : RO; bitpos: [7]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l2_ch1_int_st:1;
        /** out_dscr_task_ovf_ch1_int_st : RO; bitpos: [8]; default: 0;
         *  The raw interrupt status bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t out_dscr_task_ovf_ch1_int_st:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_out_int_st_ch1_reg_t;

/** Type of out_int_clr_ch1 register
 *  TX CH1  interrupt clr register
 */
typedef union {
    struct {
        /** out_done_ch1_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_ch1_int_clr:1;
        /** out_eof_ch1_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_ch1_int_clr:1;
        /** out_dscr_err_ch1_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_ch1_int_clr:1;
        /** out_total_eof_ch1_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_ch1_int_clr:1;
        /** outfifo_ovf_l1_ch1_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l1_ch1_int_clr:1;
        /** outfifo_udf_l1_ch1_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l1_ch1_int_clr:1;
        /** outfifo_ovf_l2_ch1_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l2_ch1_int_clr:1;
        /** outfifo_udf_l2_ch1_int_clr : WT; bitpos: [7]; default: 0;
         *  Set this bit to clear the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l2_ch1_int_clr:1;
        /** out_dscr_task_ovf_ch1_int_clr : WT; bitpos: [8]; default: 0;
         *  Set this bit to clear the OUT_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t out_dscr_task_ovf_ch1_int_clr:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_out_int_clr_ch1_reg_t;

/** Type of out_int_raw_ch2 register
 *  TX CH2  interrupt raw register
 */
typedef union {
    struct {
        /** out_done_ch2_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one outlink
         *  descriptor has been transmitted to peripherals for Tx channel 0.
         */
        uint32_t out_done_ch2_int_raw:1;
        /** out_eof_ch2_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one outlink
         *  descriptor has been read from memory for Tx channel 0.
         */
        uint32_t out_eof_ch2_int_raw:1;
        /** out_dscr_err_ch2_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt bit turns to high level when detecting outlink descriptor error,
         *  including owner error, the second and third word error of outlink descriptor for Tx
         *  channel 0.
         */
        uint32_t out_dscr_err_ch2_int_raw:1;
        /** out_total_eof_ch2_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt bit turns to high level when data corresponding a outlink
         *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
         *  channel 0.
         */
        uint32_t out_total_eof_ch2_int_raw:1;
        /** outfifo_ovf_l1_ch2_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is overflow.
         */
        uint32_t outfifo_ovf_l1_ch2_int_raw:1;
        /** outfifo_udf_l1_ch2_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is underflow.
         */
        uint32_t outfifo_udf_l1_ch2_int_raw:1;
        /** outfifo_ovf_l2_ch2_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is overflow.
         */
        uint32_t outfifo_ovf_l2_ch2_int_raw:1;
        /** outfifo_udf_l2_ch2_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is underflow.
         */
        uint32_t outfifo_udf_l2_ch2_int_raw:1;
        /** out_dscr_task_ovf_ch2_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
         */
        uint32_t out_dscr_task_ovf_ch2_int_raw:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_out_int_raw_ch2_reg_t;

/** Type of out_int_ena_ch2 register
 *  TX CH2  interrupt ena register
 */
typedef union {
    struct {
        /** out_done_ch2_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_ch2_int_ena:1;
        /** out_eof_ch2_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_ch2_int_ena:1;
        /** out_dscr_err_ch2_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_ch2_int_ena:1;
        /** out_total_eof_ch2_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_ch2_int_ena:1;
        /** outfifo_ovf_l1_ch2_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l1_ch2_int_ena:1;
        /** outfifo_udf_l1_ch2_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l1_ch2_int_ena:1;
        /** outfifo_ovf_l2_ch2_int_ena : R/W; bitpos: [6]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l2_ch2_int_ena:1;
        /** outfifo_udf_l2_ch2_int_ena : R/W; bitpos: [7]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l2_ch2_int_ena:1;
        /** out_dscr_task_ovf_ch2_int_ena : R/W; bitpos: [8]; default: 0;
         *  The interrupt enable bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t out_dscr_task_ovf_ch2_int_ena:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_out_int_ena_ch2_reg_t;

/** Type of out_int_st_ch2 register
 *  TX CH2  interrupt st register
 */
typedef union {
    struct {
        /** out_done_ch2_int_st : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_ch2_int_st:1;
        /** out_eof_ch2_int_st : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_ch2_int_st:1;
        /** out_dscr_err_ch2_int_st : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_ch2_int_st:1;
        /** out_total_eof_ch2_int_st : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_ch2_int_st:1;
        /** outfifo_ovf_l1_ch2_int_st : RO; bitpos: [4]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l1_ch2_int_st:1;
        /** outfifo_udf_l1_ch2_int_st : RO; bitpos: [5]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l1_ch2_int_st:1;
        /** outfifo_ovf_l2_ch2_int_st : RO; bitpos: [6]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l2_ch2_int_st:1;
        /** outfifo_udf_l2_ch2_int_st : RO; bitpos: [7]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l2_ch2_int_st:1;
        /** out_dscr_task_ovf_ch2_int_st : RO; bitpos: [8]; default: 0;
         *  The raw interrupt status bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t out_dscr_task_ovf_ch2_int_st:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_out_int_st_ch2_reg_t;

/** Type of out_int_clr_ch2 register
 *  TX CH2  interrupt clr register
 */
typedef union {
    struct {
        /** out_done_ch2_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_ch2_int_clr:1;
        /** out_eof_ch2_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_ch2_int_clr:1;
        /** out_dscr_err_ch2_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_ch2_int_clr:1;
        /** out_total_eof_ch2_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_ch2_int_clr:1;
        /** outfifo_ovf_l1_ch2_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l1_ch2_int_clr:1;
        /** outfifo_udf_l1_ch2_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l1_ch2_int_clr:1;
        /** outfifo_ovf_l2_ch2_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l2_ch2_int_clr:1;
        /** outfifo_udf_l2_ch2_int_clr : WT; bitpos: [7]; default: 0;
         *  Set this bit to clear the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l2_ch2_int_clr:1;
        /** out_dscr_task_ovf_ch2_int_clr : WT; bitpos: [8]; default: 0;
         *  Set this bit to clear the OUT_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t out_dscr_task_ovf_ch2_int_clr:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_out_int_clr_ch2_reg_t;

/** Type of out_int_raw_ch3 register
 *  TX CH3  interrupt raw register
 */
typedef union {
    struct {
        /** out_done_ch3_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one outlink
         *  descriptor has been transmitted to peripherals for Tx channel 0.
         */
        uint32_t out_done_ch3_int_raw:1;
        /** out_eof_ch3_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one outlink
         *  descriptor has been read from memory for Tx channel 0.
         */
        uint32_t out_eof_ch3_int_raw:1;
        /** out_dscr_err_ch3_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt bit turns to high level when detecting outlink descriptor error,
         *  including owner error, the second and third word error of outlink descriptor for Tx
         *  channel 0.
         */
        uint32_t out_dscr_err_ch3_int_raw:1;
        /** out_total_eof_ch3_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt bit turns to high level when data corresponding a outlink
         *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
         *  channel 0.
         */
        uint32_t out_total_eof_ch3_int_raw:1;
        /** outfifo_ovf_l1_ch3_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is overflow.
         */
        uint32_t outfifo_ovf_l1_ch3_int_raw:1;
        /** outfifo_udf_l1_ch3_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is underflow.
         */
        uint32_t outfifo_udf_l1_ch3_int_raw:1;
        /** outfifo_ovf_l2_ch3_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is overflow.
         */
        uint32_t outfifo_ovf_l2_ch3_int_raw:1;
        /** outfifo_udf_l2_ch3_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is underflow.
         */
        uint32_t outfifo_udf_l2_ch3_int_raw:1;
        /** out_dscr_task_ovf_ch3_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
         */
        uint32_t out_dscr_task_ovf_ch3_int_raw:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_out_int_raw_ch3_reg_t;

/** Type of out_int_ena_ch3 register
 *  TX CH3  interrupt ena register
 */
typedef union {
    struct {
        /** out_done_ch3_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_ch3_int_ena:1;
        /** out_eof_ch3_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_ch3_int_ena:1;
        /** out_dscr_err_ch3_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_ch3_int_ena:1;
        /** out_total_eof_ch3_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_ch3_int_ena:1;
        /** outfifo_ovf_l1_ch3_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l1_ch3_int_ena:1;
        /** outfifo_udf_l1_ch3_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l1_ch3_int_ena:1;
        /** outfifo_ovf_l2_ch3_int_ena : R/W; bitpos: [6]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l2_ch3_int_ena:1;
        /** outfifo_udf_l2_ch3_int_ena : R/W; bitpos: [7]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l2_ch3_int_ena:1;
        /** out_dscr_task_ovf_ch3_int_ena : R/W; bitpos: [8]; default: 0;
         *  The interrupt enable bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t out_dscr_task_ovf_ch3_int_ena:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_out_int_ena_ch3_reg_t;

/** Type of out_int_st_ch3 register
 *  TX CH3  interrupt st register
 */
typedef union {
    struct {
        /** out_done_ch3_int_st : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_ch3_int_st:1;
        /** out_eof_ch3_int_st : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_ch3_int_st:1;
        /** out_dscr_err_ch3_int_st : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_ch3_int_st:1;
        /** out_total_eof_ch3_int_st : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_ch3_int_st:1;
        /** outfifo_ovf_l1_ch3_int_st : RO; bitpos: [4]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l1_ch3_int_st:1;
        /** outfifo_udf_l1_ch3_int_st : RO; bitpos: [5]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l1_ch3_int_st:1;
        /** outfifo_ovf_l2_ch3_int_st : RO; bitpos: [6]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l2_ch3_int_st:1;
        /** outfifo_udf_l2_ch3_int_st : RO; bitpos: [7]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l2_ch3_int_st:1;
        /** out_dscr_task_ovf_ch3_int_st : RO; bitpos: [8]; default: 0;
         *  The raw interrupt status bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t out_dscr_task_ovf_ch3_int_st:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_out_int_st_ch3_reg_t;

/** Type of out_int_clr_ch3 register
 *  TX CH3  interrupt clr register
 */
typedef union {
    struct {
        /** out_done_ch3_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_ch3_int_clr:1;
        /** out_eof_ch3_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_ch3_int_clr:1;
        /** out_dscr_err_ch3_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_ch3_int_clr:1;
        /** out_total_eof_ch3_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_ch3_int_clr:1;
        /** outfifo_ovf_l1_ch3_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l1_ch3_int_clr:1;
        /** outfifo_udf_l1_ch3_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l1_ch3_int_clr:1;
        /** outfifo_ovf_l2_ch3_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l2_ch3_int_clr:1;
        /** outfifo_udf_l2_ch3_int_clr : WT; bitpos: [7]; default: 0;
         *  Set this bit to clear the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l2_ch3_int_clr:1;
        /** out_dscr_task_ovf_ch3_int_clr : WT; bitpos: [8]; default: 0;
         *  Set this bit to clear the OUT_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t out_dscr_task_ovf_ch3_int_clr:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_out_int_clr_ch3_reg_t;

/** Type of out_int_raw_ch4 register
 *  TX CH4  interrupt raw register
 */
typedef union {
    struct {
        /** out_done_ch4_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one outlink
         *  descriptor has been transmitted to peripherals for Tx channel 0.
         */
        uint32_t out_done_ch4_int_raw:1;
        /** out_eof_ch4_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one outlink
         *  descriptor has been read from memory for Tx channel 0.
         */
        uint32_t out_eof_ch4_int_raw:1;
        /** out_dscr_err_ch4_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt bit turns to high level when detecting outlink descriptor error,
         *  including owner error, the second and third word error of outlink descriptor for Tx
         *  channel 0.
         */
        uint32_t out_dscr_err_ch4_int_raw:1;
        /** out_total_eof_ch4_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt bit turns to high level when data corresponding a outlink
         *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
         *  channel 0.
         */
        uint32_t out_total_eof_ch4_int_raw:1;
        /** outfifo_ovf_l1_ch4_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is overflow.
         */
        uint32_t outfifo_ovf_l1_ch4_int_raw:1;
        /** outfifo_udf_l1_ch4_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is underflow.
         */
        uint32_t outfifo_udf_l1_ch4_int_raw:1;
        /** outfifo_ovf_l2_ch4_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is overflow.
         */
        uint32_t outfifo_ovf_l2_ch4_int_raw:1;
        /** outfifo_udf_l2_ch4_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is underflow.
         */
        uint32_t outfifo_udf_l2_ch4_int_raw:1;
        /** out_dscr_task_ovf_ch4_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
         */
        uint32_t out_dscr_task_ovf_ch4_int_raw:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_out_int_raw_ch4_reg_t;

/** Type of out_int_ena_ch4 register
 *  TX CH4  interrupt ena register
 */
typedef union {
    struct {
        /** out_done_ch4_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_ch4_int_ena:1;
        /** out_eof_ch4_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_ch4_int_ena:1;
        /** out_dscr_err_ch4_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_ch4_int_ena:1;
        /** out_total_eof_ch4_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_ch4_int_ena:1;
        /** outfifo_ovf_l1_ch4_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l1_ch4_int_ena:1;
        /** outfifo_udf_l1_ch4_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l1_ch4_int_ena:1;
        /** outfifo_ovf_l2_ch4_int_ena : R/W; bitpos: [6]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l2_ch4_int_ena:1;
        /** outfifo_udf_l2_ch4_int_ena : R/W; bitpos: [7]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l2_ch4_int_ena:1;
        /** out_dscr_task_ovf_ch4_int_ena : R/W; bitpos: [8]; default: 0;
         *  The interrupt enable bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t out_dscr_task_ovf_ch4_int_ena:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_out_int_ena_ch4_reg_t;

/** Type of out_int_st_ch4 register
 *  TX CH4  interrupt st register
 */
typedef union {
    struct {
        /** out_done_ch4_int_st : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_ch4_int_st:1;
        /** out_eof_ch4_int_st : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_ch4_int_st:1;
        /** out_dscr_err_ch4_int_st : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_ch4_int_st:1;
        /** out_total_eof_ch4_int_st : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_ch4_int_st:1;
        /** outfifo_ovf_l1_ch4_int_st : RO; bitpos: [4]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l1_ch4_int_st:1;
        /** outfifo_udf_l1_ch4_int_st : RO; bitpos: [5]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l1_ch4_int_st:1;
        /** outfifo_ovf_l2_ch4_int_st : RO; bitpos: [6]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l2_ch4_int_st:1;
        /** outfifo_udf_l2_ch4_int_st : RO; bitpos: [7]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l2_ch4_int_st:1;
        /** out_dscr_task_ovf_ch4_int_st : RO; bitpos: [8]; default: 0;
         *  The raw interrupt status bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t out_dscr_task_ovf_ch4_int_st:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_out_int_st_ch4_reg_t;

/** Type of out_int_clr_ch4 register
 *  TX CH4  interrupt clr register
 */
typedef union {
    struct {
        /** out_done_ch4_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_ch4_int_clr:1;
        /** out_eof_ch4_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_ch4_int_clr:1;
        /** out_dscr_err_ch4_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_ch4_int_clr:1;
        /** out_total_eof_ch4_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_ch4_int_clr:1;
        /** outfifo_ovf_l1_ch4_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l1_ch4_int_clr:1;
        /** outfifo_udf_l1_ch4_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l1_ch4_int_clr:1;
        /** outfifo_ovf_l2_ch4_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l2_ch4_int_clr:1;
        /** outfifo_udf_l2_ch4_int_clr : WT; bitpos: [7]; default: 0;
         *  Set this bit to clear the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l2_ch4_int_clr:1;
        /** out_dscr_task_ovf_ch4_int_clr : WT; bitpos: [8]; default: 0;
         *  Set this bit to clear the OUT_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t out_dscr_task_ovf_ch4_int_clr:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_out_int_clr_ch4_reg_t;

/** Type of in_int_raw_ch0 register
 *  RX CH0  interrupt raw register
 */
typedef union {
    struct {
        /** in_done_ch0_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been transmitted to peripherals for Rx channel 0.
         */
        uint32_t in_done_ch0_int_raw:1;
        /** in_suc_eof_ch0_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been received and no data error is detected for Rx channel 0.
         */
        uint32_t in_suc_eof_ch0_int_raw:1;
        /** in_err_eof_ch0_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been received and data error is detected
         */
        uint32_t in_err_eof_ch0_int_raw:1;
        /** in_dscr_err_ch0_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt bit turns to high level when detecting inlink descriptor error,
         *  including owner error, the second and third word error of inlink descriptor for Rx
         *  channel 0.
         */
        uint32_t in_dscr_err_ch0_int_raw:1;
        /** infifo_ovf_l1_ch0_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt bit turns to high level when fifo of Rx channel is overflow.
         */
        uint32_t infifo_ovf_l1_ch0_int_raw:1;
        /** infifo_udf_l1_ch0_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt bit turns to high level when fifo of Rx channel is underflow.
         */
        uint32_t infifo_udf_l1_ch0_int_raw:1;
        /** infifo_ovf_l2_ch0_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt bit turns to high level when fifo of Rx channel is overflow.
         */
        uint32_t infifo_ovf_l2_ch0_int_raw:1;
        /** infifo_udf_l2_ch0_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw interrupt bit turns to high level when fifo of Rx channel is underflow.
         */
        uint32_t infifo_udf_l2_ch0_int_raw:1;
        /** in_dscr_empty_ch0_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw interrupt bit turns to high level when the last descriptor is done but fifo
         *  also remain data.
         */
        uint32_t in_dscr_empty_ch0_int_raw:1;
        /** in_dscr_task_ovf_ch0_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
         */
        uint32_t in_dscr_task_ovf_ch0_int_raw:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_int_raw_ch0_reg_t;

/** Type of in_int_ena_ch0 register
 *  RX CH0  interrupt ena register
 */
typedef union {
    struct {
        /** in_done_ch0_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_ch0_int_ena:1;
        /** in_suc_eof_ch0_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_ch0_int_ena:1;
        /** in_err_eof_ch0_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_ch0_int_ena:1;
        /** in_dscr_err_ch0_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_ch0_int_ena:1;
        /** infifo_ovf_l1_ch0_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_ch0_int_ena:1;
        /** infifo_udf_l1_ch0_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_ch0_int_ena:1;
        /** infifo_ovf_l2_ch0_int_ena : R/W; bitpos: [6]; default: 0;
         *  The interrupt enable bit for the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l2_ch0_int_ena:1;
        /** infifo_udf_l2_ch0_int_ena : R/W; bitpos: [7]; default: 0;
         *  The interrupt enable bit for the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_udf_l2_ch0_int_ena:1;
        /** in_dscr_empty_ch0_int_ena : R/W; bitpos: [8]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_ch0_int_ena:1;
        /** in_dscr_task_ovf_ch0_int_ena : R/W; bitpos: [9]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t in_dscr_task_ovf_ch0_int_ena:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_int_ena_ch0_reg_t;

/** Type of in_int_st_ch0 register
 *  RX CH0  interrupt st register
 */
typedef union {
    struct {
        /** in_done_ch0_int_st : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_ch0_int_st:1;
        /** in_suc_eof_ch0_int_st : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_ch0_int_st:1;
        /** in_err_eof_ch0_int_st : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_ch0_int_st:1;
        /** in_dscr_err_ch0_int_st : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_ch0_int_st:1;
        /** infifo_ovf_l1_ch0_int_st : RO; bitpos: [4]; default: 0;
         *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_ch0_int_st:1;
        /** infifo_udf_l1_ch0_int_st : RO; bitpos: [5]; default: 0;
         *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_ch0_int_st:1;
        /** infifo_ovf_l2_ch0_int_st : RO; bitpos: [6]; default: 0;
         *  The raw interrupt status bit for the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l2_ch0_int_st:1;
        /** infifo_udf_l2_ch0_int_st : RO; bitpos: [7]; default: 0;
         *  The raw interrupt status bit for the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_udf_l2_ch0_int_st:1;
        /** in_dscr_empty_ch0_int_st : RO; bitpos: [8]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_ch0_int_st:1;
        /** in_dscr_task_ovf_ch0_int_st : RO; bitpos: [9]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t in_dscr_task_ovf_ch0_int_st:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_int_st_ch0_reg_t;

/** Type of in_int_clr_ch0 register
 *  RX CH0  interrupt clr register
 */
typedef union {
    struct {
        /** in_done_ch0_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_ch0_int_clr:1;
        /** in_suc_eof_ch0_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_ch0_int_clr:1;
        /** in_err_eof_ch0_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_ch0_int_clr:1;
        /** in_dscr_err_ch0_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the INDSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_ch0_int_clr:1;
        /** infifo_ovf_l1_ch0_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_ch0_int_clr:1;
        /** infifo_udf_l1_ch0_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_ch0_int_clr:1;
        /** infifo_ovf_l2_ch0_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l2_ch0_int_clr:1;
        /** infifo_udf_l2_ch0_int_clr : WT; bitpos: [7]; default: 0;
         *  Set this bit to clear the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_udf_l2_ch0_int_clr:1;
        /** in_dscr_empty_ch0_int_clr : WT; bitpos: [8]; default: 0;
         *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_ch0_int_clr:1;
        /** in_dscr_task_ovf_ch0_int_clr : WT; bitpos: [9]; default: 0;
         *  Set this bit to clear the IN_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t in_dscr_task_ovf_ch0_int_clr:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_int_clr_ch0_reg_t;

/** Type of in_int_raw_ch1 register
 *  RX CH1  interrupt raw register
 */
typedef union {
    struct {
        /** in_done_ch1_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been transmitted to peripherals for Rx channel 1.
         */
        uint32_t in_done_ch1_int_raw:1;
        /** in_suc_eof_ch1_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been received and no data error is detected for Rx channel 1.
         */
        uint32_t in_suc_eof_ch1_int_raw:1;
        /** in_err_eof_ch1_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been received and data error is detected
         */
        uint32_t in_err_eof_ch1_int_raw:1;
        /** in_dscr_err_ch1_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt bit turns to high level when detecting inlink descriptor error,
         *  including owner error, the second and third word error of inlink descriptor for Rx
         *  channel 1.
         */
        uint32_t in_dscr_err_ch1_int_raw:1;
        /** infifo_ovf_l1_ch1_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  This raw interrupt bit turns to high level when fifo of Rx channel is overflow.
         */
        uint32_t infifo_ovf_l1_ch1_int_raw:1;
        /** infifo_udf_l1_ch1_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  This raw interrupt bit turns to high level when fifo of Rx channel is underflow.
         */
        uint32_t infifo_udf_l1_ch1_int_raw:1;
        /** infifo_ovf_l2_ch1_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  This raw interrupt bit turns to high level when fifo of Rx channel is overflow.
         */
        uint32_t infifo_ovf_l2_ch1_int_raw:1;
        /** infifo_udf_l2_ch1_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  This raw interrupt bit turns to high level when fifo of Rx channel is underflow.
         */
        uint32_t infifo_udf_l2_ch1_int_raw:1;
        /** in_dscr_empty_ch1_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw interrupt bit turns to high level when the last descriptor is done but fifo
         *  also remain data.
         */
        uint32_t in_dscr_empty_ch1_int_raw:1;
        /** in_dscr_task_ovf_ch1_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
         */
        uint32_t in_dscr_task_ovf_ch1_int_raw:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_int_raw_ch1_reg_t;

/** Type of in_int_ena_ch1 register
 *  RX CH1  interrupt ena register
 */
typedef union {
    struct {
        /** in_done_ch1_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_ch1_int_ena:1;
        /** in_suc_eof_ch1_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_ch1_int_ena:1;
        /** in_err_eof_ch1_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_ch1_int_ena:1;
        /** in_dscr_err_ch1_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_ch1_int_ena:1;
        /** infifo_ovf_l1_ch1_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_ch1_int_ena:1;
        /** infifo_udf_l1_ch1_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_ch1_int_ena:1;
        /** infifo_ovf_l2_ch1_int_ena : R/W; bitpos: [6]; default: 0;
         *  The interrupt enable bit for the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l2_ch1_int_ena:1;
        /** infifo_udf_l2_ch1_int_ena : R/W; bitpos: [7]; default: 0;
         *  The interrupt enable bit for the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_udf_l2_ch1_int_ena:1;
        /** in_dscr_empty_ch1_int_ena : R/W; bitpos: [8]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_ch1_int_ena:1;
        /** in_dscr_task_ovf_ch1_int_ena : R/W; bitpos: [9]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t in_dscr_task_ovf_ch1_int_ena:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_int_ena_ch1_reg_t;

/** Type of in_int_st_ch1 register
 *  RX CH1  interrupt st register
 */
typedef union {
    struct {
        /** in_done_ch1_int_st : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_ch1_int_st:1;
        /** in_suc_eof_ch1_int_st : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_ch1_int_st:1;
        /** in_err_eof_ch1_int_st : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_ch1_int_st:1;
        /** in_dscr_err_ch1_int_st : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_ch1_int_st:1;
        /** infifo_ovf_l1_ch1_int_st : RO; bitpos: [4]; default: 0;
         *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_ch1_int_st:1;
        /** infifo_udf_l1_ch1_int_st : RO; bitpos: [5]; default: 0;
         *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_ch1_int_st:1;
        /** infifo_ovf_l2_ch1_int_st : RO; bitpos: [6]; default: 0;
         *  The raw interrupt status bit for the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l2_ch1_int_st:1;
        /** infifo_udf_l2_ch1_int_st : RO; bitpos: [7]; default: 0;
         *  The raw interrupt status bit for the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_udf_l2_ch1_int_st:1;
        /** in_dscr_empty_ch1_int_st : RO; bitpos: [8]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_ch1_int_st:1;
        /** in_dscr_task_ovf_ch1_int_st : RO; bitpos: [9]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t in_dscr_task_ovf_ch1_int_st:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_int_st_ch1_reg_t;

/** Type of in_int_clr_ch1 register
 *  RX CH1  interrupt clr register
 */
typedef union {
    struct {
        /** in_done_ch1_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_ch1_int_clr:1;
        /** in_suc_eof_ch1_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_ch1_int_clr:1;
        /** in_err_eof_ch1_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_ch1_int_clr:1;
        /** in_dscr_err_ch1_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the INDSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_ch1_int_clr:1;
        /** infifo_ovf_l1_ch1_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_ch1_int_clr:1;
        /** infifo_udf_l1_ch1_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_ch1_int_clr:1;
        /** infifo_ovf_l2_ch1_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l2_ch1_int_clr:1;
        /** infifo_udf_l2_ch1_int_clr : WT; bitpos: [7]; default: 0;
         *  Set this bit to clear the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_udf_l2_ch1_int_clr:1;
        /** in_dscr_empty_ch1_int_clr : WT; bitpos: [8]; default: 0;
         *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_ch1_int_clr:1;
        /** in_dscr_task_ovf_ch1_int_clr : WT; bitpos: [9]; default: 0;
         *  Set this bit to clear the IN_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t in_dscr_task_ovf_ch1_int_clr:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_int_clr_ch1_reg_t;

/** Type of in_int_raw_ch2 register
 *  RX CH2  interrupt raw register
 */
typedef union {
    struct {
        /** in_done_ch2_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been transmitted to peripherals for Rx channel 1.
         */
        uint32_t in_done_ch2_int_raw:1;
        /** in_suc_eof_ch2_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been received and no data error is detected for Rx channel 1.
         */
        uint32_t in_suc_eof_ch2_int_raw:1;
        /** in_err_eof_ch2_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been received and data error is detected
         */
        uint32_t in_err_eof_ch2_int_raw:1;
        /** in_dscr_err_ch2_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt bit turns to high level when detecting inlink descriptor error,
         *  including owner error, the second and third word error of inlink descriptor for Rx
         *  channel 1.
         */
        uint32_t in_dscr_err_ch2_int_raw:1;
        /** infifo_ovf_l1_ch2_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  This raw interrupt bit turns to high level when fifo of Rx channel is overflow.
         */
        uint32_t infifo_ovf_l1_ch2_int_raw:1;
        /** infifo_udf_l1_ch2_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  This raw interrupt bit turns to high level when fifo of Rx channel is underflow.
         */
        uint32_t infifo_udf_l1_ch2_int_raw:1;
        /** infifo_ovf_l2_ch2_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  This raw interrupt bit turns to high level when fifo of Rx channel is overflow.
         */
        uint32_t infifo_ovf_l2_ch2_int_raw:1;
        /** infifo_udf_l2_ch2_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  This raw interrupt bit turns to high level when fifo of Rx channel is underflow.
         */
        uint32_t infifo_udf_l2_ch2_int_raw:1;
        /** in_dscr_empty_ch2_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw interrupt bit turns to high level when the last descriptor is done but fifo
         *  also remain data.
         */
        uint32_t in_dscr_empty_ch2_int_raw:1;
        /** in_dscr_task_ovf_ch2_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
         */
        uint32_t in_dscr_task_ovf_ch2_int_raw:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_int_raw_ch2_reg_t;

/** Type of in_int_ena_ch2 register
 *  RX CH2  interrupt ena register
 */
typedef union {
    struct {
        /** in_done_ch2_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_ch2_int_ena:1;
        /** in_suc_eof_ch2_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_ch2_int_ena:1;
        /** in_err_eof_ch2_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_ch2_int_ena:1;
        /** in_dscr_err_ch2_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_ch2_int_ena:1;
        /** infifo_ovf_l1_ch2_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_ch2_int_ena:1;
        /** infifo_udf_l1_ch2_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_ch2_int_ena:1;
        /** infifo_ovf_l2_ch2_int_ena : R/W; bitpos: [6]; default: 0;
         *  The interrupt enable bit for the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l2_ch2_int_ena:1;
        /** infifo_udf_l2_ch2_int_ena : R/W; bitpos: [7]; default: 0;
         *  The interrupt enable bit for the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_udf_l2_ch2_int_ena:1;
        /** in_dscr_empty_ch2_int_ena : R/W; bitpos: [8]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_ch2_int_ena:1;
        /** in_dscr_task_ovf_ch2_int_ena : R/W; bitpos: [9]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t in_dscr_task_ovf_ch2_int_ena:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_int_ena_ch2_reg_t;

/** Type of in_int_st_ch2 register
 *  RX CH2  interrupt st register
 */
typedef union {
    struct {
        /** in_done_ch2_int_st : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_ch2_int_st:1;
        /** in_suc_eof_ch2_int_st : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_ch2_int_st:1;
        /** in_err_eof_ch2_int_st : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_ch2_int_st:1;
        /** in_dscr_err_ch2_int_st : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_ch2_int_st:1;
        /** infifo_ovf_l1_ch2_int_st : RO; bitpos: [4]; default: 0;
         *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_ch2_int_st:1;
        /** infifo_udf_l1_ch2_int_st : RO; bitpos: [5]; default: 0;
         *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_ch2_int_st:1;
        /** infifo_ovf_l2_ch2_int_st : RO; bitpos: [6]; default: 0;
         *  The raw interrupt status bit for the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l2_ch2_int_st:1;
        /** infifo_udf_l2_ch2_int_st : RO; bitpos: [7]; default: 0;
         *  The raw interrupt status bit for the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_udf_l2_ch2_int_st:1;
        /** in_dscr_empty_ch2_int_st : RO; bitpos: [8]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_ch2_int_st:1;
        /** in_dscr_task_ovf_ch2_int_st : RO; bitpos: [9]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t in_dscr_task_ovf_ch2_int_st:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_int_st_ch2_reg_t;

/** Type of in_int_clr_ch2 register
 *  RX CH2  interrupt clr register
 */
typedef union {
    struct {
        /** in_done_ch2_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_ch2_int_clr:1;
        /** in_suc_eof_ch2_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_ch2_int_clr:1;
        /** in_err_eof_ch2_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_ch2_int_clr:1;
        /** in_dscr_err_ch2_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the INDSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_ch2_int_clr:1;
        /** infifo_ovf_l1_ch2_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_ch2_int_clr:1;
        /** infifo_udf_l1_ch2_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_ch2_int_clr:1;
        /** infifo_ovf_l2_ch2_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l2_ch2_int_clr:1;
        /** infifo_udf_l2_ch2_int_clr : WT; bitpos: [7]; default: 0;
         *  Set this bit to clear the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_udf_l2_ch2_int_clr:1;
        /** in_dscr_empty_ch2_int_clr : WT; bitpos: [8]; default: 0;
         *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_ch2_int_clr:1;
        /** in_dscr_task_ovf_ch2_int_clr : WT; bitpos: [9]; default: 0;
         *  Set this bit to clear the IN_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t in_dscr_task_ovf_ch2_int_clr:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_int_clr_ch2_reg_t;

/** Type of in_int_raw_ch3 register
 *  RX CH3  interrupt raw register
 */
typedef union {
    struct {
        /** in_done_ch3_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been transmitted to peripherals for Rx channel 1.
         */
        uint32_t in_done_ch3_int_raw:1;
        /** in_suc_eof_ch3_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been received and no data error is detected for Rx channel 1.
         */
        uint32_t in_suc_eof_ch3_int_raw:1;
        /** in_err_eof_ch3_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been received and data error is detected
         */
        uint32_t in_err_eof_ch3_int_raw:1;
        /** in_dscr_err_ch3_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt bit turns to high level when detecting inlink descriptor error,
         *  including owner error, the second and third word error of inlink descriptor for Rx
         *  channel 1.
         */
        uint32_t in_dscr_err_ch3_int_raw:1;
        /** infifo_ovf_l1_ch3_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  This raw interrupt bit turns to high level when fifo of Rx channel is overflow.
         */
        uint32_t infifo_ovf_l1_ch3_int_raw:1;
        /** infifo_udf_l1_ch3_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  This raw interrupt bit turns to high level when fifo of Rx channel is underflow.
         */
        uint32_t infifo_udf_l1_ch3_int_raw:1;
        /** infifo_ovf_l2_ch3_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  This raw interrupt bit turns to high level when fifo of Rx channel is overflow.
         */
        uint32_t infifo_ovf_l2_ch3_int_raw:1;
        /** infifo_udf_l2_ch3_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  This raw interrupt bit turns to high level when fifo of Rx channel is underflow.
         */
        uint32_t infifo_udf_l2_ch3_int_raw:1;
        /** in_dscr_empty_ch3_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw interrupt bit turns to high level when the last descriptor is done but fifo
         *  also remain data.
         */
        uint32_t in_dscr_empty_ch3_int_raw:1;
        /** in_dscr_task_ovf_ch3_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
         */
        uint32_t in_dscr_task_ovf_ch3_int_raw:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_int_raw_ch3_reg_t;

/** Type of in_int_ena_ch3 register
 *  RX CH3  interrupt ena register
 */
typedef union {
    struct {
        /** in_done_ch3_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_ch3_int_ena:1;
        /** in_suc_eof_ch3_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_ch3_int_ena:1;
        /** in_err_eof_ch3_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_ch3_int_ena:1;
        /** in_dscr_err_ch3_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_ch3_int_ena:1;
        /** infifo_ovf_l1_ch3_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_ch3_int_ena:1;
        /** infifo_udf_l1_ch3_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_ch3_int_ena:1;
        /** infifo_ovf_l2_ch3_int_ena : R/W; bitpos: [6]; default: 0;
         *  The interrupt enable bit for the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l2_ch3_int_ena:1;
        /** infifo_udf_l2_ch3_int_ena : R/W; bitpos: [7]; default: 0;
         *  The interrupt enable bit for the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_udf_l2_ch3_int_ena:1;
        /** in_dscr_empty_ch3_int_ena : R/W; bitpos: [8]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_ch3_int_ena:1;
        /** in_dscr_task_ovf_ch3_int_ena : R/W; bitpos: [9]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t in_dscr_task_ovf_ch3_int_ena:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_int_ena_ch3_reg_t;

/** Type of in_int_st_ch3 register
 *  RX CH3  interrupt st register
 */
typedef union {
    struct {
        /** in_done_ch3_int_st : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_ch3_int_st:1;
        /** in_suc_eof_ch3_int_st : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_ch3_int_st:1;
        /** in_err_eof_ch3_int_st : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_ch3_int_st:1;
        /** in_dscr_err_ch3_int_st : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_ch3_int_st:1;
        /** infifo_ovf_l1_ch3_int_st : RO; bitpos: [4]; default: 0;
         *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_ch3_int_st:1;
        /** infifo_udf_l1_ch3_int_st : RO; bitpos: [5]; default: 0;
         *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_ch3_int_st:1;
        /** infifo_ovf_l2_ch3_int_st : RO; bitpos: [6]; default: 0;
         *  The raw interrupt status bit for the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l2_ch3_int_st:1;
        /** infifo_udf_l2_ch3_int_st : RO; bitpos: [7]; default: 0;
         *  The raw interrupt status bit for the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_udf_l2_ch3_int_st:1;
        /** in_dscr_empty_ch3_int_st : RO; bitpos: [8]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_ch3_int_st:1;
        /** in_dscr_task_ovf_ch3_int_st : RO; bitpos: [9]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t in_dscr_task_ovf_ch3_int_st:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_int_st_ch3_reg_t;

/** Type of in_int_clr_ch3 register
 *  RX CH3  interrupt clr register
 */
typedef union {
    struct {
        /** in_done_ch3_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_ch3_int_clr:1;
        /** in_suc_eof_ch3_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_ch3_int_clr:1;
        /** in_err_eof_ch3_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_ch3_int_clr:1;
        /** in_dscr_err_ch3_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the INDSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_ch3_int_clr:1;
        /** infifo_ovf_l1_ch3_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_ch3_int_clr:1;
        /** infifo_udf_l1_ch3_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_ch3_int_clr:1;
        /** infifo_ovf_l2_ch3_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l2_ch3_int_clr:1;
        /** infifo_udf_l2_ch3_int_clr : WT; bitpos: [7]; default: 0;
         *  Set this bit to clear the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_udf_l2_ch3_int_clr:1;
        /** in_dscr_empty_ch3_int_clr : WT; bitpos: [8]; default: 0;
         *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_ch3_int_clr:1;
        /** in_dscr_task_ovf_ch3_int_clr : WT; bitpos: [9]; default: 0;
         *  Set this bit to clear the IN_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t in_dscr_task_ovf_ch3_int_clr:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_int_clr_ch3_reg_t;

/** Type of in_int_raw_ch4 register
 *  RX CH4  interrupt raw register
 */
typedef union {
    struct {
        /** in_done_ch4_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been transmitted to peripherals for Rx channel 1.
         */
        uint32_t in_done_ch4_int_raw:1;
        /** in_suc_eof_ch4_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been received and no data error is detected for Rx channel 1.
         */
        uint32_t in_suc_eof_ch4_int_raw:1;
        /** in_err_eof_ch4_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been received and data error is detected
         */
        uint32_t in_err_eof_ch4_int_raw:1;
        /** in_dscr_err_ch4_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt bit turns to high level when detecting inlink descriptor error,
         *  including owner error, the second and third word error of inlink descriptor for Rx
         *  channel 1.
         */
        uint32_t in_dscr_err_ch4_int_raw:1;
        /** infifo_ovf_l1_ch4_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  This raw interrupt bit turns to high level when fifo of Rx channel is overflow.
         */
        uint32_t infifo_ovf_l1_ch4_int_raw:1;
        /** infifo_udf_l1_ch4_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  This raw interrupt bit turns to high level when fifo of Rx channel is underflow.
         */
        uint32_t infifo_udf_l1_ch4_int_raw:1;
        /** infifo_ovf_l2_ch4_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  This raw interrupt bit turns to high level when fifo of Rx channel is overflow.
         */
        uint32_t infifo_ovf_l2_ch4_int_raw:1;
        /** infifo_udf_l2_ch4_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  This raw interrupt bit turns to high level when fifo of Rx channel is underflow.
         */
        uint32_t infifo_udf_l2_ch4_int_raw:1;
        /** in_dscr_empty_ch4_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw interrupt bit turns to high level when the last descriptor is done but fifo
         *  also remain data.
         */
        uint32_t in_dscr_empty_ch4_int_raw:1;
        /** in_dscr_task_ovf_ch4_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
         */
        uint32_t in_dscr_task_ovf_ch4_int_raw:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_int_raw_ch4_reg_t;

/** Type of in_int_ena_ch4 register
 *  RX CH4  interrupt ena register
 */
typedef union {
    struct {
        /** in_done_ch4_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_ch4_int_ena:1;
        /** in_suc_eof_ch4_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_ch4_int_ena:1;
        /** in_err_eof_ch4_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_ch4_int_ena:1;
        /** in_dscr_err_ch4_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_ch4_int_ena:1;
        /** infifo_ovf_l1_ch4_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_ch4_int_ena:1;
        /** infifo_udf_l1_ch4_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_ch4_int_ena:1;
        /** infifo_ovf_l2_ch4_int_ena : R/W; bitpos: [6]; default: 0;
         *  The interrupt enable bit for the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l2_ch4_int_ena:1;
        /** infifo_udf_l2_ch4_int_ena : R/W; bitpos: [7]; default: 0;
         *  The interrupt enable bit for the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_udf_l2_ch4_int_ena:1;
        /** in_dscr_empty_ch4_int_ena : R/W; bitpos: [8]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_ch4_int_ena:1;
        /** in_dscr_task_ovf_ch4_int_ena : R/W; bitpos: [9]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t in_dscr_task_ovf_ch4_int_ena:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_int_ena_ch4_reg_t;

/** Type of in_int_st_ch4 register
 *  RX CH4  interrupt st register
 */
typedef union {
    struct {
        /** in_done_ch4_int_st : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_ch4_int_st:1;
        /** in_suc_eof_ch4_int_st : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_ch4_int_st:1;
        /** in_err_eof_ch4_int_st : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_ch4_int_st:1;
        /** in_dscr_err_ch4_int_st : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_ch4_int_st:1;
        /** infifo_ovf_l1_ch4_int_st : RO; bitpos: [4]; default: 0;
         *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_ch4_int_st:1;
        /** infifo_udf_l1_ch4_int_st : RO; bitpos: [5]; default: 0;
         *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_ch4_int_st:1;
        /** infifo_ovf_l2_ch4_int_st : RO; bitpos: [6]; default: 0;
         *  The raw interrupt status bit for the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l2_ch4_int_st:1;
        /** infifo_udf_l2_ch4_int_st : RO; bitpos: [7]; default: 0;
         *  The raw interrupt status bit for the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_udf_l2_ch4_int_st:1;
        /** in_dscr_empty_ch4_int_st : RO; bitpos: [8]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_ch4_int_st:1;
        /** in_dscr_task_ovf_ch4_int_st : RO; bitpos: [9]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t in_dscr_task_ovf_ch4_int_st:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_int_st_ch4_reg_t;

/** Type of in_int_clr_ch4 register
 *  RX CH4  interrupt clr register
 */
typedef union {
    struct {
        /** in_done_ch4_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_ch4_int_clr:1;
        /** in_suc_eof_ch4_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_ch4_int_clr:1;
        /** in_err_eof_ch4_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_ch4_int_clr:1;
        /** in_dscr_err_ch4_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the INDSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_ch4_int_clr:1;
        /** infifo_ovf_l1_ch4_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_ch4_int_clr:1;
        /** infifo_udf_l1_ch4_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_ch4_int_clr:1;
        /** infifo_ovf_l2_ch4_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l2_ch4_int_clr:1;
        /** infifo_udf_l2_ch4_int_clr : WT; bitpos: [7]; default: 0;
         *  Set this bit to clear the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_udf_l2_ch4_int_clr:1;
        /** in_dscr_empty_ch4_int_clr : WT; bitpos: [8]; default: 0;
         *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_ch4_int_clr:1;
        /** in_dscr_task_ovf_ch4_int_clr : WT; bitpos: [9]; default: 0;
         *  Set this bit to clear the IN_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t in_dscr_task_ovf_ch4_int_clr:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_int_clr_ch4_reg_t;

/** Type of in_int_raw_ch5 register
 *  RX CH5  interrupt raw register
 */
typedef union {
    struct {
        /** in_done_ch5_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been transmitted to peripherals for Rx channel 1.
         */
        uint32_t in_done_ch5_int_raw:1;
        /** in_suc_eof_ch5_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been received and no data error is detected for Rx channel 1.
         */
        uint32_t in_suc_eof_ch5_int_raw:1;
        /** infifo_ovf_l1_ch5_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  This raw interrupt bit turns to high level when fifo of Rx channel is overflow.
         */
        uint32_t infifo_ovf_l1_ch5_int_raw:1;
        /** infifo_udf_l1_ch5_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  This raw interrupt bit turns to high level when fifo of Rx channel is underflow.
         */
        uint32_t infifo_udf_l1_ch5_int_raw:1;
        /** fetch_mb_col_cnt_ovf_ch5_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  This raw interrupt bit turns to high level when fifo of Rx channel is underflow.
         */
        uint32_t fetch_mb_col_cnt_ovf_ch5_int_raw:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} h264_dma_in_int_raw_ch5_reg_t;

/** Type of in_int_ena_ch5 register
 *  RX CH5  interrupt ena register
 */
typedef union {
    struct {
        /** in_done_ch5_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_ch5_int_ena:1;
        /** in_suc_eof_ch5_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_ch5_int_ena:1;
        /** infifo_ovf_l1_ch5_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_ch5_int_ena:1;
        /** infifo_udf_l1_ch5_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_ch5_int_ena:1;
        /** fetch_mb_col_cnt_ovf_ch5_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t fetch_mb_col_cnt_ovf_ch5_int_ena:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} h264_dma_in_int_ena_ch5_reg_t;

/** Type of in_int_st_ch5 register
 *  RX CH5  interrupt st register
 */
typedef union {
    struct {
        /** in_done_ch5_int_st : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_ch5_int_st:1;
        /** in_suc_eof_ch5_int_st : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_ch5_int_st:1;
        /** infifo_ovf_l1_ch5_int_st : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_ch5_int_st:1;
        /** infifo_udf_l1_ch5_int_st : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_ch5_int_st:1;
        /** fetch_mb_col_cnt_ovf_ch5_int_st : RO; bitpos: [4]; default: 0;
         *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t fetch_mb_col_cnt_ovf_ch5_int_st:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} h264_dma_in_int_st_ch5_reg_t;

/** Type of in_int_clr_ch5 register
 *  RX CH5  interrupt clr register
 */
typedef union {
    struct {
        /** in_done_ch5_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_ch5_int_clr:1;
        /** in_suc_eof_ch5_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_ch5_int_clr:1;
        /** infifo_ovf_l1_ch5_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_ch5_int_clr:1;
        /** infifo_udf_l1_ch5_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_ch5_int_clr:1;
        /** fetch_mb_col_cnt_ovf_ch5_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t fetch_mb_col_cnt_ovf_ch5_int_clr:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} h264_dma_in_int_clr_ch5_reg_t;


/** Group: Status Registers */
/** Type of outfifo_status_ch0 register
 *  TX CH0 outfifo status register
 */
typedef union {
    struct {
        /** outfifo_full_l2_ch0 : RO; bitpos: [0]; default: 0;
         *  Tx FIFO full signal for Tx channel 0.
         */
        uint32_t outfifo_full_l2_ch0:1;
        /** outfifo_empty_l2_ch0 : RO; bitpos: [1]; default: 1;
         *  Tx FIFO empty signal for Tx channel 0.
         */
        uint32_t outfifo_empty_l2_ch0:1;
        /** outfifo_cnt_l2_ch0 : RO; bitpos: [5:2]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
         */
        uint32_t outfifo_cnt_l2_ch0:4;
        /** outfifo_full_l1_ch0 : RO; bitpos: [6]; default: 0;
         *  Tx FIFO full signal for Tx channel 0.
         */
        uint32_t outfifo_full_l1_ch0:1;
        /** outfifo_empty_l1_ch0 : RO; bitpos: [7]; default: 1;
         *  Tx FIFO empty signal for Tx channel 0.
         */
        uint32_t outfifo_empty_l1_ch0:1;
        /** outfifo_cnt_l1_ch0 : RO; bitpos: [12:8]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
         */
        uint32_t outfifo_cnt_l1_ch0:5;
        uint32_t reserved_13:3;
        /** outfifo_full_l3_ch0 : RO; bitpos: [16]; default: 0;
         *  Tx FIFO full signal for Tx channel 0.
         */
        uint32_t outfifo_full_l3_ch0:1;
        /** outfifo_empty_l3_ch0 : RO; bitpos: [17]; default: 1;
         *  Tx FIFO empty signal for Tx channel 0.
         */
        uint32_t outfifo_empty_l3_ch0:1;
        /** outfifo_cnt_l3_ch0 : RO; bitpos: [19:18]; default: 0;
         *  The register stores the 8byte number of the data in Tx FIFO for Tx channel 0.
         */
        uint32_t outfifo_cnt_l3_ch0:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} h264_dma_outfifo_status_ch0_reg_t;

/** Type of out_state_ch0 register
 *  TX CH0 state register
 */
typedef union {
    struct {
        /** outlink_dscr_addr_ch0 : RO; bitpos: [17:0]; default: 0;
         *  This register stores the current outlink descriptor's address.
         */
        uint32_t outlink_dscr_addr_ch0:18;
        /** out_dscr_state_ch0 : RO; bitpos: [19:18]; default: 0;
         *  This register stores the current descriptor state machine state.
         */
        uint32_t out_dscr_state_ch0:2;
        /** out_state_ch0 : RO; bitpos: [23:20]; default: 0;
         *  This register stores the current control module state machine state.
         */
        uint32_t out_state_ch0:4;
        /** out_reset_avail_ch0 : RO; bitpos: [24]; default: 1;
         *  This register indicate that if the channel reset is safety.
         */
        uint32_t out_reset_avail_ch0:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} h264_dma_out_state_ch0_reg_t;

/** Type of out_eof_des_addr_ch0 register
 *  TX CH0 eof des addr register
 */
typedef union {
    struct {
        /** out_eof_des_addr_ch0 : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the outlink descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t out_eof_des_addr_ch0:32;
    };
    uint32_t val;
} h264_dma_out_eof_des_addr_ch0_reg_t;

/** Type of out_dscr_ch0 register
 *  TX CH0 next dscr addr register
 */
typedef union {
    struct {
        /** outlink_dscr_ch0 : RO; bitpos: [31:0]; default: 0;
         *  The address of the next outlink descriptor address y.
         */
        uint32_t outlink_dscr_ch0:32;
    };
    uint32_t val;
} h264_dma_out_dscr_ch0_reg_t;

/** Type of out_dscr_bf0_ch0 register
 *  TX CH0 last dscr addr register
 */
typedef union {
    struct {
        /** outlink_dscr_bf0_ch0 : RO; bitpos: [31:0]; default: 0;
         *  The address of the last outlink descriptor's next address y-1.
         */
        uint32_t outlink_dscr_bf0_ch0:32;
    };
    uint32_t val;
} h264_dma_out_dscr_bf0_ch0_reg_t;

/** Type of out_dscr_bf1_ch0 register
 *  TX CH0 second-to-last dscr addr register
 */
typedef union {
    struct {
        /** outlink_dscr_bf1_ch0 : RO; bitpos: [31:0]; default: 0;
         *  The address of the second-to-last outlink descriptor's next address y-2.
         */
        uint32_t outlink_dscr_bf1_ch0:32;
    };
    uint32_t val;
} h264_dma_out_dscr_bf1_ch0_reg_t;

/** Type of out_ro_status_ch0 register
 *  TX CH0 reorder status register
 */
typedef union {
    struct {
        /** outfifo_ro_cnt_ch0 : RO; bitpos: [1:0]; default: 0;
         *  The register stores the 8byte number of the data in reorder Tx FIFO for channel 0.
         */
        uint32_t outfifo_ro_cnt_ch0:2;
        uint32_t reserved_2:4;
        /** out_ro_wr_state_ch0 : RO; bitpos: [7:6]; default: 0;
         *  The register stores the state of read ram of reorder
         */
        uint32_t out_ro_wr_state_ch0:2;
        /** out_ro_rd_state_ch0 : RO; bitpos: [9:8]; default: 0;
         *  The register stores the state of write ram of reorder
         */
        uint32_t out_ro_rd_state_ch0:2;
        /** out_pixel_byte_ch0 : RO; bitpos: [13:10]; default: 2;
         *  the number of bytes contained in a pixel at TX channel     0: 1byte     1: 1.5bytes
         *  2 : 2bytes     3: 2.5bytes     4: 3bytes     5: 4bytes
         */
        uint32_t out_pixel_byte_ch0:4;
        /** out_burst_block_num_ch0 : RO; bitpos: [17:14]; default: 0;
         *  the number of macro blocks contained in a burst of data at TX channel
         */
        uint32_t out_burst_block_num_ch0:4;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} h264_dma_out_ro_status_ch0_reg_t;

/** Type of outfifo_status_ch1 register
 *  TX CH1 outfifo status register
 */
typedef union {
    struct {
        /** outfifo_full_l2_ch1 : RO; bitpos: [0]; default: 0;
         *  Tx FIFO full signal for Tx channel 1.
         */
        uint32_t outfifo_full_l2_ch1:1;
        /** outfifo_empty_l2_ch1 : RO; bitpos: [1]; default: 1;
         *  Tx FIFO empty signal for Tx channel 1.
         */
        uint32_t outfifo_empty_l2_ch1:1;
        /** outfifo_cnt_l2_ch1 : RO; bitpos: [5:2]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
         */
        uint32_t outfifo_cnt_l2_ch1:4;
        /** outfifo_full_l1_ch1 : RO; bitpos: [6]; default: 0;
         *  Tx FIFO full signal for Tx channel 1.
         */
        uint32_t outfifo_full_l1_ch1:1;
        /** outfifo_empty_l1_ch1 : RO; bitpos: [7]; default: 1;
         *  Tx FIFO empty signal for Tx channel 1.
         */
        uint32_t outfifo_empty_l1_ch1:1;
        /** outfifo_cnt_l1_ch1 : RO; bitpos: [12:8]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
         */
        uint32_t outfifo_cnt_l1_ch1:5;
        uint32_t reserved_13:3;
        /** outfifo_full_l3_ch1 : RO; bitpos: [16]; default: 0;
         *  Tx FIFO full signal for Tx channel 1.
         */
        uint32_t outfifo_full_l3_ch1:1;
        /** outfifo_empty_l3_ch1 : RO; bitpos: [17]; default: 1;
         *  Tx FIFO empty signal for Tx channel 1.
         */
        uint32_t outfifo_empty_l3_ch1:1;
        /** outfifo_cnt_l3_ch1 : RO; bitpos: [19:18]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
         */
        uint32_t outfifo_cnt_l3_ch1:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} h264_dma_outfifo_status_ch1_reg_t;

/** Type of out_state_ch1 register
 *  TX CH1 state register
 */
typedef union {
    struct {
        /** outlink_dscr_addr_ch1 : RO; bitpos: [17:0]; default: 0;
         *  This register stores the current outlink descriptor's address.
         */
        uint32_t outlink_dscr_addr_ch1:18;
        /** out_dscr_state_ch1 : RO; bitpos: [19:18]; default: 0;
         *  This register stores the current descriptor state machine state.
         */
        uint32_t out_dscr_state_ch1:2;
        /** out_state_ch1 : RO; bitpos: [23:20]; default: 0;
         *  This register stores the current control module state machine state.
         */
        uint32_t out_state_ch1:4;
        /** out_reset_avail_ch1 : RO; bitpos: [24]; default: 1;
         *  This register indicate that if the channel reset is safety.
         */
        uint32_t out_reset_avail_ch1:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} h264_dma_out_state_ch1_reg_t;

/** Type of out_eof_des_addr_ch1 register
 *  TX CH1 eof des addr register
 */
typedef union {
    struct {
        /** out_eof_des_addr_ch1 : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the outlink descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t out_eof_des_addr_ch1:32;
    };
    uint32_t val;
} h264_dma_out_eof_des_addr_ch1_reg_t;

/** Type of out_dscr_ch1 register
 *  TX CH1 next dscr addr register
 */
typedef union {
    struct {
        /** outlink_dscr_ch1 : RO; bitpos: [31:0]; default: 0;
         *  The address of the next outlink descriptor address y.
         */
        uint32_t outlink_dscr_ch1:32;
    };
    uint32_t val;
} h264_dma_out_dscr_ch1_reg_t;

/** Type of out_dscr_bf0_ch1 register
 *  TX CH1 last dscr addr register
 */
typedef union {
    struct {
        /** outlink_dscr_bf0_ch1 : RO; bitpos: [31:0]; default: 0;
         *  The address of the last outlink descriptor's next address y-1.
         */
        uint32_t outlink_dscr_bf0_ch1:32;
    };
    uint32_t val;
} h264_dma_out_dscr_bf0_ch1_reg_t;

/** Type of out_dscr_bf1_ch1 register
 *  TX CH1 second-to-last dscr addr register
 */
typedef union {
    struct {
        /** outlink_dscr_bf1_ch1 : RO; bitpos: [31:0]; default: 0;
         *  The address of the second-to-last outlink descriptor's next address y-2.
         */
        uint32_t outlink_dscr_bf1_ch1:32;
    };
    uint32_t val;
} h264_dma_out_dscr_bf1_ch1_reg_t;

/** Type of outfifo_status_ch2 register
 *  TX CH2 outfifo status register
 */
typedef union {
    struct {
        /** outfifo_full_l2_ch2 : RO; bitpos: [0]; default: 0;
         *  Tx FIFO full signal for Tx channel 2.
         */
        uint32_t outfifo_full_l2_ch2:1;
        /** outfifo_empty_l2_ch2 : RO; bitpos: [1]; default: 1;
         *  Tx FIFO empty signal for Tx channel 2.
         */
        uint32_t outfifo_empty_l2_ch2:1;
        /** outfifo_cnt_l2_ch2 : RO; bitpos: [5:2]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 2.
         */
        uint32_t outfifo_cnt_l2_ch2:4;
        /** outfifo_full_l1_ch2 : RO; bitpos: [6]; default: 0;
         *  Tx FIFO full signal for Tx channel 2.
         */
        uint32_t outfifo_full_l1_ch2:1;
        /** outfifo_empty_l1_ch2 : RO; bitpos: [7]; default: 1;
         *  Tx FIFO empty signal for Tx channel 2.
         */
        uint32_t outfifo_empty_l1_ch2:1;
        /** outfifo_cnt_l1_ch2 : RO; bitpos: [12:8]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 2.
         */
        uint32_t outfifo_cnt_l1_ch2:5;
        uint32_t reserved_13:3;
        /** outfifo_full_l3_ch2 : RO; bitpos: [16]; default: 0;
         *  Tx FIFO full signal for Tx channel 2.
         */
        uint32_t outfifo_full_l3_ch2:1;
        /** outfifo_empty_l3_ch2 : RO; bitpos: [17]; default: 1;
         *  Tx FIFO empty signal for Tx channel 2.
         */
        uint32_t outfifo_empty_l3_ch2:1;
        /** outfifo_cnt_l3_ch2 : RO; bitpos: [19:18]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 2.
         */
        uint32_t outfifo_cnt_l3_ch2:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} h264_dma_outfifo_status_ch2_reg_t;

/** Type of out_state_ch2 register
 *  TX CH2 state register
 */
typedef union {
    struct {
        /** outlink_dscr_addr_ch2 : RO; bitpos: [17:0]; default: 0;
         *  This register stores the current outlink descriptor's address.
         */
        uint32_t outlink_dscr_addr_ch2:18;
        /** out_dscr_state_ch2 : RO; bitpos: [19:18]; default: 0;
         *  This register stores the current descriptor state machine state.
         */
        uint32_t out_dscr_state_ch2:2;
        /** out_state_ch2 : RO; bitpos: [23:20]; default: 0;
         *  This register stores the current control module state machine state.
         */
        uint32_t out_state_ch2:4;
        /** out_reset_avail_ch2 : RO; bitpos: [24]; default: 1;
         *  This register indicate that if the channel reset is safety.
         */
        uint32_t out_reset_avail_ch2:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} h264_dma_out_state_ch2_reg_t;

/** Type of out_eof_des_addr_ch2 register
 *  TX CH2 eof des addr register
 */
typedef union {
    struct {
        /** out_eof_des_addr_ch2 : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the inlink descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t out_eof_des_addr_ch2:32;
    };
    uint32_t val;
} h264_dma_out_eof_des_addr_ch2_reg_t;

/** Type of out_dscr_ch2 register
 *  TX CH2 next dscr addr register
 */
typedef union {
    struct {
        /** outlink_dscr_ch2 : RO; bitpos: [31:0]; default: 0;
         *  The address of the next outlink descriptor address y.
         */
        uint32_t outlink_dscr_ch2:32;
    };
    uint32_t val;
} h264_dma_out_dscr_ch2_reg_t;

/** Type of out_dscr_bf0_ch2 register
 *  TX CH2 last dscr addr register
 */
typedef union {
    struct {
        /** outlink_dscr_bf0_ch2 : RO; bitpos: [31:0]; default: 0;
         *  The address of the last outlink descriptor's next address y-1.
         */
        uint32_t outlink_dscr_bf0_ch2:32;
    };
    uint32_t val;
} h264_dma_out_dscr_bf0_ch2_reg_t;

/** Type of out_dscr_bf1_ch2 register
 *  TX CH2 second-to-last dscr addr register
 */
typedef union {
    struct {
        /** outlink_dscr_bf1_ch2 : RO; bitpos: [31:0]; default: 0;
         *  The address of the second-to-last outlink descriptor's next address y-2.
         */
        uint32_t outlink_dscr_bf1_ch2:32;
    };
    uint32_t val;
} h264_dma_out_dscr_bf1_ch2_reg_t;

/** Type of outfifo_status_ch3 register
 *  TX CH3 outfifo status register
 */
typedef union {
    struct {
        /** outfifo_full_l2_ch3 : RO; bitpos: [0]; default: 0;
         *  Tx FIFO full signal for Tx channel 2.
         */
        uint32_t outfifo_full_l2_ch3:1;
        /** outfifo_empty_l2_ch3 : RO; bitpos: [1]; default: 1;
         *  Tx FIFO empty signal for Tx channel 2.
         */
        uint32_t outfifo_empty_l2_ch3:1;
        /** outfifo_cnt_l2_ch3 : RO; bitpos: [5:2]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 2.
         */
        uint32_t outfifo_cnt_l2_ch3:4;
        /** outfifo_full_l1_ch3 : RO; bitpos: [6]; default: 0;
         *  Tx FIFO full signal for Tx channel 2.
         */
        uint32_t outfifo_full_l1_ch3:1;
        /** outfifo_empty_l1_ch3 : RO; bitpos: [7]; default: 1;
         *  Tx FIFO empty signal for Tx channel 2.
         */
        uint32_t outfifo_empty_l1_ch3:1;
        /** outfifo_cnt_l1_ch3 : RO; bitpos: [12:8]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 2.
         */
        uint32_t outfifo_cnt_l1_ch3:5;
        uint32_t reserved_13:3;
        /** outfifo_full_l3_ch3 : RO; bitpos: [16]; default: 0;
         *  Tx FIFO full signal for Tx channel 2.
         */
        uint32_t outfifo_full_l3_ch3:1;
        /** outfifo_empty_l3_ch3 : RO; bitpos: [17]; default: 1;
         *  Tx FIFO empty signal for Tx channel 2.
         */
        uint32_t outfifo_empty_l3_ch3:1;
        /** outfifo_cnt_l3_ch3 : RO; bitpos: [19:18]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 2.
         */
        uint32_t outfifo_cnt_l3_ch3:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} h264_dma_outfifo_status_ch3_reg_t;

/** Type of out_state_ch3 register
 *  TX CH3 state register
 */
typedef union {
    struct {
        /** outlink_dscr_addr_ch3 : RO; bitpos: [17:0]; default: 0;
         *  This register stores the current outlink descriptor's address.
         */
        uint32_t outlink_dscr_addr_ch3:18;
        /** out_dscr_state_ch3 : RO; bitpos: [19:18]; default: 0;
         *  This register stores the current descriptor state machine state.
         */
        uint32_t out_dscr_state_ch3:2;
        /** out_state_ch3 : RO; bitpos: [23:20]; default: 0;
         *  This register stores the current control module state machine state.
         */
        uint32_t out_state_ch3:4;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} h264_dma_out_state_ch3_reg_t;

/** Type of out_eof_des_addr_ch3 register
 *  TX CH3 eof des addr register
 */
typedef union {
    struct {
        /** out_eof_des_addr_ch3 : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the inlink descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t out_eof_des_addr_ch3:32;
    };
    uint32_t val;
} h264_dma_out_eof_des_addr_ch3_reg_t;

/** Type of out_dscr_ch3 register
 *  TX CH3 next dscr addr register
 */
typedef union {
    struct {
        /** outlink_dscr_ch3 : RO; bitpos: [31:0]; default: 0;
         *  The address of the next outlink descriptor address y.
         */
        uint32_t outlink_dscr_ch3:32;
    };
    uint32_t val;
} h264_dma_out_dscr_ch3_reg_t;

/** Type of out_dscr_bf0_ch3 register
 *  TX CH3 last dscr addr register
 */
typedef union {
    struct {
        /** outlink_dscr_bf0_ch3 : RO; bitpos: [31:0]; default: 0;
         *  The address of the last outlink descriptor's next address y-1.
         */
        uint32_t outlink_dscr_bf0_ch3:32;
    };
    uint32_t val;
} h264_dma_out_dscr_bf0_ch3_reg_t;

/** Type of out_dscr_bf1_ch3 register
 *  TX CH3 second-to-last dscr addr register
 */
typedef union {
    struct {
        /** outlink_dscr_bf1_ch3 : RO; bitpos: [31:0]; default: 0;
         *  The address of the second-to-last outlink descriptor's next address y-2.
         */
        uint32_t outlink_dscr_bf1_ch3:32;
    };
    uint32_t val;
} h264_dma_out_dscr_bf1_ch3_reg_t;

/** Type of outfifo_status_ch4 register
 *  TX CH4 outfifo status register
 */
typedef union {
    struct {
        /** outfifo_full_l2_ch4 : RO; bitpos: [0]; default: 0;
         *  Tx FIFO full signal for Tx channel 2.
         */
        uint32_t outfifo_full_l2_ch4:1;
        /** outfifo_empty_l2_ch4 : RO; bitpos: [1]; default: 1;
         *  Tx FIFO empty signal for Tx channel 2.
         */
        uint32_t outfifo_empty_l2_ch4:1;
        /** outfifo_cnt_l2_ch4 : RO; bitpos: [5:2]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 2.
         */
        uint32_t outfifo_cnt_l2_ch4:4;
        /** outfifo_full_l1_ch4 : RO; bitpos: [6]; default: 0;
         *  Tx FIFO full signal for Tx channel 2.
         */
        uint32_t outfifo_full_l1_ch4:1;
        /** outfifo_empty_l1_ch4 : RO; bitpos: [7]; default: 1;
         *  Tx FIFO empty signal for Tx channel 2.
         */
        uint32_t outfifo_empty_l1_ch4:1;
        /** outfifo_cnt_l1_ch4 : RO; bitpos: [12:8]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 2.
         */
        uint32_t outfifo_cnt_l1_ch4:5;
        uint32_t reserved_13:3;
        /** outfifo_full_l3_ch4 : RO; bitpos: [16]; default: 0;
         *  Tx FIFO full signal for Tx channel 2.
         */
        uint32_t outfifo_full_l3_ch4:1;
        /** outfifo_empty_l3_ch4 : RO; bitpos: [17]; default: 1;
         *  Tx FIFO empty signal for Tx channel 2.
         */
        uint32_t outfifo_empty_l3_ch4:1;
        /** outfifo_cnt_l3_ch4 : RO; bitpos: [19:18]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 2.
         */
        uint32_t outfifo_cnt_l3_ch4:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} h264_dma_outfifo_status_ch4_reg_t;

/** Type of out_state_ch4 register
 *  TX CH4 state register
 */
typedef union {
    struct {
        /** outlink_dscr_addr_ch4 : RO; bitpos: [17:0]; default: 0;
         *  This register stores the current outlink descriptor's address.
         */
        uint32_t outlink_dscr_addr_ch4:18;
        /** out_dscr_state_ch4 : RO; bitpos: [19:18]; default: 0;
         *  This register stores the current descriptor state machine state.
         */
        uint32_t out_dscr_state_ch4:2;
        /** out_state_ch4 : RO; bitpos: [23:20]; default: 0;
         *  This register stores the current control module state machine state.
         */
        uint32_t out_state_ch4:4;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} h264_dma_out_state_ch4_reg_t;

/** Type of out_eof_des_addr_ch4 register
 *  TX CH4 eof des addr register
 */
typedef union {
    struct {
        /** out_eof_des_addr_ch4 : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the inlink descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t out_eof_des_addr_ch4:32;
    };
    uint32_t val;
} h264_dma_out_eof_des_addr_ch4_reg_t;

/** Type of out_dscr_ch4 register
 *  TX CH4 next dscr addr register
 */
typedef union {
    struct {
        /** outlink_dscr_ch4 : RO; bitpos: [31:0]; default: 0;
         *  The address of the next outlink descriptor address y.
         */
        uint32_t outlink_dscr_ch4:32;
    };
    uint32_t val;
} h264_dma_out_dscr_ch4_reg_t;

/** Type of out_dscr_bf0_ch4 register
 *  TX CH4 last dscr addr register
 */
typedef union {
    struct {
        /** outlink_dscr_bf0_ch4 : RO; bitpos: [31:0]; default: 0;
         *  The address of the last outlink descriptor's next address y-1.
         */
        uint32_t outlink_dscr_bf0_ch4:32;
    };
    uint32_t val;
} h264_dma_out_dscr_bf0_ch4_reg_t;

/** Type of out_dscr_bf1_ch4 register
 *  TX CH4 second-to-last dscr addr register
 */
typedef union {
    struct {
        /** outlink_dscr_bf1_ch4 : RO; bitpos: [31:0]; default: 0;
         *  The address of the second-to-last outlink descriptor's next address y-2.
         */
        uint32_t outlink_dscr_bf1_ch4:32;
    };
    uint32_t val;
} h264_dma_out_dscr_bf1_ch4_reg_t;

/** Type of infifo_status_ch0 register
 *  RX CH0 INFIFO status register
 */
typedef union {
    struct {
        /** infifo_full_l2_ch0 : RO; bitpos: [0]; default: 0;
         *  Rx FIFO full signal for Rx channel.
         */
        uint32_t infifo_full_l2_ch0:1;
        /** infifo_empty_l2_ch0 : RO; bitpos: [1]; default: 1;
         *  Rx FIFO empty signal for Rx channel.
         */
        uint32_t infifo_empty_l2_ch0:1;
        /** infifo_cnt_l2_ch0 : RO; bitpos: [5:2]; default: 0;
         *  The register stores the byte number of the data in Rx FIFO for Rx channel.
         */
        uint32_t infifo_cnt_l2_ch0:4;
        /** infifo_full_l1_ch0 : RO; bitpos: [6]; default: 0;
         *  Tx FIFO full signal for Tx channel 0.
         */
        uint32_t infifo_full_l1_ch0:1;
        /** infifo_empty_l1_ch0 : RO; bitpos: [7]; default: 1;
         *  Tx FIFO empty signal for Tx channel 0.
         */
        uint32_t infifo_empty_l1_ch0:1;
        /** infifo_cnt_l1_ch0 : RO; bitpos: [12:8]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
         */
        uint32_t infifo_cnt_l1_ch0:5;
        uint32_t reserved_13:3;
        /** infifo_full_l3_ch0 : RO; bitpos: [16]; default: 0;
         *  Tx FIFO full signal for Tx channel 0.
         */
        uint32_t infifo_full_l3_ch0:1;
        /** infifo_empty_l3_ch0 : RO; bitpos: [17]; default: 1;
         *  Tx FIFO empty signal for Tx channel 0.
         */
        uint32_t infifo_empty_l3_ch0:1;
        /** infifo_cnt_l3_ch0 : RO; bitpos: [19:18]; default: 0;
         *  The register stores the 8byte number of the data in Tx FIFO for Tx channel 0.
         */
        uint32_t infifo_cnt_l3_ch0:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} h264_dma_infifo_status_ch0_reg_t;

/** Type of in_state_ch0 register
 *  RX CH0 state register
 */
typedef union {
    struct {
        /** inlink_dscr_addr_ch0 : RO; bitpos: [17:0]; default: 0;
         *  This register stores the current inlink descriptor's address.
         */
        uint32_t inlink_dscr_addr_ch0:18;
        /** in_dscr_state_ch0 : RO; bitpos: [19:18]; default: 0;
         *  This register stores the current descriptor state machine state.
         */
        uint32_t in_dscr_state_ch0:2;
        /** in_state_ch0 : RO; bitpos: [22:20]; default: 0;
         *  This register stores the current control module state machine state.
         */
        uint32_t in_state_ch0:3;
        /** in_reset_avail_ch0 : RO; bitpos: [23]; default: 1;
         *  This register indicate that if the channel reset is safety.
         */
        uint32_t in_reset_avail_ch0:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} h264_dma_in_state_ch0_reg_t;

/** Type of in_suc_eof_des_addr_ch0 register
 *  RX CH0 eof des addr register
 */
typedef union {
    struct {
        /** in_suc_eof_des_addr_ch0 : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the inlink descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t in_suc_eof_des_addr_ch0:32;
    };
    uint32_t val;
} h264_dma_in_suc_eof_des_addr_ch0_reg_t;

/** Type of in_err_eof_des_addr_ch0 register
 *  RX CH0 err eof des addr register
 */
typedef union {
    struct {
        /** in_err_eof_des_addr_ch0 : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the inlink descriptor when there are some
         *  errors in current receiving data.
         */
        uint32_t in_err_eof_des_addr_ch0:32;
    };
    uint32_t val;
} h264_dma_in_err_eof_des_addr_ch0_reg_t;

/** Type of in_dscr_ch0 register
 *  RX CH0 next dscr addr register
 */
typedef union {
    struct {
        /** inlink_dscr_ch0 : RO; bitpos: [31:0]; default: 0;
         *  The address of the next inlink descriptor address x.
         */
        uint32_t inlink_dscr_ch0:32;
    };
    uint32_t val;
} h264_dma_in_dscr_ch0_reg_t;

/** Type of in_dscr_bf0_ch0 register
 *  RX CH0 last dscr addr register
 */
typedef union {
    struct {
        /** inlink_dscr_bf0_ch0 : RO; bitpos: [31:0]; default: 0;
         *  The address of the last inlink descriptor's next address x-1.
         */
        uint32_t inlink_dscr_bf0_ch0:32;
    };
    uint32_t val;
} h264_dma_in_dscr_bf0_ch0_reg_t;

/** Type of in_dscr_bf1_ch0 register
 *  RX CH0 second-to-last dscr addr register
 */
typedef union {
    struct {
        /** inlink_dscr_bf1_ch0 : RO; bitpos: [31:0]; default: 0;
         *  The address of the second-to-last inlink descriptor's next address x-2.
         */
        uint32_t inlink_dscr_bf1_ch0:32;
    };
    uint32_t val;
} h264_dma_in_dscr_bf1_ch0_reg_t;

/** Type of infifo_status_ch1 register
 *  RX CH1 INFIFO status register
 */
typedef union {
    struct {
        /** infifo_full_l2_ch1 : RO; bitpos: [0]; default: 0;
         *  Rx FIFO full signal for Rx channel.
         */
        uint32_t infifo_full_l2_ch1:1;
        /** infifo_empty_l2_ch1 : RO; bitpos: [1]; default: 1;
         *  Rx FIFO empty signal for Rx channel.
         */
        uint32_t infifo_empty_l2_ch1:1;
        /** infifo_cnt_l2_ch1 : RO; bitpos: [5:2]; default: 0;
         *  The register stores the byte number of the data in Rx FIFO for Rx channel.
         */
        uint32_t infifo_cnt_l2_ch1:4;
        /** infifo_full_l1_ch1 : RO; bitpos: [6]; default: 0;
         *  Tx FIFO full signal for Tx channel 1.
         */
        uint32_t infifo_full_l1_ch1:1;
        /** infifo_empty_l1_ch1 : RO; bitpos: [7]; default: 1;
         *  Tx FIFO empty signal for Tx channel 1.
         */
        uint32_t infifo_empty_l1_ch1:1;
        /** infifo_cnt_l1_ch1 : RO; bitpos: [12:8]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
         */
        uint32_t infifo_cnt_l1_ch1:5;
        uint32_t reserved_13:3;
        /** infifo_full_l3_ch1 : RO; bitpos: [16]; default: 0;
         *  Tx FIFO full signal for Tx channel 1.
         */
        uint32_t infifo_full_l3_ch1:1;
        /** infifo_empty_l3_ch1 : RO; bitpos: [17]; default: 1;
         *  Tx FIFO empty signal for Tx channel 1.
         */
        uint32_t infifo_empty_l3_ch1:1;
        /** infifo_cnt_l3_ch1 : RO; bitpos: [19:18]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
         */
        uint32_t infifo_cnt_l3_ch1:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} h264_dma_infifo_status_ch1_reg_t;

/** Type of in_state_ch1 register
 *  RX CH1 state register
 */
typedef union {
    struct {
        /** inlink_dscr_addr_ch1 : RO; bitpos: [17:0]; default: 0;
         *  This register stores the current inlink descriptor's address.
         */
        uint32_t inlink_dscr_addr_ch1:18;
        /** in_dscr_state_ch1 : RO; bitpos: [19:18]; default: 0;
         *  This register stores the current descriptor state machine state.
         */
        uint32_t in_dscr_state_ch1:2;
        /** in_state_ch1 : RO; bitpos: [22:20]; default: 0;
         *  This register stores the current control module state machine state.
         */
        uint32_t in_state_ch1:3;
        /** in_reset_avail_ch1 : RO; bitpos: [23]; default: 1;
         *  This register indicate that if the channel reset is safety.
         */
        uint32_t in_reset_avail_ch1:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} h264_dma_in_state_ch1_reg_t;

/** Type of in_suc_eof_des_addr_ch1 register
 *  RX CH1 eof des addr register
 */
typedef union {
    struct {
        /** in_suc_eof_des_addr_ch1 : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the inlink descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t in_suc_eof_des_addr_ch1:32;
    };
    uint32_t val;
} h264_dma_in_suc_eof_des_addr_ch1_reg_t;

/** Type of in_err_eof_des_addr_ch1 register
 *  RX CH1 err eof des addr register
 */
typedef union {
    struct {
        /** in_err_eof_des_addr_ch1 : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the inlink descriptor when there are some
         *  errors in current receiving data.
         */
        uint32_t in_err_eof_des_addr_ch1:32;
    };
    uint32_t val;
} h264_dma_in_err_eof_des_addr_ch1_reg_t;

/** Type of in_dscr_ch1 register
 *  RX CH1 next dscr addr register
 */
typedef union {
    struct {
        /** inlink_dscr_ch1 : RO; bitpos: [31:0]; default: 0;
         *  The address of the next inlink descriptor address x.
         */
        uint32_t inlink_dscr_ch1:32;
    };
    uint32_t val;
} h264_dma_in_dscr_ch1_reg_t;

/** Type of in_dscr_bf0_ch1 register
 *  RX CH1 last dscr addr register
 */
typedef union {
    struct {
        /** inlink_dscr_bf0_ch1 : RO; bitpos: [31:0]; default: 0;
         *  The address of the last inlink descriptor's next address x-1.
         */
        uint32_t inlink_dscr_bf0_ch1:32;
    };
    uint32_t val;
} h264_dma_in_dscr_bf0_ch1_reg_t;

/** Type of in_dscr_bf1_ch1 register
 *  RX CH1 second-to-last dscr addr register
 */
typedef union {
    struct {
        /** inlink_dscr_bf1_ch1 : RO; bitpos: [31:0]; default: 0;
         *  The address of the second-to-last inlink descriptor's next address x-2.
         */
        uint32_t inlink_dscr_bf1_ch1:32;
    };
    uint32_t val;
} h264_dma_in_dscr_bf1_ch1_reg_t;

/** Type of infifo_status_ch2 register
 *  RX CH2 INFIFO status register
 */
typedef union {
    struct {
        /** infifo_full_l2_ch2 : RO; bitpos: [0]; default: 0;
         *  Rx FIFO full signal for Rx channel.
         */
        uint32_t infifo_full_l2_ch2:1;
        /** infifo_empty_l2_ch2 : RO; bitpos: [1]; default: 1;
         *  Rx FIFO empty signal for Rx channel.
         */
        uint32_t infifo_empty_l2_ch2:1;
        /** infifo_cnt_l2_ch2 : RO; bitpos: [5:2]; default: 0;
         *  The register stores the byte number of the data in Rx FIFO for Rx channel.
         */
        uint32_t infifo_cnt_l2_ch2:4;
        /** infifo_full_l1_ch2 : RO; bitpos: [6]; default: 0;
         *  Tx FIFO full signal for Tx channel 1.
         */
        uint32_t infifo_full_l1_ch2:1;
        /** infifo_empty_l1_ch2 : RO; bitpos: [7]; default: 1;
         *  Tx FIFO empty signal for Tx channel 1.
         */
        uint32_t infifo_empty_l1_ch2:1;
        /** infifo_cnt_l1_ch2 : RO; bitpos: [12:8]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
         */
        uint32_t infifo_cnt_l1_ch2:5;
        uint32_t reserved_13:3;
        /** infifo_full_l3_ch2 : RO; bitpos: [16]; default: 0;
         *  Tx FIFO full signal for Tx channel 1.
         */
        uint32_t infifo_full_l3_ch2:1;
        /** infifo_empty_l3_ch2 : RO; bitpos: [17]; default: 1;
         *  Tx FIFO empty signal for Tx channel 1.
         */
        uint32_t infifo_empty_l3_ch2:1;
        /** infifo_cnt_l3_ch2 : RO; bitpos: [19:18]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
         */
        uint32_t infifo_cnt_l3_ch2:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} h264_dma_infifo_status_ch2_reg_t;

/** Type of in_state_ch2 register
 *  RX CH2 state register
 */
typedef union {
    struct {
        /** inlink_dscr_addr_ch2 : RO; bitpos: [17:0]; default: 0;
         *  This register stores the current inlink descriptor's address.
         */
        uint32_t inlink_dscr_addr_ch2:18;
        /** in_dscr_state_ch2 : RO; bitpos: [19:18]; default: 0;
         *  This register stores the current descriptor state machine state.
         */
        uint32_t in_dscr_state_ch2:2;
        /** in_state_ch2 : RO; bitpos: [22:20]; default: 0;
         *  This register stores the current control module state machine state.
         */
        uint32_t in_state_ch2:3;
        /** in_reset_avail_ch2 : RO; bitpos: [23]; default: 1;
         *  This register indicate that if the channel reset is safety.
         */
        uint32_t in_reset_avail_ch2:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} h264_dma_in_state_ch2_reg_t;

/** Type of in_suc_eof_des_addr_ch2 register
 *  RX CH2 eof des addr register
 */
typedef union {
    struct {
        /** in_suc_eof_des_addr_ch2 : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the inlink descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t in_suc_eof_des_addr_ch2:32;
    };
    uint32_t val;
} h264_dma_in_suc_eof_des_addr_ch2_reg_t;

/** Type of in_err_eof_des_addr_ch2 register
 *  RX CH2 err eof des addr register
 */
typedef union {
    struct {
        /** in_err_eof_des_addr_ch2 : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the inlink descriptor when there are some
         *  errors in current receiving data.
         */
        uint32_t in_err_eof_des_addr_ch2:32;
    };
    uint32_t val;
} h264_dma_in_err_eof_des_addr_ch2_reg_t;

/** Type of in_dscr_ch2 register
 *  RX CH2 next dscr addr register
 */
typedef union {
    struct {
        /** inlink_dscr_ch2 : RO; bitpos: [31:0]; default: 0;
         *  The address of the next inlink descriptor address x.
         */
        uint32_t inlink_dscr_ch2:32;
    };
    uint32_t val;
} h264_dma_in_dscr_ch2_reg_t;

/** Type of in_dscr_bf0_ch2 register
 *  RX CH2 last dscr addr register
 */
typedef union {
    struct {
        /** inlink_dscr_bf0_ch2 : RO; bitpos: [31:0]; default: 0;
         *  The address of the last inlink descriptor's next address x-1.
         */
        uint32_t inlink_dscr_bf0_ch2:32;
    };
    uint32_t val;
} h264_dma_in_dscr_bf0_ch2_reg_t;

/** Type of in_dscr_bf1_ch2 register
 *  RX CH2 second-to-last dscr addr register
 */
typedef union {
    struct {
        /** inlink_dscr_bf1_ch2 : RO; bitpos: [31:0]; default: 0;
         *  The address of the second-to-last inlink descriptor's next address x-2.
         */
        uint32_t inlink_dscr_bf1_ch2:32;
    };
    uint32_t val;
} h264_dma_in_dscr_bf1_ch2_reg_t;

/** Type of infifo_status_ch3 register
 *  RX CH3 INFIFO status register
 */
typedef union {
    struct {
        /** infifo_full_l2_ch3 : RO; bitpos: [0]; default: 0;
         *  Rx FIFO full signal for Rx channel.
         */
        uint32_t infifo_full_l2_ch3:1;
        /** infifo_empty_l2_ch3 : RO; bitpos: [1]; default: 1;
         *  Rx FIFO empty signal for Rx channel.
         */
        uint32_t infifo_empty_l2_ch3:1;
        /** infifo_cnt_l2_ch3 : RO; bitpos: [5:2]; default: 0;
         *  The register stores the byte number of the data in Rx FIFO for Rx channel.
         */
        uint32_t infifo_cnt_l2_ch3:4;
        /** infifo_full_l1_ch3 : RO; bitpos: [6]; default: 0;
         *  Tx FIFO full signal for Tx channel 1.
         */
        uint32_t infifo_full_l1_ch3:1;
        /** infifo_empty_l1_ch3 : RO; bitpos: [7]; default: 1;
         *  Tx FIFO empty signal for Tx channel 1.
         */
        uint32_t infifo_empty_l1_ch3:1;
        /** infifo_cnt_l1_ch3 : RO; bitpos: [12:8]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
         */
        uint32_t infifo_cnt_l1_ch3:5;
        uint32_t reserved_13:3;
        /** infifo_full_l3_ch3 : RO; bitpos: [16]; default: 0;
         *  Tx FIFO full signal for Tx channel 1.
         */
        uint32_t infifo_full_l3_ch3:1;
        /** infifo_empty_l3_ch3 : RO; bitpos: [17]; default: 1;
         *  Tx FIFO empty signal for Tx channel 1.
         */
        uint32_t infifo_empty_l3_ch3:1;
        /** infifo_cnt_l3_ch3 : RO; bitpos: [19:18]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
         */
        uint32_t infifo_cnt_l3_ch3:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} h264_dma_infifo_status_ch3_reg_t;

/** Type of in_state_ch3 register
 *  RX CH3 state register
 */
typedef union {
    struct {
        /** inlink_dscr_addr_ch3 : RO; bitpos: [17:0]; default: 0;
         *  This register stores the current inlink descriptor's address.
         */
        uint32_t inlink_dscr_addr_ch3:18;
        /** in_dscr_state_ch3 : RO; bitpos: [19:18]; default: 0;
         *  This register stores the current descriptor state machine state.
         */
        uint32_t in_dscr_state_ch3:2;
        /** in_state_ch3 : RO; bitpos: [22:20]; default: 0;
         *  This register stores the current control module state machine state.
         */
        uint32_t in_state_ch3:3;
        /** in_reset_avail_ch3 : RO; bitpos: [23]; default: 1;
         *  This register indicate that if the channel reset is safety.
         */
        uint32_t in_reset_avail_ch3:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} h264_dma_in_state_ch3_reg_t;

/** Type of in_suc_eof_des_addr_ch3 register
 *  RX CH3 eof des addr register
 */
typedef union {
    struct {
        /** in_suc_eof_des_addr_ch3 : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the inlink descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t in_suc_eof_des_addr_ch3:32;
    };
    uint32_t val;
} h264_dma_in_suc_eof_des_addr_ch3_reg_t;

/** Type of in_err_eof_des_addr_ch3 register
 *  RX CH3 err eof des addr register
 */
typedef union {
    struct {
        /** in_err_eof_des_addr_ch3 : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the inlink descriptor when there are some
         *  errors in current receiving data.
         */
        uint32_t in_err_eof_des_addr_ch3:32;
    };
    uint32_t val;
} h264_dma_in_err_eof_des_addr_ch3_reg_t;

/** Type of in_dscr_ch3 register
 *  RX CH3 next dscr addr register
 */
typedef union {
    struct {
        /** inlink_dscr_ch3 : RO; bitpos: [31:0]; default: 0;
         *  The address of the next inlink descriptor address x.
         */
        uint32_t inlink_dscr_ch3:32;
    };
    uint32_t val;
} h264_dma_in_dscr_ch3_reg_t;

/** Type of in_dscr_bf0_ch3 register
 *  RX CH3 last dscr addr register
 */
typedef union {
    struct {
        /** inlink_dscr_bf0_ch3 : RO; bitpos: [31:0]; default: 0;
         *  The address of the last inlink descriptor's next address x-1.
         */
        uint32_t inlink_dscr_bf0_ch3:32;
    };
    uint32_t val;
} h264_dma_in_dscr_bf0_ch3_reg_t;

/** Type of in_dscr_bf1_ch3 register
 *  RX CH3 second-to-last dscr addr register
 */
typedef union {
    struct {
        /** inlink_dscr_bf1_ch3 : RO; bitpos: [31:0]; default: 0;
         *  The address of the second-to-last inlink descriptor's next address x-2.
         */
        uint32_t inlink_dscr_bf1_ch3:32;
    };
    uint32_t val;
} h264_dma_in_dscr_bf1_ch3_reg_t;

/** Type of infifo_status_ch4 register
 *  RX CH4 INFIFO status register
 */
typedef union {
    struct {
        /** infifo_full_l2_ch4 : RO; bitpos: [0]; default: 0;
         *  Rx FIFO full signal for Rx channel.
         */
        uint32_t infifo_full_l2_ch4:1;
        /** infifo_empty_l2_ch4 : RO; bitpos: [1]; default: 1;
         *  Rx FIFO empty signal for Rx channel.
         */
        uint32_t infifo_empty_l2_ch4:1;
        /** infifo_cnt_l2_ch4 : RO; bitpos: [5:2]; default: 0;
         *  The register stores the byte number of the data in Rx FIFO for Rx channel.
         */
        uint32_t infifo_cnt_l2_ch4:4;
        /** infifo_full_l1_ch4 : RO; bitpos: [6]; default: 0;
         *  Tx FIFO full signal for Tx channel 1.
         */
        uint32_t infifo_full_l1_ch4:1;
        /** infifo_empty_l1_ch4 : RO; bitpos: [7]; default: 1;
         *  Tx FIFO empty signal for Tx channel 1.
         */
        uint32_t infifo_empty_l1_ch4:1;
        /** infifo_cnt_l1_ch4 : RO; bitpos: [12:8]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
         */
        uint32_t infifo_cnt_l1_ch4:5;
        uint32_t reserved_13:3;
        /** infifo_full_l3_ch4 : RO; bitpos: [16]; default: 0;
         *  Tx FIFO full signal for Tx channel 1.
         */
        uint32_t infifo_full_l3_ch4:1;
        /** infifo_empty_l3_ch4 : RO; bitpos: [17]; default: 1;
         *  Tx FIFO empty signal for Tx channel 1.
         */
        uint32_t infifo_empty_l3_ch4:1;
        /** infifo_cnt_l3_ch4 : RO; bitpos: [19:18]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
         */
        uint32_t infifo_cnt_l3_ch4:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} h264_dma_infifo_status_ch4_reg_t;

/** Type of in_state_ch4 register
 *  RX CH4 state register
 */
typedef union {
    struct {
        /** inlink_dscr_addr_ch4 : RO; bitpos: [17:0]; default: 0;
         *  This register stores the current inlink descriptor's address.
         */
        uint32_t inlink_dscr_addr_ch4:18;
        /** in_dscr_state_ch4 : RO; bitpos: [19:18]; default: 0;
         *  This register stores the current descriptor state machine state.
         */
        uint32_t in_dscr_state_ch4:2;
        /** in_state_ch4 : RO; bitpos: [22:20]; default: 0;
         *  This register stores the current control module state machine state.
         */
        uint32_t in_state_ch4:3;
        /** in_reset_avail_ch4 : RO; bitpos: [23]; default: 1;
         *  This register indicate that if the channel reset is safety.
         */
        uint32_t in_reset_avail_ch4:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} h264_dma_in_state_ch4_reg_t;

/** Type of in_suc_eof_des_addr_ch4 register
 *  RX CH4 eof des addr register
 */
typedef union {
    struct {
        /** in_suc_eof_des_addr_ch4 : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the inlink descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t in_suc_eof_des_addr_ch4:32;
    };
    uint32_t val;
} h264_dma_in_suc_eof_des_addr_ch4_reg_t;

/** Type of in_err_eof_des_addr_ch4 register
 *  RX CH4 err eof des addr register
 */
typedef union {
    struct {
        /** in_err_eof_des_addr_ch4 : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the inlink descriptor when there are some
         *  errors in current receiving data.
         */
        uint32_t in_err_eof_des_addr_ch4:32;
    };
    uint32_t val;
} h264_dma_in_err_eof_des_addr_ch4_reg_t;

/** Type of in_dscr_ch4 register
 *  RX CH4 next dscr addr register
 */
typedef union {
    struct {
        /** inlink_dscr_ch4 : RO; bitpos: [31:0]; default: 0;
         *  The address of the next inlink descriptor address x.
         */
        uint32_t inlink_dscr_ch4:32;
    };
    uint32_t val;
} h264_dma_in_dscr_ch4_reg_t;

/** Type of in_dscr_bf0_ch4 register
 *  RX CH4 last dscr addr register
 */
typedef union {
    struct {
        /** inlink_dscr_bf0_ch4 : RO; bitpos: [31:0]; default: 0;
         *  The address of the last inlink descriptor's next address x-1.
         */
        uint32_t inlink_dscr_bf0_ch4:32;
    };
    uint32_t val;
} h264_dma_in_dscr_bf0_ch4_reg_t;

/** Type of in_dscr_bf1_ch4 register
 *  RX CH4 second-to-last dscr addr register
 */
typedef union {
    struct {
        /** inlink_dscr_bf1_ch4 : RO; bitpos: [31:0]; default: 0;
         *  The address of the second-to-last inlink descriptor's next address x-2.
         */
        uint32_t inlink_dscr_bf1_ch4:32;
    };
    uint32_t val;
} h264_dma_in_dscr_bf1_ch4_reg_t;

/** Type of infifo_status_ch5 register
 *  RX CH5 INFIFO status register
 */
typedef union {
    struct {
        /** infifo_full_l1_ch5 : RO; bitpos: [0]; default: 0;
         *  Tx FIFO full signal for Tx channel 1.
         */
        uint32_t infifo_full_l1_ch5:1;
        /** infifo_empty_l1_ch5 : RO; bitpos: [1]; default: 1;
         *  Tx FIFO empty signal for Tx channel 1.
         */
        uint32_t infifo_empty_l1_ch5:1;
        /** infifo_cnt_l1_ch5 : RO; bitpos: [6:2]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
         */
        uint32_t infifo_cnt_l1_ch5:5;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} h264_dma_infifo_status_ch5_reg_t;

/** Type of in_state_ch5 register
 *  RX CH5 state register
 */
typedef union {
    struct {
        /** in_state_ch5 : RO; bitpos: [2:0]; default: 0;
         *  This register stores the current control module state machine state.
         */
        uint32_t in_state_ch5:3;
        /** in_reset_avail_ch5 : RO; bitpos: [3]; default: 1;
         *  This register indicate that if the channel reset is safety.
         */
        uint32_t in_reset_avail_ch5:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} h264_dma_in_state_ch5_reg_t;


/** Group: out_link addr register */
/** Type of out_link_addr_ch0 register
 *  TX CH0 out_link dscr addr register
 */
typedef union {
    struct {
        /** outlink_addr_ch0 : R/W; bitpos: [31:0]; default: 0;
         *  This register stores the first outlink descriptor's address.
         */
        uint32_t outlink_addr_ch0:32;
    };
    uint32_t val;
} h264_dma_out_link_addr_ch0_reg_t;


/** Group: tx ch0 arb register */
/** Type of out_arb_ch0 register
 *  TX CH0 arb register
 */
typedef union {
    struct {
        /** out_arb_token_num_ch0 : R/W; bitpos: [3:0]; default: 1;
         *  Set the max number of token count of arbiter
         */
        uint32_t out_arb_token_num_ch0:4;
        /** exter_out_arb_priority_ch0 : R/W; bitpos: [5:4]; default: 1;
         *  Set the priority of channel
         */
        uint32_t exter_out_arb_priority_ch0:2;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} h264_dma_out_arb_ch0_reg_t;


/** Group: TX CH0 test mode register */
/** Type of out_mode_enable_ch0 register
 *  tx CH0 mode enable register
 */
typedef union {
    struct {
        /** out_test_mode_enable_ch0 : R/W; bitpos: [0]; default: 0;
         *  tx CH0 test mode enable.0 : H264_DMA work in normal mode.1 : H264_DMA work in test
         *  mode
         */
        uint32_t out_test_mode_enable_ch0:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} h264_dma_out_mode_enable_ch0_reg_t;

/** Type of out_mode_yuv_ch0 register
 *  tx CH0 test mode yuv value register
 */
typedef union {
    struct {
        /** out_test_y_value_ch0 : R/W; bitpos: [7:0]; default: 0;
         *  tx CH0 test mode y value
         */
        uint32_t out_test_y_value_ch0:8;
        /** out_test_u_value_ch0 : R/W; bitpos: [15:8]; default: 0;
         *  tx CH0 test mode u value
         */
        uint32_t out_test_u_value_ch0:8;
        /** out_test_v_value_ch0 : R/W; bitpos: [23:16]; default: 0;
         *  tx CH0 test mode v value
         */
        uint32_t out_test_v_value_ch0:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} h264_dma_out_mode_yuv_ch0_reg_t;


/** Group: ETM config register */
/** Type of out_etm_conf_ch0 register
 *  TX CH0 ETM config register
 */
typedef union {
    struct {
        /** out_etm_en_ch0 : R/W; bitpos: [0]; default: 0;
         *  Set this bit to 1 to enable ETM task function
         */
        uint32_t out_etm_en_ch0:1;
        /** out_etm_loop_en_ch0 : R/W; bitpos: [1]; default: 0;
         *  when this bit is 1, dscr can be processed after receiving a task
         */
        uint32_t out_etm_loop_en_ch0:1;
        /** out_dscr_task_mak_ch0 : R/W; bitpos: [3:2]; default: 1;
         *  ETM dscr_ready maximum cache numbers
         */
        uint32_t out_dscr_task_mak_ch0:2;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} h264_dma_out_etm_conf_ch0_reg_t;


/** Group: TX CH0 debug info */
/** Type of out_buf_len_ch0 register
 *  tx CH0 buf len register
 */
typedef union {
    struct {
        /** out_cmdfifo_buf_len_hb_ch0 : RO; bitpos: [12:0]; default: 0;
         *  only for debug
         */
        uint32_t out_cmdfifo_buf_len_hb_ch0:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} h264_dma_out_buf_len_ch0_reg_t;

/** Type of out_fifo_bcnt_ch0 register
 *  tx CH0 fifo byte cnt register
 */
typedef union {
    struct {
        /** out_cmdfifo_outfifo_bcnt_ch0 : RO; bitpos: [9:0]; default: 0;
         *  only for debug
         */
        uint32_t out_cmdfifo_outfifo_bcnt_ch0:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_out_fifo_bcnt_ch0_reg_t;

/** Type of out_push_bytecnt_ch0 register
 *  tx CH0 push byte cnt register
 */
typedef union {
    struct {
        /** out_cmdfifo_push_bytecnt_ch0 : RO; bitpos: [7:0]; default: 255;
         *  only for debug
         */
        uint32_t out_cmdfifo_push_bytecnt_ch0:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} h264_dma_out_push_bytecnt_ch0_reg_t;

/** Type of out_xaddr_ch0 register
 *  tx CH0 xaddr register
 */
typedef union {
    struct {
        /** out_cmdfifo_xaddr_ch0 : RO; bitpos: [31:0]; default: 0;
         *  only for debug
         */
        uint32_t out_cmdfifo_xaddr_ch0:32;
    };
    uint32_t val;
} h264_dma_out_xaddr_ch0_reg_t;


/** Group: TX CH1 config0 register */
/** Type of out_conf0_ch1 register
 *  TX CH1 config0 register
 */
typedef union {
    struct {
        /** out_auto_wrback_ch1 : R/W; bitpos: [0]; default: 0;
         *  Set this bit to enable automatic outlink-writeback when all the data pointed by
         *  outlink descriptor has been received.
         */
        uint32_t out_auto_wrback_ch1:1;
        /** out_eof_mode_ch1 : R/W; bitpos: [1]; default: 1;
         *  EOF flag generation mode when receiving data. 1: EOF flag for Tx channel 0 is
         *  generated when data need to read has been popped from FIFO in DMA
         */
        uint32_t out_eof_mode_ch1:1;
        /** outdscr_burst_en_ch1 : R/W; bitpos: [2]; default: 0;
         *  Set this bit to 1 to enable INCR burst transfer for Tx channel 0 reading link
         *  descriptor when accessing internal SRAM.
         */
        uint32_t outdscr_burst_en_ch1:1;
        /** out_ecc_aes_en_ch1 : R/W; bitpos: [3]; default: 0;
         *  When access address space is ecc/aes area, this bit should be set to 1. In this
         *  case, the start address of square should be 16-bit aligned. The width of square
         *  multiply byte number of one pixel should be 16-bit aligned.
         */
        uint32_t out_ecc_aes_en_ch1:1;
        /** out_check_owner_ch1 : R/W; bitpos: [4]; default: 0;
         *  Set this bit to enable checking the owner attribute of the link descriptor.
         */
        uint32_t out_check_owner_ch1:1;
        uint32_t reserved_5:1;
        /** out_mem_burst_length_ch1 : R/W; bitpos: [8:6]; default: 0;
         *  Block size of Tx channel 1. 0: single      1: 16 bytes      2: 32 bytes    3: 64
         *  bytes    4: 128 64 bytes
         */
        uint32_t out_mem_burst_length_ch1:3;
        uint32_t reserved_9:3;
        /** out_page_bound_en_ch1 : R/W; bitpos: [12]; default: 0;
         *  Set this bit to 1 to make sure AXI read data don't cross the address boundary which
         *  define by mem_burst_length
         */
        uint32_t out_page_bound_en_ch1:1;
        uint32_t reserved_13:11;
        /** out_rst_ch1 : R/W; bitpos: [24]; default: 0;
         *  Write 1 then write 0 to this bit to reset TX channel
         */
        uint32_t out_rst_ch1:1;
        /** out_cmd_disable_ch1 : R/W; bitpos: [25]; default: 0;
         *  Write 1 before reset and write 0 after reset
         */
        uint32_t out_cmd_disable_ch1:1;
        /** out_arb_weight_opt_dis_ch1 : R/W; bitpos: [26]; default: 0;
         *  Set this bit to 1 to disable arbiter optimum weight function.
         */
        uint32_t out_arb_weight_opt_dis_ch1:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} h264_dma_out_conf0_ch1_reg_t;


/** Group: TX CH1 out_link dscr addr register */
/** Type of out_link_addr_ch1 register
 *  TX CH1 out_link dscr addr register
 */
typedef union {
    struct {
        /** outlink_addr_ch1 : R/W; bitpos: [31:0]; default: 0;
         *  This register stores the first outlink descriptor's address.
         */
        uint32_t outlink_addr_ch1:32;
    };
    uint32_t val;
} h264_dma_out_link_addr_ch1_reg_t;


/** Group: TX CH1 arb register */
/** Type of out_arb_ch1 register
 *  TX CH1 arb register
 */
typedef union {
    struct {
        /** out_arb_token_num_ch1 : R/W; bitpos: [3:0]; default: 1;
         *  Set the max number of token count of arbiter
         */
        uint32_t out_arb_token_num_ch1:4;
        uint32_t reserved_4:2;
        /** inter_out_arb_priority_ch1 : R/W; bitpos: [6]; default: 1;
         *  Set the priority of channel
         */
        uint32_t inter_out_arb_priority_ch1:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} h264_dma_out_arb_ch1_reg_t;


/** Group: TX CH1 ETM config register */
/** Type of out_etm_conf_ch1 register
 *  TX CH1 ETM config register
 */
typedef union {
    struct {
        /** out_etm_en_ch1 : R/W; bitpos: [0]; default: 0;
         *  Set this bit to 1 to enable ETM task function
         */
        uint32_t out_etm_en_ch1:1;
        /** out_etm_loop_en_ch1 : R/W; bitpos: [1]; default: 0;
         *  when this bit is 1, dscr can be processed after receiving a task
         */
        uint32_t out_etm_loop_en_ch1:1;
        /** out_dscr_task_mak_ch1 : R/W; bitpos: [3:2]; default: 1;
         *  ETM dscr_ready maximum cache numbers
         */
        uint32_t out_dscr_task_mak_ch1:2;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} h264_dma_out_etm_conf_ch1_reg_t;


/** Group: TX CH1 debug info */
/** Type of out_buf_len_ch1 register
 *  tx CH1 buf len register
 */
typedef union {
    struct {
        /** out_cmdfifo_buf_len_hb_ch1 : RO; bitpos: [12:0]; default: 0;
         *  only for debug
         */
        uint32_t out_cmdfifo_buf_len_hb_ch1:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} h264_dma_out_buf_len_ch1_reg_t;

/** Type of out_fifo_bcnt_ch1 register
 *  tx CH1 fifo byte cnt register
 */
typedef union {
    struct {
        /** out_cmdfifo_outfifo_bcnt_ch1 : RO; bitpos: [9:0]; default: 0;
         *  only for debug
         */
        uint32_t out_cmdfifo_outfifo_bcnt_ch1:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_out_fifo_bcnt_ch1_reg_t;

/** Type of out_push_bytecnt_ch1 register
 *  tx CH1 push byte cnt register
 */
typedef union {
    struct {
        /** out_cmdfifo_push_bytecnt_ch1 : RO; bitpos: [7:0]; default: 255;
         *  only for debug
         */
        uint32_t out_cmdfifo_push_bytecnt_ch1:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} h264_dma_out_push_bytecnt_ch1_reg_t;

/** Type of out_xaddr_ch1 register
 *  tx CH1 xaddr register
 */
typedef union {
    struct {
        /** out_cmdfifo_xaddr_ch1 : RO; bitpos: [31:0]; default: 0;
         *  only for debug
         */
        uint32_t out_cmdfifo_xaddr_ch1:32;
    };
    uint32_t val;
} h264_dma_out_xaddr_ch1_reg_t;


/** Group: TX CH2 config0 register */
/** Type of out_conf0_ch2 register
 *  TX CH2 config0 register
 */
typedef union {
    struct {
        /** out_auto_wrback_ch2 : R/W; bitpos: [0]; default: 0;
         *  Set this bit to enable automatic outlink-writeback when all the data pointed by
         *  outlink descriptor has been received.
         */
        uint32_t out_auto_wrback_ch2:1;
        /** out_eof_mode_ch2 : R/W; bitpos: [1]; default: 1;
         *  EOF flag generation mode when receiving data. 1: EOF flag for Tx channel 0 is
         *  generated when data need to read has been popped from FIFO in DMA
         */
        uint32_t out_eof_mode_ch2:1;
        /** outdscr_burst_en_ch2 : R/W; bitpos: [2]; default: 0;
         *  Set this bit to 1 to enable INCR burst transfer for Tx channel 0 reading link
         *  descriptor when accessing internal SRAM.
         */
        uint32_t outdscr_burst_en_ch2:1;
        /** out_ecc_aes_en_ch2 : R/W; bitpos: [3]; default: 0;
         *  When access address space is ecc/aes area, this bit should be set to 1. In this
         *  case, the start address of square should be 16-bit aligned. The width of square
         *  multiply byte number of one pixel should be 16-bit aligned.
         */
        uint32_t out_ecc_aes_en_ch2:1;
        /** out_check_owner_ch2 : R/W; bitpos: [4]; default: 0;
         *  Set this bit to enable checking the owner attribute of the link descriptor.
         */
        uint32_t out_check_owner_ch2:1;
        uint32_t reserved_5:1;
        /** out_mem_burst_length_ch2 : R/W; bitpos: [8:6]; default: 0;
         *  Block size of Tx channel 2. 0: single      1: 16 bytes      2: 32 bytes    3: 64
         *  bytes    4: 128 bytes
         */
        uint32_t out_mem_burst_length_ch2:3;
        uint32_t reserved_9:3;
        /** out_page_bound_en_ch2 : R/W; bitpos: [12]; default: 0;
         *  Set this bit to 1 to make sure AXI read data don't cross the address boundary which
         *  define by mem_burst_length
         */
        uint32_t out_page_bound_en_ch2:1;
        uint32_t reserved_13:11;
        /** out_rst_ch2 : R/W; bitpos: [24]; default: 0;
         *  Write 1 then write 0 to this bit to reset TX channel
         */
        uint32_t out_rst_ch2:1;
        /** out_cmd_disable_ch2 : R/W; bitpos: [25]; default: 0;
         *  Write 1 before reset and write 0 after reset
         */
        uint32_t out_cmd_disable_ch2:1;
        /** out_arb_weight_opt_dis_ch2 : R/W; bitpos: [26]; default: 0;
         *  Set this bit to 1 to disable arbiter optimum weight function.
         */
        uint32_t out_arb_weight_opt_dis_ch2:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} h264_dma_out_conf0_ch2_reg_t;


/** Group: TX CH2 out_link dscr addr register */
/** Type of out_link_addr_ch2 register
 *  TX CH2 out_link dscr addr register
 */
typedef union {
    struct {
        /** outlink_addr_ch2 : R/W; bitpos: [31:0]; default: 0;
         *  This register stores the first outlink descriptor's address.
         */
        uint32_t outlink_addr_ch2:32;
    };
    uint32_t val;
} h264_dma_out_link_addr_ch2_reg_t;


/** Group: TX CH2 arb register */
/** Type of out_arb_ch2 register
 *  TX CH2 arb register
 */
typedef union {
    struct {
        /** out_arb_token_num_ch2 : R/W; bitpos: [3:0]; default: 1;
         *  Set the max number of token count of arbiter
         */
        uint32_t out_arb_token_num_ch2:4;
        uint32_t reserved_4:2;
        /** inter_out_arb_priority_ch2 : R/W; bitpos: [6]; default: 1;
         *  Set the priority of channel
         */
        uint32_t inter_out_arb_priority_ch2:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} h264_dma_out_arb_ch2_reg_t;


/** Group: TX CH2 ETM config register */
/** Type of out_etm_conf_ch2 register
 *  TX CH2 ETM config register
 */
typedef union {
    struct {
        /** out_etm_en_ch2 : R/W; bitpos: [0]; default: 0;
         *  Set this bit to 1 to enable ETM task function
         */
        uint32_t out_etm_en_ch2:1;
        /** out_etm_loop_en_ch2 : R/W; bitpos: [1]; default: 0;
         *  when this bit is 1, dscr can be processed after receiving a task
         */
        uint32_t out_etm_loop_en_ch2:1;
        /** out_dscr_task_mak_ch2 : R/W; bitpos: [3:2]; default: 1;
         *  ETM dscr_ready maximum cache numbers
         */
        uint32_t out_dscr_task_mak_ch2:2;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} h264_dma_out_etm_conf_ch2_reg_t;


/** Group: TX CH2 debug info */
/** Type of out_buf_len_ch2 register
 *  tx CH2 buf len register
 */
typedef union {
    struct {
        /** out_cmdfifo_buf_len_hb_ch2 : RO; bitpos: [12:0]; default: 0;
         *  only for debug
         */
        uint32_t out_cmdfifo_buf_len_hb_ch2:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} h264_dma_out_buf_len_ch2_reg_t;

/** Type of out_fifo_bcnt_ch2 register
 *  tx CH2 fifo byte cnt register
 */
typedef union {
    struct {
        /** out_cmdfifo_outfifo_bcnt_ch2 : RO; bitpos: [9:0]; default: 0;
         *  only for debug
         */
        uint32_t out_cmdfifo_outfifo_bcnt_ch2:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_out_fifo_bcnt_ch2_reg_t;

/** Type of out_push_bytecnt_ch2 register
 *  tx CH2 push byte cnt register
 */
typedef union {
    struct {
        /** out_cmdfifo_push_bytecnt_ch2 : RO; bitpos: [7:0]; default: 255;
         *  only for debug
         */
        uint32_t out_cmdfifo_push_bytecnt_ch2:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} h264_dma_out_push_bytecnt_ch2_reg_t;

/** Type of out_xaddr_ch2 register
 *  tx CH2 xaddr register
 */
typedef union {
    struct {
        /** out_cmdfifo_xaddr_ch2 : RO; bitpos: [31:0]; default: 0;
         *  only for debug
         */
        uint32_t out_cmdfifo_xaddr_ch2:32;
    };
    uint32_t val;
} h264_dma_out_xaddr_ch2_reg_t;


/** Group: TX CH3 config0 register */
/** Type of out_conf0_ch3 register
 *  TX CH3 config0 register
 */
typedef union {
    struct {
        /** out_auto_wrback_ch3 : R/W; bitpos: [0]; default: 0;
         *  Set this bit to enable automatic outlink-writeback when all the data pointed by
         *  outlink descriptor has been received.
         */
        uint32_t out_auto_wrback_ch3:1;
        /** out_eof_mode_ch3 : R/W; bitpos: [1]; default: 1;
         *  EOF flag generation mode when receiving data. 1: EOF flag for Tx channel 0 is
         *  generated when data need to read has been popped from FIFO in DMA
         */
        uint32_t out_eof_mode_ch3:1;
        /** outdscr_burst_en_ch3 : R/W; bitpos: [2]; default: 0;
         *  Set this bit to 1 to enable INCR burst transfer for Tx channel 0 reading link
         *  descriptor when accessing internal SRAM.
         */
        uint32_t outdscr_burst_en_ch3:1;
        /** out_ecc_aes_en_ch3 : R/W; bitpos: [3]; default: 0;
         *  When access address space is ecc/aes area, this bit should be set to 1. In this
         *  case, the start address of square should be 16-bit aligned. The width of square
         *  multiply byte number of one pixel should be 16-bit aligned.
         */
        uint32_t out_ecc_aes_en_ch3:1;
        /** out_check_owner_ch3 : R/W; bitpos: [4]; default: 0;
         *  Set this bit to enable checking the owner attribute of the link descriptor.
         */
        uint32_t out_check_owner_ch3:1;
        uint32_t reserved_5:1;
        /** out_mem_burst_length_ch3 : R/W; bitpos: [8:6]; default: 0;
         *  Block size of Tx channel 3. 0: single      1: 16 bytes      2: 32 bytes    3: 64
         *  bytes    4: 128 bytes
         */
        uint32_t out_mem_burst_length_ch3:3;
        uint32_t reserved_9:3;
        /** out_page_bound_en_ch3 : R/W; bitpos: [12]; default: 0;
         *  Set this bit to 1 to make sure AXI read data don't cross the address boundary which
         *  define by mem_burst_length
         */
        uint32_t out_page_bound_en_ch3:1;
        uint32_t reserved_13:13;
        /** out_arb_weight_opt_dis_ch3 : R/W; bitpos: [26]; default: 0;
         *  Set this bit to 1 to disable arbiter optimum weight function.
         */
        uint32_t out_arb_weight_opt_dis_ch3:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} h264_dma_out_conf0_ch3_reg_t;


/** Group: TX CH3 out_link dscr addr register */
/** Type of out_link_addr_ch3 register
 *  TX CH3 out_link dscr addr register
 */
typedef union {
    struct {
        /** outlink_addr_ch3 : R/W; bitpos: [31:0]; default: 0;
         *  This register stores the first outlink descriptor's address.
         */
        uint32_t outlink_addr_ch3:32;
    };
    uint32_t val;
} h264_dma_out_link_addr_ch3_reg_t;


/** Group: TX CH3 arb register */
/** Type of out_arb_ch3 register
 *  TX CH3 arb register
 */
typedef union {
    struct {
        /** out_arb_token_num_ch3 : R/W; bitpos: [3:0]; default: 1;
         *  Set the max number of token count of arbiter
         */
        uint32_t out_arb_token_num_ch3:4;
        /** exter_out_arb_priority_ch3 : R/W; bitpos: [5:4]; default: 1;
         *  Set the priority of channel
         */
        uint32_t exter_out_arb_priority_ch3:2;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} h264_dma_out_arb_ch3_reg_t;


/** Group: TX CH3 ETM config register */
/** Type of out_etm_conf_ch3 register
 *  TX CH3 ETM config register
 */
typedef union {
    struct {
        /** out_etm_en_ch3 : R/W; bitpos: [0]; default: 0;
         *  Set this bit to 1 to enable ETM task function
         */
        uint32_t out_etm_en_ch3:1;
        /** out_etm_loop_en_ch3 : R/W; bitpos: [1]; default: 0;
         *  when this bit is 1, dscr can be processed after receiving a task
         */
        uint32_t out_etm_loop_en_ch3:1;
        /** out_dscr_task_mak_ch3 : R/W; bitpos: [3:2]; default: 1;
         *  ETM dscr_ready maximum cache numbers
         */
        uint32_t out_dscr_task_mak_ch3:2;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} h264_dma_out_etm_conf_ch3_reg_t;


/** Group: TX CH3 debug info */
/** Type of out_buf_len_ch3 register
 *  tx CH3 buf len register
 */
typedef union {
    struct {
        /** out_cmdfifo_buf_len_hb_ch3 : RO; bitpos: [12:0]; default: 0;
         *  only for debug
         */
        uint32_t out_cmdfifo_buf_len_hb_ch3:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} h264_dma_out_buf_len_ch3_reg_t;

/** Type of out_fifo_bcnt_ch3 register
 *  tx CH3 fifo byte cnt register
 */
typedef union {
    struct {
        /** out_cmdfifo_outfifo_bcnt_ch3 : RO; bitpos: [9:0]; default: 0;
         *  only for debug
         */
        uint32_t out_cmdfifo_outfifo_bcnt_ch3:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_out_fifo_bcnt_ch3_reg_t;

/** Type of out_push_bytecnt_ch3 register
 *  tx CH3 push byte cnt register
 */
typedef union {
    struct {
        /** out_cmdfifo_push_bytecnt_ch3 : RO; bitpos: [7:0]; default: 63;
         *  only for debug
         */
        uint32_t out_cmdfifo_push_bytecnt_ch3:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} h264_dma_out_push_bytecnt_ch3_reg_t;

/** Type of out_xaddr_ch3 register
 *  tx CH3 xaddr register
 */
typedef union {
    struct {
        /** out_cmdfifo_xaddr_ch3 : RO; bitpos: [31:0]; default: 0;
         *  only for debug
         */
        uint32_t out_cmdfifo_xaddr_ch3:32;
    };
    uint32_t val;
} h264_dma_out_xaddr_ch3_reg_t;

/** Type of out_block_buf_len_ch3 register
 *  tx CH3 block buf len register
 */
typedef union {
    struct {
        /** out_block_buf_len_ch3 : RO; bitpos: [27:0]; default: 0;
         *  only for debug
         */
        uint32_t out_block_buf_len_ch3:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} h264_dma_out_block_buf_len_ch3_reg_t;


/** Group: TX CH4 config0 register */
/** Type of out_conf0_ch4 register
 *  TX CH4 config0 register
 */
typedef union {
    struct {
        /** out_auto_wrback_ch4 : R/W; bitpos: [0]; default: 0;
         *  Set this bit to enable automatic outlink-writeback when all the data pointed by
         *  outlink descriptor has been received.
         */
        uint32_t out_auto_wrback_ch4:1;
        /** out_eof_mode_ch4 : R/W; bitpos: [1]; default: 1;
         *  EOF flag generation mode when receiving data. 1: EOF flag for Tx channel 0 is
         *  generated when data need to read has been popped from FIFO in DMA
         */
        uint32_t out_eof_mode_ch4:1;
        /** outdscr_burst_en_ch4 : R/W; bitpos: [2]; default: 0;
         *  Set this bit to 1 to enable INCR burst transfer for Tx channel 0 reading link
         *  descriptor when accessing internal SRAM.
         */
        uint32_t outdscr_burst_en_ch4:1;
        /** out_ecc_aes_en_ch4 : R/W; bitpos: [3]; default: 0;
         *  When access address space is ecc/aes area, this bit should be set to 1. In this
         *  case, the start address of square should be 16-bit aligned. The width of square
         *  multiply byte number of one pixel should be 16-bit aligned.
         */
        uint32_t out_ecc_aes_en_ch4:1;
        /** out_check_owner_ch4 : R/W; bitpos: [4]; default: 0;
         *  Set this bit to enable checking the owner attribute of the link descriptor.
         */
        uint32_t out_check_owner_ch4:1;
        uint32_t reserved_5:1;
        /** out_mem_burst_length_ch4 : R/W; bitpos: [8:6]; default: 0;
         *  Block size of Tx channel 4. 0: single      1: 16 bytes      2: 32 bytes    3: 64
         *  bytes    4: 128 bytes
         */
        uint32_t out_mem_burst_length_ch4:3;
        uint32_t reserved_9:3;
        /** out_page_bound_en_ch4 : R/W; bitpos: [12]; default: 0;
         *  Set this bit to 1 to make sure AXI read data don't cross the address boundary which
         *  define by mem_burst_length
         */
        uint32_t out_page_bound_en_ch4:1;
        uint32_t reserved_13:13;
        /** out_arb_weight_opt_dis_ch4 : R/W; bitpos: [26]; default: 0;
         *  Set this bit to 1 to disable arbiter optimum weight function.
         */
        uint32_t out_arb_weight_opt_dis_ch4:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} h264_dma_out_conf0_ch4_reg_t;


/** Group: TX CH4 out_link dscr addr register */
/** Type of out_link_addr_ch4 register
 *  TX CH4 out_link dscr addr register
 */
typedef union {
    struct {
        /** outlink_addr_ch4 : R/W; bitpos: [31:0]; default: 0;
         *  This register stores the first outlink descriptor's address.
         */
        uint32_t outlink_addr_ch4:32;
    };
    uint32_t val;
} h264_dma_out_link_addr_ch4_reg_t;


/** Group: TX CH4 arb register */
/** Type of out_arb_ch4 register
 *  TX CH4 arb register
 */
typedef union {
    struct {
        /** out_arb_token_num_ch4 : R/W; bitpos: [3:0]; default: 1;
         *  Set the max number of token count of arbiter
         */
        uint32_t out_arb_token_num_ch4:4;
        /** exter_out_arb_priority_ch4 : R/W; bitpos: [5:4]; default: 1;
         *  Set the priority of channel
         */
        uint32_t exter_out_arb_priority_ch4:2;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} h264_dma_out_arb_ch4_reg_t;


/** Group: TX CH4 ETM config register */
/** Type of out_etm_conf_ch4 register
 *  TX CH4 ETM config register
 */
typedef union {
    struct {
        /** out_etm_en_ch4 : R/W; bitpos: [0]; default: 0;
         *  Set this bit to 1 to enable ETM task function
         */
        uint32_t out_etm_en_ch4:1;
        /** out_etm_loop_en_ch4 : R/W; bitpos: [1]; default: 0;
         *  when this bit is 1, dscr can be processed after receiving a task
         */
        uint32_t out_etm_loop_en_ch4:1;
        /** out_dscr_task_mak_ch4 : R/W; bitpos: [3:2]; default: 1;
         *  ETM dscr_ready maximum cache numbers
         */
        uint32_t out_dscr_task_mak_ch4:2;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} h264_dma_out_etm_conf_ch4_reg_t;


/** Group: TX CH4 debug info */
/** Type of out_buf_len_ch4 register
 *  tx CH4 buf len register
 */
typedef union {
    struct {
        /** out_cmdfifo_buf_len_hb_ch4 : RO; bitpos: [12:0]; default: 0;
         *  only for debug
         */
        uint32_t out_cmdfifo_buf_len_hb_ch4:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} h264_dma_out_buf_len_ch4_reg_t;

/** Type of out_fifo_bcnt_ch4 register
 *  tx CH4 fifo byte cnt register
 */
typedef union {
    struct {
        /** out_cmdfifo_outfifo_bcnt_ch4 : RO; bitpos: [9:0]; default: 0;
         *  only for debug
         */
        uint32_t out_cmdfifo_outfifo_bcnt_ch4:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_out_fifo_bcnt_ch4_reg_t;

/** Type of out_push_bytecnt_ch4 register
 *  tx CH4 push byte cnt register
 */
typedef union {
    struct {
        /** out_cmdfifo_push_bytecnt_ch4 : RO; bitpos: [7:0]; default: 63;
         *  only for debug
         */
        uint32_t out_cmdfifo_push_bytecnt_ch4:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} h264_dma_out_push_bytecnt_ch4_reg_t;

/** Type of out_xaddr_ch4 register
 *  tx CH4 xaddr register
 */
typedef union {
    struct {
        /** out_cmdfifo_xaddr_ch4 : RO; bitpos: [31:0]; default: 0;
         *  only for debug
         */
        uint32_t out_cmdfifo_xaddr_ch4:32;
    };
    uint32_t val;
} h264_dma_out_xaddr_ch4_reg_t;

/** Type of out_block_buf_len_ch4 register
 *  tx CH4 block buf len register
 */
typedef union {
    struct {
        /** out_block_buf_len_ch4 : RO; bitpos: [27:0]; default: 0;
         *  only for debug
         */
        uint32_t out_block_buf_len_ch4:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} h264_dma_out_block_buf_len_ch4_reg_t;


/** Group: RX CH0 in_link dscr addr register */
/** Type of in_link_addr_ch0 register
 *  RX CH0 in_link dscr addr register
 */
typedef union {
    struct {
        /** inlink_addr_ch0 : R/W; bitpos: [31:0]; default: 0;
         *  This register stores the first inlink descriptor's address.
         */
        uint32_t inlink_addr_ch0:32;
    };
    uint32_t val;
} h264_dma_in_link_addr_ch0_reg_t;


/** Group: RX CH0 arb register */
/** Type of in_arb_ch0 register
 *  RX CH0 arb register
 */
typedef union {
    struct {
        /** in_arb_token_num_ch0 : R/W; bitpos: [3:0]; default: 1;
         *  Set the max number of token count of arbiter
         */
        uint32_t in_arb_token_num_ch0:4;
        /** exter_in_arb_priority_ch0 : R/W; bitpos: [5:4]; default: 1;
         *  Set the priority of channel
         */
        uint32_t exter_in_arb_priority_ch0:2;
        /** inter_in_arb_priority_ch0 : R/W; bitpos: [8:6]; default: 1;
         *  Set the priority of channel
         */
        uint32_t inter_in_arb_priority_ch0:3;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_in_arb_ch0_reg_t;


/** Group: RX CH0 ETM config register */
/** Type of in_etm_conf_ch0 register
 *  RX CH0 ETM config register
 */
typedef union {
    struct {
        /** in_etm_en_ch0 : R/W; bitpos: [0]; default: 0;
         *  Set this bit to 1 to enable ETM task function
         */
        uint32_t in_etm_en_ch0:1;
        /** in_etm_loop_en_ch0 : R/W; bitpos: [1]; default: 0;
         *  when this bit is 1, dscr can be processed after receiving a task
         */
        uint32_t in_etm_loop_en_ch0:1;
        /** in_dscr_task_mak_ch0 : R/W; bitpos: [3:2]; default: 1;
         *  ETM dscr_ready maximum cache numbers
         */
        uint32_t in_dscr_task_mak_ch0:2;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} h264_dma_in_etm_conf_ch0_reg_t;


/** Group: RX CH0 debug info */
/** Type of in_fifo_cnt_ch0 register
 *  rx CH0 fifo cnt register
 */
typedef union {
    struct {
        /** in_cmdfifo_infifo_cnt_ch0 : RO; bitpos: [9:0]; default: 0;
         *  only for debug
         */
        uint32_t in_cmdfifo_infifo_cnt_ch0:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_fifo_cnt_ch0_reg_t;

/** Type of in_pop_data_cnt_ch0 register
 *  rx CH0 pop data cnt register
 */
typedef union {
    struct {
        /** in_cmdfifo_pop_data_cnt_ch0 : RO; bitpos: [7:0]; default: 7;
         *  only for debug
         */
        uint32_t in_cmdfifo_pop_data_cnt_ch0:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} h264_dma_in_pop_data_cnt_ch0_reg_t;

/** Type of in_xaddr_ch0 register
 *  rx CH0 xaddr register
 */
typedef union {
    struct {
        /** in_cmdfifo_xaddr_ch0 : RO; bitpos: [31:0]; default: 0;
         *  only for debug
         */
        uint32_t in_cmdfifo_xaddr_ch0:32;
    };
    uint32_t val;
} h264_dma_in_xaddr_ch0_reg_t;

/** Type of in_buf_hb_rcv_ch0 register
 *  rx CH0 buf len hb rcv register
 */
typedef union {
    struct {
        /** in_cmdfifo_buf_hb_rcv_ch0 : RO; bitpos: [28:0]; default: 0;
         *  only for debug
         */
        uint32_t in_cmdfifo_buf_hb_rcv_ch0:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} h264_dma_in_buf_hb_rcv_ch0_reg_t;


/** Group: RX CH1 in_link dscr addr register */
/** Type of in_link_addr_ch1 register
 *  RX CH1 in_link dscr addr register
 */
typedef union {
    struct {
        /** inlink_addr_ch1 : R/W; bitpos: [31:0]; default: 0;
         *  This register stores the first inlink descriptor's address.
         */
        uint32_t inlink_addr_ch1:32;
    };
    uint32_t val;
} h264_dma_in_link_addr_ch1_reg_t;


/** Group: RX CH1 arb register */
/** Type of in_arb_ch1 register
 *  RX CH1 arb register
 */
typedef union {
    struct {
        /** in_arb_token_num_ch1 : R/W; bitpos: [3:0]; default: 1;
         *  Set the max number of token count of arbiter
         */
        uint32_t in_arb_token_num_ch1:4;
        /** exter_in_arb_priority_ch1 : R/W; bitpos: [5:4]; default: 1;
         *  Set the priority of channel
         */
        uint32_t exter_in_arb_priority_ch1:2;
        /** inter_in_arb_priority_ch1 : R/W; bitpos: [8:6]; default: 1;
         *  Set the priority of channel
         */
        uint32_t inter_in_arb_priority_ch1:3;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_in_arb_ch1_reg_t;


/** Group: RX CH1 ETM config register */
/** Type of in_etm_conf_ch1 register
 *  RX CH1 ETM config register
 */
typedef union {
    struct {
        /** in_etm_en_ch1 : R/W; bitpos: [0]; default: 0;
         *  Set this bit to 1 to enable ETM task function
         */
        uint32_t in_etm_en_ch1:1;
        /** in_etm_loop_en_ch1 : R/W; bitpos: [1]; default: 0;
         *  when this bit is 1, dscr can be processed after receiving a task
         */
        uint32_t in_etm_loop_en_ch1:1;
        /** in_dscr_task_mak_ch1 : R/W; bitpos: [3:2]; default: 1;
         *  ETM dscr_ready maximum cache numbers
         */
        uint32_t in_dscr_task_mak_ch1:2;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} h264_dma_in_etm_conf_ch1_reg_t;


/** Group: RX CH1 debug info */
/** Type of in_fifo_cnt_ch1 register
 *  rx CH1 fifo cnt register
 */
typedef union {
    struct {
        /** in_cmdfifo_infifo_cnt_ch1 : RO; bitpos: [9:0]; default: 0;
         *  only for debug
         */
        uint32_t in_cmdfifo_infifo_cnt_ch1:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_fifo_cnt_ch1_reg_t;

/** Type of in_pop_data_cnt_ch1 register
 *  rx CH1 pop data cnt register
 */
typedef union {
    struct {
        /** in_cmdfifo_pop_data_cnt_ch1 : RO; bitpos: [7:0]; default: 7;
         *  only for debug
         */
        uint32_t in_cmdfifo_pop_data_cnt_ch1:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} h264_dma_in_pop_data_cnt_ch1_reg_t;

/** Type of in_xaddr_ch1 register
 *  rx CH1 xaddr register
 */
typedef union {
    struct {
        /** in_cmdfifo_xaddr_ch1 : RO; bitpos: [31:0]; default: 0;
         *  only for debug
         */
        uint32_t in_cmdfifo_xaddr_ch1:32;
    };
    uint32_t val;
} h264_dma_in_xaddr_ch1_reg_t;

/** Type of in_buf_hb_rcv_ch1 register
 *  rx CH1 buf len hb rcv register
 */
typedef union {
    struct {
        /** in_cmdfifo_buf_hb_rcv_ch1 : RO; bitpos: [28:0]; default: 0;
         *  only for debug
         */
        uint32_t in_cmdfifo_buf_hb_rcv_ch1:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} h264_dma_in_buf_hb_rcv_ch1_reg_t;


/** Group: RX CH2 in_link dscr addr register */
/** Type of in_link_addr_ch2 register
 *  RX CH2 in_link dscr addr register
 */
typedef union {
    struct {
        /** inlink_addr_ch2 : R/W; bitpos: [31:0]; default: 0;
         *  This register stores the first inlink descriptor's address.
         */
        uint32_t inlink_addr_ch2:32;
    };
    uint32_t val;
} h264_dma_in_link_addr_ch2_reg_t;


/** Group: RX CH2 arb register */
/** Type of in_arb_ch2 register
 *  RX CH2 arb register
 */
typedef union {
    struct {
        /** in_arb_token_num_ch2 : R/W; bitpos: [3:0]; default: 1;
         *  Set the max number of token count of arbiter
         */
        uint32_t in_arb_token_num_ch2:4;
        uint32_t reserved_4:2;
        /** inter_in_arb_priority_ch2 : R/W; bitpos: [8:6]; default: 1;
         *  Set the priority of channel
         */
        uint32_t inter_in_arb_priority_ch2:3;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_in_arb_ch2_reg_t;


/** Group: RX CH2 ETM config register */
/** Type of in_etm_conf_ch2 register
 *  RX CH2 ETM config register
 */
typedef union {
    struct {
        /** in_etm_en_ch2 : R/W; bitpos: [0]; default: 0;
         *  Set this bit to 1 to enable ETM task function
         */
        uint32_t in_etm_en_ch2:1;
        /** in_etm_loop_en_ch2 : R/W; bitpos: [1]; default: 0;
         *  when this bit is 1, dscr can be processed after receiving a task
         */
        uint32_t in_etm_loop_en_ch2:1;
        /** in_dscr_task_mak_ch2 : R/W; bitpos: [3:2]; default: 1;
         *  ETM dscr_ready maximum cache numbers
         */
        uint32_t in_dscr_task_mak_ch2:2;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} h264_dma_in_etm_conf_ch2_reg_t;


/** Group: RX CH2 debug info */
/** Type of in_fifo_cnt_ch2 register
 *  rx CH2 fifo cnt register
 */
typedef union {
    struct {
        /** in_cmdfifo_infifo_cnt_ch2 : RO; bitpos: [9:0]; default: 0;
         *  only for debug
         */
        uint32_t in_cmdfifo_infifo_cnt_ch2:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_fifo_cnt_ch2_reg_t;

/** Type of in_pop_data_cnt_ch2 register
 *  rx CH2 pop data cnt register
 */
typedef union {
    struct {
        /** in_cmdfifo_pop_data_cnt_ch2 : RO; bitpos: [7:0]; default: 7;
         *  only for debug
         */
        uint32_t in_cmdfifo_pop_data_cnt_ch2:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} h264_dma_in_pop_data_cnt_ch2_reg_t;

/** Type of in_xaddr_ch2 register
 *  rx CH2 xaddr register
 */
typedef union {
    struct {
        /** in_cmdfifo_xaddr_ch2 : RO; bitpos: [31:0]; default: 0;
         *  only for debug
         */
        uint32_t in_cmdfifo_xaddr_ch2:32;
    };
    uint32_t val;
} h264_dma_in_xaddr_ch2_reg_t;

/** Type of in_buf_hb_rcv_ch2 register
 *  rx CH2 buf len hb rcv register
 */
typedef union {
    struct {
        /** in_cmdfifo_buf_hb_rcv_ch2 : RO; bitpos: [28:0]; default: 0;
         *  only for debug
         */
        uint32_t in_cmdfifo_buf_hb_rcv_ch2:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} h264_dma_in_buf_hb_rcv_ch2_reg_t;


/** Group: RX CH3 in_link dscr addr register */
/** Type of in_link_addr_ch3 register
 *  RX CH3 in_link dscr addr register
 */
typedef union {
    struct {
        /** inlink_addr_ch3 : R/W; bitpos: [31:0]; default: 0;
         *  This register stores the first inlink descriptor's address.
         */
        uint32_t inlink_addr_ch3:32;
    };
    uint32_t val;
} h264_dma_in_link_addr_ch3_reg_t;


/** Group: RX CH3 arb register */
/** Type of in_arb_ch3 register
 *  RX CH3 arb register
 */
typedef union {
    struct {
        /** in_arb_token_num_ch3 : R/W; bitpos: [3:0]; default: 1;
         *  Set the max number of token count of arbiter
         */
        uint32_t in_arb_token_num_ch3:4;
        uint32_t reserved_4:2;
        /** inter_in_arb_priority_ch3 : R/W; bitpos: [8:6]; default: 1;
         *  Set the priority of channel
         */
        uint32_t inter_in_arb_priority_ch3:3;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_in_arb_ch3_reg_t;


/** Group: RX CH3 ETM config register */
/** Type of in_etm_conf_ch3 register
 *  RX CH3 ETM config register
 */
typedef union {
    struct {
        /** in_etm_en_ch3 : R/W; bitpos: [0]; default: 0;
         *  Set this bit to 1 to enable ETM task function
         */
        uint32_t in_etm_en_ch3:1;
        /** in_etm_loop_en_ch3 : R/W; bitpos: [1]; default: 0;
         *  when this bit is 1, dscr can be processed after receiving a task
         */
        uint32_t in_etm_loop_en_ch3:1;
        /** in_dscr_task_mak_ch3 : R/W; bitpos: [3:2]; default: 1;
         *  ETM dscr_ready maximum cache numbers
         */
        uint32_t in_dscr_task_mak_ch3:2;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} h264_dma_in_etm_conf_ch3_reg_t;


/** Group: RX CH3 debug info */
/** Type of in_fifo_cnt_ch3 register
 *  rx CH3 fifo cnt register
 */
typedef union {
    struct {
        /** in_cmdfifo_infifo_cnt_ch3 : RO; bitpos: [9:0]; default: 0;
         *  only for debug
         */
        uint32_t in_cmdfifo_infifo_cnt_ch3:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_fifo_cnt_ch3_reg_t;

/** Type of in_pop_data_cnt_ch3 register
 *  rx CH3 pop data cnt register
 */
typedef union {
    struct {
        /** in_cmdfifo_pop_data_cnt_ch3 : RO; bitpos: [7:0]; default: 7;
         *  only for debug
         */
        uint32_t in_cmdfifo_pop_data_cnt_ch3:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} h264_dma_in_pop_data_cnt_ch3_reg_t;

/** Type of in_xaddr_ch3 register
 *  rx CH3 xaddr register
 */
typedef union {
    struct {
        /** in_cmdfifo_xaddr_ch3 : RO; bitpos: [31:0]; default: 0;
         *  only for debug
         */
        uint32_t in_cmdfifo_xaddr_ch3:32;
    };
    uint32_t val;
} h264_dma_in_xaddr_ch3_reg_t;

/** Type of in_buf_hb_rcv_ch3 register
 *  rx CH3 buf len hb rcv register
 */
typedef union {
    struct {
        /** in_cmdfifo_buf_hb_rcv_ch3 : RO; bitpos: [28:0]; default: 0;
         *  only for debug
         */
        uint32_t in_cmdfifo_buf_hb_rcv_ch3:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} h264_dma_in_buf_hb_rcv_ch3_reg_t;


/** Group: RX CH4 in_link dscr addr register */
/** Type of in_link_addr_ch4 register
 *  RX CH4 in_link dscr addr register
 */
typedef union {
    struct {
        /** inlink_addr_ch4 : R/W; bitpos: [31:0]; default: 0;
         *  This register stores the first inlink descriptor's address.
         */
        uint32_t inlink_addr_ch4:32;
    };
    uint32_t val;
} h264_dma_in_link_addr_ch4_reg_t;


/** Group: RX CH4 arb register */
/** Type of in_arb_ch4 register
 *  RX CH4 arb register
 */
typedef union {
    struct {
        /** in_arb_token_num_ch4 : R/W; bitpos: [3:0]; default: 1;
         *  Set the max number of token count of arbiter
         */
        uint32_t in_arb_token_num_ch4:4;
        /** exter_in_arb_priority_ch4 : R/W; bitpos: [5:4]; default: 1;
         *  Set the priority of channel
         */
        uint32_t exter_in_arb_priority_ch4:2;
        /** inter_in_arb_priority_ch4 : R/W; bitpos: [8:6]; default: 1;
         *  Set the priority of channel
         */
        uint32_t inter_in_arb_priority_ch4:3;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_in_arb_ch4_reg_t;


/** Group: RX CH4 ETM config register */
/** Type of in_etm_conf_ch4 register
 *  RX CH4 ETM config register
 */
typedef union {
    struct {
        /** in_etm_en_ch4 : R/W; bitpos: [0]; default: 0;
         *  Set this bit to 1 to enable ETM task function
         */
        uint32_t in_etm_en_ch4:1;
        /** in_etm_loop_en_ch4 : R/W; bitpos: [1]; default: 0;
         *  when this bit is 1, dscr can be processed after receiving a task
         */
        uint32_t in_etm_loop_en_ch4:1;
        /** in_dscr_task_mak_ch4 : R/W; bitpos: [3:2]; default: 1;
         *  ETM dscr_ready maximum cache numbers
         */
        uint32_t in_dscr_task_mak_ch4:2;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} h264_dma_in_etm_conf_ch4_reg_t;


/** Group: RX CH4 debug info */
/** Type of in_fifo_cnt_ch4 register
 *  rx CH4 fifo cnt register
 */
typedef union {
    struct {
        /** in_cmdfifo_infifo_cnt_ch4 : RO; bitpos: [9:0]; default: 0;
         *  only for debug
         */
        uint32_t in_cmdfifo_infifo_cnt_ch4:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_fifo_cnt_ch4_reg_t;

/** Type of in_pop_data_cnt_ch4 register
 *  rx CH4 pop data cnt register
 */
typedef union {
    struct {
        /** in_cmdfifo_pop_data_cnt_ch4 : RO; bitpos: [7:0]; default: 7;
         *  only for debug
         */
        uint32_t in_cmdfifo_pop_data_cnt_ch4:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} h264_dma_in_pop_data_cnt_ch4_reg_t;

/** Type of in_xaddr_ch4 register
 *  rx CH4 xaddr register
 */
typedef union {
    struct {
        /** in_cmdfifo_xaddr_ch4 : RO; bitpos: [31:0]; default: 0;
         *  only for debug
         */
        uint32_t in_cmdfifo_xaddr_ch4:32;
    };
    uint32_t val;
} h264_dma_in_xaddr_ch4_reg_t;

/** Type of in_buf_hb_rcv_ch4 register
 *  rx CH4 buf len hb rcv register
 */
typedef union {
    struct {
        /** in_cmdfifo_buf_hb_rcv_ch4 : RO; bitpos: [28:0]; default: 0;
         *  only for debug
         */
        uint32_t in_cmdfifo_buf_hb_rcv_ch4:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} h264_dma_in_buf_hb_rcv_ch4_reg_t;


/** Group: RX CH5 config1 register */
/** Type of in_conf1_ch5 register
 *  RX CH5 config1 register
 */
typedef union {
    struct {
        /** block_start_addr_ch5 : R/W; bitpos: [31:0]; default: 0;
         *  RX Channel 5 destination start address
         */
        uint32_t block_start_addr_ch5:32;
    };
    uint32_t val;
} h264_dma_in_conf1_ch5_reg_t;


/** Group: RX CH5 config2 register */
/** Type of in_conf2_ch5 register
 *  RX CH5 config2 register
 */
typedef union {
    struct {
        /** block_row_length_12line_ch5 : R/W; bitpos: [15:0]; default: 30720;
         *  The number of bytes contained in a row block 12line in RX channel 5
         */
        uint32_t block_row_length_12line_ch5:16;
        /** block_row_length_4line_ch5 : R/W; bitpos: [31:16]; default: 15360;
         *  The number of bytes contained in a row block 4line in RX channel 5
         */
        uint32_t block_row_length_4line_ch5:16;
    };
    uint32_t val;
} h264_dma_in_conf2_ch5_reg_t;


/** Group: RX CH5 config3 register */
/** Type of in_conf3_ch5 register
 *  RX CH5 config3 register
 */
typedef union {
    struct {
        /** block_length_12line_ch5 : R/W; bitpos: [13:0]; default: 256;
         *  The number of bytes contained in a block 12line
         */
        uint32_t block_length_12line_ch5:14;
        /** block_length_4line_ch5 : R/W; bitpos: [27:14]; default: 128;
         *  The number of bytes contained in a block 4line
         */
        uint32_t block_length_4line_ch5:14;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} h264_dma_in_conf3_ch5_reg_t;


/** Group: RX CH5 arb register */
/** Type of in_arb_ch5 register
 *  RX CH5 arb register
 */
typedef union {
    struct {
        /** in_arb_token_num_ch5 : R/W; bitpos: [3:0]; default: 1;
         *  Set the max number of token count of arbiter
         */
        uint32_t in_arb_token_num_ch5:4;
        uint32_t reserved_4:2;
        /** inter_in_arb_priority_ch5 : R/W; bitpos: [8:6]; default: 1;
         *  Set the priority of channel
         */
        uint32_t inter_in_arb_priority_ch5:3;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_dma_in_arb_ch5_reg_t;


/** Group: RX CH5 debug info */
/** Type of in_fifo_cnt_ch5 register
 *  rx CH5 fifo cnt register
 */
typedef union {
    struct {
        /** in_cmdfifo_infifo_cnt_ch5 : RO; bitpos: [9:0]; default: 0;
         *  only for debug
         */
        uint32_t in_cmdfifo_infifo_cnt_ch5:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} h264_dma_in_fifo_cnt_ch5_reg_t;

/** Type of in_pop_data_cnt_ch5 register
 *  rx CH5 pop data cnt register
 */
typedef union {
    struct {
        /** in_cmdfifo_pop_data_cnt_ch5 : RO; bitpos: [7:0]; default: 255;
         *  only for debug
         */
        uint32_t in_cmdfifo_pop_data_cnt_ch5:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} h264_dma_in_pop_data_cnt_ch5_reg_t;

/** Type of in_xaddr_ch5 register
 *  rx CH5 xaddr register
 */
typedef union {
    struct {
        /** in_cmdfifo_xaddr_ch5 : RO; bitpos: [31:0]; default: 0;
         *  only for debug
         */
        uint32_t in_cmdfifo_xaddr_ch5:32;
    };
    uint32_t val;
} h264_dma_in_xaddr_ch5_reg_t;

/** Type of in_buf_hb_rcv_ch5 register
 *  rx CH5 buf len hb rcv register
 */
typedef union {
    struct {
        /** in_cmdfifo_buf_hb_rcv_ch5 : RO; bitpos: [28:0]; default: 0;
         *  only for debug
         */
        uint32_t in_cmdfifo_buf_hb_rcv_ch5:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} h264_dma_in_buf_hb_rcv_ch5_reg_t;


/** Group: inter axi err */
/** Type of inter_axi_err register
 *  inter memory axi err register
 */
typedef union {
    struct {
        /** inter_rid_err_cnt : RO; bitpos: [3:0]; default: 0;
         *  AXI read id err cnt
         */
        uint32_t inter_rid_err_cnt:4;
        /** inter_rresp_err_cnt : RO; bitpos: [7:4]; default: 0;
         *  AXI read resp err cnt
         */
        uint32_t inter_rresp_err_cnt:4;
        /** inter_wresp_err_cnt : RO; bitpos: [11:8]; default: 0;
         *  AXI write resp err cnt
         */
        uint32_t inter_wresp_err_cnt:4;
        /** inter_rd_fifo_cnt : RO; bitpos: [14:12]; default: 0;
         *  AXI read cmd fifo remain cmd count
         */
        uint32_t inter_rd_fifo_cnt:3;
        /** inter_rd_bak_fifo_cnt : RO; bitpos: [18:15]; default: 0;
         *  AXI read backup cmd fifo remain cmd count
         */
        uint32_t inter_rd_bak_fifo_cnt:4;
        /** inter_wr_fifo_cnt : RO; bitpos: [21:19]; default: 0;
         *  AXI write cmd fifo remain cmd count
         */
        uint32_t inter_wr_fifo_cnt:3;
        /** inter_wr_bak_fifo_cnt : RO; bitpos: [25:22]; default: 0;
         *  AXI write backup cmd fifo remain cmd count
         */
        uint32_t inter_wr_bak_fifo_cnt:4;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} h264_dma_inter_axi_err_reg_t;


/** Group: exter axi err */
/** Type of exter_axi_err register
 *  exter memory axi err register
 */
typedef union {
    struct {
        /** exter_rid_err_cnt : RO; bitpos: [3:0]; default: 0;
         *  AXI read id err cnt
         */
        uint32_t exter_rid_err_cnt:4;
        /** exter_rresp_err_cnt : RO; bitpos: [7:4]; default: 0;
         *  AXI read resp err cnt
         */
        uint32_t exter_rresp_err_cnt:4;
        /** exter_wresp_err_cnt : RO; bitpos: [11:8]; default: 0;
         *  AXI write resp err cnt
         */
        uint32_t exter_wresp_err_cnt:4;
        /** exter_rd_fifo_cnt : RO; bitpos: [14:12]; default: 0;
         *  AXI read cmd fifo remain cmd count
         */
        uint32_t exter_rd_fifo_cnt:3;
        /** exter_rd_bak_fifo_cnt : RO; bitpos: [18:15]; default: 0;
         *  AXI read backup cmd fifo remain cmd count
         */
        uint32_t exter_rd_bak_fifo_cnt:4;
        /** exter_wr_fifo_cnt : RO; bitpos: [21:19]; default: 0;
         *  AXI write cmd fifo remain cmd count
         */
        uint32_t exter_wr_fifo_cnt:3;
        /** exter_wr_bak_fifo_cnt : RO; bitpos: [25:22]; default: 0;
         *  AXI write backup cmd fifo remain cmd count
         */
        uint32_t exter_wr_bak_fifo_cnt:4;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} h264_dma_exter_axi_err_reg_t;


/** Group: dscr addr range register */
/** Type of inter_mem_start_addr0 register
 *  Start address of inter memory range0 register
 */
typedef union {
    struct {
        /** access_inter_mem_start_addr0 : R/W; bitpos: [31:0]; default: 806354944;
         *  The start address of accessible address space.
         */
        uint32_t access_inter_mem_start_addr0:32;
    };
    uint32_t val;
} h264_dma_inter_mem_start_addr0_reg_t;

/** Type of inter_mem_end_addr0 register
 *  end address of inter memory range0 register
 */
typedef union {
    struct {
        /** access_inter_mem_end_addr0 : R/W; bitpos: [31:0]; default: 2415919103;
         *  The end address of accessible address space. The access address beyond this range
         *  would lead to descriptor error.
         */
        uint32_t access_inter_mem_end_addr0:32;
    };
    uint32_t val;
} h264_dma_inter_mem_end_addr0_reg_t;

/** Type of inter_mem_start_addr1 register
 *  Start address of inter memory range1 register
 */
typedef union {
    struct {
        /** access_inter_mem_start_addr1 : R/W; bitpos: [31:0]; default: 806354944;
         *  The start address of accessible address space.
         */
        uint32_t access_inter_mem_start_addr1:32;
    };
    uint32_t val;
} h264_dma_inter_mem_start_addr1_reg_t;

/** Type of inter_mem_end_addr1 register
 *  end address of inter memory range1 register
 */
typedef union {
    struct {
        /** access_inter_mem_end_addr1 : R/W; bitpos: [31:0]; default: 2415919103;
         *  The end address of accessible address space. The access address beyond this range
         *  would lead to descriptor error.
         */
        uint32_t access_inter_mem_end_addr1:32;
    };
    uint32_t val;
} h264_dma_inter_mem_end_addr1_reg_t;

/** Type of exter_mem_start_addr0 register
 *  Start address of exter memory range0 register
 */
typedef union {
    struct {
        /** access_exter_mem_start_addr0 : R/W; bitpos: [31:0]; default: 806354944;
         *  The start address of accessible address space.
         */
        uint32_t access_exter_mem_start_addr0:32;
    };
    uint32_t val;
} h264_dma_exter_mem_start_addr0_reg_t;

/** Type of exter_mem_end_addr0 register
 *  end address of exter memory range0 register
 */
typedef union {
    struct {
        /** access_exter_mem_end_addr0 : R/W; bitpos: [31:0]; default: 2415919103;
         *  The end address of accessible address space. The access address beyond this range
         *  would lead to descriptor error.
         */
        uint32_t access_exter_mem_end_addr0:32;
    };
    uint32_t val;
} h264_dma_exter_mem_end_addr0_reg_t;

/** Type of exter_mem_start_addr1 register
 *  Start address of exter memory range1 register
 */
typedef union {
    struct {
        /** access_exter_mem_start_addr1 : R/W; bitpos: [31:0]; default: 806354944;
         *  The start address of accessible address space.
         */
        uint32_t access_exter_mem_start_addr1:32;
    };
    uint32_t val;
} h264_dma_exter_mem_start_addr1_reg_t;

/** Type of exter_mem_end_addr1 register
 *  end address of exter memory range1 register
 */
typedef union {
    struct {
        /** access_exter_mem_end_addr1 : R/W; bitpos: [31:0]; default: 2415919103;
         *  The end address of accessible address space. The access address beyond this range
         *  would lead to descriptor error.
         */
        uint32_t access_exter_mem_end_addr1:32;
    };
    uint32_t val;
} h264_dma_exter_mem_end_addr1_reg_t;


/** Group: out arb config register */
/** Type of out_arb_config register
 *  reserved
 */
typedef union {
    struct {
        /** out_arb_timeout_num : R/W; bitpos: [15:0]; default: 0;
         *  Set the max number of timeout count of arbiter
         */
        uint32_t out_arb_timeout_num:16;
        /** out_weight_en : R/W; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t out_weight_en:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} h264_dma_out_arb_config_reg_t;


/** Group: in arb config register */
/** Type of in_arb_config register
 *  reserved
 */
typedef union {
    struct {
        /** in_arb_timeout_num : R/W; bitpos: [15:0]; default: 0;
         *  Set the max number of timeout count of arbiter
         */
        uint32_t in_arb_timeout_num:16;
        /** in_weight_en : R/W; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t in_weight_en:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} h264_dma_in_arb_config_reg_t;


/** Group: date register */
/** Type of date register
 *  reserved
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 539165699;
         *  register version.
         */
        uint32_t date:32;
    };
    uint32_t val;
} h264_dma_date_reg_t;


/** Group: counter rst register */
/** Type of counter_rst register
 *  counter reset register
 */
typedef union {
    struct {
        /** rx_ch0_exter_counter_rst : R/W; bitpos: [0]; default: 0;
         *  Write 1 then write 0 to this bit to reset rx ch0 counter.
         */
        uint32_t rx_ch0_exter_counter_rst:1;
        /** rx_ch1_exter_counter_rst : R/W; bitpos: [1]; default: 0;
         *  Write 1 then write 0 to this bit to reset rx ch1 counter.
         */
        uint32_t rx_ch1_exter_counter_rst:1;
        /** rx_ch2_inter_counter_rst : R/W; bitpos: [2]; default: 0;
         *  Write 1 then write 0 to this bit to reset rx ch2 counter.
         */
        uint32_t rx_ch2_inter_counter_rst:1;
        /** rx_ch5_inter_counter_rst : R/W; bitpos: [3]; default: 0;
         *  Write 1 then write 0 to this bit to reset rx ch5 counter.
         */
        uint32_t rx_ch5_inter_counter_rst:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} h264_dma_counter_rst_reg_t;


/** Group: counter register */
/** Type of rx_ch0_counter register
 *  rx ch0 counter register
 */
typedef union {
    struct {
        /** rx_ch0_cnt : RO; bitpos: [22:0]; default: 0;
         *  rx ch0 counter register
         */
        uint32_t rx_ch0_cnt:23;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} h264_dma_rx_ch0_counter_reg_t;

/** Type of rx_ch1_counter register
 *  rx ch1 counter register
 */
typedef union {
    struct {
        /** rx_ch1_cnt : RO; bitpos: [20:0]; default: 0;
         *  rx ch1 counter register
         */
        uint32_t rx_ch1_cnt:21;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} h264_dma_rx_ch1_counter_reg_t;

/** Type of rx_ch2_counter register
 *  rx ch2 counter register
 */
typedef union {
    struct {
        /** rx_ch2_cnt : RO; bitpos: [10:0]; default: 0;
         *  rx ch2 counter register
         */
        uint32_t rx_ch2_cnt:11;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} h264_dma_rx_ch2_counter_reg_t;

/** Type of rx_ch5_counter register
 *  rx ch5 counter register
 */
typedef union {
    struct {
        /** rx_ch5_cnt : RO; bitpos: [16:0]; default: 0;
         *  rx ch5 counter register
         */
        uint32_t rx_ch5_cnt:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} h264_dma_rx_ch5_counter_reg_t;


typedef struct {
    volatile h264_dma_out_conf0_ch0_reg_t out_conf0_ch0;
    volatile h264_dma_out_int_raw_ch0_reg_t out_int_raw_ch0;
    volatile h264_dma_out_int_ena_ch0_reg_t out_int_ena_ch0;
    volatile h264_dma_out_int_st_ch0_reg_t out_int_st_ch0;
    volatile h264_dma_out_int_clr_ch0_reg_t out_int_clr_ch0;
    volatile h264_dma_outfifo_status_ch0_reg_t outfifo_status_ch0;
    volatile h264_dma_out_push_ch0_reg_t out_push_ch0;
    volatile h264_dma_out_link_conf_ch0_reg_t out_link_conf_ch0;
    volatile h264_dma_out_link_addr_ch0_reg_t out_link_addr_ch0;
    volatile h264_dma_out_state_ch0_reg_t out_state_ch0;
    volatile h264_dma_out_eof_des_addr_ch0_reg_t out_eof_des_addr_ch0;
    volatile h264_dma_out_dscr_ch0_reg_t out_dscr_ch0;
    volatile h264_dma_out_dscr_bf0_ch0_reg_t out_dscr_bf0_ch0;
    volatile h264_dma_out_dscr_bf1_ch0_reg_t out_dscr_bf1_ch0;
    uint32_t reserved_038;
    volatile h264_dma_out_arb_ch0_reg_t out_arb_ch0;
    volatile h264_dma_out_ro_status_ch0_reg_t out_ro_status_ch0;
    volatile h264_dma_out_ro_pd_conf_ch0_reg_t out_ro_pd_conf_ch0;
    uint32_t reserved_048[2];
    volatile h264_dma_out_mode_enable_ch0_reg_t out_mode_enable_ch0;
    volatile h264_dma_out_mode_yuv_ch0_reg_t out_mode_yuv_ch0;
    uint32_t reserved_058[4];
    volatile h264_dma_out_etm_conf_ch0_reg_t out_etm_conf_ch0;
    uint32_t reserved_06c;
    volatile h264_dma_out_buf_len_ch0_reg_t out_buf_len_ch0;
    volatile h264_dma_out_fifo_bcnt_ch0_reg_t out_fifo_bcnt_ch0;
    volatile h264_dma_out_push_bytecnt_ch0_reg_t out_push_bytecnt_ch0;
    volatile h264_dma_out_xaddr_ch0_reg_t out_xaddr_ch0;
    uint32_t reserved_080[32];
    volatile h264_dma_out_conf0_ch1_reg_t out_conf0_ch1;
    volatile h264_dma_out_int_raw_ch1_reg_t out_int_raw_ch1;
    volatile h264_dma_out_int_ena_ch1_reg_t out_int_ena_ch1;
    volatile h264_dma_out_int_st_ch1_reg_t out_int_st_ch1;
    volatile h264_dma_out_int_clr_ch1_reg_t out_int_clr_ch1;
    volatile h264_dma_outfifo_status_ch1_reg_t outfifo_status_ch1;
    volatile h264_dma_out_push_ch1_reg_t out_push_ch1;
    volatile h264_dma_out_link_conf_ch1_reg_t out_link_conf_ch1;
    volatile h264_dma_out_link_addr_ch1_reg_t out_link_addr_ch1;
    volatile h264_dma_out_state_ch1_reg_t out_state_ch1;
    volatile h264_dma_out_eof_des_addr_ch1_reg_t out_eof_des_addr_ch1;
    volatile h264_dma_out_dscr_ch1_reg_t out_dscr_ch1;
    volatile h264_dma_out_dscr_bf0_ch1_reg_t out_dscr_bf0_ch1;
    volatile h264_dma_out_dscr_bf1_ch1_reg_t out_dscr_bf1_ch1;
    uint32_t reserved_138;
    volatile h264_dma_out_arb_ch1_reg_t out_arb_ch1;
    uint32_t reserved_140[10];
    volatile h264_dma_out_etm_conf_ch1_reg_t out_etm_conf_ch1;
    uint32_t reserved_16c;
    volatile h264_dma_out_buf_len_ch1_reg_t out_buf_len_ch1;
    volatile h264_dma_out_fifo_bcnt_ch1_reg_t out_fifo_bcnt_ch1;
    volatile h264_dma_out_push_bytecnt_ch1_reg_t out_push_bytecnt_ch1;
    volatile h264_dma_out_xaddr_ch1_reg_t out_xaddr_ch1;
    uint32_t reserved_180[32];
    volatile h264_dma_out_conf0_ch2_reg_t out_conf0_ch2;
    volatile h264_dma_out_int_raw_ch2_reg_t out_int_raw_ch2;
    volatile h264_dma_out_int_ena_ch2_reg_t out_int_ena_ch2;
    volatile h264_dma_out_int_st_ch2_reg_t out_int_st_ch2;
    volatile h264_dma_out_int_clr_ch2_reg_t out_int_clr_ch2;
    volatile h264_dma_outfifo_status_ch2_reg_t outfifo_status_ch2;
    volatile h264_dma_out_push_ch2_reg_t out_push_ch2;
    volatile h264_dma_out_link_conf_ch2_reg_t out_link_conf_ch2;
    volatile h264_dma_out_link_addr_ch2_reg_t out_link_addr_ch2;
    volatile h264_dma_out_state_ch2_reg_t out_state_ch2;
    volatile h264_dma_out_eof_des_addr_ch2_reg_t out_eof_des_addr_ch2;
    volatile h264_dma_out_dscr_ch2_reg_t out_dscr_ch2;
    volatile h264_dma_out_dscr_bf0_ch2_reg_t out_dscr_bf0_ch2;
    volatile h264_dma_out_dscr_bf1_ch2_reg_t out_dscr_bf1_ch2;
    uint32_t reserved_238;
    volatile h264_dma_out_arb_ch2_reg_t out_arb_ch2;
    uint32_t reserved_240[10];
    volatile h264_dma_out_etm_conf_ch2_reg_t out_etm_conf_ch2;
    uint32_t reserved_26c;
    volatile h264_dma_out_buf_len_ch2_reg_t out_buf_len_ch2;
    volatile h264_dma_out_fifo_bcnt_ch2_reg_t out_fifo_bcnt_ch2;
    volatile h264_dma_out_push_bytecnt_ch2_reg_t out_push_bytecnt_ch2;
    volatile h264_dma_out_xaddr_ch2_reg_t out_xaddr_ch2;
    uint32_t reserved_280[32];
    volatile h264_dma_out_conf0_ch3_reg_t out_conf0_ch3;
    volatile h264_dma_out_int_raw_ch3_reg_t out_int_raw_ch3;
    volatile h264_dma_out_int_ena_ch3_reg_t out_int_ena_ch3;
    volatile h264_dma_out_int_st_ch3_reg_t out_int_st_ch3;
    volatile h264_dma_out_int_clr_ch3_reg_t out_int_clr_ch3;
    volatile h264_dma_outfifo_status_ch3_reg_t outfifo_status_ch3;
    volatile h264_dma_out_push_ch3_reg_t out_push_ch3;
    volatile h264_dma_out_link_conf_ch3_reg_t out_link_conf_ch3;
    volatile h264_dma_out_link_addr_ch3_reg_t out_link_addr_ch3;
    volatile h264_dma_out_state_ch3_reg_t out_state_ch3;
    volatile h264_dma_out_eof_des_addr_ch3_reg_t out_eof_des_addr_ch3;
    volatile h264_dma_out_dscr_ch3_reg_t out_dscr_ch3;
    volatile h264_dma_out_dscr_bf0_ch3_reg_t out_dscr_bf0_ch3;
    volatile h264_dma_out_dscr_bf1_ch3_reg_t out_dscr_bf1_ch3;
    uint32_t reserved_338;
    volatile h264_dma_out_arb_ch3_reg_t out_arb_ch3;
    uint32_t reserved_340[10];
    volatile h264_dma_out_etm_conf_ch3_reg_t out_etm_conf_ch3;
    uint32_t reserved_36c;
    volatile h264_dma_out_buf_len_ch3_reg_t out_buf_len_ch3;
    volatile h264_dma_out_fifo_bcnt_ch3_reg_t out_fifo_bcnt_ch3;
    volatile h264_dma_out_push_bytecnt_ch3_reg_t out_push_bytecnt_ch3;
    volatile h264_dma_out_xaddr_ch3_reg_t out_xaddr_ch3;
    volatile h264_dma_out_block_buf_len_ch3_reg_t out_block_buf_len_ch3;
    uint32_t reserved_384[31];
    volatile h264_dma_out_conf0_ch4_reg_t out_conf0_ch4;
    volatile h264_dma_out_int_raw_ch4_reg_t out_int_raw_ch4;
    volatile h264_dma_out_int_ena_ch4_reg_t out_int_ena_ch4;
    volatile h264_dma_out_int_st_ch4_reg_t out_int_st_ch4;
    volatile h264_dma_out_int_clr_ch4_reg_t out_int_clr_ch4;
    volatile h264_dma_outfifo_status_ch4_reg_t outfifo_status_ch4;
    volatile h264_dma_out_push_ch4_reg_t out_push_ch4;
    volatile h264_dma_out_link_conf_ch4_reg_t out_link_conf_ch4;
    volatile h264_dma_out_link_addr_ch4_reg_t out_link_addr_ch4;
    volatile h264_dma_out_state_ch4_reg_t out_state_ch4;
    volatile h264_dma_out_eof_des_addr_ch4_reg_t out_eof_des_addr_ch4;
    volatile h264_dma_out_dscr_ch4_reg_t out_dscr_ch4;
    volatile h264_dma_out_dscr_bf0_ch4_reg_t out_dscr_bf0_ch4;
    volatile h264_dma_out_dscr_bf1_ch4_reg_t out_dscr_bf1_ch4;
    uint32_t reserved_438;
    volatile h264_dma_out_arb_ch4_reg_t out_arb_ch4;
    uint32_t reserved_440[10];
    volatile h264_dma_out_etm_conf_ch4_reg_t out_etm_conf_ch4;
    uint32_t reserved_46c;
    volatile h264_dma_out_buf_len_ch4_reg_t out_buf_len_ch4;
    volatile h264_dma_out_fifo_bcnt_ch4_reg_t out_fifo_bcnt_ch4;
    volatile h264_dma_out_push_bytecnt_ch4_reg_t out_push_bytecnt_ch4;
    volatile h264_dma_out_xaddr_ch4_reg_t out_xaddr_ch4;
    volatile h264_dma_out_block_buf_len_ch4_reg_t out_block_buf_len_ch4;
    uint32_t reserved_484[31];
    volatile h264_dma_in_conf0_ch0_reg_t in_conf0_ch0;
    volatile h264_dma_in_int_raw_ch0_reg_t in_int_raw_ch0;
    volatile h264_dma_in_int_ena_ch0_reg_t in_int_ena_ch0;
    volatile h264_dma_in_int_st_ch0_reg_t in_int_st_ch0;
    volatile h264_dma_in_int_clr_ch0_reg_t in_int_clr_ch0;
    volatile h264_dma_infifo_status_ch0_reg_t infifo_status_ch0;
    volatile h264_dma_in_pop_ch0_reg_t in_pop_ch0;
    volatile h264_dma_in_link_conf_ch0_reg_t in_link_conf_ch0;
    volatile h264_dma_in_link_addr_ch0_reg_t in_link_addr_ch0;
    volatile h264_dma_in_state_ch0_reg_t in_state_ch0;
    volatile h264_dma_in_suc_eof_des_addr_ch0_reg_t in_suc_eof_des_addr_ch0;
    volatile h264_dma_in_err_eof_des_addr_ch0_reg_t in_err_eof_des_addr_ch0;
    volatile h264_dma_in_dscr_ch0_reg_t in_dscr_ch0;
    volatile h264_dma_in_dscr_bf0_ch0_reg_t in_dscr_bf0_ch0;
    volatile h264_dma_in_dscr_bf1_ch0_reg_t in_dscr_bf1_ch0;
    uint32_t reserved_53c;
    volatile h264_dma_in_arb_ch0_reg_t in_arb_ch0;
    uint32_t reserved_544;
    volatile h264_dma_in_ro_pd_conf_ch0_reg_t in_ro_pd_conf_ch0;
    uint32_t reserved_54c[8];
    volatile h264_dma_in_etm_conf_ch0_reg_t in_etm_conf_ch0;
    uint32_t reserved_570[4];
    volatile h264_dma_in_fifo_cnt_ch0_reg_t in_fifo_cnt_ch0;
    volatile h264_dma_in_pop_data_cnt_ch0_reg_t in_pop_data_cnt_ch0;
    volatile h264_dma_in_xaddr_ch0_reg_t in_xaddr_ch0;
    volatile h264_dma_in_buf_hb_rcv_ch0_reg_t in_buf_hb_rcv_ch0;
    uint32_t reserved_590[28];
    volatile h264_dma_in_conf0_ch1_reg_t in_conf0_ch1;
    volatile h264_dma_in_int_raw_ch1_reg_t in_int_raw_ch1;
    volatile h264_dma_in_int_ena_ch1_reg_t in_int_ena_ch1;
    volatile h264_dma_in_int_st_ch1_reg_t in_int_st_ch1;
    volatile h264_dma_in_int_clr_ch1_reg_t in_int_clr_ch1;
    volatile h264_dma_infifo_status_ch1_reg_t infifo_status_ch1;
    volatile h264_dma_in_pop_ch1_reg_t in_pop_ch1;
    volatile h264_dma_in_link_conf_ch1_reg_t in_link_conf_ch1;
    volatile h264_dma_in_link_addr_ch1_reg_t in_link_addr_ch1;
    volatile h264_dma_in_state_ch1_reg_t in_state_ch1;
    volatile h264_dma_in_suc_eof_des_addr_ch1_reg_t in_suc_eof_des_addr_ch1;
    volatile h264_dma_in_err_eof_des_addr_ch1_reg_t in_err_eof_des_addr_ch1;
    volatile h264_dma_in_dscr_ch1_reg_t in_dscr_ch1;
    volatile h264_dma_in_dscr_bf0_ch1_reg_t in_dscr_bf0_ch1;
    volatile h264_dma_in_dscr_bf1_ch1_reg_t in_dscr_bf1_ch1;
    uint32_t reserved_63c;
    volatile h264_dma_in_arb_ch1_reg_t in_arb_ch1;
    uint32_t reserved_644;
    volatile h264_dma_in_etm_conf_ch1_reg_t in_etm_conf_ch1;
    uint32_t reserved_64c[13];
    volatile h264_dma_in_fifo_cnt_ch1_reg_t in_fifo_cnt_ch1;
    volatile h264_dma_in_pop_data_cnt_ch1_reg_t in_pop_data_cnt_ch1;
    volatile h264_dma_in_xaddr_ch1_reg_t in_xaddr_ch1;
    volatile h264_dma_in_buf_hb_rcv_ch1_reg_t in_buf_hb_rcv_ch1;
    uint32_t reserved_690[28];
    volatile h264_dma_in_conf0_ch2_reg_t in_conf0_ch2;
    volatile h264_dma_in_int_raw_ch2_reg_t in_int_raw_ch2;
    volatile h264_dma_in_int_ena_ch2_reg_t in_int_ena_ch2;
    volatile h264_dma_in_int_st_ch2_reg_t in_int_st_ch2;
    volatile h264_dma_in_int_clr_ch2_reg_t in_int_clr_ch2;
    volatile h264_dma_infifo_status_ch2_reg_t infifo_status_ch2;
    volatile h264_dma_in_pop_ch2_reg_t in_pop_ch2;
    volatile h264_dma_in_link_conf_ch2_reg_t in_link_conf_ch2;
    volatile h264_dma_in_link_addr_ch2_reg_t in_link_addr_ch2;
    volatile h264_dma_in_state_ch2_reg_t in_state_ch2;
    volatile h264_dma_in_suc_eof_des_addr_ch2_reg_t in_suc_eof_des_addr_ch2;
    volatile h264_dma_in_err_eof_des_addr_ch2_reg_t in_err_eof_des_addr_ch2;
    volatile h264_dma_in_dscr_ch2_reg_t in_dscr_ch2;
    volatile h264_dma_in_dscr_bf0_ch2_reg_t in_dscr_bf0_ch2;
    volatile h264_dma_in_dscr_bf1_ch2_reg_t in_dscr_bf1_ch2;
    uint32_t reserved_73c;
    volatile h264_dma_in_arb_ch2_reg_t in_arb_ch2;
    uint32_t reserved_744;
    volatile h264_dma_in_etm_conf_ch2_reg_t in_etm_conf_ch2;
    uint32_t reserved_74c[13];
    volatile h264_dma_in_fifo_cnt_ch2_reg_t in_fifo_cnt_ch2;
    volatile h264_dma_in_pop_data_cnt_ch2_reg_t in_pop_data_cnt_ch2;
    volatile h264_dma_in_xaddr_ch2_reg_t in_xaddr_ch2;
    volatile h264_dma_in_buf_hb_rcv_ch2_reg_t in_buf_hb_rcv_ch2;
    uint32_t reserved_790[28];
    volatile h264_dma_in_conf0_ch3_reg_t in_conf0_ch3;
    volatile h264_dma_in_int_raw_ch3_reg_t in_int_raw_ch3;
    volatile h264_dma_in_int_ena_ch3_reg_t in_int_ena_ch3;
    volatile h264_dma_in_int_st_ch3_reg_t in_int_st_ch3;
    volatile h264_dma_in_int_clr_ch3_reg_t in_int_clr_ch3;
    volatile h264_dma_infifo_status_ch3_reg_t infifo_status_ch3;
    volatile h264_dma_in_pop_ch3_reg_t in_pop_ch3;
    volatile h264_dma_in_link_conf_ch3_reg_t in_link_conf_ch3;
    volatile h264_dma_in_link_addr_ch3_reg_t in_link_addr_ch3;
    volatile h264_dma_in_state_ch3_reg_t in_state_ch3;
    volatile h264_dma_in_suc_eof_des_addr_ch3_reg_t in_suc_eof_des_addr_ch3;
    volatile h264_dma_in_err_eof_des_addr_ch3_reg_t in_err_eof_des_addr_ch3;
    volatile h264_dma_in_dscr_ch3_reg_t in_dscr_ch3;
    volatile h264_dma_in_dscr_bf0_ch3_reg_t in_dscr_bf0_ch3;
    volatile h264_dma_in_dscr_bf1_ch3_reg_t in_dscr_bf1_ch3;
    uint32_t reserved_83c;
    volatile h264_dma_in_arb_ch3_reg_t in_arb_ch3;
    uint32_t reserved_844;
    volatile h264_dma_in_etm_conf_ch3_reg_t in_etm_conf_ch3;
    uint32_t reserved_84c[13];
    volatile h264_dma_in_fifo_cnt_ch3_reg_t in_fifo_cnt_ch3;
    volatile h264_dma_in_pop_data_cnt_ch3_reg_t in_pop_data_cnt_ch3;
    volatile h264_dma_in_xaddr_ch3_reg_t in_xaddr_ch3;
    volatile h264_dma_in_buf_hb_rcv_ch3_reg_t in_buf_hb_rcv_ch3;
    uint32_t reserved_890[28];
    volatile h264_dma_in_conf0_ch4_reg_t in_conf0_ch4;
    volatile h264_dma_in_int_raw_ch4_reg_t in_int_raw_ch4;
    volatile h264_dma_in_int_ena_ch4_reg_t in_int_ena_ch4;
    volatile h264_dma_in_int_st_ch4_reg_t in_int_st_ch4;
    volatile h264_dma_in_int_clr_ch4_reg_t in_int_clr_ch4;
    volatile h264_dma_infifo_status_ch4_reg_t infifo_status_ch4;
    volatile h264_dma_in_pop_ch4_reg_t in_pop_ch4;
    volatile h264_dma_in_link_conf_ch4_reg_t in_link_conf_ch4;
    volatile h264_dma_in_link_addr_ch4_reg_t in_link_addr_ch4;
    volatile h264_dma_in_state_ch4_reg_t in_state_ch4;
    volatile h264_dma_in_suc_eof_des_addr_ch4_reg_t in_suc_eof_des_addr_ch4;
    volatile h264_dma_in_err_eof_des_addr_ch4_reg_t in_err_eof_des_addr_ch4;
    volatile h264_dma_in_dscr_ch4_reg_t in_dscr_ch4;
    volatile h264_dma_in_dscr_bf0_ch4_reg_t in_dscr_bf0_ch4;
    volatile h264_dma_in_dscr_bf1_ch4_reg_t in_dscr_bf1_ch4;
    uint32_t reserved_93c;
    volatile h264_dma_in_arb_ch4_reg_t in_arb_ch4;
    uint32_t reserved_944;
    volatile h264_dma_in_etm_conf_ch4_reg_t in_etm_conf_ch4;
    uint32_t reserved_94c[13];
    volatile h264_dma_in_fifo_cnt_ch4_reg_t in_fifo_cnt_ch4;
    volatile h264_dma_in_pop_data_cnt_ch4_reg_t in_pop_data_cnt_ch4;
    volatile h264_dma_in_xaddr_ch4_reg_t in_xaddr_ch4;
    volatile h264_dma_in_buf_hb_rcv_ch4_reg_t in_buf_hb_rcv_ch4;
    uint32_t reserved_990[28];
    volatile h264_dma_in_conf0_ch5_reg_t in_conf0_ch5;
    volatile h264_dma_in_conf1_ch5_reg_t in_conf1_ch5;
    volatile h264_dma_in_conf2_ch5_reg_t in_conf2_ch5;
    volatile h264_dma_in_conf3_ch5_reg_t in_conf3_ch5;
    volatile h264_dma_in_int_raw_ch5_reg_t in_int_raw_ch5;
    volatile h264_dma_in_int_ena_ch5_reg_t in_int_ena_ch5;
    volatile h264_dma_in_int_st_ch5_reg_t in_int_st_ch5;
    volatile h264_dma_in_int_clr_ch5_reg_t in_int_clr_ch5;
    volatile h264_dma_infifo_status_ch5_reg_t infifo_status_ch5;
    volatile h264_dma_in_pop_ch5_reg_t in_pop_ch5;
    volatile h264_dma_in_state_ch5_reg_t in_state_ch5;
    uint32_t reserved_a2c[5];
    volatile h264_dma_in_arb_ch5_reg_t in_arb_ch5;
    uint32_t reserved_a44[15];
    volatile h264_dma_in_fifo_cnt_ch5_reg_t in_fifo_cnt_ch5;
    volatile h264_dma_in_pop_data_cnt_ch5_reg_t in_pop_data_cnt_ch5;
    volatile h264_dma_in_xaddr_ch5_reg_t in_xaddr_ch5;
    volatile h264_dma_in_buf_hb_rcv_ch5_reg_t in_buf_hb_rcv_ch5;
    uint32_t reserved_a90[28];
    volatile h264_dma_inter_axi_err_reg_t inter_axi_err;
    volatile h264_dma_exter_axi_err_reg_t exter_axi_err;
    volatile h264_dma_rst_conf_reg_t rst_conf;
    volatile h264_dma_inter_mem_start_addr0_reg_t inter_mem_start_addr0;
    volatile h264_dma_inter_mem_end_addr0_reg_t inter_mem_end_addr0;
    volatile h264_dma_inter_mem_start_addr1_reg_t inter_mem_start_addr1;
    volatile h264_dma_inter_mem_end_addr1_reg_t inter_mem_end_addr1;
    uint32_t reserved_b1c;
    volatile h264_dma_exter_mem_start_addr0_reg_t exter_mem_start_addr0;
    volatile h264_dma_exter_mem_end_addr0_reg_t exter_mem_end_addr0;
    volatile h264_dma_exter_mem_start_addr1_reg_t exter_mem_start_addr1;
    volatile h264_dma_exter_mem_end_addr1_reg_t exter_mem_end_addr1;
    volatile h264_dma_out_arb_config_reg_t out_arb_config;
    volatile h264_dma_in_arb_config_reg_t in_arb_config;
    uint32_t reserved_b38;
    volatile h264_dma_date_reg_t date;
    uint32_t reserved_b40[4];
    volatile h264_dma_counter_rst_reg_t counter_rst;
    volatile h264_dma_rx_ch0_counter_reg_t rx_ch0_counter;
    volatile h264_dma_rx_ch1_counter_reg_t rx_ch1_counter;
    volatile h264_dma_rx_ch2_counter_reg_t rx_ch2_counter;
    volatile h264_dma_rx_ch5_counter_reg_t rx_ch5_counter;
} h264_dma_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(h264_dma_dev_t) == 0xb64, "Invalid size of h264_dma_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
