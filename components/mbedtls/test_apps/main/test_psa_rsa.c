/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "esp_log.h"

#include "psa/crypto.h"
#include "mbedtls/pk.h"
#include "mbedtls/pem.h"
#include "mbedtls/error.h"
#include "unity.h"
#include "ccomp_timer.h"
#include "test_utils.h"

typedef enum {
    PSA_RSA_KEY_SIZE_2048,
    PSA_RSA_KEY_SIZE_3072,
    PSA_RSA_KEY_SIZE_4096,
} psa_rsa_key_size_t;

static const char privkey_4096_buf[] = "-----BEGIN RSA PRIVATE KEY-----\n"
                                  "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\n"
                                  "-----END RSA PRIVATE KEY-----";

static const char privkey_2048_buf[] = "-----BEGIN RSA PRIVATE KEY-----\r\n"
    "MIIEowIBAAKCAQEA8N8hdkemvj6Tpk975/OWhv9BrTsCBCu+ZYfDb5VI7U2meKBg\r\n"
    "3dAkyyhRlY3fNwSRzBUMCzsHjpgnsB40wxOgiwlB9n6PMhq0qUVKAdCpKwFztsKd\r\n"
    "JJAsCUC+Zlwxn4RpH6ZnMl3a/njRYjuDyI32kucMP/lBRo7ks1798Gy/j+x1h5xA\r\n"
    "vZSlFoEXKjCC6S1DWhALePuZnk4m/jGP6g+YfyJXSTqsenKa/DcWndfn/JoElZ0J\r\n"
    "nhud8lBXwVe6mMheE1yqfL+VTU1nwg/TPNZrZsFz2sXig/RQCKt6LuSuzhRpsLp+\r\n"
    "BdwqEs9xrwlhZnp7j4kQBomISd6kAxQfYVROHQIDAQABAoIBAHgtO4rB8QWWPyCJ\r\n"
    "I670r7OnA2OkvzrJgHMzq2SuvPX4+gfRLMM+qDzcXugZIrdWhk+maJ3p07lnXNXY\r\n"
    "HEcAMedstQaA2n0LKfwSX/xL2TtlvBABRVoKvI3ZSaXUdcW60KBD69ULUsoICZ/T\r\n"
    "Rcr4WX+t20TH3bOQc7ayvEwKVgE95xIUpTH9asw8uOPvKxW2j5OLQgZuWrWyUDg0\r\n"
    "MFh92PhWtw3i5zq6OpTTsFJeceKYV/VstIYjZ+FslmhjQxJbr+2DJRbpHXKceqy6\r\n"
    "9yWlSV0EM7neFCHlDa2WPhK8we+6IvMiNVQKj46fHGYNBaW/ZSX7TiG5J0Uqj2e9\r\n"
    "0MUGJ8ECgYEA+frJabhfzW5+JfGjTObeznJZE6fAOjFzaBIwFu8Kz2mIjYpQlwVK\r\n"
    "EepMkv2KkrJuqS4GnI+Nkq7G0BAUyUj9tTJ3HQzvtJrxsnxVi99Yofx1s1P4YAnu\r\n"
    "c8t3ElJoQ4BRoQIs/hIvyYn22IxllBHiGESrnPQ38D82xyXQgd6S8JkCgYEA9qww\r\n"
    "j7jx6Xpy/D1Dq8Dvalm7pz3J+yHnti4w2cqZ67grUoyGnNPtciNDdfi4JzLiKkUu\r\n"
    "SDS3DacvFpFyND0m8sbpMjnR8Rvhj+bfH8KcOAowD+YR/+6vSb/P/aBt6gYXcaBn\r\n"
    "cjepx+sE81mnC7UrHb4TjG4hO5t3ZTc6X28gyCUCgYAMZn9lSisecrO5SCJUp0M4\r\n"
    "NH3stq6XdGqIKBbQnG0J2u9WLh1PUIjbGKdRx1f/bPCGXe0gCRL5yse7/IA7d+51\r\n"
    "9ZnpDAI8EE+bDgXkWWD5MB/alHjGstdsURSICSR47L2f4g6/T8GlGr3vAg/r53My\r\n"
    "xv1IXOkFdu1NtbeBKbxaSQKBgENDmw5mAVmIcXiFAEICn4ahp4EoYT6g9T2BhQKu\r\n"
    "s6BKnU2qUj7Lr5ETOp8dzqGpx3B9Yux/q3cGotmFmd3S2x8SzJ5MlAoqbyy9aRSR\r\n"
    "DeZeKNL9CuV+YcA7lOz1ZWOOe7AZbHwB38NLPBNb3CheI769iTkfAuLtNvabw8go\r\n"
    "VokdAoGBALyvBhW+Squ5tx8NOEgAisakhAVOnT6jcoeKy6FyjcvKaWagmCOCC7Gz\r\n"
    "QB9Yf1tJ+3di+aLtWWdmU494iKJHBtPMhfrYltCpxHHQGlUc/GLPY3Z5bBYYYWpb\r\n"
    "Wzw4ZvDraKlAs7a9CRwS5cpktk5ptK4rc5noSXkvV+yOT75zXat2\r\n"
    "-----END RSA PRIVATE KEY-----\r\n";

