/*
 * Simultaneous authentication of equals
 * Copyright (c) 2012-2016, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifdef CONFIG_WPA3_SAE

#include "utils/includes.h"
#include "utils/common.h"
#include "utils/const_time.h"
#include "crypto/crypto.h"
#include "crypto/sha256.h"
#include "crypto/random.h"
#include "crypto/dh_groups.h"
#include "ieee802_11_defs.h"
#include "sae.h"
#include "dragonfly.h"
#include "esp_wifi_crypto_types.h"
#include "common/defs.h"

int sae_set_group(struct sae_data *sae, int group)
{
	struct sae_temporary_data *tmp;

	sae_clear_data(sae);
	tmp = sae->tmp = os_zalloc(sizeof(*tmp));
	if (tmp == NULL)
		return ESP_FAIL;

	/* First, check if this is an ECC group */
	tmp->ec = crypto_ec_init(group);
	if (tmp->ec) {
		wpa_printf(MSG_DEBUG, "SAE: Selecting supported ECC group %d",
			   group);
		sae->group = group;
		tmp->prime_len = crypto_ec_prime_len(tmp->ec);
		tmp->prime = crypto_ec_get_prime(tmp->ec);
		tmp->order_len = crypto_ec_order_len(tmp->ec);
		tmp->order = crypto_ec_get_order(tmp->ec);
		return ESP_OK;
	}

	/* Not an ECC group, check FFC */
	tmp->dh = dh_groups_get(group);
	if (tmp->dh) {
		wpa_printf(MSG_DEBUG, "SAE: Selecting supported FFC group %d",
			   group);
		sae->group = group;
		tmp->prime_len = tmp->dh->prime_len;
		if (tmp->prime_len > SAE_MAX_PRIME_LEN) {
			sae_clear_data(sae);
			return ESP_FAIL;
		}

		tmp->prime_buf = crypto_bignum_init_set(tmp->dh->prime,
							tmp->prime_len);
		if (tmp->prime_buf == NULL) {
			sae_clear_data(sae);
			return ESP_FAIL;
		}
		tmp->prime = tmp->prime_buf;

		tmp->order_len = tmp->dh->order_len;
		tmp->order_buf = crypto_bignum_init_set(tmp->dh->order,
							tmp->dh->order_len);
		if (tmp->order_buf == NULL) {
			sae_clear_data(sae);
			return ESP_FAIL;
		}
		tmp->order = tmp->order_buf;

		return ESP_OK;
	}

	/* Unsupported group */
	wpa_printf(MSG_DEBUG,
		   "SAE: Group %d not supported by the crypto library", group);
	return ESP_FAIL;
}


void sae_clear_temp_data(struct sae_data *sae)
{
	struct sae_temporary_data *tmp;
	if (sae == NULL || sae->tmp == NULL)
		return;
	tmp = sae->tmp;
	crypto_ec_deinit(tmp->ec);
	crypto_bignum_deinit(tmp->prime_buf, 0);
	crypto_bignum_deinit(tmp->order_buf, 0);
	crypto_bignum_deinit(tmp->sae_rand, 1);
	crypto_bignum_deinit(tmp->pwe_ffc, 1);
	crypto_bignum_deinit(tmp->own_commit_scalar, 0);
	crypto_bignum_deinit(tmp->own_commit_element_ffc, 0);
	crypto_bignum_deinit(tmp->peer_commit_element_ffc, 0);
	crypto_ec_point_deinit(tmp->pwe_ecc, 1);
	crypto_ec_point_deinit(tmp->own_commit_element_ecc, 0);
	crypto_ec_point_deinit(tmp->peer_commit_element_ecc, 0);
	wpabuf_free(tmp->anti_clogging_token);
	wpabuf_free(tmp->own_rejected_groups);
	wpabuf_free(tmp->peer_rejected_groups);
	os_free(tmp->pw_id);
	bin_clear_free(tmp, sizeof(*tmp));
	sae->tmp = NULL;
}

void sae_clear_data(struct sae_data *sae)
{
	if (sae == NULL)
		return;
	sae_clear_temp_data(sae);
	crypto_bignum_deinit(sae->peer_commit_scalar, 0);
	crypto_bignum_deinit(sae->peer_commit_scalar_accepted, 0);
	os_memset(sae, 0, sizeof(*sae));
}


static void sae_pwd_seed_key(const u8 *addr1, const u8 *addr2, u8 *key)
{
	wpa_printf(MSG_DEBUG, "SAE: PWE derivation - addr1=" MACSTR
		   " addr2=" MACSTR, MAC2STR(addr1), MAC2STR(addr2));
	if (os_memcmp(addr1, addr2, ETH_ALEN) > 0) {
		os_memcpy(key, addr1, ETH_ALEN);
		os_memcpy(key + ETH_ALEN, addr2, ETH_ALEN);
	} else {
		os_memcpy(key, addr2, ETH_ALEN);
		os_memcpy(key + ETH_ALEN, addr1, ETH_ALEN);
	}
}

static int sae_test_pwd_seed_ecc(struct sae_data *sae, const u8 *pwd_seed,
				 const u8 *prime, const u8 *qr, const u8 *qnr,
				 u8 *pwd_value)
{
	struct crypto_bignum *y_sqr, *x_cand;
	int res;
	size_t bits;
	int cmp_prime;
	unsigned int in_range;

	wpa_hexdump_key(MSG_MSGDUMP, "SAE: pwd-seed", pwd_seed, SHA256_MAC_LEN);

	/* pwd-value = KDF-z(pwd-seed, "SAE Hunting and Pecking", p) */
	bits = crypto_ec_prime_len_bits(sae->tmp->ec);
	if (sha256_prf_bits(pwd_seed, SHA256_MAC_LEN, "SAE Hunting and Pecking",
			    prime, sae->tmp->prime_len, pwd_value, bits) < 0)
		return ESP_FAIL;
	if (bits % 8)
		buf_shift_right(pwd_value, sae->tmp->prime_len, 8 - bits % 8);
	wpa_hexdump_key(MSG_MSGDUMP, "SAE: pwd-value",
			pwd_value, sae->tmp->prime_len);

	cmp_prime = const_time_memcmp(pwd_value, prime, sae->tmp->prime_len);
	/* Create a const_time mask for selection based on prf result
	 * being smaller than prime. */
	in_range = const_time_fill_msb((unsigned int) cmp_prime);
	/* The algorithm description would skip the next steps if
	 * cmp_prime >= 0 (return 0 here), but go through them regardless to
	 * minimize externally observable differences in behavior. */

	x_cand = crypto_bignum_init_set(pwd_value, sae->tmp->prime_len);
	if (!x_cand)
		return ESP_FAIL;
	y_sqr = crypto_ec_point_compute_y_sqr(sae->tmp->ec, x_cand);
	crypto_bignum_deinit(x_cand, 1);
	if (!y_sqr)
		return ESP_FAIL;

	res = dragonfly_is_quadratic_residue_blind(sae->tmp->ec, qr, qnr,
                           y_sqr);
	crypto_bignum_deinit(y_sqr, 1);

	if (res < 0) {
		return res;
	}
	return const_time_select_int(in_range, res, 0);
}


/* Returns -1 on fatal failure, 0 if PWE cannot be derived from the provided
 * pwd-seed, or 1 if a valid PWE was derived from pwd-seed. */
static int sae_test_pwd_seed_ffc(struct sae_data *sae, const u8 *pwd_seed,
				 struct crypto_bignum *pwe)
{
	u8 pwd_value[SAE_MAX_PRIME_LEN];
	size_t bits = sae->tmp->prime_len * 8;
	u8 exp[1];
	struct crypto_bignum *a, *b = NULL;
	int res, is_val;
	u8 pwd_value_valid;

	wpa_hexdump_key(MSG_DEBUG, "SAE: pwd-seed", pwd_seed, SHA256_MAC_LEN);

	/* pwd-value = KDF-z(pwd-seed, "SAE Hunting and Pecking", p) */
	if (sha256_prf_bits(pwd_seed, SHA256_MAC_LEN, "SAE Hunting and Pecking",
			    sae->tmp->dh->prime, sae->tmp->prime_len, pwd_value,
			    bits) < 0)
		return ESP_FAIL;
	wpa_hexdump_key(MSG_DEBUG, "SAE: pwd-value", pwd_value,
			sae->tmp->prime_len);

	/* Check whether pwd-value < p */
	res = const_time_memcmp(pwd_value, sae->tmp->dh->prime,
				sae->tmp->prime_len);
	/* pwd-value >= p is invalid, so res is < 0 for the valid cases and
	 * the negative sign can be used to fill the mask for constant time
	 * selection */
	pwd_value_valid = const_time_fill_msb(res);

	/* If pwd-value >= p, force pwd-value to be < p and perform the
	 * calculations anyway to hide timing difference. The derived PWE will
	 * be ignored in that case. */
	pwd_value[0] = const_time_select_u8(pwd_value_valid, pwd_value[0], 0);

	/* PWE = pwd-value^((p-1)/r) modulo p */

	res = -1;
	a = crypto_bignum_init_set(pwd_value, sae->tmp->prime_len);

	if (!a)
		goto fail;

	/* This is an optimization based on the used group that does not depend
	 * on the password in any way, so it is fine to use separate branches
	 * for this step without constant time operations. */
	if (sae->tmp->dh->safe_prime) {
		/*
		 * r = (p-1)/2 for the group used here, so this becomes:
		 * PWE = pwd-value^2 modulo p
		 */
		exp[0] = 2;
		b = crypto_bignum_init_set(exp, sizeof(exp));
	} else {
		/* Calculate exponent: (p-1)/r */
		exp[0] = 1;
		b = crypto_bignum_init_set(exp, sizeof(exp));
		if (b == NULL ||
		    crypto_bignum_sub(sae->tmp->prime, b, b) < 0 ||
		    crypto_bignum_div(b, sae->tmp->order, b) < 0)
		    goto fail;
	}


	if (!b)
		goto fail;

	res = crypto_bignum_exptmod(a, b, sae->tmp->prime, pwe);
	if (res < 0)
		goto fail;

	/* There were no fatal errors in calculations, so determine the return
	 * value using constant time operations. We get here for number of
	 * invalid cases which are cleared here after having performed all the
	 * computation. PWE is valid if pwd-value was less than prime and
	 * PWE > 1. Start with pwd-value check first and then use constant time
	 * operations to clear res to 0 if PWE is 0 or 1.
	 */
	res = const_time_select_u8(pwd_value_valid, 1, 0);
	is_val = crypto_bignum_is_zero(pwe);
	res = const_time_select_u8(const_time_is_zero(is_val), res, 0);
	is_val = crypto_bignum_is_one(pwe);
	res = const_time_select_u8(const_time_is_zero(is_val), res, 0);

fail:
	crypto_bignum_deinit(a, 1);
	crypto_bignum_deinit(b, 1);
	return res;
}


