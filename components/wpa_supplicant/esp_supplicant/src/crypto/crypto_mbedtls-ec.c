/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef ESP_PLATFORM
#include "esp_system.h"
#include "esp_random.h"
#include "soc/soc_caps.h"
#include "mbedtls/bignum.h"
#include "mbedtls/esp_mbedtls_random.h"
#if CONFIG_MBEDTLS_HARDWARE_ECC
#include "ecc_impl.h"
#endif
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
#include "mbedtls/oid.h"
#include "mbedtls/platform_util.h"
#include "p256_common.h"

#define ECP_PRV_DER_MAX_BYTES   ( 29 + 3 * MBEDTLS_ECP_MAX_BYTES )
#define ECP_PUB_DER_MAX_BYTES   ( 30 + 2 * MBEDTLS_ECP_MAX_BYTES )

#ifdef CONFIG_MBEDTLS_ECDH_LEGACY_CONTEXT
#define ACCESS_ECDH(S, var) S->MBEDTLS_PRIVATE(var)
#else
#define ACCESS_ECDH(S, var) S->MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(var)
#endif

#define ESP_WIFI_P256_SOFT_ACCEL \
    (CONFIG_ESP_WIFI_P256_ACCEL && CONFIG_MBEDTLS_HARDWARE_MPI)

#define ESP_WIFI_P256_SOFT_ACCEL_PREFERRED \
    (ESP_WIFI_P256_SOFT_ACCEL && !CONFIG_MBEDTLS_HARDWARE_ECC)

#if CONFIG_MBEDTLS_HARDWARE_ECC
static bool crypto_ec_point_mul_curve_supported(const mbedtls_ecp_group *grp)
{
    switch (grp->id) {
    case MBEDTLS_ECP_DP_SECP192R1:
    case MBEDTLS_ECP_DP_SECP256R1:
#if SOC_ECC_SUPPORT_CURVE_P384
    case MBEDTLS_ECP_DP_SECP384R1:
#endif
        return true;
    default:
        return false;
    }
}

static int crypto_ec_point_mul_ecc_hw(const mbedtls_ecp_group *grp,
                                      const mbedtls_ecp_point *p,
                                      const mbedtls_mpi *k,
                                      mbedtls_ecp_point *res)
{
    int ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    ecc_point_t p_hw = { 0 };
    ecc_point_t r_hw = { 0 };
    unsigned char scalar_le[MAX_SIZE] = { 0 };
    size_t curve_len = grp->pbits / 8;

    if (!crypto_ec_point_mul_curve_supported(grp)) {
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    if (curve_len != P192_LEN && curve_len != P256_LEN
#if SOC_ECC_SUPPORT_CURVE_P384
            && curve_len != P384_LEN
#endif
       ) {
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    /* Preserve mbedTLS input validation semantics for this fast path. */
    MBEDTLS_MPI_CHK(mbedtls_ecp_check_privkey(grp, k));
    MBEDTLS_MPI_CHK(mbedtls_ecp_check_pubkey(grp, p));

    p_hw.len = curve_len;
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary_le(&p->MBEDTLS_PRIVATE(X), p_hw.x, MAX_SIZE));
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary_le(&p->MBEDTLS_PRIVATE(Y), p_hw.y, MAX_SIZE));
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary_le(k, scalar_le, MAX_SIZE));

    if (esp_ecc_point_multiply(&p_hw, scalar_le, &r_hw, false) != 0) {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        goto cleanup;
    }

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary_le(&res->MBEDTLS_PRIVATE(X), r_hw.x, curve_len));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary_le(&res->MBEDTLS_PRIVATE(Y), r_hw.y, curve_len));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&res->MBEDTLS_PRIVATE(Z), 1));

cleanup:
    mbedtls_platform_zeroize(scalar_le, sizeof(scalar_le));
    mbedtls_platform_zeroize(&p_hw, sizeof(p_hw));
    mbedtls_platform_zeroize(&r_hw, sizeof(r_hw));
    return ret;
}
#endif

static mbedtls_ecp_group_id crypto_ec_get_iana_to_mbedtls_group_id(int group)
{
    switch (group) {
    case IANA_SECP256R1:
        return MBEDTLS_ECP_DP_SECP256R1;
    case IANA_SECP384R1:
        return MBEDTLS_ECP_DP_SECP384R1;
    case IANA_SECP521R1:
        return MBEDTLS_ECP_DP_SECP521R1;
    case 28:
        return MBEDTLS_ECP_DP_BP256R1;
    case 29:
        return MBEDTLS_ECP_DP_BP384R1;
    case 30:
        return MBEDTLS_ECP_DP_BP512R1;
    default:
        return MBEDTLS_ECP_DP_NONE;
    }
}

static unsigned int crypto_ec_get_mbedtls_to_iana_group_id(mbedtls_ecp_group_id id)
{
    switch (id) {
    case MBEDTLS_ECP_DP_SECP256R1:
        return IANA_SECP256R1;
    case MBEDTLS_ECP_DP_SECP384R1:
        return IANA_SECP384R1;
    case MBEDTLS_ECP_DP_SECP521R1:
        return IANA_SECP521R1;
    case MBEDTLS_ECP_DP_BP256R1:
        return 28;
    case MBEDTLS_ECP_DP_BP384R1:
        return 29;
    case MBEDTLS_ECP_DP_BP512R1:
        return 30;
    default:
        return 0;
    }
}
#ifdef CONFIG_ECC

static int crypto_ec_is_p256_group(const mbedtls_ecp_group *grp)
{
    return grp && grp->id == MBEDTLS_ECP_DP_SECP256R1;
}

struct crypto_ec *crypto_ec_init(int group)
{
    mbedtls_ecp_group *e;
    mbedtls_ecp_group_id grp_id = crypto_ec_get_iana_to_mbedtls_group_id(group);

