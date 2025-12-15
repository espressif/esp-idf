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
#include "mbedtls/asn1write.h"
#include "mbedtls/error.h"
#include "mbedtls/oid.h"
#include <mbedtls/psa_util.h>
#include "psa/crypto.h"
#include "psa/crypto_sizes.h"
#include "esp_heap_caps.h"

#define ECP_PRV_DER_MAX_BYTES   ( 29 + 3 * MBEDTLS_ECP_MAX_BYTES )
#define ECP_PUB_DER_MAX_BYTES   ( 30 + 2 * MBEDTLS_ECP_MAX_BYTES )

#ifdef CONFIG_MBEDTLS_ECDH_LEGACY_CONTEXT
#define ACCESS_ECDH(S, var) S->MBEDTLS_PRIVATE(var)
#else
#define ACCESS_ECDH(S, var) S->MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(var)
#endif

#define ESP_SUP_MAX_ECC_KEY_SIZE 256

#ifdef CONFIG_ECC

// Wrapper structure for EC keys that includes PSA key ID, curve info, and group
// This allows us to avoid memory leaks by storing everything with the key
typedef struct {
    psa_key_id_t key_id;
    mbedtls_ecp_group_id curve_id;
    mbedtls_ecp_group group;
    mbedtls_ecp_point *cached_public_key;
    mbedtls_mpi *cached_private_key;
} crypto_ec_key_wrapper_t;

// Helper macro to get key_id from wrapper
#define GET_KEY_ID(key) (((crypto_ec_key_wrapper_t *)(key))->key_id)

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
    crypto_ec_key_wrapper_t *wrapper1 = (crypto_ec_key_wrapper_t *)key1;
    crypto_ec_key_wrapper_t *wrapper2 = (crypto_ec_key_wrapper_t *)key2;
    if (!wrapper1 || !wrapper2) {
        return 0;
    }

    unsigned char pub1[PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(ESP_SUP_MAX_ECC_KEY_SIZE)];
    unsigned char pub2[PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(ESP_SUP_MAX_ECC_KEY_SIZE)];

    size_t key1_len, key2_len;

    psa_status_t status = psa_export_public_key(wrapper1->key_id, pub1, sizeof(pub1), &key1_len);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "crypto_ec_key_compare: psa_export_public_key failed with %d", status);
        return 0;
    }
    status = psa_export_public_key(wrapper2->key_id, pub2, sizeof(pub2), &key2_len);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "crypto_ec_key_compare: psa_export_public_key failed with %d", status);
        return 0;
    }

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

// Helper to initialize group in wrapper
static int init_group_in_wrapper(crypto_ec_key_wrapper_t *wrapper)
{
    if (!wrapper || wrapper->curve_id == MBEDTLS_ECP_DP_NONE) {
        return -1;
    }

    mbedtls_ecp_group_init(&wrapper->group);
    if (mbedtls_ecp_group_load(&wrapper->group, wrapper->curve_id) != 0) {
        mbedtls_ecp_group_free(&wrapper->group);
        return -1;
    }

    return 0;
}

static psa_ecc_family_t group_id_to_psa(mbedtls_ecp_group_id grp_id, size_t *bits)
{
    switch (grp_id) {
    case MBEDTLS_ECP_DP_SECP192R1:
        if (bits) {
            *bits = 192;
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
    // case MBEDTLS_ECP_DP_SECP224K1:
    //     if (bits) {
    //         *bits = 224;
    //     }
    //     return PSA_ECC_FAMILY_SECP_K1;
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
    mbedtls_ecp_group *grp = (mbedtls_ecp_group *)group;
    if (!grp || grp->id == MBEDTLS_ECP_DP_NONE) {
        wpa_printf(MSG_ERROR, "Invalid ECC group");
        return NULL;
    }

    // Convert mbedtls group ID to PSA curve family
    size_t bits = 0;
    psa_ecc_family_t ecc_family = group_id_to_psa(grp->id, &bits);
    if (ecc_family == 0) {
        wpa_printf(MSG_ERROR, "Unsupported curve group");
        return NULL;
    }

    // Create wrapper structure with key ID and curve info
    crypto_ec_key_wrapper_t *wrapper = os_calloc(1, sizeof(crypto_ec_key_wrapper_t));
    if (!wrapper) {
        wpa_printf(MSG_ERROR, "memory allocation failed");
        return NULL;
    }
    wrapper->curve_id = grp->id;  // Store curve ID for later use
    mbedtls_ecp_group_init(&wrapper->group);  // Initialize group structure
    wrapper->group.id = MBEDTLS_ECP_DP_NONE;  // Mark as not loaded yet (lazy init)

    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;

    u8* key_buf = NULL;
    size_t key_len = 0;
    size_t key_bits = 0;

    if (ecc_family != PSA_ECC_FAMILY_MONTGOMERY) {
        /*
         * For non-Montgomery curves, the public key is represented as an
         * uncompressed point (0x04 || X || Y).
         * DPP protocol sends raw X||Y (even length, no prefix).
         * Also supports compressed format (0x02/0x03 || X) and
         * uncompressed format (0x04 || X || Y).
         */

        // Check if buffer has a format prefix (0x04, 0x02, or 0x03)
        if (len > 0 && (buf[0] == 0x04 || buf[0] == 0x02 || buf[0] == 0x03)) {
            // Already has format prefix (0x04, 0x02, or 0x03)
            key_buf = os_calloc(1, len);
            if (!key_buf) {
                wpa_printf(MSG_ERROR, "memory allocation failed");
                os_free(wrapper);
                return NULL;
            }

            os_memcpy(key_buf, buf, len);
            key_len = len;
            // For uncompressed: key_bits = (len - 1) * 4
            // For compressed: key_bits = (len - 1) * 8
            if (buf[0] == 0x04) {
                key_bits = (len - 1) * 4;
            } else {
                key_bits = (len - 1) * 8;
            }
        } else if ((len & 1) == 0) {
            // Raw X||Y format (even length, no prefix) - prepend 0x04
            key_buf = os_calloc(1, len + 1);
            if (!key_buf) {
                wpa_printf(MSG_ERROR, "memory allocation failed");
                os_free(wrapper);
                return NULL;
            }

            key_buf[0] = 0x04;
            os_memcpy(key_buf + 1, buf, len);
            key_len = len + 1;
            // key_bits = len * 4 (since len = 2 * coordinate_size)
            key_bits = len * 4;
        } else {
            // Odd length without format prefix - invalid format
            wpa_printf(MSG_ERROR, "Invalid public key format: odd length without prefix");
            os_free(wrapper);
            return NULL;
        }
    } else {
        // Montgomery curves
        key_buf = os_calloc(1, len);
        if (!key_buf) {
            wpa_printf(MSG_ERROR, "memory allocation failed");
            os_free(wrapper);
            return NULL;
        }
        os_memcpy(key_buf, buf, len);
        key_len = len;
        key_bits = len * 8;
    }

    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_VERIFY_HASH | PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_DERIVE);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(ecc_family));
    psa_set_key_bits(&key_attributes, key_bits);

    psa_status_t status = psa_import_key(&key_attributes, key_buf, key_len, &wrapper->key_id);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "Failed to import key, %d", status);
        os_free(key_buf);
        os_free(wrapper);
        return NULL;
    }

    os_free(key_buf);

    return (struct crypto_ec_key *)wrapper;
}

