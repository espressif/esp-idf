/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#define MBEDTLS_ALLOW_PRIVATE_ACCESS
#include "esp_system.h"
#include "mbedtls/bignum.h"
#include "mbedtls/esp_mbedtls_random.h"
#include "soc/soc_caps.h"

#include "utils/includes.h"
#include "utils/common.h"
#include "crypto.h"
#include "random.h"
#include "sha256.h"
#include "mbedtls/pk.h"
#include "mbedtls/psa_util.h"
#include "p256_common.h"

static int mpi_is_secp256r1_prime(const mbedtls_mpi *p)
{
    u8 p_be[P256_LEN_BYTES];

    if (!p || mbedtls_mpi_size(p) != P256_LEN_BYTES) {
        return 0;
    }

    if (mbedtls_mpi_write_binary(p, p_be, sizeof(p_be)) != 0) {
        return 0;
    }

    return os_memcmp(p_be, p256_p_be, sizeof(p_be)) == 0;
}

static size_t p256_words_ctz(const u32 *a)
{
    size_t i;

    for (i = 0; i < P256_WORDS; i++) {
        if (a[i] != 0) {
            return i * 32 + __builtin_ctz(a[i]);
        }
    }

    return P256_WORDS * 32;
}

static void p256_words_rshift(u32 *a, size_t count)
{
    size_t word_shift = count / 32;
    size_t bit_shift = count % 32;
    size_t i;

    if (word_shift >= P256_WORDS) {
        os_memset(a, 0, sizeof(u32) * P256_WORDS);
        return;
    }

    if (word_shift > 0) {
        for (i = 0; i + word_shift < P256_WORDS; i++) {
            a[i] = a[i + word_shift];
        }
        for (; i < P256_WORDS; i++) {
            a[i] = 0;
        }
    }

    if (bit_shift > 0) {
        for (i = 0; i < P256_WORDS - 1; i++) {
            a[i] = (a[i] >> bit_shift) |
                   (a[i + 1] << (32 - bit_shift));
        }
        a[P256_WORDS - 1] >>= bit_shift;
    }
}

static void p256_words_lshift(const u32 *in, size_t count, u32 *out)
{
    size_t word_shift = count / 32;
    size_t bit_shift = count % 32;
    size_t i;

    os_memset(out, 0, sizeof(u32) * P256_WORDS);

    if (word_shift >= P256_WORDS) {
        return;
    }

    for (i = 0; i < P256_WORDS; i++) {
        u64 val;
        size_t dst;

        if (in[i] == 0) {
            continue;
        }

        dst = i + word_shift;
        if (dst >= P256_WORDS) {
            break;
        }

        val = (u64) in[i] << bit_shift;
        out[dst] |= (u32) val;
        if (bit_shift > 0 && dst + 1 < P256_WORDS) {
            out[dst + 1] |= (u32)(val >> 32);
        }
    }
}

static void p256_words_sub(u32 *a, const u32 *b)
{
    size_t i;
    u64 borrow = 0;

    for (i = 0; i < P256_WORDS; i++) {
        u64 ai = a[i];
        u64 bi = b[i];
        u64 res = ai - bi - borrow;

        a[i] = (u32) res;
        borrow = (ai < bi + borrow) ? 1 : 0;
    }
}

static void p256_words_swap(u32 *a, u32 *b)
{
    u32 tmp[P256_WORDS] = {0};

    os_memcpy(tmp, a, sizeof(tmp));
    os_memcpy(a, b, sizeof(tmp));
    os_memcpy(b, tmp, sizeof(tmp));
}

static void p256_words_mod(u32 *a, const u32 *n)
{
    u32 tmp[P256_WORDS] = {0};

    while (p256_words_cmp(a, n) >= 0) {
        size_t a_bits = p256_words_bitlen(a);
        size_t n_bits = p256_words_bitlen(n);
        size_t shift = a_bits - n_bits;

        p256_words_lshift(n, shift, tmp);
        if (p256_words_cmp(a, tmp) < 0) {
            shift--;
            p256_words_lshift(n, shift, tmp);
        }
        p256_words_sub(a, tmp);
    }
}

