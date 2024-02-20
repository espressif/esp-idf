/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_ieee802154.h"
#include "esp_err.h"
#include "esp_phy_init.h"
#include "esp_ieee802154_ack.h"
#include "esp_ieee802154_dev.h"
#include "esp_ieee802154_frame.h"
#include "esp_ieee802154_pib.h"
#include "esp_ieee802154_sec.h"
#include "esp_ieee802154_util.h"
#include "esp_log.h"
#include "esp_coex_i154.h"
#include "hal/ieee802154_ll.h"
#include "hal/ieee802154_common_ll.h"

esp_err_t esp_ieee802154_enable(void)
{
    ieee802154_enable();
    ieee802154_rf_enable();
    esp_btbb_enable();
    return ieee802154_mac_init();
}

esp_err_t esp_ieee802154_disable(void)
{
    esp_btbb_disable();
    ieee802154_rf_disable();
    ieee802154_disable();
    return ieee802154_mac_deinit();
}

uint8_t esp_ieee802154_get_channel(void)
{
    return ieee802154_pib_get_channel();
}

esp_err_t esp_ieee802154_set_channel(uint8_t channel)
{
    ieee802154_pib_set_channel(channel);
    return ESP_OK;
}

int8_t esp_ieee802154_get_txpower(void)
{
    return ieee802154_pib_get_power();
}

esp_err_t esp_ieee802154_set_txpower(int8_t power)
{
    ieee802154_pib_set_power(power);
    return ESP_OK;
}

bool esp_ieee802154_get_promiscuous(void)
{
    return ieee802154_pib_get_promiscuous();
}

esp_err_t esp_ieee802154_set_promiscuous(bool enable)
{
    ieee802154_pib_set_promiscuous(enable);
    ieee802154_pib_set_auto_ack_rx(!enable);
    ieee802154_pib_set_auto_ack_tx(!enable);
    ieee802154_pib_set_enhance_ack_tx(!enable);
    return ESP_OK;
}

int8_t esp_ieee802154_get_cca_threshold(void)
{
    return ieee802154_pib_get_cca_threshold();
}

esp_err_t esp_ieee802154_set_cca_threshold(int8_t cca_threshold)
{
    ieee802154_pib_set_cca_threshold(cca_threshold);
    return ESP_OK;
}

esp_ieee802154_cca_mode_t esp_ieee802154_get_cca_mode(void)
{
    return ieee802154_pib_get_cca_mode();
}

esp_err_t esp_ieee802154_set_cca_mode(esp_ieee802154_cca_mode_t cca_mode)
{
    ieee802154_pib_set_cca_mode(cca_mode);
    return ESP_OK;
}

bool esp_ieee802154_get_auto_ack_tx(void)
{
    return ieee802154_pib_get_auto_ack_tx();
}

esp_err_t esp_ieee802154_set_auto_ack_tx(bool enable)
{
    ieee802154_pib_set_auto_ack_tx(enable);
    return ESP_OK;
}

bool esp_ieee802154_get_auto_ack_rx(void)
{
    return ieee802154_pib_get_auto_ack_rx();
}

esp_err_t esp_ieee802154_set_auto_ack_rx(bool enable)
{
    ieee802154_pib_set_auto_ack_rx(enable);
    return ESP_OK;
}

bool esp_ieee802154_get_coordinator(void)
{
    return ieee802154_pib_get_coordinator();
}

esp_err_t esp_ieee802154_set_coordinator(bool enable)
{
    ieee802154_pib_set_coordinator(enable);
    return ESP_OK;
}

#if CONFIG_IEEE802154_MULTI_PAN_ENABLE

uint16_t esp_ieee802154_get_multipan_panid(esp_ieee802154_multipan_index_t index)
{
    assert(index < ESP_IEEE802154_MULTIPAN_MAX);
    return ieee802154_ll_get_multipan_panid(index);
}

esp_err_t esp_ieee802154_set_multipan_panid(esp_ieee802154_multipan_index_t index, uint16_t panid)
{
    assert(index < ESP_IEEE802154_MULTIPAN_MAX);
    ieee802154_ll_set_multipan_panid(index, panid);
    return ESP_OK;
}

