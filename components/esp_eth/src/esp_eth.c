// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <sys/cdefs.h>
#include "esp_log.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_netif.h"
#include "tcpip_adapter_compatible/tcpip_adapter_compat.h"

static const char *TAG = "esp_eth";
#define ETH_CHECK(a, str, goto_tag, ret_value, ...)                               \
    do                                                                            \
    {                                                                             \
        if (!(a))                                                                 \
        {                                                                         \
            ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ret = ret_value;                                                      \
            goto goto_tag;                                                        \
        }                                                                         \
    } while (0)

ESP_EVENT_DEFINE_BASE(ETH_EVENT);

/**
 * @brief The Ethernet driver mainly consists of PHY, MAC and
 * the mediator who will handle the request/response from/to MAC, PHY and Users.
 * Ethernet driver adopts an OS timer to check the link status periodically.
 * This structure preserves some important Ethernet attributes (e.g. speed, duplex, link).
 * Function stack_input is the channel which set by user, it will deliver all received packets.
 * If stack_input is set to NULL, then all received packets will be passed to tcp/ip stack.
 * on_lowlevel_init_done and on_lowlevel_deinit_done are callbacks set by user.
 * In the callback, user can do any low level operations (e.g. enable/disable crystal clock).
 */
typedef struct {
    esp_netif_driver_base_t base;
    esp_eth_mediator_t mediator;
    esp_eth_phy_t *phy;
    esp_eth_mac_t *mac;
    TimerHandle_t check_link_timer;
    eth_speed_t speed;
    eth_duplex_t duplex;
    eth_link_t link;
    esp_err_t (*stack_input)(esp_eth_handle_t eth_handle, uint8_t *buffer, uint32_t length);
    esp_err_t (*on_lowlevel_init_done)(esp_eth_handle_t eth_handle);
    esp_err_t (*on_lowlevel_deinit_done)(esp_eth_handle_t eth_handle);
} esp_eth_driver_t;

////////////////////////////////Mediator Functions////////////////////////////////////////////
// Following functions are owned by mediator, which will get invoked by MAC or PHY.
// Mediator functions need to find the right actor (MAC, PHY or user) to perform the operation.
// So in the head of mediator function, we have to get the esp_eth_driver_t pointer.
// With this pointer, we could deliver the task to the real actor (MAC, PHY or user).
// This might sound excessive, but is helpful to separate the PHY with MAC (they can not contact with each other directly).
// For more details, please refer to WiKi. https://en.wikipedia.org/wiki/Mediator_pattern
//////////////////////////////////////////////////////////////////////////////////////////////

static esp_err_t eth_phy_reg_read(esp_eth_mediator_t *eth, uint32_t phy_addr, uint32_t phy_reg, uint32_t *reg_value)
{
    esp_eth_driver_t *eth_driver = __containerof(eth, esp_eth_driver_t, mediator);
    esp_eth_mac_t *mac = eth_driver->mac;
    return mac->read_phy_reg(mac, phy_addr, phy_reg, reg_value);
}

static esp_err_t eth_phy_reg_write(esp_eth_mediator_t *eth, uint32_t phy_addr, uint32_t phy_reg, uint32_t reg_value)
{
    esp_eth_driver_t *eth_driver = __containerof(eth, esp_eth_driver_t, mediator);
    esp_eth_mac_t *mac = eth_driver->mac;
    return mac->write_phy_reg(mac, phy_addr, phy_reg, reg_value);
}

static esp_err_t eth_stack_input(esp_eth_mediator_t *eth, uint8_t *buffer, uint32_t length)
{
    esp_eth_driver_t *eth_driver = __containerof(eth, esp_eth_driver_t, mediator);
    if (!eth_driver->stack_input) {
        return esp_netif_receive(eth_driver->base.netif, buffer, length, NULL);
    } else {
        return eth_driver->stack_input((esp_eth_handle_t)eth_driver, buffer, length);
    }
}

