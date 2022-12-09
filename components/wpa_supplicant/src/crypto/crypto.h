/*
 * WPA Supplicant / wrapper functions for crypto libraries
 * Copyright (c) 2004-2009, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 *
 * This file defines the cryptographic functions that need to be implemented
 * for wpa_supplicant and hostapd. When TLS is not used, internal
 * implementation of MD5, SHA1, and AES is used and no external libraries are
 * required. When TLS is enabled (e.g., by enabling EAP-TLS or EAP-PEAP), the
 * crypto library used by the TLS implementation is expected to be used for
 * non-TLS needs, too, in order to save space by not implementing these
 * functions twice.
 *
 * Wrapper code for using each crypto library is in its own file (crypto*.c)
 * and one of these files is build and linked in to provide the functions
 * defined here.
 */

#ifndef CRYPTO_H
#define CRYPTO_H

#include "utils/common.h"

/**
 * md4_vector - MD4 hash for data vector
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash
 * Returns: 0 on success, -1 on failure
 */
int md4_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac);

/**
 * md5_vector - MD5 hash for data vector
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash
 * Returns: 0 on success, -1 on failure
 */
int md5_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac);

#ifdef CONFIG_FIPS
/**
 * md5_vector_non_fips_allow - MD5 hash for data vector (non-FIPS use allowed)
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash
 * Returns: 0 on success, -1 on failure
 */
int md5_vector_non_fips_allow(size_t num_elem, const u8 *addr[],
			      const size_t *len, u8 *mac);
#else /* CONFIG_FIPS */
#define md5_vector_non_fips_allow md5_vector
#endif /* CONFIG_FIPS */


/**
 * sha1_vector - SHA-1 hash for data vector
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash
 * Returns: 0 on success, -1 on failure
 */
int sha1_vector(size_t num_elem, const u8 *addr[], const size_t *len,
		u8 *mac);

/**
 * fips186_2-prf - NIST FIPS Publication 186-2 change notice 1 PRF
 * @seed: Seed/key for the PRF
 * @seed_len: Seed length in bytes
 * @x: Buffer for PRF output
 * @xlen: Output length in bytes
 * Returns: 0 on success, -1 on failure
 *
 * This function implements random number generation specified in NIST FIPS
 * Publication 186-2 for EAP-SIM. This PRF uses a function that is similar to
 * SHA-1, but has different message padding.
 */
int __must_check fips186_2_prf(const u8 *seed, size_t seed_len, u8 *x,
			       size_t xlen);

/**
 * sha256_vector - SHA256 hash for data vector
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash
 * Returns: 0 on success, -1 on failure
 */
int sha256_vector(size_t num_elem, const u8 *addr[], const size_t *len,
		  u8 *mac);

/**
 * des_encrypt - Encrypt one block with DES
 * @clear: 8 octets (in)
 * @key: 7 octets (in) (no parity bits included)
 * @cypher: 8 octets (out)
 */
void des_encrypt(const u8 *clear, const u8 *key, u8 *cypher);

/**
 * aes_encrypt_init - Initialize AES for encryption
 * @key: Encryption key
 * @len: Key length in bytes (usually 16, i.e., 128 bits)
 * Returns: Pointer to context data or %NULL on failure
 */
void * aes_encrypt_init(const u8 *key, size_t len);

/**
 * aes_encrypt - Encrypt one AES block
 * @ctx: Context pointer from aes_encrypt_init()
 * @plain: Plaintext data to be encrypted (16 bytes)
 * @crypt: Buffer for the encrypted data (16 bytes)
 */
void aes_encrypt(void *ctx, const u8 *plain, u8 *crypt);

/**
 * aes_encrypt_deinit - Deinitialize AES encryption
 * @ctx: Context pointer from aes_encrypt_init()
 */
void aes_encrypt_deinit(void *ctx);

/**
 * aes_decrypt_init - Initialize AES for decryption
 * @key: Decryption key
 * @len: Key length in bytes (usually 16, i.e., 128 bits)
 * Returns: Pointer to context data or %NULL on failure
 */
void * aes_decrypt_init(const u8 *key, size_t len);

/**
 * aes_decrypt - Decrypt one AES block
 * @ctx: Context pointer from aes_encrypt_init()
 * @crypt: Encrypted data (16 bytes)
 * @plain: Buffer for the decrypted data (16 bytes)
 */
void aes_decrypt(void *ctx, const u8 *crypt, u8 *plain);

/**
 * aes_decrypt_deinit - Deinitialize AES decryption
 * @ctx: Context pointer from aes_encrypt_init()
 */
