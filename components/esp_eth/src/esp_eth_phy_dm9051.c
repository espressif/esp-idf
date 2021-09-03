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

static const char *TAG = "dm9051.phy";

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
    int addr;
    uint32_t reset_timeout_ms;
    uint32_t autonego_timeout_ms;
    eth_link_t link_status;
    int reset_gpio_num;
} phy_dm9051_t;

static esp_err_t dm9051_update_link_duplex_speed(phy_dm9051_t *dm9051)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = dm9051->eth;
    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    uint32_t peer_pause_ability = false;
    bmsr_reg_t bmsr;
    dscsr_reg_t dscsr;
    anlpar_reg_t anlpar;
    // BMSR is a latch low register
    // after power up, the first latched value must be 0, which means down
    // to speed up power up link speed, double read this register as a workaround
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)), err, TAG, "read BMSR failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)), err, TAG, "read BMSR failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_ANLPAR_REG_ADDR, &(anlpar.val)), err, TAG, "read ANLPAR failed");
    eth_link_t link = bmsr.link_status ? ETH_LINK_UP : ETH_LINK_DOWN;
    /* check if link status changed */
    if (dm9051->link_status != link) {
        /* when link up, read negotiation result */
        if (link == ETH_LINK_UP) {
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_DSCSR_REG_ADDR, &(dscsr.val)), err, TAG, "read DSCSR failed");
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
        dm9051->link_status = link;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dm9051_set_mediator(esp_eth_phy_t *phy, esp_eth_mediator_t *eth)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(eth, ESP_ERR_INVALID_ARG, err, TAG, "can't set mediator to null");
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    dm9051->eth = eth;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dm9051_get_link(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    /* Updata information about link, speed, duplex */
    ESP_GOTO_ON_ERROR(dm9051_update_link_duplex_speed(dm9051), err, TAG, "update link duplex speed failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dm9051_reset(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    dm9051->link_status = ETH_LINK_DOWN;
    esp_eth_mediator_t *eth = dm9051->eth;
    dscr_reg_t dscr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_DSCR_REG_ADDR, &(dscr.val)), err, TAG, "read DSCR failed");
    dscr.smrst = 1;
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, dm9051->addr, ETH_PHY_DSCR_REG_ADDR, dscr.val), err, TAG, "write DSCR failed");
    bmcr_reg_t bmcr = {.reset = 1};
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, dm9051->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    /* Wait for reset complete */
    uint32_t to = 0;
    for (to = 0; to < dm9051->reset_timeout_ms / 10; to++) {
        vTaskDelay(pdMS_TO_TICKS(10));
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_DSCR_REG_ADDR, &(dscr.val)), err, TAG, "read DSCR failed");
        if (!bmcr.reset && !dscr.smrst) {
            break;
        }
    }
    ESP_GOTO_ON_FALSE(to < dm9051->reset_timeout_ms / 10, ESP_FAIL, err, TAG, "PHY reset timeout");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dm9051_reset_hw(esp_eth_phy_t *phy)
{
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    // set reset_gpio_num minus zero can skip hardware reset phy chip
    if (dm9051->reset_gpio_num >= 0) {
        esp_rom_gpio_pad_select_gpio(dm9051->reset_gpio_num);
        gpio_set_direction(dm9051->reset_gpio_num, GPIO_MODE_OUTPUT);
        gpio_set_level(dm9051->reset_gpio_num, 0);
        esp_rom_delay_us(100); // insert min input assert time
        gpio_set_level(dm9051->reset_gpio_num, 1);
    }
    return ESP_OK;
}

/**
 * @note This function is responsible for restarting a new auto-negotiation,
 *       the result of negotiation won't be relected to uppler layers.
 *       Instead, the negotiation result is fetched by linker timer, see `dm9051_get_link()`
 */