static int crypto_bignum_mulmod_secp256r1(const mbedtls_mpi *a,
                                          const mbedtls_mpi *b,
                                          const mbedtls_mpi *mod,
                                          mbedtls_mpi *out)
{
    u32 a_words[P256_WORDS] = {0};
    u32 b_words[P256_WORDS] = {0};
    u32 b_mont[P256_WORDS] = {0};
    u32 result[P256_WORDS] = {0};

    if (!mpi_is_secp256r1_prime(mod) ||
            p256_words_from_mpi_reduced(a, a_words) != 0 ||
            p256_words_from_mpi_reduced(b, b_words) != 0) {
        return -2;
    }

    p256_mont_mul(b_mont, b_words, p256_r2_le);
    p256_mont_mul(result, a_words, b_mont);

    return p256_words_to_mpi(result, out) == 0 ? 0 : -1;
}

static int crypto_bignum_legendre_secp256r1(const mbedtls_mpi *a,
                                            const mbedtls_mpi *p)
{
    u32 A[P256_WORDS] = {0};
    u32 N[P256_WORDS] = {0};
    unsigned int n_mod8;
    unsigned int a_mod4;
    unsigned int n_mod4;
    size_t two_power;
    int sign = 1;

    if (!mpi_is_secp256r1_prime(p) ||
            p256_words_from_mpi_reduced(a, A) != 0) {
        return -2;
    }

    os_memcpy(N, p256_p_le, sizeof(N));

    if (p256_words_is_zero(A)) {
        return 0;
    }

    while (!p256_words_is_zero(A)) {
        if (p256_words_is_one(A)) {
            return sign;
        }

        n_mod8 = N[0] & 0x7;
        two_power = p256_words_ctz(A);
        if (two_power > 0) {
            p256_words_rshift(A, two_power);
            if ((n_mod8 == 3 || n_mod8 == 5) && (two_power & 1U)) {
                sign = -sign;
            }
        }

        p256_words_swap(A, N);

        a_mod4 = A[0] & 0x3;
        n_mod4 = N[0] & 0x3;
        if (a_mod4 == 3 && n_mod4 == 3) {
            sign = -sign;
        }

        if (p256_words_cmp(A, N) >= 0) {
            p256_words_mod(A, N);
        }

        if (p256_words_is_one(N)) {
            return sign;
        }
    }

    return p256_words_is_one(N) ? sign : 0;
}

/*
 * Compute Legendre symbol through Jacobi algorithm for odd prime modulus.
 * This avoids an expensive modular exponentiation in the SAE hot loop.
 */
static int crypto_bignum_legendre_jacobi(const mbedtls_mpi *a,
                                         const mbedtls_mpi *p)
{
    mbedtls_mpi A, N;
    unsigned int n_mod8, a_mod4, n_mod4;
    size_t two_power;
    int ret = 0;
    int sign = 1;
    int res = -2;

    if (mbedtls_mpi_cmp_int(p, 3) < 0 || mbedtls_mpi_get_bit(p, 0) == 0) {
        return -2;
    }

    mbedtls_mpi_init(&A);
    mbedtls_mpi_init(&N);

    MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&N, p));
    if (mbedtls_mpi_cmp_int(a, 0) >= 0 && mbedtls_mpi_cmp_mpi(a, &N) < 0) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&A, a));
    } else {
        MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&A, a, &N));
    }

    if (mbedtls_mpi_cmp_int(&A, 0) == 0) {
        res = 0;
        goto cleanup;
    }

    while (mbedtls_mpi_cmp_int(&A, 0) != 0) {
        if (mbedtls_mpi_cmp_int(&A, 1) == 0) {
            res = sign;
            goto cleanup;
        }

        n_mod8 = (mbedtls_mpi_get_bit(&N, 0) << 0) |
                 (mbedtls_mpi_get_bit(&N, 1) << 1) |
                 (mbedtls_mpi_get_bit(&N, 2) << 2);
        two_power = mbedtls_mpi_lsb(&A);
        if (two_power > 0) {
            MBEDTLS_MPI_CHK(mbedtls_mpi_shift_r(&A, two_power));
            if ((n_mod8 == 3 || n_mod8 == 5) && (two_power & 1U)) {
                sign = -sign;
            }
        }

        mbedtls_mpi_swap(&A, &N);

        a_mod4 = (mbedtls_mpi_get_bit(&A, 0) << 0) |
                 (mbedtls_mpi_get_bit(&A, 1) << 1);
        n_mod4 = (mbedtls_mpi_get_bit(&N, 0) << 0) |
                 (mbedtls_mpi_get_bit(&N, 1) << 1);
        if (a_mod4 == 3 && n_mod4 == 3) {
            sign = -sign;
        }

        if (mbedtls_mpi_cmp_mpi(&A, &N) >= 0) {
            size_t a_bits = mbedtls_mpi_bitlen(&A);
            size_t n_bits = mbedtls_mpi_bitlen(&N);
            if (a_bits <= n_bits + 4) {
                while (mbedtls_mpi_cmp_mpi(&A, &N) >= 0) {
                    MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(&A, &A, &N));
                }
            } else {
                MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&A, &A, &N));
            }
        }

        if (mbedtls_mpi_cmp_int(&N, 1) == 0) {
            res = sign;
            goto cleanup;
        }
    }

    res = (mbedtls_mpi_cmp_int(&N, 1) == 0) ? sign : 0;

