/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* itwt Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
   this example shows how to use itwt
   set a router or a AP using the same SSID&PASSWORD as configuration of this example.
   start esp32c6 and when it connected to AP it will setup itwt.
*/
#include <netdb.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_console.h"
#include "cmd_system.h"
#include "esp_wifi_he.h"
#include "esp_pm.h"
#include "esp_timer.h"

#include "wifi_cmd.h"

/*******************************************************
 *                Constants
 *******************************************************/
static const char *TAG = "itwt";

/*******************************************************
 *                Structures
 *******************************************************/

/*******************************************************
 *                Variable Definitions
 *******************************************************/
/*set the ssid and password via "idf.py menuconfig"*/
#define DEFAULT_SSID CONFIG_EXAMPLE_WIFI_SSID
#define DEFAULT_PWD CONFIG_EXAMPLE_WIFI_PASSWORD
#define ITWT_SETUP_SUCCESS 1

#if CONFIG_EXAMPLE_ITWT_TRIGGER_ENABLE
uint8_t trigger_enabled = 1;
#else
uint8_t trigger_enabled = 0;
#endif

#if CONFIG_EXAMPLE_ITWT_ANNOUNCED
uint8_t flow_type_announced = 1;
#else
uint8_t flow_type_announced = 0;
#endif

esp_netif_t *netif_sta = NULL;
const int CONNECTED_BIT = BIT0;
const int DISCONNECTED_BIT = BIT1;
EventGroupHandle_t wifi_event_group;

/*******************************************************
 *                Function Declarations
 *******************************************************/

/*******************************************************
 *                Function Definitions
 *******************************************************/

static void example_set_static_ip(esp_netif_t *netif)
{
#if CONFIG_EXAMPLE_ENABLE_STATIC_IP
    if (esp_netif_dhcpc_stop(netif) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to stop dhcp client");
        return;
    }
    esp_netif_ip_info_t ip;
    memset(&ip, 0 , sizeof(esp_netif_ip_info_t));
    ip.ip.addr = ipaddr_addr(CONFIG_EXAMPLE_STATIC_IP_ADDR);
    ip.netmask.addr = ipaddr_addr(CONFIG_EXAMPLE_STATIC_NETMASK_ADDR);
    ip.gw.addr = ipaddr_addr(CONFIG_EXAMPLE_STATIC_GW_ADDR);
    if (esp_netif_set_ip_info(netif, &ip) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set ip info");
        return;
    }
    ESP_LOGI(TAG, "Success to set static ip: %s, netmask: %s, gw: %s",
             CONFIG_EXAMPLE_STATIC_IP_ADDR, CONFIG_EXAMPLE_STATIC_NETMASK_ADDR, CONFIG_EXAMPLE_STATIC_GW_ADDR);
#endif
}

static const char *itwt_probe_status_to_str(wifi_itwt_probe_status_t status)
{
    switch (status) {
    case ITWT_PROBE_FAIL:                 return "itwt probe fail";
    case ITWT_PROBE_SUCCESS:              return "itwt probe success";
    case ITWT_PROBE_TIMEOUT:              return "itwt probe timeout";
    case ITWT_PROBE_STA_DISCONNECTED:     return "Sta disconnected";
    default:                              return "Unknown status";
    }
}

static void got_ip_handler(void *arg, esp_event_base_t event_base,
                           int32_t event_id, void *event_data)
{
    xEventGroupClearBits(wifi_event_group, DISCONNECTED_BIT);
    xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);

    /* setup a trigger-based announce individual TWT agreement. */
    wifi_phy_mode_t phymode;
    wifi_config_t sta_cfg = { 0, };
    esp_wifi_get_config(WIFI_IF_STA, &sta_cfg);
    esp_wifi_sta_get_negotiated_phymode(&phymode);
    if (phymode == WIFI_PHY_MODE_HE20) {
        esp_err_t err = ESP_OK;
        wifi_twt_setup_config_t setup_config = {
            .setup_cmd = TWT_REQUEST,
            .flow_id = 0,
            .twt_id = CONFIG_EXAMPLE_ITWT_ID,
            .flow_type = flow_type_announced ? 0 : 1,
            .min_wake_dura = CONFIG_EXAMPLE_ITWT_MIN_WAKE_DURA,
            .wake_duration_unit = CONFIG_EXAMPLE_ITWT_WAKE_DURATION_UNIT,
            .wake_invl_expn = CONFIG_EXAMPLE_ITWT_WAKE_INVL_EXPN,
            .wake_invl_mant = CONFIG_EXAMPLE_ITWT_WAKE_INVL_MANT,
            .trigger = trigger_enabled,
            .timeout_time_ms = CONFIG_EXAMPLE_ITWT_SETUP_TIMEOUT_TIME_MS,
        };
        err = esp_wifi_sta_itwt_setup(&setup_config);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "itwt setup failed, err:0x%x", err);
        }
    } else {
        ESP_LOGE(TAG, "Must be in 11ax mode to support itwt");
    }
}

