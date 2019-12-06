// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifdef ESP_PLATFORM
#include "esp_system.h"
#include "mbedtls/bignum.h"
#endif

#include "utils/includes.h"
#include "utils/common.h"
#include "crypto.h"

#include "mbedtls/ecp.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"



#define IANA_SECP256R1 19

#ifdef ESP_PLATFORM
int crypto_get_random(void *buf, size_t len)
{
    if (!buf) {
        return -1;
    }
    esp_fill_random(buf, len);
    return 0;
}
#endif

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


void crypto_bignum_deinit(struct crypto_bignum *n, int clear)
{
    mbedtls_mpi_free((mbedtls_mpi *)n);
    os_free((mbedtls_mpi *)n);
}


int crypto_bignum_to_bin(const struct crypto_bignum *a,
                         u8 *buf, size_t buflen, size_t padlen)
{
    int num_bytes, offset;

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
    mbedtls_mpi_write_binary((mbedtls_mpi *) a, buf + offset, mbedtls_mpi_size((mbedtls_mpi *)a) );

    return num_bytes + offset;
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
    int res;
#if ALLOW_EVEN_MOD // Must enable this macro if c is even.
    mbedtls_mpi temp;
    mbedtls_mpi_init(&temp);

    res = mbedtls_mpi_mul_mpi(&temp, (const mbedtls_mpi *) a, (const mbedtls_mpi *) b);
    if (res) {
        return -1;
    }

    res = mbedtls_mpi_mod_mpi((mbedtls_mpi *) d, &temp, (mbedtls_mpi *) c);

    mbedtls_mpi_free(&temp);
#else
    // Works with odd modulus only, but it is faster with HW acceleration
    res = esp_mpi_mul_mpi_mod((mbedtls_mpi *) d, (mbedtls_mpi *) a, (mbedtls_mpi *) b, (mbedtls_mpi *) c);
#endif
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

#ifdef CONFIG_ECC
struct crypto_ec {
    mbedtls_ecp_group group;
};

struct crypto_ec *crypto_ec_init(int group)
{
    struct crypto_ec *e;

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
    if (e == NULL) {
        return NULL;
    }

    mbedtls_ecp_group_init( &e->group );

    if (mbedtls_ecp_group_load(&e->group, grp_id)) {
        crypto_ec_deinit(e);
        e = NULL;
    }

    return e;
}


void crypto_ec_deinit(struct crypto_ec *e)
{
    if (e == NULL) {
        return;
    }

    mbedtls_ecp_group_free( &e->group );
    os_free(e);
}


struct crypto_ec_point *crypto_ec_point_init(struct crypto_ec *e)
{
    mbedtls_ecp_point *pt;
    if (e == NULL) {
        return NULL;
    }

    pt = os_zalloc(sizeof(mbedtls_ecp_point));

    if( pt == NULL) {
        return NULL;
    }

    mbedtls_ecp_point_init(pt);

    return (struct crypto_ec_point *) pt;
}


size_t crypto_ec_prime_len(struct crypto_ec *e)
{
    return mbedtls_mpi_size(&e->group.P);
}


size_t crypto_ec_prime_len_bits(struct crypto_ec *e)
{
    return mbedtls_mpi_bitlen(&e->group.P);
}


const struct crypto_bignum *crypto_ec_get_prime(struct crypto_ec *e)
{
    return (const struct crypto_bignum *) &e->group.P;
}


const struct crypto_bignum *crypto_ec_get_order(struct crypto_ec *e)
{
    return (const struct crypto_bignum *) &e->group.N;
}


void crypto_ec_point_deinit(struct crypto_ec_point *p, int clear)
{
    mbedtls_ecp_point_free((mbedtls_ecp_point *) p);
    os_free(p);
}


int crypto_ec_point_to_bin(struct crypto_ec *e,
                           const struct crypto_ec_point *point, u8 *x, u8 *y)
{
    int len = mbedtls_mpi_size(&e->group.P);

    if (x) {
        if(crypto_bignum_to_bin((struct crypto_bignum *) & ((mbedtls_ecp_point *) point)->X,
                             x, len, len) < 0) {
            return -1;
        }

    }

    if (y) {
        if(crypto_bignum_to_bin((struct crypto_bignum *) & ((mbedtls_ecp_point *) point)->Y,
                             y, len, len) < 0) {
            return -1;
        }
    }

    return 0;
}


struct crypto_ec_point *crypto_ec_point_from_bin(struct crypto_ec *e,
        const u8 *val)
{
    mbedtls_ecp_point *pt;
    int len, ret;

    if (e == NULL) {
        return NULL;
    }

    len = mbedtls_mpi_size(&e->group.P);

    pt = os_zalloc(sizeof(mbedtls_ecp_point));
    mbedtls_ecp_point_init(pt);

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&pt->X, val, len));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&pt->Y, val + len, len));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset((&pt->Z), 1));

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

    MBEDTLS_MPI_CHK(mbedtls_mpi_lset( &one, 1 ));
    MBEDTLS_MPI_CHK(mbedtls_ecp_muladd(&e->group, (mbedtls_ecp_point *) c, &one, (const mbedtls_ecp_point *)a , &one, (const mbedtls_ecp_point *)b));

