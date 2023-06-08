/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/* DESCRIPTION:
 * This example contains code to make ESP32-S2/S3 as a USB network Device.
 */

#include <stdio.h>
#include <esp_timer.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_private/wifi.h"
#include "nvs_flash.h"
#include "dhcpserver/dhcpserver_options.h"
#include "esp_mac.h"
#include "driver/gpio.h"

#include "tinyusb.h"
#include "tinyusb_net.h"
#include "provisioning.h"

static const char *TAG = "USB_NCM";

static EventGroupHandle_t s_event_flags;
static bool s_wifi_is_connected = false;

const int CONNECTED_BIT = BIT0;
const int DISCONNECTED_BIT = BIT1;
const int RECONFIGURE_BIT = BIT2;
const int PROV_SUCCESS_BIT = BIT3;
const int PROV_FAIL_BIT = BIT4;

static esp_netif_t *s_netif = NULL;

/**
 * WiFi -- USB bridge functionality
 */

static esp_err_t usb_recv_callback(void *buffer, uint16_t len, void* ctx)
{
    if (s_wifi_is_connected) {
        if (esp_wifi_internal_tx(ESP_IF_WIFI_STA, buffer, len) != ESP_OK) {
            ESP_LOGD(TAG, "Failed to send packet to WiFi!");
        }
    }
    return ESP_OK;
}

static void wifi_buff_free(void* buffer, void* ctx)
{
    esp_wifi_internal_free_rx_buffer(buffer);
}

static esp_err_t pkt_wifi2usb(void *buffer, uint16_t len, void *eb)
{
    if (tinyusb_net_send_sync(buffer, len, eb, pdMS_TO_TICKS(100)) != ESP_OK) {
        esp_wifi_internal_free_rx_buffer(eb);
        ESP_LOGD(TAG, "Failed to send packet to USB!");
    }
    return ESP_OK;
}

static void event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Wi-Fi STA disconnected");
        s_wifi_is_connected = false;
        esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, NULL);
        esp_wifi_connect();
        xEventGroupClearBits(s_event_flags, CONNECTED_BIT);
        xEventGroupSetBits(s_event_flags, DISCONNECTED_BIT);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        ESP_LOGI(TAG, "Wi-Fi STA connected");
        esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, pkt_wifi2usb);
        s_wifi_is_connected = true;
        xEventGroupClearBits(s_event_flags, DISCONNECTED_BIT);
        xEventGroupSetBits(s_event_flags, CONNECTED_BIT);
    }
}

static esp_err_t connect_wifi(void)
{
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    wifi_config_t wifi_cfg;
    if (esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg) != ESP_OK) {
        // configuration not available, report error to restart provisioning
        return ESP_FAIL;
    }
    esp_wifi_connect();
    EventBits_t status = xEventGroupWaitBits(s_event_flags, CONNECTED_BIT, 0, 1, 10000/portTICK_PERIOD_MS);
    if (status & CONNECTED_BIT) {
        ESP_LOGI(TAG, "WiFi station connected successfully");
        return ESP_OK;
    }
    ESP_LOGE(TAG, "WiFi station connected failed");
    return ESP_ERR_TIMEOUT;
}

static void on_usb_net_init(void *ctx)
{
    ESP_LOGE(TAG, "USB NET device has been initialized!");
}

static esp_err_t usb_ncm_wifi_bridge(void)
{
    const tinyusb_config_t tusb_cfg = {
            .external_phy = false,
    };
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    tinyusb_net_config_t net_config = {
            .on_recv_callback = usb_recv_callback,
            .free_tx_buffer = wifi_buff_free,
            .on_init_callback = on_usb_net_init
    };

    esp_read_mac(net_config.mac_addr, ESP_MAC_WIFI_STA);

    esp_err_t ret = tinyusb_net_init(TINYUSB_USBDEV_0, &net_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "USB net init but not connect wifi");
        return ret;
    }
    return ESP_OK;
}

/**
 * USB internal network functionality
 */

esp_err_t netif_recv_callback(void *buffer, uint16_t len, void* ctx)
{
    if (s_netif) {
        void *buf_copy = malloc(len);
        if (!buf_copy) {
            return ESP_ERR_NO_MEM;
        }
        memcpy(buf_copy, buffer, len);
        return esp_netif_receive(s_netif, buf_copy, len, NULL);
    }
    return ESP_OK;
}

