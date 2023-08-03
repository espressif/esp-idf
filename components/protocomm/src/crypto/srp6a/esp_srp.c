/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
// This file contains SRP6a implementation based on RFC 5054
#include <string.h>

#include "esp_log.h"
#include "esp_err.h"

#include <mbedtls/sha512.h>
#include "esp_srp_mpi.h"
#include "esp_srp.h"

#define SHA512_HASH_SZ      64

static const char *TAG = "srp6a";

typedef struct esp_srp_handle {
    esp_ng_type_t type;
    esp_mpi_ctx_t *ctx;

    /* N
     * the bytes_n simply points to the static array
     */
    esp_mpi_t *n;
    const char    *bytes_n;
    int      len_n;

    /* g
     * the bytes_g simply points to the static array
     */
    esp_mpi_t *g;
    const char    *bytes_g;
    int      len_g;

    /* Salt */
    esp_mpi_t *s;
    char    *bytes_s;
    int      len_s;
    /* Verifier */
    esp_mpi_t *v;
    /* B */
    esp_mpi_t *B;
    char    *bytes_B;
    int      len_B;
    /* b */
    esp_mpi_t *b;
    /* A */
    esp_mpi_t *A;
    char    *bytes_A;
    int      len_A;
    /* K - session key*/
    char *session_key;
} esp_srp_handle;

static void hexdump_mpi(const char *name, esp_mpi_t *bn)
{
    int len = 0;
    char *str = esp_mpi_to_bin(bn, &len);
    if (str) {
        ESP_LOGD(TAG, "%s ->", name);
        ESP_LOG_BUFFER_HEX_LEVEL(TAG, str, len, ESP_LOG_DEBUG);
        free(str);
    }
}

/************************* SRP Stuff *************************/
static const char N_3072[] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2, 0x34,
    0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1, 0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74,
    0x02, 0x0B, 0xBE, 0xA6, 0x3B, 0x13, 0x9B, 0x22, 0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
    0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 0x30, 0x2B, 0x0A, 0x6D, 0xF2, 0x5F, 0x14, 0x37,
    0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45, 0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6,
    0xF4, 0x4C, 0x42, 0xE9, 0xA6, 0x37, 0xED, 0x6B, 0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
    0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5, 0xAE, 0x9F, 0x24, 0x11, 0x7C, 0x4B, 0x1F, 0xE6,
    0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D, 0xC2, 0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05,
    0x98, 0xDA, 0x48, 0x36, 0x1C, 0x55, 0xD3, 0x9A, 0x69, 0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F,
    0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96, 0x1C, 0x62, 0xF3, 0x56, 0x20, 0x85, 0x52, 0xBB,
    0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D, 0x67, 0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04,
    0xF1, 0x74, 0x6C, 0x08, 0xCA, 0x18, 0x21, 0x7C, 0x32, 0x90, 0x5E, 0x46, 0x2E, 0x36, 0xCE, 0x3B,
    0xE3, 0x9E, 0x77, 0x2C, 0x18, 0x0E, 0x86, 0x03, 0x9B, 0x27, 0x83, 0xA2, 0xEC, 0x07, 0xA2, 0x8F,
    0xB5, 0xC5, 0x5D, 0xF0, 0x6F, 0x4C, 0x52, 0xC9, 0xDE, 0x2B, 0xCB, 0xF6, 0x95, 0x58, 0x17, 0x18,
    0x39, 0x95, 0x49, 0x7C, 0xEA, 0x95, 0x6A, 0xE5, 0x15, 0xD2, 0x26, 0x18, 0x98, 0xFA, 0x05, 0x10,
    0x15, 0x72, 0x8E, 0x5A, 0x8A, 0xAA, 0xC4, 0x2D, 0xAD, 0x33, 0x17, 0x0D, 0x04, 0x50, 0x7A, 0x33,
    0xA8, 0x55, 0x21, 0xAB, 0xDF, 0x1C, 0xBA, 0x64, 0xEC, 0xFB, 0x85, 0x04, 0x58, 0xDB, 0xEF, 0x0A,
    0x8A, 0xEA, 0x71, 0x57, 0x5D, 0x06, 0x0C, 0x7D, 0xB3, 0x97, 0x0F, 0x85, 0xA6, 0xE1, 0xE4, 0xC7,
    0xAB, 0xF5, 0xAE, 0x8C, 0xDB, 0x09, 0x33, 0xD7, 0x1E, 0x8C, 0x94, 0xE0, 0x4A, 0x25, 0x61, 0x9D,
    0xCE, 0xE3, 0xD2, 0x26, 0x1A, 0xD2, 0xEE, 0x6B, 0xF1, 0x2F, 0xFA, 0x06, 0xD9, 0x8A, 0x08, 0x64,
    0xD8, 0x76, 0x02, 0x73, 0x3E, 0xC8, 0x6A, 0x64, 0x52, 0x1F, 0x2B, 0x18, 0x17, 0x7B, 0x20, 0x0C,
    0xBB, 0xE1, 0x17, 0x57, 0x7A, 0x61, 0x5D, 0x6C, 0x77, 0x09, 0x88, 0xC0, 0xBA, 0xD9, 0x46, 0xE2,
    0x08, 0xE2, 0x4F, 0xA0, 0x74, 0xE5, 0xAB, 0x31, 0x43, 0xDB, 0x5B, 0xFC, 0xE0, 0xFD, 0x10, 0x8E,
    0x4B, 0x82, 0xD1, 0x20, 0xA9, 0x3A, 0xD2, 0xCA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

