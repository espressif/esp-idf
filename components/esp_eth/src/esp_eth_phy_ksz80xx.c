/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
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

#define KSZ80XX_PHY_ID_MSB (0x22)
#define KSZ80XX_PHY_ID_LSB (0x05)

#define KSZ80XX_PC1R_REG_ADDR (0x1E)
#define KSZ80XX_PC2R_REG_ADDR (0x1F)

typedef enum
{
    KSZ80XX_MODEL_NUMBER_11 = 0x11,     // KSZ8041
    KSZ80XX_MODEL_NUMBER_15 = 0x15,     // KSZ8021/31
    KSZ80XX_MODEL_NUMBER_16 = 0x16,     // KSZ8051/81/91
    KSZ80XX_MODEL_NUMBER_17 = 0x17,     // KSZ8061
    KSZ80XX_MODEL_NUMBER_21 = 0x21,     // KSZ8001
} ksz80xx_model_number_t;

typedef struct
{
    esp_eth_phy_t parent;
    esp_eth_mediator_t *eth;
    int addr;
    uint32_t reset_timeout_ms;
    uint32_t autonego_timeout_ms;
    eth_link_t link_status;
    int reset_gpio_num;
    uint8_t model_number;
    uint32_t op_mode_reg;
    uint32_t op_mode_offset;
} phy_ksz80xx_t;

static const uint8_t supported_model_numbers[] =
{
    KSZ80XX_MODEL_NUMBER_11,
    KSZ80XX_MODEL_NUMBER_15,
    KSZ80XX_MODEL_NUMBER_16,
    KSZ80XX_MODEL_NUMBER_17,
    KSZ80XX_MODEL_NUMBER_21,
};

static const char *model_names[] = {
    "41",           // models with model number 0x11
    "21/31",        // models with model number 0x15
    "51/81/91",     // models with model number 0x16
    "61",           // models with model number 0x17
    "01",           // models with model number 0x21
};

static const char *TAG = "ksz80xx";

static esp_err_t ksz80xx_update_link_duplex_speed(phy_ksz80xx_t * ksz80xx)
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
            uint32_t reg_value = 0;
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz80xx->addr, ksz80xx->op_mode_reg, &reg_value), err, TAG, "read %#04x failed", ksz80xx->op_mode_reg);
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
static esp_err_t ksz80xx_autonego_ctrl(esp_eth_phy_t *phy, eth_phy_autoneg_cmd_t cmd, bool *autonego_en_stat)
{
    esp_err_t ret = ESP_OK;
    phy_ksz80xx_t *ksz80xx = __containerof(phy, phy_ksz80xx_t, parent);
    esp_eth_mediator_t *eth = ksz80xx->eth;

    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz80xx->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");

    switch (cmd) {
    case ESP_ETH_PHY_AUTONEGO_RESTART:
        ESP_GOTO_ON_FALSE(bmcr.en_auto_nego, ESP_ERR_INVALID_STATE, err, TAG, "auto negotiation is disabled");
        /* in case any link status has changed, let's assume we're in link down status */
        ksz80xx->link_status = ETH_LINK_DOWN;

        bmcr.restart_auto_nego = 1; /* Restart Auto Negotiation */

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
        break;
    case ESP_ETH_PHY_AUTONEGO_DIS:
        if (bmcr.en_auto_nego == 1) {
            bmcr.en_auto_nego = 0;     /* Disable Auto Negotiation */
            ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz80xx->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
            /* read configuration back */
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz80xx->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
            ESP_GOTO_ON_FALSE(bmcr.en_auto_nego == 0, ESP_FAIL, err, TAG, "disable auto-negotiation failed");
        }
        break;
    case ESP_ETH_PHY_AUTONEGO_EN:
        if (bmcr.en_auto_nego == 0) {
            bmcr.en_auto_nego = 1;     /* Enable Auto Negotiation */
            ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz80xx->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
            /* read configuration back */
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz80xx->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
            ESP_GOTO_ON_FALSE(bmcr.en_auto_nego == 1, ESP_FAIL, err, TAG, "enable auto-negotiation failed");
        }
        break;
    case ESP_ETH_PHY_AUTONEGO_G_STAT:
        /* do nothing autonego_en_stat is set at the function end */
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    *autonego_en_stat = bmcr.en_auto_nego;
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

static esp_err_t ksz80xx_set_speed(esp_eth_phy_t *phy, eth_speed_t speed)
{
    esp_err_t ret = ESP_OK;
    phy_ksz80xx_t *ksz80xx = __containerof(phy, phy_ksz80xx_t, parent);
    esp_eth_mediator_t *eth = ksz80xx->eth;
    if (ksz80xx->link_status == ETH_LINK_UP) {
        /* Since the link is going to be reconfigured, consider it down for a while */
        ksz80xx->link_status = ETH_LINK_DOWN;
        /* Indicate to upper stream apps the link is cosidered down */
        ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_LINK, (void *)ksz80xx->link_status), err, TAG, "change link failed");
    }
    /* Set speed */
    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz80xx->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    bmcr.speed_select = speed;
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz80xx->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");

    return ESP_OK;
err:
    return ret;
}