void aes_decrypt_deinit(void *ctx);


enum crypto_hash_alg {
        CRYPTO_HASH_ALG_MD5, CRYPTO_HASH_ALG_SHA1,
        CRYPTO_HASH_ALG_HMAC_MD5, CRYPTO_HASH_ALG_HMAC_SHA1,
        CRYPTO_HASH_ALG_SHA256, CRYPTO_HASH_ALG_HMAC_SHA256
};

struct crypto_hash;

/**
 * crypto_hash_init - Initialize hash/HMAC function
 * @alg: Hash algorithm
 * @key: Key for keyed hash (e.g., HMAC) or %NULL if not needed
 * @key_len: Length of the key in bytes
 * Returns: Pointer to hash context to use with other hash functions or %NULL
 * on failure
 *
 * This function is only used with internal TLSv1 implementation
 * (CONFIG_TLS=internal). If that is not used, the crypto wrapper does not need
 * to implement this.
 */
struct crypto_hash * crypto_hash_init(enum crypto_hash_alg alg, const u8 *key,
				      size_t key_len);

/**
 * crypto_hash_update - Add data to hash calculation
 * @ctx: Context pointer from crypto_hash_init()
 * @data: Data buffer to add
 * @len: Length of the buffer
 *
 * This function is only used with internal TLSv1 implementation
 * (CONFIG_TLS=internal). If that is not used, the crypto wrapper does not need
 * to implement this.
 */
void crypto_hash_update(struct crypto_hash *ctx, const u8 *data, size_t len);

/**
 * crypto_hash_finish - Complete hash calculation
 * @ctx: Context pointer from crypto_hash_init()
 * @hash: Buffer for hash value or %NULL if caller is just freeing the hash
 * context
 * @len: Pointer to length of the buffer or %NULL if caller is just freeing the
 * hash context; on return, this is set to the actual length of the hash value
 * Returns: 0 on success, -1 if buffer is too small (len set to needed length),
 * or -2 on other failures (including failed crypto_hash_update() operations)
 *
 * This function calculates the hash value and frees the context buffer that
 * was used for hash calculation.
 *
 * This function is only used with internal TLSv1 implementation
 * (CONFIG_TLS=internal). If that is not used, the crypto wrapper does not need
 * to implement this.
 */
int crypto_hash_finish(struct crypto_hash *ctx, u8 *hash, size_t *len);

enum crypto_cipher_alg {
	CRYPTO_CIPHER_NULL = 0, CRYPTO_CIPHER_ALG_AES, CRYPTO_CIPHER_ALG_3DES,
	CRYPTO_CIPHER_ALG_DES, CRYPTO_CIPHER_ALG_RC2, CRYPTO_CIPHER_ALG_RC4
};

struct crypto_cipher;

/**
 * crypto_cipher_init - Initialize block/stream cipher function
 * @alg: Cipher algorithm
 * @iv: Initialization vector for block ciphers or %NULL for stream ciphers
 * @key: Cipher key
 * @key_len: Length of key in bytes
 * Returns: Pointer to cipher context to use with other cipher functions or
 * %NULL on failure
 *
 * This function is only used with internal TLSv1 implementation
 * (CONFIG_TLS=internal). If that is not used, the crypto wrapper does not need
 * to implement this.
 */
struct crypto_cipher * crypto_cipher_init(enum crypto_cipher_alg alg,
					  const u8 *iv, const u8 *key,
					  size_t key_len);

/**
 * crypto_cipher_encrypt - Cipher encrypt
 * @ctx: Context pointer from crypto_cipher_init()
 * @plain: Plaintext to cipher
 * @crypt: Resulting ciphertext
 * @len: Length of the plaintext
 * Returns: 0 on success, -1 on failure
 *
 * This function is only used with internal TLSv1 implementation
 * (CONFIG_TLS=internal). If that is not used, the crypto wrapper does not need
 * to implement this.
 */
int __must_check crypto_cipher_encrypt(struct crypto_cipher *ctx,
				       const u8 *plain, u8 *crypt, size_t len);

/**
 * crypto_cipher_decrypt - Cipher decrypt
 * @ctx: Context pointer from crypto_cipher_init()
 * @crypt: Ciphertext to decrypt
 * @plain: Resulting plaintext
 * @len: Length of the cipher text
 * Returns: 0 on success, -1 on failure
 *
 * This function is only used with internal TLSv1 implementation
 * (CONFIG_TLS=internal). If that is not used, the crypto wrapper does not need
 * to implement this.
 */
