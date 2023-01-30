/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include "esp_log.h"
#include "esp_eth.h"
#include "eth_phy_802_3_regs.h"
#include "esp_eth_enc28j60.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

static const char *TAG = "enc28j60";
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
 * @brief PHCON2(PHY Control Register 2)
 *
 */
typedef union {
    struct {
        uint32_t reserved_7_0 : 8;  // Reserved
        uint32_t pdpxmd : 1;        // PHY Duplex Mode bit
        uint32_t reserved_10_9: 2;  // Reserved
        uint32_t ppwrsv: 1;         // PHY Power-Down bit
        uint32_t reserved_13_12: 2; // Reserved
        uint32_t ploopbk: 1;        // PHY Loopback bit
        uint32_t prst: 1;           // PHY Software Reset bit
    };
    uint32_t val;
} phcon1_reg_t;
#define ETH_PHY_PHCON1_REG_ADDR (0x00)

/**
 * @brief PHCON2(PHY Control Register 2)
 *
 */
typedef union {
    struct {
        uint32_t reserved_7_0 : 8;  // Reserved
        uint32_t hdldis : 1;        // Half-Duplex Loopback Disable
        uint32_t reserved_9: 1;     // Reserved
        uint32_t jabber: 1;         // Disable Jabber Correction
        uint32_t reserved_12_11: 2; // Reserved
        uint32_t txdis: 1;          // Disable Twist-Pair Transmitter
        uint32_t frclnk: 1;         // Force Linkup
        uint32_t reserved_15: 1;    //Reserved
    };
    uint32_t val;
} phcon2_reg_t;
#define ETH_PHY_PHCON2_REG_ADDR (0x10)

/**
 * @brief PHSTAT2(PHY Status Register 2)
 *
 */
typedef union {
    struct {
        uint32_t reserved_4_0 : 5;   // Reserved
        uint32_t plrity : 1;         // Polarity Status
        uint32_t reserved_8_6 : 3;   // Reserved
        uint32_t dpxstat : 1;        // PHY Duplex Status
        uint32_t lstat : 1;          // PHY Link Status (non-latching)
        uint32_t colstat : 1;        // PHY Collision Status
        uint32_t rxstat : 1;         // PHY Receive Status
        uint32_t txstat : 1;         // PHY Transmit Status
        uint32_t reserved_15_14 : 2; // Reserved
    };
    uint32_t val;
} phstat2_reg_t;
#define ETH_PHY_PHSTAT2_REG_ADDR (0x11)

typedef struct {
    esp_eth_phy_t parent;
    esp_eth_mediator_t *eth;
    uint32_t addr;
    uint32_t reset_timeout_ms;
    eth_link_t link_status;
    int reset_gpio_num;
} phy_enc28j60_t;

