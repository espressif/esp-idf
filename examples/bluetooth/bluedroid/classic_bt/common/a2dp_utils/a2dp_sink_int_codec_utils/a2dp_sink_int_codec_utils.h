/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __A2DP_SINK_INT_CODEC_UTILS_H__
#define __A2DP_SINK_INT_CODEC_UTILS_H__

#include <stdint.h>

/**
 * @brief  handle function for A2DP sink internal codec
 *
 * @param [in] event  event id
 * @param [in] param  callback parameter
 */
void bt_a2d_evt_int_codec_hdl(uint16_t event, void *param);

/**
 * @brief  handle function for A2DP sink audio data stream
 *
 * @param [out] data  data stream written by application task
 * @param [in]  len   length of data stream in byte
 */
void bt_a2d_data_hdl(const uint8_t *data, uint32_t len);

#endif /* __A2DP_SINK_INT_CODEC_UTILS_H__*/