/**
 * crypto_ec_key_get_public_key - Get public key point from PSA key
 * @key: Pointer to crypto_ec_key (PSA key ID)
 * Returns: Pointer to mbedtls_ecp_point on success, NULL on failure
 *
 * Exports the public key directly from PSA and constructs an mbedtls_ecp_point.
 * The exported format is uncompressed point: 0x04 || X || Y
 */
struct crypto_ec_point *crypto_ec_key_get_public_key(struct crypto_ec_key *key)
{
    crypto_ec_key_wrapper_t *wrapper = (crypto_ec_key_wrapper_t *)key;
    if (!wrapper) {
        return NULL;
    }

    // Return cached public key if already computed
    if (wrapper->cached_public_key) {
        return (struct crypto_ec_point *)wrapper->cached_public_key;
    }

    psa_status_t status;

    // Export public key in uncompressed format: 0x04 || X || Y
    uint8_t pub_key_buf[PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(ESP_SUP_MAX_ECC_KEY_SIZE)];
    size_t pub_key_len = 0;

    status = psa_export_public_key(wrapper->key_id, pub_key_buf, sizeof(pub_key_buf), &pub_key_len);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "Failed to export public key: %d", status);
        return NULL;
    }

    // Validate format: should be 0x04 (uncompressed) || X || Y
    if (pub_key_len < 3 || pub_key_buf[0] != 0x04) {
        wpa_printf(MSG_ERROR, "Invalid public key format (expected uncompressed point)");
        return NULL;
    }

    if ((pub_key_len - 1) % 2 != 0) {
        wpa_printf(MSG_ERROR, "Invalid public key format: odd length");
        return NULL;
    }

    // Calculate coordinate size: (total_len - 1) / 2
    size_t coord_size = (pub_key_len - 1) / 2;

    // Allocate and initialize the ECC point (cache it in wrapper)
    mbedtls_ecp_point *point = os_calloc(1, sizeof(mbedtls_ecp_point));
    if (!point) {
        wpa_printf(MSG_ERROR, "Failed to allocate ECC point");
        return NULL;
    }
    mbedtls_ecp_point_init(point);

    // Parse X coordinate
    int ret = mbedtls_mpi_read_binary(&point->MBEDTLS_PRIVATE(X),
                                      pub_key_buf + 1, coord_size);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to parse X coordinate: -0x%04x", -ret);
        mbedtls_ecp_point_free(point);
        os_free(point);
        return NULL;
    }

    // Parse Y coordinate
    ret = mbedtls_mpi_read_binary(&point->MBEDTLS_PRIVATE(Y),
                                  pub_key_buf + 1 + coord_size, coord_size);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to parse Y coordinate: -0x%04x", -ret);
        mbedtls_ecp_point_free(point);
        os_free(point);
        return NULL;
    }

    // Set Z coordinate to 1 (affine coordinates)
    ret = mbedtls_mpi_lset(&point->MBEDTLS_PRIVATE(Z), 1);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to set Z coordinate: -0x%04x", -ret);
        mbedtls_ecp_point_free(point);
        os_free(point);
        return NULL;
    }

    // Cache the point in wrapper for later cleanup
    wrapper->cached_public_key = point;

    return (struct crypto_ec_point *)point;
}

/**
 * crypto_ec_get_priv_key_der - Export private key in DER format
 * @key: Pointer to crypto_ec_key (PSA key ID)
 * @key_data: Output buffer for DER-encoded private key (caller must free)
 * @key_len: Length of the DER-encoded key
 * Returns: 0 on success, -1 on failure
 *
 * Note: Uses mbedtls temporarily for DER encoding as PSA lacks DER export API
 */
int crypto_ec_get_priv_key_der(struct crypto_ec_key *key, unsigned char **key_data, int *key_len)
{
    if (!key || !key_data || !key_len) {
        wpa_printf(MSG_ERROR, "Invalid parameters for DER export");
        return -1;
    }

    crypto_ec_key_wrapper_t *wrapper = (crypto_ec_key_wrapper_t *)key;
    if (!wrapper) {
        wpa_printf(MSG_ERROR, "Invalid key parameter for DER export");
        return -1;
    }

    // Use mbedtls temporarily for DER encoding (PSA has no DER export)
    mbedtls_pk_context pk_ctx;
    mbedtls_pk_init(&pk_ctx);

    int ret = mbedtls_pk_copy_from_psa(wrapper->key_id, &pk_ctx);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to copy key from PSA: -0x%04x", -ret);
        mbedtls_pk_free(&pk_ctx);
        return -1;
    }

    // Allocate buffer for DER encoding
    char *der_data = os_malloc(ECP_PRV_DER_MAX_BYTES);
    if (!der_data) {
        wpa_printf(MSG_ERROR, "Memory allocation failed for DER buffer");
        mbedtls_pk_free(&pk_ctx);
        return -1;
    }

    // Write private key to DER format
    *key_len = mbedtls_pk_write_key_der(&pk_ctx, (unsigned char *)der_data, ECP_PRV_DER_MAX_BYTES);
    mbedtls_pk_free(&pk_ctx);

    if (*key_len <= 0) {
        wpa_printf(MSG_ERROR, "Failed to write private key to DER: -0x%04x", -*key_len);
        os_free(der_data);
        return -1;
    }

    // Allocate output buffer
    *key_data = os_malloc(*key_len);
    if (!*key_data) {
        wpa_printf(MSG_ERROR, "Memory allocation failed for output buffer");
        os_free(der_data);
        return -1;
    }

    // Copy DER data (mbedtls writes from end of buffer)
    os_memcpy(*key_data, der_data + ECP_PRV_DER_MAX_BYTES - *key_len, *key_len);
    os_free(der_data);

    return 0;
}

