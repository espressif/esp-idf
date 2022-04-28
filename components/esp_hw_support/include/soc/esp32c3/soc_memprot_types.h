/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

//////////////////////////////////////////////////////////
// ESP32-C3 PMS memory protection types
//

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Memory types recognized by PMS
 */
typedef enum {
    MEMPROT_TYPE_NONE                           = 0x00000000,
    MEMPROT_TYPE_IRAM0_SRAM                     = 0x00000001,
    MEMPROT_TYPE_DRAM0_SRAM                     = 0x00000002,
    MEMPROT_TYPE_IRAM0_RTCFAST                  = 0x00000004,
    MEMPROT_TYPE_ALL                            = 0x7FFFFFFF,
    MEMPROT_TYPE_INVALID                        = 0x80000000,
    MEMPROT_TYPE_IRAM0_ANY                      = MEMPROT_TYPE_IRAM0_SRAM | MEMPROT_TYPE_IRAM0_RTCFAST
} esp_mprot_mem_t;

/**
 * @brief Splitting address (line) type
 */
typedef enum {
    MEMPROT_SPLIT_ADDR_NONE                     = 0x00000000,
    MEMPROT_SPLIT_ADDR_IRAM0_DRAM0              = 0x00000001,
    MEMPROT_SPLIT_ADDR_IRAM0_LINE_0             = 0x00000002,
    MEMPROT_SPLIT_ADDR_IRAM0_LINE_1             = 0x00000004,
    MEMPROT_SPLIT_ADDR_DRAM0_DMA_LINE_0         = 0x00000008,
    MEMPROT_SPLIT_ADDR_DRAM0_DMA_LINE_1         = 0x00000010,
    MEMPROT_SPLIT_ADDR_ALL                      = 0x7FFFFFFF,
    MEMPROT_SPLIT_ADDR_INVALID                  = 0x80000000,
    MEMPROT_SPLIT_ADDR_MAIN                     = MEMPROT_SPLIT_ADDR_IRAM0_DRAM0
} esp_mprot_split_addr_t;

/**
 * @brief PMS area type (memory space between adjacent splitting addresses or above/below the main splt.address)
 */
typedef enum {
    MEMPROT_PMS_AREA_NONE                       = 0x00000000,
    MEMPROT_PMS_AREA_IRAM0_0                    = 0x00000001,
    MEMPROT_PMS_AREA_IRAM0_1                    = 0x00000002,
    MEMPROT_PMS_AREA_IRAM0_2                    = 0x00000004,
    MEMPROT_PMS_AREA_IRAM0_3                    = 0x00000008,
    MEMPROT_PMS_AREA_DRAM0_0                    = 0x00000010,
    MEMPROT_PMS_AREA_DRAM0_1                    = 0x00000020,
    MEMPROT_PMS_AREA_DRAM0_2                    = 0x00000040,
    MEMPROT_PMS_AREA_DRAM0_3                    = 0x00000080,
    MEMPROT_PMS_AREA_IRAM0_RTCFAST_LO           = 0x00000100,
    MEMPROT_PMS_AREA_IRAM0_RTCFAST_HI           = 0x00000200,
    MEMPROT_PMS_AREA_ALL                        = 0x7FFFFFFF,
    MEMPROT_PMS_AREA_INVALID                    = 0x80000000
} esp_mprot_pms_area_t;

/**
* @brief Memory protection configuration
*/
typedef struct {
    bool invoke_panic_handler;  /*!< Register PMS violation interrupt for panic-handling */
    bool lock_feature;          /*!< Lock all PMS settings */
    void *split_addr;           /*!< Main I/D splitting address */
    uint32_t mem_type_mask;     /*!< Memory types required to protect. See esp_mprot_mem_t enum */
} esp_memp_config_t;

#define ESP_MEMPROT_DEFAULT_CONFIG() { \
    .invoke_panic_handler = true, \
    .lock_feature = true, \
    .split_addr = NULL, \
    .mem_type_mask = MEMPROT_TYPE_ALL \
}

