/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "esp_err.h"
#include "esp_assert.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define ESP_BOOTLOADER_DESC_MAGIC_BYTE (80)  /*!< The magic byte for the esp_bootloader_desc structure that is in DRAM. */

/**
 * @brief Bootloader description structure
 */
typedef struct {
    uint8_t magic_byte;         /*!< Magic byte ESP_BOOTLOADER_DESC_MAGIC_BYTE */
    uint8_t reserved[3];        /*!< reserved for IDF */
    uint32_t version;           /*!< Bootloader version */
    char idf_ver[32];           /*!< Version IDF */
    char date_time[24];         /*!< Compile date and time*/
    uint8_t reserved2[16];      /*!< reserved for IDF */
} esp_bootloader_desc_t;

/** @cond */
ESP_STATIC_ASSERT(sizeof(esp_bootloader_desc_t) == 80, "esp_bootloader_desc_t should be 80 bytes");
/** @endcond */

/**
 * @brief   Return esp_bootloader_desc structure.
 *
 * Intended for use by the bootloader.
 * @return Pointer to esp_bootloader_desc structure.
 */
const esp_bootloader_desc_t *esp_bootloader_get_description(void);

#ifdef __cplusplus
}
#endif
