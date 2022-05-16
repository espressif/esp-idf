/**
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Group: FIFO R/W registers */
/** Type of chndata register
 *  Read and write data for channel n via APB FIFO
 */
typedef union {
    struct {
        /** chn_data : RO; bitpos: [31:0]; default: 0;
         *  This register is used to read and write data for channel n via APB FIFO.
         */
        uint32_t chn_data: 32;
    };
    uint32_t val;
} rmt_chndata_reg_t;


/** Group: Configuration registers */
/** Type of chnconf0 register
 *  Channel n configuration register 0
 */
typedef union {
    struct {
        /** div_cnt : R/W; bitpos: [7:0]; default: 2;
         *  This field is used to configure clock divider for channel n.
         */
        uint32_t div_cnt: 8;
        /** idle_thres : R/W; bitpos: [23:8]; default: 4096;
         *  Receiving ends when no edge is detected on input signals for continuous clock
         *  cycles longer than this field value.
         */
        uint32_t idle_thres: 16;
        /** mem_size : R/W; bitpos: [26:24]; default: 1;
         *  This field is used to configure the maximum blocks allocated to channel n. The
         *  valid range is from 1 ~ 4-n.
         */
        uint32_t mem_size: 3;
        /** carrier_eff_en : R/W; bitpos: [27]; default: 1;
         *  1: Add carrier modulation on output signals only at data sending state for channel
         *  n. 0: Add carrier modulation on signals at all states for channel n. States here
         *  include idle state (ST_IDLE), reading data from RAM (ST_RD_MEM), and sending data
         *  stored in RAM (ST_SEND). Only valid when RMT_CARRIER_EN_CHn is set to 1.
         */
        uint32_t carrier_eff_en: 1;
        /** carrier_en : R/W; bitpos: [28]; default: 1;
         *  This bit is used to enable carrier modulation for channel n. 1: Add carrier
         *  modulation on output signals. 0: No carrier modulation is added on output signals.
         */
        uint32_t carrier_en: 1;
        /** carrier_out_lv : R/W; bitpos: [29]; default: 1;
         *  This bit is used to configure the position of carrier wave for channel n.
         *
         *  1'h0: Add carrier wave on low-level output signals.
         *
         *  1'h1: Add carrier wave on high-level output signals.
         */
        uint32_t carrier_out_lv: 1;
        uint32_t reserved_30: 2;
    };
    uint32_t val;
} rmt_chnconf0_reg_t;

/** Type of chnconf1 register
 *  Channel n configuration register 1
 */
typedef union {
    struct {
        /** tx_start : R/W; bitpos: [0]; default: 0;
         *  Set this bit to start sending data on channel n.
         */
        uint32_t tx_start: 1;
        /** rx_en : R/W; bitpos: [1]; default: 0;
         *  Set this bit to enable receiver to receive data on channel n.
         */
        uint32_t rx_en: 1;
        /** mem_wr_rst : WO; bitpos: [2]; default: 0;
         *  Set this bit to reset RAM write address accessed by the receiver for channel n.
         */
        uint32_t mem_wr_rst: 1;
        /** mem_rd_rst : WO; bitpos: [3]; default: 0;
         *  Set this bit to reset RAM read address accessed by the transmitter for channel n.
         */
        uint32_t mem_rd_rst: 1;
        /** apb_mem_rst : WO; bitpos: [4]; default: 0;
         *  Set this bit to reset W/R ram address for channel n by accessing apb fifo.
         */
        uint32_t apb_mem_rst: 1;
        /** mem_owner : R/W; bitpos: [5]; default: 1;
         *  This bit marks the ownership of channel n's RAM block.
         *
         *  1'h1: Receiver is using the RAM.
         *
         *  1'h0: Transmitter is using the RAM.
         */
        uint32_t mem_owner: 1;
        /** tx_conti_mode : R/W; bitpos: [6]; default: 0;
         *  Set this bit to restart transmission in continuous node from the first data in
         *  channel n.
         */
        uint32_t tx_conti_mode: 1;
        /** rx_filter_en : R/W; bitpos: [7]; default: 0;
         *  Set this bit to enable the receiver's filter for channel n.
         */
        uint32_t rx_filter_en: 1;
        /** rx_filter_thres : R/W; bitpos: [15:8]; default: 15;
         *  Set this field to ignore the input pulse when its width is less than
         *  RMT_RX_FILTER_THRES_CHn APB clock cycles in receive mode.
         */
        uint32_t rx_filter_thres: 8;
        /** chk_rx_carrier_en : R/W; bitpos: [16]; default: 0;
         *  Set this bit to enable memory loop read mode when carrier modulation is enabled for
         *  channel n.
         */
        uint32_t chk_rx_carrier_en: 1;
        /** ref_always_on : R/W; bitpos: [17]; default: 0;
         *  Set this bit to select a base clock for channel n.
         *
         *  1'h1: APB_CLK    1'h0: REF_TICK
         */
        uint32_t ref_always_on: 1;
        /** idle_out_lv : R/W; bitpos: [18]; default: 0;
         *  This bit configures the level of output signals in channel n when the transmitter
         *  is in idle state.
         */
        uint32_t idle_out_lv: 1;
        /** idle_out_en : R/W; bitpos: [19]; default: 0;
         *  This is the output enable bit for channel n in idle state.
         */
        uint32_t idle_out_en: 1;
        /** tx_stop : R/W; bitpos: [20]; default: 0;
         *  Set this bit to stop the transmitter of channel n sending data out.
         */
        uint32_t tx_stop: 1;
        uint32_t reserved_21: 11;
    };
    uint32_t val;
} rmt_chnconf1_reg_t;

