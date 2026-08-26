/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "unity.h"
#include "memory_checks.h"
#include "psa/crypto.h"
#if SOC_SHA_SUPPORT_PARALLEL_ENG
#include "sha/sha_parallel_engine.h"
#else
#include "sha/sha_core.h"
#endif
#include "bignum_impl.h"

/* setUp runs before every test */
void setUp(void)
{
#if SOC_SHA_SUPPORTED
    // Execute esp_sha operation to allocate internal SHA semaphore (in case of ESP32)
    // and initial DMA setup memory which is considered as leaked otherwise
    const uint8_t input_buffer[64] = {0};
    uint8_t output_buffer[64];
#if SOC_SHA_SUPPORT_SHA1
    esp_sha(SHA1, input_buffer, sizeof(input_buffer), output_buffer);
#endif // SOC_SHA_SUPPORT_SHA1
#if SOC_SHA_SUPPORT_SHA256
    esp_sha(SHA2_256, input_buffer, sizeof(input_buffer), output_buffer);
#endif // SOC_SHA_SUPPORT_SHA256
#if SOC_SHA_SUPPORT_SHA512
    esp_sha(SHA2_512, input_buffer, sizeof(input_buffer), output_buffer);
#endif // SOC_SHA_SUPPORT_SHA512
#endif // SOC_SHA_SUPPORTED

#if defined(CONFIG_MBEDTLS_HARDWARE_MPI)
    esp_mpi_enable_hardware_hw_op();
    esp_mpi_disable_hardware_hw_op();
#endif // CONFIG_MBEDTLS_HARDWARE_MPI

#if SOC_AES_SUPPORTED
    // Execute mbedtls_aes_init operation to allocate AES interrupt
    // allocation memory which is considered as leak otherwise
    const uint8_t plaintext[16] = {0};
    uint8_t ciphertext[32];
    const uint8_t key[16] = { 0 };
    psa_status_t status;
    psa_key_id_t key_id = 0;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CBC_NO_PADDING);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);
    status = psa_import_key(&attributes, key, sizeof(key), &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    size_t output_len = 0;
    status = psa_cipher_encrypt(key_id, PSA_ALG_CBC_NO_PADDING, plaintext, sizeof(plaintext), ciphertext, sizeof(ciphertext), &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    const uint8_t plaintext_long[256] = {0};
    uint8_t ciphertext_long[272];
    output_len = 0;
    status = psa_cipher_encrypt(key_id, PSA_ALG_CBC_NO_PADDING, plaintext_long, sizeof(plaintext_long), ciphertext_long, sizeof(ciphertext_long), &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    psa_destroy_key(key_id);
#endif // SOC_AES_SUPPORTED
    test_utils_record_free_mem();
    test_utils_set_leak_level(CONFIG_UNITY_CRITICAL_LEAK_LEVEL_GENERAL, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL);
    test_utils_set_leak_level(CONFIG_UNITY_WARN_LEAK_LEVEL_GENERAL, ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_GENERAL);
    test_utils_set_leak_level(0, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_LWIP);
}

/* tearDown runs after every test */
void tearDown(void)
{
    /* some FreeRTOS stuff is cleaned up by idle task */
    vTaskDelay(5);

    /* clean up some of the newlib's lazy allocations */
    esp_reent_cleanup();

    /* check if unit test has caused heap corruption in any heap */
    TEST_ASSERT_MESSAGE(heap_caps_check_integrity(MALLOC_CAP_INVALID, true), "The test has corrupted the heap");

    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
                                         test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));

}

static void test_task(void *pvParameters)
{
    vTaskDelay(10); /* Delay a bit to let the main task be deleted */
    unity_run_menu();
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    xTaskCreatePinnedToCore(test_task, "testTask", CONFIG_UNITY_FREERTOS_STACK_SIZE, NULL, CONFIG_UNITY_FREERTOS_PRIORITY, NULL, CONFIG_UNITY_FREERTOS_CPU);
}
