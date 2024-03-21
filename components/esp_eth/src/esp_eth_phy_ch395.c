/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2024 Sergey Kharenko
 * SPDX-FileContributor: 2024 Espressif Systems (Shanghai) CO LTD
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_eth_phy.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_eth_phy_802_3.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"

#include "ch395.h"
#include "hal/eth_types.h"

#define CH395_WAIT_FOR_RESET_MS (50) // wait for CH395 hardware reset
#define CH395_WAIT_FOR_PHY_SET_MS (350)

static const char *TAG = "ch395.phy";

typedef struct {
    esp_eth_phy_t parent;
    esp_eth_mediator_t *eth;
    int addr;
    uint32_t reset_timeout_ms;
    uint32_t autonego_timeout_ms;
    eth_link_t link_status;
    int reset_gpio_num;
} phy_ch395_t;

static esp_err_t ch395_update_link_duplex_speed(phy_ch395_t *ch395)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = ch395->eth;
    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;

    uint8_t status;
    eth_link_t link;

    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ch395->addr, CMD01_GET_PHY_STATUS,
                                        (uint32_t *)(&status)),
                      err, TAG, "read PHY status failed");

    switch (status) {
    case PHY_DISCONN: link = ETH_LINK_DOWN; break;

    case PHY_10M_HALF:
    case PHY_10M_FLL:
    case PHY_100M_HALF:
    case PHY_100M_FLL: link = ETH_LINK_UP; break;

    default:
        ESP_LOGE(TAG, "PHY in unknow state!");
        return ESP_ERR_INVALID_STATE;
    }

    /* check if link status changed */
    if (ch395->link_status != link) {
        /* when link up, read negotiation result */
        if (link == ETH_LINK_UP) {
            switch (status) {
            case PHY_10M_HALF:
                speed = ETH_SPEED_10M;
                duplex = ETH_DUPLEX_HALF;
                break;

            case PHY_10M_FLL:
                speed = ETH_SPEED_10M;
                duplex = ETH_DUPLEX_FULL;
                break;

            case PHY_100M_HALF:
                speed = ETH_SPEED_100M;
                duplex = ETH_DUPLEX_HALF;
                break;

            case PHY_100M_FLL:
                speed = ETH_SPEED_100M;
                duplex = ETH_DUPLEX_FULL;
                break;

            default:
                ESP_LOGE(TAG, "PHY in unknow state!");
                return ESP_ERR_INVALID_STATE;
            }

            ESP_GOTO_ON_ERROR(
                eth->on_state_changed(eth, ETH_STATE_SPEED, (void *)speed), err,
                TAG, "change speed failed");
            ESP_GOTO_ON_ERROR(
                eth->on_state_changed(eth, ETH_STATE_DUPLEX, (void *)duplex),
                err, TAG, "change duplex failed");
        }
        ESP_GOTO_ON_ERROR(
            eth->on_state_changed(eth, ETH_STATE_LINK, (void *)link), err, TAG,
            "change link failed");
        ch395->link_status = link;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ch395_set_mediator(esp_eth_phy_t *phy,
                                    esp_eth_mediator_t *eth)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(eth, ESP_ERR_INVALID_ARG, err, TAG,
                      "mediator can't be null");
    phy_ch395_t *ch395 = __containerof(phy, phy_ch395_t, parent);
    ch395->eth = eth;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ch395_get_link(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_ch395_t *ch395 = __containerof(phy, phy_ch395_t, parent);
    /* Updata information about link, speed, duplex */
    ESP_GOTO_ON_ERROR(ch395_update_link_duplex_speed(ch395), err, TAG,
                      "update link duplex speed failed");
    return ESP_OK;
err:
    return ret;
}

// static esp_err_t ch395_set_link(esp_eth_phy_t *phy, eth_link_t link)
// {
//     esp_err_t ret = ESP_OK;
//     phy_ch395_t *ch395 = __containerof(phy, phy_ch395_t, parent);
//     esp_eth_mediator_t *eth   = ch395->eth;

//     if (ch395->link_status != link) {
//         ch395->link_status = link;
//         // link status changed, inmiedately report to upper layers
//         ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_LINK, (void
//         *)ch395->link_status), err, TAG, "change link failed");
//     }

// err:
//     return ret;
// }

static esp_err_t ch395_reset(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_ch395_t *ch395 = __containerof(phy, phy_ch395_t, parent);
    ch395->link_status = ETH_LINK_DOWN;
    esp_eth_mediator_t *eth = ch395->eth;
    uint8_t mode = PHY_AUTO;

    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ch395->addr, CMD10_SET_PHY, mode),
                      err, TAG, "send RESET cmd failed");
    vTaskDelay(pdMS_TO_TICKS(CH395_WAIT_FOR_PHY_SET_MS));

err:
    return ret;
}