/** Type of apb_conf register
 *  RMT APB configuration register
 */
typedef union {
    struct {
        /** apb_fifo_mask : R/W; bitpos: [0]; default: 0;
         *  1'h1: Access memory directly.  1'h0: access memory via APB FIFO.
         */
        uint32_t apb_fifo_mask: 1;
        /** mem_tx_wrap_en : R/W; bitpos: [1]; default: 0;
         *  Set this bit to enable wrap mode.
         */
        uint32_t mem_tx_wrap_en: 1;
        /** mem_clk_force_on : R/W; bitpos: [2]; default: 1;
         *  Set this bit to enable the clock for RAM when RMT module starts working, disable
         *  this clock when RMT stops working, to achieve low-power scheme.
         */
        uint32_t mem_clk_force_on: 1;
        /** mem_force_pd : R/W; bitpos: [3]; default: 0;
         *  Set this bit to power down RMT memory.
         */
        uint32_t mem_force_pd: 1;
        /** mem_force_pu : R/W; bitpos: [4]; default: 0;
         *  1: Disable RAM's Light-sleep power down function. 0: power down RMT RAM when RMT is
         *  in Light-sleep mode.
         */
        uint32_t mem_force_pu: 1;
        uint32_t reserved_5: 26;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  Clock gating enable bit for RMT registers to achieve low-power scheme. 1: Power up
         *  drive clock for RMT registers. 0: Power down drive clock for RMT registers.
         */
        uint32_t clk_en: 1;
    };
    uint32_t val;
} rmt_apb_conf_reg_t;

/** Type of ref_cnt_rst register
 *  RMT clock divider reset register
 */
typedef union {
    struct {
        /** ref_cnt_rst_ch0 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to reset the clock divider of channel 0.
         */
        uint32_t ref_cnt_rst_ch0: 1;
        /** ref_cnt_rst_ch1 : R/W; bitpos: [1]; default: 0;
         *  This bit is used to reset the clock divider of channel 1.
         */
        uint32_t ref_cnt_rst_ch1: 1;
        /** ref_cnt_rst_ch2 : R/W; bitpos: [2]; default: 0;
         *  This bit is used to reset the clock divider of channel 2.
         */
        uint32_t ref_cnt_rst_ch2: 1;
        /** ref_cnt_rst_ch3 : R/W; bitpos: [3]; default: 0;
         *  This bit is used to reset the clock divider of channel 3.
         */
        uint32_t ref_cnt_rst_ch3: 1;
        uint32_t reserved_4: 28;
    };
    uint32_t val;
} rmt_ref_cnt_rst_reg_t;

/** Type of chn_rx_carrier_rm register
 *  Channel n carrier remove register
 */
typedef union {
    struct {
        /** carrier_low_thres_ch : R/W; bitpos: [15:0]; default: 0;
         *  The low level period in carrier modulation mode is (RMT_CARRIER_LOW_THRES_CHn + 1)
         *  clock cycles for channel n.
         */
        uint32_t carrier_low_thres_ch: 16;
        /** carrier_high_thres_ch : R/W; bitpos: [31:16]; default: 0;
         *  The high level period in carrier modulation mode is (RMT_CARRIER_HIGH_THRES_CHn +
         *  1) clock cycles for channel n.
         */
        uint32_t carrier_high_thres_ch: 16;
    };
    uint32_t val;
} rmt_chn_rx_carrier_rm_reg_t;


