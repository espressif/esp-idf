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
#ifndef _SOC_I2C_STRUCT_H_
#define _SOC_I2C_STRUCT_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"

typedef volatile struct i2c_dev_s {
    union {
        struct {
            uint32_t period                        :    9;
            uint32_t reserved9                     :    23;
        };
        uint32_t val;
    } scl_low_period;
    union {
        struct {
            uint32_t sda_force_out                 :    1;
            uint32_t scl_force_out                 :    1;
            uint32_t sample_scl_level              :    1;
            uint32_t rx_full_ack_level             :    1;
            uint32_t ms_mode                       :    1;
            uint32_t trans_start                   :    1;
            uint32_t tx_lsb_first                  :    1;
            uint32_t rx_lsb_first                  :    1;
            uint32_t clk_en                        :    1;
            uint32_t arbitration_en                :    1;
            uint32_t fsm_rst                       :    1;
            uint32_t conf_upgate                   :    1;
            uint32_t slv_tx_auto_start_en          :    1;
            uint32_t addr_10bit_rw_check_en        :    1;
            uint32_t addr_broadcasting_en          :    1;
            uint32_t reserved15                    :    17;
        };
        uint32_t val;
    } ctr;
    union {
        struct {
            uint32_t resp_rec                      :    1;
            uint32_t slave_rw                      :    1;
            uint32_t reserved2                     :    1;
            uint32_t arb_lost                      :    1;
            uint32_t bus_busy                      :    1;
            uint32_t slave_addressed               :    1;
            uint32_t reserved6                     :    1;
            uint32_t reserved7                     :    1;
            uint32_t rx_fifo_cnt                   :    6;
            uint32_t stretch_cause                 :    2;
            uint32_t reserved16                    :    2;
            uint32_t tx_fifo_cnt                   :    6;
            uint32_t scl_main_state_last           :    3;
            uint32_t reserved27                    :    1;
            uint32_t scl_state_last                :    3;
            uint32_t reserved31                    :    1;
        };
        uint32_t val;
    } sr;
    union {
        struct {
            uint32_t time_out_value                :    5;
            uint32_t time_out_en                   :    1;
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } timeout;
    union {
        struct {
            uint32_t addr                          :    15;
            uint32_t reserved15                    :    16;
            uint32_t en_10bit                      :    1;
        };
        uint32_t val;
    } slave_addr;
    union {
        struct {
            uint32_t rx_fifo_raddr                 :    5;
            uint32_t rx_fifo_waddr                 :    5;
            uint32_t tx_fifo_raddr                 :    5;
            uint32_t tx_fifo_waddr                 :    5;
            uint32_t reserved20                    :    1;
            uint32_t reserved21                    :    1;
            uint32_t slave_rw_point                :    8;
            uint32_t reserved30                    :    2;
        };
        uint32_t val;
    } fifo_st;
    union {
        struct {
            uint32_t rx_fifo_wm_thrhd              :    5;
            uint32_t tx_fifo_wm_thrhd              :    5;
            uint32_t nonfifo_en                    :    1;
            uint32_t fifo_addr_cfg_en              :    1;
            uint32_t rx_fifo_rst                   :    1;
            uint32_t tx_fifo_rst                   :    1;
            uint32_t fifo_prt_en                   :    1;
            uint32_t reserved15                    :    5;
            uint32_t reserved20                    :    6;
            uint32_t reserved26                    :    1;
            uint32_t reserved27                    :    5;
        };
        uint32_t val;
    } fifo_conf;
    union {
        struct {
            uint32_t data                          :    8;
            uint32_t reserved8                     :    24;
        };
        uint32_t val;
    } fifo_data;
    union {
        struct {
            uint32_t rx_fifo_wm                    :    1;
            uint32_t tx_fifo_wm                    :    1;
            uint32_t rx_fifo_ovf                   :    1;
            uint32_t end_detect                    :    1;
            uint32_t byte_trans_done               :    1;
            uint32_t arbitration_lost              :    1;
            uint32_t mst_tx_fifo_udf               :    1;
            uint32_t trans_complete                :    1;
            uint32_t time_out                      :    1;
            uint32_t trans_start                   :    1;
            uint32_t nack                          :    1;
            uint32_t tx_fifo_ovf                   :    1;
            uint32_t rx_fifo_udf                   :    1;
            uint32_t scl_st_to                     :    1;
            uint32_t scl_main_st_to                :    1;
            uint32_t det_start                     :    1;
            uint32_t slave_stretch                 :    1;
            uint32_t general_call                  :    1;
            uint32_t reserved18                    :    14;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t rx_fifo_wm                    :    1;
            uint32_t tx_fifo_wm                    :    1;
            uint32_t rx_fifo_ovf                   :    1;
            uint32_t end_detect                    :    1;
            uint32_t byte_trans_done               :    1;
            uint32_t arbitration_lost              :    1;
            uint32_t mst_tx_fifo_udf               :    1;
            uint32_t trans_complete                :    1;
            uint32_t time_out                      :    1;
            uint32_t trans_start                   :    1;
            uint32_t nack                          :    1;
            uint32_t tx_fifo_ovf                   :    1;
            uint32_t rx_fifo_udf                   :    1;
            uint32_t scl_st_to                     :    1;
            uint32_t scl_main_st_to                :    1;
            uint32_t det_start                     :    1;
            uint32_t slave_stretch                 :    1;
            uint32_t general_call                  :    1;
            uint32_t reserved18                    :    14;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t rx_fifo_wm                    :    1;
            uint32_t tx_fifo_wm                    :    1;
            uint32_t rx_fifo_ovf                   :    1;
            uint32_t end_detect                    :    1;
            uint32_t byte_trans_done               :    1;
            uint32_t arbitration_lost              :    1;
            uint32_t mst_tx_fifo_udf               :    1;
            uint32_t trans_complete                :    1;
            uint32_t time_out                      :    1;
            uint32_t trans_start                   :    1;
            uint32_t nack                          :    1;
            uint32_t tx_fifo_ovf                   :    1;
            uint32_t rx_fifo_udf                   :    1;
            uint32_t scl_st_to                     :    1;
            uint32_t scl_main_st_to                :    1;
            uint32_t det_start                     :    1;
            uint32_t slave_stretch                 :    1;
            uint32_t general_call                  :    1;
            uint32_t reserved18                    :    14;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t rx_fifo_wm                    :    1;
            uint32_t tx_fifo_wm                    :    1;
            uint32_t rx_fifo_ovf                   :    1;
            uint32_t end_detect                    :    1;
            uint32_t byte_trans_done               :    1;
            uint32_t arbitration_lost              :    1;
            uint32_t mst_tx_fifo_udf               :    1;
            uint32_t trans_complete                :    1;
            uint32_t time_out                      :    1;
            uint32_t trans_start                   :    1;
            uint32_t nack                          :    1;
            uint32_t tx_fifo_ovf                   :    1;
            uint32_t rx_fifo_udf                   :    1;
            uint32_t scl_st_to                     :    1;
            uint32_t scl_main_st_to                :    1;
            uint32_t det_start                     :    1;
            uint32_t slave_stretch                 :    1;
            uint32_t general_call                  :    1;
            uint32_t reserved18                    :    14;
        };
        uint32_t val;
    } int_status;
    union {
        struct {
            uint32_t time                          :    9;
            uint32_t reserved9                     :    23;
        };
        uint32_t val;
    } sda_hold;
    union {
        struct {
            uint32_t time                          :    9;
            uint32_t reserved9                     :    23;
        };
        uint32_t val;
    } sda_sample;
    union {
        struct {
            uint32_t period                        :    9;
            uint32_t scl_wait_high_period          :    7;
            uint32_t reserved16                    :    16;
        };
        uint32_t val;
    } scl_high_period;
    uint32_t reserved_3c;
    union {
        struct {
            uint32_t time                          :    9;
            uint32_t reserved9                     :    23;
        };
        uint32_t val;
    } scl_start_hold;
    union {
        struct {
            uint32_t time                          :    9;
            uint32_t reserved9                     :    23;
        };
        uint32_t val;
    } scl_rstart_setup;
    union {
        struct {
            uint32_t time                          :    9;
            uint32_t reserved9                     :    23;
        };
        uint32_t val;
    } scl_stop_hold;
    union {
        struct {
            uint32_t time                          :    9;
            uint32_t reserved9                     :    23;
        };
        uint32_t val;
    } scl_stop_setup;
    union {
        struct {
            uint32_t scl_thres                     :    4;
            uint32_t sda_thres                     :    4;
            uint32_t scl_en                        :    1;
            uint32_t sda_en                        :    1;
            uint32_t reserved10                    :    22;
        };
        uint32_t val;
    } filter_cfg;
    union {
        struct {
            uint32_t sclk_div_num                  :    8;
            uint32_t sclk_div_a                    :    6;
            uint32_t sclk_div_b                    :    6;
            uint32_t sclk_sel                      :    1;
            uint32_t sclk_active                   :    1;
            uint32_t reserved22                    :    10;
        };
        uint32_t val;
    } clk_conf;
    union {
        struct {
            uint32_t command0                      :    14;
            uint32_t reserved14                    :    17;
            uint32_t command0_done                 :    1;
        };
        uint32_t val;
    } command[8];
    union {
        struct {
            uint32_t scl_st_to                     :    5;  /*no more than 23*/
            uint32_t reserved5                     :    27;
        };
        uint32_t val;
    } scl_st_time_out;
    union {
        struct {
            uint32_t scl_main_st_to                :    5;  /*no more than 23*/
            uint32_t reserved5                     :    27;
        };
        uint32_t val;
    } scl_main_st_time_out;
    union {
        struct {
            uint32_t scl_rst_slv_en                :    1;
            uint32_t scl_rst_slv_num               :    5;
            uint32_t scl_pd_en                     :    1;
            uint32_t sda_pd_en                     :    1;
            uint32_t reserved8                     :    24;
        };
        uint32_t val;
    } scl_sp_conf;
    union {
        struct {
            uint32_t stretch_protect_num           :    10;
            uint32_t slave_scl_stretch_en          :    1;
            uint32_t slave_scl_stretch_clr         :    1;
            uint32_t slave_byte_ack_ctl_en         :    1;
            uint32_t slave_byte_ack_level          :    1;
            uint32_t reserved14                    :    18;
        };
        uint32_t val;
    } scl_stretch_conf;
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
    uint32_t date;
    uint32_t reserved_fc;
    uint32_t txfifo_start_addr;
    uint32_t reserved_104;
    uint32_t reserved_108;
    uint32_t reserved_10c;
    uint32_t reserved_110;
    uint32_t reserved_114;
    uint32_t reserved_118;
    uint32_t reserved_11c;
    uint32_t reserved_120;
    uint32_t reserved_124;
    uint32_t reserved_128;
    uint32_t reserved_12c;
    uint32_t reserved_130;
    uint32_t reserved_134;
    uint32_t reserved_138;
    uint32_t reserved_13c;
    uint32_t reserved_140;
    uint32_t reserved_144;
    uint32_t reserved_148;
    uint32_t reserved_14c;
    uint32_t reserved_150;
    uint32_t reserved_154;
    uint32_t reserved_158;
    uint32_t reserved_15c;
    uint32_t reserved_160;
    uint32_t reserved_164;
    uint32_t reserved_168;
    uint32_t reserved_16c;
    uint32_t reserved_170;
    uint32_t reserved_174;
    uint32_t reserved_178;
    uint32_t reserved_17c;
    uint32_t rxfifo_start_addr;
} i2c_dev_t;
extern i2c_dev_t I2C0;
#ifdef __cplusplus
}
#endif



#endif /*_SOC_I2C_STRUCT_H_ */
