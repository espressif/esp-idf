/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"

#if !CONFIG_ETH_USE_ESP32_EMAC
#error "test_app_emac requires CONFIG_ETH_USE_ESP32_EMAC"
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_eth_mac_esp.h"
#include "esp_eth_phy.h"
#include "esp_eth_test_emac_utils.h"

// Local override of TEST_ASSERT and TEST_ESP_OK to fix Unity file name reporting
// when assertions are in a different file than the test.
#define ETH_TEST_ASSERT(condition) do { \
    const char* _unity_test_file_save = Unity.TestFile; \
    Unity.TestFile = __FILE__; \
    TEST_ASSERT(condition); \
    Unity.TestFile = _unity_test_file_save; \
} while(0)

#define ETH_TEST_ESP_OK(rc) do { \
    const char* _unity_test_file_save = Unity.TestFile; \
    Unity.TestFile = __FILE__; \
    TEST_ASSERT_EQUAL_HEX32(ESP_OK, rc); \
    Unity.TestFile = _unity_test_file_save; \
} while(0)

static const char *TAG = "emac_test_utils";

static EventGroupHandle_t s_eth_event_group;
static esp_eth_handle_t s_eth_handle;
static esp_netif_t *s_eth_netif;
static esp_eth_netif_glue_handle_t s_eth_glue;
static void *s_memory_p[MAX_HEAP_ALLOCATION_POINTERS];

static bool skip_setup_teardown(void)
{
    return Unity.CurrentDetail1 != NULL && strstr(Unity.CurrentDetail1, "[skip_setup_teardown]") != NULL;
}

static bool use_esp_netif(void)
{
    return Unity.CurrentDetail1 != NULL && strstr(Unity.CurrentDetail1, "[esp-netif]") != NULL;
}

static esp_err_t emac_test_eth_init(esp_eth_handle_t *eth_handle)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mac_t *mac = NULL;
    esp_eth_phy_t *phy = NULL;

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_esp32_emac_config_t emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();

    phy_config.reset_gpio_num = CONFIG_EMAC_TEST_PHY_RST_GPIO;
#if CONFIG_EMAC_TEST_PHY_HOLD_RESET_GPIO
    phy_config.hold_reset_gpio = true;
#endif

#if CONFIG_EMAC_TEST_SLEEP_MODE_ALLOW_PD
    emac_config.sleep_mode = EMAC_SLEEP_MODE_ALLOW_PD;
#elif CONFIG_EMAC_TEST_SLEEP_MODE_MAC_WAKE_ON_LAN
    emac_config.sleep_mode = EMAC_SLEEP_MODE_MAC_WAKE_ON_LAN;
#elif CONFIG_EMAC_TEST_SLEEP_MODE_NO_SLEEP
    emac_config.sleep_mode = EMAC_SLEEP_MODE_NO_SLEEP;
#endif

#if CONFIG_EMAC_TEST_RMII_CLK_OUTPUT
    emac_config.interface = EMAC_DATA_INTERFACE_RMII;
    emac_config.clock_config.rmii.clock_mode = EMAC_CLK_OUT;
    emac_config.clock_config.rmii.clock_gpio = CONFIG_EMAC_TEST_RMII_CLK_GPIO;
#if CONFIG_EMAC_TEST_RMII_CLK_EXT_LOOPBACK_EN && !SOC_EMAC_RMII_CLK_OUT_INTERNAL_LOOPBACK
    emac_config.clock_config_out_in.rmii.clock_mode = EMAC_CLK_EXT_IN;
    emac_config.clock_config_out_in.rmii.clock_gpio = CONFIG_EMAC_TEST_RMII_CLK_EXT_LOOPBACK_IN_GPIO;
#endif
#endif

    mac = esp_eth_mac_new_esp32(&emac_config, &mac_config);
    ESP_GOTO_ON_FALSE(mac, ESP_FAIL, err, TAG, "create ESP32 EMAC instance failed");
    phy = esp_eth_phy_new_generic(&phy_config);
    ESP_GOTO_ON_FALSE(phy, ESP_FAIL, err, TAG, "create generic PHY instance failed");

    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    ESP_GOTO_ON_ERROR(esp_eth_driver_install(&eth_config, eth_handle), err, TAG, "Ethernet driver install failed");
    return ESP_OK;

err:
    if (phy) {
        phy->del(phy);
    }
    if (mac) {
        mac->del(mac);
    }
    return ret;
}

static esp_err_t emac_test_eth_deinit(esp_eth_handle_t eth_handle)
{
    esp_eth_mac_t *mac = NULL;
    esp_eth_phy_t *phy = NULL;
    esp_eth_get_mac_instance(eth_handle, &mac);
    esp_eth_get_phy_instance(eth_handle, &phy);
    esp_err_t ret = esp_eth_driver_uninstall(eth_handle);
    if (mac != NULL) {
        mac->del(mac);
    }
    if (phy != NULL) {
        phy->del(phy);
    }
    return ret;
}