/** Group: Status registers */
/** Type of chnstatus register
 *  Channel n status register
 */
typedef union {
    struct {
        /** mem_waddr_ex : RO; bitpos: [8:0]; default: 0;
         *  This field records the memory address offset when receiver of channel n is using
         *  the RAM.
         */
        uint32_t mem_waddr_ex: 9;
        uint32_t reserved_9: 1;
        /** mem_raddr_ex : RO; bitpos: [18:10]; default: 0;
         *  This field records the memory address offset when transmitter of channel n is using
         *  the RAM.
         */
        uint32_t mem_raddr_ex: 9;
        uint32_t reserved_19: 1;
        /** state : RO; bitpos: [22:20]; default: 0;
         *  This field records the FSM status of channel n.
         */
        uint32_t state: 3;
        /** mem_owner_err : RO; bitpos: [23]; default: 0;
         *  This status bit will be set when the ownership of memory block is violated.
         */
        uint32_t mem_owner_err: 1;
        /** mem_full : RO; bitpos: [24]; default: 0;
         *  This status bit will be set if the receiver receives more data than the memory
         *  allows.
         */
        uint32_t mem_full: 1;
        /** mem_empty : RO; bitpos: [25]; default: 0;
         *  This status bit will be set when the data to be sent is more than memory allows and
         *  the wrap mode is disabled.
         */
        uint32_t mem_empty: 1;
        /** apb_mem_wr_err : RO; bitpos: [26]; default: 0;
         *  This status bit will be set if the offset address out of memory size when writes
         *  RAM via APB bus.
         */
        uint32_t apb_mem_wr_err: 1;
        /** apb_mem_rd_err : RO; bitpos: [27]; default: 0;
         *  This status bit will be set if the offset address out of memory size when reads RAM
         *  via APB bus.
         */
        uint32_t apb_mem_rd_err: 1;
        uint32_t reserved_28: 4;
    };
    uint32_t val;
} rmt_chnstatus_reg_t;

/** Type of chnaddr register
 *  Channel n address register
 */
typedef union {
    struct {
        /** apb_mem_waddr_ch0 : RO; bitpos: [8:0]; default: 0;
         *  This field records the memory address offset when channel n writes RAM via APB bus.
         */
        uint32_t apb_mem_waddr_ch0: 9;
        uint32_t reserved_9: 1;
        /** apb_mem_raddr_ch0 : RO; bitpos: [18:10]; default: 0;
         *  This field records the memory address offset when channel n reads RAM via APB bus.
         */
        uint32_t apb_mem_raddr_ch0: 9;
        uint32_t reserved_19: 13;
    };
    uint32_t val;
} rmt_chnaddr_reg_t;


/** Group: Interrupt registers */
/** Type of int_raw register
 *  Raw interrupt status register
 */
