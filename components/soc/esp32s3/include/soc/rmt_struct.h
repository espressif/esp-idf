/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
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
 *  The read and write  data register for CHANNELn by apb fifo access.
 */
typedef union {
    struct {
        /** chndata : RO; bitpos: [31:0]; default: 0;
         *  Read and write data for channel n via APB FIFO.
         */
        uint32_t chndata: 32;
    };
    uint32_t val;
} rmt_chndata_reg_t;

/** Type of chmdata register
 *  The read and write  data register for CHANNEL$n by apb fifo access.
 */
typedef union {
    struct {
        /** chmdata : RO; bitpos: [31:0]; default: 0;
         *  Read and write data for channel $n via APB FIFO.
         */
        uint32_t chmdata: 32;
    };
    uint32_t val;
} rmt_chmdata_reg_t;


/** Group: Configuration registers */
/** Type of chnconf0 register
 *  Channel n configure register 0
 */
typedef union {
    struct {
        /** tx_start_chn : WT; bitpos: [0]; default: 0;
         *  Set this bit to start sending data on CHANNELn.
         */
        uint32_t tx_start_chn: 1;
        /** mem_rd_rst_chn : WT; bitpos: [1]; default: 0;
         *  Set this bit to reset read ram address for CHANNELn by accessing transmitter.
         */
        uint32_t mem_rd_rst_chn: 1;
        /** apb_mem_rst_chn : WT; bitpos: [2]; default: 0;
         *  Set this bit to reset W/R ram address for CHANNELn by accessing apb fifo.
         */
        uint32_t apb_mem_rst_chn: 1;
        /** tx_conti_mode_chn : R/W; bitpos: [3]; default: 0;
         *  Set this bit to restart transmission  from the first data to the last data in
         *  CHANNELn.
         */
        uint32_t tx_conti_mode_chn: 1;
        /** mem_tx_wrap_en_chn : R/W; bitpos: [4]; default: 0;
         *  This is the channel n enable bit for wraparound mode: it will resume sending at the
         *  start when the data to be sent is more than its memory size.
         */
        uint32_t mem_tx_wrap_en_chn: 1;
        /** idle_out_lv_chn : R/W; bitpos: [5]; default: 0;
         *  This bit configures the level of output signal in CHANNELn when the latter is in
         *  IDLE state.
         */
        uint32_t idle_out_lv_chn: 1;
        /** idle_out_en_chn : R/W; bitpos: [6]; default: 0;
         *  This is the output enable-control bit for CHANNELn in IDLE state.
         */
        uint32_t idle_out_en_chn: 1;
        /** tx_stop_chn : R/W/SC; bitpos: [7]; default: 0;
         *  Set this bit to stop the transmitter of CHANNELn sending data out.
         */
        uint32_t tx_stop_chn: 1;
        /** div_cnt_chn : R/W; bitpos: [15:8]; default: 2;
         *  This register is used to configure the divider for clock of CHANNELn.
         */
        uint32_t div_cnt_chn: 8;
        /** mem_size_chn : R/W; bitpos: [19:16]; default: 1;
         *  This register is used to configure the maximum size of memory allocated to CHANNELn.
         */
        uint32_t mem_size_chn: 4;
        /** carrier_eff_en_chn : R/W; bitpos: [20]; default: 1;
         *  1: Add carrier modulation on the output signal only at the send data state for
         *  CHANNELn. 0: Add carrier modulation on the output signal at all state for CHANNELn.
         *  Only valid when RMT_CARRIER_EN_CHn is 1.
         */
        uint32_t carrier_eff_en_chn: 1;
        /** carrier_en_chn : R/W; bitpos: [21]; default: 1;
         *  This is the carrier modulation enable-control bit for CHANNELn. 1: Add carrier
         *  modulation in the output signal. 0: No carrier modulation in sig_out.
         */
        uint32_t carrier_en_chn: 1;
        /** carrier_out_lv_chn : R/W; bitpos: [22]; default: 1;
         *  This bit is used to configure the position of carrier wave for CHANNELn.
         *
         *  1'h0: add carrier wave on low level.
         *
         *  1'h1: add carrier wave on high level.
         */
        uint32_t carrier_out_lv_chn: 1;
        /** afifo_rst_chn : WT; bitpos: [23]; default: 0;
         *  Reserved
         */
        uint32_t afifo_rst_chn: 1;
        /** conf_update_chn : WT; bitpos: [24]; default: 0;
         *  synchronization bit for CHANNELn
         */
        uint32_t conf_update_chn: 1;
        /** dma_access_en_chn : WT; bitpos: [25]; default: 0;
         *  DMA access control bit for CHANNELn (only CHANNEL3 has this control bit)
         */
        uint32_t dma_access_en_chn: 1;
        uint32_t reserved_26: 6;
    };
    uint32_t val;
} rmt_chnconf0_reg_t;

/** Type of chmconf0 register
 *  Channel m configure register 0
 */
typedef union {
    struct {
        /** div_cnt_chm : R/W; bitpos: [7:0]; default: 2;
         *  This register is used to configure the divider for clock of CHANNELm.
         */
        uint32_t div_cnt_chm: 8;
        /** idle_thres_chm : R/W; bitpos: [22:8]; default: 32767;
         *  When no edge is detected on the input signal and continuous clock cycles is longer
         *  than this register value, received process is finished.
         */
        uint32_t idle_thres_chm: 15;
        /** dma_access_en_m : WT; bitpos: [23]; default: 0;
         *  DMA access control bit for CHANNELm (only channel7 has this control bit)
         */
        uint32_t dma_access_en_chm: 1;
        /** mem_size_chm : R/W; bitpos: [27:24]; default: 1;
         *  This register is used to configure the maximum size of memory allocated to CHANNELm.
         */
        uint32_t mem_size_chm: 4;
        /** carrier_en_chm : R/W; bitpos: [28]; default: 1;
         *  This is the carrier modulation enable-control bit for CHANNELm. 1: Add carrier
         *  modulation in the output signal. 0: No carrier modulation in sig_out.
         */
        uint32_t carrier_en_chm: 1;
        /** carrier_out_lv_chm : R/W; bitpos: [29]; default: 1;
         *  This bit is used to configure the position of carrier wave for CHANNELm.
         *
         *  1'h0: add carrier wave on low level.
         *
         *  1'h1: add carrier wave on high level.
         */
        uint32_t carrier_out_lv_chm: 1;
        uint32_t reserved_30: 2;
    };
    uint32_t val;
} rmt_chmconf0_reg_t;

