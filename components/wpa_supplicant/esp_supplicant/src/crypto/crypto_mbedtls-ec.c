/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
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
#include "sha256.h"
#include "random.h"

#include "mbedtls/ecp.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

#include "mbedtls/pk.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/sha256.h"
#include "mbedtls/asn1write.h"
#include "mbedtls/error.h"
#include "mbedtls/oid.h"

#define ECP_PRV_DER_MAX_BYTES   29 + 3 * MBEDTLS_ECP_MAX_BYTES

#ifdef CONFIG_MBEDTLS_ECDH_LEGACY_CONTEXT
#define ACCESS_ECDH(S, var) S->MBEDTLS_PRIVATE(var)
#else
#define ACCESS_ECDH(S, var) S->MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(var)
#endif

#ifdef CONFIG_ECC
struct crypto_ec {
	mbedtls_ecp_group group;
};

static int crypto_rng_wrapper(void *ctx, unsigned char *buf, size_t len)
{
	return random_get_bytes(buf, len);
}

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

	mbedtls_ecp_group_init(&e->group);

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

	mbedtls_ecp_group_free(&e->group);
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

size_t crypto_ec_order_len(struct crypto_ec *e)
{
	return mbedtls_mpi_size(&e->group.N);
}


size_t crypto_ec_prime_len_bits(struct crypto_ec *e)
{
	return mbedtls_mpi_bitlen(&e->group.P);
}
struct crypto_ec_group *crypto_ec_get_group_byname(const char *name)
{
	struct crypto_ec *e;
	const mbedtls_ecp_curve_info *curve = mbedtls_ecp_curve_info_from_name(name);

	e = os_zalloc(sizeof(*e));
	if (e == NULL) {
		return NULL;
	}

	mbedtls_ecp_group_init( &e->group );

	if (mbedtls_ecp_group_load(&e->group, curve->grp_id)) {
		crypto_ec_deinit(e);
		e = NULL;
	}

	return (struct crypto_ec_group *) &e->group;
}

const struct crypto_bignum *crypto_ec_get_prime(struct crypto_ec *e)
{
	return (const struct crypto_bignum *) &e->group.P;
}


const struct crypto_bignum *crypto_ec_get_order(struct crypto_ec *e)
{
	return (const struct crypto_bignum *) &e->group.N;
}


const struct crypto_bignum * crypto_ec_get_b(struct crypto_ec *e)
{
	return (const struct crypto_bignum *) &e->group.B;
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
		if(crypto_bignum_to_bin((struct crypto_bignum *) & ((mbedtls_ecp_point *) point)->MBEDTLS_PRIVATE(X),
					x, len, len) < 0) {
			return -1;
		}

	}

	if (y) {
		if(crypto_bignum_to_bin((struct crypto_bignum *) & ((mbedtls_ecp_point *) point)->MBEDTLS_PRIVATE(Y),
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

	if (!mbedtls_ecp_is_zero(point)  && (mbedtls_mpi_cmp_int( &point->MBEDTLS_PRIVATE(Z), 1 ) == 0 )) {
		// Affine coordinates mean that z should be 1,
		wpa_printf(MSG_ERROR, "Z coordinate is neither 0 or 1");
		return -1;
	}

	if (x) {
		MBEDTLS_MPI_CHK(mbedtls_mpi_copy((mbedtls_mpi*) x, &((mbedtls_ecp_point* )point)->MBEDTLS_PRIVATE(X)));
	}
	if (y) {
		MBEDTLS_MPI_CHK(mbedtls_mpi_copy((mbedtls_mpi*) y, &((mbedtls_ecp_point* )point)->MBEDTLS_PRIVATE(Y)));
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

	if (e == NULL) {
		return NULL;
	}

	len = mbedtls_mpi_size(&e->group.P);

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
	mbedtls_ctr_drbg_init(&ctr_drbg);

	MBEDTLS_MPI_CHK(mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
				NULL, 0));

	MBEDTLS_MPI_CHK(mbedtls_ecp_mul(&e->group,
				(mbedtls_ecp_point *) res,
				(const mbedtls_mpi *)b,
				(const mbedtls_ecp_point *)p,
				mbedtls_ctr_drbg_random,
				&ctr_drbg));
cleanup:
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);
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
	return (ret );
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

		MBEDTLS_MPI_CHK(mbedtls_mpi_add_int(&temp, &e->group.P, 1));
		MBEDTLS_MPI_CHK(mbedtls_mpi_div_int(&temp, NULL, &temp, 4));
		MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(y, y_sqr, &temp, &e->group.P, NULL));

		if (y_bit != mbedtls_mpi_get_bit(y, 0))
			MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(y, &e->group.P, y));

		MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&((mbedtls_ecp_point* )p)->MBEDTLS_PRIVATE(X), (const mbedtls_mpi*) x));
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
	if (y_sqr == NULL) {
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
	MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&temp, &temp, &e->group.P));
