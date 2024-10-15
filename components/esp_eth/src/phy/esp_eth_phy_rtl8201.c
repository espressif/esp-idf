/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_eth_phy_802_3.h"

#define RTL8201_PHY_RESET_ASSERTION_TIME_US 10000
#define RTL8201_PHY_POST_RESET_INIT_TIME_MS 150

static const char *TAG = "rtl8201";

/***************Vendor Specific Register***************/

/**
 * @brief PSMR(Power Saving Mode Register)
 *
 */
typedef union {
    struct {
        uint16_t reserved : 15;   /* Reserved */
        uint16_t en_pwr_save : 1; /* Enable power saving mode */
    };
    uint16_t val;
} psmr_reg_t;
#define ETH_PHY_PSMR_REG_ADDR (0x18)

/**
 * @brief PSR(Page Select Register)
 *
 */
typedef union {
    struct {
        uint16_t page_select : 8; /* Select register page, default is 0 */
        uint16_t reserved : 8;    /* Reserved */
    };
    uint16_t val;
} psr_reg_t;
#define ETH_PHY_PSR_REG_ADDR (0x1F)

typedef struct {
    phy_802_3_t phy_802_3;
} phy_rtl8201_t;

static esp_err_t rtl8201_page_select(phy_rtl8201_t *rtl8201, uint32_t page)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = rtl8201->phy_802_3.eth;
    psr_reg_t psr = {
        .page_select = page
    };
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, rtl8201->phy_802_3.addr, ETH_PHY_PSR_REG_ADDR, psr.val), err, TAG, "write PSR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t rtl8201_update_link_duplex_speed(phy_rtl8201_t *rtl8201)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = rtl8201->phy_802_3.eth;
    uint32_t addr = rtl8201->phy_802_3.addr;
    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    bmcr_reg_t bmcr;
    bmsr_reg_t bmsr;
    uint32_t peer_pause_ability = false;
    anlpar_reg_t anlpar;
    ESP_GOTO_ON_ERROR(rtl8201_page_select(rtl8201, 0), err, TAG, "select page 0 failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)), err, TAG, "read BMSR failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_ANLPAR_REG_ADDR, &(anlpar.val)), err, TAG, "read ANLPAR failed");
    eth_link_t link = bmsr.link_status ? ETH_LINK_UP : ETH_LINK_DOWN;
    /* check if link status changed */
    if (rtl8201->phy_802_3.link_status != link) {
        /* when link up, read negotiation result */
        if (link == ETH_LINK_UP) {
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
            if (bmcr.speed_select) {
                speed = ETH_SPEED_100M;
            } else {
                speed = ETH_SPEED_10M;
            }
            if (bmcr.duplex_mode) {
                duplex = ETH_DUPLEX_FULL;
            } else {
                duplex = ETH_DUPLEX_HALF;
            }
            ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_SPEED, (void *)speed), err, TAG, "change speed failed");
            ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_DUPLEX, (void *)duplex), err, TAG, "change duplex failed");
            /* if we're in duplex mode, and peer has the flow control ability */
            if (duplex == ETH_DUPLEX_FULL && anlpar.symmetric_pause) {
                peer_pause_ability = 1;
            } else {
                peer_pause_ability = 0;
            }
            ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_PAUSE, (void *)peer_pause_ability), err, TAG, "change pause ability failed");
        }
        ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_LINK, (void *)link), err, TAG, "change link failed");
        rtl8201->phy_802_3.link_status = link;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t rtl8201_get_link(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_rtl8201_t *rtl8201 = __containerof(esp_eth_phy_into_phy_802_3(phy), phy_rtl8201_t, phy_802_3);
    /* Updata information about link, speed, duplex */
    ESP_GOTO_ON_ERROR(rtl8201_update_link_duplex_speed(rtl8201), err, TAG, "update link duplex speed failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t rtl8201_autonego_ctrl(esp_eth_phy_t *phy, eth_phy_autoneg_cmd_t cmd, bool *autonego_en_stat)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = esp_eth_phy_into_phy_802_3(phy);
    esp_eth_mediator_t *eth = phy_802_3->eth;
    if (cmd == ESP_ETH_PHY_AUTONEGO_EN) {
        bmcr_reg_t bmcr;
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
        ESP_GOTO_ON_FALSE(bmcr.en_loopback == 0, ESP_ERR_INVALID_STATE, err, TAG, "Autonegotiation can't be enabled while in loopback operation");
    }
    return esp_eth_phy_802_3_autonego_ctrl(phy_802_3, cmd, autonego_en_stat);
err:
    return ret;
}

static esp_err_t rtl8201_loopback(esp_eth_phy_t *phy, bool enable)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = esp_eth_phy_into_phy_802_3(phy);
    bool auto_nego_en;
    ESP_GOTO_ON_ERROR(rtl8201_autonego_ctrl(phy, ESP_ETH_PHY_AUTONEGO_G_STAT, &auto_nego_en), err, TAG, "get status of autonegotiation failed");
    ESP_GOTO_ON_FALSE(!(auto_nego_en && enable), ESP_ERR_INVALID_STATE, err, TAG, "Unable to set loopback while autonegotiation is enabled. Disable it to use loopback");
    return esp_eth_phy_802_3_loopback(phy_802_3, enable);
err:
    return ret;
}