typedef union {
    struct {
        /** ch0_tx_end_int_raw : RO; bitpos: [0]; default: 0;
         *  The interrupt raw bit for channel 0. Triggered when transmitting ends.
         */
        uint32_t ch0_tx_end_int_raw: 1;
        /** ch0_rx_end_int_raw : RO; bitpos: [1]; default: 0;
         *  The interrupt raw bit for channel 0. Triggered when receiving ends.
         */
        uint32_t ch0_rx_end_int_raw: 1;
        /** ch0_err_int_raw : RO; bitpos: [2]; default: 0;
         *  The interrupt raw bit for channel 0. Triggered when error occurs.
         */
        uint32_t ch0_err_int_raw: 1;
        /** ch1_tx_end_int_raw : RO; bitpos: [3]; default: 0;
         *  The interrupt raw bit for channel 1. Triggered when transmitting ends.
         */
        uint32_t ch1_tx_end_int_raw: 1;
        /** ch1_rx_end_int_raw : RO; bitpos: [4]; default: 0;
         *  The interrupt raw bit for channel 1. Triggered when receiving ends.
         */
        uint32_t ch1_rx_end_int_raw: 1;
        /** ch1_err_int_raw : RO; bitpos: [5]; default: 0;
         *  The interrupt raw bit for channel 1. Triggered when error occurs.
         */
        uint32_t ch1_err_int_raw: 1;
        /** ch2_tx_end_int_raw : RO; bitpos: [6]; default: 0;
         *  The interrupt raw bit for channel 2. Triggered when transmitting ends.
         */
        uint32_t ch2_tx_end_int_raw: 1;
        /** ch2_rx_end_int_raw : RO; bitpos: [7]; default: 0;
         *  The interrupt raw bit for channel 2. Triggered when receiving ends.
         */
        uint32_t ch2_rx_end_int_raw: 1;
        /** ch2_err_int_raw : RO; bitpos: [8]; default: 0;
         *  The interrupt raw bit for channel 2. Triggered when error occurs.
         */
        uint32_t ch2_err_int_raw: 1;
        /** ch3_tx_end_int_raw : RO; bitpos: [9]; default: 0;
         *  The interrupt raw bit for channel 3. Triggered when transmitting ends.
         */
        uint32_t ch3_tx_end_int_raw: 1;
        /** ch3_rx_end_int_raw : RO; bitpos: [10]; default: 0;
         *  The interrupt raw bit for channel 3. Triggered when receiving ends.
         */
        uint32_t ch3_rx_end_int_raw: 1;
        /** ch3_err_int_raw : RO; bitpos: [11]; default: 0;
         *  The interrupt raw bit for channel 3. Triggered when error occurs.
         */
        uint32_t ch3_err_int_raw: 1;
        /** ch0_tx_thr_event_int_raw : RO; bitpos: [12]; default: 0;
         *  The interrupt raw bit for channel 0. Triggered when transmitter sends more data
         *  than configured value.
         */
        uint32_t ch0_tx_thr_event_int_raw: 1;
        /** ch1_tx_thr_event_int_raw : RO; bitpos: [13]; default: 0;
         *  The interrupt raw bit for channel 1. Triggered when transmitter sends more data
         *  than configured value.
         */
        uint32_t ch1_tx_thr_event_int_raw: 1;
        /** ch2_tx_thr_event_int_raw : RO; bitpos: [14]; default: 0;
         *  The interrupt raw bit for channel 2. Triggered when transmitter sends more data
         *  than configured value.
         */
        uint32_t ch2_tx_thr_event_int_raw: 1;
        /** ch3_tx_thr_event_int_raw : RO; bitpos: [15]; default: 0;
         *  The interrupt raw bit for channel 3. Triggered when transmitter sends more data
         *  than configured value.
         */
        uint32_t ch3_tx_thr_event_int_raw: 1;
        /** ch0_tx_loop_int_raw : RO; bitpos: [16]; default: 0;
         *  The interrupt raw bit for channel 0. Triggered when loop counting reaches the
         *  configured threshold value.
         */
        uint32_t ch0_tx_loop_int_raw: 1;
        /** ch1_tx_loop_int_raw : RO; bitpos: [17]; default: 0;
         *  The interrupt raw bit for channel 1. Triggered when loop counting reaches the
         *  configured threshold value.
         */
        uint32_t ch1_tx_loop_int_raw: 1;
        /** ch2_tx_loop_int_raw : RO; bitpos: [18]; default: 0;
         *  The interrupt raw bit for channel 2. Triggered when loop counting reaches the
         *  configured threshold value.
         */
        uint32_t ch2_tx_loop_int_raw: 1;
        /** ch3_tx_loop_int_raw : RO; bitpos: [19]; default: 0;
         *  The interrupt raw bit for channel 3. Triggered when loop counting reaches the
         *  configured threshold value.
         */
        uint32_t ch3_tx_loop_int_raw: 1;
        uint32_t reserved_20: 12;
    };
    uint32_t val;
} rmt_int_raw_reg_t;

/** Type of int_st register
 *  Masked interrupt status register
 */
