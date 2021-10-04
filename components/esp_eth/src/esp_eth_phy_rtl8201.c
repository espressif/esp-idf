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

#include <string.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include "esp_log.h"
#include "esp_check.h"
#include "esp_eth.h"
#include "eth_phy_regs_struct.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"

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
    esp_eth_phy_t parent;
    esp_eth_mediator_t *eth;
    int addr;
    uint32_t reset_timeout_ms;
    uint32_t autonego_timeout_ms;
    eth_link_t link_status;
    int reset_gpio_num;
} phy_rtl8201_t;

static esp_err_t rtl8201_page_select(phy_rtl8201_t *rtl8201, uint32_t page)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = rtl8201->eth;
    psr_reg_t psr = {
        .page_select = page
    };
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, rtl8201->addr, ETH_PHY_PSR_REG_ADDR, psr.val), err, TAG, "write PSR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t rtl8201_update_link_duplex_speed(phy_rtl8201_t *rtl8201)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = rtl8201->eth;
    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    bmcr_reg_t bmcr;
    bmsr_reg_t bmsr;
    uint32_t peer_pause_ability = false;
    anlpar_reg_t anlpar;
    ESP_GOTO_ON_ERROR(rtl8201_page_select(rtl8201, 0), err, TAG, "select page 0 failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, rtl8201->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)), err, TAG, "read BMSR failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, rtl8201->addr, ETH_PHY_ANLPAR_REG_ADDR, &(anlpar.val)), err, TAG, "read ANLPAR failed");
    eth_link_t link = bmsr.link_status ? ETH_LINK_UP : ETH_LINK_DOWN;
    /* check if link status changed */
    if (rtl8201->link_status != link) {
        /* when link up, read negotiation result */
        if (link == ETH_LINK_UP) {
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, rtl8201->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
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
        rtl8201->link_status = link;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t rtl8201_set_mediator(esp_eth_phy_t *phy, esp_eth_mediator_t *eth)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(eth, ESP_ERR_INVALID_ARG, err, TAG, "can't set mediator to null");
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    rtl8201->eth = eth;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t rtl8201_get_link(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    /* Updata information about link, speed, duplex */
    ESP_GOTO_ON_ERROR(rtl8201_update_link_duplex_speed(rtl8201), err, TAG, "update link duplex speed failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t rtl8201_reset(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    rtl8201->link_status = ETH_LINK_DOWN;
    esp_eth_mediator_t *eth = rtl8201->eth;
    bmcr_reg_t bmcr = {.reset = 1};
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, rtl8201->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    /* Wait for reset complete */
    uint32_t to = 0;
    for (to = 0; to < rtl8201->reset_timeout_ms / 10; to++) {
        vTaskDelay(pdMS_TO_TICKS(10));
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, rtl8201->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
        if (!bmcr.reset) {
            break;
        }
    }
    ESP_GOTO_ON_FALSE(to < rtl8201->reset_timeout_ms / 10, ESP_FAIL, err, TAG, "reset timeout");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t rtl8201_reset_hw(esp_eth_phy_t *phy)
{
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    if (rtl8201->reset_gpio_num >= 0) {
        esp_rom_gpio_pad_select_gpio(rtl8201->reset_gpio_num);
        gpio_set_direction(rtl8201->reset_gpio_num, GPIO_MODE_OUTPUT);
        gpio_set_level(rtl8201->reset_gpio_num, 0);
        esp_rom_delay_us(100); // insert min input assert time
        gpio_set_level(rtl8201->reset_gpio_num, 1);
    }
    return ESP_OK;
}

/**
 * @note This function is responsible for restarting a new auto-negotiation,
 *       the result of negotiation won't be relected to uppler layers.
 *       Instead, the negotiation result is fetched by linker timer, see `rtl8201_get_link()`
 */
static esp_err_t rtl8201_negotiate(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    esp_eth_mediator_t *eth = rtl8201->eth;
    /* in case any link status has changed, let's assume we're in link down status */
    rtl8201->link_status = ETH_LINK_DOWN;
    /* Restart auto negotiation */
    bmcr_reg_t bmcr = {
        .speed_select = 1,     /* 100Mbps */
        .duplex_mode = 1,      /* Full Duplex */
        .en_auto_nego = 1,     /* Auto Negotiation */
        .restart_auto_nego = 1 /* Restart Auto Negotiation */
    };
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, rtl8201->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    /* Wait for auto negotiation complete */
    bmsr_reg_t bmsr;
    uint32_t to = 0;
    for (to = 0; to < rtl8201->autonego_timeout_ms / 100; to++) {
        vTaskDelay(pdMS_TO_TICKS(100));
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, rtl8201->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)), err, TAG, "read BMSR failed");
        if (bmsr.auto_nego_complete) {
            break;
        }
    }
    if ((to >= rtl8201->autonego_timeout_ms / 100) && (rtl8201->link_status == ETH_LINK_UP)) {
        ESP_LOGW(TAG, "auto negotiation timeout");
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t rtl8201_pwrctl(esp_eth_phy_t *phy, bool enable)
{
    esp_err_t ret = ESP_OK;
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    esp_eth_mediator_t *eth = rtl8201->eth;
    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, rtl8201->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    if (!enable) {
        /* Enable IEEE Power Down Mode */
        bmcr.power_down = 1;
    } else {
        /* Disable IEEE Power Down Mode */
        bmcr.power_down = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, rtl8201->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    if (!enable) {
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, rtl8201->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
        ESP_GOTO_ON_FALSE(bmcr.power_down == 1, ESP_FAIL, err, TAG, "power down failed");
    } else {
        /* wait for power up complete */
        uint32_t to = 0;
        for (to = 0; to < rtl8201->reset_timeout_ms / 10; to++) {
            vTaskDelay(pdMS_TO_TICKS(10));
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, rtl8201->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
            if (bmcr.power_down == 0) {
                break;
            }
        }
        ESP_GOTO_ON_FALSE(to < rtl8201->reset_timeout_ms / 10, ESP_FAIL, err, TAG, "power up timeout");
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t rtl8201_set_addr(esp_eth_phy_t *phy, uint32_t addr)
{
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    rtl8201->addr = addr;
    return ESP_OK;
}

static esp_err_t rtl8201_get_addr(esp_eth_phy_t *phy, uint32_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "addr can't be null");
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    *addr = rtl8201->addr;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t rtl8201_del(esp_eth_phy_t *phy)
{
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    free(rtl8201);
    return ESP_OK;
}

static esp_err_t rtl8201_advertise_pause_ability(esp_eth_phy_t *phy, uint32_t ability)
{
    esp_err_t ret = ESP_OK;
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    esp_eth_mediator_t *eth = rtl8201->eth;
    /* Set PAUSE function ability */
    anar_reg_t anar;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, rtl8201->addr, ETH_PHY_ANAR_REG_ADDR, &(anar.val)), err, TAG, "read ANAR failed");
    if (ability) {
        anar.asymmetric_pause = 1;
        anar.symmetric_pause = 1;
    } else {
        anar.asymmetric_pause = 0;
        anar.symmetric_pause = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, rtl8201->addr, ETH_PHY_ANAR_REG_ADDR, anar.val), err, TAG, "write ANAR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t rtl8201_loopback(esp_eth_phy_t *phy, bool enable)
{
    esp_err_t ret = ESP_OK;
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    esp_eth_mediator_t *eth = rtl8201->eth;
    /* Set Loopback function */
    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, rtl8201->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    if (enable) {
        bmcr.en_loopback = 1;
    } else {
        bmcr.en_loopback = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, rtl8201->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t rtl8201_init(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    esp_eth_mediator_t *eth = rtl8201->eth;
    // Detect PHY address
    if (rtl8201->addr == ESP_ETH_PHY_ADDR_AUTO) {
        ESP_GOTO_ON_ERROR(esp_eth_detect_phy_addr(eth, &rtl8201->addr), err, TAG, "Detect PHY address failed");
    }
    /* Power on Ethernet PHY */
    ESP_GOTO_ON_ERROR(rtl8201_pwrctl(phy, true), err, TAG, "power control failed");
    /* Reset Ethernet PHY */
    ESP_GOTO_ON_ERROR(rtl8201_reset(phy), err, TAG, "reset failed");
    /* Check PHY ID */
    phyidr1_reg_t id1;
    phyidr2_reg_t id2;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, rtl8201->addr, ETH_PHY_IDR1_REG_ADDR, &(id1.val)), err, TAG, "read ID1 failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, rtl8201->addr, ETH_PHY_IDR2_REG_ADDR, &(id2.val)), err, TAG, "read ID2 failed");
    ESP_GOTO_ON_FALSE(id1.oui_msb == 0x1C && id2.oui_lsb == 0x32 && id2.vendor_model == 0x1, ESP_FAIL, err, TAG, "wrong chip ID");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t rtl8201_deinit(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    /* Power off Ethernet PHY */
    ESP_GOTO_ON_ERROR(rtl8201_pwrctl(phy, false), err, TAG, "power control failed");
    return ESP_OK;
err:
    return ret;
}

esp_eth_phy_t *esp_eth_phy_new_rtl8201(const eth_phy_config_t *config)
{
    esp_eth_phy_t *ret = NULL;
    ESP_GOTO_ON_FALSE(config, NULL, err, TAG, "can't set phy config to null");
    phy_rtl8201_t *rtl8201 = calloc(1, sizeof(phy_rtl8201_t));
    ESP_GOTO_ON_FALSE(rtl8201, NULL, err, TAG, "calloc rtl8201 failed");
    rtl8201->addr = config->phy_addr;
    rtl8201->reset_gpio_num = config->reset_gpio_num;
    rtl8201->reset_timeout_ms = config->reset_timeout_ms;
    rtl8201->link_status = ETH_LINK_DOWN;
    rtl8201->autonego_timeout_ms = config->autonego_timeout_ms;
    rtl8201->parent.reset = rtl8201_reset;
    rtl8201->parent.reset_hw = rtl8201_reset_hw;
    rtl8201->parent.init = rtl8201_init;
    rtl8201->parent.deinit = rtl8201_deinit;
    rtl8201->parent.set_mediator = rtl8201_set_mediator;
    rtl8201->parent.negotiate = rtl8201_negotiate;
    rtl8201->parent.get_link = rtl8201_get_link;
    rtl8201->parent.pwrctl = rtl8201_pwrctl;
    rtl8201->parent.get_addr = rtl8201_get_addr;
    rtl8201->parent.set_addr = rtl8201_set_addr;
    rtl8201->parent.advertise_pause_ability = rtl8201_advertise_pause_ability;
    rtl8201->parent.loopback = rtl8201_loopback;
    rtl8201->parent.del = rtl8201_del;

    return &(rtl8201->parent);
err:
    return ret;
}
