/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include "esp_log.h"
#include "esp_check.h"
#include "esp_eth.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"
#include "esp_eth_phy_802_3.h"

static const char *TAG = "eth_phy_802_3";

static esp_err_t eth_phy_802_3_set_mediator(esp_eth_phy_t *phy, esp_eth_mediator_t *eth)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = __containerof(phy, phy_802_3_t, parent);
    ESP_GOTO_ON_FALSE(eth, ESP_ERR_INVALID_ARG, err, TAG, "mediator can't be null");
    phy_802_3->eth = eth;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t eth_phy_802_3_reset(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = __containerof(phy, phy_802_3_t, parent);
    phy_802_3->link_status = ETH_LINK_DOWN;
    esp_eth_mediator_t *eth = phy_802_3->eth;
    bmcr_reg_t bmcr = {.reset = 1};
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    /* wait for reset complete */
    uint32_t to = 0;
    for (to = 0; to < phy_802_3->reset_timeout_ms / 10; to++) {
        vTaskDelay(pdMS_TO_TICKS(10));
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
        if (!bmcr.reset) {
            break;
        }
    }
    ESP_GOTO_ON_FALSE(to < phy_802_3->reset_timeout_ms / 10, ESP_FAIL, err, TAG, "reset timeout");
    return ESP_OK;
err:
    return ret;
}

/**
 * @brief PHY hardware reset with default assert time
 *
 * @note Default reset assertion time is selected to be 100us as it is most commonly used value among PHY chips.
 *       If your PHY chip requires different value, redefine the `reset_hw` function in derived PHY specific driver structure.
 *
 * @param phy Ethernet PHY instance
 * @return
 *         - ESP_OK on success
 */
static esp_err_t eth_phy_802_3_reset_hw_default(esp_eth_phy_t *phy)
{
    phy_802_3_t *phy_802_3 = __containerof(phy, phy_802_3_t, parent);
    return esp_eth_phy_802_3_reset_hw(phy_802_3, 100);
}

