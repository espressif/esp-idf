/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "test_utils.h"
#include "memory_checks.h"
#include "esp_newlib.h"
#include "nvs_flash.h"
#include "mbedtls/aes.h"
#if SOC_SHA_SUPPORT_PARALLEL_ENG
#include "sha/sha_parallel_engine.h"
#else
#include "sha/sha_block.h"
#endif
#include "bignum_impl.h"

/* setUp runs before every test */
void setUp(void)
{
#if CONFIG_MBEDTLS_HARDWARE_SHA
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
#endif // CONFIG_MBEDTLS_HARDWARE_SHA

#if defined(CONFIG_MBEDTLS_HARDWARE_MPI)
    esp_mpi_enable_hardware_hw_op();
    esp_mpi_disable_hardware_hw_op();
#endif // CONFIG_MBEDTLS_HARDWARE_MPI

#if SOC_AES_SUPPORTED
    // Execute mbedtls_aes_crypt_cbc operation to allocate AES interrupt
    // allocation memory which is considered as leak otherwise
    {
        mbedtls_aes_context aes_ctx;
        const uint8_t aes_key[16] = {0};
        uint8_t iv[16] = {0};
        const uint8_t plaintext[16] = {0};
        uint8_t ciphertext[16];

        mbedtls_aes_init(&aes_ctx);
        TEST_ASSERT_EQUAL(0, mbedtls_aes_setkey_enc(&aes_ctx, aes_key, 128));
        TEST_ASSERT_EQUAL(0, mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_ENCRYPT,
                          sizeof(plaintext), iv, plaintext, ciphertext));

        const uint8_t plaintext_long[256] = {0};
        uint8_t ciphertext_long[256];
        memset(iv, 0, sizeof(iv));
        TEST_ASSERT_EQUAL(0, mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_ENCRYPT,
                          sizeof(plaintext_long), iv, plaintext_long, ciphertext_long));
        mbedtls_aes_free(&aes_ctx);
    }
#endif // SOC_AES_SUPPORTED

    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(50, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
    TEST_ESP_OK(test_utils_set_leak_level(50, ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_GENERAL));
}

/* tearDown runs after every test */
void tearDown(void)
{
    /* some FreeRTOS stuff is cleaned up by idle task */
    vTaskDelay(5);

    /* clean up some of the newlib's lazy allocations */
    esp_reent_cleanup();

    /* check if unit test has caused heap corruption in any heap */
    TEST_ASSERT_MESSAGE( heap_caps_check_integrity(MALLOC_CAP_INVALID, true), "The test has corrupted the heap");

    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
            test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}

static void test_task(void *pvParameters)
{
    vTaskDelay(2); /* Delay a bit to let the main task be deleted */
    unity_run_menu();
}

void app_main(void)
{
    /* Initialize NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    xTaskCreatePinnedToCore(test_task, "testTask", CONFIG_UNITY_FREERTOS_STACK_SIZE, NULL, CONFIG_UNITY_FREERTOS_PRIORITY, NULL, CONFIG_UNITY_FREERTOS_CPU);
}
