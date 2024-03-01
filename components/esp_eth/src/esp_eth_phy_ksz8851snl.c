/*
 * SPDX-FileCopyrightText: 2021 Vladimir Chistyakov
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2021-2024 Espressif Systems (Shanghai) CO LTD
 */
#include <stdlib.h>
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_eth_driver.h"
#include "ksz8851.h"


typedef struct {
    esp_eth_phy_t parent;
    esp_eth_mediator_t *eth;
    int32_t addr;
    uint32_t reset_timeout_ms;
    uint32_t autonego_timeout_ms;
    eth_link_t link_status;
    int reset_gpio_num;
} phy_ksz8851snl_t;


static const char *TAG = "ksz8851snl-phy";


static esp_err_t ksz8851_update_link_duplex_speed(phy_ksz8851snl_t *ksz8851)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = ksz8851->eth;
    eth_speed_t speed       = ETH_SPEED_10M;
    eth_duplex_t duplex     = ETH_DUPLEX_HALF;
    uint32_t status;

    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz8851->addr, KSZ8851_P1SR, &status), err, TAG, "P1SR read failed");
    eth_link_t link = (status & P1SR_LINK_GOOD) ? ETH_LINK_UP : ETH_LINK_DOWN;
    if (ksz8851->link_status != link) {
        if (link == ETH_LINK_UP) {
            if (status & P1SR_OPERATION_SPEED) {
                speed = ETH_SPEED_100M;
                ESP_LOGD(TAG, "speed 100M");
            } else {
                speed = ETH_SPEED_10M;
                ESP_LOGD(TAG, "speed 10M");
            }
            if (status & P1SR_OPERATION_DUPLEX) {
                duplex = ETH_DUPLEX_FULL;
                ESP_LOGD(TAG, "duplex full");
            } else {
                duplex = ETH_DUPLEX_HALF;
                ESP_LOGD(TAG, "duplex half");
            }
            ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_SPEED, (void *)speed), err, TAG, "change speed failed");
            ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_DUPLEX, (void *)duplex), err, TAG, "change duplex failed");
        }
        ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_LINK, (void *)link), err, TAG, "change link failed");
        ksz8851->link_status = link;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t phy_ksz8851_set_mediator(esp_eth_phy_t *phy, esp_eth_mediator_t *eth)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(eth, ESP_ERR_INVALID_ARG, err, TAG, "mediator can not be null");
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    ksz8851->eth              = eth;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t phy_ksz8851_reset(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    ksz8851->link_status      = ETH_LINK_DOWN;
    esp_eth_mediator_t *eth   = ksz8851->eth;
    ESP_LOGD(TAG, "soft reset");
    // NOTE(v.chistyakov): PHY_RESET bit is self-clearing
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz8851->addr, KSZ8851_PHYRR, PHYRR_PHY_RESET), err, TAG, "PHYRR write failed");
    vTaskDelay(pdMS_TO_TICKS(ksz8851->reset_timeout_ms));
    return ESP_OK;
err:
    return ret;
}

static esp_err_t phy_ksz8851_reset_hw(esp_eth_phy_t *phy)
{
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    // NOTE(v.chistyakov): set reset_gpio_num to a negative value can skip hardware reset phy chip
    if (ksz8851->reset_gpio_num >= 0) {
        ESP_LOGD(TAG, "hard reset");
        esp_rom_gpio_pad_select_gpio(ksz8851->reset_gpio_num);
        gpio_set_direction(ksz8851->reset_gpio_num, GPIO_MODE_OUTPUT);
        gpio_set_level(ksz8851->reset_gpio_num, 0);
        esp_rom_delay_us(ksz8851->reset_timeout_ms * 1000);
        gpio_set_level(ksz8851->reset_gpio_num, 1);
    }
    return ESP_OK;
}

