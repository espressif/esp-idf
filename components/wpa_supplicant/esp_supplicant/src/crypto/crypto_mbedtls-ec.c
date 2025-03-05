/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#ifdef ESP_PLATFORM
#include "esp_system.h"
#include "mbedtls/bignum.h"
#include "mbedtls/esp_mbedtls_random.h"
#endif

#include "utils/includes.h"
#include "utils/common.h"
#include "crypto.h"
#include "sha256.h"
#include "random.h"

#include "mbedtls/ecp.h"

#include "mbedtls/pk.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/sha256.h"
#include "mbedtls/asn1write.h"
#include "mbedtls/error.h"
// #include "mbedtls/crypto_oid.h"

#include <mbedtls/psa_util.h>
#include "psa/crypto.h"

#include "esp_heap_caps.h"

#include <mbedtls/psa_util.h>
#include "psa/crypto.h"

#include "esp_heap_caps.h"

#define ECP_PRV_DER_MAX_BYTES   ( 29 + 3 * MBEDTLS_ECP_MAX_BYTES )
#define ECP_PUB_DER_MAX_BYTES   ( 30 + 2 * MBEDTLS_ECP_MAX_BYTES )

#ifdef CONFIG_MBEDTLS_ECDH_LEGACY_CONTEXT
#define ACCESS_ECDH(S, var) S->MBEDTLS_PRIVATE(var)
#else
#define ACCESS_ECDH(S, var) S->MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(var)
#endif

#ifdef CONFIG_ECC

// NOTE: Used with mpi, no PSA equivalent
struct crypto_ec *crypto_ec_init(int group)
{
    mbedtls_ecp_group *e;

    mbedtls_ecp_group_id  grp_id;

    /* IANA registry to mbedtls internal mapping*/
    switch (group) {
    case IANA_SECP256R1:
        /* For now just support NIST-P256.
         * This is of type "short Weierstrass".
         */
        grp_id = MBEDTLS_ECP_DP_SECP256R1;
        break;
    default:
        return NULL;

    }
    e = os_zalloc(sizeof(*e));
    if (!e) {
        return NULL;
    }

    mbedtls_ecp_group_init(e);

    if (mbedtls_ecp_group_load(e, grp_id)) {
        mbedtls_ecp_group_free(e);
        os_free(e);
        e = NULL;
    }

    return (struct crypto_ec *)e;
}

void crypto_ec_deinit(struct crypto_ec *e)
{
    if (!e) {
        return;
    }

    mbedtls_ecp_group_free((mbedtls_ecp_group *)e);
    os_free(e);
}

// NOTE: Used with mpi, no PSA equivalent
struct crypto_ec_point *crypto_ec_point_init(struct crypto_ec *e)
{
    mbedtls_ecp_point *pt;
    if (!e) {
        return NULL;
    }

    pt = os_zalloc(sizeof(mbedtls_ecp_point));

    if (!pt) {
        return NULL;
    }

    mbedtls_ecp_point_init(pt);

    return (struct crypto_ec_point *) pt;
}

size_t crypto_ec_prime_len(struct crypto_ec *e)
{
    return mbedtls_mpi_size(&((mbedtls_ecp_group *)e)->P);
}

size_t crypto_ec_order_len(struct crypto_ec *e)
{
    return mbedtls_mpi_size(&((mbedtls_ecp_group *)e)->N);
}

size_t crypto_ec_prime_len_bits(struct crypto_ec *e)
{
    return mbedtls_mpi_bitlen(&((mbedtls_ecp_group *)e)->P);
}

struct crypto_ec_group *crypto_ec_get_group_byname(const char *name)
{
    mbedtls_ecp_group *e;
    const mbedtls_ecp_curve_info *curve = mbedtls_ecp_curve_info_from_name(name);

    e = os_zalloc(sizeof(*e));
    if (!e) {
        return NULL;
    }

    mbedtls_ecp_group_init(e);

    if (mbedtls_ecp_group_load(e, curve->grp_id)) {
        mbedtls_ecp_group_free(e);
        os_free(e);
        e = NULL;
    }

    return (struct crypto_ec_group *)e;
}

const struct crypto_bignum *crypto_ec_get_prime(struct crypto_ec *e)
{
    return (const struct crypto_bignum *) & ((mbedtls_ecp_group *)e)->P;
}

const struct crypto_bignum *crypto_ec_get_order(struct crypto_ec *e)
{
    return (const struct crypto_bignum *) & ((mbedtls_ecp_group *)e)->N;
}

struct crypto_bignum * crypto_ec_get_a(struct crypto_ec *e)
{
    int ret = -1;
    struct crypto_bignum *a;
    mbedtls_mpi *m_a;
    mbedtls_ecp_group *grp = (mbedtls_ecp_group *)e;
    if (mbedtls_ecp_get_type(grp) != MBEDTLS_ECP_TYPE_SHORT_WEIERSTRASS) {
        return NULL;
    }
    a = crypto_bignum_init();
    if (!a) {
        return NULL;
    }
    m_a = (mbedtls_mpi *)a;
    /* Handle Mbed TLS quirk.
     *
     * Mbed TLS default ECP implementation is using grp->A = NULL to represent A = -3 for
     * Short Weierstrass curves(e.g. P-256) thus accessing A needs some tweaking.
     *
     * See mbedtls/ecp.h for details. */
#ifdef MBEDTLS_ECP_SHORT_WEIERSTRASS_ENABLED
    if (mbedtls_ecp_group_a_is_minus_3(grp)) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(m_a, &grp->P, 3));
    } else {
        MBEDTLS_MPI_CHK(mbedtls_mpi_copy(m_a, &grp->A));
    }
#else
    goto cleanup;
#endif
    return a;

cleanup:
    crypto_bignum_deinit(a, 0);
    return NULL;
}

const struct crypto_bignum * crypto_ec_get_b(struct crypto_ec *e)
{
    return (const struct crypto_bignum *) & ((mbedtls_ecp_group *)e)->B;
}

void crypto_ec_point_deinit(struct crypto_ec_point *p, int clear)
{
    mbedtls_ecp_point_free((mbedtls_ecp_point *) p);
    os_free(p);
}

