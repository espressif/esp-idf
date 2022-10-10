/*
 * Shared Dragonfly functionality
 * Copyright (c) 2012-2016, Jouni Malinen <j@w1.fi>
 * Copyright (c) 2019, The Linux Foundation
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef DRAGONFLY_H
#define DRAGONFLY_H

#define DRAGONFLY_MAX_ECC_PRIME_LEN 66

struct crypto_bignum;
struct crypto_ec;

int dragonfly_suitable_group(int group, int ecc_only);
unsigned int dragonfly_min_pwe_loop_iter(int group);
int dragonfly_get_random_qr_qnr(const struct crypto_bignum *prime,
				struct crypto_bignum **qr,
				struct crypto_bignum **qnr);
int dragonfly_is_quadratic_residue_blind(struct crypto_ec *ec,
					 const u8 *qr, const u8 *qnr,
					 const struct crypto_bignum *val);
int dragonfly_generate_scalar(const struct crypto_bignum *order,
			      struct crypto_bignum *_rand,
			      struct crypto_bignum *_mask,
			      struct crypto_bignum *scalar);
int dragonfly_sqrt(struct crypto_ec *ec, const struct crypto_bignum *val,
		   struct crypto_bignum *res);

#endif /* DRAGONFLY_H */
