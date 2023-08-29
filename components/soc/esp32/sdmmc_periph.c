/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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

const sdmmc_slot_io_info_t sdmmc_slot_gpio_sig[SOC_SDMMC_NUM_SLOTS] = {
    {
        .clk = -1,
        .cmd = -1,
        .d0 = -1,
        .d1 = -1,
        .d2 = -1,
        .d3 = -1,
        .d4 = -1,
        .d5 = -1,
        .d6 = -1,
        .d7 = -1,
    },
    {
        .clk = -1,
        .cmd = -1,
        .d0 = -1,
        .d1 = -1,
        .d2 = -1,
        .d3 = -1,
        .d4 = -1,
        .d5 = -1,
        .d6 = -1,
        .d7 = -1,
    }
};