static esp_err_t phy_ksz8851_pwrctl(esp_eth_phy_t *phy, bool enable)
{
    esp_err_t ret = ESP_OK;
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    esp_eth_mediator_t *eth   = ksz8851->eth;
    if (enable) {
        ESP_LOGD(TAG, "normal mode");
        ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz8851->addr, KSZ8851_PMECR, PMECR_PME_MODE_POWER_SAVING), err, TAG, "PMECR write failed");
    } else {
        ESP_LOGD(TAG, "power saving mode");
        ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz8851->addr, KSZ8851_PMECR, PMECR_PME_MODE_NORMAL), err, TAG, "PMECR write failed");
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t phy_ksz8851_init(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    ESP_LOGD(TAG, "initializing PHY");
    ESP_GOTO_ON_ERROR(phy_ksz8851_pwrctl(phy, true), err, TAG, "power control failed");
    ESP_GOTO_ON_ERROR(phy_ksz8851_reset(phy), err, TAG, "reset failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t phy_ksz8851_deinit(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    ESP_LOGD(TAG, "deinitializing PHY");
    ESP_GOTO_ON_ERROR(phy_ksz8851_pwrctl(phy, false), err, TAG, "power control failed");
    return ESP_OK;
err:
    return ret;
}

/**
 * @note This function is responsible for restarting a new auto-negotiation,
 *       the result of negotiation won't be relected to uppler layers.
 *       Instead, the negotiation result is fetched by linker timer, see `phy_ksz8851_get_link()`
 */
static esp_err_t phy_ksz8851_autonego_ctrl(esp_eth_phy_t *phy, eth_phy_autoneg_cmd_t cmd, bool *autonego_en_stat)
{
    esp_err_t ret = ESP_OK;
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    esp_eth_mediator_t *eth   = ksz8851->eth;

    uint32_t control;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz8851->addr, KSZ8851_P1CR, &control), err, TAG, "P1CR read failed");

    switch (cmd) {
    case ESP_ETH_PHY_AUTONEGO_RESTART:
        ESP_GOTO_ON_FALSE(control & P1CR_AUTO_NEGOTIATION_ENABLE, ESP_ERR_INVALID_STATE, err, TAG, "auto negotiation is disabled");
        ESP_LOGD(TAG, "restart negotiation");
        /* in case any link status has changed, let's assume we're in link down status */
        ksz8851->link_status = ETH_LINK_DOWN;
        ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz8851->addr, KSZ8851_P1CR, control | P1CR_RESTART_AN), err, TAG, "P1CR write failed");

        uint32_t status;
        unsigned to;
        for (to = 0; to < ksz8851->autonego_timeout_ms / 100; to++) {
            vTaskDelay(pdMS_TO_TICKS(100));
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz8851->addr, KSZ8851_P1SR, &status), err, TAG, "P1SR read failed");
            if (status & P1SR_AN_DONE) {
                break;
            }
        }
        if ((to >= ksz8851->autonego_timeout_ms / 100) && (ksz8851->link_status == ETH_LINK_UP)) {
            ESP_LOGW(TAG, "auto negotiation timeout");
        }

        ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz8851->addr, KSZ8851_P1CR, control), err, TAG, "P1CR write failed");

        ESP_LOGD(TAG, "negotiation succeeded");
        break;
    case ESP_ETH_PHY_AUTONEGO_DIS:
        if (control & P1CR_AUTO_NEGOTIATION_ENABLE) {
            control &= ~P1CR_AUTO_NEGOTIATION_ENABLE;     /* Disable Auto Negotiation */
            ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz8851->addr, KSZ8851_P1CR, control), err, TAG, "P1CR write failed");
            /* read configuration back */
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz8851->addr, KSZ8851_P1CR, &control), err, TAG, "P1CR read failed");
            ESP_GOTO_ON_FALSE((control & P1CR_AUTO_NEGOTIATION_ENABLE) == 0, ESP_FAIL, err, TAG, "disable auto-negotiation failed");
        }
        break;
    case ESP_ETH_PHY_AUTONEGO_EN:
        if (!(control & P1CR_AUTO_NEGOTIATION_ENABLE)) {
            control |= P1CR_AUTO_NEGOTIATION_ENABLE;     /* Enable Auto Negotiation */
            ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz8851->addr, KSZ8851_P1CR, control), err, TAG, "P1CR write failed");
            /* read configuration back */
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz8851->addr, KSZ8851_P1CR, &control), err, TAG, "P1CR read failed");
            ESP_GOTO_ON_FALSE(control & P1CR_AUTO_NEGOTIATION_ENABLE, ESP_FAIL, err, TAG, "disable auto-negotiation failed");
        }
        break;
    case ESP_ETH_PHY_AUTONEGO_G_STAT:
        /* do nothing autonego_en_stat is set at the function end */
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    *autonego_en_stat = (control & P1CR_AUTO_NEGOTIATION_ENABLE) != 0;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t phy_ksz8851_get_link(esp_eth_phy_t *phy)
{
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    return ksz8851_update_link_duplex_speed(ksz8851);
}