static esp_err_t eth_on_state_changed(esp_eth_mediator_t *eth, esp_eth_state_t state, void *args)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = __containerof(eth, esp_eth_driver_t, mediator);
    esp_eth_mac_t *mac = eth_driver->mac;
    switch (state) {
    case ETH_STATE_LLINIT: {
        if (eth_driver->on_lowlevel_init_done) {
            ETH_CHECK(eth_driver->on_lowlevel_init_done(eth_driver) == ESP_OK, "extra lowlevel init failed", err, ESP_FAIL);
        }
        break;
    }
    case ETH_STATE_DEINIT: {
        if (eth_driver->on_lowlevel_deinit_done) {
            ETH_CHECK(eth_driver->on_lowlevel_deinit_done(eth_driver) == ESP_OK, "extra lowlevel deinit failed", err, ESP_FAIL);
        }
        break;
    }
    case ETH_STATE_LINK: {
        eth_link_t link = (eth_link_t)args;
        ETH_CHECK(mac->set_link(mac, link) == ESP_OK, "ethernet mac set link failed", err, ESP_FAIL);
        eth_driver->link = link;
        if (link == ETH_LINK_UP) {
            ETH_CHECK(esp_event_post(ETH_EVENT, ETHERNET_EVENT_CONNECTED, &eth_driver, sizeof(eth_driver), 0) == ESP_OK,
                      "send ETHERNET_EVENT_CONNECTED event failed", err, ESP_FAIL);
        } else if (link == ETH_LINK_DOWN) {
            ETH_CHECK(esp_event_post(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, &eth_driver, sizeof(eth_driver), 0) == ESP_OK,
                      "send ETHERNET_EVENT_DISCONNECTED event failed", err, ESP_FAIL);
        }
        break;
    }
    case ETH_STATE_SPEED: {
        eth_speed_t speed = (eth_speed_t)args;
        ETH_CHECK(mac->set_speed(mac, speed) == ESP_OK, "ethernet mac set speed failed", err, ESP_FAIL);
        eth_driver->speed = speed;
        break;
    }
    case ETH_STATE_DUPLEX: {
        eth_duplex_t duplex = (eth_duplex_t)args;
        ETH_CHECK(mac->set_duplex(mac, duplex) == ESP_OK, "ethernet mac set duplex failed", err, ESP_FAIL);
        eth_driver->duplex = duplex;
        break;
    }
    default:
        ETH_CHECK(false, "unknown ethernet state: %d", err, ESP_ERR_INVALID_ARG, state);
        break;
    }
    return ESP_OK;
err:
    return ret;
}

static void eth_check_link_timer_cb(TimerHandle_t xTimer)
{
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)pvTimerGetTimerID(xTimer);
    esp_eth_phy_t *phy = eth_driver->phy;
    phy->get_link(phy);
}

static esp_err_t esp_eth_post_attach_driver_start(esp_netif_t * esp_netif, void * args)
{
    uint8_t eth_mac[6];
    esp_eth_driver_t *eth_driver = args;
    eth_driver->base.netif = esp_netif;

    // Set driver related config to esp-netif
    esp_netif_driver_ifconfig_t driver_ifconfig = {
            .handle =  eth_driver,
            .transmit = esp_eth_transmit,
            .driver_free_rx_buffer = NULL
    };

    ESP_ERROR_CHECK(esp_netif_set_driver_config(esp_netif, &driver_ifconfig));
    esp_eth_ioctl(eth_driver, ETH_CMD_G_MAC_ADDR, eth_mac);
    ESP_LOGI(TAG, "%x %x %x %x %x %x", eth_mac[0], eth_mac[1], eth_mac[2], eth_mac[3], eth_mac[4], eth_mac[5]);

    esp_netif_set_mac(esp_netif, eth_mac);
    ESP_LOGI(TAG, "ETH netif started");
    return esp_eth_driver_start(eth_driver);
}

////////////////////////////////User face APIs////////////////////////////////////////////////
// User has to pass the handle of Ethernet driver to each API.
// Different Ethernet driver instance is identified with a unique handle.
// It's helpful for us to support multiple Ethernet port on ESP32.
//////////////////////////////////////////////////////////////////////////////////////////////
esp_err_t esp_eth_driver_start(esp_eth_handle_t eth_handle)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = eth_handle;
    ETH_CHECK(xTimerStart(eth_driver->check_link_timer, 0) == pdPASS, "start eth_link_timer failed", err_start_timer, ESP_FAIL);

    ETH_CHECK(esp_event_post(ETH_EVENT, ETHERNET_EVENT_START, &eth_driver, sizeof(eth_driver), 0) == ESP_OK,
              "send ETHERNET_EVENT_START event failed", err_event, ESP_FAIL);
    return ret;
err_start_timer:
    xTimerDelete(eth_driver->check_link_timer, 0);
