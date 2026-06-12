/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "esp_rom_caps.h"
#if ESP_ROM_ECDSA_VERIFY_PATCH
#include "soc/soc_caps.h"
#include "esp_fault.h"
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
#endif /* ESP_ROM_ECDSA_VERIFY_PATCH */
