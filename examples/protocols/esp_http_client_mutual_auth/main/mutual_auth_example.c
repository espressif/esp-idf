/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* ESP HTTP Client Mutual Authentication Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_http_client.h"

#if CONFIG_EXAMPLE_MUTUAL_AUTH_USE_HW_RSA_DS
#include "esp_secure_cert_read.h"
#if CONFIG_MBEDTLS_VER_4_X_SUPPORT
#include "psa_crypto_driver_esp_rsa_ds_contexts.h"
#else
#include "esp_rsa_sign_alt.h"
#endif
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "MUTUAL_AUTH";

#define URL_BUF_SIZE 256

/* CA cert for server verification (embedded in both modes) */
extern const char ca_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const char ca_cert_pem_end[]   asm("_binary_ca_cert_pem_end");

#if !CONFIG_EXAMPLE_MUTUAL_AUTH_USE_HW_RSA_DS
/* Software mode: client cert and key embedded as PEM */
extern const char client_cert_pem_start[] asm("_binary_client_cert_pem_start");
extern const char client_cert_pem_end[]   asm("_binary_client_cert_pem_end");
extern const char client_key_pem_start[]  asm("_binary_client_key_pem_start");
extern const char client_key_pem_end[]    asm("_binary_client_key_pem_end");
#endif

/**
 * Read the server URL from stdin. Used by pytest to inject the mTLS server address.
 * Returns ESP_OK on success, ESP_FAIL on error or empty input.
 */
static esp_err_t read_url_from_stdin(char *url_buf, size_t buf_size)
{
    example_configure_stdin_stdout();
    ESP_LOGI(TAG, "Enter mutual auth server URL:");

    if (fgets(url_buf, (int)buf_size, stdin) == NULL) {
        ESP_LOGE(TAG, "Failed to read URL from stdin");
        return ESP_FAIL;
    }
    /* Strip trailing newline */
    size_t len = strlen(url_buf);
    if (len > 0 && url_buf[len - 1] == '\n') {
        url_buf[--len] = '\0';
    }
    if (len == 0) {
        ESP_LOGE(TAG, "Empty URL");
        return ESP_FAIL;
    }
    return ESP_OK;
}

#if CONFIG_EXAMPLE_MUTUAL_AUTH_USE_HW_RSA_DS
/**
 * DS peripheral mode: load client cert and DS context from esp_secure_cert partition.
 */
static void mutual_auth_with_ds(const char *url)
{
    char *dev_cert_buf = NULL;
    uint32_t dev_cert_len = 0;
    esp_ds_data_ctx_t *ds_ctx = NULL;

    esp_err_t ret = esp_secure_cert_get_device_cert(&dev_cert_buf, &dev_cert_len);
    if (ret != ESP_OK || dev_cert_buf == NULL || dev_cert_len == 0) {
        ESP_LOGE(TAG, "Failed to get device cert: %s", esp_err_to_name(ret));
        return;
    }

    ds_ctx = esp_secure_cert_get_ds_ctx();
    if (ds_ctx == NULL) {
        ESP_LOGE(TAG, "Failed to get DS context");
        esp_secure_cert_free_device_cert(dev_cert_buf);
        return;
    }

    size_t ca_cert_len = ca_cert_pem_end - ca_cert_pem_start;
    esp_http_client_config_t config = {
        .url = url,
        .cert_pem = ca_cert_pem_start,
        .cert_len = ca_cert_len,
        .client_cert_pem = dev_cert_buf,
        .client_cert_len = dev_cert_len,
        .ds_data = ds_ctx,
        .skip_cert_common_name_check = true,
        .timeout_ms = 10000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to init HTTP client");
        goto cleanup;
    }

    esp_err_t err = esp_http_client_perform(client);
    int status = esp_http_client_get_status_code(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTPS Mutual Auth Status = %d, content_length = %" PRId64,
                 status, esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "Request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);

cleanup:
    esp_secure_cert_free_ds_ctx(ds_ctx);
    esp_secure_cert_free_device_cert(dev_cert_buf);
}

#else /* Software key mode */

/**
 * Software key mode: use embedded PEM client cert and key.
 */
static void mutual_auth_with_software_keys(const char *url)
{
    size_t ca_cert_len = ca_cert_pem_end - ca_cert_pem_start;
    size_t client_cert_len = client_cert_pem_end - client_cert_pem_start;
    size_t client_key_len = client_key_pem_end - client_key_pem_start;

    esp_http_client_config_t config = {
        .url = url,
        .cert_pem = ca_cert_pem_start,
        .cert_len = ca_cert_len,
        .client_cert_pem = client_cert_pem_start,
        .client_cert_len = client_cert_len,
        .client_key_pem = client_key_pem_start,
        .client_key_len = client_key_len,
        .skip_cert_common_name_check = true,
        .timeout_ms = 10000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to init HTTP client");
        return;
    }

    esp_err_t err = esp_http_client_perform(client);
    int status = esp_http_client_get_status_code(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTPS Mutual Auth Status = %d, content_length = %" PRId64,
                 status, esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "Request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}
#endif /* CONFIG_EXAMPLE_MUTUAL_AUTH_USE_HW_RSA_DS */

static void mutual_auth_task(void *pvParameters)
{
    char url_buf[URL_BUF_SIZE];
    if (read_url_from_stdin(url_buf, sizeof(url_buf)) != ESP_OK) {
        goto done;
    }

#if CONFIG_EXAMPLE_MUTUAL_AUTH_USE_HW_RSA_DS
    mutual_auth_with_ds(url_buf);
#else
    mutual_auth_with_software_keys(url_buf);
#endif

done:
    ESP_LOGI(TAG, "Finish mutual auth example");
    vTaskDelete(NULL);
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    ESP_LOGI(TAG, "Connected, starting mutual auth example");
    xTaskCreate(mutual_auth_task, "mutual_auth", 8192, NULL, 5, NULL);
}
