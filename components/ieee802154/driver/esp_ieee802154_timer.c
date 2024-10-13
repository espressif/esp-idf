/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "hal/ieee802154_ll.h"
#include "esp_check.h"
#include "esp_ieee802154_timer.h"
#include "esp_ieee802154_util.h"

void ieee802154_timer0_start(void)
{
    ieee802154_ll_set_cmd(IEEE802154_CMD_TIMER0_START);
}

void ieee802154_timer0_stop(void)
{
    ieee802154_ll_set_cmd(IEEE802154_CMD_TIMER0_STOP);
}

esp_err_t ieee802154_timer0_set_threshold(uint32_t value)
{
    ESP_RETURN_ON_FALSE((value < IEEE802154_TIMER0_THRESHOLD), ESP_ERR_INVALID_ARG, IEEE802154_TAG, "invalid timer0 threshold");

    ieee802154_ll_timer0_set_threshold(value);

    return ESP_OK;
}

uint32_t ieee802154_timer0_get_value(void)
{
    return ieee802154_ll_timer0_get_value();
}

void ieee802154_timer1_start(void)
{
    ieee802154_ll_set_cmd(IEEE802154_CMD_TIMER1_START);
}

void ieee802154_timer1_stop(void)
{
    ieee802154_ll_set_cmd(IEEE802154_CMD_TIMER1_STOP);
}

esp_err_t ieee802154_timer1_set_threshold(uint32_t value)
{
    ESP_RETURN_ON_FALSE((value < IEEE802154_TIMER1_THRESHOLD), ESP_ERR_INVALID_ARG, IEEE802154_TAG, "invalid timer1 threshold");

    ieee802154_ll_timer1_set_threshold(value);

    return ESP_OK;
}

uint32_t ieee802154_timer1_get_value(void)
{
    return ieee802154_ll_timer1_get_value();
}
