/* Wi-Fi Provisioning Manager Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>

#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_ble.h>
#include <wifi_provisioning/scheme_softap.h>

static const char *TAG = "app";

/* Signal Wi-Fi events on this event-group */
const int WIFI_CONNECTED_EVENT = BIT0;
static EventGroupHandle_t wifi_event_group;

/* Event handler for catching system events */
static void event_handler(void* arg, esp_event_base_t event_base,
                          int event_id, void* event_data)
{
    if (event_base == WIFI_PROV_EVENT) {
        switch (event_id) {
            case WIFI_PROV_START:
                ESP_LOGI(TAG, "Provisioning started");
                break;
            case WIFI_PROV_CRED_RECV: {
                wifi_sta_config_t *wifi_sta_cfg = (wifi_sta_config_t *)event_data;
                ESP_LOGI(TAG, "Received Wi-Fi credentials"
                         "\n\tSSID     : %s\n\tPassword : %s",
                         (const char *) wifi_sta_cfg->ssid,
                         (const char *) wifi_sta_cfg->password);
                break;
            }
            case WIFI_PROV_CRED_FAIL: {
                wifi_prov_sta_fail_reason_t *reason = (wifi_prov_sta_fail_reason_t *)event_data;
                ESP_LOGE(TAG, "Provisioning failed!\n\tReason : %s"
                         "\n\tPlease reset to factory and retry provisioning",
                         (*reason == WIFI_PROV_STA_AUTH_ERROR) ?
                         "Wi-Fi station authentication failed" : "Wi-Fi access-point not found");
                break;
            }
            case WIFI_PROV_CRED_SUCCESS:
                ESP_LOGI(TAG, "Provisioning successful");
                break;
            case WIFI_PROV_END:
                /* De-initialize manager once provisioning is finished */
                wifi_prov_mgr_deinit();
                break;
            default:
                break;
        }
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Connected with IP Address:" IPSTR, IP2STR(&event->ip_info.ip));
        /* Signal main application to continue execution */
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_EVENT);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Disconnected. Connecting to the AP again...");
        esp_wifi_connect();
    }
}

static void wifi_init_sta(void)
{
    /* Start Wi-Fi in station mode */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void get_device_service_name(char *service_name, size_t max)
{
    uint8_t eth_mac[6];
    const char *ssid_prefix = "PROV_";
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    snprintf(service_name, max, "%s%02X%02X%02X",
             ssid_prefix, eth_mac[3], eth_mac[4], eth_mac[5]);
}

void app_main(void)
{
    /* Initialize NVS partition */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        /* NVS partition was truncated
         * and needs to be erased */
        ESP_ERROR_CHECK(nvs_flash_erase());

        /* Retry nvs_flash_init */
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    /* Initialize TCP/IP */
    esp_netif_init();

    /* Initialize the event loop */
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_event_group = xEventGroupCreate();

    /* Register our event handler for Wi-Fi, IP and Provisioning related events */
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    /* Initialize Wi-Fi including netif with default config */
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* Configuration for the provisioning manager */
    wifi_prov_mgr_config_t config = {
        /* What is the Provisioning Scheme that we want ?
         * wifi_prov_scheme_softap or wifi_prov_scheme_ble */
        .scheme = wifi_prov_scheme_ble,

        /* Any default scheme specific event handler that you would
         * like to choose. Since our example application requires
         * neither BT nor BLE, we can choose to release the associated
         * memory once provisioning is complete, or not needed
         * (in case when device is already provisioned). Choosing
         * appropriate scheme specific event handler allows the manager
         * to take care of this automatically. This can be set to
         * WIFI_PROV_EVENT_HANDLER_NONE when using wifi_prov_scheme_softap*/
        .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM
    };

    /* Initialize provisioning manager with the
     * configuration parameters set above */
    ESP_ERROR_CHECK(wifi_prov_mgr_init(config));

    bool provisioned = false;
    /* Let's find out if the device is provisioned */
    ESP_ERROR_CHECK(wifi_prov_mgr_is_provisioned(&provisioned));

    /* If device is not yet provisioned start provisioning service */
    if (!provisioned) {
        ESP_LOGI(TAG, "Starting provisioning");

        /* What is the Device Service Name that we want
         * This translates to :
         *     - Wi-Fi SSID when scheme is wifi_prov_scheme_softap
         *     - device name when scheme is wifi_prov_scheme_ble
         */
        char service_name[12];
        get_device_service_name(service_name, sizeof(service_name));

        /* What is the security level that we want (0 or 1):
         *      - WIFI_PROV_SECURITY_0 is simply plain text communication.
         *      - WIFI_PROV_SECURITY_1 is secure communication which consists of secure handshake
         *          using X25519 key exchange and proof of possession (pop) and AES-CTR
         *          for encryption/decryption of messages.
         */
        wifi_prov_security_t security = WIFI_PROV_SECURITY_1;

        /* Do we want a proof-of-possession (ignored if Security 0 is selected):
         *      - this should be a string with length > 0
         *      - NULL if not used
         */
        const char *pop = "abcd1234";

        /* What is the service key (could be NULL)
         * This translates to :
         *     - Wi-Fi password when scheme is wifi_prov_scheme_softap
         *     - simply ignored when scheme is wifi_prov_scheme_ble
         */
        const char *service_key = NULL;

        /* This step is only useful when scheme is wifi_prov_scheme_ble. This will
         * set a custom 128 bit UUID which will be included in the BLE advertisement
         * and will correspond to the primary GATT service that provides provisioning
         * endpoints as GATT characteristics. Each GATT characteristic will be
         * formed using the primary service UUID as base, with different auto assigned
         * 12th and 13th bytes (assume counting starts from 0th byte). The client side
         * applications must identify the endpoints by reading the User Characteristic
         * Description descriptor (0x2901) for each characteristic, which contains the
         * endpoint name of the characteristic */
        uint8_t custom_service_uuid[] = {
            /* LSB <---------------------------------------
             * ---------------------------------------> MSB */
            0x21, 0x43, 0x65, 0x87, 0x09, 0xba, 0xdc, 0xfe,
            0xef, 0xcd, 0xab, 0x90, 0x78, 0x56, 0x34, 0x12
        };
        wifi_prov_scheme_ble_set_service_uuid(custom_service_uuid);

        /* Start provisioning service */
        ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(security, pop, service_name, service_key));

        /* Uncomment the following to wait for the provisioning to finish and then release
         * the resources of the manager. Since in this case de-initialization is triggered
         * by the default event loop handler, we don't need to call the following */
        // wifi_prov_mgr_wait();
        // wifi_prov_mgr_deinit();
    } else {
        ESP_LOGI(TAG, "Already provisioned, starting Wi-Fi STA");

        /* We don't need the manager as device is already provisioned,
         * so let's release it's resources */
        wifi_prov_mgr_deinit();

        /* Start Wi-Fi station */
        wifi_init_sta();
    }

    /* Wait for Wi-Fi connection */
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_EVENT, false, true, portMAX_DELAY);

    /* Start main application now */
    while (1) {
        ESP_LOGI(TAG, "Hello World!");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