static const char privkey_3072_buf[] = "-----BEGIN RSA PRIVATE KEY-----\r\n"
    "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\r\n"
    "-----END RSA PRIVATE KEY-----\r\n";

// Keep the old version for reference (has issues with PSA-based PK)
static int pem_to_der_rsa_key(const char *pem_key, size_t pem_key_len,
                               uint8_t *der_buf, size_t der_buf_size,
                               uint8_t **der_data_ptr, size_t *der_len)
{
    // Use direct PEM parsing instead of PK layer for PSA compatibility
    // return pem_to_der_rsa_key_direct(pem_key, pem_key_len, der_buf, der_buf_size,
    //                                   der_data_ptr, der_len);

    mbedtls_pk_context pk;
    int ret;

    mbedtls_pk_init(&pk);

    // Parse PEM key
    ret = mbedtls_pk_parse_key(&pk,
                               (const uint8_t *)pem_key,
                               pem_key_len,
                               NULL, 0);  // No password
    if (ret != 0) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        printf("mbedtls_pk_parse_key failed: -0x%04x - %s\n", -ret, error_buf);
        mbedtls_pk_free(&pk);
        return ret;
    }

    // Write key to DER format
    // NOTE: mbedtls_pk_write_key_der writes to the END of the buffer!
    // Returns the length on success, or negative error code
    printf("Attempting to write DER key (buffer size: %zu)...\n", der_buf_size);
    ret = mbedtls_pk_write_key_der(&pk, der_buf, der_buf_size);
    if (ret < 0) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        printf("mbedtls_pk_write_key_der failed: -0x%04x - %s\n", -ret, error_buf);
        mbedtls_pk_free(&pk);
        return ret;
    }

    printf("DER key written successfully, length: %d\n", ret);

    // ret contains the length of DER data
    *der_len = ret;

    // Calculate the start position of DER data (at end of buffer)
    *der_data_ptr = der_buf + der_buf_size - ret;

    mbedtls_pk_free(&pk);
    return 0;
}

static psa_key_id_t import_rsa_key(psa_rsa_key_size_t key_size)
{
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t status;
    int ret;

    // Convert PEM to DER format
    uint8_t *der_buf = calloc(1, 10000);   // Buffer for DER-encoded key (data written at end)
    uint8_t *der_key = NULL; // Pointer to actual DER data location
    size_t der_key_len = 0;

    char *key_buf = NULL;

    if (key_size == PSA_RSA_KEY_SIZE_2048) {
        key_buf = (char *)privkey_2048_buf;
    } else if (key_size == PSA_RSA_KEY_SIZE_3072) {
        key_buf = (char *)privkey_3072_buf;
    } else if (key_size == PSA_RSA_KEY_SIZE_4096) {
        key_buf = (char *)privkey_4096_buf;
    } else {
        printf("Unsupported key size for import_rsa_key\n");
        free(der_buf);
        return 0;
    }

    ret = pem_to_der_rsa_key(key_buf,
                             strlen(key_buf) + 1,  // Include null terminator
                             der_buf,
                             10000,
                             &der_key,      // Returns pointer to DER data
                             &der_key_len);
    TEST_ASSERT_EQUAL(0, ret);

    // Configure key attributes for RSA encryption/decryption
    psa_set_key_type(&attributes, PSA_KEY_TYPE_RSA_KEY_PAIR);
    psa_set_key_algorithm(&attributes, PSA_ALG_RSA_PKCS1V15_CRYPT);
    psa_set_key_usage_flags(&attributes,
                           PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_VOLATILE);
    size_t key_bits = 0;
    if (key_size == PSA_RSA_KEY_SIZE_2048) {
        key_bits = 2048;
    } else if (key_size == PSA_RSA_KEY_SIZE_3072) {
        key_bits = 3072;
    } else if (key_size == PSA_RSA_KEY_SIZE_4096) {
        key_bits = 4096;
    }
    psa_set_key_bits(&attributes, key_bits);

    status = psa_import_key(&attributes,
                           der_key,  // Pointer to DER data (at end of buffer)
                           der_key_len,
                           &key_id);
    if (status != PSA_SUCCESS) {
        printf("PSA import failed with error: %ld (0x%x)\n", status, (unsigned int)status);
        printf("Expected error codes:\n");
        printf("  PSA_ERROR_INVALID_ARGUMENT = %ld\n", PSA_ERROR_INVALID_ARGUMENT);
        printf("  PSA_ERROR_NOT_SUPPORTED = %ld\n", PSA_ERROR_NOT_SUPPORTED);
        printf("  PSA_ERROR_INSUFFICIENT_MEMORY = %ld\n", PSA_ERROR_INSUFFICIENT_MEMORY);
    }
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    free(der_buf);
    psa_reset_key_attributes(&attributes);
    return key_id;
}

