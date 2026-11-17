/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <string.h>
#include "esp_err.h"
#include "esp_ieee802154_frame.h"
#include "esp_ieee802154_sec.h"
#include "esp_ieee802154_util.h"

#include "esp_ieee802154.h"

esp_err_t ieee802154_transmit_security_config(uint8_t *frame, uint8_t *key, uint8_t *addr)
{
#if !CONFIG_IEEE802154_TEST
    IEEE802154_ASSERT(ieee802154_frame_is_security_enabled(frame));
#endif
    uint8_t security_offset = ieee802154_frame_get_security_payload_offset(frame);
#if !CONFIG_IEEE802154_TEST
    if (security_offset == IEEE802154_FRAME_INVALID_VALUE) {
        return ESP_ERR_INVALID_ARG;
    }
#endif
    ieee802154_ll_set_security_addr(addr);
    ieee802154_ll_set_security_key(key);
    ieee802154_ll_set_security_offset(security_offset);
    ieee802154_ll_set_transmit_security(true);
    return ESP_OK;
}

void ieee802154_sec_clear(void)
{
    ieee802154_ll_set_transmit_security(false);
}