#else
	/* Calculate x^3  mod P*/
	MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&num, 3));
	MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&temp, (const mbedtls_mpi *) x, &num, &e->group.P, NULL));
#endif

	/* Calculate ax  mod P*/
	MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&num, -3));
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
	MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&two, 2));
	MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&y_sqr_lhs, &((const mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(Y) , &two, &e->group.P, NULL));

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

int crypto_key_compare(struct crypto_key *key1, struct crypto_key *key2)
{
	int ret = 0;
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;

	mbedtls_entropy_init(&entropy);
	mbedtls_ctr_drbg_init(&ctr_drbg);

	MBEDTLS_MPI_CHK(mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0));
	if (mbedtls_pk_check_pair((mbedtls_pk_context *)key1, (mbedtls_pk_context *)key2, mbedtls_ctr_drbg_random, &ctr_drbg) < 0) {
		goto cleanup;
	}

	ret = 1;
cleanup:
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);
	return ret;
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

static struct crypto_key *crypto_alloc_key(void)
{
	mbedtls_pk_context *key = os_malloc(sizeof(*key));

	if (!key) {
		wpa_printf(MSG_ERROR, "%s: memory allocation failed", __func__);
		return NULL;
	}
	mbedtls_pk_init(key);

	return (struct crypto_key *)key;
}


struct crypto_key * crypto_ec_set_pubkey_point(const struct crypto_ec_group *group,
		const u8 *buf, size_t len)
{
	mbedtls_ecp_point *point = NULL;
	struct crypto_key *pkey = NULL;
	int ret;
	mbedtls_pk_context *key = (mbedtls_pk_context *)crypto_alloc_key();

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

	if (mbedtls_ecp_check_pubkey((mbedtls_ecp_group *)group, point) < 0) { //typecast
		// ideally should have failed in upper condition, duplicate code??
		wpa_printf(MSG_ERROR, "Invalid key");
		goto fail;
	}
	/* Assign values */
	if( ( ret = mbedtls_pk_setup( key,
					mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY) ) ) != 0 )
		goto fail;
	mbedtls_ecp_copy(&mbedtls_pk_ec(*key)->MBEDTLS_PRIVATE(Q), point);
	mbedtls_ecp_group_load(&mbedtls_pk_ec(*key)->MBEDTLS_PRIVATE(grp), MBEDTLS_ECP_DP_SECP256R1);

	pkey = (struct crypto_key *)key;
	crypto_ec_point_deinit((struct crypto_ec_point *)point, 0);
	return pkey;
fail:
	if (point)
		crypto_ec_point_deinit((struct crypto_ec_point *)point, 0);
	if (key)
		mbedtls_pk_free(key);
	pkey = NULL;
	return pkey;
}


void crypto_ec_free_key(struct crypto_key *key)
{
	mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;
	mbedtls_pk_free(pkey);
	os_free(key);
}

struct crypto_ec_point *crypto_ec_get_public_key(struct crypto_key *key)
{
	mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;

	return (struct crypto_ec_point *)&mbedtls_pk_ec(*pkey)->MBEDTLS_PRIVATE(Q);
}


int crypto_ec_get_priv_key_der(struct crypto_key *key, unsigned char **key_data, int *key_len)
{
	mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;
	char der_data[ECP_PRV_DER_MAX_BYTES];

	*key_len = mbedtls_pk_write_key_der(pkey, (unsigned char *)der_data, ECP_PRV_DER_MAX_BYTES);
	if (*key_len <= 0)
		return -1;

	*key_data = os_malloc(*key_len);

	if (!*key_data) {
		wpa_printf(MSG_ERROR, "memory allocation failed");
		return -1;
	}
	os_memcpy(*key_data, der_data, *key_len);

	return 0;
}