static const char g_3072[] = { 5 };


esp_srp_handle_t *esp_srp_init(esp_ng_type_t ng)
{
    esp_srp_handle_t *hd = calloc(1, sizeof(esp_srp_handle));
    if (!hd) {
        return NULL;
    }

    hd->ctx = esp_mpi_ctx_new();
    if (! hd->ctx) {
        goto error;
    }
    if (ng != ESP_NG_3072) {
        goto error;
    }

    hd->n = esp_mpi_new_from_bin(N_3072, sizeof(N_3072));
    hd->bytes_n = N_3072;
    hd->len_n = sizeof(N_3072);
    if (! hd->n) {
        goto error;
    }

    hd->g = esp_mpi_new_from_bin(g_3072, sizeof(g_3072));
    hd->bytes_g = g_3072;
    hd->len_g = sizeof(g_3072);
    if (! hd->g) {
        goto error;
    }
    hd->type = ng;
    return hd;
error:
    esp_srp_free(hd);
    return NULL;
}

void esp_srp_free(esp_srp_handle_t *hd)
{
    if (!hd) {
        return;
    }
    if (hd->ctx) {
        esp_mpi_ctx_free(hd->ctx);
    }
    if (hd->n) {
        esp_mpi_free(hd->n);
    }
    if (hd->g) {
        esp_mpi_free(hd->g);
    }
    if (hd->s) {
        esp_mpi_free(hd->s);
    }
    if (hd->bytes_s) {
        free(hd->bytes_s);
    }
    if (hd->v) {
        esp_mpi_free(hd->v);
    }
    if (hd->B) {
        esp_mpi_free(hd->B);
    }
    if (hd->bytes_B) {
        free(hd->bytes_B);
    }
    if (hd->b) {
        esp_mpi_free(hd->b);
    }
    if (hd->A) {
        esp_mpi_free(hd->A);
    }
    if (hd->bytes_A) {
        free(hd->bytes_A);
    }
    if (hd->session_key) {
        free(hd->session_key);
    }
    free(hd);
}

static esp_mpi_t *calculate_x(char *bytes_salt, int salt_len, const char *username, int username_len, const char *pass, int pass_len)
{
    unsigned char digest[SHA512_HASH_SZ];
    mbedtls_sha512_context ctx;
    ESP_LOGD(TAG, "Username: %s | Passphrase: %s | Passphrase length: %d", username, pass, pass_len);
    mbedtls_sha512_init(&ctx);
    mbedtls_sha512_starts(&ctx, 0);
    mbedtls_sha512_update(&ctx, (unsigned char *)username, username_len);
    mbedtls_sha512_update(&ctx, (unsigned char *)":", 1);
    mbedtls_sha512_update(&ctx, (unsigned char *)pass, pass_len);
    mbedtls_sha512_finish(&ctx, digest);

    mbedtls_sha512_init(&ctx);
    mbedtls_sha512_starts(&ctx, 0);
    mbedtls_sha512_update(&ctx, (unsigned char *)bytes_salt, salt_len);
    mbedtls_sha512_update(&ctx, digest, sizeof(digest));
    mbedtls_sha512_finish(&ctx, digest);
    mbedtls_sha512_free(&ctx);

    return esp_mpi_new_from_bin((char *)digest, sizeof(digest));
}

