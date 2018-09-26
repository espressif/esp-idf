/*
 Tests for the Wi-Fi
*/
#include "esp_system.h"
#include "unity.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "test_utils.h"

static const char* TAG = "test_wifi";

#define DEFAULT_SSID "TEST_SSID"
#define DEFAULT_PWD "TEST_PASS"

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    printf("ev_handle_called.\n");
    switch(event->event_id) {
        case SYSTEM_EVENT_STA_START:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
    //do not actually connect in test case
            //;
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
            ESP_LOGI(TAG, "got ip:%s\n",
            ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
            TEST_ESP_OK(esp_wifi_connect());
            break;
        default:
            break;
    }
    return ESP_OK;
}

#define EMPH_STR(s) "****** "s" ******"

static void test_wifi_init_deinit(wifi_init_config_t *cfg, wifi_config_t* wifi_config)
{
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_deinit"));
    TEST_ESP_ERR(ESP_ERR_WIFI_NOT_INIT, esp_wifi_deinit());
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_get_mode"));
    wifi_mode_t mode_get;
    TEST_ESP_ERR(ESP_ERR_WIFI_NOT_INIT, esp_wifi_get_mode(&mode_get));
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_init"));
    TEST_ESP_OK(esp_wifi_init(cfg));
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_set_mode"));
    TEST_ESP_OK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_set_config"));
    TEST_ESP_OK(esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config));
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_deinit..."));
    TEST_ESP_OK(esp_wifi_deinit());
}

static void test_wifi_start_stop(wifi_init_config_t *cfg, wifi_config_t* wifi_config)
{
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_stop"));
    TEST_ESP_ERR(ESP_ERR_WIFI_NOT_INIT, esp_wifi_stop());
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_init"));
    TEST_ESP_OK(esp_wifi_init(cfg));
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_set_mode"));
    TEST_ESP_OK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_set_config"));
    TEST_ESP_OK(esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config));
    //now start wifi
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_start..."));
    TEST_ESP_OK(esp_wifi_start());
    //wifi stop
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_stop..."));
    TEST_ESP_OK( esp_wifi_stop() );
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_deinit..."));
    TEST_ESP_OK(esp_wifi_deinit());
}

TEST_CASE("wifi stop and deinit","[wifi]")
{
    test_case_uses_tcpip();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = DEFAULT_SSID,
            .password = DEFAULT_PWD
        },
    };
    
    //init nvs
    ESP_LOGI(TAG, EMPH_STR("nvs_flash_init"));
    esp_err_t r = nvs_flash_init();
    if (r == ESP_ERR_NVS_NO_FREE_PAGES || r == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGI(TAG, EMPH_STR("no free pages or nvs version mismatch, erase.."));
        TEST_ESP_OK(nvs_flash_erase());
        r = nvs_flash_init();
    } 
    TEST_ESP_OK(r);
    //init tcpip
    ESP_LOGI(TAG, EMPH_STR("tcpip_adapter_init"));
    tcpip_adapter_init();
    //init event loop
    ESP_LOGI(TAG, EMPH_STR("esp_event_loop_init"));
    TEST_ESP_OK(esp_event_loop_init(event_handler, NULL));
    
    ESP_LOGI(TAG, "test wifi init & deinit...");
    test_wifi_init_deinit(&cfg, &wifi_config);
    ESP_LOGI(TAG, "wifi init & deinit seem to be OK.");

    ESP_LOGI(TAG, "test wifi start & stop...");
    test_wifi_start_stop(&cfg, &wifi_config);
    ESP_LOGI(TAG, "wifi start & stop seem to be OK.");

    ESP_LOGI(TAG, EMPH_STR("nvs_flash_deinit..."));
    nvs_flash_deinit();
    ESP_LOGI(TAG, "test passed...");

    TEST_IGNORE_MESSAGE("this test case is ignored due to the critical memory leak of tcpip_adapter and event_loop.");
}