struct crypto_ec_group *crypto_ec_get_group_from_key(struct crypto_key *key)
{
	mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;

	return (struct crypto_ec_group *)&(mbedtls_pk_ec(*pkey)->MBEDTLS_PRIVATE(grp));
}

int crypto_ec_key_group(struct crypto_ec_key *key)
{
	mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;

	int iana_group = (int)crypto_ec_get_mbedtls_to_nist_group_id(mbedtls_pk_ec(*pkey)->MBEDTLS_PRIVATE(grp).id);
	return iana_group;
}

struct crypto_bignum *crypto_ec_get_private_key(struct crypto_key *key)
{
	mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;

	return ((struct crypto_bignum *)&(mbedtls_pk_ec(*pkey)->MBEDTLS_PRIVATE(d)));
}

int crypto_ec_get_publickey_buf(struct crypto_key *key, u8 *key_buf, int len)
{
	mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;
	unsigned char buf[MBEDTLS_MPI_MAX_SIZE + 10]; /* tag, length + MPI */
	unsigned char *c = buf + sizeof(buf );
	int pk_len = 0;

	memset(buf, 0, sizeof(buf) );
	pk_len = mbedtls_pk_write_pubkey( &c, buf, pkey);

	if (pk_len < 0)
		return -1;

	if (len == 0)
		return pk_len;

	os_memcpy(key_buf, buf + MBEDTLS_MPI_MAX_SIZE + 10 - pk_len, pk_len);

	return pk_len;
}

int crypto_write_pubkey_der(struct crypto_key *key, unsigned char **key_buf)
{
	unsigned char output_buf[1600] = {0};
	int len = mbedtls_pk_write_pubkey_der((mbedtls_pk_context *)key, output_buf, 1600);
	if (len <= 0)
		return 0;

	*key_buf = os_malloc(len);
	if (!*key_buf) {
		return 0;
	}
	os_memcpy(*key_buf, output_buf + 1600 - len, len);

	return len;
}

struct crypto_key *crypto_ec_get_key(const u8 *privkey, size_t privkey_len)
{
	int ret;
	mbedtls_pk_context *kctx = (mbedtls_pk_context *)crypto_alloc_key();

	if (!kctx) {
		wpa_printf(MSG_ERROR, "memory allocation failed");
		return NULL;
	}
	ret = mbedtls_pk_parse_key(kctx, privkey, privkey_len, NULL, 0, crypto_rng_wrapper, NULL);

	if (ret < 0) {
		//crypto_print_error_string(ret);
		goto fail;
	}

	return (struct crypto_key *)kctx;

fail:
	mbedtls_pk_free(kctx);
	os_free(kctx);
	return NULL;
}