struct crypto_ec_group *crypto_ec_get_group_from_key(struct crypto_ec_key *key)
{
    crypto_ec_key_wrapper_t *wrapper = (crypto_ec_key_wrapper_t *)key;
    if (!wrapper || wrapper->curve_id == MBEDTLS_ECP_DP_NONE) {
        return NULL;
    }

    // Group is stored directly in the wrapper, initialize if not already done
    if (wrapper->group.id == MBEDTLS_ECP_DP_NONE) {
        if (init_group_in_wrapper(wrapper) != 0) {
            wpa_printf(MSG_ERROR, "crypto_ec_get_group_from_key: Failed to initialize group for curve %d", wrapper->curve_id);
            return NULL;
        }
    }

    return (struct crypto_ec_group *)&wrapper->group;
}

int crypto_ec_key_group(struct crypto_ec_key *key)
{
    crypto_ec_key_wrapper_t *wrapper = (crypto_ec_key_wrapper_t *)key;
    if (!wrapper) {
        return -1;
    }

    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;

    psa_status_t status = psa_get_key_attributes(wrapper->key_id, &key_attributes);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "crypto_ec_key_group: psa_get_key_attributes failed: %d", status);
        psa_reset_key_attributes(&key_attributes);
        return -1;
    }

    psa_key_type_t key_type = psa_get_key_type(&key_attributes);
    psa_ecc_family_t ecc_family = PSA_KEY_TYPE_ECC_GET_FAMILY(key_type);
    int key_bits = psa_get_key_bits(&key_attributes);
    psa_reset_key_attributes(&key_attributes);

    int iana_group = (int)crypto_ec_get_mbedtls_to_nist_group_id(ecc_family, key_bits);
    return iana_group;
}

struct crypto_bignum *crypto_ec_key_get_private_key(struct crypto_ec_key *key)
{
    crypto_ec_key_wrapper_t *wrapper = (crypto_ec_key_wrapper_t *)key;
    if (!wrapper) {
        return NULL;
    }

    // Return cached private key if already computed
    if (wrapper->cached_private_key) {
        return (struct crypto_bignum *)wrapper->cached_private_key;
    }

    mbedtls_pk_context *pkey_ctx = os_calloc(1, sizeof(mbedtls_pk_context));
    if (!pkey_ctx) {
        return NULL;
    }

    mbedtls_pk_init(pkey_ctx);

    int ret = mbedtls_pk_copy_from_psa(wrapper->key_id, pkey_ctx);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to copy key from PSA");
        mbedtls_pk_free(pkey_ctx);
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

    // Access the EC keypair directly from the PK context
    // pkey_ctx->pk_ctx points to the underlying EC keypair
    mbedtls_ecp_keypair *ec_key = (mbedtls_ecp_keypair *)(pkey_ctx->MBEDTLS_PRIVATE(pk_ctx));
    if (!ec_key) {
        wpa_printf(MSG_ERROR, "Failed to get EC keypair from PK context");
        mbedtls_mpi_free(d);
        os_free(d);
        mbedtls_pk_free(pkey_ctx);
        os_free(pkey_ctx);
        return NULL;
    }

    ret = mbedtls_mpi_copy(d, &ec_key->MBEDTLS_PRIVATE(d));
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

    // Cache the private key in wrapper for later cleanup
    wrapper->cached_private_key = d;

    return (struct crypto_bignum *)d;
}

int crypto_ec_get_publickey_buf(struct crypto_ec_key *key, u8 *key_buf, int len)
{
    crypto_ec_key_wrapper_t *wrapper = (crypto_ec_key_wrapper_t *)key;
    if (!wrapper) {
        return -1;
    }

    if (key_buf == NULL && len != 0) {
        return -1;
    }

    psa_status_t status = PSA_SUCCESS;

    if (key_buf == NULL) {
        // This is a call to determine the buffer length
        // needed for the public key

        psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;

        status = psa_get_key_attributes(wrapper->key_id, &key_attributes);
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
    status = psa_export_public_key(wrapper->key_id, key_buf, len, &key_len);
    if (status != PSA_SUCCESS) {
        printf("psa_export_public_key failed with %d\n", status);
        return -1;
    }

    return key_len;
}

int crypto_write_pubkey_der(struct crypto_ec_key *key, unsigned char **key_buf)
{
    unsigned char *buf = os_malloc(ECP_PUB_DER_MAX_BYTES);
    if (!buf) {
        wpa_printf(MSG_ERROR, "memory allocation failed");
        return -1;
    }

    crypto_ec_key_wrapper_t *wrapper = (crypto_ec_key_wrapper_t *)key;
    if (!wrapper) {
        os_free(buf);
        return -1;
    }
    mbedtls_pk_context *pkey_ctx = os_calloc(1, sizeof(mbedtls_pk_context));
    if (!pkey_ctx) {
        os_free(buf);
        return -1;
    }

    mbedtls_pk_init(pkey_ctx);

    int ret = mbedtls_pk_copy_from_psa(wrapper->key_id, pkey_ctx);
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

/**
 * crypto_ec_key_parse_priv - Parse private key and import to PSA
 * @privkey: DER-encoded private key data
 * @privkey_len: Length of private key data
 * Returns: Pointer to crypto_ec_key (PSA key ID) or NULL on failure
 *
 * Note: Uses mbedtls for DER parsing (PSA needs metadata), returns PSA key
 */
struct crypto_ec_key *crypto_ec_key_parse_priv(const u8 *privkey, size_t privkey_len)
{
    /*
     * As of PSA API v1.0, there is no way to import a private key with PSA APIs without
     * knowing the metadata (such as type, size, etc.) of the key. So, we need to use
     * mbedtls_pk_parse_key() to parse the private key and then import it into PSA.
    */

    int ret;
    mbedtls_pk_context kctx_storage;
    mbedtls_pk_context *kctx = &kctx_storage;

    // Create wrapper structure
    crypto_ec_key_wrapper_t *wrapper = os_calloc(1, sizeof(crypto_ec_key_wrapper_t));
    if (!wrapper) {
        wpa_printf(MSG_ERROR, "Memory allocation failed for key wrapper");
        return NULL;
    }
    mbedtls_ecp_group_init(&wrapper->group);
    wrapper->group.id = MBEDTLS_ECP_DP_NONE;

    mbedtls_pk_init(kctx);
    ret = mbedtls_pk_parse_key(kctx, privkey, privkey_len, NULL, 0);

    if (ret < 0) {
        goto fail;
    }

    // Get PSA attributes from parsed key to extract curve information
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    ret = mbedtls_pk_get_psa_attributes(kctx, PSA_KEY_USAGE_DERIVE, &key_attributes);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "mbedtls_pk_get_psa_attributes failed with %d", ret);
        goto fail;
    }

    // Extract curve ID from PSA attributes
    psa_key_type_t key_type = psa_get_key_type(&key_attributes);
    psa_ecc_family_t ecc_family = PSA_KEY_TYPE_ECC_GET_FAMILY(key_type);
    int key_bits = psa_get_key_bits(&key_attributes);
    if (ecc_family != 0 && key_bits > 0) {
        // Map PSA ECC family to mbedtls curve ID
        mbedtls_ecp_group_id grp_id = MBEDTLS_ECP_DP_NONE;
        if (ecc_family == PSA_ECC_FAMILY_SECP_R1) {
            if (key_bits == 256) {
                grp_id = MBEDTLS_ECP_DP_SECP256R1;
            } else if (key_bits == 384) {
                grp_id = MBEDTLS_ECP_DP_SECP384R1;
            } else if (key_bits == 521) {
                grp_id = MBEDTLS_ECP_DP_SECP521R1;
            }
        }
        wrapper->curve_id = grp_id;
    } else {
        wrapper->curve_id = MBEDTLS_ECP_DP_NONE;
    }

    // Allow ECDH as enrollment algorithm for key agreement operations
    // Note: While usage flags allow DERIVE, the algorithm policy must also permit ECDH.
    // This enables keys imported by this function to be used for ECDH operations.
    psa_set_key_enrollment_algorithm(&key_attributes, PSA_ALG_ECDH);

    ret = mbedtls_pk_import_into_psa(kctx, &key_attributes, &wrapper->key_id);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "mbedtls_pk_import_into_psa failed with %d", ret);
        goto fail;
    }

    mbedtls_pk_free(kctx);

    return (struct crypto_ec_key *)wrapper;

