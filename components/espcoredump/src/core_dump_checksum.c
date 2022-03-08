/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * @file
 * @brief Checksum interface implemetation
 *
 * This file is an implementation for the coredump checksum interface defined
 * in "core_dump_checksum.h".
 * Please refer to this file for more information about the functions.
 */

#include <stddef.h>
#include "esp_core_dump_port_impl.h"
#include "esp_core_dump_types.h"
#include "core_dump_checksum.h"
#include "esp_attr.h"

#if CONFIG_ESP_COREDUMP_CHECKSUM_CRC32
#include "esp_rom_crc.h"
#elif CONFIG_ESP_COREDUMP_CHECKSUM_SHA256
#include "mbedtls/sha256.h"
#endif

#if CONFIG_ESP_COREDUMP_ENABLE

const static DRAM_ATTR char TAG[] __attribute__((unused)) = "esp_core_dump_checksum";

#define COREDUMP_SHA256_LEN                 32

typedef uint32_t core_dump_crc_t;

struct core_dump_checksum_ctx {
#if CONFIG_ESP_COREDUMP_CHECKSUM_SHA256
    mbedtls_sha256_context  ctx;
    uint8_t sha_output[COREDUMP_SHA256_LEN];
#elif CONFIG_ESP_COREDUMP_CHECKSUM_CRC32
    core_dump_crc_t crc;
#endif
    /* Number of bytes used to calculate the checksum */
    uint32_t total_bytes_checksum;
};

static core_dump_checksum_ctx s_checksum_context = { 0 };

/**
 * Get ELF core dump version.
 * Please check esp checksum interface for more details.
 */
uint32_t esp_core_dump_elf_version(void)
{
#if CONFIG_ESP_COREDUMP_CHECKSUM_CRC32
    return COREDUMP_VERSION_ELF_CRC32;
#elif CONFIG_ESP_COREDUMP_CHECKSUM_SHA256
    return COREDUMP_VERSION_ELF_SHA256;
#endif
}

void esp_core_dump_checksum_init(core_dump_checksum_ctx** out_ctx)
{
    if (out_ctx) {
#if CONFIG_ESP_COREDUMP_CHECKSUM_CRC32
        s_checksum_context.crc = 0;
#elif CONFIG_ESP_COREDUMP_CHECKSUM_SHA256
        mbedtls_sha256_init(&s_checksum_context.ctx);
        (void)mbedtls_sha256_starts(&s_checksum_context.ctx, 0);
#endif
        s_checksum_context.total_bytes_checksum = 0;

        *out_ctx = &s_checksum_context;
    }
}


void esp_core_dump_checksum_update(core_dump_checksum_ctx* cks_ctx, void* data, size_t data_len)
{
    ESP_COREDUMP_DEBUG_ASSERT(cks_ctx);

    if (data) {
#if CONFIG_ESP_COREDUMP_CHECKSUM_CRC32
        cks_ctx->crc = esp_rom_crc32_le(cks_ctx->crc, data, data_len);
#elif CONFIG_ESP_COREDUMP_CHECKSUM_SHA256
#if CONFIG_MBEDTLS_HARDWARE_SHA
        // set software mode of SHA calculation
        cks_ctx->ctx.mode = ESP_MBEDTLS_SHA256_SOFTWARE;
#endif
        (void)mbedtls_sha256_update(&cks_ctx->ctx, data, data_len);
#endif
        // keep counter of cashed bytes
        cks_ctx->total_bytes_checksum += data_len;
    } else {
        ESP_COREDUMP_LOGE("Empty data to add to checksum calculation!");
    }
}


uint32_t esp_core_dump_checksum_finish(core_dump_checksum_ctx* cks_ctx, core_dump_checksum_bytes* chs_ptr)
{
    uint32_t chs_len = 0;

    /* cks_ctx pointer can be NULL only if chs_ptr is also NULL. */
    ESP_COREDUMP_DEBUG_ASSERT(cks_ctx != NULL || chs_ptr == NULL);

#if CONFIG_ESP_COREDUMP_CHECKSUM_CRC32
    if (chs_ptr != NULL) {
        *chs_ptr = (core_dump_checksum_bytes) &cks_ctx->crc;
    }
    chs_len = sizeof(cks_ctx->crc);

#elif CONFIG_ESP_COREDUMP_CHECKSUM_SHA256
    if (chs_ptr != NULL) {
        (void)mbedtls_sha256_finish(&cks_ctx->ctx, (uint8_t*)&cks_ctx->sha_output);
        *chs_ptr = &cks_ctx->sha_output[0];
        mbedtls_sha256_free(&cks_ctx->ctx);
    }
    chs_len = sizeof(cks_ctx->sha_output);

#endif

    if (cks_ctx) {
        ESP_COREDUMP_LOG_PROCESS("Total length of hashed data: %d", cks_ctx->total_bytes_checksum);
    }

    return chs_len;
}


/**
 * Returns the size, in bytes, of the checksums.
 * Currently, this function is just an alias to esp_core_dump_checksum_finish
 * function, which can return the size of the checksum if given parameters
 * are NULL. However, the implementation can evolve in the future independently
 * from esp_core_dump_checksum_finish function.
 */
uint32_t esp_core_dump_checksum_size(void)
{
    return esp_core_dump_checksum_finish(NULL, NULL);
}


#if CONFIG_ESP_COREDUMP_CHECKSUM_SHA256

static void esp_core_dump_print_sha256(const char* msg, const uint8_t* sha_output)
{
    /* As this function is only called by `esp_core_dump_print_checksum`, we
     * have the guarantee that sha_output is not NULL. */
    if (msg != NULL) {
        esp_rom_printf(DRAM_STR("%s='"), msg);
    }

    for (int i = 0; i < COREDUMP_SHA256_LEN; i++) {
        esp_rom_printf(DRAM_STR("%02x"), sha_output[i]);
    }
    esp_rom_printf(DRAM_STR("'\r\n"));
}

#endif


/**
 * Prints a message and a checksum given as parameters.
 * This function is useful when the caller isn't explicitly aware of which
 * checksum type (CRC32, SHA256, etc) is being used.
 */
void esp_core_dump_print_checksum(const char* msg, core_dump_checksum_bytes checksum)
{
    ESP_COREDUMP_DEBUG_ASSERT(checksum != NULL);

#if CONFIG_ESP_COREDUMP_CHECKSUM_CRC32
    if (msg != NULL) {
        esp_rom_printf(DRAM_STR("%s='"), msg);
    }
    esp_rom_printf(DRAM_STR("%08x"), *((const uint32_t*) checksum));
    esp_rom_printf(DRAM_STR("'\r\n"));
#elif CONFIG_ESP_COREDUMP_CHECKSUM_SHA256
    esp_core_dump_print_sha256(msg, (const uint8_t*) checksum);
#endif
}


#endif
