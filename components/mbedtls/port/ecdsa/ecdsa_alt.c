/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "hal/ecdsa_hal.h"
#include "esp_crypto_lock.h"
#include "esp_efuse.h"
#include "mbedtls/ecp.h"
#include "mbedtls/error.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/asn1.h"
#include "mbedtls/asn1write.h"
#include "mbedtls/platform_util.h"
#include "esp_private/periph_ctrl.h"
#include "ecdsa/ecdsa_alt.h"
#include "hal/ecc_ll.h"

#define ECDSA_KEY_MAGIC             (short) 0xECD5A
#define ECDSA_SHA_LEN               32
#define MAX_ECDSA_COMPONENT_LEN     32

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_CONSTANT_TIME_CM
#include "esp_timer.h"
#include "soc/chip_revision.h"
#include "hal/efuse_hal.h"

#if CONFIG_ESP_CRYPTO_DPA_PROTECTION_LEVEL_HIGH
/*
 * This is the maximum time (in us) required for performing 1 ECDSA signature
 * in this configuration along some additional margin considerations
 */
#define ECDSA_MAX_SIG_TIME 24000
#else /* CONFIG_ESP_CRYPTO_DPA_PROTECTION_LEVEL_HIGH */
#define ECDSA_MAX_SIG_TIME  17500
#endif /* !CONFIG_ESP_CRYPTO_DPA_PROTECTION_LEVEL_HIGH */

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_MASKING_CM
#define DUMMY_OP_COUNT ECDSA_SIGN_MAX_DUMMY_OP_COUNT
#else /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_MASKING_CM */
#define DUMMY_OP_COUNT 0
#endif /* !CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_MASKING_CM */
#define ECDSA_CM_FIXED_SIG_TIME ECDSA_MAX_SIG_TIME * (DUMMY_OP_COUNT + 1)

#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_CONSTANT_TIME_CM */

__attribute__((unused)) static const char *TAG = "ecdsa_alt";

static void esp_ecdsa_acquire_hardware(void)
{
    esp_crypto_ecdsa_lock_acquire();

    periph_module_enable(PERIPH_ECDSA_MODULE);
    ecc_ll_power_up();
}

static void esp_ecdsa_release_hardware(void)
{
    periph_module_disable(PERIPH_ECDSA_MODULE);
    ecc_ll_power_down();

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

    bool process_again = false;

    do {
        ecdsa_hal_config_t conf = {
            .mode = ECDSA_MODE_SIGN_GEN,
            .curve = curve,
            .sha_mode = ECDSA_Z_USER_PROVIDED,
            .efuse_key_blk = d->MBEDTLS_PRIVATE(n),
        };

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_CONSTANT_TIME_CM
        uint64_t sig_time = esp_timer_get_time();
#endif
        ecdsa_hal_gen_signature(&conf, sha_le, r_le, s_le, len);
#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_CONSTANT_TIME_CM
        if (!ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 102)) {
            sig_time = esp_timer_get_time() - sig_time;
            if (sig_time < ECDSA_CM_FIXED_SIG_TIME) {
                esp_rom_delay_us(ECDSA_CM_FIXED_SIG_TIME - sig_time);
            }
        }
#endif
        process_again = !ecdsa_hal_get_operation_result()
                        || !memcmp(r_le, zeroes, len)
                        || !memcmp(s_le, zeroes, len);

    } while (process_again);

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
extern int __real_mbedtls_ecdsa_verify_restartable(mbedtls_ecp_group *grp,
                         const unsigned char *buf, size_t blen,
                         const mbedtls_ecp_point *Q,
                         const mbedtls_mpi *r,
                         const mbedtls_mpi *s,
                         mbedtls_ecdsa_restart_ctx *rs_ctx);

int __wrap_mbedtls_ecdsa_verify_restartable(mbedtls_ecp_group *grp,
                         const unsigned char *buf, size_t blen,
                         const mbedtls_ecp_point *Q,
                         const mbedtls_mpi *r,
                         const mbedtls_mpi *s,
                         mbedtls_ecdsa_restart_ctx *rs_ctx);

