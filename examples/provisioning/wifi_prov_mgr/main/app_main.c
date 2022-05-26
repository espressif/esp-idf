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

#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
#include <wifi_provisioning/scheme_ble.h>
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */

#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP
#include <wifi_provisioning/scheme_softap.h>
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP */
#include "qrcode.h"

static const char *TAG = "app";

static const char sec2_salt[4] = {0xc1, 0x85, 0x74, 0xfe};

static const char sec2_verifier[384] = {0x88, 0x98, 0x2a, 0xd3, 0xb1, 0x8a, 0xf6, 0xe3, 0x00, 0x5d, 0x34, 0x63, 0x56, 0x73, 0x2a, 0x03,
                                 0x7b, 0x1c, 0xae, 0xe8, 0x2e, 0x59, 0x03, 0x48, 0x68, 0x00, 0xba, 0xa1, 0xb0, 0xff, 0x12, 0xca,
                                 0xa7, 0xc9, 0xff, 0x69, 0xaf, 0x7a, 0x1e, 0x53, 0x98, 0x15, 0x79, 0xd1, 0x11, 0x83, 0x68, 0xf2,
                                 0x8c, 0x7a, 0xcf, 0x8b, 0x1b, 0x25, 0x2e, 0x0c, 0xcb, 0xdc, 0x6a, 0x7c, 0xda, 0x2e, 0x6d, 0x5b,
                                 0xcc, 0xaa, 0x8c, 0x6e, 0x92, 0x07, 0xd8, 0x48, 0x08, 0xd1, 0xbb, 0x72, 0x80, 0x65, 0xfd, 0x7d,
                                 0x77, 0xfb, 0x76, 0x14, 0x62, 0x58, 0xdd, 0xc4, 0xbf, 0x6b, 0xe4, 0x48, 0x83, 0xc4, 0x0b, 0x83,
                                 0x16, 0xa7, 0xf5, 0x1c, 0xd5, 0x9a, 0x47, 0xbc, 0xb3, 0xcd, 0x7c, 0x15, 0x5c, 0x6d, 0x40, 0x6e,
                                 0xaf, 0xa6, 0x7c, 0x49, 0x12, 0x07, 0x74, 0xa3, 0xf3, 0x98, 0x8d, 0x12, 0x69, 0xbd, 0xbc, 0x29,
                                 0xe8, 0x2b, 0x03, 0x82, 0x50, 0x5e, 0xb2, 0x95, 0xbd, 0x7d, 0x36, 0x43, 0x20, 0xd1, 0x0f, 0xbc,
                                 0x0e, 0xd5, 0x92, 0xe4, 0xe2, 0x64, 0xe6, 0x90, 0x66, 0xf5, 0x27, 0xee, 0x7e, 0xf1, 0x02, 0x35,
                                 0x8c, 0xea, 0xe7, 0x96, 0x0b, 0x00, 0x57, 0x54, 0xfa, 0x9e, 0x08, 0x75, 0xb9, 0x36, 0x71, 0x5e,
                                 0x68, 0xb8, 0x66, 0xff, 0x0c, 0x1b, 0x2e, 0xbe, 0x23, 0x8b, 0x37, 0xc1, 0xd5, 0x3b, 0xf7, 0x7f,
                                 0xcf, 0xb7, 0x65, 0x09, 0xda, 0x2b, 0x01, 0xbb, 0xd0, 0x5e, 0xf6, 0xd3, 0x21, 0xae, 0x33, 0x3d,
                                 0xb7, 0xdf, 0xfb, 0xb6, 0x88, 0xbe, 0x9e, 0xb5, 0xef, 0xd8, 0x74, 0x58, 0x08, 0xef, 0x8a, 0x40,
                                 0xa2, 0xda, 0x63, 0xe1, 0xb9, 0x71, 0x21, 0x46, 0xae, 0x45, 0x1f, 0x76, 0x14, 0xbf, 0x74, 0xb6,
                                 0x2e, 0xe2, 0x81, 0x7e, 0x5e, 0x99, 0x15, 0x2a, 0xd7, 0x73, 0xbf, 0x0a, 0x00, 0xe8, 0xea, 0x3c,
                                 0xbe, 0xb7, 0x70, 0xb3, 0x63, 0xa2, 0x5c, 0xff, 0x37, 0xbd, 0x40, 0x9a, 0xa9, 0x6c, 0xde, 0x1e,
                                 0xc0, 0x73, 0xd3, 0x00, 0x00, 0x89, 0x78, 0x16, 0x3b, 0x86, 0x36, 0x99, 0x36, 0xa9, 0x7c, 0x92,
                                 0xa2, 0xea, 0x92, 0x37, 0x1f, 0x13, 0xd4, 0x1a, 0x88, 0x3d, 0x2b, 0x49, 0xb1, 0x27, 0xb6, 0x8c,
                                 0xa5, 0x54, 0xf7, 0xba, 0x1b, 0x3e, 0x13, 0xe4, 0x52, 0xfc, 0xa2, 0x92, 0xa1, 0x77, 0x21, 0x02,
                                 0x16, 0xd9, 0xd1, 0x52, 0xb5, 0x2a, 0x6a, 0xc7, 0xe6, 0x2d, 0x1c, 0x26, 0x33, 0x89, 0x28, 0x36,
                                 0xeb, 0xcb, 0x4b, 0x19, 0x77, 0x6a, 0xe7, 0x2e, 0x1a, 0x53, 0x3e, 0xe1, 0xf4, 0x12, 0x94, 0x2a,
                                 0x90, 0xca, 0x47, 0x65, 0xdc, 0xca, 0xc5, 0x5c, 0xb3, 0xea, 0x70, 0xf9, 0xf0, 0xe1, 0x17, 0xc3,
                                 0xfb, 0x9d, 0xbd, 0xf6, 0x27, 0x3d, 0xa9, 0xd0, 0x4c, 0xb7, 0x6f, 0xe6, 0x21, 0x25, 0x0f, 0x29};