int __must_check crypto_cipher_decrypt(struct crypto_cipher *ctx,
				       const u8 *crypt, u8 *plain, size_t len);

/**
 * crypto_cipher_decrypt - Free cipher context
 * @ctx: Context pointer from crypto_cipher_init()
 *
 * This function is only used with internal TLSv1 implementation
 * (CONFIG_TLS=internal). If that is not used, the crypto wrapper does not need
 * to implement this.
 */
void crypto_cipher_deinit(struct crypto_cipher *ctx);

struct crypto_public_key;
struct crypto_private_key;

/**
 * crypto_public_key_import - Import an RSA public key
 * @key: Key buffer (DER encoded RSA public key)
 * @len: Key buffer length in bytes
 * Returns: Pointer to the public key or %NULL on failure
 *
 * This function can just return %NULL if the crypto library supports X.509
 * parsing. In that case, crypto_public_key_from_cert() is used to import the
 * public key from a certificate.
 *
 * This function is only used with internal TLSv1 implementation
 * (CONFIG_TLS=internal). If that is not used, the crypto wrapper does not need
 * to implement this.
 */
struct crypto_public_key * crypto_public_key_import(const u8 *key, size_t len);

/**
 * crypto_private_key_import - Import an RSA private key
 * @key: Key buffer (DER encoded RSA private key)
 * @len: Key buffer length in bytes
 * @passwd: Key encryption password or %NULL if key is not encrypted
 * Returns: Pointer to the private key or %NULL on failure
 *
 * This function is only used with internal TLSv1 implementation
 * (CONFIG_TLS=internal). If that is not used, the crypto wrapper does not need
 * to implement this.
 */
struct crypto_private_key * crypto_private_key_import(const u8 *key,
						      size_t len,
						      const char *passwd);

/**
 * crypto_public_key_from_cert - Import an RSA public key from a certificate
 * @buf: DER encoded X.509 certificate
 * @len: Certificate buffer length in bytes
 * Returns: Pointer to public key or %NULL on failure
 *
 * This function can just return %NULL if the crypto library does not support
 * X.509 parsing. In that case, internal code will be used to parse the
 * certificate and public key is imported using crypto_public_key_import().
 *
 * This function is only used with internal TLSv1 implementation
 * (CONFIG_TLS=internal). If that is not used, the crypto wrapper does not need
 * to implement this.
 */
struct crypto_public_key * crypto_public_key_from_cert(const u8 *buf,
						       size_t len);

/**
 * crypto_public_key_encrypt_pkcs1_v15 - Public key encryption (PKCS #1 v1.5)
 * @key: Public key
 * @in: Plaintext buffer
 * @inlen: Length of plaintext buffer in bytes
 * @out: Output buffer for encrypted data
 * @outlen: Length of output buffer in bytes; set to used length on success
 * Returns: 0 on success, -1 on failure
 *
 * This function is only used with internal TLSv1 implementation
 * (CONFIG_TLS=internal). If that is not used, the crypto wrapper does not need
 * to implement this.
 */
int __must_check crypto_public_key_encrypt_pkcs1_v15(
	struct crypto_public_key *key, const u8 *in, size_t inlen,
	u8 *out, size_t *outlen);

/**
 * crypto_private_key_decrypt_pkcs1_v15 - Private key decryption (PKCS #1 v1.5)
 * @key: Private key
 * @in: Encrypted buffer
 * @inlen: Length of encrypted buffer in bytes
 * @out: Output buffer for encrypted data
 * @outlen: Length of output buffer in bytes; set to used length on success
 * Returns: 0 on success, -1 on failure
 *
 * This function is only used with internal TLSv1 implementation
 * (CONFIG_TLS=internal). If that is not used, the crypto wrapper does not need
 * to implement this.
 */
int __must_check crypto_private_key_decrypt_pkcs1_v15(
	struct crypto_private_key *key, const u8 *in, size_t inlen,
	u8 *out, size_t *outlen);

/**
 * crypto_private_key_sign_pkcs1 - Sign with private key (PKCS #1)
 * @key: Private key from crypto_private_key_import()
 * @in: Plaintext buffer
 * @inlen: Length of plaintext buffer in bytes
 * @out: Output buffer for encrypted (signed) data
 * @outlen: Length of output buffer in bytes; set to used length on success
 * Returns: 0 on success, -1 on failure
 *
 * This function is only used with internal TLSv1 implementation
 * (CONFIG_TLS=internal). If that is not used, the crypto wrapper does not need
 * to implement this.
 */
int __must_check crypto_private_key_sign_pkcs1(struct crypto_private_key *key,
					       const u8 *in, size_t inlen,
					       u8 *out, size_t *outlen);

