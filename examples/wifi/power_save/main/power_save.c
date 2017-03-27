/* Power save Example
 * this example shows how to use power save mode
 * 
 * set a router or a AP using the same SSID&PASSWORD as configuration of this example.
 * start esp32 and when it connected to AP it will inter power save mode
 */
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event_loop.h"

/*set the ssid and password via "make menuconfig"*/
#define DEFAULT_SSID CONFIG_WIFI_SSID
#define DEFAULT_PWD CONFIG_WIFI_PASSWORD
/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

static const char *TAG = "power_save";


static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
	ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
	ESP_LOGI(TAG, "got ip:%s\n",
		ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
	ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    default:
        break;
    }
    return ESP_OK;
}

/*init wifi as sta and set pawer save mode*/
static void wifi_power_save(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
	.sta = {
	    .ssid = DEFAULT_SSID,
	    .password = DEFAULT_PWD
	},
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    ESP_LOGI(TAG, "esp_wifi_set_ps(WIFI_PS_MODEM).");
    esp_wifi_set_ps(WIFI_PS_MODEM);
}

void app_main()
{
    wifi_power_save();
}