err_event:
    esp_eth_driver_uninstall(eth_driver);
    return ret;
}

esp_err_t esp_eth_driver_install(const esp_eth_config_t *config, esp_eth_handle_t *out_hdl)
{
    esp_err_t ret = ESP_OK;
    ETH_CHECK(config, "eth config can't be null", err, ESP_ERR_INVALID_ARG);
    ETH_CHECK(out_hdl, "eth handle can't be null", err, ESP_ERR_INVALID_ARG);
    esp_eth_mac_t *mac = config->mac;
    esp_eth_phy_t *phy = config->phy;
    ETH_CHECK(mac && phy, "can't set eth->mac or eth->phy to null", err, ESP_ERR_INVALID_ARG);
    esp_eth_driver_t *eth_driver = calloc(1, sizeof(esp_eth_driver_t));
    ETH_CHECK(eth_driver, "request memory for eth_driver failed", err, ESP_ERR_NO_MEM);
    eth_driver->mac = mac;
    eth_driver->phy = phy;
    eth_driver->link = ETH_LINK_DOWN;
    eth_driver->duplex = ETH_DUPLEX_HALF;
    eth_driver->speed = ETH_SPEED_10M;
    eth_driver->stack_input = config->stack_input;
    eth_driver->on_lowlevel_init_done = config->on_lowlevel_init_done;
    eth_driver->on_lowlevel_deinit_done = config->on_lowlevel_deinit_done;
    eth_driver->mediator.phy_reg_read = eth_phy_reg_read;
    eth_driver->mediator.phy_reg_write = eth_phy_reg_write;
    eth_driver->mediator.stack_input = eth_stack_input;
    eth_driver->mediator.on_state_changed = eth_on_state_changed;
    ETH_CHECK(mac->set_mediator(mac, &eth_driver->mediator) == ESP_OK, "set mediator for mac failed", err_mediator, ESP_FAIL);
    ETH_CHECK(phy->set_mediator(phy, &eth_driver->mediator) == ESP_OK, "set mediator for phy failed", err_mediator, ESP_FAIL);
    ETH_CHECK(mac->init(mac) == ESP_OK, "init mac failed", err_init_mac, ESP_FAIL);
    ETH_CHECK(phy->init(phy) == ESP_OK, "init phy failed", err_init_phy, ESP_FAIL);
    eth_driver->check_link_timer = xTimerCreate("eth_link_timer", pdMS_TO_TICKS(config->check_link_period_ms), pdTRUE,
                                   eth_driver, eth_check_link_timer_cb);
    ETH_CHECK(eth_driver->check_link_timer, "create eth_link_timer failed", err_create_timer, ESP_FAIL);
    eth_driver->base.post_attach = esp_eth_post_attach_driver_start;
    *out_hdl = (esp_eth_handle_t)eth_driver;
    tcpip_adapter_compat_start_eth(eth_driver);
    return ESP_OK;
err_create_timer:
    phy->deinit(phy);
err_init_phy:
    mac->deinit(mac);
err_init_mac:
err_mediator:
    free(eth_driver);
err:
    return ret;
}

esp_err_t esp_eth_driver_uninstall(esp_eth_handle_t hdl)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)hdl;
    ETH_CHECK(eth_driver, "ethernet driver handle can't be null", err, ESP_ERR_INVALID_ARG);
    esp_eth_mac_t *mac = eth_driver->mac;
    esp_eth_phy_t *phy = eth_driver->phy;
    ETH_CHECK(xTimerDelete(eth_driver->check_link_timer, 0) == pdPASS, "delete eth_link_timer failed", err, ESP_FAIL);
    ETH_CHECK(esp_event_post(ETH_EVENT, ETHERNET_EVENT_STOP, &eth_driver, sizeof(eth_driver), 0) == ESP_OK,
              "send ETHERNET_EVENT_STOP event failed", err, ESP_FAIL);
    ETH_CHECK(phy->deinit(phy) == ESP_OK, "deinit phy failed", err, ESP_FAIL);
    ETH_CHECK(mac->deinit(mac) == ESP_OK, "deinit mac failed", err, ESP_FAIL);
    free(eth_driver);
    return ESP_OK;
err:
    return ret;
}

esp_err_t esp_eth_transmit(void* hdl, void *buf, uint32_t length)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)hdl;
    ETH_CHECK(eth_driver, "ethernet driver handle can't be null", err, ESP_ERR_INVALID_ARG);
    esp_eth_mac_t *mac = eth_driver->mac;
    return mac->transmit(mac, buf, length);