cleanup:
    mbedtls_mpi_free(&A);
    mbedtls_mpi_free(&N);

    if (ret != 0) {
        return -2;
    }
    return res;
}

struct crypto_bignum *crypto_bignum_init(void)
{
    mbedtls_mpi *bn = os_zalloc(sizeof(mbedtls_mpi));
    if (bn == NULL) {
        return NULL;
    }

    mbedtls_mpi_init(bn);

    return (struct crypto_bignum *)bn;
}

struct crypto_bignum *crypto_bignum_init_set(const u8 *buf, size_t len)
{
    int ret = 0;
    mbedtls_mpi *bn = os_zalloc(sizeof(mbedtls_mpi));
    if (bn == NULL) {
        return NULL;
    }

    mbedtls_mpi_init(bn);
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(bn, buf, len));
    return (struct crypto_bignum *) bn;

cleanup:
    mbedtls_mpi_free(bn);
    os_free(bn);
    return NULL;
}

struct crypto_bignum * crypto_bignum_init_uint(unsigned int val)
{
    int ret;

    mbedtls_mpi *bn = os_zalloc(sizeof(mbedtls_mpi));
    if (!bn) {
        return NULL;
    }

    mbedtls_mpi_init(bn);
    ret = mbedtls_mpi_lset(bn, val);

    if (ret) {
        crypto_bignum_deinit((struct crypto_bignum *)bn, 0);
        bn = NULL;
    }
    return (struct crypto_bignum *)bn;
}

void crypto_bignum_deinit(struct crypto_bignum *n, int clear)
{
    mbedtls_mpi_free((mbedtls_mpi *)n);
    os_free((mbedtls_mpi *)n);
}

int crypto_bignum_to_bin(const struct crypto_bignum *a,
                         u8 *buf, size_t buflen, size_t padlen)
{
    int num_bytes, offset;
    int ret;

    if (padlen > buflen) {
        return -1;
    }

    num_bytes = mbedtls_mpi_size((mbedtls_mpi *) a);

    if ((size_t) num_bytes > buflen) {
        return -1;
    }
    if (padlen > (size_t) num_bytes) {
        offset = padlen - num_bytes;
    } else {
        offset = 0;
    }

    os_memset(buf, 0, offset);
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary((mbedtls_mpi *) a, buf + offset, mbedtls_mpi_size((mbedtls_mpi *)a)));

    return num_bytes + offset;
cleanup:
    return ret;
}

int crypto_bignum_add(const struct crypto_bignum *a,
                      const struct crypto_bignum *b,
                      struct crypto_bignum *c)
{
    return mbedtls_mpi_add_mpi((mbedtls_mpi *) c, (const mbedtls_mpi *) a, (const mbedtls_mpi *) b) ?
           -1 : 0;
}

int crypto_bignum_mod(const struct crypto_bignum *a,
                      const struct crypto_bignum *b,
                      struct crypto_bignum *c)
{
    return mbedtls_mpi_mod_mpi((mbedtls_mpi *) c, (const mbedtls_mpi *) a, (const mbedtls_mpi *) b) ? -1 : 0;
}

int crypto_bignum_exptmod(const struct crypto_bignum *a,
                          const struct crypto_bignum *b,
                          const struct crypto_bignum *c,
                          struct crypto_bignum *d)
{
    int ret;

