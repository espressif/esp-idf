/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <stdint.h>

#include "esp_err.h"
#include "esp_ieee802154_ack.h"
#include "esp_ieee802154_pib.h"
#include "esp_ieee802154_util.h"

static uint8_t s_rx_when_idle_mask = 0;

uint16_t esp_ieee802154_get_multipan_panid(esp_ieee802154_multipan_index_t index)
{
    IEEE802154_ASSERT(index < CONFIG_IEEE802154_INTERFACE_NUM);
    return ieee802154_ll_get_multipan_panid(index);
}

esp_err_t esp_ieee802154_set_multipan_panid(esp_ieee802154_multipan_index_t index, uint16_t panid)
{
    IEEE802154_ASSERT(index < CONFIG_IEEE802154_INTERFACE_NUM);
    ieee802154_ll_set_multipan_panid(index, panid);
    return ESP_OK;
}

uint16_t esp_ieee802154_get_multipan_short_address(esp_ieee802154_multipan_index_t index)
{
    IEEE802154_ASSERT(index < CONFIG_IEEE802154_INTERFACE_NUM);
    return ieee802154_ll_get_multipan_short_addr(index);
}

esp_err_t esp_ieee802154_set_multipan_short_address(esp_ieee802154_multipan_index_t index, uint16_t short_address)
{
    IEEE802154_ASSERT(index < CONFIG_IEEE802154_INTERFACE_NUM);
    ieee802154_ll_set_multipan_short_addr(index, short_address);
    return ESP_OK;
}

esp_err_t esp_ieee802154_get_multipan_extended_address(esp_ieee802154_multipan_index_t index, uint8_t *ext_addr)
{
    IEEE802154_ASSERT(index < CONFIG_IEEE802154_INTERFACE_NUM);
    IEEE802154_ASSERT(ext_addr != NULL);
    ieee802154_ll_get_multipan_ext_addr(index, ext_addr);
    return ESP_OK;
}

esp_err_t esp_ieee802154_set_multipan_extended_address(esp_ieee802154_multipan_index_t index, const uint8_t *ext_addr)
{
    IEEE802154_ASSERT(index < CONFIG_IEEE802154_INTERFACE_NUM);
    IEEE802154_ASSERT(ext_addr != NULL);
    ieee802154_ll_set_multipan_ext_addr(index, ext_addr);
    return ESP_OK;
}

uint8_t esp_ieee802154_get_multipan_enable(void)
{
    return ieee802154_ll_get_multipan_enable_mask();
}

esp_err_t esp_ieee802154_set_multipan_enable(uint8_t mask)
{
    IEEE802154_ASSERT(mask < (1 << CONFIG_IEEE802154_INTERFACE_NUM));
    ieee802154_ll_set_multipan_enable_mask(mask);
    return ESP_OK;
}

esp_ieee802154_pending_mode_t esp_ieee802154_multipan_get_pending_mode(esp_ieee802154_multipan_index_t index)
{
    IEEE802154_ASSERT(index < CONFIG_IEEE802154_INTERFACE_NUM);
    return ieee802154_pib_get_pending_mode(index);
}

esp_err_t esp_ieee802154_multipan_set_pending_mode(esp_ieee802154_multipan_index_t inf_index, esp_ieee802154_pending_mode_t pending_mode)
{
    IEEE802154_ASSERT(inf_index < CONFIG_IEEE802154_INTERFACE_NUM);
    ieee802154_pib_set_pending_mode(inf_index, pending_mode);
    return ESP_OK;
}

esp_err_t esp_ieee802154_multipan_add_pending_addr(esp_ieee802154_multipan_index_t inf_index, const uint8_t *addr, bool is_short)
{
    IEEE802154_ASSERT(inf_index < CONFIG_IEEE802154_INTERFACE_NUM);
    return ieee802154_add_pending_addr(inf_index, addr, is_short);
}

esp_err_t esp_ieee802154_multipan_clear_pending_addr(esp_ieee802154_multipan_index_t inf_index, const uint8_t *addr, bool is_short)
{
    IEEE802154_ASSERT(inf_index < CONFIG_IEEE802154_INTERFACE_NUM);
    return ieee802154_clear_pending_addr(inf_index, addr, is_short);
}

esp_err_t esp_ieee802154_multipan_reset_pending_table(esp_ieee802154_multipan_index_t inf_index, bool is_short)
{
    IEEE802154_ASSERT(inf_index < CONFIG_IEEE802154_INTERFACE_NUM);
    ieee802154_reset_pending_table(inf_index, is_short);
    return ESP_OK;
}

esp_err_t esp_ieee802154_multipan_sleep(int8_t idx)
{
    IEEE802154_ASSERT(idx >= 0 && idx < CONFIG_IEEE802154_INTERFACE_NUM);
    uint8_t enable_mask = ieee802154_ll_get_multipan_enable_mask();
    enable_mask &= ~(1 << idx);
    IEEE802154_ASSERT(enable_mask < (1 << CONFIG_IEEE802154_INTERFACE_NUM));
    ieee802154_ll_set_multipan_enable_mask(enable_mask);
    if (enable_mask == 0) {
        return ieee802154_sleep();
    }
    return ESP_OK;
}

esp_err_t esp_ieee802154_multipan_receive(int8_t idx)
{
    IEEE802154_ASSERT(idx >= 0 && idx < CONFIG_IEEE802154_INTERFACE_NUM);
    uint8_t enable_mask = ieee802154_ll_get_multipan_enable_mask();
    enable_mask |= (1 << idx);
    IEEE802154_ASSERT(enable_mask < (1 << CONFIG_IEEE802154_INTERFACE_NUM));
    ieee802154_ll_set_multipan_enable_mask(enable_mask);
    return ieee802154_receive();
}

esp_err_t esp_ieee802154_multipan_set_rx_when_idle(int8_t idx, bool enable)
{
    IEEE802154_ASSERT(idx >= 0 && idx < CONFIG_IEEE802154_INTERFACE_NUM);
    if (enable) {
        s_rx_when_idle_mask |= (1 << idx);
        ieee802154_pib_set_rx_when_idle(true);
    } else {
        s_rx_when_idle_mask &= ~(1 << idx);
        if (s_rx_when_idle_mask == 0) {
            ieee802154_pib_set_rx_when_idle(false);
        }
    }
    return ESP_OK;
}

// Note: multipan receive_at is not supported yet.
