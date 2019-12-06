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

static const char *TAG = "dm9051";
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
 * @brief DSCR(DAVICOM Specified Configuration Register)
 *
 */
typedef union {
    struct {
        uint32_t reserved1 : 1; /* Reserved */
        uint32_t sleep : 1;     /* Set 1 to enable PHY into sleep mode */
        uint32_t mfpsc : 1;     /* MII frame preamble suppression control bit */
        uint32_t smrst : 1;     /* Set 1 to reset all state machines of PHY */
        uint32_t rpdctr_en : 1; /* Set 1 to enable automatic reduced power down */
        uint32_t reserved2 : 2; /* Reserved */
        uint32_t flink100 : 1;  /* Force Good Link in 100Mbps */
        uint32_t reserved3 : 2; /* Reserved */
        uint32_t tx_fx : 1;     /* 100BASE-TX or FX Mode Control */
        uint32_t reserved4 : 1; /* Reserved */
        uint32_t bp_adpok : 1;  /* BYPASS ADPOK */
        uint32_t bp_align : 1;  /* Bypass Symbol Alignment Function */
        uint32_t bp_scr : 1;    /* Bypass Scrambler/Descrambler Function */
        uint32_t bp_4b5b : 1;   /* Bypass 4B5B Encoding and 5B4B Decoding */
    };
    uint32_t val;
} dscr_reg_t;
#define ETH_PHY_DSCR_REG_ADDR (0x10)

/**
 * @brief DSCSR(DAVICOM Specified Configuration and Status Register)
 *
 */
typedef union {
    struct {
        uint32_t anmb : 4;     /* Auto-Negotiation Monitor Bits */
        uint32_t phy_addr : 5; /* PHY Address */
        uint32_t reserved : 3; /* Reserved */
        uint32_t hdx10 : 1;    /* 10M Half-Duplex Operation Mode */
        uint32_t fdx10 : 1;    /* 10M Full-Duplex Operation Mode */
        uint32_t hdx100 : 1;   /* 100M Half-Duplex Operation Mode */
        uint32_t fdx100 : 1;   /* 100M Full-Duplex Operation Mode */
    };
    uint32_t val;
} dscsr_reg_t;
#define ETH_PHY_DSCSR_REG_ADDR (0x11)

typedef struct {
    esp_eth_phy_t parent;
    esp_eth_mediator_t *eth;
    uint32_t addr;
    uint32_t reset_timeout_ms;
    uint32_t autonego_timeout_ms;
    eth_link_t link_status;
} phy_dm9051_t;

static esp_err_t dm9051_update_link_duplex_speed(phy_dm9051_t *dm9051)
{
    esp_eth_mediator_t *eth = dm9051->eth;
    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    bmsr_reg_t bmsr;
    dscsr_reg_t dscsr;
    PHY_CHECK(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)) == ESP_OK,
              "read BMSR failed", err);
    eth_link_t link = bmsr.link_status ? ETH_LINK_UP : ETH_LINK_DOWN;
    /* check if link status changed */
    if (dm9051->link_status != link) {
        /* when link up, read negotiation result */
        if (link == ETH_LINK_UP) {
            PHY_CHECK(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_DSCSR_REG_ADDR, &(dscsr.val)) == ESP_OK,
                      "read DSCSR failed", err);
            if (dscsr.fdx100 || dscsr.hdx100) {
                speed = ETH_SPEED_100M;
            } else {
                speed = ETH_SPEED_10M;
            }
            if (dscsr.fdx100 || dscsr.fdx10) {
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
        dm9051->link_status = link;
    }
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t dm9051_set_mediator(esp_eth_phy_t *phy, esp_eth_mediator_t *eth)
{
    PHY_CHECK(eth, "can't set mediator to null", err);
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    dm9051->eth = eth;
    return ESP_OK;
err:
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t dm9051_get_link(esp_eth_phy_t *phy)
{
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    /* Updata information about link, speed, duplex */
    PHY_CHECK(dm9051_update_link_duplex_speed(dm9051) == ESP_OK, "update link duplex speed failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t dm9051_reset(esp_eth_phy_t *phy)
{
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    esp_eth_mediator_t *eth = dm9051->eth;
    dscr_reg_t dscr;
    PHY_CHECK(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_DSCR_REG_ADDR, &(dscr.val)) == ESP_OK,
              "read DSCR failed", err);
    dscr.smrst = 1;
    PHY_CHECK(eth->phy_reg_write(eth, dm9051->addr, ETH_PHY_DSCR_REG_ADDR, dscr.val) == ESP_OK,
              "write DSCR failed", err);
    bmcr_reg_t bmcr = {.reset = 1};
    PHY_CHECK(eth->phy_reg_write(eth, dm9051->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val) == ESP_OK,
              "write BMCR failed", err);
    /* Wait for reset complete */
    uint32_t to = 0;
    for (to = 0; to < dm9051->reset_timeout_ms / 10; to++) {
        vTaskDelay(pdMS_TO_TICKS(10));
        PHY_CHECK(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)) == ESP_OK,
                  "read BMCR failed", err);
        PHY_CHECK(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_DSCR_REG_ADDR, &(dscr.val)) == ESP_OK,
                  "read DSCR failed", err);
        if (!bmcr.reset && !dscr.smrst) {
            break;
        }
    }
    PHY_CHECK(to < dm9051->reset_timeout_ms / 10, "PHY reset timeout", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t dm9051_negotiate(esp_eth_phy_t *phy)
{
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    esp_eth_mediator_t *eth = dm9051->eth;
    /* Start auto negotiation */
    bmcr_reg_t bmcr = {
        .speed_select = 1,     /* 100Mbps */
        .duplex_mode = 1,      /* Full Duplex */
        .en_auto_nego = 1,     /* Auto Negotiation */
        .restart_auto_nego = 1 /* Restart Auto Negotiation */
    };
    PHY_CHECK(eth->phy_reg_write(eth, dm9051->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val) == ESP_OK,
              "write BMCR failed", err);
    /* Wait for auto negotiation complete */
    bmsr_reg_t bmsr;
    dscsr_reg_t dscsr;
    uint32_t to = 0;
    for (to = 0; to < dm9051->autonego_timeout_ms / 10; to++) {
        vTaskDelay(pdMS_TO_TICKS(10));
        PHY_CHECK(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)) == ESP_OK,
                  "read BMSR failed", err);
        PHY_CHECK(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_DSCSR_REG_ADDR, &(dscsr.val)) == ESP_OK,
                  "read DSCSR failed", err);
        if (bmsr.auto_nego_complete && dscsr.anmb & 0x08) {
            break;
        }
    }
    if (to >= dm9051->autonego_timeout_ms / 10) {
        ESP_LOGW(TAG, "Ethernet PHY auto negotiation timeout");
    }
    /* Updata information about link, speed, duplex */
    PHY_CHECK(dm9051_update_link_duplex_speed(dm9051) == ESP_OK, "update link duplex speed failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t dm9051_pwrctl(esp_eth_phy_t *phy, bool enable)
{
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    esp_eth_mediator_t *eth = dm9051->eth;
    bmcr_reg_t bmcr;
    PHY_CHECK(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)) == ESP_OK,
              "read BMCR failed", err);
    if (!enable) {
        /* Enable IEEE Power Down Mode */
        bmcr.power_down = 1;
    } else {
        /* Disable IEEE Power Down Mode */
        bmcr.power_down = 0;
    }
    PHY_CHECK(eth->phy_reg_write(eth, dm9051->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val) == ESP_OK,
              "write BMCR failed", err);
    PHY_CHECK(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)) == ESP_OK,
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

