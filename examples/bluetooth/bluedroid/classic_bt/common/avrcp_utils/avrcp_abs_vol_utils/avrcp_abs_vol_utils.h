/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __AVRCP_ABS_VOL_UTILS_H__
#define __AVRCP_ABS_VOL_UTILS_H__

#include <stdint.h>

/**
 * @brief  handle function for AVRCP target absolute volume control event
 *
 * @param [in] event  event id
 * @param [in] param  callback parameter
 */
void bt_avrc_avc_tg_evt_hdl(uint16_t event, void *param);

#endif /* __AVRCP_ABS_VOL_UTILS_H__ */
