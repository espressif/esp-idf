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

static const char *TAG = "ip101";

/***************Vendor Specific Register***************/

/**
 * @brief PCR(Page Control Register)
 *
 */
typedef union {
    struct {
        uint32_t register_page_select : 5; /* Select register page, default is 16 */
        uint32_t reserved : 11;            /* Reserved */
    };
    uint32_t val;
} pcr_reg_t;
#define ETH_PHY_PCR_REG_ADDR (0x14)

/**
 * @brief ISR(Interrupt Status Register), Page 16
 *
 */
typedef union {
    struct {
        uint32_t link_changed : 1;   /* Flag to indicate link status change interrupt */
        uint32_t duplex_changed : 1; /* Flag to indicate duplex change interrupt */
        uint32_t speed_changed : 1;  /* Flag to indicate speed change interrupt */
        uint32_t intr_status : 1;    /* Flag to indicate interrupt status */
        uint32_t reserved1 : 4;      /* Reserved */
        uint32_t link_mask : 1;      /* Mask link change interrupt */
        uint32_t duplex_mask : 1;    /* Mask duplex change interrupt */
        uint32_t speed_mask : 1;     /* Mask speed change interrupt */
        uint32_t all_mask : 1;       /* Mask all interrupt */
        uint32_t reserved2 : 3;      /* Reserved */
        uint32_t use_intr_pin : 1;   /* Set high to use INTR and INTR_32 as an interrupt pin */
    };
    uint32_t val;
} isr_reg_t;
#define ETH_PHY_ISR_REG_ADDR (0x11)

/**
 * @brief PHY MDI/MDIX Control and Specific Status Register, Page 16
 *
 */
typedef union {
    struct {
        uint32_t op_mode : 3;    /* Operation Mode Idicator */
        uint32_t force_mdix : 1; /* Force the MDIX channel to be selected */
        uint32_t reserved1 : 4;  /* Reserved */
        uint32_t link_up : 1;    /* Indicate the link status is OK or FAIL */
        uint32_t reserved2 : 7;  /* Reserved */
    };
    uint32_t val;
} cssr_reg_t;
#define ETH_PHY_CSSR_REG_ADDR (0x1E)

/**
 * @brief PSCR(PHY Specific Control Register), Page 1
 *
 */
typedef union {
    struct {
        uint32_t reserved1 : 7;      /* Reserved */
        uint32_t force_link_100 : 1; /* Force Link 100 */
        uint32_t force_link_10 : 1;  /* Force Link 10 */
        uint32_t reserved2 : 7;      /* Reserved */
    };
    uint32_t val;
} pscr_reg_t;
#define ETH_PHY_PSCR_REG_ADDR (0x11)

typedef struct {
    esp_eth_phy_t parent;
    esp_eth_mediator_t *eth;
    int addr;
    uint32_t reset_timeout_ms;
    uint32_t autonego_timeout_ms;
    eth_link_t link_status;
    int reset_gpio_num;
} phy_ip101_t;

