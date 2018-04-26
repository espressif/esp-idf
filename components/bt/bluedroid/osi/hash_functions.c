/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#include <string.h>

#include "osi/hash_functions.h"

hash_index_t hash_function_naive(const void *key)
{
    return (hash_index_t)key;
}

hash_index_t hash_function_integer(const void *key)
{
    return ((hash_index_t)key) * 2654435761;
}

hash_index_t hash_function_pointer(const void *key)
{
    return ((hash_index_t)key) * 2654435761;
}

hash_index_t hash_function_string(const void *key)
{
    hash_index_t hash = 5381;
    const char *name = (const char *)key;
    size_t string_len = strlen(name);
    for (size_t i = 0; i < string_len; ++i) {
        hash = ((hash << 5) + hash ) + name[i];
    }
    return hash;
}

void hash_function_blob(const unsigned char *s, unsigned int len, hash_key_t h)
{
    size_t j;

    while (len--) {
        j = sizeof(hash_key_t)-1;

        while (j) {
            h[j] = ((h[j] << 7) | (h[j-1] >> 1)) + h[j];
            --j;
        }

        h[0] = (h[0] << 7) + h[0] + *s++;
    }
}

