/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include <inttypes.h>
#include "esp_eth_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_eth_test_utils.h"
#include "esp_check.h"
#include "ethernet_init.h"

// Local override of TEST_ASSERT and TEST_ESP_OK to fix Unity file name reporting
// when assertions are in a different file than the test. This temporarily sets Unity.
// TestFile to __FILE__ so the correct file name is reported on assertion failure.
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

static const char *TAG = "esp32_eth_test_common";

static esp_eth_handle_t *s_eth_handles; // only because we use Ethernet Init component

static EventGroupHandle_t s_eth_event_group;
static esp_eth_handle_t s_eth_handle;
static esp_netif_t *s_eth_netif;
static esp_eth_netif_glue_handle_t s_eth_glue;
static void *s_memory_p[MAX_HEAP_ALLOCATION_POINTERS];

esp_err_t eth_test_set_phy_reg_bits(esp_eth_handle_t eth_handle, uint32_t reg_addr, uint32_t bitmask, uint32_t max_attempts)
{

    esp_eth_phy_reg_rw_data_t reg = {
        .reg_addr = reg_addr,
        .reg_value_p = NULL
    };
    uint32_t reg_value, reg_value_rb;

    for (uint32_t i = 0; i < max_attempts; i++) {
        reg.reg_value_p = &reg_value;
        esp_err_t ret = esp_eth_ioctl(eth_handle, ETH_CMD_READ_PHY_REG, &reg);
        if (ret != ESP_OK) {
            return ret;
        }
        reg_value |= bitmask;
        ret = esp_eth_ioctl(eth_handle, ETH_CMD_WRITE_PHY_REG, &reg);
        if (ret != ESP_OK) {
            return ret;
        }
        reg.reg_value_p = &reg_value_rb;
        ret = esp_eth_ioctl(eth_handle, ETH_CMD_READ_PHY_REG, &reg);
        if (ret != ESP_OK) {
            return ret;
        }
        // Check if the write was successful
        if ((reg_value_rb & bitmask) == bitmask) {
            return ESP_OK;
        }
        // Add delay only if not the last attempt
        if (i < max_attempts - 1) {
            ESP_LOGW(TAG, "Setting PHY register %04"PRIx32" failed, retrying... (attempt %"PRIu32" of %"PRIu32")", reg_addr, i + 1, max_attempts);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    return ESP_ERR_TIMEOUT;
}

esp_err_t eth_test_clear_phy_reg_bits(esp_eth_handle_t eth_handle, uint32_t reg_addr, uint32_t bitmask, uint32_t max_attempts)
{
    esp_eth_phy_reg_rw_data_t reg = {
        .reg_addr = reg_addr,
        .reg_value_p = NULL
    };
    uint32_t reg_value, reg_value_rb;

    for (uint32_t i = 0; i < max_attempts; i++) {
        reg.reg_value_p = &reg_value;
        esp_err_t ret = esp_eth_ioctl(eth_handle, ETH_CMD_READ_PHY_REG, &reg);
        if (ret != ESP_OK) {
            return ret;
        }
        reg_value &= ~bitmask;
        ret = esp_eth_ioctl(eth_handle, ETH_CMD_WRITE_PHY_REG, &reg);
        if (ret != ESP_OK) {
            return ret;
        }
        reg.reg_value_p = &reg_value_rb;
        ret = esp_eth_ioctl(eth_handle, ETH_CMD_READ_PHY_REG, &reg);
        if (ret != ESP_OK) {
            return ret;
        }
        // Check if the write was successful
        if ((reg_value_rb & bitmask) == 0) {
            return ESP_OK;
        }
        // Add delay only if not the last attempt
        if (i < max_attempts - 1) {
            ESP_LOGW(TAG, "Clearing PHY register %04"PRIx32" failed, retrying... (attempt %"PRIu32" of %"PRIu32")", reg_addr, i + 1, max_attempts);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    return ESP_ERR_TIMEOUT;
}

__attribute__((weak)) esp_err_t esp_eth_test_eth_init(esp_eth_handle_t *eth_handle)
{
    esp_err_t ret = ESP_OK;
    uint8_t eth_port_cnt = 0;
    ESP_LOGI(TAG, "Default Ethernet Initialization...");
    ESP_GOTO_ON_ERROR(ethernet_init_all(&s_eth_handles, &eth_port_cnt), err, TAG, "Failed to initialize Ethernet");
    ESP_GOTO_ON_FALSE(eth_port_cnt == 1, ESP_FAIL, err, TAG, "Multiple Ethernet devices detected, the test cannot continue...");
    *eth_handle = s_eth_handles[0];
err:
    return ret;
}

__attribute__((weak)) esp_err_t esp_eth_test_eth_deinit(esp_eth_handle_t eth_handle)
{
    esp_err_t ret = ESP_OK;
    ESP_LOGI(TAG, "Default Ethernet Deinitialization...");
    ESP_GOTO_ON_FALSE(s_eth_handles[0] == eth_handle, ESP_FAIL, err, TAG, "Ethernet handle does not match");
    ESP_GOTO_ON_ERROR(ethernet_deinit_all(s_eth_handles), err, TAG, "Failed to deinitialize Ethernet");
err:
    return ret;
}

void setUp(void)
{
    if (strstr(Unity.CurrentDetail1, "[skip_setup_teardown]") != NULL) {
        return;
    }
    // Use Unity to check if the initialization is successful, it's intended to fail the test if initialization fails
    s_eth_event_group = xEventGroupCreate();
    ETH_TEST_ASSERT(s_eth_event_group != NULL);
    ETH_TEST_ESP_OK(esp_event_loop_create_default());
    ETH_TEST_ESP_OK(esp_eth_test_eth_init(&s_eth_handle));
    eth_dev_info_t phy_info = ethernet_init_get_dev_info(s_eth_handle);
    ESP_LOGI(TAG, "DUT PHY: %s", phy_info.name);
    if (strstr(Unity.CurrentDetail1, "[esp-netif]") != NULL) {
        esp_netif_init();
        // create TCP/IP netif
        esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
        s_eth_netif = esp_netif_new(&netif_cfg);
        // combine driver with netif
        s_eth_glue = esp_eth_new_netif_glue(s_eth_handle);
        ETH_TEST_ESP_OK(esp_netif_attach(s_eth_netif, s_eth_glue));
        // register user defined event handlers
        ETH_TEST_ESP_OK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &eth_test_got_ip_event_handler, s_eth_event_group));
    }
    ETH_TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_test_default_event_handler, s_eth_event_group));
}

