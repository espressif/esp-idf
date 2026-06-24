/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include "mbedtls/private/aes.h"
#include "mbedtls/asn1.h"
#include "mbedtls/asn1write.h"
#include "mbedtls/base64.h"
#include "mbedtls/bignum.h"
#include "mbedtls/private/ccm.h"
#include "mbedtls/private/cipher.h"
#include "mbedtls/private/cmac.h"
#include "mbedtls/private/ctr_drbg.h"
#include "mbedtls/private/ecp.h"
#include "mbedtls/md.h"
#include "mbedtls/private/md5.h"
#include "mbedtls/platform.h"
#include "mbedtls/private/sha1.h"
#include "mbedtls/private/sha256.h"
#include "mbedtls/private/sha512.h"
#include "mbedtls/threading.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (!defined(CONFIG_MBEDTLS_THREADING_C))
#error CONFIG_MBEDTLS_THREADING_C
#endif

typedef void (*_rom_mbedtls_threading_set_alt_t)(void (*mutex_init)(mbedtls_threading_mutex_t *),
                                          void (*mutex_free)(mbedtls_threading_mutex_t *),
                                          int (*mutex_lock)(mbedtls_threading_mutex_t *),
                                          int (*mutex_unlock)(mbedtls_threading_mutex_t *));

