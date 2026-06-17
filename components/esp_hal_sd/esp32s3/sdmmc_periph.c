/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/sdmmc_periph.h"
#include "soc/sdmmc_pins.h"

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

const sdmmc_slot_io_info_t sdmmc_slot_gpio_num[SOC_SDMMC_NUM_SLOTS] = {
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
