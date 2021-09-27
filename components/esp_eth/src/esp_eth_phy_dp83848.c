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

static const char *TAG = "dp83848";

/***************Vendor Specific Register***************/

/**
 * @brief PHYSTS(PHY Status Register)
 *
 */
typedef union {
    struct {
        uint32_t link_status : 1;               /* Link Status */
        uint32_t speed_status : 1;              /* Speed Status */
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
    int addr;
    uint32_t reset_timeout_ms;
    uint32_t autonego_timeout_ms;
    eth_link_t link_status;
    int reset_gpio_num;
} phy_dp83848_t;

static esp_err_t dp83848_update_link_duplex_speed(phy_dp83848_t *dp83848)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = dp83848->eth;
    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    uint32_t peer_pause_ability = false;
    anlpar_reg_t anlpar;
    physts_reg_t physts;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_ANLPAR_REG_ADDR, &(anlpar.val)), err, TAG, "read ANLPAR failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_STS_REG_ADDR, &(physts.val)), err, TAG, "read PHYSTS failed");
    eth_link_t link = physts.link_status ? ETH_LINK_UP : ETH_LINK_DOWN;
    /* check if link status changed */
    if (dp83848->link_status != link) {
        /* when link up, read negotiation result */
        if (link == ETH_LINK_UP) {
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
        dp83848->link_status = link;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dp83848_set_mediator(esp_eth_phy_t *phy, esp_eth_mediator_t *eth)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(eth, ESP_ERR_INVALID_ARG, err, TAG, "can't set mediator to null");
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    dp83848->eth = eth;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dp83848_get_link(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    /* Updata information about link, speed, duplex */
    ESP_GOTO_ON_ERROR(dp83848_update_link_duplex_speed(dp83848), err, TAG, "update link duplex speed failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dp83848_reset(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    dp83848->link_status = ETH_LINK_DOWN;
    esp_eth_mediator_t *eth = dp83848->eth;
    bmcr_reg_t bmcr = {.reset = 1};
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, dp83848->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    /* Wait for reset complete */
    uint32_t to = 0;
    for (to = 0; to < dp83848->reset_timeout_ms / 10; to++) {
        vTaskDelay(pdMS_TO_TICKS(10));
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
        if (!bmcr.reset) {
            break;
        }
    }
    ESP_GOTO_ON_FALSE(to < dp83848->reset_timeout_ms / 10, ESP_FAIL, err, TAG, "reset timeout");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dp83848_reset_hw(esp_eth_phy_t *phy)
{
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    if (dp83848->reset_gpio_num >= 0) {
        esp_rom_gpio_pad_select_gpio(dp83848->reset_gpio_num);
        gpio_set_direction(dp83848->reset_gpio_num, GPIO_MODE_OUTPUT);
        gpio_set_level(dp83848->reset_gpio_num, 0);
        esp_rom_delay_us(100); // insert min input assert time
        gpio_set_level(dp83848->reset_gpio_num, 1);
    }
    return ESP_OK;
}

/**
 * @note This function is responsible for restarting a new auto-negotiation,
 *       the result of negotiation won't be relected to uppler layers.
 *       Instead, the negotiation result is fetched by linker timer, see `dp83848_get_link()`
 */
static esp_err_t dp83848_negotiate(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    esp_eth_mediator_t *eth = dp83848->eth;
    /* in case any link status has changed, let's assume we're in link down status */
    dp83848->link_status = ETH_LINK_DOWN;
    /* Start auto negotiation */
    bmcr_reg_t bmcr = {
        .speed_select = 1,     /* 100Mbps */
        .duplex_mode = 1,      /* Full Duplex */
        .en_auto_nego = 1,     /* Auto Negotiation */
        .restart_auto_nego = 1 /* Restart Auto Negotiation */
    };
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, dp83848->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    /* Wait for auto negotiation complete */
    bmsr_reg_t bmsr;
    physts_reg_t physts;
    uint32_t to = 0;
    for (to = 0; to < dp83848->autonego_timeout_ms / 100; to++) {
        vTaskDelay(pdMS_TO_TICKS(100));
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)), err, TAG, "read BMSR failed");
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_STS_REG_ADDR, &(physts.val)), err, TAG, "read PHYSTS failed");
        if (bmsr.auto_nego_complete && physts.auto_nego_complete) {
            break;
        }
    }
    if ((to >= dp83848->autonego_timeout_ms / 100) && (dp83848->link_status == ETH_LINK_UP)) {
        ESP_LOGW(TAG, "auto negotiation timeout");
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dp83848_pwrctl(esp_eth_phy_t *phy, bool enable)
{
    esp_err_t ret = ESP_OK;
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    esp_eth_mediator_t *eth = dp83848->eth;
    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    if (!enable) {
        /* Enable IEEE Power Down Mode */
        bmcr.power_down = 1;
    } else {
        /* Disable IEEE Power Down Mode */
        bmcr.power_down = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, dp83848->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    if (!enable) {
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
        ESP_GOTO_ON_FALSE(bmcr.power_down == 1, ESP_FAIL, err, TAG, "power down failed");
    } else {
        /* wait for power up complete */
        uint32_t to = 0;
        for (to = 0; to < dp83848->reset_timeout_ms / 10; to++) {
            vTaskDelay(pdMS_TO_TICKS(10));
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
            if (bmcr.power_down == 0) {
                break;
            }
        }
        ESP_GOTO_ON_FALSE(to < dp83848->reset_timeout_ms / 10, ESP_FAIL, err, TAG, "power up timeout");
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dp83848_set_addr(esp_eth_phy_t *phy, uint32_t addr)
{
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    dp83848->addr = addr;
    return ESP_OK;
}

static esp_err_t dp83848_get_addr(esp_eth_phy_t *phy, uint32_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "addr can't be null");
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    *addr = dp83848->addr;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dp83848_del(esp_eth_phy_t *phy)
{
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    free(dp83848);
    return ESP_OK;
}

static esp_err_t dp83848_advertise_pause_ability(esp_eth_phy_t *phy, uint32_t ability)
{
    esp_err_t ret = ESP_OK;
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    esp_eth_mediator_t *eth = dp83848->eth;
    /* Set PAUSE function ability */
    anar_reg_t anar;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_ANAR_REG_ADDR, &(anar.val)), err, TAG, "read ANAR failed");
    if (ability) {
        anar.asymmetric_pause = 1;
        anar.symmetric_pause = 1;
    } else {
        anar.asymmetric_pause = 0;
        anar.symmetric_pause = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, dp83848->addr, ETH_PHY_ANAR_REG_ADDR, anar.val), err, TAG, "write ANAR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dp83848_loopback(esp_eth_phy_t *phy, bool enable)
{
    esp_err_t ret = ESP_OK;
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    esp_eth_mediator_t *eth = dp83848->eth;
    /* Set Loopback function */
    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    if (enable) {
        bmcr.en_loopback = 1;
    } else {
        bmcr.en_loopback = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, dp83848->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dp83848_init(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_dp83848_t *dp83848 = __containerof(phy, phy_dp83848_t, parent);
    esp_eth_mediator_t *eth = dp83848->eth;
    // Detect PHY address
    if (dp83848->addr == ESP_ETH_PHY_ADDR_AUTO) {
        ESP_GOTO_ON_ERROR(esp_eth_detect_phy_addr(eth, &dp83848->addr), err, TAG, "Detect PHY address failed");
    }
    /* Power on Ethernet PHY */
    ESP_GOTO_ON_ERROR(dp83848_pwrctl(phy, true), err, TAG, "power control failed");
    /* Reset Ethernet PHY */
    ESP_GOTO_ON_ERROR(dp83848_reset(phy), err, TAG, "reset failed");
    /* Check PHY ID */
    phyidr1_reg_t id1;
    phyidr2_reg_t id2;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_IDR1_REG_ADDR, &(id1.val)), err, TAG, "read ID1 failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dp83848->addr, ETH_PHY_IDR2_REG_ADDR, &(id2.val)), err, TAG, "read ID2 failed");
    ESP_GOTO_ON_FALSE(id1.oui_msb == 0x2000 && id2.oui_lsb == 0x17 && id2.vendor_model == 0x09, ESP_FAIL, err, TAG, "wrong chip ID");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dp83848_deinit(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    /* Power off Ethernet PHY */
    ESP_GOTO_ON_ERROR(dp83848_pwrctl(phy, false), err, TAG, "power control failed");
    return ESP_OK;
err:
    return ret;
}

esp_eth_phy_t *esp_eth_phy_new_dp83848(const eth_phy_config_t *config)
{
    esp_eth_phy_t *ret = NULL;
    ESP_GOTO_ON_FALSE(config, NULL, err, TAG, "can't set phy config to null");
    phy_dp83848_t *dp83848 = calloc(1, sizeof(phy_dp83848_t));
    ESP_GOTO_ON_FALSE(dp83848, NULL, err, TAG, "calloc dp83848 failed");
    dp83848->addr = config->phy_addr;
    dp83848->reset_timeout_ms = config->reset_timeout_ms;
    dp83848->link_status = ETH_LINK_DOWN;
    dp83848->reset_gpio_num = config->reset_gpio_num;
    dp83848->autonego_timeout_ms = config->autonego_timeout_ms;
    dp83848->parent.reset = dp83848_reset;
    dp83848->parent.reset_hw = dp83848_reset_hw;
    dp83848->parent.init = dp83848_init;
    dp83848->parent.deinit = dp83848_deinit;
    dp83848->parent.set_mediator = dp83848_set_mediator;
    dp83848->parent.negotiate = dp83848_negotiate;
    dp83848->parent.get_link = dp83848_get_link;
    dp83848->parent.pwrctl = dp83848_pwrctl;
    dp83848->parent.get_addr = dp83848_get_addr;
    dp83848->parent.set_addr = dp83848_set_addr;
    dp83848->parent.advertise_pause_ability = dp83848_advertise_pause_ability;
    dp83848->parent.loopback = dp83848_loopback;
    dp83848->parent.del = dp83848_del;
    return &(dp83848->parent);
err:
    return ret;
}