/** Type of chmconf1 register
 *  Channel m configure register 1
 */
typedef union {
    struct {
        /** rx_en_chm : R/W; bitpos: [0]; default: 0;
         *  Set this bit to enable receiver to receive data on CHANNELm.
         */
        uint32_t rx_en_chm: 1;
        /** mem_wr_rst_chm : WT; bitpos: [1]; default: 0;
         *  Set this bit to reset write ram address for CHANNELm by accessing receiver.
         */
        uint32_t mem_wr_rst_chm: 1;
        /** apb_mem_rst_chm : WT; bitpos: [2]; default: 0;
         *  Set this bit to reset W/R ram address for CHANNELm by accessing apb fifo.
         */
        uint32_t apb_mem_rst_chm: 1;
        /** mem_owner_chm : R/W/SC; bitpos: [3]; default: 1;
         *  This register marks the ownership of CHANNELm's ram block.
         *
         *  1'h1: Receiver is using the ram.
         *
         *  1'h0: APB bus is using the ram.
         */
        uint32_t mem_owner_chm: 1;
        /** rx_filter_en_chm : R/W; bitpos: [4]; default: 0;
         *  This is the receive filter's enable bit for CHANNELm.
         */
        uint32_t rx_filter_en_chm: 1;
        /** rx_filter_thres_chm : R/W; bitpos: [12:5]; default: 15;
         *  Ignores the input pulse when its width is smaller than this register value in APB
         *  clock periods (in receive mode).
         */
        uint32_t rx_filter_thres_chm: 8;
        /** mem_rx_wrap_en_chm : R/W; bitpos: [13]; default: 0;
         *  This is the channel m enable bit for wraparound mode: it will resume receiving at
         *  the start when the data to be received is more than its memory size.
         */
        uint32_t mem_rx_wrap_en_chm: 1;
        /** afifo_rst_chm : WT; bitpos: [14]; default: 0;
         *  Reserved
         */
        uint32_t afifo_rst_chm: 1;
        /** conf_update_chm : WT; bitpos: [15]; default: 0;
         *  synchronization bit for CHANNELm
         */
        uint32_t conf_update_chm: 1;
        uint32_t reserved_16: 16;
    };
    uint32_t val;
} rmt_chmconf1_reg_t;

/** Type of chm_rx_carrier_rm register
 *  Channel m carrier remove register
 */
typedef union {
    struct {
        /** carrier_low_thres_chm : R/W; bitpos: [15:0]; default: 0;
         *  The low level period in a carrier modulation mode is
         *  (REG_RMT_REG_CARRIER_LOW_THRES_CHm + 1) for channel m.
         */
        uint32_t carrier_low_thres_chm: 16;
        /** carrier_high_thres_chm : R/W; bitpos: [31:16]; default: 0;
         *  The high level period in a carrier modulation mode is
         *  (REG_RMT_REG_CARRIER_HIGH_THRES_CHm + 1) for channel m.
         */
        uint32_t carrier_high_thres_chm: 16;
    };
    uint32_t val;
} rmt_chm_rx_carrier_rm_reg_t;

/** Type of sys_conf register
 *  RMT apb configuration register
 */
typedef union {
    struct {
        /** apb_fifo_mask : R/W; bitpos: [0]; default: 0;
         *  1'h1: access memory directly.   1'h0: access memory by FIFO.
         */
        uint32_t apb_fifo_mask: 1;
        /** mem_clk_force_on : R/W; bitpos: [1]; default: 0;
         *  Set this bit to enable the clock for RMT memory.
         */
        uint32_t mem_clk_force_on: 1;
        /** mem_force_pd : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down RMT memory.
         */
        uint32_t mem_force_pd: 1;
        /** mem_force_pu : R/W; bitpos: [3]; default: 0;
         *  1: Disable RMT memory light sleep power down function. 0: Power down RMT memory
         *  when RMT is in light sleep mode.
         */
        uint32_t mem_force_pu: 1;
        /** sclk_div_num : R/W; bitpos: [11:4]; default: 1;
         *  the integral part of the fractional divisor
         */
        uint32_t sclk_div_num: 8;
        /** sclk_div_a : R/W; bitpos: [17:12]; default: 0;
         *  the numerator of the fractional part of the fractional divisor
         */
        uint32_t sclk_div_a: 6;
        /** sclk_div_b : R/W; bitpos: [23:18]; default: 0;
         *  the denominator of the fractional part of the fractional divisor
         */
        uint32_t sclk_div_b: 6;
        /** sclk_sel : R/W; bitpos: [25:24]; default: 1;
         *  choose the clock source of rmt_sclk. 1:CLK_80Mhz;2:CLK_8MHz; 2:XTAL
         */
        uint32_t sclk_sel: 2;
        /** sclk_active : R/W; bitpos: [26]; default: 1;
         *  rmt_sclk switch
         */
        uint32_t sclk_active: 1;
        uint32_t reserved_27: 4;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  RMT register clock gate enable signal. 1: Power up the drive clock of registers. 0:
         *  Power down the drive clock of registers
         */
        uint32_t clk_en: 1;
    };
    uint32_t val;
} rmt_sys_conf_reg_t;

/** Type of ref_cnt_rst register
 *  RMT clock divider reset register
 */
