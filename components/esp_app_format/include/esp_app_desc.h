/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "esp_err.h"

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
_Static_assert(sizeof(esp_app_desc_t) == 256, "esp_app_desc_t should be 256 bytes");
/** @endcond */

/**
 * @brief   Return esp_app_desc structure. This structure includes app version.
 *
 * Return description for running app.
 * @return Pointer to esp_app_desc structure.
 */
const esp_app_desc_t *esp_app_get_description(void);

/**
 * @brief   Fill the provided buffer with SHA256 of the ELF file, formatted as hexadecimal, null-terminated.
 * If the buffer size is not sufficient to fit the entire SHA256 in hex plus a null terminator,
 * the largest possible number of bytes will be written followed by a null.
 * @param dst   Destination buffer
 * @param size  Size of the buffer
 * @return      Number of bytes written to dst (including null terminator)
 */
int esp_app_get_elf_sha256(char* dst, size_t size);

#ifdef __cplusplus
}
#endif
