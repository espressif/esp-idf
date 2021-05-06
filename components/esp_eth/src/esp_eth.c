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
#include <stdatomic.h>
#include "esp_log.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

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

typedef enum {
    ESP_ETH_FSM_STOP,
    ESP_ETH_FSM_START
} esp_eth_fsm_t;

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
    esp_eth_mediator_t mediator;
    esp_eth_phy_t *phy;
    esp_eth_mac_t *mac;
    TimerHandle_t check_link_timer;
    eth_speed_t speed;
    eth_duplex_t duplex;
    eth_link_t link;
    atomic_int ref_count;
    void *priv;
    _Atomic esp_eth_fsm_t fsm;
    esp_err_t (*stack_input)(esp_eth_handle_t eth_handle, uint8_t *buffer, uint32_t length, void *priv);
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
    if (eth_driver->stack_input) {
        return eth_driver->stack_input((esp_eth_handle_t)eth_driver, buffer, length, eth_driver->priv);
    } else {
        free(buffer);
        return ESP_OK;
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
    esp_eth_increase_reference(eth_driver);
    phy->get_link(phy);
    esp_eth_decrease_reference(eth_driver);
}

////////////////////////////////User face APIs////////////////////////////////////////////////
// User has to pass the handle of Ethernet driver to each API.
// Different Ethernet driver instance is identified with a unique handle.
// It's helpful for us to support multiple Ethernet port on ESP32.
//////////////////////////////////////////////////////////////////////////////////////////////

esp_err_t esp_eth_driver_install(const esp_eth_config_t *config, esp_eth_handle_t *out_hdl)
{
    esp_err_t ret = ESP_OK;
    ETH_CHECK(config, "eth config can't be null", err, ESP_ERR_INVALID_ARG);
    ETH_CHECK(out_hdl, "eth handle can't be null", err, ESP_ERR_INVALID_ARG);
    esp_eth_mac_t *mac = config->mac;
    esp_eth_phy_t *phy = config->phy;
    ETH_CHECK(mac && phy, "can't set eth->mac or eth->phy to null", err, ESP_ERR_INVALID_ARG);
    // eth_driver contains an atomic variable, which should not be put in PSRAM
    esp_eth_driver_t *eth_driver = heap_caps_calloc(1, sizeof(esp_eth_driver_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ETH_CHECK(eth_driver, "request memory for eth_driver failed", err, ESP_ERR_NO_MEM);
    atomic_init(&eth_driver->ref_count, 1);
    atomic_init(&eth_driver->fsm, ESP_ETH_FSM_STOP);
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
    /* some PHY can't output RMII clock if in reset state, so hardware reset PHY chip firstly */
    phy->reset_hw(phy);
    ETH_CHECK(mac->set_mediator(mac, &eth_driver->mediator) == ESP_OK, "set mediator for mac failed", err_mediator, ESP_FAIL);
    ETH_CHECK(phy->set_mediator(phy, &eth_driver->mediator) == ESP_OK, "set mediator for phy failed", err_mediator, ESP_FAIL);
    ETH_CHECK(mac->init(mac) == ESP_OK, "init mac failed", err_init_mac, ESP_FAIL);
    ETH_CHECK(phy->init(phy) == ESP_OK, "init phy failed", err_init_phy, ESP_FAIL);
    eth_driver->check_link_timer = xTimerCreate("eth_link_timer", pdMS_TO_TICKS(config->check_link_period_ms), pdTRUE,
                                   eth_driver, eth_check_link_timer_cb);
    ETH_CHECK(eth_driver->check_link_timer, "create eth_link_timer failed", err_create_timer, ESP_FAIL);
    *out_hdl = (esp_eth_handle_t)eth_driver;

    // for backward compatible to 4.0, and will get removed in 5.0
#if CONFIG_ESP_NETIF_TCPIP_ADAPTER_COMPATIBLE_LAYER
    extern esp_err_t tcpip_adapter_compat_start_eth(void *eth_driver);
    tcpip_adapter_compat_start_eth(eth_driver);
#endif

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
    // check if driver has started
    esp_eth_fsm_t expected_fsm = ESP_ETH_FSM_STOP;
    if (!atomic_compare_exchange_strong(&eth_driver->fsm, &expected_fsm, ESP_ETH_FSM_STOP)) {
        ESP_LOGW(TAG, "driver not stopped yet");
        ret = ESP_ERR_INVALID_STATE;
        goto err;
    }
    // don't uninstall driver unless there's only one reference
    int expected_ref_count = 1;
    if (!atomic_compare_exchange_strong(&eth_driver->ref_count, &expected_ref_count, 0)) {
        ESP_LOGE(TAG, "%d ethernet reference in use", expected_ref_count);
        ret = ESP_ERR_INVALID_STATE;
        goto err;
    }
    esp_eth_mac_t *mac = eth_driver->mac;
    esp_eth_phy_t *phy = eth_driver->phy;
    ETH_CHECK(xTimerDelete(eth_driver->check_link_timer, 0) == pdPASS, "delete eth_link_timer failed", err, ESP_FAIL);
    ETH_CHECK(phy->deinit(phy) == ESP_OK, "deinit phy failed", err, ESP_FAIL);
    ETH_CHECK(mac->deinit(mac) == ESP_OK, "deinit mac failed", err, ESP_FAIL);
    free(eth_driver);
    return ESP_OK;
err:
    return ret;
}

esp_err_t esp_eth_start(esp_eth_handle_t hdl)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)hdl;
    ETH_CHECK(eth_driver, "ethernet driver handle can't be null", err, ESP_ERR_INVALID_ARG);
    esp_eth_phy_t *phy = eth_driver->phy;
    esp_eth_mac_t *mac = eth_driver->mac;
    // check if driver has started
    esp_eth_fsm_t expected_fsm = ESP_ETH_FSM_STOP;
    if (!atomic_compare_exchange_strong(&eth_driver->fsm, &expected_fsm, ESP_ETH_FSM_START)) {
        ESP_LOGW(TAG, "driver started already");
        ret = ESP_ERR_INVALID_STATE;
        goto err;
    }
    ETH_CHECK(phy->negotiate(phy) == ESP_OK, "phy negotiation failed", err, ESP_FAIL);
    ETH_CHECK(mac->start(mac) == ESP_OK, "start mac failed", err, ESP_FAIL);
    ETH_CHECK(esp_event_post(ETH_EVENT, ETHERNET_EVENT_START, &eth_driver, sizeof(eth_driver), 0) == ESP_OK,
              "send ETHERNET_EVENT_START event failed", err, ESP_FAIL);
    ETH_CHECK(phy->get_link(phy) == ESP_OK, "phy get link status failed", err, ESP_FAIL);
    ETH_CHECK(xTimerStart(eth_driver->check_link_timer, 0) == pdPASS,
              "start eth_link_timer failed", err, ESP_FAIL);
err:
    return ret;
}

