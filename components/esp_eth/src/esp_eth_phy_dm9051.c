/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include "esp_log.h"
#include "esp_check.h"
#include "esp_eth_phy_802_3.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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
    phy_802_3_t phy_802_3;
} phy_dm9051_t;

static esp_err_t dm9051_update_link_duplex_speed(phy_dm9051_t *dm9051)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = dm9051->phy_802_3.eth;
    uint32_t addr = dm9051->phy_802_3.addr;
    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    uint32_t peer_pause_ability = false;
    bmsr_reg_t bmsr;
    dscsr_reg_t dscsr;
    anlpar_reg_t anlpar;
    // BMSR is a latch low register
    // after power up, the first latched value must be 0, which means down
    // to speed up power up link speed, double read this register as a workaround
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)), err, TAG, "read BMSR failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)), err, TAG, "read BMSR failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_ANLPAR_REG_ADDR, &(anlpar.val)), err, TAG, "read ANLPAR failed");
    eth_link_t link = bmsr.link_status ? ETH_LINK_UP : ETH_LINK_DOWN;
    /* check if link status changed */
    if (dm9051->phy_802_3.link_status != link) {
        /* when link up, read negotiation result */
        if (link == ETH_LINK_UP) {
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_DSCSR_REG_ADDR, &(dscsr.val)), err, TAG, "read DSCSR failed");
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
        dm9051->phy_802_3.link_status = link;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dm9051_get_link(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_dm9051_t *dm9051 = __containerof(esp_eth_phy_into_phy_802_3(phy), phy_dm9051_t, phy_802_3);
    /* Update information about link, speed, duplex */
    ESP_GOTO_ON_ERROR(dm9051_update_link_duplex_speed(dm9051), err, TAG, "update link duplex speed failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dm9051_reset(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_dm9051_t *dm9051 = __containerof(esp_eth_phy_into_phy_802_3(phy), phy_dm9051_t, phy_802_3);
    uint32_t addr = dm9051->phy_802_3.addr;
    dm9051->phy_802_3.link_status = ETH_LINK_DOWN;
    esp_eth_mediator_t *eth = dm9051->phy_802_3.eth;
    dscr_reg_t dscr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_DSCR_REG_ADDR, &(dscr.val)), err, TAG, "read DSCR failed");
    dscr.smrst = 1;
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, addr, ETH_PHY_DSCR_REG_ADDR, dscr.val), err, TAG, "write DSCR failed");
    bmcr_reg_t bmcr = {.reset = 1};
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    /* Wait for reset complete */
    uint32_t to = 0;
    for (to = 0; to < dm9051->phy_802_3.reset_timeout_ms / 10; to++) {
        vTaskDelay(pdMS_TO_TICKS(10));
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_DSCR_REG_ADDR, &(dscr.val)), err, TAG, "read DSCR failed");
        if (!bmcr.reset && !dscr.smrst) {
            break;
        }
    }
    ESP_GOTO_ON_FALSE(to < dm9051->phy_802_3.reset_timeout_ms / 10, ESP_FAIL, err, TAG, "PHY reset timeout");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dm9051_init(esp_eth_phy_t *phy)
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
    ESP_GOTO_ON_FALSE(oui == 0x606E && model == 0x0A, ESP_FAIL, err, TAG, "wrong chip ID");

    return ESP_OK;
err:
    return ret;
}

esp_eth_phy_t *esp_eth_phy_new_dm9051(const eth_phy_config_t *config)
{
    esp_eth_phy_t *ret = NULL;
    phy_dm9051_t *dm9051 = calloc(1, sizeof(phy_dm9051_t));
    ESP_GOTO_ON_FALSE(dm9051, NULL, err, TAG, "calloc dm9051 failed");
    ESP_GOTO_ON_FALSE(esp_eth_phy_802_3_obj_config_init(&dm9051->phy_802_3, config) == ESP_OK,
                        NULL, err, TAG, "configuration initialization of PHY 802.3 failed");

    // redefine functions which need to be customized for sake of dm9051
    dm9051->phy_802_3.parent.init = dm9051_init;
    dm9051->phy_802_3.parent.reset = dm9051_reset;
    dm9051->phy_802_3.parent.get_link = dm9051_get_link;

    return &dm9051->phy_802_3.parent;
err:
    if (dm9051 != NULL) {
        free(dm9051);
    }
    return ret;
}
