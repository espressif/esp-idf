/*
 * Shared Dragonfly functionality
 * Copyright (c) 2012-2016, Jouni Malinen <j@w1.fi>
 * Copyright (c) 2019, The Linux Foundation
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "utils/const_time.h"
#include "crypto/crypto.h"
#include "dragonfly.h"


int dragonfly_suitable_group(int group, int ecc_only)
{
	/* Enforce REVmd rules on which SAE groups are suitable for production
	 * purposes: FFC groups whose prime is >= 3072 bits and ECC groups
	 * defined over a prime field whose prime is >= 256 bits. Furthermore,
	 * ECC groups defined over a characteristic 2 finite field and ECC
	 * groups with a co-factor greater than 1 are not suitable. Disable
	 * groups that use Brainpool curves as well for now since they leak more
	 * timing information due to the prime not being close to a power of
	 * two. */
	return group == 19 || group == 20 || group == 21 ||
		(!ecc_only &&
		 (group == 15 || group == 16 || group == 17 || group == 18));
}


unsigned int dragonfly_min_pwe_loop_iter(int group)
{
	if (group == 22 || group == 23 || group == 24) {
		/* FFC groups for which pwd-value is likely to be >= p
		 * frequently */
		return 40;
	}

	if (group == 1 || group == 2 || group == 5 || group == 14 ||
	    group == 15 || group == 16 || group == 17 || group == 18) {
		/* FFC groups that have prime that is close to a power of two */
		return 1;
	}

	/* Default to 40 (this covers most ECC groups) */
	return 40;
}


int dragonfly_get_random_qr_qnr(const struct crypto_bignum *prime,
				struct crypto_bignum **qr,
				struct crypto_bignum **qnr)
{
	*qr = *qnr = NULL;

	while (!(*qr) || !(*qnr)) {
		struct crypto_bignum *tmp;
		int res;

		tmp = crypto_bignum_init();
		if (!tmp || crypto_bignum_rand(tmp, prime) < 0) {
			crypto_bignum_deinit(tmp, 0);
			break;
		}

		res = crypto_bignum_legendre(tmp, prime);
		if (res == 1 && !(*qr))
			*qr = tmp;
		else if (res == -1 && !(*qnr))
			*qnr = tmp;
		else
			crypto_bignum_deinit(tmp, 0);
	}

	if (*qr && *qnr)
		return 0;
	crypto_bignum_deinit(*qr, 0);
	crypto_bignum_deinit(*qnr, 0);
	*qr = *qnr = NULL;
	return -1;
}


static struct crypto_bignum *
dragonfly_get_rand_1_to_p_1(const struct crypto_bignum *prime)
{
	struct crypto_bignum *tmp, *pm1, *one;

	tmp = crypto_bignum_init();
	pm1 = crypto_bignum_init();
	one = crypto_bignum_init_set((const u8 *) "\x01", 1);
	if (!tmp || !pm1 || !one ||
	    crypto_bignum_sub(prime, one, pm1) < 0 ||
	    crypto_bignum_rand(tmp, pm1) < 0 ||
	    crypto_bignum_add(tmp, one, tmp) < 0) {
		crypto_bignum_deinit(tmp, 0);
		tmp = NULL;
	}

	crypto_bignum_deinit(pm1, 0);
	crypto_bignum_deinit(one, 0);
	return tmp;
}


