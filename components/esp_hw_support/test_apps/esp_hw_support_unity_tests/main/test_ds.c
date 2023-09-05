/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "unity.h"

#include "soc/soc_caps.h"

#if SOC_DIG_SIGN_SUPPORTED
#if CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/efuse.h"
#include "esp32s2/rom/digital_signature.h"
#include "esp32s2/rom/aes.h"
#include "esp32s2/rom/sha.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/efuse.h"
#include "esp32c3/rom/digital_signature.h"
#include "esp32c3/rom/hmac.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/efuse.h"
#include "esp32s3/rom/digital_signature.h"
#include "esp32s3/rom/aes.h"
#include "esp32s3/rom/sha.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/rom/efuse.h"
#include "esp32c6/rom/digital_signature.h"
#include "esp32c6/rom/aes.h"
#include "esp32c6/rom/sha.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/efuse.h"
#include "esp32h2/rom/digital_signature.h"
#include "esp32h2/rom/aes.h"
#include "esp32h2/rom/sha.h"
#elif CONFIG_IDF_TARGET_ESP32P4
#include "esp32p4/rom/efuse.h"
#include "esp32p4/rom/digital_signature.h"
#include "esp32p4/rom/aes.h"
#include "esp32p4/rom/sha.h"
#endif

#include "esp_ds.h"

#define NUM_RESULTS 10

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#define DS_MAX_BITS (4096)
#else
#define DS_MAX_BITS (ETS_DS_MAX_BITS)
#endif

typedef struct {
    uint8_t iv[ETS_DS_IV_LEN];
    ets_ds_p_data_t p_data;
    uint8_t expected_c[ETS_DS_C_LEN];
    uint8_t hmac_key_idx;
    uint32_t expected_results[NUM_RESULTS][DS_MAX_BITS / 32];
} encrypt_testcase_t;

// Generated header digital_signature_test_cases_<bits>.h (by gen_digital_signature_tests.py) defines
// NUM_HMAC_KEYS, test_hmac_keys, NUM_MESSAGES, NUM_CASES, test_messages[], test_cases[]
// Some adaptations were made: removed the 512 bit case and changed RSA lengths to the enums from esp_ds.h
#if DS_MAX_BITS == 4096
#define RSA_LEN         (ESP_DS_RSA_4096)
#include "digital_signature_test_cases_4096.h"
#elif DS_MAX_BITS == 3072
#define RSA_LEN         (ESP_DS_RSA_3072)
#include "digital_signature_test_cases_3072.h"
#endif

_Static_assert(NUM_RESULTS == NUM_MESSAGES, "expected_results size should be the same as NUM_MESSAGES in generated header");

TEST_CASE("Digital Signature Parameter Encryption data NULL", "[hw_crypto] [ds]")
{
    const char iv [32] = {0};
    esp_ds_p_data_t p_data = {0};
    const char key [32] = {0};

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ds_encrypt_params(NULL, iv, &p_data, key));
}

TEST_CASE("Digital Signature Parameter Encryption iv NULL", "[hw_crypto] [ds]")
{
    esp_ds_data_t data = {0};
    esp_ds_p_data_t p_data = {0};
    const char key [32] = {0};

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ds_encrypt_params(&data, NULL, &p_data, key));
}

TEST_CASE("Digital Signature Parameter Encryption p_data NULL", "[hw_crypto] [ds]")
{
    esp_ds_data_t data = {0};
    const char iv [32] = {0};
    const char key [32] = {0};

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ds_encrypt_params(&data, iv, NULL, key));
}

TEST_CASE("Digital Signature Parameter Encryption key NULL", "[hw_crypto] [ds]")
{
    esp_ds_data_t data = {0};
    const char iv [32] = {0};
    esp_ds_p_data_t p_data = {0};

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ds_encrypt_params(&data, iv, &p_data, NULL));
}

TEST_CASE("Digital Signature Parameter Encryption", "[hw_crypto] [ds]")
{
    for (int i = 0; i < NUM_CASES; i++) {
        printf("Encrypting test case %d...\n", i);
        const encrypt_testcase_t *t = &test_cases[i];
        esp_ds_data_t result = { };
        esp_ds_p_data_t p_data;

        memcpy(p_data.Y, t->p_data.Y, DS_MAX_BITS / 8);
        memcpy(p_data.M, t->p_data.M, DS_MAX_BITS / 8);
        memcpy(p_data.Rb, t->p_data.Rb, DS_MAX_BITS / 8);
        p_data.M_prime = t->p_data.M_prime;
        p_data.length = t->p_data.length;

        esp_err_t r = esp_ds_encrypt_params(&result, t->iv, &p_data,
                                            test_hmac_keys[t->hmac_key_idx]);
        printf("Encrypting test case %d done\n", i);
        TEST_ASSERT_EQUAL(ESP_OK, r);
        TEST_ASSERT_EQUAL(t->p_data.length, result.rsa_length);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(t->iv, result.iv, ETS_DS_IV_LEN);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(t->expected_c, result.c, ETS_DS_C_LEN);
    }
}

