/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
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
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_sntp.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "time_sync.h"

static const char *TAG = "time_sync";

#define STORAGE_NAMESPACE "storage"

void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
#endif
    sntp_init();
}

static void obtain_time(void)
{
    /**
     * NTP server address could be aquired via DHCP,
     * see LWIP_DHCP_GET_NTP_SRV menuconfig option
     */
#ifdef LWIP_DHCP_GET_NTP_SRV
    sntp_servermode_dhcp(1);
#endif

    // wait for time to be set
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void fetch_and_store_time_in_nvs(void *args)
{
    initialize_sntp();
    obtain_time();

    nvs_handle_t my_handle;
    esp_err_t err;

    time_t now;
    time(&now);

    //Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        goto exit;
    }

    //Write
    err = nvs_set_i64(my_handle, "timestamp", now);
    if (err != ESP_OK) {
        goto exit;
    }

    err = nvs_commit(my_handle);
    if (err != ESP_OK) {
        goto exit;
    }

    nvs_close(my_handle);
    sntp_stop();

exit:
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error updating time in nvs");
    } else {
        ESP_LOGI(TAG, "Updated time in NVS");
    }
}

esp_err_t update_time_from_nvs(void)
{
    nvs_handle_t my_handle;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS");
        goto exit;
    }

    int64_t timestamp = 0;

    err = nvs_get_i64(my_handle, "timestamp", &timestamp);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        fetch_and_store_time_in_nvs(NULL);
        err = ESP_OK;
    } else if (err == ESP_OK) {
        struct timeval get_nvs_time;
        get_nvs_time.tv_sec = timestamp;
        settimeofday(&get_nvs_time, NULL);
    }

exit:
    nvs_close(my_handle);
    return err;
}