unsigned int crypto_ec_get_mbedtls_to_nist_group_id(int id)
{
	unsigned int nist_grpid = 0;
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

int crypto_ecdh(struct crypto_key *key_own, struct crypto_key *key_peer,
		u8 *secret, size_t *secret_len)
{
	mbedtls_ecdh_context *ctx = NULL;
	mbedtls_pk_context *own = (mbedtls_pk_context *)key_own;
	mbedtls_pk_context *peer = (mbedtls_pk_context *)key_peer;
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	int ret = -1;

	mbedtls_entropy_init(&entropy);
	mbedtls_ctr_drbg_init(&ctr_drbg);

	if (mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0) < 0) {
		goto fail;
	}

	*secret_len = 0;
	ctx = os_malloc(sizeof(*ctx));
	if (!ctx) {
		wpa_printf(MSG_ERROR, "DPP: EVP_PKEY_CTX_new failed: %s",
				__func__);
		goto fail;
	}

	mbedtls_ecdh_init(ctx);
	/* No need to setup, done through mbedtls_ecdh_get_params */

	/* set params from our key */
	if (mbedtls_ecdh_get_params(ctx, mbedtls_pk_ec(*own), MBEDTLS_ECDH_OURS) < 0) {
		wpa_printf(MSG_ERROR, "failed to set our ecdh params");
		goto fail;
	}

#ifndef DPP_MAX_SHARED_SECRET_LEN
#define DPP_MAX_SHARED_SECRET_LEN 66
#endif
	/* set params from peers key */
	if (mbedtls_ecdh_get_params(ctx, mbedtls_pk_ec(*peer), MBEDTLS_ECDH_THEIRS) < 0) {
		wpa_printf(MSG_ERROR, "failed to set peer's ecdh params");
		goto fail;
	}

	if (mbedtls_ecdh_calc_secret(ctx, secret_len, secret, DPP_MAX_SHARED_SECRET_LEN,
				     mbedtls_ctr_drbg_random, &ctr_drbg) < 0) {
		wpa_printf(MSG_ERROR, "failed to calculate secret");
		goto fail;
	}

	if (*secret_len > DPP_MAX_SHARED_SECRET_LEN) {
		wpa_printf(MSG_ERROR, "secret len=%d is too big", *secret_len);
		goto fail;
	}

	ret = 0;

fail:
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);
	if (ctx) {
		mbedtls_ecdh_free(ctx);
		os_free(ctx);
	}
	return ret;
}


int crypto_ecdsa_get_sign(unsigned char *hash,
		const struct crypto_bignum *r, const struct crypto_bignum *s, struct crypto_key *csign, int hash_len)
{
	int ret = -1;
	mbedtls_pk_context *pkey = (mbedtls_pk_context *)csign;

	mbedtls_ecdsa_context *ctx = os_malloc(sizeof(*ctx));
	if (!ctx) {
		wpa_printf(MSG_ERROR,"failed to allcate memory");
		return -1;
	}
	mbedtls_ecdsa_init(ctx);

	if (mbedtls_ecdsa_from_keypair(ctx, mbedtls_pk_ec(*pkey)) < 0) {
		goto fail;
	}
	ret = mbedtls_ecdsa_sign(&ctx->MBEDTLS_PRIVATE(grp), (mbedtls_mpi *)r, (mbedtls_mpi *)s,
			&ctx->MBEDTLS_PRIVATE(d), hash, SHA256_MAC_LEN, crypto_rng_wrapper, NULL);

fail:
	mbedtls_ecdsa_free(ctx);
	os_free(ctx);

	return  ret;
}

int crypto_edcsa_sign_verify(const unsigned char *hash,
		const struct crypto_bignum *r, const struct crypto_bignum *s, struct crypto_key *csign, int hlen)
{
	mbedtls_pk_context *pkey = (mbedtls_pk_context *)csign;
	int ret = 0;

	mbedtls_ecdsa_context *ctx = os_malloc(sizeof(*ctx));
	if (!ctx) {
		wpa_printf(MSG_ERROR, "failed to allcate memory");
		return ret;
	}
	mbedtls_ecdsa_init(ctx);

	if (mbedtls_ecdsa_from_keypair(ctx, mbedtls_pk_ec(*pkey)) < 0)
		return ret;

	if((ret = mbedtls_ecdsa_verify(&ctx->MBEDTLS_PRIVATE(grp), hash, hlen,
					&ctx->MBEDTLS_PRIVATE(Q), (mbedtls_mpi *)r, (mbedtls_mpi *)s)) != 0){
		wpa_printf(MSG_ERROR, "ecdsa verification failed");
		return ret;
	}

	mbedtls_ecdsa_free(ctx);
	os_free(ctx);

	return ret;
}

void crypto_debug_print_ec_key(const char *title, struct crypto_key *key)
{
#ifdef DEBUG_PRINT
	mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;
	mbedtls_ecp_keypair *ecp = mbedtls_pk_ec( *pkey );
	u8 x[32], y[32], d[32];
	wpa_printf(MSG_ERROR, "curve: %s",
			mbedtls_ecp_curve_info_from_grp_id( ecp->MBEDTLS_PRIVATE(grp).id )->name );
	int len = mbedtls_mpi_size((mbedtls_mpi *)crypto_ec_get_prime((struct crypto_ec *)crypto_ec_get_group_from_key(key)));

	wpa_printf(MSG_ERROR, "prime len is %d", len);
	crypto_ec_point_to_bin((struct crypto_ec *)crypto_ec_get_group_from_key(key), crypto_ec_get_public_key(key), x, y);
	crypto_bignum_to_bin(crypto_ec_get_private_key(key),
			d, len, len);
	wpa_hexdump(MSG_ERROR, "Q_x:", x, 32);
	wpa_hexdump(MSG_ERROR, "Q_y:", y, 32);
	wpa_hexdump(MSG_ERROR, "d:     ",  d , 32);
#endif
}

