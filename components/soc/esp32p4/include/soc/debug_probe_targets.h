/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Target module that we want to attach the probe to
 *
 * @note The target module ID can be obtained from the prob_grp15[7:0]
 */
typedef enum {
    DEBUG_PROBE_TARGET_MIPI_CSI_HOST = 0,  // MIPI CSI Host
    DEBUG_PROBE_TARGET_AXI_GDMA = 1,       // AXI GDMA
    DEBUG_PROBE_TARGET_I3C = 2,            // I3C
    DEBUG_PROBE_TARGET_HP_TCM = 3,         // HP TCM Memory
    DEBUG_PROBE_TARGET_USB_OTG_FS = 4,     // USB OTG FS (1.1)
    DEBUG_PROBE_TARGET_MIPI_CSI_BRG = 5,   // MIPI CSI Bridge
    DEBUG_PROBE_TARGET_MSPI_PSRAM = 6,     // MSPI PSRAM
    DEBUG_PROBE_TARGET_MIPI_DSI_BRG = 7,   // MIPI DSI Bridge
    DEBUG_PROBE_TARGET_DW_GDMA = 8,        // DW GDMA
    DEBUG_PROBE_TARGET_AXI_ICM = 9,        // AXI ICM matrix
    DEBUG_PROBE_TARGET_L2_MEM = 10,        // L2 Memory
    DEBUG_PROBE_TARGET_BIT_SCRAMBLER = 11, // Bit Scrambler
    DEBUG_PROBE_TARGET_MSPI_FLASH = 12,    // MSPI Flash
    DEBUG_PROBE_TARGET_L1_CACHE = 13,      // L1 Cache
    DEBUG_PROBE_TARGET_HP_CORE = 14,       // HP Core
    DEBUG_PROBE_TARGET_L2_CACHE = 15,      // L2 Cache
    DEBUG_PROBE_TARGET_LP_PROBE_IN = 16,   // LP System Probe In
    DEBUG_PROBE_TARGET_USJ = 17,           // USB Serial JTAG
    DEBUG_PROBE_TARGET_EMAC = 18,          // EMAC
    DEBUG_PROBE_TARGET_JPEG = 19,          // JPEG
    DEBUG_PROBE_TARGET_PPA = 20,           // PPA
    DEBUG_PROBE_TARGET_DMA2D = 21,         // DMA2D
    DEBUG_PROBE_TARGET_LEDC = 22,          // LEDC
    DEBUG_PROBE_TARGET_SDMMC = 23,         // SDMMC
    DEBUG_PROBE_TARGET_ISP = 24,           // ISP
    DEBUG_PROBE_TARGET_USB_OTG_HS = 25,    // USB OTG HS (2.0)
    DEBUG_PROBE_TARGET_H264 = 26,          // H264
    DEBUG_PROBE_TARGET_MCPWM0 = 27,        // MCPWM0
    DEBUG_PROBE_TARGET_MCPWM1 = 28,        // MCPWM1
    DEBUG_PROBE_TARGET_REGDMA = 29,        // REGDMA
    DEBUG_PROBE_TARGET_PVT = 30,           // PVT
} soc_debug_probe_target_t;

#ifdef __cplusplus
}
#endif
