/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef _ESP_CRT_BUNDLE_H_
#define _ESP_CRT_BUNDLE_H_

#include "esp_err.h"
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
 *             - Other   if an error occurred or an action must be taken by the calling process.
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
 * Overrides the default certificate bundle only in case of successful initialization. In most use cases the bundle should be
 * set through menuconfig. The bundle needs to be sorted by subject name since binary search is
 * used to find certificates.
 *
 * @param[in]  x509_bundle     A pointer to the certificate bundle.
 *
 * @param[in]  bundle_size     Size of the certificate bundle in bytes.
 *
 * @return
 *             - ESP_OK  if adding certificates was successful.
 *             - Other   if an error occurred or an action must be taken by the calling process.
 */
esp_err_t esp_crt_bundle_set(const uint8_t *x509_bundle, size_t bundle_size);

/**
 * @brief   Check if the given CA certificate chain is the default "dummy"
 *          certificate chain attached by the esp_crt_bundle
 *
 * @param ca_chain  A pointer to the CA chain.
 * @return true     if the ca_chain is the dummy CA chain attached by esp_crt_bundle
 * @return false    otherwise
 */
bool esp_crt_bundle_in_use(const mbedtls_x509_crt* ca_chain);

#ifdef __cplusplus
}
#endif

#endif //_ESP_CRT_BUNDLE_H_