TEST_CASE("Digital Signature start Invalid message", "[hw_crypto] [ds]")
{
    esp_ds_data_t ds_data = { };
    ds_data.rsa_length = RSA_LEN;
    esp_ds_context_t *ctx;

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ds_start_sign(NULL, &ds_data, HMAC_KEY1, &ctx));
}

TEST_CASE("Digital Signature start Invalid data", "[hw_crypto] [ds]")
{
    const char *message = "test";
    esp_ds_context_t *ctx;

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ds_start_sign(message, NULL, HMAC_KEY1, &ctx));
}

TEST_CASE("Digital Signature start Invalid context", "[hw_crypto] [ds]")
{
    esp_ds_data_t ds_data = {};
    ds_data.rsa_length = RSA_LEN;
    const char *message = "test";

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ds_start_sign(message, &ds_data, HMAC_KEY1, NULL));
}

TEST_CASE("Digital Signature RSA length 0", "[hw_crypto] [ds]")
{
    esp_ds_data_t ds_data = {};
    ds_data.rsa_length = 0;
    const char *message = "test";

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ds_start_sign(message, &ds_data, HMAC_KEY1, NULL));
}

TEST_CASE("Digital Signature RSA length too long", "[hw_crypto] [ds]")
{
    esp_ds_data_t ds_data = {};
    ds_data.rsa_length = 128;
    const char *message = "test";

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ds_start_sign(message, &ds_data, HMAC_KEY1, NULL));
}

TEST_CASE("Digital Signature start HMAC key out of range", "[hw_crypto] [ds]")
{
    esp_ds_data_t ds_data = {};
    ds_data.rsa_length = RSA_LEN;
    esp_ds_context_t *ctx;
    const char *message = "test";

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ds_start_sign(message, &ds_data, HMAC_KEY5 + 1, &ctx));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ds_start_sign(message, &ds_data, HMAC_KEY0 - 1, &ctx));
}

TEST_CASE("Digital Signature finish Invalid signature ptr", "[hw_crypto] [ds]")
{
    esp_ds_context_t *ctx = NULL;

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ds_finish_sign(NULL, ctx));
}

TEST_CASE("Digital Signature finish Invalid context", "[hw_crypto] [ds]")
{
    uint8_t signature_data [128 * 4];

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ds_finish_sign(signature_data, NULL));
}

TEST_CASE("Digital Signature Blocking Invalid message", "[hw_crypto] [ds]")
{
    esp_ds_data_t ds_data = { };
    ds_data.rsa_length = RSA_LEN;
    uint8_t signature_data [128 * 4];

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ds_sign(NULL, &ds_data, HMAC_KEY1, signature_data));
}

TEST_CASE("Digital Signature Blocking Invalid data", "[hw_crypto] [ds]")
{
    const char *message = "test";
    uint8_t signature_data [128 * 4];

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ds_sign(message, NULL, HMAC_KEY1, signature_data));
}

TEST_CASE("Digital Signature Blocking Invalid signature ptr", "[hw_crypto] [ds]")
{
    esp_ds_data_t ds_data = {};
    ds_data.rsa_length =  RSA_LEN;
    const char *message = "test";

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ds_sign(message, &ds_data, HMAC_KEY1, NULL));
}

TEST_CASE("Digital Signature Blocking RSA length 0", "[hw_crypto] [ds]")
{
    esp_ds_data_t ds_data = {};
    ds_data.rsa_length = 0;
    const char *message = "test";
    uint8_t signature_data [128 * 4];

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ds_sign(message, &ds_data, HMAC_KEY1, signature_data));
}

TEST_CASE("Digital Signature Blocking RSA length too long", "[hw_crypto] [ds]")
{
    esp_ds_data_t ds_data = {};
    ds_data.rsa_length = 128;
    const char *message = "test";
    uint8_t signature_data [128 * 4];

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ds_sign(message, &ds_data, HMAC_KEY1, signature_data));
}