static esp_err_t eth_phy_802_3_autonego_ctrl(esp_eth_phy_t *phy, eth_phy_autoneg_cmd_t cmd, bool *autonego_en_stat)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = __containerof(phy, phy_802_3_t, parent);
    esp_eth_mediator_t *eth = phy_802_3->eth;

    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");

    switch (cmd) {
    case ESP_ETH_PHY_AUTONEGO_RESTART:
        ESP_GOTO_ON_FALSE(bmcr.en_auto_nego, ESP_ERR_INVALID_STATE, err, TAG, "auto negotiation is disabled");
        /* in case any link status has changed, let's assume we're in link down status */
        phy_802_3->link_status = ETH_LINK_DOWN;

        bmcr.restart_auto_nego = 1; /* Restart Auto Negotiation */

        ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
        /* Wait for auto negotiation complete */
        bmsr_reg_t bmsr;
        uint32_t to = 0;
        for (to = 0; to < phy_802_3->autonego_timeout_ms / 100; to++) {
            vTaskDelay(pdMS_TO_TICKS(100));
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, phy_802_3->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)), err, TAG, "read BMSR failed");
            if (bmsr.auto_nego_complete) {
                break;
            }
        }
        if ((to >= phy_802_3->autonego_timeout_ms / 100) && (phy_802_3->link_status == ETH_LINK_UP)) {
            ESP_LOGW(TAG, "auto negotiation timeout");
        }
        break;
    case ESP_ETH_PHY_AUTONEGO_DIS:
        if (bmcr.en_auto_nego == 1) {
            bmcr.en_auto_nego = 0;     /* Disable Auto Negotiation */
            ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
            /* read configuration back */
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
            ESP_GOTO_ON_FALSE(bmcr.en_auto_nego == 0, ESP_FAIL, err, TAG, "disable auto-negotiation failed");
        }
        break;
    case ESP_ETH_PHY_AUTONEGO_EN:
        if (bmcr.en_auto_nego == 0) {
            bmcr.en_auto_nego = 1;     /* Enable Auto Negotiation */
            ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
            /* read configuration back */
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
            ESP_GOTO_ON_FALSE(bmcr.en_auto_nego == 1, ESP_FAIL, err, TAG, "enable auto-negotiation failed");
        }
        break;
    case ESP_ETH_PHY_AUTONEGO_G_STAT:
        /* do nothing autonego_en_stat is set at the function end */
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    *autonego_en_stat = bmcr.en_auto_nego;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t eth_phy_802_3_pwrctl(esp_eth_phy_t *phy, bool enable)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = __containerof(phy, phy_802_3_t, parent);
    esp_eth_mediator_t *eth = phy_802_3->eth;
    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    if (!enable) {
        /* Enable IEEE Power Down Mode */
        bmcr.power_down = 1;
    } else {
        /* Disable IEEE Power Down Mode */
        bmcr.power_down = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    if (!enable) {
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
        ESP_GOTO_ON_FALSE(bmcr.power_down == 1, ESP_FAIL, err, TAG, "power down failed");
    } else {
        /* wait for power up complete */
        uint32_t to = 0;
        for (to = 0; to < phy_802_3->reset_timeout_ms / 10; to++) {
            vTaskDelay(pdMS_TO_TICKS(10));
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
            if (bmcr.power_down == 0) {
                break;
            }
        }
        ESP_GOTO_ON_FALSE(to < phy_802_3->reset_timeout_ms / 10, ESP_FAIL, err, TAG, "power up timeout");
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t eth_phy_802_3_set_addr(esp_eth_phy_t *phy, uint32_t addr)
{
    phy_802_3_t *phy_802_3 = __containerof(phy, phy_802_3_t, parent);
    phy_802_3->addr = addr;
    return ESP_OK;
}

static esp_err_t eth_phy_802_3_get_addr(esp_eth_phy_t *phy, uint32_t *addr)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = __containerof(phy, phy_802_3_t, parent);
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "addr can't be null");
    *addr = phy_802_3->addr;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t eth_phy_802_3_advertise_pause_ability(esp_eth_phy_t *phy, uint32_t ability)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = __containerof(phy, phy_802_3_t, parent);
    esp_eth_mediator_t *eth = phy_802_3->eth;
    /* Set PAUSE function ability */
    anar_reg_t anar;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, phy_802_3->addr, ETH_PHY_ANAR_REG_ADDR, &(anar.val)), err, TAG, "read ANAR failed");
    if (ability) {
        anar.asymmetric_pause = 1;
        anar.symmetric_pause = 1;
    } else {
        anar.asymmetric_pause = 0;
        anar.symmetric_pause = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, phy_802_3->addr, ETH_PHY_ANAR_REG_ADDR, anar.val), err, TAG, "write ANAR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t eth_phy_802_3_loopback(esp_eth_phy_t *phy, bool enable)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = __containerof(phy, phy_802_3_t, parent);
    esp_eth_mediator_t *eth = phy_802_3->eth;
    /* Set Loopback function */
    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    if (enable) {
        bmcr.en_loopback = 1;
    } else {
        bmcr.en_loopback = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t eth_phy_802_3_set_speed(esp_eth_phy_t *phy, eth_speed_t speed)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = __containerof(phy, phy_802_3_t, parent);
    esp_eth_mediator_t *eth = phy_802_3->eth;

    if (phy_802_3->link_status == ETH_LINK_UP) {
        /* Since the link is going to be reconfigured, consider it down for a while */
        phy_802_3->link_status = ETH_LINK_DOWN;
        /* Indicate to upper stream apps the link is cosidered down */
        ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_LINK, (void *)phy_802_3->link_status), err, TAG, "change link failed");
    }
    /* Set speed */
    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    bmcr.speed_select = speed == ETH_SPEED_100M ? 1 : 0;
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");

    return ESP_OK;
err:
    return ret;
}

