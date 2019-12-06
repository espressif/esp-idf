// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __ESP_TLS_ERROR_CAPTURE_INTERNAL_H__
#define __ESP_TLS_ERROR_CAPTURE_INTERNAL_H__
/**
* Note: this is an implementation placeholder for error logger.
* This version is internal to esp-tls component and only saves single esp_err of last occurred error
*/

#ifdef __cplusplus
extern "C" {
#endif

/**
* Definition of different types/sources of error codes reported
* from different components
*/
typedef enum {
    ERR_TYPE_UNKNOWN = 0,
    ERR_TYPE_SYSTEM,
    ERR_TYPE_MBEDTLS,
    ERR_TYPE_MBEDTLS_CERT_FLAGS,
    ERR_TYPE_ESP,
    ERR_TYPE_WOLFSSL,
    ERR_TYPE_WOLFSSL_CERT_FLAGS,
} err_type_t;

/**
 * Error tracker logging macro, this implementation saves latest errors of
 *  ERR_TYPE_ESP, ERR_TYPE_ESP_TLS and ERR_TYPE_ESP_TLS_CERT_FLAGS types
 */
#define ESP_INT_EVENT_TRACKER_CAPTURE(h, type, code)    esp_int_event_tracker_capture(h, type, code)

static inline void esp_int_event_tracker_capture(esp_tls_error_handle_t h, uint32_t type, int code)
{
    if (h) {
        if (type == ERR_TYPE_ESP) {
            h->last_error = code;
        } else if (type == ERR_TYPE_MBEDTLS) {
            h->esp_tls_error_code = code;
        } else if (type == ERR_TYPE_MBEDTLS_CERT_FLAGS) {
            h->esp_tls_flags = code;
        } else if (type == ERR_TYPE_WOLFSSL) {
            h->esp_tls_error_code = code;
        } else if (type == ERR_TYPE_WOLFSSL_CERT_FLAGS) {
            h->esp_tls_flags = code;
        }
    }
}



#ifdef __cplusplus
}
#endif

#endif //__ESP_TLS_ERROR_CAPTURE_INTERNAL_H__
