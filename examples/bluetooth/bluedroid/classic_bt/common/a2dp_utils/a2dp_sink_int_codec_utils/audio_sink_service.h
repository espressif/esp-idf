/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __AUDIO_SINK_SERVICE_H__
#define __AUDIO_SINK_SERVICE_H__

#include <stdint.h>
#include <string.h>
#include "esp_a2dp_api.h"

/**
 * @brief  open audio sink service
 */
void audio_sink_srv_open(void);

/**
 * @brief  close audio sink service
 */
void audio_sink_srv_close(void);

/**
 * @brief  start audio sink service
 */
void audio_sink_srv_start(void);

/**
 * @brief  stop audio sink service
 */
void audio_sink_srv_stop(void);

/**
 * @brief  update codec information
 *
 * @param [in] mcc  codec information
 */
void audio_sink_srv_codec_info_update(esp_a2d_mcc_t *mcc);

/**
 * @brief  write data to ringbuffer
 *
 * @param [in] data  pointer to data stream
 * @param [in] size  data length in byte
 *
 * @return size if written ringbuffer successfully, 0 others
 */
size_t audio_sink_srv_data_output(const uint8_t *data, size_t size);

#endif /* __AUDIO_SINK_SERVICE_H__ */
