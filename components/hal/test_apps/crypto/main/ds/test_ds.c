/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "esp_private/esp_crypto_lock_internal.h"
#include "memory_checks.h"
#include "unity_fixture.h"

#include "soc/soc_caps.h"
typedef enum {
	HMAC_KEY0 = 0,
	HMAC_KEY1,
	HMAC_KEY2,
	HMAC_KEY3,
	HMAC_KEY4,
	HMAC_KEY5,
	HMAC_KEY_MAX
} hmac_key_id_t;

#if CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/efuse.h"
#include "esp32s2/rom/digital_signature.h"
#include "esp32s2/rom/aes.h"
#include "esp32s2/rom/sha.h"
#include "esp32s2/rom/hmac.h"
#include "soc/soc_memory_layout.h"
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

#define ESP_ERR_HW_CRYPTO_DS_HMAC_FAIL           (0x1) /*!< HMAC peripheral problem */
#define ESP_ERR_HW_CRYPTO_DS_INVALID_KEY         (0x2) /*!< given HMAC key isn't correct, HMAC peripheral problem */
#define ESP_ERR_HW_CRYPTO_DS_INVALID_DIGEST      (0x4) /*!< message digest check failed, result is invalid */
#define ESP_ERR_HW_CRYPTO_DS_INVALID_PADDING     (0x5) /*!< padding check failed, but result is produced anyway and can be read*/

#define ESP_DS_IV_BIT_LEN 128
#define ESP_DS_IV_LEN (ESP_DS_IV_BIT_LEN / 8)
#define ESP_DS_SIGNATURE_MAX_BIT_LEN SOC_RSA_MAX_BIT_LEN
#define ESP_DS_SIGNATURE_MD_BIT_LEN 256
#define ESP_DS_SIGNATURE_M_PRIME_BIT_LEN 32
#define ESP_DS_SIGNATURE_L_BIT_LEN 32
#define ESP_DS_SIGNATURE_PADDING_BIT_LEN 64

#define ESP_DS_C_LEN (((ESP_DS_SIGNATURE_MAX_BIT_LEN * 3 \
        + ESP_DS_SIGNATURE_MD_BIT_LEN   \
        + ESP_DS_SIGNATURE_M_PRIME_BIT_LEN   \
        + ESP_DS_SIGNATURE_L_BIT_LEN   \
        + ESP_DS_SIGNATURE_PADDING_BIT_LEN) / 8))

typedef enum {
    ESP_DS_RSA_1024 = (1024 / 32) - 1,
    ESP_DS_RSA_2048 = (2048 / 32) - 1,
    ESP_DS_RSA_3072 = (3072 / 32) - 1,
    ESP_DS_RSA_4096 = (4096 / 32) - 1
} esp_digital_signature_length_t;

typedef struct esp_digital_signature_data {
    esp_digital_signature_length_t rsa_length;
    uint32_t iv[ESP_DS_IV_BIT_LEN / 32];
    uint8_t c[ESP_DS_C_LEN];
} esp_ds_data_t;

typedef struct {
    uint32_t  Y[ESP_DS_SIGNATURE_MAX_BIT_LEN / 32];
    uint32_t  M[ESP_DS_SIGNATURE_MAX_BIT_LEN / 32];
    uint32_t Rb[ESP_DS_SIGNATURE_MAX_BIT_LEN / 32];
    uint32_t M_prime;
    uint32_t length;
} esp_ds_p_data_t;

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


#if DS_MAX_BITS == 4096
#define RSA_LEN         (ESP_DS_RSA_4096)
#include "digital_signature_test_cases_4096.h"
#elif DS_MAX_BITS == 3072
#define RSA_LEN         (ESP_DS_RSA_3072)
#include "digital_signature_test_cases_3072.h"
#endif

_Static_assert(NUM_RESULTS == NUM_MESSAGES, "expected_results size should be the same as NUM_MESSAGES in generated header");

#if !CONFIG_IDF_TARGET_ESP32S2

#include "esp_private/periph_ctrl.h"
#include "hal/ds_hal.h"
#include "hal/ds_ll.h"
#include "hal/hmac_hal.h"
#include "hal/hmac_ll.h"


