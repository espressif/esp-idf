/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_newlib.h"
#include "memory_checks.h"
#include "nvs_flash.h"
#include "psa/crypto.h"
#include "unity.h"
#include "test_persistent_format.h"

/* First ITS access caches the NVS psa_its namespace (and related one-shot
 * PSA storage state). Prime it before leak accounting so consume tests are
 * not charged ~1.2 KB against the 1200-byte critical threshold — the same
 * pattern mbedtls_ut uses for AES interrupt allocation. */
static void prime_psa_its(psa_key_id_t id)
{
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    (void)psa_get_key_attributes(id, &attr);
    psa_reset_key_attributes(&attr);
    (void)psa_purge_key(id);
}

void setUp(void)
{
    prime_psa_its(ESP_PERSISTENT_FIXTURE_DS_KEY_ID);
    prime_psa_its(ESP_PERSISTENT_FIXTURE_HMAC_KEY_ID);
    prime_psa_its(ESP_PERSISTENT_FIXTURE_ECDSA_KEY_ID);

    test_utils_record_free_mem();
    test_utils_set_leak_level(CONFIG_UNITY_CRITICAL_LEAK_LEVEL_GENERAL,
                              ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL);
    test_utils_set_leak_level(CONFIG_UNITY_WARN_LEAK_LEVEL_GENERAL,
                              ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_GENERAL);
}

void tearDown(void)
{
    vTaskDelay(5);
    esp_reent_cleanup();
    TEST_ASSERT_MESSAGE(heap_caps_check_integrity(MALLOC_CAP_INVALID, true),
                        "The test has corrupted the heap");
    test_utils_finish_and_evaluate_leaks(
        test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
        test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}

static void test_task(void *pvParameters)
{
    vTaskDelay(2);
    unity_run_menu();
}

void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    xTaskCreatePinnedToCore(test_task, "testTask",
                            CONFIG_UNITY_FREERTOS_STACK_SIZE, NULL,
                            CONFIG_UNITY_FREERTOS_PRIORITY, NULL,
                            CONFIG_UNITY_FREERTOS_CPU);
}
