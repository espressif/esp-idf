/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <string.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "protocol_examples_common.h"
#include "example_common_private.h"

#if CONFIG_EXAMPLE_CONNECT_PPP
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_netif_ppp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#if CONFIG_EXAMPLE_CONNECT_PPP_DEVICE_USB
#include "tinyusb.h"
#include "tusb_cdc_acm.h"

static int s_itf;
static uint8_t buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE];

#else // DEVICE is UART

#include "driver/uart.h"
#define BUF_SIZE (1024)
static bool s_stop_task = false;

#endif // CONNECT_PPP_DEVICE


static const char *TAG = "example_connect_ppp";
static int s_retry_num = 0;
static EventGroupHandle_t s_event_group = NULL;
static esp_netif_t *s_netif;
static const int GOT_IPV4 = BIT0;
static const int CONNECTION_FAILED = BIT1;
#if CONFIG_EXAMPLE_CONNECT_IPV6
static const int GOT_IPV6 = BIT2;
#define CONNECT_BITS (GOT_IPV4|GOT_IPV6|CONNECTION_FAILED)
#else
#define CONNECT_BITS (GOT_IPV4|CONNECTION_FAILED)
#endif

static esp_err_t transmit(void *h, void *buffer, size_t len)
{
    ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, len, ESP_LOG_VERBOSE);
#if CONFIG_EXAMPLE_CONNECT_PPP_DEVICE_USB
    tinyusb_cdcacm_write_queue(s_itf, buffer, len);
    tinyusb_cdcacm_write_flush(s_itf, 0);
#else // DEVICE_UART
    uart_write_bytes(UART_NUM_1, buffer, len);
#endif // CONNECT_PPP_DEVICE
    return ESP_OK;
}

static esp_netif_driver_ifconfig_t driver_cfg = {
        .handle = (void *)1,    // singleton driver, just to != NULL
        .transmit = transmit,
};
const esp_netif_driver_ifconfig_t *ppp_driver_cfg = &driver_cfg;

static void on_ip_event(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{

    if (event_id == IP_EVENT_PPP_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        if (!example_is_our_netif(EXAMPLE_NETIF_DESC_PPP, event->esp_netif)) {
            return;
        }
        esp_netif_t *netif = event->esp_netif;
        esp_netif_dns_info_t dns_info;
        ESP_LOGI(TAG, "Got IPv4 event: Interface \"%s\" address: " IPSTR, esp_netif_get_desc(event->esp_netif), IP2STR(&event->ip_info.ip));
        esp_netif_get_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns_info);
        ESP_LOGI(TAG, "Main DNS server : " IPSTR, IP2STR(&dns_info.ip.u_addr.ip4));
        xEventGroupSetBits(s_event_group, GOT_IPV4);
#if CONFIG_EXAMPLE_CONNECT_IPV6
    } else if (event_id == IP_EVENT_GOT_IP6) {
        ip_event_got_ip6_t *event = (ip_event_got_ip6_t *)event_data;
        if (!example_is_our_netif(EXAMPLE_NETIF_DESC_PPP, event->esp_netif)) {
            return;
        }
        esp_ip6_addr_type_t ipv6_type = esp_netif_ip6_get_addr_type(&event->ip6_info.ip);
        ESP_LOGI(TAG, "Got IPv6 event: Interface \"%s\" address: " IPV6STR ", type: %s", esp_netif_get_desc(event->esp_netif),
                 IPV62STR(event->ip6_info.ip), example_ipv6_addr_types_to_str[ipv6_type]);
        if (ipv6_type == EXAMPLE_CONNECT_PREFERRED_IPV6_TYPE) {
            xEventGroupSetBits(s_event_group, GOT_IPV6);
        }
#endif
    } else if (event_id == IP_EVENT_PPP_LOST_IP) {
        ESP_LOGI(TAG, "Disconnect from PPP Server");
        s_retry_num++;
        if (s_retry_num > CONFIG_EXAMPLE_PPP_CONN_MAX_RETRY) {
            ESP_LOGE(TAG, "PPP Connection failed %d times, stop reconnecting.", s_retry_num);
            xEventGroupSetBits(s_event_group, CONNECTION_FAILED);
        } else {
            ESP_LOGI(TAG, "PPP Connection failed %d times, try to reconnect.", s_retry_num);
            esp_netif_action_start(s_netif, 0, 0, 0);
            esp_netif_action_connected(s_netif, 0, 0, 0);
        }

    }
}

#if CONFIG_EXAMPLE_CONNECT_PPP_DEVICE_USB
static void cdc_rx_callback(int itf, cdcacm_event_t *event)
{
    size_t rx_size = 0;
    if (itf != s_itf) {
        // Not our channel
        return;
    }
    esp_err_t ret = tinyusb_cdcacm_read(itf, buf, CONFIG_TINYUSB_CDC_RX_BUFSIZE, &rx_size);
    if (ret == ESP_OK) {
        ESP_LOG_BUFFER_HEXDUMP(TAG, buf, rx_size, ESP_LOG_VERBOSE);
        // pass the received data to the network interface
        esp_netif_receive(s_netif, buf, rx_size, NULL);
    } else {
        ESP_LOGE(TAG, "Read error");
    }
}

