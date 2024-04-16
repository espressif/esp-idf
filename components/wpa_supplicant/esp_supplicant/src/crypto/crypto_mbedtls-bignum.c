/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef ESP_PLATFORM
#include "esp_system.h"
#include "mbedtls/bignum.h"
#endif

#include "utils/includes.h"
#include "utils/common.h"
#include "crypto.h"
#include "random.h"
#include "sha256.h"
#include "mbedtls/pk.h"

static int crypto_rng_wrapper(void *ctx, unsigned char *buf, size_t len)
{
    return random_get_bytes(buf, len);
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

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(bn, buf, len));
    return (struct crypto_bignum *) bn;

cleanup:
    os_free(bn);
    return NULL;
}

struct crypto_bignum * crypto_bignum_init_uint(unsigned int val)
{

    mbedtls_mpi *bn = os_zalloc(sizeof(mbedtls_mpi));
    if (bn == NULL) {
        return NULL;
    }

    mbedtls_mpi_init(bn);
    mbedtls_mpi_lset(bn, val);

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
    return  mbedtls_mpi_exp_mod((mbedtls_mpi *) d, (const mbedtls_mpi *) a, (const mbedtls_mpi *) b, (const mbedtls_mpi *) c, NULL) ? -1 : 0;

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
    return mbedtls_mpi_mul_mpi((mbedtls_mpi *)d, (const mbedtls_mpi *)a, (const mbedtls_mpi *)b) ||
           mbedtls_mpi_mod_mpi((mbedtls_mpi *)d, (mbedtls_mpi *)d, (const mbedtls_mpi *)c) ? -1 : 0;
}

int crypto_bignum_sqrmod(const struct crypto_bignum *a,
                         const struct crypto_bignum *b,
                         struct crypto_bignum *c)
{
    int res;
    struct crypto_bignum *tmp = crypto_bignum_init();
    if (!tmp) {
        return -1;
    }

    res = mbedtls_mpi_copy((mbedtls_mpi *) tmp, (const mbedtls_mpi *) a);
    res = crypto_bignum_mulmod(a, tmp, b, c);

    crypto_bignum_deinit(tmp, 0);
    return res ? -1 : 0;
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
                                crypto_rng_wrapper, NULL) != 0) ? -1 : 0);
}

int crypto_bignum_legendre(const struct crypto_bignum *a,
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

int crypto_bignum_to_string(const struct crypto_bignum *a,
                            u8 *buf, size_t buflen, size_t padlen)
{
    int num_bytes, offset;
    size_t outlen;

    if (padlen > buflen) {
        return -1;
    }

    num_bytes = mbedtls_mpi_size((mbedtls_mpi *) a);

    if (padlen > (size_t) num_bytes) {
        offset = padlen - num_bytes;
    } else {
        offset = 0;
    }

    os_memset(buf, 0, offset);
    mbedtls_mpi_write_string((mbedtls_mpi *) a, 16, (char *)(buf + offset),
                             mbedtls_mpi_size((mbedtls_mpi *)a), &outlen);

    return outlen;
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