/**
 * crypto_public_key_free - Free public key
 * @key: Public key
 *
 * This function is only used with internal TLSv1 implementation
 * (CONFIG_TLS=internal). If that is not used, the crypto wrapper does not need
 * to implement this.
 */
void crypto_public_key_free(struct crypto_public_key *key);

/**
 * crypto_private_key_free - Free private key
 * @key: Private key from crypto_private_key_import()
 *
 * This function is only used with internal TLSv1 implementation
 * (CONFIG_TLS=internal). If that is not used, the crypto wrapper does not need
 * to implement this.
 */
void crypto_private_key_free(struct crypto_private_key *key);

/**
 * crypto_public_key_decrypt_pkcs1 - Decrypt PKCS #1 signature
 * @key: Public key
 * @crypt: Encrypted signature data (using the private key)
 * @crypt_len: Encrypted signature data length
 * @plain: Buffer for plaintext (at least crypt_len bytes)
 * @plain_len: Plaintext length (max buffer size on input, real len on output);
 * Returns: 0 on success, -1 on failure
 */
int __must_check crypto_public_key_decrypt_pkcs1(
	struct crypto_public_key *key, const u8 *crypt, size_t crypt_len,
	u8 *plain, size_t *plain_len);

/**
 * crypto_global_init - Initialize crypto wrapper
 *
 * This function is only used with internal TLSv1 implementation
 * (CONFIG_TLS=internal). If that is not used, the crypto wrapper does not need
 * to implement this.
 */
int __must_check crypto_global_init(void);

/**
 * crypto_global_deinit - Deinitialize crypto wrapper
 *
 * This function is only used with internal TLSv1 implementation
 * (CONFIG_TLS=internal). If that is not used, the crypto wrapper does not need
 * to implement this.
 */
void crypto_global_deinit(void);

/**
 * crypto_mod_exp - Modular exponentiation of large integers
 * @base: Base integer (big endian byte array)
 * @base_len: Length of base integer in bytes
 * @power: Power integer (big endian byte array)
 * @power_len: Length of power integer in bytes
 * @modulus: Modulus integer (big endian byte array)
 * @modulus_len: Length of modulus integer in bytes
 * @result: Buffer for the result
 * @result_len: Result length (max buffer size on input, real len on output)
 * Returns: 0 on success, -1 on failure
 *
 * This function calculates result = base ^ power mod modulus. modules_len is
 * used as the maximum size of modulus buffer. It is set to the used size on
 * success.
 *
 * This function is only used with internal TLSv1 implementation
 * (CONFIG_TLS=internal). If that is not used, the crypto wrapper does not need
 * to implement this.
 */
int __must_check crypto_mod_exp(const u8 *base, size_t base_len,
				const u8 *power, size_t power_len,
				const u8 *modulus, size_t modulus_len,
				u8 *result, size_t *result_len);

/**
 * rc4_skip - XOR RC4 stream to given data with skip-stream-start
 * @key: RC4 key
 * @keylen: RC4 key length
 * @skip: number of bytes to skip from the beginning of the RC4 stream
 * @data: data to be XOR'ed with RC4 stream
 * @data_len: buf length
 * Returns: 0 on success, -1 on failure
 *
 * Generate RC4 pseudo random stream for the given key, skip beginning of the
 * stream, and XOR the end result with the data buffer to perform RC4
 * encryption/decryption.
 */
int rc4_skip(const u8 *key, size_t keylen, size_t skip,
	     u8 *data, size_t data_len);


/**
 * struct crypto_bignum - bignum
 *
 * Internal data structure for bignum implementation. The contents is specific
 * to the used crypto library.
 */
struct crypto_bignum;

/**
 * struct crypto_key - key
 *
 * Internal data structure for ssl key. The contents is specific
 * to the used crypto library.
 */
struct crypto_key;

/**
 * crypto_bignum_init - Allocate memory for bignum
 * Returns: Pointer to allocated bignum or %NULL on failure
 */
struct crypto_bignum * crypto_bignum_init(void);

/**
 * crypto_bignum_init_set - Allocate memory for bignum and set the value
 * @buf: Buffer with unsigned binary value
 * @len: Length of buf in octets
 * Returns: Pointer to allocated bignum or %NULL on failure
 */
struct crypto_bignum * crypto_bignum_init_set(const u8 *buf, size_t len);

/**
 * crypto_bignum_deinit - Free bignum
 * @n: Bignum from crypto_bignum_init() or crypto_bignum_init_set()
 * @clear: Whether to clear the value from memory
 */
