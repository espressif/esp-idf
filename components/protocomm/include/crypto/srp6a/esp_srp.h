/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file esp_srp.h
 * @brief SRP-6a protocol implementation
 *
 * More information on protocol can be found: https://datatracker.ietf.org/doc/html/rfc5054
 *
 * This implementation is used by security2 of wifi_provisioning and local control features.
 * Details on how these protocols use this feature can be found here: https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-reference/provisioning/provisioning.html#security-2-scheme
 *
 * Below is the example usage of the srp6a protocol in protocomm component,
 * which can help understand the APIs better.
 *
 * Variables used:
 *
 *    N, g: group parameters (prime and generator)
 *    s: salt
 *    B, b: server's public and private values
 *    A, a: client's public and private values
 *    I: user name (aka "identity")
 *    P: password
 *    v: verifier
 *    k: SRP-6 multiplier
 *
 *      salt (s) is random of given length, 16 in our case, which along with username and password
 *      is used to generate verifier.
 *
 *      x = SHA1(s | SHA1(I | ":" | P))
 *      v = g^x % N
 *
 *  Steps involved (From protocomm component usage):
 *      Step1. Client Hello (PhoneApp):
 *          a. Generate Key pair:
 *              a (cli_privkey) = 256 bit random value,
 *              A (cli_pubkey) = g^a.
 *              g - generator, N - large safe prime, All arithmetic operations are performed in ring of integers modulo N,
 *                  thus all occurrences like y^z should be read as y^z modulo N.
 *          b. SessionCmd0 (cli_pubkey, username I)
 *
 *      Step2. Device(ESP):
 *          a. Obtain Salt and verifier stored on ESP
 *              Salt s = 256 bit random value,
 *              Verifier v = g^× where x = H(s | I | P)
 *          b. Generate Key Pair
 *              b (dev_privkey) = 256 bit random value
 *              B(dev_pubkey) = k*v + g^b where k = H(N, g)
 *          c. Shared Key K = H(S) where,
 *              S = (A * v^u) ^ b
 *              u = H(A, B)
 *          d. SessionResp0(dev_pubkey B, dev_random)
 *
 *      Step3. Client (PhoneApp)
 *          a. shared_key(K) = H(S) where,
 *              S = (B - k*v) ^ (a + ux),
 *              u = H(A, B),
 *              k = H(N, g),
 *              V = g^x,
 *              x = H(s | I | P).
 *          b. Verification token
 *              client proof M = H[H(N) XOR H(g) | H(I) | S | A | B | K]
 *          c. SessionCmd1(Client proof M1)
 *
 *      Step4. Device (ESP):
 *          a. Verify client:
 *              device generates M1 = H[H(N) XOR H(g) | H(I) | S | A | B | K]
 *              device verifies this M1 with the M1 obtained from Client
 *          b. Verification token: Device generate device proof M2 = H(A, M, K)
 *          c. Initialization Vector(IV):
 *              dev_rand = gen_16byte_random) This random number is to be used for AES-GCM operation
 *              for encryption and decryption of the data using the shared secret
 *          d. SessionResp1 (DeviceProofM2, dev_rand)
 *
 *      Step5. Client (PhoneApp)
 *          a. Verify Device
 *              Client calculates device proof M2 as M2 = H(A, M, K)
 *              verifies this M2 with M2 obtained from device
 */

/**
 * @brief Large prime+generator to be used for the algorithm
 */
typedef enum {
    /* SRP specific:
     * N = 3072 bit large safe prime,
     * g = generator */
    ESP_NG_3072 = 0,
} esp_ng_type_t;

/**
 * @brief esp_srp handle as the result of `esp_srp_init`
 *
 * The handle is returned by `esp_srp_init` on successful init. It is then
 * passed for subsequent API calls as an argument. `esp_srp_free` can be used to
 * clean up the handle. After `esp_srp_free` the handle becomes invalid.
 */
typedef struct esp_srp_handle esp_srp_handle_t;

/**
 * @brief Initialize srp context for given NG type
 *
 * @param ng NG type given by `esp_ng_type_t`
 * @return   esp_srp_handle_t* srp handle
 *
 * @note    the handle gets freed with `esp_srp_free`
 */
esp_srp_handle_t *esp_srp_init(esp_ng_type_t ng);

/**
 * @brief free esp_srp_context
 *
 * @param hd handle to be free
 */
void esp_srp_free(esp_srp_handle_t *hd);

