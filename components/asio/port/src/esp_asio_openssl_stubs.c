/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
//

#include "esp_asio_config.h"
#include "internal/ssl_dbg.h"
#include "openssl/esp_asio_openssl_stubs.h"

// Unsupported features as macros to make the assertions more readable
#define ESP_OPENSSL_DH_IS_SUPPORTED 0
#define ESP_OPENSSL_GENERAL_NAMES_IS_SUPPORTED 0

void DH_free (DH *r)
{
    SSL_ASSERT3(ESP_OPENSSL_DH_IS_SUPPORTED);
}

DH *PEM_read_bio_DHparams(BIO *bp, DH **x, pem_password_cb *cb, void *u)
{
    SSL_ASSERT2(ESP_OPENSSL_DH_IS_SUPPORTED);
    return NULL;
}

int SSL_CTX_set_tmp_dh(SSL_CTX *ctx, const DH *dh)
{
    SSL_ASSERT1(ESP_OPENSSL_DH_IS_SUPPORTED);
    return -1;
}

void GENERAL_NAMES_free(GENERAL_NAMES * gens)
{
    SSL_ASSERT3(ESP_OPENSSL_GENERAL_NAMES_IS_SUPPORTED);
}

X509_NAME *X509_get_subject_name(X509 *a)
{
    SSL_ASSERT2(ESP_OPENSSL_GENERAL_NAMES_IS_SUPPORTED);
    return NULL;
}

int SSL_CTX_clear_chain_certs(SSL_CTX *ctx)
{
    return 1;
}