int crypto_ec_point_to_bin(struct crypto_ec *e,
                           const struct crypto_ec_point *point, u8 *x, u8 *y)
{
    int len = mbedtls_mpi_size(&((mbedtls_ecp_group *)e)->P);

    if (x) {
        if (crypto_bignum_to_bin((struct crypto_bignum *) & ((mbedtls_ecp_point *) point)->MBEDTLS_PRIVATE(X),
                                 x, len, len) < 0) {
            return -1;
        }

    }

    if (y) {
        if (crypto_bignum_to_bin((struct crypto_bignum *) & ((mbedtls_ecp_point *) point)->MBEDTLS_PRIVATE(Y),
                                 y, len, len) < 0) {
            return -1;
        }
    }

    return 0;
}

int crypto_ec_get_affine_coordinates(struct crypto_ec *e, struct crypto_ec_point *pt,
                                     struct crypto_bignum *x, struct crypto_bignum *y)
{
    int ret = -1;
    mbedtls_ecp_point *point = (mbedtls_ecp_point *)pt;

    if (!mbedtls_ecp_is_zero(point)  && (mbedtls_mpi_cmp_int(&point->MBEDTLS_PRIVATE(Z), 1) == 0)) {
        // Affine coordinates mean that z should be 1,
        wpa_printf(MSG_ERROR, "Z coordinate is neither 0 or 1");
        return -1;
    }

    if (x) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_copy((mbedtls_mpi*) x, &((mbedtls_ecp_point*)point)->MBEDTLS_PRIVATE(X)));
    }
    if (y) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_copy((mbedtls_mpi*) y, &((mbedtls_ecp_point*)point)->MBEDTLS_PRIVATE(Y)));
    }
    return 0;
cleanup:
    return ret;
}

struct crypto_ec_point *crypto_ec_point_from_bin(struct crypto_ec *e,
                                                 const u8 *val)
{
    mbedtls_ecp_point *pt;
    int len, ret;

    if (!e) {
        return NULL;
    }

    len = mbedtls_mpi_size(&((mbedtls_ecp_group *)e)->P);

    pt = os_zalloc(sizeof(mbedtls_ecp_point));
    if (!pt) {
        return NULL;
    }
    mbedtls_ecp_point_init(pt);

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&pt->MBEDTLS_PRIVATE(X), val, len));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&pt->MBEDTLS_PRIVATE(Y), val + len, len));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset((&pt->MBEDTLS_PRIVATE(Z)), 1));

    return (struct crypto_ec_point *) pt;

cleanup:
    mbedtls_ecp_point_free(pt);
    os_free(pt);
    return NULL;
}

int crypto_ec_point_add(struct crypto_ec *e, const struct crypto_ec_point *a,
                        const struct crypto_ec_point *b,
                        struct crypto_ec_point *c)
{
    int ret;
    mbedtls_mpi one;

    mbedtls_mpi_init(&one);

    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&one, 1));
    MBEDTLS_MPI_CHK(mbedtls_ecp_muladd((mbedtls_ecp_group *)e, (mbedtls_ecp_point *)c, &one, (const mbedtls_ecp_point *)a, &one, (const mbedtls_ecp_point *)b));

cleanup:
    mbedtls_mpi_free(&one);
    return ret ? -1 : 0;
}

int crypto_ec_point_mul(struct crypto_ec *e, const struct crypto_ec_point *p,
                        const struct crypto_bignum *b,
                        struct crypto_ec_point *res)
{
    int ret;
    MBEDTLS_MPI_CHK(mbedtls_ecp_mul((mbedtls_ecp_group *)e,
                                    (mbedtls_ecp_point *) res,
                                    (const mbedtls_mpi *)b,
                                    (const mbedtls_ecp_point *)p,
                                    mbedtls_psa_get_random,
                                    MBEDTLS_PSA_RANDOM_STATE));
cleanup:
    return ret ? -1 : 0;
}

/*  Currently mbedtls does not have any function for inverse
 *  This function calculates inverse of a point.
 *  Set R = -P
 */
static int ecp_opp(const mbedtls_ecp_group *grp, mbedtls_ecp_point *R, const mbedtls_ecp_point *P)
{
    int ret = 0;

    /* Copy */
    if (R != P) {
        MBEDTLS_MPI_CHK(mbedtls_ecp_copy(R, P));
    }

    /* In-place opposite */
    if (mbedtls_mpi_cmp_int(&R->MBEDTLS_PRIVATE(Y), 0) != 0) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(&R->MBEDTLS_PRIVATE(Y), &grp->P, &R->MBEDTLS_PRIVATE(Y)));
    }

cleanup:
    return (ret);
}

int crypto_ec_point_invert(struct crypto_ec *e, struct crypto_ec_point *p)
{
    return ecp_opp((mbedtls_ecp_group *)e, (mbedtls_ecp_point *) p, (mbedtls_ecp_point *) p) ? -1 : 0;
}

int crypto_ec_point_solve_y_coord(struct crypto_ec *e,
                                  struct crypto_ec_point *p,
                                  const struct crypto_bignum *x, int y_bit)
{
    mbedtls_mpi temp;
    mbedtls_mpi *y_sqr, *y;
    mbedtls_mpi_init(&temp);
    int ret = 0;

    y = &((mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(Y);

    /* Faster way to find sqrt
     * Works only with curves having prime p
     * such that p ≡ 3 (mod 4)
     *  y_ = (y2 ^ ((p+1)/4)) mod p
     *
     *  if LSB of both x and y are same: y = y_
     *   else y = p - y_
     * y_bit is LSB of x
     */
    y_bit = (y_bit != 0);

    y_sqr = (mbedtls_mpi *) crypto_ec_point_compute_y_sqr(e, x);

    if (y_sqr) {

        MBEDTLS_MPI_CHK(mbedtls_mpi_add_int(&temp, &((mbedtls_ecp_group *)e)->P, 1));
        MBEDTLS_MPI_CHK(mbedtls_mpi_div_int(&temp, NULL, &temp, 4));
        MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(y, y_sqr, &temp, &((mbedtls_ecp_group *)e)->P, NULL));

        if (y_bit != mbedtls_mpi_get_bit(y, 0)) {
            MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(y, &((mbedtls_ecp_group *)e)->P, y));
        }

        MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&((mbedtls_ecp_point*)p)->MBEDTLS_PRIVATE(X), (const mbedtls_mpi*) x));
        MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&((mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(Z), 1));
    } else {
        ret = 1;
    }
