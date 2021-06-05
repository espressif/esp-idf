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

static const char *TAG = "lan8720";

/***************Vendor Specific Register***************/

/**
 * @brief MCSR(Mode Control Status Register)
 *
 */
typedef union {
    struct {
        uint32_t reserved1 : 1;                  /* Reserved */
        uint32_t energy_is_on : 1;               /* Energy is On */
        uint32_t reserved2 : 4;                  /* Reserved */
        uint32_t en_alternate_interrupt : 1;     /* Enable Alternate Interrupt Mode */
        uint32_t reserved3 : 2;                  /* Reserved */
        uint32_t en_far_loopback : 1;            /* Enable Far Loopback Mode */
        uint32_t reserved4 : 3;                  /* Reserved */
        uint32_t en_energy_detect_powerdown : 1; /* Enable Energy Detect Power Down */
        uint32_t reserved5 : 2;                  /* Reserved */
    };
    uint32_t val;
} mcsr_reg_t;
#define ETH_PHY_MCSR_REG_ADDR (0x11)

/**
 * @brief SMR(Special Modes Register)
 *
 */
typedef union {
    struct {
        uint32_t phy_addr : 5; /* PHY Address */
        uint32_t mode : 3;     /* Transceiver Mode of Operation */
        uint32_t reserved : 8; /* Reserved */
    };
    uint32_t val;
} smr_reg_t;
#define ETH_PHY_SMR_REG_ADDR (0x12)

/**
 * @brief SECR(Symbol Error Counter Register)
 *
 */
typedef union {
    struct {
        uint32_t symbol_err_count : 16; /* Symbol Error Counter */
    };
    uint32_t val;
} secr_reg_t;
#define EHT_PHY_SECR_REG_ADDR (0x1A)

/**
 * @brief CSIR(Control Status Indications Register)
 *
 */
typedef union {
    struct {
        uint32_t reserved1 : 4;         /* Reserved */
        uint32_t base10_t_polarity : 1; /* Polarity State of 10Base-T */
        uint32_t reserved2 : 6;         /* Reserved */
        uint32_t dis_sqe : 1;           /* Disable SQE test(Heartbeat) */
        uint32_t reserved3 : 1;         /* Reserved */
        uint32_t select_channel : 1;    /* Manual channel select:MDI(0) or MDIX(1) */
        uint32_t reserved4 : 1;         /* Reserved */
        uint32_t auto_mdix_ctrl : 1;    /* Auto-MDIX Control: EN(0) or DE(1) */
    };
    uint32_t val;
} scsir_reg_t;
#define ETH_PHY_CSIR_REG_ADDR (0x1B)

/**
 * @brief ISR(Interrupt Source Register)
 *
 */
typedef union {
    struct {
        uint32_t reserved1 : 1;                /* Reserved */
        uint32_t auto_nego_page_received : 1;  /* Auto-Negotiation Page Received */
        uint32_t parallel_detect_fault : 1;    /* Parallel Detection Fault */
        uint32_t auto_nego_lp_acknowledge : 1; /* Auto-Negotiation LP Acknowledge */
        uint32_t link_down : 1;                /* Link Down */
        uint32_t remote_fault_detect : 1;      /* Remote Fault Detect */
        uint32_t auto_nego_complete : 1;       /* Auto-Negotiation Complete */
        uint32_t energy_on_generate : 1;       /* ENERYON generated */
        uint32_t reserved2 : 8;                /* Reserved */
    };
    uint32_t val;
} isfr_reg_t;
#define ETH_PHY_ISR_REG_ADDR (0x1D)

/**
 * @brief IMR(Interrupt Mask Register)
 *
 */
