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

#define KSZ80XX_PHY_ID_MSB (0x22)
#define KSZ80XX_PHY_ID_LSB (0x05)
#define KSZ80XX_PHY_OUI (KSZ80XX_PHY_ID_MSB << 6 | KSZ80XX_PHY_ID_LSB)

#define KSZ80XX_PC1R_REG_ADDR (0x1E)
#define KSZ80XX_PC2R_REG_ADDR (0x1F)

typedef enum
{
    KSZ80XX_MODEL_NUMBER_11 = 0x11,     // KSZ8041
    KSZ80XX_MODEL_NUMBER_13 = 0x13,     // KSZ8041RLNI
    KSZ80XX_MODEL_NUMBER_15 = 0x15,     // KSZ8021/31
    KSZ80XX_MODEL_NUMBER_16 = 0x16,     // KSZ8051/81/91
    KSZ80XX_MODEL_NUMBER_17 = 0x17,     // KSZ8061
    KSZ80XX_MODEL_NUMBER_21 = 0x21,     // KSZ8001
} ksz80xx_model_number_t;

typedef struct
{
    phy_802_3_t phy_802_3;
    uint8_t model_number;
    uint32_t op_mode_reg;
    uint32_t op_mode_offset;
} phy_ksz80xx_t;

static const uint8_t supported_model_numbers[] =
{
    KSZ80XX_MODEL_NUMBER_11,
    KSZ80XX_MODEL_NUMBER_13,
    KSZ80XX_MODEL_NUMBER_15,
    KSZ80XX_MODEL_NUMBER_16,
    KSZ80XX_MODEL_NUMBER_17,
    KSZ80XX_MODEL_NUMBER_21,
};

static const char *model_names[] = {
    "41",           // models with model number 0x11
    "41RLNI",       // models with model number 0x13
    "21/31",        // models with model number 0x15
    "51/81/91",     // models with model number 0x16
    "61",           // models with model number 0x17
    "01",           // models with model number 0x21
};

static const char *TAG = "ksz80xx";

static esp_err_t ksz80xx_update_link_duplex_speed(phy_ksz80xx_t * ksz80xx)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = ksz80xx->phy_802_3.eth;
    uint32_t addr = ksz80xx->phy_802_3.addr;
    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    uint32_t peer_pause_ability = false;
    anlpar_reg_t anlpar;
    bmsr_reg_t bmsr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_ANLPAR_REG_ADDR, &(anlpar.val)), err, TAG, "read ANLPAR failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)), err, TAG, "read BMSR failed");
    eth_link_t link = bmsr.link_status ? ETH_LINK_UP : ETH_LINK_DOWN;
    /* check if link status changed */
    if (ksz80xx->phy_802_3.link_status != link) {
        /* when link up, read negotiation result */
        if (link == ETH_LINK_UP) {
            uint32_t reg_value = 0;
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ksz80xx->op_mode_reg, &reg_value), err, TAG, "read %#04x failed", ksz80xx->op_mode_reg);
            uint8_t op_mode = (reg_value >> ksz80xx->op_mode_offset) & 0x07;
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
        ksz80xx->phy_802_3.link_status = link;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ksz80xx_get_link(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_ksz80xx_t *ksz80xx = __containerof(esp_eth_phy_into_phy_802_3(phy), phy_ksz80xx_t, phy_802_3);
    /* Update information about link, speed, duplex */
    ESP_GOTO_ON_ERROR(ksz80xx_update_link_duplex_speed(ksz80xx), err, TAG, "update link duplex speed failed");
    return ESP_OK;
err:
    return ret;
}