static esp_err_t eth_phy_802_3_set_duplex(esp_eth_phy_t *phy, eth_duplex_t duplex)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = __containerof(phy, phy_802_3_t, parent);
    esp_eth_mediator_t *eth = phy_802_3->eth;

    if (phy_802_3->link_status == ETH_LINK_UP) {
        /* Since the link is going to be reconfigured, consider it down for a while */
        phy_802_3->link_status = ETH_LINK_DOWN;
        /* Indicate to upper stream apps the link is cosidered down */
        ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_LINK, (void *)phy_802_3->link_status), err, TAG, "change link failed");
    }
    /* Set duplex mode */
    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    bmcr.duplex_mode = duplex;
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");

    return ESP_OK;
err:
    return ret;
}

static esp_err_t eth_phy_802_3_init(esp_eth_phy_t *phy)
{
    phy_802_3_t *phy_802_3 = __containerof(phy, phy_802_3_t, parent);
    return esp_eth_phy_802_3_basic_phy_init(phy_802_3);
}

static esp_err_t eth_phy_802_3_deinit(esp_eth_phy_t *phy)
{
    phy_802_3_t *phy_802_3 = __containerof(phy, phy_802_3_t, parent);
    return esp_eth_phy_802_3_basic_phy_deinit(phy_802_3);
}

static esp_err_t eth_phy_802_3_del(esp_eth_phy_t *phy)
{
    free(phy);
    return ESP_OK;
}

esp_err_t esp_eth_phy_802_3_reset_hw(phy_802_3_t *phy_802_3, uint32_t reset_assert_us)
{
    if (phy_802_3->reset_gpio_num >= 0) {
        esp_rom_gpio_pad_select_gpio(phy_802_3->reset_gpio_num);
        gpio_set_direction(phy_802_3->reset_gpio_num, GPIO_MODE_OUTPUT);
        gpio_set_level(phy_802_3->reset_gpio_num, 0);
        esp_rom_delay_us(reset_assert_us);
        gpio_set_level(phy_802_3->reset_gpio_num, 1);
    }
    return ESP_OK;
}

esp_err_t esp_eth_phy_802_3_detect_phy_addr(esp_eth_mediator_t *eth, int *detected_addr)
{
    if (!eth || !detected_addr) {
        ESP_LOGE(TAG, "eth and detected_addr can't be null");
        return ESP_ERR_INVALID_ARG;
    }
    int addr_try = 0;
    uint32_t reg_value = 0;
    for (; addr_try < 16; addr_try++) {
        eth->phy_reg_read(eth, addr_try, ETH_PHY_IDR1_REG_ADDR, &reg_value);
        if (reg_value != 0xFFFF && reg_value != 0x00) {
            *detected_addr = addr_try;
            break;
        }
    }
    if (addr_try < 16) {
        ESP_LOGD(TAG, "Found PHY address: %d", addr_try);
        return ESP_OK;
    }
    ESP_LOGE(TAG, "No PHY device detected");
    return ESP_ERR_NOT_FOUND;
}

esp_err_t esp_eth_phy_802_3_basic_phy_init(phy_802_3_t *phy_802_3)
{
    esp_err_t ret = ESP_OK;

    // Detect PHY address
    if (phy_802_3->addr == ESP_ETH_PHY_ADDR_AUTO) {
        ESP_GOTO_ON_ERROR(esp_eth_phy_802_3_detect_phy_addr(phy_802_3->eth, &phy_802_3->addr), err, TAG, "Detect PHY address failed");
    }
    /* Power on Ethernet PHY */
    ESP_GOTO_ON_ERROR(eth_phy_802_3_pwrctl(&phy_802_3->parent, true), err, TAG, "power control failed");
    /* Reset Ethernet PHY */
    ESP_GOTO_ON_ERROR(eth_phy_802_3_reset(&phy_802_3->parent), err, TAG, "reset failed");

    return ESP_OK;
err:
    return ret;
}