static void ds_acquire_enable(void)
{
    HMAC_RCC_ATOMIC() {
        hmac_ll_enable_bus_clock(true);
        hmac_ll_reset_register();
    }

    periph_module_enable(PERIPH_SHA_MODULE);

    DS_RCC_ATOMIC() {
        ds_ll_enable_bus_clock(true);
        ds_ll_reset_register();
    }

    hmac_hal_start();
}

static void ds_disable_release(void)
{
    ds_hal_finish();

    DS_RCC_ATOMIC() {
        ds_ll_enable_bus_clock(false);
    }

    periph_module_disable(PERIPH_SHA_MODULE);

    HMAC_RCC_ATOMIC() {
        hmac_ll_enable_bus_clock(false);
    }
}


static esp_err_t esp_ds_start_sign(const void *message, const esp_ds_data_t *data, hmac_key_id_t key_id)
{
    ds_acquire_enable();

    uint32_t conf_error = hmac_hal_configure(HMAC_OUTPUT_DS, key_id);
    if (conf_error) {
        ds_disable_release();
        return ESP_ERR_HW_CRYPTO_DS_HMAC_FAIL;
    }

    ds_hal_start();

    while (ds_hal_busy() != 0) { }

    size_t rsa_len = (data->rsa_length + 1) * 4;
    ds_hal_write_private_key_params(data->c);
    ds_hal_configure_iv((uint32_t *)data->iv);
    ds_hal_write_message(message, rsa_len);

    ds_hal_start_sign();

    return ESP_OK;
}

static esp_err_t esp_ds_finish_sign(void *signature, const esp_ds_data_t *data)
{
    unsigned rsa_len = (data->rsa_length + 1) * 4;

    while (ds_hal_busy()) { }

    ds_signature_check_t sig_check_result = ds_hal_read_result((uint8_t *) signature, (size_t) rsa_len);

    esp_err_t return_value = ESP_OK;

    if (sig_check_result == DS_SIGNATURE_MD_FAIL || sig_check_result == DS_SIGNATURE_PADDING_AND_MD_FAIL) {
        return_value = ESP_ERR_HW_CRYPTO_DS_INVALID_DIGEST;
    }

    if (sig_check_result == DS_SIGNATURE_PADDING_FAIL) {
        return_value = ESP_ERR_HW_CRYPTO_DS_INVALID_PADDING;
    }

    hmac_hal_clean();

    ds_disable_release();

    return return_value;
}

static esp_err_t esp_ds_sign(const void *message,
                      const esp_ds_data_t *data,
                      hmac_key_id_t key_id,
                      void *signature)
{
    esp_err_t result = esp_ds_start_sign(message, data, key_id);
    if (result != ESP_OK) {
        return result;
    }

    while (ds_hal_busy()) { }

    return esp_ds_finish_sign(signature, data);
}

static esp_err_t esp_ds_encrypt_params(esp_ds_data_t *data,
                                const void *iv,
                                const esp_ds_p_data_t *p_data,
                                const void *key)
{
    if (!p_data) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t result = ESP_OK;

    periph_module_enable(PERIPH_AES_MODULE);
    periph_module_enable(PERIPH_SHA_MODULE);

    ets_ds_data_t *ds_data = (ets_ds_data_t *) data;
    const ets_ds_p_data_t *ds_plain_data = (const ets_ds_p_data_t *) p_data;

    ets_ds_result_t ets_result = ets_ds_encrypt_params(ds_data, iv, ds_plain_data, key, ETS_DS_KEY_HMAC);

    if (ets_result == ETS_DS_INVALID_PARAM) {
        result = ESP_ERR_INVALID_ARG;
    }

    periph_module_disable(PERIPH_SHA_MODULE);
    periph_module_disable(PERIPH_AES_MODULE);

    return result;
}
#else /* !CONFIG_IDF_TARGET_ESP32S2 */

static void ds_acquire_enable(void)
{
    ets_hmac_enable();
    ets_ds_enable();
}

static void ds_disable_release(void)
{
    ets_ds_disable();
    ets_hmac_disable();
}

