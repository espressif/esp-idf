#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_types.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_timer.h"
#include "soc/cpu.h"
#include "unity.h"
#include "test_utils.h"
#include "esp32/rom/uart.h"
#include "esp32/rom/sha.h"
#include "soc/uart_periph.h"
#include "soc/dport_reg.h"
#include "soc/rtc.h"
#include "esp_log.h"
#include "mbedtls/sha256.h"
#include "esp32/sha.h"
#include "esp32/aes.h"
#include "mbedtls/rsa.h"

static const char *TAG = "test";
static volatile bool exit_flag = false;
#define TASK_STACK_SIZE (8*1024)

static void aes_task(void *pvParameters)
{
    xSemaphoreHandle *sema = (xSemaphoreHandle *) pvParameters;
    ESP_LOGI(TAG, "aes_task is started");
    esp_aes_context ctx = {
            .key_bytes = 16,
            .key = {101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116}
    };
    const unsigned char input[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    unsigned char output[16];
    unsigned char output2[16];
    while (exit_flag == false) {
        memset(output, 0, sizeof(output));
        memset(output, 0, sizeof(output2));
        esp_internal_aes_encrypt(&ctx, input, output);
        esp_internal_aes_decrypt(&ctx, output, output2);
        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(input, output2, sizeof(input), "AES must match");
    }
    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

static void sha_task(void *pvParameters)
{
    xSemaphoreHandle *sema = (xSemaphoreHandle *) pvParameters;
    ESP_LOGI(TAG, "sha_task is started");
    const char *input = "Space!#$%&()*+,-.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_abcdefghijklmnopqrstuvwxyz~DEL0123456789";
    unsigned char output[64];
    unsigned char output_origin[64];
    esp_sha(SHA2_512, (const unsigned char *)input, sizeof(input), output);
    memcpy(output_origin, output, sizeof(output));
    while (exit_flag == false) {
        memset(output, 0, sizeof(output));
        esp_sha(SHA2_512, (const unsigned char *)input, sizeof(input), output);
        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(output, output_origin, sizeof(output), "SHA256 must match");
    }
    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

static void mbedtls_sha256_task(void *pvParameters)
{
    xSemaphoreHandle *sema = (xSemaphoreHandle *) pvParameters;
    ESP_LOGI(TAG, "mbedtls_sha256_task is started");
    const char *input = "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_abcdefghijklmnopqrstuvwxyz~DEL0123456789Space!#$%&()*+,-.0123456789:;<=>?";
    mbedtls_sha256_context sha256_ctx;
    unsigned char output[32];
    unsigned char output_origin[32];

    mbedtls_sha256_init(&sha256_ctx);
    memset(output, 0, sizeof(output));
    mbedtls_sha256_starts_ret(&sha256_ctx, false);
    for (int i = 0; i < 3; ++i) {
        mbedtls_sha256_update_ret(&sha256_ctx, (unsigned char *)input, 100);
    }
    mbedtls_sha256_finish_ret(&sha256_ctx, output);
    memcpy(output_origin, output, sizeof(output));

    while (exit_flag == false) {
        mbedtls_sha256_init(&sha256_ctx);
        memset(output, 0, sizeof(output));
        mbedtls_sha256_starts_ret(&sha256_ctx, false);
        for (int i = 0; i < 3; ++i) {
            mbedtls_sha256_update_ret(&sha256_ctx, (unsigned char *)input, 100);
        }
        mbedtls_sha256_finish_ret(&sha256_ctx, output);

        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(output, output_origin, sizeof(output), "MBEDTLS SHA256 must match");
    }
    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

TEST_CASE("Test shared using AES SHA512 SHA256", "[hw_crypto]")
{
#ifndef CONFIG_FREERTOS_UNICORE
    const int max_tasks = 6;
#else
    const int max_tasks = 3;
#endif
    xSemaphoreHandle exit_sema[max_tasks];

    for (int i = 0; i < max_tasks; ++i) {
        exit_sema[i] = xSemaphoreCreateBinary();
    }
    exit_flag = false;
#ifndef CONFIG_FREERTOS_UNICORE
    xTaskCreatePinnedToCore(&aes_task,            "aes_task",            TASK_STACK_SIZE, &exit_sema[0], UNITY_FREERTOS_PRIORITY - 1, NULL, 1);
    xTaskCreatePinnedToCore(&aes_task,            "aes_task",            TASK_STACK_SIZE, &exit_sema[1], UNITY_FREERTOS_PRIORITY - 1, NULL, 0);
    xTaskCreatePinnedToCore(&sha_task,            "sha_task",            TASK_STACK_SIZE, &exit_sema[2], UNITY_FREERTOS_PRIORITY - 1, NULL, 1);
    xTaskCreatePinnedToCore(&sha_task,            "sha_task",            TASK_STACK_SIZE, &exit_sema[3], UNITY_FREERTOS_PRIORITY - 1, NULL, 0);
    xTaskCreatePinnedToCore(&mbedtls_sha256_task, "mbedtls_sha256_task", TASK_STACK_SIZE, &exit_sema[4], UNITY_FREERTOS_PRIORITY - 1, NULL, 1);
    xTaskCreatePinnedToCore(&mbedtls_sha256_task, "mbedtls_sha256_task", TASK_STACK_SIZE, &exit_sema[5], UNITY_FREERTOS_PRIORITY - 1, NULL, 0);
#else
    xTaskCreate(&aes_task,            "aes_task",            TASK_STACK_SIZE, &exit_sema[0], UNITY_FREERTOS_PRIORITY - 1, NULL);
    xTaskCreate(&sha_task,            "sha_task",            TASK_STACK_SIZE, &exit_sema[1], UNITY_FREERTOS_PRIORITY - 1, NULL);
    xTaskCreate(&mbedtls_sha256_task, "mbedtls_sha256_task", TASK_STACK_SIZE, &exit_sema[2], UNITY_FREERTOS_PRIORITY - 1, NULL);
#endif

    ESP_LOGI(TAG, "Waiting for 10s ...");
    vTaskDelay(10000 / portTICK_PERIOD_MS);

    // set exit flag to let thread exit
    exit_flag = true;
    for (int i = 0; i < max_tasks; ++i) {
        if (!xSemaphoreTake(exit_sema[i], 2000/portTICK_PERIOD_MS)) {
            TEST_FAIL_MESSAGE("exit_sema not released by test task");
        }
        vSemaphoreDelete(exit_sema[i]);
    }
}

static void rsa_task(void *pvParameters)
{
    xSemaphoreHandle *sema = (xSemaphoreHandle *) pvParameters;
    ESP_LOGI(TAG, "rsa_task is started");
    while (exit_flag == false) {
        mbedtls_rsa_self_test(0);
    }
    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

TEST_CASE("Test shared using AES RSA", "[hw_crypto]")
{
#ifndef CONFIG_FREERTOS_UNICORE
    const int max_tasks = 2;
#else
    const int max_tasks = 2;
#endif
    xSemaphoreHandle exit_sema[max_tasks];

    for (int i = 0; i < max_tasks; ++i) {
        exit_sema[i] = xSemaphoreCreateBinary();
    }
    exit_flag = false;
#ifndef CONFIG_FREERTOS_UNICORE
    xTaskCreatePinnedToCore(&aes_task, "aes_task", TASK_STACK_SIZE, &exit_sema[0], UNITY_FREERTOS_PRIORITY - 1, NULL, 1);
    xTaskCreatePinnedToCore(&rsa_task, "rsa_task", TASK_STACK_SIZE, &exit_sema[1], UNITY_FREERTOS_PRIORITY - 1, NULL, 0);
#else
    xTaskCreate(&aes_task, "aes_task", TASK_STACK_SIZE, &exit_sema[0], UNITY_FREERTOS_PRIORITY - 1, NULL);
    xTaskCreate(&rsa_task, "rsa_task", TASK_STACK_SIZE, &exit_sema[1], UNITY_FREERTOS_PRIORITY - 1, NULL);
#endif

    ESP_LOGI(TAG, "Waiting for 10s ...");
    vTaskDelay(10000 / portTICK_PERIOD_MS);

    // set exit flag to let thread exit
    exit_flag = true;
    for (int i = 0; i < max_tasks; ++i) {
        if (!xSemaphoreTake(exit_sema[i], 2000/portTICK_PERIOD_MS)) {
            TEST_FAIL_MESSAGE("exit_sema not released by test task");
        }
        vSemaphoreDelete(exit_sema[i]);
    }
}

TEST_CASE("Test shared using SHA512 RSA", "[hw_crypto]")
{
#ifndef CONFIG_FREERTOS_UNICORE
    const int max_tasks = 2;
#else
    const int max_tasks = 2;
#endif
    xSemaphoreHandle exit_sema[max_tasks];

    for (int i = 0; i < max_tasks; ++i) {
        exit_sema[i] = xSemaphoreCreateBinary();
    }
    exit_flag = false;
#ifndef CONFIG_FREERTOS_UNICORE
    xTaskCreatePinnedToCore(&sha_task, "sha_task", TASK_STACK_SIZE, &exit_sema[0], UNITY_FREERTOS_PRIORITY - 2, NULL, 1);
    xTaskCreatePinnedToCore(&rsa_task, "rsa_task", TASK_STACK_SIZE, &exit_sema[1], UNITY_FREERTOS_PRIORITY - 1, NULL, 0);
#else
    xTaskCreate(&sha_task, "sha_task", TASK_STACK_SIZE, &exit_sema[0], UNITY_FREERTOS_PRIORITY - 1, NULL);
    xTaskCreate(&rsa_task, "rsa_task", TASK_STACK_SIZE, &exit_sema[1], UNITY_FREERTOS_PRIORITY - 1, NULL);
#endif

    ESP_LOGI(TAG, "Waiting for 10s ...");
    vTaskDelay(10000 / portTICK_PERIOD_MS);

    // set exit flag to let thread exit
    exit_flag = true;
    for (int i = 0; i < max_tasks; ++i) {
        if (!xSemaphoreTake(exit_sema[i], 2000/portTICK_PERIOD_MS)) {
            TEST_FAIL_MESSAGE("exit_sema not released by test task");
        }
        vSemaphoreDelete(exit_sema[i]);
    }
}

TEST_CASE("Test shared using SHA256 RSA", "[hw_crypto]")
{
#ifndef CONFIG_FREERTOS_UNICORE
    const int max_tasks = 2;
#else
    const int max_tasks = 2;
#endif
    xSemaphoreHandle exit_sema[max_tasks];

    for (int i = 0; i < max_tasks; ++i) {
        exit_sema[i] = xSemaphoreCreateBinary();
    }
    exit_flag = false;
#ifndef CONFIG_FREERTOS_UNICORE
    xTaskCreatePinnedToCore(&mbedtls_sha256_task, "mbedtls_sha256_task", TASK_STACK_SIZE, &exit_sema[0], UNITY_FREERTOS_PRIORITY - 1, NULL, 1);
    xTaskCreatePinnedToCore(&rsa_task, "rsa_task", TASK_STACK_SIZE, &exit_sema[1], UNITY_FREERTOS_PRIORITY - 1, NULL, 0);
#else
    xTaskCreate(&mbedtls_sha256_task, "mbedtls_sha256_task", TASK_STACK_SIZE, &exit_sema[0], UNITY_FREERTOS_PRIORITY - 1, NULL);
    xTaskCreate(&rsa_task,            "rsa_task",            TASK_STACK_SIZE, &exit_sema[1], UNITY_FREERTOS_PRIORITY - 1, NULL);
#endif

    ESP_LOGI(TAG, "Waiting for 10s ...");
    vTaskDelay(10000 / portTICK_PERIOD_MS);

    // set exit flag to let thread exit
    exit_flag = true;
    for (int i = 0; i < max_tasks; ++i) {
        if (!xSemaphoreTake(exit_sema[i], 2000/portTICK_PERIOD_MS)) {
            TEST_FAIL_MESSAGE("exit_sema not released by test task");
        }
        vSemaphoreDelete(exit_sema[i]);
    }
}

TEST_CASE("Test shared using AES SHA RSA", "[hw_crypto]")
{
#ifndef CONFIG_FREERTOS_UNICORE
    const int max_tasks = 3;
#else
    const int max_tasks = 3;
#endif
    xSemaphoreHandle exit_sema[max_tasks];

    for (int i = 0; i < max_tasks; ++i) {
        exit_sema[i] = xSemaphoreCreateBinary();
    }
    exit_flag = false;
#ifndef CONFIG_FREERTOS_UNICORE
    xTaskCreatePinnedToCore(&aes_task, "aes_task", TASK_STACK_SIZE, &exit_sema[0], UNITY_FREERTOS_PRIORITY - 1, NULL, 0);
    xTaskCreatePinnedToCore(&sha_task, "sha_task", TASK_STACK_SIZE, &exit_sema[1], UNITY_FREERTOS_PRIORITY - 1, NULL, 0);
    xTaskCreatePinnedToCore(&rsa_task, "rsa_task", TASK_STACK_SIZE, &exit_sema[2], UNITY_FREERTOS_PRIORITY - 1, NULL, 1);
#else
    xTaskCreate(&aes_task, "aes_task", TASK_STACK_SIZE, &exit_sema[0], UNITY_FREERTOS_PRIORITY - 1, NULL);
    xTaskCreate(&sha_task, "sha_task", TASK_STACK_SIZE, &exit_sema[1], UNITY_FREERTOS_PRIORITY - 1, NULL);
    xTaskCreate(&rsa_task, "rsa_task", TASK_STACK_SIZE, &exit_sema[2], UNITY_FREERTOS_PRIORITY - 1, NULL);
#endif

    ESP_LOGI(TAG, "Waiting for 10s ...");
    vTaskDelay(10000 / portTICK_PERIOD_MS);

    // set exit flag to let thread exit
    exit_flag = true;
    for (int i = 0; i < max_tasks; ++i) {
        if (!xSemaphoreTake(exit_sema[i], 2000/portTICK_PERIOD_MS)) {
            TEST_FAIL_MESSAGE("exit_sema not released by test task");
        }
        vSemaphoreDelete(exit_sema[i]);
    }
}