void crypto_bignum_deinit(struct crypto_bignum *n, int clear);

/**
 * crypto_bignum_to_bin - Set binary buffer to unsigned bignum
 * @a: Bignum
 * @buf: Buffer for the binary number
 * @len: Length of @buf in octets
 * @padlen: Length in octets to pad the result to or 0 to indicate no padding
 * Returns: Number of octets written on success, -1 on failure
 */
int crypto_bignum_to_bin(const struct crypto_bignum *a,
        u8 *buf, size_t buflen, size_t padlen);

/**
 * crypto_bignum_add - c = a + b
 * @a: Bignum
 * @b: Bignum
 * @c: Bignum; used to store the result of a + b
 * Returns: 0 on success, -1 on failure
 */
int crypto_bignum_add(const struct crypto_bignum *a,
        const struct crypto_bignum *b,
        struct crypto_bignum *c);

/**
 * crypto_bignum_mod - c = a % b
 * @a: Bignum
 * @b: Bignum
 * @c: Bignum; used to store the result of a % b
 * Returns: 0 on success, -1 on failure
 */
int crypto_bignum_mod(const struct crypto_bignum *a,
        const struct crypto_bignum *b,
        struct crypto_bignum *c);

/**
 * crypto_bignum_exptmod - Modular exponentiation: d = a^b (mod c)
 * @a: Bignum; base
 * @b: Bignum; exponent
 * @c: Bignum; modulus
 * @d: Bignum; used to store the result of a^b (mod c)
 * Returns: 0 on success, -1 on failure
 */
int crypto_bignum_exptmod(const struct crypto_bignum *a,
        const struct crypto_bignum *b,
        const struct crypto_bignum *c,
        struct crypto_bignum *d);

/**
 * crypto_bignum_inverse - Inverse a bignum so that a * c = 1 (mod b)
 * @a: Bignum
 * @b: Bignum
 * @c: Bignum; used to store the result
 * Returns: 0 on success, -1 on failure
 */
int crypto_bignum_inverse(const struct crypto_bignum *a,
        const struct crypto_bignum *b,
        struct crypto_bignum *c);

/**
 * crypto_bignum_sub - c = a - b
 * @a: Bignum
 * @b: Bignum
 * @c: Bignum; used to store the result of a - b
 * Returns: 0 on success, -1 on failure
 */
int crypto_bignum_sub(const struct crypto_bignum *a,
        const struct crypto_bignum *b,
        struct crypto_bignum *c);

/**
 * crypto_bignum_div - c = a / b
 * @a: Bignum
 * @b: Bignum
 * @c: Bignum; used to store the result of a / b
 * Returns: 0 on success, -1 on failure
 */
int crypto_bignum_div(const struct crypto_bignum *a,
        const struct crypto_bignum *b,
        struct crypto_bignum *c);

/**
 * crypto_bignum_mulmod - d = a * b (mod c)
 * @a: Bignum
 * @b: Bignum
 * @c: Bignum
 * @d: Bignum; used to store the result of (a * b) % c
 * Returns: 0 on success, -1 on failure
 */
int crypto_bignum_mulmod(const struct crypto_bignum *a,
        const struct crypto_bignum *b,
        const struct crypto_bignum *c,
        struct crypto_bignum *d);

/**
 * crypto_bignum_cmp - Compare two bignums
 * @a: Bignum
 * @b: Bignum
 * Returns: -1 if a < b, 0 if a == b, or 1 if a > b
 */
int crypto_bignum_cmp(const struct crypto_bignum *a,
        const struct crypto_bignum *b);

/**
 * crypto_bignum_bits - Get size of a bignum in bits
 * @a: Bignum
 * Returns: Number of bits in the bignum
 */
int crypto_bignum_bits(const struct crypto_bignum *a);

/**
 * crypto_bignum_is_zero - Is the given bignum zero
 * @a: Bignum
 * Returns: 1 if @a is zero or 0 if not
 */
int crypto_bignum_is_zero(const struct crypto_bignum *a);

/**
 * crypto_bignum_is_one - Is the given bignum one
 * @a: Bignum
 * Returns: 1 if @a is one or 0 if not
 */
int crypto_bignum_is_one(const struct crypto_bignum *a);

/**
 * crypto_bignum_legendre - Compute the Legendre symbol (a/p)
 * @a: Bignum
 * @p: Bignum
 * Returns: Legendre symbol -1,0,1 on success; -2 on calculation failure
 */
int crypto_bignum_legendre(const struct crypto_bignum *a,
        const struct crypto_bignum *p);


