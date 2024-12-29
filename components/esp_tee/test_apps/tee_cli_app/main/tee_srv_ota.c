/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <inttypes.h>
#include <errno.h>

#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_app_format.h"
#include "esp_image_format.h"
#include "esp_flash_partitions.h"
#include "esp_partition.h"

#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_console.h"
#include "argtable3/argtable3.h"

#include "esp_tee_ota_ops.h"
#include "example_tee_srv.h"

#define BUF_SIZE 256

static const char *TAG = "ota_with_tee";

/* Semaphore governing the TEE and User app OTA processes; only one should be active at a time */
static SemaphoreHandle_t s_ota_mgmt;

static void http_cleanup(esp_http_client_handle_t client)
{
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
}

static void task_fatal_error(void)
{
    ESP_LOGE(TAG, "Exiting task due to fatal error...");
    (void)vTaskDelete(NULL);
}

static esp_err_t setup_task_conn(esp_http_client_config_t *config, const char *url)
{
    if (config == NULL || url == NULL) {
        ESP_LOGE(TAG, "Invalid arguments!");
        return ESP_ERR_INVALID_ARG;
    }

    /* Wait for the semaphore to be free for the taking */
    if (xSemaphoreTake(s_ota_mgmt, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGE(TAG, "Other OTA already in progress!");
        return ESP_FAIL;
    }

    config->url = url;
    config->timeout_ms = CONFIG_EXAMPLE_OTA_RECV_TIMEOUT;
    config->keep_alive_enable = true;
    config->crt_bundle_attach = esp_crt_bundle_attach;
#ifdef CONFIG_EXAMPLE_SKIP_COMMON_NAME_CHECK
    config->skip_cert_common_name_check = true;
#endif

    return ESP_OK;
}

static void tee_ota_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting TEE OTA...");

    esp_http_client_config_t config = {};
    if (setup_task_conn(&config, (const char *)pvParameter) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to setup OTA task");
        task_fatal_error();
    }

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialise HTTP connection");
        xSemaphoreGive(s_ota_mgmt);
        task_fatal_error();
    }

    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        xSemaphoreGive(s_ota_mgmt);
        task_fatal_error();
    }
    esp_http_client_fetch_headers(client);

    uint32_t curr_write_offset = 0;
    bool image_header_was_checked = false;    /* deal with all receive packet */
    char ota_write_data[BUF_SIZE + 1] = {0};  /* an ota data write buffer ready to write to the flash */

    while (1) {
        int data_read = esp_http_client_read(client, ota_write_data, BUF_SIZE);
        if (data_read < 0) {
            ESP_LOGE(TAG, "Error: SSL data read error");
            http_cleanup(client);
            xSemaphoreGive(s_ota_mgmt);
            task_fatal_error();
        } else if (data_read > 0) {
            if (image_header_was_checked == false) {
                /* TODO: TEE image header is missing the `esp_app_desc_t` configuration structure */
                if (data_read > sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t)) {
                    esp_image_header_t img_hdr;
                    memcpy(&img_hdr, ota_write_data, sizeof(esp_image_header_t));
                    if (img_hdr.chip_id != CONFIG_IDF_FIRMWARE_CHIP_ID) {
                        ESP_LOGE(TAG, "Mismatch chip id, expected %d, found %d", CONFIG_IDF_FIRMWARE_CHIP_ID, img_hdr.chip_id);
                        http_cleanup(client);
                        xSemaphoreGive(s_ota_mgmt);
                        task_fatal_error();
                    }
                    image_header_was_checked = true;

                    err = esp_tee_ota_begin();
                    if (err != ESP_OK) {
                        ESP_LOGE(TAG, "esp_tee_ota_begin failed (%s)", esp_err_to_name(err));
                        http_cleanup(client);
                        xSemaphoreGive(s_ota_mgmt);
                        task_fatal_error();
                    }
                    ESP_LOGI(TAG, "esp_tee_ota_begin succeeded");

                } else {
                    ESP_LOGE(TAG, "received package is not fit len");
                    http_cleanup(client);
                    xSemaphoreGive(s_ota_mgmt);
                    task_fatal_error();
                }
            }
            err = esp_tee_ota_write(curr_write_offset, (const void *)ota_write_data, data_read);
            if (err != ESP_OK) {
                http_cleanup(client);
                xSemaphoreGive(s_ota_mgmt);
                task_fatal_error();
            }
            curr_write_offset += data_read;
            memset(ota_write_data, 0x00, sizeof(ota_write_data));
            ESP_LOGD(TAG, "Written image length: %lu", curr_write_offset);
        } else if (data_read == 0) {
            /*
             * As esp_http_client_read never returns negative error code, we rely on
             * `errno` to check for underlying transport connectivity closure if any
             */
            if (errno == ECONNRESET || errno == ENOTCONN) {
                ESP_LOGE(TAG, "Connection closed, errno = %d", errno);
                break;
            }
            if (esp_http_client_is_complete_data_received(client) == true) {
                ESP_LOGI(TAG, "Connection closed");
                break;
            }
        }
    }

    ESP_LOGI(TAG, "esp_tee_ota_write succeeded");
    ESP_LOGI(TAG, "Total binary data written: %lu", curr_write_offset);

    if (esp_http_client_is_complete_data_received(client) != true) {
        ESP_LOGE(TAG, "Error in receiving complete file");
        http_cleanup(client);
        xSemaphoreGive(s_ota_mgmt);
        task_fatal_error();
    }

    err = esp_tee_ota_end();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_end failed (%s)!", esp_err_to_name(err));
        http_cleanup(client);
        xSemaphoreGive(s_ota_mgmt);
        task_fatal_error();
    }
    ESP_LOGI(TAG, "esp_tee_ota_end succeeded");

    /* Ending connection, freeing the semaphore */
    http_cleanup(client);
    xSemaphoreGive(s_ota_mgmt);

    ESP_LOGI(TAG, "Prepare to restart system!");
    esp_restart();
    return;
}