static esp_err_t ch395_reset_hw(esp_eth_phy_t *phy)
{
    phy_ch395_t *ch395 = __containerof(phy, phy_ch395_t, parent);
    // set reset_gpio_num to a negative value can skip hardware reset phy chip
    if (ch395->reset_gpio_num >= 0) {
        esp_rom_gpio_pad_select_gpio(ch395->reset_gpio_num);
        gpio_set_direction(ch395->reset_gpio_num, GPIO_MODE_OUTPUT);
        gpio_set_level(ch395->reset_gpio_num, 0);
        esp_rom_delay_us(100); // insert min input assert time
        gpio_set_level(ch395->reset_gpio_num, 1);
        vTaskDelay(pdMS_TO_TICKS(CH395_WAIT_FOR_RESET_MS));
    }

    return ESP_OK;
}

static esp_err_t ch395_autonego_ctrl(esp_eth_phy_t *phy,
                                     eth_phy_autoneg_cmd_t cmd,
                                     bool *autonego_en_stat)
{
    esp_err_t ret = ESP_OK;
    phy_ch395_t *ch395 = __containerof(phy, phy_ch395_t, parent);
    esp_eth_mediator_t *eth = ch395->eth;

    uint8_t status;
    uint8_t mode;

    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ch395->addr, CMD01_GET_PHY_STATUS,
                                        (uint32_t *) & (status)),
                      err, TAG, "read PHY status failed");

    switch (cmd) {
    case ESP_ETH_PHY_AUTONEGO_RESTART:
    case ESP_ETH_PHY_AUTONEGO_EN:
        mode = PHY_AUTO;
        ESP_GOTO_ON_ERROR(
            eth->phy_reg_write(eth, ch395->addr, CMD10_SET_PHY, mode), err, TAG,
            "set auto negotiate failed");
        vTaskDelay(pdMS_TO_TICKS(CH395_WAIT_FOR_PHY_SET_MS));
        break;

    case ESP_ETH_PHY_AUTONEGO_DIS:
        ESP_LOGW(
            TAG,
            "autonego cannot be turned off individually, manually set up speed and link instead");
        break;

    case ESP_ETH_PHY_AUTONEGO_G_STAT: *autonego_en_stat = true; break;

    default: return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
err:
    return ret;
}

static esp_err_t ch395_pwrctl(esp_eth_phy_t *phy, bool enable)
{
    // power control is not supported for CH395 internal PHY
    return ESP_OK;
}

static esp_err_t ch395_set_addr(esp_eth_phy_t *phy, uint32_t addr)
{
    phy_ch395_t *ch395 = __containerof(phy, phy_ch395_t, parent);
    ch395->addr = addr;
    return ESP_OK;
}

static esp_err_t ch395_get_addr(esp_eth_phy_t *phy, uint32_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG,
                      "addr can't be null");
    phy_ch395_t *ch395 = __containerof(phy, phy_ch395_t, parent);
    *addr = ch395->addr;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ch395_del(esp_eth_phy_t *phy)
{
    phy_ch395_t *ch395 = __containerof(phy, phy_ch395_t, parent);
    free(ch395);
    return ESP_OK;
}

static esp_err_t ch395_advertise_pause_ability(esp_eth_phy_t *phy,
        uint32_t ability)
{
    // pause ability advertisement is not supported for CH395 internal PHY
    return ESP_OK;
}

static esp_err_t ch395_loopback(esp_eth_phy_t *phy, bool enable)
{
    // Loopback is not supported for CH395 internal PHY
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t ch395_set_speed(esp_eth_phy_t *phy, eth_speed_t speed)
{
    esp_err_t ret = ESP_OK;
    phy_ch395_t *ch395 = __containerof(phy, phy_ch395_t, parent);
    esp_eth_mediator_t *eth = ch395->eth;

    /* Since the link is going to be reconfigured, consider it down to be status
     * updated once the driver re-started */
    ch395->link_status = ETH_LINK_DOWN;

    uint8_t status;
    uint8_t mode;

    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ch395->addr, CMD01_GET_PHY_STATUS,
                                        (uint32_t *) & (status)),
                      err, TAG, "read PHY status failed");

    if (status == PHY_100M_FLL || status == PHY_10M_FLL) {
        if (speed == ETH_SPEED_100M) {
            mode = PHY_100M_FLL;
        } else {
            mode = PHY_10M_FLL;
        }
        ESP_GOTO_ON_ERROR(
            eth->phy_reg_write(eth, ch395->addr, CMD10_SET_PHY, mode), err, TAG,
            "set speed failed");
        vTaskDelay(pdMS_TO_TICKS(CH395_WAIT_FOR_PHY_SET_MS));
    } else if (status == PHY_100M_HALF || status == PHY_10M_HALF) {
        if (speed == ETH_SPEED_100M) {
            mode = PHY_100M_HALF;
        } else {
            mode = PHY_10M_HALF;
        }
        ESP_GOTO_ON_ERROR(
            eth->phy_reg_write(eth, ch395->addr, CMD10_SET_PHY, mode), err, TAG,
            "set speed failed");
        vTaskDelay(pdMS_TO_TICKS(CH395_WAIT_FOR_PHY_SET_MS));
    } else {
        ESP_LOGE(TAG, "cannot set speed");
        return ESP_ERR_INVALID_STATE;
    }

