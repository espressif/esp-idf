/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "hal/ecdsa_hal.h"
#include "esp_efuse.h"
#include "mbedtls/ecp.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/platform_util.h"
#include "esp_private/periph_ctrl.h"
#include "ecdsa/ecdsa_alt.h"

#define ECDSA_KEY_MAGIC             0xECD5A
#define ECDSA_SHA_LEN               32
#define MAX_ECDSA_COMPONENT_LEN     32

__attribute__((unused)) static const char *TAG = "ecdsa_alt";

static _lock_t s_crypto_ecdsa_lock;

static void esp_ecdsa_acquire_hardware(void)
{
    _lock_acquire(&s_crypto_ecdsa_lock);

    periph_module_enable(PERIPH_ECDSA_MODULE);
}

static void esp_ecdsa_release_hardware(void)
{
    periph_module_disable(PERIPH_ECDSA_MODULE);

    _lock_release(&s_crypto_ecdsa_lock);
}

static void ecdsa_be_to_le(const uint8_t* be_point, uint8_t *le_point, uint8_t len)
{
    /* When the size is 24 bytes, it should be padded with 0 bytes*/
    memset(le_point, 0x0, 32);

    for(int i = 0; i < len; i++) {
        le_point[i] = be_point[len - i - 1];
    }
}

#ifdef CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
int esp_ecdsa_privkey_load_mpi(mbedtls_mpi *key, int efuse_blk)
{
    if (!key) {
        ESP_LOGE(TAG, "Invalid memory");
        return -1;
    }

    if (efuse_blk < EFUSE_BLK_KEY0 || efuse_blk >= EFUSE_BLK_KEY_MAX) {
        ESP_LOGE(TAG, "Invalid efuse block");
        return -1;
    }

    mbedtls_mpi_init(key);

    /* We use the mbedtls_mpi struct to pass our own context to hardware ECDSA peripheral
     * MPI struct expects `s` to be either 1 or -1, by setting it to 0xECD5A, we ensure that it does
     * not collide with a valid MPI. This is done to differentiate between using the private key stored in efuse
     * or using the private key provided by software
     *
     * `n` is used to store the efuse block which should be used as key
     */
    key->MBEDTLS_PRIVATE(s) = ECDSA_KEY_MAGIC;
    key->MBEDTLS_PRIVATE(n) = efuse_blk;
    key->MBEDTLS_PRIVATE(p) = NULL;

    return 0;
}

int esp_ecdsa_privkey_load_pk_context(mbedtls_pk_context *key_ctx, int efuse_blk)
{
    const mbedtls_pk_info_t *pk_info;
    mbedtls_ecp_keypair *keypair;

    if (!key_ctx) {
        ESP_LOGE(TAG, "Invalid memory");
        return -1;
    }

    if (efuse_blk < EFUSE_BLK_KEY0 || efuse_blk >= EFUSE_BLK_KEY_MAX) {
        ESP_LOGE(TAG, "Invalid efuse block");
        return -1;
    }

    mbedtls_pk_init(key_ctx);
    pk_info = mbedtls_pk_info_from_type(MBEDTLS_PK_ECDSA);
    mbedtls_pk_setup(key_ctx, pk_info);
    keypair = mbedtls_pk_ec(*key_ctx);

    return esp_ecdsa_privkey_load_mpi(&(keypair->MBEDTLS_PRIVATE(d)), efuse_blk);
}

