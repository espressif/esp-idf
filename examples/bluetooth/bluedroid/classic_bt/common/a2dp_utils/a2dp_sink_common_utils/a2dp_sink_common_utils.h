/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __A2DP_SINK_COMMON_UTILS_H__
#define __A2DP_SINK_COMMON_UTILS_H__

#include <stdint.h>

/**
 * @brief  default handle function for A2DP sink event
 *
 * @param [in] event  event id
 * @param [in] param  callback parameter
 */
void bt_a2d_evt_def_hdl(uint16_t event, void *param);

#endif /* __A2DP_SINK_COMMON_UTILS_H__*/