fail:
    mbedtls_pk_free(kctx);
    if (wrapper) {
        if (wrapper->key_id != 0) {
            psa_destroy_key(wrapper->key_id);
        }
        // Always free group since we always initialize it
        mbedtls_ecp_group_free(&wrapper->group);
        os_free(wrapper);
    }
    return NULL;
}

unsigned int crypto_ec_get_mbedtls_to_nist_group_id(int id, int bits)
{
    unsigned int nist_grpid = 0;
    switch (id) {
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

int crypto_ecdh(struct crypto_ec_key *key_own, struct crypto_ec_key *key_peer,
                u8 *secret, size_t *secret_len)
{
    int ret = 0;

    crypto_ec_key_wrapper_t *peer_wrapper = (crypto_ec_key_wrapper_t *)key_peer;
    crypto_ec_key_wrapper_t *own_wrapper = (crypto_ec_key_wrapper_t *)key_own;
    if (!peer_wrapper || !own_wrapper) {
        return -1;
    }

    unsigned char *peer_key_buf = os_calloc(PSA_EXPORT_PUBLIC_KEY_MAX_SIZE, sizeof(uint8_t));
    if (!peer_key_buf) {
        wpa_printf(MSG_ERROR, "memory allocation failed");
        ret = -1;
        goto fail;
    }

    size_t peer_key_len = 0;
    psa_status_t status = psa_export_public_key(peer_wrapper->key_id, peer_key_buf, PSA_EXPORT_PUBLIC_KEY_MAX_SIZE, &peer_key_len);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "psa_export_public_key failed with %d", status);
        ret = -1;
        goto fail;
    }

    *secret_len = 0;
    size_t secret_length = 0;

    // Debug: Check key attributes before key agreement
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t attr_status = psa_get_key_attributes(own_wrapper->key_id, &key_attributes);
    if (attr_status == PSA_SUCCESS) {
        psa_key_usage_t usage = psa_get_key_usage_flags(&key_attributes);
        psa_algorithm_t alg = psa_get_key_algorithm(&key_attributes);
        psa_key_type_t key_type = psa_get_key_type(&key_attributes);
        wpa_printf(MSG_DEBUG, "crypto_ecdh: key usage=0x%x, algorithm=0x%x, type=0x%x",
                   usage, alg, key_type);
        printf("crypto_ecdh: key usage=0x%x, algorithm=0x%x, type=0x%x\n",
               usage, alg, key_type);
        psa_reset_key_attributes(&key_attributes);
    }

    status = psa_raw_key_agreement(PSA_ALG_ECDH, own_wrapper->key_id, peer_key_buf, peer_key_len, secret, 66, &secret_length);
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
    crypto_ec_key_wrapper_t *wrapper = (crypto_ec_key_wrapper_t *)csign;
    if (!wrapper) {
        return -1;
    }

    size_t key_size = hash_len / 2;
    unsigned char signature[128];  // Max for P-521
    size_t signature_length = 0;

    psa_status_t status = psa_sign_hash(wrapper->key_id, PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_256), hash, hash_len, signature, sizeof(signature), &signature_length);
    if (status != PSA_SUCCESS) {
        printf("psa_sign_hash failed with %d\n", status);
        return -1;
    }

    // Extract r component
    int ret = mbedtls_mpi_read_binary((mbedtls_mpi *)r, signature, key_size);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to parse r: -0x%04x", -ret);
        return -1;
    }

    // Extract s component
    ret = mbedtls_mpi_read_binary((mbedtls_mpi *)s, signature + key_size, key_size);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to parse s: -0x%04x", -ret);
        return -1;
    }

    return 0;
}

