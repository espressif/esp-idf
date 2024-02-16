/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include "esp_log.h"
#include "esp_check.h"
#include "esp_eth_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"
#include "w5500.h"

#define W5500_WAIT_FOR_RESET_MS (10) // wait for W5500 internal PLL to be Locked after reset assert

static const char *TAG = "w5500.phy";

/***************Vendor Specific Register***************/
/**
 * @brief PHYCFGR(PHY Configuration Register)
 *
 */
typedef union {
    struct {
        uint8_t link: 1;   /*!< Link status */
        uint8_t speed: 1;  /*!< Speed status */
        uint8_t duplex: 1; /*!< Duplex status */
        uint8_t opmode: 3; /*!< Operation mode */
        uint8_t opsel: 1;  /*!< Operation select */
        uint8_t reset: 1;  /*!< Reset, when this bit is '0', PHY will get reset */
    };
    uint8_t val;
} phycfg_reg_t;

typedef enum {
    W5500_OP_MODE_10BT_HALF_AUTO_DIS,
    W5500_OP_MODE_10BT_FULL_AUTO_DIS,
    W5500_OP_MODE_100BT_HALF_AUTO_DIS,
    W5500_OP_MODE_100BT_FULL_AUTO_DIS,
    W5500_OP_MODE_100BT_HALF_AUTO_EN,
    W5500_OP_MODE_NOT_USED,
    W5500_OP_MODE_PWR_DOWN,
    W5500_OP_MODE_ALL_CAPABLE,
} phy_w5500_op_mode_e;

typedef struct {
    esp_eth_phy_t parent;
    esp_eth_mediator_t *eth;
    int addr;
    uint32_t reset_timeout_ms;
    uint32_t autonego_timeout_ms;
    eth_link_t link_status;
    int reset_gpio_num;
} phy_w5500_t;

static esp_err_t w5500_update_link_duplex_speed(phy_w5500_t *w5500)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = w5500->eth;
    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    phycfg_reg_t phycfg;

    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, w5500->addr, W5500_REG_PHYCFGR, (uint32_t *) & (phycfg.val)), err, TAG, "read PHYCFG failed");
    eth_link_t link = phycfg.link ? ETH_LINK_UP : ETH_LINK_DOWN;
    /* check if link status changed */
    if (w5500->link_status != link) {
        /* when link up, read negotiation result */
        if (link == ETH_LINK_UP) {
            if (phycfg.speed) {
                speed = ETH_SPEED_100M;
            } else {
                speed = ETH_SPEED_10M;
            }
            if (phycfg.duplex) {
                duplex = ETH_DUPLEX_FULL;
            } else {
                duplex = ETH_DUPLEX_HALF;
            }
            ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_SPEED, (void *)speed), err, TAG, "change speed failed");
            ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_DUPLEX, (void *)duplex), err, TAG, "change duplex failed");
        }
        ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_LINK, (void *)link), err, TAG, "change link failed");
        w5500->link_status = link;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t w5500_set_mediator(esp_eth_phy_t *phy, esp_eth_mediator_t *eth)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(eth, ESP_ERR_INVALID_ARG, err, TAG, "mediator can't be null");
    phy_w5500_t *w5500 = __containerof(phy, phy_w5500_t, parent);
    w5500->eth = eth;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t w5500_get_link(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_w5500_t *w5500 = __containerof(phy, phy_w5500_t, parent);
    /* Updata information about link, speed, duplex */
    ESP_GOTO_ON_ERROR(w5500_update_link_duplex_speed(w5500), err, TAG, "update link duplex speed failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t w5500_set_link(esp_eth_phy_t *phy, eth_link_t link)
{
    esp_err_t ret = ESP_OK;
    phy_w5500_t *w5500 = __containerof(phy, phy_w5500_t, parent);
    esp_eth_mediator_t *eth   = w5500->eth;

    if (w5500->link_status != link) {
        w5500->link_status = link;
        // link status changed, inmiedately report to upper layers
        ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_LINK, (void *)w5500->link_status), err, TAG, "change link failed");
    }
err:
    return ret;
}

