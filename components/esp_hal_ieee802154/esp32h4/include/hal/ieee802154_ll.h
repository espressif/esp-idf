/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/ieee802154_common_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

// todo: TZ-2155
#define IEEE802154_TXPOWER_VALUE_MAX  10
#define IEEE802154_TXPOWER_VALUE_MIN -14
#define IEEE802154_TXPOWER_INDEX_MIN   0

#define IEEE802154_RSSI_COMPENSATION_VALUE 0

static inline uint32_t ieee802154_ll_get_rx_filter_not_work_cnt(void)
{
    return IEEE802154.debug_sfd_timeout_cnt.rx_filter_not_work_cnt;
}

static inline uint32_t ieee802154_ll_get_rx_preamble_detect_err_cnt(void)
{
    return IEEE802154.debug_crc_error_cnt.rx_preamble_detect_err_cnt;
}

#ifdef __cplusplus
}
#endif
