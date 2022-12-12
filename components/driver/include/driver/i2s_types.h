/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief I2S controller port number, the max port number is (SOC_I2S_NUM -1).
 */
typedef enum {
    I2S_NUM_0 = 0,                 /*!< I2S controller port 0 */
#if SOC_I2S_NUM > 1
    I2S_NUM_1 = 1,                 /*!< I2S controller port 1 */
#endif
    I2S_NUM_AUTO,                  /*!< Select whichever port is available */
} i2s_port_t;

/**
 * @brief I2S controller communication mode
 */
typedef enum {
    I2S_COMM_MODE_STD,              /*!< I2S controller using standard communication mode, support philips/MSB/PCM format */
#if SOC_I2S_SUPPORTS_PDM
    I2S_COMM_MODE_PDM,              /*!< I2S controller using PDM communication mode, support PDM output or input */
#endif
#if SOC_I2S_SUPPORTS_TDM
    I2S_COMM_MODE_TDM,              /*!< I2S controller using TDM communication mode, support up to 16 slots per frame */
#endif
    I2S_COMM_MODE_NONE,             /*!< Unspecified I2S controller mode */
} i2s_comm_mode_t;

/**
 * @brief The multiple of mclk to sample rate
 */
typedef enum {
    I2S_MCLK_MULTIPLE_128       = 128,     /*!< mclk = sample_rate * 128 */
    I2S_MCLK_MULTIPLE_256       = 256,     /*!< mclk = sample_rate * 256 */
    I2S_MCLK_MULTIPLE_384       = 384,     /*!< mclk = sample_rate * 384 */
    I2S_MCLK_MULTIPLE_512       = 512,     /*!< mclk = sample_rate * 512 */
} i2s_mclk_multiple_t;

/**
 * @brief Event structure used in I2S event queue
 */
typedef struct {
    void                *data;  /**< The pointer of DMA buffer that just finished sending or receiving for `on_recv` and `on_sent` callback
                                  *  NULL for `on_recv_q_ovf` and `on_send_q_ovf` callback
                                  */
    size_t              size;   /**< The buffer size of DMA buffer when success to send or receive,
                                  *  also the buffer size that dropped when queue overflow.
                                  *  It is related to the dma_frame_num and data_bit_width, typically it is fixed when data_bit_width is not changed.
                                  */
} i2s_event_data_t;

typedef struct i2s_channel_obj_t    *i2s_chan_handle_t; /*!< i2s channel object handle, the control unit of the i2s driver*/

/**
 * @brief I2S event callback
 * @param[in] handle    I2S channel handle, created from `i2s_new_channel()`
 * @param[in] event     I2S event data
 * @param[in] user_ctx  User registered context, passed from `i2s_channel_register_event_callback()`
 *
 * @return Whether a high priority task has been waken up by this callback function
 */
typedef bool (*i2s_isr_callback_t)(i2s_chan_handle_t handle, i2s_event_data_t *event, void *user_ctx);

#ifdef __cplusplus
}
#endif
