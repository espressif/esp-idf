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


#ifndef _ESP_CRT_BUNDLE_H_
#define _ESP_CRT_BUNDLE_H_

#include "mbedtls/ssl.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief      Attach and enable use of a bundle for certificate verification
 *
 * Attach and enable use of a bundle for certificate verification through a verification callback.
 * If no specific bundle has been set through esp_crt_bundle_set() it will default to the
 * bundle defined in menuconfig and embedded in the binary.
 *
 * @param[in]  conf      The config struct for the SSL connection.
 *
 * @return
 *             - ESP_OK  if adding certificates was successful.
 *             - Other   if an error occured or an action must be taken by the calling process.
 */
esp_err_t esp_crt_bundle_attach(void *conf);


/**
 * @brief      Disable and dealloc the certification bundle
 *
 * Removes the certificate verification callback and deallocates used resources
 *
 * @param[in]  conf      The config struct for the SSL connection.
 */
void esp_crt_bundle_detach(mbedtls_ssl_config *conf);


/**
 * @brief      Set the default certificate bundle used for verification
 *
 * Overrides the default certificate bundle. In most use cases the bundle should be
 * set through menuconfig. The bundle needs to be sorted by subject name since binary search is
 * used to find certificates.
 *
 * @param[in]  x509_bundle     A pointer to the certificate bundle.
 */
void esp_crt_bundle_set(const uint8_t *x509_bundle);


#ifdef __cplusplus
}
#endif

#endif //_ESP_CRT_BUNDLE_H_