static int sae_derive_pwe_ecc(struct sae_data *sae, const u8 *addr1,
			      const u8 *addr2, const u8 *password,
			      size_t password_len)
{
	u8 counter, k;
	u8 addrs[2 * ETH_ALEN];
	const u8 *addr[2];
	size_t len[2];
	u8 *stub_password, *tmp_password;
	int pwd_seed_odd = 0;
	u8 prime[SAE_MAX_ECC_PRIME_LEN];
	size_t prime_len;
	struct crypto_bignum *x = NULL, *y = NULL, *qr = NULL, *qnr = NULL;
	u8 x_bin[SAE_MAX_ECC_PRIME_LEN];
	u8 x_cand_bin[SAE_MAX_ECC_PRIME_LEN];
	u8 qr_bin[SAE_MAX_ECC_PRIME_LEN];
	u8 qnr_bin[SAE_MAX_ECC_PRIME_LEN];
	u8 x_y[2 * SAE_MAX_ECC_PRIME_LEN];
	int res = -1;
	u8 found = 0; /* 0 (false) or 0xff (true) to be used as const_time_*
		       * mask */
	unsigned int is_eq;

	os_memset(x_bin, 0, sizeof(x_bin));

	stub_password = os_malloc(password_len);
	tmp_password = os_malloc(password_len);
	if (!stub_password || !tmp_password ||
	    random_get_bytes(stub_password, password_len) < 0)
		goto fail;

	prime_len = sae->tmp->prime_len;
	if (crypto_bignum_to_bin(sae->tmp->prime, prime, sizeof(prime),
				 prime_len) < 0)
		goto fail;

	/*
	 * Create a random quadratic residue (qr) and quadratic non-residue
	 * (qnr) modulo p for blinding purposes during the loop.
	 */
	if (dragonfly_get_random_qr_qnr(sae->tmp->prime, &qr, &qnr) < 0 ||
	    crypto_bignum_to_bin(qr, qr_bin, sizeof(qr_bin), prime_len) < 0 ||
	    crypto_bignum_to_bin(qnr, qnr_bin, sizeof(qnr_bin), prime_len) < 0)
		goto fail;

	wpa_hexdump_ascii_key(MSG_DEBUG, "SAE: password",
			      password, password_len);

	/*
	 * H(salt, ikm) = HMAC-SHA256(salt, ikm)
	 * base = password
	 * pwd-seed = H(MAX(STA-A-MAC, STA-B-MAC) || MIN(STA-A-MAC, STA-B-MAC),
	 *              base || counter)
	 */
	sae_pwd_seed_key(addr1, addr2, addrs);

	addr[0] = tmp_password;
	len[0] = password_len;
	addr[1] = &counter;
	len[1] = sizeof(counter);

	k = dragonfly_min_pwe_loop_iter(sae->group);

	/*
	 * Continue for at least k iterations to protect against side-channel
	 * attacks that attempt to determine the number of iterations required
	 * in the loop.
	 */
	for (counter = 1; counter <= k || !found; counter++) {
		u8 pwd_seed[SHA256_MAC_LEN];

		if (counter > 200) {
			/* This should not happen in practice */
			wpa_printf(MSG_DEBUG, "SAE: Failed to derive PWE");
			break;
		}

		wpa_printf(MSG_MSGDUMP, "SAE: counter = %03u", counter);
		const_time_select_bin(found, stub_password, password,
				      password_len, tmp_password);
		if (hmac_sha256_vector(addrs, sizeof(addrs), 2,
				       addr, len, pwd_seed) < 0)
			break;

		res = sae_test_pwd_seed_ecc(sae, pwd_seed,
					    prime, qr_bin, qnr_bin, x_cand_bin);
		const_time_select_bin(found, x_bin, x_cand_bin, prime_len,
				      x_bin);
		pwd_seed_odd = const_time_select_u8(
			found, pwd_seed_odd,
			pwd_seed[SHA256_MAC_LEN - 1] & 0x01);
		os_memset(pwd_seed, 0, sizeof(pwd_seed));
		if (res < 0)
			goto fail;
		/* Need to minimize differences in handling res == 0 and 1 here
		 * to avoid differences in timing and instruction cache access,
		 * so use const_time_select_*() to make local copies of the
		 * values based on whether this loop iteration was the one that
		 * found the pwd-seed/x. */

		/* found is 0 or 0xff here and res is 0 or 1. Bitwise OR of them
		 * (with res converted to 0/0xff) handles this in constant time.
		 */
		found |= res * 0xff;
		wpa_printf(MSG_MSGDUMP, "SAE: pwd-seed result %d found=0x%02x",
			   res, found);
	}

	if (!found) {
		wpa_printf(MSG_DEBUG, "SAE: Could not generate PWE");
		res = -1;
		goto fail;
	}

	x = crypto_bignum_init_set(x_bin, prime_len);
	if (!x) {
		res = -1;
		goto fail;
	}

	/* y = sqrt(x^3 + ax + b) mod p
	 * if LSB(save) == LSB(y): PWE = (x, y)
	 * else: PWE = (x, p - y)
	 *
	 * Calculate y and the two possible values for PWE and after that,
	 * use constant time selection to copy the correct alternative.
	 */
	y = crypto_ec_point_compute_y_sqr(sae->tmp->ec, x);
	if (!y ||
	    dragonfly_sqrt(sae->tmp->ec, y, y) < 0 ||
	    crypto_bignum_to_bin(y, x_y, SAE_MAX_ECC_PRIME_LEN,
				 prime_len) < 0 ||
	    crypto_bignum_sub(sae->tmp->prime, y, y) < 0 ||
	    crypto_bignum_to_bin(y, x_y + SAE_MAX_ECC_PRIME_LEN,
				 SAE_MAX_ECC_PRIME_LEN, prime_len) < 0) {
		wpa_printf(MSG_DEBUG, "SAE: Could not solve y");
		goto fail;
	}

	is_eq = const_time_eq(pwd_seed_odd, x_y[prime_len - 1] & 0x01);
	const_time_select_bin(is_eq, x_y, x_y + SAE_MAX_ECC_PRIME_LEN,
			      prime_len, x_y + prime_len);
	os_memcpy(x_y, x_bin, prime_len);
	wpa_hexdump_key(MSG_DEBUG, "SAE: PWE", x_y, 2 * prime_len);
	crypto_ec_point_deinit(sae->tmp->pwe_ecc, 1);
	sae->tmp->pwe_ecc = crypto_ec_point_from_bin(sae->tmp->ec, x_y);
	if (!sae->tmp->pwe_ecc) {
		wpa_printf(MSG_DEBUG, "SAE: Could not generate PWE");
		res = -1;
	}

fail:
	forced_memzero(x_y, sizeof(x_y));
	crypto_bignum_deinit(qr, 0);
	crypto_bignum_deinit(qnr, 0);
	crypto_bignum_deinit(y, 1);
	os_free(stub_password);
	bin_clear_free(tmp_password, password_len);
	crypto_bignum_deinit(x, 1);
	os_memset(x_bin, 0, sizeof(x_bin));
	os_memset(x_cand_bin, 0, sizeof(x_cand_bin));
	return res;
}


static int sae_derive_pwe_ffc(struct sae_data *sae, const u8 *addr1,
			      const u8 *addr2, const u8 *password,
			      size_t password_len)
{
	u8 counter, k, sel_counter = 0;
	u8 addrs[2 * ETH_ALEN];
	const u8 *addr[2];
	size_t len[2];
	u8 found = 0; /* 0 (false) or 0xff (true) to be used as const_time_*
		       * mask */
	u8 mask;
	struct crypto_bignum *pwe;
	size_t prime_len = sae->tmp->prime_len * 8;
	u8 *pwe_buf;
	crypto_bignum_deinit(sae->tmp->pwe_ffc, 1);
	sae->tmp->pwe_ffc = NULL;

	/* Allocate a buffer to maintain selected and candidate PWE for constant
	 * time selection. */
	pwe_buf = os_zalloc(prime_len * 2);
	pwe = crypto_bignum_init();
	if (!pwe_buf || !pwe)
		goto fail;

	wpa_hexdump_ascii_key(MSG_DEBUG, "SAE: password",
			      password, password_len);

	/*
	 * H(salt, ikm) = HMAC-SHA256(salt, ikm)
	 * pwd-seed = H(MAX(STA-A-MAC, STA-B-MAC) || MIN(STA-A-MAC, STA-B-MAC),
	 *              password || counter)
	 */
	sae_pwd_seed_key(addr1, addr2, addrs);

	addr[0] = password;
	len[0] = password_len;
	addr[1] = &counter;
	len[1] = sizeof(counter);

	k = dragonfly_min_pwe_loop_iter(sae->group);

	for (counter = 1; counter <= k || !found; counter++) {
        u8 pwd_seed[SHA256_MAC_LEN];
		int res;

		if (counter > 200) {
			/* This should not happen in practice */
			wpa_printf(MSG_DEBUG, "SAE: Failed to derive PWE");
			break;
		}

		wpa_printf(MSG_DEBUG, "SAE: counter = %02u", counter);
		if (hmac_sha256_vector(addrs, sizeof(addrs), 2,
				       addr, len, pwd_seed) < 0)
			break;
		res = sae_test_pwd_seed_ffc(sae, pwd_seed, pwe);
		/* res is -1 for fatal failure, 0 if a valid PWE was not found,
		 * or 1 if a valid PWE was found. */
		if (res < 0)
			break;
		/* Store the candidate PWE into the second half of pwe_buf and
		 * the selected PWE in the beginning of pwe_buf using constant
		 * time selection. */
		if (crypto_bignum_to_bin(pwe, pwe_buf + prime_len, prime_len,
					 prime_len) < 0)
			break;
		const_time_select_bin(found, pwe_buf, pwe_buf + prime_len,
				      prime_len, pwe_buf);
		sel_counter = const_time_select_u8(found, sel_counter, counter);
		mask = const_time_eq_u8(res, 1);
		found = const_time_select_u8(found, found, mask);
	}

	if (!found)
		goto fail;

	wpa_printf(MSG_DEBUG, "SAE: Use PWE from counter = %02u", sel_counter);
	sae->tmp->pwe_ffc = crypto_bignum_init_set(pwe_buf, prime_len);
fail:
	crypto_bignum_deinit(pwe, 1);
	bin_clear_free(pwe_buf, prime_len * 2);
	return sae->tmp->pwe_ffc ? 0 : -1;
}


static int hkdf_extract(size_t hash_len, const u8 *salt, size_t salt_len,
			size_t num_elem, const u8 *addr[], const size_t len[],
			u8 *prk)
{
	if (hash_len == 32)
		return hmac_sha256_vector(salt, salt_len, num_elem, addr, len,
					  prk);
	return -1;
}


static int hkdf_expand(size_t hash_len, const u8 *prk, size_t prk_len,
		       const char *info, u8 *okm, size_t okm_len)
{
	size_t info_len = os_strlen(info);

	if (hash_len == 32)
		return hmac_sha256_kdf(prk, prk_len, NULL,
				       (const u8 *) info, info_len,
				       okm, okm_len);
	return -1;
}


static int sswu_curve_param(int group, int *z)
{
	switch (group) {
	case 19:
		*z = -10;
		return 0;
	}

	return -1;
}


static void debug_print_bignum(const char *title, const struct crypto_bignum *a,
			       size_t prime_len)
{
	u8 *bin;

	bin = os_malloc(prime_len);
	if (bin && crypto_bignum_to_bin(a, bin, prime_len, prime_len) >= 0)
		wpa_hexdump_key(MSG_DEBUG, title, bin, prime_len);
	else
		wpa_printf(MSG_DEBUG, "Could not print bignum (%s)", title);
	bin_clear_free(bin, prime_len);
}

