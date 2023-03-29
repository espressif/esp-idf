/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/*  WiFi Aware (NAN) Subscriber Example

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
#include "ping/ping_sock.h"

#define EXAMPLE_NAN_SERV_NAME           CONFIG_ESP_WIFI_NAN_SVC_NAME
#define EXAMPLE_NAN_MATCHING_FILTER     CONFIG_ESP_WIFI_NAN_MATCHING_FILTER
#ifdef CONFIG_ESP_WIFI_NAN_SERVICE_MESSAGE
#define EXAMPLE_NAN_SVC_MSG             CONFIG_ESP_WIFI_NAN_SERVICE_MESSAGE
#else
#define EXAMPLE_NAN_SVC_MSG             "Hello"
#endif

static const char *TAG = "nan_sub";

static EventGroupHandle_t nan_event_group;

const int NAN_SERVICE_MATCH = BIT0;
const int NDP_CONFIRMED = BIT1;
const int NDP_FAILED = BIT2;

static wifi_event_nan_svc_match_t g_svc_match_evt;

#ifdef CONFIG_EXAMPLE_NAN_SEND_PING
static uint8_t g_peer_ndi[6];

static void cmd_ping_on_ping_success(esp_ping_handle_t hdl, void *args)
{
    uint8_t ttl;
    uint16_t seqno;
    uint32_t elapsed_time, recv_len;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TTL, &ttl, sizeof(ttl));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
    ESP_LOGI(TAG, "%lu bytes from %s icmp_seq=%u ttl=%u time=%lu ms",
             recv_len, ipaddr_ntoa((ip_addr_t *)&target_addr), seqno, ttl, elapsed_time);
}

static void cmd_ping_on_ping_timeout(esp_ping_handle_t hdl, void *args)
{
    uint16_t seqno;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    ESP_LOGI(TAG, "From %s icmp_seq=%d timeout", ipaddr_ntoa((ip_addr_t *)&target_addr), seqno);
}

static void cmd_ping_on_ping_end(esp_ping_handle_t hdl, void *args)
{
    ip_addr_t target_addr;
    uint32_t transmitted;
    uint32_t received;
    uint32_t total_time_ms;
    esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
    esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));
    uint32_t loss = (uint32_t)((1 - ((float)received) / transmitted) * 100);
    ESP_LOGI(TAG, "\n--- %s ping statistics ---", inet6_ntoa(*ip_2_ip6(&target_addr)));
    ESP_LOGI(TAG, "%lu packets transmitted, %lu received, %lu%% packet loss, time %lums",
             transmitted, received, loss, total_time_ms);
    // delete the ping sessions, so that we clean up all resources and can create a new ping session
    // we don't have to call delete function in the callback, instead we can call delete function from other tasks
    esp_ping_delete_session(hdl);
}

static void nan_ndp_confirmed_event_handler(void *arg, esp_event_base_t event_base,
        int32_t event_id, void *event_data)
{
    wifi_event_ndp_confirm_t *evt = (wifi_event_ndp_confirm_t *)event_data;

    if (evt->status == NDP_STATUS_REJECTED) {
        ESP_LOGE(TAG, "NDP request to Peer "MACSTR" rejected [NDP ID - %d]", MAC2STR(evt->peer_nmi), evt->ndp_id);
        xEventGroupSetBits(nan_event_group, NDP_FAILED);
    } else {
        memcpy(g_peer_ndi, evt->peer_ndi, sizeof(g_peer_ndi));
        xEventGroupSetBits(nan_event_group, NDP_CONFIRMED);
    }
}

static void ping_nan_peer(esp_netif_t *netif)
{
    esp_ping_config_t config = ESP_PING_DEFAULT_CONFIG();
    config.task_stack_size = 4096;
    ip_addr_t target_addr = {0};

    esp_wifi_nan_get_ipv6_linklocal_from_mac(&target_addr.u_addr.ip6, g_peer_ndi);
    target_addr.type = IPADDR_TYPE_V6;
    config.target_addr = target_addr;
    config.interface = esp_netif_get_netif_impl_index(netif);

    /* set callback functions */
    esp_ping_callbacks_t cbs = {
        .on_ping_success = cmd_ping_on_ping_success,
        .on_ping_timeout = cmd_ping_on_ping_timeout,
        .on_ping_end = cmd_ping_on_ping_end,
        .cb_args = NULL
    };

    esp_ping_handle_t ping;
    if (esp_ping_new_session(&config, &cbs, &ping) == ESP_OK) {
        ESP_LOGI(TAG, "Pinging Peer with IPv6 addr %s", ipaddr_ntoa((ip_addr_t*)&target_addr));
        esp_ping_start(ping);
    } else {
        ESP_LOGI(TAG, "Failed to ping Peer with IPv6 addr %s", ipaddr_ntoa((ip_addr_t*)&target_addr));
    }
}
#endif

