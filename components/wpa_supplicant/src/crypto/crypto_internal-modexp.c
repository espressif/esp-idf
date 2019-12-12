/*
 * Crypto wrapper for internal crypto implementation - modexp
 * Copyright (c) 2006-2009, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */
/*
 * Hardware crypto support Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "utils/includes.h"

#include "utils/common.h"
#ifdef USE_MBEDTLS_CRYPTO
#include "mbedtls/bignum.h"
#else /* USE_MBEDTLS_CRYPTO */
#include "bignum.h"
#endif /* USE_MBEDTLS_CRYPTO */
#include "crypto.h"

#ifdef USE_MBEDTLS_CRYPTO
int
crypto_mod_exp(const uint8_t *base, size_t base_len,
	       const uint8_t *power, size_t power_len,
	       const uint8_t *modulus, size_t modulus_len,
	       uint8_t *result, size_t *result_len)
{
	mbedtls_mpi bn_base, bn_exp, bn_modulus, bn_result, bn_rinv;
	int ret = 0;
	mbedtls_mpi_init(&bn_base);
	mbedtls_mpi_init(&bn_exp);
	mbedtls_mpi_init(&bn_modulus);
	mbedtls_mpi_init(&bn_result);
	mbedtls_mpi_init(&bn_rinv);

	mbedtls_mpi_read_binary(&bn_base, base, base_len);
	mbedtls_mpi_read_binary(&bn_exp, power, power_len);
	mbedtls_mpi_read_binary(&bn_modulus, modulus, modulus_len);

	ret = mbedtls_mpi_exp_mod(&bn_result, &bn_base, &bn_exp, &bn_modulus, &bn_rinv);
	if (ret < 0) {
		mbedtls_mpi_free(&bn_base);
		mbedtls_mpi_free(&bn_exp);
		mbedtls_mpi_free(&bn_modulus);
		mbedtls_mpi_free(&bn_result);
		mbedtls_mpi_free(&bn_rinv);
		return ret;
	}

	ret = mbedtls_mpi_write_binary(&bn_result, result, *result_len);

	mbedtls_mpi_free(&bn_base);
	mbedtls_mpi_free(&bn_exp);
	mbedtls_mpi_free(&bn_modulus);
	mbedtls_mpi_free(&bn_result);
	mbedtls_mpi_free(&bn_rinv);

	return ret;
}
#else /* USE_MBEDTLS_CRYPTO */
int 
crypto_mod_exp(const u8 *base, size_t base_len,
		   const u8 *power, size_t power_len,
		   const u8 *modulus, size_t modulus_len,
		   u8 *result, size_t *result_len)
{
	struct bignum *bn_base, *bn_exp, *bn_modulus, *bn_result;
	int ret = -1;

	bn_base = bignum_init();
	bn_exp = bignum_init();
	bn_modulus = bignum_init();
	bn_result = bignum_init();

	if (bn_base == NULL || bn_exp == NULL || bn_modulus == NULL ||
	    bn_result == NULL)
		goto error;

	if (bignum_set_unsigned_bin(bn_base, base, base_len) < 0 ||
	    bignum_set_unsigned_bin(bn_exp, power, power_len) < 0 ||
	    bignum_set_unsigned_bin(bn_modulus, modulus, modulus_len) < 0)
		goto error;

	if (bignum_exptmod(bn_base, bn_exp, bn_modulus, bn_result) < 0)
		goto error;

	ret = bignum_get_unsigned_bin(bn_result, result, result_len);

error:
	bignum_deinit(bn_base);
	bignum_deinit(bn_exp);
	bignum_deinit(bn_modulus);
	bignum_deinit(bn_result);
	return ret;
}
#endif /* USE_MBEDTLS_CRYPTO */