/**
 * @brief Converts Memory protection type to string
 *
 * @param mem_type Memory protection type
 */
static inline const char *esp_mprot_mem_type_to_str(const esp_mprot_mem_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_TYPE_NONE:
        return "NONE";
    case MEMPROT_TYPE_IRAM0_SRAM:
        return "IRAM0_SRAM";
    case MEMPROT_TYPE_DRAM0_SRAM:
        return "DRAM0_SRAM";
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        return "IRAM0_RTCFAST";
    case MEMPROT_TYPE_IRAM0_ANY:
        return "IRAM0_ANY";
    case MEMPROT_TYPE_ALL:
        return "ALL";
    default:
        return "INVALID";
    }
}

/**
 * @brief Converts Splitting address type to string
 *
 * @param line_type Split line type
 */
static inline const char *esp_mprot_split_addr_to_str(const esp_mprot_split_addr_t line_type)
{
    switch (line_type) {
    case MEMPROT_SPLIT_ADDR_NONE:
        return "SPLIT_ADDR_NONE";
    case MEMPROT_SPLIT_ADDR_IRAM0_DRAM0:
        return "SPLIT_ADDR_IRAM0_DRAM0";
    case MEMPROT_SPLIT_ADDR_IRAM0_LINE_0:
        return "SPLIT_ADDR_IRAM0_LINE_0";
    case MEMPROT_SPLIT_ADDR_IRAM0_LINE_1:
        return "SPLIT_ADDR_IRAM0_LINE_1";
    case MEMPROT_SPLIT_ADDR_DRAM0_DMA_LINE_0:
        return "SPLIT_ADDR_DRAM0_DMA_LINE_0";
    case MEMPROT_SPLIT_ADDR_DRAM0_DMA_LINE_1:
        return "SPLIT_ADDR_DRAM0_DMA_LINE_1";
    case MEMPROT_SPLIT_ADDR_ALL:
        return "SPLIT_ADDR_ALL";
    default:
        return "SPLIT_ADDR_INVALID";
    }
}

/**
 * @brief Converts PMS Area type to string
 *
 * @param area_type PMS Area type
 */
static inline const char *esp_mprot_pms_area_to_str(const esp_mprot_pms_area_t area_type)
{
    switch (area_type) {
    case MEMPROT_PMS_AREA_NONE:
        return "PMS_AREA_NONE";
    case MEMPROT_PMS_AREA_IRAM0_0:
        return "PMS_AREA_IRAM0_0";
    case MEMPROT_PMS_AREA_IRAM0_1:
        return "PMS_AREA_IRAM0_1";
    case MEMPROT_PMS_AREA_IRAM0_2:
        return "PMS_AREA_IRAM0_2";
    case MEMPROT_PMS_AREA_IRAM0_3:
        return "PMS_AREA_IRAM0_3";
    case MEMPROT_PMS_AREA_DRAM0_0:
        return "PMS_AREA_DRAM0_0";
    case MEMPROT_PMS_AREA_DRAM0_1:
        return "PMS_AREA_DRAM0_1";
    case MEMPROT_PMS_AREA_DRAM0_2:
        return "PMS_AREA_DRAM0_2";
    case MEMPROT_PMS_AREA_DRAM0_3:
        return "PMS_AREA_DRAM0_3";
    case MEMPROT_PMS_AREA_IRAM0_RTCFAST_LO:
        return "PMS_AREA_IRAM0_RTCFAST_LO";
    case MEMPROT_PMS_AREA_IRAM0_RTCFAST_HI:
        return "PMS_AREA_IRAM0_RTCFAST_HI";
    case MEMPROT_PMS_AREA_ALL:
        return "PMS_AREA_ALL";
    default:
        return "PMS_AREA_INVALID";
    }
}

#ifdef __cplusplus
}
#endif
