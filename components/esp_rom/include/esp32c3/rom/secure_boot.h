/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "ets_sys.h"
#include "rsa_pss.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ets_secure_boot_sig_block ets_secure_boot_sig_block_t;
typedef struct ets_secure_boot_signature ets_secure_boot_signature_t;
typedef struct ets_secure_boot_key_digests ets_secure_boot_key_digests_t;

/* Anti-FI measure: use full words for success/fail, instead of
   0/non-zero
*/
typedef enum {
    SB_SUCCESS = 0x3A5A5AA5,
    SB_FAILED = 0x7533885E,
} ets_secure_boot_status_t;


/* Verify and stage-load the bootloader image
   (reconfigures cache to map, loads trusted key digests from efuse,
   copies the bootloader into the staging buffer.)

   If allow_key_revoke is true and aggressive revoke efuse is set,
   any failed signature has its associated key revoked in efuse.

   If result is SB_SUCCESS, the "simple hash" of the bootloader
   is copied into verified_hash.
*/
ets_secure_boot_status_t ets_secure_boot_verify_stage_bootloader(uint8_t *verified_hash, bool allow_key_revoke);

/* Verify bootloader image (reconfigures cache to map),
   with key digests provided as parameters.)

   Can be used to verify secure boot status before enabling
   secure boot permanently.

   If stage_load parameter is true, bootloader is copied into staging
   buffer in RAM at the same time.

   If result is SB_SUCCESS, the "simple hash" of the bootloader is
   copied into verified_hash.
*/
ets_secure_boot_status_t ets_secure_boot_verify_bootloader_with_keys(uint8_t *verified_hash, const ets_secure_boot_key_digests_t *trusted_keys, bool stage_load);

/* Read key digests from efuse. Any revoked/missing digests will be
   marked as NULL
*/
ETS_STATUS ets_secure_boot_read_key_digests(ets_secure_boot_key_digests_t *trusted_keys);

/* Verify supplied signature against supplied digest, using
   supplied trusted key digests.

   Doesn't reconfigure cache or any other hardware access except for RSA peripheral.

   If result is SB_SUCCESS, the image_digest value is copied into verified_digest.
*/
ets_secure_boot_status_t ets_secure_boot_verify_signature(const ets_secure_boot_signature_t *sig, const uint8_t *image_digest, const ets_secure_boot_key_digests_t *trusted_keys, uint8_t *verified_digest);

/* Revoke a public key digest in efuse.
   @param index Digest to revoke. Must be 0, 1 or 2.
 */
void ets_secure_boot_revoke_public_key_digest(int index);

#define CRC_SIGN_BLOCK_LEN 1196
#define SIG_BLOCK_PADDING 4096
#define ETS_SECURE_BOOT_V2_SIGNATURE_MAGIC 0xE7

/* Secure Boot V2 signature block

   (Up to 3 in a signature sector are appended to the image)
 */
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

_Static_assert(sizeof(ets_secure_boot_sig_block_t) == 1216, "invalid sig block size");

#define SECURE_BOOT_NUM_BLOCKS 3

/* V2 Secure boot signature sector (up to 3 blocks) */
struct ets_secure_boot_signature {
    ets_secure_boot_sig_block_t block[SECURE_BOOT_NUM_BLOCKS];
    uint8_t _padding[4096 - (sizeof(ets_secure_boot_sig_block_t) * SECURE_BOOT_NUM_BLOCKS)];
};

_Static_assert(sizeof(ets_secure_boot_signature_t) == 4096, "invalid sig sector size");

#define MAX_KEY_DIGESTS 3

struct ets_secure_boot_key_digests {
    const void *key_digests[MAX_KEY_DIGESTS];
    bool allow_key_revoke;
};

#ifdef __cplusplus
}
#endif