static bool ksz80xx_init_model(phy_ksz80xx_t *ksz80xx)
{
    // set variables for op_mode access
    switch (ksz80xx->model_number) {
    case KSZ80XX_MODEL_NUMBER_21:   // models KSZ8001
    case KSZ80XX_MODEL_NUMBER_11:   // models KSZ8041
    case KSZ80XX_MODEL_NUMBER_13:   // models KSZ8041RLNI
        ksz80xx->op_mode_reg = KSZ80XX_PC2R_REG_ADDR;
        ksz80xx->op_mode_offset = 2; // bits 4:2
        break;

    case KSZ80XX_MODEL_NUMBER_15:   // models KSZ8021/31
    case KSZ80XX_MODEL_NUMBER_16:   // models KSZ8051/81/91
    case KSZ80XX_MODEL_NUMBER_17:   // models KSZ8061
        ksz80xx->op_mode_reg = KSZ80XX_PC1R_REG_ADDR;
        ksz80xx->op_mode_offset = 0; // bits 2:0
        break;

    default:
        return false;
    }
    return true;
}

static esp_err_t ksz80xx_init(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = esp_eth_phy_into_phy_802_3(phy);
    phy_ksz80xx_t *ksz80xx = __containerof(phy_802_3, phy_ksz80xx_t, phy_802_3);
       /* Basic PHY init */
    ESP_GOTO_ON_ERROR(esp_eth_phy_802_3_basic_phy_init(phy_802_3), err, TAG, "failed to init PHY");

    /* Check PHY ID */
    uint32_t oui;
    ESP_GOTO_ON_ERROR(esp_eth_phy_802_3_read_oui(phy_802_3, &oui), err, TAG, "read OUI failed");
    ESP_GOTO_ON_ERROR(esp_eth_phy_802_3_read_manufac_info(phy_802_3, &ksz80xx->model_number, NULL), err, TAG, "read manufacturer's info failed");
    ESP_GOTO_ON_FALSE(oui == KSZ80XX_PHY_OUI, ESP_FAIL, err, TAG, "wrong chip ID");

    const char* supported_model_name = NULL;
    for (size_t i = 0; i < sizeof(supported_model_numbers); i++) {
        if (ksz80xx->model_number == supported_model_numbers[i]) {
            supported_model_name = model_names[i];
            break;
        }
    }
    ESP_GOTO_ON_FALSE(supported_model_name != NULL && ksz80xx_init_model(ksz80xx), ESP_FAIL, err, TAG, "unsupported model number: %#04x", ksz80xx->model_number);
    ESP_LOGI(TAG, "auto detected phy KSZ80%s", supported_model_name);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ksz80xx_set_speed(esp_eth_phy_t *phy, eth_speed_t speed)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = esp_eth_phy_into_phy_802_3(phy);
    esp_eth_mediator_t *eth = phy_802_3->eth;

    /* Check if loopback is enabled, and if so, can it work with proposed speed or not */
    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    if (bmcr.en_loopback) {
        ESP_GOTO_ON_FALSE(speed == ETH_SPEED_100M, ESP_ERR_INVALID_STATE, err, TAG, "Speed must be 100M for loopback operation");
    }

    return esp_eth_phy_802_3_set_speed(phy_802_3, speed);
err:
    return ret;
}


esp_eth_phy_t *esp_eth_phy_new_ksz80xx(const eth_phy_config_t *config)
{
    esp_eth_phy_t *ret = NULL;
    phy_ksz80xx_t *ksz80xx = calloc(1, sizeof(phy_ksz80xx_t));
    ESP_GOTO_ON_FALSE(ksz80xx, NULL, err, TAG, "calloc ksz80xx failed");
    ESP_GOTO_ON_FALSE(esp_eth_phy_802_3_obj_config_init(&ksz80xx->phy_802_3, config) == ESP_OK,
                        NULL, err, TAG, "configuration initialization of PHY 802.3 failed");

    // redefine functions which need to be customized for sake of ksz80xx
    ksz80xx->phy_802_3.parent.init = ksz80xx_init;
    ksz80xx->phy_802_3.parent.get_link = ksz80xx_get_link;
    ksz80xx->phy_802_3.parent.set_speed = ksz80xx_set_speed;

    return &ksz80xx->phy_802_3.parent;
err:
    if (ksz80xx != NULL) {
        free(ksz80xx);
    }
    return ret;
}
