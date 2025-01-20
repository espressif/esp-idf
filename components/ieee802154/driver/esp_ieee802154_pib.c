/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include "hal/ieee802154_ll.h"
#include "esp_check.h"
#include "esp_ieee802154_pib.h"
#include "esp_ieee802154_util.h"

static ieee802154_pib_t s_ieee802154_pib;
static bool is_pending = false;

static inline void set_pending(void)
{
    is_pending = true;
}

static inline void clr_pending(void)
{
    is_pending = false;
}

bool inline ieee802154_pib_is_pending(void)
{
    return is_pending;
}

void ieee802154_pib_init(void)
{
    memset(&s_ieee802154_pib, 0, sizeof(ieee802154_pib_t));
    s_ieee802154_pib.auto_ack_rx = true;
    s_ieee802154_pib.auto_ack_tx = true;
    s_ieee802154_pib.enhance_ack_tx = true;
    s_ieee802154_pib.coordinator = false;
    s_ieee802154_pib.promiscuous = true;
    s_ieee802154_pib.rx_when_idle = false;
    s_ieee802154_pib.channel = 11;
    s_ieee802154_pib.cca_threshold = CONFIG_IEEE802154_CCA_THRESHOLD;
    s_ieee802154_pib.cca_mode = CONFIG_IEEE802154_CCA_MODE;
    memset(&s_ieee802154_pib.power_table, IEEE802154_TXPOWER_VALUE_MAX, sizeof(s_ieee802154_pib.power_table));

    set_pending();
}

IEEE802154_NOINLINE static uint8_t ieee802154_txpower_convert(int8_t txpower)
{
    uint8_t ieee820154_txpower_index = 0;
    if (txpower >= IEEE802154_TXPOWER_VALUE_MAX) {
        ieee820154_txpower_index = 15;
    } else if (txpower <= IEEE802154_TXPOWER_VALUE_MIN) {
        ieee820154_txpower_index = IEEE802154_TXPOWER_INDEX_MIN;
    } else {
        ieee820154_txpower_index = (uint8_t)((txpower - IEEE802154_TXPOWER_VALUE_MIN) / 3) + IEEE802154_TXPOWER_INDEX_MIN;
    }
    return ieee820154_txpower_index;
}

void ieee802154_pib_update(void)
{
    if (ieee802154_pib_is_pending()) {
        ieee802154_ll_set_freq(ieee802154_channel_to_freq(s_ieee802154_pib.channel));
        ieee802154_ll_set_power(ieee802154_txpower_convert(ieee802154_pib_get_power()));

        ieee802154_ll_set_cca_mode(s_ieee802154_pib.cca_mode);
        ieee802154_ll_set_cca_threshold(s_ieee802154_pib.cca_threshold);

        ieee802154_ll_set_tx_auto_ack(s_ieee802154_pib.auto_ack_tx);
        ieee802154_ll_set_rx_auto_ack(s_ieee802154_pib.auto_ack_rx);
        ieee802154_ll_set_tx_enhance_ack(s_ieee802154_pib.enhance_ack_tx);

        ieee802154_ll_set_coordinator(s_ieee802154_pib.coordinator);
        ieee802154_ll_set_promiscuous(s_ieee802154_pib.promiscuous);
        ieee802154_ll_set_pending_mode(s_ieee802154_pib.pending_mode == IEEE802154_AUTO_PENDING_ENHANCED);

        clr_pending();
    }
}

uint8_t ieee802154_pib_get_channel(void)
{
    return s_ieee802154_pib.channel;
}

void ieee802154_pib_set_channel(uint8_t channel)
{
    ESP_RETURN_ON_FALSE(ieee802154_is_valid_channel(channel), , IEEE802154_TAG, "Failed to set channel, reason: Invalid channel: %d", channel);
    if (s_ieee802154_pib.channel != channel) {
        s_ieee802154_pib.channel = channel;
        set_pending();
    }
}

int8_t ieee802154_pib_get_power(void)
{
    int8_t ret_power = 0;
    ieee802154_pib_get_power_with_channel(s_ieee802154_pib.channel, &ret_power);
    return ret_power;
}

void ieee802154_pib_set_power(int8_t power)
{
    ieee802154_pib_set_power_with_channel(s_ieee802154_pib.channel, power);
}

esp_err_t ieee802154_pib_set_power_table(esp_ieee802154_txpower_table_t power_table)
{
    if (memcmp(&s_ieee802154_pib.power_table, &power_table, sizeof(esp_ieee802154_txpower_table_t)) != 0) {
        memcpy((void *)&s_ieee802154_pib.power_table, (void *)&power_table, sizeof(esp_ieee802154_txpower_table_t));
        set_pending();
    }
    return ESP_OK;
}