    if (grp_id == MBEDTLS_ECP_DP_NONE) {
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

#if CONFIG_ESP_WIFI_P256_ACCEL && CONFIG_MBEDTLS_HARDWARE_MPI && !CONFIG_MBEDTLS_HARDWARE_ECC
typedef struct {
    u32 X[P256_WORDS];
    u32 Y[P256_WORDS];
    u32 Z[P256_WORDS];
} p256_fast_jac_point;

static int p256_words_is_one(const u32 *a)
{
    size_t i;

    if (a[0] != 1U) {
        return 0;
    }

    for (i = 1; i < P256_WORDS; i++) {
        if (a[i] != 0U) {
            return 0;
        }
    }

    return 1;
}

static int p256_words_cmp(const u32 *x, const u32 *y)
{
    int i;

    for (i = P256_WORDS - 1; i >= 0; i--) {
        if (x[i] > y[i]) {
            return 1;
        }
        if (x[i] < y[i]) {
            return -1;
        }
    }

    return 0;
}

static u32 p256_fast_words_add_carry(u32 *z, const u32 *x, const u32 *y)
{
    size_t i;
    u64 carry = 0;

    for (i = 0; i < P256_WORDS; i++) {
        u64 sum = (u64) x[i] + y[i] + carry;

        z[i] = (u32) sum;
        carry = sum >> 32;
    }

    return (u32) carry;
}

static void p256_fast_words_add_mod(u32 *z, const u32 *x, const u32 *y)
{
    u32 reduced[P256_WORDS];
    u32 carry = p256_fast_words_add_carry(z, x, y);
    u32 borrow = p256_words_sub_borrow(reduced, z, p256_p_le);
    u32 use_sub = carry | (1U - borrow);

    p256_words_cmov(z, reduced, use_sub);
}

static void p256_fast_words_sub_mod(u32 *z, const u32 *x, const u32 *y)
{
    u32 tmp[P256_WORDS];

    if (p256_words_sub_borrow(z, x, y) != 0) {
        (void) p256_fast_words_add_carry(tmp, z, p256_p_le);
        os_memcpy(z, tmp, sizeof(tmp));
    }
}

static void p256_words_rshift1_with_carry(u32 *z, u32 carry)
{
    int i;

    for (i = P256_WORDS - 1; i >= 0; i--) {
        u32 next = z[i] & 1U;

        z[i] = (z[i] >> 1) | (carry << 31);
        carry = next;
    }
}

static void p256_fast_half_mod(u32 *z)
{
    if (z[0] & 1U) {
        u32 tmp[P256_WORDS];
        u32 carry = p256_fast_words_add_carry(tmp, z, p256_p_le);

        p256_words_rshift1_with_carry(tmp, carry);
        os_memcpy(z, tmp, sizeof(tmp));
        return;
    }

    p256_words_rshift1_with_carry(z, 0);
}

static int p256_fast_inv_std(u32 out[P256_WORDS], const u32 in[P256_WORDS])
{
    u32 u[P256_WORDS], v[P256_WORDS], r[P256_WORDS], s[P256_WORDS];

    if (p256_words_is_zero(in)) {
        return -1;
    }

    os_memcpy(u, in, sizeof(u));
    os_memcpy(v, p256_p_le, sizeof(v));
    os_memset(r, 0, sizeof(r));
    os_memset(s, 0, sizeof(s));
    r[0] = 1U;

    while (!p256_words_is_one(u) && !p256_words_is_one(v)) {
        while ((u[0] & 1U) == 0U) {
            p256_words_rshift1_with_carry(u, 0);
            p256_fast_half_mod(r);
        }

        while ((v[0] & 1U) == 0U) {
            p256_words_rshift1_with_carry(v, 0);
            p256_fast_half_mod(s);
        }

        if (p256_words_cmp(u, v) >= 0) {
            (void) p256_words_sub_borrow(u, u, v);
            p256_fast_words_sub_mod(r, r, s);
        } else {
            (void) p256_words_sub_borrow(v, v, u);
            p256_fast_words_sub_mod(s, s, r);
        }
    }

    os_memcpy(out, p256_words_is_one(u) ? r : s, sizeof(u));
    return 0;
}

static void p256_fast_to_mont(u32 out[P256_WORDS],
                              const u32 in[P256_WORDS])
{
    p256_mont_mul(out, in, p256_r2_le);
}

static void p256_fast_from_mont(u32 out[P256_WORDS],
                                const u32 in[P256_WORDS])
{
    static const u32 one[P256_WORDS] = {1};

    p256_mont_mul(out, in, one);
}

static const u32 p256_base_comb4_x[16][P256_WORDS] = {
    {0},
    {0x18a9143cU, 0x79e730d4U, 0x5fedb601U, 0x75ba95fcU, 0x77622510U, 0x79fb732bU, 0xa53755c6U, 0x18905f76U},
    {0x16a0d2bbU, 0x4f922fc5U, 0x1a623499U, 0x0d5cc16cU, 0x57c62c8bU, 0x9241cf3aU, 0xfd1b667fU, 0x2f5e6961U},
    {0xe137bbbcU, 0x9e566847U, 0x8a6a0becU, 0xe434469eU, 0x79d73463U, 0xb1c42761U, 0x133d0015U, 0x5abe0285U},
    {0xbfe20925U, 0x62a8c244U, 0x8fdce867U, 0x91c19ac3U, 0xdd387063U, 0x5a96a5d5U, 0x21d324f6U, 0x61d587d4U},
    {0x2cb19ffdU, 0x1c891f2bU, 0xb1923c23U, 0x01ba8d5bU, 0x8ac5ca8eU, 0xb6d03d67U, 0x1f13bedcU, 0x586eb04cU},
    {0xd2b533d5U, 0x62577734U, 0xa1bdddc0U, 0x673b8af6U, 0xa79ec293U, 0x577e7c9aU, 0xc3b266b1U, 0xbb6de651U},
    {0x1ae5aa1cU, 0xbd6a38e1U, 0x49e73658U, 0xb8b7652bU, 0xee5f87edU, 0x0b130014U, 0xaeebffcdU, 0x9d0f27b2U},
    {0xf4f8b16aU, 0x56f8410eU, 0xc47b266aU, 0x97241afeU, 0x6d9c87c1U, 0x0a406b8eU, 0xcd42ab1bU, 0x803f3e02U},
    {0xc379ab34U, 0x846a56f2U, 0x841df8d1U, 0xa8ee068bU, 0x176c68efU, 0x20314459U, 0x915f1f30U, 0xf1af32d5U},
    {0xd5be5a2bU, 0xed93e225U, 0x5934f3c6U, 0x6fe79983U, 0x22626ffcU, 0x43140926U, 0x7990216aU, 0x50bbb4d9U},
    {0x9b391593U, 0xfc68b5c5U, 0x598270fcU, 0xc385f5a2U, 0xd19adcbbU, 0x7144f3aaU, 0x83fbae0cU, 0xdd558999U},
    {0x80ec21feU, 0x5fe14bfeU, 0xc255be82U, 0xf6ce116aU, 0x2f4a5d67U, 0x98bc5a07U, 0xdb7e63afU, 0xfad27148U},
    {0xa56c0dd7U, 0x1e9ecc49U, 0x46086c74U, 0xa5cffcd8U, 0xf505aeceU, 0x8f7a1408U, 0xbef0c47eU, 0xb37b85c0U},
    {0x95c8f8beU, 0x0a1c7294U, 0x3bf362bfU, 0x2961c480U, 0xdf63d4acU, 0x9e418403U, 0x91ece900U, 0xc109f9cbU},
    {0x42913074U, 0x0d5ae356U, 0x48a542b1U, 0x55491b27U, 0xb310732aU, 0x469ca665U, 0x5f1a4cc1U, 0x29591d52U},
};

static const u32 p256_base_comb4_y[16][P256_WORDS] = {
    {0},
    {0xce95560aU, 0xddf25357U, 0xba19e45cU, 0x8b4ab8e4U, 0xdd21f325U, 0xd2e88688U, 0x25885d85U, 0x8571ff18U},
    {0xf5a01797U, 0x5c15c70bU, 0x60956192U, 0x3d20b44dU, 0x071fdb52U, 0x04911b37U, 0x8d6f0f7bU, 0xf648f916U},
    {0xc04c7dabU, 0x92aa837cU, 0x43260c07U, 0x573d9f4cU, 0x78e6cc37U, 0x0c931562U, 0x6b6f7383U, 0x94bb725bU},
    {0xa37173eaU, 0xe87673a2U, 0x53778b65U, 0x23848008U, 0x05bab43eU, 0x10f8441eU, 0x4621efbeU, 0xfa11fe12U},
    {0x27e8ed09U, 0x0c35c6e5U, 0x1819ede2U, 0x1e81a33cU, 0x56c652faU, 0x278fd6c0U, 0x70864f11U, 0x19d5ac08U},
    {0xb65259b3U, 0xe7e9303aU, 0xd03a7480U, 0xd6a0afd3U, 0x9b3cfc27U, 0xc5ac83d1U, 0x5d18b99bU, 0x60b4619aU},
    {0x7a730a55U, 0xca924631U, 0xddbbc83aU, 0x9c955b2fU, 0xac019a71U, 0x07c1dfe0U, 0x356ec48dU, 0x244a566dU},
    {0x04dbec69U, 0x7f0309a8U, 0x3bbad05fU, 0xa83b85f7U, 0xad8e197fU, 0xc6097273U, 0x5067adc1U, 0xc097440eU},
    {0x5d75bd50U, 0x99c37531U, 0xf72f67bcU, 0x837cffbaU, 0x48d7723fU, 0x0613a418U, 0xe2d41c8bU, 0x23d0f130U},
    {0xe57ec63eU, 0x378191c6U, 0x181dcdb2U, 0x65422c40U, 0x0236e0f6U, 0x41a8099bU, 0x01fe49c3U, 0x2b100118U},
    {0x74b82ff4U, 0x93b88b8eU, 0x71e734c9U, 0xd2e03c40U, 0x43c0322aU, 0x9a7a9eafU, 0x149d6041U, 0xe6e4c551U},
    {0x29ab05b3U, 0x90c0b6acU, 0x4e251ae6U, 0x37a9a83cU, 0xc2aade7dU, 0x0a7dc875U, 0x9f0e1a84U, 0x77387de3U},
    {0xcc0e6a8fU, 0x3596b6e4U, 0x6b388f23U, 0xfd6d4bbfU, 0xc39cef4eU, 0xaba453faU, 0xf9f628d5U, 0x9c135ac8U},
    {0x58945705U, 0xc2d095d0U, 0xddeb85c0U, 0xb9083d96U, 0x7a40449bU, 0x84692b8dU, 0x2eee1ee1U, 0x9bc3344fU},
    {0xb84f983fU, 0xe76f5b6bU, 0x9f5f84e1U, 0xbe7eef41U, 0x80baa189U, 0x1200d496U, 0x18ef332cU, 0x6376551fU},
};

static void p256_fast_sqr(u32 out[P256_WORDS], const u32 in[P256_WORDS])
{
    p256_mont_mul(out, in, in);
}

static void p256_fast_mul(u32 out[P256_WORDS],
                          const u32 a[P256_WORDS],
                          const u32 b[P256_WORDS])
{
    p256_mont_mul(out, a, b);
}

static void p256_fast_dbl(u32 out[P256_WORDS], const u32 in[P256_WORDS])
{
    p256_fast_words_add_mod(out, in, in);
}

static void p256_fast_triple(u32 out[P256_WORDS], const u32 in[P256_WORDS])
{
    u32 tmp[P256_WORDS];

    p256_fast_dbl(tmp, in);
    p256_fast_words_add_mod(out, tmp, in);
}

static void p256_fast_eight(u32 out[P256_WORDS], const u32 in[P256_WORDS])
{
    u32 tmp[P256_WORDS];

    p256_fast_dbl(tmp, in);
    p256_fast_dbl(tmp, tmp);
    p256_fast_dbl(out, tmp);
}

static void p256_fast_point_set_zero(p256_fast_jac_point *p)
{
    os_memset(p, 0, sizeof(*p));
}

static inline u32 p256_words_get_bit(const u32 *scalar, unsigned bit)
{
    return (scalar[bit / 32] >> (bit % 32)) & 1U;
}

static inline u32 p256_words_get_window(const u32 *scalar,
                                        unsigned bit,
                                        unsigned width)
{
    u32 value = scalar[bit / 32] >> (bit % 32);

    if ((bit % 32) + width > 32 && (bit / 32) + 1 < P256_WORDS) {
        value |= scalar[(bit / 32) + 1] << (32 - (bit % 32));
    }

    return value & ((1U << width) - 1U);
}

static void p256_fast_point_from_affine(p256_fast_jac_point *p,
                                        const u32 x[P256_WORDS],
                                        const u32 y[P256_WORDS],
                                        const u32 one_mont[P256_WORDS])
{
    os_memcpy(p->X, x, sizeof(p->X));
    os_memcpy(p->Y, y, sizeof(p->Y));
    os_memcpy(p->Z, one_mont, sizeof(p->Z));
}

#define P256_WINDOW_BITS               4U
#define P256_WINDOW_ENTRY_COUNT        (1U << P256_WINDOW_BITS)
#define P256_WINDOW_PRECOMP_COUNT      (P256_WINDOW_ENTRY_COUNT - 1U)
#define P256_WINDOW_BATCH_COUNT        (P256_WINDOW_PRECOMP_COUNT - 1U)
#define P256_SCALAR_WINDOW_COUNT       ((P256_WORDS * 32U) / P256_WINDOW_BITS)

static void p256_fast_point_double(p256_fast_jac_point *r)
{
    u32 z2[P256_WORDS], y2[P256_WORDS], y4[P256_WORDS];
    u32 s[P256_WORDS], m[P256_WORDS], x3[P256_WORDS];
    u32 y3[P256_WORDS], z3[P256_WORDS], tmp1[P256_WORDS];
    u32 tmp2[P256_WORDS];

    if (p256_words_is_zero(r->Z) || p256_words_is_zero(r->Y)) {
        p256_fast_point_set_zero(r);
        return;
    }

    p256_fast_sqr(z2, r->Z);
    p256_fast_sqr(y2, r->Y);
    p256_fast_sqr(y4, y2);

    p256_fast_mul(s, r->X, y2);
    p256_fast_dbl(s, s);
    p256_fast_dbl(s, s);

    p256_fast_words_add_mod(tmp1, r->X, z2);
    p256_fast_words_sub_mod(tmp2, r->X, z2);
    p256_fast_mul(m, tmp1, tmp2);
    p256_fast_triple(m, m);

    p256_fast_sqr(x3, m);
    p256_fast_words_sub_mod(x3, x3, s);
    p256_fast_words_sub_mod(x3, x3, s);

    p256_fast_words_sub_mod(tmp1, s, x3);
    p256_fast_mul(y3, m, tmp1);
    p256_fast_eight(tmp2, y4);
    p256_fast_words_sub_mod(y3, y3, tmp2);

    p256_fast_mul(z3, r->Y, r->Z);
    p256_fast_dbl(z3, z3);

    os_memcpy(r->X, x3, sizeof(r->X));
    os_memcpy(r->Y, y3, sizeof(r->Y));
    os_memcpy(r->Z, z3, sizeof(r->Z));
}

static void p256_fast_point_add_mixed(p256_fast_jac_point *r,
                                      const u32 qx[P256_WORDS],
                                      const u32 qy[P256_WORDS],
                                      const u32 one_mont[P256_WORDS])
{
    u32 z1z1[P256_WORDS], z1z1z1[P256_WORDS];
    u32 u2[P256_WORDS], s2[P256_WORDS], h[P256_WORDS];
    u32 rr[P256_WORDS], hh[P256_WORDS], hhh[P256_WORDS];
    u32 v[P256_WORDS], x3[P256_WORDS], y3[P256_WORDS];
    u32 z3[P256_WORDS], tmp[P256_WORDS], y1[P256_WORDS];

    if (p256_words_is_zero(r->Z)) {
        p256_fast_point_from_affine(r, qx, qy, one_mont);
        return;
    }

    os_memcpy(y1, r->Y, sizeof(y1));

    p256_fast_sqr(z1z1, r->Z);
    p256_fast_mul(z1z1z1, z1z1, r->Z);
    p256_fast_mul(u2, qx, z1z1);
    p256_fast_mul(s2, qy, z1z1z1);
    p256_fast_words_sub_mod(h, u2, r->X);
    p256_fast_words_sub_mod(rr, s2, y1);

    if (p256_words_is_zero(h)) {
        if (p256_words_is_zero(rr)) {
            p256_fast_point_double(r);
        } else {
            p256_fast_point_set_zero(r);
        }
        return;
    }

    p256_fast_sqr(hh, h);
    p256_fast_mul(hhh, hh, h);
    p256_fast_mul(v, r->X, hh);

    p256_fast_sqr(x3, rr);
    p256_fast_words_sub_mod(x3, x3, hhh);
    p256_fast_words_sub_mod(x3, x3, v);
    p256_fast_words_sub_mod(x3, x3, v);

    p256_fast_words_sub_mod(tmp, v, x3);
    p256_fast_mul(y3, rr, tmp);
    p256_fast_mul(tmp, y1, hhh);
    p256_fast_words_sub_mod(y3, y3, tmp);

    p256_fast_mul(z3, r->Z, h);

    os_memcpy(r->X, x3, sizeof(r->X));
    os_memcpy(r->Y, y3, sizeof(r->Y));
    os_memcpy(r->Z, z3, sizeof(r->Z));
}

static int p256_fast_point_normalize(const mbedtls_ecp_group *grp,
                                     const p256_fast_jac_point *p,
                                     mbedtls_ecp_point *res)
{
    u32 z_std[P256_WORDS], inv_std[P256_WORDS], inv_mont[P256_WORDS];
    u32 x_std[P256_WORDS], y_std[P256_WORDS], tmp[P256_WORDS];
    int ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;

    if (p256_words_is_zero(p->Z)) {
        return mbedtls_ecp_set_zero(res);
    }

    p256_fast_from_mont(z_std, p->Z);
    if (p256_fast_inv_std(inv_std, z_std) != 0) {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        goto cleanup;
    }

    p256_fast_to_mont(inv_mont, inv_std);
    p256_fast_mul(y_std, p->Y, inv_mont);
    p256_fast_sqr(tmp, inv_mont);
    p256_fast_mul(x_std, p->X, tmp);
    p256_fast_mul(y_std, y_std, tmp);
    p256_fast_from_mont(x_std, x_std);
    p256_fast_from_mont(y_std, y_std);

    MBEDTLS_MPI_CHK(p256_words_to_mpi(x_std, &res->MBEDTLS_PRIVATE(X)));
    MBEDTLS_MPI_CHK(p256_words_to_mpi(y_std, &res->MBEDTLS_PRIVATE(Y)));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&res->MBEDTLS_PRIVATE(Z), 1));

cleanup:
    return ret;
}