static esp_err_t ksz80xx_set_duplex(esp_eth_phy_t *phy, eth_duplex_t duplex)
{
    esp_err_t ret = ESP_OK;
    phy_ksz80xx_t *ksz80xx = __containerof(phy, phy_ksz80xx_t, parent);
    esp_eth_mediator_t *eth = ksz80xx->eth;

    if (ksz80xx->link_status == ETH_LINK_UP) {
        /* Since the link is going to be reconfigured, consider it down for a while */
        ksz80xx->link_status = ETH_LINK_DOWN;
        /* Indicate to upper stream apps the link is cosidered down */
        ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_LINK, (void *)ksz80xx->link_status), err, TAG, "change link failed");
    }
    /* Set duplex mode */
    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, ksz80xx->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    bmcr.duplex_mode = duplex;
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, ksz80xx->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");

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
    ESP_GOTO_ON_FALSE(id1.oui_msb == KSZ80XX_PHY_ID_MSB && id2.oui_lsb == KSZ80XX_PHY_ID_LSB, ESP_FAIL, err, TAG, "wrong chip ID");
    const char* supported_model_name = NULL;
    ksz80xx->model_number = id2.vendor_model;
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

static esp_err_t ksz80xx_deinit(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    /* Power off Ethernet PHY */
    ESP_GOTO_ON_ERROR(ksz80xx_pwrctl(phy, false), err, TAG, "power control failed");
    return ESP_OK;
err:
    return ret;
}

esp_eth_phy_t *esp_eth_phy_new_ksz80xx(const eth_phy_config_t *config)
{
    esp_eth_phy_t *ret = NULL;
    ESP_GOTO_ON_FALSE(config, NULL, err, TAG, "can't set phy config to null");
    phy_ksz80xx_t *ksz80xx = calloc(1, sizeof(phy_ksz80xx_t));
    ESP_GOTO_ON_FALSE(ksz80xx, NULL, err, TAG, "calloc ksz80xx failed");
    ksz80xx->addr = config->phy_addr;
    ksz80xx->reset_gpio_num = config->reset_gpio_num;
    ksz80xx->reset_timeout_ms = config->reset_timeout_ms;
    ksz80xx->link_status = ETH_LINK_DOWN;
    ksz80xx->autonego_timeout_ms = config->autonego_timeout_ms;
    ksz80xx->parent.reset = ksz80xx_reset;
    ksz80xx->parent.reset_hw = ksz80xx_reset_hw;
    ksz80xx->parent.init = ksz80xx_init;
    ksz80xx->parent.deinit = ksz80xx_deinit;
    ksz80xx->parent.set_mediator = ksz80xx_set_mediator;
    ksz80xx->parent.autonego_ctrl = ksz80xx_autonego_ctrl;
    ksz80xx->parent.get_link = ksz80xx_get_link;
    ksz80xx->parent.pwrctl = ksz80xx_pwrctl;
    ksz80xx->parent.get_addr = ksz80xx_get_addr;
    ksz80xx->parent.set_addr = ksz80xx_set_addr;
    ksz80xx->parent.advertise_pause_ability = ksz80xx_advertise_pause_ability;
    ksz80xx->parent.loopback = ksz80xx_loopback;
    ksz80xx->parent.set_speed = ksz80xx_set_speed;
    ksz80xx->parent.set_duplex = ksz80xx_set_duplex;
    ksz80xx->parent.del = ksz80xx_del;
    return &(ksz80xx->parent);
err:
    return ret;
}
