/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "hal/ecdsa_ll.h"
#include "hal/ecdsa_hal.h"
#include "esp_crypto_lock.h"
#include "esp_efuse.h"
#include "esp_private/esp_crypto_lock_internal.h"
#include "mbedtls/error.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/asn1write.h"
#include "mbedtls/platform_util.h"
#include "ecdsa/ecdsa_alt.h"

#define ECDSA_KEY_MAGIC             (short) 0xECD5A
#define ECDSA_SHA_LEN               32
#define MAX_ECDSA_COMPONENT_LEN     32

__attribute__((unused)) static const char *TAG = "ecdsa_alt";

static void esp_ecdsa_acquire_hardware(void)
{
    esp_crypto_ecdsa_lock_acquire();

    ECDSA_RCC_ATOMIC() {
        ecdsa_ll_enable_bus_clock(true);
        ecdsa_ll_reset_register();
    }
}

static void esp_ecdsa_release_hardware(void)
{
    ECDSA_RCC_ATOMIC() {
        ecdsa_ll_enable_bus_clock(false);
    }

    esp_crypto_ecdsa_lock_release();
}

static void ecdsa_be_to_le(const uint8_t* be_point, uint8_t *le_point, uint8_t len)
{
    /* When the size is 24 bytes, it should be padded with 0 bytes*/
    memset(le_point, 0x0, 32);

    for(int i = 0; i < len; i++) {
        le_point[i] = be_point[len - i - 1];
    }
}

#ifdef SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
int esp_ecdsa_load_pubkey(mbedtls_ecp_keypair *keypair, int efuse_blk)
{
    int ret = -1;

    if (efuse_blk < EFUSE_BLK_KEY0 || efuse_blk >= EFUSE_BLK_KEY_MAX) {
        ESP_LOGE(TAG, "Invalid efuse block selected");
        return ret;
    }

    ecdsa_curve_t curve;
    esp_efuse_block_t blk;
    uint16_t len;
    uint8_t zeroes[MAX_ECDSA_COMPONENT_LEN] = {0};
    uint8_t qx_le[MAX_ECDSA_COMPONENT_LEN];
    uint8_t qy_le[MAX_ECDSA_COMPONENT_LEN];

    if (keypair->MBEDTLS_PRIVATE(grp).id == MBEDTLS_ECP_DP_SECP192R1) {
        curve = ECDSA_CURVE_SECP192R1;
        len = 24;
    } else if (keypair->MBEDTLS_PRIVATE(grp).id == MBEDTLS_ECP_DP_SECP256R1) {
        curve = ECDSA_CURVE_SECP256R1;
        len = 32;
    } else {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (!esp_efuse_find_purpose(ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY, &blk)) {
        ESP_LOGE(TAG, "No efuse block with purpose ECDSA_KEY found");
        return MBEDTLS_ERR_ECP_INVALID_KEY;
    }

    ecdsa_hal_config_t conf = {
        .mode = ECDSA_MODE_EXPORT_PUBKEY,
        .curve = curve,
        .use_km_key = 0, //TODO: IDF-7992
        .efuse_key_blk = efuse_blk,
    };

    esp_ecdsa_acquire_hardware();

    do {
        ecdsa_hal_export_pubkey(&conf, qx_le, qy_le, len);
    } while (!memcmp(qx_le, zeroes, len) || !memcmp(qy_le, zeroes, len));

    esp_ecdsa_release_hardware();

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary_le(&(keypair->MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(X)), qx_le, len));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary_le(&(keypair->MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Y)), qy_le, len));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&(keypair->MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Z)), 1));

    return 0;

cleanup:
    return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
}
#endif /* SOC_ECDSA_SUPPORT_EXPORT_PUBKEY */

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

int esp_ecdsa_set_pk_context(mbedtls_pk_context *key_ctx, esp_ecdsa_pk_conf_t *conf)
{
    int ret = -1;

    if (!key_ctx) {
        ESP_LOGE(TAG, "mbedtls_pk_context cannot be NULL");
        return ret;
    }

    if (!conf) {
        ESP_LOGE(TAG, "esp_ecdsa_pk_conf_t cannot be NULL");
        return ret;
    }

    if (conf->grp_id != MBEDTLS_ECP_DP_SECP192R1 && conf->grp_id != MBEDTLS_ECP_DP_SECP256R1) {
        ESP_LOGE(TAG, "Invalid EC curve group id mentioned in esp_ecdsa_pk_conf_t");
        return ret;
    }

    if ((ret = esp_ecdsa_privkey_load_pk_context(key_ctx, conf->efuse_block)) != 0) {
        ESP_LOGE(TAG, "Loading private key context failed, esp_ecdsa_privkey_load_pk_context() returned %d", ret);
        return ret;
    }

    mbedtls_ecp_keypair *keypair = mbedtls_pk_ec(*key_ctx);
    if ((ret = mbedtls_ecp_group_load(&(keypair->MBEDTLS_PRIVATE(grp)), conf->grp_id)) != 0) {
        ESP_LOGE(TAG, "Loading ecp group failed, mbedtls_pk_ec() returned %d", ret);
        return ret;
    }

#ifdef SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
    if (conf->load_pubkey) {
        if ((ret = esp_ecdsa_load_pubkey(keypair, conf->efuse_block)) != 0) {
            ESP_LOGE(TAG, "Loading public key context failed, esp_ecdsa_load_pubkey() returned %d", ret);
            return ret;
        }
    }
#endif
    return 0;
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
            .sha_mode = ECDSA_Z_USER_PROVIDED,
            .efuse_key_blk = d->MBEDTLS_PRIVATE(n),
            .use_km_key = 0, //TODO: IDF-7992
        };

        ecdsa_hal_gen_signature(&conf, sha_le, r_le, s_le, len);
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

