/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/cdefs.h>
#include <stdatomic.h>
#include "esp_log.h"
#include "esp_check.h"
#include "esp_eth_driver.h"
#include "esp_event.h"
#include "esp_heap_caps.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#if CONFIG_ETH_TRANSMIT_MUTEX
/**
 * @brief Transmit timeout when multiple accesses to network driver
 */
#define ESP_ETH_TX_TIMEOUT_MS   250
#endif

static const char *TAG = "esp_eth";

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
    esp_timer_handle_t check_link_timer;
    uint32_t check_link_period_ms;
    bool auto_nego_en;
    eth_speed_t speed;
    eth_duplex_t duplex;
    _Atomic eth_link_t link;
    atomic_int ref_count;
    void *priv;
    _Atomic esp_eth_fsm_t fsm;
#if CONFIG_ETH_TRANSMIT_MUTEX
    SemaphoreHandle_t transmit_mutex;
#endif // CONFIG_ETH_TRANSMIT_MUTEX
    esp_err_t (*stack_input)(esp_eth_handle_t eth_handle, uint8_t *buffer, uint32_t length, void *priv);
    esp_err_t (*on_lowlevel_init_done)(esp_eth_handle_t eth_handle);
    esp_err_t (*on_lowlevel_deinit_done)(esp_eth_handle_t eth_handle);
    esp_err_t (*customized_read_phy_reg)(esp_eth_handle_t eth_handle, uint32_t phy_addr, uint32_t phy_reg, uint32_t *reg_value);
    esp_err_t (*customized_write_phy_reg)(esp_eth_handle_t eth_handle, uint32_t phy_addr, uint32_t phy_reg, uint32_t reg_value);
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
    // invoking user customized PHY IO function if necessary
    if (eth_driver->customized_read_phy_reg) {
        return eth_driver->customized_read_phy_reg(eth_driver, phy_addr, phy_reg, reg_value);
    }
    // by default, PHY device is managed by MAC's SMI interface
    esp_eth_mac_t *mac = eth_driver->mac;
    return mac->read_phy_reg(mac, phy_addr, phy_reg, reg_value);
}

static esp_err_t eth_phy_reg_write(esp_eth_mediator_t *eth, uint32_t phy_addr, uint32_t phy_reg, uint32_t reg_value)
{
    esp_eth_driver_t *eth_driver = __containerof(eth, esp_eth_driver_t, mediator);
    // invoking user customized PHY IO function if necessary
    if (eth_driver->customized_write_phy_reg) {
        return eth_driver->customized_write_phy_reg(eth_driver, phy_addr, phy_reg, reg_value);
    }
    // by default, PHY device is managed by MAC's SMI interface
    esp_eth_mac_t *mac = eth_driver->mac;
    return mac->write_phy_reg(mac, phy_addr, phy_reg, reg_value);
}

static esp_err_t eth_stack_input(esp_eth_mediator_t *eth, uint8_t *buffer, uint32_t length)
{
    esp_eth_driver_t *eth_driver = __containerof(eth, esp_eth_driver_t, mediator);
    if (eth_driver->stack_input) {
        return eth_driver->stack_input((esp_eth_handle_t)eth_driver, buffer, length, eth_driver->priv);
    }
    // No stack input path has been installed, just drop the incoming packets
    free(buffer);
    return ESP_OK;
}