int dragonfly_is_quadratic_residue_blind(struct crypto_ec *ec,
					 const u8 *qr, const u8 *qnr,
					 const struct crypto_bignum *val)
{
	struct crypto_bignum *r, *num, *qr_or_qnr = NULL;
	int check, res = -1;
	u8 qr_or_qnr_bin[DRAGONFLY_MAX_ECC_PRIME_LEN];
	const struct crypto_bignum *prime;
	size_t prime_len;
	unsigned int mask;

	prime = crypto_ec_get_prime(ec);
	prime_len = crypto_ec_prime_len(ec);

	/*
	 * Use a blinding technique to mask val while determining whether it is
	 * a quadratic residue modulo p to avoid leaking timing information
	 * while determining the Legendre symbol.
	 *
	 * v = val
	 * r = a random number between 1 and p-1, inclusive
	 * num = (v * r * r) modulo p
	 */
	r = dragonfly_get_rand_1_to_p_1(prime);
	if (!r)
		return -1;

	num = crypto_bignum_init();
	if (!num ||
	    crypto_bignum_mulmod(val, r, prime, num) < 0 ||
	    crypto_bignum_mulmod(num, r, prime, num) < 0)
		goto fail;

	/*
	 * Need to minimize differences in handling different cases, so try to
	 * avoid branches and timing differences.
	 *
	 * If r is odd:
	 * num = (num * qr) module p
	 * LGR(num, p) = 1 ==> quadratic residue
	 * else:
	 * num = (num * qnr) module p
	 * LGR(num, p) = -1 ==> quadratic residue
	 *
	 * mask is set to !odd(r)
	 */
	mask = const_time_is_zero(crypto_bignum_is_odd(r));
	const_time_select_bin(mask, qnr, qr, prime_len, qr_or_qnr_bin);
	qr_or_qnr = crypto_bignum_init_set(qr_or_qnr_bin, prime_len);
	if (!qr_or_qnr ||
	    crypto_bignum_mulmod(num, qr_or_qnr, prime, num) < 0)
		goto fail;
	/* branchless version of check = odd(r) ? 1 : -1, */
	check = const_time_select_int(mask, -1, 1);

	/* Determine the Legendre symbol on the masked value */
	res = crypto_bignum_legendre(num, prime);
	if (res == -2) {
		res = -1;
		goto fail;
	}
	/* branchless version of res = res == check
	 * (res is -1, 0, or 1; check is -1 or 1) */
	mask = const_time_eq(res, check);
	res = const_time_select_int(mask, 1, 0);
fail:
	crypto_bignum_deinit(num, 1);
	crypto_bignum_deinit(r, 1);
	crypto_bignum_deinit(qr_or_qnr, 1);
	return res;
}


static int dragonfly_get_rand_2_to_r_1(struct crypto_bignum *val,
				       const struct crypto_bignum *order)
{
	return crypto_bignum_rand(val, order) == 0 &&
		!crypto_bignum_is_zero(val) &&
		!crypto_bignum_is_one(val);
}


int dragonfly_generate_scalar(const struct crypto_bignum *order,
			      struct crypto_bignum *_rand,
			      struct crypto_bignum *_mask,
			      struct crypto_bignum *scalar)
{
	int count;

	/* Select two random values rand,mask such that 1 < rand,mask < r and
	 * rand + mask mod r > 1. */
	for (count = 0; count < 100; count++) {
		if (dragonfly_get_rand_2_to_r_1(_rand, order) &&
		    dragonfly_get_rand_2_to_r_1(_mask, order) &&
		    crypto_bignum_add(_rand, _mask, scalar) == 0 &&
		    crypto_bignum_mod(scalar, order, scalar) == 0 &&
		    !crypto_bignum_is_zero(scalar) &&
		    !crypto_bignum_is_one(scalar))
			return 0;
	}

	/* This should not be reachable in practice if the random number
	 * generation is working. */
	wpa_printf(MSG_INFO,
		   "dragonfly: Unable to get randomness for own scalar");
	return -1;
}


/* res = sqrt(val) */
int dragonfly_sqrt(struct crypto_ec *ec, const struct crypto_bignum *val,
		   struct crypto_bignum *res)
{
	const struct crypto_bignum *prime;
	struct crypto_bignum *tmp, *one;
	int ret = 0;
	u8 prime_bin[DRAGONFLY_MAX_ECC_PRIME_LEN];
	size_t prime_len;

	/* For prime p such that p = 3 mod 4, sqrt(w) = w^((p+1)/4) mod p */

	prime = crypto_ec_get_prime(ec);
	prime_len = crypto_ec_prime_len(ec);
	tmp = crypto_bignum_init();
	one = crypto_bignum_init_uint(1);

	if (crypto_bignum_to_bin(prime, prime_bin, sizeof(prime_bin),
				 prime_len) < 0 ||
	    (prime_bin[prime_len - 1] & 0x03) != 3 ||
	    !tmp || !one ||
	    /* tmp = (p+1)/4 */
	    crypto_bignum_add(prime, one, tmp) < 0 ||
	    crypto_bignum_rshift(tmp, 2, tmp) < 0 ||
	    /* res = sqrt(val) */
	    crypto_bignum_exptmod(val, tmp, prime, res) < 0)
		ret = -1;

	crypto_bignum_deinit(tmp, 0);
	crypto_bignum_deinit(one, 0);
	return ret;
}
