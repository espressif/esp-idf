/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "test_utils.h"
#include "freertos/event_groups.h"
#include "unity_test_utils.h"

#define EMPH_STR(s) "****** "s" ******"


static const char* TAG = "test_wifi_init";

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    printf("wifi event handle called.\n");
    switch(event_id) {
        case WIFI_EVENT_AP_START:
            ESP_LOGI(TAG, "WIFI_EVENT_AP_START");
            break;
        case WIFI_EVENT_STA_START:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
            break;
        default:
            break;
    }
    return;
}

static esp_err_t event_init(void)
{
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    return ESP_OK;
}

static esp_err_t event_deinit(void)
{
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT,ESP_EVENT_ANY_ID,&wifi_event_handler));
    ESP_ERROR_CHECK(esp_event_loop_delete_default());
    return ESP_OK;
}

static void wifi_driver_can_start_on_APP_CPU_task(void* arg)
{
    SemaphoreHandle_t *sema = (SemaphoreHandle_t *) arg;
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_LOGI(TAG, EMPH_STR("event_init"));
    TEST_ESP_OK(event_init());
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_init"));
    TEST_ESP_OK(esp_wifi_init(&cfg));
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_deinit..."));
    TEST_ESP_OK(esp_wifi_deinit());
    ESP_LOGI(TAG, EMPH_STR("event_deinit"));
    TEST_ESP_OK(event_deinit());
    ESP_LOGI(TAG, "exit task...");
    xSemaphoreGive(*sema);
    vTaskSuspend(NULL); // wait to be deleted
}

TEST_CASE("wifi driver can start on APP CPU", "[wifi_init]")
{
    TaskHandle_t th = NULL;
    SemaphoreHandle_t sema = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(sema);
    printf("Creating tasks\n");
#ifndef CONFIG_FREERTOS_UNICORE
    xTaskCreatePinnedToCore(wifi_driver_can_start_on_APP_CPU_task, "wifi_driver_can_start_on_APP_CPU_task", 2048*2, &sema, 3, &th, 1);
#else
    xTaskCreate(wifi_driver_can_start_on_APP_CPU_task, "wifi_driver_can_start_on_APP_CPU_task", 2048*2, &sema, 3, &th);
#endif
    TEST_ASSERT_NOT_NULL(th);
    xSemaphoreTake(sema, portMAX_DELAY);
    vSemaphoreDelete(sema);
    sema = NULL;
    unity_utils_task_delete(th);
}

TEST_CASE("Calling esp_wifi_stop() with start", "[wifi_init]")
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_LOGI(TAG, EMPH_STR("event_init"));
    TEST_ESP_OK(event_init());
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_init"));
    TEST_ESP_OK(esp_wifi_init(&cfg));
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_start"));
    TEST_ESP_OK(esp_wifi_start());
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_stop"));
    TEST_ESP_OK(esp_wifi_stop());
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_deinit"));
    TEST_ESP_OK(esp_wifi_deinit());
    ESP_LOGI(TAG, EMPH_STR("event_deinit"));
    TEST_ESP_OK(event_deinit());
    ESP_LOGI(TAG, "test passed...");
}

TEST_CASE("Calling esp_wifi_stop() without start", "[wifi_init]")
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_LOGI(TAG, EMPH_STR("event_init"));
    TEST_ESP_OK(event_init());
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_init"));
    TEST_ESP_OK(esp_wifi_init(&cfg));
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_stop"));
    TEST_ESP_OK(esp_wifi_stop());
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_deinit"));
    TEST_ESP_OK(esp_wifi_deinit());
    ESP_LOGI(TAG, EMPH_STR("event_deinit"));
    TEST_ESP_OK(event_deinit());
    ESP_LOGI(TAG, "test passed...");
}

TEST_CASE("Calling esp_wifi_deinit() without stop", "[wifi_init]")
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_LOGI(TAG, EMPH_STR("event_init"));
    TEST_ESP_OK(event_init());
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_init"));
    TEST_ESP_OK(esp_wifi_init(&cfg));
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_start"));
    TEST_ESP_OK(esp_wifi_start());
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_deinit"));
    TEST_ESP_ERR(ESP_ERR_WIFI_NOT_STOPPED, esp_wifi_deinit());
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_stop"));
    TEST_ESP_OK(esp_wifi_stop());
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_deinit"));
    TEST_ESP_OK(esp_wifi_deinit());
    ESP_LOGI(TAG, EMPH_STR("event_deinit"));
    TEST_ESP_OK(event_deinit());
    ESP_LOGI(TAG, "test passed...");
}

TEST_CASE("Calling esp_wifi_connect() without start", "[wifi_init]")
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_LOGI(TAG, EMPH_STR("event_init"));
    TEST_ESP_OK(event_init());
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_init"));
    TEST_ESP_OK(esp_wifi_init(&cfg));
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_connect"));
    TEST_ESP_ERR(ESP_ERR_WIFI_NOT_STARTED, esp_wifi_connect());
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_deinit"));
    TEST_ESP_OK(esp_wifi_deinit());
    ESP_LOGI(TAG, EMPH_STR("event_deinit"));
    TEST_ESP_OK(event_deinit());
    ESP_LOGI(TAG, "test passed...");
}

TEST_CASE("Calling esp_wifi_connect() without init", "[wifi_init]")
{
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_connect"));
    TEST_ESP_ERR(ESP_ERR_WIFI_NOT_INIT, esp_wifi_connect());
    ESP_LOGI(TAG, "test passed...");
}