cleanup:
    mbedtls_mpi_free(&temp);
    mbedtls_mpi_free(y_sqr);
    os_free(y_sqr);
    return ret ? -1 : 0;
}

int crypto_get_order(struct crypto_ec_group *group, struct crypto_bignum *x)
{
    return mbedtls_mpi_copy((mbedtls_mpi *) x, &((mbedtls_ecp_group *)group)->N);
}

struct crypto_bignum *crypto_ec_point_compute_y_sqr(struct crypto_ec *e,
                                                    const struct crypto_bignum *x)
{
    mbedtls_mpi temp, temp2, num;
    int ret = 0;

    mbedtls_mpi *y_sqr = os_zalloc(sizeof(mbedtls_mpi));
    if (!y_sqr) {
        return NULL;
    }

    mbedtls_mpi_init(&temp);
    mbedtls_mpi_init(&temp2);
    mbedtls_mpi_init(&num);
    mbedtls_mpi_init(y_sqr);

    /* y^2 = x^3 + ax + b  mod  P */
    /* X*X*X is faster on esp32 whereas X^3 is faster on other chips */
#if CONFIG_IDF_TARGET_ESP32
    /* Calculate x*x*x  mod P*/
    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&temp, (const mbedtls_mpi *) x, (const mbedtls_mpi *) x));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&temp, &temp, (const mbedtls_mpi *) x));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&temp, &temp, &((mbedtls_ecp_group *)e)->P));
#else
    /* Calculate x^3  mod P*/
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&num, 3));
    MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&temp, (const mbedtls_mpi *) x, &num, &((mbedtls_ecp_group *)e)->P, NULL));
#endif

    /* Calculate ax  mod P*/
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&num, -3));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&temp2, (const mbedtls_mpi *) x, &num));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&temp2, &temp2, &((mbedtls_ecp_group *)e)->P));

    /* Calculate ax + b  mod P. Note that b is already < P*/
    MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&temp2, &temp2, &((mbedtls_ecp_group *)e)->B));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&temp2, &temp2, &((mbedtls_ecp_group *)e)->P));

    /* Calculate x^3 + ax + b  mod P*/
    MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&temp2, &temp2, &temp));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(y_sqr, &temp2, &((mbedtls_ecp_group *)e)->P));

cleanup:
    mbedtls_mpi_free(&temp);
    mbedtls_mpi_free(&temp2);
    mbedtls_mpi_free(&num);
    if (ret) {
        mbedtls_mpi_free(y_sqr);
        os_free(y_sqr);
        return NULL;
    } else {
        return (struct crypto_bignum *) y_sqr;
    }
}

int crypto_ec_point_is_at_infinity(struct crypto_ec *e,
                                   const struct crypto_ec_point *p)
{
    return mbedtls_ecp_is_zero((mbedtls_ecp_point *) p);
}

int crypto_ec_point_is_on_curve(struct crypto_ec *e,
                                const struct crypto_ec_point *p)
{
    mbedtls_mpi y_sqr_lhs, *y_sqr_rhs = NULL, two;
    int ret = 0, on_curve = 0;

    mbedtls_mpi_init(&y_sqr_lhs);
    mbedtls_mpi_init(&two);

    /* Calculate y^2  mod P*/
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&two, 2));
    MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&y_sqr_lhs, &((const mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(Y), &two, &((mbedtls_ecp_group *)e)->P, NULL));

    y_sqr_rhs = (mbedtls_mpi *) crypto_ec_point_compute_y_sqr(e, (const struct crypto_bignum *) & ((const mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(X));

    if (y_sqr_rhs && (mbedtls_mpi_cmp_mpi(y_sqr_rhs, &y_sqr_lhs) == 0)) {
        on_curve = 1;
    }

cleanup:
    mbedtls_mpi_free(&y_sqr_lhs);
    mbedtls_mpi_free(&two);
    mbedtls_mpi_free(y_sqr_rhs);
    os_free(y_sqr_rhs);
    return (ret == 0) && (on_curve == 1);
}

int crypto_ec_point_cmp(const struct crypto_ec *e,
                        const struct crypto_ec_point *a,
                        const struct crypto_ec_point *b)
{
    return mbedtls_ecp_point_cmp((const mbedtls_ecp_point *) a,
                                 (const mbedtls_ecp_point *) b);
}

int crypto_ec_key_compare(struct crypto_ec_key *key1, struct crypto_ec_key *key2)
{
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        return 0;
    }

    psa_key_id_t *key1_id = (psa_key_id_t *)key1;
    psa_key_id_t *key2_id = (psa_key_id_t *)key2;

    unsigned char pub1[PSA_EXPORT_PUBLIC_KEY_MAX_SIZE];
    unsigned char pub2[PSA_EXPORT_PUBLIC_KEY_MAX_SIZE];

    size_t key1_len, key2_len;

    psa_export_public_key(*key1_id, pub1, sizeof(pub1), &key1_len);
    psa_export_public_key(*key2_id, pub2, sizeof(pub2), &key2_len);

    if ((key1_len == key2_len) && (os_memcmp(pub1, pub2, key1_len) == 0)) {
        return 1;
    }

    return 0;
}

void crypto_debug_print_point(const char *title, struct crypto_ec *e,
                              const struct crypto_ec_point *point)
{
    u8 x[32], y[32];

    if (crypto_ec_point_to_bin(e, point, x, y) < 0) {
        wpa_printf(MSG_ERROR, "error: failed to get corrdinates");
        return;
    }

    wpa_hexdump(MSG_ERROR, "x:", x, 32);
    wpa_hexdump(MSG_ERROR, "y:", y, 32);
}

static struct crypto_ec_key *crypto_alloc_key(void)
{
    /*
     * Not moving this to PSA as there is no direct replacement
     * for mbedtls_pk_context in PSA. Once all the other functions
     * are moved to PSA, this can be removed.
    */
    mbedtls_pk_context *key = os_malloc(sizeof(*key));

