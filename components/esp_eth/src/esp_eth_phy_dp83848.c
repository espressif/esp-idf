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

static const char *TAG = "dp83848";
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
 * @brief PHYSTS(PHY Status Register)
 *
 */
typedef union {
    struct {
        uint32_t link_status : 1;               /* Link Status */
        uint32_t speed_status : 1;              /* Link Status */
        uint32_t duplex_status : 1;             /* Duplex Status */
        uint32_t loopback_status : 1;           /* MII Loopback */
        uint32_t auto_nego_complete : 1;        /* Auto-Negotiation Complete */
        uint32_t jabber_detect : 1;             /* Jabber Detect */
        uint32_t remote_fault : 1;              /* Remote Fault */
        uint32_t mii_interrupt : 1;             /* MII Interrupt Pending */
        uint32_t page_received : 1;             /* Link Code Word Page Received */
        uint32_t descrambler_lock : 1;          /* Descrambler Lock */
        uint32_t signal_detect : 1;             /* Signal Detect */
        uint32_t false_carrier_sense_latch : 1; /* False Carrier Sense Latch */
        uint32_t polarity_status : 1;           /* Polarity Status */
        uint32_t receive_error_latch : 1;       /* Receive Error Latch */
        uint32_t mdix_mode : 1;                 /* MDI-X mode reported by auto-negotiation */
        uint32_t reserved : 1;                  /* Reserved */
    };
    uint32_t val;
} physts_reg_t;
#define ETH_PHY_STS_REG_ADDR (0x10)

/**
 * @brief PHYCR(PHY Control Register)
 *
 */
typedef union {
    struct {
        uint32_t phy_addr : 5;               /* PHY Address */
        uint32_t led_cfg : 2;                /* LED Configuration Modes */
        uint32_t bypass_led_stretching : 1;  /* Bypass LED Stretching */
        uint32_t bist_start : 1;             /* BIST Start */
        uint32_t bist_status : 1;            /* BIST Test Status */
        uint32_t psr_15 : 1;                 /* BIST Sequence select */
        uint32_t bist_force_error : 1;       /* BIST Force Error */
        uint32_t pause_trans_negotiate : 1;  /* Pause Transmit Negotiated Status */
        uint32_t pause_receive_negotiat : 1; /* Pause Receive Negotiated Status */
        uint32_t force_mdix : 1;             /* Force MDIX */
        uint32_t en_auto_mdix : 1;           /* Auto-MDIX Enable */
    };
    uint32_t val;
} phycr_reg_t;
#define ETH_PHY_CR_REG_ADDR (0x19)

typedef struct {
    esp_eth_phy_t parent;
    esp_eth_mediator_t *eth;
    uint32_t addr;
    uint32_t reset_timeout_ms;
    uint32_t autonego_timeout_ms;
    eth_link_t link_status;
} phy_dp83848_t;

