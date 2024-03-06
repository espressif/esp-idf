/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: FIFO R/W registers */
/** Type of chndata register
 *  The read and write data register for channel 0 by APB FIFO access.
 */
typedef union {
    struct {
        /** chndata : HRO; bitpos: [31:0]; default: 0;
         *  Read and write data for channel n via APB FIFO.
         */
        uint32_t chndata:32;
    };
    uint32_t val;
} rmt_chndata_reg_t;


/** Group: Configuration registers */
/** Type of chnconf0 register
 *  Configuration register 0 for channel 0
 */
typedef union {
    struct {
        /** tx_start_chn : WT; bitpos: [0]; default: 0;
         *  Configures whether to enable sending data in channel n. \\
         *  0: No effect\\
         *  1: Enable\\
         */
        uint32_t tx_start_chn:1;
        /** mem_rd_rst_chn : WT; bitpos: [1]; default: 0;
         *  Configures whether to reset RAM read address accessed by the transmitter for
         *  channel n. \\
         *  0: No effect\\
         *  1: Reset\\
         */
        uint32_t mem_rd_rst_chn:1;
        /** apb_mem_rst_chn : WT; bitpos: [2]; default: 0;
         *  Configures whether to reset RAM W/R address accessed by APB FIFO for channel n. \\
         *  0: No effect\\
         *  1: Reset\\
         */
        uint32_t apb_mem_rst_chn:1;
        /** tx_conti_mode_chn : R/W; bitpos: [3]; default: 0;
         *  Configures whether to enable continuous TX mode for channel n. \\
         *  0: No Effect\\
         *  1: Enable\\
         *  In this mode, the transmitter starts transmission from the first data. If an
         *  end-marker is encountered, the transmitter starts transmitting data from the first
         *  data again. if no end-marker is encountered, the transmitter starts transmitting
         *  the first data again when the last data is transmitted.\\
         */
        uint32_t tx_conti_mode_chn:1;
        /** mem_tx_wrap_en_chn : R/W; bitpos: [4]; default: 0;
         *  Configures whether to enable wrap TX mode for channel n. \\
         *  0: No effect\\
         *  1: Enable\\
         *  In this mode, if the TX data size is larger than the channel's RAM block size, the
         *  transmitter continues transmitting the first data to the last data in loops.\\
         */
        uint32_t mem_tx_wrap_en_chn:1;
        /** idle_out_lv_chn : R/W; bitpos: [5]; default: 0;
         *  Configures the level of output signal for channel n when the transmitter is in idle
         *  state.
         */
        uint32_t idle_out_lv_chn:1;
        /** idle_out_en_chn : R/W; bitpos: [6]; default: 0;
         *  Configures whether to enable the output for channel n in idle state. \\
         *  0: No effect\\
         *  1: Enable\\
         */
        uint32_t idle_out_en_chn:1;
        /** tx_stop_chn : R/W/SC; bitpos: [7]; default: 0;
         *  Configures whether to stop the transmitter of channel n sending data out. \\
         *  0: No effect\\
         *  1: Stop\\
         */
        uint32_t tx_stop_chn:1;
        /** div_cnt_chn : R/W; bitpos: [15:8]; default: 2;
         *  Configures the divider for clock of channel n. \\
         *  Measurement unit: rmt_sclk\\
         */
        uint32_t div_cnt_chn:8;
        /** mem_size_chn : R/W; bitpos: [18:16]; default: 1;
         *  Configures the maximum number of memory blocks allocated to channel n.
         */
        uint32_t mem_size_chn:3;
        uint32_t reserved_19:1;
        /** carrier_eff_en_chn : R/W; bitpos: [20]; default: 1;
         *  Configures whether to add carrier modulation on the output signal only at
         *  data-sending state for channel n. \\
         *  0: Add carrier modulation on the output signal at data-sending state and idle state
         *  for channel n\\
         *  1: Add carrier modulation on the output signal only at data-sending state for
         *  channel n\\
         *  Only valid when RMT_CARRIER_EN_CHn is 1.\\
         */
        uint32_t carrier_eff_en_chn:1;
        /** carrier_en_chn : R/W; bitpos: [21]; default: 1;
         *  Configures whether to enable the carrier modulation on output signal for channel n.
         *  \\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t carrier_en_chn:1;
        /** carrier_out_lv_chn : R/W; bitpos: [22]; default: 1;
         *  Configures the position of carrier wave for channel n. \\
         *  0: Add carrier wave on low level\\
         *  1: Add carrier wave on high level\\
         */
        uint32_t carrier_out_lv_chn:1;
        /** afifo_rst_chn : WT; bitpos: [23]; default: 0;
         *  Reserved
         */
        uint32_t afifo_rst_chn:1;
        /** conf_update_chn : WT; bitpos: [24]; default: 0;
         *  Synchronization bit for channel n.
         */
        uint32_t conf_update_chn:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} rmt_chnconf0_reg_t;

/** Type of chmconf0 register
 *  Configuration register 0 for channel 2
 */
typedef union {
    struct {
        /** div_cnt_chm : R/W; bitpos: [7:0]; default: 2;
         *  Configures the clock divider of channel m. \\
         *  Measurement unit: rmt_sclk\\
         */
        uint32_t div_cnt_chm:8;
        /** idle_thres_chm : R/W; bitpos: [22:8]; default: 32767;
         *  Configures RX threshold. \\
         *  When no edge is detected on the input signal for continuous clock cycles longer
         *  than this field value, the receiver stops receiving data.\\
         *  Measurement unit: clk_div\\
         */
        uint32_t idle_thres_chm:15;
        /** mem_size_chm : R/W; bitpos: [25:23]; default: 1;
         *  Configures the maximum number of memory blocks allocated to channel m.
         */
        uint32_t mem_size_chm:3;
        uint32_t reserved_26:2;
        /** carrier_en_chm : R/W; bitpos: [28]; default: 1;
         *  Configures whether to enable carrier modulation on output signal for channel m. \\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t carrier_en_chm:1;
        /** carrier_out_lv_chm : R/W; bitpos: [29]; default: 1;
         *  Configures the position of carrier wave for channel m. \\
         *  0: Add carrier wave on low level\\
         *  1: Add carrier wave on high level\\
         */
        uint32_t carrier_out_lv_chm:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} rmt_chmconf0_reg_t;

/** Type of chmconf1 register
 *  Configuration register 1 for channel 2
 */
typedef union {
    struct {
        /** rx_en_chm : R/W; bitpos: [0]; default: 0;
         *  Configures whether to enable the receiver to start receiving data in channel m. \\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t rx_en_chm:1;
        /** mem_wr_rst_chm : WT; bitpos: [1]; default: 0;
         *  Configures whether to reset RAM write address accessed by the receiver for channel
         *  m. \\
         *  0: No effect\\
         *  1: Reset\\
         */
        uint32_t mem_wr_rst_chm:1;
        /** apb_mem_rst_chm : WT; bitpos: [2]; default: 0;
         *  Configures whether to reset RAM W/R address accessed by APB FIFO for channel m. \\
         *  0: No effect\\
         *  1: Reset\\
         */
        uint32_t apb_mem_rst_chm:1;
        /** mem_owner_chm : R/W/SC; bitpos: [3]; default: 1;
         *  Configures the ownership of channel m's RAM block. \\
         *  0: APB bus is using the RAM\\
         *  1: Receiver is using the RAM\\
         */
        uint32_t mem_owner_chm:1;
        /** rx_filter_en_chm : R/W; bitpos: [4]; default: 0;
         *  Configures whether to enable the receiver's filter for channel m. \\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t rx_filter_en_chm:1;
        /** rx_filter_thres_chm : R/W; bitpos: [12:5]; default: 15;
         *  Configures whether the receiver, when receiving data, ignores the input pulse when
         *  its width is shorter than this register value in units of rmt_sclk cycles. \\
         *  0: No effect\\
         *  1: Reset\\
         */
        uint32_t rx_filter_thres_chm:8;
        /** mem_rx_wrap_en_chm : R/W; bitpos: [13]; default: 0;
         *  Configures whether to enable wrap RX mode for channel m. \\
         *  0: Disable\\
         *  1: Enable\\
         *  In this mode, if the RX data size is larger than channel m's RAM block size, the
         *  receiver stores the RX data from the first address to the last address in loops.\\
         */
        uint32_t mem_rx_wrap_en_chm:1;
        /** afifo_rst_chm : WT; bitpos: [14]; default: 0;
         *  Reserved
         */
        uint32_t afifo_rst_chm:1;
        /** conf_update_chm : WT; bitpos: [15]; default: 0;
         *  Synchronization bit for channel m.
         */
        uint32_t conf_update_chm:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} rmt_chmconf1_reg_t;

/** Type of sys_conf register
 *  Configuration register for RMT APB
 */
typedef union {
    struct {
        /** apb_fifo_mask : R/W; bitpos: [0]; default: 0;
         *  Configures the memory access mode. \\
         *  0: Access memory by FIFO\\
         *  1: Access memory directly\\
         */
        uint32_t apb_fifo_mask:1;
        /** mem_clk_force_on : R/W; bitpos: [1]; default: 0;
         *  Configures whether to enable the clock for RMT memory. \\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t mem_clk_force_on:1;
        /** mem_force_pd : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down RMT memory.
         */
        uint32_t mem_force_pd:1;
        /** mem_force_pu : R/W; bitpos: [3]; default: 0;
         *  1: Disable RMT memory light sleep power down function. 0: Power down RMT memory
         *  when RMT is in light sleep mode.
         */
        uint32_t mem_force_pu:1;
        /** sclk_div_num : R/W; bitpos: [11:4]; default: 1;
         *  the integral part of the fractional divisor
         */
        uint32_t sclk_div_num:8;
        /** sclk_div_a : R/W; bitpos: [17:12]; default: 0;
         *  the numerator of the fractional part of the fractional divisor
         */
        uint32_t sclk_div_a:6;
        /** sclk_div_b : R/W; bitpos: [23:18]; default: 0;
         *  the denominator of the fractional part of the fractional divisor
         */
        uint32_t sclk_div_b:6;
        /** sclk_sel : R/W; bitpos: [25:24]; default: 1;
         *  choose the clock source of rmt_sclk. 1:CLK_80Mhz,2:CLK_FOSC, 3:XTAL
         */
        uint32_t sclk_sel:2;
        /** sclk_active : R/W; bitpos: [26]; default: 1;
         *  rmt_sclk switch
         */
        uint32_t sclk_active:1;
        uint32_t reserved_27:4;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  Configures whether to enable signal of RMT register clock gate. \\
         *  0: Power down the drive clock of registers\\
         *  1: Power up the drive clock of registers\\
         */
        uint32_t clk_en:1;
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
        uint32_t ref_cnt_rst_ch0:1;
        /** ref_cnt_rst_ch1 : WT; bitpos: [1]; default: 0;
         *  This register is used to reset the clock divider of CHANNEL1.
         */
        uint32_t ref_cnt_rst_ch1:1;
        /** ref_cnt_rst_ch2 : WT; bitpos: [2]; default: 0;
         *  This register is used to reset the clock divider of CHANNEL2.
         */
        uint32_t ref_cnt_rst_ch2:1;
        /** ref_cnt_rst_ch3 : WT; bitpos: [3]; default: 0;
         *  This register is used to reset the clock divider of CHANNEL3.
         */
        uint32_t ref_cnt_rst_ch3:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} rmt_ref_cnt_rst_reg_t;


/** Group: Status registers */
/** Type of chnstatus register
 *  Channel 0 status register
 */
typedef union {
    struct {
        /** mem_raddr_ex_chn : RO; bitpos: [8:0]; default: 0;
         *  Represents the memory address offset when transmitter of channel n is using the RAM.
         */
        uint32_t mem_raddr_ex_chn:9;
        /** state_chn : RO; bitpos: [11:9]; default: 0;
         *  Represents the FSM status of channel n.
         */
        uint32_t state_chn:3;
        /** apb_mem_waddr_chn : RO; bitpos: [20:12]; default: 0;
         *  Represents the memory address offset when writes RAM over APB bus.
         */
        uint32_t apb_mem_waddr_chn:9;
        /** apb_mem_rd_err_chn : RO; bitpos: [21]; default: 0;
         *  Represents whether the offset address exceeds memory size when reading via APB bus.
         *  \\
         *  0: Not exceed\\
         *  1: Exceed\\
         */
        uint32_t apb_mem_rd_err_chn:1;
        /** mem_empty_chn : RO; bitpos: [22]; default: 0;
         *  Represents whether the TX data size exceeds the memory size and the wrap TX mode is
         *  disabled. \\
         *  0: Not exceed\\
         *  1: Exceed\\
         */
        uint32_t mem_empty_chn:1;
        /** apb_mem_wr_err_chn : RO; bitpos: [23]; default: 0;
         *  Represents whether the offset address exceeds memory size (overflows) when writes
         *  via APB bus. \\
         *  0: Not exceed\\
         *  1: Exceed\\
         */
        uint32_t apb_mem_wr_err_chn:1;
        /** apb_mem_raddr_chn : RO; bitpos: [31:24]; default: 0;
         *  Represents the memory address offset when reading RAM over APB bus.
         */
        uint32_t apb_mem_raddr_chn:8;
    };
    uint32_t val;
} rmt_chnstatus_reg_t;

/** Type of chmstatus register
 *  Channel 2 status register
 */
typedef union {
    struct {
        /** mem_waddr_ex_chm : RO; bitpos: [8:0]; default: 0;
         *  Represents the memory address offset when receiver of channel m is using the RAM.
         */
        uint32_t mem_waddr_ex_chm:9;
        uint32_t reserved_9:3;
        /** apb_mem_raddr_chm : RO; bitpos: [20:12]; default: 0;
         *  Represents the memory address offset when reads RAM over APB bus.
         */
        uint32_t apb_mem_raddr_chm:9;
        uint32_t reserved_21:1;
        /** state_chm : RO; bitpos: [24:22]; default: 0;
         *  Represents the FSM status of channel m.
         */
        uint32_t state_chm:3;
        /** mem_owner_err_chm : RO; bitpos: [25]; default: 0;
         *  Represents whether the ownership of memory block is wrong. \\
         *  0: The ownership of memory block is correct\\
         *  1: The ownership of memory block is wrong\\
         */
        uint32_t mem_owner_err_chm:1;
        /** mem_full_chm : RO; bitpos: [26]; default: 0;
         *  Represents whether the receiver receives more data than the memory can fit. \\
         *  0: The receiver does not receive more data than the memory can fit\\
         *  1: The receiver receives more data than the memory can fit\\
         */
        uint32_t mem_full_chm:1;
        /** apb_mem_rd_err_chm : RO; bitpos: [27]; default: 0;
         *  Represents whether the offset address exceeds memory size (overflows) when reads
         *  RAM via APB bus. \\
         *  0: Not exceed\\
         *  1: Exceed\\
         */
        uint32_t apb_mem_rd_err_chm:1;
        uint32_t reserved_28:4;
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
        uint32_t ch0_tx_end_int_raw:1;
        /** ch1_tx_end_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The interrupt raw bit for CHANNEL1. Triggered when transmission done.
         */
        uint32_t ch1_tx_end_int_raw:1;
        /** ch2_rx_end_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The interrupt raw bit for CHANNEL2. Triggered when reception done.
         */
        uint32_t ch2_rx_end_int_raw:1;
        /** ch3_rx_end_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The interrupt raw bit for CHANNEL3. Triggered when reception done.
         */
        uint32_t ch3_rx_end_int_raw:1;
        /** ch0_err_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The interrupt raw bit for CHANNEL$m. Triggered when error occurs.
         */
        uint32_t ch0_err_int_raw:1;
        /** ch1_err_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The interrupt raw bit for CHANNEL$m. Triggered when error occurs.
         */
        uint32_t ch1_err_int_raw:1;
        /** ch2_err_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The interrupt raw bit for CHANNEL$m. Triggered when error occurs.
         */
        uint32_t ch2_err_int_raw:1;
        /** ch3_err_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The interrupt raw bit for CHANNEL$m. Triggered when error occurs.
         */
        uint32_t ch3_err_int_raw:1;
        /** ch0_tx_thr_event_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The interrupt raw bit for CHANNEL0. Triggered when transmitter sent more data than
         *  configured value.
         */
        uint32_t ch0_tx_thr_event_int_raw:1;
        /** ch1_tx_thr_event_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  The interrupt raw bit for CHANNEL1. Triggered when transmitter sent more data than
         *  configured value.
         */
        uint32_t ch1_tx_thr_event_int_raw:1;
        /** ch2_rx_thr_event_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  The interrupt raw bit for CHANNEL2. Triggered when receiver receive more data than
         *  configured value.
         */
        uint32_t ch2_rx_thr_event_int_raw:1;
        /** ch3_rx_thr_event_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  The interrupt raw bit for CHANNEL3. Triggered when receiver receive more data than
         *  configured value.
         */
        uint32_t ch3_rx_thr_event_int_raw:1;
        /** ch0_tx_loop_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  The interrupt raw bit for CHANNEL0. Triggered when the loop count reaches the
         *  configured threshold value.
         */
        uint32_t ch0_tx_loop_int_raw:1;
        /** ch1_tx_loop_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  The interrupt raw bit for CHANNEL1. Triggered when the loop count reaches the
         *  configured threshold value.
         */
        uint32_t ch1_tx_loop_int_raw:1;
        uint32_t reserved_14:18;
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
        uint32_t ch0_tx_end_int_st:1;
        /** ch1_tx_end_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status bit for CH1_TX_END_INT.
         */
        uint32_t ch1_tx_end_int_st:1;
        /** ch2_rx_end_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status bit for CH2_RX_END_INT.
         */
        uint32_t ch2_rx_end_int_st:1;
        /** ch3_rx_end_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status bit for CH3_RX_END_INT.
         */
        uint32_t ch3_rx_end_int_st:1;
        /** ch0_err_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status bit for CH$n_ERR_INT.
         */
        uint32_t ch0_err_int_st:1;
        /** ch1_err_int_st : RO; bitpos: [5]; default: 0;
         *  The masked interrupt status bit for CH$n_ERR_INT.
         */
        uint32_t ch1_err_int_st:1;
        /** ch2_err_int_st : RO; bitpos: [6]; default: 0;
         *  The masked interrupt status bit for CH$n_ERR_INT.
         */
        uint32_t ch2_err_int_st:1;
        /** ch3_err_int_st : RO; bitpos: [7]; default: 0;
         *  The masked interrupt status bit for CH$n_ERR_INT.
         */
        uint32_t ch3_err_int_st:1;
        /** ch0_tx_thr_event_int_st : RO; bitpos: [8]; default: 0;
         *  The masked interrupt status bit for CH0_TX_THR_EVENT_INT.
         */
        uint32_t ch0_tx_thr_event_int_st:1;
        /** ch1_tx_thr_event_int_st : RO; bitpos: [9]; default: 0;
         *  The masked interrupt status bit for CH1_TX_THR_EVENT_INT.
         */
        uint32_t ch1_tx_thr_event_int_st:1;
        /** ch2_rx_thr_event_int_st : RO; bitpos: [10]; default: 0;
         *  The masked interrupt status bit for CH2_RX_THR_EVENT_INT.
         */
        uint32_t ch2_rx_thr_event_int_st:1;
        /** ch3_rx_thr_event_int_st : RO; bitpos: [11]; default: 0;
         *  The masked interrupt status bit for CH3_RX_THR_EVENT_INT.
         */
        uint32_t ch3_rx_thr_event_int_st:1;
        /** ch0_tx_loop_int_st : RO; bitpos: [12]; default: 0;
         *  The masked interrupt status bit for CH0_TX_LOOP_INT.
         */
        uint32_t ch0_tx_loop_int_st:1;
        /** ch1_tx_loop_int_st : RO; bitpos: [13]; default: 0;
         *  The masked interrupt status bit for CH1_TX_LOOP_INT.
         */
        uint32_t ch1_tx_loop_int_st:1;
        uint32_t reserved_14:18;
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
        uint32_t ch0_tx_end_int_ena:1;
        /** ch1_tx_end_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for CH1_TX_END_INT.
         */
        uint32_t ch1_tx_end_int_ena:1;
        /** ch2_rx_end_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for CH2_RX_END_INT.
         */
        uint32_t ch2_rx_end_int_ena:1;
        /** ch3_rx_end_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for CH3_RX_END_INT.
         */
        uint32_t ch3_rx_end_int_ena:1;
        /** ch0_err_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for CH$n_ERR_INT.
         */
        uint32_t ch0_err_int_ena:1;
        /** ch1_err_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit for CH$n_ERR_INT.
         */
        uint32_t ch1_err_int_ena:1;
        /** ch2_err_int_ena : R/W; bitpos: [6]; default: 0;
         *  The interrupt enable bit for CH$n_ERR_INT.
         */
        uint32_t ch2_err_int_ena:1;
        /** ch3_err_int_ena : R/W; bitpos: [7]; default: 0;
         *  The interrupt enable bit for CH$n_ERR_INT.
         */
        uint32_t ch3_err_int_ena:1;
        /** ch0_tx_thr_event_int_ena : R/W; bitpos: [8]; default: 0;
         *  The interrupt enable bit for CH0_TX_THR_EVENT_INT.
         */
        uint32_t ch0_tx_thr_event_int_ena:1;
        /** ch1_tx_thr_event_int_ena : R/W; bitpos: [9]; default: 0;
         *  The interrupt enable bit for CH1_TX_THR_EVENT_INT.
         */
        uint32_t ch1_tx_thr_event_int_ena:1;
        /** ch2_rx_thr_event_int_ena : R/W; bitpos: [10]; default: 0;
         *  The interrupt enable bit for CH2_RX_THR_EVENT_INT.
         */
        uint32_t ch2_rx_thr_event_int_ena:1;
        /** ch3_rx_thr_event_int_ena : R/W; bitpos: [11]; default: 0;
         *  The interrupt enable bit for CH3_RX_THR_EVENT_INT.
         */
        uint32_t ch3_rx_thr_event_int_ena:1;
        /** ch0_tx_loop_int_ena : R/W; bitpos: [12]; default: 0;
         *  The interrupt enable bit for CH0_TX_LOOP_INT.
         */
        uint32_t ch0_tx_loop_int_ena:1;
        /** ch1_tx_loop_int_ena : R/W; bitpos: [13]; default: 0;
         *  The interrupt enable bit for CH1_TX_LOOP_INT.
         */
        uint32_t ch1_tx_loop_int_ena:1;
        uint32_t reserved_14:18;
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
        uint32_t ch0_tx_end_int_clr:1;
        /** ch1_tx_end_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear theCH1_TX_END_INT interrupt.
         */
        uint32_t ch1_tx_end_int_clr:1;
        /** ch2_rx_end_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear theCH2_RX_END_INT interrupt.
         */
        uint32_t ch2_rx_end_int_clr:1;
        /** ch3_rx_end_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear theCH3_RX_END_INT interrupt.
         */
        uint32_t ch3_rx_end_int_clr:1;
        /** ch0_err_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear theCH$n_ERR_INT interrupt.
         */
        uint32_t ch0_err_int_clr:1;
        /** ch1_err_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear theCH$n_ERR_INT interrupt.
         */
        uint32_t ch1_err_int_clr:1;
        /** ch2_err_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear theCH$n_ERR_INT interrupt.
         */
        uint32_t ch2_err_int_clr:1;
        /** ch3_err_int_clr : WT; bitpos: [7]; default: 0;
         *  Set this bit to clear theCH$n_ERR_INT interrupt.
         */
        uint32_t ch3_err_int_clr:1;
        /** ch0_tx_thr_event_int_clr : WT; bitpos: [8]; default: 0;
         *  Set this bit to clear theCH0_TX_THR_EVENT_INT interrupt.
         */
        uint32_t ch0_tx_thr_event_int_clr:1;
        /** ch1_tx_thr_event_int_clr : WT; bitpos: [9]; default: 0;
         *  Set this bit to clear theCH1_TX_THR_EVENT_INT interrupt.
         */
        uint32_t ch1_tx_thr_event_int_clr:1;
        /** ch2_rx_thr_event_int_clr : WT; bitpos: [10]; default: 0;
         *  Set this bit to clear theCH2_RX_THR_EVENT_INT interrupt.
         */
        uint32_t ch2_rx_thr_event_int_clr:1;
        /** ch3_rx_thr_event_int_clr : WT; bitpos: [11]; default: 0;
         *  Set this bit to clear theCH3_RX_THR_EVENT_INT interrupt.
         */
        uint32_t ch3_rx_thr_event_int_clr:1;
        /** ch0_tx_loop_int_clr : WT; bitpos: [12]; default: 0;
         *  Set this bit to clear theCH0_TX_LOOP_INT interrupt.
         */
        uint32_t ch0_tx_loop_int_clr:1;
        /** ch1_tx_loop_int_clr : WT; bitpos: [13]; default: 0;
         *  Set this bit to clear theCH1_TX_LOOP_INT interrupt.
         */
        uint32_t ch1_tx_loop_int_clr:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} rmt_int_clr_reg_t;


/** Group: Carrier wave duty cycle registers */
/** Type of chncarrier_duty register
 *  Duty cycle configuration register for channel 0
 */
typedef union {
    struct {
        /** carrier_low_chn : R/W; bitpos: [15:0]; default: 64;
         *  Configures carrier wave's low level clock period for channel n. \\
         *  Measurement unit: rmt_sclk\\
         */
        uint32_t carrier_low_chn:16;
        /** carrier_high_chn : R/W; bitpos: [31:16]; default: 64;
         *  Configures carrier wave's high level clock period for channel n. \\
         *  Measurement unit: rmt_sclk\\
         */
        uint32_t carrier_high_chn:16;
    };
    uint32_t val;
} rmt_chncarrier_duty_reg_t;

/** Type of chm_rx_carrier_rm register
 *  Carrier remove register for channel 2
 */
typedef union {
    struct {
        /** carrier_low_thres_chm : R/W; bitpos: [15:0]; default: 0;
         *  Configures the low level period in a carrier modulation mode for channel m. \\
         *  The low level period in a carrier modulation mode is (RMT_CARRIER_LOW_THRES_CHm +
         *  1) for channel m. \\
         *  Measurement unit: clk_div\\
         */
        uint32_t carrier_low_thres_chm:16;
        /** carrier_high_thres_chm : R/W; bitpos: [31:16]; default: 0;
         *  Configures the high level period in a carrier modulation mode for channel m. \\
         *  The high level period in a carrier modulation mode is
         *  (REG_RMT_REG_CARRIER_HIGH_THRES_CHm + 1) for channel m.\\
         *  Measurement unit: clk_div\\
         */
        uint32_t carrier_high_thres_chm:16;
    };
    uint32_t val;
} rmt_chm_rx_carrier_rm_reg_t;


/** Group: Tx event configuration registers */
/** Type of chn_tx_lim register
 *  Configuration register for channel 0 TX event
 */
typedef union {
    struct {
        /** tx_lim_chn : R/W; bitpos: [8:0]; default: 128;
         *  Configures the maximum entries that channel n can send out.
         */
        uint32_t tx_lim_chn:9;
        /** tx_loop_num_chn : R/W; bitpos: [18:9]; default: 0;
         *  Configures the maximum loop count when Continuous TX mode is valid.
         */
        uint32_t tx_loop_num_chn:10;
        /** tx_loop_cnt_en_chn : R/W; bitpos: [19]; default: 0;
         *  Configures whether to enable loop count. \\
         *  0: No effect\\
         *  1: Enable\\
         */
        uint32_t tx_loop_cnt_en_chn:1;
        /** loop_count_reset_chn : WT; bitpos: [20]; default: 0;
         *  Configures whether to reset the loop count when tx_conti_mode is valid. \\
         *  0: No effect\\
         *  1: Reset\\
         */
        uint32_t loop_count_reset_chn:1;
        /** loop_stop_en_chn : R/W; bitpos: [21]; default: 0;
         *  Configures whether to enable the loop send stop function after the loop counter
         *  counts to loop number for channel n. \\
         *  0: No effect\\
         *  1: Enable\\
         */
        uint32_t loop_stop_en_chn:1;
        uint32_t reserved_22:10;
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
        uint32_t tx_sim_ch0:1;
        /** tx_sim_ch1 : R/W; bitpos: [1]; default: 0;
         *  Set this bit to enable CHANNEL1 to start sending data synchronously with other
         *  enabled channels.
         */
        uint32_t tx_sim_ch1:1;
        /** tx_sim_en : R/W; bitpos: [2]; default: 0;
         *  This register is used to enable multiple of channels to start sending data
         *  synchronously.
         */
        uint32_t tx_sim_en:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} rmt_tx_sim_reg_t;


/** Group: Rx event configuration registers */
/** Type of chm_rx_lim register
 *  Configuration register for channel 2 RX event
 */
typedef union {
    struct {
        /** rx_lim_chm : R/W; bitpos: [8:0]; default: 128;
         *  This register is used to configure the maximum entries that CHANNELm can receive.
         */
        uint32_t rx_lim_chm:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} rmt_chm_rx_lim_reg_t;


/** Group: Version register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 34636307;
         *  This is the version register.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} rmt_date_reg_t;


typedef struct {
    volatile rmt_chndata_reg_t chndata[4];
    volatile rmt_chnconf0_reg_t chnconf0[2];
    volatile rmt_chmconf0_reg_t ch2conf0;
    volatile rmt_chmconf1_reg_t ch2conf1;
    volatile rmt_chmconf0_reg_t ch3conf0;
    volatile rmt_chmconf1_reg_t ch3conf1;
    volatile rmt_chnstatus_reg_t chnstatus[2];
    volatile rmt_chmstatus_reg_t chmstatus[2];
    volatile rmt_int_raw_reg_t int_raw;
    volatile rmt_int_st_reg_t int_st;
    volatile rmt_int_ena_reg_t int_ena;
    volatile rmt_int_clr_reg_t int_clr;
    volatile rmt_chncarrier_duty_reg_t chncarrier_duty[2];
    volatile rmt_chm_rx_carrier_rm_reg_t chm_rx_carrier_rm[2];
    volatile rmt_chn_tx_lim_reg_t chn_tx_lim[2];
    volatile rmt_chm_rx_lim_reg_t chm_rx_lim[2];
    volatile rmt_sys_conf_reg_t sys_conf;
    volatile rmt_tx_sim_reg_t tx_sim;
    volatile rmt_ref_cnt_rst_reg_t ref_cnt_rst;
    uint32_t reserved_074[22];
    volatile rmt_date_reg_t date;
} rmt_dev_t;

extern rmt_dev_t RMT;

#ifndef __cplusplus
_Static_assert(sizeof(rmt_dev_t) == 0xd0, "Invalid size of rmt_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