static esp_err_t w5500_reset(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_w5500_t *w5500 = __containerof(phy, phy_w5500_t, parent);
    w5500->link_status = ETH_LINK_DOWN;
    esp_eth_mediator_t *eth = w5500->eth;
    phycfg_reg_t phycfg;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, w5500->addr, W5500_REG_PHYCFGR, (uint32_t *) & (phycfg.val)), err, TAG, "read PHYCFG failed");
    phycfg.reset = 0; // set to '0' will reset internal PHY
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, w5500->addr, W5500_REG_PHYCFGR, phycfg.val), err, TAG, "write PHYCFG failed");
    vTaskDelay(pdMS_TO_TICKS(W5500_WAIT_FOR_RESET_MS));
    phycfg.reset = 1; // set to '1' after reset
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, w5500->addr, W5500_REG_PHYCFGR, phycfg.val), err, TAG, "write PHYCFG failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t w5500_reset_hw(esp_eth_phy_t *phy)
{
    phy_w5500_t *w5500 = __containerof(phy, phy_w5500_t, parent);
    // set reset_gpio_num to a negative value can skip hardware reset phy chip
    if (w5500->reset_gpio_num >= 0) {
        esp_rom_gpio_pad_select_gpio(w5500->reset_gpio_num);
        gpio_set_direction(w5500->reset_gpio_num, GPIO_MODE_OUTPUT);
        gpio_set_level(w5500->reset_gpio_num, 0);
        esp_rom_delay_us(100); // insert min input assert time
        gpio_set_level(w5500->reset_gpio_num, 1);
    }
    return ESP_OK;
}

static esp_err_t w5500_autonego_ctrl(esp_eth_phy_t *phy, eth_phy_autoneg_cmd_t cmd, bool *autonego_en_stat)
{
    esp_err_t ret = ESP_OK;
    phy_w5500_t *w5500 = __containerof(phy, phy_w5500_t, parent);
    esp_eth_mediator_t *eth = w5500->eth;

    phycfg_reg_t phycfg;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, w5500->addr, W5500_REG_PHYCFGR, (uint32_t *) & (phycfg.val)), err, TAG, "read PHYCFG failed");

    switch (cmd) {
    case ESP_ETH_PHY_AUTONEGO_RESTART:
        ESP_GOTO_ON_FALSE(phycfg.opmode == W5500_OP_MODE_ALL_CAPABLE || phycfg.opmode == W5500_OP_MODE_100BT_HALF_AUTO_EN,
                            ESP_ERR_INVALID_STATE, err, TAG, "auto negotiation is disabled");
        /* in case any link status has changed, let's assume we're in link down status */
        w5500->link_status = ETH_LINK_DOWN;

        phycfg.opsel = 1; // Configure PHY Operation Mode based on registry setting
        phycfg.reset = 0; // PHY needs to be reset after configuring opsel and opmode
        ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, w5500->addr, W5500_REG_PHYCFGR, phycfg.val), err, TAG, "write PHYCFG failed");
        vTaskDelay(pdMS_TO_TICKS(W5500_WAIT_FOR_RESET_MS));
        phycfg.reset = 1; // reset flag needs to be put back to 1
        ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, w5500->addr, W5500_REG_PHYCFGR, phycfg.val), err, TAG, "write PHYCFG failed");

        *autonego_en_stat = phycfg.opmode == W5500_OP_MODE_ALL_CAPABLE || phycfg.opmode == W5500_OP_MODE_100BT_HALF_AUTO_EN;
        break;
    case ESP_ETH_PHY_AUTONEGO_DIS:
        /* W5500 autonegotiation cannot be separately disabled, only specific speed/duplex mode needs to be configured. Hence set the
        last used configuration */
        if (phycfg.duplex) { // Full duplex
            if (phycfg.speed) { // 100 Mbps speed
                phycfg.opmode = W5500_OP_MODE_100BT_FULL_AUTO_DIS;
            } else {
                phycfg.opmode = W5500_OP_MODE_10BT_FULL_AUTO_DIS;
            }
        } else {
            if (phycfg.speed) { // 100 Mbps speed
                phycfg.opmode = W5500_OP_MODE_100BT_HALF_AUTO_DIS;
            } else {
                phycfg.opmode = W5500_OP_MODE_10BT_HALF_AUTO_DIS;
            }
        }
        phycfg.opsel = 1;
        ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, w5500->addr, W5500_REG_PHYCFGR, phycfg.val), err, TAG, "write PHYCFG failed");
        *autonego_en_stat = false;
        break;
    case ESP_ETH_PHY_AUTONEGO_EN:
        phycfg.opsel = 1;                          // PHY working mode configured by register
        phycfg.opmode = W5500_OP_MODE_ALL_CAPABLE; // all capable, auto-negotiation enabled
        ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, w5500->addr, W5500_REG_PHYCFGR, phycfg.val), err, TAG, "write PHYCFG failed");
        *autonego_en_stat = true;
        break;
    case ESP_ETH_PHY_AUTONEGO_G_STAT:
        *autonego_en_stat = phycfg.opmode == W5500_OP_MODE_ALL_CAPABLE || phycfg.opmode == W5500_OP_MODE_100BT_HALF_AUTO_EN;
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
err:
    return ret;
}