struct crypto_key *crypto_ec_parse_subpub_key(const unsigned char *p, size_t len)
{
	int ret;
	mbedtls_pk_context *pkey = (mbedtls_pk_context *)crypto_alloc_key();
	ret = mbedtls_pk_parse_subpubkey((unsigned char **)&p, p + len, pkey);

	if (ret < 0) {
		os_free(pkey);
		return NULL;
	}

	return (struct crypto_key *)pkey;
}

int crypto_is_ec_key(struct crypto_key *key)
{
	int ret = mbedtls_pk_can_do((mbedtls_pk_context *)key, MBEDTLS_PK_ECKEY);
	return  ret;
}

struct crypto_key * crypto_ec_gen_keypair(u16 ike_group)
{
	mbedtls_pk_context *kctx = (mbedtls_pk_context *)crypto_alloc_key();

	if (!kctx) {
		wpa_printf(MSG_ERROR, "%s: memory allocation failed", __func__);
		return NULL;
	}

	if(mbedtls_pk_setup(kctx,
				mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY)) != 0 )
		goto fail;

	mbedtls_ecp_gen_key(MBEDTLS_ECP_DP_SECP256R1, mbedtls_pk_ec(*kctx), //get this from argument
			crypto_rng_wrapper, NULL);

	return (struct crypto_key *)kctx;
fail:
	mbedtls_pk_free(kctx);
	os_free(kctx);
	return NULL;
}

/*
 * ECParameters ::= CHOICE {
 *   namedCurve         OBJECT IDENTIFIER
 * }
 */
static int pk_write_ec_param( unsigned char **p, unsigned char *start,
		mbedtls_ecp_keypair *ec )
{
	int ret;
	size_t len = 0;
	const char *oid;
	size_t oid_len;

	if( ( ret = mbedtls_oid_get_oid_by_ec_grp( ec->MBEDTLS_PRIVATE(grp).id, &oid, &oid_len ) ) != 0 )
		return( ret );

	MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_oid( p, start, oid, oid_len ) );

	return( (int) len );
}

static int pk_write_ec_pubkey_formatted( unsigned char **p, unsigned char *start,
		mbedtls_ecp_keypair *ec, int format )
{
	int ret;
	size_t len = 0;
	unsigned char buf[MBEDTLS_ECP_MAX_PT_LEN];

	if( ( ret = mbedtls_ecp_point_write_binary( &ec->MBEDTLS_PRIVATE(grp), &ec->MBEDTLS_PRIVATE(Q),
					format,
					&len, buf, sizeof( buf ) ) ) != 0 )
	{
		return( ret );
	}

	if( *p < start || (size_t)( *p - start ) < len )
		return( MBEDTLS_ERR_ASN1_BUF_TOO_SMALL );

	*p -= len;
	memcpy( *p, buf, len );

	return( (int) len );
}

int mbedtls_pk_write_pubkey_formatted( unsigned char **p, unsigned char *start,
		const mbedtls_pk_context *key, int format )
{
	int ret;
	size_t len = 0;

	if( mbedtls_pk_get_type( key ) == MBEDTLS_PK_ECKEY )
		MBEDTLS_ASN1_CHK_ADD( len, pk_write_ec_pubkey_formatted( p, start, mbedtls_pk_ec( *key ), format ) );
	else
		return( MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE );

	return( (int) len );
}

