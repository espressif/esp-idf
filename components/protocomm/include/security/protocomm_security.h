// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Proof Of Possession for authenticating a secure session
 */
typedef struct protocomm_security_pop {
    /**
     * Pointer to buffer containing the proof of possession data
     */
    const uint8_t *data;

    /**
     * Length (in bytes) of the proof of possession data
     */
    uint16_t len;
} protocomm_security_pop_t;

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
                                      const protocomm_security_pop_t *pop,
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
                         uint8_t *outbuf, ssize_t *outlen);

    /**
     * Function which implements the decryption algorithm
     */
    esp_err_t (*decrypt)(protocomm_security_handle_t handle,
                         uint32_t session_id,
                         const uint8_t *inbuf, ssize_t inlen,
                         uint8_t *outbuf, ssize_t *outlen);
} protocomm_security_t;

#ifdef __cplusplus
}
#endif
