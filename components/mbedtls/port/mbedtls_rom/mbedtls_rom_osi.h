/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "mbedtls/aes.h"
#include "mbedtls/asn1.h"
#include "mbedtls/asn1write.h"
#include "mbedtls/base64.h"
#include "mbedtls/bignum.h"
#include "mbedtls/ccm.h"
#include "mbedtls/cipher.h"
#include "mbedtls/cmac.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/dhm.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/ecjpake.h"
#include "mbedtls/ecp.h"
#include "mbedtls/entropy.h"
#include "mbedtls/hmac_drbg.h"
#include "mbedtls/md.h"
#include "mbedtls/oid.h"
#include "mbedtls/pem.h"
#include "mbedtls/pkcs12.h"
#include "mbedtls/pkcs5.h"
#include "mbedtls/pk.h"
#include "mbedtls/platform.h"
#include "mbedtls/rsa.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
#include "mbedtls/ssl_ciphersuites.h"
#include "mbedtls/ssl.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/x509.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (!defined(CONFIG_MBEDTLS_THREADING_C))
typedef struct mbedtls_threading_mutex_t {
    int dummy;
} mbedtls_threading_mutex_t;
#endif

typedef struct mbedtls_rom_funcs {
    void (*_rom_mbedtls_aes_init)( mbedtls_aes_context *ctx );
    int (*_rom_ssl_write_client_hello)( mbedtls_ssl_context *ssl );
    int (*_rom_ssl_parse_server_hello)( mbedtls_ssl_context *ssl );
    int (*_rom_ssl_parse_server_key_exchange)( mbedtls_ssl_context *ssl );
    int (*_rom_ssl_parse_certificate_request)( mbedtls_ssl_context *ssl );
    int (*_rom_ssl_parse_server_hello_done)( mbedtls_ssl_context *ssl );
    int (*_rom_ssl_write_client_key_exchange)( mbedtls_ssl_context *ssl );
    int (*_rom_ssl_write_certificate_verify)( mbedtls_ssl_context *ssl );
    int (*_rom_ssl_parse_new_session_ticket)( mbedtls_ssl_context *ssl );
    void (*_rom_mbedtls_aes_free)( mbedtls_aes_context *ctx );
    int (*_rom_mbedtls_aes_setkey_enc)( mbedtls_aes_context *ctx, const unsigned char *key, unsigned int keybits );
    int (*_rom_mbedtls_aes_setkey_dec)( mbedtls_aes_context *ctx, const unsigned char *key, unsigned int keybits );
    int (*_rom_mbedtls_aes_crypt_ecb)( mbedtls_aes_context *ctx, int mode, const unsigned char input[16], unsigned char output[16] );
    int (*_rom_mbedtls_aes_crypt_cbc)( mbedtls_aes_context *ctx, int mode, size_t length, unsigned char iv[16], const unsigned char *input, unsigned char *output );
    int (*_rom_mbedtls_internal_aes_encrypt)( mbedtls_aes_context *ctx, const unsigned char input[16], unsigned char output[16] );
    int (*_rom_mbedtls_internal_aes_decrypt)( mbedtls_aes_context *ctx, const unsigned char input[16], unsigned char output[16] );
    int (*_rom_mbedtls_asn1_get_len)( unsigned char **p, const unsigned char *end, size_t *len );
    int (*_rom_mbedtls_asn1_get_tag)( unsigned char **p, const unsigned char *end, size_t *len, int tag );
    int (*_rom_mbedtls_asn1_get_bool)( unsigned char **p, const unsigned char *end, int *val );
    int (*_rom_mbedtls_asn1_get_int)( unsigned char **p, const unsigned char *end, int *val );
    int (*_rom_mbedtls_asn1_get_bitstring)( unsigned char **p, const unsigned char *end, mbedtls_asn1_bitstring *bs);
    int (*_rom_mbedtls_asn1_get_bitstring_null)( unsigned char **p, const unsigned char *end, size_t *len );
    int (*_rom_mbedtls_asn1_get_sequence_of)( unsigned char **p, const unsigned char *end, mbedtls_asn1_sequence *cur, int tag);
    int (*_rom_mbedtls_asn1_get_mpi)( unsigned char **p, const unsigned char *end, mbedtls_mpi *X );
    int (*_rom_mbedtls_asn1_get_alg)( unsigned char **p, const unsigned char *end, mbedtls_asn1_buf *alg, mbedtls_asn1_buf *params );
    int (*_rom_mbedtls_asn1_get_alg_null)( unsigned char **p, const unsigned char *end, mbedtls_asn1_buf *alg );
    int (*_rom_mbedtls_asn1_write_len)( unsigned char **p, const unsigned char *start, size_t len );
    int (*_rom_mbedtls_asn1_write_tag)( unsigned char **p, const unsigned char *start, unsigned char tag );
    int (*_rom_mbedtls_asn1_write_mpi)( unsigned char **p, const unsigned char *start, const mbedtls_mpi *X );
    int (*_rom_mbedtls_base64_decode)( unsigned char *dst, size_t dlen, size_t *olen, const unsigned char *src, size_t slen );
    void (*_rom_mbedtls_mpi_init)( mbedtls_mpi *X );
    void (*_rom_mbedtls_mpi_free)( mbedtls_mpi *X );
    int (*_rom_mbedtls_mpi_grow)( mbedtls_mpi *X, size_t nblimbs );
    int (*_rom_mbedtls_mpi_shrink)( mbedtls_mpi *X, size_t nblimbs );
    int (*_rom_mbedtls_mpi_copy)( mbedtls_mpi *X, const mbedtls_mpi *Y );
    int (*_rom_mbedtls_mpi_safe_cond_assign)( mbedtls_mpi *X, const mbedtls_mpi *Y, unsigned char assign );
    int (*_rom_mbedtls_mpi_safe_cond_swap)( mbedtls_mpi *X, mbedtls_mpi *Y, unsigned char assign );
    int (*_rom_mbedtls_mpi_lset)( mbedtls_mpi *X, mbedtls_mpi_sint z );
    int (*_rom_mbedtls_mpi_get_bit)( const mbedtls_mpi *X, size_t pos );
    int (*_rom_mbedtls_mpi_set_bit)( mbedtls_mpi *X, size_t pos, unsigned char val );
    size_t (*_rom_mbedtls_mpi_lsb)( const mbedtls_mpi *X );
    size_t (*_rom_mbedtls_mpi_bitlen)( const mbedtls_mpi *X );
    size_t (*_rom_mbedtls_mpi_size)( const mbedtls_mpi *X );
    int (*_rom_mbedtls_mpi_read_binary)( mbedtls_mpi *X, const unsigned char *buf, size_t buflen );
    int (*_rom_mbedtls_mpi_write_binary)( const mbedtls_mpi *X, unsigned char *buf, size_t buflen );
    int (*_rom_mbedtls_mpi_shift_l)( mbedtls_mpi *X, size_t count );
    int (*_rom_mbedtls_mpi_shift_r)( mbedtls_mpi *X, size_t count );
    int (*_rom_mbedtls_mpi_cmp_abs)( const mbedtls_mpi *X, const mbedtls_mpi *Y );
    int (*_rom_mbedtls_mpi_cmp_mpi)( const mbedtls_mpi *X, const mbedtls_mpi *Y );
    int (*_rom_mbedtls_mpi_lt_mpi_ct)( const mbedtls_mpi *X, const mbedtls_mpi *Y, unsigned *ret );
    int (*_rom_mbedtls_mpi_cmp_int)( const mbedtls_mpi *X, mbedtls_mpi_sint z );
    int (*_rom_mbedtls_mpi_add_abs)( mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *B );
    int (*_rom_mbedtls_mpi_sub_abs)( mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *B );
    int (*_rom_mbedtls_mpi_add_mpi)( mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *B );
    int (*_rom_mbedtls_mpi_sub_mpi)( mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *B );
    int (*_rom_mbedtls_mpi_add_int)( mbedtls_mpi *X, const mbedtls_mpi *A, mbedtls_mpi_sint b );
    int (*_rom_mbedtls_mpi_sub_int)( mbedtls_mpi *X, const mbedtls_mpi *A, mbedtls_mpi_sint b );
    int (*_rom_mbedtls_mpi_mul_mpi)( mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *B );
    int (*_rom_mbedtls_mpi_mul_int)( mbedtls_mpi *X, const mbedtls_mpi *A, mbedtls_mpi_uint b );
    int (*_rom_mbedtls_mpi_div_mpi)( mbedtls_mpi *Q, mbedtls_mpi *R, const mbedtls_mpi *A, const mbedtls_mpi *B );
    int (*_rom_mbedtls_mpi_div_int)( mbedtls_mpi *Q, mbedtls_mpi *R, const mbedtls_mpi *A, mbedtls_mpi_sint b );
    int (*_rom_mbedtls_mpi_mod_mpi)( mbedtls_mpi *R, const mbedtls_mpi *A, const mbedtls_mpi *B );
    int (*_rom_mbedtls_mpi_mod_int)( mbedtls_mpi_uint *r, const mbedtls_mpi *A, mbedtls_mpi_sint b );
    int (*_rom_mbedtls_mpi_exp_mod)( mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *E, const mbedtls_mpi *N, mbedtls_mpi *_RR );
    int (*_rom_mbedtls_mpi_fill_random)( mbedtls_mpi *X, size_t size, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );
    int (*_rom_mbedtls_mpi_gcd)( mbedtls_mpi *G, const mbedtls_mpi *A, const mbedtls_mpi *B );
    int (*_rom_mbedtls_mpi_inv_mod)( mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *N );
    int (*_rom_mbedtls_mpi_is_prime_ext)( const mbedtls_mpi *X, int rounds, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );
    int (*_rom_mbedtls_ccm_star_encrypt_and_tag)( mbedtls_ccm_context *ctx, size_t length, const unsigned char *iv, size_t iv_len, const unsigned char *add, size_t add_len, const unsigned char *input, unsigned char *output, unsigned char *tag, size_t tag_len );
    int (*_rom_mbedtls_ccm_star_auth_decrypt)( mbedtls_ccm_context *ctx, size_t length, const unsigned char *iv, size_t iv_len, const unsigned char *add, size_t add_len, const unsigned char *input, unsigned char *output, const unsigned char *tag, size_t tag_len );
    void (*_rom_mbedtls_cipher_init)( mbedtls_cipher_context_t *ctx );
    int (*_rom_mbedtls_cipher_set_padding_mode)( mbedtls_cipher_context_t *ctx, mbedtls_cipher_padding_t mode );
    int (*_rom_mbedtls_cipher_reset)( mbedtls_cipher_context_t *ctx );
    int (*_rom_mbedtls_cipher_finish)( mbedtls_cipher_context_t *ctx, unsigned char *output, size_t *olen );
    int (*_rom_mbedtls_cipher_crypt)( mbedtls_cipher_context_t *ctx, const unsigned char *iv, size_t iv_len, const unsigned char *input, size_t ilen, unsigned char *output, size_t *olen );
    int (*_rom_mbedtls_cipher_cmac_starts)( mbedtls_cipher_context_t *ctx, const unsigned char *key, size_t keybits );
    int (*_rom_mbedtls_cipher_cmac_update)( mbedtls_cipher_context_t *ctx, const unsigned char *input, size_t ilen );
    int (*_rom_mbedtls_cipher_cmac_finish)( mbedtls_cipher_context_t *ctx, unsigned char *output );
    void (*_rom_mbedtls_ctr_drbg_init)( mbedtls_ctr_drbg_context *ctx );
    int (*_rom_mbedtls_ctr_drbg_seed)( mbedtls_ctr_drbg_context *ctx, int (*f_entropy)(void *, unsigned char *, size_t), void *p_entropy, const unsigned char *custom, size_t len );
    void (*_rom_mbedtls_ctr_drbg_free)( mbedtls_ctr_drbg_context *ctx );
    int (*_rom_mbedtls_ctr_drbg_reseed)( mbedtls_ctr_drbg_context *ctx, const unsigned char *additional, size_t len );
    int (*_rom_mbedtls_ctr_drbg_random_with_add)( void *p_rng, unsigned char *output, size_t output_len, const unsigned char *additional, size_t add_len );
    int (*_rom_mbedtls_ctr_drbg_random)( void *p_rng, unsigned char *output, size_t output_len );
    void (*_rom_mbedtls_dhm_init)( mbedtls_dhm_context *ctx );
    int (*_rom_mbedtls_dhm_read_params)( mbedtls_dhm_context *ctx, unsigned char **p, const unsigned char *end );
    int (*_rom_mbedtls_dhm_make_public)( mbedtls_dhm_context *ctx, int x_size, unsigned char *output, size_t olen, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );
    int (*_rom_mbedtls_dhm_calc_secret)( mbedtls_dhm_context *ctx, unsigned char *output, size_t output_size, size_t *olen, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );
    void (*_rom_mbedtls_dhm_free)( mbedtls_dhm_context *ctx );
    void (*_rom_mbedtls_ecdh_init)( mbedtls_ecdh_context *ctx );
    int (*_rom_mbedtls_ecdh_setup)( mbedtls_ecdh_context *ctx, mbedtls_ecp_group_id grp_id );
    void (*_rom_mbedtls_ecdh_free)( mbedtls_ecdh_context *ctx );
    int (*_rom_mbedtls_ecdh_read_params)( mbedtls_ecdh_context *ctx, const unsigned char **buf, const unsigned char *end );
    int (*_rom_mbedtls_ecdh_get_params)( mbedtls_ecdh_context *ctx, const mbedtls_ecp_keypair *key, mbedtls_ecdh_side side );
    int (*_rom_mbedtls_ecdh_make_public)( mbedtls_ecdh_context *ctx, size_t *olen, unsigned char *buf, size_t blen, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );
    int (*_rom_mbedtls_ecdh_calc_secret)( mbedtls_ecdh_context *ctx, size_t *olen, unsigned char *buf, size_t blen, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );
    void (*_rom_mbedtls_ecdh_enable_restart)( mbedtls_ecdh_context *ctx );
    int (*_rom_mbedtls_ecdsa_write_signature)( mbedtls_ecdsa_context *ctx, mbedtls_md_type_t md_alg, const unsigned char *hash, size_t hlen, unsigned char *sig, size_t *slen, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );
    int (*_rom_mbedtls_ecdsa_write_signature_restartable)( mbedtls_ecdsa_context *ctx, mbedtls_md_type_t md_alg, const unsigned char *hash, size_t hlen, unsigned char *sig, size_t *slen, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, mbedtls_ecdsa_restart_ctx *rs_ctx );
    int (*_rom_mbedtls_ecdsa_read_signature)( mbedtls_ecdsa_context *ctx, const unsigned char *hash, size_t hlen, const unsigned char *sig, size_t slen );
    int (*_rom_mbedtls_ecdsa_read_signature_restartable)( mbedtls_ecdsa_context *ctx, const unsigned char *hash, size_t hlen, const unsigned char *sig, size_t slen, mbedtls_ecdsa_restart_ctx *rs_ctx );
    int (*_rom_mbedtls_ecdsa_from_keypair)( mbedtls_ecdsa_context *ctx, const mbedtls_ecp_keypair *key );
    void (*_rom_mbedtls_ecdsa_init)( mbedtls_ecdsa_context *ctx );
    void (*_rom_mbedtls_ecdsa_free)( mbedtls_ecdsa_context *ctx );
    void (*_rom_mbedtls_ecdsa_restart_init)( mbedtls_ecdsa_restart_ctx *ctx );
    void (*_rom_mbedtls_ecdsa_restart_free)( mbedtls_ecdsa_restart_ctx *ctx );
    void (*_rom_mbedtls_ecjpake_init)( mbedtls_ecjpake_context *ctx );
    int (*_rom_mbedtls_ecjpake_check)( const mbedtls_ecjpake_context *ctx );
    int (*_rom_mbedtls_ecjpake_write_round_one)( mbedtls_ecjpake_context *ctx, unsigned char *buf, size_t len, size_t *olen, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );
    int (*_rom_mbedtls_ecjpake_read_round_one)( mbedtls_ecjpake_context *ctx, const unsigned char *buf, size_t len );
    int (*_rom_mbedtls_ecjpake_write_round_two)( mbedtls_ecjpake_context *ctx, unsigned char *buf, size_t len, size_t *olen, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );
    int (*_rom_mbedtls_ecjpake_read_round_two)( mbedtls_ecjpake_context *ctx, const unsigned char *buf, size_t len );
    int (*_rom_mbedtls_ecjpake_derive_secret)( mbedtls_ecjpake_context *ctx, unsigned char *buf, size_t len, size_t *olen, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );
    void (*_rom_mbedtls_ecjpake_free)( mbedtls_ecjpake_context *ctx );
    int (*_rom_mbedtls_ecp_check_budget)( const mbedtls_ecp_group *grp, mbedtls_ecp_restart_ctx *rs_ctx, unsigned ops );
    int (*_rom_mbedtls_ecp_restart_is_enabled)( void );
    const mbedtls_ecp_curve_info *(*_rom_mbedtls_ecp_curve_list)( void );
    const mbedtls_ecp_group_id *(*_rom_mbedtls_ecp_grp_id_list)( void );
    const mbedtls_ecp_curve_info *(*_rom_mbedtls_ecp_curve_info_from_grp_id)( mbedtls_ecp_group_id grp_id );
    const mbedtls_ecp_curve_info *(*_rom_mbedtls_ecp_curve_info_from_tls_id)( uint16_t tls_id );
    void (*_rom_mbedtls_ecp_point_init)( mbedtls_ecp_point *pt );
    void (*_rom_mbedtls_ecp_group_init)( mbedtls_ecp_group *grp );
    void (*_rom_mbedtls_ecp_keypair_init)( mbedtls_ecp_keypair *key );
    void (*_rom_mbedtls_ecp_point_free)( mbedtls_ecp_point *pt );
    void (*_rom_mbedtls_ecp_group_free)( mbedtls_ecp_group *grp );
    void (*_rom_mbedtls_ecp_keypair_free)( mbedtls_ecp_keypair *key );
    void (*_rom_mbedtls_ecp_restart_init)( mbedtls_ecp_restart_ctx *ctx );
    void (*_rom_mbedtls_ecp_restart_free)( mbedtls_ecp_restart_ctx *ctx );
    int (*_rom_mbedtls_ecp_copy)( mbedtls_ecp_point *P, const mbedtls_ecp_point *Q );
    int (*_rom_mbedtls_ecp_group_copy)( mbedtls_ecp_group *dst, const mbedtls_ecp_group *src );
    int (*_rom_mbedtls_ecp_set_zero)( mbedtls_ecp_point *pt );
    int (*_rom_mbedtls_ecp_is_zero)( mbedtls_ecp_point *pt );
    int (*_rom_mbedtls_ecp_point_cmp)( const mbedtls_ecp_point *P, const mbedtls_ecp_point *Q );
    int (*_rom_mbedtls_ecp_point_write_binary)( const mbedtls_ecp_group *grp, const mbedtls_ecp_point *P, int format, size_t *olen, unsigned char *buf, size_t buflen );
    int (*_rom_mbedtls_ecp_point_read_binary)( const mbedtls_ecp_group *grp, mbedtls_ecp_point *P, const unsigned char *buf, size_t ilen );
    int (*_rom_mbedtls_ecp_tls_read_point)( const mbedtls_ecp_group *grp, mbedtls_ecp_point *pt, const unsigned char **buf, size_t len );
    int (*_rom_mbedtls_ecp_tls_write_point)( const mbedtls_ecp_group *grp, const mbedtls_ecp_point *pt, int format, size_t *olen, unsigned char *buf, size_t blen );
    int (*_rom_mbedtls_ecp_group_load)( mbedtls_ecp_group *grp, mbedtls_ecp_group_id id );
    int (*_rom_mbedtls_ecp_tls_read_group)( mbedtls_ecp_group *grp, const unsigned char **buf, size_t len );
    int (*_rom_mbedtls_ecp_tls_read_group_id)( mbedtls_ecp_group_id *grp, const unsigned char **buf, size_t len );
    int (*_rom_mbedtls_ecp_tls_write_group)( const mbedtls_ecp_group *grp, size_t *olen, unsigned char *buf, size_t blen );
    int (*_rom_mbedtls_ecp_mul)( mbedtls_ecp_group *grp, mbedtls_ecp_point *R, const mbedtls_mpi *m, const mbedtls_ecp_point *P, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );
    int (*_rom_mbedtls_ecp_mul_restartable)( mbedtls_ecp_group *grp, mbedtls_ecp_point *R, const mbedtls_mpi *m, const mbedtls_ecp_point *P, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, mbedtls_ecp_restart_ctx *rs_ctx );
    int (*_rom_mbedtls_ecp_muladd)( mbedtls_ecp_group *grp, mbedtls_ecp_point *R, const mbedtls_mpi *m, const mbedtls_ecp_point *P, const mbedtls_mpi *n, const mbedtls_ecp_point *Q );
    int (*_rom_mbedtls_ecp_muladd_restartable)( mbedtls_ecp_group *grp, mbedtls_ecp_point *R, const mbedtls_mpi *m, const mbedtls_ecp_point *P, const mbedtls_mpi *n, const mbedtls_ecp_point *Q, mbedtls_ecp_restart_ctx *rs_ctx );
    int (*_rom_mbedtls_ecp_check_pubkey)( const mbedtls_ecp_group *grp, const mbedtls_ecp_point *pt );
    int (*_rom_mbedtls_ecp_check_privkey)( const mbedtls_ecp_group *grp, const mbedtls_mpi *d );
    int (*_rom_mbedtls_ecp_gen_privkey)( const mbedtls_ecp_group *grp, mbedtls_mpi *d, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );
    int (*_rom_mbedtls_ecp_gen_keypair_base)( mbedtls_ecp_group *grp, const mbedtls_ecp_point *G, mbedtls_mpi *d, mbedtls_ecp_point *Q, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );
    int (*_rom_mbedtls_ecp_check_pub_priv)( const mbedtls_ecp_keypair *pub, const mbedtls_ecp_keypair *prv );
    int (*_rom_mbedtls_reserved0)(void);
    int (*_rom_mbedtls_reserved1)(void);
    int (*_rom_mbedtls_gcm_crypt_and_tag)( mbedtls_gcm_context *ctx, int mode, size_t length, const unsigned char *iv, size_t iv_len, const unsigned char *add, size_t add_len, const unsigned char *input, unsigned char *output, size_t tag_len, unsigned char *tag );
    int (*_rom_mbedtls_gcm_starts)( mbedtls_gcm_context *ctx, int mode, const unsigned char *iv, size_t iv_len, const unsigned char *add, size_t add_len );
    int (*_rom_mbedtls_gcm_update)( mbedtls_gcm_context *ctx, size_t length, const unsigned char *input, unsigned char *output );
    int (*_rom_mbedtls_gcm_finish)( mbedtls_gcm_context *ctx, unsigned char *tag, size_t tag_len );
    void (*_rom_mbedtls_hmac_drbg_init)( mbedtls_hmac_drbg_context *ctx );
    int (*_rom_mbedtls_hmac_drbg_seed_buf)( mbedtls_hmac_drbg_context *ctx, const mbedtls_md_info_t * md_info, const unsigned char *data, size_t data_len );
    int (*_rom_mbedtls_hmac_drbg_update_ret)( mbedtls_hmac_drbg_context *ctx, const unsigned char *additional, size_t add_len );
    int (*_rom_mbedtls_hmac_drbg_reseed)( mbedtls_hmac_drbg_context *ctx, const unsigned char *additional, size_t len );
    int (*_rom_mbedtls_hmac_drbg_random_with_add)( void *p_rng, unsigned char *output, size_t output_len, const unsigned char *additional, size_t add_len );
    int (*_rom_mbedtls_hmac_drbg_random)( void *p_rng, unsigned char *output, size_t out_len );
    void (*_rom_mbedtls_hmac_drbg_free)( mbedtls_hmac_drbg_context *ctx );
    const int *(*_rom_mbedtls_md_list)( void );
    void (*_rom_mbedtls_md_init)( mbedtls_md_context_t *ctx );
    void (*_rom_mbedtls_md_free)( mbedtls_md_context_t *ctx );
    int (*_rom_mbedtls_md_setup)( mbedtls_md_context_t *ctx, const mbedtls_md_info_t *md_info, int hmac );
    int (*_rom_mbedtls_md_clone)( mbedtls_md_context_t *dst, const mbedtls_md_context_t *src );
    unsigned char (*_rom_mbedtls_md_get_size)( const mbedtls_md_info_t *md_info );
    mbedtls_md_type_t (*_rom_mbedtls_md_get_type)( const mbedtls_md_info_t *md_info );
    int (*_rom_mbedtls_md_starts)( mbedtls_md_context_t *ctx );
    int (*_rom_mbedtls_md_update)( mbedtls_md_context_t *ctx, const unsigned char *input, size_t ilen );
    int (*_rom_mbedtls_md_finish)( mbedtls_md_context_t *ctx, unsigned char *output );
    int (*_rom_mbedtls_md)( const mbedtls_md_info_t *md_info, const unsigned char *input, size_t ilen, unsigned char *output );
    int (*_rom_mbedtls_md_hmac_starts)( mbedtls_md_context_t *ctx, const unsigned char *key, size_t keylen );
    int (*_rom_mbedtls_md_hmac_update)( mbedtls_md_context_t *ctx, const unsigned char *input, size_t ilen );
    int (*_rom_mbedtls_md_hmac_finish)( mbedtls_md_context_t *ctx, unsigned char *output);
    int (*_rom_mbedtls_md_hmac_reset)( mbedtls_md_context_t *ctx );
    int (*_rom_mbedtls_oid_get_x509_ext_type)( const mbedtls_asn1_buf *oid, int *ext_type );
    int (*_rom_mbedtls_oid_get_pk_alg)( const mbedtls_asn1_buf *oid, mbedtls_pk_type_t *pk_alg );
    int (*_rom_mbedtls_oid_get_ec_grp)( const mbedtls_asn1_buf *oid, mbedtls_ecp_group_id *grp_id );
    int (*_rom_mbedtls_oid_get_sig_alg)( const mbedtls_asn1_buf *oid, mbedtls_md_type_t *md_alg, mbedtls_pk_type_t *pk_alg );
    int (*_rom_mbedtls_oid_get_md_alg)( const mbedtls_asn1_buf *oid, mbedtls_md_type_t *md_alg );
    int (*_rom_mbedtls_oid_get_md_hmac)( const mbedtls_asn1_buf *oid, mbedtls_md_type_t *md_hmac );
    int (*_rom_mbedtls_oid_get_oid_by_md)( mbedtls_md_type_t md_alg, const char **oid, size_t *olen );
    int (*_rom_mbedtls_oid_get_cipher_alg)( const mbedtls_asn1_buf *oid, mbedtls_cipher_type_t *cipher_alg );
    int (*_rom_mbedtls_oid_get_pkcs12_pbe_alg)( const mbedtls_asn1_buf *oid, mbedtls_md_type_t *md_alg, mbedtls_cipher_type_t *cipher_alg );
    void (*_rom_mbedtls_pem_init)( void *ctx );
    void (*_rom_mbedtls_pem_free)( void *ctx );
    int (*_rom_mbedtls_pkcs12_pbe_sha1_rc4_128)( mbedtls_asn1_buf *pbe_params, int mode, const unsigned char *pwd, size_t pwdlen, const unsigned char *input, size_t len, unsigned char *output );
    int (*_rom_mbedtls_pkcs12_pbe)( mbedtls_asn1_buf *pbe_params, int mode, mbedtls_cipher_type_t cipher_type, mbedtls_md_type_t md_type, const unsigned char *pwd, size_t pwdlen, const unsigned char *input, size_t len, unsigned char *output );
    int (*_rom_mbedtls_pkcs12_derivation)( unsigned char *data, size_t datalen, const unsigned char *pwd, size_t pwdlen, const unsigned char *salt, size_t saltlen, mbedtls_md_type_t mbedtls_md, int id, int iterations );
    int (*_rom_mbedtls_pkcs5_pbes2)( const mbedtls_asn1_buf *pbe_params, int mode, const unsigned char *pwd, size_t pwdlen, const unsigned char *data, size_t datalen, unsigned char *output );
    int (*_rom_mbedtls_pkcs5_pbkdf2_hmac)( mbedtls_md_context_t *ctx, const unsigned char *password, size_t plen, const unsigned char *salt, size_t slen, unsigned int iteration_count, uint32_t key_length, unsigned char *output );
    const mbedtls_pk_info_t *(*_rom_mbedtls_pk_info_from_type)( mbedtls_pk_type_t pk_type );
    void (*_rom_mbedtls_pk_init)( mbedtls_pk_context *ctx );
    void (*_rom_mbedtls_pk_free)( mbedtls_pk_context *ctx );
    void (*_rom_mbedtls_pk_restart_init)( mbedtls_pk_restart_ctx *ctx );
    void (*_rom_mbedtls_pk_restart_free)( mbedtls_pk_restart_ctx *ctx );
    int (*_rom_mbedtls_pk_setup)( mbedtls_pk_context *ctx, const mbedtls_pk_info_t *info );
    int (*_rom_mbedtls_pk_can_do)( const mbedtls_pk_context *ctx, mbedtls_pk_type_t type );
    int (*_rom_mbedtls_pk_verify)( mbedtls_pk_context *ctx, mbedtls_md_type_t md_alg, const unsigned char *hash, size_t hash_len, const unsigned char *sig, size_t sig_len );
    int (*_rom_mbedtls_pk_verify_restartable)( mbedtls_pk_context *ctx, mbedtls_md_type_t md_alg, const unsigned char *hash, size_t hash_len, const unsigned char *sig, size_t sig_len, mbedtls_pk_restart_ctx *rs_ctx );
    int (*_rom_mbedtls_pk_verify_ext)( mbedtls_pk_type_t type, const void *options, mbedtls_pk_context *ctx, mbedtls_md_type_t md_alg, const unsigned char *hash, size_t hash_len, const unsigned char *sig, size_t sig_len );
    int (*_rom_mbedtls_pk_sign_restartable)( mbedtls_pk_context *ctx, mbedtls_md_type_t md_alg, const unsigned char *hash, size_t hash_len, unsigned char *sig, size_t *sig_len, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, mbedtls_pk_restart_ctx *rs_ctx );
    int (*_rom_mbedtls_pk_encrypt)( mbedtls_pk_context *ctx, const unsigned char *input, size_t ilen, unsigned char *output, size_t *olen, size_t osize, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );
    mbedtls_pk_type_t (*_rom_mbedtls_pk_get_type)( const mbedtls_pk_context *ctx );
    int (*_rom_mbedtls_pk_parse_subpubkey)( unsigned char **p, const unsigned char *end, mbedtls_pk_context *pk );
    void (*_rom_mbedtls_rsa_init)( mbedtls_rsa_context *ctx );
    int (*_rom_mbedtls_rsa_import)( mbedtls_rsa_context *ctx, const mbedtls_mpi *N, const mbedtls_mpi *P, const mbedtls_mpi *Q, const mbedtls_mpi *D, const mbedtls_mpi *E );
    int (*_rom_mbedtls_rsa_import_raw)( mbedtls_rsa_context *ctx, unsigned char const *N, size_t N_len, unsigned char const *P, size_t P_len, unsigned char const *Q, size_t Q_len, unsigned char const *D, size_t D_len, unsigned char const *E, size_t E_len );
    int (*_rom_mbedtls_rsa_complete)( mbedtls_rsa_context *ctx );
    int (*_rom_mbedtls_rsa_set_padding)( mbedtls_rsa_context *ctx, int padding, mbedtls_md_type_t hash_id );
    size_t (*_rom_mbedtls_rsa_get_len)( const mbedtls_rsa_context *ctx );
    int (*_rom_mbedtls_rsa_check_pubkey)( const mbedtls_rsa_context *ctx );
    int (*_rom_mbedtls_rsa_check_privkey)( const mbedtls_rsa_context *ctx );
    int (*_rom_mbedtls_rsa_check_pub_priv)( const mbedtls_rsa_context *pub, const mbedtls_rsa_context *prv );
    int (*_rom_mbedtls_rsa_public)( mbedtls_rsa_context *ctx, const unsigned char *input, unsigned char *output );
    int (*_rom_mbedtls_rsa_private)( mbedtls_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, const unsigned char *input, unsigned char *output );
    int (*_rom_mbedtls_rsa_pkcs1_encrypt)( mbedtls_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, int mode, size_t ilen, const unsigned char *input, unsigned char *output );
    int (*_rom_mbedtls_rsa_rsaes_pkcs1_v15_encrypt)( mbedtls_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, int mode, size_t ilen, const unsigned char *input, unsigned char *output );
    int (*_rom_mbedtls_rsa_rsaes_oaep_encrypt)( mbedtls_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, int mode, const unsigned char *label, size_t label_len, size_t ilen, const unsigned char *input, unsigned char *output );
    int (*_rom_mbedtls_rsa_pkcs1_decrypt)( mbedtls_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, int mode, size_t *olen, const unsigned char *input, unsigned char *output, size_t output_max_len );
    int (*_rom_mbedtls_rsa_rsaes_pkcs1_v15_decrypt)( mbedtls_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, int mode, size_t *olen, const unsigned char *input, unsigned char *output, size_t output_max_len );
    int (*_rom_mbedtls_rsa_rsaes_oaep_decrypt)( mbedtls_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, int mode, const unsigned char *label, size_t label_len, size_t *olen, const unsigned char *input, unsigned char *output, size_t output_max_len );
    int (*_rom_mbedtls_rsa_pkcs1_sign)( mbedtls_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, int mode, mbedtls_md_type_t md_alg, unsigned int hashlen, const unsigned char *hash, unsigned char *sig );
    int (*_rom_mbedtls_rsa_rsassa_pkcs1_v15_sign)( mbedtls_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, int mode, mbedtls_md_type_t md_alg, unsigned int hashlen, const unsigned char *hash, unsigned char *sig );
    int (*_rom_mbedtls_rsa_rsassa_pss_sign)( mbedtls_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, int mode, mbedtls_md_type_t md_alg, unsigned int hashlen, const unsigned char *hash, unsigned char *sig );
    int (*_rom_mbedtls_rsa_pkcs1_verify)( mbedtls_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, int mode, mbedtls_md_type_t md_alg, unsigned int hashlen, const unsigned char *hash, const unsigned char *sig );
    int (*_rom_mbedtls_rsa_rsassa_pkcs1_v15_verify)( mbedtls_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, int mode, mbedtls_md_type_t md_alg, unsigned int hashlen, const unsigned char *hash, const unsigned char *sig );
    int (*_rom_mbedtls_rsa_rsassa_pss_verify)( mbedtls_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, int mode, mbedtls_md_type_t md_alg, unsigned int hashlen, const unsigned char *hash, const unsigned char *sig );
    int (*_rom_mbedtls_rsa_rsassa_pss_verify_ext)( mbedtls_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, int mode, mbedtls_md_type_t md_alg, unsigned int hashlen, const unsigned char *hash, mbedtls_md_type_t mgf1_hash_id, int expected_salt_len, const unsigned char *sig );
    void (*_rom_mbedtls_rsa_free)( mbedtls_rsa_context *ctx );
    int (*_rom_mbedtls_rsa_deduce_primes)( mbedtls_mpi const *N, mbedtls_mpi const *E, mbedtls_mpi const *D, mbedtls_mpi *P, mbedtls_mpi *Q );
    int (*_rom_mbedtls_rsa_deduce_private_exponent)( mbedtls_mpi const *P, mbedtls_mpi const *Q, mbedtls_mpi const *E, mbedtls_mpi *D );
    int (*_rom_mbedtls_rsa_deduce_crt)( const mbedtls_mpi *P, const mbedtls_mpi *Q, const mbedtls_mpi *D, mbedtls_mpi *DP, mbedtls_mpi *DQ, mbedtls_mpi *QP );
    int (*_rom_mbedtls_rsa_validate_params)( const mbedtls_mpi *N, const mbedtls_mpi *P, const mbedtls_mpi *Q, const mbedtls_mpi *D, const mbedtls_mpi *E, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );
    int (*_rom_mbedtls_rsa_validate_crt)( const mbedtls_mpi *P, const mbedtls_mpi *Q, const mbedtls_mpi *D, const mbedtls_mpi *DP, const mbedtls_mpi *DQ, const mbedtls_mpi *QP );
    void (*_rom_mbedtls_sha1_init)( mbedtls_sha1_context *ctx );
    void (*_rom_mbedtls_sha1_free)( mbedtls_sha1_context *ctx );
    void (*_rom_mbedtls_sha1_clone)( mbedtls_sha1_context *dst, const mbedtls_sha1_context *src );
    int (*_rom_mbedtls_sha1_starts)( mbedtls_sha1_context *ctx );
    int (*_rom_mbedtls_sha1_finish)( mbedtls_sha1_context *ctx, unsigned char output[20] );
    void (*_rom_mbedtls_sha256_init)( mbedtls_sha256_context *ctx );
    void (*_rom_mbedtls_sha256_free)( mbedtls_sha256_context *ctx );
    void (*_rom_mbedtls_sha256_clone)( mbedtls_sha256_context *dst, const mbedtls_sha256_context *src );
    int (*_rom_mbedtls_sha256_starts)( mbedtls_sha256_context *ctx, int is224 );
    int (*_rom_mbedtls_sha256_finish)( mbedtls_sha256_context *ctx, unsigned char output[32] );
    int (*_rom_mbedtls_sha256)( const unsigned char *input, size_t ilen, unsigned char output[32], int is224 );
    void (*_rom_mbedtls_sha512_init)( mbedtls_sha512_context *ctx );
    void (*_rom_mbedtls_sha512_free)( mbedtls_sha512_context *ctx );
    void (*_rom_mbedtls_sha512_clone)( mbedtls_sha512_context *dst, const mbedtls_sha512_context *src );
    int (*_rom_mbedtls_sha512_starts)( mbedtls_sha512_context *ctx, int is384 );
    int (*_rom_mbedtls_sha512_update)( mbedtls_sha512_context *ctx, const unsigned char *input, size_t ilen );
    int (*_rom_mbedtls_sha512_finish)( mbedtls_sha512_context *ctx, unsigned char output[64] );
    int (*_rom_mbedtls_internal_sha512_process)( mbedtls_sha512_context *ctx, const unsigned char data[128] );
    int (*_rom_mbedtls_sha512)( const unsigned char *input, size_t ilen, unsigned char output[64], int is384 );
    void (*_rom_mbedtls_ssl_conf_endpoint)( mbedtls_ssl_config *conf, int endpoint );
    void (*_rom_mbedtls_ssl_conf_transport)( mbedtls_ssl_config *conf, int transport );
    void (*_rom_mbedtls_ssl_set_bio)( mbedtls_ssl_context *ssl, void *p_bio, mbedtls_ssl_send_t *f_send, mbedtls_ssl_recv_t *f_recv, mbedtls_ssl_recv_timeout_t *f_recv_timeout );
    int (*_rom_mbedtls_ssl_conf_dh_param_bin)( mbedtls_ssl_config *conf, const unsigned char *dhm_P, size_t P_len, const unsigned char *dhm_G, size_t G_len );
    size_t (*_rom_mbedtls_ssl_get_max_frag_len)( const mbedtls_ssl_context *ssl );
    int (*_rom_mbedtls_ssl_get_max_out_record_payload)( const mbedtls_ssl_context *ssl );
    int (*_rom_mbedtls_ssl_handshake)( mbedtls_ssl_context *ssl );
    int (*_rom_mbedtls_ssl_handshake_step)( mbedtls_ssl_context *ssl );
    int (*_rom_mbedtls_ssl_renegotiate)( mbedtls_ssl_context *ssl );
    int (*_rom_mbedtls_ssl_send_alert_message)( mbedtls_ssl_context *ssl, unsigned char level, unsigned char message );
    int (*_rom_mbedtls_ssl_config_defaults)( mbedtls_ssl_config *conf, int endpoint, int transport, int preset );
    void (*_rom_mbedtls_ssl_session_init)( mbedtls_ssl_session *session );
    void (*_rom_mbedtls_ssl_session_free)( mbedtls_ssl_session *session );
    void (*_rom_mbedtls_ssl_transform_free)( mbedtls_ssl_transform *transform );
    void (*_rom_mbedtls_ssl_handshake_free)( mbedtls_ssl_context *ssl );
    int (*_rom_mbedtls_ssl_handshake_client_step)( mbedtls_ssl_context *ssl );
    void (*_rom_mbedtls_ssl_handshake_wrapup)( mbedtls_ssl_context *ssl );
    int (*_rom_mbedtls_ssl_derive_keys)( mbedtls_ssl_context *ssl );
    int (*_rom_mbedtls_ssl_handle_message_type)( mbedtls_ssl_context *ssl );
    int (*_rom_mbedtls_ssl_prepare_handshake_record)( mbedtls_ssl_context *ssl );
    void (*_rom_mbedtls_ssl_update_handshake_status)( mbedtls_ssl_context *ssl );
    int (*_rom_mbedtls_ssl_read_record)( mbedtls_ssl_context *ssl, unsigned update_hs_digest );
    int (*_rom_mbedtls_ssl_fetch_input)( mbedtls_ssl_context *ssl, size_t nb_want );
    int (*_rom_mbedtls_ssl_write_handshake_msg)( mbedtls_ssl_context *ssl );
    int (*_rom_mbedtls_ssl_write_record)( mbedtls_ssl_context *ssl, uint8_t force_flush );
    int (*_rom_mbedtls_ssl_flush_output)( mbedtls_ssl_context *ssl );
    int (*_rom_mbedtls_ssl_parse_certificate)( mbedtls_ssl_context *ssl );
    int (*_rom_mbedtls_ssl_write_certificate)( mbedtls_ssl_context *ssl );
    int (*_rom_mbedtls_ssl_parse_change_cipher_spec)( mbedtls_ssl_context *ssl );
    int (*_rom_mbedtls_ssl_write_change_cipher_spec)( mbedtls_ssl_context *ssl );
    int (*_rom_mbedtls_ssl_parse_finished)( mbedtls_ssl_context *ssl );
    int (*_rom_mbedtls_ssl_write_finished)( mbedtls_ssl_context *ssl );
    void (*_rom_mbedtls_ssl_optimize_checksum)( mbedtls_ssl_context *ssl, const mbedtls_ssl_ciphersuite_t *ciphersuite_info );
    int (*_rom_mbedtls_ssl_psk_derive_premaster)( mbedtls_ssl_context *ssl, mbedtls_key_exchange_type_t key_ex );
    unsigned char (*_rom_mbedtls_ssl_sig_from_pk)( mbedtls_pk_context *pk );
    mbedtls_pk_type_t (*_rom_mbedtls_ssl_pk_alg_from_sig)( unsigned char sig );
    mbedtls_md_type_t (*_rom_mbedtls_ssl_md_alg_from_hash)( unsigned char hash );
    unsigned char (*_rom_mbedtls_ssl_hash_from_md_alg)( int md );
    int (*_rom_mbedtls_ssl_check_curve)( const mbedtls_ssl_context *ssl, mbedtls_ecp_group_id grp_id );
    int (*_rom_mbedtls_ssl_check_sig_hash)( const mbedtls_ssl_context *ssl, mbedtls_md_type_t md );
    void (*_rom_mbedtls_ssl_write_version)( int major, int minor, int transport, unsigned char ver[2] );
    void (*_rom_mbedtls_ssl_read_version)( int *major, int *minor, int transport, const unsigned char ver[2] );
    int (*_rom_mbedtls_ssl_get_key_exchange_md_ssl_tls)( mbedtls_ssl_context *ssl, unsigned char *output, unsigned char *data, size_t data_len );
    int (*_rom_mbedtls_ssl_get_key_exchange_md_tls1_2)( mbedtls_ssl_context *ssl, unsigned char *hash, size_t *hashlen, unsigned char *data, size_t data_len, mbedtls_md_type_t md_alg );
    int (*_rom_mbedtls_ssl_cf_hmac)( mbedtls_md_context_t *ctx, const unsigned char *add_data, size_t add_data_len, const unsigned char *data, size_t data_len_secret, size_t min_data_len, size_t max_data_len, unsigned char *output );
    void (*_rom_mbedtls_ssl_cf_memcpy_offset)( unsigned char *dst, const unsigned char *src_base, size_t offset_secret, size_t offset_min, size_t offset_max, size_t len );
    int (*_rom_mbedtls_x509_crt_parse_der)( mbedtls_x509_crt *chain, const unsigned char *buf, size_t buflen );
    int (*_rom_mbedtls_x509_crt_verify_restartable)( mbedtls_x509_crt *crt, mbedtls_x509_crt *trust_ca, mbedtls_x509_crl *ca_crl, const mbedtls_x509_crt_profile *profile, const char *cn, uint32_t *flags, int (*f_vrfy)(void *, mbedtls_x509_crt *, int, uint32_t *), void *p_vrfy, mbedtls_x509_crt_restart_ctx *rs_ctx );
    int (*_rom_mbedtls_x509_crt_check_key_usage)( const mbedtls_x509_crt *crt, unsigned int usage );
    int (*_rom_mbedtls_x509_crt_check_extended_key_usage)( const mbedtls_x509_crt *crt, const char *usage_oid, size_t usage_len );
    int (*_rom_mbedtls_x509_crt_is_revoked)( const mbedtls_x509_crt *crt, const mbedtls_x509_crl *crl );
    void (*_rom_mbedtls_x509_crt_init)( mbedtls_x509_crt *crt );
    void (*_rom_mbedtls_x509_crt_free)( mbedtls_x509_crt *crt );
    void (*_rom_mbedtls_x509_crt_restart_init)( mbedtls_x509_crt_restart_ctx *ctx );
    void (*_rom_mbedtls_x509_crt_restart_free)( mbedtls_x509_crt_restart_ctx *ctx );
    int (*_rom_mbedtls_x509_get_name)( unsigned char **p, const unsigned char *end, mbedtls_x509_name *cur );
    int (*_rom_mbedtls_x509_get_alg_null)( unsigned char **p, const unsigned char *end, mbedtls_x509_buf *alg );
    int (*_rom_mbedtls_x509_get_alg)( unsigned char **p, const unsigned char *end, mbedtls_x509_buf *alg, mbedtls_x509_buf *params );
    int (*_rom_mbedtls_x509_get_rsassa_pss_params)( const mbedtls_x509_buf *params, mbedtls_md_type_t *md_alg, mbedtls_md_type_t *mgf_md, int *salt_len );
    int (*_rom_mbedtls_x509_get_sig)( unsigned char **p, const unsigned char *end, mbedtls_x509_buf *sig );
    int (*_rom_mbedtls_x509_get_sig_alg)( const mbedtls_x509_buf *sig_oid, const mbedtls_x509_buf *sig_params, mbedtls_md_type_t *md_alg, mbedtls_pk_type_t *pk_alg, void **sig_opts );
    int (*_rom_mbedtls_x509_get_time)( unsigned char **p, const unsigned char *end, mbedtls_x509_time *t );
    int (*_rom_mbedtls_x509_get_serial)( unsigned char **p, const unsigned char *end, mbedtls_x509_buf *serial );
    int (*_rom_mbedtls_x509_get_ext)( unsigned char **p, const unsigned char *end, mbedtls_x509_buf *ext, int tag );
    void (*_mbedtls_mutex_init)( mbedtls_threading_mutex_t *mutex );
    void (*_mbedtls_mutex_free)( mbedtls_threading_mutex_t *mutex );
    int (*_mbedtls_mutex_lock)( mbedtls_threading_mutex_t *mutex );
    int (*_mbedtls_mutex_unlock)( mbedtls_threading_mutex_t *mutex );
    bool (*_mbedtls_allow_unsupported_critical_ext)( void );
    const mbedtls_cipher_info_t *(*_mbedtls_cipher_info_from_type)( const mbedtls_cipher_type_t cipher_type );
    const mbedtls_cipher_info_t *(*_mbedtls_cipher_info_from_values)( const mbedtls_cipher_id_t cipher_id, int key_bitlen, const mbedtls_cipher_mode_t mode );
    void (*_mbedtls_cipher_free)( mbedtls_cipher_context_t *ctx );
    int (*_mbedtls_cipher_setup)( mbedtls_cipher_context_t *ctx, const mbedtls_cipher_info_t *cipher_info );
    int (*_mbedtls_cipher_setkey)( mbedtls_cipher_context_t *ctx, const unsigned char *key, int key_bitlen, const mbedtls_operation_t operation );
    int (*_mbedtls_cipher_set_iv)( mbedtls_cipher_context_t *ctx, const unsigned char *iv, size_t iv_len );
    int (*_mbedtls_cipher_update)( mbedtls_cipher_context_t *ctx, const unsigned char *input, size_t ilen, unsigned char *output, size_t *olen );
    int (*_mbedtls_cipher_auth_encrypt)( mbedtls_cipher_context_t *ctx, const unsigned char *iv, size_t iv_len, const unsigned char *ad, size_t ad_len, const unsigned char *input, size_t ilen, unsigned char *output, size_t *olen, unsigned char *tag, size_t tag_len );
    int (*_mbedtls_cipher_auth_decrypt)( mbedtls_cipher_context_t *ctx, const unsigned char *iv, size_t iv_len, const unsigned char *ad, size_t ad_len, const unsigned char *input, size_t ilen, unsigned char *output, size_t *olen, const unsigned char *tag, size_t tag_len );
    int (*_mbedtls_hardware_poll)( void *data, unsigned char *output, size_t len, size_t *olen );
    const mbedtls_md_info_t *(*_mbedtls_md_info_from_type)( mbedtls_md_type_t md_type );
    int (*_mbedtls_pem_read_buffer)( void *ctx, const char *header, const char *footer, const unsigned char *data, const unsigned char *pwd, size_t pwdlen, size_t *use_len );
    void *(*_mbedtls_calloc)( size_t n, size_t size );
    void (*_mbedtls_free)( void *ptr );
    int (*_mbedtls_sha1_update)( mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen );
    int (*_mbedtls_internal_sha1_process)( mbedtls_sha1_context *ctx, const unsigned char data[64] );
    int (*_mbedtls_sha256_update)( mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen );
    int (*_mbedtls_internal_sha256_process)( mbedtls_sha256_context *ctx, const unsigned char data[64] );
    const int *(*_mbedtls_ssl_list_ciphersuites)( void );
    const mbedtls_ssl_ciphersuite_t *(*_mbedtls_ssl_ciphersuite_from_id)( int ciphersuite_id );
    mbedtls_pk_type_t (*_mbedtls_ssl_get_ciphersuite_sig_pk_alg)( const mbedtls_ssl_ciphersuite_t *info );
    int (*_mbedtls_ssl_ciphersuite_uses_ec)( const mbedtls_ssl_ciphersuite_t *info );
    int (*_mbedtls_ssl_ciphersuite_uses_psk)( const mbedtls_ssl_ciphersuite_t *info );
    int (*_mbedtls_ssl_handshake_server_step)( mbedtls_ssl_context *ssl );
    int (*_mbedtls_ssl_check_cert_usage)( const mbedtls_x509_crt *cert, const mbedtls_ssl_ciphersuite_t *ciphersuite, int cert_endpoint, uint32_t *flags );
    int (*_mbedtls_x509_time_is_past)( const mbedtls_x509_time *to );
    int (*_mbedtls_x509_time_is_future)( const mbedtls_x509_time *from );
} mbedtls_rom_funcs_t;