    if (!key) {
        wpa_printf(MSG_ERROR, "%s: memory allocation failed", __func__);
        return NULL;
    }
    mbedtls_pk_init(key);

    return (struct crypto_ec_key *)key;
}

static psa_ecc_family_t group_id_to_psa(mbedtls_ecp_group_id grp_id, size_t *bits)
{
    switch (grp_id) {
    case MBEDTLS_ECP_DP_SECP192R1:
        if (bits) {
            *bits = 192;
        }
        return PSA_ECC_FAMILY_SECP_R1;
    case MBEDTLS_ECP_DP_SECP224R1:
        if (bits) {
            *bits = 224;
        }
        return PSA_ECC_FAMILY_SECP_R1;
    case MBEDTLS_ECP_DP_SECP256R1:
        if (bits) {
            *bits = 256;
        }
        return PSA_ECC_FAMILY_SECP_R1;
    case MBEDTLS_ECP_DP_SECP384R1:
        if (bits) {
            *bits = 384;
        }
        return PSA_ECC_FAMILY_SECP_R1;
    case MBEDTLS_ECP_DP_SECP521R1:
        if (bits) {
            *bits = 521;
        }
        return PSA_ECC_FAMILY_SECP_R1;
    case MBEDTLS_ECP_DP_BP256R1:
        if (bits) {
            *bits = 256;
        }
        return PSA_ECC_FAMILY_BRAINPOOL_P_R1;
    case MBEDTLS_ECP_DP_BP384R1:
        if (bits) {
            *bits = 384;
        }
        return PSA_ECC_FAMILY_BRAINPOOL_P_R1;
    case MBEDTLS_ECP_DP_BP512R1:
        if (bits) {
            *bits = 512;
        }
        return PSA_ECC_FAMILY_BRAINPOOL_P_R1;
    case MBEDTLS_ECP_DP_CURVE25519:
        if (bits) {
            *bits = 255;
        }
        return PSA_ECC_FAMILY_MONTGOMERY;
    case MBEDTLS_ECP_DP_SECP192K1:
        if (bits) {
            *bits = 192;
        }
        return PSA_ECC_FAMILY_SECP_K1;
    case MBEDTLS_ECP_DP_SECP224K1:
        if (bits) {
            *bits = 224;
        }
        return PSA_ECC_FAMILY_SECP_K1;
    case MBEDTLS_ECP_DP_SECP256K1:
        if (bits) {
            *bits = 256;
        }
        return PSA_ECC_FAMILY_SECP_K1;
    case MBEDTLS_ECP_DP_CURVE448:
        if (bits) {
            *bits = 448;
        }
        return PSA_ECC_FAMILY_MONTGOMERY;
    default:
        if (bits) {
            *bits = 0;
        }
        return 0;
    }
}

struct crypto_ec_key * crypto_ec_key_set_pub(const struct crypto_ec_group *group,
                                             const u8 *buf, size_t len)
{
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        return NULL;
    }

    mbedtls_ecp_group *grp = (mbedtls_ecp_group *)group;
    if (!grp) {
        wpa_printf(MSG_ERROR, "Invalid ECC group");
        return NULL;
    }
    size_t key_bits = 0;
    psa_ecc_family_t ecc_family = group_id_to_psa(grp->id, &key_bits);
    psa_key_id_t *key_id = os_calloc(1, sizeof(psa_key_id_t));
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;

    u8* key_buf = NULL;
    size_t key_len = 0;
    if (PSA_KEY_TYPE_ECC_GET_FAMILY(ecc_family) != PSA_ECC_FAMILY_MONTGOMERY) {
        /*
            * For non-Montgomery curves, the public key is represented as an
            * uncompressed point (0x04 || X || Y).
            * Check if the buffer starts with 0x04 to indicate an uncompressed
            * point.
        */

        if (((len & 1) == 0) && (buf[0] != 0x04)) {
            // Key doesn't start with 0x04.
            key_buf = os_calloc(1, len + 1);
            if (!key_buf) {
                wpa_printf(MSG_ERROR, "memory allocation failed");
                return NULL;
            }

            key_buf[0] = 0x04;
            os_memcpy(key_buf + 1, buf, len);
            key_len = len + 1;
        } else {
            key_buf = os_calloc(1, len);
            if (!key_buf) {
                wpa_printf(MSG_ERROR, "memory allocation failed");
                return NULL;
            }

            os_memcpy(key_buf, buf, len);
            key_len = len;
        }
    }

    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_VERIFY_HASH | PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_EXPORT);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    psa_set_key_type(&key_attributes, ecc_family);

    status = psa_import_key(&key_attributes, key_buf, key_len, key_id);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "Failed to import key, %d", status);
        return NULL;
    }

    if (key_buf) {
        os_free(key_buf);
    }

    return (struct crypto_ec_key *)key_id;
}

struct crypto_ec_point *crypto_ec_key_get_public_key(struct crypto_ec_key *key)
{
    psa_key_id_t *pkey = (psa_key_id_t *)key;

    mbedtls_pk_context *pkey_ctx = os_calloc(1, sizeof(mbedtls_pk_context));
    if (!pkey_ctx) {
        return NULL;
    }
    mbedtls_pk_init(pkey_ctx);

    int ret = mbedtls_pk_copy_from_psa(*pkey, pkey_ctx);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to copy key from PSA");
        os_free(pkey_ctx);
        return NULL;
    }

    mbedtls_ecp_point *point = os_calloc(1, sizeof(mbedtls_ecp_point));
    if (!point) {
        mbedtls_pk_free(pkey_ctx);
        os_free(pkey_ctx);
        return NULL;
    }

    mbedtls_ecp_point_init(point);

    ret = mbedtls_ecp_copy(point, &mbedtls_pk_ec(*pkey_ctx)->MBEDTLS_PRIVATE(Q));
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to copy point");
        mbedtls_ecp_point_free(point);
        os_free(point);
        mbedtls_pk_free(pkey_ctx);
        os_free(pkey_ctx);
        return NULL;
    }

    mbedtls_pk_free(pkey_ctx);
    os_free(pkey_ctx);
    return (struct crypto_ec_point *)point;
}

