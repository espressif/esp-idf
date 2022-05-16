// Copyright 2019-2021 Espressif Systems (Shanghai) PTE LTD
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

static const char *TAG = "ksz80xx";

#define KSZ8041_MODEL_ID (0x11)
#define KSZ8081_MODEL_ID (0x16)

/***************Vendor Specific Register***************/
/**
 * @brief PC2R(PHY Control 2 Register) for KSZ8041
 *
 */
typedef union {
    struct {
        uint32_t dis_data_scr: 1;    /* Disable Scrambler */
        uint32_t en_sqe_test : 1;    /* Enable SQE Test */
        uint32_t op_mode : 3;        /* Operation Mode */
        uint32_t phy_iso : 1;        /* PHY Isolate */
        uint32_t en_flow_ctrl : 1;   /* Enable Flow Control */
        uint32_t auto_nego_comp : 1; /* Auto Negotiation Complete */
        uint32_t en_jabber : 1;      /* Enable Jabber Counter */
        uint32_t irq_level : 1;      /* Interrupt Pin Active Level */
        uint32_t power_saving : 1;   /* Enable Powering Saving */
        uint32_t force_link : 1;     /* Force Link Pass */
        uint32_t energy_det : 1;     /* Presence of Signal on RX+/- Wire Pair */
        uint32_t pairswap_dis : 1;   /* Disable Auto MDI/MDI-X */
        uint32_t mdix_select : 1;    /* MDI/MDI-X Select */
        uint32_t hp_mdix : 1;        /* HP Auto MDI/MDI-X Mode */
    };
    uint32_t val;
} ksz8041_pc2r_reg_t;
#define KSZ8041_PC2R_ERG_ADDR (0x1F)

/**
 * @brief PC1R(PHY Control 1 Register) for KSZ8081
 *
 */
typedef union {
    struct {
        uint32_t op_mode : 3;         /* Operation Mode Indication */
        uint32_t phy_iso : 1;         /* PHY in Isolate Mode */
        uint32_t energy_det: 1;       /* Signal presence on RX pair */
        uint32_t mdix_state : 1;      /* MDI/MDI-X state */
        uint32_t reserved_6 : 1;      /* Reserved */
        uint32_t polarity_status : 1; /* Polarity status */
        uint32_t link_status : 1;     /* Link status */
        uint32_t en_flow_ctrl : 1;    /* Flow control */
        uint32_t reserved_15_10 : 6;  /* Reserved */
    };
    uint32_t val;
} ksz8081_pc1r_reg_t;
#define KSZ8081_PC1R_REG_ADDR (0x1E)

typedef struct {
    esp_eth_phy_t parent;
    esp_eth_mediator_t *eth;
    int addr;
    uint32_t reset_timeout_ms;
    uint32_t autonego_timeout_ms;
    eth_link_t link_status;
    int reset_gpio_num;
    int vendor_model;
} phy_ksz80xx_t;