int crypto_ec_key_verify_signature_r_s(struct crypto_ec_key *csign,
                                       const unsigned char *hash, int hlen,
                                       const u8 *r, size_t r_len,
                                       const u8 *s, size_t s_len)
{
    crypto_ec_key_wrapper_t *wrapper = (crypto_ec_key_wrapper_t *)csign;
    if (!wrapper) {
        return -1;
    }

    u8 *sig = os_zalloc(r_len + s_len);
    if (!sig) {
        return -1;
    }

    os_memcpy(sig, r, r_len);
    os_memcpy(sig + r_len, s, s_len);

    psa_status_t status = psa_verify_hash(wrapper->key_id, PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_256), hash, hlen, sig, r_len + s_len);
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
    crypto_ec_key_wrapper_t *wrapper = (crypto_ec_key_wrapper_t *)key;
    if (!wrapper) {
        return;
    }

    unsigned char pub[PSA_EXPORT_PUBLIC_KEY_MAX_SIZE] = {0};
    size_t pub_len = 0;
    psa_status_t status = psa_export_public_key(wrapper->key_id, pub, sizeof(pub), &pub_len);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "crypto_ec_key_debug_print: psa_export_public_key failed with %d", status);
        return;
    }

    wpa_hexdump(MSG_INFO, "public key:", pub, pub_len);
    wpa_printf(MSG_INFO, "public key len: %d", pub_len);

    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    status = psa_get_key_attributes(wrapper->key_id, &key_attributes);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "crypto_ec_key_debug_print: psa_get_key_attributes failed with %d", status);
        return;
    }

    psa_key_type_t key_type = psa_get_key_type(&key_attributes);
    psa_ecc_family_t ecc_family = PSA_KEY_TYPE_ECC_GET_FAMILY(key_type);
    size_t bits = psa_get_key_bits(&key_attributes);
    wpa_printf(MSG_INFO, "bits: %d", bits);
    wpa_printf(MSG_INFO, "psa_ecc_family: %d", ecc_family);

    psa_reset_key_attributes(&key_attributes);

#endif
}

/**
 * crypto_ec_parse_subpub_key - Parse ASN.1 SubjectPublicKey and import to PSA
 * @p: Pointer to ASN.1 encoded SubjectPublicKey
 * @len: Length of the ASN.1 data
 * Returns: Pointer to crypto_ec_key (PSA key ID) or NULL on failure
 *
 * Note: Uses mbedtls for ASN.1 parsing (no PSA equivalent), but returns PSA key
 */
struct crypto_ec_key *crypto_ec_parse_subpub_key(const unsigned char *p, size_t len)
{
    // Use mbedtls_pk_parse_public_key() to parse SubjectPublicKeyInfo
    // This is the recommended replacement for the deprecated mbedtls_pk_parse_subpubkey()
    mbedtls_pk_context pk_ctx;
    mbedtls_pk_init(&pk_ctx);

    int ret = mbedtls_pk_parse_public_key(&pk_ctx, p, len);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to parse SubjectPublicKeyInfo: -0x%04x", -ret);
        mbedtls_pk_free(&pk_ctx);
        return NULL;
    }

    // Get the EC keypair from the PK context
    mbedtls_ecp_keypair *ec = (mbedtls_ecp_keypair *)(pk_ctx.MBEDTLS_PRIVATE(pk_ctx));
    if (!ec) {
        wpa_printf(MSG_ERROR, "Failed to get EC keypair from parsed key");
        mbedtls_pk_free(&pk_ctx);
        return NULL;
    }

    mbedtls_ecp_group_id grp_id = ec->MBEDTLS_PRIVATE(grp).id;

    // Convert mbedtls curve ID to PSA curve family and bits
    size_t key_bits = 0;
    psa_ecc_family_t ecc_family = group_id_to_psa(grp_id, &key_bits);
    if (ecc_family == 0) {
        wpa_printf(MSG_ERROR, "Unsupported or invalid curve: %d", grp_id);
        mbedtls_pk_free(&pk_ctx);
        return NULL;
    }

    // Export public key in uncompressed format for PSA import
    unsigned char pub_key_buf[PSA_EXPORT_PUBLIC_KEY_MAX_SIZE] = {0};
    size_t pub_key_len = 0;

    ret = mbedtls_ecp_point_write_binary(
              &ec->MBEDTLS_PRIVATE(grp),
              &ec->MBEDTLS_PRIVATE(Q),
              MBEDTLS_ECP_PF_UNCOMPRESSED,
              &pub_key_len,
              pub_key_buf,
              sizeof(pub_key_buf)
          );

    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to export public key: -0x%04x", -ret);
        mbedtls_pk_free(&pk_ctx);
        return NULL;
    }

    // Done with mbedtls temporary context
    mbedtls_pk_free(&pk_ctx);

    // Create wrapper structure
    crypto_ec_key_wrapper_t *wrapper = os_calloc(1, sizeof(crypto_ec_key_wrapper_t));
    if (!wrapper) {
        wpa_printf(MSG_ERROR, "Memory allocation failed for key wrapper");
        return NULL;
    }
    wrapper->curve_id = grp_id;  // Store curve ID
    mbedtls_ecp_group_init(&wrapper->group);  // Initialize group structure
    wrapper->group.id = MBEDTLS_ECP_DP_NONE;  // Mark as not loaded yet (lazy init)

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes,
                            PSA_KEY_USAGE_VERIFY_HASH | PSA_KEY_USAGE_VERIFY_MESSAGE | PSA_KEY_USAGE_EXPORT);
    psa_set_key_algorithm(&attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(ecc_family));
    psa_set_key_bits(&attributes, key_bits);

    psa_status_t status = psa_import_key(&attributes, pub_key_buf, pub_key_len, &wrapper->key_id);
    psa_reset_key_attributes(&attributes);

    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "Failed to import key to PSA: %d", status);
        mbedtls_ecp_group_free(&wrapper->group);
        os_free(wrapper);
        return NULL;
    }

    return (struct crypto_ec_key *)wrapper;
}

/**
 * crypto_is_ec_key - Check if a key is an EC key
 * @key: Pointer to crypto_ec_key (PSA key ID)
 * Returns: 1 if key is an EC key, 0 otherwise
 */
int crypto_is_ec_key(struct crypto_ec_key *key)
{
    if (!key) {
        return 0;
    }

    crypto_ec_key_wrapper_t *wrapper = (crypto_ec_key_wrapper_t *)key;

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t status = psa_get_key_attributes(wrapper->key_id, &attributes);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_DEBUG, "Failed to get key attributes: %d", status);
        return 0;
    }

    psa_key_type_t key_type = psa_get_key_type(&attributes);
    psa_reset_key_attributes(&attributes);

    // Check if it's an ECC key (public or private key pair)
    return PSA_KEY_TYPE_IS_ECC(key_type) ? 1 : 0;
}

struct crypto_ec_key * crypto_ec_key_gen(u16 ike_group)
{
    size_t key_bit_length = 0;

    // Hardcoded this to match the current master implementation with mbedTLS
    // That also enforces the use of the same curve for the key pair
    psa_ecc_family_t ecc_family = PSA_ECC_FAMILY_SECP_R1;
    key_bit_length = ESP_SUP_MAX_ECC_KEY_SIZE;
    mbedtls_ecp_group_id curve_id = MBEDTLS_ECP_DP_SECP256R1;  // P-256