static esp_err_t eth_on_state_changed(esp_eth_mediator_t *eth, esp_eth_state_t state, void *args)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = __containerof(eth, esp_eth_driver_t, mediator);
    esp_eth_mac_t *mac = eth_driver->mac;
    switch (state) {
    case ETH_STATE_LLINIT: {
        if (eth_driver->on_lowlevel_init_done) {
            ESP_GOTO_ON_ERROR(eth_driver->on_lowlevel_init_done(eth_driver), err, TAG, "extra lowlevel init failed");
        }
        break;
    }
    case ETH_STATE_DEINIT: {
        if (eth_driver->on_lowlevel_deinit_done) {
            ESP_GOTO_ON_ERROR(eth_driver->on_lowlevel_deinit_done(eth_driver), err, TAG, "extra lowlevel deinit failed");
        }
        break;
    }
    case ETH_STATE_LINK: {
        eth_link_t link = (eth_link_t)args;
        ESP_GOTO_ON_ERROR(mac->set_link(mac, link), err, TAG, "ethernet mac set link failed");
        atomic_store(&eth_driver->link, link);
        if (link == ETH_LINK_UP) {
            ESP_GOTO_ON_ERROR(esp_event_post(ETH_EVENT, ETHERNET_EVENT_CONNECTED, &eth_driver, sizeof(esp_eth_driver_t *), 0), err,
                              TAG, "send ETHERNET_EVENT_CONNECTED event failed");
        } else if (link == ETH_LINK_DOWN) {
            ESP_GOTO_ON_ERROR(esp_event_post(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, &eth_driver, sizeof(esp_eth_driver_t *), 0), err,
                              TAG, "send ETHERNET_EVENT_DISCONNECTED event failed");
        }
        break;
    }
    case ETH_STATE_SPEED: {
        eth_speed_t speed = (eth_speed_t)args;
        ESP_GOTO_ON_ERROR(mac->set_speed(mac, speed), err, TAG, "ethernet mac set speed failed");
        eth_driver->speed = speed;
        break;
    }
    case ETH_STATE_DUPLEX: {
        eth_duplex_t duplex = (eth_duplex_t)args;
        ESP_GOTO_ON_ERROR(mac->set_duplex(mac, duplex), err, TAG, "ethernet mac set duplex failed");
        eth_driver->duplex = duplex;
        break;
    }
    case ETH_STATE_PAUSE: {
        uint32_t peer_pause_ability = (uint32_t)args;
        ESP_GOTO_ON_ERROR(mac->set_peer_pause_ability(mac, peer_pause_ability), err, TAG, "ethernet mac set peer pause ability failed");
        break;
    }
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "unknown ethernet state: %d", state);
        break;
    }
err:
    return ret;
}

static void eth_check_link_timer_cb(void *args)
{
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)args;
    esp_eth_phy_t *phy = eth_driver->phy;
    phy->get_link(phy);
}

////////////////////////////////User face APIs////////////////////////////////////////////////
// User has to pass the handle of Ethernet driver to each API.
// Different Ethernet driver instance is identified with a unique handle.
// It's helpful for us to support multiple Ethernet port on ESP32.
//////////////////////////////////////////////////////////////////////////////////////////////