static struct crypto_ec_point * sswu(struct crypto_ec *ec, int group,
				     const struct crypto_bignum *u)
{
	int z_int;
	const struct crypto_bignum *b, *prime;
	struct crypto_bignum *a, *u2, *t1, *t2, *z, *t, *zero, *one, *two, *three,
		*x1a, *x1b, *y = NULL;
	struct crypto_bignum *x1 = NULL, *x2, *gx1, *gx2, *v = NULL;
	struct crypto_bignum *tmp = NULL;
	unsigned int m_is_zero, is_qr, is_eq;
	size_t prime_len;
	u8 bin[SAE_MAX_ECC_PRIME_LEN];
	u8 bin1[SAE_MAX_ECC_PRIME_LEN];
	u8 bin2[SAE_MAX_ECC_PRIME_LEN];
	u8 x_y[2 * SAE_MAX_ECC_PRIME_LEN];
	struct crypto_ec_point *p = NULL;

	if (sswu_curve_param(group, &z_int) < 0)
		return NULL;

	prime = crypto_ec_get_prime(ec);
	prime_len = crypto_ec_prime_len(ec);
	/* Value of 'a' defined for curve secp256r1 in 'y^2 = x^3 + ax + b' */
	uint8_t buf[32] = {0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc};
	a = crypto_bignum_init_set(buf, 32);
	b = crypto_ec_get_b(ec);

	u2 = crypto_bignum_init();
	t1 = crypto_bignum_init();
	t2 = crypto_bignum_init();
	z = crypto_bignum_init_uint(abs(z_int));
	t = crypto_bignum_init();
	zero = crypto_bignum_init_uint(0);
	one = crypto_bignum_init_uint(1);
	two = crypto_bignum_init_uint(2);
	three = crypto_bignum_init_uint(3);
	x1a = crypto_bignum_init();
	x1b = crypto_bignum_init();
	x2 = crypto_bignum_init();
	gx1 = crypto_bignum_init();
	gx2 = crypto_bignum_init();
	tmp = crypto_bignum_init();
	if (!u2 || !t1 || !t2 || !z || !t || !zero || !one || !two || !three ||
	    !x1a || !x1b || !x2 || !gx1 || !gx2 || !tmp)
		goto fail;

	if (z_int < 0 && crypto_bignum_sub(prime, z, z) < 0)
		goto fail;

	/* m = z^2 * u^4 + z * u^2 */
	/* --> tmp = z * u^2, m = tmp^2 + tmp */

	/* u2 = u^2
	 * t1 = z * u2
	 * t2 = t1^2
	 * m = t1 = t1 + t2 */
	if (crypto_bignum_sqrmod(u, prime, u2) < 0 ||
	    crypto_bignum_mulmod(z, u2, prime, t1) < 0 ||
	    crypto_bignum_sqrmod(t1, prime, t2) < 0 ||
	    crypto_bignum_addmod(t1, t2, prime, t1) < 0)
		goto fail;
	debug_print_bignum("SSWU: m", t1, prime_len);

	/* l = CEQ(m, 0)
	 * t = CSEL(l, 0, inverse(m); where inverse(x) is calculated as
	 * x^(p-2) modulo p which will handle m == 0 case correctly */
	/* TODO: Make sure crypto_bignum_is_zero() is constant time */
	m_is_zero = const_time_eq(crypto_bignum_is_zero(t1), 1);
	/* t = m^(p-2) modulo p */
	if (crypto_bignum_sub(prime, two, t2) < 0 ||
	    crypto_bignum_exptmod(t1, t2, prime, t) < 0)
		goto fail;
	debug_print_bignum("SSWU: t", t, prime_len);

	/* b / (z * a) */
	if (crypto_bignum_mulmod(z, a, prime, t1) < 0 ||
	    crypto_bignum_inverse(t1, prime, t1) < 0 ||
	    crypto_bignum_mulmod(b, t1, prime, x1a) < 0)
		goto fail;
	debug_print_bignum("SSWU: x1a = b / (z * a)", x1a, prime_len);

	/* (-b/a) * (1 + t) */
	if (crypto_bignum_sub(prime, b, t1) < 0 ||
	    crypto_bignum_inverse(a, prime, t2) < 0 ||
	    crypto_bignum_mulmod(t1, t2, prime, t1) < 0 ||
	    crypto_bignum_addmod(one, t, prime, t2) < 0 ||
	    crypto_bignum_mulmod(t1, t2, prime, x1b) < 0)
		goto fail;
	debug_print_bignum("SSWU: x1b = (-b/a) * (1 + t)", x1b, prime_len);

	/* x1 = CSEL(CEQ(m, 0), x1a, x1b) */
	if (crypto_bignum_to_bin(x1a, bin1, sizeof(bin1), prime_len) < 0 ||
	    crypto_bignum_to_bin(x1b, bin2, sizeof(bin2), prime_len) < 0)
		goto fail;
	const_time_select_bin(m_is_zero, bin1, bin2, prime_len, bin);
	x1 = crypto_bignum_init_set(bin, prime_len);
	if (!x1)
		goto fail;
	debug_print_bignum("SSWU: x1 = CSEL(l, x1a, x1b)", x1, prime_len);

	/* gx1 = x1^3 + a * x1 + b */
	if (crypto_bignum_exptmod(x1, three, prime, t1) < 0 ||
	    crypto_bignum_mulmod(a, x1, prime, t2) < 0 ||
	    crypto_bignum_addmod(t1, t2, prime, t1) < 0 ||
	    crypto_bignum_addmod(t1, b, prime, gx1) < 0)
		goto fail;
	debug_print_bignum("SSWU: gx1 = x1^3 + a * x1 + b", gx1, prime_len);

	/* x2 = z * u^2 * x1 */
	if (crypto_bignum_mulmod(z, u2, prime, t1) < 0 ||
	    crypto_bignum_mulmod(t1, x1, prime, x2) < 0)
		goto fail;
	debug_print_bignum("SSWU: x2 = z * u^2 * x1", x2, prime_len);

	/* gx2 = x2^3 + a * x2 + b */
	if (crypto_bignum_exptmod(x2, three, prime, t1) < 0 ||
	    crypto_bignum_mulmod(a, x2, prime, t2) < 0 ||
	    crypto_bignum_addmod(t1, t2, prime, t1) < 0 ||
	    crypto_bignum_addmod(t1, b, prime, gx2) < 0)
		goto fail;
	debug_print_bignum("SSWU: gx2 = x2^3 + a * x2 + b", gx2, prime_len);

	/* l = gx1 is a quadratic residue modulo p
	 * --> gx1^((p-1)/2) modulo p is zero or one */
	if (crypto_bignum_sub(prime, one, t1) < 0 ||
	    crypto_bignum_rshift(t1, 1, t1) < 0 ||
	    crypto_bignum_exptmod(gx1, t1, prime, tmp) < 0)
		goto fail;
	debug_print_bignum("SSWU: gx1^((p-1)/2) modulo p", t1, prime_len);
	is_qr = const_time_eq(crypto_bignum_is_zero(tmp) |
			      crypto_bignum_is_one(tmp), 1);

	/* v = CSEL(l, gx1, gx2) */
	if (crypto_bignum_to_bin(gx1, bin1, sizeof(bin1), prime_len) < 0 ||
	    crypto_bignum_to_bin(gx2, bin2, sizeof(bin2), prime_len) < 0)
		goto fail;
	const_time_select_bin(is_qr, bin1, bin2, prime_len, bin);
	v = crypto_bignum_init_set(bin, prime_len);
	if (!v)
		goto fail;
	debug_print_bignum("SSWU: v = CSEL(l, gx1, gx2)", v, prime_len);

	/* x = CSEL(l, x1, x2) */
	if (crypto_bignum_to_bin(x1, bin1, sizeof(bin1), prime_len) < 0 ||
	    crypto_bignum_to_bin(x2, bin2, sizeof(bin2), prime_len) < 0)
		goto fail;
	const_time_select_bin(is_qr, bin1, bin2, prime_len, x_y);
	wpa_hexdump_key(MSG_DEBUG, "SSWU: x = CSEL(l, x1, x2)", x_y, prime_len);

	/* y = sqrt(v) */
	y = crypto_bignum_init();
	if (!y || dragonfly_sqrt(ec, v, y) < 0)
		goto fail;
	debug_print_bignum("SSWU: y = sqrt(v)", y, prime_len);

	/* l = CEQ(LSB(u), LSB(y)) */
	if (crypto_bignum_to_bin(u, bin1, sizeof(bin1), prime_len) < 0 ||
	    crypto_bignum_to_bin(y, bin2, sizeof(bin2), prime_len) < 0)
		goto fail;
	is_eq = const_time_eq(bin1[prime_len - 1] & 0x01,
			      bin2[prime_len - 1] & 0x01);

	/* P = CSEL(l, (x,y), (x, p-y)) */
	if (crypto_bignum_sub(prime, y, t1) < 0)
		goto fail;
	debug_print_bignum("SSWU: p - y", t1, prime_len);
	if (crypto_bignum_to_bin(y, bin1, sizeof(bin1), prime_len) < 0 ||
	    crypto_bignum_to_bin(t1, bin2, sizeof(bin2), prime_len) < 0)
		goto fail;
	const_time_select_bin(is_eq, bin1, bin2, prime_len, &x_y[prime_len]);

	/* output P */
	wpa_hexdump_key(MSG_DEBUG, "SSWU: P.x", x_y, prime_len);
	wpa_hexdump_key(MSG_DEBUG, "SSWU: P.y", &x_y[prime_len], prime_len);
	p = crypto_ec_point_from_bin(ec, x_y);

fail:
	crypto_bignum_deinit(a, 0);
	crypto_bignum_deinit(tmp, 0);
	crypto_bignum_deinit(u2, 1);
	crypto_bignum_deinit(t1, 1);
	crypto_bignum_deinit(t2, 1);
	crypto_bignum_deinit(z, 0);
	crypto_bignum_deinit(t, 1);
	crypto_bignum_deinit(x1a, 1);
	crypto_bignum_deinit(x1b, 1);
	crypto_bignum_deinit(x1, 1);
	crypto_bignum_deinit(x2, 1);
	crypto_bignum_deinit(gx1, 1);
	crypto_bignum_deinit(gx2, 1);
	crypto_bignum_deinit(y, 1);
	crypto_bignum_deinit(v, 1);
	crypto_bignum_deinit(zero, 0);
	crypto_bignum_deinit(one, 0);
	crypto_bignum_deinit(two, 0);
	crypto_bignum_deinit(three, 0);
	forced_memzero(bin, sizeof(bin));
	forced_memzero(bin1, sizeof(bin1));
	forced_memzero(bin2, sizeof(bin2));
	forced_memzero(x_y, sizeof(x_y));
	return p;
}


static int sae_pwd_seed(size_t hash_len, const u8 *ssid, size_t ssid_len,
			const u8 *password, size_t password_len,
			const char *identifier, u8 *pwd_seed)
{
	const u8 *addr[2];
	size_t len[2];
	size_t num_elem;

	/* pwd-seed = HKDF-Extract(ssid, password [ || identifier ]) */
	addr[0] = password;
	len[0] = password_len;
	num_elem = 1;
	wpa_hexdump_ascii(MSG_DEBUG, "SAE: SSID", ssid, ssid_len);
	wpa_hexdump_ascii_key(MSG_DEBUG, "SAE: password",
			      password, password_len);
	if (identifier) {
		wpa_printf(MSG_DEBUG, "SAE: password identifier: %s",
			   identifier);
		addr[num_elem] = (const u8 *) identifier;
		len[num_elem] = os_strlen(identifier);
		num_elem++;
	}
	if (hkdf_extract(hash_len, ssid, ssid_len, num_elem, addr, len,
			 pwd_seed) < 0)
		return -1;
	wpa_hexdump_key(MSG_DEBUG, "SAE: pwd-seed", pwd_seed, hash_len);
	return 0;
}


size_t sae_ecc_prime_len_2_hash_len(size_t prime_len)
{
	if (prime_len <= 256 / 8)
		return 32;
	if (prime_len <= 384 / 8)
		return 48;
	return 64;
}