typedef union {
    struct {
        uint32_t reserved1 : 1;                /* Reserved */
        uint32_t auto_nego_page_received : 1;  /* Auto-Negotiation Page Received */
        uint32_t parallel_detect_fault : 1;    /* Parallel Detection Fault */
        uint32_t auto_nego_lp_acknowledge : 1; /* Auto-Negotiation LP Acknowledge */
        uint32_t link_down : 1;                /* Link Down */
        uint32_t remote_fault_detect : 1;      /* Remote Fault Detect */
        uint32_t auto_nego_complete : 1;       /* Auto-Negotiation Complete */
        uint32_t energy_on_generate : 1;       /* ENERGY ON generated */
        uint32_t reserved2 : 8;                /* Reserved */
    };
    uint32_t val;
} imr_reg_t;
#define ETH_PHY_IMR_REG_ADDR (0x1E)

/**
 * @brief PSCSR(PHY Special Control Status Register)
 *
 */
typedef union {
    struct {
        uint32_t reserved1 : 2;        /* Reserved */
        uint32_t speed_indication : 3; /* Speed Indication */
        uint32_t reserved2 : 7;        /* Reserved */
        uint32_t auto_nego_done : 1;   /* Auto Negotiation Done */
        uint32_t reserved3 : 3;        /* Reserved */
    };
    uint32_t val;
} pscsr_reg_t;
#define ETH_PHY_PSCSR_REG_ADDR (0x1F)

typedef struct {
    esp_eth_phy_t parent;
    esp_eth_mediator_t *eth;
    int addr;
    uint32_t reset_timeout_ms;
    uint32_t autonego_timeout_ms;
    eth_link_t link_status;
    int reset_gpio_num;
} phy_lan8720_t;