TEST_CASE("test performance RSA key operations", "[bignum]")
{
    psa_status_t status;
    psa_rsa_key_size_t keysize = PSA_RSA_KEY_SIZE_2048;
    for (int i = 0; i < 3; i++) {
        // Use der_key (not der_buf) as it points to the actual DER data at end of buffer
        psa_key_id_t key_id = import_rsa_key(keysize);
        printf("RSA key imported successfully (key_id: %u)\n", (unsigned int)key_id);

        size_t ciphertext_size = 0;
        if (keysize == PSA_RSA_KEY_SIZE_2048) {
            ciphertext_size = 256; // 2048 bits / 8
        } else if (keysize == PSA_RSA_KEY_SIZE_3072) {
            ciphertext_size = 384; // 3072 bits / 8
        } else if (keysize == PSA_RSA_KEY_SIZE_4096) {
            ciphertext_size = 512; // 4096 bits / 8
        } else {
            printf("Unsupported key size for ciphertext size calculation\n");
            return;
        }

        uint8_t plaintext[] = "Test message for RSA encryption";
        size_t plaintext_len = sizeof(plaintext);
        uint8_t ciphertext[ciphertext_size]; // RSA 2048-bit key produces 256-byte ciphertext
        size_t ciphertext_len = sizeof(ciphertext);
        uint8_t decrypted[ciphertext_size];
        size_t decrypted_len = sizeof(decrypted);
        size_t encrypt_len = 0;

#ifdef SOC_CCOMP_TIMER_SUPPORTED
        int public_perf, private_perf;
        ccomp_timer_start();
#endif
        // Encrypt the plaintext
        status = psa_asymmetric_encrypt(key_id,
                                        PSA_ALG_RSA_PKCS1V15_CRYPT,
                                        plaintext,
                                        plaintext_len,
                                        NULL,
                                        0,
                                        ciphertext,
                                        ciphertext_len,
                                        &encrypt_len);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
#ifdef SOC_CCOMP_TIMER_SUPPORTED
        public_perf = ccomp_timer_stop();
#endif // SOC_CCOMP_TIMER_SUPPORTED

        size_t decrypt_len = 0;
#ifdef SOC_CCOMP_TIMER_SUPPORTED
        ccomp_timer_start();
#endif
        // Decrypt the ciphertext
        status = psa_asymmetric_decrypt(key_id,
                                        PSA_ALG_RSA_PKCS1V15_CRYPT,
                                        ciphertext,
                                        encrypt_len,
                                        NULL,
                                        0,
                                        decrypted,
                                        decrypted_len,
                                        &decrypt_len);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    #ifdef SOC_CCOMP_TIMER_SUPPORTED
        private_perf = ccomp_timer_stop();
    #endif // SOC_CCOMP_TIMER_SUPPORTED

        // Verify decrypted data matches original plaintext
        TEST_ASSERT_EQUAL(plaintext_len, decrypt_len);

    #ifdef SOC_CCOMP_TIMER_SUPPORTED
        printf("RSA Key Size: %d bits\n", (keysize == PSA_RSA_KEY_SIZE_2048) ? 2048 :
                                            (keysize == PSA_RSA_KEY_SIZE_3072) ? 3072 : 4096);
        printf("Encryption took %d us, Decryption took %d us\n", public_perf, private_perf);

        if (keysize == PSA_RSA_KEY_SIZE_2048) {
            TEST_PERFORMANCE_CCOMP_LESS_THAN(RSA_2048KEY_PUBLIC_OP, "%d us", public_perf);
            TEST_PERFORMANCE_CCOMP_LESS_THAN(RSA_2048KEY_PRIVATE_OP, "%d us", private_perf);
        } else if (keysize == 4096) {
            TEST_PERFORMANCE_CCOMP_LESS_THAN(RSA_4096KEY_PUBLIC_OP, "%d us", public_perf);
            TEST_PERFORMANCE_CCOMP_LESS_THAN(RSA_4096KEY_PRIVATE_OP, "%d us", private_perf);
        }
    #endif // SOC_CCOMP_TIMER_SUPPORTED
        psa_destroy_key(key_id);
        keysize++;
    }
}