static struct crypto_ec_point *
sae_derive_pt_ecc(struct crypto_ec *ec, int group,
		  const u8 *ssid, size_t ssid_len,
		  const u8 *password, size_t password_len,
		  const char *identifier)
{
	u8 pwd_seed[64];
	u8 pwd_value[SAE_MAX_ECC_PRIME_LEN * 2];
	size_t pwd_value_len, hash_len, prime_len;
	const struct crypto_bignum *prime;
	struct crypto_bignum *bn = NULL;
	struct crypto_ec_point *p1 = NULL, *p2 = NULL, *pt = NULL;

	prime = crypto_ec_get_prime(ec);
	prime_len = crypto_ec_prime_len(ec);
	if (prime_len > SAE_MAX_ECC_PRIME_LEN)
		goto fail;
	hash_len = sae_ecc_prime_len_2_hash_len(prime_len);

	/* len = olen(p) + ceil(olen(p)/2) */
	pwd_value_len = prime_len + (prime_len + 1) / 2;

	if (sae_pwd_seed(hash_len, ssid, ssid_len, password, password_len,
			 identifier, pwd_seed) < 0)
		goto fail;

	/* pwd-value = HKDF-Expand(pwd-seed, "SAE Hash to Element u1 P1", len)
	 */
	if (hkdf_expand(hash_len, pwd_seed, hash_len,
			"SAE Hash to Element u1 P1", pwd_value, pwd_value_len) <
	    0)
		goto fail;
	wpa_hexdump_key(MSG_DEBUG, "SAE: pwd-value (u1 P1)",
			pwd_value, pwd_value_len);

	/* u1 = pwd-value modulo p */
	bn = crypto_bignum_init_set(pwd_value, pwd_value_len);
	if (!bn || crypto_bignum_mod(bn, prime, bn) < 0 ||
	    crypto_bignum_to_bin(bn, pwd_value, sizeof(pwd_value),
				 prime_len) < 0)
		goto fail;
	wpa_hexdump_key(MSG_DEBUG, "SAE: u1", pwd_value, prime_len);

	/* P1 = SSWU(u1) */
	p1 = sswu(ec, group, bn);
	if (!p1)
		goto fail;

	/* pwd-value = HKDF-Expand(pwd-seed, "SAE Hash to Element u2 P2", len)
	 */
	if (hkdf_expand(hash_len, pwd_seed, hash_len,
			"SAE Hash to Element u2 P2", pwd_value,
			pwd_value_len) < 0)
		goto fail;
	wpa_hexdump_key(MSG_DEBUG, "SAE: pwd-value (u2 P2)",
			pwd_value, pwd_value_len);

	/* u2 = pwd-value modulo p */
	crypto_bignum_deinit(bn, 1);
	bn = crypto_bignum_init_set(pwd_value, pwd_value_len);
	if (!bn || crypto_bignum_mod(bn, prime, bn) < 0 ||
	    crypto_bignum_to_bin(bn, pwd_value, sizeof(pwd_value),
				 prime_len) < 0)
		goto fail;
	wpa_hexdump_key(MSG_DEBUG, "SAE: u2", pwd_value, prime_len);

	/* P2 = SSWU(u2) */
	p2 = sswu(ec, group, bn);
	if (!p2)
		goto fail;

	/* PT = elem-op(P1, P2) */
	pt = crypto_ec_point_init(ec);
	if (!pt)
		goto fail;
	if (crypto_ec_point_add(ec, p1, p2, pt) < 0) {
		crypto_ec_point_deinit(pt, 1);
		pt = NULL;
	}

fail:
	forced_memzero(pwd_seed, sizeof(pwd_seed));
	forced_memzero(pwd_value, sizeof(pwd_value));
	crypto_bignum_deinit(bn, 1);
	crypto_ec_point_deinit(p1, 1);
	crypto_ec_point_deinit(p2, 1);
	return pt;
}


size_t sae_ffc_prime_len_2_hash_len(size_t prime_len)
{
	if (prime_len <= 2048 / 8)
		return 32;
	if (prime_len <= 3072 / 8)
		return 48;
	return 64;
}


static struct crypto_bignum *
sae_derive_pt_ffc(const struct dh_group *dh, int group,
		  const u8 *ssid, size_t ssid_len,
		  const u8 *password, size_t password_len,
		  const char *identifier)
{
	size_t hash_len, prime_len, pwd_value_len;
	struct crypto_bignum *prime, *order;
	struct crypto_bignum *one = NULL, *two = NULL, *bn = NULL, *tmp = NULL,
		*pt = NULL;
	u8 pwd_seed[64];
	u8 pwd_value[SAE_MAX_PRIME_LEN + SAE_MAX_PRIME_LEN / 2];

	prime = crypto_bignum_init_set(dh->prime, dh->prime_len);
	order = crypto_bignum_init_set(dh->order, dh->order_len);
	if (!prime || !order)
		goto fail;
	prime_len = dh->prime_len;
	if (prime_len > SAE_MAX_PRIME_LEN)
		goto fail;
	hash_len = sae_ffc_prime_len_2_hash_len(prime_len);

	/* len = olen(p) + ceil(olen(p)/2) */
	pwd_value_len = prime_len + (prime_len + 1) / 2;
	if (pwd_value_len > sizeof(pwd_value))
		goto fail;

	if (sae_pwd_seed(hash_len, ssid, ssid_len, password, password_len,
			 identifier, pwd_seed) < 0)
		goto fail;

	/* pwd-value = HKDF-Expand(pwd-seed, "SAE Hash to Element", len) */
	if (hkdf_expand(hash_len, pwd_seed, hash_len,
			"SAE Hash to Element", pwd_value, pwd_value_len) < 0)
		goto fail;
	wpa_hexdump_key(MSG_DEBUG, "SAE: pwd-value",
			pwd_value, pwd_value_len);

	/* pwd-value = (pwd-value modulo (p-2)) + 2 */
	bn = crypto_bignum_init_set(pwd_value, pwd_value_len);
	one = crypto_bignum_init_uint(1);
	two = crypto_bignum_init_uint(2);
	tmp = crypto_bignum_init();
	if (!bn || !one || !two || !tmp ||
	    crypto_bignum_sub(prime, two, tmp) < 0 ||
	    crypto_bignum_mod(bn, tmp, bn) < 0 ||
	    crypto_bignum_add(bn, two, bn) < 0 ||
	    crypto_bignum_to_bin(bn, pwd_value, sizeof(pwd_value),
				 prime_len) < 0)
		goto fail;
	wpa_hexdump_key(MSG_DEBUG, "SAE: pwd-value(reduced)",
			pwd_value, prime_len);

	/* PT = pwd-value^((p-1)/q) modulo p */
	pt = crypto_bignum_init();
	if (!pt ||
	    crypto_bignum_sub(prime, one, tmp) < 0 ||
	    crypto_bignum_div(tmp, order, tmp) < 0 ||
	    crypto_bignum_exptmod(bn, tmp, prime, pt) < 0) {
		crypto_bignum_deinit(pt, 1);
		pt = NULL;
		goto fail;
	}
	debug_print_bignum("SAE: PT", pt, prime_len);

fail:
	forced_memzero(pwd_seed, sizeof(pwd_seed));
	forced_memzero(pwd_value, sizeof(pwd_value));
	crypto_bignum_deinit(bn, 1);
	crypto_bignum_deinit(tmp, 1);
	crypto_bignum_deinit(one, 0);
	crypto_bignum_deinit(two, 0);
	crypto_bignum_deinit(prime, 0);
	crypto_bignum_deinit(order, 0);
	return pt;
}


static struct sae_pt *
sae_derive_pt_group(int group, const u8 *ssid, size_t ssid_len,
		    const u8 *password, size_t password_len,
		    const char *identifier)
{
	struct sae_pt *pt;

	wpa_printf(MSG_DEBUG, "SAE: Derive PT - group %d", group);

	if (ssid_len > 32)
		return NULL;

	pt = os_zalloc(sizeof(*pt));
	if (!pt)
		return NULL;
#ifdef CONFIG_SAE_PK
	os_memcpy(pt->ssid, ssid, ssid_len);
	pt->ssid_len = ssid_len;
#endif /* CONFIG_SAE_PK */

	pt->group = group;
	pt->ec = crypto_ec_init(group);
	if (pt->ec) {
		pt->ecc_pt = sae_derive_pt_ecc(pt->ec, group, ssid, ssid_len,
					       password, password_len,
					       identifier);
		if (!pt->ecc_pt) {
			wpa_printf(MSG_DEBUG, "SAE: Failed to derive PT");
			goto fail;
		}

		return pt;
	}

	pt->dh = dh_groups_get(group);
	if (!pt->dh) {
		wpa_printf(MSG_DEBUG, "SAE: Unsupported group %d", group);
		goto fail;
	}

	pt->ffc_pt = sae_derive_pt_ffc(pt->dh, group, ssid, ssid_len,
				       password, password_len, identifier);
	if (!pt->ffc_pt) {
		wpa_printf(MSG_DEBUG, "SAE: Failed to derive PT");
		goto fail;
	}

	return pt;
fail:
	sae_deinit_pt(pt);
	return NULL;
}


struct sae_pt * sae_derive_pt(int *groups, const u8 *ssid, size_t ssid_len,
			      const u8 *password, size_t password_len,
			      const char *identifier)
{
	struct sae_pt *pt = NULL, *last = NULL, *tmp;
	int default_groups[] = { 19, 0 };
	int i;

	if (!groups)
		groups = default_groups;
	for (i = 0; groups[i] > 0; i++) {
		tmp = sae_derive_pt_group(groups[i], ssid, ssid_len, password,
					  password_len, identifier);
		if (!tmp)
			continue;

		if (last)
			last->next = tmp;
		else
			pt = tmp;
		last = tmp;
	}

	return pt;
}


static void sae_max_min_addr(const u8 *addr[], size_t len[],
			     const u8 *addr1, const u8 *addr2)
{
	len[0] = ETH_ALEN;
	len[1] = ETH_ALEN;
	if (os_memcmp(addr1, addr2, ETH_ALEN) > 0) {
		addr[0] = addr1;
		addr[1] = addr2;
	} else {
		addr[0] = addr2;
		addr[1] = addr1;
	}
}


struct crypto_ec_point *
sae_derive_pwe_from_pt_ecc(const struct sae_pt *pt,
			   const u8 *addr1, const u8 *addr2)
{
	u8 bin[SAE_MAX_ECC_PRIME_LEN * 2];
	size_t prime_len;
	const u8 *addr[2];
	size_t len[2];
	u8 salt[64], hash[64];
	size_t hash_len;
	const struct crypto_bignum *order;
	struct crypto_bignum *tmp = NULL, *val = NULL, *one = NULL;
	struct crypto_ec_point *pwe = NULL;

	wpa_printf(MSG_DEBUG, "SAE: Derive PWE from PT");
	prime_len = crypto_ec_prime_len(pt->ec);
	if (crypto_ec_point_to_bin(pt->ec, pt->ecc_pt,
				   bin, bin + prime_len) < 0)
		return NULL;
	wpa_hexdump_key(MSG_DEBUG, "SAE: PT.x", bin, prime_len);
	wpa_hexdump_key(MSG_DEBUG, "SAE: PT.y", bin + prime_len, prime_len);

	sae_max_min_addr(addr, len, addr1, addr2);

	/* val = H(0^n,
	 *         MAX(STA-A-MAC, STA-B-MAC) || MIN(STA-A-MAC, STA-B-MAC)) */
	wpa_printf(MSG_DEBUG, "SAE: val = H(0^n, MAX(addrs) || MIN(addrs))");
	hash_len = sae_ecc_prime_len_2_hash_len(prime_len);
	os_memset(salt, 0, hash_len);
	if (hkdf_extract(hash_len, salt, hash_len, 2, addr, len, hash) < 0)
		goto fail;
	wpa_hexdump(MSG_DEBUG, "SAE: val", hash, hash_len);

	/* val = val modulo (q - 1) + 1 */
	order = crypto_ec_get_order(pt->ec);
	tmp = crypto_bignum_init();
	val = crypto_bignum_init_set(hash, hash_len);
	one = crypto_bignum_init_uint(1);
	if (!tmp || !val || !one ||
	    crypto_bignum_sub(order, one, tmp) < 0 ||
	    crypto_bignum_mod(val, tmp, val) < 0 ||
	    crypto_bignum_add(val, one, val) < 0)
		goto fail;
	debug_print_bignum("SAE: val(reduced to 1..q-1)", val, prime_len);

	/* PWE = scalar-op(val, PT) */
	pwe = crypto_ec_point_init(pt->ec);
	if (!pwe ||
	    crypto_ec_point_mul(pt->ec, pt->ecc_pt, val, pwe) < 0 ||
	    crypto_ec_point_to_bin(pt->ec, pwe, bin, bin + prime_len) < 0) {
		crypto_ec_point_deinit(pwe, 1);
		pwe = NULL;
		goto fail;
	}
	wpa_hexdump_key(MSG_DEBUG, "SAE: PWE.x", bin, prime_len);
	wpa_hexdump_key(MSG_DEBUG, "SAE: PWE.y", bin + prime_len, prime_len);

fail:
	crypto_bignum_deinit(tmp, 1);
	crypto_bignum_deinit(val, 1);
	crypto_bignum_deinit(one, 0);
	return pwe;
}