static esp_err_t dm9051_negotiate(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    esp_eth_mediator_t *eth = dm9051->eth;
    /* in case any link status has changed, let's assume we're in link down status */
    dm9051->link_status = ETH_LINK_DOWN;
    /* Start auto negotiation */
    bmcr_reg_t bmcr = {
        .speed_select = 1,     /* 100Mbps */
        .duplex_mode = 1,      /* Full Duplex */
        .en_auto_nego = 1,     /* Auto Negotiation */
        .restart_auto_nego = 1 /* Restart Auto Negotiation */
    };
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, dm9051->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    /* Wait for auto negotiation complete */
    bmsr_reg_t bmsr;
    dscsr_reg_t dscsr;
    uint32_t to = 0;
    for (to = 0; to < dm9051->autonego_timeout_ms / 100; to++) {
        vTaskDelay(pdMS_TO_TICKS(100));
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)), err, TAG, "read BMSR failed");
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_DSCSR_REG_ADDR, &(dscsr.val)), err, TAG, "read DSCSR failed");
        if (bmsr.auto_nego_complete && dscsr.anmb & 0x08) {
            break;
        }
    }
    if ((to >= dm9051->autonego_timeout_ms / 100) && (dm9051->link_status == ETH_LINK_UP)) {
        ESP_LOGW(TAG, "Ethernet PHY auto negotiation timeout");
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dm9051_pwrctl(esp_eth_phy_t *phy, bool enable)
{
    esp_err_t ret = ESP_OK;
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    esp_eth_mediator_t *eth = dm9051->eth;
    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    if (!enable) {
        /* Enable IEEE Power Down Mode */
        bmcr.power_down = 1;
    } else {
        /* Disable IEEE Power Down Mode */
        bmcr.power_down = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, dm9051->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    if (!enable) {
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
        ESP_GOTO_ON_FALSE(bmcr.power_down == 1, ESP_FAIL, err, TAG, "power down failed");
    } else {
        /* wait for power up complete */
        uint32_t to = 0;
        for (to = 0; to < dm9051->reset_timeout_ms / 10; to++) {
            vTaskDelay(pdMS_TO_TICKS(10));
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
            if (bmcr.power_down == 0) {
                break;
            }
        }
        ESP_GOTO_ON_FALSE(to < dm9051->reset_timeout_ms / 10, ESP_FAIL, err, TAG, "power up timeout");
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dm9051_set_addr(esp_eth_phy_t *phy, uint32_t addr)
{
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    dm9051->addr = addr;
    return ESP_OK;
}

static esp_err_t dm9051_get_addr(esp_eth_phy_t *phy, uint32_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "addr can't be null");
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    *addr = dm9051->addr;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dm9051_del(esp_eth_phy_t *phy)
{
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    free(dm9051);
    return ESP_OK;
}

static esp_err_t dm9051_advertise_pause_ability(esp_eth_phy_t *phy, uint32_t ability)
{
    esp_err_t ret = ESP_OK;
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    esp_eth_mediator_t *eth = dm9051->eth;
    /* Set PAUSE function ability */
    anar_reg_t anar;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_ANAR_REG_ADDR, &(anar.val)), err, TAG, "read ANAR failed");
    if (ability) {
        anar.asymmetric_pause = 1;
        anar.symmetric_pause = 1;
    } else {
        anar.asymmetric_pause = 0;
        anar.symmetric_pause = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, dm9051->addr, ETH_PHY_ANAR_REG_ADDR, anar.val), err, TAG, "write ANAR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dm9051_loopback(esp_eth_phy_t *phy, bool enable)
{
    esp_err_t ret = ESP_OK;
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    esp_eth_mediator_t *eth = dm9051->eth;
    /* Set Loopback function */
    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    if (enable) {
        bmcr.en_loopback = 1;
    } else {
        bmcr.en_loopback = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, dm9051->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dm9051_init(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_dm9051_t *dm9051 = __containerof(phy, phy_dm9051_t, parent);
    esp_eth_mediator_t *eth = dm9051->eth;
    // Detect PHY address
    if (dm9051->addr == ESP_ETH_PHY_ADDR_AUTO) {
        ESP_GOTO_ON_ERROR(esp_eth_detect_phy_addr(eth, &dm9051->addr), err, TAG, "Detect PHY address failed");
    }
    /* Power on Ethernet PHY */
    ESP_GOTO_ON_ERROR(dm9051_pwrctl(phy, true), err, TAG, "power control failed");
    /* Reset Ethernet PHY */
    ESP_GOTO_ON_ERROR(dm9051_reset(phy), err, TAG, "reset failed");
    /* Check PHY ID */
    phyidr1_reg_t id1;
    phyidr2_reg_t id2;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_IDR1_REG_ADDR, &(id1.val)), err, TAG, "read ID1 failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, dm9051->addr, ETH_PHY_IDR2_REG_ADDR, &(id2.val)), err, TAG, "read ID2 failed");
    ESP_GOTO_ON_FALSE(id1.oui_msb == 0x0181 && id2.oui_lsb == 0x2E && id2.vendor_model == 0x0A, ESP_FAIL, err, TAG, "wrong chip ID");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dm9051_deinit(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    /* Power off Ethernet PHY */
    ESP_GOTO_ON_ERROR(dm9051_pwrctl(phy, false), err, TAG, "power control failed");
    return ESP_OK;
err:
    return ret;
}

esp_eth_phy_t *esp_eth_phy_new_dm9051(const eth_phy_config_t *config)
{
    esp_eth_phy_t *ret = NULL;
    ESP_GOTO_ON_FALSE(config, NULL, err, TAG, "can't set phy config to null");
    phy_dm9051_t *dm9051 = calloc(1, sizeof(phy_dm9051_t));
    ESP_GOTO_ON_FALSE(dm9051, NULL, err, TAG, "calloc dm9051 failed");
    dm9051->addr = config->phy_addr;
    dm9051->reset_timeout_ms = config->reset_timeout_ms;
    dm9051->reset_gpio_num = config->reset_gpio_num;
    dm9051->link_status = ETH_LINK_DOWN;
    dm9051->autonego_timeout_ms = config->autonego_timeout_ms;
    dm9051->parent.reset = dm9051_reset;
    dm9051->parent.reset_hw = dm9051_reset_hw;
    dm9051->parent.init = dm9051_init;
    dm9051->parent.deinit = dm9051_deinit;
    dm9051->parent.set_mediator = dm9051_set_mediator;
    dm9051->parent.negotiate = dm9051_negotiate;
    dm9051->parent.get_link = dm9051_get_link;
    dm9051->parent.pwrctl = dm9051_pwrctl;
    dm9051->parent.get_addr = dm9051_get_addr;
    dm9051->parent.set_addr = dm9051_set_addr;
    dm9051->parent.advertise_pause_ability = dm9051_advertise_pause_ability;
    dm9051->parent.loopback = dm9051_loopback;
    dm9051->parent.del = dm9051_del;
    return &(dm9051->parent);
err:
    return ret;
}