int __wrap_mbedtls_ecdsa_verify_restartable(mbedtls_ecp_group *grp,
                         const unsigned char *buf, size_t blen,
                         const mbedtls_ecp_point *Q,
                         const mbedtls_mpi *r,
                         const mbedtls_mpi *s,
                         mbedtls_ecdsa_restart_ctx *rs_ctx)
{
    if ((grp->id == MBEDTLS_ECP_DP_SECP192R1 || grp->id == MBEDTLS_ECP_DP_SECP256R1) && blen == ECDSA_SHA_LEN) {
        return esp_ecdsa_verify(grp, buf, blen, Q, r, s);
    } else {
        return __real_mbedtls_ecdsa_verify_restartable(grp, buf, blen, Q, r, s, rs_ctx);
    }
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
    return __wrap_mbedtls_ecdsa_verify_restartable(grp, buf, blen, Q, r, s, NULL);
}


int __real_mbedtls_ecdsa_read_signature_restartable(mbedtls_ecdsa_context *ctx,
                                             const unsigned char *hash, size_t hlen,
                                             const unsigned char *sig, size_t slen,
                                             mbedtls_ecdsa_restart_ctx *rs_ctx);

int __wrap_mbedtls_ecdsa_read_signature_restartable(mbedtls_ecdsa_context *ctx,
                                             const unsigned char *hash, size_t hlen,
                                             const unsigned char *sig, size_t slen,
                                             mbedtls_ecdsa_restart_ctx *rs_ctx);

int __wrap_mbedtls_ecdsa_read_signature_restartable(mbedtls_ecdsa_context *ctx,
                                             const unsigned char *hash, size_t hlen,
                                             const unsigned char *sig, size_t slen,
                                             mbedtls_ecdsa_restart_ctx *rs_ctx)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    unsigned char *p = (unsigned char *) sig;
    const unsigned char *end = sig + slen;
    size_t len;
    mbedtls_mpi r, s;
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    if ((ret = mbedtls_asn1_get_tag(&p, end, &len,
                                    MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE)) != 0) {
        ret += MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        goto cleanup;
    }

    if (p + len != end) {
        ret = MBEDTLS_ERROR_ADD(MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                                MBEDTLS_ERR_ASN1_LENGTH_MISMATCH);
        goto cleanup;
    }

    if ((ret = mbedtls_asn1_get_mpi(&p, end, &r)) != 0 ||
        (ret = mbedtls_asn1_get_mpi(&p, end, &s)) != 0) {
        ret += MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        goto cleanup;
    }

    if ((ret = __wrap_mbedtls_ecdsa_verify_restartable(&ctx->MBEDTLS_PRIVATE(grp), hash, hlen,
                                                    &ctx->MBEDTLS_PRIVATE(Q), &r, &s, NULL)) != 0) {
        goto cleanup;
    }

    /* At this point we know that the buffer starts with a valid signature.
     * Return 0 if the buffer just contains the signature, and a specific
     * error code if the valid signature is followed by more data. */
    if (p != end) {
        ret = MBEDTLS_ERR_ECP_SIG_LEN_MISMATCH;
    }

cleanup:
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);

    return ret;
}


int __real_mbedtls_ecdsa_read_signature(mbedtls_ecdsa_context *ctx,
                                    const unsigned char *hash, size_t hlen,
                                    const unsigned char *sig, size_t slen);

int __wrap_mbedtls_ecdsa_read_signature(mbedtls_ecdsa_context *ctx,
                                    const unsigned char *hash, size_t hlen,
                                    const unsigned char *sig, size_t slen);

int __wrap_mbedtls_ecdsa_read_signature(mbedtls_ecdsa_context *ctx,
                                    const unsigned char *hash, size_t hlen,
                                    const unsigned char *sig, size_t slen)
{
    return __wrap_mbedtls_ecdsa_read_signature_restartable(
            ctx, hash, hlen, sig, slen, NULL);
}
#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_VERIFY */
