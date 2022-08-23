/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Protocomm Security 1 parameters: Proof Of Possession
 */
typedef struct protocomm_security1_params {
    /**
     * Pointer to buffer containing the proof of possession data
     */
    const uint8_t *data;

    /**
     * Length (in bytes) of the proof of possession data
     */
    uint16_t len;
} protocomm_security1_params_t;

typedef protocomm_security1_params_t  protocomm_security_pop_t __attribute__((deprecated("Use protocomm_security1_params_t instead")));

/**
 * @brief Protocomm Security 2 parameters: Salt and Verifier
 *
 */
typedef struct protocomm_security2_params {
    /**
     * Pointer to the buffer containing the salt
     */
    const char *salt;

    /**
     * Length (in bytes) of the salt
     */
    uint16_t salt_len;

    /**
     * Pointer to the buffer containing the verifier
     */
    const char *verifier;

    /**
     * Length (in bytes) of the verifier
     */
    uint16_t verifier_len;
} protocomm_security2_params_t;

typedef void * protocomm_security_handle_t;

/**
 * @brief   Protocomm security object structure.
 *
 * The member functions are used for implementing secure
 * protocomm sessions.
 *
 * @note    This structure should not have any dynamic
 *          members to allow re-entrancy
 */
typedef struct protocomm_security {
    /**
     * Unique version number of security implementation
     */
    int ver;

    /**
     * Function for initializing/allocating security
     * infrastructure
     */
    esp_err_t (*init)(protocomm_security_handle_t *handle);

    /**
     * Function for deallocating security infrastructure
     */
    esp_err_t (*cleanup)(protocomm_security_handle_t handle);

    /**
     * Starts new secure transport session with specified ID
     */
    esp_err_t (*new_transport_session)(protocomm_security_handle_t handle,
                                       uint32_t session_id);

    /**
     * Closes a secure transport session with specified ID
     */
    esp_err_t (*close_transport_session)(protocomm_security_handle_t handle,
                                         uint32_t session_id);

    /**
     * Handler function for authenticating connection
     * request and establishing secure session
     */
    esp_err_t (*security_req_handler)(protocomm_security_handle_t handle,
                                      const void *sec_params,
                                      uint32_t session_id,
                                      const uint8_t *inbuf, ssize_t inlen,
                                      uint8_t **outbuf, ssize_t *outlen,
                                      void *priv_data);

    /**
     * Function which implements the encryption algorithm
     */
    esp_err_t (*encrypt)(protocomm_security_handle_t handle,
                         uint32_t session_id,
                         const uint8_t *inbuf, ssize_t inlen,
                         uint8_t **outbuf, ssize_t *outlen);

    /**
     * Function which implements the decryption algorithm
     */
    esp_err_t (*decrypt)(protocomm_security_handle_t handle,
                         uint32_t session_id,
                         const uint8_t *inbuf, ssize_t inlen,
                         uint8_t **outbuf, ssize_t *outlen);
} protocomm_security_t;

#ifdef __cplusplus
}
#endif
