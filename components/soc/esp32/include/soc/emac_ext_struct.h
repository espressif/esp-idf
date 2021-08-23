// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef volatile struct emac_ext_dev_s {
    union {
        struct {
            uint32_t div_num : 4;
            uint32_t h_div_num : 4;
            uint32_t dly_num : 2;
            uint32_t reserved10 : 22;
        };
        uint32_t val;
    } ex_clkout_conf;
    union {
        struct {
            uint32_t div_num_10m : 6;
            uint32_t h_div_num_10m : 6;
            uint32_t div_num_100m : 6;
            uint32_t h_div_num_100m : 6;
            uint32_t clk_sel : 1;
            uint32_t reserved25 : 7;
        };
        uint32_t val;
    } ex_oscclk_conf;
    union {
        struct {
            uint32_t ext_en : 1;
            uint32_t int_en : 1;
            uint32_t rx_125_clk_en : 1;
            uint32_t mii_clk_tx_en : 1;
            uint32_t mii_clk_rx_en : 1;
            uint32_t clk_en : 1;
            uint32_t reserved6 : 26;
        };
        uint32_t val;
    } ex_clk_ctrl;
    union {
        struct {
            uint32_t int_revmii_rx_clk_sel : 1;
            uint32_t ext_revmii_rx_clk_sel : 1;
            uint32_t sbd_flowctrl : 1;
            uint32_t core_phy_addr : 5;
            uint32_t revmii_phy_addr : 5;
            uint32_t phy_intf_sel : 3;
            uint32_t ss_mode : 1;
            uint32_t sbd_clk_gating_en : 1;
            uint32_t pmt_ctrl_en : 1;
            uint32_t scr_smi_dly_rx_sync : 1;
            uint32_t tx_err_out_en : 1;
            uint32_t reserved21 : 11;
        };
        uint32_t val;
    } ex_phyinf_conf;
    union {
        struct {
            uint32_t ram_pd_en : 2;
            uint32_t reserved2 : 30;
        };
        uint32_t val;
    } pd_sel;
    uint32_t reserved_14;
    uint32_t reserved_18;
    uint32_t reserved_1c;
    uint32_t reserved_20;
    uint32_t reserved_24;
    uint32_t reserved_28;
    uint32_t reserved_2c;
    uint32_t reserved_30;
    uint32_t reserved_34;
    uint32_t reserved_38;
    uint32_t reserved_3c;
    uint32_t reserved_40;
    uint32_t reserved_44;
    uint32_t reserved_48;
    uint32_t reserved_4c;
    uint32_t reserved_50;
    uint32_t reserved_54;
    uint32_t reserved_58;
    uint32_t reserved_5c;
    uint32_t reserved_60;
    uint32_t reserved_64;
    uint32_t reserved_68;
    uint32_t reserved_6c;
    uint32_t reserved_70;
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
    uint32_t reserved_f8;
    uint32_t ex_date;
} emac_ext_dev_t;

extern emac_ext_dev_t EMAC_EXT;

#ifdef __cplusplus
}
#endif