typedef union {
    struct {
        /** ref_cnt_rst_ch0 : WT; bitpos: [0]; default: 0;
         *  This register is used to reset the clock divider of CHANNEL0.
         */
        uint32_t ref_cnt_rst_ch0: 1;
        /** ref_cnt_rst_ch1 : WT; bitpos: [1]; default: 0;
         *  This register is used to reset the clock divider of CHANNEL1.
         */
        uint32_t ref_cnt_rst_ch1: 1;
        /** ref_cnt_rst_ch2 : WT; bitpos: [2]; default: 0;
         *  This register is used to reset the clock divider of CHANNEL2.
         */
        uint32_t ref_cnt_rst_ch2: 1;
        /** ref_cnt_rst_ch3 : WT; bitpos: [3]; default: 0;
         *  This register is used to reset the clock divider of CHANNEL3.
         */
        uint32_t ref_cnt_rst_ch3: 1;
        /** ref_cnt_rst_ch4 : WT; bitpos: [4]; default: 0;
         *  This register is used to reset the clock divider of CHANNEL4.
         */
        uint32_t ref_cnt_rst_ch4: 1;
        /** ref_cnt_rst_ch5 : WT; bitpos: [5]; default: 0;
         *  This register is used to reset the clock divider of CHANNEL5.
         */
        uint32_t ref_cnt_rst_ch5: 1;
        /** ref_cnt_rst_ch6 : WT; bitpos: [6]; default: 0;
         *  This register is used to reset the clock divider of CHANNEL6.
         */
        uint32_t ref_cnt_rst_ch6: 1;
        /** ref_cnt_rst_ch7 : WT; bitpos: [7]; default: 0;
         *  This register is used to reset the clock divider of CHANNEL7.
         */
        uint32_t ref_cnt_rst_ch7: 1;
        uint32_t reserved_8: 24;
    };
    uint32_t val;
} rmt_ref_cnt_rst_reg_t;


/** Group: Status registers */
/** Type of chnstatus register
 *  Channel n status register
 */
typedef union {
    struct {
        /** mem_raddr_ex_chn : RO; bitpos: [9:0]; default: 0;
         *  This register records the memory address offset when transmitter of CHANNELn is
         *  using the RAM.
         */
        uint32_t mem_raddr_ex_chn: 10;
        uint32_t reserved_10: 1;
        /** apb_mem_waddr_chn : RO; bitpos: [20:11]; default: 0;
         *  This register records the memory address offset when writes RAM over APB bus.
         */
        uint32_t apb_mem_waddr_chn: 10;
        uint32_t reserved_21: 1;
        /** state_chn : RO; bitpos: [24:22]; default: 0;
         *  This register records the FSM status of CHANNELn.
         */
        uint32_t state_chn: 3;
        /** mem_empty_chn : RO; bitpos: [25]; default: 0;
         *  This status bit will be set when the data to be set is more than memory size and
         *  the wraparound mode is disabled.
         */
        uint32_t mem_empty_chn: 1;
        /** apb_mem_wr_err_chn : RO; bitpos: [26]; default: 0;
         *  This status bit will be set if the offset address out of memory size when writes
         *  via APB bus.
         */
        uint32_t apb_mem_wr_err_chn: 1;
        uint32_t reserved_27: 5;
    };
    uint32_t val;
} rmt_chnstatus_reg_t;

/** Type of chmstatus register
 *  Channel m status register
 */
typedef union {
    struct {
        /** mem_waddr_ex_chm : RO; bitpos: [9:0]; default: 192;
         *  This register records the memory address offset when receiver of CHANNELm is using
         *  the RAM.
         */
        uint32_t mem_waddr_ex_chm: 10;
        uint32_t reserved_10: 1;
        /** apb_mem_raddr_chm : RO; bitpos: [20:11]; default: 192;
         *  This register records the memory address offset when reads RAM over APB bus.
         */
        uint32_t apb_mem_raddr_chm: 10;
        uint32_t reserved_21: 1;
        /** state_chm : RO; bitpos: [24:22]; default: 0;
         *  This register records the FSM status of CHANNELm.
         */
        uint32_t state_chm: 3;
        /** mem_owner_err_chm : RO; bitpos: [25]; default: 0;
         *  This status bit will be set when the ownership of memory block is wrong.
         */
        uint32_t mem_owner_err_chm: 1;
        /** mem_full_chm : RO; bitpos: [26]; default: 0;
         *  This status bit will be set if the receiver receives more data than the memory size.
         */
        uint32_t mem_full_chm: 1;
        /** apb_mem_rd_err_chm : RO; bitpos: [27]; default: 0;
         *  This status bit will be set if the offset address out of memory size when reads via
         *  APB bus.
         */
        uint32_t apb_mem_rd_err_chm: 1;
        uint32_t reserved_28: 4;
    };
    uint32_t val;
} rmt_chmstatus_reg_t;


/** Group: Interrupt registers */
/** Type of int_raw register
 *  Raw interrupt status
 */