static int p256_fast_points_batch_to_affine_mont(
    const p256_fast_jac_point *points, size_t num,
    u32(*xs)[P256_WORDS], u32(*ys)[P256_WORDS])
{
    u32 prefix[P256_WINDOW_BATCH_COUNT][P256_WORDS];
    u32 prod_std[P256_WORDS], inv_std[P256_WORDS];
    u32 running_inv[P256_WORDS], inv_z[P256_WORDS];
    u32 tmp[P256_WORDS];
    size_t i;

    if (!points || !xs || !ys) {
        return -1;
    }

    if (num == 0) {
        return 0;
    }

    if (num > ARRAY_SIZE(prefix)) {
        return -1;
    }

    os_memcpy(prefix[0], points[0].Z, sizeof(prefix[0]));
    for (i = 1; i < num; i++) {
        p256_fast_mul(prefix[i], prefix[i - 1], points[i].Z);
    }

    p256_fast_from_mont(prod_std, prefix[num - 1]);
    if (p256_fast_inv_std(inv_std, prod_std) != 0) {
        return -1;
    }

    p256_fast_to_mont(running_inv, inv_std);

    for (i = num; i-- > 0;) {
        if (i == 0) {
            os_memcpy(inv_z, running_inv, sizeof(inv_z));
        } else {
            p256_fast_mul(inv_z, running_inv, prefix[i - 1]);
        }

        p256_fast_sqr(tmp, inv_z);
        p256_fast_mul(xs[i], points[i].X, tmp);
        p256_fast_mul(tmp, tmp, inv_z);
        p256_fast_mul(ys[i], points[i].Y, tmp);

        p256_fast_mul(tmp, running_inv, points[i].Z);
        os_memcpy(running_inv, tmp, sizeof(running_inv));
    }

    return 0;
}

