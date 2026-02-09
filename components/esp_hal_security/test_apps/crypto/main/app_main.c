/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "unity.h"
#include "unity_fixture.h"
#include "unity_fixture_extras.h"

static void run_all_tests(void)
{
#if CONFIG_SOC_MPI_SUPPORTED
    RUN_TEST_GROUP(mpi);
#endif

#if CONFIG_SOC_ECC_SUPPORTED
    RUN_TEST_GROUP(ecc);
#endif

#if CONFIG_SOC_AES_SUPPORTED
    RUN_TEST_GROUP(aes);
#endif

#if CONFIG_SOC_SHA_SUPPORTED
#if !CONFIG_SOC_SHA_SUPPORT_PARALLEL_ENG
    RUN_TEST_GROUP(sha);
#endif /* !CONFIG_SOC_SHA_SUPPORT_PARALLEL_ENG*/
#endif

#if CONFIG_CRYPTO_IS_KEY_MANAGER_SUPPORTED
    RUN_TEST_GROUP(key_manager);
#endif

#if CONFIG_CRYPTO_TEST_APP_ENABLE_FPGA_TESTS

#if CONFIG_SOC_HMAC_SUPPORTED && CONFIG_CRYPTO_TEST_APP_ENABLE_HMAC_TESTS
    RUN_TEST_GROUP(hmac);
#endif

#if CONFIG_SOC_DIG_SIGN_SUPPORTED && CONFIG_CRYPTO_TEST_APP_ENABLE_DS_TESTS
    RUN_TEST_GROUP(ds);
#endif

#if CONFIG_SOC_ECDSA_SUPPORTED && CONFIG_CRYPTO_TEST_APP_ENABLE_ECDSA_TESTS
    RUN_TEST_GROUP(ecdsa)
#endif

#if CONFIG_SOC_FLASH_ENCRYPTION_XTS_AES && CONFIG_CRYPTO_TEST_APP_ENABLE_XTS_AES_TESTS
    RUN_TEST_GROUP(xts_aes)
#endif

#endif /* CONFIG_CRYPTO_TEST_APP_ENABLE_FPGA_TESTS */
}

static void test_task(void *pvParameters)
{
    vTaskDelay(2); /* Delay a bit to let the main task be deleted */
    UNITY_MAIN_FUNC(run_all_tests);
    vTaskDelete(NULL);
}

void app_main(void)
{
    xTaskCreatePinnedToCore(test_task, "testTask", CONFIG_UNITY_FREERTOS_STACK_SIZE, NULL, CONFIG_UNITY_FREERTOS_PRIORITY, NULL, CONFIG_UNITY_FREERTOS_CPU);
}