typedef union {
    struct {
        /** ch0_tx_end_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The interrupt raw bit for CHANNEL0. Triggered when transmission done.
         */
        uint32_t ch0_tx_end_int_raw: 1;
        /** ch1_tx_end_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The interrupt raw bit for CHANNEL1. Triggered when transmission done.
         */
        uint32_t ch1_tx_end_int_raw: 1;
        /** ch2_tx_end_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The interrupt raw bit for CHANNEL2. Triggered when transmission done.
         */
        uint32_t ch2_tx_end_int_raw: 1;
        /** ch3_tx_end_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The interrupt raw bit for CHANNEL3. Triggered when transmission done.
         */
        uint32_t ch3_tx_end_int_raw: 1;
        /** ch0_err_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The interrupt raw bit for CHANNEL0. Triggered when error occurs.
         */
        uint32_t ch0_err_int_raw: 1;
        /** ch1_err_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The interrupt raw bit for CHANNEL1. Triggered when error occurs.
         */
        uint32_t ch1_err_int_raw: 1;
        /** ch2_err_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The interrupt raw bit for CHANNEL2. Triggered when error occurs.
         */
        uint32_t ch2_err_int_raw: 1;
        /** ch3_err_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The interrupt raw bit for CHANNEL3. Triggered when error occurs.
         */
        uint32_t ch3_err_int_raw: 1;
        /** ch0_tx_thr_event_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The interrupt raw bit for CHANNEL0. Triggered when transmitter sent more data than
         *  configured value.
         */
        uint32_t ch0_tx_thr_event_int_raw: 1;
        /** ch1_tx_thr_event_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  The interrupt raw bit for CHANNEL1. Triggered when transmitter sent more data than
         *  configured value.
         */
        uint32_t ch1_tx_thr_event_int_raw: 1;
        /** ch2_tx_thr_event_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  The interrupt raw bit for CHANNEL2. Triggered when transmitter sent more data than
         *  configured value.
         */
        uint32_t ch2_tx_thr_event_int_raw: 1;
        /** ch3_tx_thr_event_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  The interrupt raw bit for CHANNEL3. Triggered when transmitter sent more data than
         *  configured value.
         */
        uint32_t ch3_tx_thr_event_int_raw: 1;
        /** ch0_tx_loop_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  The interrupt raw bit for CHANNEL0. Triggered when the loop count reaches the
         *  configured threshold value.
         */
        uint32_t ch0_tx_loop_int_raw: 1;
        /** ch1_tx_loop_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  The interrupt raw bit for CHANNEL1. Triggered when the loop count reaches the
         *  configured threshold value.
         */
        uint32_t ch1_tx_loop_int_raw: 1;
        /** ch2_tx_loop_int_raw : R/WTC/SS; bitpos: [14]; default: 0;
         *  The interrupt raw bit for CHANNEL2. Triggered when the loop count reaches the
         *  configured threshold value.
         */
        uint32_t ch2_tx_loop_int_raw: 1;
        /** ch3_tx_loop_int_raw : R/WTC/SS; bitpos: [15]; default: 0;
         *  The interrupt raw bit for CHANNEL3. Triggered when the loop count reaches the
         *  configured threshold value.
         */
        uint32_t ch3_tx_loop_int_raw: 1;
        /** ch4_rx_end_int_raw : R/WTC/SS; bitpos: [16]; default: 0;
         *  The interrupt raw bit for CHANNEL4. Triggered when reception done.
         */
        uint32_t ch4_rx_end_int_raw: 1;
        /** ch5_rx_end_int_raw : R/WTC/SS; bitpos: [17]; default: 0;
         *  The interrupt raw bit for CHANNEL5. Triggered when reception done.
         */
        uint32_t ch5_rx_end_int_raw: 1;
        /** ch6_rx_end_int_raw : R/WTC/SS; bitpos: [18]; default: 0;
         *  The interrupt raw bit for CHANNEL6. Triggered when reception done.
         */
        uint32_t ch6_rx_end_int_raw: 1;
        /** ch7_rx_end_int_raw : R/WTC/SS; bitpos: [19]; default: 0;
         *  The interrupt raw bit for CHANNEL7. Triggered when reception done.
         */
        uint32_t ch7_rx_end_int_raw: 1;
        /** ch4_err_int_raw : R/WTC/SS; bitpos: [20]; default: 0;
         *  The interrupt raw bit for CHANNEL4. Triggered when error occurs.
         */
        uint32_t ch4_err_int_raw: 1;
        /** ch5_err_int_raw : R/WTC/SS; bitpos: [21]; default: 0;
         *  The interrupt raw bit for CHANNEL5. Triggered when error occurs.
         */
        uint32_t ch5_err_int_raw: 1;
        /** ch6_err_int_raw : R/WTC/SS; bitpos: [22]; default: 0;
         *  The interrupt raw bit for CHANNEL6. Triggered when error occurs.
         */
        uint32_t ch6_err_int_raw: 1;
        /** ch7_err_int_raw : R/WTC/SS; bitpos: [23]; default: 0;
         *  The interrupt raw bit for CHANNEL7. Triggered when error occurs.
         */
        uint32_t ch7_err_int_raw: 1;
        /** ch4_rx_thr_event_int_raw : R/WTC/SS; bitpos: [24]; default: 0;
         *  The interrupt raw bit for CHANNEL4. Triggered when receiver receive more data than
         *  configured value.
         */
        uint32_t ch4_rx_thr_event_int_raw: 1;
        /** ch5_rx_thr_event_int_raw : R/WTC/SS; bitpos: [25]; default: 0;
         *  The interrupt raw bit for CHANNEL5. Triggered when receiver receive more data than
         *  configured value.
         */
        uint32_t ch5_rx_thr_event_int_raw: 1;
        /** ch6_rx_thr_event_int_raw : R/WTC/SS; bitpos: [26]; default: 0;
         *  The interrupt raw bit for CHANNEL6. Triggered when receiver receive more data than
         *  configured value.
         */
        uint32_t ch6_rx_thr_event_int_raw: 1;
        /** ch7_rx_thr_event_int_raw : R/WTC/SS; bitpos: [27]; default: 0;
         *  The interrupt raw bit for CHANNEL7. Triggered when receiver receive more data than
         *  configured value.
         */
        uint32_t ch7_rx_thr_event_int_raw: 1;
        /** ch3_dma_access_fail_int_raw : R/WTC/SS; bitpos: [28]; default: 0;
         *  The interrupt raw bit for CHANNEL3. Triggered when dma accessing CHANNEL3 fails.
         */
        uint32_t ch3_dma_access_fail_int_raw: 1;
        /** ch7_dma_access_fail_int_raw : R/WTC/SS; bitpos: [29]; default: 0;
         *  The interrupt raw bit for CHANNEL7. Triggered when dma accessing CHANNEL7 fails.
         */
        uint32_t ch7_dma_access_fail_int_raw: 1;
        uint32_t reserved_30: 2;
    };
    uint32_t val;
} rmt_int_raw_reg_t;

/** Type of int_st register
 *  Masked interrupt status
 */
