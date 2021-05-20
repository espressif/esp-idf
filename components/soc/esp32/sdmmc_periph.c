// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

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
        .card_detect = HOST_CARD_DETECT_N_1_IDX,
        .write_protect = HOST_CARD_WRITE_PRT_1_IDX,
        .card_int = HOST_CARD_INT_N_1_IDX,
    },
    {
        .width = 4,
        .card_detect = HOST_CARD_DETECT_N_2_IDX,
        .write_protect = HOST_CARD_WRITE_PRT_2_IDX,
        .card_int = HOST_CARD_INT_N_2_IDX,
    }
};

const sdmmc_slot_io_info_t sdmmc_slot_gpio_num[SOC_SDMMC_NUM_SLOTS] = {
    {
        .clk = SDMMC_SLOT0_IOMUX_PIN_NUM_CLK,
        .cmd = SDMMC_SLOT0_IOMUX_PIN_NUM_CMD,
        .d0 = SDMMC_SLOT0_IOMUX_PIN_NUM_D0,
        .d1 = SDMMC_SLOT0_IOMUX_PIN_NUM_D1,
        .d2 = SDMMC_SLOT0_IOMUX_PIN_NUM_D2,
        .d3 = SDMMC_SLOT0_IOMUX_PIN_NUM_D3,
        .d4 = SDMMC_SLOT0_IOMUX_PIN_NUM_D4,
        .d5 = SDMMC_SLOT0_IOMUX_PIN_NUM_D5,
        .d6 = SDMMC_SLOT0_IOMUX_PIN_NUM_D6,
        .d7 = SDMMC_SLOT0_IOMUX_PIN_NUM_D7,
    },
    {
        .clk = SDMMC_SLOT1_IOMUX_PIN_NUM_CLK,
        .cmd = SDMMC_SLOT1_IOMUX_PIN_NUM_CMD,
        .d0 = SDMMC_SLOT1_IOMUX_PIN_NUM_D0,
        .d1 = SDMMC_SLOT1_IOMUX_PIN_NUM_D1,
        .d2 = SDMMC_SLOT1_IOMUX_PIN_NUM_D2,
        .d3 = SDMMC_SLOT1_IOMUX_PIN_NUM_D3,
        .d4 = -1,  //slot1 has no D4-7
        .d5 = -1,
        .d6 = -1,
        .d7 = -1,
    }
};
