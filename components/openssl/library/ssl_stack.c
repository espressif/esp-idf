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

#include "ssl_stack.h"
#include "ssl_dbg.h"
#include "ssl_port.h"

#ifndef CONFIG_MIN_NODES
    #define MIN_NODES 4
#else
    #define MIN_NODES CONFIG_MIN_NODES
#endif

/**
 * @brief create a openssl stack object
 */
OPENSSL_STACK* OPENSSL_sk_new(OPENSSL_sk_compfunc c)
{
    OPENSSL_STACK *stack;
    char **data;

    stack = ssl_mem_zalloc(sizeof(OPENSSL_STACK));
    if (!stack)
        SSL_RET(failed1, "ssl_mem_zalloc\n");

    data = ssl_mem_zalloc(sizeof(*data) * MIN_NODES);
    if (!data)
        SSL_RET(failed2, "ssl_mem_zalloc\n");

    stack->data = data;
    stack->num_alloc = MIN_NODES;
    stack->c = c;

    return stack;

failed2:
    ssl_mem_free(stack);
failed1:
    return NULL;
}

/**
 * @brief create a NULL function openssl stack object
 */
OPENSSL_STACK *OPENSSL_sk_new_null(void)
{
    return OPENSSL_sk_new((OPENSSL_sk_compfunc)NULL);
}

/**
 * @brief free openssl stack object
 */
void OPENSSL_sk_free(OPENSSL_STACK *stack)
{
    SSL_ASSERT(stack);

    ssl_mem_free(stack->data);
    ssl_mem_free(stack);
}