struct crypto_bignum *
sae_derive_pwe_from_pt_ffc(const struct sae_pt *pt,
			   const u8 *addr1, const u8 *addr2)
{
	size_t prime_len;
	const u8 *addr[2];
	size_t len[2];
	u8 salt[64], hash[64];
	size_t hash_len;
	struct crypto_bignum *tmp = NULL, *val = NULL, *one = NULL;
	struct crypto_bignum *pwe = NULL, *order = NULL, *prime = NULL;

	wpa_printf(MSG_DEBUG, "SAE: Derive PWE from PT");
	prime = crypto_bignum_init_set(pt->dh->prime, pt->dh->prime_len);
	order = crypto_bignum_init_set(pt->dh->order, pt->dh->order_len);
	if (!prime || !order)
		goto fail;
	prime_len = pt->dh->prime_len;

	sae_max_min_addr(addr, len, addr1, addr2);

	/* val = H(0^n,
	 *         MAX(STA-A-MAC, STA-B-MAC) || MIN(STA-A-MAC, STA-B-MAC)) */
	wpa_printf(MSG_DEBUG, "SAE: val = H(0^n, MAX(addrs) || MIN(addrs))");
	hash_len = sae_ffc_prime_len_2_hash_len(prime_len);
	os_memset(salt, 0, hash_len);
	if (hkdf_extract(hash_len, salt, hash_len, 2, addr, len, hash) < 0)
		goto fail;
	wpa_hexdump(MSG_DEBUG, "SAE: val", hash, hash_len);

	/* val = val modulo (q - 1) + 1 */
	tmp = crypto_bignum_init();
	val = crypto_bignum_init_set(hash, hash_len);
	one = crypto_bignum_init_uint(1);
	if (!tmp || !val || !one ||
	    crypto_bignum_sub(order, one, tmp) < 0 ||
	    crypto_bignum_mod(val, tmp, val) < 0 ||
	    crypto_bignum_add(val, one, val) < 0)
		goto fail;
	debug_print_bignum("SAE: val(reduced to 1..q-1)", val, prime_len);

	/* PWE = scalar-op(val, PT) */
	pwe = crypto_bignum_init();
	if (!pwe || crypto_bignum_exptmod(pt->ffc_pt, val, prime, pwe) < 0) {
		crypto_bignum_deinit(pwe, 1);
		pwe = NULL;
		goto fail;
	}
	debug_print_bignum("SAE: PWE", pwe, prime_len);

fail:
	crypto_bignum_deinit(tmp, 1);
	crypto_bignum_deinit(val, 1);
	crypto_bignum_deinit(one, 0);
	crypto_bignum_deinit(prime, 0);
	crypto_bignum_deinit(order, 0);
	return pwe;
}


void sae_deinit_pt(struct sae_pt *pt)
{
	struct sae_pt *prev;

	while (pt) {
		crypto_ec_point_deinit(pt->ecc_pt, 1);
		crypto_bignum_deinit(pt->ffc_pt, 1);
		crypto_ec_deinit(pt->ec);
		prev = pt;
		pt = pt->next;
		os_free(prev);
	}
}


static int sae_derive_commit_element_ecc(struct sae_data *sae,
					 struct crypto_bignum *mask)
{
	/* COMMIT-ELEMENT = inverse(scalar-op(mask, PWE)) */
	if (!sae->tmp->own_commit_element_ecc) {
		sae->tmp->own_commit_element_ecc =
			crypto_ec_point_init(sae->tmp->ec);
		if (!sae->tmp->own_commit_element_ecc)
			return ESP_FAIL;
	}

	if (crypto_ec_point_mul(sae->tmp->ec, sae->tmp->pwe_ecc, mask,
				sae->tmp->own_commit_element_ecc) < 0 ||
	    crypto_ec_point_invert(sae->tmp->ec,
				   sae->tmp->own_commit_element_ecc) < 0) {
		wpa_printf(MSG_DEBUG, "SAE: Could not compute commit-element");
		return ESP_FAIL;
	}

	return ESP_OK;
}


static int sae_derive_commit_element_ffc(struct sae_data *sae,
					 struct crypto_bignum *mask)
{
	/* COMMIT-ELEMENT = inverse(scalar-op(mask, PWE)) */
	if (!sae->tmp->own_commit_element_ffc) {
		sae->tmp->own_commit_element_ffc = crypto_bignum_init();
		if (!sae->tmp->own_commit_element_ffc)
			return ESP_FAIL;
	}

	if (crypto_bignum_exptmod(sae->tmp->pwe_ffc, mask, sae->tmp->prime,
				  sae->tmp->own_commit_element_ffc) < 0 ||
	    crypto_bignum_inverse(sae->tmp->own_commit_element_ffc,
				  sae->tmp->prime,
				  sae->tmp->own_commit_element_ffc) < 0) {
		wpa_printf(MSG_DEBUG, "SAE: Could not compute commit-element");
		return ESP_FAIL;
	}

	return ESP_OK;
}


static int sae_derive_commit(struct sae_data *sae)
{
	struct crypto_bignum *mask = NULL;
	int ret;

	mask = crypto_bignum_init();
	if (!sae->tmp->sae_rand)
		sae->tmp->sae_rand = crypto_bignum_init();
	if (!sae->tmp->own_commit_scalar)
		sae->tmp->own_commit_scalar = crypto_bignum_init();
	ret = !mask || !sae->tmp->sae_rand || !sae->tmp->own_commit_scalar ||
		dragonfly_generate_scalar(sae->tmp->order, sae->tmp->sae_rand,
					  mask,
					  sae->tmp->own_commit_scalar) < 0 ||
		(sae->tmp->ec &&
		 sae_derive_commit_element_ecc(sae, mask) < 0) ||
		(sae->tmp->dh &&
		 sae_derive_commit_element_ffc(sae, mask) < 0);
	crypto_bignum_deinit(mask, 1);
	return ret ? -1 : 0;
}


int sae_prepare_commit(const u8 *addr1, const u8 *addr2,
		       const u8 *password, size_t password_len,
		       struct sae_data *sae)
{
	if (sae->tmp == NULL ||
	    (sae->tmp->ec && sae_derive_pwe_ecc(sae, addr1, addr2, password,
						password_len) < 0) ||
	    (sae->tmp->dh && sae_derive_pwe_ffc(sae, addr1, addr2, password,
						password_len) < 0))
		return ESP_FAIL;

	sae->h2e = 0;
	sae->pk = 0;
	return sae_derive_commit(sae);
}


int sae_prepare_commit_pt(struct sae_data *sae, const struct sae_pt *pt,
			  const u8 *addr1, const u8 *addr2,
			  int *rejected_groups, const struct sae_pk *pk)
{
	if (!sae->tmp)
		return -1;

	while (pt) {
		if (pt->group == sae->group)
			break;
		pt = pt->next;
	}
	if (!pt) {
		wpa_printf(MSG_INFO, "SAE: Could not find PT for group %u",
			   sae->group);
		return -1;
	}
#ifdef CONFIG_SAE_PK
	os_memcpy(sae->tmp->ssid, pt->ssid, pt->ssid_len);
	sae->tmp->ssid_len = pt->ssid_len;
	sae->tmp->ap_pk = pk;
#endif /* CONFIG_SAE_PK */

	sae->tmp->own_addr_higher = os_memcmp(addr1, addr2, ETH_ALEN) > 0;
	wpabuf_free(sae->tmp->own_rejected_groups);
	sae->tmp->own_rejected_groups = NULL;
	if (rejected_groups) {
		int count, i;
		struct wpabuf *groups;

		count = int_array_len(rejected_groups);
		groups = wpabuf_alloc(count * 2);
		if (!groups)
			return -1;
		for (i = 0; i < count; i++)
			wpabuf_put_le16(groups, rejected_groups[i]);
		sae->tmp->own_rejected_groups = groups;
	}

	if (pt->ec) {
		crypto_ec_point_deinit(sae->tmp->pwe_ecc, 1);
		sae->tmp->pwe_ecc = sae_derive_pwe_from_pt_ecc(pt, addr1,
							       addr2);
		if (!sae->tmp->pwe_ecc)
			return -1;
	}

	if (pt->dh) {
		crypto_bignum_deinit(sae->tmp->pwe_ffc, 1);
		sae->tmp->pwe_ffc = sae_derive_pwe_from_pt_ffc(pt, addr1,
							       addr2);
		if (!sae->tmp->pwe_ffc)
			return -1;
	}

	sae->h2e = 1;
	return sae_derive_commit(sae);
}


static int sae_derive_k_ecc(struct sae_data *sae, u8 *k)
{
	struct crypto_ec_point *K;
	int ret = -1;

	K = crypto_ec_point_init(sae->tmp->ec);
	if (K == NULL)
		goto fail;

	/*
	 * K = scalar-op(rand, (elem-op(scalar-op(peer-commit-scalar, PWE),
	 *                                        PEER-COMMIT-ELEMENT)))
	 * If K is identity element (point-at-infinity), reject
	 * k = F(K) (= x coordinate)
	 */

	if (crypto_ec_point_mul(sae->tmp->ec, sae->tmp->pwe_ecc,
				sae->peer_commit_scalar, K) < 0 ||
	    crypto_ec_point_add(sae->tmp->ec, K,
				sae->tmp->peer_commit_element_ecc, K) < 0 ||
	    crypto_ec_point_mul(sae->tmp->ec, K, sae->tmp->sae_rand, K) < 0 ||
	    crypto_ec_point_is_at_infinity(sae->tmp->ec, K) ||
	    crypto_ec_point_to_bin(sae->tmp->ec, K, k, NULL) < 0) {
		wpa_printf(MSG_DEBUG, "SAE: Failed to calculate K and k");
		goto fail;
	}

	wpa_hexdump_key(MSG_DEBUG, "SAE: k", k, sae->tmp->prime_len);

	ret = 0;
fail:
	crypto_ec_point_deinit(K, 1);
	return ret;
}


static int sae_derive_k_ffc(struct sae_data *sae, u8 *k)
{
	struct crypto_bignum *K;
	int ret = -1;

	K = crypto_bignum_init();
	if (K == NULL)
		goto fail;

	/*
	 * K = scalar-op(rand, (elem-op(scalar-op(peer-commit-scalar, PWE),
	 *                                        PEER-COMMIT-ELEMENT)))
	 * If K is identity element (one), reject.
	 * k = F(K) (= x coordinate)
	 */

	if (crypto_bignum_exptmod(sae->tmp->pwe_ffc, sae->peer_commit_scalar,
				  sae->tmp->prime, K) < 0 ||
	    crypto_bignum_mulmod(K, sae->tmp->peer_commit_element_ffc,
				 sae->tmp->prime, K) < 0 ||
	    crypto_bignum_exptmod(K, sae->tmp->sae_rand, sae->tmp->prime, K) < 0
	    ||
	    crypto_bignum_is_one(K) ||
	    crypto_bignum_to_bin(K, k, SAE_MAX_PRIME_LEN, sae->tmp->prime_len) <
	    0) {
		wpa_printf(MSG_DEBUG, "SAE: Failed to calculate K and k");
		goto fail;
	}

	wpa_hexdump_key(MSG_DEBUG, "SAE: k", k, sae->tmp->prime_len);

	ret = 0;
fail:
	crypto_bignum_deinit(K, 1);
	return ret;
}