static esp_err_t ip101_page_select(phy_ip101_t *ip101, uint32_t page)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = ip101->eth;
    pcr_reg_t pcr = {
        .register_page_select = page
    };
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ip101->addr, ETH_PHY_PCR_REG_ADDR, pcr.val), err, TAG, "write PCR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ip101_update_link_duplex_speed(phy_ip101_t *ip101)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = ip101->eth;
    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    uint32_t peer_pause_ability = false;
    cssr_reg_t cssr;
    anlpar_reg_t anlpar;
    ESP_GOTO_ON_ERROR(ip101_page_select(ip101, 16), err, TAG, "select page 16 failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ip101->addr, ETH_PHY_CSSR_REG_ADDR, &(cssr.val)), err, TAG, "read CSSR failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ip101->addr, ETH_PHY_ANLPAR_REG_ADDR, &(anlpar.val)), err, TAG, "read ANLPAR failed");
    eth_link_t link = cssr.link_up ? ETH_LINK_UP : ETH_LINK_DOWN;
    /* check if link status changed */
    if (ip101->link_status != link) {
        /* when link up, read negotiation result */
        if (link == ETH_LINK_UP) {
            switch (cssr.op_mode) {
            case 1: //10M Half
                speed = ETH_SPEED_10M;
                duplex = ETH_DUPLEX_HALF;
                break;
            case 2: //100M Half
                speed = ETH_SPEED_100M;
                duplex = ETH_DUPLEX_HALF;
                break;
            case 5: //10M Full
                speed = ETH_SPEED_10M;
                duplex = ETH_DUPLEX_FULL;
                break;
            case 6: //100M Full
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
        ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_LINK, (void *)link), err, TAG, "chagne link failed");
        ip101->link_status = link;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ip101_set_mediator(esp_eth_phy_t *phy, esp_eth_mediator_t *eth)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(eth, ESP_ERR_INVALID_ARG, err, TAG, "can't set mediator to null");
    phy_ip101_t *ip101 = __containerof(phy, phy_ip101_t, parent);
    ip101->eth = eth;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ip101_get_link(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_ip101_t *ip101 = __containerof(phy, phy_ip101_t, parent);
    /* Updata information about link, speed, duplex */
    ESP_GOTO_ON_ERROR(ip101_update_link_duplex_speed(ip101), err, TAG, "update link duplex speed failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ip101_reset(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_ip101_t *ip101 = __containerof(phy, phy_ip101_t, parent);
    ip101->link_status = ETH_LINK_DOWN;
    esp_eth_mediator_t *eth = ip101->eth;
    bmcr_reg_t bmcr = {.reset = 1};
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ip101->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    /* wait for reset complete */
    uint32_t to = 0;
    for (to = 0; to < ip101->reset_timeout_ms / 10; to++) {
        vTaskDelay(pdMS_TO_TICKS(10));
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ip101->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
        if (!bmcr.reset) {
            break;
        }
    }
    ESP_GOTO_ON_FALSE(to < ip101->reset_timeout_ms / 10, ESP_FAIL, err, TAG, "reset timeout");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ip101_reset_hw(esp_eth_phy_t *phy)
{
    phy_ip101_t *ip101 = __containerof(phy, phy_ip101_t, parent);
    if (ip101->reset_gpio_num >= 0) {
        esp_rom_gpio_pad_select_gpio(ip101->reset_gpio_num);
        gpio_set_direction(ip101->reset_gpio_num, GPIO_MODE_OUTPUT);
        gpio_set_level(ip101->reset_gpio_num, 0);
        esp_rom_delay_us(100); // insert min input assert time
        gpio_set_level(ip101->reset_gpio_num, 1);
    }
    return ESP_OK;
}

/**
 * @note This function is responsible for restarting a new auto-negotiation,
 *       the result of negotiation won't be relected to uppler layers.
 *       Instead, the negotiation result is fetched by linker timer, see `ip101_get_link()`
 */
static esp_err_t ip101_negotiate(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_ip101_t *ip101 = __containerof(phy, phy_ip101_t, parent);
    esp_eth_mediator_t *eth = ip101->eth;
    /* in case any link status has changed, let's assume we're in link down status */
    ip101->link_status = ETH_LINK_DOWN;
    /* Restart auto negotiation */
    bmcr_reg_t bmcr = {
        .speed_select = 1,     /* 100Mbps */
        .duplex_mode = 1,      /* Full Duplex */
        .en_auto_nego = 1,     /* Auto Negotiation */
        .restart_auto_nego = 1 /* Restart Auto Negotiation */
    };
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ip101->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    /* Wait for auto negotiation complete */
    bmsr_reg_t bmsr;
    uint32_t to = 0;
    for (to = 0; to < ip101->autonego_timeout_ms / 100; to++) {
        vTaskDelay(pdMS_TO_TICKS(100));
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ip101->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)), err, TAG, "read BMSR failed");
        if (bmsr.auto_nego_complete) {
            break;
        }
    }
    if ((to >= ip101->autonego_timeout_ms / 100) && (ip101->link_status == ETH_LINK_UP)) {
        ESP_LOGW(TAG, "auto negotiation timeout");
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ip101_pwrctl(esp_eth_phy_t *phy, bool enable)
{
    esp_err_t ret = ESP_OK;
    phy_ip101_t *ip101 = __containerof(phy, phy_ip101_t, parent);
    esp_eth_mediator_t *eth = ip101->eth;
    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ip101->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    if (!enable) {
        /* Enable IEEE Power Down Mode */
        bmcr.power_down = 1;
    } else {
        /* Disable IEEE Power Down Mode */
        bmcr.power_down = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ip101->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    if (!enable) {
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ip101->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
        ESP_GOTO_ON_FALSE(bmcr.power_down == 1, ESP_FAIL, err, TAG, "power down failed");
    } else {
        /* wait for power up complete */
        uint32_t to = 0;
        for (to = 0; to < ip101->reset_timeout_ms / 10; to++) {
            vTaskDelay(pdMS_TO_TICKS(10));
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ip101->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
            if (bmcr.power_down == 0) {
                break;
            }
        }
        ESP_GOTO_ON_FALSE(to < ip101->reset_timeout_ms / 10, ESP_FAIL, err, TAG, "power up timeout");
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ip101_set_addr(esp_eth_phy_t *phy, uint32_t addr)
{
    phy_ip101_t *ip101 = __containerof(phy, phy_ip101_t, parent);
    ip101->addr = addr;
    return ESP_OK;
}

static esp_err_t ip101_get_addr(esp_eth_phy_t *phy, uint32_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "addr can't be null");
    phy_ip101_t *ip101 = __containerof(phy, phy_ip101_t, parent);
    *addr = ip101->addr;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ip101_del(esp_eth_phy_t *phy)
{
    phy_ip101_t *ip101 = __containerof(phy, phy_ip101_t, parent);
    free(ip101);
    return ESP_OK;
}

static esp_err_t ip101_advertise_pause_ability(esp_eth_phy_t *phy, uint32_t ability)
{
    esp_err_t ret = ESP_OK;
    phy_ip101_t *ip101 = __containerof(phy, phy_ip101_t, parent);
    esp_eth_mediator_t *eth = ip101->eth;
    /* Set PAUSE function ability */
    anar_reg_t anar;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ip101->addr, ETH_PHY_ANAR_REG_ADDR, &(anar.val)), err, TAG, "read ANAR failed");
    if (ability) {
        anar.asymmetric_pause = 1;
        anar.symmetric_pause = 1;
    } else {
        anar.asymmetric_pause = 0;
        anar.symmetric_pause = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ip101->addr, ETH_PHY_ANAR_REG_ADDR, anar.val), err, TAG, "write ANAR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ip101_loopback(esp_eth_phy_t *phy, bool enable)
{
    esp_err_t ret = ESP_OK;
    phy_ip101_t *ip101 = __containerof(phy, phy_ip101_t, parent);
    esp_eth_mediator_t *eth = ip101->eth;
    /* Set Loopback function */
    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ip101->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    if (enable) {
        bmcr.en_loopback = 1;
    } else {
        bmcr.en_loopback = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ip101->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ip101_init(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_ip101_t *ip101 = __containerof(phy, phy_ip101_t, parent);
    esp_eth_mediator_t *eth = ip101->eth;
    // Detect PHY address
    if (ip101->addr == ESP_ETH_PHY_ADDR_AUTO) {
        ESP_GOTO_ON_ERROR(esp_eth_detect_phy_addr(eth, &ip101->addr), err, TAG, "Detect PHY address failed");
    }
    /* Power on Ethernet PHY */
    ESP_GOTO_ON_ERROR(ip101_pwrctl(phy, true), err, TAG, "power control failed");
    /* Reset Ethernet PHY */
    ESP_GOTO_ON_ERROR(ip101_reset(phy), err, TAG, "reset failed");
    /* Check PHY ID */
    phyidr1_reg_t id1;
    phyidr2_reg_t id2;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ip101->addr, ETH_PHY_IDR1_REG_ADDR, &(id1.val)), err, TAG, "read ID1 failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ip101->addr, ETH_PHY_IDR2_REG_ADDR, &(id2.val)), err, TAG, "read ID2 failed");
    ESP_GOTO_ON_FALSE(id1.oui_msb == 0x243 && id2.oui_lsb == 0x3 && id2.vendor_model == 0x5, ESP_FAIL, err, TAG, "wrong chip ID");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ip101_deinit(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    /* Power off Ethernet PHY */
    ESP_GOTO_ON_ERROR(ip101_pwrctl(phy, false), err, TAG, "power control failed");
    return ESP_OK;
err:
    return ret;
}

esp_eth_phy_t *esp_eth_phy_new_ip101(const eth_phy_config_t *config)
{
    esp_eth_phy_t *ret = NULL;
    ESP_GOTO_ON_FALSE(config, NULL, err, TAG, "can't set phy config to null");
    phy_ip101_t *ip101 = calloc(1, sizeof(phy_ip101_t));
    ESP_GOTO_ON_FALSE(ip101, NULL, err, TAG, "calloc ip101 failed");
    ip101->addr = config->phy_addr;
    ip101->reset_timeout_ms = config->reset_timeout_ms;
    ip101->reset_gpio_num = config->reset_gpio_num;
    ip101->link_status = ETH_LINK_DOWN;
    ip101->autonego_timeout_ms = config->autonego_timeout_ms;
    ip101->parent.reset = ip101_reset;
    ip101->parent.reset_hw = ip101_reset_hw;
    ip101->parent.init = ip101_init;
    ip101->parent.deinit = ip101_deinit;
    ip101->parent.set_mediator = ip101_set_mediator;
    ip101->parent.negotiate = ip101_negotiate;
    ip101->parent.get_link = ip101_get_link;
    ip101->parent.pwrctl = ip101_pwrctl;
    ip101->parent.get_addr = ip101_get_addr;
    ip101->parent.set_addr = ip101_set_addr;
    ip101->parent.advertise_pause_ability = ip101_advertise_pause_ability;
    ip101->parent.loopback = ip101_loopback;
    ip101->parent.del = ip101_del;

    return &(ip101->parent);
err:
    return ret;
}