    // Create wrapper structure
    crypto_ec_key_wrapper_t *wrapper = os_calloc(1, sizeof(crypto_ec_key_wrapper_t));
    if (!wrapper) {
        wpa_printf(MSG_ERROR, "Memory allocation failed for key wrapper");
        return NULL;
    }
    wrapper->curve_id = curve_id;
    mbedtls_ecp_group_init(&wrapper->group);
    wrapper->group.id = MBEDTLS_ECP_DP_NONE;

    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_DERIVE | PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH | PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_ANY_HASH));
    // Allow ECDH as enrollment algorithm for key agreement operations
    // Note: While usage flags allow DERIVE, the algorithm policy must also permit ECDH.
    // This enables keys created by this function to be used for ECDH operations.
    psa_set_key_enrollment_algorithm(&key_attributes, PSA_ALG_ECDH);
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(ecc_family));
    psa_set_key_bits(&key_attributes, key_bit_length);

    psa_status_t status = psa_generate_key(&key_attributes, &wrapper->key_id);
    if (status != PSA_SUCCESS) {
        os_free(wrapper);
        psa_reset_key_attributes(&key_attributes);
        return NULL;
    }

    psa_reset_key_attributes(&key_attributes);

    return (struct crypto_ec_key *)wrapper;
}

int crypto_pk_write_formatted_pubkey_der(psa_key_id_t key_id, unsigned char *buf, size_t size, int format)
{
    int ret;
    unsigned char *c;
    size_t len = 0, par_len = 0;

    if (size == 0) {
        return (MBEDTLS_ERR_ASN1_BUF_TOO_SMALL);
    }

    c = buf + size;

    // Export the public key directly from PSA using the key ID
    unsigned char *point_buf = os_calloc(PSA_EXPORT_PUBLIC_KEY_MAX_SIZE, sizeof(unsigned char));
    if (!point_buf) {
        return MBEDTLS_ERR_PK_ALLOC_FAILED;
    }
    size_t point_len = 0;

    psa_status_t status = psa_export_public_key(key_id, point_buf, PSA_EXPORT_PUBLIC_KEY_MAX_SIZE, &point_len);
    if (status != PSA_SUCCESS) {
        os_free(point_buf);
        wpa_printf(MSG_ERROR, "psa_export_public_key failed: %d", status);
        return MBEDTLS_ERR_PK_BAD_INPUT_DATA;
    }

    // If compressed format requested and we have uncompressed data, convert it
    if (format == MBEDTLS_ECP_PF_COMPRESSED && point_len > 1 && point_buf[0] == 0x04) {
        // Uncompressed format: 0x04 || X || Y
        // Compressed format: 0x02/0x03 || X (based on Y's parity)
        size_t coord_len = (point_len - 1) / 2;
        unsigned char compressed[PSA_EXPORT_PUBLIC_KEY_MAX_SIZE];

        // Determine compression prefix based on Y coordinate's last bit (LSB of last byte)
        compressed[0] = 0x02 | (point_buf[point_len - 1] & 0x01);
        os_memcpy(compressed + 1, point_buf + 1, coord_len);
        point_len = coord_len + 1;
        os_memcpy(point_buf, compressed, point_len);
    }

    // Write point data to buffer
    if (c - buf < (int)point_len) {
        os_free(point_buf);
        return MBEDTLS_ERR_ASN1_BUF_TOO_SMALL;
    }
    c -= point_len;
    os_memcpy(c, point_buf, point_len);
    len += point_len;
    os_free(point_buf);

    // Add BIT STRING wrapper with padding byte
    if (c - buf < 1) {
        return MBEDTLS_ERR_ASN1_BUF_TOO_SMALL;
    }
    *--c = 0;
    len += 1;

    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_len(&c, buf, len));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_tag(&c, buf, MBEDTLS_ASN1_BIT_STRING));

    // Get curve parameters from PSA key attributes and write curve OID
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    status = psa_get_key_attributes(key_id, &attributes);
    if (status == PSA_SUCCESS) {
        psa_key_type_t key_type = psa_get_key_type(&attributes);
        size_t bits = psa_get_key_bits(&attributes);
        psa_reset_key_attributes(&attributes);

        psa_ecc_family_t family = PSA_KEY_TYPE_ECC_GET_FAMILY(key_type);

        // Map PSA curve to OID
        const char *curve_oid = NULL;
        size_t curve_oid_len = 0;

        // OID for secp256r1 (prime256v1): 1.2.840.10045.3.1.7
        static const char oid_secp256r1[] = {0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07};
        // OID for secp384r1: 1.3.132.0.34
        static const char oid_secp384r1[] = {0x2B, 0x81, 0x04, 0x00, 0x22};
        // OID for secp521r1: 1.3.132.0.35
        static const char oid_secp521r1[] = {0x2B, 0x81, 0x04, 0x00, 0x23};

        if (family == PSA_ECC_FAMILY_SECP_R1) {
            if (bits == 256) {
                curve_oid = oid_secp256r1;
                curve_oid_len = sizeof(oid_secp256r1);
            } else if (bits == 384) {
                curve_oid = oid_secp384r1;
                curve_oid_len = sizeof(oid_secp384r1);
            } else if (bits == 521) {
                curve_oid = oid_secp521r1;
                curve_oid_len = sizeof(oid_secp521r1);
            }
        }

        if (curve_oid) {
            MBEDTLS_ASN1_CHK_ADD(par_len, mbedtls_asn1_write_oid(&c, buf, curve_oid, curve_oid_len));
        }
    }

    // OID for id-ecPublicKey: 1.2.840.10045.2.1
    static const char oid_ec_pubkey[] = {0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01};

    // Write algorithm identifier with parameters
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_algorithm_identifier(&c, buf,
                                                                      oid_ec_pubkey,
                                                                      sizeof(oid_ec_pubkey),
                                                                      par_len));

    // Write outer SEQUENCE
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_len(&c, buf, len));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_tag(&c, buf, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE));

    return (int) len;
}

