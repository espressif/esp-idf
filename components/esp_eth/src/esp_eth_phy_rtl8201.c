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
#include "esp_eth.h"
#include "eth_phy_regs_struct.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "rtl8201";
#define PHY_CHECK(a, str, goto_tag, ...)                                          \
    do                                                                            \
    {                                                                             \
        if (!(a))                                                                 \
        {                                                                         \
            ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            goto goto_tag;                                                        \
        }                                                                         \
    } while (0)

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
    uint32_t addr;
    uint32_t reset_timeout_ms;
    uint32_t autonego_timeout_ms;
    eth_link_t link_status;
} phy_rtl8201_t;

static esp_err_t rtl8201_page_select(phy_rtl8201_t *rtl8201, uint32_t page)
{
    esp_eth_mediator_t *eth = rtl8201->eth;
    psr_reg_t psr = {
        .page_select = page
    };
    PHY_CHECK(eth->phy_reg_write(eth, rtl8201->addr, ETH_PHY_PSR_REG_ADDR, psr.val) == ESP_OK, "write PSR failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t rtl8201_update_link_duplex_speed(phy_rtl8201_t *rtl8201)
{
    esp_eth_mediator_t *eth = rtl8201->eth;
    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    bmcr_reg_t bmcr;
    bmsr_reg_t bmsr;
    PHY_CHECK(rtl8201_page_select(rtl8201, 0) == ESP_OK, "select page 0 failed", err);
    PHY_CHECK(eth->phy_reg_read(eth, rtl8201->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)) == ESP_OK,
              "read BMSR failed", err);
    eth_link_t link = bmsr.link_status ? ETH_LINK_UP : ETH_LINK_DOWN;
    /* check if link status changed */
    if (rtl8201->link_status != link) {
        /* when link up, read negotiation result */
        if (link == ETH_LINK_UP) {
            PHY_CHECK(eth->phy_reg_read(eth, rtl8201->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)) == ESP_OK,
                      "read BMCR failed", err);
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
            PHY_CHECK(eth->on_state_changed(eth, ETH_STATE_SPEED, (void *)speed) == ESP_OK,
                      "change speed failed", err);
            PHY_CHECK(eth->on_state_changed(eth, ETH_STATE_DUPLEX, (void *)duplex) == ESP_OK,
                      "change duplex failed", err);
        }
        PHY_CHECK(eth->on_state_changed(eth, ETH_STATE_LINK, (void *)link) == ESP_OK,
                  "change link failed", err);
        rtl8201->link_status = link;
    }
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t rtl8201_set_mediator(esp_eth_phy_t *phy, esp_eth_mediator_t *eth)
{
    PHY_CHECK(eth, "can't set mediator to null", err);
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    rtl8201->eth = eth;
    return ESP_OK;
err:
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t rtl8201_get_link(esp_eth_phy_t *phy)
{
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    /* Updata information about link, speed, duplex */
    PHY_CHECK(rtl8201_update_link_duplex_speed(rtl8201) == ESP_OK, "update link duplex speed failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t rtl8201_reset(esp_eth_phy_t *phy)
{
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    esp_eth_mediator_t *eth = rtl8201->eth;
    bmcr_reg_t bmcr = {.reset = 1};
    PHY_CHECK(eth->phy_reg_write(eth, rtl8201->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val) == ESP_OK,
              "write BMCR failed", err);
    /* Wait for reset complete */
    uint32_t to = 0;
    for (to = 0; to < rtl8201->reset_timeout_ms / 10; to++) {
        vTaskDelay(pdMS_TO_TICKS(10));
        PHY_CHECK(eth->phy_reg_read(eth, rtl8201->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)) == ESP_OK,
                  "read BMCR failed", err);
        if (!bmcr.reset) {
            break;
        }
    }
    PHY_CHECK(to < rtl8201->reset_timeout_ms / 10, "reset timeout", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t rtl8201_negotiate(esp_eth_phy_t *phy)
{
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    esp_eth_mediator_t *eth = rtl8201->eth;
    /* Restart auto negotiation */
    bmcr_reg_t bmcr = {
        .speed_select = 1,     /* 100Mbps */
        .duplex_mode = 1,      /* Full Duplex */
        .en_auto_nego = 1,     /* Auto Negotiation */
        .restart_auto_nego = 1 /* Restart Auto Negotiation */
    };
    PHY_CHECK(eth->phy_reg_write(eth, rtl8201->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val) == ESP_OK,
              "write BMCR failed", err);
    /* Wait for auto negotiation complete */
    bmsr_reg_t bmsr;
    uint32_t to = 0;
    for (to = 0; to < rtl8201->autonego_timeout_ms / 10; to++) {
        vTaskDelay(pdMS_TO_TICKS(10));
        PHY_CHECK(eth->phy_reg_read(eth, rtl8201->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)) == ESP_OK,
                  "read BMSR failed", err);
        if (bmsr.auto_nego_complete) {
            break;
        }
    }
    /* Auto negotiation failed, maybe no network cable plugged in, so output a warning */
    if (to >= rtl8201->autonego_timeout_ms / 10) {
        ESP_LOGW(TAG, "auto negotiation timeout");
    }
    /* Updata information about link, speed, duplex */
    PHY_CHECK(rtl8201_update_link_duplex_speed(rtl8201) == ESP_OK, "update link duplex speed failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t rtl8201_pwrctl(esp_eth_phy_t *phy, bool enable)
{
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    esp_eth_mediator_t *eth = rtl8201->eth;
    bmcr_reg_t bmcr;
    PHY_CHECK(eth->phy_reg_read(eth, rtl8201->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)) == ESP_OK,
              "read BMCR failed", err);
    if (!enable) {
        /* Enable IEEE Power Down Mode */
        bmcr.power_down = 1;
    } else {
        /* Disable IEEE Power Down Mode */
        bmcr.power_down = 0;
    }
    PHY_CHECK(eth->phy_reg_write(eth, rtl8201->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val) == ESP_OK,
              "write BMCR failed", err);
    PHY_CHECK(eth->phy_reg_read(eth, rtl8201->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)) == ESP_OK,
              "read BMCR failed", err);
    if (!enable) {
        PHY_CHECK(bmcr.power_down == 1, "power down failed", err);
    } else {
        PHY_CHECK(bmcr.power_down == 0, "power up failed", err);
    }
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t rtl8201_set_addr(esp_eth_phy_t *phy, uint32_t addr)
{
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    rtl8201->addr = addr;
    return ESP_OK;
}

static esp_err_t rtl8201_get_addr(esp_eth_phy_t *phy, uint32_t *addr)
{
    PHY_CHECK(addr, "addr can't be null", err);
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    *addr = rtl8201->addr;
    return ESP_OK;
err:
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t rtl8201_del(esp_eth_phy_t *phy)
{
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    free(rtl8201);
    return ESP_OK;
}

static esp_err_t rtl8201_init(esp_eth_phy_t *phy)
{
    phy_rtl8201_t *rtl8201 = __containerof(phy, phy_rtl8201_t, parent);
    esp_eth_mediator_t *eth = rtl8201->eth;
    /* Power on Ethernet PHY */
    PHY_CHECK(rtl8201_pwrctl(phy, true) == ESP_OK, "power control failed", err);
    /* Reset Ethernet PHY */
    PHY_CHECK(rtl8201_reset(phy) == ESP_OK, "reset failed", err);
    /* Check PHY ID */
    phyidr1_reg_t id1;
    phyidr2_reg_t id2;
    PHY_CHECK(eth->phy_reg_read(eth, rtl8201->addr, ETH_PHY_IDR1_REG_ADDR, &(id1.val)) == ESP_OK,
              "read ID1 failed", err);
    PHY_CHECK(eth->phy_reg_read(eth, rtl8201->addr, ETH_PHY_IDR2_REG_ADDR, &(id2.val)) == ESP_OK,
              "read ID2 failed", err);
    PHY_CHECK(id1.oui_msb == 0x1C && id2.oui_lsb == 0x32 && id2.vendor_model == 0x1,
              "wrong chip ID", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t rtl8201_deinit(esp_eth_phy_t *phy)
{
    /* Power off Ethernet PHY */
    PHY_CHECK(rtl8201_pwrctl(phy, false) == ESP_OK, "power control failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

esp_eth_phy_t *esp_eth_phy_new_rtl8201(const eth_phy_config_t *config)
{
    PHY_CHECK(config, "can't set phy config to null", err);
    phy_rtl8201_t *rtl8201 = calloc(1, sizeof(phy_rtl8201_t));
    PHY_CHECK(rtl8201, "calloc rtl8201 failed", err);
    rtl8201->addr = config->phy_addr;
    rtl8201->reset_timeout_ms = config->reset_timeout_ms;
    rtl8201->link_status = ETH_LINK_DOWN;
    rtl8201->autonego_timeout_ms = config->autonego_timeout_ms;
    rtl8201->parent.reset = rtl8201_reset;
    rtl8201->parent.init = rtl8201_init;
    rtl8201->parent.deinit = rtl8201_deinit;
    rtl8201->parent.set_mediator = rtl8201_set_mediator;
    rtl8201->parent.negotiate = rtl8201_negotiate;
    rtl8201->parent.get_link = rtl8201_get_link;
    rtl8201->parent.pwrctl = rtl8201_pwrctl;
    rtl8201->parent.get_addr = rtl8201_get_addr;
    rtl8201->parent.set_addr = rtl8201_set_addr;
    rtl8201->parent.del = rtl8201_del;

    return &(rtl8201->parent);
err:
    return NULL;
}