static esp_err_t lan8720_update_link_duplex_speed(phy_lan8720_t *lan8720)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = lan8720->eth;
    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    bmsr_reg_t bmsr;
    pscsr_reg_t pscsr;
    uint32_t peer_pause_ability = false;
    anlpar_reg_t anlpar;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan8720->addr, ETH_PHY_ANLPAR_REG_ADDR, &(anlpar.val)), err, TAG, "read ANLPAR failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan8720->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)), err, TAG, "read BMSR failed");
    eth_link_t link = bmsr.link_status ? ETH_LINK_UP : ETH_LINK_DOWN;
    /* check if link status changed */
    if (lan8720->link_status != link) {
        /* when link up, read negotiation result */
        if (link == ETH_LINK_UP) {
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan8720->addr, ETH_PHY_PSCSR_REG_ADDR, &(pscsr.val)), err, TAG, "read PSCSR failed");
            switch (pscsr.speed_indication) {
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
        lan8720->link_status = link;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan8720_set_mediator(esp_eth_phy_t *phy, esp_eth_mediator_t *eth)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(eth, ESP_ERR_INVALID_ARG, err, TAG, "can't set mediator to null");
    phy_lan8720_t *lan8720 = __containerof(phy, phy_lan8720_t, parent);
    lan8720->eth = eth;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan8720_get_link(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_lan8720_t *lan8720 = __containerof(phy, phy_lan8720_t, parent);
    /* Updata information about link, speed, duplex */
    ESP_GOTO_ON_ERROR(lan8720_update_link_duplex_speed(lan8720), err, TAG, "update link duplex speed failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan8720_reset(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_lan8720_t *lan8720 = __containerof(phy, phy_lan8720_t, parent);
    lan8720->link_status = ETH_LINK_DOWN;
    esp_eth_mediator_t *eth = lan8720->eth;
    bmcr_reg_t bmcr = {.reset = 1};
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, lan8720->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    /* wait for reset complete */
    uint32_t to = 0;
    for (to = 0; to < lan8720->reset_timeout_ms / 10; to++) {
        vTaskDelay(pdMS_TO_TICKS(10));
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan8720->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
        if (!bmcr.reset) {
            break;
        }
    }
    ESP_GOTO_ON_FALSE(to < lan8720->reset_timeout_ms / 10, ESP_FAIL, err, TAG, "reset timeout");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan8720_reset_hw(esp_eth_phy_t *phy)
{
    phy_lan8720_t *lan8720 = __containerof(phy, phy_lan8720_t, parent);
    if (lan8720->reset_gpio_num >= 0) {
        esp_rom_gpio_pad_select_gpio(lan8720->reset_gpio_num);
        gpio_set_direction(lan8720->reset_gpio_num, GPIO_MODE_OUTPUT);
        gpio_set_level(lan8720->reset_gpio_num, 0);
        esp_rom_delay_us(100); // insert min input assert time
        gpio_set_level(lan8720->reset_gpio_num, 1);
    }
    return ESP_OK;
}

static esp_err_t lan8720_negotiate(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_lan8720_t *lan8720 = __containerof(phy, phy_lan8720_t, parent);
    esp_eth_mediator_t *eth = lan8720->eth;
    /* Restart auto negotiation */
    bmcr_reg_t bmcr = {
        .speed_select = 1,     /* 100Mbps */
        .duplex_mode = 1,      /* Full Duplex */
        .en_auto_nego = 1,     /* Auto Negotiation */
        .restart_auto_nego = 1 /* Restart Auto Negotiation */
    };
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, lan8720->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    /* Wait for auto negotiation complete */
    bmsr_reg_t bmsr;
    pscsr_reg_t pscsr;
    uint32_t to = 0;
    for (to = 0; to < lan8720->autonego_timeout_ms / 10; to++) {
        vTaskDelay(pdMS_TO_TICKS(10));
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan8720->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)), err, TAG, "read BMSR failed");
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan8720->addr, ETH_PHY_PSCSR_REG_ADDR, &(pscsr.val)), err, TAG, "read PSCSR failed");
        if (bmsr.auto_nego_complete && pscsr.auto_nego_done) {
            break;
        }
    }
    /* Auto negotiation failed, maybe no network cable plugged in, so output a warning */
    if (to >= lan8720->autonego_timeout_ms / 10) {
        ESP_LOGW(TAG, "auto negotiation timeout");
    }
    /* Updata information about link, speed, duplex */
    ESP_GOTO_ON_ERROR(lan8720_update_link_duplex_speed(lan8720), err, TAG, "update link duplex speed failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan8720_pwrctl(esp_eth_phy_t *phy, bool enable)
{
    esp_err_t ret = ESP_OK;
    phy_lan8720_t *lan8720 = __containerof(phy, phy_lan8720_t, parent);
    esp_eth_mediator_t *eth = lan8720->eth;
    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan8720->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    if (!enable) {
        /* General Power Down Mode */
        bmcr.power_down = 1;
    } else {
        /* Normal operation Mode */
        bmcr.power_down = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, lan8720->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    if (!enable) {
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan8720->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
        ESP_GOTO_ON_FALSE(bmcr.power_down == 1, ESP_FAIL, err, TAG, "power down failed");
    } else {
        /* wait for power up complete */
        uint32_t to = 0;
        for (to = 0; to < lan8720->reset_timeout_ms / 10; to++) {
            vTaskDelay(pdMS_TO_TICKS(10));
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan8720->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
            if (bmcr.power_down == 0) {
                break;
            }
        }
        ESP_GOTO_ON_FALSE(to < lan8720->reset_timeout_ms / 10, ESP_FAIL, err, TAG, "power up timeout");
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan8720_set_addr(esp_eth_phy_t *phy, uint32_t addr)
{
    phy_lan8720_t *lan8720 = __containerof(phy, phy_lan8720_t, parent);
    lan8720->addr = addr;
    return ESP_OK;
}

static esp_err_t lan8720_get_addr(esp_eth_phy_t *phy, uint32_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "addr can't be null");
    phy_lan8720_t *lan8720 = __containerof(phy, phy_lan8720_t, parent);
    *addr = lan8720->addr;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan8720_del(esp_eth_phy_t *phy)
{
    phy_lan8720_t *lan8720 = __containerof(phy, phy_lan8720_t, parent);
    free(lan8720);
    return ESP_OK;
}

static esp_err_t lan8720_advertise_pause_ability(esp_eth_phy_t *phy, uint32_t ability)
{
    esp_err_t ret = ESP_OK;
    phy_lan8720_t *lan8720 = __containerof(phy, phy_lan8720_t, parent);
    esp_eth_mediator_t *eth = lan8720->eth;
    /* Set PAUSE function ability */
    anar_reg_t anar;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan8720->addr, ETH_PHY_ANAR_REG_ADDR, &(anar.val)), err, TAG, "read ANAR failed");
    if (ability) {
        anar.asymmetric_pause = 1;
        anar.symmetric_pause = 1;
    } else {
        anar.asymmetric_pause = 0;
        anar.symmetric_pause = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, lan8720->addr, ETH_PHY_ANAR_REG_ADDR, anar.val), err, TAG, "write ANAR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan8720_init(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_lan8720_t *lan8720 = __containerof(phy, phy_lan8720_t, parent);
    esp_eth_mediator_t *eth = lan8720->eth;
    // Detect PHY address
    if (lan8720->addr == ESP_ETH_PHY_ADDR_AUTO) {
        ESP_GOTO_ON_ERROR(esp_eth_detect_phy_addr(eth, &lan8720->addr), err, TAG, "Detect PHY address failed");
    }
    /* Power on Ethernet PHY */
    ESP_GOTO_ON_ERROR(lan8720_pwrctl(phy, true), err, TAG, "power control failed");
    /* Reset Ethernet PHY */
    ESP_GOTO_ON_ERROR(lan8720_reset(phy), err, TAG, "reset failed");
    /* Check PHY ID */
    phyidr1_reg_t id1;
    phyidr2_reg_t id2;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan8720->addr, ETH_PHY_IDR1_REG_ADDR, &(id1.val)), err, TAG, "read ID1 failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan8720->addr, ETH_PHY_IDR2_REG_ADDR, &(id2.val)), err, TAG, "read ID2 failed");
    ESP_GOTO_ON_FALSE(id1.oui_msb == 0x7 && id2.oui_lsb == 0x30 && id2.vendor_model == 0xF, ESP_FAIL, err, TAG, "wrong chip ID");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan8720_deinit(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    /* Power off Ethernet PHY */
    ESP_GOTO_ON_ERROR(lan8720_pwrctl(phy, false), err, TAG, "power control failed");
    return ESP_OK;
err:
    return ret;
}

esp_eth_phy_t *esp_eth_phy_new_lan8720(const eth_phy_config_t *config)
{
    esp_eth_phy_t *ret = NULL;
    ESP_GOTO_ON_FALSE(config, NULL, err, TAG, "can't set phy config to null");
    phy_lan8720_t *lan8720 = calloc(1, sizeof(phy_lan8720_t));
    ESP_GOTO_ON_FALSE(lan8720, NULL, err, TAG, "calloc lan8720 failed");
    lan8720->addr = config->phy_addr;
    lan8720->reset_gpio_num = config->reset_gpio_num;
    lan8720->reset_timeout_ms = config->reset_timeout_ms;
    lan8720->link_status = ETH_LINK_DOWN;
    lan8720->autonego_timeout_ms = config->autonego_timeout_ms;
    lan8720->parent.reset = lan8720_reset;
    lan8720->parent.reset_hw = lan8720_reset_hw;
    lan8720->parent.init = lan8720_init;
    lan8720->parent.deinit = lan8720_deinit;
    lan8720->parent.set_mediator = lan8720_set_mediator;
    lan8720->parent.negotiate = lan8720_negotiate;
    lan8720->parent.get_link = lan8720_get_link;
    lan8720->parent.pwrctl = lan8720_pwrctl;
    lan8720->parent.get_addr = lan8720_get_addr;
    lan8720->parent.set_addr = lan8720_set_addr;
    lan8720->parent.advertise_pause_ability = lan8720_advertise_pause_ability;
    lan8720->parent.del = lan8720_del;

    return &(lan8720->parent);
err:
    return ret;
}