int crypto_ec_get_priv_key_der(struct crypto_ec_key *key, unsigned char **key_data, int *key_len)
{
    mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;
    char *der_data = os_malloc(ECP_PRV_DER_MAX_BYTES);

    if (!der_data) {
        wpa_printf(MSG_ERROR, "memory allocation failed");
        return -1;
    }
    *key_len = mbedtls_pk_write_key_der(pkey, (unsigned char *)der_data, ECP_PRV_DER_MAX_BYTES);
    if (*key_len <= 0) {
        wpa_printf(MSG_ERROR, "Failed to write priv key");
        os_free(der_data);
        return -1;
    }
    *key_data = os_malloc(*key_len);

    if (!*key_data) {
        wpa_printf(MSG_ERROR, "memory allocation failed");
        os_free(der_data);
        return -1;
    }
    os_memcpy(*key_data, der_data + ECP_PRV_DER_MAX_BYTES - *key_len, *key_len);
    os_free(der_data);

    return 0;
}

struct crypto_ec_group *crypto_ec_get_group_from_key(struct crypto_ec_key *key)
{
    // mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;

    // return (struct crypto_ec_group *) & (mbedtls_pk_ec(*pkey)->MBEDTLS_PRIVATE(grp));
    psa_key_id_t *pkey = (psa_key_id_t *)key;

    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;

    psa_status_t status = psa_get_key_attributes(*pkey, &key_attributes);
    if (status != PSA_SUCCESS) {
        return NULL;
    }

    psa_ecc_family_t ecc_family = psa_get_key_type(&key_attributes);
    psa_ecc_family_t *curve = os_zalloc(sizeof(psa_ecc_family_t));
    if (!curve) {
        wpa_printf(MSG_ERROR, "memory allocation failed");
        return NULL;
    }
    *curve = PSA_KEY_TYPE_ECC_GET_FAMILY(ecc_family);
    // int ret = mbedtls_ecc_group_from_psa(ecc_family, bits);
    // if (ret == 0) {
    //     wpa_printf(MSG_ERROR, "Unsupported ECC group");
    // }

    // mbedtls_ecp_group *e = os_zalloc(sizeof(*e));
    // if (!e) {
    //     return NULL;
    // }

    // mbedtls_ecp_group_init(e);

    // if (mbedtls_ecp_group_load(e, ret)) {
    //     mbedtls_ecp_group_free(e);
    //     os_free(e);
    //     e = NULL;
    // }

    return (struct crypto_ec_group *)curve;
}

int crypto_ec_key_group(struct crypto_ec_key *key)
{
    // mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;

    // int iana_group = (int)crypto_ec_get_mbedtls_to_nist_group_id(mbedtls_pk_ec(*pkey)->MBEDTLS_PRIVATE(grp).id);
    // return iana_group;
    psa_key_id_t *pkey = (psa_key_id_t *)key;

    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;

    psa_status_t status = psa_get_key_attributes(*pkey, &key_attributes);
    if (status != PSA_SUCCESS) {
        return -1;
    }

    psa_ecc_family_t ecc_family = psa_get_key_type(&key_attributes);

    int iana_group = (int)crypto_ec_get_mbedtls_to_nist_group_id(ecc_family);
    return iana_group;
}

struct crypto_bignum *crypto_ec_key_get_private_key(struct crypto_ec_key *key)
{
    // mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;

    // return ((struct crypto_bignum *) & (mbedtls_pk_ec(*pkey)->MBEDTLS_PRIVATE(d)));
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        return NULL;
    }

    psa_key_id_t *pkey = (psa_key_id_t *)key;

    mbedtls_pk_context *pkey_ctx = os_calloc(1, sizeof(mbedtls_pk_context));
    if (!pkey_ctx) {
        return NULL;
    }

    mbedtls_pk_init(pkey_ctx);

    int ret = mbedtls_pk_copy_from_psa(*pkey, pkey_ctx);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to copy key from PSA");
        os_free(pkey_ctx);
        return NULL;
    }

    mbedtls_mpi *d = os_calloc(1, sizeof(mbedtls_mpi));
    if (!d) {
        mbedtls_pk_free(pkey_ctx);
        os_free(pkey_ctx);
        return NULL;
    }

    mbedtls_mpi_init(d);
    ret = mbedtls_mpi_copy(d, &mbedtls_pk_ec(*pkey_ctx)->MBEDTLS_PRIVATE(d));
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to copy private key");
        mbedtls_mpi_free(d);
        os_free(d);
        mbedtls_pk_free(pkey_ctx);
        os_free(pkey_ctx);
        return NULL;
    }

    mbedtls_pk_free(pkey_ctx);
    os_free(pkey_ctx);

    return (struct crypto_bignum *)d;
}

int crypto_ec_get_publickey_buf(struct crypto_ec_key *key, u8 *key_buf, int len)
{

    psa_key_id_t *pkey = (psa_key_id_t *)key;
    psa_status_t status = PSA_SUCCESS;

    if (key_buf == NULL && len == 0) {
        // This is a call to determine the buffer length
        // needed for the public key

        psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;

        status = psa_get_key_attributes(*pkey, &key_attributes);
        if (status != PSA_SUCCESS) {
            printf("psa_get_key_attributes failed with %d\n", status);
            return -1;
        }

        size_t key_bits = psa_get_key_bits(&key_attributes);
        if (key_bits == 0) {
            printf("psa_get_key_bits failed with %d\n", -1);
            return -1;
        }

        psa_key_type_t key_type = psa_get_key_type(&key_attributes);

        psa_reset_key_attributes(&key_attributes);
        return PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(key_type, key_bits);
    }

    size_t key_len = 0;
    status = psa_export_public_key(*pkey, key_buf, len, &key_len);
    if (status != PSA_SUCCESS) {
        printf("psa_export_public_key failed with %d\n", status);
        return -1;
    }

    return key_len;
}