static esp_mpi_t *calculate_padded_hash(esp_srp_handle_t *hd, const char *a, int len_a, const char *b, int len_b)
{
    unsigned char digest[SHA512_HASH_SZ];
    mbedtls_sha512_context ctx;
    int pad_len;
    char *s = NULL;

    if (len_a > len_b) {
        pad_len = hd->len_n - len_b;
    } else {
        pad_len = hd->len_n - len_a;
    }

    if (pad_len) {
        s = malloc(pad_len);
        if (s) {
            memset(s, 0, pad_len);
        }
    }

    mbedtls_sha512_init(&ctx);
    mbedtls_sha512_starts(&ctx, 0);
    /* PAD (a) */
    if (s && (len_a != hd->len_n)) {
        mbedtls_sha512_update(&ctx, (unsigned char *)s, hd->len_n - len_a);
    }

    mbedtls_sha512_update(&ctx, (unsigned char *)a, len_a);

    /* PAD (b) */
    if (s && (len_b != hd->len_n)) {
        mbedtls_sha512_update(&ctx, (unsigned char *)s, hd->len_n - len_b);
    }

    mbedtls_sha512_update(&ctx, (unsigned char *)b, len_b);

    mbedtls_sha512_finish(&ctx, digest);
    mbedtls_sha512_free(&ctx);

    if (s) {
        free(s);
    }

    return esp_mpi_new_from_bin((char *)digest, sizeof(digest));
}

/* k = SHA (N, PAD(g))
 *
 * https://tools.ietf.org/html/draft-ietf-tls-srp-08
 */
static esp_mpi_t *calculate_k(esp_srp_handle_t *hd)
{
    return calculate_padded_hash(hd, hd->bytes_n, hd->len_n, hd->bytes_g, hd->len_g);
}

static esp_mpi_t *calculate_u(esp_srp_handle_t *hd, char *A, int len_A)
{
    return calculate_padded_hash(hd, A, len_A, hd->bytes_B, hd->len_B);
}

static esp_err_t __esp_srp_srv_pubkey(esp_srp_handle_t *hd, char **bytes_B, int *len_B)
{
    esp_mpi_t *k = calculate_k(hd);
    esp_mpi_t *kv = NULL;
    esp_mpi_t *gb = NULL;
    if (!k) {
        goto error;
    }
    hexdump_mpi("k", k);

    hd->b = esp_mpi_new();
    if (!hd->b) {
        goto error;
    }
    esp_mpi_get_rand(hd->b, 256, -1, 0);
    hexdump_mpi("b", hd->b);

    /* B = kv + g^b */
    kv = esp_mpi_new();
    gb = esp_mpi_new();
    hd->B = esp_mpi_new();
    if (!kv || !gb || ! hd->B) {
        goto error;
    }
    esp_mpi_a_mul_b_mod_c(kv, k, hd->v, hd->n, hd->ctx);
    esp_mpi_a_exp_b_mod_c(gb, hd->g, hd->b, hd->n, hd->ctx);
    esp_mpi_a_add_b_mod_c(hd->B, kv, gb, hd->n, hd->ctx);
    hd->bytes_B = esp_mpi_to_bin(hd->B, len_B);
    hd->len_B = *len_B;
    *bytes_B = hd->bytes_B;

    esp_mpi_free(k);
    esp_mpi_free(kv);
    esp_mpi_free(gb);
    return ESP_OK;
error:
    if (k) {
        esp_mpi_free(k);
    }
    if (kv) {
        esp_mpi_free(kv);
    }
    if (gb) {
        esp_mpi_free(gb);
    }
    if (hd->B) {
        esp_mpi_free(hd->B);
        hd->B = NULL;
    }
    if (hd->b) {
        esp_mpi_free(hd->b);
        hd->b = NULL;
    }
    return ESP_FAIL;
}

