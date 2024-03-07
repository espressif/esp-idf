/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include <sys/lock.h>

#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "esp_private/esp_crypto_lock_internal.h"
#include "esp_private/periph_ctrl.h"

#include "sha/sha_block.h"
#include "hal/sha_hal.h"
#include "hal/sha_ll.h"


static _lock_t s_sha_lock;

void esp_sha_write_digest_state(esp_sha_type sha_type, void *digest_state)
{
    sha_hal_write_digest(sha_type, digest_state);
}

void esp_sha_read_digest_state(esp_sha_type sha_type, void *digest_state)
{
    sha_hal_read_digest(sha_type, digest_state);
}

/* Return block size (in bytes) for a given SHA type */
inline static size_t block_length(esp_sha_type type)
{
    switch (type) {
    case SHA1:
    case SHA2_224:
    case SHA2_256:
        return 64;
#if SOC_SHA_SUPPORT_SHA384
    case SHA2_384:
#endif
#if SOC_SHA_SUPPORT_SHA512
    case SHA2_512:
#endif
#if SOC_SHA_SUPPORT_SHA512_T
    case SHA2_512224:
    case SHA2_512256:
    case SHA2_512T:
#endif
        return 128;
    default:
        return 0;
    }
}


/* Lock the SHA peripheral and then enable it */
void esp_sha_acquire_hardware()
{
    _lock_acquire(&s_sha_lock); /* Released when releasing hw with esp_sha_release_hardware() */

    SHA_RCC_ATOMIC() {
        sha_ll_enable_bus_clock(true);
        sha_ll_reset_register();
    }
}

/* Disable SHA peripheral block and then release it */
void esp_sha_release_hardware()
{
    SHA_RCC_ATOMIC() {
        sha_ll_enable_bus_clock(false);
    }

    _lock_release(&s_sha_lock);
}


void esp_sha_block(esp_sha_type sha_type, const void *data_block, bool is_first_block)
{
    sha_hal_hash_block(sha_type, data_block, block_length(sha_type) / 4, is_first_block);
}