static esp_err_t dm9051_set_addr(esp_eth_phy_t *phy, uint32_t addr)
{
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    dm9051->addr = addr;
    return ESP_OK;
}

static esp_err_t dm9051_get_addr(esp_eth_phy_t *phy, uint32_t *addr)
{
    PHY_CHECK(addr, "addr can't be null", err);
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    *addr = dm9051->addr;
    return ESP_OK;
err:
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t dm9051_del(esp_eth_phy_t *phy)
{
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    free(dm9051);
    return ESP_OK;
}

static esp_err_t dm9051_init(esp_eth_phy_t *phy)
{
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    esp_eth_mediator_t *eth = dm9051->eth;
    /* Power on Ethernet PHY */
    PHY_CHECK(dm9051_pwrctl(phy, true) == ESP_OK, "power control failed", err);
    /* Reset Ethernet PHY */
    PHY_CHECK(dm9051_reset(phy) == ESP_OK, "reset failed", err);
    /* Check PHY ID */
    phyidr1_reg_t id1;
    phyidr2_reg_t id2;
    PHY_CHECK(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_IDR1_REG_ADDR, &(id1.val)) == ESP_OK,
              "read ID1 failed", err);
    PHY_CHECK(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_IDR2_REG_ADDR, &(id2.val)) == ESP_OK,
              "read ID2 failed", err);
    PHY_CHECK(id1.oui_msb == 0x0181 && id2.oui_lsb == 0x2E && id2.vendor_model == 0x0A,
              "wrong chip ID", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t dm9051_deinit(esp_eth_phy_t *phy)
{
    /* Power off Ethernet PHY */
    PHY_CHECK(dm9051_pwrctl(phy, false) == ESP_OK, "power control failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

esp_eth_phy_t *esp_eth_phy_new_dm9051(const eth_phy_config_t *config)
{
    PHY_CHECK(config, "can't set phy config to null", err);
    PHY_CHECK(config->phy_addr == 1, "dm9051's phy address can only set to 1", err);
    phy_dm9051_t *dm9051 = calloc(1, sizeof(phy_dm9051_t));
    PHY_CHECK(dm9051, "calloc dm9051 failed", err);
    dm9051->addr = config->phy_addr;
    dm9051->reset_timeout_ms = config->reset_timeout_ms;
    dm9051->link_status = ETH_LINK_DOWN;
    dm9051->autonego_timeout_ms = config->autonego_timeout_ms;
    dm9051->parent.reset = dm9051_reset;
    dm9051->parent.init = dm9051_init;
    dm9051->parent.deinit = dm9051_deinit;
    dm9051->parent.set_mediator = dm9051_set_mediator;
    dm9051->parent.negotiate = dm9051_negotiate;
    dm9051->parent.get_link = dm9051_get_link;
    dm9051->parent.pwrctl = dm9051_pwrctl;
    dm9051->parent.get_addr = dm9051_get_addr;
    dm9051->parent.set_addr = dm9051_set_addr;
    dm9051->parent.del = dm9051_del;
    return &(dm9051->parent);
err:
    return NULL;
}