extern int __real_mbedtls_ecdsa_sign_restartable(mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                                          const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                                          int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
                                          int (*f_rng_blind)(void *, unsigned char *, size_t), void *p_rng_blind,
                                          mbedtls_ecdsa_restart_ctx *rs_ctx);

int __wrap_mbedtls_ecdsa_sign_restartable(mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                                          const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                                          int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
                                          int (*f_rng_blind)(void *, unsigned char *, size_t), void *p_rng_blind,
                                          mbedtls_ecdsa_restart_ctx *rs_ctx);

int __wrap_mbedtls_ecdsa_sign_restartable(mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                                          const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                                          int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
                                          int (*f_rng_blind)(void *, unsigned char *, size_t), void *p_rng_blind,
                                          mbedtls_ecdsa_restart_ctx *rs_ctx)
{
    /*
     * Check `d` whether it contains the hardware key
     */
    if (d->MBEDTLS_PRIVATE(s) == ECDSA_KEY_MAGIC) {
        // Use hardware ECDSA peripheral
        return esp_ecdsa_sign(grp, r, s, d, buf, blen);
    } else {
        return __real_mbedtls_ecdsa_sign_restartable(grp, r, s, d, buf, blen, f_rng, p_rng, f_rng_blind, p_rng_blind, rs_ctx);
    }
}

int __real_mbedtls_ecdsa_write_signature_restartable(mbedtls_ecdsa_context *ctx,
                                              mbedtls_md_type_t md_alg,
                                              const unsigned char *hash, size_t hlen,
                                              unsigned char *sig, size_t sig_size, size_t *slen,
                                              int (*f_rng)(void *, unsigned char *, size_t),
                                              void *p_rng,
                                              mbedtls_ecdsa_restart_ctx *rs_ctx);

int __wrap_mbedtls_ecdsa_write_signature_restartable(mbedtls_ecdsa_context *ctx,
                                              mbedtls_md_type_t md_alg,
                                              const unsigned char *hash, size_t hlen,
                                              unsigned char *sig, size_t sig_size, size_t *slen,
                                              int (*f_rng)(void *, unsigned char *, size_t),
                                              void *p_rng,
                                              mbedtls_ecdsa_restart_ctx *rs_ctx);

/*
 * Convert a signature (given by context) to ASN.1
 */
static int ecdsa_signature_to_asn1(const mbedtls_mpi *r, const mbedtls_mpi *s,
                                   unsigned char *sig, size_t sig_size,
                                   size_t *slen)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    unsigned char buf[MBEDTLS_ECDSA_MAX_LEN] = { 0 };
    // Setting the pointer p to the end of the buffer as the functions used afterwards write in backwards manner in the given buffer.
    unsigned char *p = buf + sizeof(buf);
    size_t len = 0;

    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_mpi(&p, buf, s));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_mpi(&p, buf, r));

    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_len(&p, buf, len));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_tag(&p, buf,
                                                     MBEDTLS_ASN1_CONSTRUCTED |
                                                     MBEDTLS_ASN1_SEQUENCE));

    if (len > sig_size) {
        return MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;
    }

    memcpy(sig, p, len);
    *slen = len;

    return 0;
}

int __wrap_mbedtls_ecdsa_write_signature_restartable(mbedtls_ecdsa_context *ctx,
                                              mbedtls_md_type_t md_alg,
                                              const unsigned char *hash, size_t hlen,
                                              unsigned char *sig, size_t sig_size, size_t *slen,
                                              int (*f_rng)(void *, unsigned char *, size_t),
                                              void *p_rng,
                                              mbedtls_ecdsa_restart_ctx *rs_ctx)
{
    if (ctx->MBEDTLS_PRIVATE(d).MBEDTLS_PRIVATE(s) != ECDSA_KEY_MAGIC) {
        return __real_mbedtls_ecdsa_write_signature_restartable(ctx, md_alg, hash, hlen, sig, sig_size, slen, f_rng, p_rng, rs_ctx);
    }

    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    mbedtls_mpi r, s;

    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    /*
     * Check `d` whether it contains the hardware key
     */
    if (ctx->MBEDTLS_PRIVATE(d).MBEDTLS_PRIVATE(s) == ECDSA_KEY_MAGIC) {
        // Use hardware ECDSA peripheral

        MBEDTLS_MPI_CHK(esp_ecdsa_sign(&ctx->MBEDTLS_PRIVATE(grp), &r, &s, &ctx->MBEDTLS_PRIVATE(d), hash, hlen));
    }

    MBEDTLS_MPI_CHK(ecdsa_signature_to_asn1(&r, &s, sig, sig_size, slen));

cleanup:
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);

    return ret;
}

int __wrap_mbedtls_ecdsa_write_signature(mbedtls_ecdsa_context *ctx,
                                  mbedtls_md_type_t md_alg,
                                  const unsigned char *hash, size_t hlen,
                                  unsigned char *sig, size_t sig_size, size_t *slen,
                                  int (*f_rng)(void *, unsigned char *, size_t),
                                  void *p_rng);

int __wrap_mbedtls_ecdsa_write_signature(mbedtls_ecdsa_context *ctx,
                                  mbedtls_md_type_t md_alg,
                                  const unsigned char *hash, size_t hlen,
                                  unsigned char *sig, size_t sig_size, size_t *slen,
                                  int (*f_rng)(void *, unsigned char *, size_t),
                                  void *p_rng)
{
    return __wrap_mbedtls_ecdsa_write_signature_restartable(
        ctx, md_alg, hash, hlen, sig, sig_size, slen,
        f_rng, p_rng, NULL);
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