struct p256_window4_scratch {
    p256_fast_jac_point precomp[P256_WINDOW_PRECOMP_COUNT];
    u32 table_x[P256_WINDOW_ENTRY_COUNT][P256_WORDS];
    u32 table_y[P256_WINDOW_ENTRY_COUNT][P256_WORDS];
};

static int crypto_ec_point_mul_p256_window4_core(const mbedtls_ecp_group *grp,
                                                 const mbedtls_ecp_point *p,
                                                 const mbedtls_mpi *k,
                                                 p256_fast_jac_point *r,
                                                 bool validate_inputs)
{
    struct p256_window4_scratch *scratch = NULL;
    u32 scalar[P256_WORDS], x_std[P256_WORDS], y_std[P256_WORDS];
    u32 x_mont[P256_WORDS], y_mont[P256_WORDS], one_mont[P256_WORDS];
    static const u32 one_std[P256_WORDS] = {1};
    int window;
    int ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    bool started = false;

    if (!grp || grp->id != MBEDTLS_ECP_DP_SECP256R1 ||
            mbedtls_mpi_cmp_int(&p->MBEDTLS_PRIVATE(Z), 1) != 0) {
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    if (validate_inputs) {
        MBEDTLS_MPI_CHK(mbedtls_ecp_check_privkey(grp, k));
        MBEDTLS_MPI_CHK(mbedtls_ecp_check_pubkey(grp, p));
    }

    if (p256_words_from_mpi(k, scalar) != 0 ||
            p256_words_from_mpi_reduced(&p->MBEDTLS_PRIVATE(X), x_std) != 0 ||
            p256_words_from_mpi_reduced(&p->MBEDTLS_PRIVATE(Y), y_std) != 0) {
        ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
        goto cleanup;
    }

    if (p256_words_bitlen(scalar) == 0) {
        p256_fast_point_set_zero(r);
        ret = 0;
        goto cleanup;
    }

    scratch = os_malloc(sizeof(*scratch));
    if (!scratch) {
        ret = MBEDTLS_ERR_MPI_ALLOC_FAILED;
        goto cleanup;
    }

    p256_fast_to_mont(x_mont, x_std);
    p256_fast_to_mont(y_mont, y_std);
    p256_fast_to_mont(one_mont, one_std);
    os_memcpy(scratch->table_x[1], x_mont, sizeof(scratch->table_x[1]));
    os_memcpy(scratch->table_y[1], y_mont, sizeof(scratch->table_y[1]));

    p256_fast_point_from_affine(&scratch->precomp[0], x_mont, y_mont, one_mont);
    os_memcpy(&scratch->precomp[1], &scratch->precomp[0], sizeof(scratch->precomp[1]));
    p256_fast_point_double(&scratch->precomp[1]);
    for (window = 2; window < P256_WINDOW_PRECOMP_COUNT; window++) {
        os_memcpy(&scratch->precomp[window], &scratch->precomp[window - 1],
                  sizeof(scratch->precomp[window]));
        p256_fast_point_add_mixed(&scratch->precomp[window], x_mont, y_mont,
                                  one_mont);
    }

    if (p256_fast_points_batch_to_affine_mont(&scratch->precomp[1],
                                              P256_WINDOW_BATCH_COUNT,
                                              &scratch->table_x[2],
                                              &scratch->table_y[2]) != 0) {
        ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
        goto cleanup;
    }

    p256_fast_point_set_zero(r);

    for (window = P256_SCALAR_WINDOW_COUNT - 1; window >= 0; window--) {
        u32 idx = p256_words_get_window(scalar,
                                        (unsigned) window * P256_WINDOW_BITS,
                                        P256_WINDOW_BITS);

        if (started) {
            unsigned int dbl;

            for (dbl = 0; dbl < P256_WINDOW_BITS; dbl++) {
                p256_fast_point_double(r);
            }
        }

        if (idx == 0U) {
            continue;
        }

        if (!started) {
            p256_fast_point_from_affine(r, scratch->table_x[idx],
                                        scratch->table_y[idx], one_mont);
            started = true;
        } else {
            p256_fast_point_add_mixed(r, scratch->table_x[idx],
                                      scratch->table_y[idx], one_mont);
        }
    }

    ret = 0;

cleanup:
    forced_memzero(scalar, sizeof(scalar));
    if (scratch) {
        forced_memzero(scratch, sizeof(*scratch));
    }
    os_free(scratch);
    return ret;
}

static int crypto_ec_point_mul_p256_jacobian_fast_internal(const mbedtls_ecp_group *grp,
                                                           const mbedtls_ecp_point *p,
                                                           const mbedtls_mpi *k,
                                                           mbedtls_ecp_point *res,
                                                           bool validate_inputs)
{
    p256_fast_jac_point r;
    int ret;

    ret = crypto_ec_point_mul_p256_window4_core(grp, p, k, &r, validate_inputs);
    if (ret != 0) {
        return ret;
    }

    return p256_fast_point_normalize(grp, &r, res);
}

static int crypto_ec_point_mul_p256_generator_comb_fast(const mbedtls_ecp_group *grp,
                                                        const mbedtls_mpi *k,
                                                        mbedtls_ecp_point *res,
                                                        bool validate_inputs)
{
    p256_fast_jac_point r;
    u32 scalar[P256_WORDS];
    u32 one_mont[P256_WORDS];
    static const u32 one_std[P256_WORDS] = {1};
    int col;
    int ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;

    if (!grp || grp->id != MBEDTLS_ECP_DP_SECP256R1) {
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    if (validate_inputs) {
        MBEDTLS_MPI_CHK(mbedtls_ecp_check_privkey(grp, k));
    }

    if (p256_words_from_mpi(k, scalar) != 0) {
        ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
        goto cleanup;
    }

    if (p256_words_bitlen(scalar) == 0) {
        ret = mbedtls_ecp_set_zero(res);
        goto cleanup;
    }

    p256_fast_to_mont(one_mont, one_std);
    p256_fast_point_set_zero(&r);

    for (col = 63; col >= 0; col--) {
        u32 idx = p256_words_get_bit(scalar, (unsigned) col) |
                  (p256_words_get_bit(scalar, (unsigned)(col + 64)) << 1) |
                  (p256_words_get_bit(scalar, (unsigned)(col + 128)) << 2) |
                  (p256_words_get_bit(scalar, (unsigned)(col + 192)) << 3);

        p256_fast_point_double(&r);
        if (idx != 0U) {
            p256_fast_point_add_mixed(&r,
                                      p256_base_comb4_x[idx],
                                      p256_base_comb4_y[idx],
                                      one_mont);
        }
    }

    ret = p256_fast_point_normalize(grp, &r, res);

cleanup:
    mbedtls_platform_zeroize(scalar, sizeof(scalar));
    mbedtls_platform_zeroize(&r, sizeof(r));
    return ret;
}

static int crypto_ec_point_mul_p256_jacobian_fast(const mbedtls_ecp_group *grp,
                                                  const mbedtls_ecp_point *p,
                                                  const mbedtls_mpi *k,
                                                  mbedtls_ecp_point *res)
{
    return crypto_ec_point_mul_p256_jacobian_fast_internal(grp, p, k, res,
                                                           true);
}
#endif

static int crypto_ec_point_mul_generic(const mbedtls_ecp_group *grp,
                                       const mbedtls_ecp_point *p,
                                       const mbedtls_mpi *k,
                                       mbedtls_ecp_point *res)
{
    return mbedtls_ecp_mul((mbedtls_ecp_group *) grp, res, k, p,
                           mbedtls_esp_random, NULL);
}

static int crypto_ec_point_mul_fast(const mbedtls_ecp_group *grp,
                                    const mbedtls_ecp_point *p,
                                    const mbedtls_mpi *k,
                                    mbedtls_ecp_point *res)
{
#if CONFIG_MBEDTLS_HARDWARE_ECC
    int ret;

    ret = crypto_ec_point_mul_ecc_hw(grp, p, k, res);
    if (ret != MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE) {
        return ret;
    }
#elif ESP_WIFI_P256_SOFT_ACCEL
    if (crypto_ec_is_p256_group(grp)) {
        return crypto_ec_point_mul_p256_jacobian_fast(grp, p, k, res);
    }
#endif

    return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
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

    ret = crypto_ec_point_mul_fast((mbedtls_ecp_group *) e,
                                   (const mbedtls_ecp_point *) p,
                                   (const mbedtls_mpi *) b,
                                   (mbedtls_ecp_point *) res);
    if (ret == MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE) {
        ret = crypto_ec_point_mul_generic((mbedtls_ecp_group *) e,
                                          (const mbedtls_ecp_point *) p,
                                          (const mbedtls_mpi *) b,
                                          (mbedtls_ecp_point *) res);
    }

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
    mbedtls_ecp_group *grp = (mbedtls_ecp_group *) e;
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
    MBEDTLS_MPI_CHK(crypto_bignum_mulmod(x, x,
                                         (const struct crypto_bignum *) &grp->P,
                                         (struct crypto_bignum *) &temp));
    MBEDTLS_MPI_CHK(crypto_bignum_mulmod((const struct crypto_bignum *) &temp, x,
                                         (const struct crypto_bignum *) &grp->P,
                                         (struct crypto_bignum *) &temp));

    if (mbedtls_ecp_group_a_is_minus_3(grp)) {
        /*
         * For NIST P-curves used in SAE, a == -3. Compute (-3x + b) mod p
         * with additions/subtractions instead of a generic multiply+mod path.
         */
        MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&temp2, (const mbedtls_mpi *) x,
                                            (const mbedtls_mpi *) x));
        if (mbedtls_mpi_cmp_mpi(&temp2, &grp->P) >= 0) {
            MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(&temp2, &temp2, &grp->P));
        }

        MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&temp2, &temp2,
                                            (const mbedtls_mpi *) x));
        while (mbedtls_mpi_cmp_mpi(&temp2, &grp->P) >= 0) {
            MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(&temp2, &temp2, &grp->P));
        }

        if (mbedtls_mpi_cmp_int(&temp2, 0) != 0) {
            MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(&temp2, &grp->P, &temp2));
        }
    } else {
        MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&temp2, (const mbedtls_mpi *) x,
                                            &grp->A));
        MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&temp2, &temp2, &grp->P));
    }

    MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&temp2, &temp2, &grp->B));
    while (mbedtls_mpi_cmp_mpi(&temp2, &grp->P) >= 0) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(&temp2, &temp2, &grp->P));
    }

    MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(y_sqr, &temp2, &temp));
    while (mbedtls_mpi_cmp_mpi(y_sqr, &grp->P) >= 0) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(y_sqr, y_sqr, &grp->P));
    }

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