/**
 * struct crypto_ec - Elliptic curve context
 *
 * Internal data structure for EC implementation. The contents is specific
 * to the used crypto library.
 */
struct crypto_ec;

/**
 * crypto_ec_init - Initialize elliptic curve context
 * @group: Identifying number for the ECC group (IANA "Group Description"
 *  attribute registrty for RFC 2409)
 * Returns: Pointer to EC context or %NULL on failure
 */
struct crypto_ec * crypto_ec_init(int group);

/**
 * crypto_ec_deinit - Deinitialize elliptic curve context
 * @e: EC context from crypto_ec_init()
 */
void crypto_ec_deinit(struct crypto_ec *e);

/**
 * crypto_ec_prime_len - Get length of the prime in octets
 * @e: EC context from crypto_ec_init()
 * Returns: Length of the prime defining the group
 */
size_t crypto_ec_prime_len(struct crypto_ec *e);

/**
 * crypto_ec_prime_len_bits - Get length of the prime in bits
 * @e: EC context from crypto_ec_init()
 * Returns: Length of the prime defining the group in bits
 */
size_t crypto_ec_prime_len_bits(struct crypto_ec *e);

/**
 * crypto_ec_get_prime - Get prime defining an EC group
 * @e: EC context from crypto_ec_init()
 * Returns: Prime (bignum) defining the group
 */
const struct crypto_bignum * crypto_ec_get_prime(struct crypto_ec *e);

/**
 * crypto_ec_get_order - Get order of an EC group
 * @e: EC context from crypto_ec_init()
 * Returns: Order (bignum) of the group
 */
const struct crypto_bignum * crypto_ec_get_order(struct crypto_ec *e);

/**
 * struct crypto_ec_point - Elliptic curve point
 *
 * Internal data structure for EC implementation to represent a point. The
 * contents is specific to the used crypto library.
 */
struct crypto_ec_point;

/**
 * crypto_ec_point_init - Initialize data for an EC point
 * @e: EC context from crypto_ec_init()
 * Returns: Pointer to EC point data or %NULL on failure
 */
struct crypto_ec_point * crypto_ec_point_init(struct crypto_ec *e);

/**
 * crypto_ec_point_deinit - Deinitialize EC point data
 * @p: EC point data from crypto_ec_point_init()
 * @clear: Whether to clear the EC point value from memory
 */
void crypto_ec_point_deinit(struct crypto_ec_point *p, int clear);

/**
 * crypto_ec_point_to_bin - Write EC point value as binary data
 * @e: EC context from crypto_ec_init()
 * @p: EC point data from crypto_ec_point_init()
 * @x: Buffer for writing the binary data for x coordinate or %NULL if not used
 * @y: Buffer for writing the binary data for y coordinate or %NULL if not used
 * Returns: 0 on success, -1 on failure
 *
 * This function can be used to write an EC point as binary data in a format
 * that has the x and y coordinates in big endian byte order fields padded to
 * the length of the prime defining the group.
 */
int crypto_ec_point_to_bin(struct crypto_ec *e,
        const struct crypto_ec_point *point, u8 *x, u8 *y);

/**
 * crypto_ec_point_from_bin - Create EC point from binary data
 * @e: EC context from crypto_ec_init()
 * @val: Binary data to read the EC point from
 * Returns: Pointer to EC point data or %NULL on failure
 *
 * This function readers x and y coordinates of the EC point from the provided
 * buffer assuming the values are in big endian byte order with fields padded to
 * the length of the prime defining the group.
 */
struct crypto_ec_point * crypto_ec_point_from_bin(struct crypto_ec *e,
        const u8 *val);

/**
 * crypto_bignum_add - c = a + b
 * @e: EC context from crypto_ec_init()
 * @a: Bignum
 * @b: Bignum
 * @c: Bignum; used to store the result of a + b
 * Returns: 0 on success, -1 on failure
 */
int crypto_ec_point_add(struct crypto_ec *e, const struct crypto_ec_point *a,
        const struct crypto_ec_point *b,
        struct crypto_ec_point *c);

/**
 * crypto_bignum_mul - res = b * p
 * @e: EC context from crypto_ec_init()
 * @p: EC point
 * @b: Bignum
 * @res: EC point; used to store the result of b * p
 * Returns: 0 on success, -1 on failure
 */
int crypto_ec_point_mul(struct crypto_ec *e, const struct crypto_ec_point *p,
        const struct crypto_bignum *b,
        struct crypto_ec_point *res);