uint16_t esp_ieee802154_get_multipan_short_address(esp_ieee802154_multipan_index_t index)
{
    assert(index < ESP_IEEE802154_MULTIPAN_MAX);
    return ieee802154_ll_get_multipan_short_addr(index);
}

esp_err_t esp_ieee802154_set_multipan_short_address(esp_ieee802154_multipan_index_t index, uint16_t short_address)
{
    assert(index < ESP_IEEE802154_MULTIPAN_MAX);
    ieee802154_ll_set_multipan_short_addr(index, short_address);
    return ESP_OK;
}

esp_err_t esp_ieee802154_get_multipan_extended_address(esp_ieee802154_multipan_index_t index, uint8_t *ext_addr)
{
    assert(index < ESP_IEEE802154_MULTIPAN_MAX);
    ieee802154_ll_get_multipan_ext_addr(index, ext_addr);
    return ESP_OK;
}

esp_err_t esp_ieee802154_set_multipan_extended_address(esp_ieee802154_multipan_index_t index, const uint8_t *ext_addr)
{
    assert(index < ESP_IEEE802154_MULTIPAN_MAX);
    ieee802154_ll_set_multipan_ext_addr(index, ext_addr);
    return ESP_OK;
}

uint8_t esp_ieee802154_get_multipan_enable(void)
{
    return ieee802154_ll_get_multipan_enable_mask();
}

esp_err_t esp_ieee802154_set_multipan_enable(uint8_t mask)
{
    assert(mask < (1 << ESP_IEEE802154_MULTIPAN_MAX));
    ieee802154_ll_set_multipan_enable_mask(mask);
    return ESP_OK;
}

#else

uint16_t esp_ieee802154_get_panid(void)
{
    return ieee802154_ll_get_multipan_panid(ESP_IEEE802154_MULTIPAN_0);
}

esp_err_t esp_ieee802154_set_panid(uint16_t panid)
{
    ieee802154_ll_set_multipan_panid(ESP_IEEE802154_MULTIPAN_0, panid);
    return ESP_OK;
}

uint16_t esp_ieee802154_get_short_address(void)
{
    return ieee802154_ll_get_multipan_short_addr(ESP_IEEE802154_MULTIPAN_0);
}

esp_err_t esp_ieee802154_set_short_address(uint16_t short_address)
{
    ieee802154_ll_set_multipan_short_addr(ESP_IEEE802154_MULTIPAN_0, short_address);
    return ESP_OK;
}

esp_err_t esp_ieee802154_get_extended_address(uint8_t *ext_addr)
{
    ieee802154_ll_get_multipan_ext_addr(ESP_IEEE802154_MULTIPAN_0, ext_addr);
    return ESP_OK;
}

esp_err_t esp_ieee802154_set_extended_address(const uint8_t *ext_addr)
{
    ieee802154_ll_set_multipan_ext_addr(ESP_IEEE802154_MULTIPAN_0, ext_addr);
    return ESP_OK;
}

#endif // CONFIG_IEEE802154_MULTI_PAN_ENABLE

esp_ieee802154_pending_mode_t esp_ieee802154_get_pending_mode(void)
{
    return ieee802154_pib_get_pending_mode();
}

esp_err_t esp_ieee802154_set_pending_mode(esp_ieee802154_pending_mode_t pending_mode)
{
    ieee802154_pib_set_pending_mode(pending_mode);
    return ESP_OK;
}

esp_err_t esp_ieee802154_set_rx_when_idle(bool enable)
{
    ieee802154_pib_set_rx_when_idle(enable);
    return ESP_OK;
}

bool esp_ieee802154_get_rx_when_idle(void)
{
    return ieee802154_pib_get_rx_when_idle();
}

esp_err_t esp_ieee802154_transmit(const uint8_t *frame, bool cca)
{
    return ieee802154_transmit(frame, cca);
}

esp_err_t esp_ieee802154_transmit_at(const uint8_t *frame, bool cca, uint32_t time)
{
    return ieee802154_transmit_at(frame, cca, time);
}

esp_err_t esp_ieee802154_sleep(void)
{
    return ieee802154_sleep();
}

esp_err_t esp_ieee802154_receive(void)
{
    return ieee802154_receive();
}

esp_err_t esp_ieee802154_receive_at(uint32_t time)
{
    return ieee802154_receive_at(time);
}

