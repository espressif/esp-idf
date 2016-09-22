// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#include "ssl_cert.h"
#include "ssl_pkey.h"
#include "ssl_x509.h"
#include "ssl_dbg.h"
#include "ssl_port.h"

/*
 * ssl_cert_new - create a certification object include private key object
 *
 * @param none
 *
 * @return certification object point or NULL if failed
 */
CERT *ssl_cert_new(void)
{
    CERT *cert;

    cert = ssl_zalloc(sizeof(CERT));
    if (!cert)
        SSL_RET(failed1, "ssl_zalloc\n");

    cert->pkey = EVP_PKEY_new();
    if (!cert->pkey)
        SSL_RET(failed2, "EVP_PKEY_new\n");

    cert->x509 = sk_X509_NAME_new_null();
    if (!cert->x509)
        SSL_RET(failed3, "sk_X509_NAME_new_null\n");

    return cert;

failed3:
    EVP_PKEY_free(cert->pkey);
failed2:
    ssl_free(cert);
failed1:
    return NULL;
}

/*
 * ssl_cert_free - free a certification object
 *
 * @param c - certification object point
 *
 * @return none
 */
void ssl_cert_free(CERT *c)
{
    X509_free(c->x509);

    EVP_PKEY_free(c->pkey);

    ssl_free(c);
}
