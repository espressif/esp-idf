/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp_err.h"
#include "esp_check.h"
#include "esp_macros.h"
#include "esp_log.h"
#include "unity.h"

static const char *TAG = "TEST";

static void close_device(void)
{
    ESP_LOGI(TAG, "Closing device");
}

static void free_buffer(void)
{
    ESP_LOGI(TAG, "Freeing buffer");
}

static int where_to_fail = 0;

static esp_err_t test_esp_checks(esp_err_t error)
{
    if (where_to_fail == 0) {
        where_to_fail++;
        ESP_RETURN_ON_ERROR_CLEANUP(error);
    }

    if (where_to_fail == 1) {
        where_to_fail++;
        ESP_RETURN_ON_ERROR_CLEANUP(error, close_device());
    }

    if (where_to_fail == 2) {
        where_to_fail++;
        ESP_RETURN_ON_ERROR_CLEANUP(
            error,
            free_buffer(),
            printf("Failed to initialize sensor: %s\n", esp_err_to_name(err_rc_))
        );
    }

    if (where_to_fail == 3) {
        where_to_fail++;
        ESP_RETURN_ON_ERROR_CLEANUP(
            error,
        do {
            if (err_rc_ == ESP_ERR_NOT_FINISHED) {
                ESP_LOGE(TAG, "Sensor in invalid state during calibration %s", esp_err_to_name(err_rc_));
            }
            free_buffer();
            close_device();
            ESP_LOGE(TAG, "Drop connection");
        } while (0)
        );
    }
    return ESP_OK;
}

TEST_CASE("Tests of example ESP_RETURN_ON_ERROR_CLEANUP", "[esp_check]")
{
    where_to_fail = 0;
    TEST_ESP_ERR(test_esp_checks(ESP_FAIL), ESP_FAIL);
    TEST_ESP_ERR(test_esp_checks(ESP_ERR_NOT_SUPPORTED), ESP_ERR_NOT_SUPPORTED);
    TEST_ESP_ERR(test_esp_checks(ESP_ERR_NO_MEM), ESP_ERR_NO_MEM);
    TEST_ESP_ERR(test_esp_checks(ESP_ERR_NOT_FINISHED), ESP_ERR_NOT_FINISHED);
    TEST_ESP_ERR(test_esp_checks(ESP_FAIL), ESP_OK);
}