/* Signal Wi-Fi events on this event-group */
const int WIFI_CONNECTED_EVENT = BIT0;
static EventGroupHandle_t wifi_event_group;

#define PROV_QR_VERSION         "v1"
#define PROV_TRANSPORT_SOFTAP   "softap"
#define PROV_TRANSPORT_BLE      "ble"
#define QRCODE_BASE_URL         "https://espressif.github.io/esp-jumpstart/qrcode.html"

/* Event handler for catching system events */
static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
#ifdef CONFIG_EXAMPLE_RESET_PROV_MGR_ON_FAILURE
    static int retries;
#endif
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
#ifdef CONFIG_EXAMPLE_RESET_PROV_MGR_ON_FAILURE
                retries++;
                if (retries >= CONFIG_EXAMPLE_PROV_MGR_MAX_RETRY_CNT) {
                    ESP_LOGI(TAG, "Failed to connect with provisioned AP, reseting provisioned credentials");
                    wifi_prov_mgr_reset_sm_state_on_failure();
                    retries = 0;
                }
#endif
                break;
            }
            case WIFI_PROV_CRED_SUCCESS:
                ESP_LOGI(TAG, "Provisioning successful");
#ifdef CONFIG_EXAMPLE_RESET_PROV_MGR_ON_FAILURE
                retries = 0;
#endif
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

/* Handler for the optional provisioning endpoint registered by the application.
 * The data format can be chosen by applications. Here, we are using plain ascii text.
 * Applications can choose to use other formats like protobuf, JSON, XML, etc.
 */
esp_err_t custom_prov_data_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
                                          uint8_t **outbuf, ssize_t *outlen, void *priv_data)
{
    if (inbuf) {
        ESP_LOGI(TAG, "Received data: %.*s", inlen, (char *)inbuf);
    }
    char response[] = "SUCCESS";
    *outbuf = (uint8_t *)strdup(response);
    if (*outbuf == NULL) {
        ESP_LOGE(TAG, "System out of memory");
        return ESP_ERR_NO_MEM;
    }
    *outlen = strlen(response) + 1; /* +1 for NULL terminating byte */

    return ESP_OK;
}

static void wifi_prov_print_qr(const char *name, const char *pop, const char *transport)
{
    if (!name || !transport) {
        ESP_LOGW(TAG, "Cannot generate QR code payload. Data missing.");
        return;
    }
    char payload[150] = {0};
    if (pop) {
        snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\"" \
                    ",\"pop\":\"%s\",\"transport\":\"%s\"}",
                    PROV_QR_VERSION, name, pop, transport);
    } else {
        snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\"" \
                    ",\"transport\":\"%s\"}",
                    PROV_QR_VERSION, name, transport);
    }
#ifdef CONFIG_EXAMPLE_PROV_SHOW_QR
    ESP_LOGI(TAG, "Scan this QR code from the provisioning application for Provisioning.");
    esp_qrcode_config_t cfg = ESP_QRCODE_CONFIG_DEFAULT();
    esp_qrcode_generate(&cfg, payload);
