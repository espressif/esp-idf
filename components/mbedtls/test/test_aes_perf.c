/* mbedTLS AES performance test
*/
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <esp_system.h>
#include "mbedtls/aes.h"
#include "mbedtls/gcm.h"
#include "unity.h"
#include "sdkconfig.h"
#include "esp_heap_caps.h"
#include "test_utils.h"
#include "ccomp_timer.h"

TEST_CASE("mbedtls AES performance", "[aes][timeout=60]")
{
    const unsigned CALLS = 256;
    const unsigned CALL_SZ = 32 * 1024;
    mbedtls_aes_context ctx;
    float elapsed_usec;
    uint8_t iv[16];
    uint8_t key[16];

    memset(iv, 0xEE, 16);
    memset(key, 0x44, 16);

    // allocate internal memory
    uint8_t *buf = heap_caps_malloc(CALL_SZ, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buf);
    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, key, 128);

    ccomp_timer_start();
    for (int c = 0; c < CALLS; c++) {
        memset(buf, 0xAA, CALL_SZ);
        mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, CALL_SZ, iv, buf, buf);
    }
    elapsed_usec = ccomp_timer_stop();

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

    // bytes/usec = MB/sec
    float mb_sec = (CALL_SZ * CALLS) / elapsed_usec;
    printf("Encryption rate %.3fMB/sec\n", mb_sec);
#ifdef CONFIG_MBEDTLS_HARDWARE_AES
    // Don't put a hard limit on software AES performance
    TEST_PERFORMANCE_GREATER_THAN(AES_CBC_THROUGHPUT_MBSEC, "%.3fMB/sec", mb_sec);
#endif
}


TEST_CASE("mbedtls AES GCM performance", "[aes]")
{
    const unsigned CALL_SZ = 32 * 1024;
    mbedtls_gcm_context ctx;
    float elapsed_usec;
    unsigned char tag_buf[16];
    mbedtls_cipher_id_t cipher = MBEDTLS_CIPHER_ID_AES;
    uint8_t iv[16];
    uint8_t key[16];

    memset(iv, 0xEE, 16);
    memset(key, 0x44, 16);

    // allocate internal memory
    uint8_t *buf = heap_caps_malloc(CALL_SZ, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buf);
    uint8_t aad[16];
    memset(aad, 0x22, 16);

    mbedtls_gcm_init(&ctx);
    mbedtls_gcm_setkey( &ctx, cipher, key, 128);

    ccomp_timer_start();

    memset(buf, 0xAA, CALL_SZ);
    mbedtls_gcm_crypt_and_tag(&ctx, MBEDTLS_AES_ENCRYPT, CALL_SZ, iv, sizeof(iv), aad, sizeof(aad), buf, buf, 16, tag_buf);

    elapsed_usec = ccomp_timer_stop();

    /* Sanity check: make sure the last ciphertext block matches
       what we expect to see.

       Last block and tag produced via this Python:

        import os, binascii
        from cryptography.hazmat.primitives.ciphers.aead import AESGCM

        key = b'\x44' * 16
        iv = b'\xEE' * 16
        data = b'\xAA' * 100
        aad = b'\x22 * 16

        aesgcm = AESGCM(key)

        ct = aesgcm.encrypt(iv, data, aad)
    */
    const uint8_t expected_last_block[] = {
        0x7d, 0x3d, 0x16, 0x84, 0xd0, 0xb4, 0x38, 0x30,
        0xd1, 0x24, 0x6f, 0x7e, 0x9a, 0x9c, 0x81, 0x58,
    };

    const uint8_t expected_tag[] = {
        0x7e, 0x16, 0x04, 0x07, 0x4b, 0x7e, 0x6b, 0xf7,
        0x5d, 0xce, 0x9e, 0x7d, 0x3f, 0x85, 0xc5, 0xa5,
    };

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_last_block, buf + CALL_SZ - 16 , 16);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_tag, tag_buf, 16);

    free(buf);

    // bytes/usec = MB/sec
    float mb_sec = CALL_SZ / elapsed_usec;
    printf("GCM encryption rate %.3fMB/sec\n", mb_sec);

#ifdef CONFIG_MBEDTLS_HARDWARE_GCM
    // Don't put a hard limit on software AES performance
    TEST_PERFORMANCE_GREATER_THAN(AES_GCM_THROUGHPUT_MBSEC, "%.3fMB/sec", mb_sec);
#endif
}