typedef union {
    struct {
        /** ch0_tx_end_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status bit for RMT_CH0_TX_END_INT.
         */
        uint32_t ch0_tx_end_int_st: 1;
        /** ch0_rx_end_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status bit for RMT_CH0_RX_END_INT.
         */
        uint32_t ch0_rx_end_int_st: 1;
        /** ch0_err_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status bit for RMT_CH0_ERR_INT.
         */
        uint32_t ch0_err_int_st: 1;
        /** ch1_tx_end_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status bit for RMT_CH1_TX_END_INT.
         */
        uint32_t ch1_tx_end_int_st: 1;
        /** ch1_rx_end_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status bit for RMT_CH1_RX_END_INT.
         */
        uint32_t ch1_rx_end_int_st: 1;
        /** ch1_err_int_st : RO; bitpos: [5]; default: 0;
         *  The masked interrupt status bit for RMT_CH1_ERR_INT.
         */
        uint32_t ch1_err_int_st: 1;
        /** ch2_tx_end_int_st : RO; bitpos: [6]; default: 0;
         *  The masked interrupt status bit for RMT_CH2_TX_END_INT.
         */
        uint32_t ch2_tx_end_int_st: 1;
        /** ch2_rx_end_int_st : RO; bitpos: [7]; default: 0;
         *  The masked interrupt status bit for RMT_CH2_RX_END_INT.
         */
        uint32_t ch2_rx_end_int_st: 1;
        /** ch2_err_int_st : RO; bitpos: [8]; default: 0;
         *  The masked interrupt status bit for RMT_CH2_ERR_INT.
         */
        uint32_t ch2_err_int_st: 1;
        /** ch3_tx_end_int_st : RO; bitpos: [9]; default: 0;
         *  The masked interrupt status bit for RMT_CH3_TX_END_INT.
         */
        uint32_t ch3_tx_end_int_st: 1;
        /** ch3_rx_end_int_st : RO; bitpos: [10]; default: 0;
         *  The masked interrupt status bit for RMT_CH3_RX_END_INT.
         */
        uint32_t ch3_rx_end_int_st: 1;
        /** ch3_err_int_st : RO; bitpos: [11]; default: 0;
         *  The masked interrupt status bit for RMT_CH3_ERR_INT.
         */
        uint32_t ch3_err_int_st: 1;
        /** ch0_tx_thr_event_int_st : RO; bitpos: [12]; default: 0;
         *  The masked interrupt status bit for RMT_CH0_TX_THR_EVENT_INT.
         */
        uint32_t ch0_tx_thr_event_int_st: 1;
        /** ch1_tx_thr_event_int_st : RO; bitpos: [13]; default: 0;
         *  The masked interrupt status bit for RMT_CH1_TX_THR_EVENT_INT.
         */
        uint32_t ch1_tx_thr_event_int_st: 1;
        /** ch2_tx_thr_event_int_st : RO; bitpos: [14]; default: 0;
         *  The masked interrupt status bit for RMT_CH2_TX_THR_EVENT_INT.
         */
        uint32_t ch2_tx_thr_event_int_st: 1;
        /** ch3_tx_thr_event_int_st : RO; bitpos: [15]; default: 0;
         *  The masked interrupt status bit for RMT_CH3_TX_THR_EVENT_INT.
         */
        uint32_t ch3_tx_thr_event_int_st: 1;
        /** ch0_tx_loop_int_st : RO; bitpos: [16]; default: 0;
         *  The masked interrupt status bit for RMT_CH0_TX_LOOP_INT.
         */
        uint32_t ch0_tx_loop_int_st: 1;
        /** ch1_tx_loop_int_st : RO; bitpos: [17]; default: 0;
         *  The masked interrupt status bit for RMT_CH1_TX_LOOP_INT.
         */
        uint32_t ch1_tx_loop_int_st: 1;
        /** ch2_tx_loop_int_st : RO; bitpos: [18]; default: 0;
         *  The masked interrupt status bit for RMT_CH2_TX_LOOP_INT.
         */
        uint32_t ch2_tx_loop_int_st: 1;
        /** ch3_tx_loop_int_st : RO; bitpos: [19]; default: 0;
         *  The masked interrupt status bit for RMT_CH3_TX_LOOP_INT.
         */
        uint32_t ch3_tx_loop_int_st: 1;
        uint32_t reserved_20: 12;
    };
    uint32_t val;
} rmt_int_st_reg_t;

/** Type of int_ena register
 *  Interrupt enable register
 */