static int sae_kdf_hash(size_t hash_len, const u8 *k, const char *label,
			const u8 *context, size_t context_len,
			u8 *out, size_t out_len)
{
	if (hash_len == 32)
		return sha256_prf(k, hash_len, label,
				  context, context_len, out, out_len);
	return -1;
}


static int sae_derive_keys(struct sae_data *sae, const u8 *k)
{
	u8 zero[SAE_MAX_HASH_LEN], val[SAE_MAX_PRIME_LEN];
	const u8 *salt;
	struct wpabuf *rejected_groups = NULL;
	u8 keyseed[SAE_MAX_HASH_LEN];
	u8 keys[2 * SAE_MAX_HASH_LEN + SAE_PMK_LEN];
	struct crypto_bignum *tmp;
	int ret = -1;
	size_t hash_len, salt_len, prime_len = sae->tmp->prime_len;
	const u8 *addr[1];
	size_t len[1];

	tmp = crypto_bignum_init();
	if (tmp == NULL)
		goto fail;

	/* keyseed = H(salt, k)
	 * KCK || PMK = KDF-Hash-Length(keyseed, "SAE KCK and PMK",
	 *                      (commit-scalar + peer-commit-scalar) modulo r)
	 * PMKID = L((commit-scalar + peer-commit-scalar) modulo r, 0, 128)
	*
	* When SAE_PK is used,
	* KCK || PMK || KEK = KDF-Hash-Length(keyseed, "SAE-PK keys", context)
	 */
	if (!sae->h2e)
		hash_len = SHA256_MAC_LEN;
	else if (sae->tmp->dh)
		hash_len = sae_ffc_prime_len_2_hash_len(prime_len);
	else
		hash_len = sae_ecc_prime_len_2_hash_len(prime_len);

	if (sae->h2e && (sae->tmp->own_rejected_groups ||
			 sae->tmp->peer_rejected_groups)) {
		struct wpabuf *own, *peer;

		own = sae->tmp->own_rejected_groups;
		peer = sae->tmp->peer_rejected_groups;
		salt_len = 0;
		if (own)
			salt_len += wpabuf_len(own);
		if (peer)
			salt_len += wpabuf_len(peer);
		rejected_groups = wpabuf_alloc(salt_len);
		if (!rejected_groups)
			goto fail;
		if (sae->tmp->own_addr_higher) {
			if (own)
				wpabuf_put_buf(rejected_groups, own);
			if (peer)
				wpabuf_put_buf(rejected_groups, peer);
		} else {
			if (peer)
				wpabuf_put_buf(rejected_groups, peer);
			if (own)
				wpabuf_put_buf(rejected_groups, own);
		}
		salt = wpabuf_head(rejected_groups);
		salt_len = wpabuf_len(rejected_groups);
	} else {
		os_memset(zero, 0, hash_len);
		salt = zero;
		salt_len = hash_len;
	}
	wpa_hexdump(MSG_DEBUG, "SAE: salt for keyseed derivation",
		    salt, salt_len);
	addr[0] = k;
	len[0] = prime_len;
	if (hkdf_extract(hash_len, salt, salt_len, 1, addr, len, keyseed) < 0)
		goto fail;
	wpa_hexdump_key(MSG_DEBUG, "SAE: keyseed", keyseed, hash_len);

	if (crypto_bignum_add(sae->tmp->own_commit_scalar,
			      sae->peer_commit_scalar, tmp) < 0 ||
	    crypto_bignum_mod(tmp, sae->tmp->order, tmp) < 0)
		goto fail;
	/* IEEE Std 802.11-2016 is not exactly clear on the encoding of the bit
	 * string that is needed for KCK, PMK, and PMKID derivation, but it
	 * seems to make most sense to encode the
	 * (commit-scalar + peer-commit-scalar) mod r part as a bit string by
	 * zero padding it from left to the length of the order (in full
	 * octets). */
	if (crypto_bignum_to_bin(tmp, val, sizeof(val),
                                  sae->tmp->order_len) < 0) {
		goto fail;
	}
	wpa_hexdump(MSG_DEBUG, "SAE: PMKID", val, SAE_PMKID_LEN);

#ifdef CONFIG_SAE_PK
	if (sae->pk) {
		if (sae_kdf_hash(hash_len, keyseed, "SAE-PK keys",
			   val, sae->tmp->order_len,
			   keys, 2 * hash_len + SAE_PMK_LEN) < 0)
			goto fail;
	} else {
		if (sae_kdf_hash(hash_len, keyseed, "SAE KCK and PMK",
			   val, sae->tmp->order_len,
			   keys, hash_len + SAE_PMK_LEN) < 0)
			goto fail;
	}
#else /* CONFIG_SAE_PK */
	if (sae_kdf_hash(hash_len, keyseed, "SAE KCK and PMK",
			 val, sae->tmp->order_len,
			 keys, hash_len + SAE_PMK_LEN) < 0)
		goto fail;
#endif /* !CONFIG_SAE_PK */

	forced_memzero(keyseed, sizeof(keyseed));
	os_memcpy(sae->tmp->kck, keys, hash_len);
	sae->tmp->kck_len = hash_len;
	os_memcpy(sae->pmk, keys + hash_len, SAE_PMK_LEN);
	sae->pmk_len = SAE_PMK_LEN;
	os_memcpy(sae->pmkid, val, SAE_PMKID_LEN);

#ifdef CONFIG_SAE_PK
	if (sae->pk) {
		os_memcpy(sae->tmp->kek, keys + hash_len + SAE_PMK_LEN, hash_len);
		sae->tmp->kek_len = hash_len;
		wpa_hexdump_key(MSG_DEBUG, "SAE: KEK for SAE-PK",
				   sae->tmp->kek, sae->tmp->kek_len);
	}
#endif /* CONFIG_SAE_PK */
	forced_memzero(keys, sizeof(keys));
	wpa_hexdump_key(MSG_DEBUG, "SAE: KCK",
			sae->tmp->kck, sae->tmp->kck_len);
	wpa_hexdump_key(MSG_DEBUG, "SAE: PMK", sae->pmk, SAE_PMK_LEN);

	ret = 0;
fail:
	wpabuf_free(rejected_groups);
	crypto_bignum_deinit(tmp, 0);
	return ret;
}


int sae_process_commit(struct sae_data *sae)
{
	u8 k[SAE_MAX_PRIME_LEN] = {0};
	if (sae->tmp == NULL ||
	    (sae->tmp->ec && sae_derive_k_ecc(sae, k) < 0) ||
	    (sae->tmp->dh && sae_derive_k_ffc(sae, k) < 0) ||
	    sae_derive_keys(sae, k) < 0)
		return ESP_FAIL;
	return ESP_OK;
}


int sae_write_commit(struct sae_data *sae, struct wpabuf *buf,
		     const struct wpabuf *token, const char *identifier)
{
	u8 *pos;

	if (sae->tmp == NULL)
		return ESP_FAIL;

	wpabuf_put_le16(buf, sae->group); /* Finite Cyclic Group */
	if (!sae->h2e && token) {
		wpabuf_put_buf(buf, token);
		wpa_hexdump(MSG_DEBUG, "SAE: Anti-clogging token",
			    wpabuf_head(token), wpabuf_len(token));
	}
	pos = wpabuf_put(buf, sae->tmp->prime_len);
	if (crypto_bignum_to_bin(sae->tmp->own_commit_scalar, pos,
				 sae->tmp->prime_len, sae->tmp->prime_len) < 0) {
		wpa_printf(MSG_ERROR, "SAE: failed bignum operation on own commit scalar");
		return ESP_FAIL;
	}
	wpa_hexdump(MSG_DEBUG, "SAE: own commit-scalar",
		    pos, sae->tmp->prime_len);
	if (sae->tmp->ec) {
		pos = wpabuf_put(buf, 2 * sae->tmp->prime_len);
		if (crypto_ec_point_to_bin(sae->tmp->ec,
					   sae->tmp->own_commit_element_ecc,
					   pos, pos + sae->tmp->prime_len) < 0) {
			wpa_printf(MSG_ERROR, "SAE: failed bignum op while deriving ec point");
			return ESP_FAIL;
		}
		wpa_hexdump(MSG_DEBUG, "SAE: own commit-element(x)",
			    pos, sae->tmp->prime_len);
		wpa_hexdump(MSG_DEBUG, "SAE: own commit-element(y)",
			    pos + sae->tmp->prime_len, sae->tmp->prime_len);
	} else {
		pos = wpabuf_put(buf, sae->tmp->prime_len);
		if (crypto_bignum_to_bin(sae->tmp->own_commit_element_ffc, pos,
					 sae->tmp->prime_len, sae->tmp->prime_len) < 0) {
			wpa_printf(MSG_ERROR, "SAE: failed bignum operation on commit elem ffc");
			return ESP_FAIL;
		}
		wpa_hexdump(MSG_DEBUG, "SAE: own commit-element",
			    pos, sae->tmp->prime_len);
	}

	if (identifier) {
		/* Password Identifier element */
		wpabuf_put_u8(buf, WLAN_EID_EXTENSION);
		wpabuf_put_u8(buf, 1 + os_strlen(identifier));
		wpabuf_put_u8(buf, WLAN_EID_EXT_PASSWORD_IDENTIFIER);
		wpabuf_put_str(buf, identifier);
		wpa_printf(MSG_DEBUG, "SAE: own Password Identifier: %s",
			   identifier);
	}
	if (sae->h2e && sae->tmp->own_rejected_groups) {
		wpa_hexdump_buf(MSG_DEBUG, "SAE: own Rejected Groups",
				sae->tmp->own_rejected_groups);
		wpabuf_put_u8(buf, WLAN_EID_EXTENSION);
		wpabuf_put_u8(buf,
			      1 + wpabuf_len(sae->tmp->own_rejected_groups));
		wpabuf_put_u8(buf, WLAN_EID_EXT_REJECTED_GROUPS);
		wpabuf_put_buf(buf, sae->tmp->own_rejected_groups);
	}

	if (sae->h2e && token) {
		wpabuf_put_u8(buf, WLAN_EID_EXTENSION);
		wpabuf_put_u8(buf, 1 + wpabuf_len(token));
		wpabuf_put_u8(buf, WLAN_EID_EXT_ANTI_CLOGGING_TOKEN);
		wpabuf_put_buf(buf, token);
		wpa_hexdump_buf(MSG_DEBUG,
				"SAE: Anti-clogging token (in container)",
				token);
	}
	return ESP_OK;
}


u16 sae_group_allowed(struct sae_data *sae, int *allowed_groups, u16 group)
{
	if (allowed_groups) {
		int i;
		for (i = 0; allowed_groups[i] > 0; i++) {
			if (allowed_groups[i] == group)
				break;
		}
		if (allowed_groups[i] != group) {
			wpa_printf(MSG_DEBUG, "SAE: Proposed group %u not "
				   "enabled in the current configuration",
				   group);
			return WLAN_STATUS_FINITE_CYCLIC_GROUP_NOT_SUPPORTED;
		}
	}

	if (sae->state == SAE_COMMITTED && group != sae->group) {
		wpa_printf(MSG_DEBUG, "SAE: Do not allow group to be changed");
		return WLAN_STATUS_FINITE_CYCLIC_GROUP_NOT_SUPPORTED;
	}

	if (group != sae->group && sae_set_group(sae, group) < 0) {
		wpa_printf(MSG_DEBUG, "SAE: Unsupported Finite Cyclic Group %u",
			   group);
		return WLAN_STATUS_FINITE_CYCLIC_GROUP_NOT_SUPPORTED;
	}

	if (sae->tmp == NULL) {
		wpa_printf(MSG_DEBUG, "SAE: Group information not yet initialized");
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}

	if (sae->tmp->dh && !allowed_groups) {
		wpa_printf(MSG_DEBUG, "SAE: Do not allow FFC group %u without "
			   "explicit configuration enabling it", group);
		return WLAN_STATUS_FINITE_CYCLIC_GROUP_NOT_SUPPORTED;
	}

	return WLAN_STATUS_SUCCESS;
}