typedef struct mbedtls_rom_eco4_funcs {
    // aes module
    void (*_rom_mbedtls_aes_init)(mbedtls_aes_context *ctx);
    void (*_rom_mbedtls_aes_free)(mbedtls_aes_context *ctx);
    void (*_rom_mbedtls_aes_xts_init)(mbedtls_aes_xts_context *ctx);
    void (*_rom_mbedtls_aes_xts_free)(mbedtls_aes_xts_context *ctx);
    int (*_rom_mbedtls_aes_setkey_enc)(mbedtls_aes_context *ctx, const unsigned char *key, unsigned int keybits);
    int (*_rom_mbedtls_aes_setkey_dec)(mbedtls_aes_context *ctx, const unsigned char *key, unsigned int keybits);
    int (*_rom_mbedtls_aes_xts_setkey_enc)(mbedtls_aes_xts_context *ctx, const unsigned char *key, unsigned int keybits);
    int (*_rom_mbedtls_aes_xts_setkey_dec)(mbedtls_aes_xts_context *ctx, const unsigned char *key, unsigned int keybits);
    int (*_rom_mbedtls_aes_crypt_ecb)(mbedtls_aes_context *ctx, int mode, const unsigned char input[16], unsigned char output[16]);
    int (*_rom_mbedtls_aes_crypt_cbc)(mbedtls_aes_context *ctx, int mode, size_t length, unsigned char iv[16], const unsigned char *input, unsigned char *output);
    int (*_rom_mbedtls_aes_crypt_xts)(mbedtls_aes_xts_context *ctx, int mode, size_t length, const unsigned char data_unit[16], const unsigned char *input, unsigned char *output);
    int (*_rom_mbedtls_aes_crypt_cfb128)(mbedtls_aes_context *ctx, int mode, size_t length, size_t *iv_off, unsigned char iv[16], const unsigned char *input, unsigned char *output);
    int (*_rom_mbedtls_aes_crypt_ofb)(mbedtls_aes_context *ctx, size_t length, size_t *iv_off, unsigned char iv[16], const unsigned char *input, unsigned char *output);
    int (*_rom_mbedtls_aes_crypt_ctr)(mbedtls_aes_context *ctx, size_t length, size_t *nc_off, unsigned char nonce_counter[16], unsigned char stream_block[16], const unsigned char *input, unsigned char *output);
    int (*_rom_mbedtls_internal_aes_encrypt)(mbedtls_aes_context *ctx, const unsigned char input[16], unsigned char output[16]);
    int (*_rom_mbedtls_internal_aes_decrypt)(mbedtls_aes_context *ctx, const unsigned char input[16], unsigned char output[16]);
    // md5 module
    void (*_rom_mbedtls_md5_init)(mbedtls_md5_context *ctx);
    void (*_rom_mbedtls_md5_free)(mbedtls_md5_context *ctx);
    void (*_rom_mbedtls_md5_clone)(mbedtls_md5_context *dst, const mbedtls_md5_context *src);
    int (*_rom_mbedtls_md5_starts)(mbedtls_md5_context *ctx);
    int (*_rom_mbedtls_md5_update)(mbedtls_md5_context *ctx, const unsigned char *input, size_t ilen);
    int (*_rom_mbedtls_md5_finish)(mbedtls_md5_context *ctx, unsigned char output[16]);
    int (*_rom_mbedtls_md5)(const unsigned char *input, size_t ilen, unsigned char output[16]);
    // bignum module
    void (*_rom_mbedtls_mpi_init)(mbedtls_mpi *X);
    void (*_rom_mbedtls_mpi_free)(mbedtls_mpi *X);
    int (*_rom_mbedtls_mpi_grow)(mbedtls_mpi *X, size_t nblimbs);
    int (*_rom_mbedtls_mpi_shrink)(mbedtls_mpi *X, size_t nblimbs);
    int (*_rom_mbedtls_mpi_copy)(mbedtls_mpi *X, const mbedtls_mpi *Y);
    int (*_rom_mbedtls_mpi_safe_cond_assign)(mbedtls_mpi *X, const mbedtls_mpi *Y, unsigned char assign);
    int (*_rom_mbedtls_mpi_safe_cond_swap)(mbedtls_mpi *X, mbedtls_mpi *Y, unsigned char swap);
    int (*_rom_mbedtls_mpi_lset)(mbedtls_mpi *X, mbedtls_mpi_sint z);
    int (*_rom_mbedtls_mpi_get_bit)(const mbedtls_mpi *X, size_t pos);
    int (*_rom_mbedtls_mpi_set_bit)(mbedtls_mpi *X, size_t pos, unsigned char val);
    size_t (*_rom_mbedtls_mpi_lsb)(const mbedtls_mpi *X);
    size_t (*_rom_mbedtls_mpi_bitlen)(const mbedtls_mpi *X);
    size_t (*_rom_mbedtls_mpi_size)(const mbedtls_mpi *X);
    int (*_rom_mbedtls_mpi_read_binary)(mbedtls_mpi *X, const unsigned char *buf, size_t buflen);
    int (*_rom_mbedtls_mpi_write_binary)(const mbedtls_mpi *X, unsigned char *buf, size_t buflen);
    int (*_rom_mbedtls_mpi_shift_l)(mbedtls_mpi *X, size_t count);
    int (*_rom_mbedtls_mpi_shift_r)(mbedtls_mpi *X, size_t count);
    int (*_rom_mbedtls_mpi_cmp_abs)(const mbedtls_mpi *X, const mbedtls_mpi *Y);
    int (*_rom_mbedtls_mpi_cmp_mpi)(const mbedtls_mpi *X, const mbedtls_mpi *Y);
    int (*_rom_mbedtls_mpi_lt_mpi_ct)(const mbedtls_mpi *X, const mbedtls_mpi *Y, unsigned *ret);
    int (*_rom_mbedtls_mpi_cmp_int)(const mbedtls_mpi *X, mbedtls_mpi_sint z);
    int (*_rom_mbedtls_mpi_add_abs)(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *B);
    int (*_rom_mbedtls_mpi_sub_abs)(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *B);
    int (*_rom_mbedtls_mpi_add_mpi)(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *B);
    int (*_rom_mbedtls_mpi_sub_mpi)(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *B);
    int (*_rom_mbedtls_mpi_add_int)(mbedtls_mpi *X, const mbedtls_mpi *A, mbedtls_mpi_sint b);
    int (*_rom_mbedtls_mpi_sub_int)(mbedtls_mpi *X, const mbedtls_mpi *A, mbedtls_mpi_sint b);
    int (*_rom_mbedtls_mpi_mul_mpi)(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *B);
    int (*_rom_mbedtls_mpi_mul_int)(mbedtls_mpi *X, const mbedtls_mpi *A, mbedtls_mpi_uint b);
    int (*_rom_mbedtls_mpi_div_mpi)(mbedtls_mpi *Q, mbedtls_mpi *R, const mbedtls_mpi *A, const mbedtls_mpi *B);
    int (*_rom_mbedtls_mpi_div_int)(mbedtls_mpi *Q, mbedtls_mpi *R, const mbedtls_mpi *A, mbedtls_mpi_sint b);
    int (*_rom_mbedtls_mpi_mod_mpi)(mbedtls_mpi *R, const mbedtls_mpi *A, const mbedtls_mpi *B);
    int (*_rom_mbedtls_mpi_mod_int)(mbedtls_mpi_uint *r, const mbedtls_mpi *A, mbedtls_mpi_sint b);
    int (*_rom_mbedtls_mpi_exp_mod)(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *E, const mbedtls_mpi *N, mbedtls_mpi *prec_RR);
    int (*_rom_mbedtls_mpi_fill_random)(mbedtls_mpi *X, size_t size, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng);
    int (*_rom_mbedtls_mpi_gcd)(mbedtls_mpi *G, const mbedtls_mpi *A, const mbedtls_mpi *B);
    int (*_rom_mbedtls_mpi_inv_mod)(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *N);
    int (*_rom_mbedtls_mpi_is_prime_ext)(const mbedtls_mpi *X, int rounds, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng);
    // sha1 sha256 sha512 module
    void (*_rom_mbedtls_sha1_init)(mbedtls_sha1_context *ctx);
    void (*_rom_mbedtls_sha1_free)(mbedtls_sha1_context *ctx);
    void (*_rom_mbedtls_sha1_clone)(mbedtls_sha1_context *dst, const mbedtls_sha1_context *src);
    int (*_rom_mbedtls_sha1_starts)(mbedtls_sha1_context *ctx);
    int (*_rom_mbedtls_sha1_finish)(mbedtls_sha1_context *ctx, unsigned char output[20]);
    int (*_rom_mbedtls_sha1)(const unsigned char *input, size_t ilen, unsigned char output[20]);
    void (*_rom_mbedtls_sha256_init)(mbedtls_sha256_context *ctx);
    void (*_rom_mbedtls_sha256_free)(mbedtls_sha256_context *ctx);
    void (*_rom_mbedtls_sha256_clone)(mbedtls_sha256_context *dst, const mbedtls_sha256_context *src);
    int (*_rom_mbedtls_sha256_starts)(mbedtls_sha256_context *ctx, int is224);
    int (*_rom_mbedtls_sha256_finish)(mbedtls_sha256_context *ctx, unsigned char *output);
    int (*_rom_mbedtls_sha256)(const unsigned char *input, size_t ilen, unsigned char *output, int is224);
    void (*_rom_mbedtls_sha512_init)(mbedtls_sha512_context *ctx);
    void (*_rom_mbedtls_sha512_free)(mbedtls_sha512_context *ctx);
    void (*_rom_mbedtls_sha512_clone)(mbedtls_sha512_context *dst, const mbedtls_sha512_context *src);
    int (*_rom_mbedtls_sha512_starts)(mbedtls_sha512_context *ctx, int is384);
    int (*_rom_mbedtls_sha512_update)(mbedtls_sha512_context *ctx, const unsigned char *input, size_t ilen);
    int (*_rom_mbedtls_sha512_finish)(mbedtls_sha512_context *ctx, unsigned char *output);
    int (*_rom_mbedtls_sha512)(const unsigned char *input, size_t ilen, unsigned char *output, int is384);
    // ecp module
    // asn1 module
    int (*_rom_mbedtls_asn1_get_len)(unsigned char **p, const unsigned char *end, size_t *len);
    int (*_rom_mbedtls_asn1_get_tag)(unsigned char **p, const unsigned char *end, size_t *len, int tag);
    int (*_rom_mbedtls_asn1_get_bool)(unsigned char **p, const unsigned char *end, int *val);
    int (*_rom_mbedtls_asn1_get_int)(unsigned char **p, const unsigned char *end, int *val);
    int (*_rom_mbedtls_asn1_get_bitstring)(unsigned char **p, const unsigned char *end, mbedtls_asn1_bitstring *bs);
    int (*_rom_mbedtls_asn1_get_bitstring_null)(unsigned char **p, const unsigned char *end, size_t *len);
    int (*_rom_mbedtls_asn1_get_sequence_of)(unsigned char **p, const unsigned char *end, mbedtls_asn1_sequence *cur, int tag);
    int (*_rom_mbedtls_asn1_get_mpi)(unsigned char **p, const unsigned char *end, mbedtls_mpi *X);
    int (*_rom_mbedtls_asn1_get_alg)(unsigned char **p, const unsigned char *end, mbedtls_asn1_buf *alg, mbedtls_asn1_buf *params);
    int (*_rom_mbedtls_asn1_get_alg_null)(unsigned char **p, const unsigned char *end, mbedtls_asn1_buf *alg);
    // asn1write module
    int (*_rom_mbedtls_asn1_write_len)(unsigned char **p, const unsigned char *start, size_t len);
    int (*_rom_mbedtls_asn1_write_tag)(unsigned char **p, const unsigned char *start, unsigned char tag);
    int (*_rom_mbedtls_asn1_write_mpi)(unsigned char **p, const unsigned char *start, const mbedtls_mpi *X);
    // ccm module
    void (*_rom_mbedtls_ccm_init)(mbedtls_ccm_context *ctx);
    int (*_rom_mbedtls_ccm_setkey)(mbedtls_ccm_context *ctx, mbedtls_cipher_id_t cipher, const unsigned char *key, unsigned int keybits);
    void (*_rom_mbedtls_ccm_free)(mbedtls_ccm_context *ctx);
    int (*_rom_mbedtls_ccm_encrypt_and_tag)(mbedtls_ccm_context *ctx, size_t length, const unsigned char *iv, size_t iv_len, const unsigned char *ad, size_t ad_len, const unsigned char *input, unsigned char *output, unsigned char *tag, size_t tag_len);
    int (*_rom_mbedtls_ccm_star_encrypt_and_tag)(mbedtls_ccm_context *ctx, size_t length, const unsigned char *iv, size_t iv_len, const unsigned char *ad, size_t ad_len, const unsigned char *input, unsigned char *output, unsigned char *tag, size_t tag_len);
    int (*_rom_mbedtls_ccm_auth_decrypt)(mbedtls_ccm_context *ctx, size_t length, const unsigned char *iv, size_t iv_len, const unsigned char *ad, size_t ad_len, const unsigned char *input, unsigned char *output, const unsigned char *tag, size_t tag_len);
    int (*_rom_mbedtls_ccm_star_auth_decrypt)(mbedtls_ccm_context *ctx, size_t length, const unsigned char *iv, size_t iv_len, const unsigned char *ad, size_t ad_len, const unsigned char *input, unsigned char *output, const unsigned char *tag, size_t tag_len);
    // cipher module
    void (*_rom_mbedtls_cipher_init)(mbedtls_cipher_context_t *ctx);
    int (*_rom_mbedtls_cipher_set_padding_mode)(mbedtls_cipher_context_t *ctx, mbedtls_cipher_padding_t mode);
    int (*_rom_mbedtls_cipher_reset)(mbedtls_cipher_context_t *ctx);
    int (*_rom_mbedtls_cipher_finish)(mbedtls_cipher_context_t *ctx, unsigned char *output, size_t *olen);
    int (*_rom_mbedtls_cipher_crypt)(mbedtls_cipher_context_t *ctx, const unsigned char *iv, size_t iv_len, const unsigned char *input, size_t ilen, unsigned char *output, size_t *olen);
    int (*_rom_mbedtls_cipher_cmac_starts)(mbedtls_cipher_context_t *ctx, const unsigned char *key, size_t keybits);
    int (*_rom_mbedtls_cipher_cmac_update)(mbedtls_cipher_context_t *ctx, const unsigned char *input, size_t ilen);
    int (*_rom_mbedtls_cipher_cmac_finish)(mbedtls_cipher_context_t *ctx, unsigned char *output);
    // ctr drbg module
    void (*_rom_mbedtls_ctr_drbg_init)(mbedtls_ctr_drbg_context *ctx);
    int (*_rom_mbedtls_ctr_drbg_seed)(mbedtls_ctr_drbg_context *ctx, int (*f_entropy)(void *, unsigned char *, size_t), void *p_entropy, const unsigned char *custom, size_t len);
    void (*_rom_mbedtls_ctr_drbg_free)(mbedtls_ctr_drbg_context *ctx);
    int (*_rom_mbedtls_ctr_drbg_reseed)(mbedtls_ctr_drbg_context *ctx, const unsigned char *additional, size_t len);
    int (*_rom_mbedtls_ctr_drbg_random_with_add)(void *p_rng, unsigned char *output, size_t output_len, const unsigned char *additional, size_t add_len);
    int (*_rom_mbedtls_ctr_drbg_random)(void *p_rng, unsigned char *output, size_t output_len);
    // base64 module
    int (*_rom_mbedtls_base64_decode)(unsigned char *dst, size_t dlen, size_t *olen, const unsigned char *src, size_t slen);
    // aes module
    int (*_rom_mbedtls_aes_crypt_cfb8)(mbedtls_aes_context *ctx, int mode, size_t length, unsigned char iv[16], const unsigned char *input, unsigned char *output);
    // md5 module
    // bignum module
    void (*_rom_mbedtls_mpi_swap)(mbedtls_mpi *X, mbedtls_mpi *Y);
    int (*_rom_mbedtls_mpi_read_string)(mbedtls_mpi *X, int radix, const char *s);
    int (*_rom_mbedtls_mpi_write_string)(const mbedtls_mpi *X, int radix, char *buf, size_t buflen, size_t *olen);
    int (*_rom_mbedtls_mpi_read_binary_le)(mbedtls_mpi *X, const unsigned char *buf, size_t buflen);
    int (*_rom_mbedtls_mpi_write_binary_le)(const mbedtls_mpi *X, unsigned char *buf, size_t buflen);
    int (*_rom_mbedtls_mpi_random)(mbedtls_mpi *X, mbedtls_mpi_sint min, const mbedtls_mpi *N, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng);
    int (*_rom_mbedtls_mpi_gen_prime)(mbedtls_mpi *X, size_t nbits, int flags, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng);
    // ecp module
    int (*_rom_mbedtls_ecp_check_budget)(const mbedtls_ecp_group *grp, mbedtls_ecp_restart_ctx *rs_ctx, unsigned ops);
    void (*_rom_mbedtls_ecp_set_max_ops)(unsigned max_ops);
    int (*_rom_mbedtls_ecp_restart_is_enabled)(void);
    mbedtls_ecp_curve_type (*_rom_mbedtls_ecp_get_type)(const mbedtls_ecp_group *grp);
    const mbedtls_ecp_curve_info *(*_rom_mbedtls_ecp_curve_list)(void);
    const mbedtls_ecp_group_id *(*_rom_mbedtls_ecp_grp_id_list)(void);
    const mbedtls_ecp_curve_info *(*_rom_mbedtls_ecp_curve_info_from_grp_id)(mbedtls_ecp_group_id grp_id);
    const mbedtls_ecp_curve_info *(*_rom_mbedtls_ecp_curve_info_from_tls_id)(uint16_t tls_id);
    const mbedtls_ecp_curve_info *(*_rom_mbedtls_ecp_curve_info_from_name)(const char *name);
    void (*_rom_mbedtls_ecp_point_init)(mbedtls_ecp_point *pt);
    void (*_rom_mbedtls_ecp_group_init)(mbedtls_ecp_group *grp);
    void (*_rom_mbedtls_ecp_keypair_init)(mbedtls_ecp_keypair *key);
    void (*_rom_mbedtls_ecp_point_free)(mbedtls_ecp_point *pt);
    void (*_rom_mbedtls_ecp_group_free)(mbedtls_ecp_group *grp);
    void (*_rom_mbedtls_ecp_keypair_free)(mbedtls_ecp_keypair *key);
    void (*_rom_mbedtls_ecp_restart_init)(mbedtls_ecp_restart_ctx *ctx);
    void (*_rom_mbedtls_ecp_restart_free)(mbedtls_ecp_restart_ctx *ctx);
    int (*_rom_mbedtls_ecp_copy)(mbedtls_ecp_point *P, const mbedtls_ecp_point *Q);
    int (*_rom_mbedtls_ecp_group_copy)(mbedtls_ecp_group *dst, const mbedtls_ecp_group *src);
    int (*_rom_mbedtls_ecp_set_zero)(mbedtls_ecp_point *pt);
    int (*_rom_mbedtls_ecp_is_zero)(mbedtls_ecp_point *pt);
    int (*_rom_mbedtls_ecp_point_cmp)(const mbedtls_ecp_point *P, const mbedtls_ecp_point *Q);
    int (*_rom_mbedtls_ecp_point_read_string)(mbedtls_ecp_point *P, int radix, const char *x, const char *y);
    int (*_rom_mbedtls_ecp_point_write_binary)(const mbedtls_ecp_group *grp, const mbedtls_ecp_point *P, int format, size_t *olen, unsigned char *buf, size_t buflen);
    int (*_rom_mbedtls_ecp_point_read_binary)(const mbedtls_ecp_group *grp, mbedtls_ecp_point *P, const unsigned char *buf, size_t ilen);
    int (*_rom_mbedtls_ecp_tls_read_point)(const mbedtls_ecp_group *grp, mbedtls_ecp_point *pt, const unsigned char **buf, size_t len);
    int (*_rom_mbedtls_ecp_tls_write_point)(const mbedtls_ecp_group *grp, const mbedtls_ecp_point *pt, int format, size_t *olen, unsigned char *buf, size_t blen);
    int (*_rom_mbedtls_ecp_group_load)(mbedtls_ecp_group *grp, mbedtls_ecp_group_id id);
    int (*_rom_mbedtls_ecp_tls_read_group)(mbedtls_ecp_group *grp, const unsigned char **buf, size_t len);
    int (*_rom_mbedtls_ecp_tls_read_group_id)(mbedtls_ecp_group_id *grp, const unsigned char **buf, size_t len);
    int (*_rom_mbedtls_ecp_tls_write_group)(const mbedtls_ecp_group *grp, size_t *olen, unsigned char *buf, size_t blen);
    int (*_rom_mbedtls_ecp_mul)(mbedtls_ecp_group *grp, mbedtls_ecp_point *R, const mbedtls_mpi *m, const mbedtls_ecp_point *P, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng);
    int (*_rom_mbedtls_ecp_mul_restartable)(mbedtls_ecp_group *grp, mbedtls_ecp_point *R, const mbedtls_mpi *m, const mbedtls_ecp_point *P, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, mbedtls_ecp_restart_ctx *rs_ctx);
    int (*_rom_mbedtls_ecp_muladd)(mbedtls_ecp_group *grp, mbedtls_ecp_point *R, const mbedtls_mpi *m, const mbedtls_ecp_point *P, const mbedtls_mpi *n, const mbedtls_ecp_point *Q);
    int (*_rom_mbedtls_ecp_muladd_restartable)( mbedtls_ecp_group *grp, mbedtls_ecp_point *R, const mbedtls_mpi *m, const mbedtls_ecp_point *P, const mbedtls_mpi *n, const mbedtls_ecp_point *Q, mbedtls_ecp_restart_ctx *rs_ctx);
    int (*_rom_mbedtls_ecp_check_pubkey)(const mbedtls_ecp_group *grp, const mbedtls_ecp_point *pt);
    int (*_rom_mbedtls_ecp_check_privkey)(const mbedtls_ecp_group *grp, const mbedtls_mpi *d);
    int (*_rom_mbedtls_ecp_gen_privkey)(const mbedtls_ecp_group *grp, mbedtls_mpi *d, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng);
    int (*_rom_mbedtls_ecp_gen_keypair_base)(mbedtls_ecp_group *grp, const mbedtls_ecp_point *G, mbedtls_mpi *d, mbedtls_ecp_point *Q, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng);
    int (*_rom_mbedtls_ecp_gen_keypair)(mbedtls_ecp_group *grp, mbedtls_mpi *d, mbedtls_ecp_point *Q, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng);
    int (*_rom_mbedtls_ecp_gen_key)(mbedtls_ecp_group_id grp_id, mbedtls_ecp_keypair *key, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng);
    int (*_rom_mbedtls_ecp_read_key)(mbedtls_ecp_group_id grp_id, mbedtls_ecp_keypair *key, const unsigned char *buf, size_t buflen);
    int (*_rom_mbedtls_ecp_write_key_ext)(const mbedtls_ecp_keypair *key, size_t *olen, unsigned char *buf, size_t buflen);
    int (*_rom_mbedtls_ecp_check_pub_priv)( const mbedtls_ecp_keypair *pub, const mbedtls_ecp_keypair *prv, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng);
    int (*_rom_mbedtls_ecp_export)(const mbedtls_ecp_keypair *key, mbedtls_ecp_group *grp, mbedtls_mpi *d, mbedtls_ecp_point *Q);
    // asn1 module
    int (*_rom_mbedtls_asn1_get_enum)(unsigned char **p, const unsigned char *end, int *val);
    void (*_rom_mbedtls_asn1_sequence_free)(mbedtls_asn1_sequence *seq);
    int (*_rom_mbedtls_asn1_traverse_sequence_of)( unsigned char **p, const unsigned char *end, unsigned char tag_must_mask, unsigned char tag_must_val, unsigned char tag_may_mask, unsigned char tag_may_val, int (*cb)(void *ctx, int tag, unsigned char *start, size_t len), void *ctx);
    const mbedtls_asn1_named_data *(*_rom_mbedtls_asn1_find_named_data)(const mbedtls_asn1_named_data *list, const char *oid, size_t len);
    void (*_rom_mbedtls_asn1_free_named_data_list)(mbedtls_asn1_named_data **head);
    void (*_rom_mbedtls_asn1_free_named_data_list_shallow)(mbedtls_asn1_named_data *name);
    // asn1write module
    int (*_rom_mbedtls_asn1_write_raw_buffer)(unsigned char **p, const unsigned char *start, const unsigned char *buf, size_t size);
    int (*_rom_mbedtls_asn1_write_null)(unsigned char **p, const unsigned char *start);
    int (*_rom_mbedtls_asn1_write_oid)(unsigned char **p, const unsigned char *start, const char *oid, size_t oid_len);
    int (*_rom_mbedtls_asn1_write_algorithm_identifier)(unsigned char **p, const unsigned char *start, const char *oid, size_t oid_len, size_t par_len);
    int (*_rom_mbedtls_asn1_write_bool)(unsigned char **p, const unsigned char *start, int boolean);
    int (*_rom_mbedtls_asn1_write_int)(unsigned char **p, const unsigned char *start, int val);
    int (*_rom_mbedtls_asn1_write_enum)(unsigned char **p, const unsigned char *start, int val);
    int (*_rom_mbedtls_asn1_write_tagged_string)(unsigned char **p, const unsigned char *start, int tag, const char *text, size_t text_len);
    int (*_rom_mbedtls_asn1_write_printable_string)(unsigned char **p, const unsigned char *start, const char *text, size_t text_len);
    int (*_rom_mbedtls_asn1_write_utf8_string)(unsigned char **p, const unsigned char *start, const char *text, size_t text_len);
    int (*_rom_mbedtls_asn1_write_ia5_string)(unsigned char **p, const unsigned char *start, const char *text, size_t text_len);
    int (*_rom_mbedtls_asn1_write_bitstring)(unsigned char **p, const unsigned char *start, const unsigned char *buf, size_t bits);
    int (*_rom_mbedtls_asn1_write_named_bitstring)(unsigned char **p, const unsigned char *start, const unsigned char *buf, size_t bits);
    int (*_rom_mbedtls_asn1_write_octet_string)(unsigned char **p, const unsigned char *start, const unsigned char *buf, size_t size);
    mbedtls_asn1_named_data *(*_rom_mbedtls_asn1_store_named_data)(mbedtls_asn1_named_data **list, const char *oid, size_t oid_len, const unsigned char *val, size_t val_len);
    // ccm module
    int (*_rom_mbedtls_ccm_starts)(mbedtls_ccm_context *ctx, int mode, const unsigned char *iv, size_t iv_len);
    int (*_rom_mbedtls_ccm_set_lengths)(mbedtls_ccm_context *ctx, size_t total_ad_len, size_t plaintext_len, size_t tag_len);
    int (*_rom_mbedtls_ccm_update_ad)(mbedtls_ccm_context *ctx, const unsigned char *ad, size_t ad_len);
    int (*_rom_mbedtls_ccm_update)(mbedtls_ccm_context *ctx, const unsigned char *input, size_t input_len, unsigned char *output, size_t output_size, size_t *output_len);
    int (*_rom_mbedtls_ccm_finish)(mbedtls_ccm_context *ctx, unsigned char *tag, size_t tag_len);
    // cipher module
    const int *(*_rom_mbedtls_cipher_list)(void);
    const mbedtls_cipher_info_t *(*_rom_mbedtls_cipher_info_from_string)(const char *cipher_name);
    const mbedtls_cipher_info_t *(*_rom_mbedtls_cipher_info_from_type)(const mbedtls_cipher_type_t cipher_type);
    const mbedtls_cipher_info_t *(*_rom_mbedtls_cipher_info_from_values)(const mbedtls_cipher_id_t cipher_id, int key_bitlen, const mbedtls_cipher_mode_t mode);
    void (*_rom_mbedtls_cipher_free)(mbedtls_cipher_context_t *ctx);
    int (*_rom_mbedtls_cipher_setup)(mbedtls_cipher_context_t *ctx, const mbedtls_cipher_info_t *cipher_info);
    int (*_rom_mbedtls_cipher_setkey)(mbedtls_cipher_context_t *ctx, const unsigned char *key, int key_bitlen, const mbedtls_operation_t operation);
    int (*_rom_mbedtls_cipher_set_iv)(mbedtls_cipher_context_t *ctx, const unsigned char *iv, size_t iv_len);
    int (*_rom_mbedtls_cipher_update_ad)(mbedtls_cipher_context_t *ctx, const unsigned char *ad, size_t ad_len);
    int (*_rom_mbedtls_cipher_update)(mbedtls_cipher_context_t *ctx, const unsigned char *input, size_t ilen, unsigned char *output, size_t *olen);
    int (*_rom_mbedtls_cipher_write_tag)(mbedtls_cipher_context_t *ctx, unsigned char *tag, size_t tag_len);
    int (*_rom_mbedtls_cipher_check_tag)(mbedtls_cipher_context_t *ctx, const unsigned char *tag, size_t tag_len);
    int (*_rom_mbedtls_cipher_auth_encrypt_ext)(mbedtls_cipher_context_t *ctx, const unsigned char *iv, size_t iv_len, const unsigned char *ad, size_t ad_len, const unsigned char *input, size_t ilen, unsigned char *output, size_t output_len, size_t *olen, size_t tag_len);
    int (*_rom_mbedtls_cipher_auth_decrypt_ext)(mbedtls_cipher_context_t *ctx, const unsigned char *iv, size_t iv_len, const unsigned char *ad, size_t ad_len, const unsigned char *input, size_t ilen, unsigned char *output, size_t output_len, size_t *olen, size_t tag_len);
    int (*_rom_mbedtls_cipher_cmac_reset)(mbedtls_cipher_context_t *ctx);
    int (*_rom_mbedtls_cipher_cmac)(const mbedtls_cipher_info_t *cipher_info, const unsigned char *key, size_t keylen, const unsigned char *input, size_t ilen, unsigned char *output);
    int (*_rom_mbedtls_aes_cmac_prf_128)(const unsigned char *key, size_t key_len, const unsigned char *input, size_t in_len, unsigned char output[16]);
    // ctr drbg module
    void (*_rom_mbedtls_ctr_drbg_set_prediction_resistance)(mbedtls_ctr_drbg_context *ctx, int resistance);
    void (*_rom_mbedtls_ctr_drbg_set_entropy_len)(mbedtls_ctr_drbg_context *ctx, size_t len);
    int (*_rom_mbedtls_ctr_drbg_set_nonce_len)(mbedtls_ctr_drbg_context *ctx, size_t len);
    void (*_rom_mbedtls_ctr_drbg_set_reseed_interval)(mbedtls_ctr_drbg_context *ctx, int interval);
    int (*_rom_mbedtls_ctr_drbg_update)(mbedtls_ctr_drbg_context *ctx, const unsigned char *additional, size_t add_len);
    // base64 module
    int (*_rom_mbedtls_base64_encode)(unsigned char *dst, size_t dlen, size_t *olen, const unsigned char *src, size_t slen);
    // sha1 sha256 sha512 module
    int (*_rom_mbedtls_sha1_update)(mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen);
    int (*_rom_mbedtls_sha256_update)(mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen);
    // memory calloc free
    void *(*_rom_mbedtls_mem_calloc)(size_t n, size_t size);
    void (*_rom_mbedtls_mem_free)(void *ptr);
} mbedtls_rom_eco4_funcs_t;

#define STRUCT_OFFSET_CHECK(x, y, z) _Static_assert((offsetof(x,y)==(z)), "The variables type of "#x" before "#y" should be "#z)
#define STRUCT_SIZE_CHECK(x, y) _Static_assert((sizeof(x)==(y)), "The sizeof "#x" should be "#y)

#if (!defined(CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL) && !defined(CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL_BOOTLOADER))
#error "Please enable CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL or CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL_BOOTLOADER"
#endif

/* platform_util.c */
#if (defined(MBEDTLS_PLATFORM_ZEROIZE_ALT))
#error "MBEDTLS_PLATFORM_ZEROIZE_ALT"
#endif

#ifdef __cplusplus
}
#endif
