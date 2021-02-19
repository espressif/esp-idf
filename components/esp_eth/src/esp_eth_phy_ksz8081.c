// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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
#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"

static const char *TAG = "ksz8081";
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
 * @brief PC1R(PHY Control 1 Register)
 *
 */
typedef union {
    struct {
   
            uint32_t op_mode_ind : 3;               /* Operation Mode Indication */
            uint32_t phy_iso : 1;                   /* PHY in Isolate Mode */
            uint32_t energy_detect: 1;              /* Signal presence on RX pair */
            uint32_t mdi_mdix_state : 1;            /* MDI/MDI-X state */
            uint32_t reserved_6 : 1;
            uint32_t polarity_status : 1;           /* Polarity status */
            uint32_t link_status : 1;               /* Link status */
            uint32_t enable_pause_flow_control : 1; /* Flow control */ 
            uint32_t reserved_15_10 : 6;
    };
    uint32_t val;
} pc1r_reg_t;
#define ETH_PHY_PC1R_REG_ADDR (0x1E)

typedef struct {
    esp_eth_phy_t parent;
    esp_eth_mediator_t *eth;
    int addr;
    uint32_t reset_timeout_ms;
    uint32_t autonego_timeout_ms;
    eth_link_t link_status;
    int reset_gpio_num;
} phy_ksz8081_t;