#if !CONFIG_MBEDTLS_HARDWARE_ECC
static int crypto_ec_point_is_on_curve_mpi(struct crypto_ec *e,
                                           const struct crypto_ec_point *p)
{
    mbedtls_mpi y_sqr_lhs, *y_sqr_rhs = NULL, two;
    int ret = 0, on_curve = 0;

    mbedtls_mpi_init(&y_sqr_lhs);
    mbedtls_mpi_init(&two);

    /* Calculate y^2  mod P */
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&two, 2));
    MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&y_sqr_lhs,
                                        &((const mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(Y),
                                        &two, &((mbedtls_ecp_group *)e)->P, NULL));

    y_sqr_rhs = (mbedtls_mpi *) crypto_ec_point_compute_y_sqr(
                    e, (const struct crypto_bignum *)
                    & ((const mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(X));

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
#endif

int crypto_ec_point_is_on_curve(struct crypto_ec *e,
                                const struct crypto_ec_point *p)
{
#if CONFIG_MBEDTLS_HARDWARE_ECC
    /* ECC HW verify path via mbedTLS alt hooks. */
    return mbedtls_ecp_check_pubkey((const mbedtls_ecp_group *)e,
                                    (const mbedtls_ecp_point *)p) == 0;
#else
    /* MPI implementation. */
    return crypto_ec_point_is_on_curve_mpi(e, p);
#endif
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
    if (mbedtls_pk_check_pair((mbedtls_pk_context *)key1, (mbedtls_pk_context *)key2, mbedtls_esp_random, NULL) < 0) {
        return 0;
    }
    return 1;
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
    mbedtls_pk_context *key = os_malloc(sizeof(*key));

    if (!key) {
        wpa_printf(MSG_ERROR, "%s: memory allocation failed", __func__);
        return NULL;
    }
    mbedtls_pk_init(key);

    return (struct crypto_ec_key *)key;
}

struct crypto_ec_key * crypto_ec_key_set_pub(const struct crypto_ec_group *group,
                                             const u8 *buf, size_t len)
{
    mbedtls_ecp_point *point = NULL;
    struct crypto_ec_key *pkey = NULL;
    int ret;
    mbedtls_pk_context *key = (mbedtls_pk_context *)crypto_alloc_key();
    mbedtls_ecp_group *ecp_grp = (mbedtls_ecp_group *)group;

    if (!key) {
        wpa_printf(MSG_ERROR, "%s: memory allocation failed", __func__);
        return NULL;
    }

    point = (mbedtls_ecp_point *)crypto_ec_point_from_bin((struct crypto_ec *)group, buf);
    if (!point) {
        wpa_printf(MSG_ERROR, "%s: Point initialization failed", __func__);
        goto fail;
    }
    if (crypto_ec_point_is_at_infinity((struct crypto_ec *)group, (struct crypto_ec_point *)point)) {
        wpa_printf(MSG_ERROR, "Point is at infinity");
        goto fail;
    }
    if (!crypto_ec_point_is_on_curve((struct crypto_ec *)group, (struct crypto_ec_point *)point)) {
        wpa_printf(MSG_ERROR, "Point not on curve");
        goto fail;
    }

    if (mbedtls_ecp_check_pubkey(ecp_grp, point) < 0) {
        // ideally should have failed in upper condition, duplicate code??
        wpa_printf(MSG_ERROR, "Invalid key");
        goto fail;
    }
    /* Assign values */
    if ((ret = mbedtls_pk_setup(key,
                                mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY))) != 0) {
        goto fail;
    }

    mbedtls_ecp_copy(&mbedtls_pk_ec(*key)->MBEDTLS_PRIVATE(Q), point);
    mbedtls_ecp_group_load(&mbedtls_pk_ec(*key)->MBEDTLS_PRIVATE(grp), ecp_grp->id);

    pkey = (struct crypto_ec_key *)key;
    crypto_ec_point_deinit((struct crypto_ec_point *)point, 0);
    return pkey;
fail:
    if (point) {
        crypto_ec_point_deinit((struct crypto_ec_point *)point, 0);
    }
    if (key) {
        mbedtls_pk_free(key);
    }
    pkey = NULL;
    return pkey;
}

struct crypto_ec_point *crypto_ec_key_get_public_key(struct crypto_ec_key *key)
{
    mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;

    return (struct crypto_ec_point *)&mbedtls_pk_ec(*pkey)->MBEDTLS_PRIVATE(Q);
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
    mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;

    return (struct crypto_ec_group *) & (mbedtls_pk_ec(*pkey)->MBEDTLS_PRIVATE(grp));
}

int crypto_ec_key_group(struct crypto_ec_key *key)
{
    mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;

    int iana_group = (int)crypto_ec_get_mbedtls_to_nist_group_id(mbedtls_pk_ec(*pkey)->MBEDTLS_PRIVATE(grp).id);
    return iana_group;
}

struct crypto_bignum *crypto_ec_key_get_private_key(struct crypto_ec_key *key)
{
    mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;

    return ((struct crypto_bignum *) & (mbedtls_pk_ec(*pkey)->MBEDTLS_PRIVATE(d)));
}

int crypto_ec_get_publickey_buf(struct crypto_ec_key *key, u8 *key_buf, int len)
{
    mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;
    mbedtls_ecp_keypair *ec = NULL;
    size_t pk_len = 0;

    if (!pkey || !mbedtls_pk_can_do(pkey, MBEDTLS_PK_ECKEY)) {
        return -1;
    }

    ec = mbedtls_pk_ec(*pkey);
    if (!ec) {
        return -1;
    }

    pk_len = 1 + (2 * mbedtls_mpi_size(&ec->MBEDTLS_PRIVATE(grp).P));
    if (len == 0) {
        return (int) pk_len;
    }

    if (len < 0 || (size_t) len < pk_len) {
        return -1;
    }

    if (mbedtls_ecp_point_write_binary(&ec->MBEDTLS_PRIVATE(grp),
                                       &ec->MBEDTLS_PRIVATE(Q),
                                       MBEDTLS_ECP_PF_UNCOMPRESSED,
                                       &pk_len, key_buf, len) != 0) {
        return -1;
    }

    return (int) pk_len;
}

int crypto_write_pubkey_der(struct crypto_ec_key *key, unsigned char **key_buf)
{
    unsigned char *buf = os_malloc(ECP_PUB_DER_MAX_BYTES);

    if (!buf) {
        wpa_printf(MSG_ERROR, "memory allocation failed");
        return -1;
    }
    int len = mbedtls_pk_write_pubkey_der((mbedtls_pk_context *)key, buf, ECP_PUB_DER_MAX_BYTES);
    if (len <= 0) {
        os_free(buf);
        return -1;
    }

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
    int ret;
    mbedtls_pk_context *kctx = (mbedtls_pk_context *)crypto_alloc_key();

    if (!kctx) {
        wpa_printf(MSG_ERROR, "memory allocation failed");
        return NULL;
    }
    ret = mbedtls_pk_parse_key(kctx, privkey, privkey_len, NULL, 0, mbedtls_esp_random, NULL);

    if (ret < 0) {
        //crypto_print_error_string(ret);
        goto fail;
    }

    return (struct crypto_ec_key *)kctx;

fail:
    mbedtls_pk_free(kctx);
    os_free(kctx);
    return NULL;
}

unsigned int crypto_ec_get_mbedtls_to_nist_group_id(int id)
{
    return crypto_ec_get_mbedtls_to_iana_group_id((mbedtls_ecp_group_id) id);
}

int crypto_ec_get_curve_id(const struct crypto_ec_group *group)
{
    mbedtls_ecp_group *grp = (mbedtls_ecp_group *)group;
    return (crypto_ec_get_mbedtls_to_nist_group_id(grp->id));
}

static int crypto_ecdh_fast_p256(struct crypto_ec_key *key_own,
                                 struct crypto_ec_key *key_peer,
                                 u8 *secret, size_t *secret_len)
{
    mbedtls_pk_context *own = (mbedtls_pk_context *)key_own;
    mbedtls_pk_context *peer = (mbedtls_pk_context *)key_peer;
    mbedtls_ecp_keypair *own_ec;
    mbedtls_ecp_keypair *peer_ec;
    mbedtls_ecp_point shared_point;
    size_t coord_len;
    int ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;

    if (!own || !peer || !mbedtls_pk_can_do(own, MBEDTLS_PK_ECKEY) ||
            !mbedtls_pk_can_do(peer, MBEDTLS_PK_ECKEY)) {
        return ret;
    }

    own_ec = mbedtls_pk_ec(*own);
    peer_ec = mbedtls_pk_ec(*peer);
    if (!own_ec || !peer_ec ||
            !crypto_ec_is_p256_group(&own_ec->MBEDTLS_PRIVATE(grp)) ||
            !crypto_ec_is_p256_group(&peer_ec->MBEDTLS_PRIVATE(grp))) {
        return ret;
    }

    ret = mbedtls_ecp_check_privkey(&own_ec->MBEDTLS_PRIVATE(grp),
                                    &own_ec->MBEDTLS_PRIVATE(d));
    if (ret != 0) {
        return ret;
    }

    ret = mbedtls_ecp_check_pubkey(&peer_ec->MBEDTLS_PRIVATE(grp),
                                   &peer_ec->MBEDTLS_PRIVATE(Q));
    if (ret != 0) {
        return ret;
    }

    coord_len = mbedtls_mpi_size(&own_ec->MBEDTLS_PRIVATE(grp).P);
    mbedtls_ecp_point_init(&shared_point);

    ret = crypto_ec_point_mul_fast(&own_ec->MBEDTLS_PRIVATE(grp),
                                   &peer_ec->MBEDTLS_PRIVATE(Q),
                                   &own_ec->MBEDTLS_PRIVATE(d),
                                   &shared_point);
    if (ret == 0 && !mbedtls_ecp_is_zero(&shared_point) &&
            mbedtls_mpi_write_binary(&shared_point.MBEDTLS_PRIVATE(X),
                                     secret, coord_len) == 0) {
        *secret_len = coord_len;
    } else if (ret == 0) {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    mbedtls_ecp_point_free(&shared_point);
    return ret;
}

static int crypto_ecdh_generic(struct crypto_ec_key *key_own,
                               struct crypto_ec_key *key_peer,
                               u8 *secret, size_t *secret_len)
{
    mbedtls_pk_context *own = (mbedtls_pk_context *)key_own;
    mbedtls_pk_context *peer = (mbedtls_pk_context *)key_peer;
    mbedtls_ecdh_context *ctx = NULL;
    int ret = -1;

    *secret_len = 0;
    ctx = os_malloc(sizeof(*ctx));
    if (!ctx) {
        wpa_printf(MSG_ERROR, "DPP: EVP_PKEY_CTX_new failed: %s",
                   __func__);
        goto fail;
    }

    mbedtls_ecdh_init(ctx);

    if (mbedtls_ecdh_get_params(ctx, mbedtls_pk_ec(*own),
                                MBEDTLS_ECDH_OURS) < 0) {
        wpa_printf(MSG_ERROR, "failed to set our ecdh params");
        goto fail;
    }

#ifndef DPP_MAX_SHARED_SECRET_LEN
#define DPP_MAX_SHARED_SECRET_LEN 66
#endif
    if (mbedtls_ecdh_get_params(ctx, mbedtls_pk_ec(*peer),
                                MBEDTLS_ECDH_THEIRS) < 0) {
        wpa_printf(MSG_ERROR, "failed to set peer's ecdh params");
        goto fail;
    }

    if (mbedtls_ecdh_calc_secret(ctx, secret_len, secret,
                                 DPP_MAX_SHARED_SECRET_LEN,
                                 mbedtls_esp_random, NULL) < 0) {
        wpa_printf(MSG_ERROR, "failed to calculate secret");
        goto fail;
    }

    if (*secret_len > DPP_MAX_SHARED_SECRET_LEN) {
        wpa_printf(MSG_ERROR, "secret len=%d is too big", *secret_len);
        goto fail;
    }

    ret = 0;

fail:
    if (ctx) {
        mbedtls_ecdh_free(ctx);
        os_free(ctx);
    }
    return ret;
}

int crypto_ecdh(struct crypto_ec_key *key_own, struct crypto_ec_key *key_peer,
                u8 *secret, size_t *secret_len)
{
    int ret = crypto_ecdh_fast_p256(key_own, key_peer, secret, secret_len);

    if (ret == 0) {
        return 0;
    }

    return crypto_ecdh_generic(key_own, key_peer, secret, secret_len);
}

int crypto_ecdsa_get_sign(unsigned char *hash,
                          const struct crypto_bignum *r, const struct crypto_bignum *s, struct crypto_ec_key *csign, int hash_len)
{
    int ret = -1;
    mbedtls_pk_context *pkey = (mbedtls_pk_context *)csign;

    mbedtls_ecdsa_context *ctx = os_malloc(sizeof(*ctx));
    if (!ctx) {
        wpa_printf(MSG_ERROR, "failed to allcate memory");
        return -1;
    }
    mbedtls_ecdsa_init(ctx);

    if (mbedtls_ecdsa_from_keypair(ctx, mbedtls_pk_ec(*pkey)) < 0) {
        goto fail;
    }
    ret = mbedtls_ecdsa_sign(&ctx->MBEDTLS_PRIVATE(grp), (mbedtls_mpi *)r, (mbedtls_mpi *)s,
                             &ctx->MBEDTLS_PRIVATE(d), hash, SHA256_MAC_LEN, mbedtls_esp_random, NULL);

fail:
    mbedtls_ecdsa_free(ctx);
    os_free(ctx);

    return  ret;
}

int crypto_ec_key_verify_signature_r_s(struct crypto_ec_key *csign,
                                       const unsigned char *hash, int hlen,
                                       const u8 *r, size_t r_len,
                                       const u8 *s, size_t s_len)
{
    /* (mbedtls_ecdsa_context *) */
    mbedtls_ecp_keypair *ecp_kp = mbedtls_pk_ec(*(mbedtls_pk_context *)csign);
    int ret;
    mbedtls_ecp_group *ecp_kp_grp;
    mbedtls_ecp_point *ecp_kp_q;
    struct crypto_bignum *rb = NULL, *sb = NULL;

    if (!ecp_kp) {
        return -1;
    }

    rb = crypto_bignum_init_set(r, r_len);
    sb = crypto_bignum_init_set(s, s_len);
    if (!rb || !sb) {
        crypto_bignum_deinit(rb, 0);
        crypto_bignum_deinit(sb, 0);
        return -1;
    }

    ecp_kp_grp = &ecp_kp->MBEDTLS_PRIVATE(grp);
    ecp_kp_q = &ecp_kp->MBEDTLS_PRIVATE(Q);
    ret = mbedtls_ecdsa_verify(ecp_kp_grp, hash, hlen,
                               ecp_kp_q, (mbedtls_mpi *)rb, (mbedtls_mpi *)sb);
    crypto_bignum_deinit(rb, 0);
    crypto_bignum_deinit(sb, 0);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "ecdsa verification failed");
        return ret;
    }

    return ret;
}

