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
#ifndef _SOC_I2C_STRUCT_H_
#define _SOC_I2C_STRUCT_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    union {
        struct {
            uint32_t period:        14;
            uint32_t reserved14:    18;
        };
        uint32_t val;
    } scl_low_period;
    union {
        struct {
            uint32_t sda_force_out:    1;
            uint32_t scl_force_out:    1;
            uint32_t sample_scl_level: 1;
            uint32_t ack_level:        1;
            uint32_t ms_mode:          1;
            uint32_t trans_start:      1;
            uint32_t tx_lsb_first:     1;
            uint32_t rx_lsb_first:     1;
            uint32_t clk_en:           1;
            uint32_t arbitration_en:   1;
            uint32_t fsm_rst:          1;
            uint32_t ref_always_on:    1;
            uint32_t reserved12:      20;
        };
        uint32_t val;
    } ctr;
    union {
        struct {
            uint32_t ack_rec:             1;
            uint32_t slave_rw:            1;
            uint32_t time_out:            1;
            uint32_t arb_lost:            1;
            uint32_t bus_busy:            1;
            uint32_t slave_addressed:     1;
            uint32_t byte_trans:          1;
            uint32_t reserved7:           1;
            uint32_t rx_fifo_cnt:         6;
            uint32_t reserved14:          4;
            uint32_t tx_fifo_cnt:         6;
            uint32_t scl_main_state_last: 3;
            uint32_t reserved27:          1;
            uint32_t scl_state_last:      3;
            uint32_t reserved31:          1;
        };
        uint32_t val;
    } status_reg;
    union {
        struct {
            uint32_t tout:       24;
            uint32_t time_out_en: 1;
            uint32_t reserved25:  7;
        };
        uint32_t val;
    } timeout;
    union {
        struct {
            uint32_t addr:         15;
            uint32_t reserved15:   16;
            uint32_t en_10bit:      1;
        };
        uint32_t val;
    } slave_addr;
    union {
        struct {
            uint32_t rx_fifo_start_addr: 5;
            uint32_t rx_fifo_end_addr:  5;
            uint32_t tx_fifo_start_addr: 5;
            uint32_t tx_fifo_end_addr:  5;
            uint32_t rx_update:         1;
            uint32_t tx_update:         1;
            uint32_t tx_fifo_init_raddr: 5;
            uint32_t rx_fifo_init_waddr: 5;
        };
        uint32_t val;
    } fifo_st;
    union {
        struct {
            uint32_t rx_fifo_full_thrhd: 5;
            uint32_t tx_fifo_empty_thrhd: 5;
            uint32_t nonfifo_en:         1;
            uint32_t fifo_addr_cfg_en:   1;
            uint32_t rx_fifo_rst:        1;
            uint32_t tx_fifo_rst:        1;
            uint32_t nonfifo_rx_thres:   6;
            uint32_t nonfifo_tx_thres:   6;
            uint32_t reserved26:         6;
        };
        uint32_t val;
    } fifo_conf;
    union {
        struct {
            uint8_t data;
            uint8_t reserved[3];
        };
        uint32_t val;
    } fifo_data;
    union {
        struct {
            uint32_t rx_fifo_full:             1;
            uint32_t tx_fifo_empty:            1;
            uint32_t rx_fifo_ovf:              1;
            uint32_t end_detect:               1;
            uint32_t slave_tran_comp:          1;
            uint32_t arbitration_lost:         1;
            uint32_t master_tran_comp:         1;
            uint32_t trans_complete:           1;
            uint32_t time_out:                 1;
            uint32_t trans_start:              1;
            uint32_t ack_err:                  1;
            uint32_t rx_rec_full:              1;
            uint32_t tx_send_empty:            1;
            uint32_t scl_st_to:                1;
            uint32_t scl_main_st_to:           1;
            uint32_t det_start:                1;
            uint32_t reserved16:              16;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t rx_fifo_full:             1;
            uint32_t tx_fifo_empty:            1;
            uint32_t rx_fifo_ovf:              1;
            uint32_t end_detect:               1;
            uint32_t slave_tran_comp:          1;
            uint32_t arbitration_lost:         1;
            uint32_t master_tran_comp:         1;
            uint32_t trans_complete:           1;
            uint32_t time_out:                 1;
            uint32_t trans_start:              1;
            uint32_t ack_err:                  1;
            uint32_t rx_rec_full:              1;
            uint32_t tx_send_empty:            1;
            uint32_t scl_st_to:                1;
            uint32_t scl_main_st_to:           1;
            uint32_t det_start:                1;
            uint32_t reserved16:              16;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t rx_fifo_full:             1;
            uint32_t tx_fifo_empty:            1;
            uint32_t rx_fifo_ovf:              1;
            uint32_t end_detect:               1;
            uint32_t slave_tran_comp:          1;
            uint32_t arbitration_lost:         1;
            uint32_t master_tran_comp:         1;
            uint32_t trans_complete:           1;
            uint32_t time_out:                 1;
            uint32_t trans_start:              1;
            uint32_t ack_err:                  1;
            uint32_t rx_rec_full:              1;
            uint32_t tx_send_empty:            1;
            uint32_t scl_st_to:                1;
            uint32_t scl_main_st_to:           1;
            uint32_t det_start:                1;
            uint32_t reserved16:              16;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t rx_fifo_full:            1;
            uint32_t tx_fifo_empty:           1;
            uint32_t rx_fifo_ovf:             1;
            uint32_t end_detect:              1;
            uint32_t slave_tran_comp:         1;
            uint32_t arbitration_lost:        1;
            uint32_t master_tran_comp:        1;
            uint32_t trans_complete:          1;
            uint32_t time_out:                1;
            uint32_t trans_start:             1;
            uint32_t ack_err:                 1;
            uint32_t rx_rec_full:             1;
            uint32_t tx_send_empty:           1;
            uint32_t scl_st_to:               1;
            uint32_t scl_main_st_to:          1;
            uint32_t det_start:               1;
            uint32_t reserved16:             16;
        };
        uint32_t val;
    } int_status;
    union {
        struct {
            uint32_t time:         10;
            uint32_t reserved10:   22;
        };
        uint32_t val;
    } sda_hold;
    union {
        struct {
            uint32_t time:           10;
            uint32_t reserved10:     22;
        };
        uint32_t val;
    } sda_sample;
    union {
        struct {
            uint32_t period:              14;
            uint32_t scl_wait_high_period:14;
            uint32_t reserved28:           4;
        };
        uint32_t val;
    } scl_high_period;
    uint32_t reserved_3c;
    union {
        struct {
            uint32_t time:               10;
            uint32_t reserved10:         22;
        };
        uint32_t val;
    } scl_start_hold;
    union {
        struct {
            uint32_t time:                 10;
            uint32_t reserved10:           22;
        };
        uint32_t val;
    } scl_rstart_setup;
    union {
        struct {
            uint32_t time:              14;
            uint32_t reserved14:        18;
        };
        uint32_t val;
    } scl_stop_hold;
    union {
        struct {
            uint32_t time:               10;
            uint32_t reserved10:         22;
        };
        uint32_t val;
    } scl_stop_setup;
    union {
        struct {
            uint32_t thres:            3;
            uint32_t en:               1;
            uint32_t reserved4:       28;
        };
        uint32_t val;
    } scl_filter_cfg;
    union {
        struct {
            uint32_t thres:            3;
            uint32_t en:               1;
            uint32_t reserved4:       28;
        };
        uint32_t val;
    } sda_filter_cfg;
    union {
        struct {
            uint32_t byte_num:      8;              /*Byte_num represent the number of data need to be send or data need to be received.*/
            uint32_t ack_en:        1;              /*ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
            uint32_t ack_exp:       1;              /*ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
            uint32_t ack_val:       1;              /*ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
            uint32_t op_code:       3;              /*op_code is the command  0：RSTART   1：WRITE  2：READ  3：STOP . 4:END.*/
            uint32_t reserved14:   17;
            uint32_t done:  1;                      /*When command0 is done in I2C Master mode  this bit changes to high level.*/
        };
        uint32_t val;
    } command[16];
    union {
        struct {
            uint32_t scl_st_to: 24;
            uint32_t reserved24: 8;
        };
        uint32_t val;
    } scl_st_time_out;
    union {
        struct {
            uint32_t scl_main_st_to:24;
            uint32_t reserved24:     8;
        };
        uint32_t val;
    } scl_main_st_time_out;
    union {
        struct {
            uint32_t scl_rst_slv_en:  1;
            uint32_t scl_rst_slv_num: 5;
            uint32_t scl_pd_en:       1;
            uint32_t sda_pd_en:       1;
            uint32_t reserved8:      24;
        };
        uint32_t val;
    } scl_sp_conf;
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
    uint32_t reserved_cc;
    uint32_t reserved_d0;
    uint32_t reserved_d4;
    uint32_t reserved_d8;
    uint32_t reserved_dc;
    uint32_t reserved_e0;
    uint32_t reserved_e4;
    uint32_t reserved_e8;
    uint32_t reserved_ec;
    uint32_t reserved_f0;
    uint32_t reserved_f4;
    uint32_t date;                                  /**/
    uint32_t reserved_fc;
    uint32_t ram_data[32];                       /**/
} i2c_dev_t;
extern i2c_dev_t I2C0;
extern i2c_dev_t I2C1;
#ifdef __cplusplus
}
#endif

#endif  /* _SOC_I2C_STRUCT_H_ */
