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

#ifndef _HASH_FUNCTIONS_H_
#define _HASH_FUNCTIONS_H_

#include "osi/hash_map.h"

typedef unsigned char hash_key_t[4];

hash_index_t hash_function_naive(const void *key);

hash_index_t hash_function_integer(const void *key);

// Hashes a pointer based only on its address value
hash_index_t hash_function_pointer(const void *key);

hash_index_t hash_function_string(const void *key);

void hash_function_blob(const unsigned char *s, unsigned int len, hash_key_t h);

#endif /* _HASH_FUNCTIONS_H_ */