esp_err_t esp_ieee802154_energy_detect(uint32_t duration)
{
    return ieee802154_energy_detect(duration);
}

esp_err_t esp_ieee802154_cca(void)
{
    return ieee802154_cca();
}

esp_ieee802154_state_t esp_ieee802154_get_state(void)
{
    switch (ieee802154_get_state()) {
    case IEEE802154_STATE_DISABLE:
        return ESP_IEEE802154_RADIO_DISABLE;

    case IEEE802154_STATE_IDLE:
        return ESP_IEEE802154_RADIO_IDLE;

    case IEEE802154_STATE_SLEEP:
        return ESP_IEEE802154_RADIO_SLEEP;

    case IEEE802154_STATE_RX:
    case IEEE802154_STATE_TX_ACK:
    case IEEE802154_STATE_ED:
        return ESP_IEEE802154_RADIO_RECEIVE;

    case IEEE802154_STATE_TX_CCA:
    case IEEE802154_STATE_CCA:
    case IEEE802154_STATE_TX:
    case IEEE802154_STATE_RX_ACK:
        return ESP_IEEE802154_RADIO_TRANSMIT;

    default:
        assert(false);
        return ESP_IEEE802154_RADIO_DISABLE;
    }
}

esp_err_t esp_ieee802154_set_transmit_security(uint8_t *frame, uint8_t *key, uint8_t *addr)
{
    ieee802154_transmit_security_config(frame, key, addr);
    return ESP_OK;
}

esp_err_t esp_ieee802154_add_pending_addr(const uint8_t *addr, bool is_short)
{
    return ieee802154_add_pending_addr(addr, is_short);
}

esp_err_t esp_ieee802154_clear_pending_addr(const uint8_t *addr, bool is_short)
{
    return ieee802154_clear_pending_addr(addr, is_short);
}

esp_err_t esp_ieee802154_reset_pending_table(bool is_short)
{
    ieee802154_reset_pending_table(is_short);
    return ESP_OK;
}

int8_t esp_ieee802154_get_recent_rssi(void)
{
    return ieee802154_get_recent_rssi();
}

uint8_t esp_ieee802154_get_recent_lqi(void)
{
    return ieee802154_get_recent_lqi();
}

esp_err_t esp_ieee802154_receive_handle_done(const uint8_t *frame)
{
    return ieee802154_receive_handle_done(frame);
}

__attribute__((weak)) void esp_ieee802154_receive_done(uint8_t *data, esp_ieee802154_frame_info_t *frame_info)
{

}

__attribute__((weak)) void esp_ieee802154_receive_sfd_done(void)
{

}

__attribute__((weak)) void esp_ieee802154_receive_failed(uint16_t error)
{

}

__attribute__((weak)) void esp_ieee802154_transmit_done(const uint8_t *frame, const uint8_t *ack, esp_ieee802154_frame_info_t *ack_frame_info)
{

}

__attribute__((weak)) void esp_ieee802154_transmit_failed(const uint8_t *frame, esp_ieee802154_tx_error_t error)
{

}

__attribute__((weak)) void esp_ieee802154_transmit_sfd_done(uint8_t *frame)
{

}

__attribute__((weak)) void esp_ieee802154_cca_done(bool channel_free)
{

}

__attribute__((weak)) void esp_ieee802154_energy_detect_done(int8_t power)
{

}

__attribute__((weak)) void esp_ieee802154_ed_failed(uint16_t error)
{

}

__attribute__((weak)) esp_err_t esp_ieee802154_enh_ack_generator(uint8_t *frame, esp_ieee802154_frame_info_t *frame_info, uint8_t* enhack_frame)
{
    return ESP_OK;
}

__attribute__((weak)) void esp_ieee802154_timer0_done(void)
{

}

__attribute__((weak)) void esp_ieee802154_timer1_done(void)
{

}

#if CONFIG_IEEE802154_TXRX_STATISTIC
void esp_ieee802154_txrx_statistic_clear(void)
{
    ieee802154_txrx_statistic_clear();
}

void esp_ieee802154_txrx_statistic_print(void)
{
    ieee802154_txrx_statistic_print();
}
#endif // CONFIG_IEEE802154_TXRX_STATISTIC