int crypto_pk_write_formatted_pubkey_der(mbedtls_pk_context *key, unsigned char *buf, size_t size, int format)
{
	int ret;
	unsigned char *c;
	size_t len = 0, par_len = 0, oid_len;
	const char *oid;

	if( size == 0 )
		return( MBEDTLS_ERR_ASN1_BUF_TOO_SMALL );

	c = buf + size;

	MBEDTLS_ASN1_CHK_ADD( len, mbedtls_pk_write_pubkey_formatted( &c, buf, key, format) );

	if( c - buf < 1 )
		return( MBEDTLS_ERR_ASN1_BUF_TOO_SMALL );

	/*
	 *  SubjectPublicKeyInfo  ::=  SEQUENCE  {
	 *       algorithm            AlgorithmIdentifier,
	 *       subjectPublicKey     BIT STRING }
	 */
	*--c = 0;
	len += 1;


	MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_len( &c, buf, len ) );
	MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_tag( &c, buf, MBEDTLS_ASN1_BIT_STRING ) );

	if( ( ret = mbedtls_oid_get_oid_by_pk_alg( mbedtls_pk_get_type( key ),
					&oid, &oid_len ) ) != 0 )
	{
		return( ret );
	}

	if( mbedtls_pk_get_type( key ) == MBEDTLS_PK_ECKEY )
	{
		MBEDTLS_ASN1_CHK_ADD( par_len, pk_write_ec_param( &c, buf, mbedtls_pk_ec( *key ) ) );
	}

	MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_algorithm_identifier( &c, buf, oid, oid_len,
				par_len ) );

	MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_len( &c, buf, len ) );
	MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_tag( &c, buf, MBEDTLS_ASN1_CONSTRUCTED |
				MBEDTLS_ASN1_SEQUENCE ) );

	return( (int) len );
}

int crypto_ec_write_pub_key(struct crypto_key *key, unsigned char **key_buf)
{
	unsigned char output_buf[1600] = {0};
	int len = crypto_pk_write_formatted_pubkey_der((mbedtls_pk_context *)key, output_buf, 1600, 1);
	if (len <= 0)
		return 0;

	*key_buf = os_malloc(len);
	if (!*key_buf) {
		wpa_printf(MSG_ERROR, "%s: memory allocation failed", __func__);
		return 0;
	}
	os_memcpy(*key_buf, output_buf + 1600 - len, len);

	return len;
}

int crypto_mbedtls_get_grp_id(int group)
{
        switch(group) {
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
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_entropy_context entropy;
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

	/* Initialize CTR_DRBG context */
	mbedtls_ctr_drbg_init(&ctr_drbg);
	mbedtls_entropy_init(&entropy);

	/* Seed and setup CTR_DRBG entropy source for future reseeds */
	if (mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0) != 0) {
                wpa_printf(MSG_ERROR, "Seeding entropy source failed");
                goto fail;
	}

	/* Generates ECDH keypair on elliptic curve */
	if (mbedtls_ecdh_gen_public(ACCESS_ECDH(&ctx, grp), ACCESS_ECDH(&ctx, d), ACCESS_ECDH(&ctx, Q), mbedtls_ctr_drbg_random, &ctr_drbg)!=0) {
                wpa_printf(MSG_ERROR, "ECDH keypair on curve failed");
                goto fail;
	}

	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);
	return (struct crypto_ecdh *)ctx;
fail:
	if (ctx) {
		mbedtls_ecdh_free(ctx);
		os_free(ctx);
		ctx = NULL;
	}
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);
	return NULL;
}

struct wpabuf * crypto_ecdh_get_pubkey(struct crypto_ecdh *ecdh, int y)
{
	struct wpabuf *public_key = NULL;
	uint8_t *buf = NULL;
	mbedtls_ecdh_context *ctx = (mbedtls_ecdh_context *)ecdh;
	size_t prime_len = ACCESS_ECDH(ctx, grp).pbits/8;

	buf = os_zalloc(y ? prime_len : 2 * prime_len);
        if (!buf) {
                wpa_printf(MSG_ERROR, "Memory allocation failed");
                return NULL;
        }

	/* Export an MPI into unsigned big endian binary data of fixed size */
	mbedtls_mpi_write_binary(ACCESS_ECDH(&ctx, Q).MBEDTLS_PRIVATE(X), buf, prime_len);
	public_key = wpabuf_alloc_copy(buf, 32);
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
	struct crypto_key *pkey = NULL;
	struct wpabuf *sh_secret = NULL;
	int secret_key = 0;