void setUp(void)
{
    if (skip_setup_teardown()) {
        return;
    }
    s_eth_event_group = xEventGroupCreate();
    ETH_TEST_ASSERT(s_eth_event_group != NULL);
    ETH_TEST_ESP_OK(esp_event_loop_create_default());
    ETH_TEST_ESP_OK(emac_test_eth_init(&s_eth_handle));
    if (use_esp_netif()) {
        esp_netif_init();
        esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
        s_eth_netif = esp_netif_new(&netif_cfg);
        s_eth_glue = esp_eth_new_netif_glue(s_eth_handle);
        ETH_TEST_ESP_OK(esp_netif_attach(s_eth_netif, s_eth_glue));
        ETH_TEST_ESP_OK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &eth_test_got_ip_event_handler, s_eth_event_group));
    }
    ETH_TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_test_default_event_handler, s_eth_event_group));
}

void tearDown(void)
{
    eth_test_free_all();

    if (skip_setup_teardown()) {
        return;
    }

    esp_err_t ret;
    if (s_eth_glue != NULL) {
        ret = esp_eth_del_netif_glue(s_eth_glue);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "esp_eth_del_netif_glue failed: %s", esp_err_to_name(ret));
        }
        s_eth_glue = NULL;
    }
    if (s_eth_netif != NULL) {
        esp_netif_destroy(s_eth_netif);
        s_eth_netif = NULL;
        ret = esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, eth_test_got_ip_event_handler);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "esp_event_handler_unregister IP event failed: %s", esp_err_to_name(ret));
        }
    }

    ret = esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_test_default_event_handler);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "esp_event_handler_unregister failed: %s", esp_err_to_name(ret));
    }

    if (s_eth_handle != NULL) {
        ret = emac_test_eth_deinit(s_eth_handle);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "emac_test_eth_deinit failed: %s", esp_err_to_name(ret));
            ESP_LOGI(TAG, "Trying to stop Ethernet driver and deinitialize it again...");
            ret = esp_eth_stop(s_eth_handle);
            vTaskDelay(pdMS_TO_TICKS(500));
            if (ret != ESP_OK) {
                ESP_LOGW(TAG, "esp_eth_stop failed: %s", esp_err_to_name(ret));
            }
            ret = emac_test_eth_deinit(s_eth_handle);
            if (ret != ESP_OK) {
                ESP_LOGW(TAG, "emac_test_eth_deinit failed: %s", esp_err_to_name(ret));
            }
        }
        s_eth_handle = NULL;
    }

    ret = esp_event_loop_delete_default();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "esp_event_loop_delete_default failed: %s", esp_err_to_name(ret));
    }

    if (s_eth_event_group != NULL) {
        vEventGroupDelete(s_eth_event_group);
        s_eth_event_group = NULL;
    }
}

esp_eth_handle_t eth_test_get_eth_handle(void)
{
    return s_eth_handle;
}

EventGroupHandle_t eth_test_get_default_event_group(void)
{
    return s_eth_event_group;
}

esp_netif_t *eth_test_get_netif(void)
{
    return s_eth_netif;
}

void *eth_test_alloc(size_t size)
{
    for (int i = 0; i < MAX_HEAP_ALLOCATION_POINTERS; i++) {
        if (s_memory_p[i] == NULL) {
            s_memory_p[i] = malloc(size);
            return s_memory_p[i];
        }
    }
    return NULL;
}

void eth_test_free(void *ptr)
{
    for (int i = 0; i < MAX_HEAP_ALLOCATION_POINTERS; i++) {
        if (s_memory_p[i] == ptr) {
            free(s_memory_p[i]);
            s_memory_p[i] = NULL;
            return;
        }
    }
}

void eth_test_free_all(void)
{
    for (int i = 0; i < MAX_HEAP_ALLOCATION_POINTERS; i++) {
        if (s_memory_p[i] != NULL) {
            free(s_memory_p[i]);
            s_memory_p[i] = NULL;
        }
    }
}

void eth_test_default_event_handler(void *arg, esp_event_base_t event_base,
                                    int32_t event_id, void *event_data)
{
    (void)event_base;
    (void)event_data;
    EventGroupHandle_t eth_event_group = (EventGroupHandle_t)arg;
    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Up");
        xEventGroupSetBits(eth_event_group, ETH_CONNECT_BIT);
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Down");
        break;
    case ETHERNET_EVENT_START:
        ESP_LOGI(TAG, "Ethernet Started");
        xEventGroupSetBits(eth_event_group, ETH_START_BIT);
        break;
    case ETHERNET_EVENT_STOP:
        ESP_LOGI(TAG, "Ethernet Stopped");
        xEventGroupSetBits(eth_event_group, ETH_STOP_BIT);
        break;
    default:
        break;
    }
}

void eth_test_got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data)
{
    (void)event_base;
    (void)event_id;
    EventGroupHandle_t eth_event_group = (EventGroupHandle_t)arg;
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;
    ESP_LOGI(TAG, "Ethernet Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    xEventGroupSetBits(eth_event_group, ETH_GOT_IP_BIT);
}
