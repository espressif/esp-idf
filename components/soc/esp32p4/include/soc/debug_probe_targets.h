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
    DEBUG_PROBE_TARGET_HP_MIPI_CSI_HOST = 0,  // MIPI CSI Host
    DEBUG_PROBE_TARGET_HP_AXI_GDMA = 1,       // AXI GDMA
    DEBUG_PROBE_TARGET_HP_I3C = 2,            // I3C
    DEBUG_PROBE_TARGET_HP_TCM = 3,            // HP TCM Memory
    DEBUG_PROBE_TARGET_HP_USB_OTG_FS = 4,     // USB OTG FS (1.1)
    DEBUG_PROBE_TARGET_HP_MIPI_CSI_BRG = 5,   // MIPI CSI Bridge
    DEBUG_PROBE_TARGET_HP_MSPI_PSRAM = 6,     // MSPI PSRAM
    DEBUG_PROBE_TARGET_HP_MIPI_DSI_BRG = 7,   // MIPI DSI Bridge
    DEBUG_PROBE_TARGET_HP_DW_GDMA = 8,        // DW GDMA
    DEBUG_PROBE_TARGET_HP_AXI_ICM = 9,        // AXI ICM matrix
    DEBUG_PROBE_TARGET_HP_L2_MEM = 10,        // L2 Memory
    DEBUG_PROBE_TARGET_HP_BIT_SCRAMBLER = 11, // Bit Scrambler
    DEBUG_PROBE_TARGET_HP_MSPI_FLASH = 12,    // MSPI Flash
    DEBUG_PROBE_TARGET_HP_L1_CACHE = 13,      // L1 Cache
    DEBUG_PROBE_TARGET_HP_CORE = 14,          // HP Core
    DEBUG_PROBE_TARGET_HP_L2_CACHE = 15,      // L2 Cache
    DEBUG_PROBE_TARGET_HP_LP_PROBE_IN = 16,   // LP System Probe In
    DEBUG_PROBE_TARGET_HP_USJ = 17,           // USB Serial JTAG
    DEBUG_PROBE_TARGET_HP_EMAC = 18,          // EMAC
    DEBUG_PROBE_TARGET_HP_JPEG = 19,          // JPEG
    DEBUG_PROBE_TARGET_HP_PPA = 20,           // PPA
    DEBUG_PROBE_TARGET_HP_DMA2D = 21,         // DMA2D
    DEBUG_PROBE_TARGET_HP_LEDC0 = 22,         // LEDC0
    DEBUG_PROBE_TARGET_HP_SDMMC = 23,         // SDMMC
    DEBUG_PROBE_TARGET_HP_ISP = 24,           // ISP
    DEBUG_PROBE_TARGET_HP_USB_OTG_HS = 25,    // USB OTG HS (2.0)
    DEBUG_PROBE_TARGET_HP_H264 = 26,          // H264
    DEBUG_PROBE_TARGET_HP_MCPWM0 = 27,        // MCPWM0
    DEBUG_PROBE_TARGET_HP_MCPWM1 = 28,        // MCPWM1
    DEBUG_PROBE_TARGET_HP_REGDMA = 29,        // REGDMA
    DEBUG_PROBE_TARGET_HP_PVT = 30,           // PVT
} soc_debug_probe_target_hp_t;

/**
 * @brief Target module that we want to attach the LP probe to
 */
typedef enum {
    DEBUG_PROBE_TARGET_LP_PMU = 0,           // LP PMU
    DEBUG_PROBE_TARGET_LP_TCM_RAM = 1,       // LP TCM RAM
    DEBUG_PROBE_TARGET_LP_TCM_ROM = 2,       // LP TCM ROM
    DEBUG_PROBE_TARGET_LP_CORE = 3,          // LP Core
    DEBUG_PROBE_TARGET_LP_ADC = 4,           // LP ADC
    DEBUG_PROBE_TARGET_LP_SPI = 5,           // LP SPI
    DEBUG_PROBE_TARGET_LP_TOUCH = 6,         // LP Touch
    DEBUG_PROBE_TARGET_LP_ANA_PERI = 7,      // LP Analog Peripherals
    DEBUG_PROBE_TARGET_LP_AON_CLKRST = 8,    // LP AON Clock/Reset
    DEBUG_PROBE_TARGET_LP_MISC = 9,          // LP System Misc
    DEBUG_PROBE_TARGET_LP_HP_PROBE_IN = 10,  // HP System Probe In
} soc_debug_probe_target_lp_t;

#ifdef __cplusplus
}
#endif