    /* Fast path for small public exponents frequently used in SAE math. */
    if (mbedtls_mpi_cmp_int((const mbedtls_mpi *) b, 0) >= 0 &&
            mbedtls_mpi_cmp_int((const mbedtls_mpi *) b, 3) <= 0) {
        if (mbedtls_mpi_cmp_int((const mbedtls_mpi *) b, 0) == 0) {
            ret = mbedtls_mpi_lset((mbedtls_mpi *) d, 1) ||
                  mbedtls_mpi_mod_mpi((mbedtls_mpi *) d, (mbedtls_mpi *) d,
                                      (const mbedtls_mpi *) c);
            return ret ? -1 : 0;
        }

        if (mbedtls_mpi_cmp_int((const mbedtls_mpi *) b, 1) == 0) {
            ret = mbedtls_mpi_copy((mbedtls_mpi *) d, (const mbedtls_mpi *) a) ||
                  mbedtls_mpi_mod_mpi((mbedtls_mpi *) d, (mbedtls_mpi *) d,
                                      (const mbedtls_mpi *) c);
            return ret ? -1 : 0;
        }

        if (mbedtls_mpi_cmp_int((const mbedtls_mpi *) b, 2) == 0) {
            return crypto_bignum_mulmod(a, a, c, d);
        } else {
            mbedtls_mpi tmp;
            mbedtls_mpi_init(&tmp);
            ret = crypto_bignum_mulmod(a, a, c, (struct crypto_bignum *) &tmp);
            if (ret == 0) {
                ret = crypto_bignum_mulmod((struct crypto_bignum *) &tmp,
                                           a, c, d);
            }
            mbedtls_mpi_free(&tmp);
            return ret ? -1 : 0;
        }
    }

    return mbedtls_mpi_exp_mod((mbedtls_mpi *) d, (const mbedtls_mpi *) a,
                               (const mbedtls_mpi *) b,
                               (const mbedtls_mpi *) c, NULL) ? -1 : 0;

}

int crypto_bignum_inverse(const struct crypto_bignum *a,
                          const struct crypto_bignum *b,
                          struct crypto_bignum *c)
{
    return mbedtls_mpi_inv_mod((mbedtls_mpi *) c, (const mbedtls_mpi *) a,
                               (const mbedtls_mpi *) b) ? -1 : 0;
}

int crypto_bignum_sub(const struct crypto_bignum *a,
                      const struct crypto_bignum *b,
                      struct crypto_bignum *c)
{
    return mbedtls_mpi_sub_mpi((mbedtls_mpi *) c, (const mbedtls_mpi *) a, (const mbedtls_mpi *) b) ?
           -1 : 0;
}

int crypto_bignum_div(const struct crypto_bignum *a,
                      const struct crypto_bignum *b,
                      struct crypto_bignum *c)
{
    return mbedtls_mpi_div_mpi((mbedtls_mpi *) c, NULL, (const mbedtls_mpi *) a, (const mbedtls_mpi *) b) ?
           -1 : 0;
}

int crypto_bignum_mulmod(const struct crypto_bignum *a,
                         const struct crypto_bignum *b,
                         const struct crypto_bignum *c,
                         struct crypto_bignum *d)
{
    int fast_ret = crypto_bignum_mulmod_secp256r1((const mbedtls_mpi *) a,
                                                  (const mbedtls_mpi *) b,
                                                  (const mbedtls_mpi *) c,
                                                  (mbedtls_mpi *) d);
    if (fast_ret != -2) {
        return fast_ret;
    }
    return mbedtls_mpi_mul_mpi((mbedtls_mpi *)d, (const mbedtls_mpi *)a, (const mbedtls_mpi *)b) ||
           mbedtls_mpi_mod_mpi((mbedtls_mpi *)d, (mbedtls_mpi *)d, (const mbedtls_mpi *)c) ? -1 : 0;
}

int crypto_bignum_sqrmod(const struct crypto_bignum *a,
                         const struct crypto_bignum *b,
                         struct crypto_bignum *c)
{
    return crypto_bignum_mulmod(a, a, b, c);
}