TEST_CASE("Digital Signature Blocking HMAC key out of range", "[hw_crypto] [ds]")
{
    esp_ds_data_t ds_data = {};
    ds_data.rsa_length = 127;
    const char *message = "test";
    uint8_t signature_data [128 * 4];

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ds_sign(message, &ds_data, HMAC_KEY5 + 1, signature_data));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ds_sign(message, &ds_data, HMAC_KEY0 - 1, signature_data));
}

#if CONFIG_IDF_ENV_FPGA

static void burn_hmac_keys(void)
{
    printf("Burning %d HMAC keys to efuse...\n", NUM_HMAC_KEYS);
    for (int i = 0; i < NUM_HMAC_KEYS; i++) {
        // TODO: vary the purpose across the keys
        ets_efuse_purpose_t purpose = ETS_EFUSE_KEY_PURPOSE_HMAC_DOWN_DIGITAL_SIGNATURE;

        // starting from block 1, block 0 occupied with HMAC upstream test key
        int __attribute__((unused)) ets_status = ets_efuse_write_key(ETS_EFUSE_BLOCK_KEY1 + i,
                purpose,
                test_hmac_keys[i], 32);

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
        if (ets_status == ESP_OK) {
            printf("written DS test key to block [%d]!\n", ETS_EFUSE_BLOCK_KEY1 + i);
        } else {
            printf("writing DS test key to block [%d] failed, maybe written already\n", ETS_EFUSE_BLOCK_KEY1 + i);
        }
#endif
    }
#if CONFIG_IDF_TARGET_ESP32C3
    /* verify the keys are what we expect (possibly they're already burned, doesn't matter but they have to match) */
    uint8_t block_compare[32];
    for (int i = 0; i < NUM_HMAC_KEYS; i++) {
        printf("Checking key %d...\n", i);
        memcpy(block_compare, (void *)ets_efuse_get_read_register_address(ETS_EFUSE_BLOCK_KEY1 + i), 32);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(test_hmac_keys[i], block_compare, 32);
    }
#endif
}

// This test uses the HMAC_KEY0 eFuse key which hasn't been burned by burn_hmac_keys().
// HMAC_KEY0 is usually used for HMAC upstream (user access) tests.
TEST_CASE("Digital Signature wrong HMAC key purpose (FPGA only)", "[hw_crypto] [ds]")
{
    esp_ds_data_t ds_data = {};
    ds_data.rsa_length =  RSA_LEN;
    esp_ds_context_t *ctx;
    const char *message = "test";

    // HMAC fails in that case because it checks for the correct purpose
    TEST_ASSERT_EQUAL(ESP_ERR_HW_CRYPTO_DS_HMAC_FAIL, esp_ds_start_sign(message, &ds_data, HMAC_KEY0, &ctx));
}

// This test uses the HMAC_KEY0 eFuse key which hasn't been burned by burn_hmac_keys().
// HMAC_KEY0 is usually used for HMAC upstream (user access) tests.
TEST_CASE("Digital Signature Blocking wrong HMAC key purpose (FPGA only)", "[hw_crypto] [ds]")
{
    esp_ds_data_t ds_data = {};
    ds_data.rsa_length = RSA_LEN;
    const char *message = "test";
    uint8_t signature_data [128 * 4];

    // HMAC fails in that case because it checks for the correct purpose
    TEST_ASSERT_EQUAL(ESP_ERR_HW_CRYPTO_DS_HMAC_FAIL, esp_ds_sign(message, &ds_data, HMAC_KEY0, signature_data));
}

TEST_CASE("Digital Signature Operation (FPGA only)", "[hw_crypto] [ds]")
{
    burn_hmac_keys();

    for (int i = 0; i < NUM_CASES; i++) {
        printf("Running test case %d...\n", i);
        const encrypt_testcase_t *t = &test_cases[i];

        // copy encrypt parameter test case into ds_data structure
        esp_ds_data_t ds_data = { };
        memcpy(ds_data.iv, t->iv, ETS_DS_IV_LEN);
        memcpy(ds_data.c, t->expected_c, ETS_DS_C_LEN);
        ds_data.rsa_length = t->p_data.length;

        for (int j = 0; j < NUM_MESSAGES; j++) {
            uint8_t signature[DS_MAX_BITS / 8] = { 0 };
            printf(" ... message %d\n", j);

            esp_ds_context_t *esp_ds_ctx;

            esp_err_t ds_r = esp_ds_start_sign(test_messages[j],
                                               &ds_data,
                                               t->hmac_key_idx + 1,
                                               &esp_ds_ctx);
            TEST_ASSERT_EQUAL(ESP_OK, ds_r);

            ds_r = esp_ds_finish_sign(signature, esp_ds_ctx);
            TEST_ASSERT_EQUAL(ESP_OK, ds_r);

            TEST_ASSERT_EQUAL_HEX8_ARRAY(t->expected_results[j], signature, sizeof(signature));
        }
#if CONFIG_IDF_TARGET_ESP32C3
        ets_hmac_invalidate_downstream(ETS_EFUSE_KEY_PURPOSE_HMAC_DOWN_DIGITAL_SIGNATURE);
#endif
    }
}