static void start_handler(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "sta connect to %s", DEFAULT_SSID);
    esp_wifi_connect();
}

static void disconnect_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "sta disconnect, reconnect...");
    xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
    esp_wifi_connect();
}

static void itwt_setup_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    wifi_event_sta_itwt_setup_t *setup = (wifi_event_sta_itwt_setup_t *) event_data;
    if (setup->status == ITWT_SETUP_SUCCESS) {
        /* TWT Wake Interval = TWT Wake Interval Mantissa * (2 ^ TWT Wake Interval Exponent) */
        ESP_LOGI(TAG, "<WIFI_EVENT_ITWT_SETUP>twt_id:%d, flow_id:%d, %s, %s, wake_dura:%d, wake_dura_unit:%d, wake_invl_e:%d, wake_invl_m:%d", setup->config.twt_id,
                setup->config.flow_id, setup->config.trigger ? "trigger-enabled" : "non-trigger-enabled", setup->config.flow_type ? "unannounced" : "announced",
                setup->config.min_wake_dura, setup->config.wake_duration_unit, setup->config.wake_invl_expn, setup->config.wake_invl_mant);
        ESP_LOGI(TAG, "<WIFI_EVENT_ITWT_SETUP>target wake time:%lld, wake duration:%d us, service period:%d us", setup->target_wake_time, setup->config.min_wake_dura << (setup->config.wake_duration_unit == 1 ? 10 : 8),
                setup->config.wake_invl_mant << setup->config.wake_invl_expn);
    } else {
        if (setup->status == ESP_ERR_WIFI_TWT_SETUP_TIMEOUT) {
            ESP_LOGE(TAG, "<WIFI_EVENT_ITWT_SETUP>twt_id:%d, timeout of receiving twt setup response frame", setup->config.twt_id);
        } else if (setup->status == ESP_ERR_WIFI_TWT_SETUP_TXFAIL) {
            ESP_LOGE(TAG, "<WIFI_EVENT_ITWT_SETUP>twt_id:%d, twt setup frame tx failed, reason: %d", setup->config.twt_id, setup->reason);
        } else if (setup->status == ESP_ERR_WIFI_TWT_SETUP_REJECT) {
            ESP_LOGE(TAG, "<WIFI_EVENT_ITWT_SETUP>twt_id:%d, twt setup request was rejected, setup cmd: %d", setup->config.twt_id, setup->config.setup_cmd);
        } else {
            ESP_LOGE(TAG, "<WIFI_EVENT_ITWT_SETUP>twt_id:%d, twt setup failed, status: %d", setup->config.twt_id, setup->status);
        }
    }
}

static void itwt_teardown_handler(void *arg, esp_event_base_t event_base,
                                  int32_t event_id, void *event_data)
{
    wifi_event_sta_itwt_teardown_t *teardown = (wifi_event_sta_itwt_teardown_t *) event_data;
    ESP_LOGI(TAG, "<WIFI_EVENT_ITWT_TEARDOWN>flow_id %d%s", teardown->flow_id, (teardown->flow_id == 8) ? "(all twt)" : "");
}

