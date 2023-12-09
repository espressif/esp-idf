/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
//include soc related (generated) definitions
#include "soc/soc_caps.h"
#include "soc/soc_pins.h"
#if SOC_SDMMC_HOST_SUPPORTED
#include "soc/sdmmc_reg.h"
#include "soc/sdmmc_struct.h"
#include "soc/gpio_sig_map.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_SDMMC_HOST_SUPPORTED

/**
 * Common SDMMC slot info, doesn't depend on SOC_SDMMC_USE_{IOMUX,GPIO_MATRIX}
 */
typedef struct {
    uint8_t width;          /*!< Maximum supported slot width (1, 4, 8) */
    uint8_t card_detect;    /*!< Card detect signal in GPIO Matrix */
    uint8_t write_protect;  /*!< Write protect signal in GPIO Matrix */
    uint8_t card_int;       /*!< Card interrupt signal in GPIO Matrix */
} sdmmc_slot_info_t;

/** Width and GPIO matrix signal numbers for auxillary SD host signals, one structure per slot */
extern const sdmmc_slot_info_t sdmmc_slot_info[SOC_SDMMC_NUM_SLOTS];

/**
 * This structure lists pin numbers (if SOC_SDMMC_USE_IOMUX is set)
 * or GPIO Matrix signal numbers (if SOC_SDMMC_USE_GPIO_MATRIX is set)
 * for the SD bus signals. Field names match SD bus signal names.
 */
typedef struct {
    uint8_t clk;
    uint8_t cmd;
    uint8_t d0;
    uint8_t d1;
    uint8_t d2;
    uint8_t d3;
    uint8_t d4;
    uint8_t d5;
    uint8_t d6;
    uint8_t d7;
} sdmmc_slot_io_info_t;

/** GPIO pin numbers of SD bus signals, one structure per slot */
extern const sdmmc_slot_io_info_t sdmmc_slot_gpio_num[SOC_SDMMC_NUM_SLOTS];

/** GPIO matrix signal numbers of SD bus signals, one structure per slot */
extern const sdmmc_slot_io_info_t sdmmc_slot_gpio_sig[SOC_SDMMC_NUM_SLOTS];

#endif // SOC_SDMMC_HOST_SUPPORTED
#ifdef __cplusplus
}
#endif
