/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "sdkconfig.h"

/**
 * The MD5 functions calculate a 128-bit cryptographic digest for any number of input bytes.
 */
#define ESP_ROM_MD5_DIGEST_LEN 16

#if CONFIG_IDF_TARGET_ESP32C2
/**
 * \brief          MD5 context structure
 *
 * \warning        MD5 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
typedef struct mbedtls_md5_context {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[4];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
} md5_context_t;
/* Functions extracted from ROM, do not use it as an public API */
void esp_rom_mbedtls_md5_starts_ret(md5_context_t *context);
void esp_rom_mbedtls_md5_update_ret(md5_context_t *context, const void *buf, uint32_t len);
void esp_rom_mbedtls_md5_finish_ret(md5_context_t *context, uint8_t *digest);

/**
 * @brief Initialize the MD5 context
 *
 * @param context Context object allocated by user
 */
static inline void esp_rom_md5_init(md5_context_t *context)
{
    esp_rom_mbedtls_md5_starts_ret(context);
}

/**
 * @brief Running MD5 algorithm over input data
 *
 * @param context MD5 context which has been initialized by `MD5Init`
 * @param buf Input buffer
 * @param len Buffer length in bytes
 */
static inline void esp_rom_md5_update(md5_context_t *context, const void *buf, uint32_t len)
{
    esp_rom_mbedtls_md5_update_ret(context, buf, len);
}

/**
 * @brief Extract the MD5 result, and erase the context
 *
 * @param digest Where to store the 128-bit digest value
 * @param context MD5 context
 */
static inline void esp_rom_md5_final(uint8_t *digest, md5_context_t *context)
{
    esp_rom_mbedtls_md5_finish_ret(context, digest);
}

#else //#if !CONFIG_IDF_TARGET_ESP32C2
/**
 * @brief Type defined for MD5 context
 *
 */
typedef struct MD5Context {
    uint32_t buf[4];
    uint32_t bits[2];
    uint8_t in[64];
} md5_context_t;

/**
 * @brief Initialize the MD5 context
 *
 * @param context Context object allocated by user
 */
void esp_rom_md5_init(md5_context_t *context);

/**
 * @brief Running MD5 algorithm over input data
 *
 * @param context MD5 context which has been initialized by `MD5Init`
 * @param buf Input buffer
 * @param len Buffer length in bytes
 */
void esp_rom_md5_update(md5_context_t *context, const void *buf, uint32_t len);

/**
 * @brief Extract the MD5 result, and erase the context
 *
 * @param digest Where to store the 128-bit digest value
 * @param context MD5 context
 */
void esp_rom_md5_final(uint8_t *digest, md5_context_t *context);

#endif

#ifdef __cplusplus
}
#endif