#define STRUCT_OFFSET_CHECK(x, y, z) _Static_assert((offsetof(x,y)==(z)), "The variables type of "#x" before "#y" should be "#z)
#define STRUCT_SIZE_CHECK(x, y) _Static_assert((sizeof(x)==(y)), "The sizeof "#x" should be "#y)

#if (!defined(CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL))
#error "CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL"
#endif

/* platform_util.c */
#if (defined(MBEDTLS_PLATFORM_ZEROIZE_ALT))
#error "MBEDTLS_PLATFORM_ZEROIZE_ALT"
#endif

/* sha1.c */
STRUCT_OFFSET_CHECK(mbedtls_sha1_context, total, 0);
STRUCT_OFFSET_CHECK(mbedtls_sha1_context, state, 8);
STRUCT_OFFSET_CHECK(mbedtls_sha1_context, buffer, 28);
#if (!defined(MBEDTLS_SHA1_C)) || \
    (!defined(MBEDTLS_SHA1_ALT)) || \
    (defined(MBEDTLS_SHA1_PROCESS_ALT))
#error "MBEDTLS_SHA1_C"
#endif

/* sha256.c */
STRUCT_OFFSET_CHECK(mbedtls_sha256_context, total, 0);
STRUCT_OFFSET_CHECK(mbedtls_sha256_context, state, 8);
STRUCT_OFFSET_CHECK(mbedtls_sha256_context, buffer, 40);
STRUCT_OFFSET_CHECK(mbedtls_sha256_context, first_block, 104);
STRUCT_OFFSET_CHECK(mbedtls_sha256_context, mode, 108);
STRUCT_OFFSET_CHECK(mbedtls_sha256_context, sha_state, 112);
STRUCT_SIZE_CHECK(mbedtls_sha256_context, 116);
#if (!defined(MBEDTLS_SHA256_C)) || \
    (!defined(MBEDTLS_SHA256_ALT)) || \
    (defined(MBEDTLS_SHA256_PROCESS_ALT)) || \
    (defined(MBEDTLS_SHA256_SMALLER))