static esp_err_t w5500_pwrctl(esp_eth_phy_t *phy, bool enable)
{
    // power control is not supported for W5500 internal PHY
    return ESP_OK;
}

static esp_err_t w5500_set_addr(esp_eth_phy_t *phy, uint32_t addr)
{
    phy_w5500_t *w5500 = __containerof(phy, phy_w5500_t, parent);
    w5500->addr = addr;
    return ESP_OK;
}

static esp_err_t w5500_get_addr(esp_eth_phy_t *phy, uint32_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "addr can't be null");
    phy_w5500_t *w5500 = __containerof(phy, phy_w5500_t, parent);
    *addr = w5500->addr;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t w5500_del(esp_eth_phy_t *phy)
{
    phy_w5500_t *w5500 = __containerof(phy, phy_w5500_t, parent);
    free(w5500);
    return ESP_OK;
}

static esp_err_t w5500_advertise_pause_ability(esp_eth_phy_t *phy, uint32_t ability)
{
    // pause ability advertisement is not supported for W5500 internal PHY
    return ESP_OK;
}

static esp_err_t w5500_loopback(esp_eth_phy_t *phy, bool enable)
{
    // Loopback is not supported for W5500 internal PHY
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t w5500_set_speed(esp_eth_phy_t *phy, eth_speed_t speed)
{
    esp_err_t ret = ESP_OK;
    phy_w5500_t *w5500 = __containerof(phy, phy_w5500_t, parent);
    esp_eth_mediator_t *eth = w5500->eth;

    /* Since the link is going to be reconfigured, consider it down to be status updated once the driver re-started */
    w5500->link_status = ETH_LINK_DOWN;

    phycfg_reg_t phycfg;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, w5500->addr, W5500_REG_PHYCFGR, (uint32_t *) & (phycfg.val)), err, TAG, "read PHYCFG failed");
    if (phycfg.duplex) { // Full duplex
        if (speed == ETH_SPEED_100M) {
            phycfg.opmode = W5500_OP_MODE_100BT_FULL_AUTO_DIS;
        } else {
            phycfg.opmode = W5500_OP_MODE_10BT_FULL_AUTO_DIS;
        }
    } else {
        if (speed == ETH_SPEED_100M) {
            phycfg.opmode = W5500_OP_MODE_100BT_HALF_AUTO_DIS;
        } else {
            phycfg.opmode = W5500_OP_MODE_10BT_HALF_AUTO_DIS;
        }
    }
    phycfg.opsel = 1;  // PHY working mode configured by register
    phycfg.reset = 0;  // PHY needs to be reset after configuring opsel and opmode
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, w5500->addr, W5500_REG_PHYCFGR, phycfg.val), err, TAG, "write PHYCFG failed");
    vTaskDelay(pdMS_TO_TICKS(W5500_WAIT_FOR_RESET_MS));
    phycfg.reset = 1; // reset flag needs to be put back to 1
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, w5500->addr, W5500_REG_PHYCFGR, phycfg.val), err, TAG, "write PHYCFG failed");

err:
    return ret;
}