typedef union {
    struct {
        /** ch0_tx_end_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status bit for CH0_TX_END_INT.
         */
        uint32_t ch0_tx_end_int_st: 1;
        /** ch1_tx_end_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status bit for CH1_TX_END_INT.
         */
        uint32_t ch1_tx_end_int_st: 1;
        /** ch2_tx_end_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status bit for CH2_TX_END_INT.
         */
        uint32_t ch2_tx_end_int_st: 1;
        /** ch3_tx_end_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status bit for CH3_TX_END_INT.
         */
        uint32_t ch3_tx_end_int_st: 1;
        /** ch0_err_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status bit for CH0_ERR_INT.
         */
        uint32_t ch0_err_int_st: 1;
        /** ch1_err_int_st : RO; bitpos: [5]; default: 0;
         *  The masked interrupt status bit for CH1_ERR_INT.
         */
        uint32_t ch1_err_int_st: 1;
        /** ch2_err_int_st : RO; bitpos: [6]; default: 0;
         *  The masked interrupt status bit for CH2_ERR_INT.
         */
        uint32_t ch2_err_int_st: 1;
        /** ch3_err_int_st : RO; bitpos: [7]; default: 0;
         *  The masked interrupt status bit for CH3_ERR_INT.
         */
        uint32_t ch3_err_int_st: 1;
        /** ch0_tx_thr_event_int_st : RO; bitpos: [8]; default: 0;
         *  The masked interrupt status bit for CH0_TX_THR_EVENT_INT.
         */
        uint32_t ch0_tx_thr_event_int_st: 1;
        /** ch1_tx_thr_event_int_st : RO; bitpos: [9]; default: 0;
         *  The masked interrupt status bit for CH1_TX_THR_EVENT_INT.
         */
        uint32_t ch1_tx_thr_event_int_st: 1;
        /** ch2_tx_thr_event_int_st : RO; bitpos: [10]; default: 0;
         *  The masked interrupt status bit for CH2_TX_THR_EVENT_INT.
         */
        uint32_t ch2_tx_thr_event_int_st: 1;
        /** ch3_tx_thr_event_int_st : RO; bitpos: [11]; default: 0;
         *  The masked interrupt status bit for CH3_TX_THR_EVENT_INT.
         */
        uint32_t ch3_tx_thr_event_int_st: 1;
        /** ch0_tx_loop_int_st : RO; bitpos: [12]; default: 0;
         *  The masked interrupt status bit for CH0_TX_LOOP_INT.
         */
        uint32_t ch0_tx_loop_int_st: 1;
        /** ch1_tx_loop_int_st : RO; bitpos: [13]; default: 0;
         *  The masked interrupt status bit for CH1_TX_LOOP_INT.
         */
        uint32_t ch1_tx_loop_int_st: 1;
        /** ch2_tx_loop_int_st : RO; bitpos: [14]; default: 0;
         *  The masked interrupt status bit for CH2_TX_LOOP_INT.
         */
        uint32_t ch2_tx_loop_int_st: 1;
        /** ch3_tx_loop_int_st : RO; bitpos: [15]; default: 0;
         *  The masked interrupt status bit for CH3_TX_LOOP_INT.
         */
        uint32_t ch3_tx_loop_int_st: 1;
        /** ch4_rx_end_int_st : RO; bitpos: [16]; default: 0;
         *  The masked interrupt status bit for CH4_RX_END_INT.
         */
        uint32_t ch4_rx_end_int_st: 1;
        /** ch5_rx_end_int_st : RO; bitpos: [17]; default: 0;
         *  The masked interrupt status bit for CH5_RX_END_INT.
         */
        uint32_t ch5_rx_end_int_st: 1;
        /** ch6_rx_end_int_st : RO; bitpos: [18]; default: 0;
         *  The masked interrupt status bit for CH6_RX_END_INT.
         */
        uint32_t ch6_rx_end_int_st: 1;
        /** ch7_rx_end_int_st : RO; bitpos: [19]; default: 0;
         *  The masked interrupt status bit for CH7_RX_END_INT.
         */
        uint32_t ch7_rx_end_int_st: 1;
        /** ch4_err_int_st : RO; bitpos: [20]; default: 0;
         *  The masked interrupt status bit for CH4_ERR_INT.
         */
        uint32_t ch4_err_int_st: 1;
        /** ch5_err_int_st : RO; bitpos: [21]; default: 0;
         *  The masked interrupt status bit for CH5_ERR_INT.
         */
        uint32_t ch5_err_int_st: 1;
        /** ch6_err_int_st : RO; bitpos: [22]; default: 0;
         *  The masked interrupt status bit for CH6_ERR_INT.
         */
        uint32_t ch6_err_int_st: 1;
        /** ch7_err_int_st : RO; bitpos: [23]; default: 0;
         *  The masked interrupt status bit for CH7_ERR_INT.
         */
        uint32_t ch7_err_int_st: 1;
        /** ch4_rx_thr_event_int_st : RO; bitpos: [24]; default: 0;
         *  The masked interrupt status bit for CH4_RX_THR_EVENT_INT.
         */
        uint32_t ch4_rx_thr_event_int_st: 1;
        /** ch5_rx_thr_event_int_st : RO; bitpos: [25]; default: 0;
         *  The masked interrupt status bit for CH5_RX_THR_EVENT_INT.
         */
        uint32_t ch5_rx_thr_event_int_st: 1;
        /** ch6_rx_thr_event_int_st : RO; bitpos: [26]; default: 0;
         *  The masked interrupt status bit for CH6_RX_THR_EVENT_INT.
         */
        uint32_t ch6_rx_thr_event_int_st: 1;
        /** ch7_rx_thr_event_int_st : RO; bitpos: [27]; default: 0;
         *  The masked interrupt status bit for CH7_RX_THR_EVENT_INT.
         */
        uint32_t ch7_rx_thr_event_int_st: 1;
        /** ch3_dma_access_fail_int_st : RO; bitpos: [28]; default: 0;
         *  The masked interrupt status bit for  CH3_DMA_ACCESS_FAIL_INT.
         */
        uint32_t ch3_dma_access_fail_int_st: 1;
        /** ch7_dma_access_fail_int_st : RO; bitpos: [29]; default: 0;
         *  The masked interrupt status bit for  CH7_DMA_ACCESS_FAIL_INT.
         */
        uint32_t ch7_dma_access_fail_int_st: 1;
        uint32_t reserved_30: 2;
    };
    uint32_t val;
} rmt_int_st_reg_t;

/** Type of int_ena register
 *  Interrupt enable bits
 */