static esp_err_t ksz8081_update_link_duplex_speed(phy_ksz8081_t *ksz8081)
{
    esp_eth_mediator_t *eth = ksz8081->eth;
    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    uint32_t peer_pause_ability = false;
    anlpar_reg_t anlpar;
    bmsr_reg_t bmsr;
    pc1r_reg_t pc1r;
    PHY_CHECK(eth->phy_reg_read(eth, ksz8081->addr, ETH_PHY_ANLPAR_REG_ADDR, &(anlpar.val)) == ESP_OK,
              "read ANLPAR failed", err);
    PHY_CHECK(eth->phy_reg_read(eth, ksz8081->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)) == ESP_OK,
              "read BMSR failed", err);
    eth_link_t link = bmsr.link_status ? ETH_LINK_UP : ETH_LINK_DOWN;
    /* check if link status changed */
    if (ksz8081->link_status != link) {
        /* when link up, read negotiation result */
        if (link == ETH_LINK_UP) {
            PHY_CHECK(eth->phy_reg_read(eth, ksz8081->addr, ETH_PHY_PC1R_REG_ADDR, &(pc1r.val)) == ESP_OK,
                      "read PC1R failed", err);
            switch (pc1r.op_mode_ind) {
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
            PHY_CHECK(eth->on_state_changed(eth, ETH_STATE_SPEED, (void *)speed) == ESP_OK,
                      "change speed failed", err);
            PHY_CHECK(eth->on_state_changed(eth, ETH_STATE_DUPLEX, (void *)duplex) == ESP_OK,
                      "change duplex failed", err);
            /* if we're in duplex mode, and peer has the flow control ability */
            if (duplex == ETH_DUPLEX_FULL && anlpar.symmetric_pause) {
                peer_pause_ability = 1;
            } else {
                peer_pause_ability = 0;
            }
            PHY_CHECK(eth->on_state_changed(eth, ETH_STATE_PAUSE, (void *)peer_pause_ability) == ESP_OK,
                      "change pause ability failed", err);
        }
        PHY_CHECK(eth->on_state_changed(eth, ETH_STATE_LINK, (void *)link) == ESP_OK,
                  "change link failed", err);
        ksz8081->link_status = link;
    }
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t ksz8081_set_mediator(esp_eth_phy_t *phy, esp_eth_mediator_t *eth)
{
    PHY_CHECK(eth, "can't set mediator to null", err);
    phy_ksz8081_t *ksz8081 = __containerof(phy, phy_ksz8081_t, parent);
    ksz8081->eth = eth;
    return ESP_OK;
err:
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t ksz8081_get_link(esp_eth_phy_t *phy)
{
    phy_ksz8081_t *ksz8081 = __containerof(phy, phy_ksz8081_t, parent);
    /* Update information about link, speed, duplex */
    PHY_CHECK(ksz8081_update_link_duplex_speed(ksz8081) == ESP_OK, "update link duplex speed failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t ksz8081_reset(esp_eth_phy_t *phy)
{
    phy_ksz8081_t *ksz8081 = __containerof(phy, phy_ksz8081_t, parent);
    ksz8081->link_status = ETH_LINK_DOWN;
    esp_eth_mediator_t *eth = ksz8081->eth;
    bmcr_reg_t bmcr = {.reset = 1};
    PHY_CHECK(eth->phy_reg_write(eth, ksz8081->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val) == ESP_OK,
              "write BMCR failed", err);
    /* wait for reset complete */
    uint32_t to = 0;
    for (to = 0; to < ksz8081->reset_timeout_ms / 10; to++) {
        vTaskDelay(pdMS_TO_TICKS(10));
        PHY_CHECK(eth->phy_reg_read(eth, ksz8081->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)) == ESP_OK,
                  "read BMCR failed", err);
        if (!bmcr.reset) {
            break;
        }
    }
    PHY_CHECK(to < ksz8081->reset_timeout_ms / 10, "reset timeout", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t ksz8081_reset_hw(esp_eth_phy_t *phy)
{
    phy_ksz8081_t *ksz8081 = __containerof(phy, phy_ksz8081_t, parent);
    if (ksz8081->reset_gpio_num >= 0) {
        esp_rom_gpio_pad_select_gpio(ksz8081->reset_gpio_num);
        gpio_set_direction(ksz8081->reset_gpio_num, GPIO_MODE_OUTPUT);
        gpio_set_level(ksz8081->reset_gpio_num, 0);
        esp_rom_delay_us(100); // insert min input assert time
        gpio_set_level(ksz8081->reset_gpio_num, 1);
    }
    return ESP_OK;
}

static esp_err_t ksz8081_negotiate(esp_eth_phy_t *phy)
{
    phy_ksz8081_t *ksz8081 = __containerof(phy, phy_ksz8081_t, parent);
    esp_eth_mediator_t *eth = ksz8081->eth;
    /* Restart auto negotiation */
    bmcr_reg_t bmcr = {
        .speed_select = 1,     /* 100Mbps */
        .duplex_mode = 1,      /* Full Duplex */
        .en_auto_nego = 1,     /* Auto Negotiation */
        .restart_auto_nego = 1 /* Restart Auto Negotiation */
    };
    PHY_CHECK(eth->phy_reg_write(eth, ksz8081->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val) == ESP_OK, "write BMCR failed", err);
    /* Wait for auto negotiation complete */
    bmsr_reg_t bmsr;
    uint32_t to = 0;
    for (to = 0; to < ksz8081->autonego_timeout_ms / 10; to++) {
        vTaskDelay(pdMS_TO_TICKS(10));
        PHY_CHECK(eth->phy_reg_read(eth, ksz8081->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)) == ESP_OK,
                  "read BMSR failed", err);
        if (bmsr.auto_nego_complete){ 
            break;
        }
    }
    /* Auto negotiation failed, maybe no network cable plugged in, so output a warning */
    if (to >= ksz8081->autonego_timeout_ms / 10) {
        ESP_LOGW(TAG, "auto negotiation timeout");
    }
    /* Updata information about link, speed, duplex */
    PHY_CHECK(ksz8081_update_link_duplex_speed(ksz8081) == ESP_OK, "update link duplex speed failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t ksz8081_pwrctl(esp_eth_phy_t *phy, bool enable)
{
    phy_ksz8081_t *ksz8081 = __containerof(phy, phy_ksz8081_t, parent);
    esp_eth_mediator_t *eth = ksz8081->eth;
    bmcr_reg_t bmcr;
    PHY_CHECK(eth->phy_reg_read(eth, ksz8081->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)) == ESP_OK,
              "read BMCR failed", err);
    if (!enable) {
        /* General Power Down Mode */
        bmcr.power_down = 1;
    } else {
        /* Normal operation Mode */
        bmcr.power_down = 0;
    }
    PHY_CHECK(eth->phy_reg_write(eth, ksz8081->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val) == ESP_OK,
              "write BMCR failed", err);
    if (!enable) {
        PHY_CHECK(eth->phy_reg_read(eth, ksz8081->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)) == ESP_OK,
                  "read BMCR failed", err);
        PHY_CHECK(bmcr.power_down == 1, "power down failed", err);
    } else {
        /* wait for power up complete */
        uint32_t to = 0;
        for (to = 0; to < ksz8081->reset_timeout_ms / 10; to++) {
            vTaskDelay(pdMS_TO_TICKS(10));
            PHY_CHECK(eth->phy_reg_read(eth, ksz8081->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)) == ESP_OK,
                      "read BMCR failed", err);
            if (bmcr.power_down == 0) {
                break;
            }
        }
        PHY_CHECK(to < ksz8081->reset_timeout_ms / 10, "power up timeout", err);
    }

    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t ksz8081_set_addr(esp_eth_phy_t *phy, uint32_t addr)
{
    phy_ksz8081_t *ksz8081 = __containerof(phy, phy_ksz8081_t, parent);
    ksz8081->addr = addr;
    return ESP_OK;
}

static esp_err_t ksz8081_get_addr(esp_eth_phy_t *phy, uint32_t *addr)
{
    PHY_CHECK(addr, "addr can't be null", err);
    phy_ksz8081_t *ksz8081 = __containerof(phy, phy_ksz8081_t, parent);
    *addr = ksz8081->addr;
    return ESP_OK;
err:
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t ksz8081_del(esp_eth_phy_t *phy)
{
    phy_ksz8081_t *ksz8081 = __containerof(phy, phy_ksz8081_t, parent);
    free(ksz8081);
    return ESP_OK;
}

static esp_err_t ksz8081_advertise_pause_ability(esp_eth_phy_t *phy, uint32_t ability)
{
    phy_ksz8081_t *ksz8081 = __containerof(phy, phy_ksz8081_t, parent);
    esp_eth_mediator_t *eth = ksz8081->eth;
    /* Set PAUSE function ability */
    anar_reg_t anar;
    PHY_CHECK(eth->phy_reg_read(eth, ksz8081->addr, ETH_PHY_ANAR_REG_ADDR, &(anar.val)) == ESP_OK,
              "read ANAR failed", err);
    if (ability) {
        anar.asymmetric_pause = 1;
        anar.symmetric_pause = 1;
    } else {
        anar.asymmetric_pause = 0;
        anar.symmetric_pause = 0;
    }
    PHY_CHECK(eth->phy_reg_write(eth, ksz8081->addr, ETH_PHY_ANAR_REG_ADDR, anar.val) == ESP_OK,
              "write ANAR failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t ksz8081_init(esp_eth_phy_t *phy)
{
    phy_ksz8081_t *ksz8081 = __containerof(phy, phy_ksz8081_t, parent);
    esp_eth_mediator_t *eth = ksz8081->eth;
    /* Power on Ethernet PHY */
    PHY_CHECK(ksz8081_pwrctl(phy, true) == ESP_OK, "power control failed", err);
    /* Reset Ethernet PHY */
    PHY_CHECK(ksz8081_reset(phy) == ESP_OK, "reset failed", err);
    /* Check PHY ID */
    phyidr1_reg_t id1;
    phyidr2_reg_t id2;
    PHY_CHECK(eth->phy_reg_read(eth, ksz8081->addr, ETH_PHY_IDR1_REG_ADDR, &(id1.val)) == ESP_OK,
              "read ID1 failed", err);
    PHY_CHECK(eth->phy_reg_read(eth, ksz8081->addr, ETH_PHY_IDR2_REG_ADDR, &(id2.val)) == ESP_OK,
              "read ID2 failed", err);
    PHY_CHECK(id1.oui_msb == 0x22 && id2.oui_lsb == 0x5 && id2.vendor_model == 0x16, "wrong chip ID", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t ksz8081_deinit(esp_eth_phy_t *phy)
{
    /* Power off Ethernet PHY */
    PHY_CHECK(ksz8081_pwrctl(phy, false) == ESP_OK, "power control failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

esp_eth_phy_t *esp_eth_phy_new_ksz8081(const eth_phy_config_t *config)
{
    PHY_CHECK(config, "can't set phy config to null", err);
    phy_ksz8081_t *ksz8081 = calloc(1, sizeof(phy_ksz8081_t));
    PHY_CHECK(ksz8081, "calloc ksz8081 failed", err);
    ksz8081->addr = config->phy_addr;
    ksz8081->reset_gpio_num = config->reset_gpio_num;
    ksz8081->reset_timeout_ms = config->reset_timeout_ms;
    ksz8081->link_status = ETH_LINK_DOWN;
    ksz8081->autonego_timeout_ms = config->autonego_timeout_ms;
    ksz8081->parent.reset = ksz8081_reset;
    ksz8081->parent.reset_hw = ksz8081_reset_hw;
    ksz8081->parent.init = ksz8081_init;
    ksz8081->parent.deinit = ksz8081_deinit;
    ksz8081->parent.set_mediator = ksz8081_set_mediator;
    ksz8081->parent.negotiate = ksz8081_negotiate;
    ksz8081->parent.get_link = ksz8081_get_link;
    ksz8081->parent.pwrctl = ksz8081_pwrctl;
    ksz8081->parent.get_addr = ksz8081_get_addr;
    ksz8081->parent.set_addr = ksz8081_set_addr;
    ksz8081->parent.advertise_pause_ability = ksz8081_advertise_pause_ability;
    ksz8081->parent.del = ksz8081_del;

    return &(ksz8081->parent);
err:
    return NULL;
}