esp_err_t esp_eth_driver_install(const esp_eth_config_t *config, esp_eth_handle_t *out_hdl)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mac_t *mac = NULL;
    esp_eth_phy_t *phy = NULL;
    esp_eth_driver_t *eth_driver = NULL;
    ESP_GOTO_ON_FALSE(config && out_hdl, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    mac = config->mac;
    phy = config->phy;
    ESP_GOTO_ON_FALSE(mac && phy, ESP_ERR_INVALID_ARG, err, TAG, "can't set eth->mac or eth->phy to null");
    // eth_driver contains an atomic variable, which should not be put in PSRAM
    eth_driver = heap_caps_calloc(1, sizeof(esp_eth_driver_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(eth_driver, ESP_ERR_NO_MEM, err, TAG, "no mem for eth_driver");
    const esp_timer_create_args_t check_link_timer_args = {
        .callback = eth_check_link_timer_cb,
        .name = "eth_link_timer",
        .arg = eth_driver,
        .skip_unhandled_events = true
    };
    ESP_GOTO_ON_ERROR(esp_timer_create(&check_link_timer_args, &eth_driver->check_link_timer), err, TAG, "create link timer failed");
#if CONFIG_ETH_TRANSMIT_MUTEX
    eth_driver->transmit_mutex = xSemaphoreCreateMutex();
    ESP_GOTO_ON_FALSE(eth_driver->transmit_mutex, ESP_ERR_NO_MEM, err, TAG, "Failed to create transmit mutex");
#endif // CONFIG_ETH_TRANSMIT_MUTEX
    atomic_init(&eth_driver->ref_count, 1);
    atomic_init(&eth_driver->fsm, ESP_ETH_FSM_STOP);
    eth_driver->mac = mac;
    eth_driver->phy = phy;
    atomic_init(&eth_driver->link, ETH_LINK_DOWN);
    eth_driver->duplex = ETH_DUPLEX_HALF;
    eth_driver->speed = ETH_SPEED_10M;
    eth_driver->stack_input = config->stack_input;
    eth_driver->on_lowlevel_init_done = config->on_lowlevel_init_done;
    eth_driver->on_lowlevel_deinit_done = config->on_lowlevel_deinit_done;
    eth_driver->check_link_period_ms = config->check_link_period_ms;
    eth_driver->customized_read_phy_reg = config->read_phy_reg;
    eth_driver->customized_write_phy_reg = config->write_phy_reg;
    eth_driver->mediator.phy_reg_read = eth_phy_reg_read;
    eth_driver->mediator.phy_reg_write = eth_phy_reg_write;
    eth_driver->mediator.stack_input = eth_stack_input;
    eth_driver->mediator.on_state_changed = eth_on_state_changed;
    // set mediator for both mac and phy object, so that mac and phy are connected to each other via mediator
    mac->set_mediator(mac, &eth_driver->mediator);
    phy->set_mediator(phy, &eth_driver->mediator);
    // for PHY whose internal PLL has been configured to generate RMII clock, but is put in reset state during power up,
    // we need to deasseert the reset GPIO of PHY device first, ensure the RMII is clocked out from PHY
    phy->reset_hw(phy);
    // init MAC first, so that MAC can generate the correct SMI signals
    ESP_GOTO_ON_ERROR(mac->init(mac), err, TAG, "init mac failed");
    ESP_GOTO_ON_ERROR(phy->init(phy), err, TAG, "init phy failed");
    // get default status of PHY autonegotiation (ultimately may also indicate if it is supported)
    ESP_GOTO_ON_ERROR(phy->autonego_ctrl(phy, ESP_ETH_PHY_AUTONEGO_G_STAT, &eth_driver->auto_nego_en),
                        err, TAG, "get autonegotiation status failed");
    ESP_LOGD(TAG, "new ethernet driver @%p", eth_driver);
    *out_hdl = eth_driver;

    return ESP_OK;
err:
    if (eth_driver) {
        if (eth_driver->check_link_timer) {
            esp_timer_delete(eth_driver->check_link_timer);
        }
#if CONFIG_ETH_TRANSMIT_MUTEX
        if (eth_driver->transmit_mutex) {
            vSemaphoreDelete(eth_driver->transmit_mutex);
        }
#endif // CONFIG_ETH_TRANSMIT_MUTEX
        free(eth_driver);
    }
    return ret;
}

esp_err_t esp_eth_driver_uninstall(esp_eth_handle_t hdl)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)hdl;
    ESP_GOTO_ON_FALSE(eth_driver, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    // check if driver has stopped
    esp_eth_fsm_t expected_fsm = ESP_ETH_FSM_STOP;
    ESP_GOTO_ON_FALSE(atomic_compare_exchange_strong(&eth_driver->fsm, &expected_fsm, ESP_ETH_FSM_STOP),
                      ESP_ERR_INVALID_STATE, err, TAG, "driver not stopped yet");
    // don't uninstall driver unless there's only one reference
    int expected_ref_count = 1;
    ESP_GOTO_ON_FALSE(atomic_compare_exchange_strong(&eth_driver->ref_count, &expected_ref_count, 0),
                      ESP_ERR_INVALID_STATE, err, TAG, "%d ethernet reference in use", expected_ref_count);
    esp_eth_mac_t *mac = eth_driver->mac;
    esp_eth_phy_t *phy = eth_driver->phy;
    ESP_GOTO_ON_ERROR(esp_timer_delete(eth_driver->check_link_timer), err, TAG, "delete link timer failed");
#if CONFIG_ETH_TRANSMIT_MUTEX
    vSemaphoreDelete(eth_driver->transmit_mutex);
#endif // CONFIG_ETH_TRANSMIT_MUTEX
    ESP_GOTO_ON_ERROR(phy->deinit(phy), err, TAG, "deinit phy failed");
    ESP_GOTO_ON_ERROR(mac->deinit(mac), err, TAG, "deinit mac failed");
    free(eth_driver);
err:
    return ret;
}

esp_err_t esp_eth_start(esp_eth_handle_t hdl)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)hdl;
    ESP_GOTO_ON_FALSE(eth_driver, ESP_ERR_INVALID_ARG, err, TAG, "ethernet driver handle can't be null");
    esp_eth_phy_t *phy = eth_driver->phy;
    // check if driver has stopped
    esp_eth_fsm_t expected_fsm = ESP_ETH_FSM_STOP;
    ESP_GOTO_ON_FALSE(atomic_compare_exchange_strong(&eth_driver->fsm, &expected_fsm, ESP_ETH_FSM_START),
                      ESP_ERR_INVALID_STATE, err, TAG, "driver started already");
    // Autonegotiate link speed and duplex mode when enabled
    if (eth_driver->auto_nego_en == true) {
        ESP_GOTO_ON_ERROR(phy->autonego_ctrl(phy, ESP_ETH_PHY_AUTONEGO_RESTART, &eth_driver->auto_nego_en), err, TAG, "phy negotiation failed");
    }
    ESP_GOTO_ON_ERROR(esp_event_post(ETH_EVENT, ETHERNET_EVENT_START, &eth_driver, sizeof(esp_eth_driver_t *), 0),
                      err, TAG, "send ETHERNET_EVENT_START event failed");
    ESP_GOTO_ON_ERROR(phy->get_link(phy), err, TAG, "phy get link status failed");
    ESP_GOTO_ON_ERROR(esp_timer_start_periodic(eth_driver->check_link_timer, eth_driver->check_link_period_ms * 1000),
                      err, TAG, "start link timer failed");