esp_err_t ieee802154_pib_get_power_table(esp_ieee802154_txpower_table_t *out_power_table)
{
    ESP_RETURN_ON_FALSE(out_power_table != NULL, ESP_ERR_INVALID_ARG, IEEE802154_TAG, "Invalid power table");
    memcpy((void *)out_power_table, (void *)&s_ieee802154_pib.power_table, sizeof(esp_ieee802154_txpower_table_t));
    return ESP_OK;
}

esp_err_t ieee802154_pib_set_power_with_channel(uint8_t channel, int8_t power)
{
    ESP_RETURN_ON_FALSE(ieee802154_is_valid_channel(channel), ESP_ERR_INVALID_ARG, IEEE802154_TAG, "Invalid channel: %d", channel);
    if (s_ieee802154_pib.power_table.channel[channel - IEEE802154_OQPSK_2P4G_CHANNEL_MIN] != power) {
        s_ieee802154_pib.power_table.channel[channel - IEEE802154_OQPSK_2P4G_CHANNEL_MIN] = power;
        set_pending();
    }
    return ESP_OK;
}

esp_err_t ieee802154_pib_get_power_with_channel(uint8_t channel, int8_t *out_power)
{
    ESP_RETURN_ON_FALSE(ieee802154_is_valid_channel(channel), ESP_ERR_INVALID_ARG, IEEE802154_TAG, "Invalid channel: %d", channel);
    ESP_RETURN_ON_FALSE(out_power != NULL, ESP_ERR_INVALID_ARG, IEEE802154_TAG, "The pointer of out_power should not be NULL");
    *out_power = s_ieee802154_pib.power_table.channel[channel - IEEE802154_OQPSK_2P4G_CHANNEL_MIN];
    return ESP_OK;
}

bool ieee802154_pib_get_promiscuous(void)
{
    return s_ieee802154_pib.promiscuous;
}

void ieee802154_pib_set_promiscuous(bool enable)
{
    if (s_ieee802154_pib.promiscuous != enable) {
        s_ieee802154_pib.promiscuous = enable;
        set_pending();
    }
}

int8_t ieee802154_pib_get_cca_threshold(void)
{
    return s_ieee802154_pib.cca_threshold;
}

void ieee802154_pib_set_cca_threshold(int8_t cca_threshold)
{
    if (s_ieee802154_pib.cca_threshold != cca_threshold) {
        s_ieee802154_pib.cca_threshold = cca_threshold;
        set_pending();
    }
}

ieee802154_ll_cca_mode_t ieee802154_pib_get_cca_mode(void)
{
    return s_ieee802154_pib.cca_mode;
}

void ieee802154_pib_set_cca_mode(ieee802154_ll_cca_mode_t cca_mode)
{
    if (s_ieee802154_pib.cca_mode != cca_mode) {
        s_ieee802154_pib.cca_mode = cca_mode;
        set_pending();
    }
}

bool ieee802154_pib_get_auto_ack_tx(void)
{
    return s_ieee802154_pib.auto_ack_tx;
}

void ieee802154_pib_set_auto_ack_tx(bool enable)
{
    if (s_ieee802154_pib.auto_ack_tx != enable) {
        s_ieee802154_pib.auto_ack_tx = enable;
        set_pending();
    }
}

bool ieee802154_pib_get_auto_ack_rx(void)
{
    return s_ieee802154_pib.auto_ack_rx;
}

void ieee802154_pib_set_auto_ack_rx(bool enable)
{
    if (s_ieee802154_pib.auto_ack_rx != enable) {
        s_ieee802154_pib.auto_ack_rx = enable;
        set_pending();
    }
}

bool ieee802154_pib_get_enhance_ack_tx(void)
{
    return s_ieee802154_pib.enhance_ack_tx;
}

void ieee802154_pib_set_enhance_ack_tx(bool enable)
{
    if (s_ieee802154_pib.enhance_ack_tx != enable) {
        s_ieee802154_pib.enhance_ack_tx = enable;
        set_pending();
    }
}

bool ieee802154_pib_get_coordinator(void)
{
    return s_ieee802154_pib.coordinator;
}

void ieee802154_pib_set_coordinator(bool enable)
{
    if (s_ieee802154_pib.coordinator != enable) {
        s_ieee802154_pib.coordinator = enable;
        set_pending();
    }
}

ieee802154_ll_pending_mode_t ieee802154_pib_get_pending_mode(void)
{
    return s_ieee802154_pib.pending_mode;
}

void ieee802154_pib_set_pending_mode(ieee802154_ll_pending_mode_t pending_mode)
{
    if (s_ieee802154_pib.pending_mode != pending_mode) {
        s_ieee802154_pib.pending_mode = pending_mode;
        set_pending();
    }
}

void ieee802154_pib_set_rx_when_idle(bool enable)
{
    s_ieee802154_pib.rx_when_idle = enable;
}

bool ieee802154_pib_get_rx_when_idle(void)
{
    return s_ieee802154_pib.rx_when_idle;
}