static int esp_ecdsa_sign(mbedtls_ecp_group *grp, mbedtls_mpi* r, mbedtls_mpi* s,
                          const mbedtls_mpi *d, const unsigned char* msg, size_t msg_len)
{
    ecdsa_curve_t curve;
    esp_efuse_block_t blk;
    uint16_t len;
    uint8_t zeroes[MAX_ECDSA_COMPONENT_LEN] = {0};
    uint8_t sha_le[ECDSA_SHA_LEN];
    uint8_t r_le[MAX_ECDSA_COMPONENT_LEN];
    uint8_t s_le[MAX_ECDSA_COMPONENT_LEN];

    if (!grp || !r || !s || !d || !msg) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (msg_len != ECDSA_SHA_LEN) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (grp->id == MBEDTLS_ECP_DP_SECP192R1) {
        curve = ECDSA_CURVE_SECP192R1;
        len = 24;
    } else if (grp->id == MBEDTLS_ECP_DP_SECP256R1) {
        curve = ECDSA_CURVE_SECP256R1;
        len = 32;
    } else {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (!esp_efuse_find_purpose(ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY, &blk)) {
        ESP_LOGE(TAG, "No efuse block with purpose ECDSA_KEY found");
        return MBEDTLS_ERR_ECP_INVALID_KEY;
    }

    ecdsa_be_to_le(msg, sha_le, len);

    esp_ecdsa_acquire_hardware();

    do {
        ecdsa_hal_config_t conf = {
            .mode = ECDSA_MODE_SIGN_GEN,
            .curve = curve,
            .k_mode = ECDSA_K_USE_TRNG,
            .sha_mode = ECDSA_Z_USER_PROVIDED,
            .efuse_key_blk = d->MBEDTLS_PRIVATE(n),
        };

        ecdsa_hal_gen_signature(&conf, NULL, sha_le, r_le, s_le, len);
    } while (!memcmp(r_le, zeroes, len) || !memcmp(s_le, zeroes, len));

    esp_ecdsa_release_hardware();

    mbedtls_mpi_read_binary_le(r, r_le, len);
    mbedtls_mpi_read_binary_le(s, s_le, len);

    return 0;
}

/*
 * Compute ECDSA signature of a hashed message;
 */
extern int __real_mbedtls_ecdsa_sign(mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                       const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                       int (*f_rng)(void *, unsigned char *, size_t), void *p_rng);

int __wrap_mbedtls_ecdsa_sign(mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                       const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                       int (*f_rng)(void *, unsigned char *, size_t), void *p_rng);

int __wrap_mbedtls_ecdsa_sign(mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                       const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                       int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    /*
     * Check `d` whether it contains the hardware key
     */
    if (d->MBEDTLS_PRIVATE(s) == ECDSA_KEY_MAGIC) {
        // Use hardware ECDSA peripheral
        return esp_ecdsa_sign(grp, r, s, d, buf, blen);
    } else {
        return __real_mbedtls_ecdsa_sign(grp, r, s, d, buf, blen, f_rng, p_rng);
    }
}
#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN */

#ifdef CONFIG_MBEDTLS_HARDWARE_ECDSA_VERIFY
static int esp_ecdsa_verify(mbedtls_ecp_group *grp,
                            const unsigned char *buf, size_t blen,
                            const mbedtls_ecp_point *Q,
                            const mbedtls_mpi *r,
                            const mbedtls_mpi *s)
{
    ecdsa_curve_t curve;
    uint16_t len;
    uint8_t r_le[MAX_ECDSA_COMPONENT_LEN];
    uint8_t s_le[MAX_ECDSA_COMPONENT_LEN];
    uint8_t qx_le[MAX_ECDSA_COMPONENT_LEN];
    uint8_t qy_le[MAX_ECDSA_COMPONENT_LEN];
    uint8_t sha_le[ECDSA_SHA_LEN];

    if (!grp || !buf || !Q || !r || !s) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (blen != ECDSA_SHA_LEN) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (grp->id == MBEDTLS_ECP_DP_SECP192R1) {
        curve = ECDSA_CURVE_SECP192R1;
        len = 24;
    } else if (grp->id == MBEDTLS_ECP_DP_SECP256R1) {
        curve = ECDSA_CURVE_SECP256R1;
        len = 32;
    } else {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (mbedtls_mpi_cmp_int(r, 1) < 0 || mbedtls_mpi_cmp_mpi(r, &grp->N) >= 0 ||
       mbedtls_mpi_cmp_int(s, 1) < 0 || mbedtls_mpi_cmp_mpi(s, &grp->N) >= 0 )
    {
        return MBEDTLS_ERR_ECP_VERIFY_FAILED;
    }

    ecdsa_be_to_le(buf, sha_le, len);

    mbedtls_mpi_write_binary_le(&Q->MBEDTLS_PRIVATE(X), qx_le, len);
    mbedtls_mpi_write_binary_le(&Q->MBEDTLS_PRIVATE(Y), qy_le, len);
    mbedtls_mpi_write_binary_le(r, r_le, len);
    mbedtls_mpi_write_binary_le(s, s_le, len);

    esp_ecdsa_acquire_hardware();

    ecdsa_hal_config_t conf = {
        .mode = ECDSA_MODE_SIGN_VERIFY,
        .curve = curve,
        .k_mode = ECDSA_K_USE_TRNG,
        .sha_mode = ECDSA_Z_USER_PROVIDED,
    };

    int ret = ecdsa_hal_verify_signature(&conf, sha_le, r_le, s_le, qx_le, qy_le, len);

    esp_ecdsa_release_hardware();

    if (ret != 0) {
        return MBEDTLS_ERR_ECP_VERIFY_FAILED;
    }

    return ret;
}

/*
 * Verify ECDSA signature of hashed message
 */
extern int __real_mbedtls_ecdsa_verify(mbedtls_ecp_group *grp,
                         const unsigned char *buf, size_t blen,
                         const mbedtls_ecp_point *Q,
                         const mbedtls_mpi *r,
                         const mbedtls_mpi *s);

int __wrap_mbedtls_ecdsa_verify(mbedtls_ecp_group *grp,
                         const unsigned char *buf, size_t blen,
                         const mbedtls_ecp_point *Q,
                         const mbedtls_mpi *r,
                         const mbedtls_mpi *s);

int __wrap_mbedtls_ecdsa_verify(mbedtls_ecp_group *grp,
                         const unsigned char *buf, size_t blen,
                         const mbedtls_ecp_point *Q,
                         const mbedtls_mpi *r,
                         const mbedtls_mpi *s)
{
    if (grp->id == MBEDTLS_ECP_DP_SECP192R1 || grp->id == MBEDTLS_ECP_DP_SECP256R1) {
        return esp_ecdsa_verify(grp, buf, blen, Q, r, s);
    } else {
        return __real_mbedtls_ecdsa_verify(grp, buf, blen, Q, r, s);
    }
}
#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_VERIFY */
