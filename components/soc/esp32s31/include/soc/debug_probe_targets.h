/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Target module that we want to attach the HP probe to
 */
typedef enum {
    DEBUG_PROBE_TARGET_HP_USB_OTG_HS = 0,     // USB OTG HS
    DEBUG_PROBE_TARGET_HP_AXI_GDMA = 1,       // AXI GDMA
    DEBUG_PROBE_TARGET_HP_TCM = 3,            // HP TCM RAM
    DEBUG_PROBE_TARGET_HP_MSPI_PSRAM = 6,     // MSPI PSRAM
    DEBUG_PROBE_TARGET_HP_AXI_ICM = 9,        // HP ICM matrix
    DEBUG_PROBE_TARGET_HP_BIT_SCRAMBLER = 11, // Bit Scrambler
    DEBUG_PROBE_TARGET_HP_MSPI_FLASH = 12,    // MSPI Flash
    DEBUG_PROBE_TARGET_HP_CACHE = 13,         // Cache
    DEBUG_PROBE_TARGET_HP_CORE = 14,          // HP Core
    DEBUG_PROBE_TARGET_HP_LP_PROBE_IN = 16,   // LP System Probe In
    DEBUG_PROBE_TARGET_HP_EMAC = 18,          // GMAC
    DEBUG_PROBE_TARGET_HP_JPEG = 19,          // JPEG
    DEBUG_PROBE_TARGET_HP_PPA = 20,           // PPA
    DEBUG_PROBE_TARGET_HP_DMA2D = 21,         // DMA2D
    DEBUG_PROBE_TARGET_HP_LEDC0 = 22,         // LEDC0
    DEBUG_PROBE_TARGET_HP_SDMMC = 23,         // SDMMC
    DEBUG_PROBE_TARGET_HP_LEDC1 = 24,         // LEDC1
    DEBUG_PROBE_TARGET_HP_MCPWM2 = 25,        // MCPWM2
    DEBUG_PROBE_TARGET_HP_MCPWM3 = 26,        // MCPWM3
    DEBUG_PROBE_TARGET_HP_MCPWM0 = 27,        // MCPWM0
    DEBUG_PROBE_TARGET_HP_MCPWM1 = 28,        // MCPWM1
    DEBUG_PROBE_TARGET_HP_REGDMA = 29,        // REGDMA
    DEBUG_PROBE_TARGET_HP_ZERO_DET = 30,      // Zero detection
} soc_debug_probe_target_hp_t;

/**
 * @brief Target module that we want to attach the LP probe to
 */
typedef enum {
    DEBUG_PROBE_TARGET_LP_PMU = 0,           // LP PMU
    DEBUG_PROBE_TARGET_LP_TCM = 1,           // LP TCM
    DEBUG_PROBE_TARGET_LP_TSENS = 2,         // LP TSENS
    DEBUG_PROBE_TARGET_LP_CORE = 3,          // LP Core
    DEBUG_PROBE_TARGET_LP_ADC = 4,           // LP ADC
    DEBUG_PROBE_TARGET_LP_SPI = 5,           // LP SPI
    DEBUG_PROBE_TARGET_LP_TOUCH = 6,         // LP Touch
    DEBUG_PROBE_TARGET_LP_ANA_PERI = 7,      // LP Analog Peripherals
    DEBUG_PROBE_TARGET_LP_AON_CLKRST = 8,    // LP AON Clock/Reset
    DEBUG_PROBE_TARGET_LP_TRNG = 9,          // LP TRNG
    DEBUG_PROBE_TARGET_LP_MISC = 10,         // LP System Misc
    DEBUG_PROBE_TARGET_LP_HP_PROBE_IN = 11,  // HP System Probe In
} soc_debug_probe_target_lp_t;

#ifdef __cplusplus
}
#endif
