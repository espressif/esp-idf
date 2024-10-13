/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include "esp_log.h"
#include "esp_check.h"
#include "esp_eth_phy_802_3.h"

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
    phy_802_3_t phy_802_3;
} phy_dp83848_t;

static esp_err_t dp83848_update_link_duplex_speed(phy_dp83848_t *dp83848)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = dp83848->phy_802_3.eth;
    uint32_t addr = dp83848->phy_802_3.addr;
    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    uint32_t peer_pause_ability = false;
    anlpar_reg_t anlpar;
    physts_reg_t physts;
    bmsr_reg_t bmsr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)), err, TAG, "read BMSR failed");
    eth_link_t link = bmsr.link_status ? ETH_LINK_UP : ETH_LINK_DOWN;
    /* check if link status changed */
    if (dp83848->phy_802_3.link_status != link) {
        /* when link up, read negotiation result */
        if (link == ETH_LINK_UP) {
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_ANLPAR_REG_ADDR, &(anlpar.val)), err, TAG, "read ANLPAR failed");
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_STS_REG_ADDR, &(physts.val)), err, TAG, "read PHYSTS failed");
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
        dp83848->phy_802_3.link_status = link;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dp83848_get_link(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_dp83848_t *dp83848 = __containerof(esp_eth_phy_into_phy_802_3(phy), phy_dp83848_t, phy_802_3);
    /* Update information about link, speed, duplex */
    ESP_GOTO_ON_ERROR(dp83848_update_link_duplex_speed(dp83848), err, TAG, "update link duplex speed failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dp83848_autonego_ctrl(esp_eth_phy_t *phy, eth_phy_autoneg_cmd_t cmd, bool *autonego_en_stat)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = esp_eth_phy_into_phy_802_3(phy);
    esp_eth_mediator_t *eth = phy_802_3->eth;
    if (cmd == ESP_ETH_PHY_AUTONEGO_EN) {
        bmcr_reg_t bmcr;
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
        ESP_GOTO_ON_FALSE(bmcr.en_loopback == 0, ESP_ERR_INVALID_STATE, err, TAG, "Autonegotiation can't be enabled while in loopback operation");
    }
    return esp_eth_phy_802_3_autonego_ctrl(phy_802_3, cmd, autonego_en_stat);
err:
    return ret;
}

static esp_err_t dp83848_loopback(esp_eth_phy_t *phy, bool enable)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = esp_eth_phy_into_phy_802_3(phy);
    bool auto_nego_en = true;
    ESP_GOTO_ON_ERROR(dp83848_autonego_ctrl(phy, ESP_ETH_PHY_AUTONEGO_G_STAT, &auto_nego_en), err, TAG, "get status of autonegotiation failed");
    ESP_GOTO_ON_FALSE(!(auto_nego_en && enable), ESP_ERR_INVALID_STATE, err, TAG, "Unable to set loopback while autonegotiation is enabled. Disable it to use loopback");
    return esp_eth_phy_802_3_loopback(phy_802_3, enable);
err:
    return ret;
}

static esp_err_t dp83848_init(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = esp_eth_phy_into_phy_802_3(phy);

    /* Basic PHY init */
    ESP_GOTO_ON_ERROR(esp_eth_phy_802_3_basic_phy_init(phy_802_3), err, TAG, "failed to init PHY");

    /* Check PHY ID */
    uint32_t oui;
    uint8_t model;
    ESP_GOTO_ON_ERROR(esp_eth_phy_802_3_read_oui(phy_802_3, &oui), err, TAG, "read OUI failed");
    ESP_GOTO_ON_ERROR(esp_eth_phy_802_3_read_manufac_info(phy_802_3, &model, NULL), err, TAG, "read manufacturer's info failed");
    ESP_GOTO_ON_FALSE(oui == 0x80017 && model == 0x09, ESP_FAIL, err, TAG, "wrong chip ID");

    return ESP_OK;
err:
    return ret;
}

esp_eth_phy_t *esp_eth_phy_new_dp83848(const eth_phy_config_t *config)
{
    esp_eth_phy_t *ret = NULL;
    phy_dp83848_t *dp83848 = calloc(1, sizeof(phy_dp83848_t));
    ESP_GOTO_ON_FALSE(dp83848, NULL, err, TAG, "calloc dp83848 failed");
    ESP_GOTO_ON_FALSE(esp_eth_phy_802_3_obj_config_init(&dp83848->phy_802_3, config) == ESP_OK,
                        NULL, err, TAG, "configuration initialization of PHY 802.3 failed");

    // redefine functions which need to be customized for sake of dp83848
    dp83848->phy_802_3.parent.init = dp83848_init;
    dp83848->phy_802_3.parent.get_link = dp83848_get_link;
    dp83848->phy_802_3.parent.autonego_ctrl = dp83848_autonego_ctrl;
    dp83848->phy_802_3.parent.loopback = dp83848_loopback;

    return &dp83848->phy_802_3.parent;
err:
    if (dp83848 != NULL) {
        free(dp83848);
    }
    return ret;
}
