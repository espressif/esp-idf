/* WiFi Connection Example using WPA2 Enterprise using EAP-NOOB
 */

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"

static EventGroupHandle_t wifi_event_group;
static esp_netif_t *sta_netif = NULL;

const int CONNECTED_BIT = BIT0;

static const char *TAG = "example";

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
    }
}

static void initialize_wifi(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "EAP-NOOB-Janfred"
        }
    };

    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)"noob@eap-noob.arpa", strlen("noob@eap-noob.arpa")) );
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_eap_noob_set_initial_association() );
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_eap_noob_set_oob_dir(EAP_NOOB_OOB_DIRECTION_BOTH));
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_enable() );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

static void wpa2_enterprise_example_task(void *pvParameters)
{
    esp_netif_ip_info_t ip;
    memset(&ip, 0, sizeof(esp_netif_ip_info_t));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    while (1) {
        vTaskDelay(2000 / portTICK_PERIOD_MS);

        if ( esp_wifi_sta_wpa2_ent_eap_noob_oob_pending() ) {
            eap_noob_oob_msg_t *oobmsg = esp_wifi_sta_wpa2_ent_eap_noob_generate_oob_message();
            char *hoob_str = malloc(33);
            char *noob_str = malloc(33);
            char *noobid_str = malloc(33);

            for (int i = 0; i < 16; i++) {
                snprintf(hoob_str + i * 2, 3, "%02x", oobmsg->hoob[i]);
                snprintf(noob_str + i * 2, 3, "%02x", oobmsg->noob[i]);
                snprintf(noobid_str + i * 2, 3, "%02x", oobmsg->noob_id[i]);
            }


            ESP_LOGI(TAG, "OOBMsg");
            ESP_LOGI(TAG, "Hoob: %s", hoob_str);
            ESP_LOGI(TAG, "Noob: %s", noob_str);
            ESP_LOGI(TAG, "NoobId: %s", noobid_str);
            ESP_LOGI(TAG, "PeerId: %s", esp_wifi_sta_wpa2_ent_eap_noob_get_peerid());
        }

        if (esp_netif_get_ip_info(sta_netif, &ip) == 0) {
            ESP_LOGI(TAG, "~~~~~~~~~~~");
            ESP_LOGI(TAG, "IP:"IPSTR, IP2STR(&ip.ip));
            ESP_LOGI(TAG, "MASK:"IPSTR, IP2STR(&ip.netmask));
            ESP_LOGI(TAG, "GW:"IPSTR, IP2STR(&ip.gw));
            ESP_LOGI(TAG, "~~~~~~~~~~~");
        }
    }
}

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_VERBOSE);
    ESP_ERROR_CHECK( nvs_flash_init() );
    initialize_wifi();
    xTaskCreate(&wpa2_enterprise_example_task, "wpa2_enterprise_example_task", 4096, NULL, 5, NULL);
}