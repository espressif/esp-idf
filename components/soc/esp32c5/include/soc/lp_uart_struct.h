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

/** Group: FIFO Configuration */
/** Type of fifo register
 *  FIFO data register
 */
typedef union {
    struct {
        /** rxfifo_rd_byte : RO; bitpos: [7:0]; default: 0;
         *  Represents the data LP UART $n read from FIFO.\\
         *  Measurement unit: byte.
         */
        uint32_t rxfifo_rd_byte:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_uart_fifo_reg_t;

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
} lp_uart_mem_conf_reg_t;

/** Type of tout_conf_sync register
 *  LP UART threshold and allocation configuration
 */
typedef union {
    struct {
        /** rx_tout_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable LP UART receiver's timeout function.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t rx_tout_en:1;
        /** rx_tout_flow_dis : R/W; bitpos: [1]; default: 0;
         *  Configures whether or not to stop the idle status counter when hardware flow
         *  control is enabled.\\
         *  0: Invalid. No effect\\
         *  1: Stop\\
         */
        uint32_t rx_tout_flow_dis:1;
        /** rx_tout_thrhd : R/W; bitpos: [11:2]; default: 10;
         *  Configures the amount of time that the bus can remain idle before timeout.\\
         *  Measurement unit: bit time (the time to transmit 1 bit).
         */
        uint32_t rx_tout_thrhd:10;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_uart_tout_conf_sync_reg_t;


/** Group: Interrupt Register */
/** Type of int_raw register
 *  Raw interrupt status
 */
typedef union {
    struct {
        /** rxfifo_full_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status of LP_UART_RXFIFO_FULL_INT.
         */
        uint32_t rxfifo_full_int_raw:1;
        /** txfifo_empty_int_raw : R/WTC/SS; bitpos: [1]; default: 1;
         *  The raw interrupt status of LP_UART_TXFIFO_EMPTY_INT.
         */
        uint32_t txfifo_empty_int_raw:1;
        /** parity_err_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt status of LP_UART_PARITY_ERR_INT.
         */
        uint32_t parity_err_int_raw:1;
        /** frm_err_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt status of LP_UART_FRM_ERR_INT.
         */
        uint32_t frm_err_int_raw:1;
        /** rxfifo_ovf_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt status of LP_UART_RXFIFO_OVF_INT.
         */
        uint32_t rxfifo_ovf_int_raw:1;
        /** dsr_chg_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt status of LP_UART_DSR_CHG_INT.
         */
        uint32_t dsr_chg_int_raw:1;
        /** cts_chg_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt status of LP_UART_CTS_CHG_INT.
         */
        uint32_t cts_chg_int_raw:1;
        /** brk_det_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw interrupt status of LP_UART_BRK_DET_INT.
         */
        uint32_t brk_det_int_raw:1;
        /** rxfifo_tout_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw interrupt status of LP_UART_RXFIFO_TOUT_INT.
         */
        uint32_t rxfifo_tout_int_raw:1;
        /** sw_xon_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  The raw interrupt status of LP_UART_SW_XON_INT.
         */
        uint32_t sw_xon_int_raw:1;
        /** sw_xoff_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  LP_UART_SW_XOFF_INT.
         */
        uint32_t sw_xoff_int_raw:1;
        /** glitch_det_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  The raw interrupt status of LP_UART_GLITCH_DET_INT.
         */
        uint32_t glitch_det_int_raw:1;
        /** tx_brk_done_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  The raw interrupt status of LP_UART_TX_BRK_DONE_INT.
         */
        uint32_t tx_brk_done_int_raw:1;
        /** tx_brk_idle_done_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  The raw interrupt status of LP_UART_TX_BRK_IDLE_DONE_INT.
         */
        uint32_t tx_brk_idle_done_int_raw:1;
        /** tx_done_int_raw : R/WTC/SS; bitpos: [14]; default: 0;
         *  The raw interrupt status of LP_UART_TX_DONE_INT.
         */
        uint32_t tx_done_int_raw:1;
        uint32_t reserved_15:3;
        /** at_cmd_char_det_int_raw : R/WTC/SS; bitpos: [18]; default: 0;
         *  The raw interrupt status of LP_UART_AT_CMD_CHAR_DET_INT.
         */
        uint32_t at_cmd_char_det_int_raw:1;
        /** wakeup_int_raw : R/WTC/SS; bitpos: [19]; default: 0;
         *  The raw interrupt status of LP_UART_WAKEUP_INT.
         */
        uint32_t wakeup_int_raw:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} lp_uart_int_raw_reg_t;

/** Type of int_st register
 *  Masked interrupt status
 */
typedef union {
    struct {
        /** rxfifo_full_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status of LP_UART_RXFIFO_FULL_INT.
         */
        uint32_t rxfifo_full_int_st:1;
        /** txfifo_empty_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status of LP_UART_TXFIFO_EMPTY_INT.
         */
        uint32_t txfifo_empty_int_st:1;
        /** parity_err_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status of LP_UART_PARITY_ERR_INT.
         */
        uint32_t parity_err_int_st:1;
        /** frm_err_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status of LP_UART_FRM_ERR_INT.
         */
        uint32_t frm_err_int_st:1;
        /** rxfifo_ovf_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status of LP_UART_RXFIFO_OVF_INT.
         */
        uint32_t rxfifo_ovf_int_st:1;
        /** dsr_chg_int_st : RO; bitpos: [5]; default: 0;
         *  The masked interrupt status of LP_UART_DSR_CHG_INT.
         */
        uint32_t dsr_chg_int_st:1;
        /** cts_chg_int_st : RO; bitpos: [6]; default: 0;
         *  The masked interrupt status of LP_UART_CTS_CHG_INT.
         */
        uint32_t cts_chg_int_st:1;
        /** brk_det_int_st : RO; bitpos: [7]; default: 0;
         *  The masked interrupt status of LP_UART_BRK_DET_INT.
         */
        uint32_t brk_det_int_st:1;
        /** rxfifo_tout_int_st : RO; bitpos: [8]; default: 0;
         *  The masked interrupt status of LP_UART_RXFIFO_TOUT_INT.
         */
        uint32_t rxfifo_tout_int_st:1;
        /** sw_xon_int_st : RO; bitpos: [9]; default: 0;
         *  The masked interrupt status of LP_UART_SW_XON_INT.
         */
        uint32_t sw_xon_int_st:1;
        /** sw_xoff_int_st : RO; bitpos: [10]; default: 0;
         *  The masked interrupt status of LP_UART_SW_XOFF_INT.
         */
        uint32_t sw_xoff_int_st:1;
        /** glitch_det_int_st : RO; bitpos: [11]; default: 0;
         *  The masked interrupt status of LP_UART_GLITCH_DET_INT.
         */
        uint32_t glitch_det_int_st:1;
        /** tx_brk_done_int_st : RO; bitpos: [12]; default: 0;
         *  The masked interrupt status of LP_UART_TX_BRK_DONE_INT.
         */
        uint32_t tx_brk_done_int_st:1;
        /** tx_brk_idle_done_int_st : RO; bitpos: [13]; default: 0;
         *  The masked interrupt status of LP_UART_TX_BRK_IDLE_DONE_INT.
         */
        uint32_t tx_brk_idle_done_int_st:1;
        /** tx_done_int_st : RO; bitpos: [14]; default: 0;
         *  The masked interrupt status of LP_UART_TX_DONE_INT.
         */
        uint32_t tx_done_int_st:1;
        uint32_t reserved_15:3;
        /** at_cmd_char_det_int_st : RO; bitpos: [18]; default: 0;
         *  The masked interrupt status of LP_UART_AT_CMD_CHAR_DET_INT.
         */
        uint32_t at_cmd_char_det_int_st:1;
        /** wakeup_int_st : RO; bitpos: [19]; default: 0;
         *  The masked interrupt status of LP_UART_WAKEUP_INT.
         */
        uint32_t wakeup_int_st:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} lp_uart_int_st_reg_t;

/** Type of int_ena register
 *  Interrupt enable bits
 */
typedef union {
    struct {
        /** rxfifo_full_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable LP_UART_RXFIFO_FULL_INT.
         */
        uint32_t rxfifo_full_int_ena:1;
        /** txfifo_empty_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable LP_UART_TXFIFO_EMPTY_INT.
         */
        uint32_t txfifo_empty_int_ena:1;
        /** parity_err_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable LP_UART_PARITY_ERR_INT.
         */
        uint32_t parity_err_int_ena:1;
        /** frm_err_int_ena : R/W; bitpos: [3]; default: 0;
         *  Write 1 to enable LP_UART_FRM_ERR_INT.
         */
        uint32_t frm_err_int_ena:1;
        /** rxfifo_ovf_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable LP_UART_RXFIFO_OVF_INT.
         */
        uint32_t rxfifo_ovf_int_ena:1;
        /** dsr_chg_int_ena : R/W; bitpos: [5]; default: 0;
         *  Write 1 to enable LP_UART_DSR_CHG_INT.
         */
        uint32_t dsr_chg_int_ena:1;
        /** cts_chg_int_ena : R/W; bitpos: [6]; default: 0;
         *  Write 1 to enable LP_UART_CTS_CHG_INT.
         */
        uint32_t cts_chg_int_ena:1;
        /** brk_det_int_ena : R/W; bitpos: [7]; default: 0;
         *  Write 1 to enable LP_UART_BRK_DET_INT.
         */
        uint32_t brk_det_int_ena:1;
        /** rxfifo_tout_int_ena : R/W; bitpos: [8]; default: 0;
         *  Write 1 to enable LP_UART_RXFIFO_TOUT_INT.
         */
        uint32_t rxfifo_tout_int_ena:1;
        /** sw_xon_int_ena : R/W; bitpos: [9]; default: 0;
         *  Write 1 to enable LP_UART_SW_XON_INT.
         */
        uint32_t sw_xon_int_ena:1;
        /** sw_xoff_int_ena : R/W; bitpos: [10]; default: 0;
         *  Write 1 to enable LP_UART_SW_XOFF_INT.
         */
        uint32_t sw_xoff_int_ena:1;
        /** glitch_det_int_ena : R/W; bitpos: [11]; default: 0;
         *  Write 1 to enable LP_UART_GLITCH_DET_INT.
         */
        uint32_t glitch_det_int_ena:1;
        /** tx_brk_done_int_ena : R/W; bitpos: [12]; default: 0;
         *  Write 1 to enable LP_UART_TX_BRK_DONE_INT.
         */
        uint32_t tx_brk_done_int_ena:1;
        /** tx_brk_idle_done_int_ena : R/W; bitpos: [13]; default: 0;
         *  Write 1 to enable LP_UART_TX_BRK_IDLE_DONE_INT.
         */
        uint32_t tx_brk_idle_done_int_ena:1;
        /** tx_done_int_ena : R/W; bitpos: [14]; default: 0;
         *  Write 1 to enable LP_UART_TX_DONE_INT.
         */
        uint32_t tx_done_int_ena:1;
        uint32_t reserved_15:3;
        /** at_cmd_char_det_int_ena : R/W; bitpos: [18]; default: 0;
         *  Write 1 to enable LP_UART_AT_CMD_CHAR_DET_INT.
         */
        uint32_t at_cmd_char_det_int_ena:1;
        /** wakeup_int_ena : R/W; bitpos: [19]; default: 0;
         *  Write 1 to enable LP_UART_WAKEUP_INT.
         */
        uint32_t wakeup_int_ena:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} lp_uart_int_ena_reg_t;

/** Type of int_clr register
 *  Interrupt clear bits
 */
typedef union {
    struct {
        /** rxfifo_full_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear LP_UART_RXFIFO_FULL_INT.
         */
        uint32_t rxfifo_full_int_clr:1;
        /** txfifo_empty_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear LP_UART_TXFIFO_EMPTY_INT.
         */
        uint32_t txfifo_empty_int_clr:1;
        /** parity_err_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear LP_UART_PARITY_ERR_INT.
         */
        uint32_t parity_err_int_clr:1;
        /** frm_err_int_clr : WT; bitpos: [3]; default: 0;
         *  Write 1 to clear LP_UART_FRM_ERR_INT.
         */
        uint32_t frm_err_int_clr:1;
        /** rxfifo_ovf_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear LP_UART_RXFIFO_OVF_INT.
         */
        uint32_t rxfifo_ovf_int_clr:1;
        /** dsr_chg_int_clr : WT; bitpos: [5]; default: 0;
         *  Write 1 to clear LP_UART_DSR_CHG_INT.
         */
        uint32_t dsr_chg_int_clr:1;
        /** cts_chg_int_clr : WT; bitpos: [6]; default: 0;
         *  Write 1 to clear LP_UART_CTS_CHG_INT.
         */
        uint32_t cts_chg_int_clr:1;
        /** brk_det_int_clr : WT; bitpos: [7]; default: 0;
         *  Write 1 to clear LP_UART_BRK_DET_INT.
         */
        uint32_t brk_det_int_clr:1;
        /** rxfifo_tout_int_clr : WT; bitpos: [8]; default: 0;
         *  Write 1 to clear LP_UART_RXFIFO_TOUT_INT.
         */
        uint32_t rxfifo_tout_int_clr:1;
        /** sw_xon_int_clr : WT; bitpos: [9]; default: 0;
         *  Write 1 to clear LP_UART_SW_XON_INT.
         */
        uint32_t sw_xon_int_clr:1;
        /** sw_xoff_int_clr : WT; bitpos: [10]; default: 0;
         *  Write 1 to clear LP_UART_SW_XOFF_INT.
         */
        uint32_t sw_xoff_int_clr:1;
        /** glitch_det_int_clr : WT; bitpos: [11]; default: 0;
         *  Write 1 to clear LP_UART_GLITCH_DET_INT.
         */
        uint32_t glitch_det_int_clr:1;
        /** tx_brk_done_int_clr : WT; bitpos: [12]; default: 0;
         *  Write 1 to clear LP_UART_TX_BRK_DONE_INT.
         */
        uint32_t tx_brk_done_int_clr:1;
        /** tx_brk_idle_done_int_clr : WT; bitpos: [13]; default: 0;
         *  Write 1 to clear LP_UART_TX_BRK_IDLE_DONE_INT.
         */
        uint32_t tx_brk_idle_done_int_clr:1;
        /** tx_done_int_clr : WT; bitpos: [14]; default: 0;
         *  Write 1 to clear LP_UART_TX_DONE_INT.
         */
        uint32_t tx_done_int_clr:1;
        uint32_t reserved_15:3;
        /** at_cmd_char_det_int_clr : WT; bitpos: [18]; default: 0;
         *  Write 1 to clear LP_UART_AT_CMD_CHAR_DET_INT.
         */
        uint32_t at_cmd_char_det_int_clr:1;
        /** wakeup_int_clr : WT; bitpos: [19]; default: 0;
         *  Write 1 to clear LP_UART_WAKEUP_INT.
         */
        uint32_t wakeup_int_clr:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} lp_uart_int_clr_reg_t;


/** Group: Configuration Register */
/** Type of clkdiv_sync register
 *  Clock divider configuration
 */
typedef union {
    struct {
        /** clkdiv : R/W; bitpos: [11:0]; default: 694;
         *  Configures the integral part of the divisor for baud rate generation.
         */
        uint32_t clkdiv:12;
        uint32_t reserved_12:8;
        /** clkdiv_frag : R/W; bitpos: [23:20]; default: 0;
         *  Configures the fractional part of the divisor for baud rate generation.
         */
        uint32_t clkdiv_frag:4;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} lp_uart_clkdiv_sync_reg_t;

/** Type of rx_filt register
 *  RX filter configuration
 */
typedef union {
    struct {
        /** glitch_filt : R/W; bitpos: [7:0]; default: 8;
         *  Configures the width of a pulse to be filtered.\\Measurement unit: UART Core's
         *  clock cycle.\\Pulses whose width is lower than this value will be ignored.
         */
        uint32_t glitch_filt:8;
        /** glitch_filt_en : R/W; bitpos: [8]; default: 0;
         *  Configures whether or not to enable RX signal filter.\\
         *  0: Disable\\
         *  1: Enable
         */
        uint32_t glitch_filt_en:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_uart_rx_filt_reg_t;

/** Type of conf0_sync register
 *  Configuration register 0
 */
typedef union {
    struct {
        /** parity : R/W; bitpos: [0]; default: 0;
         *  Configures the parity check mode.\\
         *  0: Even parity\\
         *  1: Odd parity\\
         */
        uint32_t parity:1;
        /** parity_en : R/W; bitpos: [1]; default: 0;
         *  Configures whether or not to enable LP UART parity check.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t parity_en:1;
        /** bit_num : R/W; bitpos: [3:2]; default: 3;
         *  Configures the number of data bits.\\
         *  0: 5 bits\\
         *  1: 6 bits\\
         *  2: 7 bits\\
         *  3: 8 bits\\
         */
        uint32_t bit_num:2;
        /** stop_bit_num : R/W; bitpos: [5:4]; default: 1;
         *  Configures the number of stop bits.\\
         *  0: Invalid. No effect\\
         *  1: 1 bits\\
         *  2: 1.5 bits\\
         *  3: 2 bits\\
         */
        uint32_t stop_bit_num:2;
        /** txd_brk : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to send NULL characters when finishing data
         *  transmission.\\
         *  0: Not send\\
         *  1: Send\\
         */
        uint32_t txd_brk:1;
        uint32_t reserved_7:5;
        /** loopback : R/W; bitpos: [12]; default: 0;
         *  Configures whether or not to enable LP UART loopback test.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t loopback:1;
        /** tx_flow_en : R/W; bitpos: [13]; default: 0;
         *  Configures whether or not to enable flow control for the transmitter.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t tx_flow_en:1;
        uint32_t reserved_14:1;
        /** rxd_inv : R/W; bitpos: [15]; default: 0;
         *  Configures whether or not to invert the level of LP UART RXD signal.\\
         *  0: Not invert\\
         *  1: Invert\\
         */
        uint32_t rxd_inv:1;
        /** txd_inv : R/W; bitpos: [16]; default: 0;
         *  Configures whether or not to invert the level of LP UART TXD signal.\\
         *  0: Not invert\\
         *  1: Invert\\
         */
        uint32_t txd_inv:1;
        /** dis_rx_dat_ovf : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to disable data overflow detection for the LP UART
         *  receiver.\\
         *  0: Enable\\
         *  1: Disable\\
         */
        uint32_t dis_rx_dat_ovf:1;
        /** err_wr_mask : R/W; bitpos: [18]; default: 0;
         *  Configures whether or not to store the received data with errors into FIFO.\\
         *  0: Store\\
         *  1: Not store\\
         */
        uint32_t err_wr_mask:1;
        uint32_t reserved_19:1;
        /** mem_clk_en : R/W; bitpos: [20]; default: 0;
         *  Configures whether or not to enable clock gating for LP UART memory.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t mem_clk_en:1;
        /** sw_rts : R/W; bitpos: [21]; default: 0;
         *  Configures the RTS signal used in software flow control.\\
         *  0: The LP UART transmitter is allowed to send data.\\
         *  1: The LP UART transmitted is not allowed to send data.\\
         */
        uint32_t sw_rts:1;
        /** rxfifo_rst : R/W; bitpos: [22]; default: 0;
         *  Configures whether or not to reset the LP UART RX FIFO.\\
         *  0: Not reset\\
         *  1: Reset\\
         */
        uint32_t rxfifo_rst:1;
        /** txfifo_rst : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to reset the LP UART TX FIFO.\\
         *  0: Not reset\\
         *  1: Reset\\
         */
        uint32_t txfifo_rst:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} lp_uart_conf0_sync_reg_t;

/** Type of conf1 register
 *  Configuration register 1
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
        /** rxfifo_full_thrhd : R/W; bitpos: [7:3]; default: 12;
         *  Configures the threshold for RX FIFO being full.\\Measurement unit: byte.
         */
        uint32_t rxfifo_full_thrhd:5;
        uint32_t reserved_8:3;
        /** txfifo_empty_thrhd : R/W; bitpos: [15:11]; default: 12;
         *  Configures the threshold for TX FIFO being empty.\\Measurement unit: byte.
         */
        uint32_t txfifo_empty_thrhd:5;
        /** cts_inv : R/W; bitpos: [16]; default: 0;
         *  Configures whether or not to invert the level of LP UART CTS signal.\\
         *  0: Not invert\\
         *  1: Invert\\
         */
        uint32_t cts_inv:1;
        /** dsr_inv : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to invert the level of LP UART DSR signal.\\
         *  0: Not invert\\
         *  1: Invert\\
         */
        uint32_t dsr_inv:1;
        /** rts_inv : R/W; bitpos: [18]; default: 0;
         *  Configures whether or not to invert the level of LP UART RTS signal.\\
         *  0: Not invert\\
         *  1: Invert\\
         */
        uint32_t rts_inv:1;
        /** dtr_inv : R/W; bitpos: [19]; default: 0;
         *  Configures whether or not to invert the level of LP UART DTR signal.\\
         *  0: Not invert\\
         *  1: Invert\\
         */
        uint32_t dtr_inv:1;
        /** sw_dtr : R/W; bitpos: [20]; default: 0;
         *  Configures the DTR signal used in software flow control.\\
         *  0: Data to be transmitted is not ready.\\
         *  1: Data to be transmitted is ready.\\
         */
        uint32_t sw_dtr:1;
        /** clk_en : R/W; bitpos: [21]; default: 0;
         *  Configures clock gating.\\
         *  0: Support clock only when the application writes registers.\\
         *  1: Always force the clock on for registers.\\
         */
        uint32_t clk_en:1;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} lp_uart_conf1_reg_t;

/** Type of hwfc_conf_sync register
 *  Hardware flow control configuration
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
        /** rx_flow_thrhd : R/W; bitpos: [7:3]; default: 0;
         *  Configures the maximum number of data bytes that can be received  during hardware
         *  flow control.\\Measurement unit: byte.
         */
        uint32_t rx_flow_thrhd:5;
        /** rx_flow_en : R/W; bitpos: [8]; default: 0;
         *  Configures whether or not to enable the LP UART receiver.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t rx_flow_en:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_uart_hwfc_conf_sync_reg_t;

/** Type of sleep_conf0 register
 *  LP UART sleep configuration register 0
 */
typedef union {
    struct {
        /** wk_char1 : R/W; bitpos: [7:0]; default: 0;
         *  Configures wakeup character 1.
         */
        uint32_t wk_char1:8;
        /** wk_char2 : R/W; bitpos: [15:8]; default: 0;
         *  Configures wakeup character 2.
         */
        uint32_t wk_char2:8;
        /** wk_char3 : R/W; bitpos: [23:16]; default: 0;
         *  Configures wakeup character 3.
         */
        uint32_t wk_char3:8;
        /** wk_char4 : R/W; bitpos: [31:24]; default: 0;
         *  Configures wakeup character 4.
         */
        uint32_t wk_char4:8;
    };
    uint32_t val;
} lp_uart_sleep_conf0_reg_t;

/** Type of sleep_conf1 register
 *  LP UART sleep configuration register 1
 */
typedef union {
    struct {
        /** wk_char0 : R/W; bitpos: [7:0]; default: 0;
         *  Configures wakeup character 0.
         */
        uint32_t wk_char0:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_uart_sleep_conf1_reg_t;

/** Type of sleep_conf2 register
 *  LP UART sleep configuration register 2
 */
typedef union {
    struct {
        /** active_threshold : R/W; bitpos: [9:0]; default: 240;
         *  Configures the number of RXD edge changes to wake up the chip in wakeup mode 0.
         */
        uint32_t active_threshold:10;
        uint32_t reserved_10:3;
        /** rx_wake_up_thrhd : R/W; bitpos: [17:13]; default: 1;
         *  Configures the number of received data bytes to wake up the chip in wakeup mode 1.
         */
        uint32_t rx_wake_up_thrhd:5;
        /** wk_char_num : R/W; bitpos: [20:18]; default: 5;
         *  Configures the number of wakeup characters.
         */
        uint32_t wk_char_num:3;
        /** wk_char_mask : R/W; bitpos: [25:21]; default: 0;
         *  Configures whether or not to mask wakeup characters.\\
         *  0: Not mask\\
         *  1: Mask\\
         */
        uint32_t wk_char_mask:5;
        /** wk_mode_sel : R/W; bitpos: [27:26]; default: 0;
         *  Configures which wakeup mode to select.\\
         *  0: Mode 0\\
         *  1: Mode 1\\
         *  2: Mode 2\\
         *  3: Mode 3\\
         */
        uint32_t wk_mode_sel:2;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lp_uart_sleep_conf2_reg_t;

/** Type of swfc_conf0_sync register
 *  Software flow control character configuration
 */
typedef union {
    struct {
        /** xon_char : R/W; bitpos: [7:0]; default: 17;
         *  Configures the XON character for flow control.
         */
        uint32_t xon_char:8;
        /** xoff_char : R/W; bitpos: [15:8]; default: 19;
         *  Configures the XOFF character for flow control.
         */
        uint32_t xoff_char:8;
        /** xon_xoff_still_send : R/W; bitpos: [16]; default: 0;
         *  Configures whether the LP UART transmitter can send XON or XOFF characters when it
         *  is disabled.\\
         *  0: Cannot send\\
         *  1: Can send\\
         */
        uint32_t xon_xoff_still_send:1;
        /** sw_flow_con_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable software flow control.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t sw_flow_con_en:1;
        /** xonoff_del : R/W; bitpos: [18]; default: 0;
         *  Configures whether or not to remove flow control characters from the received
         *  data.\\
         *  0: Not move\\
         *  1: Move\\
         */
        uint32_t xonoff_del:1;
        /** force_xon : R/W; bitpos: [19]; default: 0;
         *  Configures whether the transmitter continues to sending data.\\
         *  0: Not send\\
         *  1: Send\\
         */
        uint32_t force_xon:1;
        /** force_xoff : R/W; bitpos: [20]; default: 0;
         *  Configures whether or not to stop the transmitter from sending data.\\
         *  0: Not stop\\
         *  1: Stop\\
         */
        uint32_t force_xoff:1;
        /** send_xon : R/W/SS/SC; bitpos: [21]; default: 0;
         *  Configures whether or not to send XON characters.\\
         *  0: Not send\\
         *  1: Send\\
         */
        uint32_t send_xon:1;
        /** send_xoff : R/W/SS/SC; bitpos: [22]; default: 0;
         *  Configures whether or not to send XOFF characters.\\
         *  0: Not send\\
         *  1: Send\\
         */
        uint32_t send_xoff:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} lp_uart_swfc_conf0_sync_reg_t;

/** Type of swfc_conf1 register
 *  Software flow control character configuration
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
        /** xon_threshold : R/W; bitpos: [7:3]; default: 0;
         *  Configures the threshold for data in RX FIFO to send XON characters in software
         *  flow control.\\Measurement unit: byte.
         */
        uint32_t xon_threshold:5;
        uint32_t reserved_8:3;
        /** xoff_threshold : R/W; bitpos: [15:11]; default: 12;
         *  Configures the threshold for data in RX FIFO to send XOFF characters in software
         *  flow control.\\Measurement unit: byte.
         */
        uint32_t xoff_threshold:5;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_uart_swfc_conf1_reg_t;

/** Type of txbrk_conf_sync register
 *  TX break character configuration
 */
typedef union {
    struct {
        /** tx_brk_num : R/W; bitpos: [7:0]; default: 10;
         *  Configures the number of NULL characters to be sent after finishing data
         *  transmission.\\Valid only when LP_UART_TXD_BRK is 1.
         */
        uint32_t tx_brk_num:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_uart_txbrk_conf_sync_reg_t;

/** Type of idle_conf_sync register
 *  Frame end idle time configuration
 */
typedef union {
    struct {
        /** rx_idle_thrhd : R/W; bitpos: [9:0]; default: 256;
         *  Configures the threshold to generate a frame end signal when the receiver takes
         *  more time to receive one data byte data.\\Measurement unit: bit time (the time to
         *  transmit 1 bit).
         */
        uint32_t rx_idle_thrhd:10;
        /** tx_idle_num : R/W; bitpos: [19:10]; default: 256;
         *  Configures the interval between two data transfers.\\Measurement unit: bit time
         *  (the time to transmit 1 bit).
         */
        uint32_t tx_idle_num:10;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} lp_uart_idle_conf_sync_reg_t;

/** Type of delay_conf_sync register
 *  RS485 mode configuration
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** dl0_en : R/W; bitpos: [1]; default: 0;
         *  Configures whether or not to add a turnaround delay of 1 bit before the start bit.\\
         *  0: Not add\\
         *  1: Add\\
         */
        uint32_t dl0_en:1;
        /** dl1_en : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to add a turnaround delay of 1 bit after the stop bit.\\
         *  0: Not add\\
         *  1: Add\\
         */
        uint32_t dl1_en:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} lp_uart_delay_conf_sync_reg_t;

/** Type of clk_conf register
 *  LP UART core clock configuration
 */
typedef union {
    struct {
        /** sclk_div_b : R/W; bitpos: [5:0]; default: 0;
         *  The  denominator of the frequency divider factor.
         *  Only available to LP UART instance
         */
        uint32_t sclk_div_b:6;
        /** sclk_div_a : R/W; bitpos: [11:6]; default: 0;
         *  The numerator of the frequency divider factor.
         *  Only available to LP UART instance
         */
        uint32_t sclk_div_a:6;
        /** sclk_div_num : R/W; bitpos: [19:12]; default: 1;
         *  The integral part of the frequency divider factor.
         *  Only available to LP UART instance
         */
        uint32_t sclk_div_num:8;
        uint32_t reserved_20:4;
        /** tx_sclk_en : R/W; bitpos: [24]; default: 1;
         *  Configures whether or not to enable LP UART TX clock.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t tx_sclk_en:1;
        /** rx_sclk_en : R/W; bitpos: [25]; default: 1;
         *  Configures whether or not to enable LP UART RX clock.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t rx_sclk_en:1;
        /** tx_rst_core : R/W; bitpos: [26]; default: 0;
         *  Write 1 and then write 0 to reset LP UART TX.
         */
        uint32_t tx_rst_core:1;
        /** rx_rst_core : R/W; bitpos: [27]; default: 0;
         *  Write 1 and then write 0 to reset LP UART RX.
         */
        uint32_t rx_rst_core:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lp_uart_clk_conf_reg_t;


/** Group: Status Register */
/** Type of status register
 *  LP UART status register
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
        /** rxfifo_cnt : RO; bitpos: [7:3]; default: 0;
         *  Represents the number of valid data bytes in RX FIFO.
         */
        uint32_t rxfifo_cnt:5;
        uint32_t reserved_8:5;
        /** dsrn : RO; bitpos: [13]; default: 0;
         *  Represents the level of the internal LP UART DSR signal.
         */
        uint32_t dsrn:1;
        /** ctsn : RO; bitpos: [14]; default: 1;
         *  Represents the level of the internal LP UART CTS signal.
         */
        uint32_t ctsn:1;
        /** rxd : RO; bitpos: [15]; default: 1;
         *  Represents the  level of the internal LP UART RXD signal.
         */
        uint32_t rxd:1;
        uint32_t reserved_16:3;
        /** txfifo_cnt : RO; bitpos: [23:19]; default: 0;
         *  Represents the number of valid data bytes in RX FIFO.
         */
        uint32_t txfifo_cnt:5;
        uint32_t reserved_24:5;
        /** dtrn : RO; bitpos: [29]; default: 1;
         *  Represents the level of the internal LP UART DTR signal.
         */
        uint32_t dtrn:1;
        /** rtsn : RO; bitpos: [30]; default: 1;
         *  Represents the level of the internal LP UART RTS signal.
         */
        uint32_t rtsn:1;
        /** txd : RO; bitpos: [31]; default: 1;
         *  Represents the  level of the internal LP UART TXD signal.
         */
        uint32_t txd:1;
    };
    uint32_t val;
} lp_uart_status_reg_t;

/** Type of mem_tx_status register
 *  TX FIFO write and read offset address
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
        /** tx_sram_waddr : RO; bitpos: [7:3]; default: 0;
         *  Represents the offset address to write TX FIFO.
         */
        uint32_t tx_sram_waddr:5;
        uint32_t reserved_8:4;
        /** tx_sram_raddr : RO; bitpos: [16:12]; default: 0;
         *  Represents the offset address to read TX FIFO.
         */
        uint32_t tx_sram_raddr:5;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} lp_uart_mem_tx_status_reg_t;

/** Type of mem_rx_status register
 *  RX FIFO write and read offset address
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
        /** rx_sram_raddr : RO; bitpos: [7:3]; default: 16;
         *  Represents the offset address to read RX FIFO.
         */
        uint32_t rx_sram_raddr:5;
        uint32_t reserved_8:4;
        /** rx_sram_waddr : RO; bitpos: [16:12]; default: 16;
         *  Represents the offset address to write RX FIFO.
         */
        uint32_t rx_sram_waddr:5;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} lp_uart_mem_rx_status_reg_t;

/** Type of fsm_status register
 *  LP UART transmit and receive status
 */
typedef union {
    struct {
        /** st_urx_out : RO; bitpos: [3:0]; default: 0;
         *  Represents the status of the receiver.
         */
        uint32_t st_urx_out:4;
        /** st_utx_out : RO; bitpos: [7:4]; default: 0;
         *  Represents the status of the transmitter.
         */
        uint32_t st_utx_out:4;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_uart_fsm_status_reg_t;

/** Type of afifo_status register
 *  LP UART asynchronous FIFO Status
 */
typedef union {
    struct {
        /** tx_afifo_full : RO; bitpos: [0]; default: 0;
         *  Represents whether or not the APB TX asynchronous FIFO is full.\\
         *  0: Not full\\
         *  1: Full\\
         */
        uint32_t tx_afifo_full:1;
        /** tx_afifo_empty : RO; bitpos: [1]; default: 1;
         *  Represents whether or not the APB TX asynchronous FIFO is empty.\\
         *  0: Not empty\\
         *  1: Empty\\
         */
        uint32_t tx_afifo_empty:1;
        /** rx_afifo_full : RO; bitpos: [2]; default: 0;
         *  Represents whether or not the APB RX asynchronous FIFO is full.\\
         *  0: Not full\\
         *  1: Full\\
         */
        uint32_t rx_afifo_full:1;
        /** rx_afifo_empty : RO; bitpos: [3]; default: 1;
         *  Represents whether or not the APB RX asynchronous FIFO is empty.\\
         *  0: Not empty\\
         *  1: Empty\\
         */
        uint32_t rx_afifo_empty:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} lp_uart_afifo_status_reg_t;


/** Group: AT Escape Sequence Selection Configuration */
/** Type of at_cmd_precnt_sync register
 *  Pre-sequence timing configuration
 */
typedef union {
    struct {
        /** pre_idle_num : R/W; bitpos: [15:0]; default: 2305;
         *  Configures the idle time before the receiver receives the first
         *  AT_CMD.\\Measurement unit: bit time (the time to transmit 1 bit).
         */
        uint32_t pre_idle_num:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_uart_at_cmd_precnt_sync_reg_t;

/** Type of at_cmd_postcnt_sync register
 *  Post-sequence timing configuration
 */
typedef union {
    struct {
        /** post_idle_num : R/W; bitpos: [15:0]; default: 2305;
         *  Configures the interval between the last AT_CMD and subsequent data.\\Measurement
         *  unit: bit time (the time to transmit 1 bit).
         */
        uint32_t post_idle_num:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_uart_at_cmd_postcnt_sync_reg_t;

/** Type of at_cmd_gaptout_sync register
 *  Timeout configuration
 */
typedef union {
    struct {
        /** rx_gap_tout : R/W; bitpos: [15:0]; default: 11;
         *  Configures the interval between two AT_CMD characters.\\Measurement unit: bit time
         *  (the time to transmit 1 bit).
         */
        uint32_t rx_gap_tout:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_uart_at_cmd_gaptout_sync_reg_t;

/** Type of at_cmd_char_sync register
 *  AT escape sequence detection configuration
 */
typedef union {
    struct {
        /** at_cmd_char : R/W; bitpos: [7:0]; default: 43;
         *  Configures the AT_CMD character.
         */
        uint32_t at_cmd_char:8;
        /** char_num : R/W; bitpos: [15:8]; default: 3;
         *  Configures the number of continuous AT_CMD characters a receiver can receive.
         */
        uint32_t char_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_uart_at_cmd_char_sync_reg_t;


/** Group: Version Register */
/** Type of date register
 *  LP UART version register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 36774432;
         *  Version control register.
         */
        uint32_t date:32;
    };
    uint32_t val;
} lp_uart_date_reg_t;

/** Type of reg_update register
 *  LP UART register configuration update register
 */
typedef union {
    struct {
        /** reg_update : R/W/SC; bitpos: [0]; default: 0;
         *  Configures whether or not to synchronize registers.\\
         *  0: Not synchronize\\
         *  1: Synchronize\\
         */
        uint32_t reg_update:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_uart_reg_update_reg_t;

/** Type of id register
 *  LP UART ID register
 */
typedef union {
    struct {
        /** id : R/W; bitpos: [31:0]; default: 1280;
         *  Configures the LP UART ID.
         */
        uint32_t id:32;
    };
    uint32_t val;
} lp_uart_id_reg_t;


typedef struct {
    volatile lp_uart_fifo_reg_t fifo;
    volatile lp_uart_int_raw_reg_t int_raw;
    volatile lp_uart_int_st_reg_t int_st;
    volatile lp_uart_int_ena_reg_t int_ena;
    volatile lp_uart_int_clr_reg_t int_clr;
    volatile lp_uart_clkdiv_sync_reg_t clkdiv_sync;
    volatile lp_uart_rx_filt_reg_t rx_filt;
    volatile lp_uart_status_reg_t status;
    volatile lp_uart_conf0_sync_reg_t conf0_sync;
    volatile lp_uart_conf1_reg_t conf1;
    uint32_t reserved_028;
    volatile lp_uart_hwfc_conf_sync_reg_t hwfc_conf_sync;
    volatile lp_uart_sleep_conf0_reg_t sleep_conf0;
    volatile lp_uart_sleep_conf1_reg_t sleep_conf1;
    volatile lp_uart_sleep_conf2_reg_t sleep_conf2;
    volatile lp_uart_swfc_conf0_sync_reg_t swfc_conf0_sync;
    volatile lp_uart_swfc_conf1_reg_t swfc_conf1;
    volatile lp_uart_txbrk_conf_sync_reg_t txbrk_conf_sync;
    volatile lp_uart_idle_conf_sync_reg_t idle_conf_sync;
    volatile lp_uart_delay_conf_sync_reg_t delay_conf_sync;
    volatile lp_uart_at_cmd_precnt_sync_reg_t at_cmd_precnt_sync;
    volatile lp_uart_at_cmd_postcnt_sync_reg_t at_cmd_postcnt_sync;
    volatile lp_uart_at_cmd_gaptout_sync_reg_t at_cmd_gaptout_sync;
    volatile lp_uart_at_cmd_char_sync_reg_t at_cmd_char_sync;
    volatile lp_uart_mem_conf_reg_t mem_conf;
    volatile lp_uart_tout_conf_sync_reg_t tout_conf_sync;
    volatile lp_uart_mem_tx_status_reg_t mem_tx_status;
    volatile lp_uart_mem_rx_status_reg_t mem_rx_status;
    volatile lp_uart_fsm_status_reg_t fsm_status;
    uint32_t reserved_074[5];
    volatile lp_uart_clk_conf_reg_t clk_conf;
    volatile lp_uart_date_reg_t date;
    volatile lp_uart_afifo_status_reg_t afifo_status;
    uint32_t reserved_094;
    volatile lp_uart_reg_update_reg_t reg_update;
    volatile lp_uart_id_reg_t id;
} lp_uart_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(lp_uart_dev_t) == 0xa0, "Invalid size of lp_uart_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