int crypto_ec_write_pub_key(struct crypto_ec_key *key, unsigned char **key_buf)
{

    crypto_ec_key_wrapper_t *wrapper = (crypto_ec_key_wrapper_t *)key;
    if (!wrapper) {
        return 0;
    }

    unsigned char *output_buf = os_zalloc(1600);
    if (!output_buf) {
        wpa_printf(MSG_ERROR, "%s: memory allocation failed", __func__);
        return 0;
    }

    int len = crypto_pk_write_formatted_pubkey_der(wrapper->key_id, output_buf, 1600, 1);
    if (len <= 0) {
        os_free(output_buf);
        return 0;
    }

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
    if (!ecdh) {
        return;
    }
    psa_key_id_t *key_id = (psa_key_id_t *)ecdh;
    psa_destroy_key(*key_id);
    os_free(key_id);
}

struct crypto_ecdh * crypto_ecdh_init(int group)
{
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t *key_id = os_calloc(1, sizeof(psa_key_id_t));
    if (!key_id) {
        return NULL;
    }
    size_t key_size = 0;

    psa_ecc_family_t ecc_family = group_id_to_psa(crypto_mbedtls_get_grp_id(group), &key_size);
    if (ecc_family == 0) {
        os_free(key_id);
        wpa_printf(MSG_ERROR, "group_id_to_psa failed, group: %d", group);
        return NULL;
    }

    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(ecc_family));
    psa_set_key_bits(&key_attributes, key_size);
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_DERIVE);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_ECDH);

    psa_status_t status = psa_generate_key(&key_attributes, key_id);
    if (status != PSA_SUCCESS) {
        os_free(key_id);
        psa_reset_key_attributes(&key_attributes);
        return NULL;
    }

    psa_reset_key_attributes(&key_attributes);

    return (struct crypto_ecdh *)key_id;
}

struct wpabuf * crypto_ecdh_get_pubkey(struct crypto_ecdh *ecdh, int y)
{
    struct wpabuf *public_key = NULL;
    psa_key_id_t *key_id = (psa_key_id_t *)ecdh;

    size_t key_size = 0;

    uint8_t raw_key[PSA_EXPORT_PUBLIC_KEY_MAX_SIZE] = {0};

    psa_status_t status = psa_export_public_key(*key_id, raw_key, sizeof(raw_key), &key_size);
    if (status != PSA_SUCCESS) {
        return NULL;
    }

    size_t coord_size = (key_size - 1) / 2;
    if (y) {
        public_key = wpabuf_alloc_copy(raw_key + 1, key_size - 1);
    } else {
        public_key = wpabuf_alloc_copy(raw_key + 1, coord_size);
    }

    return public_key;
}

struct wpabuf * crypto_ecdh_set_peerkey(struct crypto_ecdh *ecdh, int inc_y,
                                        const u8 *key, size_t len)
{
    psa_key_id_t *key_id = (psa_key_id_t *)ecdh;

    size_t secret_len = inc_y ? 2 * len : len;
    uint8_t *secret = os_zalloc(secret_len);
    if (!secret) {
        return NULL;
    }
    size_t secret_length = 0;

    /* PSA expects peer public key in uncompressed format: 0x04 || X || Y
     * For OWE (inc_y=0), we only have X coordinate - but PSA requires full uncompressed format.
     * For full keys (inc_y=1), we have X || Y and need to prepend 0x04.
     */
    uint8_t *peer_key_buf = NULL;
    size_t peer_key_len = 0;

    if (inc_y) {
        /* Full public key: prepend 0x04 prefix for uncompressed format */
        peer_key_len = 1 + len;  /* len should be 64 for P-256 (X+Y) */
        peer_key_buf = os_zalloc(peer_key_len);
        if (!peer_key_buf) {
            os_free(secret);
            return NULL;
        }
        peer_key_buf[0] = 0x04; /* Uncompressed point format */
        os_memcpy(peer_key_buf + 1, key, len);
    } else {
        /* Only X coordinate provided (OWE case): need to convert to uncompressed format
         * RFC 8110: OWE transmits only X coordinate (32 bytes for P-256).
         * PSA expects uncompressed format: 0x04 || X || Y (65 bytes for P-256).
         * Use mbedtls to convert compressed (0x02 || X) to uncompressed (0x04 || X || Y).
         */
        mbedtls_ecp_group grp;
        mbedtls_ecp_point pt;
        mbedtls_ecp_group_init(&grp);
        mbedtls_ecp_point_init(&pt);

        int ret = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
        if (ret != 0) {
            wpa_printf(MSG_ERROR, "Failed to load ECC group: -0x%04x", -ret);
            mbedtls_ecp_point_free(&pt);
            mbedtls_ecp_group_free(&grp);
            os_free(secret);
            return NULL;
        }

        /* Create compressed format buffer: 0x02 || X (assuming even Y) */
        uint8_t *compressed = os_zalloc(1 + len);
        if (!compressed) {
            mbedtls_ecp_point_free(&pt);
            mbedtls_ecp_group_free(&grp);
            os_free(secret);
            return NULL;
        }
        compressed[0] = 0x02; /* Compressed format with even Y */
        os_memcpy(compressed + 1, key, len);

        /* Parse compressed point - mbedtls will compute Y from X */
        ret = mbedtls_ecp_point_read_binary(&grp, &pt, compressed, 1 + len);
        os_free(compressed);

        if (ret != 0) {
            /* Try with odd Y (0x03) if even Y failed */
            compressed = os_zalloc(1 + len);
            if (!compressed) {
                mbedtls_ecp_point_free(&pt);
                mbedtls_ecp_group_free(&grp);
                os_free(secret);
                return NULL;
            }
            compressed[0] = 0x03; /* Compressed format with odd Y */
            os_memcpy(compressed + 1, key, len);

            ret = mbedtls_ecp_point_read_binary(&grp, &pt, compressed, 1 + len);
            os_free(compressed);

            if (ret != 0) {
                wpa_printf(MSG_ERROR, "Failed to parse compressed ECC point: -0x%04x", -ret);
                mbedtls_ecp_point_free(&pt);
                mbedtls_ecp_group_free(&grp);
                os_free(secret);
                return NULL;
            }
        }

        /* Export point in uncompressed format: 0x04 || X || Y */
        peer_key_len = 1 + 2 * len;  /* 65 bytes for P-256 */
        peer_key_buf = os_zalloc(peer_key_len);
        if (!peer_key_buf) {
            mbedtls_ecp_point_free(&pt);
            mbedtls_ecp_group_free(&grp);
            os_free(secret);
            return NULL;
        }

        size_t olen = 0;
        ret = mbedtls_ecp_point_write_binary(&grp, &pt, MBEDTLS_ECP_PF_UNCOMPRESSED,
                                             &olen, peer_key_buf, peer_key_len);
        if (ret != 0 || olen != peer_key_len) {
            wpa_printf(MSG_ERROR, "Failed to export uncompressed ECC point: -0x%04x", -ret);
            os_free(peer_key_buf);
            mbedtls_ecp_point_free(&pt);
            mbedtls_ecp_group_free(&grp);
            os_free(secret);
            return NULL;
        }

        mbedtls_ecp_point_free(&pt);
        mbedtls_ecp_group_free(&grp);
    }

    psa_status_t status = psa_raw_key_agreement(PSA_ALG_ECDH, *key_id, peer_key_buf, peer_key_len,
                                                secret, secret_len, &secret_length);
    os_free(peer_key_buf);

    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "psa_raw_key_agreement failed with PSA error 0x%x", status);
        os_free(secret);
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

    // Extract curve ID from parsed key
    mbedtls_ecp_keypair *ec = (mbedtls_ecp_keypair *)(pkey->MBEDTLS_PRIVATE(pk_ctx));
    mbedtls_ecp_group_id grp_id = MBEDTLS_ECP_DP_NONE;
    if (ec) {
        grp_id = ec->MBEDTLS_PRIVATE(grp).id;
    }

    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    ret = mbedtls_pk_get_psa_attributes(pkey, PSA_KEY_USAGE_VERIFY_HASH, &key_attributes);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "mbedtls_pk_get_psa_attributes failed with %d", ret);
        mbedtls_pk_free(pkey);
        os_free(pkey);
        return NULL;
    }

    // Create wrapper structure
    crypto_ec_key_wrapper_t *wrapper = os_calloc(1, sizeof(crypto_ec_key_wrapper_t));
    if (!wrapper) {
        wpa_printf(MSG_ERROR, "Memory allocation failed for key wrapper");
        mbedtls_pk_free(pkey);
        os_free(pkey);
        return NULL;
    }
    wrapper->curve_id = grp_id;  // Store curve ID
    mbedtls_ecp_group_init(&wrapper->group);
    wrapper->group.id = MBEDTLS_ECP_DP_NONE;  // Mark as not loaded yet (lazy init)

    ret = mbedtls_pk_import_into_psa(pkey, &key_attributes, &wrapper->key_id);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "mbedtls_pk_import_into_psa failed with %d", ret);
        mbedtls_ecp_group_free(&wrapper->group);
        os_free(wrapper);
        mbedtls_pk_free(pkey);
        os_free(pkey);
        return NULL;
    }
    psa_reset_key_attributes(&key_attributes);
    mbedtls_pk_free(pkey);
    os_free(pkey);

    return (struct crypto_ec_key *)wrapper;
}