static esp_err_t phy_ksz8851_set_link(esp_eth_phy_t *phy, eth_link_t link)
{
    esp_err_t ret = ESP_OK;
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    esp_eth_mediator_t *eth   = ksz8851->eth;

    if (ksz8851->link_status != link) {
        ksz8851->link_status = link;
        // link status changed, inmiedately report to upper layers
        ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_LINK, (void *)ksz8851->link_status), err, TAG, "change link failed");
    }
err:
    return ret;
}

static esp_err_t phy_ksz8851_set_addr(esp_eth_phy_t *phy, uint32_t addr)
{
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    ksz8851->addr             = addr;
    ESP_LOGD(TAG, "setting PHY addr to %" PRIu32, addr);
    return ESP_OK;
}

static esp_err_t phy_ksz8851_get_addr(esp_eth_phy_t *phy, uint32_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "addr can not be null");
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    *addr                     = ksz8851->addr;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t phy_ksz8851_advertise_pause_ability(esp_eth_phy_t *phy, uint32_t ability)
{
    esp_err_t ret = ESP_OK;
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    esp_eth_mediator_t *eth   = ksz8851->eth;

    uint32_t anar;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz8851->addr, KSZ8851_P1ANAR, &anar), err, TAG, "P1ANAR read failed");
    if (ability) {
        ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz8851->addr, KSZ8851_P1ANAR, anar | P1ANAR_PAUSE), err, TAG, "P1ANAR write failed");
        ESP_LOGD(TAG, "start advertising pause ability");
    } else {
        ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz8851->addr, KSZ8851_P1ANAR, anar & ~P1ANAR_PAUSE), err, TAG, "P1ANAR write failed");
        ESP_LOGD(TAG, "stop advertising pause ability");
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t phy_ksz8851_loopback(esp_eth_phy_t *phy, bool enable)
{
    esp_err_t ret = ESP_OK;
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    esp_eth_mediator_t *eth   = ksz8851->eth;

    uint32_t mbcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz8851->addr, KSZ8851_P1MBCR, &mbcr), err, TAG, "P1MBCR read failed");
    if (enable) {
        ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz8851->addr, KSZ8851_P1MBCR, mbcr | P1MBCR_LOCAL_LOOPBACK), err, TAG, "P1MBCR write failed");
        ESP_LOGD(TAG, "set Local (far-end) loopback");
    } else {
        ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz8851->addr, KSZ8851_P1MBCR, mbcr & ~P1MBCR_LOCAL_LOOPBACK), err, TAG, "P1MBCR write failed");
        ESP_LOGD(TAG, "disabled Local (far-end) loopback");
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t phy_ksz8851_set_speed(esp_eth_phy_t *phy, eth_speed_t speed)
{
    esp_err_t ret = ESP_OK;
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    esp_eth_mediator_t *eth   = ksz8851->eth;

    /* Since the link is going to be reconfigured, consider it down to be status updated once the driver re-started */
    ksz8851->link_status = ETH_LINK_DOWN;

    /* Set speed */
    uint32_t control;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz8851->addr, KSZ8851_P1CR, &control), err, TAG, "P1CR read failed");
    if (speed == ETH_SPEED_100M) {
        control |= P1CR_FORCE_SPEED;
    } else {
        control &= ~P1CR_FORCE_SPEED;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz8851->addr, KSZ8851_P1CR, control), err, TAG, "P1CR write failed");

    return ESP_OK;