static esp_err_t _esp_srp_gen_salt_verifier(esp_srp_handle_t *hd, const char *username, int username_len,
                                            const char *pass, int pass_len, int salt_len)
{
    /* Get Salt */
    int str_salt_len;
    esp_mpi_t *x = NULL;
    hd->s = esp_mpi_new();
    if (!hd->s) {
        ESP_LOGE(TAG, "Failed to allocate bignum s");
        goto error;
    }

    esp_mpi_get_rand(hd->s, 8 * salt_len, -1, 0);
    hd->bytes_s = esp_mpi_to_bin(hd->s, &str_salt_len);
    if (!hd->bytes_s) {
        ESP_LOGE(TAG, "Failed to generate salt of len %d", salt_len);
        goto error;
    }

    hd->len_s = salt_len;
    ESP_LOGD(TAG, "Salt ->");
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, hd->bytes_s, str_salt_len, ESP_LOG_DEBUG);

    /* Calculate X which is simply a hash for all these things */
    x = calculate_x(hd->bytes_s, str_salt_len, username, username_len, pass, pass_len);
    if (!x) {
        ESP_LOGE(TAG, "Failed to calculate x");
        goto error;
    }
    hexdump_mpi("x", x);

    /* v = g^x % N */
    hd->v = esp_mpi_new();
    if (!hd->v) {
        ESP_LOGE(TAG, "Failed to allocate bignum v");
        goto error;
    }
    esp_mpi_a_exp_b_mod_c(hd->v, hd->g, x, hd->n, hd->ctx);
    hexdump_mpi("Verifier", hd->v);

    esp_mpi_free(x);
    return ESP_OK;

error:
    if (hd->s) {
        esp_mpi_free(hd->s);
        hd->s = NULL;
    }
    if (hd->bytes_s) {
        free(hd->bytes_s);
        hd->bytes_s = NULL;
        hd->len_s = 0;
    }
    if (x) {
        esp_mpi_free(x);
        x = NULL;
    }
    if (hd->v) {
        esp_mpi_free(hd->v);
        hd->v = NULL;
    }
    return ESP_FAIL;
}

esp_err_t esp_srp_srv_pubkey(esp_srp_handle_t *hd, const char *username, int username_len,
                             const char *pass, int pass_len, int salt_len,
                             char **bytes_B, int *len_B, char **bytes_salt)
{
    if (!hd || !username || !pass) {
        return ESP_ERR_INVALID_ARG;
    }
    if (ESP_OK != _esp_srp_gen_salt_verifier(hd, username, username_len, pass, pass_len, salt_len)) {
        goto error;
    }
    *bytes_salt = hd->bytes_s;

    if (__esp_srp_srv_pubkey(hd, bytes_B, len_B) < 0 ) {
        goto error;
    }
    return ESP_OK;

error:
    if (hd->s) {
        esp_mpi_free(hd->s);
        hd->s = NULL;
    }
    if (*bytes_salt) {
        free(*bytes_salt);
        *bytes_salt = NULL;
        hd->bytes_s = NULL;
        hd->len_s = 0;
    }
    if (hd->v) {
        esp_mpi_free(hd->v);
        hd->v = NULL;
    }
    return ESP_FAIL;
}

esp_err_t esp_srp_srv_pubkey_from_salt_verifier(esp_srp_handle_t *hd, char **bytes_B, int *len_B)
{
    if (!hd || !bytes_B || !len_B) {
        return ESP_ERR_INVALID_ARG;
    }
    return __esp_srp_srv_pubkey(hd, bytes_B, len_B);
}

