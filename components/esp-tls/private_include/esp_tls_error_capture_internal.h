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
} err_type_t;

/**
* Internal structure for error description
* - contains only the last error of esp_err_t in this implementation
*/
typedef struct esp_error_private {
    esp_err_t last_error;
} esp_error_private_t;

/**
 * Error tracker logging macro, this implementation only saves the ERR_TYPE_ESP error, other types are ignored
 */
#define ESP_INT_EVENT_TRACKER_CAPTURE(h, type, code)    do { if (h && type==ERR_TYPE_ESP) { h->last_error = code; } } while (0)

#endif //__ESP_TLS_ERROR_CAPTURE_INTERNAL_H__