static void nan_svc_match_event_handler(void* arg, esp_event_base_t event_base,
                                        int32_t event_id, void* event_data)
{
    wifi_event_nan_svc_match_t *evt = (wifi_event_nan_svc_match_t *)event_data;
    ESP_LOGI(TAG, "NAN Publisher found for Serv ID %d", evt->subscribe_id);
    memcpy(&g_svc_match_evt, evt, sizeof(wifi_event_nan_svc_match_t));
    xEventGroupSetBits(nan_event_group, NAN_SERVICE_MATCH);
}

void wifi_nan_subscribe(void)
{
    nan_event_group = xEventGroupCreate();
    esp_event_handler_instance_t instance_any_id;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        WIFI_EVENT_NAN_SVC_MATCH,
                                                        &nan_svc_match_event_handler,
                                                        NULL,
                                                        &instance_any_id));

#ifdef CONFIG_EXAMPLE_NAN_SEND_PING
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        WIFI_EVENT_NDP_CONFIRM,
                                                        &nan_ndp_confirmed_event_handler,
                                                        NULL,
                                                        &instance_any_id));
#endif

    /* Start NAN Discovery */
    wifi_nan_config_t nan_cfg = WIFI_NAN_CONFIG_DEFAULT();
    esp_netif_t *nan_netif = esp_netif_create_default_wifi_nan();
    (void) nan_netif;
    esp_wifi_nan_start(&nan_cfg);

    /* Subscribe a service */
    uint8_t sub_id;
    wifi_nan_subscribe_cfg_t subscribe_cfg = {
        .service_name = EXAMPLE_NAN_SERV_NAME,
#if CONFIG_EXAMPLE_NAN_SUBSCRIBE_PASSIVE
        .type = NAN_SUBSCRIBE_PASSIVE,
#else
        .type = NAN_SUBSCRIBE_ACTIVE,
#endif
        .matching_filter = EXAMPLE_NAN_MATCHING_FILTER,
        .single_match_event = 1,
    };

    sub_id = esp_wifi_nan_subscribe_service(&subscribe_cfg);
    if (sub_id == 0) {
        return;
    }

    EventBits_t bits_1 = xEventGroupWaitBits(nan_event_group, NAN_SERVICE_MATCH, pdFALSE, pdFALSE,
                                           portMAX_DELAY);
    if (bits_1 & NAN_SERVICE_MATCH) {
#ifdef CONFIG_EXAMPLE_NAN_SEND_MESSAGE
        wifi_nan_followup_params_t fup = {
            .inst_id = sub_id,
            .peer_inst_id = g_svc_match_evt.publish_id,
            .svc_info = EXAMPLE_NAN_SVC_MSG,
        };
        memcpy(fup.peer_mac, g_svc_match_evt.pub_if_mac, sizeof(fup.peer_mac));

        if (esp_wifi_nan_send_message(&fup) == ESP_OK)
            ESP_LOGI(TAG, "Sending message '%s' to Publisher "MACSTR" ...",
                          EXAMPLE_NAN_SVC_MSG, MAC2STR(g_svc_match_evt.pub_if_mac));
#endif
#ifdef CONFIG_EXAMPLE_NAN_SEND_PING
        wifi_nan_datapath_req_t ndp_req = {0};
        ndp_req.confirm_required = true;
        ndp_req.pub_id = g_svc_match_evt.publish_id;
        memcpy(ndp_req.peer_mac, g_svc_match_evt.pub_if_mac, sizeof(ndp_req.peer_mac));
        esp_wifi_nan_datapath_req(&ndp_req);

        EventBits_t bits_2 = xEventGroupWaitBits(nan_event_group, NDP_CONFIRMED, pdFALSE, pdFALSE, portMAX_DELAY);
        if (bits_2 & NDP_CONFIRMED) {
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            ping_nan_peer(nan_netif);
        } else if (bits_2 & NDP_FAILED) {
            ESP_LOGI(TAG, "Failed to setup NAN Datapath");
        }
#endif
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

    wifi_nan_subscribe();
}