int crypto_write_pubkey_der(struct crypto_ec_key *key, unsigned char **key_buf)
{
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        return -1;
    }

    unsigned char *buf = os_malloc(ECP_PUB_DER_MAX_BYTES);
    if (!buf) {
        wpa_printf(MSG_ERROR, "memory allocation failed");
        return -1;
    }

    psa_key_id_t *pkey = (psa_key_id_t *)key;
    mbedtls_pk_context *pkey_ctx = os_calloc(1, sizeof(mbedtls_pk_context));
    if (!pkey_ctx) {
        os_free(buf);
        return -1;
    }

    mbedtls_pk_init(pkey_ctx);

    int ret = mbedtls_pk_copy_from_psa(*pkey, pkey_ctx);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to copy key from PSA. ret: %d", ret);
        os_free(buf);
        mbedtls_pk_free(pkey_ctx);
        os_free(pkey_ctx);
        return -1;
    }

    int len = mbedtls_pk_write_pubkey_der(pkey_ctx, buf, ECP_PUB_DER_MAX_BYTES);
    if (len <= 0) {
        os_free(buf);
        mbedtls_pk_free(pkey_ctx);
        os_free(pkey_ctx);
        return -1;
    }

    mbedtls_pk_free(pkey_ctx);
    os_free(pkey_ctx);

    *key_buf = os_malloc(len);
    if (!*key_buf) {
        os_free(buf);
        return -1;
    }
    os_memcpy(*key_buf, buf + ECP_PUB_DER_MAX_BYTES - len, len);
    os_free(buf);

    return len;
}

struct crypto_ec_key *crypto_ec_key_parse_priv(const u8 *privkey, size_t privkey_len)
{
    /*
     * As of PSA API v1.0, there is no way to import a private key with PSA APIs without
     * knowing the metadata (such as type, size, etc.) of the key. So, we need to use
     * mbedtls_pk_parse_key() to parse the private key and then import it into PSA.
    */

    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        return NULL;
    }

    int ret;
    mbedtls_pk_context *kctx = (mbedtls_pk_context *)crypto_alloc_key();
    psa_key_id_t *key_id = os_calloc(1, sizeof(psa_key_id_t));

    if (!kctx) {
        wpa_printf(MSG_ERROR, "memory allocation failed");
        return NULL;
    }
    ret = mbedtls_pk_parse_key(kctx, privkey, privkey_len, NULL, 0);

    if (ret < 0) {
        //crypto_print_error_string(ret);
        goto fail;
    }

    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    ret = mbedtls_pk_get_psa_attributes(kctx, PSA_KEY_USAGE_DERIVE, &key_attributes);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "mbedtls_pk_get_psa_attributes failed with %d", ret);
        goto fail;
    }

    ret = mbedtls_pk_import_into_psa(kctx, &key_attributes, key_id);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "mbedtls_pk_import_into_psa failed with %d", ret);
        goto fail;
    }

    mbedtls_pk_free(kctx);
    os_free(kctx);

    return (struct crypto_ec_key *)key_id;

fail:
    mbedtls_pk_free(kctx);
    os_free(kctx);
    if (key_id) {
        psa_destroy_key(*key_id);
        os_free(key_id);
    }
    return NULL;
}

unsigned int crypto_ec_get_mbedtls_to_nist_group_id(int id)
{
    unsigned int nist_grpid = 0;
    size_t bits = 0;
    psa_ecc_family_t family = PSA_KEY_TYPE_ECC_GET_FAMILY(id);
    if (family == PSA_ECC_FAMILY_MONTGOMERY) {
        // Montgomery curves are not supported in NIST
        return 0;
    }
    switch (id) {
    case MBEDTLS_ECP_DP_SECP256R1:
        nist_grpid = 19;
        break;
    case MBEDTLS_ECP_DP_SECP384R1:
        nist_grpid = 20;
        break;
    case MBEDTLS_ECP_DP_SECP521R1:
        nist_grpid = 21;
        break;
    case MBEDTLS_ECP_DP_BP256R1:
        nist_grpid = 28;
        break;
    case MBEDTLS_ECP_DP_BP384R1:
        nist_grpid = 29;
        break;
    case MBEDTLS_ECP_DP_BP512R1:
        nist_grpid = 30;
        break;
    case PSA_ECC_FAMILY_SECP_R1:
        if (bits == 256) {
            nist_grpid = 19; // NIST P-256
        } else if (bits == 384) {
            nist_grpid = 20; // NIST P-384
        } else if (bits == 521) {
            nist_grpid = 21; // NIST P-521
        }
        break;
    case PSA_ECC_FAMILY_BRAINPOOL_P_R1:
        if (bits == 256) {
            nist_grpid = 28; // Brainpool P-256
        } else if (bits == 384) {
            nist_grpid = 29; // Brainpool P-384
        } else if (bits == 512) {
            nist_grpid = 30; // Brainpool P-512
        }
        break;
    default:
        break;
    }

    return nist_grpid;
}

int crypto_ec_get_curve_id(const struct crypto_ec_group *group)
{
    mbedtls_ecp_group *grp = (mbedtls_ecp_group *)group;
    return (crypto_ec_get_mbedtls_to_nist_group_id(grp->id));
}

int crypto_ecdh(struct crypto_ec_key *key_own, struct crypto_ec_key *key_peer,
                u8 *secret, size_t *secret_len)
{
    int ret = 0;
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        return -1;
    }

    psa_key_id_t *peer = (psa_key_id_t *)key_peer;
    psa_key_id_t *own = (psa_key_id_t *)key_own;

    unsigned char *peer_key_buf = os_calloc(PSA_EXPORT_PUBLIC_KEY_MAX_SIZE, sizeof(uint8_t));
    if (!peer_key_buf) {
        wpa_printf(MSG_ERROR, "memory allocation failed");
        ret = -1;
        goto fail;
    }

    size_t peer_key_len = 0;
    status = psa_export_public_key(*peer, peer_key_buf, PSA_EXPORT_PUBLIC_KEY_MAX_SIZE, &peer_key_len);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "psa_export_public_key failed with %d", status);
        ret = -1;
        goto fail;
    }

    psa_key_attributes_t peer_key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    status = psa_get_key_attributes(*peer, &peer_key_attributes);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "psa_get_key_attributes failed with %d", status);
        ret = -1;
        goto fail;
    }

    *secret_len = 0;
    size_t secret_length = 0;
    status = psa_raw_key_agreement(PSA_ALG_ECDH, *own, peer_key_buf, peer_key_len, secret, 66, &secret_length);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "psa_raw_key_agreement failed with %d", status);
        printf("psa_raw_key_agreement failed with %d\n", status);
        ret = -1;
        goto fail;
    }

    *secret_len = secret_length;

