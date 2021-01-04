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

#pragma once
#include <stdint.h>
//include soc related (generated) definitions
#include "soc/soc_caps.h"
#include "soc/soc_pins.h"
#include "soc/sdmmc_reg.h"
#include "soc/sdmmc_struct.h"
#include "soc/gpio_sig_map.h"

#ifdef __cplusplus
extern "C" {
#endif

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

/* Note: it is in theory possible to have both IOMUX and GPIO Matrix supported
 * in the same SoC. However this is not used on any SoC at this point, and would
 * complicate the driver. Hence only one of these options is supported at a time.
 */
#if SOC_SDMMC_USE_IOMUX
/** GPIO pin numbers of SD bus signals, one structure per slot */
extern const sdmmc_slot_io_info_t sdmmc_slot_gpio_num[SOC_SDMMC_NUM_SLOTS];

#elif SOC_SDMMC_USE_GPIO_MATRIX
/** GPIO matrix signal numbers of SD bus signals, one structure per slot */
extern const sdmmc_slot_io_info_t sdmmc_slot_gpio_sig[SOC_SDMMC_NUM_SLOTS];

#endif // SOC_SDMMC_USE_{IOMUX,GPIO_MATRIX}

#ifdef __cplusplus
}
#endif