static esp_err_t enc28j60_update_link_duplex_speed(phy_enc28j60_t *enc28j60)
{
    esp_eth_mediator_t *eth = enc28j60->eth;
    eth_speed_t speed = ETH_SPEED_10M; // enc28j60 speed is fixed to 10Mbps
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    phstat2_reg_t phstat;
    PHY_CHECK(eth->phy_reg_read(eth, enc28j60->addr, ETH_PHY_PHSTAT2_REG_ADDR, &(phstat.val)) == ESP_OK,
              "read PHSTAT2 failed", err);
    eth_link_t link = phstat.lstat ? ETH_LINK_UP : ETH_LINK_DOWN;
    /* check if link status changed */
    if (enc28j60->link_status != link) {
        /* when link up, read result */
        if (link == ETH_LINK_UP) {
            if (phstat.dpxstat) {
                duplex = ETH_DUPLEX_FULL;
            } else {
                duplex = ETH_DUPLEX_HALF;
            }
            PHY_CHECK(eth->on_state_changed(eth, ETH_STATE_SPEED, (void *)speed) == ESP_OK,
                      "change speed failed", err);
            PHY_CHECK(eth->on_state_changed(eth, ETH_STATE_DUPLEX, (void *)duplex) == ESP_OK,
                      "change duplex failed", err);
        }
        PHY_CHECK(eth->on_state_changed(eth, ETH_STATE_LINK, (void *)link) == ESP_OK,
                  "change link failed", err);
        enc28j60->link_status = link;
    }
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t enc28j60_set_mediator(esp_eth_phy_t *phy, esp_eth_mediator_t *eth)
{
    PHY_CHECK(eth, "can't set mediator for enc28j60 to null", err);
    phy_enc28j60_t *enc28j60 = __containerof(phy, phy_enc28j60_t, parent);
    enc28j60->eth = eth;
    return ESP_OK;
err:
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t enc28j60_get_link(esp_eth_phy_t *phy)
{
    phy_enc28j60_t *enc28j60 = __containerof(phy, phy_enc28j60_t, parent);
    /* Updata information about link, speed, duplex */
    PHY_CHECK(enc28j60_update_link_duplex_speed(enc28j60) == ESP_OK, "update link duplex speed failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t enc28j60_reset(esp_eth_phy_t *phy)
{
    phy_enc28j60_t *enc28j60 = __containerof(phy, phy_enc28j60_t, parent);
    enc28j60->link_status = ETH_LINK_DOWN;
    esp_eth_mediator_t *eth = enc28j60->eth;
    bmcr_reg_t bmcr = {.reset = 1};
    PHY_CHECK(eth->phy_reg_write(eth, enc28j60->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val) == ESP_OK,
              "write BMCR failed", err);
    /* Wait for reset complete */
    uint32_t to = 0;
    for (to = 0; to < enc28j60->reset_timeout_ms / 10; to++) {
        vTaskDelay(pdMS_TO_TICKS(10));
        PHY_CHECK(eth->phy_reg_read(eth, enc28j60->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)) == ESP_OK,
                  "read BMCR failed", err);
        if (!bmcr.reset) {
            break;
        }
    }
    PHY_CHECK(to < enc28j60->reset_timeout_ms / 10, "PHY reset timeout", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t enc28j60_reset_hw(esp_eth_phy_t *phy)
{
    phy_enc28j60_t *enc28j60 = __containerof(phy, phy_enc28j60_t, parent);
    // set reset_gpio_num minus zero can skip hardware reset phy chip
    if (enc28j60->reset_gpio_num >= 0) {
        gpio_reset_pin(enc28j60->reset_gpio_num);
        gpio_set_direction(enc28j60->reset_gpio_num, GPIO_MODE_OUTPUT);
        gpio_set_level(enc28j60->reset_gpio_num, 0);
        gpio_set_level(enc28j60->reset_gpio_num, 1);
    }
    return ESP_OK;
}

static esp_err_t enc28j60_autonego_ctrl(esp_eth_phy_t *phy, eth_phy_autoneg_cmd_t cmd, bool *autoneg_en_stat)
{
    /**
     * ENC28J60 does not support automatic duplex negotiation.
     * If it is connected to an automatic duplex negotiation enabled network switch,
     * ENC28J60 will be detected as a half-duplex device.
     * To communicate in Full-Duplex mode, ENC28J60 and the remote node
     * must be manually configured for full-duplex operation.
     */

    switch (cmd)
    {
    case ESP_ETH_PHY_AUTONEGO_RESTART:
        /* Fallthrough */
    case ESP_ETH_PHY_AUTONEGO_EN:
        return ESP_ERR_NOT_SUPPORTED;
    case ESP_ETH_PHY_AUTONEGO_DIS:
        /* Fallthrough */
    case ESP_ETH_PHY_AUTONEGO_G_STAT:
        *autoneg_en_stat = false;
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t enc28j60_set_speed(esp_eth_phy_t *phy, eth_speed_t speed)
{
    /* ENC28J60 supports only 10Mbps */
    if (speed == ETH_SPEED_10M) {
        return ESP_OK;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t enc28j60_set_duplex(esp_eth_phy_t *phy, eth_duplex_t duplex)
{
    phy_enc28j60_t *enc28j60 = __containerof(phy, phy_enc28j60_t, parent);
    esp_eth_mediator_t *eth = enc28j60->eth;
    phcon1_reg_t phcon1;

    /* Since the link is going to be reconfigured, consider it down to be status updated once the driver re-started */
    enc28j60->link_status = ETH_LINK_DOWN;

    PHY_CHECK(eth->phy_reg_read(eth, enc28j60->addr, 0, &phcon1.val) == ESP_OK,
              "read PHCON1 failed", err);
    switch (duplex) {
    case ETH_DUPLEX_HALF:
        phcon1.pdpxmd = 0;
        break;
    case ETH_DUPLEX_FULL:
        phcon1.pdpxmd = 1;
        break;
    default:
        PHY_CHECK(false, "unknown duplex", err);
        break;
    }

    PHY_CHECK(eth->phy_reg_write(eth, enc28j60->addr, 0, phcon1.val) == ESP_OK,
              "write PHCON1 failed", err);

    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t enc28j60_pwrctl(esp_eth_phy_t *phy, bool enable)
{
    phy_enc28j60_t *enc28j60 = __containerof(phy, phy_enc28j60_t, parent);
    esp_eth_mediator_t *eth = enc28j60->eth;
    bmcr_reg_t bmcr;
    PHY_CHECK(eth->phy_reg_read(eth, enc28j60->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)) == ESP_OK,
              "read BMCR failed", err);
    if (!enable) {
        /* Enable IEEE Power Down Mode */
        bmcr.power_down = 1;
    } else {
        /* Disable IEEE Power Down Mode */
        bmcr.power_down = 0;
    }
    PHY_CHECK(eth->phy_reg_write(eth, enc28j60->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val) == ESP_OK,
              "write BMCR failed", err);
    PHY_CHECK(eth->phy_reg_read(eth, enc28j60->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)) == ESP_OK,
              "read BMCR failed", err);
    if (!enable) {
        PHY_CHECK(bmcr.power_down == 1, "power down failed", err);
    } else {
        PHY_CHECK(bmcr.power_down == 0, "power up failed", err);
    }
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t enc28j60_set_addr(esp_eth_phy_t *phy, uint32_t addr)
{
    phy_enc28j60_t *enc28j60 = __containerof(phy, phy_enc28j60_t, parent);
    enc28j60->addr = addr;
    return ESP_OK;
}

static esp_err_t enc28j60_get_addr(esp_eth_phy_t *phy, uint32_t *addr)
{
    PHY_CHECK(addr, "addr can't be null", err);
    phy_enc28j60_t *enc28j60 = __containerof(phy, phy_enc28j60_t, parent);
    *addr = enc28j60->addr;
    return ESP_OK;
err:
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t enc28j60_del(esp_eth_phy_t *phy)
{
    phy_enc28j60_t *enc28j60 = __containerof(phy, phy_enc28j60_t, parent);
    free(enc28j60);
    return ESP_OK;
}

static esp_err_t enc28j60_init(esp_eth_phy_t *phy)
{
    phy_enc28j60_t *enc28j60 = __containerof(phy, phy_enc28j60_t, parent);
    esp_eth_mediator_t *eth = enc28j60->eth;
    /* Power on Ethernet PHY */
    PHY_CHECK(enc28j60_pwrctl(phy, true) == ESP_OK, "power control failed", err);
    /* Reset Ethernet PHY */
    PHY_CHECK(enc28j60_reset(phy) == ESP_OK, "reset failed", err);
    /* Check PHY ID */
    phyidr1_reg_t id1;
    phyidr2_reg_t id2;
    PHY_CHECK(eth->phy_reg_read(eth, enc28j60->addr, ETH_PHY_IDR1_REG_ADDR, &(id1.val)) == ESP_OK,
              "read ID1 failed", err);
    PHY_CHECK(eth->phy_reg_read(eth, enc28j60->addr, ETH_PHY_IDR2_REG_ADDR, &(id2.val)) == ESP_OK,
              "read ID2 failed", err);
    PHY_CHECK(id1.oui_msb == 0x0083 && id2.oui_lsb == 0x05 && id2.vendor_model == 0x00,
              "wrong chip ID", err);
    /* Disable half duplex loopback */
    phcon2_reg_t phcon2;
    PHY_CHECK(eth->phy_reg_read(eth, enc28j60->addr, ETH_PHY_PHCON2_REG_ADDR, &(phcon2.val)) == ESP_OK,
              "read PHCON2 failed", err);
    phcon2.hdldis = 1;
    PHY_CHECK(eth->phy_reg_write(eth, enc28j60->addr, ETH_PHY_PHCON2_REG_ADDR, phcon2.val) == ESP_OK,
              "write PHCON2 failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t enc28j60_deinit(esp_eth_phy_t *phy)
{
    /* Power off Ethernet PHY */
    PHY_CHECK(enc28j60_pwrctl(phy, false) == ESP_OK, "power off Ethernet PHY failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

esp_eth_phy_t *esp_eth_phy_new_enc28j60(const eth_phy_config_t *config)
{
    PHY_CHECK(config, "can't set phy config to null", err);
    phy_enc28j60_t *enc28j60 = calloc(1, sizeof(phy_enc28j60_t));
    PHY_CHECK(enc28j60, "calloc enc28j60 failed", err);
    enc28j60->addr = config->phy_addr; // although PHY addr is meaningless to ENC28J60
    enc28j60->reset_timeout_ms = config->reset_timeout_ms;
    enc28j60->reset_gpio_num = config->reset_gpio_num;
    enc28j60->link_status = ETH_LINK_DOWN;
    enc28j60->parent.reset = enc28j60_reset;
    enc28j60->parent.reset_hw = enc28j60_reset_hw;
    enc28j60->parent.init = enc28j60_init;
    enc28j60->parent.deinit = enc28j60_deinit;
    enc28j60->parent.set_mediator = enc28j60_set_mediator;
    enc28j60->parent.autonego_ctrl = enc28j60_autonego_ctrl;
    enc28j60->parent.get_link = enc28j60_get_link;
    enc28j60->parent.pwrctl = enc28j60_pwrctl;
    enc28j60->parent.get_addr = enc28j60_get_addr;
    enc28j60->parent.set_addr = enc28j60_set_addr;
    enc28j60->parent.set_speed = enc28j60_set_speed;
    enc28j60->parent.set_duplex = enc28j60_set_duplex;
    enc28j60->parent.del = enc28j60_del;
    return &(enc28j60->parent);
err:
    return NULL;
}