static int sae_is_password_id_elem(const u8 *pos, const u8 *end)
{
	return  end - pos >= 3 &&
		pos[0] == WLAN_EID_EXTENSION &&
		pos[1] >= 1 &&
		end - pos - 2 >= pos[1] &&
		pos[2] == WLAN_EID_EXT_PASSWORD_IDENTIFIER;
}


static int sae_is_rejected_groups_elem(const u8 *pos, const u8 *end)
{
	return end - pos >= 3 &&
		pos[0] == WLAN_EID_EXTENSION &&
		pos[1] >= 2 &&
		end - pos - 2 >= pos[1] &&
		pos[2] == WLAN_EID_EXT_REJECTED_GROUPS;
}


static int sae_is_token_container_elem(const u8 *pos, const u8 *end)
{
	return end - pos >= 3 &&
		pos[0] == WLAN_EID_EXTENSION &&
		pos[1] >= 1 &&
		end - pos - 2 >= pos[1] &&
		pos[2] == WLAN_EID_EXT_ANTI_CLOGGING_TOKEN;
}


static void sae_parse_commit_token(struct sae_data *sae, const u8 **pos,
				   const u8 *end, const u8 **token,
				   size_t *token_len, int h2e)
{
	size_t scalar_elem_len, tlen;

	if (token)
		*token = NULL;
	if (token_len)
		*token_len = 0;

	if (h2e)
		return; /* No Anti-Clogging Token field outside container IE */

	scalar_elem_len = (sae->tmp->ec ? 3 : 2) * sae->tmp->prime_len;
	if (scalar_elem_len >= (size_t) (end - *pos))
		return; /* No extra data beyond peer scalar and element */

	tlen = end - (*pos + scalar_elem_len);

	if (tlen < SHA256_MAC_LEN) {
		wpa_printf(MSG_DEBUG,
			   "SAE: Too short optional data (%u octets) to include our Anti-Clogging Token",
			   (unsigned int) tlen);
		return;
	}

	wpa_hexdump(MSG_DEBUG, "SAE: Anti-Clogging Token", *pos, tlen);
	if (token)
		*token = *pos;
	if (token_len)
		*token_len = tlen;
	*pos += tlen;
}


static void sae_parse_token_container(struct sae_data *sae,
				      const u8 *pos, const u8 *end,
				      const u8 **token, size_t *token_len)
{
	wpa_hexdump(MSG_DEBUG, "SAE: Possible elements at the end of the frame",
		    pos, end - pos);
	if (!sae_is_token_container_elem(pos, end))
		return;
	*token = pos + 3;
	*token_len = pos[1] - 1;
	wpa_hexdump(MSG_DEBUG, "SAE: Anti-Clogging Token (in container)",
		    *token, *token_len);
}


static u16 sae_parse_commit_scalar(struct sae_data *sae, const u8 **pos,
				   const u8 *end)
{
	struct crypto_bignum *peer_scalar;

	if (sae->tmp->prime_len > end - *pos) {
		wpa_printf(MSG_DEBUG, "SAE: Not enough data for scalar");
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}

	peer_scalar = crypto_bignum_init_set(*pos, sae->tmp->prime_len);
	if (peer_scalar == NULL)
		return WLAN_STATUS_UNSPECIFIED_FAILURE;

	/*
	 * IEEE Std 802.11-2012, 11.3.8.6.1: If there is a protocol instance for
	 * the peer and it is in Authenticated state, the new Commit Message
	 * shall be dropped if the peer-scalar is identical to the one used in
	 * the existing protocol instance.
	 */
	if (sae->state == SAE_ACCEPTED && sae->peer_commit_scalar_accepted &&
	    crypto_bignum_cmp(sae->peer_commit_scalar_accepted,
			      peer_scalar) == 0) {
		wpa_printf(MSG_DEBUG, "SAE: Do not accept re-use of previous "
			   "peer-commit-scalar");
		crypto_bignum_deinit(peer_scalar, 0);
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}

	/* 1 < scalar < r */
	if (crypto_bignum_is_zero(peer_scalar) ||
	    crypto_bignum_is_one(peer_scalar) ||
	    crypto_bignum_cmp(peer_scalar, sae->tmp->order) >= 0) {
		wpa_printf(MSG_DEBUG, "SAE: Invalid peer scalar");
		crypto_bignum_deinit(peer_scalar, 0);
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}

	crypto_bignum_deinit(sae->peer_commit_scalar, 0);
	sae->peer_commit_scalar = peer_scalar;
	wpa_hexdump(MSG_DEBUG, "SAE: Peer commit-scalar",
		    *pos, sae->tmp->prime_len);
	*pos += sae->tmp->prime_len;

	return WLAN_STATUS_SUCCESS;
}


static u16 sae_parse_commit_element_ecc(struct sae_data *sae, const u8 **pos,
					const u8 *end)
{
	u8 prime[SAE_MAX_ECC_PRIME_LEN];

	if (2 * sae->tmp->prime_len > end - *pos) {
		wpa_printf(MSG_DEBUG, "SAE: Not enough data for "
			   "commit-element");
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}

	if (crypto_bignum_to_bin(sae->tmp->prime, prime, sizeof(prime),
				 sae->tmp->prime_len) < 0)
		return WLAN_STATUS_UNSPECIFIED_FAILURE;

	/* element x and y coordinates < p */
	if (os_memcmp(*pos, prime, sae->tmp->prime_len) >= 0 ||
	    os_memcmp(*pos + sae->tmp->prime_len, prime,
		      sae->tmp->prime_len) >= 0) {
		wpa_printf(MSG_DEBUG, "SAE: Invalid coordinates in peer "
			   "element");
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}

	wpa_hexdump(MSG_DEBUG, "SAE: Peer commit-element(x)",
		    *pos, sae->tmp->prime_len);
	wpa_hexdump(MSG_DEBUG, "SAE: Peer commit-element(y)",
		    *pos + sae->tmp->prime_len, sae->tmp->prime_len);

	crypto_ec_point_deinit(sae->tmp->peer_commit_element_ecc, 0);
	sae->tmp->peer_commit_element_ecc =
		crypto_ec_point_from_bin(sae->tmp->ec, *pos);
	if (!sae->tmp->peer_commit_element_ecc) {
		wpa_printf(MSG_DEBUG, "SAE: Peer element is not a valid point");
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}

	if (!crypto_ec_point_is_on_curve(sae->tmp->ec,
					 sae->tmp->peer_commit_element_ecc)) {
		wpa_printf(MSG_DEBUG, "SAE: Peer element is not on curve");
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}

	*pos += 2 * sae->tmp->prime_len;

	return WLAN_STATUS_SUCCESS;
}


static u16 sae_parse_commit_element_ffc(struct sae_data *sae, const u8 **pos,
					const u8 *end)
{
	struct crypto_bignum *res, *one;
	const u8 one_bin[1] = { 0x01 };

	if (sae->tmp->prime_len > end - *pos) {
		wpa_printf(MSG_DEBUG, "SAE: Not enough data for "
			   "commit-element");
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}
	wpa_hexdump(MSG_DEBUG, "SAE: Peer commit-element", *pos,
		    sae->tmp->prime_len);

	crypto_bignum_deinit(sae->tmp->peer_commit_element_ffc, 0);
	sae->tmp->peer_commit_element_ffc =
		crypto_bignum_init_set(*pos, sae->tmp->prime_len);
	if (sae->tmp->peer_commit_element_ffc == NULL)
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	/* 1 < element < p - 1 */
	res = crypto_bignum_init();
	one = crypto_bignum_init_set(one_bin, sizeof(one_bin));
	if (!res || !one ||
	    crypto_bignum_sub(sae->tmp->prime, one, res) ||
	    crypto_bignum_is_zero(sae->tmp->peer_commit_element_ffc) ||
	    crypto_bignum_is_one(sae->tmp->peer_commit_element_ffc) ||
	    crypto_bignum_cmp(sae->tmp->peer_commit_element_ffc, res) >= 0) {
		crypto_bignum_deinit(res, 0);
		crypto_bignum_deinit(one, 0);
		wpa_printf(MSG_DEBUG, "SAE: Invalid peer element");
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}
	crypto_bignum_deinit(one, 0);

	/* scalar-op(r, ELEMENT) = 1 modulo p */
	if (crypto_bignum_exptmod(sae->tmp->peer_commit_element_ffc,
				  sae->tmp->order, sae->tmp->prime, res) < 0 ||
	    !crypto_bignum_is_one(res)) {
		wpa_printf(MSG_DEBUG, "SAE: Invalid peer element (scalar-op)");
		crypto_bignum_deinit(res, 0);
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}
	crypto_bignum_deinit(res, 0);

	*pos += sae->tmp->prime_len;

	return WLAN_STATUS_SUCCESS;
}


static u16 sae_parse_commit_element(struct sae_data *sae, const u8 **pos,
				    const u8 *end)
{
	if (sae->tmp->dh)
		return sae_parse_commit_element_ffc(sae, pos, end);
	return sae_parse_commit_element_ecc(sae, pos, end);
}


static int sae_parse_password_identifier(struct sae_data *sae,
					 const u8 **pos, const u8 *end)
{
	const u8 *epos;
	u8 len;

	wpa_hexdump(MSG_DEBUG, "SAE: Possible elements at the end of the frame",
		    *pos, end - *pos);
	if (!sae_is_password_id_elem(*pos, end)) {
		if (sae->tmp->pw_id) {
			wpa_printf(MSG_DEBUG,
				   "SAE: No Password Identifier included, but expected one (%s)",
				   sae->tmp->pw_id);
			return WLAN_STATUS_UNKNOWN_PASSWORD_IDENTIFIER;
		}
		os_free(sae->tmp->pw_id);
		sae->tmp->pw_id = NULL;
		return WLAN_STATUS_SUCCESS; /* No Password Identifier */
	}

	epos = *pos;
	epos++; /* skip IE type */
	len = *epos++; /* IE length */
	if (len > end - epos || len < 1)
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	epos++; /* skip ext ID */
	len--;

	if (sae->tmp->pw_id &&
	    (len != os_strlen(sae->tmp->pw_id) ||
	     os_memcmp(sae->tmp->pw_id, epos, len) != 0)) {
		wpa_printf(MSG_DEBUG,
			   "SAE: The included Password Identifier does not match the expected one (%s)",
			   sae->tmp->pw_id);
		return WLAN_STATUS_UNKNOWN_PASSWORD_IDENTIFIER;
	}

	os_free(sae->tmp->pw_id);
	sae->tmp->pw_id = os_malloc(len + 1);
	if (!sae->tmp->pw_id)
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	os_memcpy(sae->tmp->pw_id, epos, len);
	sae->tmp->pw_id[len] = '\0';
	*pos = epos + len;
	return WLAN_STATUS_SUCCESS;
}


static int sae_parse_rejected_groups(struct sae_data *sae,
				     const u8 **pos, const u8 *end)
{
	const u8 *epos;
	u8 len;

	wpa_hexdump(MSG_DEBUG, "SAE: Possible elements at the end of the frame",
		    *pos, end - *pos);
	if (!sae_is_rejected_groups_elem(*pos, end)) {
		wpabuf_free(sae->tmp->peer_rejected_groups);
		sae->tmp->peer_rejected_groups = NULL;
		return WLAN_STATUS_SUCCESS;
	}

	epos = *pos;
	epos++; /* skip IE type */
	len = *epos++; /* IE length */
	if (len > end - epos || len < 1)
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	epos++; /* skip ext ID */
	len--;

	if (len & 1) {
		wpa_printf(MSG_DEBUG,
			   "SAE: Invalid length of the Rejected Groups element payload: %u",
			   len);
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}
	wpabuf_free(sae->tmp->peer_rejected_groups);
	sae->tmp->peer_rejected_groups = wpabuf_alloc(len);
	if (!sae->tmp->peer_rejected_groups)
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	wpabuf_put_data(sae->tmp->peer_rejected_groups, epos, len);
	wpa_hexdump_buf(MSG_DEBUG, "SAE: Received Rejected Groups list",
			sae->tmp->peer_rejected_groups);
	*pos = epos + len;
	return WLAN_STATUS_SUCCESS;
}


