/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#if CONFIG_ESP_COREDUMP_CHECKSUM_SHA256

#include <string.h>
#include "esp_core_dump_types.h"

const static char TAG[] __attribute__((unused)) = "esp_core_dump_sha";

void esp_core_dump_checksum_init(core_dump_checksum_ctx cks_ctx) __attribute__((alias("core_dump_sha_init")));
void esp_core_dump_checksum_update(core_dump_checksum_ctx cks_ctx, void* data, size_t data_len) __attribute__((alias("core_dump_sha_update")));
uint32_t esp_core_dump_checksum_finish(core_dump_checksum_ctx cks_ctx, core_dump_checksum_bytes* chs_ptr) __attribute__((alias("core_dump_sha_finish")));
void esp_core_dump_print_checksum(const char* msg, core_dump_checksum_bytes checksum) __attribute__((alias("core_dump_sha256_print")));
uint32_t esp_core_dump_checksum_size(void) __attribute__((alias("core_dump_sha_size")));
uint32_t esp_core_dump_elf_version(void) __attribute__((alias("core_dump_sha_version")));

#if CONFIG_IDF_TARGET_ESP32

static void core_dump_sha256_start(core_dump_sha_ctx_t *sha_ctx)
{
    mbedtls_sha256_init(&sha_ctx->ctx);
    mbedtls_sha256_starts(&sha_ctx->ctx, false);
}

static void core_dump_sha256_update(core_dump_sha_ctx_t *sha_ctx, const void *data, size_t data_len)
{
    // set software mode of SHA calculation
#if CONFIG_MBEDTLS_HARDWARE_SHA
    sha_ctx->ctx.mode = ESP_MBEDTLS_SHA256_SOFTWARE;
#endif
    mbedtls_sha256_update(&sha_ctx->ctx, data, data_len);
}

static void core_dump_sha256_finish(core_dump_sha_ctx_t *sha_ctx)
{
    mbedtls_sha256_finish(&sha_ctx->ctx, sha_ctx->result);
    mbedtls_sha256_free(&sha_ctx->ctx);
}

#else

static void core_dump_sha256_start(core_dump_sha_ctx_t *sha_ctx)
{
    /* Enable SHA hardware */
    ets_sha_enable();
    ets_sha_init(&sha_ctx->ctx, SHA2_256);
}

static void core_dump_sha256_update(core_dump_sha_ctx_t *sha_ctx, const void *data, size_t data_len)
{
    ets_sha_update(&sha_ctx->ctx, data, data_len, false);
}

static void core_dump_sha256_finish(core_dump_sha_ctx_t *sha_ctx)
{
    ets_sha_finish(&sha_ctx->ctx, sha_ctx->result);
    ets_sha_disable();
}

#endif

static void core_dump_sha256_print(const char* msg, const uint8_t *checksum)
{
    /* As this function is only called by `esp_core_dump_print_checksum`, we
     * have the guarantee that checksum is not NULL. */
    if (msg != NULL) {
        ESP_COREDUMP_PRINT("%s=", msg);
    }

    ESP_COREDUMP_PRINT("'");
    for (int i = 0; i < COREDUMP_SHA256_LEN; i++) {
        ESP_COREDUMP_PRINT("%02x", checksum[i]);
    }
    ESP_COREDUMP_PRINT("'\r\n");
}

static uint32_t core_dump_sha_size(void)
{
    return COREDUMP_SHA256_LEN;
}

static uint32_t core_dump_sha_version(void)
{
    return COREDUMP_VERSION_ELF_SHA256;
}

static void core_dump_sha_init(core_dump_checksum_ctx cks_ctx)
{
    if (cks_ctx) {
        core_dump_sha_ctx_t *sha_ctx = cks_ctx;
        core_dump_sha256_start(sha_ctx);
        sha_ctx->total_bytes_checksum = 0;
    }
}

static void core_dump_sha_update(core_dump_checksum_ctx cks_ctx, void* data, size_t data_len)
{
    if (data) {
        core_dump_sha_ctx_t *sha_ctx = cks_ctx;
        core_dump_sha256_update(sha_ctx, data, data_len);
        // keep counter of cashed bytes
        sha_ctx->total_bytes_checksum += data_len;
    } else {
        ESP_COREDUMP_LOGE("Empty data to add to checksum calculation!");
    }
}

static uint32_t core_dump_sha_finish(core_dump_checksum_ctx cks_ctx, core_dump_checksum_bytes* chs_ptr)
{
    if (cks_ctx) {
        core_dump_sha_ctx_t *sha_ctx = cks_ctx;
        core_dump_sha256_finish(sha_ctx);
        if (chs_ptr) {
            *chs_ptr = (core_dump_checksum_bytes)&sha_ctx->result;
        }
        ESP_COREDUMP_LOG_PROCESS("Total length of hashed data: %d", sha_ctx->total_bytes_checksum);
    }

    return core_dump_sha_size();
}

#endif /* CONFIG_ESP_COREDUMP_CHECKSUM_SHA256 */
