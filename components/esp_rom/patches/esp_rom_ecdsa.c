/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "sdkconfig.h"
#include "esp_rom_caps.h"
#if ESP_ROM_ECDSA_VERIFY_PATCH
#include "soc/soc_caps.h"
#include "esp_fault.h"
#include "hal/ecc_ll.h"
#include "rom/ecdsa.h"

#define VALID_MAGIC_OK   0x6A6A6A6AU
#define VALID_MAGIC_FAIL 0x95959595U

static const uint32_t ecdsa_n_p192[6] = {
    0xb4d22831U, 0x146bc9b1U, 0x99def836U, 0xffffffffU, 0xffffffffU, 0xffffffffU,
};

static const uint32_t ecdsa_n_p256[8] = {
    0xfc632551U, 0xf3b9cac2U, 0xa7179e84U, 0xbce6faadU, 0xffffffffU, 0xffffffffU, 0x00000000U, 0xffffffffU,
};

#if SOC_ECDSA_SUPPORT_CURVE_P384
static const uint32_t ecdsa_n_p384[12] = {
    0xccc52973U, 0xecec196aU, 0x48b0a77aU, 0x581a0db2U, 0xf4372ddfU, 0xc7634d81U,
    0xffffffffU, 0xffffffffU, 0xffffffffU, 0xffffffffU, 0xffffffffU, 0xffffffffU,
};
#endif

static uint32_t ecdsa_mpi_isZero(const uint32_t *mpi, int num_words)
{
    uint32_t bits = 0;
    for (int i = 0; i < num_words; ++i) {
        bits |= mpi[i];
    }
    return (bits == 0);
}

static int ecdsa_mpi_cmp_unsafe(const uint32_t *left, const uint32_t *right, int num_words)
{
    for (int i = num_words - 1; i >= 0; --i) {
        if (left[i] > right[i]) {
            return 1;
        } else if (left[i] < right[i]) {
            return -1;
        }
    }
    return 0;
}

static bool ecdsa_scalars_in_range(const uint32_t *r, const uint32_t *s, const uint32_t *n, int num_words, uint32_t *result)
{
    volatile uint32_t verdict = VALID_MAGIC_FAIL;
    if (ecdsa_mpi_isZero(r, num_words) == 0 && ecdsa_mpi_cmp_unsafe(n, r, num_words) == 1 && ecdsa_mpi_isZero(s, num_words) == 0
        && ecdsa_mpi_cmp_unsafe(n, s, num_words) == 1) {
        verdict = VALID_MAGIC_OK;
    }
    if (verdict != VALID_MAGIC_OK) {
        return false;
    }
    ESP_FAULT_ASSERT(verdict == VALID_MAGIC_OK);

    *result = VALID_MAGIC_OK;
    return true;
}

// TODO: IDF-15721
/*
 * Runtime gate that decides whether the ROM ECDSA verification routines
 * (ets_ecdsa_verify / ets_secure_boot_verify_signature) need the software patch
 * in this file, or whether the ROM implementation is safe to call directly.
 *
 * When a future revision of one of these chips ships a ROM with these ECDSA
 * verification issues fixed, add a ROM-version check here (e.g. compare the
 * _rom_eco_version symbol against the first fixed ROM ECO version for that
 * target) and return false for the fixed ROMs, so they skip the patch and jump
 * straight to the _rom_ routine.
 */

extern int _rom_ets_ecdsa_verify(const uint8_t *key, const uint8_t *sig,
                                 ECDSA_CURVE curve_id, const uint8_t *image_digest,
                                 uint8_t *verified_digest);

int ets_ecdsa_verify(const uint8_t *key, const uint8_t *sig,
                     ECDSA_CURVE curve_id, const uint8_t *image_digest,
                     uint8_t *verified_digest)
{
    int words;
    int bytes;
    const uint32_t *n;

    if (curve_id == ECDSA_CURVE_P256) {
        words = 8;
        bytes = 32;
        n = ecdsa_n_p256;
    }
#if SOC_ECDSA_SUPPORT_CURVE_P384
    else if (curve_id == ECDSA_CURVE_P384) {
        words = 12;
        bytes = 48;
        n = ecdsa_n_p384;
    }
#endif
    else {
        // curve_id == ECDSA_CURVE_P192
        words = 6;
        bytes = 24;
        n = ecdsa_n_p192;
    }

    uint32_t r[12] = { 0 };
    uint32_t s[12] = { 0 };
    memcpy(r, &sig[0], bytes);
    memcpy(s, &sig[bytes], bytes);

    uint32_t ret_status = VALID_MAGIC_FAIL;
    bool ok = ecdsa_scalars_in_range(r, s, n, words, &ret_status);
    if (!ok || ret_status != VALID_MAGIC_OK) {
        return 0;
    }

    ESP_FAULT_ASSERT(ok && ret_status == VALID_MAGIC_OK);

    ecc_ll_power_up();
    ESP_FAULT_ASSERT(ecc_ll_mem_force_pd_is_clear());

    int ret = _rom_ets_ecdsa_verify(key, sig, curve_id, image_digest, verified_digest);

    if (ret == 1) {
        ESP_FAULT_ASSERT(ret_status == VALID_MAGIC_OK);
        int sig_diff = (memcmp(r, &sig[0], bytes) | memcmp(s, &sig[bytes], bytes));
        ESP_FAULT_ASSERT(sig_diff == 0);
        ESP_FAULT_ASSERT(ret == 1);
        return ret;
    }

    return 0;
}