static esp_err_t dp83848_update_link_duplex_speed(phy_dp83848_t *dp83848)
{
    esp_eth_mediator_t *eth = dp83848->eth;
    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    bmsr_reg_t bmsr;
    physts_reg_t physts;
    PHY_CHECK(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)) == ESP_OK,
              "read BMSR failed", err);
    eth_link_t link = bmsr.link_status ? ETH_LINK_UP : ETH_LINK_DOWN;
    /* check if link status changed */
    if (dp83848->link_status != link) {
        /* when link up, read negotiation result */
        if (link == ETH_LINK_UP) {
            PHY_CHECK(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_STS_REG_ADDR, &(physts.val)) == ESP_OK,
                      "read PHYSTS failed", err);
            if (physts.speed_status) {
                speed = ETH_SPEED_10M;
            } else {
                speed = ETH_SPEED_100M;
            }
            if (physts.duplex_status) {
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
        dp83848->link_status = link;
    }
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t dp83848_set_mediator(esp_eth_phy_t *phy, esp_eth_mediator_t *eth)
{
    PHY_CHECK(eth, "can't set mediator to null", err);
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    dp83848->eth = eth;
    return ESP_OK;
err:
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t dp83848_get_link(esp_eth_phy_t *phy)
{
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    /* Updata information about link, speed, duplex */
    PHY_CHECK(dp83848_update_link_duplex_speed(dp83848) == ESP_OK, "update link duplex speed failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t dp83848_reset(esp_eth_phy_t *phy)
{
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    esp_eth_mediator_t *eth = dp83848->eth;
    bmcr_reg_t bmcr = {.reset = 1};
    PHY_CHECK(eth->phy_reg_write(eth, dp83848->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val) == ESP_OK,
              "write BMCR failed", err);
    /* Wait for reset complete */
    uint32_t to = 0;
    for (to = 0; to < dp83848->reset_timeout_ms / 10; to++) {
        vTaskDelay(pdMS_TO_TICKS(10));
        PHY_CHECK(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)) == ESP_OK,
                  "read BMCR failed", err);
        if (!bmcr.reset) {
            break;
        }
    }
    PHY_CHECK(to < dp83848->reset_timeout_ms / 10, "reset timeout", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t dp83848_negotiate(esp_eth_phy_t *phy)
{
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    esp_eth_mediator_t *eth = dp83848->eth;
    /* Start auto negotiation */
    bmcr_reg_t bmcr = {
        .speed_select = 1,     /* 100Mbps */
        .duplex_mode = 1,      /* Full Duplex */
        .en_auto_nego = 1,     /* Auto Negotiation */
        .restart_auto_nego = 1 /* Restart Auto Negotiation */
    };
    PHY_CHECK(eth->phy_reg_write(eth, dp83848->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val) == ESP_OK,
              "write BMCR failed", err);
    /* Wait for auto negotiation complete */
    bmsr_reg_t bmsr;
    physts_reg_t physts;
    uint32_t to = 0;
    for (to = 0; to < dp83848->autonego_timeout_ms / 10; to++) {
        vTaskDelay(pdMS_TO_TICKS(10));
        PHY_CHECK(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)) == ESP_OK,
                  "read BMSR failed", err);
        PHY_CHECK(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_STS_REG_ADDR, &(physts.val)) == ESP_OK,
                  "read PHYSTS failed", err);
        if (bmsr.auto_nego_complete && physts.auto_nego_complete) {
            break;
        }
    }
    /* Auto negotiation failed, maybe no network cable plugged in, so output a warning */
    if (to >= dp83848->autonego_timeout_ms / 10) {
        ESP_LOGW(TAG, "auto negotiation timeout");
    }
    /* Updata information about link, speed, duplex */
    PHY_CHECK(dp83848_update_link_duplex_speed(dp83848) == ESP_OK, "update link duplex speed failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t dp83848_pwrctl(esp_eth_phy_t *phy, bool enable)
{
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    esp_eth_mediator_t *eth = dp83848->eth;
    bmcr_reg_t bmcr;
    PHY_CHECK(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)) == ESP_OK,
              "read BMCR failed", err);
    if (!enable) {
        /* Enable IEEE Power Down Mode */
        bmcr.power_down = 1;
    } else {
        /* Disable IEEE Power Down Mode */
        bmcr.power_down = 0;
    }
    PHY_CHECK(eth->phy_reg_write(eth, dp83848->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val) == ESP_OK,
              "write BMCR failed", err);
    PHY_CHECK(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)) == ESP_OK,
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

static esp_err_t dp83848_set_addr(esp_eth_phy_t *phy, uint32_t addr)
{
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    dp83848->addr = addr;
    return ESP_OK;
}

static esp_err_t dp83848_get_addr(esp_eth_phy_t *phy, uint32_t *addr)
{
    PHY_CHECK(addr, "addr can't be null", err);
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    *addr = dp83848->addr;
    return ESP_OK;
err:
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t dp83848_del(esp_eth_phy_t *phy)
{
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    free(dp83848);
    return ESP_OK;
}

static esp_err_t dp83848_init(esp_eth_phy_t *phy)
{
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    esp_eth_mediator_t *eth = dp83848->eth;
    /* Power on Ethernet PHY */
    PHY_CHECK(dp83848_pwrctl(phy, true) == ESP_OK, "power control failed", err);
    /* Reset Ethernet PHY */
    PHY_CHECK(dp83848_reset(phy) == ESP_OK, "reset failed", err);
    /* Check PHY ID */
    phyidr1_reg_t id1;
    phyidr2_reg_t id2;
    PHY_CHECK(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_IDR1_REG_ADDR, &(id1.val)) == ESP_OK,
              "read ID1 failed", err);
    PHY_CHECK(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_IDR2_REG_ADDR, &(id2.val)) == ESP_OK,
              "read ID2 failed", err);
    PHY_CHECK(id1.oui_msb == 0x2000 && id2.oui_lsb == 0x17 && id2.vendor_model == 0x09,
              "wrong chip ID", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t dp83848_deinit(esp_eth_phy_t *phy)
{
    /* Power off Ethernet PHY */
    PHY_CHECK(dp83848_pwrctl(phy, false) == ESP_OK, "power control failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

esp_eth_phy_t *esp_eth_phy_new_dp83848(const eth_phy_config_t *config)
{
    PHY_CHECK(config, "can't set phy config to null", err);
    phy_dp83848_t *dp83848 = calloc(1, sizeof(phy_dp83848_t));
    PHY_CHECK(dp83848, "calloc dp83848 failed", err);
    dp83848->addr = config->phy_addr;
    dp83848->reset_timeout_ms = config->reset_timeout_ms;
    dp83848->link_status = ETH_LINK_DOWN;
    dp83848->autonego_timeout_ms = config->autonego_timeout_ms;
    dp83848->parent.reset = dp83848_reset;
    dp83848->parent.init = dp83848_init;
    dp83848->parent.deinit = dp83848_deinit;
    dp83848->parent.set_mediator = dp83848_set_mediator;
    dp83848->parent.negotiate = dp83848_negotiate;
    dp83848->parent.get_link = dp83848_get_link;
    dp83848->parent.pwrctl = dp83848_pwrctl;
    dp83848->parent.get_addr = dp83848_get_addr;
    dp83848->parent.set_addr = dp83848_set_addr;
    dp83848->parent.del = dp83848_del;

    return &(dp83848->parent);
err:
    return NULL;
}