typedef union {
    struct {
        /** ch0_tx_end_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enabled bit for RMT_CH0_TX_END_INT.
         */
        uint32_t ch0_tx_end_int_ena: 1;
        /** ch0_rx_end_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enabled bit for RMT_CH0_RX_END_INT.
         */
        uint32_t ch0_rx_end_int_ena: 1;
        /** ch0_err_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enabled bit for RMT_CH0_ERR_INT.
         */
        uint32_t ch0_err_int_ena: 1;
        /** ch1_tx_end_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enabled bit for RMT_CH1_TX_END_INT.
         */
        uint32_t ch1_tx_end_int_ena: 1;
        /** ch1_rx_end_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enabled bit for RMT_CH1_RX_END_INT.
         */
        uint32_t ch1_rx_end_int_ena: 1;
        /** ch1_err_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enabled bit for RMT_CH1_ERR_INT.
         */
        uint32_t ch1_err_int_ena: 1;
        /** ch2_tx_end_int_ena : R/W; bitpos: [6]; default: 0;
         *  The interrupt enabled bit for RMT_CH2_TX_END_INT.
         */
        uint32_t ch2_tx_end_int_ena: 1;
        /** ch2_rx_end_int_ena : R/W; bitpos: [7]; default: 0;
         *  The interrupt enabled bit for RMT_CH2_RX_END_INT.
         */
        uint32_t ch2_rx_end_int_ena: 1;
        /** ch2_err_int_ena : R/W; bitpos: [8]; default: 0;
         *  The interrupt enabled bit for RMT_CH2_ERR_INT.
         */
        uint32_t ch2_err_int_ena: 1;
        /** ch3_tx_end_int_ena : R/W; bitpos: [9]; default: 0;
         *  The interrupt enabled bit for RMT_CH3_TX_END_INT.
         */
        uint32_t ch3_tx_end_int_ena: 1;
        /** ch3_rx_end_int_ena : R/W; bitpos: [10]; default: 0;
         *  The interrupt enabled bit for RMT_CH3_RX_END_INT.
         */
        uint32_t ch3_rx_end_int_ena: 1;
        /** ch3_err_int_ena : R/W; bitpos: [11]; default: 0;
         *  The interrupt enabled bit for RMT_CH3_ERR_INT.
         */
        uint32_t ch3_err_int_ena: 1;
        /** ch0_tx_thr_event_int_ena : R/W; bitpos: [12]; default: 0;
         *  The interrupt enabled bit for RMT_CH0_TX_THR_EVENT_INT.
         */
        uint32_t ch0_tx_thr_event_int_ena: 1;
        /** ch1_tx_thr_event_int_ena : R/W; bitpos: [13]; default: 0;
         *  The interrupt enabled bit for RMT_CH1_TX_THR_EVENT_INT.
         */
        uint32_t ch1_tx_thr_event_int_ena: 1;
        /** ch2_tx_thr_event_int_ena : R/W; bitpos: [14]; default: 0;
         *  The interrupt enabled bit for RMT_CH2_TX_THR_EVENT_INT.
         */
        uint32_t ch2_tx_thr_event_int_ena: 1;
        /** ch3_tx_thr_event_int_ena : R/W; bitpos: [15]; default: 0;
         *  The interrupt enabled bit for RMT_CH3_TX_THR_EVENT_INT.
         */
        uint32_t ch3_tx_thr_event_int_ena: 1;
        /** ch0_tx_loop_int_ena : R/W; bitpos: [16]; default: 0;
         *  The interrupt enabled bit for RMT_CH0_TX_LOOP_INT.
         */
        uint32_t ch0_tx_loop_int_ena: 1;
        /** ch1_tx_loop_int_ena : R/W; bitpos: [17]; default: 0;
         *  The interrupt enabled bit for RMT_CH1_TX_LOOP_INT.
         */
        uint32_t ch1_tx_loop_int_ena: 1;
        /** ch2_tx_loop_int_ena : R/W; bitpos: [18]; default: 0;
         *  The interrupt enabled bit for RMT_CH2_TX_LOOP_INT.
         */
        uint32_t ch2_tx_loop_int_ena: 1;
        /** ch3_tx_loop_int_ena : R/W; bitpos: [19]; default: 0;
         *  The interrupt enabled bit for RMT_CH3_TX_LOOP_INT.
         */
        uint32_t ch3_tx_loop_int_ena: 1;
        uint32_t reserved_20: 12;
    };
    uint32_t val;
} rmt_int_ena_reg_t;

/** Type of int_clr register
 *  Interrupt clear register
 */
