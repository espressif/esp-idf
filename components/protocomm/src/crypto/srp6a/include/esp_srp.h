/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_srp_mpi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    /* SRP specific:
     * N = 3072 bit large safe prime,
     * g = generator */
    ESP_NG_3072 = 0,
} esp_ng_type_t;

typedef struct esp_srp_handle {
    int allocated;
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
} esp_srp_handle_t;

int esp_srp_init(esp_srp_handle_t *hd, esp_ng_type_t ng);

void esp_srp_free(esp_srp_handle_t *hd);

/* Returns B (pub key) and salt
 *
 * *bytes_B MUST NOT BE FREED BY THE CALLER
 * *bytes_salt MUST NOT BE FREE BY THE CALLER
 *
 */
esp_err_t esp_srp_srv_pubkey(esp_srp_handle_t *hd, const char *username, int username_len, const char *pass, int pass_len, int salt_len,
                             char **bytes_B, int *len_B, char **bytes_salt);

/* Set the Salt and Verifier pre-generated for a given password.
 * This should be used only if the actual password is not available.
 * The public key can then be generated using esp_srp_srv_pubkey_from_salt_verifier()
 * and not esp_srp_srv_pubkey()
 */
esp_err_t esp_srp_set_salt_verifier(esp_srp_handle_t *hd, const char *salt, int salt_len,
                                    const char *verifier, int verifier_len);

/* Returns B (pub key) when the salt and verifier are set using esp_srp_set_salt_verifier()
 *
 * *bytes_B MUST NOT BE FREED BY THE CALLER
 */
esp_err_t esp_srp_srv_pubkey_from_salt_verifier(esp_srp_handle_t *hd, char **bytes_B, int *len_B);

/* Returns bytes_key
 * *bytes_key MUST NOT BE FREED BY THE CALLER
 */
esp_err_t esp_srp_get_session_key(esp_srp_handle_t *hd, char *bytes_A, int len_A, char **bytes_key, uint16_t *len_key);

/* Exchange proofs
 * Returns 1 if user's proof is ok. Also 1 when is returned, bytes_host_proof contains our proof.
 *
 * bytes_user_proof is parameter in
 * bytes_host_proof is parameter out (should be SHA512_DIGEST_LENGTH) bytes in size
 */
esp_err_t esp_srp_exchange_proofs(esp_srp_handle_t *hd, char *username, uint16_t username_len, char *bytes_user_proof, char *bytes_host_proof);

#ifdef __cplusplus
}
#endif
