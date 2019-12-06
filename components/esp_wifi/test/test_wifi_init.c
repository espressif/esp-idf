#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "test_utils.h"
#include "freertos/event_groups.h"

#define GOT_IP_EVENT        0x00000001
#define DISCONNECT_EVENT    0x00000002
#define EVENT_HANDLER_FLAG_DO_NOT_AUTO_RECONNECT 0x00000001
#define EMPH_STR(s) "****** "s" ******"

static const char* TAG = "test_wifi_init";
static uint32_t wifi_event_handler_flag;
static EventGroupHandle_t wifi_events;

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    printf("wifi ev_handle_called.\n");
    switch(event_id) {
        case WIFI_EVENT_STA_START:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
            if (! (EVENT_HANDLER_FLAG_DO_NOT_AUTO_RECONNECT & wifi_event_handler_flag) ) {
                TEST_ESP_OK(esp_wifi_connect());
            }
            if (wifi_events) {
                xEventGroupSetBits(wifi_events, DISCONNECT_EVENT);
            }
            break;
        default:
            break;
    }
    return;
}

static void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    ip_event_got_ip_t *event;
    printf("ip ev_handle_called.\n");
    switch(event_id) {
        case IP_EVENT_STA_GOT_IP:
            event = (ip_event_got_ip_t*)event_data;
            ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
            ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
            if (wifi_events) {
                xEventGroupSetBits(wifi_events, GOT_IP_EVENT);
            }
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
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &ip_event_handler, NULL));
    return ESP_OK;
}

