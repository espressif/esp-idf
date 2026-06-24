/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __AVRCP_METADATA_UTILS_H__
#define __AVRCP_METADATA_UTILS_H__

#include <stdint.h>

/**
 * @brief  handle function for AVRCP controller metadata event
 *
 * @param [in] event  event id
 * @param [in] param  callback parameter
 */
void bt_avrc_md_ct_evt_hdl(uint16_t event, void *param);

#endif /* __AVRCP_METADATA_UTILS_H__*/