void crypto_ec_key_debug_print(struct crypto_ec_key *key, const char *title)
{
#if defined(CONFIG_LOG_DEFAULT_LEVEL_DEBUG) || defined(CONFIG_LOG_DEFAULT_LEVEL_VERBOSE)
#if defined(DEBUG_PRINT)
    mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;
    mbedtls_ecp_keypair *ecp = mbedtls_pk_ec(*pkey);
    u8 x[32], y[32], d[32];
    wpa_printf(MSG_EXCESSIVE, "curve: %s",
               mbedtls_ecp_curve_info_from_grp_id(ecp->MBEDTLS_PRIVATE(grp).id)->name);
    int len = mbedtls_mpi_size((mbedtls_mpi *)crypto_ec_get_prime((struct crypto_ec *)crypto_ec_get_group_from_key(key)));

    wpa_printf(MSG_EXCESSIVE, "prime len is %d", len);
    crypto_ec_point_to_bin((struct crypto_ec *)crypto_ec_get_group_from_key(key), crypto_ec_key_get_public_key(key), x, y);
    crypto_bignum_to_bin(crypto_ec_key_get_private_key(key),
                         d, len, len);
    wpa_hexdump(MSG_EXCESSIVE, "Q_x:", x, 32);
    wpa_hexdump(MSG_EXCESSIVE, "Q_y:", y, 32);
    wpa_hexdump(MSG_EXCESSIVE, "d:     ",  d, 32);
#endif
#endif
}

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

