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
#include "mbedtls/aes.h"

/**
 * fast_aes_unwrap - Unwrap key with AES Key Wrap Algorithm (128-bit KEK) (RFC3394)
 * @kek: Key encryption key (KEK)
 * @n: Length of the plaintext key in 64-bit units; e.g., 2 = 128-bit = 16
 * bytes
 * @cipher: Wrapped key to be unwrapped, (n + 1) * 64 bits
 * @plain: Plaintext key, n * 64 bits
 * Returns: 0 on success, -1 on failure (e.g., integrity verification failed)
 */
int 
fast_aes_unwrap(const uint8_t *kek, int n, const uint8_t *cipher, uint8_t *plain)
{
    uint8_t a[8], *r, b[16];
    int32_t i, j;
    int32_t ret = 0;
    mbedtls_aes_context ctx;

    /* 1) Initialize variables. */
    os_memcpy(a, cipher, 8);
    r = plain;
    os_memcpy(r, cipher + 8, 8 * n);

    mbedtls_aes_init(&ctx);
    ret = mbedtls_aes_setkey_dec(&ctx, kek, 128);
    if (ret < 0) {
        mbedtls_aes_free(&ctx);
        return ret;
    }

    /* 2) Compute intermediate values.
     * For j = 5 to 0
     *     For i = n to 1
     *         B = AES-1(K, (A ^ t) | R[i]) where t = n*j+i
     *         A = MSB(64, B)
     *         R[i] = LSB(64, B)
     */
    for (j = 5; j >= 0; j--) {
        r = plain + (n - 1) * 8;
        for (i = n; i >= 1; i--) {
            os_memcpy(b, a, 8);
            b[7] ^= n * j + i;
            os_memcpy(b + 8, r, 8);
            mbedtls_aes_decrypt(&ctx, b, b);
            os_memcpy(a, b, 8);
            os_memcpy(r, b + 8, 8);
            r -= 8;
        }
    }
    mbedtls_aes_free(&ctx);

    /* 3) Output results.
     *
     * These are already in @plain due to the location of temporary
     * variables. Just verify that the IV matches with the expected value.
     */
    for (i = 0; i < 8; i++) {
        if (a[i] != 0xa6) {
            return -1;
        }
    }

    return 0;
}
