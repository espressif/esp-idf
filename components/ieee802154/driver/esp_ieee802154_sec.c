/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_err.h"
#include "hal/ieee802154_ll.h"
#include "esp_ieee802154_frame.h"
#include "esp_ieee802154_sec.h"

#include "esp_ieee802154.h"
static bool s_is_security = false;

void ieee802154_transmit_security_config(uint8_t *frame, uint8_t *key, uint8_t *addr)
{
    ieee802154_ll_set_security_addr(addr);
    ieee802154_ll_set_security_key(key);
    ieee802154_ll_set_security_offset(ieee802154_frame_get_security_payload_offset(frame));
    s_is_security = true;
}

void ieee802154_sec_update(void)
{
    ieee802154_ll_set_transmit_security(s_is_security);
    s_is_security = false;
}
