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

#ifndef _SSL_X509_H_
#define _SSL_X509_H_

#include "ssl_types.h"
#include "ssl_stack.h"

DEFINE_STACK_OF(X509_NAME)

/*
 * sk_X509_NAME_new_null - create a X509 certification object
 *
 * @param none
 *
 * @return X509 certification object point or NULL if failed
 */
X509* X509_new(void);

X509* d2i_X509(X509 **cert, const unsigned char *buffer, long len);

void X509_free(X509 *cert);

#endif
