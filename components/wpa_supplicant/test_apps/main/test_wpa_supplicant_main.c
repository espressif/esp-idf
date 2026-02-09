/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "esp_heap_caps.h"
#include "bignum_impl.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#if SOC_SHA_SUPPORT_PARALLEL_ENG
#include "sha/sha_parallel_engine.h"
#else
#include "sha/sha_core.h"
#endif

#include "psa/crypto.h"

#define TEST_MEMORY_LEAK_THRESHOLD_DEFAULT 0
static int leak_threshold = TEST_MEMORY_LEAK_THRESHOLD_DEFAULT;
void set_leak_threshold(int threshold)
{
    leak_threshold = -threshold;
}

static size_t before_free_8bit;
static size_t before_free_32bit;

static void check_leak(size_t before_free, size_t after_free, const char *type)
{
    ssize_t delta = after_free - before_free;
    printf("MALLOC_CAP_%s: Before %u bytes free, After %u bytes free (delta %d, threshold %d)\n", type, before_free, after_free, delta, leak_threshold);
    TEST_ASSERT_MESSAGE(delta > leak_threshold, "memory leak");
}

#if SOC_SHA_SUPPORT_SHA512
#define SHA_TYPE SHA2_512
#else
#define SHA_TYPE SHA2_256
#endif //SOC_SHA_SUPPORT_SHA512

void setUp(void)
{
#if CONFIG_MBEDTLS_HARDWARE_SHA
    // Execute esp_sha operation to allocate internal SHA semaphore (in case of ESP32)
    // and initial DMA setup memory which is considered as leaked otherwise
    const uint8_t input_buffer[64] = {0};
    uint8_t output_buffer[64];
#if SOC_SHA_SUPPORT_SHA512
    esp_sha(SHA2_512, input_buffer, sizeof(input_buffer), output_buffer);
#endif // SOC_SHA_SUPPORT_SHA512
#if SOC_SHA_SUPPORT_SHA256
    esp_sha(SHA2_256, input_buffer, sizeof(input_buffer), output_buffer);
#endif // SOC_SHA_SUPPORT_SHA256
#if SOC_SHA_SUPPORT_SHA1
    esp_sha(SHA1, input_buffer, sizeof(input_buffer), output_buffer);
#endif // SOC_SHA_SUPPORT_SHA1
#endif // CONFIG_MBEDTLS_HARDWARE_SHA

    // Execute mbedtls_aes_init operation to allocate AES interrupt
    // allocation memory which is considered as leak otherwise
    const uint8_t plaintext[16] = {0};
    uint8_t ciphertext[16];
    const uint8_t key[16] = { 0 };
    psa_status_t status;
    psa_key_id_t key_id = 0;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_ECB_NO_PADDING);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);
    status = psa_import_key(&attributes, key, sizeof(key), &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    size_t output_len = 0;
    status = psa_cipher_encrypt(key_id, PSA_ALG_ECB_NO_PADDING, plaintext, sizeof(plaintext), ciphertext, sizeof(ciphertext), &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    psa_destroy_key(key_id);

#if defined(CONFIG_MBEDTLS_HARDWARE_MPI)
    esp_mpi_enable_hardware_hw_op();
    esp_mpi_disable_hardware_hw_op();
#endif // CONFIG_MBEDTLS_HARDWARE_MPI

    before_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    before_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
}

void tearDown(void)
{
    size_t after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    check_leak(before_free_8bit, after_free_8bit, "8BIT");
    check_leak(before_free_32bit, after_free_32bit, "32BIT");

    leak_threshold = TEST_MEMORY_LEAK_THRESHOLD_DEFAULT;
}

void app_main(void)
{
    printf("Running wpa_supplicant component tests\n");
    unity_run_menu();
}
