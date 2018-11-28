/*
 Tests for the Wi-Fi
*/
#include "esp_system.h"
#include "unity.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "esp_wifi_types.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "test_utils.h"
#include "freertos/task.h"

static const char* TAG = "test_wifi";

#define DEFAULT_SSID "TEST_SSID"
#define DEFAULT_PWD "TEST_PASS"

static void start_wifi_as_softap(void)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.nvs_enable = false;

    wifi_config_t w_config = {
        .ap.ssid = "default_ssid",
        .ap.password = "default_password",
        .ap.ssid_len = 0,
        .ap.channel = 1,
        .ap.authmode = WIFI_AUTH_WPA2_PSK,
        .ap.ssid_hidden = false,
        .ap.max_connection = 4,
        .ap.beacon_interval = 100,
    };

    TEST_ESP_OK(esp_wifi_init(&cfg));
    TEST_ESP_OK(esp_wifi_set_mode(WIFI_MODE_AP));
    TEST_ESP_OK(esp_wifi_set_config(WIFI_IF_AP, &w_config));
    TEST_ESP_OK(esp_wifi_start());

}

static void stop_wifi(void)
{
    TEST_ESP_OK(esp_wifi_stop());
    TEST_ESP_OK(esp_wifi_deinit());
}

static void receive_ds2ds_packet(void)
{
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


extern esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);

static void send_ds2ds_packet(void)
{
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