#error "!MBEDTLS_SHA256_C"
#endif

/* sha512.c */
STRUCT_OFFSET_CHECK(mbedtls_sha512_context, MBEDTLS_PRIVATE(total), 0);
STRUCT_OFFSET_CHECK(mbedtls_sha512_context, MBEDTLS_PRIVATE(state), 16);
STRUCT_OFFSET_CHECK(mbedtls_sha512_context, MBEDTLS_PRIVATE(buffer), 80);
STRUCT_OFFSET_CHECK(mbedtls_sha512_context, MBEDTLS_PRIVATE(is384), 208);
STRUCT_SIZE_CHECK(mbedtls_sha512_context, 216);
#if (!defined(MBEDTLS_SHA512_C)) || \
    (defined(MBEDTLS_SHA512_ALT)) || \
    (defined(MBEDTLS_SHA512_PROCESS_ALT))
#error "MBEDTLS_SHA256_C"
#endif

/* aes.c */
STRUCT_OFFSET_CHECK(mbedtls_aes_context, MBEDTLS_PRIVATE(nr), 0);
STRUCT_OFFSET_CHECK(mbedtls_aes_context, MBEDTLS_PRIVATE(rk_offset), 4);
STRUCT_OFFSET_CHECK(mbedtls_aes_context, MBEDTLS_PRIVATE(buf), 8);
STRUCT_SIZE_CHECK(mbedtls_aes_context, 280);
STRUCT_OFFSET_CHECK(mbedtls_aes_xts_context, MBEDTLS_PRIVATE(crypt), 0);
STRUCT_OFFSET_CHECK(mbedtls_aes_xts_context, MBEDTLS_PRIVATE(tweak), 280);
STRUCT_SIZE_CHECK(mbedtls_aes_xts_context, 560);
#if (defined(MBEDTLS_HAVE_X86)) || \
    (defined(MBEDTLS_HAVE_X86_64))