int crypto_is_ec_key(struct crypto_ec_key *key)
{
    int ret = mbedtls_pk_can_do((mbedtls_pk_context *)key, MBEDTLS_PK_ECKEY);
    return  ret;
}

static struct crypto_ec_key *crypto_ec_key_gen_fast_p256(void)
{
    mbedtls_pk_context *kctx = (mbedtls_pk_context *)crypto_alloc_key();
    int ret;

    if (!kctx) {
        wpa_printf(MSG_ERROR, "%s: memory allocation failed", __func__);
        return NULL;
    }

    if (mbedtls_pk_setup(kctx,
                         mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY)) != 0) {
        goto fail;
    }

    if ((ret = mbedtls_ecp_group_load(&mbedtls_pk_ec(*kctx)->MBEDTLS_PRIVATE(grp),
                                      MBEDTLS_ECP_DP_SECP256R1)) != 0) {
        goto fail;
    }

    ret = mbedtls_ecp_gen_privkey(&mbedtls_pk_ec(*kctx)->MBEDTLS_PRIVATE(grp),
                                  &mbedtls_pk_ec(*kctx)->MBEDTLS_PRIVATE(d),
                                  mbedtls_esp_random, NULL);
    if (ret != 0) {
        goto fail;
    }

#if ESP_WIFI_P256_SOFT_ACCEL_PREFERRED
    ret = crypto_ec_point_mul_p256_generator_comb_fast(
              &mbedtls_pk_ec(*kctx)->MBEDTLS_PRIVATE(grp),
              &mbedtls_pk_ec(*kctx)->MBEDTLS_PRIVATE(d),
              &mbedtls_pk_ec(*kctx)->MBEDTLS_PRIVATE(Q), false);
#else
    ret = crypto_ec_point_mul_fast(&mbedtls_pk_ec(*kctx)->MBEDTLS_PRIVATE(grp),
                                   &mbedtls_pk_ec(*kctx)->MBEDTLS_PRIVATE(grp).G,
                                   &mbedtls_pk_ec(*kctx)->MBEDTLS_PRIVATE(d),
                                   &mbedtls_pk_ec(*kctx)->MBEDTLS_PRIVATE(Q));
#endif
    if (ret != 0) {
        goto fail;
    }

    return (struct crypto_ec_key *)kctx;
fail:
    mbedtls_pk_free(kctx);
    os_free(kctx);
    return NULL;
}

static struct crypto_ec_key *crypto_ec_key_gen_generic(u16 ike_group)
{
    mbedtls_ecp_group_id grp_id = crypto_ec_get_iana_to_mbedtls_group_id(ike_group);
    mbedtls_pk_context *kctx = (mbedtls_pk_context *)crypto_alloc_key();

    if (grp_id == MBEDTLS_ECP_DP_NONE || !kctx) {
        os_free(kctx);
        return NULL;
    }

    if (mbedtls_pk_setup(kctx,
                         mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY)) != 0) {
        goto fail;
    }

    if (mbedtls_ecp_gen_key(grp_id, mbedtls_pk_ec(*kctx),
                            mbedtls_esp_random, NULL) != 0) {
        goto fail;
    }

    return (struct crypto_ec_key *) kctx;

fail:
    mbedtls_pk_free(kctx);
    os_free(kctx);
    return NULL;
}

struct crypto_ec_key * crypto_ec_key_gen(u16 ike_group)
{
    if (ike_group == IANA_SECP256R1) {
        struct crypto_ec_key *key = crypto_ec_key_gen_fast_p256();

        if (key) {
            return key;
        }
    }

    return crypto_ec_key_gen_generic(ike_group);
}

/*
 * ECParameters ::= CHOICE {
 *   namedCurve         OBJECT IDENTIFIER
 * }
 */
static int pk_write_ec_param(unsigned char **p, unsigned char *start,
                             mbedtls_ecp_keypair *ec)
{
    int ret;
    size_t len = 0;
    const char *oid;
    size_t oid_len;

    if ((ret = mbedtls_oid_get_oid_by_ec_grp(ec->MBEDTLS_PRIVATE(grp).id, &oid, &oid_len)) != 0) {
        return (ret);
    }

    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_oid(p, start, oid, oid_len));

    return ((int) len);
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
    size_t len = 0, par_len = 0, oid_len;
    const char *oid;

    if (size == 0) {
        return (MBEDTLS_ERR_ASN1_BUF_TOO_SMALL);
    }

    c = buf + size;

    ret = mbedtls_pk_write_pubkey_formatted(&c, buf, key, format);

    if (ret < 0) {
        return ret;
    }
    MBEDTLS_ASN1_CHK_ADD(len, ret);

    if (c - buf < 1) {
        return (MBEDTLS_ERR_ASN1_BUF_TOO_SMALL);
    }

    /*
     *  SubjectPublicKeyInfo  ::=  SEQUENCE  {
     *       algorithm            AlgorithmIdentifier,
     *       subjectPublicKey     BIT STRING }
     */
    *--c = 0;
    len += 1;

    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_len(&c, buf, len));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_tag(&c, buf, MBEDTLS_ASN1_BIT_STRING));

    if ((ret = mbedtls_oid_get_oid_by_pk_alg(mbedtls_pk_get_type(key),
                                             &oid, &oid_len)) != 0) {
        return (ret);
    }

    if (mbedtls_pk_get_type(key) == MBEDTLS_PK_ECKEY) {
        MBEDTLS_ASN1_CHK_ADD(par_len, pk_write_ec_param(&c, buf, mbedtls_pk_ec(*key)));
    }

    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_algorithm_identifier(&c, buf, oid, oid_len,
                                                                      par_len));

    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_len(&c, buf, len));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_tag(&c, buf, MBEDTLS_ASN1_CONSTRUCTED |
                                                     MBEDTLS_ASN1_SEQUENCE));

    return ((int) len);
}

int crypto_ec_write_pub_key(struct crypto_ec_key *key, unsigned char **key_buf)
{
    unsigned char output_buf[1600] = {0};
    int len = crypto_pk_write_formatted_pubkey_der((mbedtls_pk_context *)key, output_buf, 1600, 1);
    if (len <= 0) {
        return 0;
    }

    *key_buf = os_malloc(len);
    if (!*key_buf) {
        wpa_printf(MSG_ERROR, "%s: memory allocation failed", __func__);
        return 0;
    }
    os_memcpy(*key_buf, output_buf + 1600 - len, len);

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
    }
    ret = wpabuf_alloc_copy(der, der_len);

    os_free(der);
    return ret;
}