cleanup:
    mbedtls_mpi_free(&one);
    return ret ? -1 : 0;
}


int crypto_ec_point_mul(struct crypto_ec *e, const struct crypto_ec_point *p,
                        const struct crypto_bignum *b,
                        struct crypto_ec_point *res)
{
    int ret;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

    mbedtls_entropy_init(&entropy);

    MBEDTLS_MPI_CHK(mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                    NULL, 0));

    MBEDTLS_MPI_CHK(mbedtls_ecp_mul(&e->group,
                (mbedtls_ecp_point *) res,
                (const mbedtls_mpi *)b,
                (const mbedtls_ecp_point *)p,
                mbedtls_ctr_drbg_random,
                &ctr_drbg));
cleanup:
    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );
    return ret ? -1 : 0;
}


/*  Currently mbedtls does not have any function for inverse
 *  This function calculates inverse of a point.
 *  Set R = -P
 */
static int ecp_opp( const mbedtls_ecp_group *grp, mbedtls_ecp_point *R, const mbedtls_ecp_point *P)
{
    int ret = 0;

    /* Copy */
    if (R != P) {
        MBEDTLS_MPI_CHK(mbedtls_ecp_copy(R, P));
    }

    /* In-place opposite */
    if (mbedtls_mpi_cmp_int( &R->Y, 0) != 0) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(&R->Y, &grp->P, &R->Y));
    }

cleanup:
    return ( ret );
}

int crypto_ec_point_invert(struct crypto_ec *e, struct crypto_ec_point *p)
{
    return ecp_opp(&e->group, (mbedtls_ecp_point *) p, (mbedtls_ecp_point *) p) ? -1 : 0;
}

int crypto_ec_point_solve_y_coord(struct crypto_ec *e,
                                  struct crypto_ec_point *p,
                                  const struct crypto_bignum *x, int y_bit)
{
    mbedtls_mpi temp;
    mbedtls_mpi *y_sqr, *y;
    mbedtls_mpi_init(&temp);
    int ret = 0;

    y = &((mbedtls_ecp_point *)p)->Y;

    /* Faster way to find sqrt
     * Works only with curves having prime p
     * such that p ≡ 3 (mod 4)
     *  y_ = (y2 ^ ((p+1)/4)) mod p
     *
     *  if y_bit: y = p-y_
     *   else y = y_`
     */

    y_sqr = (mbedtls_mpi *) crypto_ec_point_compute_y_sqr(e, x);

    if (y_sqr) {

        MBEDTLS_MPI_CHK(mbedtls_mpi_add_int(&temp, &e->group.P, 1));
        MBEDTLS_MPI_CHK(mbedtls_mpi_div_int(&temp, NULL, &temp, 4));
        MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(y, y_sqr, &temp, &e->group.P, NULL));

        if (y_bit) {
            MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(y, &e->group.P, y));
        }
    } else {
        ret = 1;
    }
cleanup:
    mbedtls_mpi_free(&temp);
    mbedtls_mpi_free(y_sqr);
    os_free(y_sqr);
    return ret ? -1 : 0;
}

struct crypto_bignum *
crypto_ec_point_compute_y_sqr(struct crypto_ec *e,
                              const struct crypto_bignum *x)
{
    mbedtls_mpi temp, temp2, num;
    int ret = 0;

    mbedtls_mpi *y_sqr = os_zalloc(sizeof(mbedtls_mpi));
    if (y_sqr == NULL) {
        return NULL;
    }

    mbedtls_mpi_init(&temp);
    mbedtls_mpi_init(&temp2);
    mbedtls_mpi_init(&num);
    mbedtls_mpi_init(y_sqr);

    /* y^2 = x^3 + ax + b  mod  P*/
    /* mbedtls does not have mod-add or mod-mul apis.
     *
     */

    /* Calculate x^3  mod P*/
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset( &num, 3));
    MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&temp, (const mbedtls_mpi *) x, &num, &e->group.P, NULL));

    /* Calculate ax  mod P*/
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset( &num, -3));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&temp2, (const mbedtls_mpi *) x, &num));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&temp2, &temp2, &e->group.P));

    /* Calculate ax + b  mod P. Note that b is already < P*/
    MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&temp2, &temp2, &e->group.B));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&temp2, &temp2, &e->group.P));

    /* Calculate x^3 + ax + b  mod P*/
    MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&temp2, &temp2, &temp));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(y_sqr, &temp2, &e->group.P));


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
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset( &two, 2));
    MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&y_sqr_lhs, &((const mbedtls_ecp_point *)p)->Y , &two, &e->group.P, NULL));

    y_sqr_rhs = (mbedtls_mpi *) crypto_ec_point_compute_y_sqr(e, (const struct crypto_bignum *) & ((const mbedtls_ecp_point *)p)->X);

    if (y_sqr_rhs && (mbedtls_mpi_cmp_mpi(y_sqr_rhs, &y_sqr_lhs) == 0)) {
        on_curve = 1;
    }

cleanup:
    mbedtls_mpi_free(&y_sqr_lhs);
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

#endif /* CONFIG_ECC */