static esp_err_t rtl8201_init(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = esp_eth_phy_into_phy_802_3(phy);

    /* Basic PHY init */
    ESP_GOTO_ON_ERROR(esp_eth_phy_802_3_basic_phy_init(phy_802_3), err, TAG, "failed to init PHY");

    /* Check PHY ID */
    uint32_t oui;
    uint8_t model;
    ESP_GOTO_ON_ERROR(esp_eth_phy_802_3_read_oui(phy_802_3, &oui), err, TAG, "read OUI failed");
    ESP_GOTO_ON_ERROR(esp_eth_phy_802_3_read_manufac_info(phy_802_3, &model, NULL), err, TAG, "read manufacturer's info failed");
    ESP_GOTO_ON_FALSE(oui == 0x732 && model == 0x1, ESP_FAIL, err, TAG, "wrong chip ID");

    return ESP_OK;
err:
    return ret;
}

esp_eth_phy_t *esp_eth_phy_new_rtl8201(const eth_phy_config_t *config)
{
    esp_eth_phy_t *ret = NULL;
    phy_rtl8201_t *rtl8201 = calloc(1, sizeof(phy_rtl8201_t));
    ESP_GOTO_ON_FALSE(rtl8201, NULL, err, TAG, "calloc rtl8201 failed");
    eth_phy_config_t rtl8201_config = *config;
    // default chip specific configuration
    if (config->hw_reset_assert_time_us == 0) {
        rtl8201_config.hw_reset_assert_time_us = RTL8201_PHY_RESET_ASSERTION_TIME_US;
    }
    if (config->post_hw_reset_delay_ms == 0) {
        rtl8201_config.post_hw_reset_delay_ms = RTL8201_PHY_POST_RESET_INIT_TIME_MS;
    }
    ESP_GOTO_ON_FALSE(esp_eth_phy_802_3_obj_config_init(&rtl8201->phy_802_3, &rtl8201_config) == ESP_OK,
                        NULL, err, TAG, "configuration initialization of PHY 802.3 failed");

    // redefine functions which need to be customized for sake of RTL8201
    rtl8201->phy_802_3.parent.init = rtl8201_init;
    rtl8201->phy_802_3.parent.get_link = rtl8201_get_link;
    rtl8201->phy_802_3.parent.autonego_ctrl = rtl8201_autonego_ctrl;
    rtl8201->phy_802_3.parent.loopback = rtl8201_loopback;

    return &rtl8201->phy_802_3.parent;
err:
    if (rtl8201 != NULL) {
        free(rtl8201);
    }
    return ret;
}