static void line_state_changed(int itf, cdcacm_event_t *event)
{
    s_itf = itf; // use this channel for the netif communication
    ESP_LOGI(TAG, "Line state changed on channel %d", itf);
}
#else // DEVICE is UART

static void ppp_task(void *args)
{
    uart_config_t uart_config = {};
    uart_config.baud_rate = CONFIG_EXAMPLE_CONNECT_UART_BAUDRATE;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity    = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.source_clk = UART_SCLK_DEFAULT;

    QueueHandle_t event_queue;
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, BUF_SIZE, 0, 16, &event_queue, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, CONFIG_EXAMPLE_CONNECT_UART_TX_PIN, CONFIG_EXAMPLE_CONNECT_UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_set_rx_timeout(UART_NUM_1, 1));

    char *buffer = (char*)malloc(BUF_SIZE);
    uart_event_t event;
    esp_event_handler_register(IP_EVENT, IP_EVENT_PPP_GOT_IP, esp_netif_action_connected, s_netif);
    esp_netif_action_start(s_netif, 0, 0, 0);
    esp_netif_action_connected(s_netif, 0, 0, 0);
    while (!s_stop_task) {
        xQueueReceive(event_queue, &event, pdMS_TO_TICKS(1000));
        if (event.type == UART_DATA) {
            size_t len;
            uart_get_buffered_data_len(UART_NUM_1, &len);
            if (len) {
                len = uart_read_bytes(UART_NUM_1, buffer, BUF_SIZE, 0);
                ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, len, ESP_LOG_VERBOSE);
                esp_netif_receive(s_netif, buffer, len, NULL);
            }
        } else {
            ESP_LOGW(TAG, "Received UART event: %d", event.type);
        }
    }
    free(buffer);
    vTaskDelete(NULL);
}

#endif // CONNECT_PPP_DEVICE

esp_err_t example_ppp_connect(void)
{
    ESP_LOGI(TAG, "Start example_connect.");

#if CONFIG_EXAMPLE_CONNECT_PPP_DEVICE_USB
    ESP_LOGI(TAG, "USB initialization");
    const tinyusb_config_t tusb_cfg = {
            .device_descriptor = NULL,
            .string_descriptor = NULL,
            .external_phy = false,
            .configuration_descriptor = NULL,
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    tinyusb_config_cdcacm_t acm_cfg = {
            .usb_dev = TINYUSB_USBDEV_0,
            .cdc_port = TINYUSB_CDC_ACM_0,
            .callback_rx = &cdc_rx_callback,
            .callback_rx_wanted_char = NULL,
            .callback_line_state_changed = NULL,
            .callback_line_coding_changed = NULL
    };

    ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));
    /* the second way to register a callback */
    ESP_ERROR_CHECK(tinyusb_cdcacm_register_callback(
            TINYUSB_CDC_ACM_0,
            CDC_EVENT_LINE_STATE_CHANGED,
            &line_state_changed));
#endif // CONFIG_EXAMPLE_CONNECT_PPP_DEVICE_USB

    s_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, on_ip_event, NULL));

    esp_netif_inherent_config_t base_netif_cfg = ESP_NETIF_INHERENT_DEFAULT_PPP();
    base_netif_cfg.if_desc = EXAMPLE_NETIF_DESC_PPP;
    esp_netif_config_t netif_ppp_config = { .base = &base_netif_cfg,
            .driver = ppp_driver_cfg,
            .stack = ESP_NETIF_NETSTACK_DEFAULT_PPP
    };

    s_netif = esp_netif_new(&netif_ppp_config);
    assert(s_netif);
#if CONFIG_EXAMPLE_CONNECT_PPP_DEVICE_USB
    esp_netif_action_start(s_netif, 0, 0, 0);
    esp_netif_action_connected(s_netif, 0, 0, 0);
#else // DEVICE is UART
    s_stop_task = false;
    if (xTaskCreate(ppp_task, "ppp connect", 4096, NULL, 5, NULL) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to create a ppp connection task");
        return ESP_FAIL;
    }
#endif // CONNECT_PPP_DEVICE

    ESP_LOGI(TAG, "Waiting for IP address");
    EventBits_t bits = xEventGroupWaitBits(s_event_group, CONNECT_BITS, pdFALSE, pdFALSE, portMAX_DELAY);
    if (bits & CONNECTION_FAILED) {
        ESP_LOGE(TAG, "Connection failed!");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Connected!");

    return ESP_OK;
}

void example_ppp_shutdown(void)
{
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, ESP_EVENT_ANY_ID, on_ip_event));
#if CONFIG_EXAMPLE_CONNECT_PPP_DEVICE_UART
    s_stop_task = true;
    vTaskDelay(pdMS_TO_TICKS(1000)); // wait for the ppp task to stop
#endif

    esp_netif_action_disconnected(s_netif, 0, 0, 0);

    vEventGroupDelete(s_event_group);
    esp_netif_action_stop(s_netif, 0, 0, 0);
    esp_netif_destroy(s_netif);
    s_netif = NULL;
    s_event_group = NULL;
}

#endif // CONFIG_EXAMPLE_CONNECT_PPP