static esp_err_t esp_ds_start_sign(const void *message,
                            const esp_ds_data_t *data,
                            hmac_key_id_t key_id)
{
    ds_acquire_enable();

    int r = ets_hmac_calculate_downstream(ETS_EFUSE_BLOCK_KEY0 + (ets_efuse_block_t) key_id,
                                          ETS_EFUSE_KEY_PURPOSE_HMAC_DOWN_DIGITAL_SIGNATURE);
    if (r != ETS_OK) {
        ds_disable_release();
        return ESP_ERR_HW_CRYPTO_DS_HMAC_FAIL;
    }

    ets_ds_data_t *ds_data = (ets_ds_data_t *) data;

    ets_ds_result_t result = ets_ds_start_sign(message, ds_data);

    // ETS_DS_INVALID_PARAM only happens if a parameter is NULL or data->rsa_length is wrong
    // We checked all of that already
    assert(result != ETS_DS_INVALID_PARAM);

    if (result == ETS_DS_INVALID_KEY) {
        ds_disable_release();
        return ESP_ERR_HW_CRYPTO_DS_INVALID_KEY;
    }

    return ESP_OK;
}

esp_err_t esp_ds_finish_sign(void *signature, const esp_ds_data_t *data)
{
    ets_ds_result_t result = ets_ds_finish_sign(signature, (ets_ds_data_t*) data);

    esp_err_t return_value = ESP_OK;

    assert(result != ETS_DS_INVALID_PARAM);

    if (result == ETS_DS_INVALID_DIGEST) {
        return_value = ESP_ERR_HW_CRYPTO_DS_INVALID_DIGEST;
    }
    if (result == ETS_DS_INVALID_PADDING) {
        return_value = ESP_ERR_HW_CRYPTO_DS_INVALID_PADDING;
    }

    int res = ets_hmac_invalidate_downstream(ETS_EFUSE_KEY_PURPOSE_HMAC_DOWN_DIGITAL_SIGNATURE);
    assert(res == ETS_OK); // should not fail if called with correct purpose
    (void)res;

    ds_disable_release();

    return return_value;
}

static esp_err_t esp_ds_sign(const void *message,
                      const esp_ds_data_t *data,
                      hmac_key_id_t key_id,
                      void *signature)
{
    esp_err_t result = esp_ds_start_sign(message, data, key_id);
    if (result != ESP_OK) {
        return result;
    }

    while (ets_ds_is_busy()) { }

    return esp_ds_finish_sign(signature, (void *)data);
}

static esp_err_t esp_ds_encrypt_params(esp_ds_data_t *data,
                                const void *iv,
                                const esp_ds_p_data_t *p_data,
                                const void *key)
{
    assert(esp_ptr_internal(p_data) && esp_ptr_word_aligned(p_data));

    esp_err_t result = ESP_OK;

    ets_aes_enable();
    ets_sha_enable();

    ets_ds_data_t *ds_data = (ets_ds_data_t *) data;
    const ets_ds_p_data_t *ds_plain_data = (const ets_ds_p_data_t *) p_data;

    ets_ds_result_t ets_result = ets_ds_encrypt_params(ds_data, iv, ds_plain_data, key, ETS_DS_KEY_HMAC);

    if (ets_result == ETS_DS_INVALID_PARAM) {
        result = ESP_ERR_INVALID_ARG;
    }

    ets_sha_disable();
    ets_aes_disable();

    return result;
}

#endif /* !CONFIG_IDF_TARGET_ESP32S2 */

TEST_GROUP(ds);