typedef union {
    struct {
        /** ch0_tx_end_int_clr : WO; bitpos: [0]; default: 0;
         *  Set this bit to clear RMT_CH0_TX_END_INT interrupt.
         */
        uint32_t ch0_tx_end_int_clr: 1;
        /** ch0_rx_end_int_clr : WO; bitpos: [1]; default: 0;
         *  Set this bit to clear RMT_CH0_RX_END_INT interrupt.
         */
        uint32_t ch0_rx_end_int_clr: 1;
        /** ch0_err_int_clr : WO; bitpos: [2]; default: 0;
         *  Set this bit to clear RMT_CH0_ERR_INT interrupt.
         */
        uint32_t ch0_err_int_clr: 1;
        /** ch1_tx_end_int_clr : WO; bitpos: [3]; default: 0;
         *  Set this bit to clear RMT_CH1_TX_END_INT interrupt.
         */
        uint32_t ch1_tx_end_int_clr: 1;
        /** ch1_rx_end_int_clr : WO; bitpos: [4]; default: 0;
         *  Set this bit to clear RMT_CH1_RX_END_INT interrupt.
         */
        uint32_t ch1_rx_end_int_clr: 1;
        /** ch1_err_int_clr : WO; bitpos: [5]; default: 0;
         *  Set this bit to clear RMT_CH1_ERR_INT interrupt.
         */
        uint32_t ch1_err_int_clr: 1;
        /** ch2_tx_end_int_clr : WO; bitpos: [6]; default: 0;
         *  Set this bit to clear RMT_CH2_TX_END_INT interrupt.
         */
        uint32_t ch2_tx_end_int_clr: 1;
        /** ch2_rx_end_int_clr : WO; bitpos: [7]; default: 0;
         *  Set this bit to clear RMT_CH2_RX_END_INT interrupt.
         */
        uint32_t ch2_rx_end_int_clr: 1;
        /** ch2_err_int_clr : WO; bitpos: [8]; default: 0;
         *  Set this bit to clear RMT_CH2_ERR_INT interrupt.
         */
        uint32_t ch2_err_int_clr: 1;
        /** ch3_tx_end_int_clr : WO; bitpos: [9]; default: 0;
         *  Set this bit to clear RMT_CH3_TX_END_INT interrupt.
         */
        uint32_t ch3_tx_end_int_clr: 1;
        /** ch3_rx_end_int_clr : WO; bitpos: [10]; default: 0;
         *  Set this bit to clear RMT_CH3_RX_END_INT interrupt.
         */
        uint32_t ch3_rx_end_int_clr: 1;
        /** ch3_err_int_clr : WO; bitpos: [11]; default: 0;
         *  Set this bit to clear RMT_CH3_ERR_INT interrupt.
         */
        uint32_t ch3_err_int_clr: 1;
        /** ch0_tx_thr_event_int_clr : WO; bitpos: [12]; default: 0;
         *  Set this bit to clear RMT_CH0_TX_THR_EVENT_INT interrupt.
         */
        uint32_t ch0_tx_thr_event_int_clr: 1;
        /** ch1_tx_thr_event_int_clr : WO; bitpos: [13]; default: 0;
         *  Set this bit to clear RMT_CH1_TX_THR_EVENT_INT interrupt.
         */
        uint32_t ch1_tx_thr_event_int_clr: 1;
        /** ch2_tx_thr_event_int_clr : WO; bitpos: [14]; default: 0;
         *  Set this bit to clear RMT_CH2_TX_THR_EVENT_INT interrupt.
         */
        uint32_t ch2_tx_thr_event_int_clr: 1;
        /** ch3_tx_thr_event_int_clr : WO; bitpos: [15]; default: 0;
         *  Set this bit to clear RMT_CH3_TX_THR_EVENT_INT interrupt.
         */
        uint32_t ch3_tx_thr_event_int_clr: 1;
        /** ch0_tx_loop_int_clr : WO; bitpos: [16]; default: 0;
         *  Set this bit to clear RMT_CH0_TX_LOOP_INT interrupt.
         */
        uint32_t ch0_tx_loop_int_clr: 1;
        /** ch1_tx_loop_int_clr : WO; bitpos: [17]; default: 0;
         *  Set this bit to clear RMT_CH1_TX_LOOP_INT interrupt.
         */
        uint32_t ch1_tx_loop_int_clr: 1;
        /** ch2_tx_loop_int_clr : WO; bitpos: [18]; default: 0;
         *  Set this bit to clear RMT_CH2_TX_LOOP_INT interrupt.
         */
        uint32_t ch2_tx_loop_int_clr: 1;
        /** ch3_tx_loop_int_clr : WO; bitpos: [19]; default: 0;
         *  Set this bit to clear RMT_CH3_TX_LOOP_INT interrupt.
         */
        uint32_t ch3_tx_loop_int_clr: 1;
        uint32_t reserved_20: 12;
    };
    uint32_t val;
} rmt_int_clr_reg_t;


/** Group: Carrier wave duty cycle registers */
/** Type of chncarrier_duty register
 *  Channel n duty cycle configuration register
 */
typedef union {
    struct {
        /** low : R/W; bitpos: [15:0]; default: 64;
         *  This field is used to configure the clock cycles of carrier wave at low level for
         *  channel n.
         */
        uint32_t low: 16;
        /** high : R/W; bitpos: [31:16]; default: 64;
         *  This field is used to configure the clock cycles of carrier wave at high level for
         *  channel n.
         */
        uint32_t high: 16;
    };
    uint32_t val;
} rmt_chncarrier_duty_reg_t;


/** Group: Tx event configuration registers */
/** Type of chn_tx_lim register
 *  Channel n Tx event configuration register
 */