static esp_err_t netif_transmit (void *h, void *buffer, size_t len)
{
    if (tinyusb_net_send_sync(buffer, len, NULL, pdMS_TO_TICKS(100)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send buffer to USB!");
    }
    return ESP_OK;
}

static void l2_free(void *h, void* buffer)
{
    free(buffer);
}

static esp_err_t usb_ncm_with_network(void)
{
    const tinyusb_config_t tusb_cfg = {
            .external_phy = false,
    };
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    const tinyusb_net_config_t net_config = {
            // locally administrated address for the ncm device as it's going to be used internally
            // for configuration only
            .mac_addr = {0x02, 0x02, 0x11, 0x22, 0x33, 0x01},
            .on_recv_callback = netif_recv_callback,
    };

    esp_err_t ret = tinyusb_net_init(TINYUSB_USBDEV_0, &net_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Cannot initialize USB Net device");
        return ret;
    }

    // with OUI range MAC to create a virtual netif running http server
    // this needs to be different to usb_interface_mac (==client)
    uint8_t lwip_addr[6]=  {0x02, 0x02, 0x11, 0x22, 0x33, 0x02};


    // 1) Derive the base config from the default AP (using DHCP server)
    esp_netif_inherent_config_t base_cfg = ESP_NETIF_INHERENT_DEFAULT_WIFI_AP();
    base_cfg.if_key = "usb";
    base_cfg.if_desc = "usb ncm config device";
    // 2) Use static config for driver's config pointing only to static transmit and free functions
    esp_netif_driver_ifconfig_t driver_cfg = {
            .handle = (void*)1,     // will be replaced by the driver pointer only tinyusb_net supports ti
            .transmit = netif_transmit,
            .driver_free_rx_buffer = l2_free
    };

    // Config the esp-netif with:
    //   1) inherent config (behavioural settings of an interface)
    //   2) driver's config (connection to IO functions -- usb)
    //   3) stack config (using lwip IO functions -- derive from eth)
    esp_netif_config_t cfg = {
            .base = &base_cfg,
            .driver = &driver_cfg,
            // 3) use ethernet style of lwip netif settings
            .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH
    };

    s_netif = esp_netif_new(&cfg);
    if (s_netif == NULL) {
        return ESP_FAIL;
    }
    esp_netif_set_mac(s_netif, lwip_addr);

    // set the minimum lease time
    uint32_t  lease_opt = 1;
    esp_netif_dhcps_option(s_netif, ESP_NETIF_OP_SET, IP_ADDRESS_LEASE_TIME, &lease_opt, sizeof(lease_opt));

    // start the interface manually (as the driver has been started already)
    esp_netif_action_start(s_netif, 0, 0, 0);
    return ESP_OK;
}

/**
 * GPIO button functionality
 */

#define GPIO_INPUT_IO_0     0
#define GPIO_LONG_PUSH_US   2000000  /* push for 2 seconds to reconfigure */

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    static int64_t last_pushed = -1;
    if (gpio_get_level(GPIO_INPUT_IO_0) == 0) {
        last_pushed = esp_timer_get_time();
    } else {
        uint64_t now = esp_timer_get_time();
        if (last_pushed != -1 && now - last_pushed > GPIO_LONG_PUSH_US) {
            BaseType_t high_task_wakeup;
            xEventGroupSetBitsFromISR(s_event_flags, RECONFIGURE_BIT, &high_task_wakeup);
            if (high_task_wakeup) {
                portYIELD_FROM_ISR();
            }
        }
        last_pushed = -1;
    }
}

static void gpio_init(void)
{
    gpio_config_t io_conf = { .intr_type = GPIO_INTR_ANYEDGE,
            .pin_bit_mask = (1ULL<<GPIO_INPUT_IO_0),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = 1 };
    gpio_config(&io_conf);
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, NULL);

}

/**
 * Application
 */

void app_main(void)
{
    static __NOINIT_ATTR uint32_t s_reconfigure_requested;
    static const uint32_t RECONFIGURE_REQUEST = 0x1C55AA;

    /* Check reset reason and decide if we should re-provision */
    bool do_provision = false;
    esp_reset_reason_t reason = esp_reset_reason();
    ESP_LOGW(TAG, "After restart! %d", reason);
    if (reason != ESP_RST_SW) {
        s_reconfigure_requested = 0;
    } else if (s_reconfigure_requested == RECONFIGURE_REQUEST) {
        do_provision = true;
    }

    /* Initialize NVS and WiFi */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // init the flags and event loop
    s_event_flags = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* Init the re-provisioning button (long-press with initiate provisioning restart) */
    gpio_init();

    /* Start the application in configuration mode (to perform provisioning)
     * or in a bridge mode (already provisioned) */
    if (do_provision || !is_provisioned())
    {
        ESP_LOGI(TAG, "Starting provisioning");
        ESP_ERROR_CHECK(esp_netif_init());
        // needed to complete provisioning with getting a valid IP event
        esp_netif_create_default_wifi_sta();

        // starts usb ncm with virtual network used to configure/provision the example
        usb_ncm_with_network();
        start_provisioning(&s_event_flags, PROV_SUCCESS_BIT, PROV_FAIL_BIT);
    } else {
        ESP_LOGI(TAG, "Starting USB-WiFi bridge");
        if (connect_wifi() != ESP_OK) {
            // if we cannot connect to WiFi we just try to re-provision
            xEventGroupSetBits(s_event_flags, RECONFIGURE_BIT);
        } else {
            usb_ncm_wifi_bridge();
        }
    }

    EventBits_t bits = xEventGroupWaitBits(s_event_flags, RECONFIGURE_BIT | PROV_SUCCESS_BIT | PROV_FAIL_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
    if (bits & RECONFIGURE_BIT || bits & PROV_FAIL_BIT) {
        // retry provisioning if it previously failed or if requested by the button press
        s_reconfigure_requested = RECONFIGURE_REQUEST;
    } else {
        // provisioning successfully finished, restart to the bridge mode
        s_reconfigure_requested = 0;
    }

    vTaskDelay(pdMS_TO_TICKS(1000));    // to let httpd handle the closure
    esp_restart();
}