TEST_SETUP(ds)
{
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(0, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
}

TEST_TEAR_DOWN(ds)
{
    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
                                         test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}

TEST(ds, digital_signature_parameter_encryption)
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

// This test uses the HMAC_KEY0 eFuse key which hasn't been burned by burn_hmac_keys().
// HMAC_KEY0 is usually used for HMAC upstream (user access) tests.
TEST(ds, digital_signature_wrong_hmac_key_purpose)
{
    esp_ds_data_t ds_data = {};
    ds_data.rsa_length =  RSA_LEN;
    const char *message = "test";

    // HMAC fails in that case because it checks for the correct purpose
    TEST_ASSERT_EQUAL(ESP_ERR_HW_CRYPTO_DS_HMAC_FAIL, esp_ds_start_sign(message, &ds_data, HMAC_KEY0));
}

// This test uses the HMAC_KEY0 eFuse key which hasn't been burned by burn_hmac_keys().
// HMAC_KEY0 is usually used for HMAC upstream (user access) tests.
TEST(ds, digital_signature_blocking_wrong_hmac_key_purpose)
{
    esp_ds_data_t ds_data = {};
    ds_data.rsa_length = RSA_LEN;
    const char *message = "test";
    uint8_t signature_data [128 * 4];

    // HMAC fails in that case because it checks for the correct purpose
    TEST_ASSERT_EQUAL(ESP_ERR_HW_CRYPTO_DS_HMAC_FAIL, esp_ds_sign(message, &ds_data, HMAC_KEY0, signature_data));
}

TEST(ds, digital_signature_operation)
{
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

            esp_err_t ds_r = esp_ds_start_sign(test_messages[j],
                                               &ds_data,
                                               t->hmac_key_idx + 1);
            TEST_ASSERT_EQUAL(ESP_OK, ds_r);

            ds_r = esp_ds_finish_sign(signature, &ds_data);
            TEST_ASSERT_EQUAL(ESP_OK, ds_r);

            TEST_ASSERT_EQUAL_HEX8_ARRAY(t->expected_results[j], signature, sizeof(signature));
        }
#if CONFIG_IDF_TARGET_ESP32C3
        ets_hmac_invalidate_downstream(ETS_EFUSE_KEY_PURPOSE_HMAC_DOWN_DIGITAL_SIGNATURE);
#endif
    }
}

TEST(ds, digital_signature_blocking_operation)
{
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

        esp_err_t ds_r = esp_ds_start_sign(test_messages[0],
                                           &ds_data,
                                           t->hmac_key_idx + 1);
        TEST_ASSERT_EQUAL(ESP_OK, ds_r);

        ds_r = esp_ds_finish_sign(signature, &ds_data);
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

TEST(ds, digital_signature_invalid_data)
{
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

        esp_err_t ds_r = esp_ds_start_sign(test_messages[0], &ds_data, t->hmac_key_idx + 1);
        TEST_ASSERT_EQUAL(ESP_OK, ds_r);

        ds_r = esp_ds_finish_sign(signature, &ds_data);
        TEST_ASSERT_EQUAL(ESP_ERR_HW_CRYPTO_DS_INVALID_DIGEST, ds_r);

        TEST_ASSERT_EQUAL_HEX8_ARRAY(zero, signature, DS_MAX_BITS / 8);
        ds_data.iv[bit / 8] ^= 1 << (bit % 8);
    }

    // Corrupt encrypted key data one bit at a time, rerun and expect failure
    printf("Corrupting C...\n");
    for (int bit = 0; bit < ETS_DS_C_LEN * 8; bit++) {
        printf("Corrupting C bit %d...\n", bit);
        ds_data.c[bit / 8] ^= 1 << (bit % 8);

        esp_err_t ds_r = esp_ds_start_sign(test_messages[0], &ds_data, t->hmac_key_idx + 1);
        TEST_ASSERT_EQUAL(ESP_OK, ds_r);

        ds_r = esp_ds_finish_sign(signature, &ds_data);
        TEST_ASSERT_EQUAL(ESP_ERR_HW_CRYPTO_DS_INVALID_DIGEST, ds_r);

        TEST_ASSERT_EQUAL_HEX8_ARRAY(zero, signature, DS_MAX_BITS / 8);

        ds_data.c[bit / 8] ^= 1 << (bit % 8);
    }
}

TEST_GROUP_RUNNER(ds)
{
    RUN_TEST_CASE(ds, digital_signature_parameter_encryption);
    RUN_TEST_CASE(ds, digital_signature_wrong_hmac_key_purpose);
    RUN_TEST_CASE(ds, digital_signature_blocking_wrong_hmac_key_purpose);
    RUN_TEST_CASE(ds, digital_signature_operation);
    RUN_TEST_CASE(ds, digital_signature_blocking_operation);
    RUN_TEST_CASE(ds, digital_signature_invalid_data);
}