static esp_err_t w5500_set_duplex(esp_eth_phy_t *phy, eth_duplex_t duplex)
{
    esp_err_t ret = ESP_OK;
    phy_w5500_t *w5500 = __containerof(phy, phy_w5500_t, parent);
    esp_eth_mediator_t *eth = w5500->eth;

    /* Since the link is going to be reconfigured, consider it down to be status updated once the driver re-started */
    w5500->link_status = ETH_LINK_DOWN;

    phycfg_reg_t phycfg;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, w5500->addr, W5500_REG_PHYCFGR, (uint32_t *) & (phycfg.val)), err, TAG, "read PHYCFG failed");
    if (phycfg.speed) { // 100Mbps
        if (duplex == ETH_DUPLEX_FULL) {
            phycfg.opmode = W5500_OP_MODE_100BT_FULL_AUTO_DIS;
        } else {
            phycfg.opmode = W5500_OP_MODE_100BT_HALF_AUTO_DIS;
        }
    } else {
        if (duplex == ETH_DUPLEX_FULL) {
            phycfg.opmode = W5500_OP_MODE_10BT_FULL_AUTO_DIS;
        } else {
            phycfg.opmode = W5500_OP_MODE_10BT_HALF_AUTO_DIS;
        }
    }
    phycfg.opsel = 1;  // PHY working mode configured by register
    phycfg.reset = 0;  // PHY needs to be reset after configuring opsel and opmode
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, w5500->addr, W5500_REG_PHYCFGR, phycfg.val), err, TAG, "write PHYCFG failed");
    vTaskDelay(pdMS_TO_TICKS(W5500_WAIT_FOR_RESET_MS));
    phycfg.reset = 1; // reset flag needs to be put back to 1
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, w5500->addr, W5500_REG_PHYCFGR, phycfg.val), err, TAG, "write PHYCFG failed");
err:
    return ret;
}

static esp_err_t w5500_init(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    /* Power on Ethernet PHY */
    ESP_GOTO_ON_ERROR(w5500_pwrctl(phy, true), err, TAG, "power control failed");
    /* Reset Ethernet PHY */
    ESP_GOTO_ON_ERROR(w5500_reset(phy), err, TAG, "reset failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t w5500_deinit(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    /* Power off Ethernet PHY */
    ESP_GOTO_ON_ERROR(w5500_pwrctl(phy, false), err, TAG, "power control failed");
    return ESP_OK;
err:
    return ret;
}

esp_eth_phy_t *esp_eth_phy_new_w5500(const eth_phy_config_t *config)
{
    esp_eth_phy_t *ret = NULL;
    ESP_GOTO_ON_FALSE(config, NULL, err, TAG, "invalid arguments");
    phy_w5500_t *w5500 = calloc(1, sizeof(phy_w5500_t));
    ESP_GOTO_ON_FALSE(w5500, NULL, err, TAG, "no mem for PHY instance");
    w5500->addr = config->phy_addr;
    w5500->reset_timeout_ms = config->reset_timeout_ms;
    w5500->reset_gpio_num = config->reset_gpio_num;
    w5500->link_status = ETH_LINK_DOWN;
    w5500->autonego_timeout_ms = config->autonego_timeout_ms;
    w5500->parent.reset = w5500_reset;
    w5500->parent.reset_hw = w5500_reset_hw;
    w5500->parent.init = w5500_init;
    w5500->parent.deinit = w5500_deinit;
    w5500->parent.set_mediator = w5500_set_mediator;
    w5500->parent.autonego_ctrl = w5500_autonego_ctrl;
    w5500->parent.get_link = w5500_get_link;
    w5500->parent.set_link = w5500_set_link;
    w5500->parent.pwrctl = w5500_pwrctl;
    w5500->parent.get_addr = w5500_get_addr;
    w5500->parent.set_addr = w5500_set_addr;
    w5500->parent.advertise_pause_ability = w5500_advertise_pause_ability;
    w5500->parent.loopback = w5500_loopback;
    w5500->parent.set_speed = w5500_set_speed;
    w5500->parent.set_duplex = w5500_set_duplex;
    w5500->parent.del = w5500_del;
    return &(w5500->parent);
err:
    return ret;
}
