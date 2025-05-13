/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SD bus width
 */
typedef enum {
    SD_BUS_WIDTH_1_BIT = 1,    ///< 1 bit
    SD_BUS_WIDTH_4_BIT = 4,    ///< 4 bit
    SD_BUS_WIDTH_8_BIT = 8,    ///< 8 bit
} sd_bus_width_t;

 /**
  * @brief SD mode
  */
 typedef enum {
    SD_MODE_NORMAL,        ///< Normal SD mode
    SD_MODE_UHS1,          ///< UHS-I SD mode
} sd_mode_t;

 /**
  * @brief SD sampling mode
  */
 typedef enum {
    SD_SAMPLING_MODE_SDR,  ///< Single data rate mode
    SD_SAMPLING_MODE_DDR,  ///< Double data rate mode
} sd_sampling_mode_t;
#if SOC_SDMMC_DATA_WIDTH_MAX
#define SDMMC_DATA_SIG_NUM     SOC_SDMMC_DATA_WIDTH_MAX    ///< Number of data signals
#else
#define SDMMC_DATA_SIG_NUM     0                           ///< Number of data signals
#endif

#define SDMMC_DMA_ALIGNMENT    4

#ifdef __cplusplus
}
#endif
