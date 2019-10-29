// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "bootloader_sha.h"
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <sys/param.h>

#include "esp32s2beta/rom/sha.h"

static SHA_CTX ctx;

// Words per SHA256 block
// static const size_t BLOCK_WORDS = (64/sizeof(uint32_t));
// Words in final SHA256 digest
// static const size_t DIGEST_WORDS = (32/sizeof(uint32_t));

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
    assert(data_len % 4 == 0);
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