err:
    return ret;
}

esp_err_t esp_eth_stop(esp_eth_handle_t hdl)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)hdl;
    ESP_GOTO_ON_FALSE(eth_driver, ESP_ERR_INVALID_ARG, err, TAG, "ethernet driver handle can't be null");
    esp_eth_phy_t *phy = eth_driver->phy;
    // check if driver has started
    esp_eth_fsm_t expected_fsm = ESP_ETH_FSM_START;
    ESP_GOTO_ON_FALSE(atomic_compare_exchange_strong(&eth_driver->fsm, &expected_fsm, ESP_ETH_FSM_STOP),
                      ESP_ERR_INVALID_STATE, err, TAG, "driver not started yet");
    ESP_GOTO_ON_ERROR(esp_timer_stop(eth_driver->check_link_timer), err, TAG, "stop link timer failed");

    eth_link_t expected_link = ETH_LINK_UP;
    if (atomic_compare_exchange_strong(&eth_driver->link, &expected_link, ETH_LINK_DOWN)){
        // MAC is stopped by setting link down at PHY layer
        ESP_GOTO_ON_ERROR(phy->set_link(phy, ETH_LINK_DOWN), err, TAG, "ethernet phy reset link failed");
    }

    ESP_GOTO_ON_ERROR(esp_event_post(ETH_EVENT, ETHERNET_EVENT_STOP, &eth_driver, sizeof(esp_eth_driver_t *), 0),
                      err, TAG, "send ETHERNET_EVENT_STOP event failed");
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
    ESP_GOTO_ON_FALSE(eth_driver, ESP_ERR_INVALID_ARG, err, TAG, "ethernet driver handle can't be null");
    eth_driver->priv = priv;
    eth_driver->stack_input = stack_input;
err:
    return ret;
}

esp_err_t esp_eth_transmit(esp_eth_handle_t hdl, void *buf, size_t length)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)hdl;

    if (atomic_load(&eth_driver->link) != ETH_LINK_UP) {
        ret = ESP_ERR_INVALID_STATE;
        ESP_LOGD(TAG, "Ethernet link is not up, can't transmit");
        goto err;
    }

    ESP_GOTO_ON_FALSE(buf, ESP_ERR_INVALID_ARG, err, TAG, "can't set buf to null");
    ESP_GOTO_ON_FALSE(length, ESP_ERR_INVALID_ARG, err, TAG, "buf length can't be zero");
    ESP_GOTO_ON_FALSE(eth_driver, ESP_ERR_INVALID_ARG, err, TAG, "ethernet driver handle can't be null");
    esp_eth_mac_t *mac = eth_driver->mac;

