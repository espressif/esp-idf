// Copyright (c) 2021 Vladimir Chistyakov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#include "ksz8851.h"

#include "driver/gpio.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_rom_gpio.h"

#include <stdlib.h>
#include <string.h>

static const char *TAG = "ksz8851snl-phy";
#define PHY_CHECK(a, str, goto_tag, ...)                                      \
    do {                                                                      \
        if (!(a)) {                                                           \
        ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        goto goto_tag;                                                        \
        }                                                                     \
    } while (0)

static esp_err_t ksz8851_update_link_duplex_speed(phy_ksz8851snl_t *ksz8851)
{
    esp_eth_mediator_t *eth = ksz8851->eth;
    eth_speed_t speed       = ETH_SPEED_10M;
    eth_duplex_t duplex     = ETH_DUPLEX_HALF;
    uint32_t status;

    PHY_CHECK(eth->phy_reg_read(eth, ksz8851->addr, KSZ8851_P1SR, &status) == ESP_OK, "P1SR read failed", err);
    eth_link_t link = (status & P1SR_LINK_GOOD) ? ETH_LINK_UP : ETH_LINK_DOWN;
    if (ksz8851->link_status != link) {
        if (link == ETH_LINK_UP) {
            if (status & P1SR_OPERATION_SPEED) {
                speed = ETH_SPEED_100M;
                ESP_LOGD(TAG, "speed 100M");
            } else {
                speed = ETH_SPEED_10M;
                ESP_LOGD(TAG, "speed 10M");
            }
            if (status & P1SR_OPERATION_DUPLEX) {
                duplex = ETH_DUPLEX_FULL;
                ESP_LOGD(TAG, "duplex full");
            } else {
                duplex = ETH_DUPLEX_HALF;
                ESP_LOGD(TAG, "duplex half");
            }
            PHY_CHECK(eth->on_state_changed(eth, ETH_STATE_SPEED, (void *)speed) == ESP_OK, "change speed failed", err);
            PHY_CHECK(eth->on_state_changed(eth, ETH_STATE_DUPLEX, (void *)duplex) == ESP_OK, "change duplex failed", err);
        }
        PHY_CHECK(eth->on_state_changed(eth, ETH_STATE_LINK, (void *)link) == ESP_OK, "change link failed", err);
        ksz8851->link_status = link;
    }
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t phy_ksz8851_set_mediator(esp_eth_phy_t *phy, esp_eth_mediator_t *eth)
{
    PHY_CHECK(eth, "mediator can not be null", err);
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    ksz8851->eth              = eth;
    return ESP_OK;
err:
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t phy_ksz8851_reset(esp_eth_phy_t *phy)
{
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    ksz8851->link_status      = ETH_LINK_DOWN;
    esp_eth_mediator_t *eth   = ksz8851->eth;
    ESP_LOGD(TAG, "soft reset");
    // NOTE(v.chistyakov): PHY_RESET bit is self-clearing
    PHY_CHECK(eth->phy_reg_write(eth, ksz8851->addr, KSZ8851_PHYRR, PHYRR_PHY_RESET) == ESP_OK, "PHYRR write failed",
              err);
    vTaskDelay(pdMS_TO_TICKS(ksz8851->reset_timeout_ms));
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t phy_ksz8851_reset_hw(esp_eth_phy_t *phy)
{
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    // NOTE(v.chistyakov): set reset_gpio_num to a negative value can skip hardware reset phy chip
    if (ksz8851->reset_gpio_num >= 0) {
        ESP_LOGD(TAG, "hard reset");
        esp_rom_gpio_pad_select_gpio(ksz8851->reset_gpio_num);
        gpio_set_direction(ksz8851->reset_gpio_num, GPIO_MODE_OUTPUT);
        gpio_set_level(ksz8851->reset_gpio_num, 0);
        esp_rom_delay_us(ksz8851->reset_timeout_ms * 1000);
        gpio_set_level(ksz8851->reset_gpio_num, 1);
    }
    return ESP_OK;
}

static esp_err_t phy_ksz8851_pwrctl(esp_eth_phy_t *phy, bool enable)
{
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    esp_eth_mediator_t *eth   = ksz8851->eth;
    if (enable) {
        ESP_LOGD(TAG, "normal mode");
        PHY_CHECK(eth->phy_reg_write(eth, ksz8851->addr, KSZ8851_PMECR, PMECR_PME_MODE_POWER_SAVING) == ESP_OK,
                  "PMECR write failed", err);
    } else {
        ESP_LOGD(TAG, "power saving mode");
        PHY_CHECK(eth->phy_reg_write(eth, ksz8851->addr, KSZ8851_PMECR, PMECR_PME_MODE_NORMAL) == ESP_OK,
                  "PMECR write failed", err);
    }
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t phy_ksz8851_init(esp_eth_phy_t *phy)
{
    ESP_LOGD(TAG, "initializing PHY");
    PHY_CHECK(phy_ksz8851_pwrctl(phy, true) == ESP_OK, "power control failed", err);
    PHY_CHECK(phy_ksz8851_reset(phy) == ESP_OK, "reset failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t phy_ksz8851_deinit(esp_eth_phy_t *phy)
{
    ESP_LOGD(TAG, "deinitializing PHY");
    PHY_CHECK(phy_ksz8851_pwrctl(phy, false) == ESP_OK, "power control failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t phy_ksz8851_negotiate(esp_eth_phy_t *phy)
{
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    esp_eth_mediator_t *eth   = ksz8851->eth;
    ESP_LOGD(TAG, "restart negotiation");

    uint32_t control;
    PHY_CHECK(eth->phy_reg_read(eth, ksz8851->addr, KSZ8851_P1CR, &control) == ESP_OK, "P1CR read failed", err);
    PHY_CHECK(eth->phy_reg_write(eth, ksz8851->addr, KSZ8851_P1CR, control | P1CR_RESTART_AN) == ESP_OK,
              "P1CR write failed", err);

    vTaskDelay(pdMS_TO_TICKS(ksz8851->autonego_timeout_ms));
    uint32_t status;
    PHY_CHECK(eth->phy_reg_read(eth, ksz8851->addr, KSZ8851_P1SR, &status) == ESP_OK, "P1SR read failed", err);
    PHY_CHECK(status & P1SR_AN_DONE, "auto-negotiation failed", err);
    PHY_CHECK(eth->phy_reg_write(eth, ksz8851->addr, KSZ8851_P1CR, control) == ESP_OK, "P1CR write failed", err);

    PHY_CHECK(ksz8851_update_link_duplex_speed(ksz8851) == ESP_OK, "update link duplex speed failed", err);
    ESP_LOGD(TAG, "negotiation succeded");
    return ESP_OK;
err:
    ESP_LOGD(TAG, "negotiation failed");
    return ESP_FAIL;
}

static esp_err_t phy_ksz8851_get_link(esp_eth_phy_t *phy)
{
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    return ksz8851_update_link_duplex_speed(ksz8851);
}

static esp_err_t phy_ksz8851_set_addr(esp_eth_phy_t *phy, uint32_t addr)
{
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    ksz8851->addr             = addr;
    ESP_LOGD(TAG, "setting PHY addr to %u", addr);
    return ESP_OK;
}

static esp_err_t phy_ksz8851_get_addr(esp_eth_phy_t *phy, uint32_t *addr)
{
    PHY_CHECK(addr, "addr can not be null", err);
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    *addr                     = ksz8851->addr;
    return ESP_OK;
err:
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t phy_ksz8851_advertise_pause_ability(esp_eth_phy_t *phy, uint32_t ability)
{
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    esp_eth_mediator_t *eth   = ksz8851->eth;

    uint32_t anar;
    PHY_CHECK(eth->phy_reg_read(eth, ksz8851->addr, KSZ8851_P1ANAR, &anar) == ESP_OK, "P1ANAR read failed", err);
    if (ability) {
        PHY_CHECK(eth->phy_reg_write(eth, ksz8851->addr, KSZ8851_P1ANAR, anar | P1ANAR_PAUSE) == ESP_OK,
                  "P1ANAR write failed", err);
        ESP_LOGD(TAG, "start advertising pause ability");
    } else {
        PHY_CHECK(eth->phy_reg_write(eth, ksz8851->addr, KSZ8851_P1ANAR, anar & ~P1ANAR_PAUSE) == ESP_OK,
                  "P1ANAR write failed", err);
        ESP_LOGD(TAG, "stop advertising pause ability");
    }
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t phy_ksz8851_del(esp_eth_phy_t *phy)
{
    ESP_LOGD(TAG, "deleting PHY");
    phy_ksz8851snl_t *ksz8851 = __containerof(phy, phy_ksz8851snl_t, parent);
    free(ksz8851);
    return ESP_OK;
}

esp_eth_phy_t *esp_eth_phy_new_ksz8851snl(const eth_phy_config_t *config)
{
    PHY_CHECK(config, "config can not be null", err);
    phy_ksz8851snl_t *ksz8851 = calloc(1, sizeof(phy_ksz8851snl_t));
    PHY_CHECK(ksz8851, "no mem for PHY instance", err);
    ksz8851->addr                           = config->phy_addr;
    ksz8851->reset_timeout_ms               = config->reset_timeout_ms;
    ksz8851->reset_gpio_num                 = config->reset_gpio_num;
    ksz8851->link_status                    = ETH_LINK_DOWN;
    ksz8851->autonego_timeout_ms            = config->autonego_timeout_ms;
    ksz8851->parent.set_mediator            = phy_ksz8851_set_mediator;
    ksz8851->parent.reset                   = phy_ksz8851_reset;
    ksz8851->parent.reset_hw                = phy_ksz8851_reset_hw;
    ksz8851->parent.init                    = phy_ksz8851_init;
    ksz8851->parent.deinit                  = phy_ksz8851_deinit;
    ksz8851->parent.negotiate               = phy_ksz8851_negotiate;
    ksz8851->parent.get_link                = phy_ksz8851_get_link;
    ksz8851->parent.pwrctl                  = phy_ksz8851_pwrctl;
    ksz8851->parent.set_addr                = phy_ksz8851_set_addr;
    ksz8851->parent.get_addr                = phy_ksz8851_get_addr;
    ksz8851->parent.advertise_pause_ability = phy_ksz8851_advertise_pause_ability;
    ksz8851->parent.del                     = phy_ksz8851_del;
    return &(ksz8851->parent);
err:
    return NULL;
}