/* Generate salt-verifier pair for given username and password */
esp_err_t esp_srp_gen_salt_verifier(const char *username, int username_len,
                                    const char *pass, int pass_len,
                                    char **bytes_salt, int salt_len,
                                    char **verifier, int *verifier_len)
{
    esp_err_t ret = ESP_FAIL;

    /* allocate and init temporary SRP handle */
    esp_srp_handle_t *srp_hd = esp_srp_init(ESP_NG_3072);
    if (!srp_hd) {
        ESP_LOGE(TAG, "Failed to initialise security context!");
        return ESP_ERR_NO_MEM;
    }

    // get salt and verifier
    if (ESP_OK != _esp_srp_gen_salt_verifier(srp_hd, username, username_len, pass, pass_len, salt_len)) {
        goto cleanup;
    }

    // convert to verifier bytes
    *verifier = esp_mpi_to_bin(srp_hd->v, verifier_len);
    if (!*verifier) {
        ESP_LOGE(TAG, "Failed to allocate verifier bytes!");
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    *bytes_salt = srp_hd->bytes_s;
    srp_hd->bytes_s = NULL; // so that it won't be freed in `esp_srp_free` step
    ret = ESP_OK;

cleanup:
    esp_srp_free(srp_hd);
    return ret;
}

esp_err_t esp_srp_set_salt_verifier(esp_srp_handle_t *hd, const char *salt, int salt_len,
                                    const char *verifier, int verifier_len)
{
    hd->bytes_s = malloc(salt_len);
    if (!hd->bytes_s) {
        goto error;
    }
    memcpy(hd->bytes_s, salt, salt_len);
    hd->len_s = salt_len;

    hd->s = esp_mpi_new_from_bin(salt, salt_len);
    if (!hd->s) {
        goto error;
    }

    hd->v = esp_mpi_new_from_bin(verifier, verifier_len);
    if (!hd->v) {
        goto error;
    }
    return ESP_OK;

error:
    if (hd->bytes_s) {
        free(hd->bytes_s);
        hd->bytes_s = NULL;
        hd->len_s = 0;
    }
    if (hd->s) {
        esp_mpi_free(hd->s);
        hd->s = NULL;
    }
    if (hd->v) {
        esp_mpi_free(hd->v);
        hd->v = NULL;
    }
    return ESP_FAIL;
}

esp_err_t esp_srp_get_session_key(esp_srp_handle_t *hd, char *bytes_A, int len_A, char **bytes_key, uint16_t *len_key)
{
    esp_mpi_t *u = NULL;
    esp_mpi_t *vu = NULL;
    esp_mpi_t *avu = NULL;
    esp_mpi_t *S = NULL;

    char *bytes_S;
    int len_S;

    u = vu = avu = S = NULL;
    bytes_S = NULL;

    hd->bytes_A = malloc(len_A);
    if (! hd->bytes_A) {
        goto error;
    }
    memcpy(hd->bytes_A, bytes_A, len_A);
    hd->len_A = len_A;

    hd->A = esp_mpi_new_from_bin(bytes_A, len_A);
    if (! hd->A) {
        goto error;
    }
    u = calculate_u(hd, bytes_A, len_A);
    if (! u) {
        goto error;
    }
    hexdump_mpi("u", u);

    /* S = (A v^u)^b */
    vu = esp_mpi_new();
    avu = esp_mpi_new();
    S = esp_mpi_new();
    if (!vu || !avu || !S ) {
        goto error;
    }

    esp_mpi_a_exp_b_mod_c(vu, hd->v, u, hd->n, hd->ctx);
    esp_mpi_a_mul_b_mod_c(avu, hd->A, vu, hd->n, hd->ctx);
    esp_mpi_a_exp_b_mod_c(S, avu, hd->b, hd->n, hd->ctx);
    hexdump_mpi("S", S);

    bytes_S = esp_mpi_to_bin(S, &len_S);
    hd->session_key = malloc(SHA512_HASH_SZ);
    if (!hd->session_key || ! bytes_S) {
        goto error;
    }

    mbedtls_sha512((unsigned char *)bytes_S, len_S, (unsigned char *)hd->session_key, 0);
    *bytes_key = hd->session_key;
    *len_key = SHA512_HASH_SZ;

    free(bytes_S);
    esp_mpi_free(vu);
    esp_mpi_free(avu);
    esp_mpi_free(S);
    esp_mpi_free(u);
    return ESP_OK;
error:
    if (bytes_S) {
        free(bytes_S);
    }
    if (vu) {
        esp_mpi_free(vu);
    }
    if (avu) {
        esp_mpi_free(avu);
    }
    if (S) {
        esp_mpi_free(S);
    }
    if (u) {
        esp_mpi_free(u);
    }
    if (hd->session_key) {
        free(hd->session_key);
        hd->session_key = NULL;
    }
    if (hd->A) {
        esp_mpi_free(hd->A);
        hd->A = NULL;
    }
    if (hd->bytes_A) {
        free(hd->bytes_A);
        hd->bytes_A = NULL;
    }
    return ESP_FAIL;
}

esp_err_t esp_srp_exchange_proofs(esp_srp_handle_t *hd, char *username, uint16_t username_len, char *bytes_user_proof, char *bytes_host_proof)
{
    /* First calculate M */
    unsigned char hash_n[SHA512_HASH_SZ];
    unsigned char hash_g[SHA512_HASH_SZ];
    unsigned char hash_n_xor_g[SHA512_HASH_SZ];
    int i;

    unsigned char hash_I[SHA512_HASH_SZ];
    mbedtls_sha512((unsigned char *)username, username_len, (unsigned char *)hash_I, 0);
    mbedtls_sha512((unsigned char *)hd->bytes_n, hd->len_n, (unsigned char *)hash_n, 0);

    int pad_len = hd->len_n - hd->len_g;
    char *s = calloc(pad_len, sizeof(char));
    if (!s) {
        return ESP_ERR_NO_MEM;
    }

    mbedtls_sha512_context ctx;
    mbedtls_sha512_init(&ctx);
    mbedtls_sha512_starts(&ctx, 0);
    mbedtls_sha512_update(&ctx, (unsigned char *)s, pad_len);
    mbedtls_sha512_update(&ctx, (unsigned char *)hd->bytes_g, hd->len_g);
    mbedtls_sha512_finish(&ctx, hash_g);
    mbedtls_sha512_free(&ctx);

    for (i = 0; i < SHA512_HASH_SZ; i++) {
        hash_n_xor_g[i] = hash_n[i] ^ hash_g[i];
    }

    unsigned char digest[SHA512_HASH_SZ];
    mbedtls_sha512_init(&ctx);
    mbedtls_sha512_starts(&ctx, 0);
    mbedtls_sha512_update(&ctx, hash_n_xor_g, SHA512_HASH_SZ);
    mbedtls_sha512_update(&ctx, hash_I, SHA512_HASH_SZ);
    mbedtls_sha512_update(&ctx, (unsigned char *)hd->bytes_s, hd->len_s);
    mbedtls_sha512_update(&ctx, (unsigned char *)hd->bytes_A, hd->len_A);
    mbedtls_sha512_update(&ctx, (unsigned char *)hd->bytes_B, hd->len_B);
    mbedtls_sha512_update(&ctx, (unsigned char *)hd->session_key, SHA512_HASH_SZ);
    mbedtls_sha512_finish(&ctx, digest);
    mbedtls_sha512_free(&ctx);

    ESP_LOGD(TAG, "M ->");
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, (char *)digest, sizeof(digest), ESP_LOG_DEBUG);

    if (memcmp(bytes_user_proof, digest, SHA512_HASH_SZ) != 0) {
        free(s);
        return ESP_FAIL;
    }

    /* M is now validated, let's proceed to H(AMK) */
    mbedtls_sha512_init(&ctx);
    mbedtls_sha512_starts(&ctx, 0);
    mbedtls_sha512_update(&ctx, (unsigned char *)hd->bytes_A, hd->len_A);
    mbedtls_sha512_update(&ctx, digest, SHA512_HASH_SZ);
    mbedtls_sha512_update(&ctx, (unsigned char *)hd->session_key, SHA512_HASH_SZ);
    mbedtls_sha512_finish(&ctx, (unsigned char *)bytes_host_proof);
    mbedtls_sha512_free(&ctx);

    ESP_LOGD(TAG, "AMK ->");
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, (char *)bytes_host_proof, SHA512_HASH_SZ, ESP_LOG_DEBUG);

    if (s) {
        free(s);
    }
    return ESP_OK;
}