#if CONFIG_ETH_TRANSMIT_MUTEX
    if (xSemaphoreTake(eth_driver->transmit_mutex, pdMS_TO_TICKS(ESP_ETH_TX_TIMEOUT_MS)) == pdFALSE) {
        return ESP_ERR_TIMEOUT;
    }
#endif // CONFIG_ETH_TRANSMIT_MUTEX
    ret = mac->transmit(mac, buf, length);
#if CONFIG_ETH_TRANSMIT_MUTEX
    xSemaphoreGive(eth_driver->transmit_mutex);
#endif // CONFIG_ETH_TRANSMIT_MUTEX
err:
    return ret;
}

esp_err_t esp_eth_transmit_vargs(esp_eth_handle_t hdl, uint32_t argc, ...)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)hdl;

    if (atomic_load(&eth_driver->link) != ETH_LINK_UP) {
        ret = ESP_ERR_INVALID_STATE;
        ESP_LOGD(TAG, "Ethernet link is not up, can't transmit");
        goto err;
    }

    va_list args;
    esp_eth_mac_t *mac = eth_driver->mac;
#if CONFIG_ETH_TRANSMIT_MUTEX
    if (xSemaphoreTake(eth_driver->transmit_mutex, pdMS_TO_TICKS(ESP_ETH_TX_TIMEOUT_MS)) == pdFALSE) {
        return ESP_ERR_TIMEOUT;
    }
#endif // CONFIG_ETH_TRANSMIT_MUTEX
    va_start(args, argc);
    ret = mac->transmit_vargs(mac, argc, args);
#if CONFIG_ETH_TRANSMIT_MUTEX
    xSemaphoreGive(eth_driver->transmit_mutex);
#endif // CONFIG_ETH_TRANSMIT_MUTEX
    va_end(args);
err:
    return ret;
}