void crypto_ec_key_deinit(struct crypto_ec_key *key)
{
    crypto_ec_key_wrapper_t *wrapper = (crypto_ec_key_wrapper_t *)key;
    if (wrapper == NULL) {
        return;
    }
    if (wrapper->key_id != 0) {
        psa_destroy_key(wrapper->key_id);
    }
    // Free the group if it was initialized
    if (wrapper->group.id != MBEDTLS_ECP_DP_NONE) {
        mbedtls_ecp_group_free(&wrapper->group);
    }
    // Free cached public key point if allocated
    if (wrapper->cached_public_key) {
        mbedtls_ecp_point_free(wrapper->cached_public_key);
        os_free(wrapper->cached_public_key);
    }
    // Free cached private key bignum if allocated
    if (wrapper->cached_private_key) {
        mbedtls_mpi_free(wrapper->cached_private_key);
        os_free(wrapper->cached_private_key);
    }
    os_free(wrapper);
}

int crypto_ec_key_verify_signature(struct crypto_ec_key *key, const u8 *data,
                                   size_t len, const u8 *sig, size_t sig_len)
{
    crypto_ec_key_wrapper_t *wrapper = (crypto_ec_key_wrapper_t *)key;
    if (!wrapper) {
        return -1;
    }

    /* Get key attributes to extract key_bits needed for DER-to-raw conversion */
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t status = psa_get_key_attributes(wrapper->key_id, &key_attributes);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "crypto_ec_key_verify_signature: psa_get_key_attributes failed: %d", status);
        psa_reset_key_attributes(&key_attributes);
        return -1;
    }

    size_t key_bits = psa_get_key_bits(&key_attributes);
    psa_reset_key_attributes(&key_attributes);

    /* Determine hash algorithm from data length */
    psa_algorithm_t verify_alg;
    if (len == 32) {
        verify_alg = PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_256);
    } else if (len == 48) {
        verify_alg = PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_384);
    } else if (len == 64) {
        verify_alg = PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_512);
    } else if (len == 20) {
        verify_alg = PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_1);
    } else {
        wpa_printf(MSG_ERROR, "crypto_ec_key_verify_signature: Unsupported hash length %d", (int)len);
        return -1;
    }

    /* Convert DER-encoded signature to raw format (r||s) for PSA */
    /* PSA verify_hash expects raw format, not DER format */
    /* API specification requires DER format input */
    /* Raw signature length = 2 * PSA_BITS_TO_BYTES(key_bits), max 132 bytes for P-521 */
    unsigned char raw_sig[132];
    size_t raw_sig_len = 0;
    const u8 *sig_to_verify = sig;
    size_t sig_len_to_verify = sig_len;

    /* Check if signature is DER format (starts with 0x30) */
    if (sig_len > 0 && sig[0] == 0x30) {
        /* Convert DER to raw format */
        int ret = mbedtls_ecdsa_der_to_raw(key_bits, sig, sig_len,
                                           raw_sig, sizeof(raw_sig), &raw_sig_len);
        if (ret != 0) {
            wpa_printf(MSG_ERROR, "crypto_ec_key_verify_signature: Failed to convert DER to raw format: %d", ret);
            return -1;
        }
        sig_to_verify = raw_sig;
        sig_len_to_verify = raw_sig_len;
    } else {
        /* Signature must be in DER format as per API specification */
        wpa_printf(MSG_ERROR, "crypto_ec_key_verify_signature: Invalid signature format (expected DER, got 0x%02x)", sig_len > 0 ? sig[0] : 0);
        return -1;
    }

    /* Perform signature verification */
    status = psa_verify_hash(wrapper->key_id, verify_alg,
                             data, len, sig_to_verify, sig_len_to_verify);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "crypto_ec_key_verify_signature: psa_verify_hash failed: %d", status);
        return -1;
    }

    return 1;
}

#endif /* CONFIG_ECC */
