// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _SOC_UART_STRUCT_H_
#define _SOC_UART_STRUCT_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    union {
        struct {
            uint32_t rw_byte;/*note: rw_byte is a uint8_t field, however, ESP32-S2 do not support 8 bits read/write*/
        };
        uint32_t val;
    } ahb_fifo;
    union {
        struct {
            uint32_t rxfifo_full:              1;
            uint32_t txfifo_empty:             1;
            uint32_t parity_err:               1;
            uint32_t frm_err:                  1;
            uint32_t rxfifo_ovf:               1;
            uint32_t dsr_chg:                  1;
            uint32_t cts_chg:                  1;
            uint32_t brk_det:                  1;
            uint32_t rxfifo_tout:              1;
            uint32_t sw_xon:                   1;
            uint32_t sw_xoff:                  1;
            uint32_t glitch_det:               1;
            uint32_t tx_brk_done:              1;
            uint32_t tx_brk_idle_done:         1;
            uint32_t tx_done:                  1;
            uint32_t rs485_parity_err:         1;
            uint32_t rs485_frm_err:            1;
            uint32_t rs485_clash:              1;
            uint32_t at_cmd_char_det:          1;
            uint32_t wakeup:                   1;
            uint32_t reserved20:              12;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t rxfifo_full:             1;
            uint32_t txfifo_empty:            1;
            uint32_t parity_err:              1;
            uint32_t frm_err:                 1;
            uint32_t rxfifo_ovf:              1;
            uint32_t dsr_chg:                 1;
            uint32_t cts_chg:                 1;
            uint32_t brk_det:                 1;
            uint32_t rxfifo_tout:             1;
            uint32_t sw_xon:                  1;
            uint32_t sw_xoff:                 1;
            uint32_t glitch_det:              1;
            uint32_t tx_brk_done:             1;
            uint32_t tx_brk_idle_done:        1;
            uint32_t tx_done:                 1;
            uint32_t rs485_parity_err:        1;
            uint32_t rs485_frm_err:           1;
            uint32_t rs485_clash:             1;
            uint32_t at_cmd_char_det:         1;
            uint32_t wakeup:                  1;
            uint32_t reserved20:             12;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t rxfifo_full:              1;
            uint32_t txfifo_empty:             1;
            uint32_t parity_err:               1;
            uint32_t frm_err:                  1;
            uint32_t rxfifo_ovf:               1;
            uint32_t dsr_chg:                  1;
            uint32_t cts_chg:                  1;
            uint32_t brk_det:                  1;
            uint32_t rxfifo_tout:              1;
            uint32_t sw_xon:                   1;
            uint32_t sw_xoff:                  1;
            uint32_t glitch_det:               1;
            uint32_t tx_brk_done:              1;
            uint32_t tx_brk_idle_done:         1;
            uint32_t tx_done:                  1;
            uint32_t rs485_parity_err:         1;
            uint32_t rs485_frm_err:            1;
            uint32_t rs485_clash:              1;
            uint32_t at_cmd_char_det:          1;
            uint32_t wakeup:                   1;
            uint32_t reserved20:              12;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t rxfifo_full:              1;
            uint32_t txfifo_empty:             1;
            uint32_t parity_err:               1;
            uint32_t frm_err:                  1;
            uint32_t rxfifo_ovf:               1;
            uint32_t dsr_chg:                  1;
            uint32_t cts_chg:                  1;
            uint32_t brk_det:                  1;
            uint32_t rxfifo_tout:              1;
            uint32_t sw_xon:                   1;
            uint32_t sw_xoff:                  1;
            uint32_t glitch_det:               1;
            uint32_t tx_brk_done:              1;
            uint32_t tx_brk_idle_done:         1;
            uint32_t tx_done:                  1;
            uint32_t rs485_parity_err:         1;
            uint32_t rs485_frm_err:            1;
            uint32_t rs485_clash:              1;
            uint32_t at_cmd_char_det:          1;
            uint32_t wakeup:                   1;
            uint32_t reserved20:              12;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t div_int:    20;
            uint32_t div_frag:    4;
            uint32_t reserved24:  8;
        };
        uint32_t val;
    } clk_div;
    union {
        struct {
            uint32_t en:          1;
            uint32_t reserved1:   7;
            uint32_t glitch_filt: 8;
            uint32_t reserved16: 16;
        };
        uint32_t val;
    } auto_baud;
    union {
        struct {
            uint32_t rxfifo_cnt:10;
            uint32_t reserved10: 3;
            uint32_t dsrn:       1;
            uint32_t ctsn:       1;
            uint32_t rxd:        1;
            uint32_t txfifo_cnt:10;
            uint32_t reserved26: 3;
            uint32_t dtrn:       1;
            uint32_t rtsn:       1;
            uint32_t txd:        1;
        };
        uint32_t val;
    } status;
    union {
        struct {
            uint32_t parity:             1;
            uint32_t parity_en:          1;
            uint32_t bit_num:            2;
            uint32_t stop_bit_num:       2;
            uint32_t sw_rts:             1;
            uint32_t sw_dtr:             1;
            uint32_t txd_brk:            1;
            uint32_t irda_dplx:          1;
            uint32_t irda_tx_en:         1;
            uint32_t irda_wctl:          1;
            uint32_t irda_tx_inv:        1;
            uint32_t irda_rx_inv:        1;
            uint32_t loopback:           1;
            uint32_t tx_flow_en:         1;
            uint32_t irda_en:            1;
            uint32_t rxfifo_rst:         1;
            uint32_t txfifo_rst:         1;
            uint32_t rxd_inv:            1;
            uint32_t cts_inv:            1;
            uint32_t dsr_inv:            1;
            uint32_t txd_inv:            1;
            uint32_t rts_inv:            1;
            uint32_t dtr_inv:            1;
            uint32_t clk_en:             1;
            uint32_t err_wr_mask:        1;
            uint32_t tick_ref_always_on: 1;
            uint32_t mem_clk_en:         1;
            uint32_t reserved29:         3;
        };
        uint32_t val;
    } conf0;
    union {
        struct {
            uint32_t rxfifo_full_thrhd:  9;
            uint32_t txfifo_empty_thrhd: 9;
            uint32_t reserved18:        11;
            uint32_t rx_tout_flow_dis:   1;
            uint32_t rx_flow_en:         1;
            uint32_t rx_tout_en:         1;
        };
        uint32_t val;
    } conf1;
    union {
        struct {
            uint32_t min_cnt:         20;
            uint32_t reserved20:      12;
        };
        uint32_t val;
    } lowpulse;
    union {
        struct {
            uint32_t min_cnt:          20;
            uint32_t reserved20:       12;
        };
        uint32_t val;
    } highpulse;
    union {
        struct {
            uint32_t edge_cnt:    10;
            uint32_t reserved10:  22;
        };
        uint32_t val;
    } rxd_cnt;
    union {
        struct {
            uint32_t sw_flow_con_en: 1;
            uint32_t xonoff_del:     1;
            uint32_t force_xon:      1;
            uint32_t force_xoff:     1;
            uint32_t send_xon:       1;
            uint32_t send_xoff:      1;
            uint32_t reserved6:     26;
        };
        uint32_t val;
    } flow_conf;
    union {
        struct {
            uint32_t active_threshold:10;
            uint32_t reserved10:      22;
        };
        uint32_t val;
    } sleep_conf;
    union {
        struct {
            uint32_t xoff_threshold: 9;
            uint32_t xoff_char:      8;
            uint32_t reserved17:    15;
        };
        uint32_t val;
    } swfc_conf0;
    union {
        struct {
            uint32_t xon_threshold: 9;
            uint32_t xon_char:      8;
            uint32_t reserved17:   15;
        };
        uint32_t val;
    } swfc_conf1;
    union {
        struct {
            uint32_t rx_idle_thrhd:10;
            uint32_t tx_idle_num:  10;
            uint32_t tx_brk_num:    8;
            uint32_t reserved28:    4;
        };
        uint32_t val;
    } idle_conf;
    union {
        struct {
            uint32_t en:               1;
            uint32_t dl0_en:           1;
            uint32_t dl1_en:           1;
            uint32_t tx_rx_en:         1;
            uint32_t rx_busy_tx_en:    1;
            uint32_t rx_dly_num:       1;
            uint32_t tx_dly_num:       4;
            uint32_t reserved10:      22;
        };
        uint32_t val;
    } rs485_conf;
    union {
        struct {
            uint32_t pre_idle_num:16;
            uint32_t reserved16:  16;
        };
        uint32_t val;
    } at_cmd_precnt;
    union {
        struct {
            uint32_t post_idle_num:16;
            uint32_t reserved16:   16;
        };
        uint32_t val;
    } at_cmd_postcnt;
    union {
        struct {
            uint32_t rx_gap_tout:16;
            uint32_t reserved16: 16;
        };
        uint32_t val;
    } at_cmd_gaptout;
    union {
        struct {
            uint32_t data:        8;
            uint32_t char_num:    8;
            uint32_t reserved16: 16;
        };
        uint32_t val;
    } at_cmd_char;
    union {
        struct {
            uint32_t reserved0:     1;
            uint32_t rx_size:       3;
            uint32_t tx_size:       3;
            uint32_t rx_flow_thrhd: 9;
            uint32_t rx_tout_thrhd:10;
            uint32_t force_pd:      1;
            uint32_t force_pu:      1;
            uint32_t reserved28:    4;
        };
        uint32_t val;
    } mem_conf;
    union {
        struct {
            uint32_t apb_tx_waddr:10;
            uint32_t reserved10:   1;
            uint32_t tx_raddr:    10;
            uint32_t reserved21:  11;
        };
        uint32_t val;
    } mem_tx_status;
    union {
        struct {
            uint32_t apb_rx_raddr:10;
            uint32_t reserved10:   1;
            uint32_t rx_waddr:    10;
            uint32_t reserved21:  11;
        };
        uint32_t val;
    } mem_rx_status;
    union {
        struct {
            uint32_t st_urx_out: 4;
            uint32_t st_utx_out: 4;
            uint32_t reserved8: 24;
        };
        uint32_t val;
    } fsm_status;
    union {
        struct {
            uint32_t min_cnt:        20;
            uint32_t reserved20:     12;
        };
        uint32_t val;
    } pospulse;
    union {
        struct {
            uint32_t min_cnt:        20;
            uint32_t reserved20:     12;
        };
        uint32_t val;
    } negpulse;
    uint32_t date;                                  /**/
    uint32_t id;                                    /**/
} uart_dev_t;
extern uart_dev_t UART0;
extern uart_dev_t UART1;
#ifdef __cplusplus
}
#endif

#endif  /* _SOC_UART_STRUCT_H_ */