#error "MBEDTLS_HAVE_X86"
#endif
#if (!defined(MBEDTLS_AES_C)) || \
    (defined(MBEDTLS_AES_ALT)) || \
    (defined(MBEDTLS_AES_ENCRYPT_ALT)) || \
    (defined(MBEDTLS_AES_DECRYPT_ALT)) || \
    (defined(MBEDTLS_AES_SETKEY_ENC_ALT)) || \
    (defined(MBEDTLS_AES_SETKEY_DEC_ALT))
#error "MBEDTLS_AES_C"
#endif
#if (!defined(MBEDTLS_AES_ROM_TABLES)) || \
    (defined(MBEDTLS_AES_FEWER_TABLES))
#error "MBEDTLS_AES_ROM_TABLES"
#endif
#if (!defined(MBEDTLS_CIPHER_MODE_XTS)) || \
    (!defined(MBEDTLS_CIPHER_MODE_CBC)) || \
    (!defined(MBEDTLS_CIPHER_MODE_CFB)) || \
    (!defined(MBEDTLS_CIPHER_MODE_OFB)) || \
    (!defined(MBEDTLS_CIPHER_MODE_CTR))
#error "MBEDTLS_CIPHER_MODE"
#endif

/* asn1parse.c asn1write.c */
STRUCT_OFFSET_CHECK(mbedtls_asn1_buf, tag, 0);
STRUCT_OFFSET_CHECK(mbedtls_asn1_buf, len, 4);
STRUCT_OFFSET_CHECK(mbedtls_asn1_buf, p, 8);
STRUCT_SIZE_CHECK(mbedtls_asn1_buf, 12);
STRUCT_OFFSET_CHECK(mbedtls_asn1_bitstring, len, 0);
STRUCT_OFFSET_CHECK(mbedtls_asn1_bitstring, unused_bits, 4);
STRUCT_OFFSET_CHECK(mbedtls_asn1_bitstring, p, 8);
STRUCT_SIZE_CHECK(mbedtls_asn1_bitstring, 12);
STRUCT_OFFSET_CHECK(mbedtls_asn1_sequence, buf, 0);
STRUCT_OFFSET_CHECK(mbedtls_asn1_sequence, next, 12);
STRUCT_SIZE_CHECK(mbedtls_asn1_sequence, 16);
STRUCT_OFFSET_CHECK(mbedtls_asn1_named_data, oid, 0);
STRUCT_OFFSET_CHECK(mbedtls_asn1_named_data, val, 12);
STRUCT_OFFSET_CHECK(mbedtls_asn1_named_data, next, 24);
STRUCT_OFFSET_CHECK(mbedtls_asn1_named_data, MBEDTLS_PRIVATE(next_merged), 28);
STRUCT_SIZE_CHECK(mbedtls_asn1_named_data, 32);
#if (!defined(MBEDTLS_ASN1_PARSE_C))
#error "MBEDTLS_ASN1_PARSE_C"
#endif
#if (!defined(MBEDTLS_ASN1_WRITE_C))
#error "MBEDTLS_ASN1_PARSE_C"
#endif