esp_err_t esp_eth_ioctl(esp_eth_handle_t hdl, esp_eth_io_cmd_t cmd, void *data)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)hdl;
    ESP_GOTO_ON_FALSE(eth_driver, ESP_ERR_INVALID_ARG, err, TAG, "ethernet driver handle can't be null");
    esp_eth_mac_t *mac = eth_driver->mac;
    esp_eth_phy_t *phy = eth_driver->phy;
    switch (cmd) {
    case ETH_CMD_S_MAC_ADDR:
        ESP_GOTO_ON_FALSE(data, ESP_ERR_INVALID_ARG, err, TAG, "can't set mac addr to null");
        ESP_GOTO_ON_ERROR(mac->set_addr(mac, (uint8_t *)data), err, TAG, "set mac address failed");
        break;
    case ETH_CMD_G_MAC_ADDR:
        ESP_GOTO_ON_FALSE(data, ESP_ERR_INVALID_ARG, err, TAG, "no mem to store mac addr");
        ESP_GOTO_ON_ERROR(mac->get_addr(mac, (uint8_t *)data), err, TAG, "get mac address failed");
        break;
    case ETH_CMD_S_PHY_ADDR:
        ESP_GOTO_ON_ERROR(phy->set_addr(phy, *(uint32_t *)data), err, TAG, "set phy address failed");
        break;
    case ETH_CMD_G_PHY_ADDR:
        ESP_GOTO_ON_FALSE(data, ESP_ERR_INVALID_ARG, err, TAG, "no mem to store phy addr");
        ESP_GOTO_ON_ERROR(phy->get_addr(phy, (uint32_t *)data), err, TAG, "get phy address failed");
        break;
    case ETH_CMD_S_AUTONEGO:
        ESP_GOTO_ON_FALSE(data, ESP_ERR_INVALID_ARG, err, TAG, "can't set autonegotiation to null");
        // check if driver is stopped; configuration should be changed only when transmitting/receiving is not active
        ESP_GOTO_ON_FALSE(atomic_load(&eth_driver->fsm) == ESP_ETH_FSM_STOP, ESP_ERR_INVALID_STATE, err, TAG, "link configuration is only allowed when driver is stopped");
        if (*(bool *)data == true) {
            ESP_GOTO_ON_ERROR(phy->autonego_ctrl(phy, ESP_ETH_PHY_AUTONEGO_EN, &eth_driver->auto_nego_en), err, TAG, "phy negotiation enable failed");
        } else {
            ESP_GOTO_ON_ERROR(phy->autonego_ctrl(phy, ESP_ETH_PHY_AUTONEGO_DIS, &eth_driver->auto_nego_en), err, TAG, "phy negotiation disable failed");
        }
        break;
    case ETH_CMD_G_AUTONEGO:
        ESP_GOTO_ON_FALSE(data, ESP_ERR_INVALID_ARG, err, TAG, "no mem to store autonegotiation configuration");
        *(bool *)data = eth_driver->auto_nego_en;
        break;
    case ETH_CMD_S_SPEED:
        ESP_GOTO_ON_FALSE(data, ESP_ERR_INVALID_ARG, err, TAG, "can't set speed to null");
        // check if driver is stopped; configuration should be changed only when transmitting/receiving is not active
        ESP_GOTO_ON_FALSE(atomic_load(&eth_driver->fsm) == ESP_ETH_FSM_STOP, ESP_ERR_INVALID_STATE, err, TAG, "link configuration is only allowed when driver is stopped");
        ESP_GOTO_ON_FALSE(eth_driver->auto_nego_en == false, ESP_ERR_INVALID_STATE, err, TAG, "autonegotiation needs to be disabled to change this parameter");
        ESP_GOTO_ON_ERROR(phy->set_speed(phy, *(eth_speed_t *)data), err, TAG, "set speed mode failed");
        break;
    case ETH_CMD_G_SPEED:
        ESP_GOTO_ON_FALSE(data, ESP_ERR_INVALID_ARG, err, TAG, "no mem to store speed value");
        *(eth_speed_t *)data = eth_driver->speed;
        break;
    case ETH_CMD_S_PROMISCUOUS:
        ESP_GOTO_ON_FALSE(data, ESP_ERR_INVALID_ARG, err, TAG, "can't set promiscuous to null");
        ESP_GOTO_ON_ERROR(mac->set_promiscuous(mac, *(bool *)data), err, TAG, "set promiscuous mode failed");
        break;
    case ETH_CMD_S_FLOW_CTRL:
        ESP_GOTO_ON_FALSE(data, ESP_ERR_INVALID_ARG, err, TAG, "can't set flow ctrl to null");
        ESP_GOTO_ON_ERROR(mac->enable_flow_ctrl(mac, *(bool *)data), err, TAG, "enable mac flow control failed");
        ESP_GOTO_ON_ERROR(phy->advertise_pause_ability(phy, *(uint32_t *)data), err, TAG, "phy advertise pause ability failed");
        break;
    case ETH_CMD_S_DUPLEX_MODE:
        ESP_GOTO_ON_FALSE(data, ESP_ERR_INVALID_ARG, err, TAG, "can't set duplex to null");
        ESP_GOTO_ON_FALSE(eth_driver->auto_nego_en == false, ESP_ERR_INVALID_STATE, err, TAG, "autonegotiation needs to be disabled to change this parameter");
        // check if driver is stopped; configuration should be changed only when transmitting/receiving is not active
        ESP_GOTO_ON_FALSE(atomic_load(&eth_driver->fsm) == ESP_ETH_FSM_STOP, ESP_ERR_INVALID_STATE, err, TAG, "link configuration is only allowed when driver is stopped");
        ESP_GOTO_ON_ERROR(phy->set_duplex(phy, *(eth_duplex_t *)data), err, TAG, "set duplex mode failed");
        break;
    case ETH_CMD_G_DUPLEX_MODE:
        ESP_GOTO_ON_FALSE(data, ESP_ERR_INVALID_ARG, err, TAG, "no mem to store duplex value");
        *(eth_duplex_t *)data = eth_driver->duplex;
        break;
    case ETH_CMD_S_PHY_LOOPBACK:
        ESP_GOTO_ON_FALSE(data, ESP_ERR_INVALID_ARG, err, TAG, "can't set loopback to null");
        ESP_GOTO_ON_ERROR(phy->loopback(phy, *(bool *)data), err, TAG, "configuration of phy loopback mode failed");
        break;
    case ETH_CMD_READ_PHY_REG: {
        uint32_t phy_addr;
        ESP_GOTO_ON_FALSE(data, ESP_ERR_INVALID_ARG, err, TAG, "invalid register read/write info");
        esp_eth_phy_reg_rw_data_t *phy_r_data = (esp_eth_phy_reg_rw_data_t *)data;
        ESP_GOTO_ON_FALSE(phy_r_data->reg_value_p, ESP_ERR_INVALID_ARG, err, TAG, "can't read registers to null");
        ESP_GOTO_ON_ERROR(phy->get_addr(phy, &phy_addr), err, TAG, "get phy address failed");
        ESP_GOTO_ON_ERROR(eth_driver->mediator.phy_reg_read(&eth_driver->mediator,
                          phy_addr, phy_r_data->reg_addr, phy_r_data->reg_value_p), err, TAG, "failed to read PHY register");
        }
        break;
    case ETH_CMD_WRITE_PHY_REG: {
        uint32_t phy_addr;
        ESP_GOTO_ON_FALSE(data, ESP_ERR_INVALID_ARG, err, TAG, "invalid register read/write info");
        esp_eth_phy_reg_rw_data_t *phy_w_data = (esp_eth_phy_reg_rw_data_t *)data;
        ESP_GOTO_ON_FALSE(phy_w_data->reg_value_p, ESP_ERR_INVALID_ARG, err, TAG, "can't write registers from null");
        ESP_GOTO_ON_ERROR(phy->get_addr(phy, &phy_addr), err, TAG, "get phy address failed");
        ESP_GOTO_ON_ERROR(eth_driver->mediator.phy_reg_write(&eth_driver->mediator,
                          phy_addr, phy_w_data->reg_addr, *(phy_w_data->reg_value_p)), err, TAG, "failed to write PHY register");
        }
        break;
    default:
        if (phy->custom_ioctl != NULL && cmd >= ETH_CMD_CUSTOM_PHY_CMDS) {
            ret = phy->custom_ioctl(phy, cmd, data);
        } else if (mac->custom_ioctl != NULL && cmd >= ETH_CMD_CUSTOM_MAC_CMDS) {
            ret = mac->custom_ioctl(mac, cmd, data);
        } else {
            ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "unknown io command: %d", cmd);
        }
        break;
    }