typedef union {
    struct {
        /** ch0_tx_end_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for CH0_TX_END_INT.
         */
        uint32_t ch0_tx_end_int_ena: 1;
        /** ch1_tx_end_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for CH1_TX_END_INT.
         */
        uint32_t ch1_tx_end_int_ena: 1;
        /** ch2_tx_end_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for CH2_TX_END_INT.
         */
        uint32_t ch2_tx_end_int_ena: 1;
        /** ch3_tx_end_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for CH3_TX_END_INT.
         */
        uint32_t ch3_tx_end_int_ena: 1;
        /** ch0_err_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for CH0_ERR_INT.
         */
        uint32_t ch0_err_int_ena: 1;
        /** ch1_err_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit for CH1_ERR_INT.
         */
        uint32_t ch1_err_int_ena: 1;
        /** ch2_err_int_ena : R/W; bitpos: [6]; default: 0;
         *  The interrupt enable bit for CH2_ERR_INT.
         */
        uint32_t ch2_err_int_ena: 1;
        /** ch3_err_int_ena : R/W; bitpos: [7]; default: 0;
         *  The interrupt enable bit for CH3_ERR_INT.
         */
        uint32_t ch3_err_int_ena: 1;
        /** ch0_tx_thr_event_int_ena : R/W; bitpos: [8]; default: 0;
         *  The interrupt enable bit for CH0_TX_THR_EVENT_INT.
         */
        uint32_t ch0_tx_thr_event_int_ena: 1;
        /** ch1_tx_thr_event_int_ena : R/W; bitpos: [9]; default: 0;
         *  The interrupt enable bit for CH1_TX_THR_EVENT_INT.
         */
        uint32_t ch1_tx_thr_event_int_ena: 1;
        /** ch2_tx_thr_event_int_ena : R/W; bitpos: [10]; default: 0;
         *  The interrupt enable bit for CH2_TX_THR_EVENT_INT.
         */
        uint32_t ch2_tx_thr_event_int_ena: 1;
        /** ch3_tx_thr_event_int_ena : R/W; bitpos: [11]; default: 0;
         *  The interrupt enable bit for CH3_TX_THR_EVENT_INT.
         */
        uint32_t ch3_tx_thr_event_int_ena: 1;
        /** ch0_tx_loop_int_ena : R/W; bitpos: [12]; default: 0;
         *  The interrupt enable bit for CH0_TX_LOOP_INT.
         */
        uint32_t ch0_tx_loop_int_ena: 1;
        /** ch1_tx_loop_int_ena : R/W; bitpos: [13]; default: 0;
         *  The interrupt enable bit for CH1_TX_LOOP_INT.
         */
        uint32_t ch1_tx_loop_int_ena: 1;
        /** ch2_tx_loop_int_ena : R/W; bitpos: [14]; default: 0;
         *  The interrupt enable bit for CH2_TX_LOOP_INT.
         */
        uint32_t ch2_tx_loop_int_ena: 1;
        /** ch3_tx_loop_int_ena : R/W; bitpos: [15]; default: 0;
         *  The interrupt enable bit for CH3_TX_LOOP_INT.
         */
        uint32_t ch3_tx_loop_int_ena: 1;
        /** ch4_rx_end_int_ena : R/W; bitpos: [16]; default: 0;
         *  The interrupt enable bit for CH4_RX_END_INT.
         */
        uint32_t ch4_rx_end_int_ena: 1;
        /** ch5_rx_end_int_ena : R/W; bitpos: [17]; default: 0;
         *  The interrupt enable bit for CH5_RX_END_INT.
         */
        uint32_t ch5_rx_end_int_ena: 1;
        /** ch6_rx_end_int_ena : R/W; bitpos: [18]; default: 0;
         *  The interrupt enable bit for CH6_RX_END_INT.
         */
        uint32_t ch6_rx_end_int_ena: 1;
        /** ch7_rx_end_int_ena : R/W; bitpos: [19]; default: 0;
         *  The interrupt enable bit for CH7_RX_END_INT.
         */
        uint32_t ch7_rx_end_int_ena: 1;
        /** ch4_err_int_ena : R/W; bitpos: [20]; default: 0;
         *  The interrupt enable bit for CH4_ERR_INT.
         */
        uint32_t ch4_err_int_ena: 1;
        /** ch5_err_int_ena : R/W; bitpos: [21]; default: 0;
         *  The interrupt enable bit for CH5_ERR_INT.
         */
        uint32_t ch5_err_int_ena: 1;
        /** ch6_err_int_ena : R/W; bitpos: [22]; default: 0;
         *  The interrupt enable bit for CH6_ERR_INT.
         */
        uint32_t ch6_err_int_ena: 1;
        /** ch7_err_int_ena : R/W; bitpos: [23]; default: 0;
         *  The interrupt enable bit for CH7_ERR_INT.
         */
        uint32_t ch7_err_int_ena: 1;
        /** ch4_rx_thr_event_int_ena : R/W; bitpos: [24]; default: 0;
         *  The interrupt enable bit for CH4_RX_THR_EVENT_INT.
         */
        uint32_t ch4_rx_thr_event_int_ena: 1;
        /** ch5_rx_thr_event_int_ena : R/W; bitpos: [25]; default: 0;
         *  The interrupt enable bit for CH5_RX_THR_EVENT_INT.
         */
        uint32_t ch5_rx_thr_event_int_ena: 1;
        /** ch6_rx_thr_event_int_ena : R/W; bitpos: [26]; default: 0;
         *  The interrupt enable bit for CH6_RX_THR_EVENT_INT.
         */
        uint32_t ch6_rx_thr_event_int_ena: 1;
        /** ch7_rx_thr_event_int_ena : R/W; bitpos: [27]; default: 0;
         *  The interrupt enable bit for CH7_RX_THR_EVENT_INT.
         */
        uint32_t ch7_rx_thr_event_int_ena: 1;
        /** ch3_dma_access_fail_int_ena : R/W; bitpos: [28]; default: 0;
         *  The interrupt enable bit for CH3_DMA_ACCESS_FAIL_INT.
         */
        uint32_t ch3_dma_access_fail_int_ena: 1;
        /** ch7_dma_access_fail_int_ena : R/W; bitpos: [29]; default: 0;
         *  The interrupt enable bit for CH7_DMA_ACCESS_FAIL_INT.
         */
        uint32_t ch7_dma_access_fail_int_ena: 1;
        uint32_t reserved_30: 2;
    };
    uint32_t val;
} rmt_int_ena_reg_t;

/** Type of int_clr register
 *  Interrupt clear bits
 */
