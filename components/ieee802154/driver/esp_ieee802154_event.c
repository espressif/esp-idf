/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_check.h"
#include "esp_ieee802154_dev.h"
#include "esp_ieee802154_event.h"
#include "esp_ieee802154_types.h"
#include "esp_ieee802154_util.h"

static esp_ieee802154_event_cb_list_t s_event_cb_list;

esp_err_t ieee802154_event_callback_list_register(esp_ieee802154_event_cb_list_t cb_list)
{
    ESP_RETURN_ON_FALSE(!ieee802154_mac_is_inited(), ESP_FAIL, IEEE802154_TAG,
        "The event callback register function should be called when the IEEE 802.15.4 subsystem is disabled");
    memcpy((void*)(&s_event_cb_list), (void*)(&cb_list), sizeof(esp_ieee802154_event_cb_list_t));
    return ESP_OK;
}
esp_err_t ieee802154_event_callback_list_unregister(void)
{
    ESP_RETURN_ON_FALSE(!ieee802154_mac_is_inited(), ESP_FAIL, IEEE802154_TAG,
        "The event callback unregister function should be called when the IEEE 802.15.4 subsystem is disabled");
    memset((void*)(&s_event_cb_list), 0, sizeof(esp_ieee802154_event_cb_list_t));
    return ESP_OK;
}

void ieee802154_inner_receive_done(uint8_t *data, esp_ieee802154_frame_info_t *frame_info)
{
    if (s_event_cb_list.rx_done_cb) {
        s_event_cb_list.rx_done_cb(data, frame_info);
    } else {
        esp_ieee802154_receive_done(data, frame_info);
    }
}

void ieee802154_inner_receive_sfd_done(void)
{
    if (s_event_cb_list.rx_sfd_done_cb) {
        s_event_cb_list.rx_sfd_done_cb();
    } else {
        esp_ieee802154_receive_sfd_done();
    }
}

void ieee802154_inner_transmit_done(const uint8_t *frame, const uint8_t *ack, esp_ieee802154_frame_info_t *ack_frame_info)
{
    if (s_event_cb_list.tx_done_cb) {
        s_event_cb_list.tx_done_cb(frame, ack, ack_frame_info);
    } else {
        esp_ieee802154_transmit_done(frame, ack, ack_frame_info);
    }
}

void ieee802154_inner_transmit_failed(const uint8_t *frame, esp_ieee802154_tx_error_t error)
{
    if (s_event_cb_list.tx_failed_cb) {
        s_event_cb_list.tx_failed_cb(frame, error);
    } else {
        esp_ieee802154_transmit_failed(frame, error);
    }
}

void ieee802154_inner_transmit_sfd_done(uint8_t *frame)
{
    if (s_event_cb_list.tx_sfd_done_cb) {
        s_event_cb_list.tx_sfd_done_cb(frame);
    } else {
        esp_ieee802154_transmit_sfd_done(frame);
    }
}

void ieee802154_inner_energy_detect_done(int8_t power)
{
    if (s_event_cb_list.ed_done_cb) {
        s_event_cb_list.ed_done_cb(power);
    } else {
        esp_ieee802154_energy_detect_done(power);
    }
}

esp_err_t ieee802154_inner_enh_ack_generator(uint8_t *frame, esp_ieee802154_frame_info_t *frame_info, uint8_t* enhack_frame)
{
    esp_err_t err = ESP_FAIL;
    if (s_event_cb_list.enh_ack_generator_cb) {
        err = s_event_cb_list.enh_ack_generator_cb(frame, frame_info, enhack_frame);
    } else {
        err = esp_ieee802154_enh_ack_generator(frame, frame_info, enhack_frame);
    }
    return err;
}
