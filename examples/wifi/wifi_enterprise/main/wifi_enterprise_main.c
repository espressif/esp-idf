/*
 * SPDX-FileCopyrightText: 2006-2016 ARM Limited
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_eap_client.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"

/* The examples use simple WiFi configuration that you can set via
   project configuration menu.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"

   You can choose EAP method via project configuration according to the
   configuration of AP.
*/
#define EXAMPLE_WIFI_SSID CONFIG_EXAMPLE_WIFI_SSID
#define EXAMPLE_EAP_METHOD CONFIG_EXAMPLE_EAP_METHOD

#define EXAMPLE_EAP_ID CONFIG_EXAMPLE_EAP_ID
#define EXAMPLE_EAP_USERNAME CONFIG_EXAMPLE_EAP_USERNAME
#define EXAMPLE_EAP_PASSWORD CONFIG_EXAMPLE_EAP_PASSWORD

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* esp netif object representing the WIFI station */
static esp_netif_t *sta_netif = NULL;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;

static const char *TAG = "example";

/* CA cert, taken from ca.pem
   Client cert, taken from client.crt
   Client key, taken from client.key

   The PEM, CRT and KEY file were provided by the person or organization
   who configured the AP with wifi enterprise.

   To embed it in the app binary, the PEM, CRT and KEY file is named
   in the component.mk COMPONENT_EMBED_TXTFILES variable.
*/
#if defined(CONFIG_EXAMPLE_VALIDATE_SERVER_CERT) || \
    defined(CONFIG_EXAMPLE_WPA3_ENTERPRISE) || \
    defined(CONFIG_EXAMPLE_WPA3_192BIT_ENTERPRISE) || \
    defined(CONFIG_ESP_WIFI_EAP_TLS1_3)
#define SERVER_CERT_VALIDATION_ENABLED
#endif

#ifdef SERVER_CERT_VALIDATION_ENABLED
extern uint8_t ca_pem_start[] asm("_binary_ca_pem_start");
extern uint8_t ca_pem_end[]   asm("_binary_ca_pem_end");
#endif /* SERVER_CERT_VALIDATION_ENABLED */

#ifdef CONFIG_EXAMPLE_EAP_METHOD_TLS
extern uint8_t client_crt_start[] asm("_binary_client_crt_start");
extern uint8_t client_crt_end[]   asm("_binary_client_crt_end");
extern uint8_t client_key_start[] asm("_binary_client_key_start");
extern uint8_t client_key_end[]   asm("_binary_client_key_end");
#endif /* CONFIG_EXAMPLE_EAP_METHOD_TLS */

#if defined CONFIG_EXAMPLE_EAP_METHOD_TTLS
esp_eap_ttls_phase2_types TTLS_PHASE2_METHOD = CONFIG_EXAMPLE_EAP_METHOD_TTLS_PHASE_2;
#endif /* CONFIG_EXAMPLE_EAP_METHOD_TTLS */

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
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

static void initialise_wifi(void)
{
#ifdef SERVER_CERT_VALIDATION_ENABLED
    unsigned int ca_pem_bytes = ca_pem_end - ca_pem_start;
#endif /* SERVER_CERT_VALIDATION_ENABLED */

#ifdef CONFIG_EXAMPLE_EAP_METHOD_TLS
    unsigned int client_crt_bytes = client_crt_end - client_crt_start;
    unsigned int client_key_bytes = client_key_end - client_key_start;
#endif /* CONFIG_EXAMPLE_EAP_METHOD_TLS */

    ESP_ERROR_CHECK(esp_netif_init());
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
#if defined (CONFIG_EXAMPLE_WPA3_192BIT_ENTERPRISE) || defined (CONFIG_EXAMPLE_WPA3_ENTERPRISE)
            .pmf_cfg = {
                .required = true
            },
#endif
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_eap_client_set_identity((uint8_t *)EXAMPLE_EAP_ID, strlen(EXAMPLE_EAP_ID)) );

#ifdef SERVER_CERT_VALIDATION_ENABLED
    ESP_ERROR_CHECK(esp_eap_client_set_ca_cert(ca_pem_start, ca_pem_bytes) );
#endif /* SERVER_CERT_VALIDATION_ENABLED */

#ifdef CONFIG_EXAMPLE_EAP_METHOD_TLS
    ESP_ERROR_CHECK(esp_eap_client_set_certificate_and_key(client_crt_start, client_crt_bytes,
                                      client_key_start, client_key_bytes, NULL, 0) );
#endif /* CONFIG_EXAMPLE_EAP_METHOD_TLS */

#if defined (CONFIG_EXAMPLE_EAP_METHOD_PEAP) || \
    defined (CONFIG_EXAMPLE_EAP_METHOD_TTLS)
    ESP_ERROR_CHECK(esp_eap_client_set_username((uint8_t *)EXAMPLE_EAP_USERNAME, strlen(EXAMPLE_EAP_USERNAME)) );
    ESP_ERROR_CHECK(esp_eap_client_set_password((uint8_t *)EXAMPLE_EAP_PASSWORD, strlen(EXAMPLE_EAP_PASSWORD)) );
#endif /* CONFIG_EXAMPLE_EAP_METHOD_PEAP || CONFIG_EXAMPLE_EAP_METHOD_TTLS */

#if defined CONFIG_EXAMPLE_EAP_METHOD_TTLS
    ESP_ERROR_CHECK(esp_eap_client_set_ttls_phase2_method(TTLS_PHASE2_METHOD) );
#endif /* CONFIG_EXAMPLE_EAP_METHOD_TTLS */

#if defined (CONFIG_EXAMPLE_WPA3_192BIT_ENTERPRISE)
    ESP_LOGI(TAG, "Enabling 192 bit certification");
    ESP_ERROR_CHECK(esp_eap_client_set_suiteb_192bit_certification(true));
#endif
#ifdef CONFIG_EXAMPLE_USE_DEFAULT_CERT_BUNDLE
    ESP_ERROR_CHECK(esp_eap_client_use_default_cert_bundle(true));
#endif
    ESP_ERROR_CHECK(esp_wifi_sta_enterprise_enable());
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void wifi_enterprise_example_task(void *pvParameters)
{
    esp_netif_ip_info_t ip;
    memset(&ip, 0, sizeof(esp_netif_ip_info_t));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    while (1) {
        vTaskDelay(2000 / portTICK_PERIOD_MS);

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
    ESP_ERROR_CHECK(nvs_flash_init());
    initialise_wifi();
    xTaskCreate(&wifi_enterprise_example_task, "wifi_enterprise_example_task", 4096, NULL, 5, NULL);
}