esp_err_t esp_eth_stop(esp_eth_handle_t hdl)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)hdl;
    ETH_CHECK(eth_driver, "ethernet driver handle can't be null", err, ESP_ERR_INVALID_ARG);
    // check if driver has started
    esp_eth_fsm_t expected_fsm = ESP_ETH_FSM_START;
    if (!atomic_compare_exchange_strong(&eth_driver->fsm, &expected_fsm, ESP_ETH_FSM_STOP)) {
        ESP_LOGW(TAG, "driver not started yet");
        ret = ESP_ERR_INVALID_STATE;
        goto err;
    }
    esp_eth_mac_t *mac = eth_driver->mac;
    ETH_CHECK(mac->stop(mac) == ESP_OK, "stop mac failed", err, ESP_FAIL);
    ETH_CHECK(xTimerStop(eth_driver->check_link_timer, 0) == pdPASS,
              "stop eth_link_timer failed", err, ESP_FAIL);
    ETH_CHECK(esp_event_post(ETH_EVENT, ETHERNET_EVENT_STOP, &eth_driver, sizeof(eth_driver), 0) == ESP_OK,
              "send ETHERNET_EVENT_STOP event failed", err, ESP_FAIL);
    return ESP_OK;
err:
    return ret;
}

esp_err_t esp_eth_update_input_path(
    esp_eth_handle_t hdl,
    esp_err_t (*stack_input)(esp_eth_handle_t hdl, uint8_t *buffer, uint32_t length, void *priv),
    void *priv)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)hdl;
    ETH_CHECK(eth_driver, "ethernet driver handle can't be null", err, ESP_ERR_INVALID_ARG);
    eth_driver->priv = priv;
    eth_driver->stack_input = stack_input;
    return ESP_OK;
err:
    return ret;
}

esp_err_t esp_eth_transmit(esp_eth_handle_t hdl, void *buf, uint32_t length)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)hdl;
    ETH_CHECK(buf, "can't set buf to null", err, ESP_ERR_INVALID_ARG);
    ETH_CHECK(length, "buf length can't be zero", err, ESP_ERR_INVALID_ARG);
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
    ETH_CHECK(buf && length, "can't set buf and length to null", err, ESP_ERR_INVALID_ARG);
    ETH_CHECK(*length > 60, "length can't be less than 60", err, ESP_ERR_INVALID_ARG);
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

esp_err_t esp_eth_increase_reference(esp_eth_handle_t hdl)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)hdl;
    ETH_CHECK(eth_driver, "ethernet driver handle can't be null", err, ESP_ERR_INVALID_ARG);
    atomic_fetch_add(&eth_driver->ref_count, 1);
    return ESP_OK;
err:
    return ret;
}

esp_err_t esp_eth_decrease_reference(esp_eth_handle_t hdl)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)hdl;
    ETH_CHECK(eth_driver, "ethernet driver handle can't be null", err, ESP_ERR_INVALID_ARG);
    atomic_fetch_sub(&eth_driver->ref_count, 1);
    return ESP_OK;
err:
    return ret;
}
