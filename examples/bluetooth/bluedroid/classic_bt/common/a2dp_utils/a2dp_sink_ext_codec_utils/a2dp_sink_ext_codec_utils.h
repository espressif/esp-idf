/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __A2DP_SINK_EXT_CODEC_UTILS_H__
#define __A2DP_SINK_EXT_CODEC_UTILS_H__

#include <stdint.h>
#include "esp_a2dp_api.h"

/**
 * @brief  handle function for A2DP sink external codec
 *
 * @param [in] event  event id
 * @param [in] param  callback parameter
 */
void bt_a2d_evt_ext_codec_hdl(uint16_t event, void *param);

/**
 * @brief  handle function for A2DP sink undecoded audio data
 *
 * @param [in]  conn_hdl  connection handle
 * @param [in]  audio_buf pointer to audio buff
 */
void bt_a2d_audio_data_hdl(esp_a2d_conn_hdl_t conn_hdl, esp_a2d_audio_buff_t *audio_buf);

#endif /* __A2DP_SINK_EXT_CODEC_UTILS_H__*/
