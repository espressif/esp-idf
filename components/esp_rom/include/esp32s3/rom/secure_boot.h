// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "rsa_pss.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ets_secure_boot_sig_block;
struct ets_secure_boot_signature_t;

typedef struct ets_secure_boot_sig_block ets_secure_boot_sig_block_t;
typedef struct ets_secure_boot_signature ets_secure_boot_signature_t;
typedef struct ets_secure_boot_key_digests ets_secure_boot_key_digests_t;

/* Verify bootloader image (reconfigures cache to map,
 loads trusted key digests from efuse)

 If allow_key_revoke is true and aggressive revoke efuse is set,
 any failed signature has its associated key revoked in efuse.

 If result is ETS_OK, the "simple hash" of the bootloader
 is copied into verified_hash.
*/
int ets_secure_boot_verify_bootloader(uint8_t *verified_hash, bool allow_key_revoke);

/* Verify bootloader image (reconfigures cache to map), with
   key digests provided as parameters.)

   Can be used to verify secure boot status before enabling
   secure boot permanently.

   If result is ETS_OK, the "simple hash" of the bootloader is
   copied into verified_hash.
*/
int ets_secure_boot_verify_bootloader_with_keys(uint8_t *verified_hash, const ets_secure_boot_key_digests_t *trusted_keys);

/* Verify supplied signature against supplied digest, using
   supplied trusted key digests.

   Doesn't reconfigure cache or any other hardware access.
*/
int ets_secure_boot_verify_signature(const ets_secure_boot_signature_t *sig, const uint8_t *image_digest, const ets_secure_boot_key_digests_t *trusted_keys);

/* Read key digests from efuse. Any revoked/missing digests will be
   marked as NULL

   Returns 0 if at least one valid digest was found.
*/
int ets_secure_boot_read_key_digests(ets_secure_boot_key_digests_t *trusted_keys);

#define ETS_SECURE_BOOT_V2_SIGNATURE_MAGIC 0xE7

/* Secure Boot V2 signature block (up to 3 can be appended) */
struct ets_secure_boot_sig_block {
    uint8_t magic_byte;
    uint8_t version;
    uint8_t _reserved1;
    uint8_t _reserved2;
    uint8_t image_digest[32];
    ets_rsa_pubkey_t key;
    uint8_t signature[384];
    uint32_t block_crc;
    uint8_t _padding[16];
};

_Static_assert(sizeof(ets_secure_boot_sig_block_t) == 1216, "ets_secure_boot_sig_block_t should occupy 1216 Bytes in memory");

#define SECURE_BOOT_NUM_BLOCKS 3

/* V2 Secure boot signature sector (up to 3 blocks) */
struct ets_secure_boot_signature {
    ets_secure_boot_sig_block_t block[SECURE_BOOT_NUM_BLOCKS];
    uint8_t _padding[4096 - (sizeof(ets_secure_boot_sig_block_t) * SECURE_BOOT_NUM_BLOCKS)];
};

_Static_assert(sizeof(ets_secure_boot_signature_t) == 4096, "ets_secure_boot_signature_t should occupy 4096 Bytes in memory");

struct ets_secure_boot_key_digests {
    const void *key_digests[3];
    bool allow_key_revoke;
};

#ifdef __cplusplus
}
#endif