static void wifi_driver_can_start_on_APP_CPU_task(void* arg)
{
    SemaphoreHandle_t *sema = (SemaphoreHandle_t *) arg;
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    //init nvs
    ESP_LOGI(TAG, EMPH_STR("nvs_flash_init"));
    esp_err_t r = nvs_flash_init();
    if (r == ESP_ERR_NVS_NO_FREE_PAGES || r == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGI(TAG, EMPH_STR("no free pages or NFS version mismatch, erase.."));
        TEST_ESP_OK(nvs_flash_erase());
        r = nvs_flash_init();
    }
    TEST_ESP_OK(r);
    //init event loop
    ESP_LOGI(TAG, EMPH_STR("event_init"));
    event_init();
    unity_reset_leak_checks();
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_init"));
    TEST_ESP_OK(esp_wifi_init(&cfg));
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_deinit..."));
    TEST_ESP_OK(esp_wifi_deinit());
    ESP_LOGI(TAG, EMPH_STR("nvs_flash_deinit..."));
    nvs_flash_deinit();
    ESP_LOGI(TAG, "test passed...");
    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

TEST_CASE("wifi driver can start on APP CPU", "[wifi_init]")
{
    TaskHandle_t th;
    SemaphoreHandle_t sema = xSemaphoreCreateBinary();
    printf("Creating tasks\n");
#ifndef CONFIG_FREERTOS_UNICORE
    xTaskCreatePinnedToCore(wifi_driver_can_start_on_APP_CPU_task, "wifi_driver_can_start_on_APP_CPU_task", 2048*2, &sema, 3, &th, 1);
#else
    xTaskCreate(wifi_driver_can_start_on_APP_CPU_task, "wifi_driver_can_start_on_APP_CPU_task", 2048*2, &sema, 3, &th);
#endif
    xSemaphoreTake(sema, portMAX_DELAY);
    vSemaphoreDelete(sema);
    sema = NULL;
    TEST_IGNORE_MESSAGE("this test case is ignored due to the event_loop.");
}

static void wifi_start_stop_task(void* arg)
{
    SemaphoreHandle_t *sema = (SemaphoreHandle_t *) arg;
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    //init nvs
    ESP_LOGI(TAG, EMPH_STR("nvs_flash_init"));
    esp_err_t r = nvs_flash_init();
    if (r == ESP_ERR_NVS_NO_FREE_PAGES || r == ESP_ERR_NVS_NEW_VERSION_FOUND) {
	    ESP_LOGI(TAG, EMPH_STR("no free pages or NFS version mismatch, erase.."));
	    TEST_ESP_OK(nvs_flash_erase());
	    r = nvs_flash_init();
    }
    TEST_ESP_OK(r);
    //init tcpip stack
    ESP_LOGI(TAG, EMPH_STR("esp_netif_init"));
    esp_netif_init();
    //init event loop
    ESP_LOGI(TAG, EMPH_STR("event_init"));
    event_init();
    unity_reset_leak_checks();
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_init"));
    TEST_ASSERT(esp_wifi_init(&cfg) == ESP_OK);
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_start"));
    TEST_ASSERT(esp_wifi_start() == ESP_OK);
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_stop"));
    TEST_ASSERT(esp_wifi_stop() == ESP_OK);
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_stop"));
    TEST_ASSERT(esp_wifi_stop() == ESP_OK);
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_deinit"));
    TEST_ASSERT(esp_wifi_deinit() == ESP_OK);
    ESP_LOGI(TAG, EMPH_STR("nvs_flash_deinit..."));
    nvs_flash_deinit();
    ESP_LOGI(TAG, "test passed...");
    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

TEST_CASE("Calling esp_wifi_stop() with start", "[wifi_init]")
{
    TaskHandle_t th;
    SemaphoreHandle_t sema = xSemaphoreCreateBinary();
    printf("Creating tasks\n");
#ifndef CONFIG_FREERTOS_UNICORE
    xTaskCreatePinnedToCore(wifi_start_stop_task, "wifi_start_stop_task", 2048*2, &sema, 3, &th, 0);
#else
    xTaskCreate(wifi_start_stop_task, "wifi_start_stop_task", 2048*2, &sema, 3, &th);
#endif
    xSemaphoreTake(sema, portMAX_DELAY);
    vSemaphoreDelete(sema);
    sema = NULL;
    TEST_IGNORE_MESSAGE("this test case is ignored due to the critical memory leak of tcpip_adapter and event_loop.");
}

static void wifi_stop_task(void* arg)
{
    SemaphoreHandle_t *sema = (SemaphoreHandle_t *) arg;
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    //init nvs
    ESP_LOGI(TAG, EMPH_STR("nvs_flash_init"));
    esp_err_t r = nvs_flash_init();
    if (r == ESP_ERR_NVS_NO_FREE_PAGES || r == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGI(TAG, EMPH_STR("no free pages or NFS version mismatch, erase.."));
        TEST_ESP_OK(nvs_flash_erase());
        r = nvs_flash_init();
    }
    TEST_ESP_OK(r);
    //init event loop
    ESP_LOGI(TAG, EMPH_STR("event_init"));
    event_init();
    unity_reset_leak_checks();
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_init"));
    TEST_ASSERT(esp_wifi_init(&cfg) == ESP_OK);
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_stop"));
    TEST_ASSERT(esp_wifi_stop() == ESP_OK);
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_stop"));
    TEST_ASSERT(esp_wifi_stop() == ESP_OK);
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_deinit"));
    TEST_ASSERT(esp_wifi_deinit() == ESP_OK);
    ESP_LOGI(TAG, EMPH_STR("nvs_flash_deinit..."));
    nvs_flash_deinit();
    ESP_LOGI(TAG, "test passed...");
    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

TEST_CASE("Calling esp_wifi_stop() without start", "[wifi_init]")
{
    TaskHandle_t th;
    SemaphoreHandle_t sema = xSemaphoreCreateBinary();
    printf("Creating tasks\n");
#ifndef CONFIG_FREERTOS_UNICORE
    xTaskCreatePinnedToCore(wifi_stop_task, "wifi_stop_task", 2048*2, &sema, 3, &th, 0);
#else
    xTaskCreate(wifi_stop_task, "wifi_stop_task", 2048*2, &sema, 3, &th);
#endif
    xSemaphoreTake(sema, portMAX_DELAY);
    vSemaphoreDelete(sema);
    sema = NULL;
    TEST_IGNORE_MESSAGE("this test case is ignored due to the event_loop.");
}
