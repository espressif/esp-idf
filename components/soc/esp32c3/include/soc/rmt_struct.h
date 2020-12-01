// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _SOC_RMT_STRUCT_H_
#define _SOC_RMT_STRUCT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    uint32_t data_ch[4];                                   /**/
    union {
        struct {
            uint32_t tx_start:           1;
            uint32_t mem_rd_rst:         1;
            uint32_t mem_rst:            1;
            uint32_t tx_conti_mode:      1;
            uint32_t mem_tx_wrap_en:     1;
            uint32_t idle_out_lv:        1;
            uint32_t idle_out_en:        1;
            uint32_t tx_stop:            1;
            uint32_t div_cnt:            8;
            uint32_t mem_size:           3;
            uint32_t reserved19:         1;
            uint32_t carrier_eff_en:     1;
            uint32_t carrier_en:         1;
            uint32_t carrier_out_lv:     1;
            uint32_t afifo_rst:          1;
            uint32_t conf_update:        1;
            uint32_t reserved25:         7;
        };
        uint32_t val;
    } tx_conf[2];
    struct {
        union {
            struct {
                uint32_t div_cnt:            8;
                uint32_t idle_thres:        15;
                uint32_t mem_size:           3;
                uint32_t reserved26:         2;
                uint32_t carrier_en:         1;
                uint32_t carrier_out_lv:     1;
                uint32_t reserved30:         2;
            };
            uint32_t val;
        } conf0;
        union {
            struct {
                uint32_t rx_en:               1;
                uint32_t mem_wr_rst:          1;
                uint32_t mem_rst:             1;
                uint32_t mem_owner:           1;
                uint32_t rx_filter_en:        1;
                uint32_t rx_filter_thres:     8;
                uint32_t mem_rx_wrap_en:      1;
                uint32_t afifo_rst:           1;
                uint32_t conf_update:         1;
                uint32_t reserved16:         16;
            };
            uint32_t val;
        } conf1;
    } rx_conf[2];
    union {
        struct {
            uint32_t mem_raddr_ex:       9;
            uint32_t state:              3;
            uint32_t waddr:              9;
            uint32_t mem_rd_err:         1;
            uint32_t mem_empty:          1;
            uint32_t mem_wr_err:         1;
            uint32_t raddr:              8;
        };
        uint32_t val;
    } tx_status[2];
    union {
        struct {
            uint32_t mem_waddr_ex:       9;
            uint32_t reserved9:          3;
            uint32_t mem_raddr:          9;
            uint32_t reserved21:         1;
            uint32_t state:              3;
            uint32_t mem_owner_err:      1;
            uint32_t mem_full:           1;
            uint32_t mem_rd_err:         1;
            uint32_t reserved28:         4;
        };
        uint32_t val;
    } rx_status[2];
    union {
        struct {
            uint32_t ch0_tx_end:               1;
            uint32_t ch1_tx_end:               1;
            uint32_t ch2_rx_end:               1;
            uint32_t ch3_rx_end:               1;
            uint32_t ch0_err:                  1;
            uint32_t ch1_err:                  1;
            uint32_t ch2_err:                  1;
            uint32_t ch3_err:                  1;
            uint32_t ch0_tx_thr_event:         1;
            uint32_t ch1_tx_thr_event:         1;
            uint32_t ch2_rx_thr_event:         1;
            uint32_t ch3_rx_thr_event:         1;
            uint32_t ch0_tx_loop:              1;
            uint32_t ch1_tx_loop:              1;
            uint32_t reserved14:              18;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t ch0_tx_end:              1;
            uint32_t ch1_tx_end:              1;
            uint32_t ch2_rx_end:              1;
            uint32_t ch3_rx_end:              1;
            uint32_t ch0_err:                 1;
            uint32_t ch1_err:                 1;
            uint32_t ch2_err:                 1;
            uint32_t ch3_err:                 1;
            uint32_t ch0_tx_thr_event:        1;
            uint32_t ch1_tx_thr_event:        1;
            uint32_t ch2_rx_thr_event:        1;
            uint32_t ch3_rx_thr_event:        1;
            uint32_t ch0_tx_loop:             1;
            uint32_t ch1_tx_loop:             1;
            uint32_t reserved14:             18;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t ch0_tx_end:               1;
            uint32_t ch1_tx_end:               1;
            uint32_t ch2_rx_end:               1;
            uint32_t ch3_rx_end:               1;
            uint32_t ch0_err:                  1;
            uint32_t ch1_err:                  1;
            uint32_t ch2_err:                  1;
            uint32_t ch3_err:                  1;
            uint32_t ch0_tx_thr_event:         1;
            uint32_t ch1_tx_thr_event:         1;
            uint32_t ch2_rx_thr_event:         1;
            uint32_t ch3_rx_thr_event:         1;
            uint32_t ch0_tx_loop:              1;
            uint32_t ch1_tx_loop:              1;
            uint32_t reserved14:              18;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t ch0_tx_end:               1;
            uint32_t ch1_tx_end:               1;
            uint32_t ch2_rx_end:               1;
            uint32_t ch3_rx_end:               1;
            uint32_t ch0_err:                  1;
            uint32_t ch1_err:                  1;
            uint32_t ch2_err:                  1;
            uint32_t ch3_err:                  1;
            uint32_t ch0_tx_thr_event:         1;
            uint32_t ch1_tx_thr_event:         1;
            uint32_t ch2_rx_thr_event:         1;
            uint32_t ch3_rx_thr_event:         1;
            uint32_t ch0_tx_loop:              1;
            uint32_t ch1_tx_loop:              1;
            uint32_t reserved14:              18;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t low:             16;
            uint32_t high:            16;
        };
        uint32_t val;
    } tx_carrier[2];
    union {
        struct {
            uint32_t low_thres:             16;
            uint32_t high_thres:            16;
        };
        uint32_t val;
    } rx_carrier[2];
    union {
        struct {
            uint32_t limit:                9;
            uint32_t tx_loop_num:         10;
            uint32_t tx_loop_cnt_en:       1;
            uint32_t loop_count_reset:     1;
            uint32_t reserved21:          11;
        };
        uint32_t val;
    } tx_lim[2];
    union {
        struct {
            uint32_t rx_lim:     9;
            uint32_t reserved9: 23;
        };
        uint32_t val;
    } rx_lim[2];
    union {
        struct {
            uint32_t fifo_mask:        1;
            uint32_t mem_clk_force_on: 1;
            uint32_t mem_force_pd:     1;
            uint32_t mem_force_pu:     1;
            uint32_t sclk_div_num:     8;
            uint32_t sclk_div_a:       6;
            uint32_t sclk_div_b:       6;
            uint32_t sclk_sel:         2;
            uint32_t sclk_active:      1;
            uint32_t reserved27:       4;
            uint32_t clk_en:           1;
        };
        uint32_t val;
    } sys_conf;
    union {
        struct {
            uint32_t ch0:        1;
            uint32_t ch1:        1;
            uint32_t en:         1;
            uint32_t reserved3: 29;
        };
        uint32_t val;
    } tx_sim;
    union {
        struct {
            uint32_t ch0:             1;
            uint32_t ch1:             1;
            uint32_t ch2:             1;
            uint32_t ch3:             1;
            uint32_t reserved4:      28;
        };
        uint32_t val;
    } ref_cnt_rst;
    uint32_t reserved_74;
    uint32_t reserved_78;
    uint32_t reserved_7c;
    uint32_t reserved_80;
    uint32_t reserved_84;
    uint32_t reserved_88;
    uint32_t reserved_8c;
    uint32_t reserved_90;
    uint32_t reserved_94;
    uint32_t reserved_98;
    uint32_t reserved_9c;
    uint32_t reserved_a0;
    uint32_t reserved_a4;
    uint32_t reserved_a8;
    uint32_t reserved_ac;
    uint32_t reserved_b0;
    uint32_t reserved_b4;
    uint32_t reserved_b8;
    uint32_t reserved_bc;
    uint32_t reserved_c0;
    uint32_t reserved_c4;
    uint32_t reserved_c8;
    union {
        struct {
            uint32_t date:      28;
            uint32_t reserved28: 4;
        };
        uint32_t val;
    } date;
} rmt_dev_t;

extern rmt_dev_t RMT;

typedef struct {
    union {
        struct {
            uint32_t duration0 : 15;
            uint32_t level0 : 1;
            uint32_t duration1 : 15;
            uint32_t level1 : 1;
        };
        uint32_t val;
    };
} rmt_item32_t;

//Allow access to RMT memory using RMTMEM.chan[0].data32[8]
typedef volatile struct {
    struct {
        union {
            rmt_item32_t data32[48];
        };
    } chan[4];
} rmt_mem_t;

extern rmt_mem_t RMTMEM;

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_RMT_STRUCT_H_ */