int crypto_mbedtls_get_grp_id(int group)
{
    return (int) crypto_ec_get_iana_to_mbedtls_group_id(group);
}

void crypto_ecdh_deinit(struct crypto_ecdh *ecdh)
{
    mbedtls_ecdh_context *ctx = (mbedtls_ecdh_context *)ecdh;
    if (!ctx) {
        return;
    }
    mbedtls_ecdh_free(ctx);
    os_free(ctx);
    ctx = NULL;
}

struct crypto_ecdh * crypto_ecdh_init(int group)
{
    mbedtls_ecdh_context *ctx;

    ctx = os_zalloc(sizeof(*ctx));
    if (!ctx) {
        wpa_printf(MSG_ERROR, "Memory allocation failed for ecdh context");
        goto fail;
    }
    mbedtls_ecdh_init(ctx);
#ifndef CONFIG_MBEDTLS_ECDH_LEGACY_CONTEXT
    ctx->MBEDTLS_PRIVATE(var) = MBEDTLS_ECDH_VARIANT_MBEDTLS_2_0;
#endif

    if ((mbedtls_ecp_group_load(ACCESS_ECDH(&ctx, grp), crypto_mbedtls_get_grp_id(group))) != 0) {
        wpa_printf(MSG_ERROR, "Failed to set up ECDH context with group info");
        goto fail;
    }

    /* Generates ECDH keypair on elliptic curve */
    if (mbedtls_ecdh_gen_public(ACCESS_ECDH(&ctx, grp), ACCESS_ECDH(&ctx, d), ACCESS_ECDH(&ctx, Q), mbedtls_esp_random, NULL) != 0) {
        wpa_printf(MSG_ERROR, "ECDH keypair on curve failed");
        goto fail;
    }

    return (struct crypto_ecdh *)ctx;
fail:
    if (ctx) {
        mbedtls_ecdh_free(ctx);
        os_free(ctx);
        ctx = NULL;
    }
    return NULL;
}

struct wpabuf * crypto_ecdh_get_pubkey(struct crypto_ecdh *ecdh, int y)
{
    struct wpabuf *public_key = NULL;
    uint8_t *buf = NULL;
    int ret;
    mbedtls_ecdh_context *ctx = (mbedtls_ecdh_context *)ecdh;
    size_t prime_len = ACCESS_ECDH(ctx, grp).pbits / 8;

    buf = os_zalloc(y ? prime_len : 2 * prime_len);
    if (!buf) {
        wpa_printf(MSG_ERROR, "Memory allocation failed");
        return NULL;
    }

    /* Export an MPI into unsigned big endian binary data of fixed size */
    ret = mbedtls_mpi_write_binary(ACCESS_ECDH(&ctx, Q).MBEDTLS_PRIVATE(X), buf, prime_len);
    if (ret) {
        goto cleanup;
    }
    public_key = wpabuf_alloc_copy(buf, 32);

cleanup:
    os_free(buf);
    return public_key;
}

struct wpabuf * crypto_ecdh_set_peerkey(struct crypto_ecdh *ecdh, int inc_y,
                                        const u8 *key, size_t len)
{
    uint8_t *secret = 0;
    size_t olen = 0, len_prime = 0;
    struct crypto_bignum *bn_x = NULL;
    struct crypto_ec_point *ec_pt = NULL;
    uint8_t *px = NULL, *py = NULL, *buf = NULL;
    struct crypto_ec_key *pkey = NULL;
    struct wpabuf *sh_secret = NULL;
    int secret_key = 0;

    mbedtls_ecdh_context *ctx = (mbedtls_ecdh_context *)ecdh;
    if (!ctx) {
        wpa_printf(MSG_ERROR, "ECDH Context is NULL");
        return 0;
    }

    len_prime = ACCESS_ECDH(ctx, grp).pbits / 8;
    bn_x = crypto_bignum_init_set(key, len);

    /* Initialize data for EC point */
    ec_pt = crypto_ec_point_init((struct crypto_ec*)ACCESS_ECDH(&ctx, grp));
    if (!ec_pt) {
        wpa_printf(MSG_ERROR, "Initializing for EC point failed");
        goto cleanup;
    }

    if (crypto_ec_point_solve_y_coord((struct crypto_ec *)ACCESS_ECDH(&ctx, grp), ec_pt, bn_x, inc_y) != 0) {
        wpa_printf(MSG_ERROR, "Failed to solve for y coordinate");
        goto cleanup;
    }
    px = os_zalloc(len);
    py = os_zalloc(len);
    buf = os_zalloc(2 * len);

    if (!px || !py || !buf) {
        wpa_printf(MSG_ERROR, "Memory allocation failed");
        goto cleanup;
    }
    if (crypto_ec_point_to_bin((struct crypto_ec *)ACCESS_ECDH(&ctx, grp), ec_pt, px, py) != 0) {
        wpa_printf(MSG_ERROR, "Failed to write EC point value as binary data");
        goto cleanup;
    }

    os_memcpy(buf, px, len);
    os_memcpy(buf + len, py, len);

    pkey = crypto_ec_key_set_pub((struct crypto_ec_group*)ACCESS_ECDH(&ctx, grp), buf, len);
    if (!pkey) {
        wpa_printf(MSG_ERROR, "Failed to set point for peer's public key");
        goto cleanup;
    }

    mbedtls_pk_context *peer = (mbedtls_pk_context*)pkey;

    /* Setup ECDH context from EC key */
    /* Call to mbedtls_ecdh_get_params() will initialize the context when not LEGACY context */
    if (peer != NULL) {
        mbedtls_ecp_copy(ACCESS_ECDH(&ctx, Qp), &(mbedtls_pk_ec(*peer))->MBEDTLS_PRIVATE(Q));
#ifndef CONFIG_MBEDTLS_ECDH_LEGACY_CONTEXT
        ctx->MBEDTLS_PRIVATE(var) = MBEDTLS_ECDH_VARIANT_MBEDTLS_2_0;
#endif
    } else {
        wpa_printf(MSG_ERROR, "Failed to set peer's ECDH context");
        goto cleanup;
    }
    int len_secret = inc_y ? 2 * len : len;
    secret = os_zalloc(len_secret);
    if (!secret) {
        wpa_printf(MSG_ERROR, "Allocation failed for secret");
        goto cleanup;
    }

    /* Calculate secret
    z = F(DH(x,Y)) */
    secret_key = mbedtls_ecdh_calc_secret(ctx, &olen, secret, len_prime, mbedtls_esp_random, NULL);
    if (secret_key != 0) {
        wpa_printf(MSG_ERROR, "Calculation of secret failed");
        goto cleanup;
    }
    sh_secret = wpabuf_alloc_copy(secret, len_secret);

cleanup:
    os_free(px);
    os_free(py);
    os_free(buf);
    os_free(secret);
    crypto_ec_key_deinit(pkey);
    crypto_bignum_deinit(bn_x, 1);
    crypto_ec_point_deinit(ec_pt, 1);
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
    mbedtls_pk_free((mbedtls_pk_context *)key);
    os_free(key);
}

int crypto_ec_key_verify_signature(struct crypto_ec_key *key, const u8 *data,
                                   size_t len, const u8 *sig, size_t sig_len)
{
    int ret = 0;

    mbedtls_ecdsa_context *ctx_verify = os_malloc(sizeof(mbedtls_ecdsa_context));
    if (!ctx_verify) {
        return -1;
    }

    mbedtls_ecdsa_init(ctx_verify);

    mbedtls_ecp_keypair *ec_key = mbedtls_pk_ec(*((mbedtls_pk_context *)key));
    mbedtls_ecp_group *grp = &ec_key->MBEDTLS_PRIVATE(grp);

    if ((ret = mbedtls_ecp_group_copy(&ctx_verify->MBEDTLS_PRIVATE(grp), grp)) != 0) {
        goto cleanup;
    }

    if ((ret = mbedtls_ecp_copy(&ctx_verify->MBEDTLS_PRIVATE(Q), &ec_key->MBEDTLS_PRIVATE(Q))) != 0) {
        goto cleanup;
    }

    if ((ret = mbedtls_ecdsa_read_signature(ctx_verify,
                                            data, len,
                                            sig, sig_len)) != 0) {
        goto cleanup;
    }
    ret = 1;

cleanup:
    mbedtls_ecdsa_free(ctx_verify);
    os_free(ctx_verify);
    return ret;
}

#endif /* CONFIG_ECC */