typedef union {
    struct {
        /** tx_lim : R/W; bitpos: [8:0]; default: 128;
         *  This field is used to configure the maximum entries that channel n can send out.
         *  When RMT_MEM_SIZE_CHn = 1, this field can be set to any value among 0 ~ 128
         *  (64*32/16 = 128). When RMT_MEM_SIZE_CHn > 1, this field can be set to any value
         *  among (0 ~ 128)*RMT_MEM_SIZE_CHn.
         */
        uint32_t tx_lim: 9;
        /** tx_loop_num : R/W; bitpos: [18:9]; default: 0;
         *  This field is used to configure the maximum loop times when continuous transmission
         *  mode is enabled.
         */
        uint32_t tx_loop_num: 10;
        /** tx_loop_cnt_en : R/W; bitpos: [19]; default: 0;
         *  This bit is used to enable loop counting.
         */
        uint32_t tx_loop_cnt_en: 1;
        /** loop_count_reset : WO; bitpos: [20]; default: 0;
         *  This bit is used to reset loop counting when continuous transmission mode is valid.
         */
        uint32_t loop_count_reset: 1;
        uint32_t reserved_21: 11;
    };
    uint32_t val;
} rmt_chn_tx_lim_reg_t;

/** Type of tx_sim register
 *  Enable RMT simultaneous transmission
 */
typedef union {
    struct {
        /** tx_sim_ch0 : R/W; bitpos: [0]; default: 0;
         *  Set this bit to enable channel 0 to start sending data simultaneously with other
         *  enabled channels.
         */
        uint32_t tx_sim_ch0: 1;
        /** tx_sim_ch1 : R/W; bitpos: [1]; default: 0;
         *  Set this bit to enable channel 1 to start sending data simultaneously with other
         *  enabled channels.
         */
        uint32_t tx_sim_ch1: 1;
        /** tx_sim_ch2 : R/W; bitpos: [2]; default: 0;
         *  Set this bit to enable channel 2 to start sending data simultaneously with other
         *  enabled channels.
         */
        uint32_t tx_sim_ch2: 1;
        /** tx_sim_ch3 : R/W; bitpos: [3]; default: 0;
         *  Set this bit to enable channel 3 to start sending data simultaneously with other
         *  enabled channels.
         */
        uint32_t tx_sim_ch3: 1;
        /** tx_sim_en : R/W; bitpos: [4]; default: 0;
         *  This bit is used to enable multiple channels to start sending data simultaneously.
         */
        uint32_t en: 1;
        uint32_t reserved_5: 27;
    };
    uint32_t val;
} rmt_tx_sim_reg_t;


/** Group: Version register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 419898881;
         *  Version control register
         */
        uint32_t date: 32;
    };
    uint32_t val;
} rmt_date_reg_t;


typedef struct rmt_dev_t {
    volatile rmt_chndata_reg_t data_ch[4];
    volatile struct {
        volatile rmt_chnconf0_reg_t conf0;
        volatile rmt_chnconf1_reg_t conf1;
    } conf_ch[4];
    volatile rmt_chnstatus_reg_t status_ch[4];
    volatile rmt_chnaddr_reg_t apb_mem_addr_ch[4];
    volatile rmt_int_raw_reg_t int_raw;
    volatile rmt_int_st_reg_t int_st;
    volatile rmt_int_ena_reg_t int_ena;
    volatile rmt_int_clr_reg_t int_clr;
    volatile rmt_chncarrier_duty_reg_t carrier_duty_ch[4];
    volatile rmt_chn_tx_lim_reg_t tx_lim_ch[4];
    volatile rmt_apb_conf_reg_t apb_conf;
    volatile rmt_tx_sim_reg_t tx_sim;
    volatile rmt_ref_cnt_rst_reg_t ref_cnt_rst;
    volatile rmt_chn_rx_carrier_rm_reg_t ch_rx_carrier_rm[4];
    uint32_t reserved_09c[24];
    volatile rmt_date_reg_t date;
} rmt_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(rmt_dev_t) == 0x100, "Invalid size of rmt_dev_t structure");
#endif

extern rmt_dev_t RMT;

typedef struct {
    union {
        struct {
            uint32_t duration0 :15;
            uint32_t level0 :1;
            uint32_t duration1 :15;
            uint32_t level1 :1;
        };
        uint32_t val;
    };
} rmt_item32_t;

//Allow access to RMT memory using RMTMEM.chan[0].data32[8]
typedef volatile struct rmt_mem_s {
    struct {
        rmt_item32_t data32[64];
    } chan[4];
} rmt_mem_t;

extern rmt_mem_t RMTMEM;

#ifdef __cplusplus
}
#endif
