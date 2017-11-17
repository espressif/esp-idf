// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Hardware crypto support Copyright 2017 Espressif Systems (Shanghai) PTE LTD
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "crypto/includes.h"

#include "crypto/common.h"
#include "crypto/aes.h"
#include "crypto/aes_wrap.h"
#include "mbedtls/aes.h"

/**
 * fast_aes_wrap - Wrap keys with AES Key Wrap Algorithm (128-bit KEK) (RFC3394)
 * @kek: 16-octet Key encryption key (KEK)
 * @n: Length of the plaintext key in 64-bit units; e.g., 2 = 128-bit = 16
 * bytes
 * @plain: Plaintext key to be wrapped, n * 64 bits
 * @cipher: Wrapped key, (n + 1) * 64 bits
 * Returns: 0 on success, -1 on failure
 */
int fast_aes_wrap(const uint8_t *kek, int n, const uint8_t *plain, uint8_t *cipher)
{
    uint8_t *a, *r, b[16];
    int32_t i, j;
    int32_t ret = 0;
    mbedtls_aes_context ctx;

    a = cipher;
    r = cipher + 8;

    /* 1) Initialize variables. */
    os_memset(a, 0xa6, 8);
    os_memcpy(r, plain, 8 * n);

    mbedtls_aes_init(&ctx);
    ret = mbedtls_aes_setkey_enc(&ctx, kek, 128);
    if (ret < 0) {
        mbedtls_aes_free(&ctx);
        return ret;
    }

    /* 2) Calculate intermediate values.
     * For j = 0 to 5
     *     For i=1 to n
     *         B = AES(K, A | R[i])
     *         A = MSB(64, B) ^ t where t = (n*j)+i
     *         R[i] = LSB(64, B)
     */
    for (j = 0; j <= 5; j++) {
	r = cipher + 8;
	for (i = 1; i <= n; i++) {
            os_memcpy(b, a, 8);
            os_memcpy(b + 8, r, 8);
            mbedtls_aes_encrypt(&ctx, b, b);
            os_memcpy(a, b, 8);
            a[7] ^= n * j + i;
            os_memcpy(r, b + 8, 8);
            r += 8;
	}
    }
    mbedtls_aes_free(&ctx);

    /* 3) Output the results.
     *
     * These are already in @cipher due to the location of temporary
     * variables.
     */

    return 0;
}