err:
    return ret;
}

esp_err_t esp_eth_receive(esp_eth_handle_t hdl, uint8_t *buf, uint32_t *length)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)hdl;
    ETH_CHECK(eth_driver, "ethernet driver handle can't be null", err, ESP_ERR_INVALID_ARG);
    esp_eth_mac_t *mac = eth_driver->mac;
    return mac->receive(mac, buf, length);
err:
    return ret;
}

esp_err_t esp_eth_ioctl(esp_eth_handle_t hdl, esp_eth_io_cmd_t cmd, void *data)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)hdl;
    ETH_CHECK(eth_driver, "ethernet driver handle can't be null", err, ESP_ERR_INVALID_ARG);
    esp_eth_mac_t *mac = eth_driver->mac;
    esp_eth_phy_t *phy = eth_driver->phy;
    switch (cmd) {
    case ETH_CMD_S_MAC_ADDR:
        ETH_CHECK(data, "can't set mac addr to null", err, ESP_ERR_INVALID_ARG);
        ETH_CHECK(mac->set_addr(mac, (uint8_t *)data) == ESP_OK, "set mac address failed", err, ESP_FAIL);
        break;
    case ETH_CMD_G_MAC_ADDR:
        ETH_CHECK(data, "no mem to store mac addr", err, ESP_ERR_INVALID_ARG);
        ETH_CHECK(mac->get_addr(mac, (uint8_t *)data) == ESP_OK, "get mac address failed", err, ESP_FAIL);
        break;
    case ETH_CMD_S_PHY_ADDR:
        ETH_CHECK(data, "can't set phy addr to null", err, ESP_ERR_INVALID_ARG);
        ETH_CHECK(phy->set_addr(phy, (uint32_t)data) == ESP_OK, "set phy address failed", err, ESP_FAIL);
        break;
    case ETH_CMD_G_PHY_ADDR:
        ETH_CHECK(data, "no mem to store phy addr", err, ESP_ERR_INVALID_ARG);
        ETH_CHECK(phy->get_addr(phy, (uint32_t *)data) == ESP_OK, "get phy address failed", err, ESP_FAIL);
        break;
    case ETH_CMD_G_SPEED:
        ETH_CHECK(data, "no mem to store speed value", err, ESP_ERR_INVALID_ARG);
        *(eth_speed_t *)data = eth_driver->speed;
        break;
    case ETH_CMD_S_PROMISCUOUS:
        ETH_CHECK(mac->set_promiscuous(mac, (bool)data) == ESP_OK, "set promiscuous mode failed", err, ESP_FAIL);
        break;
    default:
        ETH_CHECK(false, "unknown io command: %d", err, ESP_ERR_INVALID_ARG, cmd);
        break;
    }
    return ESP_OK;
err:
    return ret;
}

esp_err_t esp_eth_clear_default_handlers(void* esp_netif)
{
    esp_err_t ret;
    ETH_CHECK(esp_netif, "esp-netif handle can't be null", fail, ESP_ERR_INVALID_ARG);

    esp_event_handler_unregister(ETH_EVENT, ETHERNET_EVENT_START, esp_netif_action_start);
    esp_event_handler_unregister(ETH_EVENT, ETHERNET_EVENT_STOP, esp_netif_action_stop);
    esp_event_handler_unregister(ETH_EVENT, ETHERNET_EVENT_CONNECTED, esp_netif_action_connected);
    esp_event_handler_unregister(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, esp_netif_action_disconnected);
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, esp_netif_action_got_ip);
    return ESP_OK;
fail:
    return ret;
}

esp_err_t esp_eth_set_default_handlers(void* esp_netif)
{
    esp_err_t ret;
    ETH_CHECK(esp_netif, "esp-netif handle can't be null", fail, ESP_ERR_INVALID_ARG);

    ret = esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_START, esp_netif_action_start, esp_netif);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_STOP, esp_netif_action_stop, esp_netif);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_CONNECTED, esp_netif_action_connected, esp_netif);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, esp_netif_action_disconnected, esp_netif);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, esp_netif_action_got_ip, esp_netif);
    if (ret != ESP_OK) {
        goto fail;
    }

    return ESP_OK;

fail:
    esp_eth_clear_default_handlers(esp_netif);
    return ret;
}