	mbedtls_ecdh_context *ctx = (mbedtls_ecdh_context *)ecdh;

	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_entropy_context entropy;

	/* Initialize CTR_DRBG context */
	mbedtls_ctr_drbg_init(&ctr_drbg);
	mbedtls_entropy_init(&entropy);

	/* Seed and setup CTR_DRBG entropy source for future reseeds */
	if (mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0) != 0) {
		wpa_printf(MSG_ERROR, "Seeding entropy source failed");
		goto cleanup;
	}
	len_prime = ACCESS_ECDH(ctx, grp).pbits/8;
	bn_x = crypto_bignum_init_set(key, len);

	/* Initialize data for EC point */
	ec_pt = crypto_ec_point_init((struct crypto_ec*)ACCESS_ECDH(&ctx, grp));
	if (!ec_pt) {
		wpa_printf(MSG_ERROR,"Initializing for EC point failed");
		goto cleanup;
	}

	if (crypto_ec_point_solve_y_coord((struct crypto_ec*)ACCESS_ECDH(&ctx, grp), ec_pt, bn_x, inc_y) != 0) {
                wpa_printf(MSG_ERROR,"Failed to solve for y coordinate");
                goto cleanup;
	}
	px = os_zalloc(len);
	py = os_zalloc(len);
	buf = os_zalloc(2*len);

        if (!px || !py || !buf) {
                wpa_printf(MSG_ERROR, "Memory allocation failed");
                goto cleanup;
        }
	if (crypto_ec_point_to_bin((struct crypto_ec*)ACCESS_ECDH(&ctx, grp), ec_pt, px, py) != 0) {
                wpa_printf(MSG_ERROR,"Failed to write EC point value as binary data");
                goto cleanup;
	}

	os_memcpy(buf, px, len);
	os_memcpy(buf+len, py, len);

	pkey = crypto_ec_set_pubkey_point((struct crypto_ec_group*)ACCESS_ECDH(&ctx, grp), buf, len);
        if (!pkey) {
                wpa_printf(MSG_ERROR, "Failed to set point for peer's public key");
                goto cleanup;
	}


	mbedtls_pk_context *peer = (mbedtls_pk_context*)pkey;

	/* Setup ECDH context from EC key */
/* Call to mbedtls_ecdh_get_params() will initialize the context when not LEGACY context */
        if (ctx != NULL && peer != NULL) {
				mbedtls_ecp_copy( ACCESS_ECDH(&ctx, Qp), &(mbedtls_pk_ec(*peer))->MBEDTLS_PRIVATE(Q) );
#ifndef CONFIG_MBEDTLS_ECDH_LEGACY_CONTEXT
				ctx->MBEDTLS_PRIVATE(var) = MBEDTLS_ECDH_VARIANT_MBEDTLS_2_0;
#endif
        } else {
                wpa_printf(MSG_ERROR, "Failed to set peer's ECDH context");
                goto cleanup;
        }
	int len_secret = inc_y ? 2*len : len;
	secret = os_zalloc(len_secret);
	if (!secret) {
		wpa_printf(MSG_ERROR, "Allocation failed for secret");
		goto cleanup;
	}

	/* Calculate secret
	z = F(DH(x,Y)) */
	secret_key = mbedtls_ecdh_calc_secret(ctx, &olen, secret, len_prime, mbedtls_ctr_drbg_random, &ctr_drbg);
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
	crypto_ec_free_key(pkey);
	crypto_bignum_deinit(bn_x, 1);
	crypto_ec_point_deinit(ec_pt, 1);
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);
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
	if (ctx_verify == NULL) {
		return -1;
	}

	mbedtls_ecdsa_init(ctx_verify);

	mbedtls_ecp_keypair *ec_key = mbedtls_pk_ec(*((mbedtls_pk_context *)key));
	mbedtls_ecp_group *grp = &ec_key->MBEDTLS_PRIVATE(grp);

	if ((ret = mbedtls_ecp_group_copy(&ctx_verify->MBEDTLS_PRIVATE(grp),grp)) != 0) {
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
