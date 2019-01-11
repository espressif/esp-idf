/*
 Tests for the Wi-Fi
*/
#include "string.h"
#include "esp_system.h"
#include "unity.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "test_utils.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

static const char* TAG = "test_wifi";

#define DEFAULT_SSID "TEST_SSID"
#define DEFAULT_PWD "TEST_PASS"

#define GOT_IP_EVENT        0x00000001
#define DISCONNECT_EVENT    0x00000002

#define EVENT_HANDLER_FLAG_DO_NOT_AUTO_RECONNECT 0x00000001

static uint32_t wifi_event_handler_flag;

static EventGroupHandle_t wifi_events;

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
            if (wifi_events) {
                xEventGroupSetBits(wifi_events, GOT_IP_EVENT);
            }
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
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

static void start_wifi_as_softap(void)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.nvs_enable = false;

    wifi_config_t w_config = {
        .ap.ssid = DEFAULT_SSID,
        .ap.password = DEFAULT_PWD,
        .ap.ssid_len = 0,
        .ap.channel = 1,
        .ap.authmode = WIFI_AUTH_WPA2_PSK,
        .ap.ssid_hidden = false,
        .ap.max_connection = 4,
        .ap.beacon_interval = 100,
    };

    TEST_ESP_OK(esp_event_loop_init(event_handler, NULL));

    // can't deinit event loop, need to reset leak check
    unity_reset_leak_checks();

    if (wifi_events == NULL) {
        wifi_events = xEventGroupCreate();
    }

    TEST_ESP_OK(esp_wifi_init(&cfg));
    TEST_ESP_OK(esp_wifi_set_mode(WIFI_MODE_AP));
    TEST_ESP_OK(esp_wifi_set_config(WIFI_IF_AP, &w_config));
    TEST_ESP_OK(esp_wifi_start());
}

static void start_wifi_as_sta(void)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.nvs_enable = false;

    // do not auto connect
    wifi_event_handler_flag |= EVENT_HANDLER_FLAG_DO_NOT_AUTO_RECONNECT;
    TEST_ESP_OK(esp_event_loop_init(event_handler, NULL));

    // can't deinit event loop, need to reset leak check
    unity_reset_leak_checks();

    if (wifi_events == NULL) {
        wifi_events = xEventGroupCreate();
    } else {
        xEventGroupClearBits(wifi_events, 0x00ffffff);
    }

    TEST_ESP_OK(esp_wifi_init(&cfg));
    TEST_ESP_OK(esp_wifi_set_mode(WIFI_MODE_STA));
    TEST_ESP_OK(esp_wifi_start());

}

static void stop_wifi(void)
{
    printf("stop wifi\n");
    TEST_ESP_OK(esp_wifi_stop());
    TEST_ESP_OK(esp_wifi_deinit());
    if (wifi_events) {
        vEventGroupDelete(wifi_events);
        wifi_events = NULL;
    }
    vTaskDelay(1000/portTICK_PERIOD_MS);
}

static void receive_ds2ds_packet(void)
{
    test_case_uses_tcpip();
    start_wifi_as_softap();
    unity_wait_for_signal("sender ready");
    unity_send_signal("receiver ready");

    // wait for sender to send packets
    vTaskDelay(1000/portTICK_PERIOD_MS);
    stop_wifi();
}

static const char ds2ds_pdu[] = {
    0x48, 0x03, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xE8, 0x65, 0xD4, 0xCB, 0x74, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x60, 0x94, 0xE8, 0x65, 0xD4, 0xCB, 0x74, 0x1C, 0x26, 0xB9,
    0x0D, 0x02, 0x7D, 0x13, 0x00, 0x00, 0x01, 0xE8, 0x65, 0xD4, 0xCB, 0x74,
    0x1C, 0x00, 0x00, 0x26, 0xB9, 0x00, 0x00, 0x00, 0x00
};

static void send_ds2ds_packet(void)
{
    test_case_uses_tcpip();
    start_wifi_as_softap();
    unity_send_signal("sender ready");
    unity_wait_for_signal("receiver ready");

    // send packet 20 times to make sure receiver will get this packet
    for (uint16_t i = 0; i < 20; i++) {
        esp_wifi_80211_tx(ESP_IF_WIFI_AP, ds2ds_pdu, sizeof(ds2ds_pdu), true);
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    stop_wifi();
}

TEST_CASE_MULTIPLE_DEVICES("receive ds2ds packet without exception", "[wifi][test_env=UT_T2_1]", receive_ds2ds_packet, send_ds2ds_packet);

static void wifi_connect_by_bssid(uint8_t *bssid)
{
    EventBits_t bits;

    wifi_config_t w_config = {
        .sta.ssid = DEFAULT_SSID,
        .sta.password = DEFAULT_PWD,
        .sta.bssid_set = true,
    };

    memcpy(w_config.sta.bssid, bssid, 6);

    TEST_ESP_OK(esp_wifi_set_config(WIFI_IF_STA, &w_config));
    TEST_ESP_OK(esp_wifi_connect());
    bits = xEventGroupWaitBits(wifi_events, GOT_IP_EVENT, 1, 0, 5000/portTICK_RATE_MS);
    TEST_ASSERT(bits == GOT_IP_EVENT);
}

static void test_wifi_connection_sta(void)
{
    char mac_str[19];
    uint8_t mac[6];
    EventBits_t bits;

    test_case_uses_tcpip();

    start_wifi_as_sta();

    unity_wait_for_signal_param("SoftAP mac", mac_str, 19);

    TEST_ASSERT_TRUE(unity_util_convert_mac_from_string(mac_str, mac));

    wifi_connect_by_bssid(mac);

    unity_send_signal("STA connected");

    bits = xEventGroupWaitBits(wifi_events, DISCONNECT_EVENT, 1, 0, 60000 / portTICK_RATE_MS);
    // disconnect event not triggered
    printf("wait finish\n");
    TEST_ASSERT(bits == 0);

    stop_wifi();
}

static void test_wifi_connection_softap(void)
{
    char mac_str[19] = {0};
    uint8_t mac[6];

    test_case_uses_tcpip();

    start_wifi_as_softap();

    TEST_ESP_OK(esp_wifi_get_mac(ESP_IF_WIFI_AP, mac));
    sprintf(mac_str, MACSTR, MAC2STR(mac));

    unity_send_signal_param("SoftAP mac", mac_str);

    unity_wait_for_signal("STA connected");

    vTaskDelay(60000 / portTICK_PERIOD_MS);

    stop_wifi();
}

TEST_CASE_MULTIPLE_DEVICES("test wifi retain connection for 60s", "[wifi][test_env=UT_T2_1][timeout=90]", test_wifi_connection_sta, test_wifi_connection_softap);
