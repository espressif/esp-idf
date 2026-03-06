/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/ieee802154_periph.h"

const ieee802154_conn_t ieee802154_periph = {
    .module = PERIPH_IEEE802154_MODULE,
    .irq_id = ETS_ZB_MAC_INTR_SOURCE,
};