esp_err_t esp_eth_phy_802_3_basic_phy_deinit(phy_802_3_t *phy_802_3)
{
    esp_err_t ret = ESP_OK;
    /* Power off Ethernet PHY */
    ESP_GOTO_ON_ERROR(eth_phy_802_3_pwrctl(&phy_802_3->parent, false), err, TAG, "power control failed");
    return ESP_OK;
err:
    return ret;
}

esp_err_t esp_eth_phy_802_3_read_oui(phy_802_3_t *phy_802_3, uint32_t *oui)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = phy_802_3->eth;
    phyidr1_reg_t id1;
    phyidr2_reg_t id2;

    ESP_GOTO_ON_FALSE(oui != NULL, ESP_ERR_INVALID_ARG, err, TAG, "oui can't be null");

    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, phy_802_3->addr, ETH_PHY_IDR1_REG_ADDR, &(id1.val)), err, TAG, "read ID1 failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, phy_802_3->addr, ETH_PHY_IDR2_REG_ADDR, &(id2.val)), err, TAG, "read ID2 failed");

    *oui = (id1.oui_msb << 6) | id2.oui_lsb;

    return ESP_OK;
err:
    return ret;
}

esp_err_t esp_eth_phy_802_3_read_manufac_info(phy_802_3_t *phy_802_3, uint8_t *model, uint8_t *rev)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = phy_802_3->eth;

    phyidr2_reg_t id2;

    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, phy_802_3->addr, ETH_PHY_IDR2_REG_ADDR, &(id2.val)), err, TAG, "read ID2 failed");

    if (model != NULL) {
        *model = id2.vendor_model;
    }
    if (rev != NULL) {
        *rev = id2.model_revision;
    }

    return ESP_OK;
err:
    return ret;
}

phy_802_3_t *esp_eth_phy_into_phy_802_3(esp_eth_phy_t *phy)
{
    return __containerof(phy, phy_802_3_t, parent);
}

esp_err_t esp_eth_phy_802_3_obj_config_init(phy_802_3_t *phy_802_3, const eth_phy_config_t *config)
{
    esp_err_t ret = ESP_OK;

    ESP_GOTO_ON_FALSE(config, ESP_ERR_INVALID_ARG, err, TAG, "can't set phy config to null");

    phy_802_3->link_status = ETH_LINK_DOWN;
    phy_802_3->addr = config->phy_addr;
    phy_802_3->reset_timeout_ms = config->reset_timeout_ms;
    phy_802_3->reset_gpio_num = config->reset_gpio_num;
    phy_802_3->autonego_timeout_ms = config->autonego_timeout_ms;

    phy_802_3->parent.reset = eth_phy_802_3_reset;
    phy_802_3->parent.reset_hw = eth_phy_802_3_reset_hw_default;
    phy_802_3->parent.init = eth_phy_802_3_init;
    phy_802_3->parent.deinit = eth_phy_802_3_deinit;
    phy_802_3->parent.set_mediator = eth_phy_802_3_set_mediator;
    phy_802_3->parent.autonego_ctrl = eth_phy_802_3_autonego_ctrl;
    phy_802_3->parent.pwrctl = eth_phy_802_3_pwrctl;
    phy_802_3->parent.get_addr = eth_phy_802_3_get_addr;
    phy_802_3->parent.set_addr = eth_phy_802_3_set_addr;
    phy_802_3->parent.advertise_pause_ability = eth_phy_802_3_advertise_pause_ability;
    phy_802_3->parent.loopback = eth_phy_802_3_loopback;
    phy_802_3->parent.set_speed = eth_phy_802_3_set_speed;
    phy_802_3->parent.set_duplex = eth_phy_802_3_set_duplex;
    phy_802_3->parent.del = eth_phy_802_3_del;
    phy_802_3->parent.get_link = NULL;
    phy_802_3->parent.custom_ioctl = NULL;

err:
    return ret;
}
