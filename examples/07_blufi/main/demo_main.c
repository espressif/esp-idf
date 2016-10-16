#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "bt.h"
#include "bta_api.h"

extern tBTA_STATUS BTA_DisableBluetooth(void);
extern void phy_set_wifi_mode_only(bool wifi_only);
extern void bte_main_boot_entry(void *);
extern void bt_app_task_start_up(void);
extern void bt_app_task_shut_down(void);
extern void bt_app_core_start(void);

#define WIFI_LIST_NUM	10

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;



static wifi_scan_config_t scan_config;
static wifi_config_t sta_config;

static char tmp_ssid[33];
static char tmp_passwd[33];
static bool confirm = false;

void wifi_set_blue_config(char *ssid, char *passwd)
{
	memset(tmp_ssid, 0, 33);
	memset(tmp_passwd, 0, 33);
	strcpy(tmp_ssid, ssid);
	strcpy(tmp_passwd, passwd);
	confirm = true;
	printf("confirm true\n");
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}


static int loop = 0;
void wifiTestTask(void *pvParameters)
{
    esp_err_t ret;

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if (confirm) {
		confirm = false;
		BTA_DisableBluetooth();

		strcpy(sta_config.sta.ssid, tmp_ssid);
		strcpy(sta_config.sta.password, tmp_passwd);
		sta_config.sta.bssid_set = 0;

		ret = esp_wifi_disconnect();
		printf("esp_wifi config\n");
		esp_wifi_set_config(WIFI_IF_STA, &sta_config);
		printf("esp_wifi connect\n");
		ret = esp_wifi_connect();
		if (ret != ESP_OK) {
			printf("esp_wifi connect failed\n");
		}
	}
    }
}

void app_main()
{
    esp_err_t ret;

    phy_set_wifi_mode_only(0);
    nvs_flash_init();
    system_init();
    initialise_wifi();

    vTaskDelay(3000 / portTICK_PERIOD_MS);

    bt_controller_init();
    xTaskCreatePinnedToCore(&wifiTestTask, "wifiTestTask", 2048, NULL, 20, NULL, 0);
    bt_app_task_start_up();
    bte_main_boot_entry(bt_app_core_start);
}
