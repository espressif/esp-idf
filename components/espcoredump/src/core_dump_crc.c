/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#if CONFIG_ESP_COREDUMP_CHECKSUM_CRC32

#include "esp_rom_crc.h"
#include "esp_core_dump_types.h"

const static char TAG[] __attribute__((unused)) = "esp_core_dump_crc";

typedef uint32_t core_dump_crc_t;

typedef struct {
    core_dump_crc_t crc;
    uint32_t total_bytes_checksum;  /* Number of bytes used to calculate the checksum */
} core_dump_crc_ctx_t;

static core_dump_crc_ctx_t s_core_dump_crc_ctx = { 0 };

void esp_core_dump_checksum_init(core_dump_checksum_ctx *out_ctx) __attribute__((alias("core_dump_crc_init")));
void esp_core_dump_checksum_update(core_dump_checksum_ctx cks_ctx, void* data, size_t data_len) __attribute__((alias("core_dump_crc_update")));
uint32_t esp_core_dump_checksum_finish(core_dump_checksum_ctx cks_ctx, core_dump_checksum_bytes* chs_ptr) __attribute__((alias("core_dump_crc_finish")));
void esp_core_dump_print_checksum(const char* msg, core_dump_checksum_bytes checksum) __attribute__((alias("core_dump_crc_print")));
uint32_t esp_core_dump_checksum_size(void) __attribute__((alias("core_dump_crc_size")));
uint32_t esp_core_dump_elf_version(void) __attribute__((alias("core_dump_crc_version")));

static void core_dump_crc_print(const char *msg, const uint8_t *checksum)
{
    ESP_COREDUMP_DEBUG_ASSERT(checksum != NULL);

    if (msg != NULL) {
        ESP_COREDUMP_PRINT("%s=", msg);
    }
    ESP_COREDUMP_PRINT("'%08x'\r\n", *((const uint32_t*)checksum));
}

static uint32_t core_dump_crc_size(void)
{
    return sizeof(core_dump_crc_t);
}

static uint32_t core_dump_crc_version(void)
{
    return COREDUMP_VERSION_ELF_CRC32;
}

static void core_dump_crc_init(core_dump_checksum_ctx *out_ctx)
{
    if (out_ctx) {
        s_core_dump_crc_ctx.crc = 0;
        s_core_dump_crc_ctx.total_bytes_checksum = 0;
        *out_ctx = &s_core_dump_crc_ctx;
    }
}

static void core_dump_crc_update(core_dump_checksum_ctx cks_ctx, void* data, size_t data_len)
{
    if (data) {
        core_dump_crc_ctx_t *crc_ctx = cks_ctx;
        crc_ctx->crc = esp_rom_crc32_le(crc_ctx->crc, data, data_len);
        // keep counter of cashed bytes
        crc_ctx->total_bytes_checksum += data_len;
    } else {
        ESP_COREDUMP_LOGE("Empty data to add to checksum calculation!");
    }
}

static uint32_t core_dump_crc_finish(core_dump_checksum_ctx cks_ctx, core_dump_checksum_bytes* chs_ptr)
{
    if (cks_ctx) {
        core_dump_crc_ctx_t *crc_ctx = cks_ctx;

        if (chs_ptr) {
            *chs_ptr = (core_dump_checksum_bytes)&crc_ctx->crc;
        }

        ESP_COREDUMP_LOG_PROCESS("Total length of hashed data: %d", crc_ctx->total_bytes_checksum);
    }

    return core_dump_crc_size();
}

#endif /* CONFIG_ESP_COREDUMP_CHECKSUM_CRC32 */
