/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __AVRCP_COMMON_UTILS_H__
#define __AVRCP_COMMON_UTILS_H__

#include <stdint.h>
#include "esp_avrc_api.h"

/**
 * @brief  default handle function for AVRCP controller event
 *
 * @param [in] event  event id
 * @param [in] param  callback parameter
 */
void bt_avrc_common_ct_evt_def_hdl(uint16_t event, void *param);

/**
 * @brief  default handle function for AVRCP target event
 *
 * @param [in] event  event id
 * @param [in] param  callback parameter
 */
void bt_avrc_common_tg_evt_def_hdl(uint16_t event, void *param);

/**
 * @brief  AVRCP transaction labels allocation
 *
 * @param [out] tl  transaction label
 *
 * @return transaction label
 */
uint8_t bt_avrc_common_alloc_tl(void);

/**
 * @brief  AVRCP deep copy function
 *
 * @param [in] p_dest   pointer to destination data
 * @param [in] p_src    pointer to source data
 * @param [in] len      data length in byte
 */
void bt_avrc_common_copy_metadata(void *p_dest, void *p_src, int len);

/**
 * @brief  AVRCP controller get notification capabilities command
 */
void bt_avrc_common_ct_get_peer_rn_cap(void);

/**
 * @brief  AVRCP controller set notification capabilities command
 *
 * @param [in] peer_rn_cap_t  register notification capabilities of peer in bits
 */
void bt_avrc_common_ct_set_peer_rn_cap(uint16_t peer_rn_cap_t);

/**
 * @brief  AVRCP controller track changed notification command
 */
void bt_avrc_common_ct_rn_track_changed(void);

/**
 * @brief  AVRCP controller play status changed notification command
 */
void bt_avrc_common_ct_rn_play_status_changed(void);

/**
 * @brief  AVRCP controller play position changed notification command
 */
void bt_avrc_common_ct_rn_play_pos_changed(void);

/**
 * @brief  AVRCP controller notification event handler
 *
 * @param [in] event_id  event id
 * @param [in] event_parameter  event parameter
 */
void bt_avrc_common_ct_notify_evt_handler(uint8_t event_id, esp_avrc_rn_param_t *event_parameter);

#endif /* __AVRCP_COMMON_UTILS_H__ */
