// Hardware crypto support Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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

#include "crypto/includes.h"
#include "crypto/common.h"
#include "mbedtls/sha256.h"

/**
 * fast_sha256_vector - SHA256 hash for data vector
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash
 * Returns: 0 on success, -1 of failure
 */
int 
fast_sha256_vector(size_t num_elem, const uint8_t *addr[], const size_t *len,
		  uint8_t *mac)
{
    mbedtls_sha256_context ctx;

    mbedtls_sha256_init(&ctx);

    mbedtls_sha256_starts(&ctx, 0);

    for(size_t index = 0; index < num_elem; index++) {
        mbedtls_sha256_update(&ctx, addr[index], len[index]);
    }

    mbedtls_sha256_finish(&ctx, mac);

    mbedtls_sha256_free(&ctx);

    return 0;
}


