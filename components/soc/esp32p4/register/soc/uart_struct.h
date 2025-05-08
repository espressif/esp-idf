/**
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: FIFO Configuration */
/** Type of fifo register
 *  FIFO data register
 */
typedef union {
    struct {
        /** rxfifo_rd_byte : RO; bitpos: [7:0]; default: 0;
         *  UART $n accesses FIFO via this register.
         */
        uint32_t rxfifo_rd_byte:32;
    };
    uint32_t val;
} uart_fifo_reg_t;

/** Type of mem_conf register
 *  UART memory power configuration
 */
typedef union {
    struct {
        uint32_t reserved_0:25;
        /** mem_force_pd : R/W; bitpos: [25]; default: 0;
         *  Set this bit to force power down UART memory.
         */
        uint32_t mem_force_pd:1;
        /** mem_force_pu : R/W; bitpos: [26]; default: 0;
         *  Set this bit to force power up UART memory.
         */
        uint32_t mem_force_pu:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} uart_mem_conf_reg_t;

/** Type of tout_conf_sync register
 *  UART threshold and allocation configuration
 */
typedef union {
    struct {
        /** rx_tout_en : R/W; bitpos: [0]; default: 0;
         *  This is the enable bit for uart receiver's timeout function.
         */
        uint32_t rx_tout_en:1;
        /** rx_tout_flow_dis : R/W; bitpos: [1]; default: 0;
         *  Set this bit to stop accumulating idle_cnt when hardware flow control works.
         */
        uint32_t rx_tout_flow_dis:1;
        /** rx_tout_thrhd : R/W; bitpos: [11:2]; default: 10;
         *  This register is used to configure the threshold time that receiver takes to
         *  receive one byte. The rxfifo_tout_int interrupt will be trigger when the receiver
         *  takes more time to receive one byte with rx_tout_en set to 1.
         */
        uint32_t rx_tout_thrhd:10;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} uart_tout_conf_sync_reg_t;


/** Group: Interrupt Register */
/** Type of int_raw register
 *  Raw interrupt status
 */
typedef union {
    struct {
        /** rxfifo_full_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  This interrupt raw bit turns to high level when receiver receives more data than
         *  what rxfifo_full_thrhd specifies.
         */
        uint32_t rxfifo_full_int_raw:1;
        /** txfifo_empty_int_raw : R/WTC/SS; bitpos: [1]; default: 1;
         *  This interrupt raw bit turns to high level when the amount of data in Tx-FIFO is
         *  less than what txfifo_empty_thrhd specifies .
         */
        uint32_t txfifo_empty_int_raw:1;
        /** parity_err_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  This interrupt raw bit turns to high level when receiver detects a parity error in
         *  the data.
         */
        uint32_t parity_err_int_raw:1;
        /** frm_err_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  This interrupt raw bit turns to high level when receiver detects a data frame error
         *  .
         */
        uint32_t frm_err_int_raw:1;
        /** rxfifo_ovf_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  This interrupt raw bit turns to high level when receiver receives more data than
         *  the FIFO can store.
         */
        uint32_t rxfifo_ovf_int_raw:1;
        /** dsr_chg_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  This interrupt raw bit turns to high level when receiver detects the edge change of
         *  DSRn signal.
         */
        uint32_t dsr_chg_int_raw:1;
        /** cts_chg_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  This interrupt raw bit turns to high level when receiver detects the edge change of
         *  CTSn signal.
         */
        uint32_t cts_chg_int_raw:1;
        /** brk_det_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  This interrupt raw bit turns to high level when receiver detects a 0 after the stop
         *  bit.
         */
        uint32_t brk_det_int_raw:1;
        /** rxfifo_tout_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  This interrupt raw bit turns to high level when receiver takes more time than
         *  rx_tout_thrhd to receive a byte.
         */
        uint32_t rxfifo_tout_int_raw:1;
        /** sw_xon_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  This interrupt raw bit turns to high level when receiver receives Xon char when
         *  uart_sw_flow_con_en is set to 1.
         */
        uint32_t sw_xon_int_raw:1;
        /** sw_xoff_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  This interrupt raw bit turns to high level when receiver receives Xoff char when
         *  uart_sw_flow_con_en is set to 1.
         */
        uint32_t sw_xoff_int_raw:1;
        /** glitch_det_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  This interrupt raw bit turns to high level when receiver detects a glitch in the
         *  middle of a start bit.
         */
        uint32_t glitch_det_int_raw:1;
        /** tx_brk_done_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  This interrupt raw bit turns to high level when transmitter completes  sending
         *  NULL characters after all data in Tx-FIFO are sent.
         */
        uint32_t tx_brk_done_int_raw:1;
        /** tx_brk_idle_done_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  This interrupt raw bit turns to high level when transmitter has kept the shortest
         *  duration after sending the  last data.
         */
        uint32_t tx_brk_idle_done_int_raw:1;
        /** tx_done_int_raw : R/WTC/SS; bitpos: [14]; default: 0;
         *  This interrupt raw bit turns to high level when transmitter has send out all data
         *  in FIFO.
         */
        uint32_t tx_done_int_raw:1;
        /** rs485_parity_err_int_raw : R/WTC/SS; bitpos: [15]; default: 0;
         *  This interrupt raw bit turns to high level when receiver detects a parity error
         *  from the echo of transmitter in rs485 mode.
         */
        uint32_t rs485_parity_err_int_raw:1;
        /** rs485_frm_err_int_raw : R/WTC/SS; bitpos: [16]; default: 0;
         *  This interrupt raw bit turns to high level when receiver detects a data frame error
         *  from the echo of transmitter in rs485 mode.
         */
        uint32_t rs485_frm_err_int_raw:1;
        /** rs485_clash_int_raw : R/WTC/SS; bitpos: [17]; default: 0;
         *  This interrupt raw bit turns to high level when detects a clash between transmitter
         *  and receiver in rs485 mode.
         */
        uint32_t rs485_clash_int_raw:1;
        /** at_cmd_char_det_int_raw : R/WTC/SS; bitpos: [18]; default: 0;
         *  This interrupt raw bit turns to high level when receiver detects the configured
         *  at_cmd char.
         */
        uint32_t at_cmd_char_det_int_raw:1;
        /** wakeup_int_raw : R/WTC/SS; bitpos: [19]; default: 0;
         *  This interrupt raw bit turns to high level when input rxd edge changes more times
         *  than what reg_active_threshold specifies in light sleeping mode.
         */
        uint32_t wakeup_int_raw:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} uart_int_raw_reg_t;

/** Type of int_st register
 *  Masked interrupt status
 */
typedef union {
    struct {
        /** rxfifo_full_int_st : RO; bitpos: [0]; default: 0;
         *  This is the status bit for rxfifo_full_int_raw when rxfifo_full_int_ena is set to 1.
         */
        uint32_t rxfifo_full_int_st:1;
        /** txfifo_empty_int_st : RO; bitpos: [1]; default: 0;
         *  This is the status bit for  txfifo_empty_int_raw  when txfifo_empty_int_ena is set
         *  to 1.
         */
        uint32_t txfifo_empty_int_st:1;
        /** parity_err_int_st : RO; bitpos: [2]; default: 0;
         *  This is the status bit for parity_err_int_raw when parity_err_int_ena is set to 1.
         */
        uint32_t parity_err_int_st:1;
        /** frm_err_int_st : RO; bitpos: [3]; default: 0;
         *  This is the status bit for frm_err_int_raw when frm_err_int_ena is set to 1.
         */
        uint32_t frm_err_int_st:1;
        /** rxfifo_ovf_int_st : RO; bitpos: [4]; default: 0;
         *  This is the status bit for rxfifo_ovf_int_raw when rxfifo_ovf_int_ena is set to 1.
         */
        uint32_t rxfifo_ovf_int_st:1;
        /** dsr_chg_int_st : RO; bitpos: [5]; default: 0;
         *  This is the status bit for dsr_chg_int_raw when dsr_chg_int_ena is set to 1.
         */
        uint32_t dsr_chg_int_st:1;
        /** cts_chg_int_st : RO; bitpos: [6]; default: 0;
         *  This is the status bit for cts_chg_int_raw when cts_chg_int_ena is set to 1.
         */
        uint32_t cts_chg_int_st:1;
        /** brk_det_int_st : RO; bitpos: [7]; default: 0;
         *  This is the status bit for brk_det_int_raw when brk_det_int_ena is set to 1.
         */
        uint32_t brk_det_int_st:1;
        /** rxfifo_tout_int_st : RO; bitpos: [8]; default: 0;
         *  This is the status bit for rxfifo_tout_int_raw when rxfifo_tout_int_ena is set to 1.
         */
        uint32_t rxfifo_tout_int_st:1;
        /** sw_xon_int_st : RO; bitpos: [9]; default: 0;
         *  This is the status bit for sw_xon_int_raw when sw_xon_int_ena is set to 1.
         */
        uint32_t sw_xon_int_st:1;
        /** sw_xoff_int_st : RO; bitpos: [10]; default: 0;
         *  This is the status bit for sw_xoff_int_raw when sw_xoff_int_ena is set to 1.
         */
        uint32_t sw_xoff_int_st:1;
        /** glitch_det_int_st : RO; bitpos: [11]; default: 0;
         *  This is the status bit for glitch_det_int_raw when glitch_det_int_ena is set to 1.
         */
        uint32_t glitch_det_int_st:1;
        /** tx_brk_done_int_st : RO; bitpos: [12]; default: 0;
         *  This is the status bit for tx_brk_done_int_raw when tx_brk_done_int_ena is set to 1.
         */
        uint32_t tx_brk_done_int_st:1;
        /** tx_brk_idle_done_int_st : RO; bitpos: [13]; default: 0;
         *  This is the status bit for tx_brk_idle_done_int_raw when tx_brk_idle_done_int_ena
         *  is set to 1.
         */
        uint32_t tx_brk_idle_done_int_st:1;
        /** tx_done_int_st : RO; bitpos: [14]; default: 0;
         *  This is the status bit for tx_done_int_raw when tx_done_int_ena is set to 1.
         */
        uint32_t tx_done_int_st:1;
        /** rs485_parity_err_int_st : RO; bitpos: [15]; default: 0;
         *  This is the status bit for rs485_parity_err_int_raw when rs485_parity_int_ena is
         *  set to 1.
         */
        uint32_t rs485_parity_err_int_st:1;
        /** rs485_frm_err_int_st : RO; bitpos: [16]; default: 0;
         *  This is the status bit for rs485_frm_err_int_raw when rs485_fm_err_int_ena is set
         *  to 1.
         */
        uint32_t rs485_frm_err_int_st:1;
        /** rs485_clash_int_st : RO; bitpos: [17]; default: 0;
         *  This is the status bit for rs485_clash_int_raw when rs485_clash_int_ena is set to 1.
         */
        uint32_t rs485_clash_int_st:1;
        /** at_cmd_char_det_int_st : RO; bitpos: [18]; default: 0;
         *  This is the status bit for at_cmd_det_int_raw when at_cmd_char_det_int_ena is set
         *  to 1.
         */
        uint32_t at_cmd_char_det_int_st:1;
        /** wakeup_int_st : RO; bitpos: [19]; default: 0;
         *  This is the status bit for uart_wakeup_int_raw when uart_wakeup_int_ena is set to 1.
         */
        uint32_t wakeup_int_st:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} uart_int_st_reg_t;

/** Type of int_ena register
 *  Interrupt enable bits
 */
typedef union {
    struct {
        /** rxfifo_full_int_ena : R/W; bitpos: [0]; default: 0;
         *  This is the enable bit for rxfifo_full_int_st register.
         */
        uint32_t rxfifo_full_int_ena:1;
        /** txfifo_empty_int_ena : R/W; bitpos: [1]; default: 0;
         *  This is the enable bit for txfifo_empty_int_st register.
         */
        uint32_t txfifo_empty_int_ena:1;
        /** parity_err_int_ena : R/W; bitpos: [2]; default: 0;
         *  This is the enable bit for parity_err_int_st register.
         */
        uint32_t parity_err_int_ena:1;
        /** frm_err_int_ena : R/W; bitpos: [3]; default: 0;
         *  This is the enable bit for frm_err_int_st register.
         */
        uint32_t frm_err_int_ena:1;
        /** rxfifo_ovf_int_ena : R/W; bitpos: [4]; default: 0;
         *  This is the enable bit for rxfifo_ovf_int_st register.
         */
        uint32_t rxfifo_ovf_int_ena:1;
        /** dsr_chg_int_ena : R/W; bitpos: [5]; default: 0;
         *  This is the enable bit for dsr_chg_int_st register.
         */
        uint32_t dsr_chg_int_ena:1;
        /** cts_chg_int_ena : R/W; bitpos: [6]; default: 0;
         *  This is the enable bit for cts_chg_int_st register.
         */
        uint32_t cts_chg_int_ena:1;
        /** brk_det_int_ena : R/W; bitpos: [7]; default: 0;
         *  This is the enable bit for brk_det_int_st register.
         */
        uint32_t brk_det_int_ena:1;
        /** rxfifo_tout_int_ena : R/W; bitpos: [8]; default: 0;
         *  This is the enable bit for rxfifo_tout_int_st register.
         */
        uint32_t rxfifo_tout_int_ena:1;
        /** sw_xon_int_ena : R/W; bitpos: [9]; default: 0;
         *  This is the enable bit for sw_xon_int_st register.
         */
        uint32_t sw_xon_int_ena:1;
        /** sw_xoff_int_ena : R/W; bitpos: [10]; default: 0;
         *  This is the enable bit for sw_xoff_int_st register.
         */
        uint32_t sw_xoff_int_ena:1;
        /** glitch_det_int_ena : R/W; bitpos: [11]; default: 0;
         *  This is the enable bit for glitch_det_int_st register.
         */
        uint32_t glitch_det_int_ena:1;
        /** tx_brk_done_int_ena : R/W; bitpos: [12]; default: 0;
         *  This is the enable bit for tx_brk_done_int_st register.
         */
        uint32_t tx_brk_done_int_ena:1;
        /** tx_brk_idle_done_int_ena : R/W; bitpos: [13]; default: 0;
         *  This is the enable bit for tx_brk_idle_done_int_st register.
         */
        uint32_t tx_brk_idle_done_int_ena:1;
        /** tx_done_int_ena : R/W; bitpos: [14]; default: 0;
         *  This is the enable bit for tx_done_int_st register.
         */
        uint32_t tx_done_int_ena:1;
        /** rs485_parity_err_int_ena : R/W; bitpos: [15]; default: 0;
         *  This is the enable bit for rs485_parity_err_int_st register.
         */
        uint32_t rs485_parity_err_int_ena:1;
        /** rs485_frm_err_int_ena : R/W; bitpos: [16]; default: 0;
         *  This is the enable bit for rs485_parity_err_int_st register.
         */
        uint32_t rs485_frm_err_int_ena:1;
        /** rs485_clash_int_ena : R/W; bitpos: [17]; default: 0;
         *  This is the enable bit for rs485_clash_int_st register.
         */
        uint32_t rs485_clash_int_ena:1;
        /** at_cmd_char_det_int_ena : R/W; bitpos: [18]; default: 0;
         *  This is the enable bit for at_cmd_char_det_int_st register.
         */
        uint32_t at_cmd_char_det_int_ena:1;
        /** wakeup_int_ena : R/W; bitpos: [19]; default: 0;
         *  This is the enable bit for uart_wakeup_int_st register.
         */
        uint32_t wakeup_int_ena:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} uart_int_ena_reg_t;

/** Type of int_clr register
 *  Interrupt clear bits
 */
typedef union {
    struct {
        /** rxfifo_full_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the rxfifo_full_int_raw interrupt.
         */
        uint32_t rxfifo_full_int_clr:1;
        /** txfifo_empty_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear txfifo_empty_int_raw interrupt.
         */
        uint32_t txfifo_empty_int_clr:1;
        /** parity_err_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear parity_err_int_raw interrupt.
         */
        uint32_t parity_err_int_clr:1;
        /** frm_err_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear frm_err_int_raw interrupt.
         */
        uint32_t frm_err_int_clr:1;
        /** rxfifo_ovf_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear rxfifo_ovf_int_raw interrupt.
         */
        uint32_t rxfifo_ovf_int_clr:1;
        /** dsr_chg_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the dsr_chg_int_raw interrupt.
         */
        uint32_t dsr_chg_int_clr:1;
        /** cts_chg_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear the cts_chg_int_raw interrupt.
         */
        uint32_t cts_chg_int_clr:1;
        /** brk_det_int_clr : WT; bitpos: [7]; default: 0;
         *  Set this bit to clear the brk_det_int_raw interrupt.
         */
        uint32_t brk_det_int_clr:1;
        /** rxfifo_tout_int_clr : WT; bitpos: [8]; default: 0;
         *  Set this bit to clear the rxfifo_tout_int_raw interrupt.
         */
        uint32_t rxfifo_tout_int_clr:1;
        /** sw_xon_int_clr : WT; bitpos: [9]; default: 0;
         *  Set this bit to clear the sw_xon_int_raw interrupt.
         */
        uint32_t sw_xon_int_clr:1;
        /** sw_xoff_int_clr : WT; bitpos: [10]; default: 0;
         *  Set this bit to clear the sw_xoff_int_raw interrupt.
         */
        uint32_t sw_xoff_int_clr:1;
        /** glitch_det_int_clr : WT; bitpos: [11]; default: 0;
         *  Set this bit to clear the glitch_det_int_raw interrupt.
         */
        uint32_t glitch_det_int_clr:1;
        /** tx_brk_done_int_clr : WT; bitpos: [12]; default: 0;
         *  Set this bit to clear the tx_brk_done_int_raw interrupt..
         */
        uint32_t tx_brk_done_int_clr:1;
        /** tx_brk_idle_done_int_clr : WT; bitpos: [13]; default: 0;
         *  Set this bit to clear the tx_brk_idle_done_int_raw interrupt.
         */
        uint32_t tx_brk_idle_done_int_clr:1;
        /** tx_done_int_clr : WT; bitpos: [14]; default: 0;
         *  Set this bit to clear the tx_done_int_raw interrupt.
         */
        uint32_t tx_done_int_clr:1;
        /** rs485_parity_err_int_clr : WT; bitpos: [15]; default: 0;
         *  Set this bit to clear the rs485_parity_err_int_raw interrupt.
         */
        uint32_t rs485_parity_err_int_clr:1;
        /** rs485_frm_err_int_clr : WT; bitpos: [16]; default: 0;
         *  Set this bit to clear the rs485_frm_err_int_raw interrupt.
         */
        uint32_t rs485_frm_err_int_clr:1;
        /** rs485_clash_int_clr : WT; bitpos: [17]; default: 0;
         *  Set this bit to clear the rs485_clash_int_raw interrupt.
         */
        uint32_t rs485_clash_int_clr:1;
        /** at_cmd_char_det_int_clr : WT; bitpos: [18]; default: 0;
         *  Set this bit to clear the at_cmd_char_det_int_raw interrupt.
         */
        uint32_t at_cmd_char_det_int_clr:1;
        /** wakeup_int_clr : WT; bitpos: [19]; default: 0;
         *  Set this bit to clear the uart_wakeup_int_raw interrupt.
         */
        uint32_t wakeup_int_clr:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} uart_int_clr_reg_t;


/** Group: Configuration Register */
/** Type of clkdiv_sync register
 *  Clock divider configuration
 */
typedef union {
    struct {
        /** clkdiv : R/W; bitpos: [11:0]; default: 694;
         *  The integral part of the frequency divider factor.
         */
        uint32_t clkdiv:12;
        uint32_t reserved_12:8;
        /** clkdiv_frag : R/W; bitpos: [23:20]; default: 0;
         *  The decimal part of the frequency divider factor.
         */
        uint32_t clkdiv_frag:4;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} uart_clkdiv_sync_reg_t;

/** Type of rx_filt register
 *  Rx Filter configuration
 */
typedef union {
    struct {
        /** glitch_filt : R/W; bitpos: [7:0]; default: 8;
         *  when input pulse width is lower than this value the pulse is ignored.
         */
        uint32_t glitch_filt:8;
        /** glitch_filt_en : R/W; bitpos: [8]; default: 0;
         *  Set this bit to enable Rx signal filter.
         */
        uint32_t glitch_filt_en:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} uart_rx_filt_reg_t;

/** Type of conf0_sync register
 *  a
 */
typedef union {
    struct {
        /** parity : R/W; bitpos: [0]; default: 0;
         *  This register is used to configure the parity check mode.
         */
        uint32_t parity:1;
        /** parity_en : R/W; bitpos: [1]; default: 0;
         *  Set this bit to enable uart parity check.
         */
        uint32_t parity_en:1;
        /** bit_num : R/W; bitpos: [3:2]; default: 3;
         *  This register is used to set the length of data.
         */
        uint32_t bit_num:2;
        /** stop_bit_num : R/W; bitpos: [5:4]; default: 1;
         *  This register is used to set the length of  stop bit.
         */
        uint32_t stop_bit_num:2;
        /** txd_brk : R/W; bitpos: [6]; default: 0;
         *  Set this bit to enable transmitter to  send NULL when the process of sending data
         *  is done.
         */
        uint32_t txd_brk:1;
        /** irda_dplx : R/W; bitpos: [7]; default: 0;
         *  Set this bit to enable IrDA loopback mode.
         */
        uint32_t irda_dplx:1;
        /** irda_tx_en : R/W; bitpos: [8]; default: 0;
         *  This is the start enable bit for IrDA transmitter.
         */
        uint32_t irda_tx_en:1;
        /** irda_wctl : R/W; bitpos: [9]; default: 0;
         *  1'h1: The IrDA transmitter's 11th bit is the same as 10th bit. 1'h0: Set IrDA
         *  transmitter's 11th bit to 0.
         */
        uint32_t irda_wctl:1;
        /** irda_tx_inv : R/W; bitpos: [10]; default: 0;
         *  Set this bit to invert the level of  IrDA transmitter.
         */
        uint32_t irda_tx_inv:1;
        /** irda_rx_inv : R/W; bitpos: [11]; default: 0;
         *  Set this bit to invert the level of IrDA receiver.
         */
        uint32_t irda_rx_inv:1;
        /** loopback : R/W; bitpos: [12]; default: 0;
         *  Set this bit to enable uart loopback test mode.
         */
        uint32_t loopback:1;
        /** tx_flow_en : R/W; bitpos: [13]; default: 0;
         *  Set this bit to enable flow control function for transmitter.
         */
        uint32_t tx_flow_en:1;
        /** irda_en : R/W; bitpos: [14]; default: 0;
         *  Set this bit to enable IrDA protocol.
         */
        uint32_t irda_en:1;
        /** rxd_inv : R/W; bitpos: [15]; default: 0;
         *  Set this bit to inverse the level value of uart rxd signal.
         */
        uint32_t rxd_inv:1;
        /** txd_inv : R/W; bitpos: [16]; default: 0;
         *  Set this bit to inverse the level value of uart txd signal.
         */
        uint32_t txd_inv:1;
        /** dis_rx_dat_ovf : R/W; bitpos: [17]; default: 0;
         *  Disable UART Rx data overflow detect.
         */
        uint32_t dis_rx_dat_ovf:1;
        /** err_wr_mask : R/W; bitpos: [18]; default: 0;
         *  1'h1: Receiver stops storing data into FIFO when data is wrong. 1'h0: Receiver
         *  stores the data even if the  received data is wrong.
         */
        uint32_t err_wr_mask:1;
        /** autobaud_en : R/W; bitpos: [19]; default: 0;
         *  This is the enable bit for detecting baudrate.
         */
        uint32_t autobaud_en:1;
        /** mem_clk_en : R/W; bitpos: [20]; default: 0;
         *  UART memory clock gate enable signal.
         */
        uint32_t mem_clk_en:1;
        /** sw_rts : R/W; bitpos: [21]; default: 0;
         *  This register is used to configure the software rts signal which is used in
         *  software flow control.
         */
        uint32_t sw_rts:1;
        /** rxfifo_rst : R/W; bitpos: [22]; default: 0;
         *  Set this bit to reset the uart receive-FIFO.
         */
        uint32_t rxfifo_rst:1;
        /** txfifo_rst : R/W; bitpos: [23]; default: 0;
         *  Set this bit to reset the uart transmit-FIFO.
         */
        uint32_t txfifo_rst:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} uart_conf0_sync_reg_t;

/** Type of conf1 register
 *  Configuration register 1
 */
typedef union {
    struct {
        /** rxfifo_full_thrhd : R/W; bitpos: [7:0]; default: 96;
         *  It will produce rxfifo_full_int interrupt when receiver receives more data than
         *  this register value.
         */
        uint32_t rxfifo_full_thrhd:8;
        /** txfifo_empty_thrhd : R/W; bitpos: [15:8]; default: 96;
         *  It will produce txfifo_empty_int interrupt when the data amount in Tx-FIFO is less
         *  than this register value.
         */
        uint32_t txfifo_empty_thrhd:8;
        /** cts_inv : R/W; bitpos: [16]; default: 0;
         *  Set this bit to inverse the level value of uart cts signal.
         */
        uint32_t cts_inv:1;
        /** dsr_inv : R/W; bitpos: [17]; default: 0;
         *  Set this bit to inverse the level value of uart dsr signal.
         */
        uint32_t dsr_inv:1;
        /** rts_inv : R/W; bitpos: [18]; default: 0;
         *  Set this bit to inverse the level value of uart rts signal.
         */
        uint32_t rts_inv:1;
        /** dtr_inv : R/W; bitpos: [19]; default: 0;
         *  Set this bit to inverse the level value of uart dtr signal.
         */
        uint32_t dtr_inv:1;
        /** sw_dtr : R/W; bitpos: [20]; default: 0;
         *  This register is used to configure the software dtr signal which is used in
         *  software flow control.
         */
        uint32_t sw_dtr:1;
        /** clk_en : R/W; bitpos: [21]; default: 0;
         *  1'h1: Force clock on for register. 1'h0: Support clock only when application writes
         *  registers.
         */
        uint32_t clk_en:1;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} uart_conf1_reg_t;

/** Type of hwfc_conf_sync register
 *  Hardware flow-control configuration
 */
typedef union {
    struct {
        /** rx_flow_thrhd : R/W; bitpos: [7:0]; default: 0;
         *  This register is used to configure the maximum amount of data that can be received
         *  when hardware flow control works.
         */
        uint32_t rx_flow_thrhd:8;
        /** rx_flow_en : R/W; bitpos: [8]; default: 0;
         *  This is the flow enable bit for UART receiver.
         */
        uint32_t rx_flow_en:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} uart_hwfc_conf_sync_reg_t;

/** Type of sleep_conf0 register
 *  UART sleep configure register 0
 */
typedef union {
    struct {
        /** wk_char1 : R/W; bitpos: [7:0]; default: 0;
         *  This register restores the specified wake up char1 to wake up
         */
        uint32_t wk_char1:8;
        /** wk_char2 : R/W; bitpos: [15:8]; default: 0;
         *  This register restores the specified wake up char2 to wake up
         */
        uint32_t wk_char2:8;
        /** wk_char3 : R/W; bitpos: [23:16]; default: 0;
         *  This register restores the specified wake up char3 to wake up
         */
        uint32_t wk_char3:8;
        /** wk_char4 : R/W; bitpos: [31:24]; default: 0;
         *  This register restores the specified wake up char4 to wake up
         */
        uint32_t wk_char4:8;
    };
    uint32_t val;
} uart_sleep_conf0_reg_t;

/** Type of sleep_conf1 register
 *  UART sleep configure register 1
 */
typedef union {
    struct {
        /** wk_char0 : R/W; bitpos: [7:0]; default: 0;
         *  This register restores the specified char0 to wake up
         */
        uint32_t wk_char0:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} uart_sleep_conf1_reg_t;

/** Type of sleep_conf2 register
 *  UART sleep configure register 2
 */
typedef union {
    struct {
        /** active_threshold : R/W; bitpos: [9:0]; default: 240;
         *  The uart is activated from light sleeping mode when the input rxd edge changes more
         *  times than this register value.
         */
        uint32_t active_threshold:10;
        /** rx_wake_up_thrhd : R/W; bitpos: [17:10]; default: 1;
         *  In wake up mode 1 this field is used to set the received data number threshold to
         *  wake up chip.
         */
        uint32_t rx_wake_up_thrhd:8;
        /** wk_char_num : R/W; bitpos: [20:18]; default: 5;
         *  This register is used to select number of wake up char.
         */
        uint32_t wk_char_num:3;
        /** wk_char_mask : R/W; bitpos: [25:21]; default: 0;
         *  This register is used to mask  wake up char.
         */
        uint32_t wk_char_mask:5;
        /** wk_mode_sel : R/W; bitpos: [27:26]; default: 0;
         *  This register is used to select wake up mode. 0: RXD toggling to wake up. 1:
         *  received data number larger than
         */
        uint32_t wk_mode_sel:2;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} uart_sleep_conf2_reg_t;

/** Type of swfc_conf0_sync register
 *  Software flow-control character configuration
 */
typedef union {
    struct {
        /** xon_character : R/W; bitpos: [7:0]; default: 17;
         *  This register stores the Xon flow control char.
         */
        uint32_t xon_character:8;
        /** xoff_character : R/W; bitpos: [15:8]; default: 19;
         *  This register stores the Xoff flow control char.
         */
        uint32_t xoff_character:8;
        /** xon_xoff_still_send : R/W; bitpos: [16]; default: 0;
         *  In software flow control mode, UART Tx is disabled once UART Rx receives XOFF. In
         *  this status, UART Tx can not transmit XOFF even the received data number is larger
         *  than UART_XOFF_THRESHOLD. Set this bit to enable UART Tx can transmit XON/XOFF when
         *  UART Tx is disabled.
         */
        uint32_t xon_xoff_still_send:1;
        /** sw_flow_con_en : R/W; bitpos: [17]; default: 0;
         *  Set this bit to enable software flow control. It is used with register sw_xon or
         *  sw_xoff.
         */
        uint32_t sw_flow_con_en:1;
        /** xonoff_del : R/W; bitpos: [18]; default: 0;
         *  Set this bit to remove flow control char from the received data.
         */
        uint32_t xonoff_del:1;
        /** force_xon : R/W; bitpos: [19]; default: 0;
         *  Set this bit to enable the transmitter to go on sending data.
         */
        uint32_t force_xon:1;
        /** force_xoff : R/W; bitpos: [20]; default: 0;
         *  Set this bit to stop the  transmitter from sending data.
         */
        uint32_t force_xoff:1;
        /** send_xon : R/W/SS/SC; bitpos: [21]; default: 0;
         *  Set this bit to send Xon char. It is cleared by hardware automatically.
         */
        uint32_t send_xon:1;
        /** send_xoff : R/W/SS/SC; bitpos: [22]; default: 0;
         *  Set this bit to send Xoff char. It is cleared by hardware automatically.
         */
        uint32_t send_xoff:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} uart_swfc_conf0_sync_reg_t;

/** Type of swfc_conf1 register
 *  Software flow-control character configuration
 */
typedef union {
    struct {
        /** xon_threshold : R/W; bitpos: [7:0]; default: 0;
         *  When the data amount in Rx-FIFO is less than this register value with
         *  uart_sw_flow_con_en set to 1  it will send a Xon char.
         */
        uint32_t xon_threshold:8;
        /** xoff_threshold : R/W; bitpos: [15:8]; default: 224;
         *  When the data amount in Rx-FIFO is more than this register value with
         *  uart_sw_flow_con_en set to 1  it will send a Xoff char.
         */
        uint32_t xoff_threshold:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} uart_swfc_conf1_reg_t;

/** Type of txbrk_conf_sync register
 *  Tx Break character configuration
 */
typedef union {
    struct {
        /** tx_brk_num : R/W; bitpos: [7:0]; default: 10;
         *  This register is used to configure the number of 0 to be sent after the process of
         *  sending data is done. It is active when txd_brk is set to 1.
         */
        uint32_t tx_brk_num:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} uart_txbrk_conf_sync_reg_t;

/** Type of idle_conf_sync register
 *  Frame-end idle configuration
 */
typedef union {
    struct {
        /** rx_idle_thrhd : R/W; bitpos: [9:0]; default: 256;
         *  It will produce frame end signal when receiver takes more time to receive one byte
         *  data than this register value.
         */
        uint32_t rx_idle_thrhd:10;
        /** tx_idle_num : R/W; bitpos: [19:10]; default: 256;
         *  This register is used to configure the duration time between transfers.
         */
        uint32_t tx_idle_num:10;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} uart_idle_conf_sync_reg_t;

/** Type of rs485_conf_sync register
 *  RS485 mode configuration
 */
typedef union {
    struct {
        /** rs485_en : R/W; bitpos: [0]; default: 0;
         *  Set this bit to choose the rs485 mode.
         */
        uint32_t rs485_en:1;
        /** dl0_en : R/W; bitpos: [1]; default: 0;
         *  Set this bit to delay the stop bit by 1 bit.
         */
        uint32_t dl0_en:1;
        /** dl1_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to delay the stop bit by 1 bit.
         */
        uint32_t dl1_en:1;
        /** rs485tx_rx_en : R/W; bitpos: [3]; default: 0;
         *  Set this bit to enable receiver could receive data when the transmitter is
         *  transmitting data in rs485 mode.
         */
        uint32_t rs485tx_rx_en:1;
        /** rs485rxby_tx_en : R/W; bitpos: [4]; default: 0;
         *  1'h1: enable rs485 transmitter to send data when rs485 receiver line is busy.
         */
        uint32_t rs485rxby_tx_en:1;
        /** rs485_rx_dly_num : R/W; bitpos: [5]; default: 0;
         *  This register is used to delay the receiver's internal data signal.
         */
        uint32_t rs485_rx_dly_num:1;
        /** rs485_tx_dly_num : R/W; bitpos: [9:6]; default: 0;
         *  This register is used to delay the transmitter's internal data signal.
         */
        uint32_t rs485_tx_dly_num:4;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} uart_rs485_conf_sync_reg_t;

/** Type of clk_conf register
 *  UART core clock configuration
 */
typedef union {
    struct {
        uint32_t reserved_0:24;
        /** tx_sclk_en : R/W; bitpos: [24]; default: 1;
         *  Set this bit to enable UART Tx clock.
         */
        uint32_t tx_sclk_en:1;
        /** rx_sclk_en : R/W; bitpos: [25]; default: 1;
         *  Set this bit to enable UART Rx clock.
         */
        uint32_t rx_sclk_en:1;
        /** tx_rst_core : R/W; bitpos: [26]; default: 0;
         *  Write 1 then write 0 to this bit to reset UART Tx.
         */
        uint32_t tx_rst_core:1;
        /** rx_rst_core : R/W; bitpos: [27]; default: 0;
         *  Write 1 then write 0 to this bit to reset UART Rx.
         */
        uint32_t rx_rst_core:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} uart_clk_conf_reg_t;


/** Group: Status Register */
/** Type of status register
 *  UART status register
 */
typedef union {
    struct {
        /** rxfifo_cnt : RO; bitpos: [7:0]; default: 0;
         *  Stores the byte number of valid data in Rx-FIFO.
         */
        uint32_t rxfifo_cnt:8;
        uint32_t reserved_8:5;
        /** dsrn : RO; bitpos: [13]; default: 0;
         *  The register represent the level value of the internal uart dsr signal.
         */
        uint32_t dsrn:1;
        /** ctsn : RO; bitpos: [14]; default: 1;
         *  This register represent the level value of the internal uart cts signal.
         */
        uint32_t ctsn:1;
        /** rxd : RO; bitpos: [15]; default: 1;
         *  This register represent the  level value of the internal uart rxd signal.
         */
        uint32_t rxd:1;
        /** txfifo_cnt : RO; bitpos: [23:16]; default: 0;
         *  Stores the byte number of data in Tx-FIFO.
         */
        uint32_t txfifo_cnt:8;
        uint32_t reserved_24:5;
        /** dtrn : RO; bitpos: [29]; default: 1;
         *  This bit represents the level of the internal uart dtr signal.
         */
        uint32_t dtrn:1;
        /** rtsn : RO; bitpos: [30]; default: 1;
         *  This bit represents the level of the internal uart rts signal.
         */
        uint32_t rtsn:1;
        /** txd : RO; bitpos: [31]; default: 1;
         *  This bit represents the  level of the internal uart txd signal.
         */
        uint32_t txd:1;
    };
    uint32_t val;
} uart_status_reg_t;

/** Type of mem_tx_status register
 *  Tx-SRAM write and read offset address.
 */
typedef union {
    struct {
        /** tx_sram_waddr : RO; bitpos: [7:0]; default: 0;
         *  This register stores the offset write address in Tx-SRAM.
         */
        uint32_t tx_sram_waddr:8;
        uint32_t reserved_8:1;
        /** tx_sram_raddr : RO; bitpos: [16:9]; default: 0;
         *  This register stores the offset read address in Tx-SRAM.
         */
        uint32_t tx_sram_raddr:8;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} uart_mem_tx_status_reg_t;

/** Type of mem_rx_status register
 *  Rx-SRAM write and read offset address.
 */
typedef union {
    struct {
        /** rx_sram_raddr : RO; bitpos: [7:0]; default: 128;
         *  This register stores the offset read address in RX-SRAM.
         */
        uint32_t rx_sram_raddr:8;
        uint32_t reserved_8:1;
        /** rx_sram_waddr : RO; bitpos: [16:9]; default: 128;
         *  This register stores the offset write address in Rx-SRAM.
         */
        uint32_t rx_sram_waddr:8;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} uart_mem_rx_status_reg_t;

/** Type of fsm_status register
 *  UART transmit and receive status.
 */
typedef union {
    struct {
        /** st_urx_out : RO; bitpos: [3:0]; default: 0;
         *  This is the status register of receiver.
         */
        uint32_t st_urx_out:4;
        /** st_utx_out : RO; bitpos: [7:4]; default: 0;
         *  This is the status register of transmitter.
         */
        uint32_t st_utx_out:4;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} uart_fsm_status_reg_t;

/** Type of afifo_status register
 *  UART AFIFO Status
 */
typedef union {
    struct {
        /** tx_afifo_full : RO; bitpos: [0]; default: 0;
         *  Full signal of APB TX AFIFO.
         */
        uint32_t tx_afifo_full:1;
        /** tx_afifo_empty : RO; bitpos: [1]; default: 1;
         *  Empty signal of APB TX AFIFO.
         */
        uint32_t tx_afifo_empty:1;
        /** rx_afifo_full : RO; bitpos: [2]; default: 0;
         *  Full signal of APB RX AFIFO.
         */
        uint32_t rx_afifo_full:1;
        /** rx_afifo_empty : RO; bitpos: [3]; default: 1;
         *  Empty signal of APB RX AFIFO.
         */
        uint32_t rx_afifo_empty:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} uart_afifo_status_reg_t;


/** Group: AT Escape Sequence Selection Configuration */
/** Type of at_cmd_precnt_sync register
 *  Pre-sequence timing configuration
 */
typedef union {
    struct {
        /** pre_idle_num : R/W; bitpos: [15:0]; default: 2305;
         *  This register is used to configure the idle duration time before the first at_cmd
         *  is received by receiver.
         */
        uint32_t pre_idle_num:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} uart_at_cmd_precnt_sync_reg_t;

/** Type of at_cmd_postcnt_sync register
 *  Post-sequence timing configuration
 */
typedef union {
    struct {
        /** post_idle_num : R/W; bitpos: [15:0]; default: 2305;
         *  This register is used to configure the duration time between the last at_cmd and
         *  the next data.
         */
        uint32_t post_idle_num:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} uart_at_cmd_postcnt_sync_reg_t;

/** Type of at_cmd_gaptout_sync register
 *  Timeout configuration
 */
typedef union {
    struct {
        /** rx_gap_tout : R/W; bitpos: [15:0]; default: 11;
         *  This register is used to configure the duration time between the at_cmd chars.
         */
        uint32_t rx_gap_tout:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} uart_at_cmd_gaptout_sync_reg_t;

/** Type of at_cmd_char_sync register
 *  AT escape sequence detection configuration
 */
typedef union {
    struct {
        /** at_cmd_char : R/W; bitpos: [7:0]; default: 43;
         *  This register is used to configure the content of at_cmd char.
         */
        uint32_t at_cmd_char:8;
        /** at_char_num : R/W; bitpos: [15:8]; default: 3;
         *  This register is used to configure the num of continuous at_cmd chars received by
         *  receiver.
         */
        uint32_t at_char_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} uart_at_cmd_char_sync_reg_t;


/** Group: Autobaud Register */
/** Type of pospulse register
 *  Autobaud high pulse register
 */
typedef union {
    struct {
        /** posedge_min_cnt : RO; bitpos: [11:0]; default: 4095;
         *  This register stores the minimal input clock count between two positive edges. It
         *  is used in boudrate-detect process.
         */
        uint32_t posedge_min_cnt:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} uart_pospulse_reg_t;

/** Type of negpulse register
 *  Autobaud low pulse register
 */
typedef union {
    struct {
        /** negedge_min_cnt : RO; bitpos: [11:0]; default: 4095;
         *  This register stores the minimal input clock count between two negative edges. It
         *  is used in boudrate-detect process.
         */
        uint32_t negedge_min_cnt:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} uart_negpulse_reg_t;

/** Type of lowpulse register
 *  Autobaud minimum low pulse duration register
 */
typedef union {
    struct {
        /** lowpulse_min_cnt : RO; bitpos: [11:0]; default: 4095;
         *  This register stores the value of the minimum duration time of the low level pulse.
         *  It is used in baud rate-detect process.
         */
        uint32_t lowpulse_min_cnt:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} uart_lowpulse_reg_t;

/** Type of highpulse register
 *  Autobaud minimum high pulse duration register
 */
typedef union {
    struct {
        /** highpulse_min_cnt : RO; bitpos: [11:0]; default: 4095;
         *  This register stores  the value of the maximum duration time for the high level
         *  pulse. It is used in baud rate-detect process.
         */
        uint32_t highpulse_min_cnt:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} uart_highpulse_reg_t;

/** Type of rxd_cnt register
 *  Autobaud edge change count register
 */
typedef union {
    struct {
        /** rxd_edge_cnt : RO; bitpos: [9:0]; default: 0;
         *  This register stores the count of rxd edge change. It is used in baud rate-detect
         *  process.
         */
        uint32_t rxd_edge_cnt:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} uart_rxd_cnt_reg_t;


/** Group: Version Register */
/** Type of date register
 *  UART Version register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 36720720;
         *  This is the version register.
         */
        uint32_t date:32;
    };
    uint32_t val;
} uart_date_reg_t;

/** Type of reg_update register
 *  UART Registers Configuration Update register
 */
typedef union {
    struct {
        /** reg_update : R/W/SC; bitpos: [0]; default: 0;
         *  Software write 1 would synchronize registers into UART Core clock domain and would
         *  be cleared by hardware after synchronization is done.
         */
        uint32_t reg_update:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} uart_reg_update_reg_t;

/** Type of id register
 *  UART ID register
 */
typedef union {
    struct {
        /** id : R/W; bitpos: [31:0]; default: 1280;
         *  This register is used to configure the uart_id.
         */
        uint32_t id:32;
    };
    uint32_t val;
} uart_id_reg_t;


typedef struct uart_dev_t{
    volatile uart_fifo_reg_t fifo;
    volatile uart_int_raw_reg_t int_raw;
    volatile uart_int_st_reg_t int_st;
    volatile uart_int_ena_reg_t int_ena;
    volatile uart_int_clr_reg_t int_clr;
    volatile uart_clkdiv_sync_reg_t clkdiv_sync;
    volatile uart_rx_filt_reg_t rx_filt;
    volatile uart_status_reg_t status;
    volatile uart_conf0_sync_reg_t conf0_sync;
    volatile uart_conf1_reg_t conf1;
    uint32_t reserved_028;
    volatile uart_hwfc_conf_sync_reg_t hwfc_conf_sync;
    volatile uart_sleep_conf0_reg_t sleep_conf0;
    volatile uart_sleep_conf1_reg_t sleep_conf1;
    volatile uart_sleep_conf2_reg_t sleep_conf2;
    volatile uart_swfc_conf0_sync_reg_t swfc_conf0_sync;
    volatile uart_swfc_conf1_reg_t swfc_conf1;
    volatile uart_txbrk_conf_sync_reg_t txbrk_conf_sync;
    volatile uart_idle_conf_sync_reg_t idle_conf_sync;
    volatile uart_rs485_conf_sync_reg_t rs485_conf_sync;
    volatile uart_at_cmd_precnt_sync_reg_t at_cmd_precnt_sync;
    volatile uart_at_cmd_postcnt_sync_reg_t at_cmd_postcnt_sync;
    volatile uart_at_cmd_gaptout_sync_reg_t at_cmd_gaptout_sync;
    volatile uart_at_cmd_char_sync_reg_t at_cmd_char_sync;
    volatile uart_mem_conf_reg_t mem_conf;
    volatile uart_tout_conf_sync_reg_t tout_conf_sync;
    volatile uart_mem_tx_status_reg_t mem_tx_status;
    volatile uart_mem_rx_status_reg_t mem_rx_status;
    volatile uart_fsm_status_reg_t fsm_status;
    volatile uart_pospulse_reg_t pospulse;      /* LP_UART instance has this register reserved */
    volatile uart_negpulse_reg_t negpulse;      /* LP_UART instance has this register reserved */
    volatile uart_lowpulse_reg_t lowpulse;      /* LP_UART instance has this register reserved */
    volatile uart_highpulse_reg_t highpulse;    /* LP_UART instance has this register reserved */
    volatile uart_rxd_cnt_reg_t rxd_cnt;        /* LP_UART instance has this register reserved */
    volatile uart_clk_conf_reg_t clk_conf;
    volatile uart_date_reg_t date;
    volatile uart_afifo_status_reg_t afifo_status;
    uint32_t reserved_094;
    volatile uart_reg_update_reg_t reg_update;
    volatile uart_id_reg_t id;
} uart_dev_t;

extern uart_dev_t UART0;
extern uart_dev_t UART1;
extern uart_dev_t UART2;
extern uart_dev_t UART3;
extern uart_dev_t UART4;
extern uart_dev_t LP_UART;

#ifndef __cplusplus
_Static_assert(sizeof(uart_dev_t) == 0xa0, "Invalid size of uart_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
