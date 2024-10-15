/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_eth_phy_802_3.h"

static const char *TAG = "ip101";

#define IP101_PHY_RESET_ASSERTION_TIME_US 10000
#define IP101_PHY_POST_RESET_INIT_TIME_MS 10

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
        uint32_t op_mode : 3;    /* Operation Mode Indicator */
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
    phy_802_3_t phy_802_3;
} phy_ip101_t;

static esp_err_t ip101_page_select(phy_ip101_t *ip101, uint32_t page)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = ip101->phy_802_3.eth;
    pcr_reg_t pcr = {
        .register_page_select = page
    };
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ip101->phy_802_3.addr, ETH_PHY_PCR_REG_ADDR, pcr.val), err, TAG, "write PCR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ip101_update_link_duplex_speed(phy_ip101_t *ip101)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = ip101->phy_802_3.eth;
    uint32_t addr = ip101->phy_802_3.addr;
    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    uint32_t peer_pause_ability = false;
    cssr_reg_t cssr;
    anlpar_reg_t anlpar;

    ESP_GOTO_ON_ERROR(ip101_page_select(ip101, 16), err, TAG, "select page 16 failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_CSSR_REG_ADDR, &(cssr.val)), err, TAG, "read CSSR failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_ANLPAR_REG_ADDR, &(anlpar.val)), err, TAG, "read ANLPAR failed");
    eth_link_t link = cssr.link_up ? ETH_LINK_UP : ETH_LINK_DOWN;
    /* check if link status changed */
    if (ip101->phy_802_3.link_status != link) {
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
        ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_LINK, (void *)link), err, TAG, "change link failed");
        ip101->phy_802_3.link_status = link;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ip101_get_link(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_ip101_t *ip101 = __containerof(esp_eth_phy_into_phy_802_3(phy), phy_ip101_t, phy_802_3);

    /* Update information about link, speed, duplex */
    ESP_GOTO_ON_ERROR(ip101_update_link_duplex_speed(ip101), err, TAG, "update link duplex speed failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ip101_init(esp_eth_phy_t *phy)
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
    ESP_GOTO_ON_FALSE(oui == 0x90C3 && model == 0x5, ESP_FAIL, err, TAG, "wrong chip ID");

    return ESP_OK;
err:
    return ret;
}

esp_eth_phy_t *esp_eth_phy_new_ip101(const eth_phy_config_t *config)
{
    esp_eth_phy_t *ret = NULL;
    phy_ip101_t *ip101 = calloc(1, sizeof(phy_ip101_t));
    ESP_GOTO_ON_FALSE(ip101, NULL, err, TAG, "calloc ip101 failed");
    eth_phy_config_t ip101_config = *config;
    // default chip specific configuration
    if (config->hw_reset_assert_time_us == 0) {
        ip101_config.hw_reset_assert_time_us = IP101_PHY_RESET_ASSERTION_TIME_US;
    }
    if (config->post_hw_reset_delay_ms == 0) {
        ip101_config.post_hw_reset_delay_ms = IP101_PHY_POST_RESET_INIT_TIME_MS;
    }
    ESP_GOTO_ON_FALSE(esp_eth_phy_802_3_obj_config_init(&ip101->phy_802_3, &ip101_config) == ESP_OK,
                        NULL, err, TAG, "configuration initialization of PHY 802.3 failed");

    // redefine functions which need to be customized for sake of IP101
    ip101->phy_802_3.parent.init = ip101_init;
    ip101->phy_802_3.parent.get_link = ip101_get_link;

    return &ip101->phy_802_3.parent;
err:
    if (ip101 != NULL) {
        free(ip101);
    }
    return ret;
}