typedef union {
    struct {
        /** ch0_tx_end_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear theCH0_TX_END_INT interrupt.
         */
        uint32_t ch0_tx_end_int_clr: 1;
        /** ch1_tx_end_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear theCH1_TX_END_INT interrupt.
         */
        uint32_t ch1_tx_end_int_clr: 1;
        /** ch2_tx_end_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear theCH2_TX_END_INT interrupt.
         */
        uint32_t ch2_tx_end_int_clr: 1;
        /** ch3_tx_end_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear theCH3_TX_END_INT interrupt.
         */
        uint32_t ch3_tx_end_int_clr: 1;
        /** ch0_err_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear theCH0_ERR_INT interrupt.
         */
        uint32_t ch0_err_int_clr: 1;
        /** ch1_err_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear theCH1_ERR_INT interrupt.
         */
        uint32_t ch1_err_int_clr: 1;
        /** ch2_err_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear theCH2_ERR_INT interrupt.
         */
        uint32_t ch2_err_int_clr: 1;
        /** ch3_err_int_clr : WT; bitpos: [7]; default: 0;
         *  Set this bit to clear theCH3_ERR_INT interrupt.
         */
        uint32_t ch3_err_int_clr: 1;
        /** ch0_tx_thr_event_int_clr : WT; bitpos: [8]; default: 0;
         *  Set this bit to clear theCH0_TX_THR_EVENT_INT interrupt.
         */
        uint32_t ch0_tx_thr_event_int_clr: 1;
        /** ch1_tx_thr_event_int_clr : WT; bitpos: [9]; default: 0;
         *  Set this bit to clear theCH1_TX_THR_EVENT_INT interrupt.
         */
        uint32_t ch1_tx_thr_event_int_clr: 1;
        /** ch2_tx_thr_event_int_clr : WT; bitpos: [10]; default: 0;
         *  Set this bit to clear theCH2_TX_THR_EVENT_INT interrupt.
         */
        uint32_t ch2_tx_thr_event_int_clr: 1;
        /** ch3_tx_thr_event_int_clr : WT; bitpos: [11]; default: 0;
         *  Set this bit to clear theCH3_TX_THR_EVENT_INT interrupt.
         */
        uint32_t ch3_tx_thr_event_int_clr: 1;
        /** ch0_tx_loop_int_clr : WT; bitpos: [12]; default: 0;
         *  Set this bit to clear theCH0_TX_LOOP_INT interrupt.
         */
        uint32_t ch0_tx_loop_int_clr: 1;
        /** ch1_tx_loop_int_clr : WT; bitpos: [13]; default: 0;
         *  Set this bit to clear theCH1_TX_LOOP_INT interrupt.
         */
        uint32_t ch1_tx_loop_int_clr: 1;
        /** ch2_tx_loop_int_clr : WT; bitpos: [14]; default: 0;
         *  Set this bit to clear theCH2_TX_LOOP_INT interrupt.
         */
        uint32_t ch2_tx_loop_int_clr: 1;
        /** ch3_tx_loop_int_clr : WT; bitpos: [15]; default: 0;
         *  Set this bit to clear theCH3_TX_LOOP_INT interrupt.
         */
        uint32_t ch3_tx_loop_int_clr: 1;
        /** ch4_rx_end_int_clr : WT; bitpos: [16]; default: 0;
         *  Set this bit to clear theCH4_RX_END_INT interrupt.
         */
        uint32_t ch4_rx_end_int_clr: 1;
        /** ch5_rx_end_int_clr : WT; bitpos: [17]; default: 0;
         *  Set this bit to clear theCH5_RX_END_INT interrupt.
         */
        uint32_t ch5_rx_end_int_clr: 1;
        /** ch6_rx_end_int_clr : WT; bitpos: [18]; default: 0;
         *  Set this bit to clear theCH6_RX_END_INT interrupt.
         */
        uint32_t ch6_rx_end_int_clr: 1;
        /** ch7_rx_end_int_clr : WT; bitpos: [19]; default: 0;
         *  Set this bit to clear theCH7_RX_END_INT interrupt.
         */
        uint32_t ch7_rx_end_int_clr: 1;
        /** ch4_err_int_clr : WT; bitpos: [20]; default: 0;
         *  Set this bit to clear theCH4_ERR_INT interrupt.
         */
        uint32_t ch4_err_int_clr: 1;
        /** ch5_err_int_clr : WT; bitpos: [21]; default: 0;
         *  Set this bit to clear theCH5_ERR_INT interrupt.
         */
        uint32_t ch5_err_int_clr: 1;
        /** ch6_err_int_clr : WT; bitpos: [22]; default: 0;
         *  Set this bit to clear theCH6_ERR_INT interrupt.
         */
        uint32_t ch6_err_int_clr: 1;
        /** ch7_err_int_clr : WT; bitpos: [23]; default: 0;
         *  Set this bit to clear theCH7_ERR_INT interrupt.
         */
        uint32_t ch7_err_int_clr: 1;
        /** ch4_rx_thr_event_int_clr : WT; bitpos: [24]; default: 0;
         *  Set this bit to clear theCH4_RX_THR_EVENT_INT interrupt.
         */
        uint32_t ch4_rx_thr_event_int_clr: 1;
        /** ch5_rx_thr_event_int_clr : WT; bitpos: [25]; default: 0;
         *  Set this bit to clear theCH5_RX_THR_EVENT_INT interrupt.
         */
        uint32_t ch5_rx_thr_event_int_clr: 1;
        /** ch6_rx_thr_event_int_clr : WT; bitpos: [26]; default: 0;
         *  Set this bit to clear theCH6_RX_THR_EVENT_INT interrupt.
         */
        uint32_t ch6_rx_thr_event_int_clr: 1;
        /** ch7_rx_thr_event_int_clr : WT; bitpos: [27]; default: 0;
         *  Set this bit to clear theCH7_RX_THR_EVENT_INT interrupt.
         */
        uint32_t ch7_rx_thr_event_int_clr: 1;
        /** ch3_dma_access_fail_int_clr : WT; bitpos: [28]; default: 0;
         *  Set this bit to clear the CH3_DMA_ACCESS_FAIL_INT interrupt.
         */
        uint32_t ch3_dma_access_fail_int_clr: 1;
        /** ch7_dma_access_fail_int_clr : WT; bitpos: [29]; default: 0;
         *  Set this bit to clear the CH7_DMA_ACCESS_FAIL_INT interrupt.
         */
        uint32_t ch7_dma_access_fail_int_clr: 1;
        uint32_t reserved_30: 2;
    };
    uint32_t val;
} rmt_int_clr_reg_t;


