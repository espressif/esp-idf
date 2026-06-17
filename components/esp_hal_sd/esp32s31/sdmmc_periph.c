/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/sdmmc_periph.h"
#include "soc/sdmmc_pins.h"

const sdmmc_slot_info_t sdmmc_slot_info[SOC_SDMMC_NUM_SLOTS] = {
    {
        .width = 4,
        .card_detect = SD_CARD_DETECT_N_1_PAD_IN_IDX,
        .write_protect = SD_CARD_WRITE_PRT_1_PAD_IN_IDX,
        .card_int = SD_CARD_INT_N_1_PAD_IN_IDX,
    },
    {
        .width = 4,
        .card_detect = SD_CARD_DETECT_N_2_PAD_IN_IDX,
        .write_protect = SD_CARD_WRITE_PRT_2_PAD_IN_IDX,
        .card_int = SD_CARD_INT_N_2_PAD_IN_IDX,
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
        .d4 = -1,
        .d5 = -1,
        .d6 = -1,
        .d7 = -1,
    },
    {
        .clk = SDMMC_SLOT1_IOMUX_PIN_NUM_CLK,
        .cmd = SDMMC_SLOT1_IOMUX_PIN_NUM_CMD,
        .d0 = SDMMC_SLOT1_IOMUX_PIN_NUM_D0,
        .d1 = SDMMC_SLOT1_IOMUX_PIN_NUM_D1,
        .d2 = SDMMC_SLOT1_IOMUX_PIN_NUM_D2,
        .d3 = SDMMC_SLOT1_IOMUX_PIN_NUM_D3,
        .d4 = -1,
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
    },
};