/**
 * crypto_ec_point_invert - Compute inverse of an EC point
 * @e: EC context from crypto_ec_init()
 * @p: EC point to invert (and result of the operation)
 * Returns: 0 on success, -1 on failure
 */
int crypto_ec_point_invert(struct crypto_ec *e, struct crypto_ec_point *p);

/**
 * crypto_ec_point_solve_y_coord - Solve y coordinate for an x coordinate
 * @e: EC context from crypto_ec_init()
 * @p: EC point to use for the returning the result
 * @x: x coordinate
 * @y_bit: y-bit (0 or 1) for selecting the y value to use
 * Returns: 0 on success, -1 on failure
 */
int crypto_ec_point_solve_y_coord(struct crypto_ec *e,
        struct crypto_ec_point *p,
        const struct crypto_bignum *x, int y_bit);

/**
 * crypto_ec_point_compute_y_sqr - Compute y^2 = x^3 + ax + b
 * @e: EC context from crypto_ec_init()
 * @x: x coordinate
 * Returns: y^2 on success, %NULL failure
 */
struct crypto_bignum *
crypto_ec_point_compute_y_sqr(struct crypto_ec *e,
        const struct crypto_bignum *x);

/**
 * crypto_ec_point_is_at_infinity - Check whether EC point is neutral element
 * @e: EC context from crypto_ec_init()
 * @p: EC point
 * Returns: 1 if the specified EC point is the neutral element of the group or
 *   0 if not
 */
int crypto_ec_point_is_at_infinity(struct crypto_ec *e,
        const struct crypto_ec_point *p);

/**
 * crypto_ec_point_is_on_curve - Check whether EC point is on curve
 * @e: EC context from crypto_ec_init()
 * @p: EC point
 * Returns: 1 if the specified EC point is on the curve or 0 if not
 */
int crypto_ec_point_is_on_curve(struct crypto_ec *e,
        const struct crypto_ec_point *p);

/**
 * crypto_ec_point_cmp - Compare two EC points
 * @e: EC context from crypto_ec_init()
 * @a: EC point
 * @b: EC point
 * Returns: 0 on equal, non-zero otherwise
 */
int crypto_ec_point_cmp(const struct crypto_ec *e,
        const struct crypto_ec_point *a,
        const struct crypto_ec_point *b);

/**
 * crypto_ec_get_publickey_buf - Write EC public key to buffer
 * @key: crypto key
 * @key_buf: key buffer
 * @len: length of buffer
 * Returns: 0 on success, non-zero otherwise
 */
int crypto_ec_get_publickey_buf(struct crypto_key *key, u8 *key_buf, int len);

/**
 * crypto_ec_get_group_from_key - Write EC group from key
 * @key: crypto key
 * Returns: EC group
 */
struct crypto_ec_group *crypto_ec_get_group_from_key(struct crypto_key *key);

/**
 * crypto_ec_get_private_key - Get EC private key (in bignum format)
 * @key: crypto key
 * Returns: Private key
 */
struct crypto_bignum *crypto_ec_get_private_key(struct crypto_key *key);

/**
 * crypto_ec_get_key - Read key from character stream
 * @privkey: Private key
 * @privkey_len: private key len
 * Returns: Crypto key
 */
struct crypto_key *crypto_ec_get_key(const u8 *privkey, size_t privkey_len);

/**
 * crypto_ec_get_mbedtls_to_nist_group_id - get nist group from mbedtls internal group
 * @id: mbedtls group
 * Returns: NIST group
 */
unsigned int crypto_ec_get_mbedtls_to_nist_group_id(int id);

/**
 * crypto_ec_get_curve_id - get curve id from ec group
 * @group: EC group
 * Returns: curve ID
 */
int crypto_ec_get_curve_id(const struct crypto_ec_group *group);

/**
 * crypto_ecdh: crypto ecdh
 * @key_own: own key
 * @key_peer: peer key
 * @secret: secret
 * @secret_len: secret len
 * Returns: 0 if success else negative value
 */
int crypto_ecdh(struct crypto_key *key_own, struct crypto_key *key_peer,
		    u8 *secret, size_t *secret_len);

/**
 * crypto_ecdsa_get_sign: get crypto ecdsa signed hash
 * @hash: signed hash
 * @r: ecdsa r
 * @s: ecdsa s
 * @csign: csign
 * @hash_len: length of hash
 * Return: 0 if success else negative value
 */
int crypto_ecdsa_get_sign(unsigned char *hash,
		const struct crypto_bignum *r, const struct crypto_bignum *s,
		struct crypto_key *csign, int hash_len);