/**
 * @brief   Returns B (pub key) and salt. [Step2.b]
 *
 * @param hd    esp_srp handle
 * @param username  Username not expected NULL terminated
 * @param username_len  Username length
 * @param pass      Password not expected to be NULL terminated
 * @param pass_len  Pasword length
 * @param salt_len  Salt length
 * @param bytes_B   Public Key returned
 * @param len_B     Length of the public key
 * @param bytes_salt    Salt bytes generated
 * @return esp_err_t    ESP_OK on success, appropriate error otherwise
 *
 * @note    *bytes_B MUST NOT BE FREED BY THE CALLER
 * @note    *bytes_salt MUST NOT BE FREE BY THE CALLER
 */
esp_err_t esp_srp_srv_pubkey(esp_srp_handle_t *hd, const char *username, int username_len,
                             const char *pass, int pass_len, int salt_len,
                             char **bytes_B, int *len_B, char **bytes_salt);

/**
 * @brief   Generate salt-verifier pair, given username, password and salt length
 *
 * @param[in] username      username
 * @param[in] username_len  length of the username
 * @param[in] pass          password
 * @param[in] pass_len      length of the password
 * @param[out] bytes_salt   generated salt on successful generation, or NULL
 * @param[in] salt_len      salt length
 * @param[out] verifier     generated verifier on successful generation, or NULL
 * @param[out] verifier_len length of the generated verifier
 * @return esp_err_t        ESP_OK on success, appropriate error otherwise
 *
 * @note if API has returned ESP_OK, salt and verifier generated need to be freed by caller
 * @note Usually, username and password are not saved on the device. Rather salt and verifier are
 *      generated outside the device and are embedded.
 *      this covenience API can be used to generate salt and verifier on the fly for development use case.
 *      OR for devices which intentionally want to generate different password each time and can send it
 *      to the client securely. e.g., a device has a display and it shows the pin
 */
esp_err_t esp_srp_gen_salt_verifier(const char *username, int username_len,
                                    const char *pass, int pass_len,
                                    char **bytes_salt, int salt_len,
                                    char **verifier, int *verifier_len);

/**
 * @brief       Set the Salt and Verifier pre-generated for a given password.
 * This should be used only if the actual password is not available.
 * The public key can then be generated using esp_srp_srv_pubkey_from_salt_verifier()
 * and not esp_srp_srv_pubkey()
 *
 * @param hd            esp_srp_handle
 * @param salt          pre-generated salt bytes
 * @param salt_len      length of the salt bytes
 * @param verifier      pre-generated verifier
 * @param verifier_len  length of the verifier bytes
 * @return esp_err_t    ESP_OK on success, appropriate error otherwise
 */
esp_err_t esp_srp_set_salt_verifier(esp_srp_handle_t *hd, const char *salt, int salt_len,
                                    const char *verifier, int verifier_len);

/**
 * @brief   Returns B (pub key)[Step2.b] when the salt and verifier are set using esp_srp_set_salt_verifier()
 *
 * @param hd        esp_srp handle
 * @param bytes_B   Key returned to the called
 * @param len_B     Length of the key returned
 * @return esp_err_t ESP_OK on success, appropriate error otherwise
 *
 * @note    *bytes_B MUST NOT BE FREED BY THE CALLER
 */
esp_err_t esp_srp_srv_pubkey_from_salt_verifier(esp_srp_handle_t *hd, char **bytes_B, int *len_B);

/**
 * @brief   Get session key in `*bytes_key` given by len in `*len_key`. [Step2.c].
 *
 * This calculated session key is used for further communication given the proofs are
 * exchanged/authenticated with `esp_srp_exchange_proofs`
 *
 * @param hd        esp_srp handle
 * @param bytes_A   Private Key
 * @param len_A     Private Key length
 * @param bytes_key Key returned to the caller
 * @param len_key   length of the key in *bytes_key
 * @return esp_err_t ESP_OK on success, appropriate error otherwise
 *
 * @note    *bytes_key MUST NOT BE FREED BY THE CALLER
 */
esp_err_t esp_srp_get_session_key(esp_srp_handle_t *hd, char *bytes_A, int len_A, char **bytes_key, uint16_t *len_key);

/**
 * @brief Complete the authentication. If this step fails, the session_key exchanged should not be used
 *
 * This is the final authentication step in SRP algorithm [Step4.1, Step4.b, Step4.c]
 *
 * @param hd                esp_srp handle
 * @param username          Username not expected NULL terminated
 * @param username_len      Username length
 * @param bytes_user_proof  param in
 * @param bytes_host_proof  parameter out (should be SHA512_DIGEST_LENGTH) bytes in size
 * @return esp_err_t    ESP_OK if user's proof is ok and subsequently bytes_host_proof is populated with our own proof.
 */
esp_err_t esp_srp_exchange_proofs(esp_srp_handle_t *hd, char *username, uint16_t username_len,
                                  char *bytes_user_proof, char *bytes_host_proof);

#ifdef __cplusplus
}
#endif