static void itwt_suspend_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
    wifi_event_sta_itwt_suspend_t *suspend = (wifi_event_sta_itwt_suspend_t *) event_data;
    ESP_LOGI(TAG, "<WIFI_EVENT_ITWT_SUSPEND>status:%d, flow_id_bitmap:0x%x, actual_suspend_time_ms:[%lu %lu %lu %lu %lu %lu %lu %lu]",
             suspend->status, suspend->flow_id_bitmap,
             suspend->actual_suspend_time_ms[0], suspend->actual_suspend_time_ms[1], suspend->actual_suspend_time_ms[2], suspend->actual_suspend_time_ms[3],
             suspend->actual_suspend_time_ms[4], suspend->actual_suspend_time_ms[5], suspend->actual_suspend_time_ms[6], suspend->actual_suspend_time_ms[7]);
}

static void itwt_probe_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    wifi_event_sta_itwt_probe_t *probe = (wifi_event_sta_itwt_probe_t *) event_data;
    ESP_LOGI(TAG, "<WIFI_EVENT_ITWT_PROBE>status:%s, reason:0x%x", itwt_probe_status_to_str(probe->status), probe->reason);
}

static void wifi_itwt(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    netif_sta = esp_netif_create_default_wifi_sta();
    assert(netif_sta);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                    WIFI_EVENT_STA_START,
                    &start_handler,
                    NULL,
                    NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                    WIFI_EVENT_STA_DISCONNECTED,
                    &disconnect_handler,
                    NULL,
                    NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                    IP_EVENT_STA_GOT_IP,
                    &got_ip_handler,
                    NULL,
                    NULL));
    /* itwt */
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                    WIFI_EVENT_ITWT_SETUP,
                    &itwt_setup_handler,
                    NULL,
                    NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                    WIFI_EVENT_ITWT_TEARDOWN,
                    &itwt_teardown_handler,
                    NULL,
                    NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                    WIFI_EVENT_ITWT_SUSPEND,
                    &itwt_suspend_handler,
                    NULL,
                    NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                    WIFI_EVENT_ITWT_PROBE,
                    &itwt_probe_handler,
                    NULL,
                    NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = DEFAULT_SSID,
            .password = DEFAULT_PWD,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW_HT20);
    esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_11AX);
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);

#if CONFIG_EXAMPLE_ENABLE_STATIC_IP
    example_set_static_ip(netif_sta);
#endif

    ESP_ERROR_CHECK(esp_wifi_start());

#if CONFIG_ESP_WIFI_ENABLE_WIFI_RX_STATS
#if CONFIG_ESP_WIFI_ENABLE_WIFI_RX_MU_STATS
    esp_wifi_enable_rx_statistics(true, true);
#else
    esp_wifi_enable_rx_statistics(true, false);
#endif
#endif
#if CONFIG_ESP_WIFI_ENABLE_WIFI_TX_STATS
    esp_wifi_enable_tx_statistics(ESP_WIFI_ACI_VO, true); //VO, mgmt
    esp_wifi_enable_tx_statistics(ESP_WIFI_ACI_BE, true); //BE, data
#endif

}

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

#if CONFIG_PM_ENABLE
    // Configure dynamic frequency scaling:
    // maximum and minimum frequencies are set in sdkconfig,
    // automatic light sleep is enabled if tickless idle support is enabled.
    esp_pm_config_t pm_config = {
        .max_freq_mhz = CONFIG_EXAMPLE_MAX_CPU_FREQ_MHZ,
        .min_freq_mhz = CONFIG_EXAMPLE_MIN_CPU_FREQ_MHZ,
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
        .light_sleep_enable = true
#endif
    };
    ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
    ESP_ERROR_CHECK( ret );

#else
    printf("\n =================================================\n");
    printf(" |                   Test WiFi 6 itwt             |\n");
    printf(" =================================================\n\n");

    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = "itwt>";

    // init console REPL environment
#if CONFIG_ESP_CONSOLE_UART
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));
#elif CONFIG_ESP_CONSOLE_USB_CDC
    esp_console_dev_usb_cdc_config_t cdc_config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_cdc(&cdc_config, &repl_config, &repl));
#elif CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    esp_console_dev_usb_serial_jtag_config_t usbjtag_config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_serial_jtag(&usbjtag_config, &repl_config, &repl));
#endif

    // start console REPL
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
#endif

    //start wifi
    wifi_itwt();
    // register commands
    register_system();
    register_wifi_itwt();
    register_wifi_stats();

}