int crypto_bignum_rshift(const struct crypto_bignum *a, int n,
                         struct crypto_bignum *r)
{
    int res;
    res = mbedtls_mpi_copy((mbedtls_mpi *) r, (const mbedtls_mpi *) a);
    if (res) {
        return -1;
    }

    res = mbedtls_mpi_shift_r((mbedtls_mpi *)r, n);
    return res ? -1 : 0;

}

int crypto_bignum_cmp(const struct crypto_bignum *a,
                      const struct crypto_bignum *b)
{
    return mbedtls_mpi_cmp_mpi((const mbedtls_mpi *) a, (const mbedtls_mpi *) b);
}

int crypto_bignum_bits(const struct crypto_bignum *a)
{
    return mbedtls_mpi_bitlen((const mbedtls_mpi *) a);
}

int crypto_bignum_is_zero(const struct crypto_bignum *a)
{
    return (mbedtls_mpi_cmp_int((const mbedtls_mpi *) a, 0) == 0);
}

int crypto_bignum_is_one(const struct crypto_bignum *a)
{
    return (mbedtls_mpi_cmp_int((const mbedtls_mpi *) a, 1) == 0);
}

int crypto_bignum_is_odd(const struct crypto_bignum *a)
{
    return (mbedtls_mpi_get_bit((const mbedtls_mpi *) a, 0) == 1);
}

int crypto_bignum_rand(struct crypto_bignum *r, const struct crypto_bignum *m)
{
    return ((mbedtls_mpi_random((mbedtls_mpi *) r, 0, (const mbedtls_mpi *) m,
                                mbedtls_psa_get_random, MBEDTLS_PSA_RANDOM_STATE) != 0) ? -1 : 0);
}

static int mbedtls_bignum_legendre(const struct crypto_bignum *a,
                                   const struct crypto_bignum *p)
{
    mbedtls_mpi exp, tmp;
    int res = -2, ret;

    mbedtls_mpi_init(&exp);
    mbedtls_mpi_init(&tmp);

    /* exp = (p-1) / 2 */
    MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(&exp, (const mbedtls_mpi *) p, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_shift_r(&exp, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&tmp, (const mbedtls_mpi *) a, &exp, (const mbedtls_mpi *) p, NULL));

    if (mbedtls_mpi_cmp_int(&tmp, 1) == 0) {
        res = 1;
    } else if (mbedtls_mpi_cmp_int(&tmp, 0) == 0
               /* The below check is workaround for the case where HW
                * does not behave properly for X ^ A mod M when X is
                * power of M. Instead of returning value 0, value M is
                * returned.*/
               || mbedtls_mpi_cmp_mpi(&tmp, (const mbedtls_mpi *)p) == 0) {
        res = 0;
    } else {
        res = -1;
    }

cleanup:
    mbedtls_mpi_free(&tmp);
    mbedtls_mpi_free(&exp);
    return res;
}

int crypto_bignum_legendre(const struct crypto_bignum *a,
                           const struct crypto_bignum *p)
{
    int jacobi_res;

    jacobi_res = crypto_bignum_legendre_secp256r1((const mbedtls_mpi *) a,
                                                  (const mbedtls_mpi *) p);
    if (jacobi_res != -2) {
        return jacobi_res;
    }

    jacobi_res = crypto_bignum_legendre_jacobi((const mbedtls_mpi *) a,
                                               (const mbedtls_mpi *) p);
    if (jacobi_res != -2) {
        return jacobi_res;
    }

    return mbedtls_bignum_legendre(a, p);
}

int crypto_bignum_addmod(const struct crypto_bignum *a,
                         const struct crypto_bignum *b,
                         const struct crypto_bignum *c,
                         struct crypto_bignum *d)
{
    struct crypto_bignum *tmp = crypto_bignum_init();
    int ret = -1;

    if (mbedtls_mpi_add_mpi((mbedtls_mpi *) tmp, (const mbedtls_mpi *) a, (const mbedtls_mpi *) b) < 0) {
        goto fail;
    }

    if (mbedtls_mpi_mod_mpi((mbedtls_mpi *) d, (const mbedtls_mpi *) tmp, (const mbedtls_mpi *) c) < 0) {
        goto fail;
    }

    ret = 0;
fail:
    crypto_bignum_deinit(tmp, 0);
    return ret;
}

void crypto_free_buffer(unsigned char *buf)
{
    os_free(buf);
}
