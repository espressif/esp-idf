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
            uint32_t reserved8 : 24;
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
            uint32_t reserved2 : 1;
            uint32_t mii_clk_tx_en : 1;
            uint32_t mii_clk_rx_en : 1;
            uint32_t reserved5 : 27;
        };
        uint32_t val;
    } ex_clk_ctrl;
    union {
        struct {
            uint32_t reserved0 : 13;
            uint32_t phy_intf_sel : 3;
            uint32_t reserved16 : 16;
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
} emac_ext_dev_t;

extern emac_ext_dev_t EMAC_EXT;

#ifdef __cplusplus
}
#endif
