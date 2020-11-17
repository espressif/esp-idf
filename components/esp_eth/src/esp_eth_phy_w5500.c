// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"
#include "w5500.h"

static const char *TAG = "w5500-phy";
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
 * @brief PHYCFGR(PHY Configuration Register)
 *
 */
typedef union {
    struct {
        uint8_t link: 1;   /*!< Link status */
        uint8_t speed: 1;  /*!< Speed status */
        uint8_t duplex: 1; /*!< Duplex status */
        uint8_t opmode: 3; /*!< Operation mode */
        uint8_t opsel: 1;  /*!< Operation select */
        uint8_t reset: 1;  /*!< Reset, when this bit is '0', PHY will get reset */
    };
    uint8_t val;
} phycfg_reg_t;


typedef struct {
    esp_eth_phy_t parent;
    esp_eth_mediator_t *eth;
    int addr;
    uint32_t reset_timeout_ms;
    uint32_t autonego_timeout_ms;
    eth_link_t link_status;
    int reset_gpio_num;
} phy_w5500_t;

static esp_err_t w5500_update_link_duplex_speed(phy_w5500_t *w5500)
{
    esp_eth_mediator_t *eth = w5500->eth;
    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    phycfg_reg_t phycfg;

    PHY_CHECK(eth->phy_reg_read(eth, w5500->addr, W5500_REG_PHYCFGR, (uint32_t *) & (phycfg.val)) == ESP_OK, "read PHYCFG failed", err);
    eth_link_t link = phycfg.link ? ETH_LINK_UP : ETH_LINK_DOWN;
    /* check if link status changed */
    if (w5500->link_status != link) {
        /* when link up, read negotiation result */
        if (link == ETH_LINK_UP) {
            if (phycfg.speed) {
                speed = ETH_SPEED_100M;
            } else {
                speed = ETH_SPEED_10M;
            }
            if (phycfg.duplex) {
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
        w5500->link_status = link;
    }
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t w5500_set_mediator(esp_eth_phy_t *phy, esp_eth_mediator_t *eth)
{
    PHY_CHECK(eth, "can't set mediator to null", err);
    phy_w5500_t *w5500 = __containerof(phy, phy_w5500_t, parent);
    w5500->eth = eth;
    return ESP_OK;
err:
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t w5500_get_link(esp_eth_phy_t *phy)
{
    phy_w5500_t *w5500 = __containerof(phy, phy_w5500_t, parent);
    /* Updata information about link, speed, duplex */
    PHY_CHECK(w5500_update_link_duplex_speed(w5500) == ESP_OK, "update link duplex speed failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t w5500_reset(esp_eth_phy_t *phy)
{
    phy_w5500_t *w5500 = __containerof(phy, phy_w5500_t, parent);
    w5500->link_status = ETH_LINK_DOWN;
    esp_eth_mediator_t *eth = w5500->eth;
    phycfg_reg_t phycfg;
    PHY_CHECK(eth->phy_reg_read(eth, w5500->addr, W5500_REG_PHYCFGR, (uint32_t *) & (phycfg.val)) == ESP_OK, "read PHYCFG failed", err);
    phycfg.reset = 0; // set to '0' will reset internal PHY
    PHY_CHECK(eth->phy_reg_write(eth, w5500->addr, W5500_REG_PHYCFGR, phycfg.val) == ESP_OK, "write PHYCFG failed", err);
    vTaskDelay(pdMS_TO_TICKS(10));
    phycfg.reset = 1; // set to '1' after reset
    PHY_CHECK(eth->phy_reg_write(eth, w5500->addr, W5500_REG_PHYCFGR, phycfg.val) == ESP_OK, "write PHYCFG failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t w5500_reset_hw(esp_eth_phy_t *phy)
{
    phy_w5500_t *w5500 = __containerof(phy, phy_w5500_t, parent);
    // set reset_gpio_num to a negative value can skip hardware reset phy chip
    if (w5500->reset_gpio_num >= 0) {
        esp_rom_gpio_pad_select_gpio(w5500->reset_gpio_num);
        gpio_set_direction(w5500->reset_gpio_num, GPIO_MODE_OUTPUT);
        gpio_set_level(w5500->reset_gpio_num, 0);
        esp_rom_delay_us(100); // insert min input assert time
        gpio_set_level(w5500->reset_gpio_num, 1);
    }
    return ESP_OK;
}

static esp_err_t w5500_negotiate(esp_eth_phy_t *phy)
{
    phy_w5500_t *w5500 = __containerof(phy, phy_w5500_t, parent);
    esp_eth_mediator_t *eth = w5500->eth;

    phycfg_reg_t phycfg;
    PHY_CHECK(eth->phy_reg_read(eth, w5500->addr, W5500_REG_PHYCFGR, (uint32_t *) & (phycfg.val)) == ESP_OK, "read PHYCFG failed", err);
    phycfg.opsel = 1;  // PHY working mode configured by register
    phycfg.opmode = 7; // all capable, auto-negotiation enabled
    PHY_CHECK(eth->phy_reg_write(eth, w5500->addr, W5500_REG_PHYCFGR, phycfg.val) == ESP_OK, "write PHYCFG failed", err);

    /* Update information about link, speed, duplex */
    PHY_CHECK(w5500_update_link_duplex_speed(w5500) == ESP_OK, "update link duplex speed failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t w5500_pwrctl(esp_eth_phy_t *phy, bool enable)
{
    // power control is not supported for W5500 internal PHY
    return ESP_OK;
}

static esp_err_t w5500_set_addr(esp_eth_phy_t *phy, uint32_t addr)
{
    phy_w5500_t *w5500 = __containerof(phy, phy_w5500_t, parent);
    w5500->addr = addr;
    return ESP_OK;
}

static esp_err_t w5500_get_addr(esp_eth_phy_t *phy, uint32_t *addr)
{
    PHY_CHECK(addr, "addr can't be null", err);
    phy_w5500_t *w5500 = __containerof(phy, phy_w5500_t, parent);
    *addr = w5500->addr;
    return ESP_OK;
err:
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t w5500_del(esp_eth_phy_t *phy)
{
    phy_w5500_t *w5500 = __containerof(phy, phy_w5500_t, parent);
    free(w5500);
    return ESP_OK;
}

static esp_err_t w5500_advertise_pause_ability(esp_eth_phy_t *phy, uint32_t ability)
{
    // pause ability advertisement is not supported for W5500 internal PHY
    return ESP_OK;
}

static esp_err_t w5500_init(esp_eth_phy_t *phy)
{
    /* Power on Ethernet PHY */
    PHY_CHECK(w5500_pwrctl(phy, true) == ESP_OK, "power control failed", err);
    /* Reset Ethernet PHY */
    PHY_CHECK(w5500_reset(phy) == ESP_OK, "reset failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t w5500_deinit(esp_eth_phy_t *phy)
{
    /* Power off Ethernet PHY */
    PHY_CHECK(w5500_pwrctl(phy, false) == ESP_OK, "power control failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

esp_eth_phy_t *esp_eth_phy_new_w5500(const eth_phy_config_t *config)
{
    PHY_CHECK(config, "invalid arguments", err);
    phy_w5500_t *w5500 = calloc(1, sizeof(phy_w5500_t));
    PHY_CHECK(w5500, "no mem for PHY instance", err);
    w5500->addr = config->phy_addr;
    w5500->reset_timeout_ms = config->reset_timeout_ms;
    w5500->reset_gpio_num = config->reset_gpio_num;
    w5500->link_status = ETH_LINK_DOWN;
    w5500->autonego_timeout_ms = config->autonego_timeout_ms;
    w5500->parent.reset = w5500_reset;
    w5500->parent.reset_hw = w5500_reset_hw;
    w5500->parent.init = w5500_init;
    w5500->parent.deinit = w5500_deinit;
    w5500->parent.set_mediator = w5500_set_mediator;
    w5500->parent.negotiate = w5500_negotiate;
    w5500->parent.get_link = w5500_get_link;
    w5500->parent.pwrctl = w5500_pwrctl;
    w5500->parent.get_addr = w5500_get_addr;
    w5500->parent.set_addr = w5500_set_addr;
    w5500->parent.advertise_pause_ability = w5500_advertise_pause_ability;
    w5500->parent.del = w5500_del;
    return &(w5500->parent);
err:
    return NULL;
}