#endif /* CONFIG_APP_WIFI_PROV_SHOW_QR */
    ESP_LOGI(TAG, "If QR code is not visible, copy paste the below URL in a browser.\n%s?data=%s", QRCODE_BASE_URL, payload);
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
    ESP_ERROR_CHECK(esp_netif_init());

    /* Initialize the event loop */
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_event_group = xEventGroupCreate();

    /* Register our event handler for Wi-Fi, IP and Provisioning related events */
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    /* Initialize Wi-Fi including netif with default config */
    esp_netif_create_default_wifi_sta();
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP
    esp_netif_create_default_wifi_ap();
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* Configuration for the provisioning manager */
    wifi_prov_mgr_config_t config = {
        /* What is the Provisioning Scheme that we want ?
         * wifi_prov_scheme_softap or wifi_prov_scheme_ble */
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
        .scheme = wifi_prov_scheme_ble,
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP
        .scheme = wifi_prov_scheme_softap,
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP */

        /* Any default scheme specific event handler that you would
         * like to choose. Since our example application requires
         * neither BT nor BLE, we can choose to release the associated
         * memory once provisioning is complete, or not needed
         * (in case when device is already provisioned). Choosing
         * appropriate scheme specific event handler allows the manager
         * to take care of this automatically. This can be set to
         * WIFI_PROV_EVENT_HANDLER_NONE when using wifi_prov_scheme_softap*/
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
        .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP
        .scheme_event_handler = WIFI_PROV_EVENT_HANDLER_NONE
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP */
    };

    /* Initialize provisioning manager with the
     * configuration parameters set above */
    ESP_ERROR_CHECK(wifi_prov_mgr_init(config));

    bool provisioned = false;
#ifdef CONFIG_EXAMPLE_RESET_PROVISIONED
    wifi_prov_mgr_reset_provisioning();
#else
    /* Let's find out if the device is provisioned */
    ESP_ERROR_CHECK(wifi_prov_mgr_is_provisioned(&provisioned));

#endif
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
         *      - WIFI_PROV_SECURITY_2 SRP6a based authentication and key exchange
         *        + AES-CTR encryption/decryption of messages
         */
        // wifi_prov_security_t security = WIFI_PROV_SECURITY_1;
        wifi_prov_security_t security = WIFI_PROV_SECURITY_2;

        /* Do we want a proof-of-possession (ignored if Security 0 is selected):
         *      - this should be a string with length > 0
         *      - NULL if not used
         */
        const char *pop = "abcd1234";
        /* This is the structure for passing security parameters
         * for the protocomm security 1
         */
        //const wifi_prov_security1_params_t sec1_params = {
        //    .data = (const unsigned char *)pop,
        //    .len = strlen(pop),
        //};

        static const wifi_prov_security2_params_t sec2_params = {
            .salt = sec2_salt,
            .salt_len = sizeof(sec2_salt),
            .verifier = sec2_verifier,
            .verifier_len = sizeof(sec2_verifier),
        };
        /* What is the service key (could be NULL)
         * This translates to :
         *     - Wi-Fi password when scheme is wifi_prov_scheme_softap
         *          (Minimum expected length: 8, maximum 64 for WPA2-PSK)
         *     - simply ignored when scheme is wifi_prov_scheme_ble
         */
        const char *service_key = NULL;

#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
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
            0xb4, 0xdf, 0x5a, 0x1c, 0x3f, 0x6b, 0xf4, 0xbf,
            0xea, 0x4a, 0x82, 0x03, 0x04, 0x90, 0x1a, 0x02,
        };

        /* If your build fails with linker errors at this point, then you may have
         * forgotten to enable the BT stack or BTDM BLE settings in the SDK (e.g. see
         * the sdkconfig.defaults in the example project) */
        wifi_prov_scheme_ble_set_service_uuid(custom_service_uuid);
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */

        /* An optional endpoint that applications can create if they expect to
         * get some additional custom data during provisioning workflow.
         * The endpoint name can be anything of your choice.
         * This call must be made before starting the provisioning.
         */
        wifi_prov_mgr_endpoint_create("custom-data");
        /* Start provisioning service */
        // ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(security, pop, service_name, service_key));

        ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(security, (void *) &sec2_params, service_name, service_key));

        /* The handler for the optional endpoint created above.
         * This call must be made after starting the provisioning, and only if the endpoint
         * has already been created above.
         */
        wifi_prov_mgr_endpoint_register("custom-data", custom_prov_data_handler, NULL);

        /* Uncomment the following to wait for the provisioning to finish and then release
         * the resources of the manager. Since in this case de-initialization is triggered
         * by the default event loop handler, we don't need to call the following */
        // wifi_prov_mgr_wait();
        // wifi_prov_mgr_deinit();
        /* Print QR code for provisioning */
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
        wifi_prov_print_qr(service_name, pop, PROV_TRANSPORT_BLE);
#else /* CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP */
        wifi_prov_print_qr(service_name, pop, PROV_TRANSPORT_SOFTAP);
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */
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
