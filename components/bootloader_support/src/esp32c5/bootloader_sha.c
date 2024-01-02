/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bootloader_sha.h"
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <sys/param.h>

#include "esp32c5/rom/sha.h"

static SHA_CTX ctx;

bootloader_sha256_handle_t bootloader_sha256_start()
{
    // Enable SHA hardware
    ets_sha_enable();
    ets_sha_init(&ctx, SHA2_256);
    return &ctx; // Meaningless non-NULL value
}

void bootloader_sha256_data(bootloader_sha256_handle_t handle, const void *data, size_t data_len)
{
    assert(handle != NULL);
    /* C5 secure boot key field consists of 1 byte of curve identifier and 64 bytes of ECDSA public key.
     * While verifying the signature block, we need to calculate the SHA of this key field which is of 65 bytes.
     * ets_sha_update handles it cleanly so we can safely remove the check:
     * assert(data_len % 4) == 0
     */
    ets_sha_update(&ctx, data, data_len, false);
}

void bootloader_sha256_finish(bootloader_sha256_handle_t handle, uint8_t *digest)
{
    assert(handle != NULL);

    if (digest == NULL) {
        bzero(&ctx, sizeof(ctx));
        return;
    }
    ets_sha_finish(&ctx, digest);
}
