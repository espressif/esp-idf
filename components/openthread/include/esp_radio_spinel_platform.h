/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Get frame counter.
 *
 * @param[in]  idx   The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - The frame counter
 *
 */
uint32_t esp_radio_spinel_extern_get_frame_counter(esp_radio_spinel_idx_t idx);

#ifdef __cplusplus
}
#endif
