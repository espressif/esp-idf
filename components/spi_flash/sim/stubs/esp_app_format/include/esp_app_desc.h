/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This is a STUB FILE used when compiling ESP-IDF to run tests on the host system.
 * The source file used normally for ESP-IDF has the same name but is located elsewhere.
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

#define ESP_APP_DESC_MAGIC_WORD (0xABCD5432)  /*!< The magic word for the esp_app_desc structure that is in DROM. */

/**
 * @brief Description about application.
 */
typedef struct {
    uint32_t magic_word;        /*!< Magic word ESP_APP_DESC_MAGIC_WORD */
    uint32_t secure_version;    /*!< Secure version */
    uint32_t reserv1[2];        /*!< reserv1 */
    char version[32];           /*!< Application version */
    char project_name[32];      /*!< Project name */
    char time[16];              /*!< Compile time */
    char date[16];              /*!< Compile date*/
    char idf_ver[32];           /*!< Version IDF */
    uint8_t app_elf_sha256[32]; /*!< sha256 of elf file */
    uint32_t reserv2[20];       /*!< reserv2 */
} esp_app_desc_t;

/** @cond */
ESP_STATIC_ASSERT(sizeof(esp_app_desc_t) == 256, "esp_app_desc_t should be 256 bytes");
/** @endcond */

#ifdef __cplusplus
}
#endif
