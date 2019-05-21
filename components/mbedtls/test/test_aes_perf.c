/* mbedTLS AES performance test
*/
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <esp_system.h>
#include "mbedtls/aes.h"
#include "unity.h"
#include "sdkconfig.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "test_utils.h"

TEST_CASE("mbedtls AES performance", "[aes]")
{
    const unsigned CALLS = 256;
    const unsigned CALL_SZ = 32*1024;
    mbedtls_aes_context ctx;
    int64_t start, end;
    uint8_t iv[16];
    uint8_t key[16];

    memset(iv, 0xEE, 16);
    memset(key, 0x44, 16);

    // allocate internal memory
    uint8_t *buf = heap_caps_malloc(CALL_SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buf);
    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, key, 128);

    start = esp_timer_get_time();
    for (int c = 0; c < CALLS; c++) {
        memset(buf, 0xAA, CALL_SZ);
        mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, CALL_SZ, iv, buf, buf);
    }
    end = esp_timer_get_time();

    /* Sanity check: make sure the last ciphertext block matches
       what we expect to see.

       Last block produced via this Python:
       import os, binascii
       from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
       from cryptography.hazmat.backends import default_backend
       key = b'\x44' * 16
       iv = b'\xee' * 16
       cipher = Cipher(algorithms.AES(key), modes.CBC(iv), backend=default_backend())
       encryptor = cipher.encryptor()
       ct = encryptor.update(b'\xaa' * 256 * 32 * 1024) + encryptor.finalize()
       print(binascii.hexlify(ct[-16:]))
    */
    const uint8_t expected_last_block[] = {
        0x50, 0x81, 0xe0, 0xe1, 0x15, 0x2f, 0x14, 0xe9,
        0x97, 0xa0, 0xc6, 0xe6, 0x36, 0xf3, 0x5c, 0x25,
    };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_last_block, buf + CALL_SZ - 16, 16);

    free(buf);

    float usecs = end - start;
    // bytes/usec = MB/sec
    float mb_sec = (CALL_SZ * CALLS) / usecs;
    printf("Encryption rate %.3fMB/sec\n", mb_sec);
#ifdef CONFIG_MBEDTLS_HARDWARE_AES
    // Don't put a hard limit on software AES performance (software is approx 2.3MB/sec on Release config)
    TEST_PERFORMANCE_GREATER_THAN(AES_CBC_THROUGHPUT_MBSEC, "%.3fMB/sec", mb_sec);
#endif
}


