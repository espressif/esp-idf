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

#include "ssl_x509.h"
#include "ssl_methods.h"
#include "ssl_dbg.h"
#include "ssl_port.h"

/*
 * sk_X509_NAME_new_null - create a X509 certification object
 *
 * @param none
 *
 * @return X509 certification object point or NULL if failed
 */
X509* sk_X509_NAME_new_null(void)
{
    int ret;
    X509 *x;

    x = ssl_malloc(sizeof(X509));
    if (!x)
        SSL_RET(failed1, "ssl_malloc\n");

    x->method = X509_method();

    ret = x->method->x509_new(x);
    if (ret)
        SSL_RET(failed2, "x509_new\n");

    return x;

failed2:
    ssl_free(x);
failed1:
    return NULL;
}

/*
 * X509_free - free a X509 certification object
 *
 * @param x - X509 certification object point
 *
 * @return none
 */
void X509_free(X509 *x)
{
    X509_METHOD_CALL(free, x);

    ssl_free(x);
};

/*
 * d2i_X509 - load a character certification context into system context. If '*cert' is pointed to the
 *            certification, then load certification into it. Or create a new X509 certification object
 *
 * @param cert   - a point pointed to X509 certification
 * @param buffer - a point pointed to the certification context memory point
 * @param length - certification bytes
 *
 * @return X509 certification object point or NULL if failed
 */
X509* d2i_X509(X509 **cert, const unsigned char *buffer, long len)
{
    int ret;
    X509 *x;

    SSL_ASSERT(buffer);
    SSL_ASSERT(len);

    if (cert && *cert) {
        x = *cert;
    } else {
        x = sk_X509_NAME_new_null();
        if (!x)
            SSL_RET(failed1, "sk_X509_NAME_new_null\n");
    }

    ret = X509_METHOD_CALL(load, x, buffer, len);
    if (ret)
        SSL_RET(failed2, "x509_load\n");

    return x;

failed2:
    X509_free(x);
failed1:
    return NULL;
}
