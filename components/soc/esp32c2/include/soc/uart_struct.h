/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct uart_dev_s {
    union {
        struct {
            uint32_t rw_byte                       :    32;  /*UART $n accesses FIFO via this register.*/
        };
        uint32_t val;
    } ahb_fifo;
    union {
        struct {
            uint32_t rxfifo_full                   :    1;  /*This interrupt raw bit turns to high level when receiver receives more data than what rxfifo_full_thrhd specifies.*/
            uint32_t txfifo_empty                  :    1;  /*This interrupt raw bit turns to high level when the amount of data in Tx-FIFO is less than what txfifo_empty_thrhd specifies .*/
            uint32_t parity_err                    :    1;  /*This interrupt raw bit turns to high level when receiver detects a parity error in the data.*/
            uint32_t frm_err                       :    1;  /*This interrupt raw bit turns to high level when receiver detects a data frame error .*/
            uint32_t rxfifo_ovf                    :    1;  /*This interrupt raw bit turns to high level when receiver receives more data than the FIFO can store.*/
            uint32_t dsr_chg                       :    1;  /*This interrupt raw bit turns to high level when receiver detects the edge change of DSRn signal.*/
            uint32_t cts_chg                       :    1;  /*This interrupt raw bit turns to high level when receiver detects the edge change of CTSn signal.*/
            uint32_t brk_det                       :    1;  /*This interrupt raw bit turns to high level when receiver detects a 0 after the stop bit.*/
            uint32_t rxfifo_tout                   :    1;  /*This interrupt raw bit turns to high level when receiver takes more time than rx_tout_thrhd to receive a byte.*/
            uint32_t sw_xon                        :    1;  /*This interrupt raw bit turns to high level when receiver recevies Xon char when uart_sw_flow_con_en is set to 1.*/
            uint32_t sw_xoff                       :    1;  /*This interrupt raw bit turns to high level when receiver receives Xoff char when uart_sw_flow_con_en is set to 1.*/
            uint32_t glitch_det                    :    1;  /*This interrupt raw bit turns to high level when receiver detects a glitch in the middle of a start bit.*/
            uint32_t tx_brk_done                   :    1;  /*This interrupt raw bit turns to high level when transmitter completes  sending  NULL characters, after all data in Tx-FIFO are sent.*/
            uint32_t tx_brk_idle_done              :    1;  /*This interrupt raw bit turns to high level when transmitter has kept the shortest duration after sending the  last data.*/
            uint32_t tx_done                       :    1;  /*This interrupt raw bit turns to high level when transmitter has send out all data in FIFO.*/
            uint32_t rs485_parity_err              :    1;  /*This interrupt raw bit turns to high level when receiver detects a parity error from the echo of transmitter in rs485 mode.*/
            uint32_t rs485_frm_err                 :    1;  /*This interrupt raw bit turns to high level when receiver detects a data frame error from the echo of transmitter in rs485 mode.*/
            uint32_t rs485_clash                   :    1;  /*This interrupt raw bit turns to high level when detects a clash between transmitter and receiver in rs485 mode.*/
            uint32_t at_cmd_char_det               :    1;  /*This interrupt raw bit turns to high level when receiver detects the configured at_cmd char.*/
            uint32_t wakeup                        :    1;  /*This interrupt raw bit turns to high level when input rxd edge changes more times than what reg_active_threshold specifies in light sleeping mode.*/
            uint32_t reserved20                    :    12;  /*Reserved*/
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t rxfifo_full                   :    1;  /*This is the status bit for rxfifo_full_int_raw when rxfifo_full_int_ena is set to 1.*/
            uint32_t txfifo_empty                  :    1;  /*This is the status bit for  txfifo_empty_int_raw  when txfifo_empty_int_ena is set to 1.*/
            uint32_t parity_err                    :    1;  /*This is the status bit for parity_err_int_raw when parity_err_int_ena is set to 1.*/
            uint32_t frm_err                       :    1;  /*This is the status bit for frm_err_int_raw when frm_err_int_ena is set to 1.*/
            uint32_t rxfifo_ovf                    :    1;  /*This is the status bit for rxfifo_ovf_int_raw when rxfifo_ovf_int_ena is set to 1.*/
            uint32_t dsr_chg                       :    1;  /*This is the status bit for dsr_chg_int_raw when dsr_chg_int_ena is set to 1.*/
            uint32_t cts_chg                       :    1;  /*This is the status bit for cts_chg_int_raw when cts_chg_int_ena is set to 1.*/
            uint32_t brk_det                       :    1;  /*This is the status bit for brk_det_int_raw when brk_det_int_ena is set to 1.*/
            uint32_t rxfifo_tout                   :    1;  /*This is the status bit for rxfifo_tout_int_raw when rxfifo_tout_int_ena is set to 1.*/
            uint32_t sw_xon                        :    1;  /*This is the status bit for sw_xon_int_raw when sw_xon_int_ena is set to 1.*/
            uint32_t sw_xoff                       :    1;  /*This is the status bit for sw_xoff_int_raw when sw_xoff_int_ena is set to 1.*/
            uint32_t glitch_det                    :    1;  /*This is the status bit for glitch_det_int_raw when glitch_det_int_ena is set to 1.*/
            uint32_t tx_brk_done                   :    1;  /*This is the status bit for tx_brk_done_int_raw when tx_brk_done_int_ena is set to 1.*/
            uint32_t tx_brk_idle_done              :    1;  /*This is the stauts bit for tx_brk_idle_done_int_raw when tx_brk_idle_done_int_ena is set to 1.*/
            uint32_t tx_done                       :    1;  /*This is the status bit for tx_done_int_raw when tx_done_int_ena is set to 1.*/
            uint32_t rs485_parity_err              :    1;  /*This is the status bit for rs485_parity_err_int_raw when rs485_parity_int_ena is set to 1.*/
            uint32_t rs485_frm_err                 :    1;  /*This is the status bit for rs485_frm_err_int_raw when rs485_fm_err_int_ena is set to 1.*/
            uint32_t rs485_clash                   :    1;  /*This is the status bit for rs485_clash_int_raw when rs485_clash_int_ena is set to 1.*/
            uint32_t at_cmd_char_det               :    1;  /*This is the status bit for at_cmd_det_int_raw when at_cmd_char_det_int_ena is set to 1.*/
            uint32_t wakeup                        :    1;  /*This is the status bit for uart_wakeup_int_raw when uart_wakeup_int_ena is set to 1.*/
            uint32_t reserved20                    :    12;  /*Reserved*/
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t rxfifo_full                   :    1;  /*This is the enable bit for rxfifo_full_int_st register.*/
            uint32_t txfifo_empty                  :    1;  /*This is the enable bit for txfifo_empty_int_st register.*/
            uint32_t parity_err                    :    1;  /*This is the enable bit for parity_err_int_st register.*/
            uint32_t frm_err                       :    1;  /*This is the enable bit for frm_err_int_st register.*/
            uint32_t rxfifo_ovf                    :    1;  /*This is the enable bit for rxfifo_ovf_int_st register.*/
            uint32_t dsr_chg                       :    1;  /*This is the enable bit for dsr_chg_int_st register.*/
            uint32_t cts_chg                       :    1;  /*This is the enable bit for cts_chg_int_st register.*/
            uint32_t brk_det                       :    1;  /*This is the enable bit for brk_det_int_st register.*/
            uint32_t rxfifo_tout                   :    1;  /*This is the enable bit for rxfifo_tout_int_st register.*/
            uint32_t sw_xon                        :    1;  /*This is the enable bit for sw_xon_int_st register.*/
            uint32_t sw_xoff                       :    1;  /*This is the enable bit for sw_xoff_int_st register.*/
            uint32_t glitch_det                    :    1;  /*This is the enable bit for glitch_det_int_st register.*/
            uint32_t tx_brk_done                   :    1;  /*This is the enable bit for tx_brk_done_int_st register.*/
            uint32_t tx_brk_idle_done              :    1;  /*This is the enable bit for tx_brk_idle_done_int_st register.*/
            uint32_t tx_done                       :    1;  /*This is the enable bit for tx_done_int_st register.*/
            uint32_t rs485_parity_err              :    1;  /*This is the enable bit for rs485_parity_err_int_st register.*/
            uint32_t rs485_frm_err                 :    1;  /*This is the enable bit for rs485_parity_err_int_st register.*/
            uint32_t rs485_clash                   :    1;  /*This is the enable bit for rs485_clash_int_st register.*/
            uint32_t at_cmd_char_det               :    1;  /*This is the enable bit for at_cmd_char_det_int_st register.*/
            uint32_t wakeup                        :    1;  /*This is the enable bit for uart_wakeup_int_st register.*/
            uint32_t reserved20                    :    12;  /*Reserved*/
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t rxfifo_full                   :    1;  /*Set this bit to clear the rxfifo_full_int_raw interrupt.*/
            uint32_t txfifo_empty                  :    1;  /*Set this bit to clear txfifo_empty_int_raw interrupt.*/
            uint32_t parity_err                    :    1;  /*Set this bit to clear parity_err_int_raw interrupt.*/
            uint32_t frm_err                       :    1;  /*Set this bit to clear frm_err_int_raw interrupt.*/
            uint32_t rxfifo_ovf                    :    1;  /*Set this bit to clear rxfifo_ovf_int_raw interrupt.*/
            uint32_t dsr_chg                       :    1;  /*Set this bit to clear the dsr_chg_int_raw interrupt.*/
            uint32_t cts_chg                       :    1;  /*Set this bit to clear the cts_chg_int_raw interrupt.*/
            uint32_t brk_det                       :    1;  /*Set this bit to clear the brk_det_int_raw interrupt.*/
            uint32_t rxfifo_tout                   :    1;  /*Set this bit to clear the rxfifo_tout_int_raw interrupt.*/
            uint32_t sw_xon                        :    1;  /*Set this bit to clear the sw_xon_int_raw interrupt.*/
            uint32_t sw_xoff                       :    1;  /*Set this bit to clear the sw_xoff_int_raw interrupt.*/
            uint32_t glitch_det                    :    1;  /*Set this bit to clear the glitch_det_int_raw interrupt.*/
            uint32_t tx_brk_done                   :    1;  /*Set this bit to clear the tx_brk_done_int_raw interrupt..*/
            uint32_t tx_brk_idle_done              :    1;  /*Set this bit to clear the tx_brk_idle_done_int_raw interrupt.*/
            uint32_t tx_done                       :    1;  /*Set this bit to clear the tx_done_int_raw interrupt.*/
            uint32_t rs485_parity_err              :    1;  /*Set this bit to clear the rs485_parity_err_int_raw interrupt.*/
            uint32_t rs485_frm_err                 :    1;  /*Set this bit to clear the rs485_frm_err_int_raw interrupt.*/
            uint32_t rs485_clash                   :    1;  /*Set this bit to clear the rs485_clash_int_raw interrupt.*/
            uint32_t at_cmd_char_det               :    1;  /*Set this bit to clear the at_cmd_char_det_int_raw interrupt.*/
            uint32_t wakeup                        :    1;  /*Set this bit to clear the uart_wakeup_int_raw interrupt.*/
            uint32_t reserved20                    :    12;  /*Reserved*/
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t div_int                       :    12;  /*The integral part of the frequency divider factor.*/
            uint32_t reserved12                    :    8;
            uint32_t div_frag                      :    4;  /*The decimal part of the frequency divider factor.*/
            uint32_t reserved24                    :    8;  /*Reserved*/
        };
        uint32_t val;
    } clk_div;
    union {
        struct {
            uint32_t glitch_filt                   :    8;  /*when input pulse width is lower than this value, the pulse is ignored.*/
            uint32_t glitch_filt_en                :    1;  /*Set this bit to enable Rx signal filter.*/
            uint32_t reserved9                     :    23;
        };
        uint32_t val;
    } rx_filt;
    union {
        struct {
            uint32_t rxfifo_cnt                    :    10;  /*Stores the byte number of valid data in Rx-FIFO.*/
            uint32_t reserved10                    :    3;
            uint32_t dsrn                          :    1;  /*The register represent the level value of the internal uart dsr signal.*/
            uint32_t ctsn                          :    1;  /*This register represent the level value of the internal uart cts signal.*/
            uint32_t rxd                           :    1;  /*This register represent the  level value of the internal uart rxd signal.*/
            uint32_t txfifo_cnt                    :    10;  /*Stores the byte number of data in Tx-FIFO.*/
            uint32_t reserved26                    :    3;  /*Reserved*/
            uint32_t dtrn                          :    1;  /*This bit represents the level of the internal uart dtr signal.*/
            uint32_t rtsn                          :    1;  /*This bit represents the level of the internal uart rts signal.*/
            uint32_t txd                           :    1;  /*This bit represents the  level of the internal uart txd signal.*/
        };
        uint32_t val;
    } status;
    union {
        struct {
            uint32_t parity                        :    1;  /*This register is used to configure the parity check mode.*/
            uint32_t parity_en                     :    1;  /*Set this bit to enable uart parity check.*/
            uint32_t bit_num                       :    2;  /*This register is used to set the length of data.*/
            uint32_t stop_bit_num                  :    2;  /*This register is used to set the length of  stop bit.*/
            uint32_t sw_rts                        :    1;  /*This register is used to configure the software rts signal which is used in software flow control.*/
            uint32_t sw_dtr                        :    1;  /*This register is used to configure the software dtr signal which is used in software flow control.*/
            uint32_t txd_brk                       :    1;  /*Set this bit to enbale transmitter to  send NULL when the process of sending data is done.*/
            uint32_t irda_dplx                     :    1;  /*Set this bit to enable IrDA loopback mode.*/
            uint32_t irda_tx_en                    :    1;  /*This is the start enable bit for IrDA transmitter.*/
            uint32_t irda_wctl                     :    1;  /*1'h1: The IrDA transmitter's 11th bit is the same as 10th bit. 1'h0: Set IrDA transmitter's 11th bit to 0.*/
            uint32_t irda_tx_inv                   :    1;  /*Set this bit to invert the level of  IrDA transmitter.*/
            uint32_t irda_rx_inv                   :    1;  /*Set this bit to invert the level of IrDA receiver.*/
            uint32_t loopback                      :    1;  /*Set this bit to enable uart loopback test mode.*/
            uint32_t tx_flow_en                    :    1;  /*Set this bit to enable flow control function for transmitter.*/
            uint32_t irda_en                       :    1;  /*Set this bit to enable IrDA protocol.*/
            uint32_t rxfifo_rst                    :    1;  /*Set this bit to reset the uart receive-FIFO.*/
            uint32_t txfifo_rst                    :    1;  /*Set this bit to reset the uart transmit-FIFO.*/
            uint32_t rxd_inv                       :    1;  /*Set this bit to inverse the level value of uart rxd signal.*/
            uint32_t cts_inv                       :    1;  /*Set this bit to inverse the level value of uart cts signal.*/
            uint32_t dsr_inv                       :    1;  /*Set this bit to inverse the level value of uart dsr signal.*/
            uint32_t txd_inv                       :    1;  /*Set this bit to inverse the level value of uart txd signal.*/
            uint32_t rts_inv                       :    1;  /*Set this bit to inverse the level value of uart rts signal.*/
            uint32_t dtr_inv                       :    1;  /*Set this bit to inverse the level value of uart dtr signal.*/
            uint32_t clk_en                        :    1;  /*1'h1: Force clock on for register. 1'h0: Support clock only when application writes registers.*/
            uint32_t err_wr_mask                   :    1;  /*1'h1: Receiver stops storing data into FIFO when data is wrong. 1'h0: Receiver stores the data even if the  received data is wrong.*/
            uint32_t autobaud_en                   :    1;  /*This is the enable bit for detecting baudrate.*/
            uint32_t mem_clk_en                    :    1;  /*UART memory clock gate enable signal.*/
            uint32_t reserved29                    :    3;
        };
        uint32_t val;
    } conf0;
    union {
        struct {
            uint32_t rxfifo_full_thrhd             :    9;  /*It will produce rxfifo_full_int interrupt when receiver receives more data than this register value.*/
            uint32_t txfifo_empty_thrhd            :    9;  /*It will produce txfifo_empty_int interrupt when the data amount in Tx-FIFO is less than this register value.*/
            uint32_t dis_rx_dat_ovf                :    1;  /*Disable UART Rx data overflow detect. */
            uint32_t rx_tout_flow_dis              :    1;  /*Set this bit to stop accumulating idle_cnt when hardware flow control works.*/
            uint32_t rx_flow_en                    :    1;  /*This is the flow enable bit for UART receiver.*/
            uint32_t rx_tout_en                    :    1;  /*This is the enble bit for uart receiver's timeout function.*/
            uint32_t reserved22                    :    10;
        };
        uint32_t val;
    } conf1;
    union {
        struct {
            uint32_t min_cnt                       :    12;  /*This register stores the value of the minimum duration time of the low level pulse. It is used in baud rate-detect process.*/
            uint32_t reserved12                    :    20;  /*Reserved*/
        };
        uint32_t val;
    } lowpulse;
    union {
        struct {
            uint32_t min_cnt                       :    12;  /*This register stores  the value of the maxinum duration time for the high level pulse. It is used in baud rate-detect process.*/
            uint32_t reserved12                    :    20;  /*Reserved*/
        };
        uint32_t val;
    } highpulse;
    union {
        struct {
            uint32_t edge_cnt                      :    10;  /*This register stores the count of rxd edge change. It is used in baud rate-detect process.*/
            uint32_t reserved10                    :    22;  /*Reserved*/
        };
        uint32_t val;
    } rxd_cnt;
    union {
        struct {
            uint32_t sw_flow_con_en                :    1;  /*Set this bit to enable software flow control. It is used with register sw_xon or sw_xoff.*/
            uint32_t xonoff_del                    :    1;  /*Set this bit to remove flow control char from the received data.*/
            uint32_t force_xon                     :    1;  /*Set this bit to enable the transmitter to go on sending data.*/
            uint32_t force_xoff                    :    1;  /*Set this bit to stop the  transmitter from sending data.*/
            uint32_t send_xon                      :    1;  /*Set this bit to send Xon char. It is cleared by hardware automatically.*/
            uint32_t send_xoff                     :    1;  /*Set this bit to send Xoff char. It is cleared by hardware automatically.*/
            uint32_t reserved6                     :    26;  /*Reserved*/
        };
        uint32_t val;
    } flow_conf;
    union {
        struct {
            uint32_t active_threshold              :    10;  /*The uart is activated from light sleeping mode when the input rxd edge changes more times than this register value.*/
            uint32_t reserved10                    :    22;  /*Reserved*/
        };
        uint32_t val;
    } sleep_conf;
    union {
        struct {
            uint32_t xoff_threshold                :    9;  /*When the data amount in Rx-FIFO is more than this register value with uart_sw_flow_con_en set to 1, it will send a Xoff char.*/
            uint32_t xoff_char                     :    8;  /*This register stores the Xoff flow control char.*/
            uint32_t reserved17                    :    15;  /*Reserved*/
        };
        uint32_t val;
    } swfc_conf0;
    union {
        struct {
            uint32_t xon_threshold                 :    9;  /*When the data amount in Rx-FIFO is less than this register value with uart_sw_flow_con_en set to 1, it will send a Xon char.*/
            uint32_t xon_char                      :    8;  /*This register stores the Xon flow control char.*/
            uint32_t reserved17                    :    15;  /*Reserved*/
        };
        uint32_t val;
    } swfc_conf1;
    union {
        struct {
            uint32_t tx_brk_num                    :    8;  /*This register is used to configure the number of 0 to be sent after the process of sending data is done. It is active when txd_brk is set to 1.*/
            uint32_t reserved8                     :    24;
        };
        uint32_t val;
    } txbrk_conf;
    union {
        struct {
            uint32_t rx_idle_thrhd                 :    10;  /*It will produce frame end signal when receiver takes more time to receive one byte data than this register value.*/
            uint32_t tx_idle_num                   :    10;  /*This register is used to configure the duration time between transfers.*/
            uint32_t reserved20                    :    12;  /*Reserved*/
        };
        uint32_t val;
    } idle_conf;
    union {
        struct {
            uint32_t en                            :    1;  /*Set this bit to choose the rs485 mode.*/
            uint32_t dl0_en                        :    1;  /*Set this bit to delay the stop bit by 1 bit.*/
            uint32_t dl1_en                        :    1;  /*Set this bit to delay the stop bit by 1 bit.*/
            uint32_t tx_rx_en                      :    1;  /*Set this bit to enable receiver could receive data when the transmitter is transmitting data in rs485 mode.  */
            uint32_t rx_busy_tx_en                 :    1;  /*1'h1: enable rs485 transmitter to send data when rs485 receiver line is busy. */
            uint32_t rx_dly_num                    :    1;  /*This register is used to delay the receiver's internal data signal.*/
            uint32_t tx_dly_num                    :    4;  /*This register is used to delay the transmitter's internal data signal.*/
            uint32_t reserved10                    :    22;  /*Reserved*/
        };
        uint32_t val;
    } rs485_conf;
    union {
        struct {
            uint32_t pre_idle_num                  :    16;  /*This register is used to configure the idle duration time before the first at_cmd is received by receiver. */
            uint32_t reserved16                    :    16;  /*Reserved*/
        };
        uint32_t val;
    } at_cmd_precnt;
    union {
        struct {
            uint32_t post_idle_num                 :    16;  /*This register is used to configure the duration time between the last at_cmd and the next data.*/
            uint32_t reserved16                    :    16;  /*Reserved*/
        };
        uint32_t val;
    } at_cmd_postcnt;
    union {
        struct {
            uint32_t rx_gap_tout                   :    16;  /*This register is used to configure the duration time between the at_cmd chars.*/
            uint32_t reserved16                    :    16;  /*Reserved*/
        };
        uint32_t val;
    } at_cmd_gaptout;
    union {
        struct {
            uint32_t data                          :    8;  /*This register is used to configure the content of at_cmd char.*/
            uint32_t char_num                      :    8;  /*This register is used to configure the num of continuous at_cmd chars received by receiver.*/
            uint32_t reserved16                    :    16;  /*Reserved*/
        };
        uint32_t val;
    } at_cmd_char;
    union {
        struct {
            uint32_t reserved0                     :    1;
            uint32_t rx_size                       :    3;  /*This register is used to configure the amount of mem allocated for receive-FIFO. The default number is 128 bytes.*/
            uint32_t tx_size                       :    3;  /*This register is used to configure the amount of mem allocated for transmit-FIFO. The default number is 128 bytes.*/
            uint32_t rx_flow_thrhd                 :    9;  /*This register is used to configure the maximum amount of data that can be received  when hardware flow control works.*/
            uint32_t rx_tout_thrhd                 :    10;  /*This register is used to configure the threshold time that receiver takes to receive one byte. The rxfifo_tout_int interrupt will be trigger when the receiver takes more time to receive one byte with rx_tout_en set to 1.*/
            uint32_t force_pd                      :    1;  /*Set this bit to force power down UART memory.*/
            uint32_t force_pu                      :    1;  /*Set this bit to force power up UART memory.*/
            uint32_t reserved28                    :    4;
        };
        uint32_t val;
    } mem_conf;
    union {
        struct {
            uint32_t apb_tx_waddr                  :    10;  /*This register stores the offset address in Tx-FIFO when software writes Tx-FIFO via APB.*/
            uint32_t reserved10                    :    1;  /*Reserved*/
            uint32_t tx_raddr                      :    10;  /*This register stores the offset address in Tx-FIFO when Tx-FSM reads data via Tx-FIFO_Ctrl.*/
            uint32_t reserved21                    :    11;  /*Reserved*/
        };
        uint32_t val;
    } mem_tx_status;
    union {
        struct {
            uint32_t apb_rx_raddr                  :    10;  /*This register stores the offset address in RX-FIFO when software reads data from Rx-FIFO via APB. UART0 is 10'h100. UART1 is 10'h180.*/
            uint32_t reserved10                    :    1;  /*Reserved*/
            uint32_t rx_waddr                      :    10;  /*This register stores the offset address in Rx-FIFO when Rx-FIFO_Ctrl writes Rx-FIFO. UART0 is 10'h100. UART1 is 10'h180.*/
            uint32_t reserved21                    :    11;  /*Reserved*/
        };
        uint32_t val;
    } mem_rx_status;
    union {
        struct {
            uint32_t st_urx_out                    :    4;  /*This is the status register of receiver.*/
            uint32_t st_utx_out                    :    4;  /*This is the status register of transmitter.*/
            uint32_t reserved8                     :    24;  /*Reserved*/
        };
        uint32_t val;
    } fsm_status;
    union {
        struct {
            uint32_t min_cnt                       :    12;  /*This register stores the minimal input clock count between two positive edges. It is used in boudrate-detect process.*/
            uint32_t reserved12                    :    20;  /*Reserved*/
        };
        uint32_t val;
    } pospulse;
    union {
        struct {
            uint32_t min_cnt                       :    12;  /*This register stores the minimal input clock count between two negative edges. It is used in boudrate-detect process.*/
            uint32_t reserved12                    :    20;  /*Reserved*/
        };
        uint32_t val;
    } negpulse;
    union {
        struct {
            uint32_t sclk_div_b                    :    6;  /*The  denominator of the frequency divider factor.*/
            uint32_t sclk_div_a                    :    6;  /*The numerator of the frequency divider factor.*/
            uint32_t sclk_div_num                  :    8;  /*The integral part of the frequency divider factor.*/
            uint32_t sclk_sel                      :    2;  /*UART clock source select. 1: 80Mhz, 2: 8Mhz, 3: XTAL.*/
            uint32_t sclk_en                       :    1;  /*Set this bit to enable UART Tx/Rx clock.*/
            uint32_t rst_core                      :    1;  /*Write 1 then write 0 to this bit, reset UART Tx/Rx.*/
            uint32_t tx_sclk_en                    :    1;  /*Set this bit to enable UART Tx clock.*/
            uint32_t rx_sclk_en                    :    1;  /*Set this bit to enable UART Rx clock.*/
            uint32_t tx_rst_core                   :    1;  /*Write 1 then write 0 to this bit, reset UART Tx.*/
            uint32_t rx_rst_core                   :    1;  /*Write 1 then write 0 to this bit, reset UART Rx.*/
            uint32_t reserved28                    :    4;
        };
        uint32_t val;
    } clk_conf;
    uint32_t date;
    union {
        struct {
            uint32_t id                            :    30;  /*This register is used to configure the uart_id.*/
            uint32_t high_speed                    :    1;  /*This bit used to select synchronize mode. 1: Registers are auto synchronized into UART Core clock and UART core should be keep the same with APB clock. 0: After configure registers, software needs to write 1 to UART_REG_UPDATE to synchronize registers. */
            uint32_t update                        :    1;  /*Software write 1 would synchronize registers into UART Core clock domain and would be cleared by hardware after synchronization is done.*/
        };
        uint32_t val;
    } id;
} uart_dev_t;
extern uart_dev_t UART0;
extern uart_dev_t UART1;
#ifdef __cplusplus
}
#endif
