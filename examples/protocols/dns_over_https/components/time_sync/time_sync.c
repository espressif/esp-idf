/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_netif_sntp.h"
#include "esp_timer.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"

#include "time_sync.h"

static const char *TAG = "time_sync";

#define STORAGE_NAMESPACE "storage"

/* Timer interval once every day (24 Hours) */
#define SECONDS_PER_HOUR 3600ULL
#define MICROSECONDS_PER_SECOND 1000000ULL
#define TIME_PERIOD ((uint64_t)CONFIG_TIME_SYNC_NTP_UPDATE_PERIOD * SECONDS_PER_HOUR * MICROSECONDS_PER_SECOND) // Convert hours to microseconds

/**
 * @brief Initialize SNTP service with predefined servers.
 *
 * This function sets up the SNTP service to sync time.
 */
void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG_MULTIPLE(1,
                                                                      ESP_SNTP_SERVER_LIST(CONFIG_TIME_SYNC_NTP_SERVER));
    esp_netif_sntp_init(&config);
}

/**
 * @brief Obtain system time from SNTP service.
 *
 * This function attempts to synchronize the system time using SNTP, retrying
 * up to 10 times if necessary. If the synchronization is successful, it returns
 * ESP_OK, otherwise ESP_FAIL.
 *
 * @return esp_err_t ESP_OK on success, ESP_FAIL on failure.
 */
static esp_err_t obtain_time(void)
{
    /* wait for time to be set */
    int retry = 0;
    const int retry_count = 10;
    while (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(2000)) != ESP_OK && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
    }
    if (retry == retry_count) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

/**
 * @brief Fetch the current system time and store it in NVS.
 *
 * This function initializes SNTP, retrieves the current system time, and stores
 * it in Non-Volatile Storage (NVS). In case of failure, an error message is logged.
 *
 * @param[in] args Unused argument placeholder.
 * @return esp_err_t ESP_OK on success, ESP_FAIL on failure.
 */
esp_err_t fetch_and_store_time_in_nvs(void *args)
{
    nvs_handle_t my_handle = 0;
    esp_err_t err;

    initialize_sntp();
    if (obtain_time() != ESP_OK) {
        err = ESP_FAIL;
        goto exit;
    }

    time_t now;
    time(&now);

    /* Open */
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        goto exit;
    }

    /* Write */
    err = nvs_set_i64(my_handle, "timestamp", now);
    if (err != ESP_OK) {
        goto exit;
    }

    err = nvs_commit(my_handle);
    if (err != ESP_OK) {
        goto exit;
    }

exit:
    if (my_handle != 0) {
        nvs_close(my_handle);
    }
    esp_netif_sntp_deinit();

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error updating time in nvs");
    } else {
        ESP_LOGI(TAG, "Updated time in NVS");
    }
    return err;
}

/**
 * @brief Update system time from NVS or SNTP if not available.
 *
 * This function retrieves the stored system time from NVS. If the time is not found,
 * it synchronizes with the SNTP server and updates the time in NVS.
 *
 * @return esp_err_t ESP_OK on success, ESP_FAIL on failure.
 */
esp_err_t update_time_from_nvs(void)
{
    nvs_handle_t my_handle = 0;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS");
        goto exit;
    }

    int64_t timestamp = 0;

    err = nvs_get_i64(my_handle, "timestamp", &timestamp);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "Time not found in NVS. Syncing time from SNTP server.");
        if (fetch_and_store_time_in_nvs(NULL) != ESP_OK) {
            err = ESP_FAIL;
        } else {
            err = ESP_OK;
        }
    } else if (err == ESP_OK) {
        struct timeval get_nvs_time;
        get_nvs_time.tv_sec = timestamp;
        settimeofday(&get_nvs_time, NULL);
    }

exit:
    if (my_handle != 0) {
        nvs_close(my_handle);
    }
    return err;
}

/**
 * @brief Sets up periodic time updates.
 *
 * If the reset reason is power-on, this function updates the system time from NVS.
 * It also creates a periodic timer to regularly fetch and store the time in NVS
 * using `fetch_and_store_time_in_nvs` as the callback.
 *
 * @return void
 */
void setup_periodic_time_updates(void)
{
    /* Check if the reset reason is power-on, and update time from NVS */
    if (esp_reset_reason() == ESP_RST_POWERON) {
        ESP_LOGI(TAG, "Updating time from NVS");
        ESP_ERROR_CHECK(update_time_from_nvs());
    }

    /* Set up a periodic timer to fetch and store the time in NVS */
    const esp_timer_create_args_t nvs_update_timer_args = {
        .callback = (void *) &fetch_and_store_time_in_nvs,
    };

    esp_timer_handle_t nvs_update_timer;
    ESP_ERROR_CHECK(esp_timer_create(&nvs_update_timer_args, &nvs_update_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(nvs_update_timer, TIME_PERIOD));
}