fail:
    if (peer_key_buf) {
        os_free(peer_key_buf);
    }

    return ret;
}

int crypto_ecdsa_get_sign(unsigned char *hash,
                          const struct crypto_bignum *r, const struct crypto_bignum *s, struct crypto_ec_key *csign, int hash_len)
{
    psa_key_id_t *pkey = (psa_key_id_t *)csign;

    (void)r;
    (void)s;

    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        return -1;
    }

    size_t signature_length = 0;
    status = psa_sign_hash(*pkey, PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_256), hash, hash_len, hash, hash_len, &signature_length);
    if (status != PSA_SUCCESS) {
        printf("psa_sign_hash failed with %d\n", status);
        return -1;
    }

    return 0;
}

int crypto_ec_key_verify_signature_r_s(struct crypto_ec_key *csign,
                                       const unsigned char *hash, int hlen,
                                       const u8 *r, size_t r_len,
                                       const u8 *s, size_t s_len)
{
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        return -1;
    }

    psa_key_id_t *key_id = (psa_key_id_t *)csign;

    u8 *sig = os_zalloc(r_len + s_len);
    if (!sig) {
        return -1;
    }

    os_memcpy(sig, r, r_len);
    os_memcpy(sig + r_len, s, s_len);

    status = psa_verify_hash(*key_id, PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_256), hash, hlen, sig, r_len + s_len);
    if (status != PSA_SUCCESS) {
        printf("psa_verify_hash failed with %d\n", status);
        os_free(sig);
        return -1;
    }

    os_free(sig);

    return 0;
}

void crypto_ec_key_debug_print(struct crypto_ec_key *key, const char *title)
{
#ifdef DEBUG_PRINT
    psa_key_id_t *pkey = (psa_key_id_t *)key;

    unsigned char pub[PSA_EXPORT_PUBLIC_KEY_MAX_SIZE];
    size_t pub_len = 0;
    psa_export_public_key(*pkey, pub, sizeof(pub), &pub_len);

    wpa_hexdump(MSG_INFO, "public key:", pub, pub_len);
    wpa_printf(MSG_INFO, "public key len: %d", pub_len);

    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_get_key_attributes(*pkey, &key_attributes);

    psa_ecc_family_t ecc_family = psa_get_key_type(&key_attributes);
    size_t bits = psa_get_key_bits(&key_attributes);
    int ret = mbedtls_ecc_group_from_psa(ecc_family, bits);
    if (ret == 0) {
        wpa_printf(MSG_ERROR, "Unsupported ECC group");
    }

    wpa_printf(MSG_INFO, "curve: %s", mbedtls_ecp_curve_info_from_grp_id(ret)->name);
    wpa_printf(MSG_INFO, "bits: %d", bits);
    wpa_printf(MSG_INFO, "psa_ecc_family: %d", ecc_family);

#endif
}

// NOTE: PSA doesn't have replacements for mbedtls_asn1_* functions
// so this function is not migrated to PSA
struct crypto_ec_key *crypto_ec_parse_subpub_key(const unsigned char *p, size_t len)
{
    int ret;
    mbedtls_pk_context *pkey = (mbedtls_pk_context *)crypto_alloc_key();

    if (!pkey) {
        return NULL;
    }
    ret = mbedtls_pk_parse_subpubkey((unsigned char **)&p, p + len, pkey);
    if (ret == 0) {
        return (struct crypto_ec_key *)pkey;
    }

    mbedtls_pk_free(pkey);
    os_free(pkey);
    return NULL;
}

// TODO: Migrate this to PSA along with crypto_ec_parse_subpub_key
int crypto_is_ec_key(struct crypto_ec_key *key)
{
    int ret = mbedtls_pk_can_do((mbedtls_pk_context *)key, MBEDTLS_PK_ECKEY);
    return  ret;
}

struct crypto_ec_key * crypto_ec_key_gen(u16 ike_group)
{
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        return NULL;
    }

    size_t key_bit_length = 0;
    psa_ecc_family_t ecc_family = group_id_to_psa(ike_group, &key_bit_length);
    if (ecc_family == 0) {
        printf("mbedtls_ecc_group_to_psa failed\n");
        return NULL;
    }

    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_DERIVE | PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH | PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_ANY_HASH));
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(ecc_family));
    psa_set_key_bits(&key_attributes, key_bit_length);

    psa_key_id_t *key_id = os_calloc(1, sizeof(psa_key_id_t));

    status = psa_generate_key(&key_attributes, key_id);
    if (status != PSA_SUCCESS) {
        return NULL;
    }

    psa_reset_key_attributes(&key_attributes);

    return (struct crypto_ec_key *)key_id;
}

static int pk_write_ec_pubkey_formatted(unsigned char **p, unsigned char *start,
                                        mbedtls_ecp_keypair *ec, int format)
{
    int ret;
    size_t len = 0;
    unsigned char buf[MBEDTLS_ECP_MAX_PT_LEN];

    if ((ret = mbedtls_ecp_point_write_binary(&ec->MBEDTLS_PRIVATE(grp), &ec->MBEDTLS_PRIVATE(Q),
                                              format,
                                              &len, buf, sizeof(buf))) != 0) {
        return (ret);
    }

    if (*p < start || (size_t)(*p - start) < len) {
        return (MBEDTLS_ERR_ASN1_BUF_TOO_SMALL);
    }

    *p -= len;
    memcpy(*p, buf, len);

    return ((int) len);
}

int mbedtls_pk_write_pubkey_formatted(unsigned char **p, unsigned char *start,
                                      const mbedtls_pk_context *key, int format)
{
    int ret;
    size_t len = 0;

    if (mbedtls_pk_get_type(key) == MBEDTLS_PK_ECKEY) {
        MBEDTLS_ASN1_CHK_ADD(len, pk_write_ec_pubkey_formatted(p, start, mbedtls_pk_ec(*key), format));
    } else {
        return (MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE);
    }

    return ((int) len);
}