void tearDown(void)
{
    eth_test_free_all();

    if (strstr(Unity.CurrentDetail1, "[skip_setup_teardown]") != NULL) {
        return;
    }
    // Use regular error checking instead of ETH_TEST_ESP_OK to ensure cleanup always completes
    // even if errors occur (e.g., if test failed and resources are in invalid state)
    // This is critical because TEST_ASSERT failures use longjmp which can skip cleanup
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
        // ESP_ERR_INVALID_STATE is OK - handler might already be unregistered
        ESP_LOGW(TAG, "esp_event_handler_unregister failed: %s", esp_err_to_name(ret));
    }

    if (s_eth_handle != NULL) {
        ret = esp_eth_test_eth_deinit(s_eth_handle);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "esp_eth_test_eth_deinit failed: %s", esp_err_to_name(ret));
            ESP_LOGI(TAG, "Trying to stop Ethernet driver and deinitialize it again...");
            ret = esp_eth_stop(s_eth_handle);
            vTaskDelay(pdMS_TO_TICKS(500));
            if (ret != ESP_OK) {
                ESP_LOGW(TAG, "esp_eth_stop failed: %s", esp_err_to_name(ret));
            }
            // try to deinitialize Ethernet driver again
            ret = esp_eth_test_eth_deinit(s_eth_handle);
            if (ret != ESP_OK) {
                ESP_LOGW(TAG, "esp_eth_test_eth_deinit failed: %s", esp_err_to_name(ret));
            }
        }
        s_eth_handle = NULL;
    }

    ret = esp_event_loop_delete_default();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        // ESP_ERR_INVALID_STATE is OK - loop might already be deleted
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

void* eth_test_alloc(size_t size)
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
    return;
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

/** Event handler for Ethernet events */
void eth_test_default_event_handler(void *arg, esp_event_base_t event_base,
                                int32_t event_id, void *event_data)
{
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

/** Event handler for IP_EVENT_ETH_GOT_IP */
void eth_test_got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
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