/** Group: Carrier wave duty cycle registers */
/** Type of chncarrier_duty register
 *  Channel n duty cycle configuration register
 */
typedef union {
    struct {
        /** carrier_low_chn : R/W; bitpos: [15:0]; default: 64;
         *  This register is used to configure carrier wave 's low level clock period for
         *  CHANNELn.
         */
        uint32_t carrier_low_chn: 16;
        /** carrier_high_chn : R/W; bitpos: [31:16]; default: 64;
         *  This register is used to configure carrier wave 's high level clock period for
         *  CHANNELn.
         */
        uint32_t carrier_high_chn: 16;
    };
    uint32_t val;
} rmt_chncarrier_duty_reg_t;


/** Group: Tx event configuration registers */
/** Type of chn_tx_lim register
 *  Channel n Tx event configuration register
 */
typedef union {
    struct {
        /** tx_lim_chn : R/W; bitpos: [8:0]; default: 128;
         *  This register is used to configure the maximum entries that CHANNELn can send out.
         */
        uint32_t tx_lim_chn: 9;
        /** tx_loop_num_chn : R/W; bitpos: [18:9]; default: 0;
         *  This register is used to configure the maximum loop count when tx_conti_mode is
         *  valid.
         */
        uint32_t tx_loop_num_chn: 10;
        /** tx_loop_cnt_en_chn : R/W; bitpos: [19]; default: 0;
         *  This register is the enabled bit for loop count.
         */
        uint32_t tx_loop_cnt_en_chn: 1;
        /** loop_count_reset_chn : WT; bitpos: [20]; default: 0;
         *  This register is used to reset the loop count when tx_conti_mode is valid.
         */
        uint32_t loop_count_reset_chn: 1;
        /** loop_stop_en_chn : R/W; bitpos: [21]; default: 0;
         *  This bit is used to enable the loop send stop function after the loop counter
         *  counts to  loop number for CHANNELn.
         */
        uint32_t loop_stop_en_chn: 1;
        uint32_t reserved_22: 10;
    };
    uint32_t val;
} rmt_chn_tx_lim_reg_t;

/** Type of tx_sim register
 *  RMT TX synchronous register
 */
typedef union {
    struct {
        /** tx_sim_ch0 : R/W; bitpos: [0]; default: 0;
         *  Set this bit to enable CHANNEL0 to start sending data synchronously with other
         *  enabled channels.
         */
        uint32_t tx_sim_ch0: 1;
        /** tx_sim_ch1 : R/W; bitpos: [1]; default: 0;
         *  Set this bit to enable CHANNEL1 to start sending data synchronously with other
         *  enabled channels.
         */
        uint32_t tx_sim_ch1: 1;
        /** tx_sim_ch2 : R/W; bitpos: [2]; default: 0;
         *  Set this bit to enable CHANNEL2 to start sending data synchronously with other
         *  enabled channels.
         */
        uint32_t tx_sim_ch2: 1;
        /** tx_sim_ch3 : R/W; bitpos: [3]; default: 0;
         *  Set this bit to enable CHANNEL3 to start sending data synchronously with other
         *  enabled channels.
         */
        uint32_t tx_sim_ch3: 1;
        /** tx_sim_en : R/W; bitpos: [4]; default: 0;
         *  This register is used to enable multiple of channels to start sending data
         *  synchronously.
         */
        uint32_t tx_sim_en: 1;
        uint32_t reserved_5: 27;
    };
    uint32_t val;
} rmt_tx_sim_reg_t;


/** Group: Rx event configuration registers */
/** Type of chm_rx_lim register
 *  Channel m Rx event configuration register
 */
typedef union {
    struct {
        /** rx_lim_chm : R/W; bitpos: [8:0]; default: 128;
         *  This register is used to configure the maximum entries that CHANNELm can receive.
         */
        uint32_t rx_lim_chm: 9;
        uint32_t reserved_9: 23;
    };
    uint32_t val;
} rmt_chm_rx_lim_reg_t;


/** Group: Version register */
/** Type of date register
 *  RMT version register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 34607489;
         *  This is the version register.
         */
        uint32_t date: 28;
        uint32_t reserved_28: 4;
    };
    uint32_t val;
} rmt_date_reg_t;


typedef struct rmt_dev_t {
    volatile rmt_chndata_reg_t chndata[4];
    volatile rmt_chmdata_reg_t chmdata[4];
    volatile rmt_chnconf0_reg_t chnconf0[4];
    volatile struct {
        rmt_chmconf0_reg_t conf0;
        rmt_chmconf1_reg_t conf1;
    } chmconf[4];
    volatile rmt_chnstatus_reg_t chnstatus[4];
    volatile rmt_chmstatus_reg_t chmstatus[4];
    volatile rmt_int_raw_reg_t int_raw;
    volatile rmt_int_st_reg_t int_st;
    volatile rmt_int_ena_reg_t int_ena;
    volatile rmt_int_clr_reg_t int_clr;
    volatile rmt_chncarrier_duty_reg_t chncarrier_duty[4];
    volatile rmt_chm_rx_carrier_rm_reg_t chm_rx_carrier_rm[4];
    volatile rmt_chn_tx_lim_reg_t chn_tx_lim[4];
    volatile rmt_chm_rx_lim_reg_t chm_rx_lim[4];
    volatile rmt_sys_conf_reg_t sys_conf;
    volatile rmt_tx_sim_reg_t tx_sim;
    volatile rmt_ref_cnt_rst_reg_t ref_cnt_rst;
    volatile rmt_date_reg_t date;
} rmt_dev_t;

#ifndef __cplusplus
_Static_assert(sizeof(rmt_dev_t) == 0xd0, "Invalid size of rmt_dev_t structure");
#endif

extern rmt_dev_t RMT;

#ifdef __cplusplus
}
#endif