int crypto_pk_write_formatted_pubkey_der(mbedtls_pk_context *key, unsigned char *buf, size_t size, int format)
{
    int ret;
    unsigned char *c;

    if (size == 0) {
        return (MBEDTLS_ERR_ASN1_BUF_TOO_SMALL);
    }

    c = buf + size;

    ret = mbedtls_pk_write_pubkey_der(key, c, size);
    if (ret < 0) {
        wpa_printf(MSG_ERROR, "mbedtls_pk_write_pubkey_der failed with %d", ret);
        return ret;
    }

    return ((int) ret);
}

int crypto_ec_write_pub_key(struct crypto_ec_key *key, unsigned char **key_buf)
{

    psa_key_id_t *pkey = (psa_key_id_t *)key;

    mbedtls_pk_context *pk = os_zalloc(sizeof(mbedtls_pk_context));
    mbedtls_pk_init(pk);

    int ret = mbedtls_pk_copy_public_from_psa(*pkey, pk);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to copy public key from psa key");
        return 0;
    }
    unsigned char *output_buf = os_zalloc(1600);
    if (!output_buf) {
        wpa_printf(MSG_ERROR, "%s: memory allocation failed", __func__);
        return 0;
    }
    int len = crypto_pk_write_formatted_pubkey_der(pk, output_buf, 1600, 1);
    if (len <= 0) {
        os_free(output_buf);
        return 0;
    }

    mbedtls_pk_free(pk);
    os_free(pk);

    *key_buf = os_malloc(len);
    if (!*key_buf) {
        wpa_printf(MSG_ERROR, "%s: memory allocation failed", __func__);
        os_free(output_buf);
        return 0;
    }
    os_memcpy(*key_buf, output_buf + 1600 - len, len);
    os_free(output_buf);

    return len;
}

struct wpabuf * crypto_ec_key_get_subject_public_key(struct crypto_ec_key *key)
{
    unsigned char *der = NULL;
    struct wpabuf *ret = NULL;
    int der_len;

    der_len = crypto_ec_write_pub_key(key, &der);
    if (!der) {
        wpa_printf(MSG_ERROR, "failed to get der for bootstrapping key\n");
        return NULL;
    } else {
        wpa_printf(MSG_DEBUG, "der_len: %d\n", der_len);
    }
    ret = wpabuf_alloc_copy(der, der_len);

    os_free(der);
    return ret;
}

int crypto_mbedtls_get_grp_id(int group)
{
    switch (group) {
    case IANA_SECP256R1:
        return MBEDTLS_ECP_DP_SECP256R1;
    case IANA_SECP384R1:
        return MBEDTLS_ECP_DP_SECP384R1;
    case IANA_SECP521R1:
        return MBEDTLS_ECP_DP_SECP521R1;
    default:
        return MBEDTLS_ECP_DP_NONE;
    }
}

void crypto_ecdh_deinit(struct crypto_ecdh *ecdh)
{
    psa_key_id_t *key_id = (psa_key_id_t *)ecdh;
    psa_destroy_key(*key_id);
    os_free(key_id);
}

struct crypto_ecdh * crypto_ecdh_init(int group)
{
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        return NULL;
    }

    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id;
    size_t key_size = 0;

    psa_ecc_family_t ecc_family = group_id_to_psa(crypto_mbedtls_get_grp_id(group), &key_size);

    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(ecc_family));
    psa_set_key_bits(&key_attributes, key_size);
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_DERIVE);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_ECDH);

    status = psa_generate_key(&key_attributes, &key_id);
    if (status != PSA_SUCCESS) {
        return NULL;
    }

    return (struct crypto_ecdh *)key_id;
}

struct wpabuf * crypto_ecdh_get_pubkey(struct crypto_ecdh *ecdh, int y)
{
    struct wpabuf *public_key = NULL;
    psa_key_id_t *key_id = (psa_key_id_t *)ecdh;

    size_t key_size = 0;

    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        return NULL;
    }

    uint8_t raw_key[PSA_EXPORT_PUBLIC_KEY_MAX_SIZE] = {0};

    status = psa_export_public_key(*key_id, raw_key, sizeof(raw_key), &key_size);
    if (status != PSA_SUCCESS) {
        return NULL;
    }

    public_key = wpabuf_alloc_copy(raw_key, key_size);
    return public_key;
}

struct wpabuf * crypto_ecdh_set_peerkey(struct crypto_ecdh *ecdh, int inc_y,
                                        const u8 *key, size_t len)
{
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        return NULL;
    }

    psa_key_id_t *key_id = (psa_key_id_t *)ecdh;

    size_t secret_len = inc_y ? 2 * len : len;
    uint8_t *secret = os_zalloc(secret_len);
    size_t secret_length = 0;

    status = psa_raw_key_agreement(PSA_ALG_ECDH, *key_id, key, len, secret, secret_len, &secret_length);
    if (status != PSA_SUCCESS) {
        return NULL;
    }

    struct wpabuf *sh_secret = wpabuf_alloc_copy(secret, secret_len);

    os_free(secret);

    return sh_secret;
}

struct crypto_ec_key *crypto_ec_key_parse_pub(const u8 *der, size_t der_len)
{
    int ret;
    mbedtls_pk_context *pkey = os_zalloc(sizeof(*pkey));

    if (!pkey) {
        return NULL;
    }

    mbedtls_pk_init(pkey);
    ret = mbedtls_pk_parse_public_key(pkey, der, der_len);

    if (ret < 0) {
        wpa_printf(MSG_ERROR, "failed to parse ec public key");
        os_free(pkey);
        return NULL;
    }
    return (struct crypto_ec_key *)pkey;
}

void crypto_ec_key_deinit(struct crypto_ec_key *key)
{
    psa_key_id_t *key_id = (psa_key_id_t *)key;
    if (key_id == NULL) {
        return;
    }
    if (*key_id != 0) {
        psa_destroy_key(*key_id);
    }
    os_free(key);
}

int crypto_ec_key_verify_signature(struct crypto_ec_key *key, const u8 *data,
                                   size_t len, const u8 *sig, size_t sig_len)
{
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        return -1;
    }

    psa_key_id_t *key_id = (psa_key_id_t *)key;
    status = psa_verify_hash(*key_id, PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_256), data, len, sig, sig_len);
    if (status != PSA_SUCCESS) {
        return -1;
    }

    return 0;
}

#endif /* CONFIG_ECC */