TEST_CASE("Digital Signature Blocking Operation (FPGA only)", "[hw_crypto] [ds]")
{
    burn_hmac_keys();

    for (int i = 0; i < NUM_CASES; i++) {
        printf("Running test case %d...\n", i);
        const encrypt_testcase_t *t = &test_cases[i];

        // copy encrypt parameter test case into ds_data structure
        esp_ds_data_t ds_data = { };
        memcpy(ds_data.iv, t->iv, ETS_DS_IV_LEN);
        memcpy(ds_data.c, t->expected_c, ETS_DS_C_LEN);
        ds_data.rsa_length = t->p_data.length;

        uint8_t signature[DS_MAX_BITS / 8] = { 0 };
#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
        esp_ds_context_t *esp_ds_ctx;

        esp_err_t ds_r = esp_ds_start_sign(test_messages[0],
                                           &ds_data,
                                           t->hmac_key_idx + 1,
                                           &esp_ds_ctx);
        TEST_ASSERT_EQUAL(ESP_OK, ds_r);

        ds_r = esp_ds_finish_sign(signature, esp_ds_ctx);
        TEST_ASSERT_EQUAL(ESP_OK, ds_r);
#else
        esp_err_t ds_r = esp_ds_sign(test_messages[0],
                                     &ds_data,
                                     t->hmac_key_idx + 1,
                                     signature);
        TEST_ASSERT_EQUAL(ESP_OK, ds_r);
#endif

        TEST_ASSERT_EQUAL_HEX8_ARRAY(t->expected_results[0], signature, sizeof(signature));
    }
}

TEST_CASE("Digital Signature Invalid Data (FPGA only)", "[hw_crypto] [ds]")
{
    burn_hmac_keys();

    // Set up a valid test case
    const encrypt_testcase_t *t = &test_cases[0];
    esp_ds_data_t ds_data = { };
    memcpy(ds_data.iv, t->iv, ETS_DS_IV_LEN);
    memcpy(ds_data.c, t->expected_c, ETS_DS_C_LEN);
    ds_data.rsa_length = t->p_data.length;

    uint8_t signature[DS_MAX_BITS / 8] = { 0 };
    const uint8_t zero[DS_MAX_BITS / 8] = { 0 };

    // Corrupt the IV one bit at a time, rerun and expect failure
    for (int bit = 0; bit < 128; bit++) {
        printf("Corrupting IV bit %d...\n", bit);
        ds_data.iv[bit / 8] ^= 1 << (bit % 8);
        esp_ds_context_t *esp_ds_ctx;

        esp_err_t ds_r = esp_ds_start_sign(test_messages[0], &ds_data, t->hmac_key_idx + 1, &esp_ds_ctx);
        TEST_ASSERT_EQUAL(ESP_OK, ds_r);
        ds_r = esp_ds_finish_sign(signature, esp_ds_ctx);
        TEST_ASSERT_EQUAL(ESP_ERR_HW_CRYPTO_DS_INVALID_DIGEST, ds_r);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(zero, signature, DS_MAX_BITS / 8);

        ds_data.iv[bit / 8] ^= 1 << (bit % 8);
    }

    // Corrupt encrypted key data one bit at a time, rerun and expect failure
    printf("Corrupting C...\n");
    for (int bit = 0; bit < ETS_DS_C_LEN * 8; bit++) {
        printf("Corrupting C bit %d...\n", bit);
        ds_data.c[bit / 8] ^= 1 << (bit % 8);
        esp_ds_context_t *esp_ds_ctx;

        esp_err_t ds_r = esp_ds_start_sign(test_messages[0], &ds_data, t->hmac_key_idx + 1, &esp_ds_ctx);
        TEST_ASSERT_EQUAL(ESP_OK, ds_r);
        ds_r = esp_ds_finish_sign(signature, esp_ds_ctx);
        TEST_ASSERT_EQUAL(ESP_ERR_HW_CRYPTO_DS_INVALID_DIGEST, ds_r);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(zero, signature, DS_MAX_BITS / 8);

        ds_data.c[bit / 8] ^= 1 << (bit % 8);
    }
}

#endif // CONFIG_IDF_ENV_FPGA
#endif // SOC_DIG_SIGN_SUPPORTED