/**
 * crypto_edcsa_sign_verify: verify crypto ecdsa signed hash
 * @hash: signed hash
 * @r: ecdsa r
 * @s: ecdsa s
 * @csign: csign
 * @hlen: length of hash
 * Return: 0 if success else negative value
 */
int crypto_edcsa_sign_verify(const unsigned char *hash, const struct crypto_bignum *r,
			const struct crypto_bignum *s, struct crypto_key *csign, int hlen);

/**
 * crypto_ec_parse_subpub_key: get EC key context from sub public key
 * @p: data
 * @len: data len
 * Return: crypto_key
 */
struct crypto_key *crypto_ec_parse_subpub_key(const unsigned char *p, size_t len);

/**
 * crypto_is_ec_key: check whether a key is EC key or not
 * @key: crypto key
 * Return: true if key else false
 */
int crypto_is_ec_key(struct crypto_key *key);

/**
 * crypto_ec_gen_keypair: generate crypto ec keypair
 * @ike_group: grpup
 * Return: crypto key
 */
struct crypto_key * crypto_ec_gen_keypair(u16 ike_group);

/**
 * crypto_ec_write_pub_key: return public key in charater buffer
 * @key: crypto key
 * @der_len: buffer len
 * Return: public key buffer
 */
int crypto_ec_write_pub_key(struct crypto_key *key, unsigned char **key_buf);

/**
 * crypto_ec_set_pubkey_point: set bignum point on ec curve
 * @group: ec group
 * @buf: x,y coordinate
 * @len: length of x and y coordiate
 * Return : crypto key
 */
struct crypto_key * crypto_ec_set_pubkey_point(const struct crypto_ec_group *group,
					     const u8 *buf, size_t len);
/**
 * crypto_ec_free_key: free crypto key
 * Return : None
 */
void crypto_ec_free_key(struct crypto_key *key);
/**
 * crypto_debug_print_ec_key: print ec key
 * @title: title
 * @key: crypto key
 * Return: None
 */
void crypto_debug_print_ec_key(const char *title, struct crypto_key *key);

/**
 * crypto_ec_get_public_key: Public key from crypto key
 * @key: crypto key
 * Return : Public key
 */
struct crypto_ec_point *crypto_ec_get_public_key(struct crypto_key *key);

/**
 * crypto_get_order: free crypto key
 * Return : None
 */
int crypto_get_order(struct crypto_ec_group *group, struct crypto_bignum *x);

/**
 * crypto_bignum_addmod: a = (b + c) mod d
 * Return : 0 in success
 */
int crypto_bignum_addmod(struct crypto_bignum *a,
                      struct crypto_bignum *b,
                      struct crypto_bignum *c,
                      struct crypto_bignum *d);

/**
 * crypto_ec_get_affine_coordinates : get affine corrdinate of ec curve
 * @e: ec curve
 * @pt: point
 * @x: x coordinate
 * @y: y coordinate
 * Return : 0 if success
 */
int crypto_ec_get_affine_coordinates(struct crypto_ec *e, struct crypto_ec_point *pt,
        struct crypto_bignum *x, struct crypto_bignum *y);

/**
 * crypto_ec_get_group_byname: get ec curve group by name
 * @name: ec curve name
 * Return : EC group
 */
struct crypto_ec_group *crypto_ec_get_group_byname(const char *name);

/**
 * crypto_key_compare: check whether two keys belong to same
 * Return : 1 if yes else 0
 */
int crypto_key_compare(struct crypto_key *key1, struct crypto_key *key2);

/*
 * crypto_write_pubkey_der: get public key in der format
 * @csign: key
 * @key_buf: key buffer in charater format
 * Return : len of char buffer if success
 */
int crypto_write_pubkey_der(struct crypto_key *csign, unsigned char **key_buf);

/**
 * crypto_free_buffer: free buffer allocated by crypto API
 * @buf: buffer pointer
 * Return : None
 */
void crypto_free_buffer(unsigned char *buf);

/**
 * @crypto_ec_get_priv_key_der: get private key in der format
 * @key: key structure
 * @key_data: key data in charater buffer
 * @key_len = key lenght of charater buffer
 * Return : 0 if success
 */
int crypto_ec_get_priv_key_der(struct crypto_key *key, unsigned char **key_data, int *key_len);

/**
 * crypto_bignum_to_string: get big number in ascii format
 * @a: big number
 * @buf: buffer in which number will written to
 * @buflen: buffer length
 * @padlen: padding length
 * Return : 0 if success
 */
int crypto_bignum_to_string(const struct crypto_bignum *a,
                         u8 *buf, size_t buflen, size_t padlen);
#endif /* CRYPTO_H */