err:
    return ret;
}

static esp_err_t ch395_set_duplex(esp_eth_phy_t *phy, eth_duplex_t duplex)
{
    esp_err_t ret = ESP_OK;
    phy_ch395_t *ch395 = __containerof(phy, phy_ch395_t, parent);
    esp_eth_mediator_t *eth = ch395->eth;

    /* Since the link is going to be reconfigured, consider it down to be status
     * updated once the driver re-started */
    ch395->link_status = ETH_LINK_DOWN;

    uint8_t status;
    uint8_t mode;

    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ch395->addr, CMD01_GET_PHY_STATUS,
                                        (uint32_t *) & (status)),
                      err, TAG, "read PHY status failed");

    if (status == PHY_100M_FLL || status == PHY_100M_HALF) {
        if (duplex == ETH_DUPLEX_FULL) {
            mode = PHY_100M_FLL;
        } else {
            mode = PHY_100M_HALF;
        }
        ESP_GOTO_ON_ERROR(
            eth->phy_reg_write(eth, ch395->addr, CMD10_SET_PHY, mode), err, TAG,
            "set duplex failed");
        vTaskDelay(pdMS_TO_TICKS(CH395_WAIT_FOR_PHY_SET_MS));
    } else if (status == PHY_10M_FLL || status == PHY_10M_HALF) {
        if (duplex == ETH_DUPLEX_FULL) {
            mode = PHY_10M_FLL;
        } else {
            mode = PHY_10M_HALF;
        }
        ESP_GOTO_ON_ERROR(
            eth->phy_reg_write(eth, ch395->addr, CMD10_SET_PHY, mode), err, TAG,
            "set duplex failed");
        vTaskDelay(pdMS_TO_TICKS(CH395_WAIT_FOR_PHY_SET_MS));
    } else {
        ESP_LOGE(TAG, "cannot set duplex");
        return ESP_ERR_INVALID_STATE;
    }

err:
    return ret;
}

static esp_err_t ch395_init(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    /* Power on Ethernet PHY */
    ESP_GOTO_ON_ERROR(ch395_pwrctl(phy, true), err, TAG,
                      "power control failed");
    /* Reset Ethernet PHY */
    ESP_GOTO_ON_ERROR(ch395_reset(phy), err, TAG, "reset failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ch395_deinit(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    /* Power off Ethernet PHY */
    ESP_GOTO_ON_ERROR(ch395_pwrctl(phy, false), err, TAG,
                      "power control failed");
    return ESP_OK;
err:
    return ret;
}

esp_eth_phy_t *esp_eth_phy_new_ch395(const eth_phy_config_t *config)
{
    esp_eth_phy_t *ret = NULL;
    ESP_GOTO_ON_FALSE(config, NULL, err, TAG, "invalid arguments");
    phy_ch395_t *ch395 = calloc(1, sizeof(phy_ch395_t));
    ESP_GOTO_ON_FALSE(ch395, NULL, err, TAG, "no mem for PHY instance");
    ch395->addr = config->phy_addr;
    ch395->reset_timeout_ms = config->reset_timeout_ms;
    ch395->reset_gpio_num = config->reset_gpio_num;
    ch395->link_status = ETH_LINK_DOWN;
    ch395->autonego_timeout_ms = config->autonego_timeout_ms;
    ch395->parent.reset = ch395_reset;
    ch395->parent.reset_hw = ch395_reset_hw;
    ch395->parent.init = ch395_init;
    ch395->parent.deinit = ch395_deinit;
    ch395->parent.set_mediator = ch395_set_mediator;
    ch395->parent.autonego_ctrl = ch395_autonego_ctrl;
    ch395->parent.get_link = ch395_get_link;
    // ch395->parent.set_link = ch395_set_link;
    ch395->parent.pwrctl = ch395_pwrctl;
    ch395->parent.get_addr = ch395_get_addr;
    ch395->parent.set_addr = ch395_set_addr;
    ch395->parent.advertise_pause_ability = ch395_advertise_pause_ability;
    ch395->parent.loopback = ch395_loopback;
    ch395->parent.set_speed = ch395_set_speed;
    ch395->parent.set_duplex = ch395_set_duplex;
    ch395->parent.del = ch395_del;
    return &(ch395->parent);
err:
    return ret;
}