static esp_err_t ksz80xx_update_link_duplex_speed(phy_ksz80xx_t *ksz80xx)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = ksz80xx->eth;
    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    uint32_t peer_pause_ability = false;
    anlpar_reg_t anlpar;
    bmsr_reg_t bmsr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz80xx->addr, ETH_PHY_ANLPAR_REG_ADDR, &(anlpar.val)), err, TAG, "read ANLPAR failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz80xx->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)), err, TAG, "read BMSR failed");
    eth_link_t link = bmsr.link_status ? ETH_LINK_UP : ETH_LINK_DOWN;
    /* check if link status changed */
    if (ksz80xx->link_status != link) {
        /* when link up, read negotiation result */
        if (link == ETH_LINK_UP) {
            int op_mode = 0;
            if (ksz80xx->vendor_model == KSZ8041_MODEL_ID) {
                ksz8041_pc2r_reg_t pc2r;
                ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz80xx->addr, KSZ8041_PC2R_ERG_ADDR, &(pc2r.val)), err, TAG, "read PC2R failed");
                op_mode = pc2r.op_mode;
            } else if (ksz80xx->vendor_model == KSZ8081_MODEL_ID) {
                ksz8081_pc1r_reg_t pc1r;
                ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz80xx->addr, KSZ8081_PC1R_REG_ADDR, &(pc1r.val)), err, TAG, "read PC1R failed");
                op_mode = pc1r.op_mode;
            }
            switch (op_mode) {
            case 1: //10Base-T half-duplex
                speed = ETH_SPEED_10M;
                duplex = ETH_DUPLEX_HALF;
                break;
            case 2: //100Base-TX half-duplex
                speed = ETH_SPEED_100M;
                duplex = ETH_DUPLEX_HALF;
                break;
            case 5: //10Base-T full-duplex
                speed = ETH_SPEED_10M;
                duplex = ETH_DUPLEX_FULL;
                break;
            case 6: //100Base-TX full-duplex
                speed = ETH_SPEED_100M;
                duplex = ETH_DUPLEX_FULL;
                break;
            default:
                break;
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
        ksz80xx->link_status = link;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ksz80xx_set_mediator(esp_eth_phy_t *phy, esp_eth_mediator_t *eth)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(eth, ESP_ERR_INVALID_ARG, err, TAG, "can't set mediator to null");
    phy_ksz80xx_t *ksz80xx = __containerof(phy, phy_ksz80xx_t, parent);
    ksz80xx->eth = eth;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ksz80xx_get_link(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_ksz80xx_t *ksz80xx = __containerof(phy, phy_ksz80xx_t, parent);
    /* Update information about link, speed, duplex */
    ESP_GOTO_ON_ERROR(ksz80xx_update_link_duplex_speed(ksz80xx), err, TAG, "update link duplex speed failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ksz80xx_reset(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_ksz80xx_t *ksz80xx = __containerof(phy, phy_ksz80xx_t, parent);
    ksz80xx->link_status = ETH_LINK_DOWN;
    esp_eth_mediator_t *eth = ksz80xx->eth;
    bmcr_reg_t bmcr = {.reset = 1};
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz80xx->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    /* wait for reset complete */
    uint32_t to = 0;
    for (to = 0; to < ksz80xx->reset_timeout_ms / 10; to++) {
        vTaskDelay(pdMS_TO_TICKS(10));
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz80xx->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
        if (!bmcr.reset) {
            break;
        }
    }
    ESP_GOTO_ON_FALSE(to < ksz80xx->reset_timeout_ms / 10, ESP_FAIL, err, TAG, "reset timeout");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ksz80xx_reset_hw(esp_eth_phy_t *phy)
{
    phy_ksz80xx_t *ksz80xx = __containerof(phy, phy_ksz80xx_t, parent);
    if (ksz80xx->reset_gpio_num >= 0) {
        esp_rom_gpio_pad_select_gpio(ksz80xx->reset_gpio_num);
        gpio_set_direction(ksz80xx->reset_gpio_num, GPIO_MODE_OUTPUT);
        gpio_set_level(ksz80xx->reset_gpio_num, 0);
        esp_rom_delay_us(100); // insert min input assert time
        gpio_set_level(ksz80xx->reset_gpio_num, 1);
    }
    return ESP_OK;
}

/**
 * @note This function is responsible for restarting a new auto-negotiation,
 *       the result of negotiation won't be relected to uppler layers.
 *       Instead, the negotiation result is fetched by linker timer, see `ksz80xx_get_link()`
 */
static esp_err_t ksz80xx_negotiate(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_ksz80xx_t *ksz80xx = __containerof(phy, phy_ksz80xx_t, parent);
    esp_eth_mediator_t *eth = ksz80xx->eth;
    /* in case any link status has changed, let's assume we're in link down status */
    ksz80xx->link_status = ETH_LINK_DOWN;
    /* Restart auto negotiation */
    bmcr_reg_t bmcr = {
        .speed_select = 1,     /* 100Mbps */
        .duplex_mode = 1,      /* Full Duplex */
        .en_auto_nego = 1,     /* Auto Negotiation */
        .restart_auto_nego = 1 /* Restart Auto Negotiation */
    };
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz80xx->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    /* Wait for auto negotiation complete */
    bmsr_reg_t bmsr;
    uint32_t to = 0;
    for (to = 0; to < ksz80xx->autonego_timeout_ms / 100; to++) {
        vTaskDelay(pdMS_TO_TICKS(100));
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz80xx->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)), err, TAG, "read BMSR failed");
        if (bmsr.auto_nego_complete) {
            break;
        }
    }
    if ((to >= ksz80xx->autonego_timeout_ms / 100) && (ksz80xx->link_status == ETH_LINK_UP)) {
        ESP_LOGW(TAG, "auto negotiation timeout");
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ksz80xx_pwrctl(esp_eth_phy_t *phy, bool enable)
{
    esp_err_t ret = ESP_OK;
    phy_ksz80xx_t *ksz80xx = __containerof(phy, phy_ksz80xx_t, parent);
    esp_eth_mediator_t *eth = ksz80xx->eth;
    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz80xx->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    if (!enable) {
        /* General Power Down Mode */
        bmcr.power_down = 1;
    } else {
        /* Normal operation Mode */
        bmcr.power_down = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz80xx->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    if (!enable) {
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz80xx->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
        ESP_GOTO_ON_FALSE(bmcr.power_down == 1, ESP_FAIL, err, TAG, "power down failed");
    } else {
        /* wait for power up complete */
        uint32_t to = 0;
        for (to = 0; to < ksz80xx->reset_timeout_ms / 10; to++) {
            vTaskDelay(pdMS_TO_TICKS(10));
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz80xx->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
            if (bmcr.power_down == 0) {
                break;
            }
        }
        ESP_GOTO_ON_FALSE(to < ksz80xx->reset_timeout_ms / 10, ESP_FAIL, err, TAG, "power up timeout");
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ksz80xx_set_addr(esp_eth_phy_t *phy, uint32_t addr)
{
    phy_ksz80xx_t *ksz80xx = __containerof(phy, phy_ksz80xx_t, parent);
    ksz80xx->addr = addr;
    return ESP_OK;
}

static esp_err_t ksz80xx_get_addr(esp_eth_phy_t *phy, uint32_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "addr can't be null");
    phy_ksz80xx_t *ksz80xx = __containerof(phy, phy_ksz80xx_t, parent);
    *addr = ksz80xx->addr;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ksz80xx_del(esp_eth_phy_t *phy)
{
    phy_ksz80xx_t *ksz80xx = __containerof(phy, phy_ksz80xx_t, parent);
    free(ksz80xx);
    return ESP_OK;
}

static esp_err_t ksz80xx_advertise_pause_ability(esp_eth_phy_t *phy, uint32_t ability)
{
    esp_err_t ret = ESP_OK;
    phy_ksz80xx_t *ksz80xx = __containerof(phy, phy_ksz80xx_t, parent);
    esp_eth_mediator_t *eth = ksz80xx->eth;
    /* Set PAUSE function ability */
    anar_reg_t anar;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz80xx->addr, ETH_PHY_ANAR_REG_ADDR, &(anar.val)), err, TAG, "read ANAR failed");
    if (ability) {
        anar.asymmetric_pause = 1;
        anar.symmetric_pause = 1;
    } else {
        anar.asymmetric_pause = 0;
        anar.symmetric_pause = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz80xx->addr, ETH_PHY_ANAR_REG_ADDR, anar.val), err, TAG, "write ANAR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ksz80xx_loopback(esp_eth_phy_t *phy, bool enable)
{
    esp_err_t ret = ESP_OK;
    phy_ksz80xx_t *ksz80xx = __containerof(phy, phy_ksz80xx_t, parent);
    esp_eth_mediator_t *eth = ksz80xx->eth;
    /* Set Loopback function */
    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz80xx->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    if (enable) {
        bmcr.en_loopback = 1;
    } else {
        bmcr.en_loopback = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz80xx->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ksz80xx_init(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_ksz80xx_t *ksz80xx = __containerof(phy, phy_ksz80xx_t, parent);
    esp_eth_mediator_t *eth = ksz80xx->eth;
    /* Power on Ethernet PHY */
    ESP_GOTO_ON_ERROR(ksz80xx_pwrctl(phy, true), err, TAG, "power control failed");
    /* Reset Ethernet PHY */
    ESP_GOTO_ON_ERROR(ksz80xx_reset(phy), err, TAG, "reset failed");
    /* Check PHY ID */
    phyidr1_reg_t id1;
    phyidr2_reg_t id2;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz80xx->addr, ETH_PHY_IDR1_REG_ADDR, &(id1.val)), err, TAG, "read ID1 failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz80xx->addr, ETH_PHY_IDR2_REG_ADDR, &(id2.val)), err, TAG, "read ID2 failed");
    ESP_GOTO_ON_FALSE(id1.oui_msb == 0x22 && id2.oui_lsb == 0x5 && id2.vendor_model == ksz80xx->vendor_model, ESP_FAIL, err, TAG, "wrong chip ID");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ksz80xx_deinit(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    /* Power off Ethernet PHY */
    ESP_GOTO_ON_ERROR(ksz80xx_pwrctl(phy, false), err, TAG, "power control failed");
    return ESP_OK;
err:
    return ret;
}

esp_eth_phy_t *esp_eth_phy_new_ksz8041(const eth_phy_config_t *config)
{
    esp_eth_phy_t *ret = NULL;
    ESP_GOTO_ON_FALSE(config, NULL, err, TAG, "can't set phy config to null");
    phy_ksz80xx_t *ksz8041 = calloc(1, sizeof(phy_ksz80xx_t));
    ESP_GOTO_ON_FALSE(ksz8041, NULL, err, TAG, "calloc ksz80xx failed");
    ksz8041->vendor_model = KSZ8041_MODEL_ID;
    ksz8041->addr = config->phy_addr;
    ksz8041->reset_gpio_num = config->reset_gpio_num;
    ksz8041->reset_timeout_ms = config->reset_timeout_ms;
    ksz8041->link_status = ETH_LINK_DOWN;
    ksz8041->autonego_timeout_ms = config->autonego_timeout_ms;
    ksz8041->parent.reset = ksz80xx_reset;
    ksz8041->parent.reset_hw = ksz80xx_reset_hw;
    ksz8041->parent.init = ksz80xx_init;
    ksz8041->parent.deinit = ksz80xx_deinit;
    ksz8041->parent.set_mediator = ksz80xx_set_mediator;
    ksz8041->parent.negotiate = ksz80xx_negotiate;
    ksz8041->parent.get_link = ksz80xx_get_link;
    ksz8041->parent.pwrctl = ksz80xx_pwrctl;
    ksz8041->parent.get_addr = ksz80xx_get_addr;
    ksz8041->parent.set_addr = ksz80xx_set_addr;
    ksz8041->parent.advertise_pause_ability = ksz80xx_advertise_pause_ability;
    ksz8041->parent.loopback = ksz80xx_loopback;
    ksz8041->parent.del = ksz80xx_del;
    return &(ksz8041->parent);
err:
    return ret;
}

esp_eth_phy_t *esp_eth_phy_new_ksz8081(const eth_phy_config_t *config)
{
    esp_eth_phy_t *ret = NULL;
    ESP_GOTO_ON_FALSE(config, NULL, err, TAG, "can't set phy config to null");
    phy_ksz80xx_t *ksz8081 = calloc(1, sizeof(phy_ksz80xx_t));
    ESP_GOTO_ON_FALSE(ksz8081, NULL, err, TAG, "calloc ksz8081 failed");
    ksz8081->vendor_model = KSZ8081_MODEL_ID;
    ksz8081->addr = config->phy_addr;
    ksz8081->reset_gpio_num = config->reset_gpio_num;
    ksz8081->reset_timeout_ms = config->reset_timeout_ms;
    ksz8081->link_status = ETH_LINK_DOWN;
    ksz8081->autonego_timeout_ms = config->autonego_timeout_ms;
    ksz8081->parent.reset = ksz80xx_reset;
    ksz8081->parent.reset_hw = ksz80xx_reset_hw;
    ksz8081->parent.init = ksz80xx_init;
    ksz8081->parent.deinit = ksz80xx_deinit;
    ksz8081->parent.set_mediator = ksz80xx_set_mediator;
    ksz8081->parent.negotiate = ksz80xx_negotiate;
    ksz8081->parent.get_link = ksz80xx_get_link;
    ksz8081->parent.pwrctl = ksz80xx_pwrctl;
    ksz8081->parent.get_addr = ksz80xx_get_addr;
    ksz8081->parent.set_addr = ksz80xx_set_addr;
    ksz8081->parent.advertise_pause_ability = ksz80xx_advertise_pause_ability;
    ksz8081->parent.loopback = ksz80xx_loopback;
    ksz8081->parent.del = ksz80xx_del;
    return &(ksz8081->parent);
err:
    return ret;
}
