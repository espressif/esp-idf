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
    SD_SAMPLING_MODE_SDR = 1,  ///< Single data rate mode
    SD_SAMPLING_MODE_DDR,      ///< Double data rate mode
} sd_sampling_mode_t;

/**
 * @brief SD/MMC Host clock timing delay phases
 *
 * This will only take effect when the host works in
 * - SDMMC_FREQ_HIGHSPEED
 * - SDMMC_FREQ_52M
 * - SDR50
 * - DDR50
 * - SDR104
 * Driver will print out how long the delay is, in picosecond (ps).
 */
typedef enum {
    SDMMC_DELAY_PHASE_0 = 1,        /*!< Delay phase 0 */
    SDMMC_DELAY_PHASE_1,            /*!< Delay phase 1 */
    SDMMC_DELAY_PHASE_2,            /*!< Delay phase 2 */
    SDMMC_DELAY_PHASE_3,            /*!< Delay phase 3 */
    SDMMC_DELAY_PHASE_4,            /*!< Delay phase 4 */
    SDMMC_DELAY_PHASE_5,            /*!< Delay phase 5 */
    SDMMC_DELAY_PHASE_6,            /*!< Delay phase 6 */
    SDMMC_DELAY_PHASE_7,            /*!< Delay phase 7 */
    SDMMC_DELAY_PHASE_AUTO,         /*!< Auto detect phase, only valid for UHS-I modes */
} sdmmc_delay_phase_t;

/**
 * @brief SD/MMC Host clock timing delay lines
 */
typedef enum {
    SDMMC_DELAY_LINE_0 = 1,         /*!< Delay line 0 */
    SDMMC_DELAY_LINE_1,             /*!< Delay line 1 */
    SDMMC_DELAY_LINE_2,             /*!< Delay line 2 */
    SDMMC_DELAY_LINE_3,             /*!< Delay line 3 */
    SDMMC_DELAY_LINE_4,             /*!< Delay line 4 */
    SDMMC_DELAY_LINE_5,             /*!< Delay line 5 */
    SDMMC_DELAY_LINE_6,             /*!< Delay line 6 */
    SDMMC_DELAY_LINE_7,             /*!< Delay line 7 */
    SDMMC_DELAY_LINE_AUTO,          /*!< Auto detect line */
} sdmmc_delay_line_t;

#if SOC_SDMMC_DATA_WIDTH_MAX
#define SDMMC_DATA_SIG_NUM     SOC_SDMMC_DATA_WIDTH_MAX    ///< Number of data signals
#else
#define SDMMC_DATA_SIG_NUM     0                           ///< Number of data signals
#endif

#define SDMMC_DMA_ALIGNMENT    4

#ifdef __cplusplus
}
#endif