static void user_ota_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting User OTA task...");

    esp_http_client_config_t config = {};
    if (setup_task_conn(&config, (const char *)pvParameter) != 0) {
        ESP_LOGE(TAG, "Failed to setup OTA task");
        task_fatal_error();
    }

    esp_https_ota_config_t ota_config = {
        .http_config = &config,
    };
    ESP_LOGI(TAG, "Attempting to download update from %s", config.url);
    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "OTA Succeed, Rebooting...");
        esp_restart();
    } else {
        ESP_LOGE(TAG, "Firmware upgrade failed");
    }

    xSemaphoreGive(s_ota_mgmt);
    task_fatal_error();
}

static void init_ota_sem(void)
{
    static bool first_call = true;
    if (first_call) {
        s_ota_mgmt = xSemaphoreCreateBinary();
        xSemaphoreGive(s_ota_mgmt);
        first_call = false;
    }
}

static int create_ota_task(const char *url, const char *task_name, void (*ota_task)(void *))
{
    init_ota_sem();
    if (xTaskCreate(ota_task, task_name, configMINIMAL_STACK_SIZE * 3, (void *)url, 5, NULL) != pdPASS) {
        ESP_LOGE(TAG, "Task creation failed for %s", task_name);
        return ESP_FAIL;
    }

    return ESP_OK;
}

static struct {
    struct arg_str *url;
    struct arg_end *end;
} tee_ota_args;

static struct {
    struct arg_str *url;
    struct arg_end *end;
} user_ota_args;

static int tee_app_ota_task(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &tee_ota_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, tee_ota_args.end, argv[0]);
        return ESP_ERR_INVALID_ARG;
    }
    return create_ota_task(tee_ota_args.url->sval[0], "tee_ota_task", &tee_ota_task);
}

static int user_app_ota_task(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &user_ota_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, user_ota_args.end, argv[0]);
        return ESP_ERR_INVALID_ARG;
    }
    return create_ota_task(user_ota_args.url->sval[0], "user_ota_task", &user_ota_task);
}

void register_srv_tee_ota(void)
{
    tee_ota_args.url = arg_str1(NULL, NULL, "<url>", "URL for fetching the update");
    tee_ota_args.end = arg_end(2);

    const esp_console_cmd_t tee_ota_cmd = {
        .command = "tee_ota",
        .help = "Initiate TEE app OTA",
        .hint = NULL,
        .func = &tee_app_ota_task,
        .argtable = &tee_ota_args,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&tee_ota_cmd));
}

void register_srv_user_ota(void)
{
    user_ota_args.url = arg_str1(NULL, NULL, "<url>", "URL for fetching the update");
    user_ota_args.end = arg_end(2);

    const esp_console_cmd_t user_ota_cmd = {
        .command = "user_ota",
        .help = "Initiate User app OTA",
        .hint = NULL,
        .func = &user_app_ota_task,
        .argtable = &user_ota_args,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&user_ota_cmd));
}