/* base64.c */
#if (!defined(MBEDTLS_BASE64_C))
#error "MBEDTLS_BASE64_C"
#endif

/* md5.c */
#if (defined(MBEDTLS_MD2_C)) || \
    (defined(MBEDTLS_MD4_C)) || \
    (!defined(MBEDTLS_MD5_C)) /* || \
    (defined(MBEDTLS_MD5_ALT)) */
#error "MBEDTLS_MD_C"
#endif
#ifdef CONFIG_MBEDTLS_ROM_MD5
STRUCT_OFFSET_CHECK(mbedtls_md5_context, total, 0);
STRUCT_OFFSET_CHECK(mbedtls_md5_context, state, 8);
STRUCT_OFFSET_CHECK(mbedtls_md5_context, buffer, 24);
STRUCT_SIZE_CHECK(mbedtls_md5_context, 88);
#else
STRUCT_OFFSET_CHECK(mbedtls_md5_context, MBEDTLS_PRIVATE(total), 0);
STRUCT_OFFSET_CHECK(mbedtls_md5_context, MBEDTLS_PRIVATE(state), 8);
STRUCT_OFFSET_CHECK(mbedtls_md5_context, MBEDTLS_PRIVATE(buffer), 24);
STRUCT_SIZE_CHECK(mbedtls_md5_context, 88);
#endif

#ifdef __cplusplus
}
#endif