err:
    return ret;
}

esp_err_t esp_eth_get_phy_instance(esp_eth_handle_t hdl, esp_eth_phy_t **phy)
{
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)hdl;
    ESP_RETURN_ON_FALSE(eth_driver, ESP_ERR_INVALID_ARG, TAG, "ethernet driver handle can't be null");
    ESP_RETURN_ON_FALSE(phy != NULL, ESP_ERR_INVALID_ARG, TAG, "can't store PHY instance to null");
    *phy = eth_driver->phy;
    return ESP_OK;
}

esp_err_t esp_eth_get_mac_instance(esp_eth_handle_t hdl, esp_eth_mac_t **mac)
{
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)hdl;
    ESP_RETURN_ON_FALSE(eth_driver, ESP_ERR_INVALID_ARG, TAG, "ethernet driver handle can't be null");
    ESP_RETURN_ON_FALSE(mac != NULL, ESP_ERR_INVALID_ARG, TAG, "can't store MAC instance to null");
    *mac = eth_driver->mac;
    return ESP_OK;
}

esp_err_t esp_eth_increase_reference(esp_eth_handle_t hdl)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)hdl;
    ESP_GOTO_ON_FALSE(eth_driver, ESP_ERR_INVALID_ARG, err, TAG, "ethernet driver handle can't be null");
    atomic_fetch_add(&eth_driver->ref_count, 1);
err:
    return ret;
}

esp_err_t esp_eth_decrease_reference(esp_eth_handle_t hdl)
{
    esp_err_t ret = ESP_OK;
    esp_eth_driver_t *eth_driver = (esp_eth_driver_t *)hdl;
    ESP_GOTO_ON_FALSE(eth_driver, ESP_ERR_INVALID_ARG, err, TAG, "ethernet driver handle can't be null");
    atomic_fetch_sub(&eth_driver->ref_count, 1);
err:
    return ret;
}
