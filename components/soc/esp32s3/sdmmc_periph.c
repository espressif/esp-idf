// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
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

#include "soc/sdmmc_periph.h"

const sdmmc_slot_info_t sdmmc_slot_info[SOC_SDMMC_NUM_SLOTS] = {
    {
        .width = 8,
        .card_detect = SDHOST_CARD_DETECT_N_1_IDX,
        .write_protect = SDHOST_CARD_WRITE_PRT_1_IDX,
        .card_int = SDHOST_CARD_INT_N_1_IDX,
    },
    {
        .width = 8,
        .card_detect = SDHOST_CARD_DETECT_N_2_IDX,
        .write_protect = SDHOST_CARD_WRITE_PRT_2_IDX,
        .card_int = SDHOST_CARD_INT_N_2_IDX,
    }
};

const sdmmc_slot_io_info_t sdmmc_slot_gpio_sig[SOC_SDMMC_NUM_SLOTS] = {
    {
        .clk = SDHOST_CCLK_OUT_1_IDX,
        .cmd = SDHOST_CCMD_OUT_1_IDX,
        .d0 = SDHOST_CDATA_OUT_10_IDX,
        .d1 = SDHOST_CDATA_OUT_11_IDX,
        .d2 = SDHOST_CDATA_OUT_12_IDX,
        .d3 = SDHOST_CDATA_OUT_13_IDX,
        .d4 = SDHOST_CDATA_OUT_14_IDX,
        .d5 = SDHOST_CDATA_OUT_15_IDX,
        .d6 = SDHOST_CDATA_OUT_16_IDX,
        .d7 = SDHOST_CDATA_OUT_17_IDX,
    },
    {
        .clk = SDHOST_CCLK_OUT_2_IDX,
        .cmd = SDHOST_CCMD_OUT_2_IDX,
        .d0 = SDHOST_CDATA_OUT_20_IDX,
        .d1 = SDHOST_CDATA_OUT_21_IDX,
        .d2 = SDHOST_CDATA_OUT_22_IDX,
        .d3 = SDHOST_CDATA_OUT_23_IDX,
        .d4 = SDHOST_CDATA_OUT_24_IDX,
        .d5 = SDHOST_CDATA_OUT_25_IDX,
        .d6 = SDHOST_CDATA_OUT_26_IDX,
        .d7 = SDHOST_CDATA_OUT_27_IDX,
    }
};