err:
    return ret;
}

static esp_err_t phy_ksz8851_set_duplex(esp_eth_phy_t *phy, eth_duplex_t duplex)
{
    esp_err_t ret = ESP_OK;
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    esp_eth_mediator_t *eth   = ksz8851->eth;

    /* Since the link is going to be reconfigured, consider it down to be status updated once the driver re-started */
    ksz8851->link_status = ETH_LINK_DOWN;

    /* Set duplex mode */
    uint32_t control;
    uint32_t mbcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz8851->addr, KSZ8851_P1CR, &control), err, TAG, "P1CR read failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz8851->addr, KSZ8851_P1MBCR, &mbcr), err, TAG, "P1MBCR read failed");
    if (mbcr & P1MBCR_LOCAL_LOOPBACK) {
        ESP_GOTO_ON_FALSE(duplex == ETH_DUPLEX_FULL, ESP_ERR_INVALID_STATE, err, TAG, "Duplex mode must be FULL for loopback operation");
    }
    if (duplex == ETH_DUPLEX_FULL) {
        control |= P1CR_FORCE_DUPLEX;
    } else {
        control &= ~P1CR_FORCE_DUPLEX;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz8851->addr, KSZ8851_P1CR, control), err, TAG, "P1CR write failed");

    return ESP_OK;
err:
    return ret;
}

static esp_err_t phy_ksz8851_del(esp_eth_phy_t *phy)
{
    ESP_LOGD(TAG, "deleting PHY");
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    free(ksz8851);
    return ESP_OK;
}

esp_eth_phy_t *esp_eth_phy_new_ksz8851snl(const eth_phy_config_t *config)
{
    esp_eth_phy_t *ret = NULL;
    ESP_GOTO_ON_FALSE(config, NULL, err, TAG, "config can not be null");
    phy_ksz8851snl_t *ksz8851 = calloc(1, sizeof(phy_ksz8851snl_t));
    ESP_GOTO_ON_FALSE(ksz8851, NULL, err, TAG, "no mem for PHY instance");
    ksz8851->addr                           = config->phy_addr;
    ksz8851->reset_timeout_ms               = config->reset_timeout_ms;
    ksz8851->reset_gpio_num                 = config->reset_gpio_num;
    ksz8851->link_status                    = ETH_LINK_DOWN;
    ksz8851->autonego_timeout_ms            = config->autonego_timeout_ms;
    ksz8851->parent.set_mediator            = phy_ksz8851_set_mediator;
    ksz8851->parent.reset                   = phy_ksz8851_reset;
    ksz8851->parent.reset_hw                = phy_ksz8851_reset_hw;
    ksz8851->parent.init                    = phy_ksz8851_init;
    ksz8851->parent.deinit                  = phy_ksz8851_deinit;
    ksz8851->parent.autonego_ctrl           = phy_ksz8851_autonego_ctrl;
    ksz8851->parent.get_link                = phy_ksz8851_get_link;
    ksz8851->parent.set_link                = phy_ksz8851_set_link;
    ksz8851->parent.pwrctl                  = phy_ksz8851_pwrctl;
    ksz8851->parent.set_addr                = phy_ksz8851_set_addr;
    ksz8851->parent.get_addr                = phy_ksz8851_get_addr;
    ksz8851->parent.advertise_pause_ability = phy_ksz8851_advertise_pause_ability;
    ksz8851->parent.loopback                = phy_ksz8851_loopback;
    ksz8851->parent.set_speed               = phy_ksz8851_set_speed;
    ksz8851->parent.set_duplex              = phy_ksz8851_set_duplex;
    ksz8851->parent.del                     = phy_ksz8851_del;
    return &(ksz8851->parent);
err:
    return ret;
}