#if CONFIG_SECURE_BOOT_V2_ENABLED || CONFIG_SECURE_SIGNED_APPS_NO_SECURE_BOOT
#include "rom/secure_boot.h"

#if CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME
static bool esp_rom_ecdsa_scalars_in_range(const uint8_t *r_le, const uint8_t *s_le, size_t component_len)
{
    const uint32_t *n;
    int words;
    switch (component_len) {
    case 24: n = ecdsa_n_p192; words = 6; break;
    case 32: n = ecdsa_n_p256; words = 8; break;
#if SOC_ECDSA_SUPPORT_CURVE_P384
    case 48: n = ecdsa_n_p384; words = 12; break;
#endif
    default: return false;
    }

    uint32_t r[12] = { 0 };
    uint32_t s[12] = { 0 };
    memcpy(r, r_le, component_len);
    memcpy(s, s_le, component_len);

    uint32_t result = VALID_MAGIC_FAIL;
    bool ok = ecdsa_scalars_in_range(r, s, n, words, &result);
    if (!ok || result != VALID_MAGIC_OK) {
        return false;
    }
    ESP_FAULT_ASSERT(ok && result == VALID_MAGIC_OK);
    return true;
}

static bool esp_rom_ecdsa_sig_block_in_range(const ets_secure_boot_sig_block_t *block)
{
    if (block->magic_byte != ETS_SECURE_BOOT_V2_SIGNATURE_MAGIC) {
        return true;
    }
    size_t component_len;
    switch (block->ecdsa.key.curve_id) {
    case ECDSA_CURVE_P256: component_len = 32; break;
#if SOC_ECDSA_SUPPORT_CURVE_P384
    case ECDSA_CURVE_P384: component_len = 48; break;
#endif
    default: return false;
    }
    return esp_rom_ecdsa_scalars_in_range(&block->ecdsa.signature[0],
                                          &block->ecdsa.signature[component_len],
                                          component_len);
}
#endif /* CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME */

extern ets_secure_boot_status_t _rom_ets_secure_boot_verify_signature(const ets_secure_boot_signature_t *sig,
                                                                      const uint8_t *image_digest,
                                                                      const ets_secure_boot_key_digests_t *trusted_keys,
                                                                      uint8_t *verified_digest);

ets_secure_boot_status_t ets_secure_boot_verify_signature(const ets_secure_boot_signature_t *sig,
                                                          const uint8_t *image_digest,
                                                          const ets_secure_boot_key_digests_t *trusted_keys,
                                                          uint8_t *verified_digest)
{
#if CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME
    volatile ets_secure_boot_status_t range_status = SB_FAILED;
    unsigned blocks_in_range = 0;
    for (unsigned i = 0; i < SECURE_BOOT_NUM_BLOCKS; i++) {
        if (esp_rom_ecdsa_sig_block_in_range(&sig->block[i])) {
            blocks_in_range++;
        }
    }
    if (blocks_in_range == SECURE_BOOT_NUM_BLOCKS) {
        range_status = SB_SUCCESS;
    }
    if (range_status != SB_SUCCESS) {
        return SB_FAILED;
    }
    ESP_FAULT_ASSERT(range_status == SB_SUCCESS);
    ESP_FAULT_ASSERT(blocks_in_range == SECURE_BOOT_NUM_BLOCKS);

    ecc_ll_power_up();
    ESP_FAULT_ASSERT(ecc_ll_mem_force_pd_is_clear());
#endif /* CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME */
    return _rom_ets_secure_boot_verify_signature(sig, image_digest, trusted_keys, verified_digest);
}
#endif /* CONFIG_SECURE_BOOT_V2_ENABLED || CONFIG_SECURE_SIGNED_APPS_NO_SECURE_BOOT */
#endif /* ESP_ROM_ECDSA_VERIFY_PATCH */
