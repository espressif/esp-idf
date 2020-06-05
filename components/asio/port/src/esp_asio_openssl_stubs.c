// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
