/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stddef.h>
#include <stdint.h>

typedef void *bootloader_sha256_handle_t;

bootloader_sha256_handle_t bootloader_sha256_start(void);

void bootloader_sha256_data(bootloader_sha256_handle_t handle, const void *data, size_t data_len);

void bootloader_sha256_finish(bootloader_sha256_handle_t handle, uint8_t *digest);

typedef void mbedtls_sha256_context;

void mbedtls_sha256_init(mbedtls_sha256_context *ctx);

void mbedtls_sha256_free(mbedtls_sha256_context *ctx);

int mbedtls_sha256_starts(mbedtls_sha256_context *ctx, int is224);

int mbedtls_sha256_update(mbedtls_sha256_context *ctx,
                          const unsigned char *input,
                          size_t ilen);
int mbedtls_sha256_finish(mbedtls_sha256_context *ctx,
                          unsigned char *output);
