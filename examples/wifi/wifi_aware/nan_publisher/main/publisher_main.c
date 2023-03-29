/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/*  WiFi Aware (NAN) Publisher Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_nan.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#define EXAMPLE_NAN_SERV_NAME           CONFIG_ESP_WIFI_NAN_SVC_NAME
#define EXAMPLE_NAN_MATCHING_FILTER     CONFIG_ESP_WIFI_NAN_MATCHING_FILTER
#ifdef CONFIG_ESP_WIFI_NAN_SERVICE_MESSAGE
#define EXAMPLE_NAN_SVC_MSG            CONFIG_ESP_WIFI_NAN_SERVICE_MESSAGE
#else
#define EXAMPLE_NAN_SVC_MSG            "Welcome"
#endif

static EventGroupHandle_t nan_event_group;

static int NAN_RECEIVE = BIT0;
uint8_t g_peer_inst_id;

static void nan_receive_event_handler(void *arg, esp_event_base_t event_base,
                                      int32_t event_id, void *event_data)
{
    wifi_event_nan_receive_t *evt = (wifi_event_nan_receive_t *)event_data;
    g_peer_inst_id = evt->peer_inst_id;
    xEventGroupSetBits(nan_event_group, NAN_RECEIVE);
}

static void nan_ndp_indication_event_handler(void *arg, esp_event_base_t event_base,
                                             int32_t event_id, void *event_data)
{
    if (event_data == NULL) {
        return;
    }
    wifi_event_ndp_indication_t *evt = (wifi_event_ndp_indication_t *)event_data;

    wifi_nan_datapath_resp_t ndp_resp = {0};
    ndp_resp.accept = true; /* Accept incoming datapath request */
    ndp_resp.ndp_id = evt->ndp_id;
    memcpy(ndp_resp.peer_mac, evt->peer_nmi, 6);

    esp_wifi_nan_datapath_resp(&ndp_resp);

}
void wifi_nan_publish(void)
{
    nan_event_group = xEventGroupCreate();
    esp_event_handler_instance_t instance_any_id;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                    WIFI_EVENT_NAN_RECEIVE,
                    &nan_receive_event_handler,
                    NULL,
                    &instance_any_id));

    /* ndp_require_consent
     * If set to false - All incoming NDP requests will be internally accepted if valid.
     * If set to true - All incoming NDP requests raise NDP_INDICATION event, upon which application can either accept or reject them.
     */
    bool ndp_require_consent = true;

    /* Start NAN Discovery */
    wifi_nan_config_t nan_cfg = WIFI_NAN_CONFIG_DEFAULT();

    esp_netif_create_default_wifi_nan();
    esp_wifi_nan_start(&nan_cfg);

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                    WIFI_EVENT_NDP_INDICATION,
                    &nan_ndp_indication_event_handler,
                    NULL,
                    &instance_any_id));

    /* Publish a service */
    uint8_t pub_id;
    wifi_nan_publish_cfg_t publish_cfg = {
        .service_name = EXAMPLE_NAN_SERV_NAME,
#if CONFIG_EXAMPLE_NAN_PUBLISH_UNSOLICITED
        .type = NAN_PUBLISH_UNSOLICITED,
#else
        .type = NAN_PUBLISH_SOLICITED,
#endif
        .matching_filter = EXAMPLE_NAN_MATCHING_FILTER,
        .single_replied_event = 1,
    };

    pub_id = esp_wifi_nan_publish_service(&publish_cfg, ndp_require_consent);
    if (pub_id == 0) {
        return;
    }

    while (1) {
        EventBits_t bits = xEventGroupWaitBits(nan_event_group, NAN_RECEIVE, pdFALSE, pdFALSE, portMAX_DELAY);
        if (bits & NAN_RECEIVE) {
            xEventGroupClearBits(nan_event_group, NAN_RECEIVE);
            wifi_nan_followup_params_t fup = {0};
            fup.inst_id = pub_id,
            fup.peer_inst_id = g_peer_inst_id,
            strlcpy(fup.svc_info, EXAMPLE_NAN_SVC_MSG, ESP_WIFI_MAX_SVC_INFO_LEN);

            /* Reply to the message from a subscriber */
            esp_wifi_nan_send_message(&fup);
        }
        vTaskDelay(10);
    }
}

void initialise_wifi(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM) );
}

void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    initialise_wifi();

    wifi_nan_publish();
}