u16 sae_parse_commit(struct sae_data *sae, const u8 *data, size_t len,
		     const u8 **token, size_t *token_len, int *allowed_groups,
		     int h2e)
{
	const u8 *pos = data, *end = data + len;
	u16 res;

	/* Check Finite Cyclic Group */
	if (end - pos < 2)
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	res = sae_group_allowed(sae, allowed_groups, WPA_GET_LE16(pos));
	if (res != WLAN_STATUS_SUCCESS)
		return res;
	pos += 2;

	/* Optional Anti-Clogging Token */
	sae_parse_commit_token(sae, &pos, end, token, token_len, h2e);

	/* commit-scalar */
	res = sae_parse_commit_scalar(sae, &pos, end);
	if (res != WLAN_STATUS_SUCCESS)
		return res;

	/* commit-element */
	res = sae_parse_commit_element(sae, &pos, end);
	if (res != WLAN_STATUS_SUCCESS)
		return res;

	/* Optional Password Identifier element */
	res = sae_parse_password_identifier(sae, &pos, end);
	if (res != WLAN_STATUS_SUCCESS)
		return res;

	/* Conditional Rejected Groups element */
	if (h2e) {
		res = sae_parse_rejected_groups(sae, &pos, end);
		if (res != WLAN_STATUS_SUCCESS)
			return res;
	} else {
		wpabuf_free(sae->tmp->peer_rejected_groups);
		sae->tmp->peer_rejected_groups = NULL;
	}

	/* Optional Anti-Clogging Token Container element */
	if (h2e)
		sae_parse_token_container(sae, pos, end, token, token_len);

	/*
	 * Check whether peer-commit-scalar and PEER-COMMIT-ELEMENT are same as
	 * the values we sent which would be evidence of a reflection attack.
	 */
	if (!sae->tmp->own_commit_scalar ||
	    crypto_bignum_cmp(sae->tmp->own_commit_scalar,
			      sae->peer_commit_scalar) != 0 ||
	    (sae->tmp->dh &&
	     (!sae->tmp->own_commit_element_ffc ||
	      crypto_bignum_cmp(sae->tmp->own_commit_element_ffc,
				sae->tmp->peer_commit_element_ffc) != 0)) ||
	    (sae->tmp->ec &&
	     (!sae->tmp->own_commit_element_ecc ||
	      crypto_ec_point_cmp(sae->tmp->ec,
				  sae->tmp->own_commit_element_ecc,
				  sae->tmp->peer_commit_element_ecc) != 0)))
		return WLAN_STATUS_SUCCESS; /* scalars/elements are different */

	/*
	 * This is a reflection attack - return special value to trigger caller
	 * to silently discard the frame instead of replying with a specific
	 * status code.
	 */
	return SAE_SILENTLY_DISCARD;
}


static int sae_cn_confirm(struct sae_data *sae, const u8 *sc,
			  const struct crypto_bignum *scalar1,
			  const u8 *element1, size_t element1_len,
			  const struct crypto_bignum *scalar2,
			  const u8 *element2, size_t element2_len,
			  u8 *confirm)
{
	const u8 *addr[5];
	size_t len[5];
	u8 scalar_b1[SAE_MAX_PRIME_LEN], scalar_b2[SAE_MAX_PRIME_LEN];

	/* Confirm
	 * CN(key, X, Y, Z, ...) =
	 *    HMAC-SHA256(key, D2OS(X) || D2OS(Y) || D2OS(Z) | ...)
	 * confirm = CN(KCK, send-confirm, commit-scalar, COMMIT-ELEMENT,
	 *              peer-commit-scalar, PEER-COMMIT-ELEMENT)
	 * verifier = CN(KCK, peer-send-confirm, peer-commit-scalar,
	 *               PEER-COMMIT-ELEMENT, commit-scalar, COMMIT-ELEMENT)
	 */
	if (crypto_bignum_to_bin(scalar1, scalar_b1, sizeof(scalar_b1),
				 sae->tmp->prime_len) < 0 ||
	    crypto_bignum_to_bin(scalar2, scalar_b2, sizeof(scalar_b2),
				 sae->tmp->prime_len) < 0)
		return ESP_FAIL;
	addr[0] = sc;
	len[0] = 2;
	addr[1] = scalar_b1;
	len[1] = sae->tmp->prime_len;
	addr[2] = element1;
	len[2] = element1_len;
	addr[3] = scalar_b2;
	len[3] = sae->tmp->prime_len;
	addr[4] = element2;
	len[4] = element2_len;
	return hkdf_extract(SAE_KCK_LEN, sae->tmp->kck, sae->tmp->kck_len,
			    5, addr, len, confirm);
}


static int sae_cn_confirm_ecc(struct sae_data *sae, const u8 *sc,
			      const struct crypto_bignum *scalar1,
			      const struct crypto_ec_point *element1,
			      const struct crypto_bignum *scalar2,
			      const struct crypto_ec_point *element2,
			      u8 *confirm)
{
	u8 element_b1[2 * SAE_MAX_ECC_PRIME_LEN];
	u8 element_b2[2 * SAE_MAX_ECC_PRIME_LEN];

	if (crypto_ec_point_to_bin(sae->tmp->ec, element1, element_b1,
				   element_b1 + sae->tmp->prime_len) < 0) {
		wpa_printf(MSG_ERROR, "SAE: failed bignum op while deriving ec point");
		return ESP_FAIL;
	}
	if (crypto_ec_point_to_bin(sae->tmp->ec, element2, element_b2,
				   element_b2 + sae->tmp->prime_len) < 0) {
		wpa_printf(MSG_ERROR, "SAE: failed bignum op while deriving ec point");
		return ESP_FAIL;
	}

	sae_cn_confirm(sae, sc, scalar1, element_b1, 2 * sae->tmp->prime_len,
		       scalar2, element_b2, 2 * sae->tmp->prime_len, confirm);
	return ESP_OK;
}


static int sae_cn_confirm_ffc(struct sae_data *sae, const u8 *sc,
			      const struct crypto_bignum *scalar1,
			      const struct crypto_bignum *element1,
			      const struct crypto_bignum *scalar2,
			      const struct crypto_bignum *element2,
			      u8 *confirm)
{
	u8 element_b1[SAE_MAX_PRIME_LEN];
	u8 element_b2[SAE_MAX_PRIME_LEN];

	if (crypto_bignum_to_bin(element1, element_b1, sizeof(element_b1),
			         sae->tmp->prime_len) < 0) {
		wpa_printf(MSG_ERROR, "SAE: failed bignum op while generating SAE confirm - e1");
		return ESP_FAIL;
	}
	if (crypto_bignum_to_bin(element2, element_b2, sizeof(element_b2),
				 sae->tmp->prime_len) < 0) {
		wpa_printf(MSG_ERROR, "SAE: failed bignum op while generating SAE confirm - e2");
		return ESP_FAIL;
	}

	if (sae_cn_confirm(sae, sc, scalar1, element_b1, sae->tmp->prime_len,
			   scalar2, element_b2, sae->tmp->prime_len, confirm) < 0)
		return -1;
	return ESP_OK;
}


int sae_write_confirm(struct sae_data *sae, struct wpabuf *buf)
{
	const u8 *sc;
	size_t hash_len;

	if (sae->tmp == NULL)
		return ESP_FAIL;

	hash_len = sae->tmp->kck_len;
	/* Send-Confirm */
	if (sae->send_confirm < 0xffff)
		sae->send_confirm++;
	sc = wpabuf_put(buf, 0);
	wpabuf_put_le16(buf, sae->send_confirm);

	if (sae->tmp->ec) {
		if (sae_cn_confirm_ecc(sae, sc, sae->tmp->own_commit_scalar,
					 sae->tmp->own_commit_element_ecc,
					 sae->peer_commit_scalar,
					 sae->tmp->peer_commit_element_ecc,
					 wpabuf_put(buf, hash_len))) {
			wpa_printf(MSG_ERROR, "SAE: failed generate SAE confirm (ecc)");
			return ESP_FAIL;
		}
	} else {
		if (sae_cn_confirm_ffc(sae, sc, sae->tmp->own_commit_scalar,
					 sae->tmp->own_commit_element_ffc,
					 sae->peer_commit_scalar,
					 sae->tmp->peer_commit_element_ffc,
					 wpabuf_put(buf, hash_len))) {
			wpa_printf(MSG_ERROR, "SAE: failed generate SAE confirm (ffc)");
			return ESP_FAIL;
		}
	}
	return ESP_OK;
}


int sae_check_confirm(struct sae_data *sae, const u8 *data, size_t len)
{
	u8 verifier[SAE_MAX_HASH_LEN];
	size_t hash_len;

	if (!sae->tmp) {
		return ESP_FAIL;
	}
	hash_len = sae->tmp->kck_len;
	if (len < 2 + hash_len) {
		wpa_printf(MSG_DEBUG, "SAE: Too short confirm message");
		return ESP_FAIL;
	}

	wpa_printf(MSG_DEBUG, "SAE: peer-send-confirm %u", WPA_GET_LE16(data));

	if (sae->tmp == NULL || !sae->peer_commit_scalar ||
		!sae->tmp->own_commit_scalar) {
		wpa_printf(MSG_DEBUG, "SAE: Temporary data not yet available");
		return ESP_FAIL;
	}

	if (sae->tmp->ec) {
		if (!sae->tmp->peer_commit_element_ecc ||
		    !sae->tmp->own_commit_element_ecc)
			return ESP_FAIL;
		if (sae_cn_confirm_ecc(sae, data, sae->peer_commit_scalar,
				       sae->tmp->peer_commit_element_ecc,
				       sae->tmp->own_commit_scalar,
				       sae->tmp->own_commit_element_ecc,
				       verifier)) {
			wpa_printf(MSG_ERROR, "SAE: failed to check SAE confirm (ecc)");
			return ESP_FAIL;
		}
	} else {
		if (!sae->tmp->peer_commit_element_ffc ||
		    !sae->tmp->own_commit_element_ffc)
			return ESP_FAIL;
		if (sae_cn_confirm_ffc(sae, data, sae->peer_commit_scalar,
				       sae->tmp->peer_commit_element_ffc,
				       sae->tmp->own_commit_scalar,
				       sae->tmp->own_commit_element_ffc,
				       verifier)) {
			wpa_printf(MSG_ERROR, "SAE: failed check SAE confirm (ffc)");
			return ESP_FAIL;
		}
	}

	if (os_memcmp_const(verifier, data + 2, hash_len) != 0) {
		wpa_printf(MSG_DEBUG, "SAE: Confirm mismatch");
		wpa_hexdump(MSG_DEBUG, "SAE: Received confirm",
			    data + 2, hash_len);
		wpa_hexdump(MSG_DEBUG, "SAE: Calculated verifier",
			    verifier, hash_len);
		return ESP_FAIL;
	}

#ifdef CONFIG_SAE_PK
	if (sae_check_confirm_pk(sae, data + 2 + hash_len,
		   len - 2 - hash_len) != ESP_OK)
		return ESP_FAIL;
#endif /* CONFIG_SAE_PK */

	return ESP_OK;
}

const char * sae_state_txt(enum sae_state state)
{
	switch (state) {
	case SAE_NOTHING:
		return "Nothing";
	case SAE_COMMITTED:
		return "Committed";
	case SAE_CONFIRMED:
		return "Confirmed";
	case SAE_ACCEPTED:
		return "Accepted";
	}
	return "?";
}

#endif /* CONFIG_WPA3_SAE */
